/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_vxworks.c,v $ $Revision: 1.96.16.1 $
 *     $Source: /home/interpeak/CVSRoot/ipcom/port/vxworks/src/ipcom_vxworks.c,v $
 *     $Author: ulf $ $Date: 2010-07-02 13:20:59 $
 *     $State: Exp $ $Locker:  $
 *
 *     INTERPEAK_COPYRIGHT_STRING
 *     Design and implementation by Roger Boden <roger@interpeak.se>
 ******************************************************************************
 */

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 *
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

#include "ipcom_type.h"
#include "ipcom_cstyle.h"
#include "ipcom_err.h"
#include "ipcom_clib.h"
#include "ipcom_syslog.h"
#include "ipcom_h.h"
#include "ipcom_err.h"
#include "ipcom_os.h"
#include "ipcom_sock2.h"
#include "ipcom_strlib.h"
#include "ipcom_shell.h"
#include "ipcom_vxworks.h"
#include "ipcom_windview.h"
#include "ipcom_ccivalidate.h"

#ifdef IPCOM_VXWORKS_DRV_NULLPOOL
#include <ipcom_slab.h>
#endif

#undef s6_addr /* Remove the ipcom definition, vxWorks 6.0 has a conflicting macro definition */
#undef ifa_dstaddr /* The ifa_dstaddr #define in ipcom_sock2.h breaks code in target/h/.../ifvar.h */
#undef ifa_broadaddr /* The ifa_broadaddr #define in ipcom_sock2.h breaks code in target/h/.../ifvar.h */

#include <vxWorks.h>
#if !defined(WRS_IPNET)
#include <config.h>
#endif
#include <private/rtpLibP.h>
#include <private/taskUtilLibP.h>
#include <taskLib.h>
#include <taskHookLib.h>
#include <time.h>
#include <blkIo.h>
#include <ramDrv.h>
#include <dosFsLib.h>
#include <fcntl.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <endLib.h>
#if IP_PORT_VXWORKS >= 60
#if IP_PORT_VXWORKS >= 62
#include <xbdRamDisk.h>
#endif
#include <cbioLib.h>
#include <ramDiskCbio.h>
#endif
#if defined(IPNET) || defined(IPLITE)
#include <sockLib.h>
#include <sockFunc.h>
#endif

#if defined(IPCOM_USE_VXSEM_INLINE) || defined(IPCOM_USE_VXSEM_SCALABLE)
#include <inline/semLibInline.h>
#define MUTEX_CREATE_OPTS (SEM_MUT_OPTIONS_MASK & IPCOM_VXSEM_MUTEX_OPTS)
#elif defined(IPCOM_USE_VXSEM_MINIMAL)
#include <semMinLib.h>
#define MUTEX_CREATE_OPTS SEM_MIN_MUTEX_CREATE_OPTS
#else
#include <semLib.h>
#define MUTEX_CREATE_OPTS \
    (SEM_INVERSION_SAFE | SEM_Q_PRIORITY | SEM_DELETE_SAFE)
#endif /* IPCOM_USE_VXSEM_INLINE */

#ifdef IPCOM_USE_SMP
#ifdef IPCOM_USE_SPINLOCK_ISR_ND
#include <private/spinlockIsrNdLibP.h>
#else
#include <spinLockLib.h>
#endif
#endif

#if defined(IPCOM_ZEROCOPY)
#include <zbufLib.h>
#include <zbufSockLib.h>
#endif

#include "../../../../InterpeakMntn/include/InterpeakMntn.h"

/*
 ****************************************************************************
 * 4                    EXTERN PROTOTYPES
 ****************************************************************************
 */

#if defined(WRS_IPNET)
IP_EXTERN FUNCPTR vxfs_ramdisk_init_hook;
IP_EXTERN FUNCPTR ipcom_shell_find_cmd_hook;
IP_EXTERN void ipcom_init_config_vars();
#endif

