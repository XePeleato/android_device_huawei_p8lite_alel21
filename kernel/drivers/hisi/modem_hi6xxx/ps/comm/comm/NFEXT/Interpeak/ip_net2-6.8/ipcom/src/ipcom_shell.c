/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_shell.c,v $ $Revision: 1.149 $
 *     $Source: /home/interpeak/CVSRoot/ipcom/src/ipcom_shell.c,v $
 *     $Author: ulf $
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
 * ipcom_shell.c contains source code for the ipcom shell. It communicates
 * with the telnet client and collects commands. It starts shell command
 * processes and relays data between them and and telnet clients.
 *
 */

/*
 ****************************************************************************
 * 2                    CONFIGURATION
 ****************************************************************************
 */

#include "ipcom_config.h"

/* Maximum length of a shell command including its arguments */
#define IPCOM_SHELL_CMD_STRING   512

/* Maximum number of commands in the shell history list */
#define IPCOM_SHELL_HISTORY_LENGTH  10


/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */


/* Ipcom include files */
#define IPCOM_USE_CLIB_PROTO
#include "ipcom_clib.h"
#include "ipcom_cstyle.h"
#include "ipcom_err.h"
#include "ipcom_getopt.h"
#include "ipcom_os.h"
#include "ipcom_shell.h"
#include "ipcom_sock.h"
#include "ipcom_sock2.h"
#include "ipcom_strlib.h"
#include "ipcom_syslog.h"
#include "ipcom_sysvar.h"
#include "ipcom_time.h"

/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

#define IPCOM_SYSLOG_PRIORITY    IPCOM_SYSLOG_IPCOM_PRIORITY
#define IPCOM_SYSLOG_FACILITY    IPCOM_LOG_IPCOM_SHELL



/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

#if IPCOM_USE_SHELL == IPCOM_SHELL_IPCOM

/*
 *===========================================================================
 *                         Ipcom_shell_intcmd
 *===========================================================================
 */
typedef struct Ipcom_shell_intcmd_struct
{
    char    name[IPCOM_SHELL_MAX_CMD_NAME_LENGTH];
    char    usage[IPCOM_SHELL_MAX_CMD_USAGE_LENGTH];
    char    description[IPCOM_SHELL_MAX_CMD_DESCRIPTION_LENGTH];

    Ipcom_shell_cmd_type   hook;
}
Ipcom_shell_intcmd;


/*
 *===========================================================================
 *                         Ipcom_shell_info
 *===========================================================================
 */
typedef struct Ipcom_shell_info_struct
{
    union Ip_sockaddr_union sa_prompt;
    Ip_pid_t ppid;
    Ip_u32   seqno;
    Ip_u16   port;
    Ip_u16   pad;
}
Ipcom_shell_info;


/*
 ****************************************************************************
 * 6                    EXTERN PROTOTYPES
 ****************************************************************************
 */

IP_EXTERN Ipcom_list ipcom_shell_cmd_head;

/*
 ****************************************************************************
 * 7                    LOCAL PROTOTYPES
 ****************************************************************************
 */

IPCOM_PROCESS(ipcom_shell);

IP_STATIC int ipcom_shell_cmd_help(int argc, char **argv);



/*
 ****************************************************************************
 * 8                    DATA
 ****************************************************************************
 */

