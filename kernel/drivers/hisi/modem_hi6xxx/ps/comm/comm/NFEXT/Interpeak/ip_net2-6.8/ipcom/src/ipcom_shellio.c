/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_shellio.c,v $ $Revision: 1.36 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/src/ipcom_shellio.c,v $
 *   $Author: markus $
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


/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#if IPCOM_USE_SHELL == IPCOM_SHELL_IPCOM

#define IPCOM_USE_CLIB_PROTO
#include "ipcom_type.h"
#include "ipcom_cstyle.h"
#include "ipcom_clib.h"
#include "ipcom_clib2.h"
#include "ipcom_file.h"
#include "ipcom_os.h"
#include "ipcom_sock.h"
#include "ipcom_err.h"
#include "ipcom_shell.h"
#include "ipcom_syslog.h"


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
 ****************************************************************************
 * 6                    EXTERN PROTOTYPES
 ****************************************************************************
 */

#ifdef IP_PORT_UNIX
IP_GLOBAL Ip_size_t ipcom_unix_stdout_write(IP_CONST void *buf, Ip_size_t count);
IP_GLOBAL Ip_size_t ipcom_unix_stdin_read(void *buf, Ip_size_t buf_len);
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
 * 10                    PUBLIC FUNCTIONS
 ****************************************************************************
 */


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
    Ipcom_proc  *proc;
    Ip_ssize_t   ret;

    (void) fd;
    ip_assert((IP_FILE *)(Ip_ptrdiff_t)fd == IP_FILE_STDOUT
              || (IP_FILE *) (Ip_ptrdiff_t)fd == IP_FILE_STDERR);

    if (count == 0)
        return 0;

    proc = ipcom_proc_self();
    if (proc && proc->shell_fd != IP_INVALID_SOCKET)
        ret = ipcom_socketwrite(proc->shell_fd, buf, count);
    else
    {
#if defined(IP_PORT_INTEGRITY) || defined (IP_PORT_VXWORKS)
        /*!!FIXME--Don't assume null-termination */
        ipcom_debug_printf("%s", buf);
        return count;
#elif defined(IP_PORT_UNIX)
        return ipcom_unix_stdout_write(buf, count);
#else
        ipcom_errno = IP_ERRNO_EBADF;
        ret = -1;
#endif
    }
    return ret;
}


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
ipcom_shell_fwrite(IP_CONST void *ptr, Ip_size_t size,
                   Ip_size_t nitems, IP_FILE *stream)
{
    Ipcom_proc  *proc;
    Ip_ssize_t ret = 0;

    (void)stream;
#if defined(IP_PORT_INTEGRITY) || defined (IP_PORT_VXWORKS) || defined (IP_PORT_UNIX)
    /* FIXME--route all boot printouts to debug_printf */
    if (stream != IP_NULL && (stream == ip_stdout || stream == ip_stderr))
    {
        proc = ipcom_proc_self();
        if (proc != IP_NULL && proc->shell_fd != IP_INVALID_SOCKET)
        {
            goto ok;
        }
    }

#ifdef IP_PORT_UNIX
    return ipcom_shell_write((Ip_fd) IP_FILE_STDOUT, ptr, size * nitems);
#else
    /* FIXME--Don't assume null-termination */
    ipcom_debug_printf("%s", ptr);
    return nitems;
#endif

ok:
#endif
    if (stream == IP_NULL)
    {
        ipcom_debug_printf("%s", ptr);
        return nitems;
    }

    ip_assert(stream != IP_NULL);
    ip_assert(stream == ip_stdout || stream == ip_stderr);

    if(nitems == 0)
        return 0;

    proc = ipcom_proc_self();
    if (proc != IP_NULL && proc->shell_fd != IP_INVALID_SOCKET)
    {
        ret = ipcom_socketwrite(proc->shell_fd, ptr, size * nitems);
        ret = (ret < 0) ? 0 : ret / (Ip_ssize_t)size;
    }
    else
    {
        ipcom_errno = IP_ERRNO_EBADF;
    }
    return (Ip_size_t)ret;
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
    Ipcom_proc  *proc;
    Ip_ssize_t   ret = 0;

    (void)stream;
#ifdef IP_PORT_UNIX
    if (stream == IP_NULL)
        return ipcom_unix_stdin_read(ptr, size * nitems);
#endif
    ip_assert(stream);
    ip_assert(stream == ip_stdin);

    proc = ipcom_proc_self();
    if (proc && proc->shell_fd != IP_INVALID_SOCKET)
    {
        ret = ipcom_socketread(proc->shell_fd, ptr, size * nitems);
        ret = (ret < 0) ? 0 : ret / (Ip_ssize_t)size;
    }
    else
        ipcom_errno = IP_ERRNO_EBADF;

    return (Ip_size_t)ret;
}


/*
 *===========================================================================
 *                    ipcom_shell_exit
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_shell_exit(int status)
{
    Ipcom_proc  *proc;
    char procname[40];
    Ip_u32  *einfo;
    Ipcom_ipc  ipc;
    Ip_err retval;
    Ip_bool ipc_opened = IP_FALSE;

    proc = ipcom_proc_self();
    if (proc == IP_NULL)
    {
        IP_PANIC();
        goto fail;
    }

    /* Non-SHELL process calls ipcom_exit(). */
    if (proc->shell_ppid == 0)
        goto leave;

    /* Prevent socket to telnet client from being closed by ipcom_proc_exit(). */
    proc->shell_fd = IP_INVALID_SOCKET;

    /* Open IPC with ipcom_shell. */
    ipcom_sprintf(procname, "ipcom_shell_%lx_%lx", (Ip_u32)proc->shell_ppid, proc->shell_seqno);
    retval = ipcom_ipc_open(&ipc, procname, -1);
    if (retval != IPCOM_SUCCESS)
    {
        IP_PANIC();
        goto fail;
    }
    ipc_opened = IP_TRUE;

    /* Send a message to ipcom_shell. */
    einfo = ipcom_ipc_malloc(8);
    if (einfo == IP_NULL)
    {
        IP_PANIC();
        goto fail;
    }

    /* Send the IPC info. */
    einfo[0] = 1;
    einfo[1] = (Ip_u32)status; /*lint !e571 */
    retval = ipcom_ipc_send(&ipc, einfo);
    if (retval != IPCOM_SUCCESS)
    {
        ipcom_ipc_free(einfo);
        IP_PANIC();
    }

 fail:
    IPCOM_LOG1(DEBUG2, "ipcom_shell_cmd :: terminating(%x)", proc->pid);
    if (ipc_opened)
        (void)ipcom_ipc_close(&ipc);
 leave:
    /* note: ports must now accept an ipc_uninstall call without asserts. */
    (void)ipcom_ipc_uninstall();
    ipcom_proc_exit();
}

#else
int ipcom_shellio_empty_file;
#endif   /* #if IPCOM_USE_SHELL == IPCOM_SHELL_IPCOM */


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