#ifdef IPCOM_VXWORKS_DRV_NULLPOOL
IP_EXTERN Ip_bool ipcom_vxworks_pkt_pool_hdr_ctor(void * unused,
                                                  Ipcom_pkt *pkt);
IP_EXTERN Ipcom_slab_ctor ipnet_pkt_pool_hdr_ctor_func;
#endif

/*
****************************************************************************
* 5                    DEFINES
****************************************************************************
 */

#if (defined (IPCOM_WV_INSTRUMENTATION_TEST) || defined(IPCOM_CCIPERFORMANCE_TEST) || defined(IPCOM_CCIVALIDATE_TEST))
#ifndef INCLUDE_XBD
#define INCLUDE_XBD
#endif
#ifndef INCLUDE_XBD_PART_LIB
#define INCLUDE_XBD_PART_LIB
#endif
#ifndef INCLUDE_XBD_BLK_DEV
#define INCLUDE_XBD_BLK_DEV
#endif
#ifndef INCLUDE_XBD_RAMDRV
#define INCLUDE_XBD_RAMDRV
#endif
#endif /* #ifdef IPCOM_WV_INSTRUMENTATION_TEST */


#if (defined(IPCRYPTO) || defined(IPMCRYPTO)) && defined(IPCCI)
#error Choose_one_of_ip[m]crypto_or_ipcci
#endif


/*
 ****************************************************************************
 * 6                    TYPES
 ****************************************************************************
 */


/*
 ****************************************************************************
 * 7                    LOCAL PROTOTYPES
 ****************************************************************************
 */

#if defined(IPCOM_USE_SOCK_COMPAT) && (defined(IPNET) || defined(IPLITE))
IP_PUBLIC SOCK_FUNC  *ipcom_windnet_socklib_init(void);
#endif


/*
 ****************************************************************************
 * 8                    DATA
 ****************************************************************************
 */

/* Port specific global data. */
IP_GLOBAL IP_FASTDATA Ipcom_port ipcom_port;

#ifdef IPCOM_VXWORKS_DRV_NULLPOOL
IP_GLOBAL void (*ipcom_null_pool_create) (void) = IP_NULL;
#endif

#ifdef IPCOM_USE_SMP
#ifdef IPCOM_USE_SPINLOCK_ISR_ND
IP_GLOBAL spinlockIsrNd_t ipcom_spinlock;  /* global IPCOM spinlock */
#else
IP_GLOBAL spinlockIsr_t ipcom_spinlock;  /* global IPCOM spinlock */
#endif
#endif

IP_STATIC RESERVED_NUM ipcom_rtp_tcb_index;


/*
 ****************************************************************************
 * 9                    FUNCTIONS
 ****************************************************************************
 */


/*
 *===========================================================================
 *                         ipcom_vxworks_init_ram_fs
 *===========================================================================
 */
