/*******************************************************************
* (c) Copyright 2011-2012 Discretix Technologies Ltd.              *
* This software is protected by copyright, international           *
* treaties and patents, and distributed under multiple licenses.   *
* Any use of this Software as part of the Discretix CryptoCell or  *
* Packet Engine products requires a commercial license.            *
* Copies of this Software that are distributed with the Discretix  *
* CryptoCell or Packet Engine product drivers, may be used in      *
* accordance with a commercial license, or at the user's option,   *
* used and redistributed under the terms and conditions of the GNU *
* General Public License ("GPL") version 2, as published by the    *
* Free Software Foundation.                                        *
* This program is distributed in the hope that it will be useful,  *
* but WITHOUT ANY LIABILITY AND WARRANTY; without even the implied *
* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. *
* See the GNU General Public License version 2 for more details.   *
* You should have received a copy of the GNU General Public        *
* License version 2 along with this Software; if not, please write *
* to the Free Software Foundation, Inc., 59 Temple Place - Suite   *
* 330, Boston, MA 02111-1307, USA.                                 *
* Any copy or reproduction of this Software, as permitted under    *
* the GNU General Public License version 2, must include this      *
* Copyright Notice as well as any other notices provided under     *
* the said license.                                                *
********************************************************************/

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <crypto/algapi.h>
#include <crypto/internal/skcipher.h>
#include <crypto/internal/hash.h>
#include <crypto/aead.h>
#include <crypto/sha.h>
#include <crypto/ctr.h>
#include <crypto/authenc.h>
#include <crypto/aes.h>
#include <crypto/des.h>
#include <linux/rtnetlink.h>

#include "dx_config.h"
#include "dx_driver.h"
#include "dx_buffer_mgr.h"
#include "dx_aead.h"
#include "dx_request_mgr.h"
#include "dx_hash.h"
#include "dx_sysfs.h"
#include "dx_sram_mgr.h"
#include "dx_hash.h"

#define template_aead	template_u.aead

#define MAX_AEAD_SETKEY_SEQ 12
#define MAX_AEAD_PROCESS_SEQ 23

#define MAX_HMAC_DIGEST_SIZE (SHA256_DIGEST_SIZE)
#define MAX_HMAC_BLOCK_SIZE (SHA256_BLOCK_SIZE)

#define AES_CCM_RFC4309_NONCE_SIZE 3
#define MAX_NONCE_SIZE CTR_RFC3686_NONCE_SIZE


/* Value of each ICV_CMP byte (of 8) in case of success */
#define ICV_VERIF_OK 0x01

#define	AEAD_MODE_CCM_A  8
#define	AEAD_MODE_CCM_PE 9
#define	AEAD_MODE_CCM_PD 10

struct dx_aead_handle {
	uint32_t sram_workspace_addr;
	struct list_head aead_list;
};

struct dx_aead_ctx {
	struct dx_drvdata *drvdata;
	uint8_t ctr_nonce[MAX_NONCE_SIZE]; /* used for ctr3686 iv and aes ccm */
	uint8_t *enckey;
	dma_addr_t enckey_dma_addr;
	union {
		struct {
			uint8_t *padded_authkey;
			uint8_t *ipad_opad; /* IPAD, OPAD*/
			dma_addr_t padded_authkey_dma_addr;
			dma_addr_t ipad_opad_dma_addr;
		} hmac;
		struct {
			uint8_t *xcbc_keys; /* K1,K2,K3 */
			dma_addr_t xcbc_keys_dma_addr;
		} xcbc;
	} auth_state;
	unsigned int enc_keylen;
	unsigned int auth_keylen;
	unsigned int authsize; /* Actual (reduced?) size of the MAC/ICv */
	enum sep_cipher_mode cipher_mode;
	enum FlowMode flow_mode;
	enum sep_hash_mode auth_mode;
};

/* Private pointer to the driver instance */
static struct dx_drvdata *crypto_drvdata;

static void dx_aead_exit(struct crypto_tfm *tfm)
{
	struct dx_aead_ctx *ctx = crypto_tfm_ctx(tfm);
	struct device *dev = &crypto_drvdata->plat_dev->dev;

	DX_LOG_DEBUG("Clearing context @%p for %s\n",
		crypto_tfm_ctx(tfm), crypto_tfm_alg_name(tfm));

	/* Unmap enckey buffer */
	if (ctx->enckey != NULL) {
		dma_free_coherent(dev, AES_MAX_KEY_SIZE, ctx->enckey, ctx->enckey_dma_addr);
		DX_LOG_DEBUG("Freed enckey DMA buffer enckey_dma_addr=0x%08lX\n",
			(unsigned long)ctx->enckey_dma_addr);
		ctx->enckey_dma_addr = 0;
		ctx->enckey = NULL;
	}

	if (ctx->auth_mode == SEP_HASH_XCBC_MAC) { /* XCBC authetication */
		if (ctx->auth_state.xcbc.xcbc_keys != NULL) {
			dma_free_coherent(dev, SEP_AES_128_BIT_KEY_SIZE * 3,
				ctx->auth_state.xcbc.xcbc_keys,
				ctx->auth_state.xcbc.xcbc_keys_dma_addr);
		}
		DX_LOG_DEBUG("Freed xcbc_keys DMA buffer xcbc_keys_dma_addr=0x%08lX\n",
			(unsigned long)ctx->auth_state.xcbc.xcbc_keys_dma_addr);
		ctx->auth_state.xcbc.xcbc_keys_dma_addr = 0;
		ctx->auth_state.xcbc.xcbc_keys = NULL;
	} else if (ctx->auth_mode != SEP_HASH_NULL) { /* HMAC auth. */
		if (ctx->auth_state.hmac.ipad_opad != NULL) {
			dma_free_coherent(dev, 2 * MAX_HMAC_DIGEST_SIZE,
				ctx->auth_state.hmac.ipad_opad,
				ctx->auth_state.hmac.ipad_opad_dma_addr);
			DX_LOG_DEBUG("Freed ipad_opad DMA buffer ipad_opad_dma_addr=0x%08lX\n",
				(unsigned long)ctx->auth_state.hmac.ipad_opad_dma_addr);
			ctx->auth_state.hmac.ipad_opad_dma_addr = 0;
			ctx->auth_state.hmac.ipad_opad = NULL;
		}
		if (ctx->auth_state.hmac.padded_authkey != NULL) {
			dma_free_coherent(dev, MAX_HMAC_BLOCK_SIZE,
				ctx->auth_state.hmac.padded_authkey,
				ctx->auth_state.hmac.padded_authkey_dma_addr);
			DX_LOG_DEBUG("Freed padded_authkey DMA buffer padded_authkey_dma_addr=0x%08lX\n",
				(unsigned long)ctx->auth_state.hmac.padded_authkey_dma_addr);
			ctx->auth_state.hmac.padded_authkey_dma_addr = 0;
			ctx->auth_state.hmac.padded_authkey = NULL;
		}
	}
}

static int dx_aead_init(struct crypto_tfm *tfm)
{
	struct dx_aead_ctx *ctx = crypto_tfm_ctx(tfm);
	struct device *dev = &crypto_drvdata->plat_dev->dev;
	struct aead_tfm *aead = &tfm->crt_aead;
	struct crypto_alg *alg = tfm->__crt_alg;
	struct dx_crypto_alg *dx_alg =
			container_of(alg, struct dx_crypto_alg, crypto_alg);

	DX_LOG_DEBUG("Initializing context @%p for %s\n", ctx, crypto_tfm_alg_name(tfm));

	/* Initialize modes in instance */
	ctx->cipher_mode = dx_alg->cipher_mode;
	ctx->flow_mode = dx_alg->flow_mode;
	ctx->auth_mode = dx_alg->auth_mode;

	aead->reqsize = sizeof(struct aead_req_ctx);

	/* Allocate key buffer, cache line aligned */
	ctx->enckey = dma_alloc_coherent(dev, AES_MAX_KEY_SIZE, &ctx->enckey_dma_addr, GFP_KERNEL);
	if (ctx->enckey == NULL) {
		DX_LOG_ERR("Failed allocating key buffer\n");
		goto init_failed;
	}
	DX_LOG_DEBUG("Allocated enckey buffer in context ctx->enckey=@%p\n", ctx->enckey);

	/* Set default authlen value */
	ctx->authsize = tfm->crt_u.aead.authsize;
	if (ctx->auth_mode == SEP_HASH_XCBC_MAC) { /* XCBC authetication */
		/* Allocate dma-coherent buffer for XCBC's K1+K2+K3 */
		/* (and temporary for user key - up to 256b) */
		ctx->auth_state.xcbc.xcbc_keys = dma_alloc_coherent(dev,
			SEP_AES_128_BIT_KEY_SIZE * 3,
			&ctx->auth_state.xcbc.xcbc_keys_dma_addr, GFP_KERNEL);
		if (ctx->auth_state.xcbc.xcbc_keys == NULL) {
			DX_LOG_ERR("Failed allocating buffer for XCBC keys\n");
			goto init_failed;
		}
	} else if (ctx->auth_mode != SEP_HASH_NULL) { /* HMAC authentication */
		/* Allocate dma-coherent buffer for IPAD + OPAD */
		ctx->auth_state.hmac.ipad_opad = dma_alloc_coherent(dev,
			2 * MAX_HMAC_DIGEST_SIZE,
			&ctx->auth_state.hmac.ipad_opad_dma_addr, GFP_KERNEL);
		if (ctx->auth_state.hmac.ipad_opad == NULL) {
			DX_LOG_ERR("Failed allocating IPAD/OPAD buffer\n");
			goto init_failed;
		}
		DX_LOG_DEBUG("Allocated authkey buffer in context ctx->authkey=@%p\n",
			ctx->auth_state.hmac.ipad_opad);

		ctx->auth_state.hmac.padded_authkey = dma_alloc_coherent(dev,
			MAX_HMAC_BLOCK_SIZE,
			&ctx->auth_state.hmac.padded_authkey_dma_addr, GFP_KERNEL);
		if (ctx->auth_state.hmac.padded_authkey == NULL) {
			DX_LOG_ERR("failed to allocate padded_authkey\n");
			goto init_failed;
		}
	} else {
		ctx->auth_state.hmac.ipad_opad = NULL;
		ctx->auth_state.hmac.padded_authkey = NULL;
	}

	return 0;

init_failed:
	dx_aead_exit(tfm);
	return -ENOMEM;
}

static void dx_aead_complete(struct device *dev, void *dx_req)
{
	struct aead_request *areq = (struct aead_request *)dx_req;
	struct aead_req_ctx *areq_ctx = aead_request_ctx(areq);
	struct crypto_aead *tfm = crypto_aead_reqtfm(dx_req);
	struct dx_aead_ctx *ctx = crypto_aead_ctx(tfm);
	int err = 0;
	DECL_CYCLE_COUNT_RESOURCES;

	START_CYCLE_COUNT();

	unmap_aead_request(dev, areq);

	/* Restore ordinary iv pointer */
	areq->iv = areq_ctx->backup_iv;

	if (areq_ctx->gen_ctx.op_type == SEP_CRYPTO_DIRECTION_DECRYPT) {

		if ((ctx->cipher_mode == SEP_CIPHER_CCM) ||
		    unlikely(!IS_ALIGNED(ctx->authsize, sizeof(uint32_t)))) {
			enum dx_sg_cpy_direct sg_cpy_dir =
				(ctx->cipher_mode == SEP_CIPHER_CCM) ?
					DX_SG_FROM_BUF : DX_SG_TO_BUF;

			/* For CCM: re-copy saved mac to it's original location
			   to deal with possible data memory overriding that
			   caused by cache coherence problem.
			   Other ciphers: copy origin ICV to local buffer */
			dx_sg_copy_part(areq_ctx->backup_mac, areq->src,
					      areq->cryptlen - ctx->authsize,
					      areq->cryptlen, sg_cpy_dir);

			if (memcmp(areq_ctx->mac_buf, areq_ctx->backup_mac,
				ctx->authsize)) {
				DX_LOG_DEBUG("SW compare: "
					"Payload authentication failure, "
					"(auth-size=%d, cipher=%d).\n",
					ctx->authsize, ctx->cipher_mode);
				err = -EBADMSG;
			}
		} else {
			/* Verify ICV by hardware only if ICV is mult of 4 */
			if (areq_ctx->mac_buf[0] != ICV_VERIF_OK) {
				DX_LOG_DEBUG("HW compare: "
					"Payload authentication failure, "
					"(auth-size=%d, cipher=%d).\n",
					ctx->authsize, ctx->cipher_mode);
				err = -EBADMSG;
			}
		}
	} else if (ctx->cipher_mode == SEP_CIPHER_CCM) {
		/* Copy MAC to dst buffer only if decrypt && CCM */
		dx_sg_copy_part(areq_ctx->mac_buf, areq->dst,
			     areq->cryptlen,
			     areq->cryptlen + ctx->authsize,
			     DX_SG_FROM_BUF);
	}

	if (areq_ctx->backup_giv &&
	    (ctx->cipher_mode == SEP_CIPHER_CTR)) {
		memcpy(areq_ctx->backup_giv, areq_ctx->ctr_iv +
		CTR_RFC3686_NONCE_SIZE, CTR_RFC3686_IV_SIZE);
	}

	END_CYCLE_COUNT(STAT_OP_TYPE_GENERIC, STAT_PHASE_4);
	aead_request_complete(areq, err);
}

