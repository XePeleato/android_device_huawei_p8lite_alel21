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

#include "dx_rng.h"
#ifndef CRYS_RND_SEM_DISABLE
#include "DX_PAL_Sem.h"
#include "PLAT_SystemDep.h"
#else
#include "plat_print_dbg.h"
#endif
#include "dx_pal_mem.h"
#include "dx_pal_dma.h"
#include "dx_crys_kernel.h"
#include "dx_hal.h" 
#include "bsp.h"
#include "hw_queue.h"
#include "crys.h"
#include "crys_common_math.h"
#include "crys_rnd_local.h"
#include "crys_rnd_error.h"
#include "llf_rnd_hwdefs.h"
#include "llf_rnd.h"
#include "llf_rnd_error.h"
#include "debug.h"

/*
*  Object %name    : %
*  State           :  %state%
*  Creation date   :  09 March 2010
*  Last modified   :  %modify_time%
*/
/** @file
*  \brief A brief description of this module
*
*  \version LLF_AES.c#1:csrc:1
*  \author R.Levin
*  \remarks Copyright (C) 2010 by Discretix Technologies Ltd.
*           All Rights reserved
*/


/* canceling the lint warning:
   Use of goto is deprecated */
/*lint --e{801} */

/* canceling the lint warning:
   Info 717: do ... while(0) */
/*lint --e{717} */

/********************************* Defines ******************************/
#ifndef max
#define max(a,b) (a) > (b) ? (a) : (b)
#endif

/* definitions used in the Entropy Estimator functions */
#define S(a,n) ((DxUint32_t)((a) * (1<<(n)))) /* a scaled by n: a \times 2^n */
#define U(a,n) ((DxUint32_t)(a) >> (n)) /* unscale unsigned: a / 2^n */
#define SQR(x) (((x)&0xffff)*((x)&0xffff))

/* macros for updating histogram for any separate bit;  
   where x represents cw  or e1 */
#define   LLF_RND_UpdateHistOneBit( h_ptr, ec_ptr, x ) \
   	h_ptr[x & 0xff]++;  \
   	ec_ptr[x & 0x7f] = ((ec_ptr[x & 0x7f] & 1) == ((x & 0xff) >> 7)) ? ec_ptr[x & 0x7f] + 2 : ec_ptr[x & 0x7f] ^ 1; \
   	x >>= 1; 

/* Entropy estimation histogram width (prefix size + 1) */
#define LLF_RND_nb   CRYS_RND_nb 
#define LLF_RND_NB   CRYS_RND_NB
#define halfNB   (LLF_RND_NB / 2)

/* macro for calculation max. allowed time for */
#define LLF_RND_CalcMaxTrngTime(ehrSamples, SubSamplingRatio) \
	(((ehrSamples) * LLF_RND_TRNG_MAX_TIME_COEFF * \
	LLF_RND_TRNG_VON_NEUMAN_COEFF * \
	LLF_RND_HW_TRNG_EHR_WIDTH_IN_BITS * \
	(SubSamplingRatio)) >> LLF_RND_TRNG_MAX_TIME_SCALE) 

#define ROSC_INIT_START_BIT   0x80000000


/*********************************** Enums ******************************/
/*********************************Typedefs ******************************/

/**************** Global Data to be read by RNG function ****************/

/* includes  and definitions for semaphores management */
#if  !defined CRYS_RND_SEM_DISABLE && !defined CRYS_NO_GLOBAL_DATA
#include "DX_VOS_Sem.h"
/* importing the semaphore used for the access to the hardware machine */
extern DxVosSem SemRndId;       /* semaphore for TRNG */
extern DxVosSem SemHwAccessId;  /* semaphore for AES */  
#else
/* working from HOST side does not require any synchronization */
#define DX_VOS_SemWait(SemPkaId,timeout)	CRYS_OK
#define DX_VOS_SemGive(SemPkaId)
#endif

/* test variables */
#ifdef RND_TEST_TRNG_WITH_ESTIMATOR
DxUint32_t  gEntrSize[4];
#endif


	 
/******************************************************************************/
/***************   Prototypes and Private functions    ************************/ 
/******************************************************************************/
extern CRYSError_t RNG_PLAT_SetUserRngParameters(
			CRYS_RND_State_t   *rndState_ptr,         
			CRYS_RND_Params_t  *trngParams_ptr);

/****************************************************************************/ 
/***********         Functions used for Entropy estimation      *************/
/****************************************************************************/
/**
 * The function calculates low half of 32*32 bits multiplication result
 * 
 * @param a 
 * @param b 
 * 
 * @return DxUint64_t 
 */
DxUint64_t Mult32x32(DxUint32_t a, DxUint32_t b)
{
	DxUint64_t res=0;

	res = (((a>>16)*(b>>16)) + (((a>>16)*(b&0xffff))>>16) + (((b>>16)*(a&0xffff))>>16));
	res <<= 32;
	res += (DxUint64_t)((a&0xffff)*(b&0xffff)) + (((a>>16)*(b&0xffff))<<16) + (((b>>16)*(a&0xffff))<<16);

	return res;
}

/* Calculate 48*16 bits multiple using 16*16 bit multiplier */
/* Code ASM takes 62 bytes */
uint64_t Mult48x16(uint64_t a, uint32_t b)
{
	DxUint32_t a3 = (a >> 32), a2 = (a >> 16) & 0xffff, a1 = a & 0xffff;
	DxUint32_t b1 = (b & 0xffff);
	DxUint32_t r31 = a3*b1, r21 = a2*b1, r11 = a1*b1;
	return (((uint64_t)r31) << 32) +
		(((uint64_t)r21) << 16) +
		((uint64_t)r11);
}


