/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_forwarder_data.c,v $ $Revision: 1.2.8.2 $
 *     $Source: /home/interpeak/CVSRoot/ipcom/src/ipcom_forwarder_data.c,v $
 *     $Author: ulf $
 *     $State: Exp $ $Locker:  $
 *
 *     INTERPEAK_COPYRIGHT_STRING
 ******************************************************************************
 */

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 *
 */

/*
 ****************************************************************************
 * 2                    CONFIGURATION
 ****************************************************************************
 */

#define IPCOM_FORWARDER_DATA_C

#include <ipcom_config.h>

#ifdef IPCOM_USE_FORWARDER

/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

/* #define FW_DEBUG */

/* Ipcom include files */
#define IPCOM_USE_CLIB_PROTO
#include <ipcom_clib.h>
#include <ipcom_err.h>
#include <ipcom_os.h>

#include <ipcom_cstyle.h>
#include <ipcom_netif.h>
#include <ipcom_pkt.h>
#include <ipcom_syslog.h>
#include <ipcom_type.h>
#include <ipcom_pipe.h>
#include <ipcom_shell.h>
#include <ipcom_getopt.h>
#include <ipcom_atomic.h>


#include <ipnet.h>
#include <ipnet_config.h>
#include <ipnet_netif.h>
#include <ipnet_eth.h>
#include <ipnet_route.h>
#include <ipnet_usr_sock.h>

#include <ipcom_forwarder.h>

#ifdef IPIPSEC2
#include <ipipsec_config.h>
#endif


/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

#define GET_V6_HOPL(p) ((Ip_u8 *)p)[7]
#define SET_V6_HOPL(p, t) (((Ip_u8 *)p)[7] = t)

#ifdef IP_PORT_LKM
extern void printk(const char *fmt, ...);
#define ipcom_printf printk
#endif


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


/* In ipcom_config.c */
IP_EXTERN void ipcom_forwarder_startup_delay(void);

/*
 ****************************************************************************
 * 7                    LOCAL PROTOTYPES
 ****************************************************************************
 */

IP_STATIC __inline__ int ipcom_forwarder_get_fw_if(fw_msg_t *msg);

IPCOM_PROCESS(ipcom_forwarder);

/*
 ****************************************************************************
 * 8                    DATA
 ****************************************************************************
 */

IP_EXTERN Ipcom_fw_t ipcom_fw;

#ifdef IPCOM_USE_FORWARDER_IPSEC
IP_PUBLIC unsigned long long ipsec_out_ok;
IP_PUBLIC unsigned long long ipsec_out_skip;
IP_PUBLIC unsigned long long ipsec_out_fail;
IP_PUBLIC unsigned long long ipsec_in_ok;
IP_PUBLIC unsigned long long ipsec_in_skip;
IP_PUBLIC unsigned long long ipsec_in_fail;
IP_PUBLIC unsigned long long ipsec_recrypt_ok;
IP_PUBLIC unsigned long long ipsec_recrypt_fail;
#endif /* IPCOM_USE_FORWARDER_IPSEC */

/*
 ****************************************************************************
 * 9                    STATIC FUNCTIONS
 ****************************************************************************
 */

#ifdef IPCOM_USE_CALCULATE_KEY
/*
 *===========================================================================
 *                    ipcom_forwarder_calculate_pkt_key
 *===========================================================================
 * Context:     any
 * Description: Calculate key based on 'ipcom_pkt'
 * Parameters:
 * Returns:
 *
 */
IP_STATIC __inline__ Ip_u16
ipcom_forwarder_calculate_pkt_key(Ipcom_pkt *pkt)
{
    Ip_u32 k;
    Ip_u16 *p = (Ip_u16 *)(pkt->data + pkt->start + LL_ETH_HDR_SZ);

    k = ((p[6] + p[7]) << 1) + p[8] + p[9];

    return k + (k >> 16);
}

/*
 *===========================================================================
 *                    ipcom_forwarder_calculate_packet_key
 *===========================================================================
 * Context:     any
 * Description: Calculate key based on 'packet'
 * Parameters:
 * Returns:
 *
 */
IP_STATIC __inline__ Ip_u16
ipcom_forwarder_calculate_packet_key(void *packet)
{
    Ip_u32 k;
    Ip_u16 *p = packet;

    k = ((p[7 + 6] + p[7 + 7]) << 1) + p[7 + 8] + p[7 + 9];

    return k + (k >> 16);
}
#endif /* IPCOM_USE_CALCULATE_KEY */


#ifdef IPCOM_USE_FORWARDER_IPSEC
/*
 *===========================================================================
 *                    ipcom_forwarder_ipsec_decrypt_packet
 *===========================================================================
 * Context:     fw
 * Description: Decrypt packet if it is encrypted.
 *              Called only when fw_msg->ipsec_dec is nonzero.
 * Parameters:  'fw_msg' : packet descriptor
 * Returns:     = 0 : continue
 *              < 0 : drop
 *
 */
