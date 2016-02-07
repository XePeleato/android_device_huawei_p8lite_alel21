/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_nat_pptpproxy.c,v $ $Revision: 1.11 $
 *     $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_nat_pptpproxy.c,v $
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
 * Proxy (ALG) for the PPTP/GRE protocol.
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

#define IPNET_NAT_PPTP_GRE_VERSION      1
#define IPNET_NAT_PPTP_GRE_PROTOCOL     0x880B

#define IPNET_NAT_PPTP_CONTROL_PACKET_TYPE      1
#define IPNET_NAT_PPTP_MAGIC_COOKIE             0x1a2b3c4d

/* PPTP control message types */
#define IPNET_NAT_PPTP_START_SESSION_REQUEST     1
#define IPNET_NAT_PPTP_START_SESSION_REPLY       2
#define IPNET_NAT_PPTP_STOP_SESSION_REQUEST      3
#define IPNET_NAT_PPTP_STOP_SESSION_REPLY        4
#define IPNET_NAT_PPTP_ECHO_REQUEST              5
#define IPNET_NAT_PPTP_ECHO_REPLY                6
#define IPNET_NAT_PPTP_OUT_CALL_REQUEST          7
#define IPNET_NAT_PPTP_OUT_CALL_REPLY            8
#define IPNET_NAT_PPTP_IN_CALL_REQUEST           9
#define IPNET_NAT_PPTP_IN_CALL_REPLY             10
#define IPNET_NAT_PPTP_INCOMING_CALL_CONNECTED   11
#define IPNET_NAT_PPTP_CALL_CLEAR_REQUEST        12
#define IPNET_NAT_PPTP_CALL_DISCONNECT_NOTIFY    13
#define IPNET_NAT_PPTP_WAN_ERROR_NOTIFY          14
#define IPNET_NAT_PPTP_SET_LINK_INFO             15

/* Constants used to generate spoofed call ids for outgoing translation */
#define IPNET_NAT_PPTP_LOWER_SPOOFED_CALL_ID_VALUE      0x8000
#define IPNET_NAT_PPTP_UPPER_SPOOFED_CALL_ID_VALUE      0xFFFE
#define IPNET_NAT_PPTP_FAKE_SPOOFED_CALL_ID_MASK        0x7FFF

/* PPTP timeouts in seconds */
#define IPNET_NAT_PPTP_NATED_CLIENT_TIMEOUT     30*60
#define IPNET_NAT_PPTP_NATED_SERVER_TIMEOUT     2*60
#define IPNET_NAT_PPTP_DATA_INIT_TIMEOUT        2*60
#define IPNET_NAT_PPTP_QUICK_TIMEOUT            30

/* Maximum number of PPTP translations */
#define IPNET_NAT_PPTP_TRANS_MAX_ENTRIES        128


/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

/*
 *===========================================================================
 *                         Ipnet_nat_gre_hdr
 *===========================================================================
 */
#include <ipcom_align16.h>
typedef IP_PACKED1 struct Ipnet_nat_gre_hdr_struct
{
    Ip_u16 version;         /* Version in the 3 lowest order bits */
    Ip_u16 protocol_type;
    Ip_u16 payload_length;
    Ip_u16 call_id;
}
IP_PACKED2 Ipnet_nat_gre_hdr;


/*
 *===========================================================================
 *                         Ipnet_nat_pptp_message_hdr
 *===========================================================================
 */
typedef IP_PACKED1 struct Ipnet_nat_pptp_message_hdr_struct
{
    Ip_u16 length;
    Ip_u16 type;
    Ip_u32 magic_cookie;
}
IP_PACKED2 Ipnet_nat_pptp_message_hdr;


/*
 *===========================================================================
 *                         Ipnet_nat_pptp_control_hdr
 *===========================================================================
 */
typedef IP_PACKED1 struct Ipnet_nat_pptp_control_hdr_struct
{
    Ip_u16 message_type;
    Ip_u16 reserved_0;
}
IP_PACKED2 Ipnet_nat_pptp_control_hdr;


/*
 *===========================================================================
 *                         Ipnet_nat_pptp_outgoing_call_request
 *===========================================================================
 */
typedef IP_PACKED1 struct Ipnet_nat_pptp_outgoing_call_request_struct
{
    Ip_u16 call_id;
    Ip_u16 call_serial_number;
    Ip_u32 minimum_bits_per_sec;
    Ip_u32 maximum_bits_per_sec;
    Ip_u32 bearer_type;
    Ip_u32 framing_type;
    Ip_u16 packet_receive_window_size;
    Ip_u16 packet_processing_delay;
    Ip_u8  phone_number[64];
    Ip_u8  sub_address[64];
}
IP_PACKED2 Ipnet_nat_pptp_outgoing_call_request;