/* approximation of entropy  */
/**
 * @brief The function approximates the entropy for separate prefix 
 *        ae = n * log2(n/m).
 *
 *    Implementation according A.Klimov algorithm uses approximation by 
 *    polynomial: ae = (n-m)*(A1 + A2*x + A3*x^2), where x = (n-m)/n <= 0.5 .
 *    The coefficients are defined above in this file. 
 *    
 * @param[in] n - The summ of  0-bits and 1-bits in the test.
 * @param[in] m - The maximal from the two above named counts.
 *
 * @return - result value of entropy ae.  
 */
static DxUint32_t ae(DxUint32_t n, DxUint32_t m)
{ 
	/* logarithm calculation constants */
	#define A1 1.4471280
	#define A2 0.6073851
	#define A3 0.9790318


    DxUint32_t d = n-m,
	    x = S(d,16) / n,                 /* x; 16 */
	    a = S(A3,14) * x,                /* x*A3; 30 */
	    b = U(S(A2,30) + a, 16) * x,     /* x*(A2 + x*A3); 30 */
	    c = (S(A1,30) + b),              /* (A1 + x*(A2 + x*A3)); 30 */
	    r = d * U(c,14);                 /* result: 16 bits scaled */

  return r;

}

/*****************************************************************************/
/**
 * @brief The function calculates a histogram of 0-s and 1-s distribution    
 *        depending on forgouing bits combination - prefix.  
 *     
 *     Implementation according A.Klimov algorithm modified by A.Ziv 
 *        
 * @param[in]  h_ptr - The pointer to the histogramm h buffer.
 * @param[in]  ec_ptr - The pointer to the histogramm equality counter (ec) buffer.
 * @param[in]  r_ptr - The pointer to Entropy source.
 * @param[in]  nr    - The size of Entropy source in words.
 * @param[in/out] pref_ptr - The pointer to last saved prefix.
 * @param[in]  snp_ptr   - The pointer to the flag defining whether the new prefix should be set.
 *
 * @return CRYSError_t - no return value  
 */ 
static void LLF_RND_HistogramUpdate(
			DxUint32_t   *h_ptr,   /* in/out */
			DxUint32_t   *ec_ptr,  /* in/out */
			DxUint32_t   *r_ptr,   /* in - input sequence */
			DxUint32_t     nr)      /* in - input sequence size in words */
{
	DxInt32_t   i, j = 0;
	DxUint32_t  cW; /*current word of sequence*/
	DxUint32_t  pref;
	
	/* FUNCTION  LOGIC  */
	
	/*------------------------------------------------------*/
	/* update for first word of sequence: begin new prefix  */
	/*------------------------------------------------------*/
        cW = r_ptr[0];
        
		/* 25 sequences are purely from new bits */
		for (i = 0; i < 5; i++) 
		{
		    LLF_RND_UpdateHistOneBit( h_ptr, ec_ptr, cW );
		    LLF_RND_UpdateHistOneBit( h_ptr, ec_ptr, cW );
		    LLF_RND_UpdateHistOneBit( h_ptr, ec_ptr, cW );
		    LLF_RND_UpdateHistOneBit( h_ptr, ec_ptr, cW );
		    LLF_RND_UpdateHistOneBit( h_ptr, ec_ptr, cW );
		}
			
		pref = cW;
		j = 1;
	
	/*-----------------------------------------------------------------------*/
	/* update for remaining words of sequence: continue with previous prefix */
	/*-----------------------------------------------------------------------*/	
	for( ; j < nr; j++ )
	{
		DxUint32_t e1;
        
		/*current word of random sequence*/
		cW = r_ptr[j];
		/* concatenation of previous saved prefix and new bits */
		e1 = (cW << 7) | pref;       
		
		/* first 7 sequences are combined from previous prefix and new bits  */
		LLF_RND_UpdateHistOneBit( h_ptr, ec_ptr, e1 );
		LLF_RND_UpdateHistOneBit( h_ptr, ec_ptr, e1 );
		LLF_RND_UpdateHistOneBit( h_ptr, ec_ptr, e1 );
		LLF_RND_UpdateHistOneBit( h_ptr, ec_ptr, e1 );
		LLF_RND_UpdateHistOneBit( h_ptr, ec_ptr, e1 );
		LLF_RND_UpdateHistOneBit( h_ptr, ec_ptr, e1 );
		LLF_RND_UpdateHistOneBit( h_ptr, ec_ptr, e1 );

		/* next 25 sequences are purely from new bits */
		for (i = 0; i < 5; i++) 
		{
			LLF_RND_UpdateHistOneBit( h_ptr, ec_ptr, cW );
			LLF_RND_UpdateHistOneBit( h_ptr, ec_ptr, cW );
			LLF_RND_UpdateHistOneBit( h_ptr, ec_ptr, cW );
			LLF_RND_UpdateHistOneBit( h_ptr, ec_ptr, cW );
			LLF_RND_UpdateHistOneBit( h_ptr, ec_ptr, cW );
		}
			
		pref = cW;
	}

} /* End of LLF_RND_HistogramUpdate() */


/*****************************************************************************/
/**
 * @brief The function calculates estimation of entropy, generated by TRNG and 
 *        used for control the TRNG work.  
 *        
 *   Implementation based on algorithm developed by A.Klimov.
 *    
 * @param[in] h - The pointer to the h-buffer (counts of 0-s and 1-s for each prefix).
 * @param[in] ec - The pointer to the ec-buffer (equality counters).
 * @param[out] e_ptr - The pointer to count of accumulated Entropy (bits multiplied by 2^16).
 *
 * @return CRYSError_t - according to module definitions  
 */ 