IP_STATIC __inline__ int
ipcom_forwarder_ipsec_decrypt_packet(fw_msg_t *fw_msg, fw_ipsec_replay_t *fw_ipsec_replay, int offset)
{
    unsigned int headers, trailers;
    int          ret;
    Ip_u32       dstaddr_n[5]; /* Five elements needed to fit IPv6 address and the seed */

    ret = ipcom_forwarder_ipsec_input(&fw_msg->ipsec_dec, fw_ipsec_replay,
                                      &fw_msg->packet[offset],
                                      fw_msg->len - offset,
                                      &headers, &trailers);
    if (ret == 0)
    {
#ifdef FW_IPSEC_DEBUG
        ipcom_printf("FW_IPSEC :: input ok! headers = %d trailers = %d\n", headers, trailers);
#endif
        ipsec_in_ok++;

        if (fw_msg->ipsec_dec == IP_AF_INET)
        {
            /* Change table in case of a v4 in v6 tunnel */
#ifdef IPCOM_USE_FORWARDER_VLAN
            fw_msg->tbl[0] = offset > LL_ETH_HDR_SZ ? IPCOM_FORWARDER_PACKET_TYPE_IPV4_VLAN : IPCOM_FORWARDER_PACKET_TYPE_IPV4;
#else
            fw_msg->tbl[0] = IPCOM_FORWARDER_PACKET_TYPE_IPV4;
#endif
            /* Create a new tag based on the destination address of the decrypted packet and the seed */
            dstaddr_n[0] = *(Ip_u32 *)&fw_msg->packet[offset+headers+16];    /* not alignment safe! */
            dstaddr_n[1] = ipcom_forwarder_get_seed();
            fw_msg->fw_key =
                (ipcom_forwarder_hw_crc16(dstaddr_n, 2) & (ipcom_fw.fw_v4_entries - 1)) <<
                IPCOM_FORWARDER_TRUE_CACHE_WAYS_SHIFT;
        }
        else if (fw_msg->ipsec_dec == IP_AF_INET6)
        {
            /* Change table in case of a v6 in v4 tunnel */
#ifdef IPCOM_USE_FORWARDER_VLAN
            fw_msg->tbl[0] = offset > LL_ETH_HDR_SZ ? IPCOM_FORWARDER_PACKET_TYPE_IPV6_VLAN : IPCOM_FORWARDER_PACKET_TYPE_IPV6;
#else
            fw_msg->tbl[0] = IPCOM_FORWARDER_PACKET_TYPE_IPV6;
#endif
            /* Create a new tag based on the destination address of the decrypted packet and the seed */
            dstaddr_n[0] = *(Ip_u32 *)&fw_msg->packet[offset+headers+24];    /* not alignment safe! */
            dstaddr_n[1] = *(Ip_u32 *)&fw_msg->packet[offset+headers+28];    /* not alignment safe! */
            dstaddr_n[2] = *(Ip_u32 *)&fw_msg->packet[offset+headers+32];    /* not alignment safe! */
            dstaddr_n[3] = *(Ip_u32 *)&fw_msg->packet[offset+headers+36];    /* not alignment safe! */
            dstaddr_n[4] = ipcom_forwarder_get_seed();
            fw_msg->fw_key =
                (ipcom_forwarder_hw_crc16(dstaddr_n, 5) & (ipcom_fw.fw_v6_entries - 1)) <<
                IPCOM_FORWARDER_TRUE_CACHE_WAYS_SHIFT;

        }

        /* Mark packet as decrypted */
        fw_msg->ipsec_dec = 0xff;

        /* Adjust length and offset */
        fw_msg->len -= headers + trailers;
        fw_msg->packet += headers;
    }
    else
    {
        if (ret == 1)
        {
#ifdef FW_IPSEC_DEBUG
            ipcom_printf("FW_IPSEC :: input skipped!\n");
#endif
            ipsec_in_skip++;
        }
        else
        {
#ifdef FW_IPSEC_DEBUG
            if (ret == -2)
                ipcom_printf("FW_IPSEC :: input HMAC validation failed!\n");
            else
                ipcom_printf("FW_IPSEC :: input failed!\n");
#endif
            ipsec_in_fail++;
            return -1;
        }
    }
    return 0;
}


/*
 *===========================================================================
 *                    ipcom_forwarder_ipsec_encrypt_packet
 *===========================================================================
 * Context:     fw
 * Description: Encrypt packet
 * Parameters:  'fw_msg' : packet descriptor
 * Returns:     <= 0: send packet to slowpath
 *               > 0: forward packet
 */
IP_STATIC __inline__ int
ipcom_forwarder_ipsec_encrypt_packet(fw_msg_t *fw_msg, fw_mac_if_t *mac_if, fw_ipsec_t *ipsec, fw_ipsec_replay_t *fw_ipsec_replay, int offset)
{
    unsigned int headers, trailers;
    int          ret;

    if (IP_LIKELY(mac_if->enc[1] == IPCOM_FWD_IPSEC_ENCRYPT))
    {
        ret = ipcom_forwarder_ipsec_output(ipsec,
                                           &fw_msg->packet[offset],
                                           fw_msg->len - offset,
                                           &headers, &trailers);
        if (ret == 0)
        {
#ifdef FW_IPSEC_DEBUG
            ipcom_printf("FW_IPSEC :: output ok! headers = %d trailers = %d\n", headers, trailers);
#endif
            ipsec_out_ok++;
            fw_msg->len += headers + trailers;
            fw_msg->packet -= headers;
            return 1; /* fast forward */
        }
        else
        {
            if (ret == 1)
            {
#ifdef FW_IPSEC_DEBUG
                ipcom_printf("FW_IPSEC :: output skipped!\n");
#endif
                ipsec_out_skip++;
            }
            else
            {
#ifdef FW_IPSEC_DEBUG
                ipcom_printf("FW_IPSEC :: output failed!\n");
#endif
                ipsec_out_fail++;
            }
            return 0; /* slowpath */
        }
    }
    else
    {
        if (mac_if->enc[1] == IPCOM_FWD_IPSEC_DECRYPT)
        {
            if (IP_LIKELY(fw_msg->ipsec_dec == 0xff))
            {
                /* Anti-replay check */
                if (fw_ipsec_replay->replay)
                {
                    fw_lock_key_t lkey;
                    lkey = ipcom_forwarder_atomic_lock(fw_ipsec_replay->key);
                    if (ipcom_forwarder_ipsec_check_replay_window(&ipcom_fw.fw_cache_tbl_dec[fw_ipsec_replay->key],
                                                                  fw_ipsec_replay->seq) < 0)
                    {
                        ipcom_forwarder_atomic_unlock(fw_ipsec_replay->key, lkey);
#ifdef FW_IPSEC_DEBUG
                        ipcom_printf("FW_IPSEC :: input replay check failed!\n");
#endif
                        ipsec_in_ok--;
                        ipsec_in_fail++;
                        fw_msg->tbl[0] = IPCOM_FORWARDER_PACKET_IPSEC_DROP_PKT; /* Drop packet */
                        return 0;
                    }
                    ipcom_forwarder_atomic_unlock(fw_ipsec_replay->key, lkey);
                }
            }
            else
            {
                /*
                 * Do not forward incoming clear text packets that matches
                 * cache entries marked for decryption
                 */
#ifdef FW_IPSEC_DEBUG
                ipcom_printf("FW_IPSEC :: Clear text packet matched cache entry marked for IPsec decryption!\n");
#endif
                return 0; /* slowpath */
            }
        }
        return 1; /* fast forward */
    }
}
#endif /* IPCOM_USE_FORWARDER_IPSEC */


