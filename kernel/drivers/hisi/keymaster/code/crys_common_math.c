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

 

/********************** Include Files **********************************/
#include "crys.h"
#include "crys_common_math.h"
#include "crys_common_error.h"

/************************ Defines **************************************/

/************************ Enums ****************************************/

/************************ Typedefs *************************************/

/************************ Global Data **********************************/

/************* Private function prototype ******************************/

/************************ Public Functions *****************************/

#ifndef DX_OEM_FW
/********************************************************************************
 * @brief This function adds a value to a large counter presented in a buffer.
 *        The MSB of the counter is stored in the first cell in the array.
 *        
 *        for example:
 *
 *        a counter of 64 bit : the value is : 
 *
 *        byte[0] << 56 | byte[1] << 48 ............ byte[6] << 8 | byte[7]  
 *
 * @param[in] CounterBuff_ptr - The buffer containing the counter.
 * @param[in] Val             - this value to add.
 * @param[in] CounterSize      - the counter size in 32bit words. 
 *
 * @return CRYSError_t - On success CRYS_OK is returned, on failure a
 *                        value MODULE_* as defined in ...
 */

 void CRYS_COMMON_IncMsbUnsignedCounter( DxUint32_t     *CounterBuff_ptr ,
                                         DxUint32_t      Val,
                                         DxUint32_t      CounterSize )
{
   /* FUNCTION LOCAL DECLERATIONS */
   
   /* a value for storing the current counter word value */
   DxUint32_t curretCounterWordVal;
   
   /* loop variable */
   DxInt32_t i;
   
   /* FUNCTION LOGIC */
   
   /* .. inversing the counters bytes to a word in little endian format. */
   /* ------------------------------------------------------------------ */ 
   	for( i = 0 ; i < CounterSize ; i++ ) 
     		CounterBuff_ptr[i] = CRYS_COMMON_REVERSE32( CounterBuff_ptr[i] );
   
   /* .................... initialize local variables .................. */
   /* ------------------------------------------------------------------ */ 
   
   /* initialize the current local counter value to the first word */
   curretCounterWordVal = CounterBuff_ptr[CounterSize-1];
   
   /* .................... adding the value to the LSW ................. */
   /* ------------------------------------------------------------------ */
   
   /* adding the value to the word */
   CounterBuff_ptr[CounterSize-1] += Val;
        
   /* .................... adding the carry to the higher words ........ */
   /* ------------------------------------------------------------------ */  
   
   /* if there is overflow on the word then handle the upper words */
   if( curretCounterWordVal > CounterBuff_ptr[CounterSize-1] )
   {  
     /* adding the carry to the counter loop */
     for( i = CounterSize - 2 ; i >= 0 ; i-- )
     {
        /* set the current word value */
        curretCounterWordVal = CounterBuff_ptr[i];
      
        /* adding the carry to the current word */
        CounterBuff_ptr[i]++;
            
        /* if there is no overflow on the current word after adding the value 
           exit the loop */
        if( curretCounterWordVal < CounterBuff_ptr[i] ) 
      
          break;
        
     }/* end of adding the carry loop */ 
         
   }/* end of setting the carrier on the upper words */  

   /* .. restore the counters bytes order */
   /* ----------------------------------- */ 

   	for( i = 0 ; i < CounterSize ; i++ ) 
     		CounterBuff_ptr[i] = CRYS_COMMON_REVERSE32( CounterBuff_ptr[i] );

   
   return;
   
}/* END OF CRYS_COMMON_IncMsbUnsignedCounter */                                         

/********************************************************************************
 * @brief This function adds a value to a large counter presented in a buffer.
 *        The LSB of the counter is stored in the first cell in the array.
 *        
 *        for example:
 *
 *        a counter of 64 bit : the value is : 
 *
 *        byte[7] << 56 | byte[6] << 48 ............ byte[1] << 8 | byte[0]  
 *
 * @param[in] CounterBuff_ptr - The buffer containing the counter.
 * @param[in] Val             - this value to add.
 * @param[in] CounterSize      - the counter size in 32bit words. 
 *
 * @return carry bit from MS word if carry occur
 *                        
 */

 DxUint32_t CRYS_COMMON_IncLsbUnsignedCounter( 
                                         DxUint32_t     *CounterBuff_ptr ,
                                         DxUint32_t      Val,
                                         DxUint32_t      CounterSize )
{
   /* FUNCTION LOCAL DECLERATIONS */
   
   /* a value for storing the current counter word value */
   DxUint32_t curretCounterWordVal;
   
   /* loop variable */
   DxInt32_t i;
   
   /* carry bit */
   DxUint32_t  carry = 0;
   
   /* FUNCTION LOGIC */
      
   /* .................... initialize local variables .................. */
   /* ------------------------------------------------------------------ */ 
   
   /* initialize the current local counter value to the first word */
   curretCounterWordVal = CounterBuff_ptr[0];
   
   /* .................... adding the value to the LSW ................. */
   /* ------------------------------------------------------------------ */
   
   /* adding the value to the word */
   CounterBuff_ptr[0] += Val;
        
   /* .................... adding the carry to the higher words ........ */
   /* ------------------------------------------------------------------ */  
   
   /* if there is overflow on the word then handle the upper words */
   if( curretCounterWordVal > CounterBuff_ptr[0] )
   {  
     /* adding the carry to the counter loop */
     for( i = 1 ; i < CounterSize ; i++ )
     {
        /* set the current word value */
        curretCounterWordVal = CounterBuff_ptr[i];
      
        /* adding the carry to the current word */
        CounterBuff_ptr[i]++;
            
        /* if there is no overflow on the current word after adding the value 
           exit the loop */
        if( curretCounterWordVal < CounterBuff_ptr[i] ) 
        {
            carry = 0;	
        	break;	
        }
        else
        	carry = 1;  
        
     }/* end of adding the carry loop */ 
         
   }/* end of setting the carrier on the upper words */  
   
   return carry;
   
}/* END OF CRYS_COMMON_IncLsbUnsignedCounter */ 
                                        
