/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_nat_ipsecproxy.c,v $ $Revision: 1.13 $
 *     $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_nat_ipsecproxy.c,v $
 *     $Author: markus $
 *     $State: Exp $ $Locker:  $
 *
 *     INTERPEAK_COPYRIGHT_STRING
 *     Design and implementation by Markus Carlstedt <markus@interpeak.se>
 ******************************************************************************
 */

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 * Proxy (ALG) for the IKE/ESP protocol.
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
#include <ipcom_clib.h>
#include <ipcom_sock.h>
#include <ipcom_inet.h>
#include <ipcom_syslog.h>
#include <ipcom_list.h>

#include "ipnet_nat.h"


/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

#ifndef IPCOM_SYSLOG_PRIORITY
#define IPCOM_SYSLOG_PRIORITY    IPNET_SYSLOG_PRIORITY
#endif
#ifndef IPCOM_SYSLOG_FACILITY
#define IPCOM_SYSLOG_FACILITY    IPCOM_LOG_IPNET
#endif

/* ISAKMP timeouts in seconds */
#define IPNET_NAT_ISAKMP_INIT_TIMEOUT             2*60
#define IPNET_NAT_ISAKMP_ESTABLISHED_TIMEOUT      15*60
#define IPNET_NAT_ISAKMP_OLD_TIMEOUT              5*60

/* ESP timeouts in seconds */
#define IPNET_NAT_ESP_INIT_TIMEOUT                2*60
#define IPNET_NAT_ESP_ESTABLISHED_TIMEOUT         15*60
#define IPNET_NAT_ESP_SQUELCHED_TIMEOUT           30

/* Maximum number of retries to establish a translation entry */
#define IPNET_NAT_ESP_INIT_MAX_RETRIES            5

/* Maximum number of ESP translations */
#define IPNET_NAT_ESP_TRANS_MAX_ENTRIES           128

/* Maximum number of ISAKMP translations */
#define IPNET_NAT_ISAKMP_TRANS_MAX_ENTRIES        128


/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

/*
 *===========================================================================
 *                         Ipnet_nat_esp_hdr
 *===========================================================================
 */
#include <ipcom_align16.h>
typedef IP_PACKED1 struct Ipnet_nat_esp_hdr_struct
{
    Ip_u32 spi;
    Ip_u32 seqno;
}
IP_PACKED2 Ipnet_nat_esp_hdr;
#include <ipcom_align16.h>


/*
 *===========================================================================
 *                         Ipnet_nat_esp_trans
 *===========================================================================
 */
typedef struct Ipnet_nat_esp_trans_struct
{
    Ipcom_list list;            /* list pointer */
    Ip_u32     local_address;
    Ip_u32     remote_address;
    Ip_u32     outbound_spi;
    Ip_u32     inbound_spi;
    Ip_u32     blocking;
    Ip_u32     squelched;
    Ip_u32     outbound_retry_count;
    void       *tmo;            /* pointer to timeout */
}
Ipnet_nat_esp_trans;


/*
 *===========================================================================
 *                         Ipnet_nat_isakmp_hdr
 *===========================================================================
 */
#include <ipcom_align16.h>
typedef IP_PACKED1 struct Ipnet_nat_isakmp_hdr_struct
{
    Ip_u64 init_cookie;
    Ip_u64 resp_cookie;
}
IP_PACKED2 Ipnet_nat_isakmp_hdr;
#include <ipcom_align16.h>


/*
 *===========================================================================
 *                         Ipnet_nat_isakmp_trans
 *===========================================================================
 */
typedef struct Ipnet_nat_isakmp_trans_struct
{
    Ipcom_list list;            /* list pointer */
    Ip_u32     local_address;
    Ip_u32     remote_address;
    Ip_u64     init_cookie;
    Ip_u64     resp_cookie;
    Ip_u32     old;
    Ip_u32     create_time;
    void       *tmo;            /* pointer to timeout */
}
Ipnet_nat_isakmp_trans;


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

IP_STATIC Ipcom_list ipnet_nat_proxy_ipsec_esp_list;
IP_STATIC Ipcom_list ipnet_nat_proxy_ipsec_isakmp_list;


/*
 ****************************************************************************
 * 9                   LOCAL FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipnet_nat_proxy_esp_translation_timeout
 *===========================================================================
 * Description: Timeout handler for ESP translation timeouts.
 * Parameters:  cookie - a cookie that is set when the timeout is added.
 * Returns:     nothing.
 */
IP_STATIC void
ipnet_nat_proxy_esp_translation_timeout(void *cookie)
{
    Ipnet_nat_esp_trans *trans = cookie;

    IPCOM_LOG4(DEBUG, "ipnet_nat_proxy_esp_translation_timeout() :: expired translation:\n"
                      "local address=0x%08x remote address=0x%08x\n"
                      "outbound spi=0x%08x inbound spi=0x%08x",
                       trans->local_address, trans->remote_address,
                       trans->outbound_spi, trans->inbound_spi);

    ipcom_list_remove(&trans->list);
    ipcom_free(trans);
}


