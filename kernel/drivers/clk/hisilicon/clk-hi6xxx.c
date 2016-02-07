/*
 * Hisilicon Balong Hi6620 SOC clock driver
 *
 * Copyright (c) 2013-2014 Hisilicon Limited.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#include <linux/kernel.h>
#include <linux/clk-provider.h>
#include <linux/clk-private.h>
#include <linux/clkdev.h>
#include <linux/delay.h>
#include <linux/io.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_device.h>
#include <linux/slab.h>
#include <linux/clk.h>
#include <linux/hisi/pm/pwrctrl_multi_dfs.h>
#include <linux/mfd/hisi_hi6xxx_pmic.h>
#include <soc_baseaddr_interface.h>
#include <soc_smart_interface.h>
#include <soc_pmctrl_interface.h>

#include <linux/hisi/pm/hi6xxx-power-dfs.h>
#include <linux/hisi/pm/pwrctrl_multi_memcfg.h>
#include "hisi-clk-mailbox.h"

#define HI6XXX_CLK_GATE_DISABLE_OFFSET		0x4
#define HI6XXX_CLK_GATE_STATUS_OFFSET		0x8

#define HI6XXX_SMARTCLK_GATE_DISABLE_OFFSET		0x1
#define HI6XXX_SMARTCLK_GATE_STATUS_OFFSET		0x2

/* reset register offset */
#define HI6XXX_RST_DISABLE_REG_OFFSET		0x4

#define WIDTH_TO_MASK(width)	((1 << (width)) - 1)

#define HI6XXX_PMIC_DCXO_MODE    0
#define HI6XXX_PMIC_TCXO_MODE    1

#define PLL_CLK_READY           0x10000000 /*Bit 28*/

/*
 * The reverse of DIV_ROUND_UP: The maximum number which
 * divided by m is r
 */
#define MULT_ROUND_UP(r, m) ((r) * (m) + (m) - 1)

enum {
	HS_PMCTRL,
	HS_SYSCTRL,
	HS_AOSYSCTRL,
	HS_MEDIASYSCTRL,
	HS_SMARTCTRL,
};

struct reg_ops {
	void		(*enable)(void __iomem *reg, int enable_bit);
	void		(*disable)(void __iomem *reg, int enable_bit);
	int		    (*is_enabled)(void __iomem *reg, int enable_bit);
};

struct hi6xxx_periclk {
	struct clk_hw	hw;
	void __iomem	*enable;	/* enable register */
	void __iomem	*reset;		/* reset register */
	u32		ebits;				/* bits in enable/disable register */
	u32		rbits;				/* bits in reset/unreset register */
	const char	*friend;
	spinlock_t	*lock;
    struct reg_ops *ops;
};

struct hi6xxx_muxclk {
	struct clk_hw	hw;
	void __iomem	*reg;		/* mux register */
	u8		shift;
	u8		width;
	u32		mbits;				/* mask bits in mux register */
	spinlock_t	*lock;
};

struct hi6xxx_divclk {
	struct clk_hw	hw;
	void __iomem	*reg;		/* divider register */
	u8		shift;
	u8		width;
	u32		mbits;		/* mask bits in divider register */
	const struct clk_div_table	*table;
	spinlock_t	*lock;
};

struct hi6xxx_xfreq_clk {
	struct clk_hw	hw;
	void __iomem	*reg;	/* ctrl register */

	/* v8r1 -- 0: A7; 1: A15;  2: gpu; 3:ddr */
    /* v8r2 -- 0: A53; 1: A53;  2: gpu; 3:ddr */
	u32		id;
	u32		set_rate_cmd[LPM3_CMD_LEN];
	u32		get_rate_cmd[LPM3_CMD_LEN];

	void __iomem	*pll_sel_reg;
	u32		pll_sel_mask;
	void __iomem	*pll_div_reg;
	u32		pll_div_mask;
	u32		pll_div_aclk_mask;
	u32 	pll_div_busclk_mask;

	u32		rate;
};

struct hs_clk {
	void __iomem	*pmctrl;
	void __iomem	*sctrl;
	void __iomem	*aosctrl;
	void __iomem	*mediasctrl;
	char __iomem	*smartctrl;
	spinlock_t	lock;
};

static struct hs_clk hs_clk = {
	.lock = __SPIN_LOCK_UNLOCKED(hs_clk.lock),
};

static void pmctrl_enable(void __iomem *reg, int enable_bit)
{
    int ret = enable_bit;
    int pll_ready_bit = 0;
    int count = 5;

	ret |= readl_relaxed(reg);
    //pr_info("%s reg : 0x%p value : 0x%x status : 0x%x\n", __FUNCTION__,
    //        reg, ret, readl_relaxed(reg));
	writel_relaxed(ret, reg);

	pll_ready_bit = readl_relaxed(reg) & PLL_CLK_READY;
	/* ddrpll and bbppll use shadow register */
	if(((((unsigned long)reg) & 0xFF) != SOC_PMCTRL_DDRPLL0ALIAS_ADDR(0))
	    && ((((unsigned long)reg) & 0xFF) != SOC_PMCTRL_BBPPLLALIAS_ADDR(0))){
		while(pll_ready_bit == 0 && (count >0))
		{
			mdelay(1);
			pll_ready_bit = readl_relaxed(reg) & PLL_CLK_READY;
			count = count - 1;
		}
		if(count <= 0){
			pr_err("PLL_CLK[0x%p] is NOT Ready for using \n", reg);
		}
	}
	else{
		mdelay(5);
	}
}

static void pmctrl_disable(void __iomem *reg, int enable_bit)
{
    int ret = ~enable_bit;
	ret &= readl_relaxed(reg);
    //pr_info("%s reg : 0x%p value : 0x%x status : 0x%x\n", __FUNCTION__,
        //reg, ret, readl_relaxed(reg));
	writel_relaxed(ret, reg);
}

static int pmctrl_is_enabled(void __iomem *reg, int enable_bit)
{
    int ret = enable_bit;
	ret &= readl_relaxed(reg);
    return ret ? 1 : 0;
}