static CRYSError_t LLF_RND_EntropyEstimate( 
                        DxUint32_t *h, /*in/out*/
			DxUint32_t *ec, 
                        DxUint32_t *e_ptr )   /* out - result Entropy size */
{

	DxUint64_t t = 0;              /* total entropy */
	DxUint32_t i, ac = 0;          /* number of active prefixes */
  

	/*-------------  calculate entropy -----------------*/
  
	for(i = 0; i < halfNB; ++i)
	{

		DxUint32_t n = h[i] + h[i+halfNB], m = max(h[i], h[i+halfNB]);
    
		/* check that n < 2^16, else return overflow error */
		if( n >= (1UL<<16)) 
			return LLF_RND_TRNG_ENTR_ESTIM_SIZE_EXCEED_ERROR;
   
		if(n != m) /* if active prefix */
		{                 
			DxUint32_t n2, pp, od;
			DxUint64_t od2, od2n, var;

			/* increment count of active prefixes */
			++ac;       
      
			pp = SQR(m) + SQR(n-m);       /* related to theoretical "autocorrelation" probability */
			n2 = Mult16x16((ec[i]>>1),n); /* n2 used as temp */
      
			/* value, related to observed deviation of autocorrelation */
			if(n2 > pp)
				od = n2 - pp;
			else
				od = pp - n2; 
			
			/* theoretical variance of B(n, pp): always > 0 */ 
			n2 = SQR(n);
			var = Mult32x32(pp,(n2-pp));
      
			/* if  n*od^2 < var then accumulate entropy, else return Error; 
		           Note: that this condition is True only if od < 2^32 */
			if( od < ~0UL )
			{
				od2 = Mult32x32(od, od);
      		
				/* scale variables */
				if( od2 > ((DxUint64_t)1ULL << 48) )
				{
					od2 /= (1UL<<16);
					var /= (1UL<<16);
			        }
      		
				od2n = Mult48x16(od2, n); 
      		
				if( od2n < var )
					t += ae(n, m);
			}
		}
	}
  
	/* output entropy size value in bits (rescaled) */
	
	*e_ptr = ac > 3 ? (t / (1UL << 16)) : 0;
  
	return CRYS_OK;

} /* End of LLF_RND_EntropyEstimate */

/*****************************************************************************/
/**
 * @brief The function calculates estimation of entropy, generated by 4 ROSCs 
 *        
 * @param[in] ramAddr - The pointer to random source.  
 * @param[in] blockSizeWords - The size of each block of random source in words.  
 * @param[in] countBlocks - The blocks count (according to given ROSCS).
 * @param[in] h_ptr - The pointer to the h-buffer (counts of 0-s and 1-s for each prefix).
 * @param[in] ec_ptr - The pointer to the ec-buffer (equality counters).
 * @param[out] entrSize_ptr - The pointer to count of accumulated Entropy in bits.
 * @param[in] rndState_ptr - The pointer to random State.
 *
 * @return CRYSError_t - according to module definitions  
 */ 
static CRYSError_t LLF_RND_EntropyEstimateFull(
			DxUint32_t *ramAddr,        /*in*/
			DxUint32_t  blockSizeWords, /*in*/
			DxUint32_t  countBlocks,    /*in*/
                        DxUint32_t *entrSize_ptr,   /*out*/
			CRYS_RND_State_t *rndState_ptr, /*in*/
			DxUint32_t  *rndWorkBuff_ptr)   /*in*/
{

	CRYSError_t error;
	DxUint32_t i, totalEntr = 0, currEntr;
	DxUint32_t *h_ptr, *ec_ptr; 
	DxUint32_t *eachRoscEntr_ptr = rndWorkBuff_ptr + CRYS_RND_WORK_BUFF_TMP2_OFFSET;


	/* Initialization */

	h_ptr  = rndWorkBuff_ptr + CRYS_RND_H_BUFF_OFFSET;   
	ec_ptr = rndWorkBuff_ptr + CRYS_RND_EC_BUFF_OFFSET;
	 
	/* estimate entropy for given blocks (ROSCs) */
	for (i = 0; i < countBlocks; i++) {
		
		/* Zeroe working buffer for entr. estimator */
		DX_PAL_MemSetZero(h_ptr, H_BUFF_SIZE_WORDS*4);
		DX_PAL_MemSetZero(ec_ptr, EC_BUFF_SIZE_WORDS*4);
		
		LLF_RND_HistogramUpdate( 
			h_ptr, ec_ptr,  
			ramAddr + i*blockSizeWords,    
			blockSizeWords);       
		
		error = LLF_RND_EntropyEstimate( 
			h_ptr, ec_ptr,
			&currEntr);   /* out - result Entropy size */
	
		if(error)
			goto End;
	
		/*total entropy and separate ROSCs entropy*/
		totalEntr += currEntr; 
		eachRoscEntr_ptr[i] = currEntr;
	}
	
	/* entropy correction: down ~1.5% */
	totalEntr  -= totalEntr >> 6;
	
	*entrSize_ptr = totalEntr;

End:
	return error;
}

/****************************************************************************************/
/***********************      Auxiliary Functions              **************************/
/****************************************************************************************/


/************************************************************************************/
/*!
 * Busy wait upon RNG Interrupt signals.
 * 
 * This function waits RNG interrupt and then disables RNG source. 
 * It uses DX_HAL_WaitInterrupt function
 * to receive common RNG interrupt and then reads and
 * outputs the RNG ISR (status) register.
 * 
 * 
 * \return uint32_t RNG Interrupt status.
 */
