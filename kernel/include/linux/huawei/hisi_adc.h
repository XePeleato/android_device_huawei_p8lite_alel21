/*
 * hisi_adc.h.
 *
 * Copyright (c) 2013 Hisilicon Technologies CO.Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef	HISI_ADC_H
#define	HISI_ADC_H

/*
 * Function name:hisi_adc_get_value.
 * Discription:get value from hkadc.
 * Parameters:
 *      @ adc_channel
 * return value:
 *      @ adc value: negative-->failed, other-->succeed.
 * notice: user must check the result of returning value,
 *         if return negative, should give up it.
 *         suggestion, could call the interface several times.
 */
#ifdef CONFIG_HI3XXX_HKADC
extern int hisi_adc_get_value(int adc_channel);
#else
static inline int hisi_adc_get_value(int adc_channel)
{
	return -1;
}
#endif /* CONFIG_HI3XXX_HKADC */

#endif /* HISI_ADC_H */
