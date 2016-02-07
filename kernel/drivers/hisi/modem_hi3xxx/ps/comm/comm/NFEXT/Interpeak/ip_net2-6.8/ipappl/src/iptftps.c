/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: iptftps.c,v $ $Revision: 1.24 $
 *   $Source: /home/interpeak/CVSRoot/ipappl/src/iptftps.c,v $
 *   $Author: jhorteli $
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
 * TFTP server implementation.
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

#define IPCOM_USE_CLIB_PROTO
#include <ipcom_ipd.h>

#include "iptftp_h.h"


/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

/* Specifies if TFTP write requests are permitted */
#define IPTFTP_SERVER_ALLOW_UPLOAD

/* Specifies if TFTP read requests are permitted */
#define IPTFTP_SERVER_ALLOW_DOWNLOAD


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

IP_PUBLIC Ip_err iptftps_create(void);
IP_PUBLIC Ip_err iptftps_start(void);
IP_PUBLIC const char *iptftps_version(void);

IP_STATIC IPCOM_PROCESS(iptftps);
IP_STATIC Ip_err iptftp_server_spawn_handler(Iptftp *tftp);
IP_STATIC Ip_err iptftp_server_parse_request(Ip_fd sock, Ip_s32 family, Iptftp *tftp);
IP_STATIC void iptftp_server_send_file(Iptftp *tftp);
IP_STATIC void iptftp_server_receive_file(Iptftp *tftp);
IP_STATIC IPCOM_PROCESS(iptftp_server_handler);


/*
 ****************************************************************************
 * 8                    DATA
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 9                    LOCAL FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    iptftp_server_handler
 *===========================================================================
 * Description: Spawned TFTP handler.
 * Parameters:  none.
 * Returns:     nothing.
 */
IP_STATIC
IPCOM_PROCESS(iptftp_server_handler)
{
    Ip_bool ipc_opened = IP_FALSE;
    Ipcom_ipc  ipc;
    Iptftp    *ipc_tftp = IP_NULL;

    ipcom_proc_init();

    if (ipcom_ipc_open(&ipc, "iptftps", -1) != IPCOM_SUCCESS)
    {
        IPCOM_LOG0(ERR, "iptftp_server_handler :: ipcom_ipc_open(\"ipwebsd\") failed");
        goto exit;
    }
    ipc_opened = IP_TRUE;

    ipc_tftp = ipcom_ipc_malloc(sizeof(*ipc_tftp));
    if (ipc_tftp == IP_NULL)
    {
        IPCOM_LOG0(ERR, "iptftp_server_handler :: ipcom_ipc_malloc() failed");
        goto exit;
    }

    /* Get info from tftp server. */
    if (ipcom_ipc_send(&ipc, ipc_tftp) != IPCOM_SUCCESS)
    {
        IPCOM_LOG0(ERR, "iptftp_server_handler :: ipcom_ipc_send() failed");
        goto exit;
    }

    /* Wait for reply. */
    if (ipcom_ipc_receive(IP_NULL, &ipc_tftp, -1) != IPCOM_SUCCESS)
    {
        IPCOM_LOG0(ERR, "iptftp_server_handler :: ipcom_ipc_receive() failed");
        goto exit;
    }

    if (ipc_tftp->opcode == IPTFTP_OPCODE_RRQ)
    {
        iptftp_server_send_file(ipc_tftp);
    }
    else if (ipc_tftp->opcode == IPTFTP_OPCODE_WRQ)
    {
        iptftp_server_receive_file(ipc_tftp);
    }

exit:
    if (ipc_opened)
        (void)ipcom_ipc_close(&ipc);
    if (ipc_tftp != IP_NULL)
        ipcom_ipc_free(ipc_tftp);

    ipcom_proc_exit();
}


/*
 *===========================================================================
 *                    iptftp_server_spawn_handler
 *===========================================================================
 * Description: Called by the tftp server to spawn a new handler for each
 *              connection
 * Parameters:  sock       - The socket the connection arrived on.
 *              family     - The Internet address family.
 *              srv_handle - handle for ipcom call messages
 * Returns:     IPCOM_SUCCESS or IPCOM_ERR_FAILED
 */