static void LLF_RND_WaitRngInterrupt(CRYS_RND_Params_t *trngParams_ptr, DxUint32_t *isr_ptr)
{
	DxUint32_t tmp = 0;

	/* busy wait upon RNG IRR signals */
	while (!tmp) {
		PRINT_INFO("--->into LLF_RND_WaitRngInterrupt,NOW read HOST_RGF_IRR register");
		//irr = DX_HAL_WaitInterrupt(0/*dummy*/);
		//tmp = DX_CC_REG_FLD_GET(HOST_RGF, HOST_RGF_IRR, RNG_INT, irr);
		tmp = 1;
	}
	
	/* stop DMA and the RNG source */
	DX_HAL_WriteCcRegister(DX_CC_REG_OFFSET(RNG,RNG_DMA_ENABLE), 0); 
        /*!!TBD: DMA_ENABLE -> SOURCE_ENABLE */
	DX_HAL_WriteCcRegister(DX_CC_REG_OFFSET(RNG, RND_SOURCE_ENABLE), 0);

	/* Clear RNG interrupt bit in HOST RGF ISR */
	DX_HAL_WriteCcRegister(DX_CC_REG_OFFSET(HOST_RGF, HOST_RGF_ICR), 1UL << DX_HOST_RGF_ICR_RNG_INT_CLEAR_BIT_SHIFT);

	/* read specific RNG interrupt status */
	*isr_ptr = DX_HAL_ReadCcRegister(DX_CC_REG_OFFSET(RNG, RNG_ISR));

	/* clear RNG interrupt status besides HW errors */
	DX_HAL_WriteCcRegister(DX_CC_REG_OFFSET(RNG, RNG_ICR), *isr_ptr);

	return;
} 


/************************************************************************************/
/**
 * The function checks that parameters, loaded in the TRNG HW 
 * are match to parameters, required by trngParams_ptr structures.
 * 
 * @author reuvenl (6/25/2012)
 * 
 * @param rndState_ptr 
 * @param trngParams_ptr 
 * 
 * @return CRYSError_t 
 */
static CRYSError_t LLF_RND_TRNG_CheckHwParams(CRYS_RND_Params_t *trngParams_ptr)
{
	DxUint32_t temp;
	DxBool_t isTrue = DX_TRUE;

	/* check Debug control - masked TRNG tests according to mode */
	temp = DX_HAL_ReadCcRegister(DX_CC_REG_OFFSET(RNG, TRNG_DEBUG_CONTROL));
	if(trngParams_ptr->TrngMode == CRYS_RND_Fast)
		isTrue &= (temp == LLF_RND_HW_DEBUG_CONTROL_VALUE_ON_FAST_MODE);
	else
		isTrue &= (temp == LLF_RND_HW_DEBUG_CONTROL_VALUE_ON_SLOW_MODE);
	/* check samplesCount */
	temp = DX_HAL_ReadCcRegister(DX_CC_REG_OFFSET(RNG,SAMPLE_CNT1));
	isTrue &= (temp == trngParams_ptr->SubSamplingRatio);
	
	/* if any parameters are not match return an Error */
	if(isTrue == DX_FALSE)
		return LLF_RND_TRNG_PREVIOUS_PARAMS_NOT_MATCH_ERROR;
	else
		return CRYS_OK;
}

/**********************************************************************/ 
/*!
 * Copy TRNG source from SRAM to RAM using CC HW descriptors.
 * 
 * \param inSramAddr - Input SRAM address of the source buffer, must be word 
 * aligned. 
 * \param inSize - Size in octets of the source buffer, must be multiple of 
 * word. 
 * \param outRamAddr - Output RAM address of the destination buffer, must be 
 * word aligned. 
 *  
 * \return 0 if success, else 1. 
 * 
 *  Note: The AXI bus secure mode for in/out buffers is used: AxiNs = 0.
 */
static DxUint32_t LLF_RND_DescBypass(DxUint32_t  inSramAddr, DxUint32_t inSize, DxUint32_t *outAddr_ptr)
{
	DxUint32_t error = 0;
	HwDesc_s desc;
	/* Virtual and physical address of allocated temp buffer */
	DxUint8_t *tmpVirtAddr_ptr;
	DX_PAL_DmaBlockInfo_t  tmpBlockInfo;
	uint32_t  numOfBlocks = 1;
	DX_PAL_DmaBufferHandle dmaH;

	/* Allocate contigious buffer for DMA transfer */
	error = DX_PAL_DmaContigBufferAllocate(inSize,
                                &tmpVirtAddr_ptr);
	if(error)
		return error;

	numOfBlocks = 1;
	error = DX_PAL_DmaBufferMap(tmpVirtAddr_ptr,
				    inSize, 
				    DX_PAL_DMA_DIR_FROM_DEVICE, 
				    &numOfBlocks,
				    &tmpBlockInfo,
				    &dmaH);
	if((error) || (numOfBlocks != 1)) {
		return error;
	}
	
	/* Execute BYPASS operation */
	HW_DESC_INIT(&desc);
	HW_DESC_SET_DIN_TYPE(&desc, DMA_SRAM/*inType*/, (DxUint32_t)inSramAddr, 
		inSize, QID_TO_AXI_ID(NO_OS_QUEUE_ID), DEFALUT_AXI_SECURITY_MODE/*inAxiNs*/);
	HW_DESC_SET_DOUT_TYPE(&desc, DMA_DLLI/*outType*/, tmpBlockInfo.blockPhysAddr, 
		inSize, QID_TO_AXI_ID(NO_OS_QUEUE_ID), DEFALUT_AXI_SECURITY_MODE/*outAxiNs*/);
	HW_DESC_SET_FLOW_MODE(&desc, BYPASS);
	AddHWDescSequence(NO_OS_QUEUE_ID, &desc);

	/* Wait */
	WaitForSequenceCompletionPlat();
	DX_PAL_DmaBufferUnmap(tmpVirtAddr_ptr,
			      inSize, 
			      DX_PAL_DMA_DIR_FROM_DEVICE, 
			      numOfBlocks,
			      &tmpBlockInfo,
			      dmaH);

	/* Copy data from temp buffer into RAM output, usung virt. addresses */
	DX_PAL_MemCopy((DxUint8_t*)outAddr_ptr, tmpVirtAddr_ptr, inSize);
	
	/* Release the temp buffer */
	error = DX_PAL_DmaContigBufferFree(inSize, 
				   tmpVirtAddr_ptr);

	return error;
}