#if !defined(WRS_IPNET)
#if defined(IPCOM_USE_RAM_DISK) && IPCOM_USE_FILE == IPCOM_FILE_NATIVE
IP_STATIC void
ipcom_vxworks_init_ram_fs(void)
{
#if IP_PORT_VXWORKS >= 60
#if IP_PORT_VXWORKS >= 62
#if !defined(INCLUDE_XBD) || !defined(INCLUDE_XBD_PART_LIB) || !defined(INCLUDE_XBD_BLK_DEV) || !defined(INCLUDE_XBD_RAMDRV)
#error MUST_SET_INCLUDE_XBD_DEFINES
#endif
    device_t	xbd;
    char		path[sizeof(IPCOM_FILE_ROOT) + 4];

    strcpy(path, IPCOM_FILE_ROOT );

    /* Remove the trailing slash */
    if (path[strlen(path)-1] == '/' )
        path[strlen(path)-1] = '\0';

    xbd = xbdRamDiskDevCreate(IPCOM_RAM_DISK_BLOCK_SIZE, (IPCOM_RAM_DISK_NO_BLOCK * IPCOM_RAM_DISK_BLOCK_SIZE), FALSE, path);
    if (xbd == NULLDEV)
    {
        IPCOM_LOG1(ERR, "ipcom_init.c: init_ram_fs, "
                   "xbdRamDiskDevCreate() failed, errno=%d",
                   ipcom_errno);
        return;
    }

    if (dosFsVolFormat (path, DOS_OPT_BLANK, NULL) != IPCOM_SUCCESS )
    {
        IPCOM_LOG1(ERR, "ipcom_init.c: init_ram_fs, "
                   "dosFsVolFormat() failed, errno=%d",
                   ipcom_errno);
        return;
    }
#else /* IP_PORT_VXWORKS >= 62 */
    CBIO_DEV_ID cbio;

    cbio = ramDiskDevCreate(NULL, IPCOM_RAM_DISK_BLOCK_SIZE, 0, IPCOM_RAM_DISK_NO_BLOCK, 0);
    if (!cbio)
    {
        IPCOM_LOG1(ERR, "ipcom_init.c: init_ram_fs, "
                   "ramDiskDevCreate() failed, errno=%d",
                   ipcom_errno);
        return;
    }
    if (dosFsDevCreate(IPCOM_FILE_ROOT, cbio, 4, NONE) == ERROR)
    {
        IPCOM_LOG1(ERR, "ipcom_init.c: init_ram_fs, "
                   "dosFsDevCreate() failed, errno=%d",
                   ipcom_errno);
        return;
    }
    if (dosFsVolFormat(IPCOM_FILE_ROOT, DOS_OPT_BLANK | DOS_OPT_QUIET, NULL) == ERROR )
    {
        IPCOM_LOG1(ERR, "ipcom_init.c: init_ram_fs, "
                   "dosFsVolFormat() failed, errno=%d",
                   ipcom_errno);
        return;
    }
#endif /* IP_PORT_VXWORKS >= 62 */
#else	/* IP_PORT_VXWORKS >= 60 */
    BLK_DEV* p_blk_dev;
    DOS_VOL_DESC* p_vol_desc;

    p_blk_dev = ramDevCreate(NULL, 512, 0, 64, 0);
    if (!p_blk_dev)
    {
        IPCOM_LOG1(ERR, "ipcom_init.c: init_ram_fs, "
                   "ramDevCreate() failed, errno=%d",
                   ipcom_errno);
        return;
    }
    p_vol_desc = dosFsMkfs(IPCOM_FILE_ROOT, p_blk_dev);
    if (!p_vol_desc)
    {
        IPCOM_LOG1(ERR, "ipcom_init.c: init_ram_fs, "
                   "dosFsMkfs() failed, errno=%d",
                   ipcom_errno);
        return;
    }
#endif	/* IP_PORT_VXWORKS >= 60 */
}
#endif /* #if defined(IPCOM_USE_RAM_DISK) && IPCOM_USE_FILE == IPCOM_FILE_NATIVE */
#endif /* #if !defined(WRS_IPNET) */


/*
 *===========================================================================
 *                         ipcom_vxworks_stackcheck
 *===========================================================================
 */
#if defined(IPTESTENGINE) && IP_PORT_VXWORKS >= 65
#include <dbgLib.h>
#include <usrLib.h>

IP_STATIC void
ipcom_vxworks_stackcheck(WIND_TCB *old, WIND_TCB *new)
{
    TASK_DESC task_desc;

    if (taskInfoGet((int)old, &task_desc) != OK)
        IPCOM_LOG0(ERR, "Failed to retrieve task information");
    else if (task_desc.td_options & VX_NO_STACK_FILL)
        IPCOM_LOG0(ERR, "Stack fill not available");
    else
    {
        if (task_desc.td_stackMargin <= 256)
            IPCOM_LOG1(ERR, "Stack overflow in task %s", task_desc.td_name);
        else if (task_desc.td_excStackMargin <= 256)
            IPCOM_LOG1(ERR, "Exception stack overflow in task %s", task_desc.td_name);
        else
            return;

        (void)ti((int)old);
        (void)tt((int)old);
        (void)taskSuspend(0);
    }
}
#endif /* defined(IPTESTENGINE) && IP_PORT_VXWORKS >= 65 */

