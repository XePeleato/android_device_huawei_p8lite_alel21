/*
 * hi6402es.c -- ALSA SoC HI6402ES codec driver
 *
 * Copyright (c) 2014 Hisilicon Technologies CO., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/init.h>
#include <linux/input.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <linux/mutex.h>
#include <linux/regulator/consumer.h>
#include <linux/io.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_device.h>
#include <linux/of_irq.h>
#include <linux/of_platform.h>
#include <linux/wakelock.h>
#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/initval.h>
#include <sound/pcm_params.h>
#include <sound/tlv.h>
#include <sound/soc.h>
#include <sound/jack.h>
#include <asm/io.h>

#include <linux/mfd/hi6402es_irq.h>

#include "hi6402es.h"
#include "huawei_platform/audio/anc_hs.h"

#define HI6402ES_FORMATS	( SNDRV_PCM_FMTBIT_S16_LE | \
			  SNDRV_PCM_FMTBIT_S16_BE | \
			  SNDRV_PCM_FMTBIT_S24_LE | \
			  SNDRV_PCM_FMTBIT_S24_BE )

#define HI6402ES_RATES	SNDRV_PCM_RATE_8000_96000

#define FS_8000		0x0
#define FS_16000	0x1
#define FS_32000	0x2
#define FS_48000	0x4
#define FS_96000	0x5

#ifdef CONFIG_HI6402_MBHC
static struct snd_soc_jack hs_jack;
#endif

/* codec private data */
struct hi6402es_priv {
	struct snd_soc_codec * codec;
	struct hi6402es_irq * p_irq;
	struct clk *codec_clk;
	struct regulator *codec_regulator;
	struct mutex dapm_mutex;

	struct mutex cp_mutex;
	unsigned int ref_cp;

	unsigned int ref_audio_clk;
	unsigned int ref_voice_clk;
	unsigned int ref_audio_tx;
	unsigned int ref_audio_rx;
	bool hsr_power_on;
	bool hsl_power_on;
};

static const struct of_device_id hi6402es_codec_match[] = {
	{ .compatible = "hisilicon,hi6402es-codec", },
	{},
};
MODULE_DEVICE_TABLE(of, hi6402es_codec_match);

static inline unsigned int hi6402es_reg_read(struct snd_soc_codec *codec,
		unsigned int reg)
{
	struct hi6402es_priv *priv = snd_soc_codec_get_drvdata(codec);

	BUG_ON(NULL == priv);
	BUG_ON(NULL == priv->p_irq);

	return hi6402es_irq_read(priv->p_irq, reg);
}

static inline unsigned int hi6402es_dapm_reg_read(struct snd_soc_codec *codec,
		unsigned int reg)
{
	return hi6402es_reg_read(codec, CODEC_BASE_ADDR + reg);
}

static inline int hi6402es_reg_write(struct snd_soc_codec *codec,
		unsigned int reg, unsigned int value)
{
	struct hi6402es_priv *priv = snd_soc_codec_get_drvdata(codec);

	BUG_ON(NULL == priv);
	BUG_ON(NULL == priv->p_irq);
	hi6402es_irq_write(priv->p_irq, reg, value);

	return 0;
}

static inline int hi6402es_dapm_reg_write(struct snd_soc_codec *codec,
		unsigned int reg, unsigned int value)
{
	return hi6402es_reg_write(codec, CODEC_BASE_ADDR + reg, value);
}

static inline void hi6402es_dapm_reg_set_bit(struct snd_soc_codec *codec, unsigned int reg, unsigned int offset)
{
	struct hi6402es_priv *priv = snd_soc_codec_get_drvdata(codec);

	BUG_ON(NULL == priv);
	BUG_ON(NULL == priv->p_irq);
	hi6402es_reg_set_bit(priv->p_irq, CODEC_BASE_ADDR + reg, offset);

	return;
}
static inline void hi6402es_dapm_reg_clr_bit(struct snd_soc_codec *codec, unsigned int reg, unsigned int offset)
{
	struct hi6402es_priv *priv = snd_soc_codec_get_drvdata(codec);

	BUG_ON(NULL == priv);
	BUG_ON(NULL == priv->p_irq);
	hi6402es_reg_clr_bit(priv->p_irq, CODEC_BASE_ADDR + reg, offset);
	return;
}

static inline void hi6402es_dapm_reg_write_bits(struct snd_soc_codec *codec, unsigned int reg, unsigned int value, unsigned int mask)
{
	struct hi6402es_priv *priv = snd_soc_codec_get_drvdata(codec);

	BUG_ON(NULL == priv);
	BUG_ON(NULL == priv->p_irq);
	hi6402es_reg_write_bits(priv->p_irq, CODEC_BASE_ADDR + reg, value, mask);
	return;
}

static void hi6402es_ibias_work_enable(struct snd_soc_codec *codec, bool enable)
{
	struct hi6402es_priv *priv = snd_soc_codec_get_drvdata(codec);

	BUG_ON(NULL == priv);
	BUG_ON(NULL == priv->p_irq);
	hi6402es_irq_ibias_work_enable(priv->p_irq, enable);
	return;
}

static void hi6402es_pll_enable(struct snd_soc_codec *codec, bool enable)
{
	struct hi6402es_priv *priv = snd_soc_codec_get_drvdata(codec);

	BUG_ON(NULL == priv);
	BUG_ON(NULL == priv->p_irq);
	mutex_lock(&priv->dapm_mutex);
	hi6402es_irq_pll_enable(priv->p_irq, enable);
	mutex_unlock(&priv->dapm_mutex);
	return;
}

static void hi6402es_mad_enable(struct snd_soc_codec *codec, bool enable)
{
	struct hi6402es_priv *priv = snd_soc_codec_get_drvdata(codec);

	BUG_ON(NULL == priv);
	BUG_ON(NULL == priv->p_irq);
	mutex_lock(&priv->dapm_mutex);
	hi6402es_irq_mad_pll_enable(priv->p_irq, enable);
	mutex_unlock(&priv->dapm_mutex);
	return;
}

/* IBIAS EVENT */
int hi6402es_ibias_power_mode_event(struct snd_soc_dapm_widget *w,
		struct snd_kcontrol *kcontrol, int event)
{
	struct snd_soc_codec *codec = w->codec;

	BUG_ON(NULL == codec);

	switch (event) {
	case SND_SOC_DAPM_PRE_PMU:
		hi6402es_ibias_work_enable(codec, true);
		break;
	case SND_SOC_DAPM_POST_PMD:
		hi6402es_ibias_work_enable(codec, false);
		break;
	default :
		pr_warn("%s : power mode event err : %d\n", __FUNCTION__, event);
		break;
	}

	return 0;
}

static void hi6402es_mad_power_optimize(struct snd_soc_codec *codec, int enable)
{
	if(enable)
	{
		hi6402es_dapm_reg_write(codec, HI6402ES_ANA_REG29, 0x0);		//1.8v power optimize
		hi6402es_dapm_reg_write(codec, HI6402ES_ANA_REG30, 0x0);		//1.8v power optimize
		hi6402es_dapm_reg_write(codec, HI6402ES_ANA_REG31, 0x0);		//1.8v power optimize
		hi6402es_dapm_reg_write(codec, HI6402ES_ANA_REG35, 0x0);		//3.3v power optimize
		//hi6402es_dapm_reg_write(codec, HI6402ES_SC_MAD_CTRL0, 0x1);		//auto cg set
		//hi6402es_dapm_reg_write(codec, HI6402ES_DSP_CTRL6_DMAC, 0x3D);	//auto cg set
	} else {
		//hi6402es_dapm_reg_write(codec, HI6402ES_DSP_CTRL6_DMAC, 0x11);	//auto cg set
		//hi6402es_dapm_reg_write(codec, HI6402ES_SC_MAD_CTRL0, 0x3E);	//auto cg set
		hi6402es_dapm_reg_write(codec, HI6402ES_ANA_REG29, 0x2D);		//1.8v power optimize
		hi6402es_dapm_reg_write(codec, HI6402ES_ANA_REG30, 0x2D);		//1.8v power optimize
		hi6402es_dapm_reg_write(codec, HI6402ES_ANA_REG31, 0x2D);		//1.8v power optimize
		hi6402es_dapm_reg_write(codec, HI6402ES_ANA_REG35, 0x2D);		//3.3v power optimize
	}
	return;
}

int hi6402es_mad_power_mode_event(struct snd_soc_dapm_widget *w,
		struct snd_kcontrol *kcontrol, int event)
{
	struct snd_soc_codec *codec = w->codec;

	BUG_ON(NULL == codec);

	switch (event) {
	case SND_SOC_DAPM_PRE_PMU:
		hi6402es_mad_enable(codec, true);
		hi6402es_dapm_reg_set_bit(codec, HI6402ES_MISC_CLK_EN, HI6402ES_MAD_CLK1_EN_BIT);
		hi6402es_dapm_reg_set_bit(codec, HI6402ES_MISC_CLK_EN, HI6402ES_MAD_CLK0_EN_BIT);
		hi6402es_mad_power_optimize(codec, true);
		break;
	case SND_SOC_DAPM_POST_PMD:
		hi6402es_mad_power_optimize(codec, false);
		hi6402es_dapm_reg_clr_bit(codec, HI6402ES_MISC_CLK_EN, HI6402ES_MAD_CLK1_EN_BIT);
		hi6402es_dapm_reg_clr_bit(codec, HI6402ES_MISC_CLK_EN, HI6402ES_MAD_CLK0_EN_BIT);
		hi6402es_mad_enable(codec, false);
		break;
	default :
		pr_warn("%s : power mode event err : %d\n", __FUNCTION__, event);
		break;
	}

	return 0;
}

int hi6402es_pll_power_mode_event(struct snd_soc_dapm_widget *w,
		struct snd_kcontrol *kcontrol, int event)
{
	struct snd_soc_codec *codec = w->codec;

	BUG_ON(NULL == codec);

	switch (event) {
	case SND_SOC_DAPM_PRE_PMU:
		hi6402es_pll_enable(codec, true);
		break;
	case SND_SOC_DAPM_POST_PMD:
		hi6402es_pll_enable(codec, false);
		break;
	default :
		pr_warn("%s : power mode event err : %d\n", __FUNCTION__, event);
		break;
	}

	return 0;
}

/* VOLUME CONTROLS */
/*
* MAIN MIC GAIN volume control:
* from 0 to 36 dB in 2 dB steps
* MAX VALUE is 18
*/
static DECLARE_TLV_DB_SCALE(main_mic_tlv, 0, 200, 0);

/*
* AUX MIC GAIN volume control:
* from 0 to 36 dB in 2 dB steps
* MAX VALUE is 18
*/
static DECLARE_TLV_DB_SCALE(aux_mic_tlv, 0, 200, 0);

/*
* LINEINR MIC GAIN volume control:
* from -20 to 36 dB in 2 dB steps
* MAX VALUE is 18
*/
static DECLARE_TLV_DB_SCALE(lineinr_mic_tlv, -2000, 200, 0);

/*
* LINEINL MIC GAIN volume control:
* from -20 to 36 dB in 2 dB steps
* MAX VALUE is 18
*/
static DECLARE_TLV_DB_SCALE(lineinl_mic_tlv, -2000, 200, 0);

/*
* LOL PGA GAIN volume control:
* from -21 to 6 dB in 1.5 dB steps
* MAX VALUE is 18
*/
static DECLARE_TLV_DB_SCALE(lol_pga_tlv, -2100, 150, 0);

/*
* LOR PGA GAIN volume control:
* from -21 to 6 dB in 1.5 dB steps
* MAX VALUE is 18
*/
static DECLARE_TLV_DB_SCALE(lor_pga_tlv, -2100, 150, 0);

/*
* HPL PGA GAIN volume control:
* from -32 to 6 dB in 1.5 dB steps
* MAX VALUE is 18
*/
//static DECLARE_TLV_DB_SCALE(hpl_pga_tlv, -3200, 100, 0);

/*
* HPR PGA GAIN volume control:
* from -32 to 6 dB in 1.5 dB steps
* MAX VALUE is 18
*/
//static DECLARE_TLV_DB_SCALE(hpr_pga_tlv, -3200, 100, 0);

/*
* EP PGA GAIN volume control:
* from -21to 6 dB in 1.5 dB steps
* MAX VALUE is 18
*/
static DECLARE_TLV_DB_SCALE(ep_pga_tlv, -2100, 150, 0);

static void hi6402es_audio_clk_enable(struct snd_soc_codec *codec, bool enable)
{
	struct hi6402es_priv *priv = snd_soc_codec_get_drvdata(codec);

	BUG_ON(NULL == priv);

	mutex_lock(&priv->dapm_mutex);
	if (enable) {
		if (0 == priv->ref_audio_clk) {
			hi6402es_dapm_reg_set_bit(codec, HI6402ES_S1_FS_CFG_L, HI6402ES_PORT_CLK_EN);
		}
		++priv->ref_audio_clk;
	} else {
		--priv->ref_audio_clk;
		if (0 == priv->ref_audio_clk) {
			hi6402es_dapm_reg_clr_bit(codec, HI6402ES_S1_FS_CFG_L, HI6402ES_PORT_CLK_EN);
		}
	}
	mutex_unlock(&priv->dapm_mutex);
}

static void hi6402es_audio_rx_clk_enable(struct snd_soc_codec *codec, bool enable)
{
	struct hi6402es_priv *priv = snd_soc_codec_get_drvdata(codec);

	BUG_ON(NULL == priv);

	mutex_lock(&priv->dapm_mutex);
	if (enable) {
		if (0 == priv->ref_audio_rx)
			hi6402es_dapm_reg_set_bit(codec, HI6402ES_S1_PORT_CFG_L, HI6402ES_IF_RX_EN_BIT);
		++priv->ref_audio_rx;
	} else {
		--priv->ref_audio_rx;
		if (0 == priv->ref_audio_rx)
			hi6402es_dapm_reg_clr_bit(codec, HI6402ES_S1_PORT_CFG_L, HI6402ES_IF_RX_EN_BIT);
	}
	mutex_unlock(&priv->dapm_mutex);
}

static void hi6402es_audio_tx_clk_enable(struct snd_soc_codec *codec, bool enable)
{
	struct hi6402es_priv *priv = snd_soc_codec_get_drvdata(codec);

	BUG_ON(NULL == priv);
	mutex_lock(&priv->dapm_mutex);
	if (enable) {
		if (0 == priv->ref_audio_tx)
			hi6402es_dapm_reg_set_bit(codec, HI6402ES_S1_PORT_CFG_L, HI6402ES_IF_TX_EN_BIT);
		++priv->ref_audio_tx;
	} else {
		--priv->ref_audio_tx;
		if (0 == priv->ref_audio_tx)
			hi6402es_dapm_reg_clr_bit(codec, HI6402ES_S1_PORT_CFG_L, HI6402ES_IF_TX_EN_BIT);
	}
	mutex_unlock(&priv->dapm_mutex);
}

static void hi6402es_voice_clk_enable(struct snd_soc_codec *codec, bool enable)
{
	struct hi6402es_priv *priv = snd_soc_codec_get_drvdata(codec);

	BUG_ON(NULL == priv);

	mutex_lock(&priv->dapm_mutex);
	if (enable) {
		if (0 == priv->ref_voice_clk) {
			hi6402es_dapm_reg_set_bit(codec, HI6402ES_S3_FS_CFG_L, HI6402ES_PORT_CLK_EN);
			hi6402es_dapm_reg_set_bit(codec, HI6402ES_S3_PORT_CFG_L, HI6402ES_IF_TX_EN_BIT);
			hi6402es_dapm_reg_set_bit(codec, HI6402ES_S3_PORT_CFG_L, HI6402ES_IF_RX_EN_BIT);
		}
		++priv->ref_voice_clk;
	} else {
		--priv->ref_voice_clk;
		if (0 == priv->ref_voice_clk) {
			hi6402es_dapm_reg_clr_bit(codec, HI6402ES_S3_PORT_CFG_L, HI6402ES_IF_RX_EN_BIT);
			hi6402es_dapm_reg_clr_bit(codec, HI6402ES_S3_PORT_CFG_L, HI6402ES_IF_TX_EN_BIT);
			hi6402es_dapm_reg_clr_bit(codec, HI6402ES_S3_FS_CFG_L, HI6402ES_PORT_CLK_EN);
		}
	}
	mutex_unlock(&priv->dapm_mutex);
}

int hi6402es_s1_tx_clk_power_mode_event(struct snd_soc_dapm_widget *w,
		struct snd_kcontrol *kcontrol, int event)
{
	struct snd_soc_codec *codec = w->codec;

	BUG_ON(NULL == codec);

	switch (event) {
	case SND_SOC_DAPM_PRE_PMU:
		hi6402es_audio_tx_clk_enable(codec, true);
		break;
	case SND_SOC_DAPM_POST_PMD:
		hi6402es_audio_tx_clk_enable(codec, false);
		break;
	default :
		pr_warn("%s : power mode event err : %d\n", __FUNCTION__, event);
		break;
	}

	return 0;

}

int hi6402es_s1_rx_clk_power_mode_event(struct snd_soc_dapm_widget *w,
		struct snd_kcontrol *kcontrol, int event)
{
	struct snd_soc_codec *codec = w->codec;

	BUG_ON(NULL == codec);

	switch (event) {
	case SND_SOC_DAPM_PRE_PMU:
		hi6402es_audio_rx_clk_enable(codec, true);
		break;
	case SND_SOC_DAPM_POST_PMD:
		hi6402es_audio_rx_clk_enable(codec, false);
		break;
	default :
		pr_warn("%s : power mode event err : %d\n", __FUNCTION__, event);
		break;
	}

	return 0;

}

int hi6402es_s1_interface_clk_power_mode_event(struct snd_soc_dapm_widget *w,
		struct snd_kcontrol *kcontrol, int event)
{
	struct snd_soc_codec *codec = w->codec;

	BUG_ON(NULL == codec);

	switch (event) {
	case SND_SOC_DAPM_PRE_PMU:
		hi6402es_audio_clk_enable(codec, true);
		break;
	case SND_SOC_DAPM_POST_PMD:
		hi6402es_audio_clk_enable(codec, false);
		break;
	default :
		pr_warn("%s : power mode event err : %d\n", __FUNCTION__, event);
		break;
	}

	return 0;

}

int hi6402es_s3_interface_clk_power_mode_event(struct snd_soc_dapm_widget *w,
		struct snd_kcontrol *kcontrol, int event)
{
	struct snd_soc_codec *codec = w->codec;

	BUG_ON(NULL == codec);

	switch (event) {
	case SND_SOC_DAPM_PRE_PMU:
		hi6402es_voice_clk_enable(codec, true);
		break;
	case SND_SOC_DAPM_POST_PMD:
		hi6402es_voice_clk_enable(codec, false);
		break;
	default :
		pr_warn("%s : power mode event err : %d\n", __FUNCTION__, event);
		break;
	}

	return 0;
}

enum hi6402es_cp_clk_div {
	HI6402ES_CP_CLK_DIV_2 = 0,
	HI6402ES_CP_CLK_DIV_4 = 1,
	HI6402ES_CP_CLK_DIV_8 = 2,
	HI6402ES_CP_CLK_DIV_16 = 3,
	HI6402ES_CP_CLK_DIV_32 = 4,
	HI6402ES_CP_CLK_DIV_64 = 5,
	HI6402ES_CP_CLK_DIV_128 = 6,
	HI6402ES_CP_CLK_DIV_256 = 7,
};

static inline void hi6402es_cp_clk_select(struct snd_soc_codec *codec, enum hi6402es_cp_clk_div div)
{
	hi6402es_dapm_reg_write_bits(codec, HI6402ES_CP_CLK_SEL_REG, div, 0x7);
}

int hi6402es_cp1_power_mode_event(struct snd_soc_dapm_widget *w,
		struct snd_kcontrol *kcontrol, int event)
{
	struct snd_soc_codec *codec = w->codec;

	BUG_ON(NULL == codec);

	switch (event) {
	case SND_SOC_DAPM_PRE_PMU:
		/* cp CP1 cfg */
		hi6402es_cp_clk_select(codec, HI6402ES_CP_CLK_DIV_8);
		break;
	case SND_SOC_DAPM_POST_PMD:
		/* cp CP1 cfg */
		hi6402es_cp_clk_select(codec, HI6402ES_CP_CLK_DIV_256);
		break;
	default :
		pr_warn("%s : event err : %d\n", __FUNCTION__, event);
		break;
	}

	return 0;
}

static void hi6402es_hp_power_on(struct snd_soc_codec *codec)
{
	unsigned int hsl_reg_value = hi6402es_dapm_reg_read(codec, HI6402ES_HPL_PGA_CFG_REG);
	unsigned int hsr_reg_value = hi6402es_dapm_reg_read(codec, HI6402ES_HPR_PGA_CFG_REG);
	unsigned int hsl_pga_value = hsl_reg_value & 0x3F;
	unsigned int hsr_pga_value = hsr_reg_value & 0x3F;
	unsigned int min_pga = (hsl_pga_value<hsr_pga_value? hsl_pga_value:hsr_pga_value);

	int i = 0;

	/* -32dB init */
	hi6402es_dapm_reg_write_bits(codec, HI6402ES_HPL_PGA_CFG_REG, 0x00, 0x3F);
	hi6402es_dapm_reg_write_bits(codec, HI6402ES_HPR_PGA_CFG_REG, 0x00, 0x3F);
	/* CP1 Normal Mode*/
	/* shutdown cp clk before clk switch*/
	hi6402es_dapm_reg_set_bit(codec, HI6402ES_POP_CLK_PD_REG,0 );
	hi6402es_dapm_reg_write_bits(codec, HI6402ES_CP_CLK_SEL_REG, 0x02, 0x07);
	hi6402es_dapm_reg_clr_bit(codec, HI6402ES_POP_CLK_PD_REG,0 );
	mdelay(1);
	
	/* power on */
	hi6402es_dapm_reg_write_bits(codec, HI6402ES_DAC_HP_PD_REG, 0x00, 0xA0);
	mdelay(65);
	hi6402es_dapm_reg_write_bits(codec, HI6402ES_DAC_HP_PD_REG, 0x00, 0x50);
	hi6402es_dapm_reg_write_bits(codec, HI6402ES_DAC_HP_PD_REG, 0x00, 0x0C);
	hi6402es_dapm_reg_write_bits(codec, HI6402ES_DAC_HP_PD_REG, 0x00, 0x03);
	
	mdelay(80);
	hi6402es_dapm_reg_clr_bit(codec, HI6402ES_HPL_PGA_CFG_REG, HI6402ES_HPL_PGA_MUTE_BIT);
	hi6402es_dapm_reg_clr_bit(codec, HI6402ES_HPR_PGA_CFG_REG, HI6402ES_HPR_PGA_MUTE_BIT);
	for (i += 1; i <= min_pga; i++){
		hi6402es_dapm_reg_write_bits(codec, HI6402ES_HPL_PGA_CFG_REG, i, 0x3F);
		//mdelay(2);
		hi6402es_dapm_reg_write_bits(codec, HI6402ES_HPR_PGA_CFG_REG, i, 0x3F);
		//mdelay(2);
	}

	/* zero cross on */
	hi6402es_dapm_reg_set_bit(codec, HI6402ES_ZCD_CFG_REG, HI6402ES_HPL_ZCD_BIT );
	hi6402es_dapm_reg_set_bit(codec, HI6402ES_ZCD_CFG_REG, HI6402ES_HPR_ZCD_BIT);
}

static void hi6402es_hp_power_off(struct snd_soc_codec *codec)
{
	unsigned int hsl_reg_value = hi6402es_dapm_reg_read(codec, HI6402ES_HPL_PGA_CFG_REG);
	unsigned int hsr_reg_value = hi6402es_dapm_reg_read(codec, HI6402ES_HPR_PGA_CFG_REG);
	unsigned int hsl_pga_value = hsl_reg_value & 0x3F;
	unsigned int hsr_pga_value = hsr_reg_value & 0x3F;
	unsigned int min_pga = (hsl_pga_value<hsr_pga_value? hsl_pga_value:hsr_pga_value);
	
	int i = min_pga;

	/* zero cross off */
	hi6402es_dapm_reg_clr_bit(codec, HI6402ES_ZCD_CFG_REG, HI6402ES_HPL_ZCD_BIT );
	hi6402es_dapm_reg_clr_bit(codec, HI6402ES_ZCD_CFG_REG, HI6402ES_HPR_ZCD_BIT);

	/* power off */
	for (i -= 1; i > 0; i--){
		hi6402es_dapm_reg_write_bits(codec, HI6402ES_HPL_PGA_CFG_REG, i, 0x3F);
		//mdelay(2);
		hi6402es_dapm_reg_write_bits(codec, HI6402ES_HPR_PGA_CFG_REG, i, 0x3F);
		//mdelay(2);
	}
	hi6402es_dapm_reg_set_bit(codec, HI6402ES_HPL_PGA_CFG_REG, HI6402ES_HPL_PGA_MUTE_BIT);
	hi6402es_dapm_reg_set_bit(codec, HI6402ES_HPR_PGA_CFG_REG, HI6402ES_HPR_PGA_MUTE_BIT);

	hi6402es_dapm_reg_write_bits(codec, HI6402ES_DAC_HP_PD_REG, 0x03, 0x03);

	mdelay(80);
	hi6402es_dapm_reg_write_bits(codec, HI6402ES_DAC_HP_PD_REG, 0x0C, 0x0C);
	hi6402es_dapm_reg_write_bits(codec, HI6402ES_DAC_HP_PD_REG, 0xF0, 0xF0);

	mdelay(50);
	/* CP1 LP PLLOn Mode*/
	/* shutdown cp clk before clk switch*/
	hi6402es_dapm_reg_set_bit(codec, HI6402ES_POP_CLK_PD_REG,0 );
	hi6402es_dapm_reg_write_bits(codec, HI6402ES_CP_CLK_SEL_REG, 0x07, 0x07);
	hi6402es_dapm_reg_clr_bit(codec, HI6402ES_POP_CLK_PD_REG,0 );

	hi6402es_dapm_reg_write_bits(codec, HI6402ES_HPL_PGA_CFG_REG, min_pga, 0x3F);
	hi6402es_dapm_reg_write_bits(codec, HI6402ES_HPR_PGA_CFG_REG, min_pga, 0x3F);
}

