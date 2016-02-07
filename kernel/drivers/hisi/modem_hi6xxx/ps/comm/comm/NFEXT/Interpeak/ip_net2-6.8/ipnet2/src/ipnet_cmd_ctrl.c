/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_cmd_ctrl.c,v $ $Revision: 1.81 $
 *     $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_cmd_ctrl.c,v $
 *     $Author: kenneth $
 *     $State: Exp $ $Locker:  $
 *
 *     INTERPEAK_COPYRIGHT_STRING
 *     Design and implementation by Kenneth Jonsson <kenneth@interpeak.se>
 ******************************************************************************
 */


/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 * Prints information about internal structures in IPNET.
 */

/*
 ****************************************************************************
 * 2                    CONFIGURATION
 ****************************************************************************
 */

#ifdef IPCOM_KERNEL
#undef IPCOM_KERNEL
#endif
#include "ipnet_config.h"


/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#define IPCOM_USE_CLIB_PROTO
#include <ipcom_clib.h>
#include <ipcom_file.h>
#include <ipcom_getopt.h>

#include "ipnet.h"
#include "ipnet_cmd.h"
#include "ipnet_h.h"
#include "ipnet_timeout.h"

#ifdef IPTCP
#include <iptcp.h>
#endif


#ifdef IPNET_DEBUG

/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

#define IPNET_CTRL_PRINT_BUF_SIZE      30000

/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

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

/*
 ****************************************************************************
 * 8                    DATA
 ****************************************************************************
 */

#define IPCOM_PKT_ID_UNINITIALIZED 0xffff

IP_STATIC Ipcom_cmd_int_str_map ipnet_ctrl_id_map[] =
{
    { IPCOM_PKT_ID_INIT,             "Init" },
    { IPCOM_PKT_ID_UNINITIALIZED,    "Uninitialized" },
    { IPCOM_PKT_ID_MALLOC,           "Just allocated" },
    { IPCOM_PKT_ID_RX,               "Driver input irq" },
    { IPCOM_PKT_ID_TX,               "Driver output" },
    { IPCOM_PKT_ID_FREEQ,            "Free queue" },
    { IPCOM_PKT_ID_DELREF,           "Delete ref" },
    { IPCOM_PKT_ID_ADDREF,           "Add ref" },
    { IPCOM_PKT_ID_INPUT,            "Pkt input" },
    { IPCOM_PKT_ID_INQ,              "Input queue" },
    { IPCOM_PKT_ID_OUTQ,             "Output queue" },
    { IPCOM_PKT_ID_NETIF_OUTQ,       "Netif output queue" },
    { IPCOM_PKT_ID_SOCKRECVQ,        "Recv. queue" },
    { IPCOM_PKT_ID_TCP,              "TCP" },
    { IPCOM_PKT_ID_TCP_INPUT,        "TCP input" },
    { IPCOM_PKT_ID_TCP_OUTPUT,       "TCP output" },
    { IPCOM_PKT_ID_TCP_SOCKOOORECVQ, "TCP out-of-order" },
    { IPCOM_PKT_ID_TCP_RETRANSQ,     "TCP retrans. queue" },
    { IPCOM_PKT_ID_IP4_INPUT,        "IPv4 input" },
    { IPCOM_PKT_ID_IP4_OUTPUT,       "IPv4 output" },
    { IPCOM_PKT_ID_IP4_REASSEMBLY,   "IPv4 reassembly" },
    { IPCOM_PKT_ID_IP6_INPUT,        "IPv6 input" },
    { IPCOM_PKT_ID_IP6_OUTPUT,       "IPv6 output" },
    { IPCOM_PKT_ID_IP6_REASSEMBLY,   "IPv6 reassembly" },
    { IPCOM_PKT_ID_REMOVE,           "Removed from queue" },
    { IPCOM_PKT_ID_IPSEC_INPUT,      "input ipsec" },
    { IPCOM_PKT_ID_IPSEC_OUTPUT,     "output ipsec(new)" },
    { IPCOM_PKT_ID_IPSEC_OUTPUT_ORG, "output ipsec(org)" },
    { IPCOM_PKT_ID_IPSEC_PF_KEY_IN,  "PF_KEY input" },
    { IPCOM_PKT_ID_PPP,              "PPP" },
    { IPCOM_PKT_ID_PPP_ORGOUT,       "PPP orgout" },
    { IPCOM_PKT_ID_PPP_INPUT,        "PPP input" },
    { IPCOM_PKT_ID_PPP_OUTPUT,       "PPP output" },
    { IPCOM_PKT_ID_RTSOCK,           "Route sock" },
    { IPCOM_PKT_ID_UDP_INPUT,        "UDP input" },
    { IPCOM_PKT_ID_NETIF_DEQUEUE,    "Dequeued from netif" },
    { IPCOM_PKT_ID_SOCKSENDQ,        "Send queue" },
    { IPCOM_PKT_ID_SENT_PIPE,        "Send to pipe" },
    { IPCOM_PKT_ID_RECV_PIPE,        "Received from pipe" },
    { IPCOM_PKT_ID_FRAG,             "New fragment" },
    { IPCOM_PKT_ID_PRE_SEND_SIG,     "Before SEND signature" },
    { IPCOM_PKT_ID_POST_SEND_SIG,    "After SEND signature" },
    { IPCOM_PKT_ID_PER_CPU_CACHE,    "Per-CPU cache" },
    { -1,                           IP_NULL }
};

