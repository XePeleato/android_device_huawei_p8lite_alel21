/*
 * Hisilicon clock driver
 *
 * Copyright (c) 2012-2013 Hisilicon Limited.
 * Copyright (c) 2012-2013 Linaro Limited.
 *
 * Author: Haojian Zhuang <haojian.zhuang@linaro.org>
 *	   Xin Li <li.xin@linaro.org>
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

#define HI3620_DISABLE_OFF		0x4
#define HI3620_STATUS_OFF		0x8

#define WIDTH_TO_MASK(width)	((1 << (width)) - 1)

/*
 * The reverse of DIV_ROUND_UP: The maximum number which
 * divided by m is r
 */
#define MULT_ROUND_UP(r, m) ((r) * (m) + (m) - 1)

struct hi3620_periclk {
	struct clk_hw	hw;
	void __iomem	*enable;	/* enable register */
	void __iomem	*reset;		/* reset register */
	u32		ebits;		/* bits in enable/disable register */
	u32		rbits;		/* bits in reset/unreset register */
	spinlock_t	*lock;
};

struct hi3620_muxclk {
	struct clk_hw	hw;
	void __iomem	*reg;		/* mux register */
	u8		shift;
	u8		width;
	u32		mbits;		/* mask bits in mux register */
	spinlock_t	*lock;
};

struct hi3620_divclk {
	struct clk_hw	hw;
	void __iomem	*reg;		/* divider register */
	u8		shift;
	u8		width;
	u32		mbits;		/* mask bits in divider register */
	const struct clk_div_table	*table;
	spinlock_t	*lock;
};

struct hs_clk {
	void __iomem	*pmctrl;
	void __iomem	*sctrl;
	spinlock_t	lock;
};

static void __init hs_init_clocks(void);

static struct hs_clk hs_clk;

static int hi3620_clkgate_prepare(struct clk_hw *hw)
{
	struct hi3620_periclk *pclk;
	unsigned long flags = 0;

	pclk = container_of(hw, struct hi3620_periclk, hw);

	if (pclk->lock)
		spin_lock_irqsave(pclk->lock, flags);
	writel_relaxed(pclk->ebits, pclk->enable + HI3620_DISABLE_OFF);
	writel_relaxed(pclk->rbits, pclk->reset + HI3620_DISABLE_OFF);
	readl_relaxed(pclk->reset + HI3620_STATUS_OFF);
	if (pclk->lock)
		spin_unlock_irqrestore(pclk->lock, flags);
	return 0;
}

static int hi3620_clkgate_enable(struct clk_hw *hw)
{
	struct hi3620_periclk *pclk;
	unsigned long flags = 0;

	pclk = container_of(hw, struct hi3620_periclk, hw);
	if (pclk->lock)
		spin_lock_irqsave(pclk->lock, flags);
	writel_relaxed(pclk->ebits, pclk->enable);
	readl_relaxed(pclk->enable + HI3620_STATUS_OFF);
	if (pclk->lock)
		spin_unlock_irqrestore(pclk->lock, flags);
	return 0;
}

static void hi3620_clkgate_disable(struct clk_hw *hw)
{
	struct hi3620_periclk *pclk;
	unsigned long flags = 0;

	pclk = container_of(hw, struct hi3620_periclk, hw);
	if (pclk->lock)
		spin_lock_irqsave(pclk->lock, flags);
	writel_relaxed(pclk->ebits, pclk->enable + HI3620_DISABLE_OFF);
	readl_relaxed(pclk->enable + HI3620_STATUS_OFF);
	if (pclk->lock)
		spin_unlock_irqrestore(pclk->lock, flags);
}

static struct clk_ops hi3620_clkgate_ops = {
	.prepare	= hi3620_clkgate_prepare,
	.enable		= hi3620_clkgate_enable,
	.disable	= hi3620_clkgate_disable,
};

