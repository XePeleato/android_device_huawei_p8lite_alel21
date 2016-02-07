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

#include <linux/crypto.h>
#include <crypto/algapi.h>
#include <crypto/aes.h>
#include <crypto/sha.h>
#include <crypto/aead.h>
#include <crypto/authenc.h>
#include <crypto/scatterwalk.h>
#include <crypto/internal/skcipher.h>

#include <linux/init.h>
#include <linux/moduleparam.h>
#include <linux/types.h>
#include <linux/random.h>
#include <linux/ioport.h>
#include <linux/interrupt.h>
#include <linux/fcntl.h>
#include <linux/poll.h>
#include <linux/proc_fs.h>
#include <linux/mutex.h>
#include <linux/sysctl.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/platform_device.h>
#include <linux/mm.h>
#include <linux/delay.h>
#include <linux/dma-mapping.h>
#include <linux/dmapool.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/pm.h>

/* cache.h required for L1_CACHE_ALIGN() and cache_line_size() */
#include <linux/cache.h>
#include <asm/byteorder.h>
#include <linux/io.h>
#include <linux/uaccess.h>
#include <asm/timex.h>
#include <asm/system.h>
#include <asm/cacheflush.h>
#include <linux/pagemap.h>
#include <linux/sched.h>
#include <linux/random.h>
#include <linux/of.h>
#include <linux/clk.h>
#include <linux/of_address.h>

#include "dx_config.h"
#include "dx_driver.h"
#include "dx_request_mgr.h"
#include "dx_buffer_mgr.h"
#include "dx_sysfs.h"
#include "dx_cipher.h"
#include "dx_aead.h"
#include "dx_hash.h"
#include "dx_ivgen.h"
#include "dx_sram_mgr.h"

#define DX_CACHE_PARAMS_SET_MASK 0x80000000
#define DX_CACHE_PARAMS_SET_TIMEOUT_MS 100

#define CRG_PERRSTEN4 0x90
#define CRG_PERRSTDIS4 0x94
#define CRG_CCP_RST_VAL (1<<15)
static void __iomem *hs_crgctrl_base;

#ifdef DX_DUMP_BYTES
void dump_byte_array(const char *name, const uint8_t *the_array, unsigned long size)
{
	int i , line_offset = 0;
	const uint8_t *cur_byte;
	char line_buf[80];

	line_offset = snprintf(line_buf, sizeof(line_buf), "%s[%lu]: ",
		name, size);

	for (i = 0 , cur_byte = the_array;
	     (i < size) && (line_offset < sizeof(line_buf)); i++, cur_byte++) {
		line_offset += snprintf(line_buf + line_offset,
					sizeof(line_buf) - line_offset,
					"0x%02X ", *cur_byte);
		if (line_offset > 75) { /* Cut before line end */
			DX_LOG_DEBUG("%s\n", line_buf);
			line_offset = 0;
		}
	}

	if (line_offset > 0) /* Dump remainding line */
		DX_LOG_DEBUG("%s\n", line_buf);

}
#endif

#ifdef DX_FPGA_ENV
const DxUint32_t int_session_key[4] = {0x12345678,0x12345678,0x12345678,0x12345678};
#endif