#define IPNET_CTRL_INQ     (-2)    /* Must no be a valid socket or IP_SOCKERR */
#define IPNET_CTRL_CPU(cpu_ret) ((cpu_ret) < 10000 ? ((cpu_ret) + 10000) : -1)
#define IPNET_CTRL_CREATE_CPU_RETVAL(cpu) ((cpu) - 10000)

/*
 ****************************************************************************
 * 9                    LOCAL FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipnet_ctrl_verify_inq
 *===========================================================================
 * Description: Verifies that a packet can be found in the input queue.
 * Parameters:  pkt - Packet to find.
 * Returns:     IPNET_CTRL_INQ if the packet was found in the input queue,
 *              IP_SOCKERR if it was not found.
 *
 */
IP_STATIC int
ipnet_ctrl_verify_inq(Ipcom_pkt *pkt)
{
    Ipcom_pkt *ipkt;

    for (ipkt = ipnet->task.inq.head; ipkt != IP_NULL; ipkt = ipkt->next)
        if (ipkt == pkt)
            return IPNET_CTRL_INQ;
    return IP_SOCKERR;
}


/*
 *===========================================================================
 *                    ipnet_ctrl_verify_per_cpu_cache
 *===========================================================================
 * Description: Verify that this packet can be found in one of the per CPU
 *              caches.
 * Parameters:  pkt - packet to find
 * Returns:
 *
 */
IP_STATIC int
ipnet_ctrl_verify_per_cpu_cache(Ipcom_pkt *pkt)
{
#ifdef IPNET_USE_PER_CPU_PKT_CACHE
    Ipcom_pkt       *head[2];
    int              h;
    Ipcom_pkt       *ipkt;
    int              cpu;
    Ipnet_pkt_cache *cache;
    IPNET_PKT_CACHE_KEY_DECLARE(key)

    cpu = ipcom_num_configured_cpus();

    while (--cpu >= 0)
    {
        IPNET_PKT_CACHE_LOCK(cache, cpu, key);

        head[0] = cache->hdrs;
        head[1] = cache->pkts;

        for (h=0; h<2; h++)
            for (ipkt = head[h]; ipkt != IP_NULL; ipkt = ipkt->next)
                if (ipkt == pkt)
                {
                    IPNET_PKT_CACHE_UNLOCK(cache, key);
                    return IPNET_CTRL_CREATE_CPU_RETVAL(cpu);
                }

        IPNET_PKT_CACHE_UNLOCK(cache, key);
    }
#endif /* IPNET_USE_PER_CPU_PKT_CACHE */
    return IP_SOCKERR;
}


/*
 *===========================================================================
 *                    ipnet_ctrl_verify_reassembly
 *===========================================================================
 * Description: Verifies that a packet can be found in the input queue.
 * Parameters:  pkt_list_head - Head of list
 *              pkt - Packet to find.
 * Returns:     IPNET_CTRL_INQ if the packet was found in the reassembly queue,
 *              IP_SOCKERR if it was not found.
 *
 */