struct reg_ops pmctrl_regctrl = {
	.enable		= pmctrl_enable,
	.disable	= pmctrl_disable,
	.is_enabled = pmctrl_is_enabled,
};


static void sctrl_enable(void __iomem *reg, int enable_bit)
{
	writel_relaxed(enable_bit, reg);
    //pr_info("%s reg : 0x%p value : 0x%x status : 0x%x\n", __FUNCTION__,
     //   reg, enable_bit,  readl_relaxed(reg + HI6XXX_CLK_GATE_STATUS_OFFSET));
}

static void sctrl_disable(void __iomem *reg, int enable_bit)
{
	writel_relaxed(enable_bit, reg + HI6XXX_CLK_GATE_DISABLE_OFFSET);
    //pr_info("%s reg : 0x%p value : 0x%x status : 0x%x\n", __FUNCTION__,
     //   reg + HI6XXX_CLK_GATE_DISABLE_OFFSET, enable_bit, readl_relaxed(reg + HI6XXX_CLK_GATE_STATUS_OFFSET));
}

static int sctrl_is_enabled(void __iomem *reg, int enable_bit)
{
    int ret = enable_bit;
	ret &= readl_relaxed(reg + HI6XXX_CLK_GATE_STATUS_OFFSET);
    return ret ? 1 : 0;
}

static struct reg_ops sctrl_regctrl = {
	.enable		= sctrl_enable,
	.disable	= sctrl_disable,
	.is_enabled = sctrl_is_enabled,
};


static void smartrw_enable(void __iomem *reg, int enable_bit)
{
    unsigned char ret = (unsigned char)enable_bit;
    unsigned int reg_offset = (unsigned int)((char __iomem *)reg - hs_clk.smartctrl);
    ret |= hi6xxx_pmic_reg_read_ex ((char *)hs_clk.smartctrl, reg_offset);
    //pr_info("%s reg : 0x%p value : 0x%x status : 0x%x\n", __FUNCTION__,
        //reg, ret, hi6xxx_pmic_reg_read_ex (hs_clk.smartctrl, reg_offset));
	hi6xxx_pmic_reg_write_ex (hs_clk.smartctrl, reg_offset, ret);
}

static void smartrw_disable(void __iomem *reg, int enable_bit)
{
    unsigned char ret = (unsigned char)enable_bit;
    unsigned int reg_offset = (unsigned int)((char __iomem *)reg - hs_clk.smartctrl);
    ret = ~ret;
    ret &= hi6xxx_pmic_reg_read_ex (hs_clk.smartctrl, reg_offset);
    //pr_info("%s reg : 0x%p value : 0x%x status : 0x%x\n", __FUNCTION__,
        //reg, ret, hi6xxx_pmic_reg_read_ex (hs_clk.smartctrl, reg_offset));
	hi6xxx_pmic_reg_write_ex (hs_clk.smartctrl, reg_offset, ret);
}

static int smartrw_is_enabled(void __iomem *reg, int enable_bit)
{
    unsigned char ret = (unsigned char)enable_bit;
    unsigned int reg_offset = (unsigned int)((char __iomem *)reg - hs_clk.smartctrl);
    ret &= hi6xxx_pmic_reg_read_ex (hs_clk.smartctrl, reg_offset);
    return ret ? 1 : 0;
}

static struct reg_ops smartrw_regctrl = {
	.enable		= smartrw_enable,
	.disable	= smartrw_disable,
	.is_enabled = smartrw_is_enabled,
};


static void smart_enable(void __iomem *reg, int enable_bit)
{
    unsigned char ret = (unsigned char)enable_bit;
    unsigned int reg_offset = (unsigned int)((char __iomem *)reg - hs_clk.smartctrl);
    //pr_info("%s reg : 0x%p value : 0x%x status : 0x%x\n", __FUNCTION__,
        //reg, ret, hi6xxx_pmic_reg_read_ex (hs_clk.smartctrl, reg_offset + HI6XXX_SMARTCLK_GATE_STATUS_OFFSET));
	hi6xxx_pmic_reg_write_ex (hs_clk.smartctrl, reg_offset, ret);
}

static void smart_disable(void __iomem *reg, int enable_bit)
{
    unsigned char ret = (unsigned char)enable_bit;
    unsigned int reg_offset = (unsigned int)((char __iomem *)reg - hs_clk.smartctrl);
    //pr_info("%s reg : 0x%p value : 0x%x status : 0x%x\n", __FUNCTION__,
        //reg, ret, hi6xxx_pmic_reg_read_ex (hs_clk.smartctrl, reg_offset + HI6XXX_SMARTCLK_GATE_STATUS_OFFSET));
	hi6xxx_pmic_reg_write_ex (hs_clk.smartctrl, reg_offset + HI6XXX_SMARTCLK_GATE_DISABLE_OFFSET, ret);
}

static int smart_is_enabled(void __iomem *reg, int enable_bit)
{
    unsigned char ret = (unsigned char)enable_bit;
    unsigned int reg_offset = (unsigned int)((char __iomem *)reg - hs_clk.smartctrl);
    ret &= hi6xxx_pmic_reg_read_ex (hs_clk.smartctrl, reg_offset + HI6XXX_SMARTCLK_GATE_STATUS_OFFSET);
    return ret ? 1 : 0;
}

static struct reg_ops smart_regctrl = {
	.enable		= smart_enable,
	.disable	= smart_disable,
	.is_enabled = smart_is_enabled,
};

static void __iomem __init *hs_clk_get_base(struct device_node *np, struct reg_ops **r_ops);

extern int __clk_prepare(struct clk *clk);
extern int __clk_enable(struct clk *clk);

static int xfreq_req_id[] = {
    DFS_INVALID_ID,
    DFS_INVALID_ID,
    DFS_INVALID_ID,
    DFS_INVALID_ID,
    DFS_INVALID_ID,
    DFS_INVALID_ID
};

