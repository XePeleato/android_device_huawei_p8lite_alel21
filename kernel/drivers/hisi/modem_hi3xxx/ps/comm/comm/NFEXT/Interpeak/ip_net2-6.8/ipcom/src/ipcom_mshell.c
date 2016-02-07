/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_mshell.c,v $ $Revision: 1.26 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/src/ipcom_mshell.c,v $
 *   $Author: jonas $
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

#define IPCOM_MSHELL_PORT 2323

/* Define if you want support for a 2nd login on the telnet port+1.
 * #define IPCOM_MSHELL_USE_PORT2
 */


/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#if IPCOM_USE_SHELL == IPCOM_SHELL_MINI

#define IPCOM_USE_CLIB_PROTO
#include "ipcom_sock.h"
#include "ipcom_clib.h"
#include "ipcom_clib2.h"
#include "ipcom_syslog.h"
#include "ipcom_err.h"
#include "ipcom_os.h"
#include "ipcom_shell.h"


/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

#define IPCOM_SYSLOG_PRIORITY    IPCOM_SYSLOG_IPCOM_PRIORITY
#define IPCOM_SYSLOG_FACILITY    IPCOM_LOG_IPCOM_SHELL

/* Telnet command codes */
#define IPCOM_TELNET_WILL           251
#define IPCOM_TELNET_WONT           252
#define IPCOM_TELNET_DO             253
#define IPCOM_TELNET_DONT           254
#define IPCOM_TELNET_IAC            255

/* Telnet option id's */
#define IPCOM_TELNET_ECHO           1
#define IPCOM_TELNET_SGA            3

#define IPCOM_MAX_ARGV      12

/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

typedef struct  Ipcom_mshell_command_struct
{
  IP_CONST char     *name;
  int              (*function)(int argc, char **argv);
}
Ipcom_mshell_command;


/*
 ****************************************************************************
 * 6                    EXTERN PROTOTYPES
 ****************************************************************************
 */

IP_PUBLIC int ipcom_cmd_ttcp(int argc, char **argv);

/*
 ****************************************************************************
 * 7                    LOCAL PROTOTYPES
 ****************************************************************************
 */

IP_GLOBAL Ip_err ipcom_mshell_init(void);

IPCOM_PROCESS(ipcom_mshell_proc);
#ifdef IPCOM_MSHELL_USE_PORT2
IPCOM_PROCESS(ipcom_mshell_proc2);
#endif

IP_STATIC int ipcom_mshell_cmd_msleep(int argc, char **argv);


/*
 ****************************************************************************
 * 8                    DATA
 ****************************************************************************
 */

IP_STATIC Ipcom_mshell_command ipcom_mshell_commands[] =
{
    {  "msleep", ipcom_mshell_cmd_msleep },
#ifndef IP_SIZE
    {  "ttcp", ipcom_cmd_ttcp },
#endif
    {  IP_NULL, IP_NULL }
};


/*
 ****************************************************************************
 * 9                    STATIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipcom_mshell
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipcom_mshell_cmd_msleep(int argc, char **argv)
{
    if(argc != 2)
        ipcom_fprintf(ip_stdout, "usage: msleep <milliseconds>\n\r");
    else
    {
        int msec = ipcom_atoi(argv[1]);

        ipcom_fprintf(ip_stdout, "Sleeping %d milliseconds, ZZzzz...\n\r", msec);
        ipcom_millisleep(msec);
        ipcom_fprintf(ip_stdout, "Morning!\n\r");
    }

    return 0;
}


/*
 *===========================================================================
 *                    ipcom_mshell
 *===========================================================================
 * Description:
 * Parameters:     Server TCP port.
 * Returns:        Never returns.
 *
 */
