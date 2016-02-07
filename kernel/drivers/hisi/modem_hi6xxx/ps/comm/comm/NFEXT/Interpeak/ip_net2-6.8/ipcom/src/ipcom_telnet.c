/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_telnet.c,v $ $Revision: 1.125 $
 *     $Source: /home/interpeak/CVSRoot/ipcom/src/ipcom_telnet.c,v $
 *     $Author: kenneth $
 *     $State: Exp $ $Locker:  $
 *
 *     INTERPEAK_COPYRIGHT_STRING
 *     Design and implementation by Markus Carlstedt <markus@interpeak.se>
 ******************************************************************************
 */

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 * ipcom_telnet.c contains source code for Interpeaks telnet server.
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

/* Ipcom include files */
#define IPCOM_USE_CLIB_PROTO
#include "ipcom_clib.h"
#include "ipcom_err.h"
#include "ipcom_os.h"
#include "ipcom_syslog.h"
#include "ipcom_sock.h"
#include "ipcom_strlib.h"
#include "ipcom_shell.h"
#include "ipcom_sysvar.h"
#include "ipcom_auth.h"
#include "ipcom_ipd.h"


/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

#define IPCOM_SYSLOG_PRIORITY    IPCOM_SYSLOG_IPCOM_PRIORITY
#define IPCOM_SYSLOG_FACILITY    IPCOM_LOG_IPCOM

/* Telnet daemon listening port and backlog */
#define IPCOM_TELNET_BACKLOG        5

/* Telnet command codes */
#define IPCOM_TELNET_SE             '\xF0'
#define IPCOM_TELNET_SB             '\xFA'
#define IPCOM_TELNET_WILL           '\xFB'
#define IPCOM_TELNET_WONT           '\xFC'
#define IPCOM_TELNET_DO             '\xFD'
#define IPCOM_TELNET_DONT           '\xFE'
#define IPCOM_TELNET_IAC            '\xFF'

/* Telnet option ids */
#define IPCOM_TELNET_ECHO           '\x01'
#define IPCOM_TELNET_SGA            '\x03'
#define IPCOM_TELNET_NAWS           '\x1F'

/* Maximum length of a telnet command */
#define IPCOM_TELNET_MAXCMD         80
#define IPCOM_TELNET_TEMPBUF        512  /* Not less than 2 */


#undef IPCOM_SYSLOG_FACILITY
#define IPCOM_SYSLOG_FACILITY    IPCOM_LOG_IPCOM_TELNET


/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */


/*
 *===========================================================================
 *                         Ipcom_telnet_info
 *===========================================================================
 */
typedef struct Ipcom_telnet_info_struct
{
    Ip_u32   msgtype;

    Ip_fd fd;       /* info. */
}
Ipcom_telnet_info;


/*
 *===========================================================================
 *                         Ipcom_telnet_buf
 *===========================================================================
 */
typedef struct Ipcom_telnet_buf_struct
{
    char        inbuf[IPCOM_TELNET_TEMPBUF];
    Ip_ssize_t  inlen;

    char        outbuf[IPCOM_TELNET_TEMPBUF];
    Ip_ssize_t  outlen;
    char        lastc;
}
Ipcom_telnet_buf;


/*
 *===========================================================================
 *                         Ipcom_telnet_opt
 *===========================================================================
 */
typedef struct Ipcom_telnet_opt_struct
{
    Ip_bool lcl_echo;
    Ip_bool rem_echo;
    Ip_bool lcl_sga;
    Ip_bool rem_sga;
#if IPCOM_USE_SHELL == IPCOM_SHELL_IPCOM
    Ip_bool lcl_naws;
    Ip_bool rem_naws;
#endif
}
Ipcom_telnet_opt;


/*
****************************************************************************
 * 6                    EXTERN PROTOTYPES
 ****************************************************************************
 */

IP_PUBLIC Ip_err iptelnets_configure(void);

#if IPCOM_USE_SHELL == IPCOM_SHELL_NATIVE
#ifdef IP_PORT_OSE5
IP_PUBLIC Ip_err ipcom_start_extshell(Ip_fd* stdio_sock, Ip_fd cli_sock, Ip_bool userpwd);
#else
IP_PUBLIC Ip_err ipcom_start_extshell(Ip_fd* stdio_sock, Ip_fd cli_sock);
#endif
#endif

#ifdef IP_PORT_OSE5
extern int efs_donate_fd(int fd, Ip_pid_t to);
extern int efs_receive_fd(Ip_pid_t from);
#endif


/*
 ****************************************************************************
 * 7                    LOCAL PROTOTYPES
 ****************************************************************************
 */


IP_PUBLIC Ip_err iptelnets_start(void);
IP_PUBLIC const char *iptelnets_version(void);

IPCOM_PROCESS(ipcom_telnetd);
IPCOM_PROCESS(ipcom_telnetspawn);

#ifdef IPCOM_USE_AUTH
IP_STATIC Ip_err ipcom_telnet_login(Ip_fd sock, Ipcom_telnet_buf *tbuf, Ipcom_telnet_opt *topt);
IP_STATIC Ip_err ipcom_telnet_getline(Ip_fd sock, Ipcom_telnet_buf *tbuf, Ipcom_telnet_opt *topt, char *buf, Ip_u32 size, Ip_bool echo);
#endif
IP_STATIC Ip_err ipcom_telnet_negotiate(Ip_fd sock, Ipcom_telnet_opt *topt);
IP_STATIC Ip_err ipcom_telnet_read(Ip_fd sock, Ipcom_telnet_buf *tbuf, Ipcom_telnet_opt *topt, Ip_bool echo);
IP_STATIC Ip_err ipcom_telnet_write(Ip_fd sock, char *buf, Ip_size_t size);


/*
 ****************************************************************************
 * 8                    DATA
 ****************************************************************************
 */
IP_STATIC int ipcom_telnet_session_count = 0;
IP_STATIC const char ipcom_telnet_max_session_msg[] = "Sorry, session limit reached.\n";

/*
 ****************************************************************************
 * 9                    STATIC FUNCTIONS
 ****************************************************************************
 */


