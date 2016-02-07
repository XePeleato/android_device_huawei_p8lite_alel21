/* Copyright (c) 2012, Code Aurora Forum. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/thermal.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/kernel.h>
#include <linux/io.h>
#include <linux/slab.h>
#include <linux/err.h>
#include <linux/of.h>
#include <linux/of_irq.h>
#ifdef CONFIG_DEBUG_FS
#include <linux/debugfs.h>
#include <linux/seq_file.h>
#include <linux/uaccess.h>
#endif
#ifdef CONFIG_HI3XXX_RPROC
#include <linux/hisi/hisi_rproc.h>
#endif

static DEFINE_MUTEX(tsens_mutex);

#define TSENS_DRIVER_NAME	"hisi-tsens"
static struct tsens_tm_device	*g_tmdev;

#define CLUSTER0 				0
#define CLUSTER1 				1
#define GPU 					2
#define MODEM 					3
#define DDR 					4
#define	TSENSOR_BUFFER_LENGTH	40

char *hisi_tsensor_name[] = {
	[CLUSTER0]			= "cluster0",
	[CLUSTER1]			= "cluster1",
	[GPU]			= "gpu",
	[MODEM]			= "modem",
	[DDR]			= "ddr",
};


/* Trips: warm and cool */
enum tsens_trip_type {
	TSENS_TRIP_WARM = 0,
	TSENS_TRIP_COOL,
	TSENS_TRIP_NUM,
};
struct tsens_tm_device_sensor {
	struct thermal_zone_device		*tz_dev;
	enum thermal_device_mode		mode;
	unsigned int			sensor_num;
	struct work_struct			work;
	u32				enable[2];
	u32				disable[2];
	u32				read_thermal_h[2];
	u32				read_thermal_l[2];
	u32				write_thermal_h[2];
	u32				write_thermal_l[2];
	u32				read_temperature[2];
};

struct tsens_tm_device {
	struct platform_device		*pdev;
	uint32_t				tsens_num_sensor;
	struct notifier_block 		*nb;
	struct hisi_mbox 			*mbox;
	struct work_struct			tsens_work;
	u32				tsensor_enable_flag;
	u32				tsensor_disable_flag;
	u32				read_thermal_h_flag;
	u32				read_thermal_l_flag;
	u32				write_thermal_h_flag;
	u32				write_thermal_l_flag;
	u32				read_temperature_flag;
	u32				int_reback_flag;
	struct tsens_tm_device_sensor	sensor[0];/*put the struct last place*/
};
struct tsens_context {
	u32 		sensor_id;
	u32 		sensor_cur_temp;
	u32 		sensor_thermal_h;
	u32 		sensor_thermal_l;
};
#ifdef CONFIG_HI3XXX_RPROC
enum {
	TSENSOR_ENABLE = 0,
	TSENSOR_DISABLE,
	TSENSOR_READ_THERMAL_H,
	TSENSOR_READ_THERMAL_L,
	TSENSOR_WRITE_THERMAL_H,
	TSENSOR_WRITE_THERMAL_L,
	TSENSOR_READ_TEMPERATURE,
};

#define TSENS_TO_LPM3_MSG_NUM 	2
#define TSENS_RED_BACK_MSG_NUM 	2
#define TSENSOR_MASK_VALUE		0xFF
#define TSENSOR_8BIT			8
#define TSENSOR_16BIT			16
/*Tsensor temperature change value*/
#define TSENS_THRESHOLD_MAX_CODE	0xff
#define TSENS_THRESHOLD_MIN_CODE	0x0
#define TSENS_TEMP_START_VALUE		(-40)/* -40 deg C */
#define TSENS_TEMP_END_VALUE		140/* 140 deg C */
#define TSENS_ADC_START_VALUE		0
#define TSENS_ADC_END_VALUE		255
/*line -emacro(750,*)*/
/*lint -e750*/
#define ONE_DEGREE_VALUE		(180/255)/*degree C*/
/*lint +e750*/
/*line +emacro(750,*)*/


static int tsens_tz_code_to_degc(int adc_val)
{
	int temp = 0;

	if ((TSENS_THRESHOLD_MIN_CODE > adc_val) || (adc_val > TSENS_THRESHOLD_MAX_CODE)) {
		pr_debug("adc_to_temp_conversion failed \n\r ");
		return temp;
	}

	/*temp = (adc_val * (140 - (-40)))/(255 - 0);*/
	temp = (adc_val * (TSENS_TEMP_END_VALUE - TSENS_TEMP_START_VALUE))
			/ (TSENS_ADC_END_VALUE - TSENS_ADC_START_VALUE);
	/*temp  = temp - 40*/
	temp = temp + TSENS_TEMP_START_VALUE;

	return temp;
}

