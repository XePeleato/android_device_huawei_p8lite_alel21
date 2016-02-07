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

#include "bsp.h"
#include "dx_pal_mem.h"
#include "cc_plat.h"
#include "mlli.h"
#include "hw_queue.h"
#include "completion.h"
#include "dx_error.h"
#include "aead.h"


DX_PAL_COMPILER_ASSERT(sizeof(struct sep_ctx_aead)==SEP_CTX_SIZE,"sep_ctx_aead is larger than 128 bytes!");

/******************************************************************************
*				PRIVATE FUNCTIONS
******************************************************************************/

static void LoadAeadHeaderMac(int qid, struct sep_ctx_aead *pCtx)
{
	HwDesc_s desc;

	HW_DESC_INIT(&desc);
	HW_DESC_SET_CIPHER_MODE(&desc, SEP_AEAD_MODE_CCM_A);
	HW_DESC_SET_KEY_SIZE_AES(&desc, ReadContextWord(&pCtx->key_size));
	HW_DESC_SET_FLOW_MODE(&desc, S_DIN_to_AES);
	HW_DESC_SET_STATE_DIN_PARAM(&desc, (uint32_t)pCtx->mac_state, SEP_AES_BLOCK_SIZE);
	HW_DESC_SET_CIPHER_CONFIG0(&desc, ReadContextWord(&pCtx->direction));
	HW_DESC_SET_SETUP_MODE(&desc, SETUP_LOAD_STATE0);
	AddHWDescSequence(qid, &desc);
}

static void StoreAeadHeaderMac(int qid, struct sep_ctx_aead *pCtx)
{
	HwDesc_s desc;

	HW_DESC_INIT(&desc);
	HW_DESC_SET_CIPHER_MODE(&desc, SEP_AEAD_MODE_CCM_A);
	HW_DESC_SET_STATE_DOUT_PARAM(&desc, (uint32_t)pCtx->mac_state, SEP_AES_BLOCK_SIZE);
	HW_DESC_SET_FLOW_MODE(&desc, S_AES_to_DOUT);
	HW_DESC_SET_SETUP_MODE(&desc, SETUP_WRITE_STATE0);
	HW_DESC_SET_CIPHER_CONFIG0(&desc, ReadContextWord(&pCtx->direction));
	AddHWDescSequence(qid, &desc);
}

static void LoadAeadCipherMac(int qid, struct sep_ctx_aead *pCtx)
{
	HwDesc_s desc;
	SepAeadPrivateContext_s *pAeadPrivateCtx = (SepAeadPrivateContext_s *)pCtx->reserved;

	HW_DESC_INIT(&desc);
	HW_DESC_SET_CIPHER_MODE(&desc, ReadContextWord(&pAeadPrivateCtx->internalMode));
	HW_DESC_SET_KEY_SIZE_AES(&desc, ReadContextWord(&pCtx->key_size));
	HW_DESC_SET_FLOW_MODE(&desc, S_DIN_to_AES2);
	HW_DESC_SET_CIPHER_CONFIG0(&desc, ReadContextWord(&pCtx->direction));
	HW_DESC_SET_SETUP_MODE(&desc, SETUP_LOAD_STATE0);
	HW_DESC_SET_STATE_DIN_PARAM(&desc, (uint32_t)pCtx->mac_state, SEP_AES_BLOCK_SIZE);
	HW_DESC_SET_CIPHER_CONFIG1(&desc, TUNNEL_ON);
	if (ReadContextWord(&pAeadPrivateCtx->internalMode) == SEP_AEAD_MODE_CCM_PE) {
		HW_DESC_SET_CIPHER_CONFIG0(&desc, DESC_DIRECTION_ENCRYPT_ENCRYPT);
	} else {
		HW_DESC_SET_CIPHER_CONFIG0(&desc, DESC_DIRECTION_DECRYPT_ENCRYPT);
	}
	AddHWDescSequence(qid, &desc);
}

static void StoreAeadCipherMac(int qid, struct sep_ctx_aead *pCtx)
{
	HwDesc_s desc;
	SepAeadPrivateContext_s *pAeadPrivateCtx = (SepAeadPrivateContext_s *)pCtx->reserved;

	HW_DESC_INIT(&desc);
	HW_DESC_SET_CIPHER_MODE(&desc, ReadContextWord(&pAeadPrivateCtx->internalMode));
	HW_DESC_SET_STATE_DOUT_PARAM(&desc, (uint32_t)pCtx->mac_state, SEP_AES_BLOCK_SIZE);
	HW_DESC_SET_SETUP_MODE(&desc, SETUP_WRITE_STATE0);
	HW_DESC_SET_CIPHER_CONFIG1(&desc, TUNNEL_ON);
	HW_DESC_SET_FLOW_MODE(&desc, S_AES2_to_DOUT);
	AddHWDescSequence(qid, &desc);
}

