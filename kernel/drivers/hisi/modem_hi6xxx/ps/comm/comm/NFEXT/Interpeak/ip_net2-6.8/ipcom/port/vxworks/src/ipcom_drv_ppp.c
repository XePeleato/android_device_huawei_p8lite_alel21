/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_drv_ppp.c,v $ $Revision: 1.21 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/port/vxworks/src/ipcom_drv_ppp.c,v $
 *   $Author: kenneth $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Roger Boden <roger@interpeak.se>
 ******************************************************************************
 */


/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 2                    CONFIGURATION
 ****************************************************************************
 */
#define IPCOM_SKIP_NATIVE_SOCK_API
#include "ipcom_config.h"

#if defined(IPPPP) && defined(IPCOM_SOCK)

#include <ipppp.h>


/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#include "ipcom_cstyle.h"
#include "ipcom_err.h"
#include "ipcom_clib.h"
#include "ipcom_pkt.h"
#include "ipcom_sock.h"
#include "ipcom_sock2.h"
#include "ipcom_netif.h"
#include "ipcom_vxworks.h"
#include "ipcom_syslog.h"

/* VxWorks include files. */
#include <sockLib.h>

/* Std include files */
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

#define IPCOM_SYSLOG_PRIORITY    IPCOM_SYSLOG_IPCOM_PRIORITY
#define IPCOM_SYSLOG_FACILITY    IPCOM_LOG_IPPPP


/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

/*
 *===========================================================================
 *                         Ipcom_drv_ppp_ipc
 *===========================================================================
 */
typedef struct Ipcom_drv_ppp_ipc_struct
{
    Ipcom_netif   *netif;
    Ipcom_pkt     *pkt;
}
Ipcom_drv_ppp_ipc;


/*
 *===========================================================================
 *                         Ipcom_drv_ppp_link
 *===========================================================================
 * Port specific PPP link data.
 */
typedef struct Ipcom_drv_ppp_link_struct
{
    Ip_u32         index;    /* link index. */
    Ipcom_netif   *netif;    /* back pointer to generic interface data. */

    Ipcom_ipc      out_ipc;
    int            out_q_nr;
    int            fd;
    int            sv[2];
    int            baudrate;
    int            wincompat;
    char          *devname;
}
Ipcom_drv_ppp_link;


/*
 *===========================================================================
 *                         Ipcom_pdrv_ppp
 *===========================================================================
 */
typedef struct Ipcom_pdrv_ppp_struct
{
    Ipcom_drv_ppp_link       plink[IPPPP_MAX_LINKS];
}
Ipcom_pdrv_ppp;


/*
 ****************************************************************************
 * 6                    EXTERN PROTOTYPES
 ****************************************************************************
 */


/*
 ****************************************************************************
 * 7                    LOCAL PROTOTYPES
 ****************************************************************************
 */

IP_STATIC int
ipcom_drv_ppp_ioctl(Ipcom_netif *netif, Ip_u32 command, void *data);
IP_STATIC int
ipcom_drv_ppp_output(Ipcom_netif *netif, Ipcom_pkt *pkt);


IPCOM_PROCESS(ipcom_drv_ppp_inputd);
IPCOM_PROCESS(ipcom_drv_ppp_outputd);


/*
 ****************************************************************************
 * 8                    DATA
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 9                    LOCAL FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipcom_socketpair_udp
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipcom_socketpair_udp(int fd[2])
{
    struct sockaddr_storage ss;
	socklen_t               socklen;
    struct addrinfo         hints;
    struct addrinfo        *res = IP_NULL;
    int                     opt;

	fd[0] = fd[1] = -1;

    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_DGRAM;
    if (getaddrinfo(NULL, "0", &hints, &res) != 0)
        goto failed;

    /* Server socket */
    fd[0] = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (fd[0] == -1)
        goto failed;
    opt = 1;
    if (bind(fd[0], res->ai_addr, res->ai_addrlen) != 0)
        goto failed;
    socklen = res->ai_addrlen;
	if (getsockname(fd[0], (struct sockaddr *) &ss, (void*)&socklen) != 0)
        goto failed;

    /* Client socket */
    fd[1] = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (fd[1] == -1)
        goto failed;
    opt = 1;
	if (connect(fd[1], (struct sockaddr *) &ss, socklen) != 0)
        goto failed;

    if (res != NULL)
        freeaddrinfo(res);
	return 0;

 failed:
    if (res != IP_NULL)
        freeaddrinfo(res);
	if (fd[0] != -1)
        close(fd[0]);
	if (fd[1] != -1)
        close(fd[1]);
	return -1;
}


