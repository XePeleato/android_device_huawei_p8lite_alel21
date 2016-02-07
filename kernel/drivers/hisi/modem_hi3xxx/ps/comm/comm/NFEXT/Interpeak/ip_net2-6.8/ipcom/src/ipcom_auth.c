/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_auth.c,v $ $Revision: 1.32.18.5 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/src/ipcom_auth.c,v $
 *   $Author: gzadoyan $
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
 */

/*
 ****************************************************************************
 * 2                    CONFIGURATION
 ****************************************************************************
 */

#ifndef IPCOM_KERNEL
#define IPCOM_KERNEL
#endif
#include "ipcom_config.h"

#ifdef IP_PORT_OSE5
#include "sysparam.h"
#endif


/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#define IPCOM_USE_CLIB_PROTO
#include "ipcom_clib.h"
#include "ipcom_auth.h"
#include "ipcom_err.h"
#include "ipcom_list.h"
#include "ipcom_syslog.h"
#include "ipcom_os.h"
#include "ipcom_shell.h"

#ifdef IPMCRYPTO
#include <ip/openssl/sha.h>
#include <ip/openssl/encode.h>
#endif
 
#ifdef IPCRYPTO
#include "ipcrypto_config.h"
#include <openssl/sha.h>
#include <openssl/evp.h>
#endif

/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

#define IPCOM_SYSLOG_PRIORITY    IPCOM_SYSLOG_IPCOM_PRIORITY
#define IPCOM_SYSLOG_FACILITY    IPCOM_LOG_IPCOM

/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

/*
 *===========================================================================
 *                         Ipcom_auth_user
 *===========================================================================
 */
typedef struct Ipcom_auth_user_struct
{
    Ipcom_list  list;

    char*       username;
    char*       password;
    Ip_uid_t    userid;
    Ip_bool     pwdhash; /*  Cleartext or hashed password */ 
}
Ipcom_auth_user;


/*
 *===========================================================================
 *                   Global ipcom_auth data
 *===========================================================================
 * Internal global Ipcom_auth data.
 */
typedef struct Ipcom_auth_data_struct
{
    Ipcom_mutex     mutex;
    Ipcom_list      users_head;
    Ip_u16          hash_ln;     /* Digest size */
}
Ipcom_auth_data;


/*
 ****************************************************************************
 * 6                    EXTERN PROTOTYPES
 ****************************************************************************
 */

IP_PUBLIC int ipcom_cmd_user(int argc, char **argv);


/*
 ****************************************************************************
 * 7                    LOCAL PROTOTYPES
 ****************************************************************************
 */

IP_STATIC Ip_err ipcom_auth_init(void *unused);

IP_STATIC Ip_err (*ipcom_auth_hash_rtn)(const char *password, char *buf) =
                                        ipcom_auth_default_hash_rtn;

IP_STATIC Ip_err ipcom_auth_userid_get(Ip_uid_t *userid);


/*
 ****************************************************************************
 * 8                    DATA
 ****************************************************************************
 */

/*
 *===========================================================================
 *                   Global ipcom_auth data
 *===========================================================================
 */
IP_STATIC Ipcom_auth_data   ipcom_auth;

IP_STATIC Ipcom_once_t   ipcom_auth_once = IPCOM_ONCE_INIT;

IP_STATIC Ip_err ipcom_auth_decode(const Ip_u8* in, int inl,
			           Ip_u8* out, int* outl);

/*
 ****************************************************************************
 * 9                    STATIC FUNCTIONS
 ****************************************************************************
 */