/********************************************************************************
 * @brief This function adds a value to a large counter presented in a buffer.
 *        Exactly like CRYS_COMMON_IncLsbUnsignedCounter, but when the processor is big endian
 *        
 *
 * @param[in] CounterBuff_ptr - The buffer containing the counter.
 * @param[in] Val             - this value to add.
 * @param[in] CounterSize      - the counter size in 32bit words. 
 *
 * @return carry bit from MS word if carry occur
 *                        
 */

 void CRYS_COMMON_IncLsbUnsignedCounterBigEndian(DxUint32_t     *CounterBuff_ptr ,
						DxUint32_t       Val,
						DxUint32_t       CounterSize )
{
   /* FUNCTION LOCAL DECLERATIONS */
   
   /* a value for storing the current counter word value */
   DxUint32_t curretCounterWordVal;
   
   /* loop variable */
   DxInt32_t i;
   
   /* FUNCTION LOGIC */
   
   /* .................... initialize local variables .................. */
   /* ------------------------------------------------------------------ */ 
   
   /* initialize the current local counter value to the first word */
   curretCounterWordVal = CounterBuff_ptr[CounterSize-1];
   
   /* .................... adding the value to the LSW ................. */
   /* ------------------------------------------------------------------ */
   
   /* adding the value to the word */
   CounterBuff_ptr[CounterSize-1] += Val;
        
   /* .................... adding the carry to the higher words ........ */
   /* ------------------------------------------------------------------ */  
   
   /* if there is overflow on the word then handle the upper words */
   if( curretCounterWordVal > CounterBuff_ptr[CounterSize-1] )
   {  
     /* adding the carry to the counter loop */
     for( i = CounterSize - 2 ; i >= 0 ; i-- )
     {
        /* set the current word value */
        curretCounterWordVal = CounterBuff_ptr[i];
      
        /* adding the carry to the current word */
        CounterBuff_ptr[i]++;
            
        /* if there is no overflow on the current word after adding the value 
           exit the loop */
        if( curretCounterWordVal < CounterBuff_ptr[i] ) 
      
          break;
        
     }/* end of adding the carry loop */ 
         
   }/* end of setting the carrier on the upper words */  

   
   return;
   
}/* END OF CRYS_COMMON_IncLsbUnsignedCounterBigEndian */                                         

/********************************************************************************
 * @brief This function subtracts a value from a large counter presented in a first 
 *        buffer and sets result in a second buffer. The first and the second 
 *        buffers may be the same.
 *        The LSB of the counter is stored in the first cell in the array,        
 *
 * @param[in]  CounterBuff_ptr - the buffer containing the counter.
 * @param[in]  Val             - the value to subtract.
 * @param[out] ResCounterBuff_ptr - the buffer containing the result counter.
 * @param[in]  CounterSize      - the counter size in 32bit words. 
 *
 * @return CRYSError_t - On success CRYS_OK is returned, on failure a
 *                        value MODULE_* as defined in ...
 */

 void CRYS_COMMON_DecrLsbUnsignedCounter( DxUint32_t     *CounterBuff_ptr,
                                          DxUint32_t      Val,
                                          DxUint32_t      CounterSizeInWords )
{
   /* FUNCTION LOCAL DECLERATIONS */
   
   /* borrow and temp word */
   DxUint32_t borrow, temp;
   
   /* loop variable */
   DxInt32_t i;
   
   /* FUNCTION LOGIC */
      
   /* .................... initialize local variables .................. */
   /* ------------------------------------------------------------------ */ 
   
   /* initialize the borrow to Val */
   borrow = Val;
   
   /* Subtracting loop */
   for( i = 0 ; i < CounterSizeInWords ; i++ )
   { 
     	temp = CounterBuff_ptr[i];
     	
   		CounterBuff_ptr[i] = CounterBuff_ptr[i] - borrow;
   		
   		if( CounterBuff_ptr[i] > temp ) 
   			borrow = 1;
   		else 
   			break; 
   }
  
   return;
   
}/* END OF CRYS_COMMON_DecrLsbUnsignedCounter */ 

/********************************************************************************
 * @brief This function subtracts a value from a large counter presented in a first 
 *        buffer and sets result in a second buffer. The first and the second 
 *        buffers may be the same.
 *        The LSB of the counter is stored in the last cell in the array,        
 *
 * @param[in]  CounterBuff_ptr - the buffer containing the counter.
 * @param[in]  Val             - the value to subtract.
 * @param[out] ResCounterBuff_ptr - the buffer containing the result counter.
 * @param[in]  CounterSize      - the counter size in 32bit words. 
 *
 * @return CRYSError_t - On success CRYS_OK is returned, on failure a
 *                        value MODULE_* as defined in ...
 */

 void CRYS_COMMON_DecrMsbUnsignedCounter( DxUint32_t     *CounterBuff_ptr,
                                          DxUint32_t      Val,
                                          DxUint32_t      CounterSizeInWords )
{
   /* FUNCTION LOCAL DECLERATIONS */
   
   /* borrow and temp word */
   DxUint32_t borrow, temp;
   
   /* loop variable */
   DxInt32_t i;
   
   /* FUNCTION LOGIC */
      
   /* .................... initialize local variables .................. */
   /* ------------------------------------------------------------------ */ 
   
   /* initialize the borrow to Val */
   borrow = Val;
   
   /* Subtracting loop */
   for( i = CounterSizeInWords - 1 ; i >= 0 ; i-- )
   { 
     	temp = CounterBuff_ptr[i];
     	
   		CounterBuff_ptr[i] = CounterBuff_ptr[i] - borrow;
   		
   		if( CounterBuff_ptr[i] > temp ) 
   			borrow = 1;
   		else 
   			break; 
   }
  
   return;
   
}/* END OF CRYS_COMMON_DecrMsbUnsignedCounter */
                                        