/*
 *===========================================================================
 *                    ipcom_forwarder_get_fw_if
 *===========================================================================
 * Context:     fw
 * Description: Obtain forwarding interface index corresponding to 'msg' from fw cache
 *              In case of a match the packet referred to by 'msg' is updated
 *              with destination hw address and IP header info.
 *              source MAC must be added before packet is sent.
 * Parameters:  'msg' : packet descriptor
 * Returns:     index of interface, or -1 if no fw cache entry matches 'msg'
 *
 */
IP_STATIC __inline__ int ipcom_forwarder_get_fw_if(fw_msg_t *msg)
{
#ifdef IPCOM_USE_INET
    fw_cache_entry_v4_t *e4;
    Ip_u32 dst4;
#endif
#ifdef IPCOM_USE_INET6
    fw_cache_entry_v6_t *e6;
#endif
#ifdef IPCOM_USE_FORWARDER_VLAN
    Ip_u16 vid;
    fw_cache_entry_vlan_t *ev;
#endif

#ifdef IPCOM_USE_FORWARDER_IPSEC
    fw_ipsec_replay_t fw_ipsec_replay;

    if (msg->ipsec_dec)
    {
        int offset = LL_ETH_HDR_SZ;
#ifdef IPCOM_USE_FORWARDER_VLAN
        if (IP_UNLIKELY(msg->tbl[0] == IPCOM_FORWARDER_PACKET_TYPE_IPV4_VLAN ||
                        msg->tbl[0] == IPCOM_FORWARDER_PACKET_TYPE_IPV6_VLAN))
            offset = LL_ETH_HDR_SZ + LL_ETH_VLAN_ADD;
#endif /* IPCOM_USE_FORWARDER_VLAN */

        if (ipcom_forwarder_ipsec_decrypt_packet(msg, &fw_ipsec_replay, offset) < 0)
        {
            msg->tbl[0] = IPCOM_FORWARDER_PACKET_IPSEC_DROP_PKT; /* Drop packet */
            return -1;
        }
    }
#endif /* IPCOM_USE_FORWARDER_IPSEC */

    if (IP_LIKELY(msg->tbl[0] == IPCOM_FORWARDER_PACKET_TYPE_IPV4))
    {
#ifdef IPCOM_USE_INET
        /* IPv4 */
        e4 = &ipcom_fw.fw_cache_tbl_v4[msg->fw_key];
        dst4 = *(Ip_u32 *)(msg->packet + LL_ETH_HDR_SZ + 16);
        if (IP_UNLIKELY(dst4 != e4->ip4.addr) || IP_UNLIKELY(e4->ip4.rci != *ipcom_fw.rt_cache_id))
        {
#if IPCOM_FORWARDER_CACHE_WAYS > 1
            if (IP_UNLIKELY(dst4 != (++e4)->ip4.addr) || IP_UNLIKELY(e4->ip4.rci != *ipcom_fw.rt_cache_id))
#if IPCOM_FORWARDER_CACHE_WAYS > 2
                if (IP_UNLIKELY(dst4 != (++e4)->ip4.addr) || IP_UNLIKELY(e4->ip4.rci != *ipcom_fw.rt_cache_id))
#if IPCOM_FORWARDER_CACHE_WAYS > 3
                    if (IP_UNLIKELY(dst4 != (++e4)->ip4.addr) || IP_UNLIKELY(e4->ip4.rci != *ipcom_fw.rt_cache_id))
#if IPCOM_FORWARDER_CACHE_WAYS > 4
                        if (IP_UNLIKELY(dst4 != (++e4)->ip4.addr) || IP_UNLIKELY(e4->ip4.rci != *ipcom_fw.rt_cache_id))
#if IPCOM_FORWARDER_CACHE_WAYS > 5
                            if (IP_UNLIKELY(dst4 != (++e4)->ip4.addr) || IP_UNLIKELY(e4->ip4.rci != *ipcom_fw.rt_cache_id))
#if IPCOM_FORWARDER_CACHE_WAYS > 6
                                if (IP_UNLIKELY(dst4 != (++e4)->ip4.addr) || IP_UNLIKELY(e4->ip4.rci != *ipcom_fw.rt_cache_id))
#if IPCOM_FORWARDER_CACHE_WAYS > 7
                                    if (IP_UNLIKELY(dst4 != (++e4)->ip4.addr) || IP_UNLIKELY(e4->ip4.rci != *ipcom_fw.rt_cache_id))

#endif
#endif
#endif
#endif
#endif
#endif
#endif
                goto nomatch;
        }

        IP_INCREMENTAL_CHECKSUM(((Ipnet_pkt_ip *)(msg->packet + LL_ETH_HDR_SZ)));
#ifdef IPCOM_USE_FORWARDER_IPSEC
        if (ipcom_forwarder_ipsec_encrypt_packet(msg, &e4->mac_if, &e4->ipsec, &fw_ipsec_replay, LL_ETH_HDR_SZ) <= 0)
        {
            Ipnet_pkt_ip *ip = (Ipnet_pkt_ip *)(msg->packet + LL_ETH_HDR_SZ);

            /* Restore IPv4 header */
            ip->ttl++;
            ip->sum = 0;
            ip->sum = ipcom_in_checksum(ip, IPCOM_IP_HEADER_LENGTH);
            return -1;
        }
#endif /* IPCOM_USE_FORWARDER_IPSEC */
        *(Ip_u32 *)(msg->packet - 2) = e4->mac_if.dst_mac[0];
        *(Ip_u32 *)(msg->packet + 2) = e4->mac_if.dst_mac[1];
        *(Ip_u32 *)(msg->packet + 6) = ipcom_fw.fw_port[e4->mac_if.ix[0]].src_type[0];
        *(Ip_u16 *)(msg->packet + 10) = ipcom_fw.fw_port[e4->mac_if.ix[0]].src_type[1];
#ifdef IPCOM_USE_FORWARDER_IPSEC
        *(Ip_u16 *)(msg->packet + 12) = ip_htons(IP_IPv4);
#endif /* IPCOM_USE_FORWARDER_IPSEC */
        return e4->mac_if.ix[0];
#endif /* IPCOM_USE_INET */
    }
    else if (IP_LIKELY(msg->tbl[0] == IPCOM_FORWARDER_PACKET_TYPE_IPV6))
    {
#ifdef IPCOM_USE_INET6
        /* IPv6 */
        e6 = &ipcom_fw.fw_cache_tbl_v6[msg->fw_key];

        if (IP_UNLIKELY((*(Ip_u64 *)(msg->packet + LL_ETH_HDR_SZ + 24) != e6->ip6.addr[0])) ||
            IP_UNLIKELY((*(Ip_u64 *)(msg->packet + LL_ETH_HDR_SZ + 32) != e6->ip6.addr[1])) ||
            IP_UNLIKELY(e6->ip6.rci != *ipcom_fw.rt_cache_id)
#ifdef IPCOM_USE_FORWARDER_VLAN
            || e6->ip6.type != IPCOM_FORWARDER_PACKET_TYPE_IPV6
#endif
            )
        {
#if IPCOM_FORWARDER_CACHE_WAYS > 1
            if (IP_UNLIKELY((*(Ip_u64 *)(msg->packet + LL_ETH_HDR_SZ + 24) != (++e6)->ip6.addr[0])) ||
                IP_UNLIKELY((*(Ip_u64 *)(msg->packet + LL_ETH_HDR_SZ + 32) != e6->ip6.addr[1])) ||
                IP_UNLIKELY(e6->ip6.rci != *ipcom_fw.rt_cache_id)
#ifdef IPCOM_USE_FORWARDER_VLAN
                || e6->ip6.type != IPCOM_FORWARDER_PACKET_TYPE_IPV6
#endif
                )
#if IPCOM_FORWARDER_CACHE_WAYS > 2
                if (IP_UNLIKELY((*(Ip_u64 *)(msg->packet + LL_ETH_HDR_SZ + 24) != (++e6)->ip6.addr[0])) ||
                    IP_UNLIKELY((*(Ip_u64 *)(msg->packet + LL_ETH_HDR_SZ + 32) != e6->ip6.addr[1])) ||
                    IP_UNLIKELY(e6->ip6.rci != *ipcom_fw.rt_cache_id)
#ifdef IPCOM_USE_FORWARDER_VLAN
                    || e6->ip6.type != IPCOM_FORWARDER_PACKET_TYPE_IPV6
#endif
                    )
#if IPCOM_FORWARDER_CACHE_WAYS > 3
                    if (IP_UNLIKELY((*(Ip_u64 *)(msg->packet + LL_ETH_HDR_SZ + 24) != (++e6)->ip6.addr[0])) ||
                        IP_UNLIKELY((*(Ip_u64 *)(msg->packet + LL_ETH_HDR_SZ + 32) != e6->ip6.addr[1])) ||
                        IP_UNLIKELY(e6->ip6.rci != *ipcom_fw.rt_cache_id)
#ifdef IPCOM_USE_FORWARDER_VLAN
                        || e6->ip6.type != IPCOM_FORWARDER_PACKET_TYPE_IPV6
#endif
                        )
#if IPCOM_FORWARDER_CACHE_WAYS > 4
                        if (IP_UNLIKELY((*(Ip_u64 *)(msg->packet + LL_ETH_HDR_SZ + 24) != (++e6)->ip6.addr[0])) ||
                            IP_UNLIKELY((*(Ip_u64 *)(msg->packet + LL_ETH_HDR_SZ + 32) != e6->ip6.addr[1])) ||
                            IP_UNLIKELY(e6->ip6.rci != *ipcom_fw.rt_cache_id)
#ifdef IPCOM_USE_FORWARDER_VLAN
                            || e6->ip6.type != IPCOM_FORWARDER_PACKET_TYPE_IPV6
#endif
                            )
#if IPCOM_FORWARDER_CACHE_WAYS > 5
                            if (IP_UNLIKELY((*(Ip_u64 *)(msg->packet + LL_ETH_HDR_SZ + 24) != (++e6)->ip6.addr[0])) ||
                                IP_UNLIKELY((*(Ip_u64 *)(msg->packet + LL_ETH_HDR_SZ + 32) != e6->ip6.addr[1])) ||
                                IP_UNLIKELY(e6->ip6.rci != *ipcom_fw.rt_cache_id)
#ifdef IPCOM_USE_FORWARDER_VLAN
                                || e6->ip6.type != IPCOM_FORWARDER_PACKET_TYPE_IPV6
#endif
                                )
#if IPCOM_FORWARDER_CACHE_WAYS > 6
                                if (IP_UNLIKELY((*(Ip_u64 *)(msg->packet + LL_ETH_HDR_SZ + 24) != (++e6)->ip6.addr[0])) ||
                                    IP_UNLIKELY((*(Ip_u64 *)(msg->packet + LL_ETH_HDR_SZ + 32) != e6->ip6.addr[1])) ||
                                    IP_UNLIKELY(e6->ip6.rci != *ipcom_fw.rt_cache_id)
#ifdef IPCOM_USE_FORWARDER_VLAN
                                    || e6->ip6.type != IPCOM_FORWARDER_PACKET_TYPE_IPV6
#endif
                                    )
#if IPCOM_FORWARDER_CACHE_WAYS > 7
                                    if (IP_UNLIKELY((*(Ip_u64 *)(msg->packet + LL_ETH_HDR_SZ + 24) != (++e6)->ip6.addr[0])) ||
                                        IP_UNLIKELY((*(Ip_u64 *)(msg->packet + LL_ETH_HDR_SZ + 32) != e6->ip6.addr[1])) ||
                                        IP_UNLIKELY(e6->ip6.rci != *ipcom_fw.rt_cache_id)
#ifdef IPCOM_USE_FORWARDER_VLAN
                                        || e6->ip6.type != IPCOM_FORWARDER_PACKET_TYPE_IPV6
#endif
                                        )
#endif
#endif
#endif
#endif
#endif
#endif
#endif
                                        goto nomatch;
        }

        SET_V6_HOPL((msg->packet + LL_ETH_HDR_SZ), GET_V6_HOPL((msg->packet + LL_ETH_HDR_SZ)) - 1);
#ifdef IPCOM_USE_FORWARDER_IPSEC
        if (ipcom_forwarder_ipsec_encrypt_packet(msg, &e6->mac_if, &e6->ipsec, &fw_ipsec_replay, LL_ETH_HDR_SZ) <= 0)
        {
            /* Restore IPv6 header */
            SET_V6_HOPL((msg->packet + LL_ETH_HDR_SZ), GET_V6_HOPL((msg->packet + LL_ETH_HDR_SZ)) + 1);
            return -1;
        }
#endif /* IPCOM_USE_FORWARDER_IPSEC */
        *(Ip_u64 *)(msg->packet - 2) = e6->mac_if.dst_mac64;
        *(Ip_u64 *)(msg->packet + 6) = *(Ip_u64 *)&ipcom_fw.fw_port[e6->mac_if.ix[0]].src_type[0];

        return e6->mac_if.ix[0];
#endif /* IPCOM_USE_INET6 */
    }
#ifdef IPCOM_USE_FORWARDER_VLAN
    else if (IP_LIKELY(msg->tbl[0] == IPCOM_FORWARDER_PACKET_TYPE_IPV4_VLAN))
    {
#ifdef IPCOM_USE_INET
        /* IPv6 table is also used for VLAN entries, both IPv4 and IPv6 */
        ev = (fw_cache_entry_vlan_t *)&ipcom_fw.fw_cache_tbl_v6[msg->fw_key];
        dst4 = *(Ip_u32 *)(msg->packet + LL_ETH_HDR_SZ + LL_ETH_VLAN_ADD + 16);
        vid = *(Ip_u16 *)(msg->packet + LL_ETH_HDR_SZ); /* UP/CFI/VID word */

        if (IP_UNLIKELY(ev->ipvlan.type != IPCOM_FORWARDER_PACKET_TYPE_IPV4_VLAN ||
                        dst4 != ev->ipvlan.a.addr_v4 ||
                        vid != ev->ipvlan.vlan_id ||
                        ev->ipvlan.rci != *ipcom_fw.rt_cache_id))
        {
#if IPCOM_FORWARDER_CACHE_WAYS > 1
            if (IP_UNLIKELY((++ev)->ipvlan.type != IPCOM_FORWARDER_PACKET_TYPE_IPV4_VLAN ||
                            dst4 != ev->ipvlan.a.addr_v4 ||
                            vid != ev->ipvlan.vlan_id ||
                            ev->ipvlan.rci != *ipcom_fw.rt_cache_id))
#if IPCOM_FORWARDER_CACHE_WAYS > 2
                if (IP_UNLIKELY((++ev)->ipvlan.type != IPCOM_FORWARDER_PACKET_TYPE_IPV4_VLAN ||
                                dst4 != ev->ipvlan.a.addr_v4 ||
                                vid != ev->ipvlan.vlan_id ||
                                ev->ipvlan.rci != *ipcom_fw.rt_cache_id))
#if IPCOM_FORWARDER_CACHE_WAYS > 3
                    if (IP_UNLIKELY((++ev)->ipvlan.type != IPCOM_FORWARDER_PACKET_TYPE_IPV4_VLAN ||
                                    dst4 != ev->ipvlan.a.addr_v4 ||
                                    vid != ev->ipvlan.vlan_id ||
                                    ev->ipvlan.rci != *ipcom_fw.rt_cache_id))
#if IPCOM_FORWARDER_CACHE_WAYS > 4
                        if (IP_UNLIKELY((++ev)->ipvlan.type != IPCOM_FORWARDER_PACKET_TYPE_IPV4_VLAN ||
                                        dst4 != ev->ipvlan.a.addr_v4 ||
                                        vid != ev->ipvlan.vlan_id ||
                                        ev->ipvlan.rci != *ipcom_fw.rt_cache_id))
#if IPCOM_FORWARDER_CACHE_WAYS > 5
                            if (IP_UNLIKELY((++ev)->ipvlan.type != IPCOM_FORWARDER_PACKET_TYPE_IPV4_VLAN ||
                                            dst4 != ev->ipvlan.a.addr_v4 ||
                                            vid != ev->ipvlan.vlan_id ||
                                            ev->ipvlan.rci != *ipcom_fw.rt_cache_id))
#if IPCOM_FORWARDER_CACHE_WAYS > 6
                                if (IP_UNLIKELY((++ev)->ipvlan.type != IPCOM_FORWARDER_PACKET_TYPE_IPV4_VLAN ||
                                                dst4 != ev->ipvlan.a.addr_v4 ||
                                                vid != ev->ipvlan.vlan_id ||
                                                ev->ipvlan.rci != *ipcom_fw.rt_cache_id))
#if IPCOM_FORWARDER_CACHE_WAYS > 7
                                    if (IP_UNLIKELY((++ev)->ipvlan.type != IPCOM_FORWARDER_PACKET_TYPE_IPV4_VLAN ||
                                                    dst4 != ev->ipvlan.a.addr_v4 ||
                                                    vid != ev->ipvlan.vlan_id ||
                                                    ev->ipvlan.rci != *ipcom_fw.rt_cache_id))
#endif
#endif
#endif
#endif
#endif
#endif
#endif
                                        goto nomatch;
        }

        IP_INCREMENTAL_CHECKSUM(((Ipnet_pkt_ip *)(msg->packet + LL_ETH_HDR_SZ + LL_ETH_VLAN_ADD)));
#ifdef IPCOM_USE_FORWARDER_IPSEC
        if (ipcom_forwarder_ipsec_encrypt_packet(msg, &ev->mac_if, &ev->ipsec, &fw_ipsec_replay, LL_ETH_HDR_SZ + LL_ETH_VLAN_ADD) <= 0)
        {
            Ipnet_pkt_ip *ip = (Ipnet_pkt_ip *)(msg->packet + LL_ETH_HDR_SZ + LL_ETH_VLAN_ADD);

            /* Restore IPv4 header */
            ip->ttl--;
            ip->sum = 0;
            ip->sum = ipcom_in_checksum(ip, IPCOM_IP_HEADER_LENGTH);
            return -1;
        }
#endif /* IPCOM_USE_FORWARDER_IPSEC */
        *(Ip_u32 *)(msg->packet - 2) = ev->mac_if.dst_mac[0];
        *(Ip_u32 *)(msg->packet + 2) = ev->mac_if.dst_mac[1];
        *(Ip_u32 *)(msg->packet + 6) = ipcom_fw.fw_port[ev->mac_if.ix[0]].src_type[0];
        *(Ip_u32 *)(msg->packet + 10) = ipcom_fw.fw_port[ev->mac_if.ix[0]].src_type[1];
        *(Ip_u16 *)(msg->packet + 12) = ip_htons(IP_VLAN);
        *(Ip_u16 *)(msg->packet + 14) = ev->ipvlan.vlan_id;
#ifdef IPCOM_USE_FORWARDER_IPSEC
        *(Ip_u16 *)(msg->packet + 16) = ip_htons(IP_IPv4);
#endif
        return ev->mac_if.ix[0];
#endif /* IPCOM_USE_INET */
    }
    else if (IP_LIKELY(msg->tbl[0] == IPCOM_FORWARDER_PACKET_TYPE_IPV6_VLAN))
    {
#ifdef IPCOM_USE_INET6
        /* IPv6 table is also used for VLAN entries, both IPv4 and IPv6 */
        ev = (fw_cache_entry_vlan_t *)&ipcom_fw.fw_cache_tbl_v6[msg->fw_key];
        vid = *(Ip_u16 *)(msg->packet + LL_ETH_HDR_SZ); /* UP/CFI/VID word */

        if (IP_UNLIKELY(ev->ipvlan.type != IPCOM_FORWARDER_PACKET_TYPE_IPV6_VLAN ||
                        (*(Ip_u64 *)(msg->packet + LL_ETH_HDR_SZ + LL_ETH_VLAN_ADD + 24) != ev->ipvlan.a.addr_v6[0]) ||
                        (*(Ip_u64 *)(msg->packet + LL_ETH_HDR_SZ + LL_ETH_VLAN_ADD + 32) != ev->ipvlan.a.addr_v6[1]) ||
                        vid != ev->ipvlan.vlan_id ||
                        ev->ipvlan.rci != *ipcom_fw.rt_cache_id))
        {
#if IPCOM_FORWARDER_CACHE_WAYS > 1
            if (IP_UNLIKELY((++ev)->ipvlan.type != IPCOM_FORWARDER_PACKET_TYPE_IPV6_VLAN ||
                            (*(Ip_u64 *)(msg->packet + LL_ETH_HDR_SZ + LL_ETH_VLAN_ADD + 24) != ev->ipvlan.a.addr_v6[0]) ||
                            (*(Ip_u64 *)(msg->packet + LL_ETH_HDR_SZ + LL_ETH_VLAN_ADD + 32) != ev->ipvlan.a.addr_v6[1]) ||
                            vid != ev->ipvlan.vlan_id ||
                            ev->ipvlan.rci != *ipcom_fw.rt_cache_id))
#if IPCOM_FORWARDER_CACHE_WAYS > 2
                if (IP_UNLIKELY((++ev)->ipvlan.type != IPCOM_FORWARDER_PACKET_TYPE_IPV6_VLAN ||
                                (*(Ip_u64 *)(msg->packet + LL_ETH_HDR_SZ + LL_ETH_VLAN_ADD + 24) != ev->ipvlan.a.addr_v6[0]) ||
                                (*(Ip_u64 *)(msg->packet + LL_ETH_HDR_SZ + LL_ETH_VLAN_ADD + 32) != ev->ipvlan.a.addr_v6[1]) ||
                                vid != ev->ipvlan.vlan_id ||
                                ev->ipvlan.rci != *ipcom_fw.rt_cache_id))
#if IPCOM_FORWARDER_CACHE_WAYS > 3
                    if (IP_UNLIKELY((++ev)->ipvlan.type != IPCOM_FORWARDER_PACKET_TYPE_IPV6_VLAN ||
                                    (*(Ip_u64 *)(msg->packet + LL_ETH_HDR_SZ + LL_ETH_VLAN_ADD + 24) != ev->ipvlan.a.addr_v6[0]) ||
                                    (*(Ip_u64 *)(msg->packet + LL_ETH_HDR_SZ + LL_ETH_VLAN_ADD + 32) != ev->ipvlan.a.addr_v6[1]) ||
                                    vid != ev->ipvlan.vlan_id ||
                                    ev->ipvlan.rci != *ipcom_fw.rt_cache_id))
#if IPCOM_FORWARDER_CACHE_WAYS > 4
                        if (IP_UNLIKELY((++ev)->ipvlan.type != IPCOM_FORWARDER_PACKET_TYPE_IPV6_VLAN ||
                                        (*(Ip_u64 *)(msg->packet + LL_ETH_HDR_SZ + LL_ETH_VLAN_ADD + 24) != ev->ipvlan.a.addr_v6[0]) ||
                                        (*(Ip_u64 *)(msg->packet + LL_ETH_HDR_SZ + LL_ETH_VLAN_ADD + 32) != ev->ipvlan.a.addr_v6[1]) ||
                                        vid != ev->ipvlan.vlan_id ||
                                        ev->ipvlan.rci != *ipcom_fw.rt_cache_id))
#if IPCOM_FORWARDER_CACHE_WAYS > 5
                            if (IP_UNLIKELY((++ev)->ipvlan.type != IPCOM_FORWARDER_PACKET_TYPE_IPV6_VLAN ||
                                            (*(Ip_u64 *)(msg->packet + LL_ETH_HDR_SZ + LL_ETH_VLAN_ADD + 24) != ev->ipvlan.a.addr_v6[0]) ||
                                            (*(Ip_u64 *)(msg->packet + LL_ETH_HDR_SZ + LL_ETH_VLAN_ADD + 32) != ev->ipvlan.a.addr_v6[1]) ||
                                            vid != ev->ipvlan.vlan_id ||
                                            ev->ipvlan.rci != *ipcom_fw.rt_cache_id))
#if IPCOM_FORWARDER_CACHE_WAYS > 6
                                if (IP_UNLIKELY((++ev)->ipvlan.type != IPCOM_FORWARDER_PACKET_TYPE_IPV6_VLAN ||
                                                (*(Ip_u64 *)(msg->packet + LL_ETH_HDR_SZ + LL_ETH_VLAN_ADD + 24) != ev->ipvlan.a.addr_v6[0]) ||
                                                (*(Ip_u64 *)(msg->packet + LL_ETH_HDR_SZ + LL_ETH_VLAN_ADD + 32) != ev->ipvlan.a.addr_v6[1]) ||
                                                vid != ev->ipvlan.vlan_id ||
                                                ev->ipvlan.rci != *ipcom_fw.rt_cache_id))
#if IPCOM_FORWARDER_CACHE_WAYS > 7
                                    if (IP_UNLIKELY((++ev)->ipvlan.type != IPCOM_FORWARDER_PACKET_TYPE_IPV6_VLAN ||
                                                    (*(Ip_u64 *)(msg->packet + LL_ETH_HDR_SZ + LL_ETH_VLAN_ADD + 24) != ev->ipvlan.a.addr_v6[0]) ||
                                                    (*(Ip_u64 *)(msg->packet + LL_ETH_HDR_SZ + LL_ETH_VLAN_ADD + 32) != ev->ipvlan.a.addr_v6[1]) ||
                                                    vid != ev->ipvlan.vlan_id ||
                                                    ev->ipvlan.rci != *ipcom_fw.rt_cache_id))
#endif
#endif
#endif
#endif
#endif
#endif
#endif
                                        goto nomatch;
        }


        SET_V6_HOPL((msg->packet + LL_ETH_HDR_SZ + LL_ETH_VLAN_ADD),
                    GET_V6_HOPL((msg->packet + LL_ETH_HDR_SZ + LL_ETH_VLAN_ADD)) - 1);
#ifdef IPCOM_USE_FORWARDER_IPSEC
        if (ipcom_forwarder_ipsec_encrypt_packet(msg, &ev->mac_if, &ev->ipsec, &fw_ipsec_replay, LL_ETH_HDR_SZ + LL_ETH_VLAN_ADD) <= 0)
        {
            /* Restore IPv6 header */
            SET_V6_HOPL((msg->packet + LL_ETH_HDR_SZ + LL_ETH_VLAN_ADD), GET_V6_HOPL((msg->packet + LL_ETH_HDR_SZ + LL_ETH_VLAN_ADD)) + 1);
            return -1;
        }
#endif /* IPCOM_USE_FORWARDER_IPSEC */
        *(Ip_u32 *)(msg->packet - 2) = ev->mac_if.dst_mac[0];
        *(Ip_u32 *)(msg->packet + 2) = ev->mac_if.dst_mac[1];
        *(Ip_u32 *)(msg->packet + 6) = ipcom_fw.fw_port[ev->mac_if.ix[0]].src_type[0];
        *(Ip_u32 *)(msg->packet + 10) = ipcom_fw.fw_port[ev->mac_if.ix[0]].src_type[1];
        *(Ip_u16 *)(msg->packet + 12) = ip_htons(IP_VLAN);
        *(Ip_u16 *)(msg->packet + 14) = ev->ipvlan.vlan_id;
#ifdef IPCOM_USE_FORWARDER_IPSEC
        *(Ip_u16 *)(msg->packet + 16) = ip_htons(IP_IPv6);
#endif
        return ev->mac_if.ix[0];
#endif /* IPCOM_USE_INET6 */
    }
#endif /* IPCOM_USE_FORWARDER_VLAN */

nomatch:

#ifdef IPCOM_USE_FORWARDER_IPSEC
    if (msg->ipsec_dec == 0xff)
    {
        int ret, offset = LL_ETH_HDR_SZ;

        if (msg->tbl[0] == IPCOM_FORWARDER_PACKET_TYPE_IPV4_VLAN ||
            msg->tbl[0] == IPCOM_FORWARDER_PACKET_TYPE_IPV6_VLAN)
        {
            offset += LL_ETH_VLAN_ADD;
        }

#ifdef FW_IPSEC_DEBUG
        ipcom_printf("FW_IPSEC :: re-encrypt packet! offset=%d msg->fw_key: %x msg->len=%d(%d) msg->packet=%p(%p)\n",
                     offset, msg->fw_key, msg->len, ipcom_forwarder_ipnet_msg_len(msg),
                     msg->packet, ipcom_forwarder_ipnet_msg_packet(msg));

#endif

        /* Restore buffer pointer and length */
        msg->len    = ipcom_forwarder_ipnet_msg_len(msg);
        msg->packet = ipcom_forwarder_ipnet_msg_packet(msg);

        /* Re-encrypt packet */
        ret = ipcom_forwarder_ipsec_recrypt(&msg->packet[offset], msg->len - offset);
        if (ret < 0)
        {
#ifdef FW_IPSEC_DEBUG
            ipcom_printf("FW_IPSEC :: re-encrypt packet failed!!!\n");

#endif
            ipsec_recrypt_fail++;
            msg->tbl[0] = IPCOM_FORWARDER_PACKET_IPSEC_DROP_PKT; /* Drop packet */
            return -1;
        }
        ipsec_recrypt_ok++;
    }
#endif /* IPCOM_USE_FORWARDER_IPSEC */

    return -1;
}


