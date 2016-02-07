/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_nat.c,v $ $Revision: 1.124 $
 *   $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_nat.c,v $
 *   $Author: ulf $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Markus Carlstedt <markus@interpeak.se>
 ******************************************************************************
 */

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 * The core of the NAT.
 */

/*
 ****************************************************************************
 * 2                    CONFIGURATION
 ****************************************************************************
 */

#ifndef IPCOM_KERNEL
#define IPCOM_KERNEL
#endif
#include "ipnet_config.h"


/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#ifdef IPNET_USE_NAT

#define IPCOM_USE_CLIB_PROTO
#include <ipcom_type.h>
#include <ipcom_clib.h>
#include <ipcom_sock.h>
#include <ipcom_sock2.h>
#include <ipcom_syslog.h>
#include <ipcom_shell.h>
#include <ipcom_err.h>

#include "ipnet.h"
#include "ipnet_h.h"
#include "ipnet_nat.h"
#include "ipnet_nat_h.h"

#include "../../InterpeakMntn/include/InterpeakMntn.h"

/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

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

IP_EXTERN Ipnet_nat_trigger example_trigger;


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



/*
 ****************************************************************************
 * 9                    STATIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipnet_nat_syslog_mapping
 *===========================================================================
 * Description: Logs information about a NAT mapping to syslog.
 * Parameters:  nat_mapping - pointer to the NAT mapping.
 * Returns:     nothing.
 */
#ifdef IPNET_DEBUG
IP_STATIC void
ipnet_nat_syslog_mapping(Ipnet_nat_mapping *nat_mapping)
{
    char src[64];
    char nat_addr[IP_INET_ADDRSTRLEN];
    char dst[IP_INET_ADDRSTRLEN];
    char protostr[16];
    char state[8];
    char buf[200];
    Ip_u16 sport, dport, nat_sport;

    ipcom_inet_ntop(IP_AF_INET, &nat_mapping->inner.nat_addr_n, nat_addr, sizeof(nat_addr));
    ipcom_inet_ntop(IP_AF_INET, &nat_mapping->inner.tuple.dst_n, dst, sizeof(dst));
    if(nat_mapping->rule->inner.map == 1 || nat_mapping->rule->inner.map_block == 1
        || nat_mapping->rule->inner.rdr == 1)
    {
        ipcom_inet_ntop(IP_AF_INET, &nat_mapping->inner.tuple.src_n, src, sizeof(src));
    }
    else
    {
#ifdef IPNET_USE_NAT_PT
        ipcom_inet_ntop(IP_AF_INET6, &nat_mapping->inner.tuple.pt_src, src, sizeof(src));
#else
        IP_PANIC();
        return;
#endif
    }

    if(nat_mapping->inner.inner_state == IPNET_NAT_MAPPING_CLO)
        ipcom_strcpy(state, "CLO/");
    else if(nat_mapping->inner.inner_state == IPNET_NAT_MAPPING_SYN)
        ipcom_strcpy(state, "SYN/");
    else if(nat_mapping->inner.inner_state == IPNET_NAT_MAPPING_EST)
        ipcom_strcpy(state, "EST/");
    else if(nat_mapping->inner.inner_state == IPNET_NAT_MAPPING_FIN)
        ipcom_strcpy(state, "FIN/");
    else
        ipcom_strcpy(state, "NUL/");

    if(nat_mapping->inner.outer_state == IPNET_NAT_MAPPING_CLO)
        ipcom_strcpy(&state[4], "CLO");
    else if(nat_mapping->inner.outer_state == IPNET_NAT_MAPPING_SYN)
        ipcom_strcpy(&state[4], "SYN");
    else if(nat_mapping->inner.outer_state == IPNET_NAT_MAPPING_EST)
        ipcom_strcpy(&state[4], "EST");
    else if(nat_mapping->inner.outer_state == IPNET_NAT_MAPPING_FIN)
        ipcom_strcpy(&state[4], "FIN");
    else
        ipcom_strcpy(&state[4], "NUL");


    if(nat_mapping->inner.tuple.fragoff == 0
        && (nat_mapping->inner.tuple.protocol == IP_IPPROTO_UDP
            || nat_mapping->inner.tuple.protocol == IP_IPPROTO_TCP
            || (nat_mapping->inner.tuple.protocol == IP_IPPROTO_ICMP
                && IPNET_NAT_IS_ICMP_REQUEST(nat_mapping->inner.tuple.icmp_type))))
    {
        sport = ip_ntohs(nat_mapping->inner.tuple.src_port_n);
        dport = ip_ntohs(nat_mapping->inner.tuple.dst_port_n);
        nat_sport = ip_ntohs(nat_mapping->inner.nat_port_n);
        ipcom_sprintf(buf, "%s:%d -> %s:%d proto %s (%s:%d) state %s\n"
        "-------------------------------------------------------------",
                src, sport, dst, dport, ipnet_nat_protocol_name(nat_mapping->inner.tuple.protocol, protostr, sizeof(protostr)),
                nat_addr, nat_sport, state);
    }
    else
    {
        if(nat_mapping->inner.tuple.fragoff == 0)
            ipcom_sprintf(buf, "%s -> %s proto %s (%s) state %s\n"
            "-------------------------------------------------------------",
                    src, dst, ipnet_nat_protocol_name(nat_mapping->inner.tuple.protocol, protostr, sizeof(protostr)),
                    nat_addr, state);
        else
            ipcom_sprintf(buf, "%s -> %s proto %s FRAG (%s) state %s\n"
            "-------------------------------------------------------------",
                    src, dst, ipnet_nat_protocol_name(nat_mapping->inner.tuple.protocol, protostr, sizeof(protostr)),
                    nat_addr, state);
    }

    IPCOM_LOG0(DEBUG, buf);
}
#endif


/*
 *===========================================================================
 *                    ipnet_nat_mapping_get
 *===========================================================================
 * Description: Retrieves a pointer to a NAT mapping in the linked list of
 *              mappings using its sequential number. Sequential numbering
 *              starts with zero at the first element in the list.
 * Parameters:  i - sequential number
 * Returns:     Pointer to the mapping or IP_NULL if not found.
 */
IP_STATIC Ipnet_nat_mapping *
ipnet_nat_mapping_get(int i)
{
    int         c;
    Ipcom_list *l;

    for (c = 0, l = ipcom_list_first(&ipnet->nat.head_mapping);
         l != IP_NULL;
         c++, l = ipcom_list_next(l))
    {
        if (c == i)
            return (Ipnet_nat_mapping*) l;
    }
    return IP_NULL;
}


/*
 *===========================================================================
 *                    ipnet_nat_get_id
 *===========================================================================
 * Description: Allocates a port from the pool of ports available for NAT
 *              port translation.
 * Parameters:  port_hi - end port number.
 *              port_lo - start port number.
 *              ports   - pointer to port array.
 * Returns:     The allocated port or -1 if unavailable.
 */
IP_STATIC Ip_s32
ipnet_nat_get_id(Ip_u16 port_hi, Ip_u16 port_lo, Ip_u32 *ports)
{
    Ip_u32 i, j, num_longs;
    Ip_s32 cnt = -1;

    ip_assert(ports != IP_NULL);
    num_longs = ((port_hi - port_lo)>>5) + 1;
    for(i=0; i<num_longs; i++)
    {
        if(ports[i] != 0xffffffff)
        {
            for(j=0; j<32; j++)
            {
                if(!IP_BIT_ISSET(ports[i], 1<<j))
                {
                    cnt = (i<<5) + j;
                    IP_BIT_SET(ports[i], 1<<j);
                    goto done;
                }
            }
        }
    }
done:
    if(cnt<0)
        return -1;

    cnt += port_lo;
    if(cnt > port_hi)
        return -1;
    else
        return cnt;
}


/*
 *===========================================================================
 *                    ipnet_nat_release_id
 *===========================================================================
 * Description: Return previously allocated port to the pool of ports
 *              available for NAT port translation.
 * Parameters:  port    - port to release.
 *              port_lo - start port number.
 *              ports   - pointer to port array.
 * Returns:     nothing.
 *
 */
IP_STATIC void
ipnet_nat_release_id(Ip_u16 port, Ip_u16 port_lo, Ip_u32 *ports)
{
    int id;

    ip_assert(ports != IP_NULL);
    id = (int)(port - port_lo);
    IP_BIT_CLR(ports[id>>5], 1<<(id%32));
}


/*
 *===========================================================================
 *                    ipnet_nat_clean_mapping
 *===========================================================================
 * Description: Deletes a NAT mapping. Cancels timeout and releases the port.
 * Parameters:  nat_mapping - pointer to the mapping.
 * Returns:     nothing.
 */
IP_STATIC void
ipnet_nat_clean_mapping(Ipnet_nat_mapping *nat_mapping)
{
    ipnet_timeout_cancel(nat_mapping->tmo);
    ipcom_list_remove(&nat_mapping->list);
    (void)ipcom_hash_remove(ipnet->nat.hash_out_port, nat_mapping);
    (void)ipcom_hash_remove(ipnet->nat.hash_out_noport, nat_mapping);
    (void)ipcom_hash_remove(ipnet->nat.hash_in_port, nat_mapping);
    (void)ipcom_hash_remove(ipnet->nat.hash_in_noport, nat_mapping);
    ip_assert(ipnet->nat.head_mapping.size > 0 ||
                (ipnet->nat.head_mapping.size == 0
                && ipnet->nat.hash_out_port->elem == 0
                && ipnet->nat.hash_out_noport->elem == 0
                && ipnet->nat.hash_in_port->elem == 0
                && ipnet->nat.hash_in_noport->elem == 0));
    if (nat_mapping->rule->ports != IP_NULL)
    {
        ip_assert(nat_mapping->inner.autoport == 0);
        ipnet_nat_release_id(ip_ntohs(nat_mapping->inner.nat_port_n), nat_mapping->rule->inner.port_lo, nat_mapping->rule->ports);
    }
    if (nat_mapping->inner.autoport != 0)
    {
        ip_assert(nat_mapping->rule->ports == IP_NULL);
        ipnet_nat_release_id(ip_ntohs(nat_mapping->inner.nat_port_n), ipnet->nat.inner.port_lo, ipnet->nat.ports);
    }
    ipcom_free(nat_mapping);
}


/*
 *===========================================================================
 *                    ipnet_nat_mapping_timeout
 *===========================================================================
 * Description: Timeout handler for expired mappings.
 * Parameters:  nat_mapping - pointer to the mapping.
 * Returns:     nothing.
 */
IP_STATIC void
ipnet_nat_mapping_timeout(Ipnet_nat_mapping *nat_mapping)
{
    Ip_u32 sec_now;

    sec_now = ipnet_sec_now();

    if (IPCOM_IS_GT(nat_mapping->inner.tmo_abs_sec, sec_now))
    {
        (void) ipnet_timeout_schedule((nat_mapping->inner.tmo_abs_sec - sec_now) * 1000,
                                      (Ipnet_timeout_handler) ipnet_nat_mapping_timeout,
                                      nat_mapping,
                                      &nat_mapping->tmo);
        return;
    }
#ifdef IPNET_DEBUG
    IPCOM_LOG0(DEBUG, "ipnet_nat_mapping_timeout() :: Mapping expired");
    ipnet_nat_syslog_mapping(nat_mapping);
#endif
    ipnet_nat_clean_mapping(nat_mapping);
    ipnet->nat.inner.stats.mappings_expired++;
}


/*
 *===========================================================================
 *                    ipnet_nat_mapping_to_string
 *===========================================================================
 * Description: Helper function called when timeouts are listed with IPNET.
 * Parameters:  nat_mapping - pointer to the NAT mapping.
 *              buf         - pointer to buffer for printing info.
 *              buflen      - length of buffer.
 * Returns:     The number of bytes written.
 */
#ifdef IPNET_DEBUG
IP_STATIC int
ipnet_nat_mapping_to_string(Ipnet_nat_mapping *nat_mapping, char *buf, int buf_len)
{
    (void)nat_mapping;
    (void)buf;
    (void)buf_len;
    return ipcom_snprintf(buf, buf_len, "NAT mapping. Use \"nat -m\" for details.");
}
#endif


/*
 *===========================================================================
 *                    ipnet_nat_obj_hash_out_port
 *===========================================================================
 * Description: Hash value compute function for the object in the output port
 *              hash table.
 * Parameters:  nat_mapping - pointer to the NAT mapping.
 * Returns:     The hash value.
 */
IP_STATIC Ip_u32
ipnet_nat_obj_hash_out_port(Ipnet_nat_mapping *nat_mapping)
{
    Ip_u32 initval;

    initval = ipcom_hash_update(&nat_mapping->inner.tuple.protocol, sizeof(nat_mapping->inner.tuple.protocol), 0);
    initval = ipcom_hash_update(&nat_mapping->inner.tuple.dst_n, sizeof(nat_mapping->inner.tuple.dst_n), initval);
    initval = ipcom_hash_update(&nat_mapping->inner.tuple.dst_port_n, sizeof(nat_mapping->inner.tuple.dst_port_n), initval);
    initval = ipcom_hash_update(&nat_mapping->inner.tuple.src_n, sizeof(nat_mapping->inner.tuple.src_n), initval);
    return ipcom_hash_update(&nat_mapping->inner.tuple.src_port_n, sizeof(nat_mapping->inner.tuple.src_port_n), initval);
}


/*
 *===========================================================================
 *                    ipnet_nat_key_hash_out_port
 *===========================================================================
 * Description: Hash value compute function for the search key in the output
 *              port hash table.
 * Parameters:  nat_key - pointer to the search key.
 * Returns:     The hash value.
 */
IP_STATIC Ip_u32
ipnet_nat_key_hash_out_port(Ipnet_nat_hash_key *nat_key)
{
    Ip_u32 initval;

    initval = ipcom_hash_update(&nat_key->protocol, sizeof(nat_key->protocol), 0);
    initval = ipcom_hash_update(&nat_key->dst_n, sizeof(nat_key->dst_n), initval);
    initval = ipcom_hash_update(&nat_key->dst_port_n, sizeof(nat_key->dst_port_n), initval);
    initval = ipcom_hash_update(&nat_key->src_n, sizeof(nat_key->src_n), initval);
    return ipcom_hash_update(&nat_key->src_port_n, sizeof(nat_key->src_port_n), initval);
}


/*
 *===========================================================================
 *                    ipnet_nat_cmp_hash_out_port
 *===========================================================================
 * Description: Compare function for objects and keys in the output port
 *              hash table.
 * Parameters:  nat_mapping - pointer to the NAT mapping.
 *              nat_key     - pointer to th search key.
 * Returns:     IP_TRUE if equal IP_FALSE if not.
 */
IP_STATIC Ip_bool
ipnet_nat_cmp_hash_out_port(Ipnet_nat_mapping *nat_mapping, Ipnet_nat_hash_key *nat_key)
{
    return nat_mapping->inner.tuple.protocol == nat_key->protocol
            && nat_mapping->inner.tuple.dst_n == nat_key->dst_n
            && nat_mapping->inner.tuple.dst_port_n == nat_key->dst_port_n
            && nat_mapping->inner.tuple.src_n == nat_key->src_n
            && nat_mapping->inner.tuple.src_port_n == nat_key->src_port_n ? IP_TRUE : IP_FALSE;
}


/*
 *===========================================================================
 *                    ipnet_nat_obj_hash_out_noport
 *===========================================================================
 * Description: Hash value compute function for the object in the output
 *              no-port hash table.
 * Parameters:  nat_mapping - pointer to the NAT mapping.
 * Returns:     The hash value.
 */
IP_STATIC Ip_u32
ipnet_nat_obj_hash_out_noport(Ipnet_nat_mapping *nat_mapping)
{
    Ip_u32 initval;

    initval = ipcom_hash_update(&nat_mapping->inner.tuple.protocol, sizeof(nat_mapping->inner.tuple.protocol), 0);
    initval = ipcom_hash_update(&nat_mapping->inner.tuple.dst_n, sizeof(nat_mapping->inner.tuple.dst_n), initval);
    return ipcom_hash_update(&nat_mapping->inner.tuple.src_n, sizeof(nat_mapping->inner.tuple.src_n), initval);
}


/*
 *===========================================================================
 *                    ipnet_nat_key_hash_out_noport
 *===========================================================================
 * Description: Hash value compute function for the search key in the output
 *              no-port hash table.
 * Parameters:  nat_key - pointer to the search key.
 * Returns:     The hash value.
 */
IP_STATIC Ip_u32
ipnet_nat_key_hash_out_noport(Ipnet_nat_hash_key *nat_key)
{
    Ip_u32 initval;

    initval = ipcom_hash_update(&nat_key->protocol, sizeof(nat_key->protocol), 0);
    initval = ipcom_hash_update(&nat_key->dst_n, sizeof(nat_key->dst_n), initval);
    return ipcom_hash_update(&nat_key->src_n, sizeof(nat_key->src_n), initval);
}


/*
 *===========================================================================
 *                    ipnet_nat_cmp_hash_out_noport
 *===========================================================================
 * Description: Compare function for objects and keys in the output no-port
 *              hash table.
 * Parameters:  nat_mapping - pointer to the NAT mapping.
 *              nat_key     - pointer to th search key.
 * Returns:     IP_TRUE if equal IP_FALSE if not.
 */
IP_STATIC Ip_bool
ipnet_nat_cmp_hash_out_noport(Ipnet_nat_mapping *nat_mapping, Ipnet_nat_hash_key *nat_key)
{
    return nat_mapping->inner.tuple.protocol == nat_key->protocol
            && nat_mapping->inner.tuple.dst_n == nat_key->dst_n
            && nat_mapping->inner.tuple.src_n == nat_key->src_n ? IP_TRUE : IP_FALSE;
}


/*
 *===========================================================================
 *                    ipnet_nat_obj_hash_in_port
 *===========================================================================
 * Description: Hash value compute function for the object in the input port
 *              hash table.
 * Parameters:  nat_mapping - pointer to the NAT mapping.
 * Returns:     The hash value.
 */
IP_STATIC Ip_u32
ipnet_nat_obj_hash_in_port(Ipnet_nat_mapping *nat_mapping)
{
    Ip_u32 initval;

    initval = ipcom_hash_update(&nat_mapping->inner.tuple.protocol, sizeof(nat_mapping->inner.tuple.protocol), 0);
    initval = ipcom_hash_update(&nat_mapping->inner.tuple.dst_n, sizeof(nat_mapping->inner.tuple.dst_n), initval);
    initval = ipcom_hash_update(&nat_mapping->inner.tuple.dst_port_n, sizeof(nat_mapping->inner.tuple.dst_port_n), initval);
    initval = ipcom_hash_update(&nat_mapping->inner.nat_addr_n, sizeof(nat_mapping->inner.nat_addr_n), initval);
    return ipcom_hash_update(&nat_mapping->inner.nat_port_n, sizeof(nat_mapping->inner.nat_port_n), initval);
}


/*
 *===========================================================================
 *                    ipnet_nat_key_hash_in_port
 *===========================================================================
 * Description: Hash value compute function for the search key in the input
 *              port hash table.
 * Parameters:  nat_key - pointer to the search key.
 * Returns:     The hash value.
 */
IP_STATIC Ip_u32
ipnet_nat_key_hash_in_port(Ipnet_nat_hash_key *nat_key)
{
    Ip_u32 initval;

    initval = ipcom_hash_update(&nat_key->protocol, sizeof(nat_key->protocol), 0);
    initval = ipcom_hash_update(&nat_key->dst_n, sizeof(nat_key->dst_n), initval);
    initval = ipcom_hash_update(&nat_key->dst_port_n, sizeof(nat_key->dst_port_n), initval);
    initval = ipcom_hash_update(&nat_key->nat_addr_n, sizeof(nat_key->nat_addr_n), initval);
    return ipcom_hash_update(&nat_key->nat_port_n, sizeof(nat_key->nat_port_n), initval);
}


/*
 *===========================================================================
 *                    ipnet_nat_cmp_hash_in_port
 *===========================================================================
 * Description: Compare function for objects and keys in the input port
 *              hash table.
 * Parameters:  nat_mapping - pointer to the NAT mapping.
 *              nat_key     - pointer to th search key.
 * Returns:     IP_TRUE if equal IP_FALSE if not.
 */
IP_STATIC Ip_bool
ipnet_nat_cmp_hash_in_port(Ipnet_nat_mapping *nat_mapping, Ipnet_nat_hash_key *nat_key)
{
    return nat_mapping->inner.tuple.protocol == nat_key->protocol
            && nat_mapping->inner.tuple.dst_n == nat_key->dst_n
            && nat_mapping->inner.tuple.dst_port_n == nat_key->dst_port_n
            && nat_mapping->inner.nat_addr_n == nat_key->nat_addr_n
            && nat_mapping->inner.nat_port_n == nat_key->nat_port_n ? IP_TRUE : IP_FALSE;
}


/*
 *===========================================================================
 *                    ipnet_nat_obj_hash_in_noport
 *===========================================================================
 * Description: Hash value compute function for the object in the input
 *              no-port hash table.
 * Parameters:  nat_mapping - pointer to the NAT mapping.
 * Returns:     The hash value.
 */
