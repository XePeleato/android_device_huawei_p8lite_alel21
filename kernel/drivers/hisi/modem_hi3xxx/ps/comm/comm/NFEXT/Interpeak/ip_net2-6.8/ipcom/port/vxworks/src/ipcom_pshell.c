/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_pshell.c,v $ $Revision: 1.31.16.1 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/port/vxworks/src/ipcom_pshell.c,v $
 *   $Author: ulf $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Roger Boden <roger@interpeak.se>
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

#define IPCOM_SKIP_NATIVE_SOCK_API

#include "ipcom_config.h"


/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#if IPCOM_USE_SHELL == IPCOM_SHELL_NATIVE

#include <ipcom_type.h>
#include <ipcom_cstyle.h>
#include <ipcom_clib.h>
#include <ipcom_sock.h>
#include <ipcom_sock2.h>
#include <ipcom_err.h>
#include <ipcom_syslog.h>
#include <ipcom_shell.h>
#include <ipcom_list.h>
#include <ipcom_os.h>
#ifdef IPCOM_USE_INET6
#include <ipcom_sock6.h>
#endif

#undef s6_addr /* Remove the ipcom definition, vxWorks 6.0 has a conflicting macro definition */
#undef ifa_dstaddr /* The ifa_dstaddr #define in ipcom_sock2.h breaks code in target/h/.../ifvar.h */
#undef ifa_broadaddr /* The ifa_broadaddr #define in ipcom_sock2.h breaks code in target/h/.../ifvar.h */

#include <vxWorks.h>
#include <stdio.h>
#include <string.h>
#include <shellLib.h>
#include <sockLib.h>
#include <taskLib.h>
#include <wrn/coreip/telnetLib.h>
#include <ptyDrv.h>
#include <stdio.h>
#include <netinet/in.h>
#include <netinet/tcp.h>


/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

#define IPCOM_SYSLOG_PRIORITY    IPCOM_SYSLOG_IPCOM_PRIORITY
#define IPCOM_SYSLOG_FACILITY    IPCOM_LOG_IPCOM

#ifndef IPCOM_PSHELL_PTY_DEV_NAME_MAX_LEN
#define IPCOM_PSHELL_PTY_DEV_NAME_MAX_LEN 64
#endif

#ifndef IPCOM_PSHELL_PTY_BUFFER_SIZE
#define IPCOM_PSHELL_PTY_BUFFER_SIZE 512
#endif

#define IPCOM_PSHELL_TASK_NAME_LEN             32
#define IPCOM_PSHELL_LOGIN_TASK_BASENAME       "tLogin"
#define IPCOM_PSHELL_STDIO_PROXY_TASK_BASENAME "tStdioProxy"


/*
 * Ideally, we would not define IPCOM_SKIP_NATIVE_SOCK_API above, then we
 * would handle the both the case of IPCOM_USE_NATIVE_SOCK_API being defined
 * and not defined. Unfortunately, when IPCOM_USE_NATIVE_SOCK_API is defined,
 * we get compile errors (inconsistent function declarations for the socket
 * API. As a work-around, we therefore define IPCOM_SKIP_NATIVE_SOCK_API,
 * include all headers and thereafter define macros that include type casts
 * where necessary (below).
 */
#ifdef IPCOM_USE_NATIVE_SOCK_API
#define ipcom_socket           socket
#define ipcom_socketclose(fd)  close((int)fd)
#define ipcom_shutdown(fd,how) shutdown((int)fd, how)
#define ipcom_bind(s,a,l)      bind((int)s,(struct sockaddr*)a,(int)l)
#define ipcom_connect          connect
#define ipcom_accept           accept
#define ipcom_listen           listen
#define ipcom_socketwrite      write
#define ipcom_socketwritev     writev
#define ipcom_send             send
#define ipcom_sendto           sendto
#define ipcom_sendmsg          sendmsg
#define ipcom_socketread       read
#define ipcom_recv             recv
#define ipcom_recvfrom         recvfrom
#define ipcom_recvmsg          recvmsg
#define ipcom_getsockopt       getsockopt
#define ipcom_setsockopt(s,l,n,v,d) setsockopt(s,l,n,(char *)v,d)
#define ipcom_getsockname(s,a,l) getsockname((int)s,(struct sockaddr*)a,(int*)l)
#define ipcom_getpeername      getpeername
#define ipcom_socketioctl      ioctl
#define ipcom_if_nametoindex   if_nametoindex
#define ipcom_if_indextoname   if_indextoname
#define ipcom_if_nameindex     if_nameindex
#define ipcom_if_freenameindex if_freenameindex
#define ipcom_socketselect     select
#endif

