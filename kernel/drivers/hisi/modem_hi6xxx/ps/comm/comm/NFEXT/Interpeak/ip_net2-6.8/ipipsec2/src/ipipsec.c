/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipipsec.c,v $ $Revision: 1.194.8.1 $
 *   $Source: /home/interpeak/CVSRoot/ipipsec2/src/ipipsec.c,v $
 *   $Author: ulf $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Lennart Bang <lob@interpeak.se>
 ******************************************************************************
 */


/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 * General RFC support:
 * RFC4301 s3.1p8: "A compliant security gateway must support all three of these forms of connectivity.."
 * RFC4301 s3.2p9: "IPsec implementations MUST support ESP and MAY support AH"
 * RFC4301 s4p12: "All implementations of AH or ESP MUST support the concept of an SA as described below."
 * RFC4301 s4.1p15: "a transport mode security protocol header MUST appear before next layer protocols (e.g. TCP, UDP ..)". Note: This is the definition of transport mode.
 * RFC4301 s4.1p16: "A host implementation of IPsec MUST support both transport and tunnel mode."
 * RFC4301 s4.1p16: "A security gateway MUST support tunnel mode.."
 * RFC4301 s4.4p18: "but the external behavior of implementations MUST correspond to the externally observable characteristics of this model.."
 * RFC4301 s5:p50: "As mentioned in Section 4.4.1, "The Security Policy Database (SPD)", the SPD (or associated caches) MUST be consulted during the processing of all traffic that crosses the IPsec protection boundary, including IPsec management traffic."
 * RFC4301 s5.1.2.1p54: "If so, there MUST be an entry in SPD-I database that permits inbound bypassing of the packet, otherwise the packet will be discarded."
 * RFC4301 s5.2p63: "If so, then as with ALL outbound traffic that is to be bypassed, the packet MUST be matched against an SPD-O entry"
 * RFC4301 s6p63: "Disposition of non-error, ICMP messages (that are not addressed to the IPsec implementation itself) MUST be explicitly accounted for using SPD entries."
 * RFC4301 s7.1p67: "All implementations MUST support tunnel mode SAs that are configured to pass traffic without regard to port field"
 *
 * Application RFC support:
 * RFC4301 s4.1p15: "whenever either end of a security association is a security gateway, the SA MUST be tunnel mode.".
 * RFC4301 s4.4.1p21: "if a packet that is looked up in the SPD-I cannot be matched to an entry there, then the packet MUST be discarded."
 * RFC4301 s4.4.1p21: "The SPD MUST permit a user or administrator to specify policy entries as follows: SPD-I, SPD-O, SPD-S"
 * RFC4301 s4.5p47: "All IPsec implementations MUST support both manual and automated SA and cryptographic key management"
 * RFC4301 s7.1p67: "If the SA will carry traffic for specified protocols, the selector set for the SA MUST specify the port fields (or ICMP type/code or Mobility Header type) as ANY"
 * RFC4301 s7.1p67: "If the SA will carry traffic without regard to a specific protocol value (i.e., ANY is specified as the (Next Layer) protocol selector value), then the port field values are undefined and MUST be set to ANY as well"
 */


/*
 ****************************************************************************
 * 2                    CONFIGURATION
 ****************************************************************************
 */

#ifndef IPCOM_KERNEL
#define IPCOM_KERNEL
#endif
#include "ipipsec_config.h"


/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#define IPCOM_USE_CLIB_PROTO
#include <ipcom_type.h>
#include <ipcom_cstyle.h>
#include <ipcom_clib.h>
#include <ipcom_inet.h>
#include <ipcom_err.h>
#include <ipcom_os.h>
#include <ipcom_shell.h>
#include <ipcom_hash.h>
#include <ipcom_sysvar.h>
#include <ipcom_syslog.h>

#include "ipipsec_h.h"
#include "ipipsec_pfkeyv2.h"

#ifdef IPNET
#include <ipnet_signal.h>
#endif


/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

#define IPCOM_SYSLOG_PRIORITY    IPIPSEC_SYSTEM_PRIORITY
#define IPCOM_SYSLOG_FACILITY    IPCOM_LOG_IPIPSEC

#define IPIPSEC_TIME_INFINITE    0xffffffffu

#define IPIPSEC_ERROUT(errval)   do { retval = errval; goto errout; } while(0)

#define IPIPSEC_ROUTE_SET_FAMILY(rtab, af)   ((rtab)->user1 = (Ip_u16) af)
#define IPIPSEC_ROUTE_SET_RTAB(rtab, index)  ((rtab)->user2 = (Ip_u16) index)

#define IP_U64_HIGHER(a,b)   ((IP_U64_GETHI(a) > IP_U64_GETHI(b)) || \
                              ((IP_U64_GETHI(a) == IP_U64_GETHI(b)) && IP_U64_GETLO(a) > IP_U64_GETLO(b)))

#define IPIPSEC_PKT_QUEUE_SIZE     32

#define IPIPSEC_MODVAL 4294967295UL   /* 2^32 -1 */

/*!!FIXME PRODUCTION */
#define IPIPSEC_ON_LOOPBACK


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

#ifdef IPHWCRYPTO
IP_GLOBAL void iphwcrypto_ipsec_esp_release(struct Ipipsec_sa_struct *sa);
#endif

#ifdef IPCOM_USE_FORWARDER_IPSEC
IP_EXTERN void ipcom_forwarder_ipsec_remove_sa(Ipipsec_sa *sa);
IP_EXTERN Ip_u32 ipcom_forwarder_ipsec_get_seq(Ipipsec_sa *sa);
IP_EXTERN void ipcom_forwarder_ipsec_get_replay(Ipipsec_sa *sa);
IP_EXTERN int ipcom_forwarder_ipsec_check_replay(Ipipsec_sa *sa, Ip_u32 seq);
#endif


/*
 ****************************************************************************
 * 7                    LOCAL PROTOTYPES
 ****************************************************************************
 */

IP_PUBLIC Ip_err ipipsec_create(void);
IP_PUBLIC Ip_err ipipsec_start(void) ;
IP_PUBLIC const char *ipipsec_version(void);

IP_STATIC Ip_err ipipsec_init(void);

#ifdef IPNET
IP_PUBLIC void ipnet_route_cache_invalidate(void);
IP_PUBLIC void ipnet_sock_rx_cache_invalidate(int domain);
#endif

IP_STATIC Ip_bool ipipsec_has_expired(Ipipsec_sa_lifetime *current, Ipipsec_sa_lifetime *lt);

#ifdef IPIPSEC_USE_AH
IP_STATIC int ipipsec_ah_digest(Ipipsec_sa *sa, Ipcom_pkt *pkt,
                                Ipipsec_param *param, Ip_u32 seqhi,
                                Ip_u8 *calc_digest);
#endif

#ifdef IPIPSEC_USE_TRAPS
IP_STATIC void ipipsec_send_trap(Ipipsec_sa *sa, Ipipsec_param *param, Ip_u32 traptype, Ip_u32 errors);
#endif

#ifdef IPIPSEC_USE_TFC
IP_STATIC void ipipsec_tfc_timeout(Ipipsec_sa  *sa);
IP_STATIC int ipipsec_tfc_remove_padding(Ipipsec_param *param, Ipcom_pkt *pkt);
#ifdef IPCOM_USE_INET
IP_STATIC void ipipsec_tfc_generate_dummy_inet(Ipipsec_sa *sa);
#endif
#ifdef IPCOM_USE_INET6
IP_STATIC void ipipsec_tfc_generate_dummy_inet6(Ipipsec_sa *sa);
#endif
#endif

#ifdef IPIPSEC_USE_DAEMONS
IP_STATIC IPCOM_PROCESS(ipipsecd);
IP_STATIC void ipipsec_affinity_timeout(void *cookie);
IP_STATIC int ipipsec_packet_order_cmp(Ipcom_pkt *pkt1, Ipcom_pkt *pkt2);
IP_STATIC Ip_bool ipipsec_verify_sa_exists(Ipipsec_sa *sa_verify);
IP_STATIC void ipipsec_send_control_signal(enum Ipipsec_sig_type type, Ipipsec_sa *sa);
IP_STATIC void ipipsec_send_notify_signal(void);
#endif

#if defined(IPIPSEC_USE_DAEMONS) || defined(IPHWCRYPTO)
IP_STATIC void ipipsec_process_pktinput(Ipcom_pkt *pkt);
IP_STATIC void ipipsec_process_pktoutput(Ipcom_pkt *pkt);
#endif

/* SA */
IP_STATIC void ipipsec_sa_ungroup(Ipipsec_sa *sa);
IP_STATIC void ipipsec_sa_timeout(Ipipsec_sa  *sa);
IP_STATIC Ip_u32 ipipsec_sa_next_timeout(Ipipsec_sa *sa);

/* Hash */
IP_STATIC unsigned ipipsec_hash_obj_hash(Ipipsec_hash *hash);
IP_STATIC unsigned ipipsec_hash_key_hash(Ipipsec_key *key);
IP_STATIC Ip_bool ipipsec_hash_key_cmp(Ipipsec_hash *hash, Ipipsec_key *key);
IP_STATIC Ip_bool ipipsec_hash_lookup(Ipipsec_hash_head *hash_head, Ipipsec_key *key, void **entryp);
IP_STATIC void ipipsec_hash_add(Ipipsec_hash_head *hash_head, Ipipsec_key *key, void *entry);

/* Flows */
IP_STATIC void ipipsec_flow_insert(Ipipsec_flow *this);
IP_STATIC int ipipsec_flow_lookup_output(Ipipsec_param *param, Ipcom_pkt *pkt);


/*
 ****************************************************************************
 * 8                    DATA
 ****************************************************************************
 */

IP_STATIC int ipipsec_opened = 0;

/*
 *===========================================================================
 *                         ipipsec
 *===========================================================================
 */
IP_GLOBAL Ipipsec_data  ipipsec;


/*
 ****************************************************************************
 * 9                    DEBUG FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipipsec_sa_name
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL char *
ipipsec_sa_name(Ipipsec_sa *sa)
{
    static char buf[128];
    char str[46], str2[46];

    if (sa == IP_NULL)
        ipcom_strcpy(buf, "none");
    else if (sa == &ipipsec.sa_bypass)
        ipcom_strcpy(buf, "bypass");
    else
    {
        ipcom_sprintf(buf, "%s %s spi=0x%lx src=%s dst=%s",
                      IPSECSATYPENAME(sa->sadb_msg_satype),
                      sa->key.direction == IPIPSEC_DIR_IN ? "in" :
                      sa->key.direction == IPIPSEC_DIR_OUT ? "out" : "any",
                      ip_ntohl(sa->key.ports.espah_spi_n),
                      ipcom_inet_ntop(sa->key.domain, &sa->key.src, str2, sizeof(str2)),
                      ipcom_inet_ntop(sa->key.domain, &sa->key.dst, str, sizeof(str)));
    }

    return (char *)buf;
}


/*
 *===========================================================================
 *                    ipipsec_print_iph
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
#ifdef IPIPSEC_USE_SYSLOG
IP_STATIC void
ipipsec_print_iph(Ipcom_pkt *pkt, Ipipsec_param *param)
{
    (void)pkt;
    (void)param;

    IPCOM_LOG2(DEBUG2, "%s %s",
               param->key.direction == IPIPSEC_DIR_IN ? "-> input " :
               param->key.direction == IPIPSEC_DIR_OUT ? "<- output" : "<-> unknown",
               ipipsec_key_name(pkt, param));
}
#else
#define ipipsec_print_iph(xpkt,xparam)  IP_NOOP
#endif


/*
 *===========================================================================
 *                     ipipsec_sa_to_string
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
#if defined(IPNET) && defined(IP_DEBUG)
IP_STATIC int
ipipsec_sa_to_string(Ipipsec_sa *sa, char *buf, Ip_size_t buf_len)
{
    return ipcom_snprintf(buf, buf_len, "IPSec SA (SA: %s)", ipipsec_sa_name(sa));

}
#endif


/*
 ****************************************************************************
 * 9                    STATIC FUNCTIONS
 ****************************************************************************
 */


/*
 *===========================================================================
 *                    ipipsec_init
 *===========================================================================
 * Description: Initialize the IPSEC module.
 * Parameters:
 * Returns:     IPCOM_SUCCESS = success else IPCOM_ERR_XX.
 *
 */
IP_STATIC Ip_err
ipipsec_init(void)
{
    int i, j;

    ipcom_memset(&ipipsec, 0, sizeof(Ipipsec_data));

#ifdef IPIPSEC_USE_DAEMONS
    {
        Ipcom_pipe_attr pipe_attr;
        Ipcom_proc_attr proc_attr;
        Ip_err          ret;
        char            tname[16];

        if (ipcom_spinlock_create(&ipipsec.spl) != IPCOM_SUCCESS)
        {
            IP_PANIC();
            return IPCOM_ERR_NO_MEMORY;
        }

        /* Get the number of ipsec daemons from configuration */
        ipipsec.numd = ipipsec_conf_num_cpus;
        if (ipipsec.numd == 0)
            ipipsec.numd = 1;

        /* Initialize pipe attributes */
        ipcom_pipe_attr_init(&pipe_attr);
        pipe_attr.msg_limit = IPCOM_PIPE_MSG_LIMIT_MANY;

        /* Create the ipsec control pipe */
        ipipsec.ctrlpipe = ipcom_pipe_new(IPCOM_PIPE_GENERIC, &pipe_attr);
        if (ipipsec.ctrlpipe == IPCOM_PIPE_INVALID)
        {
            IP_PANIC();
            return IPCOM_ERR_FAILED;
        }

        /* Initialize proc attributes */
        ipcom_proc_attr_init(&proc_attr);
        proc_attr.priority  = IPCOM_PRIORITY_DEFAULT;
        proc_attr.stacksize = IPCOM_PROC_STACK_DEFAULT;

        /* Create the ipsec pipes and processes */
        pipe_attr.msg_limit = IPCOM_PIPE_MSG_LIMIT_FEW;
        ipipsec.pipes = ipcom_malloc(ipipsec.numd * sizeof(Ipcom_pipe *));
        ipipsec.pids  = ipcom_malloc(ipipsec.numd * sizeof(Ip_pid_t));
        ipipsec.cpus  = ipcom_malloc(ipipsec.numd * sizeof(int));
        if (ipipsec.pipes == IP_NULL || ipipsec.pids == IP_NULL || ipipsec.cpus == IP_NULL)
        {
            IP_PANIC();
            return IPCOM_ERR_FAILED;
        }
        for (i = 0; i < ipipsec.numd; i++)
        {
            ipipsec.pipes[i] = ipcom_pipe_new(IPCOM_PIPE_GENERIC, &pipe_attr);
            if (ipipsec.pipes[i] == IPCOM_PIPE_INVALID)
            {
                IP_PANIC();
                return IPCOM_ERR_FAILED;
            }

            ipcom_sprintf(tname, "ipsecd%d", i+1);
            ret = ipcom_proc_acreate(tname, (Ipcom_proc_func)ipipsecd, &proc_attr, &ipipsec.pids[i]);
            if (ret != IPCOM_SUCCESS)
            {
                IP_PANIC();
                return ret;
            }

            ipipsec.cpus[i] = -1;
        }

        /* CPUs may not be online yet. Schedule a one second timeout for setting affinity */
        (void)ipnet_timeout_schedule(1000,
                                     ipipsec_affinity_timeout,
                                     IP_NULL,
                                     &ipipsec.atmo);
    }

    /* Create the packet order queue */
    ipipsec.pktq = ipcom_pqueue_new((Ipcom_pqueue_cmp_func)ipipsec_packet_order_cmp,
                                    ipcom_pqueue_nop_store_index);
    if (ipipsec.pktq == IP_NULL)
    {
        IP_PANIC();
        return IPCOM_ERR_NO_MEMORY;
    }
#endif /* #ifdef IPIPSEC_USE_DAEMONS */


    /* PFKeyv2 */
    ipipsec.pfkeyv2_seq = 1;


    /* Crypt init */
    /* [rfc2405:4] - Weak key checks SHOULD be performed */
#if defined(IPIPSEC_USE_DES) || defined(IPIPSEC_USE_3DES)
    des_check_key = 0;     /* OPENSSL_EXTERN int des_check_key */
#endif
    for (i = 0; i < 64; i++)
    {
        ipipsec.o_buffer[i] = HMAC_OPAD;
        ipipsec.i_buffer[i] = HMAC_IPAD;
    }


    /* Init global bypass SA */
    ipcom_list_init(&ipipsec.sa_bypass.flow_head[IPIPSEC_DIR_IN]);
    ipcom_list_init(&ipipsec.sa_bypass.flow_head[IPIPSEC_DIR_OUT]);
    ipipsec.sa_bypass.sadb_msg_satype = SADB_X_SATYPE_BYPASS;
    ipipsec.sa_bypass.sadb_sa_state   = SADB_SASTATE_MATURE;
    ipipsec.sa_bypass.key.direction   = IPIPSEC_DIR_ANY;
    ipipsec.sa_bypass.key.domain      = IPIPSEC_AF_BYPASS;

    /* SA hash */
    ipipsec.sa_hash.hash = ipcom_hash_new((Ipcom_hash_obj_func)ipipsec_hash_obj_hash,
                                          (Ipcom_hash_key_func)ipipsec_hash_key_hash,
                                          (Ipcom_hash_cmp_func)ipipsec_hash_key_cmp);
    if (ipipsec.sa_hash.hash == IP_NULL)
    {
        IP_PANIC();
        return IPCOM_ERR_FAILED;
    }
    ipcom_list_init(&ipipsec.sa_hash.list);
    ipipsec.sa_hash.maxsize = IP_MAX(ipipsec_conf_sa_cache_maxsize, 8);

    /* Flow hash */
    ipipsec.flow_hash.hash = ipcom_hash_new((Ipcom_hash_obj_func)ipipsec_hash_obj_hash,
                                                (Ipcom_hash_key_func)ipipsec_hash_key_hash,
                                                (Ipcom_hash_cmp_func)ipipsec_hash_key_cmp);
    if (ipipsec.flow_hash.hash == IP_NULL)
    {
        IP_PANIC();
        return IPCOM_ERR_FAILED;
    }
    ipcom_list_init(&ipipsec.flow_hash.list);
    ipipsec.flow_hash.maxsize = IP_MAX(ipipsec_conf_flow_cache_maxsize, 32);

    /* Init flow and SA lists */
    for (i = 0; i < 2; i++)
        for (j = 0; j < 2; j++)
        {
            ipcom_list_init(&ipipsec.flow_head[i][j]);
            ipcom_list_init(&ipipsec.sa_head[i][j]);
        }

    ipipsec_opened = 1;

#if defined(IPNET) && defined(IP_DEBUG)
    ipipsec_timeout_to_string((Ipipsec_timeout_handler)ipipsec_sa_timeout,
                              (Ipipsec_timeout_to_string_f)ipipsec_sa_to_string);
#endif

    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    ipipsec_sa_ungroup
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipipsec_sa_ungroup(Ipipsec_sa *sa)
{
    Ipipsec_sa   *head, *prev;

    head = sa->group_head;
    if (head)
    {
        IPCOM_LOG1(DEBUG, "Ungrouping SA: %s", ipipsec_sa_name(sa));

        if (head == sa)
        {
            /* sa is group head, set new head and change all head pointers */
            head = sa->group_next;
            ip_assert(head);
            for (prev = head; prev; prev = prev->group_next)
            {
                ip_assert(prev->group_head == sa);
                prev->group_head = head;
            }
        }
        else
        {
            /* sa is a member in a group, find prev in front of sa */
            for (prev = head; prev; prev = prev->group_next)
                if (prev->group_next == sa)
                    break;
            ip_assert(prev);
            ip_assert(prev->group_next == sa);
            prev->group_next = sa->group_next;
        }
        sa->group_next = IP_NULL;
        sa->group_head = IP_NULL;
        if (head->group_next == IP_NULL)
            head->group_head = IP_NULL;
    }
}


/*
 *===========================================================================
 *                    ipipsec_sa_timeout
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipipsec_sa_timeout(Ipipsec_sa *sa)
{
    Ip_u32 now = ipipsec_seconds_now();

#ifdef IPIPSEC_USE_DAEMONS
    ipipsec_poll_control_pipe();
    /* Check if SA was removed by a hard expire in the control pipe */
    if (ipipsec_verify_sa_exists(sa) == IP_FALSE)
        return;
#endif

    /* Check larval timeout */
    if (sa->larval_timeout && IPCOM_IS_GEQ(now, sa->larval_timeout))
    {
        IPCOM_LOG1(INFO, "<sa_timeout> :: larval expiration  (SA: %s)", ipipsec_sa_name(sa));
        ipipsec_sa_free(sa, IP_FALSE, IP_TRUE);
        return;
    }

    /* Check hard time expiration */
    if (ipipsec_has_expired(&sa->ltcurrent, &sa->lthard))
    {
        IPCOM_LOG1(INFO, "<sa_timeout> :: hard time expiration  (SA: %s)", ipipsec_sa_name(sa));

        /* remove this SA */
        ipipsec_pfkeyv2_expire(sa, IPIPSEC_EXPIRE_TYPE_HARD);
        ipipsec_sa_free(sa, IP_FALSE, IP_TRUE);
        return;
    }

    /* Check soft time expiration */
    if (ipipsec_has_expired(&sa->ltcurrent, &sa->ltsoft))
    {
        IPCOM_LOG1(INFO, "<sa_timeout> :: soft time expiration  (SA: %s)", ipipsec_sa_name(sa));

        /* notice subscribers */
        ipipsec_pfkeyv2_expire(sa, IPIPSEC_EXPIRE_TYPE_SOFT);
    }
}


/*
 *===========================================================================
 *                    ipipsec_sa_next_timeout
 *===========================================================================
 * Description: Returns the absolute time, in seconds, when the first timeout
 *              happens on this SA.
 * Parameters:  sa - SA
 * Returns:     Minimum expiration time.
 *
 */
IP_STATIC Ip_u32
ipipsec_sa_next_timeout(Ipipsec_sa *sa)
{
    Ip_u32   add_t, use_t;
    Ip_u32   soft_expire_t, hard_expire_t;
    Ip_u32   now;

    now = ipipsec_seconds_now();

    if (sa->larval_timeout)
        return (Ip_u32)sa->larval_timeout - now;

    /* Soft expiration */
    if (sa->ltsoft.addtime)
        add_t = (sa->ltcurrent.addtime - now) + sa->ltsoft.addtime;
    else
        add_t = IPIPSEC_TIME_INFINITE;
    if (sa->ltsoft.usetime && sa->ltcurrent.usetime)
        use_t = (sa->ltcurrent.usetime - now) + sa->ltsoft.usetime;
    else
        use_t = IPIPSEC_TIME_INFINITE;
    soft_expire_t = IP_MIN(add_t, use_t);

    /* Hard expiration */
    if (sa->lthard.addtime)
        add_t = (sa->ltcurrent.addtime - now) + sa->lthard.addtime;
    else
        add_t = IPIPSEC_TIME_INFINITE;
    if (sa->lthard.usetime && sa->ltcurrent.usetime)
        use_t = (sa->ltcurrent.usetime - now) + sa->lthard.usetime;
    else
        use_t = IPIPSEC_TIME_INFINITE;
    hard_expire_t = IP_MIN(add_t, use_t);

    return IP_MIN(soft_expire_t, hard_expire_t);
}


/*
 *===========================================================================
 *                    ipipsec_has_expired
 *===========================================================================
 * Description: Checks if addtime or usetime has expired in any sa.
 * Parameters:  now - The current time.
 *              current - The current lifetime structure for the sa.
 *              lt - The lifetime structure that should be checked for
 *              expiration (hard or soft).
 * Returns:
 *
 */
IP_STATIC Ip_bool
ipipsec_has_expired(Ipipsec_sa_lifetime *current, Ipipsec_sa_lifetime *lt)
{
    Ip_u32 now = ipipsec_seconds_now();

    if (lt->addtime)
    {
        ip_assert(IPCOM_IS_GEQ(now, current->addtime));
        if ((now - current->addtime) >= lt->addtime)
            return IP_TRUE;
    }

    if (lt->usetime && current->usetime)
    {
        ip_assert(IPCOM_IS_GEQ(now, current->usetime));
        if ((now - current->usetime) >= lt->usetime)
            return IP_TRUE;
    }

    return IP_FALSE;
}


/*
 *===========================================================================
 *                    ipipsec_key_update
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL int
ipipsec_key_update(Ipcom_pkt *pkt, Ipipsec_param *param)
{
    int     pkt_start;
    Ip_u8   this_hdr;

    ip_assert(pkt != IP_NULL);
    ip_assert(param->key.domain == IP_AF_INET || param->key.domain == IP_AF_INET6);
    ip_assert(param->key.direction == IPIPSEC_DIR_IN
              || param->key.direction == IPIPSEC_DIR_OUT);

    param->key.priority = 0;
    param->key.dscp = param->tos_class & 0xe0; /* IP precedence P2,P1,P0 */

    pkt_start = pkt->ipstart;

    switch(param->key.domain)
    {
#ifdef IPCOM_USE_INET
    case IP_AF_INET:
        {
            Ipcom_ip_header    *ip_hdr;

            /* Set scope ID (not used for IPv4) */
            param->key.srcscope_n = 0;
            param->key.dstscope_n = 0;

            /* Increment pkt_start to point at transport header */
            ip_hdr  = (Ipcom_ip_header *)&pkt->data[pkt_start];
            ip_assert((ip_hdr->vers_len & 0xf0) == 0x40);

            param->iph_length = (int)((ip_hdr->vers_len & 0x0f) << 2);
            pkt_start += param->iph_length;

            /* Save TOS (not key value) */
            this_hdr = ip_hdr->protocol;
            param->tos_class  = ip_hdr->tos;
        }
        goto done;
#endif

#ifdef IPCOM_USE_INET6
    case IP_AF_INET6:
        {
            Ipcom_ip6_header    *ip6_hdr;
            Ip_pkt_ip6_ext_hdr  *ext_hdr;
            int                  payload_len;

            /* Set scope ID */
            if (IPCOM_IN6_IS_ADDR_SCOPED(&param->key.src.in6))
                param->key.srcscope_n = ip_htonl(pkt->ifindex);
            else
                param->key.srcscope_n = 0;
            if (IPCOM_IN6_IS_ADDR_SCOPED(&param->key.dst.in6))
                param->key.dstscope_n = ip_htonl(pkt->ifindex);
            else
                param->key.dstscope_n = 0;

            /* Get transport protocol (by skipping extensions) */
            ip6_hdr = (Ipcom_ip6_header *)&pkt->data[pkt_start];
            ip_assert(IP_IP6_GET_VER(ip6_hdr) == 6);
            this_hdr    = ip6_hdr->nxt;
            payload_len = ip_ntohs(ip6_hdr->plen);
            pkt_start  += IPCOM_IP6_HEADER_LENGTH;
            param->iph_length = IPCOM_IP6_HEADER_LENGTH;

            /* Save TOS (not key value) */
            param->tos_class  = (Ip_u8)((IP_GET_NTOHL(&ip6_hdr->ver_class_flow) >> 20) & 0xff);

            for (;;)
            {
                unsigned ext_hdr_len;

                if (pkt_start > pkt->end || pkt->end - pkt_start > payload_len)
                {
                    ip_assert2();
                    return -1;
                }

                ext_hdr     = (Ip_pkt_ip6_ext_hdr *)&pkt->data[pkt_start];
                ext_hdr_len = (ext_hdr->len + 1) << 3;

                switch (this_hdr)
                {
                case IP_IPPROTO_HOPOPTS:  /*   0 */
                case IP_IPPROTO_DSTOPTS:  /*  60 */
                case IP_IPPROTO_ROUTING:  /*  43 */
                    /* Verify that the options are within the boundary of the IP datagram */
                    if (pkt->end - pkt_start < (int)ext_hdr_len)
                    {
                        ip_assert2();
                        return -1;
                    }
                    break;

                default:
                    /* Transport layer protocol -> done */
                    goto done;
                }

                /* Step to next extension header */
                this_hdr     = ext_hdr->nxt;
                payload_len -= ext_hdr_len;
                pkt_start   += ext_hdr_len;
                param->iph_length += ext_hdr_len;
            } /* for(;;) */
        }
        /* unreachable: break; */
#endif /* IPCOM_USE_INET6 */

    default:
        IP_PANIC();
        return -1;
    }


    /* Update ports in param */
 done:
    switch (this_hdr)
    {
    case IP_IPPROTO_TCP:
    case IP_IPPROTO_UDP:
        param->key.ports.espah_spi_n = 0;
        if (param->key.domain == IP_AF_INET
            && IP_BIT_ISTRUE(pkt->flags, IPCOM_PKT_FLAG_FRAGMENT))
        {
            /* Note: No fragments (including the first) use the ports in a lookup.
             *       We set both borts to 0 to make them match 'any' ports only!
             * RFC4301 s7.3p68: "a receiver MUST discard non-initial fragments that arrive on an SA with non-trivial port (or ICMP type/code or MH type) selector values unless Stafeful Fragment Checking has been negotiated". Note: which is "MAY" requirement only. */
            param->key.ports.espah_spi_n = 0;
            break;
        }
        param->key.ports.udptcp_srcdst_n[0] = *(Ip_u16 *)&pkt->data[pkt_start];
        param->key.ports.udptcp_srcdst_n[1] = *(Ip_u16 *)&pkt->data[pkt_start + 2];
        break;
    case IP_IPPROTO_ESP:
        param->key.ports.espah_spi_n = IP_GET_32ON16(&pkt->data[pkt_start]);
        break;
    case IP_IPPROTO_AH:
        param->key.ports.espah_spi_n = IP_GET_32ON16(&pkt->data[pkt_start + 4]);
        break;
    case IP_IPPROTO_ICMP:
    case IP_IPPROTO_ICMPV6:
        param->key.ports.icmp_typecode[0] = pkt->data[pkt_start];      /* icmp type */
        param->key.ports.icmp_typecode[1] = pkt->data[pkt_start + 1];  /* icmp code */
        param->key.ports.icmp_typecode[2] = 0;
        param->key.ports.icmp_typecode[3] = 0;
        break;
    case IP_IPPROTO_MH:
        param->key.ports.mh_type[0] = pkt->data[pkt_start + 2];  /* mh type */
        break;
    default:
        break;
    }

    return (int)this_hdr;
}


