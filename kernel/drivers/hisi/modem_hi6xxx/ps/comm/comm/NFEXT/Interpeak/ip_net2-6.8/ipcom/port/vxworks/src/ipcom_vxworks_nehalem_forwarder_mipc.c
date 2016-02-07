/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *     Document no: @(#) $Name$ $RCSfile$ $Revision$
 *     $Source$
 *     $Author$
 *     $State$ $Locker$
 *
 *     INTERPEAK_COPYRIGHT_STRING
 ******************************************************************************
 */

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 * This is part of the port specific part of the 'ipcom_forwarder'. It is
 * only valid if IPCOM_USE_NEHALEM_FORWARDER is defined (typically in
 * ipcom/config/ipcom_config.h). When IPCOM_USE_NEHALEM_FORWARDER is not
 * defined, the contents of this file is "ifdefed out".
 *
 * This file handles communication over MIPC with the ifproxy driver in
 * the management OS.
 */

/*
 ****************************************************************************
 * 2                    CONFIGURATION
 ****************************************************************************
 */

#include "ipcom_config.h"

#ifdef IPCOM_USE_NEHALEM_FORWARDER

/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#include <ipcom_type.h>
#include <ipcom_cstyle.h>
#include <ipcom_clib.h>
#include <ipcom_pkt.h>
#include <ipcom_sock.h>
#include <ipcom_netlink.h>

#include <vxWorks.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <stdio.h>
#include <taskLib.h>
#include <multios_ipc/mipc.h>
#include <muxLib.h>
#include <endMedia.h>
#include <net/if.h>

#include <ipcom_forwarder.h>
#include <ipcom_ifproxy_proto.h>
#include <ipcom_vxworks_nehalem_forwarder.h>

/*
 *===========================================================================
 *                    External references
 *===========================================================================
 */



/*
 *===========================================================================
 *                    Local defines
 *===========================================================================
 */

/* Define for debug messages */
#define MIPC_VX_NEHALEM_DEBUG

#ifdef MIPC_VX_NEHALEM_DEBUG
#define MIPCFWDBG(fmt, arg...) printf(fmt, ##arg);
#else
#define MIPCFWDBG(fmt, arg...)   while(0) {}
#endif

#define MIPC_NODE_MGMT        0    /* Management node  */
#define MIPC_NODE_CTRLNAE     2    /* Control NAE node */
#define MIPC_NODE_NAD         0    /* Network acceleration daemon (NAD) node */
#define MIPC_PORT_MGMT        30   /* Management port  */
#define MIPC_PORT_NAD         3    /* Network acceleration daemon (NAD) port */
#define MIPC_MAX_BUS          1    /* Maximum MIPC buses supported */
#define MIPC_MAX_PORTS        256  /* Maximum MIPC ports supported */

#define MIPC_CTRL_TX_BUFS     16   /* Transmit buffers for mgmt control socket */
#define MIPC_CTRL_RX_BUFS     128  /* Receive buffers for mgmt control socket */
#define MIPC_DATA_TX_BUFS     4    /* Transmit buffers for mgmt data socket */
#define MIPC_DATA_RX_BUFS     128  /* Receive buffers for mgmt data socket */
#define MIPC_NAD_TX_BUFS      16   /* Transmit buffers for NAD data socket */
#define MIPC_NAD_RX_BUFS      16   /* Receive buffers for NAD data socket */

#if 0
#define PORT_DISCONNECTED     0
#define PORT_CONNECTED        1
#define PORT_PENDING          2
#define PORT_UP               3
#define PORT_DOWN             5
#endif

#if 0
#define IFPROXY_CTRL_DOWN      0x00 /* Tell the data-plane that the
                                       interface is bought down */
#define IFPROXY_CTRL_UP        0x01 /* Tell the data-plane that the
                                       interface is bought up */
#define IFPROXY_CTRL_GET_MAC   0x02 /* Get the MAC address of the real device */

#endif

#define IFPROXY_CTRL_IP        0x03 /* Incoming data for the management
                                       plane */
#define IFPROXY_CTRL_OP        0x04 /* Outgoing data for the control
                                       NAE */

#define IFPROXY_CTRL_LSC       0x05 /* Link status change */
#define IFPROXY_CTRL_NEWPORT   0x06 /* Sent from the control NAE to the
                                       management plane indicating a
                                       new network interface is available */
#define IFPROXY_CTRL_DELPORT   0x07 /* Sent from the control NAE to the
                                       management plane indicating a
                                       network interface is unavailable */
#define IFPROXY_CTRL_IOCTL     0x08 /* Sent from the management plane to the
                                       control NAE indicating miscellaneous
                                       I/O should be performed */
#define IFPROXY_CTRL_CONNECT   0x09
#define IFPROXY_CTRL_RESULT    0xff /* Result of set operation */

#define IFPROXY_CMD_FLAG_REPLY 0x1 /* 0 - request, 1 - reply                */
#define IFPROXY_CMD_FLAG_FINAL 0x2 /* 0 - interim reply, 1 - final reply    */

#define MAC_ADDRESS_LEN        6
#define MAC_ADDRESS_PAD        2
#define ADDRESS_BUF_LEN        MAC_ADDRESS_LEN + MAC_ADDRESS_PAD
#define MAX_POLLING_CORES      64

#define MIPC_CONN_DOWN         0x0
#define MIPC_CONN_UP           0x1

#define NAD_TASK_NAME          "tMipcNadInit"
#define NAD_TASK_PRIO          100
#define NAD_TASK_SIZE          4000
#define IFPROXY_TASK_NAME      "tMipcIfproxyInit"
#define IFPROXY_TASK_PRIO      100
#define IFPROXY_TASK_SIZE      4000

#define IFPROXY_CONNECT_RETRIES 60


/*
 *===========================================================================
 *                    Local types
 *===========================================================================
 */

typedef struct mipc_connection_s
{
/* TODO: we may not need all of these members after initialization time */
    uint8_t  state;                     /* connection state to peer      */
    uint32_t mtu;                       /* max data size on the MIPC bus */
    int      sock;                      /* MIPC socket                   */
    struct   mipc_sockaddr addr;        /* local MIPC address            */
    struct   mipc_sockaddr peer_addr;   /* peer MIPC address             */

} mipc_connection_t;

typedef struct ifproxy_handler_s
{
    uint8_t           status;          /* interface status              */
    uint16_t          port_index;      /* forwarder port index          */
    int               ifindex;         /* management OS interface index */
    ifproxy_ctrl_set_mclist_t
                     *last_mclist;     /* held last SET MCLIST message  */
    MIPC_ZBUFFER      last_mclist_zbuf;            /* ZBUFFER for above */

    mipc_connection_t data_connection; /* MIPC data connection info     */
    mipc_connection_t ctrl_connection; /* MIPC ctrl connection info     */

    PROTO_COOKIE      mux_cookie;

} ifproxy_handler_t;

typedef struct nad_handler_s
{
    uint8_t  status;                    /* interface status         */
    mipc_connection_t connection;       /* MIPC connection info     */
    int listen_sock;                    /* Preserve parent socket to
                                           keep well-known port in use */
} nad_handler_t;

struct ifproxy_cmd_ip {
    void    *pad;
    void    *pkt;
};

struct ifproxy_cmd_op {
    uint32_t res1;
    uint32_t res2;
    void    *pad;
    void    *pkt;
};

struct ifproxy_cmd_ioctl_req {
    int   cookie;               /* Used to identify ioctl/reply pairs */
    int   cmd;                  /* Ioctl code                         */
    void *buffer ;               /* Variable data                      */
};

struct ifproxy_cmd_ioctl_rep {
    int   cookie;               /* Used to identify ioctl/reply pairs */
    int   cmd;                  /* Ioctl code                         */
    int   status;               /* Status of ioctl request            */
    void *buffer;               /* Variable data                      */
};

struct ifproxy_cmd_lsc {
    int media_active;          /* Interface information (speed, duplex, etc) */
    int media_status;          /* Interface status (up, down, etc.)          */
};

struct ifproxy_cmd_newport {
    int max_mtu;               /* Maximum MTU supported by the interface     */
    int mtu;                   /* Current MTU of the interface               */
    char address[ADDRESS_BUF_LEN]; /* MAC address of the interface           */
    int media_active;          /* Interface information (speed, duplex, etc) */
    int media_status;          /* Interface status (up, down, etc.)          */
};

struct ifproxy_cmd_polling_cores {
    int  mgt_index; /* Interface index used by the management OS to
                       identify this network interface in RTNETLINK messages */
    char polling_cores[MAX_POLLING_CORES]; /* Bit map specifying which cores
                                              should poll the physical port for
                                              received packets.               */
};

union ifproxy_cmd_type {
    struct ifproxy_cmd_ip      *ip;
    struct ifproxy_cmd_op      *op;
    struct ifproxy_cmd_lsc     *lsc;
    struct ifproxy_cmd_newport *newport;
    struct ifproxy_cmd_delport *delport;
    struct ifproxy_cmd_ioctl   *ioctl;
    struct ifproxy_cmd_polling_cores *polling_cores;
};

/*
 *===========================================================================
 *                    Local prototypes
 *===========================================================================
 */

IP_STATIC void ipcom_ifproxy_ctrl_set_mclist(ifproxy_handler_t *proxy,
                                             ifproxy_ctrl_msg_t *msg,
                                             MIPC_ZBUFFER *zbuf);
IP_STATIC void ipcom_ifproxy_ctrl_set_mac(ifproxy_handler_t *proxy,
                                          ifproxy_ctrl_msg_t *msg);
IP_STATIC void ipcom_ifproxy_ctrl_down(ifproxy_handler_t *proxy,
                                       ifproxy_ctrl_msg_t *msg);
IP_STATIC void ipcom_ifproxy_ctrl_up(ifproxy_handler_t *proxy,
                                     ifproxy_ctrl_msg_t *msg);
IP_STATIC void ipcom_ifproxy_linkstate_notify(ifproxy_handler_t *proxy);

IP_STATIC void ipcom_forwarder_nad_mipc_handler_init(nad_handler_t *handler,
                                                     int bus_num);

/*
 *===========================================================================
 *                    Local variables
 *===========================================================================
 */

IP_STATIC ifproxy_handler_t port_proxy[MAX_FW_PORTS];

IP_STATIC nad_handler_t the_nad;

IP_STATIC int ifproxy_nettask_sock = -1;

IP_EXTERN void * fw_msg_q;

#if 0
IP_EXTERN int ipcom_forwarder_fw_started;
IP_EXTERN Ip_u32 ipcom_forwarder_seed;
#endif

IP_EXTERN Ipcom_fw_port_info_t * ipcom_fw_port_info;
IP_EXTERN int ipcom_fw_num;
IP_EXTERN cpuset_t  ipcom_forwarder_cpus;

/*
 ****************************************************************************
 *                      LOCAL ROUTINES
 ****************************************************************************
 */


/*
 *===========================================================================
 *                    ipcom_forwarder_mipc_socket_init
 *===========================================================================
 * Context:     forwarder
 * Description: Initialize MIPC socket with receive, transmit bufs
 * Parameters:
 *
 * Note, we keep the default MTU from that of the MIPC node on the MIPC
 * bus; we can't make it any bigger.
 */
IP_STATIC int
ipcom_forwarder_mipc_socket_init(int s, int rxBufs, int txBufs)
{
    int r, t, b;

    if (s < 0)
        return -1;

    r = rxBufs;
    t = txBufs;

    b = 0;

    if ((mipc_setsockopt(s, MIPC_SOL, MIPC_SO_TXBUFS, (void *)&t, sizeof(t)) < 0) ||
        (mipc_setsockopt(s, MIPC_SOL, MIPC_SO_RXBUFS, (void *)&r, sizeof(r)) < 0) ||
        (mipc_setsockopt(s, MIPC_SOL, MIPC_SO_NBIO,   (void *)&b, sizeof(b)) < 0))
    {
        MIPCFWDBG("Error: configuring mipc socket with txbufs: %d rxbufs: %d\n",
                  t,r);
        return -1;
    }

    return 0;
}

/*
 *===========================================================================
 *                    ipcom_forwarder_mipc_addr_init
 *===========================================================================
 * Context:     forwarder
 * Description: Initialize a MIPC address
 * Parameters:
 *
 */
IP_STATIC void
ipcom_forwarder_mipc_addr_init(struct mipc_sockaddr *addr, unsigned short bus_num, unsigned short node_num, unsigned short port_num)
{
    addr->busNum   = bus_num;
    addr->nodeNum  = node_num;
    addr->portNum  = port_num;
    addr->family   = MIPC_AF;

    return;
}

/*
 *===========================================================================
 *                  ipcom_ipc_msgq_express_cb
 *===========================================================================
 * Context:     Control NAE mipc callback context
 * Description: executes IPC message queue jobs, in particular slow path
 *              packet delivery and initialization.
 * Parameters:  sd - the mipc socket descriptor
 *              src_node - sender's source node -- unused
 *              src_port - sender's source port -- unused
 *              data - MIPC express data. Presently unused.
 */
IP_STATIC void
ipcom_ipc_msgq_express_cb(int sd, unsigned short src_node,
                          unsigned short src_port, uint64_t data)
{
    unsigned long handle;
    Ipcom_ipc_msgq_t *msgq;
    Ipcom_fw_offset_t offset;
    Ipcom_fw_offset_t next_offset;
    Ipcom_ipc_msg_t *msg;
    Ipcom_int_key_t key;
    Ipcom_ipc_msgq_cmd_num_t cmd;
    int count = IPCOM_IPC_MSGQ_JOBS_MAX;

    (void)data;
    (void)src_node;
    (void)src_port;

    (void)mipc_getuserhandle(sd, &handle);
    msgq = (Ipcom_ipc_msgq_t *)handle;

    offset = msgq->cached;
    if (offset != 0)
    {
        msgq->cached = 0;
        goto got_cached;
    }

#if 0
    /*
     * We only empty the message list at most once per express data event.
     * This avoids the possibility that if we empty the message list
     * more than once in this routine, it could lead to a forwarder
     * sending multiple (> 2) express data events before we free any of
     * them. In that case, the forwarder core could potentially run out
     * of express data events and stall the message queue permanently.
     *
     * Note that MIPC doesn't free the event that triggered this routine
     * until after this routine returns, which is probably not ideal,
     * but that choice doesn't affect the above analysis.
     */
    while (TRUE)
#endif
    {
        key = ipcom_ipc_spinlock_take (&msgq->u.lock);
        offset = msgq->msg_offset_head;
        msgq->msg_offset_head = 0;
        msgq->msg_offset_tail = 0;
        ipcom_ipc_spinlock_give (&msgq->u.lock, key);

        /*
         * offsets are measured from the address of the Ipcom_ipc_msgq_t itself.
         * An offset of zero means no message is present.
         * The messages are assumed to come from the same (or a virtually
         * contiguous) shared memory block as the queue, so that the offset
         * is the same for the sender and the single receiver, even if the virtual
         * addresses differ.
         *
         * If this does not hold we may need to use another mechanism, e.g.
         * 'offsets' that are actually host physical addresses.
         */
        if (offset == 0)
            return;

    got_cached:

        do
        {
            if (IP_UNLIKELY(--count < 0))
            {
                IPCQ_STATS (msgq->reposts++);
                msgq->cached = offset;
                /*
                 * Repost the event to ourselves so that we don't
                 * completely hog the MIPC event processing job queue.
                 */
                if (mipc_sendexpress64(sd, &msgq->to, msgq->express_data)
                    == MIPC_OK)
                    return;

                /* Hmm, mipc_sendexpress64() failed, just keep running */
                count = IPCOM_IPC_MSGQ_JOBS_MAX;
            }

            msg = (Ipcom_ipc_msg_t *)((char *)msgq + offset);
            cmd = msg->cmd;
            next_offset = msg->offset_next;

            if (IP_LIKELY(cmd < msgq->n_cmds))
            {
                /* Call the function to execute the command */
                msgq->cmds[cmd] (msg);
                IPCQ_STATS (msgq->goodcmds++);
            }
            else
            {
#if 1
                printf ("Bad command %u queue %p msg offset 0x%lx\n",
                        cmd, msgq, offset);
#endif
                msgq->badcmds++;
            }
            offset = next_offset;
        }
        while (offset != 0);
    }
}

/*
 *===========================================================================
 *                    ipcom_forwarder_ifproxy_data_rx_cb
 *===========================================================================
 * Context:     Control NAE mipc callback context
 * Description: MIPC data socket receive callback
 * Parameters:  s - the mipc socket
 *              buf - pointer to the packet data (1st byte ethernet header)
 *              len - size of packet
 *              srcNode - source node number
 *              srcPort - source port number
 *              zbuf - the MIPC_ZBUFFER for the packet
 */
IP_STATIC int ipcom_forwarder_ifproxy_data_rx_cb(int s,
                                                 const unsigned char *buf,
                                                 size_t len,
                                                 unsigned short srcNode,
                                                 unsigned short srcPort,
                                                 MIPC_ZBUFFER zbuf)
{
    unsigned long l;
    ifproxy_handler_t *proxy;
    ifproxy_cmd_hdr_t *hdr;

    /*
     * Grab a pointer to our ifproxy struct. Socket handle should be
     * valid here.
     */
    mipc_getuserhandle(s, &l); /* Grr. MIPC should pass us our own cookie. */

    proxy = (ifproxy_handler_t *)l;

    hdr = (ifproxy_cmd_hdr_t *)(unsigned char *)buf; /* discarding 'const' */

    /* Here we only have to handle the IFPROXY_CMD_PACKET_OUT command */

    /*
     * Ugh: overwrite the forwarder port index since the ifproxy driver
     * currently just sets it to zero.
     */
    hdr->PortIndex = proxy->port_index;

    if (ipcom_fw_packet_send(hdr, zbuf) == 0)
        return MIPC_RX_KEEPBUF;         /* success! */

    /*
     * The return value was -IP_ERRNO_EWOULDBLOCK, indicating TX congestion.
     * For now, just drop the packet.
     * TODO: think about allowing the packet to be queued, and continuing
     * transmission in response to MUX_TX_RESTART callbacks -- or
     * calls to ipcom_fw_packet_done() ?
     */
    return MIPC_RX_RELEASEBUF;
}

/*
 *===========================================================================
 *                    ipcom_forwarder_ifproxy_ctrl_rx_cb
 *===========================================================================
 * Context:     Control NAE mipc callback context
 * Description: MIPC control socket receive callback
 * Parameters:  s - the mipc socket
 *              buf - pointer to the request
 *              len - size of the request
 *              srcNode - source node number
 *              srcPort - source port number
 *              zbuf - the MIPC_ZBUFFER for the packet
 */
IP_STATIC int ipcom_forwarder_ifproxy_ctrl_rx_cb(int s,
                                                 const unsigned char *buf,
                                                 size_t len,
                                                 unsigned short srcNode,
                                                 unsigned short srcPort,
                                                 MIPC_ZBUFFER zbuf)
{
    unsigned long l;
    ifproxy_handler_t *proxy;
    ifproxy_ctrl_msg_t *msg;
    fw_port_t *fw_port;

    /* Grab a pointer to our ifproxy struct. Socket handle should be
     * valid here.
     */
    mipc_getuserhandle(s, &l);

    proxy = (ifproxy_handler_t *)l;

    if (IP_UNLIKELY (len < 1))
    {
        MIPCFWDBG("ctrl sock port %u runt message len %lu\n",
                  proxy->port_index, len);
        return MIPC_RX_RELEASEBUF;
    }

    fw_port = &ipcom_fw.fw_port[proxy->port_index];

    msg = (ifproxy_ctrl_msg_t *)buf;
    switch (msg->cmd)
    {
    case IFPROXY_CTRL_DOWN:
        ipcom_ifproxy_ctrl_down(proxy, msg);
        break;

    case IFPROXY_CTRL_UP:
        ipcom_ifproxy_ctrl_up(proxy, msg);
        break;

    case IFPROXY_CTRL_GET_MAC:
        (void)mipc_send(proxy->ctrl_connection.sock,
                        &fw_port->src_type[0], MAC_ADDRESS_LEN, 0);
        break;

    case IFPROXY_CTRL_SET_MAC:
        ipcom_ifproxy_ctrl_set_mac(proxy, msg);
        break;

    case IFPROXY_CTRL_SET_MCLIST:
        ipcom_ifproxy_ctrl_set_mclist(proxy, msg, zbuf);

        /* The buffer is held by ifproxy_ctrl_set_mclist() */
        return MIPC_RX_KEEPBUF;

    default:
        MIPCFWDBG("ctrl sock port %u unknown command %u\n",
                  proxy->port_index, msg->cmd);
        break;
    }

    return MIPC_RX_RELEASEBUF;
}

#ifdef MIPC_VX_NEHALEM_DEBUG
BOOL ipcom_forwarder_nad_debug = 0;
#endif

/*
 *===========================================================================
 *                    ipcom_ifproxy_nad_wait_task_spawn
 *===========================================================================
 * Context:     init or MIPC message processing
 * Description: Spawn a task to wait for a connection from wrnad
 * Parameters:
 *
 */
IP_STATIC int 
ipcom_ifproxy_nad_wait_task_spawn (int bus_num)
{
    if (taskSpawn(NAD_TASK_NAME, NAD_TASK_PRIO, 0, NAD_TASK_SIZE,
                  (FUNCPTR) ipcom_forwarder_nad_mipc_handler_init,
                  (long)&the_nad, (long)bus_num, 0, 0, 0, 0, 0, 0, 0, 0) < 0)
    {
        MIPCFWDBG("Error: spawning NAD init task\n");
        return -1;
    }
    MIPCFWDBG("NAD init task (re)spawned!\n");
    return 0;
}

/*
 *===========================================================================
 *                    ipcom_forwarder_nad_disconnect_cb
 *===========================================================================
 * Context:     forwarder
 * Description: MIPC_DISCONNECTED_CALLBACK handler. If the NAD disconnects,
 *              spawn a task to wait for it to connect again.
 * Parameters:
 *
 */
IP_STATIC void ipcom_forwarder_nad_disconnect_cb(int s)
{
    nad_handler_t *nad;

#if 0
    unsigned long l;

    /* Grab a pointer to our nad handler struct. Socket handle should be
     * valid here. (Actually, it's not -- see notes in
     * ipcom_forwarder_nad_mipc_handler_init().)
     */
    mipc_getuserhandle(s, &l);
    nad = (nad_handler_t *)l;
#else  /* workaround to avoid race condition -- use &the_nad */
    nad = &the_nad;
#endif

    /* 
     * Don't mipc_close(s) here in the callback, that will cause a hang.
     * The spawned task will close the original child socket.
     */
    (void)ipcom_ifproxy_nad_wait_task_spawn (nad->connection.addr.busNum);
}

/*
 *===========================================================================
 *                    ipcom_forwarder_nad_rx_cb
 *===========================================================================
 * Context:     forwarder
 * Description: MIPC RX CALLBACK receive handler for network
 *              acceleration daemon (NAD) msgs
 * Parameters:
 *
 */
IP_STATIC int ipcom_forwarder_nad_rx_cb(int s,
                                        const unsigned char *buf,
                                        size_t len,
                                        unsigned short srcNode,
                                        unsigned short srcPort,
                                        MIPC_ZBUFFER zbuf)
{
    nad_handler_t *nad;
    struct Ip_nlmsghdr *nlh;
#if 0
    unsigned long l;
#endif

    (void)srcNode;
    (void)srcPort;

#if 0
    /* Grab a pointer to our nad handler struct. Socket handle should be
     * valid here. (Actually, it's not -- see notes in
     * ipcom_forwarder_nad_mipc_handler_init().)
     */
    mipc_getuserhandle(s, &l);
    nad = (nad_handler_t *)l;
#else  /* workaround to avoid race condition -- use &the_nad */
       /* Urk, we don't actually use nad at present! */
    nad = &the_nad;
#endif

    nlh = (struct Ip_nlmsghdr *)buf;  /* buf is always 8-byte aligned */

    if (len < sizeof(struct Ip_nlmsghdr) || len != nlh->nlmsg_len)
    {
        MIPCFWDBG("Error: unknown netlink message received\n");
        return MIPC_RX_RELEASEBUF;
    }

#ifdef MIPC_VX_NEHALEM_DEBUG
    /* TODO: remove this dbg */
    if (ipcom_forwarder_nad_debug)
    {
        MIPCFWDBG("nad msg [len: %d, type: %d, flags: 0x%x, seq: %d, pid: %d]\n",
                  nlh->nlmsg_len, nlh->nlmsg_type, nlh->nlmsg_flags, nlh->nlmsg_seq,
                  nlh->nlmsg_pid);
    }
#endif

    switch (nlh->nlmsg_type)
    {
    case IP_RTM_NEWLINK:
    case IP_RTM_DELLINK:
    case IP_RTM_NEWADDR:
    case IP_RTM_DELADDR:
    case IP_RTM_NEWROUTE:
    case IP_RTM_DELROUTE:
    case IP_RTM_NEWNEIGH:
    case IP_RTM_DELNEIGH:
        /* TODO: handle these more intelligently. We may need a
           FIB on the NAE to do so. */
        ipcom_forwarder_route_cache_invalidate();
        break;
    default:
        break;
    }

    return MIPC_RX_RELEASEBUF;
}

/*
 *===========================================================================
 *                    ipcom_forwarder_ifproxy_cpus_set
 *===========================================================================
 * Context:     initialization (control NAE)
 * Description: calculate the set of forwarder cores and the number of forwarders
 * Parameters:  nports - the number of forwarder ports
 *
 */
IP_STATIC void
ipcom_forwarder_ifproxy_cpus_set(int nports)
{
    int i, j;
    int ncpus;
    int ports_for_cpu;
    int maxcpus = IPCOM_FW_MAX_CORES();
    cpuset_t fw_cpus;

    CPUSET_ZERO(fw_cpus);
    for (i = 0; i < nports; ++i)
    {
        CPUSET_ADD(fw_cpus, ipcom_fw_port_info[i].pollers);
    }

    ncpus = 0;

    for (i = 0; i < maxcpus; ++i)
    {
        if (CPUSET_ISSET(fw_cpus, i))
        {
            ++ncpus;
            ports_for_cpu = 0;
            for (j = 0; j < nports; ++j)
            {
                if (CPUSET_ISSET(ipcom_fw_port_info[j].pollers, i))
                    ++ports_for_cpu;
            }

            if (ports_for_cpu > IPCOM_FW_RX_QUEUES_MAX)
                MIPCFWDBG("ERROR: Forwarder core %d polls too many ports\n", i);

            if (ncpus <= IPCOM_MAX_FORWARDERS)
            {
                CPUSET_SET(ipcom_forwarder_cpus, i);
            }
        }
    }
    if (ncpus > IPCOM_MAX_FORWARDERS)
    {
        MIPCFWDBG("ERROR: Too many configured forwarders (%d > %d)\n",
                   ncpus, IPCOM_MAX_FORWARDERS);
        ncpus = IPCOM_MAX_FORWARDERS;
    }

    ipcom_fw_num = ncpus;

    return;
}

/*
 *===========================================================================
 *                    ipcom_ifproxy_ctrl_set_mac
 *===========================================================================
 * Context:     Control NAE mipc callback context
 * Description: set the MAC address of the physical port
 * Parameters:  proxy - the handler object for the proxy port
 *              msg - the ifproxy SET MAC control message
 */
IP_STATIC void
ipcom_ifproxy_ctrl_set_mac(ifproxy_handler_t *proxy,
                     ifproxy_ctrl_msg_t *msg)
{
    ifproxy_ctrl_set_mac_t *mac_msg = (ifproxy_ctrl_set_mac_t *)msg;
    Ipcom_fw_port_t *fw_port = ipcom_forwarder_get_port_cookie(proxy->port_index);

    MIPCFWDBG("ctrl msg SET MAC port %u\n", proxy->port_index);
    (void)muxIoctl(fw_port->pEnd, EIOCSADDR, (caddr_t)mac_msg->eth_addr);
    ipcom_forwarder_port_mac_update(proxy->port_index, mac_msg->eth_addr);
}

/*
 *===========================================================================
 *                    ipcom_ifproxy_ctrl_mclist_adjust
 *===========================================================================
 * Context:     Control NAE mipc callback context
 * Description: helper function to clear an old multicast receiver
 *              table and set a new one
 * Parameters:  pEnd - the END object of the physical port
 *              new_msg - contains the new multicast table to set (or IP_NULL)
 *              old_msg - contains the old multicast table to unset (or IP_NULL)
 */
IP_STATIC void
ipcom_ifproxy_mclist_adjust(END_OBJ *pEnd,
                            ifproxy_ctrl_set_mclist_t *new_msg,
                            ifproxy_ctrl_set_mclist_t *old_msg)
{
    unsigned int i;
    Ip_u8 *addr;

    /*
     * Add the new MAC addresses before removing the old addresses.
     * That minimizes memory allocation/freeing churn in etherMultiLib.
     */
    if (new_msg != IP_NULL)
    {
        addr = &new_msg->eth_addrs[0];
        for (i = 0; i < new_msg->num_addrs; ++i)
        {
            (void)muxIoctl(pEnd, EIOCMULTIADD, (caddr_t)addr);
            addr += MAC_ADDRESS_LEN;
        }
    }
    if (old_msg != NULL)
    {
        addr = &old_msg->eth_addrs[0];
        for (i = 0; i < old_msg->num_addrs; ++i)
        {
            (void)muxIoctl(pEnd, EIOCMULTIDEL, (caddr_t)addr);
            addr += MAC_ADDRESS_LEN;
        }
    }
}


/*
 *===========================================================================
 *                    ipcom_ifproxy_ctrl_set_mclist
 *===========================================================================
 * Context:     Control NAE mipc callback context
 * Description: set the multicast group receive list for the port
 * Parameters:  proxy - the handler object for the proxy port
 *              msg - the ifproxy SET MCLIST control message
 *              zbuf - the MIPC_ZBUFFER corresponding to msg
 *
 * Note: This routine sets a reference to msg and zbuf in
 * proxy.  If there was previously such a reference to a multicast
 * receiver table, it's old 'msg' and 'zbuf' are freed.
 */
IP_STATIC void
ipcom_ifproxy_ctrl_set_mclist(ifproxy_handler_t *proxy,
                              ifproxy_ctrl_msg_t *msg,
                              MIPC_ZBUFFER *zbuf)
{
    ifproxy_ctrl_set_mclist_t *mc_msg = (ifproxy_ctrl_set_mclist_t *)msg;
    Ipcom_fw_port_t *fw_port = ipcom_forwarder_get_port_cookie(proxy->port_index);

    ipcom_ifproxy_mclist_adjust(fw_port->pEnd, mc_msg, proxy->last_mclist);

    if (proxy->last_mclist_zbuf != NULL)
        mipc_zfree (proxy->ctrl_connection.sock, proxy->last_mclist_zbuf);

    proxy->last_mclist = mc_msg;
    proxy->last_mclist_zbuf = zbuf;

    if (mc_msg->hdr.flags & IFPROXY_CTRL_FLAGS_ALLMULTI)
        (void)muxIoctl(fw_port->pEnd, EIOCSFLAGS, (caddr_t)IFF_ALLMULTI);
    else
        (void)muxIoctl(fw_port->pEnd, EIOCSFLAGS, (caddr_t)~IFF_ALLMULTI);
}


/*
 *===========================================================================
 *                    ipcom_ifproxy_ctrl_down
 *===========================================================================
 * Context:     Control NAE MIPC event processing context
 * Description: Process and ifproxy 'down' message
 * Parameters:  proxy - the handler object for the proxy port
 *              msg - the ifproxy port-is-down message
 *
 * Notes: Currently this routine frees the current multicast receiver
 * table, if one is held. The port is brought out of IFF_ALLMULTI mode
 * if it was in that mode. More actions may be added later.
 */
IP_STATIC void
ipcom_ifproxy_ctrl_down(ifproxy_handler_t *proxy, ifproxy_ctrl_msg_t *msg)
{
    Ipcom_fw_port_t *fw_port = ipcom_forwarder_get_port_cookie(proxy->port_index);

    MIPCFWDBG("ctrl msg port %u is DOWN\n",
              proxy->port_index);

    ipcom_ifproxy_mclist_adjust(fw_port->pEnd, IP_NULL, proxy->last_mclist);

    if (proxy->last_mclist_zbuf != NULL)
        mipc_zfree (proxy->ctrl_connection.sock, proxy->last_mclist_zbuf);

    proxy->last_mclist = IP_NULL;
    proxy->last_mclist_zbuf = NULL;
    (void)muxIoctl(fw_port->pEnd, EIOCSFLAGS, (caddr_t)~IFF_ALLMULTI);

    /* TODO: More ? Tell forwarders to quiesce, get ready for reconfiguration? */
}

/*
 *===========================================================================
 *                    ipcom_ifproxy_ctrl_up
 *===========================================================================
 * Context:     Control NAE MIPC event processing context
 * Description: Process an ifproxy 'up' message
 * Parameters:
 *
 */
IP_STATIC void
ipcom_ifproxy_ctrl_up(ifproxy_handler_t *proxy, ifproxy_ctrl_msg_t *msg)
{
    MIPCFWDBG("ctrl msg port %u is UP\n",
              proxy->port_index);

    ipcom_ifproxy_linkstate_notify(proxy);
    return; /* TODO: anything else? */
}

/*
 *===========================================================================
 *               ipcom_forwarder_alloc_rxq_stats_ix
 *===========================================================================
 * Context:     control NAE
 * Description: For the forwarder NAE specified by <cpuid>, find an unused
 *              index in the forwarder's RX queue statistics array,
 *              for assignment to the next port that the forwarder will poll.
 *              Returns the index, or -1 if all slots are assigned.
 * Parameters:  cpuid - the forwarder's logical CPU number.
 *
 */
IP_STATIC int
ipcom_forwarder_alloc_rxq_stats_ix(Ip_u32 cpuid)
{
    Ip_u32 i, j, ix;

#if IPCOM_FW_RX_QUEUES_MAX > 32
#error "Need a wider mask."
#endif
    Ip_u32 mask = 0;
    Ipcom_fw_port_t *fw_port;

    for (j = 0; j < MAX_FW_PORTS; j++)
    {
	if (!CPUSET_ISSET(ipcom_fw_port_info[j].pollers, cpuid))
	    continue;
	fw_port = ipcom_fw.fw_port[j].fwd;
	for (i = 0; i < fw_port->nRxPollers; i++)
	{
	    if (fw_port->rxqi[i].cpuid != cpuid)
		continue;
	    ix = fw_port->rxqi[i].rxq_stats_ix;

	    if (ix < IPCOM_FW_RX_QUEUES_MAX)
		mask |= (1 << ix);
	}
    }
    for (ix = 0; ix < IPCOM_FW_RX_QUEUES_MAX; ix++)
    {
	if ((mask & (1 << ix)) == 0)
	    return (int)ix;
    }
    return -1;
}


/*
 *===========================================================================
 *                    ipcom_forwarder_ifproxy_connect
 *===========================================================================
 * Context:     forwarder
 * Description: Negotiate connection with ifproxy driver
 * Parameters:
 *
 */
IP_STATIC int
ipcom_forwarder_ifproxy_connect(ifproxy_handler_t *proxy)
{
    int j;
    int ret;
    int len;
    int retries;
    ifproxy_initial_t initreq;
    ifproxy_initresp_t initresp;
    size_t addrlen;
    int maxcores;
    Ipcom_fw_port_t *fw_port;
    int rxq_stats_ix;

    if (proxy == IP_NULL)
        return -1;

    for (retries = 0; retries < IFPROXY_CONNECT_RETRIES; ++retries)
    {
        /* Attempt connect to 'accepting' ifproxy driver */

        ret = mipc_connect(proxy->ctrl_connection.sock,
                           &proxy->ctrl_connection.peer_addr,
                           sizeof(proxy->ctrl_connection.peer_addr));
        if (ret == 0)
            break;
        
        taskDelay(sysClkRateGet());
    }

    if (retries == IFPROXY_CONNECT_RETRIES)
    {
        MIPCFWDBG("ERROR[%d]: Couldn't connect to ifproxy driver\n", ret);
        return -1;
    }

    /* Now get the port of the connected peer child socket, which
       is not the same as that of the parent. */

    addrlen = sizeof (proxy->ctrl_connection.peer_addr);
    ret = mipc_getpeername(proxy->ctrl_connection.sock,
                           &proxy->ctrl_connection.peer_addr,
                           &addrlen);
    if (ret < 0)
    {
        MIPCFWDBG("Error[%d]: obtaining peer address\n", ret);
        return -1;
    }

    MIPCFWDBG("Connected to port: %d\n", proxy->ctrl_connection.peer_addr.portNum);

    /* Set RX callback for data socket. */
    mipc_setsockopt(proxy->data_connection.sock,
                    MIPC_SOL,
                    MIPC_SO_RX_CALLBACK,
                    (void *)ipcom_forwarder_ifproxy_data_rx_cb,
                    sizeof (MIPC_RX_CALLBACK));

    /*
     * Ifproxy driver expects us to send the port for the data socket
     * so that it can connect (DGRAM-style) to it.
     */

    initreq.dataport = proxy->data_connection.addr.portNum;
    initreq.running = 1; /* In response to the UP message, we'll
                            send a link state change notification
                            with the current link state; so just assume
                            the carrier is active here. */
    memcpy (&initreq.hw_addr,
            &ipcom_fw.fw_port[proxy->port_index].src_type[0],
            MAC_ADDRESS_LEN);

    if (mipc_send(proxy->ctrl_connection.sock,
                  &initreq, sizeof(initreq), 0) < 0)
    {
        MIPCFWDBG("Error: sending initial ifproxy connection request!\n");
        return -1;
    }

    /*
     * Ifproxy driver will send back it's data port.  We
     * use this to connect (DGRAM-style) our data socket.
     * The driver will also inform us of any polling cores for this port.
     */

    len = mipc_recv (proxy->ctrl_connection.sock,
                     &initresp, sizeof(initresp), 0);
    if (len != sizeof(initresp))
    {
        MIPCFWDBG("Error: Retrieving ifproxy initial response msg\n");
        return -1;
    }

    proxy->data_connection.peer_addr = proxy->ctrl_connection.peer_addr;
    proxy->data_connection.peer_addr.portNum = initresp.dataport;
    proxy->ifindex = initresp.if_index;

    ret = mipc_connect(proxy->data_connection.sock,
                       &proxy->data_connection.peer_addr,
                       sizeof(proxy->data_connection.peer_addr));
    if (ret < 0)
    {
        MIPCFWDBG("Error[%d]: connecting to data port [%d]\n",
                  ret,
                  proxy->data_connection.peer_addr.portNum);
        return -1;
    }

    fw_port = ipcom_fw.fw_port[proxy->port_index].fwd;
    fw_port->nRxPollers = 0;
    fw_port->ifproxy_addr = proxy->data_connection.peer_addr;

    maxcores = IPCOM_FW_MAX_CORES();
    for (j = 0; j < sizeof(initresp.polling_cores) * CHAR_BIT; j++)
    {
        if ((initresp.polling_cores & (1ULL << j)) == 0)
	    continue;

	if (j >= maxcores)
	{
	    MIPCFWDBG("WARNING: Forwarder CPU id %d > %d\n",
		      j, maxcores);
	    break;
	}
	if (fw_port->nRxPollers >= fw_port->numRxQs)
	{
	    MIPCFWDBG("WARNING: Too many RX pollers for port %d\n",
		      proxy->port_index);
	    break;
	}
	rxq_stats_ix = ipcom_forwarder_alloc_rxq_stats_ix(j);
	if (rxq_stats_ix < 0)
	{
	    MIPCFWDBG("WARNING: Too many RX queues assigned to "
		      "forwarder core %u\n", j);
	    break;
	}
	fw_port->rxqi[fw_port->nRxPollers].cpuid = j;
	fw_port->rxqi[fw_port->nRxPollers].rxq_stats_ix = rxq_stats_ix;
	fw_port->nRxPollers++;

	if (CPUSET_ISSET(ipcom_fw_port_info[proxy->port_index].pollers, j))
	    MIPCFWDBG("WARNING: Forwarder CPU id %d repeated\n",
		      j);

	CPUSET_SET(ipcom_fw_port_info[proxy->port_index].pollers, j);
    }

    /* Set RX callback for control socket. */
    mipc_setsockopt(proxy->ctrl_connection.sock,
                    MIPC_SOL,
                    MIPC_SO_RX_CALLBACK,
                    (void *)ipcom_forwarder_ifproxy_ctrl_rx_cb,
                    sizeof (MIPC_RX_CALLBACK));

    /*
     * Send back a final ACK. We'd like to send a zero-length
     * message, but mipc_send() succeeds without doing anything
     * if one tries to send a message of zero length. So, send one
     * byte.
     */

    if (mipc_send(proxy->ctrl_connection.sock, &initreq, 1, 0) < 0)
    {
        MIPCFWDBG("Error: Acknowledging ifproxy connect!\n");
        return -1;
    }
    MIPCFWDBG("Finished connection for forwarder port index %u\n",
              proxy->port_index);

    return 0;
}

/*
 *===========================================================================
 *                    ipcom_forwarder_nad_mipc_handler_init
 *===========================================================================
 * Context:     forwarder
 * Description: Initialize an network acceleration daemon (NAD) MIPC
 *              connection handler
 * Parameters:
 *
 */
IP_STATIC void
ipcom_forwarder_nad_mipc_handler_init(nad_handler_t *handler, int bus_num)
{
    int ret;
    struct mipc_sockaddr from;
    size_t fromlen = sizeof(from);
    int non_blocking = 0;

    if (handler == IP_NULL)
        return;

    if (handler->connection.sock >= 0)
    {
        taskDelay (sysClkRateGet());
        (void)mipc_close(handler->connection.sock);
        handler->connection.sock = -1;
    }

    /* Create, initialize NAD listening socket on the first run only. */
    if (handler->listen_sock < 0)
    {
        handler->listen_sock = mipc_socket (MIPC_AF, MIPC_SOCK_SEQPACKET, 0);
        if (handler->listen_sock < 0)
        {
            MIPCFWDBG("Error: couldn't create ctrl socket\n");
            return;
        }

        ret = ipcom_forwarder_mipc_socket_init(handler->listen_sock,
                                               MIPC_CTRL_RX_BUFS,
                                               MIPC_CTRL_TX_BUFS);
        if (ret < 0)
        {
            MIPCFWDBG("Error: couldn't initialize ctrl socket\n");
close_listen_sock:
            (void)mipc_close(handler->listen_sock);
            handler->listen_sock = -1;
            return;
        }
        /* Initialize local, NAD MIPC addresses */

        ipcom_forwarder_mipc_addr_init(&handler->connection.addr,
                                       bus_num, MIPC_NODE_ANY, MIPC_PORT_NAD);
        /* Bind ctrl socket to local address */

        ret = mipc_bind (handler->listen_sock,
                         &handler->connection.addr,
                         sizeof(handler->connection.addr));
        if (ret < 0)
        {
            MIPCFWDBG("Error[%d]: binding socket: node[%d], port[%d]\n",
                      ret,
                      handler->connection.addr.nodeNum,
                      handler->connection.addr.portNum);
            goto close_listen_sock;
        }

        /* Ensure socket is blocking */

        ret = mipc_setsockopt(handler->listen_sock,
                              MIPC_SOL,
                              MIPC_SO_NBIO,
                              &non_blocking,
                              sizeof(non_blocking));
        if (ret != MIPC_OK)
        {
            MIPCFWDBG("Error[%d]: disabling non-blocking mode on the"
                      " NAD socket\n", ret);
            goto close_listen_sock;
        }
        /* Allow 1 connection */

        ret = mipc_listen(handler->listen_sock, 1);
        if (ret != MIPC_OK)
        {
            MIPCFWDBG("Error[%d]: listen failed on the NAD socket: %d\n",
                      ret);
            goto close_listen_sock;
        }

        /*
         * Init RX callback for NAD on the listening parent socket. It won't
         * be called for the parent, but will be inherited by (and called for)
         * the child.
         */

        mipc_setsockopt(handler->listen_sock,
                        MIPC_SOL,
                        MIPC_SO_RX_CALLBACK,
                        (void *)ipcom_forwarder_nad_rx_cb,
                        sizeof (MIPC_RX_CALLBACK));

        /*
         * Init MIPC_DISCONNECTED_CALLBACK callback for NAD on the listening
         * parent socket. It won't be called for the parent, but will be
         * inherited by (and called for) the child if the connection terminates.
         * It will close the current socket and respawn a task to re-run this
         * routine.
         */

        mipc_setsockopt(handler->listen_sock,
                        MIPC_SOL,
                        MIPC_SO_DISCONNECTED_CALLBACK,
                        (void *)ipcom_forwarder_nad_disconnect_cb,
                        sizeof (MIPC_DISCONNECTED_CALLBACK));
    }


    /* Blocking accept */

    handler->connection.sock = mipc_accept(handler->listen_sock,
                                           &from, &fromlen);
    if (handler->connection.sock < 0)
    {
        MIPCFWDBG("Error: accept failed on the NAD socket: %d\n");
        return;
    }

    /*
     * RACE -- wrnad may send data, and the RX callback called, as soon
     * as mipc_accept() runs. (Not before, unlike TCP!)
     * However, we can't set the user handle on the child socket or
     * the peer address in the handler until mipc_accept() has returned.
     * So, if wrnad sends data before we finish setting up the handle,
     * the RX callback may run without the info it needs.  So, the RX
     * callback at present just uses the single &the_nad rather than the
     * connection socket's associated user handle, which may not be set up.
     *
     * The callback doesn't actually currently depend on either the
     * socket handler or handler->connection.peer_addr.  We go ahead
     * and set them just for fun below, anyhow.
     *
     * In the future, MIPC may be modified to let the user handle be
     * inherited from the parent listening socket as well as the socket
     * options.  One could set it as desired just before calling mipc_accept().
     * Since the mipc_accept() call is required to establish the connection
     * with the peer, this would be race-free. If the peer address were ever
     * really needed, that is passed directly to the RX callback.
     */

    MIPCFWDBG("Connection accepted on NAD port %d from node %d, port %d\n",
               MIPC_PORT_NAD, from.nodeNum, from.portNum);

    ipcom_forwarder_mipc_addr_init(&handler->connection.peer_addr,
                                   bus_num, from.nodeNum, from.portNum);

    /* Make socket user handles point to the NAD handler.  This provides
     * us direct access to our context from within the MIPC callbacks.
     */

    mipc_setuserhandle(handler->connection.sock, (unsigned long)handler);

    /* Note, we do not close the parent socket, since we want to keep it
       alive to hold on to the well known cNAE 'NAD server' port. */

    return;
}


/*
 *===========================================================================
 *                    ipcom_ifproxy_end_error
 *===========================================================================
 * Context:     tNet0 only
 * Description: MUX protocol callback to receive END 'error' events
 *              Handles link-up and link-down events.
 * Parameters:  arg - the ifproxy handler
 *              err - the END_ERR structure
 */
IP_STATIC void
ipcom_ifproxy_end_error(void *arg, END_ERR *err)
{
    ifproxy_handler_t *proxy = arg;
    int link = -1;

    switch (err->errCode)
    {
    case END_ERR_LINKUP:
        link = 1;
        break;
    case END_ERR_LINKDOWN:
        link = 0;
        break;
    default:
        /* don't care */
        break;
    }
    if (link >= 0)
    {
        /*
         * Use ifproxy_nettask_sock in order to send an express event to
         * the control socket peer.
         * TODO: In principle, there's no upper bound on the
         * rate at which we send link-state-change messages;
         * but in practice, we expect them not to be too fast.
         */
        if (mipc_sendexpress64(ifproxy_nettask_sock,
                               &proxy->ctrl_connection.peer_addr,
                               IFPROXY_EXPRESS_DATA(IFPROXY_CTRL_EXPRESS_LSC,
                                                    link)) != 0)
        {
            MIPCFWDBG("ifproxy cannot send LSC for port %u\n",
                      proxy->port_index);
        }
    }
}

IP_STATIC void
ipcom_ifproxy_linkstate_notify(ifproxy_handler_t *proxy)
{
    Ipcom_fw_port_t *fw_port;
    END_MEDIA media;
    int link;

    fw_port = ipcom_forwarder_get_port_cookie(proxy->port_index);

    if (muxIoctl(fw_port->pEnd, EIOCGIFMEDIA, (caddr_t)&media) != OK)
    {
        MIPCFWDBG("EIOCGIFMEDIA failed for %p\n", fw_port->pEnd);
        return;
    }

    link = 0;
    if ((media.endMediaStatus & (IFM_ACTIVE|IFM_AVALID)) ==
        (IFM_ACTIVE|IFM_AVALID))
        link = 1;

    if (mipc_sendexpress64(proxy->ctrl_connection.sock,
                           &proxy->ctrl_connection.peer_addr,
                           IFPROXY_EXPRESS_DATA(IFPROXY_CTRL_EXPRESS_LSC,
                                                link)) != 0)
    {
        MIPCFWDBG("ipcom_ifproxy_linkstate_notify failed for %u\n",
                  proxy->port_index);
    }
}

/*
 *===========================================================================
 *                    ipcom_ifproxy_end_attach
 *===========================================================================
 * Context:     control NAE initialization
 * Description: Attach to END installing MUX protocol callback to receive
 *              END 'error' events to handle link-up and link-down events.
 * Parameters:  
 */
IP_STATIC int
ipcom_ifproxy_end_attach(void)
{
    int i;
    Ipcom_fw_port_t *fw_port; 
    ifproxy_handler_t *proxy;

    /* Attach to the END in order to get link-state-change notifications */
    for (i = 0; i < MAX_FW_PORTS; ++i)
    {
        fw_port = ipcom_forwarder_get_port_cookie(i);
        
        if (fw_port == IP_NULL)
            continue;
        
        proxy = &port_proxy[i];

        proxy->mux_cookie =
            mux2Bind(fw_port->pEnd->devObject.name, 
                     fw_port->pEnd->devObject.unit,
                     NULL,  /* stackRcvRtn  -- won't receive packets this way */
                     NULL,  /* stackShutdownRtn -- TODO ? */
                     NULL,  /* stackTxRestartRtn -- TODO */
                     ipcom_ifproxy_end_error, /* stackErrorRtn */
                     MUX_PROTO_PROMISC,     /* doesn't matter, no RX routine */
                     "ifproxy",     /* protocol name */
                     proxy          /* protocol callback argument */
                     );

        if (proxy->mux_cookie == NULL)
        {
            MIPCFWDBG("ifproxy cannot bind to %s%d\n",
                    fw_port->pEnd->devObject.name, 
                    fw_port->pEnd->devObject.unit);
            return -1;
        }
        /* TODO: check for current link state in case link state change
           was delivererd before we bound to the device ... */
    }
    return 0;
}

IP_STATIC void
ipcom_ifproxy_end_detach(ifproxy_handler_t *proxy)
{
    if (proxy->mux_cookie != NULL)
        muxUnbind(proxy->mux_cookie, MUX_PROTO_PROMISC, NULL);
}

/*
 *===========================================================================
 *                    ipcom_ifproxy_nettask_socket_init
 *===========================================================================
 * Context:     control NAE initialization
 * Description: Create the MIPC socket used to send link state change events
 *              from tNet0.
 * Parameters:  busnum - the MIPC bus number
 * Returns:     0 if successful, -1 if not.
 */
IP_STATIC int
ipcom_ifproxy_nettask_socket_init(int busnum)
{
    struct mipc_sockaddr addr;

    ifproxy_nettask_sock = mipc_socket (MIPC_AF, MIPC_SOCK_DGRAM, 0);
    if (ifproxy_nettask_sock < 0)
    {
        MIPCFWDBG("ifproxy cannot create nettask socket\n");
        return -1;
    }
    (void)ipcom_forwarder_mipc_socket_init(ifproxy_nettask_sock,
                                           1,
                                           0);

    /*
     * Hmm, can we guarantee we don't get a well known port
     * that we need to use (i.e. for the NAD)? Since ports are assigned
     * downwards and the NAD port is 3, we should be pretty safe.
     * But we could bind to the NAD port first if needed.
     */
    ipcom_forwarder_mipc_addr_init(&addr,
                                   busnum, MIPC_NODE_ANY, MIPC_PORT_ANY);
    if (mipc_bind(ifproxy_nettask_sock, &addr, sizeof(addr)) < 0)
    {
        (void)mipc_close(ifproxy_nettask_sock);
        ifproxy_nettask_sock = -1;
        MIPCFWDBG("ifproxy cannot bind nettask socket\n");
        return -1;
    }
    return 0;
}

/*
 ****************************************************************************
 *                      GLOBAL ROUTINES
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipcom_fw_packet_done
 *===========================================================================
 * Context:     tNet0 on control NAE
 * Description: called to free TX buffer after ifproxy transmit completes
 *              on physical device
 * Parameters:  hdr - ifproxy command header at start of packet
 *              cookie - contains MIPC_ZBUFFER
 * Returns:     N/A
 */
IP_GLOBAL void
ipcom_fw_packet_done(ifproxy_cmd_hdr_t *hdr, void *cookie)
{
    ifproxy_handler_t *proxy = &port_proxy[hdr->PortIndex];
    MIPC_ZBUFFER zbuf = cookie;

    mipc_zfree(proxy->data_connection.sock, zbuf);
}


/*
 *===========================================================================
 *                    ipcom_forwarder_port_announce
 *===========================================================================
 * Context:     Control NAE start-up task
 * Description: Create managment, nae socket
 * Parameters:  port_index - the index for the forwarder port to be announced
 *              bus_num - the MIPC bus number
 * Returns:     0 if successful, -1 on failure.
 */
IP_GLOBAL int
ipcom_forwarder_port_announce(int port_index, unsigned short bus_num)
{
    int ret;
    ifproxy_handler_t *proxy;
    Ipcom_fw_port_t *fw_port = ipcom_forwarder_get_port_cookie(port_index);

    if (fw_port == IP_NULL)
        return -1;

    proxy = &port_proxy[port_index];

    memset(proxy, 0, sizeof(ifproxy_handler_t));
    proxy->ctrl_connection.sock = -1;
    proxy->data_connection.sock = -1;

    proxy->port_index = port_index;

    if (bus_num > MIPC_MAX_BUS)
    {
        MIPCFWDBG("Error: bus num [%d] too high\n", bus_num);
        goto errout;
    }

    /* Attach to the END in order to get link-state-change notifications */
    /*if (ipcom_ifproxy_end_attach(proxy, fw_port->pEnd) < 0)
        goto errout;*/

    /* Create, initialize ifproxy control socket. */

    proxy->ctrl_connection.sock = mipc_socket (MIPC_AF, MIPC_SOCK_SEQPACKET, 0);
    if (proxy->ctrl_connection.sock < 0)
    {
        MIPCFWDBG("Error: couldn't create ctrl socket\n");
        goto errout;
    }

    ret = ipcom_forwarder_mipc_socket_init(proxy->ctrl_connection.sock,
                                           MIPC_CTRL_RX_BUFS,
                                           MIPC_CTRL_TX_BUFS);
    if (ret < 0)
    {
        MIPCFWDBG("Error: couldn't initialize ctrl socket\n");
        goto errout;
    }

    /*
     * Create and initialize ifproxy data socket.  Note, since this
     * is a SOCK_DGRAM socket, we don't need to create a parent to
     * listen/accept.
     */

    proxy->data_connection.sock = mipc_socket (MIPC_AF, MIPC_SOCK_DGRAM, 0);
    if (proxy->data_connection.sock < 0)
    {
        MIPCFWDBG("Error: couldn't create data socket\n");
        goto errout;
    }

    ret = ipcom_forwarder_mipc_socket_init(proxy->data_connection.sock,
                                           MIPC_DATA_RX_BUFS,
                                           MIPC_DATA_TX_BUFS);
    if (ret < 0)
    {
        MIPCFWDBG("Error: couldn't create data socket\n");
        goto errout;
    }

    MIPCFWDBG("forwarder port #%u : ctrl_sock=%u, data_sock=%u\n",
              proxy->port_index,
              proxy->ctrl_connection.sock,
              proxy->data_connection.sock);

    /* Make socket user handles point to the ifproxy handler.  This provides
     * us direct access to our context from within the MIPC callbacks.
     */

    mipc_setuserhandle(proxy->ctrl_connection.sock, (unsigned long)proxy);
    mipc_setuserhandle(proxy->data_connection.sock, (unsigned long)proxy);

    /* Initialize local, ifproxy driver MIPC addresses */

    ipcom_forwarder_mipc_addr_init(&proxy->ctrl_connection.addr,
                                   bus_num, MIPC_NODE_ANY, MIPC_PORT_ANY);
    ipcom_forwarder_mipc_addr_init(&proxy->ctrl_connection.peer_addr,
                                   bus_num, MIPC_NODE_MGMT, MIPC_PORT_MGMT);

    /* Bind ctrl socket to local address */

    ret = mipc_bind(proxy->ctrl_connection.sock,
                    &proxy->ctrl_connection.addr,
                    sizeof(proxy->ctrl_connection.addr));
    if (ret < 0)
    {
        MIPCFWDBG("Error[%d]: binding socket: node[%d], port[%d]\n",
                  ret,
                  proxy->ctrl_connection.addr.nodeNum,
                  proxy->ctrl_connection.addr.portNum);
        goto errout;
    }

    /* Bind data socket to local address */
    ipcom_forwarder_mipc_addr_init(&proxy->data_connection.addr,
                                   bus_num, MIPC_NODE_ANY, MIPC_PORT_ANY);

    ret = mipc_bind(proxy->data_connection.sock,
                    &proxy->data_connection.addr,
                    sizeof(proxy->data_connection.addr));
    if (ret < 0)
    {
        MIPCFWDBG("Error[%d]: binding socket: node[%d], port[%d]\n",
                  ret,
                  proxy->data_connection.addr.nodeNum,
                  proxy->data_connection.addr.portNum);
        goto errout;
    }

    /* Connect to ifproxy [management] ctrl port */

    MIPCFWDBG("Connect to node: %d port: %d bus: %d\n",
              proxy->ctrl_connection.peer_addr.nodeNum,
              proxy->ctrl_connection.peer_addr.portNum,
              proxy->ctrl_connection.peer_addr.busNum);


    if (ipcom_forwarder_ifproxy_connect(proxy) < 0)
    {
        MIPCFWDBG("Error: connecting to management board for port %d\n",
                  port_index);
        goto errout;
    }

    return 0;

errout:
    ipcom_ifproxy_end_detach(proxy);

    if (proxy->ctrl_connection.sock >= 0)
        mipc_close(proxy->ctrl_connection.sock);
    if (proxy->data_connection.sock >= 0)
        mipc_close(proxy->data_connection.sock);


    return -1;
}

/*
 ****************************************************************************
 *                      PUBLIC ROUTINES
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipcom_forwarder_ifproxy_init
 *===========================================================================
 * Context:     TODO: This should be our init function
 * Description:
 * Parameters:
 * Returns:     0 if successful, -1 if not.
 */
IP_PUBLIC int
ipcom_forwarder_ifproxy_init(void)
{
    int i;
    int busnum;
    int nports = 0;

    /* Ensure we've got a valid MIPC bus */

    busnum = ipcom_ifproxy_mipc_bus();
    if (busnum < 0)
    {
        return -1;
    }

    if (ipcom_ifproxy_nettask_socket_init(busnum) < 0)
        return -1;

    for (i = 0; i < MAX_FW_PORTS; ++i)
    {
        if (ipcom_forwarder_get_port_cookie(i) == IP_NULL)
            continue;

        /* inform ifproxy of this port. */

        if (ipcom_forwarder_port_announce(i, busnum) == 0)
        {
            ++nports;
        }
    }

    ipcom_forwarder_ifproxy_cpus_set(nports);

    ipcom_forwarder_setup_hw();
    
    if (ipcom_ifproxy_end_attach() < 0)
    {
        MIPCFWDBG("Error: couldn't attach to END\n");
    }

    /* spawn NAD init task to wait for connection from wrnad */
    the_nad.listen_sock = -1;
    the_nad.connection.sock = -1;
    return ipcom_ifproxy_nad_wait_task_spawn (busnum);

}

/*
 *===========================================================================
 *                    ipcom_ifproxy_mipc_bus
 *===========================================================================
 * Context:     initialization, control NAE
 * Description: Return MIPC bus number used by the control NAE
 * Parameters:
 * Returns:     mipc bus number
 */
IP_PUBLIC int ipcom_ifproxy_mipc_bus(void)
{
    char * bus_name = IPCOM_FW_MIPC_BUS_NAME;
    int busnum;
    int ret;

    ret = mipc_getbusbyname(bus_name, &busnum);
    if (ret < 0)
    {
        MIPCFWDBG("Error: no bus found for %s\n", bus_name);
        return -1;
    }
    return busnum;
}


/*
 *===========================================================================
 *                    ipcom_ipc_msgq_init
 *===========================================================================
 * Context:     initialization, control NAE
 * Description: initialize an IPC message queue
 * Parameters:  msgq - the IPC message queue structure to initialize
 *              busno - the MIPC bus number on which the msgq will be used
 *              express_data - value of MIPC express data to send to notify
 *                             the message queue handler of pending work.
 *              n_cmds - the number of IPC message queue commands
 *                       in the cmds array
 *              cmds - an array of command functions, one corresponding to
 *                     each mipc command C, (0 <= C < n_cmds)
 * Returns:     0 if successful, a negative error code if not.
 *
 * Notes: This routine creates and binds a MIPC socket on the current
 * node, and sets the socket's express data callback to handle messages
 * sent to the IPC message queue using ipcom_ipc_msgq_send().  The
 * value of the express_data argument is not important at this point;
 * the callback installed does not use express_data unless it needs
 * to repost itself.  (TODO: We could specify an alternate function
 * to be called if the express data received by the callback did
 * not match the specified express data).
 */
IP_PUBLIC int
ipcom_ipc_msgq_init(Ipcom_ipc_msgq_t *msgq, unsigned short busno,
                    uint64_t express_data,
                    Ip_u32 n_cmds, Ipcom_ipc_msgq_cmd *cmds)
{
    int ret;

    if (msgq == IP_NULL || (n_cmds != 0 && cmds == IP_NULL))
        return -IP_ERRNO_EINVAL;

    if (((uint32_t)msgq & (_CACHE_ALIGN_SIZE - 1)) != 0)
        return -IP_ERRNO_EINVAL;

    memset (msgq, 0, sizeof (*msgq));

    ipcom_ipc_spinlock_init(&msgq->u.lock);

    msgq->n_cmds = n_cmds;
    msgq->cmds = cmds;

    msgq->sock = mipc_socket (MIPC_AF, MIPC_SOCK_DGRAM, 0);
    if (msgq->sock < 0)
    {
        MIPCFWDBG("Error: couldn't create message queue socket\n");
        return msgq->sock;
    }

    ret = ipcom_forwarder_mipc_socket_init(msgq->sock, 1, 0);
    if (ret < 0)
    {
        MIPCFWDBG("Error: couldn't set message queue socket options\n");
        goto errout;
    }

    mipc_setuserhandle(msgq->sock, (unsigned long)msgq);

    mipc_setsockopt(msgq->sock,
                    MIPC_SOL,
                    MIPC_SO_EXPRESS_CALLBACK,
                    (void *)ipcom_ipc_msgq_express_cb,
                    sizeof (MIPC_EXPRESS_CALLBACK));


    ipcom_forwarder_mipc_addr_init(&msgq->to, busno,
                                   MIPC_NODE_ANY, MIPC_PORT_ANY);

    ret = mipc_bind(msgq->sock, &msgq->to, sizeof (msgq->to));
    if (ret < 0)
    {
        MIPCFWDBG("Error[%d]: binding msgq socket\n", ret);
        goto errout;
    }

    return 0;

errout:
    (void)mipc_close(msgq->sock);
    return ret;
}
/*
 ****************************************************************************
 *                           DEBUG/SHOW ROUTINES
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipcom_ifproxy_show
 *===========================================================================
 * Context:     control NAE
 * Description: Show ctrl nae <-> ifproxy MIPC connection info
 * Parameters:
 * Returns:     N/A
 */
IP_PUBLIC void
ipcom_ifproxy_show(void)
{
    int i;
    int index;
    mipc_connection_t *ctrl;
    mipc_connection_t *data;

    printf ("FWD  MGMT     CTRL loc  remote  DATA loc  remote\n");
    printf ("port ifindex  sock port port    sock port port \n");
    for (i = 0; i < MAX_FW_PORTS; ++i)
    {
        ctrl = &port_proxy[i].ctrl_connection;
        data = &port_proxy[i].data_connection;
        index = port_proxy[i].ifindex;
        printf ("%4u %7u %5u %4u %4u   %5u %4u %4u\n",
                i, index,
                ctrl->sock, ctrl->addr.portNum, ctrl->peer_addr.portNum,
                data->sock, data->addr.portNum, data->peer_addr.portNum);
    }
}

#else
char ipcom_vxworks_nehalem_forwarder_mipc_empty_file;
#endif /* IPCOM_USE_NEHALEM_FORWARDER */
