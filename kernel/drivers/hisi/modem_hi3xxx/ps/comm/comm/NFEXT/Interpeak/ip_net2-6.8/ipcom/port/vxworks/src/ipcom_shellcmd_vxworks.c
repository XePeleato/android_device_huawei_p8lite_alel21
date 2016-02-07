/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_shellcmd_vxworks.c,v $ $Revision: 1.7 $
 *     $Source: /home/interpeak/CVSRoot/ipcom/port/vxworks/src/ipcom_shellcmd_vxworks.c,v $
 *     $Author: rime $
 *     $State: Exp $ $Locker:  $
 *
 *     INTERPEAK_COPYRIGHT_STRING
 *     Design and implementation by Lennart Bang <lennart.bang@windriver.com>
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

/* OS */
#include <vxWorks.h>
#include <stdlib.h>
#include <string.h>
#include <shellInterpCmdLib.h>


/* Ipcom include files */
#include "ipcom_clib.h"
#include "ipcom_err.h"
#include "ipcom_os.h"
#include "ipcom_shell.h"
#include "ipcom_syslog.h"


/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

#define IPCOM_SYSLOG_PRIORITY    IPCOM_SYSLOG_IPCOM_PRIORITY
#define IPCOM_SYSLOG_FACILITY    IPCOM_LOG_IPCOM

#ifdef ipcom_shell_add_cmd
#undef ipcom_shell_add_cmd
#endif

#ifdef ipcom_shell_remove_cmd
#undef ipcom_shell_remove_cmd
#endif


/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

#if defined(WRS_IPNET)
typedef STATUS (*vxshell_add_cmd_type)(char *, SHELL_CMD  *cmd);
#endif


/*
 ****************************************************************************
 * 6                    EXTERN PROTOTYPES
 ****************************************************************************
 */

#if defined(WRS_IPNET)
IP_EXTERN vxshell_add_cmd_type ipcom_vxshell_add_cmd_hook;
#endif


/*
 ****************************************************************************
 * 7                    LOCAL PROTOTYPES
 ****************************************************************************
 */

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
 ****************************************************************************
 * 10                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

#if defined(WRS_IPNET)
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
 *
 */
IP_PUBLIC Ip_err
ipcom_vxshell_add_cmd(const char *name, const char *usage,
                      const char *description, Ipcom_shell_cmd_type hook,
                      Ip_s32 priority, Ip_s32 stack_size)
{
    STATUS status;
    SHELL_CMD  *cmd;

    (void)priority;
    (void)stack_size;

    if (ipcom_vxshell_add_cmd_hook == NULL)
    {
        IPCOM_LOG1(WARNING, "ipcom_shell_add_cmd() :: INCLUDE_SHELL component not included %s",
                   ".");
        return IPCOM_ERR_FAILED;
    }

    /* Note: must malloc the SHELL_CMD structure */
    cmd = (SHELL_CMD *)malloc(sizeof(SHELL_CMD));
    if (cmd == NULL)
    {
        IPCOM_LOG1(WARNING, "ipcom_shell_add_cmd() :: failed to add '%' command, out of memory.",
                   name);
        return IPCOM_ERR_NO_MEMORY;
    }

    cmd->name = name;
    cmd->func = (FUNCPTR)hook;
    cmd->opt  = IP_NULL; /* use argc & argv function */
    cmd->shortDesc = description;
    cmd->fullDesc  = description;
    cmd->synopsis  = usage;

    /* Add shell command */
    status = ipcom_vxshell_add_cmd_hook("network", cmd);
    if (status != OK)
    {
        IPCOM_LOG2(WARNING, "ipcom_shell_add_cmd() :: CmdAdd failed for %s : status = %d",
                   name, status);
        return IPCOM_ERR_DUPLICATE;
    }

    return IPCOM_SUCCESS;
}


#else /* !defined(WRS_IPNET) */
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
 *
 */
IP_PUBLIC Ip_err
ipcom_shell_add_cmd(const char *name, const char *usage,
                    const char *description, Ipcom_shell_cmd_type hook,
                    Ip_s32 priority, Ip_s32 stack_size)
{
    STATUS status;
    SHELL_CMD  *cmd;

    (void)priority;
    (void)stack_size;

    /* Note: must malloc the SHELL_CMD structure */
    cmd = (SHELL_CMD *)malloc(sizeof(SHELL_CMD));
    if (cmd == NULL)
    {
        IPCOM_LOG1(WARNING, "ipcom_shell_add_cmd() :: failed to add '%' command, out of memory.",
                   name);
        return IPCOM_ERR_NO_MEMORY;
    }

    cmd->name = name;
    cmd->func = (FUNCPTR)hook;
    cmd->opt  = IP_NULL; /* use argc & argv function */
    cmd->shortDesc = description;
    cmd->fullDesc  = description;
    cmd->synopsis  = usage;

    /* Add topic in case it does not exist */
    status = shellCmdTopicAdd("network", "Networking routines");
    if (status != OK)
    {
        IPCOM_LOG1(WARNING, "ipcom_shell_add_cmd() :: CmdTopicAdd(network) failed : status = %d",
                   status);
        return IPCOM_ERR_FAILED;
    }

    /* Add shell command */
    status = shellCmdAdd("network", cmd);
    if (status != OK)
    {
        IPCOM_LOG2(WARNING, "ipcom_shell_add_cmd() :: CmdAdd failed for %s : status = %d",
                   name, status);
        return IPCOM_ERR_DUPLICATE;
    }

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
    (void)name;
    (void)hook;

    /*!! Note: vxWorks shell does not support removing a shell command */

    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    ipcom_shell_find_cmd
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ipcom_shell_cmd*
ipcom_shell_find_cmd(const char *name)
{
    (void)name;

    return IP_NULL;
}
#endif /* defined(WRS_IPNET) */


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