/*
 *===========================================================================
 *                    ipcom_auth_userid_get
 *===========================================================================
 * Description:   Get next available userid 
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err ipcom_auth_userid_get(Ip_uid_t *userid)
{
    Ipcom_auth_user  *user;   
    Ip_uid_t userid_tmp;
    Ip_err  err = IPCOM_SUCCESS; 
    Ip_bool flag;
    
    /* Find next available userid */
    for (userid_tmp = 0; (userid_tmp < ipcom_auth.users_head.size) &&
    (userid_tmp < (~0)); userid_tmp++)
    {    
        flag = IP_FALSE; 
        for (user = IPCOM_LIST_FIRST(&ipcom_auth.users_head); user;
                               user = IPCOM_LIST_NEXT(&user->list))
        { 
            if(userid_tmp == user->userid)   
            {
                flag = IP_TRUE; /* Userid is in use */       
                break;
            } 
        }        
        if (flag == IP_FALSE)
            break; /* Userid is not in use */ 
     }
   
    *userid = userid_tmp;
    if (*userid == (~0))
        err = IPCOM_ERR_FAILED;    
    return err;
}


 /*
 *===========================================================================
 *			ipcom_auth_decode
 *===========================================================================
 * Description: Decode base64 encoded string 
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err ipcom_auth_decode(const Ip_u8* in, int inl,
                                   Ip_u8* out, int* outl)
{
   EVP_ENCODE_CTX ctx;
   int i, outl_tmp = 0;
   
   EVP_DecodeInit(&ctx);
   i = EVP_DecodeUpdate(&ctx, out, outl, in, inl);
   outl_tmp = *outl + outl_tmp;
   if (i < 0) return IPCOM_ERR_FAILED;
   i = EVP_DecodeFinal(&ctx, out + outl_tmp, outl);
   if (i < 0) return IPCOM_ERR_FAILED;
   outl_tmp = *outl + outl_tmp;
   *outl = outl_tmp;
   return IPCOM_SUCCESS;
}

	
 /*
 *===========================================================================
 *                    ipcom_auth_init
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err
ipcom_auth_init(void *unused)
{
    Ip_err retval;

    (void)unused;

    ipcom_memset(&ipcom_auth, 0, sizeof(Ipcom_auth_data));

    ipcom_list_init(&ipcom_auth.users_head);

    retval = ipcom_mutex_create(&ipcom_auth.mutex);
    ip_assert(retval == IPCOM_SUCCESS);
    if (retval != IPCOM_SUCCESS)
        return retval;

    ipcom_auth.hash_ln = 32; /* Default algorithm is SHA-256*/

    return IPCOM_SUCCESS;
}


/*
 ****************************************************************************
 * 10                   GLOBAL FUNCTIONS
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 11                   PUBLIC FUNCTIONS
 ****************************************************************************
 */


/*
 *===========================================================================
 *                    ipcom_auth_useradd
 *===========================================================================
 * Description:   Add IPCOM user with a cleartext password.
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipcom_auth_useradd(const char *username, const char *password, Ip_uid_t userid)
{
  /* Cleartext password */
    return ipcom_auth_useradd_hash(username, password, userid, IP_FALSE);
}


/*
 *===========================================================================
 *                    ipcom_auth_useradd_hash
 *===========================================================================
 * Description:   Add IPCOM user with a hashed or cleartext password.
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipcom_auth_useradd_hash(const char *username, const char *password, Ip_uid_t userid, 
                        Ip_bool hash)
{
    Ipcom_auth_user *user;
    Ip_err  err;
    Ip_uid_t userid_tmp = ~0;
    Ip_u8 hash_buf[IPCOM_AUTH_PASSWORD_MAX]; 
    int outl;

    err = ipcom_once(&ipcom_auth_once, ipcom_auth_init, IP_NULL);
    if (err != IPCOM_SUCCESS)
        return err;

    ip_assert(username);
    ip_assert(password);

    ipcom_mutex_lock(ipcom_auth.mutex);

    if ((ipcom_strlen(username) == 0)  || (ipcom_strlen(username) > 80))  
    {
        err = IPCOM_ERR_FAILED;
        goto leave;    
    }

 /* Get a userid if not provided */
    if (userid == (~0)) 
    {
        if ((err = ipcom_auth_userid_get(&userid_tmp)) != IPCOM_SUCCESS)
            goto leave;
    }
    
    /* Check for duplicate. */
    for (user = IPCOM_LIST_FIRST(&ipcom_auth.users_head); user; user = IPCOM_LIST_NEXT(&user->list))
        if (ipcom_strcmp(username, user->username) == 0
           || user->userid == userid)
        {
            err = IPCOM_ERR_DUPLICATE;
            goto leave;
        }

    /* Create new user. */
    user = ipcom_malloc(sizeof(Ipcom_auth_user));
    ipcom_memset(user, 0, sizeof(Ipcom_auth_user));

    if (user == IP_NULL)
    { 
        err = IPCOM_ERR_NO_MEMORY;
        goto leave;
    }

    if (hash == IP_TRUE) 
    {
        if ((ipcom_auth_decode((Ip_u8*)password, (int)ipcom_strlen(password),
             hash_buf, &outl) != IPCOM_SUCCESS) || (outl != ipcom_auth.hash_ln))
	{
            ipcom_free(user);
            err = IPCOM_ERR_FAILED;
            goto leave;                
	}
        user->password = ipcom_malloc(ipcom_auth.hash_ln);        
   
        if (user->password == IP_NULL)
        {
            ipcom_free(user);
            err = IPCOM_ERR_NO_MEMORY;
            goto leave;        
        }
        
        ipcom_memcpy(user->password, hash_buf, ipcom_auth.hash_ln);    
    }
    else 
    {
        user->password = ipcom_malloc(ipcom_strlen(password) + 1);
    
        if (user->password == IP_NULL)
        {
            ipcom_free(user);
            err = IPCOM_ERR_NO_MEMORY;
            goto leave;        
        }
        
        ipcom_strcpy(user->password, password);        
    }
    
    user->username = ipcom_malloc(ipcom_strlen(username) + 1);
    if (user->username == IP_NULL)
    {
        ipcom_free(user->password);
        ipcom_free(user);        
        err = IPCOM_ERR_NO_MEMORY;
        goto leave;        
    }
    
    /* Init user. */
    ipcom_strcpy(user->username, username);
    user->pwdhash = hash; /* Hashed or cleartext */
    if (userid != ((~0))) user->userid = userid; 
    else user->userid = userid_tmp;

    ipcom_list_insert_last(&ipcom_auth.users_head, &user->list);