#if ! defined(IPCOM_VXWORKS_DRV_NULLPOOL)
/*
 *===========================================================================
 *                    ipcom_init_mblk_pool
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipcom_init_mblk_pool(void)
{
    M_CL_CONFIG cl_config;
    CL_DESC cl_desc[] = { { 0, 0, NULL, 0} };

    cl_config.mBlkNum = ipcom_drv_eth_get_no_pkts();
    cl_config.clBlkNum = cl_config.mBlkNum;
    cl_config.memSize =
        (cl_config.mBlkNum*(M_BLK_SZ+sizeof(long)) + cl_config.clBlkNum*CL_BLK_SZ);
    cl_config.memArea = ipcom_malloc(cl_config.memSize);
    if (cl_config.memArea == IP_NULL)
    {
        IPCOM_LOG0(ERR, "ipcom_drv_eth_init_net_buf :: out of memory 1");
        return;
    }

    cl_desc[0].memSize = (cl_desc[0].clSize+sizeof(long))*cl_desc[0].clNum;
    if (cl_desc[0].memSize != 0)
    {
        cl_desc[0].memArea = ipcom_malloc(cl_desc[0].memSize);
        if (cl_desc[0].memArea == IP_NULL)
        {
            IPCOM_LOG0(ERR, "ipcom_drv_eth_init_net_buf :: out of memory 2");
            return;
        }
    }

    if (netPoolInit(&ipcom_port.net_pool, &cl_config, cl_desc, 1, NULL) != OK)
    {
        IPCOM_LOG1(ERR, "ipcom_drv_eth_init_net_buf :: netPoolInit() failed, errno=%d",
                   ipcom_errno);
    }
}
#endif /* ! defined(IPCOM_VXWORKS_DRV_NULLPOOL) */


/*
 *===========================================================================
 *                       ipcom_vxworks_rtp_del_hook
 *===========================================================================
 * Description: RTP delete hook. This hook is run before all resources owned
 *              by the RTP is automatically freed. It is not possible to run
 *              the task delete hook after the RTP is gone since some
 *              resources accessed by ipcom_vxworks_task_del_hook() is gone.
 * Parameters:  rtpId - RTP ID of the RTP about to be deleted
 *              exitCode - exit code or delete status for the RTP
 * Returns:
 *
 */
#ifdef _WRS_CONFIG_RTP
IP_STATIC void
ipcom_vxworks_rtp_del_hook(const RTP_ID rtpId, const int exitCode)
{
    ipcom_vxworks_task_del_hook(taskTcb(taskIdSelf()));
}
#endif /* _WRS_CONFIG_RTP */


/*
*****************************************************************************
 * 10                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipcom_initd
 *===========================================================================
 * Description: ipcom initialization
 */