/*
 *===========================================================================
 *                         Ipnet_nat_pptp_outgoing_call_reply
 *===========================================================================
 */
typedef IP_PACKED1 struct Ipnet_nat_pptp_outgoing_call_reply_struct
{
    Ip_u16 call_id;
    Ip_u16 peers_call_id;
    Ip_u8  result_code;
    Ip_u8  error_code;
    Ip_u16 cause_code;
    Ip_u32 connect_speed;
    Ip_u16 packet_receive_window_size;
    Ip_u16 packet_processing_delay;
    Ip_u32 physical_channel_id;
}
IP_PACKED2 Ipnet_nat_pptp_outgoing_call_reply;


/*
 *===========================================================================
 *                         Ipnet_nat_pptp_incoming_call_request
 *===========================================================================
 */
typedef IP_PACKED1 struct Ipnet_nat_pptp_incoming_call_request_struct
{
    Ip_u16 call_id;
    Ip_u16 call_serial_number;
    Ip_u32 call_bearer_type;
    Ip_u32 physical_channel_id;
    Ip_u16 dialed_number_length;
    Ip_u16 dialing_number_length;
    Ip_u8  dialed_number[64];
    Ip_u8  dialing_number[64];
    Ip_u8  sub_address[64];
}
IP_PACKED2 Ipnet_nat_pptp_incoming_call_request;


/*
 *===========================================================================
 *                         Ipnet_nat_pptp_incoming_call_reply
 *===========================================================================
 */
typedef IP_PACKED1 struct Ipnet_nat_pptp_incoming_call_reply_struct
{
    Ip_u16 call_id;
    Ip_u16 peers_call_id;
    Ip_u8  result_code;
    Ip_u8  error_code;
    Ip_u16 packet_receive_window_size;
    Ip_u16 packet_transmit_delay;
    Ip_u16 reserved_1;
}
IP_PACKED2 Ipnet_nat_pptp_incoming_call_reply;


/*
 *===========================================================================
 *                         Ipnet_nat_pptp_call_clear_request
 *===========================================================================
 */
typedef IP_PACKED1 struct Ipnet_nat_pptp_call_clear_request_struct
{
    Ip_u16 call_id;
    Ip_u16 reserved_1;
}
IP_PACKED2 Ipnet_nat_pptp_call_clear_request;


/*
 *===========================================================================
 *                         Ipnet_nat_pptp_call_disconnect_notify
 *===========================================================================
 */
typedef IP_PACKED1 struct Ipnet_nat_pptp_call_disconnect_notify_struct
{
    Ip_u16 call_id;
    Ip_u8  result_code;
    Ip_u8  error_code;
    Ip_u16 cause_code;
    Ip_u16 reserved_1;
    Ip_u8  call_statistics[128];
}
IP_PACKED2 Ipnet_nat_pptp_call_disconnect_notify;


/*
 *===========================================================================
 *                         Ipnet_nat_wan_error_notify
 *===========================================================================
 */
typedef IP_PACKED1 struct Ipnet_nat_wan_error_notify_struct
{
    Ip_u16 peers_call_id;
    Ip_u16 reserved_1;
    Ip_u32 crc_errors;
    Ip_u32 framing_errors;
    Ip_u32 hardware_overruns;
    Ip_u32 buffer_overruns;
    Ip_u32 timeout_errors;
    Ip_u32 alignment_errors;
}
IP_PACKED2 Ipnet_nat_wan_error_notify;


/*
 *===========================================================================
 *                         Ipnet_nat_pptp_set_link_info
 *===========================================================================
 */
typedef IP_PACKED1 struct Ipnet_nat_pptp_set_link_info_struct
{
    Ip_u16 peers_call_id;
    Ip_u16 reserved_1;
    Ip_u32 send_accm_value;
    Ip_u32 receive_accm_value;
}
IP_PACKED2 Ipnet_nat_pptp_set_link_info;


/*
 *===========================================================================
 *                         Ipnet_nat_pptp_message
 *===========================================================================
 */
typedef union Ipnet_nat_pptp_message_union
{
    Ipnet_nat_pptp_outgoing_call_request  outgoing_call_request;
    Ipnet_nat_pptp_outgoing_call_reply    outgoing_call_reply;
    Ipnet_nat_pptp_incoming_call_request  incoming_call_request;
    Ipnet_nat_pptp_incoming_call_reply    incoming_call_reply;
    Ipnet_nat_pptp_call_clear_request     call_clear_request;
    Ipnet_nat_pptp_call_disconnect_notify call_disconnect_notify;
    Ipnet_nat_wan_error_notify            wan_error_notify;
    Ipnet_nat_pptp_set_link_info          set_link_info;
}
IP_PACKED2 Ipnet_nat_pptp_message;


