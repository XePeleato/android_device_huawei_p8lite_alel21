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

/* \file dx_driver.h
   Discretix Linux Crypto Driver
 */

#ifndef __DX_DRIVER_H__
#define __DX_DRIVER_H__

#include "dx_config.h"
#ifdef COMP_IN_WQ
#include <linux/workqueue.h>
#else
#include <linux/interrupt.h>
#endif
#include <linux/dma-mapping.h>
#include <crypto/algapi.h>
#include <crypto/internal/skcipher.h>
#include <crypto/aes.h>
#include <crypto/sha.h>
#include <crypto/aead.h>
#include <crypto/authenc.h>
#include <crypto/hash.h>

#ifndef INT32_MAX /* Missing in Linux kernel */
#define INT32_MAX 0x7FFFFFFFL
#endif

/* Registers definitions from shared/hw/include */
#include "dx_reg_base_host.h"
#include "dx_host.h"
#ifdef DX_FPGA_ENV
#include "dx_env.h"
#endif
#define DX_CC_HOST_VIRT /* must be defined before including dx_cc_regs.h */
#include "dx_cc_regs.h"
#include "dx_reg_common.h"
#include "dx_hal.h"
#include "hw_queue_defs.h"
#define SEP_SUPPORT_SHA 256
#include "sep_ctx.h"
#include "dx_sysfs.h"
#include "hash_defs.h"
#include "secure_key_defs.h"
#include "secure_key_int_defs.h"

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

#define DX_AXI_IRQ_MASK ((1 << DX_AXIM_CFG1_BRESPMASK_BIT_SHIFT) | (1 << DX_AXIM_CFG1_RRESPMASK_BIT_SHIFT) |	\
			(1 << DX_AXIM_CFG1_INFLTMASK_BIT_SHIFT) | (1 << DX_AXIM_CFG1_COMPMASK_BIT_SHIFT))

#define DX_AXI_ERR_IRQ_MASK ((1 << DX_HOST_IRR_AXI_ERR_INT_BIT_SHIFT) | \
			(1 << DX_HOST_IRR_SEP_HOST_GPR5_INT_BIT_SHIFT))

#if (DX_DEV_SIGNATURE == DX_CC441P_SIG)
#define DX_COMP_IRQ_MASK DX_AXI_ERR_IRQ_MASK
#else /* DX_CC442P_SIG */
#define DX_COMP_IRQ_MASK (1 << DX_HOST_IRR_AXIM_COMP_INT_BIT_SHIFT)
#endif

#define DX_GPR6_IRQ_MASK (1 << DX_HOST_IRR_SEP_HOST_GPR6_INT_BIT_SHIFT)
#define REG_SPACE_SIZE 0x10000 /* 64KB for ENV. resource that is missing from the device tree */

#define DX_CRA_PRIO 3000
#define XTS_DATA_UNIT_SIZE 512

#define MAX_KEY_BUF_SIZE (DX_SECURE_KEY_PACKAGE_BUF_SIZE_IN_BYTES)

#define HW_QUEUE_SIZE 819

#define MAX_REQUEST_QUEUE_SIZE 4096
#define MAX_MLLI_BUFF_SIZE 2080

/* Definitions for HW descriptors DIN/DOUT fields */
#define NS_BIT 1
#define AXI_ID 0
/* AXI_ID is not actually the AXI ID of the transaction but the value of AXI_ID
   field in the HW descriptor. The DMA engine +8 that value. */

