/*
 ******************************************************************************
 *                     INTERPEAK API HEADER FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_classifier.h,v $ $Revision: 1.6 $
 *   $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_classifier.h,v $
 *   $Author: kenneth $ $Date: 2005-05-27 10:09:01 $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Kenneth Jonsson <kenneth@interpeak.se>
 ******************************************************************************
 */
#ifndef IPNET_CLASSIFIER_H
#define IPNET_CLASSIFIER_H

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

#include "ipnet_config.h"


/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#include <ipcom_cstyle.h>
#include <ipcom_hash.h>
#include <ipcom_list.h>
#include <ipcom_sock.h>
#include <ipcom_type.h>

#include "ipnet_qos.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

#define IPNET_CLASSIFIER_INVALID_TAG (-1)


/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

/* Root structure for each classifier */
typedef struct Ipnet_classifier_struct
{
    Ipcom_list  rules;
    Ipcom_hash *cache;
}
Ipnet_classifier;


/*
 ****************************************************************************
 * 6                    FUNCTIONS
 ****************************************************************************
 */

IP_GLOBAL int  ipnet_classifier_pkt_to_tag(Ipnet_classifier *c, Ipcom_pkt *pkt);
IP_GLOBAL int  ipnet_classifier_set_tag(Ipnet_classifier *c, int id, int tag);
IP_GLOBAL void ipnet_classifier_invalidate_tag(Ipnet_classifier *c, int tag);

IP_GLOBAL int  ipnet_classifier_get_smallest_unused_id(Ipnet_classifier *c);

IP_GLOBAL int  ipnet_classifier_add_mapping(Ipnet_classifier *c,
                                            struct Ipnet_classifier_rule *rule,
                                            int id);
IP_GLOBAL int  ipnet_classifier_del_mapping(Ipnet_classifier *c, int id);

IP_GLOBAL Ipnet_classifier * ipnet_classifier_new(void);
IP_GLOBAL void               ipnet_classifier_delete(Ipnet_classifier *c);


#ifdef __cplusplus
}
#endif

#endif

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
