/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_cmd_vxshell.c,v $ $Revision: 1.6 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/port/vxworks/src/ipcom_cmd_vxshell.c,v $
 *   $Author: rboden $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by FirstName LastName <email@interpeak.se>
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


/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#if IPCOM_USE_SHELL == IPCOM_SHELL_IPCOM && defined(IPCOM_USE_VXSHELL_CMD)

#include <ipcom_type.h>
#include <ipcom_cstyle.h>
#include <ipcom_os.h>
#include <ipcom_vxworks.h>

#include <vxWorks.h>
#include <shellLib.h>
#include <wrn/coreip/telnetLib.h>


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

IP_EXTERN int ipcom_windnet_socket_use_existing(Ip_fd fd);


/*
 ****************************************************************************
 * 7                    LOCAL PROTOTYPES
 ****************************************************************************
 */

IP_GLOBAL int ipcom_cmd_vxshell(int argc, char **argv);
IP_STATIC void ipcom_vxworks_shell_logout_cb(void *arg);


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
 *                    ipcom_vxworks_shell_logout_cb
 *===========================================================================
 * Description: Called when VxWorks shell exits.
 * Parameters:  Id of VxWorks shell session
 * Returns:     -
 *
 */
IP_STATIC void
ipcom_vxworks_shell_logout_cb(void *arg)
{
    Ipcom_vxshell_session_t *session;

    session = (Ipcom_vxshell_session_t *) arg;

    ipcom_sem_post(session->shell_exit_sem);
}


/*
 ****************************************************************************
 * 10                   GLOBAL FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipcom_enter_vxworks_shell
 *===========================================================================
 * Description: IP shell command to enter the VxWorks shell. The command blocks
 *              until the VxWorks shell exits
 * Parameters:  Command and arguments (not used)
 * Returns:     0
 *
 */
IP_GLOBAL int
ipcom_cmd_vxshell(int argc, char **argv)
{
    int fd;
    Ipcom_proc *proc;
    Ipcom_vxshell_session_t session;

    (void)argc;
    (void)argv;

    proc = ipcom_proc_find(ipcom_getpid(), IP_FALSE);

#ifdef IPCOM_USE_NATIVE_SOCK_API
    fd = proc->shell_fd;
#else
    fd = ipcom_windnet_socket_use_existing(proc->shell_fd);
#endif

    ipcom_sem_create(&session.shell_exit_sem, 0);

    if (shellParserControl(REMOTE_START, (Ip_u32)&session, fd, ipcom_vxworks_shell_logout_cb) == 0)
    {
        ipcom_sem_wait(session.shell_exit_sem);
        shellParserControl(REMOTE_STOP, (Ip_u32)&session, 0, IP_NULL);
    }

    ipcom_sem_delete(&session.shell_exit_sem);

#ifndef IPCOM_USE_NATIVE_SOCK_API
    close(fd);
#endif

    return 0;
}


/*
 ****************************************************************************
 * 11                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

#else
int ipcom_cmd_vxshell_empty_file;
#endif /* IPCOM_USE_SHELL == IPCOM_SHELL_IPCOM && defined(IPCOM_USE_VXSHELL_CMD) */


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

