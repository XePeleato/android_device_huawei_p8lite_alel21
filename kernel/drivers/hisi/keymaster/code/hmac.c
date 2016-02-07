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

#define DX_PAL_LOG_CUR_COMPONENT DX_LOG_MASK_CRYS_SYM_DRIVER

#include "dx_pal_types.h"
#include "dx_pal_mem.h"
#include "sym_crypto_driver.h"
#include "dx_error.h"
#include "cc_plat.h"
#include "bsp.h"
#include "mlli.h"
#include "hw_queue.h"
#include "sep_ctx.h"
#include "completion.h"
#include "hash.h"
#include "hmac.h"
#include "hmac_defs.h"




DX_PAL_COMPILER_ASSERT(sizeof(struct sep_ctx_hmac)==SEP_CTX_SIZE,"sep_ctx_hmac is larger than 128 bytes!");
DX_PAL_COMPILER_ASSERT(sizeof(ZeroBlock) >= (SEP_HASH_LENGTH_WORDS * sizeof(uint32_t)), "ZeroBlock is too small for HASH_LENGTH field init.");
DX_PAL_COMPILER_ASSERT(sizeof(ZeroBlock) >= SEP_AES_128_BIT_KEY_SIZE, "ZeroBlock is too small for key field init.");

/******************************************************************************
*				GLOBALS
******************************************************************************/
extern const uint32_t gLarvalSha1Digest[];
extern const uint32_t gLarvalSha224Digest[];
extern const uint32_t gLarvalSha256Digest[];
#ifdef DX_CONFIG_HASH_SHA_512_SUPPORTED
extern const uint32_t gLarvalSha384Digest[];
extern const uint32_t gLarvalSha512Digest[];
#endif

const uint32_t gOpadDecrypedBlock[] DX_SRAM_CONST = { HMAC_DECRYPTED_OPAD_CONST_BLOCK };
const uint32_t gIpadDecrypedBlock[] DX_SRAM_CONST = { HMAC_DECRYPTED_IPAD_CONST_BLOCK };
#define HMAC_IPAD_CONST_BLOCK		0x36363636
#define HMAC_OPAD_CONST_BLOCK		0x5C5C5C5C
/******************************************************************************
*				PRIVATE FUNCTIONS
******************************************************************************/

#ifdef DX_CONFIG_HW_HASH_XOR_WORKAROUND
/*!
 * This function is used to initialize the HMAC machine to perform the HMAC
 * operations. This should be the first function called.
 * 
 * \param qid The HW queue Id.
 * \param aesIv Pointer to AES IV data.
 * \param aesData Pointer to AES input data
 * \param aesDataSize AES input data size
 * \param hmode HASH mode
 * \param hashCurrentLength [out] Pointer on HASH current length
 * \param hashResult [out] Pointer on HASH output buffer
 * 
 * \return int One of DX_SYM_* error codes defined in dx_error.h.
 */

/*This code is legacy that would not run in CC44 so it was not updated to the new mechanism*/
static int ProcessAesToHash(int qid, uint8_t *aesIv, uint8_t *aesData, uint32_t aesDataSize, 
	enum sep_hash_mode hmode, uint8_t *hashCurrentLength, uint8_t *hashResult)
{
	HwDesc_s desc;
	uint32_t lhmode;
	uint32_t DigestSize;
	uint32_t digestAddr = 0;
	int drvRc = DX_RET_OK;

	drvRc = GetHashHwMode(hmode, &lhmode);
	if (drvRc != DX_RET_OK) {
		return drvRc;
	}

	HW_DESC_INIT(&desc);
	HW_DESC_SET_SETUP_MODE(&desc, SETUP_LOAD_STATE1);
	HW_DESC_SET_KEY_SIZE_AES(&desc, SEP_AES_128_BIT_KEY_SIZE);
	HW_DESC_SET_CIPHER_MODE(&desc, SEP_CIPHER_CTR);
	HW_DESC_SET_CIPHER_CONFIG0(&desc, SEP_CRYPTO_DIRECTION_ENCRYPT);
	HW_DESC_SET_STATE_DIN_PARAM(&desc, (uint32_t)aesIv, SEP_AES_BLOCK_SIZE);
	HW_DESC_SET_FLOW_MODE(&desc, S_DIN_to_AES);
	HW_DESC_LOCK_QUEUE(&desc, LOCK_HW_QUEUE);
	AddHWDescSequence(qid, &desc);