/*
 *===========================================================================
 *                    ipnet_nat_proxy_ipsec_esp_find_pending
 *===========================================================================
 * Description: Match ESP packets with the list of pending ESP translation
 *              entries.
 * Parameters:  remote_address - public side address.
 * Returns:     Pointer to the translation or IP_NULL if not found.
 */
IP_STATIC Ipnet_nat_esp_trans *
ipnet_nat_proxy_ipsec_esp_find_pending(Ip_u32 remote_address)
{
    Ipnet_nat_esp_trans *trans;

    for (trans = IPCOM_LIST_FIRST(&ipnet_nat_proxy_ipsec_esp_list);
         trans != IP_NULL;
         trans = IPCOM_LIST_NEXT(&trans->list))
    {
        if (trans->remote_address == remote_address
            && trans->inbound_spi == 0)
        {
            IPCOM_LOG1(DEBUG, "ipnet_nat_proxy_ipsec_esp_find_pending() :: found pending entry for:\n"
                              "remote address=0x%08x inbound spi=0",
                              remote_address);
            return trans;
        }
    }

    IPCOM_LOG1(DEBUG, "ipnet_nat_proxy_ipsec_esp_find_pending() :: no pending entry for:\n"
                      "remote address=0x%08x inbound spi=0",
                      remote_address);
    return IP_NULL;
}


/*
 *===========================================================================
 *                    ipnet_nat_proxy_ipsec_esp_match_outbound
 *===========================================================================
 * Description: Match outgoing ESP packets with the list of ESP translation
 *              entries.
 * Parameters:  spi            - security parameter index.
 *              local_address  - private side address.
 *              remote_address - public side address.
 * Returns:     Pointer to the translation or IP_NULL if not found.
 */
IP_STATIC Ipnet_nat_esp_trans *
ipnet_nat_proxy_ipsec_esp_match_outbound(Ip_u32 spi,
                                         Ip_u32 local_address,
                                         Ip_u32 remote_address)
{
    Ipnet_nat_esp_trans *trans;

    for (trans = IPCOM_LIST_FIRST(&ipnet_nat_proxy_ipsec_esp_list);
         trans != IP_NULL;
         trans = IPCOM_LIST_NEXT(&trans->list))
    {
        if (trans->outbound_spi == spi
            && trans->local_address == local_address
            && trans->remote_address == remote_address)
        {
            IPCOM_LOG3(DEBUG, "ipnet_nat_proxy_ipsec_esp_match_outbound() :: found match for:\n"
                              "local address=0x%08x remote address=0x%08x outbound spi=0x%08x",
                               local_address, remote_address, spi);
            return trans;
        }
    }
    IPCOM_LOG3(DEBUG, "ipnet_nat_proxy_ipsec_esp_match_outbound() :: no match for:\n"
                      "local address=0x%08x remote address=0x%08x outbound spi=0x%08x",
                       local_address, remote_address, spi);
    return IP_NULL;
}


/*
 *===========================================================================
 *                    ipnet_nat_proxy_ipsec_esp_match_inbound
 *===========================================================================
 * Description: Match incoming ESP packets with the list of ESP translation
 *              entries.
 * Parameters:  spi            - security parameter index.
 *              local_address  - private side address.
 *              remote_address - public side address.
 * Returns:     Pointer to the translation or IP_NULL if not found.
 */
IP_STATIC Ipnet_nat_esp_trans *
ipnet_nat_proxy_ipsec_esp_match_inbound(Ip_u32 spi, Ip_u32 remote_address)
{
    Ipnet_nat_esp_trans *trans;

    for (trans = IPCOM_LIST_FIRST(&ipnet_nat_proxy_ipsec_esp_list);
         trans != IP_NULL;
         trans = IPCOM_LIST_NEXT(&trans->list))
    {
        if (trans->inbound_spi == spi
            && trans->remote_address == remote_address)
        {
            IPCOM_LOG2(DEBUG, "ipnet_nat_proxy_ipsec_esp_match_inbound() :: found match for:\n"
                              "remote address=0x%08x inbound spi=0x%08x",
                               remote_address, spi);
            return trans;
        }
    }
    IPCOM_LOG2(DEBUG, "ipnet_nat_proxy_ipsec_esp_match_inbound() :: no match for:\n"
                      "remote address=0x%08x inbound spi=0x%08x",
                       remote_address, spi);
    return IP_NULL;
}


/*
 *===========================================================================
 *                    ipnet_nat_proxy_ipsec_esp_new
 *===========================================================================
 * Description: Create a new ESP translation entry and add it to the list.
 * Parameters:  spi            - security parameter index.
 *              local_address  - private side address.
 *              remote_address - public side address.
 * Returns:     pointer to the translation entry or IP_NULL if failed.
 */