/*
 *===========================================================================
 *                    ipipsec_hash_key_hash
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC unsigned
ipipsec_hash_key_hash(Ipipsec_key *key)
{
    return ipcom_hash_update(key, sizeof(Ipipsec_key), 0);
}


/*
 *===========================================================================
 *                    ipipsec_hash_obj_hash
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC unsigned
ipipsec_hash_obj_hash(Ipipsec_hash *hash)
{
    ip_assert(hash != IP_NULL);

    ip_assert(hash->key.domain == IP_AF_INET || hash->key.domain == IP_AF_INET6);
    ip_assert(hash->key.direction == IPIPSEC_DIR_IN
              || hash->key.direction == IPIPSEC_DIR_OUT);

    return ipipsec_hash_key_hash(&hash->key);
}


/*
 *===========================================================================
 *                         ipipsec_hash_key_cmp
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_bool
ipipsec_hash_key_cmp(Ipipsec_hash *hash, Ipipsec_key *key)
{
    if (ipcom_memcmp(&hash->key, key, sizeof(Ipipsec_key)))
        return IP_FALSE;

    return IP_TRUE;
}


/*
 *===========================================================================
 *                    ipipsec_hash_lookup
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_bool
ipipsec_hash_lookup(Ipipsec_hash_head *hash_head, Ipipsec_key *key, void **entryp)
{
    Ipipsec_hash  *hash;

    key->policy_id = ipipsec.policy_id;
    key->priority  = 0;

    if (key->domain == IP_AF_INET)
    {
#ifdef IPCOM_USE_INET6
        key->src.in6.in6.addr32[1] = 0;
        key->src.in6.in6.addr32[2] = 0;
        key->src.in6.in6.addr32[3] = 0;
        key->dst.in6.in6.addr32[1] = 0;
        key->dst.in6.in6.addr32[2] = 0;
        key->dst.in6.in6.addr32[3] = 0;
#endif /* IPCOM_USE_INET6 */
        key->srcscope_n = 0;
        key->dstscope_n = 0;
    }

    IPIPSEC_DATA_LOCK(ipipsec.spl);

    hash = ipcom_hash_get(hash_head->hash, key);
    if (hash != IP_NULL)
    {
        *entryp = hash->entry;
        if (hash_head->list.next != &hash->hash_list)
        {
            /* Put entry first in list to refresh cache */
            ipcom_list_remove(&hash->hash_list);
            ipcom_list_insert_first(&hash_head->list, &hash->hash_list);
        }
    }

    IPIPSEC_DATA_UNLOCK(ipipsec.spl);

    if (hash != IP_NULL)
        return IP_TRUE;
    else
        return IP_FALSE;
}


/*
 *===========================================================================
 *                    ipipsec_hash_add
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipipsec_hash_add(Ipipsec_hash_head *hash_head, Ipipsec_key *key, void *entry)
{
    Ip_err  retval;
    Ipipsec_hash  *hash;

    hash = (Ipipsec_hash *)ipcom_malloc(sizeof(Ipipsec_hash));
    if (hash == IP_NULL)
        return;

    ipcom_memcpy(&hash->key, key, sizeof(Ipipsec_key));
    hash->key.policy_id = ipipsec.policy_id;
    hash->entry = entry;

    IPIPSEC_DATA_LOCK(ipipsec.spl);

    /* Note that entry may already be in list but not found because policy_id changed */
    if (hash_head->size >= hash_head->maxsize)
    {
        /* Hash table full -> remove oldest entry */
        Ipcom_list  *list_entry;

        list_entry = ipcom_list_last(&hash_head->list);
        ip_assert(list_entry != IP_NULL);

        if (list_entry != IP_NULL)
        {
            Ipipsec_hash  *old_hash;

            /* Remove from list */
            ipcom_list_remove(list_entry);

            /* Remove from hash */
            old_hash = IPIPSEC_LIST_ENTRY(list_entry, Ipipsec_hash, hash_list);
            retval = ipcom_hash_remove(hash_head->hash, old_hash);
            ip_assert(retval == IPCOM_SUCCESS);

            /* Release memory */
            ipcom_free(old_hash);

            /* Subtract hash size */
            ip_assert(hash_head->size > 0);
            hash_head->size--;

#ifdef IPIPSEC_USE_STATS
            if (hash_head == &ipipsec.flow_hash)
                ipipsec.stats.flow_hash_full++;
            else
                ipipsec.stats.sa_hash_full++;
#endif
        }
    }

    /* Add new entry to hash cache */
    retval = ipcom_hash_add(hash_head->hash, hash);
    if (retval == IPCOM_SUCCESS)
    {
        /* Add latest hash entry first in hash timeout list */
        hash_head->size++;
        ipcom_list_insert_first(&hash_head->list, &hash->hash_list);
    }
    else
    {
        IP_PANIC();
    }

    IPIPSEC_DATA_UNLOCK(ipipsec.spl);
}


/*
 *===========================================================================
 *                    ipipsec_flow_lookup_output
 *===========================================================================
 * Description:   Lookup output flow
 * Parameters:
 * Returns:  1   = Apply security
 *           0   = Send in clear text
 *         < 0   = Drop packet (negative errno)
 */
IP_STATIC int
ipipsec_flow_lookup_output(Ipipsec_param *param, Ipcom_pkt *pkt)
{
    Ipcom_socket     *sock = IP_NULL;
    Ipipsec_flow     *flow;
    Ip_bool          cache_flow;

    ip_assert(IP_BIT_ISSET(pkt->flags, IPCOM_PKT_FLAG_HAS_IP_HDR));
    ip_assert(pkt->start == pkt->ipstart);

#ifndef IPIPSEC_ON_LOOPBACK
    if (pkt->ifindex == 1)
        /* Always bypass packets sent on standard loopback interface */
        return 0;
#endif

    if (ipipsec_opened == 0)
    {
        if (ipipsec_conf_enable_outfilter == 1)
        {
            IPCOM_LOG0(DEBUG, "<output discard> :: packet dropped due to IPSec not initialized.");
            return -IP_ERRNO_EINVAL;  /* silent drop */
        }
        return 0;
    }


    /***** Prepare param *****/
    param->output_flow   = IP_NULL;
    param->key.direction = IPIPSEC_DIR_OUT;
    param->iph = (Ipipsec_ip_header *)&pkt->data[pkt->ipstart];

    /* Only first 'level' of output -- recurs_level will be increased if output is looped back */
    cache_flow = (pkt->recurs_level == 0);

    /***** Get flow from socket cache *****/
    if (pkt->next_original == IP_NULL && cache_flow == IP_TRUE)
    {
        void   *ioret;

        /* Note:
         * We do not use the cache if packet already has been IPSec'd
         * because in v4inv6 or v6inv4 ipsec tunneling the ipipsec_output()
         * may be called twice resulting in using the socket cache
         * erronously the second time (applying the 1st flow the 2nd time)
         *
         * Note 2:
         * We do not use the cache if the packet's been tunneled, as indicated
         * by the tunnel_ipstart; If it's set to -1 (ffffffff), it's been
         * looped through a tunnel device
         */
        ioret = ipcom_ipsec_ioevent(IPIPSEC_EIOXFDTOSOCK, &pkt->fd);
        sock  = (Ipcom_socket *)ioret;
        if (sock != IP_NULL)
        {
            IPSEC_STATS(ipipsec.stats.output_socket_have++);
            if (sock->ipsec_flow_cache == IPIPSEC_BYPASS_FLOW)
            {
                /* Socket has IP_SO_X_BYPASS_FLOW option set -> bypass */
                IPSEC_STATS(ipipsec.stats.output_socket_flow_cache_match++);
                return 0;
            }
            if (sock->ipsec_policy_id == ipipsec.policy_id)
                param->output_flow = sock->ipsec_flow_cache;
        }
        else
        {
            IPSEC_STATS(ipipsec.stats.output_socket_none++);
        }
    }

    /***** Update param and key *****/
    if (ipipsec_key_update(pkt, param) == -1)
        return -IP_ERRNO_EINVAL;  /* silent drop */


    /***** Lookup output flow  *****/
    if (param->output_flow == IP_NULL)
    {
        param->output_flow = ipipsec_flow_lookup(&param->key, IP_NULL, IPIPSEC_LFLAGS_USEHASH);

        /* Cache output flow in socket */
        if (sock != IP_NULL && cache_flow == IP_TRUE)
        {
            IPSEC_STATS(ipipsec.stats.output_socket_cache_flow++);
            sock->ipsec_flow_cache = param->output_flow;
            sock->ipsec_policy_id  = ipipsec.policy_id;
        }
    }
    else
    {
        IPSEC_STATS(ipipsec.stats.output_socket_flow_cache_match++);
    }


    /***** No output flow match *****/
    if (param->output_flow == IP_NULL)
    {
        IPSEC_STATS(ipipsec.stats.sa_outflow_miss++);

        /* Output filter enabled -> discard packet */
        if (ipipsec_conf_enable_outfilter == 1)
        {
            IPSEC_STATS(ipipsec.stats.sa_outflow_drop++);
            IPCOM_LOG1(NOTICE, "<output discard> :: packet (pkt '%s') dropped due to missing output policy",
                       ipipsec_key_name(pkt, param));
            return -IP_ERRNO_EHOSTUNREACH;
        }

        /* Ok to send packet in cleartext */
        return 0;
    }

    flow = param->output_flow;
    ip_assert(flow->magic == IPIPSEC_FLOW_MAGIC);

    /***** Output flow match *****/
    ipipsec_print_iph(pkt, param);
    IPSEC_STATS(ipipsec.stats.sa_outflow_match++);

    IPCOM_LOG1(DEBUG2, "          Flow: %s",
                  ipipsec_flow_name(flow, flow->sa));

    switch (flow->flowtype)
    {
        /* Bypass flow -> let packet pass in cleartext */
    case SADB_X_FLOW_TYPE_BYPASS:
        ip_assert(flow->sa == &ipipsec.sa_bypass);
        IPSEC_STATS(ipipsec.stats.sa_outflow_bypass++);
        return 0;

        /* Deny flow -> drop packet with permission error */
    case SADB_X_FLOW_TYPE_DENY:
        ip_assert(flow->sa == IP_NULL);
        IPSEC_STATS(ipipsec.stats.sa_outflow_deny++);
        return -IP_ERRNO_EPERM;

        /* Send acquire message and drop packet silently */
    case SADB_X_FLOW_TYPE_REQUIRE:
        ip_assert(flow->sa == IP_NULL);
        IPSEC_STATS(ipipsec.stats.sa_outflow_require++);

        if (ipipsec_key_update(pkt, param) != -1)
            ipipsec_pfkeyv2_acquire(&param->key);
        IPCOM_LOG0(DEBUG2, "<output discard> :: packet dropped due to no SA");
        return -IP_ERRNO_EINVAL;  /* silent drop */

        /* Send acquire message and let packet pass */
    case SADB_X_FLOW_TYPE_ACQUIRE:
        ip_assert(flow->sa == IP_NULL);
        IPSEC_STATS(ipipsec.stats.sa_outflow_acquire++);

        if (ipipsec_key_update(pkt, param) != -1)
            ipipsec_pfkeyv2_acquire(&param->key);
        return 0;

        /* Apply security (standard flow) */
    case SADB_X_FLOW_TYPE_USE:
#ifndef IPNET
        if (param->key.proto == IP_IPPROTO_UDP)
        {
            /* */
            if (pkt->end - pkt->start == 9
                && pkt->data[pkt->start + 8] == 0xff)
                /* NAT-keepalive Packet */
                return 0;

                if (IP_GET_32ON16(&pkt->data[pkt->start + 8]) == 0
                    && ipcom_sysvar_get_as_int("ipike.port", 0)
                    == IP_GET_NTOHS(&pkt->data[pkt->start]))
                /* IKE packet */
                return 0;
        }
#endif /* IPNET */
        break;

    default:
        IP_PANIC();
        break;
    }

    return 1;  /* USE -> Apply security */
}


/*
 *===========================================================================
 *                    ipipsec_flow_lookup_input
 *===========================================================================
 * Description:   Check if input SA has a policy that matches the incoming
 *                packet.
 * Parameters:
 * Returns:       0 if packet is accepted, -1 if it should be discarded.
 *
 */
IP_GLOBAL int
ipipsec_flow_lookup_input(Ipipsec_sa *sa, Ipcom_pkt *pkt, Ipipsec_param *param)
{
    Ipipsec_flow     *flow;

    ip_assert(sa != IP_NULL);
    ip_assert(sa->key.direction == IPIPSEC_DIR_IN);
    ip_assert(pkt != IP_NULL);
    ip_assert(param != IP_NULL);
    ip_assert(param->key.direction == IPIPSEC_DIR_IN);

    /* No input access filter policy checks performed */
    if (ipipsec_conf_enable_infilter != 1)
        return 0;

    if (IPCOM_LIST_FIRST(&sa->flow_head[IPIPSEC_DIR_IN]) == IP_NULL)
    {
        /* No policies on SA -> drop packet */
        IPSEC_STATS(ipipsec.stats.sa_infilter_drop++);
        IPCOM_LOG2(NOTICE, "<input discard> :: pkt '%s' dropped due to empty policy list  (SA: %s)",
                   ipipsec_key_name(pkt, param),
                   ipipsec_sa_name(sa));
        return -1;
    }

    /* Lookup input flow on SA */
    if (ipipsec_key_update(pkt, param) == -1)
        return -1;
    flow = ipipsec_flow_lookup(&param->key, sa, IPIPSEC_LFLAGS_USEHASH);

    /* Match -> deny or accept packet */
    if (flow != IP_NULL)
    {
        if (flow->flowtype == SADB_X_FLOW_TYPE_DENY)
        {
            IPCOM_LOG2(NOTICE, "<input discard> :: packet '%s' dropped due to policy (Flow: %s)",
                       ipipsec_key_name(pkt, param),
                       ipipsec_flow_name(flow, sa));
            return -1;
        }
        IPSEC_STATS(ipipsec.stats.sa_infilter_match++);
        IPCOM_LOG1(DEBUG2, "          Flow: %s",
                   ipipsec_flow_name(flow, flow->sa));
        return 0;
    }

    /* No ingress flow matched -> policy error */
    IPSEC_STATS(ipipsec.stats.sa_infilter_miss++);
    IPCOM_LOG2(NOTICE, "<input discard> :: packet '%s' dropped due to policy (SA: %s)",
               ipipsec_key_name(pkt, param),
               ipipsec_sa_name(sa));
    return -1;
}


/*
 *===========================================================================
 *                    ipipsec_flow_insert
 *===========================================================================
 * Description:   Insert flows in a sorted list where the order of importance
 * is as follows: priority, IP protocol, destination port, source port,
 *                destination IP address, source IP address
 *                RFC4301 s4.4.1p20: "a user or administrator MUST be able to order the entries to express a desired access control policy"
 *                RFC4301 s4.4.1p24: "MUST support (total) ordering of these entries (SPD)"
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipipsec_flow_insert(Ipipsec_flow *this)
{
    Ipcom_list    *head = IP_NULL;
    Ipipsec_flow  *flow;

    ip_assert(this->flowsel.key.direction < 2);

    switch(this->flowsel.key.domain)
    {
#ifdef IPCOM_USE_INET
    case IP_AF_INET:
        head = &ipipsec.flow_head[IPIPSEC_INET][this->flowsel.key.direction];
        break;
#endif
#ifdef IPCOM_USE_INET6
    case IP_AF_INET6:
        head = &ipipsec.flow_head[IPIPSEC_INET6][this->flowsel.key.direction];
        break;
#endif
    default:
        IP_PANIC();
        return;
    }

    for(flow = IPCOM_LIST_FIRST(head);
        flow != IP_NULL;
        flow = IPCOM_LIST_NEXT(&flow->flow_list))
    {
        /* Current entry has higher priority than new -> insert after this entry */
        if (flow->flowsel.key.priority < this->flowsel.key.priority)
            continue;

        /* Current entry has lower priority than new -> insert before this entry */
        if (flow->flowsel.key.priority > this->flowsel.key.priority)
            break;


        /* Current entry has DSCP and new not -> insert after this entry */
        if (flow->flowsel.key.dscp != 0
            && this->flowsel.key.dscp == 0)
            continue;

        /* New entry has DSCP and current not -> insert before this entry */
        if (this->flowsel.key.dscp != 0
            && flow->flowsel.key.dscp == 0)
            break;


        /* Current entry has protocol and new not -> insert after this entry */
        if (flow->flowsel.key.proto != 0
            && this->flowsel.key.proto == 0)
            continue;

        /* New entry has protocol and current not -> insert before this entry */
        if (this->flowsel.key.proto != 0
            && flow->flowsel.key.proto == 0)
            break;


        /* Both entries are of same protocol -> check ports */
        if (this->flowsel.key.proto == flow->flowsel.key.proto)
        {
            switch (this->flowsel.key.proto)
            {
            case IP_IPPROTO_TCP:
            case IP_IPPROTO_UDP:
                /* Current entry has dst_port and new not -> insert after this entry */
                if (flow->flowsel.key.ports.udptcp_srcdst_n[1] != 0
                    && this->flowsel.key.ports.udptcp_srcdst_n[1] == 0)
                    goto continue_loop;

                /* New entry has dst_port and current not -> insert before this entry */
                if (this->flowsel.key.ports.udptcp_srcdst_n[1] != 0
                    && flow->flowsel.key.ports.udptcp_srcdst_n[1] == 0)
                    goto break_loop;

                /* Current entry is exact dst_port and new a range -> insert after this entry */
                if (ip_ntohs(flow->flowsel.key.ports.udptcp_srcdst_n[1]) == flow->flowsel.dst_max_port
                    && ip_ntohs(this->flowsel.key.ports.udptcp_srcdst_n[1]) != this->flowsel.dst_max_port)
                    goto continue_loop;

                /* New entry is exact dst_port and current a range -> insert before this entry */
                if (ip_ntohs(this->flowsel.key.ports.udptcp_srcdst_n[1]) == this->flowsel.dst_max_port
                    && ip_ntohs(flow->flowsel.key.ports.udptcp_srcdst_n[1]) != flow->flowsel.dst_max_port)
                    goto break_loop;


                /* Current entry has src_port and new not -> insert after this entry */
                if (flow->flowsel.key.ports.udptcp_srcdst_n[0] != 0
                    && this->flowsel.key.ports.udptcp_srcdst_n[0] == 0)
                    goto continue_loop;

                /* New entry has src_port and current not -> insert before this entry */
                if (this->flowsel.key.ports.udptcp_srcdst_n[0] != 0
                    && flow->flowsel.key.ports.udptcp_srcdst_n[0] == 0)
                    goto break_loop;

                /* Current entry is exact src_port and new a range -> insert after this entry */
                if (ip_ntohs(flow->flowsel.key.ports.udptcp_srcdst_n[0]) == flow->flowsel.src_max_port
                    && ip_ntohs(this->flowsel.key.ports.udptcp_srcdst_n[0]) != this->flowsel.src_max_port)
                    goto continue_loop;

                /* New entry is exact src_port and current a range -> insert before this entry */
                if (ip_ntohs(this->flowsel.key.ports.udptcp_srcdst_n[0]) == this->flowsel.src_max_port
                    && ip_ntohs(flow->flowsel.key.ports.udptcp_srcdst_n[0]) != flow->flowsel.src_max_port)
                    goto break_loop;
                break;

            case IP_IPPROTO_ICMP:
            case IP_IPPROTO_ICMPV6:
                /* Current entry has ICMP type and new not -> insert after this entry */
                if (flow->flowsel.key.ports.icmp_typecode[0] != 0
                    && this->flowsel.key.ports.icmp_typecode[0] == 0)
                    goto continue_loop;

                /* New entry has ICMP type and current not -> insert before this entry */
                if (this->flowsel.key.ports.icmp_typecode[0] != 0
                    && flow->flowsel.key.ports.icmp_typecode[0] == 0)
                    goto break_loop;

                /* Current entry has ICMP code and new not -> insert after this entry */
                if (flow->flowsel.key.ports.icmp_typecode[1] != 0
                    && this->flowsel.key.ports.icmp_typecode[1] == 0)
                    goto continue_loop;

                /* New entry has ICMP code and current not -> insert before this entry */
                if (this->flowsel.key.ports.icmp_typecode[1] != 0
                    && flow->flowsel.key.ports.icmp_typecode[1] == 0)
                    goto break_loop;
                break;

            case IP_IPPROTO_MH:
                /* Current entry has MH type and new not -> insert after this entry */
                if (flow->flowsel.key.ports.mh_type[0] != 0
                    && this->flowsel.key.ports.mh_type[0] == 0)
                    goto continue_loop;

                /* New entry has MH type and current not -> insert before this entry */
                if (this->flowsel.key.ports.mh_type[0] != 0
                    && flow->flowsel.key.ports.mh_type[0] == 0)
                    goto break_loop;
                break;

            default:
                break;
            } /* switch() */
        }

#ifdef IPCOM_USE_INET
        if (flow->flowsel.key.domain == IP_AF_INET)
        {
            ip_assert(this->flowsel.key.domain == flow->flowsel.key.domain);

            /* Destination address */

            /* Current entry is exact dst and new a range -> insert after this entry */
            if (flow->flowsel.key.dst.in.s_addr == flow->flowsel.dst_max.in.s_addr
                && this->flowsel.key.dst.in.s_addr != this->flowsel.dst_max.in.s_addr)
                continue;

            /* New entry is exact dst and current a range -> insert before this entry */
            if (this->flowsel.key.dst.in.s_addr == this->flowsel.dst_max.in.s_addr
                && flow->flowsel.key.dst.in.s_addr != flow->flowsel.dst_max.in.s_addr)
                break;

            /* New entry has smaller dst mask than current -> insert after this entry */
            if (ip_ntohl(this->flowsel.mask.dst.in.s_addr) <
                ip_ntohl(flow->flowsel.mask.dst.in.s_addr))
                continue;

            /* New entry has larger dst mask than current -> insert before this entry */
            if (ip_ntohl(this->flowsel.mask.dst.in.s_addr) >
                ip_ntohl(flow->flowsel.mask.dst.in.s_addr))
                break;

            /* Source address */

            /* Current entry is exact src and new a range -> insert after this entry */
            if (flow->flowsel.key.src.in.s_addr == flow->flowsel.src_max.in.s_addr
                && this->flowsel.key.src.in.s_addr != this->flowsel.src_max.in.s_addr)
                continue;

            /* New entry is exact src and current a range -> insert before this entry */
            if (this->flowsel.key.src.in.s_addr == this->flowsel.src_max.in.s_addr
                && flow->flowsel.key.src.in.s_addr != flow->flowsel.src_max.in.s_addr)
                break;

            /* New entry has smaller src mask than current -> insert after this entry */
            if (ip_ntohl(this->flowsel.mask.src.in.s_addr) <
                ip_ntohl(flow->flowsel.mask.src.in.s_addr))
                continue;

            /* New entry has larger src mask than current -> insert before this entry */
            if (ip_ntohl(this->flowsel.mask.src.in.s_addr) >
                ip_ntohl(flow->flowsel.mask.src.in.s_addr))
                break;
        }
#endif /* IPCOM_USE_INET */

#ifdef IPCOM_USE_INET6
        if (flow->flowsel.key.domain == IP_AF_INET6)
        {
            int this_mask_len;
            int flow_mask_len;

            ip_assert(this->flowsel.key.domain == flow->flowsel.key.domain);

            /* Destination address */

            /* Current entry is exact dst and new a range -> insert after this entry */
            if (ipcom_memcmp(&flow->flowsel.key.dst, &flow->flowsel.dst_max, 16) == 0
                && ipcom_memcmp(&this->flowsel.key.dst, &this->flowsel.dst_max, 16))
                continue;

            /* New entry is exact dst and current a range -> insert before this entry */
            if (ipcom_memcmp(&this->flowsel.key.dst, &this->flowsel.dst_max, 16) == 0
                && ipcom_memcmp(&flow->flowsel.key.dst, &flow->flowsel.dst_max, 16))
                break;

            this_mask_len = ipcom_mask_to_prefixlen(&this->flowsel.mask.dst, 128);
            flow_mask_len = ipcom_mask_to_prefixlen(&flow->flowsel.mask.dst, 128);

            /* New entry has larger dst mask than current -> insert before this entry */
            if (this_mask_len > flow_mask_len)
                break;

            /* New entry has smaller dst mask than current -> insert after this entry */
            if (this_mask_len < flow_mask_len)
                continue;

            /* Source address */

            /* Current entry is exact src and new a range -> insert after this entry */
            if (ipcom_memcmp(&flow->flowsel.key.src, &flow->flowsel.src_max, 16) == 0
                && ipcom_memcmp(&this->flowsel.key.src, &this->flowsel.src_max, 16))
                continue;

            /* New entry is exact src and current a range -> insert before this entry */
            if (ipcom_memcmp(&this->flowsel.key.src, &this->flowsel.src_max, 16) == 0
                && ipcom_memcmp(&flow->flowsel.key.src, &flow->flowsel.src_max, 16))
                break;

            this_mask_len = ipcom_mask_to_prefixlen(&this->flowsel.mask.src, 128);
            flow_mask_len = ipcom_mask_to_prefixlen(&flow->flowsel.mask.src, 128);

            /* New entry has larger src mask than current -> insert before this entry */
            if (this_mask_len > flow_mask_len)
                break;

            /* New entry has smaller src mask than current -> insert after this entry */
            if (this_mask_len < flow_mask_len)
                continue;
        }
#endif /* IPCOM_USE_INET6 */

    continue_loop:
        IP_NOOP;
    }

    /* Done sorting -> insert the new flow */
 break_loop:
    if (flow != IP_NULL)
        ipcom_list_insert_before(&flow->flow_list, &this->flow_list);
    else
        ipcom_list_insert_last(head, &this->flow_list);
}


/*
 ****************************************************************************
 * 10                    GLOBAL FUNCTIONS
 ****************************************************************************
 */


/*
 *===========================================================================
 *                    ipipsec_flow_lookup_exact
 *===========================================================================
 * Description:   Get exact flow (used by pfkey code)
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL Ipipsec_flow *
ipipsec_flow_lookup_exact(Ipipsec_flowsel *flowsel, Ipipsec_sa *sa, Ip_u32 flowid)
{
    Ipipsec_flow   *flow, *next;
    Ipcom_list     *list_element;
    int flowdir, domain;

    flow = IP_NULL;

    if (flowid != 0)
    {
        int domain_index;
        for (domain_index = 0; domain_index < 2; domain_index++)
            for (flowdir = 0; flowdir < 2; flowdir++)
                for (flow = IPCOM_LIST_FIRST(&ipipsec.flow_head[domain_index][flowdir]);
                     flow != IP_NULL;
                     flow = IPCOM_LIST_NEXT(&flow->flow_list))
                    if (flowid == flow->id)
                        return flow;
        return IP_NULL;
    }

    ip_assert(flowsel != IP_NULL);
    ip_assert(flowsel->key.policy_id == 0);

    flowdir = (int)flowsel->key.direction;
    domain  = (int)flowsel->key.domain;
    ip_assert(flowdir == IPIPSEC_DIR_OUT || flowdir == IPIPSEC_DIR_IN);
    ip_assert(domain == IP_AF_INET || domain == IP_AF_INET6);

#ifdef IPCOM_USE_INET
    if (domain == IP_AF_INET)
        flow = IPCOM_LIST_FIRST(&ipipsec.flow_head[IPIPSEC_INET][flowdir]);
#endif
#ifdef IPCOM_USE_INET6
    if (domain == IP_AF_INET6)
        flow = IPCOM_LIST_FIRST(&ipipsec.flow_head[IPIPSEC_INET6][flowdir]);
#endif
    if (sa != IP_NULL)
    {
        list_element = IPCOM_LIST_FIRST(&sa->flow_head[flowdir]);
        flow = IPIPSEC_LIST_ENTRY(list_element, Ipipsec_flow, sa_list);
    }

    ip_assert(flow == IP_NULL || flow->magic == IPIPSEC_FLOW_MAGIC);


    /* Scan flow list */
    for(; flow != IP_NULL; flow = next)
    {
        ip_assert(flow->magic == IPIPSEC_FLOW_MAGIC);

        if (sa != IP_NULL)
        {
            list_element = IPCOM_LIST_NEXT(&flow->sa_list);
            next = IPIPSEC_LIST_ENTRY(list_element, Ipipsec_flow, sa_list);
        }
        else
        {
            list_element = IPCOM_LIST_NEXT(&flow->flow_list);
            next = IPIPSEC_LIST_ENTRY(list_element, Ipipsec_flow, flow_list);
        }

        /* Domain may differ on e.g. sa_bypass SA */
        if (domain != flow->flowsel.key.domain)
            continue;

        ip_assert(flow->flowsel.key.policy_id == 0);
        if (ipcom_memcmp(&flow->flowsel, flowsel, sizeof(Ipipsec_flowsel)) == 0)
            return flow;
    }

    return IP_NULL;
}