/*
 *===========================================================================
 *                    ipcom_telnetd_spawn
 *===========================================================================
 * Context:
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err
ipcom_telnetd_spawn(Ip_fd fd)
{
    Ip_err             retval;
    Ipcom_telnet_info *tinfo;
    Ipcom_proc_attr    attr;
    Ip_pid_t           pid;
    struct Ip_linger   linger;
    int                enable = IP_TRUE;
    int session_max;
    int msr;

    session_max = ipcom_sysvar_get_as_int("ipcom.telnet.max_sessions", 0);
    if (session_max != 0 && ipcom_telnet_session_count >= session_max)
    {
        (void)ipcom_socketwrite(fd,
                                ipcom_telnet_max_session_msg,
                                ipcom_strlen(ipcom_telnet_max_session_msg));
        IPCOM_LOG1(WARNING, "ipcom_telnetd :: Maximum no of sessions reached = %d", session_max);
        ipcom_socketclose(fd);
    }

    /* Lower the linger time to 10 second if 'fd' is a socket */
    linger.l_onoff  = 1;
    linger.l_linger = 10;
    (void)ipcom_setsockopt(fd, IP_SOL_SOCKET, IP_SO_LINGER, &linger, sizeof(linger));

    /* Disable Nagle (enable no delay) on this socket since it is an
       interactive connection */
    (void)ipcom_setsockopt(fd, IP_IPPROTO_TCP, IP_TCP_NODELAY, &enable, sizeof(enable));

    ipcom_proc_attr_init(&attr);
    attr.priority   = IPCOM_PRIORITY_DEFAULT;
#ifdef IP_PORT_VXWORKS
    attr.stacksize  = IPCOM_PROC_STACK_LARGE;
#endif

    /* Spawn a new telnet process */
    retval = ipcom_proc_acreate("ipcom_telnetspawn", (Ipcom_proc_func)ipcom_telnetspawn, &attr, &pid);
    if (retval != IPCOM_SUCCESS)
        return retval;

    msr = ipcom_interrupt_disable();
    ipcom_telnet_session_count++;
    ipcom_interrupt_enable(msr);

    /* Wait for the message from the spawned process */
    retval = ipcom_ipc_receive(IP_NULL, &tinfo, -1);
    if (retval != IPCOM_SUCCESS)
        return retval;

    /* Reply with the 'fd' */
    tinfo->fd = fd;
    retval = ipcom_ipc_send(IPCOM_IPC_SENDER, tinfo);
    if (retval != IPCOM_SUCCESS)
        return retval;

#ifdef IP_PORT_OSE5
    /* OSE5 has process specific sockets -> donate child fd */
    retval = (Ip_err)efs_donate_fd(fd, pid);
    if (retval != 0)
        return IPCOM_ERR_FAILED;
#endif /* IP_PORT_OSE5 */

    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    ipcom_telnet_negotiate
 *===========================================================================
 * Description:     Negotiate telnet options with client at start of connection
 * Parameters:      sock - the client connection socket
 * Returns:         IPCOM_SUCCESS or IPCOM_ERR_FAILED
 *
 */