IP_STATIC int
ipnet_ctrl_verify_reassembly(Ipcom_pkt *pkt_list_head, Ipcom_pkt *pkt)
{
    Ipcom_pkt *fragment_list;
    Ipcom_pkt *fragment;

    for (fragment_list = pkt_list_head; fragment_list != IP_NULL; fragment_list = fragment_list->next)
    {
        for (fragment = fragment_list; fragment != IP_NULL; fragment = fragment->next_fragment)
            if (fragment == pkt)
                return IPNET_CTRL_INQ;
    }
    return IP_SOCKERR;
}


/*
 *===========================================================================
 *                    ipnet_ctrl_verify_socksendq
 *===========================================================================
 * Description: Verify that the packet is located at one of the sockets
 *              send queue.
 * Parameters:  pkt - Packet to find.
 * Returns:     The socket descriptor where the packet is in send queue.
 *
 */
IP_STATIC Ip_fd
ipnet_ctrl_verify_socksendq(Ipcom_pkt *pkt)
{
    int            i;
    Iptcp_socket  *sock;

    for (i = 0; i < ipnet_conf_max_sockets; i++)
    {
        sock = ipstack->sockets[i];
        if (sock != IP_NULL && pkt == sock->snd_head)
            return sock->ipcom.fd;
    }
    return IP_SOCKERR;
}


/*
 *===========================================================================
 *                    ipnet_ctrl_verify_location
 *===========================================================================
 * Description: Verifies that the packet can be found in the location specified
 *              by the 'id' field.
 * Parameters:  pkt - Packet to find.
 *              str - Buffer to print the status.
 * Returns:     A description of the location status.
 *
 */
IP_STATIC IP_CONST char *
ipnet_ctrl_verify_location(Ipcom_pkt *pkt, char *str)
{
    Ip_fd fd;
    enum Ipcom_pkt_id id;

    id = IPCOM_GET_PKT_ID(pkt);
    switch (id)
    {
#ifdef IPTCP
    case IPCOM_PKT_ID_TCP_RETRANSQ:
        fd = iptcp_ctrl_verify_retransq(pkt);
        break;
    case IPCOM_PKT_ID_SOCKRECVQ:
        fd =  iptcp_ctrl_verify_sockrecvq(pkt);
        break;
    case IPCOM_PKT_ID_TCP_SOCKOOORECVQ:
        fd = iptcp_ctrl_verify_sockooorecvq(pkt);
        break;
#endif /* IPTCP */
#ifdef IPCOM_USE_INET
    case IPCOM_PKT_ID_IP4_REASSEMBLY:
        fd = ipnet_ctrl_verify_reassembly(ipnet->ip4.reassembly_list, pkt);
        break;
#endif /* IPCOM_USE_INET */
#ifdef IPCOM_USE_INET6
    case IPCOM_PKT_ID_IP6_REASSEMBLY:
        fd = ipnet_ctrl_verify_reassembly(ipnet->ip6.reassembly_list, pkt);
        break;
#endif /* IPCOM_USE_INET6 */
    case IPCOM_PKT_ID_SOCKSENDQ:
        fd = ipnet_ctrl_verify_socksendq(pkt);
        break;

    case IPCOM_PKT_ID_INIT:
    case IPCOM_PKT_ID_ADDREF:
    case IPCOM_PKT_ID_DELREF:
    case IPCOM_PKT_ID_UNINITIALIZED:
    case IPCOM_PKT_ID_RX:
    case IPCOM_PKT_ID_TX:
    case IPCOM_PKT_ID_INPUT:
    case IPCOM_PKT_ID_OUTQ:
    case IPCOM_PKT_ID_NETIF_OUTQ:
    case IPCOM_PKT_ID_TCP:
    case IPCOM_PKT_ID_TCP_INPUT:
    case IPCOM_PKT_ID_TCP_OUTPUT:
    case IPCOM_PKT_ID_IP4_INPUT:
    case IPCOM_PKT_ID_IP4_OUTPUT:
    case IPCOM_PKT_ID_IP6_INPUT:
    case IPCOM_PKT_ID_IP6_OUTPUT:
    case IPCOM_PKT_ID_REMOVE:
    case IPCOM_PKT_ID_IPSEC_INPUT:
    case IPCOM_PKT_ID_IPSEC_OUTPUT:
    case IPCOM_PKT_ID_IPSEC_OUTPUT_ORG:
    case IPCOM_PKT_ID_IPSEC_PF_KEY_IN:
    case IPCOM_PKT_ID_PPP:
    case IPCOM_PKT_ID_PPP_ORGOUT:
    case IPCOM_PKT_ID_PPP_INPUT:
    case IPCOM_PKT_ID_PPP_OUTPUT:
    case IPCOM_PKT_ID_RTSOCK:
    case IPCOM_PKT_ID_UDP_INPUT:
    case IPCOM_PKT_ID_NETIF_DEQUEUE:
    case IPCOM_PKT_ID_PRE_SEND_SIG:
    case IPCOM_PKT_ID_POST_SEND_SIG:
    case IPCOM_PKT_ID_SENT_PIPE:
    case IPCOM_PKT_ID_RECV_PIPE:
        /* Packet is not located in any list in this state */
        return ipcom_strcpy(str, "Can't check");
    case IPCOM_PKT_ID_INQ:
        fd = ipnet_ctrl_verify_inq(pkt);
        break;
    case IPCOM_PKT_ID_PER_CPU_CACHE:
        fd = ipnet_ctrl_verify_per_cpu_cache(pkt);
        break;
    case IPCOM_PKT_ID_FREEQ:
        /* This can only be caused by a bug! */
        return ipcom_strcpy(str, "BUG!");
    default:
        ipcom_sprintf(str, "Invalid id:%d", id);
        return str;
    }
    if (fd > 0)
        ipcom_sprintf(str, "Yes: sock=%d", fd);
    else if (fd == IP_SOCKERR)
        ipcom_strcpy(str, "No");
    else if (fd == IPNET_CTRL_INQ)
        ipcom_strcpy(str, "Yes");
    else if (IPNET_CTRL_CPU(fd) >= 0)
        ipcom_sprintf(str, "Yes: cpu=%d", IPNET_CTRL_CPU(fd));
    else
        ipcom_strcpy(str, "BUG!");
    return str;
}