/*****************************************************************/ 
/**
 * The function gets next allowed rosc
 * 
 * @author reuvenl (9/12/2012)
 * 
 * @param trngParams_ptr - a pointer to params structure. 
 * @param rosc_ptr - a pointer to previous rosc /in/, and 
 * 			to next rosc /out/.
 * @param isNext - defines is increment of rosc ID needed or not. 
 *      	   if isNext = TRUE - the function shifts rosc by one bit; Then
 *      	   the function checks is this rosc allowed, if yes - updates
 *      	   the rosc, else repeats previous steps. If no roscs allowed -
 *      	   returns an error.
 *  
 *  
 * @return CRYSError_t 
 */
static CRYSError_t LLF_RND_GetFastestRosc(
				CRYS_RND_Params_t *trngParams_ptr, 
				DxUint32_t *rosc_ptr,  /*in/out*/
				DxBool_t isNext )
{
	/* setting rosc */
	do{

		if(*rosc_ptr & trngParams_ptr->RoscsAllowed) {
			return CRYS_OK;
		}else{
			*rosc_ptr <<= 1;
		}

	}while (*rosc_ptr <= 0x08);

	return LLF_RND_TRNG_REQUIRED_ROSCS_NOT_ALLOWED_ERROR;

}

	
	
/**
 * The macros calculates count ROSCs to start, as count of bits "1" in allowed 
 * roscToStart parameter. 
 * 
 * @author reuvenl (9/20/2012)
 * 
 * @param roscsAllowed 
 * @param roscToStart 
 * 
 * @return DxUint32_t 
 */
static DxUint32_t LLF_RND_GetCountRoscs(
			DxUint32_t roscsAllowed,
			DxUint32_t roscToStart)
{
	DxUint32_t countRoscs = 0;

	roscToStart &= roscsAllowed;
	while(roscToStart){
		countRoscs += (roscToStart & 1UL);
		roscToStart >>= 1;
	}

	return countRoscs;
}


/****************************************************************************************/
/*****************************       Public Functions      ******************************/
/****************************************************************************************/


/************************************************************************************/
/**
 * @brief The LLF_RND_TurnOffTrng stops the hardware random bits collection
 *        closes RND clocks and releases HW semaphore.
 *
 * 
 *
 * @return CRYSError_t - On success CRYS_OK is returned, on failure a
 *                        value MODULE_* as defined in ...
 */
void LLF_RND_TurnOffTrng(void)  
{
   	/* LOCAL DECLARATIONS */

   	/* FUNCTION LOGIC */
   	
	/* disable the RND source  */
	DX_HAL_WriteCcRegister(DX_CC_REG_OFFSET(RNG,RND_SOURCE_ENABLE), LLF_RND_HW_RND_SRC_DISABLE_VAL);    
   	
	/* close the Hardware clock */
	DX_HAL_WriteCcRegister(DX_CC_REG_OFFSET(RNG,RNG_CLK_ENABLE), LLF_RND_HW_RND_CLK_DISABLE_VAL);

	#ifndef CRYS_RND_SEM_DISABLE
		DX_VOS_SemGive(SemRndId); 
	#endif

	return;    

}/* END OF LLF_RND_TurnOffTrng*/

/****************************************************************************************/ 
/**
 * @brief The function starts the TRNG with given parameters and ROSCs lengths
 * 
 *      NOTE: It is assumed, that before calling this function, the previously
 *            started TRNG processes were compleated and Interrupts cleared.
 * 
 *      Algorithm:
 *      1. If is continued mode, the function does:
 *              checks actual parameters, loaded in TRNG registers,
 *              vs. user given parameters; if any not matchs - returns Error.
 *         Else /do restart/:
 *      	sets ROSCs to start: for "fast" - all allowed, for "slow" -
 *      	fastest from allowed; gets the user given parameters and sets
 *      	them in the HW, starts the TRNG clocks and sets TRNG parameters
 *      	into HW registers.
 *      2. Initializes the RND DMA according to ROSCs required to start,
 *         initializes the TRNG Interrupt. Enables RNG source.
 *      3. Exits.
 *       
 * @param[in/out] rndState_ptr - The pointer to the internal State buffer of DRNG.
 * @param[in/out] trngParams_ptr - The pointer to structure, containing TRNG parameters.
 * @isContinued[in] isRestart - The variable indicates is a restart required or not.
 * @roscsToStart[in] roscsToStart_ptr - The variable, defining which ROSCs to 
 *      			    start according to bits set: b'0...b'3. When
 *      			    isRestart=TRUE, then:
 *      			    for "fast" - starts all allowed ROSCs, for
 *      			    "slow" - starts fastest ROSC from allowed.
 *      			    Note: if isRestart = 1, then 
 * @sramAddr[in] SRAM address to write the random source.
 *  
 * @return CRYSError_t - no return value
 */