static void LoadAeadCipherState(int qid, struct sep_ctx_aead *pCtx)
{
	HwDesc_s desc;
	SepAeadPrivateContext_s *pAeadPrivateCtx = (SepAeadPrivateContext_s *)pCtx->reserved;

	HW_DESC_INIT(&desc);
	HW_DESC_SET_CIPHER_MODE(&desc, ReadContextWord(&pAeadPrivateCtx->internalMode));
	HW_DESC_SET_KEY_SIZE_AES(&desc, ReadContextWord(&pCtx->key_size));
	HW_DESC_SET_FLOW_MODE(&desc, S_DIN_to_AES);
	HW_DESC_SET_STATE_DIN_PARAM(&desc, (uint32_t)pCtx->block_state, SEP_AES_BLOCK_SIZE);
	if (ReadContextWord(&pAeadPrivateCtx->internalMode) == SEP_AEAD_MODE_CCM_PE) {
		HW_DESC_SET_CIPHER_CONFIG0(&desc, DESC_DIRECTION_ENCRYPT_ENCRYPT);
	} else {
		HW_DESC_SET_CIPHER_CONFIG0(&desc, DESC_DIRECTION_DECRYPT_ENCRYPT);
	}
	HW_DESC_SET_SETUP_MODE(&desc, SETUP_LOAD_STATE1);
	HW_DESC_SET_CIPHER_CONFIG1(&desc, TUNNEL_ON);
	AddHWDescSequence(qid, &desc);
}

static void StoreAeadCipherState(int qid, struct sep_ctx_aead *pCtx)
{
	HwDesc_s desc;
	SepAeadPrivateContext_s *pAeadPrivateCtx = (SepAeadPrivateContext_s *)pCtx->reserved;

	HW_DESC_INIT(&desc);
	HW_DESC_SET_CIPHER_MODE(&desc, ReadContextWord(&pAeadPrivateCtx->internalMode));
	HW_DESC_SET_STATE_DOUT_PARAM(&desc, (uint32_t)pCtx->block_state, SEP_AES_BLOCK_SIZE);
	HW_DESC_SET_FLOW_MODE(&desc, S_AES_to_DOUT);
	HW_DESC_SET_SETUP_MODE(&desc, SETUP_WRITE_STATE1);
	HW_DESC_SET_CIPHER_CONFIG1(&desc, TUNNEL_ON);
	if (ReadContextWord(&pAeadPrivateCtx->internalMode) == SEP_AEAD_MODE_CCM_PE) {
		HW_DESC_SET_CIPHER_CONFIG0(&desc, DESC_DIRECTION_ENCRYPT_ENCRYPT);
	} else {
		HW_DESC_SET_CIPHER_CONFIG0(&desc, DESC_DIRECTION_DECRYPT_ENCRYPT);
	}
	AddHWDescSequence(qid, &desc);
}

static void LoadAeadKey(int qid, struct sep_ctx_aead *pCtx, FlowMode_t engineFlow, TunnelOp_t isTunnel)
{
	HwDesc_s desc;
	uint32_t keySize = ReadContextWord(&pCtx->key_size);
	SepAeadPrivateContext_s *pAeadPrivateCtx = (SepAeadPrivateContext_s *)pCtx->reserved;

	/* key size 24 bytes count as 32 bytes, make sure to zero wise upper 8 bytes */
	if (keySize == 24) {
		keySize	= SEP_AES_KEY_SIZE_MAX;
		ClearCtxField(&pCtx->key[24], SEP_AES_KEY_SIZE_MAX - 24);
	}

	HW_DESC_INIT(&desc);
	HW_DESC_SET_CIPHER_MODE(&desc, ReadContextWord(&pAeadPrivateCtx->internalMode));
	HW_DESC_SET_KEY_SIZE_AES(&desc, ReadContextWord(&pCtx->key_size));
	HW_DESC_SET_STATE_DIN_PARAM(&desc, (uint32_t)pCtx->key, keySize);
	HW_DESC_SET_FLOW_MODE(&desc, engineFlow);
	HW_DESC_SET_SETUP_MODE(&desc, SETUP_LOAD_KEY0);
	HW_DESC_SET_CIPHER_CONFIG1(&desc, isTunnel);

	if (ReadContextWord(&pAeadPrivateCtx->internalMode) == SEP_AEAD_MODE_CCM_PE) {
		HW_DESC_SET_CIPHER_CONFIG0(&desc, DESC_DIRECTION_ENCRYPT_ENCRYPT);
	} else {
		HW_DESC_SET_CIPHER_CONFIG0(&desc, DESC_DIRECTION_DECRYPT_ENCRYPT);
	}
	AddHWDescSequence(qid, &desc);
}

