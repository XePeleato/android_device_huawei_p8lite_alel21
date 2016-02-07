/*
 ******************************************************************************
 *                     INTERPEAK API HEADER FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_timeout.h,v $ $Revision: 1.23 $
 *   $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_timeout.h,v $
 *   $Author: kenneth $ $Date: 2008-09-18 11:48:28 $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Kenneth Jonsson <kenneth@interpeak.se>
 ******************************************************************************
 */
#ifndef IPNET_TIMEOUT_H
#define IPNET_TIMEOUT_H

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 * IPNET timeout machinery.
 * Timeouts can not be scheduled from interrupt context.
 * Timeout handlers is always run with the IPNET code lock taken.
 *
 * IPNET soft interrupt machinery.
 * Soft timeouts should be scheduled by code running in interrupt context
 * and have code that what to update the internal state of IPNET.
 * Soft interrupt handlers is always run with the IPNET code lock taken.
 * The only IPNET functions that can safly be called from interrupt context are:
 * - ipnet_schedule_soft_interrupt()
 * - ipcom_pkt_malloc()
 * - ipcom_pkt_free()
 *
 */

/*
 ****************************************************************************
 * 2                    CONFIGURATION
 ****************************************************************************
 */

#include "ipnet_config.h"


/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#include <ipcom_type.h>
#include <ipcom_cstyle.h>


#ifdef __cplusplus
extern "C" {
#endif

/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

/* Evalutates to the current system absolute time in seconds */
#define IPNET_SECONDS_ABS  ipnet_sec_now()


/*
 * set/get the timeout structure (if any) used for this fragment
 */
#define IPNET_PKT_GET_TMO_PTR(pkt)     ((Ipnet_timeout **) &(pkt)->link_cookie)
#define IPNET_PKT_GET_TMO(pkt)         ((Ipnet_timeout *) (pkt)->link_cookie)
#define IPNET_PKT_SET_TMO(pkt, timeo)  ((pkt)->link_cookie = (Ip_ptrdiff_t) (timeo))

/*
 * set/get the absolute time (in seconds) when this fragment times out.
 */
#define IPNET_PKT_GET_TIMEOUT(pkt)             (IPNET_PKT_GET_TIMEOUT_ABS(pkt) - IPNET_SECONDS_ABS)
#define IPNET_PKT_GET_TIMEOUT_ABS(pkt) \
    ipcom_pkt_get_int(pkt, IPNET_PKT_INFO_TIMEOUT, Ip_u32)
#define IPNET_PKT_SET_TIMEOUT_ABS(pkt, timeo) \
    ipcom_pkt_set_int(pkt, IPNET_PKT_INFO_TIMEOUT, timeo, Ip_u32)


/*
 * Evaluates to a pointer where the soft interrupt structure can be stored
 * inside the packet
 */
#define IPNET_PKT_TO_SOFTIRQ_STRUCT(pkt) ((Ipnet_softirq *) ((pkt)->data))


/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

/* Signature of timeout handler */
typedef void (*Ipnet_timeout_handler)(void *cookie);

typedef struct Ipnet_timeout_struct
{
    int                           pq_index;  /* The index this timout has in the priority queue */
    Ip_u32                        msec;      /* The absolute time when this timeout is triggered */
    Ipnet_timeout_handler         handler;   /* The function that will be called when
                                                the current time is equal to 'timeout_msec',
                                                this member can be IP_NULL if no action should be taken */
    void                         *cookie;    /* The (user specified) argument to the handler */
    struct Ipnet_timeout_struct **ptmo;      /* Pointer to the location the caller has stored the
                                                Ipnet_timeout handler, that location will be set to
                                                IP_NULL when the timer expires or is cancled. */
}
Ipnet_timeout;


/* Signature of soft IRQ handler */
typedef void (*Ipnet_softirq_handler)(void *cookie);

typedef struct Ipnet_softirq_struct
{
    struct Ipnet_softirq_struct *next;    /* Points to the next soft interrupt */
    volatile Ip_bool             pending; /* IP_TRUE if this is currently in the list
                                              of pending soft interrupts */
    Ipnet_softirq_handler        handler; /* The function that will handle this soft interrupt */
    void                        *cookie;  /* The argument to the handler */
}
Ipnet_softirq;


/*
 ****************************************************************************
 * 6                    FUNCTIONS
 ****************************************************************************
 */

IP_GLOBAL void ipnet_softirq_init(Ipnet_softirq *soft_irq);
IP_GLOBAL void ipnet_softirq_wait_for_pending(void);
IP_GLOBAL void ipnet_softirq_schedule(Ipnet_softirq *soft_irq,
                                      Ipnet_softirq_handler handler,
                                      void *cookie);


IP_GLOBAL void   ipnet_timeout_cancel(Ipnet_timeout *tmo);
IP_GLOBAL int ipnet_timeout_schedule(Ip_u32 msec_to_timeout,
                                     Ipnet_timeout_handler handler,
                                     void *cookie,
                                     Ipnet_timeout **ptmo);
IP_GLOBAL Ip_u32 ipnet_timeout_msec_until(Ipnet_timeout *tmo);

#ifdef IPNET_DEBUG
typedef int (*Ipnet_timeout_to_string_f)(void *tmo_elem, char *buf, Ip_size_t buf_len);
typedef struct Ipnet_cmd_tmo_handler_to_str_struct
{
    Ipnet_timeout_handler     key;
    Ipnet_timeout_to_string_f to_str_func;
}
Ipnet_cmd_tmo_handler_to_str;

IP_GLOBAL void   ipnet_timeout_to_string(Ipnet_timeout_handler handler,
                                         Ipnet_timeout_to_string_f to_str);
#endif


#ifdef __cplusplus
}
#endif

#endif

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