static int hi6xxx_clkgate_enable(struct clk_hw *hw)
{
	struct hi6xxx_periclk *pclk;
	struct clk *friend_clk;
	unsigned long flags = 0;
	int ret = 0;

	pclk = container_of(hw, struct hi6xxx_periclk, hw);
	if (pclk->lock)
		spin_lock_irqsave(pclk->lock, flags);
	if (pclk->enable && pclk->ops && pclk->ops->enable) {
		//pr_info("%s %s\n", __FUNCTION__, hw->clk->name);
        pclk->ops->enable(pclk->enable, pclk->ebits);
	}

	/* disable reset register */
	if (pclk->reset)
		writel_relaxed(pclk->rbits, pclk->reset + HI6XXX_RST_DISABLE_REG_OFFSET);

	if (pclk->lock)
		spin_unlock_irqrestore(pclk->lock, flags);

	/*if friend clk exist,enable it*/
	if (pclk->friend) {
		friend_clk = __clk_lookup(pclk->friend);
		if (IS_ERR_OR_NULL(friend_clk)) {
			pr_err("%s get failed!\n", pclk->friend);
			return -1;
		}
		ret = __clk_prepare(friend_clk);
		if (ret) {
			pr_err("[%s], friend clock prepare faild!", __func__);
			return ret;
		}
		ret = __clk_enable(friend_clk);
		if (ret) {
			__clk_unprepare(friend_clk);
			pr_err("[%s], friend clock enable faild!", __func__);
			return ret;
		}
	}

	return 0;
}

extern int __clk_disable(struct clk *clk);
extern void __clk_unprepare(struct clk *clk);
static void hi6xxx_clkgate_disable(struct clk_hw *hw)
{
	struct hi6xxx_periclk *pclk;
	struct clk *friend_clk;
	unsigned long flags = 0;

	pclk = container_of(hw, struct hi6xxx_periclk, hw);
	if (pclk->lock)
		spin_lock_irqsave(pclk->lock, flags);

	/* reset the ip, then disalbe clk */
	if (pclk->reset)
		writel_relaxed(pclk->rbits, pclk->reset);

#ifdef CONFIG_HI6XXX_CLK_ALWAYS_ON
#else

	if (pclk->enable && pclk->ops && pclk->ops->disable) {
		//pr_info("%s %s \n", __FUNCTION__, hw->clk->name);
        pclk->ops->disable(pclk->enable, pclk->ebits);
	}
#endif
	if (pclk->lock)
		spin_unlock_irqrestore(pclk->lock, flags);

#ifndef CONFIG_HI6XXX_CLK_ALWAYS_ON
	/*if friend clk exist, disable it .*/
	if (pclk->friend) {
		friend_clk = __clk_lookup(pclk->friend);
		if (IS_ERR_OR_NULL(friend_clk)) {
			pr_err("%s get failed!\n", pclk->friend);
		}
		__clk_disable(friend_clk);
		__clk_unprepare(friend_clk);
	}
#endif
}

static int hi6xxx_clkgate_is_enabled(struct clk_hw *hw)
{
	struct hi6xxx_periclk *pclk;

	pclk = container_of(hw, struct hi6xxx_periclk, hw);

	if (pclk->enable && pclk->ops && pclk->ops->is_enabled)
        return pclk->ops->is_enabled(pclk->enable, pclk->ebits);
	else
		return 2;
}
#if 0
static void hi6xxx_clkgate_init(struct clk_hw *hw)
{
    if (hi6xxx_clkgate_is_enabled(hw)){
		pr_err("%s enabled default\n", hw->clk->name);
        hw->clk->enable_count = 1;
        hw->clk->prepare_count = 1;
    }
}
#endif

#ifdef CONFIG_HISI_CLK_DEBUG
static void __iomem *hi6xxx_clkgate_get_reg(struct clk_hw *hw)
{
	struct hi6xxx_periclk *pclk;
	void __iomem	*ret = NULL;
	u32 val = 0;

	pclk = container_of(hw, struct hi6xxx_periclk, hw);

	if (pclk->enable){
		ret = pclk->enable + HI6XXX_CLK_GATE_STATUS_OFFSET;

		val = readl(ret);
		val &= pclk->ebits;
		pr_info("\n[%s]: reg = 0x%p, bits = 0x%x, regval = 0x%x\n",
			hw->clk->name, ret, pclk->ebits, val);
	}
	else{
		pr_err("[%s:%d]: clkgate reg not fount \n", __FUNCTION__, __LINE__);
	}
	return ret;
}
#endif



static struct clk_ops hi6xxx_clkgate_ops = {
	.enable		= hi6xxx_clkgate_enable,
	.disable	= hi6xxx_clkgate_disable,
	.is_enabled = hi6xxx_clkgate_is_enabled,
#ifdef CONFIG_HISI_CLK_DEBUG
	.get_reg	= hi6xxx_clkgate_get_reg,
#endif
};

static int hi6xxx_32k_clk_check(void)
{
    void __iomem *pmu_baseaddr = NULL;
    unsigned char reg_value;
    unsigned char xo_mode;
    unsigned char xo_status;
    int ret = 0;

    pmu_baseaddr = ioremap(SOC_PMUSSI_BASE_ADDR, SZ_4K);
    if(NULL == pmu_baseaddr)
    {
        pr_err("[%s] fail to ioremap pmu_baseaddr!\n", __func__);
        return SSI_DEVICE_ERR;
    }

    reg_value = hi6xxx_pmic_reg_read_ex(pmu_baseaddr, SOC_SMART_STATUS2_ADDR(0));
    xo_mode = ((reg_value >> SOC_SMART_STATUS2_xo_mode_a2d_ff2_START)&(0x1));
    if(HI6XXX_PMIC_TCXO_MODE == xo_mode)
    {
        xo_status = ((reg_value >> SOC_SMART_STATUS2_tcxo_clk_sel_START)&(0x1));
        if(0 == xo_status)
        {
            pr_err("[%s] tcxo mode status err!\n", __func__);
            ret = SSI_DEVICE_ERR;
        }
        else
        {
            ret = SSI_DEVICE_OK;
        }
    }
    else if(HI6XXX_PMIC_DCXO_MODE == xo_mode)
    {
        xo_status = ((reg_value >> SOC_SMART_STATUS2_dcxo_clk_sel_START)&(0x1));
        if(0 == xo_status)
        {
            pr_err("[%s] dcxo mode status err!\n", __func__);
            ret = SSI_DEVICE_ERR;
        }
        else
        {
            ret = SSI_DEVICE_OK;
        }
    }

    iounmap(pmu_baseaddr);
    return ret;
}