static uint16_t FormatCcmB0(uint8_t *Buf,
			uint8_t *Nonce,
			uint32_t NonceSize,
			uint32_t Tag,
			uint32_t AddDataSize,
			uint32_t InputDataLen)
{
	uint32_t len, Q, x, y;

	/* let's get the L value */
	len = InputDataLen;
	Q = 0;

	while (len) {
		++Q;
		len >>= 8;
	}

	if (Q <= 1) {
		Q = 2;
	}

	/* increase L to match the nonce len */
	NonceSize = (NonceSize > 13) ? 13 : NonceSize;
	if ((15 - NonceSize) > Q) {
		Q = 15 - NonceSize;
	}

	/* decrease nonce len to match L */
	if ((NonceSize + Q) > 15) {
		NonceSize = 15 - Q;
	}

	/* form B_0 == flags | Nonce N | l(m) */
	x = 0;
	Buf[x++] = (unsigned char)(((AddDataSize > 0) ? (1 << 6) : 0) | (((Tag - 2) >> 1) << 3) | (Q-1));

	/* nonce */
	for (y = 0; y < (16 - (Q + 1)); y++) {
		Buf[x++] = Nonce[y];
	}

	/* store len */
	len = InputDataLen;

	/* shift len so the upper bytes of len are the contents of the length */
	for (y = Q; y < 4; y++) {
		len <<= 8;
	}

	/* store l(m) (only store 32-bits) */
	for (y = 0; Q > 4 && (Q - y) > 4; y++) {
		Buf[x++] = 0;
	}

	for (; y < Q; y++) {
		Buf[x++] = (unsigned char)((len >> 24) & 0xFF);
		len <<= 8;
	}

	return (uint16_t)Q;
}