/*
 *===========================================================================
 *                    ipcom_socketpair_tcp
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
#if 0
IP_PUBLIC int
ipcom_socketpair_tcp(int fd[2])
{
	int server;
	union Ip_sockaddr_union  sock;
	Ip_socklen_t socklen;
	Ip_bool connected = IP_FALSE;
    int opt;
    struct Ip_addrinfo   hints;
    struct Ip_addrinfo  *res = IP_NULL;

	fd[0] = fd[1] = server = -1;

    ipcom_memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = IP_SOCK_STREAM;
    if (ipcom_getaddrinfo(IP_NULL, "0", &hints, &res) != 0)
        goto failed;

    /* Server socket */
    server = ipcom_socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (server == -1)
        goto failed;
    opt = 1;
	(void)ipcom_setsockopt(server, IP_SOL_SOCKET, IP_SO_REUSEADDR, &opt, sizeof(opt));
    if (ipcom_bind(server, res->ai_addr, res->ai_addrlen) != 0)
        goto failed;
	if (ipcom_listen(server, 1) != 0)
        goto failed;
    socklen = res->ai_addrlen;
	if (ipcom_getsockname(server, &sock.sa, &socklen) != 0)
        goto failed;

    /* Client socket */
    fd[1] = ipcom_socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (fd[1] == -1)
        goto failed;
    opt = 1;
	(void)ipcom_setsockopt(fd[1], IP_SOL_SOCKET, IP_SO_REUSEADDR, &opt, sizeof(opt));
    opt = 1;
    (void)ipcom_socketioctl(fd[1], IP_FIONBIO, &opt);

	if (ipcom_connect(fd[1], &sock.sa, socklen) == -1)
    {
		if (ipcom_errno != IP_ERRNO_EINPROGRESS)
            goto failed;
	}
    else
		connected = IP_TRUE;

    /* Spawn socket */
    fd[0] = ipcom_accept(server, IP_NULL, IP_NULL);
	if (fd[0] == -1)
        goto failed;
    opt = 1;
	(void)ipcom_setsockopt(fd[0], IP_SOL_SOCKET, IP_SO_REUSEADDR, &opt, sizeof(opt));
	ipcom_socketclose(server);

    /* Client socket revisited */
	if (connected == IP_FALSE)
    {
        Ip_fd_set  write_set;
        IP_FD_ZERO(&write_set);
        IP_FD_SET(fd[1], &write_set);
        if (ipcom_socketselect(fd[1] + 1, IP_NULL, &write_set, IP_NULL, IP_NULL) != 1)
            goto failed;
	}
    opt = 0;
    (void)ipcom_socketioctl(fd[1], IP_FIONBIO, &opt);

    if (res != IP_NULL)
        ipcom_freeaddrinfo(res);
	return 0;

 failed:
    if (res != IP_NULL)
        ipcom_freeaddrinfo(res);
	if (fd[0] != -1)
        ipcom_socketclose(fd[0]);
	if (fd[1] != -1)
        ipcom_socketclose(fd[1]);
	if (server != -1)
        ipcom_socketclose(server);
	return -1;
}
#endif