	HW_DESC_INIT(&desc);
	HW_DESC_SET_SETUP_MODE(&desc, SETUP_LOAD_KEY0);
	HW_DESC_SET_CIPHER_MODE(&desc, SEP_CIPHER_CTR);
	HW_DESC_SET_CIPHER_CONFIG0(&desc, SEP_CRYPTO_DIRECTION_ENCRYPT);
	HW_DESC_SET_KEY_SIZE_AES(&desc, SEP_AES_128_BIT_KEY_SIZE);
	HW_DESC_SET_STATE_DIN_PARAM(&desc, (uint32_t)ZeroBlock, SEP_AES_128_BIT_KEY_SIZE);
	HW_DESC_SET_FLOW_MODE(&desc, S_DIN_to_AES);
	HW_DESC_LOCK_QUEUE(&desc, LOCK_HW_QUEUE);
	AddHWDescSequence(qid, &desc);

	/* SHA224 uses SHA256 HW mode with different init. val. */
	drvRc = GetHashHwDigestSize(hmode, &DigestSize);
	if (drvRc != DX_RET_OK) {
		return drvRc; 
	}
    
	switch(hmode) {
	case SEP_HASH_SHA1:
		digestAddr = (uint32_t)gLarvalSha1Digest;
		break;
	case SEP_HASH_SHA224:
		digestAddr = (uint32_t)gLarvalSha224Digest;
		break;
	case SEP_HASH_SHA256:
		digestAddr = (uint32_t)gLarvalSha256Digest;
		break;
	default:
		DX_PAL_LOG_ERR("Unsupported hash mode %d\n", hmode);
		return DX_RET_UNSUPP_ALG_MODE;
	}
	/* load initial hash digest */
	HW_DESC_INIT(&desc);
	HW_DESC_SET_SETUP_MODE(&desc, SETUP_LOAD_STATE0);
	HW_DESC_SET_CIPHER_MODE(&desc, lhmode);
	HW_DESC_SET_CIPHER_DO(&desc, DO_NOT_PAD);
	HW_DESC_SET_STATE_DIN_PARAM(&desc, digestAddr, DigestSize);
	HW_DESC_SET_FLOW_MODE(&desc, S_DIN_to_HASH);
	HW_DESC_LOCK_QUEUE(&desc, LOCK_HW_QUEUE);
	AddHWDescSequence(qid, &desc);

	/* load the hash zero length */
	HW_DESC_INIT(&desc);
	HW_DESC_SET_CIPHER_MODE(&desc, lhmode);
	HW_DESC_SET_CIPHER_DO(&desc, DO_NOT_PAD);
	HW_DESC_SET_STATE_DIN_PARAM(&desc, (uint32_t)ZeroBlock, SEP_HASH_LENGTH_WORDS * sizeof(uint32_t));
	HW_DESC_SET_FLOW_MODE(&desc, S_DIN_to_HASH);
	HW_DESC_SET_SETUP_MODE(&desc, SETUP_LOAD_KEY0);
	HW_DESC_LOCK_QUEUE(&desc, LOCK_HW_QUEUE);
	AddHWDescSequence(qid, &desc);

	/* process the AES to HASH flow */
	HW_DESC_INIT(&desc);
	HW_DESC_SET_STATE_DIN_PARAM(&desc, (uint32_t)aesData, aesDataSize);
	HW_DESC_SET_FLOW_MODE(&desc, AES_to_HASH);
	HW_DESC_LOCK_QUEUE(&desc, LOCK_HW_QUEUE);
	AddHWDescSequence(qid, &desc);

	/* store the hash digest result in the context */
	HW_DESC_INIT(&desc);
	HW_DESC_SET_CIPHER_MODE(&desc, lhmode);
	HW_DESC_SET_CIPHER_DO(&desc, DO_NOT_PAD);
	HW_DESC_SET_STATE_DOUT_PARAM(&desc, (uint32_t)hashResult, DigestSize);
	HW_DESC_SET_FLOW_MODE(&desc, S_HASH_to_DOUT);
	HW_DESC_SET_SETUP_MODE(&desc, SETUP_WRITE_STATE0);
	HW_DESC_LOCK_QUEUE(&desc, LOCK_HW_QUEUE);
	AddHWDescSequence(qid, &desc);