static int xcbc_setkey(HwDesc_s *desc, struct dx_aead_ctx *ctx)
{
	/* Load the AES key */
	HW_DESC_INIT(&desc[0]);
	/* We are using for the source/user key the same buffer as for the output keys,
	   because after this key loading it is not needed anymore */
	HW_DESC_SET_DIN_TYPE(&desc[0], DMA_DLLI, ctx->auth_state.xcbc.xcbc_keys_dma_addr, ctx->auth_keylen, AXI_ID, NS_BIT);
	HW_DESC_SET_CIPHER_MODE(&desc[0], SEP_CIPHER_ECB);
	HW_DESC_SET_CIPHER_CONFIG0(&desc[0], SEP_CRYPTO_DIRECTION_ENCRYPT);
	HW_DESC_SET_KEY_SIZE_AES(&desc[0], ctx->auth_keylen);
	HW_DESC_SET_FLOW_MODE(&desc[0], S_DIN_to_AES);
	HW_DESC_SET_SETUP_MODE(&desc[0], SETUP_LOAD_KEY0);

	HW_DESC_INIT(&desc[1]);
	HW_DESC_SET_DIN_CONST(&desc[1], 0x01010101, SEP_AES_128_BIT_KEY_SIZE);
	HW_DESC_SET_FLOW_MODE(&desc[1], DIN_AES_DOUT);
	HW_DESC_SET_DOUT_DLLI(&desc[1], ctx->auth_state.xcbc.xcbc_keys_dma_addr, AES_KEYSIZE_128, AXI_ID, NS_BIT, 0);

	HW_DESC_INIT(&desc[2]);
	HW_DESC_SET_DIN_CONST(&desc[2], 0x02020202, SEP_AES_128_BIT_KEY_SIZE);
	HW_DESC_SET_FLOW_MODE(&desc[2], DIN_AES_DOUT);
	HW_DESC_SET_DOUT_DLLI(&desc[2], ctx->auth_state.xcbc.xcbc_keys_dma_addr + AES_KEYSIZE_128, AES_KEYSIZE_128, AXI_ID, NS_BIT, 0);

	HW_DESC_INIT(&desc[3]);
	HW_DESC_SET_DIN_CONST(&desc[3], 0x03030303, SEP_AES_128_BIT_KEY_SIZE);
	HW_DESC_SET_FLOW_MODE(&desc[3], DIN_AES_DOUT);
	HW_DESC_SET_DOUT_DLLI(&desc[3], ctx->auth_state.xcbc.xcbc_keys_dma_addr + 2 * AES_KEYSIZE_128, AES_KEYSIZE_128, AXI_ID, NS_BIT, 0);

	return 4;
}

static int hmac_setkey(HwDesc_s *desc, struct dx_aead_ctx *ctx)
{
	unsigned int hmacPadConst[2] = { HMAC_IPAD_CONST, HMAC_OPAD_CONST };
	unsigned int digest_ofs = 0;
	unsigned int hash_mode = (ctx->auth_mode == SEP_HASH_SHA1) ?
			SEP_HASH_HW_SHA1 : SEP_HASH_HW_SHA256;
	unsigned int digest_size = (ctx->auth_mode == SEP_HASH_SHA1) ?
			SEP_SHA1_DIGEST_SIZE : SEP_SHA256_DIGEST_SIZE;

	int idx = 0;
	int i;

	/* calc derived HMAC key */
	for (i = 0; i < 2; i++) {
		/* Load hash initial state */
		HW_DESC_INIT(&desc[idx]);
		HW_DESC_SET_CIPHER_MODE(&desc[idx], hash_mode);
		HW_DESC_SET_DIN_SRAM(&desc[idx],
			dx_ahash_get_larval_digest_sram_addr(NULL, ctx->auth_mode),
			digest_size);
		HW_DESC_SET_FLOW_MODE(&desc[idx], S_DIN_to_HASH);
		HW_DESC_SET_SETUP_MODE(&desc[idx], SETUP_LOAD_STATE0);
		idx++;

		/* Load the hash current length*/
		HW_DESC_INIT(&desc[idx]);
		HW_DESC_SET_CIPHER_MODE(&desc[idx], hash_mode);
		HW_DESC_SET_DIN_CONST(&desc[idx], 0, HASH_LEN_SIZE);
		HW_DESC_SET_FLOW_MODE(&desc[idx], S_DIN_to_HASH);
		HW_DESC_SET_SETUP_MODE(&desc[idx], SETUP_LOAD_KEY0);
		idx++;

		/* Prepare ipad key */
		HW_DESC_INIT(&desc[idx]);
		HW_DESC_SET_XOR_VAL(&desc[idx], hmacPadConst[i]);
		HW_DESC_SET_CIPHER_MODE(&desc[idx], hash_mode);
		HW_DESC_SET_FLOW_MODE(&desc[idx], S_DIN_to_HASH);
		HW_DESC_SET_SETUP_MODE(&desc[idx], SETUP_LOAD_STATE1);
		idx++;

		/* Perform HASH update */
		HW_DESC_INIT(&desc[idx]);
		HW_DESC_SET_DIN_TYPE(&desc[idx], DMA_DLLI, ctx->auth_state.hmac.padded_authkey_dma_addr, SHA256_BLOCK_SIZE, AXI_ID, NS_BIT);
		HW_DESC_SET_CIPHER_MODE(&desc[idx], hash_mode);
		HW_DESC_SET_XOR_ACTIVE(&desc[idx]);
		HW_DESC_SET_FLOW_MODE(&desc[idx], DIN_HASH);
		idx++;

		/* Get the digset */
		HW_DESC_INIT(&desc[idx]);
		HW_DESC_SET_CIPHER_MODE(&desc[idx], hash_mode);
		HW_DESC_SET_DOUT_DLLI(&desc[idx], ctx->auth_state.hmac.ipad_opad_dma_addr + digest_ofs, digest_size, AXI_ID, NS_BIT, 0);
		HW_DESC_SET_FLOW_MODE(&desc[idx], S_HASH_to_DOUT);
		HW_DESC_SET_SETUP_MODE(&desc[idx], SETUP_WRITE_STATE0);
		idx++;

		digest_ofs += digest_size;
	}

	return idx;
}

static int validate_keys_sizes(struct dx_aead_ctx *ctx)
{
	DX_LOG_DEBUG("enc_keylen=%u  authkeylen=%u\n",
		ctx->enc_keylen, ctx->auth_keylen);

	switch (ctx->auth_mode) {
	case SEP_HASH_SHA1:
	case SEP_HASH_SHA256:
		break;
	case SEP_HASH_XCBC_MAC:
		if ((ctx->auth_keylen != AES_KEYSIZE_128) &&
		    (ctx->auth_keylen != AES_KEYSIZE_192) &&
		    (ctx->auth_keylen != AES_KEYSIZE_256))
			return -ENOTSUPP;
		break;
	case SEP_HASH_NULL: /* Not authenc (e.g., CCM) - no auth_key) */
		if (ctx->auth_keylen > 0)
			return -EINVAL;
		break;
	default:
		DX_LOG_ERR("Invalid auth_mode=%d\n", ctx->auth_mode);
		return -EINVAL;
	}
	/* Check cipher key size */
	if (unlikely(ctx->flow_mode == S_DIN_to_DES)) {
		if (ctx->enc_keylen != DES3_EDE_KEY_SIZE) {
			DX_LOG_ERR("Invalid cipher(3DES) key size: %u\n",
				ctx->enc_keylen);
			return -EINVAL;
		}
	} else { /* Default assumed to be AES ciphers */
		if ((ctx->enc_keylen != AES_KEYSIZE_128) &&
		    (ctx->enc_keylen != AES_KEYSIZE_192) &&
		    (ctx->enc_keylen != AES_KEYSIZE_256)) {
			DX_LOG_ERR("Invalid cipher(AES) key size: %u\n",
				ctx->enc_keylen);
			return -EINVAL;
		}
	}

	return 0; /* All tests of keys sizes passed */
}
/*This function prepers the user key so it can pass to the hmac processing
  (copy to intenral buffer or hash in case of key longer than block */
static int
dx_get_plain_hmac_key(struct crypto_aead *tfm, const u8 *key, unsigned int keylen)
{
	dma_addr_t key_dma_addr;
	struct dx_aead_ctx *ctx = crypto_aead_ctx(tfm);
	uint32_t larval_addr = dx_ahash_get_larval_digest_sram_addr(NULL, ctx->auth_mode);
	struct dx_crypto_req dx_req = {0};
	unsigned int blocksize;
	unsigned int digestsize;
	unsigned int hashmode;
	unsigned int idx = 0;
	int rc = 0;
	HwDesc_s desc[MAX_AEAD_SETKEY_SEQ];
	dma_addr_t padded_authkey_dma_addr =
		ctx->auth_state.hmac.padded_authkey_dma_addr;

	key_dma_addr = dma_map_single(&crypto_drvdata->plat_dev->dev,
				      (void *)key, keylen, DMA_TO_DEVICE);
	if (unlikely(dma_mapping_error(&crypto_drvdata->plat_dev->dev,
				       key_dma_addr))) {
		DX_LOG_ERR("Mapping key va=0x%08X len 0x%X for"
			   " DMA failed\n", (uint32_t)key,
			   (uint32_t)keylen);
		return -ENOMEM;
	}
	switch (ctx->auth_mode) { /* auth_key required and >0 */
	case SEP_HASH_SHA1:
		blocksize = SHA1_BLOCK_SIZE;
		digestsize = SHA1_DIGEST_SIZE;
		hashmode = SEP_HASH_HW_SHA1;
		break;
	case SEP_HASH_SHA256:
	default:
		blocksize = SHA256_BLOCK_SIZE;
		digestsize = SHA256_DIGEST_SIZE;
		hashmode = SEP_HASH_HW_SHA256;
	}
	if (keylen > blocksize) {
		/* Load hash initial state */
		HW_DESC_INIT(&desc[idx]);
		HW_DESC_SET_CIPHER_MODE(&desc[idx], hashmode);
		HW_DESC_SET_DIN_SRAM(&desc[idx], larval_addr, digestsize);
		HW_DESC_SET_FLOW_MODE(&desc[idx], S_DIN_to_HASH);
		HW_DESC_SET_SETUP_MODE(&desc[idx], SETUP_LOAD_STATE0);
		idx++;

		/* Load the hash current length*/
		HW_DESC_INIT(&desc[idx]);
		HW_DESC_SET_CIPHER_MODE(&desc[idx], hashmode);
		HW_DESC_SET_DIN_CONST(&desc[idx], 0, HASH_LEN_SIZE);
		HW_DESC_SET_CIPHER_CONFIG1(&desc[idx], HASH_PADDING_ENABLED);
		HW_DESC_SET_FLOW_MODE(&desc[idx], S_DIN_to_HASH);
		HW_DESC_SET_SETUP_MODE(&desc[idx], SETUP_LOAD_KEY0);
		idx++;

		HW_DESC_INIT(&desc[idx]);
		HW_DESC_SET_DIN_TYPE(&desc[idx], DMA_DLLI,
				     key_dma_addr,
				     keylen, AXI_ID, NS_BIT);
		HW_DESC_SET_FLOW_MODE(&desc[idx], DIN_HASH);
		idx++;


		/* Get hashed key */
		HW_DESC_INIT(&desc[idx]);
		HW_DESC_SET_CIPHER_MODE(&desc[idx], hashmode);
		HW_DESC_SET_DOUT_DLLI(&desc[idx],
				 padded_authkey_dma_addr,
				 digestsize,
				 AXI_ID, NS_BIT, 0);
		HW_DESC_SET_FLOW_MODE(&desc[idx], S_HASH_to_DOUT);
		HW_DESC_SET_SETUP_MODE(&desc[idx], SETUP_WRITE_STATE0);
		HW_DESC_SET_CIPHER_CONFIG1(&desc[idx],
						HASH_PADDING_DISABLED);
		HW_DESC_SET_CIPHER_CONFIG0(&desc[idx],
					   HASH_DIGEST_RESULT_LITTLE_ENDIAN);
		idx++;

		HW_DESC_INIT(&desc[idx]);
		HW_DESC_SET_DIN_CONST(&desc[idx], 0, (blocksize - digestsize));
		HW_DESC_SET_FLOW_MODE(&desc[idx], BYPASS);
		HW_DESC_SET_DOUT_DLLI(&desc[idx],
				      (padded_authkey_dma_addr + digestsize),
				      (blocksize - digestsize),
				      AXI_ID, NS_BIT, 0);
		idx++;
	} else {
		HW_DESC_INIT(&desc[idx]);
		HW_DESC_SET_DIN_TYPE(&desc[idx], DMA_DLLI,
				     key_dma_addr,
				     keylen, AXI_ID, NS_BIT);
		HW_DESC_SET_FLOW_MODE(&desc[idx], BYPASS);
		HW_DESC_SET_DOUT_DLLI(&desc[idx],
				      (padded_authkey_dma_addr),
				      keylen, AXI_ID, NS_BIT, 0);
		idx++;

		if ((blocksize - keylen) != 0) {
			HW_DESC_INIT(&desc[idx]);
			HW_DESC_SET_DIN_CONST(&desc[idx], 0,
					      (blocksize - keylen));
			HW_DESC_SET_FLOW_MODE(&desc[idx], BYPASS);
			HW_DESC_SET_DOUT_DLLI(&desc[idx],
				(padded_authkey_dma_addr + keylen),
				(blocksize - keylen),
				AXI_ID, NS_BIT, 0);
			idx++;
		}
	}

	rc = send_request(crypto_drvdata, &dx_req, desc, idx, 0);
	if (unlikely(rc != 0)) {
		DX_LOG_ERR("send_request() failed (rc=%d)\n", rc);
	}

	dma_unmap_single(&crypto_drvdata->plat_dev->dev,
			key_dma_addr,
			keylen, DMA_TO_DEVICE);

	return rc;
}