IP_STATIC Ip_u32
ipnet_nat_obj_hash_in_noport(Ipnet_nat_mapping *nat_mapping)
{
    Ip_u32 initval;

    initval = ipcom_hash_update(&nat_mapping->inner.tuple.protocol, sizeof(nat_mapping->inner.tuple.protocol), 0);
    initval = ipcom_hash_update(&nat_mapping->inner.tuple.dst_n, sizeof(nat_mapping->inner.tuple.dst_n), initval);
    return ipcom_hash_update(&nat_mapping->inner.nat_addr_n, sizeof(nat_mapping->inner.nat_addr_n), initval);
}


/*
 *===========================================================================
 *                    ipnet_nat_key_hash_in_noport
 *===========================================================================
 * Description: Hash value compute function for the search key in the input
 *              no-port hash table.
 * Parameters:  nat_key - pointer to the search key.
 * Returns:     The hash value.
 */
IP_STATIC Ip_u32
ipnet_nat_key_hash_in_noport(Ipnet_nat_hash_key *nat_key)
{
    Ip_u32 initval;

    initval = ipcom_hash_update(&nat_key->protocol, sizeof(nat_key->protocol), 0);
    initval = ipcom_hash_update(&nat_key->dst_n, sizeof(nat_key->dst_n), initval);
    return ipcom_hash_update(&nat_key->nat_addr_n, sizeof(nat_key->nat_addr_n), initval);
}


/*
 *===========================================================================
 *                    ipnet_nat_cmp_hash_in_noport
 *===========================================================================
 * Description: Compare function for objects and keys in the input no-port
 *              hash table.
 * Parameters:  nat_mapping - pointer to the NAT mapping.
 *              nat_key     - pointer to th search key.
 * Returns:     IP_TRUE if equal IP_FALSE if not.
 */
IP_STATIC Ip_bool
ipnet_nat_cmp_hash_in_noport(Ipnet_nat_mapping *nat_mapping, Ipnet_nat_hash_key *nat_key)
{
    return nat_mapping->inner.tuple.protocol == nat_key->protocol
            && nat_mapping->inner.tuple.dst_n == nat_key->dst_n
            && nat_mapping->inner.nat_addr_n == nat_key->nat_addr_n ? IP_TRUE : IP_FALSE;
}


/*
 *===========================================================================
 *                    ipnet_nat_icmp_checksum
 *===========================================================================
 * Description: Updates ICMP checksum.
 * Parameters:  pkt       - pointer to the packet.
 *              icmp_old  - pointer to the old ICMP header.
 *              mod       - IP_TRUE if complete checksum calculation necessary.
 *                          IP_FALSE if smart update possible.
 * Returns:     nothing.
 */
IP_STATIC void
ipnet_nat_icmp_checksum(Ipcom_pkt *pkt, Ip_u16 orig_icmp_id, Ip_bool mod)
{
    Ipcom_ip_header *iph;
    Ipnet_pkt_icmp  *icmp;
    Ip_u16 hlen;

    iph  = (Ipcom_ip_header *)&pkt->data[pkt->ipstart];
    icmp = (Ipnet_pkt_icmp *)&pkt->data[pkt->start];
    if (mod == IP_TRUE)
    {
        /* Complete checksum calculation required */
        hlen = (Ip_u16)((iph->vers_len & 0x0f)<<2);
        icmp->cksum = 0;
        icmp->cksum = ipcom_in_checksum(icmp, (Ip_u16)(ip_ntohs(iph->length) - hlen));
    }
    else
    {
       if (IPNET_NAT_IS_ICMP_REQUEST(icmp->type) || IPNET_NAT_IS_ICMP_REPLY(icmp->type))
       {
            /* Smart update */
            if(icmp->data.echo.id != orig_icmp_id)
            {
                ipnet_nat_checksum_update((unsigned char *)&icmp->cksum,
                                          (unsigned char *)&orig_icmp_id,
                                          sizeof(orig_icmp_id),
                                          (unsigned char *)&icmp->data.echo.id,
                                          sizeof(icmp->data.echo.id));
            }
       }
    }
}


/*
 *===========================================================================
 *                    ipnet_nat_udp_checksum
 *===========================================================================
 * Description: Updates UDP checksum.
 * Parameters:  pkt       - pointer to the packet.
 *              old_src_n - old source address.
 *              old_dst_n - old destination address.
 *              udp_old   - pointer to the old UDP header.
 *              mod       - IP_TRUE if complete checksum calculation necessary.
 *                          IP_FALSE if smart update possible.
 * Returns:     nothing.
 */
IP_STATIC void
ipnet_nat_udp_checksum(Ipcom_pkt *pkt, Ip_u32 old_src_n, Ip_u32 old_dst_n, Ipcom_udp_header *udp_old, Ip_bool mod)
{
    Ipcom_ip_header *iph;
    Ipcom_udp_header *udp;
    Ipnet_nat_pseudo_hdr pseudo_hdr;
    Ip_u16 hlen;

    if(udp_old->checksum != 0)
    {
        iph = (Ipcom_ip_header *)&pkt->data[pkt->ipstart];
        udp = (Ipcom_udp_header *)&pkt->data[pkt->start];
        if(mod == IP_TRUE)
        {
            /* Complete checksum calculation required */
            hlen = (Ip_u16)((iph->vers_len & 0x0f)<<2);
            pseudo_hdr.src_n = IP_GET_32ON16((void *)&iph->src.addr32);
            pseudo_hdr.dst_n = IP_GET_32ON16((void *)&iph->dst.addr32);
            pseudo_hdr.mbz = 0;
            pseudo_hdr.protocol = iph->protocol;
            pseudo_hdr.length_n = (Ip_u16)(ip_htons(ip_ntohs(iph->length) - hlen));

            udp->checksum = 0;
            udp->checksum = (Ip_u16)(~ipcom_in_checksum(&pseudo_hdr, sizeof(pseudo_hdr)));
 	        udp->checksum = ipcom_in_checksum(udp, ip_ntohs(pseudo_hdr.length_n));
        }
        else
        {
            /* Smart update */
            if(IP_GET_32ON16((void *)&iph->src.addr32) != old_src_n)
            {
                ipnet_nat_checksum_update((unsigned char *)&udp->checksum, (unsigned char *)&old_src_n, sizeof(old_src_n),
                (unsigned char *)&iph->src.addr32, sizeof(iph->src.addr32));
            }

            if(IP_GET_32ON16((void *)&iph->dst.addr32) != old_dst_n)
            {
                ipnet_nat_checksum_update((unsigned char *)&udp->checksum, (unsigned char *)&old_dst_n, sizeof(old_dst_n),
                (unsigned char *)&iph->dst.addr32, sizeof(iph->dst.addr32));
            }

            if(udp->src_port != udp_old->src_port)
            {
                ipnet_nat_checksum_update((unsigned char *)&udp->checksum, (unsigned char *)&udp_old->src_port, sizeof(udp_old->src_port),
                (unsigned char *)&udp->src_port, sizeof(udp->src_port));
            }

            if(udp->dst_port != udp_old->dst_port)
            {
                ipnet_nat_checksum_update((unsigned char *)&udp->checksum, (unsigned char *)&udp_old->dst_port, sizeof(udp_old->dst_port),
                (unsigned char *)&udp->dst_port, sizeof(udp->dst_port));
            }
        }
    }
}


/*
 *===========================================================================
 *                    ipnet_nat_tcp_checksum
 *===========================================================================
 * Description: Updates TCP checksum.
 * Parameters:  pkt       - pointer to the packet.
 *              old_src_n - old source address.
 *              old_dst_n - old destination address.
 *              udp_old   - pointer to the old TCP header.
 *              mod       - IP_TRUE if complete checksum calculation necessary.
 *                          IP_FALSE if smart update possible.
 * Returns:     nothing.
 */
IP_STATIC void
ipnet_nat_tcp_checksum(Ipcom_pkt *pkt, Ip_u32 old_src_n, Ip_u32 old_dst_n, Ipcom_tcp_header *tcp_old, Ip_bool mod)
{
    Ipcom_ip_header *iph;
    Ipcom_tcp_header *tcp;
    Ipnet_nat_pseudo_hdr pseudo_hdr;
    Ip_u16 hlen;

    iph = (Ipcom_ip_header *)&pkt->data[pkt->ipstart];
    tcp = (Ipcom_tcp_header *)&pkt->data[pkt->start];
    if(mod == IP_TRUE)
    {
        /* Complete checksum calculation required */
        hlen = (Ip_u16)((iph->vers_len & 0x0f)<<2);
        pseudo_hdr.src_n = IP_GET_32ON16((void *)&iph->src.addr32);
        pseudo_hdr.dst_n = IP_GET_32ON16((void *)&iph->dst.addr32);
        pseudo_hdr.mbz = 0;
        pseudo_hdr.protocol = iph->protocol;
        pseudo_hdr.length_n = (Ip_u16)(ip_htons(ip_ntohs(iph->length) - hlen));

        tcp->checksum = 0;
        tcp->checksum = (Ip_u16)(~ipcom_in_checksum(&pseudo_hdr, sizeof(pseudo_hdr)));
 	    tcp->checksum = ipcom_in_checksum(tcp, ip_ntohs(pseudo_hdr.length_n));
    }
    else
    {
        /* Smart update */
        if(IP_GET_32ON16((void *)&iph->src.addr32) != old_src_n)
        {
            ipnet_nat_checksum_update((unsigned char *)&tcp->checksum, (unsigned char *)&old_src_n, sizeof(old_src_n),
            (unsigned char *)&iph->src.addr32, sizeof(iph->src.addr32));
        }

        if(IP_GET_32ON16((void *)&iph->dst.addr32) != old_dst_n)
        {
            ipnet_nat_checksum_update((unsigned char *)&tcp->checksum, (unsigned char *)&old_dst_n, sizeof(old_dst_n),
            (unsigned char *)&iph->dst.addr32, sizeof(iph->dst.addr32));
        }

        if(tcp->src_port != tcp_old->src_port)
        {
            ipnet_nat_checksum_update((unsigned char *)&tcp->checksum, (unsigned char *)&tcp_old->src_port, sizeof(tcp_old->src_port),
            (unsigned char *)&tcp->src_port, sizeof(tcp->src_port));
        }

        if(tcp->dst_port != tcp_old->dst_port)
        {
            ipnet_nat_checksum_update((unsigned char *)&tcp->checksum, (unsigned char *)&tcp_old->dst_port, sizeof(tcp_old->dst_port),
            (unsigned char *)&tcp->dst_port, sizeof(tcp->dst_port));
        }

        if(IP_GET_32ON16((void *)&tcp->seq_num) != IP_GET_32ON16((void *)&tcp_old->seq_num))
        {
            ipnet_nat_checksum_update((unsigned char *)&tcp->checksum, (unsigned char *)&tcp_old->seq_num, sizeof(tcp_old->seq_num),
            (unsigned char *)&tcp->seq_num, sizeof(tcp->seq_num));
        }

        if(IP_GET_32ON16((void *)&tcp->ack_num) != IP_GET_32ON16((void *)&tcp_old->ack_num))
        {
            ipnet_nat_checksum_update((unsigned char *)&tcp->checksum, (unsigned char *)&tcp_old->ack_num, sizeof(tcp_old->ack_num),
            (unsigned char *)&tcp->ack_num, sizeof(tcp->ack_num));
        }
    }
}


/*
 *===========================================================================
 *                    ipnet_nat_translate_embedded_icmp
 *===========================================================================
 * Description: Translate an embedded icmp error message.
 * Parameters:  incoming    - IP_TRUE for incoming packets.
 *              pkt         - pointer to the packet.
 *              nat_mapping - pointer to the NAT mapping.
 * Returns:     1 = Translated.
 *              0 = No translation.
 */
IP_STATIC Ip_s32
ipnet_nat_translate_embedded_icmp(Ip_bool incoming, Ipcom_pkt *pkt, Ipnet_nat_mapping *nat_mapping)
{
    Ipnet_pkt_icmp   *icmp;
    Ipnet_pkt_icmp   *icmp_embed;
    Ipcom_ip_header  *iph_embed;
    Ipcom_udp_header *udp_embed;
    Ipcom_tcp_header *tcp_embed;
    Ip_u16            hlen_embed, old_port_n, new_port_n;
    Ip_u32            old_n, new_n;

    icmp       = (Ipnet_pkt_icmp *)&pkt->data[pkt->start];
    iph_embed  = (Ipcom_ip_header *)icmp->data.failing_pkt.ip;
    hlen_embed = (Ip_u16)((iph_embed->vers_len & 0x0f)<<2);
    if (IPNET_NAT_IS_ICMP_ERROR(icmp->type) == IP_FALSE || icmp->type == IPCOM_ICMP_REDIRECT)
    {
        /* Skip re-direct mesage type */
        return 0;
    }

	/* Restore the embedded ip header */
    if(incoming == IP_TRUE)
    {
        old_n = IP_GET_32ON16((void *)&iph_embed->src.addr32);
        new_n = nat_mapping->inner.tuple.src_n;
        ipcom_memcpy(&iph_embed->src, &nat_mapping->inner.tuple.src_n, sizeof(iph_embed->src));
    }
    else
    {
        old_n = IP_GET_32ON16((void *)&iph_embed->dst.addr32);
        new_n = nat_mapping->inner.nat_addr_n;
        ipcom_memcpy(&iph_embed->dst, &nat_mapping->inner.nat_addr_n, sizeof(iph_embed->dst));
    }

    /* Recalculate embedded ip header checksum */
    iph_embed->checksum = 0;
    iph_embed->checksum = ipcom_in_checksum(iph_embed, hlen_embed);

	/* Restore the embedded port */
    if(iph_embed->protocol == IP_IPPROTO_UDP)
    {
        udp_embed = (Ipcom_udp_header *)((Ip_u8 *)iph_embed + hlen_embed);
        if(udp_embed->checksum != 0 && new_n != old_n)
        {
            ipnet_nat_checksum_update((unsigned char *)&udp_embed->checksum,
                                        (unsigned char *)&old_n, sizeof(old_n),
                                        (unsigned char *)&new_n, sizeof(new_n));
        }
        if(incoming == IP_TRUE)
        {
            old_port_n = udp_embed->src_port;
            new_port_n = nat_mapping->inner.tuple.src_port_n;
            udp_embed->src_port = nat_mapping->inner.tuple.src_port_n;
        }
        else
        {
            old_port_n = udp_embed->dst_port;
            new_port_n = nat_mapping->inner.nat_port_n;
            udp_embed->dst_port = nat_mapping->inner.nat_port_n;
        }

        if(udp_embed->checksum != 0 && new_port_n != old_port_n)
        {
            ipnet_nat_checksum_update((unsigned char *)&udp_embed->checksum,
                                        (unsigned char *)&old_port_n, sizeof(old_port_n),
                                        (unsigned char *)&new_port_n, sizeof(new_port_n));
        }
    }
    else if(iph_embed->protocol == IP_IPPROTO_TCP)
    {
        tcp_embed = (Ipcom_tcp_header *)((Ip_u8 *)iph_embed + hlen_embed);

        /* Check if packet contains enough data to update TCP checksum */
        if (pkt->end - pkt->start >= 8 + hlen_embed + IPCOM_TCP_HEADER_LENGTH)
        {
            if(new_n != old_n)
            {
                ipnet_nat_checksum_update((unsigned char *)&tcp_embed->checksum,
                                            (unsigned char *)&old_n, sizeof(old_n),
                                            (unsigned char *)&new_n, sizeof(new_n));
            }
        }

        if(incoming == IP_TRUE)
        {
            old_port_n = tcp_embed->src_port;
            new_port_n = nat_mapping->inner.tuple.src_port_n;
            tcp_embed->src_port = nat_mapping->inner.tuple.src_port_n;
        }
        else
        {
            old_port_n = tcp_embed->dst_port;
            new_port_n = nat_mapping->inner.nat_port_n;
            tcp_embed->dst_port = nat_mapping->inner.nat_port_n;
        }

        /* Check if packet contains enough data to update TCP checksum */
        if (pkt->end - pkt->start >= 8 + hlen_embed + IPCOM_TCP_HEADER_LENGTH)
        {
            if(new_port_n != old_port_n)
            {
                ipnet_nat_checksum_update((unsigned char *)&tcp_embed->checksum,
                                            (unsigned char *)&old_port_n, sizeof(old_port_n),
                                            (unsigned char *)&new_port_n, sizeof(new_port_n));
            }
        }
    }
    else if(iph_embed->protocol == IP_IPPROTO_ICMP)
    {
        icmp_embed = (Ipnet_pkt_icmp *)((Ip_u8 *)iph_embed + hlen_embed);
        if(IPNET_NAT_IS_ICMP_REQUEST(icmp_embed->type) || IPNET_NAT_IS_ICMP_REPLY(icmp_embed->type))
        {
            if(incoming == IP_TRUE)
            {
                old_port_n = icmp_embed->data.echo.id;
                new_port_n = nat_mapping->inner.tuple.icmp_id_n;
                icmp_embed->data.echo.id = nat_mapping->inner.tuple.icmp_id_n;
            }
            else
            {
                old_port_n = icmp_embed->data.echo.id;
                new_port_n = nat_mapping->inner.nat_port_n;
                icmp_embed->data.echo.id = nat_mapping->inner.nat_port_n;
            }

            if(new_port_n != old_port_n)
            {
                ipnet_nat_checksum_update((unsigned char *)&icmp_embed->cksum,
                                            (unsigned char *)&old_port_n, sizeof(old_port_n),
                                            (unsigned char *)&new_port_n, sizeof(new_port_n));
            }
        }
    }

    return 1;
}


/*
 *===========================================================================
 *                    ipnet_nat_translate_address
 *===========================================================================
 * Description: Translate an IPv4 address.
 * Parameters:  incoming - IP_TRUE for incoming packets.
 * Returns:     nothing.
 */
IP_STATIC void
ipnet_nat_translate_address(Ip_bool incoming, Ipcom_pkt *pkt, Ipnet_nat_mapping *nat_mapping)
{
    Ipcom_ip_header *iph;

    iph = (Ipcom_ip_header *)&pkt->data[pkt->ipstart];
    /* Translate the source/destination address */
    if(incoming == IP_FALSE)
        ipcom_memcpy(&iph->src, &nat_mapping->inner.nat_addr_n, sizeof(iph->src));
    else
        ipcom_memcpy(&iph->dst, &nat_mapping->inner.tuple.src_n, sizeof(iph->dst));
}


/*
 *===========================================================================
 *                    ipnet_nat_get_configuration
 *===========================================================================
 * Description: Get NAT configuration from sysvars.
 * Parameters:  none.
 * Returns:     IPCOM_SUCCESS
 *              IPCOM_ERR_FAILED
 */