static void __init hi6xxx_clkgate_setup(struct device_node *np)
{
	struct hi6xxx_periclk *pclk;
	struct clk_init_data *init;
	struct clk *clk;
    struct reg_ops *r_ops = NULL;
	const char *clk_name, *name, *clk_friend;
	const char *parent_names;
	void __iomem *reg_base, *pmu_baseaddr;
	u32 rdata[2]={0,0}, gdata[2]={0,0};
	u32 pmu_version =0;

	reg_base = hs_clk_get_base(np, &r_ops);
	if (!reg_base) {
		pr_err("[%s] fail to get reg_base!\n", __func__);
		return;
	}

	if (of_property_read_string(np, "clock-output-names", &clk_name)) {
		pr_err("[%s] %s node doesn't have clock-output-name property!\n",
			 __func__, np->name);
		return;
	}
	if (of_property_read_u32_array(np, "hisilicon,hi6xxx-clkgate",
					   &gdata[0], 2)) {
		pr_err("[%s] %s node doesn't have hi6xxx-clkgate property!\n",
			 __func__, np->name);
		return;
	}
	/* for hi6553, temp solution */
#if 1
	pmu_baseaddr = ioremap(SOC_PMUSSI_BASE_ADDR, SZ_4K);
	pmu_version = hi6xxx_pmic_reg_read_ex(pmu_baseaddr, SOC_SMART_VERSION_ADDR(0));
	if((pmu_version != 0x30) && (pmu_version != 0x31)){
		if (of_property_read_u32_array(np, "hisilicon,hi6xxx-clkgate-hi6553",
						   &gdata[0], 2)) {
	        /* hi6553 4in1 gate Bit is diff from hi6552, replace */
		}
	}
#endif
	if (of_property_read_string(np, "clock-friend-names", &clk_friend))
		clk_friend = NULL;

	parent_names = of_clk_get_parent_name(np, 0);

	pclk = kzalloc(sizeof(*pclk), GFP_KERNEL);
	if (!pclk) {
		pr_err("[%s] fail to alloc pclk!\n", __func__);
		return;
	}

	init = kzalloc(sizeof(*init), GFP_KERNEL);
	if (!init) {
		pr_err("[%s] fail to alloc init!\n", __func__);
		goto err_init;
	}
	init->name = kstrdup(clk_name, GFP_KERNEL);
	init->ops = &hi6xxx_clkgate_ops;
	init->flags = CLK_SET_RATE_PARENT | CLK_IGNORE_UNUSED;
	init->parent_names = &parent_names;
	init->num_parents = 1;

	if (of_property_read_u32_array(np, "hisilicon,hi6xxx-clkreset",
					   &rdata[0], 2)) {
		pclk->reset = NULL;
		pclk->rbits = 0;
	} else {
		pclk->reset = reg_base + rdata[0];
		pclk->rbits = rdata[1];
	}

	/* if gdata[1] is 0, represents the enable reg is fake */
	if (gdata[1] == 0)
			pclk->enable = NULL;
	else
			pclk->enable = reg_base + gdata[0];

	pclk->ebits = gdata[1];
	pclk->lock = &hs_clk.lock;
	pclk->hw.init = init;
	pclk->friend = clk_friend;
    pclk->ops = r_ops;

	clk = clk_register(NULL, &pclk->hw);
	if (IS_ERR(clk)) {
		pr_err("[%s] fail to reigister clk %s!\n",
			__func__, clk_name);
		goto err_clk;
	}
	if (!of_property_read_string(np, "clock-output-names", &name))
		clk_register_clkdev(clk, name, NULL);
	of_clk_add_provider(np, of_clk_src_simple_get, clk);

	return;

err_clk:
	kfree(init);
err_init:
	kfree(pclk);
	return;
}


static int __init hi6xxx_parse_mux(struct device_node *np,
				   u8 *num_parents)
{
	int i, cnt;

	/* get the count of items in mux */
	for (i = 0, cnt = 0;; i++, cnt++) {
		/* parent's #clock-cells property is always 0 */
		if (!of_parse_phandle(np, "clocks", i))
			break;
	}

	for (i = 0; i < cnt; i++) {
		if (!of_clk_get_parent_name(np, i)) {
			pr_err("[%s] cannot get %dth parent_clk name!\n",
				__func__, i);
			return -ENOENT;
		}
	}
	*num_parents = cnt;

	return 0;
}

static void __init hi6xxx_clkmux_setup(struct device_node *np)
{
	struct clk *clk;
	const char *clk_name, **parent_names = NULL;
	u32 rdata[2], width;
	u8 num_parents, shift, flag = 0;
	void __iomem *reg, *base;
	int i, ret;

	base = hs_clk_get_base(np, NULL);
	if (!base) {
		pr_err("[%s] fail to get base!\n", __func__);
		return;
	}

	if (of_property_read_string(np, "clock-output-names", &clk_name)) {
		pr_err("[%s] %s node doesn't have clock-output-name property!\n",
				__func__, np->name);
		return;
	}
	if (of_property_read_u32_array(np, "hisilicon,clkmux-reg",
					   &rdata[0], 2)) {
		pr_err("[%s] %s node doesn't have clkmux-reg property!\n",
				__func__, np->name);
		return;
	}

	if (of_property_read_bool(np, "hiword"))
		flag = CLK_MUX_HIWORD_MASK;

	ret = hi6xxx_parse_mux(np, &num_parents);
	if (ret) {
		pr_err("[%s] %s node cannot get num_parents!\n",
			__func__, np->name);
		return;
	}

	parent_names = kzalloc(sizeof(char *) * num_parents, GFP_KERNEL);
	if (!parent_names) {
		pr_err("[%s] fail to alloc parent_names!\n", __func__);
		return;
	}
	for (i = 0; i < num_parents; i++)
		parent_names[i] = of_clk_get_parent_name(np, i);

	reg = base + rdata[0];
	shift = ffs(rdata[1]) - 1;
	width = fls(rdata[1]) - ffs(rdata[1]) + 1;

	clk = clk_register_mux(NULL, clk_name, parent_names, num_parents,
					 CLK_SET_RATE_PARENT, reg, shift, width,
					 flag, &hs_clk.lock);
	if (IS_ERR(clk)) {
		pr_err("[%s] fail to register mux clk %s!\n",
			__func__, clk_name);
		goto err_clk;
	}

	clk_register_clkdev(clk, clk_name, NULL);
	of_clk_add_provider(np, of_clk_src_simple_get, clk);

err_clk:
	kfree(parent_names);
	return;
}