/* Logging macros */
#define DX_LOG(level, format, ...) \
	printk(level "cc44p::%s: " format , __func__, ##__VA_ARGS__)
#define DX_LOG_ERR(format, ...) DX_LOG(KERN_ERR, format, ##__VA_ARGS__)
#define DX_LOG_WARNING(format, ...) DX_LOG(KERN_WARNING, format, ##__VA_ARGS__)
#define DX_LOG_NOTICE(format, ...) DX_LOG(KERN_NOTICE, format, ##__VA_ARGS__)
#define DX_LOG_INFO(format, ...) DX_LOG(KERN_INFO, format, ##__VA_ARGS__)
#ifdef DX_DEBUG
#define DX_LOG_DEBUG(format, ...) DX_LOG(KERN_DEBUG, format, ##__VA_ARGS__)
#else /* Debug log messages are removed at compile time for non-DEBUG config. */
#define DX_LOG_DEBUG(format, ...) do {} while (0)
#endif

struct dx_crypto_req {
	void (*user_cb)(struct device *dev, void *req);
	void *user_arg;
	dma_addr_t ivgen_dma_addr; /* If not NULL -IV generation required
				during send_request */
	unsigned int ivgen_size; /* The generated IV size required, 8/16 B allowed. */
	struct completion seq_compl; /* request completion */
#ifdef ENABLE_CYCLE_COUNT
	enum stat_op op_type;
	cycles_t submit_cycle;
#endif
};

/**
 * struct dx_drvdata - SeP driver private data context
 * @mem_start:	phys. address of the control registers
 * @mem_end:	phys. address of the control registers
 * @mem_size:	Control registers memory range size (mem_end - mem_start)
 * @cc_base:	virt address of the CC registers
 * @irq:	device IRQ number
 * @irq_mask:	Interrupt mask shadow (1 for masked interrupts)
 * @rom_ver:	SeP ROM version
 * @fw_ver:	SeP loaded firmware version
 * @devt_base:	Allocated char.dev. major/minor (with alloc_chrdev_region)
 * @dev:	Device context
 */
struct dx_drvdata {
	struct resource *res_mem;
	struct resource *res_irq;
	resource_size_t mem_start;
	resource_size_t mem_end;
	resource_size_t mem_size;
	void __iomem *cc_base;
#ifdef DX_BASE_ENV_REGS
	void __iomem *env_base; /* Discretix development FPGAs only */
#endif
	unsigned int irq;
	uint32_t irq_mask;
	uint32_t rom_ver;
	uint32_t fw_ver;
	dev_t devt_base;
	struct device *dev;
	struct platform_device *plat_dev;
	uint32_t *mlli_sram_addr;
	struct completion icache_setup_completion;
	void *buff_mgr_handle;
	void *ahash_handle;
	void *aead_handle;
	void *ablckcipher_handle;
	void *request_mgr_handle;
	void *ivgen_handle;
	void *sram_mgr_handle;
	struct clk	*clk;

#ifdef ENABLE_CYCLE_COUNT
	cycles_t isr_exit_cycles; /* Save for isr-to-tasklet latency */
#endif
};

struct dx_crypto_alg {
	struct list_head entry;
	int cipher_mode;
	int flow_mode; /* Note: currently, refers to the cipher mode only. */
	int auth_mode;
	int is_secure_key;
	struct crypto_alg crypto_alg;
};

struct dx_alg_template {
	char name[CRYPTO_MAX_ALG_NAME];
	char driver_name[CRYPTO_MAX_ALG_NAME];
	unsigned int blocksize;
	u32 type;
	union {
		struct ablkcipher_alg ablkcipher;
		struct aead_alg aead;
		struct blkcipher_alg blkcipher;
		struct cipher_alg cipher;
		struct compress_alg compress;
		struct rng_alg rng;
	} template_u;
	int cipher_mode;
	int flow_mode; /* Note: currently, refers to the cipher mode only. */
	int auth_mode;
	int is_secure_key;
};

struct async_gen_req_ctx {
	dma_addr_t iv_dma_addr;
	enum sep_crypto_direction op_type;
};

#ifdef DX_DUMP_BYTES
void dump_byte_array(const char *name, const uint8_t *the_array, unsigned long size);
#else
#define dump_byte_array(name, array, size) do {	\
} while (0);
#endif

#ifdef ENABLE_CYCLE_COUNT
#define DECL_CYCLE_COUNT_RESOURCES cycles_t _last_cycles_read
#define START_CYCLE_COUNT() do { _last_cycles_read = get_cycles(); } while (0)
#define END_CYCLE_COUNT(_stat_op_type, _stat_phase) update_stat(_stat_op_type, _stat_phase, get_cycles() - _last_cycles_read)
#define GET_START_CYCLE_COUNT() _last_cycles_read
#define START_CYCLE_COUNT_AT(_var) do { _var = get_cycles(); } while(0)
#define END_CYCLE_COUNT_AT(_var, _stat_op_type, _stat_phase) update_stat(_stat_op_type, _stat_phase, get_cycles() - _var)
#else
#define DECL_CYCLE_COUNT_RESOURCES
#define START_CYCLE_COUNT() do { } while (0)
#define END_CYCLE_COUNT(_stat_op_type, _stat_phase) do { } while (0)
#define GET_START_CYCLE_COUNT() 0
#define START_CYCLE_COUNT_AT(_var) do { } while (0)
#define END_CYCLE_COUNT_AT(_var, _stat_op_type, _stat_phase) do { } while (0)
#endif /*ENABLE_CYCLE_COUNT*/
#endif /*__DX_DRIVER_H__*/