	/* store current hash length in the private context */
	HW_DESC_INIT(&desc);
	HW_DESC_SET_CIPHER_MODE(&desc, lhmode);
	HW_DESC_SET_CIPHER_DO(&desc, DO_NOT_PAD);
	HW_DESC_SET_STATE_DOUT_PARAM(&desc, (uint32_t)hashCurrentLength, sizeof(uint32_t) * SEP_HASH_LENGTH_WORDS);
	HW_DESC_SET_FLOW_MODE(&desc, S_HASH_to_DOUT);
	HW_DESC_SET_SETUP_MODE(&desc, SETUP_WRITE_STATE1);
	HW_DESC_LOCK_QUEUE(&desc, LOCK_HW_QUEUE);
	AddHWDescSequence(qid, &desc);

	return DX_RET_OK;
}
#else

static int ProcessHmacPad(int qid, uint32_t constPadData, uint8_t *hashData, uint32_t hashDataSize, 
	enum sep_hash_mode hmode, uint8_t *hashCurrentLength, uint8_t *hashResult, struct sep_ctx_hmac *pCtx)
{

	HwDesc_s desc;
	uint32_t lhmode;
	uint32_t DigestSize;
	uint32_t digestAddr;
	int drvRc = DX_RET_OK;


	drvRc = GetHashHwMode(hmode, &lhmode);
	if (drvRc != DX_RET_OK) {
		return drvRc;
	}


	/* SHA224 uses SHA256 HW mode with different init. val. */
	drvRc = GetHashHwDigestSize(hmode, &DigestSize);
	if (drvRc != DX_RET_OK) {
		return drvRc; 
	}

    
#ifdef DX_CC_SRAM_INDIRECT_ACCESS
	/* get the SRAM address right after the context cache */
	digestAddr = (uint32_t)((uint8_t*)pCtx + SEP_CTX_SIZE);
	switch(hmode) {
	case SEP_HASH_SHA1:
		 WriteContextField((uint32_t*)digestAddr, gLarvalSha1Digest, SEP_SHA1_DIGEST_SIZE);
		break;
	case SEP_HASH_SHA224:
		WriteContextField((uint32_t*)digestAddr, gLarvalSha224Digest, SEP_SHA256_DIGEST_SIZE);
		break;
	case SEP_HASH_SHA256:
		WriteContextField((uint32_t*)digestAddr, gLarvalSha256Digest, SEP_SHA256_DIGEST_SIZE);
		break;
#ifdef DX_CONFIG_HASH_SHA_512_SUPPORTED
	case SEP_HASH_SHA384:
		WriteContextField((uint32_t*)digestAddr, gLarvalSha384Digest, SEP_SHA512_DIGEST_SIZE);
		break;
	case SEP_HASH_SHA512:
		WriteContextField((uint32_t*)digestAddr, gLarvalSha512Digest, SEP_SHA512_DIGEST_SIZE);
		break;
#endif
	default:
		DX_PAL_LOG_ERR("Unsupported hash mode %d\n", hmode);
		return DX_RET_UNSUPP_ALG_MODE;
	}
#else
	switch(hmode) {
	case SEP_HASH_SHA1:
		digestAddr = (uint32_t)gLarvalSha1Digest;
		break;
	case SEP_HASH_SHA224:
		digestAddr = (uint32_t)gLarvalSha224Digest;
		break;
	case SEP_HASH_SHA256:
		digestAddr = (uint32_t)gLarvalSha256Digest;
		break;
#ifdef DX_CONFIG_HASH_SHA_512_SUPPORTED
	case SEP_HASH_SHA384:
		digestAddr = (uint32_t)gLarvalSha384Digest;
		break;
	case SEP_HASH_SHA512:
		digestAddr = (uint32_t)gLarvalSha512Digest;
		break;
#endif
	default:
		DX_PAL_LOG_ERR("Unsupported hash mode %d\n", hmode);
		return DX_RET_UNSUPP_ALG_MODE;
	}
#endif


	/* 1. Load hash initial state */
	HW_DESC_INIT(&desc);
	HW_DESC_SET_CIPHER_MODE(&desc, lhmode);
	HW_DESC_SET_STATE_DIN_PARAM(&desc, (digestAddr), DigestSize);
	HW_DESC_SET_FLOW_MODE(&desc, S_DIN_to_HASH);
	HW_DESC_SET_SETUP_MODE(&desc, SETUP_LOAD_STATE0);
	AddHWDescSequence(0, &desc);


	/* 2. load the hash current length, should be greater than zero */
	HW_DESC_INIT(&desc);
	HW_DESC_SET_CIPHER_MODE(&desc, lhmode);
	HW_DESC_SET_DIN_CONST(&desc, 0,SEP_HASH_LENGTH_WORDS * sizeof(uint32_t));
	HW_DESC_SET_FLOW_MODE(&desc, S_DIN_to_HASH);
	HW_DESC_SET_SETUP_MODE(&desc, SETUP_LOAD_KEY0);
	AddHWDescSequence(0, &desc);