/*
 *===========================================================================
 *                         Ipnet_nat_pptp_control_packet
 *===========================================================================
 */
typedef IP_PACKED1 struct Ipnet_nat_pptp_control_packet_struct
{
    Ipnet_nat_pptp_message_hdr pptp_header;
    Ipnet_nat_pptp_control_hdr control_header;
    Ipnet_nat_pptp_message     message;
}
IP_PACKED2 Ipnet_nat_pptp_control_packet;
#include <ipcom_align16.h>


/*
 *===========================================================================
 *                         Ipnet_nat_pptp_trans
 *===========================================================================
 */
typedef struct Ipnet_nat_pptp_trans_struct
{
    Ipcom_list list;            /* list pointer */
    Ip_u32     local_address;
    Ip_u32     remote_address;
    Ip_bool    nated_client;
    Ip_u16     call_id;
    Ip_u16     spoofed_call_id;
    Ip_u16     peers_call_id;
    void       *tmo;            /* pointer to timeout */
}
Ipnet_nat_pptp_trans;


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

IP_STATIC Ipcom_list ipnet_nat_proxy_pptp_list;


/*
 ****************************************************************************
 * 9                   LOCAL FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipnet_nat_proxy_pptp_messsage_type
 *===========================================================================
 * Description: Convert a PPTP message control code to a string
 * Parameters:  code - the control code.
 * Returns:     A string with a human readable control code.
 */
#ifdef IPCOM_USE_SYSLOG
IP_STATIC char *
ipnet_nat_proxy_pptp_messsage_type(int code)
{
    switch (code)
    {
        case IPNET_NAT_PPTP_START_SESSION_REQUEST:
            return "Start-session-request";
        case IPNET_NAT_PPTP_START_SESSION_REPLY:
            return "Start-session-reply";
        case IPNET_NAT_PPTP_STOP_SESSION_REQUEST:
            return "Stop-session-request";
        case IPNET_NAT_PPTP_STOP_SESSION_REPLY:
            return "Stop-session-reply";
        case IPNET_NAT_PPTP_ECHO_REQUEST:
            return "Echo-request";
        case IPNET_NAT_PPTP_ECHO_REPLY:
            return "Echo-reply";
        case IPNET_NAT_PPTP_OUT_CALL_REQUEST:
            return "Out-call-request";
        case IPNET_NAT_PPTP_OUT_CALL_REPLY:
            return "Out-call-reply";
        case IPNET_NAT_PPTP_IN_CALL_REQUEST:
            return "In-call-request";
        case IPNET_NAT_PPTP_IN_CALL_REPLY:
            return "In-call-reply";
        case IPNET_NAT_PPTP_INCOMING_CALL_CONNECTED:
            return "Incoming-call-connected";
        case IPNET_NAT_PPTP_CALL_CLEAR_REQUEST:
            return "Call-clear-request";
        case IPNET_NAT_PPTP_CALL_DISCONNECT_NOTIFY:
            return "Call-disconnect-notify";
        case IPNET_NAT_PPTP_WAN_ERROR_NOTIFY:
            return "Wan-error-notify";
        case IPNET_NAT_PPTP_SET_LINK_INFO:
            return "Set-link-info";
        default:
            return "Unknown-pptp-control-code";
    }
}
#endif /* #ifdef IPCOM_USE_SYSLOG */


/*
 *===========================================================================
 *                    ipnet_nat_proxy_pptp_translation_timeout
 *===========================================================================
 * Description: Timeout handler for PPTP translation timeouts.
 * Parameters:  cookie - a cookie that is set when the timeout is added.
 * Returns:     nothing.
 */
IP_STATIC void
ipnet_nat_proxy_pptp_translation_timeout(void *cookie)
{
    Ipnet_nat_pptp_trans *trans = cookie;

    IPCOM_LOG5(DEBUG, "ipnet_nat_proxy_pptp_translation_timeout() :: expired translation:\n"
                      "local addr = 0x%08x, remote addr = 0x%08x\n"
                      "call id = 0x%04x spoofed call id = 0x%04x peers_call_id = 0x%04x",
                       trans->local_address, trans->remote_address,
                       trans->call_id, trans->spoofed_call_id, trans->peers_call_id);

    ipcom_list_remove(&trans->list);
    ipcom_free(trans);
}


/*
 *===========================================================================
 *                    ipnet_nat_proxy_pptp_find_outbound_entry_peer
 *===========================================================================
 * Description: Look for matching outbound PPTP entry with peer call id
 * Parameters:  local_address  - private side address.
 *              remote_address - public side address.
 *              call_id        - call id.
 * Returns:     pointer to the translation entry or IP_NULL if not found.
 */