int hi6402es_hpl_drv_power_mode_event(struct snd_soc_dapm_widget *w,
		struct snd_kcontrol *kcontrol, int event)
{
	struct snd_soc_codec *codec = w->codec;
	struct hi6402es_priv *priv = NULL;

	BUG_ON(NULL == codec);
	priv = snd_soc_codec_get_drvdata(codec);

	BUG_ON(NULL == priv);

	switch (event) {
	case SND_SOC_DAPM_PRE_PMU:
		priv->hsl_power_on = true;
		if (priv->hsl_power_on && priv->hsr_power_on)
			hi6402es_hp_power_on(codec);
		break;
	case SND_SOC_DAPM_POST_PMD:

		if (priv->hsl_power_on && priv->hsr_power_on)
			hi6402es_hp_power_off(codec);
		priv->hsl_power_on = false;
		break;
	default :
		pr_warn("%s : event err : %d\n", __FUNCTION__, event);
		break;
	}

	return 0;
}

int hi6402es_hpr_drv_power_mode_event(struct snd_soc_dapm_widget *w,
		struct snd_kcontrol *kcontrol, int event)
{
	struct snd_soc_codec *codec = w->codec;
	struct hi6402es_priv *priv = NULL;

	BUG_ON(NULL == codec);
	priv = snd_soc_codec_get_drvdata(codec);

	BUG_ON(NULL == priv);

	switch (event) {
	case SND_SOC_DAPM_PRE_PMU:
		/* hpr dac on */

		priv->hsr_power_on = true;
		if (priv->hsl_power_on && priv->hsr_power_on)
			hi6402es_hp_power_on(codec);
		break;
	case SND_SOC_DAPM_POST_PMD:

		if (priv->hsl_power_on && priv->hsr_power_on)
			hi6402es_hp_power_off(codec);
		priv->hsr_power_on = false;
		break;
	default :
		pr_warn("%s : event err : %d\n", __FUNCTION__, event);
		break;
	}

	return 0;
}

int hi6402es_ep_drv_power_mode_event(struct snd_soc_dapm_widget *w,
		struct snd_kcontrol *kcontrol, int event)
{
	struct snd_soc_codec *codec = w->codec;

	BUG_ON(NULL == codec);

	switch (event) {
	case SND_SOC_DAPM_PRE_PMU:
		/* ep dac on */
		hi6402es_dapm_reg_clr_bit(codec, HI6402ES_DAC_EP_LO_PD_REG, HI6402ES_DAC_EP_PD_BIT);
		/* ep pga on */
		hi6402es_dapm_reg_clr_bit(codec, HI6402ES_DAC_EP_LO_PD_REG, HI6402ES_EP_PD_BIT);
		/* ep mute off */
		hi6402es_dapm_reg_clr_bit(codec, HI6402ES_EP_PGA_CFG_REG, HI6402ES_EP_PGA_MUTE_BIT);
		break;
	case SND_SOC_DAPM_POST_PMD:
		/* ep mute on */
		hi6402es_dapm_reg_set_bit(codec, HI6402ES_EP_PGA_CFG_REG, HI6402ES_EP_PGA_MUTE_BIT);
		/* ep pga on */
		hi6402es_dapm_reg_set_bit(codec, HI6402ES_DAC_EP_LO_PD_REG, HI6402ES_EP_PD_BIT);
		/* ep dac pb */
		hi6402es_dapm_reg_set_bit(codec, HI6402ES_DAC_EP_LO_PD_REG, HI6402ES_DAC_EP_PD_BIT);
		break;
	default :
		pr_warn("%s : event err : %d\n", __FUNCTION__, event);
		break;
	}

	return 0;
}

int hi6402es_lol_drv_power_mode_event(struct snd_soc_dapm_widget *w,
		struct snd_kcontrol *kcontrol, int event)
{
	struct snd_soc_codec *codec = w->codec;

	BUG_ON(NULL == codec);

	switch (event) {
	case SND_SOC_DAPM_PRE_PMU:
		/* lol dac on */
		hi6402es_dapm_reg_clr_bit(codec, HI6402ES_DAC_EP_LO_PD_REG, HI6402ES_DAC_LOL_PD_BIT);
		/* lol pga on */
		hi6402es_dapm_reg_clr_bit(codec, HI6402ES_DAC_EP_LO_PD_REG, HI6402ES_LOL_PD_BIT);
		/* lol mute off */
		hi6402es_dapm_reg_clr_bit(codec, HI6402ES_LOL_PGA_CFG_REG, HI6402ES_LOL_PGA_MUTE_BIT);
		break;
	case SND_SOC_DAPM_POST_PMD:
		/* lol mute off */
		hi6402es_dapm_reg_set_bit(codec, HI6402ES_LOL_PGA_CFG_REG, HI6402ES_LOL_PGA_MUTE_BIT);
		/* lol pga on */
		hi6402es_dapm_reg_set_bit(codec, HI6402ES_DAC_EP_LO_PD_REG, HI6402ES_LOL_PD_BIT);
		/* lol dac pb */
		hi6402es_dapm_reg_set_bit(codec, HI6402ES_DAC_EP_LO_PD_REG, HI6402ES_DAC_LOL_PD_BIT);
		break;
	default :
		pr_warn("%s : event err : %d\n", __FUNCTION__, event);
		break;
	}

	return 0;
}

int hi6402es_lor_drv_power_mode_event(struct snd_soc_dapm_widget *w,
		struct snd_kcontrol *kcontrol, int event)
{
	struct snd_soc_codec *codec = w->codec;

	BUG_ON(NULL == codec);

	switch (event) {
	case SND_SOC_DAPM_PRE_PMU:
		/* lor dac on */
		hi6402es_dapm_reg_clr_bit(codec, HI6402ES_DAC_EP_LO_PD_REG, HI6402ES_DAC_LOR_PD_BIT);
		/* lor pga on */
		hi6402es_dapm_reg_clr_bit(codec, HI6402ES_DAC_EP_LO_PD_REG, HI6402ES_LOR_PD_BIT);
		/* lor mute off */
		hi6402es_dapm_reg_clr_bit(codec, HI6402ES_LOR_PGA_CFG_REG, HI6402ES_LOR_PGA_MUTE_BIT);
		break;
	case SND_SOC_DAPM_POST_PMD:
		/* lor mute off */
		hi6402es_dapm_reg_set_bit(codec, HI6402ES_LOR_PGA_CFG_REG, HI6402ES_LOR_PGA_MUTE_BIT);
		/* lor pga on */
		hi6402es_dapm_reg_set_bit(codec, HI6402ES_DAC_EP_LO_PD_REG, HI6402ES_LOR_PD_BIT);
		/* lor dac pb */
		hi6402es_dapm_reg_set_bit(codec, HI6402ES_DAC_EP_LO_PD_REG, HI6402ES_DAC_LOR_PD_BIT);
		break;
	default :
		pr_warn("%s : event err : %d\n", __FUNCTION__, event);
		break;
	}

	return 0;
}

int hi6402es_mainmic_pga_power_mode_event(struct snd_soc_dapm_widget *w,
		struct snd_kcontrol *kcontrol, int event)
{
	struct snd_soc_codec *codec = w->codec;

	BUG_ON(NULL == codec);

	switch (event) {
	case SND_SOC_DAPM_PRE_PMU:
		/* main pga mute1 */
		hi6402es_dapm_reg_clr_bit(codec, HI6402ES_ADC_PGA_MUTE1_SEL_REG, HI6402ES_MAIN_PGA_MUTE1_BIT);
		/* main pga mute2 */
		hi6402es_dapm_reg_clr_bit(codec, HI6402ES_MAIN_PGA_SEL_REG, HI6402ES_MAIN_PGA_MUTE2_BIT);
		/* main pga on */
		hi6402es_dapm_reg_clr_bit(codec, HI6402ES_ADC_PD_REG, HI6402ES_MAINPGA_PD_BIT);
		/* main pga unlock bypass */
		//hi6402es_dapm_reg_set_bit(codec, HI6402ES_ADC_PGA_MUTE1_SEL_REG, HI6402ES_MAIN_PGA_MUTE_UNLOCK_BIT);
		/* adc0l on */
		hi6402es_dapm_reg_clr_bit(codec, HI6402ES_ADC_PD_REG, HI6402ES_ADC0L_PD_BIT);
		break;
	case SND_SOC_DAPM_POST_PMD:
		/* adc0l pd */
		hi6402es_dapm_reg_set_bit(codec, HI6402ES_ADC_PD_REG, HI6402ES_ADC0L_PD_BIT);
		/* main pga unlock bypass */
		//hi6402es_dapm_reg_clr_bit(codec, HI6402ES_ADC_PGA_MUTE1_SEL_REG, HI6402ES_MAIN_PGA_MUTE_UNLOCK_BIT);
		/* main pga pd */
		hi6402es_dapm_reg_set_bit(codec, HI6402ES_ADC_PD_REG, HI6402ES_MAINPGA_PD_BIT);
		/* main pga mute2 */
		hi6402es_dapm_reg_set_bit(codec, HI6402ES_MAIN_PGA_SEL_REG, HI6402ES_MAIN_PGA_MUTE2_BIT);
		/* main pga mute1 */
		hi6402es_dapm_reg_set_bit(codec, HI6402ES_ADC_PGA_MUTE1_SEL_REG, HI6402ES_MAIN_PGA_MUTE1_BIT);
		break;
	default :
		pr_warn("%s : event err : %d\n", __FUNCTION__, event);
		break;
	}

	return 0;
}
int hi6402es_auxmic_pga_power_mode_event(struct snd_soc_dapm_widget *w,
		struct snd_kcontrol *kcontrol, int event)
{
	struct snd_soc_codec *codec = w->codec;

	BUG_ON(NULL == codec);

	switch (event) {
	case SND_SOC_DAPM_PRE_PMU:
		/* main pga mute1 */
		hi6402es_dapm_reg_clr_bit(codec, HI6402ES_AUX_PGA_SEL_REG, HI6402ES_AUX_PGA_MUTE1_BIT);
		/* main pga mute2 */
		hi6402es_dapm_reg_clr_bit(codec, HI6402ES_AUX_PGA_SEL_REG, HI6402ES_AUX_PGA_MUTE2_BIT);
		/* aux pga on */
		hi6402es_dapm_reg_clr_bit(codec, HI6402ES_ADC_PD_REG, HI6402ES_AUXPGA_PD_BIT);
		/* aux pga unlock bypass */
		//hi6402es_dapm_reg_set_bit(codec, HI6402ES_AUX_PGA_SEL_REG, HI6402ES_AUX_PGA_MUTE_UNLOCK_BIT);
		/* adc0r on */
		hi6402es_dapm_reg_clr_bit(codec, HI6402ES_ADC_PD_REG, HI6402ES_ADC0R_PD_BIT);
		break;
	case SND_SOC_DAPM_POST_PMD:
		/* adc0r pd */
		hi6402es_dapm_reg_set_bit(codec, HI6402ES_ADC_PD_REG, HI6402ES_ADC0R_PD_BIT);
		/* aux pga unlock bypass */
		//hi6402es_dapm_reg_clr_bit(codec, HI6402ES_AUX_PGA_SEL_REG, HI6402ES_AUX_PGA_MUTE_UNLOCK_BIT);
		/* aux pga pd */
		hi6402es_dapm_reg_set_bit(codec, HI6402ES_ADC_PD_REG, HI6402ES_AUXPGA_PD_BIT);
		/* main pga mute2 */
		hi6402es_dapm_reg_set_bit(codec, HI6402ES_AUX_PGA_SEL_REG, HI6402ES_AUX_PGA_MUTE2_BIT);
		/* main pga mute1 */
		hi6402es_dapm_reg_set_bit(codec, HI6402ES_AUX_PGA_SEL_REG, HI6402ES_AUX_PGA_MUTE1_BIT);
		break;
	default :
		pr_warn("%s : event err : %d\n", __FUNCTION__, event);
		break;
	}

	return 0;
}
int hi6402es_lineinl_pga_power_mode_event(struct snd_soc_dapm_widget *w,
		struct snd_kcontrol *kcontrol, int event)
{
	struct snd_soc_codec *codec = w->codec;

	BUG_ON(NULL == codec);

	switch (event) {
	case SND_SOC_DAPM_PRE_PMU:
		/* lineinl mute1 */
		hi6402es_dapm_reg_clr_bit(codec, HI6402ES_ADC_PGA_MUTE1_SEL_REG, HI6402ES_LINEL_PGA_MUTE1_BIT);
		/* lineinl mute2 */
		hi6402es_dapm_reg_clr_bit(codec, HI6402ES_LINEL_PGA_SEL_REG, HI6402ES_LINEL_PGA_MUTE2_BIT);
		/* lineinl pga on */
		hi6402es_dapm_reg_clr_bit(codec, HI6402ES_ADC_PD_REG, HI6402ES_LINEPGAL_PD_BIT);
		/* adc1l on */
		hi6402es_dapm_reg_clr_bit(codec, HI6402ES_ADC_PD_REG, HI6402ES_ADC1L_PD_BIT);
		break;
	case SND_SOC_DAPM_POST_PMD:
		/* adc1l pd */
		hi6402es_dapm_reg_set_bit(codec, HI6402ES_ADC_PD_REG, HI6402ES_ADC1L_PD_BIT);
		/* lineinl pga pd */
		hi6402es_dapm_reg_set_bit(codec, HI6402ES_ADC_PD_REG, HI6402ES_LINEPGAL_PD_BIT);
		/* lineinl mute2 */
		hi6402es_dapm_reg_set_bit(codec, HI6402ES_LINEL_PGA_SEL_REG, HI6402ES_LINEL_PGA_MUTE2_BIT);
		/* lineinl mute1 */
		hi6402es_dapm_reg_set_bit(codec, HI6402ES_ADC_PGA_MUTE1_SEL_REG, HI6402ES_LINEL_PGA_MUTE1_BIT);
		break;
	default :
		pr_warn("%s : event err : %d\n", __FUNCTION__, event);
		break;
	}

	return 0;
}
int hi6402es_lineinr_pga_power_mode_event(struct snd_soc_dapm_widget *w,
		struct snd_kcontrol *kcontrol, int event)
{
	struct snd_soc_codec *codec = w->codec;

	BUG_ON(NULL == codec);

	switch (event) {
	case SND_SOC_DAPM_PRE_PMU:
		/* lineinr mute1 */
		hi6402es_dapm_reg_clr_bit(codec, HI6402ES_ADC_PGA_MUTE1_SEL_REG, HI6402ES_LINER_PGA_MUTE1_BIT);
		/* lineinr mute2 */
		hi6402es_dapm_reg_clr_bit(codec, HI6402ES_LINER_PGA_SEL_REG, HI6402ES_LINER_PGA_MUTE2_BIT);
		/* lineinr pga on */
		hi6402es_dapm_reg_clr_bit(codec, HI6402ES_ADC_PD_REG, HI6402ES_LINEPGAR_PD_BIT);
		/* adc1r on */
		hi6402es_dapm_reg_clr_bit(codec, HI6402ES_ADC_PD_REG, HI6402ES_ADC1R_PD_BIT);
		break;
	case SND_SOC_DAPM_POST_PMD:
		/* adc1rl pd */
		hi6402es_dapm_reg_set_bit(codec, HI6402ES_ADC_PD_REG, HI6402ES_ADC1R_PD_BIT);
		/* lineinr pga pd */
		hi6402es_dapm_reg_set_bit(codec, HI6402ES_ADC_PD_REG, HI6402ES_LINEPGAR_PD_BIT);
		/* lineinr mute2 */
		hi6402es_dapm_reg_set_bit(codec, HI6402ES_LINER_PGA_SEL_REG, HI6402ES_LINER_PGA_MUTE2_BIT);
		/* lineinr mute1 */
		hi6402es_dapm_reg_set_bit(codec, HI6402ES_ADC_PGA_MUTE1_SEL_REG, HI6402ES_LINER_PGA_MUTE1_BIT);
		break;
	default :
		pr_warn("%s : event err : %d\n", __FUNCTION__, event);
		break;
	}

	return 0;
}
int hi6402es_mad_src_power_mode_event(struct snd_soc_dapm_widget *w,
		struct snd_kcontrol *kcontrol, int event)
{
	struct snd_soc_codec *codec = w->codec;

	BUG_ON(NULL == codec);

	switch (event) {
	case SND_SOC_DAPM_PRE_PMU:
		/* s1_ol_src_cfg 96KHz to 16KHz */
		hi6402es_dapm_reg_set_bit(codec, HI6402ES_S1_FS_CFG_L, HI6402ES_OL_SRC_FS_CFG);
		hi6402es_dapm_reg_write_bits(codec, HI6402ES_S1_SRC_OUT_MODE_CGF, 0x2<<HI6402ES_S1_SRC_OL_MODE_BIT,
			0x3<<HI6402ES_S1_SRC_OL_MODE_BIT);
		break;
	case SND_SOC_DAPM_POST_PMD:
		/* s1_ol_src_cfg 48KHz to 48KHz */
		hi6402es_dapm_reg_clr_bit(codec, HI6402ES_S1_FS_CFG_L, HI6402ES_OL_SRC_FS_CFG);
		break;
	default :
		pr_warn("%s : event err : %d\n", __FUNCTION__, event);
		break;
	}

	return 0;
}
int hi6402es_s1_il_pga_power_mode_event(struct snd_soc_dapm_widget *w,
		struct snd_kcontrol *kcontrol, int event)
{
	struct snd_soc_codec *codec = w->codec;

	BUG_ON(NULL == codec);

	switch (event) {
	case SND_SOC_DAPM_PRE_PMU:
		/* s1_il_pga_clk en */
		hi6402es_dapm_reg_set_bit(codec, HI6402ES_S1_MODULE_CLK_REG, HI6402ES_S1_PGA_IL_EN_BIT);
		break;
	case SND_SOC_DAPM_POST_PMD:
		/* s1_il_pga_clk disable */
		hi6402es_dapm_reg_clr_bit(codec, HI6402ES_S1_MODULE_CLK_REG, HI6402ES_S1_PGA_IL_EN_BIT);
		break;
	default :
		pr_warn("%s : event err : %d\n", __FUNCTION__, event);
		break;
	}

	return 0;
}
int hi6402es_s1_ir_pga_power_mode_event(struct snd_soc_dapm_widget *w,
		struct snd_kcontrol *kcontrol, int event)
{	
	struct snd_soc_codec *codec = w->codec;

	BUG_ON(NULL == codec);

	switch (event) {
	case SND_SOC_DAPM_PRE_PMU:
		/* s1_irl_pga_clk en */
		hi6402es_dapm_reg_set_bit(codec, HI6402ES_S1_MODULE_CLK_REG, HI6402ES_S2_PGA_IR_EN_BIT);
		break;
	case SND_SOC_DAPM_POST_PMD:
		/* s1_ir_pga_clk disable */
		hi6402es_dapm_reg_clr_bit(codec, HI6402ES_S1_MODULE_CLK_REG, HI6402ES_S2_PGA_IR_EN_BIT);
		break;
	default :
		pr_warn("%s : event err : %d\n", __FUNCTION__, event);
		break;
	}

	return 0;
}

int hi6402es_s2_il_pga_power_mode_event(struct snd_soc_dapm_widget *w,
		struct snd_kcontrol *kcontrol, int event)
{
	struct snd_soc_codec *codec = w->codec;

	BUG_ON(NULL == codec);

	switch (event) {
	case SND_SOC_DAPM_PRE_PMU:
		/* s2_il_pga_clk en */
		hi6402es_dapm_reg_set_bit(codec, HI6402ES_S2_MODULE_CLK_REG, HI6402ES_S2_PGA_IL_EN_BIT);
		break;
	case SND_SOC_DAPM_POST_PMD:
		/* s2_il_pga_clk disable */
		hi6402es_dapm_reg_clr_bit(codec, HI6402ES_S2_MODULE_CLK_REG, HI6402ES_S2_PGA_IL_EN_BIT);
		break;
	default :
		pr_warn("%s : event err : %d\n", __FUNCTION__, event);
		break;
	}

	return 0;
}

int hi6402es_s2_ir_pga_power_mode_event(struct snd_soc_dapm_widget *w,
		struct snd_kcontrol *kcontrol, int event)
{
	struct snd_soc_codec *codec = w->codec;

	BUG_ON(NULL == codec);

	switch (event) {
	case SND_SOC_DAPM_PRE_PMU:
		/* s2_ir_pga_clk en */
		hi6402es_dapm_reg_set_bit(codec, HI6402ES_S2_MODULE_CLK_REG, HI6402ES_S2_PGA_IR_EN_BIT);
		break;
	case SND_SOC_DAPM_POST_PMD:
		/* s2_ir_pga_clk disable */
		hi6402es_dapm_reg_clr_bit(codec, HI6402ES_S2_MODULE_CLK_REG, HI6402ES_S2_PGA_IR_EN_BIT);
		break;
	default :
		pr_warn("%s : event err : %d\n", __FUNCTION__, event);
		break;
	}

	return 0;
}
int hi6402es_s3_il_pga_power_mode_event(struct snd_soc_dapm_widget *w,
		struct snd_kcontrol *kcontrol, int event)
{
	struct snd_soc_codec *codec = w->codec;

	BUG_ON(NULL == codec);

	switch (event) {
	case SND_SOC_DAPM_PRE_PMU:
		/* s3_il_pga_clk en */
		hi6402es_dapm_reg_set_bit(codec, HI6402ES_S3_MODULE_CLK_REG, HI6402ES_S3_PGA_IL_EN_BIT);
		break;
	case SND_SOC_DAPM_POST_PMD:
		/* s3_il_pga_clk disable */
		hi6402es_dapm_reg_clr_bit(codec, HI6402ES_S3_MODULE_CLK_REG, HI6402ES_S3_PGA_IL_EN_BIT);
		break;
	default :
		pr_warn("%s : event err : %d\n", __FUNCTION__, event);
		break;
	}

	return 0;
}
int hi6402es_s3_ir_pga_power_mode_event(struct snd_soc_dapm_widget *w,
		struct snd_kcontrol *kcontrol, int event)
{
	struct snd_soc_codec *codec = w->codec;

	BUG_ON(NULL == codec);

	switch (event) {
	case SND_SOC_DAPM_PRE_PMU:
		/* s3_ir_pga_clk en */
		hi6402es_dapm_reg_set_bit(codec, HI6402ES_S3_MODULE_CLK_REG, HI6402ES_S3_PGA_IR_EN_BIT);
		break;
	case SND_SOC_DAPM_POST_PMD:
		/* s3_ir_pga_clk disable */
		hi6402es_dapm_reg_clr_bit(codec, HI6402ES_S3_MODULE_CLK_REG, HI6402ES_S3_PGA_IR_EN_BIT);
		break;
	default :
		pr_warn("%s : event err : %d\n", __FUNCTION__, event);
		break;
	}

	return 0;
}
int hi6402es_s4_il_pga_power_mode_event(struct snd_soc_dapm_widget *w,
		struct snd_kcontrol *kcontrol, int event)
{
	struct snd_soc_codec *codec = w->codec;

	BUG_ON(NULL == codec);

	switch (event) {
	case SND_SOC_DAPM_PRE_PMU:
		/* s4_il_pga_clk en */
		hi6402es_dapm_reg_set_bit(codec, HI6402ES_S4_MODULE_CLK_REG, HI6402ES_S4_PGA_IL_EN_BIT);
		break;
	case SND_SOC_DAPM_POST_PMD:
		/* s4_il_pga_clk disable */
		hi6402es_dapm_reg_clr_bit(codec, HI6402ES_S4_MODULE_CLK_REG, HI6402ES_S4_PGA_IL_EN_BIT);
		break;
	default :
		pr_warn("%s : event err : %d\n", __FUNCTION__, event);
		break;
	}

	return 0;
}
int hi6402es_s4_ir_pga_power_mode_event(struct snd_soc_dapm_widget *w,
		struct snd_kcontrol *kcontrol, int event)
{
	struct snd_soc_codec *codec = w->codec;

	BUG_ON(NULL == codec);

	switch (event) {
	case SND_SOC_DAPM_PRE_PMU:
		/* s4_ir_pga_clk en */
		hi6402es_dapm_reg_set_bit(codec, HI6402ES_S4_MODULE_CLK_REG, HI6402ES_S4_PGA_IR_EN_BIT);
		break;
	case SND_SOC_DAPM_POST_PMD:
		/* s4_ir_pga_clk disable */
		hi6402es_dapm_reg_clr_bit(codec, HI6402ES_S4_MODULE_CLK_REG, HI6402ES_S4_PGA_IR_EN_BIT);
		break;
	default :
		pr_warn("%s : event err : %d\n", __FUNCTION__, event);
		break;
	}

	return 0;
}

int hi6402es_s1_ol_switch_power_mode_event(struct snd_soc_dapm_widget *w,
		struct snd_kcontrol *kcontrol, int event)
{
	struct snd_soc_codec *codec = w->codec;

	BUG_ON(NULL == codec);

	switch (event) {
	case SND_SOC_DAPM_PRE_PMU:
		/* s1 ol pga clk enable */
		//hi6402es_dapm_reg_set_bit(codec, HI6402ES_S1_MODULE_CLK_REG, HI6402ES_S1_PGA_OL_EN_BIT);
		break;
	case SND_SOC_DAPM_POST_PMD:
		/* s1 ol pga clk disable */
		//hi6402es_dapm_reg_clr_bit(codec, HI6402ES_S1_MODULE_CLK_REG, HI6402ES_S1_PGA_OL_EN_BIT);
		break;
	default :
		pr_warn("%s : event err : %d\n", __FUNCTION__, event);
		break;
	}

	return 0;
}
int hi6402es_s1_or_switch_power_mode_event(struct snd_soc_dapm_widget *w,
		struct snd_kcontrol *kcontrol, int event)
{
	struct snd_soc_codec *codec = w->codec;

	BUG_ON(NULL == codec);