/* Internal shell commands */
IP_STATIC const Ipcom_shell_intcmd shell_intcmd[] =
{
    /* exit - only to to be able to print usage and description */
    {
        "exit",
        "exit",
        "Exit shell",
        IP_NULL,
    },
    /* help */
    {
        "help",
        "help [command]",
        "Get help on command",
        ipcom_shell_cmd_help,
    },
    /* prompt */
    {
        "prompt",
        "prompt",
        "Set shell prompt",
        IP_NULL,
    },

#if IPCOM_USE_FILE != IPCOM_FILE_NONE
    /* cat */
    {
        "cat",
        "cat <file>",
        "Display file",
        ipcom_cmd_cat,
    },
    /* cd */
    {
        "cd",
        "cd <directory>",
        "Change working directory",
        ipcom_cmd_cd,
    },
    /* cp */
    {
        "cp",
        "cp <source file> <destination file>",
        "Copy file",
        ipcom_cmd_cp,
    },
    /* ls */
    {
        "ls",
        "ls [directory]",
        "Show directory contents",
        ipcom_cmd_ls,
    },
    /* mkdir */
    {
        "mkdir",
        "mkdir <directory>",
        "Create directory",
        ipcom_cmd_mkdir,
    },
    /* pwd */
    {
        "pwd",
        "pwd",
        "Show current directory",
        ipcom_cmd_pwd,
    },
    /* rm */
    {
        "rm",
        "rm <file>",
        "Delete file",
        ipcom_cmd_rm,
    },
    /* rmdir */
    {
        "rmdir",
        "rmdir <directory>",
        "Delete directory",
        ipcom_cmd_rmdir,
    },
    /* mv */
    {
        "mv",
        "mv <source> <dest>",
        "Renames/move file/directory",
        ipcom_cmd_mv,
    },
    /* uuencode */
    {
        "uuencode",
        "uuencode <file>",
        "Display uuencoded file",
        ipcom_cmd_uuencode,
    },
#endif /* IPCOM_USE_FILE */


#ifdef IPCOM_USE_ALIAS
    {
        "alias",
        "alias name \"cmd\"",
        "Add an alias",
        ipcom_shellalias_alias,
    },

    {
        "unalias",
        "unalias name",
        "Remove an alias",
        ipcom_shellalias_unalias,
    },
#endif

    /* No more commands */
    {
        "",
        "",
        "",
        IP_NULL
    }
};


/*
 ****************************************************************************
 * 9                    LOCAL FUNCTIONS
 ****************************************************************************
 */


#ifdef IP_PORT_UNIX
IP_GLOBAL void * ipcom_shell_info_new(void);
IP_GLOBAL void *
ipcom_shell_info_new(void)
{
    Ipcom_shell_info *shell_info;

    shell_info = ipcom_ipc_malloc(sizeof(Ipcom_shell_info));
    if (shell_info != IP_NULL)
    {
        ipcom_memset(shell_info, 0, sizeof(Ipcom_shell_info));
        shell_info->ppid = ipcom_getpid();
    }

    return shell_info;
}
#endif


/*
 *===========================================================================
 *                    ipcom_shell_cmd_help
 *===========================================================================
 * Description:     Shell command: Get help on command or display all commands
 * Parameters:      argc - number of arguments
 *                  argv - pointer to pointer to the arguments
 * Returns:         0
 *
 */
IP_STATIC int
ipcom_shell_cmd_help(int argc, char **argv)
{
    Ipcom_shell_cmd *cmd;
    Ipcom_shell_intcmd *intcmd;

    Ip_s32 i = 0;

    /* Check if argument is supplied */
    if (argc > 1)
    {
        intcmd = IP_NULL;

        /* Find shell internal command */
        while(shell_intcmd[i].name[0] != 0)
        {
            intcmd = (Ipcom_shell_intcmd *)&shell_intcmd[i];
            if (ipcom_strcmp(intcmd->name, argv[1]) == 0)
                break;

            intcmd = IP_NULL;
            i++;
        }
        if (intcmd)
        {
            ipcom_printf("Usage: %s\n", intcmd->usage);
            return 0;
        }

        /* Find shell command */
        cmd = ipcom_shell_find_cmd(argv[1]);
        if (cmd)
        {
            ipcom_printf("Usage: %s\n", cmd->usage);
            return 0;
        }

        /* Command not found */
        ipcom_printf("help: Unknown command '%s'\n", argv[1]);
    }
    else
    {
        /* List all shell internal commands */
        while(shell_intcmd[i].name[0] != 0)
        {
            ipcom_printf("%-32s %s\n", shell_intcmd[i].name, shell_intcmd[i].description);
            i++;
        }

        /* List all shell commands */
        for (cmd = IPCOM_LIST_FIRST(&ipcom_shell_cmd_head); cmd; cmd = IPCOM_LIST_NEXT(&cmd->cmd_list))
        {
            ipcom_printf("%-32s %s\n", cmd->name, cmd->description);
        }
        ipcom_printf("\n");
    }

    return 0;
}


/*
 *===========================================================================
 *                    ipcom_shell_print_prompt
 *===========================================================================
 * Description:     Print shell prompt.
 * Parameters:
 * Returns:         0 = success, <0 = error code.
 *
 */