/*
 *===========================================================================
 *                    ipipsec_flow_lookup
 *===========================================================================
 * Description:
 * Parameters:   Optional sa parameter
 * Returns:
 *
 */
IP_GLOBAL Ipipsec_flow *
ipipsec_flow_lookup(Ipipsec_key *key, Ipipsec_sa *sa, Ip_u16 lookup_flags)
{
    Ipipsec_flow  *flow, *next;
    Ipcom_list    *list_element;
    void          *entry;

    ip_assert(key != IP_NULL);
    ip_assert(key->domain == IP_AF_INET || key->domain == IP_AF_INET6);
    ip_assert(key->direction == IPIPSEC_DIR_IN || key->direction == IPIPSEC_DIR_OUT);

    if (IP_BIT_ISSET(lookup_flags, IPIPSEC_LFLAGS_USEHASH))
    {
        if (ipipsec_hash_lookup(&ipipsec.flow_hash, key, &entry))
        {
            IPSEC_STATS(ipipsec.stats.flow_hash_match++);
            return (Ipipsec_flow *)entry;
        }
    }

    if (sa != IP_NULL)
    {
        list_element = IPCOM_LIST_FIRST(&sa->flow_head[key->direction]);
        flow = IPIPSEC_LIST_ENTRY(list_element, Ipipsec_flow, sa_list);
    }
#ifdef IPCOM_USE_INET
    else if (key->domain == IP_AF_INET)
        flow = IPCOM_LIST_FIRST(&ipipsec.flow_head[IPIPSEC_INET][key->direction]);
#endif
#ifdef IPCOM_USE_INET6
    else if (key->domain == IP_AF_INET6)
        flow = IPCOM_LIST_FIRST(&ipipsec.flow_head[IPIPSEC_INET6][key->direction]);
#endif
    else
        return IP_NULL;

    for(; flow != IP_NULL; flow = next)
    {
        ip_assert(flow->magic == IPIPSEC_FLOW_MAGIC);

        if (sa != IP_NULL)
        {
            list_element = IPCOM_LIST_NEXT(&flow->sa_list);
            next = IPIPSEC_LIST_ENTRY(list_element, Ipipsec_flow, sa_list);
        }
        else
            next = IPCOM_LIST_NEXT(&flow->flow_list);
        ip_assert(next == IP_NULL || next->magic == IPIPSEC_FLOW_MAGIC);


        /* Domain */
        if (key->domain != flow->flowsel.key.domain)
            continue;


        if (IP_BIT_ISFALSE(lookup_flags, IPIPSEC_LFLAGS_NOCHECK_PROTO))
        {
            /* IP protocol */
            if (flow->flowsel.key.proto != 0)
            {
                if (flow->flowsel.key.proto != key->proto)
                    continue;
            }
        }

        /* DSCP */
        if (flow->flowsel.key.dscp != 0)
        {
            if (flow->flowsel.key.dscp != key->dscp)
                continue;
        }

        /* Ports */
        if (IP_BIT_ISFALSE(lookup_flags, IPIPSEC_LFLAGS_NOCHECK_PORTS))
        {
            switch(key->proto)
            {
            case IP_IPPROTO_TCP:
            case IP_IPPROTO_UDP:
                /* Destination port */
                if (flow->flowsel.key.ports.udptcp_srcdst_n[1] != 0)
                {
                    if (ip_ntohs(key->ports.udptcp_srcdst_n[1])
                        < ip_ntohs(flow->flowsel.key.ports.udptcp_srcdst_n[1]))
                        goto continue_loop;
                    if (ip_ntohs(key->ports.udptcp_srcdst_n[1])
                        > flow->flowsel.dst_max_port)
                        goto continue_loop;
                }

                /* Source port */
                if (flow->flowsel.key.ports.udptcp_srcdst_n[0] != 0)
                {
                    if (ip_ntohs(key->ports.udptcp_srcdst_n[0])
                        < ip_ntohs(flow->flowsel.key.ports.udptcp_srcdst_n[0]))
                        goto continue_loop;
                    if (ip_ntohs(key->ports.udptcp_srcdst_n[0])
                        > flow->flowsel.src_max_port)
                        goto continue_loop;
                }
                break;

                /* RFC4301 s6.1.1p64 "To accommodate both ends of this spectrum, a compliant IPsec implementation MUST permit a local administrator to configure an IPsec implementation to accept or reject unauthenticated ICMP traffic.  This control MUST be at the granularity of ICMP type and MAY be at the granularity of ICMP type and code." */
                /* RFC4301 s6.1.2p64 "Thus, implementers MUST provide controls to allow local administrators to constrain the processing of ICMP error messages received on the protected side of the boundary" */
            case IP_IPPROTO_ICMP:
            case IP_IPPROTO_ICMPV6:
                /* ICMP type */
                if (flow->flowsel.key.ports.icmp_typecode[0] != 0)
                {
                    if (flow->flowsel.key.ports.icmp_typecode[0]
                        != key->ports.icmp_typecode[0])
                        goto continue_loop;
                }

                /* ICMP code */
                if (flow->flowsel.key.ports.icmp_typecode[1] != 0)
                {
                    if (flow->flowsel.key.ports.icmp_typecode[1]
                        != key->ports.icmp_typecode[1])
                        goto continue_loop;
                }
                break;

            case IP_IPPROTO_MH:
                /* MH type */
                if (flow->flowsel.key.ports.mh_type[0] != 0)
                {
                    if (flow->flowsel.key.ports.mh_type[0]
                        != key->ports.mh_type[0])
                        goto continue_loop;
                }
                break;

            default:
                break;
            }
        }


        /* IPv4 addresses */
#ifdef IPCOM_USE_INET
        if (flow->flowsel.key.domain == IP_AF_INET)
        {
            /* Destination address */
            if (flow->flowsel.key.dst.in.s_addr != 0)
            {
                if (flow->flowsel.key.dst.in.s_addr == flow->flowsel.dst_max.in.s_addr)
                {
                    /* Destination mask check */
                    if ((flow->flowsel.key.dst.in.s_addr & flow->flowsel.mask.dst.in.s_addr) !=
                        (key->dst.in.s_addr & flow->flowsel.mask.dst.in.s_addr))
                        continue;
                }
                else
                {
                    /* Destination range checks */
                    if (ip_ntohl(key->dst.in.s_addr) < ip_ntohl(flow->flowsel.key.dst.in.s_addr))
                        continue;
                    if (ip_ntohl(key->dst.in.s_addr) > ip_ntohl(flow->flowsel.dst_max.in.s_addr))
                        continue;
                }
            }

            if (IP_BIT_ISFALSE(lookup_flags, IPIPSEC_LFLAGS_NOCHECK_SRC))
            {
                /* Source address */
                if (flow->flowsel.key.src.in.s_addr != 0)
                {
                    if (flow->flowsel.key.src.in.s_addr == flow->flowsel.src_max.in.s_addr)
                    {
                        /* Source mask check */
                        if ((flow->flowsel.key.src.in.s_addr & flow->flowsel.mask.src.in.s_addr) !=
                            (key->src.in.s_addr & flow->flowsel.mask.src.in.s_addr))
                            continue;
                    }
                    else
                    {
                        /* Source range checks */
                        if (ip_ntohl(key->src.in.s_addr) < ip_ntohl(flow->flowsel.key.src.in.s_addr))
                            continue;
                        if (ip_ntohl(key->src.in.s_addr) > ip_ntohl(flow->flowsel.src_max.in.s_addr))
                            continue;
                    }
                }
            }
        }
#endif /* IPCOM_USE_INET */


        /* IPv6 addresses */
#ifdef IPCOM_USE_INET6
        if (flow->flowsel.key.domain == IP_AF_INET6)
        {
            int  shorts;

            if (ipcom_memcmp(&flow->flowsel.key.dst, &flow->flowsel.dst_max, 16))
            {
                /* Destination range */
                if (ipipsec_addrcmp(IP_AF_INET6, &key->dst, &flow->flowsel.key.dst) == -1)
                    continue;
                if (ipipsec_addrcmp(IP_AF_INET6, &key->dst, &flow->flowsel.dst_max) == 1)
                    continue;
            }
            else
            {
                /* Destination mask */
                for (shorts = 0; shorts < 8; shorts++)
                    if (flow->flowsel.mask.dst.in6.in6.addr16[shorts])
                    {
                        /* Check eight 16-bit variables = 128-bit */
                        if ((flow->flowsel.key.dst.in6.in6.addr16[shorts] &
                             flow->flowsel.mask.dst.in6.in6.addr16[shorts])
                            != (key->dst.in6.in6.addr16[shorts] &
                                flow->flowsel.mask.dst.in6.in6.addr16[shorts]))
                            goto continue_loop;
                    }
            }

            if (IP_BIT_ISFALSE(lookup_flags, IPIPSEC_LFLAGS_NOCHECK_SRC))
            {
                if (ipcom_memcmp(&flow->flowsel.key.src, &flow->flowsel.src_max, 16))
                {
                    /* Source range */
                    if (ipipsec_addrcmp(IP_AF_INET6, &key->src, &flow->flowsel.key.src) == -1)
                        continue;
                    if (ipipsec_addrcmp(IP_AF_INET6, &key->src, &flow->flowsel.src_max) == 1)
                        continue;
                }
                else
                {
                    /* Source mask */
                    for (shorts = 0; shorts < 8; shorts++)
                        if (flow->flowsel.mask.src.in6.in6.addr16[shorts])
                        {
                            /* Check eight 16-bit variables = 128-bit */
                            if ((flow->flowsel.key.src.in6.in6.addr16[shorts] &
                                 flow->flowsel.mask.src.in6.in6.addr16[shorts])
                                != (key->src.in6.in6.addr16[shorts] &
                                    flow->flowsel.mask.src.in6.in6.addr16[shorts]))
                                goto continue_loop;
                        }
                }
            }
        }
#endif /* IPCOM_USE_INET6 */


        /* Found a matching flow -> break */
        break;

    continue_loop:
        IP_NOOP;
    }

    /* Okey to hash -> Cache the lookup, regardless of whether its found or not */
    if (IP_BIT_ISSET(lookup_flags, IPIPSEC_LFLAGS_USEHASH))
    {
        IPSEC_STATS(ipipsec.stats.flow_list_match++);
        ipipsec_hash_add(&ipipsec.flow_hash, key, flow);
        IPSEC_STATS(ipipsec.stats.flow_hash_add++);
    }

    return flow;
}


/*
 *===========================================================================
 *                    ipipsec_flow_malloc
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL Ipipsec_flow *
ipipsec_flow_malloc(Ipipsec_flowsel *flowsel, Ipipsec_sa *sa, Ip_u8 flowtype)
{
    Ipipsec_flow   *flow;

    ip_assert(flowsel != IP_NULL);
    ip_assert(flowsel->key.direction == IPIPSEC_DIR_IN
              || flowsel->key.direction == IPIPSEC_DIR_OUT);
    ip_assert(flowsel->key.domain == IP_AF_INET
              || flowsel->key.domain == IP_AF_INET6);

    /* Get memory for flow */
    flow = (Ipipsec_flow *)ipcom_malloc(sizeof(Ipipsec_flow));
    if (flow == IP_NULL)
    {
        IPCOM_LOG0(ERR, "ipipsec_flow_malloc() :: out of memory ('flow')");
        return IP_NULL;
    }

    /* Setup flow */
    ipcom_memset(flow, 0, sizeof(Ipipsec_flow));
    ipcom_list_init(&flow->flow_list);
    ipcom_list_init(&flow->sa_list);
    flow->flowtype  = flowtype;
    flow->magic     = IPIPSEC_FLOW_MAGIC;
    ipcom_memcpy(&flow->flowsel, flowsel, sizeof(Ipipsec_flowsel));

#ifdef IPNET
    /* Invalidate IPNET caches */
    ipnet_route_cache_invalidate();
    ipnet_sock_rx_cache_invalidate(IP_AF_INET);
    ipnet_sock_rx_cache_invalidate(IP_AF_INET6);
#endif

    /* Insert flow in SA list */
    flow->sa = sa;
    flow->id = ++ipipsec.flow_id;
    if (flow->sa != IP_NULL)
        ipcom_list_insert_first(&sa->flow_head[flow->flowsel.key.direction], &flow->sa_list);

    /* Insert in ordered global list */
    ipipsec_flow_insert(flow);

    /* Flow added -> disable all cached entries */
    ipipsec.policy_id++;

    /* info syslog */
    IPCOM_LOG1(INFO, "Creating flow [%s]",
               ipipsec_flow_name(flow, sa));

    return flow;
}


/*
 *===========================================================================
 *                    ipipsec_flow_free
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL void
ipipsec_flow_free(Ipipsec_flow *flow)
{
    ip_assert(flow != IP_NULL);
    ip_assert(flow->magic == IPIPSEC_FLOW_MAGIC);
    ip_assert(flow->flowsel.key.domain == IP_AF_INET || flow->flowsel.key.domain == IP_AF_INET6);

    /* Remove flow from list(s) */
    ipcom_list_remove(&flow->flow_list);
    if (flow->sa)
        ipcom_list_remove(&flow->sa_list);

#ifdef IPNET
    /* Invalidate IPNET caches */
    ipnet_route_cache_invalidate();
    ipnet_sock_rx_cache_invalidate(IP_AF_INET);
    ipnet_sock_rx_cache_invalidate(IP_AF_INET6);
#endif

    /* info syslog */
    IPCOM_LOG1(INFO, "Removing flow [%s]",
               ipipsec_flow_name(flow, flow->sa));

    ipcom_free(flow);

    /* Flow removed -> disable all cached entries */
    ipipsec.policy_id++;
}


/*
 *===========================================================================
 *                    ipipsec_sa_group
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL Ipipsec_sa *
ipipsec_sa_group(Ipipsec_sa *sa1, Ipipsec_sa *sa2)
{
    Ipipsec_sa   *head, *prev;

    ip_assert(sa1);
    ip_assert(sa2);

    /* First ungroup sa1 to make sure its not in a group already.
     * This also effectively avoids circle groups.
     */
    ipipsec_sa_ungroup(sa1);

    /* Grouping to oneself is same as ungrouping */
    if (sa1 == sa2)
    {
        ip_assert(sa1->group_head == IP_NULL);
        return sa1;
    }

#ifdef IPIPSEC_USE_SYSLOG
    {
        int len;
        len = ipcom_sprintf(ipipsec.sysbuf, "Grouping <SA1: %s> -> ", ipipsec_sa_name(sa1));
        (void)ipcom_sprintf(ipipsec.sysbuf + len, "<SA2: %s>", ipipsec_sa_name(sa2));
        IPCOM_LOG0(DEBUG, ipipsec.sysbuf);
    }
#endif

    head = sa2->group_head;
    if (head)
    {
        sa1->group_next = sa2;
        if (head == sa2)
        {
            /* sa2 is group head, make sa1 new group head */
            sa1->group_head = sa1;
            for (prev = sa2; prev; prev = prev->group_next)
                prev->group_head = sa1;
            head = sa1;
        }
        else
        {
            /* sa2 is member of a group, insert sa1 just before sa2 */
            for (prev = head; prev; prev = prev->group_next)
                if (prev->group_next == sa2)
                    break;
            ip_assert(prev);
            ip_assert(prev->group_next = sa2);
            prev->group_next = sa1;
            sa1->group_head = head;
        }
    }
    else
    {
        /* sa2 is not grouped, make new group */
        ip_assert(sa2->group_next == IP_NULL);
        sa1->group_head = sa1;
        sa1->group_next = sa2;
        sa2->group_head = sa1;
        head = sa1;
    }

    return head;
}


/*
 *===========================================================================
 *                    ipipsec_sa_insert
 *===========================================================================
 * Description:   Insert SA in a sorted list where the order of importance
 *                is as follows: SPI, destination IP address, source IP address
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL void
ipipsec_sa_insert(Ipipsec_sa *this)
{
    Ipcom_list    *head;
    Ipipsec_sa    *sa;

    ip_assert(this != &ipipsec.sa_bypass);
    ip_assert(this->key.direction == IPIPSEC_DIR_IN || this->key.direction == IPIPSEC_DIR_OUT);

    switch(this->key.domain)
    {
#ifdef IPCOM_USE_INET
    case IP_AF_INET:
        head = &ipipsec.sa_head[IPIPSEC_INET][this->key.direction];
        break;
#endif
#ifdef IPCOM_USE_INET6
    case IP_AF_INET6:
        head = &ipipsec.sa_head[IPIPSEC_INET6][this->key.direction];
        break;
#endif
    default:
        IP_PANIC();
        return;
    }

    /* Remove SA from list if already in list (due to update) */
    if (IP_BIT_ISSET(this->iflags, IPIPSEC_IFLAGS_IN_SALIST))
        ipcom_list_remove(&this->sa_list);

    /* Find spot to insert SA */
    for(sa = IPCOM_LIST_FIRST(head);
        sa != IP_NULL;
        sa = IPCOM_LIST_NEXT(&sa->sa_list))
    {
        /* Current entry has higher priority than new -> insert after this entry */
        if (sa->key.priority < this->key.priority)
            continue;

        /* Current entry has lower priority than new -> insert before this entry */
        if (sa->key.priority > this->key.priority)
            break;

        /* Current entry has specified destination address -> insert after this entry */
        if (ipcom_memcmp(ipipsec.zeroes, &sa->key.dst, sa->in_addr_size) != 0)
            continue;

        /* Our entry has specified destination address -> insert before this entry */
        if (ipcom_memcmp(ipipsec.zeroes, &this->key.dst, this->in_addr_size) != 0)
            break;

        /* Current entry has specified source address -> insert after this entry */
        if (ipcom_memcmp(ipipsec.zeroes, &sa->key.src, sa->in_addr_size) != 0)
            continue;

        /* Our entry has specified source address -> insert before this entry */
        if (ipcom_memcmp(ipipsec.zeroes, &this->key.src, this->in_addr_size) != 0)
            break;
    }

    /* Done sorting -> insert the new SA */
    if (sa != IP_NULL)
        ipcom_list_insert_before(&sa->sa_list, &this->sa_list);
    else
        ipcom_list_insert_last(head, &this->sa_list);

    /* SA added -> disable all cached entries */
    ipipsec.policy_id++;
    IP_BIT_SET(this->iflags, IPIPSEC_IFLAGS_IN_SALIST);
}


/*
 *===========================================================================
 *                    ipipsec_sa_malloc
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL Ipipsec_sa *
ipipsec_sa_malloc(Ip_u32 spi_n, int sadir, int domain)
{
    Ipipsec_sa    *sa;
    Ip_u32         timeout;

    sa = (Ipipsec_sa *)ipcom_malloc(sizeof(Ipipsec_sa));
    if (sa == IP_NULL)
    {
        IPCOM_LOG1(ERR, "ipipsec_sa_malloc() :: out of memory ('sa' %ld bytes)",
                   sizeof(Ipipsec_sa));
        return IP_NULL;
    }

    /* pre-init SA */
    ipcom_memset(sa, 0, sizeof(Ipipsec_sa));
    ipcom_list_init(&sa->flow_head[IPIPSEC_DIR_IN]);
    ipcom_list_init(&sa->flow_head[IPIPSEC_DIR_OUT]);
    sa->sadb_sa_state         = SADB_SASTATE_LARVAL;
    sa->seql                  = 1;
    sa->key.ports.espah_spi_n = spi_n;
    sa->key.direction         = (Ip_u8)sadir;
    sa->key.domain            = (Ip_u8)domain;
#ifdef IPCOM_USE_INET
    if (domain == IP_AF_INET)
    {
        sa->in_addr_size = 4;
        sa->sock_in_addr_offset = 2;
    }
#endif
#ifdef IPCOM_USE_INET6
    if (domain == IP_AF_INET6)
    {
        sa->in_addr_size = 16;
        sa->sock_in_addr_offset = 6;
    }
#endif

    /* Set current time when added */
    sa->ltcurrent.addtime = ipipsec_seconds_now();

    /* Set SA timeout */
    timeout = ipipsec_sa_next_timeout(sa);
    if (timeout != IPIPSEC_TIME_INFINITE)
    {
        ip_assert(sa->tmo == IP_NULL);
        ipipsec_timeout_schedule(timeout,
                                 (Ipipsec_timeout_handler) ipipsec_sa_timeout,
                                 sa,
                                 &sa->tmo);
    }

    return sa;
}


/*
 *===========================================================================
 *                    ipipsec_sa_free
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL void
ipipsec_sa_free(Ipipsec_sa *sa, Ip_bool remove_group, Ip_bool remlog)
{
    Ipipsec_flow *flow;
    int      flowdir;

    ip_assert(ipnet_is_stack_task());

    if (remlog == IP_TRUE)
    {
        IPCOM_LOG1(INFO, "Removing SA: %s", ipipsec_sa_name(sa));
    }

#ifdef IPHWCRYPTO
    /* Let the hardware finish all outstanding requests */
    iphwcrypto_ipsec_esp_release(sa);
#endif

    /* Statistics */
#ifdef IPIPSEC_USE_MIB
    if (sa->key.direction == IPIPSEC_DIR_IN)
    {
#ifdef IPIPSEC_USE_ESP
        if (sa->sadb_msg_satype == SADB_SATYPE_ESP)
            ipipsec.stats.ipsecEspCurrentInboundSAs--;
#endif
#ifdef IPIPSEC_USE_AH
        if (sa->sadb_msg_satype == SADB_SATYPE_AH)
            ipipsec.stats.ipsecAhCurrentInboundSAs--;
#endif
    }
    else
    {
#ifdef IPIPSEC_USE_ESP
        if (sa->sadb_msg_satype == SADB_SATYPE_ESP)
            ipipsec.stats.ipsecEspCurrentOutboundSAs--;
#endif
#ifdef IPIPSEC_USE_AH
        if (sa->sadb_msg_satype == SADB_SATYPE_AH)
            ipipsec.stats.ipsecAhCurrentOutboundSAs--;
#endif
    }
#endif

    /* Delete all flows using this SA */
    for (flowdir = 0; flowdir < 2; flowdir++)
    {
        Ipcom_list *list_element;

        while ((list_element = IPCOM_LIST_FIRST(&sa->flow_head[flowdir])) != IP_NULL)
        {
            flow = ipcom_list_entry(list_element, Ipipsec_flow, sa_list);
            ipipsec_flow_free(flow);
        }
    }

    /* Remove SA from group and possibly its trailing members */
    if (remove_group == IP_TRUE)
    {
        Ipipsec_sa  *sam, *nextsam;
        for (sam = sa->group_next; sam; sam = nextsam)
        {
            nextsam = sam->group_next;
            ipipsec_sa_free(sam, IP_FALSE, remlog);
        }
    }
    ipipsec_sa_ungroup(sa);

    /* Cancel any pending timeout */
    ipipsec_timeout_cancel(sa->tmo);
#ifdef IPIPSEC_USE_TFC
    /* Cancel dummy packet timeout */
    ipipsec_timeout_cancel(sa->tfc_tmo);
#endif


    if (sa == &ipipsec.sa_bypass)
    {
        /* Bypass SA -> assert some then return */
        ip_assert(sa->sa_list.next == IP_NULL);
        ip_assert(sa->encrypt == IP_NULL);
        ip_assert(sa->auth == IP_NULL);
        return;
    }

#ifdef IPCOM_USE_FORWARDER_IPSEC
    ipcom_forwarder_ipsec_remove_sa(sa);
#endif

    /* Remove SA from global list */
    if (IP_BIT_ISSET(sa->iflags, IPIPSEC_IFLAGS_IN_SALIST))
        ipcom_list_remove(&sa->sa_list);

    /* SA removed -> disable all cached entries */
    ipipsec.policy_id++;

    /* Free SA memory */
    if (sa->encrypt)
    {
        ipcom_memset(sa->encrypt, 0, sa->encrypt->alg->key_size);   /* clear encrypt data */
        ipcom_free(sa->encrypt);
    }
    if (sa->auth)
    {
        if (sa->auth->key)
        {
            ipcom_memset(sa->auth->key, 0, sa->auth->keyLen);   /* clear auth key */
            ipcom_free(sa->auth->key);
        }
        ipcom_memset(sa->auth, 0, sizeof(Ipipsec_sa_auth));   /* clear auth data */
        ipcom_free(sa->auth);
    }
#ifdef IPIPSEC_USE_BITARRAY
    if (sa->replay_bitarray != IP_NULL)
        ipcom_bitarray_free(sa->replay_bitarray);
#endif
    ipcom_free(sa);
}


/*
 *===========================================================================
 *                    ipipsec_sa_lookup_exact
 *===========================================================================
 * Description:   Exact lookup of SA in global database
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL Ipipsec_sa *
ipipsec_sa_lookup_exact(Ipipsec_key *key)
{
    Ipcom_list    *head = IP_NULL;
    Ipipsec_sa    *sa;

    ip_assert(key != IP_NULL);
    ip_assert(key->domain == IP_AF_INET || key->domain == IP_AF_INET6);
    ip_assert(key->direction == IPIPSEC_DIR_IN || key->direction == IPIPSEC_DIR_OUT);

#ifdef IPCOM_USE_INET
    if (key->domain == IP_AF_INET)
        head = &ipipsec.sa_head[IPIPSEC_INET][key->direction];
#endif
#ifdef IPCOM_USE_INET6
    if (key->domain == IP_AF_INET6)
        head = &ipipsec.sa_head[IPIPSEC_INET6][key->direction];
#endif
    ip_assert(head != IP_NULL);

    for(sa = IPCOM_LIST_FIRST(head);
        sa != IP_NULL;
        sa = IPCOM_LIST_NEXT(&sa->sa_list))
    {
        if (ipcom_memcmp(key, &sa->key, sizeof(Ipipsec_key)) == 0)
            return sa;
    }

    return IP_NULL;
}


/*
 *===========================================================================
 *                    ipipsec_sa_lookup
 *===========================================================================
 * Description:   Lookup SA in global database
 *
 * RFC4301 s4.1p12: "If an IPsec implementation supports multicast, then it MUST support multicast SAs.."
 * RFC4301 s4.1p12: "A multicast-capable IPsec implementation MUST correctly de-multiplex inbound traffic even in the context of SPI collisions."
 * RFC4301 s4.1p13: "although its externally visible behavior MUST be functionally equivalent to having searched the SAD in the above order."
 * RFC4301 s4.1p13: "The indication of whether source and destination address matching is required to map inbound IPsec traffic to SAs MUST be set either as a side effect of manual SA configuration or via negotiation using an SA management protocol" (note: implemented using SRCREQ sa flag)
 * RFC4301 s4.1p13: "..Quality of Service (QoS) appropriately. To permit this, the IPsec implementation MUST permit establishment and maintenance of multiple SAs between a given sender and receiver"
 * RFC4301 s4.1p13: "The receiver MUST process the packets from the different SAs without prejudice."
 * RFC4301 s4.1p34: "See Section 4.1 for details on the algorithm that MUST be used for mapping inbound IPsec datagrams to SAs"
 *
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL Ipipsec_sa *
ipipsec_sa_lookup(Ipipsec_key *key, Ip_u16 lookup_flags)
{
    Ipcom_list    *head = IP_NULL;
    Ipipsec_sa    *sa;
    void          *entry;

    ip_assert(key != IP_NULL);
    ip_assert(key->domain == IP_AF_INET || key->domain == IP_AF_INET6);
    ip_assert(key->direction == IPIPSEC_DIR_IN || key->direction == IPIPSEC_DIR_OUT);

    if (IP_BIT_ISSET(lookup_flags, IPIPSEC_LFLAGS_USEHASH))
    {
        if (ipipsec_hash_lookup(&ipipsec.sa_hash, key, &entry))
        {
            IPSEC_STATS(ipipsec.stats.sa_hash_match++);
            return (Ipipsec_sa *)entry;
        }
    }

#ifdef IPCOM_USE_INET
    if (key->domain == IP_AF_INET)
        head = &ipipsec.sa_head[IPIPSEC_INET][key->direction];
#endif
#ifdef IPCOM_USE_INET6
    if (key->domain == IP_AF_INET6)
        head = &ipipsec.sa_head[IPIPSEC_INET6][key->direction];
#endif
    ip_assert(head != IP_NULL);

    /* First -> search using source, destination, SPI and protocol */
    for(sa = IPCOM_LIST_FIRST(head);
        sa != IP_NULL;
        sa = IPCOM_LIST_NEXT(&sa->sa_list))
    {
        ip_assert(sa->key.domain == key->domain);
        if (key->ports.espah_spi_n != sa->key.ports.espah_spi_n)
            continue;
        if (key->proto != sa->key.proto)
            continue;
        if (ipcom_memcmp(&key->src, &sa->key.src, sa->in_addr_size) != 0)
            continue;
        if (ipcom_memcmp(&key->dst, &sa->key.dst, sa->in_addr_size) != 0)
            continue;
        goto found;
    }

    /* Second -> search using destination, SPI and protocol */
    for(sa = IPCOM_LIST_FIRST(head);
        sa != IP_NULL;
        sa = IPCOM_LIST_NEXT(&sa->sa_list))
    {
        ip_assert(sa->key.domain == key->domain);
        if (IP_BIT_ISSET(sa->sadb_sa_flags, SADB_X_SAFLAGS_SRCREQ))
            continue;
        if (key->ports.espah_spi_n != sa->key.ports.espah_spi_n)
            continue;
        if (key->proto != sa->key.proto)
            continue;
        if (ipcom_memcmp(&key->dst, &sa->key.dst, sa->in_addr_size) != 0)
            continue;
        goto found;
    }

    /* Third -> search using SPI and protocol only (NOTE: packet input only!) */
    if (IP_BIT_ISSET(lookup_flags, IPIPSEC_LFLAGS_USEHASH))
    {
        for(sa = IPCOM_LIST_FIRST(head);
            sa != IP_NULL;
            sa = IPCOM_LIST_NEXT(&sa->sa_list))
        {
            ip_assert(sa->key.domain == key->domain);
            if (IP_BIT_ISSET(sa->sadb_sa_flags, SADB_X_SAFLAGS_SRCREQ))
                continue;
            if (key->ports.espah_spi_n != sa->key.ports.espah_spi_n)
                continue;
            if (key->proto != sa->key.proto)
                continue;
            goto found;
        }
    }

    return IP_NULL;

 found:
    if (IP_BIT_ISSET(lookup_flags, IPIPSEC_LFLAGS_USEHASH))
    {
        IPSEC_STATS(ipipsec.stats.sa_list_match++);
        ipipsec_hash_add(&ipipsec.sa_hash, key, sa);
        IPSEC_STATS(ipipsec.stats.sa_hash_add++);
    }

    return sa;
}