IP_STATIC void
ipcom_mshell(Ip_u16 port)
{
#ifdef IPCOM_USE_INET
    Ip_fd  mother4 = IP_INVALID_SOCKET;
    struct Ip_sockaddr_in   addr4;
#endif
#ifdef IPCOM_USE_INET6
    Ip_fd  mother6 = IP_INVALID_SOCKET;
    struct Ip_sockaddr_in6  addr6;
#endif
    Ip_ssize_t  length;
    Ip_u8 *buf, bufdata[64], cmd[160];
    int n, cmdlen, num, max = 0;
    Ip_fd_set  read_set;
    Ip_bool  host_echo, peer_echo;
    Ip_fd  ipcom_mshell_fd;
    Ipcom_proc *proc;

    ipcom_proc_init();

    proc = ipcom_proc_self();
    if(proc == IP_NULL)
    {
        IPCOM_LOG0(ERR, "ipcom_mshell :: ipcom_proc_self() failed");
        goto error;
    }

#ifdef IPCOM_USE_INET
    mother4 = ipcom_socket(IP_AF_INET, IP_SOCK_STREAM, 0);
    if(mother4 == IP_INVALID_SOCKET)
    {
        IPCOM_LOG1(ERR, "ipcom_mshell :: ipcom_socket(IPv4, IP_SOCK_STREAM) failed, errno = %d", ipcom_errno);
        goto ipv6;   /* If stack not compiled with IPv4, try IPv6 if enabled. */
    }
    max = mother4;

    ipcom_memset(&addr4, 0, sizeof(addr4));
    addr4.sin_len         = sizeof(struct Ip_sockaddr_in);
    addr4.sin_family      = IP_AF_INET;
    addr4.sin_port        = ip_htons(port);
    if(ipcom_bind(mother4, (struct Ip_sockaddr *)&addr4, sizeof(addr4)) == IP_SOCKERR)
    {
        IPCOM_LOG2(ERR, "ipcom_mshell :: ipcom_bind(IPv4, %d) failed, errno = %d",
                   port, ipcom_errno);
        goto error;
    }

    if(ipcom_listen(mother4, 5) == IP_SOCKERR)
    {
        IPCOM_LOG1(ERR, "ipcom_mshell :: ipcom_listen(IPv4) failed, errno = %d", ipcom_errno);
        goto error;
    }
#endif  /* IPCOM_USE_INET */

#ifdef IPCOM_USE_INET
 ipv6:
#endif

#ifdef IPCOM_USE_INET6
    mother6 = ipcom_socket(IP_AF_INET6, IP_SOCK_STREAM, 0);
    if(mother6 == IP_INVALID_SOCKET)
    {
        IPCOM_LOG1(ERR, "ipcom_mshell :: ipcom_socket(IPv6, IP_SOCK_STREAM) failed, errno = %d", ipcom_errno);
        goto main_loop;
    }
    max = IP_MAX(max, mother6);

    ipcom_memset(&addr6, 0, sizeof(struct Ip_sockaddr_in6));
    addr6.sin6_len    = sizeof(struct Ip_sockaddr_in6);
    addr6.sin6_family = IP_AF_INET6;
    addr6.sin6_port   = ip_htons(port);
    if(ipcom_bind(mother6, (struct Ip_sockaddr *)&addr6, sizeof(addr6)) == IP_SOCKERR)
    {
        IPCOM_LOG2(ERR, "ipcom_mshell :: ipcom_bind(IPv6, %d) failed, errno = %d",
                   port, ipcom_errno);
        goto error;
    }

    if(ipcom_listen(mother6, 5) == IP_SOCKERR)
    {
        IPCOM_LOG1(ERR, "ipcom_mshell :: ipcom_listen(IPv6) failed, errno = %d", ipcom_errno);
        goto error;
    }
#endif


#ifdef IPCOM_USE_INET6
 main_loop:
#endif

    for(;;)
    {
        IP_FD_ZERO(&read_set);
#ifdef IPCOM_USE_INET
        if(mother4 != IP_INVALID_SOCKET)
            IP_FD_SET(mother4, &read_set);
#endif
#ifdef IPCOM_USE_INET6
        if(mother6 != IP_INVALID_SOCKET)
            IP_FD_SET(mother6, &read_set);
#endif

        num = ipcom_socketselect(max + 1, &read_set, IP_NULL, IP_NULL, IP_NULL);

#ifdef IPCOM_USE_INET
        if(num > 0 && mother4 != IP_INVALID_SOCKET && IP_FD_ISSET(mother4, &read_set))
        {
            ipcom_mshell_fd = ipcom_accept(mother4, IP_NULL, IP_NULL);
            if (ipcom_mshell_fd == IP_INVALID_SOCKET)
            {
                IPCOM_LOG1(DEBUG, "ipcom_mshell :: ipcom_accept(IPv4) failed, errno = %d", ipcom_errno);
                continue;
            }
            goto accept;
        }
#endif
#ifdef IPCOM_USE_INET6
        if(num > 0 && mother6 != IP_INVALID_SOCKET && IP_FD_ISSET(mother6, &read_set))
        {
            ipcom_mshell_fd = ipcom_accept(mother6, IP_NULL, IP_NULL);
            if (ipcom_mshell_fd == IP_INVALID_SOCKET)
            {
                IPCOM_LOG1(DEBUG, "ipcom_mshell :: ipcom_accept(IPv6) failed, errno = %d", ipcom_errno);
                continue;
            }
            goto accept;
        }
#endif

        continue;


    accept:
        proc->shell_fd = ipcom_mshell_fd;

        /* Init the connection. */
        bufdata[0] = IPCOM_TELNET_IAC;
        bufdata[1] = IPCOM_TELNET_WILL;
        bufdata[2] = IPCOM_TELNET_ECHO;
        bufdata[3] = IPCOM_TELNET_IAC;
        bufdata[4] = IPCOM_TELNET_WILL;
        bufdata[5] = IPCOM_TELNET_SGA;
        if(ipcom_socketwrite(ipcom_mshell_fd, bufdata, 6) != 6)
            goto logout;
        host_echo = IP_TRUE;
        peer_echo = IP_FALSE;
        cmdlen = 0;

        if(ipcom_socketwrite(ipcom_mshell_fd, "Welcome to Interpeak Mini Shell.\n\r$> ", 38) != 38)
            goto logout;

        /* Logged in. */
        for(;;)
        {
            buf = bufdata;
            length = ipcom_socketread(ipcom_mshell_fd, bufdata, sizeof(bufdata));
            if(length <= 0)
                goto logout;

            /* Ignore all telnet options except ECHO. */
            while(length >= 2 && buf[0] == IPCOM_TELNET_IAC)
            {
                Ip_u8  reply;
                Ip_ssize_t  offset;

                reply = 0;
                offset = 3;

                switch(buf[1])
                {
                case IPCOM_TELNET_DO :
                    if (buf[2] == IPCOM_TELNET_ECHO)
                    {
                        host_echo = IP_TRUE;
                        reply = IPCOM_TELNET_WILL;
                    }
                    else if (buf[2] == IPCOM_TELNET_SGA)
                        reply = IPCOM_TELNET_WILL;
                    else
                        reply = IPCOM_TELNET_WONT;
                    break;
                case IPCOM_TELNET_DONT :
                    if (buf[2] == IPCOM_TELNET_ECHO)
                        host_echo = IP_FALSE;
                    reply = IPCOM_TELNET_WONT;
                    break;
                case IPCOM_TELNET_WILL :
                    if (buf[2] == IPCOM_TELNET_ECHO)
                    {
                        peer_echo = IP_TRUE;
                        reply = IPCOM_TELNET_DO;
                    }
                    else
                        reply = IPCOM_TELNET_WONT;
                    break;
                case IPCOM_TELNET_WONT :
                    if (buf[2] == IPCOM_TELNET_ECHO)
                        peer_echo = IP_FALSE;
                    reply = IPCOM_TELNET_DONT;
                    break;
                default:
                    offset = 2;
                    break;
                }

                if(reply != 0)
                {
                    Ip_u8 cbuf[3];

                    cbuf[0] = (Ip_u8)IPCOM_TELNET_IAC;
                    cbuf[1] = reply;
                    cbuf[2] = buf[2];
                    if(ipcom_socketwrite(ipcom_mshell_fd, cbuf, 3) != 3)
                        goto logout;
                }

                buf += offset;
                length -= offset;
            }

            if(length > 0 && peer_echo == IP_FALSE && host_echo == IP_TRUE)
            {
                /* Echo client data. */
                if(ipcom_socketwrite(ipcom_mshell_fd, buf, length) != length)
                    goto logout;

                /* Add 'buf' data to 'cmd' buffer.  */
                length = IP_MIN(length, sizeof(cmd) - cmdlen);
                ipcom_strncpy((char*)cmd + cmdlen, (char*)buf, length);
                cmdlen += length;
            }

            /* Full command buffer, parse what we got. */
            if(cmdlen == sizeof(cmd))
            {
                n = cmdlen - 1;
                goto parse_cmd;
            }

            /* Check for 'Enter' */
            for(n = 0; n < cmdlen; n++)
                if(cmd[n] == '\n' || cmd[n] == '\r')
                {
                    cmd[n] = '\0';
                    if(n > 0)
                    {
                        int   i, argc;
                        char *argv[IPCOM_MAX_ARGV];

                    parse_cmd:
                        i = 0;
                        argc = 0;

                        /* Fill in argv and set argc. */
                        while(cmd[i] == ' ')
                            i++;
                        argv[argc++] = (char*)&cmd[i];
                        for(; i < n; i++)
                            if(cmd[i] == ' ')
                            {
                                cmd[i++] = '\0';
                                while(cmd[i] == ' ')
                                    i++;
                                if(cmd[i] != '\0')
                                    argv[argc++] = (char*)&cmd[i];
                            }

                        if(ipcom_socketwrite(ipcom_mshell_fd, "\n\r", 2) != 2)
                            goto logout;

                        /* Run the command. */
                        ip_assert(argv[0] != IP_NULL);
                        if(ipcom_strcmp(argv[0], "exit") == 0)
                            goto logout;
                        if(ipcom_strcmp(argv[0], "help") == 0 || *argv[0] == '?')
                        {
                            if(ipcom_socketwrite(ipcom_mshell_fd, "? help exit", 11) != 11)
                                goto logout;
                            for(i = 0; ipcom_mshell_commands[i].name; i++)
                            {
                                if(ipcom_socketwrite(ipcom_mshell_fd, " ", 1) != 1)
                                    goto logout;
                                length = ipcom_strlen(ipcom_mshell_commands[i].name);
                                if(ipcom_socketwrite(ipcom_mshell_fd, ipcom_mshell_commands[i].name, length) != length)
                                    goto logout;
                            }
                            goto prompt;
                        }
                        for(i = 0; ipcom_mshell_commands[i].name; i++)
                            if(ipcom_strcmp(ipcom_mshell_commands[i].name, argv[0]) == 0)
                            {
                                (void)ipcom_mshell_commands[i].function(argc, argv);
                                goto prompt;
                            }
                        if(ipcom_socketwrite(ipcom_mshell_fd, "Unknown command, type '?' for help.", 35) != 35)
                            goto logout;
                    }

                    /* Write prompt. */
                prompt:
                    if(ipcom_socketwrite(ipcom_mshell_fd, "\n\r$> ", 5) != 5)
                        goto logout;
                    cmdlen = 0;
                }
        }

    logout:
        (void)ipcom_socketwrite(ipcom_mshell_fd, "Bye!\n\r", 6);
        ipcom_socketclose(ipcom_mshell_fd);
        proc->shell_fd = IP_INVALID_SOCKET;
    }

 error:
#ifdef IPCOM_USE_INET
    if(mother4 != IP_INVALID_SOCKET)
        ipcom_socketclose(mother4);
#endif
#ifdef IPCOM_USE_INET6
    if(mother6 != IP_INVALID_SOCKET)
        ipcom_socketclose(mother6);
#endif
    ipcom_proc_exit();
}