	switch (event) {
	case SND_SOC_DAPM_PRE_PMU:
		/* s1 or pga clk enable */
		//hi6402es_dapm_reg_set_bit(codec, HI6402ES_S1_MODULE_CLK_REG, HI6402ES_S1_PGA_OR_EN_BIT);
		break;
	case SND_SOC_DAPM_POST_PMD:
		/* s1 or pga clk disable */
		//hi6402es_dapm_reg_clr_bit(codec, HI6402ES_S1_MODULE_CLK_REG, HI6402ES_S1_PGA_OR_EN_BIT);
		break;
	default :
		pr_warn("%s : event err : %d\n", __FUNCTION__, event);
		break;
	}

	return 0;
}
int hi6402es_s2_ol_switch_power_mode_event(struct snd_soc_dapm_widget *w,
		struct snd_kcontrol *kcontrol, int event)
{
	struct snd_soc_codec *codec = w->codec;

	BUG_ON(NULL == codec);

	switch (event) {
	case SND_SOC_DAPM_PRE_PMU:
		/* s2 ol pga clk enable */
		//hi6402es_dapm_reg_set_bit(codec, HI6402ES_S2_MODULE_CLK_REG, HI6402ES_S2_PGA_OL_EN_BIT);
		break;
	case SND_SOC_DAPM_POST_PMD:
		/* s2 ol pga clk disable */
		//hi6402es_dapm_reg_clr_bit(codec, HI6402ES_S2_MODULE_CLK_REG, HI6402ES_S2_PGA_OL_EN_BIT);
		break;
	default :
		pr_warn("%s : event err : %d\n", __FUNCTION__, event);
		break;
	}

	return 0;
}
int hi6402es_s2_or_switch_power_mode_event(struct snd_soc_dapm_widget *w,
		struct snd_kcontrol *kcontrol, int event)
{
	struct snd_soc_codec *codec = w->codec;

	BUG_ON(NULL == codec);

	switch (event) {
	case SND_SOC_DAPM_PRE_PMU:
		/* s2 or pga clk enable */
		//hi6402es_dapm_reg_set_bit(codec, HI6402ES_S2_MODULE_CLK_REG, HI6402ES_S2_PGA_OR_EN_BIT);
		break;
	case SND_SOC_DAPM_POST_PMD:
		/* s2 or pga clk disable */
		//hi6402es_dapm_reg_clr_bit(codec, HI6402ES_S2_MODULE_CLK_REG, HI6402ES_S2_PGA_OR_EN_BIT);
		break;
	default :
		pr_warn("%s : event err : %d\n", __FUNCTION__, event);
		break;
	}

	return 0;
}
int hi6402es_s3_ol_switch_power_mode_event(struct snd_soc_dapm_widget *w,
		struct snd_kcontrol *kcontrol, int event)
{
	struct snd_soc_codec *codec = w->codec;

	BUG_ON(NULL == codec);

	switch (event) {
	case SND_SOC_DAPM_PRE_PMU:
		/* s3 ol pga clk enable */
		//hi6402es_dapm_reg_set_bit(codec, HI6402ES_S3_MODULE_CLK_REG, HI6402ES_S3_PGA_OL_EN_BIT);
		break;
	case SND_SOC_DAPM_POST_PMD:
		/* s3 ol pga clk disable */
		//hi6402es_dapm_reg_clr_bit(codec, HI6402ES_S3_MODULE_CLK_REG, HI6402ES_S3_PGA_OL_EN_BIT);
		break;
	default :
		pr_warn("%s : event err : %d\n", __FUNCTION__, event);
		break;
	}

	return 0;
}
int hi6402es_s3_or_switch_power_mode_event(struct snd_soc_dapm_widget *w,
		struct snd_kcontrol *kcontrol, int event)
{
	struct snd_soc_codec *codec = w->codec;

	BUG_ON(NULL == codec);

	switch (event) {
	case SND_SOC_DAPM_PRE_PMU:
		/* s3 or pga clk enable */
		//hi6402es_dapm_reg_set_bit(codec, HI6402ES_S3_MODULE_CLK_REG, HI6402ES_S3_PGA_OR_EN_BIT);
		break;
	case SND_SOC_DAPM_POST_PMD:
		/* s3 or pga clk disable */
		//hi6402es_dapm_reg_clr_bit(codec, HI6402ES_S3_MODULE_CLK_REG, HI6402ES_S3_PGA_OR_EN_BIT);
		break;
	default :
		pr_warn("%s : event err : %d\n", __FUNCTION__, event);
		break;
	}

	return 0;
}
int hi6402es_s4_ol_switch_power_mode_event(struct snd_soc_dapm_widget *w,
		struct snd_kcontrol *kcontrol, int event)
{
	struct snd_soc_codec *codec = w->codec;

	BUG_ON(NULL == codec);

	switch (event) {
	case SND_SOC_DAPM_PRE_PMU:
		/* s4 ol pga clk enable */
		//hi6402es_dapm_reg_set_bit(codec, HI6402ES_S4_MODULE_CLK_REG, HI6402ES_S4_PGA_OL_EN_BIT);
		break;
	case SND_SOC_DAPM_POST_PMD:
		/* s4 ol pga clk disable */
		//hi6402es_dapm_reg_clr_bit(codec, HI6402ES_S4_MODULE_CLK_REG, HI6402ES_S4_PGA_OL_EN_BIT);
		break;
	default :
		pr_warn("%s : event err : %d\n", __FUNCTION__, event);
		break;
	}

	return 0;
}
int hi6402es_s4_or_switch_power_mode_event(struct snd_soc_dapm_widget *w,
		struct snd_kcontrol *kcontrol, int event)
{
	struct snd_soc_codec *codec = w->codec;

	BUG_ON(NULL == codec);

	switch (event) {
	case SND_SOC_DAPM_PRE_PMU:
		/* s4 or pga clk enable */
		//hi6402es_dapm_reg_set_bit(codec, HI6402ES_S4_MODULE_CLK_REG, HI6402ES_S4_PGA_OR_EN_BIT);
		break;
	case SND_SOC_DAPM_POST_PMD:
		/* s4 or pga clk disable */
		//hi6402es_dapm_reg_clr_bit(codec, HI6402ES_S4_MODULE_CLK_REG, HI6402ES_S4_PGA_OR_EN_BIT);
		break;
	default :
		pr_warn("%s : event err : %d\n", __FUNCTION__, event);
		break;
	}

	return 0;
}

int hi6402es_dacl_pga_power_mode_event(struct snd_soc_dapm_widget *w,
		struct snd_kcontrol *kcontrol, int event)
{
	struct snd_soc_codec *codec = w->codec;

	BUG_ON(NULL == codec);

	switch (event) {
	case SND_SOC_DAPM_PRE_PMU:
		/* dacl src clk enable */
		hi6402es_dapm_reg_set_bit(codec, HI6402ES_DAC_SRC_CLK_REG, HI6402ES_DACL_SRC_EN_BIT);
		/* dacl clk enable */
		hi6402es_dapm_reg_set_bit(codec, HI6402ES_DAC_ADC_CLK_REG, HI6402ES_DACL_EN_BIT);
		/* dacl pga clk enable */
		hi6402es_dapm_reg_set_bit(codec, HI6402ES_DAC_PGA_CLK_REG, HI6402ES_DACL_PGA_EN_BIT);
		/* dacl src lk enable */
		hi6402es_dapm_reg_set_bit(codec, HI6402ES_DAC_SRC_CLK_REG, HI6402ES_DACL_SRC_EN_BIT);
		/* dacl src up16 clk enable */
		hi6402es_dapm_reg_set_bit(codec, HI6402ES_EQ_SM_CLK_REG, HI6402ES_SRC_UP16L_EN_BIT);
		/* dacl sdm clk enable */
		hi6402es_dapm_reg_set_bit(codec, HI6402ES_SDM_CLK_REG, HI6402ES_DACL_SDM_EN_BIT);
		/* dacl sdm enable */
		hi6402es_dapm_reg_set_bit(codec, HI6402ES_SDM_DAC_REG, HI6402ES_SDM_DACL_EN_BIT);
		break;
	case SND_SOC_DAPM_POST_PMD:
		/* dacl sdm disable */
		hi6402es_dapm_reg_clr_bit(codec, HI6402ES_SDM_DAC_REG, HI6402ES_SDM_DACL_EN_BIT);
		/* dacl sdm clk disable */
		hi6402es_dapm_reg_clr_bit(codec, HI6402ES_SDM_CLK_REG, HI6402ES_DACL_SDM_EN_BIT);
		/* dacl src up16 clk disable */
		hi6402es_dapm_reg_clr_bit(codec, HI6402ES_EQ_SM_CLK_REG, HI6402ES_SRC_UP16L_EN_BIT);
		/* dacl src lk disable */
		hi6402es_dapm_reg_clr_bit(codec, HI6402ES_DAC_SRC_CLK_REG, HI6402ES_DACR_SRC_EN_BIT);
		/* dacl pga clk disable */
		hi6402es_dapm_reg_clr_bit(codec, HI6402ES_DAC_PGA_CLK_REG, HI6402ES_DACL_PGA_EN_BIT);
		/* dacl clk disable */
		hi6402es_dapm_reg_clr_bit(codec, HI6402ES_DAC_ADC_CLK_REG, HI6402ES_DACL_EN_BIT);
		/* dacl src clk disable */
		hi6402es_dapm_reg_clr_bit(codec, HI6402ES_DAC_SRC_CLK_REG, HI6402ES_DACL_SRC_EN_BIT);
		break;
	default :
		pr_warn("%s : event err : %d\n", __FUNCTION__, event);
		break;
	}

	return 0;
}

int hi6402es_dacr_pga_power_mode_event(struct snd_soc_dapm_widget *w,
		struct snd_kcontrol *kcontrol, int event)
{
	struct snd_soc_codec *codec = w->codec;

	BUG_ON(NULL == codec);

	switch (event) {
	case SND_SOC_DAPM_PRE_PMU:
		/* dacr src clk enable */
		hi6402es_dapm_reg_set_bit(codec, HI6402ES_DAC_SRC_CLK_REG, HI6402ES_DACR_SRC_EN_BIT);
		/* dacr clk enable */
		hi6402es_dapm_reg_set_bit(codec, HI6402ES_DAC_ADC_CLK_REG, HI6402ES_DACR_EN_BIT);
		/* dacr pga clk enable */
		hi6402es_dapm_reg_set_bit(codec, HI6402ES_DAC_PGA_CLK_REG, HI6402ES_DACR_PGA_EN_BIT);
		/* dacr src lk enable */
		hi6402es_dapm_reg_set_bit(codec, HI6402ES_DAC_SRC_CLK_REG, HI6402ES_DACR_SRC_EN_BIT);
		/* dacr src up16 clk enable */
		hi6402es_dapm_reg_set_bit(codec, HI6402ES_EQ_SM_CLK_REG, HI6402ES_SRC_UP16R_EN_BIT);
		/* dacr sdm clk enable */
		hi6402es_dapm_reg_set_bit(codec, HI6402ES_SDM_CLK_REG, HI6402ES_DACR_SDM_EN_BIT);
		/* dacr sdm enable */
		hi6402es_dapm_reg_set_bit(codec, HI6402ES_SDM_DAC_REG, HI6402ES_SDM_DACR_EN_BIT);
		break;
	case SND_SOC_DAPM_POST_PMD:
		/* dacr sdm disable */
		hi6402es_dapm_reg_clr_bit(codec, HI6402ES_SDM_DAC_REG, HI6402ES_SDM_DACR_EN_BIT);
		/* dacr sdm clk disable */
		hi6402es_dapm_reg_clr_bit(codec, HI6402ES_SDM_CLK_REG, HI6402ES_DACR_SDM_EN_BIT);
		/* dacr src up16 clk disable */
		hi6402es_dapm_reg_clr_bit(codec, HI6402ES_EQ_SM_CLK_REG, HI6402ES_SRC_UP16R_EN_BIT);
		/* dacr src lk disable */
		hi6402es_dapm_reg_clr_bit(codec, HI6402ES_DAC_SRC_CLK_REG, HI6402ES_DACR_SRC_EN_BIT);
		/* dacrl pga clk disable */
		hi6402es_dapm_reg_clr_bit(codec, HI6402ES_DAC_PGA_CLK_REG, HI6402ES_DACR_PGA_EN_BIT);
		/* dacr clk disable */
		hi6402es_dapm_reg_clr_bit(codec, HI6402ES_DAC_ADC_CLK_REG, HI6402ES_DACR_EN_BIT);
		/* dacrl src clk disable */
		hi6402es_dapm_reg_clr_bit(codec, HI6402ES_DAC_SRC_CLK_REG, HI6402ES_DACR_SRC_EN_BIT);
		break;
	default :
		pr_warn("%s : event err : %d\n", __FUNCTION__, event);
		break;
	}

	return 0;
}

int hi6402es_dacsl_power_mode_event(struct snd_soc_dapm_widget *w,
		struct snd_kcontrol *kcontrol, int event)
{
	struct snd_soc_codec *codec = w->codec;

	BUG_ON(NULL == codec);

	switch (event) {
	case SND_SOC_DAPM_PRE_PMU:
		/* dacl clk enable */
		hi6402es_dapm_reg_set_bit(codec, HI6402ES_DAC_ADC_CLK_REG, HI6402ES_DACSL_EN_BIT);
		/* dacl src up16 clk enable */
		hi6402es_dapm_reg_set_bit(codec, HI6402ES_EQ_SM_CLK_REG, HI6402ES_SRC_UP16L_EN_BIT);
		/* dacl sdm clk enable */
		hi6402es_dapm_reg_set_bit(codec, HI6402ES_SDM_CLK_REG, HI6402ES_DACSL_SDM_EN_BIT);
		/* dacsl sdm enable */
		hi6402es_dapm_reg_set_bit(codec, HI6402ES_SDM_DACS_REG, HI6402ES_SDM_DACSL_EN_BIT);
		break;
	case SND_SOC_DAPM_POST_PMD:
		/* dacsl sdm disable */
		hi6402es_dapm_reg_clr_bit(codec, HI6402ES_SDM_DACS_REG, HI6402ES_SDM_DACSL_EN_BIT);
		/* dacl sdm clk disable */
		hi6402es_dapm_reg_clr_bit(codec, HI6402ES_SDM_CLK_REG, HI6402ES_DACSL_SDM_EN_BIT);
		/* dacl src up16 clk disable */
		hi6402es_dapm_reg_clr_bit(codec, HI6402ES_EQ_SM_CLK_REG, HI6402ES_SRC_UP16L_EN_BIT);
		/* dacl clk disable */
		hi6402es_dapm_reg_clr_bit(codec, HI6402ES_DAC_ADC_CLK_REG, HI6402ES_DACSL_EN_BIT);
		break;
	default :
		pr_warn("%s : event err : %d\n", __FUNCTION__, event);
		break;
	}

	return 0;
}

int hi6402es_dacsr_power_mode_event(struct snd_soc_dapm_widget *w,
		struct snd_kcontrol *kcontrol, int event)
{
	struct snd_soc_codec *codec = w->codec;

	BUG_ON(NULL == codec);

	switch (event) {
	case SND_SOC_DAPM_PRE_PMU:
		/* dacr clk enable */
		hi6402es_dapm_reg_set_bit(codec, HI6402ES_DAC_ADC_CLK_REG, HI6402ES_DACSR_EN_BIT);
		/* dacr src up16 clk enable */
		hi6402es_dapm_reg_set_bit(codec, HI6402ES_EQ_SM_CLK_REG, HI6402ES_SRC_UP16R_EN_BIT);
		/* dacr sdm clk enable */
		hi6402es_dapm_reg_set_bit(codec, HI6402ES_SDM_CLK_REG, HI6402ES_DACSR_SDM_EN_BIT);
		/* dacsrl sdm enable */
		hi6402es_dapm_reg_set_bit(codec, HI6402ES_SDM_DACS_REG, HI6402ES_SDM_DACSR_EN_BIT);
		break;
	case SND_SOC_DAPM_POST_PMD:
		/* dacsrsdm disable */
		hi6402es_dapm_reg_set_bit(codec, HI6402ES_SDM_DACS_REG, HI6402ES_SDM_DACSR_EN_BIT);
		/* dacr sdm clk disable */
		hi6402es_dapm_reg_clr_bit(codec, HI6402ES_SDM_CLK_REG, HI6402ES_DACSR_SDM_EN_BIT);
		/* dac rsrc up16 clk disable */
		hi6402es_dapm_reg_clr_bit(codec, HI6402ES_EQ_SM_CLK_REG, HI6402ES_SRC_UP16R_EN_BIT);
		/* dacrl clk disable */
		hi6402es_dapm_reg_clr_bit(codec, HI6402ES_DAC_ADC_CLK_REG, HI6402ES_DACSR_EN_BIT);
		break;
	default :
		pr_warn("%s : event err : %d\n", __FUNCTION__, event);
		break;
	}

	return 0;
}

int hi6402es_side_pga_power_mode_event(struct snd_soc_dapm_widget *w,
		struct snd_kcontrol *kcontrol, int event)
{
	struct snd_soc_codec *codec = w->codec;

	BUG_ON(NULL == codec);

	switch (event) {
	case SND_SOC_DAPM_PRE_PMU:
		/* sidetone pga & srcclk enable */
		hi6402es_dapm_reg_set_bit(codec, HI6402ES_MDM_SRC_SEL, HI6402ES_SIDE_PGA_EN_BIT);
		hi6402es_dapm_reg_set_bit(codec, HI6402ES_MDM_SRC_SEL, HI6402ES_SIDE_SRC_EN_BIT);
		break;
	case SND_SOC_DAPM_POST_PMD:
		/* sidetone pga & src clk disable */
		hi6402es_dapm_reg_clr_bit(codec, HI6402ES_MDM_SRC_SEL, HI6402ES_SIDE_SRC_EN_BIT);
		hi6402es_dapm_reg_clr_bit(codec, HI6402ES_MDM_SRC_SEL, HI6402ES_SIDE_PGA_EN_BIT);
		break;
	default :
		pr_warn("%s : event err : %d\n", __FUNCTION__, event);
		break;
	}

	return 0;
}

int hi6402es_main_micbias_power_mode_event(struct snd_soc_dapm_widget *w,
		struct snd_kcontrol *kcontrol, int event)
{
	struct snd_soc_codec *codec = w->codec;

	BUG_ON(NULL == codec);

	switch (event) {
	case SND_SOC_DAPM_PRE_PMU:
		/* mainmic chg */
		hi6402es_dapm_reg_clr_bit(codec, HI6402ES_MIC_DISCHG_REG, HI6402ES_MAIN_MIC_DISCHG_BIT);
		/* mainmic on */
		hi6402es_dapm_reg_clr_bit(codec, HI6402ES_ANA_MIC_PD_REG, HI6402ES_MAIN_MIC_PD_BIT);
		break;
	case SND_SOC_DAPM_POST_PMD:
		/* mainmic pd */
		hi6402es_dapm_reg_set_bit(codec, HI6402ES_ANA_MIC_PD_REG, HI6402ES_MAIN_MIC_PD_BIT);
		/* mainmic dischg */
		hi6402es_dapm_reg_set_bit(codec, HI6402ES_MIC_DISCHG_REG, HI6402ES_MAIN_MIC_DISCHG_BIT);
		break;
	default :
		pr_warn("%s : event err : %d\n", __FUNCTION__, event);
		break;
	}

	return 0;
}

int hi6402es_aux_micbias_power_mode_event(struct snd_soc_dapm_widget *w,
		struct snd_kcontrol *kcontrol, int event)
{
	struct snd_soc_codec *codec = w->codec;

	BUG_ON(NULL == codec);

	switch (event) {
	case SND_SOC_DAPM_PRE_PMU:
		/* auxmic chg */
		hi6402es_dapm_reg_clr_bit(codec, HI6402ES_MIC_DISCHG_REG, HI6402ES_AUX_MIC_DISCHG_BIT);
		/* auxmic on */
		hi6402es_dapm_reg_clr_bit(codec, HI6402ES_ANA_MIC_PD_REG, HI6402ES_AUX_MIC_PD_BIT);
		break;
	case SND_SOC_DAPM_POST_PMD:
		/* dismic pd */
		hi6402es_dapm_reg_set_bit(codec, HI6402ES_ANA_MIC_PD_REG, HI6402ES_AUX_MIC_PD_BIT);
		/* auxmic dischg */
		hi6402es_dapm_reg_set_bit(codec, HI6402ES_MIC_DISCHG_REG, HI6402ES_AUX_MIC_DISCHG_BIT);
		break;
	default :
		pr_warn("%s : event err : %d\n", __FUNCTION__, event);
		break;
	}

	return 0;
}

int hi6402es_hs_micbias_power_mode_event(struct snd_soc_dapm_widget *w,
		struct snd_kcontrol *kcontrol, int event)
{
	struct snd_soc_codec *codec = w->codec;

	BUG_ON(NULL == codec);

	switch (event) {
	case SND_SOC_DAPM_PRE_PMU:
		anc_hs_force_charge(ANC_HS_DISABLE_CHARGE);
		/* hsmic chg */
		hi6402es_dapm_reg_clr_bit(codec, HI6402ES_MIC_DISCHG_REG, HI6402ES_HS_MIC_DISCHG_BIT);
		/* hsmic on */
		hi6402es_dapm_reg_clr_bit(codec, HI6402ES_ANA_MIC_PD_REG, HI6402ES_HS_MIC_PD_BIT);
		break;
	case SND_SOC_DAPM_POST_PMD:
		/* hsmic pd */
		hi6402es_dapm_reg_set_bit(codec, HI6402ES_ANA_MIC_PD_REG, HI6402ES_HS_MIC_PD_BIT);
		/* hsmic dischg */
		hi6402es_dapm_reg_set_bit(codec, HI6402ES_MIC_DISCHG_REG, HI6402ES_HS_MIC_DISCHG_BIT);
		anc_hs_force_charge(ANC_HS_ENABLE_CHARGE);
		break;
	default :
		pr_warn("%s : event err : %d\n", __FUNCTION__, event);
		break;
	}

	return 0;
}

int hi6402es_mic2_micbias_power_mode_event(struct snd_soc_dapm_widget *w,
		struct snd_kcontrol *kcontrol, int event)
{
	struct snd_soc_codec *codec = w->codec;

	BUG_ON(NULL == codec);

	switch (event) {
	case SND_SOC_DAPM_PRE_PMU:
		/* mic2 chg */
		hi6402es_dapm_reg_clr_bit(codec, HI6402ES_MIC_DISCHG_REG, HI6402ES_MIC2_DISCHG_BIT);
		/* mic2 on */
		hi6402es_dapm_reg_clr_bit(codec, HI6402ES_ANA_MIC_PD_REG, HI6402ES_MIC2_PD_BIT);
		break;
	case SND_SOC_DAPM_POST_PMD:
		/* mic2 pd */
		hi6402es_dapm_reg_set_bit(codec, HI6402ES_ANA_MIC_PD_REG, HI6402ES_MIC2_PD_BIT);
		/* mic2 dischg */
		hi6402es_dapm_reg_set_bit(codec, HI6402ES_MIC_DISCHG_REG, HI6402ES_MIC2_DISCHG_BIT);
		break;
	default :
		pr_warn("%s : event err : %d\n", __FUNCTION__, event);
		break;
	}

	return 0;
}

int hi6402es_mic3_micbias_power_mode_event(struct snd_soc_dapm_widget *w,
		struct snd_kcontrol *kcontrol, int event)
{
	struct snd_soc_codec *codec = w->codec;

	BUG_ON(NULL == codec);

	switch (event) {
	case SND_SOC_DAPM_PRE_PMU:
		/* mic3 chg */
		hi6402es_dapm_reg_clr_bit(codec, HI6402ES_MIC_DISCHG_REG, HI6402ES_MIC3_DISCHG_BIT);
		/* mic3 on */
		hi6402es_dapm_reg_clr_bit(codec, HI6402ES_ANA_MIC_PD_REG, HI6402ES_MIC3_PD_BIT);
		break;
	case SND_SOC_DAPM_POST_PMD:
		/* mic3 pd */
		hi6402es_dapm_reg_set_bit(codec, HI6402ES_ANA_MIC_PD_REG, HI6402ES_MIC3_PD_BIT);
		/* mic3 dischg */
		hi6402es_dapm_reg_set_bit(codec, HI6402ES_MIC_DISCHG_REG, HI6402ES_MIC3_DISCHG_BIT);
		break;
	default :
		pr_warn("%s : event err : %d\n", __FUNCTION__, event);
		break;
	}

	return 0;
}