IP_STATIC Ip_err
ipnet_nat_get_configuration(void)
{
    /* Get NAT configuration */
    ipnet->nat.inner.timeout_icmp  = ipcom_sysvar_get_as_int0("ipnet.nat.timeout.icmp", 5);
    ipnet->nat.inner.timeout_udp   = ipcom_sysvar_get_as_int0("ipnet.nat.timeout.udp", 60);
    ipnet->nat.inner.timeout_tcp   = ipcom_sysvar_get_as_int0("ipnet.nat.timeout.tcp", 432000);
    ipnet->nat.inner.timeout_other = ipcom_sysvar_get_as_int0("ipnet.nat.timeout.other", 60);
    ipnet->nat.inner.max_mappings  = ipcom_sysvar_get_as_int0("ipnet.nat.capacity.mappings", 1000);
    ipnet->nat.inner.port_lo       = ipcom_sysvar_get_as_int0("ipnet.nat.autoport.start", 29000);
    ipnet->nat.inner.port_hi       = ipcom_sysvar_get_as_int0("ipnet.nat.autoport.end", 29999);
    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    ipnet_nat_remove_mappings
 *===========================================================================
 * Description: Remove all mappings belonging to a rule or all mappings
 *              globally if the nat_rule parameter is zero.
 * Parameters:  nat_rule - pointer to the rule.
 * Returns:     nothing.
 */
IP_STATIC void
ipnet_nat_remove_mappings(Ipnet_nat_rule *nat_rule)
{
    Ipnet_nat_mapping *nat_mapping;
    int i = 0;

    while (IP_NULL != (nat_mapping = ipnet_nat_mapping_get(i)))
    {
	    if (nat_rule == IP_NULL || nat_rule == nat_mapping->rule)
            ipnet_nat_clean_mapping(nat_mapping);
        else
            ++i;
    }
}


/*
 *===========================================================================
 *                    ipnet_nat_internal_add_rule
 *===========================================================================
 * Description: Add a rule.
 * Parameters:  nat_rule   - pointer to the rule.
 *              rule_index - the index of the rule in the list of rules.
 * Returns:     0 = success
 *             -1 = fail
 */
IP_STATIC Ip_s32
ipnet_nat_internal_add_rule(struct Ipnet_nat_rule_inner *nat_rule_info, Ip_u32 rule_index)
{
    Ipnet_nat_rule *nat_rule_tmp, *nat_rule_new = IP_NULL;
    Ipnet_nat_proxy *nat_proxy;
    Ip_u32 i;
    Ip_s32 num_longs, ret = 0;

    if(ipnet->nat.head_rule.size >= IPNET_NAT_MAX_RULES)
    {
        ret = -1;
        goto exit;
    }

    /* Allocate space for the rule */
    nat_rule_new = ipcom_malloc(sizeof(*nat_rule_new));
    if(nat_rule_new == IP_NULL)
    {
        ret = -1;
        goto exit;
    }
    ipcom_memset(nat_rule_new, 0, sizeof(*nat_rule_new));
    ipcom_memcpy(&nat_rule_new->inner, nat_rule_info, sizeof(nat_rule_new->inner));

    /* Allocate space for portmap and icmpidmap nat ids */
    if(nat_rule_info->portmap == 1 || nat_rule_info->icmpidmap == 1)
    {
        num_longs = ((nat_rule_new->inner.port_hi - nat_rule_new->inner.port_lo)>>5) + 1;
        nat_rule_new->ports = ipcom_malloc(num_longs<<2);
        if(nat_rule_new->ports == IP_NULL)
        {
            ret = -1;
            goto exit;
        }
        ipcom_memset(nat_rule_new->ports, 0, num_longs<<2);
    }

    /* Find the proxy */
    if(nat_rule_new->inner.proxy == 1)
    {
        nat_proxy = IPCOM_LIST_FIRST(&ipnet->nat.head_proxy);
	    while(nat_proxy != IP_NULL)
	    {
            if(ipcom_strcmp(nat_proxy->inner.label, nat_rule_new->inner.proxy_label) == 0)
            {
                /* Check proxies on rdr rules */
                if(nat_rule_new->inner.rdr_tcpudp == 1 &&
                   (nat_proxy->inner.protocol == IP_IPPROTO_TCP ||
                    nat_proxy->inner.protocol == IP_IPPROTO_UDP))
                {
                    nat_rule_new->proxy_func = nat_proxy->func;
                    nat_rule_new->proxy_cookie = nat_proxy->cookie;
                    break;
                }
                /* Next check covers also proxies on "map" and "map-block" rules
                   since the rdr_tcpudp field is zero */
                else if(nat_rule_new->inner.rdr_tcpudp == 0 &&
                        nat_rule_new->inner.proxy_protocol == nat_proxy->inner.protocol)
                {
                    nat_rule_new->proxy_func = nat_proxy->func;
                    nat_rule_new->proxy_cookie = nat_proxy->cookie;
                    break;
                }
            }
            nat_proxy = IPCOM_LIST_NEXT(&nat_proxy->list_proxy);
        }

        if(nat_proxy == IP_NULL)
        {
            /* No matching proxy found */
            ret = -1;
            goto exit;
        }
    }

	nat_rule_tmp = IPCOM_LIST_FIRST(&ipnet->nat.head_rule);
    /* Insert last if no index given or empty list */
    if(rule_index == 0 || nat_rule_tmp == IP_NULL)
    {
        ipcom_list_insert_last(&ipnet->nat.head_rule, &nat_rule_new->list_rule);
        goto exit;
    }

    /* Walk through the list and insert in the right spot */
    i = 1;
	while(nat_rule_tmp != IP_NULL)
	{
	    if(rule_index <= i++)
	    {
            ipcom_list_insert_before(&nat_rule_tmp->list_rule, &nat_rule_new->list_rule);
            goto exit;
        }
        nat_rule_tmp = IPCOM_LIST_NEXT(&nat_rule_tmp->list_rule);
    }

    /* Insert at end */
    ipcom_list_insert_last(&ipnet->nat.head_rule, &nat_rule_new->list_rule);

exit:
    if(ret != 0 && nat_rule_new != IP_NULL)
    {
        if(nat_rule_new->ports != IP_NULL)
            ipcom_free(nat_rule_new->ports);
        ipcom_free(nat_rule_new);
    }
    return ret;
}


/*
 *===========================================================================
 *                    ipnet_nat_internal_remove_rule
 *===========================================================================
 * Description: Remove a rule.
 * Parameters:  nat_rule - pointer to the rule.
 * Returns:     0 = success
 *             -1 = fail
 */
IP_STATIC Ip_s32
ipnet_nat_internal_remove_rule(struct Ipnet_nat_rule_inner *nat_rule_info)
{
    Ipnet_nat_rule *nat_rule_tmp;

    nat_rule_tmp = IPCOM_LIST_FIRST(&ipnet->nat.head_rule);
	while(nat_rule_tmp != IP_NULL)
	{
        /* Compare only actual rule paramters */
	    if(!ipcom_memcmp(nat_rule_info, &nat_rule_tmp->inner, sizeof(nat_rule_tmp->inner)))
	    {
		    ipcom_list_remove(&nat_rule_tmp->list_rule);
            ipnet_nat_remove_mappings(nat_rule_tmp);
		    if(nat_rule_tmp->ports != IP_NULL)
		        ipcom_free(nat_rule_tmp->ports);
		    ipcom_free(nat_rule_tmp);
            return 0;
	    }
        nat_rule_tmp = IPCOM_LIST_NEXT(&nat_rule_tmp->list_rule);
	}

    return -1;
}


/*
 *===========================================================================
 *                    ipnet_nat_internal_flush_rules
 *===========================================================================
 * Description: Remove all rules.
 * Parameters:  none.
 * Returns:     0 = success
 *             -1 = fail
 */
IP_STATIC Ip_s32
ipnet_nat_internal_flush_rules(void)
{
    Ipnet_nat_rule *nat_rule;
    Ip_s32 ret = 0;

	while((nat_rule = IPCOM_LIST_FIRST(&ipnet->nat.head_rule)) != IP_NULL)
    {
        ret = ipnet_nat_internal_remove_rule(&nat_rule->inner);
        if(ret != 0)
        {
            IP_PANIC();
            break;
        }
    }

    return ret;
}


/*
 *===========================================================================
 *                    ipnet_nat_internal_get_rule
 *===========================================================================
 * Description: Get a rule from the list of rules.
 * Parameters:  ctrl - pointer to the IP_IP_NAT socket option structure.
 *              lenp - pointer to the length of the IP_IP_NAT structure.
 * Returns:     0 = success.
 *             -1 = fail.
 */
IP_STATIC Ip_s32
ipnet_nat_internal_get_rule(Ipnet_nat_ctrl *ctrl, Ip_socklen_t *lenp)
{
    Ipnet_nat_rule *nat_rule;
    Ip_s32 i;

    /* Get the specified entry */
    nat_rule = IPCOM_LIST_FIRST(&ipnet->nat.head_rule);
    for(i=0; i<ctrl->seqno && nat_rule != IP_NULL; i++)
        nat_rule = IPCOM_LIST_NEXT(&nat_rule->list_rule);

    /* Check if the entry existed */
    if(nat_rule == IP_NULL)
    {
        *lenp = 0;
        return 0;
    }

    ipcom_memcpy(&ctrl->type.rule_info, &nat_rule->inner, sizeof(nat_rule->inner));
    *lenp = sizeof(*ctrl);
    return 0;
}


/*
 *===========================================================================
 *                    ipnet_nat_internal_get_mapping
 *===========================================================================
 * Description: Get a mapping from the list of mappings.
 * Parameters:  ctrl - pointer to the IP_IP_NAT socket option structure.
 *              lenp - pointer to the length of the IP_IP_NAT structure.
 * Returns:     0 = success.
 *             -1 = fail.
 */
IP_STATIC Ip_s32
ipnet_nat_internal_get_mapping(Ipnet_nat_ctrl *ctrl, Ip_socklen_t *lenp)
{
    Ipnet_nat_mapping *nat_mapping;

    /* Get the specified entry (index starts at zero) */
    nat_mapping = ipnet_nat_mapping_get(ctrl->seqno);

    /* Check if the entry existed */
    if (nat_mapping == IP_NULL)
    {
        *lenp = 0;
        return 0;
    }

    if(nat_mapping->rule->inner.pt == 1 || nat_mapping->rule->inner.pt_block == 1)
        nat_mapping->inner.pt = 1;
    else
        nat_mapping->inner.pt = 0;
    ipcom_memcpy(&ctrl->type.mapping_info, &nat_mapping->inner, sizeof(nat_mapping->inner));
    *lenp = sizeof(*ctrl);
    return 0;
}


/*
 *===========================================================================
 *                    ipnet_nat_internal_get_proxy
 *===========================================================================
 * Description: Get a proxy from the list of proxies.
 * Parameters:  ctrl - pointer to the IP_IP_NAT socket option structure.
 *              lenp - pointer to the length of the IP_IP_NAT structure.
 * Returns:     0 = success.
 *             -1 = fail.
 */
IP_STATIC Ip_s32
ipnet_nat_internal_get_proxy(Ipnet_nat_ctrl *ctrl, Ip_socklen_t *lenp)
{
    Ipnet_nat_proxy *nat_proxy;
    Ip_s32 i;

    /* Get the specified entry */
    nat_proxy = IPCOM_LIST_FIRST(&ipnet->nat.head_proxy);
    for(i=0; i<ctrl->seqno && nat_proxy != IP_NULL; i++)
        nat_proxy = IPCOM_LIST_NEXT(&nat_proxy->list_proxy);

    /* Check if the entry existed */
    if(nat_proxy == IP_NULL)
    {
        *lenp = 0;
        return 0;
    }

    ipcom_memcpy(&ctrl->type.proxy_info, &nat_proxy->inner, sizeof(nat_proxy->inner));
    *lenp = sizeof(*ctrl);
    return 0;

}


/*
 *===========================================================================
 *                    ipnet_nat_internal_get_info
 *===========================================================================
 * Description: Get NAT info.
 * Parameters:  ctrl - pointer to the IP_IP_NAT socket option structure.
 *              lenp - pointer to the length of the IP_IP_NAT structure.
 * Returns:     0 = success.
 *             -1 = fail.
 */
IP_STATIC Ip_s32
ipnet_nat_internal_get_info(Ipnet_nat_ctrl *ctrl, Ip_socklen_t *lenp)
{
    ipnet->nat.inner.second = ipnet_sec_now();
    ipcom_memcpy(&ctrl->type.info, &ipnet->nat.inner, sizeof(ipnet->nat.inner));
    *lenp = sizeof(*ctrl);
    return 0;
}


/*
 *===========================================================================
 *                    ipnet_nat_flush_mappings
 *===========================================================================
 * Description: Remove all mappings.
 * Parameters:  none.
 * Returns:     0 = success.
 *             -1 = fail.
 */
IP_STATIC Ip_s32
ipnet_nat_internal_flush_mappings(void)
{
    /* Remove all mappings */
    ipnet_nat_remove_mappings(IP_NULL);
    return 0;
}


/*
 *===========================================================================
 *                    ipnet_nat_internal_clear_stats
 *===========================================================================
 * Description: Clear NAT statistics.
 * Parameters:  none.
 * Returns:     0 = success.
 *             -1 = fail.
 */
IP_STATIC Ip_s32
ipnet_nat_internal_clear_stats(void)
{
    ipcom_memset(&ipnet->nat.inner.stats, 0, sizeof(ipnet->nat.inner.stats));
    return 0;
}


/*
 *===========================================================================
 *                    ipnet_nat_internal_enable
 *===========================================================================
 * Description: Enable NAT.
 * Parameters:  none.
 * Returns:     0 = success.
 *             -1 = fail.
 */
IP_STATIC Ip_s32
ipnet_nat_internal_enable(void)
{
    int num_longs;

    if(ipnet->nat.inner.open == 1)
        return -1;     /* Already enabled */
    if(ipnet_nat_get_configuration() != IPCOM_SUCCESS)
        return -1;     /* Invalid configuration */

    /* Allocate space for the auto port array */
    if (ipnet->nat.ports != IP_NULL)
        ipcom_free(ipnet->nat.ports);   /* Free previous array if allocated */
    num_longs = ((ipnet->nat.inner.port_hi - ipnet->nat.inner.port_lo)>>5) + 1;
    ipnet->nat.ports = ipcom_malloc(num_longs<<2);
    if(ipnet->nat.ports == IP_NULL)
        return -1;      /* Not enough memory */
    ipcom_memset(ipnet->nat.ports, 0, num_longs<<2);

    ipnet->nat.inner.open = 1;
    return 0;
}


/*
 *===========================================================================
 *                    ipnet_nat_internal_disable
 *===========================================================================
 * Description: Disable NAT.
 * Parameters:  none.
 * Returns:     0 = success.
 *             -1 = fail.
 */
IP_STATIC Ip_s32
ipnet_nat_internal_disable(void)
{
    if(ipnet->nat.inner.open == 0)
        return -1;      /* Already disabled */
    (void)ipnet_nat_internal_flush_mappings();
    ipnet->nat.inner.open = 0;
    return 0;
}


/*
 *===========================================================================
 *                    ipnet_nat_assign_mapping
 *===========================================================================
 * Description: Assign new address and port to a mapping
 * Parameters:  incoming    - IP_TRUE for incoming packets.
 *              nat_mapping - pointer to the NAT mapping.
 *              nat_tuple   - pointer to the packet tuple.
 *              port        - pointer to boolean that indicates if mapping shall
 *                            be added to the port hash tables.
 * Returns:     0 = ok.
 *             -1 = fail.
 */
IP_STATIC int
ipnet_nat_assign_mapping(Ip_bool incoming, Ipnet_nat_mapping *nat_mapping, Ipnet_nat_tuple *nat_tuple, Ip_bool *port)
{
    Ip_s32 nat_id;
    Ipnet_netif *netif;

    *port = IP_FALSE;
    if(incoming == IP_FALSE)
    {
        /* Outgoing - The rule must be "map", "map-block", "pt" or "pt-block" */
        ip_assert(nat_mapping->rule->inner.map == 1 || nat_mapping->rule->inner.map_block == 1
                    || nat_mapping->rule->inner.pt == 1 || nat_mapping->rule->inner.pt_block == 1);

        /* Assign the NAT address */
        if(nat_mapping->rule->inner.nat_addr_n == 0 && nat_mapping->rule->inner.nat_mask_n == 0xffffffff)
        {
            /* 0/32 -> Get the address from the interface's address */
            ip_assert(nat_mapping->rule->inner.map == 1 || nat_mapping->rule->inner.map_block == 1);

            netif = ipnet_if_nametonetif(ipnet_get_effective_vr(), nat_mapping->rule->inner.ifname);
            if (netif == IP_NULL || netif->inet4_addr_list == IP_NULL)
            {
                IP_PANIC();
                return -1;
            }
            nat_mapping->inner.nat_addr_n = netif->inet4_addr_list->ipaddr_n;
        }
        else if(nat_mapping->rule->inner.nat_addr_n == 0 && nat_mapping->rule->inner.nat_mask_n == 0)
        {
            /* 0/0 -> Skip translating */
            nat_mapping->inner.nat_addr_n = nat_tuple->src_n;
        }
        else if(nat_mapping->rule->inner.map == 1)
        {
            /* "map" -> Use specified address */
            nat_mapping->inner.nat_addr_n = nat_mapping->rule->inner.nat_addr_n;
        }
        else if(nat_mapping->rule->inner.pt == 1)
        {
            /* "pt" -> Use specified address */
            nat_mapping->inner.nat_addr_n = nat_mapping->rule->inner.nat_addr_n;
        }
        else if(nat_mapping->rule->inner.pt_block == 1)
        {
#ifdef IPNET_USE_NAT_PT
            /* "pt-block" -> Static translation. Calculate an address */
            nat_mapping->inner.nat_addr_n = nat_mapping->rule->inner.nat_addr_n +
                                      (nat_tuple->src_n - nat_mapping->rule->inner.src_n_pt.in6.addr32[3]);
#endif
        }
        else if(nat_mapping->rule->inner.map_block == 1)
        {
            /* "map-block" -> Static translation. Calculate an address */
            nat_mapping->inner.nat_addr_n = nat_mapping->rule->inner.nat_addr_n +
                                      (nat_tuple->src_n - nat_mapping->rule->inner.src_n);
        }

        if(nat_tuple->protocol == IP_IPPROTO_UDP || nat_tuple->protocol == IP_IPPROTO_TCP)
        {
            /* Assign the NAT port */
            if(nat_mapping->rule->inner.portmap == 1)
            {
                if((nat_id = ipnet_nat_get_id(nat_mapping->rule->inner.port_hi, nat_mapping->rule->inner.port_lo, nat_mapping->rule->ports)) < 0)
                {
                    IPCOM_LOG0(ERR, "ipnet_nat_assign_mapping() :: Could not find available nat id");
                    return -1;
                }
                nat_mapping->inner.nat_port_n = ip_htons((Ip_u16)nat_id);
            }
            else if(nat_mapping->rule->inner.proxy == 1)
            {
                if (nat_mapping->rule->inner.proxy_nonapt == 1)
                {
                    nat_mapping->inner.nat_port_n = nat_tuple->src_port_n;
                }
                else
                {
                    if((nat_id = ipnet_nat_get_id(ipnet->nat.inner.port_hi, ipnet->nat.inner.port_lo, ipnet->nat.ports)) < 0)
                    {
                        IPCOM_LOG0(ERR, "ipnet_nat_assign_mapping() :: Could not find available automatic nat id");
                        return -1;
                    }
                    nat_mapping->inner.nat_port_n = ip_htons((Ip_u16)nat_id);
                    nat_mapping->inner.autoport = 1;
                }
            }
            else
            {
                nat_mapping->inner.nat_port_n = nat_tuple->src_port_n;
            }
            *port = IP_TRUE;
        }
        else if(nat_tuple->protocol == IP_IPPROTO_ICMP && IPNET_NAT_IS_ICMP_REQUEST(nat_tuple->icmp_type))
        {
            /* Assign the NAT port */
            if(nat_mapping->rule->inner.icmpidmap == 1)
            {
                if((nat_id = ipnet_nat_get_id(nat_mapping->rule->inner.port_hi, nat_mapping->rule->inner.port_lo, nat_mapping->rule->ports)) < 0)
                {
                    IPCOM_LOG0(ERR, "ipnet_nat_assign_mapping() :: Could not find available nat id");
                    return -1;
                }
                nat_mapping->inner.nat_port_n = ip_htons((Ip_u16)nat_id);
            }
            else
            {
                nat_mapping->inner.nat_port_n = nat_tuple->icmp_id_n;
            }

            /* Use icmp id as source port in hash tables */
            nat_mapping->inner.tuple.dst_port_n = 0;
            nat_mapping->inner.tuple.src_port_n = nat_tuple->icmp_id_n;
            *port = IP_TRUE;
        }
    }
    else
    {
        /* Incoming - The rule must be "rdr" */
        ip_assert(nat_mapping->rule->inner.rdr == 1);

        /* Reverse the addresses */
        nat_mapping->inner.tuple.dst_n = nat_tuple->src_n;
        if(nat_mapping->rule->inner.rdr == 1)
            nat_mapping->inner.tuple.src_n = nat_mapping->rule->inner.nat_addr_n;

        /* Assign the NAT address */
        nat_mapping->inner.nat_addr_n = nat_tuple->dst_n;

        if(nat_tuple->protocol == IP_IPPROTO_UDP || nat_tuple->protocol == IP_IPPROTO_TCP)
        {
            /* Reverse the ports */
            nat_mapping->inner.tuple.dst_port_n = nat_tuple->src_port_n;
            if (nat_mapping->rule->inner.rdr_tcpudp == 1 || nat_mapping->rule->inner.rdr_protocol != 0)
                nat_mapping->inner.tuple.src_port_n = nat_mapping->rule->inner.rdr_new_port_n;
            else
                nat_mapping->inner.tuple.src_port_n = nat_tuple->dst_port_n;

            /* Assign the NAT port */
            nat_mapping->inner.nat_port_n = nat_tuple->dst_port_n;
            *port = IP_TRUE;
        }
        else if(nat_tuple->protocol == IP_IPPROTO_ICMP && IPNET_NAT_IS_ICMP_REQUEST(nat_tuple->icmp_type))
        {
            /* Assign the NAT port */
            nat_mapping->inner.nat_port_n = nat_tuple->icmp_id_n;

            /* Use icmp id as source port in hash tables */
            nat_mapping->inner.tuple.dst_port_n = 0;
            nat_mapping->inner.tuple.src_port_n = nat_tuple->icmp_id_n;
            *port = IP_TRUE;
        }
    }

    return 0;
}


/*
 *===========================================================================
 *                    ipnet_nat_reset_mapping_timeout
 *===========================================================================
 * Description: Sets the timeout of a mapping.
 * Parameters:  mapping  - Pointer to the NAT mapping.
 * Returns:     nothing.
 */
IP_STATIC void
ipnet_nat_reset_mapping_timeout(Ipnet_nat_mapping *nat_mapping)
{
    Ip_bool reset_timer;
    Ip_u32 sec_now = ipnet_sec_now();

    ip_assert(nat_mapping->tmo);

    reset_timer = IPCOM_IS_LT(nat_mapping->inner.tmo_int_sec + sec_now, nat_mapping->inner.tmo_abs_sec);
    nat_mapping->inner.tmo_abs_sec = nat_mapping->inner.tmo_int_sec + sec_now;
    if (reset_timer)
    {
        ipnet_timeout_cancel(nat_mapping->tmo);
        (void) ipnet_timeout_schedule(nat_mapping->inner.tmo_int_sec * 1000,
                                      (Ipnet_timeout_handler) ipnet_nat_mapping_timeout,
                                      nat_mapping,
                                      &nat_mapping->tmo);
    }
}


/*
 *===========================================================================
 *                    ipnet_nat_assign_timeout
 *===========================================================================
 * Description: Assign timeouts to a mapping
 * Parameters:  incoming    - IP_TRUE for incoming packets.
 *              nat_mapping - pointer to the NAT mapping.
 *              nat_tuple   - pointer to the packet tuple.
 * Returns:     nothing.
 */
IP_STATIC void
ipnet_nat_assign_timeout(Ip_bool incoming,
                         Ipnet_nat_mapping *nat_mapping,
                         Ipnet_nat_tuple *nat_tuple,
                         Ip_u32 timeout)
{
    /* Assign the states and timeout */
    if(nat_tuple->protocol == IP_IPPROTO_TCP)
    {
        nat_mapping->inner.tmo_int_sec = ipnet->nat.inner.timeout_other;
        nat_mapping->inner.inner_state = IPNET_NAT_MAPPING_CLO;
        nat_mapping->inner.outer_state = IPNET_NAT_MAPPING_CLO;

        if (IP_BIT_ISSET(nat_tuple->tcp_flags, IPNET_NAT_TCPFLAG_SYN))
        {
            if(incoming == IP_TRUE)
                nat_mapping->inner.outer_state = IPNET_NAT_MAPPING_SYN;
            else
                nat_mapping->inner.inner_state = IPNET_NAT_MAPPING_SYN;
        }
    }
    else if(nat_tuple->protocol == IP_IPPROTO_UDP)
    {
        nat_mapping->inner.tmo_int_sec = ipnet->nat.inner.timeout_udp;
        nat_mapping->inner.inner_state = IPNET_NAT_MAPPING_EST;
        nat_mapping->inner.outer_state = IPNET_NAT_MAPPING_EST;
    }
    else if(nat_tuple->protocol == IP_IPPROTO_ICMP || nat_tuple->protocol == IP_IPPROTO_ICMPV6)
    {
        nat_mapping->inner.tmo_int_sec = ipnet->nat.inner.timeout_icmp;
        nat_mapping->inner.inner_state = IPNET_NAT_MAPPING_EST;
        nat_mapping->inner.outer_state = IPNET_NAT_MAPPING_EST;
    }
    else
    {
        nat_mapping->inner.tmo_int_sec = ipnet->nat.inner.timeout_other;
        nat_mapping->inner.inner_state = IPNET_NAT_MAPPING_EST;
        nat_mapping->inner.outer_state = IPNET_NAT_MAPPING_EST;
    }

    /* Set the timeout if other than default */
    if (timeout)
        nat_mapping->inner.tmo_int_sec = timeout;

    /* Add the timeout to the mapping */
    (void) ipnet_timeout_schedule(nat_mapping->inner.tmo_int_sec * 1000,
                                  (Ipnet_timeout_handler) ipnet_nat_mapping_timeout,
                                  nat_mapping,
                                  &nat_mapping->tmo);

    nat_mapping->inner.tmo_abs_sec = nat_mapping->inner.tmo_int_sec + ipnet_sec_now();
}


/*
 *===========================================================================
 *                    ipnet_nat_translate_icmp_id
 *===========================================================================
 * Description: Translate the ICMP id.
 * Parameters:  incoming    - IP_TRUE for incoming packets.
 *              pkt         - pointer to the packet.
 *              nat_mapping - pointer to the NAT mapping.
 * Returns:     nothing.
 */
IP_STATIC void
ipnet_nat_translate_icmp_id(Ip_bool incoming, Ipcom_pkt *pkt, Ipnet_nat_mapping *nat_mapping, Ip_u16 *orig_icmp_id)
{
    Ipnet_pkt_icmp *icmp;

    icmp = (Ipnet_pkt_icmp *)&pkt->data[pkt->start];
    if(IPNET_NAT_IS_ICMP_REQUEST(icmp->type) || IPNET_NAT_IS_ICMP_REPLY(icmp->type))
    {
        *orig_icmp_id = icmp->data.echo.id;
        if(incoming == IP_FALSE)
            icmp->data.echo.id = nat_mapping->inner.nat_port_n;
        else
            icmp->data.echo.id = nat_mapping->inner.tuple.icmp_id_n;
    }
}


/*
 *===========================================================================
 *                    ipnet_nat_translate
 *===========================================================================
 * Description: Translate a packet.
 * Parameters:  incoming    - IP_TRUE for incoming packets.
 *              nat_tuple   - pointer to packet tuple.
 *              nat_mapping - pointer to NAT mapping.
 * Returns:     IPNET_NAT_TRANSLATED
 *              IPNET_NAT_DROP
 */
IP_STATIC Ip_s32
ipnet_nat_translate(Ip_bool incoming, Ipcom_pkt **pktp, Ipnet_nat_tuple *nat_tuple, Ipnet_nat_mapping *nat_mapping)
{
    Ipcom_ip_header *iph;
    Ip_u16 hlen, orig_icmp_id = 0;
    Ip_s32 ret, delta = 0;
    Ip_u32 orig_src_n, orig_dst_n;          /* Original source and destination addresses */
    Ip_u8 orig_tcp_hdr[IPCOM_TCP_HEADER_LENGTH];
    Ip_u8 orig_udp_hdr[IPCOM_UDP_HEADER_LENGTH];
    Ip_bool mod = IP_FALSE;

    iph = (Ipcom_ip_header *)&(*pktp)->data[(*pktp)->ipstart];
    hlen = (Ip_u16)((iph->vers_len & 0x0f)<<2);

    /* Translate the address */
    orig_src_n = IP_GET_32ON16((void *)&iph->src.addr32);
    orig_dst_n = IP_GET_32ON16((void *)&iph->dst.addr32);
    ipnet_nat_translate_address(incoming, *pktp, nat_mapping);

    /* Translate the tcp/udp port */
    if(nat_tuple->fragoff == 0)
    {
        switch(iph->protocol)
        {
        case IP_IPPROTO_TCP:
            ipcom_memcpy(orig_tcp_hdr, &(*pktp)->data[(*pktp)->start], sizeof(orig_tcp_hdr));
            ipnet_nat_translate_tcpudp_port(incoming, *pktp, nat_mapping);
            break;
        case IP_IPPROTO_UDP:
            ipcom_memcpy(orig_udp_hdr, &(*pktp)->data[(*pktp)->start], sizeof(orig_udp_hdr));
            ipnet_nat_translate_tcpudp_port(incoming, *pktp, nat_mapping);
            break;
        case IP_IPPROTO_ICMP:
            ipnet_nat_translate_icmp_id(incoming, *pktp, nat_mapping, &orig_icmp_id);
            break;
        default:
            break;
        }
    }

    /* Call the application proxy */
    if(nat_mapping->proxy_func != IP_NULL && nat_tuple->protocol != IP_IPPROTO_ICMP)
    {
        ret = ipnet_nat_proxy(incoming, pktp, nat_tuple, nat_mapping, (int)hlen, &mod, &delta);
        if (ret < 0)
        {
            /* Drop packet */
            IPCOM_LOG1(INFO, "ipnet_nat_translate() :: Proxy %s dropped packet",
                       nat_mapping->rule->inner.proxy_label);
            if(incoming == IP_FALSE)
                ipnet->nat.inner.stats.dropped_out++;
            else
                ipnet->nat.inner.stats.dropped_in++;
            return IPNET_NAT_DROP;
        }
        iph = (Ipcom_ip_header *)&(*pktp)->data[(*pktp)->ipstart];
        hlen = (Ip_u16)((iph->vers_len & 0x0f)<<2);
    }

    /* Fix IP length */
    if(delta != 0)
    {
        Ip_u16 tmp;

        tmp = ip_ntohs(iph->length);
        tmp = (Ip_u16)((Ip_s32)tmp + delta);
        iph->length = ip_htons(tmp);
    }

    if(nat_tuple->fragoff == 0)
    {
        if(iph->protocol == IP_IPPROTO_TCP)
        {
            /* Save, translate and update the sequence number */
            if(delta != 0)
            {
                Ip_u32 seq_num;

                seq_num = IP_GET_32ON16((((Ip_u32 *)&(*pktp)->data[(*pktp)->start]))+1);
                seq_num = ip_ntohl(seq_num);
                ipnet_nat_translate_sequence_number(incoming, *pktp, nat_mapping);
                ipnet_nat_update_sequence_number(incoming, nat_mapping, seq_num, delta);
            }
            else
            {
                ipnet_nat_translate_sequence_number(incoming, *pktp, nat_mapping);
            }

            /* Translate acknowledgment number */
            ipnet_nat_translate_ack_number(incoming, *pktp, nat_mapping);

            /* Recalculate tcp checksum */
            ipnet_nat_tcp_checksum(*pktp, orig_src_n, orig_dst_n, (Ipcom_tcp_header *)orig_tcp_hdr, mod);

        }
        else if(iph->protocol == IP_IPPROTO_UDP)
        {
            /* Fix UDP length */
            if(delta != 0)
            {
                Ip_u16 tmp;

                tmp = *(((Ip_u16 *)&(*pktp)->data[(*pktp)->start])+2);
                tmp = ip_ntohs(tmp);
                tmp = (Ip_u16)((Ip_s32)tmp + delta);
                *(((Ip_u16 *)&(*pktp)->data[(*pktp)->start])+2) = ip_htons(tmp);
            }

            /* Recalculate udp checksum */
            ipnet_nat_udp_checksum(*pktp, orig_src_n, orig_dst_n, (Ipcom_udp_header *)orig_udp_hdr, mod);

        }
        else if(iph->protocol == IP_IPPROTO_ICMP)
        {
            /* Translate the icmp error embedded datagram */
            if (ipnet_nat_translate_embedded_icmp(incoming, *pktp, nat_mapping) != 0)
            {
                mod = IP_TRUE;
            }

            /* Recalculate icmp checksum */
            ipnet_nat_icmp_checksum(*pktp, orig_icmp_id, mod);
        }
    }

    /* Recalculate ip header checksum */
    iph->checksum = 0;
    iph->checksum = ipcom_in_checksum(iph, hlen);

    if(incoming == IP_FALSE)
        ipnet->nat.inner.stats.translated_out++;
    else
        ipnet->nat.inner.stats.translated_in++;

    return IPNET_NAT_TRANSLATED;
}


/*
 *===========================================================================
 *                    ipnet_nat_get_tuple
 *===========================================================================
 * Description: Populates a NAT tuple structure from an IPv4 packet.
 * Parameters:  pkt       - pointer to the packet.
 *              nat_tuple - pointer to the NAT tuple.
 * Returns:     0 = OK
 *             -1 = Cannot NAT
 */
IP_STATIC Ip_s32
ipnet_nat_get_tuple(Ipcom_pkt *pkt, Ipnet_nat_tuple *nat_tuple)
{
    Ipcom_ip_header *iph;
    Ipcom_ip_header *iph_embed;
    Ip_u16           hlen_embed;

    ip_assert(pkt);

    ipcom_memset(nat_tuple, 0, sizeof(*nat_tuple));

    /* Get pointer to the ip header */
    iph = (Ipcom_ip_header *)&pkt->data[pkt->ipstart];

    /* Get tos */
    nat_tuple->tos = iph->tos;

    /* Get ttl */
    nat_tuple->ttl = iph->ttl;

    /* Get fragment offset and flags */
    nat_tuple->fragoff = ip_ntohs(iph->frag) & 0x1fff;
    nat_tuple->fragmf = (Ip_u8)((ip_ntohs(iph->frag) & 0x2000)>>13);
    nat_tuple->fragid = (Ip_u32)ip_ntohs(iph->id);

    /* Get addr */
    nat_tuple->protocol = iph->protocol;
    ipcom_memcpy(&nat_tuple->src_n, &iph->src, sizeof(iph->src));
    ipcom_memcpy(&nat_tuple->dst_n, &iph->dst, sizeof(iph->dst));

    /* Check transport length */
    if (nat_tuple->fragoff == 0)
    {
        if (nat_tuple->protocol == IP_IPPROTO_ICMP)
        {
            Ipnet_pkt_icmp *icmp;

            if(pkt->end - pkt->start < IPCOM_ICMP_HEADER_LENGTH)
                return -1;    /* Too short */
            icmp = (Ipnet_pkt_icmp *)&pkt->data[pkt->start];
            nat_tuple->icmp_type = icmp->type;
            if (IPNET_NAT_IS_ICMP_REQUEST(icmp->type) || IPNET_NAT_IS_ICMP_REPLY(icmp->type))
            {
                if(pkt->end - pkt->start < IPNET_NAT_ICMP_QUERY_LENGTH)
                    return -1;    /* Too short */
                nat_tuple->icmp_id_n = icmp->data.echo.id;
            }
            else if (IPNET_NAT_IS_ICMP_ERROR(icmp->type))
            {
                iph_embed  = (Ipcom_ip_header *)icmp->data.failing_pkt.ip;
                hlen_embed = (Ip_u16)((iph_embed->vers_len & 0x0f)<<2);
                if (pkt->end - pkt->start < 8 + hlen_embed + 8)
                    return -1;    /* Too short */
            }
        }
        else if (nat_tuple->protocol == IP_IPPROTO_UDP)
        {
            Ipcom_udp_header *udp;

            udp = (Ipcom_udp_header *)&pkt->data[pkt->start];
            if(pkt->end - pkt->start < IPCOM_UDP_HEADER_LENGTH)
                return -1;    /* Too short */

            nat_tuple->src_port_n = udp->src_port;
            nat_tuple->dst_port_n = udp->dst_port;
        }
        else if (nat_tuple->protocol == IP_IPPROTO_TCP)
        {
            Ipcom_tcp_header *tcp;

            tcp = (Ipcom_tcp_header *)&pkt->data[pkt->start];
            if(pkt->end - pkt->start < IPCOM_TCP_HEADER_LENGTH)
                return -1;    /* Too short */

            nat_tuple->src_port_n = tcp->src_port;
            nat_tuple->dst_port_n = tcp->dst_port;
            nat_tuple->tcp_flags  = ip_ntohs(tcp->offset_flags) & 0x003f;
        }
    }

    return 0;
}


/*
 *===========================================================================
 *                    ipnet_nat_loop_rules
 *===========================================================================
 * Description: Loop through rules and match packets against mappings and
 *              rules.
 * Parameters:  incoming - IP_TRUE for incoming packets.
 *              no_rules - IP_TRUE if only mappings shall be matched.
 *              pktp     - pointer to pointer to packet.
 *              param    - pointer to the output parameters.
 * Returns:     IPNET_NAT_PT_TRANSLATED
 *              IPNET_NAT_TRANSLATED
 *              IPNET_NAT_NOMATCH
 *              IPNET_NAT_INVALID
 *              IPNET_NAT_DROP
 */
IP_STATIC Ip_s32
ipnet_nat_loop_rules(Ip_bool incoming, Ip_bool no_rules, Ipcom_pkt **pktp, void *param)
{
    Ipnet_nat_rule *nat_rule;
    Ipnet_nat_tuple nat_tuple;
    Ipnet_nat_mapping *nat_mapping;
    Ip_s32 ret, i = 0;

    /* Get the tuple */
    ret = ipnet_nat_get_tuple(*pktp, &nat_tuple);
    if(ret < 0)
    {
        if(incoming == IP_FALSE)
        {
            IPCOM_LOG0(INFO, "ipnet_nat_loop_rules() :: OUTGOING packet is invalid");
            ipnet->nat.inner.stats.invalid_out++;
        }
        else
        {
            IPCOM_LOG0(INFO, "ipnet_nat_loop_rules() :: INCOMING packet is invalid");
            ipnet->nat.inner.stats.invalid_in++;
        }
        return IPNET_NAT_INVALID;   /* Packet is invalid in some way */
    }

    /* Search for existing mapping */
    nat_mapping = ipnet_nat_find_mapping(incoming, *pktp, &nat_tuple);
    if(nat_mapping != IP_NULL)
    {
#ifdef IPNET_USE_NAT_PT
        if(incoming == IP_TRUE && (nat_mapping->rule->inner.pt == 1 || nat_mapping->rule->inner.pt_block == 1))
        {
            /* Update the mapping */
            ipnet_nat_update_mapping(IP_TRUE, nat_mapping, &nat_tuple);
            return ipnet_nat_pt_translate(IP_TRUE, nat_mapping, pktp, &nat_tuple, param);
        }
        else
#endif
        {
            (void)param;
            /* Update the mapping */
            ipnet_nat_update_mapping(incoming, nat_mapping, &nat_tuple);
            /* Perform the network address translation */
            return ipnet_nat_translate(incoming, pktp, &nat_tuple, nat_mapping);
        }
    }

    if (no_rules == IP_TRUE)
        return IPNET_NAT_NOMATCH;       /* Skip rules */

    /* Match packet against nat rules */
    if (nat_tuple.fragoff == 0)
    {
	    nat_rule = IPCOM_LIST_FIRST(&ipnet->nat.head_rule);
	    while(nat_rule != IP_NULL && nat_tuple.fragoff == 0)
	    {
	        i++;
            ret = ipnet_nat_match_rule(incoming, nat_rule, (*pktp)->ifindex, &nat_tuple);
            if(ret == 1)
            {
                if(incoming == IP_FALSE)
                    IPCOM_LOG1(DEBUG, "ipnet_nat_loop_rules() :: OUTGOING packet matched rule @%d", i);
                else
                    IPCOM_LOG1(DEBUG, "ipnet_nat_loop_rules() :: INCOMING packet matched rule @%d", i);
                break;
            }
            /* Try next rule */
            nat_rule = IPCOM_LIST_NEXT(&nat_rule->list_rule);
	    }

        if(nat_rule != IP_NULL)
        {
            /* Add a mapping */
            nat_mapping = ipnet_nat_add_mapping(incoming,
                                                nat_rule,
                                                &nat_tuple,
                                                0,
                                                IP_NULL,
                                                IP_FALSE,        /* Rule selects port */
                                                nat_rule->proxy_func,
                                                nat_rule->proxy_cookie);

            if(nat_mapping == IP_NULL)
            {
                IPCOM_LOG0(WARNING, "ipnet_nat_loop_rules() :: Failed to add mapping. (dropping packet)");
                if(incoming == IP_FALSE)
                    ipnet->nat.inner.stats.dropped_out++;
                else
                    ipnet->nat.inner.stats.dropped_in++;
                return IPNET_NAT_DROP;
            }
            /* Perform the network address translation */
            return ipnet_nat_translate(incoming, pktp, &nat_tuple, nat_mapping);
        }
    }
    else
    {
        /* Incoming fragments must be re-assembled first */
        ip_assert(incoming == IP_FALSE);
    }

    if(incoming == IP_FALSE)
        ipnet->nat.inner.stats.no_rule_match_out++;
    else
        ipnet->nat.inner.stats.no_rule_match_in++;
    return IPNET_NAT_NOMATCH;
}    /*lint !e550*/


/*
 ****************************************************************************
 * 10                    GLOBAL FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipnet_nat_match_rule
 *===========================================================================
 * Description: Match packet against NAT rules.
 * Parameters:  incoming    - IP_TRUE for incoming packets.
 *              nat_rule    - pointer to the rule.
 *              ifindex     - interface index.
 *              nat_tuple   - pointer to the packet tuple.
 * Returns:     1 = packet matched a rule.
 *              0 = no matching rules.
 */
IP_GLOBAL Ip_s32
ipnet_nat_match_rule(Ip_bool incoming, Ipnet_nat_rule *nat_rule, Ip_u32 ifindex, Ipnet_nat_tuple *nat_tuple)
{
    Ipnet_netif *netif;

    /* Check interface */
    netif = ipnet_if_indextonetif(IPCOM_VR_ANY, ifindex);
    if (netif == IP_NULL
        || ipcom_strcmp(nat_rule->inner.ifname, netif->ipcom.name) != 0)
        return 0;   /* Interface does not match */

    /* Check map and map-block rules for outgoing packets */
    if(incoming == IP_FALSE
        && (nat_rule->inner.map == 1 || nat_rule->inner.map_block == 1
            || nat_rule->inner.pt == 1 || nat_rule->inner.pt_block == 1))
    {
        if(nat_rule->inner.map == 1 || nat_rule->inner.map_block == 1)
        {
            if(nat_rule->inner.src_n != (nat_tuple->src_n & nat_rule->inner.src_mask_n))
                return 0;                           /* Source address does not match */
        }
        else
        {
#ifdef IPNET_USE_NAT_PT
            int i;

            for(i=0; i<4; i++)
            {
                if(nat_rule->inner.src_n_pt.in6.addr32[i] !=
                    (nat_tuple->pt_src.in6.addr32[i] & nat_rule->inner.src_mask_n_pt.in6.addr32[i]))
                    return 0;                           /* Source address does not match */
            }
#endif
        }

        if(nat_rule->inner.portmap == 1)
        {
            if(nat_rule->inner.portmap_tcpudp == 1 && nat_tuple->protocol != IP_IPPROTO_TCP &&
                nat_tuple->protocol != IP_IPPROTO_UDP)
                return 0;                           /* Protocol does not match */

            if(nat_rule->inner.portmap_tcpudp == 0 && nat_rule->inner.portmap_protocol != nat_tuple->protocol)
                return 0;                           /* Protocol does not match */
        }

        if(nat_rule->inner.icmpidmap == 1)
        {
            if(nat_tuple->protocol != IP_IPPROTO_ICMP)
                return 0;                           /* Protocol does not match */
        }

        if(nat_rule->inner.proxy == 1)
        {
            if(nat_rule->inner.proxy_protocol != nat_tuple->protocol)
                return 0;                           /* Protocol does not match */

            if (nat_tuple->protocol == IP_IPPROTO_TCP || nat_tuple->protocol == IP_IPPROTO_UDP)
            {
                if(nat_rule->inner.proxy_trigger != ip_ntohs(nat_tuple->dst_port_n))
                    return 0;                           /* Port does not match */
            }
        }
    }
    /* Check rdr rules for incoming packets */
    else if (incoming == IP_TRUE && nat_rule->inner.rdr == 1)
    {
        if(nat_rule->inner.dst_n != (nat_tuple->dst_n & nat_rule->inner.dst_mask_n))
            return 0;                           /* Destination address does not match */

        if (nat_rule->inner.rdr_tcpudp == 1)
        {
            /* Must be UDP or TCP */
            if (nat_tuple->protocol != IP_IPPROTO_TCP
                && nat_tuple->protocol != IP_IPPROTO_UDP)
            {
                return 0;                           /* Protocol does not match */
            }

            /* Port must match */
            if(nat_rule->inner.rdr_old_port_n != nat_tuple->dst_port_n)
                return 0;                           /* Destination port does not match */
        }
        else
        {
            if (nat_rule->inner.rdr_protocol != 0)
            {
                /* Protocol must match */
                if (nat_rule->inner.rdr_protocol != nat_tuple->protocol)
                    return 0;						/* Protocol does not match */

                /* Port must match for UDP and TCP */
                if (nat_tuple->protocol == IP_IPPROTO_TCP
                    || nat_tuple->protocol == IP_IPPROTO_UDP)
                {
                    if(nat_rule->inner.rdr_old_port_n != nat_tuple->dst_port_n)
                        return 0;                    /* Destination port does not match */
                }
            }
        }
    }
    else
    {
        return 0;
    }

    /* Rule matched packet! */
    return 1;
}


/*
 *===========================================================================
 *                    ipnet_nat_checksum_update
 *===========================================================================
 * Description: Efficient checksum update algorithm taken from RFC 3022
 *              Copyright (C) The Internet Society (2001).  All Rights Reserved.
 *              assuming: unsigned char is 8 bits, long is 32 bits.
 * Parameters:  chksum - points to the chksum in the packet.
 *              optr   - points to the old data in the packet.
 *              olen   - old data length.
 *              nptr   - points to the new data in the packet.
 *              nlen   - new data length.
 * Returns:     nothing.
 */
IP_GLOBAL void
ipnet_nat_checksum_update(unsigned char *chksum, unsigned char *optr, int olen,
                          unsigned char *nptr, int nlen)
{
    int x, old, new;
    x = chksum[0] * 256 + chksum[1];
    x = ~((unsigned)x) & 0xFFFF;
    while (olen)
    {
        old = optr[0] * 256 + optr[1];
        optr += 2;
        x -= old & 0xffff;
        if (x<=0)
        {
            x--;
            x &= 0xffff;
        }
        olen -= 2;
    }
    while (nlen)
    {
        new = nptr[0] * 256 + nptr[1];
        nptr+=2;
        x += new & 0xffff;
        if (x & 0x10000)
        {
            x++;
            x&=0xffff;
        }
        nlen -= 2;
    }
    x = ~((unsigned)x) & 0xFFFF;
    chksum[0] = (unsigned char)(x / 256);
    chksum[1] = (unsigned char)(x & 0xff);
}


/*
 *===========================================================================
 *                    ipnet_nat_update_sequence_number
 *===========================================================================
 * Description: Updates TCP sequence number information.
 * Parameters:  incoming    - IP_TRUE for incoming packets.
 *              nat_mapping - pointer to the NAT mapping.
 *              seq_num     - TCP sequence number.
 *              delta       - new correction of sequence number.
 * Returns:     nothing.
 */
IP_GLOBAL void
ipnet_nat_update_sequence_number(Ip_bool incoming, Ipnet_nat_mapping *nat_mapping, Ip_u32 seq_num, Ip_s32 delta)
{
    /* Check that this was not a resend */
    if ((nat_mapping->inner.seq_initiated == 0) || ((Ip_s32)(seq_num - nat_mapping->inner.seq_start) > 0))
	{
        nat_mapping->inner.seq_start = seq_num;
		nat_mapping->inner.prev_delta = nat_mapping->inner.curr_delta;
        nat_mapping->inner.curr_delta += delta;
        nat_mapping->inner.seq_initiated = 1;
        nat_mapping->inner.seq_incoming = incoming == IP_TRUE ? 1 : 0;
    }
}


/*
 *===========================================================================
 *                    ipnet_nat_translate_sequence_number
 *===========================================================================
 * Description: Translates TCP sequence number.
 * Parameters:  incoming    - IP_TRUE for incoming packets.
 *              pkt         - pointer to the packet.
 *              nat_mapping - pointer to the NAT mapping.
 * Returns:     nothing.
 */
IP_GLOBAL void
ipnet_nat_translate_sequence_number(Ip_bool incoming, Ipcom_pkt *pkt, Ipnet_nat_mapping *nat_mapping)
{
    if(nat_mapping->inner.seq_initiated == 1)
	{
	    if((incoming == IP_TRUE && nat_mapping->inner.seq_incoming == 1) ||
            (incoming == IP_FALSE && nat_mapping->inner.seq_incoming == 0))
        {

    	    Ip_u32 seq_num;

            seq_num = IP_GET_32ON16(((Ip_u32 *)&pkt->data[pkt->start] + 1));
            seq_num = ip_ntohl(seq_num);
            if ((Ip_s32)(seq_num - nat_mapping->inner.seq_start) > 0)
                seq_num += nat_mapping->inner.curr_delta;
            else
                seq_num += nat_mapping->inner.prev_delta;
	        IP_SET_HTONL((((Ip_u32 *)&pkt->data[pkt->start])+1), seq_num);
        }
    }
}


/*
 *===========================================================================
 *                    ipnet_nat_translate_ack_number
 *===========================================================================
 * Description: Translates TCP acknowledgement number.
 * Parameters:  incoming    - IP_TRUE for incoming packets.
 *              pkt         - pointer to the packet.
 *              nat_mapping - pointer to the NAT mapping.
 * Returns:     nothing.
 */
IP_GLOBAL void
ipnet_nat_translate_ack_number(Ip_bool incoming, Ipcom_pkt *pkt, Ipnet_nat_mapping *nat_mapping)
{
    if(nat_mapping->inner.seq_initiated == 1)
	{
	    if((incoming == IP_TRUE && nat_mapping->inner.seq_incoming == 0) ||
            (incoming == IP_FALSE && nat_mapping->inner.seq_incoming == 1))
        {

    	    Ip_u32 ack_num, tmp;

            ack_num = IP_GET_32ON16(((Ip_u32 *)&pkt->data[pkt->start] + 2));
            ack_num = ip_ntohl(ack_num);

            tmp = nat_mapping->inner.seq_start + nat_mapping->inner.prev_delta;
            if ((Ip_s32)(ack_num - tmp) > 0)
                ack_num -= nat_mapping->inner.curr_delta;
            else
                ack_num -= nat_mapping->inner.prev_delta;
	        IP_SET_HTONL((((Ip_u32 *)&pkt->data[pkt->start])+2), ack_num);
        }
    }
}


/*
 *===========================================================================
 *                    ipnet_nat_proxy
 *===========================================================================
 * Description: Call application proxy (ALG).
 * Parameters:  pktp        - pointer to packet.
 *              nat_tuple   - pointer to packet tuple.
 *              nat_mapping - pointer to NAT mapping.
 *              hlen        - header length in case a new packet must be generated.
 *              mod         - pointer to boolean for indicatation that the proxy
 *                            modified the packet.
 *              delta       - pointer to variable used for returning the
 *                            difference in length after the proxy modified
 *                            the packet.
 * Returns:     1 = proxy modified packet.
 *              0 = unmodified.
 *             -1 = proxy requests to drop packet.
 */
IP_GLOBAL Ip_s32
ipnet_nat_proxy(Ip_bool incoming,
                Ipcom_pkt **pktp,
                Ipnet_nat_tuple *nat_tuple,
                Ipnet_nat_mapping *nat_mapping,
                int hlen,
                Ip_bool *mod,
                Ip_s32 *delta)
{
    Ipnet_nat_proxy_param proxy_param;
    Ipcom_tcp_header *tcp;
    Ipcom_udp_header *udp;
    Ip_u8 *newhdr, *appdata, *newdata = IP_NULL;
    int tlen, applen, origlen, ret;
    Ipcom_pkt *new_pkt;

    newhdr = &(*pktp)->data[(*pktp)->start-hlen];
    if(nat_tuple->fragoff == 0 && nat_tuple->protocol == IP_IPPROTO_TCP)
    {
        tcp     = (Ipcom_tcp_header *)&(*pktp)->data[(*pktp)->start];
        tlen    = ip_ntohs(tcp->offset_flags)>>10;
        appdata = (Ip_u8 *)tcp + tlen;
        applen  = ((*pktp)->end - (*pktp)->start) - tlen;
    }
    else if(nat_tuple->fragoff == 0 && nat_tuple->protocol == IP_IPPROTO_UDP)
    {
        udp     = (Ipcom_udp_header *)&(*pktp)->data[(*pktp)->start];
        tlen    = sizeof(*udp);
        appdata = (Ip_u8 *)udp + tlen;
        applen  = ((*pktp)->end - (*pktp)->start) - tlen;
    }
    else
    {
        tlen    = 0;
        appdata = &(*pktp)->data[(*pktp)->start] + tlen;
        applen  = ((*pktp)->end - (*pktp)->start) - tlen;
    }

    /* Populate proxy parameters */
    ipcom_memset(&proxy_param, 0, sizeof(proxy_param));
    proxy_param.mapping      = nat_mapping;
    proxy_param.inbound      = nat_mapping->inner.rdr == 1 ? IP_TRUE : IP_FALSE;
    proxy_param.incoming     = incoming;
    proxy_param.fragid       = nat_tuple->fragid;
    proxy_param.fragmf       = nat_tuple->fragmf;
    proxy_param.fragoff      = nat_tuple->fragoff;
    proxy_param.cookie       = nat_mapping->proxy_cookie;
    proxy_param.tuple.protocol     = nat_mapping->inner.tuple.protocol;
    proxy_param.tuple.private_addr = ip_ntohl(nat_mapping->inner.tuple.src_n);
    proxy_param.tuple.public_addr  = ip_ntohl(nat_mapping->inner.tuple.dst_n);
    proxy_param.tuple.private_port = ip_ntohs(nat_mapping->inner.tuple.src_port_n);
    proxy_param.tuple.public_port  = ip_ntohs(nat_mapping->inner.tuple.dst_port_n);
    proxy_param.nat_addr     = ip_ntohl(nat_mapping->inner.nat_addr_n);
    proxy_param.nat_port     = ip_ntohs(nat_mapping->inner.nat_port_n);
    if (nat_mapping->rule->inner.pt == 1 || nat_mapping->rule->inner.pt_block == 1)
    {
        proxy_param.natpt     = IP_TRUE;
#ifdef IPNET_USE_NAT_PT
        proxy_param.prefix[0] = nat_mapping->inner.tuple.pt_dst.in6.addr32[0];
        proxy_param.prefix[1] = nat_mapping->inner.tuple.pt_dst.in6.addr32[1];
        proxy_param.prefix[2] = nat_mapping->inner.tuple.pt_dst.in6.addr32[2];
#endif
    }

    origlen = applen;
    ip_assert(nat_mapping->proxy_func != IP_NULL);
    ret = nat_mapping->proxy_func(newhdr,
                                  appdata,
                                  &applen,
                                  (*pktp)->maxlen - (*pktp)->end,
                                  &proxy_param,
                                  &newdata);
    if (ret == 1)
    {
       /* Packet modified */
        *delta = (Ip_s32)(applen - origlen);
        *mod = IP_TRUE;
        if (newdata != IP_NULL)
        {
            ip_assert(applen > origlen);    /* Did not fit so new buffer must be larger */

            new_pkt = ipcom_pkt_malloc(IPNET_NAT_LINK_HDR_SIZE + hlen + tlen + applen,
                                       IP_FLAG_FC_STACKCONTEXT);
            if(new_pkt == IP_NULL)
            {
                *delta = 0;
                *mod = IP_FALSE;
                return -1;
            }

            new_pkt->ipstart = IPNET_NAT_LINK_HDR_SIZE;
            new_pkt->start   = new_pkt->ipstart + hlen;
            new_pkt->end     = new_pkt->start + tlen + applen;
            new_pkt->ifindex = (*pktp)->ifindex;
            new_pkt->flags   = (*pktp)->flags;
            ipcom_memcpy(&new_pkt->data[new_pkt->start-hlen], &(*pktp)->data[(*pktp)->start-hlen], hlen+tlen);
            ipcom_memcpy(&new_pkt->data[new_pkt->start+tlen], newdata, applen);
            ipcom_free(newdata);
            ipcom_pkt_free(*pktp);
            *pktp = new_pkt;
        }
        else
        {
            (*pktp)->end = (*pktp)->end + *delta;
        }
        ip_assert((*pktp)->end <= (*pktp)->maxlen);
    }
    else if (ret == 0)
    {
        /* Packet untouched */
        ip_assert(origlen == applen);
        *delta = 0;
        *mod = IP_FALSE;
    }
    else
    {
        /* Drop packet */
        *delta = 0;
        *mod = IP_FALSE;
    }
    return (Ip_s32)ret;
}


/*
 *===========================================================================
 *                    ipnet_nat_translate_tcpudp_port
 *===========================================================================
 * Description: Translate the TCP/UDP port.
 * Parameters:  incoming    - IP_TRUE for incoming packets.
 *              pkt         - pointer to the packet.
 *              nat_mapping - pointer to the NAT mapping.
 * Returns:     nothing.
 */
IP_GLOBAL void
ipnet_nat_translate_tcpudp_port(Ip_bool incoming, Ipcom_pkt *pkt, Ipnet_nat_mapping *nat_mapping)
{
    /* Translate the source/destination port */
    if(incoming == IP_FALSE)
        *(Ip_u16 *)&pkt->data[pkt->start] = nat_mapping->inner.nat_port_n;
    else
        *(((Ip_u16 *)&pkt->data[pkt->start])+1) = nat_mapping->inner.tuple.src_port_n;
}


/*
 *===========================================================================
 *                    ipnet_nat_add_mapping
 *===========================================================================
 * Description: Add a NAT mapping.
 * Parameters:  incoming     - IP_TRUE for incoming packets.
 *              nat_rule     - pointer to the rule.
 *              nat_tuple    - pointer to the packet tuple.
 *              timeout      - mapping timeout in seconds (0 means default)
 *              parent       - pointer to parent mapping.
 *              proxy_napt   - IP_TRUE to enable NAPT for proxy added mappings.
 *              proxy_func   - proxy function (optional).
 *              proxy_cookie - proxy cookie (optional).
 * Returns:     pointer to the NAT mapping or IP_NULL if failed.
 */
IP_GLOBAL Ipnet_nat_mapping *
ipnet_nat_add_mapping(Ip_bool              incoming,
                      Ipnet_nat_rule      *nat_rule,
                      Ipnet_nat_tuple     *nat_tuple,
                      Ip_u32               timeout,
                      Ipnet_nat_mapping   *parent,
                      Ip_bool              proxy_napt,
                      Ipnet_nat_proxy_func proxy_func,
                      void                *proxy_cookie)
{
    Ipnet_nat_mapping *nat_mapping;
    Ip_bool            add_in_port_hash;
    Ip_s32             nat_id;

    /* Check that there is room for another mapping */
    if(ipnet->nat.head_mapping.size >= (Ip_u32)ipnet->nat.inner.max_mappings)
    {
        IPCOM_LOG0(WARNING, "ipnet_nat_add_mapping() :: Maximum number of mappings reached");
        ipnet->nat.inner.stats.mapping_failures++;
        return IP_NULL;
    }

    nat_mapping = ipcom_malloc(sizeof(*nat_mapping));
    if(nat_mapping == IP_NULL)
    {
        IPCOM_LOG0(ERR, "ipnet_nat_add_mapping() :: Could not allocate memory for mapping");
        ipnet->nat.inner.stats.mapping_failures++;
        return IP_NULL;
    }
    ipcom_memset(nat_mapping, 0, sizeof(*nat_mapping));
    ipcom_memcpy(&nat_mapping->inner.tuple, nat_tuple, sizeof(*nat_tuple));
    nat_mapping->rule         = nat_rule;
    nat_mapping->inner.rdr          = incoming == IP_TRUE ? 1 : 0;
    nat_mapping->proxy_func   = proxy_func;
    nat_mapping->proxy_cookie = proxy_cookie;

    if (parent == IP_NULL)
    {
        /* Assign new address and port */
        if (ipnet_nat_assign_mapping(incoming, nat_mapping, nat_tuple, &add_in_port_hash) != 0)
        {
            ipnet->nat.inner.stats.mapping_failures++;
            ipcom_free(nat_mapping);
            return IP_NULL;
        }
    }
    else
    {
        nat_mapping->inner.nat_addr_n = parent->inner.nat_addr_n;           /* Use parent's NAT adress */
        ip_assert(nat_tuple->protocol != IP_IPPROTO_ICMP);
        if(nat_tuple->protocol == IP_IPPROTO_UDP
            || nat_tuple->protocol == IP_IPPROTO_TCP)
        {
            if (proxy_napt == IP_TRUE)
            {
                /* Allocate a port from the auto pool */
                if((nat_id = ipnet_nat_get_id(ipnet->nat.inner.port_hi, ipnet->nat.inner.port_lo, ipnet->nat.ports)) < 0)
                {
                    IPCOM_LOG0(ERR, "ipnet_nat_add_mapping() :: Could not find available automatic nat id");
                    ipnet->nat.inner.stats.mapping_failures++;
                    ipcom_free(nat_mapping);
                    return IP_NULL;
                }
                nat_mapping->inner.nat_port_n = ip_htons((Ip_u16)nat_id);
                nat_mapping->inner.autoport = 1;
            }
            else
            {
                /* NAT port is the same as source port */
                nat_mapping->inner.nat_port_n = nat_tuple->src_port_n;
            }
            add_in_port_hash = IP_TRUE;
        }
        else
            add_in_port_hash = IP_FALSE;
    }

    /* Assign state and timeout */
    ipnet_nat_assign_timeout(incoming, nat_mapping, nat_tuple, timeout);

    /* Add mapping to the hash tables */
    if(add_in_port_hash == IP_TRUE)
        if(ipcom_hash_add(ipnet->nat.hash_out_port, nat_mapping) != IPCOM_SUCCESS)
            goto cleanup;
    if(ipcom_hash_add(ipnet->nat.hash_out_noport, nat_mapping) != IPCOM_SUCCESS)
        goto cleanup;
    if(add_in_port_hash == IP_TRUE)
        if(ipcom_hash_add(ipnet->nat.hash_in_port, nat_mapping) != IPCOM_SUCCESS)
            goto cleanup;
    if(ipcom_hash_add(ipnet->nat.hash_in_noport, nat_mapping) != IPCOM_SUCCESS)
        goto cleanup;
    ipcom_list_insert_last(&ipnet->nat.head_mapping, &nat_mapping->list);

#ifdef IPNET_DEBUG
    IPCOM_LOG0(DEBUG, "ipnet_nat_add_mapping() :: Mapping added");
    ipnet_nat_syslog_mapping(nat_mapping);
#endif

    ipnet->nat.inner.stats.mappings_added++;
    return nat_mapping;

cleanup:
    ipnet_nat_clean_mapping(nat_mapping);
    return IP_NULL;
}


/*
 *===========================================================================
 *                    ipnet_nat_update_mapping
 *===========================================================================
 * Description: Updates the state of a mapping and schedules new timeout.
 * Parameters:  incoming       - IP_TRUE for incoming packets.
 *              nat_mapping    - pointer to the nat_mapping to be updated.
 *              nat_tuple      - pointer to the packet tuple.
 * Returns:     nothing.
 */
IP_GLOBAL void
ipnet_nat_update_mapping(Ip_bool incoming,
                         Ipnet_nat_mapping *nat_mapping,
                         Ipnet_nat_tuple *nat_tuple)
{
    /* Do not update the mapping for ICMP error messages */
    if (nat_tuple->protocol == IP_IPPROTO_ICMP && IPNET_NAT_IS_ICMP_ERROR(nat_tuple->icmp_type))
        return;

    /* Adjust states and set timeout */
    if (nat_mapping->inner.tuple.protocol == IP_IPPROTO_TCP)
    {
        if (nat_tuple->fragoff == 0)
        {
            /* Check SYN flag */
            if(IP_BIT_ISSET(nat_tuple->tcp_flags, IPNET_NAT_TCPFLAG_SYN))
            {
                IPCOM_LOG0(DEBUG, "ipnet_nat_update_mapping() :: (SYN)");
                if(incoming == IP_TRUE)
                    nat_mapping->inner.outer_state = IPNET_NAT_MAPPING_SYN;
                else
                    nat_mapping->inner.inner_state = IPNET_NAT_MAPPING_SYN;
            }

            /* Check FIN flag */
            if(IP_BIT_ISSET(nat_tuple->tcp_flags, IPNET_NAT_TCPFLAG_FIN))
            {
                IPCOM_LOG0(DEBUG, "ipnet_nat_update_mapping() :: (FIN)");
                if(incoming == IP_TRUE)
                    nat_mapping->inner.outer_state = IPNET_NAT_MAPPING_FIN;
                else
                    nat_mapping->inner.inner_state = IPNET_NAT_MAPPING_FIN;
            }

            /* Check RST flag */
            if(IP_BIT_ISSET(nat_tuple->tcp_flags, IPNET_NAT_TCPFLAG_RESET))
            {
                IPCOM_LOG0(DEBUG, "ipnet_nat_update_mapping() :: (RST)");
                nat_mapping->inner.outer_state = IPNET_NAT_MAPPING_FIN;
                nat_mapping->inner.inner_state = IPNET_NAT_MAPPING_FIN;
            }

            /* Go to establihed */
            if(nat_mapping->inner.outer_state == IPNET_NAT_MAPPING_SYN &&
                nat_mapping->inner.inner_state == IPNET_NAT_MAPPING_SYN)
            {
                nat_mapping->inner.outer_state = IPNET_NAT_MAPPING_EST;
                nat_mapping->inner.inner_state = IPNET_NAT_MAPPING_EST;
            }
        }

        /* Set new timeout interval */
        if(nat_mapping->inner.outer_state == IPNET_NAT_MAPPING_EST &&
            nat_mapping->inner.inner_state == IPNET_NAT_MAPPING_EST)
        {
            nat_mapping->inner.tmo_int_sec = ipnet->nat.inner.timeout_tcp;
        }
        else
        {
            nat_mapping->inner.tmo_int_sec = ipnet->nat.inner.timeout_other;
        }
    }

    /* Reset the timeout */
    ipnet_nat_reset_mapping_timeout(nat_mapping);

#ifdef IPNET_DEBUG
    IPCOM_LOG0(DEBUG, "ipnet_nat_update_mapping() :: Mapping updated");
    ipnet_nat_syslog_mapping(nat_mapping);
#endif
}


/*
 *===========================================================================
 *                    ipnet_nat_embedded_tuple
 *===========================================================================
 * Description: Find an embedded tuple in an ICMP error message.
 * Parameters:  pkt            - pointer to the packet.
 *              nat_tuple      - pointer to the incoming tuple.
 *              embedded_tuple - pointer to the returned embedded tuple.
 * Returns:     0 = Found tuple.
 *             -1 = No embedded tuple found.
 */
IP_STATIC Ip_s32
ipnet_nat_embedded_tuple(Ipcom_pkt *pkt, Ipnet_nat_tuple *nat_tuple, Ipnet_nat_tuple *embedded_tuple)
{
    Ipnet_pkt_icmp   *icmp;
    Ipnet_pkt_icmp   *icmp_embed;
    Ipcom_ip_header  *iph_embed;
    Ipcom_udp_header *udp_embed;
    Ipcom_tcp_header *tcp_embed;
    Ip_u16            hlen_embed;

    icmp       = (Ipnet_pkt_icmp *)&pkt->data[pkt->start];
    iph_embed  = (Ipcom_ip_header *)icmp->data.failing_pkt.ip;
    hlen_embed = (Ip_u16)((iph_embed->vers_len & 0x0f)<<2);
    if(nat_tuple->fragoff != 0 || IPNET_NAT_IS_ICMP_ERROR(nat_tuple->icmp_type) == IP_FALSE)
    {
        return -1;
    }

    ipcom_memset(embedded_tuple, 0, sizeof(*embedded_tuple));
    embedded_tuple->dst_n    = IP_GET_32ON16((void *)&iph_embed->src.addr32);
    embedded_tuple->src_n    = IP_GET_32ON16((void *)&iph_embed->dst.addr32);
    embedded_tuple->protocol = iph_embed->protocol;
    if(embedded_tuple->protocol == IP_IPPROTO_UDP)
    {
        udp_embed = (Ipcom_udp_header *)((Ip_u8 *)iph_embed + hlen_embed);
        embedded_tuple->dst_port_n = udp_embed->src_port;
        embedded_tuple->src_port_n = udp_embed->dst_port;
    }
    else if(embedded_tuple->protocol == IP_IPPROTO_TCP)
    {
        tcp_embed = (Ipcom_tcp_header *)((Ip_u8 *)iph_embed + hlen_embed);
        embedded_tuple->dst_port_n = tcp_embed->src_port;
        embedded_tuple->src_port_n = tcp_embed->dst_port;
    }
    else if(embedded_tuple->protocol == IP_IPPROTO_ICMP)
    {
        /* Pretend reception of a reply */
        icmp_embed = (Ipnet_pkt_icmp *)((Ip_u8 *)iph_embed + hlen_embed);
        embedded_tuple->icmp_type = icmp_embed->type;
        switch(icmp_embed->type)
        {
        case IPCOM_ICMP_ECHO_REQUEST:
            embedded_tuple->icmp_type = IPCOM_ICMP_ECHO_REPLY;
            break;
        case IPCOM_ICMP_ECHO_REPLY:
            embedded_tuple->icmp_type = IPCOM_ICMP_ECHO_REQUEST;
            break;
        case IPCOM_ICMP_TSTAMP_REQUEST:
            embedded_tuple->icmp_type = IPCOM_ICMP_TSTAMP_REPLY;
            break;
        case IPCOM_ICMP_TSTAMP_REPLY:
            embedded_tuple->icmp_type = IPCOM_ICMP_TSTAMP_REQUEST;
            break;
        case IPCOM_ICMP_INFO_REQUEST:
            embedded_tuple->icmp_type = IPCOM_ICMP_INFO_REPLY;
            break;
        case IPCOM_ICMP_INFO_REPLY:
            embedded_tuple->icmp_type = IPCOM_ICMP_INFO_REQUEST;
            break;
        case IPCOM_ICMP_MASK_REQUEST:
            embedded_tuple->icmp_type = IPCOM_ICMP_MASK_REPLY;
            break;
        case IPCOM_ICMP_MASK_REPLY:
            embedded_tuple->icmp_type = IPCOM_ICMP_MASK_REQUEST;
            break;
        default:
            return 0;
        }
        embedded_tuple->icmp_id_n = icmp_embed->data.echo.id;
    }
    return 0;
}


/*
 *===========================================================================
 *                    ipnet_nat_find_mapping
 *===========================================================================
 * Description: Searches for a matching NAT mapping based on a packet.
 * Parameters:  incoming  - IP_TRUE for incoming packets.
 *              pkt       - pointer to the packet.
 *              nat_tuple - pointer to the tuple.
 * Returns:     pointer to the nat_mapping if found or IP_NULL if not found.
 */
IP_GLOBAL Ipnet_nat_mapping *
ipnet_nat_find_mapping(Ip_bool incoming, Ipcom_pkt *pkt, Ipnet_nat_tuple *nat_tuple)
{
    Ipnet_nat_tuple embedded_tuple;
    Ipnet_nat_mapping *nat_mapping;
    Ipnet_nat_hash_key nat_key;

    if (ipnet->nat.head_mapping.size == 0)
    {
        ip_assert(ipnet->nat.hash_out_port->elem == 0);
        ip_assert(ipnet->nat.hash_out_noport->elem == 0);
        ip_assert(ipnet->nat.hash_in_port->elem == 0);
        ip_assert(ipnet->nat.hash_in_noport->elem == 0);
        return IP_NULL;
    }

    if (nat_tuple->protocol == IP_IPPROTO_ICMP)
    {
        /* Extract the embedded IPv4 header and first part of datagram */
        if (pkt != IP_NULL && ipnet_nat_embedded_tuple(pkt, nat_tuple, &embedded_tuple) == 0)
            nat_tuple = &embedded_tuple;
    }

    /* Create the hask key and try to find in hash tables */
    if(incoming == IP_TRUE)
    {
        nat_key.protocol   = nat_tuple->protocol;
        nat_key.dst_n      = nat_tuple->src_n;
        nat_key.nat_addr_n = nat_tuple->dst_n;
        if(nat_tuple->fragoff == 0 &&
            (nat_tuple->protocol == IP_IPPROTO_TCP || nat_tuple->protocol == IP_IPPROTO_UDP))
        {
            nat_key.dst_port_n = nat_tuple->src_port_n;
            nat_key.nat_port_n = nat_tuple->dst_port_n;
            nat_mapping = ipcom_hash_get(ipnet->nat.hash_in_port, &nat_key);
            if(nat_mapping == IP_NULL)
            {
                /* Check for mappings with implicit destination port */
                IPCOM_LOG0(DEBUG2, "ipnet_nat_find_mapping() :: no mapping found for INCOMING packet"
                                   " when trying with implicit destination port = 0");
                nat_key.dst_port_n = 0;
                nat_mapping = ipcom_hash_get(ipnet->nat.hash_in_port, &nat_key);
                if(nat_mapping == IP_NULL)
                {
                    /* Check for mappings with implicit destination port and destination address */
                    IPCOM_LOG0(DEBUG2, "ipnet_nat_find_mapping() :: no mapping found for INCOMING packet"
                                       " when trying with implicit destination port = 0 and destination address = 0");
                    nat_key.dst_n = 0;
                    nat_mapping = ipcom_hash_get(ipnet->nat.hash_in_port, &nat_key);
                }
                if(nat_mapping != IP_NULL)
                {
                    /* Remove the mapping from the hash tables */
                    ip_assert(nat_mapping->inner.tuple.dst_port_n == 0);
                    (void)ipcom_hash_remove(ipnet->nat.hash_out_port, nat_mapping);
                    (void)ipcom_hash_remove(ipnet->nat.hash_out_noport, nat_mapping);
                    (void)ipcom_hash_remove(ipnet->nat.hash_in_port, nat_mapping);
                    (void)ipcom_hash_remove(ipnet->nat.hash_in_noport, nat_mapping);

                    /* Update the destination port */
                    nat_mapping->inner.tuple.dst_port_n = nat_tuple->src_port_n;
                    /* Update the destination address */
                    nat_mapping->inner.tuple.dst_n = nat_tuple->src_n;

                    /* Add mapping to the hash tables */
                    if(ipcom_hash_add(ipnet->nat.hash_out_port, nat_mapping) != IPCOM_SUCCESS
                        || ipcom_hash_add(ipnet->nat.hash_out_noport, nat_mapping) != IPCOM_SUCCESS
                        || ipcom_hash_add(ipnet->nat.hash_in_port, nat_mapping) != IPCOM_SUCCESS
                        || ipcom_hash_add(ipnet->nat.hash_in_noport, nat_mapping) != IPCOM_SUCCESS)
                    {
                        ipnet_nat_clean_mapping(nat_mapping);
                        nat_mapping = IP_NULL;
                    }
                }
            }
        }
        else if(nat_tuple->fragoff == 0
                && nat_tuple->protocol == IP_IPPROTO_ICMP
                && (IPNET_NAT_IS_ICMP_REQUEST(nat_tuple->icmp_type) || IPNET_NAT_IS_ICMP_REPLY(nat_tuple->icmp_type)))
        {
            nat_key.dst_port_n = 0;
            nat_key.nat_port_n = nat_tuple->icmp_id_n;
            nat_mapping = ipcom_hash_get(ipnet->nat.hash_in_port, &nat_key);
        }
        else
        {
            nat_mapping = ipcom_hash_get(ipnet->nat.hash_in_noport, &nat_key);
            if(nat_mapping == IP_NULL)
            {
                /* Check for mappings with implicit address */
                IPCOM_LOG0(DEBUG2, "ipnet_nat_find_mapping() :: no mapping found for INCOMING packet"
                                   " when trying with implicit adress = 0");
                nat_key.dst_n = 0;
                nat_mapping = ipcom_hash_get(ipnet->nat.hash_in_noport, &nat_key);
                if(nat_mapping != IP_NULL)
                {
                    /* Remove the mapping from the hash tables */
                    ip_assert(nat_mapping->inner.tuple.dst_n == 0);
                    (void)ipcom_hash_remove(ipnet->nat.hash_out_noport, nat_mapping);
                    (void)ipcom_hash_remove(ipnet->nat.hash_in_noport, nat_mapping);

                    /* Update the destination address */
                    nat_mapping->inner.tuple.dst_n = nat_tuple->src_n;

                    /* Add mapping to the hash tables */
                    if(ipcom_hash_add(ipnet->nat.hash_out_noport, nat_mapping) != IPCOM_SUCCESS ||
                       ipcom_hash_add(ipnet->nat.hash_in_noport, nat_mapping) != IPCOM_SUCCESS)
                    {
                        ipnet_nat_clean_mapping(nat_mapping);
                        nat_mapping = IP_NULL;
                    }
                }
            }
        }
    }
    else
    {
        nat_key.protocol = nat_tuple->protocol;
        nat_key.dst_n = nat_tuple->dst_n;
        nat_key.src_n = nat_tuple->src_n;
        if(nat_tuple->fragoff == 0 &&
            (nat_tuple->protocol == IP_IPPROTO_TCP || nat_tuple->protocol == IP_IPPROTO_UDP))
        {
            nat_key.dst_port_n = nat_tuple->dst_port_n;
            nat_key.src_port_n = nat_tuple->src_port_n;
            nat_mapping = ipcom_hash_get(ipnet->nat.hash_out_port, &nat_key);
            if(nat_mapping == IP_NULL)
            {
                /* Check for mappings with implicit source port */
                IPCOM_LOG0(DEBUG2, "ipnet_nat_find_mapping() :: no mapping found for OUTGOING packet"
                                   " when trying with implicit source port = 0");
                nat_key.src_port_n = 0;
                nat_mapping = ipcom_hash_get(ipnet->nat.hash_out_port, &nat_key);
                if(nat_mapping == IP_NULL)
                {
                    /* Check for mappings with implicit source port and source address */
                    IPCOM_LOG0(DEBUG2, "ipnet_nat_find_mapping() :: no mapping found for OUTGOING packet"
                                       " when trying with implicit source port = 0 and source address = 0");
                    nat_key.src_n = 0;
                    nat_mapping = ipcom_hash_get(ipnet->nat.hash_out_port, &nat_key);
                }
                if(nat_mapping == IP_NULL)
                {
                    /* Check for mappings with implicit destination port */
                    IPCOM_LOG0(DEBUG2, "ipnet_nat_find_mapping() :: no mapping found for OUTGOING packet"
                                       " when trying with implicit destination port = 0");
                    nat_key.src_n = nat_tuple->src_n;
                    nat_key.src_port_n = nat_tuple->src_port_n;
                    nat_key.dst_port_n = 0;
                    nat_mapping = ipcom_hash_get(ipnet->nat.hash_out_port, &nat_key);
                }
                if(nat_mapping == IP_NULL)
                {
                    /* Check for mappings with implicit destination port and destination address */
                    IPCOM_LOG0(DEBUG2, "ipnet_nat_find_mapping() :: no mapping found for OUTGOING packet"
                                       " when trying with implicit destination port = 0 and destination address = 0");
                    nat_key.dst_n = 0;
                    nat_mapping = ipcom_hash_get(ipnet->nat.hash_out_port, &nat_key);
                }
                if(nat_mapping != IP_NULL)
                {
                    /* Remove the mapping from the hash tables */
                    ip_assert(nat_mapping->inner.tuple.src_port_n == 0 || nat_mapping->inner.tuple.dst_port_n == 0);
                    (void)ipcom_hash_remove(ipnet->nat.hash_out_port, nat_mapping);
                    (void)ipcom_hash_remove(ipnet->nat.hash_out_noport, nat_mapping);
                    (void)ipcom_hash_remove(ipnet->nat.hash_in_port, nat_mapping);
                    (void)ipcom_hash_remove(ipnet->nat.hash_in_noport, nat_mapping);

                    /* Update the mapping */
                    nat_mapping->inner.tuple.src_port_n = nat_tuple->src_port_n;
                    nat_mapping->inner.tuple.src_n = nat_tuple->src_n;
                    nat_mapping->inner.tuple.dst_port_n = nat_tuple->dst_port_n;
                    nat_mapping->inner.tuple.dst_n = nat_tuple->dst_n;

                    /* Add mapping to the hash tables */
                    if(ipcom_hash_add(ipnet->nat.hash_out_port, nat_mapping) != IPCOM_SUCCESS
                        || ipcom_hash_add(ipnet->nat.hash_out_noport, nat_mapping) != IPCOM_SUCCESS
                        || ipcom_hash_add(ipnet->nat.hash_in_port, nat_mapping) != IPCOM_SUCCESS
                        || ipcom_hash_add(ipnet->nat.hash_in_noport, nat_mapping) != IPCOM_SUCCESS)
                    {
                        ipnet_nat_clean_mapping(nat_mapping);
                        nat_mapping = IP_NULL;
                    }
                }
            }
        }
        else if(nat_tuple->fragoff == 0
                && nat_tuple->protocol == IP_IPPROTO_ICMP
                && (IPNET_NAT_IS_ICMP_REQUEST(nat_tuple->icmp_type) || IPNET_NAT_IS_ICMP_REPLY(nat_tuple->icmp_type)))
        {
            nat_key.dst_port_n = 0;
            nat_key.src_port_n = nat_tuple->icmp_id_n;
            nat_mapping = ipcom_hash_get(ipnet->nat.hash_out_port, &nat_key);
        }
        else
        {
            nat_mapping = ipcom_hash_get(ipnet->nat.hash_out_noport, &nat_key);
            if(nat_mapping == IP_NULL)
            {
                /* Check for mappings with implicit address */
                IPCOM_LOG0(DEBUG2, "ipnet_nat_find_mapping() :: no mapping found for OUTGOING packet"
                                   " when trying with implicit adress = 0");
                nat_key.src_n = 0;
                nat_mapping = ipcom_hash_get(ipnet->nat.hash_out_noport, &nat_key);
                if(nat_mapping != IP_NULL)
                {
                    /* Remove the mapping from the hash tables */
                    ip_assert(nat_mapping->inner.tuple.src_n == 0);
                    (void)ipcom_hash_remove(ipnet->nat.hash_out_noport, nat_mapping);
                    (void)ipcom_hash_remove(ipnet->nat.hash_in_noport, nat_mapping);

                    /* Update the source address */
                    nat_mapping->inner.tuple.src_n = nat_tuple->src_n;

                    /* Add mapping to the hash tables */
                    if(ipcom_hash_add(ipnet->nat.hash_out_noport, nat_mapping) != IPCOM_SUCCESS ||
                       ipcom_hash_add(ipnet->nat.hash_in_noport, nat_mapping) != IPCOM_SUCCESS)
                    {
                        ipnet_nat_clean_mapping(nat_mapping);
                        nat_mapping = IP_NULL;
                    }
                }
            }
        }
    }

#ifdef IPNET_DEBUG
    if(nat_mapping != IP_NULL)
    {
        IPCOM_LOG1(DEBUG, "ipnet_nat_find_mapping() :: %s packet matched mapping",
                           incoming == IP_TRUE ? "INCOMING" : "OUTGOING");
        ipnet_nat_syslog_mapping(nat_mapping);
    }
#endif

    return nat_mapping;
}


/*
 *===========================================================================
 *                    ipnet_nat_getsockopt
 *===========================================================================
 * Description: Dispatcher for the IP_IP_NAT socket option.
 * Parameters:  sock    - pointer to socket.
 *              optname - option name.
 *              optval  - pointer to option value.
 *              optlen  - pointer to option length.
 * Returns:     0 = success.
 *              IP_ERRNO_EINVAL = error.
 */
IP_GLOBAL Ip_s32
ipnet_nat_getsockopt(Ipnet_socket *sock, int optname, void *optval,
                           Ip_socklen_t *optlen)
{
    Ip_s32 ret = -1;
    Ipnet_nat_ctrl *ctrl;

    (void)sock;
    (void)optname;
    if(*optlen < sizeof(*ctrl))
        goto exit;

    ctrl = (Ipnet_nat_ctrl *)optval;
    switch(ctrl->cmd)
    {
        case IPNET_NAT_CTRL_GET_RULE:
            ret = ipnet_nat_internal_get_rule(ctrl, optlen);
            break;
        case IPNET_NAT_CTRL_GET_MAPPING:
            ret = ipnet_nat_internal_get_mapping(ctrl, optlen);
            break;
        case IPNET_NAT_CTRL_GET_PROXY:
            ret = ipnet_nat_internal_get_proxy(ctrl, optlen);
            break;
        case IPNET_NAT_CTRL_GET_INFO:
            ret = ipnet_nat_internal_get_info(ctrl, optlen);
            break;
        default:
            break;
    }

exit:
    return ret != 0 ? -IP_ERRNO_EINVAL : 0;
}


/*
 *===========================================================================
 *                    ipnet_nat_setsockopt
 *===========================================================================
 * Description: Dispatcher for the IP_IP_NAT socket option.
 * Parameters:  sock    - pointer to socket.
 *              optname - option name.
 *              optval  - pointer to option value.
 *              optlen  - option length.
 * Returns:     0 = success.
 *              IP_ERRNO_EINVAL = error.
 */
IP_GLOBAL Ip_s32
ipnet_nat_setsockopt(Ipnet_socket *sock, int optname, IP_CONST void *optval,
                           Ip_socklen_t optlen)
{
    Ipnet_nat_ctrl *ctrl;
    Ip_s32 ret = -1;

    (void)sock;
    (void)optname;
    if(optlen < sizeof(*ctrl))
        goto exit;

    ctrl = (Ipnet_nat_ctrl *)optval;
    /* Must invalidate the routing cache since NAT can affect routing */
    ipnet_route_cache_invalidate();
    switch(ctrl->cmd)
    {
        case IPNET_NAT_CTRL_ENABLE:
            ret = ipnet_nat_internal_enable();
            break;
        case IPNET_NAT_CTRL_DISABLE:
            ret = ipnet_nat_internal_disable();
            break;
        case IPNET_NAT_CTRL_ADD_RULE:
            ret = ipnet_nat_internal_add_rule(&ctrl->type.rule_info, ctrl->seqno);
            break;
        case IPNET_NAT_CTRL_DEL_RULE:
            ret = ipnet_nat_internal_remove_rule(&ctrl->type.rule_info);
            break;
        case IPNET_NAT_CTRL_FLUSH_RULES:
            ret = ipnet_nat_internal_flush_rules();
            break;
        case IPNET_NAT_CTRL_FLUSH_MAPPINGS:
            ret = ipnet_nat_internal_flush_mappings();
            break;
        case IPNET_NAT_CTRL_CLEAR_STATS:
            ret = ipnet_nat_internal_clear_stats();
            break;
        default:
            break;
    }

exit:
    return ret != 0 ? -IP_ERRNO_EINVAL : 0;
}


/*
 *===========================================================================
 *                    ipnet_nat_init
 *===========================================================================
 * Description: Initialize NAT.
 * Parameters:  none.
 * Returns:     IPCOM_ERR_NO_MEMORY
 *              IPCOM_ERR_FAILED
 *              IPCOM_SUCCESS
 */
IP_GLOBAL Ip_err
ipnet_nat_init(void)
{
    Ip_err errval;

    /* Clear NAT global memory */
    ipcom_memset(&ipnet->nat, 0, sizeof(ipnet->nat));

    /* Initialize lists */
    ipcom_list_init(&ipnet->nat.head_rule);
    ipcom_list_init(&ipnet->nat.head_proxy);

    /* Create hash tables */
    ipnet->nat.hash_out_port = ipcom_hash_new(  (Ipcom_hash_obj_func)ipnet_nat_obj_hash_out_port,
                                                (Ipcom_hash_key_func)ipnet_nat_key_hash_out_port,
                                                (Ipcom_hash_cmp_func)ipnet_nat_cmp_hash_out_port);

    ipnet->nat.hash_out_noport = ipcom_hash_new((Ipcom_hash_obj_func)ipnet_nat_obj_hash_out_noport,
                                                (Ipcom_hash_key_func)ipnet_nat_key_hash_out_noport,
                                                (Ipcom_hash_cmp_func)ipnet_nat_cmp_hash_out_noport);

    ipnet->nat.hash_in_port = ipcom_hash_new(   (Ipcom_hash_obj_func)ipnet_nat_obj_hash_in_port,
                                                (Ipcom_hash_key_func)ipnet_nat_key_hash_in_port,
                                                (Ipcom_hash_cmp_func)ipnet_nat_cmp_hash_in_port);

    ipnet->nat.hash_in_noport = ipcom_hash_new( (Ipcom_hash_obj_func)ipnet_nat_obj_hash_in_noport,
                                                (Ipcom_hash_key_func)ipnet_nat_key_hash_in_noport,
                                                (Ipcom_hash_cmp_func)ipnet_nat_cmp_hash_in_noport);

    ipcom_list_init(&ipnet->nat.head_mapping);

    /* Status check */
    if(ipnet->nat.hash_out_port == IP_NULL
       || ipnet->nat.hash_out_noport == IP_NULL
       || ipnet->nat.hash_in_port == IP_NULL
       || ipnet->nat.hash_in_noport == IP_NULL)
    {
        errval = IPCOM_ERR_NO_MEMORY;
        goto fail;
    }

    /* Add application proxies */
#ifdef IPNET_USE_NAT_FTP_ALG
    errval = ipnet_nat_add_proxy("ftp", "tcp", ipnet_nat_proxy_ftp, IP_NULL);
    if (errval != IPCOM_SUCCESS)
    {
        IPCOM_LOG0(ERR, "ipnet_nat_init() :: Failed to add FTP ALG");
        goto fail;
    }
#endif  /* #ifdef IPNET_USE_NAT_FTP_ALG  */

#ifdef IPNET_USE_NAT_H323_ALG
    errval = ipnet_nat_add_proxy("h323", "tcp", ipnet_nat_proxy_h323, IP_NULL);
    if (errval != IPCOM_SUCCESS)
    {
        IPCOM_LOG0(ERR, "ipnet_nat_init() :: Failed to add H.323 ALG");
        goto fail;
    }
#endif  /* #ifdef IPNET_USE_NAT_H323_ALG  */

#ifdef IPNET_USE_NAT_SIP_ALG
    errval = ipnet_nat_add_proxy("sip", "udp", ipnet_nat_proxy_sip, IP_NULL);
    if (errval != IPCOM_SUCCESS)
    {
        IPCOM_LOG0(ERR, "ipnet_nat_init() :: Failed to add SIP ALG");
        goto fail;
    }
#endif  /* #ifdef IPNET_USE_NAT_SIP_ALG  */

#ifdef IPNET_USE_NAT_PPTP_ALG
    errval = ipnet_nat_add_proxy("pptp", "gre", ipnet_nat_proxy_pptp, IP_NULL);
    if (errval != IPCOM_SUCCESS)
    {
        IPCOM_LOG0(ERR, "ipnet_nat_init() :: Failed to add PPTP ALG");
        goto fail;
    }
    errval = ipnet_nat_add_proxy("pptp", "tcp", ipnet_nat_proxy_pptp, IP_NULL);
    if (errval != IPCOM_SUCCESS)
    {
        IPCOM_LOG0(ERR, "ipnet_nat_init() :: Failed to add PPTP ALG");
        goto fail;
    }
#endif  /* #ifdef IPNET_USE_NAT_PPTP_ALG  */

#ifdef IPNET_USE_NAT_DNS_ALG
    errval = ipnet_nat_add_proxy("dns", "udp", ipnet_nat_proxy_dns, IP_NULL);
    if (errval != IPCOM_SUCCESS)
    {
        IPCOM_LOG0(ERR, "ipnet_nat_init() :: Failed to add DNS ALG");
        goto fail;
    }
#endif  /* #ifdef IPNET_USE_NAT_DNS_ALG  */

#ifdef IPNET_USE_NAT_IPSEC_ALG
    errval = ipnet_nat_add_proxy("ipsec", "esp", ipnet_nat_proxy_ipsec, IP_NULL);
    if (errval != IPCOM_SUCCESS)
    {
        IPCOM_LOG0(ERR, "ipnet_nat_init() :: Failed to add IPSEC ALG");
        goto fail;
    }
    errval = ipnet_nat_add_proxy("ipsec", "udp", ipnet_nat_proxy_ipsec, IP_NULL);
    if (errval != IPCOM_SUCCESS)
    {
        IPCOM_LOG0(ERR, "ipnet_nat_init() :: Failed to add IPSEC ALG");
        goto fail;
    }
#endif  /* #ifdef IPNET_USE_NAT_IPSEC_ALG */

#ifdef IPNET_USE_NAT_TRIGGER_ALG
    errval = ipnet_nat_add_proxy("example_tcp", "tcp", ipnet_nat_proxy_trigger, &example_trigger);
    if (errval != IPCOM_SUCCESS)
    {
        IPCOM_LOG0(ERR, "ipnet_nat_init() :: Failed to add TRIGGER ALG");
        goto fail;
    }
    errval = ipnet_nat_add_proxy("example_udp", "udp", ipnet_nat_proxy_trigger, &example_trigger);
    if (errval != IPCOM_SUCCESS)
    {
        IPCOM_LOG0(ERR, "ipnet_nat_init() :: Failed to add TRIGGER ALG");
        goto fail;
    }
#endif  /* #ifdef IPNET_USE_NAT_TRIGGER_ALG */

    /* Enable NAT */
    if(ipnet_nat_internal_enable() != 0)
    {
        errval = IPCOM_ERR_FAILED;
        goto fail;
    }

#ifdef IPNET_DEBUG
    ipnet_timeout_to_string((Ipnet_timeout_handler) ipnet_nat_mapping_timeout,
                            (Ipnet_timeout_to_string_f) ipnet_nat_mapping_to_string);
#endif

    return IPCOM_SUCCESS;

fail:
    if(ipnet->nat.hash_out_port != IP_NULL)
        ipcom_hash_delete(ipnet->nat.hash_out_port);
    if(ipnet->nat.hash_out_noport != IP_NULL)
        ipcom_hash_delete(ipnet->nat.hash_out_noport);
    if(ipnet->nat.hash_in_port != IP_NULL)
        ipcom_hash_delete(ipnet->nat.hash_in_port);
    if(ipnet->nat.hash_in_noport != IP_NULL)
        ipcom_hash_delete(ipnet->nat.hash_in_noport);
    return errval;
}


/*
 *===========================================================================
 *                    ipnet_nat_input_hook
 *===========================================================================
 * Description: Input hook for NAT.
 * Parameters:  pktp  - pointer to pointer to packet.
 *              param - pointer to the output parameters.
 * Returns:     IPNET_NAT_PT_TRANSLATED
 *              IPNET_NAT_TRANSLATED
 *              IPNET_NAT_NOMATCH
 *              IPNET_NAT_INVALID
 *              IPNET_NAT_DROP
 */
IP_GLOBAL Ip_s32
ipnet_nat_input_hook(Ipcom_pkt **pktp, void *param)
{
    Ip_s32 ret;

    /* Check that nat is enabled and that least one rule is defined */
    if(ipnet->nat.inner.open == 0 || ipnet->nat.head_rule.size == 0)
        return IPNET_NAT_NOMATCH;

    ret = ipnet_nat_loop_rules(IP_TRUE, IP_FALSE, pktp, param);
    return ret;
}


/*
 *===========================================================================
 *                    ipnet_nat_output_hook
 *===========================================================================
 * Description: Output hook for NAT.
 * Parameters:  pktp     - pointer to pointer to packet.
 *              no_rules - boolean that controls skipping of NAT rules
 *                         and thus only match against existing mappings.
 * Returns:     IPNET_NAT_PT_TRANSLATED
 *              IPNET_NAT_TRANSLATED
 *              IPNET_NAT_NOMATCH
 *              IPNET_NAT_INVALID
 *              IPNET_NAT_DROP
 */
IP_GLOBAL Ip_s32
ipnet_nat_output_hook(Ipcom_pkt **pktp, Ip_bool no_rules)
{
    Ip_s32 ret;

    /* Check that nat is enabled and that least one rule is defined */
    if(ipnet->nat.inner.open == 0 || ipnet->nat.head_rule.size == 0)
        return IPNET_NAT_NOMATCH;

    (*pktp)->start = (int)((*pktp)->ipstart + (((*pktp)->data[(*pktp)->ipstart] & 0x0F) << 2));
    ret = ipnet_nat_loop_rules(IP_FALSE, no_rules, pktp, IP_NULL);
    (*pktp)->start = (*pktp)->ipstart;
    return ret;
}


/*
 ****************************************************************************
 * 11                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipnet_nat_proxy_get_time
 *===========================================================================
 * Description: Get elapsed time since boot in seconds.
 * Parameters:  none.
 * Returns:     The number of seconds since boot.
 */
IP_PUBLIC Ip_u32
ipnet_nat_proxy_get_time(void)
{
    return ipnet_sec_now();
}


/*
 *===========================================================================
 *                    ipnet_nat_proxy_timeout_schedule
 *===========================================================================
 * Description: Schedules a function that will be called after sec
 *              seconds.
 * Parameters:  sec     - seconds until the timeout will occur.
 *              handler - the timeout handler or IP_NULL if no action should
 *                        be done when the timer expires.
 *              cookie  - the user defined argument to the handler.
 *              ptmo    - the location where the timeout handler should be stored
 * Returns:     0 = success, <0 = error.
 */
IP_PUBLIC int
ipnet_nat_proxy_timeout_schedule(Ip_u32 sec,
                                 Ipnet_nat_proxy_timeout_handler handler,
                                 void *cookie,
                                 void **ptmo)
{
    return ipnet_timeout_schedule(sec * 1000, handler, cookie, (struct Ipnet_timeout_struct **)ptmo);
}


/*
 *===========================================================================
 *                    ipnet_nat_proxy_timeout_cancel
 *===========================================================================
 * Description: Cancels a scheduled timeout.
 * Parameters:  tmo - the timeout event to remove.
 * Returns:     nothing.
 */
IP_PUBLIC void
ipnet_nat_proxy_timeout_cancel(void *tmo)
{
    ip_assert(tmo != IP_NULL);
    ipnet_timeout_cancel(tmo);
}


/*
 *===========================================================================
 *                    ipnet_nat_proxy_timeout_reschedule
 *===========================================================================
 * Description: Reschedules a running timer with a new timeout.
 * Parameters:  sec     - seconds until the timeout will occur.
 *              handler - the timeout handler or IP_NULL if no action should
 *                        be done when the timer expires.
 *              cookie  - the user defined argument to the handler.
 *              ptmo    - the location where the timeout handler should be stored
 * Returns:     0 = success, <0 = error.
 */
IP_PUBLIC int
ipnet_nat_proxy_timeout_reschedule(Ip_u32 sec,
                                   Ipnet_nat_proxy_timeout_handler handler,
                                   void *cookie,
                                   void **ptmo)
{
    ipnet_nat_proxy_timeout_cancel(*ptmo);
    return ipnet_nat_proxy_timeout_schedule(sec, handler, cookie, ptmo);
}


/*
 *===========================================================================
 *                    ipnet_nat_proxy_add_mapping
 *===========================================================================
 * Description: This function adds a mapping between a host on the private
 *              side of the NAT and a host on the public side of the NAT.
 *              This is typically used to open a port through the NAT which
 *              is typically closed for incoming connections. It can
 *              also be used to open an outgoing path through the NAT
 *              if there is no matching rule.
 *
 *              The 'proxy_tuple' parameter specifies the private and public
 *              addresses and ports as well as the protocol. If the source
 *              port of the connecting host is unknown it can be set to zero
 *              to allow any source port. If the source address of the
 *              connecting host is unknown it can be set to zero to allow
 *              any host. Once the mapping has been hit it can only be used
 *              by that specific host. Wildcarding both the private and public
 *              side is not allowed. The protocol can be any protocol but ICMP.
 *              For non TCP/UDP protcols the ports must be set to zero.
 *              Additionally, the source address can be set to zero to allow any
 *              host to connect.
 *
 *              The source port will be the same on the private host and the NAT gateway
 *              unless 'use_napt' is IP_TRUE which has the effect that a new port is
 *              automatically allocated and port translation will take place.
 *
 *              Optionally packets matching the mapping can be configured to call
 *              an application proxy if the 'proxy_func' and 'proxy_cookie' parameters
 *              are set.
 *
 * Parameters:  proxy_tuple  - pointer to the proxy tuple.
 *              timeout      - timeout until the port is closed if no packets
 *                             arrive (0 means that default values will be applied)
 *              parent       - Pointer to the NAT mapping that caused the call
 *                             to the proxy function. Must be the same pointer
 *                             value as received in param->mapping.
 *              use_napt     - IP_TRUE to use port translation.
 *              use_inbound  - IP_TRUE to indicate that session will start inbound.
 *              proxy_func   - Proxy function (optional).
 *              proxy_cookie - Proxy cookie (optional).
 *
 * Returns:     The NAT port or -1 if failed
 */
IP_PUBLIC int
ipnet_nat_proxy_add_mapping(Ipnet_nat_proxy_tuple *proxy_tuple,
                            Ip_u32                 timeout,
                            void                  *parent,
                            Ip_bool                use_napt,
                            Ip_bool                use_inbound,
                            Ipnet_nat_proxy_func   proxy_func,
                            void                  *proxy_cookie)
{
    Ipnet_nat_mapping *nat_mapping;
    Ipnet_nat_tuple    nat_tuple;
#ifdef IPCOM_USE_SYSLOG
    char protostr[16];
#endif

    /* Check protocol */
    if (proxy_tuple->protocol == IP_IPPROTO_ICMP)
    {
        IPCOM_LOG1(ERR, "ipnet_nat_proxy_add_mapping() :: invalid protocol (%d)", proxy_tuple->protocol);
        return -1;
    }

    /* Check ports and addresses for sanity */
    if (proxy_tuple->protocol == IP_IPPROTO_TCP || proxy_tuple->protocol == IP_IPPROTO_UDP)
    {
        /* Both ports must not be zero */
        if (proxy_tuple->private_port == 0 && proxy_tuple->public_port == 0)
        {
            IPCOM_LOG3(ERR, "ipnet_nat_proxy_add_mapping() :: invalid private port/public port (%d/%d) for protocol=%s",
                                                              proxy_tuple->private_port,
                                                              proxy_tuple->public_port,
                                                              ipnet_nat_protocol_name(proxy_tuple->protocol, protostr, sizeof(protostr)));
            return -1;
        }
    }
    else
    {
        /* Both ports must be zero */
        if (proxy_tuple->private_port != 0 || proxy_tuple->public_port != 0)
        {
            IPCOM_LOG3(ERR, "ipnet_nat_proxy_add_mapping() :: invalid private port/public port (%d/%d) for protocol=%s",
                                                              proxy_tuple->private_port,
                                                              proxy_tuple->public_port,
                                                              ipnet_nat_protocol_name(proxy_tuple->protocol, protostr, sizeof(protostr)));
            return -1;
        }
    }

    /* Both addresses must not be zero */
    if (proxy_tuple->private_addr == 0 && proxy_tuple->public_addr == 0)
    {
        IPCOM_LOG3(ERR, "ipnet_nat_proxy_add_mapping() :: invalid private address/public address (0x%08x/0x%08x) for protocol=%s",
                                                          proxy_tuple->private_addr,
                                                          proxy_tuple->public_addr,
                                                          ipnet_nat_protocol_name(proxy_tuple->protocol, protostr, sizeof(protostr)));
        return -1;
    }

    if (proxy_tuple->private_port != 0 && proxy_tuple->private_addr == 0)
    {
        IPCOM_LOG3(ERR, "ipnet_nat_proxy_add_mapping() :: invalid private port/private address (%d/0x%08x) for protocol=%s",
                                                          proxy_tuple->private_port,
                                                          proxy_tuple->private_addr,
                                                          ipnet_nat_protocol_name(proxy_tuple->protocol, protostr, sizeof(protostr)));
        return -1;
    }

    if (proxy_tuple->public_port != 0 && proxy_tuple->public_addr == 0)
    {
        IPCOM_LOG3(ERR, "ipnet_nat_proxy_add_mapping() :: invalid public port/public address (%d/0x%08x) for protocol=%s",
                                                          proxy_tuple->public_port,
                                                          proxy_tuple->public_addr,
                                                          ipnet_nat_protocol_name(proxy_tuple->protocol, protostr, sizeof(protostr)));
        return -1;
    }

    /* Prepare tuple */
    ipcom_memset(&nat_tuple, 0, sizeof(nat_tuple));
    nat_tuple.protocol   = proxy_tuple->protocol;
    nat_tuple.src_port_n = ip_htons(proxy_tuple->private_port);
    nat_tuple.dst_port_n = ip_htons(proxy_tuple->public_port);
    nat_tuple.src_n      = ip_htonl(proxy_tuple->private_addr);
    nat_tuple.dst_n      = ip_htonl(proxy_tuple->public_addr);
#ifdef IPNET_USE_NAT_PT
    ipcom_memcpy(&nat_tuple.pt_dst, &((Ipnet_nat_mapping *)parent)->inner.tuple.pt_dst, sizeof(nat_tuple.pt_dst));
    ipcom_memcpy(&nat_tuple.pt_src, &((Ipnet_nat_mapping *)parent)->inner.tuple.pt_src, sizeof(nat_tuple.pt_src));
    nat_tuple.pt_src.in6.addr32[3] = ip_htonl(proxy_tuple->private_addr);
#endif

    /* Try to find the mapping */
    nat_mapping = ipnet_nat_find_mapping(IP_FALSE, IP_NULL, &nat_tuple);
    if (nat_mapping != IP_NULL)
    {
        /* Mapping already added - just reset the timeout */
        IPCOM_LOG0(DEBUG, "ipnet_nat_proxy_add_mapping() :: mapping already added - reset timeout only");
        ipnet_nat_reset_mapping_timeout(nat_mapping);
        return (int)ip_ntohs(nat_mapping->inner.nat_port_n);
    }

    /* Add the mapping */
    nat_mapping = ipnet_nat_add_mapping(use_inbound,
                                        ((Ipnet_nat_mapping *)parent)->rule,
                                        &nat_tuple,
                                        timeout,
                                        (Ipnet_nat_mapping *)parent,
                                        use_napt,
                                        proxy_func,
                                        proxy_cookie);
    if(nat_mapping == IP_NULL)
    {
        IPCOM_LOG0(ERR, "ipnet_nat_proxy_add_mapping() :: Failed");
        return -1;
    }

    return (int)ip_ntohs(nat_mapping->inner.nat_port_n);
}


/*
 *===========================================================================
 *                    ipnet_nat_proxy_set_mapping_timeout
 *===========================================================================
 * Description: Sets the timeout of a mapping.
 * Parameters:  sec      - Seconds until the mapping will timeout.
 *              mapping  - Pointer to the NAT mapping that caused the call
 *                         to the proxy function. Must be the same pointer
 *                         value as received in param->mapping.
 * Returns:     nothing.
 */
IP_PUBLIC void
ipnet_nat_proxy_set_mapping_timeout(Ip_u32 sec, void *mapping)
{
    Ipnet_nat_mapping *nat_mapping;

    nat_mapping = (Ipnet_nat_mapping *)mapping;
    nat_mapping->inner.tmo_int_sec = sec;
    ipnet_nat_reset_mapping_timeout(nat_mapping);
}


/*
 *===========================================================================
 *                    ipnet_nat_add_proxy
 *===========================================================================
 * Description: Adds a proxy (ALG) to the NAT.
 * Parameters:  label  - ascii string identifier
 *              proto  - The IP protocol the proxy applies to. Either a
 *                       protocol name or a numerical string is allowed.
 *              func   - Pointer to the ALG function
 *              cookie - A cookie that will be supplied in the call to
 *                       the proxy function. The memory location referred to
 *                       by the cookie must be valid as long as the proxy
 *                       has not been removed.
 * Returns:     IPCOM_SUCCESS         - Proxy successfully added.
 *              IPCOM_ERR_INVALID_ARG - Invalid arguments.
 *              IPCOM_ERR_NO_MEMORY   - Out of memory.
 *              IPCOM_ERR_DUPLICATE   - Proxy already added.
 *              IPCOM_ERR_REACHED_MAX - Too many proxies.
 *              IPCOM_ERR_FAILED      - General failure.
 */
IP_PUBLIC Ip_err
ipnet_nat_add_proxy(const char *label, const char *proto,
                    Ipnet_nat_proxy_func func, void *cookie)
{
    Ipnet_nat_proxy *nat_proxy;
    long protocol;
    char *suff;
    void *suspend_handle;
    Ip_err err = IPCOM_SUCCESS;

    if (label == IP_NULL || proto == IP_NULL || func == IP_NULL)
    {
        err = IPCOM_ERR_INVALID_ARG;
        IPCOM_LOG1(ERR, "ipnet_nat_add_proxy() :: Failed (%s)", ipcom_err_string(err));
        return err;
    }

    if(ipnet->nat.head_proxy.size >= IPNET_NAT_MAX_PROXIES)
    {
        err = IPCOM_ERR_REACHED_MAX;
        IPCOM_LOG1(ERR, "ipnet_nat_add_proxy() :: Failed (%s)", ipcom_err_string(err));
        return err;
    }

    protocol = ipnet_nat_protocol_number(proto);
    if (protocol < 0)
    {
        protocol = ipcom_strtol(proto, &suff, 10);
        if(protocol < 0 || protocol > 255 || proto == suff || *suff != '\0')
        {
            err = IPCOM_ERR_FAILED;
            IPCOM_LOG3(ERR, "ipnet_nat_add_proxy() :: Failed (%s): %s %s",
                        ipcom_err_string(err), label, proto);
            return err;
        }
    }

    if (protocol == IP_IPPROTO_ICMP || protocol == IP_IPPROTO_ICMPV6)
    {
        err = IPCOM_ERR_INVALID_ARG;
        IPCOM_LOG1(ERR, "ipnet_nat_add_proxy() :: Failed (Invalid protocol): %s", proto);
        return err;
    }

    /* Check for duplicates */
    suspend_handle = ipnet_suspend_stack();
    if (suspend_handle == IP_NULL)
    {
        err = IPCOM_ERR_FAILED;
        IPCOM_LOG0(ERR, "ipnet_nat_add_proxy() :: Failed (Could not suspend stack)");
        return err;
    }
    for (nat_proxy = IPCOM_LIST_FIRST(&ipnet->nat.head_proxy);
         nat_proxy != IP_NULL;
         nat_proxy = IPCOM_LIST_NEXT(&nat_proxy->list_proxy))
    {
        if (ipcom_strcmp(nat_proxy->inner.label, label) == 0
            && nat_proxy->inner.protocol == (Ip_u8)protocol)
            {
                err = IPCOM_ERR_DUPLICATE;
                IPCOM_LOG3(ERR, "ipnet_nat_add_proxy() :: Failed (%s): %s %s",
                    ipcom_err_string(err), label, proto);
                ipnet_resume_stack(suspend_handle);
                return err;
            }
    }

    /* Allocate space for the proxy and insert in list */
    nat_proxy = ipcom_malloc(sizeof(*nat_proxy));
    if(nat_proxy == IP_NULL)
    {
        err = IPCOM_ERR_NO_MEMORY;
        IPCOM_LOG1(ERR, "ipnet_nat_add_proxy() :: Failed (%s)", ipcom_err_string(err));
        ipnet_resume_stack(suspend_handle);
        return err;
    }

    ipcom_memset(nat_proxy, 0, sizeof(*nat_proxy));
    ipcom_strncpy(nat_proxy->inner.label, label, sizeof(nat_proxy->inner.label)-1);
    nat_proxy->inner.protocol = (Ip_u8)protocol;
    nat_proxy->func = func;
    nat_proxy->cookie = cookie;
    ipcom_list_insert_last(&ipnet->nat.head_proxy, &nat_proxy->list_proxy);
    ipnet_resume_stack(suspend_handle);

    return err;
}


/*
 *===========================================================================
 *                    ipnet_nat_remove_proxy
 *===========================================================================
 * Description: Removes a proxy (ALG) from the NAT
 * Parameters:  label  - ascii string identifier
 *              proto  - The IP protocol the proxy applies to. Either
 *                       a protocol name or a numerical string is allowed.
 * Returns:     IPCOM_SUCCESS         - Proxy successfully added.
 *              IPCOM_ERR_INVALID_ARG - Invalid arguments.
 *              IPCOM_ERR_NOT_FOUND   - Did not find proxy.
 *              IPCOM_ERR_FAILED      - General failure.
 */
IP_PUBLIC Ip_err
ipnet_nat_remove_proxy(const char *label, const char *proto)
{
    Ipnet_nat_proxy *nat_proxy;
    long protocol;
    char *suff;
    void *suspend_handle;
    Ip_err err = IPCOM_SUCCESS;

    if (label == IP_NULL || proto == IP_NULL)
    {
        err = IPCOM_ERR_INVALID_ARG;
        IPCOM_LOG1(ERR, "ipnet_nat_remove_proxy() :: Failed (%s)", ipcom_err_string(err));
        return err;
    }

    /* Convert protocol */
    protocol = ipnet_nat_protocol_number(proto);
    if (protocol < 0)
    {
        protocol = ipcom_strtol(proto, &suff, 10);
        if(protocol < 0 || protocol > 255 || proto == suff || *suff != '\0')
        {
            err = IPCOM_ERR_FAILED;
            IPCOM_LOG3(ERR, "ipnet_nat_remove_proxy() :: Failed (%s): %s %s",
                        ipcom_err_string(err), label, proto);
            return err;
        }
    }

    /* Locate proxy */
    suspend_handle = ipnet_suspend_stack();
    if (suspend_handle == IP_NULL)
    {
        err = IPCOM_ERR_FAILED;
        IPCOM_LOG0(ERR, "ipnet_nat_remove_proxy() :: Failed (Could not suspend stack)");
        return err;
    }
    for (nat_proxy = IPCOM_LIST_FIRST(&ipnet->nat.head_proxy);
         nat_proxy != IP_NULL;
         nat_proxy = IPCOM_LIST_NEXT(&nat_proxy->list_proxy))
    {
        if (ipcom_strcmp(nat_proxy->inner.label, label) == 0
            && nat_proxy->inner.protocol == (Ip_u8)protocol)
            {
                break;
            }
    }

    if (nat_proxy == IP_NULL)
    {
        err = IPCOM_ERR_NOT_FOUND;
        IPCOM_LOG3(ERR, "ipnet_nat_remove_proxy() :: Failed (%s): %s %s",
                    ipcom_err_string(err), label, proto);
        ipnet_resume_stack(suspend_handle);
        return err;
    }

    /* Remove proxy and free memeory */
    ipcom_list_remove(&nat_proxy->list_proxy);
    ipcom_free(nat_proxy);
    ipnet_resume_stack(suspend_handle);
    return err;
}

/*****************************************************************************
 函 数 名  : IPS_MNTN_CCORE_GetNatRules
 功能描述  : 获取NAT表规则信息
 输入参数  : pucData        -   数据指针
             pulTotalLen    -   总信息长度
 输出参数  : 无
 返 回 值  : OK         -   获取系统信息成功
             ERROR      -   获取系统信息失败
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年8月3日
    作    者   : 冯家麟
    修改内容   : 新生成函数
  2.日    期   : 2013年7月30日
    作    者   : caikai
    修改内容   : VoLTE特性需要将其移植至V9

*****************************************************************************/
IP_PUBLIC int IPS_MNTN_CCORE_GetNatRules
(
    Ip_u8                              *pucData,
    Ip_u32                             *pulTotalLen,
    Ip_u16                              usType
)
{
    struct Ipnet_nat_rule_inner     *pstNatRule;
    Ipnet_nat_ctrl                   stCtrl;
    Ip_socklen_t                     len;
    Ip_s32                           lSeqNo;
    Ip_u8                           *pucLenCur;
    Ip_u32                           ulSysDataLen;
    Ip_u32                           ulLenTmp;

    lSeqNo  = 0;

    /*填充消息类型*/
    ipcom_memcpy(pucData, &usType, IPS_MNTN_CCORE_INFO_TYPE_LEN);
    pucData                 +=  IPS_MNTN_CCORE_INFO_TYPE_OFFSET;
    (*pulTotalLen)          +=  IPS_MNTN_CCORE_INFO_TYPE_OFFSET;

    /*记录填充信息长度的游标地址*/
    pucLenCur               =   pucData;
    pucData                 +=  IPS_MNTN_CCORE_INFO_LEN_OFFSET;
    (*pulTotalLen)          +=  IPS_MNTN_CCORE_INFO_LEN_OFFSET;

    ulSysDataLen            =   0;

    /* 获取NAT规则列表 */
    stCtrl.cmd    = IPNET_NAT_CTRL_GET_RULE;
    stCtrl.seqno  = lSeqNo;
    while(ipnet_nat_internal_get_rule(&stCtrl, &len) == 0
        && len == sizeof(Ipnet_nat_ctrl))
    {
        /*获取NAT表规则信息*/
        pstNatRule = &stCtrl.type.rule_info;
        stCtrl.seqno = ++lSeqNo;
        ip_assert(lSeqNo <= IPNET_NAT_MAX_RULES);

        /*如果加入该项信息，可维可测信息总长度超预期，则获取信息失败*/
        ulLenTmp = *pulTotalLen + ulSysDataLen + sizeof(struct Ipnet_nat_rule_inner);
        if(IPS_MNTN_CCORE_INFO_LEN < ulLenTmp)
        {
            *((Ip_u32 *)pucLenCur) =    (Ip_u32)(ulSysDataLen);
            (*pulTotalLen)         +=   ulSysDataLen;
            ipcom_printf("IPS_MNTN_CCORE_GetNatRules: len is over expect."IP_LF);
            return -1;
        }

        /*填充NAT表规则信息*/
        ipcom_memcpy(pucData, pstNatRule, sizeof(struct Ipnet_nat_rule_inner));
        pucData         +=  sizeof(struct Ipnet_nat_rule_inner);
        ulSysDataLen    +=  sizeof(struct Ipnet_nat_rule_inner);
    }

    /*填充NAT表规则总长度*/
    ipcom_memcpy(pucLenCur, &ulSysDataLen, IPS_MNTN_CCORE_INFO_LEN_OFFSET);
    (*pulTotalLen)         +=   ulSysDataLen;

    return 0;
}

/*****************************************************************************
 函 数 名  : IPS_MNTN_CCORE_GetNatStats
 功能描述  : 获取NAT表规则信息
 输入参数  : pucData        -   数据指针
             pulTotalLen    -   总信息长度
 输出参数  : 无
 返 回 值  : OK         -   获取系统信息成功
             ERROR      -   获取系统信息失败
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年8月3日
    作    者   : 冯家麟
    修改内容   : 新生成函数
  2.日    期   : 2013年7月30日
    作    者   : caikai
    修改内容   : VoLTE特性需要将其移植至V9

*****************************************************************************/
IP_PUBLIC int IPS_MNTN_CCORE_GetNatStats
(
    Ip_u8                              *pucData,
    Ip_u32                             *pulTotalLen,
    Ip_u16                              usType
)

{
    Ipnet_nat_ctrl              stCtrl;
    Ip_socklen_t                len;
    Ip_u8                      *pucLenCur;
    Ip_u32                      ulSysDataLen;
    Ip_u32                      ulLenTmp;

    /*填充消息类型*/
    ipcom_memcpy(pucData, &usType, IPS_MNTN_CCORE_INFO_TYPE_LEN);
    pucData                 +=  IPS_MNTN_CCORE_INFO_TYPE_OFFSET;
    (*pulTotalLen)          +=  IPS_MNTN_CCORE_INFO_TYPE_OFFSET;

    /*记录填充信息长度的游标地址*/
    pucLenCur               =   pucData;
    pucData                 +=  IPS_MNTN_CCORE_INFO_LEN_OFFSET;
    (*pulTotalLen)          +=  IPS_MNTN_CCORE_INFO_LEN_OFFSET;

    ulSysDataLen            =   0;

    /*获取NAT表统计信息*/
    stCtrl.cmd = IPNET_NAT_CTRL_GET_INFO;
    if(ipnet_nat_internal_get_info(&stCtrl, &len) == 0
        && len == sizeof(Ipnet_nat_ctrl))
    {
        /*如果加入该项信息，可维可测信息总长度超预期，则获取信息失败*/
        ulLenTmp = *pulTotalLen + ulSysDataLen + sizeof(Ipnet_nat_data);
        if(IPS_MNTN_CCORE_INFO_LEN < ulLenTmp)
        {
            ipcom_memcpy(pucLenCur, &ulSysDataLen, IPS_MNTN_CCORE_INFO_LEN_OFFSET);
            (*pulTotalLen)         +=   ulSysDataLen;
            ipcom_printf("IPS_MNTN_CCORE_GetNatStats: len is over expect."IP_LF);
            return -1;
        }

        /*填充NAT表统计信息*/
        ipcom_memcpy(pucData, &(stCtrl.type.info), sizeof(Ipnet_nat_data));
        ulSysDataLen    +=  sizeof(Ipnet_nat_data);
    }

    /*填充NAT表统计信息总长度*/
    ipcom_memcpy(pucLenCur, &ulSysDataLen, IPS_MNTN_CCORE_INFO_LEN_OFFSET);
    (*pulTotalLen)         +=   ulSysDataLen;

    return 0;
}

/*****************************************************************************
 函 数 名  : IPS_MNTN_CCORE_GetNatProxies
 功能描述  : 获取NAT表协议信息
 输入参数  : pucData        -   数据指针
             pulTotalLen    -   总信息长度
 输出参数  : 无
 返 回 值  : OK         -   获取系统信息成功
             ERROR      -   获取系统信息失败
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年8月3日
    作    者   : 冯家麟
    修改内容   : 新生成函数
  2.日    期   : 2013年7月30日
    作    者   : caikai
    修改内容   : VoLTE特性需要将其移植至V9

*****************************************************************************/
IP_PUBLIC int IPS_MNTN_CCORE_GetNatProxies
(
    Ip_u8                              *pucData,
    Ip_u32                             *pulTotalLen,
    Ip_u16                              usType
)
{
    struct Ipnet_nat_proxy_inner       *pstNatProxy;
    Ipnet_nat_ctrl                      stCtrl;
    Ip_socklen_t                        len;
    Ip_s32                              lSeqNo;
    Ip_u8                              *pucLenCur;
    Ip_u32                              ulSysDataLen;
    Ip_u32                              ulLenTmp;

    lSeqNo  = 0;

    /*填充消息类型*/
    ipcom_memcpy(pucData, &usType, IPS_MNTN_CCORE_INFO_TYPE_LEN);
    pucData                 +=  IPS_MNTN_CCORE_INFO_TYPE_OFFSET;
    (*pulTotalLen)          +=  IPS_MNTN_CCORE_INFO_TYPE_OFFSET;

    /*记录填充信息长度的游标地址*/
    pucLenCur               =   pucData;
    pucData                 +=  IPS_MNTN_CCORE_INFO_LEN_OFFSET;
    (*pulTotalLen)          +=  IPS_MNTN_CCORE_INFO_LEN_OFFSET;

    ulSysDataLen            =   0;

    stCtrl.cmd    = IPNET_NAT_CTRL_GET_PROXY;
    stCtrl.seqno  = lSeqNo;
    while(ipnet_nat_internal_get_proxy(&stCtrl, &len) == 0
        && len == sizeof(Ipnet_nat_ctrl))
    {
        /*获取NAT表协议信息*/
        pstNatProxy = &stCtrl.type.proxy_info;
        stCtrl.seqno = ++lSeqNo;
        ip_assert(lSeqNo <= IPNET_NAT_MAX_PROXIES);

       /*如果加入该项信息，可维可测信息总长度超预期，则获取信息失败*/
        ulLenTmp = *pulTotalLen + ulSysDataLen + sizeof(struct Ipnet_nat_proxy_inner);
        if(IPS_MNTN_CCORE_INFO_LEN < ulLenTmp)
        {
            ipcom_memcpy(pucLenCur, &ulSysDataLen, IPS_MNTN_CCORE_INFO_LEN_OFFSET);
            (*pulTotalLen)         +=   ulSysDataLen;
            ipcom_printf("IPS_MNTN_CCORE_GetNatProxies: len is over expect."IP_LF);
            return -1;
        }

        /*填充NAT表规则信息*/
        ipcom_memcpy(pucData, pstNatProxy, sizeof(struct Ipnet_nat_proxy_inner));
        pucData         +=  sizeof(struct Ipnet_nat_proxy_inner);
        ulSysDataLen    +=  sizeof(struct Ipnet_nat_proxy_inner);
    }

    /*填充NAT表规则总长度*/
    ipcom_memcpy(pucLenCur, &ulSysDataLen, IPS_MNTN_CCORE_INFO_LEN_OFFSET);
    (*pulTotalLen)         +=   ulSysDataLen;

    return 0;
}

/*****************************************************************************
 函 数 名  : IPS_MNTN_CCORE_GetNatMappings
 功能描述  : 提供给协议栈，获取Nat Mapping表中的指定数目的信息
 输入参数  : pucData    存放信息的起始地址
             ulStartNum 本次获取的NAT MAPPING信息的起始条数
             ulEndNum   本次获取的NAT MAPPING信息的终止条数
 输出参数  : 无
 返 回 值  : OK         -   获取系统信息成功
             ERROR      -   获取系统信息失败
 调用函数  : 指定条数的NAT MAPPING信息
 被调函数  :

 修改历史      :
  1.日    期   : 2011年8月3日
    作    者   : 冯家麟
    修改内容   : 新生成函数
  2.日    期   : 2013年7月30日
    作    者   : caikai
    修改内容   : VoLTE特性需要将其移植至V9

*****************************************************************************/
IP_PUBLIC int IPS_MNTN_CCORE_GetNatMappings
(
    unsigned char                  *pucData,
    int                             ulNum
)
{
    Ipnet_nat_mapping          *pstNatMappingNode = IP_NULL;
    IPS_NAT_MAPPING_TRACE_STRU  stNatMappingTraceNode;

    pstNatMappingNode   = (Ipnet_nat_mapping *)ipnet_nat_mapping_get(ulNum);

    if (pstNatMappingNode == IP_NULL)
    {
        return -1;
    }

    stNatMappingTraceNode.ucInnerStates     =   pstNatMappingNode->inner.inner_state;
    stNatMappingTraceNode.ucOuterStates     =   pstNatMappingNode->inner.outer_state;
    stNatMappingTraceNode.ulTimeToAlive     =   pstNatMappingNode->inner.tmo_abs_sec;
    stNatMappingTraceNode.ucProtocol        =   pstNatMappingNode->inner.tuple.protocol;
    stNatMappingTraceNode.ulSrcAddr         =   pstNatMappingNode->inner.tuple.src_n;
    stNatMappingTraceNode.usSrcPort         =   pstNatMappingNode->inner.tuple.src_port_n;
    stNatMappingTraceNode.ulDstAddr         =   pstNatMappingNode->inner.tuple.dst_n;
    stNatMappingTraceNode.usDstPort         =   pstNatMappingNode->inner.tuple.dst_port_n;
    stNatMappingTraceNode.ulNatTranSrcAddr  =   pstNatMappingNode->inner.nat_addr_n;
    stNatMappingTraceNode.usNatTransSrcPort =   pstNatMappingNode->inner.nat_port_n;
    stNatMappingTraceNode.ulSeqStart        =   pstNatMappingNode->inner.seq_start;
    stNatMappingTraceNode.ulCurrDelta       =   pstNatMappingNode->inner.curr_delta;
    stNatMappingTraceNode.ulPrevDelta       =   pstNatMappingNode->inner.prev_delta;

    /*填充NAT表映射信息*/
    ipcom_memcpy(pucData, &stNatMappingTraceNode, sizeof(IPS_NAT_MAPPING_TRACE_STRU));

    return 0;
}

#else
int ipnet_nat_empty_file;
#endif /* IPNET_USE_NAT */


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
