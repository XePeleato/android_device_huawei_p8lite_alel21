
/**************************************************************************
 *   Copyright 2009 © Discretix Technologies Ltd. This software is         *
 *   protected by copyright, international treaties and various patents.   *
 *   Any copy or reproduction of this Software as permitted below, must    *
 *   include this Copyright Notice as well as any other notices provided   *
 *   under such license.                                                   *
 *                                                                         *
 *   This program shall be governed by, and may be used and redistributed  *
 *   under the terms and conditions of the GNU Lesser General Public       *
 *   License, version 2.1, as published by the Free Software Foundation.   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY liability and WARRANTY; without even the implied      *
 *   warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.      *
 *   See the GNU General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this program; if not, please write to the          *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/


#ifndef _CRYPTO_DRIVER_DEFS_H
#define _CRYPTO_DRIVER_DEFS_H

#ifdef __cplusplus
extern "C"
{
#endif

#define DX_CC_HOST_VIRT /* must be defined before including dx_cc_regs.h */
#include "hw_queue_defs.h"
#include "completion_plat.h"
#include "sep_ctx.h"

/******************************************************************************
*                        	DEFINITIONS
******************************************************************************/

/* Overlap the PKA SRAM area  (PKA is not activated during sbrom) */
#define SRAM_BASE_ADDRESS		0
/* Use constant counter ID and AXI ID */
#define SB_COUNTER_ID	0
#define SB_AXI_ID	0


/* The AES block size in words and in bytes */
#define AES_BLOCK_SIZE_IN_WORDS 4
#define AES_BLOCK_SIZE_IN_BYTES  (AES_BLOCK_SIZE_IN_WORDS * sizeof(DxUint32_t))

/* The size of the IV or counter buffer */
#define AES_IV_COUNTER_SIZE_IN_WORDS   AES_BLOCK_SIZE_IN_WORDS
#define AES_IV_COUNTER_SIZE_IN_BYTES  (AES_IV_COUNTER_SIZE_IN_WORDS * sizeof(DxUint32_t))

/* The size of the AES KEY in words and bytes */
#define AES_KEY_SIZE_IN_WORDS AES_BLOCK_SIZE_IN_WORDS
#define AES_KEY_SIZE_IN_BYTES (AES_KEY_SIZE_IN_WORDS * sizeof(DxUint32_t))

#define AES_Key128Bits_SIZE_IN_WORDS	AES_BLOCK_SIZE_IN_WORDS
#define AES_Key128Bits_SIZE_IN_BYTES	AES_BLOCK_SIZE_IN_BYTES
#define AES_Key256Bits_SIZE_IN_WORDS	8
#define AES_Key256Bits_SIZE_IN_BYTES	(AES_Key256Bits_SIZE_IN_WORDS * sizeof(DxUint32_t))


/* Hash IV+Length */
#define HASH_DIGEST_SIZE_IN_WORDS	8
#define HASH_DIGEST_SIZE_IN_BYTES	(HASH_DIGEST_SIZE_IN_WORDS * sizeof(DxUint32_t))
#define HASH_LENGTH_SIZE_IN_WORDS	4
#define HASH_LENGTH_SIZE_IN_BYTES	(HASH_LENGTH_SIZE_IN_WORDS * sizeof(DxUint32_t))


/******************************************************************************
*				TYPE DEFINITIONS
******************************************************************************/

/* Defines the IV counter buffer  - 16 bytes array */
typedef DxUint32_t AES_Iv_t[AES_IV_COUNTER_SIZE_IN_WORDS];

/* Defines the AES key buffer */
typedef DxUint32_t AES_Key_t[AES_KEY_SIZE_IN_WORDS];

/* Defines the AES CMAC output result */
typedef DxUint8_t AES_CMAC_RESULT_t[AES_BLOCK_SIZE_IN_BYTES];


typedef enum {
	CRYPTO_DRIVER_HASH_MODE  	= 0,
	CRYPTO_DRIVER_AES_CTR_MODE	= 1,
	CRYPTO_DRIVER_HASH_AES_CTR_MODE	= 2,
}CryptoDriverMode_t;


/******************************************************************************
*				 HW engines related definitions
******************************************************************************/
enum HashConfig1Padding {
	HASH_PADDING_DISABLED = 0,
	HASH_PADDING_ENABLED = 1,
	HASH_DIGEST_RESULT_LITTLE_ENDIAN = 2,
	HASH_CONFIG1_PADDING_RESERVE32 = INT32_MAX,
};

#define SESSION_KEY XOR_HDCP_KEY	// overlap in HwCryptoKey_t

/*----------------------------
      MACROS
-----------------------------------*/

#define WAIT_SRAM_DATA_READY()										\
	do { 												\
		unsigned int  regVal; 									\
		do  { 											\
			SYS_ReadRegister( SB_REG_ADDR(hwBaseAddress, SRAM_DATA_READY), regVal);		\
		}while(!(regVal & 0x1)); 								\
	} while(0);


#ifdef __cplusplus
}
#endif

#endif



