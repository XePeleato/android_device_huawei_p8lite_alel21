/*
 * hisi_6402_irq.h  --  Interrupt controller support for Hisilicon HI6402
 *
 * Copyright (c) 2014 Hisilicon Technologies CO., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef	__HI6402_IRQ_H__
#define	__HI6402_IRQ_H__

#include <linux/clk.h>
#include <linux/irqdomain.h>
#include <linux/mutex.h>
#include <linux/wakelock.h>
#include <linux/regulator/consumer.h>

#define HI6402_MAX_IRQS	32

enum hi6402_pll_status {
	HI6402_PLL_PD,
	HI6402_PLL_HIGH_FREQ,
	HI6402_PLL_LOW_FREQ,
	HI6402_PLL_RST,
};

struct hi6402_irq {
	struct resource		*res;
	struct irq_domain	*domain;
	void __iomem		*reg_base_addr;
	unsigned int		page_sel_ssi;
	struct regulator_bulk_data	regu;
	struct regulator_bulk_data	regu_ldo26;
	struct clk		*codec_ssi_clk;
	struct clk		*pmu_audio_clk;
	struct pinctrl		*pctrl;
	struct pinctrl_state	*pins_default;
	struct pinctrl_state	*pins_idle;

	spinlock_t		lock;
	int			irq;
	int			gpio;
	int			rst;
	unsigned int		irqs[HI6402_MAX_IRQS];

	struct device		*dev;

	struct workqueue_struct	*hi6402_micbias_delay_wq;
	struct delayed_work	hi6402_micbias_delay_work;

	struct mutex		rw_mutex;
	struct mutex		handler_mutex;
	struct mutex		sr_mutex;
	struct mutex		irq_lock;
	struct wake_lock	wake_lock;

	u8			mask0;
	u8			mask1;
	u8			mask2;
	u8			mask3;
	u8			irq0;
	u8			irq1;
	u8			irq2;
	u8			irq3;

	bool			mbhc_micbias_work;
	bool			dapm_micbias_work;
	unsigned int	dapm_ibias_work;
	bool			mbhc_ibias_work;

	struct mutex		ibias_mutex;
	struct mutex		pll_mutex;
	struct mutex		hs_micbias_mutex;
	unsigned int		ref_pll;
	unsigned int		mad_need_pll;
	enum hi6402_pll_status pll_status;
	bool			hifi_misc_probe;
};

/* Register Access Helpers */
u32 hi6402_irq_read(struct hi6402_irq *irq, unsigned int reg);
void hi6402_irq_write(struct hi6402_irq *irq, unsigned int reg, u32 val);
void hi6402_reg_set_bit(struct hi6402_irq *irq, unsigned int reg, unsigned int offset);
void hi6402_reg_clr_bit(struct hi6402_irq *irq, unsigned int reg, unsigned int offset);
void hi6402_reg_write_bits(struct hi6402_irq *irq, unsigned int reg, unsigned int value, unsigned int mask);
void hi6402_irq_ibias_work_enable(struct hi6402_irq *irq, bool enable);
void hi6402_irq_micbias_work_enable(struct hi6402_irq *irq, bool enable);
void hi6402_irq_micbias_mbhc_enable(struct hi6402_irq *irq, bool enable);
void hi6402_irq_high_freq_pll_enable(struct hi6402_irq *irq, bool enable);
void hi6402_irq_low_freq_pll_enable(struct hi6402_irq *irq, bool enable);
void hi6402_pll_pd(struct hi6402_irq *irq);
void hi6402_irq_set_pll_mode(struct hi6402_irq *irq);
void hi6402_irq_clr_btn_irqs(struct hi6402_irq *irq);
void hi6402_irq_mask_btn_irqs(struct hi6402_irq *irq);
void hi6402_irq_unmask_btn_irqs(struct hi6402_irq *irq);
void hi6402_irq_hs_micbias_enable(struct hi6402_irq *irq, bool enable);
void hi6402_irq_cancel_delay_work(struct hi6402_irq *irq);
void hi6402_irq_enable_ldo8(struct hi6402_irq *irq, bool enable);

#endif		/* __HI6402_IRQ_H__ */