/*
 *===========================================================================
 *                    ipipsec_sa_reset_timeout
 *===========================================================================
 * Description: Insert SA in list
 * Parameters:  sa - SA
 * Returns:     Minimum expiration time.
 *
 */
IP_GLOBAL void
ipipsec_sa_reset_timeout(Ipipsec_sa  *sa)
{
    Ip_u32 timeout;

    timeout = ipipsec_sa_next_timeout(sa);

    ipipsec_timeout_cancel(sa->tmo);

    if (timeout != IPIPSEC_TIME_INFINITE)
        ipipsec_timeout_schedule(timeout,
                                 (Ipipsec_timeout_handler) ipipsec_sa_timeout,
                                 sa,
                                 &sa->tmo);
}


#ifdef IPIPSEC_USE_TFC
/*
 *===========================================================================
 *                    ipipsec_tfc_reset_timeout
 *===========================================================================
 * Description: Reset TFC timeout on ESP SA
 * Parameters:  sa - SA
 * Returns:     Minimum expiration time.
 *
 */
IP_GLOBAL void
ipipsec_tfc_reset_timeout(Ipipsec_sa *sa)
{
    Ip_u32 timeout;

    if (ipipsec_max_tfc_delay == 0)
        /* A max of 0 means no random transmissions */
        return;

    /* Timeout in seconds */
    timeout = 1 + (Ip_u32)ipcom_random();
    timeout = timeout % (Ip_u32)ipipsec_max_tfc_delay;
    if (timeout < (Ip_u32)ipipsec_min_tfc_delay)
        timeout += ipipsec_min_tfc_delay;

    ipipsec_timeout_cancel(sa->tfc_tmo);
    ipipsec_timeout_schedule(timeout,
                                 (Ipipsec_timeout_handler) ipipsec_tfc_timeout,
                                 sa,
                                 &sa->tfc_tmo);
}


/*
 *===========================================================================
 *                    ipipsec_tfc_timeout
 *===========================================================================
 * Description: tfc timeout handler
 * Parameters: sa -SA
 * Returns:
 *
 */
IP_STATIC void
ipipsec_tfc_timeout(Ipipsec_sa *sa)
{
    IPSEC_STATS(ipipsec.stats.sa_dummy_generated++);

    ip_assert(sa->key.direction == IPIPSEC_DIR_OUT);
    ip_assert(sa->sadb_msg_satype == SADB_SATYPE_ESP);

    switch(sa->key.domain)
    {
#ifdef IPCOM_USE_INET
    case IP_AF_INET:
        IPCOM_LOG0(DEBUG2,"Generated a dummy inet packet");
        ipipsec_tfc_generate_dummy_inet(sa);
        break;
#endif
#ifdef IPCOM_USE_INET6
    case IP_AF_INET6:
        IPCOM_LOG0(DEBUG2,"Generated a dummy inet6 packet");
        ipipsec_tfc_generate_dummy_inet6(sa);
        break;
#endif
    default:
        IP_PANIC2();
        return;
    }

    ipipsec_tfc_reset_timeout(sa);
}


/*
 *===========================================================================
 *                    ipipsec_tfc_generate_dummy_inet
 *===========================================================================
 * Description: generate dummy IPv4 packet
 * Parameters: sa - SA
 * Returns:
 *
 */
#ifdef IPCOM_USE_INET
IP_STATIC void
ipipsec_tfc_generate_dummy_inet(Ipipsec_sa *sa)
{
    Ipnet_ip4_output_param param;
    Ipcom_pkt             *pkt;
    Ip_u32                 random_size;

    random_size  = 1 + (Ip_u32)ipcom_random();
    random_size %= (Ip_u32)ipipsec_max_dummy_payload;

    /* Setup IPNET IPv4 output param*/
    ipcom_memset(&param, 0, sizeof(param));
    param.to    = (struct Ip_in_addr *)&sa->key.dst;
    param.from  = (struct Ip_in_addr *)&sa->key.src;
    param.proto = IP_IPPROTO_NONEXTHDR;
    param.ttl   = 255;
    param.tos   = 0;

    /* Allocate ip4 packet + random payload */
    pkt = ipcom_pkt_malloc(IPCOM_IP_HEADER_LENGTH + random_size,
                           IP_FLAG_FC_STACKCONTEXT);
    if (pkt == IP_NULL)
    {
        IPCOM_LOG0(ERR, "Out of memory for dummy IPv4 TFC packet");
        return;
    }
    IP_BIT_SET(pkt->flags, IPCOM_PKT_FLAG_NONBLOCKING);

    /* Set packet data */
    pkt->start   = ipcom_conf_max_link_hdr_size + IPCOM_IP_HEADER_LENGTH;
    pkt->end     = pkt->start + (int)random_size;
    pkt->ifindex = 0;

    /* Copy over random payload */
    ipipsec_crypt_random_fill(&pkt->data[pkt->start], (int)random_size);

    (void)ipnet_ip4_output(&param,pkt);
}
#endif /* IPCOM_USE_INET */


/*
 *===========================================================================
 *                    ipipsec_tfc_generate_dummy_inet6
 *===========================================================================
 * Description: generate dummy IPv6 pkt
 * Parameters: sa - SA
 * Returns:
 *
 */
#ifdef IPCOM_USE_INET6
IP_STATIC void
ipipsec_tfc_generate_dummy_inet6(Ipipsec_sa *sa)
{
    Ipnet_ip6_output_param param;
    Ipcom_pkt             *pkt;
    Ip_u32                 random_size;

    random_size  = 1 + (Ip_u32)ipcom_random();
    random_size %= (Ip_u32)ipipsec_max_dummy_payload;

    /* Setup IPNET IPv6 output param*/
    ipcom_memset(&param, 0, sizeof(param));
    param.from       = (struct Ip_in6_addr *)&sa->key.src;
    param.to         = (struct Ip_in6_addr *)&sa->key.dst;
    param.final_to   = (struct Ip_in6_addr *)&sa->key.dst;
    param.nxt_hdr    = IP_IPPROTO_NONEXTHDR;
    param.hlim       = 255;
    param.has_exthdr = IP_FALSE;

    /* Allocate IPv6 packet + random payload */
    pkt = ipcom_pkt_malloc(IPCOM_IP6_HEADER_LENGTH + random_size,
                           IP_FLAG_FC_STACKCONTEXT);
    if (pkt == IP_NULL)
    {
        IPCOM_LOG0(ERR, "Out of memory for dummy IPv6 TFC packet");
        return;
    }
    IP_BIT_SET(pkt->flags, IPCOM_PKT_FLAG_NONBLOCKING);

    /* Set packet data */
    pkt->start   = ipcom_conf_max_link_hdr_size + IPCOM_IP_HEADER_LENGTH;
    pkt->end     = pkt->start + (int)random_size;
    pkt->ifindex = 0;

    /* Copy over random payload & set pointers */
    ipipsec_crypt_random_fill(&pkt->data[pkt->start], (int)random_size);

    (void)ipnet_ip6_output(&param, pkt);
}
#endif /* IPCOM_USE_INET6 */


/*
 *===========================================================================
 *                    ipipsec_tfc_remove_padding
 *===========================================================================
 * Context:
 * Description:
 * Parameters:
 * Returns:   0 if valid packet, -1 if dummy packet
 *
 */
IP_STATIC int
ipipsec_tfc_remove_padding(Ipipsec_param *param, Ipcom_pkt *pkt)
{
    int  tfc_padding;

    /* Find and strip TFC padding, if any.
     * Any undetected padding might cause problems
     * at transport layer processing especially for IPSEC tranport mode
     */
    switch(param->key.proto)
    {
    case IP_IPPROTO_UDP:
        tfc_padding = (pkt->end - pkt->start) -
            ip_ntohs(((Ipcom_udp_header *)&pkt->data[pkt->start])->length);
        break;

#ifdef IPCOM_USE_INET
    case IP_IPPROTO_IPIP:
        tfc_padding = (pkt->end - pkt->start) -
            ip_ntohs(((Ipipsec_ip_header *)&pkt->data[pkt->start])->inet.length);
        break;
#endif

#ifdef IPCOM_USE_INET6
    case IP_IPPROTO_IPV6:
        tfc_padding = (pkt->end - pkt->start) -
            (ip_ntohs(((Ipipsec_ip_header *)&pkt->data[pkt->start])->inet6.plen)
             + IPCOM_IP6_HEADER_LENGTH);
        break;
#endif

    case IP_IPPROTO_NONEXTHDR:
        /* Next header = 59 support for dummy tfc packets */
        return -1; /* discard this input packet */

    default:
        tfc_padding = 0;
        break;
    }

    IPCOM_LOG1(DEBUG2,"          ESP  removing %d bytes TFC padding",
               tfc_padding);
    pkt->end -= tfc_padding;
    return 0;
}
#endif /* IPIPSEC_USE_TFC */


/*
 *===========================================================================
 *                    ipipsec_addrcmp
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:    1 if addr1 is higher than addr2, -1 if addr1 is lower than addr2
 *             and 0 if addresses are equal
 *
 */
IP_GLOBAL int
ipipsec_addrcmp(int domain, union Ip_in_addr_union *addr1, union Ip_in_addr_union *addr2)
{
#ifdef IPCOM_USE_INET
    if (domain == IP_AF_INET)
    {
        if (ip_ntohl(addr1->in.s_addr) > ip_ntohl(addr2->in.s_addr))
            return 1;
        else if (ip_ntohl(addr1->in.s_addr) < ip_ntohl(addr2->in.s_addr))
            return -1;
    }
#endif
#ifdef IPCOM_USE_INET6
    if (domain == IP_AF_INET6)
    {
        int i;

        for (i = 0; i < 4; i++)
        {
            if (ip_ntohl(addr1->in6.in6.addr32[i]) > ip_ntohl(addr2->in6.in6.addr32[i]))
                return 1;
            if (ip_ntohl(addr1->in6.in6.addr32[i]) < ip_ntohl(addr2->in6.in6.addr32[i]))
                return -1;
        }
    }
#endif

    /* same */
    return 0;
}


/*
 *===========================================================================
 *                    ipipsec_output_add_ip_header
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL void
ipipsec_output_add_ip_header(Ipcom_pkt *pkt, Ipipsec_param *param)
{
    Ipipsec_ip_header  *iph;

    /* Add UDP encapsulation header */
    if (param->udp.src_port != 0)
    {
        Ipcom_udp_header *udph;

        pkt->start -= IPCOM_UDP_HEADER_LENGTH;
        udph = (Ipcom_udp_header *)&pkt->data[pkt->start];

        udph->src_port = param->udp.src_port;
        udph->dst_port = param->udp.dst_port;
        udph->length   = (Ip_u16)ip_htons(pkt->end - pkt->start);
        udph->checksum = 0;

        param->key.proto    = IP_IPPROTO_UDP;
        param->udp.src_port = 0;
    }

    /* Make room for IP header in new packet */
    pkt->start  -= param->iph_length;
    pkt->ipstart = pkt->start;

    /* Copy the entire IP header (including options/extensions) */
    iph = (Ipipsec_ip_header *)&pkt->data[pkt->start];
    ipcom_memcpy(iph, param->iph, param->iph_length);

    /* Update IP header */
    switch (param->key.domain)
    {
#ifdef IPCOM_USE_INET
    case IP_AF_INET:
        ip_assert((pkt->data[pkt->start] & 0xf0) == 0x40);

        iph->inet.length   = (Ip_u16)ip_htons(pkt->end - pkt->start);
        iph->inet.protocol = param->key.proto;
        iph->inet.checksum = 0;
        iph->inet.checksum = ipcom_in_checksum(iph, param->iph_length);
        break;
#endif /* IPCOM_USE_INET */

#ifdef IPCOM_USE_INET6
    case IP_AF_INET6:
        ip_assert((pkt->data[pkt->start] & 0xf0) == 0x60);

        iph->inet6.plen = (Ip_u16)ip_htons(pkt->end - pkt->start - IPCOM_IP6_HEADER_LENGTH);
        if (param->iph_length == IPCOM_IP6_HEADER_LENGTH)
        {
            iph->inet6.nxt = param->key.proto;
        }
        else
        {
            int    i;
            Ip_pkt_ip6_ext_hdr  *ext_hdr;
            unsigned             ext_hdr_len;

            /* Find the last extension */
            for (i = IPCOM_IP6_HEADER_LENGTH; i < param->iph_length; i += ext_hdr_len)
            {
                ext_hdr     = (Ip_pkt_ip6_ext_hdr *)&pkt->data[pkt->ipstart + i];
                ext_hdr_len = (ext_hdr->len + 1) << 3;

                if (i + (int)ext_hdr_len  == param->iph_length)
                {
                    ext_hdr->nxt = param->key.proto;
                    return;
                }
            }
            IP_PANIC();
        }
        break;
#endif /* IPCOM_USE_INET6 */

    default:
        IP_PANIC();
        break;
    }
}


/*
 *===========================================================================
 *                    ipipsec_output_create_ip_header
 *===========================================================================
 * Description:   Create outer IP header
 *
 * RFC4301 s5.1.2.1p57 "DS field: copied from inner hdr (5)"
 * RFC4301 s8.1p69 "All IPsec implementations MUST support the option of copying the DF bit from an outbound packet to the tunnel mode header that it emits"
 * RFC4301 s8.1p69 "it MUST be possible to configure the implementation's treatment of the DF bit (set, clear, copy from inner header) for each SA.
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipipsec_output_create_ip_header(Ipipsec_sa *sa, Ipipsec_param *param)
{
    /* Prepare UDP encapsulation */
    if (IP_BIT_ISSET(sa->sadb_sa_flags, SADB_X_SAFLAGS_UDPENCAP))
    {
        param->udp.src_port = sa->encap_srcport_n;
        param->udp.dst_port = sa->encap_dstport_n;
        /* Note: checksum and length are set when added */
    }

    /* Create new IP header */
#ifdef IPCOM_USE_INET
    if (param->key.domain == IP_AF_INET)
    {
        int    id;
        Ip_u16 frag;

        frag              = param->iph->inet.frag;
        param->iph        = &param->ip;
        param->iph_length = IPCOM_IP_HEADER_LENGTH;  /* no options */

        param->iph->inet.vers_len  = 0x45;
        param->iph->inet.tos       = param->tos_class;
        /* ignore length since updated if inserted */
        (void)ipcom_ipsec_ioevent(IPIPSEC_EIOXIP4ID, &id);
        param->iph->inet.id        = (Ip_u16)id;

        if (IP_BIT_ISSET(sa->sadb_sa_flags, SADB_X_SAFLAGS_SETDF))
            param->iph->inet.frag  = IPCOM_IPFRAG_DF;  /* Set DF bit */
        else if (IP_BIT_ISSET(sa->sadb_sa_flags, SADB_X_SAFLAGS_CLRDF))
            param->iph->inet.frag  = 0;                /* Clear DF bit */
        else
            param->iph->inet.frag  = frag & IPCOM_IPFRAG_DF;  /*!! Keep DF bit */
        param->iph->inet.ttl       = param->ttl;
        param->iph->inet.protocol  = param->key.proto;
        ipcom_memcpy(&param->iph->inet.src, &param->key.src, sizeof(struct Ip_in_addr));
        ipcom_memcpy(&param->iph->inet.dst, &param->key.dst, sizeof(struct Ip_in_addr));
    }
#endif /* IPCOM_USE_INET */

#ifdef IPCOM_USE_INET6
    if (param->key.domain == IP_AF_INET6)
    {
        Ip_u32  ver_class_flow = 0x60000000;

        param->iph        = &param->ip;
        param->iph_length = IPCOM_IP6_HEADER_LENGTH;  /* no extensions */

        /* Add the traffic class */
        ver_class_flow |= (param->tos_class & 0xff) << 20;
        IP_SET_HTONL(&param->iph->inet6.ver_class_flow, ver_class_flow);
        /* ignore plen since updated if inserted */
        param->iph->inet6.nxt  = param->key.proto;
        param->iph->inet6.hlim = param->ttl;

        ipcom_memcpy(&param->iph->inet6.src, &param->key.src, sizeof(struct Ip_in6_addr));
        ipcom_memcpy(&param->iph->inet6.dst, &param->key.dst, sizeof(struct Ip_in6_addr));
    }
#endif /* IPCOM_USE_INET6 */
}


/*
 *===========================================================================
 *                    ipipsec_create_consecutive_pkt
 *===========================================================================
 * Description:  Called for transport mode packets on input to strip ESP/AH
 *               header that just has been parsed.
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL void
ipipsec_create_consecutive_pkt(Ipcom_pkt *pkt, Ipipsec_param *param)
{
    int  difflen;

    difflen = pkt->start - param->pkt_org_start;
    if (difflen != 0)
    {
        ip_assert(difflen > 0);

        /* Move up the last IP header (the most inner) to create a consecutive datagram */
        ipcom_memmove(&pkt->data[pkt->ipstart + difflen],
                      &pkt->data[pkt->ipstart],
                      param->pkt_org_start - pkt->ipstart);
        pkt->ipstart += difflen;

        switch (param->key.domain)
        {
#ifdef IPCOM_USE_INET
        case IP_AF_INET:
            {
                Ipcom_ip_header  *iph;
                Ip_u16   iph_length;

                /* Adjust IP header length, protocol and checksum */
                iph = (Ipcom_ip_header *)&pkt->data[pkt->ipstart];
                iph_length    = (Ip_u16)(ip_ntohs(iph->length) - difflen - (param->pkt_org_end - pkt->end));
                iph->length   = ip_htons(iph_length);
                iph->protocol = param->key.proto;
                iph->checksum = 0;
                iph->checksum = ipcom_in_checksum(iph, (iph->vers_len & 0x0F) << 2);
            }
            break;
#endif /* IPCOM_USE_INET */

#ifdef IPCOM_USE_INET6
        case IP_AF_INET6:
            {
                Ipcom_ip6_header *ip6;

                /* Adjust IP header length and protocol */
                ip6       = (Ipcom_ip6_header *)&pkt->data[pkt->ipstart];
                ip6->plen = (Ip_u16)ip_htons(pkt->end - pkt->start);
                ip6->nxt  = param->key.proto;
            }
            break;
#endif /* IPCOM_USE_INET6 */
        }
    }
}


/*
 *===========================================================================
 *                    ipipsec_ah_digest
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
#ifdef IPIPSEC_USE_AH
IP_STATIC int
ipipsec_ah_digest(Ipipsec_sa *sa, Ipcom_pkt *pkt,
                  Ipipsec_param *param, Ip_u32 seqh,
                  Ip_u8 *calc_digest)
{
    Ipipsec_auth_ctx   ctx;
    Ipipsec_ip_header ip_hdr;
    int     iph_length;
    Ip_u8  *buf;
    int     i;

    ip_assert(sa->auth != IP_NULL);

    /***** Calculate HMAC *****/

    /**** inner HMAC (includes IP header+AH header+12*0+payload) ****/
    ipcom_memcpy(&ctx, &sa->auth->i_ctx, sizeof(Ipipsec_auth_ctx));


    /***** IPv4 Pseudo Header *****/
#ifdef IPCOM_USE_INET
    if (sa->key.domain == IP_AF_INET)
    {
        /* Create IPv4 pseudo header */
        ip_hdr.inet.vers_len   = param->iph->inet.vers_len;
        ip_hdr.inet.tos        = 0; /* mutable field */
        ip_hdr.inet.length     = (Ip_u16)(pkt->end - pkt->start + param->iph_length);
        ip_hdr.inet.length     = ip_htons(ip_hdr.inet.length);
        ip_hdr.inet.id         = param->iph->inet.id;
        if (sa->sadb_sa_auth == SADB_X_AALG_MD5 || sa->sadb_sa_auth == SADB_X_AALG_SHA1)
            ip_hdr.inet.frag   = param->iph->inet.frag & IPCOM_IPFRAG_DF;  /* OLD AH, keep DF bit */
        else
            ip_hdr.inet.frag   = 0; /* mutable field */
        ip_hdr.inet.ttl        = 0; /* mutable field */
        ip_hdr.inet.protocol   = IP_IPPROTO_AH;
        ip_hdr.inet.checksum   = 0; /* mutable field */
        ip_hdr.inet.src.addr32 = param->iph->inet.src.addr32;
        ip_hdr.inet.dst.addr32 = param->iph->inet.dst.addr32;

        /* IPv4 header, excluding options */
        sa->auth->alg->update(&ctx, (Ip_u8 *)&ip_hdr, IPCOM_IP_HEADER_LENGTH);

        /* IPv4 options */
        ip_assert(param->iph_length >= IPCOM_IP_HEADER_LENGTH);
        iph_length = param->iph_length - IPCOM_IP_HEADER_LENGTH;
        buf = (Ip_u8 *)((Ip_u8 *)param->iph + IPCOM_IP_HEADER_LENGTH);

        for (i = 0; i < iph_length; i += buf[i+1])
        {
            /* Length check on all options except EOL & NOP */
            if (buf[i] > 0x01)
            {
                if (i + 1 >= iph_length || i + buf[i+1] > iph_length)
                {
                bad_length:
                    IP_PANIC2();
                    IPCOM_LOG0(WARNING, "<discard> :: bad IPv4 options length.");
                    return -IP_ERRNO_EFAULT;
                }
            }

            switch(buf[i])
            {
                /* Immutable options */
            case 0x00: /* EOL */
                sa->auth->alg->update(&ctx, ipipsec.zeroes, 1);
                goto ah;
            case 0x01: /* NOP */
                sa->auth->alg->update(&ctx, &buf[i], 1);
                break;
            case 0x82: /* Security - RFC 1108 (historic but in use) */
            case 0x85: /* Extended  Security - RFC 1108 (historic but in use) */
            case 0x86: /* Commercial Security - expired I-D, now US MIL STD */
            case 0x94: /* Router Alert - RFC 2113 */
            case 0x95: /* Sender Directed Multidest Delivery - RFC 1770 */
                if (buf[i+1] < 2)
                    goto bad_length;
                sa->auth->alg->update(&ctx, &buf[i], buf[i+1]);
                break;

                /* Mutable options */
            default:
                sa->auth->alg->update(&ctx, ipipsec.zeroes, buf[i+1]);
                break;
            }
        }
    }
#endif /* IPCOM_USE_INET */


    /***** IPv6 Pseudo Header *****/
#ifdef IPCOM_USE_INET6
    if (sa->key.domain == IP_AF_INET6)
    {
        struct Ip_in6_addr   dst6;
        int     length;
        Ip_u8   nxt_hdr;

        ipcom_memcpy(&dst6, &param->iph->inet6.dst, sizeof(struct Ip_in6_addr));  /* save dst for routing ext. calc */
        ipcom_memcpy(&ip_hdr.inet6, param->iph, IPCOM_IP6_HEADER_LENGTH);
        ipcom_memcpy(&ip_hdr.inet6.dst, &param->key.dst.in6, sizeof(struct Ip_in6_addr));  /* set final dest */

        if (param->iph_length == IPCOM_IP6_HEADER_LENGTH)
            ip_hdr.inet6.nxt = IP_IPPROTO_AH;
        else
        {
            ip_assert(ip_hdr.inet6.nxt == IP_IPPROTO_HOPOPTS
                      || ip_hdr.inet6.nxt == IP_IPPROTO_DSTOPTS
                      || ip_hdr.inet6.nxt == IP_IPPROTO_ROUTING);
        }

        /* Fix mutable fields */
        ip_hdr.inet6.ver_class_flow = ip_htonl(0x60000000);
        ip_hdr.inet6.plen = (Ip_u16)(ip_ntohs(pkt->end - pkt->start + param->iph_length - IPCOM_IP6_HEADER_LENGTH));
        ip_hdr.inet6.hlim = 0;

        /* IPv6 header, excluding options */
        sa->auth->alg->update(&ctx, (Ip_u8 *)&ip_hdr, IPCOM_IP6_HEADER_LENGTH);

        /* IPv6 extensions and their options */
        iph_length = param->iph_length;
        ip_assert(iph_length >= IPCOM_IP6_HEADER_LENGTH);
        iph_length -= IPCOM_IP6_HEADER_LENGTH;
        if (iph_length > 0)
        {
            buf = (Ip_u8 *)((Ip_u8 *)param->iph + IPCOM_IP6_HEADER_LENGTH);
            nxt_hdr = ip_hdr.inet6.nxt;
            for (i = 0; i < iph_length; i += length)
            {
                /* Extension header length check */
                if (i + 1 > iph_length)
                {
                bad_ipv6_length:
                    IP_PANIC2();
                    IPCOM_LOG0(WARNING, "<discard> :: bad IPv6 header length.");
                    return -IP_ERRNO_EFAULT;
                }
                length = 8 + (buf[i+1] << 3);
                if (i + length > iph_length)
                    goto bad_ipv6_length;

                switch(nxt_hdr)
                {
                    /* "outer" headers */
                case IP_IPPROTO_ROUTING:  /* type0 -> no options */
                    if (i + length == iph_length)
                        sa->auth->alg->update(&ctx, &param->key.proto, 1);  /* Last extension, use next == AH */
                    else
                        sa->auth->alg->update(&ctx, &buf[i], 1);        /* Next extension */
                    if (param->key.direction == IPIPSEC_DIR_IN)
                        sa->auth->alg->update(&ctx, &buf[i+1], length - 1);
                    else
                    {
                        /* Output, create the extension as it will look on the final dest */
                        int   j;
                        sa->auth->alg->update(&ctx, &buf[i+1], 2);          /* Hdr Ext Len + Routing Type */
                        sa->auth->alg->update(&ctx, ipipsec.zeroes, 1);     /* Segments Left */
                        sa->auth->alg->update(&ctx, &buf[i+4], 4);          /* 32-bit Reserved */
                        sa->auth->alg->update(&ctx, (void *)&dst6, 16);             /* 1st router */
                        for (j = 24; j < length; j += 16)
                            sa->auth->alg->update(&ctx, &buf[i+j-16], 16);  /* N router */
                    }
                    break;
                case IP_IPPROTO_HOPOPTS:  /* options */
                case IP_IPPROTO_DSTOPTS:  /* options */
                    if (i + length == iph_length)
                        sa->auth->alg->update(&ctx, &param->key.proto, 1);  /* Last extension, use next == AH */
                    else
                        sa->auth->alg->update(&ctx, &buf[i], 1);        /* Next extension */
                    sa->auth->alg->update(&ctx, &buf[i+1], length - 1);  /* extension excluding next header */
                    break;
                default:
                    IP_PANIC2();
                    IPCOM_LOG1(WARNING, "<discard> :: illegal IPv6 extension (%d)", nxt_hdr);
                    return -IP_ERRNO_EFAULT;
                }
                nxt_hdr = buf[i];
            }
        }
    }
#endif /* IPCOM_USE_INET6 */


 ah:
    /*** AH header ***/
    sa->auth->alg->update(&ctx, &pkt->data[pkt->start], sa->header_size);
    sa->auth->alg->update(&ctx, ipipsec.zeroes, sa->auth->alg->auth_bytes);

    /*** Payload ***/
    sa->auth->alg->update(&ctx,
                           &pkt->data[pkt->start + sa->header_size + sa->auth->alg->auth_bytes],
                           pkt->end - pkt->start - sa->header_size - sa->auth->alg->auth_bytes);
