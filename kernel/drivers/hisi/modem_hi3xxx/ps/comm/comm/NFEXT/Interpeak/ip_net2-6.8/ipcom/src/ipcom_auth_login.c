/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_auth_login.c,v $ $Revision: 1.19.24.2 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/src/ipcom_auth_login.c,v $
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

#include "ipcom_config.h"

#ifdef IPCOM_USE_MD5
#define IPCOM_USE_AUTH_CHAP
#endif


/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#define IPCOM_USE_CLIB_PROTO
#include "ipcom_type.h"
#include "ipcom_cstyle.h"
#include "ipcom_clib.h"
#include "ipcom_auth.h"
#include "ipcom_err.h"
#include "ipcom_list.h"
#include "ipcom_syslog.h"
#include "ipcom_os.h"
#include "ipcom_shell.h"

#ifdef IPCOM_USE_AUTH_CHAP
#ifdef IPMCRYPTO
#include <ip/openssl/md5.h>
#else
#include <openssl/md5.h>
#endif
#endif

#ifdef IPCOM_USE_AUTH_RADIUS
#include "ipcom_inet.h"
#include <ipradius.h>
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
 *                    Ipcom_msg_auth
 *===========================================================================
 */
typedef struct Ipcom_msg_auth_struct
{
    int       len;             /* Length of data[] contents. */
    Ip_u8     data[1];         /* Variable length data holder. */
}
Ipcom_msg_auth;


/*
 *===========================================================================
 *                         Ipcom_auth_cookie
 *===========================================================================
 */
typedef struct Ipradius_auth_block_struct
{
    Ipcom_sem   sem;
    Ip_err      retval;

    Ipcom_auth_userinfo   userinfo;

    Ipcom_auth_login_hook auth_login_hook;
    void                 *auth_login_cookie;
}
Ipcom_auth_cookie;


/*
 *===========================================================================
 *                   Global ipcom_auth_login data
 *===========================================================================
 * Internal global Ipcom_auth data.
 */
#ifdef IPCOM_USE_AUTH_RADIUS
typedef struct Ipcom_auth_login_data_struct
{
    /* AUTH RADIUS. */
    Ip_fd                    radius_fd;
    Ip_u8                    radius_challenge[16];
    union Ip_sockaddr_union  radius_server_addr;
}
Ipcom_auth_login_data;
#endif


/*
 ****************************************************************************
 * 6                    EXTERN PROTOTYPES
 ****************************************************************************
 */

#ifdef IPCOM_USE_AUTH_RADIUS
IP_EXTERN IP_CONST int ipcom_auth_radius_server_domain;
IP_EXTERN IP_CONST char *ipcom_auth_radius_server_address;
IP_EXTERN IP_CONST char *ipcom_auth_radius_server_secret;
IP_EXTERN IP_CONST Ip_u16 ipcom_auth_radius_server_port;
IP_EXTERN IP_CONST Ip_u32 ipcom_auth_radius_server_pwdtype;
#endif


/*
 ****************************************************************************
 * 7                    LOCAL PROTOTYPES
 ****************************************************************************
 */

#ifdef IPCOM_USE_AUTH_RADIUS
IP_STATIC Ip_err ipcom_auth_login_init(void *unused);
IP_STATIC Ip_err ipcom_auth_radius_init(void);

IP_STATIC Ip_err
ipcom_auth_radius_event_callback(int event, Ipradius_request *request,
                                 Ipradius_header *request_packet, Ipradius_header *response_packet,
                                 Ipradius_server *server);

IP_STATIC IPCOM_PROCESS(ipcom_authd);
#endif


/*
 ****************************************************************************
 * 8                    DATA
 ****************************************************************************
 */

/*
 *===========================================================================
 *                   Global ipcom_authlogin data
 *===========================================================================
 */
#ifdef IPCOM_USE_AUTH_RADIUS
IP_STATIC Ipcom_auth_login_data   ipcom_authlogin;

IP_STATIC Ipcom_once_t   ipcom_auth_login_once = IPCOM_ONCE_INIT;
#endif


/*
 ****************************************************************************
 * 9                    STATIC FUNCTIONS
 ****************************************************************************
 */