/*
 ****************************************************************************
 * 10                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

#ifndef IPCOM_FORWARDER_AMP
/*
 *===========================================================================
 *                    ipcom_forwarder
 *===========================================================================
 * Context:         its own
 * Description:     Reads packets from the HW and forwards each if a corresponding
 *                  forward cache entry exists. Otherwise, each packet is passed
 *                  to the management stack.
 * Parameters:      None
 * Returns:         Nothing
 *
 */

IPCOM_PROCESS(ipcom_forwarder)
{
    int fw_id;
    int cpu_i;
    Ip_cpu_set_t cpu_set;
    Ipcom_pipe **control_pipe;
    void * fw_cookie;

    ipcom_proc_init();

    IPCOM_LOG0(DEBUG2, "ipcom_forwarder :: begin");

    /* Wait for start message */
    ipcom_pipe_recv(ipcom_fw.fw_pipe, (void **)&control_pipe);

    /* Calculate id for this forwarder */
    fw_id = (int)(control_pipe - &ipcom_fw.control_pipe[0]) + 1;

    /* Wait for CPUs to become online */
    ipcom_forwarder_startup_delay();

    /* Get a previously reserved CPU */
    IP_CPU_ZERO(&cpu_set);

    cpu_i = ipcom_forwarder_cpuid_get(fw_id);

    /* The CPUs are assumed to have been actually reserved earlier */
    IP_CPU_SET(cpu_i, &cpu_set);
    if (ipcom_proc_cpu_affinity_set(ipcom_getpid(), &cpu_set) != IPCOM_SUCCESS)
    {
        ipcom_printf("Error: failed to set affinity for FW %d to cpu \'%d\'\n", fw_id, cpu_i);
    }

    /* Set up core mask since this forwarder is now locked to a core */
    fw_cookie = ipcom_forwarder_init_rx(fw_id);

    ipcom_forwarder_loop (fw_cookie, control_pipe);

    /* NOT REACHED (yet) */

#ifdef IP_PORT_LKM
    return 0;  /* An IPCOM_PROCESS(...) in linux returns an int. */
#endif
}
#endif /* !defined (IPCOM_FORWARDER_AMP) */