IP_STATIC int
ipcom_shell_print_prompt(char *prompt_template, char *prompt_addr, int *promptlen, Ip_pid_t ppid, Ip_u32 seqno)
{
    int    baselen;
    int    ret = 0;
    int    buflen = 0;
    char  *buf;
#if IPCOM_USE_FILE != IPCOM_FILE_NONE
    char  *cwd;
    char  *base;
    char   cwdbuf[128];
#endif

    buf = ipcom_malloc(IPCOM_STDIO_BUFSIZE);
    if (buf == IP_NULL)
        return -1;

    for (;*prompt_template != '\0'; ++prompt_template)
    {
        ip_assert(buflen < (int)IPCOM_STDIO_BUFSIZE - 1);
        if (*prompt_template != '\\')
        {
            /* 'Normal' character */
            buf[buflen++] = *prompt_template;
        }
        else
        {
            /* New special character*/
            switch (*++prompt_template)
            {
            case 'w':
                /* Print current working directory */
            case 'W':
#if IPCOM_USE_FILE != IPCOM_FILE_NONE
                /* Print basename of the current workning directory */
                cwd = ipcom_getcwd(cwdbuf, sizeof(cwdbuf));
                if (cwd == IP_NULL)
                {
                    IPCOM_LOG0(DEBUG, "ipcom_shell_print_prompt :: ipcom_getcwd() failed");
                    break;
                }
                if (*prompt_template == 'w')
                    base = cwd;
                else
                {
                    base = ipcom_strrchr(cwd, '/');
                    if (base != IP_NULL && base != cwd)
                        base++;
                    else
                    {
                        base = ipcom_strrchr(cwd, '\\');
                        if (base == IP_NULL)
                            base = cwd;
                        else
                            base++;
                    }
                }
                baselen = ipcom_strlen(base);
                ipcom_memcpy(buf + buflen, base, baselen);
                buflen += baselen;
#endif
                break;

            case 'p':
                /* Print the shell process name */
                {
                    char procname[40];
                    ipcom_sprintf(procname, "ipcom_shell_%lx_%lx", (Ip_u32)ppid, seqno);
                    baselen = ipcom_strlen(procname);
                    ipcom_memcpy(buf + buflen, procname, baselen);
                    buflen += baselen;
                }
                break;

            case 'P':
                /* Print the shell process pid */
                {
                    char pid[14];
                    ipcom_sprintf(pid, "0x%lx", ipcom_getpid());
                    baselen = ipcom_strlen(pid);
                    ipcom_memcpy(buf + buflen, pid, baselen);
                    buflen += baselen;
                }
                break;

            case 'i':
                /* Print the IP address */
                if (*prompt_addr)
                {
                    baselen = ipcom_strlen(prompt_addr);
                    ipcom_memcpy(buf + buflen, prompt_addr, baselen);
                    buflen += baselen;
                }
                break;

#if IPCOM_USE_ENV != IPCOM_ENV_NONE
            case 'V':
                {
                    char vrbuf[16];
                    baselen = ipcom_sprintf(vrbuf, "%d", ipcom_proc_vr_get());
                    ipcom_memcpy(buf + buflen, vrbuf, baselen);
                    buflen += baselen;
                }
                break;
#endif

            default:
                /* Unknown code. */
                IPCOM_LOG1(DEBUG, "ipcom_shell_print_prompt :: unknown code: %c", *prompt_template);
                buf[buflen++] = '\\';
                buf[buflen++] = '\\';
                buf[buflen++] = *prompt_template;
                break;
            }
        }
    }

    if (buflen > 0)
    {
        buf[buflen] = '\0';
        if (promptlen != IP_NULL)
            *promptlen = ipcom_strlen(buf);
        ipcom_printf(buf);
    }
    ipcom_free(buf);
    if (ret < 0)
        IPCOM_LOG0(WARNING, "ipcom_shell_print_prompt :: ipcom_printf() failed");
    return ret;
}


/*
 *===========================================================================
 *                    ipcom_shell
 *===========================================================================
 * Description:     ipcom_shell process
 * Parameters:      None
 * Returns:         Nothing
 *
 */
