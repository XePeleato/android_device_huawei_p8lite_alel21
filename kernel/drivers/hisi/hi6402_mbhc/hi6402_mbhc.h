/*
 * hi6402_mbhc.h -- hi6402 mbhc driver
 *
 * Copyright (c) 2014 Hisilicon Technologies CO., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef HI6402_MBHC_H
#define HI6402_MBHC_H

#include <linux/mfd/hi6402_irq.h>

enum HI6402_IRQ_TYPE {
	HI6402_IRQ_BTNUP_COMP1 = 0,
	HI6402_IRQ_BTNDOWN_COMP1,
	HI6402_IRQ_BTNUP_COMP2,
	HI6402_IRQ_BTNDOWN_COMP2,
	HI6402_IRQ_BTNUP_ECO,
	HI6402_IRQ_BTNDOWN_ECO,
	HI6402_IRQ_PLUGIN,
	HI6402_IRQ_PLUGOUT,
	HI6402_IRQ_SOUND_TRIGER,
	HI6402_IRQ_PLL_UNLOCK,

	HI6402_IRQ_MAX,
};

/* defination of register offest address */

enum hisi_jack_states {
	HISI_JACK_NONE = 0,	/* unpluged */
	HISI_JACK_HEADSET,	/* pluged 4-pole headset */
	HISI_JACK_HEADPHONE,	/* pluged 3-pole headphone */
	HISI_JACK_INVERT,	/* pluged invert 4-pole headset */
};

/* defination of platform data */
struct hi6402_mbhc_platform_data {
	struct hi6402_irq *p_irq;
	struct miscdevice miscdev;
	struct wake_lock wake_lock;
	struct wake_lock soundtrigger_wake_lock;
	struct mutex plug_mutex;
	struct mutex status_mutex;
	struct mutex saradc_mutex;
	int irq[HI6402_IRQ_MAX];

	/* headset status */
	enum hisi_jack_states hs_status;
	int btn_report;
	bool ldo8_status;

#ifdef CONFIG_SWITCH
	struct switch_dev sdev;
#endif

	/* board defination */
	int hs_det_inv;
	int hs_btn_num;
	int hs_3_pole_min_voltage;
	int hs_3_pole_max_voltage;
	int hs_4_pole_min_voltage;
	int hs_4_pole_max_voltage;
	int btn_play_min_voltage;
	int btn_play_max_voltage;
	int btn_volume_up_min_voltage;
	int btn_volume_up_max_voltage;
	int btn_volume_down_min_voltage;
	int btn_volume_down_max_voltage;
};

#endif //HI6402_MBHC_H
