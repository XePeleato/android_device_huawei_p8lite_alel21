/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: iptftpc.c,v $ $Revision: 1.25 $
 *   $Source: /home/interpeak/CVSRoot/ipappl/src/iptftpc.c,v $
 *   $Author: gzadoyan $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Markus Carlstedt <markus@interpeak.se>
 ******************************************************************************
 */

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 * TFTP client implementation.
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
#include <ipcom_clib.h>
#include <ipcom_time.h>
#include <ipcom_getopt.h>
#include <ipcom_syslog.h>
#include <ipcom_sock2.h>

#include "iptftp_h.h"
#include "iptftp.h"


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
 ****************************************************************************
 * 6                    EXTERN PROTOTYPES
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 7                    LOCAL PROTOTYPES
 ****************************************************************************
 */

IP_PUBLIC Ip_err iptftpc_create(void);
IP_PUBLIC Ip_err iptftpc_start(void);
IP_PUBLIC const char *iptftpc_version(void);

IP_STATIC Ip_err iptftp_client_send_file(const char *src, Iptftp *tftp);
IP_STATIC Ip_err iptftp_client_receive_file(const char *dst, Iptftp *tftp);


/*
 ****************************************************************************
 * 8                    DATA
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 9                    STATIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    iptftp_client_send_file
 *===========================================================================
 * Description: Internal send function for TFTP client
 * Parameters:  src  - name of the file to send
 *              tftp - TFTP structure
 * Returns:     IPTFTP_SUCCESS for success and a TFTP error code for failures.
 */
IP_STATIC Ip_err
iptftp_client_send_file(const char *src, Iptftp *tftp)
{
    Ip_fd sock = IP_INVALID_SOCKET;
    IP_FILE *fp;
    char *err_msg = IP_NULL;
    Ip_u16 err_code = 0;
    Ip_err retval;

    if(tftp->binary)
        fp = ipcom_fopen(src, "rb");
    else
        fp = ipcom_fopen(src, "r");
    if(!fp)
    {
        retval = IPTFTP_ERR_FILE_OPEN;
        IPCOM_LOG1(ERR, "iptftp_client_send_file :: Could not open file: \"%s\"", src);
        goto err;
    }

    /* Get a udp socket, and bind to port 0 */
    sock = ipcom_socket(tftp->family, IP_SOCK_DGRAM, 0);
    if (sock == IP_INVALID_SOCKET)
    {
        retval = IPTFTP_ERR_SOCKET_FAIL;
        IPCOM_LOG0(ERR, "iptftp_client_send_file :: ipcom_socket() failed");
        goto err;
    }
    if (iptftp_bind_zero(sock, tftp->family) != IPCOM_SUCCESS)
    {
        retval = IPTFTP_ERR_SOCKET_FAIL;
        goto err;
    }

    /* Send the file */
    retval = iptftp_send_loop(sock, fp, &err_msg, &err_code, tftp);

err:
    /* Send error */
    if(err_msg)
        iptftp_send_err(sock, tftp, err_code, err_msg);

    if(fp != IP_NULL)
        (void)ipcom_fclose(fp);
    if(sock != IP_INVALID_SOCKET)
        ipcom_socketclose(sock);

    return retval;
}


/*
 *===========================================================================
 *                    iptftp_client_receive_file
 *===========================================================================
 * Description: Internal receive function for TFTP client
 * Parameters:  dst  - name of the file to receive
 *              tftp - TFTP structure
 * Returns:     IPTFTP_SUCCESS for success and a TFTP error code for failures.
 */
