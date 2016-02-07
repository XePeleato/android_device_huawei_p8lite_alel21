/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_ds_srtcm.c,v $ $Revision: 1.10 $
 *   $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_ds_srtcm.c,v $
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
/* Implementation RFC 2697 - Single Rate Three Color Marker (srTMC) */

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

#define IPNET_SRTCM_METER_COLOR_GREEN  1
#define IPNET_SRTCM_METER_COLOR_YELLOW 2
#define IPNET_SRTCM_METER_COLOR_RED    3

/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

typedef struct Ipnet_srtcm_struct
{
    Ipnet_diffserv_handlers dsh;
    /* Configuration */
    struct Ipnet_ds_srtcm   conf;
    /* One of the IPNET_DS_METER_SRTCM_COLOR_xxx constants */
    int                     color;
    /* Token bucket for Committed Burst Size */
    Ip_u32                  Tc;
    /* Token bucket for Excess Burst Size */
    Ip_u32                  Te;
    /* Last tick tokens was added to the buckets */
    Ip_u32                  msec;
}
Ipnet_srtcm;


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
 *               ipnet_ds_meter_srtcm_class_to_color
 *===========================================================================
 * Description: Returns the color the packet has been gived by some before
 *              enter this srTCM (based on the recommended values from RFC2597).
 * Parameters:  pkt - The packet for which the color should be returned.
 * Returns:     The color of the packet as IPNET_DS_METER_SRTCM_COLOR_xxx.
 *
 */
IP_STATIC int
ipnet_ds_meter_srtcm_class_to_color(Ipcom_pkt *pkt)
{

    switch (ipnet_diffserv_get_ds_field(pkt))
    {
    case 0x0c:
    case 0x14:
    case 0x1c:
    case 0x24:
        /* RFC 2597: Medium Drop Prec */
        return IPNET_SRTCM_METER_COLOR_YELLOW;
    case 0x0e:
    case 0x16:
    case 0x1e:
    case 0x26:
        /* RFC 2597: High Drop Prec */
        return IPNET_SRTCM_METER_COLOR_RED;
    default:
        break;
    }
    return IPNET_SRTCM_METER_COLOR_GREEN;
}


/*
 *===========================================================================
 *                ipnet_ds_meter_srtcm_add_tokens
 *===========================================================================
 * Description: Adds more token to the stream controlled by this srTCM.
 * Parameters:  srtcm - A srTCM object.
 * Returns:
 *
 */
IP_STATIC void
ipnet_ds_meter_srtcm_add_tokens(Ipnet_srtcm *srtcm)
{
    Ip_u32 tokens;
    Ip_u32 now = ipnet_msec_now();

    tokens = (now - srtcm->msec) * srtcm->conf.CIR / 1000;

    if (tokens == 0)
        return;

    if (srtcm->conf.CBS > srtcm->Tc)
    {
        if (srtcm->Tc + tokens <= srtcm->conf.CBS)
        {
            srtcm->Tc += tokens;
            tokens = 0;
        }
        else
        {
            tokens -= srtcm->conf.CBS - srtcm->Tc;
            srtcm->Tc = srtcm->conf.CBS;
        }
    }
    if (srtcm->conf.EBS > srtcm->Te)
    {
        if (srtcm->Te + tokens <= srtcm->conf.EBS)
            srtcm->Te += tokens;
        else
        {
            /* tokens -= srtcm->conf.EBS - srtcm->Te; */
            srtcm->Te = srtcm->conf.EBS;
        }
    }
    srtcm->msec = now;
}


/*
 *===========================================================================
 *                     ipnet_ds_meter_srtcm_input
 *===========================================================================
 * Description: Input handler for packets that belong to the controlled stream.
 * Parameters:  h - The collection of diffserv handlers for this class.
 *              pkt - The IP datagram to process.
 * Returns:
 *
 */
