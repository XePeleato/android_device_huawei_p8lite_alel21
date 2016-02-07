/*
 * list.c
 * list operations
 *
 * copied from Linux_Kernel_Dir/include/linux/list.h
 */

#include "blist.h"

 /* Insert a new entry between two known consecutive entries.
This is only for internal list manipulation where we know
the prev/next entries already!*/
void __blist_add(struct list_header * newnew,
        struct list_header * prev,
        struct list_header * next)
{
    next->prev = newnew;
    newnew->next = next;
    newnew->prev = prev;
    prev->next = newnew;
}

void blist_add_tail(struct list_header *newnew, struct list_header *head)
{
    __blist_add(newnew, head->prev, head);
}

/*
 * Delete a list entry by making the prev/next entries
 * point to each other.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
void __blist_del(struct list_header * prev,
        struct list_header * next)
{
    next->prev = prev;
    prev->next = next;
}

/*lint -save -e818*/
/**
 * msp_list_del - deletes entry from list.
 * @entry: the element to delete from the list.
 * Note: msp_list_empty on entry does not return true after this, the entry is in an undefined state.
 */
void blist_del(struct list_header *entry)
{
    __blist_del(entry->prev, entry->next);
}
/*lint -restore*/