static unsigned int hi6xxx_get_table_maxdiv(const struct clk_div_table *table)
{
	unsigned int maxdiv = 0;
	const struct clk_div_table *clkt;

	for (clkt = table; clkt->div; clkt++)
		if (clkt->div > maxdiv)
			maxdiv = clkt->div;
	return maxdiv;
}

static unsigned int hi6xxx_get_table_div(const struct clk_div_table *table,
							unsigned int val)
{
	const struct clk_div_table *clkt;

	for (clkt = table; clkt->div; clkt++)
		if (clkt->val == val)
			return clkt->div;
    pr_err("%s:Value %d not in div table, table[0] is %d table[%d] is %d\n", \
        __func__, val, table->div,(unsigned int)(clkt - table),clkt->div);
	return 0;
}

static unsigned int hi6xxx_get_table_val(const struct clk_div_table *table,
					 unsigned int div)
{
	const struct clk_div_table *clkt;

	for (clkt = table; clkt->div; clkt++)
		if (clkt->div == div)
			return clkt->val;
	return 0;
}

static unsigned long hi6xxx_clkdiv_recalc_rate(struct clk_hw *hw,
						   unsigned long parent_rate)
{
	struct hi6xxx_divclk *dclk = container_of(hw, struct hi6xxx_divclk, hw);
	unsigned int div, val;

	val = readl_relaxed(dclk->reg) >> dclk->shift;
	val &= WIDTH_TO_MASK(dclk->width);

	div = hi6xxx_get_table_div(dclk->table, val);
	if (!div) {
		pr_warn("%s: Invalid divisor for clock %s\n", __func__,
			   __clk_get_name(hw->clk));
		return parent_rate;
	}

	return parent_rate / div;
}


static unsigned long hi6xxxsft_clkdiv_recalc_rate(struct clk_hw *hw,
						   unsigned long parent_rate)
{
	struct hi6xxx_divclk *dclk = container_of(hw, struct hi6xxx_divclk, hw);
	unsigned int div, val;
	u32 data;

	val = readl_relaxed(dclk->reg) >> dclk->shift;
	val &= WIDTH_TO_MASK(dclk->width);

	div = hi6xxx_get_table_div(dclk->table, val);
	if (!div) {
		if ((val%2 == 0) && (++val < WIDTH_TO_MASK(dclk->width))){
			pr_err("%s: Odd divisor for clock %s: %d\n", __func__,
				   __clk_get_name(hw->clk), div);
			data = readl_relaxed(dclk->reg);
			data &= ~(WIDTH_TO_MASK(dclk->width) << dclk->shift);
			data |= val << dclk->shift;
			data |= dclk->mbits;
			pr_info("!!!!!!WBT Debug in %s and writing reg %p with value 0x%08x !!!!!!\n",
					__func__, dclk->reg, (unsigned int)data);
			writel_relaxed(data, dclk->reg);
        	val = readl_relaxed(dclk->reg) >> dclk->shift;
        	val &= WIDTH_TO_MASK(dclk->width);
        	div = hi6xxx_get_table_div(dclk->table, val);
		}else{
			pr_err("%s: Invalid divisor for clock %s: div %d val %d\n", __func__,
				   __clk_get_name(hw->clk), div, val);
			return parent_rate;
		}
	}
	/* converity warning */
	return div ? parent_rate / div : parent_rate;
}

static bool hi6xxx_is_valid_table_div(const struct clk_div_table *table,
					  unsigned int div)
{
	const struct clk_div_table *clkt;

	for (clkt = table; clkt->div; clkt++)
		if (clkt->div == div)
			return true;
	return false;
}

static int hi6xxx_clkdiv_bestdiv(struct clk_hw *hw, unsigned long rate,
				 unsigned long *best_parent_rate)
{
	struct hi6xxx_divclk *dclk = container_of(hw, struct hi6xxx_divclk, hw);
	struct clk *clk_parent = __clk_get_parent(hw->clk);
	int i, bestdiv = 0;
	unsigned long parent_rate, best = 0, now, maxdiv;

	maxdiv = hi6xxx_get_table_maxdiv(dclk->table);

	if (!(__clk_get_flags(hw->clk) & CLK_SET_RATE_PARENT)) {
		parent_rate = *best_parent_rate;
		bestdiv = DIV_ROUND_UP(parent_rate, rate);
		bestdiv = bestdiv == 0 ? 1 : bestdiv;
		bestdiv = bestdiv > maxdiv ? maxdiv : bestdiv;
		return bestdiv;
	}

	/*
	 * The maximum divider we can use without overflowing
	 * unsigned long in rate * i below
	 */
	maxdiv = min(ULONG_MAX / rate, maxdiv);

	for (i = 1; i <= maxdiv; i++) {
		if (!hi6xxx_is_valid_table_div(dclk->table, i))
			continue;
		parent_rate = __clk_round_rate(clk_parent,
						   MULT_ROUND_UP(rate, i));
		now = parent_rate / i;
		if (now <= rate && now > best) {
			bestdiv = i;
			best = now;
			*best_parent_rate = parent_rate;
		}
	}

	if (!bestdiv) {
		bestdiv = hi6xxx_get_table_maxdiv(dclk->table);
		*best_parent_rate = __clk_round_rate(clk_parent, 1);
	}

	return bestdiv;
}