/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

typedef struct Ipcom_vxshell_session_s
{
    Ipcom_sem   shell_exit_sem;
}
Ipcom_vxshell_session_t;

/*
 ****************************************************************************
 * 6                    EXTERN PROTOTYPES
 ****************************************************************************
 */

extern char tyBackspaceChar;

/*
 ****************************************************************************
 * 7                    LOCAL PROTOTYPES
 ****************************************************************************
 */

IP_STATIC void ipcom_vxshell_login_task(int pty_fd, int vx_sock);

IP_STATIC void ipcom_vxworks_shell_logout_cb(void *arg);

IP_STATIC Ip_err ipcom_pshell_create_pty(char* pty_dev_name, int name_len, int *master_fd, int *slave_fd);

IP_STATIC Ip_err ipcom_pshell_spawn_proxy(int vx_sock);

IP_STATIC void ipcom_pshell_stdio_proxy(int vx_sock, int parent_task_id);

IP_STATIC int ipcom_pshell_proxy_data(int read_fd, int write_fd, Ip_bool can_change_direction);

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
 *                    ipcom_pshell_spawn_proxy
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err
ipcom_pshell_spawn_proxy(int vx_sock)
{
    char task_name[IPCOM_PSHELL_TASK_NAME_LEN];

    snprintf(task_name,
             sizeof(task_name),
             IPCOM_PSHELL_STDIO_PROXY_TASK_BASENAME "%x",
             taskIdSelf());

    if (taskSpawn(task_name, 50, 0, 6000, (FUNCPTR)ipcom_pshell_stdio_proxy,
                  vx_sock, taskIdSelf(), 2, 3, 4, 5, 6, 7, 8, 9) == ERROR)
    {
        IPCOM_LOG2(ERR, "Failed to create stdio proxy task: %s(%d)",
                   ipcom_strerror(ipcom_errno), ipcom_errno);
        return IPCOM_ERR_FAILED;
    }

    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    ipcom_pshell_stdio_proxy
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipcom_pshell_stdio_proxy(int vx_sock, int parent_task_id)
{
    int      master_fd;
    int      slave_fd;
    char     task_name[IPCOM_PSHELL_TASK_NAME_LEN];
    char     pty_dev_name[IPCOM_PSHELL_PTY_DEV_NAME_MAX_LEN];
    Ip_pid_t pid;

    /* Make this process the owner of the "vx_sock" socket */
    pid = ipcom_getpid();
    (void) ipcom_socketioctl(vx_sock, IP_SIOCSPGRP, &pid);

    if (ipcom_pshell_create_pty(pty_dev_name, sizeof(pty_dev_name), &master_fd, &slave_fd) != IPCOM_SUCCESS)
    {
        IPCOM_LOG0(ERR, "Failed to create ptys");
        close(vx_sock);
        return;
    }

    snprintf(task_name,
             sizeof(task_name),
             IPCOM_PSHELL_LOGIN_TASK_BASENAME "%x",
             parent_task_id);
    if (taskSpawn (task_name, 50, 0, 6000, (FUNCPTR)ipcom_vxshell_login_task,
                   slave_fd, vx_sock, 0, 0, 0, 0, 0, 0, 0, 0) == ERROR)
    {
        IPCOM_LOG1(ERR,
                   "ipcom_shell_login_task: Failed to create login task: %s",
                   strerror(errno));
        close(slave_fd);
        goto cleanup;
    }
    /* else: the ownership of "slave_fd" has been transferred to the
       ipcom_vxshell_login_task at this point */

    /* Main loop, proxy data between pty fd and socket to telnet daemon */
    for (;;)
    {
        fd_set  read_set;
        int     read_fd = -1;
        int     write_fd = -1;
        Ip_bool can_change_direction = IP_FALSE;
        int     ret;

        FD_ZERO(&read_set);
        FD_SET(master_fd, &read_set);
        FD_SET(vx_sock, &read_set);

        if ((ret = select(IP_MAX(master_fd, vx_sock)+1, &read_set, NULL, NULL, NULL)) < 0)
        {
            IPCOM_LOG3(ERR,
                       "ipcom_shell_login_task: select([%d, %d]) failed: %s",
                       master_fd,
                       vx_sock,
                       strerror(errno));
            goto cleanup;
        }

        if (FD_ISSET(master_fd, &read_set))
        {
            /* Move data from the shell client to the telnet client */
            read_fd = master_fd;
            write_fd = vx_sock;
        }
        else if (FD_ISSET(vx_sock, &read_set))
        {
            /* Move data from the telnet client to the shell */
            read_fd = vx_sock;
            write_fd = master_fd;
            /* Allow ipcom_pshell_proxy_data() to start move data from
               the shell to the telnet client if the shell cannot
               accept more data. This is done to avoid a dead lock
               where this task waits for the shell file descriptor to
               become writable, but the only thing that can make it
               writable is if someone dequeus data written by the
               shell. */
            can_change_direction = IP_TRUE;
        }
        else
            continue;

        ret = ipcom_pshell_proxy_data(read_fd, write_fd, can_change_direction);
        if (ret == 0)
            goto cleanup; /* Either shell or client initiated close */

        if (ret < 0)
        {
            if (ret < 0 && ipcom_errno != ESHUTDOWN)
            {
				IPCOM_LOG2(ERR, "Failed to proxy data: %s(%d)",
						   ipcom_strerror(ipcom_errno), ipcom_errno);
            }
            break;
        }
    }

 cleanup:
    close(master_fd);
    ptyDevRemove(pty_dev_name);
    close(vx_sock);
}


/*
 *===========================================================================
 *                    ipcom_pshell_proxy_data
 *===========================================================================
 * Description: Moves data from the read_fd to the write_fd.
 * Parameters:  read_fd - file descriptor data is moved from.
 *              write_fd - file descriptor data is move to.
 *              can_change_direction - IP_TRUE if data can be moved in the
 *              opposite direction if the write_fd is full.
 * Returns:     >0 = number of bytes moved.
 *               0 = end of file reached.
 *              <0 = error code.
 *
 */
IP_STATIC int
ipcom_pshell_proxy_data(int read_fd, int write_fd, Ip_bool can_change_direction)
{
    char  buf[200];
    char *ptr;
    int   rlen;
    int   wlen;

    rlen = read(read_fd, buf, sizeof(buf));
    if (rlen <= 0)
        return rlen;

    ptr = buf;
    wlen = rlen;
    while (wlen > 0)
    {
        int bytes_written;

        bytes_written = write(write_fd, ptr, wlen == rlen ? wlen : 1);
        if (bytes_written < 0)
            return bytes_written;

        if (bytes_written == 0)
        {
            fd_set wr_set;

            /* When the pty device is in line mode (typically when running a command
             * with a sub prompt), typing backspace when there are no characters to
             * erase will make write return 0 */
            if (ptr[0] == tyBackspaceChar)
                return rlen;

            if (can_change_direction)
                /* Try to move data in the oposite direction to free
                   up space in the input buffer of the shell file
                   descriptor. */
                (void)ipcom_pshell_proxy_data(write_fd, read_fd, IP_FALSE);

            /* Send buffer full, wait until more space becomes
               available */
            FD_ZERO(&wr_set);
            FD_SET(write_fd, &wr_set);
            if (select(write_fd + 1, NULL, &wr_set, NULL, NULL) < 0)
                return -1;
        }
        wlen -= bytes_written;
        ptr += bytes_written;
    }

    return rlen;
}


/*
 *===========================================================================
 *                    ipcom_pshell_create_pty
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err
ipcom_pshell_create_pty(char* pty_dev_name, int name_len, int *master_fd, int *slave_fd)
{
    char pty_dev_name_m[IPCOM_PSHELL_PTY_DEV_NAME_MAX_LEN];
    char pty_dev_name_s[IPCOM_PSHELL_PTY_DEV_NAME_MAX_LEN];

    *master_fd = ERROR;
    *slave_fd = ERROR;

    /* Create unique names for the pty device */
    snprintf (pty_dev_name, name_len, "%s_%x.", "stdio_pty", (int)taskIdSelf());
    snprintf (pty_dev_name_m, IPCOM_PSHELL_PTY_DEV_NAME_MAX_LEN, "%sM", pty_dev_name);
    snprintf (pty_dev_name_s, IPCOM_PSHELL_PTY_DEV_NAME_MAX_LEN, "%sS", pty_dev_name);

    /* pseudo tty device creation */
    if (ptyDevCreate (pty_dev_name,
                      IPCOM_PSHELL_PTY_BUFFER_SIZE,
                      IPCOM_PSHELL_PTY_BUFFER_SIZE) == ERROR)
    {
        IPCOM_LOG2(ERR, "ptyDevCreate failed: %s(%d)",
                   ipcom_strerror(ipcom_errno), ipcom_errno);
        return IPCOM_ERR_FAILED;
    }

    /* Master-side open of pseudo tty */
    *master_fd = open (pty_dev_name_m, O_RDWR, 0);
    if (*master_fd == ERROR)
    {
        IPCOM_LOG2(ERR, "open failed on pty device: %s(%d)",
                   ipcom_strerror(ipcom_errno), ipcom_errno);
        goto err;
    }

    /* Slave-side open of pseudo tty */
    *slave_fd = open (pty_dev_name_s, O_RDWR, 0);
    if (*slave_fd == ERROR)
    {
        IPCOM_LOG2(ERR, "open failed on pty device: %s(%d)",
                   ipcom_strerror(ipcom_errno), ipcom_errno);
        goto err;
    }
    /*
     * Set up the slave device to act like a terminal, but no ^C/^X.
     * OPT_ABORT and OPT_MON_TRAP will be enabled by the shell parser
     * routine after successful login but before restarting the shell.
     */

    if (ioctl(*slave_fd, FIOOPTIONS, OPT_TERMINAL & ~(OPT_ABORT|OPT_MON_TRAP)) != OK)
    {
        IPCOM_LOG2(ERR, "ioctl(FIOOPTIONS) failed: %s(%d)",
                   ipcom_strerror(ipcom_errno), ipcom_errno);
        goto err;
    }

    return IPCOM_SUCCESS;

 err:
    ptyDevRemove(pty_dev_name);
    if (*master_fd != ERROR)
        close(*master_fd);
    if (*slave_fd != ERROR)
        close(*slave_fd);

    return IPCOM_ERR_FAILED;
}