/*
 *===========================================================================
 *                    ipcom_mshell_proc
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IPCOM_PROCESS(ipcom_mshell_proc)
{
    ipcom_mshell(IPCOM_MSHELL_PORT);
}


/*
 *===========================================================================
 *                    ipcom_mshell_proc2
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
#ifdef IPCOM_MSHELL_USE_PORT2
IPCOM_PROCESS(ipcom_mshell_proc2)
{
    ipcom_mshell(IPCOM_MSHELL_PORT + 1);
}
#endif


/*
 ****************************************************************************
 * 10                   GLOBAL FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipcom_mshell_init
 *===========================================================================
 * Context:
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL Ip_err
ipcom_mshell_init(void)
{
    Ip_err  err;

    err = ipcom_proc_acreate("ipcom_mshell", (Ipcom_proc_func)ipcom_mshell_proc, IP_NULL, IP_NULL);
    if(err != IPCOM_SUCCESS)
        return err;

#ifdef IPCOM_MSHELL_USE_PORT2
    err = ipcom_proc_acreate("ipcom_mshell2", (Ipcom_proc_func)ipcom_mshell_proc2, IP_NULL, IP_NULL);
    if(err != IPCOM_SUCCESS)
        return err;
#endif

    return IPCOM_SUCCESS;
}


/*
 ****************************************************************************
 * 11                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipcom_shell_fwrite
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_size_t
ipcom_shell_fwrite(IP_CONST void *ptr, Ip_size_t size, Ip_size_t nitems, IP_FILE *stream)
{
    Ip_fd  ipcom_mshell_fd;
    Ipcom_proc *proc;
    int retval;

    (void)stream;

    if (size == 0 || nitems == 0)
        return 0;

    proc = ipcom_proc_self();
    if(proc == IP_NULL)
        return 0;
    ipcom_mshell_fd = proc->shell_fd;
    if(ipcom_mshell_fd == IP_INVALID_SOCKET)
        return 0;

    retval = (int)ipcom_socketwrite(ipcom_mshell_fd, ptr, size * nitems);
    if (retval < 0)
        return 0;
    return (Ip_size_t)(retval / size);
}


/*
 *===========================================================================
 *                    ipcom_shell_fread
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_size_t
ipcom_shell_fread(void *ptr, Ip_size_t size, Ip_size_t nitems, IP_FILE *stream)
{
    Ip_fd  ipcom_mshell_fd;
    Ipcom_proc *proc;
    int retval;

    (void)stream;

    proc = ipcom_proc_self();
    if(proc == IP_NULL)
        return 0;
    ipcom_mshell_fd = proc->shell_fd;
    if(ipcom_mshell_fd == IP_INVALID_SOCKET)
        return 0;

    retval = (int)ipcom_socketread(ipcom_mshell_fd, ptr, size * nitems);
    retval = IP_MAX(0, retval);
    return retval;
}


/*
 *===========================================================================
 *                    ipcom_shell_write
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_ssize_t
ipcom_shell_write(Ip_fd fd, IP_CONST void *buf, Ip_size_t count)
{
    return ipcom_shell_fwrite(buf, count, 1, IP_NULL /* ununsed */);
}


/*
 *===========================================================================
 *                    ipcom_shell_add_cmd
 *===========================================================================
 * Description:     Add a command to the list of available ipcom_shell commands
 * Parameters:      name : command's name
 *                  usage : usage string
 *                  description : description of command
 *                  hook : function to that executes the command
 *                  priority : na
 *                  stack_size : the stack to be used by the process that
 *                               executes the hook.
 *
 * Returns:         IPCOM_SUCCESS : ok
 *                  IPCOM_ERR_DUPLICATE : command already exists
 *                  IPCOM_ERR_NO_MEMORY : out of memory
 *
 *
 */
#if 0
IP_PUBLIC Ip_err
ipcom_shell_add_cmd(const char *name, const char *usage,
                    const char *description, Ipcom_shell_cmd_type hook,
                    Ip_s32 priority, Ip_s32 stack_size)
{
    (void)name;
    (void)usage;
    (void)description;
    (void)hook;
    (void)priority;
    (void)stack_size;
    return IPCOM_SUCCESS;
}
#endif


#else
int ipcom_mshell_empty_file;
#endif /* IPCOM_USE_SHELL == IPCOM_SHELL_MINI */


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