IPCOM_PROCESS(ipcom_initd)
{
    Ip_err err;

    ipcom_proc_init();

#if defined(WRS_IPNET)
    /* Initialize IPCOM configuration variables */
    ipcom_init_config_vars();
#endif

    /* Create and initialize a ram file system */
#if defined(IPCOM_USE_RAM_DISK) && IPCOM_USE_FILE == IPCOM_FILE_NATIVE
#if defined(WRS_IPNET)
    if (vxfs_ramdisk_init_hook)
        vxfs_ramdisk_init_hook();
#else
    ipcom_vxworks_init_ram_fs();
#endif /* defined(WRS_IPNET) */
#endif /* (IPCOM_USE_RAM_DISK) && IPCOM_USE_FILE == IPCOM_FILE_NATIVE */

    /* Start IPCOM */
    err = ipcom_start();
    if (err != IPCOM_SUCCESS)
    {
        IPCOM_LOG0(ERR, "ipcom_initd :: init END FAIL");
        IP_PANIC();
        goto exit;
    }

    /* Release the usrRoot task */
    semGive(ipcom_port.init_sync);

#if IPCOM_USE_SHELL != IPCOM_SHELL_NONE
#if defined(IPCOM_DRV_ETH_STATS) && defined(IPNET)
    {
        IP_PUBLIC int ipcom_drv_eth_cmd_stats(int argc, char** argv);
        ipcom_shell_add_cmd("drv_stats", "drv_stats", "Output driver statistics",
                            ipcom_drv_eth_cmd_stats,
                            IPCOM_PRIORITY_DEFAULT, IPCOM_PROC_STACK_DEFAULT);
    }
#endif

#if (!defined(WRS_IPNET) && defined(IPCOM_USE_VXSHELL_CMD) && defined(INCLUDE_SHELL))
    {
        IP_GLOBAL int ipcom_cmd_vxshell(int argc, char **argv);
        ipcom_shell_add_cmd("vxshell", "vxshell", "Enter VxWorks target shell",
                            ipcom_cmd_vxshell,
                            IPCOM_PRIORITY_DEFAULT, IPCOM_PROC_STACK_DEFAULT);
    }
#endif

#if defined(IPCOM_WV_INSTRUMENTATION) && defined(IPCOM_WV_INSTRUMENTATION_TEST)
    ipcom_shell_add_cmd("wvnet", "wvnet", "Log network events.",
                        ipcom_cmd_wvnet,
                        IPCOM_PRIORITY_DEFAULT, IPCOM_PROC_STACK_DEFAULT);
#endif

#if defined(IPCOM_CCIVALIDATE_TEST)
    ipcom_shell_add_cmd("ccivalidate", "ccivalidate", "CCI provider validate.",
                        ipcom_cmd_ccivalidate,
                        IPCOM_PRIORITY_DEFAULT, IPCOM_PROC_STACK_MAX);
#endif /* IPCOM_CCIVALIDATE_TEST */

#if defined(IPCOM_CCIPERFORMANCE_TEST)
    ipcom_shell_add_cmd("cciperf", "cciperf", "CCI provider performance benchmark test.",
                        ipcom_cmd_cciperformance,
                        IPCOM_PRIORITY_DEFAULT, IPCOM_PROC_STACK_MAX);
#endif /* IPCOM_CCIPERFORMANCE_TEST */

#endif /* IPCOM_USE_SHELL != IPCOM_SHELL_NONE */

#ifdef IPCOM_ZEROCOPY
    IPCOM_LOG0(DEBUG, "ipcom_initd :: initializing ZeroCopy");
    zbufLibInit( NULL );
    zbufSockLibInit();
#endif

    IPCOM_LOG0(DEBUG, "ipcom_initd :: init END OK");

 exit:
    ipcom_proc_exit();
}


/*
 *===========================================================================
 *                    ipcom_create
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipcom_create(void)
{
    static int initialized = 0;
    Ipcom_proc_attr proc_attr;
    static volatile void *ipcom_vxworks_need_funcs[3];

    /* Check if we have been called before */
    if (initialized == 1)
        return IPCOM_SUCCESS;
    initialized = 1;

#if defined(IPTESTENGINE) && IP_PORT_VXWORKS >= 65
    taskSwitchHookAdd((FUNCPTR)ipcom_vxworks_stackcheck);
#endif

    ipcom_vxworks_need_funcs[0] = ipcom_getaddrinfo;
    ipcom_vxworks_need_funcs[1] = ipcom_freeaddrinfo;
    ipcom_vxworks_need_funcs[2] = ipcom_getservbyname;