static irqreturn_t cc_isr(int irq, void *dev_id)
{
	struct dx_drvdata *drvdata = (struct dx_drvdata *)dev_id;
	void __iomem *cc_base = drvdata->cc_base;
	uint32_t irr;
	DECL_CYCLE_COUNT_RESOURCES;

	/* STAT_OP_TYPE_GENERIC STAT_PHASE_0: Interrupt */
	START_CYCLE_COUNT();

	/* read the interrupt status */
	irr = READ_REGISTER(cc_base + DX_CC_REG_OFFSET(HOST_RGF, HOST_IRR));
	DX_LOG_DEBUG("Got IRR=0x%08X\n", irr);
	if (unlikely(irr == 0)) { /* Probably shared interrupt line */
		DX_LOG_ERR("Got interrupt with empty IRR\n");
		return IRQ_NONE;
	}

	/* clear interrupt - must be before processing events */
	WRITE_REGISTER(cc_base + DX_CC_REG_OFFSET(HOST_RGF, HOST_ICR), irr);

	/* Completion interrupt - most probable */
	if (likely((irr & DX_COMP_IRQ_MASK) != 0)) {
#if (DX_DEV_SIGNATURE == DX_CC442P_SIG)
		/* Mask AXI completion interrupt */
		WRITE_REGISTER(cc_base + DX_CC_REG_OFFSET(HOST_RGF, HOST_IMR),
			READ_REGISTER(cc_base + DX_CC_REG_OFFSET(HOST_RGF, HOST_IMR)) | DX_COMP_IRQ_MASK);
#endif
		complete_request(drvdata);
		irr &= ~DX_COMP_IRQ_MASK;
	}

#if (DX_DEV_SIGNATURE == DX_CC441P_SIG)
	/* GPR6 interrupt - just one time... */
	if (unlikely((irr & DX_GPR6_IRQ_MASK) != 0)) {
		complete(&drvdata->icache_setup_completion);
		irr &= ~DX_GPR6_IRQ_MASK;
	}
#else /* DX_CC442P_SIG */
	/* AXI error interrupt */
	if (unlikely((irr & DX_AXI_ERR_IRQ_MASK) != 0)) {
		uint32_t axi_err;

		/* Clear on read AXI MON completion event which will enable completion interrupts */
		axi_err = READ_REGISTER(cc_base + DX_CC_REG_OFFSET(CRY_KERNEL, AXIM_MON_ERR));
		DX_LOG_DEBUG("AXI completion error: axim_mon_err=0x%08X\n", axi_err);

		irr &= ~DX_AXI_ERR_IRQ_MASK;
	}
#endif

	if (unlikely(irr != 0)) {
		DX_LOG_DEBUG("IRR includes unknown cause bits (0x%08X)\n", irr);
		/* Just warning */
	}

	END_CYCLE_COUNT(STAT_OP_TYPE_GENERIC, STAT_PHASE_0);
	START_CYCLE_COUNT_AT(drvdata->isr_exit_cycles);

	return IRQ_HANDLED;
}