IP_STATIC Ipnet_nat_pptp_trans *
ipnet_nat_proxy_pptp_find_outbound_entry_peer(Ip_u32 local_address,
                                              Ip_u32 remote_address,
                                              Ip_u16 call_id)
{
    Ipnet_nat_pptp_trans *trans;

    for (trans = IPCOM_LIST_FIRST(&ipnet_nat_proxy_pptp_list);
         trans != IP_NULL;
         trans = IPCOM_LIST_NEXT(&trans->list))
    {
        if (trans->peers_call_id == call_id
            && trans->local_address == local_address
            && trans->remote_address == remote_address)
        {
            IPCOM_LOG5(DEBUG, "ipnet_nat_proxy_pptp_find_outbound_entry_peer() :: found match for:\n"
                              "local addr = 0x%08x, remote addr = 0x%08x\n"
                              "call id = 0x%04x spoofed call id = 0x%04x peers_call_id = 0x%04x",
                               trans->local_address, trans->remote_address,
                               trans->call_id, trans->spoofed_call_id, trans->peers_call_id);
            return trans;
        }
    }

    IPCOM_LOG3(DEBUG, "ipnet_nat_proxy_pptp_find_outbound_entry_peer() :: no match for:\n"
                      "local addr = 0x%08x, remote addr = 0x%08x peers_call_id = 0x%04x",
                       local_address, remote_address, call_id);
    return IP_NULL;
}


/*
 *===========================================================================
 *                    ipnet_nat_proxy_pptp_find_outbound_entry
 *===========================================================================
 * Description: Look for matching outbound PPTP entry with call id
 * Parameters:  local_address  - private side address.
 *              remote_address - public side address.
 *              call_id        - call id.
 * Returns:     pointer to the translation entry or IP_NULL if not found.
 */
IP_STATIC Ipnet_nat_pptp_trans *
ipnet_nat_proxy_pptp_find_outbound_entry(Ip_u32 local_address,
                                         Ip_u32 remote_address,
                                         Ip_u16 call_id)
{
    Ipnet_nat_pptp_trans *trans;

    for (trans = IPCOM_LIST_FIRST(&ipnet_nat_proxy_pptp_list);
         trans != IP_NULL;
         trans = IPCOM_LIST_NEXT(&trans->list))
    {
        if (trans->call_id == call_id
            && trans->local_address == local_address
            && trans->remote_address == remote_address)
        {
            IPCOM_LOG5(DEBUG, "ipnet_nat_proxy_pptp_find_outbound_entry() :: found match for:\n"
                              "local addr = 0x%08x, remote addr = 0x%08x\n"
                              "call id = 0x%04x spoofed call id = 0x%04x peers_call_id = 0x%04x",
                               trans->local_address, trans->remote_address,
                               trans->call_id, trans->spoofed_call_id, trans->peers_call_id);
            return trans;
        }
    }

    IPCOM_LOG3(DEBUG, "ipnet_nat_proxy_pptp_find_outbound_entry() :: no match for:\n"
                      "local addr = 0x%08x, remote addr = 0x%08x call_id = 0x%04x",
                       local_address, remote_address, call_id);
    return IP_NULL;
}


/*
 *===========================================================================
 *                    ipnet_nat_proxy_pptp_find_outbound_entry
 *===========================================================================
 * Description: Look for matching inbound PPTP entry with call id
 * Parameters:  remote_address - public side address.
 *              call_id        - call id.
 * Returns:     pointer to the translation entry or IP_NULL if not found.
 */
IP_STATIC Ipnet_nat_pptp_trans *
ipnet_nat_proxy_pptp_find_inbound_entry(Ip_u32 remote_address,
                                        Ip_u16 call_id)
{
    Ipnet_nat_pptp_trans *trans;

    /* Look up by spoofed call ID and remote address (NAT'ed client) */
    for (trans = IPCOM_LIST_FIRST(&ipnet_nat_proxy_pptp_list);
         trans != IP_NULL;
         trans = IPCOM_LIST_NEXT(&trans->list))
    {
        if (trans->spoofed_call_id == call_id
            && trans->remote_address == remote_address)
        {
            IPCOM_LOG5(DEBUG, "ipnet_nat_proxy_pptp_find_inbound_entry() :: found match for:\n"
                              "local addr = 0x%08x, remote addr = 0x%08x\n"
                              "call id = 0x%04x spoofed call id = 0x%04x peers_call_id = 0x%04x",
                               trans->local_address, trans->remote_address,
                               trans->call_id, trans->spoofed_call_id, trans->peers_call_id);
            return trans;
        }
    }

    IPCOM_LOG2(DEBUG, "ipnet_nat_proxy_pptp_find_inbound_entry() :: no match for:\n"
                      "remote addr = 0x%08x call_id = 0x%04x",
                       remote_address, call_id);
    return IP_NULL;
}