/*
 *===========================================================================
 *                    ipnet_ctrl_print_allocated_packets
 *===========================================================================
 * Description: Prints information about allocated packets
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipnet_ctrl_print_allocated_packets(char *output)
{
    char           str[20];
    int            pos;
    Ipcom_pkt     *pkt;
    IP_CONST char *if_name;
    Ipnet_netif   *netif;
	char		  *buffer;

    IPNET_DATA_LOCK(ipnet->pkt_pool_lock);

    pos = ipcom_snprintf(output, IPNET_CTRL_PRINT_BUF_SIZE,
        "%-6s%-6s%-6s%-6s%-6s%-6s%-6s\n", "Pool", "Prio", "MTU", "Total", "Free", "Add", "Used");

    pos += ipcom_snprintf(output  + pos, IPNET_CTRL_PRINT_BUF_SIZE  - pos,
        "\n%-6s%-6s%-6s%-6s%-20s%-16s%-6s%-10s%-10s\n",
        "Start", "End", "Max", "Used", "Location", "Loc-verified", "If", "Flags", "Ptr");

    for (pkt = ipnet->allocated_pkt_list; pkt != IP_NULL; pkt = pkt->next_alloc)
    {
        netif = ipnet_if_indextonetif(IPCOM_VR_ANY, pkt->ifindex);
        if (netif == IP_NULL)
            if_name = "N/A";
        else
            if_name = netif->ipcom.name;
        pos += ipcom_snprintf(output + pos,
                              IPNET_CTRL_PRINT_BUF_SIZE - pos,
                              "%-6d%-6d%-6d%-6u%-20s%-16s%-6s%#08x  %p\n",
                              pkt->start,
                              pkt->end,
                              pkt->maxlen,
                              (unsigned int) pkt->num_allocated,
                              ipcom_cmd_key_to_str(ipnet_ctrl_id_map,
                                                   IPCOM_GET_PKT_ID(pkt),
                                                   IP_NULL,
                                                   0),
                              ipnet_ctrl_verify_location(pkt, str),
                              if_name,
                              (unsigned) pkt->flags,
                              (void*) pkt);
    }

    IPNET_DATA_UNLOCK(ipnet->pkt_pool_lock);

	buffer = ipcom_malloc(1001);
    buffer[1000] = 0;
	pos = 0;
	do
	{
		strncpy( buffer, output+pos, 1000 );
		pos += 1000;
		(void)ipcom_printf("%s", buffer);
	} while ( 1000 == strlen( buffer ) );
	ipcom_free( buffer );

    return 0;
}


/*
 *===========================================================================
 *                    ipnet_ctrl_print_packets
 *===========================================================================
 * Description: Prints information about allocated packets
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipnet_ctrl_print_packets(char *output)
{
    int        pos = 0;
    Ipcom_pkt *pkt;
	char	  *buffer;

    IPNET_DATA_LOCK(ipnet->pkt_pool_lock);

    for (pkt = ipnet->allocated_pkt_list; pkt != IP_NULL; pkt = pkt->next_alloc)
    {
		if ( pkt->num_allocated || IPCOM_PKT_GET_REF_COUNT(pkt) )
		{
            unsigned i;

			pos += ipcom_snprintf(output + pos,
								  IPNET_CTRL_PRINT_BUF_SIZE - pos,
								  "%-10p Allocated: %-6u Ref: %-3u\n",
								  (void *) pkt,
								  (unsigned int) pkt->num_allocated,
								  IPCOM_PKT_GET_REF_COUNT(pkt));
            for (i = (pkt->next_trace == 0 ? IPCOM_PKT_TRACE_BUFFER_LEN : pkt->next_trace) - 1;
                 i != pkt->next_trace;
                 i = (i == 0 ? IPCOM_PKT_TRACE_BUFFER_LEN : i) - 1)
            {
                Ipcom_pkt_trace_buffer *tb = &pkt->trace_buffer[i];

                pos += ipcom_snprintf(output + pos,
                                      IPNET_CTRL_PRINT_BUF_SIZE - pos,
                                      "\tPID: %u, ID: %-20s %s:%d\n",
                                      (unsigned) tb->pid,
                                      ipcom_cmd_key_to_str(ipnet_ctrl_id_map,
                                                           tb->id,
                                                           IP_NULL,
                                                           0),
                                      tb->file,
                                      tb->line);
            }
		}
    }

#ifdef FIX_ME_NEED_TO_GET_THE_FREE_PACKET_HEADERS_FROM_THE_SLAB_INSTEAD
    for (pkt = ipnet->pkt_hdr_list; pkt != IP_NULL; pkt = pkt->next)
    {
		if ( pkt->num_allocated || IPCOM_PKT_GET_REF_COUNT(pkt) )
		{
			pos += ipcom_snprintf(output + pos,
								  IPNET_CTRL_PRINT_BUF_SIZE - pos,
								  "%-10p %-20s Allocated: %-6u Ref: %-3u\n\t%-40s %-6d\n\t%-40s %-6d\n",
								  (void *) pkt,
								  ipcom_cmd_key_to_str(ipnet_ctrl_id_map, pkt->id, IP_NULL, 0),
								  (unsigned int) pkt->num_allocated,
								  IPCOM_PKT_GET_REF_COUNT(pkt),
								  pkt->alloc_file == IP_NULL ? "*" : pkt->alloc_file,
								  pkt->alloc_line,
								  pkt->free_file == IP_NULL ? "*" : pkt->free_file,
								  pkt->free_line);
		}
    }
#endif

    IPNET_DATA_UNLOCK(ipnet->pkt_pool_lock);

    /* This below instead of a simple printf coz printf can't handle
       that many characters */
	buffer = ipcom_malloc(1001);
    buffer[1000] = 0;
	pos = 0;
	do
	{
		strncpy(buffer, output+pos, 1000 );
		pos += 1000;
		(void)ipcom_printf("%s", buffer);
	} while ( 1000 == strlen( buffer ) );
	ipcom_free( buffer );

    return 0;
}


