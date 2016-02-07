/*
 ******************************************************************************
 *                     HEADER FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_vxworks.h,v $ $Revision: 1.28.46.1 $
 *     $Source: /home/interpeak/CVSRoot/ipcom/port/vxworks/include/ipcom_vxworks.h,v $
 *     $Author: ulf $ $Date: 2010-07-02 13:20:57 $
 *     $State: Exp $ $Locker:  $
 *
 *     INTERPEAK_COPYRIGHT_STRING
 *     Design and implementation by Roger Boden <roger@interpeak.se>
 ******************************************************************************
 */
#ifndef IPCOM_VXWORKS_H
#define IPCOM_VXWORKS_H

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

/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#include "ipcom_config.h"
#include "ipcom_cstyle.h"
#include "ipcom_hash.h"
#include "ipcom_list.h"
#include "ipcom_os.h"
#include "ipcom_type.h"
#ifdef IPCOM_USE_SPINLOCK_DEADLOCK_DETECTION
#include "ipcom_atomic.h"
#endif

#include "ipcom_pkt.h"

#ifdef IPCOM_SOCK
#include <vxWorks.h>
#include <vwModNum.h>
#include <selectLib.h>
#include <iosLib.h>
#endif

#ifdef _WRS_KERNEL
#include <netBufLib.h>
#endif

#include <taskLib.h>

#if !defined(_WRS_KERNEL) && !defined(IPCOM_KERNEL)
#include <private/taskLibP.h>
#endif