#ifdef IPCOM_USE_AUTH_RADIUS
/*
 *===========================================================================
 *                    ipcom_auth_login_init
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err
ipcom_auth_login_init(void *unused)
{
    IPCOM_UNUSED_ARG(unused);

    ipcom_memset(&ipcom_authlogin, 0, sizeof(Ipcom_auth_login_data));

    /* Signifies disabled RADIUS client. */
    ipcom_authlogin.radius_fd = IP_INVALID_SOCKET;

    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    ipcom_auth_radius_init
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err
ipcom_auth_radius_init(void)
{
    Ipradius_server server;
    int err;
    union Ip_sockaddr_union  addr;
    Ip_err  retval;
    Ip_socklen_t    salen;

    if (ipcom_authlogin.radius_fd != IP_INVALID_SOCKET)
        return IPCOM_SUCCESS;

    /* Radius challenge. */
    ipcom_strcpy((char *)ipcom_authlogin.radius_challenge, "123456789012345");


    /***** First resolve the RADIUS server IPv4/IPv6 address. *****/
    err = ipcom_getsockaddrbyaddrname(ipcom_auth_radius_server_domain,
                                      IP_FALSE /* use DNS */,
                                      ipcom_auth_radius_server_address,
                                      &ipcom_authlogin.radius_server_addr.sa);
    if (err)
    {
        IPCOM_LOG3(ERR, "ipcom_auth_radius_init :: invalid 'ipcom_auth_radius_server_address=%s' from ipcom_config.c, domain = %d, err = %d",
                   ipcom_auth_radius_server_address, ipcom_auth_radius_server_domain, err);
        return IPCOM_SUCCESS;  /* Do not halt system, just disable RADIUS login. */
    }

    /* Cache radius server port. */
    ipcom_authlogin.radius_server_addr.sin.sin_port = ip_htons(ipcom_auth_radius_server_port);

    /* Get RADIUS client socket. */
    ipcom_authlogin.radius_fd = ipcom_socket(ipcom_auth_radius_server_domain, IP_SOCK_DGRAM, 0);
    if (ipcom_authlogin.radius_fd == IP_INVALID_SOCKET)
    {
        IPCOM_LOG1(ERR, "ipcom_auth_radius_init :: ipcom_socket() failed, errno=%d", ipcom_errno);
        goto fail;
    }

    /* Bind socket to local random port. */
    ipcom_memset(&addr, 0, sizeof(addr));

    IPCOM_SA_LEN_SET(&addr, ipcom_authlogin.radius_server_addr.sa.sa_len);
    addr.sin.sin_family      = (Ip_sa_family_t)ipcom_auth_radius_server_domain;
    salen = IPCOM_SA_LEN_GET(&ipcom_authlogin.radius_server_addr.sa);
    if (ipcom_bind(ipcom_authlogin.radius_fd, &addr.sa, salen) == IP_SOCKERR)
    {
        IPCOM_LOG1(ERR, "ipcom_auth_radius_init :: ipcom_bind() failed, errno=%d", ipcom_errno);
        goto fail;
    }

    /* Add IPCOM AUTH RADIUS server. */
    server.index = 0;  /* let ipradius choose server index. */
    ipcom_memcpy(&server.server_addr, &ipcom_authlogin.radius_server_addr, sizeof(ipcom_authlogin.radius_server_addr));
    server.server_type = IPRADIUS_ACCESS_SERVER;
    server.pollnum = 3;
    server.pollsec = 3;
    ipcom_strcpy((char *)server.shared_secret, ipcom_auth_radius_server_secret);
    server.shared_secret_len = ipcom_strlen((char *)server.shared_secret);
    retval = ipradius_server_add(&server);
    if (retval != IPCOM_SUCCESS)
    {
        IPCOM_LOG2(ERR, "ipcom_auth_radius_init :: ipradius_server_add(%s) failed, err=%ld",
                   ipcom_auth_radius_server_address, retval);
        goto fail;
    }

    /* The auth deamon can only be started after
       ipcom_authlogin.radius_fd contains a valid socket descriptor */
    retval = ipcom_proc_acreate("ipcom_authd", ipcom_authd, IP_NULL, IP_NULL);
    if (retval != IPCOM_SUCCESS)
        return retval;

    return IPCOM_SUCCESS;

 fail:
    if (ipcom_authlogin.radius_fd != IP_INVALID_SOCKET)
        ipcom_socketclose(ipcom_authlogin.radius_fd);
    ipcom_authlogin.radius_fd = IP_INVALID_SOCKET;
    return IPCOM_ERR_FAILED;
}