/*
 *===========================================================================
 *                    ipnet_ctrl_print_timeouts
 *===========================================================================
 * Description: Prints all running timeouts in IPNET.
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipnet_ctrl_print_timeouts(char *output)
{
    Ipnet_timeout **tmo_elems;
    int             i = 0;
    int             j;
    int             num_elem;
    int             offset = 0;
    Ipnet_cmd_tmo_handler_to_str *map;
    Ip_u32          msec;
    void           *suspend_handle;

    /* Call ipcom_stackd_event() to make sure the stack as a current
       view of the wall clock time. */
    suspend_handle = ipnet_suspend_stack();

    ipnet->msec_now = ipnet_msec_now();

    tmo_elems = ipcom_malloc((1 + ipcom_pqueue_size(ipnet->timeouts)) * sizeof(*tmo_elems));
    if (tmo_elems == IP_NULL)
    {
        ipnet_resume_stack(suspend_handle);
        ipcom_printf("Timeout dump failed: out of memory" IP_LF);
        goto cleanup;
    }

    while (IP_NULL != (tmo_elems[i++] = ipcom_pqueue_remove_next(ipnet->timeouts)))
        /* Do nothing */;
    num_elem = i - 1;
    for (i = 0; i < num_elem; i++)
    {
        msec = ipnet_timeout_msec_until(tmo_elems[i]);

        if (msec / (1000 * 60 * 60 * 2))
            /* More than 2 hours */
            (void)ipcom_snprintf(ipnet->log_buf, sizeof(ipnet->log_buf), "%5u hours   ",
                                 (unsigned) (msec / (1000 * 60 * 60)));
        else if (msec / (1000 * 60 * 2))
            /* More than 2 minutes */
            (void)ipcom_snprintf(ipnet->log_buf, sizeof(ipnet->log_buf), "%5u minutes ",
                                 (unsigned) (msec / (1000 * 60)));
        else if (msec / (1000 * 2))
            /* More than 2 seconds */
            (void)ipcom_snprintf(ipnet->log_buf, sizeof(ipnet->log_buf), "%5u seconds ",
                                 (unsigned) (msec / 1000));
        else
            (void)ipcom_snprintf(ipnet->log_buf, sizeof(ipnet->log_buf), "%5u millisec",
                                 (unsigned) (msec));

        offset += ipcom_snprintf(&output[offset],
                                 IPNET_CTRL_PRINT_BUF_SIZE - offset,
                                 "%s   ", ipnet->log_buf);

        for (j = 0, map = IP_NULL; map == IP_NULL && j < ipnet->tmo_map_size; j++)
            if (ipnet->tmo_map[j].key == tmo_elems[i]->handler)
                map = &ipnet->tmo_map[j];

        if (map != IP_NULL)
            offset += map->to_str_func(tmo_elems[i]->cookie, &output[offset], IPNET_CTRL_PRINT_BUF_SIZE - offset);
        else
            offset += ipcom_snprintf(&output[offset],
                                     IPNET_CTRL_PRINT_BUF_SIZE - offset,
                                     "Handler is at %p",
                                     tmo_elems[i]->handler);
        offset += ipcom_snprintf(&output[offset], IPNET_CTRL_PRINT_BUF_SIZE - offset, IP_LF);
    }

    for (i = 0; i < num_elem; i++)
        (void)ipcom_pqueue_insert(ipnet->timeouts, tmo_elems[i]);

    ipnet_resume_stack(suspend_handle);

	(void)ipcom_printf("%s", output);


