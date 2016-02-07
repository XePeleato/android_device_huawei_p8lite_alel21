/* ipdhcpc.h - API of DHCP client */

/*
 ******************************************************************************
 *                     HEADER FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipdhcpc.h,v $ $Revision: 1.27 $
 *   $Source: /home/interpeak/CVSRoot/ipappl/include/ipdhcpc.h,v $
 *   $Author: swebster $ $Date: 2009-01-19 19:49:13 $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Lennart Bang <lob@interpeak.se>
 *           and Jonas Rendel <jonas@interpeak.se>
 ******************************************************************************
 */

#ifndef IPDHCPC_H
#define IPDHCPC_H

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 */

/*
DESCRIPTION
This library contains the API for the DHCP client
*/

/*
 ****************************************************************************
 * 2                    CONFIGURATION
 ****************************************************************************
 */


/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#include <ipcom_cstyle.h>
#include <ipcom_tmo.h>
#include <ipcom_tags.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

/*
 *===========================================================================
 *                         DHCP option codes - auto
 *===========================================================================
 * Option codes handled transparantly by IPDHCPC, i.e. user code does not
 * need to add/remove/modify these options.
 */
#define IPDHCPC_OPTCODE_DHCP_MESSAGE_TYPE            53
#define IPDHCPC_OPTCODE_DHCP_SERVER_IDENTIFIER       54
#define IPDHCPC_OPTCODE_DHCP_MAX_MESSAGE_SIZE        57
#define IPDHCPC_OPTCODE_DHCP_CLIENT_IDENTIFIER       61
#define IPDHCPC_OPTCODE_END                         255


/*
 *===========================================================================
 *                         DHCP option codes - used
 *===========================================================================
 * Option codes used by IPDHCPC in some way or another.
 */

#define IPDHCPC_OPTCODE_PAD                           0
#define IPDHCPC_OPTCODE_SUBNET_MASK                   1
#define IPDHCPC_OPTCODE_ROUTERS                       3
#define IPDHCPC_OPTCODE_DOMAIN_NAME_SERVERS           6
#define IPDHCPC_OPTCODE_DOMAIN_NAME                  15
#define IPDHCPC_OPTCODE_NTP_SERVERS                  42
#define IPDHCPC_OPTCODE_DHCP_LEASE_TIME              51
#define IPDHCPC_OPTCODE_OPTION_OVERLOAD              52
#define IPDHCPC_OPTCODE_DHCP_PARAMETER_REQUEST_LIST  55
#define IPDHCPC_OPTCODE_DHCP_MESSAGE                 56
#define IPDHCPC_OPTCODE_DHCP_RENEWAL_TIME            58
#define IPDHCPC_OPTCODE_DHCP_REBINDING_TIME          59


/*
 *===========================================================================
 *                         DHCP option codes - user
 *===========================================================================
 * Option codes not read or written by IPDHCPC.
 */