/* SOUND KCONTROLS */
static const struct snd_kcontrol_new hi6402es_snd_controls[] = {
	/* s1 port cfg */
	SOC_SINGLE("S1 IF CLK EN",
		HI6402ES_S1_FS_CFG_L, HI6402ES_PORT_CLK_EN, 1, 0),
	SOC_SINGLE("S1 IF FS CFG",
		HI6402ES_S1_FS_CFG_L, HI6402ES_PORT_FS_CFG, 7, 0),
	/* s2 port cfg */
	SOC_SINGLE("S2 IF CLK EN",
		HI6402ES_S2_FS_CFG_L, HI6402ES_PORT_CLK_EN, 1, 0),
	SOC_SINGLE("S2 IF FS CFG",
		HI6402ES_S2_FS_CFG_L, HI6402ES_PORT_FS_CFG, 7, 0),
	SOC_SINGLE("S2 IF FUNC MODE CFG",
		HI6402ES_S2_PORT_CFG_L, HI6402ES_FUNC_MODE_BIT, 5, 0),
	SOC_SINGLE("S2 MST SLV CFG",
		HI6402ES_S2_PORT_CFG_L, HI6402ES_MST_SLV_BIT, 1, 0),
	SOC_SINGLE("S2 LRCLK CFG",
		HI6402ES_S2_PORT_CFG_H, HI6402ES_LRCLK_MODE_BIT, 1, 0),
	SOC_SINGLE("S2 IF FRAME MODE CFG",
		HI6402ES_S2_PORT_CFG_H, HI6402ES_FRAME_MODE_BIT, 1, 0),
	SOC_SINGLE("S2 IF WORD LENGTH CFG",
		HI6402ES_S2_PORT_CFG_H, HI6402ES_WORD_LENGTH_BIT, 3, 0),
	SOC_SINGLE("S2 IF CHNNL MODE CFG",
		HI6402ES_S2_PORT_CFG_H, HI6402ES_CHNNL_MODE_BIT, 1, 0),
	SOC_SINGLE("S2 RX CLK SEL",
		HI6402ES_S2_PORT_CFG_H, HI6402ES_RX_CLK_SEL_BIT, 1, 0),
	SOC_SINGLE("S2 TX CLK SEL",
		HI6402ES_S2_PORT_CFG_H, HI6402ES_TX_CLK_SEL_BIT, 1, 0),
	/* s3 port cfg */
	SOC_SINGLE("S3 IF CLK EN",
		HI6402ES_S3_FS_CFG_L, HI6402ES_PORT_CLK_EN, 1, 0),
	SOC_SINGLE("S3 IF FS CFG",
		HI6402ES_S3_FS_CFG_L, HI6402ES_PORT_FS_CFG, 7, 0),
	/* s4 port cfg */
	SOC_SINGLE("S4 IF CLK EN",
		HI6402ES_S4_FS_CFG_L, HI6402ES_PORT_CLK_EN, 1, 0),
	SOC_SINGLE("S4 IF FS CFG",
		HI6402ES_S4_FS_CFG_L, HI6402ES_PORT_FS_CFG, 7, 0),

	/* s1 i pga gain kcontrol */
	SOC_SINGLE("S1 IL PGA GAIN",
		HI6402ES_S1_PGA_IL_GAIN_CFG_REG, HI6402ES_S1_PGA_IL_GAIN_BIT, 255, 0),
	SOC_SINGLE("S1 IR PGA GAIN",
		HI6402ES_S1_PGA_IR_GAIN_CFG_REG, HI6402ES_S1_PGA_IR_GAIN_BIT, 255, 0),
	/* s2 i pga gain kcontrol */
	SOC_SINGLE("S2 IL PGA GAIN",
		HI6402ES_S2_PGA_IL_GAIN_CFG_REG, HI6402ES_S2_PGA_IL_GAIN_BIT, 255, 0),
	SOC_SINGLE("S2 IR PGA GAIN",
		HI6402ES_S2_PGA_IR_GAIN_CFG_REG, HI6402ES_S2_PGA_IR_GAIN_BIT, 255, 0),
	/* s3 i pga gain kcontrol */
	SOC_SINGLE("S3 IL PGA GAIN",
		HI6402ES_S3_PGA_IL_GAIN_CFG_REG, HI6402ES_S3_PGA_IL_GAIN_BIT, 255, 0),
	SOC_SINGLE("S3 IR PGA GAIN",
		HI6402ES_S3_PGA_IR_GAIN_CFG_REG, HI6402ES_S3_PGA_IR_GAIN_BIT, 255, 0),
	/* s4 i pga gain kcontrol */
	SOC_SINGLE("S4 IL PGA GAIN",
		HI6402ES_S4_PGA_IL_GAIN_CFG_REG, HI6402ES_S4_PGA_IL_GAIN_BIT, 255, 0),
	SOC_SINGLE("S4 IR PGA GAIN",
		HI6402ES_S4_PGA_IR_GAIN_CFG_REG, HI6402ES_S4_PGA_IR_GAIN_BIT, 255, 0),
	/* s1 o pga gain kcontrol */
	SOC_SINGLE("S1 OL PGA GAIN",
		HI6402ES_S1_PGA_OL_GAIN_CFG_REG, HI6402ES_S1_PGA_OL_GAIN_BIT, 255, 0),
	SOC_SINGLE("S1 OR PGA GAIN",
		HI6402ES_S1_PGA_OR_GAIN_CFG_REG, HI6402ES_S1_PGA_OR_GAIN_BIT, 255, 0),
	/* s2 o pga gain kcontrol */
	SOC_SINGLE("S2 OL PGA GAIN",
		HI6402ES_S2_PGA_OL_GAIN_CFG_REG, HI6402ES_S2_PGA_OL_GAIN_BIT, 255, 0),
	SOC_SINGLE("S2 OR PGA GAIN",
		HI6402ES_S2_PGA_OR_GAIN_CFG_REG, HI6402ES_S2_PGA_OR_GAIN_BIT, 255, 0),
	/* s3 o pga gain kcontrol */
	SOC_SINGLE("S3 OL PGA GAIN",
		HI6402ES_S3_PGA_OL_GAIN_CFG_REG, HI6402ES_S3_PGA_OL_GAIN_BIT, 255, 0),
	SOC_SINGLE("S3 OR PGA GAIN",
		HI6402ES_S3_PGA_OR_GAIN_CFG_REG, HI6402ES_S3_PGA_OR_GAIN_BIT, 255, 0),
	/* s4 o pga gain kcontrol */
	SOC_SINGLE("S4 OL PGA GAIN",
		HI6402ES_S4_PGA_OL_GAIN_CFG_REG, HI6402ES_S4_PGA_OL_GAIN_BIT, 255, 0),
	SOC_SINGLE("S4 OR PGA GAIN",
		HI6402ES_S4_PGA_OR_GAIN_CFG_REG, HI6402ES_S4_PGA_OR_GAIN_BIT, 255, 0),

	/* side pga gain kcontrol */
	SOC_SINGLE("SIDE PGA GAIN",
		HI6402ES_SIDE_PGA_GAIN_CFG_REG, HI6402ES_SIDE_PGA_GAIN_BIT, 255, 0),

	/* dac pga gain kcontrol */
	SOC_SINGLE("DACL PGA GAIN",
		HI6402ES_DACL_PGA_GAIN_CFG_REG, HI6402ES_DACL_PGA_GAIN_BIT, 255, 0),
	SOC_SINGLE("DACR PGA GAIN",
		HI6402ES_DACR_PGA_GAIN_CFG_REG, HI6402ES_DACR_PGA_GAIN_BIT, 255, 0),

	/* lineinl pga gain kcontrol */
	SOC_SINGLE_TLV("LINEL PGA GAIN",
		HI6402ES_LINEL_PGA_SEL_REG, HI6402ES_LINEL_PGA_GAIN_BIT, 28, 0, lineinl_mic_tlv),
	/* lineinr pga gain kcontrol */
	SOC_SINGLE_TLV("LINER PGA GAIN",
		HI6402ES_LINER_PGA_SEL_REG, HI6402ES_LINER_PGA_GAIN_BIT, 28, 0, lineinr_mic_tlv),
	/* aux pga gain kcontrol */
	SOC_SINGLE_TLV("AUX PGA GAIN",
		HI6402ES_AUX_PGA_SEL_REG, HI6402ES_AUX_PGA_GAIN_BIT, 18, 0, aux_mic_tlv),
	/* main pga gain kcontrol */
	SOC_SINGLE_TLV("MAIN PGA GAIN",
		HI6402ES_MAIN_PGA_SEL_REG, HI6402ES_MAIN_PGA_GAIN_BIT, 18, 0, main_mic_tlv),

	/* lol pga gain kcontrol */
	SOC_SINGLE_TLV("LOL PGA GAIN",
		HI6402ES_LOL_PGA_CFG_REG, HI6402ES_LOL_PGA_GAIN_BIT, 18, 0, lol_pga_tlv),
	SOC_SINGLE("LOL PGA MUTE",
		HI6402ES_LOL_PGA_CFG_REG, HI6402ES_LOL_PGA_MUTE_BIT, 1, 1),

	/* lor pga gain kcontrol */
	SOC_SINGLE_TLV("LOR PGA GAIN",
		HI6402ES_LOR_PGA_CFG_REG, HI6402ES_LOR_PGA_GAIN_BIT, 18, 0, lor_pga_tlv),
	SOC_SINGLE("LOR PGA MUTE",
		HI6402ES_LOR_PGA_CFG_REG, HI6402ES_LOR_PGA_MUTE_BIT, 1, 1),

	/* hpl pga gain kcontrol */
	SOC_SINGLE("HPL PGA GAIN",
		HI6402ES_HPL_PGA_CFG_REG, HI6402ES_HPL_PGA_GAIN_BIT, 33, 0),
	SOC_SINGLE("HPL PGA MUTE",
		HI6402ES_HPL_PGA_CFG_REG, HI6402ES_HPL_PGA_MUTE_BIT, 1, 1),

	/* hpr pga gain kcontrol */
	SOC_SINGLE("HPR PGA GAIN",
		HI6402ES_HPR_PGA_CFG_REG, HI6402ES_HPR_PGA_GAIN_BIT, 33, 0),
	SOC_SINGLE("HPR PGA MUTE",
		HI6402ES_HPR_PGA_CFG_REG, HI6402ES_HPR_PGA_MUTE_BIT, 1, 1),

	/* ep pga gain kcontrol */
	SOC_SINGLE_TLV("EP PGA GAIN",
		HI6402ES_EP_PGA_CFG_REG, HI6402ES_EP_PGA_GAIN_BIT, 18, 0, ep_pga_tlv),
	SOC_SINGLE("EP PGA MUTE",
		HI6402ES_EP_PGA_CFG_REG, HI6402ES_EP_PGA_MUTE_BIT, 1, 1),

	/* s1 il src kcontrol */
	SOC_SINGLE("S1 IL SRC EN",
		HI6402ES_S1_MODULE_CLK_REG, HI6402ES_S1_SRC_IL_EN_BIT, 1, 0),
	SOC_SINGLE("S1 IL SRC MODE",
		HI6402ES_S1_SRC_IN_MODE_CGF, HI6402ES_S1_SRC_IL_MODE_BIT, 7, 0),	
	/* s1 ir src kcontrol */
	SOC_SINGLE("S1 IR SRC EN",
		HI6402ES_S1_MODULE_CLK_REG, HI6402ES_S1_SRC_IR_EN_BIT, 1, 0),
	SOC_SINGLE("S1 IR SRC MODE",
		HI6402ES_S1_SRC_IN_MODE_CGF, HI6402ES_S1_SRC_IR_MODE_BIT, 7, 0),	
	/* s1 ol src kcontrol */
	SOC_SINGLE("S1 OL SRC EN",
		HI6402ES_S1_MODULE_CLK_REG, HI6402ES_S1_SRC_OL_EN_BIT, 1, 0),
	SOC_SINGLE("S1 OL SRC MODE",
		HI6402ES_S1_SRC_OUT_MODE_CGF, HI6402ES_S1_SRC_OL_MODE_BIT, 3, 0),	
	/* s1 or src kcontrol */
	SOC_SINGLE("S1 OR SRC EN",
		HI6402ES_S1_MODULE_CLK_REG, HI6402ES_S1_SRC_OR_EN_BIT, 1, 0),
	SOC_SINGLE("S1 OR SRC MODE",
		HI6402ES_S1_SRC_OUT_MODE_CGF, HI6402ES_S1_SRC_OR_MODE_BIT, 3, 0),	
	/* s2 il src kcontrol */
	SOC_SINGLE("S2 IL SRC EN",
		HI6402ES_S2_MODULE_CLK_REG, HI6402ES_S2_SRC_IL_EN_BIT, 1, 0),
	SOC_SINGLE("S2 IL SRC MODE",
		HI6402ES_S2_SRC_IN_MODE_CGF, HI6402ES_S2_SRC_IL_MODE_BIT, 7, 0),	
	/* s2 ir src kcontrol */
	SOC_SINGLE("S2 IR SRC EN",
		HI6402ES_S2_MODULE_CLK_REG, HI6402ES_S2_SRC_IR_EN_BIT, 1, 0),
	SOC_SINGLE("S2 IR SRC MODE",
		HI6402ES_S2_SRC_IN_MODE_CGF, HI6402ES_S2_SRC_IR_MODE_BIT, 7, 0),	
	/* s2 ol src kcontrol */
	SOC_SINGLE("S2 OL SRC EN",
		HI6402ES_S2_MODULE_CLK_REG, HI6402ES_S2_SRC_OL_EN_BIT, 1, 0),
	SOC_SINGLE("S2 OL SRC MODE",
		HI6402ES_S2_SRC_OUT_MODE_CGF, HI6402ES_S2_SRC_OL_MODE_BIT, 3, 0),	
	/* s2 or src kcontrol */
	SOC_SINGLE("S2 OR SRC EN",
		HI6402ES_S2_MODULE_CLK_REG, HI6402ES_S2_SRC_OR_EN_BIT, 1, 0),
	SOC_SINGLE("S2 OR SRC MODE",
		HI6402ES_S2_SRC_OUT_MODE_CGF, HI6402ES_S2_SRC_OR_MODE_BIT, 3, 0),	
	/* s3 il src kcontrol */
	SOC_SINGLE("S3 IL SRC EN",
		HI6402ES_S3_MODULE_CLK_REG, HI6402ES_S3_SRC_IL_EN_BIT, 1, 0),
	SOC_SINGLE("S3 IL SRC MODE",
		HI6402ES_S3_SRC_IN_MODE_CGF, HI6402ES_S3_SRC_IL_MODE_BIT, 7, 0),
	/* s3 ir src kcontrol */
	SOC_SINGLE("S3 IR SRC EN",
		HI6402ES_S3_MODULE_CLK_REG, HI6402ES_S3_SRC_IR_EN_BIT, 1, 0),
	SOC_SINGLE("S3 IR SRC MODE",
		HI6402ES_S3_SRC_IN_MODE_CGF, HI6402ES_S3_SRC_IR_MODE_BIT, 7, 0),
	/* s3 ol src kcontrol */
	SOC_SINGLE("S3 OL SRC EN",
		HI6402ES_S3_MODULE_CLK_REG, HI6402ES_S3_SRC_OL_EN_BIT, 1, 0),
	SOC_SINGLE("S3 OL SRC MODE",
		HI6402ES_S3_SRC_OUT_MODE_CGF, HI6402ES_S3_SRC_OL_MODE_BIT, 3, 0),	
	/* s3 or src kcontrol */
	SOC_SINGLE("S3 OR SRC EN",
		HI6402ES_S3_MODULE_CLK_REG, HI6402ES_S3_SRC_OR_EN_BIT, 1, 0),
	SOC_SINGLE("S3 OR SRC MODE",
		HI6402ES_S3_SRC_OUT_MODE_CGF, HI6402ES_S3_SRC_OR_MODE_BIT, 3, 0),	
	/* s4 il src kcontrol */
	SOC_SINGLE("S4 IL SRC EN",
		HI6402ES_S4_MODULE_CLK_REG, HI6402ES_S4_SRC_IL_EN_BIT, 1, 0),
	SOC_SINGLE("S4 IL SRC MODE",
		HI6402ES_S4_SRC_IN_MODE_CGF, HI6402ES_S4_SRC_IL_MODE_BIT, 7, 0),
	/* s4 ir src kcontrol */
	SOC_SINGLE("S4 IR SRC EN",
		HI6402ES_S4_MODULE_CLK_REG, HI6402ES_S4_SRC_IR_EN_BIT, 1, 0),
	SOC_SINGLE("S4 IR SRC MODE",
		HI6402ES_S4_SRC_IN_MODE_CGF, HI6402ES_S4_SRC_IR_MODE_BIT, 7, 0),
	/* s4 ol src kcontrol */
	SOC_SINGLE("S4 OL SRC EN",
		HI6402ES_S4_MODULE_CLK_REG, HI6402ES_S4_SRC_OL_EN_BIT, 1, 0),
	SOC_SINGLE("S4 OL SRC MODE",
		HI6402ES_S4_SRC_OUT_MODE_CGF, HI6402ES_S4_SRC_OL_MODE_BIT, 3, 0),	
	/* s4 or src kcontrol */
	SOC_SINGLE("S4 OR SRC EN",
		HI6402ES_S4_MODULE_CLK_REG, HI6402ES_S4_SRC_OR_EN_BIT, 1, 0),
	SOC_SINGLE("S4 OR SRC MODE",
		HI6402ES_S4_SRC_OUT_MODE_CGF, HI6402ES_S4_SRC_OR_MODE_BIT, 3, 0),	

	/* sidetone src kcontrol */
	SOC_SINGLE("SIDETONE SRC EN",
		HI6402ES_MDM_SRC_SEL, HI6402ES_SIDE_SRC_EN_BIT, 1, 0),
	SOC_SINGLE("SIDETONE SRC MODE",
		HI6402ES_SIDE_MDM_SRC_MODE_REG, HI6402ES_SIDETONE_SRC_MODE_BIT, 3, 0),

	/* mdm src kcontrol */
	SOC_SINGLE("MDM SRC EN",
		HI6402ES_MDM_SRC_SEL, HI6402ES_MDM_SRC_EN_BIT, 1, 0),
	SOC_SINGLE("MDM SRC MODE",
		HI6402ES_SIDE_MDM_SRC_MODE_REG, HI6402ES_MDM_SRC_MODE_BIT, 3, 0),

	/* dac src kcontrol */
	SOC_SINGLE("DACL SRC EN",
		HI6402ES_DAC_SRC_CLK_REG, HI6402ES_DACL_SRC_EN_BIT, 1, 0),
	SOC_SINGLE("DACL SRC MODE",
		HI6402ES_DAC_SRC_MODE_REG, HI6402ES_DACL_SRC_MODE_BIT, 7, 0),
	SOC_SINGLE("DACR SRC EN",
		HI6402ES_DAC_SRC_CLK_REG, HI6402ES_DACR_SRC_EN_BIT, 1, 0),
	SOC_SINGLE("DACR SRC MODE",
		HI6402ES_DAC_SRC_MODE_REG, HI6402ES_DACR_SRC_MODE_BIT, 7, 0),
	
	/* dac dacs src up16 kcontrol */
	SOC_SINGLE("DACL SRC UP16 EN",
		HI6402ES_EQ_SM_CLK_REG, HI6402ES_SRC_UP16L_EN_BIT, 1, 0),
	SOC_SINGLE("DACL SRC UP16 MODE",
		HI6402ES_DAC_FILTER_OUTL_MUX_SEL, HI6402ES_DAC_SRC_UP16_MODE_BIT, 15, 0),
	SOC_SINGLE("DACR SRC UP16 EN",
		HI6402ES_EQ_SM_CLK_REG, HI6402ES_SRC_UP16R_EN_BIT, 1, 0),
	SOC_SINGLE("DACR SRC UP16 MODE",
		HI6402ES_DAC_FILTER_OUTR_MUX_SEL, HI6402ES_DAC_SRC_UP16_MODE_BIT, 15, 0),

	/* HP DAC MUTE */
	SOC_SINGLE("HP DACL MUTE",
		HI6402ES_DAC_HP_PD_REG, HI6402ES_DAC_HPL_MUTE_PD_BIT, 1, 1),
	SOC_SINGLE("HP DACR MUTE",
		HI6402ES_DAC_HP_PD_REG, HI6402ES_DAC_HPL_MUTE_PD_BIT, 1, 1),

	/* ANA ADC MUTE */
	SOC_SINGLE("MAINPGA MUTE1",
		HI6402ES_ADC_PGA_MUTE1_SEL_REG, HI6402ES_MAIN_PGA_MUTE1_BIT, 1, 1),
	SOC_SINGLE("MAINPGA MUTE2",
		HI6402ES_MAIN_PGA_SEL_REG, HI6402ES_MAIN_PGA_MUTE2_BIT, 1, 1),
	SOC_SINGLE("AUXPGA MUTE1",
		HI6402ES_AUX_PGA_SEL_REG, HI6402ES_AUX_PGA_MUTE1_BIT, 1, 1),
	SOC_SINGLE("AUXPGA MUTE2",
		HI6402ES_AUX_PGA_SEL_REG, HI6402ES_AUX_PGA_MUTE2_BIT, 1, 1),
	SOC_SINGLE("LINELPGA MUTE1",
		HI6402ES_ADC_PGA_MUTE1_SEL_REG, HI6402ES_LINEL_PGA_MUTE1_BIT, 1, 1),
	SOC_SINGLE("LINELPGA MUTE2",
		HI6402ES_LINEL_PGA_SEL_REG, HI6402ES_LINEL_PGA_MUTE2_BIT, 1, 1),
	SOC_SINGLE("LINERPGA MUTE1",
		HI6402ES_ADC_PGA_MUTE1_SEL_REG, HI6402ES_LINER_PGA_MUTE1_BIT, 1, 1),
	SOC_SINGLE("LINERPGA MUTE2",
		HI6402ES_LINER_PGA_SEL_REG, HI6402ES_LINER_PGA_MUTE2_BIT, 1, 1),

	/* DACL SRC MIXER GAIN */
	SOC_SINGLE("DACL MIXER GAIN0",
		HI6402ES_DACSL_MIXER_GAIN_REG, HI6402ES_DACSL_MIXER_0_GAIN_BIT, 3, 0),
	SOC_SINGLE("DACL MIXER GAIN1",
		HI6402ES_DACSL_MIXER_GAIN_REG, HI6402ES_DACSL_MIXER_1_GAIN_BIT, 3, 0),

	/* DACR SRC MIXER GAIN */
	SOC_SINGLE("DACR MIXER GAIN0",
		HI6402ES_DACSR_MIXER_GAIN_REG, HI6402ES_DACSR_MIXER_0_GAIN_BIT, 3, 0),
	SOC_SINGLE("DACR MIXER GAIN1",
		HI6402ES_DACSL_MIXER_GAIN_REG, HI6402ES_DACSR_MIXER_1_GAIN_BIT, 3, 0),

	/* SHARE MIXER GAIN */
	SOC_SINGLE("SHARE MIXER GAIN1",
		HI6402ES_SHARE_MIXER_GAIN_REG, HI6402ES_SHARE_MIXER_GAIN1_BIT, 3, 0),
	SOC_SINGLE("SHARE MIXER GAIN2",
		HI6402ES_SHARE_MIXER_GAIN_REG, HI6402ES_SHARE_MIXER_GAIN2_BIT, 3, 0),
	SOC_SINGLE("SHARE MIXER GAIN3",
		HI6402ES_SHARE_MIXER_GAIN_REG, HI6402ES_SHARE_MIXER_GAIN3_BIT, 3, 0),
	SOC_SINGLE("SHARE MIXER GAIN4",
		HI6402ES_SHARE_MIXER_GAIN_REG, HI6402ES_SHARE_MIXER_GAIN4_BIT, 3, 0),

	/* DACL MIXER GAIN */
	SOC_SINGLE("MIXERL GAIN1",
		HI6402ES_DACL_MIXER_GAIN_REG, HI6402ES_DACL_MIXER_S1_GAIN_BIT, 3, 0),
	SOC_SINGLE("MIXERL GAIN2",
		HI6402ES_DACL_MIXER_GAIN_REG, HI6402ES_DACL_MIXER_S2L_GAIN_BIT, 3, 0),
	SOC_SINGLE("MIXERL GAIN3",
		HI6402ES_DACL_MIXER_GAIN_REG, HI6402ES_DACL_MIXER_MDM_GAIN_BIT, 3, 0),
	SOC_SINGLE("MIXERL GAIN4",
		HI6402ES_DACL_MIXER_GAIN_REG, HI6402ES_DACL_MIXER_SIDETONE_GAIN_BIT, 3, 0),

	/* DACR MIXER GAIN */
	SOC_SINGLE("MIXERR GAIN1",
		HI6402ES_DACR_MIXER_GAIN_REG, HI6402ES_DACR_MIXER_S1_GAIN_BIT, 3, 0),
	SOC_SINGLE("MIXERR GAIN2",
		HI6402ES_DACR_MIXER_GAIN_REG, HI6402ES_DACR_MIXER_S2R_GAIN_BIT, 3, 0),
	SOC_SINGLE("MIXERR GAIN3",
		HI6402ES_DACR_MIXER_GAIN_REG, HI6402ES_DACR_MIXER_MDM_GAIN_BIT, 3, 0),
	SOC_SINGLE("MIXERR GAIN4",
		HI6402ES_DACR_MIXER_GAIN_REG, HI6402ES_DACR_MIXER_SIDETONE_GAIN_BIT, 3, 0),

	/* BOOST */
	SOC_SINGLE("MAIN MIC BOOST",
		HI6402ES_ANA_MIC_BOOST_REG, HI6402ES_ANA_MAINMIC_BOOST_BIT, 1, 0),
	SOC_SINGLE("AUX MIC BOOST",
		HI6402ES_ANA_MIC_BOOST_REG, HI6402ES_ANA_AUXMIC_BOOST_BIT, 1, 0),
	SOC_SINGLE("LINEINL BOOST",
		HI6402ES_ANA_LINEIN_BOOST_REG, HI6402ES_ANA_LINEINL_BOOST_BIT, 1, 0),
	SOC_SINGLE("LINEINR BOOST",
		HI6402ES_ANA_LINEIN_BOOST_REG, HI6402ES_ANA_LINEINR_BOOST_BIT, 1, 0),
};

/* SWITCH */
/* DACSL */
static const struct snd_kcontrol_new hi6402es_dapm_dacsl_switch_controls =
	SOC_DAPM_SINGLE("SWITCH",
		HI6402ES_DAC_ADC_CLK_REG, HI6402ES_DACSL_EN_BIT, 1, 0);
/* DACSR */
static const struct snd_kcontrol_new hi6402es_dapm_dacsr_switch_controls =
	SOC_DAPM_SINGLE("SWITCH",
		HI6402ES_DAC_ADC_CLK_REG, HI6402ES_DACSR_EN_BIT, 1, 0);


/* MAD SWITCH */
static const struct snd_kcontrol_new hi6402es_dapm_mad_switch_controls =
	SOC_DAPM_SINGLE("SWITCH",
		HI6402ES_S1_MODULE_CLK_REG, HI6402ES_S1_SRC_OL_EN_BIT, 1, 0);
/* S1 OL SWITCH */
static const struct snd_kcontrol_new hi6402es_dapm_s1_ol_switch_controls =
	SOC_DAPM_SINGLE("SWITCH",
		HI6402ES_S1_MODULE_CLK_REG, HI6402ES_S1_PGA_OL_EN_BIT, 1, 0);
/* S1 OR SWITCH */
static const struct snd_kcontrol_new hi6402es_dapm_s1_or_switch_controls =
	SOC_DAPM_SINGLE("SWITCH",
		HI6402ES_S1_MODULE_CLK_REG, HI6402ES_S1_PGA_OR_EN_BIT, 1, 0);