static int
dx_aead_setkey(struct crypto_aead *tfm, const u8 *key, unsigned int keylen)
{
	struct dx_aead_ctx *ctx = crypto_aead_ctx(tfm);
	struct rtattr *rta = (struct rtattr *)key;
	struct dx_crypto_req dx_req = {0};
	struct crypto_authenc_key_param *param;
	HwDesc_s desc[MAX_AEAD_SETKEY_SEQ];
	int seq_len = 0, rc = -EINVAL;
	DECL_CYCLE_COUNT_RESOURCES;

	DX_LOG_DEBUG("Setting key in context @%p for %s. key=%p keylen=%u\n",
		ctx, crypto_tfm_alg_name(crypto_aead_tfm(tfm)), key, keylen);

	/* STAT_PHASE_0: Init and sanity checks */
	START_CYCLE_COUNT();

	if (ctx->auth_mode != SEP_HASH_NULL) { /* authenc() alg. */
		if (!RTA_OK(rta, keylen))
			goto badkey;
		if (rta->rta_type != CRYPTO_AUTHENC_KEYA_PARAM)
			goto badkey;
		if (RTA_PAYLOAD(rta) < sizeof(*param))
			goto badkey;
		param = RTA_DATA(rta);
		ctx->enc_keylen = be32_to_cpu(param->enckeylen);
		key += RTA_ALIGN(rta->rta_len);
		keylen -= RTA_ALIGN(rta->rta_len);
		if (keylen < ctx->enc_keylen)
			goto badkey;
		ctx->auth_keylen = keylen - ctx->enc_keylen;

		if (ctx->cipher_mode == SEP_CIPHER_CTR) {
			/* the nonce is stored in bytes at end of key */
			if (ctx->enc_keylen <
			    (AES_MIN_KEY_SIZE + CTR_RFC3686_NONCE_SIZE))
				goto badkey;
			/* Copy nonce from last 4 bytes in CTR key to
			*  first 4 bytes in CTR IV */
			memcpy(ctx->ctr_nonce, key + ctx->auth_keylen + ctx->enc_keylen -
				CTR_RFC3686_NONCE_SIZE, CTR_RFC3686_NONCE_SIZE);
			/* Set CTR key size */
			ctx->enc_keylen -= CTR_RFC3686_NONCE_SIZE;
		}
	} else { /* non-authenc - has just one key */
		ctx->enc_keylen = keylen;
		ctx->auth_keylen = 0;
	}

	rc = validate_keys_sizes(ctx);
	if (unlikely(rc != 0))
		goto badkey;

	END_CYCLE_COUNT(STAT_OP_TYPE_SETKEY, STAT_PHASE_0);
	/* STAT_PHASE_1: Copy key to ctx */
	START_CYCLE_COUNT();

	/* Get key meterial */
	memcpy(ctx->enckey, key + ctx->auth_keylen, ctx->enc_keylen);
	if (ctx->enc_keylen == 24)
		memset(ctx->enckey + 24, 0, SEP_AES_KEY_SIZE_MAX - 24);
	if (ctx->auth_mode == SEP_HASH_XCBC_MAC) {
		memcpy(ctx->auth_state.xcbc.xcbc_keys, key, ctx->auth_keylen);
	} else if (ctx->auth_mode != SEP_HASH_NULL) { /* HMAC */
		u8 *key_p = (u8 *)key;
		if (ctx->auth_keylen == 0) {
			/* Support auth. key ZERO size.
			   Allocate one-byte-zero key and set the auth-keylen
			   to "one". The result is 64B ZEROs padded autheky. */
			const u8 one_zero_key[1] = {0};
			key_p = (u8 *)one_zero_key;
			ctx->auth_keylen = 1;
		}
		rc = dx_get_plain_hmac_key(tfm, key_p, ctx->auth_keylen);
		if (rc != 0)
			goto badkey;
	}

	END_CYCLE_COUNT(STAT_OP_TYPE_SETKEY, STAT_PHASE_1);

	/* STAT_PHASE_2: Create sequence */
	START_CYCLE_COUNT();

	switch (ctx->auth_mode) {
	case SEP_HASH_SHA1:
	case SEP_HASH_SHA256:
		seq_len = hmac_setkey(desc, ctx);
		break;
	case SEP_HASH_XCBC_MAC:
		seq_len = xcbc_setkey(desc, ctx);
		break;
	case SEP_HASH_NULL: /* non-authenc modes, e.g., CCM */
		break; /* No auth. key setup */
	default:
		DX_LOG_ERR("Unsupported authenc (%d)\n", ctx->auth_mode);
		rc = -ENOTSUPP;
		goto badkey;
	}

	END_CYCLE_COUNT(STAT_OP_TYPE_SETKEY, STAT_PHASE_2);

	/* STAT_PHASE_3: Submit sequence to HW */
	START_CYCLE_COUNT();

	if (seq_len > 0) { /* For CCM there is no sequence to setup the key */
#ifdef ENABLE_CYCLE_COUNT
		dx_req.op_type = STAT_OP_TYPE_SETKEY;
#endif
		rc = send_request(crypto_drvdata, &dx_req, desc, seq_len, 0);
		if (unlikely(rc != 0))
			goto setkey_error;
	}

	/* Update STAT_PHASE_3 */
	END_CYCLE_COUNT(STAT_OP_TYPE_SETKEY, STAT_PHASE_3);
	return rc;

badkey:
	crypto_aead_set_flags(tfm, CRYPTO_TFM_RES_BAD_KEY_LEN);

setkey_error:
	return rc;
}


static int dx_rfc4309_ccm_setkey(struct crypto_aead *tfm, const u8 *key, unsigned int keylen)
{
	struct dx_aead_ctx *ctx = crypto_aead_ctx(tfm);
	int rc = 0;

	if (keylen < 3)
		return -EINVAL;

	keylen -= 3;
	memcpy(ctx->ctr_nonce, key + keylen, 3);

	rc = dx_aead_setkey(tfm, key, keylen);

	return rc;
}

static int dx_aead_setauthsize(
	struct crypto_aead *authenc,
	unsigned int authsize)
{
	struct dx_aead_ctx *ctx = crypto_aead_ctx(authenc);

	/* Unsupported auth. sizes */
	if ((authsize == 0) || (authsize > crypto_aead_alg(authenc)->maxauthsize)){
		return -ENOTSUPP;
	}

	ctx->authsize = authsize;
	DX_LOG_DEBUG("authlen=%d\n", ctx->authsize);

	return 0;
}

static int dx_rfc4309_ccm_setauthsize(struct crypto_aead *authenc,
				      unsigned int authsize)
{
	switch (authsize) {
	case 8:
	case 12:
	case 16:
		break;
	default:
		return -EINVAL;
	}

	return dx_aead_setauthsize(authenc, authsize);
}

static int dx_ccm_setauthsize(struct crypto_aead *authenc,
				      unsigned int authsize)
{
	switch (authsize) {
	case 4:
	case 6:
	case 8:
	case 10:
	case 12:
	case 14:
	case 16:
		break;
	default:
		return -EINVAL;
	}

	return dx_aead_setauthsize(authenc, authsize);
}


static inline void dx_aead_create_assoc_desc(struct aead_request *areq,
				   unsigned int flow_mode,
				   HwDesc_s desc[],
				   unsigned int *seq_size,
				   unsigned int **mlli_sram_addr)
{
	struct crypto_aead *tfm = crypto_aead_reqtfm(areq);
	struct dx_aead_ctx *ctx = crypto_aead_ctx(tfm);
	struct aead_req_ctx *areq_ctx = aead_request_ctx(areq);
	unsigned int idx = *seq_size;

	if (likely(areq_ctx->assoc_dma_buf_type == DX_DMA_BUF_DLLI)) {
		DX_LOG_DEBUG("DLLI mode\n");
		HW_DESC_INIT(&desc[idx]);
		HW_DESC_SET_DIN_TYPE(&desc[idx], DMA_DLLI,
				     sg_dma_address(areq->assoc),
				     areq->assoclen, AXI_ID, NS_BIT);
		HW_DESC_SET_FLOW_MODE(&desc[idx], flow_mode);
		if (ctx->auth_mode == SEP_HASH_XCBC_MAC)
			HW_DESC_SET_DIN_NOT_LAST_INDICATION(&desc[idx]);
		idx++;
	} else {
		DX_LOG_DEBUG("MLLI mode\n");
		/* bypass */
		HW_DESC_INIT(&desc[idx]);
		HW_DESC_SET_DIN_TYPE(&desc[idx], DMA_DLLI,
				     areq_ctx->mlli_params.mlli_dma_addr,
				     areq_ctx->mlli_params.mlli_len,
				     AXI_ID, NS_BIT);

		HW_DESC_SET_DOUT_SRAM(&desc[idx],
				      (uint32_t)*mlli_sram_addr,
				      areq_ctx->mlli_params.mlli_len);
		HW_DESC_SET_FLOW_MODE(&desc[idx], BYPASS);
		idx++;

		/* process */
		HW_DESC_INIT(&desc[idx]);
		HW_DESC_SET_DIN_TYPE(&desc[idx], DMA_MLLI,
				(uint32_t)*mlli_sram_addr, 0, AXI_ID, NS_BIT);
		HW_DESC_SET_FLOW_MODE(&desc[idx], flow_mode);
		if (ctx->auth_mode == SEP_HASH_XCBC_MAC)
			HW_DESC_SET_DIN_NOT_LAST_INDICATION(&desc[idx]);
		idx++;
		if (ctx->cipher_mode == SEP_CIPHER_CCM) {/*we have additional entry of configutation buffer*/
			*mlli_sram_addr = &((*mlli_sram_addr)[2*(areq_ctx->assoc_nents+1)]);
		} else {
			*mlli_sram_addr = &((*mlli_sram_addr)[2*areq_ctx->assoc_nents]);
		}
	}
	/* return updated desc sequence size */
	*seq_size = idx;
}

static inline void
dx_aead_create_auth_data_desc(
	struct aead_request *areq,
	unsigned int flow_mode,
	HwDesc_s desc[],
	unsigned int *seq_size,
	unsigned int *mlli_sram_addr,
	int direct)
{
	unsigned int idx = *seq_size;
	struct aead_req_ctx *areq_ctx = aead_request_ctx(areq);

	if (likely(areq_ctx->data_dma_buf_type == DX_DMA_BUF_DLLI)) {
		struct scatterlist *cipher =
			(direct == SEP_CRYPTO_DIRECTION_ENCRYPT) ?
			areq->dst : areq->src;

		DX_LOG_DEBUG("DLLI mode\n");
		HW_DESC_INIT(&desc[idx]);
		HW_DESC_SET_DIN_TYPE(&desc[idx], DMA_DLLI,
			sg_dma_address(cipher), areq_ctx->cryptlen,
			AXI_ID, NS_BIT);
		HW_DESC_SET_FLOW_MODE(&desc[idx], flow_mode);
		idx++;
	} else {
		DX_LOG_DEBUG("MLLI mode\n");
		if (areq_ctx->assoc_dma_buf_type != DX_DMA_BUF_MLLI) {
			/* Copy MLLI-to-SRAM only if assoc data
			*  was processed thru DLLI */
			HW_DESC_INIT(&desc[idx]);
			HW_DESC_SET_DIN_TYPE(&desc[idx], DMA_DLLI,
					areq_ctx->mlli_params.mlli_dma_addr,
					areq_ctx->mlli_params.mlli_len,
					AXI_ID, NS_BIT);
			HW_DESC_SET_DOUT_SRAM(&desc[idx], (uint32_t)mlli_sram_addr,
					areq_ctx->mlli_params.mlli_len);
			HW_DESC_SET_FLOW_MODE(&desc[idx], BYPASS);
			idx++;
		}

		/* process */
		HW_DESC_INIT(&desc[idx]);
		if ((areq_ctx->out_nents == 0) ||
		    (direct == SEP_CRYPTO_DIRECTION_DECRYPT)) {
			/* Inplace or decrypt operation */
			HW_DESC_SET_DIN_TYPE(&desc[idx], DMA_MLLI,
				(uint32_t)mlli_sram_addr, 0, AXI_ID, NS_BIT);
		} else {
			/* Non-inplace and encrypt only */
			HW_DESC_SET_DIN_TYPE(&desc[idx], DMA_MLLI,
					     (uint32_t)&mlli_sram_addr[
					     2 * (areq_ctx->in_nents -
						  areq_ctx->icv_only_frag)],
						  0, AXI_ID, NS_BIT);
		}
		HW_DESC_SET_FLOW_MODE(&desc[idx], flow_mode);
		idx++;
	}
	/* return updated desc sequence size */
	*seq_size = idx;
}

static inline void
dx_aead_create_cipher_data_desc(
	struct aead_request *areq,
	unsigned int flow_mode,
	HwDesc_s desc[],
	unsigned int *seq_size,
	unsigned int *mlli_sram_addr,
	int direct)
{
	unsigned int idx = *seq_size;
	struct aead_req_ctx *areq_ctx = aead_request_ctx(areq);


	if (likely(areq_ctx->data_dma_buf_type == DX_DMA_BUF_DLLI)) {
		DX_LOG_DEBUG(" process DLLI \n");
		HW_DESC_INIT(&desc[idx]);
		HW_DESC_SET_DIN_TYPE(&desc[idx], DMA_DLLI,
				     sg_dma_address(areq->src),
				     areq_ctx->cryptlen, AXI_ID, NS_BIT);
		HW_DESC_SET_DOUT_DLLI(&desc[idx],
				     sg_dma_address(areq->dst),
				     areq_ctx->cryptlen, AXI_ID, NS_BIT, 0);
		HW_DESC_SET_FLOW_MODE(&desc[idx], flow_mode);
		idx++;
	} else {
		/* bypass */
		DX_LOG_DEBUG(" process MLLI \n");
		if (areq_ctx->assoc_dma_buf_type != DX_DMA_BUF_MLLI) {
			/* MLLI was not loaded yet */
			HW_DESC_INIT(&desc[idx]);
			HW_DESC_SET_DIN_TYPE(&desc[idx], DMA_DLLI,
				areq_ctx->mlli_params.mlli_dma_addr,
				areq_ctx->mlli_params.mlli_len, AXI_ID, NS_BIT);
			HW_DESC_SET_DOUT_SRAM(&desc[idx],
				(uint32_t)mlli_sram_addr,
				areq_ctx->mlli_params.mlli_len);
			HW_DESC_SET_FLOW_MODE(&desc[idx], BYPASS);
			idx++;
		}
		HW_DESC_INIT(&desc[idx]);
		HW_DESC_SET_DIN_TYPE(&desc[idx], DMA_MLLI,
				(uint32_t)mlli_sram_addr, 0, AXI_ID, NS_BIT);
		if (areq_ctx->out_nents == 0) {
			HW_DESC_SET_DOUT_MLLI(&desc[idx],
				(uint32_t)mlli_sram_addr, 0, AXI_ID, NS_BIT, 0);
		} else {
			HW_DESC_SET_DOUT_MLLI(&desc[idx],
				(uint32_t)&mlli_sram_addr[
					2 * (areq_ctx->in_nents -
					     areq_ctx->icv_only_frag)],
					0, AXI_ID, NS_BIT,0);
		}
		HW_DESC_SET_FLOW_MODE(&desc[idx], flow_mode);
		idx++;
	}

	*seq_size = idx;
}

