/* ipcom_auth.h - IPCOM authentication library */

/*
 ******************************************************************************
 *                     INTERPEAK API HEADER FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_auth.h,v $ $Revision: 1.21.24.2 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/include/ipcom_auth.h,v $
 *   $Author: gzadoyan $ $Date: 2010-09-15 22:28:30 $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Lennart Bang <lob@interpeak.se>
 ******************************************************************************
 */
#ifndef IPCOM_AUTH_H
#define IPCOM_AUTH_H

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 */

/*
DESCRIPTION
This library contains the API for IPCOM authentication.
INCLUDE FILES: ipcom_type.h, ipcom_err.h
*/

/*
 ****************************************************************************
 * 2                    CONFIGURATION
 ****************************************************************************
 */

#include "ipcom_config.h"


/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#include <ipcom_type.h>
#include <ipcom_err.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

#if (!defined(IPCOM_KERNEL) && defined(IPCOM_USRLIB)) || defined(IPCOM_USE_REMOTE_AUTH)
#define ipcom_auth_useradd     ipcom_auth_useradd_usr
#define ipcom_auth_userdel     ipcom_auth_userdel_usr
#define ipcom_auth_userget     ipcom_auth_userget_usr
#define ipcom_auth_userlist    ipcom_auth_userlist_usr
#endif

#define IPCOM_AUTH_USERNAME_MAX   81  /* Max user name length. */
#define IPCOM_AUTH_PASSWORD_MAX   129  /* Max user password length. */

/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

/*
 *===========================================================================
 *                         Ipcom_auth_userinfo
 *===========================================================================
 * User information filled in by the login functions, their callback or
 * retrieved by calling ipcom_auth_userget() or ipcom_auth_userlist().
 */
typedef struct Ipcom_auth_userinfo_struct
{
    char       username[IPCOM_AUTH_USERNAME_MAX];
    char       password[IPCOM_AUTH_PASSWORD_MAX]; 
    Ip_uid_t   userid;
    Ip_bool    pwdhash;

#ifdef IPCOM_USE_AUTH_RADIUS
    Ip_u8     *radius_attributes;
    int        radius_attrlen;
#endif
}
Ipcom_auth_userinfo;


/*
 *===========================================================================
 *                         Ipcom_auth_login_hook
 *===========================================================================
 * Login function callback for non-blocking login (used by PPP/RADIUS)
 */
typedef void (* Ipcom_auth_login_hook) (Ip_err retval, Ipcom_auth_userinfo *userinfo, void *auth_login_cookie);


/*
 *===========================================================================
 *                         Ipcom_auth_hash_hook
 *===========================================================================
 * Login function callback for password hashing. Hash the password string
 * into an output string in <buf>. The digest this routine store at <buf> 
 * may be no longer than IPCOM_AUTH_PASSWORD_MAX.
 */
typedef Ip_err (* Ipcom_auth_hash_hook) (const char *passwd, char *buf);


/*
 *===========================================================================
 *                         Ipcom_auth_chap
 *===========================================================================
 * Argument structure for ipcom_auth_chap_login().
 */
typedef struct Ipcom_auth_chap_struct
{
    const Ip_u8 *response;
    Ip_u8        ident;

    const Ip_u8 *challenge;
    int          challenge_size;
}
Ipcom_auth_chap;


/*
 ****************************************************************************
 * 6                    FUNCTIONS
 ****************************************************************************
 */

/*******************************************************************************
*
* ipcom_auth_useradd - add a new user with a cleartext password and user ID
*
* This routine adds a new user with <password> and <userid>. 
*
* If parameter <userid> is (~0) or 0xFFFFFFFF then next available user ID will 
* automatically be allocated for a new user. 
*
* Parameters:
* \is
* \i <username>
* Name of the user to add.
* \i <password>
* User\'s <password> in a cleartext format.
* \i <userid>
* User ID, currently not used for anything.
* \ie
*
* RETURNS: 'IPCOM_SUCCESS', or one of the following errors:
* \is
* \i 'IPCOM_ERR_DUPLICATE'
* User already exists.
* \i 'IPCOM_ERR_NO_MEMORY'
* Out of memory when adding new user.
* \ie
*
* ERRNO:
*/
IP_PUBLIC Ip_err ipcom_auth_useradd
(
    const char *username,
    const char *password,
    Ip_uid_t userid 
);

