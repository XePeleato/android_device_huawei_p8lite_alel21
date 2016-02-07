/* ipdnsc.h - Public API of Wind River DNS client */

/*
 ******************************************************************************
 *                     HEADER FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipdnsc.h,v $ $Revision: 1.15 $
 *   $Source: /home/interpeak/CVSRoot/ipappl/include/ipdnsc.h,v $
 *   $Author: lob $ $Date: 2007-02-08 13:27:56 $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 ******************************************************************************
 */
#ifndef IPDNSC_H
#define IPDNSC_H

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 */

/*
DESCRIPTION
This library contains the APIs provided by the WindRiver DNS client.
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

#define IPCOM_USE_CLIB_PROTO
#include <ipcom_type.h>
#include <ipcom_cstyle.h>
#include <ipcom_clib.h>
#include <ipcom_sock.h>
#include <ipcom_tags.h>
#include <ipcom_os.h>
#include <ipcom_err.h>

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
 *                         release
 *===========================================================================
 */

#define IPDNSC_RELEASE 60500


/*  Error codes for lookup functions */
#define IPDNSC_HOST_NOT_FOUND       1
#define IPDNSC_TRY_AGAIN            2
#define IPDNSC_NO_RECOVERY          3
#define IPDNSC_NO_DATA              4
#define IPDNSC_NO_ADDRESS           IPDNSC_NO_DATA


/* Flags for lookup functions */
#define IPDNSC_AI_V4MAPPED          0x00000008
#define IPDNSC_AI_ALL               0x00000010
#define IPDNSC_AI_ADDRCONFIG        0x00000020
#define IPDNSC_AI_DEFAULT           (IPDNSC_AI_V4MAPPED | IPDNSC_AI_ADDRCONFIG)


/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

/*
 *===========================================================================
 *                         Ipdnsc_api
 *===========================================================================
 * For better binary scalability at image build time, access to
 * the DNS resolver interface may be done through the following
 * set of function pointers:
 */
typedef struct Ipdnsc_api_struct
{
    struct Ip_hostent *(*getipnodebyname)(const char *name, int af,
                                          int flags, int *error_num);
    struct Ip_hostent *(*getipnodebyaddr)(const void *src, Ip_size_t len,
                                          int af, int *error_num);
    void (*freehostent)(struct Ip_hostent *he);
    void (*cache_flush)(void);

}
Ipdnsc_api;


/*
 ****************************************************************************
 * 6                    LOCAL FUNCTIONS
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 10                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*******************************************************************************
*
* ipdnsc_getipnodebyname - get host information by node name
*
* This routine can be used instead of the older standard gethostbyname() routine.
* gethostbyname() is inadequate for many applications, first because it provides
* no way for the caller to specify anything about the types of addresses desired
* (IPv4 only, IPv6 only, IPv4-mapped IPv6 are 'OK', and so forth), and second because
* many implementations of this function are not thread safe.
*
* Parameters:
* \is
* \i <name>
* A pointer to either a node name or a numeric address string (for example, a
* dotted-decimal IPv4 address or an IPv6 hex address).
* \i <af>
* Specifies the address family, either 'IPDNSC_AF_INET' or 'IPDNSC_AF_INET6'.
* \i <flags>
* Specifies the types of addresses that are searched for and the types of
* addresses that are returned. Applications desiring finer control over the
* types of addresses searched for and returned  can specify other combinations
* of the <flags> parameter. A <flags> parameter of '0' implies a strict
* interpretation of the <af> parameter:
*
* If <flags> is '0' and <af> is 'AF_INET', then the caller wants only IPv4
* addresses. A query is made for A records.  If successful,  the IPv4 addresses
* are returned and the <h_length> member of the hostent structure is '4',
* otherwise the function returns a NULL pointer.
*
* If <flags> is '0' and if< af> is 'AF_INET6', then the caller wants only IPv6
* addresses.  A query is made for AAAA records. If successful, the IPv6
* addresses are returned and the <h_length> member of the hostent structure is
* 16, otherwise the function returns a NULL pointer.
*
* Other constants can be logically-ORed into the <flags> parameter to modify the
* behavior of the function.
*
* If the 'AI_V4MAPPED' flag is specified along with an <af> of 'AF_INET6', then
* the caller will accept IPv4-mapped IPv6 addresses.  That is, if no AAAA
* records are found then a query is made for A records and any found are
* returned as IPv4-mapped IPv6 addresses (<h_length> is 16). The 'AI_V4MAPPED'
* flag is ignored unless af equals 'AF_INET6'.
*
* The 'AI_ALL' flag is used in conjunction with the 'AI_V4MAPPED' flag, and is
* only used with the IPv6 address family. When 'AI_ALL' is logically or'd with
* 'AI_V4MAPPED' flag then the caller wants all addresses: IPv6 and IPv4-mapped
* IPv6.  A query is first made for AAAA records and if successful, the IPv6
* addresses are returned. Another query is then made for A records and any found
* are returned as IPv4-mapped IPv6 addresses. <h_length> is 16. Only if both
* queries fail does the function return a NULL pointer. This flag is ignored
* unless af equals 'AF_INET6'.
*
*  The 'AI_ADDRCONFIG' flag specifies that a query for AAAA records should occur
* only if the node has at least one IPv6 source address configured and a query
* for A records should occur only if the node has at least one IPv4 source
* address configured. For example, if the node has no IPv6 source addresses
* configured and <af> equals 'AF_INET6', and if the node name being looked up
* has both AAAA and A records, then:
*
* \ml
* \m -
* If only 'AI_ADDRCONFIG' is specified, the function returns a NULL pointer.
* \m -
* If 'AI_ADDRCONFIG' \| 'AI_V4MAPPED' is specified, the A records are returned
* as IPv4-mapped IPv6 addresses.
* \me
* \i <error_num>
* A pointer to an error code. <error_num> value is returned to the caller, via a
* pointer, with the appropriate error code in <error_num> to support thread safe
* error code returns.
*
* <error_num> is set to one of the following values:
*
* \is
* \i 'IPDNSC_HOST_NOT_FOUND'
* No such host is known.
* \i 'IPDNSC_NO_ADDRESS'
* The server recognized the request and the name but address is available.
* Another type of request to the name server for the domain might return an
* answer.
* \i 'IPDNSC_NO_RECOVERY'
* An unexpected server failure occurred which cannot be recovered.
* \i 'IPDNSC_TRY_AGAIN'
* A temporary and possibly transient error occurred, such as a failure of a
* server to respond.
* \ie
* \ie
*
* RETURNS:
*
* ERRNO:
*/
IP_PUBLIC struct Ip_hostent* ipdnsc_getipnodebyname
(
	const char *name,
	int af,
	int flags,
	int *error_num
);