IP_STATIC Ip_err
iptftp_server_spawn_handler(Iptftp *tftp)
{
    /* Spawn a tftp handler */
    if (ipcom_proc_create("iptftp_server_handler", (Ipcom_proc_func)iptftp_server_handler, IPCOM_PROC_STACK_DEFAULT, IP_NULL))
    {
        IPCOM_LOG0(ERR, "iptftp_server_spawn_handler :: ipcom_proc_create() failed");
        goto err;
    }
    else
    {
        /* Wait for a message from the handler */
        Iptftp   *ipc_tftp;

        if (ipcom_ipc_receive(IP_NULL, &ipc_tftp, -1) != IPCOM_SUCCESS)
        {
            IPCOM_LOG0(ERR, "iptftp_server_spawn_handler :: ipcom_ipc_receive() failed");
            goto err;
        }

        /* Respond to handler */
        ipcom_memcpy(ipc_tftp, tftp, sizeof(*tftp));
        if (ipcom_ipc_send(IPCOM_IPC_SENDER, ipc_tftp) != IPCOM_SUCCESS)
        {
            IPCOM_LOG0(ERR, "iptftp_server_spawn_handler :: ipcom_ipc_send() failed");
            goto err;
        }
    }

    return IPCOM_SUCCESS;

err:
    return IPCOM_ERR_FAILED;
}


/*
 *===========================================================================
 *                    iptftp_server_parse_request
 *===========================================================================
 * Description: Parses a tftp request and calls the request handler
 * Parameters:  sock   - socket descriptor the request arrived on.
 *              family - Internet address family for the request.
 *              tftp   - TFTP structure.
 * Returns:     IPCOM_SUCCESS if ok and an IPCOM_ERR_XXX error code for
 *              failures.
 */