/*******************************************************************************
*
* ipcom_auth_useradd_hash - add a new user with a hashed or cleartext password and user ID
*
* This routine adds a new user with <password> and <userid>.
* The hashed password should be in base64 encoded format. The base 64 encoded   
* password is decoded and the hashed password is saved in the database. The 
* size of the hashed password digest should not exceed IPCOM_AUTH_PASSWORD_MAX.
*
* If parameter <userid> is (~0) or 0xFFFFFFFF then next available user ID will 
* automatically be allocated for a new user.
*
* Parameters:
* \is
* \i <username>
* Name of the user to add.
* \i <password>
* User\'s <password> in a hash or cleartext format. Hashed password should be base64 encoded.
* \i <userid>
* User ID, currently not used for anything.
* \i <hash>
* Password is hashed if this parrameter is TRUE and in cleartext if FALSE.
* \ie
*
* RETURNS: 'IPCOM_SUCCESS', or one of the following errors:
* \is
* \i 'IPCOM_ERR_DUPLICATE'
* User already exists.
* \i 'IPCOM_ERR_NO_MEMORY'
* Out of memory when adding new user.
* \i 'IPCOM_ERR_FAILED'
* Invalid user name or password.
* \ie
*
* ERRNO:
*/
IP_PUBLIC Ip_err ipcom_auth_useradd_hash
(
    const char *username,
    const char *password,
    Ip_uid_t userid,
    Ip_bool  hash
);


/*******************************************************************************
*
* ipcom_auth_userdel - delete a user
*
* This routine deletes a user.
*
* Parameter:
* \is
* \i <username>
* Name of the user to delete.
* \ie
*
* RETURNS: 'IPCOM_SUCCESS', or the following error:
* \is
* \i 'IPCOM_ERR_NOT_FOUND'
* User does not exist.
* \ie
*
* ERRNO:
*/
IP_PUBLIC Ip_err ipcom_auth_userdel
(
    const char *username
);


/*******************************************************************************
*
* ipcom_auth_userget - get user information 
*
* This routine gets user information. 
*
* Parameters:
* \is
* \i <username>
* Name of the user for which to get information.
* \i <userinfo>
* Pointer to an 'Ipcom_auth_userinfo' structure to store <username>, <password>,
* <userid> and <pwdhash>.
* \ie
*
* RETURNS: 'IPCOM_SUCCESS', or the following error:
* \is
* \i 'IPCOM_ERR_NOT_FOUND'
* User does not exist.
* \ie
*
* ERRNO:
*/
IP_PUBLIC Ip_err ipcom_auth_userget
(
    const char *username,
    Ipcom_auth_userinfo *userinfo
);


/*******************************************************************************
*
* ipcom_auth_userlist - get the username and password for a specified user ID
*
* This routine gets the username and password of the user specified through the
* <userid> parameter. When the routine returns, <userid> points to the next user
* ID registered with the system. If there is no next user ID, <userid> points to
* 0.
*
* If there is no user corresponding to the ID input through <userid>, when the
* routine returns, <userid> points to the first user ID in the system. You can 
* always obtain the first user ID by setting <userid> to (~0) or (0xFFFFFFFF)
* and using its return value.
*
* Parameters:
* \is
* \i <userid>
* A pointer to the user ID for which to get a username and password. When
* ipcom_auth_userlist() returns, <userid> points to the next valid user ID.
* \i <userinfo>
* A pointer to an 'ipcom_auth_userinfo' structure containing fields for
* username, password, and user ID. When ipcom_auth_userlist() returns, the
* structure <userinfo> points to contains the username and password
* corresponding to the user ID input through the userid parameter.
* \ie
*
* RETURNS: 'IPCOM_SUCCESS', or one of the following errors:
* \is
* \i 'IPCOM_ERR_NOT_FOUND'
* No user found with <userid>. <userid> is updated to the first user\'s ID.
* \i 'IPCOM_ERR_INVALID_ARG'
* Invalid argument.
* \ie
*
* ERRNO:
*/
IP_PUBLIC Ip_err ipcom_auth_userlist
(
    Ip_uid_t *userid,
    Ipcom_auth_userinfo *userinfo
);