/*
 *===========================================================================
 *                    ipcom_drv_ppp_open
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipcom_drv_ppp_open(Ipcom_drv_ppp_link *plink)
{
    int error;

    if (plink->fd != -1)
        return 0;

    plink->fd = open(plink->devname, O_RDWR, 0);
    if (plink->fd == -1)
        goto fail;

    if (ioctl(plink->fd, FIOSETOPTIONS, OPT_RAW) != OK)
        goto fail;

    if (ioctl(plink->fd, FIOBAUDRATE, plink->baudrate) != OK)
        goto fail;

    return 0;

 fail:
    error = -errno;
    return error;
}


/*
 *===========================================================================
 *                    ipcom_drv_ppp_init
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipcom_drv_ppp_init(Ipcom_drv_ppp_link *plink)
{
    Ipcom_ipc            ipc;
    Ipcom_drv_ppp_ipc   *ipcbuf;
    char                 procname[IPCOM_PROC_NAME_MAX];
    Ip_err               retval;
    Ipcom_proc_attr      attr;

    /* Open driver */
    if (ipcom_drv_ppp_open(plink) != 0)
        goto fail;

    /* Create driver I/O daemons */
    ipcom_proc_attr_init(&attr);
    attr.priority  = IPCOM_PRIORITY_MIN;
    attr.stacksize = IPCOM_PROC_STACK_SMALL;

    /* Create and open output daemon */
    ipcom_sprintf(procname, "ipcom_drv_%s_output", plink->netif->name);
    retval = ipcom_proc_acreate(procname, ipcom_drv_ppp_outputd, &attr, IP_NULL);
    if (retval != IPCOM_SUCCESS)
        goto fail;

    retval = ipcom_ipc_open(&plink->out_ipc, procname, -1);
    if (retval != IPCOM_SUCCESS)
        goto fail;


    /* Create and start input daemon */
    ipcom_sprintf(procname, "ipcom_drv_%s_input", plink->netif->name);
    retval = ipcom_proc_acreate(procname, ipcom_drv_ppp_inputd, &attr, IP_NULL);
    if (retval != IPCOM_SUCCESS)
        goto fail;

    /* Open IPC. */
    ipcom_sprintf(procname, "ipcom_drv_%s_input", plink->netif->name);
    retval = ipcom_ipc_open(&ipc, procname, -1);
    if (retval != IPCOM_SUCCESS)
        goto fail;

    /* Create IPC buf */
    ipcbuf = ipcom_ipc_malloc(sizeof(Ipcom_drv_ppp_ipc));
    if (ipcbuf == IP_NULL)
        goto fail;
    ipcbuf->netif = plink->netif;

    /* Send IPC buf */
    retval = ipcom_ipc_send(&ipc, ipcbuf);
    if (retval != IPCOM_SUCCESS)
    {
        (void)ipcom_ipc_close(&ipc);
        goto fail;
    }

    /* Close IPC. */
    (void)ipcom_ipc_close(&ipc);

    /* Success. */
    return 0;

 fail:
    if (plink->fd != -1)
    {
        (void)close(plink->fd);
        plink->fd = -1;
    }
    IPCOM_LOG2(ERR, "ipcom_drv_ppp_init :: failed to open %s for %s",
               plink->devname,
               plink->netif->name);
    return -IP_ERRNO_EINVAL;
}


/*
 *===========================================================================
 *                    ipcom_drv_ppp_exit
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int /*D!!*/
ipcom_drv_ppp_exit(Ipcom_drv_ppp_link *plink)
{
    Ipcom_drv_ppp_ipc   *ipcbuf;
    int  fd;

    fd = plink->fd;
    if (fd != -1)
    {
        plink->fd = -1;  /* will make input daemon exit */
        (void)close(fd);
    }

    /* Kill output daemon */
    ipcbuf = ipcom_ipc_malloc(sizeof(Ipcom_drv_ppp_ipc));
    if (ipcbuf == IP_NULL)
    {
        IP_PANIC();
        return 0;
    }
    ipcbuf->netif = plink->netif;
    ipcbuf->pkt = IP_NULL;

    /* Send IPC buf to output task */
    (void)ipcom_ipc_send(&plink->out_ipc, ipcbuf);
    (void)ipcom_ipc_close(&plink->out_ipc);

    return 0;
}


/*
 *===========================================================================
 *                    ipcom_drv_ppp_ioctl
 *===========================================================================
 * Description: IO control.
 * Parameters:  netif - A network interface.
 *              command - The control task to perform.
 *              data - Data associated with the specified command.
 * Returns:     0 = success, <0 = error code
 *
 */