IPCOM_PROCESS(ipcom_shell)
{
    Ipcom_shell_info   *sinfo;
    Ip_u16              stdio_port;
    char                stdio_port_str[6];
    Ip_err              retval;
    int                 argc, i;
    int                 sockerr;
    Ipcom_proc         *proc;
    /* Dynamic resources. */
    Ip_fd               child = IP_INVALID_SOCKET;
    char               *command = IP_NULL;
    Ipcom_getline      *gl = IP_NULL;
    char               *prompt;
    int                 promptlen = 0;
    char               *greeting;
    char              **argv = IP_NULL;
    Ip_pid_t            ppid = 0;
    Ip_u32              seqno = 0;
    Ip_pid_t            cmd_pid = 0;
    int                 proc_index = 1;
    struct Ip_addrinfo *res = IP_NULL;
    Ip_bool             echo = IP_FALSE;
#ifdef IPCOM_USE_INET6
    char                prompt_addr[IP_INET6_ADDRSTRLEN];
#else
    char                prompt_addr[IP_INET_ADDRSTRLEN];
#endif

    ipcom_proc_init();

    proc = ipcom_proc_self();
    if (proc != IP_NULL)
        IPCOM_LOG1(DEBUG2, "ipcom_shell :: starting(%x)", proc->pid);
    else
        goto exit;

    command = ipcom_malloc(IPCOM_SHELL_CMD_STRING);
    if (command == IP_NULL)
    {
        IPCOM_LOG0(ERR, "ipcom_shell :: ipcom_malloc() failed, out of memory");
        goto exit;
    }

    /* Install IPC. */
    retval = ipcom_ipc_install();
    if (retval != IPCOM_SUCCESS)
    {
        IPCOM_LOG1(ERR, "ipcom_shell :: ipcom_ipc_install() failed, ret = %d", retval);
        goto exit;
    }

    /* Wait for the message from ipcom_start_shell() function. */
    retval = ipcom_ipc_receive(IP_NULL, &sinfo, -1);
    if (retval != IPCOM_SUCCESS)
    {
        IPCOM_LOG1(ERR, "ipcom_shell :: ipcom_ipc_receive() failed, ret = %d", retval);
        goto exit;
    }

    /* Set shell prompt address */
    prompt_addr[0] = '\0';
#ifdef IPCOM_USE_INET
    if (sinfo->sa_prompt.sa.sa_family == IP_AF_INET)
    {
        if (ipcom_inet_ntop(IP_AF_INET, &sinfo->sa_prompt.sin.sin_addr, prompt_addr, sizeof(prompt_addr)) == IP_NULL)
            prompt_addr[0] = '\0';
    }
#endif
#ifdef IPCOM_USE_INET6
    if (sinfo->sa_prompt.sa.sa_family == IP_AF_INET6)
    {
        if (ipcom_inet_ntop(IP_AF_INET6, &sinfo->sa_prompt.sin6.sin6_addr, prompt_addr, sizeof(prompt_addr)) == IP_NULL)
            prompt_addr[0] = '\0';
    }
#endif

    /* Store info and free IPC info. */
    stdio_port = sinfo->port;
    ppid       = sinfo->ppid;
    seqno      = sinfo->seqno;

    if (stdio_port == 0)
        /* Use OS stdin/stdout */
        proc->shell_fd = IP_INVALID_SOCKET;
    else
    {
        struct Ip_addrinfo  hints;
        struct Ip_linger    linger;
        int                 enable = IP_TRUE;

        /* Get a tcp socket and connect to the telnet client listening port */
        ipcom_memset(&hints, 0, sizeof(hints));
        hints.ai_family   = sinfo->sa_prompt.sa.sa_family;
        hints.ai_socktype = IP_SOCK_STREAM;
        ipcom_sprintf(stdio_port_str, "%d", stdio_port);
        ipcom_ipc_free(sinfo);
        if (ipcom_getaddrinfo(IP_NULL, stdio_port_str, &hints, &res) != 0)
        {
            IPCOM_LOG1(ERR, "ipcom_shell :: ipcom_getaddrinfo() failed, errno = %d", ipcom_errno);
            goto exit;
        }

        child = ipcom_socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if (child == IP_INVALID_SOCKET)
        {
            IPCOM_LOG1(ERR, "ipcom_shell :: ipcom_socket(IP_AF_INET) failed, errno = %d", ipcom_errno);
            goto exit;
        }

        /* Note! the telnetspawn side will get a "connection reset by peer" when exiting the
           shell since the linger time is set to 0 */
        linger.l_onoff  = 1;
        linger.l_linger = 0;
        (void)ipcom_setsockopt(child, IP_SOL_SOCKET, IP_SO_LINGER, &linger, sizeof(linger));

        /* Disable Nagle (enable no delay) on this socket since it is an
           interactive connection */
        (void)ipcom_setsockopt(child, IP_IPPROTO_TCP, IP_TCP_NODELAY, &enable, sizeof(enable));

        sockerr = ipcom_connect(child, res->ai_addr, res->ai_addrlen);
        if (sockerr == IP_SOCKERR)
        {
            IPCOM_LOG1(ERR, "ipcom_shell :: ipcom_connect(IP_AF_INET) failed, errno = %d", ipcom_errno);
            goto exit;
        }

        /* Set the stdout/stdin/stderr descriptor */
        proc->shell_fd = child;
        echo = IP_TRUE;
    }

    /* Change directory to IPCOM_FILE_ROOT */
#if IPCOM_USE_FILE != IPCOM_FILE_NONE
    if (ipcom_chdir(IPCOM_FILE_ROOT) != 0)
    {
        IPCOM_LOG1(WARNING, "ipcom_shell :: ipcom_chdir(\"%s\") failed", IPCOM_FILE_ROOT);
    }
#endif

    /* Output greeting */
    greeting = ipcom_sysvar_get("ipcom.shell.greeting", IP_NULL, IP_NULL);
    if (greeting)
    {
        if (ipcom_printf(greeting) < 0)
        {
            IPCOM_LOG0(DEBUG, "ipcom_shell :: ipcom_printf(greeting) failed");
            ipcom_free(greeting);
            goto exit;
        }
        ipcom_free(greeting);
    }

    gl = ipcom_getline_init(IPCOM_SHELL_HISTORY_LENGTH);
    if (gl == IP_NULL)
        goto exit;

    /* Loop forever waiting for commands to execute or exit */
    for (;;)
    {
    more:
        argc = 0;

        /* Print shell prompt. */
        prompt = ipcom_sysvar_get("ipcom.shell.prompt", IP_NULL, IP_NULL);
        if (prompt)
        {
            if (ipcom_shell_print_prompt(prompt, prompt_addr, &promptlen, ppid, seqno) < 0)
            {
                ipcom_free(prompt);
                goto exit;
            }
            ipcom_free(prompt);
        }

        /* Wait for the command */
        command[0] = '\0';
        sockerr = ipcom_getline(child, command, IPCOM_SHELL_CMD_STRING, echo, gl, promptlen);
        if (sockerr < 0)
        {
            if (sockerr < -1)
                IPCOM_LOG1(DEBUG, "ipcom_shell :: ipcom_shell_getline() failed, errno = %d", ipcom_errno);
            goto exit;
        }

        /* Check if user just hit enter */
        if (command[0] == '\0')
            continue;

        if (argv != IP_NULL)
        {
            ipcom_free(argv);
            argv = IP_NULL;
        }

        /* Parse the command */
        retval = ipcom_parse_argstr(command, (int *)&argc, &argv);
        switch(retval)
        {
        case IPCOM_SUCCESS:
            break;

        case IPCOM_ERR_INVALID_ARG:
            ipcom_printf("Unmatched string delimiters\n");
            continue;

        case IPCOM_ERR_FAILED:
        default:
            IP_PANIC();
            IPCOM_LOG1(ERR, "ipcom_shell :: ipcom_parse_argstring('%s') failed", command);
            goto exit;
        }

        /* Check for valid argc and argv */
        if (argc == 0 || argv == IP_NULL)
            continue;

#ifdef IPCOM_USE_ALIAS
        retval = ipcom_shellalias_expand(&argc, &argv);
        if (retval != IPCOM_SUCCESS)
        {
            ipcom_printf("ipcom_shell: alias expansion failed with error %d", retval);
            continue;
        }
#endif

        /****** Check if user wants to exit ******/
        if (!ipcom_strcmp(argv[0], "exit"))
            goto exit;

        /****** Special prompt fix. ******/
        if (!ipcom_strcmp(argv[0], "prompt"))
        {
            if (argc == 1)
            {
                (void)ipcom_sysvar_set("ipcom.shell.prompt", "\\i$\\w> ", IPCOM_SYSVAR_FLAG_OVERWRITE);
                ipcom_printf("prompt reset\n");
            }
            else
            {
                (void)ipcom_sysvar_set("ipcom.shell.prompt", argv[1], IPCOM_SYSVAR_FLAG_OVERWRITE);
                ipcom_printf("ok\n");
            }
            continue;
        }

        /****** Check if internal shell command and run it ******/
        for (i = 0; shell_intcmd[i].name[0] != 0; i++)
            if (ipcom_strcmp(shell_intcmd[i].name, argv[0]) == 0)
            {
                if (shell_intcmd[i].hook)
                    (void)shell_intcmd[i].hook(argc, argv);   /* Run the command */
                goto more;
            }


        /****** Start the shell_cmd process ******/
        retval = ipcom_shell_run_extcmd(argc, argv, &child, ppid, seqno, &cmd_pid, &proc_index);
        proc->shell_fd = child;
        if (retval == 0)
        {
            ipcom_printf("ipcom_shell : Unknown command: '%s'.\n", argv[0]);
            continue;
        }
        else if (retval == -1)
        {
            IPCOM_LOG0(ERR, "ipcom_shell :: ipcom_shell_run_extcmd() failed");
            goto exit;
        }
    }

 exit:
    if (cmd_pid != 0)
    {
        /* Kill shell_cmd_xxx process by calling with argc == 0. */
        ip_assert(ppid != 0);
        (void)ipcom_shell_run_extcmd(0, IP_NULL, &child, ppid, seqno, &cmd_pid, &proc_index); /*lint !e644 ppid uninitialized */
        ip_assert(cmd_pid == 0);
        proc->shell_fd = child;
    }

    if (child != IP_INVALID_SOCKET)
        ipcom_socketclose(child);
    if (res != IP_NULL)
        ipcom_freeaddrinfo(res);
    if (command != IP_NULL)
        ipcom_free(command);
    if (gl != IP_NULL)
        ipcom_getline_exit(gl);
    if (argv != IP_NULL)
        ipcom_free(argv);
    if (proc != IP_NULL)
    {
        proc->shell_fd = IP_INVALID_SOCKET;
        IPCOM_LOG1(DEBUG2, "ipcom_shell :: terminating(%x)", proc->pid);
    }
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
 *                    ipcom_start_shell
 *===========================================================================
 * Description: Starts a shell process and sets up a TCP connection. The
 *              TCP connection is used to convey stdin and stdout data. The
 *              stdio proxy process and the underlaying shell process is
 *              terminated by closing the socket. If the shell terminates
 *              (e.g. if the user gives the command 'exit') the stdio proxy
 *              will close the TCP connection and terminate.
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipcom_start_shell(Ip_fd *stdio_sock, Ip_fd client_fd)
{
    Ip_fd               mother = IP_INVALID_SOCKET;
    Ip_fd               shell_fd = IP_INVALID_SOCKET;
    Ip_socklen_t        addr_len;
    Ipcom_ipc           ipc;
    Ipcom_shell_info   *sinfo;
    char                procname[40];
    Ip_err              retval;
    Ipcom_proc_attr     attr;
    Ip_bool             ipc_opened = IP_FALSE;
    Ip_pid_t            ppid, shell_pid;
    Ip_u16              mother_port;
    static Ip_u32       seqno = 0;
    struct Ip_addrinfo  hints;
    struct Ip_addrinfo *res = IP_NULL;
    union Ip_sockaddr_union sa;

    *stdio_sock = IP_INVALID_SOCKET;

    sinfo = ipcom_ipc_malloc(sizeof(Ipcom_shell_info));
    if (sinfo == IP_NULL)
    {
        IPCOM_LOG0(ERR, "ipcom_start_shell :: ipcom_ipc_malloc() failed");
        goto exit;
    }
    addr_len = sizeof(sinfo->sa_prompt);
    if (ipcom_getsockname(client_fd, (struct Ip_sockaddr *)&sinfo->sa_prompt, &addr_len) == IP_SOCKERR)
    {
        IPCOM_LOG1(ERR, "ipcom_start_shell :: ipcom_getsockname(client_fd) failed, errno = %d", ipcom_errno);
        goto exit;
    }

    /* Get a tcp socket and let the stack pick a port */
    ipcom_memset(&hints, 0, sizeof(hints));
    hints.ai_family   = sinfo->sa_prompt.sa.sa_family;
    hints.ai_socktype = IP_SOCK_STREAM;
    if (ipcom_getaddrinfo(IP_NULL, "0", &hints, &res) != 0)
    {
        IPCOM_LOG1(ERR, "ipcom_start_shell :: ipcom_getsockname(client_fd) failed, errno = %d", ipcom_errno);
        goto exit;
    }

    mother = ipcom_socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (mother == IP_INVALID_SOCKET)
    {
        IPCOM_LOG1(ERR, "ipcom_start_shell :: ipcom_socket(IP_AF_INET) failed, errno = %d", ipcom_errno);
        goto exit;
    }

    if (ipcom_bind(mother, res->ai_addr, res->ai_addrlen) == IP_SOCKERR)
    {
        IPCOM_LOG1(ERR, "ipcom_start_shell :: ipcom_bind(IP_AF_INET) failed, errno = %d", ipcom_errno);
        goto exit;
    }

    /* Find out which port was assigned. */
    addr_len = sizeof(sa);
    if (ipcom_getsockname(mother, (struct Ip_sockaddr *)&sa, &addr_len) == IP_SOCKERR)
    {
        IPCOM_LOG1(ERR, "ipcom_start_shell :: ipcom_getsockname(IP_AF_INET) failed, errno = %d", ipcom_errno);
        goto exit;
    }
    mother_port = ip_ntohs(sa.sin.sin_port);

    if (ipcom_listen(mother, 1) == IP_SOCKERR)
    {
        IPCOM_LOG1(ERR, "ipcom_start_shell :: ipcom_listen() failed, errno = %d", ipcom_errno);
        goto exit;
    }

    /* Create and start the shell process. */
    ipcom_proc_attr_init(&attr);
#ifdef IP_PORT_VXWORKS
    attr.stacksize = IPCOM_PROC_STACK_LARGE;
#else
    attr.priority = IPCOM_PRIORITY_DEFAULT;
#endif /* IP_PORT_VXWORKS */
    ppid = ipcom_getpid();
    ipcom_sprintf(procname, "ipcom_shell_%lx_%lx", (Ip_u32)ppid, ++seqno);
    if (ipcom_proc_acreate(procname, (Ipcom_proc_func)ipcom_shell, &attr, &shell_pid))
    {
        IPCOM_LOG0(ERR, "ipcom_start_shell :: ipcom_proc_acreate() failed");
        goto exit;
    }

    /* Open IPC with ipcom_shell. */
    retval = ipcom_ipc_open(&ipc, procname, -1);
    if (retval != IPCOM_SUCCESS)
    {
        IPCOM_LOG2(ERR, "ipcom_start_shell :: ipcom_ipc_open(%s) failed, ret = %d", procname, retval);
        goto exit;
    }
    ipc_opened = IP_TRUE;

    /* Send a message to ipcom_shell. */
    sinfo->port  = mother_port;
    sinfo->ppid  = ppid;
    sinfo->seqno = seqno;
    retval = ipcom_ipc_send(&ipc, sinfo);
    if (retval != IPCOM_SUCCESS)
    {
        IPCOM_LOG1(ERR, "ipcom_start_shell :: ipcom_ipc_send() failed, ret = %d", retval);
        goto exit;
     }

    /* Wait for the shell process to connect */
    shell_fd = ipcom_accept(mother, IP_NULL, IP_NULL);

exit:
    if (mother != IP_INVALID_SOCKET)
        (void)ipcom_socketclose(mother);
    if (res != IP_NULL)
        ipcom_freeaddrinfo(res);
    if (ipc_opened)
        (void)ipcom_ipc_close(&ipc);
    else if (sinfo != IP_NULL)
        ipcom_ipc_free(sinfo);

    if (shell_fd == IP_INVALID_SOCKET)
        return IPCOM_ERR_FAILED;
    else
    {
        int enable = IP_TRUE;

        *stdio_sock = shell_fd;
        /* Disable Nagle (enable no delay) on this socket since it is an
           interactive connection */
        (void)ipcom_setsockopt(shell_fd, IP_IPPROTO_TCP, IP_TCP_NODELAY, &enable, sizeof(enable));
        return IPCOM_SUCCESS;
    }
}

#else
int ipcom_shell_empty_file;
#endif   /* #if IPCOM_USE_SHELL == IPCOM_SHELL_IPCOM */


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