/* S2 OL SWITCH */
static const struct snd_kcontrol_new hi6402es_dapm_s2_ol_switch_controls =
	SOC_DAPM_SINGLE("SWITCH",
		HI6402ES_S2_MODULE_CLK_REG, HI6402ES_S2_PGA_OL_EN_BIT, 1, 0);
/* S2 OR SWITCH */
static const struct snd_kcontrol_new hi6402es_dapm_s2_or_switch_controls =
	SOC_DAPM_SINGLE("SWITCH",
		HI6402ES_S2_MODULE_CLK_REG, HI6402ES_S2_PGA_OR_EN_BIT, 1, 0);
/* S3 OL SWITCH */
static const struct snd_kcontrol_new hi6402es_dapm_s3_ol_switch_controls =
	SOC_DAPM_SINGLE("SWITCH",
		HI6402ES_S3_MODULE_CLK_REG, HI6402ES_S3_PGA_OL_EN_BIT, 1, 0);
/* S3 OR SWITCH */
static const struct snd_kcontrol_new hi6402es_dapm_s3_or_switch_controls =
	SOC_DAPM_SINGLE("SWITCH",
		HI6402ES_S3_MODULE_CLK_REG, HI6402ES_S3_PGA_OR_EN_BIT, 1, 0);
/* S4 OL SWITCH */
static const struct snd_kcontrol_new hi6402es_dapm_s4_ol_switch_controls =
	SOC_DAPM_SINGLE("SWITCH",
		HI6402ES_S4_MODULE_CLK_REG, HI6402ES_S4_PGA_OL_EN_BIT, 1, 0);
/* S4 OR SWITCH */
static const struct snd_kcontrol_new hi6402es_dapm_s4_or_switch_controls =
	SOC_DAPM_SINGLE("SWITCH",
		HI6402ES_S4_MODULE_CLK_REG, HI6402ES_S4_PGA_OR_EN_BIT, 1, 0);

/* SP IL SWITCH */
static const struct snd_kcontrol_new hi6402es_dapm_sp_il_switch_controls =
	SOC_DAPM_SINGLE("SWITCH",
		HI6402ES_S4_MODULE_CLK_REG, HI6402ES_S4_PGA_IL_EN_BIT, 1, 0);
/* SP IR SWITCH */
static const struct snd_kcontrol_new hi6402es_dapm_sp_ir_switch_controls =
	SOC_DAPM_SINGLE("SWITCH",
		HI6402ES_S4_MODULE_CLK_REG, HI6402ES_S4_PGA_IR_EN_BIT, 1, 0);

/* EP SWITCH */
static const struct snd_kcontrol_new hi6402es_dapm_ep_switch_controls =
	SOC_DAPM_SINGLE("SWITCH",
		HI6402ES_DAC_LT_MUX_SEL, HI6402ES_DAC_EP_MUX_H_BIT, 1, 1);


/* APL MUX 0x7208 */
static const char *hi6402es_apl_mux_texts[] = {
	"ADC0L",
	"ADC1L",
	"DACL",	
	"DACL_48",
	"S1L",
	"S2L",
	"S3L",
	"S4L",
	"MDM",
	"BT0",
	"MDM0",
	"MDM2",
};
static const struct soc_enum hi6402es_apl_mux_enum =
	SOC_ENUM_SINGLE(HI6402ES_AP_MUX_SEL, HI6402ES_APL_MUX_BIT,
		ARRAY_SIZE(hi6402es_apl_mux_texts),hi6402es_apl_mux_texts);
static const struct snd_kcontrol_new hi6402es_dapm_apl_mux_controls =
	SOC_DAPM_ENUM("Mux", hi6402es_apl_mux_enum);

/* APR MUX 0x7208 */
static const char *hi6402es_apr_mux_texts[] = {
	"ADC0R",
	"ADC1R",
	"DACR",	
	"DACR_48",
	"S1R",
	"S2R",
	"S3R",
	"S4R",
	"MDM",
	"BT1",
	"MDM1",
	"MDM3",
};
static const struct soc_enum hi6402es_apr_mux_enum =
	SOC_ENUM_SINGLE(HI6402ES_AP_MUX_SEL, HI6402ES_APR_MUX_BIT,
		ARRAY_SIZE(hi6402es_apr_mux_texts), hi6402es_apr_mux_texts);
static const struct snd_kcontrol_new hi6402es_dapm_apr_mux_controls =
	SOC_DAPM_ENUM("Mux", hi6402es_apr_mux_enum);

/* BTL MUX 0x720A */
static const char *hi6402es_btl_mux_texts[] = {
	"ADC0L",
	"ADC1L",
	"DACL",
	"DACL_48",
	"S1L",
	"S2L",
	"S3L",
	"S4L",
	"MDM",
	"AP0",
	"MDM0",
	"MDM2",
};
static const struct soc_enum hi6402es_btl_mux_enum =
	SOC_ENUM_SINGLE(HI6402ES_BT_MUX_SEL, HI6402ES_BTL_MUX_BIT,
		ARRAY_SIZE(hi6402es_btl_mux_texts),hi6402es_btl_mux_texts);
static const struct snd_kcontrol_new hi6402es_dapm_btl_mux_controls =
	SOC_DAPM_ENUM("Mux", hi6402es_btl_mux_enum);

/* BTR MUX 0x720A */
static const char *hi6402es_btr_mux_texts[] = {
	"ADC0R",
	"ADC1R",
	"DACR",
	"DACR_48",
	"S1R",
	"S2R",
	"S3R",
	"S4R",
	"MDM",
	"AP1",
	"MDM1",
	"MDM3",
};
static const struct soc_enum hi6402es_btr_mux_enum =
	SOC_ENUM_SINGLE(HI6402ES_BT_MUX_SEL, HI6402ES_BTR_MUX_BIT,
		ARRAY_SIZE(hi6402es_btr_mux_texts), hi6402es_btr_mux_texts);
static const struct snd_kcontrol_new hi6402es_dapm_btr_mux_controls =
	SOC_DAPM_ENUM("Mux", hi6402es_btr_mux_enum);

/* M0L MUX 0x7209 */
static const char *hi6402es_m0l_mux_texts[] = {
	"ADC0L",
	"ADC1L",
	"DACL",
	"DACL_48",
	"S1L",
	"S2L",
	"S3L",
	"S4L",
	"MDM",
	"AP0",
	"BT0",
	"MDM2",
};
static const struct soc_enum hi6402es_m0l_mux_enum =
	SOC_ENUM_SINGLE(HI6402ES_M0_MUX_SEL, HI6402ES_M0L_MUX_BIT,
		ARRAY_SIZE(hi6402es_m0l_mux_texts),hi6402es_m0l_mux_texts);
static const struct snd_kcontrol_new hi6402es_dapm_m0l_mux_controls =
	SOC_DAPM_ENUM("Mux", hi6402es_m0l_mux_enum);

/* M0R MUX 0x7209 */
static const char *hi6402es_m0r_mux_texts[] = {
	"ADC0R",
	"ADC1R",
	"DACR",
	"DACR_48",
	"S1R",
	"S2R",
	"S3R",
	"S4R",
	"MDM",
	"AP1",
	"BT1",
	"MDM3",
};
static const struct soc_enum hi6402es_m0r_mux_enum =
	SOC_ENUM_SINGLE(HI6402ES_M0_MUX_SEL, HI6402ES_M0R_MUX_BIT,
		ARRAY_SIZE(hi6402es_m0r_mux_texts), hi6402es_m0r_mux_texts);
static const struct snd_kcontrol_new hi6402es_dapm_m0r_mux_controls =
	SOC_DAPM_ENUM("Mux", hi6402es_m0r_mux_enum);

/* M1L MUX 0x720B */
static const char *hi6402es_m1l_mux_texts[] = {
	"ADC0L",
	"ADC1L",
	"DACL",
	"DACL_48",
	"S1L",
	"S2L",
	"S3L",
	"S4L",
	"MDM",
	"AP0",
	"BT0",
	"MDM0",
};
static const struct soc_enum hi6402es_m1l_mux_enum =
	SOC_ENUM_SINGLE(HI6402ES_M1_MUX_SEL, HI6402ES_M1L_MUX_BIT,
		ARRAY_SIZE(hi6402es_m1l_mux_texts),hi6402es_m1l_mux_texts);
static const struct snd_kcontrol_new hi6402es_dapm_m1l_mux_controls =
	SOC_DAPM_ENUM("Mux", hi6402es_m1l_mux_enum);

/* M1R MUX 0x720B */
static const char *hi6402es_m1r_mux_texts[] = {
	"ADC0R",
	"ADC1R",
	"DACR",
	"DACR_48",
	"S1R",
	"S2R",
	"S3R",
	"S4R",
	"MDM",
	"AP1",
	"BT1",
	"MDM1",
};
static const struct soc_enum hi6402es_m1r_mux_enum =
	SOC_ENUM_SINGLE(HI6402ES_M1_MUX_SEL, HI6402ES_M1R_MUX_BIT,
		ARRAY_SIZE(hi6402es_m1r_mux_texts), hi6402es_m1r_mux_texts);
static const struct snd_kcontrol_new hi6402es_dapm_m1r_mux_controls =
	SOC_DAPM_ENUM("Mux", hi6402es_m1r_mux_enum);

/* MIXERL S1 MUX 0x7210 */
static const char *hi6402es_dacl_mixer_mux_texts[] = {
	"S1 IL",
	"S2 IL",
	"S3 IL",
	"S4 IL",
	"SHARE MIXER",
};
static const struct soc_enum hi6402es_dacl_mixer_mux_enum =
	SOC_ENUM_SINGLE(HI6402ES_DAC_MIXER_MUX_SEL, HI6402ES_DACL_MIXER_MUX_BIT,
		ARRAY_SIZE(hi6402es_dacl_mixer_mux_texts), hi6402es_dacl_mixer_mux_texts);
static const struct snd_kcontrol_new hi6402es_dapm_dacl_mixer_mux_controls =
	SOC_DAPM_ENUM("Mux", hi6402es_dacl_mixer_mux_enum);

/* MIXERR S1 MUX 0x7210 */
static const char *hi6402es_dacr_mixer_mux_texts[] = {
	"S1 IR",
	"S2 IR",
	"S3 IR",
	"S4 IR",
	"SHARE MIXER",
};
static const struct soc_enum hi6402es_dacr_mixer_mux_enum =
	SOC_ENUM_SINGLE(HI6402ES_DAC_MIXER_MUX_SEL, HI6402ES_DACR_MIXER_MUX_BIT,
		ARRAY_SIZE(hi6402es_dacr_mixer_mux_texts), hi6402es_dacr_mixer_mux_texts);
static const struct snd_kcontrol_new hi6402es_dapm_dacr_mixer_mux_controls =
	SOC_DAPM_ENUM("Mux", hi6402es_dacr_mixer_mux_enum);

/* MDM MUX 0x7212 */
static const char *hi6402es_mdm_src_mux_texts[] = {
	"S1 IL",
	"S2 IL",
	"S3 IL",
	"S4 IL",
	"ADC0L",
	"ADC0R",
	"ADC1L",
	"ADC1R",
	"SHARE MIXER",
};
static const struct soc_enum hi6402es_mdm_src_mux_enum =
	SOC_ENUM_SINGLE(HI6402ES_MDM_SRC_MUX_SEL, HI6402ES_MDM_SRC_MUX_BIT,
		ARRAY_SIZE(hi6402es_mdm_src_mux_texts), hi6402es_mdm_src_mux_texts);
static const struct snd_kcontrol_new hi6402es_dapm_mdm_src_mux_controls =
	SOC_DAPM_ENUM("Mux", hi6402es_mdm_src_mux_enum);

/* SIDETONE MUX 0x7212 */
static const char *hi6402es_sidetone_src_mux_texts[] = {
	"S1 IL",
	"S2 IL",
	"S3 IL",
	"S4 IL",
	"ADC0L",
	"ADC0R",
	"ADC1L",
	"ADC1R",
	"SHARE MIXER",
};
static const struct soc_enum hi6402es_sidetone_src_mux_enum =
	SOC_ENUM_SINGLE(HI6402ES_SIDETONE_SRC_MUX_SEL, HI6402ES_SIDETONE_SRC_MUX_BIT,
		ARRAY_SIZE(hi6402es_sidetone_src_mux_texts), hi6402es_sidetone_src_mux_texts);
static const struct snd_kcontrol_new hi6402es_dapm_sidetone_src_mux_controls =
	SOC_DAPM_ENUM("Mux", hi6402es_sidetone_src_mux_enum);

/* DACSL MUX 0x7215 */
static const char *hi6402es_dacsl_mux_texts[] = {
	"DSP IF8 OUT",
	"S1 L",
	"S2 L",
	"S3 L",
	"S4 L",
	"SHARE MIXER",
};
static const struct soc_enum hi6402es_dacsl_mux_enum =
	SOC_ENUM_SINGLE(HI6402ES_DACS_MUX_SEL, HI6402ES_DACSL_MUX_BIT,
		ARRAY_SIZE(hi6402es_dacsl_mux_texts), hi6402es_dacsl_mux_texts);
static const struct snd_kcontrol_new hi6402es_dapm_dacsl_mux_controls =
	SOC_DAPM_ENUM("Mux", hi6402es_dacsl_mux_enum);

/* DACSR MUX 0x7215 */
static const char *hi6402es_dacsr_mux_texts[] = {
	"DSP IF8 OUT",
	"S1 R",
	"S2 R",
	"S3 R",
	"S4 R",
	"SHARE MIXER",
};
static const struct soc_enum hi6402es_dacsr_mux_enum =
	SOC_ENUM_SINGLE(HI6402ES_DACS_MUX_SEL, HI6402ES_DACSR_MUX_BIT,
		ARRAY_SIZE(hi6402es_dacsr_mux_texts), hi6402es_dacsr_mux_texts);
static const struct snd_kcontrol_new hi6402es_dapm_dacsr_mux_controls =
	SOC_DAPM_ENUM("Mux", hi6402es_dacsr_mux_enum);

/* DACL MUX 0x7215 */
static const char *hi6402es_dacl_src_mixer_mux_texts[] = {
	"DSP IF8 OUT",
	"DSP IF8 IN",
};
static const struct soc_enum hi6402es_dacl_src_mixer_mux_enum =
	SOC_ENUM_SINGLE(HI6402ES_DAC_SRC_MIXER_MUX_SEL, HI6402ES_DACL_SRC_MIXER_MUX_BIT,
		ARRAY_SIZE(hi6402es_dacl_src_mixer_mux_texts), hi6402es_dacl_src_mixer_mux_texts);
static const struct snd_kcontrol_new hi6402es_dapm_dacl_src_mixer_mux_controls =
	SOC_DAPM_ENUM("Mux", hi6402es_dacl_src_mixer_mux_enum);

/* DACR MUX 0x7215 */
static const char *hi6402es_dacr_src_mixer_mux_texts[] = {
	"DSP IF8 OUT",
	"DSP IF8 IN",
};
static const struct soc_enum hi6402es_dacr_src_mixer_mux_enum =
	SOC_ENUM_SINGLE(HI6402ES_DAC_SRC_MIXER_MUX_SEL, HI6402ES_DACR_SRC_MIXER_MUX_BIT,
		ARRAY_SIZE(hi6402es_dacr_src_mixer_mux_texts), hi6402es_dacr_src_mixer_mux_texts);
static const struct snd_kcontrol_new hi6402es_dapm_dacr_src_mixer_mux_controls =
	SOC_DAPM_ENUM("Mux", hi6402es_dacr_src_mixer_mux_enum);

/* HPL HPR LOL LOR MUX 0x728E-0x728F */
static const char *hi6402es_dac_mux_texts[] = {
	"DACL",
	"DACSL",
	"DACR",
	"DACSR",
	"Off",
};
static const char *hi6402es_ep_dac_mux_texts[] = {
	"DACL",
	"DACSL",
	"DACR",
	"DACSR",
};
static const struct soc_enum hi6402es_dac_hpl_mux_enum =
	SOC_ENUM_SINGLE(HI6402ES_DAC_HP_MUX_SEL, HI6402ES_DAC_HPL_MUX_BIT,
		ARRAY_SIZE(hi6402es_dac_mux_texts), hi6402es_dac_mux_texts);
static const struct snd_kcontrol_new hi6402es_dapm_dac_hpl_mux_controls =
	SOC_DAPM_ENUM("Mux", hi6402es_dac_hpl_mux_enum);

static const struct soc_enum hi6402es_dac_hpr_mux_enum =
	SOC_ENUM_SINGLE(HI6402ES_DAC_HP_MUX_SEL, HI6402ES_DAC_HPR_MUX_BIT,
		ARRAY_SIZE(hi6402es_dac_mux_texts), hi6402es_dac_mux_texts);
static const struct snd_kcontrol_new hi6402es_dapm_dac_hpr_mux_controls =
	SOC_DAPM_ENUM("Mux", hi6402es_dac_hpr_mux_enum);

static const struct soc_enum hi6402es_dac_ep_mux_enum =
	SOC_ENUM_SINGLE(HI6402ES_DAC_EP_MUX_SEL, HI6402ES_DAC_EP_MUX_BIT,
		ARRAY_SIZE(hi6402es_ep_dac_mux_texts), hi6402es_ep_dac_mux_texts);
static const struct snd_kcontrol_new hi6402es_dapm_dac_ep_mux_controls =
	SOC_DAPM_ENUM("Mux", hi6402es_dac_ep_mux_enum);

static const struct soc_enum hi6402es_dac_ltl_mux_enum =
	SOC_ENUM_SINGLE(HI6402ES_DAC_LT_MUX_SEL, HI6402ES_DAC_LTL_MUX_BIT,
		ARRAY_SIZE(hi6402es_dac_mux_texts), hi6402es_dac_mux_texts);
static const struct snd_kcontrol_new hi6402es_dapm_dac_ltl_mux_controls =
	SOC_DAPM_ENUM("Mux", hi6402es_dac_ltl_mux_enum);

static const struct soc_enum hi6402es_dac_ltr_mux_enum =
	SOC_ENUM_SINGLE(HI6402ES_DAC_LT_MUX_SEL, HI6402ES_DAC_LTR_MUX_BIT,
		ARRAY_SIZE(hi6402es_dac_mux_texts), hi6402es_dac_mux_texts);
static const struct snd_kcontrol_new hi6402es_dapm_dac_ltr_mux_controls =
	SOC_DAPM_ENUM("Mux", hi6402es_dac_ltr_mux_enum);

/* ADCL0 MUX 0x7294 */
static const char *hi6402es_adc_mux_texts[] = {
	"MAINMIC",
	"AUXMIC",
	"LINEINL",
	"LINEINR",
	"DMIC0L",
	"DMIC0R",
	"DMIC1L",
	"DMIC1R",
	"IO INPUT",
};
static const struct soc_enum hi6402es_adcl0_mux_enum =
	SOC_ENUM_SINGLE(HI6402ES_ADC0_MUX_SEL, HI6402ES_ADCL0_MUX_BIT,
		ARRAY_SIZE(hi6402es_adc_mux_texts), hi6402es_adc_mux_texts);
static const struct snd_kcontrol_new hi6402es_dapm_adcl0_mux_controls =
	SOC_DAPM_ENUM("Mux", hi6402es_adcl0_mux_enum);

/* ADCR0 MUX 0x7294 */
static const struct soc_enum hi6402es_adcr0_mux_enum =
	SOC_ENUM_SINGLE(HI6402ES_ADC0_MUX_SEL, HI6402ES_ADCR0_MUX_BIT,
		ARRAY_SIZE(hi6402es_adc_mux_texts), hi6402es_adc_mux_texts);
static const struct snd_kcontrol_new hi6402es_dapm_adcr0_mux_controls =
	SOC_DAPM_ENUM("Mux", hi6402es_adcr0_mux_enum);

/* ADCL1 MUX 0x7295 */
static const struct soc_enum hi6402es_adcl1_mux_enum =
	SOC_ENUM_SINGLE(HI6402ES_ADC1_MUX_SEL, HI6402ES_ADCL1_MUX_BIT,
		ARRAY_SIZE(hi6402es_adc_mux_texts), hi6402es_adc_mux_texts);
static const struct snd_kcontrol_new hi6402es_dapm_adcl1_mux_controls =
	SOC_DAPM_ENUM("Mux", hi6402es_adcl1_mux_enum);

/* ADCR0 MUX 0x7295 */
static const struct soc_enum hi6402es_adcr1_mux_enum =
	SOC_ENUM_SINGLE(HI6402ES_ADC1_MUX_SEL, HI6402ES_ADCR1_MUX_BIT,
		ARRAY_SIZE(hi6402es_adc_mux_texts), hi6402es_adc_mux_texts);
static const struct snd_kcontrol_new hi6402es_dapm_adcr1_mux_controls =
	SOC_DAPM_ENUM("Mux", hi6402es_adcr1_mux_enum);

/* LINEL PGA SEL 0x70A6*/
static const char *hi6402es_linel_pga_sel_texts[] = {
	"LINEIN1",
	"LINEIN2",
	"LINEOUT",
	"MIC2",
};
static const struct soc_enum hi6402es_linel_pga_sel_enum =
	SOC_ENUM_SINGLE(HI6402ES_LINEL_PGA_SEL_REG, HI6402ES_LINEL_PGA_SEL_BIT,
		ARRAY_SIZE(hi6402es_linel_pga_sel_texts), hi6402es_linel_pga_sel_texts);
static const struct snd_kcontrol_new hi6402es_dapm_linel_pga_sel_controls =
	SOC_DAPM_ENUM("Mux", hi6402es_linel_pga_sel_enum);

/* LINER PGA SEL 0x70A7 */
static const char *hi6402es_liner_pga_sel_texts[] = {
	"LINEIN1",
	"LINEIN2",
	"LINEOUT",
	"MIC3",
};
static const struct soc_enum hi6402es_liner_pga_sel_enum =
	SOC_ENUM_SINGLE(HI6402ES_LINER_PGA_SEL_REG, HI6402ES_LINER_PGA_SEL_BIT,
		ARRAY_SIZE(hi6402es_liner_pga_sel_texts), hi6402es_liner_pga_sel_texts);
static const struct snd_kcontrol_new hi6402es_dapm_liner_pga_sel_controls =
	SOC_DAPM_ENUM("Mux", hi6402es_liner_pga_sel_enum);

/* MIC MUX 0x70A9 */
static const char *hi6402es_main_pga_sel_texts[] = {
	"HS MIC",
	"MAIN MIC",
};
static const struct soc_enum hi6402es_main_pga_sel_enum =
	SOC_ENUM_SINGLE(HI6402ES_MAIN_PGA_SEL_REG, HI6402ES_MAIN_PGA_SEL_BIT,
		ARRAY_SIZE(hi6402es_main_pga_sel_texts), hi6402es_main_pga_sel_texts);
static const struct snd_kcontrol_new hi6402es_dapm_main_pga_sel_controls =
	SOC_DAPM_ENUM("Mux", hi6402es_main_pga_sel_enum);


/* MIXER */

/* DACL MIXER */
static const struct snd_kcontrol_new hi6402es_dacl_mixer_controls[] = {
	SOC_DAPM_SINGLE("S1L MUX SWITCH",
		HI6402ES_DACL_MIXER_SEL_0, HI6402ES_DACL_MIXER_S1_BIT, 1, 1),
	SOC_DAPM_SINGLE("S2L SWITCH",
		HI6402ES_DACL_MIXER_SEL_0, HI6402ES_DACL_MIXER_S2L_BIT, 1, 1),
	SOC_DAPM_SINGLE("MDM SWITCH",
		HI6402ES_DACL_MIXER_SEL_0, HI6402ES_DACL_MIXER_MDM_BIT, 1, 1),	
	SOC_DAPM_SINGLE("SIDETONE SWITCH",
		HI6402ES_DACL_MIXER_SEL_1, HI6402ES_DACL_MIXER_SIDETONE_BIT, 1, 1),	
};

/* DACR MIXER */
static const struct snd_kcontrol_new hi6402es_dacr_mixer_controls[] = {
	SOC_DAPM_SINGLE("S1R MUX SWITCH",
		HI6402ES_DACR_MIXER_SEL_0, HI6402ES_DACR_MIXER_S1_BIT, 1, 1),
	SOC_DAPM_SINGLE("S2R SWITCH",
		HI6402ES_DACR_MIXER_SEL_0, HI6402ES_DACR_MIXER_S2R_BIT, 1, 1),
	SOC_DAPM_SINGLE("MDM SWITCH",
		HI6402ES_DACR_MIXER_SEL_0, HI6402ES_DACR_MIXER_MDM_BIT, 1, 1),	
	SOC_DAPM_SINGLE("SIDETONE SWITCH",
		HI6402ES_DACR_MIXER_SEL_1, HI6402ES_DACR_MIXER_SIDETONE_BIT, 1, 1),	
};

/* SHARE MIXER */
static const struct snd_kcontrol_new hi6402es_share_mixer_controls[] = {
	SOC_DAPM_SINGLE("S1L SWITCH",
		HI6402ES_SHARE_MIXER_SEL_0, HI6402ES_SHARE_MIXER_0_BIT, 1, 1),
	SOC_DAPM_SINGLE("S1R SWITCH",
		HI6402ES_SHARE_MIXER_SEL_0, HI6402ES_SHARE_MIXER_1_BIT, 1, 1),
};

/* DACL SRC MIXER */
static const struct snd_kcontrol_new hi6402es_dacl_src_mixer_controls[] = {
	SOC_DAPM_SINGLE("DSP IF8 SWITCH",
		HI6402ES_DACSL_MIXER_SEL, HI6402ES_DACSL_MIXER_0_BIT, 1, 1),
	SOC_DAPM_SINGLE("SIDETONE SWITCH",
		HI6402ES_DACSL_MIXER_SEL, HI6402ES_DACSL_MIXER_1_BIT, 1, 1),	
};

/* DACR SRC MIXER */
static const struct snd_kcontrol_new hi6402es_dacr_src_mixer_controls[] = {
	SOC_DAPM_SINGLE("DSP IF8 SWITCH",
		HI6402ES_DACSR_MIXER_SEL, HI6402ES_DACSR_MIXER_0_BIT, 1, 1),
	SOC_DAPM_SINGLE("SIDETONE SWITCH",
		HI6402ES_DACSR_MIXER_SEL, HI6402ES_DACSR_MIXER_1_BIT, 1, 1),
};