IP_STATIC int
ipcom_drv_ppp_ioctl(Ipcom_netif *netif, Ip_u32 command, void *data)
{
    Ipcom_pdrv_ppp      *pdrv;
    Ipcom_drv_ppp_link  *plink;
    struct Ip_pppreq    *pppreq;
    int ret;

    pdrv  = (Ipcom_pdrv_ppp *)netif->pdrv;
    plink = &pdrv->plink[netif->link_index];
    pppreq = data;

    switch (command)
    {
    case IP_SIOCXOPEN:
    case IP_SIOCXPPPSDRVUP:
        /* Create the driver. */
        ret = ipcom_drv_ppp_init(plink);
        if (ret != 0)
            return ret;

        /* Signal link up immediately. */
        netif->link_ioevent(netif, IP_EIOXUP, IP_NULL, 0);
        break;

    case IP_SIOCXCLOSE:
    case IP_SIOCXPPPSDRVDOWN:
        /* Kill the driver. */
        if (ipcom_drv_ppp_exit(plink) != 0)
            return -IP_ERRNO_EINVAL;

        /* Signal link down immediately. */
        netif->link_ioevent(netif, IP_EIOXDOWN, IP_NULL, 0);
        break;

    case IP_SIOCIFDESTROY:
        if (IP_BIT_ISSET(netif->flags, IP_IFF_RUNNING))
            return -IP_ERRNO_EBUSY;
        ret = ipcom_if_detach(netif);
        if (ret != 0)
            return 0;
        if (plink->devname)
            ipcom_free(plink->devname);
        if (pdrv != IP_NULL)
            ipcom_free(pdrv);
        return ipcom_if_free(netif);

    case IP_SIOCXPPPGDRVCONF:
        pppreq->pppru.drvconf.drv_flags = 0; /* See ipcom_netif.h for IP_DRVPPP_FLAG_xxx options. */
        pppreq->pppru.drvconf.baudrate = plink->baudrate;
        pppreq->pppru.drvconf.rcv_accm = 0xffffffff;
        pppreq->pppru.drvconf.snd_accm = 0xffffffff;
        pppreq->pppru.drvconf.mru = 1500;
        pppreq->pppru.drvconf.mtu = 1500;
        break;

    case IP_SIOCXPPPSDRVBAUDRATE:
        /* Running, change the baudrate. */
        if (plink->fd != -1)
            if (ioctl(plink->fd, FIOBAUDRATE, pppreq->pppru.drv_baudrate) != OK)
                return -IP_ERRNO_EINVAL;

        /* Baudrate changed. */
        plink->baudrate = pppreq->pppru.drv_baudrate;
        break;

    case IP_SIOCXPPPSDRVWINCOMPAT:
        plink->wincompat = pppreq->pppru.drv_wincompat;
        break;

    case IP_SIOCXPROMISC:
        if (*(Ip_bool *) data)
            IP_BIT_SET(netif->flags, IP_IFF_PROMISC);
        else
            IP_BIT_CLR(netif->flags, IP_IFF_PROMISC);
        break;

    default:
        return -IP_ERRNO_EINVAL;
    }

    return 0;
}


