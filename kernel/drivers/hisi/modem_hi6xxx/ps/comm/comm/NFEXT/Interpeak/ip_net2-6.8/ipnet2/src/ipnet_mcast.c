/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_mcast.c,v $ $Revision: 1.8 $
 *   $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_mcast.c,v $
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
 * Function that operations on the multicast control block used by the
 * IGMP and MLD protocol implementation.
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

#include <ipcom_clib.h>
#include <ipcom_err.h>
#include <ipcom_errno.h>
#include <ipcom_sysvar.h>

#include "ipnet_mcast.h"

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
 ****************************************************************************
 * 10                   GLOBAL FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipnet_mcast_init
 *===========================================================================
 * Description: Initializes a multicast control block.
 * Parameters:  macb - A multicast control block.
 * Returns:
 *
 */
#ifdef IPNET_USE_SOURCE_SPECIFIC_MCAST
IP_GLOBAL int
ipnet_mcast_init(Ipnet_mcast_addr *macb)
{
    macb->sockets = ipcom_set_new(sizeof(int));
    return (macb->sockets == IP_NULL ? -IP_ERRNO_ENOMEM : 0);
}
#endif /* IPNET_USE_SOURCE_SPECIFIC_MCAST */


/*
 *===========================================================================
 *                    ipnet_mcast_free
 *===========================================================================
 * Description: Frees all resources held by the multicast control block.
 * Parameters:  macb - A multicast control block.
 * Returns:
 *
 */
IP_GLOBAL void
ipnet_mcast_free(Ipnet_mcast_addr *macb)
{
    ipnet_mcast_clear(macb);
#ifdef IPNET_USE_SOURCE_SPECIFIC_MCAST
    ipcom_set_delete(macb->filter);
    ipcom_set_delete(macb->sockets);
    ipcom_set_delete(macb->specific_query_sources);
    ipcom_set_delete(macb->filter_change_sources);
#endif /* IPNET_USE_SOURCE_SPECIFIC_MCAST */
}


/*
 *===========================================================================
 *                    ipnet_mcast_clear
 *===========================================================================
 * Description: Resets the filter for the multicast address to "include nothing".
 * Parameters:  macb - A multicast control block.
 * Returns:
 *
 */
IP_GLOBAL void
ipnet_mcast_clear(Ipnet_mcast_addr *macb)
{
    ipnet_timeout_cancel(macb->specific_query_tmo);
    ipnet_timeout_cancel(macb->general_query_tmo);
    ipnet_timeout_cancel(macb->filter_change_tmo);
#ifdef IPNET_USE_SOURCE_SPECIFIC_MCAST
    /* Set filter to "TO_IN {}" */
    if (macb->filter == IP_NULL)
        /* The element size is unimportant since this set will never contain anything */
        macb->filter = ipcom_set_new(1);
    else
        ipcom_set_remove_all(macb->filter);

    if (macb->filter)
        macb->filter->user = IPNET_MCAST_RECORD_TYPE_CHANGE_TO_INCLUDE;
#endif /* IPNET_USE_SOURCE_SPECIFIC_MCAST */
}


/*
 *===========================================================================
 *                    ipnet_mcast_schedule_membership_report
 *===========================================================================
 * Description: Schedules sending of a report in the interval
 *              [0..max_delay_msec]
 * Parameters:  macb - A multicast control block.
 *              is_specific_query - IP_TRUE if this is a report as a response
 *                                  on a specific query.
 *              max_delay_msec - The maximum delay before the report must be
 *                               sent.
 *              tmo_handler - Function to call to send the report.
 *              addr_entry - The address entry for a multicast address.
 * Returns:
 *
 */
IP_GLOBAL void
ipnet_mcast_schedule_membership_report(Ipnet_mcast_addr *macb,
                                       Ip_bool is_specific_query,
                                       Ip_u32 max_delay_msec,
                                       Ipnet_timeout_handler tmo_handler,
                                       void *addr_entry)
{
    Ipnet_timeout **ptmo;
    Ip_u32          msec1;
    Ip_u32          msec2;

    if (is_specific_query)
        ptmo = &macb->specific_query_tmo;
    else
        ptmo = &macb->general_query_tmo;

    if (*ptmo != IP_NULL)
    {
        /* Report timer already running */
        if (ipnet_timeout_msec_until(*ptmo) <= max_delay_msec)
            /* Will expire before the max delay, no need to reset it */
            return;
        ipnet_timeout_cancel(*ptmo);
    }

    msec1 = ipcom_rand() % max_delay_msec;
    msec2 = ipcom_sysvar_get_as_int0("ipnet.mcast.min_report_delay", 0);
    (void) ipnet_timeout_schedule(IP_MAX(msec1, msec2),
                                  tmo_handler,
                                  addr_entry,
                                  ptmo);
}


/*
 *===========================================================================
 *                    ipnet_mcast_report_finish
 *===========================================================================
 * Description: Schedules sending of a report in the interval
 *              [0..max_delay_msec]
 * Parameters:  macb - A multicast control block.
 *              tmo_handler - Function to call to resend a report.
 *              addr_entry - The address entry for a multicast address.
 * Returns:
 *
 */
