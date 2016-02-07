/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_diffserv.c,v $ $Revision: 1.22 $
 *   $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_diffserv.c,v $
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
#include <ipcom_err.h>
#include <ipcom_sock.h>
#include <ipcom_syslog.h>
#include <ipcom_type.h>

#include "ipnet_h.h"
#include "ipnet_classifier.h"


/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

#ifdef IPNET_DIFFSERV_CLASSIFIER_MODE_BA
#define IPNET_DIFFSERV_MAX_MAPPINGS 256
#endif

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
 *                    ipnet_diffserv_key_hash
 *===========================================================================
 * Description: Computes a hash value for the specified diffserv type.
 * Parameters:  name - A diffserv entity name.
 * Returns:     The hash value.
 *
 */
IP_STATIC unsigned
ipnet_diffserv_key_hash(const char *name)
{
    return ipcom_hash_update(name, ipcom_strlen(name), 0);
}


/*
 *===========================================================================
 *                    ipnet_diffserv_obj_hash
 *===========================================================================
 * Description: Computes a hash value for the specified diffserv type.
 * Parameters:  template - The template for an differv entity.
 * Returns:     The hash value.
 *
 */
IP_STATIC unsigned
ipnet_diffserv_obj_hash(Ipnet_diffserv_handlers_template *template)
{
    return ipcom_hash_update(template->name, ipcom_strlen(template->name), 0);
}


/*
 *===========================================================================
 *                    ipnet_diffserv_hash_cmp
 *===========================================================================
 * Description: Compares the type of the template with the specified type.
 * Parameters:  template - A template for a diffserv type.
 *              type - A diffserv type.
 * Returns:     IP_TRUE if the specified type matches the template.
 *
 */
IP_STATIC Ip_bool
ipnet_diffserv_hash_cmp(Ipnet_diffserv_handlers_template *template, const char *name)
{
    return ipcom_strcmp(template->name, name) == 0;
}


/*
 *===========================================================================
 *                  ipnet_diffserv_register_ctor
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipnet_diffserv_register_ctor(Ipnet_diffserv_handlers_template *t)
{
   if (ipcom_hash_add(ipnet->diffserv.meter_marker_ctor_db, t) != IPCOM_SUCCESS)
       return -IP_ERRNO_ENOMEM;
   return 0;
}


/*
 ****************************************************************************
 * 10                   GLOBAL FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                  ipnet_diffserv_input
 *===========================================================================
 * Description: Inputs a packet that will be processed according to the
 *              rules for its class.
 * Parameters:  pkt - A IP datagram.
 * Returns:
 *
 */
IP_GLOBAL void
ipnet_diffserv_input(Ipcom_pkt *pkt)
{
    Ipnet_diffserv_handlers *h;
    int                      class;

#ifdef IPNET_DIFFSERV_CLASSIFIER_MODE_BA
    class = ipnet_diffserv_get_ds_field(pkt);
    if (IP_BIT_ISFALSE(ipnet->diffserv.mapping_active[class >> 5], 1 << (class & 0x1f)))
        return;
    h = ipnet->diffserv.handlers[ipnet->diffserv.ds_to_handler_map[class]];
#else
    class = ipnet_classifier_pkt_to_tag(ipnet->diffserv.classifier, pkt);
    if (class < 0)
        /* No meter/maker mapping installed for this traffic */
        return;
    ip_assert(class < IPNET_DIFFSERV_MAX_HANDLERS);
    h = ipnet->diffserv.handlers[class];
#endif

    ip_assert(h != IP_NULL);

    if (h->meter_input)
        h->meter_input(h, pkt);

    h->marker_input(h, pkt);
}


/*
 *===========================================================================
 *                 ipnet_diffserv_get_ds_field
 *===========================================================================
 * Description: Returns the value of the type-of-service field (IPv4) or
 *              traffic class (IPv6).
 * Parameters:  pkt - An IP datagram.
 * Returns:     The Differentiated Services Field.
e *
 */