static int tsens_tz_degc_to_code(unsigned long degc)
{
	int code = 0;

	if ((long)degc > TSENS_TEMP_END_VALUE)
		return TSENS_THRESHOLD_MAX_CODE;
	else if ((long)degc < TSENS_TEMP_START_VALUE)
		return TSENS_THRESHOLD_MIN_CODE;

	/*modulus = (255 - 0)) / (140 - (-40))*/
	/*code = (degc + 40) * modulus;*/
	code = (degc - TSENS_TEMP_START_VALUE) * (TSENS_ADC_END_VALUE - TSENS_ADC_START_VALUE) /
			(TSENS_TEMP_END_VALUE - TSENS_TEMP_START_VALUE);

	return code;
}


static int hisi_tsens_cmd_send(struct tsens_tm_device_sensor *tm_sensor, int cmd)
{
	int i, rc = 0;
	u32 ack_buffer[2];
	u32 tx_buffer[2]={0};
	int temperature;
	int id;

	if (TSENSOR_ENABLE == cmd) {
		tx_buffer[0] = tm_sensor->enable[0];
		tx_buffer[1] = tm_sensor->enable[1];
	} else if (TSENSOR_DISABLE == cmd) {
		tx_buffer[0] = tm_sensor->disable[0];
		tx_buffer[1] = tm_sensor->disable[1];
	} else if (TSENSOR_READ_THERMAL_H == cmd) {
		tx_buffer[0] = tm_sensor->read_thermal_h[0];
		tx_buffer[1] = tm_sensor->read_thermal_h[1];
	} else if (TSENSOR_READ_THERMAL_L == cmd) {
		tx_buffer[0] = tm_sensor->read_thermal_l[0];
		tx_buffer[1] = tm_sensor->read_thermal_l[1];
	} else if (TSENSOR_WRITE_THERMAL_H == cmd) {
		tx_buffer[0] = tm_sensor->write_thermal_h[0];
		tx_buffer[1] = tm_sensor->write_thermal_h[1];
	} else if (TSENSOR_WRITE_THERMAL_L == cmd) {
		tx_buffer[0] = tm_sensor->write_thermal_l[0];
		tx_buffer[1] = tm_sensor->write_thermal_l[1];
	} else if (TSENSOR_READ_TEMPERATURE == cmd) {
		tx_buffer[0] = tm_sensor->read_temperature[0];
		tx_buffer[1] = tm_sensor->read_temperature[1];
	} else {
		pr_err("tsensor send msg to lpm3 err!\n\r");
	}

	for (i = 0; i < TSENS_TO_LPM3_MSG_NUM; i++)
		pr_debug("%s: tx_buffer[%d] = 0x%x\n", __func__, i, tx_buffer[i]);

	rc = RPROC_SYNC_SEND(HISI_RPROC_LPM3,
			tx_buffer,
			TSENS_TO_LPM3_MSG_NUM,
			1,
			ack_buffer,
			TSENS_RED_BACK_MSG_NUM);
	if (0 == rc) {
		for (i = 0; i < TSENS_RED_BACK_MSG_NUM; i++)
			pr_debug("%s: read back msg[%d] = 0x%x\n", __func__, i, ack_buffer[i]);

		if (ack_buffer[0] == g_tmdev->read_temperature_flag ||
			ack_buffer[0] == g_tmdev->read_thermal_h_flag ||
			ack_buffer[0] == g_tmdev->read_thermal_l_flag) {
			temperature = tsens_tz_code_to_degc((ack_buffer[1] >> TSENSOR_8BIT) & TSENSOR_MASK_VALUE);
			id = ack_buffer[1] & TSENSOR_MASK_VALUE;
			pr_debug("%s:Tsensor_ID[%d],Tsensor_temperature[%d]\n", __func__, id, temperature);
			return temperature;
		} else {
			/*BYPASS*/
		}
	} else {
		pr_err("%s: tsens send msg to lpm3 fail!\n", __func__);
		return -EINVAL;
	}

	return 0;
}