/********************************************************************************
 * @brief This function compares a value of 2 large counters presented in a byte buffer.
 *        The MSB of the counter is stored in the first cell in the array.
 *        
 *        for example:
 *
 *        a counter of 64 bit : the value is : 
 *
 *        byte[0] << 56 | byte[1] << 48 ............ byte[6] << 8 | byte[7]  
 *
 *
 * @param[in] CounterBuff1_ptr - The first counter buffer.
 * @param[in] Counter1Size     - the first counter size in bytes. 
 * @param[in] CounterBuff2_ptr - The second counter buffer.
 * @param[in] Counter2Size     - the second counter size in bytes.
 * @param[in] SizeUnit         - the size units. 0 - bits , 1 - bytes 
 *
 * @return result - an enum with the compare result: 
 *                                0 - both counters are identical 
 *                                1 - counter 1 is larger.
 *                                2 - counter 2 is larger.
 * @note This code executes in constant time, regardless of the arguments.
 */

 CRYS_COMMON_CmpCounter_t CRYS_COMMON_CmpMsbUnsignedCounters( DxUint8_t  *CounterBuff1_ptr,
                                                              DxUint16_t  Counter1Size, 
                                                              DxUint8_t  *CounterBuff2_ptr,
                                                              DxUint16_t Counter2Size )
 {

   /* FUNCTION LOCAL DECLERATIONS */
      
   /* loop variable */
   DxInt32_t StartInd1,StartInd2,i;
   
   /* the result after comparing the bytes */
   CRYS_COMMON_CmpCounter_t Result;
   
   /* the final result */
   CRYS_COMMON_CmpCounter_t FinalResult;
   
   /* FUNCTION LOGIC */
   
   /* ........ initialize local variables ............................................ */
   /* -------------------------------------------------------------------------------- */
   
   /* the default is that the result is the same */
   Result = CRYS_COMMON_CmpCounter1AndCounter2AreIdentical;
   FinalResult = CRYS_COMMON_CmpCounter1AndCounter2AreIdentical; /* just to avoid compilers warnings */
      
   /* ........ calculate the effective size ( decrementing the zeros at the MS bytes ) */
   /* -------------------------------------------------------------------------------- */
      
   StartInd1=0,StartInd2=0;
   /* a loop for adjusting the counter 1 size by neglecting the zeros */
   while( Counter1Size && (CounterBuff1_ptr[StartInd1] == 0) )
   {
	  StartInd1++;
      Counter1Size--;
   }  
   /* a loop for adjusting the counter 2 size by neglecting the zeros */
   while( Counter2Size && (CounterBuff2_ptr[StartInd2] == 0) )
   {
	  StartInd2++;
      Counter2Size--;
   }
      
   /* ....... step 1 : comparing the counters assuming the effective counter size is the same .......... */
   /* -------------------------------------------------------------------------------------------------- */
   
   /* for security reasons we shall execute this loop as the minimum between the counter sizes 
      the result will be neglected in steps 2,3 if the actual size is different */
   
   /* we shall compare all of the bytes from the MSB , the first one that is different
      will determine which counter is larger , if all of the bytes are equal then the counters are equal */
     
   /* loop for checking all of the bytes */
   for( i=0 ; i < min(Counter1Size, Counter2Size) ; i++ )
   {
     /* if the counter 1 byte is grater then counter 2 byte - return counter 1 is bigger */
     if( (CounterBuff1_ptr[StartInd1 + i] > CounterBuff2_ptr[StartInd2 + i]) && Result == CRYS_COMMON_CmpCounter1AndCounter2AreIdentical)
     
       Result = CRYS_COMMON_CmpCounter1GraterThenCounter2;     
         
     /* if the counter 2 byte is grater then counter 1 byte - return counter 2 is bigger */  
     if( (CounterBuff2_ptr[StartInd2 + i] > CounterBuff1_ptr[StartInd1 + i]) && Result == CRYS_COMMON_CmpCounter1AndCounter2AreIdentical)
       
       Result = CRYS_COMMON_CmpCounter2GraterThenCounter1; 
         
   }/* end of searching all of the bytes loop */        
        
   /* ....... STEP 2 : the counter 1 effective size is bigger then counter 2 effective size */
   /* ------------------------------------------------------------------------------------- */
   
   /* on this case the final result is then counter 1 is larger then counter 2 - neglecting the 
      result calculated in step 1 */
   if( Counter1Size > Counter2Size )      
   
      FinalResult = CRYS_COMMON_CmpCounter1GraterThenCounter2;     
   
   /* ....... STEP 3 : the counter 2 effective size is bigger then counter 1 effective size */
   /* ------------------------------------------------------------------------------------- */
 
    /* on this case the final result is then counter 2 is larger then counter 1 - neglecting the 
      result calculated in step 1 */  
   if( Counter2Size > Counter1Size )      
   
      FinalResult = CRYS_COMMON_CmpCounter2GraterThenCounter1; 
      
   /* ....... STEP 4 : the counter 1 effective size is the same as the counter 2 effective size */
   /* ----------------------------------------------------------------------------------------- */

   /* on this case the final result is the one calculated in STEP 1 */   
   if( Counter2Size == Counter1Size )
   
      FinalResult = Result;     
  
   /* return the final result */  
   
   return FinalResult;  
       
 } /* end CRYS_COMMON_CmpMsbUnsignedCounters */
 

#endif