	/* 3. prapare pad key - IPAD or OPAD */
	HW_DESC_INIT(&desc);
	HW_DESC_SET_XOR_VAL(&desc, constPadData);
	HW_DESC_SET_CIPHER_MODE(&desc, lhmode);
	HW_DESC_SET_FLOW_MODE(&desc, S_DIN_to_HASH);
	HW_DESC_SET_SETUP_MODE(&desc, SETUP_LOAD_STATE1);
	AddHWDescSequence(0, &desc);


	/* 4. perform HASH update */
	HW_DESC_INIT(&desc);
	HW_DESC_SET_STATE_DIN_PARAM(&desc, hashData, hashDataSize);
	HW_DESC_SET_CIPHER_MODE(&desc, lhmode);
	HW_DESC_SET_XOR_ACTIVE(&desc);
	HW_DESC_SET_FLOW_MODE(&desc, DIN_HASH);
	AddHWDescSequence(0, &desc);


	/* 5. Get the digset */
	HW_DESC_INIT(&desc);
	HW_DESC_SET_CIPHER_MODE(&desc, lhmode);
	HW_DESC_SET_STATE_DOUT_PARAM(&desc, (uint32_t)hashResult, DigestSize);
	HW_DESC_SET_FLOW_MODE(&desc, S_HASH_to_DOUT);
	HW_DESC_SET_SETUP_MODE(&desc, SETUP_WRITE_STATE0);
	AddHWDescSequence(0, &desc);


	/*6. store current hash length in the private context */
	HW_DESC_INIT(&desc);
	HW_DESC_SET_CIPHER_MODE(&desc, lhmode);
	HW_DESC_SET_CIPHER_DO(&desc, DO_NOT_PAD);
	HW_DESC_SET_STATE_DOUT_PARAM(&desc, (uint32_t)hashCurrentLength, sizeof(uint32_t) * SEP_HASH_LENGTH_WORDS);
	HW_DESC_SET_FLOW_MODE(&desc, S_HASH_to_DOUT);
	HW_DESC_SET_SETUP_MODE(&desc, SETUP_WRITE_STATE1);
	HW_DESC_LOCK_QUEUE(&desc, LOCK_HW_QUEUE);
	AddHWDescSequence(qid, &desc);

	return DX_RET_OK;

}
#endif

/******************************************************************************
*				FUNCTIONS
******************************************************************************/

/*!
 * This function is used to initialize the HMAC machine to perform the HMAC
 * operations. This should be the first function called.
 * 
 * \param pCtx A pointer to the context buffer in SRAM.
 *
 * \return int One of DX_SYM_* error codes defined in dx_error.h.
 */
