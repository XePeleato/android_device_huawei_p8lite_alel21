/* ipnet_nat.h - Public API of Wind River NAT */

/*
 ******************************************************************************
 *                     HEADER FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_nat.h,v $ $Revision: 1.15 $
 *   $Source: /home/interpeak/CVSRoot/ipnet2/include/ipnet_nat.h,v $
 *   $Author: kenneth $ $Date: 2008-03-14 12:51:34 $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *     Design and implementation by Markus Carlstedt <markus@interpeak.se>
 ******************************************************************************
 */
#ifndef IPNET_NAT_H
#define IPNET_NAT_H

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 * NAT API header file.
 */

/*
DESCRIPTION
This library contains the APIs used for configuration of Wind River NAT.
*/

/*
 ****************************************************************************
 * 2                    CONFIGURATION
 ****************************************************************************
 */

#include "ipnet_config.h"


/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#include <ipcom_type.h>
#include <ipcom_cstyle.h>

#ifdef __cplusplus
extern "C" {
#endif

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
 *===========================================================================
 *                         Ipnet_nat_proxy_tuple
 *===========================================================================
 * NAT proxy tuple structure
 */
typedef struct Ipnet_nat_proxy_tuple_struct
{
    Ip_u16  private_port;   /* Private side port */
    Ip_u16  public_port;    /* Public side port */
    Ip_u32  private_addr;   /* Private side address */
    Ip_u32  public_addr;    /* Public side address */
    Ip_u8   protocol;       /* Protocol */
}
Ipnet_nat_proxy_tuple;


/*
 *===========================================================================
 *                         Ipnet_nat_proxy_param_struct
 *===========================================================================
 * NAT proxy parameter structure.
 */
typedef struct Ipnet_nat_proxy_param_struct
{
    Ipnet_nat_proxy_tuple tuple;    /* Tuple */
    Ip_u32  nat_addr;       /* NAT address */
    Ip_u16  nat_port;       /* NAT port */
    void    *mapping;       /* Pointer to NAT mapping */
    Ip_bool inbound;        /* IP_TRUE if mapping is inbound (created by redirect rule) */
    Ip_bool incoming;       /* IP_TRUE if packet was incoming (from public to private) */
    Ip_bool natpt;          /* IP_TRUE if packet matched a NAT-PT rule */
    Ip_u32  prefix[3];      /* prefix for NAT-PT rules in network endian */
    void    *cookie;        /* proxy cookie */
    Ip_u32  fragid;         /* Fragment identifier */
    Ip_u16  fragoff;        /* Fragment offset */
    Ip_u8   fragmf;         /* More fragments bit */
}
Ipnet_nat_proxy_param;


/*
 *===========================================================================
 *                         Ipnet_nat_proxy_func
 *===========================================================================
 * Description: NAT proxy function.
 * Parameters:  newhdr    - pointer to optionally write new IP header.
 *              appdata   - pointer to application data.
 *              applen    - pointer to length of application data.
 *                          Proxy function must uppdate 'applen' with new
 *                          value if length is changed.
 *              growspace - space available to extend application data.
 *              param     - pointer to proxy parameters.
 *              newdata   - pointer to pointer to new application data
 *                          to used if application data needs to be extended
 *                          beyond growspace. Proxy function must allocate
 *                          the buffer with ipcom_malloc() and place the
 *                          pointer in 'newdata'.
 * Returns:     1 = Packet modified.
 *              0 = Packet untouched.
 *             -1 = Drop packet.
 */
typedef int (* Ipnet_nat_proxy_func) (Ip_u8 *newhdr,
                                      Ip_u8 *appdata,
                                      int   *applen,
                                      int    growspace,
                                      Ipnet_nat_proxy_param *param,
                                      Ip_u8 **newdata);

/*
 *===========================================================================
 *                         Ipnet_nat_trigger
 *===========================================================================
 * Trigger proxy structure. To be used as cookie in the port trigger proxy
 */
typedef struct Ipnet_nat_trigger_struct
{
    Ip_u8  protocol;    /* IP_IPPROTO_TCP or IP_PROTO_UDP */
    Ip_u16 portlo;      /* Low port in interval */
    Ip_u16 porthi;      /* High port in interval */
    Ip_u32 timeout;     /* Timeout in seconds until ports close (0 means default) */
    Ipnet_nat_proxy_func  func;     /* proxy function (optional) */
    void                 *cookie;   /* proxy cookie (optional) */
}
Ipnet_nat_trigger;


/*
 *===========================================================================
 *                         Ipnet_nat_proxy_timeout_handler
 *===========================================================================
 * NAT proxy timeout handler
 */
typedef void (*Ipnet_nat_proxy_timeout_handler)(void *cookie);


/*
 ****************************************************************************
 * 6                    FUNCTIONS
 ****************************************************************************
 */

IP_PUBLIC int ipnet_nat_proxy_ftp(Ip_u8 *newhdr,
                                  Ip_u8 *appdata,
                                  int   *applen,
                                  int    growspace,
                                  Ipnet_nat_proxy_param *param,
                                  Ip_u8 **newdata);


IP_PUBLIC int ipnet_nat_proxy_dns(Ip_u8 *newhdr,
                                  Ip_u8 *appdata,
                                  int   *applen,
                                  int    growspace,
                                  Ipnet_nat_proxy_param *param,
                                  Ip_u8 **newdata);


IP_PUBLIC int ipnet_nat_proxy_trigger(Ip_u8 *newhdr,
                                      Ip_u8 *appdata,
                                      int   *applen,
                                      int    growspace,
                                      Ipnet_nat_proxy_param *param,
                                      Ip_u8 **newdata);


IP_PUBLIC int ipnet_nat_proxy_ipsec(Ip_u8 *newhdr,
                                    Ip_u8 *appdata,
                                    int   *applen,
                                    int    growspace,
                                    Ipnet_nat_proxy_param *param,
                                    Ip_u8 **newdata);


IP_PUBLIC int ipnet_nat_proxy_pptp(Ip_u8 *newhdr,
                                   Ip_u8 *appdata,
                                   int   *applen,
                                   int    growspace,
                                   Ipnet_nat_proxy_param *param,
                                   Ip_u8 **newdata);


IP_PUBLIC int ipnet_nat_proxy_h323(Ip_u8 *newhdr,
                                   Ip_u8 *appdata,
                                   int   *applen,
                                   int    growspace,
                                   Ipnet_nat_proxy_param *param,
                                   Ip_u8 **newdata);


IP_PUBLIC int ipnet_nat_proxy_sip(Ip_u8 *newhdr,
                                  Ip_u8 *appdata,
                                  int   *applen,
                                  int    growspace,
                                  Ipnet_nat_proxy_param *param,
                                  Ip_u8 **newdata);


/*******************************************************************************
*
* ipnet_nat_enable - enable NAT
*
* The ipnet_nat_enable() routine enables NAT and reads configuration settings
* for it.
*
* Parameters:
*
* None.
*
* RETURNS: Either 'IPCOM_SUCCESS' or an error code (see 'ipcom_err.h').
*
* ERRNO:
*/
IP_PUBLIC Ip_err ipnet_nat_enable(void);


/*******************************************************************************
*
* ipnet_nat_disable - disable NAT
*
* The ipnet_nat_disable() routine disables NAT and flushes all mappings.
*
* Parameters:
*
* None.
*
* RETURNS: Either 'IPCOM_SUCCESS' or an error code (see 'ipcom_err.h').
*
* ERRNO:
*/
IP_PUBLIC Ip_err ipnet_nat_disable(void);


/*******************************************************************************
*
* ipnet_nat_add_rule - add a NAT rule
*
* The ipnet_nat_add_rule() routine adds a NAT rule to the curent set of NAT
* rules.
*
* Parameter:
* \is
* \i <rule>
* A pointer to a string containing the rule.
* \ie
*
* RETURNS: Either 'IPCOM_SUCCESS' or an error code (see 'ipcom_err.h').
*
* ERRNO:
*/
IP_PUBLIC Ip_err ipnet_nat_add_rule
(
    const char *rule
);


/*******************************************************************************
*
* ipnet_nat_remove_rule - remove a NAT rule
*
* The ipnet_nat_remove_rule() routine removes a NAT rule from the curent set of
* NAT rules.
*
* Parameter:
* \is
* \i <rule>
* A pointer to a string containing the rule.
* \ie
*
* RETURNS: Either 'IPCOM_SUCCESS' or an error code (see 'ipcom_err.h').
*
* ERRNO:
*/
IP_PUBLIC Ip_err ipnet_nat_remove_rule
(
    const char *rule
);


/*******************************************************************************
*
* ipnet_nat_flush_rules - remove all NAT rules
*
* The ipnet_nat_flush_rules() routine removes all NAT rules.
*
* Parameters:
*
* None.
*
* RETURNS: Either 'IPCOM_SUCCESS' or an error code (see 'ipcom_err.h').
*
* ERRNO:
*/
IP_PUBLIC Ip_err ipnet_nat_flush_rules(void);


/*******************************************************************************
*
* ipnet_nat_flush_mappings - remove all NAT mappings
*
* The ipnet_nat_flush_mappings() routine removes all NAT mappings.
*
* Parameters:
*
* None.
*
* RETURNS: Either 'IPCOM_SUCCESS' or an error code (see 'ipcom_err.h').
*
* ERRNO:
*/
IP_PUBLIC Ip_err ipnet_nat_flush_mappings(void);


/*******************************************************************************
*
* ipnet_nat_proxy_add_mapping - add a public-to-private NAT mapping
*
* The ipnet_nat_proxy_add_mapping() routine adds a mapping between a host on the
* private side of the NAT and a host on the public side of the NAT. Such a
* mapping is typically used to open a port through the NAT, which is usually
* closed to incoming connections. It can also be used to open an outgoing path
* through the NAT, if there is no matching rule.
*
* The <proxy_tuple> parameter specifies the private and public addresses and
* ports, as well as the protocol. If the source port of the connecting host is
* unknown, it can be set to zero to allow any source port to be used. Likewise,
* if the source address of the connecting host is unknown, it can be set to zero
* to allow any host connect. Once the mapping has been used for the first time,
* it can only be used by the host that connected first. Setting both the
* the private and public addresses to zero is not allowed.
*
* The protocol can be any protocol, except for ICMP. For non-TCP/UDP protocols,
* the ports must be set to zero. Additionally, the source address can be set to
* zero to allow any host to connect. The source port is be the same on the
* private host and the NAT gateway unless the <use_napt> parameter is set to
* 'IP_TRUE'. In this case, a new port is automatically allocated and port
* translation takes place.
*
* Optionally, packets matching the mapping can be configured to call an
* application proxy if the <proxy_func> and <proxy_cookie> parameters are set.
*
* Parameters:
* \is
* \i <proxy_tuple>
* A pointer to the proxy tuple.
* \i <timeout>
* A timeout, after which the port is closed if no packets arrive. A timeout of
* '0' means that default values are applied).
* \i <parent>
* A pointer to the NAT mapping that caused the call to the proxy function. This
* pointer must have the same pointer value as was received in the call to
* the proxy function in the argument 'param-\>mapping'
* \i <use_napt>
* Set to 'IP_TRUE' for port translation.
* \i <use_inbound>
* Set to 'IP_TRUE' to indicate that the session will start inbound.
* \i <proxy_func>
* An optional proxy function.
* \i <proxy_cookie>
* An optional proxy cookie.
* \ie
*
* RETURNS: The NAT port, or '-1' on failure.
*
* ERRNO:
*/
IP_PUBLIC int ipnet_nat_proxy_add_mapping
(
    Ipnet_nat_proxy_tuple *proxy_tuple,
    Ip_u32                 timeout,
    void                  *parent,
    Ip_bool                use_napt,
    Ip_bool                use_inbound,
    Ipnet_nat_proxy_func   proxy_func,
    void                  *proxy_cookie
);


/*******************************************************************************
*
* ipnet_nat_proxy_set_mapping_timeout - set a timeout for a mapping
*
* The ipnet_nat_proxy_set_mapping_timeout() routine sets a timeout for a
* mapping.
*
* Parameters:
* \is
* \i <sec>
* The number of seconds after which the mapping times out.
* \i <mapping>
* A pointer to the NAT mapping that caused the call to the proxy function. This
* pointer must have the same pointer value as was received in the call to
* the proxy function in the argument 'param-\>mapping'
* \ie
*
* RETURNS: No return value.
*
* ERRNO:
*/
IP_PUBLIC void ipnet_nat_proxy_set_mapping_timeout
(
    Ip_u32 sec,
    void *mapping
);


/*******************************************************************************
*
* ipnet_nat_proxy_get_time - return the elapsed time since the last boot
*
* The ipnet_nat_proxy_get_time() routine returns the time elapsed since booting,
* in seconds.
*
* Parameters:
*
* None.
*
* RETURNS: The number of seconds since booting.
*
* ERRNO:
*/
IP_PUBLIC Ip_u32 ipnet_nat_proxy_get_time(void);


/*******************************************************************************
*
* ipnet_nat_proxy_timeout_schedule - schedule a timeout handler
*
* The ipnet_nat_proxy_timeout_schedule() routine sets a
* timeout period, in seconds, after which a user-defined timeout handler is
* called.
*
* Parameters:
* \is
* \i <sec>
* The length of time, in seconds, after which the function is called.
* \i <handler>
* A pointer to the function to be called.
* \i <cookie>
* A cookie for use by the called function.
* \i <ptmo>
* A pointer that stores the location of a pointer to the timeout structure used
* by the timeout handler. The pointer must be provided by the user and kept until
* the timeout handler has been called.
* \ie
*
* RETURNS: '0' for success; a negative value for failure.
*
* ERRNO:
*/
IP_PUBLIC int ipnet_nat_proxy_timeout_schedule
(
    Ip_u32 sec,
    Ipnet_nat_proxy_timeout_handler handler,
    void *cookie,
    void **ptmo
);


/*******************************************************************************
*
* ipnet_nat_proxy_timeout_reschedule - reschedule a timeout handler
*
* The ipnet_nat_proxy_timeout_reschedule() routine resets the timeout period, in
* seconds, on a running timer with a new timeout period, after which a
* user-defined timeout handler is called.
*
* Parameters:
* \is
* \i <sec>
* The length of time, in seconds, after which the function is called.
* \i <handler>
* A pointer to the function to be called.
* \i <cookie>
* A cookie for use by the called function.
* \i <ptmo>
* A pointer that stores the location of a pointer to the timeout structure used
* by the timeout handler. The pointer must be provided by the user and kept until
* the timeout handler has been called.
* \ie
*
* RETURNS: '0' for success; a negative value for failure.
*
* ERRNO:
*/
IP_PUBLIC int ipnet_nat_proxy_timeout_reschedule
(
    Ip_u32 sec,
    Ipnet_nat_proxy_timeout_handler handler,
    void *cookie,
    void **ptmo
);


/*******************************************************************************
*
* ipnet_nat_proxy_timeout_cancel - cancel a timeout
*
* The ipnet_nat_proxy_timeout_cancel() routine cancels a scheduled timeout.
*
* Parameter:
* \is
* \i <tmo>
* A pointer to the timeout structure.
* \ie
*
* RETURNS: No return value.
*
* ERRNO:
*/
IP_PUBLIC void ipnet_nat_proxy_timeout_cancel
(
    void *tmo
);


/*******************************************************************************
*
* ipnet_nat_add_proxy - add a proxy to NAT
*
* The ipnet_nat_add_proxy() routine adds a proxy (ALG) to NAT.
*
* Parameters:
* \is
* \i <label>
* An ASCII string identifier.
* \i <proto>
* The IP protocol the proxy applies to. Either a protocol name or numerical
* string is allowed.
* \i <func>
* A pointer to the ALG function.
* \i <cookie>
* A cookie that is supplied in the call to the proxy function. The memory
* location referred to by the cookie must be valid as long as the proxy has not
* been removed.
* \ie
*
* RETURNS: Either 'IPCOM_SUCCESS' or an error code (see 'ipcom_err.h').
*
* ERRNO:
*/
IP_PUBLIC Ip_err ipnet_nat_add_proxy
(
    const char *label,
    const char *proto,
    Ipnet_nat_proxy_func func, void *cookie
);


/*******************************************************************************
*
* ipnet_nat_remove_proxy - remove a proxy from NAT
*
* The ipnet_nat_remove_proxy() routine removes a proxy (ALG) from NAT.
*
* Parameters:
* \is
* \i <label>
* An ASCII string identifier.
* \i <proto>
* The IP protocol the proxy applies to. Either a protocol name or numerical
* string is allowed.
* \ie
*
* RETURNS: Either 'IPCOM_SUCCESS' or an error code (see 'ipcom_err.h').
*
* ERRNO:
*/
IP_PUBLIC Ip_err ipnet_nat_remove_proxy
(
    const char *label,
    const char *proto
);


#ifdef __cplusplus
}
#endif


#endif

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