/********************************************************************************
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
 * @return result - an enum with the compare result: 
 *                                0 - both counters are identical 
 *                                1 - counter 1 is larger.
 *                                2 - counter 2 is larger.
 */

 CRYS_COMMON_CmpCounter_t CRYS_COMMON_CmpLsbUnsignedCounters( DxUint8_t  *CounterBuff1_ptr,
                                                              DxUint16_t  Counter1Size, 
                                                              DxUint8_t  *CounterBuff2_ptr,
                                                              DxUint16_t Counter2Size )
{
   /* FUNCTION LOCAL DECLERATIONS */
      
   /* loop variable */
   DxInt32_t i;
   
   /* the result after comparing the bytes */
   CRYS_COMMON_CmpCounter_t Result;
   
   /* the final result */
   CRYS_COMMON_CmpCounter_t FinalResult;
   
   /* FUNCTION LOGIC */
   
   /* ........ initialize local variables ............................................ */
   /* -------------------------------------------------------------------------------- */
   
   /* the default is that the result is the same */
   Result = CRYS_COMMON_CmpCounter1AndCounter2AreIdentical;
   FinalResult = CRYS_COMMON_CmpCounter1AndCounter2AreIdentical; /* just to avoid compilers warnings */
      
   /* ........ calculate the effective size ( decrementing the zeros at the MS bytes ) */
   /* -------------------------------------------------------------------------------- */
      
   /* a loop for adjusting the counter 1 size by neglecting the zeros */
   while( Counter1Size && (CounterBuff1_ptr[Counter1Size - 1] == 0) )
   
      Counter1Size--;
      
   /* a loop for adjusting the counter 2 size by neglecting the zeros */
   while( Counter2Size && (CounterBuff2_ptr[Counter2Size - 1] == 0) )
   
      Counter2Size--;
      
   /* ....... step 1 : comparing the counters assuming the effective counter size is the same .......... */
   /* -------------------------------------------------------------------------------------------------- */
   
   /* for security reasons we shall execute this loop as the minimum between the counter sizes 
      the result will be neglected in steps 2,3 if the actual size is different */
   
   /* we shall compare all of the bytes from the MSB , the first one that is different
      will determine which counter is larger , if all of the bytes are equal then the counters are equal */
     
   /* loop for checking all of the bytes */
   for( i = min(Counter1Size - 1,Counter2Size - 1) ; i >= 0 ; i-- )
   {
     /* if the counter 1 byte is grater then counter 2 byte - return counter 1 is bigger */
     if( (CounterBuff1_ptr[i] > CounterBuff2_ptr[i]) && Result == CRYS_COMMON_CmpCounter1AndCounter2AreIdentical)
     
       Result = CRYS_COMMON_CmpCounter1GraterThenCounter2;     
         
     /* if the counter 2 byte is grater then counter 1 byte - return counter 2 is bigger */  
     if( (CounterBuff2_ptr[i] > CounterBuff1_ptr[i]) && Result == CRYS_COMMON_CmpCounter1AndCounter2AreIdentical)
       
       Result = CRYS_COMMON_CmpCounter2GraterThenCounter1; 
         
   }/* end of searching all of the bytes loop */        
        
   /* ....... STEP 2 : the counter 1 effective size is bigger then counter 2 effective size */
   /* ------------------------------------------------------------------------------------- */
   
   /* on this case the final result is then counter 1 is larger then counter 2 - neglecting the 
      result calculated in step 1 */
   if( Counter1Size > Counter2Size )      
   
      FinalResult = CRYS_COMMON_CmpCounter1GraterThenCounter2;     
   
   /* ....... STEP 3 : the counter 2 effective size is bigger then counter 1 effective size */
   /* ------------------------------------------------------------------------------------- */
 
    /* on this case the final result is then counter 2 is larger then counter 1 - neglecting the 
      result calculated in step 1 */  
   if( Counter2Size > Counter1Size )      
   
      FinalResult = CRYS_COMMON_CmpCounter2GraterThenCounter1; 
      
   /* ....... STEP 4 : the counter 1 effective size is the same as the counter 2 effective size */
   /* ----------------------------------------------------------------------------------------- */

   /* on this case the final result is the one calculated in STEP 1 */   
   if( Counter2Size == Counter1Size )
   
      FinalResult = Result;     
  
   /* return the final result */  
   return FinalResult; 
        
}/* END OF CRYS_COMMON_CmpLsbUnsignedCounters */



/**************************************************************************
 *           CRYS_COMMON_CmpLsWordsUnsignedCounters function    	  * 
 **************************************************************************/