IP_STATIC Ipnet_nat_esp_trans *
ipnet_nat_proxy_ipsec_esp_new(Ip_u32 spi,
                              Ip_u32 local_address,
                              Ip_u32 remote_address,
                              void *mapping)
{
    Ipnet_nat_esp_trans *trans;

    /* Check that there is room for another translation entry */
    if(ipnet_nat_proxy_ipsec_esp_list.size >= IPNET_NAT_ESP_TRANS_MAX_ENTRIES)
    {
        IPCOM_LOG0(ERR, "ipnet_nat_proxy_ipsec_esp_new() ::"
                        "failed - too many translations");
        return IP_NULL;
    }
    trans = ipcom_malloc(sizeof(*trans));
    if (trans == IP_NULL)
    {
        IPCOM_LOG0(ERR, "ipnet_nat_proxy_ipsec_esp_new() ::"
                        "failed to allocate memory for translation entry");
        return IP_NULL;
    }
    ipcom_memset(trans, 0, sizeof(*trans));

    trans->inbound_spi          = 0;
    trans->blocking             = 0;
    trans->squelched            = 0;
    trans->outbound_retry_count = 0;
    trans->outbound_spi         = spi;
    trans->local_address        = local_address;
    trans->remote_address       = remote_address;
    ipcom_list_insert_last(&ipnet_nat_proxy_ipsec_esp_list, &trans->list);
    /* Add the timeout */
    if (ipnet_nat_proxy_timeout_schedule(IPNET_NAT_ESP_INIT_TIMEOUT,
                                         ipnet_nat_proxy_esp_translation_timeout,
                                         trans,
                                         &trans->tmo) < 0)
    {
        ipcom_list_remove(&trans->list);
        ipcom_free(trans);
        return IP_NULL;
    }
    /* Update the mapping timeout */
    ipnet_nat_proxy_set_mapping_timeout(IPNET_NAT_ESP_INIT_TIMEOUT, mapping);
    return trans;
}


/*
 *===========================================================================
 *                    ipnet_nat_proxy_isakmp_translation_timeout
 *===========================================================================
 * Description: Timeout handler for ISAKMP translation timeouts.
 * Parameters:  cookie - a cookie that is set when the timeout is added.
 * Returns:     nothing.
 */
IP_STATIC void
ipnet_nat_proxy_isakmp_translation_timeout(void *cookie)
{
    Ipnet_nat_isakmp_trans *trans = cookie;

    IPCOM_LOG6(DEBUG, "ipnet_nat_proxy_isakmp_translation_timeout() :: expired translation:\n"
                      "local address=0x%08x remote address=0x%08x\n"
                      "initiator cookie=0x%08x%08x responder cookie=0x%08x%08x",
                       trans->local_address, trans->remote_address,
                       IP_U64_GETHI(trans->init_cookie), IP_U64_GETLO(trans->init_cookie),
                       IP_U64_GETHI(trans->resp_cookie), IP_U64_GETLO(trans->resp_cookie));

    ipcom_list_remove(&trans->list);
    ipcom_free(trans);
}


/*
 *===========================================================================
 *                    ipnet_nat_proxy_ipsec_isakmp_match_outbound
 *===========================================================================
 * Description: Match outgoing ISAKMP packets with the list of ISAKMP
 *              translation entries.
 * Parameters:  init_cookie    - pointer to initiator cookie.
 *              resp_cookie    - pointer to responder cookie.
 *              local_address  - private side address.
 *              remote_address - public side address.
 * Returns:     Pointer to the translation or IP_NULL if not found.
 */
IP_STATIC Ipnet_nat_isakmp_trans *
ipnet_nat_proxy_ipsec_isakmp_match_outbound(Ip_u64 *init_cookie,
                                            Ip_u64 *resp_cookie,
                                            Ip_u32 local_address,
                                            Ip_u32 remote_address)
{
    Ipnet_nat_isakmp_trans *trans;

    for (trans = IPCOM_LIST_FIRST(&ipnet_nat_proxy_ipsec_isakmp_list);
         trans != IP_NULL;
         trans = IPCOM_LIST_NEXT(&trans->list))
    {
        if (IP_U64_EQUAL(trans->init_cookie, *init_cookie)
            && IP_U64_EQUAL(trans->resp_cookie, *resp_cookie)
            && trans->local_address == local_address
            && trans->remote_address == remote_address)
        {
            IPCOM_LOG6(DEBUG, "ipnet_nat_proxy_ipsec_isakmp_match_outbound() :: found match for:\n"
                              "local address=0x%08x remote address=0x%08x\n"
                              "initiator cookie=0x%08x%08x responder cookie=0x%08x%08x",
                               local_address, remote_address,
                               IP_U64_GETHI(*init_cookie), IP_U64_GETLO(*init_cookie),
                               IP_U64_GETHI(*resp_cookie), IP_U64_GETLO(*resp_cookie));
            return trans;
        }
    }

    IPCOM_LOG6(DEBUG, "ipnet_nat_proxy_ipsec_isakmp_match_outbound() :: no match for:\n"
                      "local address=0x%08x remote address=0x%08x\n"
                      "initiator cookie=0x%08x%08x responder cookie=0x%08x%08x",
                      local_address, remote_address,
                      IP_U64_GETHI(*init_cookie), IP_U64_GETLO(*init_cookie),
                      IP_U64_GETHI(*resp_cookie), IP_U64_GETLO(*resp_cookie));

    return IP_NULL;
}


