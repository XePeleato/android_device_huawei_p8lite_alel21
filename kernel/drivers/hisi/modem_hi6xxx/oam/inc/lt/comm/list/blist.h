/*
 * list.h
 * list operations
 *
 * copied from Linux_Kernel_Dir/include/linux/list.h
 */

#ifndef _LIST_S_H
#define _LIST_S_H

struct list_header {
    struct list_header *next;
    struct list_header *prev;
};


typedef struct list_header LIST_S;

//#define LIST_HEAD_INIT(name) { &(name), &(name) }


/* init a list head */
#define blist_head_init(ptr) do { \
    (ptr)->next = (ptr); (ptr)->prev = (ptr); \
} while (0)


/**
 * blist_entry - get the struct for this entry
 * @ptr:    the &struct list_header pointer.
 * @type:    the type of the struct this is embedded in.
 * @member:    the name of the list_struct within the struct.
 */
#define blist_entry(ptr, type, member) \
    ((type *)((char *)(ptr)-(unsigned long)(&((type *)16)->member)+16))

/**
 * blist_for_each    -    iterate over a list
 * @pos:    the &struct list_header to use as a loop counter.
 * @head:    the head for your list.
 */
#define blist_for_each(pos, head) \
    for (pos = (head)->next; pos != (head); pos = pos->next)


extern void __blist_add(struct list_header * newnew,
        struct list_header * prev,
        struct list_header * next);

extern void blist_add_tail(struct list_header *newnew, struct list_header *head);

extern void __blist_del(struct list_header * prev,
        struct list_header * next);

extern void blist_del(struct list_header *entry);



#endif /* _MG_LIST_H */