static inline int init_cc_regs(struct dx_drvdata *drvdata)
{
	uint32_t rom_ver;
	uint32_t hw_queue_size;
	uint32_t irq_delay;
	int rc = 0;
	unsigned int val;
	void __iomem *cc_base = drvdata->cc_base;
#if (DX_DEV_SIGNATURE == DX_CC441P_SIG)
	long wait_rc;
#endif

	rom_ver = READ_REGISTER(cc_base + DX_CC_REG_OFFSET(HOST_RGF, HOST_SEP_HOST_GPR0));
	if (rom_ver != DX_ROM_VERSION) {
		DX_LOG_ERR("wrong ROM version: ROM version=0x%08X != expected=0x%08X\n",
			rom_ver, DX_ROM_VERSION);
		rc = -EINVAL;
		goto init_cc_regs_err;
	}

	/* Unmask all AXI interrupt sources AXI_CFG1 register */
	val = READ_REGISTER(cc_base + DX_CC_REG_OFFSET(CRY_KERNEL, AXIM_CFG1));
	WRITE_REGISTER(cc_base + DX_CC_REG_OFFSET(CRY_KERNEL, AXIM_CFG1), val & ~DX_AXI_IRQ_MASK);
	DX_LOG_DEBUG("AXIM_CFG1=0x%08X\n", READ_REGISTER(cc_base + DX_CC_REG_OFFSET(CRY_KERNEL, AXIM_CFG1)));

	/* Clear all pending interrupts */
	val = READ_REGISTER(cc_base + DX_CC_REG_OFFSET(HOST_RGF, HOST_IRR));
	DX_LOG_DEBUG("IRR=0x%08X\n", val);
	WRITE_REGISTER(cc_base + DX_CC_REG_OFFSET(HOST_RGF, HOST_ICR), val);

#if (DX_DEV_SIGNATURE == DX_CC441P_SIG)
	/* Unmask relevant interrupt cause */
	WRITE_REGISTER(cc_base + DX_CC_REG_OFFSET(HOST_RGF, HOST_IMR),
			~(DX_COMP_IRQ_MASK | DX_AXI_ERR_IRQ_MASK | DX_GPR6_IRQ_MASK));
#else /* DX_CC442P_SIG */
	/* Unmask relevant interrupt cause */
	WRITE_REGISTER(cc_base + DX_CC_REG_OFFSET(HOST_RGF, HOST_IMR),
			~(DX_COMP_IRQ_MASK | DX_AXI_ERR_IRQ_MASK));
#endif

#ifdef DX_FPGA_ENV
	/* Development/testing environment settings */

	/* Force NS bit to 0 even for public operations */
	/* Required when developing with Linux/ARM in secure mode */
        WRITE_REGISTER(drvdata->env_base + DX_ENV_SECURITY_MODE_OVERRIDE_REG_OFFSET, 0xA);

	/* set the session key via env registers */
	WRITE_REGISTER(drvdata->env_base + DX_ENV_SESSION_KEY_0_REG_OFFSET, int_session_key[0]);
	WRITE_REGISTER(drvdata->env_base + DX_ENV_SESSION_KEY_1_REG_OFFSET, int_session_key[1]);
	WRITE_REGISTER(drvdata->env_base + DX_ENV_SESSION_KEY_2_REG_OFFSET, int_session_key[2]);
	WRITE_REGISTER(drvdata->env_base + DX_ENV_SESSION_KEY_3_REG_OFFSET, int_session_key[3]);
#endif

	/* Queue size: Default is 819 slots */
	hw_queue_size = READ_REGISTER(cc_base + DX_CC_REG_OFFSET(CRY_KERNEL, DSCRPTR_QUEUE_SRAM_SIZE));

	DX_LOG_DEBUG("HW_QUEUE_SIZE=0x%08X\n", hw_queue_size);
	if (hw_queue_size != HW_QUEUE_SIZE) {
		DX_LOG_ERR("Invalid HW queue size\n");
		rc = -ENOMEM;
		goto init_cc_regs_err;
	}

#ifdef DX_IRQ_DELAY
	/* Set CC IRQ delay */
	WRITE_REGISTER(cc_base + DX_HOST_IRQ_TIMER_INIT_VAL_REG_OFFSET, DX_IRQ_DELAY);
#endif

	irq_delay = READ_REGISTER(cc_base + DX_HOST_IRQ_TIMER_INIT_VAL_REG_OFFSET);
	if (irq_delay > 0) {
		DX_LOG_DEBUG("irq_delay=%d CC cycles\n", irq_delay);
	}

#if (DX_DEV_SIGNATURE == DX_CC441P_SIG)
	INIT_COMPLETION(drvdata->icache_setup_completion);
	val = READ_REGISTER(cc_base + DX_CC_REG_OFFSET(CRY_KERNEL, HOST_SEP_HOST_GPR6));
	if ((val & DX_CACHE_PARAMS_SET_MASK) == 0) {
		WRITE_REGISTER(cc_base + DX_CC_REG_OFFSET(HOST_RGF, HOST_HOST_SEP_GPR6),
			DX_CACHE_PARAMS | DX_CACHE_PARAMS_SET_MASK);
		wait_rc = wait_for_completion_interruptible_timeout(&drvdata->icache_setup_completion,
			msecs_to_jiffies(DX_CACHE_PARAMS_SET_TIMEOUT_MS));
		if (wait_rc <= 0) {
			DX_LOG_ERR("Failed waiting for cache params setting completion after %u [ms] (rc=%ld)\n",
				DX_CACHE_PARAMS_SET_TIMEOUT_MS, wait_rc);
			if (wait_rc == 0)
				rc = -ETIME;
			else
				rc = wait_rc; /* Probably -ERESTART */
			goto init_cc_regs_err;
		}
		val = READ_REGISTER(cc_base + DX_CC_REG_OFFSET(CRY_KERNEL, HOST_SEP_HOST_GPR6));
		if (val != (DX_CACHE_PARAMS | DX_CACHE_PARAMS_SET_MASK)) {
			DX_LOG_ERR("Failed to set cache params! "
				   "set value=0x%08X , read value=0x%08X\n",
				(DX_CACHE_PARAMS  | DX_CACHE_PARAMS_SET_MASK),
				val);
			rc = -ENODEV;
			goto init_cc_regs_err;
		}
		DX_LOG_INFO("Cache params set to 0x%08X\n", val);
	} else {
		val = READ_REGISTER(cc_base + DX_CC_REG_OFFSET(CRY_KERNEL, HOST_SEP_HOST_GPR6));
		DX_LOG_ERR("Cache params were already set to 0x%08X\n", val);
	}
#else /* DX_CC442P_SIG */
	val = READ_REGISTER(cc_base + DX_CC_REG_OFFSET(CRY_KERNEL, AXIM_CACHE_PARAMS));
	DX_LOG_INFO("Cache params previous: 0x%08X\n", val);
	WRITE_REGISTER(cc_base + DX_CC_REG_OFFSET(CRY_KERNEL, AXIM_CACHE_PARAMS), DX_CACHE_PARAMS);
	val = READ_REGISTER(cc_base + DX_CC_REG_OFFSET(CRY_KERNEL, AXIM_CACHE_PARAMS));
	DX_LOG_INFO("Cache params current: 0x%08X\n", val);
#endif

	return 0;

init_cc_regs_err:
	/* Mask all interrupts */
	WRITE_REGISTER(cc_base +
		       DX_CC_REG_OFFSET(HOST_RGF, HOST_IMR), 0xFFFFFFFF);
	return rc;

}

