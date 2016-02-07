/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_shellcmd.c,v $ $Revision: 1.74 $
 *     $Source: /home/interpeak/CVSRoot/ipcom/src/ipcom_shellcmd.c,v $
 *     $Author: rboden $
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
 * ipcom_shellcmd.c includes source code for the shell cmd process
 * which runs the shell command functions.
 *
01a,11oct07,rme  Defect:WIND00107249:Fix:WIND00107665 ipcom commands not present on console
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

#define IPCOM_SHELL_C
#if IPCOM_USE_SHELL == IPCOM_SHELL_IPCOM || \
    (defined(WRS_IPNET) && IPCOM_USE_SHELL != IPCOM_SHELL_NONE)

/* Ipcom include files */
#define IPCOM_USE_CLIB_PROTO
#include "ipcom_clib.h"
#include "ipcom_err.h"
#include "ipcom_os.h"
#include "ipcom_syslog.h"
#include "ipcom_sock.h"
#include "ipcom_strlib.h"
#include "ipcom_shell.h"


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

/*
 *===========================================================================
 *                         Ipcom_shellcmd_info
 *===========================================================================
 */
typedef struct Ipcom_shellcmd_info_struct
{
    Ipcom_shell_cmd_type   hook;    /* Command function pointer */
    int       argc;                 /* Command argc */
    char    **argv;                 /* Command argv */
    Ip_fd     fd;                   /* Standard input/output */
    Ip_pid_t  pid;                  /* ipcom_shell_x pid */
    Ip_pid_t  ppid;                 /* ipcom_telnet_spawn pid */
    Ip_u32    seqno;
    char      cwd[IPCOM_SHELL_MAX_PATH];
    Ip_s32    prio;
}
Ipcom_shellcmd_info;


/*
 ****************************************************************************
 * 6                    EXTERN PROTOTYPES
 ****************************************************************************
 */

#if defined(WRS_IPNET) && defined(IP_PORT_VXWORKS) && (IP_PORT_VXWORKS >= 65) \
    && (IPCOM_USE_SHELL == IPCOM_SHELL_NATIVE)
IP_EXTERN Ip_err ipcom_vxshell_add_cmd(const char *name, const char *usage,
                                       const char *description,
                                       Ipcom_shell_cmd_type hook, Ip_s32 priority,
                                       Ip_s32 stack_size);
#endif /* defined(IP_PORT_VXWORKS) && (IP_PORT_VXWORKS >= 65) */

#ifdef IP_PORT_OSE5
extern int efs_donate_fd(int fd, Ip_pid_t to);
extern int efs_receive_fd(Ip_pid_t from);
#endif


/*
 ****************************************************************************
 * 7                    LOCAL PROTOTYPES
 ****************************************************************************
 */

IPCOM_PROCESS(ipcom_shell_cmd);

IP_STATIC Ip_err ipcom_shellcmd_init(void *unused);


/*
 ****************************************************************************
 * 8                    DATA
 ****************************************************************************
 */

IP_PUBLIC Ipcom_list ipcom_shell_cmd_head;


/*
 ****************************************************************************
 * 9                    LOCAL FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipcom_shellcmd_init
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err
ipcom_shellcmd_init(void *unused)
{
    (void)unused;

    ipcom_list_init(&ipcom_shell_cmd_head);

    return IPCOM_SUCCESS;
}


#if !defined(WRS_IPNET) || IPCOM_USE_SHELL == IPCOM_SHELL_IPCOM
/*
 *===========================================================================
 *                    ipcom_shell_cmd
 *===========================================================================
 * Description:     The shell cmd process. Waits for a command and executes it.
 * Parameters:      None
 * Returns:         Nothing
 *
 */