static void InitCcmCounter(int qid, struct sep_ctx_aead *pCtx, uint8_t CounterInitialValue)
{
	SepAeadPrivateContext_s *pAeadPrivateCtx = (SepAeadPrivateContext_s *)pCtx->reserved;
	uint32_t Q = ReadContextWord(&pAeadPrivateCtx->q);
	uint32_t nonceSize = SEP_AES_BLOCK_SIZE - (Q + 1);
	uint32_t word = 0; /*work buffer*/
	uint8_t *p = (uint8_t *)&word;
	int i = 0, j = 0;
	HwDesc_s desc;
	uint32_t nonceBuff[SEP_AES_BLOCK_SIZE_WORDS];
	uint8_t *nonce = (uint8_t*)&nonceBuff;

#ifdef DX_CC_SRAM_INDIRECT_ACCESS
	ReadContextField(pCtx->nonce, nonceBuff, SEP_AES_BLOCK_SIZE );
#else
	DX_PAL_MemCopy(nonceBuff,pCtx->nonce,SEP_AES_BLOCK_SIZE);
#endif
	
	p[0] = (unsigned char)Q - 1;
	p[1] = nonce[j++];
	p[2] = nonce[j++];
	p[3] = nonce[j++];
	
/*#warning Fix the endianity issue in the codesafe (access to bytes)*/
	word = SWAP_TO_LE(word);
	/* set 1B flags + 3B of the nonce */
	HW_DESC_INIT(&desc);
	HW_DESC_SET_DIN_CONST(&desc, word, sizeof(uint32_t));
	HW_DESC_SET_STATE_DOUT_PARAM(&desc, &(pCtx->block_state[i]), sizeof(uint32_t));
	HW_DESC_SET_FLOW_MODE(&desc, BYPASS);
	HW_DESC_LOCK_QUEUE(&desc, 1); /* Lock until RESET_CONST */
	AddHWDescSequence(qid, &desc);

#ifdef DX_CONFIG_HW_RESET_CONST_SUPPORT
	HW_DESC_INIT(&desc);
	HW_DESC_RESET_CONST_INPUT(&desc);
	AddHWDescSequence(qid, &desc);
#endif

	/* set nonce */
	for (i = 4; j < nonceSize; i += 4) {

		p[0] = nonce[j++];
		p[1] = (j < nonceSize) ? nonce[j++] : 0;
		p[2] = (j < nonceSize) ? nonce[j++] : 0;
		p[3] = (j < nonceSize) ? nonce[j++] : 0;

		/* this is the last word so set the counter value
		   as passed by the user in the LSB. The nonce value
		   cannot reache the last byte */
		if (i == (SEP_AES_BLOCK_SIZE - sizeof(uint32_t))) {
			p[3] = CounterInitialValue;
		}
	
		word = SWAP_TO_LE(word);

		HW_DESC_INIT(&desc);
		HW_DESC_SET_DIN_CONST(&desc, word, sizeof(uint32_t));
		HW_DESC_SET_STATE_DOUT_PARAM(&desc, &(pCtx->block_state[i]), sizeof(uint32_t));
		HW_DESC_SET_FLOW_MODE(&desc, BYPASS);
		HW_DESC_LOCK_QUEUE(&desc, 1); /* Lock until RESET_CONST */
		AddHWDescSequence(qid, &desc);
#ifdef DX_CONFIG_HW_RESET_CONST_SUPPORT	
		HW_DESC_INIT(&desc);
		HW_DESC_RESET_CONST_INPUT(&desc);
		AddHWDescSequence(qid, &desc);
#endif
	}

	/* pad remainder with zero's */
	for (; i < SEP_AES_BLOCK_SIZE; i += 4) {

		word = 0; /*clear word*/

		if (i == (SEP_AES_BLOCK_SIZE - sizeof(uint32_t))) {
			/* this is the last word so set the counter value
			*  as passed by the user in the LSB */
			p[3] = CounterInitialValue;
		}
		word = SWAP_TO_LE(word);

		HW_DESC_INIT(&desc);
		HW_DESC_SET_DIN_CONST(&desc, word, sizeof(uint32_t));
		HW_DESC_SET_STATE_DOUT_PARAM(&desc, &(pCtx->block_state[i]), sizeof(uint32_t));
		HW_DESC_SET_FLOW_MODE(&desc, BYPASS);
		HW_DESC_LOCK_QUEUE(&desc, 1); /* Lock until RESET_CONST */
		AddHWDescSequence(qid, &desc);
#ifdef DX_CONFIG_HW_RESET_CONST_SUPPORT
		HW_DESC_INIT(&desc);
		HW_DESC_RESET_CONST_INPUT(&desc);
		AddHWDescSequence(qid, &desc);
#endif
	}
}

static void GetFinalCcmMac(int qid, struct sep_ctx_aead *pCtx)
{
	HwDesc_s desc;
	uint32_t keySize = ReadContextWord(&pCtx->key_size);

	/* key size 24 bytes count as 32 bytes, make sure to zero wise upper 8 bytes */
	if (keySize == 24) {
		keySize	= SEP_AES_KEY_SIZE_MAX;
		ClearCtxField(&pCtx->key[24], SEP_AES_KEY_SIZE_MAX - 24);
	}

	/* initialize CTR counter */
	InitCcmCounter(qid, pCtx, 0);

	/* load AES-CTR state */
	HW_DESC_INIT(&desc);
	HW_DESC_SET_CIPHER_MODE(&desc, SEP_CIPHER_CTR);
	HW_DESC_SET_SETUP_MODE(&desc, SETUP_LOAD_STATE1);
	HW_DESC_SET_KEY_SIZE_AES(&desc, keySize);
	HW_DESC_SET_CIPHER_CONFIG0(&desc, SEP_CRYPTO_DIRECTION_ENCRYPT);
	HW_DESC_SET_STATE_DIN_PARAM(&desc, (uint32_t)pCtx->block_state, SEP_AES_BLOCK_SIZE);
	HW_DESC_SET_FLOW_MODE(&desc, S_DIN_to_AES);
	AddHWDescSequence(qid, &desc);

	/* load AES-CTR key */
	HW_DESC_INIT(&desc);
	HW_DESC_SET_CIPHER_MODE(&desc, SEP_CIPHER_CTR);
	HW_DESC_SET_CIPHER_CONFIG0(&desc, SEP_CRYPTO_DIRECTION_ENCRYPT);
	HW_DESC_SET_STATE_DIN_PARAM(&desc, (uint32_t)pCtx->key, keySize);
	HW_DESC_SET_KEY_SIZE_AES(&desc, ReadContextWord(&pCtx->key_size));
	HW_DESC_SET_FLOW_MODE(&desc, S_DIN_to_AES);
	HW_DESC_SET_SETUP_MODE(&desc, SETUP_LOAD_KEY0);
	AddHWDescSequence(qid, &desc);

	/* encrypt the "T" value and store MAC in mac_state */
	HW_DESC_INIT(&desc);
	HW_DESC_SET_STATE_DIN_PARAM(&desc, (uint32_t)pCtx->mac_state, ReadContextWord(&pCtx->tag_size));
	HW_DESC_SET_STATE_DOUT_PARAM(&desc, (uint32_t)pCtx->mac_state, ReadContextWord(&pCtx->tag_size));
	HW_DESC_SET_FLOW_MODE(&desc, DIN_AES_DOUT);
	AddHWDescSequence(qid, &desc);
}