/*
 *===========================================================================
 *                    ipcom_vxworks_shell_logout_cb
 *===========================================================================
 * Description: The vxshell calls this upon exit. This is used to trigger the
 *              'ipcom_login_task_task' to tear down the remote connection
 *
 * Parameters:  arg: the session descriptor created by the 'ipcom_vxshell_login_task'
 *
 */
IP_STATIC void
ipcom_vxworks_shell_logout_cb(void *arg)
{
    Ipcom_vxshell_session_t *session = (Ipcom_vxshell_session_t *) arg;

    ipcom_sem_post(session->shell_exit_sem);
}


/*
 ****************************************************************************
 * 10                   GLOBAL FUNCTIONS
 ****************************************************************************
 */
/*
 *===========================================================================
 *                    ipcom_vxshell_login_task
 *===========================================================================
 * Description: Task to provide a context login procedure in 'shellParserControl'
 *
 * Parameters:  pty_fd: the vxworks socket the vxshell should communicate on
 * Parameters:  vx_sock: the vxworks socket the stdio_proxy is using for
 *              communication with the telnet server.
 *
 */
IP_STATIC void
ipcom_vxshell_login_task(int pty_fd, int vx_sock)
{
    Ipcom_vxshell_session_t session;

    ipcom_sem_create(&session.shell_exit_sem, 0);

    if (shellParserControl(REMOTE_START, (Ip_u32)&session, pty_fd, ipcom_vxworks_shell_logout_cb) == 0)
    {
        ipcom_sem_wait(session.shell_exit_sem);
        shellParserControl(REMOTE_STOP, (Ip_u32)&session, 0, IP_NULL);
    }

    ipcom_sem_delete(&session.shell_exit_sem);

    close(pty_fd);
    shutdown(vx_sock, SHUT_RD);
}