/*
 *===========================================================================
 *                    ipnet_nat_proxy_ipsec_isakmp_match_inbound
 *===========================================================================
 * Description: Match incoming ISAKMP packets with the list of ISAKMP
 *              translation entries.
 * Parameters:  init_cookie    - pointer to initiator cookie.
 *              resp_cookie    - pointer to responder cookie.
 *              remote_address - public side address.
 * Returns:     Pointer to the translation or IP_NULL if not found.
 */
IP_STATIC Ipnet_nat_isakmp_trans *
ipnet_nat_proxy_ipsec_isakmp_match_inbound(Ip_u64 *init_cookie,
                                           Ip_u64 *resp_cookie,
                                           Ip_u32 remote_address)
{
    Ipnet_nat_isakmp_trans *trans;

    for (trans = IPCOM_LIST_FIRST(&ipnet_nat_proxy_ipsec_isakmp_list);
         trans != IP_NULL;
         trans = IPCOM_LIST_NEXT(&trans->list))
    {
        if (IP_U64_EQUAL(trans->init_cookie, *init_cookie)
            && IP_U64_EQUAL(trans->resp_cookie, *resp_cookie)
            && trans->remote_address == remote_address)
        {
            IPCOM_LOG5(DEBUG, "ipnet_nat_proxy_ipsec_isakmp_match_inbound() :: found match for:\n"
                              "remote address=0x%08x\n"
                              "initiator cookie=0x%08x%08x responder cookie=0x%08x%08x",
                               remote_address,
                               IP_U64_GETHI(*init_cookie), IP_U64_GETLO(*init_cookie),
                               IP_U64_GETHI(*resp_cookie), IP_U64_GETLO(*resp_cookie));
            return trans;
        }
    }

    IPCOM_LOG5(DEBUG, "ipnet_nat_proxy_ipsec_isakmp_match_inbound() :: no match for:\n"
                      "remote address=0x%08x\n"
                      "initiator cookie=0x%08x%08x responder cookie=0x%08x%08x",
                      remote_address,
                      IP_U64_GETHI(*init_cookie), IP_U64_GETLO(*init_cookie),
                      IP_U64_GETHI(*resp_cookie), IP_U64_GETLO(*resp_cookie));

    return IP_NULL;
}


/*
 *===========================================================================
 *                    ipnet_nat_proxy_ipsec_isakmp_new
 *===========================================================================
 * Description: Create a new ISAKMP translation entry and add it to the list.
 * Parameters:  init_cookie    - pointer to initiator cookie.
 *              resp_cookie    - pointer to responder cookie.
 *              local_address  - private side address.
 *              remote_address - public side address.
 * Returns:     pointer to the translation entry or IP_NULL if failed.
 */
IP_STATIC Ipnet_nat_isakmp_trans *
ipnet_nat_proxy_ipsec_isakmp_new(Ip_u64 *init_cookie,
                                 Ip_u64 *resp_cookie,
                                 Ip_u32 local_address,
                                 Ip_u32 remote_address,
                                 void *mapping)
{
    Ipnet_nat_isakmp_trans *trans;
    Ipnet_nat_proxy_tuple proxy_tuple;

    /* Check that there is room for another translation */
    if(ipnet_nat_proxy_ipsec_isakmp_list.size >= IPNET_NAT_ISAKMP_TRANS_MAX_ENTRIES)
    {
        IPCOM_LOG0(ERR, "ipnet_nat_proxy_ipsec_isakmp_new() ::"
                        "failed - too many translations");
        return IP_NULL;
    }
    trans = ipcom_malloc(sizeof(*trans));
    if (trans == IP_NULL)
    {
        IPCOM_LOG0(ERR, "ipnet_nat_proxy_ipsec_isakmp_new() ::"
                        "failed to allocate memory for translation entry");
        return IP_NULL;
    }
    ipcom_memset(trans, 0, sizeof(*trans));

    IP_U64_COPY(trans->init_cookie, *init_cookie);
    IP_U64_COPY(trans->resp_cookie, *resp_cookie);
    trans->local_address  = local_address;
    trans->remote_address = remote_address;
    trans->create_time = ipnet_nat_proxy_get_time();
    ipcom_list_insert_last(&ipnet_nat_proxy_ipsec_isakmp_list, &trans->list);
    /* Add the timeout */
    if (ipnet_nat_proxy_timeout_schedule(IPNET_NAT_ISAKMP_INIT_TIMEOUT,
                                         ipnet_nat_proxy_isakmp_translation_timeout,
                                         trans,
                                         &trans->tmo) < 0)
    {
        ipcom_list_remove(&trans->list);
        ipcom_free(trans);
        return IP_NULL;
    }
    /* Update the mapping timeout */
    ipnet_nat_proxy_set_mapping_timeout(IPNET_NAT_ISAKMP_INIT_TIMEOUT, mapping);

    /* Add a mapping for ESP in case that the first ESP packet is inbound */
    ipcom_memset(&proxy_tuple, 0, sizeof(proxy_tuple));
    proxy_tuple.protocol     = IP_IPPROTO_ESP;
    proxy_tuple.private_addr = local_address;
    proxy_tuple.public_addr  = remote_address;
    (void)ipnet_nat_proxy_add_mapping(&proxy_tuple,
                                      0,
                                      mapping,
                                      IP_FALSE,         /* NAT port = local source port */
                                      IP_FALSE,
                                      ipnet_nat_proxy_ipsec,
                                      IP_NULL);

    return trans;
}