static uint32_t GetActualHeaderSize(uint32_t headerSize)
{
	if (headerSize == 0) {
		return 0;
	} else if (headerSize < ((1UL << 16) - (1UL << 8))) {
		return (2 + headerSize);
	} else {
		return (6 + headerSize);
	}
}



/******************************************************************************
*				FUNCTIONS
******************************************************************************/

/*!
 * This function is used to initialize the AES machine to perform 
 * the AEAD operations. This should be the first function called.
 * 
 * \param pCtx A pointer to the AES context buffer in SRAM.
 * 
 * \return int One of DX_SYM_* error codes defined in dx_error.h.
 */
int InitAead(struct sep_ctx_aead *pCtx)
{
	SepAeadPrivateContext_s *pAeadPrivateCtx = (SepAeadPrivateContext_s *)pCtx->reserved;
	HwDesc_s desc;
	/* qid is stored in pxTaskTag field */
	int qid = CURR_QUEUE_ID();
	uint32_t nonceBuff[SEP_AES_BLOCK_SIZE_WORDS];
#ifdef DX_CC_SRAM_INDIRECT_ACCESS
	uint8_t *nonce = (uint8_t*)&nonceBuff;
	uint32_t stateBuff[SEP_AES_BLOCK_SIZE_WORDS];
#endif
	switch (ReadContextWord(&pCtx->mode) ) {
	case SEP_CIPHER_CCM:
		/* set AES-CCM internal mode: initial state */
		WriteContextWord(&pAeadPrivateCtx->internalMode,SEP_AEAD_MODE_CCM_A);
		if (ReadContextWord(&pCtx->header_size) == 0) {
			WriteContextWord(&pAeadPrivateCtx->nextProcessingState,SEP_AEAD_FLOW_TEXT_DATA_INIT);
		} else {
			WriteContextWord(&pAeadPrivateCtx->nextProcessingState,SEP_AEAD_FLOW_ADATA_INIT);
		}

		/* clear AES CTR/MAC states */
		ClearCtxField(&pCtx->block_state, SEP_AES_BLOCK_SIZE);
		ClearCtxField(&pCtx->mac_state, SEP_AES_BLOCK_SIZE);
		WriteContextWord(&pAeadPrivateCtx->headerRemainingBytes,GetActualHeaderSize(ReadContextWord(&pCtx->header_size)));

#ifdef DX_CC_SRAM_INDIRECT_ACCESS
	ReadContextField(pCtx->nonce, nonceBuff, SEP_AES_BLOCK_SIZE );
	WriteContextWord(&pAeadPrivateCtx->q,FormatCcmB0((uint8_t *)stateBuff,
					nonce, ReadContextWord(&pCtx->nonce_size),
					ReadContextWord(&pCtx->tag_size),
					ReadContextWord(&pCtx->header_size),
					ReadContextWord(&pCtx->text_size)));
	WriteContextField(pCtx->block_state, stateBuff, SEP_AES_BLOCK_SIZE );
#else
	pAeadPrivateCtx->q = FormatCcmB0(pCtx->block_state,
					pCtx->nonce, pCtx->nonce_size,
					pCtx->tag_size,
					pCtx->header_size,
					pCtx->text_size);
#endif
		/* format B0 header */

		/* calc MAC signature on B0 header */
		LoadAeadHeaderMac(qid, pCtx);
		LoadAeadKey(qid, pCtx, S_DIN_to_AES, TUNNEL_OFF);

		HW_DESC_INIT(&desc);
		HW_DESC_SET_STATE_DIN_PARAM(&desc, (uint32_t)pCtx->block_state, SEP_AES_BLOCK_SIZE);
		HW_DESC_SET_FLOW_MODE(&desc, DIN_AES_DOUT);
		AddHWDescSequence(qid, &desc);

		/* MAC result stored in mac_state */
		StoreAeadHeaderMac(qid, pCtx);

		break;
	default:
		DX_PAL_LOG_ERR("Alg mode not supported");
		return DX_RET_UNSUPP_ALG;
	}

	return DX_RET_OK;
}

