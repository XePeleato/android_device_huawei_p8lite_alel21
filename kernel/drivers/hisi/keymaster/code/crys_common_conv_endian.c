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

  

/************* Include Files ****************/

#include "crys.h"
#include "crys_common.h"
#include "crys_common_math.h"
#include "crys_common_error.h"
#include "dx_pal_mem.h"

/************************ Defines ******************************/

/************************ Enums ********************************/

/************************ Typedefs *****************************/

/************************ Global Data **************************/

/************* Private function prototype **********************/

/************************ Public Functions *********************/

/***********************************************************************
 **
 * @brief This function executes a reverse bytes copying from one buffer to another buffer.
 *
 *        Overlapping of buffers is not allowed, excluding the case, when destination and source 
 *        buffers are the same. 
 *        Example of a 5 byte buffer:
 *
 *        dst_ptr[4] = src_ptr[0]     
 *        dst_ptr[3] = src_ptr[1]     
 *        dst_ptr[2] = src_ptr[2]     
 *        dst_ptr[1] = src_ptr[3]     
 *        dst_ptr[0] = src_ptr[4]     
 *
 * @param[in] dst_ptr - The pointer to destination buffer.
 * @param[in] src_ptr - The pointer to source buffer. 
 * @param[in] size    - The size in bytes.
 *
 */
 CRYSError_t CRYS_COMMON_ReverseMemcpy( DxUint8_t *dst_ptr , DxUint8_t *src_ptr , DxUint32_t size )
 {
	 /* FUNCTION DECLARATIONS */

	 /* loop variable */
	 DxUint32_t i;

	 /* FUNCTION LOGIC */
	 
	 /* check overlapping */	 		
	 if((dst_ptr > src_ptr && dst_ptr < (src_ptr + size)) ||
	    (dst_ptr < src_ptr && (dst_ptr + size) >= src_ptr)){
 		return CRYS_COMMON_DATA_OUT_DATA_IN_OVERLAP_ERROR;
	 }
	 

	 /* execute the reverse copy in case of different buffers */
	 if(dst_ptr != src_ptr){
		 for( i = 0 ; i < size ; i++ )
			 dst_ptr[i] = src_ptr[size - 1 - i];
	 }

	 /* execute the reverse copy in the same place */
	 else{
		 DxUint8_t  temp;

		 for( i = 0; i < size / 2; i++ ){
			 temp = src_ptr[i];
			 src_ptr[i] = src_ptr[size - 1 - i];
			 src_ptr[size - 1 - i] = temp;
		 }
	 }

	 return CRYS_OK;  

 }/* END OF CRYS_COMMON_ReverseMemcpy */

 
#ifndef DX_OEM_FW

/***********************************************************************/
 /**
  * @brief This function converts in place words byffer to bytes buffer with 
  *        reversed endianity of output array.
  *  
  *        The function can convert:
  *           - big endian bytes array to words array with little endian order
  *             of words and backward.
  *
  *      Note:
  *      1. Endianness of each word in words buffer should be set allways
  *      according to processor used.
  *      2. Implementation is given for both big and little endianness of
  *      processor.
  *        
  * @param[in]  buf_ptr - The 32-bits pointer to input/output buffer. 
  * @param[in]  sizeWords - The size in words (sizeWords > 0).
  *  
  * @return - no return value. 
  */
void CRYS_COMMON_InPlaceConvertBytesWordsAndArrayEndianness( 
					DxUint32_t *buf_ptr, 
					DxUint32_t  sizeWords)
{
	 /* FUNCTION DECLARATIONS */

	 DxUint32_t i, tmp;

	 /* FUNCTION logic */

	 /* Reverse words order and bytes in each word */
	 for(i = 0; i < sizeWords/2; i++){
	    #ifndef BIG__ENDIAN
		 tmp = CRYS_COMMON_REVERSE32(buf_ptr[i]);
		 buf_ptr[i] = CRYS_COMMON_REVERSE32(buf_ptr[sizeWords-i-1]); 
	    #else	 
		 tmp = buf_ptr[i];
		 buf_ptr[i] = buf_ptr[sizeWords-i-1]; 
	    #endif
		 buf_ptr[sizeWords-i-1] = tmp;
	 }

	 /* If sizeWords is odd revert middle word */
      #ifndef BIG__ENDIAN
	 if(sizeWords & 1UL){
		 buf_ptr[sizeWords/2] = CRYS_COMMON_REVERSE32(buf_ptr[sizeWords/2]);
	 }
      #endif


	 return;

}/* End of CRYS_COMMON_InPlaceConvertBytesWordsAndArrayEndianness */