/*
 *===========================================================================
 *                    ipcom_auth_radius_event_callback
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err
ipcom_auth_radius_event_callback(int event, Ipradius_request *request,
                                Ipradius_header *request_packet, Ipradius_header *response_packet,
                                Ipradius_server *server)
{
    Ipcom_auth_cookie   *auth_cookie = request->cookie;
    Ip_u8 *attrp;
    Ip_u8  attrlen;

    (void)response_packet;
    (void)server;

    ipcom_memset(&auth_cookie->userinfo, 0 , sizeof(Ipcom_auth_userinfo));
    switch(event)
    {
    case IPRADIUS_EVENT_ACCESS_ACCEPT :
        auth_cookie->retval = IPCOM_SUCCESS;
        attrp = ipradius_attr_find(request_packet, IPRADIUS_ATTR_User_Name, &attrlen);
        if (attrp != IP_NULL)
            ipcom_strncpy(auth_cookie->userinfo.username, (char *)attrp, attrlen - 2);
        break;

    case IPRADIUS_EVENT_ACCESS_REJECT :
    case IPRADIUS_EVENT_ACCESS_TIMEOUT :
    case IPRADIUS_EVENT_ACCESS_CANCELLED :
        auth_cookie->retval = IPCOM_ERR_AUTH_FAILED;
        break;

    default:
        IP_PANIC();
        break;
    }

    /* Copy over RADIUS attributes. */
    if (response_packet != IP_NULL)
    {
        int  len;

        len = (int)ip_ntohs(response_packet->length) - IPRADIUS_HDRSIZE;
        if (len > 0)
        {
            auth_cookie->userinfo.radius_attributes = ipcom_malloc(len);
            if (auth_cookie->userinfo.radius_attributes != IP_NULL)
            {
                ipcom_memcpy(auth_cookie->userinfo.radius_attributes, response_packet->attr, len);
                auth_cookie->userinfo.radius_attrlen = len;
            }
        }
    }

    /* Wake up process waiting for auth reply. */
    if (auth_cookie->auth_login_hook != IP_NULL)
    {
        /* Non-blocking mode. */
        auth_cookie->auth_login_hook(auth_cookie->retval,
                                     &auth_cookie->userinfo,
                                     auth_cookie->auth_login_cookie);
        ipcom_free(auth_cookie);
    }
    else
    {
        /* blocking mode. */
        (void)ipcom_sem_post(auth_cookie->sem);
    }

    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    ipcom_auth_radius_request
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err
ipcom_auth_radius_request(const char *username, const char *password, Ipcom_auth_chap *chap,
                          Ipcom_auth_userinfo *userinfo,
                          Ipcom_auth_login_hook auth_login_hook, void *auth_login_cookie)
{
    Ipradius_request  request;
    Ipradius_header  *packet;
    Ip_err retval;
    Ip_u8 errattr;
    Ipcom_auth_cookie   *auth_cookie;

    /* Init auth login (once) */
    retval = ipcom_once(&ipcom_auth_login_once, ipcom_auth_login_init, IP_NULL);
    if (retval != IPCOM_SUCCESS)
        return retval;

    /* Init radius (may not happen every time) */
    if (ipcom_auth_radius_init() == IPCOM_ERR_FAILED)
        return IPCOM_ERR_AUTH_UNKNOWN;

    auth_cookie = ipcom_malloc(sizeof(Ipcom_auth_cookie));
    if (auth_cookie == IP_NULL)
        return IPCOM_ERR_NO_MEMORY;
    ipcom_memset(auth_cookie, 0, sizeof(Ipcom_auth_cookie));
    auth_cookie->auth_login_hook = auth_login_hook;
    auth_cookie->auth_login_cookie = auth_login_cookie;

    /* Get RADIUS client request packet memory. */
    packet = ipradius_access_request_pkt_malloc();
    if (packet == IP_NULL)
    {
        ipcom_free(auth_cookie);
        return IPCOM_ERR_NO_MEMORY;
    }

    /***** Add attributes *****/
    (void)ipradius_attr_add_array(packet, IPRADIUS_ATTR_User_Name, (Ip_u8 *)username, (Ip_u8)ipcom_strlen(username));
#ifdef IPCOM_USE_AUTH_CHAP
    if (chap != IP_NULL)
    {
        Ip_u8   chap_password[MD5_DIGEST_LENGTH+1];

        (void)ipradius_attr_add_array(packet, IPRADIUS_ATTR_CHAP_Challenge, (Ip_u8 *)chap->challenge, (Ip_u8)chap->challenge_size);
        chap_password[0] = chap->ident;
        ipcom_memcpy(&chap_password[1], chap->response, MD5_DIGEST_LENGTH);
        (void)ipradius_attr_add_array(packet, IPRADIUS_ATTR_CHAP_Password, chap_password, MD5_DIGEST_LENGTH + 1);
    }
    else
    {
        if (ipcom_auth_radius_server_pwdtype == IPRADIUS_CHAP_Password)
            (void)ipradius_attr_add_array(packet, IPRADIUS_ATTR_CHAP_Challenge, ipcom_authlogin.radius_challenge, 16);
    }
#endif /* IPCOM_USE_AUTH_CHAP */

    /* Blocking mode, create blocking semaphore. */
    if (auth_login_hook == IP_NULL)
    {
        if (ipcom_sem_create(&auth_cookie->sem, 0) != IPCOM_SUCCESS)
        {
            ipcom_free(auth_cookie);
            (void)ipradius_access_request_pkt_free(packet);
            return IPCOM_ERR_NO_MEMORY;
        }
    }

    /***** Fill in request structure and send access request. *****/
    request.cookie = auth_cookie;
    request.unique = ipcom_random();
#ifdef IPCOM_USE_AUTH_CHAP
    if (chap != IP_NULL)
    {
        request.pwd_type = IPRADIUS_CHAP_Password;
        request.pwd_len = 0;
        request.chap_ident = chap->ident;
    }
    else
#endif /* IPCOM_USE_AUTH_CHAP */
    {
        request.pwd_type = ipcom_auth_radius_server_pwdtype;
        ipcom_strcpy(request.pwd, password);
        request.pwd_len = ipcom_strlen(request.pwd);
        request.chap_ident = 1;  /*!!always 1, does this matter? */
    }

    /* Set RADIUS socket descriptor. */
#if defined(IPCOM_USE_INET) && defined(IPCOM_USE_INET6)
    request.fd  = IP_INVALID_SOCKET;
    request.fd6 = IP_INVALID_SOCKET;
    if (ipcom_authlogin.radius_server_addr.sa.sa_family == IP_AF_INET)
        request.fd = ipcom_authlogin.radius_fd;
    if (ipcom_authlogin.radius_server_addr.sa.sa_family == IP_AF_INET6)
        request.fd6 = ipcom_authlogin.radius_fd;
#elif defined(IPCOM_USE_INET)
    request.fd = ipcom_authlogin.radius_fd;
#elif defined(IPCOM_USE_INET6)
    request.fd6 = ipcom_authlogin.radius_fd;
#else
#error IPCOM_AUTH_MUST_USE_INET_OR_INET6
#endif
    request.event_callback = ipcom_auth_radius_event_callback;

    /* IP address or port specified, set a desired server. */
    request.num_servers = 1;
    ipcom_memcpy(&request.serv_list[0].server_addr,
                 &ipcom_authlogin.radius_server_addr.sa,
                 IPCOM_SA_LEN_GET(&ipcom_authlogin.radius_server_addr.sa));
    request.serv_list[0].server_type = IPRADIUS_ACCESS_SERVER;
    request.serv_list[0].pollnum = 3;
    request.serv_list[0].pollsec = 3;

    retval = ipradius_access_request(packet, &request, &errattr);
    if (retval != IPCOM_SUCCESS)
    {
        IPCOM_LOG2(ERR, "ipcom_auth_radius_request() :: ipradius_access_request() failed:  err=%ld, attr=%d",
                   retval, errattr);
        return IPCOM_ERR_FAILED;
    }

    if (auth_login_hook == IP_NULL)
    {
        /* Blocking mode, wait for RADIUS result. */
        (void)ipcom_sem_wait(auth_cookie->sem);

        retval = auth_cookie->retval;

        if (userinfo != IP_NULL)
            ipcom_memcpy(userinfo, &auth_cookie->userinfo, sizeof(Ipcom_auth_userinfo));

        (void)ipcom_sem_delete(&auth_cookie->sem);
        ipcom_free(auth_cookie);

        return retval;
    }

    /* Non-blocking mode, result will come later. */
    return IPCOM_ERR_PENDING;
}