/*!
 * This function is used to process a block(s) of data on AES machine.
 * The user must process any associated data followed by the text data
 * blocks. This function MUST be called after the InitCipher function.
 * 
 * \param pCtx A pointer to the AES context buffer in SRAM.
 * \param pDmaInputBuffer A structure which represents the DMA input buffer.
 * \param pDmaOutputBuffer A structure which represents the DMA output buffer.
 * 
 * \return int One of DX_SYM_* error codes defined in dx_error.h.
 */
int ProcessAead(struct sep_ctx_aead *pCtx, DmaBuffer_s *pDmaInputBuffer, DmaBuffer_s *pDmaOutputBuffer)
{
	uint8_t *pInputData = NULL, *pOutputData = NULL;
	uint32_t DataInSize = 0, DataOutSize = 0;
	HwDesc_s desc;
	DmaMode_t dmaMode = NO_DMA;
	int nMlliTables = pDmaInputBuffer->nTables;
	uint8_t inAxiNs = pDmaInputBuffer->axiNs;
	uint8_t outAxiNs = pDmaOutputBuffer->axiNs;
	SepAeadPrivateContext_s *pAeadPrivateCtx = (SepAeadPrivateContext_s *)pCtx->reserved;
	const int isInplaceOp = ((pDmaInputBuffer->pData == pDmaOutputBuffer->pData) ||
				 (ReadContextWord(&pAeadPrivateCtx->nextProcessingState) == SEP_AEAD_FLOW_ADATA_INIT) ||
				 (ReadContextWord(&pAeadPrivateCtx->nextProcessingState) == SEP_AEAD_FLOW_ADATA_PROCESS) ||
				 (pDmaOutputBuffer->pData == 0));
	int qid = CURR_QUEUE_ID(); /* qid is stored in pxTaskTag field */
	int drvRc = DX_RET_OK;


	if (ReadContextWord(&pCtx->mode) != SEP_CIPHER_CCM) {
		DX_PAL_LOG_ERR("Alg mode not supported");
		drvRc = DX_RET_UNSUPP_ALG;
		goto EndWithErr;
	}

	dmaMode = DMA_BUF_TYPE_TO_MODE(pDmaInputBuffer->dmaBufType);

	switch (ReadContextWord(&pAeadPrivateCtx->nextProcessingState)) {
	case SEP_AEAD_FLOW_ADATA_INIT:
		/* set the next flow sate */
		if (dmaMode == DMA_MLLI) {
			/* if MLLI -we expect to have the all header at once,
			*  could be one table or more but in a single descriptor processing */
			WriteContextWord(&pAeadPrivateCtx->nextProcessingState,SEP_AEAD_FLOW_TEXT_DATA_INIT);
			WriteContextWord(&pAeadPrivateCtx->headerRemainingBytes,ReadContextWord(&pAeadPrivateCtx->headerRemainingBytes)-ReadContextWord(&pCtx->header_size) );
		} else {
			/* if SRAM or DLLI -user may process his associated data in a partial AES blocks */
			WriteContextWord(&pAeadPrivateCtx->nextProcessingState,SEP_AEAD_FLOW_ADATA_PROCESS);
		}

		/* initialize AES-CTR counter only once */
		InitCcmCounter(qid, pCtx, 1);

		/* load mac state and key */
		LoadAeadHeaderMac(qid, pCtx);
		LoadAeadKey(qid, pCtx, S_DIN_to_AES, TUNNEL_OFF);
		break;
	case SEP_AEAD_FLOW_ADATA_PROCESS:
		/* set the next flow sate */
		if (dmaMode == DMA_MLLI) {
#ifndef DX_CC_TEE
			DX_PAL_LOG_ERR("ILLEGAL flow: associated data should passed at once (DMA_MLLI)");
			drvRc = DX_RET_PERM;
			goto EndWithErr;
#else
			WriteContextWord(&pAeadPrivateCtx->nextProcessingState,SEP_AEAD_FLOW_TEXT_DATA_INIT);
			WriteContextWord(&pAeadPrivateCtx->headerRemainingBytes,
					ReadContextWord(&pAeadPrivateCtx->headerRemainingBytes)-ReadContextWord(&pCtx->header_size) );
#endif
		}

		LoadAeadHeaderMac(qid, pCtx);
		LoadAeadKey(qid, pCtx, S_DIN_to_AES, TUNNEL_OFF);
		break;
	case SEP_AEAD_FLOW_TEXT_DATA_INIT:
		/* set internal mode: CCM encrypt/decrypt */
		WriteContextWord(&pAeadPrivateCtx->internalMode,SEP_AEAD_CCM_SET_INTERNAL_MODE(ReadContextWord(&pCtx->direction)));
		WriteContextWord(&pAeadPrivateCtx->nextProcessingState,SEP_AEAD_FLOW_TEXT_DATA_PROCESS);
		/* initialize AES-CTR counter only once */
		InitCcmCounter(qid, pCtx, 1);
		/*FALLTHROUGH*/
	case SEP_AEAD_FLOW_TEXT_DATA_PROCESS:
	default:
		LoadAeadKey(qid, pCtx, S_DIN_to_AES, TUNNEL_ON);
		LoadAeadCipherState(qid, pCtx);
		LoadAeadCipherMac(qid, pCtx);
		LoadAeadKey(qid, pCtx, S_DIN_to_AES2, TUNNEL_ON);
		break;
	}

	/* set the input/output pointers according to the DMA mode */
	if ((!isInplaceOp) && (pDmaInputBuffer->dmaBufType != pDmaOutputBuffer->dmaBufType)) {
		DX_PAL_LOG_ERR("Inconsistent DMA mode for in/out buffers\n");
		drvRc = DX_RET_INVARG;
		goto EndWithErr;
	}
	
	switch (dmaMode) {
	case DMA_MLLI:	
		pInputData = (uint8_t *)GetFirstLliPtr(qid, MLLI_INPUT_TABLE);
		PrepareFirstMLLITable(qid, pDmaInputBuffer, MLLI_INPUT_TABLE);

		/* get OUT MLLI tables pointer in SRAM (if not inplace operation) */
		if (isInplaceOp == 0) {
			pOutputData = (uint8_t *)GetFirstLliPtr(qid, MLLI_OUTPUT_TABLE);
			PrepareFirstMLLITable(qid, pDmaOutputBuffer, MLLI_OUTPUT_TABLE);
		} else {
			pOutputData = pInputData;
		}

		/* data size is meaningless in DMA-MLLI mode */
		DataInSize = 0;
		DataOutSize = 0;

		break;
	case DMA_DLLI:
	case DMA_SRAM:
		pInputData = (uint8_t *)pDmaInputBuffer->pData;
		if (isInplaceOp == 0) {
			pOutputData  = (uint8_t *)pDmaOutputBuffer->pData;
		} else {
			pOutputData = pInputData;
		}

		/* data processing is done */
		nMlliTables = 0;

		/* set the data size */
		DataInSize = pDmaInputBuffer->size;
		DataOutSize = pDmaOutputBuffer->size; 

		break;
	default:
		DX_PAL_LOG_ERR("Invalid DMA mode\n");
		drvRc = DX_RET_INVARG;
		goto EndWithErr;
	}

	/* process the AEAD flow */
	HW_DESC_INIT(&desc);
	HW_DESC_SET_DIN_TYPE(&desc, dmaMode, (uint32_t)pInputData, DataInSize, QID_TO_AXI_ID(qid), inAxiNs);
	if (ReadContextWord(&pAeadPrivateCtx->internalMode) != SEP_AEAD_MODE_CCM_A) {
		HW_DESC_SET_DOUT_TYPE(&desc, dmaMode, (uint32_t)pOutputData, DataOutSize, QID_TO_AXI_ID(qid), outAxiNs);
	}
	HW_DESC_SET_FLOW_MODE(&desc, DIN_AES_DOUT);
	AddHWDescSequence(qid, &desc);

	/* process each MLLI table (MLLI processing loop only) */
	while (--nMlliTables > 0) {
		/* prepare next input MLLI table in SRAM */
		PrepareNextMLLITable(qid, inAxiNs, MLLI_INPUT_TABLE);

		/* prepare next output MLLI table in SRAM */
		if (isInplaceOp == 0) {
			PrepareNextMLLITable(qid, outAxiNs, MLLI_OUTPUT_TABLE);
		}

		/* process the AES flow */
		HW_DESC_INIT(&desc);
		HW_DESC_SET_DIN_TYPE(&desc, DMA_MLLI, (uint32_t)pInputData, 0, QID_TO_AXI_ID(qid), inAxiNs);
		if (ReadContextWord(&pAeadPrivateCtx->internalMode) != SEP_AEAD_MODE_CCM_A) {
			HW_DESC_SET_DOUT_TYPE(&desc, dmaMode, (uint32_t)pOutputData, DataOutSize, QID_TO_AXI_ID(qid), outAxiNs);
		}
		HW_DESC_SET_FLOW_MODE(&desc, DIN_AES_DOUT);
		AddHWDescSequence(qid, &desc);
	}

	/* store machine state */
	if (ReadContextWord(&pAeadPrivateCtx->internalMode) == SEP_AEAD_MODE_CCM_A) {
		StoreAeadHeaderMac(qid, pCtx);

		if ((dmaMode == DMA_DLLI) || (dmaMode == DMA_SRAM)) {
			WriteContextWord(&pAeadPrivateCtx->headerRemainingBytes,ReadContextWord(&pAeadPrivateCtx->headerRemainingBytes)-pDmaInputBuffer->size);
			if (ReadContextWord(&pAeadPrivateCtx->headerRemainingBytes) > ReadContextWord(&pCtx->header_size)) {
				DX_PAL_LOG_ERR("Inconceivable state: Assoc remaining bytes > Header size");
				drvRc = DX_RET_NOEXEC;
				goto EndWithErr;
			}
			if (ReadContextWord(&pAeadPrivateCtx->headerRemainingBytes) == 0) {
				/* we're done processing associated data move on to text initialization flow */
				WriteContextWord(&pAeadPrivateCtx->nextProcessingState,SEP_AEAD_FLOW_TEXT_DATA_INIT);
			}
		}
	} else {
		StoreAeadCipherState(qid, pCtx);
		StoreAeadCipherMac(qid, pCtx);
	}

EndWithErr:
	return drvRc;
}