/**
 * @brief This function compares a value of 2 large counter presented in a word buffer.
 *        The LSWord of the counters is stored in the first cell in the array.
 *        
 *
 * @param[in] CounterBuff1_ptr - The first counter buffer.
 * @param[in] Counter1SizeWords     - the first counter size in Words. 
 * @param[in] CounterBuff2_ptr - The second counter buffer.
 * @param[in] Counter2SizeWords     - the second counter size in Words.
 *
 * @return result - an enum with the compare result: 
 *                                0 - both counters are identical 
 *                                1 - counter 1 is larger.
 *                                2 - counter 2 is larger.
 */
 CRYS_COMMON_CmpCounter_t CRYS_COMMON_CmpLsWordsUnsignedCounters(DxUint32_t  *CounterBuff1_ptr,
                                                              DxUint16_t   Counter1SizeWords, 
                                                              DxUint32_t  *CounterBuff2_ptr,
                                                              DxUint16_t   Counter2SizeWords)
{
   /* FUNCTION LOCAL DECLARATIONS */
      
   /* loop variable */
   DxInt32_t i;
   
   /* the result after comparing the bytes */
   CRYS_COMMON_CmpCounter_t Result;
   
   /* the final result */
   CRYS_COMMON_CmpCounter_t FinalResult;
   
   /* FUNCTION LOGIC */
   
   /* ........ initialize local variables ............................................ */
   /* -------------------------------------------------------------------------------- */
   
   /* the default is that the result is the same */
   Result = CRYS_COMMON_CmpCounter1AndCounter2AreIdentical;
   FinalResult = CRYS_COMMON_CmpCounter1AndCounter2AreIdentical; /* just to avoid compilers warnings */
      
   /* ........ calculate the effective size ( decrementing the zeros at the MS bytes ) */
   /* -------------------------------------------------------------------------------- */
      
   /* a loop for adjusting the counter 1 size by neglecting the zeros */
   while(Counter1SizeWords && (CounterBuff1_ptr[Counter1SizeWords - 1] == 0))
      Counter1SizeWords--;
      
   /* a loop for adjusting the counter 2 size by neglecting the zeros */
   while(Counter2SizeWords && (CounterBuff2_ptr[Counter2SizeWords - 1] == 0))
      Counter2SizeWords--;
      
   /* ....... step 1 : comparing the counters assuming the effective counter size is the same .......... */
   /* -------------------------------------------------------------------------------------------------- */
   
   /* for security reasons we shall execute this loop as the minimum between the counter sizes 
      the result will be neglected in steps 2,3 if the actual size is different */
   
   /* we shall compare all of the bytes from the MSB , the first one that is different
      will determine which counter is larger , if all of the bytes are equal then the counters are equal */
     
   /* loop for checking all of the bytes */
   for(i = min(Counter1SizeWords - 1,Counter2SizeWords - 1) ; i >= 0 ; i--)
   {
     /* if the counter 1 byte is grater then counter 2 byte - return counter 1 is bigger */
     if((CounterBuff1_ptr[i] > CounterBuff2_ptr[i]) && (Result == CRYS_COMMON_CmpCounter1AndCounter2AreIdentical))
       Result = CRYS_COMMON_CmpCounter1GraterThenCounter2;     
         
     /* if the counter 2 byte is grater then counter 1 byte - return counter 2 is bigger */  
     if((CounterBuff2_ptr[i] > CounterBuff1_ptr[i]) && (Result == CRYS_COMMON_CmpCounter1AndCounter2AreIdentical))
       Result = CRYS_COMMON_CmpCounter2GraterThenCounter1; 
         
   }/* end of searching all of the bytes loop */        
        
   /* ....... STEP 2 : the counter 1 effective size is bigger then counter 2 effective size */
   /* ------------------------------------------------------------------------------------- */
   
   /* on this case the final result is then counter 1 is larger then counter 2 - neglecting the 
      result calculated in step 1 */
   if(Counter1SizeWords > Counter2SizeWords)
      FinalResult = CRYS_COMMON_CmpCounter1GraterThenCounter2;     
   
   /* ....... STEP 3 : the counter 2 effective size is bigger then counter 1 effective size */
   /* ------------------------------------------------------------------------------------- */
 
    /* on this case the final result is then counter 2 is larger then counter 1 - neglecting the 
      result calculated in step 1 */  
   if(Counter2SizeWords > Counter1SizeWords)
      FinalResult = CRYS_COMMON_CmpCounter2GraterThenCounter1; 
      
   /* ....... STEP 4 : the counter 1 effective size is the same as the counter 2 effective size */
   /* ----------------------------------------------------------------------------------------- */

   /* on this case the final result is the one calculated in STEP 1 */   
   if(Counter2SizeWords == Counter1SizeWords)
      FinalResult = Result;     
  
   /* return the final result */  
   return FinalResult; 
        
}/* END OF CRYS_COMMON_CmpLsbUnsignedCounters */




/*******************************************************************************
 *             CRYS_COMMON_GetBytesCounterEffectiveSizeInBits                  *
 *******************************************************************************
 *
 * @brief This function returns the effective number of bits in the byte stream counter 
 *        ( searching the highest '1' in the counter )
 *
 *        The function has one implementations: for little and big endian machines.
 *
 *        Assumed, that LSB of the counter is stored in the first cell in the array.
 *         For example, the value of the 8-Bytes counter B is : 
 *             B[7]<<56 | B[6]<<48 ............ B[1]<<8 | B[0] . 
 *        
 *
 * @param[in] CounterBuff_ptr -  The counter buffer.
 * @param[in] CounterSize     -  the counter size in bytes. 
 *
 * @return result - The effective counters size in bits.
 */

 DxUint32_t CRYS_COMMON_GetBytesCounterEffectiveSizeInBits( DxUint8_t  *CounterBuff_ptr,
                                                            DxUint16_t  CounterSize )
{                                               
   /* FUNCTION LOCAL DECLERATIONS */

   /* loop variable */
   DxInt32_t i; 
   
   /* the effective size in bits */
   DxUint32_t CounterEffectiveSizeInBits;
   
   /* the effective MS byte ( the one that is not zero ) */
   DxUint8_t EffectiveMsByteVal;

   /* FUNCTION LOGIC */
      
   /* STEP1 : a loop for adjusting the counter size by neglecting the MSB zeros */
   while( CounterSize && (CounterBuff_ptr[CounterSize - 1] == 0) )
   
     CounterSize--;

   /* STEP2 : if counter size is 0 - return 0 */
   if( CounterSize == 0 )
   
     return 0;

   /* set the effective MS byte */
   EffectiveMsByteVal = CounterBuff_ptr[CounterSize - 1];

   /* initialize the effective size as the counters size ( with MSB zeros ) */
   CounterEffectiveSizeInBits = CounterSize * 8;
   
   /* STEP 3 : adjusting the effective size in bits */
   for( i = 0; i < 8 ; i++ )
   {
     /* if the MS bit is set exit the loop */
     if( EffectiveMsByteVal & 0x80 )
     
       break;
       
     /* decrement the effective size in bits by 1 */
     CounterEffectiveSizeInBits--;
     
     /* shift the byte left by 1 bit */
     EffectiveMsByteVal <<= 1;
     
   }/* end of adjusting the effective size in bits loop */
   
   return CounterEffectiveSizeInBits;    
   
}/* END OF CRYS_COMMON_GetBytesCounterEffectiveSizeInBits */