/*******************************************************************************
*
* ipcom_auth_login - validate username and password
*
* DESCRIPTION
* This routine validates a username and password. The routine looks up the user
* and validates the password in the IPCOM users table.
*
* If a local user is not found and RADIUS client support is compiled in, the
* function will validate a user sending a RADIUS client access request packet to
* a RADIUS server.
*
* When RADIUS is used, the routine can operate in either blocking and non-
* blocking mode.  In non-blocking mode, the routine is run with an optional
* pointer to a user function and a cookie.  The user function is called when
* authentication is complete.
*
* In non-blocking mode, if the authentication cannot be immediately resolved,
* 'IPCOM_ERR_PENDING' is returned and the user routine is called when the
* authentication completes.
*
* Note that even in non-blocking mode, the routine may return success
* immediately if a local user exists with a valid password.
*
* Parameters:
* \is
* \i <username>
* Name of the user to validate.
* \i <password>
* User\'s password in cleartext or hashed format.
* \i <userinfo>
* Pointer to an 'Ipcom_auth_userinfo' structure, if user information such as
* user ID is desired. Can optionally be 'IP_NULL' if no additional information
* is desired.
* \i <auth_login_hook>
* When RADIUS is used, this routine can run in either blocking mode or non-
* blocking mode.  For blocking mode, set <auth_login_hook> to 'IP_NULL'.
* For non-blocking mode, use a pointer to an 'ipcom_auth_login_hook()' routine,
* which is called when the authentication is complete.
* \i <auth_login_cookie>
* User 32-bit cookie which is supplied in the <auth_login_hook> parameter
* \ie
*
* RETURNS
* \is
* \i 'IPCOM_SUCCESS'
* Valid user and password.
* \i 'IPCOM_ERR_AUTH_FAILED'
* Valid user, bad password.
* \i 'IPCOM_ERR_PENDING'
* Non-blocking login, the authentication reply is pending and the
* auth_login_hook() routine will be called when a result is available.
* \i 'IPCOM_ERR_AUTH_UNKNOWN'
* Unknown user (only returned without RADIUS support).
* \ie
*
* ERRNO:
*/
IP_PUBLIC Ip_err ipcom_auth_login
(
    const char *username,
    const char *password,
    Ipcom_auth_userinfo *userinfo,
    Ipcom_auth_login_hook auth_login_hook,
    void *auth_login_cookie
);