IP_STATIC Ip_err
ipcom_telnet_negotiate(Ip_fd sock, Ipcom_telnet_opt *topt)
{
    char serv_opt[6] =
        {
            IPCOM_TELNET_IAC, IPCOM_TELNET_WILL, IPCOM_TELNET_SGA,
            IPCOM_TELNET_IAC, IPCOM_TELNET_WILL, IPCOM_TELNET_ECHO
        };

    /* Send out the server's options */
    if (ipcom_telnet_write(sock, serv_opt, sizeof(serv_opt)) == IP_SOCKERR)
        return IPCOM_ERR_FAILED;

    topt->lcl_echo = IP_TRUE;
    topt->lcl_sga  = IP_TRUE;
    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    ipcom_telnet_set_option
 *===========================================================================
 * Description:
 * Parameters:
 *
 * Returns:
 *
 */
IP_STATIC Ip_s32
ipcom_telnet_set_option(int *cmd, Ip_bool *rem_state, Ip_bool rem_accept, Ip_bool *lcl_state, Ip_bool lcl_accept)
{
    if (*cmd == IPCOM_TELNET_WILL && rem_accept)
    {
        if (rem_state)
        {
            if (*rem_state == IP_TRUE)
                return 0;
            *rem_state = IP_TRUE;
        }
        *cmd = IPCOM_TELNET_DO;
    }
    else if (*cmd == IPCOM_TELNET_WONT ||
             (*cmd == IPCOM_TELNET_WILL && !rem_accept))
    {
        if (rem_state)
        {
            if (*rem_state == IP_FALSE)
                return 0;
            *rem_state = IP_FALSE;
        }
        *cmd = IPCOM_TELNET_DONT;
    }
    else if (*cmd == IPCOM_TELNET_DO && lcl_accept)
    {
        if (lcl_state)
        {
            if (*lcl_state == IP_TRUE)
                return 0;
            *lcl_state = IP_TRUE;
        }
        *cmd = IPCOM_TELNET_WILL;
    }
    else if (*cmd == IPCOM_TELNET_DONT ||
             (*cmd == IPCOM_TELNET_DO && !lcl_accept))
    {
        if (lcl_state)
        {
            if (*lcl_state == IP_FALSE)
                return 0;
            *lcl_state = IP_FALSE;
        }
        *cmd = IPCOM_TELNET_WONT;
    }
    else
        return 0;

    return 1;
}


/*
 *===========================================================================
 *                    ipcom_telnet_respond_option
 *===========================================================================
 * Description:
 * Parameters:
 *
 * Returns:
 *
 */
IP_STATIC Ip_s32
ipcom_telnet_respond_option(Ip_fd sock, int cmd, int option, Ipcom_telnet_opt *topt)
{
    Ip_u8 sopt[3];
    int ret;

    switch (option)
    {
        case IPCOM_TELNET_ECHO:
            /* Accept remote and local ECHO */
            ret = ipcom_telnet_set_option(&cmd, &topt->rem_echo, IP_TRUE, &topt->lcl_echo, IP_TRUE);
            break;

        case IPCOM_TELNET_SGA:
            /* Accept remote and local SGA */
            ret = ipcom_telnet_set_option(&cmd, &topt->rem_sga, IP_TRUE, &topt->lcl_sga, IP_TRUE);
            break;

#if IPCOM_USE_SHELL == IPCOM_SHELL_IPCOM
        case IPCOM_TELNET_NAWS:
            /* Accept remote NAWS. Negate local NAWS */
            ret = ipcom_telnet_set_option(&cmd, &topt->rem_naws, IP_TRUE, &topt->lcl_naws, IP_FALSE);
            break;
#endif

        default:
            /* Negate unknown options */
            ret = ipcom_telnet_set_option(&cmd, IP_NULL, IP_FALSE, IP_NULL, IP_FALSE);
            break;
    }

    if (ret == 1)
    {
        /* Send the resonse */
        sopt[0] = (Ip_u8)IPCOM_TELNET_IAC;
        sopt[1] = (Ip_u8)cmd;
        sopt[2] = (Ip_u8)option;
        if (ipcom_telnet_write(sock, (char *)sopt, sizeof(sopt)) == IP_SOCKERR)
            return -1;
    }

    return 0;
}


/*
 *===========================================================================
 *                    ipcom_telnet_input_option
 *===========================================================================
 * Description:
 * Parameters:
 *
 * Returns:      0 = Option not complete
 *               1 = Option complete
 *              -1 = Socket error
 *
 */
IP_STATIC Ip_s32
ipcom_telnet_input_option(Ip_fd sock, Ipcom_telnet_buf *tbuf, Ipcom_telnet_opt *topt, Ip_ssize_t *len)
{
    int cmd;
    char option, suboption;

    /* Need minimum one byte more for the command. */
    if (*len == tbuf->inlen)
    {
        (*len)--;
        return 0;
    }

    /* Get command */
    cmd = tbuf->inbuf[(*len)++];
    switch(cmd)
    {
        case IPCOM_TELNET_WILL:
        case IPCOM_TELNET_WONT:
        case IPCOM_TELNET_DO:
        case IPCOM_TELNET_DONT:
            if (*len == tbuf->inlen)
            {
                *len -= 2;
                return 0;
            }
            /* Get option */
            option = tbuf->inbuf[(*len)++];
            if (ipcom_telnet_respond_option(sock, cmd, option, topt) != 0)
                return -1;
            break;

        case IPCOM_TELNET_SB:
            if (*len == tbuf->inlen)
            {
                *len -= 2;
                return 0;
            }
            /* Get option */
            suboption = tbuf->inbuf[(*len)++];
            if (suboption == IPCOM_TELNET_NAWS)
            {
                /* window size is 4 bytes */
                if (tbuf->inlen - *len < 4)
                {
                    *len -= 3;
                    return 0;
                }

#if IPCOM_USE_SHELL == IPCOM_SHELL_IPCOM
                if (tbuf->inbuf[(*len)] != 0 || tbuf->inbuf[(*len) + 1] != 0)
                {
                    /* Propagate width to shell if it is != 0 */
                    tbuf->outbuf[tbuf->outlen++] = IPCOM_TELNET_IAC;
                    ipcom_memcpy(&tbuf->outbuf[tbuf->outlen], &tbuf->inbuf[(*len)], 2);
                    tbuf->outlen += 2;
                }
#endif
                *len += 4;
            }
            break;

        default:
            break;
    }

    return 1;
}


#ifdef IPCOM_USE_AUTH
/*
 *===========================================================================
 *                    ipcom_telnet_login
 *===========================================================================
 * Description:     Handle telnet login. Handles telnet login procedure.
 * Parameters:      sock - the client connection socket
 * Returns:         IPCOM_SUCCESS or IPCOM_ERR_FAILED or IP_SOCKERR
 *
 */
IP_STATIC Ip_err
ipcom_telnet_login(Ip_fd sock, Ipcom_telnet_buf *tbuf, Ipcom_telnet_opt *topt)
{
    char username[IPCOM_AUTH_USERNAME_MAX];
    char password[IPCOM_AUTH_PASSWORD_MAX];
    int tries;
    Ip_err  err;

    for (tries = 0; tries < 3; tries++)
    {
        /* Prompt for username */
        if (ipcom_socketwrite(sock, "login: ", 7) != 7)
            return IPCOM_ERR_FAILED;

        /* Get username */
        err = ipcom_telnet_getline(sock, tbuf, topt, username, sizeof(username), IP_TRUE);
        if (err != IPCOM_SUCCESS)
            return err;

        /* Prompt for password */
        if (ipcom_socketwrite(sock, "Password: ", 10) != 10)
            return IPCOM_ERR_FAILED;

        /* Scrap trailing single '\n' before reading password */
        if (tbuf->outlen >= 1  && tbuf->outbuf[0] == '\n')
            tbuf->outlen = 0;

        /* Get password */
        err = ipcom_telnet_getline(sock, tbuf, topt, password, sizeof(password), IP_FALSE);
        if (err != IPCOM_SUCCESS)
            return err;

        /* Scrap trailing single '\n' */
        if (tbuf->outlen >= 1  && tbuf->outbuf[0] == '\n')
            tbuf->outlen = 0;

        if (ipcom_socketwrite(sock, "\r\0\r\n", 4) != 4)
            return IPCOM_ERR_FAILED;

        /* Validate username and password */
        if (ipcom_auth_login(username, password, IP_NULL, IP_NULL, IP_NULL) == IPCOM_SUCCESS)
            return IPCOM_SUCCESS;

        /* Bad password, possibly retry */
        (void)ipcom_sleep(1);
        if (ipcom_socketwrite(sock, "Login incorrect\r\0\r\n\r\n", 21) != 21)
            return IPCOM_ERR_FAILED;
    }

    return IPCOM_ERR_FAILED;
}


/*
 *===========================================================================
 *                    ipcom_telnet_getline
 *===========================================================================
 * Description:     Read a line from a telnet connection. Block until
 *                  line is received
 * Parameters:      sock - the telnet connection socket
 *                  line - the buffer to store the received line.
 *                  size - size of buffer
 *                  echo - boolean for echo on/off
 * Returns:         IPCOM_SUCCESS or IPCOM_ERR_FAILED
 *
 */
IP_STATIC Ip_err
ipcom_telnet_getline(Ip_fd sock, Ipcom_telnet_buf *tbuf, Ipcom_telnet_opt *topt, char *buf, Ip_u32 size, Ip_bool echo)
{
    Ip_err  retval;

    for (;;)
    {
        retval = ipcom_telnet_read(sock, tbuf, topt, echo);
        if (tbuf->outlen > 0)
        {
            int i, max;

            for (i = 0; i < tbuf->outlen; i++)
                if (tbuf->outbuf[i] == '\r' || tbuf->outbuf[i] == '\n')
                {
                    max = IP_MIN((int)size-1, i);
                    if (max > 0)
                        ipcom_memcpy(buf, tbuf->outbuf, max);
                    buf[max] = '\0';

                    if (++i < tbuf->outlen)
                    {
                        /* Save trailing input. */
                        ipcom_memmove(tbuf->outbuf, tbuf->outbuf + i, tbuf->outlen - i);
                        tbuf->outlen -= i;
                        ip_assert(tbuf->outlen >= 0);
                    }
                    else
                        tbuf->outlen = 0;

                    return IPCOM_SUCCESS;
                }
        }
        if (retval != IPCOM_SUCCESS)
            return retval;
    }
}
#endif /* IPCOM_USE_AUTH */


/*
 *===========================================================================
 *                    ipcom_telnet_write
 *===========================================================================
 * Description:     Write a buffer to a telnet connection. Blocks until done.
 *                  Convert user CR/LF to telnet CR/LF
 * Parameters:      sock - the telnet connection socket
 *                  buf - pointer to the buffer to write
 *                  size - the number of bytes to write
 * Returns:         IPCOM_SUCCESS or IP_SOCKERR
 */
IP_STATIC Ip_err
ipcom_telnet_write(Ip_fd sock, char *buf, Ip_size_t size)
{
    char c, lastc = 0;
    Ip_s32 tmp_len, j=0;
    Ip_u32 i;
    char *tn_wbuf;

    if (size == 0)
        return IPCOM_SUCCESS;

    /* Allocate a temporary buffer. Add some extra for CR and LF */
    tmp_len = size + ((size / 80) << 1) + 2;
    tn_wbuf = ipcom_malloc(tmp_len);
    if (tn_wbuf == IP_NULL)
    {
        IPCOM_LOG0(ERR, "ipcom_telnet_write :: ipcom_malloc(tn_wbuf) failed");
        return IP_SOCKERR;
    }

    /* Copy user buffer to temp buffer and handle CR/LF */
    for (i = 0; i < size; i++)
    {
        c = buf[i];

        if (c == '\n' && lastc == '\r')
        {
            tn_wbuf[j++] = '\n';
        }
        else if (c != '\n' && lastc == '\r')
        {
            tn_wbuf[j++] = '\0';
            tn_wbuf[j++] = c;
        }
        else if (c == '\n')
        {
            tn_wbuf[j++] = '\r';
            tn_wbuf[j++] = '\n';
        }
        else
        {
            tn_wbuf[j++] = c;
        }

        lastc = c;

        /* Check if temp buffer is full - We need one extra */
        if (j > tmp_len - 3)
        {
            /* Send this buffer away */
            if (ipcom_socketwrite(sock, tn_wbuf, j) == IP_SOCKERR)
            {
                ipcom_free(tn_wbuf);
                return IP_SOCKERR;
            }
            /* Restart index */
            j = 0;
        }
    }

    /* Send the buffer away */
    if (j)
    {
        if (ipcom_socketwrite(sock, tn_wbuf, j) == IP_SOCKERR)
        {
            ipcom_free(tn_wbuf);
            return IP_SOCKERR;
        }
    }

    ipcom_free(tn_wbuf);
    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    ipcom_telnet_read
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err
ipcom_telnet_read(Ip_fd sock, Ipcom_telnet_buf *tbuf, Ipcom_telnet_opt *topt, Ip_bool echo)
{
    Ip_ssize_t  len, echo_start = tbuf->outlen;
    Ip_err retval = IPCOM_SUCCESS;
    char c = 0, lastc;
    Ip_s32 ret;

    /* Read the data from the socket. */
    len = ipcom_socketread(sock, tbuf->inbuf + tbuf->inlen, sizeof(tbuf->inbuf) - tbuf->inlen);

    /* Got EOF -> remote side closed socket. */
    if (len <= 0)
        retval = IPCOM_ERR_FAILED;
    else
        tbuf->inlen += len;

    /* Parse inbuf. */
    len = 0;
    while (len < tbuf->inlen)
    {
        c = tbuf->inbuf[len++];
        lastc = tbuf->lastc;
        tbuf->lastc = c;

        /* telnet command. */
        if (c == IPCOM_TELNET_IAC)
        {
            ret = ipcom_telnet_input_option(sock, tbuf, topt, &len);
            if (ret < 0)
            {
                retval = IPCOM_ERR_FAILED;
                goto breakout;
            }
            else if (ret == 0)
            {
                goto breakout;
            }
        }
        /* check if binary mode here else EOL conversion. */
        else
        {
            if (c == '\r')
            {
                /* ignore for now. */
            }
            else
            {
                if (tbuf->outlen < (Ip_ssize_t) sizeof(tbuf->outbuf))
                    tbuf->outlen++;
                else
                    echo_start = sizeof(tbuf->outbuf) - 1;
                if (c == '\0' && lastc == '\r')
                {
                    tbuf->outbuf[tbuf->outlen-1] = '\n';
                }
                else if (c == '\n' && lastc == '\r')
                {
                    tbuf->outbuf[tbuf->outlen-1] = '\n';
                }
                else
                {
                    tbuf->outbuf[tbuf->outlen-1] = c;
                }
            }
        }
    }

 breakout:
    /* Windows XP telnet client fix for single '\r'. */
    if (len == tbuf->inlen && c == '\r')
    {
        if (tbuf->outlen < (Ip_ssize_t) sizeof(tbuf->outbuf))
            tbuf->outlen++;
        else
            echo_start = sizeof(tbuf->outbuf) - 1;
        tbuf->outbuf[tbuf->outlen-1] = '\n';
    }

    /* Echo output characters. */
    if (echo == IP_TRUE && tbuf->outlen - echo_start > 0)
    {
        if (ipcom_telnet_write(sock, tbuf->outbuf + echo_start, tbuf->outlen - echo_start) == IP_SOCKERR)
        {
            retval = IPCOM_ERR_FAILED;
        }
    }

    /* adjust inbuf. */
    if (len == tbuf->inlen)
        tbuf->inlen = 0;
    else if (len > 0)
    {
        ipcom_memmove(tbuf->inbuf, tbuf->inbuf + len, tbuf->inlen - len);
        tbuf->inlen -= len;
    }

    return retval;
}


/*
 *===========================================================================
 *                    ipcom_telnetd
 *===========================================================================
 * Description:     The telnet daemon listens for connections and spawns
 *                  new processes for each connection.
 * Parameters:      None
 * Returns:         Nothing
 *
 */
IPCOM_PROCESS(ipcom_telnetd)
{
    Ip_fd max_sock = -1;
    Ip_fd_set read_set;
    Ip_err retval;
    Ip_fd ipd_sock = IP_INVALID_SOCKET;
#if defined(IPCOM_USE_INET) && (!defined(IPCOM_USE_INET6) || !defined(IP_PORT_LINUX) || IPCOM_USE_SOCK != IPCOM_SOCK_NATIVE)
    struct Ip_sockaddr_in sa;
#endif
#ifdef IPCOM_USE_INET
    Ip_fd lst_sock = IP_INVALID_SOCKET;
#endif
#ifdef IPCOM_USE_INET6
    struct Ip_sockaddr_in6 sa6;
    Ip_fd lst_sock6 = IP_INVALID_SOCKET;
#endif
#if IPCOM_VR_MAX > 1
    int vr = ipcom_proc_vr_get();
#endif
    int opt_val = 1;
    Ip_u16  port;

    ipcom_proc_init();

#if IPCOM_VR_MAX > 1
    (void)vr;
    IPCOM_LOG1(INFO, "ipcom_telnetd :: starting [VR %d]", vr);
#else
    IPCOM_LOG0(INFO, "ipcom_telnetd :: starting");
#endif

    /* Configure telnet */
    retval = iptelnets_configure();
    if (retval != IPCOM_SUCCESS)
    {
        IPCOM_LOG1(ERR, "ipcom_telnetd :: iptelnets_configure() failed, ret = %d", retval);
        goto exit;
    }

    /* Install IPC. */
    retval = ipcom_ipc_install();
    if (retval != IPCOM_SUCCESS)
    {
        IPCOM_LOG1(ERR, "ipcom_telnetd :: ipcom_ipc_install() failed, ret = %d", retval);
        goto exit;
    }

    port = (Ip_u16)ipcom_sysvar_get_as_int("ipcom.telnet.port", 23);

#if defined(IPCOM_USE_INET) && (!defined(IPCOM_USE_INET6) || !defined(IP_PORT_LINUX) || IPCOM_USE_SOCK != IPCOM_SOCK_NATIVE)
    /* Get a tcp listening socket, bind to the ipcom_telnetd external port and start listening */
    lst_sock = ipcom_socket(IP_AF_INET, IP_SOCK_STREAM, 0);
    if (lst_sock == IP_INVALID_SOCKET)
    {
        IPCOM_LOG1(WARNING, "ipcom_telnetd :: ipcom_socket(AF_INET) failed: %s", ipcom_strerror(ipcom_errno));
        goto ipv6;
    }
    max_sock = lst_sock;
    if (ipcom_setsockopt(lst_sock, IP_SOL_SOCKET, IP_SO_REUSEADDR, (void*) &opt_val, sizeof(opt_val)) )
    {
        IPCOM_LOG1(WARNING,
                   "ipcom_telnetd :: ipcom_setsockopt(IP_SO_REUSEADDR) failed for AF_INET: %s",
                   ipcom_strerror(ipcom_errno));
    }

    ipcom_memset(&sa, 0, sizeof(struct Ip_sockaddr_in));
    IPCOM_SA_LEN_SET(&sa, sizeof(struct Ip_sockaddr_in));
    sa.sin_family = IP_AF_INET;
    sa.sin_port   = ip_htons(port);
    sa.sin_addr.s_addr = IP_INADDR_ANY;
    if (ipcom_bind(lst_sock, (struct Ip_sockaddr *)&sa, sizeof(sa)) == IP_SOCKERR)
    {
        IPCOM_LOG1(ERR, "ipcom_telnetd :: ipcom_bind(AF_INET) failed: %s", ipcom_strerror(ipcom_errno));
        goto exit;
    }

    if (ipcom_listen(lst_sock, IPCOM_TELNET_BACKLOG) == IP_SOCKERR)
    {
        IPCOM_LOG1(ERR, "ipcom_telnetd :: ipcom_listen(AF_INET) failed: %s", ipcom_strerror(ipcom_errno));
        goto exit;
    }

 ipv6:
#endif /* IPCOM_USE_INET */

#ifdef IPCOM_USE_INET6
    /* Get a tcp listening v6 socket, bind to the ipcom_telnetd external port and start listening */
    lst_sock6 = ipcom_socket(IP_AF_INET6, IP_SOCK_STREAM, 0);
    if (lst_sock6 == IP_INVALID_SOCKET)
    {
        IPCOM_LOG1(ERR, "ipcom_telnetd :: ipcom_socket(AF_INET6) failed: %s", ipcom_strerror(ipcom_errno));
        goto accept;
    }
    max_sock = IP_MAX(max_sock, lst_sock6);
    if (ipcom_setsockopt(lst_sock6, IP_SOL_SOCKET, IP_SO_REUSEADDR, (void*) &opt_val, sizeof(opt_val)) )
    {
        IPCOM_LOG1(WARNING,
                   "ipcom_telnetd :: ipcom_setsockopt(IP_SO_REUSEADDR) failed for AF_INET6: %s",
                   ipcom_strerror(ipcom_errno));
    }

    ipcom_memset(&sa6, 0, sizeof(struct Ip_sockaddr_in6));
    IPCOM_SA_LEN_SET(&sa6, sizeof(struct Ip_sockaddr_in6));
    sa6.sin6_family = IP_AF_INET6;
    sa6.sin6_port = ip_htons(port);
    if (ipcom_bind(lst_sock6, (struct Ip_sockaddr *)&sa6, sizeof(sa6)) == IP_SOCKERR)
    {
        IPCOM_LOG1(ERR, "ipcom_telnetd :: ipcom_bind(AF_INET6) failed: %s", ipcom_strerror(ipcom_errno));
        goto exit;
    }
    if (ipcom_listen(lst_sock6, IPCOM_TELNET_BACKLOG) == IP_SOCKERR)
    {
        IPCOM_LOG1(ERR, "ipcom_telnetd :: ipcom_listen(AF_INET6) failed: %s", ipcom_strerror(ipcom_errno));
        goto exit;
    }
#endif /* IPCOM_USE_INET6 */

    /* Start the ipcom_telnetd accept loop... */
#ifdef IPCOM_USE_INET6
accept:
#endif
    if (max_sock == -1)
        goto exit;
    if (ipcom_ipd_init("iptelnets", IPCOM_SUCCESS, &ipd_sock) != IPCOM_SUCCESS)
        goto exit;
    max_sock = IP_MAX(max_sock, ipd_sock);
    IPCOM_LOG0(DEBUG2, "ipcom_telnetd :: listening for clients");
    for (;;)
    {
        Ip_fd tmp_sock;
        Ip_s32 num;

        IP_FD_ZERO(&read_set);
        IP_FD_SET(ipd_sock, &read_set);
#ifdef IPCOM_USE_INET
        if (lst_sock != IP_INVALID_SOCKET)
            IP_FD_SET(lst_sock, &read_set);
#endif
#ifdef IPCOM_USE_INET6
        if (lst_sock6 != IP_INVALID_SOCKET)
            IP_FD_SET(lst_sock6, &read_set);
#endif

        num = ipcom_socketselect(max_sock + 1, &read_set, IP_NULL, IP_NULL, IP_NULL);
        if (num == IP_SOCKERR)
        {
            IPCOM_LOG1(WARNING, "ipcom_telnetd :: ipcom_socketselect() failed, errno = %d", ipcom_errno);
            goto exit;
        }

        if (num > 0 && IP_FD_ISSET(ipd_sock, &read_set))
        {
            int event = ipcom_ipd_input(ipd_sock);
            switch(event)
            {
            case IPCOM_IPD_EVENT_KILL:
                IPCOM_LOG0(INFO, "received IPD kill message");
                goto exit;
            case IPCOM_IPD_EVENT_RECONFIGURE:
            default:
                /* ignore message */
                break;
            }
        }

#ifdef IPCOM_USE_INET
        if (num > 0 && lst_sock != IP_INVALID_SOCKET && IP_FD_ISSET(lst_sock, &read_set))
        {
#ifdef IPCOM_USE_INET6
            num--;
#endif
            tmp_sock = ipcom_accept(lst_sock, IP_NULL, IP_NULL);
            if (tmp_sock == IP_INVALID_SOCKET)
            {
                IPCOM_LOG1(WARNING, "ipcom_telnetd :: ipcom_accept() failed, errno =%d", ipcom_errno);
            }
            else
            {
                /* Spawn a new telnet process */
                retval = ipcom_telnetd_spawn(tmp_sock);
                if (retval != IPCOM_SUCCESS)
                {
                    IPCOM_LOG1(WARNING, "ipcom_telnetd :: ipcom_telnetd_spawn() failed, ret = %d", retval);
                    ipcom_socketclose(tmp_sock);
                }
            }
        }
#endif /* IPCOM_USE_INET */

#ifdef IPCOM_USE_INET6
        if (num > 0 && lst_sock6 != IP_INVALID_SOCKET && IP_FD_ISSET(lst_sock6, &read_set))
        {
            tmp_sock = ipcom_accept(lst_sock6, IP_NULL, IP_NULL);
            if (tmp_sock == IP_INVALID_SOCKET)
            {
                IPCOM_LOG1(WARNING, "ipcom_telnetd :: ipcom_accept(INET6) failed, errno = %d", ipcom_errno);
            }
            else
            {
                /* Spawn a new telnet process */
                retval = ipcom_telnetd_spawn(tmp_sock);
                if (retval != IPCOM_SUCCESS)
                {
                    IPCOM_LOG1(WARNING, "ipcom_telnetd :: ipcom_telnetd_spawn() failed, ret = %d", retval);
                    ipcom_socketclose(tmp_sock);
                }
            }
        }
#endif /* IPCOM_USE_INET6 */
    }

exit:
    if (ipd_sock == IP_INVALID_SOCKET)
        (void)ipcom_ipd_init("iptelnets", IPCOM_ERR_FAILED, &ipd_sock);
#ifdef IPCOM_USE_INET
    if (lst_sock != IP_INVALID_SOCKET)
        ipcom_socketclose(lst_sock);
#endif
#ifdef IPCOM_USE_INET6
    if (lst_sock6 != IP_INVALID_SOCKET)
        ipcom_socketclose(lst_sock6);
#endif

    (void)ipcom_ipc_uninstall();
    (void)ipcom_ipd_exit("iptelnets", ipd_sock);

    IPCOM_LOG0(INFO, "ipcom_telnetd :: ending");
    ipcom_proc_exit();
}


/*
 *===========================================================================
 *               ipcom_telnet_transfer_from_shell_to_client
 *===========================================================================
 * Description: Reads data from the telnet client and writes it to the shell.
 * Parameters:  shell_fd - socket connected to the shell.
 *              client_fd - socket connected to the telnet client.
 *              tbuf - temporary storage.
 * Returns:     IPCOM_SUCCESS or IPCOM_ERR_FAILED
 *
 */
IP_STATIC Ip_err
ipcom_telnet_transfer_from_shell_to_client(int shell_fd,
                                           int client_fd,
                                           Ipcom_telnet_buf  *tbuf)
{
    Ip_ssize_t len;
    Ip_err     retval;

    /* Forward the data to telnet client */
    len = ipcom_socketread(shell_fd, tbuf->outbuf, sizeof(tbuf->outbuf));
    if (len == IP_SOCKERR)
    {
        /* Note! will get a "connection reset by peer" when exiting the
           shell since the linger time is set to 0 on the shell side */
        IPCOM_LOG1(DEBUG2, "ipcom_telnetspawn :: ipcom_socket_read() failed, errno =%d", ipcom_errno);
        return IPCOM_ERR_FAILED;
    }

    if (len == 0)
        return IPCOM_ERR_FAILED;

    retval = ipcom_telnet_write(client_fd, tbuf->outbuf, len);
    if (retval != IPCOM_SUCCESS)
    {
        IPCOM_LOG1(WARNING, "ipcom_telnetspawn :: ipcom_telnet_write() failed, errno = %d", ipcom_errno);
        return IPCOM_ERR_FAILED;
    }
    /* Forward the data to the shell */
    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    ipcom_telnetspawn
 *===========================================================================
 * Description:     The spawned process handles each connection. It does
 *                  telnet option negotiation and login and then starts
 *                  the shell. During the connection data is forwarded
 *                  between the shell an the telnet client.
 *
 * Parameters:      None
 *
 * Returns:         Nothing
 *
 */
IPCOM_PROCESS(ipcom_telnetspawn)
{
    Ip_fd client_fd = IP_INVALID_SOCKET;
    Ip_fd shell_fd = IP_INVALID_SOCKET;
    Ip_fd max_sock;
    Ip_fd_set read_set;
    Ipcom_ipc ipc;
    Ipcom_telnet_info *tinfo;
    Ipcom_telnet_opt topt;
    Ip_err retval;
    Ipcom_telnet_buf  *tbuf = IP_NULL;
    Ip_bool ipc_opened = IP_FALSE;
    int enable;
    int msr;
    Ip_pid_t pid;
    int shell_sock_outq = 0;

    ipcom_proc_init();

    IPCOM_LOG0(DEBUG2, "ipcom_telnetspawn :: begin");

    /* Open IPC with telnet daemon. */
    retval = ipcom_ipc_open(&ipc, "ipcom_telnetd", -1);
    if (retval != IPCOM_SUCCESS)
    {
        IPCOM_LOG1(ERR, "ipcom_telnetspawn :: ipcom_ipc_open(\"ipcom_telnetd\") failed, ret = %d", retval);
        goto exit;
    }
    ipc_opened = IP_TRUE;

    /* Send a message to the telnet daemon */
    tinfo = ipcom_ipc_malloc(sizeof(Ipcom_telnet_info));
    if (tinfo == IP_NULL)
    {
        IPCOM_LOG0(ERR, "ipcom_telnetspawn :: ipcom_ipc_malloc() failed");
        goto exit;
    }
    retval = ipcom_ipc_send(&ipc, tinfo);
    if (retval != IPCOM_SUCCESS)
    {
        IPCOM_LOG1(ERR, "ipcom_telnetspawn :: ipcom_ipc_send() failed, ret = %d", retval);
        ipcom_ipc_free(tinfo);
        goto exit;
     }

    /* Wait for reply. */
    retval = ipcom_ipc_receive(IP_NULL, &tinfo, -1);
    if (retval != 0)
    {
        IPCOM_LOG1(ERR, "ipcom_telnetspawn :: ipcom_ipc_receive() failed, ret = %d", retval);
        if (tinfo != IP_NULL)
            ipcom_ipc_free(tinfo);
        goto exit;
     }

    /* Get info and release IPC buffer. */
    client_fd = tinfo->fd;
    ipcom_ipc_free(tinfo);

    /* Close IPC. */
    retval = ipcom_ipc_close(&ipc);
    if (retval != IPCOM_SUCCESS)
    {
        IPCOM_LOG1(ERR, "ipcom_telnetspawn :: ipcom_ipc_close(\"ipcom_telnetd\") failed, ret = %d", retval);
        goto exit;
    }
    ipc_opened = IP_FALSE;

    /* Change the owner of the telnet socket to this task */
#ifdef IP_PORT_OSE5
    client_fd = efs_receive_fd(0);
    if (client_fd == -1)
    {
        IPCOM_LOG0(ERR, "ipcom_telnetspawn :: efs_receive_fd() failed");
        goto exit;
    }
#endif

    /* Make this process the owner of this socket */
    pid = ipcom_getpid();
    if (ipcom_socketioctl(client_fd, IP_SIOCSPGRP, &pid) < 0)
    {
        IPCOM_LOG1(ERR, "ipcom_telnetspawn :: ipcom_socketioctl(IP_SIOCSPGRP) failed, errno = %d", ipcom_errno);
        goto exit;
    }

    /* Set keepalive option. */
    enable = 1;
    (void)ipcom_setsockopt(client_fd, IP_SOL_SOCKET, IP_SO_KEEPALIVE, &enable, sizeof(enable));

    /* Negotiate telnet options */
    ipcom_memset(&topt, 0, sizeof(topt)); /* All options IP_FALSE at start */
    if (ipcom_telnet_negotiate(client_fd, &topt) != IPCOM_SUCCESS)
    {
        IPCOM_LOG0(ERR, "ipcom_telnetspawn :: ipcom_telnet_negotiate() failed");
        goto exit;
    }

    /* Allocate two temporary buffer spaces for input. */
    tbuf = ipcom_calloc(2, sizeof(Ipcom_telnet_buf));
    if (tbuf == IP_NULL)
    {
        IPCOM_LOG0(ERR, "ipcom_telnetspawn :: ipcom_calloc(2, tbuf) failed");
        goto exit;
    }

#ifdef IPCOM_USE_AUTH
    if (ipcom_sysvar_get_as_int("ipcom.telnet.authenticate", 0) == 1)
    {
        /* Send OS info. */
        if (ipcom_telnet_write(client_fd, "\nIPCOM 6.7.x\n\n", 14) == IP_SOCKERR)
        {
            IPCOM_LOG0(ERR, "ipcom_telnetspawn :: ipcom_telnet_write() failed");
            goto exit;
        }

        /* Prompt login dialogue. */
        if (ipcom_telnet_login(client_fd, tbuf, &topt) != IPCOM_SUCCESS)
        {
            IPCOM_LOG0(INFO, "ipcom_telnetspawn :: ipcom_telnet_login() failed");
            goto exit;
        }
    }
#endif /* IPCOM_USE_AUTH */

#if IPCOM_USE_SHELL == IPCOM_SHELL_IPCOM
    /* Send: IPCOM_TELNET_IAC, IPCOM_TELNET_DO, IPCOM_TELNET_NAWS */
    if (ipcom_socketwrite(client_fd, "\xFF\xFD\x1F", 3) != 3)
    {
        IPCOM_LOG0(ERR, "ipcom_telnetspawn :: ipcom_socketwrite(DO NAWS) failed");
        goto exit;
    }
    topt.rem_naws = IP_TRUE;
#endif

    /* Start the shell */
#if IPCOM_USE_SHELL == IPCOM_SHELL_IPCOM
    retval = ipcom_start_shell(&shell_fd, client_fd);
#elif defined(IP_PORT_OSE5)
    retval = ipcom_start_extshell(&shell_fd, client_fd, IP_TRUE);
#elif IPCOM_USE_SHELL == IPCOM_SHELL_NATIVE
    retval = ipcom_start_extshell(&shell_fd, client_fd);
#endif
    if (retval != IPCOM_SUCCESS)
    {
        IPCOM_LOG0(ERR, "ipcom_telnetspawn :: ipcom_start_[ext]shell() failed");
        goto exit;
    }

    /* Prepare the read set */
    IP_FD_ZERO(&read_set);
    max_sock = IP_MAX(client_fd, shell_fd);

    /****** Start the select loop ******/
    IPCOM_LOG0(DEBUG2, "ipcom_telnetspawn :: select");
    for (;;)
    {
        int num;

        IP_FD_SET(client_fd, &read_set);
        IP_FD_SET(shell_fd, &read_set);

        num = ipcom_socketselect(max_sock + 1, &read_set, IP_NULL, IP_NULL, IP_NULL);
        if (num == IP_SOCKERR)
        {
            IPCOM_LOG1(WARNING, "ipcom_telnetspawn :: ipcom_socketselect() failed with error code \"%s\".", ipcom_strerror(ipcom_errno));
            goto exit;
        }

        /* Check for activity from shell */
        if (num > 0 && IP_FD_ISSET(shell_fd, &read_set))
        {
            retval = ipcom_telnet_transfer_from_shell_to_client(shell_fd, client_fd, tbuf);
            if (retval != IPCOM_SUCCESS)
                goto exit;
            continue;
        }

        /* Check for activity from terminal */
        if (num > 0 && IP_FD_ISSET(client_fd, &read_set))
        {
            retval = ipcom_telnet_read(client_fd, tbuf, &topt, IP_FALSE /* shell echos */);
            if (retval != IPCOM_SUCCESS)
                goto exit;

            if (tbuf->outlen > 0)
            {
                /* shell_sock_outq contains a lower bound on how much
                   space there is in the shell socket send queue. The
                   only reason for having this estimate is to lower
                   the number of times ipcom_socketioctl() and
                   ipcom_getsockopt() has to be called. */
                if ((shell_sock_outq -= tbuf->outlen * 2) < 0)
                {
                    int          outq;
                    int          sndbuf;
                    Ip_socklen_t sndbuf_len = sizeof(sndbuf);

                    /* Check that we can write all the data to the shell.
                       Omitting this check can lead to a deadlock where
                       the shell waits for the telnetspawn to free up
                       space in the socket, while the telnetspawn waits
                       for the shell to free up space in the same socket.
                       The deadlock is avoided by always prioritize reading
                       from the shell and writing it back to the telnet
                       client.
                    */
                    while (ipcom_socketioctl(shell_fd, IP_SIOCOUTQ, &outq) >= 0
                           && ipcom_getsockopt(shell_fd, IP_SOL_SOCKET, IP_SO_SNDBUF, &sndbuf, &sndbuf_len) >= 0
                           && ((shell_sock_outq = sndbuf - outq) / 2 < tbuf->outlen))
                    {
                        /* Not enough space in the send buffer to write
                           tbuf->outbuf content */
                        retval = ipcom_telnet_transfer_from_shell_to_client(shell_fd, client_fd, &tbuf[1]);
                        if (retval != IPCOM_SUCCESS)
                            goto exit;
                    }
                }

                if (ipcom_socketwrite(shell_fd, tbuf->outbuf, tbuf->outlen) == IP_SOCKERR)
                {
                    IPCOM_LOG1(WARNING,
                               "ipcom_telnetspawn :: ipcom_socketwrite() failed, errno = %d",
                               ipcom_errno);
                    goto exit;
                }
                tbuf->outlen = 0;
            }
        }
    }

 exit:

    IPCOM_LOG0(DEBUG2, "ipcom_telnetspawn :: end");
    msr = ipcom_interrupt_disable();
    ipcom_telnet_session_count--;
    ipcom_interrupt_enable(msr);

    if (tbuf != IP_NULL)
        ipcom_free(tbuf);

    if (shell_fd != IP_INVALID_SOCKET)
        ipcom_socketclose(shell_fd);
    if (client_fd != IP_INVALID_SOCKET)
        ipcom_socketclose(client_fd);

    if (ipc_opened)
        (void)ipcom_ipc_close(&ipc);

    ipcom_proc_exit();
}


/*
 ****************************************************************************
 * 10                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    iptelnets_start
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
iptelnets_start(void)
{
    Ipcom_proc_attr  attr;

    ipcom_proc_attr_init(&attr);

    return ipcom_proc_acreate("ipcom_telnetd", ipcom_telnetd, &attr, IP_NULL);
}


/*
 *===========================================================================
 *                    iptelnets_version
 *===========================================================================
 * Description::
 * Parameters::
 * Returns:
 *
 */
IP_PUBLIC const char *
iptelnets_version(void)
{
    return "@(#) IPTELNETS $Name: VXWORKS_ITER18A_FRZ10 $ - INTERPEAK_COPYRIGHT_STRING";
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