/*
 *===========================================================================
 *                    ipnet_nat_proxy_ipsec_isakmp_cleanup_list
 *===========================================================================
 * Description: Remove old entries in the ISAKMP translation list.
 * Parameters:  init_cookie    - pointer to initiator cookie.
 *              resp_cookie    - pointer to responder cookie.
 *              local_address  - private side address.
 *              remote_address - public side address.
 * Returns:     Pointer to the translation or IP_NULL if not found.
 */
IP_STATIC void
ipnet_nat_proxy_ipsec_isakmp_cleanup_list(Ip_u64 *init_cookie,
                                          Ip_u64 *resp_cookie,
                                          Ip_u32 local_address,
                                          Ip_u32 remote_address)
{
    Ipnet_nat_isakmp_trans *trans;

    for (trans = IPCOM_LIST_FIRST(&ipnet_nat_proxy_ipsec_isakmp_list);
         trans != IP_NULL;
         trans = IPCOM_LIST_NEXT(&trans->list))
    {
        if (!trans->old &&
            trans->local_address == local_address &&
            trans->remote_address == remote_address)
        {
            if (IP_U64_NEQUAL(trans->init_cookie, *init_cookie) ||
                IP_U64_NEQUAL(trans->resp_cookie, *resp_cookie))
            {
                IPCOM_LOG6(DEBUG, "ipnet_nat_proxy_ipsec_isakmp_cleanup_list() :: found old entry:\n"
                                  "local address=0x%08x remote address=0x%08x\n"
                                  "initiator cookie=0x%08x%08x responder cookie=0x%08x%08x",
                                   trans->local_address, trans->remote_address,
                                   IP_U64_GETHI(trans->init_cookie), IP_U64_GETLO(trans->init_cookie),
                                   IP_U64_GETHI(trans->resp_cookie), IP_U64_GETLO(trans->resp_cookie));
                trans->old = 1;
                ipnet_nat_proxy_timeout_reschedule(IPNET_NAT_ISAKMP_OLD_TIMEOUT,
                                                   ipnet_nat_proxy_isakmp_translation_timeout,
                                                   trans,
                                                   &trans->tmo);
            }
        }
    }
}


/*
 *===========================================================================
 *                    ipnet_nat_proxy_ipsec_isakmp_find_active
 *===========================================================================
 * Description: Find active entries in the ISAKMP translation list.
 * Parameters:  remote_address - public side address.
 *              active_address - pointer to active address.
 * Returns:     Pointer to the translation or IP_NULL if not found.
 */
IP_STATIC Ip_u32
ipnet_nat_proxy_ipsec_isakmp_find_active(Ip_u32 remote_address,
                                         Ip_u32 *active_address)
{
    Ipnet_nat_isakmp_trans *trans;
    Ip_u32 num = 0, tmp = 0;

    for (trans = IPCOM_LIST_FIRST(&ipnet_nat_proxy_ipsec_isakmp_list);
         trans != IP_NULL;
         trans = IPCOM_LIST_NEXT(&trans->list))
    {
        /* Get the newest entry */
        if (trans->remote_address == remote_address
            && (Ip_s32)(trans->create_time - tmp) >= 0)
        {
            tmp = trans->create_time;
            *active_address = trans->local_address;
            num = 1;
        }
    }

    if (num > 0)
    {
        IPCOM_LOG2(DEBUG, "ipnet_nat_proxy_ipsec_isakmp_find_active() :: found active isakmp entry:\n"
                   "local address=0x%08x remote address=0x%08x\n",
                   *active_address, remote_address);
    }

    return num;
}


/*
 *===========================================================================
 *                    ipnet_nat_proxy_ipsec_esp
 *===========================================================================
 * Description: Track ESP packets and forward responses to inner host.
 * Parameters:  newhdr    - pointer to optionally write new IP header.
 *              appdata   - pointer to application data.
 *              applen    - pointer to length of application data.
 *              param     - pointer to proxy parameters.
 * Returns:     1 = Packet modified.
 *              0 = Packet untouched.
 *             -1 = Drop packet.
 */