#ifndef DX_OEM_FW

/*******************************************************************************
 *             CRYS_COMMON_GetWordsCounterEffectiveSizeInBits                  *
 *******************************************************************************
 *
 * @brief This function returns the effective number of bits in the words array  
 *        ( searching the highest '1' in the counter )
 *
 *        The function may works on little and big endian machines.
 *
 *        Assumed, that the words in array are ordered from LS word to MS word.
 *        For LITTLE Endian machines assumed, that LSB of the each word is stored in the first 
 *        cell in the word. For example, the value of the 8-Bytes (B) counter is : 
 *             B[7]<<56 | B[6]<<48 ............ B[1]<<8 | B[0]  
 *        
 *        For BIG Endian machines assumed, that MS byte of each word is stored in the first 
 *        cell, LS byte is stored in the last place of the word. 
 *        For example, the value of the 64 bit counter is : 
 *         B[3] << 56 | B[2] << 48 B[1] << 8 | B[0],  B[7]<<56 | B[6]<<48 | B[5]<<8 | B[4]  
 *
 *     NOTE !!: 1. For BIG Endian the counter buffer and its size must be aligned to 4-bytes word.
 *
 * @param[in] CounterBuff_ptr   -  The counter buffer.
 * @param[in] CounterSizeWords  -  The counter size in words. 
 *
 * @return result - The effective counters size in bits.
 * 
 */
 DxUint32_t CRYS_COMMON_GetWordsCounterEffectiveSizeInBits( DxUint32_t  *CounterBuff_ptr,
                                                            DxUint16_t   CounterSizeWords)
{                                               

	/* FUNCTION LOCAL DECLARATIONS */
	
	/* loop variable */
	DxInt32_t i; 
	
	/* the effective size in bits */
	DxUint32_t CounterEffectiveSizeInBits = 0;
	
	/* the  MS word ( the first, that is not zero ) */
	DxUint32_t MsWordVal, bitsInMsWord = 0;
	
	
	/* FUNCTION LOGIC */
	
	
	/* STEP1 : a loop for adjusting the counter size by neglecting the MSW zeros */
	while( CounterSizeWords && (CounterBuff_ptr[CounterSizeWords - 1] == 0) ){
		CounterSizeWords--;
	}
	
	/* STEP2 : if counter size is 0 - return 0 */
	if( CounterSizeWords == 0 )
		return 0;
	
	/* set the effective MS word */
	MsWordVal = CounterBuff_ptr[CounterSizeWords - 1];
	
	/* initialize the effective size as the counters size ( with MSW zeros ) */
	CounterEffectiveSizeInBits = 32*(CounterSizeWords - 1);
	
	/* STEP 3 : adjusting the effective size in bits */
	for(i = 0; i < 32 ; i++){

		/* shift the word right by 1 bit */
		MsWordVal >>= 1;

		/* if the MS bit is set exit the loop */
		if(!MsWordVal){
			bitsInMsWord = i+1;
			break;
		}

	}/* end of adjusting the effective size in bits loop */
	
	return (CounterEffectiveSizeInBits + bitsInMsWord); 

}/* END OF CRYS_COMMON_GetWordsCounterEffectiveSizeInBits */


/********************************************************************************
 * @brief This function divides a vector by 2 - in a secured way
 *
 *        The LSB of the vector is stored in the first cell in the array. 
 *        
 *        for example:
 *
 *        a vector of 128 bit : the value is : 
 *
 *        word[3] << 96 | word[2] << 64 ............ word[1] << 32 | word[0]  
 *
 * @param[in] VecBuff_ptr     -  The vector buffer.
 * @param[in] SizeInWords     -  the counter size in words. 
 *
 * @return result - no return value.
 */

void CRYS_COMMON_DivideVectorBy2(DxUint32_t *VecBuff_ptr,DxUint32_t SizeInWords)
{
   /* FUNCTION LOCAL DECLERATIONS */

   DxUint32_t i;
   DxUint32_t Temp;
      
   /* FUNCTION LOGIC */ 

   /* for loop for dividing the vectors arrays by 2 */
   for (i=0;i < (SizeInWords)-1 ;i++)
   {
      VecBuff_ptr[i]=VecBuff_ptr[i] >> 1;
      Temp=VecBuff_ptr[i+1]&1UL;
      VecBuff_ptr[i]=VecBuff_ptr[i] | Temp<<(32-1);
   }

   /* dividing the MS word */
   VecBuff_ptr[SizeInWords-1]=VecBuff_ptr[SizeInWords-1]>>1;

   return;

}/* END OF CRYS_COMMON_divide_vector_by_2 */


/********************************************************************************
 * @brief This function shifts left a big endian vector by Shift - bits (Shift < 8).
 *
 *        The MSB of the vector is stored in the first cell in the array, 
 *        
 *        For example, a vector of 128 bit is : 
 *
 *        byte[n-1] | byte[n-2] ... byte[1] | byte[0]  
 *
 * @param[in] VecBuff_ptr     -  The vector buffer.
 * @param[in] SizeInBytes     -  The counter size in bytes. 
 * @param[in] Shift           -  The number of shift left bits, must be < 8.
 * @return no return value.
 */