/*!
 * This function is used as finish operation of AEAD. The function MUST either 
 * be called after "InitCipher" or "ProcessCipher".
 * 
 * \param pCtx A pointer to the AES context buffer in SRAM.
 * \param pDmaInputBuffer A structure which represents the DMA input buffer.
 * \param pDmaOutputBuffer A structure which represents the DMA output buffer.
 * 
 * \return int One of DX_SYM_* error codes defined in dx_error.h.
 */
int FinalizeAead(struct sep_ctx_aead *pCtx, DmaBuffer_s *pDmaInputBuffer, DmaBuffer_s *pDmaOutputBuffer)
{
	uint32_t isRemainingData = 0;
	DmaMode_t dmaMode = NO_DMA;
	/* qid is stored in pxTaskTag field */
	int qid = CURR_QUEUE_ID();
	int drvRc = DX_RET_OK;

	if (ReadContextWord(&pCtx->mode) != SEP_CIPHER_CCM) {
		DX_PAL_LOG_ERR("Alg mode not supported");
		drvRc = DX_RET_UNSUPP_ALG;
		goto EndWithErr;
	}

	dmaMode = DMA_BUF_TYPE_TO_MODE(pDmaInputBuffer->dmaBufType);

	/* check if we have remaining data to process */
	switch (dmaMode) {
	case DMA_MLLI:
		isRemainingData = pDmaInputBuffer->nTables;
		break;
	case DMA_DLLI:
	case DMA_SRAM:
		isRemainingData = (pDmaInputBuffer->size > 0) ? 1 : 0;
		break;
	case DMA_MODE_NULL:
		break;
	default:
		DX_PAL_LOG_ERR("Invalid DMA mode\n");
		drvRc = DX_RET_INVARG;
		goto EndWithErr;
	}	

	/* clobber remaining AEAD data */
	if (isRemainingData) {
		/* process all tables and get state from the AES machine */
		drvRc = ProcessAead(pCtx, pDmaInputBuffer, pDmaOutputBuffer);
		if (drvRc != DX_RET_OK) {
			goto EndWithErr;
		}
	}

	/* get the CCM-MAC result */
	GetFinalCcmMac(qid, pCtx);

EndWithErr:
	return drvRc;
}