#ifdef IPIPSEC_USE_ESN
    if (IP_BIT_ISSET(sa->sadb_sa_flags, SADB_X_SAFLAGS_ESN))
        sa->auth->alg->update(&ctx, (Ip_u8 *)&seqh, 4);
#else
    (void)seqh;
#endif


    /*** OLD AH (Keyed MD5/SHA) ***/
    if (sa->sadb_sa_auth == SADB_X_AALG_MD5 || sa->sadb_sa_auth == SADB_X_AALG_SHA1)
    {
        sa->auth->alg->update(&ctx, sa->auth->key, sa->auth->keyLen);
    }

    /*** AES MAC ***/
    else if (sa->sadb_sa_auth == SADB_X_AALG_AESXCBCMAC96)
    {
        /* Nothing to do */
    }

    /*** NEW AH ***/
    else
    {
        sa->auth->alg->final(calc_digest, &ctx);

        /**** outer HMAC ****/
        ipcom_memcpy(&ctx, &sa->auth->o_ctx, sizeof(Ipipsec_auth_ctx));
        sa->auth->alg->update(&ctx, calc_digest, sa->auth->alg->hash_bytes);
    }

    sa->auth->alg->final(calc_digest, &ctx);

    return 0;
}
#endif /* IPIPSEC_USE_AH */


/*
 *===========================================================================
 *                    ipipsec_check_replay_window
 *===========================================================================
 * Description:   Check replay window. Based on public code from RFC2401.
 * Parameters:
 * Returns:       0 on success else -1
 */
IP_GLOBAL int
ipipsec_check_replay_window(Ipipsec_sa *sa, Ip_u32 seq)
{
    int   diff;

    ip_assert(sa->sadb_sa_replay <= 32);

    /* first == 0 or wrapped */
    if (seq == 0)
    {
        IPSEC_STATS(ipipsec.stats.sa_inreplay_zeroseq++);
        return -1;
    }

    /* New larger sequence number */
    if (seq > sa->replay_counter)
    {
        IPSEC_STATS(ipipsec.stats.sa_inreplay_inorder++);
        return 0;    /* larger is good */
    }

    /* lower sequence number */
    else
    {
        diff = sa->replay_counter - seq;
        if (diff >= sa->sadb_sa_replay)
        {
            IPSEC_STATS(ipipsec.stats.sa_inreplay_wrap++);
            return -1;    /* too old or wrapped */
        }

        if (sa->replay_bitmap & (1UL << diff))
        {
            IPSEC_STATS(ipipsec.stats.sa_inreplay_seen++);
            return -1;    /* already seen */
        }

        IPSEC_STATS(ipipsec.stats.sa_inreplay_outoforder++);
        return 0;        /* out of order but good */
    }
}


/*
 *===========================================================================
 *                    ipipsec_update_replay_window
 *===========================================================================
 * Description:   Update replay window. Based on public code from RFC2401.
 * Parameters:
 * Returns:
 */
IP_GLOBAL Ip_err
ipipsec_update_replay_window(Ipipsec_sa *sa, Ip_u32 seq)
{
    Ip_u32   diff;

    ip_assert(ipipsec_check_replay_window(sa, seq) == 0);

#if defined(IPHWCRYPTO) || defined(IPIPSEC_USE_DAEMONS)
    /* Only needed for asyn or multi-core */
    if (ipipsec_check_replay_window(sa, seq) != 0)
        return IPCOM_ERR_FAILED;
#endif

#ifdef IPCOM_USE_FORWARDER_IPSEC
    {
        int ret;

        /* Check and update replay window in forwarder */
        ret = ipcom_forwarder_ipsec_check_replay(sa, seq);
        switch (ret)
        {
        case -1:
            return IPCOM_ERR_FAILED;
        case 0:
            return IPCOM_SUCCESS;
        default:
            break; /* SA not found */
        }
    }
#endif

    /* New larger sequence number */
    if (seq > sa->replay_counter)
    {
        diff = seq - sa->replay_counter;
        if (diff < sa->sadb_sa_replay)  /* In window */
        {
            sa->replay_bitmap <<= diff;
            sa->replay_bitmap |= 1;    /* set bit for this packet */
        }
        else
            sa->replay_bitmap = 1;       /* This packet has a "way larger" */
        sa->replay_counter = seq;
    }

    /* lower sequence number */
    else
    {
        diff = sa->replay_counter - seq;

        ip_assert(diff < sa->sadb_sa_replay);
        ip_assert((sa->replay_bitmap & (1UL << diff)) == 0);
        sa->replay_bitmap |= (1UL << diff);   /* mark as seen */
    }

    return IPCOM_SUCCESS;
}


#ifdef IPIPSEC_USE_ESN
/*
 *===========================================================================
 *                    ipipsec_check_replay_window_esn
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 */
IP_GLOBAL int
ipipsec_check_replay_window_esn(Ipipsec_sa *sa, Ip_u32 seql, Ip_u32 *diff, Ip_bool *incTh)
{
    Ip_u32 wl = (sa->sadb_sa_replay - 1) & IPIPSEC_MODVAL; /* (W-1)%2^32 */
    Ip_u32 bl = (sa->replay_Tl - wl) & IPIPSEC_MODVAL;
    Ip_u32 bitpos;

    *incTh = IP_FALSE;

    if (sa->replay_Tl >= wl)
    {
        /* Check seql against lower bound */
        if (seql >= bl)
        {
            sa->seqh = sa->replay_Th;
            if (!seql && !sa->seqh)
            {
                /* Wrap around */
                IPSEC_STATS(ipipsec.stats.sa_inreplay_zeroseq++);
                return -1;
            }

            if (seql <= sa->replay_Tl)
            {
                /* Calc diff .. if diff < window return else check
                 * bitmap and reply */
                *diff = (sa->replay_Tl - seql) & IPIPSEC_MODVAL;
                if (*diff > sa->sadb_sa_replay)
                {
                    IPSEC_STATS(ipipsec.stats.sa_inreplay_wrap++);
                    return -1;
                }
#ifdef IPIPSEC_USE_BITARRAY
                bitpos = (*diff) & IPIPSEC_MODVAL;
                if (ipcom_bitarray_isset(sa->replay_bitarray, bitpos))
                {
                    IPSEC_STATS(ipipsec.stats.sa_inreplay_seen++);
                    return -1;
                }
#else
                if (sa->replay_bitmap & (1UL << *diff))
                {
                    IPSEC_STATS(ipipsec.stats.sa_inreplay_seen++);
                    return -1; /* already seen */
                }
#endif

                /* Out of order but still good */
                IPSEC_STATS(ipipsec.stats.sa_inreplay_outoforder++);
                return 0;
            }
            else
            {
                /* OK */
                IPSEC_STATS(ipipsec.stats.sa_inreplay_inorder++);
                *diff = (seql - sa->replay_Tl);
                return 0;
            }

        }
        else
        {
            sa->seqh = (sa->replay_Th + 1) & IPIPSEC_MODVAL ;
            if (!seql && !sa->seqh)
            {
                /* Wrap around */
                IPSEC_STATS(ipipsec.stats.sa_inreplay_zeroseq++);
                return -1;
            }

            /* OK */
            IPSEC_STATS(ipipsec.stats.sa_inreplay_inorder++);
            *diff = seql + (0UL - sa->replay_Tl);
            *incTh = IP_TRUE;
            return 0;
        }
    }
    else
    {
        if (seql >= bl)
        {
            sa->seqh = (sa->replay_Th - 1) & IPIPSEC_MODVAL;
            if (!seql && !sa->seqh)
            {

                /* Wrap around */
                IPSEC_STATS(ipipsec.stats.sa_inreplay_zeroseq++);
                return -1;
            }

            /* Cal diff if diff < window return else check bitmap
               reply */
            *diff = (sa->replay_Tl + (0UL - seql)) & IPIPSEC_MODVAL;
            if (*diff > sa->sadb_sa_replay)
            {
                IPSEC_STATS(ipipsec.stats.sa_inreplay_wrap++);
                return -1;
            }

#ifdef IPIPSEC_USE_BITARRAY
                bitpos=(*diff) & IPIPSEC_MODVAL;
                if (ipcom_bitarray_isset(sa->replay_bitarray,bitpos))
                {
                    /* already seen */
                    IPSEC_STATS(ipipsec.stats.sa_inreplay_seen++);
                    return -1;
                }
#else
            if (sa->replay_bitmap & (1UL << *diff))
            {
                IPSEC_STATS(ipipsec.stats.sa_inreplay_seen++);
                return -1; /* already seen */
            }
#endif

            /* Out of order but still good */
            IPSEC_STATS(ipipsec.stats.sa_inreplay_outoforder++);
            return 0;

        }
        else
        {
            sa->seqh = sa->replay_Th;
            if (seql == 0 && sa->seqh == 0)
            {
                /* Wrap around */
                IPSEC_STATS(ipipsec.stats.sa_inreplay_zeroseq++);
                return -1;
            }

            if (seql <= sa->replay_Tl)
            {
                /* Calc diff .. if diff < window return else check
                   bitmap and reply */
                *diff = (sa->replay_Tl - seql) & IPIPSEC_MODVAL;
                if (*diff > sa->sadb_sa_replay)
                {
                    IPSEC_STATS(ipipsec.stats.sa_inreplay_wrap++);
                    return -1;
                }

#ifdef IPIPSEC_USE_BITARRAY
                bitpos=(*diff) & IPIPSEC_MODVAL;
                if (ipcom_bitarray_isset(sa->replay_bitarray,bitpos))
                {
                    /* already seen */
                    IPSEC_STATS(ipipsec.stats.sa_inreplay_seen++);
                    return -1;
                }
#else
                if (sa->replay_bitmap & (1UL << *diff))
                {
                    return -1; /* already seen */
                    IPSEC_STATS(ipipsec.stats.sa_inreplay_seen++);
                }
#endif

                /* Out of order but still good */
                IPSEC_STATS(ipipsec.stats.sa_inreplay_outoforder++);
                return 0;

            }
            else
            {
                /* OK */
                IPSEC_STATS(ipipsec.stats.sa_inreplay_inorder++);
                *diff = (seql - sa->replay_Tl);
                return 0;
            }
        }
    }
}


/*
 *===========================================================================
 *                    ipipsec_update_replay_window_esn
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 */
IP_GLOBAL Ip_err
ipipsec_update_replay_window_esn(Ipipsec_sa *sa, Ip_u32 seql, Ip_u32 diff, Ip_bool incTh)
{
    /*Ip_u32 wl;*/
    Ip_u32 bitpos = 0;

#if defined(IPHWCRYPTO) || defined(IPIPSEC_USE_DAEMONS)
    /* Only needed for async or multi-core */
    if (ipipsec_check_replay_window_esn(sa, seql, &diff, &incTh) != 0)
        return IPCOM_ERR_FAILED;
#endif

    if (seql > sa->replay_Tl)
    {
        if (diff < sa->sadb_sa_replay)
        {
#ifdef IPIPSEC_USE_BITARRAY
            (void)ipcom_bitarray_shiftR(sa->replay_bitarray,diff);
            (void)ipcom_bitarray_set(sa->replay_bitarray,0,0);
#else
            sa->replay_bitmap <<= diff;
            sa->replay_bitmap |= 1;
#endif
        }
        else
        {
#ifdef IPIPSEC_USE_BITARRAY
            (void)ipcom_bitarray_reset(sa->replay_bitarray);
            (void)ipcom_bitarray_set(sa->replay_bitarray,0,0);
#else
            sa->replay_bitmap = 1;
#endif
        }

    }
    else
    {
        ip_assert(diff < sa->sadb_sa_replay);

#ifdef IPIPSEC_USE_BITARRAY
        /*wl = (sa->sadb_sa_replay - 1) & IPIPSEC_MODVAL;  (W-1)%2^32 */
        bitpos = (diff) & IPIPSEC_MODVAL;
        ip_assert(!ipcom_bitarray_isset(sa->replay_bitarray,bitpos));

        (void)ipcom_bitarray_set(sa->replay_bitarray,bitpos,0);

#else
        ip_assert((sa->replay_bitmap & (1UL << diff)) == 0);
        sa->replay_bitmap |= (1UL << diff);   /* mark as seen */
#endif

    }

    if (incTh)
    {
        sa->replay_Th = ((sa->replay_Th+1) & IPIPSEC_MODVAL);
        sa->replay_Tl = seql;
    }
    else
    {
        if (seql > sa->replay_Tl)
            sa->replay_Tl = seql;
    }

    IPCOM_LOG3(DEBUG2,"          Updating replay window (replay=%d, seql=%lu, seqh=%lu)",
               (int)sa->sadb_sa_replay,
               sa->replay_Tl,
               sa->replay_Th);
    sa->replay_failures = 0;
    return IPCOM_SUCCESS;
}
#endif /* IPIPSEC_USE_ESN */


/*
 *===========================================================================
 *                    ipipsec_calc_digest
 *===========================================================================
 * Description: encapsulate common digest calculation code
 * Parameters:
 * Returns:
 */
IP_GLOBAL void
ipipsec_calc_digest(Ipipsec_sa *sa, Ipcom_pkt *ipsec_pkt, Ip_u32 esn, Ip_u8 *calc_digest)
{
    Ipipsec_auth_ctx   ctx;

    ip_assert(sa->sadb_sa_auth != SADB_AALG_NONE);
    ip_assert(sa->auth->alg->update);
    ip_assert(sa->auth != IP_NULL);

    /* inner HMAC (includes ESP header+payload. excludes auth field) */
    ipcom_memcpy(&ctx, &sa->auth->i_ctx, sizeof(Ipipsec_auth_ctx));
    sa->auth->alg->update(&ctx, &ipsec_pkt->data[ipsec_pkt->start],
                           ipsec_pkt->end - ipsec_pkt->start);

#ifdef IPIPSEC_USE_ESN
    if (IP_BIT_ISSET(sa->sadb_sa_flags, SADB_X_SAFLAGS_ESN))
        sa->auth->alg->update(&ctx, (Ip_u8 *)&esn, 4);
#else
    (void)esn;
#endif

    sa->auth->alg->final(calc_digest, &ctx);

    if (sa->sadb_sa_auth != SADB_X_AALG_AESXCBCMAC96)
    {
        /* outer HMAC */
        ipcom_memcpy(&ctx, &sa->auth->o_ctx, sizeof(Ipipsec_auth_ctx));
        sa->auth->alg->update(&ctx, calc_digest, sa->auth->alg->hash_bytes);
        sa->auth->alg->final(calc_digest, &ctx);
    }
}


/*
 *===========================================================================
 *                    ipipsec_lifetime_bytes
 *===========================================================================
 * Description:
 * Parameters:   0 for input packets, 1 for output packets.
 * Returns:
 *
 */
IP_GLOBAL int
ipipsec_lifetime_bytes(Ipipsec_sa *sa, Ipcom_pkt *pkt, Ip_u32 dir, Ip_u32 bytes)
{
    (void)pkt;

    ip_assert(dir == IPIPSEC_DIR_IN || dir == IPIPSEC_DIR_OUT);

    IPIPSEC_DATA_LOCK(ipipsec.spl);

    IP_U64_ADD32(sa->bytes[dir], bytes);         /* in/out bytes */
    IP_U64_ADD32(sa->packets[dir], 1);
    IP_U64_ADD32(sa->ltcurrent.bytes, bytes);    /* total bytes */

    /* Check hard byte expiration */
    if (!IP_U64_ISZERO(sa->lthard.bytes)
        && IP_U64_HIGHER(sa->ltcurrent.bytes, sa->lthard.bytes))
    {
        IPIPSEC_DATA_UNLOCK(ipipsec.spl);

        IPCOM_LOG2(INFO, "<%s discard> :: hard byte expiration  (SA: %s)",
                      dir == IPIPSEC_DIR_IN ? "input" : "output",
                      ipipsec_sa_name(sa));

        /* remove this sa */
#ifdef IPIPSEC_USE_DAEMONS
        ipipsec_send_control_signal(IPIPSEC_SIG_HARDEXPIRE, sa);
#else
        IPCOM_LOG2(INFO, "<%s discard> :: hard byte expiration  (SA: %s)",
                      dir == IPIPSEC_DIR_IN ? "input" : "output",
                      ipipsec_sa_name(sa));
        ipipsec_pfkeyv2_expire(sa, IPIPSEC_EXPIRE_TYPE_HARD);
        ipipsec_sa_free(sa, IP_FALSE, IP_TRUE);
#endif /* IPIPSEC_USE_DAEMONS */
        return -1;
    }

    /* Check soft byte expiration */
    if (!IP_U64_ISZERO(sa->ltsoft.bytes)
        && IP_U64_HIGHER(sa->ltcurrent.bytes, sa->ltsoft.bytes))
    {
        IPIPSEC_DATA_UNLOCK(ipipsec.spl);

        IPCOM_LOG2(INFO, "<%s notice> :: soft byte expiration  (SA: %s)",
                      dir == IPIPSEC_DIR_IN ? "input" : "output",
                      ipipsec_sa_name(sa));

        /* notice subscribers */
#ifdef IPIPSEC_USE_DAEMONS
        ipipsec_send_control_signal(IPIPSEC_SIG_SOFTEXPIRE, sa);
#else
        ipipsec_pfkeyv2_expire(sa, IPIPSEC_EXPIRE_TYPE_SOFT);
#endif
        return 0;
    }

    IPIPSEC_DATA_UNLOCK(ipipsec.spl);
    return 0;
}


/*
 *===========================================================================
 *                    ipipsec_send_trap
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
#ifdef IPIPSEC_USE_TRAPS
IP_STATIC void
ipipsec_send_trap(Ipipsec_sa *sa, Ipipsec_param *param, Ip_u32 traptype, Ip_u32 errors)
{
    Ipipsec_sa_handle  sah;
    Ip_u32             now;

#ifdef IPIPSEC_USE_DAEMONS
    if (ipnet_is_stack_task() == IP_FALSE)
    {
        Ipnet_sig  *sig;

        /* Send the job to the control pipe */
        sig = ipcom_malloc(sizeof(*sig));
        if (sig == IP_NULL)
        {
            IPCOM_LOG2(ERR, "ipipsec_send_trap(%d) :: Memory allocation failed. SA: %s",
                       traptype, ipipsec_sa_name(sa));
            return;
        }
        ipnet_usr_sock_init_sig(sig, IPNET_SIG_IPSEC);
        sig->d.ipsec.ipsec_sig_type  = IPIPSEC_SIG_TRAP;
        sig->d.ipsec.t.trap.sa       = sa;
        sig->d.ipsec.t.trap.traptype = traptype;
        sig->d.ipsec.t.trap.errors   = errors;
        ipcom_memcpy(&sig->d.ipsec.t.trap.param, param, sizeof(Ipipsec_param));
        if (ipcom_pipe_try_send(ipipsec.ctrlpipe, sig) == IP_FALSE)
        {
            IPCOM_LOG2(INFO, "ipipsec_send_trap(%d) :: Failed sending to control pipe. SA: %s",
                       traptype, ipipsec_sa_name(sa));
            ipcom_free(sig);
            return;
        }

        /* Notify IPNET */
        ipipsec_send_notify_signal();
        return;
    }
#endif /* #ifdef IPIPSEC_USE_DAEMONS */

    ip_assert(traptype < IPIPSEC_TRAP_NUM);
    ip_assert(ipnet_is_stack_task());

    /* no traps */
    if (ipipsec.trap_cb == IP_NULL)
        return;

    /* disabled trap type */
    if (ipipsec.enable_trap[traptype] == 0)
        return;

    /* Do not send a trap per packet */
    now = ipipsec_seconds_now();
    if (now - sa->sent_trap[traptype]  < IPIPSEC_MIN_SEND_TRAP_SECONDS)
        return;

    /* Remember time we sent a trap of this type */
    sa->sent_trap[traptype] = now;

    /* Send (i.e. report via callback) trap */
    ipcom_memcpy(&sah.dst, &sa->key.dst, sa->in_addr_size);
    sah.spi_n     = sa->key.ports.espah_spi_n;
    sah.protocol  = sa->key.proto;
    sah.direction = sa->key.direction;
    sah.domain    = sa->key.domain;
    sah.satype    = sa->sadb_msg_satype;

    switch(traptype)
    {
    case IPIPSEC_TRAPTYPE_invalidSpiTrap:
    case IPIPSEC_TRAPTYPE_otherPolicyFailureTrap:
        ip_assert(param);
        ipipsec.trap_tags[0] = IPIPSEC_TRABCBT_LOCAL_ADDR;
        ipipsec.trap_tags[1] = (Ip_tag)&param->key.src;
        ipipsec.trap_tags[2] = IPIPSEC_TRABCBT_SECURITY_PROTOCOL;
        ipipsec.trap_tags[3] = (Ip_tag)param->key.proto;
        ipipsec.trap_tags[4] = IPIPSEC_TRABCBT_PEER_ADDR;
        ipipsec.trap_tags[5] = (Ip_tag)&param->key.dst;
        ipipsec.trap_tags[6] = IPIPSEC_TRABCBT_SPI;
        ipipsec.trap_tags[7] = errors;
        ipipsec.trap_tags[8] = IPIPSEC_TRABCBT_DOMAIN;
        ipipsec.trap_tags[9] = param->key.domain;
        ipipsec.trap_tags[10] = IP_TAG_END;
        break;

    default:
        ipipsec.trap_tags[0] = IPIPSEC_TRABCBT_NUM_ERRORS;
        ipipsec.trap_tags[1] = errors;
        ipipsec.trap_tags[2] = IP_TAG_END;
        break;
    }

    /* Call the trap function */
    (void)ipipsec.trap_cb(&sah, traptype, ipipsec.trap_tags);
}

#else
#define ipipsec_send_trap(a,b,c,d)  IP_NOOP
#endif /* IPIPSEC_USE_TRAPS */


/*
 ****************************************************************************
 * 11                    PUBLIC FUNCTIONS
 ****************************************************************************
 */


/*
 *===========================================================================
 *                    ipipsec_ioevent_sock_key_close
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 */
IP_PUBLIC int
ipipsec_ioevent_sock_key_close(Ipcom_socket *sock)
{
    if (sock->ipsec_registered != 0)
    {
        ip_assert(ipipsec.registered_num > 0);
        ipipsec.registered_num--;
    }
    return 0;
}


/*
 *===========================================================================
 *                    ipipsec_output
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:   IPIPSEC_OK
 *            IPIPSEC_TUNNELED
 *            IPIPSEC_KEPT
 *            negative errno
 */
