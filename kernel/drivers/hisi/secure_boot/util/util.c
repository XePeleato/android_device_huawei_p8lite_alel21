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
#define DX_PAL_LOG_CUR_COMPONENT DX_LOG_MASK_SECURE_BOOT

/************* Include Files ****************/

#include "secureboot_basetypes.h"
#include "systemdep.h"
#include "util.h"

#if SB_DEBUG_DATA
#include <stdio.h>
#endif

/************************ Defines ******************************/


/************************ Enums ******************************/


/************************ Typedefs ******************************/


/************************ Global Data ******************************/


/************* Private function prototype ****************/


/************************ Private Functions ******************************/

/* ------------------------------------------------------------
 **
 * @brief This function executes a reversed words copy on a specified buffer.
 *
 *        on a 6 words buffer:
 *
 *        buff[5] <---> buff[0]     
 *        buff[4] <---> buff[1]     
 *        buff[3] <---> buff[2]     
 *
 * @param[in] dst_ptr - The counter buffer.
 * @param[in] size    - The counter size in words. 
 *
 */
 void UTIL_ReverseWordsBuff( DxUint32_t *buff_ptr , DxUint32_t sizeWords )
 {
   /* FUNCTION DECLARATIONS */
   
   /* loop variable */
   DxUint32_t i;
      
   /* buffer swap pointers */
   DxUint32_t *buffHighSwap_ptr,*buffLowSwap_ptr;
   
   /* a temp variable */
   DxUint32_t temp;
  
   /* FUNCTION LOGIC */

   /* initialize the source and the destination poision */
   buffHighSwap_ptr = buff_ptr + sizeWords - 1; 
   buffLowSwap_ptr  = buff_ptr;
   
   /* execute the reverse memcpoy */
   for( i = 0 ; i < (sizeWords / 2) ; i++ )
   { 
     temp                        = *buffHighSwap_ptr; 
     *(buffHighSwap_ptr--)       = *buffLowSwap_ptr; 
     *(buffLowSwap_ptr++)        = temp; 
   }
      
   return;  
 
 }/* END OF UTIL_ReverseWordsBuff */

			      
/* ------------------------------------------------------------
 **
 * @brief This function executes a reversed byte copy on a specified buffer.
 *
 *        on a 6 byte buffer:
 *
 *        buff[5] <---> buff[0]     
 *        buff[4] <---> buff[1]     
 *        buff[3] <---> buff[2]     
 *
 * @param[in] dst_ptr - The counter buffer.
 * @param[in] src_ptr - The counter size in bytes. 
 *
 */
 void UTIL_ReverseBuff( DxUint8_t *buff_ptr , DxUint32_t size )
 {
   /* FUNCTION DECLARATIONS */
   
   /* loop variable */
   DxUint32_t i;
      
   /* buffer swap pointers */
   DxUint8_t *buffHighSwap_ptr,*buffLowSwap_ptr;
   
   /* a temp variable */
   DxUint32_t temp;
  
   /* FUNCTION LOGIC */

   /* initialize the source and the destination poision */
   buffHighSwap_ptr = buff_ptr + size - 1; 
   buffLowSwap_ptr  = buff_ptr;
   
   /* execute the reverse memcpoy */
   for( i = 0 ; i < (size / 2) ; i++ )
   { 
     temp                        = *buffHighSwap_ptr; 
     *(buffHighSwap_ptr--)       = *buffLowSwap_ptr; 
     *(buffLowSwap_ptr++)        = temp; 
   }
      
   return;  
 
 }/* END OF UTIL_ReverseBuff */

/* ------------------------------------------------------------
 **
 * @brief This function executes a memory copy between 2 buffers.
 * 
 * @param[in] dst_ptr - The first counter buffer.
 * @param[in] src_ptr - The second counter buffer. 
 * @param[in] size    - the first counter size in bytes.
 *
 */
 
 void UTIL_MemCopy( DxUint8_t *dst_ptr , DxUint8_t *src_ptr , DxUint32_t size )
 {
   /* FUNCTION DECLERATIONS */
   
   /* loop variable */
   DxUint32_t i;
     
   /* FUNCTION LOGIC */

   /* execute the reverse memcpoy */
   for( i = 0 ; i < size ; i++ )
     
     dst_ptr[i] = src_ptr[i]; 
     
   return;  
 
}/* END OF UTIL_MemCopy */