/*******************************************************************************
*
* ipdnsc_getipnodebyaddr - get host information by IP address
*
* This routine gets host information based on the host\'s IP address. It is
* similar to the older standard gethostbyaddr() routine but adds an error number
* and is thread safe.
*
* Parameters:
* \is
* \i <src>
* A pointer to an IPv4 address or an IPv6 address in network endian.
* \i <len>
* The length of the address, in bytes.
* \i <af>
* The address family, either 'IPDNSC_AF_INET' or 'IPDNSC_AF_INET6'.
* \i <error_num>
* A pointer to an error code. <error_num> value is returned to the caller, via a
* pointer, with the appropriate error code in <error_num>, to support thread
* safe error code returns. The following error conditions may be returned for
* <error_num>:
*
* \is
* \i 'IPDNSC_HOST_NOT_FOUND'
* No such host is known.
* \i 'IPDNSC_NO_ADDRESS'
* The server recognized the request and the name but no address is available.
* Another type of request to the name server for the domain might return an
* answer.
* \i 'IPDNSC_NO_RECOVERY'
* An unexpected server failure occurred which cannot be recovered.
* \i 'IPDNSC_TRY_AGAIN'
* A temporary and possibly transient error occurred, such as a failure of a
* server to respond.
* \ie
* \ie
*
* RETURNS:
*
* ERRNO:
*/
IP_PUBLIC struct Ip_hostent* ipdnsc_getipnodebyaddr
(
	const void *src,
	Ip_size_t len,
	int af,
	int *error_num
);


/*******************************************************************************
*
* ipdnsc_freehostent - free memory allocated for an 'ip_hostent' structure
*
* This routine frees memory allocated for an 'ip_hostent' structure.  An
* 'ip_hostent' structure is dynamically allocated by the
* ipdnsc_getipnodebyname() and ipdnsc_getipnodebyaddr() functions and must
* therefore be freed after use.
*
* Parameter:
* \is
* \i <he>
* A pointer to an 'ip_hostent' structure.
* \ie
*
* RETURNS:
*
* ERRNO:
*/
IP_PUBLIC void ipdnsc_freehostent
(
	struct Ip_hostent *he
);


/*******************************************************************************
*
* ipdnsc_cache_flush - remove all entries in the resolver cache.
*
* This routine removes all entries in the resolver cache. It is useful after
* the name server has been updated with new information about a specific host.
*
* RETURNS:
*
* ERRNO:
*/
IP_PUBLIC void ipdnsc_cache_flush(void);


#ifdef __cplusplus
}
#endif

#endif

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