/*******************************************************************************
*
* ipcom_auth_chap_login - validate username
*
* This routine validates username using <chap_response>, <ident>, and
* <challenge>. The routine looks up the user and validates the password using
* the MD5 Challenge-Handshake Authentication Protocol (CHAP) challenge/response
* syntax used in PPP. The local IPCOM user\'s table is searched first.
*
* If a local user is not found and RADIUS client support is compiled in, the
* function validates the user by sending a RADIUS client access request
* packet to a RADIUS server.
*
* When RADIUS is used, the routine can operate in either blocking and non-
* blocking mode.  In non-blocking mode, the routine is run with an optional
* pointer to a user function and a cookie.  The user function is called when
* authentication is complete.
*
* In non-blocking mode, if the authentication cannot be immediately resolved,
* 'IPCOM_ERR_PENDING' is returned and the user routine is called when the
* authentication completes.
*
* Note that even in non-blocking mode, the function may return success
* immediately, if a local user exists and the password check was valid.
*
* Parameters:
* \is
* \i <username>
* Name of the user to validate.
* \i <chap_response>
* CHAP response, 16 bytes long.
* \i <chap_ident>
* 8-bit CHAP identifier.
* \i <chap_challenge, chap_challenge_size>
* CHAP challenge data and length.
* \i <userinfo>
* Pointer to an 'ipcom_auth_userinfo structure', if user information such as the
* user ID is desired. Can optionally be 'IP_NULL', if no additional information
* is desired.
* \i <auth_login_hook>
* When RADIUS is used, this routine can run in either blocking mode or non-
* blocking mode.  For blocking mode, set <auth_login_hook> to 'IP_NULL'.
* For non-blocking mode, use a pointer to an 'ipcom_auth_login_hook()' routine,
* which is called when the authentication is complete.
* \i <auth_login_cookie>
* User 32-bit cookie which is supplied in the auth_login_hook().
* \ie
*
* RETURNS: 'IPCOM_SUCCESS', or one of the following errors:
* \is
* \i 'IPCOM_ERR_AUTH_FAILED'
* Valid user, bad password.
* \i 'IPCOM_ERR_PENDING'
* Non-blocking login, the authentication reply is pending and the
* auth_login_hook() will be called when a result is available.
* \i 'IPCOM_ERR_AUTH_UNKNOWN'
* Unknown user (only returned without RADIUS support).
* \ie
*
* ERRNO:
*/
IP_PUBLIC Ip_err ipcom_auth_chap_login
(
    const char *username,
    Ipcom_auth_chap *chap,
    Ipcom_auth_userinfo *userinfo,
    Ipcom_auth_login_hook auth_login_hook,
    void *auth_login_cookie
);

/*******************************************************************************
*
* ipcom_auth_hash - hash passwords
*
* Function to hash passwords.
*
* Parameter:
* \is
* \i <password>
* Pointer to a password to be hased. 
* \i <buf>
* Buffer in which to save hashed password. The buffer must be maximum 
* IPCOM_AUTH_PASSWORD_MAX bytes in length and sufficient to save   
* hashed password.
* \ie
*
* RETURNS: 'IPCOM_SUCCESS', or the following error:
* \is
* \i 'IPCOM_ERR_FAILED'.
* \ie
*
* ERRNO:
*/
IP_PUBLIC Ip_err ipcom_auth_hash
(
    const char *password,
    char *buf
);

/*******************************************************************************
*
* ipcom_auth_hash_rtn_install - install custom hash routine
*
* This function installs a hash routine and sets the digest length. 
* This function can be used to install a custom hash routin.
* Parameter:
* \is
* \i <hash_rtn>
* Function pointer to the hash routine.
* \i <digest>
* Argument to set the digest size.
* \ie
*
* RETURNS: 'IPCOM_SUCCESS', or error.
*
* ERRNO:
*/
IP_PUBLIC Ip_err ipcom_auth_hash_rtn_install
(
    Ipcom_auth_hash_hook hash_rtn, 
    Ip_size_t digest
);

/*******************************************************************************
*
* ipcom_auth_hash_get -  Get the digest length
*
* This function returns installed hash function's digest length 
* Parameter:
* \is
* \i <digest>
* Argument to return the digest size.
* \ie
*
* RETURNS: 'IPCOM_SUCCESS', or error.
*
* ERRNO:
*/
IP_PUBLIC Ip_err ipcom_auth_hash_get
( 
     Ip_size_t *digest
);

/*******************************************************************************
*
* ipcom_auth_default_hash_rtn - default password hashing routine
*
* This routine implements SHA-256 hash algorithm. If a user defined 
* encryption algorithm is needed, this routine can be replaced, as
* long as the new encryption routine retains the same declarations 
* as the default routine. The digest size is determained by used algorithm. 
*
* Parameter:
* \is
* \i <const char *password>
* Pointer to a password to be hashed.
* \i <char *buf>
* Pointer to a buffer where returned hashed password is stored.
* \ie
*
* RETURNS: 'IPCOM_SUCCESS', or the following error:
* \is
* \i 'IPCOM_ERR_FAILED'.
* \ie
*
* ERRNO:
*/
IP_PUBLIC Ip_err ipcom_auth_default_hash_rtn
(
     const char *password,
     char *buf
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

