static void __init hi3620_clkgate_setup(struct device_node *np)
{
	struct hi3620_periclk *pclk;
	struct clk_init_data *init;
	struct clk *clk;
	const char *clk_name, *name, **parent_names;
	u32 rdata[2], gdata[2];

	if (!hs_clk.sctrl)
		return;

	if (of_property_read_string(np, "clock-output-names", &clk_name))
		return;
	if (of_property_read_u32_array(np, "hisilicon,hi3620-clkreset",
				       &rdata[0], 2))
		return;
	if (of_property_read_u32_array(np, "hisilicon,hi3620-clkgate",
				       &gdata[0], 2))
		return;

	/* gate only has the fixed parent */
	parent_names = kzalloc(sizeof(char *), GFP_KERNEL);
	if (!parent_names)
		return;
	parent_names[0] = of_clk_get_parent_name(np, 0);

	pclk = kzalloc(sizeof(*pclk), GFP_KERNEL);
	if (!pclk)
		goto err_pclk;

	init = kzalloc(sizeof(*init), GFP_KERNEL);
	if (!init)
		goto err_init;
	init->name = kstrdup(clk_name, GFP_KERNEL);
	init->ops = &hi3620_clkgate_ops;
	init->flags = CLK_SET_RATE_PARENT;
	init->parent_names = parent_names;
	init->num_parents = 1;

	pclk->reset = hs_clk.sctrl + rdata[0];
	pclk->rbits = rdata[1];
	pclk->enable = hs_clk.sctrl + gdata[0];
	pclk->ebits = gdata[1];
	pclk->lock = &hs_clk.lock;
	pclk->hw.init = init;

	clk = clk_register(NULL, &pclk->hw);
	if (IS_ERR(clk))
		goto err_clk;
	if (!of_property_read_string(np, "clock-names", &name))
		clk_register_clkdev(clk, name, NULL);
	of_clk_add_provider(np, of_clk_src_simple_get, clk);
	return;
err_clk:
	kfree(init);
err_init:
	kfree(pclk);
err_pclk:
	kfree(parent_names);
}

static u8 hi3620_clk_get_parent(struct clk_hw *hw)
{
	struct hi3620_muxclk *mclk;
	u32 data;
	unsigned long flags = 0;

	mclk = container_of(hw, struct hi3620_muxclk, hw);

	if (mclk->lock)
		spin_lock_irqsave(mclk->lock, flags);

	data = readl_relaxed(mclk->reg) >> mclk->shift;
	data &= WIDTH_TO_MASK(mclk->width);

	if (mclk->lock)
		spin_unlock_irqrestore(mclk->lock, flags);

	if (data >= __clk_get_num_parents(hw->clk))
		return -EINVAL;

	return (u8)data;
}

static int hi3620_clk_set_parent(struct clk_hw *hw, u8 index)
{
	struct hi3620_muxclk *mclk;
	u32 data;
	unsigned long flags = 0;

	mclk = container_of(hw, struct hi3620_muxclk, hw);

	if (mclk->lock)
		spin_lock_irqsave(mclk->lock, flags);

	data = readl_relaxed(mclk->reg);
	data &= ~(WIDTH_TO_MASK(mclk->width) << mclk->shift);
	data |= index << mclk->shift;
	writel_relaxed(data, mclk->reg);
	/* set mask enable bits */
	data |= mclk->mbits;
	writel_relaxed(data, mclk->reg);

	if (mclk->lock)
		spin_unlock_irqrestore(mclk->lock, flags);

	return 0;
}

static struct clk_ops hi3620_clkmux_ops = {
	.get_parent	= hi3620_clk_get_parent,
	.set_parent	= hi3620_clk_set_parent,
};

static void __init hi3620_clkmux_setup(struct device_node *np)
{
	struct hi3620_muxclk *mclk;
	struct clk_init_data *init;
	struct clk *clk;
	const char *clk_name, **parent_names;
	u32 rdata[2];
	u8 num_parents;
	int i;

	hs_init_clocks();
	if (!hs_clk.sctrl)
		return;

	if (of_property_read_string(np, "clock-output-names", &clk_name))
		return;
	if (of_property_read_u32_array(np, "hisilicon,hi3620-clkmux",
				       &rdata[0], 2))
		return;
	/* get the count of items in mux */
	for (i = 0; ; i++) {
		/* parent's #clock-cells property is always 0 */
		if (!of_parse_phandle(np, "clocks", i))
			break;
	}
	parent_names = kzalloc(sizeof(char *) * i, GFP_KERNEL);
	if (!parent_names)
		return;

	for (num_parents = i, i = 0; i < num_parents; i++)
		parent_names[i] = of_clk_get_parent_name(np, i);

	mclk = kzalloc(sizeof(*mclk), GFP_KERNEL);
	if (!mclk)
		goto err_mclk;
	init = kzalloc(sizeof(*init), GFP_KERNEL);
	if (!init)
		goto err_init;
	init->name = kstrdup(clk_name, GFP_KERNEL);
	init->ops = &hi3620_clkmux_ops;
	init->flags = CLK_SET_RATE_PARENT;
	init->parent_names = parent_names;
	init->num_parents = num_parents;

	mclk->reg = hs_clk.sctrl + rdata[0];
	/* enable_mask bits are in higher 16bits */
	mclk->mbits = rdata[1] << 16;
	mclk->shift = ffs(rdata[1]) - 1;
	mclk->width = fls(rdata[1]) - ffs(rdata[1]) + 1;
	mclk->lock = &hs_clk.lock;
	mclk->hw.init = init;

	clk = clk_register(NULL, &mclk->hw);
	if (IS_ERR(clk))
		goto err_clk;
	of_clk_add_provider(np, of_clk_src_simple_get, clk);

	return;
err_clk:
	kfree(init);
err_init:
	kfree(mclk);
err_mclk:
	kfree(parent_names);
}

