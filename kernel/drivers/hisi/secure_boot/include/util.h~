/*******************************************************************  
* (c) Copyright 2011-2012 Discretix Technologies Ltd.              *
* This software is protected by copyright, international           *
* treaties and patents.                                            *
* Use of this Software as part of or with the Discretix CryptoCell *
* or Packet Engine products is governed by the products'           *
* commercial end user license agreement ("EULA").                  *
* It is possible that copies of this Software might be distributed *
* under some type of GNU General Public License ("GPL").           *
* Notwithstanding any such distribution under the terms of GPL,    *
* GPL does not govern the use of this Software as part of or with  *
* the Discretix CryptoCell or Packet Engine products, for which a  *
* EULA is required.                                                *
* If the product's EULA allows any copy or reproduction of this    *
* Software, then such copy or reproduction must include this       *
* Copyright Notice as well as any other notices provided           *
* thereunder.                                                      *
********************************************************************/


 
#ifndef UTIL_H
#define UTIL_H

/*
 * All the includes that are needed for code using this module to
 * compile correctly should be #included here.
 */

#ifdef __cplusplus
extern "C"
{
#endif

  /*
   *  Object %name    : %
   *  State           :  %state%
   *  Creation date   :  Wed Feb 23 09:03:06 2005
   *  Last modified   :  %modify_time%
   */
  /** @file
   *  \brief A brief description of this module
   *
   *  \version UTIL.h#1:incl:1
   *  \author adams
   *  \remarks Copyright (C) 2004 by Discretix Technologies Ltd.
   *           All Rights reserved
   */




/************************ Defines ******************************/
#define SB_DEBUG_DATA 0

/* invers the bytes on a word- used for output from HASH */
#ifdef BIG__ENDIAN
#define UTIL_INVERSE_UINT32_BYTES( val )	(val)
#else
#define UTIL_INVERSE_UINT32_BYTES( val ) \
   ( ((val) >> 24) | (((val) & 0x00FF0000) >> 8) | (((val) & 0x0000FF00) << 8) | (((val) & 0x000000FF) << 24) )
#endif

/* invers the bytes on a word - used for input data for HASH */
#ifdef BIG__ENDIAN
#define UTIL_REVERT_UINT32_BYTES( val ) \
   ( ((val) >> 24) | (((val) & 0x00FF0000) >> 8) | (((val) & 0x0000FF00) << 8) | (((val) & 0x000000FF) << 24) )
#else
#define UTIL_REVERT_UINT32_BYTES( val )	(val)
#endif

/* the minimum and maximum macros */
#undef min
#define min( a , b ) ( ( (a) < (b) ) ? (a) : (b) )

#undef max    
#define max( a , b ) ( ( (a) > (b) ) ? (a) : (b) )

/* MACRO to count one bits */
#define COUNT_ONE_BITS(number, BitCount) \
do \
{ \
      DxUint32_t tmp_num = number; \
      BitCount = 0; \
      while (tmp_num)	\
      {			\
        tmp_num = tmp_num & (tmp_num - 1); \
        BitCount = BitCount + 1; \
      }			\
} while (0) 

/************************ Enums ********************************/

/* the counter comperation result enum */
typedef enum { UTIL_CmpCounter1AndCounter2AreIdentical = 0,
               UTIL_CmpCounter1GraterThenCounter2      = 1,
               UTIL_CmpCounter2GraterThenCounter1      = 2
               
             } UTIL_CmpCounter_t;       

/************************ Typedefs  ****************************/

/************************ Structs  ******************************/

/************************ Public Variables **********************/

/************************ Public Functions **********************/

/* ------------------------------------------------------------
 **
 * @brief This function executes a reversed words copy on a specified buffer.
 *
 *        on a 6 words byffer:
 *
 *        buff[5] <---> buff[0]     
 *        buff[4] <---> buff[1]     
 *        buff[3] <---> buff[2]     
 *
 * @param[in] dst_ptr - The counter buffer.
 * @param[in] size    - The counter size in words. 
 *
 */
 void UTIL_ReverseWordsBuff( DxUint32_t *buff_ptr , DxUint32_t sizeWords );


/* ------------------------------------------------------------
 **
 * @brief This function executes a reversed byte copy on a specified buffer.
 *
 *        on a 6 byte byffer:
 *
 *        buff[5] <---> buff[0]     
 *        buff[4] <---> buff[1]     
 *        buff[3] <---> buff[2]     
 *
 * @param[in] dst_ptr - The counter buffer.
 * @param[in] src_ptr - The counter size in bytes. 
 *
 */
 void UTIL_ReverseBuff( DxUint8_t *buff_ptr , DxUint32_t size );

/* ------------------------------------------------------------
 **
 * @brief This function executes a memory copy between 2 buffers.
 * 
 * @param[in] dst_ptr - The first counter buffer.
 * @param[in] src_ptr - The second counter buffer. 
 * @param[in] size    - The counter size in bytes.
 *
 */
 
 void UTIL_MemCopy( DxUint8_t *dst_ptr , DxUint8_t *src_ptr , DxUint32_t size );

/* ------------------------------------------------------------
 **
 * @brief This function executes a memory set operation on a buffer.
 * 
 * @param[in] buff_ptr - the buffer.
 * @param[in] val      - The value to set the buffer. 
 * @param[in] size     - the buffers size.
 *
 */
 
 void UTIL_MemSet( DxUint8_t *buff_ptr , DxUint8_t val , DxUint32_t size );

/* ------------------------------------------------------------
 **
 * @brief This function executes a memory comparation between 2 buffers.
 * 
 * @param[in] buff1_ptr - The first counter buffer.
 * @param[in] buff2_ptr - The second counter buffer. 
 * @param[in] size    - the first counter size in bytes.
 *
 */
 
 DxUint32_t UTIL_MemCmp( DxUint8_t *buff1_ptr , DxUint8_t *buff2_ptr , DxUint32_t size );
 
 /* ------------------------------------------------------------
 **
 * @brief This function executes a reverse bytes copying from one buffer to another buffer.
 *
 * @param[in] dst_ptr - The pointer to destination buffer.
 * @param[in] src_ptr - The pointer to source buffer. 
 * @param[in] size    - The size in bytes.
 *
 */
 
 void UTIL_ReverseMemCopy( DxUint8_t *dst_ptr , DxUint8_t *src_ptr , DxUint32_t size );
 
 
 /* ------------------------------------------------------------
 **
 * @brief This function subs 2 vectors: result = A - B
 * 
 * @param[in] A_ptr - The first vactor buffer.
 * @param[in] B_ptr - The second vector buffer. 
 * @param[in] SizeIn256BitBlocks    - the size in 256 bit blocks.
 * @param[in] result_ptr    - the result buffer.
 */

 void UTIL_SubVectors (DxUint32_t *A_ptr ,DxUint32_t *B_ptr, DxUint32_t SizeIn256BitBlocks ,DxUint32_t *result_ptr);


 /* ------------------------------------------------------------
**
 * @brief This function compares a value of 2 large counter presented in a byte buffer.
 *        The LSB of the counter is stored in the first cell in the array.
 *        
 *        for example:
 *
 *        a counter of 64 bit : the value is : 
 *
 *        byte[7] << 56 | byte[6] << 48 ............ byte[1] << 8 | byte[0]  
 *
 * @param[in] CounterBuff1_ptr - The first counter buffer.
 * @param[in] Counter1Size     - the first counter size in bytes. 
 * @param[in] CounterBuff2_ptr - The second counter buffer.
 * @param[in] Counter2Size     - the second counter size in bytes.
 *
 * @return result - an enum with the comparation result: 
 *                                0 - both counters are identical 
 *                                1 - counter 1 is larger.
 *                                2 - counter 2 is larger.
 */

 UTIL_CmpCounter_t UTIL_CmpLsbUnsignedCounters( DxUint8_t  *CounterBuff1_ptr,
                                                DxUint16_t  Counter1Size, 
                                                DxUint8_t  *CounterBuff2_ptr,
                                                DxUint16_t Counter2Size );

 
#if SB_DEBUG_DATA
/*!
 * Dump given data buffer to a file with the name 
 * <prefix>.<data_desc>.<proc_idx>
 * File format is hex-bytes (compatible with CryptoRef)
 * 
 * \param fname - Data description, used as file name
 * \param data_p - Buffer to dump
 * \param data_size - Size of buffer in bytes
 */
void dumping_data_to_file(
	const char *fname,
	const uint8_t *data_p,
	size_t data_size);
#endif

#ifdef __cplusplus
}
#endif

#endif