IP_STATIC int
ipnet_nat_proxy_ipsec_esp(Ip_u8 *newhdr,
                          Ip_u8 *appdata,
                          int    applen,
                          Ipnet_nat_proxy_param *param)
{
    Ipcom_ip_header     *iph;
    Ipnet_nat_esp_hdr   *esp;
    Ipnet_nat_esp_trans *trans;
    Ip_u32 spi, active_address = 0, num;

    /* Check that at least a header is included */
    if (applen < (int)sizeof(*esp))
    {
        IPCOM_LOG0(WARNING, "ipnet_nat_proxy_ipsec_esp() :: message too short for ESP header");
        return 0;
    }
    esp = (Ipnet_nat_esp_hdr *)appdata;
    spi = IP_GET_NTOHL(&esp->spi);

    if (param->incoming == IP_FALSE)
    {
        /* Try to find translation entry, with both initiator and responder cookie */
        trans = ipnet_nat_proxy_ipsec_esp_match_outbound(spi, param->tuple.private_addr, param->tuple.public_addr);

        /* No entry found, maybe create a new one */
        if (trans == IP_NULL)
        {
            trans = ipnet_nat_proxy_ipsec_esp_find_pending(param->tuple.public_addr);
            if (trans != IP_NULL && !trans->squelched)
            {
                /* Found one in progress to same host....need to wait awhile */
                trans->blocking = 1;
                ipnet_nat_proxy_timeout_reschedule(IPNET_NAT_ESP_INIT_TIMEOUT,
                                                   ipnet_nat_proxy_esp_translation_timeout,
                                                   trans,
                                                   &trans->tmo);
                /* Update the mapping timeout */
                ipnet_nat_proxy_set_mapping_timeout(IPNET_NAT_ESP_INIT_TIMEOUT, param->mapping);

                IPCOM_LOG1(DEBUG, "ipnet_nat_proxy_ipsec_esp() ::"
                                  "Blocked ESP packet dropped from local host 0x%08x", param->tuple.private_addr);
                return -1;
            }
            else
            {
                /* Create a new entry, no others in progress to same host... */
                trans = ipnet_nat_proxy_ipsec_esp_new(spi,
                                                      param->tuple.private_addr,
                                                      param->tuple.public_addr,
                                                      param->mapping);
                if (trans == IP_NULL)
                    return -1;
            }
        }
        else
        {
            if (trans->squelched)
            {
                IPCOM_LOG1(DEBUG, "ipnet_nat_proxy_ipsec_esp() ::"
                                  "Squelched ESP Packet Dropped from local host 0x%08x", param->tuple.private_addr);
                return -1;
            }

            /* The remote end hasn't responded yet.... */
            if (trans->inbound_spi == 0)
            {
                ipnet_nat_proxy_timeout_reschedule(IPNET_NAT_ESP_INIT_TIMEOUT,
                                                   ipnet_nat_proxy_esp_translation_timeout,
                                                   trans,
                                                   &trans->tmo);
                /* Update the mapping timeout */
                ipnet_nat_proxy_set_mapping_timeout(IPNET_NAT_ESP_INIT_TIMEOUT, param->mapping);

                if (trans->blocking)
                {
                    trans->outbound_retry_count++;
                    IPCOM_LOG2(DEBUG, "ipnet_nat_proxy_ipsec_esp() ::"
                                      "ESP Outbound Retry Count = %d on local host 0x%08x",
                                      trans->outbound_retry_count, param->tuple.private_addr);
                    if (trans->outbound_retry_count > IPNET_NAT_ESP_INIT_MAX_RETRIES)
                    {
                        trans->squelched = 1;
                        ipnet_nat_proxy_timeout_reschedule(IPNET_NAT_ESP_SQUELCHED_TIMEOUT,
                                                           ipnet_nat_proxy_esp_translation_timeout,
                                                           trans,
                                                           &trans->tmo);
                        /* Update the mapping timeout */
                        IPCOM_LOG1(DEBUG, "ipnet_nat_proxy_ipsec_esp() ::"
                                          "Squelched ESP Packet Dropped from local host 0x%08x", param->tuple.private_addr);
                        return -1;
                    }
                }
            }
            else
            {
                /* Update the mapping timeout */
                ipnet_nat_proxy_set_mapping_timeout(IPNET_NAT_ESP_ESTABLISHED_TIMEOUT, param->mapping);
            }
        }
        return 0;
    }
    else
    {
        /* Try to find translation entry, with both initiator and responder cookie */
        trans = ipnet_nat_proxy_ipsec_esp_match_inbound(spi, param->tuple.public_addr);

        /* If not found, try pending entry */
        if (trans == IP_NULL)
            trans = ipnet_nat_proxy_ipsec_esp_find_pending(param->tuple.public_addr);

        /* No entry found, try to guess who should get the packet based on ISAKMP traffic */
        if (trans == IP_NULL)
        {
            num = ipnet_nat_proxy_ipsec_isakmp_find_active(param->tuple.public_addr, &active_address);
            if (num == 0)
            {
                IPCOM_LOG1(DEBUG, "ipnet_nat_proxy_ipsec_esp() ::"
                                  "ESP packet droppped from remote host 0x%08x", param->tuple.public_addr);
                return -1;
            }
        }

        if (trans != IP_NULL)
        {
            /* Save the inbound spi if needed */
            if (trans->inbound_spi == 0)
                trans->inbound_spi = spi;

            /* Reset the translation entry timer */
            ipnet_nat_proxy_timeout_reschedule(IPNET_NAT_ESP_ESTABLISHED_TIMEOUT,
                                               ipnet_nat_proxy_esp_translation_timeout,
                                               trans,
                                               &trans->tmo);
            /* Update the mapping timeout */
            ipnet_nat_proxy_set_mapping_timeout(IPNET_NAT_ESP_ESTABLISHED_TIMEOUT, param->mapping);

            active_address = trans->local_address;
        }

        /* Translate the destination address.... */
        iph = (Ipcom_ip_header *)newhdr;
        IP_SET_HTONL(&iph->dst, active_address);
        return 1;
    }
}