int InitHmac(struct sep_ctx_hmac *pCtx)
{
	uint32_t BlockSize;
	uint32_t KeySize;
	SepHashPrivateContext_s *PrivateContext = (SepHashPrivateContext_s *)pCtx;
	int qid = CURR_QUEUE_ID(); /* qid is stored in pxTaskTag field */
	int drvRc = DX_RET_OK;

	drvRc = GetHashBlockSize(ReadContextWord(&pCtx->mode), &BlockSize);
	if (drvRc != DX_RET_OK) {
		goto EndWithErr;
	}


	/* pad the key with zeros */
	KeySize = ReadContextWord(&pCtx->k0_size);
#ifndef DX_CC_SRAM_INDIRECT_ACCESS
	ClearCtxField((pCtx->k0 + KeySize), (BlockSize - KeySize));
#else
	/*due to the limited access to the SRAM (words alignment)the key shold be Read/Modify/Write if the key is not aligned to words*/
	if(!(KeySize%sizeof(uint32_t))) {
		ClearCtxField((pCtx->k0 + KeySize), (BlockSize - KeySize));
	} else {
		uint32_t keywords[SEP_SHA512_BLOCK_SIZE/sizeof(uint32_t)];
		/* read the whole key and write it back */
		/*T.B.D - optimize this sequence to one word only*/
		ReadContextField(pCtx->k0,keywords,BlockSize);
		DX_PAL_MemSetZero(&((uint8_t*)&keywords[0])[KeySize],(BlockSize - KeySize));
		WriteContextField(pCtx->k0,keywords,BlockSize);
	}
#endif
	drvRc = InitHash((struct sep_ctx_hash *)pCtx);
	if (drvRc != DX_RET_OK) {
		goto EndWithErr;
	}

#ifdef DX_CONFIG_HW_HASH_XOR_WORKAROUND
/*This code is legacy that would not run in CC44 so it was not updated to the new mechanism*/
	AddSetRegisterValueDesc(qid, AES_CTR_NO_COUNTER_INC_REG_ADDR, 1, LOCK_HW_QUEUE);
	drvRc = ProcessAesToHash(qid, (uint8_t*)gIpadDecrypedBlock, pCtx->k0, BlockSize, pCtx->mode,
		(uint8_t*)PrivateContext->CurrentDigestedLength, pCtx->digest);
	if (drvRc != DX_RET_OK) {
		AddSetRegisterValueDesc(qid, AES_CTR_NO_COUNTER_INC_REG_ADDR, 0, UNLOCK_HW_QUEUE);
		goto EndWithErr;
	}
	drvRc = ProcessAesToHash(qid, (uint8_t*)gOpadDecrypedBlock, pCtx->k0, BlockSize, pCtx->mode,
		(uint8_t*)PrivateContext->CurrentDigestedLength, pCtx->k0 );
	if (drvRc != DX_RET_OK) {
		AddSetRegisterValueDesc(qid, AES_CTR_NO_COUNTER_INC_REG_ADDR, 0, UNLOCK_HW_QUEUE);
		goto EndWithErr;
	}
	AddSetRegisterValueDesc(qid, AES_CTR_NO_COUNTER_INC_REG_ADDR, 0, UNLOCK_HW_QUEUE);
#else
	drvRc = ProcessHmacPad(qid, HMAC_IPAD_CONST_BLOCK, pCtx->k0, BlockSize, ReadContextWord(&pCtx->mode),
		(uint8_t*)PrivateContext->CurrentDigestedLength, pCtx->digest, pCtx);
	if (drvRc != DX_RET_OK) {
		goto EndWithErr;
	}
	drvRc = ProcessHmacPad(qid, HMAC_OPAD_CONST_BLOCK, pCtx->k0, BlockSize, ReadContextWord(&pCtx->mode),
		(uint8_t*)PrivateContext->CurrentDigestedLength, pCtx->k0, pCtx);
	if (drvRc != DX_RET_OK) {
		goto EndWithErr;
	}
#endif

EndWithErr:
	return drvRc;
}


/********************************************************************************/
/********************************************************************************/
/*!! we do not implement "ProcessHmac" since it directly calls ProcessHash 	*/
/********************************************************************************/
/********************************************************************************/

/*!
 * This function is used as finish operation of the HMAC machine.
 * The function may be called after "InitHmac".
 * 
 * \param pCtx A pointer to the AES context buffer in SRAM.
 * \param pDmaInputBuffer A structure which represents the DMA input buffer.
 * \param pDmaOutputBuffer A structure which represents the DMA output buffer.
 * 
 * \return int One of DX_SYM_* error codes defined in dx_error.h.
 */
int FinalizeHmac(struct sep_ctx_hmac *pCtx, DmaBuffer_s *pDmaInputBuffer, DmaBuffer_s *pDmaOutputBuffer)
{
	SepHashPrivateContext_s *PrivateHashContext = (SepHashPrivateContext_s *)pCtx;
	DmaBuffer_s HashDmaBuffer;
	uint32_t DigestSize;
	int drvRc = DX_RET_OK;

	drvRc = GetHashDigestSize(ReadContextWord(&pCtx->mode), &DigestSize);
	if (drvRc != DX_RET_OK) {
		goto EndWithErr;
	}

	/* finalize user data (data may be zero length) */
	drvRc = FinalizeHash((struct sep_ctx_hash *)pCtx, pDmaInputBuffer, NULL);
	if (drvRc != DX_RET_OK) {
		goto EndWithErr;
	}

	WriteContextWord(&PrivateHashContext->hmacFinalization,1);

	HashDmaBuffer.pData = (uint32_t)pCtx->digest;
	HashDmaBuffer.size = DigestSize;
	HashDmaBuffer.dmaBufType = DMA_BUF_SEP;
	HashDmaBuffer.axiNs = DEFALUT_AXI_SECURITY_MODE;

	drvRc = FinalizeHash((struct sep_ctx_hash *)pCtx, &HashDmaBuffer, NULL);

EndWithErr:
	return drvRc;
}