cleanup:
    ipcom_free(tmo_elems);
    return 0;
}


/*
 *===========================================================================
 *                    ipnet_ctrl_print_route_entry
 *===========================================================================
 * Description: Prints an internal or leaf node in the route table.
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipnet_ctrl_print_route_entry(char *output, Ipcom_route *rtab, Ipcom_route_entry *rt)
{
    int                i;
    Ipcom_route_entry *h;

    for (h = rt; h->parent; h = h->parent)
        (void)ipcom_sprintf(output + ipcom_strlen(output), "       ");

    if (rt->on)
        (void)ipcom_sprintf(output + ipcom_strlen(output), "bit:%d", rt->bit);
    else
    {
        if (IP_BIT_ISSET(rt->flags, IPCOM_ROUTE_FLAG_INIT))
            (void)ipcom_sprintf(output + ipcom_strlen(output), "#key:");
        else
            (void)ipcom_sprintf(output + ipcom_strlen(output), " key:");

        for (i = 0; i < rtab->octet_count; i++)
            (void)ipcom_sprintf(output + ipcom_strlen(output), "%02x" , ((Ip_u8*)rt->key)[i]);
    }

    if (rt->mask)
    {
        (void)ipcom_sprintf(output + ipcom_strlen(output), " mask:");
        for (i = 0; i < rtab->octet_count; i++)
            (void)ipcom_sprintf(output + ipcom_strlen(output), "%02x", ((Ip_u8*)rt->mask)[i]);
    }

    if (rt->on == IP_NULL && IP_BIT_ISFALSE(rt->flags, IPCOM_ROUTE_FLAG_INIT))
    {
        Ipnet_route_entry *r = (Ipnet_route_entry *) rt;
        void              *a = IP_NULL;
        int                a_len;
        if (r->gateway != IP_NULL)
        {
            switch (r->gateway->sa_family)
            {
            case IP_AF_INET:
                a = &((struct Ip_sockaddr_in *)r->gateway)->sin_addr;
                a_len = 4;
                break;
            case IP_AF_INET6:
                a = &((struct Ip_sockaddr_in6 *)r->gateway)->sin6_addr;
                a_len = 16;
                break;
            case IP_AF_LINK:
                a = IP_SOCKADDR_DL_LLADDR(r->gateway);
                a_len = ((struct Ip_sockaddr_dl *)r->gateway)->sdl_alen;
                break;
            default:
                a_len = 0;
                break;
            }
            if (a_len)
            {
                (void)ipcom_sprintf(output + ipcom_strlen(output), " gw:");

                for (i = 0; i < a_len; i++)
                    (void)ipcom_sprintf(output + ipcom_strlen(output), "%02x", ((Ip_u8*)a)[i]);
            }
        }
    }

    ipcom_sprintf(output + ipcom_strlen(output), " flags:%08x", (unsigned) rt->flags);
    (void)ipcom_sprintf(output + ipcom_strlen(output), IP_LF);
}


/*
 *===========================================================================
 *                    ipnet_ctrl_inorder_route_dump
 *===========================================================================
 * Description: Walks through the entire route table and print every node.
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipnet_ctrl_inorder_route_dump(char *output, Ipcom_route *rtab, Ipcom_route_entry *rt)
{
    if (rt == IP_NULL)
        return;

    ipnet_ctrl_inorder_route_dump(output, rtab, rt->on);

    if (rt->on == IP_FALSE && IP_BIT_ISFALSE(rt->flags, IPCOM_ROUTE_FLAG_INIT))
        ipnet_ctrl_inorder_route_dump(output, rtab, &((Ipnet_route_entry *) rt)->narrow->hdr);

    ipnet_ctrl_print_route_entry(output, rtab, rt);

    if (rt->on == IP_FALSE && IP_BIT_ISFALSE(rt->flags, IPCOM_ROUTE_FLAG_INIT))
        ipnet_ctrl_inorder_route_dump(output, rtab, &((Ipnet_route_entry *) rt)->next->hdr);
    ipnet_ctrl_inorder_route_dump(output, rtab, rt->off);
}


/*
 *===========================================================================
 *                    ipnet_ctrl_dump_route_table
 *===========================================================================
 * Description: Prints all routes in a route table.
 * Parameters:
 * Returns:
 *
 */