/*
 * TODO: Might not use an Ipcom_pipe in the AMP version...
 */
IP_EXTERN void
ipcom_forwarder_loop(void * fw_cookie, void * pipe)
{
    int ix;
    fw_msg_t *fw_msg;

#ifdef IPCOM_FORWARDER_AMP
    (void)pipe; /* TODO */
#else
    int *sw_msg;
    Ipcom_pipe **control_pipe = pipe;
#endif

    for (;;)
    {
        fw_msg = ipcom_forwarder_rx(fw_cookie);
        /* IPCOM_FWD_WAIT check for potential cache update moved to ipcom_forwarder_rx() */
        if (IP_LIKELY(fw_msg))
        {
            ix = ipcom_forwarder_get_fw_if(fw_msg);
            if (IP_LIKELY(ix >= 0))
            {
                /* Cache entry found => forward packet */
                (void)ipcom_forwarder_tx(ix, fw_msg, fw_cookie);
            }
            else if (IS_DROP_MSG_FW(fw_msg) == IP_TRUE)
            {
#ifdef FW_IPSEC_DEBUG
                ipcom_printf("FW_IPSEC :: dropped!\n");
#endif
                ipcom_forwarder_drop_fw_msg(fw_msg, fw_cookie);
            }
            else if (IS_CONTROL_MSG_FW(fw_msg) == IP_FALSE)
            {
                ipcom_forwarder_slow_path(fw_msg, fw_cookie);
            }
            /* The rest are control messages */
#ifndef IPCOM_FORWARDER_AMP
            else if (FW_MSG(fw_msg) == FW_MSG_TRIGGER_PIPE_READ)
            {
                if (ipcom_pipe_try_recv(*control_pipe, (void **)&sw_msg) == IPCOM_SUCCESS)
                {
                    if (*sw_msg == IPCOM_SIG_STOP_FW)
                    {
                        do
                        {
                            ipcom_pipe_recv(*control_pipe,(void **)&sw_msg);

                        } while(*sw_msg != IPCOM_SIG_START_FW);
                    }
                }
                ipcom_forwarder_free_trigger_msg(fw_msg, fw_cookie);
            }
#else /* IPCOM_FORWARDER_AMP */
            /* TODO */
#endif /* IPCOM_FORWARDER_AMP */
        }
    }
}

#else
int ipcom_forwarder_data_empty_file;
#endif /* IPCOM_USE_FORWARDER */


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