static int init_cc_resources(struct platform_device *plat_dev)
{
	struct resource *req_mem_cc_regs = NULL;
	void __iomem *cc_base = NULL;
	int irq_registered = 0;
#ifdef DX_FPGA_ENV
	struct resource *req_mem_env_regs = NULL;
#endif
	struct dx_drvdata *new_drvdata = kzalloc(sizeof(struct dx_drvdata), GFP_KERNEL);
	uint32_t signature_val;
	uint32_t rom_ver;
	int rc = 0;

	if (unlikely(new_drvdata == NULL)) {
		DX_LOG_ERR("Failed to allocate drvdata");
		rc = -ENOMEM;
		goto init_cc_res_err;
	}
	dev_set_drvdata(&plat_dev->dev, new_drvdata);
	/* Get device resources */
	/* First CC registers space */
	new_drvdata->res_mem = platform_get_resource(plat_dev, IORESOURCE_MEM, 0);
	if (unlikely(new_drvdata->res_mem == NULL)) {
		DX_LOG_ERR("Failed getting IO memory resource\n");
		rc = -ENODEV;
		goto init_cc_res_err;
	}
	DX_LOG_DEBUG("Got MEM resource (%s): start=0x%08X end=0x%08X\n",
		new_drvdata->res_mem->name, new_drvdata->res_mem->start, new_drvdata->res_mem->end);
	/* Map registers space */
	req_mem_cc_regs = request_mem_region(new_drvdata->res_mem->start, resource_size(new_drvdata->res_mem), "dx_cc44p_regs");
	if (unlikely(req_mem_cc_regs == NULL)) {
		DX_LOG_ERR("Couldn't allocate registers memory region at "
			     "0x%08X\n", DX_BASE_CC);
		rc = -EBUSY;
		goto init_cc_res_err;
	}
	cc_base = ioremap(DX_BASE_CC, REG_SPACE_SIZE);
	if (unlikely(cc_base == NULL)) {
		DX_LOG_ERR("ioremap[CC](0x%08X,0x%08X) failed\n",
			DX_BASE_CC, REG_SPACE_SIZE);
		rc = -ENOMEM;
		goto init_cc_res_err;
	}
	DX_LOG_DEBUG("CC registers mapped from 0x%08X to 0x%p\n", DX_BASE_CC, cc_base);
	new_drvdata->cc_base = cc_base;


	/* Then IRQ */
	new_drvdata->res_irq = platform_get_resource(plat_dev, IORESOURCE_IRQ, 0);
	if (unlikely(new_drvdata->res_irq == NULL)) {
		DX_LOG_ERR("Failed getting IRQ resource\n");
		rc = -ENODEV;
		goto init_cc_res_err;
	}
	rc = request_irq(new_drvdata->res_irq->start, cc_isr,
			 IRQF_SHARED, "cc44_perf_test", new_drvdata);
	if (unlikely(rc != 0)) {
		DX_LOG_ERR("Could not register to interrupt %d\n", new_drvdata->res_irq->start);
		goto init_cc_res_err;
	}
	init_completion(&new_drvdata->icache_setup_completion);

	irq_registered = 1;
	DX_LOG_DEBUG("Registered to IRQ (%s) %u\n", new_drvdata->res_irq->name, new_drvdata->res_irq->start);

	new_drvdata->clk = devm_clk_get(&plat_dev->dev, NULL);
	if (IS_ERR_OR_NULL(new_drvdata->clk)) {
		DX_LOG_ERR("Failed to get clk\n");
		goto init_cc_res_err;
	}

	if (clk_prepare_enable(new_drvdata->clk)) {
		DX_LOG_ERR("Failed to enable clk\n");
		goto init_cc_res_err;
	}


#ifdef DX_FPGA_ENV
	/* Need to map environment for ACP config. */
	/* Map registers space */
	req_mem_env_regs = request_mem_region(DX_BASE_ENV_REGS, REG_SPACE_SIZE, "dx_cc44_env");
	if (!req_mem_env_regs) {
		DX_LOG_ERR("Couldn't allocate environment registers memory "
			     "region at 0x%08X\n", DX_BASE_CC);
		rc = -EBUSY;
		goto init_cc_res_err;
	}
	new_drvdata->env_base = ioremap(DX_BASE_ENV_REGS, REG_SPACE_SIZE);
	if (new_drvdata->env_base == NULL) {
		DX_LOG_ERR("ioremap[ENV](0x%08X,0x%08X) failed\n",
			DX_BASE_ENV_REGS, REG_SPACE_SIZE);
		rc = -ENOMEM;
		goto init_cc_res_err;
	}
	DX_LOG_DEBUG("ENV registers mapped from 0x%08X to 0x%p\n", DX_BASE_ENV_REGS, new_drvdata->env_base);
#endif

	new_drvdata->plat_dev = plat_dev;

#ifdef DISABLE_COHERENT_DMA_OPS
#warning Using default (incoherent) DMA operations
#else
	/* ARM-specific DMA coherency operations option */
	set_dma_ops(&plat_dev->dev, &arm_coherent_dma_ops);
#endif

	/* Verify correct mapping */
	signature_val = READ_REGISTER(cc_base + DX_CC_REG_OFFSET(HOST_RGF, HOST_CC_SIGNATURE));
	if (signature_val != DX_DEV_SIGNATURE) {
		DX_LOG_ERR("Invalid CC signature: SIGNATURE=0x%08X != expected=0x%08lX\n",
			signature_val, DX_DEV_SIGNATURE);
		rc = -EINVAL;
		goto init_cc_res_err;
	}
	DX_LOG_DEBUG("CC SIGNATURE=0x%08X\n", signature_val);

	rom_ver = READ_REGISTER(cc_base + DX_CC_REG_OFFSET(HOST_RGF, HOST_SEP_HOST_GPR0));
	if (rom_ver != DX_ROM_VERSION) {
		DX_LOG_ERR("wrong ROM version: ROM version=0x%08X != expected=0x%08X\n",
			rom_ver, DX_ROM_VERSION);
		rc = -EINVAL;
		goto init_cc_res_err;
	}

	/* Display the versions */
#if (DX_DEV_SIGNATURE == DX_CC441P_SIG)
	DX_LOG(KERN_INFO, "CryptoCell CC441P Driver: ROM version 0x%08X, Driver version %s\n",
		rom_ver, DRV_MODULE_VERSION);
#elif (DX_DEV_SIGNATURE == DX_CC442P_SIG)
	DX_LOG(KERN_INFO, "CryptoCell CC442P Driver: HW version 0x%08X, ROM version 0x%08X, Driver version %s\n",
		READ_REGISTER(cc_base + DX_CC_REG_OFFSET(HOST_RGF, HOST_VERSION)), rom_ver, DRV_MODULE_VERSION);
#else
#error Unsupported DX_DEV_SIGNATURE value
#endif

	rc = init_cc_regs(new_drvdata);
	if (unlikely(rc != 0)) {
		DX_LOG_ERR("init_cc_regs failed\n");
		goto init_cc_res_err;
	}

	rc = dx_sram_mgr_init(new_drvdata);
	if (unlikely(rc != 0)) {
		DX_LOG_ERR("dx_sram_mgr_init failed\n");
		goto init_cc_res_err;
	}

	new_drvdata->mlli_sram_addr =
		dx_sram_mgr_alloc(new_drvdata, MAX_MLLI_BUFF_SIZE);
	if (unlikely(new_drvdata->mlli_sram_addr == NULL)) {
		DX_LOG_ERR("Failed to alloc MLLI Sram buffer\n");
		goto init_cc_res_err;
	}

	rc = request_mgr_init(new_drvdata);
	if (unlikely(rc != 0)) {
		DX_LOG_ERR("request_mgr_init failed\n");
		goto init_cc_res_err;
	}

	rc = buffer_mgr_init(new_drvdata);
	if (unlikely(rc != 0)) {
		DX_LOG_ERR("buffer_mgr_init failed\n");
		goto init_cc_res_err;
	}

	rc = dx_ivgen_init(new_drvdata);
	if (unlikely(rc != 0)) {
		DX_LOG_ERR("dx_ivgen_init failed\n");
		goto init_cc_res_err;
	}

	/* Allocate crypto algs */
	rc = dx_ablkcipher_alloc(new_drvdata);
	if (unlikely(rc != 0)) {
		DX_LOG_ERR("dx_ablkcipher_alloc failed\n");
		goto init_cc_res_err;
	}

	/* hash must be allocated before aead since hash exports APIs */
	rc = dx_hash_alloc(new_drvdata);
	if (unlikely(rc != 0)) {
		DX_LOG_ERR("dx_hash_alloc failed\n");
		goto init_cc_res_err;
	}

	rc = dx_aead_alloc(new_drvdata);
	if (unlikely(rc != 0)) {
		DX_LOG_ERR("dx_aead_alloc failed\n");
		goto init_cc_res_err;
	}

	return 0;

init_cc_res_err:
	DX_LOG_ERR("Freeing CC HW resources!\n");

	if (new_drvdata != NULL) {
		dx_hash_free(new_drvdata);
		dx_aead_free(new_drvdata);
		dx_ablkcipher_free(new_drvdata);
		dx_ivgen_fini(new_drvdata);
		buffer_mgr_fini(new_drvdata);
		request_mgr_fini(new_drvdata);
		dx_sram_mgr_fini(new_drvdata);

	#ifdef DX_FPGA_ENV
		if (req_mem_env_regs) {
			if (new_drvdata->env_base != NULL) {
				iounmap(new_drvdata->env_base);
				new_drvdata->env_base = NULL;
			}
			release_mem_region(DX_BASE_ENV_REGS, REG_SPACE_SIZE);
		}
	#endif

		if (new_drvdata->clk != NULL) {
			clk_disable_unprepare(new_drvdata->clk);
			devm_clk_put(&plat_dev->dev, new_drvdata->clk);
			new_drvdata->clk = NULL;
		}

		if (req_mem_cc_regs != NULL) {
				if (irq_registered) {
					free_irq(new_drvdata->res_irq->start,
						new_drvdata);
					new_drvdata->res_irq = NULL;
				iounmap(cc_base);
				new_drvdata->cc_base = NULL;
			}
			release_mem_region(new_drvdata->res_mem->start,
				resource_size(new_drvdata->res_mem));
			new_drvdata->res_mem = NULL;
		}
		kfree(new_drvdata);
		dev_set_drvdata(&plat_dev->dev, NULL);
	}

	return rc;
}