void CRYS_COMMON_ShiftLeftBigEndVector(DxUint8_t *VecBuff_ptr,DxUint32_t SizeInBytes, DxInt8_t Shift)
{
   /* FUNCTION LOCAL DECLARATIONS */

   DxUint32_t i;
   DxUint32_t Temp = 0;
   
      
   /* FUNCTION LOGIC */ 
   
   if( SizeInBytes == 0 || Shift == 0 )
   		return;   
   
    /* loop for shifting the vector by Shift bits left */
   for (i=0;i < SizeInBytes - 1 ;i++)
   {
      VecBuff_ptr[i] = (DxUint8_t)(VecBuff_ptr[i] << Shift);
      Temp = VecBuff_ptr[i+1] & 0xFF ;
      VecBuff_ptr[i] = VecBuff_ptr[i] | (DxUint8_t)(Temp >> (8 - Shift));
   }

   /* shifting the LS byte */
   VecBuff_ptr[SizeInBytes - 1] = (DxUint8_t)(VecBuff_ptr[SizeInBytes - 1] << Shift);

   return;

}/* END OF CRYS_COMMON_ShiftLeftBigEndVector */


/*******************************************************************************
 *                      CRYS_COMMON_ShiftRightVector                           *
 *******************************************************************************
 * @brief This function shifts right a vector by Shift - bits (Shift < 8).
 *
 *        The LSB of the vector is stored in the first cell in the array. 
 *        For example, a vector of 128 bit is : 
 *
 *        byte[n-1] | byte[n-2] ... byte[1] | byte[0]  
 *
 * @param[in] VecBuff_ptr     -  The vector buffer.
 * @param[in] SizeInBytes     -  The counter size in bytes. 
 * @param[in] Shift           -  The number of shift left bits, must be < 8.
 * @return no return value.
 */
void CRYS_COMMON_ShiftRightVector(DxUint8_t *VecBuff_ptr,DxUint32_t SizeInBytes, DxInt8_t Shift)
{
   /* FUNCTION LOCAL DECLARATIONS */

   DxUint32_t i;
   DxUint32_t Temp = 0;
   
      
   /* FUNCTION LOGIC */ 
   
   if( SizeInBytes == 0 || Shift == 0 )
   		return;
   
    /* loop for shifting the vector by Shift bits right */
   for (i=0;i < SizeInBytes - 1 ;i++)
   {
      VecBuff_ptr[i] = (DxUint8_t)(VecBuff_ptr[i] >> Shift);
      Temp = VecBuff_ptr[i+1] & 0xFF ;
      VecBuff_ptr[i] = VecBuff_ptr[i] | (DxUint8_t)(Temp << (8 - Shift));
   }

   /* shifting the MS byte */
   VecBuff_ptr[SizeInBytes - 1] = (DxUint8_t)(VecBuff_ptr[SizeInBytes - 1] >> Shift);

   return;

}/* END OF CRYS_COMMON_ShiftRightVector */


/******************************************************************************
*                      CRYS_COMMON_ShiftLeftVector                            *
*******************************************************************************
* @brief This function shifts left a vector by Shift - bits (Shift < 8).
*
*        The LSB of the vector is stored in the first cell in the array. 
*        For example, a vector of 128 bit is : 
*
*        byte[n-1] | byte[n-2] ... byte[1] | byte[0]  
*
* @param[in] VecBuff_ptr     -  The vector buffer.
* @param[in] SizeInBytes     -  The counter size in bytes. 
* @param[in] Shift           -  The number of shift left bits, must be < 8.
* @return no return value.
*/
void CRYS_COMMON_ShiftLeftVector(DxUint8_t *VecBuff_ptr,DxUint32_t SizeInBytes, DxInt8_t Shift)
{
	/* FUNCTION LOCAL DECLARATIONS */

	DxUint32_t i;
	DxUint32_t Temp = 0;


	/* FUNCTION LOGIC */ 

	if( SizeInBytes == 0 || Shift == 0 )
		return;

	/* loop for shifting the vector by Shift bits left */
	for (i=SizeInBytes - 1;i > 0;i--)
	{
		VecBuff_ptr[i] = (DxUint8_t)(VecBuff_ptr[i] << Shift);
		Temp = VecBuff_ptr[i+1] & 0xFF ;
		VecBuff_ptr[i] = VecBuff_ptr[i] | (DxUint8_t)(Temp >> (8 - Shift));
	}

	/* shifting the LS byte */
	VecBuff_ptr[0] = (DxUint8_t)(VecBuff_ptr[0] >> Shift);

	return;

}/* END OF CRYS_COMMON_ShiftLeftVector */


/********************************************************************************
 * @brief This function adds 2 vectors ( A+B).
 *
 * @param[in] A_ptr       -  input vector A.
 * @param[in] B_ptr       -  input vector B.
 * @param[in] SizeInWords - The size in words 
 * @param[in] Res_ptr     - The result pointer
 *
 * @return - Carry from high words addition.
 */
 
DxUint32_t CRYS_COMMON_Add2vectors ( DxUint32_t *A_ptr, DxUint32_t *B_ptr,DxUint32_t SizeInWords, DxUint32_t *Res_ptr )
{
  /* FUNCTION LOCAL DECLARATIONS */

	DxUint16_t  i;
	DxUint32_t temp;
	DxUint32_t Carry, PrevCarry=0;
	DxUint32_t currentWordRes;
	
	/* FUNCTION LOGIC */
				
  for (i=0;i < SizeInWords;i++)
	{
		/* execute the addition */
    	currentWordRes = A_ptr[i]+B_ptr[i];

		/* check if carry */
		Carry = ((currentWordRes < A_ptr[i]) || (currentWordRes < B_ptr[i]));

		/* add previous carry */
		Res_ptr[i] = currentWordRes + PrevCarry;

    /*	Solve 4 problems: 1.if result > 32 bits ==> Carry to the next word.
 					      2.if the result=32 bits exactly ==> the result is = 0 but carry=1 to the next word.
 		                  3.if the result=32 bit-1 and because of PrevCarry the result come to 0 ==> Carry to the next word.
 			              4.if the result=0 because the exercise is 0+0. */
 										      
 		/* a dummy initialization for temp */								      
 		temp = 2;								      
 										      
    if( !(Res_ptr[i]) )
    
      temp = 1;
      
    if( Res_ptr[i] ) 
    
      temp = 0;  										      
 										      
		PrevCarry = Carry | ( PrevCarry & temp & 0x1);

	}/* end of adding the vectors */
				
  return PrevCarry;

}/* END OF CRYS_COMMON_Add2vectors */