/* DAPM WIDGETS */
static const struct snd_soc_dapm_widget hi6402es_dapm_widgets[] = {
	/* INPUT */
	SND_SOC_DAPM_INPUT("LINEIN1"),
	SND_SOC_DAPM_INPUT("LINEIN2"),
	SND_SOC_DAPM_INPUT("LINE OUTL"),
	SND_SOC_DAPM_INPUT("LINE OUTR"),
	SND_SOC_DAPM_INPUT("MAINMIC"),
	SND_SOC_DAPM_INPUT("AUXMIC"),
	SND_SOC_DAPM_INPUT("HSMIC"),
	SND_SOC_DAPM_INPUT("MIC2"),
	SND_SOC_DAPM_INPUT("MIC3"),

	SND_SOC_DAPM_INPUT("S1L IN"),
	SND_SOC_DAPM_INPUT("S1R IN"),
	SND_SOC_DAPM_INPUT("S2L IN"),
	SND_SOC_DAPM_INPUT("S2R IN"),
	SND_SOC_DAPM_INPUT("S3L IN"),
	SND_SOC_DAPM_INPUT("S3R IN"),
	SND_SOC_DAPM_INPUT("S4L IN"),
	SND_SOC_DAPM_INPUT("S4R IN"),
	
	SND_SOC_DAPM_INPUT("DIGMIC0"),
	SND_SOC_DAPM_INPUT("DIGMIC1"),

	/* OUTPUT */
	/* ANA OUT */
	SND_SOC_DAPM_OUTPUT("EAR OUT"),
	SND_SOC_DAPM_OUTPUT("HPL OUT"),
	SND_SOC_DAPM_OUTPUT("HPR OUT"),
	SND_SOC_DAPM_OUTPUT("LINEOUT L"),
	SND_SOC_DAPM_OUTPUT("LINEOUT R"),
	/* DIG OUT */
	SND_SOC_DAPM_OUTPUT("MAD OUT"),
	SND_SOC_DAPM_OUTPUT("S1L OUT"),
	SND_SOC_DAPM_OUTPUT("S1R OUT"),
	SND_SOC_DAPM_OUTPUT("S2L OUT"),
	SND_SOC_DAPM_OUTPUT("S2R OUT"),
	SND_SOC_DAPM_OUTPUT("S3L OUT"),
	SND_SOC_DAPM_OUTPUT("S3R OUT"),
	SND_SOC_DAPM_OUTPUT("S4L OUT"),
	SND_SOC_DAPM_OUTPUT("S4R OUT"),
	/* SMART PA OUT */
	SND_SOC_DAPM_OUTPUT("SP L OUT"),
	SND_SOC_DAPM_OUTPUT("SP R OUT"),

	/* MICS */
	SND_SOC_DAPM_MIC("MAIN MIC", hi6402es_main_micbias_power_mode_event),
	SND_SOC_DAPM_MIC("AUX MIC", hi6402es_aux_micbias_power_mode_event),
	SND_SOC_DAPM_MIC("HS MIC", hi6402es_hs_micbias_power_mode_event),
	SND_SOC_DAPM_MIC("MIC2 IN", hi6402es_mic2_micbias_power_mode_event),
	SND_SOC_DAPM_MIC("MIC3 IN", hi6402es_mic3_micbias_power_mode_event),

	/* SUPPLY */
	SND_SOC_DAPM_SUPPLY("IBIAS",
		SND_SOC_NOPM, 0, 0,
		hi6402es_ibias_power_mode_event,
		SND_SOC_DAPM_PRE_PMU | SND_SOC_DAPM_POST_PMD),
	SND_SOC_DAPM_SUPPLY("PLL CLK",
		SND_SOC_NOPM, 0, 0,
		hi6402es_pll_power_mode_event,
		SND_SOC_DAPM_PRE_PMU | SND_SOC_DAPM_POST_PMD),
	SND_SOC_DAPM_SUPPLY("MAD CLK",
		SND_SOC_NOPM, 0, 0,
		hi6402es_mad_power_mode_event,
		SND_SOC_DAPM_PRE_PMU | SND_SOC_DAPM_POST_PMD),
	SND_SOC_DAPM_SUPPLY("AUDIO CLK",
		HI6402ES_AUD_CLK_EN, HI6402ES_AUD_CLK_EN_BIT, 0, NULL, 0),
	SND_SOC_DAPM_SUPPLY("S1 TX CLK",
		SND_SOC_NOPM, 0, 0,
		hi6402es_s1_tx_clk_power_mode_event,
		SND_SOC_DAPM_PRE_PMU | SND_SOC_DAPM_POST_PMD),
	SND_SOC_DAPM_SUPPLY("S1 RX CLK",
		SND_SOC_NOPM, 0, 0,
		hi6402es_s1_rx_clk_power_mode_event,
		SND_SOC_DAPM_PRE_PMU | SND_SOC_DAPM_POST_PMD),
	SND_SOC_DAPM_SUPPLY("S1 INTERFACE CLK",
		SND_SOC_NOPM, 0, 0,
		hi6402es_s1_interface_clk_power_mode_event,
		SND_SOC_DAPM_PRE_PMU | SND_SOC_DAPM_POST_PMD),

	SND_SOC_DAPM_SUPPLY("S2 TX CLK",
		HI6402ES_S2_PORT_CFG_L, HI6402ES_IF_TX_EN_BIT, 0,
		NULL, 0),
	SND_SOC_DAPM_SUPPLY("S2 RX CLK",
		HI6402ES_S2_PORT_CFG_L, HI6402ES_IF_RX_EN_BIT, 0,
		NULL, 0),
	SND_SOC_DAPM_SUPPLY("S2 INTERFACE CLK",
		HI6402ES_S2_FS_CFG_L, HI6402ES_PORT_CLK_EN, 0,
		NULL, 0),

	SND_SOC_DAPM_SUPPLY("S3 INTERFACE CLK",
		SND_SOC_NOPM, 0, 0,
		hi6402es_s3_interface_clk_power_mode_event,
		SND_SOC_DAPM_PRE_PMU | SND_SOC_DAPM_POST_PMD),

	SND_SOC_DAPM_SUPPLY("S4 TX CLK",
		HI6402ES_S4_PORT_CFG_L, HI6402ES_IF_TX_EN_BIT, 0,
		NULL, 0),
	SND_SOC_DAPM_SUPPLY("S4 RX CLK",
		HI6402ES_S4_PORT_CFG_L, HI6402ES_IF_RX_EN_BIT, 0,
		NULL, 0),
	SND_SOC_DAPM_SUPPLY("S4 INTERFACE CLK",
		HI6402ES_S4_FS_CFG_L, HI6402ES_PORT_CLK_EN, 0,
		NULL, 0),
	SND_SOC_DAPM_SUPPLY("CP1",
		SND_SOC_NOPM, 0, 0,
		hi6402es_cp1_power_mode_event,
		SND_SOC_DAPM_PRE_PMU | SND_SOC_DAPM_POST_PMD),
	SND_SOC_DAPM_SUPPLY("CP2",
		HI6402ES_CP2_PD_REG, HI6402ES_CP2_PD_BIT, 1,
		NULL, 0),

	/* ANALOG PGA */
	SND_SOC_DAPM_PGA_E("MAINMIC PGA",
		SND_SOC_NOPM, 0, 0,
		NULL, 0, hi6402es_mainmic_pga_power_mode_event, SND_SOC_DAPM_PRE_PMU | SND_SOC_DAPM_POST_PMD),
	SND_SOC_DAPM_PGA_E("AUXMIC PGA",
		SND_SOC_NOPM, 0, 0,
		NULL, 0, hi6402es_auxmic_pga_power_mode_event, SND_SOC_DAPM_PRE_PMU | SND_SOC_DAPM_POST_PMD),
	SND_SOC_DAPM_PGA_E("LINEINL PGA",
		SND_SOC_NOPM, 0, 0,
		NULL, 0, hi6402es_lineinl_pga_power_mode_event, SND_SOC_DAPM_PRE_PMU | SND_SOC_DAPM_POST_PMD),
	SND_SOC_DAPM_PGA_E("LINEINR PGA",
		SND_SOC_NOPM, 0, 0,
		NULL, 0, hi6402es_lineinr_pga_power_mode_event, SND_SOC_DAPM_PRE_PMU | SND_SOC_DAPM_POST_PMD),

	/* OUT DRIVER */
	SND_SOC_DAPM_OUT_DRV_E("HPL DRV",
		SND_SOC_NOPM, 0, 0,
		NULL, 0, hi6402es_hpl_drv_power_mode_event, SND_SOC_DAPM_PRE_PMU | SND_SOC_DAPM_POST_PMD),
	SND_SOC_DAPM_OUT_DRV_E("HPR DRV",
		SND_SOC_NOPM, 0, 0,
		NULL, 0, hi6402es_hpr_drv_power_mode_event, SND_SOC_DAPM_PRE_PMU | SND_SOC_DAPM_POST_PMD),
	SND_SOC_DAPM_OUT_DRV_E("EP DRV",
		SND_SOC_NOPM, 0, 0,
		NULL, 0, hi6402es_ep_drv_power_mode_event, SND_SOC_DAPM_PRE_PMU | SND_SOC_DAPM_POST_PMD),
	SND_SOC_DAPM_OUT_DRV_E("LOL DRV",
		SND_SOC_NOPM, 0, 0,
		NULL, 0, hi6402es_lol_drv_power_mode_event, SND_SOC_DAPM_PRE_PMU | SND_SOC_DAPM_POST_PMD),
	SND_SOC_DAPM_OUT_DRV_E("LOR DRV",
		SND_SOC_NOPM, 0, 0,
		NULL, 0, hi6402es_lor_drv_power_mode_event, SND_SOC_DAPM_PRE_PMU | SND_SOC_DAPM_POST_PMD),
		
	
	/* DIGITAL PGA */	
	/* PGA OF S1-S4 */
	SND_SOC_DAPM_PGA_E("S1 IL PGA",
		SND_SOC_NOPM, 0, 0,
		NULL, 0, hi6402es_s1_il_pga_power_mode_event, SND_SOC_DAPM_PRE_PMU | SND_SOC_DAPM_POST_PMD),
	SND_SOC_DAPM_PGA_E("S1 IR PGA",
		SND_SOC_NOPM, 0, 0,
		NULL, 0, hi6402es_s1_ir_pga_power_mode_event, SND_SOC_DAPM_PRE_PMU | SND_SOC_DAPM_POST_PMD),
	SND_SOC_DAPM_PGA_E("S2 IL PGA",
		SND_SOC_NOPM, 0, 0,
		NULL, 0, hi6402es_s2_il_pga_power_mode_event, SND_SOC_DAPM_PRE_PMU | SND_SOC_DAPM_POST_PMD),
	SND_SOC_DAPM_PGA_E("S2 IR PGA",
		SND_SOC_NOPM, 0, 0,
		NULL, 0, hi6402es_s2_ir_pga_power_mode_event, SND_SOC_DAPM_PRE_PMU | SND_SOC_DAPM_POST_PMD),
	SND_SOC_DAPM_PGA_E("S3 IL PGA",
		SND_SOC_NOPM, 0, 0,
		NULL, 0, hi6402es_s3_il_pga_power_mode_event, SND_SOC_DAPM_PRE_PMU | SND_SOC_DAPM_POST_PMD),
	SND_SOC_DAPM_PGA_E("S3 IR PGA",
		SND_SOC_NOPM, 0, 0,
		NULL, 0, hi6402es_s3_ir_pga_power_mode_event, SND_SOC_DAPM_PRE_PMU | SND_SOC_DAPM_POST_PMD),
	SND_SOC_DAPM_PGA_E("S4 IL PGA",
		SND_SOC_NOPM, 0, 0,
		NULL, 0, hi6402es_s4_il_pga_power_mode_event, SND_SOC_DAPM_PRE_PMU | SND_SOC_DAPM_POST_PMD),
	SND_SOC_DAPM_PGA_E("S4 IR PGA",
		SND_SOC_NOPM, 0, 0,
		NULL, 0, hi6402es_s4_ir_pga_power_mode_event, SND_SOC_DAPM_PRE_PMU | SND_SOC_DAPM_POST_PMD),
	
	/* PGA FOR DAC */
	SND_SOC_DAPM_PGA_E("DACL PGA",
		SND_SOC_NOPM, 0, 0,
		NULL, 0, hi6402es_dacl_pga_power_mode_event, SND_SOC_DAPM_PRE_PMU | SND_SOC_DAPM_POST_PMD),
	SND_SOC_DAPM_PGA_E("DACR PGA",
		SND_SOC_NOPM, 0, 0,
		NULL, 0, hi6402es_dacr_pga_power_mode_event, SND_SOC_DAPM_PRE_PMU | SND_SOC_DAPM_POST_PMD),
	
	/* PGA FOR ADC */
	SND_SOC_DAPM_PGA("ADC1R",
		HI6402ES_DAC_ADC_CLK_REG, HI6402ES_ADC1R_EN_BIT, 0,
		NULL, 0),
	SND_SOC_DAPM_PGA("ADC1L",
		HI6402ES_DAC_ADC_CLK_REG, HI6402ES_ADC1L_EN_BIT, 0,
		NULL, 0),
	SND_SOC_DAPM_PGA("ADC0R",
		HI6402ES_DAC_ADC_CLK_REG, HI6402ES_ADC0R_EN_BIT, 0,
		NULL, 0),
	SND_SOC_DAPM_PGA("ADC0L",
		HI6402ES_DAC_ADC_CLK_REG, HI6402ES_ADC0L_EN_BIT, 0,
		NULL, 0),

	/* PGA FOR SIDETONE PGA */
	SND_SOC_DAPM_PGA_E("SIDE PGA",
		SND_SOC_NOPM, 0, 0,
		NULL, 0, hi6402es_side_pga_power_mode_event, SND_SOC_DAPM_PRE_PMU | SND_SOC_DAPM_POST_PMD),

	/* PGA FOR PDM */
	SND_SOC_DAPM_PGA("PDM",
		HI6402ES_MDM_SRC_SEL, HI6402ES_PDM_EN_BIT, 0,
		NULL, 0),	
	
	/* SWITCH FOR DACS */		
	SND_SOC_DAPM_SWITCH_E("DACSL SWITCH",
		SND_SOC_NOPM, 0, 0,
		&hi6402es_dapm_dacsl_switch_controls,
		hi6402es_dacsl_power_mode_event,
		SND_SOC_DAPM_PRE_PMU | SND_SOC_DAPM_POST_PMU | SND_SOC_DAPM_PRE_PMD | SND_SOC_DAPM_POST_PMD),
	SND_SOC_DAPM_SWITCH_E("DACSR SWITCH",
		SND_SOC_NOPM, 0, 0,
		&hi6402es_dapm_dacsr_switch_controls,
		hi6402es_dacsr_power_mode_event,
		SND_SOC_DAPM_PRE_PMU | SND_SOC_DAPM_POST_PMU | SND_SOC_DAPM_PRE_PMD | SND_SOC_DAPM_POST_PMD),
		
	/* SWITCH FOR  S1-S4 OUT */
	SND_SOC_DAPM_SWITCH_E("MAD SWITCH",
		HI6402ES_SC_MAD_CTRL_18, HI6402ES_MAD_EN, 0,
		&hi6402es_dapm_mad_switch_controls,
		hi6402es_mad_src_power_mode_event, SND_SOC_DAPM_PRE_PMU | SND_SOC_DAPM_POST_PMD),
	SND_SOC_DAPM_SWITCH_E("S1 OL SWITCH",
		SND_SOC_NOPM, 0, 0,
		&hi6402es_dapm_s1_ol_switch_controls,
		hi6402es_s1_ol_switch_power_mode_event, SND_SOC_DAPM_PRE_PMU | SND_SOC_DAPM_POST_PMD),
	SND_SOC_DAPM_SWITCH_E("S1 OR SWITCH",
		SND_SOC_NOPM, 0, 0,
		&hi6402es_dapm_s1_or_switch_controls,
		hi6402es_s1_or_switch_power_mode_event, SND_SOC_DAPM_PRE_PMU | SND_SOC_DAPM_POST_PMD),
	SND_SOC_DAPM_SWITCH_E("S2 OL SWITCH",
		SND_SOC_NOPM, 0, 0,
		&hi6402es_dapm_s2_ol_switch_controls,
		hi6402es_s2_ol_switch_power_mode_event, SND_SOC_DAPM_PRE_PMU | SND_SOC_DAPM_POST_PMD),
	SND_SOC_DAPM_SWITCH_E("S2 OR SWITCH",
		SND_SOC_NOPM, 0, 0,
		&hi6402es_dapm_s2_or_switch_controls,
		hi6402es_s2_or_switch_power_mode_event, SND_SOC_DAPM_PRE_PMU | SND_SOC_DAPM_POST_PMD),
	SND_SOC_DAPM_SWITCH_E("S3 OL SWITCH",
		SND_SOC_NOPM, 0, 0,
		&hi6402es_dapm_s3_ol_switch_controls,
		hi6402es_s3_ol_switch_power_mode_event, SND_SOC_DAPM_PRE_PMU | SND_SOC_DAPM_POST_PMD),
	SND_SOC_DAPM_SWITCH_E("S3 OR SWITCH",
		SND_SOC_NOPM, 0, 0,
		&hi6402es_dapm_s3_or_switch_controls,
		hi6402es_s3_or_switch_power_mode_event, SND_SOC_DAPM_PRE_PMU | SND_SOC_DAPM_POST_PMD),
	SND_SOC_DAPM_SWITCH_E("S4 OL SWITCH",
		SND_SOC_NOPM, 0, 0,
		&hi6402es_dapm_s4_ol_switch_controls,
		hi6402es_s4_ol_switch_power_mode_event, SND_SOC_DAPM_PRE_PMU | SND_SOC_DAPM_POST_PMD),
	SND_SOC_DAPM_SWITCH_E("S4 OR SWITCH",
		SND_SOC_NOPM, 0, 0,
		&hi6402es_dapm_s4_or_switch_controls,
		hi6402es_s4_or_switch_power_mode_event, SND_SOC_DAPM_PRE_PMU | SND_SOC_DAPM_POST_PMD),

	/* SWITCH FOR SMART PA */
	SND_SOC_DAPM_SWITCH("SP IL SWITCH",
		SND_SOC_NOPM, 0, 0,
		&hi6402es_dapm_sp_il_switch_controls),
	SND_SOC_DAPM_SWITCH("SP IR SWITCH",
		SND_SOC_NOPM, 0, 0,
		&hi6402es_dapm_sp_ir_switch_controls),

	/* SWITCH FOR EP DAC MUX */
	SND_SOC_DAPM_SWITCH("EP SWITCH",
		SND_SOC_NOPM, 0, 0,
		&hi6402es_dapm_ep_switch_controls),


	/* MUX */
	SND_SOC_DAPM_MUX("APL MUX",
		SND_SOC_NOPM, 0, 0,
		&hi6402es_dapm_apl_mux_controls),
	SND_SOC_DAPM_MUX("APR MUX",
		SND_SOC_NOPM, 0, 0,
		&hi6402es_dapm_apr_mux_controls),
	SND_SOC_DAPM_MUX("BTL MUX",
		SND_SOC_NOPM, 0, 0,
		&hi6402es_dapm_btl_mux_controls),
	SND_SOC_DAPM_MUX("BTR MUX",
		SND_SOC_NOPM, 0, 0,
		&hi6402es_dapm_btr_mux_controls),
	SND_SOC_DAPM_MUX("M0L MUX",
		SND_SOC_NOPM, 0, 0,
		&hi6402es_dapm_m0l_mux_controls),
	SND_SOC_DAPM_MUX("M0R MUX",
		SND_SOC_NOPM, 0, 0,
		&hi6402es_dapm_m0r_mux_controls),
	SND_SOC_DAPM_MUX("M1L MUX",
		SND_SOC_NOPM, 0, 0,
		&hi6402es_dapm_m1l_mux_controls),
	SND_SOC_DAPM_MUX("M1R MUX",
		SND_SOC_NOPM, 0, 0,
		&hi6402es_dapm_m1r_mux_controls),

	SND_SOC_DAPM_MUX("MIXERL S1 MUX",
		SND_SOC_NOPM, 0, 0,
		&hi6402es_dapm_dacl_mixer_mux_controls),
	SND_SOC_DAPM_MUX("MIXERR S1 MUX",
		SND_SOC_NOPM, 0, 0,
		&hi6402es_dapm_dacr_mixer_mux_controls),

	SND_SOC_DAPM_MUX("MDM MUX",
		SND_SOC_NOPM, 0, 0,
		&hi6402es_dapm_mdm_src_mux_controls),
	SND_SOC_DAPM_MUX("SIDETONE MUX",
		SND_SOC_NOPM, 0, 0,
		&hi6402es_dapm_sidetone_src_mux_controls),

	SND_SOC_DAPM_MUX("DACSL MUX",
		SND_SOC_NOPM, 0, 0,
		&hi6402es_dapm_dacsl_mux_controls),	
	SND_SOC_DAPM_MUX("DACSR MUX",
		SND_SOC_NOPM, 0, 0,
		&hi6402es_dapm_dacsr_mux_controls),	

	SND_SOC_DAPM_MUX("DACL MUX",
		SND_SOC_NOPM, 0, 0,
		&hi6402es_dapm_dacl_src_mixer_mux_controls),	
	SND_SOC_DAPM_MUX("DACR MUX",
		SND_SOC_NOPM, 0, 0,
		&hi6402es_dapm_dacr_src_mixer_mux_controls),

	SND_SOC_DAPM_MUX("HPL MUX",
		SND_SOC_NOPM, 0, 0,
		&hi6402es_dapm_dac_hpl_mux_controls),
	SND_SOC_DAPM_MUX("HPR MUX",
		SND_SOC_NOPM, 0, 0,
		&hi6402es_dapm_dac_hpr_mux_controls),

	SND_SOC_DAPM_MUX("EP MUX",
		SND_SOC_NOPM, 0, 0,
		&hi6402es_dapm_dac_ep_mux_controls),

	SND_SOC_DAPM_MUX("LOL MUX",
		SND_SOC_NOPM, 0, 0,
		&hi6402es_dapm_dac_ltl_mux_controls),
	SND_SOC_DAPM_MUX("LOR MUX",
		SND_SOC_NOPM, 0, 0,
		&hi6402es_dapm_dac_ltr_mux_controls),

	SND_SOC_DAPM_MUX("ADC0L MUX",
		SND_SOC_NOPM, 0, 0,
		&hi6402es_dapm_adcl0_mux_controls),
	SND_SOC_DAPM_MUX("ADC0R MUX",
		SND_SOC_NOPM, 0, 0,
		&hi6402es_dapm_adcr0_mux_controls),
	SND_SOC_DAPM_MUX("ADC1L MUX",
		SND_SOC_NOPM, 0, 0,
		&hi6402es_dapm_adcl1_mux_controls),
	SND_SOC_DAPM_MUX("ADC1R MUX",
		SND_SOC_NOPM, 0, 0,
		&hi6402es_dapm_adcr1_mux_controls),

	SND_SOC_DAPM_MUX("LIL MUX",
		SND_SOC_NOPM, 0, 0,
		&hi6402es_dapm_linel_pga_sel_controls),
	SND_SOC_DAPM_MUX("LIR MUX",
		SND_SOC_NOPM, 0, 0,
		&hi6402es_dapm_liner_pga_sel_controls),
	SND_SOC_DAPM_MUX("MIC MUX",
		SND_SOC_NOPM, 0, 0,
		&hi6402es_dapm_main_pga_sel_controls),

	/* MIXER */
	SND_SOC_DAPM_MIXER("MIXERL",
		HI6402ES_DAC_MIXER_CLK_REG, HI6402ES_DACL_MIXER_EN_BIT, 0,
		hi6402es_dacl_mixer_controls,
		ARRAY_SIZE(hi6402es_dacl_mixer_controls)),
	SND_SOC_DAPM_MIXER("MIXERR",
		HI6402ES_DAC_MIXER_CLK_REG, HI6402ES_DACR_MIXER_EN_BIT, 0,
		hi6402es_dacr_mixer_controls,
		ARRAY_SIZE(hi6402es_dacr_mixer_controls)),
	SND_SOC_DAPM_MIXER("SHARE MIXER",
		HI6402ES_EQ_SM_CLK_REG, HI6402ES_SHARE_MIXER_EN_BIT, 0,
		hi6402es_share_mixer_controls,
		ARRAY_SIZE(hi6402es_share_mixer_controls)),
	SND_SOC_DAPM_MIXER("DACL MIXER",
		HI6402ES_DAC_SRC_MIXER_CLK_REG, HI6402ES_DACL_SRC_MIXER_EN_BIT, 0,
		hi6402es_dacl_src_mixer_controls,
		ARRAY_SIZE(hi6402es_dacl_src_mixer_controls)),
	SND_SOC_DAPM_MIXER("DACR MIXER",
		HI6402ES_DAC_SRC_MIXER_CLK_REG, HI6402ES_DACR_SRC_MIXER_EN_BIT, 0,
		hi6402es_dacr_src_mixer_controls,
		ARRAY_SIZE(hi6402es_dacr_src_mixer_controls)),
};