static inline void fini_cc_regs(struct dx_drvdata *drvdata)
{
	/* Mask all interrupts */
	WRITE_REGISTER(drvdata->cc_base +
		       DX_CC_REG_OFFSET(HOST_RGF, HOST_IMR), 0xFFFFFFFF);

}

static void cleanup_cc_resources(struct platform_device *plat_dev)
{
	struct dx_drvdata *drvdata =
		(struct dx_drvdata *)dev_get_drvdata(&plat_dev->dev);

	dx_hash_free(drvdata);
	dx_aead_free(drvdata);
	dx_ablkcipher_free(drvdata);
	dx_ivgen_fini(drvdata);
	buffer_mgr_fini(drvdata);
	request_mgr_fini(drvdata);
	dx_sram_mgr_fini(drvdata);

	/* Mask all interrupts */
	WRITE_REGISTER(drvdata->cc_base + DX_CC_REG_OFFSET(HOST_RGF, HOST_IMR),
		0xFFFFFFFF);
	free_irq(drvdata->res_irq->start, drvdata);
	drvdata->res_irq = NULL;

	fini_cc_regs(drvdata);

#ifdef DX_FPGA_ENV
	if (drvdata->env_base != NULL) {
		iounmap(drvdata->env_base);
		release_mem_region(DX_BASE_ENV_REGS, REG_SPACE_SIZE);
		drvdata->env_base = NULL;
	}
#endif
	if (drvdata->cc_base != NULL) {
		iounmap(drvdata->cc_base);
		release_mem_region(drvdata->res_mem->start,
			resource_size(drvdata->res_mem));
		drvdata->cc_base = NULL;
		drvdata->res_mem = NULL;
	}

	if (drvdata->clk != NULL) {
		clk_disable_unprepare(drvdata->clk);
		devm_clk_put(&plat_dev->dev, drvdata->clk);
		drvdata->clk = NULL;
	}

	kfree(drvdata);
	dev_set_drvdata(&plat_dev->dev, NULL);
}