static void __init hs_clkgate_setup(struct device_node *np)
{
	struct clk *clk;
	const char *clk_name, **parent_names, *name;
	unsigned long flags = 0;
	u32 data[2];

	hs_init_clocks();
	if (!hs_clk.sctrl)
		return;
	if (of_property_read_string(np, "clock-output-names", &clk_name))
		return;
	if (of_property_read_u32_array(np, "hisilicon,clkgate",
				       &data[0], 2))
		return;
	if (of_property_read_bool(np, "hisilicon,clkgate-inverted"))
		flags = CLK_GATE_SET_TO_DISABLE;
	/* gate only has the fixed parent */
	parent_names = kzalloc(sizeof(char *), GFP_KERNEL);
	if (!parent_names)
		return;
	parent_names[0] = of_clk_get_parent_name(np, 0);

	clk = clk_register_gate(NULL, clk_name, parent_names[0], 0,
				hs_clk.sctrl + data[0], (u8)data[1], flags,
				&hs_clk.lock);
	if (IS_ERR(clk))
		goto err;
	if (!of_property_read_string(np, "clock-names", &name))
		clk_register_clkdev(clk, name, NULL);
	of_clk_add_provider(np, of_clk_src_simple_get, clk);
	return;
err:
	kfree(parent_names);
}

void __init hs_fixed_factor_setup(struct device_node *np)
{
	struct clk *clk;
	const char *clk_name, **parent_names;
	u32 data[2];

	if (of_property_read_string(np, "clock-output-names", &clk_name))
		return;
	if (of_property_read_u32_array(np, "hisilicon,fixed-factor",
				       data, 2))
		return ;
	/* gate only has the fixed parent */
	parent_names = kzalloc(sizeof(char *), GFP_KERNEL);
	if (!parent_names)
		return;
	parent_names[0] = of_clk_get_parent_name(np, 0);

	clk = clk_register_fixed_factor(NULL, clk_name, parent_names[0], 0,
					data[0], data[1]);
	if (IS_ERR(clk))
		goto err;
	of_clk_add_provider(np, of_clk_src_simple_get, clk);
	return;
err:
	kfree(parent_names);
}

static unsigned int hi3620_get_table_maxdiv(const struct clk_div_table *table)
{
	unsigned int maxdiv = 0;
	const struct clk_div_table *clkt;

	for (clkt = table; clkt->div; clkt++)
		if (clkt->div > maxdiv)
			maxdiv = clkt->div;
	return maxdiv;
}

static unsigned int hi3620_get_table_div(const struct clk_div_table *table,
							unsigned int val)
{
	const struct clk_div_table *clkt;

	for (clkt = table; clkt->div; clkt++)
		if (clkt->val == val)
			return clkt->div;
	return 0;
}

static unsigned int hi3620_get_table_val(const struct clk_div_table *table,
					 unsigned int div)
{
	const struct clk_div_table *clkt;

	for (clkt = table; clkt->div; clkt++)
		if (clkt->div == div)
			return clkt->val;
	return 0;
}

static unsigned long hi3620_clkdiv_recalc_rate(struct clk_hw *hw,
					       unsigned long parent_rate)
{
	struct hi3620_divclk *dclk = container_of(hw, struct hi3620_divclk, hw);
	unsigned int div, val;

	val = readl_relaxed(dclk->reg) >> dclk->shift;
	val &= WIDTH_TO_MASK(dclk->width);

	div = hi3620_get_table_div(dclk->table, val);
	if (!div) {
		pr_warning("%s: Invalid divisor for clock %s\n", __func__,
			   __clk_get_name(hw->clk));
		return parent_rate;
	}

	return parent_rate / div;
}

static bool hi3620_is_valid_table_div(const struct clk_div_table *table,
				      unsigned int div)
{
	const struct clk_div_table *clkt;

	for (clkt = table; clkt->div; clkt++)
		if (clkt->div == div)
			return true;
	return false;
}

static int hi3620_clkdiv_bestdiv(struct clk_hw *hw, unsigned long rate,
				 unsigned long *best_parent_rate)
{
	struct hi3620_divclk *dclk = container_of(hw, struct hi3620_divclk, hw);
	struct clk *clk_parent = __clk_get_parent(hw->clk);
	int i, bestdiv = 0;
	unsigned long parent_rate, best = 0, now, maxdiv;

	maxdiv = hi3620_get_table_maxdiv(dclk->table);

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
		if (!hi3620_is_valid_table_div(dclk->table, i))
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
		bestdiv = hi3620_get_table_maxdiv(dclk->table);
		*best_parent_rate = __clk_round_rate(clk_parent, 1);
	}

	return bestdiv;
}