/*
 *===========================================================================
 *                    ipcom_drv_ppp_inputd
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IPCOM_PROCESS(ipcom_drv_ppp_inputd)
{
    Ip_err     retval;
    Ipcom_ipc  ipc;
    Ipcom_drv_ppp_ipc   *ipcbuf;
    Ipcom_netif         *netif;
    Ipcom_pdrv_ppp      *pdrv;
    Ipcom_drv_ppp_link  *plink;
    Ipcom_pkt *pkt = IP_NULL;
    int i, num;
    ssize_t  maxlen, len;
    Ip_u8 buf[128];
    char   ifname[IP_IFNAMSIZ];
    fd_set  read_set;

    ipcom_proc_init();

    /* Install IPC. */
    retval = ipcom_ipc_install();
    if (retval != IPCOM_SUCCESS)
        ipcom_proc_exit();

    /* Wait for start message. */
    retval = ipcom_ipc_receive(&ipc, &ipcbuf, -1);
    if (retval != IPCOM_SUCCESS)
        ipcom_proc_exit();

    netif = ipcbuf->netif;
    ipcom_strcpy(ifname, netif->name);
    pdrv  = (Ipcom_pdrv_ppp *)netif->pdrv;
    plink = &pdrv->plink[0];  /*!! link index. */
    ipcom_ipc_free(ipcbuf);

    /* Create communication channel due to wake up problems */
    if (ipcom_socketpair_udp(plink->sv) != 0)
        goto exit;

    IPCOM_LOG2(DEBUG, "ipcom_drv_%s_input :: starting PPP input daemon on %s",
               ifname, plink->devname);

    maxlen = 12 + (IP_PPP_MTU << 1);

    FD_ZERO(&read_set);

    /* Loop forever, reading packets. */
    for(;;)
    {
        /* Wait for data or close command */
        /* (check if fd has been externally set to -1, before and after the select) */
        if (plink->fd == -1)
            goto exit;
        FD_SET(plink->fd, &read_set);
        FD_SET(plink->sv[0], &read_set);
        num = select(IP_MAX(plink->fd, plink->sv[0]) + 1, &read_set, NULL, NULL, NULL);
        if (plink->fd == -1)
            goto exit;
        if (num < 0)
        {
            IPCOM_LOG4(DEBUG, "ipcom_drv_%s_input :: select(ser=%d,local=%d) error = %d",
                       ifname, plink->fd, plink->sv[0], errno);
            continue;
        }
        else if (num == 0 || !FD_ISSET(plink->fd, &read_set))
            continue;

        /* Read data. */
        len = read(plink->fd, (char *)buf, sizeof(buf));
        if (len <= 0)
        {
            IPCOM_LOG2(DEBUG, "ipcom_drv_%s_input :: read() error=%d", ifname, errno);
            if (len <= 0 && plink->fd != -1)
            {
                /* Peer closed, reinitialize */
                IPCOM_LOG1(DEBUG, "ipcom_drv_%s_input :: peer closed -> restarting", ifname);
                (void)close(plink->fd);
                plink->fd = -1;
                if (ipcom_drv_ppp_open(plink) != 0)
                    goto exit;
            }
            continue;
        }

        /* Parse newly read bytes and copy over to packet */
        for(i = 0; i < len; i++)
        {
            /* Drop all bytes 0-0x1f */
            if (buf[i] < 0x20)
                continue;

            /* Allocate a new packet */
            if (pkt == IP_NULL)
            {
                pkt = ipcom_pkt_malloc(maxlen, IP_FLAG_FC_BLOCKOK);
                if (pkt == IP_NULL)
                    goto exit;
                IPCOM_DRV_SET_PKT_START(pkt, 6, 4);
                pkt->end = pkt->start;
                pkt->link_cookie = 0;
                IPCOM_PKT_TRACE(pkt, IPCOM_PKT_ID_RX);
            }

            /* Packet too long - drop it */
            if (pkt->end - pkt->start > maxlen)
            {
                pkt->end = pkt->start;
                continue;
            }

            /* Add byte to packet. */
            pkt->data[pkt->end++] = buf[i];

            /* Fount PPP terminator. */
            if (buf[i] == 0x7e)
            {
                /* Too short, drop. */
                if (pkt->end - pkt->start < 4)
                {
                    pkt->end = pkt->start;
                    continue;
                }

                /*IPCOM_LOG2(DEBUG2, "ipcom_drv_ppp_inputd :: received %d bytes on %s",
                  pkt->end - pkt->start, IPCOM_VXWORKS_DRV_PPP_DRVNAME);*/

                /* Give packet to IPNET/IPLITE. */
                ipcom_pkt_input_queue(netif, pkt);
                pkt = IP_NULL;
            }
        }

        /* Microsoft null-modem support. */
        if (plink->wincompat && pkt != IP_NULL && pkt->end - pkt->start >= 6)
        {
            for (i = pkt->start; i < pkt->end; i++)
                if (pkt->data[i] == 'C')
                {
                    if (pkt->end - i > 5
                        && ipcom_memcmp(&pkt->data[i + 1], "LIENT", 5) == 0)
                    {
                        Ipcom_pkt *pkt2;

                        /* Give "CLIENT" packet to IPNET/IPLITE. */
                        pkt2 = ipcom_pkt_malloc(maxlen, IP_FLAG_FC_BLOCKOK);
                        if (pkt2 == IP_NULL)
                            goto exit;
                        IPCOM_DRV_SET_PKT_START(pkt2, 6, 4);
                        pkt2->end = pkt2->start + 6;
                        ipcom_strcpy((char *)&pkt2->data[pkt2->start], "CLIENT");
                        pkt2->link_cookie = 0;
                        IPCOM_PKT_TRACE(pkt2, IPCOM_PKT_ID_RX);

                        /*IPCOM_LOG1(DEBUG2, "ipcom_drv_ppp_inputd :: rcvd CLIENT on %s",
                          IPCOM_VXWORKS_DRV_PPP_DRVNAME);*/

                        ipcom_pkt_input_queue(netif, pkt2);
                        pkt->end = pkt->start; /* drop current bytes */
                        break;
                    }
                }
        }
    }