static inline void dx_aead_create_frag_icv_desc(struct device *dev,
					       struct aead_request *req,
					       HwDesc_s desc[],
					       unsigned int *seq_size)
{
	struct aead_req_ctx *areq_ctx = aead_request_ctx(req);
	int nents = areq_ctx->frag_icv_sg_nents;
	struct scatterlist *sg = areq_ctx->frag_icv_sg;
	int lbytes = areq_ctx->last_bytes;
	int idx = *seq_size;

	if (areq_ctx->gen_ctx.op_type == SEP_CRYPTO_DIRECTION_DECRYPT){
		/* In decrypt the ICV is always copied from the src*/
		/*Copy the first part of the ICV
		  from the one before last entry*/
		HW_DESC_INIT(&desc[idx]);
		HW_DESC_SET_DIN_TYPE(&desc[idx],
				DMA_DLLI,
				(sg_phys(&sg[nents - 2]) +
				sg_dma_len(&sg[nents - 2]) -
				(areq_ctx->req_authsize - lbytes)),
				(areq_ctx->req_authsize - lbytes),
				AXI_ID, NS_BIT);
		HW_DESC_SET_DOUT_DLLI(&desc[idx],
				      areq_ctx->mac_buf_dma_addr,
				      (areq_ctx->req_authsize - lbytes),
				      AXI_ID, NS_BIT, 0);
		HW_DESC_SET_FLOW_MODE(&desc[idx], BYPASS);
		idx++;
		/*Copy the rest of the ICV
		  from the last entry*/
		HW_DESC_INIT(&desc[idx]);
		HW_DESC_SET_DIN_TYPE(&desc[idx], DMA_DLLI,
				     sg_phys(&sg[nents - 1]),
				     lbytes,
				     AXI_ID, NS_BIT);
		HW_DESC_SET_DOUT_DLLI(&desc[idx],
				      areq_ctx->mac_buf_dma_addr +
				      (areq_ctx->req_authsize - lbytes),
				      lbytes,
				      AXI_ID, NS_BIT, 0);
		HW_DESC_SET_FLOW_MODE(&desc[idx], BYPASS);
		idx++;

		HW_DESC_INIT(&desc[idx]);
		HW_DESC_SET_DIN_NO_DMA(&desc[idx], 0, 0xfffff0);
		HW_DESC_SET_DOUT_NO_DMA(&desc[idx], 0, 0, 1);
		idx++;

	} else {
		/* memory barriar */
		HW_DESC_INIT(&desc[idx]);
		HW_DESC_SET_DIN_NO_DMA(&desc[idx], 0, 0xfffff0);
		HW_DESC_SET_DOUT_NO_DMA(&desc[idx], 0, 0, 1);
		idx++;

		/*Copy the first part of the ICV
		  to the one before last entry*/
		HW_DESC_INIT(&desc[idx]);
		HW_DESC_SET_DIN_TYPE(&desc[idx],
				      DMA_DLLI,
				      areq_ctx->mac_buf_dma_addr,
				      (areq_ctx->req_authsize - lbytes),
				      AXI_ID, NS_BIT);
		HW_DESC_SET_DOUT_DLLI(&desc[idx],
				(sg_phys(&sg[nents - 2]) +
				sg_dma_len(&sg[nents - 2]) -
				(areq_ctx->req_authsize - lbytes)),
				(areq_ctx->req_authsize - lbytes),
				AXI_ID, NS_BIT, 0);
		HW_DESC_SET_FLOW_MODE(&desc[idx], BYPASS);
		idx++;
		/*Copy the rest of the ICV
		  to the last entry*/
		HW_DESC_INIT(&desc[idx]);
		HW_DESC_SET_DIN_TYPE(&desc[idx],
				      DMA_DLLI,
				      areq_ctx->mac_buf_dma_addr +
				      (areq_ctx->req_authsize - lbytes),
				      lbytes,
				      AXI_ID, NS_BIT);
		HW_DESC_SET_DOUT_DLLI(&desc[idx],sg_phys(&sg[nents - 1]),
				     lbytes,
				     AXI_ID, NS_BIT, 1);
		HW_DESC_SET_FLOW_MODE(&desc[idx], BYPASS);
		idx++;
	}
	*seq_size = idx;
}


static inline void
dx_aead_any_cipher_to_hmac_authenc(
	struct aead_request *req,
	HwDesc_s desc[],
	unsigned int *seq_size)
{
	struct crypto_aead *tfm = crypto_aead_reqtfm(req);
	struct dx_aead_ctx *ctx = crypto_aead_ctx(tfm);
	struct aead_req_ctx *req_ctx = aead_request_ctx(req);
	struct dx_aead_handle *aead_handle = crypto_drvdata->aead_handle;
	int direct = req_ctx->gen_ctx.op_type;
	unsigned hw_iv_size = req_ctx->hw_iv_size;
	unsigned int *mlli_sram_addr = crypto_drvdata->mlli_sram_addr;
	unsigned int setup_flow_mode;
	unsigned int data_flow_mode;
	unsigned int hash_mode = (ctx->auth_mode == SEP_HASH_SHA1) ?
				SEP_HASH_HW_SHA1 : SEP_HASH_HW_SHA256;
	unsigned int digest_size = (ctx->auth_mode == SEP_HASH_SHA1) ?
				SEP_SHA1_DIGEST_SIZE : SEP_SHA256_DIGEST_SIZE;
	unsigned int idx = *seq_size;
	int is_hw_verify_icv = likely(IS_ALIGNED(ctx->authsize, sizeof(uint32_t)));
	dma_addr_t icv_dma_addr;

	setup_flow_mode = ctx->flow_mode;
	if (direct == SEP_CRYPTO_DIRECTION_ENCRYPT) {
		/* Encrypt */
		if (setup_flow_mode == S_DIN_to_AES) {
			data_flow_mode = AES_to_HASH_and_DOUT;
		} else {
			data_flow_mode = DES_to_HASH_and_DOUT;
		}
	} else {
		/* Decrypt */
		if (setup_flow_mode == S_DIN_to_AES) {
			data_flow_mode = AES_and_HASH;
		} else {
			data_flow_mode = DES_and_HASH;
		}
	}

	/* Loading hash ipad xor key state */
	HW_DESC_INIT(&desc[idx]);
	HW_DESC_SET_CIPHER_MODE(&desc[idx], hash_mode);
	HW_DESC_SET_DIN_TYPE(&desc[idx], DMA_DLLI, ctx->auth_state.hmac.ipad_opad_dma_addr, digest_size, AXI_ID, NS_BIT);
	HW_DESC_SET_FLOW_MODE(&desc[idx], S_DIN_to_HASH);
	HW_DESC_SET_SETUP_MODE(&desc[idx], SETUP_LOAD_STATE0);
	idx++;

	/* Load init. digest len (64 bytes) */
	HW_DESC_INIT(&desc[idx]);
	HW_DESC_SET_CIPHER_MODE(&desc[idx], hash_mode);
	HW_DESC_SET_DIN_SRAM(&desc[idx], dx_ahash_get_initial_digest_len_sram_addr(NULL), HASH_LEN_SIZE);
	HW_DESC_SET_FLOW_MODE(&desc[idx], S_DIN_to_HASH);
	HW_DESC_SET_SETUP_MODE(&desc[idx], SETUP_LOAD_KEY0);
	idx++;

	/* Clobber associated data */
	if (req->assoclen > 0)
		dx_aead_create_assoc_desc(req, DIN_HASH, desc,
					&idx, &mlli_sram_addr);

	HW_DESC_INIT(&desc[idx]);
	if (ctx->cipher_mode == SEP_CIPHER_CTR) {
		HW_DESC_SET_DIN_TYPE(&desc[idx], DMA_DLLI,
			req_ctx->gen_ctx.iv_dma_addr + CTR_RFC3686_NONCE_SIZE,
			CTR_RFC3686_IV_SIZE, AXI_ID, NS_BIT);
	} else {
		HW_DESC_SET_DIN_TYPE(&desc[idx], DMA_DLLI,
			req_ctx->gen_ctx.iv_dma_addr, hw_iv_size, AXI_ID, NS_BIT);
	}
	HW_DESC_SET_FLOW_MODE(&desc[idx], DIN_HASH);
	idx++;

	/* Setup cipher state */
	HW_DESC_INIT(&desc[idx]);
	HW_DESC_SET_CIPHER_CONFIG0(&desc[idx], direct);
	HW_DESC_SET_FLOW_MODE(&desc[idx], setup_flow_mode);
	HW_DESC_SET_DIN_TYPE(&desc[idx], DMA_DLLI,
		req_ctx->gen_ctx.iv_dma_addr, hw_iv_size, AXI_ID, NS_BIT);
	if (ctx->cipher_mode == SEP_CIPHER_CTR) {
		HW_DESC_SET_SETUP_MODE(&desc[idx], SETUP_LOAD_STATE1);
	} else {
		HW_DESC_SET_SETUP_MODE(&desc[idx], SETUP_LOAD_STATE0);
	}
	HW_DESC_SET_CIPHER_MODE(&desc[idx], ctx->cipher_mode);
	idx++;

	/* Setup enc. key */
	HW_DESC_INIT(&desc[idx]);
	HW_DESC_SET_CIPHER_CONFIG0(&desc[idx], direct);
	HW_DESC_SET_SETUP_MODE(&desc[idx], SETUP_LOAD_KEY0);
	HW_DESC_SET_FLOW_MODE(&desc[idx], setup_flow_mode);
	if (setup_flow_mode == S_DIN_to_AES) {
		HW_DESC_SET_DIN_TYPE(&desc[idx], DMA_DLLI, ctx->enckey_dma_addr,
			((ctx->enc_keylen == 24) ? SEP_AES_KEY_SIZE_MAX : ctx->enc_keylen), AXI_ID, NS_BIT);
		HW_DESC_SET_KEY_SIZE_AES(&desc[idx], ctx->enc_keylen);
	} else {
		HW_DESC_SET_DIN_TYPE(&desc[idx], DMA_DLLI, ctx->enckey_dma_addr, ctx->enc_keylen, AXI_ID, NS_BIT);
		HW_DESC_SET_KEY_SIZE_DES(&desc[idx], ctx->enc_keylen);
	}
	HW_DESC_SET_CIPHER_MODE(&desc[idx], ctx->cipher_mode);
	idx++;

	if (req_ctx->cryptlen > 0) {
		if (ctx->cipher_mode == SEP_CIPHER_CTR) {
			if (direct == SEP_CRYPTO_DIRECTION_ENCRYPT) {
				/* Encrypt text */
				dx_aead_create_cipher_data_desc(req, DIN_AES_DOUT, desc, &idx, mlli_sram_addr, direct);

				/* We must wait for DMA to write all cipher */
				HW_DESC_INIT(&desc[idx]);
				HW_DESC_SET_DIN_NO_DMA(&desc[idx], 0, 0xfffff0);
				HW_DESC_SET_DOUT_NO_DMA(&desc[idx], 0, 0, 1);
				idx++;

				/* Authenticate */
				dx_aead_create_auth_data_desc(req, DIN_HASH, desc, &idx, mlli_sram_addr, direct);
			} else {
				/* Authenticate */
				dx_aead_create_auth_data_desc(req, DIN_HASH, desc, &idx, mlli_sram_addr, direct);
				/* Decrypt cipher */
				dx_aead_create_cipher_data_desc(req, DIN_AES_DOUT, desc, &idx, mlli_sram_addr, direct);
			}
		} else {
			/* Cipher processing + authentication */
			dx_aead_create_cipher_data_desc(req, data_flow_mode, desc, &idx, mlli_sram_addr, direct);
		}
	}

	HW_DESC_INIT(&desc[idx]);
	HW_DESC_SET_CIPHER_MODE(&desc[idx], hash_mode);
	HW_DESC_SET_DOUT_SRAM(&desc[idx], aead_handle->sram_workspace_addr, HASH_LEN_SIZE);
	HW_DESC_SET_FLOW_MODE(&desc[idx], S_HASH_to_DOUT);
	HW_DESC_SET_SETUP_MODE(&desc[idx], SETUP_WRITE_STATE1);
	HW_DESC_SET_CIPHER_DO(&desc[idx], DO_PAD);
	idx++;

	/* Get final ICV result */
	HW_DESC_INIT(&desc[idx]);
	HW_DESC_SET_DOUT_SRAM(&desc[idx], aead_handle->sram_workspace_addr, digest_size);
	HW_DESC_SET_FLOW_MODE(&desc[idx], S_HASH_to_DOUT);
	HW_DESC_SET_SETUP_MODE(&desc[idx], SETUP_WRITE_STATE0);
	HW_DESC_SET_CIPHER_CONFIG0(&desc[idx], HASH_DIGEST_RESULT_LITTLE_ENDIAN);
	HW_DESC_SET_CIPHER_MODE(&desc[idx], hash_mode);
	idx++;

	/* Loading hash opad xor key state */
	HW_DESC_INIT(&desc[idx]);
	HW_DESC_SET_CIPHER_MODE(&desc[idx], hash_mode);
	HW_DESC_SET_DIN_TYPE(&desc[idx], DMA_DLLI, ctx->auth_state.hmac.ipad_opad_dma_addr + digest_size, digest_size, AXI_ID, NS_BIT);
	HW_DESC_SET_FLOW_MODE(&desc[idx], S_DIN_to_HASH);
	HW_DESC_SET_SETUP_MODE(&desc[idx], SETUP_LOAD_STATE0);
	idx++;

	/* Load init. digest len (64 bytes) */
	HW_DESC_INIT(&desc[idx]);
	HW_DESC_SET_CIPHER_MODE(&desc[idx], hash_mode);
	HW_DESC_SET_DIN_SRAM(&desc[idx], dx_ahash_get_initial_digest_len_sram_addr(NULL), HASH_LEN_SIZE);
	HW_DESC_SET_CIPHER_CONFIG1(&desc[idx], HASH_PADDING_ENABLED);
	HW_DESC_SET_FLOW_MODE(&desc[idx], S_DIN_to_HASH);
	HW_DESC_SET_SETUP_MODE(&desc[idx], SETUP_LOAD_KEY0);
	idx++;

	/* Perform HASH update */
	HW_DESC_INIT(&desc[idx]);
	HW_DESC_SET_DIN_SRAM(&desc[idx], aead_handle->sram_workspace_addr, digest_size);
	HW_DESC_SET_FLOW_MODE(&desc[idx], DIN_HASH);
	idx++;

	/* Get final ICV result */
	if (direct == SEP_CRYPTO_DIRECTION_ENCRYPT) {
		if (unlikely(req_ctx->is_icv_frag == true))
			icv_dma_addr = req_ctx->mac_buf_dma_addr;
		else
			icv_dma_addr = (sg_dma_address(req_ctx->icv_sg) +
					req_ctx->icv_offset);

		HW_DESC_INIT(&desc[idx]);
		HW_DESC_SET_FLOW_MODE(&desc[idx], S_HASH_to_DOUT);
		HW_DESC_SET_SETUP_MODE(&desc[idx], SETUP_WRITE_STATE0);
		HW_DESC_SET_DOUT_DLLI(&desc[idx], icv_dma_addr,
			ctx->authsize, AXI_ID, NS_BIT, !req_ctx->is_icv_frag);
		HW_DESC_SET_CIPHER_CONFIG0(&desc[idx], HASH_DIGEST_RESULT_LITTLE_ENDIAN);
		HW_DESC_SET_CIPHER_MODE(&desc[idx], hash_mode);
		idx++;

		if (unlikely(req_ctx->is_icv_frag == true))
			/* Copy ICV into fragmented dst buffer */
			dx_aead_create_frag_icv_desc(
				&crypto_drvdata->plat_dev->dev,
				req, desc, &idx);
	} else { /* Decrypt */
		if ((req_ctx->is_icv_frag == true) || (is_hw_verify_icv == false)) {
			/* ICV is taken from internal buffer */
			icv_dma_addr = req_ctx->mac_buf_dma_addr;
		} else {
			/* ICV is treated from user request */
			icv_dma_addr = (sg_dma_address(req_ctx->icv_sg) +
					req_ctx->icv_offset);
		}

		if (unlikely(req_ctx->is_icv_frag == true))
			/* Copy fragmented ICV into contiguous buffer */
			dx_aead_create_frag_icv_desc(
				&crypto_drvdata->plat_dev->dev,
				req, desc, &idx);

		HW_DESC_INIT(&desc[idx]);
		if (likely(is_hw_verify_icv)) {
			/* Feed in ICV to hardware */
			HW_DESC_SET_SETUP_MODE(&desc[idx], SETUP_LOAD_XEX_KEY);
			HW_DESC_SET_FLOW_MODE(&desc[idx], S_DIN_to_HASH);
			HW_DESC_SET_DIN_TYPE(&desc[idx], DMA_DLLI, icv_dma_addr,
				ctx->authsize, AXI_ID, NS_BIT);
		} else {
			/* Get ICV out from hardware */
			HW_DESC_SET_SETUP_MODE(&desc[idx], SETUP_WRITE_STATE0);
			HW_DESC_SET_FLOW_MODE(&desc[idx], S_HASH_to_DOUT);
			HW_DESC_SET_DOUT_DLLI(&desc[idx], icv_dma_addr,
				ctx->authsize, AXI_ID, NS_BIT, 1);
		}
		HW_DESC_SET_CIPHER_CONFIG0(&desc[idx], HASH_DIGEST_RESULT_LITTLE_ENDIAN);
		HW_DESC_SET_CIPHER_CONFIG1(&desc[idx], HASH_PADDING_DISABLED);
		HW_DESC_SET_CIPHER_MODE(&desc[idx], hash_mode);
		idx++;

		if (likely(is_hw_verify_icv)) {
			/* Compare ICV in HW */
			HW_DESC_INIT(&desc[idx]);
			HW_DESC_SET_DOUT_DLLI(&desc[idx], req_ctx->mac_buf_dma_addr,
				ICV_CMP_SIZE, AXI_ID, NS_BIT, 1);
			HW_DESC_SET_FLOW_MODE(&desc[idx], S_HASH_to_DOUT);
			HW_DESC_SET_SETUP_MODE(&desc[idx], SETUP_WRITE_STATE1);
			HW_DESC_SET_CIPHER_MODE(&desc[idx], hash_mode);
			idx++;
		}
	}

	*seq_size = idx;
}