/***********************************************************************/
  /**
  * @brief This function converts big endianness bytes array to aligned words
  *        array with words order according to little endian. 
  *
  *            1. Assumed, that input bytes order is set according
  *     	  to big endianness: MS Byte is most left, i.e. order is from
  *     	  Msb to Lsb.
  *            2. Output words array should set according to
  *     	  little endianness words order: LSWord is most left, i.e. order
  *     	  is from Lsw to Msw. Order bytes in each word - according to
  *     	  processor endianness.
  *            3. Owerlapping of buffers is not allowed, besides in
  *     	  place operation and size aligned to full words.
  *            4. Implementation is given for both big and little
  *     	  endianness of processor.
  *        
  * @param[out] out32_ptr - The 32-bits pointer to output buffer. 
  * @param[in] sizeOutBuffBytes - The size in bytes of output buffer, must be 
  *            aligned to 4 bytes and not less than sizeInBytes.
  * @param[in] in8_ptr - The pointer to input buffer. 
  * @param[in] sizeInBytes - The size in bytes of input data(sizeBytes >= 1).
  * 
  * @return CRYSError_t - On success CRYS_OK is returned, on failure a
  *                        value MODULE_* as defined in .
  */
CRYSError_t CRYS_COMMON_ConvertMsbLsbBytesToLswMswWords( 
					DxUint32_t *out32_ptr,
					DxUint32_t  sizeOutBuffBytes, 
					DxUint8_t  *in8_ptr, 
					DxUint32_t  sizeInBytes)
{
	 /* FUNCTION DECLARATIONS */

	 DxUint32_t sizeInWords;

	 /* FUNCTION LOGIC */

	 /* Check pointers and size */
	 if((in8_ptr == 0) || (out32_ptr == 0))
		 return CRYS_COMMON_DATA_IN_POINTER_INVALID_ERROR;

	 /*  Check the size and  in place operation:      * 
	 *   the size must be > 0 and aligned to words    */
	 if((sizeInBytes == 0) || 
	    (((DxUint32_t)out32_ptr == (DxUint32_t)in8_ptr) && (sizeInBytes & 3)))
		 return CRYS_COMMON_DATA_SIZE_ILLEGAL;

	 /* Size in words rounded up */
	 sizeInWords = (sizeInBytes + 3)/4;

	 if(sizeOutBuffBytes < sizeInWords*4)
		 return CRYS_COMMON_OUTPUT_BUFF_SIZE_ILLEGAL;

	 /*  If not in place operation zeroe output leading*   
	     words and copy the input into aligned output
	 *   buffer starting from non zero MSB */
	 if((DxUint32_t)out32_ptr != (DxUint32_t)in8_ptr){
		 
		 /* Check overlapping */
		 if(((DxUint32_t)out32_ptr > (DxUint32_t)in8_ptr && (DxUint32_t)out32_ptr  < ((DxUint32_t)in8_ptr + sizeInBytes)) ||
		    ((DxUint32_t)out32_ptr < (DxUint32_t)in8_ptr && ((DxUint32_t)out32_ptr + sizeInBytes) >= (DxUint32_t)in8_ptr))
			return CRYS_COMMON_DATA_OUT_DATA_IN_OVERLAP_ERROR;

		 /* zeroe empty bytes */
		 out32_ptr[0] = 0;
		 DX_PAL_MemSetZero((DxUint8_t*)out32_ptr + (sizeInBytes & 3UL), sizeOutBuffBytes - (sizeInBytes & 3UL));
		 /* copy */
		 DX_PAL_MemCopy((DxUint8_t*)out32_ptr + ((4-(sizeInBytes & 3UL)) & 3UL), in8_ptr, sizeInBytes);
	 }

	 /* Reverse words order and set endianness of each word */
	 CRYS_COMMON_InPlaceConvertBytesWordsAndArrayEndianness(out32_ptr, sizeInWords);

	 return CRYS_OK;
}