leave:
    ipcom_mutex_unlock(ipcom_auth.mutex);
    return err; /*lint !e429 Custodial pointer 'user' has not been freed or returned */
}


/*
 *===========================================================================
 *                    ipcom_auth_userdel
 *===========================================================================
 * Description:   Delete IPCOM user.
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipcom_auth_userdel(const char *username)
{
    Ipcom_auth_user  *user;
    Ip_err  err;

    err = ipcom_once(&ipcom_auth_once, ipcom_auth_init, IP_NULL);
    if (err != IPCOM_SUCCESS)
        return err;

    ip_assert(username);

    ipcom_mutex_lock(ipcom_auth.mutex);

    /* Find user */
    for (user = IPCOM_LIST_FIRST(&ipcom_auth.users_head); user; user = IPCOM_LIST_NEXT(&user->list))
        if (ipcom_strcmp(username, user->username) == 0)
            break;

    if (user == IP_NULL)
    {
        err = IPCOM_ERR_NOT_FOUND;
        goto leave;
    }

    ipcom_free(user->username);
    ipcom_free(user->password);

    /* Remove and free user. */
    ipcom_list_remove(&user->list);
    ipcom_free(user);

 leave:
    ipcom_mutex_unlock(ipcom_auth.mutex);
    return err;
}


/*
 *===========================================================================
 *                    ipcom_auth_userget
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipcom_auth_userget(const char *username, Ipcom_auth_userinfo *userinfo)
{
    Ipcom_auth_user  *user;
    Ip_err  err;

    err = ipcom_once(&ipcom_auth_once, ipcom_auth_init, IP_NULL);
    if (err != IPCOM_SUCCESS)
        return err;

    ip_assert(username);

    ipcom_mutex_lock(ipcom_auth.mutex);

#ifdef IP_PORT_OSE5
    {
        char user[IPCOM_AUTH_USERNAME_MAX];
        char password[IPCOM_AUTH_PASSWORD_MAX];

        if (sysparam_readattr_string("login", "username", user, sizeof(user)) == 0
            && sysparam_readattr_string("login", "password", password, sizeof(password)) == 0
            && ipcom_strcmp(user, username) == 0)
        {
            if (userinfo)
            {
                ipcom_memset(userinfo, 0, sizeof(Ipcom_auth_userinfo));
                ipcom_strcpy(userinfo->username, username);
                ipcom_strcpy(userinfo->password, password);
                userinfo->pwdhash = IP_FALSE;/* Cleartext */
            }
            goto leave;
        }
    }
#endif /* IP_PORT_OSE5 */

    /* Find user */
    for (user = IPCOM_LIST_FIRST(&ipcom_auth.users_head); user; user = IPCOM_LIST_NEXT(&user->list))
        if (ipcom_strcmp(username, user->username) == 0)
            break;

    if (user == IP_NULL)
    {
        err = IPCOM_ERR_NOT_FOUND;
        goto leave;
    }

    /* Copy over info. */
    if (userinfo)
    {
        ipcom_memset(userinfo, 0, sizeof(Ipcom_auth_userinfo));
        ipcom_strcpy(userinfo->username, user->username);
        
        if(user->pwdhash == IP_TRUE)
        {
            ipcom_memcpy(userinfo->password, user->password, 
                        (Ip_size_t)ipcom_auth.hash_ln);
        }
        else 
        ipcom_strcpy(userinfo->password, user->password);

        userinfo->userid = user->userid;
        userinfo->pwdhash = user->pwdhash;
    }

 leave:
    ipcom_mutex_unlock(ipcom_auth.mutex);
    return err;
}


