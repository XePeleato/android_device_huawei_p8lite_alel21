/*
 * Copyright (c) 2011 Hisilicon Technologies Co., Ltd. All rights reserved.
 *
 * This software is available to you under a choice of one of two
 * licenses.  You may choose to be licensed under the terms of the GNU
 * General Public License (GPL) Version 2, available from the file
 * COPYING in the main directory of this source tree, or the
 * OpenIB.org BSD license below:
 *
 *     Redistribution and use in source and binary forms, with or
 *     without modification, are permitted provided that the following
 *     conditions are met:
 *
 *      - Redistributions of source code must retain the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer.
 *
 *      - Redistributions in binary form must reproduce the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer in the documentation and/or other materials
 *        provided with the distribution.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include <linux/err.h>
#include <linux/types.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/kthread.h>
#include <linux/input.h>
#include <linux/time.h>
#include <linux/wakelock.h>
#include <linux/syscalls.h>
#include <linux/mutex.h>
#include <linux/io.h>
#include <linux/huawei/hisi_adc.h>
#include "hisi_peripheral_tm.h"

#define PRORATE_OF_INIT	1000 /*in order to resolve divisor less than zero*/
#define multiple				100
/*line -emacro(750,*)*/
/*lint -e750*/
#define R1003				1500/*ohm*/
#define R1006				27400/*ohm*/
#define  RT1008				150000/*ohm*/
#define AVDD				15/*add to both of  hot resistance's voltage *  10 */
#define BITNUM				4096
#define CURRENT				1/*I * 10 */
/*lint +e750*/
/*line +emacro(750,*)*/
#define NCP_GENERAL1_NUM	34
#define NCP_GENERAL2_NUM	34

enum ntc_tm_table_list {
	ncp_general1_flag = 0,
	ncp_general2_flag,
};

#define ADC_RTMP_DEFAULT_VALUE	1200 /*base default value for ap temp*/

/*hi3630 IC 1 temperature from -40 to 125*/
static int hi3630_volt_to_temp_general1[NCP_GENERAL1_NUM][2] = {
	{2561, 125}, {2952, 120}, {3415, 115}, {3966, 110}, {4623, 105},
	{5410, 100}, {6357, 95}, {7500, 90}, {8887, 85}, {10580, 80},
	{12640, 75}, {15180, 70}, {18320, 65}, {22220, 60}, {27090, 55},
	{33190, 50}, {40900, 45}, {50680, 40}, {63180, 35}, {79230, 30},
	{100000, 25}, {127000, 20}, {162500, 15}, {209400, 10}, {271800, 5},
	{355600, 0}, {469100, -5}, {624100, -10}, {837800, -15}, {1135000, -20},
	{1554000, -25}, {2149000, -30}, {3005000, -35}, {4251000, -40},
};

/*hi3630 IC 2 temperature from -40 to 125*/
static int hi3630_volt_to_temp_general2[NCP_GENERAL2_NUM][2] = {
	{2561, 125}, {2952, 120}, {3415, 115}, {3966, 110}, {4623, 105},
	{5410, 100}, {6357, 95}, {7500, 90}, {8887, 85}, {10580, 80},
	{12640, 75}, {15180, 70}, {18320, 65}, {22220, 60}, {27090, 55},
	{33190, 50}, {40900, 45}, {50680, 40}, {63180, 35}, {79230, 30},
	{100000, 25}, {127000, 20}, {162500, 15}, {209400, 10}, {271800, 5},
	{355600, 0}, {469100, -5}, {624100, -10}, {837800, -15}, {1135000, -20},
	{1554000, -25}, {2149000, -30}, {3005000, -35}, {4251000, -40},
};

/****************************************************
  Function:       int change_volt_to_temp()
  Description: volt to temp
  Input:
  Output:         NA
  Return:         get temperature
  Others:          NA
*****************************************************/
static int change_volt_to_temp(int iResistance, int index_min, int index_max, int table_list)
{
	int prorate = 0;
	int resistance_max = 0;
	int resistance_min = 0;
	int temper_top = 0;
	int temper_bottom = 0;
	int itemper = 0;

	switch (table_list) {
	case ncp_general1_flag:
		resistance_min = hi3630_volt_to_temp_general1[index_min][0];
		resistance_max = hi3630_volt_to_temp_general1[index_max][0];
		temper_bottom = hi3630_volt_to_temp_general1[index_min][1];
		temper_top = hi3630_volt_to_temp_general1[index_max][1];
		break;
	case ncp_general2_flag:
		resistance_min = hi3630_volt_to_temp_general2[index_min][0];
		resistance_max = hi3630_volt_to_temp_general2[index_max][0];
		temper_bottom = hi3630_volt_to_temp_general2[index_min][1];
		temper_top = hi3630_volt_to_temp_general2[index_max][1];
		break;
	default:
		pr_err("ADC channel is not exist!\n\r");
		break;
	}
        if((resistance_max - resistance_min) == 0) {
            return 25;
        }
	prorate =  ((resistance_max - iResistance) * PRORATE_OF_INIT) / (resistance_max - resistance_min);
	itemper = ((temper_bottom - temper_top) * prorate) / PRORATE_OF_INIT + temper_top;
	return itemper;
}