/*
 *===========================================================================
 *                    ipcom_authd
 *===========================================================================
 * Description:
 *
 */
IP_STATIC
IPCOM_PROCESS(ipcom_authd)
{
    Ip_err    retval = IPCOM_ERR_FAILED;
    Ip_fd_set rd_set;
    int       num_ready_socks;

    ipcom_proc_init();

    IP_FD_ZERO(&rd_set);
    /*********** Eternal radius respose loop **********/
    for(;;)
    {
        int radius_sock = ipcom_authlogin.radius_fd;

        if (IP_UNLIKELY(radius_sock < 0))
            break;

        IP_FD_SET(radius_sock, &rd_set);
        num_ready_socks = ipcom_socketselect(radius_sock + 1,
                                             &rd_set,
                                             IP_NULL,
                                             IP_NULL,
                                             IP_NULL);
        if (num_ready_socks < 0)
        {
            if (ipcom_errno == IP_ERRNO_EINTR)
                continue;
            else
            {
                IPCOM_LOG1(ERR, "ipcom_authd :: select failed %s", ipcom_strerror(ipcom_errno));
                break;
            }
        }

        if (IP_FD_ISSET(radius_sock, &rd_set))
        {
            Ipcom_msg_auth *authmsg;
            int             msglen;

            if (ipcom_socketioctl(radius_sock, IP_FIONREAD, &msglen) < 0
                || IP_NULL == (authmsg = ipcom_malloc(sizeof(Ipcom_msg_auth) + msglen)))
            {
                /* Just drop the message */
                (void)ipcom_recv(radius_sock, IP_NULL, 0, 0);
                continue;
            }

            authmsg->len = ipcom_recv(radius_sock, authmsg->data, msglen, 0);
            if (authmsg->len > 0)
                /* Parse RADIUS input packet. */
                (void)ipradius_read(authmsg->data,
                                    (Ip_size_t)authmsg->len,
                                    &ipcom_authlogin.radius_server_addr.sa);
            else
                IPCOM_LOG1(WARNING, "ipcom_authd :: recv() failed: %s", ipcom_strerror(ipcom_errno));

            ipcom_free(authmsg);
        }
    }

    /* Failure. */
    IPCOM_LOG1(ERR, "ipcom_authd :: failed, retval = %d", retval);
    IP_PANIC();
    ipcom_proc_exit();
}
#endif /*#ifdef IPCOM_USE_AUTH_RADIUS */


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
 *                    ipcom_auth_chap_login
 *===========================================================================
 * Description:  Login 'username' with CHAP info 'chap'.
 *               For blocking mode - set auth_login_hook == IP_NULL
 * Parameters:
 * Returns:      IPCOM_SUCCESS - valid user and password.
 *               IPCOM_ERR_AUTH_FAILED - valid user, bad password.
 *               IPCOM_ERR_AUTH_UNKNOWN - unknown user.
 */