static int hisi_tsens_mbox_notifier(struct notifier_block *nb, unsigned long len, void *msg)
{
	u32 *result = (u32 *)msg;
	int sensor_id, temp, interrupt_flag;

	if (result[0] == g_tmdev->int_reback_flag) {
		sensor_id = result[1] & TSENSOR_MASK_VALUE;
		temp = tsens_tz_code_to_degc((result[1] >> TSENSOR_8BIT) & TSENSOR_MASK_VALUE);
		interrupt_flag = (result[1] >> TSENSOR_16BIT) & TSENSOR_MASK_VALUE;
		schedule_work(&g_tmdev->sensor[sensor_id].work);
		pr_debug("sensor[%d] trigger temp (%d degC) Level[%d]\n",
					sensor_id, temp, interrupt_flag);
	}

	mb();
	return 0;
}
#endif

static int tsens_tz_get_temp(struct thermal_zone_device *thermal,
			     unsigned long *temp)
{
#ifdef CONFIG_HI3XXX_RPROC
	struct tsens_tm_device_sensor *tm_sensor = thermal->devdata;

	if (!tm_sensor || tm_sensor->mode != THERMAL_DEVICE_ENABLED || !temp)
		return -EINVAL;

	tm_sensor->read_temperature[0] = g_tmdev->read_temperature_flag;
	tm_sensor->read_temperature[1] = tm_sensor->sensor_num;
	*temp = (unsigned long)hisi_tsens_cmd_send(tm_sensor, TSENSOR_READ_TEMPERATURE);
	if ((long)*temp < TSENS_TEMP_START_VALUE) {
		pr_err("%s: tsens get temperature value fail,temp=[%ld]\n", __func__, *temp);
		return -EINVAL;
	}
#endif
	return 0;
}

static int tsens_tz_get_mode(struct thermal_zone_device *thermal,
			      enum thermal_device_mode *mode)
{
	struct tsens_tm_device_sensor *tm_sensor = thermal->devdata;

	if (!tm_sensor || !mode)
		return -EINVAL;

	*mode = tm_sensor->mode;

	return 0;
}