IPCOM_PROCESS(ipcom_shell_cmd)
{
    Ipcom_proc *proc;
    Ipcom_shellcmd_info  *sinfo = IP_NULL;
    Ipcom_ipc        ipc;
    Ip_u32  *einfo;
    Ip_err retval;
    Ipcom_shell_cmd_type   hook;
    int     argc;
    char  **argv;
#ifdef IP_PORT_OSE5
    Ip_pid_t   ppid;
#endif
#if defined(IP_PORT_OSE) || defined(IP_PORT_OSE5)
    Ip_pid_t   pid;
#endif

    ipcom_proc_init();

    proc = ipcom_proc_self();
    if (proc == IP_NULL)
    {
        IPCOM_LOG0(ERR, "ipcom_shell_cmd :: proc == IP_NULL");
        ipcom_exit(0);
    }

    IPCOM_LOG1(DEBUG2, "ipcom_shell_cmd :: starting(%x)", proc->pid);

    /* Install IPC. */
    retval = ipcom_ipc_install();
    if (retval != IPCOM_SUCCESS)
    {
        IPCOM_LOG1(ERR, "ipcom_shell_cmd :: ipcom_ipc_install() failed, ret = %d", retval);
        ipcom_exit(0);
    }

    for (;;)
    {
        /* Wait for the message from ipcom_start_shell() function. */
        retval = ipcom_ipc_receive(&ipc, &sinfo, -1);
        if (retval != IPCOM_SUCCESS)
        {
            IP_PANIC2();
            IPCOM_LOG1(ERR, "ipcom_shell_cmd :: ipcom_ipc_receive() failed, ret = %d", retval);
            break;
        }

        /* Received kill command. */
        if (sinfo->argc == 0)
            break;

#ifdef IP_PORT_OSE5
        /* OSE5 has process specific sockets -> receive donated fd */
        sinfo->fd = efs_receive_fd(0);
        if (sinfo->fd == -1)
        {
            IP_PANIC();
            break;
        }
#endif

#if defined(IP_PORT_OSE) || defined(IP_PORT_OSE5)
        /* Change child socket owner (A must for OSE to work due to poor ipcom_block impl). */
        pid = ipcom_getpid();
        if (ipcom_socketioctl(sinfo->fd, IP_SIOCSPGRP, &pid) < 0)
        {
            IP_PANIC2();
            IPCOM_LOG1(WARNING, "ipcom_shell_cmd :: ipcom_socketioctl(IP_SIOCSPGRP) failed, errno = %d", ipcom_errno);
        }
#endif

        hook = sinfo->hook;
        argc = sinfo->argc;
        argv = sinfo->argv;

        /*IPCOM_LOG2(DEBUG2, "ipcom_shell_cmd :: > %s %s", argv[0], argc > 1 ? argv[1] : "");*/

        /* Set the stdout/stdin/stderr descriptor and the shell parent pid. */
        proc->shell_fd    = sinfo->fd;
        proc->shell_ppid  = sinfo->ppid;
        proc->shell_seqno = sinfo->seqno;
#ifdef IP_PORT_OSE5
        ppid              = sinfo->pid;
#endif

        /* Change the current working directory */
#if IPCOM_USE_FILE != IPCOM_FILE_NONE
        if (sinfo->cwd[0])
            (void)ipcom_chdir(sinfo->cwd);
#endif
        /* Change the task priority */
        (void)ipcom_proc_setprio(ipcom_getpid(), (Ip_u16)sinfo->prio);

        ipcom_ipc_free(sinfo);
        sinfo = IP_NULL;

        /* Execute the shell command */
        if (hook)
            (void)hook(argc, argv);

        /* Send back notice that we are still alive. */
    again:
        einfo = ipcom_ipc_malloc(8);
        if (einfo == IP_NULL)
        {
            IP_PANIC();
            ipcom_millisleep(100);
            goto again;
        }

        /* Send the IPC info. */
        einfo[0] = 0; /* process still alive, ipcom_exit() was not called. */
        einfo[1] = 0; /* status */
        retval = ipcom_ipc_send(&ipc, einfo);
        (void)ipcom_ipc_close(&ipc);
        if (retval != IPCOM_SUCCESS)
        {
            IP_PANIC();
            IPCOM_LOG1(ERR, "ipcom_shell_cmd :: ipcom_ipc_send() failed, ret = %d", retval);
            break;
        }

#ifdef IP_PORT_OSE5
        /* OSE5 has process specific sockets -> donate child fd */
        retval = (Ip_err)efs_donate_fd(proc->shell_fd, ppid);
        if (retval != 0)
        {
            IP_PANIC();
            IPCOM_LOG1(ERR, "ipcom_shell_cmd :: efs_donate_fd, ret = %d", retval);
            break;
        }
#endif /* IP_PORT_OSE5 */
    }

    /* Cleanup and terminate. */
    if (ipcom_ipc_isopen(&ipc))
        (void)ipcom_ipc_close(&ipc);
    if (sinfo != IP_NULL)
        ipcom_ipc_free(sinfo);
    ipcom_exit(0);
}
#endif /* !defined(WRS_IPNET) || IPCOM_USE_SHELL == IPCOM_SHELL_IPCOM */