IP_PUBLIC Ip_err
ipcom_auth_chap_login(const char *username, Ipcom_auth_chap *chap,
                      Ipcom_auth_userinfo *userinfo,
                      Ipcom_auth_login_hook auth_login_hook, void *auth_login_cookie)
{
#ifdef IPCOM_USE_AUTH_CHAP
    Ipcom_auth_userinfo  user;
    Ip_err  err;

    if (username == IP_NULL || *username == '\0')
        return IPCOM_ERR_INVALID_ARG;
    if (chap == IP_NULL)
        return IPCOM_ERR_INVALID_ARG;

    if (userinfo != IP_NULL)
        ipcom_memset(userinfo, 0, sizeof(Ipcom_auth_userinfo));

    /* Check local userdata base. */
    err = ipcom_auth_userget(username, &user);    
    if (err == IPCOM_SUCCESS)
    {
        MD5_CTX   ctx;
        Ip_u8     calc_response[MD5_DIGEST_LENGTH];

	/* User password should be in cleartext only */   
        if(user.pwdhash == IP_TRUE) 
        { 
           IPCOM_LOG1(WARNING, "ipcom_auth_chap_login :: user with a hashed password: %u", user.userid);
           return IPCOM_ERR_AUTH_UNKNOWN;     
        }

        if (!MD5_Init(&ctx))
            return IPCOM_ERR_NOT_SUPPORTED;
        (void)MD5_Update(&ctx, &chap->ident, 1);
        (void)MD5_Update(&ctx, user.password, ipcom_strlen(user.password));
        (void)MD5_Update(&ctx, chap->challenge, chap->challenge_size);
        (void)MD5_Final(calc_response, &ctx);

        if (ipcom_memcmp(chap->response, calc_response, MD5_DIGEST_LENGTH))
            return IPCOM_ERR_AUTH_FAILED;
        if (userinfo != IP_NULL)
            ipcom_memcpy(userinfo, &user, sizeof(Ipcom_auth_userinfo));
        return IPCOM_SUCCESS;
    }

#ifdef IPCOM_USE_AUTH_RADIUS
    return ipcom_auth_radius_request(username, IP_NULL, chap,
                                     userinfo,
                                     auth_login_hook, auth_login_cookie);
#else
    (void)auth_login_hook;
    (void)auth_login_cookie;
    return IPCOM_ERR_AUTH_UNKNOWN;
#endif

#else
    (void)username;
    (void)chap;
    (void)userinfo;
    (void)auth_login_hook;
    (void)auth_login_cookie;
    return IPCOM_ERR_AUTH_UNKNOWN;
#endif /* IPCOM_USE_AUTH_CHAP */
}