static long hi3620_clkdiv_round_rate(struct clk_hw *hw, unsigned long rate,
				     unsigned long *prate)
{
	int div;

	if (!rate)
		rate = 1;
	div = hi3620_clkdiv_bestdiv(hw, rate, prate);

	return *prate / div;
}

static int hi3620_clkdiv_set_rate(struct clk_hw *hw, unsigned long rate,
				  unsigned long parent_rate)
{
	struct hi3620_divclk *dclk = container_of(hw, struct hi3620_divclk, hw);
	unsigned int div, value;
	unsigned long flags = 0;
	u32 data;

	div = parent_rate / rate;
	value = hi3620_get_table_val(dclk->table, div);

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

	return 0;
}

static struct clk_ops hi3620_clkdiv_ops = {
	.recalc_rate = hi3620_clkdiv_recalc_rate,
	.round_rate = hi3620_clkdiv_round_rate,
	.set_rate = hi3620_clkdiv_set_rate,
};

void __init hi3620_clkdiv_setup(struct device_node *np)
{
	struct clk *clk;
	const char *clk_name, **parent_names;
	struct clk_init_data *init;
	struct clk_div_table *table;
	struct hi3620_divclk *dclk;
	unsigned int table_num;
	int i;
	u32 data[2];
	const char *propname = "hisilicon,clkdiv-table";
	const char *cellname = "#hisilicon,clkdiv-table-cells";
	struct of_phandle_args div_table;

	hs_init_clocks();
	if (!hs_clk.sctrl)
		return;

	if (of_property_read_string(np, "clock-output-names", &clk_name))
		return;
	if (of_property_read_u32_array(np, "hisilicon,clkdiv",
				       &data[0], 2))
		return;

	/*process the div_table*/
	for (i = 0; ; i++) {
		if (of_parse_phandle_with_args(np, propname, cellname,
					       i, &div_table))
			break;
	}

	/*table ends with <0, 0>, so plus one to table_num*/
	table_num = i + 1;

	table = kzalloc(sizeof(struct clk_div_table) * table_num, GFP_KERNEL);
	if (!table)
		return ;

	for (i = 0; ; i++) {
		if (of_parse_phandle_with_args(np, propname, cellname,
					       i, &div_table))
			break;

		table[i].val = div_table.args[0];
		table[i].div = div_table.args[1];
	}

	/* gate only has the fixed parent */
	parent_names = kzalloc(sizeof(char *), GFP_KERNEL);
	if (!parent_names)
		goto err_par;
	parent_names[0] = of_clk_get_parent_name(np, 0);

	dclk = kzalloc(sizeof(*dclk), GFP_KERNEL);
	if (!dclk)
		goto err_dclk;
	init = kzalloc(sizeof(*init), GFP_KERNEL);
	if (!init)
		goto err_init;
	init->name = kstrdup(clk_name, GFP_KERNEL);
	init->ops = &hi3620_clkdiv_ops;
	init->parent_names = parent_names;
	init->num_parents = 1;

	dclk->reg = hs_clk.sctrl + data[0];
	dclk->shift = ffs(data[1]) - 1;
	dclk->width = fls(data[1]) - ffs(data[1]) + 1;
	dclk->mbits = data[1] << 16;
	dclk->lock = &hs_clk.lock;
	dclk->hw.init = init;
	dclk->table = table;
	clk = clk_register(NULL, &dclk->hw);
	if (IS_ERR(clk))
		goto err_clk;
	of_clk_add_provider(np, of_clk_src_simple_get, clk);
	return;
err_clk:
	kfree(init);
err_init:
	kfree(dclk);
err_dclk:
	kfree(parent_names);
err_par:
	kfree(table);
}
CLK_OF_DECLARE(hi3620_mux, "hisilicon,hi3620-clk-mux", hi3620_clkmux_setup)
CLK_OF_DECLARE(hi3620_gate, "hisilicon,hi3620-clk-gate", hi3620_clkgate_setup)
CLK_OF_DECLARE(hi3620_div, "hisilicon,hi3620-clk-div", hi3620_clkdiv_setup)
CLK_OF_DECLARE(hs_gate, "hisilicon,clk-gate", hs_clkgate_setup)
CLK_OF_DECLARE(hs_fixed, "hisilicon,clk-fixed-factor", hs_fixed_factor_setup)

static void __init hs_init_clocks(void)
{
	struct device_node *node = NULL;

	if (!hs_clk.pmctrl) {
		/* map pmctrl registers */
		node = of_find_compatible_node(NULL, NULL, "hisilicon,pmctrl");
		hs_clk.pmctrl = of_iomap(node, 0);
		WARN_ON(!hs_clk.pmctrl);
	}

	if (!hs_clk.sctrl) {
		node = of_find_compatible_node(NULL, NULL, "hisilicon,sysctrl");
		hs_clk.sctrl = of_iomap(node, 0);
	}
}