/*
 *===========================================================================
 *                    ipnet_nat_proxy_ipsec_isakmp
 *===========================================================================
 * Description: Track ISAKMP packets and forward responses to inner host.
 * Parameters:  newhdr    - pointer to optionally write new IP header.
 *              appdata   - pointer to application data.
 *              applen    - pointer to length of application data.
 *              param     - pointer to proxy parameters.
 * Returns:     1 = Packet modified.
 *              0 = Packet untouched.
 *             -1 = Drop packet.
 */
IP_STATIC int
ipnet_nat_proxy_ipsec_isakmp(Ip_u8 *newhdr,
                             Ip_u8 *appdata,
                             int    applen,
                             Ipnet_nat_proxy_param *param)
{
    Ipcom_ip_header        *iph;
    Ipnet_nat_isakmp_hdr   *isakmp;
    Ipnet_nat_isakmp_trans *trans;
    Ip_u64 init_cookie, resp_cookie, null_cookie;

    /* Check that at least a header is included */
    if (applen < (int)sizeof(*isakmp))
    {
        IPCOM_LOG0(WARNING, "ipnet_nat_proxy_ipsec_isakmp() :: message too short for ISAKMP header");
        return 0;
    }
    isakmp = (Ipnet_nat_isakmp_hdr *)appdata;
#ifdef IPCOM_USE_LONGLONG_TYPE
    init_cookie = IP_GET_NTOH64(&isakmp->init_cookie);
    resp_cookie = IP_GET_NTOH64(&isakmp->resp_cookie);
#else
    ipcom_memcpy(&init_cookie.hi, &isakmp->init_cookie, 4);
    init_cookie.hi = ip_ntohl(init_cookie.hi);
    ipcom_memcpy(&init_cookie.lo, (Ip_u8 *)&isakmp->init_cookie + 4, 4);
    init_cookie.lo = ip_ntohl(init_cookie.lo);
    ipcom_memcpy(&resp_cookie.hi, &isakmp->resp_cookie, 4);
    resp_cookie.hi = ip_ntohl(resp_cookie.hi);
    ipcom_memcpy(&resp_cookie.lo, (Ip_u8 *)&isakmp->resp_cookie + 4, 4);
    resp_cookie.lo = ip_ntohl(resp_cookie.lo);
#endif /* #ifdef IPCOM_USE_LONGLONG_TYPE */
    IP_U64_SET(null_cookie, 0, 0);

    /* If the initiator cookie is zero, give up... */
    if (IP_U64_GETHI(init_cookie) == 0 && IP_U64_GETLO(init_cookie) == 0)
        return 0;

    if (param->incoming == IP_FALSE)
    {
        /* Try to find translation entry, with both initiator and responder cookie */
        trans = ipnet_nat_proxy_ipsec_isakmp_match_outbound(&init_cookie,
                                                            &resp_cookie,
                                                            param->tuple.private_addr,
                                                            param->tuple.public_addr);
        /* If not found, try without responder cookie */
        if (trans == IP_NULL)
            trans = ipnet_nat_proxy_ipsec_isakmp_match_outbound(&init_cookie,
                                                                &null_cookie,
                                                                param->tuple.private_addr,
                                                                param->tuple.public_addr);
        /* No entry found, create a new one */
        if (trans == IP_NULL)
            trans = ipnet_nat_proxy_ipsec_isakmp_new(&init_cookie,
                                                     &resp_cookie,
                                                     param->tuple.private_addr,
                                                     param->tuple.public_addr,
                                                     param->mapping);
        if (trans == IP_NULL)
            return -1;

        /* Reset the translation entry timer */
        if (IP_U64_GETHI(trans->resp_cookie) == 0 && IP_U64_GETLO(trans->resp_cookie) == 0)
        {
            ipnet_nat_proxy_timeout_reschedule(IPNET_NAT_ISAKMP_INIT_TIMEOUT,
                                               ipnet_nat_proxy_isakmp_translation_timeout,
                                               trans,
                                               &trans->tmo);
            /* Update the mapping timeout */
            ipnet_nat_proxy_set_mapping_timeout(IPNET_NAT_ISAKMP_INIT_TIMEOUT, param->mapping);
        }
        else
        {
            ipnet_nat_proxy_timeout_reschedule(IPNET_NAT_ISAKMP_ESTABLISHED_TIMEOUT,
                                               ipnet_nat_proxy_isakmp_translation_timeout,
                                               trans,
                                               &trans->tmo);
            /* Update the mapping timeout */
            ipnet_nat_proxy_set_mapping_timeout(IPNET_NAT_ISAKMP_ESTABLISHED_TIMEOUT, param->mapping);
        }
        return 0;
    }
    else
    {
        /* Try to find translation entry, with both initiator and responder cookie */
        trans = ipnet_nat_proxy_ipsec_isakmp_match_inbound(&init_cookie,
                                                           &resp_cookie,
                                                           param->tuple.public_addr);
        /* If not found, try without responder cookie */
        if (trans == IP_NULL)
            trans = ipnet_nat_proxy_ipsec_isakmp_match_inbound(&init_cookie,
                                                               &null_cookie,
                                                               param->tuple.public_addr);
        /* No entry found, drop packet */
        if (trans == IP_NULL)
        {
            IPCOM_LOG1(DEBUG, "ipnet_nat_proxy_ipsec_isakmp() ::"
                              "ISAKMP packet droppped from remote host 0x%08x", param->tuple.public_addr);
            return -1;
        }

        /* Save the responder cookie if needed */
        if (IP_U64_GETHI(trans->resp_cookie) == 0 && IP_U64_GETLO(trans->resp_cookie) == 0)
            IP_U64_COPY(trans->resp_cookie, resp_cookie);

        ipnet_nat_proxy_ipsec_isakmp_cleanup_list(&init_cookie,
                                                  &resp_cookie,
                                                  trans->local_address,
                                                  param->tuple.public_addr);

        /* Reset the translation entry timer */
        ipnet_nat_proxy_timeout_reschedule(IPNET_NAT_ISAKMP_ESTABLISHED_TIMEOUT,
                                           ipnet_nat_proxy_isakmp_translation_timeout,
                                           trans,
                                           &trans->tmo);

        /* Update the mapping timeout */
        ipnet_nat_proxy_set_mapping_timeout(IPNET_NAT_ISAKMP_ESTABLISHED_TIMEOUT, param->mapping);

        /* Translate the destination address.... */
        iph = (Ipcom_ip_header *)newhdr;
        IP_SET_HTONL(&iph->dst, trans->local_address);
        return 1;
    }
}