/*
 *===========================================================================
 *                    ipcom_auth_login
 *===========================================================================
 * Description:  Login 'username' with password 'password' in a cleartext or hashed format.
 *               For blocking mode - set auth_login_hook == IP_NULL
 * Parameters:
 * Returns:      IPCOM_SUCCESS - valid user and password.
 *               IPCOM_ERR_AUTH_FAILED - valid user, bad password.
 *               IPCOM_ERR_AUTH_UNKNOWN - unknown user.
 */
IP_PUBLIC Ip_err
ipcom_auth_login(const char *username, const char *password,
                 Ipcom_auth_userinfo *userinfo,
                 Ipcom_auth_login_hook auth_login_hook, void *auth_login_cookie)
{
    Ipcom_auth_userinfo  user;
    Ip_err  err;
    char hashbuf[IPCOM_AUTH_PASSWORD_MAX]; 
    Ip_size_t size;    

    if (username == IP_NULL || *username == '\0')
        return IPCOM_ERR_INVALID_ARG;
    if (password == IP_NULL || *password == '\0')
        return IPCOM_ERR_INVALID_ARG;

    if (userinfo != IP_NULL)
        ipcom_memset(userinfo, 0, sizeof(Ipcom_auth_userinfo));

    /* Check local userdata base */
    err = ipcom_auth_userget(username, &user);      

    if (err == IPCOM_SUCCESS)
    { 
      if (user.pwdhash == IP_TRUE) /* Hashed password */
        {
            if(ipcom_auth_hash(password, hashbuf) != IPCOM_SUCCESS)
                return IPCOM_ERR_FAILED;

            (void)ipcom_auth_hash_get(&size);
            if (ipcom_memcmp(hashbuf, user.password, size) != 0)
            return IPCOM_ERR_AUTH_FAILED;
        }
        else /* Cleartext password */
	{
            if (ipcom_strcmp(password, user.password))
            return IPCOM_ERR_AUTH_FAILED;
        }

        if (userinfo != IP_NULL)
            ipcom_memcpy(userinfo, &user, sizeof(Ipcom_auth_userinfo));
        return IPCOM_SUCCESS;
    }

#ifdef IPCOM_USE_AUTH_RADIUS
    return ipcom_auth_radius_request(username, password, IP_NULL,
                                     userinfo,
                                     auth_login_hook, auth_login_cookie);
#else
    (void)auth_login_hook;
    (void)auth_login_cookie;
    return IPCOM_ERR_AUTH_UNKNOWN;
#endif
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