#define IPDHCPC_OPTCODE_TIME_OFFSET                   2
#define IPDHCPC_OPTCODE_TIME_SERVERS                  4
#define IPDHCPC_OPTCODE_NAME_SERVERS                  5
#define IPDHCPC_OPTCODE_LOG_SERVERS                   7
#define IPDHCPC_OPTCODE_COOKIE_SERVERS                8
#define IPDHCPC_OPTCODE_LPR_SERVERS                   9
#define IPDHCPC_OPTCODE_IMPRESS_SERVERS              10
#define IPDHCPC_OPTCODE_RESOURCE_LOCATION_SERVERS    11
#define IPDHCPC_OPTCODE_HOST_NAME                    12
#define IPDHCPC_OPTCODE_BOOT_SIZE                    13
#define IPDHCPC_OPTCODE_MERIT_DUMP                   14
#define IPDHCPC_OPTCODE_SWAP_SERVER                  16
#define IPDHCPC_OPTCODE_ROOT_PATH                    17
#define IPDHCPC_OPTCODE_EXTENSIONS_PATH              18
#define IPDHCPC_OPTCODE_IP_FORWARDING                19
#define IPDHCPC_OPTCODE_NON_LOCAL_SOURCE_ROUTING     20
#define IPDHCPC_OPTCODE_POLICY_FILTER                21
#define IPDHCPC_OPTCODE_MAX_DGRAM_REASSEMBLY         22
#define IPDHCPC_OPTCODE_DEFAULT_IP_TTL               23
#define IPDHCPC_OPTCODE_PATH_MTU_AGING_TIMEOUT       24
#define IPDHCPC_OPTCODE_PATH_MTU_PLATEAU_TABLE       25
#define IPDHCPC_OPTCODE_INTERFACE_MTU                26
#define IPDHCPC_OPTCODE_ALL_SUBNETS_LOCAL            27
#define IPDHCPC_OPTCODE_BROADCAST_ADDRESS            28
#define IPDHCPC_OPTCODE_PERFORM_MASK_DISCOVERY       29
#define IPDHCPC_OPTCODE_MASK_SUPPLIER                30
#define IPDHCPC_OPTCODE_ROUTER_DISCOVERY             31
#define IPDHCPC_OPTCODE_ROUTER_SOLICITATION_ADDRESS  32
#define IPDHCPC_OPTCODE_STATIC_ROUTES                33
#define IPDHCPC_OPTCODE_TRAILER_ENCAPSULATION        34
#define IPDHCPC_OPTCODE_ARP_CACHE_TIMEOUT            35
#define IPDHCPC_OPTCODE_IEEE802_3_ENCAPSULATION      36
#define IPDHCPC_OPTCODE_DEFAULT_TCP_TTL              37
#define IPDHCPC_OPTCODE_TCP_KEEPALIVE_INTERVAL       38
#define IPDHCPC_OPTCODE_TCP_KEEPALIVE_GARBAGE        39
#define IPDHCPC_OPTCODE_NIS_DOMAIN                   40
#define IPDHCPC_OPTCODE_NIS_SERVERS                  41
#define IPDHCPC_OPTCODE_VENDOR_ENCAPSULATED_OPTIONS  43
#define IPDHCPC_OPTCODE_NETBIOS_NAME_SERVERS         44
#define IPDHCPC_OPTCODE_NETBIOS_DD_SERVER            45
#define IPDHCPC_OPTCODE_NETBIOS_NODE_TYPE            46
#define IPDHCPC_OPTCODE_NETBIOS_SCOPE                47
#define IPDHCPC_OPTCODE_FONT_SERVERS                 48
#define IPDHCPC_OPTCODE_X_DISPLAY_MANAGER            49
#define IPDHCPC_OPTCODE_DHCP_REQUESTED_ADDRESS       50
#define IPDHCPC_OPTCODE_DHCP_OPTION_OVERLOAD         52
#define IPDHCPC_OPTCODE_VENDOR_CLASS_IDENTIFIER      60
#define IPDHCPC_OPTCODE_TFTP_SERVER_NAME             66
#define IPDHCPC_OPTCODE_BOOTFILE_NAME                67
#define IPDHCPC_OPTCODE_VI_VENDOR_CLASS             124
#define IPDHCPC_OPTCODE_VI_VENDOR_INFORMATION       125
#define IPDHCPC_OPTCODE_TFTP_SERVER_ADDRESS           0 /* Fake option code */
	
	
/*
 *===========================================================================
 *                         DHCP magic cookie
 *===========================================================================
 * See RFC 2132.
*/	

#define IPDHCPC_MAGIC_COOKIE                 0x63825363

	
/*
 *===========================================================================
 *                         DHCP reply callback event codes
 *===========================================================================
 * Possible events to be sent and handled by a DHCP client reply callback.
*/
	
#define IPDHCPC_REPLY_CB_EVENT_ACK           1  /* Received an ACK       */
#define IPDHCPC_REPLY_CB_EVENT_NAK           2  /* Received a NAK        */
#define IPDHCPC_REPLY_CB_EVENT_EXPIRE        3  /* Lease expiration      */
#define IPDHCPC_REPLY_CB_EVENT_FAIL_IFLOST   4  /* Lost the interface    */
#define IPDHCPC_REPLY_CB_EVENT_BOOTREPLY     5  /* Received a boot reply */
#define IPDHCPC_REPLY_CB_EVENT_OFFER         6  /* Received an offer     */
#define IPDHCPC_REPLY_CB_EVENT_FAIL_RETRY    7  /* Retries expired       */

/*
 *===========================================================================
 *                         DHCP reply callback return codes
 *===========================================================================
 * Possible return values to be returned by a DHCP client reply callback.
*/

#define IPDHCPC_REPLY_CB_ERROR              -1
#define IPDHCPC_REPLY_CB_SUCCESS             0	

/*
 *===========================================================================
 *                         DHCP mode of operation
 *===========================================================================
 * Defines the different modes the client can operate in.
 */