static const struct snd_soc_dapm_route route_map[] = {
	{"S1 IL PGA",		NULL,			"S1L IN"},
	{"S1 IR PGA",		NULL,			"S1R IN"},
	{"S2 IL PGA",		NULL,			"S2L IN"},
	{"S2 IR PGA",		NULL,			"S2R IN"},
	{"S3 IL PGA",		NULL,			"S3L IN"},
	{"S3 IR PGA",		NULL,			"S3R IN"},
	{"S4 IL PGA",		NULL,			"S4L IN"},
	{"S4 IR PGA",		NULL,			"S4R IN"},

	{"MAD OUT",		NULL,			"MAD SWITCH"},
	{"S1L OUT",		NULL,			"S1 OL SWITCH"},
	{"S1R OUT",		NULL,			"S1 OR SWITCH"},
	{"S2L OUT",		NULL,			"S2 OL SWITCH"},
	{"S2R OUT",		NULL,			"S2 OR SWITCH"},
	{"S3L OUT",		NULL,			"S3 OL SWITCH"},
	{"S3R OUT",		NULL,			"S3 OR SWITCH"},
	{"S4L OUT",		NULL,			"S4 OL SWITCH"},
	{"S4R OUT",		NULL,			"S4 OR SWITCH"},

	/* CLK SUPPLY */
	{"S1L IN",		NULL,			"S1 RX CLK"},
	{"S1R IN",		NULL,			"S1 RX CLK"},
	{"S1L OUT",		NULL,			"S1 TX CLK"},
	{"S1R OUT",		NULL,			"S1 TX CLK"},
	{"MAD OUT",		NULL,			"S1 TX CLK"},
	{"S1 TX CLK",		NULL,			"S1 INTERFACE CLK"},
	{"S1 RX CLK",		NULL,			"S1 INTERFACE CLK"},

	{"S2L IN",		NULL,			"S2 RX CLK"},
	{"S2R IN",		NULL,			"S2 RX CLK"},
	{"S2L OUT",		NULL,			"S2 TX CLK"},
	{"S2R OUT",		NULL,			"S2 TX CLK"},
	{"S2 TX CLK",		NULL,			"S2 INTERFACE CLK"},
	{"S2 RX CLK",		NULL,			"S2 INTERFACE CLK"},

	{"S3L IN",		NULL,			"S3 INTERFACE CLK"},
	{"S3R IN",		NULL,			"S3 INTERFACE CLK"},
	{"S3L OUT",		NULL,			"S3 INTERFACE CLK"},
	{"S3R OUT",		NULL,			"S3 INTERFACE CLK"},

	{"S4L IN",		NULL,			"S4 RX CLK"},
	{"S4R IN",		NULL,			"S4 RX CLK"},
	{"S4L OUT",		NULL,			"S4 TX CLK"},
	{"S4R OUT",		NULL,			"S4 TX CLK"},
	{"S4 TX CLK",		NULL,			"S4 INTERFACE CLK"},
	{"S4 RX CLK",		NULL,			"S4 INTERFACE CLK"},

	{"S1 INTERFACE CLK",		NULL,			"AUDIO CLK"},
	{"S2 INTERFACE CLK",		NULL,			"AUDIO CLK"},
	{"S3 INTERFACE CLK",		NULL,			"AUDIO CLK"},
	{"S4 INTERFACE CLK",		NULL,			"AUDIO CLK"},
	//{"S1 INTERFACE CLK",		NULL,			"PLL CLK"},
	{"S1L OUT",					NULL,			"PLL CLK"},
	{"S1R OUT",					NULL,			"PLL CLK"},
	{"S2 INTERFACE CLK",		NULL,			"PLL CLK"},
	{"S3 INTERFACE CLK",		NULL,			"PLL CLK"},
	{"S4 INTERFACE CLK",		NULL,			"PLL CLK"},
	{"MAD OUT",			NULL,			"MAD CLK"},
	{"MAD OUT",			NULL,			"AUDIO CLK"},
	{"PLL CLK",			NULL,			"IBIAS"},
	{"MAD CLK",			NULL,			"IBIAS"},

	{"MAD SWITCH",			"SWITCH",		"APL MUX"},
	{"S1 OL SWITCH",		"SWITCH",		"APL MUX"},
	{"S1 OR SWITCH",		"SWITCH",		"APR MUX"},
	{"S2 OL SWITCH",		"SWITCH",		"BTL MUX"},
	{"S2 OR SWITCH",		"SWITCH",		"BTR MUX"},
	{"S3 OL SWITCH",		"SWITCH",		"M0L MUX"},
	{"S3 OR SWITCH",		"SWITCH",		"M0R MUX"},
	{"S4 OL SWITCH",		"SWITCH",		"M1L MUX"},
	{"S4 OR SWITCH",		"SWITCH",		"M1R MUX"},
	
	/* SHARE MIXER */
	{"SHARE MIXER",		"S1L SWITCH",		"S1 IL PGA"},
	{"SHARE MIXER",		"S1R SWITCH",		"S1 IR PGA"},
	
	/* DAC MIXER MUX */
	{"MIXERL S1 MUX",		"S1 IL",		"S1 IL PGA"},
	{"MIXERL S1 MUX",		"S2 IL",		"S2 IL PGA"},
	{"MIXERL S1 MUX",		"S3 IL",		"S3 IL PGA"},
	{"MIXERL S1 MUX",		"S4 IL",		"S4 IL PGA"},
	{"MIXERL S1 MUX",		"SHARE MIXER",		"SHARE MIXER"},

	{"MIXERR S1 MUX",		"S1 IR",		"S1 IR PGA"},
	{"MIXERR S1 MUX",		"S2 IR",		"S2 IR PGA"},
	{"MIXERR S1 MUX",		"S3 IR",		"S3 IR PGA"},
	{"MIXERR S1 MUX",		"S4 IR",		"S4 IR PGA"},
	{"MIXERR S1 MUX",		"SHARE MIXER",		"SHARE MIXER"},

	/* DAC MIXER */
	{"MIXERL",		"S1L MUX SWITCH",		"MIXERL S1 MUX"},
	{"MIXERL",		"S2L SWITCH",			"S2 IL PGA"},
	{"MIXERL",		"MDM SWITCH",			"MDM MUX"},
	{"MIXERL",		"SIDETONE SWITCH",		"SIDE PGA"},

	{"MIXERR",		"S1R MUX SWITCH",		"MIXERR S1 MUX"},
	{"MIXERR",		"S2R SWITCH",			"S2 IR PGA"},
	{"MIXERR",		"MDM SWITCH",			"MDM MUX"},
	{"MIXERR",		"SIDETONE SWITCH",		"SIDE PGA"},
	
	/* DAC SRC MIXER MUX */
	{"DACL MUX",		"DSP IF8 IN",			"MIXERL"},
	{"DACL MUX",		"DSP IF8 OUT",			"MIXERL"},

	{"DACR MUX",		"DSP IF8 IN",			"MIXERR"},
	{"DACR MUX",		"DSP IF8 OUT",			"MIXERR"},

	/* DAC SRC MIXER */
	{"DACL MIXER",		"DSP IF8 SWITCH",		"DACL MUX"},
	{"DACL MIXER",		"SIDETONE SWITCH",		"SIDE PGA"},

	{"DACR MIXER",		"DSP IF8 SWITCH",		"DACR MUX"},
	{"DACR MIXER",		"SIDETONE SWITCH",		"SIDE PGA"},
	/* DACS MUX */
	{"DACSL MUX",		"DSP IF8 OUT",		"MIXERL"},
	{"DACSL MUX",		"S1 L",			"S1 IL PGA"},
	{"DACSL MUX",		"SHARE MIXER",		"SHARE MIXER"},

	{"DACSR MUX",		"DSP IF8 OUT",		"MIXERR"},
	{"DACSR MUX",		"S1 R",			"S1 IR PGA"},
	{"DACSR MUX",		"SHARE MIXER",		"SHARE MIXER"},

	/* DACL PGA */
	{"DACL PGA",		NULL,			"DACL MIXER"},
	{"DACR PGA",		NULL,			"DACR MIXER"},
	{"DACSL SWITCH",	"SWITCH",		"DACSL MUX"},
	{"DACSR SWITCH",	"SWITCH",		"DACSR MUX"},

	{"DACL PGA",		NULL,		"AUDIO CLK"},
	{"DACR PGA",		NULL,		"AUDIO CLK"},
	{"DACSL SWITCH",	NULL,		"AUDIO CLK"},
	{"DACSR SWITCH",	NULL,		"AUDIO CLK"},
	{"DACL PGA",		NULL,		"PLL CLK"},
	{"DACR PGA",		NULL,		"PLL CLK"},
	{"DACSL SWITCH",	NULL,		"PLL CLK"},
	{"DACSR SWITCH",	NULL,		"PLL CLK"},

	/* HPL MUX */
	{"HPL MUX",		"DACL",		"DACL PGA"},
	{"HPL MUX",		"DACSL",	"DACSL SWITCH"},
	{"HPL MUX",		"DACR",		"DACR PGA"},
	{"HPL MUX",		"DACSR",	"DACSR SWITCH"},
	
	/* HPR MUX */
	{"HPR MUX",		"DACL",		"DACL PGA"},
	{"HPR MUX",		"DACSL",	"DACSL SWITCH"},
	{"HPR MUX",		"DACR",		"DACR PGA"},
	{"HPR MUX",		"DACSR",	"DACSR SWITCH"},
	
	/* EP MUX */
	{"EP MUX",		"DACL",		"DACL PGA"},
	{"EP MUX",		"DACSL",	"DACSL SWITCH"},
	{"EP MUX",		"DACR",		"DACR PGA"},
	{"EP MUX",		"DACSR",	"DACSR SWITCH"},
	
	/* LOL MUX */
	{"LOL MUX",		"DACL",		"DACL PGA"},
	{"LOL MUX",		"DACSL",	"DACSL SWITCH"},
	{"LOL MUX",		"DACR",		"DACR PGA"},
	{"LOL MUX",		"DACSR",	"DACSR SWITCH"},
	
	/* LOR MUX */
	{"LOR MUX",		"DACL",		"DACL PGA"},
	{"LOR MUX",		"DACSL",	"DACSL SWITCH"},
	{"LOR MUX",		"DACR",		"DACR PGA"},
	{"LOR MUX",		"DACSR",	"DACSR SWITCH"},

	/* OUT DRIVER */
	{"HPL DRV",		NULL,		"HPL MUX"},
	{"HPR DRV",		NULL,		"HPR MUX"},
	
	{"EP DRV",		NULL,		"EP SWITCH"},
	{"EP SWITCH",		"SWITCH",	"EP MUX"},
	
	{"LOL DRV",		NULL,		"LOL MUX"},
	{"LOR DRV",		NULL,		"LOR MUX"},

	/* OUTPUT */
	{"EAR OUT",		NULL,		"EP DRV"},
	{"HPL OUT",		NULL,		"HPL DRV"},
	{"HPR OUT",		NULL,		"HPR DRV"},
	{"LINEOUT L",		NULL,		"LOL DRV"},
	{"LINEOUT R",		NULL,		"LOR DRV"},

	{"HPL DRV",		NULL,		"CP2"},
	{"HPR DRV",		NULL,		"CP2"},
	{"EP DRV",		NULL,		"CP1"},

	{"CP2",			NULL,		"CP1"},
	{"CP1",			NULL,		"PLL CLK"},

	/* SIDETONE MUX */
	{"SIDETONE MUX",		"S1 IL",		"S1 IL PGA"},
	{"SIDETONE MUX",		"S2 IL",		"S2 IL PGA"},
	{"SIDETONE MUX",		"S3 IL",		"S3 IL PGA"},
	{"SIDETONE MUX",		"S4 IL",		"S4 IL PGA"},
	{"SIDETONE MUX",		"ADC0L",		"ADC0L"},
	{"SIDETONE MUX",		"ADC0R",		"ADC0R"},
	{"SIDETONE MUX",		"ADC1L",		"ADC1L"},
	{"SIDETONE MUX",		"ADC1R",		"ADC1R"},
	{"SIDETONE MUX",		"SHARE MIXER",		"SHARE MIXER"},

	/* MDM MUX */
	{"MDM MUX",		"S1 IL",		"S1 IL PGA"},
	{"MDM MUX",		"S2 IL",		"S2 IL PGA"},
	{"MDM MUX",		"S3 IL",		"S3 IL PGA"},
	{"MDM MUX",		"S4 IL",		"S4 IL PGA"},
	{"MDM MUX",		"ADC0L",		"ADC0L"},
	{"MDM MUX",		"ADC0R",		"ADC0R"},
	{"MDM MUX",		"ADC1L",		"ADC1L"},
	{"MDM MUX",		"ADC1R",		"ADC1R"},
	{"MDM MUX",		"SHARE MIXER",		"SHARE MIXER"},

	/* SIDE PGA */
	{"SIDE PGA",		NULL,		"SIDETONE MUX"},

	/* APL MUX */
	{"APL MUX",		"ADC0L",		"ADC0L"},
	{"APL MUX",		"ADC1L",		"ADC1L"},
	{"APL MUX",		"DACL",			"DACL PGA"},
	{"APL MUX",		"DACL_48",		"DACL MIXER"},
	{"APL MUX",		"S1L",			"S1 IL PGA"},
	{"APL MUX",		"S2L",			"S2 IL PGA"},
	{"APL MUX",		"S3L",			"S3 IL PGA"},
	{"APL MUX",		"S4L",			"S4 IL PGA"},
	{"APL MUX",		"MDM",			"MDM MUX"},
	{"APL MUX",		"BT0",			"S2 OL SWITCH"},
	{"APL MUX",		"MDM0",			"S3 OL SWITCH"},
	{"APL MUX",		"MDM2",			"S4 OL SWITCH"},

	/* APR MUX */
	{"APR MUX",		"ADC0R",		"ADC0R"},
	{"APR MUX",		"ADC1R",		"ADC1R"},
	{"APR MUX",		"DACR",			"DACR PGA"},
	{"APR MUX",		"DACR_48",		"DACR MIXER"},
	{"APR MUX",		"S1R",			"S1 IR PGA"},
	{"APR MUX",		"S2R",			"S2 IR PGA"},
	{"APR MUX",		"S3R",			"S3 IR PGA"},
	{"APR MUX",		"S4R",			"S4 IR PGA"},
	{"APR MUX",		"MDM",			"MDM MUX"},
	{"APR MUX",		"BT1",			"S2 OR SWITCH"},
	{"APR MUX",		"MDM1",			"S3 OR SWITCH"},
	{"APR MUX",		"MDM3",			"S4 OR SWITCH"},

	/* BTL MUX */
	{"BTL MUX",		"ADC0L",		"ADC0L"},
	{"BTL MUX",		"ADC1L",		"ADC1L"},
	{"BTL MUX",		"DACL",			"DACL PGA"},
	{"BTL MUX",		"DACL_48",		"DACL MIXER"},
	{"BTL MUX",		"S1L",			"S1 IL PGA"},
	{"BTL MUX",		"S2L",			"S2 IL PGA"},
	{"BTL MUX",		"S3L",			"S3 IL PGA"},
	{"BTL MUX",		"S4L",			"S4 IL PGA"},
	{"BTL MUX",		"MDM",			"MDM MUX"},
	{"BTL MUX",		"AP0",			"S1 OL SWITCH"},
	{"BTL MUX",		"MDM0",			"S3 OL SWITCH"},
	{"BTL MUX",		"MDM2",			"S4 OL SWITCH"},	
	
	/* BTR MUX */
	{"BTR MUX",		"ADC0R",		"ADC0R"},
	{"BTR MUX",		"ADC1R",		"ADC1R"},
	{"BTR MUX",		"DACR",			"DACR PGA"},
	{"BTR MUX",		"DACR_48",		"DACR MIXER"},
	{"BTR MUX",		"S1R",			"S1 IR PGA"},
	{"BTR MUX",		"S2R",			"S2 IR PGA"},
	{"BTR MUX",		"S3R",			"S3 IR PGA"},
	{"BTR MUX",		"S4R",			"S4 IR PGA"},
	{"BTR MUX",		"MDM",			"MDM MUX"},
	{"BTR MUX",		"AP1",			"S1 OR SWITCH"},
	{"BTR MUX",		"MDM1",			"S3 OR SWITCH"},
	{"BTR MUX",		"MDM3",			"S4 OR SWITCH"},

	/* M0L MUX */
	{"M0L MUX",		"ADC0L",		"ADC0L"},
	{"M0L MUX",		"ADC1L",		"ADC1L"},
	{"M0L MUX",		"DACL",			"DACL PGA"},
	{"M0L MUX",		"DACL_48",		"DACL MIXER"},
	{"M0L MUX",		"S1L",			"S1 IL PGA"},
	{"M0L MUX",		"S2L",			"S2 IL PGA"},
	{"M0L MUX",		"S3L",			"S3 IL PGA"},
	{"M0L MUX",		"S4L",			"S4 IL PGA"},
	{"M0L MUX",		"MDM",			"MDM MUX"},
	{"M0L MUX",		"AP0",			"S1 OL SWITCH"},
	{"M0L MUX",		"BT0",			"S2 OL SWITCH"},
	{"M0L MUX",		"MDM2",			"S4 OL SWITCH"},	
	
	/* M0R MUX */
	{"M0R MUX",		"ADC0R",		"ADC0R"},
	{"M0R MUX",		"ADC1R",		"ADC1R"},
	{"M0R MUX",		"DACR",			"DACR PGA"},
	{"M0R MUX",		"DACR_48",		"DACR MIXER"},
	{"M0R MUX",		"S1R",			"S1 IR PGA"},
	{"M0R MUX",		"S2R",			"S2 IR PGA"},
	{"M0R MUX",		"S3R",			"S3 IR PGA"},
	{"M0R MUX",		"S4R",			"S4 IR PGA"},
	{"M0R MUX",		"MDM",			"MDM MUX"},
	{"M0R MUX",		"AP1",			"S1 OR SWITCH"},
	{"M0R MUX",		"BT1",			"S2 OR SWITCH"},
	{"M0R MUX",		"MDM3",			"S4 OR SWITCH"},

	/* M1L MUX */
	{"M1L MUX",		"ADC0L",		"ADC0L"},
	{"M1L MUX",		"ADC1L",		"ADC1L"},
	{"M1L MUX",		"DACL",			"DACL PGA"},
	{"M1L MUX",		"DACL_48",		"DACL MIXER"},
	{"M1L MUX",		"S1L",			"S1 IL PGA"},
	{"M1L MUX",		"S2L",			"S2 IL PGA"},
	{"M1L MUX",		"S3L",			"S3 IL PGA"},
	{"M1L MUX",		"S4L",			"S4 IL PGA"},
	{"M1L MUX",		"MDM",			"MDM MUX"},
	{"M1L MUX",		"AP0",			"S1 OL SWITCH"},
	{"M1L MUX",		"MDM0",			"S2 OL SWITCH"},
	{"M1L MUX",		"MDM0",			"S4 OL SWITCH"},	
	
	/* M1R MUX */
	{"M1R MUX",		"ADC0R",		"ADC0R"},
	{"M1R MUX",		"ADC1R",		"ADC1R"},
	{"M1R MUX",		"DACR",			"DACR PGA"},
	{"M1R MUX",		"DACR_48",		"DACR MIXER"},
	{"M1R MUX",		"S1R",			"S1 IR PGA"},
	{"M1R MUX",		"S2R",			"S2 IR PGA"},
	{"M1R MUX",		"S3R",			"S3 IR PGA"},
	{"M1R MUX",		"S4R",			"S4 IR PGA"},
	{"M1R MUX",		"MDM",			"MDM MUX"},
	{"M1R MUX",		"AP1",			"S1 OR SWITCH"},
	{"M1R MUX",		"MDM1",			"S2 OR SWITCH"},
	{"M1R MUX",		"MDM1",			"S4 OR SWITCH"},

	/* ADC */
	{"ADC0L",		NULL,		"ADC0L MUX"},
	{"ADC0R",		NULL,		"ADC0R MUX"},
	{"ADC1L",		NULL,		"ADC1L MUX"},
	{"ADC1R",		NULL,		"ADC1R MUX"},


	/* ADC MUX */
	{"ADC0L MUX",		"MAINMIC",		"MAINMIC PGA"},
	{"ADC0L MUX",		"AUXMIC",		"AUXMIC PGA"},
	{"ADC0L MUX",		"LINEINL",		"LINEINL PGA"},
	{"ADC0L MUX",		"LINEINR",		"LINEINR PGA"},
	{"ADC0L MUX",		"DMIC0L",		"DIGMIC0"},
	{"ADC0L MUX",		"DMIC0R",		"DIGMIC0"},
	{"ADC0L MUX",		"DMIC1L",		"DIGMIC1"},
	{"ADC0L MUX",		"DMIC1R",		"DIGMIC1"},

	{"ADC0R MUX",		"MAINMIC",		"MAINMIC PGA"},
	{"ADC0R MUX",		"AUXMIC",		"AUXMIC PGA"},
	{"ADC0R MUX",		"LINEINL",		"LINEINL PGA"},
	{"ADC0R MUX",		"LINEINR",		"LINEINR PGA"},
	{"ADC0R MUX",		"DMIC0L",		"DIGMIC0"},
	{"ADC0R MUX",		"DMIC0R",		"DIGMIC0"},
	{"ADC0R MUX",		"DMIC1L",		"DIGMIC1"},
	{"ADC0R MUX",		"DMIC1R",		"DIGMIC1"},

	{"ADC1L MUX",		"MAINMIC",		"MAINMIC PGA"},
	{"ADC1L MUX",		"AUXMIC",		"AUXMIC PGA"},
	{"ADC1L MUX",		"LINEINL",		"LINEINL PGA"},
	{"ADC1L MUX",		"LINEINR",		"LINEINR PGA"},
	{"ADC1L MUX",		"DMIC0L",		"DIGMIC0"},
	{"ADC1L MUX",		"DMIC0R",		"DIGMIC0"},
	{"ADC1L MUX",		"DMIC1L",		"DIGMIC1"},
	{"ADC1L MUX",		"DMIC1R",		"DIGMIC1"},

	{"ADC1R MUX",		"MAINMIC",		"MAINMIC PGA"},
	{"ADC1R MUX",		"AUXMIC",		"AUXMIC PGA"},
	{"ADC1R MUX",		"LINEINL",		"LINEINL PGA"},
	{"ADC1R MUX",		"LINEINR",		"LINEINR PGA"},
	{"ADC1R MUX",		"DMIC0L",		"DIGMIC0"},
	{"ADC1R MUX",		"DMIC0R",		"DIGMIC0"},
	{"ADC1R MUX",		"DMIC1L",		"DIGMIC1"},
	{"ADC1R MUX",		"DMIC1R",		"DIGMIC1"},

	{"MAINMIC PGA",			NULL,		"MIC MUX"},
	{"LINEINL PGA",			NULL,		"LIL MUX"},
	{"LINEINR PGA",			NULL,		"LIR MUX"},
	{"AUXMIC PGA",			NULL,		"AUXMIC"},
	
	{"LIL MUX",		"LINEIN1",		"LINEIN1"},
	{"LIL MUX",		"LINEIN2",		"LINEIN2"},
	{"LIL MUX",		"LINEOUT",		"LINE OUTL"},
	{"LIL MUX",		"MIC2",			"MIC2"},
	
	{"LIR MUX",		"LINEIN1",		"LINEIN1"},
	{"LIR MUX",		"LINEIN2",		"LINEIN2"},
	{"LIR MUX",		"LINEOUT",		"LINE OUTR"},
	{"LIR MUX",		"MIC3",			"MIC3"},

	{"MIC MUX",			"MAIN MIC",		"MAINMIC"},
	{"MIC MUX",			"HS MIC",		"HSMIC"},
	
	/* ANA IN */
	{"MAINMIC",			NULL,		"MAIN MIC"},
	{"HSMIC",			NULL,		"HS MIC"},
	{"AUXMIC",			NULL,		"AUX MIC"},
	{"MIC2",			NULL,		"MIC2 IN"},
	{"MIC3",			NULL,		"MIC3 IN"},

	/* SMART PA ROUTE */
	{"SP IL SWITCH",		"SWITCH",		"S4L IN"},
	{"SP IR SWITCH",		"SWITCH",		"S4R IN"},

	{"SP L OUT",			NULL,			"SP IL SWITCH"},
	{"SP R OUT",			NULL,			"SP IL SWITCH"},
};

static int hi6402es_audio_hw_params(struct snd_pcm_substream *substream,
				struct snd_pcm_hw_params *params,
				struct snd_soc_dai *dai)
{
	struct snd_soc_codec *codec = dai->codec;
	int rate = 0;
	int ret = 0;

	BUG_ON(NULL == codec);

	rate = params_rate(params);
	switch (rate) {
	case 8000:
	case 11250:
	case 16000:
	case 22500:
	case 32000:
	case 44100:
	case 48000:
		break;
	case 88200:
	case 96000:
		break;
	default:
		pr_err("unknown rate : %d", rate);
		ret = -1;
		break;
	}

	return ret;
}

struct snd_soc_dai_ops hi6402es_audio_dai_ops = {
	.hw_params = hi6402es_audio_hw_params,
};

static int hi6402es_voice_hw_params(struct snd_pcm_substream *substream,
				struct snd_pcm_hw_params *params,
				struct snd_soc_dai *dai)
{
	struct snd_soc_codec *codec = dai->codec;
	int rate = 0;
	int ret = 0;

	BUG_ON(NULL == codec);

	rate = params_rate(params);
	switch (rate) {
	case 8000:
		hi6402es_dapm_reg_write_bits(codec, HI6402ES_S3_FS_CFG_L, 0x00, 0x07);
		hi6402es_dapm_reg_set_bit(codec, HI6402ES_S3_IN_SRC_SEL, HI6402ES_S3_IL_SRC_BIT);
		hi6402es_dapm_reg_set_bit(codec, HI6402ES_S3_OUT_SRC_SEL, HI6402ES_S3_OL_SRC_BIT);
		hi6402es_dapm_reg_write_bits(codec, HI6402ES_S3_SRC_IN_MODE_CGF, 0x0A, 0x0E);
		hi6402es_dapm_reg_write_bits(codec, HI6402ES_S3_SRC_OUT_MODE_CGF, 0x04, 0x06);
		break;
	case 16000:
		hi6402es_dapm_reg_write_bits(codec, HI6402ES_S3_FS_CFG_L, 0x01, 0x07);
		hi6402es_dapm_reg_set_bit(codec, HI6402ES_S3_IN_SRC_SEL, HI6402ES_S3_IL_SRC_BIT);
		hi6402es_dapm_reg_set_bit(codec, HI6402ES_S3_OUT_SRC_SEL, HI6402ES_S3_OL_SRC_BIT);
		hi6402es_dapm_reg_write_bits(codec, HI6402ES_S3_SRC_IN_MODE_CGF, 0x02, 0x0E);
		hi6402es_dapm_reg_write_bits(codec, HI6402ES_S3_SRC_OUT_MODE_CGF, 0x00, 0x06);
		break;
	case 32000:
		hi6402es_dapm_reg_write_bits(codec, HI6402ES_S3_FS_CFG_L, 0x02, 0x07);
		break;
	default:
		pr_err("unknown rate : %d", rate);
		ret = -1;
		break;
	}