/*
 *===========================================================================
 *                    ipnet_nat_proxy_pptp_new
 *===========================================================================
 * Description: Create a new PPTP translation entry and add it to the list.
 * Parameters:  call_id         - call id.
 *              spoofed_call_id - spoofed call id.
 *              nated_client    - IP_TRUE for client.
 *              local_address   - private side address.
 *              remote_address  - public side address.
 * Returns:     pointer to the translation entry or IP_NULL if failed.
 */
IP_STATIC Ipnet_nat_pptp_trans *
ipnet_nat_proxy_pptp_new(Ip_u16 call_id,
                         Ip_u16 spoofed_call_id,
                         Ip_bool nated_client,
                         Ip_u32 local_address,
                         Ip_u32 remote_address,
                         void   *mapping)
{
    Ipnet_nat_pptp_trans *trans;

    /* Check that there is room for another translation entry */
    if(ipnet_nat_proxy_pptp_list.size >= IPNET_NAT_PPTP_TRANS_MAX_ENTRIES)
    {
        IPCOM_LOG0(ERR, "ipnet_nat_proxy_pptp_new() :: failed - too many translations");
        return IP_NULL;
    }
    trans = ipcom_malloc(sizeof(*trans));
    if (trans == IP_NULL)
    {
        IPCOM_LOG0(ERR, "ipnet_nat_proxy_pptp_new() ::"
                        "failed to allocate memory for translation entry");
        return IP_NULL;
    }
    ipcom_memset(trans, 0, sizeof(*trans));

    trans->call_id         = call_id;
    trans->spoofed_call_id = spoofed_call_id;
    trans->peers_call_id   = 0;
    trans->nated_client    = nated_client;
    trans->local_address   = local_address;
    trans->remote_address  = remote_address;
    ipcom_list_insert_last(&ipnet_nat_proxy_pptp_list, &trans->list);
    /* Add the timeout */
    if (ipnet_nat_proxy_timeout_schedule(IPNET_NAT_PPTP_DATA_INIT_TIMEOUT,
                                         ipnet_nat_proxy_pptp_translation_timeout,
                                         trans,
                                         &trans->tmo) < 0)
    {
        ipcom_list_remove(&trans->list);
        ipcom_free(trans);
        return IP_NULL;
    }

    /* Update the mapping timeout */
    ipnet_nat_proxy_set_mapping_timeout(IPNET_NAT_PPTP_DATA_INIT_TIMEOUT, mapping);

    IPCOM_LOG4 (DEBUG, "ipnet_nat_proxy_pptp_new() :: "
                       "local addr = 0x%08x, remote addr = 0x%08x\n"
                       "call id = 0x%04x spoofed call id = 0x%04x",
                        local_address, remote_address,
                        call_id, spoofed_call_id);
    return trans;
}


/*
 *===========================================================================
 *                    ipnet_nat_proxy_pptp_new_spoofed_call_id
 *===========================================================================
 * Description: Generate new spoofed call id.
 * Parameters:  none.
 * Returns:     spoofed call id.
 */
IP_STATIC Ip_u16
ipnet_nat_proxy_pptp_new_spoofed_call_id(void)
{
    Ipnet_nat_pptp_trans *trans;
    Ip_bool conflict;
    Ip_bool first_pass;
    IP_STATIC Ip_u16 next_pptp_spoofed_call_id = IPNET_NAT_PPTP_LOWER_SPOOFED_CALL_ID_VALUE;

    first_pass = IP_FALSE;

    for (next_pptp_spoofed_call_id = next_pptp_spoofed_call_id + 1;
         ;
         next_pptp_spoofed_call_id = next_pptp_spoofed_call_id + 1)
    {
        if (next_pptp_spoofed_call_id > IPNET_NAT_PPTP_UPPER_SPOOFED_CALL_ID_VALUE)
        {
            next_pptp_spoofed_call_id = IPNET_NAT_PPTP_LOWER_SPOOFED_CALL_ID_VALUE;

            if (first_pass == IP_FALSE)
                first_pass = IP_TRUE;
            else
                return IPNET_NAT_PPTP_UPPER_SPOOFED_CALL_ID_VALUE + 1;
        }

        conflict = IP_FALSE;

        for (trans = IPCOM_LIST_FIRST(&ipnet_nat_proxy_pptp_list);
             trans != IP_NULL;
             trans = IPCOM_LIST_NEXT(&trans->list))
        {
            if (trans->spoofed_call_id == next_pptp_spoofed_call_id)
            {
                conflict = IP_TRUE;
                break;
            }
        }

        if (conflict == IP_TRUE)
            continue;
        return next_pptp_spoofed_call_id;
    }
}