static int cc44_probe(struct platform_device *plat_dev)
{
	int rc;
	struct device_node *np_crgctrl;

#ifndef DISABLE_COHERENT_DMA_OPS
#define PERI_CTRL13 0x34
#define PERI_COHE_EN 0x3C0

	uint32_t val = 0;
	void __iomem *hs_pctrl_base;
	struct device_node *np_pctrl;
	np_pctrl = of_find_compatible_node(NULL, NULL, "hisilicon,pctrl");
	if (!np_pctrl) {
		dev_err(&plat_dev->dev, "cc44_probe: of_find_compatible_node failed!\n");
		return -EAGAIN;
	}
	hs_pctrl_base = of_iomap(np_pctrl, 0);
	val = readl(hs_pctrl_base + PERI_CTRL13);
	val |= PERI_COHE_EN;
	writel(val, hs_pctrl_base + PERI_CTRL13);
#endif

#if defined(CONFIG_ARM) && defined(DX_DEBUG)
	uint32_t ctr, cacheline_size;

	asm volatile("mrc p15, 0, %0, c0, c0, 1" : "=r" (ctr));
	cacheline_size =  4 << ((ctr >> 16) & 0xf);
	DX_LOG_DEBUG("CP15(L1_CACHE_BYTES) = %u , Kconfig(L1_CACHE_BYTES) = %u\n",
		cacheline_size, L1_CACHE_BYTES);

	asm volatile("mrc p15, 0, %0, c0, c0, 0" : "=r" (ctr));
	DX_LOG_DEBUG("Main ID register (MIDR): Implementer 0x%02X, Arch 0x%01X,"
		     " Part 0x%03X, Rev r%dp%d\n",
		(ctr>>24), (ctr>>16)&0xF, (ctr>>4)&0xFFF, (ctr>>20)&0xF, ctr&0xF);
#endif

	np_crgctrl = of_find_compatible_node(NULL, NULL, "hisilicon,crgctrl");
	if (!np_crgctrl) {
		dev_err(&plat_dev->dev, "cc44_probe: of_find_compatible_node failed!\n");
		return -EAGAIN;
	}
	hs_crgctrl_base = of_iomap(np_crgctrl, 0);

	/* Map registers space */
	rc = init_cc_resources(plat_dev);
	if (rc != 0)
		return rc;

	/* Initialize the DB */
	init_stat_db();

	DX_LOG(KERN_INFO, "Discretix CC44P device initialized\n");

	return 0;
}