	return ret;
}

struct snd_soc_dai_ops hi6402es_voice_dai_ops = {
	.hw_params = hi6402es_voice_hw_params,
};

struct snd_soc_dai_driver hi6402es_dai[] = {
	{
		.name = "hi6402es-audio-dai",
		.playback = {
			.stream_name	= "Playback",
			.channels_min	= 2,
			.channels_max	= 2,
			.rates		= HI6402ES_RATES,
			.formats	= HI6402ES_FORMATS},
		.capture = {
			.stream_name	= "Capture",
			.channels_min	= 2,
			.channels_max	= 2,
			.rates		= HI6402ES_RATES,
			.formats	= HI6402ES_FORMATS},
		.ops = &hi6402es_audio_dai_ops,
	},
	{
		.name = "hi6402es-voice-dai",
		.playback = {
			.stream_name	= "Voice",
			.channels_min	= 1,
			.channels_max	= 2,
			.rates		= HI6402ES_RATES,
			.formats	= HI6402ES_FORMATS},
		.ops = &hi6402es_voice_dai_ops,
	},
	{
		.name = "hi6402es-fm-dai",
		.playback = {
			.stream_name	= "FM",
			.channels_min	= 1,
			.channels_max	= 2,
			.rates		= HI6402ES_RATES,
			.formats	= HI6402ES_FORMATS},
	},
};

#ifdef CONFIG_HI6402_MBHC

void hi6402es_soc_jack_report(int report, int mask)
{
	snd_soc_jack_report(&hs_jack, report, mask);
}
EXPORT_SYMBOL(hi6402es_soc_jack_report);
#endif

static void hi6402es_init_chip(struct snd_soc_codec *codec)
{
	/* S1-S4 I2S INIT */
	hi6402es_dapm_reg_write_bits(codec, 0x1000, 0x000000F0, 0x000000F0);
	hi6402es_dapm_reg_write(codec, 0x1228, 0x000000C4);
	hi6402es_dapm_reg_write(codec, 0x1238, 0x000000C4);
	hi6402es_dapm_reg_write(codec, 0x1248, 0x000000C4);
	hi6402es_dapm_reg_write(codec, 0x1258, 0x000000C4);

	/* S1 PORT IN */
	hi6402es_dapm_reg_write(codec, HI6402ES_S1_PORT_CFG_L, 0x00);
	hi6402es_dapm_reg_write(codec, HI6402ES_S1_PORT_CFG_H, 0x40);
	
	/* S1 FS INIT */
	hi6402es_dapm_reg_write_bits(codec, HI6402ES_S1_FS_CFG_L, 0x04, 0x07);
	
	/* S1 IN MUX */
	hi6402es_dapm_reg_write(codec, HI6402ES_S_PORT_IN_SEL, 0x00);

	/* S1-S4 MUX FOR SRC */
	hi6402es_dapm_reg_write(codec, HI6402ES_S_PORT_SRC_IN_SEL0, 0x00);
	hi6402es_dapm_reg_write(codec, HI6402ES_S_PORT_SRC_IN_SEL1, 0x00);
	
	/* S1 SRC */
	hi6402es_dapm_reg_write_bits(codec, HI6402ES_S1_SRC_IN_MODE_CGF, 0x00, 0xEE);
	hi6402es_dapm_reg_write_bits(codec, HI6402ES_S1_MODULE_CLK_REG, 0x00, 0x0C);
	
	/* MUX FOR EQ */
	hi6402es_dapm_reg_write_bits(codec, HI6402ES_EQ_MUX_SEL, 0x00, 0x03);
	hi6402es_dapm_reg_write_bits(codec, HI6402ES_EQ_MUX_SEL, 0x00, 0x0C);
	hi6402es_dapm_reg_clr_bit(codec, HI6402ES_SHARE_MUX_SEL, 3);
	
	/* S2 PORT IN */
	hi6402es_dapm_reg_write(codec, HI6402ES_S2_PORT_CFG_L, 0x00);
	hi6402es_dapm_reg_write(codec, HI6402ES_S2_PORT_CFG_H, 0x40);
	
	/* S2 FS INIT */
	hi6402es_dapm_reg_write_bits(codec, HI6402ES_S2_FS_CFG_L, 0x04, 0x07);
	
	/* S2 SRC */
	hi6402es_dapm_reg_write_bits(codec, HI6402ES_S2_SRC_IN_MODE_CGF, 0x00, 0xEE);
	hi6402es_dapm_reg_write_bits(codec, HI6402ES_S2_MODULE_CLK_REG, 0x00, 0x0C);
	
	/* S3 PORT IN */
	hi6402es_dapm_reg_write(codec, HI6402ES_S3_PORT_CFG_L, 0x00);
	hi6402es_dapm_reg_write(codec, HI6402ES_S3_PORT_CFG_H, 0x40);
	
	/* S3 FS INIT */
	hi6402es_dapm_reg_write_bits(codec, HI6402ES_S3_FS_CFG_L, 0x04, 0x07);
	
	/* S3 SRC */
	hi6402es_dapm_reg_write_bits(codec, HI6402ES_S3_SRC_IN_MODE_CGF, 0x00, 0xEE);
	hi6402es_dapm_reg_write_bits(codec, HI6402ES_S3_MODULE_CLK_REG, 0x00, 0x0C);
	
	/* S4 PORT IN */
	hi6402es_dapm_reg_write(codec, HI6402ES_S4_PORT_CFG_L, 0x00);
	hi6402es_dapm_reg_write(codec, HI6402ES_S4_PORT_CFG_H, 0x40);
	
	/* S4 FS INIT */
	hi6402es_dapm_reg_write_bits(codec, HI6402ES_S4_FS_CFG_L, 0x04, 0x07);
	
	/* S4 SRC */
	hi6402es_dapm_reg_write_bits(codec, HI6402ES_S4_SRC_IN_MODE_CGF, 0x00, 0xEE);
	hi6402es_dapm_reg_write_bits(codec, HI6402ES_S4_MODULE_CLK_REG, 0x00, 0x0C);
	
	/* DSP IF BYPASS INIT */
	hi6402es_dapm_reg_write_bits(codec, HI6402ES_S1_DSP_IF_SEL, 0xC0, 0xC0);
	hi6402es_dapm_reg_write_bits(codec, HI6402ES_S2_DSP_IF_SEL, 0xC0, 0xC0);
	hi6402es_dapm_reg_write_bits(codec, HI6402ES_S3_DSP_IF_SEL, 0xC0, 0xC0);
	hi6402es_dapm_reg_write_bits(codec, HI6402ES_S4_DSP_IF_SEL, 0xC0, 0xC0);
	hi6402es_dapm_reg_write_bits(codec, HI6402ES_DSP_IF8_SEL, 0x10, 0x10);

	/* DAC SRC MODE x2 */
	hi6402es_dapm_reg_write(codec, HI6402ES_DAC_SRC_MODE_REG, 0x88);

	/* DAC DEEMP BYPASS */
	hi6402es_dapm_reg_set_bit(codec, HI6402ES_DAC_FILTER_OUTR_MUX_SEL, HI6402ES_DAC_DEEMP_BYPASS_BIT);
	hi6402es_dapm_reg_set_bit(codec, HI6402ES_DAC_FILTER_OUTL_MUX_SEL, HI6402ES_DAC_DEEMP_BYPASS_BIT);

	/* pga fade in en */
	hi6402es_dapm_reg_set_bit(codec, HI6402ES_S1_PGA_IL_MODE_CFG_REG1, 7);
	hi6402es_dapm_reg_set_bit(codec, HI6402ES_S1_PGA_IR_MODE_CFG_REG1, 7);
	hi6402es_dapm_reg_set_bit(codec, HI6402ES_S1_PGA_OL_MODE_CFG_REG1, 7);
	hi6402es_dapm_reg_set_bit(codec, HI6402ES_S1_PGA_OR_MODE_CFG_REG1, 7);

	hi6402es_dapm_reg_set_bit(codec, HI6402ES_S2_PGA_IL_MODE_CFG_REG1, 7);
	hi6402es_dapm_reg_set_bit(codec, HI6402ES_S2_PGA_IR_MODE_CFG_REG1, 7);
	hi6402es_dapm_reg_set_bit(codec, HI6402ES_S2_PGA_OL_MODE_CFG_REG1, 7);
	hi6402es_dapm_reg_set_bit(codec, HI6402ES_S2_PGA_OR_MODE_CFG_REG1, 7);

	hi6402es_dapm_reg_set_bit(codec, HI6402ES_S3_PGA_IL_MODE_CFG_REG1, 7);
	hi6402es_dapm_reg_set_bit(codec, HI6402ES_S3_PGA_IR_MODE_CFG_REG1, 7);
	hi6402es_dapm_reg_set_bit(codec, HI6402ES_S3_PGA_OL_MODE_CFG_REG1, 7);
	hi6402es_dapm_reg_set_bit(codec, HI6402ES_S3_PGA_OR_MODE_CFG_REG1, 7);

	hi6402es_dapm_reg_set_bit(codec, HI6402ES_S4_PGA_IL_MODE_CFG_REG1, 7);
	hi6402es_dapm_reg_set_bit(codec, HI6402ES_S4_PGA_IR_MODE_CFG_REG1, 7);
	hi6402es_dapm_reg_set_bit(codec, HI6402ES_S4_PGA_OL_MODE_CFG_REG1, 7);
	hi6402es_dapm_reg_set_bit(codec, HI6402ES_S4_PGA_OR_MODE_CFG_REG1, 7);

	hi6402es_dapm_reg_set_bit(codec, HI6402ES_DACL_PGA_MODE_CFG_REG1, 7);
	hi6402es_dapm_reg_set_bit(codec, HI6402ES_DACR_PGA_MODE_CFG_REG1, 7);

	hi6402es_dapm_reg_set_bit(codec, HI6402ES_SIDE_PGA_MODE_CFG_REG1, 7);

	/* DSP IF8 & SIDETONE SRC & MDM SRC FS CFG */
	hi6402es_dapm_reg_write(codec, HI6402ES_FS_DSM_CFG_REG, 0x42);

	/* S1 DSP IF FS CFG */
	hi6402es_dapm_reg_write(codec, HI6402ES_S1_DSP_IF_FS_CFG_REG, 0x44);

	/* MIXER MUTE INT */
	hi6402es_dapm_reg_write_bits(codec, HI6402ES_DACL_MIXER_SEL_0, 0xE0, 0xE0);
	hi6402es_dapm_reg_write_bits(codec, HI6402ES_DACL_MIXER_SEL_1, 0x20, 0x20);

	hi6402es_dapm_reg_write_bits(codec, HI6402ES_DACR_MIXER_SEL_0, 0xE0, 0xE0);
	hi6402es_dapm_reg_write_bits(codec, HI6402ES_DACR_MIXER_SEL_1, 0x20, 0x20);

	hi6402es_dapm_reg_write_bits(codec, HI6402ES_SHARE_MIXER_SEL_0, 0x60, 0x60);

	hi6402es_dapm_reg_write_bits(codec, HI6402ES_DACSL_MIXER_SEL, 0x60, 0x60);
	hi6402es_dapm_reg_write_bits(codec, HI6402ES_DACSR_MIXER_SEL, 0x60, 0x60);

	/* MIXER GAIN */
	hi6402es_dapm_reg_write_bits(codec, HI6402ES_DACSL_MIXER_GAIN_REG, 0x0F, 0x0F);
	hi6402es_dapm_reg_write_bits(codec, HI6402ES_DACSR_MIXER_GAIN_REG, 0x0F, 0x0F);

	hi6402es_dapm_reg_write_bits(codec, HI6402ES_DACL_MIXER_GAIN_REG, 0xFF, 0xFF);
	hi6402es_dapm_reg_write_bits(codec, HI6402ES_DACR_MIXER_GAIN_REG, 0xFF, 0xFF);
	
	hi6402es_dapm_reg_write_bits(codec, HI6402ES_SHARE_MIXER_GAIN_REG, 0x0F, 0x0F);

	/* mixer fade in out */
	hi6402es_dapm_reg_write_bits(codec, HI6402ES_DACSL_MIXER_SEL, 0x09, 0x1F);
	hi6402es_dapm_reg_write_bits(codec, HI6402ES_DACSL_MIXER_FD_REG, 0x49, 0x5F);

	hi6402es_dapm_reg_write_bits(codec, HI6402ES_DACSR_MIXER_SEL, 0x09, 0x1F);
	hi6402es_dapm_reg_write_bits(codec, HI6402ES_DACSR_MIXER_FD_REG, 0x49, 0x5F);

	hi6402es_dapm_reg_write_bits(codec, HI6402ES_SHARE_MIXER_FD_REG_0, 0x09, 0x1F);
	hi6402es_dapm_reg_write_bits(codec, HI6402ES_SHARE_MIXER_FD_REG_1, 0x49, 0x5F);

	hi6402es_dapm_reg_write_bits(codec, HI6402ES_DACL_MIXER_FD_REG_0, 0x09, 0x1F);
	hi6402es_dapm_reg_write_bits(codec, HI6402ES_DACL_MIXER_FD_REG_1, 0x49, 0x5F);

	hi6402es_dapm_reg_write_bits(codec, HI6402ES_DACR_MIXER_FD_REG_0, 0x09, 0x1F);
	hi6402es_dapm_reg_write_bits(codec, HI6402ES_DACR_MIXER_FD_REG_1, 0x49, 0x5F);

	/* MUX FOR IN2 OF MIXER4  */
	hi6402es_dapm_reg_write_bits(codec, HI6402ES_IN_OUT_MIXER4_MUX_SEL, 0x01, 0x03);
	hi6402es_dapm_reg_write_bits(codec, HI6402ES_IN_OUT_MIXER4_MUX_SEL, 0x04, 0x0c);
	
	/* MUX OF MIXER4 OUT */
	hi6402es_dapm_reg_write_bits(codec, HI6402ES_IN_OUT_MIXER4_MUX_SEL, 0x10, 0x30);
	
	/* MUX OF FILTER OUT */
	hi6402es_dapm_reg_clr_bit(codec, HI6402ES_DAC_FILTER_OUTL_MUX_SEL, 7);
	hi6402es_dapm_reg_clr_bit(codec, HI6402ES_DAC_FILTER_OUTR_MUX_SEL, 7);
	
	/* MUX FOR SDM */
	hi6402es_dapm_reg_write_bits(codec, HI6402ES_DAC_SDM_MUX_SEL, 0x00, 0x03);
	hi6402es_dapm_reg_write_bits(codec, HI6402ES_DACS_SDM_MUX_SEL, 0x00, 0xF0);

	/* MUX FOR SRCDN */
	hi6402es_dapm_reg_write(codec, HI6402ES_SRCDN_MUX_SEL, 0x00);
	
	/* MUX OF SRC OUT */
	hi6402es_dapm_reg_write_bits(codec, HI6402ES_SHARE_MUX_SEL, 0x00, 0xF0);

	/* MUX OF ANADAC OUT */
	hi6402es_dapm_reg_write(codec, HI6402ES_DAC_HP_MUX_SEL, 0x24);
	hi6402es_dapm_reg_write(codec, HI6402ES_DAC_LT_MUX_SEL, 0x49);

	/* MUX FOR DAC SRC MIXER2 */
	hi6402es_dapm_reg_set_bit(codec, HI6402ES_DAC_SRC_MIXER_MUX_SEL, HI6402ES_DACL_SRC_MIXER_MUX_BIT);
	hi6402es_dapm_reg_set_bit(codec, HI6402ES_DAC_SRC_MIXER_MUX_SEL, HI6402ES_DACR_SRC_MIXER_MUX_BIT);

	/* DIG PGA */
	hi6402es_dapm_reg_write(codec, HI6402ES_S1_PGA_IL_GAIN_CFG_REG, 0x0);
	hi6402es_dapm_reg_write(codec, HI6402ES_S1_PGA_IR_GAIN_CFG_REG, 0x0);
	hi6402es_dapm_reg_write(codec, HI6402ES_S1_PGA_OL_GAIN_CFG_REG, 0x0);
	hi6402es_dapm_reg_write(codec, HI6402ES_S1_PGA_OR_GAIN_CFG_REG, 0x0);

	hi6402es_dapm_reg_write(codec, HI6402ES_S2_PGA_IL_GAIN_CFG_REG, 0x0);
	hi6402es_dapm_reg_write(codec, HI6402ES_S2_PGA_IR_GAIN_CFG_REG, 0x0);
	hi6402es_dapm_reg_write(codec, HI6402ES_S2_PGA_OL_GAIN_CFG_REG, 0x0);
	hi6402es_dapm_reg_write(codec, HI6402ES_S2_PGA_OR_GAIN_CFG_REG, 0x0);

	hi6402es_dapm_reg_write(codec, HI6402ES_S3_PGA_IL_GAIN_CFG_REG, 0x0);
	hi6402es_dapm_reg_write(codec, HI6402ES_S3_PGA_IR_GAIN_CFG_REG, 0x0);
	hi6402es_dapm_reg_write(codec, HI6402ES_S3_PGA_OL_GAIN_CFG_REG, 0x0);
	hi6402es_dapm_reg_write(codec, HI6402ES_S3_PGA_OR_GAIN_CFG_REG, 0x0);

	hi6402es_dapm_reg_write(codec, HI6402ES_S4_PGA_IL_GAIN_CFG_REG, 0x0);
	hi6402es_dapm_reg_write(codec, HI6402ES_S4_PGA_IR_GAIN_CFG_REG, 0x0);
	hi6402es_dapm_reg_write(codec, HI6402ES_S4_PGA_OL_GAIN_CFG_REG, 0x0);
	hi6402es_dapm_reg_write(codec, HI6402ES_S4_PGA_OR_GAIN_CFG_REG, 0x0);

	/* MAINMIC PGA GAIN */
	hi6402es_dapm_reg_write_bits(codec, HI6402ES_MAIN_PGA_SEL_REG, 0x40, 0x1F<<HI6402ES_MAIN_PGA_GAIN_BIT);
	/* AUXMIC PGA GAIN */
	hi6402es_dapm_reg_write_bits(codec, HI6402ES_AUX_PGA_SEL_REG, 0x80, 0x1F<<HI6402ES_AUX_PGA_GAIN_BIT);

	/* MAIN AUX MICBIAS VALUE */
	hi6402es_dapm_reg_write_bits(codec, HI6402ES_MAIN_AUX_MICBIAS_VALUE_REG, 0x36, 0x3F);

	/* HS MICBIAS VALUE */
	hi6402es_dapm_reg_write_bits(codec, HI6402ES_HS_MICBIAS_VALUE_REG, 0x06, 0x07);
	/* MAIN AUX MICBIAS VALUE */
	hi6402es_dapm_reg_write_bits(codec, HI6402ES_MAIN_AUX_MICBIAS_VALUE_REG, 0x36, 0x3F);
	/* MIC2 MIC3 MICBIAS VALUE */
	hi6402es_dapm_reg_write_bits(codec, HI6402ES_MIC2_MIC3_MICBIAS_VALUE_REG, 0x36, 0x3F);
	/* HP PGA GAIN to -15.5dB */
	hi6402es_dapm_reg_write_bits(codec, HI6402ES_HPL_PGA_CFG_REG, 0x0F, 0x3F);
	hi6402es_dapm_reg_write_bits(codec, HI6402ES_HPR_PGA_CFG_REG, 0x0F, 0x3F);

	/* CP1 always on */
	hi6402es_dapm_reg_set_bit(codec, HI6402ES_PLL_CLK_CFG_REG, 0);
	hi6402es_dapm_reg_write_bits(codec, HI6402ES_POP_CLK_PD_REG, 0x00, 0x03);
	hi6402es_dapm_reg_clr_bit(codec, HI6402ES_CP_PD_REG, HI6402ES_CP_PD_BIT);

	/* i2v cfg */
	hi6402es_dapm_reg_write_bits(codec, HI6402ES_DAC_HPL_I2V_CFG_REG, 0x0F, 0x3F);
	hi6402es_dapm_reg_write_bits(codec, HI6402ES_DAC_HPR_I2V_CFG_REG, 0x0F, 0x3F);

	/* pop cfg */
	hi6402es_dapm_reg_write_bits(codec, HI6402ES_HPL_POP_CFG1_REG, 0x01F, 0x3F);
	hi6402es_dapm_reg_write(codec, HI6402ES_HPL_POP_CFG2_REG, 0x82);

	/* ISO release */
	hi6402es_dapm_reg_set_bit(codec, HI6402ES_ISO_REG, HI6402ES_ISO_A18_REG_BIT);
	hi6402es_dapm_reg_set_bit(codec, HI6402ES_ISO_REG, HI6402ES_ISO_A33_REG_BIT);

	/* MAD PATH INIT */
	hi6402es_dapm_reg_write(codec, HI6402ES_SC_DSP_CTRL0, 0x02);
	//hi6402es_dapm_reg_write(codec, HI6402ES_SC_MAD_CTRL0, 0x20);
}

static int hi6402es_soc_probe(struct snd_soc_codec *codec)
{
	int ret = 0;
	struct hi6402es_priv *priv = snd_soc_codec_get_drvdata(codec);

	if (!priv) {
		pr_err("get driver data is NULL\n");
		return -ENOENT;
	}

	snd_soc_codec_set_drvdata(codec, priv);

	priv->codec = codec;

	hi6402es_init_chip(codec);

	mutex_init(&priv->dapm_mutex);
	mutex_init(&priv->cp_mutex);

	priv->ref_cp = 0;
	priv->ref_audio_clk = 0;
	priv->ref_voice_clk = 0;
	priv->ref_audio_tx = 0;
	priv->ref_audio_rx = 0;

#ifdef CONFIG_HI6402_MBHC
	/* register headset jack */
	ret = snd_soc_jack_new(codec, "Headset Jack", SND_JACK_HEADSET, &hs_jack);
	if (ret)
		pr_err("%s : jack new error, errornum = %d\n", __FUNCTION__, ret);

	/* register headset button */
	snd_jack_set_key(hs_jack.jack, SND_JACK_BTN_0, KEY_MEDIA);
	snd_jack_set_key(hs_jack.jack, SND_JACK_BTN_1, KEY_VOLUMEUP);
	snd_jack_set_key(hs_jack.jack, SND_JACK_BTN_2, KEY_VOLUMEDOWN);
	/* for sound triger */
	snd_jack_set_key(hs_jack.jack, SND_JACK_BTN_5, KEY_F14);
#endif

	return ret;
}

static int hi6402es_soc_remove(struct snd_soc_codec *codec)
{
	return 0;
}

static struct snd_soc_codec_driver soc_codec_dev_hi6402es = {
	.probe = hi6402es_soc_probe,
	.remove = hi6402es_soc_remove,
	.read = hi6402es_dapm_reg_read,
	.write = hi6402es_dapm_reg_write,

	.controls = hi6402es_snd_controls,
	.num_controls = ARRAY_SIZE(hi6402es_snd_controls),
	.dapm_widgets = hi6402es_dapm_widgets,
	.num_dapm_widgets = ARRAY_SIZE(hi6402es_dapm_widgets),
	.dapm_routes = route_map,
	.num_dapm_routes = ARRAY_SIZE(route_map),
};

static int hi6402es_codec_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct hi6402es_priv *priv = NULL;

	pr_info("%s\n",__FUNCTION__);

	BUG_ON(NULL == dev);

	priv = devm_kzalloc(dev, sizeof(*priv), GFP_KERNEL);
	if (NULL == priv) {
		pr_err("%s : kzalloc failed\n", __FUNCTION__);
		return -ENOMEM;
	}

	/* get clk & regulator */

	priv->p_irq = dev_get_drvdata(pdev->dev.parent);
	if (!priv->p_irq) {
		dev_err(dev, "get parend device error\n");
		return -ENOENT;
	}

	platform_set_drvdata(pdev, priv);
	dev_set_name(dev, "hi6402es-codec");

	return snd_soc_register_codec(dev, &soc_codec_dev_hi6402es,
			hi6402es_dai, ARRAY_SIZE(hi6402es_dai));
}

static int hi6402es_codec_remove(struct platform_device *pdev)
{
	struct hi6402es_priv *priv = platform_get_drvdata(pdev);

	pr_info("%s\n",__FUNCTION__);

	BUG_ON(NULL == priv);

	/* release clk & regulator */

	snd_soc_unregister_codec(&pdev->dev);

	return 0;
}

static void hi6402es_codec_shutdown(struct platform_device *pdev)
{
	struct hi6402es_priv *priv = platform_get_drvdata(pdev);
	struct snd_soc_codec *codec = NULL;

	BUG_ON(NULL == priv);
	codec = priv->codec;
	BUG_ON(NULL == codec);

	hi6402es_hp_power_off(codec);

#if 0
	if (check_headset_pluged_in(codec)){
		pr_info("hi6402es_codec_shutdown with headset\n");
	} else {
		pr_info("hi6402es_codec_shutdown without headset\n");
	}
#endif
}

static struct platform_driver hi6402es_codec_driver = {
	.probe	= hi6402es_codec_probe,
	.remove	= hi6402es_codec_remove,
	.shutdown = hi6402es_codec_shutdown,
	.driver	= {
		.owner	= THIS_MODULE,
		.name	= "hi6402es-codec",
		.of_match_table = of_match_ptr(hi6402es_codec_match),
	},
};

static int __init hi6402es_codec_init(void)
{
	return platform_driver_register(&hi6402es_codec_driver);
}

static void __exit hi6402es_codec_exit(void)
{
	platform_driver_unregister(&hi6402es_codec_driver);
}

device_initcall(hi6402es_codec_init);
module_exit(hi6402es_codec_exit);

MODULE_DESCRIPTION("ASoC hi6402es codec driver");
MODULE_AUTHOR("chengong <apollo.chengong@huawei.com>");
MODULE_LICENSE("GPL");