/* ------------------------------------------------------------
 **
 * @brief This function executes a reverse bytes copying from one buffer to another buffer.
 *
 * @param[in] dst_ptr - The pointer to destination buffer.
 * @param[in] src_ptr - The pointer to source buffer. 
 * @param[in] size    - The size in bytes.
 *
 */
 void UTIL_ReverseMemCopy( DxUint8_t *dst_ptr , DxUint8_t *src_ptr , DxUint32_t size )
 {
	 /* FUNCTION DECLARATIONS */

	 /* loop variable */
	 DxUint32_t i;

	 /* buffers position identifiers */
	 DxUint32_t buff_dst_pos, buff_src_pos;

	 /* FUNCTION LOGIC */
	 
	 /* execute the reverse copy in case of different buffers */
	 /* initialize the source and the destination position */
	 buff_dst_pos = size - 1;
	 buff_src_pos = 0;

	 for( i = 0 ; i < size ; i++ )
		 dst_ptr[buff_dst_pos--] = src_ptr[buff_src_pos++];
	
   
	 return;  

 }/* END OF UTIL_ReverseMemCopy */


/* ------------------------------------------------------------
 **
 * @brief This function executes a memory set operation on a buffer.
 * 
 * @param[in] buff_ptr - the buffer.
 * @param[in] val      - The value to set the buffer. 
 * @param[in] size     - the buffers size.
 *
 */
 
 void UTIL_MemSet( DxUint8_t *buff_ptr , DxUint8_t val , DxUint32_t size )
 {
   /* FUNCTION DECLERATIONS */
   
   /* loop variable */
   DxUint32_t i;
     
   /* FUNCTION LOGIC */

   /* execute the reverse memcpoy */
   for( i = 0 ; i < size ; i++ )
     
     buff_ptr[i] = val; 
     
   return;  
 
}/* END OF UTIL_MemCopy */

/* ------------------------------------------------------------
 **
 * @brief This function executes a memory comparation between 2 buffers.
 * 
 * @param[in] buff1_ptr - The first counter buffer.
 * @param[in] buff2_ptr - The second counter buffer. 
 * @param[in] size    - the first counter size in bytes.
 *
 */
 
 DxUint32_t UTIL_MemCmp( DxUint8_t *buff1_ptr , DxUint8_t *buff2_ptr , DxUint32_t size )
 {
   /* FUNCTION DECLERATIONS */
   
   /* loop variable */
   DxUint32_t i;
     
   /* FUNCTION LOGIC */

   /* execute the reverse memcpoy */
   for( i = 0 ; i < size ; i++ )
   {
   	
     if( buff1_ptr[i] != buff2_ptr[i] )
      
        return DX_FALSE; 
   }
     
   return DX_TRUE;  
 
}/* END OF UTIL_MemCopy */