/*
 ****************************************************************************
 * 10                   GLOBAL FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipcom_start_extshell
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL Ip_err
ipcom_start_extshell(Ip_fd *stdio_sock, Ip_fd cli_sock)
{
    Ip_fd                      ip_sock = IP_INVALID_SOCKET;
    Ip_fd                      ip_listen_sock = IP_INVALID_SOCKET;
    int                        vx_sock = IP_INVALID_SOCKET;
    Ip_err                     rc = IPCOM_ERR_FAILED;
    Ip_socklen_t               len;
    int                        enable = IP_TRUE;
    struct linger              linger;
    struct Ip_addrinfo         hints;
    struct Ip_addrinfo        *res;
    struct Ip_sockaddr_storage ss;

    ipcom_memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = IP_SOCK_STREAM;
    if (ipcom_getaddrinfo(IP_NULL, "0", &hints, &res) != 0)
        return rc;

    ip_listen_sock = ipcom_socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (ip_listen_sock == IP_INVALID_SOCKET)
    {
        IPCOM_LOG1(ERR, "ipcom_start_extshell :: ipcom_socket(AF_INET) failed: %s", ipcom_strerror(ipcom_errno));
        goto err_out;
    }

    if (ipcom_bind(ip_listen_sock, res->ai_addr, res->ai_addrlen) != OK)
    {
        IPCOM_LOG1(ERR, "ipcom_start_extshell :: ipcom_bind() failed: %s", ipcom_strerror(ipcom_errno));
        goto err_out;
    }
    if (ipcom_listen(ip_listen_sock, 1) != OK)
    {
        IPCOM_LOG1(ERR, "ipcom_start_extshell :: ipcom_listen() failed: %s", ipcom_strerror(ipcom_errno));
        goto err_out;
    }
    /* Fetch the empheral port assigned to our socket */
    len = sizeof(ss);
    if (ipcom_getsockname(ip_listen_sock, (struct Ip_sockaddr*) &ss, &len) != IPCOM_SUCCESS)
    {
        IPCOM_LOG1(ERR, "ipcom_start_extshell :: ipcom_getsockname(AF_INET) failed: %s", ipcom_strerror(ipcom_errno));
        goto err_out;
    }

    vx_sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (vx_sock == -1)
    {
        IPCOM_LOG1(ERR, "ipcom_start_extshell :: ipcom_socket() failed: %s", ipcom_strerror(ipcom_errno));
        goto err_out;
    }

    if (connect(vx_sock, (struct sockaddr *) &ss, sizeof(ss)) != OK)
    {
        IPCOM_LOG1(ERR, "ipcom_start_extshell :: connect() failed: %s", ipcom_strerror(ipcom_errno));
        goto err_out;
    }

    ip_sock = ipcom_accept(ip_listen_sock, NULL, NULL);
    if (ip_sock == IP_INVALID_SOCKET)
    {
        IPCOM_LOG1(ERR, "ipcom_start_extshell :: ipcom_accept() failed: %s", ipcom_strerror(ipcom_errno));
        goto err_out;
    }
    ipcom_socketclose(ip_listen_sock);

    /* Disable Nagle (enable no delay) on the sockets since it is an
       interactive connection */
    (void)setsockopt(vx_sock, IPPROTO_TCP, TCP_NODELAY, (char *) &enable, sizeof(enable));
    (void)ipcom_setsockopt(ip_sock, IP_IPPROTO_TCP, IP_TCP_NODELAY, &enable, sizeof(enable));

    linger.l_onoff  = 1;
    linger.l_linger = 0;
    (void)setsockopt(vx_sock, SOL_SOCKET, SO_LINGER, (char*)&linger, sizeof(linger));
    (void)ipcom_setsockopt(ip_sock, IP_SOL_SOCKET, IP_SO_LINGER, &linger, sizeof(linger));

    if (ipcom_pshell_spawn_proxy(vx_sock) != IPCOM_SUCCESS)
    {
        IPCOM_LOG0(ERR, "ipcom_start_extshell :: Failed to spawn proxy task");
        goto err_out;
    }
    *stdio_sock = ip_sock;
    ipcom_freeaddrinfo(res);

    return IPCOM_SUCCESS;

 err_out:
    ipcom_freeaddrinfo(res);
    if (ip_listen_sock != IP_INVALID_SOCKET)
        ipcom_socketclose(ip_listen_sock);
    if (vx_sock != IP_INVALID_SOCKET)
        close(vx_sock);
    if (ip_sock  != IP_INVALID_SOCKET)
        ipcom_socketclose(ip_sock);

    return rc;
}


