/*
 ******************************************************************************
 *                     HEADER FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_bitarray.h,v $ $Revision: 1.2 $
 *     $Source: /home/interpeak/CVSRoot/ipcom/include/ipcom_bitarray.h,v $
 *     $Author: ulf $ $Date: 2008-10-13 11:09:39 $
 *     $State: Exp $ $Locker:  $
 *
 *     INTERPEAK_COPYRIGHT_STRING
 *     Design and implementation by Balaji Raghavan(balaji.raghavan@windriver.com)
 ******************************************************************************
 */
#ifndef IPCOM_BITARRAY_H
#define IPCOM_BITARRAY_H

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

#include "ipcom_config.h"


/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#include "ipcom_type.h"
#include "ipcom_cstyle.h"
#include "ipcom_syslog.h"

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

typedef struct Ipcom_bitarray_struct
{
  Ip_u8   *buf;	     /* Buffer for data. */
  Ip_u32   alloc;    /* Number of bits allocated for data. */
  Ip_u32   offset;   /* Offset of first bit in the bitarray. */
}
Ipcom_bitarray;


/*
****************************************************************************
* 6                    FUNCTIONS
****************************************************************************
*/

#ifdef IPCOM_USE_BITARRAY
/* Frees any memory used for the bitarray. */
IP_PUBLIC void ipcom_bitarray_free(Ipcom_bitarray *bitarray);

/* creates bit array of size 8*size  */
IP_PUBLIC Ipcom_bitarray *ipcom_bitarray_new(Ip_s32 size);

/* resets contents of bitarray as well as internal 1st-bit offset */    
IP_PUBLIC void ipcom_bitarray_reset(Ipcom_bitarray *bitarray);

/* set "range" number of bits from bitpos (0=> 1-st bit and alloc-1=>
 * last bit in the bitarray */    
IP_PUBLIC Ip_err ipcom_bitarray_set(Ipcom_bitarray *bitarray, Ip_u32 bitpos, Ip_u32 range);

/* clear "range" number of bits from bitpos (0=> 1-st bit and alloc-1=>
 * last bit in the bitarray */        
IP_PUBLIC Ip_err ipcom_bitarray_clear(Ipcom_bitarray *bitarray, Ip_u32 bitpos, Ip_u32 range);

/* detemine if a the bitpos-th bit is set */    
IP_PUBLIC Ip_bool ipcom_bitarray_isset(Ipcom_bitarray *bitarray, Ip_u32 bitpos);

/* shift the bitarray left by bit_count */    
IP_PUBLIC Ip_err ipcom_bitarray_shiftL(Ipcom_bitarray *bitarray, Ip_u32 bit_count);

/* shift the bitarray right by bit_count */    
IP_PUBLIC Ip_err ipcom_bitarray_shiftR(Ipcom_bitarray *bitarray, Ip_u32 bit_count);
        
/*
 * Dumps the contents of the bitarray to stdout in hex.  This is intended for
 * debugging purposes only.
 */
IP_PUBLIC void ipcom_bitarray_dump(Ipcom_bitarray *bitarray);
IP_PUBLIC void ipcom_bitarray_test(void);

#endif /* IPCOM_USE_BITARRAY */
    
#ifdef __cplusplus
}
#endif

#endif


/*
****************************************************************************
*                      END OF FILE
****************************************************************************
*/