#ifdef __cplusplus
extern "C" {
#endif

/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */
#define IPCOM_DRV_MAX_IFS 32

/*
 * Convert a non-zero Ip_err value to an errno value for use in backwards
 * compatibility wrappers, where returning the Ip_err value directly may
 * not be an option.
 */
#define IPCOM_ERRNO(x) (M_ipcom | ((x) & 0xffff))

/*
 * This section duplicates error number information from ipcom_err.h.
 * These codes are used for backwards compatibility wrapper functions
 * which store IPCOM_ERRNO(x) values in errno when an underlying ipcom
 * function returns an error value x.
 *
 * If ipcom_err.h is changed or extended, it may be necessary to reflect
 * that change here.  If this becomes burdensome, it may be desirable to
 * look into a method for automatically generating the contents of this
 * section from those files.
 *
 * We don't actually use the IPCOM_ERRNO macro in these definitions
 * since the makeStatTbl script doesn't do macro expansions. M_ipcom
 * itself is defined in vwModNum.h.
 * Note, we don't need errno constants for ipcom_perrno.h, as those
 * are supposed to match those from errno.h.
 */

/* See ipcom_err.h for error descriptions */
#define S_ipcom_ERR_FAILED              (M_ipcom | (0xffff & -1000))
#define S_ipcom_ERR_NO_MEMORY           (M_ipcom | (0xffff & -1010))
#define S_ipcom_ERR_NOT_FOUND           (M_ipcom | (0xffff & -1011))
#define S_ipcom_ERR_NOT_SUPPORTED       (M_ipcom | (0xffff & -1012))
#define S_ipcom_ERR_PERMISSION_DENIED   (M_ipcom | (0xffff & -1013))
#define S_ipcom_ERR_ALREADY_OPEN        (M_ipcom | (0xffff & -1040))
#define S_ipcom_ERR_ALREADY_CLOSED      (M_ipcom | (0xffff & -1041))
#define S_ipcom_ERR_ALREADY_INSTALLED   (M_ipcom | (0xffff & -1042))
#define S_ipcom_ERR_ALREADY_STARTED     (M_ipcom | (0xffff & -1043))
#define S_ipcom_ERR_NOT_OPENED          (M_ipcom | (0xffff & -1050))
#define S_ipcom_ERR_NOT_CLOSED          (M_ipcom | (0xffff & -1051))
#define S_ipcom_ERR_NOT_INSTALLED       (M_ipcom | (0xffff & -1052))
#define S_ipcom_ERR_NOT_STARTED         (M_ipcom | (0xffff & -1053))
#define S_ipcom_ERR_INVALID_ARG         (M_ipcom | (0xffff & -1060))
#define S_ipcom_ERR_INVALID_CONFIG      (M_ipcom | (0xffff & -1061))
#define S_ipcom_ERR_INVALID_VERSION     (M_ipcom | (0xffff & -1062))
#define S_ipcom_ERR_INVALID_ALIGNMENT   (M_ipcom | (0xffff & -1063))
#define S_ipcom_ERR_INVALID_INTERFACE   (M_ipcom | (0xffff & -1064))
#define S_ipcom_ERR_NO_ROUTE            (M_ipcom | (0xffff & -1070))
#define S_ipcom_ERR_DEAD_ROUTE          (M_ipcom | (0xffff & -1071))
#define S_ipcom_ERR_INVALID_ROUTE       (M_ipcom | (0xffff & -1072))
#define S_ipcom_ERR_INVALID_MASK        (M_ipcom | (0xffff & -1073))
#define S_ipcom_ERR_INVALID_RTCLIENT    (M_ipcom | (0xffff & -1074))
#define S_ipcom_ERR_DUPLICATE           (M_ipcom | (0xffff & -1080))
#define S_ipcom_ERR_LOCKED              (M_ipcom | (0xffff & -1081))
#define S_ipcom_ERR_END                 (M_ipcom | (0xffff & -1082))
#define S_ipcom_ERR_READONLY            (M_ipcom | (0xffff & -1083))
#define S_ipcom_ERR_INVALID_TAG_VALUE   (M_ipcom | (0xffff & -1090))
#define S_ipcom_ERR_UNSUPPORTED_TAG     (M_ipcom | (0xffff & -1091))
#define S_ipcom_ERR_INVALID_PACKET      (M_ipcom | (0xffff & -1100))
#define S_ipcom_ERR_BAD_PACKET          (M_ipcom | (0xffff & -1101))
#define S_ipcom_ERR_IGNORED_PACKET      (M_ipcom | (0xffff & -1102))
#define S_ipcom_ERR_SOCKCALLFAILED      (M_ipcom | (0xffff & -1110))
#define S_ipcom_ERR_REACHED_MIN         (M_ipcom | (0xffff & -1111))
#define S_ipcom_ERR_REACHED_MAX         (M_ipcom | (0xffff & -1112))
#define S_ipcom_ERR_TIMEOUT             (M_ipcom | (0xffff & -1120))
#define S_ipcom_ERR_WOULDBLOCK          (M_ipcom | (0xffff & -1121))
#define S_ipcom_ERR_INTERRUPT           (M_ipcom | (0xffff & -1122))
#define S_ipcom_ERR_PENDING             (M_ipcom | (0xffff & -1123))
#define S_ipcom_ERR_AUTH_FAILED         (M_ipcom | (0xffff & -1130))
#define S_ipcom_ERR_AUTH_UNKNOWN        (M_ipcom | (0xffff & -1131))

/* From ipcom_errno.h */
#define S_ipcom_ENOMEDIUM           (M_ipcom | 123)
#define S_ipcom_EMEDIUMTYPE         (M_ipcom | 124)
#define S_ipcom_ENODATA             (M_ipcom | 200)
#define S_ipcom_ECOMM               (M_ipcom | 201)

#define S_ipcom_EINTERNAL	    (M_ipcom | 1000)
#define S_ipcom_ENOSUPPORT          (M_ipcom | 1001)
#define S_ipcom_EINITFAILED         (M_ipcom | 1002)
#define S_ipcom_ECONFIG             (M_ipcom | 1003)
#define S_ipcom_ENOTOME             (M_ipcom | 1004)
#define S_ipcom_ENOTFOUND           (M_ipcom | 1005)
#define S_ipcom_EFIREWALL           (M_ipcom | 1006)


/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

/*
 *===========================================================================
 *                         Ipcom_pproc
 *===========================================================================
 * Process specific data, member of Ipcom_proc struct.
 */
typedef struct Ipcom_pproc_struct
{
    int         errcode;     /* process specific 'errno' */

    /* IPC related data */
    Ip_bool     ipc_installed;
    Ipcom_list  ipc_in_queue;
    void       *in_queue_sem;
    Ip_bool     is_stack_task;
#ifdef IPCOM_USE_SPINLOCK_DEADLOCK_DETECTION
    Ipcom_atomic_t spinlocks_held;
#endif
}
Ipcom_pproc;


#ifdef IPCOM_SOCK
/*
 *===========================================================================
 *                         Ipcom_drv_eth_dev_hdr
 *===========================================================================
 */
typedef struct Ipcom_drv_eth_dev_hdr_struct
{
    DEV_HDR            dev_hdr;
}
Ipcom_drv_eth_dev_hdr;
#endif


#ifdef IPCOM_DRV_ETH_STATS
struct Ipcom_drv_eth_stats_st
{
    /* Statistics */
    unsigned int     recv_mbufs;
    unsigned int     recv_pkts;
    unsigned int     recv_lost;
    unsigned int     recv_lost_pkt_create;
    unsigned int     recv_strange_buf;
    unsigned int     recv_pkt_malloc_fail;
    unsigned int     send_pkts;
    unsigned int     send_pkt_blocked;
    unsigned int     send_pkt_err;
    unsigned int     send_pkt_err_unexp;
    unsigned int     send_out_of_mbufs;
    unsigned int     send_out_of_clblk;
    unsigned int     restart;
    unsigned int     error_end;
    unsigned int     free_out_pkt;
    unsigned int     free_out_pkt_nop;
};
typedef struct Ipcom_drv_eth_stats_st Ipcom_drv_eth_stats;
#define IPCOM_DRV_ETH_STATS_LINE(xvar) ipcom_port.eth_stats.xvar

#else

#define IPCOM_DRV_ETH_STATS_LINE(xvar) IP_NOOP

#endif /* IPCOM_DRV_ETH_STATS */


#ifdef IPCOM_VXWORKS_STATS

#define IPCOM_STATS(xvar)      ipcom_port.stats.xvar

typedef struct Ipcom_vxworks_stats_struct
{
    Ip_u32 task_del_hook;
    Ip_u32 task_del_hook_proc_found;
    Ip_u32 ipc_open;
    Ip_u32 ipc_open_before_install;
    Ip_u32 ipc_close;
    Ip_u32 ipc_install;
    Ip_u32 ipc_uninstall;
    Ip_u32 ipc_send;
    Ip_u32 ipc_receive;
}
Ipcom_vxworks_stats;

#else

#define IPCOM_STATS(xvar)      IP_NOOP

#endif /* IPCOM_VXWORKS_STATS */

struct Ipcom_drv_if_map_st
{
    const char* mux_dev_name;
    const char* ip_if_name;
};
typedef struct Ipcom_drv_if_map_st Ipcom_drv_if_map;

#ifdef _WRS_KERNEL
/*
 *===========================================================================
 *                         Ipcom_port
 *===========================================================================
 * Port specific global IPCOM data.
 */
typedef struct Ipcom_port_struct
{
    Ipcom_list    open_q_head;   /* IPC queue. */
    Ipcom_mutex   code_mutex;    /* Protects IPCOM global data */
    Ipcom_sem     init_sync;      /* Syncronizes initialization of ipcom */

#ifdef IPCOM_SOCK
#ifdef IPCOM_VXWORKS_DRV_NULLPOOL
    NET_POOL                    *net_pool;
#else
    NET_POOL                     net_pool;
#endif
#ifdef IPCOM_USE_SOCK_COMPAT
    Ipcom_drv_eth_dev_hdr        dev_hdr;
    Ipcom_drv_if_map             if_map[IPCOM_DRV_MAX_IFS];
#if 0
    Ipcom_hash                  *select_wakeup_lists;
    Ipcom_mutex                  select_wakeup_lists_mutex;
#endif
#endif
#endif /* IPCOM_SOCK */

#ifdef IPCOM_VXWORKS_STATS
    Ipcom_vxworks_stats stats;
#endif
#ifdef IPCOM_DRV_ETH_STATS
    Ipcom_drv_eth_stats eth_stats;
#endif
}
Ipcom_port;

IP_EXTERN IP_FASTDATA Ipcom_port ipcom_port;

/*
 *===========================================================================
 *                         Ipcom_port_pkt
 *===========================================================================
 * Port specific Ipcom_pkt extension.
 */

typedef struct Ipcom_port_pkt_struct
{
    Ipcom_pkt    pkt;

    /*
     * These two members must be last. They are not actually present
     * when MUX M_BLK support is not needed in the image.  That's an
     * image-build-time choice, not an archive-build-time choice, so
     * there isn't any #ifdef ... #endif here for these members.
     */
    NET_POOL_ID  poolId;  /* Hidden NET_POOL_ID preceeding M_LINK */
    M_LINK       ml;
}
Ipcom_port_pkt;

#endif /* _WRS_KERNEL */

/*
 ****************************************************************************
 * 6                    FUNCTIONS
 ****************************************************************************
 */

IP_GLOBAL void ipcom_vxworks_task_del_hook(WIND_TCB* tcb);
IP_PUBLIC Ipcom_proc *ipcom_proc_find(Ip_pid_t pid, Ip_bool create);
IP_GLOBAL Ip_err ipcom_vxworks_proc_free(Ipcom_proc* proc);
IP_GLOBAL void ipcom_register_dev_name_mapping(const char* mux_dev_name,
                                               const char* ip_if_name);
IP_GLOBAL const char* ipcom_get_mux_dev_name(const char* ip_if_name);
IP_GLOBAL const char* ipcom_get_ip_if_name(const char* mux_dev_name);
IP_GLOBAL Ip_err ipcom_drv_eth_init(const char *drvname, Ip_u32 drvunit,
                                    const char *ifname);
IP_GLOBAL int ipcom_rtp_tcb_index_get_fd(void);


#ifdef __cplusplus
}
#endif

#endif

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