exit:
    if (plink->sv[0] != -1)
        close(plink->sv[0]);
    if (plink->sv[1] != -1)
        close(plink->sv[1]);
    IPCOM_LOG1(DEBUG, "ipcom_drv_%s_input :: exiting", ifname);
    ipcom_proc_exit();
}


/*
 *===========================================================================
 *                    ipcom_drv_ppp_output
 *===========================================================================
 * Description: Outputs an IP packet on PPP.
 * Parameters:  netif - The network interface to use when sending the frame.
 *              pkt - The frame send. pkt->start is the offset to the
 *              PPP frame header.
 * Returns:     0 = success, <0 = error code.
 *
 */
IP_STATIC int
ipcom_drv_ppp_output(Ipcom_netif *netif, Ipcom_pkt *pkt)
{
    Ipcom_pdrv_ppp      *pdrv;
    Ipcom_drv_ppp_link  *plink;
    Ipcom_drv_ppp_ipc   *ipcbuf;
    Ip_err               retval;

    ip_assert(pkt->link_cookie == 0);

    pdrv = (Ipcom_pdrv_ppp *)netif->pdrv;
    plink = &pdrv->plink[pkt->link_cookie];

    /* Check if output queue full. */
    if (plink->out_q_nr > 8)
        return -IP_ERRNO_EWOULDBLOCK;

    /* Create IPC buf */
    ipcbuf = ipcom_ipc_malloc(sizeof(Ipcom_drv_ppp_ipc));
    if (ipcbuf == IP_NULL)
        goto fail;
    ipcbuf->netif = netif;
    ipcbuf->pkt = pkt;

    /* Send IPC buf */
    retval = ipcom_ipc_send(&plink->out_ipc, ipcbuf);
    if (retval != IPCOM_SUCCESS)
    {
        ipcom_ipc_free(ipcbuf);
        goto fail;
    }
    plink->out_q_nr++;

    return 0;

 fail:
    ipcom_pkt_free(pkt);
    return -IP_ERRNO_ENOBUFS;
}


/*
 *===========================================================================
 *                    ipcom_drv_ppp_outputd
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IPCOM_PROCESS(ipcom_drv_ppp_outputd)
{
    char   ifname[IP_IFNAMSIZ];
    Ip_err     retval;
    Ipcom_ipc  ipc;
    Ipcom_drv_ppp_ipc   *ipcbuf;
    Ipcom_pdrv_ppp      *pdrv;
    Ipcom_drv_ppp_link  *plink;
    ssize_t  len;
    Ipcom_pkt  *pkt;

    ipcom_strcpy(ifname, "ppp");

    ipcom_proc_init();

    /* Install IPC. */
    retval = ipcom_ipc_install();
    if (retval != IPCOM_SUCCESS)
        goto exit;

    for(;;)
    {
        retval = ipcom_ipc_receive(&ipc, &ipcbuf, -1);
        if (retval != IPCOM_SUCCESS)
            goto exit;

        ip_assert(ipcbuf->netif);
        ipcom_strcpy(ifname, ipcbuf->netif->name);

        pkt = ipcbuf->pkt;
        if (pkt == IP_NULL)
        {
            /* pkt == IP_NULL -> exit message */
            ipcom_ipc_free(ipcbuf);
            goto exit;
        }
        pdrv  = (Ipcom_pdrv_ppp *)ipcbuf->netif->pdrv;
        plink = &pdrv->plink[pkt->link_cookie];

        ip_assert(pkt != IP_NULL);
        IP_DEBUGLINE(ip_assert(IPCOM_GET_PKT_ID(pkt) != IPCOM_PKT_ID_FREEQ));
        IP_DEBUGLINE(ip_assert(IPCOM_GET_PKT_ID(pkt) != IPCOM_PKT_ID_INQ));
        IPCOM_PKT_TRACE(pkt, IPCOM_PKT_ID_TX);
        ip_assert(pkt->start >= 4);
        ip_assert(pkt->start < pkt->maxlen);
        ip_assert(pkt->link_cookie == 0);

        if (plink->fd == -1)
        {
            ipcom_ipc_free(ipcbuf);
            goto exit;

        }

        len = write(plink->fd, (char*)&pkt->data[pkt->start], pkt->end - pkt->start);
        if (len != pkt->end - pkt->start)
        {
            /*!! LOG */
        }

        ipcom_pkt_output_done(ipcbuf->netif, pkt, IP_FLAG_FC_ISRCONTEXT);
        plink->out_q_nr--;
        ipcom_ipc_free(ipcbuf);
    }

 exit:
    IPCOM_LOG1(DEBUG, "ipcom_drv_%s_output :: exiting", ifname);
    ipcom_proc_exit();
}