#ifdef IPCOM_USE_SMP
    /* Initialize the global ipcom spinlock */
#ifdef IPCOM_USE_SPINLOCK_ISR_ND
    spinlockIsrNdInit(&ipcom_spinlock);
#else
    SPIN_LOCK_ISR_INIT(&ipcom_spinlock, 0);
#endif
#endif /* IPCOM_USE_SMP */

    ipcom_memset(&ipcom_port, 0, sizeof(Ipcom_port));
    ipcom_list_init(&ipcom_port.open_q_head);
    ipcom_port.code_mutex = semMCreate(MUTEX_CREATE_OPTS);
    ipcom_port.init_sync = semCCreate(SEM_Q_PRIORITY, 0);

#ifdef IPCOM_VXWORKS_DRV_NULLPOOL
    /* If required, set up mBlks and clBlks for outgoing packets. */
    if (ipcom_null_pool_create)
    {
        ipcom_port_pkt_hdr_extra_size = (sizeof(Ipcom_port_pkt) -
					 sizeof(Ipcom_pkt));
        ipnet_pkt_pool_hdr_ctor_func =
	    (Ipcom_slab_ctor)ipcom_vxworks_pkt_pool_hdr_ctor;
	ipcom_null_pool_create ();
    }
#else
    ipcom_init_mblk_pool ();
#endif /* IPCOM_VXWORKS_DRV_NULLPOOL */

    (void)ipcom_usr_create();

    /* If we are using WR Sec Libs AND we are building from CVS, then call secLibInit */
#if !defined(IPCRYPTO) && !defined(IPMCRYPTO) && !defined(WRS_IPNET) \
     && (defined(IPSSH) || defined(IPIKE) || defined(IPIPSEC2) || defined(IPEAP))
    {
#include <openssl/evp.h> /* Need to include this file to get all #defines */
        extern void secLibInit(void);
        extern void sslLibInit(void);
        secLibInit();
        OpenSSL_add_all_algorithms();
        SSL_library_init();
    }
#endif

    ipcom_proc_attr_init(&proc_attr);
    proc_attr.stacksize = IPCOM_PROC_STACK_MAX; /*IPCOM_PROC_STACK_DEFAULT;*/
    proc_attr.flags = IPCOM_PROC_FLAG_FP;
    if (ipcom_proc_acreate("ipcom_initd", ipcom_initd, &proc_attr, IP_NULL) != IPCOM_SUCCESS)
    {
        IPCOM_LOG0(ERR, "ipcom_create :: ipcom_proc_acreate failed");
        return IPCOM_ERR_FAILED;
    }

    /* Wait for the init process to finish */
    semTake(ipcom_port.init_sync, WAIT_FOREVER);
    semDelete(ipcom_port.init_sync);
    ipcom_port.init_sync = IP_NULL;

    taskDeleteHookAdd((FUNCPTR)ipcom_vxworks_task_del_hook);
#ifdef _WRS_CONFIG_RTP
    {
        extern void ipcom_vxworks_rtp_del_hook_add(RTP_DELETE_HOOK hook, BOOL to_head);
        ipcom_vxworks_rtp_del_hook_add(ipcom_vxworks_rtp_del_hook, TRUE);

#ifndef RESERVE_OLDSTYLE
        taskReservedNumAllot(taskIdSelf(), &ipcom_rtp_tcb_index);
        if (ipcom_rtp_tcb_index == ERROR)
            IP_PANIC();
#endif
    }
#endif


    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    ipcom_rtp_tcb_index_get_fd
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL int
ipcom_rtp_tcb_index_get_fd(void)
{
    return ipcom_rtp_tcb_index;
}