IP_PUBLIC int
ipipsec_output(Ipcom_pkt **pktp, Ipipsec_param *param)
{
    Ipcom_pkt      *pkt = *pktp;
    Ipipsec_flow   *flow;
    Ipipsec_sa     *sa;
    Ipcom_pkt      *ipsec_pkt = IP_NULL;
    int tot_space, tot_header_space, tot_trailer_space;
    int retval;
#ifdef IPCOM_USE_INET6
    Ip_u32  dstaddr[4];       /* 128-bit address */
#else
    Ip_u32  dstaddr[1];
#endif
    Ip_u8   domain;
#ifdef IPIPSEC_USE_ESN
    Ip_u32  seqh = 0;
#endif
    Ip_u32  seql = 0;
#if defined(IPIPSEC_USE_ESP) && defined(IPIPSEC_USE_TFC)
    Ip_u32  tfc_padding = 0;
    Ipipsec_sa    *tfc_sa = IP_NULL;
#endif

#ifndef IPIPSEC_USE_DAEMONS
    retval = ipipsec_flow_lookup_output(param, pkt);
    if (retval == 0)
        goto cleartext;
    else if (retval < 0)
        goto errout;
#endif /* IPIPSEC_USE_DAEMONS */

    ip_assert(param->iph_length >= 20);
    ip_assert(param->iph_length <= 65536);

    /* Init param */
    param->udp.src_port = 0;

    /* Apply security */
    retval = IPIPSEC_OK;
    flow = param->output_flow;
    ip_assert(flow);
    ip_assert(flow->sa);
    ip_assert(flow->sa != &ipipsec.sa_bypass);


    /****** Calculate total header space and check configuration */
    tot_header_space  = 0;
    tot_trailer_space = 0;
    domain = param->key.domain;
#ifdef IPCOM_USE_INET
    if (param->key.domain == IP_AF_INET)
        dstaddr[0] = param->key.dst.in.s_addr;
#endif
#ifdef IPCOM_USE_INET6
    if (param->key.domain == IP_AF_INET6)
        ipcom_memcpy(dstaddr, &param->key.dst, sizeof(struct Ip_in6_addr));
#endif

    for (sa = flow->sa; sa; sa = sa->group_next)
    {
        /* Initial SA checks */
        if (sa->key.ports.espah_spi_n == 0
           || sa->sadb_sa_state != SADB_SASTATE_MATURE)
        {
            IPCOM_LOG1(INFO, "<output discard> :: imature SA  (SA: %s)", ipipsec_sa_name(sa));
            IPIPSEC_ERROUT(-IP_ERRNO_EHOSTUNREACH);
        }
#ifdef IPIPSEC_USE_ESP
        if (sa->sadb_msg_satype == SADB_SATYPE_ESP
           && ipipsec_conf_esp_enable != 1)
        {
            IPCOM_LOG1(NOTICE, "<output discard> :: ESP not globally enabled (pkt '%s')",
                          ipipsec_key_name(pkt, param));
            IPIPSEC_ERROUT(-IP_ERRNO_EHOSTUNREACH);
        }
#endif
#ifdef IPIPSEC_USE_AH
        if (sa->sadb_msg_satype == SADB_SATYPE_AH
           && ipipsec_conf_ah_enable != 1)
        {
            IPCOM_LOG1(NOTICE, "<output discard> :: AH not globally enabled (pkt '%s')",
                          ipipsec_key_name(pkt, param));
            IPIPSEC_ERROUT(-IP_ERRNO_EHOSTUNREACH);
        }
#endif
#ifdef IPIPSEC_USE_IPIP
        if (sa->sadb_msg_satype == SADB_X_SATYPE_IPIP
           && ipipsec_conf_ipip_allow != 1)
        {
            IPCOM_LOG1(NOTICE, "<output discard> :: IPIP not allowed (pkt '%s')",
                          ipipsec_key_name(pkt, param));
            IPIPSEC_ERROUT(-IP_ERRNO_EHOSTUNREACH);
        }
#endif

#ifdef IPIPSEC_USE_TFC
        if (sa->sadb_msg_satype == SADB_SATYPE_ESP
            && IP_BIT_ISSET(sa->sadb_sa_flags, SADB_X_SAFLAGS_TFC))
        {
            if (param->key.proto == IP_IPPROTO_UDP
                || param->key.proto == IP_IPPROTO_IPV4
                || param->key.proto == IP_IPPROTO_IPV6)
            {
                tfc_padding = 1; /* Place holder value that will be replaced */
                tfc_sa = sa;  /* Cache sa pointer -> only last SA will be padded */
            }
        }
#endif /* IPIPSEC_USE_TFC */


        /* Add this SA to total header and trailer space */
        tot_header_space  += sa->header_space;
        tot_trailer_space += sa->trailer_space;

        /* Tunneling mode required */
        if (IP_BIT_ISSET(sa->sadb_sa_flags, SADB_X_SAFLAGS_TUNNEL | SADB_X_SAFLAGS_UDPENCAP)
           || sa->key.domain != domain
           || ipcom_memcmp(&sa->key.dst, dstaddr, sa->in_addr_size)) /*lint !e645 */
        {
            if (ipcom_memcmp(ipipsec.zeroes, &sa->key.dst, sa->in_addr_size) == 0)
            {
                IPCOM_LOG2(NOTICE, "<output discard> :: unspecified tunnel endpoint address (pkt '%s') (SA: %s)",
                              ipipsec_key_name(pkt, param),
                              ipipsec_sa_name(sa));
                IPIPSEC_ERROUT(-IP_ERRNO_EHOSTUNREACH);
            }

            /* Note: IPv4 options are never copied from the inner to the outer header */
            tot_header_space += (sa->key.domain == IP_AF_INET ? IPCOM_IP_HEADER_LENGTH : IPCOM_IP6_HEADER_LENGTH);
            if (IP_BIT_ISSET(sa->sadb_sa_flags, SADB_X_SAFLAGS_UDPENCAP))
                tot_header_space += IPCOM_UDP_HEADER_LENGTH;

            /* Update temporary dstaddr & domain for nested tunnels */
            ipcom_memcpy(dstaddr, &sa->key.dst, sa->in_addr_size);
            domain = sa->key.domain;

#ifdef IPIPSEC_USE_TFC
            /* tunnel-mode, safe to TFC pad */
            if (sa->sadb_msg_satype == SADB_SATYPE_ESP
                && IP_BIT_ISSET(sa->sadb_sa_flags, SADB_X_SAFLAGS_TFC))
            {
                tfc_padding = 1;
                tfc_sa = sa;  /* Cache sa pointer -> only last SA will be padded */
            }
#endif
        }
    }

    /* Calculate new MTU */
    tot_space = tot_header_space + pkt->end + tot_trailer_space;

    /* Check if IP datagram is too big, > 65535 (65365 seems to be max for ipnet/ipv4/udp here) */
    if (tot_space > 65535)
    {
        IPCOM_LOG3(ERR, "<output discard> :: packet '%s' too large (requires %d bytes whereof %d for ipsec)",
                   ipipsec_key_name(pkt, param),
                   tot_space, tot_header_space + tot_trailer_space);
        IPIPSEC_ERROUT(-IP_ERRNO_EMSGSIZE);
    }

    /* Add space for TFC */
#if defined(IPIPSEC_USE_ESP) && defined(IPIPSEC_USE_TFC)
    if (tfc_padding)
    {
        tfc_padding  = 1 + (Ip_u32)ipcom_random();
        tfc_padding %= (Ip_u32)ipipsec_max_tfc_bytes;
        tfc_padding  = IP_MIN((int)tfc_padding, 65535 - tot_space); /* do not break max! */
        ip_assert(tfc_padding <= (Ip_u32)ipipsec_max_tfc_bytes);
        tot_space   += (int)tfc_padding;
    }
#endif

    /* Allocate new packet to fit ESP */
    ipsec_pkt = ipcom_pkt_malloc(tot_space, 0);
    if (ipsec_pkt == IP_NULL)
    {
        IPCOM_LOG3(ERR, "<output discard> :: out of packet memory for pkt '%s' (requires %d bytes whereof %d for ipsec)",
                   ipipsec_key_name(pkt, param),
                   tot_space, tot_header_space + tot_trailer_space);
        IPIPSEC_ERROUT(-IP_ERRNO_ENOBUFS);
    }

    /* Set ipsec_pkt PKT header */
    IPCOM_PKT_TRACE(ipsec_pkt, IPCOM_GET_PKT_ID(pkt));
    ipsec_pkt->fd            = pkt->fd;
    ipsec_pkt->ifindex       = pkt->ifindex;
    ipsec_pkt->start         = pkt->start + tot_header_space + param->iph_length;
    ipsec_pkt->next_original = pkt;
    ipsec_pkt->ipsec_seq     = pkt->ipsec_seq;    /* Copy the sequence tag */
    IPCOM_PKT_TRACE(pkt, IPCOM_PKT_ID_IPSEC_OUTPUT_ORG);
    IPCOM_PKT_TRACE(ipsec_pkt, IPCOM_PKT_ID_IPSEC_OUTPUT);

    /* Copy extra data */
    ipcom_pkt_copy_info(ipsec_pkt, pkt);

    /* Copy over payload */
    ipcom_memcpy(&ipsec_pkt->data[ipsec_pkt->start],
                 &pkt->data[pkt->start + param->iph_length],
                 pkt->end - pkt->start - param->iph_length);
    ipsec_pkt->end = ipsec_pkt->start + pkt->end - pkt->start - param->iph_length;

    ipsec_pkt->ipsec_hlen = 0;
#ifdef IPIPSEC_USE_PMTU_DISCOVERY
    if (IP_BIT_ISSET(pkt->flags, IPCOM_PKT_FLAG_FORWARDED))
    {
        /* Mark packet with required header and trailer space for path mtu detection.
         * Only relevant for forwarded packets. Additionally, the original packet
         * headers must have fit in the buffer when saved above or the original packet
         * can not be restored properly.
         */
        ipsec_pkt->ipsec_hlen = (Ip_u16)(tot_header_space + tot_trailer_space);
    }
#endif /* IPIPSEC_USE_PMTU_DISCOVERY */


    /* Apply all SA's grouped together to the outgoing packet */
    for (sa = flow->sa; sa; sa = sa->group_next)
    {
        /* Register first use and reset timeout */
        if (sa->ltcurrent.usetime == 0)
        {
#ifdef IPIPSEC_USE_DAEMONS
            ipipsec_send_control_signal(IPIPSEC_SIG_REGISTERUSE, sa);
#else
            sa->ltcurrent.usetime = ipipsec_seconds_now();
            ipipsec_sa_reset_timeout(sa);
#endif
        }

        /* Apply tunneling */
        if (IP_BIT_ISSET(sa->sadb_sa_flags, SADB_X_SAFLAGS_TUNNEL | SADB_X_SAFLAGS_UDPENCAP)
           || sa->key.domain != param->key.domain
           || ipcom_memcmp(&sa->key.dst, &param->key.dst, sa->in_addr_size))
        {
            IPCOM_LOG3(DEBUG2, "          Tunneling packet '%s' in '%s -> %s'",
                       ipipsec_key_name(ipsec_pkt, param),
                       ipcom_inet_ntop(sa->key.domain, &sa->key.src,
                                       ipipsec.sysbuf, IP_INET6_ADDRSTRLEN),
                       ipcom_inet_ntop(sa->key.domain, &sa->key.dst,
                                       &ipipsec.sysbuf[IP_INET6_ADDRSTRLEN], IP_INET6_ADDRSTRLEN));

            /* Tunnel address check on forwarded packets only */
            if (IP_BIT_ISSET(pkt->flags, IPCOM_PKT_FLAG_FORWARDED)
               && param->key.domain == sa->key.domain)
            {
                Ipipsec_eio_addr  eio_addr;

                ipcom_memcpy(eio_addr.addr, &param->key.src, sa->in_addr_size);
                eio_addr.domain = param->key.domain;

                if (ipcom_memcmp(&sa->key.dst, &param->key.src, sa->in_addr_size) == 0)
                {
                    /* Source address match tunnel destination */
                    IPCOM_LOG2(NOTICE, "<output discard> :: source == tunnel destination  (src: %s) (SA: %s)",
                                  ipcom_inet_ntop(sa->key.domain, &param->key.src, ipipsec.sysbuf, sizeof(ipipsec.sysbuf)),
                                  ipipsec_sa_name(sa));
                    IPIPSEC_ERROUT(-IP_ERRNO_ENXIO);
                }
            }

            /* Add inner IP header */
            ipipsec_output_add_ip_header(ipsec_pkt, param);

            /* Update 'param' */
            ipcom_memcpy(&param->key.src, &sa->key.src, sa->in_addr_size);
            ipcom_memcpy(&param->key.dst, &sa->key.dst, sa->in_addr_size);
            param->key.domain = sa->key.domain;
            if (sa->key.domain == IP_AF_INET)
                param->key.proto = IP_IPPROTO_IPIP;
            else
                param->key.proto = IP_IPPROTO_IPV6;

            /* Create a new outer IP header (but do not add it yet) */
            ipipsec_output_create_ip_header(sa, param);

            /* Packet is tunneled (at least once) */
            retval = IPIPSEC_TUNNELED;
        }

        IPIPSEC_DATA_LOCK(ipipsec.spl);
#ifdef IPCOM_USE_FORWARDER_IPSEC
        /* Get latest sequence number from forwarder */
        sa->seql = ipcom_forwarder_ipsec_get_seq(sa);
#endif
        /* Replay counter wrap-around check */
        if (sa->sadb_sa_replay > 0
            && sa->seql == 0
#ifdef IPIPSEC_USE_ESN
            && (IP_BIT_ISFALSE(sa->sadb_sa_flags, SADB_X_SAFLAGS_ESN) || sa->seqh == 0)
#endif
            )
        {
            IPIPSEC_DATA_UNLOCK(ipipsec.spl);
            IPSEC_MIB(sa->err_out_send++);
            IPCOM_LOG2(NOTICE, "<output discard> :: seq=0 -> replay wrap (%s -> SA: %s)",
                       ipcom_inet_ntop(param->key.domain, &param->key.dst, ipipsec.sysbuf, sizeof(ipipsec.sysbuf)),
                       ipipsec_sa_name(sa));
            IPIPSEC_ERROUT(-IP_ERRNO_ENXIO);
        }

        if (IP_BIT_ISFALSE(sa->sadb_sa_flags, SADB_X_SAFLAGS_NOREPLAY))
        {
            /* Cache low (and high) sequence numbers in local vars */
            seql = sa->seql++;
#ifdef IPIPSEC_USE_ESN
            seqh = sa->seqh;
            if (sa->seql == 0)
                sa->seqh++;
#endif
        }
        IPIPSEC_DATA_UNLOCK(ipipsec.spl);

        /*** ESP/AH output processing ***/
        switch(sa->sadb_msg_satype)
        {
#ifdef IPIPSEC_USE_AH
        case SADB_SATYPE_AH:
            {
                int  ah_ret;
                Ip_u8 calc_digest[IPIPSEC_AUTH_MAXKEYLEN];

                ip_assert((sa->header_space % 4) == 0);
                ip_assert(sa->auth != IP_NULL);
                /*IPSECSTATS(ipipsec.stats.func_ah_output++);*/

                /*** Add AH header before payload ***/
                ipsec_pkt->start -= sa->header_space;
                ipsec_pkt->data[ipsec_pkt->start]   = (Ip_u8) param->key.proto;
                ipsec_pkt->data[ipsec_pkt->start+1] = (Ip_u8)((sa->header_space >> 2) - 2);
                ipsec_pkt->data[ipsec_pkt->start+2] = 0;   /* reserved, must be 0 */
                ipsec_pkt->data[ipsec_pkt->start+3] = 0;   /* reserved, must be 0 */
                ipcom_memcpy(&ipsec_pkt->data[ipsec_pkt->start+4], &sa->key.ports.espah_spi_n, sizeof(Ip_u32));  /* copy in SPI */
                if (IP_BIT_ISFALSE(sa->sadb_sa_flags, SADB_X_SAFLAGS_NOREPLAY))
                {
                    Ip_u32  replay_n;

                    /* copy in (low) SEQ */
                    replay_n = ip_htonl(seql);
                    ipcom_memcpy(&ipsec_pkt->data[ipsec_pkt->start+8], &replay_n, sizeof(Ip_u32));

                }

                /***** Update byte counters and check expiration *****/
                if (ipipsec_lifetime_bytes(sa, ipsec_pkt, IPIPSEC_DIR_OUT, (ipsec_pkt->end - ipsec_pkt->start) + param->iph_length))
                {
                    IPSEC_MIB(sa->err_out_send++);
                    IPIPSEC_ERROUT(-IP_ERRNO_ENXIO);
                }

                /* Calculate and insert digest */
                param->key.proto = IP_IPPROTO_AH;  /* next protocol */
                ah_ret = ipipsec_ah_digest(sa, ipsec_pkt, param, seqh, calc_digest);
                if (ah_ret < 0)
                {
                    IPCOM_LOG2(NOTICE, "<output discard> :: AH digest calculation error (%s -> SA: %s)",
                               ipcom_inet_ntop(param->key.domain, &param->key.dst, ipipsec.sysbuf, sizeof(ipipsec.sysbuf)),
                               ipipsec_sa_name(sa));
                    IPIPSEC_ERROUT(ah_ret);
                }


                ipcom_memcpy(&ipsec_pkt->data[ipsec_pkt->start + sa->header_size],
                             calc_digest,
                             sa->auth->alg->auth_bytes);
            }
            break;
#endif

#ifdef IPIPSEC_USE_ESP
        case SADB_SATYPE_ESP:
            {
                int      i;
                int      length;
                int      pad_bytes, offset;
                Ip_u8    iv[IPIPSEC_MAX_IV_SIZE];
                Ip_u8    calc_digest[IPIPSEC_AUTH_MAXKEYLEN];

#ifdef IPIPSEC_USE_TFC
                if (tfc_sa == sa)
                {
                    ip_assert(IP_BIT_ISSET(sa->sadb_sa_flags, SADB_X_SAFLAGS_TFC));

                    /* Add tfc_padding number of random bytes to payload */
                    ipipsec_crypt_random_fill(&ipsec_pkt->data[ipsec_pkt->end], (int)tfc_padding);
                    ipsec_pkt->end += (int)tfc_padding;
                    IPCOM_LOG2(DEBUG2,
                               "          ESP  adding %d bytes TFC padding, packet size=%d",
                               (int)tfc_padding, tot_space);
                    tfc_padding = 0; /* but only on the first matching SA! */
                }
#endif /* IPIPSEC_USE_TFC */

                length = ipsec_pkt->end - ipsec_pkt->start;

                /*IPSECSTATS(ipipsec.stats.func_esp_output++);*/


                /*** Calculate number of ESP trailer pad_bytes ***/
                if (sa->encrypt)
                    pad_bytes = IP_ROUNDUP((length+2),sa->encrypt->alg->block_bytes) - (length+2);
                else
                    pad_bytes = IP_ROUNDUP((length+2),4) - (length+2);

                ip_assert(pad_bytes < 256);

                /*** Add padding and ESP header after payload ***/
                if (IP_BIT_ISFALSE(sa->sadb_sa_flags, SADB_X_SAFLAGS_RANDOMPADDING))
                {
                    for (i = 0; i < pad_bytes; i++)
                        ipsec_pkt->data[ipsec_pkt->end + i] = (Ip_u8)i + 1;
                }
                ipsec_pkt->end += pad_bytes;

                ipsec_pkt->data[ipsec_pkt->end++] = (Ip_u8) pad_bytes;
                ipsec_pkt->data[ipsec_pkt->end++] = (Ip_u8) param->key.proto;
                param->key.proto = IP_IPPROTO_ESP;

                /*** Add ESP header before payload ***/
                ipsec_pkt->start -= sa->header_space;
                ipcom_memcpy(&ipsec_pkt->data[ipsec_pkt->start], &sa->key.ports.espah_spi_n, sizeof(Ip_u32));  /* copy in SPI */
                if (IP_BIT_ISFALSE(sa->sadb_sa_flags, SADB_X_SAFLAGS_NOREPLAY))
                {
                    Ip_u32  replay_n;
                    replay_n = ip_htonl(seql);
                    ipcom_memcpy(&ipsec_pkt->data[ipsec_pkt->start+4], &replay_n, sizeof(Ip_u32));  /* copy in SEQ */
                }

                if (sa->encrypt)
                {
                    if (sa->encrypt->iv_length > 0)
                    {
                        /* Copy in IV and generate full sized IV */
                        ipcom_memcpy(&ipsec_pkt->data[ipsec_pkt->start + sa->header_size],
                                     sa->encrypt->iv, sa->encrypt->iv_length);
                        ipcom_memcpy(iv, sa->encrypt->iv, sa->encrypt->iv_length);
                        if (IP_BIT_ISSET(sa->sadb_sa_flags, SADB_X_SAFLAGS_HALFIV))
                        {
                            /* generate 2nd half of IV for halfiv ESP */
                            for (offset = 0; offset < sa->encrypt->iv_length; offset++)
                                iv[sa->encrypt->iv_length + offset] = (Ip_u8)~iv[offset];
                        }
                    }
                    /* Encrypt payload + trailing ESP header */
                    ip_assert(((ipsec_pkt->end - ipsec_pkt->start - sa->header_space) % sa->encrypt->alg->block_bytes) == 0);
                }

                /***** Update byte counters and check expiration *****/
                if (ipipsec_lifetime_bytes(sa, ipsec_pkt, IPIPSEC_DIR_OUT, (ipsec_pkt->end - ipsec_pkt->start) - sa->header_space))
                {
                    IPSEC_MIB(sa->err_out_send++);
                    IPIPSEC_ERROUT(-IP_ERRNO_ENXIO);
                }

#ifdef IPHWCRYPTO
                ip_assert(sa->esp_output != IP_NULL);
#ifdef IPIPSEC_USE_ESN
                param->seqh = seqh;
#endif
                i = sa->esp_output(&ipsec_pkt->data[ipsec_pkt->start],
                                    ipsec_pkt->end - ipsec_pkt->start,
                                    sa,
                                    param,
                                    calc_digest,
                                    ipsec_pkt);
                if (i == 2)
                {
                    /* Asynchronous */
                    return IPIPSEC_KEPT;
                }
                else if (i <= 0)
                {
                    IPCOM_LOG0(DEBUG, "<output discard> :: packet dropped due to hardware crypto failure");
                    IPIPSEC_ERROUT(-IP_ERRNO_EINVAL);  /* silent drop */
                }
#else
                if (sa->encrypt)
                {
                    /* Software encrypt */
                    sa->encrypt->alg->encrypt(&ipsec_pkt->data[ipsec_pkt->start + sa->header_space],
                                              ipsec_pkt->end - ipsec_pkt->start - sa->header_space,
                                              &sa->encrypt->key, iv);

                    /* Update IV in SA for next encryption */
                    if (sa->encrypt->iv_length > 0)
                        ipcom_memcpy(sa->encrypt->iv, iv, sa->encrypt->iv_length);
                }
#endif  /* #ifdef IPHWCRYPTO */

                if (sa->auth)
                {
#ifdef IPHWCRYPTO
 #ifdef IPIPSEC_USE_ESN
                    if (sa->sadb_sa_replay > 0)
                        ipipsec_calc_digest(sa, ipsec_pkt, seqh, calc_digest);
 #endif
#else
                    ipipsec_calc_digest(sa, ipsec_pkt, seqh, calc_digest);
#endif
                    /* Copy in the calculated digest */
                    ipcom_memcpy(&ipsec_pkt->data[ipsec_pkt->end], calc_digest, sa->auth->alg->auth_bytes);
                    ipsec_pkt->end += sa->auth->alg->auth_bytes;
                }
            }
            break;
#endif  /* #ifdef IPIPSEC_USE_ESP */

#ifdef IPIPSEC_USE_IPIP
        case SADB_X_SATYPE_IPIP:
            /***** Update byte counters and check expiration *****/
            if (ipipsec_lifetime_bytes(sa, ipsec_pkt, IPIPSEC_DIR_OUT, ipsec_pkt->end - ipsec_pkt->start))
            {
                IPSEC_MIB(sa->err_out_send++);
                IPIPSEC_ERROUT(-IP_ERRNO_ENXIO);
            }
            break;
#endif

        default:
            IPSEC_MIB(sa->err_out_send++);
            IPCOM_LOG1(ERR, "ipipsec_ipsec_output() :: unknown satype (%d)", sa->sadb_msg_satype);
            IP_PANIC();
            IPIPSEC_ERROUT(-IP_ERRNO_EFAULT);
        }

        IPCOM_LOG1(DEBUG2, "          %s", ipipsec_sa_name(sa));
    }

    /* Success -> add outer IP header */
    ipipsec_output_add_ip_header(ipsec_pkt, param);
    IP_BIT_SET(ipsec_pkt->flags, IPCOM_PKT_FLAG_HAS_IP_HDR);  /*!! other packet flags?*/
    IP_BIT_CLR(pkt->flags, IPCOM_PKT_FLAG_HAS_IP_HDR);  /* Must clear this for retransmits etc */

    /* Replace the cleartext packet with the IPSec packet */
    ip_assert(retval == IPIPSEC_OK || retval == IPIPSEC_TUNNELED);
    ip_assert(ipsec_pkt != IP_NULL);
    ip_assert(ipsec_pkt->next_original == pkt);

    *pktp = ipsec_pkt;
    return retval;

    /* Put back IP header */
#ifndef IPIPSEC_USE_DAEMONS
 cleartext:

#ifdef IPIPSEC_USE_TFC
    if (param->key.proto == IP_IPPROTO_NONEXTHDR)
    {
        IPCOM_LOG0(DEBUG, "<output discard> :: dummy packet dropped due to missing flow");
        IPSEC_STATS(ipipsec.stats.sa_dummy_send_errors++);
        IPIPSEC_ERROUT(-IP_ERRNO_EINVAL);  /* silent drop */
    }
#endif

    ip_assert(retval == IPIPSEC_OK);
    ip_assert(ipsec_pkt == IP_NULL);
    ip_assert(IP_BIT_ISSET(pkt->flags, IPCOM_PKT_FLAG_HAS_IP_HDR));
    ip_assert(pkt->start == pkt->ipstart);

    return IPIPSEC_OK;
#endif /* #ifdef IPIPSEC_USE_DAEMONS */

    /* Discard IPSec packet and return the original packet */
 errout:
    ip_assert(retval < 0);
    if (ipsec_pkt != IP_NULL)
    {
        ip_assert(ipsec_pkt->next_original == pkt);
        ipsec_pkt->next_original = IP_NULL;
        ipcom_pkt_free(ipsec_pkt);
    }
    return retval;
}


/*
 *===========================================================================
 *                    ipipsec_input
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:   IPIPSEC_OK
 *            IPIPSEC_TUNNELED
 *            IPIPSEC_KEPT
 *            negative errno
 */
