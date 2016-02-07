/*
 ******************************************************************************
 *                     INTERPEAK EXAMPLE SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipppp_example.c,v $ $Revision: 1.4 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/port/vxworks/osconfig/vxworks/src/ipnet/Attic/ipppp_example.c,v $
 *   $Author: tneale $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Lennart Bang <lob@interpeak.se>
 ******************************************************************************
 */

/*
modification history
--------------------
01e,22jan07,kch  Modified ipppp_action_default_callback() to retrieve PPP
                 username and password from sysvars (WIND00085576).
01d,03jan07,kch  Updated CVS revision log entry for Jan 03 code drop.
01c,12dec06,kch  Added PPP_INSTALL_CALLBACK_HOOK.
01b,11dec06,kch  Modified ipppp_example_action_cb() to always return
                 IPCOM_SUCCESS if ipppp or ipppoe is not included.
01a,20oct06,kch  Integrated to VxWorks build infrastructure.
*/


/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 * Example file of PPP action callback.
 *
 * If no callback hook has been defined (i.e. PPP_INSTALL_CALLBACK_HOOK is 
 * not true) work is done by the ipppp_example_action_work_default 
 * routine, which is in ipppp.c.
 */

/*
 ****************************************************************************
 * 2                    CONFIGURATION
 ****************************************************************************
 */

#ifdef IPCOM_USE_CLIB_PROTO
#undef IPCOM_USE_CLIB_PROTO
#endif

#ifndef IPCOM_KERNEL
#define IPCOM_KERNEL
#endif
#include "ipppp_config.h"


/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#ifdef INCLUDE_IPCOM_USE_SYSLOGD
#define IPCOM_USE_SYSLOG
#endif

#include <ipcom_err.h>

/*
 * some internal macro mappings - ugly but necessary for Ipcom_netif
 * structure alignment!
 */
#ifdef INCLUDE_IPPPP
#define IPPPP
#endif

#ifdef INCLUDE_IP_SECURITY
#define IPIPSEC2
#endif

#ifdef INCLUDE_IPNET_USE_WLAN
#define IPWLAN
#endif

#include <ipcom_netif.h>
#include <ipcom_syslog.h>
#include <ipcom_clib.h>
#include <ipcom_err.h>
#include "ipppp.h"

#if defined(INCLUDE_IPPPP) || defined(INCLUDE_IPPPPOE)

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


/* typedef to make compiler happy... */
typedef int (*ipppp_action_cb)(struct Ipppp_action_work *w);


/*
 ****************************************************************************
 * 6                    EXTERN PROTOTYPES
 ****************************************************************************
 */

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

Ipcom_once_t ipppp_work_once = IPCOM_ONCE_INIT;
IP_EXTERN Ipcom_mutex  ipppp_work_lock;
IP_EXTERN Ipcom_sem    ipppp_work_sem;
IP_EXTERN Ipcom_list   ipppp_work_queue;


/*
 ****************************************************************************
 * 9                    STATIC FUNCTIONS
 ****************************************************************************
 */



/*
 *===========================================================================
 *                       ipppp_example_action_work
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipppp_example_action_work(struct Ipppp_action_work *w)
{
    ipppp_action_cb  ppp_cb;

#if PPP_INSTALL_CALLBACK_HOOK == TRUE
    ppp_cb = (PPP_ACTION_CALLBACK_HOOK == NULL) ?
        ipppp_example_action_work_default : PPP_ACTION_CALLBACK_HOOK;
#else
    ppp_cb = ipppp_example_action_work_default;
#endif

    return ppp_cb(w);
}


/*
 *===========================================================================
 *                       ipppp_work
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC
IPCOM_PROCESS(ipppp_work)
{
    ipcom_proc_init();

    for (;;)
    {
        struct Ipppp_action_work *work;

        ipcom_sem_wait(ipppp_work_sem);

        ipcom_mutex_lock(ipppp_work_lock);
        work = IPCOM_LIST_FIRST(&ipppp_work_queue);
        ipcom_list_remove(&work->list);
        ipcom_mutex_unlock(ipppp_work_lock);

        ipppp_example_action_work(work);

        ipcom_free(work);
    }

    /* ipcom_proc_exit(); */
}


/*
 *===========================================================================
 *                       ipppp_work_init
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err
ipppp_work_init(void *unused)
{
    IPCOM_UNUSED_ARG(unused);

    ipcom_list_init(&ipppp_work_queue);
    ipcom_mutex_create(&ipppp_work_lock);
    ipcom_sem_create(&ipppp_work_sem, 0);

    return ipcom_proc_create("ipppp_work",
                             (Ipcom_proc_func)ipppp_work,
                             IPCOM_PROC_STACK_DEFAULT,
                             IP_NULL);
}


/*
 *===========================================================================
 *                       ipppp_work_alloc
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC struct Ipppp_action_work *
ipppp_work_alloc(Ipcom_netif *netif, int action, void *data)
{
    struct Ipppp_action_work *work;

    work = ipcom_malloc(sizeof(*work));
    if (work)
    {
        ipcom_strcpy(work->ifname, netif->name);
        work->ifindex    = netif->ifindex;
        work->link_index = netif->link_index;
        work->action     = action;
        work->data       = data;
    }

    return work;
}


/*
 *===========================================================================
 *                       ipppp_work_schedule
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipppp_work_schedule(struct Ipppp_action_work *work)
{
    if (ipcom_once(&ipppp_work_once,
                   ipppp_work_init,
                   IP_NULL)
        != IPCOM_SUCCESS)
    {
        IP_PANIC();
        return -1;
    }


    ipcom_mutex_lock(ipppp_work_lock);
    ipcom_list_insert_last(&ipppp_work_queue, &work->list);
    ipcom_mutex_unlock(ipppp_work_lock);

    ipcom_sem_post(ipppp_work_sem);
    return 0;
}
#endif /* INCLUDE_IPPPP || INCLUDE_IPPPPOE */


/*
 ****************************************************************************
 * 10                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*****************************************************************************
 * ipppp_example_action_cb - PPP action callback routine
 *
 * In order for PPP to function, this callback function is called by PPP to
 * handle various PPP actions like initialization, startup, peer IPv4 address
 * requests, peer authentication login etc.
 *
 * .IP <netif>
 * Ipcom_netif pointer to the PPP interface.
 * .IP <action>
 * The action type - IPPPP_ACTION_xxx.
 * .IP <data>
 * Action type specific additional data.
 *
 * RETURNS: Depends on the action type.
 *
 * NOMANUAL
 */
IP_PUBLIC int
ipppp_example_action_cb(Ipcom_netif *netif, int action, void *data)
{
#if defined(INCLUDE_IPPPP) || defined(INCLUDE_IPPPPOE)
    struct Ipppp_action_work *work;

    work = ipppp_work_alloc(netif, action, data);

    return ipppp_work_schedule(work);
#else
    return 0;
#endif /* INCLUDE_IPPPP || INCLUDE_IPPPPOE */
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

