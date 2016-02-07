/* ipcom_hash.h - API of IPCOM Hash Table */

/*
 ******************************************************************************
 *                     INTERPEAK API HEADER FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_hash.h,v $ $Revision: 1.19 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/include/ipcom_hash.h,v $
 *   $Author: kenneth $ $Date: 2008-12-10 20:44:19 $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Kenneth Jonsson <kenneth@interpeak.se>
 ******************************************************************************
 */
#ifndef IPCOM_HASH_H
#define IPCOM_HASH_H

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 */

/*
DESCRIPTION
This library contains the API for IPCOM Hash Table
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

#if !defined(IPCOM_KERNEL) && defined(IPCOM_USRLIB)
#define ipcom_hash_new ipcom_hash_new_usr
#define ipcom_hash_delete ipcom_hash_delete_usr
#define ipcom_hash_add ipcom_hash_add_usr
#define ipcom_hash_remove ipcom_hash_remove_usr
#define ipcom_hash_get ipcom_hash_get_usr
#define ipcom_hash_for_each ipcom_hash_for_each_usr
#define ipcom_hash_update ipcom_hash_update_usr
#endif


/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    Ipcom_hash_obj_func
 *===========================================================================
 * Description: Computes a hash value for the (used defined) object.
 * Parameters:  obj - The object to calculate the hashvalue for.
 * Returns:     The hash value
 *
 */
typedef unsigned (*Ipcom_hash_obj_func)(const void *obj);

/*
 *===========================================================================
 *                    Ipcom_hash_key_func
 *===========================================================================
 * Description: Computes a hash value for the (used defined) search key.
 * Parameters:  key - The key to calculate the hashvalue for.
 * Returns:     The hash value
 *
 */
typedef unsigned (*Ipcom_hash_key_func)(const void *key);

/*
 *===========================================================================
 *                    Ipcom_hash_cmp_func
 *===========================================================================
 * Description: Compares the (user defined) object with the (used defined)
 *              search key. This must always be true:
 *                  if Ipcom_hash_cmp_func(obj, key) == IP_TRUE
 *                  then Ipcom_hash_obj_func(obj) == Ipcom_hash_key_func(key)
 * Parameters:  obj - An object that can/is stored in the hash table.
 *              key - The key used when making lookups in the hash table.
 * Returns:     IP_TRUE if the key matches the object, IP_FALSE otherwise.
 *
 */
typedef Ip_bool (*Ipcom_hash_cmp_func)(const void *obj, const void *key);

/*
 *===========================================================================
 *                    Ipcom_hash_foreach_cb_func
 *===========================================================================
 * Description: Callback function that is called for each element in the hash
 *              table when using ipcom_hash_for_each().
 * Parameters:  obj - A object in the hash table.
 *              data - User defined data passed to ipcom_hash_for_each().
 * Returns:
 *
 */
typedef void (*Ipcom_hash_foreach_cb_func)(void *obj, void *data);

/*
 *===========================================================================
 *                         Ipcom_hash
 *===========================================================================
 * List head and entry structure.
 */
typedef struct Ipcom_hash_struct
{
    /* Function that creates a hash value from an object */
    Ipcom_hash_obj_func   obj_hash_func;

    /* Pointer to a function that creates a hash value from a search key */
    Ipcom_hash_key_func   key_hash_func;

    /* Function that compares an objects and a search key and return the result */
    Ipcom_hash_cmp_func   obj_key_cmp;

    unsigned  elem;     /* The number of elements currently in the table */
    unsigned  size;     /* The size of the hash table */
    void    **table;    /* The hash table */
}
Ipcom_hash;


/*
 ****************************************************************************
 * 6                    FUNCTIONS
 ****************************************************************************
 */

/*******************************************************************************
*
* ipcom_hash_new - create a new hash table
*
* This routine creates a new hash table.
*
* Parameters:
* \is
* \i <obj_hash>
* A function that creates a hash value for object <obj>.
* \i <key_hash>
* A function that creates a hash value for search key <key>.
* \i <cmp>
* A function that will evaluate to 'IP_TRUE' if <key> is the search key that
* matches <obj>.
* \ie
*
* RETURNS: Pointer to the new hash table or 'IP_NULL' if not enough memory could
* be allocated.
*
* ERRNO:
*/
IP_PUBLIC Ipcom_hash *ipcom_hash_new
(
    Ipcom_hash_obj_func obj_hash_func,
    Ipcom_hash_key_func key_hash_func,
    Ipcom_hash_cmp_func obj_key_cmp
);