static inline void
dx_aead_any_cipher_to_xcbc_authenc(
	struct aead_request *req,
	HwDesc_s desc[],
	unsigned int *seq_size)
{
	struct crypto_aead *tfm = crypto_aead_reqtfm(req);
	struct dx_aead_ctx *ctx = crypto_aead_ctx(tfm);
	struct aead_req_ctx *req_ctx = aead_request_ctx(req);
	int direct = req_ctx->gen_ctx.op_type;
	unsigned hw_iv_size = req_ctx->hw_iv_size;
	unsigned int setup_flow_mode = ctx->flow_mode;
	unsigned int *mlli_sram_addr = crypto_drvdata->mlli_sram_addr;
	unsigned int data_flow_mode;
	unsigned int idx = *seq_size;
	int is_hw_verify_icv = likely(IS_ALIGNED(ctx->authsize, sizeof(uint32_t)));
	dma_addr_t icv_dma_addr;


	if (direct == SEP_CRYPTO_DIRECTION_ENCRYPT) {
		/* Encrypt */
		if (setup_flow_mode == S_DIN_to_AES) {
			data_flow_mode = AES_to_HASH_and_DOUT;
		} else {
			data_flow_mode = DES_to_HASH_and_DOUT;
		}
	} else {
		/* Decrypt */
		if (setup_flow_mode == S_DIN_to_AES) {
			data_flow_mode = AES_and_HASH;
		} else {
			data_flow_mode = DES_and_HASH;
		}
	}

	/* Loading MAC state */
	HW_DESC_INIT(&desc[idx]);
	HW_DESC_SET_DIN_CONST(&desc[idx], 0, SEP_AES_BLOCK_SIZE);
	HW_DESC_SET_SETUP_MODE(&desc[idx], SETUP_LOAD_STATE0);
	HW_DESC_SET_CIPHER_MODE(&desc[idx], SEP_CIPHER_XCBC_MAC);
	HW_DESC_SET_CIPHER_CONFIG0(&desc[idx], DESC_DIRECTION_ENCRYPT_ENCRYPT);
	HW_DESC_SET_KEY_SIZE_AES(&desc[idx], SEP_AES_128_BIT_KEY_SIZE);
	HW_DESC_SET_FLOW_MODE(&desc[idx], S_DIN_to_HASH);
	HW_DESC_SET_AES_NOT_HASH_MODE(&desc[idx]);
	idx++;

	/* Setup XCBC MAC K1 */
	HW_DESC_INIT(&desc[idx]);
	HW_DESC_SET_DIN_TYPE(&desc[idx], DMA_DLLI, ctx->auth_state.xcbc.xcbc_keys_dma_addr, AES_KEYSIZE_128, AXI_ID, NS_BIT);
	HW_DESC_SET_SETUP_MODE(&desc[idx], SETUP_LOAD_KEY0);
	HW_DESC_SET_CIPHER_MODE(&desc[idx], SEP_CIPHER_XCBC_MAC);
	HW_DESC_SET_CIPHER_CONFIG0(&desc[idx], DESC_DIRECTION_ENCRYPT_ENCRYPT);
	HW_DESC_SET_KEY_SIZE_AES(&desc[idx], SEP_AES_128_BIT_KEY_SIZE);
	HW_DESC_SET_FLOW_MODE(&desc[idx], S_DIN_to_HASH);
	HW_DESC_SET_AES_NOT_HASH_MODE(&desc[idx]);
	idx++;

	/* Setup XCBC MAC K2 */
	HW_DESC_INIT(&desc[idx]);
	HW_DESC_SET_DIN_TYPE(&desc[idx], DMA_DLLI, ctx->auth_state.xcbc.xcbc_keys_dma_addr + AES_KEYSIZE_128, AES_KEYSIZE_128, AXI_ID, NS_BIT);
	HW_DESC_SET_SETUP_MODE(&desc[idx], SETUP_LOAD_STATE1);
	HW_DESC_SET_CIPHER_MODE(&desc[idx], SEP_CIPHER_XCBC_MAC);
	HW_DESC_SET_CIPHER_CONFIG0(&desc[idx], DESC_DIRECTION_ENCRYPT_ENCRYPT);
	HW_DESC_SET_KEY_SIZE_AES(&desc[idx], SEP_AES_128_BIT_KEY_SIZE);
	HW_DESC_SET_FLOW_MODE(&desc[idx], S_DIN_to_HASH);
	HW_DESC_SET_AES_NOT_HASH_MODE(&desc[idx]);
	idx++;

	/* Setup XCBC MAC K3 */
	HW_DESC_INIT(&desc[idx]);
	HW_DESC_SET_DIN_TYPE(&desc[idx], DMA_DLLI,ctx->auth_state.xcbc.xcbc_keys_dma_addr + 2 * AES_KEYSIZE_128, AES_KEYSIZE_128, AXI_ID, NS_BIT);
	HW_DESC_SET_SETUP_MODE(&desc[idx], SETUP_LOAD_STATE2);
	HW_DESC_SET_CIPHER_MODE(&desc[idx], SEP_CIPHER_XCBC_MAC);
	HW_DESC_SET_CIPHER_CONFIG0(&desc[idx], DESC_DIRECTION_ENCRYPT_ENCRYPT);
	HW_DESC_SET_KEY_SIZE_AES(&desc[idx], SEP_AES_128_BIT_KEY_SIZE);
	HW_DESC_SET_FLOW_MODE(&desc[idx], S_DIN_to_HASH);
	HW_DESC_SET_AES_NOT_HASH_MODE(&desc[idx]);
	idx++;

	/* Clobber associated data */
	if (req->assoclen > 0)
		dx_aead_create_assoc_desc(req, DIN_HASH, desc, &idx, &mlli_sram_addr);

	HW_DESC_INIT(&desc[idx]);
	if (ctx->cipher_mode == SEP_CIPHER_CTR) {
		HW_DESC_SET_DIN_TYPE(&desc[idx], DMA_DLLI,
			req_ctx->gen_ctx.iv_dma_addr + CTR_RFC3686_NONCE_SIZE,
			CTR_RFC3686_IV_SIZE, AXI_ID, NS_BIT);
	} else {
		HW_DESC_SET_DIN_TYPE(&desc[idx], DMA_DLLI,
			req_ctx->gen_ctx.iv_dma_addr, hw_iv_size, AXI_ID, NS_BIT);
	}
	if (req_ctx->cryptlen > 0)
		HW_DESC_SET_DIN_NOT_LAST_INDICATION(&desc[idx]);
	HW_DESC_SET_FLOW_MODE(&desc[idx], DIN_HASH);
	idx++;

	/* Setup cipher state */
	HW_DESC_INIT(&desc[idx]);
	HW_DESC_SET_DIN_TYPE(&desc[idx], DMA_DLLI, req_ctx->gen_ctx.iv_dma_addr, hw_iv_size, AXI_ID, NS_BIT);
	HW_DESC_SET_CIPHER_CONFIG0(&desc[idx], direct);
	HW_DESC_SET_FLOW_MODE(&desc[idx], setup_flow_mode);
	if (ctx->cipher_mode == SEP_CIPHER_CTR) {
		HW_DESC_SET_SETUP_MODE(&desc[idx], SETUP_LOAD_STATE1);
	} else {
		HW_DESC_SET_SETUP_MODE(&desc[idx], SETUP_LOAD_STATE0);
	}
	HW_DESC_SET_CIPHER_MODE(&desc[idx], ctx->cipher_mode);
	idx++;

	/* Setup enc. key */
	HW_DESC_INIT(&desc[idx]);
	HW_DESC_SET_CIPHER_CONFIG0(&desc[idx], direct);
	HW_DESC_SET_SETUP_MODE(&desc[idx], SETUP_LOAD_KEY0);
	HW_DESC_SET_FLOW_MODE(&desc[idx], setup_flow_mode);
	if (setup_flow_mode == S_DIN_to_AES) {
		HW_DESC_SET_DIN_TYPE(&desc[idx], DMA_DLLI, ctx->enckey_dma_addr,
			((ctx->enc_keylen == 24) ? SEP_AES_KEY_SIZE_MAX : ctx->enc_keylen), AXI_ID, NS_BIT);
		HW_DESC_SET_KEY_SIZE_AES(&desc[idx], ctx->enc_keylen);
	} else {
		HW_DESC_SET_DIN_TYPE(&desc[idx], DMA_DLLI, ctx->enckey_dma_addr, ctx->enc_keylen, AXI_ID, NS_BIT);
		HW_DESC_SET_KEY_SIZE_DES(&desc[idx], ctx->enc_keylen);
	}
	HW_DESC_SET_CIPHER_MODE(&desc[idx], ctx->cipher_mode);
	idx++;

	if (req_ctx->cryptlen > 0) {
		if (ctx->cipher_mode == SEP_CIPHER_CTR) {
			if (direct == SEP_CRYPTO_DIRECTION_ENCRYPT) {
				/* Encrypt text */
				dx_aead_create_cipher_data_desc(req, DIN_AES_DOUT, desc, &idx, mlli_sram_addr, direct);

				/* We must wait for DMA to write all cipher */
				HW_DESC_INIT(&desc[idx]);
				HW_DESC_SET_DIN_NO_DMA(&desc[idx], 0, 0xfffff0);
				HW_DESC_SET_DOUT_NO_DMA(&desc[idx], 0, 0, 1);
				idx++;

				/* Authenticate */
				dx_aead_create_auth_data_desc(req, DIN_HASH, desc, &idx, mlli_sram_addr, direct);
			} else {
				/* Authenticate */
				dx_aead_create_auth_data_desc(req, DIN_HASH, desc, &idx, mlli_sram_addr, direct);
				/* Decrypt cipher */
				dx_aead_create_cipher_data_desc(req, DIN_AES_DOUT, desc, &idx, mlli_sram_addr, direct);
			}
		} else {
			/* Cipher processing + authentication */
			dx_aead_create_cipher_data_desc(req, data_flow_mode, desc, &idx, mlli_sram_addr, direct);
		}
	}

	/* Get final ICV result */
	if (direct == SEP_CRYPTO_DIRECTION_ENCRYPT) {
		if (unlikely(req_ctx->is_icv_frag == true))
			icv_dma_addr = req_ctx->mac_buf_dma_addr;
		else
			icv_dma_addr = (sg_dma_address(req_ctx->icv_sg) +
					req_ctx->icv_offset);

		HW_DESC_INIT(&desc[idx]);
		HW_DESC_SET_FLOW_MODE(&desc[idx], S_HASH_to_DOUT);
		HW_DESC_SET_SETUP_MODE(&desc[idx], SETUP_WRITE_STATE0);
		HW_DESC_SET_DOUT_DLLI(&desc[idx], icv_dma_addr,
			ctx->authsize, AXI_ID, NS_BIT, !req_ctx->is_icv_frag);
		HW_DESC_SET_AES_NOT_HASH_MODE(&desc[idx]);
		HW_DESC_SET_CIPHER_MODE(&desc[idx], SEP_CIPHER_XCBC_MAC);
		idx++;

		if (unlikely(req_ctx->is_icv_frag == true))
			/* Copy ICV into fragmented dst buffer */
			dx_aead_create_frag_icv_desc(
				&crypto_drvdata->plat_dev->dev,
				req, desc, &idx);
	} else { /* Decrypt */
		if ((req_ctx->is_icv_frag == true) || (is_hw_verify_icv == false)) {
			/* ICV is taken from internal buffer */
			icv_dma_addr = req_ctx->mac_buf_dma_addr;
		} else {
			/* ICV is treated from user request */
			icv_dma_addr = (sg_dma_address(req_ctx->icv_sg) +
					req_ctx->icv_offset);
		}

		if (unlikely(req_ctx->is_icv_frag == true))
			/* Copy fragmented ICV into contiguous buffer */
			dx_aead_create_frag_icv_desc(
				&crypto_drvdata->plat_dev->dev,
				req, desc, &idx);

		HW_DESC_INIT(&desc[idx]);
		if (is_hw_verify_icv) {
			/* Feed in ICV to hardware */
			HW_DESC_SET_SETUP_MODE(&desc[idx], SETUP_LOAD_XEX_KEY);
			HW_DESC_SET_FLOW_MODE(&desc[idx], S_DIN_to_HASH);
			HW_DESC_SET_DIN_TYPE(&desc[idx], DMA_DLLI, icv_dma_addr,
				ctx->authsize, AXI_ID, NS_BIT);
		} else {
			/* Get ICV out from hardware */
			HW_DESC_SET_SETUP_MODE(&desc[idx], SETUP_WRITE_STATE0);
			HW_DESC_SET_FLOW_MODE(&desc[idx], S_HASH_to_DOUT);
			HW_DESC_SET_DOUT_DLLI(&desc[idx], icv_dma_addr,
				ctx->authsize, AXI_ID, NS_BIT, 1);
		}
		HW_DESC_SET_CIPHER_CONFIG0(&desc[idx], HASH_DIGEST_RESULT_LITTLE_ENDIAN);
		HW_DESC_SET_CIPHER_CONFIG1(&desc[idx], HASH_PADDING_DISABLED);
		HW_DESC_SET_CIPHER_MODE(&desc[idx], SEP_CIPHER_XCBC_MAC);
		HW_DESC_SET_AES_NOT_HASH_MODE(&desc[idx]);
		idx++;

		if (likely(is_hw_verify_icv)) {
			/* Compare ICV in HW */
			HW_DESC_INIT(&desc[idx]);
			HW_DESC_SET_DOUT_DLLI(&desc[idx], req_ctx->mac_buf_dma_addr,
				ICV_CMP_SIZE, AXI_ID, NS_BIT, 1);
			HW_DESC_SET_FLOW_MODE(&desc[idx], S_HASH_to_DOUT);
			HW_DESC_SET_SETUP_MODE(&desc[idx], SETUP_WRITE_STATE1);
			HW_DESC_SET_CIPHER_MODE(&desc[idx], SEP_CIPHER_XCBC_MAC);
			idx++;
		}
	}

	*seq_size = idx;
}

