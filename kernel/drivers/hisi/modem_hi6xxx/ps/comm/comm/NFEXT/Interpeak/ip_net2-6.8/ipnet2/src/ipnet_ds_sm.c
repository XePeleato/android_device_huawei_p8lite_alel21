/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_ds_sm.c,v $ $Revision: 1.2 $
 *   $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_ds_sm.c,v $
 *   $Author: kenneth $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Kenneth Jonsson <kenneth@interpeak.se>
 ******************************************************************************
 */


/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 */
/* Simple marker */

/*
 ****************************************************************************
 * 2                    CONFIGURATION
 ****************************************************************************
 */

#ifndef IPCOM_KERNEL
#define IPCOM_KERNEL
#endif
#include "ipnet_config.h"


/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */


#ifdef IPNET_USE_DIFFSERV

#define IPCOM_USE_CLIB_PROTO
#include <ipcom_clib.h>
#include <ipcom_cstyle.h>
#include <ipcom_pkt.h>
#include <ipcom_type.h>
#include <ipcom_syslog.h>

#include "ipnet_diffserv.h"
#include "ipnet_h.h"

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

typedef struct Ipnet_simple_marker_struct
{
    Ipnet_diffserv_handlers dsh;
    /* Configuration */
    struct Ipnet_ds_sm      conf;
}
Ipnet_simple_marker;


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

/*
 ****************************************************************************
 * 9                    STATIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                     ipnet_ds_marker_sm_input
 *===========================================================================
 * Description: Input handler for packets that belong to the controlled stream.
 * Parameters:  sm - A simple marker object.
 *              pkt - The IP datagram to mark.
 * Returns:
 *
 */
IP_STATIC void
ipnet_ds_marker_sm_input(Ipnet_simple_marker *sm, Ipcom_pkt *pkt)
{
    if (IP_BIT_ISSET(sm->conf.mask, IPNET_DS_SM_DS_VAL))
        ipnet_diffserv_set_ds_field(pkt, sm->conf.ds_value);

    if (IP_BIT_ISSET(sm->conf.mask, IPNET_DS_SM_DROP_P))
        IPCOM_PKT_SET_DROP_PRECEDENCE(pkt, sm->conf.drop_precedence);
}


/*
 *===========================================================================
 *                   ipnet_ds_sm_delete
 *===========================================================================
 * Description: Deletes the specified simple marker object.
 * Parameters:  sm - A simple marker object to delete.
 * Returns:
 *
 */
IP_STATIC void
ipnet_ds_sm_delete(Ipnet_simple_marker *sm)
{
    ipcom_free(sm);
}


/*
 *===========================================================================
 *                  ipnet_ds_srtcm_new
 *===========================================================================
 * Description: Creates a new single rate three color maker meter.
 *              Note: at least one of cbs and ebs must be >0.
 * Parameters:
 * Returns:     0 = success
 *             <0 = error code
 *
 */
IP_STATIC int
ipnet_ds_sm_new(struct Ipnet_ds_sm *conf, Ipnet_diffserv_handlers **phandler)
{
    Ipnet_simple_marker *sm;

    if (IP_BIT_ISSET(conf->mask, IPNET_DS_SM_DROP_P)
        && conf->drop_precedence != IPCOM_PKT_DROP_PRECEDENCE_UNDEF
        && conf->drop_precedence != IPCOM_PKT_DROP_PRECEDENCE_LOW
        && conf->drop_precedence != IPCOM_PKT_DROP_PRECEDENCE_MEDIUM
        && conf->drop_precedence != IPCOM_PKT_DROP_PRECEDENCE_HIGH)
        return -IP_ERRNO_EINVAL;

    sm = ipcom_calloc(1, sizeof(Ipnet_simple_marker));
    if (sm == IP_NULL)
        return IP_NULL;

    sm->dsh.destroy      = (Ipnet_diffserv_dtor) ipnet_ds_sm_delete;

    /* Initialize the marker */
    ipcom_memcpy(&sm->conf, conf, sizeof(struct Ipnet_ds_sm));
    sm->dsh.marker_input = (Ipnet_diffserv_marker_input) ipnet_ds_marker_sm_input;

    *phandler = &sm->dsh;
    return 0;
}


/*
 ****************************************************************************
 * 10                   GLOBAL FUNCTIONS
 ****************************************************************************
 */


/*
 *===========================================================================
 *                  ipnet_diffserv_srtcm_template
 *===========================================================================
 * Description: Is called at boot to register the constructor for this module.
 * Parameters:
 * Returns:     The template that knows how to create meter/markers for this
 *              module.
 *
 */
IP_GLOBAL Ipnet_diffserv_handlers_template *
ipnet_diffserv_sm_template(void)
{
    static Ipnet_diffserv_handlers_template t = {
        "SimpleMarker",
        (Ipnet_diffserv_ctor) ipnet_ds_sm_new
    };
    return &t;
}


/*
 ****************************************************************************
 * 11                   PUBLIC FUNCTIONS
 ****************************************************************************
 */


#else
int ipnet_ds_sm_empty_file;
#endif /* IPNET_USE_DIFFSERV */

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

