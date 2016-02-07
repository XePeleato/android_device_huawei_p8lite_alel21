/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_list.c,v $ $Revision: 1.15 $
 *     $Source: /home/interpeak/CVSRoot/ipcom/src/ipcom_list.c,v $
 *     $Author: keithw $ $Date: 2006-06-05 07:15:03 $
 *     $State: Exp $ $Locker:  $
 *
 *     INTERPEAK_COPYRIGHT_STRING
 *     Design and implementation by Lennart Bang <lob@interpeak.se>
 ******************************************************************************
 */

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

#define IPCOM_USE_CLIB_PROTO
#include "ipcom_list.h"
#include "ipcom_clib.h"


/*
 ****************************************************************************
 * 4                    EXTERN PROTOTYPES
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 5                    DEFINES
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 6                    TYPES
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
 * 9                    FUNCTIONS
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
IP_PUBLIC void
ipcom_list_init(Ipcom_list *head)
{
    head->next = head;
    head->prev = head;
    head->head = head;
    head->size = 0;
}


/*
 *===========================================================================
 *                         ipcom_list_assert
 *===========================================================================
 * Description:   Assert a list header and/or list entry.
 * Parameters:    head  - A pointer to the list header or IP_NULL if unknown.
 *                entry - A pointer to the list entry or IP_NULL if none.
 * Returns:       .
 *
 */
#ifdef IP_ASSERT
IP_STATIC void
ipcom_list_assert(Ipcom_list *head, Ipcom_list *entry)
{
    /* Assert the list entry. */
    if(entry != IP_NULL)
    {
        /* If no head supplied, pick this entry's head. */
        if(head == IP_NULL)
            head = entry->head;
        ip_assert(entry->head != IP_NULL);
        ip_assert(entry->next != IP_NULL);
        ip_assert(entry->prev != IP_NULL);
        ip_assert(entry->head == head);
        /* unlink_head() results in an entry being the head.
           if lone head-entry ignore below checks. */
        if(head->size > 0)
        {
            ip_assert(entry->head != entry);
            ip_assert(entry->next != entry);
            ip_assert(entry->prev != entry);
            ip_assert(entry->prev->next == entry);
            ip_assert(entry->next->prev == entry);
        }
    }

    /* Assert the list header. */
    if(head != IP_NULL)
    {
        ip_assert(head->head == head);
        ip_assert(head->next != IP_NULL);
        ip_assert(head->prev != IP_NULL);
        ip_assert(head->size != 0 || (head->next == head && head->prev == head));

        if(head->size > 0)
        {
            Ip_u32     size;
            Ipcom_list *tmp, *prev;
            Ip_bool  found;

            size  = 0;
            found = entry ? IP_FALSE : IP_TRUE;

            for(prev = head, tmp = head->next; tmp != head; prev = tmp, tmp = tmp->next)
            {
                ip_assert(tmp->head == head);
                ip_assert(tmp->prev == prev);
                ip_assert(tmp->prev->next == tmp);
                ip_assert(tmp->next != tmp);
                size++;
                if(entry == tmp)
                    found = IP_TRUE;
            }
            ip_assert(size == head->size);
            ip_assert(found == IP_TRUE);
        }
    }
}
#else
#define ipcom_list_assert(xxhead, xxentry)   do { } while((0))
#endif


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
IP_PUBLIC Ipcom_list *
ipcom_list_first(Ipcom_list *head)
{
    ipcom_list_assert(head, IP_NULL);

    if(head->next != head)
        return head->next;
    else
        return IP_NULL;
}


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
IP_PUBLIC Ipcom_list *
ipcom_list_last(Ipcom_list *head)
{
    ipcom_list_assert(head, IP_NULL);

    if(head->prev != head)
        return head->prev;
    else
        return IP_NULL;
}


/*
 *===========================================================================
 *                         ipcom_list_next
 *===========================================================================
 * Description:   Get the next list entry following 'entry'.
 * Parameters:    head - Pointer to the list entry.
 * Returns:       Pointer to the next entry in the list or IP_NULL if none.
 *
 */
IP_PUBLIC Ipcom_list *
ipcom_list_next(Ipcom_list *entry)
{
    ipcom_list_assert(IP_NULL, entry);

    if(entry->next != entry->head)
        return entry->next;
    else
        return IP_NULL;
}