IP_GLOBAL Ip_u8
ipnet_diffserv_get_ds_field(Ipcom_pkt *pkt)
{
    switch (pkt->data[pkt->start] & 0xf0)
    {
#ifdef IPCOM_USE_INET
    case 0x40:
        return ((Ipnet_pkt_ip *) &pkt->data[pkt->start])->tos;
#endif /* IPCOM_USE_INET */
#ifdef IPCOM_USE_INET6
    case 0x60:
        return IPNET_IP6_GET_CLASS((Ipnet_pkt_ip6 *) &pkt->data[pkt->start]);
#endif /* IPCOM_USE_INET6 */
    default:
        IP_PANIC2();
        break;
    }
    return 0;
}


/*
 *===========================================================================
 *                 ipnet_diffserv_set_ds_field
 *===========================================================================
 * Description: Set the TOS field (IPv4) or traffic class (IPv6) to the
 *              specified value.
 * Parameters:  pkt - An IP datagram.
 *              ds - The Differentiated Services Field.
 * Returns:
 *
 */
IP_GLOBAL void
ipnet_diffserv_set_ds_field(Ipcom_pkt *pkt, Ip_u8 ds)
{
    switch (pkt->data[pkt->start] & 0xf0)
    {
#ifdef IPCOM_USE_INET
    case 0x40:
        ((Ipnet_pkt_ip *) &pkt->data[pkt->start])->tos = ds;
        break;
#endif /* IPCOM_USE_INET */
#ifdef IPCOM_USE_INET6
    case 0x60:
        IPNET_IP6_SET_CLASS((Ipnet_pkt_ip6 *) &pkt->data[pkt->start], ds);
        break;
#endif /* IPCOM_USE_INET6 */
    default:
        IP_PANIC();
        break;
    }
}


/*
 *===========================================================================
 *                    ipnet_diffserv_ioctl
 *===========================================================================
 * Description: DiffServ public API handler.
 * Parameters:  request - The operation to perform
 *              argp - The argument to the operation.
 * Returns:      0 = success
 *              <0 = error code.
 *
 */
