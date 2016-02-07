/*
 ******************************************************************************
 *                     HEADER FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_list.h,v $ $Revision: 1.9 $
 *     $Source: /home/interpeak/CVSRoot/ipcom/include/ipcom_list.h,v $
 *     $Author: kenneth $ $Date: 2008-03-14 12:51:02 $
 *     $State: Exp $ $Locker:  $
 *
 *     INTERPEAK_COPYRIGHT_STRING
 *     Design and implementation by Lennart Bang <lob@interpeak.se>
 ******************************************************************************
 */
#ifndef IPCOM_LIST_H
#define IPCOM_LIST_H

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 * Interpeak implementation of a doubly linked list.
 */

/*
 ****************************************************************************
 * 2                    CONFIGURATION
 ****************************************************************************
 */

#include "ipcom_config.h"


/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */
#include "ipcom_type.h"
#include "ipcom_cstyle.h"

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

/*
 *===========================================================================
 *                         Ipcom_list
 *===========================================================================
 * List head and entry structure.
 */
typedef struct Ipcom_list_struct
{
  struct Ipcom_list_struct   *next;
  struct Ipcom_list_struct   *prev;
  struct Ipcom_list_struct   *head;
  Ip_u32                      size;
}
Ipcom_list;


/*
 *===========================================================================
 *                      ipcom_list_entry
 *===========================================================================
 * Description: Returns the element that contains the Ipcom_list
 *                      member. NOTE: it is not strictly necessary to
 *                      use this macro if the Ipcom_list entry is the
 *                      first member of the containing type, but it is
 *                      good practice to do so even in that case.
 * Parameters:  node - pointer returned from ipcom_list_first(),
 *                      ipcom_list_last() or ipcom_list_next()
 *              type - type of the structure that contains the
 *                      Ipcom_list node.
 *              list_member - name of the member in the 'type'
 *                      structure that is Ipcom_list.
 * Returns:     The element that contains the list member.
 *
 */
#define ipcom_list_entry(node, type, list_member)   \
    ((type *)((char*)(node) - ip_offsetof(type, list_member)))


/*
 ****************************************************************************
 * 6                    FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipcom_list_init
 *===========================================================================
 * Description:   Initialize a list header.
 * Parameters:    head - The uninitialized list head pointer.
 * Returns:       .
 *
 */
IP_PUBLIC void ipcom_list_init(Ipcom_list *head);


/*
 *===========================================================================
 *                         ipcom_list_first
 *===========================================================================
 * Description:   Get the first list entry in list 'head'.
 * Parameters:    head - Pointer to the list header.
 * Returns:       Pointer to the first entry in the list or IP_NULL if list
 *                is empty.
 *
 */
IP_PUBLIC Ipcom_list *ipcom_list_first(Ipcom_list *head);

#define IPCOM_LIST_FIRST(xxhead)   (void *)ipcom_list_first(xxhead)


/*
 *===========================================================================
 *                         ipcom_list_last
 *===========================================================================
 * Description:   Get the last list entry in list 'head'.
 * Parameters:    head - Pointer to the list header.
 * Returns:       Pointer to the last entry in the list or IP_NULL if list
 *                is empty.
 *
 */
IP_PUBLIC Ipcom_list *ipcom_list_last(Ipcom_list *head);

#define IPCOM_LIST_LAST(xxhead)   (void *)ipcom_list_last(xxhead)


/*
 *===========================================================================
 *                         ipcom_list_next
 *===========================================================================
 * Description:   Get the next list entry following 'entry'.
 * Parameters:    head - Pointer to the list entry.
 * Returns:       Pointer to the next entry in the list or IP_NULL if none.
 *
 */
IP_PUBLIC Ipcom_list *ipcom_list_next(Ipcom_list *entry);

#define IPCOM_LIST_NEXT(xxentry)   (void *)ipcom_list_next(xxentry)


/*
 *===========================================================================
 *                         ipcom_list_prev
 *===========================================================================
 * Description:   Get the previous list entry to 'entry'.
 * Parameters:    head - Pointer to the list entry.
 * Returns:       Pointer to the previous entry or IP_NULL if none.
 *
 */
IP_PUBLIC Ipcom_list *ipcom_list_prev(Ipcom_list *entry);

#define IPCOM_LIST_PREV(xxentry)   (void *)ipcom_list_prev(xxentry)


/*
 *===========================================================================
 *                    ipcom_list_insert_first
 *===========================================================================
 * Description:   Insert list 'entry' first in the list 'head'.
 * Parameters:    head  - Pointer to the list header to insert 'entry' in.
 *                entry - Pointer to the list entry to insert first.
 * Returns:       .
 *
 */
IP_PUBLIC void ipcom_list_insert_first(Ipcom_list *head, Ipcom_list *entry);


/*
 *===========================================================================
 *                    ipcom_list_insert_last
 *===========================================================================
 * Description:   Insert list 'entry' last in the list 'head'.
 * Parameters:    head  - Pointer to the list header to insert 'entry' in.
 *                entry - Pointer to the list entry to insert last.
 * Returns:       .
 *
 */
IP_PUBLIC void ipcom_list_insert_last(Ipcom_list *head, Ipcom_list *entry);


/*
 *===========================================================================
 *                    ipcom_list_insert_before
 *===========================================================================
 * Description:   Insert the new list 'entry' before the entry 'inlist' which
 *                is already in a list.
 * Parameters:    inlist - Pointer to the list entry already in a list.
 *                entry  - Pointer to the new list entry to insert before
 *                         'inlist'.
 * Returns:       .
 *
 */
IP_PUBLIC void ipcom_list_insert_before(Ipcom_list *inlist, Ipcom_list *entry);


/*
 *===========================================================================
 *                    ipcom_list_insert_after
 *===========================================================================
 * Description:   Insert the new list 'entry' after the entry 'inlist' which
 *                is already in a list.
 * Parameters:    inlist - Pointer to the list entry already in a list.
 *                entry  - Pointer to the new list entry to insert after
 *                         'inlist'.
 * Returns:       .
 *
 */
IP_PUBLIC void ipcom_list_insert_after(Ipcom_list *inlist, Ipcom_list *entry);


/*
 *===========================================================================
 *                    ipcom_list_remove
 *===========================================================================
 * Description:   Remove list entry pointer to by 'entry' from its list.
 * Parameters:    entry - Pointer to the list entry to remove.
 * Returns:       .
 *
 */
IP_PUBLIC void ipcom_list_remove(Ipcom_list *entry);


/*
 *===========================================================================
 *                    ipcom_list_unlink_head
 *===========================================================================
 * Description:   Unlink 'head' and make the first entry the new 'head'.
 * Parameters:    head  - Pointer to the list header to unlink.
 * Returns:       Pointer to the new head or IP_NULL if list was empty.
 *
 */
IP_PUBLIC Ipcom_list *ipcom_list_unlink_head(Ipcom_list *head);

#ifdef __cplusplus
}
#endif

#endif

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

