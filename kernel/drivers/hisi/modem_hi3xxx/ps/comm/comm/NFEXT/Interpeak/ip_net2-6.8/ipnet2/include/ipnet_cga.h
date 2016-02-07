/* ipnet_cga.h - Public API of Wind River Cryptographically Generated Addresses */
/*
 ******************************************************************************
 *                     INTERPEAK API HEADER FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_cga.h,v $ $Revision: 1.3 $
 *   $Source: /home/interpeak/CVSRoot/ipnet2/include/ipnet_cga.h,v $
 *   $Author: rboden $ $Date: 2008-09-17 06:27:50 $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Roger Boden
 ******************************************************************************
 */
#ifndef IPNET_CGA_H
#define IPNET_CGA_H

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 */

/*
DESCRIPTION
This library contains the APIs used for generating and verifying
Cryptographically Generated Addresses, CGA.
INCLUDE FILES: ipnet_cga.h
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


/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

    struct Ipnet_cga_param_struct
    {
        Ip_u8 modifier[16];
        Ip_u8 subnet_prefix[8];
        Ip_u8 collision_count;
        RSA* rsa_key;
        Ip_u8 * der_encoded_key;
        int der_key_len;
        int sec;
    };
    typedef struct Ipnet_cga_param_struct Ipnet_cga_param;


/*
 ****************************************************************************
 * 6                    FUNCTIONS
 ****************************************************************************
 */

/*******************************************************************************
*
* ipnet_cga_generate - generate a CGA address
*
* Parameters:
* \is
* \i <cga_param>
* A pointer to a Ipnet_cga_param struct.
* 'sec' must be set to a value between 0 and 7.
* 'subnet_prefix' member must be set.
* 'rsa_key' must hold the key to use in the key generation.
* 'der_encoded_key' is the DER encoded RSA key in 'rsa_key'.
* 'der_key_len' is the length of 'der_encoded_key'.
* 'modifier' will be set by this function.
* 'collision_count' is currently not used.
* \i <cga_addr>
* Pointer to an Ip_in6_addr struct in which the generated address will be placed.
* \ie
*
* RETURNS: Either 'IPCOM_SUCCESS' or an error code (see 'ipcom_err.h').
*
* ERRNO:
*/
    IP_PUBLIC Ip_err ipnet_cga_generate(
                                        Ipnet_cga_param *cga_param,
                                        struct Ip_in6_addr *cga_addr
                                        );

/*******************************************************************************
*
* ipnet_cga_verify - verify a CGA address
*
* Parameters:
* \is
* \i <cga_param>
* A pointer to a Ipnet_cga_param struct holding the parameters of the address.
* \i <cga_addr>
* Pointer to an Ip_in6_addr struct holding the CGA address that will be verified.
* \ie
*
* RETURNS: Either 'IPCOM_SUCCESS' or an error code (see 'ipcom_err.h').
*
* ERRNO:
*/
    IP_PUBLIC Ip_err ipnet_cga_verify(
                                      Ipnet_cga_param *cga_param,
                                      struct Ip_in6_addr *cga_addr
                                      );

/*******************************************************************************
*
* ipnet_cga_param_new - create a new Ipnet_cga_param struct
*
* Parameter:
*
* None.
*
* RETURNS: A pointer to the created struct, or NULL in case of error.
*
* ERRNO:
*/
    IP_PUBLIC Ipnet_cga_param* ipnet_cga_param_new(
                                                   void
                                                   );

/*******************************************************************************
*
* ipnet_cga_param_free - free an Ipnet_cga_param struct
*
* Parameter:
* \is
* \i <cga_param>
* Pointer to the struct to be freed.
* \ie
*
* RETURNS: void
*
* ERRNO:
*/
    IP_PUBLIC void ipnet_cga_param_free(
                                        Ipnet_cga_param* cga_param
                                        );

/*******************************************************************************
*
* ipnet_cga_param_set_rsa - set the RSA key in a Ipnet_cga_param struct
*
* Sets the RSA key in a Ipnet_cga_param struct. The RSA is also DER encoded
* and the der_encoded_key and der_key_len fields are updated.
*
* Parameters:
* \is
* \i <cga_param>
* A pointer to a Ipnet_cga_param struct in which the RSA key shall be set.
* \i <rsa>
* Pointer to the RSA key.
* \ie
*
* RETURNS: Either 'IPCOM_SUCCESS' or an error code (see 'ipcom_err.h').
*
* ERRNO:
*/
    IP_PUBLIC Ip_err ipnet_cga_param_set_rsa(
                                             Ipnet_cga_param *cga_param,
                                             RSA *rsa
                                             );


/*******************************************************************************
*
* ipnet_cga_get_sec - get the sec value of a CGA address
*
* Parameter:
* \is
* \i <addr>
* Pointer to the CGA address
* \ie
*
* RETURNS: sec value
*
* ERRNO:
*/
    IP_PUBLIC int ipnet_cga_get_sec(
                                    struct Ip_in6_addr *addr
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