IP_GLOBAL int
ipnet_diffserv_ioctl(Ip_u32 request, void *argp)
{
    Ipnet_diffserv_handlers_template *t;
    Ipnet_diffserv_handlers          *h;
    int                               i;
    struct Ipnet_ds                  *ds = argp;
    struct Ipnet_ds_map              *map = argp;
#ifndef IPNET_DIFFSERV_CLASSIFIER_MODE_BA
    struct Ipnet_ds_filter           *filter = argp;
#endif

    switch (request)
    {
#ifdef IPNET_DIFFSERV_CLASSIFIER_MODE_BA
    case IPNET_SIOCXADSMAP:
        if (map->filter_id < 0 || map->filter_id >= IPNET_DIFFSERV_MAX_MAPPINGS
            || map->ds_id < 0 || map->ds_id >= IPNET_DIFFSERV_MAX_HANDLERS)
            return -IP_ERRNO_EINVAL;

        if (ipnet->diffserv.handlers[map->ds_id] == IP_NULL)
        {
            IPCOM_LOG1(WARNING, "DiffServ: Tried to add a mapping for a non-existing DS ID: %d",
                       map->ds_id);
            return -IP_ERRNO_EINVAL;
        }
        ipnet->diffserv.ds_to_handler_map[map->filter_id] =(Ip_u8) map->ds_id;
        IP_BIT_SET(ipnet->diffserv.mapping_active[map->filter_id >> 5],
                   1 << (map->filter_id & 0x1f));
        break;

    case IPNET_SIOCXDDSMAP:
        if (map->filter_id < 0 || map->filter_id > IPNET_DIFFSERV_MAX_MAPPINGS)
            return -IP_ERRNO_EINVAL;
        IP_BIT_CLR(ipnet->diffserv.mapping_active[map->filter_id >> 5],
                   1 << (map->filter_id & 0x1f));
        break;

#else
    case IPNET_SIOCXADSMAP:
        if (map->ds_id < 0 || map->ds_id > IPNET_DIFFSERV_MAX_HANDLERS)
            return -IP_ERRNO_EINVAL;

        if (ipnet->diffserv.handlers[map->ds_id] == IP_NULL)
        {
            IPCOM_LOG1(WARNING, "DiffServ: Tried to add a mapping for a non-existing DS ID: %d",
                       map->ds_id);
            return -IP_ERRNO_EINVAL;
        }
        return ipnet_classifier_set_tag(ipnet->diffserv.classifier, map->filter_id, map->ds_id);

    case IPNET_SIOCXDDSMAP:
        ipnet_classifier_invalidate_tag(ipnet->diffserv.classifier, map->ds_id);
        return 0;

    case IPNET_SIOCXADSFILTER:
        filter->id = ipnet_classifier_get_smallest_unused_id(ipnet->diffserv.classifier);
        return ipnet_classifier_add_mapping(ipnet->diffserv.classifier, &filter->rule, filter->id);

    case IPNET_SIOCXDDSFILTER:
        return ipnet_classifier_del_mapping(ipnet->diffserv.classifier, filter->id);
#endif /* IPNET_DIFFSERV_CLASSIFIER_MODE_BA */

    case IPNET_SIOCXDSCREATE:
        for (i = 0; i < IPNET_DIFFSERV_MAX_HANDLERS; i++)
            if (ipnet->diffserv.handlers[i] == IP_NULL)
            {
                t = ipcom_hash_get(ipnet->diffserv.meter_marker_ctor_db, ds->name);
                if (t == IP_NULL)
                    return -IP_ERRNO_EINVAL;
                ds->id = i;
                return t->create(&ds->d, &ipnet->diffserv.handlers[i]);
            }

        return -IP_ERRNO_ENOBUFS;

    case IPNET_SIOCXDSDESTROY:
        if (ds->id < 0 || ds->id >= IPNET_DIFFSERV_MAX_HANDLERS)
            return -IP_ERRNO_EINVAL;

        h = ipnet->diffserv.handlers[ds->id];
        if (h == IP_NULL)
            return -IP_ERRNO_ESRCH;

        ipnet->diffserv.handlers[ds->id] = IP_NULL;
        h->destroy(h);

#ifdef IPNET_DIFFSERV_CLASSIFIER_MODE_BA
        /* Deactivate all mappings to this handler */
        for (i = 0; i < IPNET_DIFFSERV_MAX_MAPPINGS; i++)
            if (ipnet->diffserv.ds_to_handler_map[i] == ds->id)
                IP_BIT_CLR(ipnet->diffserv.mapping_active[map->filter_id >> 5],
                           1 << (map->filter_id & 0x1f));
#else
        ipnet_classifier_invalidate_tag(ipnet->diffserv.classifier, ds->id);
#endif

        break;

    default:
        return -IP_ERRNO_EINVAL;
    }
    return 0;
}


/*
 *===========================================================================
 *                    ipnet_diffserv_init
 *===========================================================================
 * Description: Initializes the DiffServ module.
 * Parameters:
 * Returns:     0 = success.
 *             <0 = error code.
 */
IP_GLOBAL int
ipnet_diffserv_init(void)
{
    int ret;

    ipnet->diffserv.meter_marker_ctor_db
        = ipcom_hash_new((Ipcom_hash_obj_func) ipnet_diffserv_obj_hash,
                         (Ipcom_hash_key_func) ipnet_diffserv_key_hash,
                         (Ipcom_hash_cmp_func) ipnet_diffserv_hash_cmp);
    if (ipnet->diffserv.meter_marker_ctor_db == IP_NULL)
    {
        IPCOM_LOG0(CRIT, "DiffServ: Out of memory when creating meter/marker database");
        return -IP_ERRNO_ENOMEM;
    }

    /* All known handlers constructors must be registered here */
    if (0 > (ret = ipnet_diffserv_register_ctor(ipnet_diffserv_srtcm_template())))
        return ret;
    if (0 > (ret = ipnet_diffserv_register_ctor(ipnet_diffserv_sm_template())))
        return ret;

#ifndef IPNET_DIFFSERV_CLASSIFIER_MODE_BA
    ipnet->diffserv.classifier = ipnet_classifier_new();
    if (IP_NULL == ipnet->diffserv.classifier)
    {
        IPCOM_LOG0(CRIT, "DiffServ: Out of memory when creating classifier");
        return -IP_ERRNO_ENOMEM;
    }
#endif

    return 0;
}

/*
 ****************************************************************************
 * 11                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

#else
int ipnet_diffserv_empty_file;
#endif /* IPNET_USE_DIFFSERV */

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