IP_PUBLIC int
ipipsec_input(Ipcom_pkt *pkt, Ipipsec_param *param)
{
    Ipipsec_sa       *sa     = IP_NULL;
    Ip_u32            seql   = 0;
    Ip_u32            seqh   = 0;
    int               ret;
    int               retval = IPIPSEC_OK;
    Ipcom_udp_header *udph   = IP_NULL;
#ifdef IPIPSEC_USE_ESN
    Ip_u32  replay_diff      = 0UL;
    Ip_bool inc_replay_Th    = IP_FALSE;
#endif
#ifdef IPIPSEC_USE_TFC
    int dummy_packet         = 0;  /* Default to 0 -> packet is valid */
#endif

    ip_assert(pkt != IP_NULL);
    ip_assert(pkt->next_original == IP_NULL);
    ip_assert(param != IP_NULL);
    ip_assert(param->key.domain == IP_AF_INET || param->key.domain == IP_AF_INET6);
    ip_assert(param->key.domain != IP_AF_INET || (pkt->start - IPCOM_IP_HEADER_LENGTH) >= pkt->ipstart);
    ip_assert(param->key.domain != IP_AF_INET6 || (pkt->start - IPCOM_IP6_HEADER_LENGTH) >= pkt->ipstart);
    ip_assert((param->key.domain == IP_AF_INET && (pkt->data[pkt->ipstart] & 0xf0) == 0x40)
              || (param->key.domain == IP_AF_INET6 && (pkt->data[pkt->ipstart] & 0xf0) == 0x60));

#ifndef IPIPSEC_ON_LOOPBACK
    if (pkt->ifindex == 1)
        /* Always bypass packets received on standard loopback interface */
        goto okout;
#endif

    if (ipipsec_opened == 0)
    {
        if (ipipsec_conf_enable_outfilter == 1)
        {
            IPCOM_LOG0(DEBUG, "<input discard> :: packet dropped due to IPSec not initialized");
            IPIPSEC_ERROUT(-IP_ERRNO_EINVAL);
        }
        goto okout;
    }

    /* Set params */
    param->key.direction = IPIPSEC_DIR_IN;
    param->iph           = (Ipipsec_ip_header *)&pkt->data[pkt->ipstart];
    param->pkt_org_start = pkt->start;
    param->pkt_org_end   = pkt->end;
    (void)ipipsec_key_update(pkt, param);
    ip_assert(param->iph_length == pkt->start - pkt->ipstart);

    IPCOM_PKT_TRACE(pkt, IPCOM_PKT_ID_IPSEC_INPUT);

    ipipsec_print_iph(pkt, param);


    /********************* ESP/AH/IPIP/x header *********************/
    for (;;)
    {
        /*********** pre-check [ESP/AH/IPIP/x] header ***********/
        switch(param->key.proto)
        {
#ifdef IPIPSEC_USE_IPIP
        case IP_IPPROTO_IPIP:
        case IP_IPPROTO_IPV6:
            /* IPIP is disabled, discard packet */
            if (ipipsec_conf_ipip_allow != 1)
            {
                IPCOM_LOG1(NOTICE, "<input discard> :: IPIP/IPV6 not allowed (pkt '%s')",
                           ipipsec_key_name(pkt, param));
                IPIPSEC_ERROUT(-IP_ERRNO_EOPNOTSUPP);
            }

            if (sa == IP_NULL)
            {
                /* Packet is IPIP-only, verify that we have an ingress
                 * IPIP flow or let packet bypass to be processed by
                 * TCP/IP stacks tunneling interfaces.
                 */
                Ipipsec_flow   *flow;
                flow = ipipsec_flow_lookup(&param->key, IP_NULL,
                                           IPIPSEC_LFLAGS_NOCHECK_PORTS | IPIPSEC_LFLAGS_USEHASH);
                if (flow == IP_NULL)
                    goto transport_packet; /* normal stack processing */
            }

            /* Store the outer header offsets */
            IP_BIT_SET(pkt->flags, IPCOM_PKT_FLAG_TUNNELED);
            ipcom_pkt_set_int(pkt, IPNET_PKT_INFO_TUNNEL_IPSTART, pkt->ipstart, int);

            /* Update param with inner IP header info */
            param->iph   = (Ipipsec_ip_header *)&pkt->data[pkt->start];
            pkt->ipstart = pkt->start;

#ifdef IPCOM_USE_INET
            if ((param->iph->inet.vers_len & 0xf0) == 0x40)
            {
                param->key.domain        = IP_AF_INET;
                param->key.src.in.s_addr = param->iph->inet.src.addr32;
                param->key.dst.in.s_addr = param->iph->inet.dst.addr32;
            }
#endif
#ifdef IPCOM_USE_INET6
            if (IP_IP6_GET_VER(&param->iph->inet6) == 6)
            {
                param->key.domain = IP_AF_INET6;
                ipcom_memcpy(&param->key.src, &param->iph->inet6.src, sizeof(struct Ip_in6_addr));
                ipcom_memcpy(&param->key.dst, &param->iph->inet6.dst, sizeof(struct Ip_in6_addr));
            }
#endif
            ret = ipipsec_key_update(pkt, param);
            if (ret == -1)
            {
                IPCOM_LOG2(DEBUG, "          Discarding bad tunneled packet: %s length=%d",
                           ipipsec_key_name(pkt, param),
                           pkt->end - pkt->ipstart);
                IPIPSEC_ERROUT(-IP_ERRNO_EINVAL);
            }
            param->key.proto = (Ip_u8)ret;
            pkt->start += param->iph_length;

            IPCOM_LOG2(DEBUG2, "          Unpacking tunneled packet: %s length=%d",
                       ipipsec_key_name(pkt, param),
                       pkt->end - pkt->ipstart);

            /* Verify that optional SA proxy address matches the inner source address */
            if (sa != IP_NULL)    /* Note: sa == IP_NULL for first outer IP datagram */
            {
                if (ipcom_memcmp(ipipsec.zeroes, &sa->proxy, sa->in_addr_size)
                   && (sa->key.domain != param->key.domain
                        || ipcom_memcmp(&sa->proxy, &param->key.src, sa->in_addr_size)))
                {
                    IPCOM_LOG3(NOTICE, "<input discard> :: tunneled packet '%s' proxy address mismatch (SA: %s proxy=%s)",
                               ipipsec_key_name(pkt, param),
                               ipipsec_sa_name(sa),
                               ipcom_inet_ntop(sa->key.domain, &sa->proxy, ipipsec.sysbuf, sizeof(ipipsec.sysbuf)));
                    goto policy_error;
                }

                /* Check policy on inner IP packet */
                if (ipipsec_flow_lookup_input(sa, pkt, param) != 0)
                    goto policy_error;
            }

            /* Let IP input parse the inner datagram from the beginning */
            pkt->start = pkt->ipstart;  /* restore pkt->start */
            IPCOM_PKT_RESET_CHECKSUM(pkt);
            retval = IPIPSEC_TUNNELED;
            goto okout;
#endif /* IPIPSEC_USE_IPIP */

#ifdef IPIPSEC_USE_ESP
        case IP_IPPROTO_UDP: /* UDP Encapsulated ESP packet */
            if (sa != IP_NULL)
                goto transport_packet; /* Inner UDP packet, normal processing */

#ifndef IPNET
            ip_assert(pkt->end - pkt->start > IPCOM_UDP_HEADER_LENGTH);
            if ((pkt->end - pkt->start == (IPCOM_UDP_HEADER_LENGTH + 1))
                 && (pkt->data[pkt->start + IPCOM_UDP_HEADER_LENGTH] == 0xff))
            {
                /* NAT-keepalive Packet */
                goto okout;  /* Ignore IPSec -> give packet to UDP socket unchanged */
            }
            if ((pkt->end - pkt->start > 12)
                && (IP_GET_32ON16(&pkt->data[pkt->start + IPCOM_UDP_HEADER_LENGTH]) == 0)
                && ipcom_sysvar_get_as_int("ipike.port", 0)
                == IP_GET_NTOHS(&pkt->data[pkt->start + IPCOM_UDP_HEADER_LENGTH + 2]))
            {
                /* IKE Packet */
                goto okout;  /* Ignore IPSec -> give packet to UDP socket unchanged */
            }
#endif
            /* Update param */

            /* Store UDP header, to later handle eventual port change */
            udph = (Ipcom_udp_header *)&pkt->data[pkt->start];
            param->key.proto = IP_IPPROTO_ESP;
            pkt->start      += IPCOM_UDP_HEADER_LENGTH;

            IPCOM_LOG2(DEBUG2, "          Unpacking UDP tunneled ESP packet: %s length=%d",
                       ipipsec_key_name(pkt, param),
                       pkt->end - pkt->ipstart);
            /* fall through*/

        case IP_IPPROTO_ESP:
            if (ipipsec_conf_esp_enable != 1)
            {
                IPCOM_LOG1(NOTICE, "<input discard> :: ESP not globally enabled (pkt '%s')",
                              ipipsec_key_name(pkt, param));
                IPIPSEC_ERROUT(-IP_ERRNO_EOPNOTSUPP);
            }
            param->key.ports.espah_spi_n = IP_GET_32ON16(&pkt->data[pkt->start]);
            break;
#endif /* IPIPSEC_USE_ESP */

#ifdef IPIPSEC_USE_AH
        case IP_IPPROTO_AH:
            if (ipipsec_conf_ah_enable != 1)
            {
                IPCOM_LOG1(NOTICE, "<input discard> :: AH not globally enabled (pkt '%s')",
                              ipipsec_key_name(pkt, param));
                IPIPSEC_ERROUT(-IP_ERRNO_EOPNOTSUPP);
            }
            param->key.ports.espah_spi_n = IP_GET_32ON16(&pkt->data[pkt->start + 4]);
            break;
#endif /* IPIPSEC_USE_AH */

        default:
            goto transport_packet;
        } /* switch(param->key.proto) */


        /****** AH or ESP packet pre-check ok ******/
        ip_assert(param->key.proto == IP_IPPROTO_ESP || param->key.proto == IP_IPPROTO_AH);

        /* [rfc2406:2.1] SPI value of zero(0) is reserved and MUST not be sent on wire */
        if (param->key.ports.espah_spi_n == 0)
        {
#ifndef IPNET
            /* If no SA found for UDP encapsulated packet, use normal UDP processing (dirty fix) */
            if (udph != IP_NULL)
            {
                param->key.proto = IP_IPPROTO_UDP;
                pkt->start  -= IPCOM_UDP_HEADER_LENGTH;
                goto okout;
            }
#endif

            IPSEC_MIB(ipipsec.stats.ipsecUnknownSpiErrors++);
            if (sa != IP_NULL)
            {
                IPSEC_MIB(sa->err_in_other++);
            }

            IPCOM_LOG2(WARNING, "<input discard> :: %s SPI == 0 (pkt '%s')",
                       IPSECPROTONAME(param->key.proto),
                       ipipsec_key_name(pkt, param));
            IPIPSEC_ERROUT(-IP_ERRNO_EINVAL);
        }


        /***** Lookup incoming SA in global SA database *****/
        ip_assert(param->key.direction == IPIPSEC_DIR_IN);
        sa = ipipsec_sa_lookup(&param->key, IPIPSEC_LFLAGS_USEHASH);
        if (sa == IP_NULL)
        {
#ifndef IPNET
            /* No SA found for UDP encap packet -> use normal UDP processing (dirty fix) */
            if (udph != IP_NULL)
            {
                param->key.proto = IP_IPPROTO_UDP;
                pkt->start      -= IPCOM_UDP_HEADER_LENGTH;
                goto okout;
            }
#endif

            IPSEC_MIB(ipipsec.stats.ipsecPolicyErrors++);
            IPSEC_STATS(ipipsec.stats.sa_miss++);

            ipipsec_send_trap(&ipipsec.sa_bypass, param, IPIPSEC_TRAPTYPE_invalidSpiTrap,
                              ip_ntohl(param->key.ports.espah_spi_n));

            IPCOM_LOG1(INFO, "<input discard> :: no SA for pkt '%s'",
                       ipipsec_key_name(pkt, param));
            IPIPSEC_ERROUT(-IP_ERRNO_ENOENT);
        }

        ip_assert(sa != IP_NULL);
        ip_assert(sa->key.domain == param->key.domain);
        ip_assert(sa->key.direction == param->key.direction);
        IPSEC_STATS(ipipsec.stats.sa_match++);

        /* SA must be valid, i.e. mature state */
        if (sa->sadb_sa_state != SADB_SASTATE_MATURE)
        {
            IPCOM_LOG2(INFO, "<input discard> :: (pkt '%s') imature SA (SA: %s)",
                       ipipsec_key_name(pkt, param),
                       ipipsec_sa_name(sa));
            IPIPSEC_ERROUT(-IP_ERRNO_EINVAL);
        }

        /* Minimum header size check */
        if (pkt->end - pkt->start < sa->header_size)
        {
            IPCOM_LOG3(WARNING, "<input discard> :: (pkt '%s') illegal length (%d) (SA: %s)",
                       ipipsec_key_name(pkt, param),
                       pkt->end - pkt->start,
                       ipipsec_sa_name(sa));
            IPIPSEC_ERROUT(-IP_ERRNO_EINVAL);
        }

        /* Update UDP encap port for corresponding outbound SA if port has changed */
        if (udph != IP_NULL && udph->src_port != sa->encap_srcport_n)
        {
            Ipipsec_sa *sa2 = (Ipipsec_sa *)sa->group_next;
            if (IP_NULL != sa2)
                sa2->encap_dstport_n = udph->src_port;
        }

        /* Optional replay window check */
        ip_assert(param->key.proto == IP_IPPROTO_ESP || param->key.proto == IP_IPPROTO_AH);
        if (sa->sadb_sa_replay > 0)
        {
            int   check_replay;

            ip_assert(IP_BIT_ISFALSE(sa->sadb_sa_flags, SADB_X_SAFLAGS_NOREPLAY));
            if (param->key.proto == IP_IPPROTO_ESP)
                seql = IP_GET_NTOHL(&pkt->data[pkt->start + 4]);
            else
                seql = IP_GET_NTOHL(&pkt->data[pkt->start + 8]);

#ifdef IPCOM_USE_FORWARDER_IPSEC
            /* Get latest replay counter from forwarder */
            ipcom_forwarder_ipsec_get_replay(sa);
#endif

#ifdef IPIPSEC_USE_ESN
            if (IP_BIT_ISSET(sa->sadb_sa_flags, SADB_X_SAFLAGS_ESN))
            {
                seqh = sa->seqh; /* High sequence number is not sent on link */
                check_replay = ipipsec_check_replay_window_esn(sa, seql, &replay_diff, &inc_replay_Th);
            }
            else
#endif
                check_replay = ipipsec_check_replay_window(sa, seql);

            if (check_replay != 0)
            {
                /* Replay window check failed */
                IPSEC_MIB(ipipsec.stats.ipsecReplayErrors++);
                IPSEC_MIB(sa->err_in_replay++);
#ifdef IPIPSEC_USE_ESN
                if (IP_BIT_ISSET(sa->sadb_sa_flags, SADB_X_SAFLAGS_ESN))
                {
                    if (sa->replay_failures > (Ip_u32)ipipsec_conf_esn_resync_trigger)
                    {
                        IPCOM_LOG5(NOTICE,
                                   "<ESN sync heuristic chk> :: failed (seql=%lu, seqh=%lu, consec failures=%lu) for (pkt '%s')  (SA: %s)",
                                   seql,
                                   seqh,
                                   sa->replay_failures,
                                   ipipsec_key_name(pkt, param),
                                   ipipsec_sa_name(sa));
                    }
                    sa->replay_failures++;
                }

                IPCOM_LOG6(NOTICE,
                           "<input discard> :: replay check (seql=%lu, seqh=%lu, th=%lu, tl=%lu) for (pkt '%s')  (SA: %s)",
                           seql,
                           seqh,
                           sa->replay_Th,
                           sa->replay_Tl,
                           ipipsec_key_name(pkt, param),
                           ipipsec_sa_name(sa));
#else
                IPCOM_LOG4(NOTICE, "<input discard> :: replay check (seq=%lu, rpl=%ul) for (pkt '%s')  (SA: %s)",
                           seq,
                           sa->replay_counter,
                           ipipsec_key_name(pkt, param),
                           ipipsec_sa_name(sa));
#endif /* IPIPSEC_USE_ESN */

#ifdef IPIPSEC_USE_TRAPS
                if (param->key.proto == IP_IPPROTO_ESP)
                    ipipsec_send_trap(sa, param, IPIPSEC_TRAPTYPE_espReplayFailureTrap, sa->err_in_replay);
                if (param->key.proto == IP_IPPROTO_AH)
                    ipipsec_send_trap(sa, param, IPIPSEC_TRAPTYPE_ahReplayFailureTrap, sa->err_in_replay);
#endif
                IPIPSEC_ERROUT(-IP_ERRNO_EACCES);
            }
        } /* end of replay check */


        /* Check SA source address */
        ip_assert(sa->key.domain == param->key.domain);
        if (ipcom_memcmp(ipipsec.zeroes, &sa->key.src, sa->in_addr_size)
           && ipcom_memcmp(&sa->key.src, &param->key.src, sa->in_addr_size))
        {
            IPCOM_LOG2(NOTICE, "<input discard> :: (pkt '%s') source address mismatch  (SA: %s)",
                       ipipsec_key_name(pkt, param),
                       ipipsec_sa_name(sa));
            goto policy_error;
        }


        /* Register first use and reset timeout */
        if (sa->ltcurrent.usetime == 0)
        {
#ifdef IPIPSEC_USE_DAEMONS
            ipipsec_send_control_signal(IPIPSEC_SIG_REGISTERUSE, sa);
#else
            sa->ltcurrent.usetime = ipipsec_seconds_now();
            ipipsec_sa_reset_timeout(sa);
#endif
        }


        /*********** process [ESP/AH] header ***********/
        switch(param->key.proto)
        {
#ifdef IPIPSEC_USE_ESP
        case IP_IPPROTO_ESP:
            {
                int     i;
                int     offset;
                Ip_u8   iv[IPIPSEC_MAX_IV_SIZE];
                Ip_u8   pad_bytes;
                Ip_u8   calc_digest[IPIPSEC_AUTH_MAXKEYLEN];
                Ip_s32  crypt_length;
#if defined(IPHWCRYPTO) && defined(IPIPSEC_USE_ESN)
                Ip_bool esn = IP_FALSE;
#endif
                ip_assert(sa != IP_NULL);

                /*IPSECSTATS(ipipsec.stats.func_esp_input++);*/

                /***** Verify that payload length is a multiple of the blocksize */
                crypt_length = pkt->end - pkt->start;
                if (sa->auth)
                    crypt_length -= sa->auth->alg->auth_bytes;
                if (sa->encrypt)
                {
                    crypt_length -= sa->header_size + sa->encrypt->iv_length;

                    /* Encrypted payload length should be at least 1 block and of even block-size */
                    if (crypt_length <= 0
                        || (sa->sadb_sa_encrypt != SADB_X_EALG_AESCTR
                            && (crypt_length & (sa->encrypt->alg->block_bytes-1))))
                    {
                        IPSEC_MIB(ipipsec.stats.ipsecDecryptionErrors++);
                        IPSEC_MIB(sa->err_in_decrypt++);
                        IPCOM_LOG3(WARNING, "<input discard> :: illegal payload length (%ld) for (pkt '%s')  (SA: %s)",
                                   crypt_length,
                                   ipipsec_key_name(pkt, param),
                                   ipipsec_sa_name(sa));
                        IPIPSEC_ERROUT(-IP_ERRNO_EINVAL);
                    }
                }

                /* Update byte counters and check expiration */
                if (ipipsec_lifetime_bytes(sa, pkt, IPIPSEC_DIR_IN, (Ip_u32)crypt_length) != 0)
                {
                    IPSEC_MIB(sa->err_in_other++);
                    IPIPSEC_ERROUT(-IP_ERRNO_ENXIO);
                }

#ifdef IPHWCRYPTO
 #ifdef IPIPSEC_USE_ESN
                if (sa->auth)
                {
                    ip_assert(sa->sadb_sa_auth != SADB_AALG_NONE);
                    ip_assert(sa->auth->alg->update);
                    if (sa->sadb_sa_replay > 0)
                    {
                        /* 'scrap' the auth field from hereon */
                        pkt->end -= sa->auth->alg->auth_bytes;

                        /* For IPHWCRYPTO and ESN the digest calculated by
                         * the HW doesnt include seqh. This can be removed
                         * in case of future driver updates */

                        /* inner HMAC (includes ESP
                         * header+payload. excludes auth field) */
                        ipipsec_calc_digest(sa, pkt, seqh, param->calc_digest);
                        esn = IP_TRUE;

                        /* 'restore' the auth field from hereon */
                        pkt->end += sa->auth->alg->auth_bytes;
                    }
                }
 #endif

                ip_assert(sa->esp_input != IP_NULL);
                i = sa->esp_input(&pkt->data[pkt->start],
                                   pkt->end - pkt->start,
                                   sa,
                                   param,
                                   calc_digest,
                                   pkt);
                if (i == 2)
                {
                    /* Replay windows updated in HWCRYPTO */
                    /* Asynchronous */
                    retval = IPIPSEC_KEPT;
                    goto okout;
                }
                else if (i <= 0)
                {
                    IPCOM_LOG0(DEBUG, "<input discard> :: packet dropped due to hardware crypto failure");
                    IPIPSEC_ERROUT(-IP_ERRNO_EINVAL);  /* silent drop */
                }
#endif  /* #ifdef IPHWCRYPTO */

               /***** Authenticate the datagram (inc. ESP header but NOT AUTH field) *****/
                if (sa->auth)
                {
                    ip_assert(sa->sadb_sa_auth != SADB_AALG_NONE);
                    ip_assert(sa->auth->alg->update);

                    /* 'scrap' the auth field from hereon */
                    pkt->end -= sa->auth->alg->auth_bytes;
#ifdef IPHWCRYPTO
 #ifdef IPIPSEC_USE_ESN
                    if (esn == IP_TRUE)
                        ipcom_memcpy(calc_digest, param->calc_digest, sizeof(calc_digest));
 #endif
#else
                    ipipsec_calc_digest(sa, pkt, seqh, calc_digest);
#endif

                    /* The calculated digest should match the digest in the packet */
                    if (ipcom_memcmp(calc_digest, &pkt->data[pkt->end], sa->auth->alg->auth_bytes) != 0)
                    {
                        IPSEC_MIB(ipipsec.stats.ipsecAuthenticationErrors++);
                        IPSEC_MIB(sa->err_in_auth++);

                        ipipsec_send_trap(sa, param, IPIPSEC_TRAPTYPE_espAuthFailureTrap, sa->err_in_auth);

                        IPCOM_LOG2(NOTICE, "<input discard> :: bad digest for (pkt '%s')  (SA: %s)",
                                   ipipsec_key_name(pkt, param),
                                   ipipsec_sa_name(sa));
                        IPIPSEC_ERROUT(-IP_ERRNO_EACCES);
                    }
                }

                /* Skip ESP header */
                pkt->start += sa->header_size;

                /***** Decrypt the datagram (including the ESP header but NOT the AUTH field) *****/
                if (sa->encrypt)
                {
                    /* Get IV from start of payload, then skip it */
                    if (sa->encrypt->iv_length)
                    {
                        ipcom_memcpy(iv, &pkt->data[pkt->start],
                                     sa->encrypt->iv_length);
                        if (IP_BIT_ISSET(sa->sadb_sa_flags, SADB_X_SAFLAGS_HALFIV))
                        {
                            /* generate 2nd half of IV for halfiv ESP */
                            for (offset = 0; offset < sa->encrypt->iv_length; offset++)
                                iv[sa->encrypt->iv_length + offset] = (Ip_u8)~iv[offset];
                        }
                        /* Skip IV */
                        pkt->start += sa->encrypt->iv_length;
                    }

                    if (sa->sadb_sa_encrypt != SADB_X_EALG_AESCTR
                        && ((pkt->end - pkt->start) % sa->encrypt->alg->block_bytes) != 0)
                    {
                        IPSEC_MIB(ipipsec.stats.ipsecDecryptionErrors++);
                        IPSEC_MIB(sa->err_in_decrypt++);
                        IPCOM_LOG3(WARNING, "<input discard> :: illegal block size (%ld) for (pkt '%s')  (SA: %s)",
                                   pkt->end - pkt->start,
                                   ipipsec_key_name(pkt, param),
                                   ipipsec_sa_name(sa));
                        IPIPSEC_ERROUT(-IP_ERRNO_EINVAL);
                    }

                    /* Decrypt */
#ifndef IPHWCRYPTO
                    sa->encrypt->alg->decrypt(&pkt->data[pkt->start],
                                           pkt->end - pkt->start,
                                           &sa->encrypt->key, iv);
#endif  /* #ifndef IPHWCRYPTO */

                }

                /***** Check pad length *****/
                pad_bytes = pkt->data[pkt->end - 2];
                if (pad_bytes + 2 > (pkt->end - pkt->start))
                {
                    IPSEC_MIB(sa->err_in_pad++);
                    IPCOM_LOG3(WARNING, "<input discard> :: illegal pad length (%ld) for (pkt '%s')  (SA: %s)",
                               pad_bytes,
                               ipipsec_key_name(pkt, param),
                               ipipsec_sa_name(sa));
                    IPIPSEC_ERROUT(-IP_ERRNO_EINVAL);
                }

                /* Check padding - check all pad bytes! */
                if (IP_BIT_ISFALSE(sa->sadb_sa_flags, SADB_X_SAFLAGS_RANDOMPADDING))
                {
                    for (i = 1; i <= pad_bytes; i++)
                        if (pkt->data[pkt->end-2-pad_bytes+i-1] != i)
                        {
                            IPSEC_MIB(sa->err_in_pad++);
                            IPCOM_LOG4(WARNING, "<input discard> :: bad padding ('%d', expected '%d') for (pkt '%s')  (SA: %s)",
                                       pkt->data[pkt->end-2-pad_bytes+i-1],
                                       i,
                                       ipipsec_key_name(pkt, param),
                                       ipipsec_sa_name(sa));
                            IPIPSEC_ERROUT(-IP_ERRNO_EINVAL);
                        }
                }


                /***** Valid ESP packet *****/

                /* Set protocol and skip trailing ESP header */
                param->key.proto = pkt->data[pkt->end-1];
                pkt->end -= (2 + pad_bytes);  /* skip the pad bytes + 2 trailing ESP header bytes */
                ip_assert(pkt->end - pkt->start >= 0);

#ifdef IPIPSEC_USE_TFC
                if (IP_BIT_ISSET(sa->sadb_sa_flags, SADB_X_SAFLAGS_TFC))
                    /* Calculate and strip TFC padding */
                    dummy_packet = ipipsec_tfc_remove_padding(param, pkt);
#endif

                /* Update the replay window */
                if (sa->sadb_sa_replay > 0)
                {
                    Ip_err   update;

                    IPIPSEC_DATA_LOCK(ipipsec.spl);
                    if (IP_BIT_ISSET(sa->sadb_sa_flags, SADB_X_SAFLAGS_ESN))
                        update = ipipsec_update_replay_window_esn(sa, seql, replay_diff, inc_replay_Th);
                    else
                        update = ipipsec_update_replay_window(sa, seql);
                    IPIPSEC_DATA_UNLOCK(ipipsec.spl);

                    if (update != IPCOM_SUCCESS)
                        IPIPSEC_ERROUT(-IP_ERRNO_EACCES);
                }

                IPCOM_LOG3(DEBUG2, "          ESP  decrypt ok, seql=%lu seqh=%lu  (SA: %s)",
                           seql,
                           seqh,
                           ipipsec_sa_name(sa));

#ifdef IPIPSEC_USE_TFC
                if (dummy_packet != 0)
                    /* Discard the dummy packet after replay window update */
                    goto discard_dummy;
#endif
            }
            break;
#endif /* IPIPSEC_USE_ESP */


#ifdef IPIPSEC_USE_AH
        case IP_IPPROTO_AH:
            {
                Ip_u8 calc_digest[IPIPSEC_AUTH_MAXKEYLEN];
                /*IPSECSTATS(ipipsec.stats.func_ah_input++);*/

                ip_assert(sa->auth != IP_NULL);

                /* AH header length check */
                if (((pkt->data[pkt->start+1] + 2) << 2) !=
                   (int) (sa->auth->alg->auth_bytes + sa->header_size))
                {
                    IPSEC_MIB(ipipsec.stats.ipsecOtherReceiveErrors++);
                    IPSEC_MIB(sa->err_in_other++);
                    IPCOM_LOG3(WARNING, "<input discard> :: illegal AH header length (%ld) for (pkt '%s')  (SA: %s)",
                               (pkt->data[pkt->start+1] + 2) * sizeof(Ip_u32),
                               ipipsec_key_name(pkt, param),
                               ipipsec_sa_name(sa));
                    IPIPSEC_ERROUT(-IP_ERRNO_EINVAL);
                }

                /* Update byte counters and check expiration */
                if (ipipsec_lifetime_bytes(sa, pkt, IPIPSEC_DIR_IN, (pkt->end - pkt->start) + param->iph_length) != 0)
                {
                    IPSEC_MIB(sa->err_in_other++);
                    IPIPSEC_ERROUT(-IP_ERRNO_ENXIO);
                }


                /* Calculate input AH digest */
                if (ipipsec_ah_digest(sa, pkt, param, seqh, calc_digest) < 0)
                {
                    IPSEC_MIB(ipipsec.stats.ipsecOtherReceiveErrors++);
                    IPSEC_MIB(sa->err_in_other++);
                    IPCOM_LOG2(NOTICE, "<input discard> :: AH digest calculation error for (pkt '%s')  (SA: %s)",
                               ipipsec_key_name(pkt, param),
                               ipipsec_sa_name(sa));
                    IPIPSEC_ERROUT(-IP_ERRNO_EINVAL);
                }

                /* The calculated digest should match the digest in the packet */
                if (ipcom_memcmp(calc_digest, &pkt->data[pkt->start] + sa->header_size, sa->auth->alg->auth_bytes) != 0)
                {
                    IPSEC_MIB(ipipsec.stats.ipsecAuthenticationErrors++);
                    IPSEC_MIB(sa->err_in_auth++);

                    ipipsec_send_trap(sa, param, IPIPSEC_TRAPTYPE_ahAuthFailureTrap, sa->err_in_auth);

                    IPCOM_LOG2(NOTICE, "<input discard> :: bad digest for (pkt '%s')  (SA: %s)",
                               ipipsec_key_name(pkt, param),
                               ipipsec_sa_name(sa));
                    IPIPSEC_ERROUT(-IP_ERRNO_EACCES);
                }


                /***** Valid AH packet *****/

                /* Set protocol and skip AH header */
                param->key.proto  = pkt->data[pkt->start];
                pkt->start += (sa->header_size + sa->auth->alg->auth_bytes);
                ip_assert(pkt->start <= pkt->end);


                /* Update the replay window */
                if (sa->sadb_sa_replay > 0)
                {
                    Ip_err   update;

                    IPIPSEC_DATA_LOCK(ipipsec.spl);
                    if (IP_BIT_ISSET(sa->sadb_sa_flags, SADB_X_SAFLAGS_ESN))
                        update = ipipsec_update_replay_window_esn(sa, seql, replay_diff, inc_replay_Th);
                    else
                        update = ipipsec_update_replay_window(sa, seql);
                    IPIPSEC_DATA_UNLOCK(ipipsec.spl);

                    if (update != IPCOM_SUCCESS)
                        IPIPSEC_ERROUT(-IP_ERRNO_EACCES);
                }

                IPCOM_LOG3(DEBUG2, "          AH   digest ok, seql=%lu seqh=%lu  (SA: %s)",
                           seql,
                           seqh,
                           ipipsec_sa_name(sa));
            }
            break;
#endif /* IPIPSEC_USE_AH */

        default:
            IP_PANIC();
            IPCOM_LOG1(ERR, "ipipsec_input() :: unknown IP protocol (%d)", param->key.proto);
            goto policy_error;
        }
    } /* for (;;)  ESP/AH/IPIP/x header */


 transport_packet:
    ipipsec_create_consecutive_pkt(pkt, param);

    IPCOM_LOG1(DEBUG2, "          %s", ipipsec_key_name(pkt, param));


    /***** Done with AH/ESP -> input filtering check *****/
    if (sa != IP_NULL
        && IP_BIT_ISFALSE(sa->sadb_sa_flags, SADB_X_SAFLAGS_TUNNEL)
        && ipipsec_flow_lookup_input(sa, pkt, param) != 0)
        goto policy_error;

    /* Clear any checksums */
    IPCOM_PKT_RESET_CHECKSUM(pkt);

 okout:
#ifdef IPIPSEC_USE_TFC
    /* For dummy packets that are tunneled */
    if (param->key.proto == IP_IPPROTO_NONEXTHDR)
        goto discard_dummy;
#endif
    return retval;

 policy_error:
    IPSEC_MIB(ipipsec.stats.ipsecPolicyErrors++);
    if (sa != IP_NULL)
    {
        IPSEC_MIB(sa->err_in_policy++);

#ifdef IPIPSEC_USE_TRAPS
        if (sa->sadb_msg_satype == SADB_SATYPE_AH)
            ipipsec_send_trap(sa, param, IPIPSEC_TRAPTYPE_ahPolicyFailureTrap, sa->err_in_policy);
        if (sa->sadb_msg_satype == SADB_SATYPE_ESP)
            ipipsec_send_trap(sa, param, IPIPSEC_TRAPTYPE_espPolicyFailureTrap, sa->err_in_policy);
#endif
    }
    retval = -IP_ERRNO_EACCES;
    return retval;

 errout:
    ipipsec_send_trap(&ipipsec.sa_bypass, param, IPIPSEC_TRAPTYPE_otherPolicyFailureTrap,
                      ip_ntohl(param->key.ports.espah_spi_n));
    ip_assert(retval < 0);
    return retval;

#ifdef IPIPSEC_USE_TFC
 discard_dummy:
#ifdef IPIPSEC_USE_DAEMONS
    /* Silent discard */
    return IPIPSEC_DUMMY_PKT;
#else
    IPCOM_LOG0(DEBUG2,"         Detected dummy TFC packet -> discarding");
    if (pkt != IP_NULL)
    {
        ip_assert(pkt->data != IP_NULL);
        ipcom_pkt_free(pkt);
    }
    return IPIPSEC_KEPT;
#endif /* IPIPSEC_USE_DAEMONS */

#endif /* IPIPSEC_USE_TFC */
}


/*
 *===========================================================================
 *                    ipipsec_policy_check
 *===========================================================================
 * Description:
 * RFC4301 s4.4.1p26: "..an ordered search of SPD MUST be performed to verify that inbound traffic arriving on an SA is consistent with the access control policy expressed in the SPD"
 * RFC4301 s5.2p62: "If an IPsec system receives an inbound packet on an SA and the packet's header fields are not consistent with the selectors for the SA, it MUST discard the packet."
 * Parameters:
 * Returns:   0 for success, else -IP_ERRNO_EACCES
 *
 */
IP_PUBLIC int
ipipsec_policy_check(Ipcom_pkt *pkt, Ipipsec_param *param)
{
    Ipipsec_flow   *flow;

#ifndef IPIPSEC_ON_LOOPBACK
    if (pkt->ifindex == 1)
        /* Always bypass packets received in cleartext on standard loopback interface */
        return 0;
#endif

    param->key.direction = IPIPSEC_DIR_IN;
    (void)ipipsec_key_update(pkt, param);

    /* Lookup input flow on cleartext packet */
    flow = ipipsec_flow_lookup(&param->key, IP_NULL,
                               IPIPSEC_LFLAGS_NOCHECK_PORTS | IPIPSEC_LFLAGS_USEHASH);

    if (flow != IP_NULL)
    {
        ipipsec_print_iph(pkt, param);
        IPCOM_LOG1(DEBUG2, "          Flow: %s",
                   ipipsec_flow_name(flow, flow->sa));

        switch (flow->flowtype)
        {
        case SADB_X_FLOW_TYPE_BYPASS:
            /* Bypass policy -> let packet pass */
            break;

        case SADB_X_FLOW_TYPE_ACQUIRE:
            /* Send acquire message and let packet pass */
            ipipsec_pfkeyv2_acquire(&param->key);
            break;

        case SADB_X_FLOW_TYPE_REQUIRE:
            /* Send acquire message and drop packet silently */
            ipipsec_pfkeyv2_acquire(&param->key);
            return -IP_ERRNO_EACCES;

        default:
            if (param->key.proto == IP_IPPROTO_IPIP
                || param->key.proto == IP_IPPROTO_IPV6)
                /* Let outer header with IPIP or IPV6 bypass or IPNET's
                 * tunneling interfaces will not work.
                 */
                break;

            /* Deny or use policy -> discard packet */
            IPCOM_LOG2(NOTICE, "IP packet '%s' dropped due to flow [%s]",
                       ipipsec_key_name(pkt, param),
                       ipipsec_flow_name(flow, flow->sa));
            return -IP_ERRNO_EACCES;
        }
    }

    return 0;
}


/*
 *===========================================================================
 *                    ipipsec_output_hdrspace
 *===========================================================================
 * Description:   Calculate extra header space needed for an outgoing 5-tuple
 *                in order to properly lower MSS for TCP connections.
 *                Note: param->key.dst and param->key.domain may be modified!
 * Parameters:
 * Returns:       The extra header space required.
 *
 */