IP_STATIC Ip_err
iptftp_server_parse_request(Ip_fd sock, Ip_s32 family, Iptftp *tftp)
{
#ifdef IPCOM_USE_INET
    struct Ip_sockaddr_in sa;
#endif
#ifdef IPCOM_USE_INET6
    struct Ip_sockaddr_in6 sa6;
#endif
    char buf[IPTFTP_MAX_PACKET];
    Ip_s32 i, len;
    char *err_msg = IP_NULL;
    struct Ip_msghdr msghdr;
    struct Ip_iovec iov;
    Ip_err ret = IPCOM_SUCCESS;

    /* Get the message from the socket */
    tftp->controllen = sizeof(tftp->controlbuf);
    msghdr.msg_controllen = tftp->controllen;
    msghdr.msg_control = tftp->controlbuf;
    msghdr.msg_flags = 0;
    msghdr.msg_iov = &iov;
    msghdr.msg_iovlen = 1;
    iov.iov_base = buf;
    iov.iov_len = sizeof(buf);

    /* Get this connection's first packet */
    switch (family)
    {
#ifdef IPCOM_USE_INET
    case IP_AF_INET:
        msghdr.msg_name = &sa;
        msghdr.msg_namelen = sizeof(sa);
        len = ipcom_recvmsg(sock, &msghdr, 0);
        if (len < 0)
        {
            ret = IPCOM_ERR_FAILED;
            goto exit;
        }
        tftp->peer_port = ip_ntohs(sa.sin_port);
        ipcom_memcpy(&tftp->peer_addr.addr, &sa.sin_addr, sizeof(tftp->peer_addr.addr));
        tftp->family = IP_AF_INET;
        break;
#endif /* IPCOM_USE_INET */

#ifdef IPCOM_USE_INET6
    case IP_AF_INET6:
        msghdr.msg_name = &sa6;
        msghdr.msg_namelen = sizeof(sa6);
        len = ipcom_recvmsg(sock, &msghdr, 0);
        if (len < 0)
        {
            ret = IPCOM_ERR_FAILED;
            goto exit;
        }
        tftp->peer_port = ip_ntohs(sa6.sin6_port);
        ipcom_memcpy(&tftp->peer_addr.addr6, &sa6.sin6_addr, sizeof(tftp->peer_addr.addr6));
        tftp->ifindex = sa6.sin6_scope_id;
        tftp->family = IP_AF_INET6;
        break;
#endif /* IPCOM_USE_INET6 */

    default:
        ret = IPCOM_ERR_FAILED;
        goto exit;
    }

    /* Sanity checking */
    if (len < 6)
    {
        /* Not enough characters */
        err_msg = IPTFTP_ERRMSG_INCOMPLETEMSG;
        goto exit;
    }
    if (buf[len-1] != '\0')
    {
        /* No null char at end */
        err_msg = IPTFTP_ERRMSG_INCOMPLETEMSG;
        goto exit;
    }
    for (i = 2; i < len; i++)
        if (buf[i] == '\0')
            break;
    if (i < 3)
    {
        /* First char is null char */
        err_msg = IPTFTP_ERRMSG_INCOMPLETEMSG;
        goto exit;
    }
    if (i > (len-3))
    {
        /* Only one null char or two trailing null chars */
        err_msg = IPTFTP_ERRMSG_INCOMPLETEMSG;
        goto exit;
    }

    /* Check opcode */
    tftp->opcode = (Ip_u16)(buf[0]*256 + buf[1]);
    if (tftp->opcode != IPTFTP_OPCODE_RRQ && tftp->opcode != IPTFTP_OPCODE_WRQ)
    {
        err_msg = IPTFTP_ERRMSG_INVALIDOPCODE;
        goto exit;
    }

    /* Check mode */
    if (!ipcom_strcasecmp(&buf[i+1], "netascii"))
        tftp->binary = IP_FALSE;
    else if (!ipcom_strcasecmp(&buf[i+1], "octet"))
        tftp->binary = IP_TRUE;
    else
    {
        err_msg = IPTFTP_ERRMSG_INVALIDMODE;
        goto exit;
    }

    /* Strip path from incoming requests */
    iptftp_strip_path(&buf[2]);
    /* Append filename to server directory */
    if (tftp->dirlen + 1 + ipcom_strlen(&buf[2]) >= sizeof(tftp->file))
    {
        ret = IPCOM_ERR_REACHED_MAX;
        goto exit;
    }
    else
    {
        tftp->file[tftp->dirlen] = '/';
        ipcom_strcpy(&tftp->file[tftp->dirlen+1], &buf[2]);
    }

exit:
    if (err_msg != IP_NULL)
        iptftp_send_err(sock, tftp, IPTFTP_ERRCODE_ILLEGALTFTPOP, err_msg);
    return ret;
}


/*
 *===========================================================================
 *                    iptftp_server_send_file
 *===========================================================================
 * Description: TFTP server internal send function.
 * Parameters:  tftp - TFTP structure.
 * Returns:     nothing.
 */
IP_STATIC void
iptftp_server_send_file(Iptftp *tftp)
{
    Ip_fd sock;
    IP_FILE *fp = IP_NULL;
    char *err_msg = IP_NULL;
    Ip_u16 err_code = 0;

    /* Get a udp socket, and bind to port 0 */
    sock = ipcom_socket(tftp->family, IP_SOCK_DGRAM, 0);
    if (sock == IP_INVALID_SOCKET)
    {
        IPCOM_LOG0(ERR, "iptftp_server_send_file :: ipcom_socket() failed");
        goto err;
    }
    if (iptftp_bind_zero(sock, tftp->family) != IPCOM_SUCCESS)
        goto err;

#ifdef IPTFTP_SERVER_ALLOW_DOWNLOAD
    /* Open the file */
    if (tftp->binary)
        fp = ipcom_fopen(tftp->file, "rb");
    else
        fp = ipcom_fopen(tftp->file, "r");
#endif
    if (!fp)
    {
        IPCOM_LOG1(ERR, "iptftp_server_send_file :: Could not open file: \"%s\"", tftp->file);
        err_code = IPTFTP_ERRCODE_FILENOTFOUND;
        err_msg = IPTFTP_ERRMSG_COULDNOTOPENFILE;
        goto err;
    }

    /* Start the send loop */
    (void)iptftp_send_loop(sock, fp, &err_msg, &err_code, tftp);

err:
    /* Send error */
    if (err_msg)
        iptftp_send_err(sock, tftp, err_code, err_msg);

    /* Terminate connection */
    if (fp != IP_NULL)
        (void)ipcom_fclose(fp);
    if (sock != IP_INVALID_SOCKET)
        ipcom_socketclose(sock);
}