/*
 *===========================================================================
 *                    ipcom_run_cmd
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
#if IPCOM_USE_SHELL != IPCOM_SHELL_NONE
IP_PUBLIC int
ipcom_run_cmd(char *cmd_str)
{
    Ipcom_shell_cmd *sh_cmd = IP_NULL;
    int argc;
    char** argv = NULL;
    int rc = IPCOM_ERR_FAILED;
    FUNCPTR ipcom_shell_find_cmd_func;
    IPS_MNTN_CCORE_CAPTURE_CB            pCaptureCb;
    IPS_MNTN_CCORE_CAPTURE_PARA_STRU     stIpsMntnCapturePara;

    pCaptureCb  = g_astCallbackTbl[IPS_MNTN_CCORE_APP_CMD_CB];

    stIpsMntnCapturePara.pucData          = cmd_str;
    stIpsMntnCapturePara.usLen            = ipcom_strlen(cmd_str);

    if ( NULL != pCaptureCb )
    {
        pCaptureCb(&stIpsMntnCapturePara);
    }

#if defined(WRS_IPNET)
    if (ipcom_shell_find_cmd_hook)
        ipcom_shell_find_cmd_func = ipcom_shell_find_cmd_hook;
    else
    {
        printf("ipcom_shell_cmd not included.");
	    return rc;
    }
#else
    ipcom_shell_find_cmd_func = (FUNCPTR)ipcom_shell_find_cmd;
#endif
    /* Copy the command string, since ipcom_parse_argstr() modifies cmd_str. */

    if (IP_NULL == cmd_str ||
        IP_NULL == (cmd_str = ipcom_strdup(cmd_str)))
	return rc;

    switch (ipcom_parse_argstr(cmd_str, &argc, &argv))
    {
    case IPCOM_SUCCESS:
        break;
    case IPCOM_ERR_INVALID_ARG:
        printf("Badly matched string escape characters"IP_LF);
        goto out;
    case IPCOM_ERR_FAILED:
    default:
        printf("Failed to parse command string, internal error"IP_LF);
        goto out;
    }

    sh_cmd = (Ipcom_shell_cmd *)ipcom_shell_find_cmd_func(argv[0]);
    if (sh_cmd)
    {
        sh_cmd->hook(argc, argv);
        rc = IPCOM_SUCCESS;
    }
    else
    {
        printf("Unknown command: %s"IP_LF, argv[0]);
        rc = IPCOM_ERR_FAILED;
    }

    ipcom_free(argv);

out:
    /* We assume commands maintain no references to their argv string arguments
     * which persist after the commands return! I.e., the commands are assumed
     * to make copies if they need such references.
     */
    ipcom_free(cmd_str);

    return rc;
}


#endif /* IPCOM_USE_SHELL == IPCOM_SHELL_IPCOM */


/*
 *===========================================================================
 *                    ipcom_is_stack_task
 *===========================================================================
 * Description: Returns if the current task is one of the network tasks.
 * Parameters:
 * Returns:     IP_TRUE if the current task is a network task.
 *
 */
IP_PUBLIC Ip_bool
ipcom_is_stack_task(void)
{
    WIND_TCB   *pTcb = taskTcb(0);
    Ipcom_proc *proc;
    return (proc = pTcb->pIpcomInfo) != NULL
        && proc->pproc->is_stack_task;
}


/*
 *===========================================================================
 *                    ipcom_ima_stack_task
 *===========================================================================
 * Description: Declare that the current task is one of the network tasks
 * Parameters:
 * Returns:     IPCOM_SUCCESS unless an Ipcom_proc structure could not be
 *              allocated.
 *
 */
IP_PUBLIC Ip_err
ipcom_ima_stack_task(void)
{
    Ipcom_proc * proc = ipcom_proc_self ();

    if (proc == IP_NULL)
    {
        IPCOM_LOG0(ERR, "ipcom_ima_stack_task :: failed");
	return IPCOM_ERR_FAILED;
    }

    proc->pproc->is_stack_task = IP_TRUE;
    return IPCOM_SUCCESS;
}

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