static int validate_data_size(struct dx_aead_ctx *ctx,
	enum sep_crypto_direction direct,
	unsigned int cryptolen, unsigned int assoclen) {
	unsigned int cipherlen = (direct == SEP_CRYPTO_DIRECTION_DECRYPT) ?
			(cryptolen - ctx->authsize) : cryptolen;

	if (unlikely((direct == SEP_CRYPTO_DIRECTION_DECRYPT) &&
		(cryptolen < ctx->authsize)))
		goto data_size_err;

	switch (ctx->flow_mode) {
	case S_DIN_to_AES:
		if (ctx->cipher_mode == SEP_CIPHER_CCM)
			break;
		/* HW limitations on associated data size (Issue #8413) */
		if (unlikely(!IS_ALIGNED(assoclen, sizeof(uint32_t))))
			goto data_size_err;

		if (unlikely((ctx->cipher_mode == SEP_CIPHER_CBC) &&
			!IS_ALIGNED(cipherlen, AES_BLOCK_SIZE)))
			goto data_size_err;
		break;
	case S_DIN_to_DES:
		/* HW limitations on associated data size (Issue #8413) */
		if (unlikely(!IS_ALIGNED(cipherlen, DES_BLOCK_SIZE)) ||
		    unlikely(!IS_ALIGNED(assoclen, DES_BLOCK_SIZE)))
			goto data_size_err;
		break;
	default:
		DX_LOG_ERR("Unexpected flow mode (%d)\n", ctx->flow_mode);
		goto data_size_err;
	}

	return 0;

data_size_err:
	return -EINVAL;
}

static unsigned int format_ccm_a0(uint8_t *pA0Buff, uint32_t headerSize)
{
	unsigned int len = 0;
	if ( headerSize == 0 ) {
		return 0;
	}
	if ( headerSize < ((1UL << 16) - (1UL << 8) )) {
		len = 2;

		pA0Buff[0] = (headerSize >> 8) & 0xFF;
		pA0Buff[1] = headerSize & 0xFF;
	} else {
		len = 6;

		pA0Buff[0] = 0xFF;
		pA0Buff[1] = 0xFE;
		pA0Buff[2] = (headerSize >> 24) & 0xFF;
		pA0Buff[3] = (headerSize >> 16) & 0xFF;
		pA0Buff[4] = (headerSize >> 8) & 0xFF;
		pA0Buff[5] = headerSize & 0xFF;
	}

	return len;
}

static int set_msg_len(u8 *block, unsigned int msglen, int csize)
{
	__be32 data;

	memset(block, 0, csize);
	block += csize;

	if (csize >= 4)
		csize = 4;
	else if (msglen > (1 << (8 * csize)))
		return -EOVERFLOW;

	data = cpu_to_be32(msglen);
	memcpy(block - csize, (u8 *)&data + 4 - csize, csize);

	return 0;
}

static inline int
dx_aead_ccm(struct aead_request *req,
	    HwDesc_s desc[],
	    unsigned int *seq_size)
{
	struct crypto_aead *tfm = crypto_aead_reqtfm(req);
	struct dx_aead_ctx *ctx = crypto_aead_ctx(tfm);
	struct aead_req_ctx *req_ctx = aead_request_ctx(req);
	int direct = req_ctx->gen_ctx.op_type;
	unsigned int *mlli_sram_addr = crypto_drvdata->mlli_sram_addr;
	unsigned int idx = *seq_size;
	unsigned int hw_mode ;

	if (req_ctx->gen_ctx.op_type) {
		hw_mode = AEAD_MODE_CCM_PD;
		direct = DESC_DIRECTION_DECRYPT_ENCRYPT;
	} else {
		hw_mode = AEAD_MODE_CCM_PE;
		direct = DESC_DIRECTION_ENCRYPT_ENCRYPT;
	}

	 /* load state */
	HW_DESC_INIT(&desc[idx]);
	HW_DESC_SET_CIPHER_MODE(&desc[idx], AEAD_MODE_CCM_A);
	HW_DESC_SET_KEY_SIZE_AES(&desc[idx], ctx->enc_keylen);
	HW_DESC_SET_FLOW_MODE(&desc[idx], S_DIN_to_AES);
	HW_DESC_SET_DIN_TYPE(&desc[idx], DMA_DLLI, req_ctx->mac_buf_dma_addr, SEP_AES_BLOCK_SIZE, AXI_ID, NS_BIT);
	HW_DESC_SET_CIPHER_CONFIG0(&desc[idx],direct);
	HW_DESC_SET_SETUP_MODE(&desc[idx], SETUP_LOAD_STATE0);
	idx++;

	/* load key */
	HW_DESC_INIT(&desc[idx]);
	HW_DESC_SET_CIPHER_MODE(&desc[idx], AEAD_MODE_CCM_A);
	HW_DESC_SET_DIN_TYPE(&desc[idx], DMA_DLLI, ctx->enckey_dma_addr,
			((ctx->enc_keylen == 24) ? SEP_AES_KEY_SIZE_MAX : ctx->enc_keylen), AXI_ID, NS_BIT);
	HW_DESC_SET_KEY_SIZE_AES(&desc[idx], ctx->enc_keylen);
	HW_DESC_SET_FLOW_MODE(&desc[idx], S_DIN_to_AES);
	HW_DESC_SET_SETUP_MODE(&desc[idx], SETUP_LOAD_KEY0);
	HW_DESC_SET_CIPHER_CONFIG0(&desc[idx], direct);
	idx++;


	/*should be changed* to one MLLI encryption*/
	if (req->assoclen) {
		dx_aead_create_assoc_desc(req, DIN_AES_DOUT, desc,
					&idx, &mlli_sram_addr);
	} else {
		HW_DESC_INIT(&desc[idx]);
		HW_DESC_SET_DIN_TYPE(&desc[idx], DMA_DLLI,
				      sg_dma_address(&req_ctx->ccm_adata_sg), AES_BLOCK_SIZE + req_ctx->ccm_hdr_size, AXI_ID, NS_BIT);
		HW_DESC_SET_FLOW_MODE(&desc[idx], DIN_AES_DOUT);
		idx++;
	}

	/* store Additional data mac */
	HW_DESC_INIT(&desc[idx]);
	HW_DESC_SET_CIPHER_MODE(&desc[idx], AEAD_MODE_CCM_A);
	HW_DESC_SET_DOUT_DLLI(&desc[idx], req_ctx->mac_buf_dma_addr, SEP_AES_BLOCK_SIZE, AXI_ID, NS_BIT, 0);
	HW_DESC_SET_FLOW_MODE(&desc[idx], S_AES_to_DOUT);
	HW_DESC_SET_SETUP_MODE(&desc[idx], SETUP_WRITE_STATE0);
	HW_DESC_SET_CIPHER_CONFIG0(&desc[idx], direct);
	idx++;

	/* load payload state */
	/* load ctr state */
	HW_DESC_INIT(&desc[idx]);
	HW_DESC_SET_CIPHER_MODE(&desc[idx], hw_mode);
	HW_DESC_SET_KEY_SIZE_AES(&desc[idx], ctx->enc_keylen);
	HW_DESC_SET_FLOW_MODE(&desc[idx], S_DIN_to_AES);
	HW_DESC_SET_DIN_TYPE(&desc[idx], DMA_DLLI,
			req_ctx->gen_ctx.iv_dma_addr, AES_BLOCK_SIZE, AXI_ID, NS_BIT);
	HW_DESC_SET_CIPHER_CONFIG0(&desc[idx], direct);
	HW_DESC_SET_SETUP_MODE(&desc[idx], SETUP_LOAD_STATE1);
	HW_DESC_SET_CIPHER_CONFIG1(&desc[idx], TUNNEL_ON);
	idx++;

	/* load key */
	HW_DESC_INIT(&desc[idx]);
	HW_DESC_SET_CIPHER_MODE(&desc[idx], hw_mode);
	HW_DESC_SET_DIN_TYPE(&desc[idx], DMA_DLLI, ctx->enckey_dma_addr,
			((ctx->enc_keylen == 24) ? SEP_AES_KEY_SIZE_MAX : ctx->enc_keylen), AXI_ID, NS_BIT);
	HW_DESC_SET_KEY_SIZE_AES(&desc[idx], ctx->enc_keylen);
	HW_DESC_SET_FLOW_MODE(&desc[idx], S_DIN_to_AES);
	HW_DESC_SET_SETUP_MODE(&desc[idx], SETUP_LOAD_KEY0);
	HW_DESC_SET_CIPHER_CONFIG0(&desc[idx], direct);
	HW_DESC_SET_CIPHER_CONFIG1(&desc[idx], TUNNEL_ON);
	idx++;

	HW_DESC_INIT(&desc[idx]);
	HW_DESC_SET_DIN_NO_DMA(&desc[idx], 0, 0xfffff0);
	HW_DESC_SET_DOUT_NO_DMA(&desc[idx], 0, 0, 1);
	idx++;

	/* load mac state */
	HW_DESC_INIT(&desc[idx]);
	HW_DESC_SET_CIPHER_MODE(&desc[idx], hw_mode);
	HW_DESC_SET_KEY_SIZE_AES(&desc[idx], ctx->enc_keylen);
	HW_DESC_SET_FLOW_MODE(&desc[idx], S_DIN_to_AES2);
	HW_DESC_SET_SETUP_MODE(&desc[idx], SETUP_LOAD_STATE0);
	HW_DESC_SET_DIN_TYPE(&desc[idx], DMA_DLLI,
			req_ctx->mac_buf_dma_addr, AES_BLOCK_SIZE, AXI_ID, NS_BIT);
	HW_DESC_SET_CIPHER_CONFIG1(&desc[idx], TUNNEL_ON);
	HW_DESC_SET_CIPHER_CONFIG0(&desc[idx], direct);
	idx++;

	/* load mac key */
	HW_DESC_INIT(&desc[idx]);
	HW_DESC_SET_CIPHER_MODE(&desc[idx], hw_mode);

	HW_DESC_SET_DIN_TYPE(&desc[idx], DMA_DLLI, ctx->enckey_dma_addr,
			((ctx->enc_keylen == 24) ? SEP_AES_KEY_SIZE_MAX : ctx->enc_keylen), AXI_ID, NS_BIT);
	HW_DESC_SET_KEY_SIZE_AES(&desc[idx], ctx->enc_keylen);
	HW_DESC_SET_FLOW_MODE(&desc[idx], S_DIN_to_AES2);
	HW_DESC_SET_SETUP_MODE(&desc[idx], SETUP_LOAD_KEY0);
	HW_DESC_SET_CIPHER_CONFIG0(&desc[idx], direct);
	HW_DESC_SET_CIPHER_CONFIG1(&desc[idx], TUNNEL_ON);
	idx++;
	if (req_ctx->cryptlen != 0) {
		dx_aead_create_cipher_data_desc(req, DIN_AES_DOUT, desc, &idx, mlli_sram_addr, direct);
	}

	HW_DESC_INIT(&desc[idx]);
	HW_DESC_SET_CIPHER_MODE(&desc[idx], hw_mode);
	HW_DESC_SET_DOUT_DLLI(&desc[idx], req_ctx->mac_buf_dma_addr , ctx->authsize, AXI_ID, NS_BIT, 0);
	HW_DESC_SET_SETUP_MODE(&desc[idx], SETUP_WRITE_STATE0);
	HW_DESC_SET_CIPHER_CONFIG1(&desc[idx], TUNNEL_ON);
	HW_DESC_SET_FLOW_MODE(&desc[idx], S_AES2_to_DOUT);
	idx++;

	/* load AES-CTR state */
	HW_DESC_INIT(&desc[idx]);
	HW_DESC_SET_CIPHER_MODE(&desc[idx], SEP_CIPHER_CTR);
	HW_DESC_SET_SETUP_MODE(&desc[idx], SETUP_LOAD_STATE1);
	HW_DESC_SET_KEY_SIZE_AES(&desc[idx], ctx->enc_keylen);
	HW_DESC_SET_CIPHER_CONFIG0(&desc[idx], SEP_CRYPTO_DIRECTION_ENCRYPT);

	HW_DESC_SET_DIN_TYPE(&desc[idx], DMA_DLLI,
			 req_ctx->ccm_iv0_dma_addr , AES_BLOCK_SIZE, AXI_ID, NS_BIT);
	HW_DESC_SET_FLOW_MODE(&desc[idx], S_DIN_to_AES);
	idx++;

	/* load AES-CTR key */
	HW_DESC_INIT(&desc[idx]);
	HW_DESC_SET_CIPHER_MODE(&desc[idx], SEP_CIPHER_CTR);
	HW_DESC_SET_CIPHER_CONFIG0(&desc[idx], SEP_CRYPTO_DIRECTION_ENCRYPT);
	HW_DESC_SET_DIN_TYPE(&desc[idx], DMA_DLLI, ctx->enckey_dma_addr,
			((ctx->enc_keylen == 24) ? SEP_AES_KEY_SIZE_MAX : ctx->enc_keylen), AXI_ID, NS_BIT);
	HW_DESC_SET_KEY_SIZE_AES(&desc[idx], ctx->enc_keylen);
	HW_DESC_SET_FLOW_MODE(&desc[idx], S_DIN_to_AES);
	HW_DESC_SET_SETUP_MODE(&desc[idx], SETUP_LOAD_KEY0);
	idx++;

	HW_DESC_INIT(&desc[idx]);
	HW_DESC_SET_DIN_NO_DMA(&desc[idx], 0, 0xfffff0);
	HW_DESC_SET_DOUT_NO_DMA(&desc[idx], 0, 0, 1);
	idx++;

	/* encrypt the "T" value and store MAC in mac_state */
	HW_DESC_INIT(&desc[idx]);

	HW_DESC_SET_DIN_TYPE(&desc[idx], DMA_DLLI,
			req_ctx->mac_buf_dma_addr , ctx->authsize, AXI_ID, NS_BIT);
	HW_DESC_SET_DOUT_DLLI(&desc[idx], req_ctx->mac_buf_dma_addr , ctx->authsize, AXI_ID, NS_BIT, 1);
	HW_DESC_SET_FLOW_MODE(&desc[idx], DIN_AES_DOUT);
	idx++;

	*seq_size = idx;
	return 0;
}