#ifdef IPTESTENGINE
IP_GLOBAL int ipnet_ctrl_dump_route_table(char *output, int domain, Ip_u16 vrtab, Ip_u32 table);
IP_GLOBAL int
#else
IP_STATIC int
#endif
ipnet_ctrl_dump_route_table(char *output, int domain, Ip_u16 vrtab, Ip_u32 table)
{
    Ipcom_route *rtab;

    if (ipnet_route_get_rtab(domain, vrtab, table, &rtab) < 0)
    {
        ipcom_printf("Failed to get route table for domain %d, virtual router %d and table ID %d"IP_LF,
                     domain, vrtab, table);
        return -1;
    }
    output[0] = '\0';
    ipnet_ctrl_inorder_route_dump(output, rtab, rtab->top);
	(void)ipcom_printf("%s", output);
    return 0;
}


/*
 *===========================================================================
 *                    ipnet_ctrl_suspend_stack
 *===========================================================================
 * Description: Suspends the stack for the specified number of seconds
 * Parameters:  sec - number of seconds to suspend the stack.
 * Returns:
 *
 */
IP_STATIC void
ipnet_ctrl_suspend_stack(int sec)
{
    void *suspend_handle;

    suspend_handle = ipnet_suspend_stack();
    if (suspend_handle == IP_NULL)
        ipcom_printf("Stack could not resume\n");
    else
    {
        ipcom_sleep(sec);
        ipnet_resume_stack(suspend_handle);
    }
}