#define IPDHCPC_MODE_ALLOC_DYNAMIC           1
#define IPDHCPC_MODE_REUSE_DYNAMIC           2
#define IPDHCPC_MODE_RELEASE_DYNAMIC         3
#define IPDHCPC_MODE_INFORM_STATIC           4
	
/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

/* Reply callback for DHCP replies. */

struct Ipdhcpc_handle_priv_struct;
struct Ipdhcpc_handle_struct;	
typedef int (*Ipdhcpc_reply_cb)(struct Ipdhcpc_handle_struct *handle, int event);	
	
/*
 *===========================================================================
 *                         Ipdhcpc_info
 *===========================================================================
 * This structure is filled in by the IPDHCPC code upon valid DHCP client
 * request reply.
 */
typedef struct Ipdhcpc_info_struct
{
    Ip_err    status;        /* IPCOM_SUCCESS else IPCOM_ERR_XXX. */

    Ip_u32    ipaddr_n;      /* Client IP address (yiaddr). */
    Ip_u32    serveraddr_n;  /* Server IP in network endian. Default = 0 -> broadcast */
    Ip_u16    serverport_n;  /* Server UDP port in network endian. Default = ip_htons(67). */
    Ip_u16    reserved;

    int       optlen;        /* Options length excluding cookie. */
    Ip_u8     *options;    /* DHCP options */

    Ip_u8     sname[64];     /* Optional server host name, null terminated string. */

    Ip_u32    bootaddr_n;    /* IP address of next bootstrap server (siaddr). */
    Ip_u8     file[128];     /* Optional boot file name, null terminated string. */

    Ip_time_t t1;            /* Time at which we enter RENEWING state */
    Ip_time_t t2;            /* Time at which we enter REBINDING state */
}
Ipdhcpc_info;


/*
 *===========================================================================
 *                         Ipdhcpc_handle
 *===========================================================================
 * Created and initialized by ipdhcpc_handle_malloc() to prepare for a
 * DHCP client request. User may modify the handle before calling
 * ipdhcpc_ifrequest().
 *
 */
typedef struct Ipdhcpc_handle_struct
{
    Ip_u32    user_res[2];   /* Reserved for user use. IPDHCPC does not touch it. */

    int       ifindex;       /* DHCP interface index. Set by ipdhcpc_handle_malloc(). */

    Ip_u32    clientaddr_n;  /* Desired IP in network endian else 0. */
    Ip_u32    serveraddr_n;  /* Server IP in network endian. Default = 0 -> broadcast */
    Ip_u16    serverport_n;  /* Server UDP port in network endian. Default = ip_htons(67). */
    Ip_u16    reserved;
    Ip_u32    magic_cookie;  /* Magic vendor cookie. Default=0x63825363. */

    char      file[128];     /* Optional boot file name, null terminated string. */
    char      sname[64];     /* Optional server host name, null terminated string. */   
    
    /* User callback for the reply. Set to IP_NULL for blocking mode. */
    Ipdhcpc_reply_cb  reply_cb;

    int       optlen;        /* User options length. */
    Ip_u8     options[IPDHCPC_MAX_USER_OPT_SIZE];  /* Additional user options. */

    /***** IPDHCPC info for the user after succesful DHCP server reply. */
    Ipdhcpc_info   info;

    /***** IPDHCPC private info. do not read/write *****/
    struct Ipdhcpc_handle_priv_struct   *priv;
}
Ipdhcpc_handle;


/*
 *===========================================================================
 *                         Ipdhcpc_ifopen
 *===========================================================================
 */

typedef struct Ipdhcpc_ifopen_struct
{
  Ip_u32   api_version;   /* Currently version 1. Used for future compability. */

  char     clientid[IPDHCPC_CLIENT_ID_MAXSIZE+1];  /* must be null terminated! */

  Ip_tag   conf[1];   /* None available, included for future compability. */
}
Ipdhcpc_ifopen;


/*
 ****************************************************************************
 * 6                    FUNCTIONS
 ****************************************************************************
 */