/*
 *===========================================================================
 *                    ipcom_auth_userlist
 *===========================================================================
 * Description:   List users
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipcom_auth_userlist(Ip_uid_t *userid, Ipcom_auth_userinfo *userinfo)
{
    Ipcom_auth_user  *user;
    Ip_err  err;

    if (userid == IP_NULL)
        return IPCOM_ERR_INVALID_ARG;
    if (userinfo)
        ipcom_memset(userinfo, 0, sizeof(Ipcom_auth_userinfo));

    err = ipcom_once(&ipcom_auth_once, ipcom_auth_init, IP_NULL);
    if (err != IPCOM_SUCCESS)
        return IP_NULL;

    ipcom_mutex_lock(ipcom_auth.mutex);

    /* Find user */
    for (user = IPCOM_LIST_FIRST(&ipcom_auth.users_head); user; user = IPCOM_LIST_NEXT(&user->list))
        if (*userid == user->userid)
            break;

    /* No user found, return first user's userid. */
    if (user == IP_NULL)
    {
        user = IPCOM_LIST_FIRST(&ipcom_auth.users_head);
        *userid = user ? user->userid : ~0u;
        err = IPCOM_ERR_NOT_FOUND;
        goto leave;
    }

    /* Copy over userinfo. */
    if (userinfo)
    {
        ipcom_strcpy(userinfo->username, user->username);
        
        if(user->pwdhash == IP_TRUE)
            ipcom_memcpy(userinfo->password, user->password, (Ip_size_t)ipcom_auth.hash_ln);
        else 
            ipcom_strcpy(userinfo->password, user->password);        
        
        userinfo->userid = user->userid;
        userinfo->pwdhash = user->pwdhash;
    }

    /* Next user's userid. */
    user = IPCOM_LIST_NEXT(&user->list);
    *userid = user ? user->userid : ~0u;

 leave:
    ipcom_mutex_unlock(ipcom_auth.mutex);
    return err;
}


/*
 *===========================================================================
 *                    ipcom_auth_hash_rtn_install
 *===========================================================================
 * Description:   Install hash routine
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err ipcom_auth_hash_rtn_install(Ipcom_auth_hash_hook hash_rtn, 
                                             Ip_size_t digest)
{
    Ip_err  err = IPCOM_SUCCESS;    
    err = ipcom_once(&ipcom_auth_once, ipcom_auth_init, IP_NULL);
    
    if (err != IPCOM_SUCCESS)
        return err;
        
    ipcom_mutex_lock(ipcom_auth.mutex);
    ipcom_auth_hash_rtn = hash_rtn;
    ipcom_auth.hash_ln = (Ip_u16)digest;
    ipcom_mutex_unlock(ipcom_auth.mutex);
    return err;
}


/*
 *===========================================================================
 *                    ipcom_auth_hash
 *===========================================================================
 * Description:   Hash passwords
 * Parameters:    password - the input unhashed password string
 *                buf - buffer of length IPCOM_AUTH_PASSWORD_MAX in which
 *                      to store the hashed password string.
 * Returns:
 */
 IP_PUBLIC Ip_err ipcom_auth_hash(const char *password, char *buf)
 {
     if (ipcom_auth_hash_rtn == IP_NULL) 
         return ipcom_auth_default_hash_rtn(password, buf);
     else if ((*ipcom_auth_hash_rtn) (password, buf) != IPCOM_SUCCESS)
         return (IPCOM_ERR_FAILED);
     return (IPCOM_SUCCESS);
}


/*
 *===========================================================================
 *                        ipcom_auth_default_hash_rtn
 *===========================================================================
 * Description:   Default hash routine
 * Parameters: 
 * Returns:
 *
 */ 
IP_PUBLIC Ip_err ipcom_auth_default_hash_rtn(const char *password, char *buf)
{
    SHA256((const unsigned char *)password, ipcom_strlen(password), 
           (unsigned char *)buf);
    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    ipcom_auth_hash_get
 *===========================================================================
 * Description: Get current hash function digest length
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err ipcom_auth_hash_get(Ip_size_t *digest)
{
    Ip_err  err = IPCOM_SUCCESS;    
    err = ipcom_once(&ipcom_auth_once, ipcom_auth_init, IP_NULL);
    
    if (err != IPCOM_SUCCESS)
        return err;
  
    ipcom_mutex_lock(ipcom_auth.mutex);
    *digest = (Ip_size_t)ipcom_auth.hash_ln;
    ipcom_mutex_unlock(ipcom_auth.mutex);
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

