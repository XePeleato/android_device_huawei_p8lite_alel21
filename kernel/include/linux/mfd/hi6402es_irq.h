/*
 * hisi_6402_irq.h  --  Interrupt controller support for Hisilicon HI6402ES
 *
 * Copyright (c) 2014 Hisilicon Technologies CO., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef	__HI6402ES_IRQ_H__
#define	__HI6402ES_IRQ_H__

#include <linux/clk.h>
#include <linux/irqdomain.h>
#include <linux/mutex.h>
#include <linux/wakelock.h>
#include <linux/regulator/consumer.h>

#define HI6402ES_MAX_IRQS	24

enum hi6402es_pll_status {
	HI6402ES_PLL_PD,
	HI6402ES_PLL_HIGH_FREQ,
	HI6402ES_PLL_LOW_FREQ,
};

struct hi6402es_irq {
	struct resource		*res;
	struct irq_domain	*domain;
	void __iomem		*reg_base_addr;
	unsigned int		page_sel_ssi;
	struct regulator_bulk_data	regu;
	struct clk		*codec_ssi_clk;
	struct clk		*pmu_audio_clk;
	struct pinctrl		*pctrl;
	spinlock_t		lock;
	int			irq;
	int			gpio;
	int			rst;
	unsigned int		irqs[HI6402ES_MAX_IRQS];

	struct device		*dev;
	struct workqueue_struct	*hi6402es_irq_delay_wq;
	struct delayed_work	hi6402es_irq_delay_work;
	struct mutex		sr_mutex;
	struct mutex		mutex;
	struct wake_lock	wake_lock;

	unsigned int		ibias_work;
	struct mutex		ibias_mutex;
	struct mutex		pll_mutex;
	unsigned int		ref_pll;
	bool			mad_need_pll;
	enum hi6402es_pll_status pll_status;
};

/* Register Access Helpers */
u32 hi6402es_irq_read(struct hi6402es_irq *irq, unsigned int reg);
void hi6402es_irq_write(struct hi6402es_irq *irq, unsigned int reg, u32 val);
void hi6402es_reg_set_bit(struct hi6402es_irq *irq, unsigned int reg, unsigned int offset);
void hi6402es_reg_clr_bit(struct hi6402es_irq *irq, unsigned int reg, unsigned int offset);
void hi6402es_reg_write_bits(struct hi6402es_irq *irq, unsigned int reg, unsigned int value, unsigned int mask);
void hi6402es_irq_ibias_work_enable(struct hi6402es_irq *irq, bool enable);
void hi6402es_irq_pll_enable(struct hi6402es_irq *irq, bool enable);
void hi6402es_irq_mad_pll_enable(struct hi6402es_irq *irq, bool enable);

#endif		/* __HI6402ES_IRQ_H__ */