/*
 ****************************************************************************
 * 10                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

#if !defined(WRS_IPNET) || IPCOM_USE_SHELL == IPCOM_SHELL_IPCOM
/*
 *===========================================================================
 *                    ipcom_shell_run_extcmd
 *===========================================================================
 * Description: Starts a shell command process and delivers the command arguments
 *              and stdio socket to that process.
 * Parameters:  argc, argv - traditional command arguments
 *              stdio_fd - the standard input, output and error sock
 *              ppid - parent pid
 *              cmd_pid - shell command process id
 *
 * Returns:          1 : command process started
 *                   0 : not an external command
 *                  -1 : error
 *
 */
IP_PUBLIC Ip_err
ipcom_shell_run_extcmd(int argc, char **argv, Ip_fd *stdio_fd, Ip_pid_t ppid, Ip_u32 seqno, Ip_pid_t *cmd_pid, int *proc_index)
{
    Ipcom_proc_attr  attr;
    Ipcom_ipc        ipc;
    Ipcom_shellcmd_info  *sinfo;
    Ipcom_shell_cmd   *cmd = IP_NULL;
    Ipcom_shell_cmd   *tcmd;
    Ip_err retval;
    char procname[40];
    Ip_u32    stack = IPCOM_PROC_STACK_DEFAULT;
    Ip_pid_t  pid;

    pid = ipcom_getpid();
    ipcom_sprintf(procname, "ipcom_sc_0x%lx_%d", (Ip_u32)pid, *proc_index);

    /* Find command and max stack size (since we are reusing the process). */
    if (argc > 0)
    {
        for (tcmd = IPCOM_LIST_FIRST(&ipcom_shell_cmd_head); tcmd; tcmd = IPCOM_LIST_NEXT(&tcmd->cmd_list))
        {
            stack = (Ip_u32)IP_MAX(stack, tcmd->stack_size);
            if (cmd == IP_NULL && ipcom_strcmp(tcmd->name, argv[0]) == 0)
            {
                cmd = tcmd;
                if (*cmd_pid != 0)
                    break;
            }
        }
        if (cmd == IP_NULL)
            return 0;

        /* Start the shell_cmd process. */
        if (*cmd_pid == 0)
        {
            ipcom_proc_attr_init(&attr);
            attr.priority  = (Ip_u32)cmd->priority;
            attr.stacksize = stack;
            attr.flags |= IPCOM_PROC_FLAG_FP;
            if (ipcom_proc_acreate(procname, (Ipcom_proc_func)ipcom_shell_cmd, &attr, cmd_pid))
            {
                IPCOM_LOG0(ERR, "ipcom_shell_run_extcmd :: ipcom_proc_acreate() failed");
                goto fail;
            }
            ip_assert(*cmd_pid != 0);
        }
    }
    else
    {
        /* argc == 0 is used to kill the shell_cmd process. */
        ip_assert(*cmd_pid != 0);
    }

    /* Open IPC with ipcom_shell. */
    retval = ipcom_ipc_open(&ipc, procname, -1);
    if (retval != IPCOM_SUCCESS)
    {
        IPCOM_LOG2(ERR, "ipcom_shell_run_extcmd :: ipcom_ipc_open(%s) failed, ret = %d", procname, retval);
        goto fail;
    }

    /* Send a message to ipcom_shell. */
    sinfo = ipcom_ipc_malloc(sizeof(Ipcom_shellcmd_info));
    if (sinfo == IP_NULL)
    {
        IPCOM_LOG1(ERR, "ipcom_shell_run_extcmd :: ipcom_ipc_malloc() failed, ret = %d", retval);
        goto fail;
    }

    /* Fill in IPC info. */
    sinfo->argc = argc;
    if (argc > 0)
    {
        sinfo->hook  = cmd->hook;
        sinfo->argv  = argv;
        sinfo->fd    = *stdio_fd;
        sinfo->pid   = pid;
        sinfo->ppid  = ppid;
        sinfo->seqno = seqno;
#if IPCOM_USE_FILE != IPCOM_FILE_NONE
        if (ipcom_getcwd(sinfo->cwd, sizeof(sinfo->cwd)) == IP_NULL)
            ipcom_memset(sinfo->cwd, 0, sizeof(sinfo->cwd));
        else
            sinfo->cwd[sizeof(sinfo->cwd)-1] = '\0';
#endif
        sinfo->prio = cmd->priority;
    }

    /* Send the IPC info. */
    retval = ipcom_ipc_send(&ipc, sinfo);
    if (retval != IPCOM_SUCCESS)
    {
        IPCOM_LOG1(ERR, "ipcom_shell_run_extcmd :: ipcom_ipc_send() failed, ret = %d", retval);
        ipcom_ipc_free(sinfo);
        goto fail;
     }

    (void)ipcom_ipc_close(&ipc);

#ifdef IP_PORT_OSE5
    if (argc > 0)
    {
        /* OSE5 has process specific sockets -> donate child fd */
        ip_assert(*cmd_pid != 0);
        retval = (Ip_err)efs_donate_fd(*stdio_fd, *cmd_pid);
        if (retval != 0)
        {
            IPCOM_LOG1(ERR, "ipcom_shell_run_extcmd :: efs_donate_fd, ret = %d", retval);
            goto fail;
        }
    }
#endif /* IP_PORT_OSE5 */

    /* Wait for exit message from shell_cmd process. */
    retval = ipcom_ipc_receive(IP_NULL, &sinfo, -1);
    if (retval != IPCOM_SUCCESS)
    {
        IPCOM_LOG1(ERR, "ipcom_shell_run_extcmd :: ipcom_ipc_receive(shell_cmd) failed, ret = %d", retval);
        goto fail;
    }
    ip_assert(argc > 0 || *(Ip_u32 *)sinfo == 1);
    if (*(Ip_u32 *)sinfo == 1)
    {
        *cmd_pid = 0;  /* shell command called ipcom_exit(). */
        (*proc_index)++;
    }
    else
    {
#ifdef IP_PORT_OSE5
        *stdio_fd = efs_receive_fd(0);
        if (*stdio_fd == -1)
        {
            IP_PANIC();
            goto fail;
        }
#endif
#if defined(IP_PORT_OSE) || defined(IP_PORT_OSE5)
        /* Change child socket owner (A must for OSE to work due to poor ipcom_block impl). */
        pid = ipcom_getpid();
        if (ipcom_socketioctl(*stdio_fd, IP_SIOCSPGRP, &pid) < 0)
        {
            IP_PANIC2();
            IPCOM_LOG1(WARNING, "ipcom_shell_run_extcmd :: ipcom_socketioctl(IP_SIOCSPGRP) failed, errno = %d", ipcom_errno);
        }
#endif

    }
    ipcom_ipc_free(sinfo);

    return 1;

 fail:
    if (ipcom_ipc_isopen(&ipc))
        (void)ipcom_ipc_close(&ipc);
    return -1;
}
#endif /*!defined(WRS_IPNET) || IPCOM_USE_SHELL == IPCOM_SHELL_IPCOM */