static long hi6xxx_clkdiv_round_rate(struct clk_hw *hw, unsigned long rate,
					 unsigned long *prate)
{
	int div;

	if (!rate)
		rate = 1;
	div = hi6xxx_clkdiv_bestdiv(hw, rate, prate);

	return *prate / div;
}

static int hi6xxx_clkdiv_set_rate(struct clk_hw *hw, unsigned long rate,
				  unsigned long parent_rate)
{
	struct hi6xxx_divclk *dclk = container_of(hw, struct hi6xxx_divclk, hw);
	unsigned int div, value;
	unsigned long flags = 0;
	u32 data;

	div = parent_rate / rate;
	value = hi6xxx_get_table_val(dclk->table, div);

	if (value > WIDTH_TO_MASK(dclk->width))
		value = WIDTH_TO_MASK(dclk->width);

	if (dclk->lock)
		spin_lock_irqsave(dclk->lock, flags);

	data = readl_relaxed(dclk->reg);
	data &= ~(WIDTH_TO_MASK(dclk->width) << dclk->shift);
	data |= value << dclk->shift;
	data |= dclk->mbits;

	writel_relaxed(data, dclk->reg);

	if (dclk->lock)
		spin_unlock_irqrestore(dclk->lock, flags);

	pr_info("!!!!!!WBT Debug in %s and writing reg %p with value 0x%08x !!!!!!\n",
			__func__, dclk->reg, (unsigned int)data);

	return 0;
}

#ifdef CONFIG_HISI_CLK_DEBUG
static int hi6xxx_divreg_check(struct clk_hw *hw)
{
	unsigned long rate;
	struct clk *clk = hw->clk;
	struct clk *pclk = clk_get_parent(clk);

	rate = hi6xxx_clkdiv_recalc_rate(hw, clk_get_rate(pclk));
	if (rate == clk_get_rate(clk))
		return 1;
	else
		return 0;
}

static void __iomem *hi6xxx_clkdiv_get_reg(struct clk_hw *hw)
{
	struct hi6xxx_divclk *dclk;
	void __iomem	*ret = NULL;
	u32 val = 0;

	dclk = container_of(hw, struct hi6xxx_divclk, hw);

	if (dclk->reg){
		ret = dclk->reg;

		val = readl(ret);
		val &= dclk->mbits;

		pr_info("\n[%s]: reg = 0x%p, bits = 0x%x, regval = 0x%x\n",
			hw->clk->name, ret, dclk->mbits, val);
	}
	else{
		pr_err("[%s:%d]: div reg not fount \n", __FUNCTION__, __LINE__);
	}
	return ret;
}
#endif

static struct clk_ops hi6xxx_clkdiv_ops = {
	.recalc_rate = hi6xxx_clkdiv_recalc_rate,
	.round_rate = hi6xxx_clkdiv_round_rate,
	.set_rate = hi6xxx_clkdiv_set_rate,
#ifdef CONFIG_HISI_CLK_DEBUG
	.check_divreg = hi6xxx_divreg_check,
	.get_reg = hi6xxx_clkdiv_get_reg,
#endif
};

static struct clk_ops hi6xxxsft_clkdiv_ops = {
	.recalc_rate = hi6xxxsft_clkdiv_recalc_rate,
	.round_rate = hi6xxx_clkdiv_round_rate,
	.set_rate = hi6xxx_clkdiv_set_rate,
#ifdef CONFIG_HISI_CLK_DEBUG
	.check_divreg = hi6xxx_divreg_check,
	.get_reg = hi6xxx_clkdiv_get_reg,
#endif
};

static void __init hi6xxx_clkdiv_setup(struct device_node *np)
{
	struct clk *clk;
	const char *clk_name;
	const char *parent_names;
	struct clk_init_data *init;
	struct clk_div_table *table;
	struct hi6xxx_divclk *dclk;
	void __iomem *reg_base;
	unsigned int table_num;
	int i,idx;
	u32 data[3];
	u32 support_odd_div = 0;
	struct device_node *root;
	unsigned int max_div, min_div;


	reg_base = hs_clk_get_base(np, NULL);
	if (!reg_base) {
		pr_err("[%s] fail to get reg_base!\n", __func__);
		return;
	}

	if (of_property_read_string(np, "clock-output-names", &clk_name)) {
		pr_err("[%s] node %s doesn't have clock-output-names property!\n",
			__func__, np->name);
		return;
	}

	/* process the div_table */
	if (of_property_read_u32_array(np, "hisilicon,clkdiv-table",
					   &data[0], 2)) {
		pr_err("[%s] node %s doesn't have clkdiv-table property!\n",
			__func__, np->name);
		return;
	}

	max_div = data[0];
	min_div = data[1];

	if (of_property_read_u32_array(np, "hisilicon,clkdiv",
								&data[0], 3)) {
		pr_err("[%s] node %s doesn't have clkdiv property!\n",
			__func__, np->name);
		return;
	}

	table_num = max_div - min_div + 1;

	/* table ends with <0, 0>, so plus one to table_num */
	table = kzalloc(sizeof(struct clk_div_table) * (table_num + 1), GFP_KERNEL);
	if (!table) {
		pr_err("[%s] fail to alloc table!\n", __func__);
		return;
	}
    root = of_find_compatible_node(NULL,NULL,"hisilicon,hi6210");
	if (!root){
		pr_err("[%s] find root node fail!\n",
			__func__);
		return;
	}
	if (of_property_read_u32(root, "clk,support_odd_div",
								&support_odd_div)) {
		pr_err("[%s] node %s doesn't have clkdiv property!\n",
			__func__, np->name);
		return;
	}
	/* max div must be even number on sft(fpga) except div = 1*/
	for (i = 0,idx = 0; i < table_num; i++) {
		if (support_odd_div && (i) && ((min_div + i)%2 == 1))
			continue;
		table[idx].div = min_div + i;
		table[idx].val = table[idx].div - 1;
		idx++;
	}

	parent_names = of_clk_get_parent_name(np, 0);

	dclk = kzalloc(sizeof(*dclk), GFP_KERNEL);
	if (!dclk) {
		pr_err("[%s] fail to alloc dclk!\n", __func__);
		goto err_par;
	}
	init = kzalloc(sizeof(*init), GFP_KERNEL);
	if (!init) {
		pr_err("[%s] fail to alloc init!\n", __func__);
		goto err_init;
	}
	init->name = kstrdup(clk_name, GFP_KERNEL);
	init->ops = support_odd_div ? &hi6xxxsft_clkdiv_ops : &hi6xxx_clkdiv_ops;
	init->flags = CLK_SET_RATE_PARENT;
	init->parent_names = &parent_names;
	init->num_parents = 1;

	dclk->reg = reg_base + data[0];
	dclk->shift = ffs(data[1]) - 1;
	dclk->width = fls(data[1]) - ffs(data[1]) + 1;
	/*valid bit cant be 0*/
	dclk->mbits = data[2]?(0x1 << data[2]):0;
	dclk->lock = &hs_clk.lock;
	dclk->hw.init = init;
	dclk->table = table;
	clk = clk_register(NULL, &dclk->hw);
	if (IS_ERR(clk)) {
		pr_err("[%s] fail to register div clk %s!\n",
				__func__, clk_name);
		goto err_clk;
	}
	of_clk_add_provider(np, of_clk_src_simple_get, clk);
	clk_register_clkdev(clk, clk_name, NULL);

	return;
err_clk:
	kfree(init);
err_init:
	kfree(dclk);
err_par:
	kfree(table);
	return;
}


