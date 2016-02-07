/*
 ******************************************************************************
 *                     INTERPEAK API HEADER FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_tree.h,v $ $Revision: 1.7 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/include/ipcom_tree.h,v $
 *   $Author: kenneth $ $Date: 2004-11-17 14:33:25 $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Kenneth Jonsson <kenneth@interpeak.se>
 ******************************************************************************
 */
#ifndef IPCOM_TREE_H
#define IPCOM_TREE_H

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 */

/* IPCOM red-black tree */

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

#include <ipcom_type.h>
#include <ipcom_cstyle.h>


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
 *                         Ipcom_tree_element
 *===========================================================================
 * An element (also called node) in the tree
 */
typedef struct Ipcom_tree_element_struct
{
    /* The parent node */
    struct Ipcom_tree_element_struct *parent;
    /* The part of the subtree that has a key that is less than this node */
    struct Ipcom_tree_element_struct *left;
    /* The part of the subtree that has a key that is greater than this node */
    struct Ipcom_tree_element_struct *right;
    /* IP_TRUE if this node is red, IP_FALSE if its black */
    Ip_bool                           is_red;
}
Ipcom_tree_element;


/*
 *===========================================================================
 *                    Ipcom_tree_elem_cmp
 *===========================================================================
 * Description: Compares two elemetns in the tree.
 * Parameters:  e1 - A element in the tree.
 *              e2 - Another element in the tree.
 * Returns:     -1 - if the key for 'e1' is less than the 'e2' key
 *               0  - if the keys are equal
 *               1  - if the key for 'e1' is greater than the 'e2' key
 *
 */
typedef int (*Ipcom_tree_elem_cmp)(const Ipcom_tree_element *e1, const Ipcom_tree_element *e2);


/*
 *===========================================================================
 *                    Ipcom_tree_key_cmp
 *===========================================================================
 * Description: Compares two elemetns in the tree.
 * Parameters:  e - A element in the tree.
 *              key - The search key that the element must match.
 * Returns:     -1 - if the key for 'e1' is less than the 'e2' key
 *               0  - if the keys are equal
 *               1  - if the key for 'e1' is greater than the 'e2' key
 *
 */
typedef int (*Ipcom_tree_key_cmp)(const Ipcom_tree_element *e, const void *key);


/*
 *===========================================================================
 *                    Ipcom_tree_for_each_cb
 *===========================================================================
 * Description: Callback function to call for each element.
 * Parameters:  e - A element in the tree.
 *              data - User defined data that will be passed each time.
 * Returns:
 *
 */
typedef void (*Ipcom_tree_for_each_cb)(const Ipcom_tree_element *e, void *data);


/*
 *===========================================================================
 *                         Ipcom_tree
 *===========================================================================
 * Tree head and entry structure.
 */
typedef struct Ipcom_tree_struct
{
    Ipcom_tree_elem_cmp elem_cmp;
    Ipcom_tree_key_cmp  key_cmp;
    /* Root element, right pointer is the head of the actual tree */
    Ipcom_tree_element  root;
    /* Sentinel */
    Ipcom_tree_element  nil;
}
Ipcom_tree;

/*
 ****************************************************************************
 * 6                    FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipcom_tree_new
 *===========================================================================
 * Description: Creates a new red-black tree.
 * Parameters:  elem_cmp - function that compares the keys of two elements
 *                         in the tree.
 *              key_cmp  - function that compares the key of the first
 *                         element with the specified key.
 * Returns:     The new tree or IP_NULL if out of memory.
 *
 */
IP_PUBLIC Ipcom_tree *
ipcom_tree_new(Ipcom_tree_elem_cmp elem_cmp, Ipcom_tree_key_cmp key_cmp);


/*
 *===========================================================================
 *                    ipcom_tree_delete
 *===========================================================================
 * Description: Delete the tree. NOTE the elements in the tree
 *              is NOT freed by this function.
 * Parameters:  tree - The tree to free.
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_tree_delete(Ipcom_tree *tree);


/*
 *===========================================================================
 *                    ipcom_tree_insert
 *===========================================================================
 * Description: Inserts an element into the red-black tree.
 * Parameters:  tree - The tree in which the element should be inserted.
 *              elem - The element to insert.
 * Returns:     IPCOM_SUCCESS       - operation successful.
 *              IPCOM_ERR_DUPLICATE - operation failed since an element with
 *                                    the same key already exist
 *
 */
IP_PUBLIC Ip_err
ipcom_tree_insert(Ipcom_tree *tree, Ipcom_tree_element *elem);


/*
 *===========================================================================
 *                    ipcom_tree_remove
 *===========================================================================
 * Description: Removes an element from the tree.
 * Parameters:  tree - the tree from which the element should be removed
 *              elem - the element to remove.
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_tree_remove(Ipcom_tree *tree, Ipcom_tree_element *elem);


/*
 *===========================================================================
 *                    ipcom_tree_get
 *===========================================================================
 * Description: Returns the node matching the key.
 * Parameters:  tree - The tree in which the element should be searched.
 * Returns:     The element with the specified key or IP_NULL if no such
 *              node exists.
 *
 */
IP_PUBLIC Ipcom_tree_element *
ipcom_tree_get(Ipcom_tree *tree, const void *key);


/*
 *===========================================================================
 *                    ipcom_tree_for_each
 *===========================================================================
 * Description: Calls the suppled function for each object in the tree
 * Parameters:  tree - the tree to traverse
 *              cb_func - Function to call for each element, with 'elem'
 *              set to the current object and 'data' is the values passed
 *              as 3:rd argument to this function.
 *              data - User defined data.
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_tree_for_each(Ipcom_tree *tree,
                    Ipcom_tree_for_each_cb cb_func,
                    void *data);


/*
 *===========================================================================
 *                       ipcom_tree_first
 *===========================================================================
 * Description: Returns the first entry in the tree, i.e. the entry that always
 *              will give <0 by the comparison function when compared to any
 *              other element in the tree.
 * Parameters:  tree - The tree to return the first entry for.
 * Returns:     The first entry in the tree.
 *
 */
IP_PUBLIC Ipcom_tree_element *
ipcom_tree_first(Ipcom_tree *tree);


/*
 *===========================================================================
 *                    ipcom_tree_successor
 *===========================================================================
 * Description: Returns the the element that is located after the
 *              specified node if an in-order tree walk was performed.
 * Parameters:  tree - the tree 'x' is part of
 *              x - The node for which the successor should be returned,
 *                  use the root of the tree to get the first element of the
 *                  tree.
 * Returns:     The successor to 'x'
 *
 */
IP_PUBLIC Ipcom_tree_element *
ipcom_tree_successor(Ipcom_tree *tree, Ipcom_tree_element *x);

#ifdef __cplusplus
}
#endif

#endif /* IPCOM_TREE_H */

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