/*
 *===========================================================================
 *                    ipnet_nat_proxy_pptp_gre
 *===========================================================================
 * Description: Track GRE packets and forward responses to inner host.
 * Parameters:  newhdr    - pointer to optionally write new IP header.
 *              appdata   - pointer to application data.
 *              applen    - pointer to length of application data.
 *              param     - pointer to proxy parameters.
 * Returns:     1 = Packet modified.
 *              0 = Packet untouched.
 *             -1 = Drop packet.
 */
IP_STATIC int
ipnet_nat_proxy_pptp_gre(Ip_u8 *newhdr,
                         Ip_u8 *appdata,
                         int    applen,
                         Ipnet_nat_proxy_param *param)
{
    Ipcom_ip_header      *iph;
    Ipnet_nat_gre_hdr    *gre;
    Ipnet_nat_pptp_trans *trans;
    Ip_u16 call_id;
    Ip_u32 address;

    /* If it's not long enough to full GRE header, give up...
     * The length field is already converted to the host byte order
     */
    if (applen < (int)sizeof(*gre))
    {
        IPCOM_LOG0(WARNING, "ipnet_nat_proxy_pptp_gre() :: message too short for GRE header");
        return 0;
    }
    gre     = (Ipnet_nat_gre_hdr *)appdata;
    call_id = IP_GET_NTOHS(&gre->call_id);

    /* If it isn't a PPTP GRE packet, give up..... */
    if ((IP_GET_NTOHS(&gre->version) & 0x0007) != IPNET_NAT_PPTP_GRE_VERSION ||
        IP_GET_NTOHS(&gre->protocol_type) != IPNET_NAT_PPTP_GRE_PROTOCOL)
    {
        IPCOM_LOG0(DEBUG, "ipnet_nat_proxy_pptp_gre() :: not a PPTP GRE packet");
        return 0;
    }

    if (param->incoming == IP_FALSE)
    {
        /* Try to find translation entry */
        trans = ipnet_nat_proxy_pptp_find_outbound_entry_peer(param->tuple.private_addr,
                                                              param->tuple.public_addr,
                                                              call_id);
        /* No entry found....toss it */
        if (trans == IP_NULL)
        {
            IPCOM_LOG1(DEBUG, "ipnet_nat_proxy_pptp_gre() ::"
                              "GRE packet dropped from local host 0x%08x", param->tuple.private_addr);
            return -1;
        }

        ipnet_nat_proxy_timeout_reschedule(IPNET_NAT_PPTP_NATED_CLIENT_TIMEOUT,
                                           ipnet_nat_proxy_pptp_translation_timeout,
                                           trans,
                                           &trans->tmo);
        /* Update the mapping timeout */
        ipnet_nat_proxy_set_mapping_timeout(IPNET_NAT_PPTP_NATED_CLIENT_TIMEOUT, param->mapping);

        return 0;
    }
    else
    {
        /* Try to find translation entry */
        trans = ipnet_nat_proxy_pptp_find_inbound_entry(param->tuple.public_addr, call_id);

        /* No entry found....toss it */
        if (trans == IP_NULL)
        {
            IPCOM_LOG1(DEBUG, "ipnet_nat_proxy_pptp_gre() ::"
                              "GRE packet dropped from remote host 0x%08x", param->tuple.public_addr);
            return -1;
        }

        ipnet_nat_proxy_timeout_reschedule(IPNET_NAT_PPTP_NATED_CLIENT_TIMEOUT,
                                           ipnet_nat_proxy_pptp_translation_timeout,
                                           trans,
                                           &trans->tmo);
        /* Update the mapping timeout */
        ipnet_nat_proxy_set_mapping_timeout(IPNET_NAT_PPTP_NATED_CLIENT_TIMEOUT, param->mapping);

        IP_SET_HTONS(&gre->call_id, trans->call_id);

        /* Translate the destination address.... */
        address = trans->local_address;
        iph = (Ipcom_ip_header *)newhdr;
        IP_SET_HTONL(&iph->dst, address);
        return 1;
    }
}


/*
 *===========================================================================
 *                    ipnet_nat_proxy_pptp_control
 *===========================================================================
 * Description: Track PPTP packets and forward responses to inner host.
 * Parameters:  newhdr    - pointer to optionally write new IP header.
 *              appdata   - pointer to application data.
 *              applen    - pointer to length of application data.
 *              param     - pointer to proxy parameters.
 * Returns:     1 = Packet modified.
 *              0 = Packet untouched.
 *             -1 = Drop packet.
 */