IP_STATIC void
ipnet_ds_meter_srtcm_input(Ipnet_srtcm *srtcm, Ipcom_pkt *pkt)
{
    int initial_color;

    ipnet_ds_meter_srtcm_add_tokens(srtcm);

    if (srtcm->conf.mode == IPNET_DS_SRTCM_MODE_COLOR_BLIND)
        initial_color = IPNET_SRTCM_METER_COLOR_GREEN;
    else
        initial_color = ipnet_ds_meter_srtcm_class_to_color(pkt);

    switch (initial_color)
    {
    case IPNET_SRTCM_METER_COLOR_GREEN:
        if (srtcm->Tc >= (Ip_u32) pkt->end - pkt->start)
        {
            srtcm->Tc   -= (Ip_u32) pkt->end - pkt->start;
            srtcm->color = IPNET_SRTCM_METER_COLOR_GREEN;
            break;
        }
        /* else: fall through */
    case IPNET_SRTCM_METER_COLOR_YELLOW:
        if (srtcm->Te >= (Ip_u32) pkt->end - pkt->start)
        {
            srtcm->Te   -= (Ip_u32) pkt->end - pkt->start;
            srtcm->color = IPNET_SRTCM_METER_COLOR_YELLOW;
            break;
        }
        /* else: fall through */
    case IPNET_SRTCM_METER_COLOR_RED:
        srtcm->color = IPNET_SRTCM_METER_COLOR_RED;
        break;
    default:
        IP_PANIC();
        break;
    }
    IPCOM_LOG3(INFO, "DiffServ (srTCM): color %d, Tc %d, Te %d",
               srtcm->color, srtcm->Tc, srtcm->Te);
}


/*
 *===========================================================================
 *                     ipnet_ds_marker_srtcm_input
 *===========================================================================
 * Description: Input handler for packets that belong to the controlled stream.
 * Parameters:  srtcm - A srTCM object.
 *              pkt - The IP datagram to process.
 * Returns:
 *
 */
IP_STATIC void
ipnet_ds_marker_srtcm_input(Ipnet_srtcm *srtcm, Ipcom_pkt *pkt)
{
    switch (srtcm->color)
    {
    case IPNET_SRTCM_METER_COLOR_GREEN:
        ipnet_diffserv_set_ds_field(pkt, srtcm->conf.ds_green);
        IPCOM_PKT_SET_DROP_PRECEDENCE(pkt, IPCOM_PKT_DROP_PRECEDENCE_LOW);
        break;
    case IPNET_SRTCM_METER_COLOR_YELLOW:
        ipnet_diffserv_set_ds_field(pkt, srtcm->conf.ds_yellow);
        IPCOM_PKT_SET_DROP_PRECEDENCE(pkt, IPCOM_PKT_DROP_PRECEDENCE_MEDIUM);
        break;
    case IPNET_SRTCM_METER_COLOR_RED:
        ipnet_diffserv_set_ds_field(pkt, srtcm->conf.ds_red);
        IPCOM_PKT_SET_DROP_PRECEDENCE(pkt, IPCOM_PKT_DROP_PRECEDENCE_HIGH);
        break;
    default:
        IP_PANIC();
        break;
    }
}


/*
 *===========================================================================
 *                   ipnet_ds_srtcm_delete
 *===========================================================================
 * Description: Deletes the specified srTCM object.
 * Parameters:  srtcm - The object to delete.
 * Returns:
 *
 */
IP_STATIC void
ipnet_ds_srtcm_delete(Ipnet_srtcm *srtcm)
{
    ipcom_free(srtcm);
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
ipnet_ds_srtcm_new(struct Ipnet_ds_srtcm *conf, Ipnet_diffserv_handlers **phandler)
{
    Ipnet_srtcm *srtcm;

    srtcm = ipcom_malloc(sizeof(Ipnet_srtcm));
    if (srtcm == IP_NULL)
        return IP_NULL;

    ipcom_memcpy(&srtcm->conf, conf, sizeof(struct Ipnet_ds_srtcm));
    srtcm->dsh.destroy      = (Ipnet_diffserv_dtor) ipnet_ds_srtcm_delete;

    /* Initialize the meter */
    srtcm->Tc               = conf->CBS;
    srtcm->Te               = conf->EBS;
    srtcm->color            = IPNET_SRTCM_METER_COLOR_GREEN;
    srtcm->msec             = ipnet_msec_now();
    srtcm->dsh.meter_input  = (Ipnet_diffserv_meter_input) ipnet_ds_meter_srtcm_input;

    /* Initialize the marker */
    srtcm->dsh.marker_input = (Ipnet_diffserv_marker_input) ipnet_ds_marker_srtcm_input;

    *phandler = &srtcm->dsh;
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
ipnet_diffserv_srtcm_template(void)
{
    static Ipnet_diffserv_handlers_template t = {
        "srTCM",
        (Ipnet_diffserv_ctor) ipnet_ds_srtcm_new
    };
    return &t;
}


/*
 ****************************************************************************
 * 11                   PUBLIC FUNCTIONS
 ****************************************************************************
 */


#else
int ipnet_ds_srtcm_empty_file;
#endif /* IPNET_USE_DIFFSERV */

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