/*
 *===========================================================================
 *                    ipcom_shell_find_cmd
 *===========================================================================
 * Description:     Search for  a command in the list of available ipcom_shell
 *                  commands
 * Parameters:      name : the command to search for
 * Returns:         pointer to the command or IP_NULL if not found
 *
 */
IP_PUBLIC Ipcom_shell_cmd *
ipcom_shell_find_cmd(const char *name)
{
    Ipcom_shell_cmd *cmd;

    /* First check if present */
    for (cmd = IPCOM_LIST_FIRST(&ipcom_shell_cmd_head); cmd; cmd = IPCOM_LIST_NEXT(&cmd->cmd_list))
        if (ipcom_strcmp(cmd->name, name) == 0)
            break;

    return cmd;
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
IP_PUBLIC Ip_err
ipcom_shell_add_cmd(const char *name, const char *usage,
                    const char *description, Ipcom_shell_cmd_type hook,
                    Ip_s32 priority, Ip_s32 stack_size)
{
    Ipcom_shell_cmd *cmd;
    static Ipcom_once_t  once = IPCOM_ONCE_INIT;
    Ip_err  err;

    err = ipcom_once(&once, ipcom_shellcmd_init, IP_NULL);
    if (err != IPCOM_SUCCESS)
        return err;

    /* First check if present */
    cmd = ipcom_shell_find_cmd(name);
    if (cmd != IP_NULL)
        return IPCOM_ERR_DUPLICATE;      /*!!allow duplicates for overlay. */

    /* Add a new entry to the list */
    cmd = (Ipcom_shell_cmd *)ipcom_malloc(sizeof(Ipcom_shell_cmd));
    if (cmd == IP_NULL)
        return IPCOM_ERR_NO_MEMORY;

    ipcom_memset(cmd, 0, sizeof(Ipcom_shell_cmd));

    ipcom_strncpy(cmd->name, name, sizeof(cmd->name)-1);
    ipcom_strncpy(cmd->usage, usage, sizeof(cmd->usage)-1);
    ipcom_strncpy(cmd->description, description, sizeof(cmd->description)-1);

    cmd->hook = hook;
    if (priority == 0 || priority == IPCOM_SHELL_PRIO_SAME)
       cmd->priority = ipcom_proc_getprio(ipcom_getpid());
    else
       cmd->priority = priority;
    cmd->stack_size = stack_size;

    ipcom_list_insert_last(&ipcom_shell_cmd_head, &cmd->cmd_list);

#if defined(WRS_IPNET) && defined(IP_PORT_VXWORKS) && (IP_PORT_VXWORKS >= 65)
    (void)ipcom_vxshell_add_cmd(name,
                                usage,
                                description,
                                hook,
                                priority,
                                stack_size);
#endif

    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    ipcom_shell_remove_cmd
 *===========================================================================
 * Description:     Remove a command from the list of available ipcom_shell commands
 * Parameters:      name : the command to remove
 * Returns:         IPCOM_SUCCESS : ok
 *                  IPCOM_ERR_NOT_FOUND : the command does not exist
 *
 */
IP_PUBLIC Ip_err
ipcom_shell_remove_cmd(const char *name, Ipcom_shell_cmd_type hook)
{
    Ipcom_shell_cmd *cmd;

    (void)hook;

    /* First check if present */
    cmd = ipcom_shell_find_cmd(name);
    if (cmd == IP_NULL)
        return IPCOM_ERR_NOT_FOUND;

    ipcom_list_remove(&cmd->cmd_list);
    ipcom_free(cmd);

    return IPCOM_SUCCESS;
}

#else
int ipcom_shellcmd_empty_file;
#endif   /* #if IPCOM_USE_SHELL == IPCOM_SHELL_IPCOM || \
            (defined(WRS_IPNET) && IPCOM_USE_SHELL != IPCOM_SHELL_NONE)*/


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