IP_PUBLIC Ip_u32
ipipsec_output_hdrspace(Ipipsec_param *param)
{
    Ipipsec_flow  *flow;
    Ipipsec_sa    *sa;
    Ip_u32         space = 0;

    ip_assert(param->key.domain == IP_AF_INET || param->key.domain == IP_AF_INET6);

    if (ipipsec_opened == 0)
        return 0;

    param->key.direction = IPIPSEC_DIR_OUT;

    flow = ipipsec_flow_lookup(&param->key, IP_NULL, 0);
    if (flow == IP_NULL)
        return 0;

    if (flow->sa == &ipipsec.sa_bypass)
        return 0;

    for (sa = flow->sa; sa; sa = sa->group_next)
    {
        ip_assert(sa != &ipipsec.sa_bypass);

        space += sa->header_space;
        space += sa->trailer_space;

        if (IP_BIT_ISSET(sa->sadb_sa_flags, SADB_X_SAFLAGS_TUNNEL | SADB_X_SAFLAGS_UDPENCAP)
            || sa->key.domain != param->key.domain
            || ipcom_memcmp(&sa->key.dst, &param->key.dst, sa->in_addr_size)) /*lint !e645 */
        {
            space += (sa->key.domain == IP_AF_INET ? IPCOM_IP_HEADER_LENGTH : IPCOM_IP6_HEADER_LENGTH);

            if (IP_BIT_ISSET(sa->sadb_sa_flags, SADB_X_SAFLAGS_UDPENCAP))
                space += IPCOM_UDP_HEADER_LENGTH;

            /* Update destination address and domain for nested tunnels */
            ipcom_memcpy(&param->key.dst, &sa->key.dst, sa->in_addr_size);
            param->key.domain = sa->key.domain;
        }
    }

    IPCOM_LOG2(DEBUG2, "Lowering MSS by %d for Flow: %s",
               (int)space,
               ipipsec_flow_name(flow, flow->sa));

    return space;
}


/*
 *===========================================================================
 *                    ipipsec_3tuple_match
 *===========================================================================
 * Description:   Used to check if a given 3-tuple might result in IPSec
 *                processing in order to determine if IPNET may cache the
 *                3-tuple in the forwarding fastcache. Note that we say
 *                "might" because depending on the packet TCP/UDP ports it
 *                may result in processing or not depending on the exact
 *                policies.
 * Parameters:
 * Returns:       IP_TRUE if a given 3-tuple is found in the output policy
 *                database, else IP_FALSE.
 *
 */
IP_PUBLIC Ip_bool
ipipsec_3tuple_match(Ipcom_pkt *pkt, Ipipsec_param *param)
{
    Ipipsec_flow   *flow;

    IPCOM_UNUSED_ARG(pkt);

    ip_assert(param->key.domain == IP_AF_INET || param->key.domain == IP_AF_INET6);

    param->key.direction = IPIPSEC_DIR_OUT;

    flow = ipipsec_flow_lookup(&param->key, IP_NULL, IPIPSEC_LFLAGS_NOCHECK_PORTS);
    if (flow != IP_NULL)
        return IP_TRUE;
    else
        return IP_FALSE;
}


/*
 *===========================================================================
 *                    ipipsec_create
 *===========================================================================
 * Description: Create and Initialize the IPSEC module.
 * Parameters:
 * Returns:     IPCOM_SUCCESS = success or IPCOM_ERR_xxx
 *
 */
IP_PUBLIC Ip_err
ipipsec_create(void)
{
    Ip_err  retval;

    IPCOM_LOG0(DEBUG, "Initializing IPIPSEC");

    retval = ipipsec_init();
    if (retval != IPCOM_SUCCESS)
        return retval;

    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    ipipsec_start
 *===========================================================================
 * Description: Start IPSEC module.
 * Parameters:
 * Returns:     IPCOM_SUCCESS = success.
 *
 */
IP_PUBLIC Ip_err
ipipsec_start(void)
{
    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    ipipsec_version
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC const char *
ipipsec_version(void)
{
    return "@(#) IPIPSEC2 $Name: VXWORKS_ITER18A_FRZ10 $ - INTERPEAK_COPYRIGHT_STRING";
}


/*
 *===========================================================================
 *                         SMP FUNCTIONS
 *===========================================================================
 */

#ifdef IPIPSEC_USE_DAEMONS
/*
 *===========================================================================
 *                    ipipsec_affinity_timeout
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipipsec_affinity_timeout(void *cookie)
{
    Ip_cpu_set_t cpuset;
    int          i;

    IPCOM_UNUSED_ARG(cookie);

    for (i = 0; i < ipipsec.numd; i++)
    {
        if (ipipsec.cpus[i] < 0)
        {
            /* Reserve a CPU */
            ipipsec.cpus[i] = ipcom_cpu_reserved_alloc();
            if (ipipsec.cpus[i] < 0)
            {
                IPCOM_LOG1(WARNING,
                           "ipipsec_affinity_timeout() :: could not reserve a CPU for pid %x",
                           ipipsec.pids[i]);

                /* Reschedule timeout and try again after one second */
                (void) ipnet_timeout_schedule(1000,
                                              ipipsec_affinity_timeout,
                                              IP_NULL,
                                              &ipipsec.atmo);
                return;
            }

            IPCOM_LOG2(INFO,
                       "ipipsec_affinity_timeout() :: reserved CPU %d for pid %x",
                       ipipsec.cpus[i], ipipsec.pids[i]);

            /* Set affinity to the reserved CPU */
            IP_CPU_ZERO(&cpuset);
            IP_CPU_SET(ipipsec.cpus[i], &cpuset);
            if (ipcom_proc_cpu_affinity_set(ipipsec.pids[i], &cpuset) != IPCOM_SUCCESS)
            {
                IPCOM_LOG2(ERR,
                           "ipipsec_affinity_timeout() :: could not set affinity to CPU %d for pid %x",
                           ipipsec.cpus[i], ipipsec.pids[i]);
            }
            else
            {
                IPCOM_LOG2(INFO,
                           "ipipsec_affinity_timeout() :: affinity set to CPU %d for pid %x",
                           ipipsec.cpus[i], ipipsec.pids[i]);
            }
        }
    }
}


/*
 *===========================================================================
 *                    ipipsec_get_next_daemon_pipe
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ipcom_pipe *
ipipsec_get_next_daemon_pipe(void)
{
    if (ipipsec.nextpipe >= ipipsec.numd)
        ipipsec.nextpipe = 0;
    return ipipsec.pipes[ipipsec.nextpipe++];
}


/*
 *===========================================================================
 *                    ipipsec_packet_order_cmp
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipipsec_packet_order_cmp(Ipcom_pkt *pkt1, Ipcom_pkt *pkt2)
{
    if (pkt1->ipsec_seq == pkt2->ipsec_seq)
    {
        IP_PANIC(); /* Must never happen */
        return 0;
    }
    if ((Ip_s16)(pkt1->ipsec_seq - pkt2->ipsec_seq) < 0)
        return 1;
    return -1;
}


/*
 *===========================================================================
 *                    ipipsec_send_notify_signal
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 */
IP_STATIC void
ipipsec_send_notify_signal(void)
{
    Ipcom_pipe *stackpipe;
    Ipnet_sig  *sig;

    /* Notify IPNET */
    stackpipe = ipnet_usr_sock_get_pipe();
    ip_assert(stackpipe != IPCOM_PIPE_INVALID);
    sig = ipcom_malloc(sizeof(*sig));
    if (sig == IP_NULL)
    {
        IPCOM_LOG0(ERR, "ipipsec_send_notify_signal() :: Memory allocation failed");
        return;
    }
    ipnet_usr_sock_init_sig(sig, IPNET_SIG_IPSEC);
    sig->d.ipsec.ipsec_sig_type = IPIPSEC_SIG_CONTROL;
    if (ipcom_pipe_try_send(stackpipe, sig) == IP_FALSE)
    {
        IPCOM_LOG0(INFO, "ipipsec_send_notify_signal() :: Failed sending to stack pipe");
        ipcom_free(sig);
    }
}


/*
 *===========================================================================
 *                    ipipsec_send_control_signal
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 */
IP_STATIC void
ipipsec_send_control_signal(enum Ipipsec_sig_type type, Ipipsec_sa *sa)
{
    Ipnet_sig  *sig;

    /* Send the job to the control pipe */
    sig = ipcom_malloc(sizeof(*sig));
    if (sig == IP_NULL)
    {
        IPCOM_LOG2(CRIT, "ipipsec_send_control_signal(%d) :: Memory allocation failed. SA: %s",
                   type, ipipsec_sa_name(sa));
        return;
    }

    sig->sig_type               = IPNET_SIG_IPSEC;
    sig->d.ipsec.ipsec_sig_type = type;
    sig->d.ipsec.t.expire.sa    = sa;
    if (ipcom_pipe_try_send(ipipsec.ctrlpipe, sig) == IP_FALSE)
    {
        IPCOM_LOG2(INFO, "ipipsec_send_control_signal(%d) :: Failed sending to control pipe. SA: %s",
                   type, ipipsec_sa_name(sa));
        ipcom_free(sig);
        return;
    }

    /* Notify IPNET */
    ipipsec_send_notify_signal();
}


/*
 *===========================================================================
 *                    ipipsec_verify_sa_exists
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 */
IP_STATIC Ip_bool
ipipsec_verify_sa_exists(Ipipsec_sa *sa_verify)
{
    Ipipsec_sa *sa;
    int     sadir, domain_index;

    if (sa_verify == &ipipsec.sa_bypass)
        return IP_TRUE;

    for (domain_index = 0; domain_index < 2; domain_index++)
        for (sadir = 0; sadir < 2; sadir++)
            for(sa = IPCOM_LIST_FIRST(&ipipsec.sa_head[domain_index][sadir]);
                sa != IP_NULL;
                sa = IPCOM_LIST_NEXT(&sa->sa_list))
            {
                if (sa == sa_verify)
                    return IP_TRUE;
            }

    IPCOM_LOG1(WARNING, "ipipsec_verify_sa_exists() :: Already removed SA: %s",
               ipipsec_sa_name(sa));
    return IP_FALSE;
}


/*
 *===========================================================================
 *                    ipipsec_check_queue
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 */
IP_STATIC void
ipipsec_check_queue(Ip_bool flush)
{
    Ipcom_pkt *pkt;
    Ip_s16 diff;

    while ((pkt = ipcom_pqueue_get_next(ipipsec.pktq)) != IP_NULL)
    {
        if (flush == IP_FALSE)
        {
            diff = (Ip_s16)(pkt->ipsec_seq - ipipsec.lastseq);
            ip_assert(diff > 0);
            if (diff > 1)
                break;
        }

        /* Found in order packet */
        pkt = ipcom_pqueue_remove_next(ipipsec.pktq);
        ip_assert(pkt != IP_NULL);
        ipipsec.lastseq = pkt->ipsec_seq;
        if (pkt->ipsec_input)
            ipipsec_process_pktinput(pkt);
        else
            ipipsec_process_pktoutput(pkt);
    }
}


/*
 *===========================================================================
 *                    ipipsec_process_packet_order
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 */
IP_STATIC void
ipipsec_process_packet_order(Ipcom_pkt *pkt)
{
    Ip_s16  diff;
    Ip_bool do_flush;
    int     pktq_size;

    IPIPSEC_DATA_LOCK(ipipsec.spl);
    do_flush = ipipsec.pktq_flush;
    ipipsec.pktq_flush = IP_FALSE;
    IPIPSEC_DATA_UNLOCK(ipipsec.spl);

    diff = (Ip_s16)(pkt->ipsec_seq - ipipsec.lastseq);
    if (diff == 1)
    {
        /* Packet completed in sequence */
        ipipsec.lastseq = pkt->ipsec_seq;
        if (pkt->ipsec_input)
            ipipsec_process_pktinput(pkt);
        else
            ipipsec_process_pktoutput(pkt);
    }
    else if (diff > 1)
    {
        /* Packet completed early. Queue it */
        if (ipcom_pqueue_insert(ipipsec.pktq, pkt) != IPCOM_SUCCESS)
        {
            IPCOM_LOG0(ERR, "ipipsec_process_packet_order() :: could not queue packet");
            do_flush = IP_TRUE;
            ipcom_pkt_free(pkt);
        }

        pktq_size = ipcom_pqueue_size(ipipsec.pktq);
        if (pktq_size >= IPIPSEC_PKT_QUEUE_SIZE)
        {
            /* Queue is full. Flush it */
            IPCOM_LOG0(INFO, "ipipsec_process_packet_order() :: queue full");
            do_flush = IP_TRUE;
        }
    }
    else
    {
        /* Packet completed late. Probably the queue was flushed */
        if (pkt->ipsec_input)
            ipipsec_process_pktinput(pkt);
        else
            ipipsec_process_pktoutput(pkt);
    }

    /* Check for queued packets */
    ipipsec_check_queue(do_flush);
    if (do_flush)
        ipipsec.lastseq = ipipsec.seq; /* Resync */
}


/*
 *===========================================================================
 *                    ipipsecd
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC
IPCOM_PROCESS(ipipsecd)
{
    Ipcom_pipe             *mypipe, *stackpipe;
    Ipnet_sig              *sig;
    struct Ipnet_sig_ipsec *ipsec_sig;
    Ipcom_pkt              *pkt;
    int                     ret;
    Ip_u8                   domain, proto;

    ipcom_proc_init();

    /* Find this daemon's pipe */
    mypipe = ipipsec.pipes[ipcom_atomic_add_and_return(&ipipsec.mypipe, 1) - 1];
    ip_assert(mypipe != IPCOM_PIPE_INVALID);

    for (;;)
    {
        /* Wait for packets */
        ipcom_pipe_recv(mypipe, (void **)&sig);
        ip_assert(sig != IP_NULL);
        ip_assert(sig->sig_type == IPNET_SIG_IPSEC);
        ipsec_sig = &sig->d.ipsec;

        /* Process signal */
        switch (ipsec_sig->ipsec_sig_type)
        {
        case IPIPSEC_SIG_DECRYPT:
            ret = ipipsec_input(ipsec_sig->t.crypto.pkt, &ipsec_sig->t.crypto.param);
            pkt = ipsec_sig->t.crypto.pkt;
            domain = ipsec_sig->t.crypto.param.key.domain;
            proto  = ipsec_sig->t.crypto.param.key.proto;
            break;
        case IPIPSEC_SIG_ENCRYPT:
            ret = ipipsec_output(&ipsec_sig->t.crypto.pkt, &ipsec_sig->t.crypto.param);
            pkt = ipsec_sig->t.crypto.pkt;
            domain = ipsec_sig->t.crypto.param.key.domain;
            proto  = ipsec_sig->t.crypto.param.key.proto;
            break;
        case IPIPSEC_SIG_LOCK:
            ipsec_sig->ipsec_sig_type = IPIPSEC_SIG_ACK;
            ipcom_pipe_send((Ipcom_pipe *)sig->response_pipe, sig);
            for (;;)
            {
                /* Wait for the unlock command */
                ipcom_pipe_recv(mypipe, (void **)&sig);
                ip_assert(sig != IP_NULL);
                ip_assert(sig->sig_type == IPNET_SIG_IPSEC);
                ipsec_sig = &sig->d.ipsec;
                if (ipsec_sig->ipsec_sig_type == IPIPSEC_SIG_UNLOCK)
                {
                    ipsec_sig->ipsec_sig_type = IPIPSEC_SIG_ACK;
                    ipcom_pipe_send((Ipcom_pipe *)sig->response_pipe, sig);
                    break;
                }
                else
                {
                    IP_PANIC();
                }
            }
            continue;
        default:
            IP_PANIC();
            continue;
        }

        ip_assert(ipsec_sig->ipsec_sig_type == IPIPSEC_SIG_DECRYPT
                  || ipsec_sig->ipsec_sig_type == IPIPSEC_SIG_ENCRYPT);

        if (ret == IPIPSEC_KEPT)
        {
            /* Asynchronous not supported with SMP */
            IP_PANIC();
            continue;
        }
        else if (ret == IPIPSEC_DUMMY_PKT)
        {
            IPCOM_LOG0(DEBUG2,"         Detected dummy TFC packet -> discarding");
        }
        else if (ret >= IPIPSEC_OK)
        {
            /* Send processed packet back to ipnet */
            stackpipe = ipnet_usr_sock_get_pipe();
            ip_assert(stackpipe != IPCOM_PIPE_INVALID);

            sig->d.ipsec.ipsec_sig_type = sig->d.ipsec.ipsec_sig_type == IPIPSEC_SIG_DECRYPT
                                          ? IPIPSEC_SIG_PKTINPUT : IPIPSEC_SIG_PKTOUTPUT;
            sig->d.ipsec.t.packet.pkt    = pkt;
            sig->d.ipsec.t.packet.domain = domain;
            sig->d.ipsec.t.packet.proto  = proto;
            if (ipcom_pipe_try_send(stackpipe, sig))
                continue;    /* Get next packet */

            /* Failed sending to ipnet pipe */
            IPCOM_LOG1(DEBUG2, "ipipsecd() :: Failed sending to stack pipe on %s",
                       ipsec_sig->ipsec_sig_type == IPIPSEC_SIG_DECRYPT ? "input" : "output");
        }
        else
        {
            /* Drop packet. No log needed here since logged in core ipsec code */
        }

        /* Flush packet order queue on next signal */
        IPIPSEC_DATA_LOCK(ipipsec.spl);
        ipipsec.pktq_flush = IP_TRUE;
        IPIPSEC_DATA_UNLOCK(ipipsec.spl);
        ipcom_pkt_free(pkt);
    }

    /* unreachable: ipcom_proc_exit(); */
}


/*
 *===========================================================================
 *                    ipipsec_suspend_daemons
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 */
IP_GLOBAL Ipnet_sig *
ipipsec_suspend_daemons(void)
{
    Ipnet_sig       *sig;
    Ipcom_pipe_attr  pipe_attr;
    int              i;

    sig = ipcom_malloc(sizeof(*sig));
    if (sig == IP_NULL)
        return IP_NULL;
    sig->sig_type = IPNET_SIG_IPSEC;

    /* Create the response pipe */
    ipcom_pipe_attr_init(&pipe_attr);
    pipe_attr.msg_limit = IPCOM_PIPE_MSG_LIMIT_ONE;
    sig->response_pipe  = ipcom_pipe_new(IPCOM_PIPE_GENERIC, &pipe_attr);
    if (sig->response_pipe == IPCOM_PIPE_INVALID)
    {
        ipcom_free(sig);
        return IP_NULL;
    }

    /* Send the command to each ipsec daemon */
    for (i=0; i<ipipsec.numd; i++)
    {
        sig->d.ipsec.ipsec_sig_type = IPIPSEC_SIG_LOCK;
        ipcom_pipe_send(ipipsec.pipes[i], sig);
        ipcom_pipe_recv(sig->response_pipe, (void **)&sig);
        ip_assert(sig->sig_type == IPNET_SIG_IPSEC
                  && sig->d.ipsec.ipsec_sig_type == IPIPSEC_SIG_ACK);
    }
    return sig;
}


/*
 *===========================================================================
 *                    ipipsec_resume_daemons
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 */
IP_GLOBAL void
ipipsec_resume_daemons(Ipnet_sig *sig)
{
    int i;

    /* Send the command to each ipsec daemon */
    for (i=0; i<ipipsec.numd; i++)
    {
        sig->d.ipsec.ipsec_sig_type = IPIPSEC_SIG_UNLOCK;
        ipcom_pipe_send(ipipsec.pipes[i], sig);
        ipcom_pipe_recv(sig->response_pipe, (void **)&sig);
        ip_assert(sig->sig_type == IPNET_SIG_IPSEC
                  && sig->d.ipsec.ipsec_sig_type == IPIPSEC_SIG_ACK);
    }


    /* Free lock resources */
    ipcom_pipe_free(sig->response_pipe);
    ipcom_free(sig);
}


/*
 *===========================================================================
 *                    ipipsec_poll_control_pipe
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 */
IP_GLOBAL void
ipipsec_poll_control_pipe(void)
{
    Ipnet_sig              *sig;
    struct Ipnet_sig_ipsec *ipsec_sig;
    Ipipsec_sa            *sa;

    while (ipcom_pipe_try_recv(ipipsec.ctrlpipe, (void **)&sig) == IPCOM_SUCCESS)
    {
        ip_assert(sig != IP_NULL);
        ip_assert(sig->sig_type == IPNET_SIG_IPSEC);
        ipsec_sig = &sig->d.ipsec;

        /* Process signal */
        switch (ipsec_sig->ipsec_sig_type)
        {
        case IPIPSEC_SIG_HARDEXPIRE:
            sa = ipsec_sig->t.expire.sa;
            if (ipipsec_verify_sa_exists(sa))
            {
                ipipsec_pfkeyv2_expire(sa, IPIPSEC_EXPIRE_TYPE_HARD);
                ipipsec_sa_free(sa, IP_FALSE, IP_TRUE);
            }
            break;
        case IPIPSEC_SIG_SOFTEXPIRE:
            sa = ipsec_sig->t.expire.sa;
            if (ipipsec_verify_sa_exists(sa))
            {
                ipipsec_pfkeyv2_expire(sa, IPIPSEC_EXPIRE_TYPE_SOFT);
            }
            break;
        case IPIPSEC_SIG_REGISTERUSE:
            sa = ipsec_sig->t.expire.sa;
            if (ipipsec_verify_sa_exists(sa))
            {
                sa->ltcurrent.usetime = ipipsec_seconds_now();
                ipipsec_sa_reset_timeout(sa);
            }
            break;
        case IPIPSEC_SIG_TRAP:
            sa = ipsec_sig->t.trap.sa;
            if (ipipsec_verify_sa_exists(sa))
            {
                ipipsec_send_trap(sa,
                                  &ipsec_sig->t.trap.param,
                                  ipsec_sig->t.trap.traptype,
                                  ipsec_sig->t.trap.errors);
            }
            break;
        default:
            IP_PANIC();
            break;
        }
        ipcom_free(sig);
    }
}


/*
 *===========================================================================
 *                    ipipsec_send_daemon_pipe
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:   IPIPSEC_OK
 *            IPIPSEC_KEPT
 *            negative errno
 */
IP_PUBLIC int
ipipsec_send_daemon_pipe(Ipcom_pkt *pkt, Ipipsec_param *param, Ip_u8 input)
{
    Ipnet_sig  *sig = IP_NULL;
    Ipcom_pipe *pipe;
    int         ret;

    if (input == 0)
    {
        /* Output path */
        ret = ipipsec_flow_lookup_output(param, pkt);
        if (ret == 0)
        {
#ifdef IPIPSEC_USE_TFC
            if (param->key.proto == IP_IPPROTO_NONEXTHDR)
            {
                IPCOM_LOG0(DEBUG, "<output discard> :: dummy TFC packet dropped due to missing flow");
                IPSEC_STATS(ipipsec.stats.sa_dummy_send_errors++);
                return -IP_ERRNO_EINVAL;  /* silent drop */
            }
#endif
            /* cleartext */
            pkt->start = pkt->ipstart;
            return IPIPSEC_OK;
        }
        else if (ret < 0)
            return ret;
    }

    /* Tag packet with sequence number */
    pkt->ipsec_seq = ++ipipsec.seq;

    /* Get an ipsec daemon pipe */
    pipe = ipipsec_get_next_daemon_pipe();
    ip_assert(pipe != IPCOM_PIPE_INVALID);

    /* Prepare signal */
    sig = ipcom_pkt_get_info(pkt, IPNET_PKT_INFO_SIG);
    if (sig == IP_NULL)
        sig = ipcom_pkt_alloc_info(pkt, IPNET_PKT_INFO_SIG, sizeof(Ipnet_sig));
    sig->sig_type               = IPNET_SIG_IPSEC;
    sig->d.ipsec.ipsec_sig_type = input ? IPIPSEC_SIG_DECRYPT : IPIPSEC_SIG_ENCRYPT;
    sig->d.ipsec.t.crypto.pkt = pkt;
    ipcom_memcpy(&sig->d.ipsec.t.crypto.param, param, sizeof(*param));

    /* Send signal */
    ipcom_pipe_send(pipe, sig);

    /* Asynchronous return code */
    return IPIPSEC_KEPT;
}

#endif /* #ifdef IPIPSEC_USE_DAEMONS */


#if defined(IPIPSEC_USE_DAEMONS) || defined(IPHWCRYPTO)
/*
 *===========================================================================
 *                    ipipsec_process_pktinput
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 */
IP_STATIC void
ipipsec_process_pktinput(Ipcom_pkt *pkt)
{
    Ipnet_netif *netif;

    netif = ipnet_if_indextonetif(IPCOM_VR_ANY, pkt->ifindex);
    if (netif == IP_NULL)
    {
        /* Interface deleted */
        IPCOM_LOG1(WARNING, "ipipsec_process_pktinput() :: Interface with index %d deleted?",
                   pkt->ifindex);
        ipcom_pkt_free(pkt);
        return;
    }

    /* Flag packet and reset pkt->start to new IP header */
    IP_BIT_SET(pkt->flags, IPCOM_PKT_FLAG_IPSEC_DONE | IPCOM_PKT_FLAG_NO_RX_CACHE);
    pkt->start = pkt->ipstart;

    switch (pkt->ipsec_domain)
    {
#ifdef IPCOM_USE_INET
    case IP_AF_INET:
        (void)ipnet_ip4_input(netif, pkt);
        break;
#endif

#ifdef IPCOM_USE_INET6
    case IP_AF_INET6:
        (void)ipnet_ip6_input(netif, pkt);
        break;
#endif
    default:
        IP_PANIC();
        ipcom_pkt_free(pkt);
        break;
    }
}


/*
 *===========================================================================
 *                    ipipsec_process_pktoutput
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 */
IP_STATIC void
ipipsec_process_pktoutput(Ipcom_pkt *pkt)
{
    /* Flag packet */
    pkt->ipsec_skip = 1;

    switch (pkt->ipsec_domain)
    {
#ifdef IPCOM_USE_INET
    case IP_AF_INET:
        {
            Ipcom_ip_header *iph;
            Ipnet_ip4_output_param outparam;

            iph = (Ipcom_ip_header *)&pkt->data[pkt->start];
            ipcom_memset(&outparam, 0, sizeof(outparam));
            outparam.from  = (struct Ip_in_addr *)&iph->src;
            outparam.to    = (struct Ip_in_addr *)&iph->dst;
            outparam.proto = iph->protocol;
            outparam.ttl   = iph->ttl;
            outparam.tos   = iph->tos;
            pkt->ifindex   = 0;

            if (pkt->ipsec_hlen)
            {
                /* Check original packet for don't fragment bit in order
                   to enable path mtu discovery on forwarded packets */
                ip_assert(pkt->next_original != IP_NULL);
                iph = (Ipcom_ip_header *)&pkt->next_original->data[pkt->next_original->ipstart];
                if (IP_BIT_ISSET(iph->frag, IPNET_OFF_DF))
                    IP_BIT_SET(outparam.flags, IPNET_IP4_OPF_DONT_FRAG);
            }

            (void)ipnet_ip4_output(&outparam, pkt);
        }
        break;
#endif  /* #ifdef IPCOM_USE_INET */

#ifdef IPCOM_USE_INET6
    case IP_AF_INET6:
        {
            Ipcom_ip6_header *ip6;
            Ipnet_ip6_output_param outparam;

            ip6 = (Ipcom_ip6_header *)&pkt->data[pkt->start];
            ipcom_memset(&outparam, 0, sizeof(outparam));
            outparam.from    = (struct Ip_in6_addr *)&ip6->src;
            outparam.to      = (struct Ip_in6_addr *)&ip6->dst;
            outparam.nxt_hdr = ip6->nxt;
            outparam.hlim    = ip6->hlim;
            pkt->ifindex     = 0;

            (void)ipnet_ip6_output(&outparam, pkt);
        }
        break;
#endif  /* #ifdef IPCOM_USE_INET6 */

    default:
        IP_PANIC();
        ipcom_pkt_free(pkt);
        break;
    }
}


/*
 *===========================================================================
 *                    ipipsec_process_signal
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 */
IP_GLOBAL void
ipipsec_process_signal(Ipnet_sig *sig)
{
    struct Ipnet_sig_ipsec *ipsec_sig = &sig->d.ipsec;
    Ipcom_pkt              *pkt;

    switch(ipsec_sig->ipsec_sig_type)
    {
    case IPIPSEC_SIG_PKTINPUT:
        pkt = ipsec_sig->t.packet.pkt;
        ip_assert(pkt != IP_NULL);
        pkt->ipsec_domain = ipsec_sig->t.packet.domain;
        pkt->ipsec_input  = 1;
#ifdef IPIPSEC_USE_DAEMONS
        ipipsec_process_packet_order(pkt);
#else
        ipipsec_process_pktinput(pkt);
#endif
        break;

    case IPIPSEC_SIG_PKTOUTPUT:
        pkt = ipsec_sig->t.packet.pkt;
        ip_assert(pkt != IP_NULL);
        pkt->ipsec_domain = ipsec_sig->t.packet.domain;
        pkt->ipsec_input  = 0;
#ifdef IPIPSEC_USE_DAEMONS
        ipipsec_process_packet_order(pkt);
#else
        ipipsec_process_pktoutput(pkt);
#endif
        break;

#ifdef IPIPSEC_USE_DAEMONS
    case IPIPSEC_SIG_CONTROL:
    {
        Ipnet_sig *suspend;

        suspend = ipipsec_suspend_daemons();
        if (suspend != IP_NULL)
        {
            ipipsec_poll_control_pipe();
            ipipsec_resume_daemons(suspend);
        }
        ipcom_free(sig);
        break;
    }
#endif /* #ifdef IPIPSEC_USE_DAEMONS */

    default:
        IP_PANIC();
        break;
    }
}
#endif /* #if defined(IPIPSEC_USE_DAEMONS) || defined(IPHWCRYPTO) */


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */


#if 0

        switch (param->key.domain)
        {
#ifdef IPCOM_USE_INET
        case IP_AF_INET:
            break;
#endif
#ifdef IPCOM_USE_INET6
        case IP_AF_INET6:
            break;
#endif
        default:
            break;
        }
#endif