IP_STATIC int
ipnet_nat_proxy_pptp_control(Ip_u8 *newhdr,
                             Ip_u8 *appdata,
                             int    applen,
                             Ipnet_nat_proxy_param *param)
{
    Ipnet_nat_pptp_control_packet *pptp;
    Ipnet_nat_pptp_trans          *trans;
    Ip_u16                        *call_id, *peers_call_id;
    Ip_u16                        spoofed_call_id, msg;
    Ipnet_nat_proxy_tuple         proxy_tuple;

    (void)newhdr;
    /* If it's not long enough to have TCP data, give up....
     * The length field is already converted to the host byte order
     */
    if (applen < ((int)sizeof(Ipnet_nat_pptp_message_hdr) + (int)sizeof(Ipnet_nat_pptp_control_hdr)))
    {
        IPCOM_LOG0(WARNING, "ipnet_nat_proxy_pptp_control() :: message too short for PPTP control header");
        return 0;
    }
    pptp = (Ipnet_nat_pptp_control_packet *)appdata;

    /* If it isn't a PPTP Control packet, give up..... */
    if (IP_GET_NTOHS(&pptp->pptp_header.type) != IPNET_NAT_PPTP_CONTROL_PACKET_TYPE ||
        IP_GET_NTOHL(&pptp->pptp_header.magic_cookie) != IPNET_NAT_PPTP_MAGIC_COOKIE)
        return 0;

    msg = IP_GET_NTOHS(&pptp->control_header.message_type);
    if (param->incoming == IP_FALSE)
    {
        IPCOM_LOG1(DEBUG, "ipnet_nat_proxy_pptp_control(out) :: %s", ipnet_nat_proxy_pptp_messsage_type(msg));
        if (msg == IPNET_NAT_PPTP_OUT_CALL_REQUEST || msg == IPNET_NAT_PPTP_IN_CALL_REQUEST)
        {
            if (msg == IPNET_NAT_PPTP_OUT_CALL_REQUEST)
                call_id = &pptp->message.outgoing_call_request.call_id;
            else
                call_id = &pptp->message.incoming_call_request.call_id;

            /* Create a new spoofed call id to use instead of the client's ID */
            spoofed_call_id = ipnet_nat_proxy_pptp_new_spoofed_call_id();

            trans = ipnet_nat_proxy_pptp_new(IP_GET_NTOHS(call_id),
                                             spoofed_call_id,
                                             IP_TRUE,
                                             param->tuple.private_addr,
                                             param->tuple.public_addr,
                                             param->mapping);

            /* Put the spoofed ID in the message and set the initial timeout... */
            if (trans != IP_NULL)
            {
                ipnet_nat_proxy_timeout_reschedule(IPNET_NAT_PPTP_DATA_INIT_TIMEOUT,
                                                   ipnet_nat_proxy_pptp_translation_timeout,
                                                   trans,
                                                   &trans->tmo);
                ipnet_nat_proxy_set_mapping_timeout(IPNET_NAT_PPTP_DATA_INIT_TIMEOUT, param->mapping);
                IP_SET_HTONS(call_id, spoofed_call_id);

                /* Add a mapping for GRE in case that the first GRE packet is inbound */
                ipcom_memset(&proxy_tuple, 0, sizeof(proxy_tuple));
                proxy_tuple.protocol     = IP_IPPROTO_GRE;
                proxy_tuple.private_addr = trans->local_address;
                proxy_tuple.public_addr  = trans->remote_address;
                (void)ipnet_nat_proxy_add_mapping(&proxy_tuple,
                                                  0,
                                                  param->mapping,
                                                  IP_FALSE,         /* NAT port = local source port */
                                                  IP_FALSE,
                                                  ipnet_nat_proxy_pptp,
                                                  IP_NULL);

                return 1;
            }
            else
            {
                return -1;
            }
        }
        else if (msg == IPNET_NAT_PPTP_CALL_CLEAR_REQUEST || msg == IPNET_NAT_PPTP_CALL_DISCONNECT_NOTIFY)
        {
            /* Get the call ID from the message and find the translation entry */
            if (msg == IPNET_NAT_PPTP_CALL_CLEAR_REQUEST)
            {
                spoofed_call_id = IP_GET_NTOHS(&pptp->message.call_clear_request.call_id);
                call_id         = &pptp->message.call_clear_request.call_id;
            }
            else
            {
                spoofed_call_id = IP_GET_NTOHS(&pptp->message.call_disconnect_notify.call_id);
                call_id         = &pptp->message.call_disconnect_notify.call_id;
            }

            trans = ipnet_nat_proxy_pptp_find_outbound_entry(param->tuple.private_addr,
                                                             param->tuple.public_addr,
                                                             spoofed_call_id);

            /* If we found one, then set the call id's to the spoofed ID */
            if (trans != IP_NULL)
            {
                if (msg == IPNET_NAT_PPTP_CALL_CLEAR_REQUEST)
                {
                    /* Call should be done...set to time out more quickly...or reinited if not */
                    ipnet_nat_proxy_timeout_reschedule(IPNET_NAT_PPTP_QUICK_TIMEOUT,
                                                       ipnet_nat_proxy_pptp_translation_timeout,
                                                       trans,
                                                       &trans->tmo);
                    ipnet_nat_proxy_set_mapping_timeout(IPNET_NAT_PPTP_QUICK_TIMEOUT, param->mapping);
                }
                else
                {
                    /* Calls done (due to disconnect), time out the translation entry quickly */
                    ipnet_nat_proxy_timeout_reschedule(IPNET_NAT_PPTP_QUICK_TIMEOUT,
                                                       ipnet_nat_proxy_pptp_translation_timeout,
                                                       trans,
                                                       &trans->tmo);
                    ipnet_nat_proxy_set_mapping_timeout(IPNET_NAT_PPTP_QUICK_TIMEOUT, param->mapping);
                    trans->nated_client = IP_FALSE;
                }
                spoofed_call_id = trans->spoofed_call_id;
            }

            IP_SET_HTONS(call_id, spoofed_call_id);
            return 1;
        }

        return 0;
    }
    else
    {
        IPCOM_LOG1(DEBUG, "ipnet_nat_proxy_pptp_control(in) :: %s", ipnet_nat_proxy_pptp_messsage_type(msg));
        /* Get the call ID and peers call ID from the message ... */
        switch(msg)
        {
            case IPNET_NAT_PPTP_OUT_CALL_REPLY:
                spoofed_call_id = IP_GET_NTOHS(&pptp->message.outgoing_call_reply.peers_call_id);
                peers_call_id   = &pptp->message.outgoing_call_reply.peers_call_id;
                break;
            case IPNET_NAT_PPTP_IN_CALL_REPLY:
                spoofed_call_id = IP_GET_NTOHS(&pptp->message.outgoing_call_reply.peers_call_id);
                peers_call_id   = &pptp->message.outgoing_call_reply.peers_call_id;
                break;
            case IPNET_NAT_PPTP_WAN_ERROR_NOTIFY:
                spoofed_call_id = IP_GET_NTOHS(&pptp->message.wan_error_notify.peers_call_id);
                peers_call_id   = &pptp->message.wan_error_notify.peers_call_id;
                break;
            case IPNET_NAT_PPTP_SET_LINK_INFO:
                spoofed_call_id = IP_GET_NTOHS(&pptp->message.set_link_info.peers_call_id);
                peers_call_id   = &pptp->message.set_link_info.peers_call_id;
                break;
            default:
                /* Do not drop. Packet will find its way to the private host since this is TCP */
                return 0;
        }

        /* Find the translation entry */
        trans = ipnet_nat_proxy_pptp_find_inbound_entry(param->tuple.public_addr, spoofed_call_id);
        if (trans != IP_NULL)
        {
            if (msg == IPNET_NAT_PPTP_OUT_CALL_REPLY || msg == IPNET_NAT_PPTP_IN_CALL_REPLY)
            {
                /* Store the peers call ID, and set the initial timer */
                if (trans->peers_call_id != IP_GET_NTOHS(&pptp->message.outgoing_call_reply.call_id))
                {
                    trans->peers_call_id = IP_GET_NTOHS(&pptp->message.outgoing_call_reply.call_id);
                    ipnet_nat_proxy_timeout_reschedule(IPNET_NAT_PPTP_DATA_INIT_TIMEOUT,
                                                       ipnet_nat_proxy_pptp_translation_timeout,
                                                       trans,
                                                       &trans->tmo);
                    ipnet_nat_proxy_set_mapping_timeout(IPNET_NAT_PPTP_DATA_INIT_TIMEOUT, param->mapping);
                }
            }

            IP_SET_HTONS(peers_call_id, trans->call_id);
            return 1;
        }
        else
        {
            /* Do not drop. Packet will find its way to the private host since this is TCP */
            return 0;
        }
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
 *                    ipnet_nat_proxy_pptp
 *===========================================================================
 * Description: NAT proxy for the PPTP protocol.
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
ipnet_nat_proxy_pptp(Ip_u8 *newhdr,
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

    /* PPTP/GRE proxy does not apply to NAT-PT or redirect rules */
    if(param->natpt == IP_TRUE || param->inbound == IP_TRUE)
        return 0;

    if (!init)
    {
        ipcom_list_init(&ipnet_nat_proxy_pptp_list);
        init = 1;
    }
    if (param->tuple.protocol == IP_IPPROTO_GRE)
        return ipnet_nat_proxy_pptp_gre(newhdr, appdata, *applen, param);
    else if (*applen > 0)
        return ipnet_nat_proxy_pptp_control(newhdr, appdata, *applen, param);

    return 0;
}

#else
int ipnet_nat_pptpproxy_empty_file;
#endif /* IPNET_USE_NAT */


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