CRYSError_t LLF_RND_StartTrngHW(
			CRYS_RND_State_t  *rndState_ptr,
			CRYS_RND_Params_t *trngParams_ptr,
			DxBool_t           isRestart,
			DxUint32_t         *roscsToStart_ptr,
	                DxUint32_t         sramAddr)
{
	/* LOCAL DECLARATIONS */

	CRYSError_t error = CRYS_OK;    
	DxUint32_t  temp, ehrSamples;
	DxUint32_t i;
	
	/* FUNCTION LOGIC */

	/* Check pointers */
	if((rndState_ptr == DX_NULL) || (trngParams_ptr == DX_NULL) ||
	   (roscsToStart_ptr == DX_NULL) || (sramAddr == 0))
		return LLF_RND_TRNG_ILLEGAL_PTR_ERROR;

	/* get user's TRNG parameters */
	error = RNG_PLAT_SetUserRngParameters(rndState_ptr, trngParams_ptr);

	if(error != CRYS_OK) 
		return error;


	/*--------------------------------------------------------------*/ 
	/* 1. If full restart, get semaphore and set initial ROSCs      */
	/*--------------------------------------------------------------*/ 
	if(isRestart){

		/* get the RNG HW semaphore */
	#ifndef CRYS_RND_SEM_DISABLE
		error = DX_VOS_SemWait(SemRndId, DX_INFINITE); 
		if( error != CRYS_OK )    
		      return error;
	#endif
	
		if(trngParams_ptr->TrngMode == CRYS_RND_Slow) {
			/* set ROSC to 1 (fastest)  */
			*roscsToStart_ptr = 1UL;; 
		}
		else { 
			/* Fast mode - start all allowed ROSCs */
			*roscsToStart_ptr = 0x0f & trngParams_ptr->RoscsAllowed;

			if(*roscsToStart_ptr == 0)
				return LLF_RND_TRNG_REQUIRED_ROSCS_NOT_ALLOWED_ERROR;
		}

		/* init rndState flags to zero */
		rndState_ptr->TrngProcesState = 0;
	}


	/*--------------------------------------------------------------*/ 
	/* 2. Restart the TRNG and set  parameters      		*/
	/*--------------------------------------------------------------*/ 

	/* enable the HW RND clock   */
	DX_HAL_WriteCcRegister(DX_CC_REG_OFFSET(RNG,RNG_CLK_ENABLE), LLF_RND_HW_RND_CLK_ENABLE_VAL);

	/* do software reset */
	DX_HAL_WriteCcRegister(DX_CC_REG_OFFSET(RNG, RNG_SW_RESET), 0x1);
	for( i = 0; i < 20; i++ ); /* delay */

	/* disable the RND source for setting new parameters in HW */
	DX_HAL_WriteCcRegister(DX_CC_REG_OFFSET(RNG,RND_SOURCE_ENABLE), LLF_RND_HW_RND_SRC_DISABLE_VAL);

	/* set TRNG_CONFIG to choose SOP_SEL = 1 - i.e. EHR output */
	DX_HAL_WriteCcRegister(DX_CC_REG_OFFSET(RNG,TRNG_CONFIG), LLF_RND_HW_TRNG_WITH_DMA_CONFIG_VAL);
	
	/* set sampling ratio (rng_clocks) between consequtive bits */
	DX_HAL_WriteCcRegister(DX_CC_REG_OFFSET(RNG,SAMPLE_CNT1), trngParams_ptr->SubSamplingRatio);

	/* Fast mode */
	if(trngParams_ptr->TrngMode == CRYS_RND_Fast){
		/* Debug Control register: NC_BYPASS + TRNG_CRNGT_BYPASS + AUTO_CORRELATE_BYPASS set to 1   */
		DX_HAL_WriteCcRegister(DX_CC_REG_OFFSET(RNG,TRNG_DEBUG_CONTROL), LLF_RND_HW_DEBUG_CONTROL_VALUE_ON_FAST_MODE);

		/* set interrupt mask */
		DX_HAL_WriteCcRegister(DX_CC_REG_OFFSET(RNG,RNG_IMR), LLF_RNG_INT_MASK_ON_FAST_MODE);
		ehrSamples = LLF_RND_HW_DMA_EHR_SAMPLES_NUM_ON_FAST_MODE;
	}

	/* Slow mode  */
	else{ 
		/* Get fastest allowed ROSC */
		error = LLF_RND_GetFastestRosc(
				trngParams_ptr, 
				roscsToStart_ptr,  /*in/out*/
				1/*isNext*/ );
		if(error) 
			return error;
		
		/* Debug Control register: set to 0 - no bypasses */
		DX_HAL_WriteCcRegister(DX_CC_REG_OFFSET(RNG,TRNG_DEBUG_CONTROL), LLF_RND_HW_DEBUG_CONTROL_VALUE_ON_SLOW_MODE);

		/* set interrupt mask */
		DX_HAL_WriteCcRegister(DX_CC_REG_OFFSET(RNG,RNG_IMR), LLF_RNG_INT_MASK_ON_SLOW_MODE);

		/* set EHR samples = 2 /384 bit/ for both AES128 and AES256 */
		ehrSamples = LLF_RND_HW_DMA_EHR_SAMPLES_NUM_ON_SLOW_MODE;

		/* Set watchdog threshold to maximal allowed time (in CPU cycles) 
		   maxTime = (expectTimeInClocks*timeCoeff) */
		temp = LLF_RND_CalcMaxTrngTime(ehrSamples, trngParams_ptr->SubSamplingRatio);
		DX_HAL_WriteCcRegister(DX_CC_REG_OFFSET(RNG,RNG_WATCHDOG_VAL), temp);
	}

	/* set ROSCS to start and EHR samples from each ROSC to DMA transfer */
	DX_HAL_WriteCcRegister(DX_CC_REG_OFFSET(RNG,RNG_DMA_SRC_MASK), *roscsToStart_ptr); 
	DX_HAL_WriteCcRegister(DX_CC_REG_OFFSET(RNG,RNG_DMA_SAMPLES_NUM), ehrSamples); 

	DX_HAL_WriteCcRegister(DX_CC_REG_OFFSET(RNG,RNG_DMA_SRAM_ADDR), (DxUint32_t)sramAddr);

	/* clear RNG interrupts !!TBD */
	temp = 0;
	DX_CC_REG_FLD_SET(HOST, HOST_RGF_ICR, RNG_INT_CLEAR, temp, 1);                                               \
	DX_HAL_WriteCcRegister(DX_CC_REG_OFFSET(HOST_RGF, HOST_RGF_ICR), temp);
	DX_HAL_WriteCcRegister(DX_CC_REG_OFFSET(RNG,RNG_ICR), 0xFFFFFFFF);

	/* enable DMA (automatically enables RNG source) */
	DX_HAL_WriteCcRegister(DX_CC_REG_OFFSET(RNG,RNG_DMA_ENABLE), LLF_RND_HW_RND_DMA_ENABLE_VAL); 
      	                                                      	                                                   	
	/* set indication about current started ROSCs:  */
	/*new started*/
	rndState_ptr->TrngProcesState = (rndState_ptr->TrngProcesState & 0x00ffffff) | (*roscsToStart_ptr << 24); 
	/*total started*/
	rndState_ptr->TrngProcesState |= (*roscsToStart_ptr << 8); 

	/* end  of function */
	return error;
}