static char *xfreq_devname[] = {
	"XFREQ_ACPU",
	"XFREQ_ACPU",
	"XFREQ_GPU",
	"XFREQ_DDR_MIN",
	"XFREQ_DDR_MAX",
	"XFREQ_DDR_FREQ"
};
static void __iomem *freqinfo_virt_addr = NULL;
static unsigned long hi6xxx_xfreq_clk_recalc_rate(struct clk_hw *hw,
                        unsigned long parent_rate)
{
    u32 rate;
    struct hi6xxx_xfreq_clk *xfreq_clk = container_of(hw, struct hi6xxx_xfreq_clk, hw);
    struct freqdump* freqinfo = (struct freqdump* )freqinfo_virt_addr;

    if(NULL == freqinfo)
    {
        printk(KERN_ERR"%s freqinfo addr is null!!!\n", __FUNCTION__);
        return 0;
    }

    /*get freq from MCU*/
    switch (xfreq_clk->id)
    {
    case 0:  /*A7*/
        rate = freqinfo->freq_acpu_freq_cur * 1000;
        break;
    case 3:
    case 4:
        rate = xfreq_clk->rate;
        break;
    case 5: /*DDR*/
        rate = freqinfo->freq_ddr_freq_cur * 1000;
        break;
    default:
        if (xfreq_clk->rate == 0)
            xfreq_clk->rate = clk_get_rate(hw->clk->parent);

        rate = xfreq_clk->rate;
        break;
    }

    return rate;
}

static long hi6xxx_xfreq_clk_round_rate(struct clk_hw *hw, unsigned long rate,
                        unsigned long *prate)
{
    return rate;
}

static long hi6xxx_xfreq_clk_determine_rate(struct clk_hw *hw, unsigned long rate,
                    unsigned long *best_parent_rate,
                    struct clk **best_parent_clk)
{
    return rate;
}

static int hi6xxx_xfreq_clk_set_rate(struct clk_hw *hw, unsigned long rate,
                    unsigned long parent_rate)
{
    struct hi6xxx_xfreq_clk *xfreq_clk = container_of(hw, struct hi6xxx_xfreq_clk, hw);
    unsigned int new_rate = (unsigned int)rate/1000;  /*Khz*/
    int ret = 0;

    pr_debug("%s: %dKhz \n", xfreq_devname[xfreq_clk->id], new_rate);

    ret = pwrctrl_dfs_qos_update(xfreq_clk->id, &xfreq_req_id[xfreq_clk->id], new_rate);
    if(ret) {
        pr_err("[%s] %s fail to send msg to MCU!\n",
              __func__, xfreq_devname[xfreq_clk->id]);

        return -EINVAL;
    }

    xfreq_clk->rate = rate;
    return ret;
}

static struct clk_ops hi6xxx_xfreq_clk_ops = {
    .recalc_rate = hi6xxx_xfreq_clk_recalc_rate,
    .determine_rate = hi6xxx_xfreq_clk_determine_rate,
    .round_rate = hi6xxx_xfreq_clk_round_rate,
    .set_rate = hi6xxx_xfreq_clk_set_rate,
};

/*
 * xfreq_clk is used for cpufreq & devfreq.
 */