int config_ccm_adata(struct aead_request *req) {
	struct crypto_aead *tfm = crypto_aead_reqtfm(req);
	struct dx_aead_ctx *ctx = crypto_aead_ctx(tfm);
	struct aead_req_ctx *req_ctx = aead_request_ctx(req);
	//unsigned int size_of_a = 0, rem_a_size = 0;
	unsigned int lp = req->iv[0];
	unsigned int l = lp + 1;
	unsigned int m = ctx->authsize;
	uint8_t *b0 = req_ctx->ccm_config + CCM_B0_OFFSET;
	uint8_t *a0 = req_ctx->ccm_config + CCM_A0_OFFSET;
	uint8_t *ctr_count_0 = req_ctx->ccm_config + CCM_CTR_COUNT_0_OFFSET;
	unsigned int cryptlen = (req_ctx->gen_ctx.op_type ==
				 SEP_CRYPTO_DIRECTION_ENCRYPT) ?
				req->cryptlen :
				(req->cryptlen - ctx->authsize);
	int rc;
	memset(req_ctx->mac_buf, 0, AES_BLOCK_SIZE);
	memset(req_ctx->ccm_config, 0, AES_BLOCK_SIZE*3);
	/*save original mac to prevent possible data overriding coused by cache coherence problem*/
	if (req_ctx->gen_ctx.op_type) {
		dx_sg_copy_part(req_ctx->backup_mac, req->src,
			     req->cryptlen - ctx->authsize,
			     req->cryptlen, DX_SG_TO_BUF);
	}
	/* taken from crypto/ccm.c */
	/* 2 <= L <= 8, so 1 <= L' <= 7. */
	if (1 > req->iv[0] || req->iv[0] > 7) {
		DX_LOG_ERR("illegal iv value %X\n",req->iv[0]);
		return -EINVAL;
	}
	memcpy(b0, req->iv, AES_BLOCK_SIZE);

	/* format control info per RFC 3610 and
	 * NIST Special Publication 800-38C
	 */
	*b0 |= (8 * ((m - 2) / 2));
	if (req->assoclen)
		*b0 |= 64;

	rc = set_msg_len(b0 + 16 - l, cryptlen, l);
	if (rc != 0) {
		return rc;
	}
	 /* END of "taken from crypto/ccm.c" */

	req_ctx->ccm_hdr_size = format_ccm_a0 (a0, req->assoclen);

	memset(req->iv + 15 - req->iv[0], 0, req->iv[0] + 1);
	req->iv [15] = 1;

	memcpy(ctr_count_0, req->iv, AES_BLOCK_SIZE) ;
	ctr_count_0[15] = 0;

	return 0;
}

static void dx_rfc4309_ccm_process(struct aead_request *req)
{

	struct crypto_aead *tfm = crypto_aead_reqtfm(req);
	struct dx_aead_ctx *ctx = crypto_aead_ctx(tfm);
	struct aead_req_ctx *areq_ctx = aead_request_ctx(req);

	/* L' */
	memset(areq_ctx->ctr_iv, 0, AES_BLOCK_SIZE);
	areq_ctx->ctr_iv[0] = 3;

	memcpy(areq_ctx->ctr_iv + 1, ctx->ctr_nonce, 3);
	memcpy(areq_ctx->ctr_iv + 4, req->iv, 8);
	req->iv = areq_ctx->ctr_iv;
}

static int dx_aead_process(struct aead_request *req, enum sep_crypto_direction direct)
{
	int rc = 0;
	int seq_len = 0;
	HwDesc_s desc[MAX_AEAD_PROCESS_SEQ];
	struct crypto_aead *tfm = crypto_aead_reqtfm(req);
	struct dx_aead_ctx *ctx = crypto_aead_ctx(tfm);
	struct aead_req_ctx *areq_ctx = aead_request_ctx(req);
	struct device *dev = &crypto_drvdata->plat_dev->dev;
	struct dx_crypto_req dx_req = {0};

	DECL_CYCLE_COUNT_RESOURCES;

	DX_LOG_DEBUG("%s context=%p req=%p iv=%p src=%p src_ofs=%d dst=%p dst_ofs=%d cryptolen=%d\n",
		((direct==SEP_CRYPTO_DIRECTION_ENCRYPT)?"Encrypt":"Decrypt"), ctx, req, req->iv,
		sg_virt(req->src), req->src->offset, sg_virt(req->dst), req->dst->offset, req->cryptlen);

	/* STAT_PHASE_0: Init and sanity checks */
	START_CYCLE_COUNT();

	/* Check data length according to mode */
	if (unlikely(validate_data_size(
		ctx, direct, req->cryptlen, req->assoclen))) {
		DX_LOG_ERR("Unsupported crypt/assoc len %d/%d.\n",
				req->cryptlen, req->assoclen);
		crypto_aead_set_flags(tfm, CRYPTO_TFM_RES_BAD_BLOCK_LEN);
		return -EINVAL;
	}

	/* Setup DX request structure */
	dx_req.user_cb = (void *)dx_aead_complete;
	dx_req.user_arg = (void *)req;

#ifdef ENABLE_CYCLE_COUNT
	dx_req.op_type = direct;
#endif
	/* Setup request context */
	areq_ctx->gen_ctx.op_type = direct;

	areq_ctx->req_authsize = ctx->authsize;

	END_CYCLE_COUNT(direct, STAT_PHASE_0);

	/* STAT_PHASE_1: Map buffers */
	START_CYCLE_COUNT();

	if (ctx->cipher_mode == SEP_CIPHER_CTR) {
		/* Build CTR IV - Copy nonce from last 4 bytes in
		*  CTR key to first 4 bytes in CTR IV */
		memcpy(areq_ctx->ctr_iv, ctx->ctr_nonce, CTR_RFC3686_NONCE_SIZE);
		if (areq_ctx->backup_giv == NULL) /*User none-generated IV*/
			memcpy(areq_ctx->ctr_iv + CTR_RFC3686_NONCE_SIZE,
				req->iv, CTR_RFC3686_IV_SIZE);
		/* Initialize counter portion of counter block */
		*(__be32 *)(areq_ctx->ctr_iv + CTR_RFC3686_NONCE_SIZE +
			    CTR_RFC3686_IV_SIZE) = cpu_to_be32(1);

		/* Replace with counter iv */
		req->iv = areq_ctx->ctr_iv;
		areq_ctx->hw_iv_size = CTR_RFC3686_BLOCK_SIZE;
	 } else if (ctx->cipher_mode == SEP_CIPHER_CCM) {
		areq_ctx->hw_iv_size = AES_BLOCK_SIZE;
		if (areq_ctx->ctr_iv != req->iv) {
			memcpy(areq_ctx->ctr_iv , req->iv, AES_BLOCK_SIZE);
			req->iv = areq_ctx->ctr_iv;
		}

	}  else {
		areq_ctx->hw_iv_size = crypto_aead_ivsize(tfm);
	}

	if (ctx->cipher_mode == SEP_CIPHER_CCM) {
		rc = config_ccm_adata(req);
        if (rc != 0) {
           goto exit;
       }
   } else {
		areq_ctx->ccm_hdr_size = ccm_header_size_null;
	}
	rc = map_aead_request(dev, req);
	if (unlikely(rc)) {
		DX_LOG_ERR("map_request() failed\n");
		goto exit;
	}

	/* do we need to generate IV? */
	if (areq_ctx->backup_giv != NULL) {
		dx_req.ivgen_dma_addr = areq_ctx->gen_ctx.iv_dma_addr;
		/* set the DMA mapped IV address*/
		if (ctx->cipher_mode == SEP_CIPHER_CTR)
			dx_req.ivgen_dma_addr += CTR_RFC3686_NONCE_SIZE;

		/* set the IV size (8/16 B long)*/
		dx_req.ivgen_size = crypto_aead_ivsize(tfm);
	}

	END_CYCLE_COUNT(direct, STAT_PHASE_1);

	/* STAT_PHASE_2: Create sequence */
	START_CYCLE_COUNT();

	/*TODO: move seq len by reference */
	switch (ctx->auth_mode) {
	case SEP_HASH_SHA1:
	case SEP_HASH_SHA256:
		dx_aead_any_cipher_to_hmac_authenc(req, desc, &seq_len);
		break;
	case SEP_HASH_XCBC_MAC:
		dx_aead_any_cipher_to_xcbc_authenc(req, desc, &seq_len);
		break;
	case SEP_HASH_NULL:
		if (ctx->cipher_mode == SEP_CIPHER_CCM) {

			dx_aead_ccm(req, desc, &seq_len);
			break;
		}

	default:
		DX_LOG_ERR("Unsupported authenc (%d)\n", ctx->auth_mode);
		unmap_aead_request(dev, req);
		rc = -ENOTSUPP;
		goto exit;
	}

	END_CYCLE_COUNT(direct, STAT_PHASE_2);

	/* STAT_PHASE_3: Lock HW and push sequence */
	START_CYCLE_COUNT();

	rc = send_request(crypto_drvdata, &dx_req, desc, seq_len, 1);
	if (unlikely(rc != -EINPROGRESS)) {
		DX_LOG_ERR("send_request() failed (rc=%d)\n", rc);
		unmap_aead_request(dev, req);
	}

	END_CYCLE_COUNT(direct, STAT_PHASE_3);
exit:
	return rc;
}

static int dx_aead_encrypt(struct aead_request *req)
{
	struct aead_req_ctx *areq_ctx = aead_request_ctx(req);

	/* No generated IV required */
	areq_ctx->backup_iv = req->iv;
	areq_ctx->backup_giv = NULL;

	return dx_aead_process(req, SEP_CRYPTO_DIRECTION_ENCRYPT);
}


static int dx_rfc4309_ccm_encrypt(struct aead_request *req)
{
	struct aead_req_ctx *areq_ctx = aead_request_ctx(req);
	/* No generated IV required */
	areq_ctx->backup_iv = req->iv;
	areq_ctx->backup_giv = NULL;
	dx_rfc4309_ccm_process(req);
	return dx_aead_process(req, SEP_CRYPTO_DIRECTION_ENCRYPT);
}