IP_GLOBAL void
ipnet_mcast_report_finish(Ipnet_mcast_addr *macb,
                          Ipnet_timeout_handler tmo_handler,
                          void *addr_entry)
{
    switch (macb->report_type)
    {
#ifdef IPNET_USE_SOURCE_SPECIFIC_MCAST
    case IPNET_MCAST_REPORT_SPECIFIC_QUERY:
        ipcom_set_delete(macb->specific_query_sources);
        macb->specific_query_sources = IP_NULL;
        break;
#endif /* IPNET_USE_SOURCE_SPECIFIC_MCAST */
    case IPNET_MCAST_REPORT_FILTER_CHANGE:
        if (macb->filter_change_resend_count)
        {
            --macb->filter_change_resend_count;
            if (ipnet_timeout_schedule(IPNET_MCAST_UNSOLICITED_REPORT_INTERVAL,
                                       tmo_handler,
                                       addr_entry,
                                       &macb->filter_change_tmo) == 0)
                break;
        }

#ifdef IPNET_USE_SOURCE_SPECIFIC_MCAST
        ipcom_set_delete(macb->filter_change_sources);
        macb->filter_change_sources = IP_NULL;
#endif /* IPNET_USE_SOURCE_SPECIFIC_MCAST */
        break;
    default:
        break;
    }
}



#ifdef IPNET_USE_SOURCE_SPECIFIC_MCAST
/*
 *===========================================================================
 *                    ipnet_mcast_build_if_filter
 *===========================================================================
 * Description: Builds the interface filter from the sources from a socket.
 *              This function should be called once for set of sources
 *              for every socket in the system.
 * Parameters:  macb - A multicast control block.
 *              sources - The set of sources for one socket.
 * Returns:
 *
 */
IP_GLOBAL void
ipnet_mcast_build_if_filter(Ipnet_mcast_addr *macb, Ipcom_set *sources)
{
    Ipcom_set *set = macb->filter;

    if (set == IP_NULL)
    {
        /* Use the this set as starting point */
        macb->filter = ipcom_set_clone(sources);
        return;
    }
    else if (macb->report_type == 0)
    {
        if (set->user == IPNET_MCAST_RECORD_TYPE_IS_INCLUDE)
        {
            if (sources->user == IPNET_MCAST_RECORD_TYPE_IS_EXCLUDE)
            {
                /* Find first exclude sets to use as a starting point */
                ipcom_set_delete(set);
                macb->filter = ipcom_set_clone(sources);
            }
        }
        else
        {
            if (sources->user == IPNET_MCAST_RECORD_TYPE_IS_EXCLUDE)
                /* The staring filter must contain all excluded
                   sources. */
                ipcom_set_union(set, sources);
        }
        return;
    }

    if (set->user == IPNET_MCAST_RECORD_TYPE_IS_INCLUDE)
    {
        ip_assert(sources->user == IPNET_MCAST_RECORD_TYPE_IS_INCLUDE);
        (void)ipcom_set_union(set, sources);
    }
    else
    {
        if (sources->user == IPNET_MCAST_RECORD_TYPE_IS_INCLUDE)
            ipcom_set_subtract(set, sources);
        else
            ipcom_set_intersection(set, sources);
    }
}


/*
 *===========================================================================
 *                  ipnet_mcast_build_source_change_report
 *===========================================================================
 * Description: Builds the change set for a specific multicast address
 *              and updates the interface filter.
 * Parameters:  macb - A multicast control block.
 *              build_if_filter_cb - Function that knows how to build the
 *                                   interface filter.
 *              addr_entry - Data passed to the "build_if_filter_cb".
 * Returns:     IP_TRUE if a report should be sent, IP_FALSE if it should not.
 *
 */