static int cc44_remove(struct platform_device *plat_dev)
{
	DX_LOG_DEBUG("Releasing CC44 resources...\n");

	cleanup_cc_resources(plat_dev);

	DX_LOG(KERN_INFO, "Discretix CC44P device terminated\n");
	display_stat_db();

	return 0;
}

#ifdef CONFIG_PM

static int dx_driver_suspend(struct platform_device *pdev, pm_message_t state)
{
	int rc;
	struct dx_drvdata *drvdata =
		(struct dx_drvdata *)dev_get_drvdata(&pdev->dev);

	dev_info(&pdev->dev, "%s+", __func__);

	fini_cc_regs(drvdata);

	rc = dx_suspend_queue(drvdata);

	clk_disable_unprepare(drvdata->clk);

	dev_info(&pdev->dev, "%s-", __func__);

	return rc;
}

static int dx_driver_resume(struct platform_device *pdev)
{
	int rc;
	struct dx_drvdata *drvdata =
		(struct dx_drvdata *)dev_get_drvdata(&pdev->dev);

	dev_info(&pdev->dev, "%s+", __func__);

	/* reset seceng-p. */
	writel(CRG_CCP_RST_VAL, hs_crgctrl_base + CRG_PERRSTEN4);
	/* disable reset seceng-p. */
	writel(CRG_CCP_RST_VAL, hs_crgctrl_base + CRG_PERRSTDIS4);

	if (clk_prepare_enable(drvdata->clk)) {
		DX_LOG_ERR("Failed to enable clk\n");
		return -EAGAIN;
	}

	rc = init_cc_regs(drvdata);
	if (rc != 0) {
		return rc;
	}
	dx_resume_queue(drvdata);
	/* must be after the queue resuming as it uses the HW queue*/
	dx_hash_init_sram_digest_consts(drvdata);

	dev_info(&pdev->dev, "%s-", __func__);

	return 0;
}

#else

#define dx_driver_suspend(pdev,state) NULL
#define dx_driver_resume(pdev) NULL

#endif


#ifdef CONFIG_OF
static const struct of_device_id dx_dev_of_match[] = {
	{.compatible = "dx,cc44p"},
	{}
};
MODULE_DEVICE_TABLE(of, dx_dev_of_match);
#endif

static struct platform_driver cc44_driver = {
	.driver = {
		   .name = "dx_cc44p",
		   .owner = THIS_MODULE,
#ifdef CONFIG_OF
		   .of_match_table = dx_dev_of_match,
#endif
	},
	.probe = cc44_probe,
	.remove = cc44_remove,
	.suspend = dx_driver_suspend,
	.resume = dx_driver_resume,
};
module_platform_driver(cc44_driver);

/* Module description */
MODULE_DESCRIPTION("Discretix CC44P Driver");
MODULE_VERSION(DRV_MODULE_VERSION);
MODULE_AUTHOR("Discretix");
MODULE_LICENSE("GPL v2");