/*******************************************************************************
*
* ipcom_hash_delete - free the hash table
*
* This routine frees the hash table. Not that the object stored in the hash
* table is not freed.
*
* Parameter:
* \is
* \i <head>
* The hash table head (returned from ipcom_hash_new()).
* \ie
*
* RETURNS: \&N/A.
*
* ERRNO:
*/
IP_PUBLIC void ipcom_hash_delete
(
    Ipcom_hash *head
);


/*******************************************************************************
*
* ipcom_hash_add - add an object to the hash table
*
* This routine adds an object to the hash table.
*
* Parameters:
* \is
* \i <hash>
* The hash table the object is added to.
* \i <obj>
* The object to add.
* \ie
*
* RETURNS: 'IPCOM_SUCCESS', or one of the following errors:
* \is
* \i 'IPCOM_ERR_DUPLICATE'
* If the object was already in the hash table.
* \i 'IPCOM_ERR_NO_MEMORY'
* If the hash table failed to increase in size. The hash table will grow when it
* gets too densely populated.
* \ie
*
* ERRNO:
*/
IP_PUBLIC Ip_err ipcom_hash_add
(
    Ipcom_hash *head,
    void *obj
);


/*******************************************************************************
*
* ipcom_hash_remove - remove an object from the hash table
*
* This routine removes an object from the hash table. Note that the resources
* held by the object are not freed.
*
* Parameters:
* \is
* \i <head>
* The hash table the object will be removed from.
* \i <obj>
* The object to remove.
* \ie
*
* RETURNS:
*
* ERRNO:
*/
IP_PUBLIC Ip_err ipcom_hash_remove
(
    Ipcom_hash *head,
    void *obj
);


/*******************************************************************************
*
* ipcom_hash_get - return the matching object
*
* This routine returns the object, if there is one, that matches the <key>
* parameter.
*
* Parameters:
* \is
* \i <head>
* The hash table to search.
* \i <key>
* The search key to use when looking for the object.
* \ie
*
* RETURNS: A pointer to the object, or 'IP_NULL' if no object exists that
* matches <key>.
*
* ERRNO:
*/
IP_FASTTEXT IP_PUBLIC void *ipcom_hash_get
(
    Ipcom_hash *head,
    const void *key
);


/*******************************************************************************
*
* ipcom_hash_for_each - call cb_func
*
* This routine calls cb_func() for each object in the hash table. Note that
* the order of the objects must be considered completely random.
*
* Parameters:
* \is
* \i <head>
* The hash table to traverse.
* \i <cb_func>
* The function to call for each object.
* \i <data>
* User define data that will passed as second argument to cb_func().
* \ie
*
* RETURNS:
*
* ERRNO:
*/
IP_PUBLIC void ipcom_hash_for_each
(
    Ipcom_hash *head,
    Ipcom_hash_foreach_cb_func cb_func,
    void *data
);


/*******************************************************************************
*
* ipcom_hash_update - create a hash value from the key
*
* This routine creates a hash value from a key given in the <key> parameter. The
* function can be called repeatedly to include any number of keys in the final
* hash value.
*
* Parameters:
* \is
* \i <key>
* The key to hash.
* \i <key_len>
* The length of the key, in octets.
* \i <initval>
* The previous hash value, or any value, if this is the first time the function
* is called. The value used in the first call is constant for the lifetime of
* the hash table.
* \ie
*
* EXAMPLE
* \cs
* struct My_struct
* {
*     int a;
*      /@ Data that we do not want to include
*          in the hash value @/
*     int b;
* };
*
* /@ Example implementation for the obj_hash() function @/
* unsigned obj_hash(struct My_struct *m)
* {
*     unsigned h;
*     /@ The initval can be any value in the first call @/
*     h = ipcom_hash_update(&m->a, sizeof(m->a), 0x2653145);
*     /@ The initval must be the previous hash value in
*        consecutive calls @/
*     h = ipcom_hash_update(&m->b, sizeof(m->b), h);
*     return h;
* }
* \ce
*
* RETURNS: The hash value for the key
*
* ERRNO:
*/
IP_PUBLIC unsigned ipcom_hash_update
(
    const void *key,
    int key_len,
    unsigned initval
);


#ifdef __cplusplus
}
#endif

#endif


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