static int dx_rfc4309_ccm_decrypt(struct aead_request *req)
{
	struct aead_req_ctx *areq_ctx = aead_request_ctx(req);
	/* No generated IV required */
	areq_ctx->backup_iv = req->iv;
	areq_ctx->backup_giv = NULL;
	dx_rfc4309_ccm_process(req);
	return dx_aead_process(req, SEP_CRYPTO_DIRECTION_DECRYPT);
}

static int dx_aead_decrypt(struct aead_request *req)
{
	struct aead_req_ctx *areq_ctx = aead_request_ctx(req);

	/* No generated IV required */
	areq_ctx->backup_iv = req->iv;
	areq_ctx->backup_giv = NULL;

	return dx_aead_process(req, SEP_CRYPTO_DIRECTION_DECRYPT);
}

static int dx_aead_givencrypt(struct aead_givcrypt_request *req)
{
	struct aead_req_ctx *areq_ctx = aead_request_ctx(&req->areq);

	/* Backup orig. IV */
	areq_ctx->backup_iv = req->areq.iv;
	/* Backup orig. "giv". This indicates that we need to generate IV */
	areq_ctx->backup_giv = req->giv;
	/* request IV points to payload's header */
	req->areq.iv = req->giv;

	return dx_aead_process(&req->areq, SEP_CRYPTO_DIRECTION_ENCRYPT);
}

/* DX Block aead alg */
static struct dx_alg_template aead_algs[] = {
	{
		.name = "authenc(hmac(sha1),cbc(aes))",
		.driver_name = "authenc-hmac-sha1-cbc-aes-dx",
		.blocksize = AES_BLOCK_SIZE,
		.type = CRYPTO_ALG_TYPE_AEAD,
		.template_aead = {
			.setkey = dx_aead_setkey,
			.setauthsize = dx_aead_setauthsize,
			.encrypt = dx_aead_encrypt,
			.decrypt = dx_aead_decrypt,
			.givencrypt = dx_aead_givencrypt,
			/* defaulted to "<built-in>" */
			.ivsize = AES_BLOCK_SIZE,
			.maxauthsize = SHA1_DIGEST_SIZE,
		},
		.cipher_mode = SEP_CIPHER_CBC,
		.flow_mode = S_DIN_to_AES,
		.auth_mode = SEP_HASH_SHA1,
	},
	{
		.name = "authenc(hmac(sha1),cbc(des3_ede))",
		.driver_name = "authenc-hmac-sha1-cbc-des3-dx",
		.blocksize = DES3_EDE_BLOCK_SIZE,
		.type = CRYPTO_ALG_TYPE_AEAD,
		.template_aead = {
			.setkey = dx_aead_setkey,
			.setauthsize = dx_aead_setauthsize,
			.encrypt = dx_aead_encrypt,
			.decrypt = dx_aead_decrypt,
			.givencrypt = dx_aead_givencrypt,
			/* defaulted to "<built-in>" */
			.ivsize = DES3_EDE_BLOCK_SIZE,
			.maxauthsize = SHA1_DIGEST_SIZE,
		},
		.cipher_mode = SEP_CIPHER_CBC,
		.flow_mode = S_DIN_to_DES,
		.auth_mode = SEP_HASH_SHA1,
	},
	{
		.name = "authenc(hmac(sha256),cbc(aes))",
		.driver_name = "authenc-hmac-sha256-cbc-aes-dx",
		.blocksize = AES_BLOCK_SIZE,
		.type = CRYPTO_ALG_TYPE_AEAD,
		.template_aead = {
			.setkey = dx_aead_setkey,
			.setauthsize = dx_aead_setauthsize,
			.encrypt = dx_aead_encrypt,
			.decrypt = dx_aead_decrypt,
			.givencrypt = dx_aead_givencrypt,
			/* defaulted to "<built-in>" */
			.ivsize = AES_BLOCK_SIZE,
			.maxauthsize = SHA256_DIGEST_SIZE,
		},
		.cipher_mode = SEP_CIPHER_CBC,
		.flow_mode = S_DIN_to_AES,
		.auth_mode = SEP_HASH_SHA256,
	},
	{
		.name = "authenc(hmac(sha256),cbc(des3_ede))",
		.driver_name = "authenc-hmac-sha256-cbc-des3-dx",
		.blocksize = DES3_EDE_BLOCK_SIZE,
		.type = CRYPTO_ALG_TYPE_AEAD,
		.template_aead = {
			.setkey = dx_aead_setkey,
			.setauthsize = dx_aead_setauthsize,
			.encrypt = dx_aead_encrypt,
			.decrypt = dx_aead_decrypt,
			.givencrypt = dx_aead_givencrypt,
			/* defaulted to "<built-in>" */
			.ivsize = DES3_EDE_BLOCK_SIZE,
			.maxauthsize = SHA256_DIGEST_SIZE,
		},
		.cipher_mode = SEP_CIPHER_CBC,
		.flow_mode = S_DIN_to_DES,
		.auth_mode = SEP_HASH_SHA256,
	},
	{
		.name = "authenc(xcbc(aes),cbc(aes))",
		.driver_name = "authenc-xcbc-aes-cbc-aes-dx",
		.blocksize = AES_BLOCK_SIZE,
		.type = CRYPTO_ALG_TYPE_AEAD,
		.template_aead = {
			.setkey = dx_aead_setkey,
			.setauthsize = dx_aead_setauthsize,
			.encrypt = dx_aead_encrypt,
			.decrypt = dx_aead_decrypt,
			.givencrypt = dx_aead_givencrypt,
			/* defaulted to "<built-in>" */
			.ivsize = AES_BLOCK_SIZE,
			.maxauthsize = AES_BLOCK_SIZE,
		},
		.cipher_mode = SEP_CIPHER_CBC,
		.flow_mode = S_DIN_to_AES,
		.auth_mode = SEP_HASH_XCBC_MAC,
	},
	{
		.name = "authenc(hmac(sha1),rfc3686(ctr(aes)))",
		.driver_name = "authenc-hmac-sha1-rfc3686-ctr-aes-dx",
		.blocksize = 1,
		.type = CRYPTO_ALG_TYPE_AEAD,
		.template_aead = {
			.setkey = dx_aead_setkey,
			.setauthsize = dx_aead_setauthsize,
			.encrypt = dx_aead_encrypt,
			.decrypt = dx_aead_decrypt,
			.givencrypt = dx_aead_givencrypt,
			.ivsize = CTR_RFC3686_IV_SIZE,
			.maxauthsize = SHA1_DIGEST_SIZE,
		},
		.cipher_mode = SEP_CIPHER_CTR,
		.flow_mode = S_DIN_to_AES,
		.auth_mode = SEP_HASH_SHA1,
	},
	{
		.name = "authenc(hmac(sha256),rfc3686(ctr(aes)))",
		.driver_name = "authenc-hmac-sha256-rfc3686-ctr-aes-dx",
		.blocksize = 1,
		.type = CRYPTO_ALG_TYPE_AEAD,
		.template_aead = {
			.setkey = dx_aead_setkey,
			.setauthsize = dx_aead_setauthsize,
			.encrypt = dx_aead_encrypt,
			.decrypt = dx_aead_decrypt,
			.givencrypt = dx_aead_givencrypt,
			.ivsize = CTR_RFC3686_IV_SIZE,
			.maxauthsize = SHA256_DIGEST_SIZE,
		},
		.cipher_mode = SEP_CIPHER_CTR,
		.flow_mode = S_DIN_to_AES,
		.auth_mode = SEP_HASH_SHA256,
	},
	{
		.name = "authenc(xcbc(aes),rfc3686(ctr(aes)))",
		.driver_name = "authenc-xcbc-aes-rfc3686-ctr-aes-dx",
		.blocksize = 1,
		.type = CRYPTO_ALG_TYPE_AEAD,
		.template_aead = {
			.setkey = dx_aead_setkey,
			.setauthsize = dx_aead_setauthsize,
			.encrypt = dx_aead_encrypt,
			.decrypt = dx_aead_decrypt,
			.givencrypt = dx_aead_givencrypt,
			.ivsize = CTR_RFC3686_IV_SIZE,
			.maxauthsize = AES_BLOCK_SIZE,
		},
		.cipher_mode = SEP_CIPHER_CTR,
		.flow_mode = S_DIN_to_AES,
		.auth_mode = SEP_HASH_XCBC_MAC,
	},
	{
		.name = "ccm(aes)",
		.driver_name = "ccm-aes-dx",
		.blocksize = 1,
		.type = CRYPTO_ALG_TYPE_AEAD,
		.template_aead = {
			.setkey = dx_aead_setkey,
			.setauthsize = dx_ccm_setauthsize,
			.encrypt = dx_aead_encrypt,
			.decrypt = dx_aead_decrypt,
			/*.givencrypt = dx_aead_givencrypt, - Not supported for ccm */
			.ivsize = AES_BLOCK_SIZE,
			.maxauthsize = AES_BLOCK_SIZE,
		},
		.cipher_mode = SEP_CIPHER_CCM,
		.flow_mode = S_DIN_to_AES,
		.auth_mode = SEP_HASH_NULL,
	},
	{
		.name = "rfc4309(ccm(aes))",
		.driver_name = "rfc4309-ccm-aes-dx",
		.blocksize = 1,
		.type = CRYPTO_ALG_TYPE_AEAD,
		.template_aead = {
			.setkey = dx_rfc4309_ccm_setkey,
			.setauthsize = dx_rfc4309_ccm_setauthsize,
			.encrypt = dx_rfc4309_ccm_encrypt,
			.decrypt = dx_rfc4309_ccm_decrypt,
			/*.givencrypt = dx_aead_givencrypt, - Not supported for ccm */
			.ivsize = 8,
			.maxauthsize = AES_BLOCK_SIZE,
		},
		.cipher_mode = SEP_CIPHER_CCM,
		.flow_mode = S_DIN_to_AES,
		.auth_mode = SEP_HASH_NULL,
	},

};

static struct dx_crypto_alg *dx_aead_create_alg(struct dx_alg_template *template)
{
	struct dx_crypto_alg *t_alg;
	struct crypto_alg *alg;

	t_alg = kzalloc(sizeof(struct dx_crypto_alg), GFP_KERNEL);
	if (!t_alg) {
		DX_LOG_ERR("failed to allocate t_alg\n");
		return ERR_PTR(-ENOMEM);
	}

	alg = &t_alg->crypto_alg;

	snprintf(alg->cra_name, CRYPTO_MAX_ALG_NAME, "%s", template->name);
	snprintf(alg->cra_driver_name, CRYPTO_MAX_ALG_NAME, "%s",
		 template->driver_name);
	alg->cra_module = THIS_MODULE;
	alg->cra_priority = DX_CRA_PRIO;
	alg->cra_blocksize = template->blocksize;
	alg->cra_alignmask = 0;
	alg->cra_ctxsize = sizeof(struct dx_aead_ctx);
	alg->cra_flags = CRYPTO_ALG_ASYNC | CRYPTO_ALG_KERN_DRIVER_ONLY |
			 template->type;
	alg->cra_init = dx_aead_init;
	alg->cra_exit = dx_aead_exit;
	alg->cra_type = &crypto_aead_type;
	alg->cra_aead = template->template_aead;

	t_alg->cipher_mode = template->cipher_mode;
	t_alg->flow_mode = template->flow_mode;
	t_alg->auth_mode = template->auth_mode;

	return t_alg;
}

int dx_aead_alloc(struct dx_drvdata *drvdata)
{
	struct device *dev;
	int alg;
	struct dx_crypto_alg *t_alg;
	int rc = -ENOMEM;
	struct dx_aead_handle *aead_handle;

	crypto_drvdata = drvdata;
	dev = &crypto_drvdata->plat_dev->dev;

	aead_handle = kmalloc(sizeof(struct dx_aead_handle), GFP_KERNEL);
	if (aead_handle == NULL) {
		rc = -ENOMEM;
		goto fail0;
	}
	drvdata->aead_handle = aead_handle;

	aead_handle->sram_workspace_addr = (uint32_t)dx_sram_mgr_alloc(
			drvdata, MAX_HMAC_DIGEST_SIZE);
	if (!aead_handle->sram_workspace_addr) {
		DX_LOG_ERR("SRAM pool exhausted\n");
		rc = -ENOMEM;
		goto fail1;
	}

	INIT_LIST_HEAD(&aead_handle->aead_list);

	/* Linux crypto */
	for (alg = 0; alg < ARRAY_SIZE(aead_algs); alg++) {
		t_alg = dx_aead_create_alg(&aead_algs[alg]);
		if (IS_ERR(t_alg)) {
			rc = PTR_ERR(t_alg);
			DX_LOG_ERR("%s alg allocation failed\n",
				 aead_algs[alg].driver_name);
			goto fail1;
		}

		rc = crypto_register_alg(&t_alg->crypto_alg);
		if (rc) {
			DX_LOG_ERR("%s alg registration failed\n",
				t_alg->crypto_alg.cra_driver_name);
			goto fail2;
		} else {
			list_add_tail(&t_alg->entry, &aead_handle->aead_list);
			DX_LOG_DEBUG("Registered %s\n", t_alg->crypto_alg.cra_driver_name);
		}
	}
	drvdata->aead_handle = aead_handle;
	return 0;

fail2:
	kfree(t_alg);
fail1:
	kfree(aead_handle);
fail0:
	return rc;
}

int dx_aead_free(struct dx_drvdata *drvdata)
{
	struct dx_crypto_alg *t_alg, *n;
	struct dx_aead_handle *aead_handle = (struct dx_aead_handle*)drvdata->aead_handle;

	if (aead_handle != 0) {
		/* Remove registered algs */
		list_for_each_entry_safe(t_alg, n, &aead_handle->aead_list, entry) {
			crypto_unregister_alg(&t_alg->crypto_alg);
			list_del(&t_alg->entry);
			kfree(t_alg);
		}
		kfree(aead_handle);
		drvdata->aead_handle = NULL;
	}

	return 0;
}