/*
 ****************************************************************************
 * 10                    GLOBAL FUNCTIONS
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 11                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipnet_nat_proxy_ipsec
 *===========================================================================
 * Description: NAT proxy for the IPSEC protocol.
 * Parameters:  newhdr    - pointer to optionally write new IP header.
 *              appdata   - pointer to application data.
 *              applen    - pointer to length of application data.
 *              growspace - space available to extend application data.
 *              param     - pointer to proxy parameters.
 *              newdata   - pointer to pointer to new application data.
 * Returns:     1 = Packet modified.
 *              0 = Packet untouched.
 *             -1 = Drop packet.
 */
IP_PUBLIC int
ipnet_nat_proxy_ipsec(Ip_u8 *newhdr,
                      Ip_u8 *appdata,
                      int   *applen,
                      int    growspace,
                      Ipnet_nat_proxy_param *param,
                      Ip_u8 **newdata)
{
    IP_STATIC int init = 0;

    (void)growspace;
    (void)newdata;

    /* Cannot handle fragments other than the first */
    if (param->fragoff != 0)
        return 0;

    /* IKE/ESP proxy does not apply to NAT-PT or redirect rules */
    if(param->natpt == IP_TRUE || param->inbound == IP_TRUE)
        return 0;

    if (!init)
    {
        ipcom_list_init(&ipnet_nat_proxy_ipsec_esp_list);
        ipcom_list_init(&ipnet_nat_proxy_ipsec_isakmp_list);
        init = 1;
    }
    if (param->tuple.protocol == IP_IPPROTO_ESP)
        return ipnet_nat_proxy_ipsec_esp(newhdr, appdata, *applen, param);
    else
        return ipnet_nat_proxy_ipsec_isakmp(newhdr, appdata, *applen, param);
}

#else
int ipnet_nat_ipsecproxy_empty_file;
#endif /* IPNET_USE_NAT */


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