/*
 *===========================================================================
 *                    ipnet_ctrl_print_usage
 *===========================================================================
 * Description: Prints usage for the shell command.
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipnet_ctrl_print_usage(void)
{
    ipcom_printf("\n");
    ipcom_printf("NAME\n");
    ipcom_printf("     ipnet_ctrl - control functions for IPNET\n");
    ipcom_printf("         WARNING: this command will access datastructures internal to IPNET\n");
    ipcom_printf("         and does it in a way that might not work on all OS:es.\n");
    ipcom_printf("         This command is only debug tool for the stack and is only guaranteed\n");
    ipcom_printf("         to work on the user-mode Linux port\n");
    ipcom_printf("\n");
    ipcom_printf("SYNOPSIS\n");
    ipcom_printf("     ipnet_ctrl -p\n");
    ipcom_printf("     ipnet_ctrl -a\n");
    ipcom_printf("     ipnet_ctrl -t\n");
    ipcom_printf("     ipnet_ctrl -r <domain>:<virtrt>\n");
    ipcom_printf("\n");
    ipcom_printf("DESCRIPTION\n");
    ipcom_printf("     ipnet_ctrl is used to check the internal state of the IPNET stack.\n");
    ipcom_printf("\n");
    ipcom_printf("     -p\n"
                 "             List all allocated packets in the system.\n"
                 "     -a\n"
                 "             List all packets in the system.\n"
                 "     -t\n"
                 "             List all timeouts.\n"
                 "     -r <domain>[:<VR>[:<table>]]\n"
                 "             Dump the routing table.\n"
                 "     -s <sec>\n"
                 "             Suspend the stack for <sec> seconds\n"
                 "\n");
}


/*
 ****************************************************************************
 * 10                   PUBLIC FUNCTIONS
 ****************************************************************************
 */


/*
 *===========================================================================
 *                    ipnet_cmd_ctrl
 *===========================================================================
 * Description: Shell command to view internal structures.
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipnet_cmd_ctrl(int argc, char **argv)
{
    Ipcom_getopt opt;
    int          c;
    char        *output;
    int          ret = 0;
    int          domain;
    Ip_u16       vrtab = 0;
    Ip_u32       table = IPCOM_ROUTE_TABLE_DEFAULT;
    char        *s;
    char        *p;
    Ip_bool      print_usage = IP_TRUE;

    output = ipcom_malloc(IPNET_CTRL_PRINT_BUF_SIZE);
    if (output == IP_NULL)
    {
        ipcom_printf("Out of memory for display buffer\n");
        return -1;
    }
    output[0] = '\0';

    ipcom_getopt_clear_r(&opt);
    while ((c = ipcom_getopt_r(argc, argv, "patr:s:", &opt)) != -1)
    {
        print_usage = IP_FALSE;
        switch (c)
        {
        case 'p':
            ret = ipnet_ctrl_print_allocated_packets(output);
            break;
        case 'a':
            ret = ipnet_ctrl_print_packets(output);
            break;
        case 't':
            ret = ipnet_ctrl_print_timeouts(output);
            break;
        case 'r':
            print_usage = IP_TRUE;
            s = ipcom_strtok_r(opt.optarg, ":", &p);
            if (s == IP_NULL)
                goto print_usage;
            domain = ipcom_atoi(s);
            s = ipcom_strtok_r(IP_NULL, ":"IP_LF, &p);
            if (s != IP_NULL)
            {
                vrtab = (Ip_u16) ipcom_atoi(s);
                s = ipcom_strtok_r(IP_NULL, IP_LF, &p);
                if (s != IP_NULL)
                    table = (Ip_u32) ipcom_atoi(s);
            }
            ret = ipnet_ctrl_dump_route_table(output, domain, vrtab, table);
            print_usage = IP_FALSE;
            break;
        case 's':
            ipnet_ctrl_suspend_stack(ipcom_atoi(opt.optarg));
            break;
        default:
            print_usage = IP_TRUE;
            goto print_usage;
        }
    }

print_usage:
    ipcom_free(output);
    if (print_usage)
        ipnet_ctrl_print_usage();
    return ret;
}

#endif /* IPNET_DEBUG */

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