/*
 *===========================================================================
 *                    iptftp_server_receive_file
 *===========================================================================
 * Description: TFTP server internal receive function.
 * Parameters:  tftp - TFTP structure.
 * Returns:     nothing.
 */
IP_STATIC void
iptftp_server_receive_file(Iptftp *tftp)
{
    Ip_fd sock;
    IP_FILE *fp = IP_NULL;
    Ip_u16 err_code = 0;
    char *err_msg = IP_NULL;

    /* Get a udp socket, and bind to port 0 */
    sock = ipcom_socket(tftp->family, IP_SOCK_DGRAM, 0);
    if (sock == IP_INVALID_SOCKET)
    {
        IPCOM_LOG0(ERR, "iptftp_server_receive_file :: ipcom_socket() failed");
        goto err;
    }
    if (iptftp_bind_zero(sock, tftp->family) != IPCOM_SUCCESS)
        goto err;

#ifdef IPTFTP_SERVER_ALLOW_UPLOAD
    /* Create the file */
    if (tftp->binary)
        fp = ipcom_fopen(tftp->file, "wb");
    else
        fp = ipcom_fopen(tftp->file, "w");
#endif
    if (!fp)
    {
        IPCOM_LOG1(ERR, "iptftp_server_receive_file :: Could not create file: \"%s\"", tftp->file);
        err_code = IPTFTP_ERRCODE_ACCESSVIOLATION;
        err_msg = IPTFTP_ERRMSG_COULDNOTCREATEFILE;
        goto err;
    }

    /* Start the receive loop */
    (void)iptftp_receive_loop(sock, fp, &err_msg, &err_code, tftp);

err:
    /* Send error */
    if (err_msg)
        iptftp_send_err(sock, tftp, err_code, err_msg);

    /* Terminate connection */
    if (fp != IP_NULL)
        (void)ipcom_fclose(fp);
    if (sock != IP_INVALID_SOCKET)
        ipcom_socketclose(sock);
}


/*
 *===========================================================================
 *                    iptftp_server_init
 *===========================================================================
 * Description: Initialize TFTP server.
 * Parameters:  tftp - TFTP structure.
 * Returns:     IPCOM_SUCCESS or an IPCOM_ERR_XXX error code for failures.
 */
IP_STATIC Ip_err
iptftp_server_init(Iptftp *tftp)
{
    Ip_err ret = IPCOM_SUCCESS;
    char *dir;
    struct Ip_stat stat_buf;

    /* Create the default directory  */
    dir = ipcom_sysvar_get("iptftp.dir", IP_NULL, IP_NULL);
    if (dir == IP_NULL)
    {
        ipcom_strcpy(tftp->file, IPCOM_FILE_ROOT "tftpdir");
    }
    else if (ipcom_strlen(dir) >= sizeof(tftp->file))
    {
        IPCOM_LOG1(ERR, "TFTP server directory \"%s\" is too long!", dir);
        ret = IPCOM_ERR_FAILED;
        goto exit;
    }
    else
    {
        ipcom_strcpy(tftp->file, dir);
    }
    tftp->dirlen = ipcom_strlen(tftp->file);

    /* Create the default TFTP directory if it does not exist. */
    if (ipcom_stat(tftp->file, &stat_buf) != 0)
    {
        if (ipcom_mkdir(tftp->file, 0777) == 0)
        {
            IPCOM_LOG1(INFO, "created TFTP server directory \"%s\"",
                       tftp->file);
        }
        else
        {
            IPCOM_LOG2(ERR, "iptftp_server_init :: ipcom_mkdir(\"%s\") - %s",
                       tftp->file, ipcom_strerror(ipcom_errno));
            ret = IPCOM_ERR_FAILED;
        }
    }

exit:
    if (dir != IP_NULL)
        ipcom_free(dir);
    return ret;
}


