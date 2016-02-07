/*
 * list.h
 * list operations
 *
 * copied from Linux_Kernel_Dir/include/linux/list.h
 */

#ifndef _HI_LIST_H
#define _HI_LIST_H

struct msp_list_header {
    struct msp_list_header *next;
    struct msp_list_header *prev;
};


typedef struct msp_list_header HI_LIST_S;

#define HI_LIST_HEAD_INIT(name) { &(name), &(name) }


/* init a list head */
#define HI_INIT_LIST_HEAD(ptr) do { \
    (ptr)->next = (ptr); (ptr)->prev = (ptr); \
} while (0)


/**
 * msp_list_entry - get the struct for this entry
 * @ptr:    the &struct msp_list_header pointer.
 * @type:    the type of the struct this is embedded in.
 * @member:    the name of the list_struct within the struct.
 */
#define msp_list_entry(ptr, type, member) \
    ((type *)((char *)(ptr)-(unsigned long)(&((type *)16)->member)+16))

/**
 * msp_list_for_each    -    iterate over a list
 * @pos:    the &struct msp_list_header to use as a loop counter.
 * @head:    the head for your list.
 */
#define msp_list_for_each(pos, head) \
    for (pos = (head)->next; pos != (head); pos = pos->next)

/**
 * msp_list_for_each_safe    -    iterate over a list safe against removal of list entry
 * @pos:    the &struct msp_list_header to use as a loop counter.
 * @n:        another &struct msp_list_header to use as temporary storage
 * @head:    the head for your list.
 */
#define msp_list_for_each_safe(pos, n, head) \
    for (pos = (head)->next, n = pos->next; pos != (head); \
            pos = n, n = pos->next)

/**
 * msp_list_for_each_ex    -    iterate over a list
 * @pos:    the &struct msp_list_header to use as a loop counter.
 * @head:   the head for your list.
 * @begin:  the previous item of the begining item
 */
#define msp_list_for_each_ex(pos, head, begin) \
    for (pos = (begin)->next; pos != (head); pos = (pos)->next)

/**
 * msp_list_for_index    -    iterate over a list for index
 * @pos:    the &struct msp_list_header to use as a loop counter.
 * @head:    the head for your list.
 */
#define msp_list_for_index(pos, i, head, index) \
    for (pos = (head)->next, i=0; (pos != (head) && i < index); pos = pos->next,i++)

/*lint -esym(752,__list_add_msp)*/
extern void __list_add_msp(struct msp_list_header * newnew,
        struct msp_list_header * prev,
        struct msp_list_header * next);

/*lint -esym(752,msp_list_add_tail)*/
extern void msp_list_add_tail(struct msp_list_header *newnew, struct msp_list_header *head);

/*lint -esym(752,__list_del_msp)*/
extern void __list_del_msp(struct msp_list_header * prev,
        struct msp_list_header * next);

/*lint -esym(752,msp_list_del)*/
extern void msp_list_del(struct msp_list_header *entry);


#endif /* _MG_LIST_H */