/**
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
 * @param[in] SizeUnit         - the size units. 0 - bits , 1 - bytes 
 *
 * @return result - an enum with the comparation result: 
 *                                0 - both counters are identical 
 *                                1 - counter 1 is larger.
 *                                2 - counter 2 is larger.
 * @note This code executes in constant time, regardless of the arguments.
 */

 UTIL_CmpCounter_t UTIL_CmpLsbUnsignedCounters( DxUint8_t  *CounterBuff1_ptr,
                                                DxUint16_t  Counter1Size, 
                                                DxUint8_t  *CounterBuff2_ptr,
                                                DxUint16_t Counter2Size )
{
   /* FUNCTION LOCAL DECLERATIONS */
      
   /* loop variable */
   DxInt32_t i;
   
   /* the result after comaring the bytes */
   UTIL_CmpCounter_t Result;
   
   /* the final result */
   UTIL_CmpCounter_t FinalResult;
   
   /* FUNCTION LOGIC */
   
   /* ........ initialize local variables ............................................ */
   /* -------------------------------------------------------------------------------- */
   
   /* the default is that the result is the same */
   Result = UTIL_CmpCounter1AndCounter2AreIdentical;
   FinalResult = UTIL_CmpCounter1AndCounter2AreIdentical; /* just to avoid compilers warnings */
      
   /* ........ calculate the effective size ( decrementing the zeros at the MS bytes ) */
   /* -------------------------------------------------------------------------------- */
      
   /* a loop for adjusting the counter 1 size by neglacting the zeros */
   while( Counter1Size && (CounterBuff1_ptr[Counter1Size - 1] == 0) )
   
      Counter1Size--;
      
   /* a loop for adjusting the counter 2 size by neglacting the zeros */
   while( Counter2Size && (CounterBuff2_ptr[Counter2Size - 1] == 0) )
   
      Counter2Size--;
      
   /* ....... step 1 : comparing the counters assuming the effective counter size is the same .......... */
   /* -------------------------------------------------------------------------------------------------- */
   
   /* for security reasons we shall execute this loop as the minimum between the counter sizes 
      the result will be neglected in steps 2,3 if the actual size is diffrent */
   
   /* we shall compare all of the bytes from the MSB , the first one that is diffrent
      will determain which counter is larger , if all of the bytes are equal then the counters are equal */
     
   /* loop for checking all of the bytes */
   for( i = min(Counter1Size - 1,Counter2Size - 1) ; i >= 0 ; i-- )
   {
     /* if the counter 1 byte is grater then counter 2 byte - return counter 1 is bigger */
     if( (CounterBuff1_ptr[i] > CounterBuff2_ptr[i]) && Result == UTIL_CmpCounter1AndCounter2AreIdentical)
     
       Result = UTIL_CmpCounter1GraterThenCounter2;     
         
     /* if the counter 2 byte is grater then counter 1 byte - return counter 2 is bigger */  
     if( (CounterBuff2_ptr[i] > CounterBuff1_ptr[i]) && Result == UTIL_CmpCounter1AndCounter2AreIdentical)
       
       Result = UTIL_CmpCounter2GraterThenCounter1; 
         
   }/* end of searching all of the bytes loop */        
        
   /* ....... STEP 2 : the counter 1 effective size is bigger then counter 2 effective size */
   /* ------------------------------------------------------------------------------------- */
   
   /* on this case the final result is then counter 1 is larger then counter 2 - neglecting the 
      result calculated in step 1 */
   if( Counter1Size > Counter2Size )      
   
      FinalResult = UTIL_CmpCounter1GraterThenCounter2;     
   
   /* ....... STEP 3 : the counter 2 effective size is bigger then counter 1 effective size */
   /* ------------------------------------------------------------------------------------- */
 
    /* on this case the final result is then counter 2 is larger then counter 1 - neglecting the 
      result calculated in step 1 */  
   if( Counter2Size > Counter1Size )      
   
      FinalResult = UTIL_CmpCounter2GraterThenCounter1; 
      
   /* ....... STEP 4 : the counter 1 effective size is the same as the counter 2 effective size */
   /* ----------------------------------------------------------------------------------------- */

   /* on this case the final result is the one calculated in STEP 1 */   
   if( Counter2Size == Counter1Size )
   
      FinalResult = Result;     
  
   /* return the final result */  
   return FinalResult; 
     
}/* END OF UTIL_CmpLsbUnsignedCounters */

/* -------------------------------------------------------------
 * @brief This functions operation : result = A-B 
 *
 * @param[in] A_ptr - vector A
 * @param[in] B_ptr - vector B.
 *
 * @param[in] DataSizeIn256Bits  - the data size in 256 bit chanks
 *
 * @param[out] result_ptr - The result.
 *
 * @return CRYSError_t - On success CRYS_OK is returned, on failure a
 *                        value MODULE_* as defined in ...
 **--------------------------------------------------------*/

void UTIL_SubVectors( 
              DxUint32_t           *A_ptr,         
              DxUint32_t           *B_ptr ,          
              DxUint32_t            DataSizeIn256Bits,          
              DxUint32_t           *result_ptr )               
{
	
	/* FUNCTION LOCAL DECLERATIONS */
	
	DxUint32_t i,sub,borrow=0;

	/*convert input data size to words*/
  	DxUint32_t len = DataSizeIn256Bits * 8;

	/*run till the len for the carry*/		
	for(i=0 ; i <= len ; i++)
  	{
		sub= A_ptr[i] - borrow - B_ptr[i];
	    
		/*  If the Sub result is smaller then 0 than the borrow is set to 1
			and only 1 byte is save in the Result (Modulo 2^32) */

	    borrow = (A_ptr[i] < borrow) || ((A_ptr[i]-borrow) < B_ptr[i]);	

	    result_ptr[i] = sub;  
	}

    return;
 
}/* END OF UTIL_SubVectors */


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
	const DxUint8_t *data_p,
	size_t data_size)
{
	FILE *dump_fp;
	int i;

	if ((data_p == DX_NULL) || (data_size == 0))
		return;

	printf("Dumping data to %s ... ", fname);
	dump_fp = fopen(fname, "w");
	if (dump_fp == NULL) {
		fprintf(stderr, "Failed creating %s\n", fname);
		exit(1);
	}
	for (i = 0; i < data_size; i++) {
		fprintf(dump_fp, "0x%02X,", data_p[i]);
	}
	fprintf(dump_fp, "\n");
	fclose(dump_fp);
	printf("dump done.\n");
} 
#endif
