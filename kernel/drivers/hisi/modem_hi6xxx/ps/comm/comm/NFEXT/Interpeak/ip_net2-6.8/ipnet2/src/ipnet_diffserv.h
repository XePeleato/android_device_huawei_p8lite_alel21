/*
 ******************************************************************************
 *                     INTERPEAK API HEADER FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_diffserv.h,v $ $Revision: 1.2 $
 *   $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_diffserv.h,v $
 *   $Author: kenneth $ $Date: 2004-12-06 12:55:07 $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Kenneth Jonsson <kenneth@interpeak.se>
 ******************************************************************************
 */
#ifndef IPNET_DIFFSERV_H
#define IPNET_DIFFSERV_H

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 */

/*
  IPNET differentiated services infrastructure.

                               +-------+
                               |       |-------------------+
                        +----->| Meter |                   |
                        |      |       |--+                |
                     2. |      +-------+  |                |
                        |                 V                V
                  +------------+      +--------+      +---------+
              1.  |            |  3.  |        |  4.  | Shaper/ |  5.
    packets =====>| Classifier |=====>| Marker |=====>| Dropper |=====>
                  |            |      |        |      |         |
                  +------------+      +--------+      +---------+

  The classifier is implemented by the ipnet_classifier module,
  the shaping is done in the queue of the outgoing interface
  (ipnet_pkt_queue_xxx modules).
  The meter and the marker is implemented by ipnet_ds_xxx modules.
  This header file provides the infrastructure to connect all those
  pieces for all forwarded traffic.

*/

/*
 ****************************************************************************
 * 2                    CONFIGURATION
 ****************************************************************************
 */

#include "ipnet_config.h"

/* Maximum number of handlers that can be defined at any time,
   must be <= IPNET_DIFFSERV_MAX_MAPPINGS */
#define IPNET_DIFFSERV_MAX_HANDLERS 64


/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#include <ipcom_type.h>
#include <ipcom_cstyle.h>
#include <ipcom_sock.h>

#include "ipnet_classifier.h"

#ifdef __cplusplus
extern "C" {
#endif

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

/* Forward declaration */
struct Ipnet_diffserv_handlers_struct;

/*
 *===========================================================================
 *                    Ipnet_diffserv_meter_input
 *===========================================================================
 * Description: Input handler for a diffserv meter.
 * Parameters:  h - Pointer to the handlers installed for the selected class.
 *              pkt - The IP datagram that beeing processed.
 * Returns:
 *
 */
typedef void (*Ipnet_diffserv_meter_input)(struct Ipnet_diffserv_handlers_struct *h,
                                           Ipcom_pkt *pkt);


/*
 *===========================================================================
 *                    Ipnet_diffserv_marker_input
 *===========================================================================
 * Description: Input handler for a diffserv marker.
 * Parameters:  h - Pointer to the handlers installed for the selected class.
 *              pkt - The IP datagram that beeing processed.
 * Returns:
 *
 */
typedef void (*Ipnet_diffserv_marker_input)(struct Ipnet_diffserv_handlers_struct *h,
                                            Ipcom_pkt *pkt);


/*
 *===========================================================================
 *                    Ipnet_diffserv_dtor
 *===========================================================================
 * Description: Constructor for the marker/meter specified by the name.
 * Parameters:  arg - The arguments for this marker/meter.
 * Returns:     A newly created diffserv handler.
 *
 */
typedef void (*Ipnet_diffserv_dtor)(struct Ipnet_diffserv_handlers_struct *h);


/* Diffserv handlers for packets that has the specified class */
typedef struct Ipnet_diffserv_handlers_struct
{
    /* Input function for the meter or IP_NULL if nothing should be measured for this class. */
    Ipnet_diffserv_meter_input          meter_input;
    /* Input function for the marker or IP_NULL if no packets should be (re) marked */
    Ipnet_diffserv_marker_input         marker_input;
    /* Destructor for this diffserv handler */
    Ipnet_diffserv_dtor                 destroy;
}
Ipnet_diffserv_handlers;



/*
 *===========================================================================
 *                    Ipnet_diffserv_ctor
 *===========================================================================
 * Description: Constructor for the marker/meter specified by the name.
 * Parameters:  arg - The arguments for this marker/meter.
 *              phandler - Will point to the newly created diffserv handler.
 * Returns:     0 = success
 *             <0 = error code.
 *
 */
typedef int (*Ipnet_diffserv_ctor)(void *arg, Ipnet_diffserv_handlers **phandler);


typedef struct Ipnet_diffserv_handlers_template_struct
{
    const char          *name;
    Ipnet_diffserv_ctor  create;
}
Ipnet_diffserv_handlers_template;


/*
 ****************************************************************************
 * 6                    FUNCTIONS
 ****************************************************************************
 */

IP_GLOBAL void ipnet_diffserv_input(Ipcom_pkt *pkt);
IP_GLOBAL Ip_u8 ipnet_diffserv_get_ds_field(Ipcom_pkt *pkt);
IP_GLOBAL void ipnet_diffserv_set_ds_field(Ipcom_pkt *pkt, Ip_u8 ds);

IP_GLOBAL int ipnet_diffserv_ioctl(Ip_u32 request, void *argp);
IP_GLOBAL int ipnet_diffserv_init(void);

/* Known diffserv handlers */
IP_GLOBAL Ipnet_diffserv_handlers_template * ipnet_diffserv_srtcm_template(void);
IP_GLOBAL Ipnet_diffserv_handlers_template * ipnet_diffserv_trtcm_template(void);
IP_GLOBAL Ipnet_diffserv_handlers_template * ipnet_diffserv_sm_template(void);


#ifdef __cplusplus
}
#endif

#endif

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