/*
 *===========================================================================
 *                         ipcom_list_prev
 *===========================================================================
 * Description:   Get the previous list entry to 'entry'.
 * Parameters:    head - Pointer to the list entry.
 * Returns:       Pointer to the previous entry or IP_NULL if none.
 *
 */
IP_PUBLIC Ipcom_list *
ipcom_list_prev(Ipcom_list *entry)
{
    ipcom_list_assert(IP_NULL, entry);

    if(entry->prev != entry->head)
        return entry->prev;
    else
        return IP_NULL;
}


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
IP_PUBLIC void
ipcom_list_insert_first(Ipcom_list *head, Ipcom_list *entry)
{
#ifdef IP_ASSERT
    {
        Ipcom_list *tmp;
        for(tmp = head->next; tmp != head; tmp = tmp->next)
            ip_assert(entry != tmp);
    }
#endif

    entry->next = head->next;
    entry->prev = head;
    entry->next->prev = entry;
    entry->prev->next = entry;

    entry->head = head;
    head->size++;

    ipcom_list_assert(head, entry);
}


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
IP_PUBLIC void
ipcom_list_insert_last(Ipcom_list *head, Ipcom_list *entry)
{
#ifdef IP_ASSERT
    {
        Ipcom_list *tmp;
        for(tmp = head->next; tmp != head; tmp = tmp->next)
            ip_assert(entry != tmp);
    }
#endif

    entry->prev = head->prev;
    entry->next = head;
    entry->next->prev = entry;
    entry->prev->next = entry;

    entry->head = head;
    head->size++;

    ipcom_list_assert(head, entry);
}


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
IP_PUBLIC void
ipcom_list_insert_before(Ipcom_list *inlist, Ipcom_list *entry)
{
#ifdef IP_ASSERT
    {
        Ipcom_list *tmp;
        for(tmp = inlist->head->next; tmp != inlist->head; tmp = tmp->next)
            ip_assert(entry != tmp);
    }
#endif

    entry->next = inlist;
    entry->prev = inlist->prev;

    entry->next->prev = entry;
    entry->prev->next = entry;

    entry->head = inlist->head;
    entry->head->size++;

    ipcom_list_assert(IP_NULL, entry);
}


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
IP_PUBLIC void
ipcom_list_insert_after(Ipcom_list *inlist, Ipcom_list *entry)
{
#ifdef IP_ASSERT
    {
        Ipcom_list *tmp;
        for(tmp = inlist->head->next; tmp != inlist->head; tmp = tmp->next)
            ip_assert(entry != tmp);
    }
#endif

    entry->prev = inlist;
    entry->next = inlist->next;

    entry->next->prev = entry;
    entry->prev->next = entry;

    entry->head = inlist->head;
    entry->head->size++;

    ipcom_list_assert(IP_NULL, entry);
}


/*
 *===========================================================================
 *                    ipcom_list_remove
 *===========================================================================
 * Description:   Remove list entry pointer to by 'entry' from its list.
 * Parameters:    entry - Pointer to the list entry to remove.
 * Returns:       .
 *
 */
IP_PUBLIC void
ipcom_list_remove(Ipcom_list *entry)
{
    ipcom_list_assert(IP_NULL, entry);
    ip_assert(entry->head->size > 0);

    entry->next->prev = entry->prev;
    entry->prev->next = entry->next;

    entry->head->size--;
    entry->head = IP_NULL;
}


/*
 *===========================================================================
 *                    ipcom_list_unlink_head
 *===========================================================================
 * Description:   Unlink 'head' and make the next entry the new 'head'.
 * Parameters:    head  - Pointer to the list header to unlink.
 * Returns:       Pointer to the new head or IP_NULL if list is empty.
 *
 */
IP_PUBLIC Ipcom_list *
ipcom_list_unlink_head(Ipcom_list *head)
{
    Ipcom_list   *new_head;

    ipcom_list_assert(head, IP_NULL);

    new_head = head->next;
    if(new_head == head)
        return IP_NULL;

    new_head->head = new_head;
    new_head->size = head->size - 1;
    new_head->prev = head->prev;
    new_head->prev->next = new_head;

    /* Restore the head pointer for all the members. */
    for(head = new_head->next; head != new_head; head = head->next)
        head->head = new_head;

    ipcom_list_assert(new_head, IP_NULL);
    return new_head;
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