/*
 *===========================================================================
 *                    iptftps
 *===========================================================================
 * Description: TFTP server damon. Listens for connections and spawns
 *              child processes for each connection.
 * Parameters:  none.
 * Returns:     nothing.
 */
IP_STATIC
IPCOM_PROCESS(iptftps)
{
#ifdef IPCOM_USE_INET
    struct Ip_sockaddr_in sa;
    Ip_fd sock = IP_INVALID_SOCKET;
#endif
#ifdef IPCOM_USE_INET6
    struct Ip_sockaddr_in6 sa6;
    Ip_fd sock6 = IP_INVALID_SOCKET;
#endif
    Ip_fd max_sock, ipd_sock = IP_INVALID_SOCKET;
    Ip_fd_set *read_set = IP_NULL;
    Iptftp *tftp;
    Ip_err err;
    int on = 1;

    ipcom_proc_init();
    tftp = ipcom_malloc(sizeof(*tftp));
    if (tftp == IP_NULL)
    {
        err = IPCOM_ERR_NO_MEMORY;
        IPCOM_LOG0(ERR, "iptftps :: ipcom_malloc(tftp) failed");
        goto fail;
    }
    ipcom_memset(tftp, 0, sizeof(*tftp));

    /* Install IPC. */
    err = ipcom_ipc_install();
    if (err != IPCOM_SUCCESS)
    {
        IPCOM_LOG0(ERR, "iptftps :: ipcom_ipc_install() failed");
        goto fail;
    }

    /* Initialize tftp server */
    err = iptftp_server_init(tftp);
    if (err != IPCOM_SUCCESS)
    {
        IPCOM_LOG0(ERR, "iptftps :: iptftp_server_init() failed");
        goto fail;
    }

    /* Get read set. */
    read_set = ipcom_malloc(sizeof(Ip_fd_set));
    if (read_set == IP_NULL)
    {
        err = IPCOM_ERR_NO_MEMORY;
        IPCOM_LOG0(ERR, "iptftps :: ipcom_malloc(read_set) failed");
        goto fail;
    }

    err = IPCOM_ERR_SOCKCALLFAILED;
#ifdef IPCOM_USE_INET
    /* Get a udp socket, bind to the tftp port */
    sock = ipcom_socket(IP_AF_INET, IP_SOCK_DGRAM, 0);
    if (sock == IP_INVALID_SOCKET)
    {
        IPCOM_LOG0(ERR, "iptftps :: ipcom_socket() failed");
#ifndef IPCOM_USE_INET6
        goto fail;
#endif
    }
    else
    {
        /* Enable ancillary data */
        if (ipcom_setsockopt(sock, IP_IPPROTO_IP, IP_IP_PKTINFO, &on, sizeof(on)) < 0)
        {
#ifndef IPLITE
            IPCOM_LOG2(INFO, "iptftps :: ipcom_setsockopt(IP_IP_PKTINFO) failed, errno = %d (%s)",
                       ipcom_errno, ipcom_strerror(ipcom_errno));
#endif
        }

        ipcom_memset(&sa, 0, sizeof(sa));
        IPCOM_SA_LEN_SET(&sa, sizeof(struct Ip_sockaddr_in));
        sa.sin_family = IP_AF_INET;
        sa.sin_port = ip_htons(IPTFTP_PORT);
        sa.sin_addr.s_addr = IP_INADDR_ANY;
        if (ipcom_bind(sock, (struct Ip_sockaddr *)&sa, sizeof(sa)) == IP_SOCKERR)
        {
            IPCOM_LOG1(ERR, "iptftps :: ipcom_bind() failed, errno = %d", ipcom_errno);
#ifdef IPCOM_USE_INET6
            ipcom_socketclose(sock);
            sock = IP_INVALID_SOCKET;
#else
            goto fail;
#endif
        }
    }
#endif /* IPCOM_USE_INET */

#ifdef IPCOM_USE_INET6
    /* Get a udp v6 socket, bind to the tftp port */
    sock6 = ipcom_socket(IP_AF_INET6, IP_SOCK_DGRAM, 0);
    if (sock6 == IP_INVALID_SOCKET)
    {
        IPCOM_LOG0(ERR, "iptftps :: ipcom_socket(INET6) failed");
#ifndef IPCOM_USE_INET
        goto fail;
#endif
    }
    else
    {
        /* Enable ancillary data */
        if (ipcom_setsockopt(sock6, IP_IPPROTO_IPV6, IP_IPV6_RECVPKTINFO, &on, sizeof(on)) < 0)
        {
#ifndef IPLITE
            IPCOM_LOG2(INFO, "iptftps :: ipcom_setsockopt(IP_IPV6_RECVPKTINFO) failed, errno = %d (%s)",
                       ipcom_errno, ipcom_strerror(ipcom_errno));
#endif
        }

        ipcom_memset(&sa6, 0, sizeof(sa6));
        IPCOM_SA_LEN_SET(&sa6, sizeof(sa6));
        sa6.sin6_family = IP_AF_INET6;
        sa6.sin6_port = ip_htons(IPTFTP_PORT);
        if (ipcom_bind(sock6, (struct Ip_sockaddr *)&sa6, sizeof(sa6)) == IP_SOCKERR)
        {
            IPCOM_LOG0(ERR, "iptftps :: ipcom_bind(INET6) failed");
#ifdef IPCOM_USE_INET
            ipcom_socketclose(sock6);
            sock6 = IP_INVALID_SOCKET;
#else
            goto fail;
#endif
        }
    }
#endif /* IPCOM_USE_INET6 */

    /* IPD init. */
    if (ipcom_ipd_init("iptftps", IPCOM_SUCCESS, &ipd_sock) != IPCOM_SUCCESS)
    {
        IPCOM_LOG0 (ERR, "iptftps :: ipcom_ipd_init() failed");
		goto fail;
    }

    max_sock = ipd_sock;
#ifdef IPCOM_USE_INET
    max_sock = IP_MAX(max_sock, sock);
#endif
#ifdef IPCOM_USE_INET6
    max_sock = IP_MAX(max_sock, sock6);
#endif
    max_sock++;

    /* Start the iptftps select loop */
    IPCOM_LOG0(DEBUG, "iptftps :: listening for connections");
    for (;;)
    {
        Ip_s32 num;

        IP_FD_ZERO(read_set);
        IP_FD_SET(ipd_sock, read_set);
#ifdef IPCOM_USE_INET
        if (sock != IP_INVALID_SOCKET)
            IP_FD_SET(sock, read_set);
#endif
#ifdef IPCOM_USE_INET6
        if (sock6 != IP_INVALID_SOCKET)
            IP_FD_SET(sock6, read_set);
#endif

        /* Wait for a connection */
        num = ipcom_socketselect(max_sock, read_set, IP_NULL, IP_NULL, IP_NULL);
        if (num == IP_SOCKERR)
        {
            IPCOM_LOG1(ERR, "iptftps :: ipcom_socketselect() failed, errno = %d", ipcom_errno);
            goto fail;
        }

#ifdef IPCOM_USE_INET
        /* Check for connection on ipv4 socket*/
        if (num > 0 && sock != IP_INVALID_SOCKET && IP_FD_ISSET(sock, read_set))
        {
            num--;
            if (iptftp_server_parse_request(sock, IP_AF_INET, tftp) == IPCOM_SUCCESS)
            {
                if (iptftp_server_spawn_handler(tftp) != IPCOM_SUCCESS)
                    IPCOM_LOG0(ERR, "iptftps :: iptftp_server_spawn_handler failed");
            }
            else
                IPCOM_LOG0(ERR, "iptftps :: iptftp_server_parse_request(INET4) failed");
        }
#endif /* IPCOM_USE_INET */

#ifdef IPCOM_USE_INET6
        /* Check for connection on ipv6 socket*/
        if (num > 0 && sock6 != IP_INVALID_SOCKET && IP_FD_ISSET(sock6, read_set))
        {
            num--;
            if (iptftp_server_parse_request(sock6, IP_AF_INET6, tftp) == IPCOM_SUCCESS)
            {
                if (iptftp_server_spawn_handler(tftp) != IPCOM_SUCCESS)
                    IPCOM_LOG0(ERR, "iptftps :: iptftp_server_spawn_handler() failed");
            }
            else
                IPCOM_LOG0(ERR, "iptftps :: iptftp_server_parse_request(INET6) failed");
        }
#endif /* IPCOM_USE_INET6 */

        /* Check for connection on ipd socket*/
        if (num > 0 && IP_FD_ISSET(ipd_sock, read_set))
        {
            int event;

            event = ipcom_ipd_input(ipd_sock);
            switch(event)
            {
                case IPCOM_IPD_EVENT_RECONFIGURE:
                    IPCOM_LOG0(INFO, "received IPD reconfigure message");
                    /* Initialize tftp server */
                    if (iptftp_server_init(tftp) != IPCOM_SUCCESS)
                    {
                        IPCOM_LOG0(ERR, "iptftps :: iptftp_server_init() failed");
                        goto fail;
                    }
                    break;
                case IPCOM_IPD_EVENT_KILL:
                    IPCOM_LOG0(INFO, "received IPD kill message");
                    goto fail;
                default:
                    break;
            }
        }
    }

fail:
    if (tftp != IP_NULL)
        ipcom_free(tftp);
    if (read_set != IP_NULL)
        ipcom_free(read_set);
#ifdef IPCOM_USE_INET
    if (sock != IP_INVALID_SOCKET)
        ipcom_socketclose(sock);
#endif
#ifdef IPCOM_USE_INET6
    if (sock6 != IP_INVALID_SOCKET)
        ipcom_socketclose(sock6);
#endif
    if (ipd_sock == IP_INVALID_SOCKET)
    {
        ip_assert(err != IPCOM_SUCCESS);
        (void)ipcom_ipd_init("iptftps", err, &ipd_sock);
    }
    if (ipd_sock != IP_INVALID_SOCKET)
        (void)ipcom_ipd_exit("iptftps", ipd_sock);
    (void)ipcom_ipc_uninstall();

    ipcom_proc_exit();
}


/*
 ****************************************************************************
 * 10                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    iptftps_create
 *===========================================================================
 * Description: TFTP server create function.
 * Parameters:  none.
 * Returns:     IPCOM_SUCCESS or an IPCOM_ERR_XXX error code.
 */
IP_PUBLIC Ip_err
iptftps_create(void)
{
    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    iptftps_start
 *===========================================================================
 * Description: TFTP server start function.
 * Parameters:  none.
 * Returns:     IPCOM_SUCCESS or an IPCOM_ERR_XXX error code.
 */
IP_PUBLIC Ip_err
iptftps_start(void)
{
    return ipcom_proc_create("iptftps", (Ipcom_proc_func)iptftps, IPCOM_PROC_STACK_DEFAULT, IP_NULL);
}


/*
 *===========================================================================
 *                    iptftps_version
 *===========================================================================
 * Description: Get TFTP server version string.
 * Parameters:  none.
 * Returns:     TFTP server version.
 */
IP_PUBLIC const char *
iptftps_version(void)
{
    return "@(#) IPTFTPS $Name: VXWORKS_ITER18A_FRZ10 $ - INTERPEAK_COPYRIGHT_STRING";
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