/*
 *===========================================================================
 *                          ipcom_stdio_set_echo
 *===========================================================================
 * Description: Enables/disabled local echo on a telnet session.
 * Parameters:  fd - shell
 *              echo - IP_TRUE to enabled echo, IP_FALSE to disable
 * Returns:
 *
 */
IP_GLOBAL void
ipcom_stdio_set_echo (int fd, Ip_bool echo)
{
    int opts;
    int int_fd;


    if (fd == -1)
        int_fd = STD_IN;
    else
        int_fd = fd;

    opts = ioctl(int_fd, FIOGETOPTIONS, 0);
    if (opts == ERROR)
    {
        IPCOM_LOG1(ERR, "ipcom_stdio_set_echo :: ioctl(FIOGETOPTIONS) failed, errno=0x%x", errno);
        return;
    }
    if (echo)
        opts |= OPT_ECHO;
    else
        opts &= ~OPT_ECHO;

    if (ioctl(int_fd, FIOOPTIONS, opts) == ERROR)
        IPCOM_LOG1(ERR, "ipcom_stdio_set_echo :: ioctl(FIOOPTIONS) failed, errno=0x%x", errno);

}


/*
 ****************************************************************************
 * 11                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

#else
int ipcom_pshell_empty_file;
#endif /* IPCOM_USE_SHELL == IPCOM_SHELL_NATIVE */

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