/*
 *===========================================================================
 *                    ipcom_drv_ppp_if_init
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipcom_drv_ppp_if_init(const char *ifname, const char *devname, int unit)
{
    Ipcom_netif         *netif;
    Ipcom_pdrv_ppp      *pdrv;
    Ipcom_drv_ppp_link  *plink;

    /* Allocate memory for the network interface structure */
    netif  = ipcom_if_malloc(IP_IFT_PPP);
    if (netif == IP_NULL)
        return IPCOM_ERR_FAILED;

    /* Set up the driver downcall function pointers */
    netif->drv_ioctl  = ipcom_drv_ppp_ioctl;
    netif->drv_output = ipcom_drv_ppp_output;

    /* Set interface name */
    ipcom_strncpy(netif->name, ifname, sizeof(netif->name) - 1);

    /* Allocate and init memory for the driver structure */
    pdrv = ipcom_malloc(sizeof(*pdrv));
    if (pdrv == IP_NULL)
        goto fail;
    ipcom_memset(pdrv, 0, sizeof(*pdrv));
    netif->pdrv = pdrv;
    plink = &pdrv->plink[unit];
    if (devname == IP_NULL)
    {
        char value[64];
        Ip_size_t value_size = sizeof(value);
        devname = ipcom_sysvar_get_conf("devname", value, &value_size,
                                        IP_NULL, ifname);
    }
    if (devname != IP_NULL)
        plink->devname = ipcom_strdup(devname);
    plink->sv[0] = plink->sv[1] = -1;

    /* Initialize PPP link */
    plink->index    = unit;
    plink->netif    = netif;
    plink->out_q_nr = 0;
    plink->fd       = -1;
    plink->baudrate = ipcom_sysvar_get_conf_as_int("ipppp.baudrate", IP_NULL, netif->name);
    if (plink->baudrate <= 0)
        plink->baudrate = IPCOM_DRV_PPP_BAUDRATE;

    /* Attach the interface. */
    if (ipcom_if_attach(netif) < 0)
        goto fail;

    /* Success. */
    return IPCOM_SUCCESS;

 fail:
    if (pdrv != IP_NULL)
        ipcom_free(pdrv);
    ipcom_if_free(netif);
    return IPCOM_ERR_FAILED;
}


/*
 *===========================================================================
 *                    ipcom_drv_ppp_create_cb
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipcom_drv_ppp_create_cb(const char *name, const char *value, int flags, void *cookie)
{
    char   ifname[IP_IFNAMSIZ];
    char  *strptr;

    (void)flags;
    (void)cookie;

    if (ipcom_strstr(name, ".devname") == IP_NULL)
        return 0;

    /* Copy out interface name */
    ipcom_strncpy(ifname, name + 9, sizeof(ifname) - 1);
    strptr = ipcom_strchr(ifname, '.');
    if (strptr == IP_NULL)
        return 0;
    *strptr = '\0';

    /* Initialize PPP interface */
    (void)ipcom_drv_ppp_if_init(ifname, value, 0);

    return 0;
}


/*
 ****************************************************************************
 * 10                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipcom_drv_ppp_create
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipcom_drv_ppp_create(void)
{
    (void)ipcom_sysvar_for_each("ipcom.if.",
                                ipcom_drv_ppp_create_cb,
                                IP_NULL);
    return IPCOM_SUCCESS;
}

#else
int ipcom_drv_ppp_empty_file;
#endif /* defined(IPPPP) && defined(IPCOM_SOCK) */


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