static void __init hi6xxx_xfreq_clk_setup(struct device_node *np)
{
    struct clk *clk;
    const char *clk_name;
    char *parent_names;
    struct clk_init_data *init;
    struct hi6xxx_xfreq_clk *xfreqclk;
    int pmic_clk_stat = 0;
    int ret = 0;
    static int notify_flag = 0;
    u32 device_id = 100;

    pr_info("\n enter [%s] !\n", __func__);

    if (of_property_read_u32(np, "hisilicon,hi6210-xfreq-devid", &device_id)) {
        pr_err("[%s] node %s doesn't have device id property!\n",
            __func__, np->name);
        return;
    }

    pr_info("[%s]: device_id = %d !\n", __func__,device_id);

    if (of_property_read_string(np, "clock-output-names", &clk_name)) {
        pr_err("[%s] node %s doesn't have clock-output-names property!\n",
            __func__, np->name);
        return;
    }
    pr_info("[%s]: clk_name = %s !\n", __func__,clk_name);

    xfreqclk = kzalloc(sizeof(*xfreqclk), GFP_KERNEL);
    if (!xfreqclk) {
        pr_err("[%s] fail to alloc xfreqclk!\n", __func__);
        return;
    }

    init = kzalloc(sizeof(*init), GFP_KERNEL);
    if (!init) {
        pr_err("[%s] fail to alloc init!\n", __func__);
        goto err_init;
    }

    if (NULL == of_clk_get_parent_name(np, 0)) {
        parent_names = NULL;
    }
    else {
        parent_names = (char *)of_clk_get_parent_name(np, 0);
    }

    init->name = kstrdup(clk_name, GFP_KERNEL);
    init->ops = &hi6xxx_xfreq_clk_ops;

    if(NULL == parent_names) {
        init->parent_names= NULL;
    }
    else {
        init->parent_names = &parent_names;
    }

    init->num_parents = (parent_names ? 1 : 0);
    init->flags = CLK_IS_ROOT | CLK_GET_RATE_NOCACHE;

    xfreqclk->hw.init = init;
    xfreqclk->id = device_id;

    freqinfo_virt_addr = ioremap(MEMORY_FREQDUMP_ADDR,MEMORY_FREQDUMP_SIZE);
    if(NULL == freqinfo_virt_addr) {
        pr_err("[%s] fail to ioremap freqdump addr!\n",
            __func__);
        goto err_clk;
    }

    clk = clk_register(NULL, &xfreqclk->hw);
    if (IS_ERR(clk)) {
        pr_err("[%s] fail to register xfreqclk %s!\n",
            __func__, clk_name);
        goto err_clk;
    }

    of_clk_add_provider(np, of_clk_src_simple_get, clk);
    clk_register_clkdev(clk, clk_name, NULL);

    if(!notify_flag)
    {
        pmic_clk_stat = hi6xxx_32k_clk_check();
        ret = hi6xxx_pmic_device_stat_notify("32KCLK", pmic_clk_stat);
        if(ret < 0)
        {
            pr_err("[%s] 32KCLK notify err!\n", __func__);
        }
        notify_flag = 1;
    }

    pr_info(" exit [%s] !\n", __func__);

    return;
err_clk:
    kfree(init);
err_init:
    kfree(xfreqclk);
    return;
}


CLK_OF_DECLARE(hi6xxx_mux, "hisilicon,hi6xxx-clk-mux", hi6xxx_clkmux_setup)
CLK_OF_DECLARE(hi6xxx_div, "hisilicon,hi6xxx-clk-div", hi6xxx_clkdiv_setup)
CLK_OF_DECLARE(hi6xxx_gate, "hisilicon,hi6xxx-clk-gate", hi6xxx_clkgate_setup)
CLK_OF_DECLARE(hi6xxx_dfs_clk, "hisilicon,hi6xxx-clk-stub", hi6xxx_xfreq_clk_setup)

static const struct of_device_id hs_of_match[] = {
	{ .compatible = "hisilicon,pmctrl",		.data = (void *)HS_PMCTRL, },
	{ .compatible = "hisilicon,sctrl",		.data = (void *)HS_SYSCTRL, },
	{ .compatible = "hisilicon,aosctrl",	.data = (void *)HS_AOSYSCTRL, },
	{ .compatible = "hisilicon,media_sctrl",	.data = (void *)HS_MEDIASYSCTRL, },
	{ .compatible = "hisilicon,hi6552-pmic-driver",	.data = (void *)HS_SMARTCTRL, },
};

static void __iomem __init *hs_clk_get_base(struct device_node *np, struct reg_ops **r_ops)
{
	struct device_node *parent;
	const struct of_device_id *match;
	void __iomem *ret = NULL;

	parent = of_get_parent(np);
	if (!parent) {
		pr_err("[%s] node %s doesn't have parent node!\n", __func__, np->name);
		goto out;
	}
	match = of_match_node(hs_of_match, parent);
	if (!match) {
		pr_err("[%s] parent node %s doesn't match!\n", __func__, parent->name);
		goto out;
	}
	switch ((unsigned long)match->data) {
	case HS_PMCTRL:
		if (!hs_clk.pmctrl) {
			ret = of_iomap(parent, 0);
			WARN_ON(!ret);
			hs_clk.pmctrl = ret;
		} else {
			ret = hs_clk.pmctrl;
		}
        if (r_ops)
            *r_ops = &pmctrl_regctrl;
		break;
	case HS_SYSCTRL:
		if (!hs_clk.sctrl) {
			ret = of_iomap(parent, 0);
			WARN_ON(!ret);
			hs_clk.sctrl = ret;
		} else {
			ret = hs_clk.sctrl;
		}
        if (r_ops)
            *r_ops = &sctrl_regctrl;
		break;
	case HS_AOSYSCTRL:
		if (!hs_clk.aosctrl) {
			ret = of_iomap(parent, 0);
			WARN_ON(!ret);
			hs_clk.aosctrl = ret;
		} else {
			ret = hs_clk.aosctrl;
		}
        if (r_ops)
            *r_ops = &sctrl_regctrl;
		break;
	case HS_MEDIASYSCTRL:
		if (!hs_clk.mediasctrl) {
			ret = of_iomap(parent, 0);
			WARN_ON(!ret);
			hs_clk.mediasctrl = ret;
		} else {
			ret = hs_clk.mediasctrl;
		}
        if (r_ops)
            *r_ops = &sctrl_regctrl;
		break;

	case HS_SMARTCTRL:
		if (!hs_clk.smartctrl) {
			ret = of_iomap(parent, 0);
			WARN_ON(!ret);
			hs_clk.smartctrl = ret;
		} else {
			ret = hs_clk.smartctrl;
		}
        if (r_ops){
    	    if (of_property_read_bool(np, "rw")){
		        //pr_err("[%s] node %s probe smart rw!\n", __func__, np->name);
                *r_ops = &smartrw_regctrl;
            } else {
        		//pr_err("[%s] node %s probe smart!\n", __func__, np->name);
                *r_ops = &smart_regctrl;
            }
        }
		break;
	default:
		pr_err("[%s] cannot find the match node!\n", __func__);
		ret = NULL;
	}
out:
	return ret;
}