static int tsens_tz_get_trip_type(struct thermal_zone_device *thermal,
				   int trip, enum thermal_trip_type *type)
{
	struct tsens_tm_device_sensor *tm_sensor = thermal->devdata;
	if (!tm_sensor || trip < 0 || !type)
		return -EINVAL;
	switch (trip) {
	case TSENS_TRIP_WARM:
		*type = THERMAL_TRIP_CONFIGURABLE_HI;
		break;
	case TSENS_TRIP_COOL:
		*type = THERMAL_TRIP_CONFIGURABLE_LOW;
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

static int tsens_tz_activate_trip_type(struct thermal_zone_device *thermal,
			int trip, enum thermal_trip_activation_mode mode)
{
#ifdef CONFIG_HI3XXX_RPROC
	struct tsens_tm_device_sensor *tm_sensor = thermal->devdata;
	int ret;

	if (!tm_sensor || trip < 0)
		return -EINVAL;

	switch (trip) {
	case TSENS_TRIP_WARM:
		if (mode == THERMAL_TRIP_ACTIVATION_DISABLED) {
			tm_sensor->disable[0] = g_tmdev->tsensor_disable_flag;
			tm_sensor->disable[1] = (0x01 << TSENSOR_8BIT) | tm_sensor->sensor_num;
			ret = hisi_tsens_cmd_send(tm_sensor, TSENSOR_DISABLE);
			if (ret) {
				pr_err("%s: tsens send disable cmd fail\n", __func__);
				return -EINVAL;
			}
		} else {
			tm_sensor->enable[0] = g_tmdev->tsensor_enable_flag;
			tm_sensor->enable[1] = (0x01 << TSENSOR_8BIT) | tm_sensor->sensor_num;
			ret = hisi_tsens_cmd_send(tm_sensor, TSENSOR_ENABLE);
			if (ret) {
				pr_err("%s: tsens send eanble cmd fail\n", __func__);
				return -EINVAL;
			}
		}

		break;
	case TSENS_TRIP_COOL:
		if (mode == THERMAL_TRIP_ACTIVATION_DISABLED) {
			tm_sensor->disable[0] = g_tmdev->tsensor_disable_flag;
			tm_sensor->disable[1] = (0x00 << TSENSOR_8BIT) | tm_sensor->sensor_num;
			ret = hisi_tsens_cmd_send(tm_sensor, TSENSOR_DISABLE);
			if (ret) {
				pr_err("%s: tsens send disable cmd fail\n", __func__);
				return -EINVAL;
			}
		} else {
			tm_sensor->enable[0] = g_tmdev->tsensor_enable_flag;
			tm_sensor->enable[1] = (0x00 << TSENSOR_8BIT) | tm_sensor->sensor_num;
			ret = hisi_tsens_cmd_send(tm_sensor, TSENSOR_ENABLE);
			if (ret) {
				pr_err("%s: tsens send eanble cmd fail\n", __func__);
				return -EINVAL;
			}
		}

		break;
	}
#endif
	mb();
	return 0;
}

static int tsens_tz_get_trip_temp(struct thermal_zone_device *thermal,
				   int trip, unsigned long *temp)
{
#ifdef CONFIG_HI3XXX_RPROC
	struct tsens_tm_device_sensor *tm_sensor = thermal->devdata;

	if (!tm_sensor || trip < 0 || !temp)
		return -EINVAL;

	switch (trip) {
	case TSENS_TRIP_WARM:
		tm_sensor->read_thermal_h[0] = g_tmdev->read_thermal_h_flag;
		tm_sensor->read_thermal_h[1] = tm_sensor->sensor_num;
		*temp = (unsigned long)hisi_tsens_cmd_send(tm_sensor, TSENSOR_READ_THERMAL_H);
		if ((long)*temp < TSENS_TEMP_START_VALUE) {
			pr_err("%s: tsens get high thermal value fail, temperature=%ld\n", __func__, *temp);
			return -EINVAL;
		}
		break;
	case TSENS_TRIP_COOL:
		tm_sensor->read_thermal_l[0] = g_tmdev->read_thermal_l_flag;
		tm_sensor->read_thermal_l[1] = tm_sensor->sensor_num;
		*temp = (unsigned long)hisi_tsens_cmd_send(tm_sensor, TSENSOR_READ_THERMAL_L);
		if ((long)*temp < TSENS_TEMP_START_VALUE) {
			pr_err("%s: tsens get low thermal value fail, temperature=%ld\n", __func__, *temp);
			return -EINVAL;
		}
		break;
	default:
		return -EINVAL;
	}
#endif
	return 0;
}

static int tsens_tz_set_trip_temp(struct thermal_zone_device *thermal,
				   int trip, unsigned long temp)
{
#ifdef CONFIG_HI3XXX_RPROC
	struct tsens_tm_device_sensor *tm_sensor = thermal->devdata;
	int ret;
	int temperature;

	if (!tm_sensor || trip < 0 || (long)temp < TSENS_TEMP_START_VALUE || (long)temp > TSENS_TEMP_END_VALUE)
		return -EINVAL;

	temperature = tsens_tz_degc_to_code(temp);

	switch (trip) {
	case TSENS_TRIP_WARM:
		tm_sensor->write_thermal_h[0] = g_tmdev->write_thermal_h_flag;
		tm_sensor->write_thermal_h[1] = (temperature << 8) | tm_sensor->sensor_num;
		ret = hisi_tsens_cmd_send(tm_sensor, TSENSOR_WRITE_THERMAL_H);
		if (ret) {
			pr_err("%s: hisi tsens write high thermal fail\n", __func__);
			return -EINVAL;
		}
		break;
	case TSENS_TRIP_COOL:
		tm_sensor->write_thermal_l[0] = g_tmdev->write_thermal_l_flag;
		tm_sensor->write_thermal_l[1] = (temperature << 8) | tm_sensor->sensor_num;
		ret = hisi_tsens_cmd_send(tm_sensor, TSENSOR_WRITE_THERMAL_L);
		if (ret) {
			pr_err("%s: hisi tsens write low thermal fail\n", __func__);
			return -EINVAL;
		}
		break;
	default:
		return -EINVAL;
	}
#endif
	mb();
	return 0;
}

static int tsens_tz_notify(struct thermal_zone_device *thermal,
				int count, enum thermal_trip_type type)
{
	/* TSENS driver does not shutdown the device.
	   All Thermal notification are sent to the
	   thermal daemon to take appropriate action */
	pr_debug("%s debug\n", __func__);
	return 0;
}

static struct thermal_zone_device_ops tsens_thermal_zone_ops = {
	.get_temp = tsens_tz_get_temp,
	.get_mode = tsens_tz_get_mode,
	.get_trip_type = tsens_tz_get_trip_type,
	.activate_trip_type = tsens_tz_activate_trip_type,
	.get_trip_temp = tsens_tz_get_trip_temp,
	.set_trip_temp = tsens_tz_set_trip_temp,
	.notify = tsens_tz_notify,
};

static void notify_uspace_tsens_fn(struct work_struct *work)
{
	struct tsens_tm_device_sensor *tm = container_of(work,
		struct tsens_tm_device_sensor, work);
	sysfs_notify(&tm->tz_dev->device.kobj,
					NULL, "type");
}

static int get_device_tree_data(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	const struct device_node *of_node = pdev->dev.of_node;
	unsigned int register_info = 0;
	u32 rc = 0, tsens_num_sensors = 0;

	/* parse .hisi tsensor number */
	rc = of_property_read_u32(of_node, "hisi,sensors", &tsens_num_sensors);
	if (rc) {
		dev_err(&pdev->dev, "missing sensor number\n");
		return -ENODEV;
	}

	g_tmdev = devm_kzalloc(&pdev->dev, sizeof(struct tsens_tm_device) +
			tsens_num_sensors * sizeof(struct tsens_tm_device_sensor), GFP_KERNEL);
	if (g_tmdev == NULL) {
		dev_err(&pdev->dev, "kzalloc() failed.\n");
		return -ENOMEM;
	}

	g_tmdev->tsens_num_sensor = tsens_num_sensors;
	/* parse .hisi tsensor enable cmd */
	rc = of_property_read_u32(of_node, "hisi,tsensor_enable_cmd", &register_info);
	if (rc) {
		dev_err(dev, "no hisi,tsensor_enable_cmd cmd!\n");
		goto dt_parse_common_end;
	}
	g_tmdev->tsensor_enable_flag = register_info;
	/* parse .hisi tsensor disable cmd */
	rc = of_property_read_u32(of_node, "hisi,tsensor_disable_cmd", &register_info);
	if (rc) {
		dev_err(dev, "no hisi,tsensor_disable_cmd cmd!\n");
		goto dt_parse_common_end;
	}
	g_tmdev->tsensor_disable_flag = register_info;
	/* parse .hisi tsensor read high thermal cmd */
	rc = of_property_read_u32(of_node, "hisi,tsensor_read_thermal_h_cmd", &register_info);
	if (rc) {
		dev_err(dev, "no hisi,tsensor read high thermal cmd!\n");
		goto dt_parse_common_end;
	}
	g_tmdev->read_thermal_h_flag = register_info;
	/* parse .hisi tsensor read low thermal cmd */
	rc = of_property_read_u32(of_node, "hisi,tsensor_read_thermal_l_cmd", &register_info);
	if (rc) {
		dev_err(dev, "no hisi,tsensor read low thermal cmd!\n");
		goto dt_parse_common_end;
	}
	g_tmdev->read_thermal_l_flag = register_info;
	/* parse .hisi tsensor write high thermal cmd */
	rc = of_property_read_u32(of_node, "hisi,tsensor_write_thermal_h_cmd", &register_info);
	if (rc) {
		dev_err(dev, "no hisi,tsensor write high thermal cmd!\n");
		goto dt_parse_common_end;
	}
	g_tmdev->write_thermal_h_flag = register_info;
	/* parse .hisi tsensor write low thermal cmd */
	rc = of_property_read_u32(of_node, "hisi,tsensor_write_thermal_l_cmd", &register_info);
	if (rc) {
		dev_err(dev, "no hisi,tsensor write low thermal cmd!\n");
		goto dt_parse_common_end;
	}
	g_tmdev->write_thermal_l_flag = register_info;
	/* parse .hisi tsensor read temperature cmd */
	rc = of_property_read_u32(of_node, "hisi,tsensor_read_temperature_cmd", &register_info);
	if (rc) {
		dev_err(dev, "no hisi,tsensor read temperature cmd!\n");
		goto dt_parse_common_end;
	}
	g_tmdev->read_temperature_flag = register_info;
	/* parse .hisi tsensor INT reback cmd */
	rc = of_property_read_u32(of_node, "hisi,tsensor_int_reback_cmd", &register_info);
	if (rc) {
		dev_err(dev, "no hisi,tsensor int reback cmd!\n");
		goto dt_parse_common_end;
	}
	g_tmdev->int_reback_flag = register_info;

dt_parse_common_end:
	return 0;
}

#ifdef CONFIG_DEBUG_FS
#define TEST_H_TEMP	221
#define TSET_L_TEMP	65
#define TEST_H_TEMP1	65
#define TEST_L_TEMP1	117
static int dbg_control_tsens_show(struct seq_file *s, void *data)
{
	pr_debug("dbg_control_tsens_show \n\r");
	return 0;
}
static ssize_t dbg_control_tsens_set_value(struct file *filp, const char __user *buffer,
	size_t count, loff_t *ppos)
{
	int rc = 0;
#ifdef CONFIG_HI3XXX_RPROC
	struct tsens_tm_device_sensor *dbg_sensor = NULL;
	char tmp[128] = {0};
	long temperature, temp1, temp2, temp3, temp4;
	int ret;
	int index = 0;

	if (count > 128) {
		pr_debug("error! buffer size big than internal buffer\n");
		return -EFAULT;
	}

	if (copy_from_user(tmp, buffer, count)) {
		pr_debug("error!\n");
		return -EFAULT;
	}
	temp1 = TEST_H_TEMP;
	temp2 = TSET_L_TEMP;
	temp3 = TEST_H_TEMP1;
	temp4 = TEST_L_TEMP1;
	dbg_sensor = kmalloc(sizeof(struct tsens_tm_device_sensor), GFP_KERNEL);
	if (!dbg_sensor) {
                pr_err("%s:Failed to alloc memory for dbg sensor!\n", __func__);
                return -ENOMEM;
        }
	if (sscanf(tmp, "%d", &index)) {
		switch (index) {
		case 0:
			dbg_sensor->sensor_num = 1;
			dbg_sensor->read_temperature[0] = g_tmdev->read_temperature_flag;
			dbg_sensor->read_temperature[1] = dbg_sensor->sensor_num;
			temperature = (long)hisi_tsens_cmd_send(dbg_sensor, TSENSOR_READ_TEMPERATURE);
			if (temperature < TSENS_TEMP_START_VALUE)
				pr_err("%s: tsens get temperature value fail,temperature=%ld\n", __func__, temperature);
			break;
		case 1:
			dbg_sensor->sensor_num = 1;
			dbg_sensor->enable[0] = g_tmdev->tsensor_enable_flag;
			dbg_sensor->enable[1] =  (0x01 << TSENSOR_8BIT) | dbg_sensor->sensor_num;
			ret = hisi_tsens_cmd_send(dbg_sensor, TSENSOR_ENABLE);
			if (ret) {
				pr_err("%s: tsens send eanble cmd fail\n", __func__);
				rc = -EINVAL;
				goto get_value_fail;
			}
			break;
		case 2:
			dbg_sensor->sensor_num = 1;
			dbg_sensor->disable[0] = g_tmdev->tsensor_disable_flag;
			dbg_sensor->disable[1] = (0x01 << TSENSOR_8BIT) | dbg_sensor->sensor_num;
			ret = hisi_tsens_cmd_send(dbg_sensor, TSENSOR_DISABLE);
			if (ret) {
				pr_err("%s: tsens send disable cmd fail\n", __func__);
				rc = -EINVAL;
				goto get_value_fail;
			}
			break;
		case 3:
			dbg_sensor->sensor_num = 1;
			dbg_sensor->read_thermal_h[0] = g_tmdev->read_thermal_h_flag;
			dbg_sensor->read_thermal_h[1] = dbg_sensor->sensor_num;
			temperature = (long)hisi_tsens_cmd_send(dbg_sensor, TSENSOR_READ_THERMAL_H);
			if (temperature < TSENS_TEMP_START_VALUE) {
				pr_err("%s: tsens get high thermal value fail, temperature=%ld\n", __func__, temperature);
				rc = -EINVAL;
				goto get_value_fail;
			}
			break;
		case 4:
			dbg_sensor->sensor_num = 1;
			dbg_sensor->read_thermal_l[0] = g_tmdev->read_thermal_l_flag;
			dbg_sensor->read_thermal_l[1] = dbg_sensor->sensor_num;
			temperature = (long)hisi_tsens_cmd_send(dbg_sensor, TSENSOR_READ_THERMAL_L);
			if (temperature < TSENS_TEMP_START_VALUE) {
				pr_err("%s: tsens get low thermal value fail, temperature=%ld\n", __func__, temperature);
				rc = -EINVAL;
				goto get_value_fail;
			}
			break;
		case 5:
			dbg_sensor->sensor_num = 1;
			dbg_sensor->write_thermal_h[0] = g_tmdev->write_thermal_h_flag;
			dbg_sensor->write_thermal_h[1] = temp1 << 8 | 0x01;
			ret = hisi_tsens_cmd_send(dbg_sensor, TSENSOR_WRITE_THERMAL_H);
			if (ret) {
				pr_err("%s: hisi tsens write high thermal fail\n", __func__);
				rc = -EINVAL;
				goto get_value_fail;
			}
			break;
		case 6:
			dbg_sensor->sensor_num = 1;
			dbg_sensor->write_thermal_l[0] = g_tmdev->write_thermal_l_flag;
			dbg_sensor->write_thermal_l[1] = temp2 << 8 | 0x01;
			ret = hisi_tsens_cmd_send(dbg_sensor, TSENSOR_WRITE_THERMAL_L);
			if (ret) {
				pr_err("%s: hisi tsens write low thermal fail\n", __func__);
				rc = -EINVAL;
				goto get_value_fail;
			}
			break;
		case 7:
			dbg_sensor->sensor_num = 1;
			dbg_sensor->enable[0] = g_tmdev->tsensor_enable_flag;
			dbg_sensor->enable[1] =  (0x00 << TSENSOR_8BIT) | dbg_sensor->sensor_num;
			ret = hisi_tsens_cmd_send(dbg_sensor, TSENSOR_ENABLE);
			if (ret) {
				pr_err("%s: tsens send eanble cmd fail\n", __func__);
				rc = -EINVAL;
				goto get_value_fail;
			}
			break;
		case 8:
			dbg_sensor->sensor_num = 1;
			dbg_sensor->disable[0] = g_tmdev->tsensor_disable_flag;
			dbg_sensor->disable[1] = (0x00 << TSENSOR_8BIT) | dbg_sensor->sensor_num;
			ret = hisi_tsens_cmd_send(dbg_sensor, TSENSOR_DISABLE);
			if (ret) {
				pr_err("%s: tsens send disable cmd fail\n", __func__);
				rc = -EINVAL;
				goto get_value_fail;
			}
			break;
		case 9:
			dbg_sensor->sensor_num = 1;
			dbg_sensor->write_thermal_h[0] = g_tmdev->write_thermal_h_flag;
			dbg_sensor->write_thermal_h[1] = temp3 << 8 | 0x01;
			ret = hisi_tsens_cmd_send(dbg_sensor, TSENSOR_WRITE_THERMAL_H);
			if (ret) {
				pr_err("%s: hisi tsens write high thermal fail\n", __func__);
				rc = -EINVAL;
				goto get_value_fail;
			}
			break;
		case 10:
			dbg_sensor->sensor_num = 1;
			dbg_sensor->write_thermal_l[0] = g_tmdev->write_thermal_l_flag;
			dbg_sensor->write_thermal_l[1] = temp4 << 8 | 0x01;
			ret = hisi_tsens_cmd_send(dbg_sensor, TSENSOR_WRITE_THERMAL_L);
			if (ret) {
				pr_err("%s: hisi tsens write low thermal fail\n", __func__);
				rc = -EINVAL;
				goto get_value_fail;
			}
			break;
		default:
			pr_err("%s:input index err!\n", __func__);
			break;
		}
	} else {
		pr_debug("ERRR~\n");
	}

	*ppos += count;
	kfree(dbg_sensor);

	return count;
get_value_fail:
	kfree(dbg_sensor);
#endif
	return rc;
}

static int dbg_control_tsens_open(struct inode *inode, struct file *file)
{
	file->private_data = inode->i_private;
	return single_open(file, dbg_control_tsens_show, &inode->i_private);
}

static const struct file_operations set_control_tsens_fops = {
	.open		= dbg_control_tsens_open,
	.read		= seq_read,
	.write		= dbg_control_tsens_set_value,
	.llseek		= seq_lseek,
	.release	= single_release,
};
#endif


static int  tsens_tm_probe(struct platform_device *pdev)
{
	int rc = 0;
#ifdef CONFIG_DEBUG_FS
	struct dentry *d;
#endif

	if (g_tmdev) {
		dev_err(&pdev->dev, "TSENS device already in use\n");
		return -EBUSY;
	}

	if (pdev->dev.of_node) {
		rc = get_device_tree_data(pdev);
		if (rc) {
			dev_err(&pdev->dev, "Error reading TSENS DT\n");
			return rc;
		}
	} else
		return -ENODEV;
	g_tmdev->pdev = pdev;

	platform_set_drvdata(pdev, g_tmdev);

#ifdef CONFIG_DEBUG_FS
	d = debugfs_create_dir("hisi_tsens_debugfs", NULL);
	if (!d) {
		dev_err(&pdev->dev, "failed to create hisi tsens dir !\n");
		return -ENOMEM;
	}

	(void) debugfs_create_file("control_tsens", S_IRUSR,
					d, NULL, &set_control_tsens_fops);
#endif
	return 0;
}

static int  _tsens_register_thermal(void)
{
	struct platform_device *pdev;
	int rc = 0, i, j, k;
	char name[18];
	char *temp_buffer;

	if (!g_tmdev) {
		pr_err("TSENS early init not done!\n");
		return -ENODEV;
	}

	pdev = g_tmdev->pdev;

	temp_buffer = (char *)kmalloc(TSENSOR_BUFFER_LENGTH * sizeof(char), GFP_KERNEL);
	if (!temp_buffer) {
                pr_err("%s:Failed to alloc memory for temp buffer!\n", __func__);
                return -ENOMEM;
    }

	for (i = 0, j = 0; i < g_tmdev->tsens_num_sensor; i++, j++) {
		memset(temp_buffer, 0, TSENSOR_BUFFER_LENGTH);
		sprintf(temp_buffer, "%s", hisi_tsensor_name[i]);
		snprintf(name, sizeof(name), temp_buffer, i);
		g_tmdev->sensor[i].mode = THERMAL_DEVICE_ENABLED;
		g_tmdev->sensor[i].sensor_num = i;
		g_tmdev->sensor[i].tz_dev = thermal_zone_device_register(name,
				TSENS_TRIP_NUM, 0, &g_tmdev->sensor[i],
				&tsens_thermal_zone_ops, NULL, 0, 0);
		if (IS_ERR(g_tmdev->sensor[i].tz_dev)) {
			dev_err(&pdev->dev, "Tsensor thermal_zone_device_register() failed\n");
			rc = -ENODEV;
			goto register_fail;
		}
	}
	kfree(temp_buffer);
	temp_buffer = NULL;

	platform_set_drvdata(pdev, g_tmdev);
	/*register tm sensor notify userspace work.*/
	for (k = 0; k < g_tmdev->tsens_num_sensor; k++) {
		INIT_WORK(&g_tmdev->sensor[k].work, notify_uspace_tsens_fn);
	}

#ifdef CONFIG_HI3XXX_RPROC
	g_tmdev->nb = kmalloc(sizeof(struct notifier_block), GFP_KERNEL);
	if (!g_tmdev->nb) {
		dev_err(&pdev->dev, "failed to get mbox notifier block !\n");
		goto mailbox_nb_malloc_fail;
	}
	g_tmdev->nb->next = NULL;
	g_tmdev->nb->notifier_call = hisi_tsens_mbox_notifier;
	rc = RPROC_MONITOR_REGISTER(HISI_RPROC_LPM3, g_tmdev->nb);
	if (rc) {
		dev_err(&pdev->dev, "rproc monitor register Failed!\n");
		goto mailbox_get_nb_fail;
	}
#endif

	return 0;
#ifdef CONFIG_HI3XXX_RPROC
mailbox_get_nb_fail:
	kfree(g_tmdev->nb);
mailbox_nb_malloc_fail:
#endif
register_fail:
        kfree(temp_buffer);
        temp_buffer = NULL;
	for (i = 0; i < j; i++)
		thermal_zone_device_unregister(g_tmdev->sensor[i].tz_dev);
	return rc;
}

static int  tsens_tm_remove(struct platform_device *pdev)
{
        int i;
	struct tsens_tm_device *tmdev = platform_get_drvdata(pdev);
        if(tmdev == NULL){
            return -1;
        }

#ifdef CONFIG_HI3XXX_RPROC
	kfree(tmdev->nb);
#endif
	for (i = 0; i < tmdev->tsens_num_sensor; i++)
		thermal_zone_device_unregister(tmdev->sensor[i].tz_dev);

	platform_set_drvdata(pdev, NULL);

	return 0;
}

static struct of_device_id tsens_match[] = {
	{.compatible = "hisi,tsens",
	},
	{}
};

static struct platform_driver tsens_tm_driver = {
	.probe = tsens_tm_probe,
	.remove = tsens_tm_remove,
	.driver = {
		.name = "hisi-tsens",
		.owner = THIS_MODULE,
		.of_match_table = tsens_match,
	},
};

static int __init tsens_tm_init_driver(void)
{
	return platform_driver_register(&tsens_tm_driver);
}
arch_initcall(tsens_tm_init_driver);

static int __init tsens_thermal_register(void)
{
	return _tsens_register_thermal();
}
module_init(tsens_thermal_register);

static void __exit _tsens_tm_remove(void)
{
	platform_driver_unregister(&tsens_tm_driver);
}
module_exit(_tsens_tm_remove);

MODULE_ALIAS("platform:" TSENS_DRIVER_NAME);
MODULE_LICENSE("GPL v2");
