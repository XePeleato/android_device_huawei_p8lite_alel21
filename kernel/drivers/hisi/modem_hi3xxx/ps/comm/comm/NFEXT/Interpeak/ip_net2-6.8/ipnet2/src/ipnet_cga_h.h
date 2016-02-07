/*
 ******************************************************************************
 *                     INTERPEAK API HEADER FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_cga_h.h,v $ $Revision: 1.1 $
 *   $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_cga_h.h,v $
 *   $Author: rboden $ $Date: 2008-09-04 12:47:51 $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Roger Boden
 ******************************************************************************
 */
#ifndef IPNET_CGA_H_H
#define IPNET_CGA_H_H

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

#include "ipnet_config.h"


/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#include <ipcom_type.h>
#include <ipcom_cstyle.h>
#include <ipcom_sock6.h>

#include "ipnet_ip6.h"

#include <openssl/rsa.h>


#ifdef __cplusplus
extern "C" {
#endif

/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

#define IPNET_CGA_ICMP6_OPT_RSA_LEN(cga_opt) (opt_cga->nd_opt_cga_len*8-4-16-8-1-opt_cga->nd_opt_cga_pad_len)

/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 6                    FUNCTIONS
 ****************************************************************************
 */
    IP_GLOBAL int ipnet_cga_der_encode_rsa_public_key(Ip_u8 **ppbuf, RSA* rsa);
    IP_GLOBAL Ip_err ipnet_cga_icmp6_opt_add(Ip_u8 *der_rsa_key,
                                             int der_len,
                                             struct Ip_in6_addr *src_addr,
                                             int ifindex,
                                             Ip_u8 *buf,
                                             int *len);

    IP_GLOBAL Ip_err ipnet_cga_icmp6_opt_verify(Ipnet_pkt_nd_opt_cga *opt_cga, struct Ip_in6_addr *cga_addr);

    IP_GLOBAL RSA* ipnet_cga_icmp6_opt_get_rsa_key(Ipnet_pkt_nd_opt_cga *opt_cga);
    IP_GLOBAL void ipnet_cga_modifier_bin2hex(Ip_u8 *bin, int buf_len, char *hex);


#ifdef __cplusplus
}
#endif

#endif

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