/*******************************************************************************
 *                      CRYS_COMMON_SubtractUintArrays                         *
 *******************************************************************************
 
 * @brief This function subtracts two little endian word arrays (length SizeInWords):
 *        Res = (A - B) and returns borrow from subtracting of high words. 
 *
 * @param[in] A_ptr       -  Pointer to input vector A.
 * @param[in] B_ptr       -  Pointer to input vector B.
 * @param[in] SizeInWords -  Size in words of each of vectors
 * @param[in] Res_ptr     -  result pointer
 *
 * @return  Borrow from high words subtracting.
 */
 
DxUint32_t CRYS_COMMON_SubtractUintArrays(DxUint32_t *A_ptr, 
                                          DxUint32_t *B_ptr, 
                                          DxUint32_t  SizeInWords, 
                                          DxUint32_t *Res_ptr )
{
  /* FUNCTION LOCAL DECLARATIONS */

	DxUint32_t temp, i;

	DxUint32_t Borrow = 0;
		
	/* FUNCTION LOGIC */
				
    for (i=0; i < SizeInWords; i++)
	{
		/* subtract previous borrow */
   		temp = A_ptr[i] - Borrow;

		/* check if borrow */	    
	    if( temp > A_ptr[i] )
	    
	    	Borrow = 1;	
	    
		else Borrow = 0;
		
		/* subtract B */
   		Res_ptr[i] = temp - B_ptr[i];

		/* check if borrow */	    
	    if( Res_ptr[i] > temp )
	    
	    	Borrow ++;	
	    
			
	}/* end of subtracting */		
				
  return Borrow;

}/* END OF CRYS_COMMON_SubtractUintArrays */


/*******************************************************************************
 *                      CRYS_COMMON_AddTwoLsbUint8Vectors                      *
 *******************************************************************************
 *
 * @brief This function adds two little endian vectors Res = (A + B) and returns carry. 
 * 
 *
 * @param[in] A_ptr       -  input vector A.
 * @param[in] B_ptr       -  input vector B.
 * @param[in] SizeInWords -  size in words 
 * @param[in] Res_ptr     -  result pointer
 *
 * @return - carry from adding of two high bytes.
 */
 
DxUint32_t CRYS_COMMON_AddTwoLsbUint8Vectors(
                                          DxUint8_t  *A_ptr, 
                                          DxUint8_t  *B_ptr, 
                                          DxUint32_t  VectSizeInBytes, 
                                          DxUint8_t  *Res_ptr )

{
  /* FUNCTION DECLARATIONS */

	DxUint32_t  i;
	DxUint32_t temp ;
		

	/* FUNCTION LOGIC */

	temp = 0;

    for (i=0; i < VectSizeInBytes; i++)
	{
		/* add A, B and previous carry */
   		temp =  temp + A_ptr[i] + B_ptr[i];
		
		/* output current byte of result */
   		Res_ptr[i] = (DxUint8_t)(temp & 0xFF);
        
		/* calculate carry */ 
		temp = temp >> 8;

	}/* end of adding */
				
  return temp;

}/* END OF CRYS_COMMON_AddTwoLsbUint8Vectors */


/*******************************************************************************
 *                      CRYS_COMMON_SubtractMSBUint8Arrays                     *
 *******************************************************************************
 
 * @brief This function subtracts two big endian byte arrays. 
 *
 *   Assuming:  SizeA >= SizeB. 
 *              Size of result buffer is not less, than sizeA.
 *
 * @param[in] A_ptr       -  Pointer to input vector A.
 * @param[in] sizeA       -  Size in bytes of each of vector A.
 * @param[in] B_ptr       -  Pointer to input vector B.
 * @param[in] sizeB       -  Size in bytes of each of vector B.
 * @param[in] Res_ptr     -  result pointer
 *
 * @return  Borrow from high byte of vector A.
 */ 
DxUint8_t CRYS_COMMON_SubtractMSBUint8Arrays(
                                          DxUint8_t  *A_ptr, 
                                          DxUint32_t  sizeA, 
                                          DxUint8_t  *B_ptr, 
                                          DxUint32_t  sizeB, 
                                          DxUint8_t  *Res_ptr )
{
  /* FUNCTION LOCAL DECLARATIONS */

	DxUint8_t temp;

	DxUint8_t Borrow = 0;
	
	DxInt32_t difSizes, i;
	
		
	/* FUNCTION LOGIC */
	
	difSizes = sizeA - sizeB;
				
    for (i = sizeA - 1; i > 0; i--)
	{
		/* subtract previous borrow */
   		temp = A_ptr[i] - Borrow;

		/* check if borrow */	    
	    if( temp > A_ptr[i] )
	    
	    	Borrow = 1;	
	    
		else Borrow = 0;
		
		/* subtract B */
		if( i - difSizes >= 0 )
  	 		Res_ptr[i] = temp - B_ptr[i - difSizes];
		else
		    Res_ptr[i] = temp;

		/* check if borrow */	    
	    if( Res_ptr[i] > temp )
	    
	    	Borrow ++;	
	    
	}/* end of subtracting */		
				
  return Borrow;

}/* END OF CRYS_COMMON_SubtractUintArrays */


#endif 