/****************************************************************************
  Function:       int get_ap_ntc_temperature(int iResistance)
  Description:    dichotomy
  Input:          NA
  Output:         NA
  Return:         return temperature
  Others:         NA
*****************************************************************************/
static int get_ntc_temperature(int iResistance, int channel, int **volt_to_temperature, int table_list)
{
	int iLow = 0;
	int iUpper = 0 ;
	int temperature = 0;
	int iMid = 0;
	int *temp_idex = (int *)volt_to_temperature;

	if (ncp_general1_flag == table_list) {
		iUpper = sizeof(hi3630_volt_to_temp_general1) / sizeof(hi3630_volt_to_temp_general1[0][0]) / 2;
	} else if (ncp_general2_flag == table_list) {
		iUpper = sizeof(hi3630_volt_to_temp_general2) / sizeof(hi3630_volt_to_temp_general2[0][0]) / 2;
	}

	if (iResistance < *(temp_idex + 2 * 0 + 0)) {
		temperature = *(temp_idex + 2 * 0 + 1);
		return temperature;
	} else if (iResistance > *(temp_idex + 2 * (iUpper - 1) + 0)) {
		temperature = *(temp_idex + 2 * (iUpper - 1) + 1);
		return temperature;
	}

	while (iLow <= iUpper) {
		iMid = (iLow + iUpper) / 2;
		if (*(temp_idex + 2 * iMid + 0) > iResistance) {
			if (*(temp_idex + 2 * (iMid - 1) + 0) < iResistance) {
				temperature = change_volt_to_temp(iResistance,
						(iMid - 1), iMid, table_list);
				return temperature;
			}
			iUpper = iMid - 1;
		} else if (*(temp_idex + 2 * iMid + 0) < iResistance) {
			if (*(temp_idex + 2 * (iMid + 1) + 0) > iResistance) {
				temperature = change_volt_to_temp(iResistance,
					iMid, (iMid + 1), table_list);
			return temperature;
			}
			iLow = iMid + 1;
		} else {
			temperature = *(temp_idex + 2 * iMid + 1);
			break;
		}
	}
	return temperature;
}

/****************************************************************************
  Function:       int getcalctemperature(int BatteryTemperature)
  Description:    get temperature value
  Calls:          NA
  Data Accessed:  NA
  Data Updated:   NA
  Input:          NA
  Output:         NA
  Return:         vol to temp
  Others:         NA
*****************************************************************************/


int hisi_peripheral_get_temp(struct periph_tsens_tm_device_sensor *chip, unsigned long *temp)
{
	int tempdata = 0;
	int Volt = 0;
	int Resistance1 = 0;
	*temp = tempdata;

	if (chip->ntc_name == NULL) {
		pr_err("ntc_name is NULL\n\r");
		return -EINVAL;
	}

	Volt = hisi_adc_get_value(chip->chanel);
	pr_debug("+++++adc_value=%d+++++\n\r", Volt);
	if (Volt <= 0) {
		pr_err("AP get ADC value is fail,adc_value[%d]!\n\r", Volt);
		Volt = ADC_RTMP_DEFAULT_VALUE;/*if read error, return default value*/
	}

	if (strcmp(chip->ntc_name, "hi3630_volt_to_temp_general1") == 0) {
		Resistance1 = (100 * 1000  * Volt) / (25 * multiple - Volt);
		pr_debug("+++++Resistance1=%d+++++\n\r", Resistance1);
		tempdata = get_ntc_temperature(Resistance1, chip->chanel, (int **)hi3630_volt_to_temp_general1, ncp_general1_flag);
	} else if (strcmp(chip->ntc_name, "hi3630_volt_to_temp_general2") == 0) {
		Resistance1 = (100 * 1000  * Volt) / (25 * multiple - Volt);
		tempdata = get_ntc_temperature(Resistance1, chip->chanel, (int **)hi3630_volt_to_temp_general2, ncp_general2_flag);
	} else {
		pr_err("input ntc name was not found!\n\r");
	}

	*temp = (unsigned long)tempdata;
	pr_debug("+++++++++++temp=%ld++++++++++\n\r", *temp);

	return 0;
}