IP_STATIC Ip_err
iptftp_client_receive_file(const char *dst, Iptftp *tftp)
{
    Ip_fd sock = IP_INVALID_SOCKET;
    IP_FILE *fp;
    char *err_msg = IP_NULL;
    Ip_u16 err_code = 0;
    Ip_err retval;

    if(tftp->binary)
        fp = ipcom_fopen(dst, "wb");
    else
        fp = ipcom_fopen(dst, "w");
    if(!fp)
    {
        retval = IPTFTP_ERR_FILE_OPEN;
        IPCOM_LOG1(ERR, "iptftp_client_receive_file :: Could not create file: \"%s\"", dst);
        goto err;
    }

    /* Get a udp socket, and bind to port 0 */
    sock = ipcom_socket(tftp->family, IP_SOCK_DGRAM, 0);
    if (sock == IP_INVALID_SOCKET)
    {
        retval = IPTFTP_ERR_SOCKET_FAIL;
        IPCOM_LOG0(ERR, "iptftp_client_receive_file :: ipcom_socket() failed");
        goto err;
    }
    if(iptftp_bind_zero(sock, tftp->family) != IPCOM_SUCCESS)
    {
        retval = IPTFTP_ERR_SOCKET_FAIL;
        goto err;
    }

    /* Receive the file */
    retval = iptftp_receive_loop(sock, fp, &err_msg, &err_code, tftp);

err:
    /* Receive error */
    if(err_msg)
        iptftp_send_err(sock, tftp, err_code, err_msg);

    if(fp != IP_NULL)
        (void)ipcom_fclose(fp);
    if(sock != IP_INVALID_SOCKET)
        ipcom_socketclose(sock);

    return retval;
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
 *                         iptftp_client_get
 *===========================================================================
 * Description: See iptftp.h for description.
 * Parameters:
 * Returns:
 */
IP_PUBLIC Ip_err
iptftp_client_get(const char *server, const char *srcfile, const char *dstfile,
                  Ip_bool binary, Ip_bool verbose)
{
    Iptftp *tftp;
    Ip_err ret;
    char* cp = IP_NULL;
    int port = 0; 

#ifdef IPCOM_USE_TIME
    Ip_u32 msec, sec;
#endif
    struct Ip_addrinfo *res = IP_NULL;

    ip_assert(srcfile != IP_NULL);

    tftp = ipcom_malloc(sizeof(*tftp));
    if(tftp == IP_NULL)
        return IPTFTP_ERR_MEMORY_FAIL;
    ipcom_memset(tftp, 0, sizeof(*tftp));
    tftp->opcode = IPTFTP_OPCODE_RRQ;
    tftp->binary = binary;
    tftp->verbose = verbose;
    
    /* Exctruct the port number if any */
    if ((cp = ipcom_strchr(server, ':')) != IP_NULL)
    {
        /* Host name contains ":". This is the port number of the server */
        port = ipcom_atoi(cp + 1); 
        if((port <= 0) || (port > 0xFFFF)) 
        {
            ipcom_printf("Invalid port"IP_LF);           
            ret = IPTFTP_ERR_SERVER_ADDR;
            goto exit;
        }
        *cp = '\0';
    }

    if(port == 0) /* No passed port number check the sysvar */
        tftp->peer_port = ipcom_sysvar_get_as_int("iptftp.clntSrvPort", IPTFTP_PORT);
    else 
        tftp->peer_port = (Ip_u16) port;

    if (ipcom_getaddrinfo(server, IP_NULL, IP_NULL, &res) == 0)
    {
        tftp->family = res->ai_addr->sa_family;
#ifdef IPCOM_USE_INET
        if(tftp->family == IP_AF_INET)
        {
            ipcom_memcpy(&tftp->peer_addr.addr,
                         &((struct Ip_sockaddr_in *)res->ai_addr)->sin_addr,
                         sizeof(tftp->peer_addr.addr));
            goto server_ok;
        }
#endif
#ifdef IPCOM_USE_INET6
        if(tftp->family == IP_AF_INET6)
        {
            ipcom_memcpy(&tftp->peer_addr.addr6,
                         &((struct Ip_sockaddr_in6 *)res->ai_addr)->sin6_addr,
                         sizeof(tftp->peer_addr.addr6));
            tftp->ifindex = ((struct Ip_sockaddr_in6 *)res->ai_addr)->sin6_scope_id;
            goto server_ok;
        }
#endif
    }
    ret = IPTFTP_ERR_SERVER_ADDR;
    goto exit;

 server_ok:
    if(ipcom_strlen(srcfile) >= sizeof(tftp->file))
    {
        ret = IPTFTP_ERR_FILE_OPEN;
        goto exit;
    }
    else
    {
        ipcom_strcpy(tftp->file, srcfile);
        iptftp_strip_path(tftp->file);
    }
    if(!dstfile)
        dstfile = tftp->file;

#ifdef IPCOM_USE_TIME
    msec = ipcom_times(IP_NULL);
#endif
    ret = iptftp_client_receive_file(dstfile, tftp);
    if(ret == IPTFTP_SUCCESS)
    {
#ifdef IPCOM_USE_TIME
        msec = ipcom_times(IP_NULL) - msec;
        sec = msec / 1000;
        msec = (msec % 1000) / 100;     /* Tenths of second */
        if(verbose)
            ipcom_printf("Transfer completed: %ld bytes in %ld.%ld seconds."IP_LF, tftp->nbytes, sec, msec);
#else
        if(verbose)
            ipcom_printf("Transfer completed: %ld bytes."IP_LF, tftp->nbytes);
#endif
    }

exit:
    if(res != IP_NULL)
        ipcom_freeaddrinfo(res);
    ipcom_free(tftp);
    return ret;
}


/*
 *===========================================================================
 *                         iptftp_client_put
 *===========================================================================
 * Description: See iptftp.h for description.
 * Parameters:
 * Returns:
 */
IP_PUBLIC Ip_err
iptftp_client_put(const char *server, const char *srcfile, const char *dstfile,
                  Ip_bool binary, Ip_bool verbose)
{
    Iptftp *tftp;
    Ip_err ret;
    char* cp = IP_NULL;
    int port = 0; 

#ifdef IPCOM_USE_TIME
    Ip_u32 msec, sec;
#endif
    struct Ip_addrinfo *res = IP_NULL;

    ip_assert(srcfile != IP_NULL);

    tftp = ipcom_malloc(sizeof(*tftp));
    if(tftp == IP_NULL)
        return IPTFTP_ERR_MEMORY_FAIL;
    ipcom_memset(tftp, 0, sizeof(*tftp));
    tftp->opcode = IPTFTP_OPCODE_WRQ;
    tftp->binary = binary;
    tftp->verbose = verbose;
    
    /* Exctruct the port number if any */
    if ((cp = ipcom_strchr(server, ':')) != IP_NULL)
    {
        /* Host name contains ":". This is the port number of the server */
        port = ipcom_atoi(cp + 1); 
        if((port <= 0) || (port > 0xFFFF)) 
        {
            ipcom_printf("Invalid port"IP_LF);           
            ret = IPTFTP_ERR_SERVER_ADDR;
            goto exit;
        }
        *cp = '\0';
    }

    if(port == 0) /* No passed port number check the sysvar */    
        tftp->peer_port = ipcom_sysvar_get_as_int("iptftp.clntSrvPort", IPTFTP_PORT);
    else 
        tftp->peer_port = (Ip_u16) port;

    if (ipcom_getaddrinfo(server, IP_NULL, IP_NULL, &res) == 0
        && res != IP_NULL && res->ai_addr != IP_NULL)
    {
        tftp->family = res->ai_addr->sa_family;
#ifdef IPCOM_USE_INET
        if(tftp->family == IP_AF_INET)
        {
            ipcom_memcpy(&tftp->peer_addr.addr,
                         &((struct Ip_sockaddr_in *)res->ai_addr)->sin_addr,
                         sizeof(tftp->peer_addr.addr));
            goto server_ok;
        }
#endif
#ifdef IPCOM_USE_INET6
        if(tftp->family == IP_AF_INET6)
        {
            ipcom_memcpy(&tftp->peer_addr.addr6,
                         &((struct Ip_sockaddr_in6 *)res->ai_addr)->sin6_addr,
                         sizeof(tftp->peer_addr.addr6));
            tftp->ifindex = ((struct Ip_sockaddr_in6 *)res->ai_addr)->sin6_scope_id;
            goto server_ok;
        }
#endif
    }
    ret = IPTFTP_ERR_SERVER_ADDR;
    goto exit;

 server_ok:
    if(!dstfile)
        dstfile = srcfile;
    if(ipcom_strlen(dstfile) >= sizeof(tftp->file))
    {
        ret = IPTFTP_ERR_FILE_OPEN;
        goto exit;
    }
    else
    {
        ipcom_strcpy(tftp->file, dstfile);
        iptftp_strip_path(tftp->file);
    }

#ifdef IPCOM_USE_TIME
    msec = ipcom_times(IP_NULL);
#endif
    ret = iptftp_client_send_file(srcfile, tftp);
    if(ret == IPTFTP_SUCCESS)
    {
#ifdef IPCOM_USE_TIME
        msec = ipcom_times(IP_NULL) - msec;
        sec = msec / 1000;
        msec = (msec % 1000) / 100;     /* Tenths of second */
        if(verbose)
            ipcom_printf("Transfer completed: %ld bytes in %ld.%ld seconds."IP_LF, tftp->nbytes, sec, msec);
#else
        if(verbose)
            ipcom_printf("Transfer completed: %ld bytes."IP_LF, tftp->nbytes);
#endif
    }

exit:
    if(res != IP_NULL)
        ipcom_freeaddrinfo(res);
    ipcom_free(tftp);
    return ret;
}


/*
 *===========================================================================
 *                    iptftpc_create
 *===========================================================================
 * Description: TFTP Client create function.
 * Parameters:  none.
 * Returns:     IPCOM_SUCCESS or an IPCOM_ERR_XXX error code.
 */
IP_PUBLIC Ip_err
iptftpc_create(void)
{
    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    iptftpc_start
 *===========================================================================
 * Description: TFTP Client start function.
 * Parameters:  none.
 * Returns:     IPCOM_SUCCESS or an IPCOM_ERR_XXX error code.
 */
IP_PUBLIC Ip_err
iptftpc_start(void)
{
    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    iptftpc_version
 *===========================================================================
 * Description: Get TFTP client version string.
 * Parameters:  none.
 * Returns:     TFTP client version.
 */
IP_PUBLIC const char *
iptftpc_version(void)
{
    return "@(#) IPTFTPC $Name: VXWORKS_ITER18A_FRZ10 $ - INTERPEAK_COPYRIGHT_STRING";
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
