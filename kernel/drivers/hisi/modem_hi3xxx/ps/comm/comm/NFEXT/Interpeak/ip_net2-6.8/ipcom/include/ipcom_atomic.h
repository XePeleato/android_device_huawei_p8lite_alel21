/*
 ******************************************************************************
 *                     HEADER FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_atomic.h,v $ $Revision: 1.3 $
 *     $Source: /home/interpeak/CVSRoot/ipcom/include/ipcom_atomic.h,v $
 *     $Author: kenneth $ $Date: 2007-06-14 18:03:19 $
 *     $State: Exp $ $Locker:  $
 *
 * INTERPEAK_COPYRIGHT_STRING
 * Design and implementation by Kenneth Jonsson <kenneth.jonsson@windriver.com>
 ******************************************************************************
 */
#ifndef IPCOM_ATOMIC_H
#define IPCOM_ATOMIC_H

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 */

/*
DESCRIPTION
API for IPCOM atomic types
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

#include "ipcom_os.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

/* The size of type is very dependent on the architecture */
typedef struct { volatile int val; } Ipcom_atomic_t;

#ifdef IPCOM_USE_ATOMIC_MACROS
/* These macros works fine with the generic IPCOM implementation
   <IPCOM>/port/src/ipcom_atomic.c */
# define ipcom_atomic_inc(v)               ipcom_atomic_add(v, 1)
# define ipcom_atomic_dec(v)               ipcom_atomic_add(v, -1)
# define ipcom_atomic_sub(v, i)            ipcom_atomic_add(v, -(i))
# define ipcom_atomic_sub_and_return(v, i) ipcom_atomic_add_and_return(v, -(i))
#endif /* IPCOM_USE_ATOMIC_MACROS */

/*
 ****************************************************************************
 * 6                 PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*******************************************************************************
 *
 * ipcom_atomic_get - get a value
 *
 * This routine get a value from a atomic variable.
 *
 * Parameters:
 * \is
 * \i <v>
 * Pointer to an atmomic variable
 * \ie
 *
 * RETURNS: The value of the atomic type
 *
 * ERRNO: None.
 */
IP_PUBLIC int ipcom_atomic_get
(
     Ipcom_atomic_t *v
);


/*******************************************************************************
 *
 * ipcom_atomic_set - set a value
 *
 * This routine set a value to a atomic variable.
 *
 * Parameters:
 * \is
 * \i <v>
 * Pointer to an atmomic variable
 * \i <i>
 * Value to set
 * \ie
 *
 * RETURNS: \&N/A.
 *
 * ERRNO: None.
 */
IP_PUBLIC void ipcom_atomic_set
(
     Ipcom_atomic_t *v,
     int             i
);


/*******************************************************************************
 *
 * ipcom_atomic_add - add an integer
 *
 * This routine add an integer to an atomic type
 *
 * Parameters:
 * \is
 * \i <v>
 * Pointer to an atomic variable
 * \i <i>
 * Value to add
 * \ie
 *
 * RETURNS: \&N/A.
 *
 * ERRNO: None.
 */
IP_PUBLIC void ipcom_atomic_add
(
     Ipcom_atomic_t *v,
     int             i
);


/*******************************************************************************
 *
 * ipcom_atomic_add_and_return - add an integer and returns the result
 *
 * This routine add an integer to an atomic type and returns the result
 *
 * Parameters:
 * \is
 * \i <v>
 * Pointer to an atomic variable
 * \i <i>
 * Value to add
 * \ie
 *
 * RETURNS: The result
 *
 * ERRNO: None.
 */
IP_PUBLIC int ipcom_atomic_add_and_return
(
     Ipcom_atomic_t *v,
     int             i
);


#ifndef IPCOM_USE_ATOMIC_MACROS
/*******************************************************************************
 *
 * ipcom_atomic_inc - add one
 *
 * This routine add one to an atomic type
 *
 * Parameters:
 * \is
 * \i <v>
 * Pointer to an atomic variable
 * \ie
 *
 * RETURNS: \&N/A.
 *
 * ERRNO: None.
 */
IP_PUBLIC void ipcom_atomic_inc
(
     Ipcom_atomic_t *v
);


/*******************************************************************************
 *
 * ipcom_atomic_dec - subtract one
 *
 * This routine subtract one from an atomic type
 *
 * Parameters:
 * \is
 * \i <v>
 * Pointer to an atomic variable
 * \ie
 *
 * RETURNS: \&N/A.
 *
 * ERRNO: None.
 */
IP_PUBLIC void ipcom_atomic_dec
(
     Ipcom_atomic_t *v
);


/*******************************************************************************
 *
 * ipcom_atomic_sub - subtract an integer
 *
 * This routine subtract an integer from an atomic type
 *
 * Parameters:
 * \is
 * \i <v>
 * Pointer to an atomic variable
 * \i <i>
 * Value to subtract
 * \ie
 *
 * RETURNS: \&N/A.
 *
 * ERRNO: None.
 */
IP_PUBLIC void ipcom_atomic_sub
(
     Ipcom_atomic_t *v,
     int             i
);


/*******************************************************************************
 *
 * ipcom_atomic_sub_and_return - subtracts an integer and returns the result
 *
 * This routine subtracts an integer from an atomic type and returns the result
 *
 * Parameters:
 * \is
 * \i <v>
 * Pointer to an atomic variable
 * \i <i>
 * Value to subtract
 * \ie
 *
 * RETURNS: The result
 *
 * ERRNO: None.
 */
IP_PUBLIC int ipcom_atomic_sub_and_return
(
     Ipcom_atomic_t *v,
     int             i
);
#endif /* !IPCOM_USE_ATOMIC_MACROS */

#ifdef __cplusplus
}
#endif

#endif /* IPCOM_ATOMIC_H */

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