IP_GLOBAL Ip_bool
ipnet_mcast_build_source_change_report(Ipnet_mcast_addr *macb,
                                       Ipcom_set_foreach_cb_func build_if_filter_cb,
                                       void *addr_entry)
{
    int        current_fm;
    int        new_fm;
    int        report_fm;
    Ip_bool    send_report = IP_FALSE;
    Ipcom_set *change_set;
    Ipcom_set *if_filter;

    if_filter = macb->filter;
    macb->filter = IP_NULL;

    if (if_filter == IP_NULL)
        current_fm = IPNET_MCAST_RECORD_TYPE_IS_EXCLUDE;
    else
    {
        /* Must find one of the exclude filters as a starting point */
        macb->report_type = 0;
        ipcom_set_for_each(macb->sockets,
                           build_if_filter_cb,
                           addr_entry);

        if (macb->filter == IP_NULL)
            /* Out of memory, use exclude nothing filter for now */
            goto done;

        current_fm = if_filter->user;
        if (current_fm == IPNET_MCAST_RECORD_TYPE_CHANGE_TO_INCLUDE)
            macb->filter->user = IPNET_MCAST_RECORD_TYPE_IS_INCLUDE;
    }

    /* Build the group interface filter from all socket filters using this group */
    macb->report_type = ~0;
    ipcom_set_for_each(macb->sockets,
                       build_if_filter_cb,
                       addr_entry);
    if (macb->filter == IP_NULL)
        goto done;

    new_fm = macb->filter->user;
    change_set = macb->filter_change_sources;

    if (new_fm != current_fm)
    {
        report_fm = (new_fm == IPNET_MCAST_RECORD_TYPE_IS_INCLUDE
                     ? IPNET_MCAST_RECORD_TYPE_CHANGE_TO_INCLUDE
                     : IPNET_MCAST_RECORD_TYPE_CHANGE_TO_EXCLUDE);
        ipcom_set_delete(change_set);
        change_set = ipcom_set_clone(macb->filter);
        if (change_set == IP_NULL)
            /* Out of memory */
            goto done;
    }
    else
    {
        int        add_op;
        int        del_op;
        Ipcom_set *this_change;
        Ip_bool    is_add = IP_FALSE;

        if (ipcom_set_equal(macb->filter, if_filter))
            /* No change to report */
            goto done;

        if (if_filter == IP_NULL || macb->filter->obj_count > if_filter->obj_count)
            is_add = IP_TRUE;

        this_change = ipcom_set_clone(is_add ? macb->filter : if_filter);
        if (this_change == IP_NULL)
            /* Out of memory */
            goto done;

        if (if_filter != IP_NULL)
            ipcom_set_subtract(this_change, is_add ? if_filter : macb->filter);
        /* 'this_change' now contains the difference between the old and the new interface filter */

        add_op = (new_fm == IPNET_MCAST_RECORD_TYPE_IS_INCLUDE
                  ? IPNET_MCAST_RECORD_TYPE_ALLOW_NEW_SOURCES
                  : IPNET_MCAST_RECORD_TYPE_BLOCK_OLD_SOURCES);
        del_op = (new_fm == IPNET_MCAST_RECORD_TYPE_IS_INCLUDE
                  ? IPNET_MCAST_RECORD_TYPE_BLOCK_OLD_SOURCES
                  : IPNET_MCAST_RECORD_TYPE_ALLOW_NEW_SOURCES);

        if (change_set != IP_NULL)
        {
            /* Reporting already in progress, merge the new change with the old report
               unless the report type different. This report will take precedence
               over the old report if they cannot be merged */
            if (new_fm == IPNET_MCAST_RECORD_TYPE_IS_INCLUDE)
            {
                if (change_set->user == IPNET_MCAST_RECORD_TYPE_BLOCK_OLD_SOURCES)
                    del_op = change_set->user;
                else if (change_set->user == IPNET_MCAST_RECORD_TYPE_CHANGE_TO_INCLUDE
                         || change_set->user == IPNET_MCAST_RECORD_TYPE_ALLOW_NEW_SOURCES)
                    add_op = change_set->user;
            }

            if (new_fm == IPNET_MCAST_RECORD_TYPE_IS_EXCLUDE)
            {
                if (change_set->user == IPNET_MCAST_RECORD_TYPE_BLOCK_OLD_SOURCES)
                    add_op = change_set->user;
                else if (change_set->user == IPNET_MCAST_RECORD_TYPE_ALLOW_NEW_SOURCES)
                    del_op = change_set->user;
            }
        }

        report_fm = is_add ? add_op : del_op;

        if (change_set != IP_NULL)
        {
            if (report_fm != change_set->user)
                /* Reports could not be merged */
                ipcom_set_delete(change_set);
            else
            {
                /* The previous report could be merged with the new,
                   'change_set' must be first so the new changes is added
                   at the end. The new changes will then be first in the report
                   that is sent out. */
                ipcom_set_union(change_set, this_change);
                ipcom_set_delete(this_change);
                this_change = change_set;
            }
        }

        change_set = this_change;
    }

    change_set->user = report_fm;
    macb->filter_change_sources = change_set;
    send_report = IP_TRUE;

 done:
    ipcom_set_delete(if_filter);
    return send_report;
}


/*
 *===========================================================================
 *                    ipnet_mcast_time_to_msec
 *===========================================================================
 * Description: Convert the time value used by IGMPv3 and MLDv2 to milliseconds.
 * Parameters:  t - The time field.
 * Returns:     The time in milliseconds.
 *
 */
IP_GLOBAL Ip_u32
ipnet_mcast_time_to_msec(Ip_u8 t)
{
    if (t < 128)
        return t * 1000;

    /*       0 1 2 3 4 5 6 7
             +-+-+-+-+-+-+-+-+
             |1| exp | mant  |
             +-+-+-+-+-+-+-+-+
             Time = (mant | 0x10) << (exp + 3) */
    return 1000 * ((t & 0xf) | 0x10) << (((t & 0x70) >> 4) + 3);
}
#endif /* IPNET_USE_SOURCE_SPECIFIC_MCAST */

/*
 ****************************************************************************
 * 11                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