/*******************************************************************************
*
* ipdhcpc_option_callback - apply user-defined actions based on DHCP option
*
* This callback routine examines each DHCP option returned by a DHCP server and
* can execute user-defined operations for specific options. For example, it can
* be implemented to apply a different operation to each of three vendor-specific
* options received from the server.
*
* The callback is executed before any other client action on the server\'s
* response and can be used to change the contents of the response before it is
* acted on further. For a sample implementation, see 'ipdhcpc_example.c'.
*
* Parameter:
* \is
* \i <option>
* A pointer to the starting location of an option, where:
*
* \ml
* \m -
* <option>[0] is a DHCP option code.
* \m -
* <option>[1] is the length of the option, in bytes, from the start of the
* option to the end of its data portion.
* \m -
* <option>[2] is the first element containing data for the option.
* \me
* \i <handle>
* A pointer to the request handle passed in ipdhcpc_ifrequest.
* \ie
*
* RETURNS: Either 'IP_TRUE', in which case the DHCP client can take further
* action on the option, or 'IP_FALSE', in which case the client takes no further
* action based on the option.
*
* ERRNO:
*/
IP_PUBLIC Ip_err ipdhcpc_option_callback
(
    Ipdhcpc_handle *handle,
    Ip_u8 *option
);

/*******************************************************************************
*
* ipdhcpc_reply_callback - apply user-defined actions based on DHCP reply
*
* This callback routine examines each DHCP reply returned by a DHCP server and
* can execute user-defined operations for specific replies. For example, it can
* be implemented to apply a different operation to NAK's, ACK's, Lease
* expiration, etc.
*
* The callback is executed before any other client action on the server\'s
* response and can be used to change the contents of the response before it is
* acted on further. For a sample implementation, see 'ipdhcpc_example.c'.
*
* Parameter:
* \is
* \i <handle>
* A pointer to the ipdhcpc handle
* 
* \is
* \i <event>
* The reply type
*
* RETURNS: Either 'IP_TRUE', in which case the DHCP client can take further
* action on the option, or 'IP_FALSE', in which case the client takes no further
* action based on the option.
*
* ERRNO:
*/
IP_PUBLIC Ip_err ipdhcpc_reply_callback
(
    Ipdhcpc_handle *handle,
    int event
);

/*
 *===========================================================================
 *                    ipdhcpc_ifopen
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL Ip_err ipdhcpc_ifopen(Ip_u32 ifindex, Ipdhcpc_ifopen *config);


/*
 *===========================================================================
 *                    ipdhcpc_ifclose
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL Ip_err ipdhcpc_ifclose(Ip_u32 ifindex);


/*
 *===========================================================================
 *                    ipdhcpc_if_isopen
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL Ip_bool ipdhcpc_if_isopen(Ip_u32 ifindex);


/*
 *===========================================================================
 *                    ipdhcpc_ifrequest
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL Ip_err ipdhcpc_ifrequest(Ip_u32 ifindex, Ipdhcpc_handle *handle);


/*
 *===========================================================================
 *                    ipdhcpc_iflease
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL Ip_err ipdhcpc_iflease(Ip_u32 ifindex, Ipdhcpc_handle *handle);


/*
 *===========================================================================
 *                    ipdhcpc_ifunlease
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL Ip_err ipdhcpc_ifunlease(Ip_u32 ifindex, Ipdhcpc_handle **handlep);


/*
 *===========================================================================
 *                    ipdhcpc_handle_malloc
 *===========================================================================
 * Description:   Mallocate and initialize a DHCP client request handle for
 *                use with ipdhcpc_ifrequest().
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL Ip_err ipdhcpc_handle_malloc(Ip_u32 ifindex,
                                       int mode,
                                       Ipdhcpc_handle **handlep);


/*
 *===========================================================================
 *                    ipdhcpc_handle_free
 *===========================================================================
 * Description:   Free a DHCP client handle.
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL Ip_err ipdhcpc_handle_free(Ip_u32 ifindex, Ipdhcpc_handle *handle);


/*
 *===========================================================================
 *                    ipdhcpc_option_sprintf
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL int ipdhcpc_option_sprintf(Ip_u8 *option, char *buf, int buflen);


/*
 *===========================================================================
 *                    ipdhcpc_option_find
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL Ip_u8 *ipdhcpc_option_find(Ipdhcpc_handle *handle, Ip_u8 code);


/*
 *===========================================================================
 *                    ipdhcpc_option_get_first
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL Ip_u8 *ipdhcpc_option_get_first(Ip_u8 *options);


/*
 *===========================================================================
 *                    ipdhcpc_option_get_next
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL Ip_u8 *ipdhcpc_option_get_next(Ip_u8 *options);

/*
 *===========================================================================
 *                    ipdhcpc_reply_cb
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *                                                
 */
IP_GLOBAL int
ipdhcpc_reply_cb(Ipdhcpc_handle *handle, int event);


#ifdef __cplusplus
}
#endif

#endif

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