/***********************************************************************/
  /**
  * @brief This function converts big endianness bytes array to aligned words
  *        array with words order according to little endian.
  *
  *            1. Assumed, that input bytes order is set according
  *     	  to big endianness: MS Byte is most left, i.e. order is from
  *     	  Msb to Lsb.
  *            2. Output words array should be set according to
  *     	  little endianness words order: LSWord is most left, i.e. order
  *     	  is from Lsw to Msw. Bytes order in each word - according to
  *     	  processor endianness.
  *            3. Owerlapping of buffers is not allowed, besides in
  *     	  place operation and size aligned to full words.
  *            4. Implementation is given for both big and little
  *     	  endianness of processor.
  *        
  * @param[in] out32_ptr - The 32-bits pointer to output buffer. 
  * @param[in] sizeOutBuffBytes - The size in bytes of output buffer, must be 
  *       not less than sizeInBytes.
  * @param[out] in8_ptr - The pointer to input buffer. 
  * @param[in] sizeInBytes - The size in bytes. The size must be not 0 and 
  *       aligned to 4 bytes word.
  * 
  * @return CRYSError_t - On success CRYS_OK is returned, on failure a
  *                        value MODULE_* as defined in .
  */
CRYSError_t CRYS_COMMON_ConvertLswMswWordsToMsbLsbBytes( 
					DxUint8_t  *out8_ptr,
					DxUint32_t  sizeOutBuffBytes, 
					DxUint32_t *in32_ptr, 
					DxUint32_t  sizeInBytes)
{
	/* FUNCTION DECLARATIONS */
	
	DxUint32_t sizeInWords;
	
	/* FUNCTION LOGIC */
	
	/* Check pointers and size */
	if((in32_ptr == 0) || (out8_ptr == 0))
		return CRYS_COMMON_DATA_IN_POINTER_INVALID_ERROR;
	
	/* Size in words rounded up */
	sizeInWords = (sizeInBytes + 3)/4;
	
	if((sizeInBytes == 0) || (sizeOutBuffBytes < sizeInBytes))
		return CRYS_COMMON_DATA_SIZE_ILLEGAL;
	
	/* Check in place operation: the size must be aligned to word */
	if(((DxUint32_t)in32_ptr == (DxUint32_t)out8_ptr) && (sizeInBytes & 3UL))
		return CRYS_COMMON_DATA_SIZE_ILLEGAL;
	
	/* Reverse words order and bytes according to endianness of CPU */
	CRYS_COMMON_InPlaceConvertBytesWordsAndArrayEndianness(in32_ptr, sizeInWords);
	
	/* Copy output buffer */
	if((DxUint32_t)out8_ptr != (DxUint32_t)in32_ptr){

		/* Check overlapping */
		if(((DxUint32_t)out8_ptr > (DxUint32_t)in32_ptr && (DxUint32_t)out8_ptr  < ((DxUint32_t)in32_ptr + sizeInBytes)) ||
		((DxUint32_t)out8_ptr < (DxUint32_t)in32_ptr && ((DxUint32_t)out8_ptr + sizeInBytes) >= (DxUint32_t)in32_ptr))
			return CRYS_COMMON_DATA_OUT_DATA_IN_OVERLAP_ERROR;

		DX_PAL_MemCopy(out8_ptr, (DxUint8_t*)in32_ptr + ((4-(sizeInBytes & 3UL)) & 3UL), sizeInBytes);
		/* Revert the input buffer to previous state */
		CRYS_COMMON_InPlaceConvertBytesWordsAndArrayEndianness(in32_ptr, sizeInWords);
	}
	
	return CRYS_OK;
}

#endif