/*******************************************************************************/
/**
 * @brief The LLF_RND_GetTrngSource reads random source of needed size from TRNG.
 *
 *        The function is used in Self, Instantiation and Reseeding functions.
 *
 * @param[in/out] rndState_ptr - The pointer to the internal State buffer of DRNG.
 * @param[in/out] trngParams_ptr - The pointer to structure, containing TRNG parameters.
 * @isContinued[in] isContinued - The variable indicates is the required process should  
 *                  continue a  previous one or restart TRNG.
 * @entropySize_ptr[in/out] - The pointer to size of entropy in bits: input - required,
 *                            output - actual size.
 * @sourceOut_ptr_ptr[out] - The pointer to to pointer to the entropy source buffer. 
 *      		     The buffer contains one empty word for using by CRYS level 
 *      		     and then buffer for output the rng source.  
 * @param[out] - sourceOutSize_ptr - The pointer to the size in bytes of entropy source    			   
 *      			    in - requirred size, output - actual size. 
 * @param[in/out] - rndWorkBuff_ptr - The pointer to the temp buffer for allocation of 
 *      			   estimator buffers.
 * 
 *
 * @return CRYSError_t - On success CRYS_OK is returned, on failure a
 *                        value MODULE_* as defined in ...
 */
CRYSError_t LLF_RND_GetTrngSource( 
			CRYS_RND_State_t   *rndState_ptr,     /*in/out*/ 
			CRYS_RND_Params_t  *trngParams_ptr,   /*in/out*/        
			DxBool_t            isContinued,      /*in*/
			DxUint32_t         *entropySize_ptr,  /*in/out*/
			DxUint32_t        **sourceOut_ptr_ptr, /*out*/ 								     
			DxUint32_t         *sourceOutSize_ptr,/*in/out*/
			DxUint32_t         *rndWorkBuff_ptr)  /*in*/
{
	/* LOCAL DECLARATIONS */
	
	/* The return error identifier */
	CRYSError_t error = 0, descrError = 0;

	DxInt32_t  i;
	DxUint32_t isr;
	DxUint32_t tmp, requirEntropy;
	DxUint32_t roscToStart, sizeToProcessBytes;
	DxUint32_t sramAddr, *ramAddr;



	/* FUNCTION LOGIC */
	PRINT_INFO("--->NOW into LLF_RND_GetTrngSource\n");
	/* ............... local initializations .............................. */
	/* -------------------------------------------------------------------- */
	/* initializing the Error to O.K */
	error = CRYS_OK;

	/* Set source RAM address with offset 8 bytes from sourceOut address in
	  order to remain empty bytes for CRYS operations */
	*sourceOut_ptr_ptr = rndWorkBuff_ptr + CRYS_RND_SRC_BUFF_OFFSET_WORDS;
	ramAddr = *sourceOut_ptr_ptr + CRYS_RND_TRNG_SRC_INNER_OFFSET_WORDS;
	sramAddr = LLF_RND_HW_DMA_SRAM_ADDRESS;
	/* init to 0 for Slow mode */
	*sourceOutSize_ptr = 0; 

	/* Case of RND KAT or TRNG KAT testing  */ 
	if((rndState_ptr->StateFlag & CRYS_RND_KAT_DRBG_mode) ||
	   (rndState_ptr->StateFlag & CRYS_RND_KAT_TRNG_mode)){
		/* set source sizes given by the user in KAT test and placed   
		   in the rndWorkBuff with offset CRYS_RND_SRC_BUFF_OFFSET_WORDS */
		*sourceOutSize_ptr = (*sourceOut_ptr_ptr)[0];

		/* Go to Estimator */
		if(rndState_ptr->StateFlag & CRYS_RND_KAT_TRNG_mode){
			tmp = 1; /*count blocks in estimation*/
			goto Estimator;
		} else 
			goto End;
	}

	/* If not continued mode, set TRNG parameters and restart TRNG 	*/
	/*--------------------------------------------------------------*/ 
	if(isContinued == DX_FALSE) {

		/* set user's TRNG parameters */
		error = RNG_PLAT_SetUserRngParameters(rndState_ptr, trngParams_ptr);
		if(error != CRYS_OK) 
			return error;

		/* Set instantiation, TRNG errors and time   * 
		* exceeding bits of State to 0  	     */
		rndState_ptr->StateFlag &= ~(CRYS_RND_Instantiated | 
					     CRYS_RND_InstantReseedAutocorrErrors |
					     CRYS_RND_InstantReseedTimeExceed | 
					     CRYS_RND_InstantReseedLessEntropy);	

		/* Full restart TRNG */
		error = LLF_RND_StartTrngHW(
				rndState_ptr, trngParams_ptr, 
				DX_TRUE/*isRestart*/, 
				&roscToStart, 
				sramAddr);

		/*Note: in case of error the TRNG HW is still not started*/
		if(error) 
			{
			PRINT_INFO("--->LLF_RND_StartTrngHW() return value 0x%x\n",error);
			goto End; 
			}
	}

	/* On continued mode check HW TRNG */
	else {
		/* check TRNG parameters */
		error = LLF_RND_TRNG_CheckHwParams(trngParams_ptr);
		if(error != CRYS_OK)
			goto End;
		
		/* previously started ROSCs */
		roscToStart = (rndState_ptr->TrngProcesState & 0xff000000)>>24;
	}

	/*====================================================*/ 
	/*====================================================*/ 
	/*         Processing after previous start            */
	/*====================================================*/ 
	/*====================================================*/ 

	/*=========================================================*/ 
	/* Fast mode processing (yet started all 4 ROSCs):   	        
	   wait interrupt, estimate entropy and output the source  */
	/*=========================================================*/
	if(trngParams_ptr->TrngMode == CRYS_RND_Fast) {
               PRINT_INFO("--->now before LLF_RND_WaitRngInterrupt \n");
		/* wait and output irr = RNG_ISR */
		LLF_RND_WaitRngInterrupt(trngParams_ptr, &isr);

		/* source size from one rosc and from all started rosc's*/
		sizeToProcessBytes = LLF_RND_HW_DMA_EHR_SAMPLES_NUM_ON_FAST_MODE * LLF_RND_HW_TRNG_EHR_WIDTH_IN_BYTES;

		/* calculate count of rosc's started */
		tmp = LLF_RND_GetCountRoscs(trngParams_ptr->RoscsAllowed, roscToStart);

		*sourceOutSize_ptr = tmp * sizeToProcessBytes;

		/* Execute BYPASS operation similar to DescBypass() */
		error = LLF_RND_DescBypass(sramAddr/*in*/, 
				*sourceOutSize_ptr/*inSize*/, 
				(DxUint32_t*)ramAddr/*out*/);
		if(error)
			goto End;

	#ifdef BIG__ENDIAN
		/* set endianness */
		for(i = 0; i < (*sourceOutSize_ptr)/4; ++i) {
			ramAddr[i] = CRYS_COMMON_REVERSE32(ramAddr[i]);
		}
	#endif

Estimator:
		/* save entropy */
		requirEntropy = *entropySize_ptr;

		error = LLF_RND_EntropyEstimateFull(
			ramAddr,               /*in*/
			sizeToProcessBytes>>2, /*in*/
			tmp/*count blocks*/,   /*in*/
                        entropySize_ptr,       /*out*/
			rndState_ptr,          /*in*/
			rndWorkBuff_ptr);      /*in*/ 

		if(error)
			goto End;
		
		/* save entropy size for testing */
		rndState_ptr->EntropySizeBits = *entropySize_ptr;

		/* if not KAT mode check entropy */
		if(!(rndState_ptr->StateFlag & (CRYS_RND_KAT_DRBG_mode | CRYS_RND_KAT_TRNG_mode))){
			if(*entropySize_ptr == 0) 
				error = LLF_RND_TRNG_NULL_ENTROPY_ERROR;
			else if(*entropySize_ptr < requirEntropy)
				error = LLF_RND_TRNG_LOW_ENTROPY_ERROR;
			goto End;
		}

		/* update processed ROSCs */
		rndState_ptr->TrngProcesState |= ((rndState_ptr->TrngProcesState >> 8) & 0x00FF0000);
		/*clean started & not processed*/
		rndState_ptr->TrngProcesState &= 0x00FFFFFF; 
	}


	/*====================================================*/ 
	/* Slow mode processing: start Roscs sequentionally - * 
	* from fast to slow Rosc 			      */		      
	/*====================================================*/ 
	else {  
		sizeToProcessBytes = LLF_RND_HW_DMA_EHR_SAMPLES_NUM_ON_SLOW_MODE * LLF_RND_HW_TRNG_EHR_WIDTH_IN_BYTES;

		for(i = 0; i < 4; ++i) {
			
			/* wait RNG interrupt: isr signals error bits */
			LLF_RND_WaitRngInterrupt(trngParams_ptr, &isr);

			/* get DMA done status */
			tmp = (isr >> DX_RNG_ISR_RNG_DMA_DONE_BIT_SHIFT) & 1UL;

			/* update output source size and sramAddr */
			*sourceOutSize_ptr += sizeToProcessBytes;
			sramAddr += sizeToProcessBytes;
	
			/* update total processed ROSCs */
			rndState_ptr->TrngProcesState |= ((rndState_ptr->TrngProcesState >> 8) & 0x00FF0000);
			/*clean started & not processed*/
			rndState_ptr->TrngProcesState &= 0x00FFFFFF;     

			/* if DMA done and no HW errors - exit */
			if((tmp == 1) && ((isr & LLF_RNG_ERRORS_MASK) == 0)) 
				break;
			
			else {/* case of erors or watchdog exceed  - try next rosc */

				/*  if no remain roscs to start, return error */
				if(roscToStart == 0x8) {
					error = LLF_RND_TRNG_GENERATION_NOT_COMPLETED_ERROR;
					break;
				}
				else { 
					/* Call StartTrng, whith next ROSC */
					roscToStart <<= 1;
					error = LLF_RND_StartTrngHW(
						rndState_ptr, trngParams_ptr, 
						DX_FALSE/*isRestart*/,
						&roscToStart, 
						sramAddr);

					if(error != CRYS_OK)
						goto End; 
				}
				LOGI("current error %x",error);
			}
		}

		/* Execute BYPASS operation from initial sramAddr */
		sramAddr = LLF_RND_HW_DMA_SRAM_ADDRESS;

		descrError = LLF_RND_DescBypass(sramAddr/*in*/, 
				*sourceOutSize_ptr/*inSize*/, 
				(DxUint32_t*)ramAddr/*out*/);
		if(descrError){
			/* Note: Priority to SW error */
			PRINT_INFO("Note: Priority to SW error,LLF_RND_DescBypass %d\n ",descrError);
			error = descrError; 
			goto End;
		}

	} /* end slow mode */


	/* ................. end of function ..................................... */
	/* ----------------------------------------------------------------------- */
End:    

	/* turn the RNG off    */
	if((rndState_ptr->StateFlag & CRYS_RND_KAT_TRNG_mode) == 0){
		LLF_RND_TurnOffTrng();
	}
	
	return error;


}/* END of LLF_RND_GetTrngSource */
