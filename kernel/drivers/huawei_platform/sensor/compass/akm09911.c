/* drivers/misc/akm09911.c - akm09911 compass driver
 *
 * Copyright (C) 2007-2008 HTC Corporation.
 * Author: Hou-Kun Chen <houkun.chen@gmail.com>
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

/*#define DEBUG*/
/*#define VERBOSE_DEBUG*/



#include "akm09911.h"
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/freezer.h>
#include <linux/gpio.h>
#include <linux/i2c.h>
#include <linux/input.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/workqueue.h>
#include <linux/board_sensors.h>
#include <linux/of_gpio.h>
#include <huawei_platform/sensor/sensor_info.h>
#ifdef CONFIG_HUAWEI_HW_DEV_DCT
#include <linux/hw_dev_dec.h>
#endif

#if defined (CONFIG_HUAWEI_DSM)
#include <linux/mfd/hisi_hi6xxx_pmic.h>
#include <huawei_platform/dsm/dsm_pub.h>
extern struct dsm_client *compass_dclient;
static int akm09911_flag = 0;
#endif
extern bool DT_tset;
extern int compass_data_count;
extern int Gyro_data_count;
/* IOCTLs for AKM library */
#define ECS_IOCTL_READ				_IOWR(AKMIO, 0x01, char)
#define ECS_IOCTL_WRITE				_IOW(AKMIO, 0x02, char)
#define ECS_IOCTL_RESET				_IO(AKMIO, 0x03)
#define ECS_IOCTL_SET_MODE			_IOW(AKMIO, 0x10, char)
#define ECS_IOCTL_SET_YPR			_IOW(AKMIO, 0x11, int32_t[AKM_YPR_DATA_SIZE])
#define ECS_IOCTL_GET_INFO			_IOR(AKMIO, 0x20, unsigned char[AKM_SENSOR_INFO_SIZE])
#define ECS_IOCTL_GET_CONF			_IOR(AKMIO, 0x21, unsigned char[AKM_SENSOR_CONF_SIZE])
#define ECS_IOCTL_GET_DATA			_IOR(AKMIO, 0x22, unsigned char[AKM_SENSOR_DATA_SIZE])
#define ECS_IOCTL_GET_OPEN_STATUS	_IOR(AKMIO, 0x23, int)
#define ECS_IOCTL_GET_CLOSE_STATUS	_IOR(AKMIO, 0x24, int)
#define ECS_IOCTL_GET_DELAY			_IOR(AKMIO, 0x25, long long int)
#define ECS_IOCTL_GET_LAYOUT		_IOR(AKMIO, 0x26, char)
#define ECS_IOCTL_GET_ACCEL			_IOR(AKMIO, 0x30, short[3])

#define AKM_DEBUG_IF			0
#define AKM_HAS_RESET			1
#define AKM_ACCEL_ITEMS 3
//#define AKM_INPUT_DEVICE_NAME	"input_compass"
#define AKM_DRDY_TIMEOUT_MS		100
#define AKM_BASE_NUM			10

struct akm_compass_data {
	struct i2c_client	*i2c;
	struct input_dev	*input;
	struct device		*class_dev;
	struct class		*compass;

	wait_queue_head_t	drdy_wq;
	wait_queue_head_t	open_wq;

	/* These two buffers are initialized at start up.
	   After that, the value is not changed */
	uint8_t sense_info[AKM_SENSOR_INFO_SIZE];
	uint8_t sense_conf[AKM_SENSOR_CONF_SIZE];

	struct	mutex sensor_mutex;
	uint8_t	sense_data[AKM_SENSOR_DATA_SIZE];
	struct mutex accel_mutex;
	int16_t	accel_data[AKM_ACCEL_ITEMS];

	/* Positive value means the device is working.
	   0 or negative value means the device is not woking,
	   i.e. in power-down mode. */
	int8_t	is_busy;

	struct mutex	val_mutex;
	uint32_t		enable_flag;
	int64_t			delay[AKM_NUM_SENSORS];

	atomic_t	active;

	atomic_t	drdy;
	atomic_t	suspend;

	char layout;
	int	irq;
	int	gpio_rstn;
};

static struct akm_compass_data *s_akm;
static int calibration_value=0;



/***** I2C I/O function ***********************************************/
static int akm_i2c_rxdata(
	struct i2c_client *i2c,
	uint8_t *rxData,
	int length)
{
	int ret;
#if defined (CONFIG_HUAWEI_DSM)
   	unsigned char LDO2_status=0;
	unsigned char LDO5_status=0;
#endif
	struct i2c_msg msgs[] = {
		{
			.addr = i2c->addr,
			.flags = 0,
			.len = 1,
			.buf = rxData,
		},
		{
			.addr = i2c->addr,
			.flags = I2C_M_RD,
			.len = length,
			.buf = rxData,
		},
	};
	uint8_t addr = rxData[0];

	ret = i2c_transfer(i2c->adapter, msgs, ARRAY_SIZE(msgs));
	if (ret < 0) {

		compass_ERR("[COMPASS]%s: transfer failed.", __func__);
#if defined (CONFIG_HUAWEI_DSM)
		    LDO2_status = hi6xxx_pmic_reg_read(LDO2_PHYS_ADDR);
		    LDO5_status = hi6xxx_pmic_reg_read(LDO5_PHYS_ADDR);
            compass_ERR("[COMPASS]%s: LDO2_status=%d\n", __func__,LDO2_PHYS_ADDR);
            compass_ERR("[COMPASS]%s: LDO5_status=%d\n", __func__,LDO5_PHYS_ADDR);

	if(1==akm09911_flag)
	  {
		if(!dsm_client_ocuppy(compass_dclient)){
		dsm_client_record(compass_dclient, "[%s]i2c_status:%d,rst is 0x%x,LDO2_status is 0x%x,LDO5_status is 0x%x.\n",__func__, ret,gpio_get_value(s_akm->gpio_rstn),LDO2_status,LDO5_status);
		dsm_client_notify(compass_dclient, DSM_COMPASS_I2C_RW_ERROR_NO);
		}
	 }
#endif
		return ret;
	} else if (ret != ARRAY_SIZE(msgs)) {
		compass_ERR("[COMPASS]%s: transfer failed(size error).\n",__func__);

#if defined (CONFIG_HUAWEI_DSM)
		LDO2_status = hi6xxx_pmic_reg_read(LDO2_PHYS_ADDR);
		LDO5_status = hi6xxx_pmic_reg_read(LDO5_PHYS_ADDR);
		compass_ERR("[COMPASS]%s: LDO2_status=%d\n", __func__,LDO2_PHYS_ADDR);
		compass_ERR("[COMPASS]%s: LDO5_status=%d\n", __func__,LDO5_PHYS_ADDR);
		//gpio_get_value(s_akm->gpio_rstn);
	if(1==akm09911_flag)
	{ if(!dsm_client_ocuppy(compass_dclient)){
		  dsm_client_record(compass_dclient, "[%s]i2c_status:%d,reset is 0x%x,LDO2_status is 0x%x,LDO5_status is 0x%x.\n",__func__, ret,gpio_get_value(s_akm->gpio_rstn),LDO2_status,LDO5_status);
		 dsm_client_notify(compass_dclient, DSM_COMPASS_I2C_RW_ERROR_NO);
		}
	}
#endif
		return -ENXIO;
	}

	compass_FLOW("[COMPASS]RxData: len=%02x, addr=%02x, data=%02x",length, addr, rxData[0]);

	return 0;
}

static int akm_i2c_txdata(
	struct i2c_client *i2c,
	uint8_t *txData,
	int length)
{
	int ret;
#if defined (CONFIG_HUAWEI_DSM)
    unsigned char LDO2_status=0;
	unsigned char LDO5_status=0;
#endif
	struct i2c_msg msg[] = {
		{
			.addr = i2c->addr,
			.flags = 0,
			.len = length,
			.buf = txData,
		},
	};

	ret = i2c_transfer(i2c->adapter, msg, ARRAY_SIZE(msg));
	if (ret < 0) {
		compass_ERR("[COMPASS]%s: transfer failed.", __func__);
#if defined (CONFIG_HUAWEI_DSM)
		LDO2_status = hi6xxx_pmic_reg_read(LDO2_PHYS_ADDR);
		LDO5_status = hi6xxx_pmic_reg_read(LDO5_PHYS_ADDR);
		compass_ERR("[COMPASS]%s: LDO2_status=%d\n", __func__,LDO2_PHYS_ADDR);
		compass_ERR("[COMPASS]%s: LDO5_status=%d\n", __func__,LDO5_PHYS_ADDR);

	if(1==akm09911_flag)
    {
		if(!dsm_client_ocuppy(compass_dclient)){
		dsm_client_record(compass_dclient, "[%s]i2c_status:%d,reset is 0x%x,LDO2_status is 0x%x,LDO5_status is 0x%x.\n",__func__, ret,gpio_get_value(s_akm->gpio_rstn),LDO2_status,LDO5_status);
		dsm_client_notify(compass_dclient, DSM_COMPASS_I2C_RW_ERROR_NO);
		}
	}
#endif
		return ret;
	} else if (ret != ARRAY_SIZE(msg)) {
		compass_ERR("[COMPASS]%s: transfer failed(size error).",__func__);

#if defined (CONFIG_HUAWEI_DSM)
	if(1==akm09911_flag)
	{
		if(!dsm_client_ocuppy(compass_dclient)){
		LDO2_status = hi6xxx_pmic_reg_read(LDO2_PHYS_ADDR);
		LDO5_status = hi6xxx_pmic_reg_read(LDO5_PHYS_ADDR);
		compass_ERR("[COMPASS]%s: LDO2_status=%d\n", __func__,LDO2_PHYS_ADDR);
		compass_ERR("[COMPASS]%s: LDO5_status=%d\n", __func__,LDO5_PHYS_ADDR);
		//gpio_get_value(s_akm->gpio_rstn);
		dsm_client_record(compass_dclient, "[%s]i2c_status:%d,reset is 0x%x,LDO2_status is 0x%x,LDO5_status is 0x%x.\n",__func__, ret,gpio_get_value(s_akm->gpio_rstn),LDO2_status,LDO5_status);
		dsm_client_notify(compass_dclient, DSM_COMPASS_I2C_RW_ERROR_NO);
		}
	}
#endif
		return -ENXIO;
	}

	compass_FLOW("[COMPASS]TxData: len=%02x, addr=%02x data=%02x",length, txData[0], txData[1]);

	return 0;
}

#if 0

static int akm_regulator_configure(struct akm09911_platform_data *pdata, struct i2c_client *client)
{
	int rc;

	pdata->akm_vdd = regulator_get(&client->dev, "SENSORS_ANALOG_VDD");
	if (IS_ERR(pdata->akm_vdd)) {
		rc = PTR_ERR(pdata->akm_vdd);
		compass_ERR("[COMPASS]Regulator get failed akm_vdd rc=%d\n", rc);
		return rc;
	}

	rc = regulator_set_voltage(pdata->akm_vdd, 2850000,
							2850000);
		if (rc) {
			compass_ERR("[COMPASS]regulator set_vtg failed rc=%d\n", rc);
			return rc;
		}
	
       pdata->akm_io_vdd = regulator_get(&client->dev, "SENSOR_IO_VDD");
	if (IS_ERR(pdata->akm_io_vdd)) {
		rc = PTR_ERR(pdata->akm_io_vdd);
	compass_ERR("[COMPASS]Regulator get failed apds990x_io_vdd rc=%d\n", rc);
		return rc;
	}

	return 0;
}
static int  akm_regulator_enable(struct akm09911_platform_data *pdata)
{
int rc = 1;
rc = regulator_enable(pdata->akm_vdd);
	if (rc) {
		compass_INFO("[COMPASS]Regulator akm_vdd enable failed rc=%d\n", rc);
		return rc;
		}

	rc = regulator_enable(pdata->akm_io_vdd);
	if (rc) {

		compass_INFO("[COMPASS]Regulator akm_io_vdd enable failed rc=%d\n", rc);
		return rc;
	}

}
static int  akm_regulator_disable(struct akm09911_platform_data *pdata)
{
	int rc = 1;
      rc = regulator_disable(pdata->akm_vdd);
	if (rc) {

		compass_INFO("[COMPASS]Regulator akm_vdd disable failed rc=%d\n", rc);
		return rc;
		}

	rc = regulator_disable(pdata->akm_io_vdd);
	if (rc) {

		compass_INFO("[COMPASS]Regulator akm_io_vdd disable failed rc=%d\n", rc);
		return rc;
	}

}

#endif

/***** akm miscdevice functions *************************************/
static int AKECS_Set_CNTL(
	struct akm_compass_data *akm,
	uint8_t mode)
{
	uint8_t buffer[2];
	int err;

	/***** lock *****/
	mutex_lock(&akm->sensor_mutex);
	/* Busy check */
	if (akm->is_busy > 0) {
		compass_ERR("[COMPASS]%s: device is busy.", __func__);
		err = -EBUSY;
	} else {
		/* Set measure mode */
		buffer[0] = AKM_REG_MODE;
		buffer[1] = mode;
		err = akm_i2c_txdata(akm->i2c, buffer, 2);
		if (err < 0) {
			compass_ERR("[COMPASS]%s: Can not set CNTL.", __func__);
		} else {
			compass_FLOW("[COMPASS]Mode is set to (%d).", mode);
			/* Set flag */
			akm->is_busy = 1;
			atomic_set(&akm->drdy, 0);
			/* wait at least 100us after changing mode */
			udelay(100);
		}
	}

	mutex_unlock(&akm->sensor_mutex);
	/***** unlock *****/

	return err;
}

static int AKECS_Set_PowerDown(
	struct akm_compass_data *akm)
{
	uint8_t buffer[2];
	int err;

	/***** lock *****/
	mutex_lock(&akm->sensor_mutex);
	/* Set powerdown mode */
	buffer[0] = AKM_REG_MODE;
	buffer[1] = AKM_MODE_POWERDOWN;
	err = akm_i2c_txdata(akm->i2c, buffer, 2);
	if (err < 0) {
		compass_ERR("[COMPASS]%s: Can not set to powerdown mode.", __func__);
	} else {
		compass_FLOW("[COMPASS]Powerdown mode is set.");
		/* wait at least 100us after changing mode */
		udelay(100);
	}
	/* Clear status */
	akm->is_busy = 0;
	atomic_set(&akm->drdy, 0);

	mutex_unlock(&akm->sensor_mutex);
	/***** unlock *****/

	return err;
}

static int AKECS_Reset(
	struct akm_compass_data *akm,
	int hard)
{
	int err;
#if AKM_HAS_RESET
	uint8_t buffer[2];

	/***** lock *****/
	mutex_lock(&akm->sensor_mutex);

	if (hard != 0) {
		gpio_set_value(akm->gpio_rstn, 0);
		udelay(5);
		gpio_set_value(akm->gpio_rstn, 1);
		/* No error is returned */
		err = 0;
	} else {
		buffer[0] = AKM_REG_RESET;
		buffer[1] = AKM_RESET_DATA;
		err = akm_i2c_txdata(akm->i2c, buffer, 2);
		if (err < 0) {
			compass_ERR("[COMPASS]%s: Can not set SRST bit.", __func__);
		}
		else
		{
			compass_FLOW("[COMPASS]Soft reset is done.");
		}
	}
	/* Device will be accessible 100 us after */
	udelay(100);
	/* Clear status */
	akm->is_busy = 0;
	atomic_set(&akm->drdy, 0);

	mutex_unlock(&akm->sensor_mutex);
	/***** unlock *****/

#else
	err = AKECS_Set_PowerDown(akm);
#endif

	return err;
}

static int AKECS_SetMode(
	struct akm_compass_data *akm,
	uint8_t mode)
{
	int err;
	switch (mode & 0x1F) {
	case AKM_MODE_SNG_MEASURE:
	case AKM_MODE_SELF_TEST:
	case AKM_MODE_FUSE_ACCESS:
		err = AKECS_Set_CNTL(akm, mode);
		break;
	case AKM_MODE_POWERDOWN:
		err = AKECS_Set_PowerDown(akm);
		break;
	default:
		compass_ERR("[COMPASS]%s: Unknown mode(%d).", __func__, mode);
		return -EINVAL;
	}

	return err;
}
static void AKECS_SetYPR(
	struct akm_compass_data *akm,
	int *rbuf)
{
	uint32_t ready;

	compass_FLOW("[COMPASS]%s: flag =0x%X", __func__, rbuf[0]);
	compass_FLOW("[COMPASS] Acc [LSB]  : %6d,%6d,%6d stat=%d",rbuf[1], rbuf[2], rbuf[3], rbuf[4]);
	compass_FLOW("[COMPASS] Geo [LSB]   : %6d,%6d,%6d stat=%d",
		rbuf[5], rbuf[6], rbuf[7], rbuf[8]);
	compass_FLOW("[COMPASS]  Gyro[LSB]   : %6d,%6d,%6d stat=%d",
		rbuf[9], rbuf[10], rbuf[11], rbuf[12]);
	compass_FLOW("[COMPASS]  Orientation : %6d,%6d,%6d",
		rbuf[13], rbuf[14], rbuf[15]);
	compass_FLOW("[COMPASS]  Gravity     : %6d,%6d,%6d",
		rbuf[16], rbuf[17], rbuf[18]);
	compass_FLOW("[COMPASS]  Linear Acc  : %6d,%6d,%6d",
		rbuf[19], rbuf[20], rbuf[21]);
	compass_FLOW("[COMPASS]  Rotation V  : %6d,%6d,%6d,%6d",
		rbuf[22], rbuf[23], rbuf[24], rbuf[25]);
	/* No events are reported */
	if (!rbuf[0]) {
		compass_FLOW("[COMPASS]Don't waste a time.");
		return;
	}

	mutex_lock(&akm->val_mutex);
	ready = (akm->enable_flag & (uint32_t)rbuf[0]);
	mutex_unlock(&akm->val_mutex);

	/* Report acceleration sensor information */
	if (ready & ACC_DATA_READY) {
		input_report_abs(akm->input, AKM_EVABS_ACC_X, rbuf[1]);
		input_report_abs(akm->input, AKM_EVABS_ACC_Y, rbuf[2]);
		input_report_abs(akm->input, AKM_EVABS_ACC_Z, rbuf[3]);
		input_report_abs(akm->input, AKM_EVABS_ACC_ST, rbuf[4]);
	}
	/* Report magnetic vector information */
	if (ready & MAG_DATA_READY) {
		input_report_abs(akm->input, AKM_EVABS_MAG_X, rbuf[5]);
		input_report_abs(akm->input, AKM_EVABS_MAG_Y, rbuf[6]);
		input_report_abs(akm->input, AKM_EVABS_MAG_Z, rbuf[7]);
		input_report_abs(akm->input, AKM_EVABS_MAG_ST, rbuf[8]);
		if(DT_tset)
		{
			compass_data_count++;
		}
	}
	/* Report fusion sensor information */
	if (ready & FUSION_DATA_READY) {
		/* Gyroscope sensor */
		input_report_abs(akm->input, AKM_EVABS_GYR_X, rbuf[9]);
		input_report_abs(akm->input, AKM_EVABS_GYR_Y, rbuf[10]);
		input_report_abs(akm->input, AKM_EVABS_GYR_Z, rbuf[11]);
		input_report_abs(akm->input, AKM_EVABS_GYR_ST, rbuf[12]);
		/* Orientation */
		input_report_abs(akm->input, AKM_EVABS_ORI_Y, rbuf[13]);
		input_report_abs(akm->input, AKM_EVABS_ORI_P, rbuf[14]);
		input_report_abs(akm->input, AKM_EVABS_ORI_R, rbuf[15]);
		input_report_abs(akm->input, AKM_EVABS_ORI_ST, rbuf[8]);

		/* Gravity */
		input_report_abs(akm->input, AKM_EVABS_GRV_X, rbuf[16]);
		input_report_abs(akm->input, AKM_EVABS_GRV_Y, rbuf[17]);
		input_report_abs(akm->input, AKM_EVABS_GRV_Z, rbuf[18]);
		/* Linear Acceleration */
		input_report_abs(akm->input, AKM_EVABS_LAC_X, rbuf[19]);
		input_report_abs(akm->input, AKM_EVABS_LAC_Y, rbuf[20]);
		input_report_abs(akm->input, AKM_EVABS_LAC_Z, rbuf[21]);
		/* Geomagnetic Rotation Vector */
		input_report_abs(akm->input, AKM_EVABS_GEORV_X, rbuf[22]);
		input_report_abs(akm->input, AKM_EVABS_GEORV_Y, rbuf[23]);
		input_report_abs(akm->input, AKM_EVABS_GEORV_Z, rbuf[24]);
		input_report_abs(akm->input, AKM_EVABS_GEORV_W, rbuf[25]);
		input_report_abs(akm->input, AKM_EVABS_GEORV_ST, rbuf[26]);
		if(DT_tset)
		{
			printk("enter FUSION_DATA_READY Gyro_data_count add 1.\n");
			Gyro_data_count++;
		}
	}
	/* Report uncalibrated magnetic vector information */
	if (ready & MAGUC_DATA_READY) {
		/* Magnetic field uncalibrated */
		input_report_abs(akm->input, AKM_EVABS_MUC_X, rbuf[27]);
		input_report_abs(akm->input, AKM_EVABS_MUC_Y, rbuf[28]);
		input_report_abs(akm->input, AKM_EVABS_MUC_Z, rbuf[29]);
		/* Bias of magnetic field uncalibrated */
		input_report_abs(akm->input, AKM_EVABS_MUB_X, rbuf[30]);
		input_report_abs(akm->input, AKM_EVABS_MUB_Y, rbuf[31]);
		input_report_abs(akm->input, AKM_EVABS_MUB_Z, rbuf[32]);
	}

	input_sync(akm->input);
}
/* This function will block a process until the latest measurement
 * data is available.
 */
static int AKECS_GetData(
	struct akm_compass_data *akm,
	uint8_t *rbuf,
	int size)
{
	int err;
	/* Block! */
	err = wait_event_interruptible_timeout(
			akm->drdy_wq,
			atomic_read(&akm->drdy),
			msecs_to_jiffies(AKM_DRDY_TIMEOUT_MS));

	if (err < 0) {
		compass_ERR("[COMPASS]%s: wait_event failed (%d).", __func__, err);
		return err;
	}
	if (!atomic_read(&akm->drdy)) {
		compass_ERR("[COMPASS]%s: DRDY is not set.", __func__);
		return -ENODATA;
	}

	/***** lock *****/
	mutex_lock(&akm->sensor_mutex);

	memcpy(rbuf, akm->sense_data, size);
	atomic_set(&akm->drdy, 0);

	mutex_unlock(&akm->sensor_mutex);
	/***** unlock *****/

	return 0;
}

static int AKECS_GetData_Poll(
	struct akm_compass_data *akm,
	uint8_t *rbuf,
	int size)
{
	uint8_t buffer[AKM_SENSOR_DATA_SIZE];
	int err;

	/* Read status */
	buffer[0] = AKM_REG_STATUS;
	err = akm_i2c_rxdata(akm->i2c, buffer, 1);
	if (err < 0) {
		compass_ERR("[COMPASS]%s failed.", __func__);
		return err;
	}

	/* Check ST bit */
	if (!(AKM_DRDY_IS_HIGH(buffer[0])))
	{
		return -EAGAIN;
	}

	/* Read rest data */
	buffer[1] = AKM_REG_STATUS + 1;
	err = akm_i2c_rxdata(akm->i2c, &(buffer[1]), AKM_SENSOR_DATA_SIZE-1);
	if (err < 0) {
		compass_ERR("[COMPASS]%s failed.", __func__);
		return err;
	}

	memcpy(rbuf, buffer, size);
	atomic_set(&akm->drdy, 0);

	/***** lock *****/
	mutex_lock(&akm->sensor_mutex);
	akm->is_busy = 0;
	mutex_unlock(&akm->sensor_mutex);
	/***** unlock *****/

	return 0;
}

static int AKECS_GetOpenStatus(
	struct akm_compass_data *akm)
{
	return wait_event_interruptible(
			akm->open_wq, (atomic_read(&akm->active) > 0));
}

static int AKECS_GetCloseStatus(
	struct akm_compass_data *akm)
{
	return wait_event_interruptible(
			akm->open_wq, (atomic_read(&akm->active) <= 0));
}

static int AKECS_Open(struct inode *inode, struct file *file)
{
	file->private_data = s_akm;
	return nonseekable_open(inode, file);
}

static int AKECS_Release(struct inode *inode, struct file *file)
{
	return 0;
}

static long
AKECS_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	void __user *argp = (void __user *)arg;
	struct akm_compass_data *akm = file->private_data;

	/* NOTE: In this function the size of "char" should be 1-byte. */
	uint8_t i2c_buf[AKM_RWBUF_SIZE];		/* for READ/WRITE */
	uint8_t dat_buf[AKM_SENSOR_DATA_SIZE];/* for GET_DATA */
	int32_t ypr_buf[AKM_YPR_DATA_SIZE];		/* for SET_YPR */
	int64_t delay[AKM_NUM_SENSORS];	/* for GET_DELAY */
	int16_t acc_buf[3];	/* for GET_ACCEL */
	uint8_t mode;			/* for SET_MODE*/
	int status;			/* for OPEN/CLOSE_STATUS */
	int ret = 0;		/* Return value. */

	memset(i2c_buf, 0, AKM_RWBUF_SIZE);

	switch (cmd) {
	case ECS_IOCTL_READ:
	case ECS_IOCTL_WRITE:
		if (argp == NULL) {
			compass_ERR("[COMPASS]invalid argument.");
			return -EINVAL;
		}
		if (copy_from_user(&i2c_buf, argp, sizeof(i2c_buf))) {
			compass_ERR("[COMPASS]copy_from_user failed.");
			return -EFAULT;
		}
		break;
	case ECS_IOCTL_SET_MODE:
		if (argp == NULL) {
			compass_ERR("[COMPASS]invalid argument.");
			return -EINVAL;
		}
		if (copy_from_user(&mode, argp, sizeof(mode))) {
			compass_ERR("[COMPASS]copy_from_user failed.");
			return -EFAULT;
		}
		break;
	case ECS_IOCTL_SET_YPR:
		if (argp == NULL) {
			compass_ERR("[COMPASS]invalid argument.");
			return -EINVAL;
		}
		if (copy_from_user(&ypr_buf, argp, sizeof(ypr_buf))) {
			compass_ERR("[COMPASS]copy_from_user failed.");
			return -EFAULT;
		}
		break;
	case ECS_IOCTL_GET_INFO:
	case ECS_IOCTL_GET_CONF:
	case ECS_IOCTL_GET_DATA:
	case ECS_IOCTL_GET_OPEN_STATUS:
	case ECS_IOCTL_GET_CLOSE_STATUS:
	case ECS_IOCTL_GET_DELAY:
	case ECS_IOCTL_GET_LAYOUT:
	case ECS_IOCTL_GET_ACCEL:
		/* Check buffer pointer for writing a data later. */
		if (argp == NULL) {
			compass_ERR("[COMPASS]invalid argument.");
			return -EINVAL;
		}
		break;
	default:
		break;
	}

	switch (cmd) {
	case ECS_IOCTL_READ:
		compass_FLOW("[COMPASS]IOCTL_READ called.");
		if ((i2c_buf[0] < 1) || (i2c_buf[0] > (AKM_RWBUF_SIZE-1))) {
			compass_ERR("[COMPASS]invalid argument.");
			return -EINVAL;
		}
		ret = akm_i2c_rxdata(akm->i2c, &i2c_buf[1], i2c_buf[0]);
		if (ret < 0)
			return ret;
		break;
	case ECS_IOCTL_WRITE:

		compass_FLOW("[COMPASS]IOCTL_WRITE called.");
		if ((i2c_buf[0] < 2) || (i2c_buf[0] > (AKM_RWBUF_SIZE-1))) {
		compass_ERR("[COMPASS]invalid argument.");
			return -EINVAL;
		}
		ret = akm_i2c_txdata(akm->i2c, &i2c_buf[1], i2c_buf[0]);
		if (ret < 0)
			return ret;
		break;
	case ECS_IOCTL_RESET:

		compass_FLOW("[COMPASS]IOCTL_RESET called.");
		ret = AKECS_Reset(akm, akm->gpio_rstn);
		if (ret < 0)
			return ret;
		break;
	case ECS_IOCTL_SET_MODE:

		compass_FLOW("[COMPASS]IOCTL_SET_MODE called.");
		ret = AKECS_SetMode(akm, mode);
		if (ret < 0)
			return ret;
		break;
	case ECS_IOCTL_SET_YPR:

		compass_FLOW("[COMPASS]IOCTL_SET_YPR called.");
		AKECS_SetYPR(akm, ypr_buf);
		break;
	case ECS_IOCTL_GET_DATA:

		compass_FLOW("[COMPASS]IOCTL_GET_DATA called.");
		if (akm->irq)
			ret = AKECS_GetData(akm, dat_buf, AKM_SENSOR_DATA_SIZE);
		else
			ret = AKECS_GetData_Poll(
					akm, dat_buf, AKM_SENSOR_DATA_SIZE);

		if (ret < 0)
			return ret;
		break;
	case ECS_IOCTL_GET_OPEN_STATUS:

		compass_FLOW("[COMPASS]IOCTL_GET_OPEN_STATUS called.");
		ret = AKECS_GetOpenStatus(akm);
		if (ret < 0) {
			compass_ERR("[COMPASS]Get Open returns error (%d).", ret);
			return ret;
		}
		break;
	case ECS_IOCTL_GET_CLOSE_STATUS:

		compass_FLOW("[COMPASS]IOCTL_GET_CLOSE_STATUS called.");
		ret = AKECS_GetCloseStatus(akm);
		if (ret < 0) {
			compass_ERR("[COMPASS]Get Close returns error (%d).", ret);
			return ret;
		}
		break;
	case ECS_IOCTL_GET_DELAY:

		compass_FLOW("[COMPASS]IOCTL_GET_DELAY called.");
		mutex_lock(&akm->val_mutex);
		delay[ACC_DATA_FLAG] = ((akm->enable_flag & ACC_DATA_READY) ?
				akm->delay[ACC_DATA_FLAG] : -1);
		delay[MAG_DATA_FLAG] = ((akm->enable_flag & MAG_DATA_READY) ?
				akm->delay[MAG_DATA_FLAG] : -1);
		delay[MAGUC_DATA_FLAG] = ((akm->enable_flag & MAGUC_DATA_READY) ?
				akm->delay[MAGUC_DATA_FLAG] : -1);
		delay[FUSION_DATA_FLAG] = ((akm->enable_flag & FUSION_DATA_READY) ?
				akm->delay[FUSION_DATA_FLAG] : -1);
		mutex_unlock(&akm->val_mutex);
		break;
	case ECS_IOCTL_GET_INFO:
		compass_FLOW("[COMPASS]IOCTL_GET_INFO called.");
		break;
	case ECS_IOCTL_GET_CONF:

		compass_FLOW("[COMPASS]IOCTL_GET_CONF called.");
		break;
	case ECS_IOCTL_GET_LAYOUT:

		compass_FLOW("[COMPASS]IOCTL_GET_LAYOUT called.");
		break;
	case ECS_IOCTL_GET_ACCEL:

		compass_FLOW("[COMPASS]IOCTL_GET_ACCEL called.");
		mutex_lock(&akm->accel_mutex);
		acc_buf[0] = akm->accel_data[0];
		acc_buf[1] = akm->accel_data[1];
		acc_buf[2] = akm->accel_data[2];
		mutex_unlock(&akm->accel_mutex);
		break;
	default:
		return -ENOTTY;
	}

	switch (cmd) {
	case ECS_IOCTL_READ:
		/* +1  is for the first byte */
		if (copy_to_user(argp, &i2c_buf, i2c_buf[0]+1)) {

			compass_ERR("[COMPASS]copy_to_user failed.");
			return -EFAULT;
		}
		break;
	case ECS_IOCTL_GET_INFO:
		if (copy_to_user(argp, &akm->sense_info,
					sizeof(akm->sense_info))) {

			compass_ERR("[COMPASS]copy_to_user failed.");
			return -EFAULT;
		}
		break;
	case ECS_IOCTL_GET_CONF:
		if (copy_to_user(argp, &akm->sense_conf,
					sizeof(akm->sense_conf))) {
			compass_ERR("[COMPASS]copy_to_user failed.");
			return -EFAULT;
		}
		break;
	case ECS_IOCTL_GET_DATA:
		if (copy_to_user(argp, &dat_buf, sizeof(dat_buf))) {

			compass_ERR("[COMPASS]copy_to_user failed.");
			return -EFAULT;
		}
		break;
	case ECS_IOCTL_GET_OPEN_STATUS:
	case ECS_IOCTL_GET_CLOSE_STATUS:
		status = atomic_read(&akm->active);
		if (copy_to_user(argp, &status, sizeof(status))) {
			compass_ERR("[COMPASS]copy_to_user failed.");
			return -EFAULT;
		}
		break;
	case ECS_IOCTL_GET_DELAY:
		if (copy_to_user(argp, &delay, sizeof(delay))) {
			compass_ERR("[COMPASS]copy_to_user failed.");
			return -EFAULT;
		}
		break;
	case ECS_IOCTL_GET_LAYOUT:
		if (copy_to_user(argp, &akm->layout, sizeof(akm->layout))) {
			compass_ERR("[COMPASS]copy_to_user failed.");
			return -EFAULT;
		}
		break;
	case ECS_IOCTL_GET_ACCEL:
		if (copy_to_user(argp, &acc_buf, sizeof(acc_buf))) {
			compass_ERR("[COMPASS]copy_to_user failed.");
			return -EFAULT;
		}
		break;
	default:
		break;
	}

	return 0;
}

static const struct file_operations AKECS_fops = {
	.owner = THIS_MODULE,
	.open = AKECS_Open,
	.release = AKECS_Release,
	.unlocked_ioctl = AKECS_ioctl,
};

static struct miscdevice akm_compass_dev = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = AKM_MISCDEV_NAME,
	.fops = &AKECS_fops,
};

/***** akm sysfs functions ******************************************/
static int create_device_attributes(
	struct device *dev,
	struct device_attribute *attrs)
{
	int i;
	int err = 0;
	for (i = 0 ; NULL != attrs[i].attr.name ; ++i) {
		err = device_create_file(dev, &attrs[i]);
		if (err)
			break;
	}

	if (err) {
		for (--i; i >= 0 ; --i)
			device_remove_file(dev, &attrs[i]);
	}

	return err;
}

static void remove_device_attributes(
	struct device *dev,
	struct device_attribute *attrs)
{
	int i;
	for (i = 0 ; NULL != attrs[i].attr.name ; ++i)
		device_remove_file(dev, &attrs[i]);
}

static int create_device_binary_attributes(
	struct kobject *kobj,
	struct bin_attribute *attrs)
{
	int i;
	int err = 0;
	err = 0;

	for (i = 0 ; NULL != attrs[i].attr.name ; ++i) {
		err = sysfs_create_bin_file(kobj, &attrs[i]);
		if (0 != err)
			break;
	}

	if (0 != err) {
		for (--i; i >= 0 ; --i)
			sysfs_remove_bin_file(kobj, &attrs[i]);
	}

	return err;
}

static void remove_device_binary_attributes(
	struct kobject *kobj,
	struct bin_attribute *attrs)
{
	int i;

	for (i = 0 ; NULL != attrs[i].attr.name ; ++i)
		sysfs_remove_bin_file(kobj, &attrs[i]);
}

/*********************************************************************
 *
 * SysFS attribute functions
 *
 * directory : /sys/class/compass/akmXXXX/
 * files :
 *  - enable_acc    [rw] [t] : enable flag for accelerometer
 *  - enable_mag    [rw] [t] : enable flag for magnetometer
 *  - enable_maguc  [rw] [t] : enable flag for uncalibrated magnetometer
 *  - enable_fusion [rw] [t] : enable flag for fusion sensor
 *  - delay_acc     [rw] [t] : delay in nanosecond for accelerometer
 *  - delay_mag     [rw] [t] : delay in nanosecond for magnetometer
 *  - delay_maguc   [rw] [t] : delay in nanosecond for uncalibrated magnetometer
 *  - delay_fusion  [rw] [t] : delay in nanosecond for fusion sensor
 *
 * debug :
 *  - mode       [w]  [t] : E-Compass mode
 *  - bdata      [r]  [t] : buffered raw data
 *  - asa        [r]  [t] : FUSEROM data
 *  - regs       [r]  [t] : read all registers
 *
 * [b] = binary format
 * [t] = text format
 */

/***** sysfs enable *************************************************/
static void akm_compass_sysfs_update_status(
	struct akm_compass_data *akm)
{
	uint32_t en;
	mutex_lock(&akm->val_mutex);
	en = akm->enable_flag;
	mutex_unlock(&akm->val_mutex);
	if (en == 0) {
		if (atomic_cmpxchg(&akm->active, 1, 0) == 1) {
			wake_up(&akm->open_wq);
			compass_FLOW("[COMPASS]Deactivated");
		}
	} else {
		if (atomic_cmpxchg(&akm->active, 0, 1) == 0) {
			wake_up(&akm->open_wq);
			compass_FLOW("[COMPASS]Activated");
		}
	}
	compass_FLOW("[COMPASS]Status updated: enable=0x%X, active=%d",en, atomic_read(&akm->active));
	
}

static ssize_t akm_compass_sysfs_enable_show(
	struct akm_compass_data *akm, char *buf, int pos)
{
	int flag;
	mutex_lock(&akm->val_mutex);
	flag = ((akm->enable_flag >> pos) & 1);
	mutex_unlock(&akm->val_mutex);

	return scnprintf(buf, PAGE_SIZE, "%d\n", flag);
}

static ssize_t akm_compass_sysfs_enable_store(
	struct akm_compass_data *akm, char const *buf, size_t count, int pos)
{
	long en = 0;
	if (NULL == buf)
		return -EINVAL;

	if (0 == count)
		return 0;

	if (strict_strtol(buf, AKM_BASE_NUM, &en))
		return -EINVAL;
	if(DT_tset)
	{
		
		if(2==en)
		{
			en=1;
		}
		else if(3==en)
		{
			en=0;
		}
		else
		{
			return count;
		}
	}

	en = en ? 1 : 0;

	mutex_lock(&akm->val_mutex);
	akm->enable_flag &= ~(1<<pos);
	akm->enable_flag |= ((uint32_t)(en))<<pos;
	mutex_unlock(&akm->val_mutex);

	akm_compass_sysfs_update_status(akm);

	return count;
}

/***** Acceleration ***/
static ssize_t akm_enable_acc_show(
	struct device *dev, struct device_attribute *attr, char *buf)
{
	return akm_compass_sysfs_enable_show(
		dev_get_drvdata(dev), buf, ACC_DATA_FLAG);
}
static ssize_t akm_enable_acc_store(
	struct device *dev, struct device_attribute *attr,
	char const *buf, size_t count)
{
	return akm_compass_sysfs_enable_store(
		dev_get_drvdata(dev), buf, count, ACC_DATA_FLAG);
}

/***** Magnetic field ***/
static ssize_t akm_enable_mag_show(
	struct device *dev, struct device_attribute *attr, char *buf)
{
	return akm_compass_sysfs_enable_show(
		dev_get_drvdata(dev), buf, MAG_DATA_FLAG);
}
static ssize_t akm_enable_mag_store(
	struct device *dev, struct device_attribute *attr,
	char const *buf, size_t count)
{
	return akm_compass_sysfs_enable_store(
		dev_get_drvdata(dev), buf, count, MAG_DATA_FLAG);
}

/***** Uncalibrated Magnetic field ***/
static ssize_t akm_enable_maguc_show(
	struct device *dev, struct device_attribute *attr, char *buf)
{
	return akm_compass_sysfs_enable_show(
		dev_get_drvdata(dev), buf, MAGUC_DATA_FLAG);
}
static ssize_t akm_enable_maguc_store(
	struct device *dev, struct device_attribute *attr,
	char const *buf, size_t count)
{
	return akm_compass_sysfs_enable_store(
		dev_get_drvdata(dev), buf, count, MAGUC_DATA_FLAG);
}

/***** Fusion ***/
static ssize_t akm_enable_fusion_show(
	struct device *dev, struct device_attribute *attr, char *buf)
{
	return akm_compass_sysfs_enable_show(
		dev_get_drvdata(dev), buf, FUSION_DATA_FLAG);
}
static ssize_t akm_enable_fusion_store(
	struct device *dev, struct device_attribute *attr,
	char const *buf, size_t count)
{
	return akm_compass_sysfs_enable_store(
		dev_get_drvdata(dev), buf, count, FUSION_DATA_FLAG);
}
/***** sysfs delay **************************************************/
static ssize_t akm_compass_sysfs_delay_show(
	struct akm_compass_data *akm, char *buf, int pos)
{
	int64_t val;
	mutex_lock(&akm->val_mutex);
	val = akm->delay[pos];
	mutex_unlock(&akm->val_mutex);

	return scnprintf(buf, PAGE_SIZE, "%lld\n", val);
}

static ssize_t akm_compass_sysfs_delay_store(
	struct akm_compass_data *akm, char const *buf, size_t count, int pos)
{
	long long val = 0;
	if (NULL == buf)
		return -EINVAL;

	if (0 == count)
		return 0;

	if (strict_strtoll(buf, AKM_BASE_NUM, &val))
		return -EINVAL;
	if(DT_tset)
	{
		if(10!=val)
		{
			return count;
		}
	}
	mutex_lock(&akm->val_mutex);
	akm->delay[pos] = val;
	mutex_unlock(&akm->val_mutex);

	return count;
}

/***** Accelerometer ***/
static ssize_t akm_delay_acc_show(
	struct device *dev, struct device_attribute *attr, char *buf)
{
	return akm_compass_sysfs_delay_show(
		dev_get_drvdata(dev), buf, ACC_DATA_FLAG);
}
static ssize_t akm_delay_acc_store(
	struct device *dev, struct device_attribute *attr,
	char const *buf, size_t count)
{
	return akm_compass_sysfs_delay_store(
		dev_get_drvdata(dev), buf, count, ACC_DATA_FLAG);
}

/***** Magnetic field ***/
static ssize_t akm_delay_mag_show(
	struct device *dev, struct device_attribute *attr, char *buf)
{
	return akm_compass_sysfs_delay_show(
		dev_get_drvdata(dev), buf, MAG_DATA_FLAG);
}
static ssize_t akm_delay_mag_store(
	struct device *dev, struct device_attribute *attr,
	char const *buf, size_t count)
{
	return akm_compass_sysfs_delay_store(
		dev_get_drvdata(dev), buf, count, MAG_DATA_FLAG);
}

/***** Uncalibrated Magnetic field ***/
static ssize_t akm_delay_maguc_show(
	struct device *dev, struct device_attribute *attr, char *buf)
{
	return akm_compass_sysfs_delay_show(
		dev_get_drvdata(dev), buf, MAGUC_DATA_FLAG);
}
static ssize_t akm_delay_maguc_store(
	struct device *dev, struct device_attribute *attr,
	char const *buf, size_t count)
{
	return akm_compass_sysfs_delay_store(
		dev_get_drvdata(dev), buf, count, MAGUC_DATA_FLAG);
}

/***** Fusion ***/
static ssize_t akm_delay_fusion_show(
	struct device *dev, struct device_attribute *attr, char *buf)
{
	return akm_compass_sysfs_delay_show(
		dev_get_drvdata(dev), buf, FUSION_DATA_FLAG);
}
static ssize_t akm_delay_fusion_store(
	struct device *dev, struct device_attribute *attr,
	char const *buf, size_t count)
{
	return akm_compass_sysfs_delay_store(
		dev_get_drvdata(dev), buf, count, FUSION_DATA_FLAG);
}
/***** accel (binary) ***/
static ssize_t akm_bin_accel_write(
	struct file *file,
	struct kobject *kobj,
	struct bin_attribute *attr,
		char *buf,
		loff_t pos,
		size_t size)
{
	struct device *dev = container_of(kobj, struct device, kobj);
	struct akm_compass_data *akm = dev_get_drvdata(dev);
	int16_t *accel_data;
	if (size == 0)
		return 0;

	accel_data = (int16_t *)buf;

	mutex_lock(&akm->accel_mutex);
	akm->accel_data[0] = accel_data[0];
	akm->accel_data[1] = accel_data[1];
	akm->accel_data[2] = accel_data[2];
	mutex_unlock(&akm->accel_mutex);

	compass_FLOW("[COMPASS]accel:%d,%d,%d\n",
			accel_data[0], accel_data[1], accel_data[2]);

	return size;
}
static ssize_t akm_set_cal_show(
	struct device *dev, struct device_attribute *attr, char *buf)
{
	int err = 0;

	compass_INFO("[COMPASS]akm_set_cal_show,calibration_value=%d\n",calibration_value);
	err = snprintf(buf, PAGE_SIZE, "%d\n", calibration_value);
	return err;
}
static ssize_t akm_set_cal_store(
	struct device *dev, struct device_attribute *attr,
	char const *buf, size_t count)
{
	calibration_value =  (int)(buf[0] - '0');

	compass_INFO("[COMPASS]akm_set_cal_store,calibration_value=%d\n",calibration_value);
	return 0;
}
#if AKM_DEBUG_IF
static ssize_t akm_sysfs_mode_store(
	struct device *dev, struct device_attribute *attr,
	char const *buf, size_t count)
{
	struct akm_compass_data *akm = dev_get_drvdata(dev);
	long mode = 0;
	if (NULL == buf)
		return -EINVAL;

	if (0 == count)
		return 0;

	if (strict_strtol(buf, AKM_BASE_NUM, &mode))
		return -EINVAL;

	if (AKECS_SetMode(akm, (uint8_t)mode) < 0)
		return -EINVAL;

	return 1;
}

static ssize_t akm_sysfs_mode_show(
	struct device *dev, struct device_attribute *attr, char *buf)
{
	return 0;
}

static ssize_t akm09911_sysfs_bdata_show(
	struct device *dev, struct device_attribute *attr, char *buf)
{
	/* AK09911 specific function */
	struct akm_compass_data *akm = dev_get_drvdata(dev);
	uint8_t rbuf[AKM_SENSOR_DATA_SIZE];
	mutex_lock(&akm->sensor_mutex);
	memcpy(&rbuf, akm->sense_data, sizeof(rbuf));
	mutex_unlock(&akm->sensor_mutex);

	return scnprintf(buf, PAGE_SIZE,
		"0x%02X,0x%02X,0x%02X,0x%02X,"
		"0x%02X,0x%02X,0x%02X,0x%02X,0x%02X\n",
		rbuf[0], rbuf[1], rbuf[2], rbuf[3],
		rbuf[4], rbuf[5], rbuf[6], rbuf[7], rbuf[8]);
}

static ssize_t akm09911_sysfs_bdata_store(
	struct device *dev, struct device_attribute *attr,
	char const *buf, size_t count)
{
	return 0;
}

static ssize_t akm09911_sysfs_asa_show(
	struct device *dev, struct device_attribute *attr, char *buf)
{
	/* AK09911 specific function */
	struct akm_compass_data *akm = dev_get_drvdata(dev);
	int err;
	uint8_t asa[3];
	err = AKECS_SetMode(akm, AK09911_MODE_FUSE_ACCESS);
	if (err < 0)
		return err;

	asa[0] = AK09911_FUSE_ASAX;
	err = akm_i2c_rxdata(akm->i2c, asa, 3);
	if (err < 0)
		return err;

	err = AKECS_SetMode(akm, AK09911_MODE_POWERDOWN);
	if (err < 0)
		return err;

	return scnprintf(buf, PAGE_SIZE,
			"0x%02X,0x%02X,0x%02X\n", asa[0], asa[1], asa[2]);
}

static ssize_t akm09911_sysfs_asa_store(
	struct device *dev, struct device_attribute *attr,
	char const *buf, size_t count)
{
	return 0;
}

static ssize_t akm09911_sysfs_regs_show(
	struct device *dev, struct device_attribute *attr, char *buf)
{
	/* AK09911 specific function */
	/* The total number of registers depends on the device. */
	struct akm_compass_data *akm = dev_get_drvdata(dev);
	int err;
	uint8_t regs[13];
	int sz;
	int n;
	char *cur;
	/* This function does not lock mutex obj */
	regs[0] = AK09911_REG_WIA1;
	err = akm_i2c_rxdata(akm->i2c, regs, 13);
	if (err < 0)
		return err;

	cur = buf;
	sz = snprintf(cur, PAGE_SIZE, "(HEX):");
	cur += sz;
	for (n = 0; n < 13; n++) {
		sz = snprintf(cur, 4, "%02X,", regs[n]);
		cur += sz;
	}

	return (ssize_t)(cur - buf);
}

static ssize_t akm09911_sysfs_regs_store(
	struct device *dev, struct device_attribute *attr,
	char const *buf, size_t count)
{
	return 0;
}
#endif


static ssize_t attr_get_accl_info(struct device *dev,
                 struct device_attribute *attr, char *buf)
{
	ssize_t count;
	count = snprintf(buf, PAGE_SIZE, "AKM AKM09911");
	return count;
}

static ssize_t attr_set_accl_info(
	struct device *dev, struct device_attribute *attr,
	char const *buf, size_t count)
{
	return 0;
}
static struct device_attribute akm_compass_attributes[] = {
	__ATTR(enable_acc, 0664, akm_enable_acc_show, akm_enable_acc_store),
	__ATTR(enable_mag, 0664, akm_enable_mag_show, akm_enable_mag_store),
	__ATTR(enable_maguc, 0664, akm_enable_maguc_show, akm_enable_maguc_store),
	__ATTR(enable_fusion, 0664, akm_enable_fusion_show,
			akm_enable_fusion_store),
	__ATTR(delay_acc,  0664, akm_delay_acc_show,  akm_delay_acc_store),
	__ATTR(delay_mag,  0664, akm_delay_mag_show,  akm_delay_mag_store),
	__ATTR(delay_maguc,  0664, akm_delay_maguc_show,  akm_delay_maguc_store),
	__ATTR(delay_fusion, 0664, akm_delay_fusion_show,	akm_delay_fusion_store),
	__ATTR(set_cal,  0664, akm_set_cal_show,  akm_set_cal_store),
	__ATTR(compass_info, 0664, attr_get_accl_info, attr_set_accl_info),
#if AKM_DEBUG_IF
	__ATTR(mode,  0664, akm_sysfs_mode_show, akm_sysfs_mode_store),
	__ATTR(bdata, 0664, akm09911_sysfs_bdata_show, akm09911_sysfs_bdata_store),
	__ATTR(asa,   0664, akm09911_sysfs_asa_show, akm09911_sysfs_asa_store),
	__ATTR(regs,  0664, akm09911_sysfs_regs_show, akm09911_sysfs_regs_store),
#endif
	__ATTR_NULL,
};
#define __BIN_ATTR(name_, mode_, size_, private_, read_, write_) \
	{ \
		.attr    = { .name = __stringify(name_), .mode = mode_ }, \
		.size    = size_, \
		.private = private_, \
		.read    = read_, \
		.write   = write_, \
	}

#define __BIN_ATTR_NULL \
	{ \
		.attr   = { .name = NULL }, \
	}

static struct bin_attribute akm_compass_bin_attributes[] = {
	__BIN_ATTR(accel, 0220, 6, NULL,
				NULL, akm_bin_accel_write),
	__BIN_ATTR_NULL
};

static char const *const device_link_name = "i2c";
static dev_t const akm_compass_device_dev_t = MKDEV(MISC_MAJOR, 240);

static int create_sysfs_interfaces(struct akm_compass_data *akm)
{
	int err;

	if (NULL == akm)
		return -EINVAL;

	err = 0;

	akm->compass = class_create(THIS_MODULE, AKM_SYSCLS_NAME);
	if (IS_ERR(akm->compass)) {
		err = PTR_ERR(akm->compass);
		goto exit_class_create_failed;
	}

	akm->class_dev = device_create(
						akm->compass,
						NULL,
						akm_compass_device_dev_t,
						akm,
						AKM_SYSDEV_NAME);
	if (IS_ERR(akm->class_dev)) {
		err = PTR_ERR(akm->class_dev);
		goto exit_class_device_create_failed;
	}

	err = sysfs_create_link(
			&akm->class_dev->kobj,
			&akm->i2c->dev.kobj,
			device_link_name);
	if (0 > err)
		goto exit_sysfs_create_link_failed;

	err = create_device_attributes(
			akm->class_dev,
			akm_compass_attributes);
	if (0 > err)
		goto exit_device_attributes_create_failed;

	err = create_device_binary_attributes(
			&akm->class_dev->kobj,
			akm_compass_bin_attributes);
	if (0 > err)
		goto exit_device_binary_attributes_create_failed;

	return err;

exit_device_binary_attributes_create_failed:
	remove_device_attributes(akm->class_dev, akm_compass_attributes);
exit_device_attributes_create_failed:
	sysfs_remove_link(&akm->class_dev->kobj, device_link_name);
exit_sysfs_create_link_failed:
	device_destroy(akm->compass, akm_compass_device_dev_t);
exit_class_device_create_failed:
	akm->class_dev = NULL;
	class_destroy(akm->compass);
exit_class_create_failed:
	akm->compass = NULL;
	return err;
}

static void remove_sysfs_interfaces(struct akm_compass_data *akm)
{
	if (NULL == akm)
		return;

	if (NULL != akm->class_dev) {
		remove_device_binary_attributes(
			&akm->class_dev->kobj,
			akm_compass_bin_attributes);
		remove_device_attributes(
			akm->class_dev,
			akm_compass_attributes);
		sysfs_remove_link(
			&akm->class_dev->kobj,
			device_link_name);
		akm->class_dev = NULL;
	}
	if (NULL != akm->compass) {
		device_destroy(
			akm->compass,
			akm_compass_device_dev_t);
		class_destroy(akm->compass);
		akm->compass = NULL;
	}
}


/***** akm input device functions ***********************************/
static int akm_compass_input_init(
	struct input_dev **input)
{
	int err = 0;
	/* Declare input device */
	*input = input_allocate_device();
	if (!*input)
		return -ENOMEM;

	/* Setup input device */
	set_bit(EV_ABS, (*input)->evbit);
	/* Accelerometer (720 x 16G)*/
	input_set_abs_params(*input, AKM_EVABS_ACC_X,
			-11520, 11520, 0, 0);
	input_set_abs_params(*input, AKM_EVABS_ACC_Y,
			-11520, 11520, 0, 0);
	input_set_abs_params(*input, AKM_EVABS_ACC_Z,
			-11520, 11520, 0, 0);
	input_set_abs_params(*input, AKM_EVABS_ACC_ST,
			0, 3, 0, 0);
	/* Magnetic field (limited to 16bit) */
	input_set_abs_params(*input, AKM_EVABS_MAG_X,
			-32768, 32767, 0, 0);
	input_set_abs_params(*input, AKM_EVABS_MAG_Y,
			-32768, 32767, 0, 0);
	input_set_abs_params(*input, AKM_EVABS_MAG_Z,
			-32768, 32767, 0, 0);
	input_set_abs_params(*input, AKM_EVABS_MAG_ST,
			0, 3, 0, 0);
	/* Gyroscope (2000 deg/sec in Q6 format) */
	input_set_abs_params(*input, AKM_EVABS_GYR_X,
			-128000, 128000, 0, 0);
	input_set_abs_params(*input, AKM_EVABS_GYR_Y,
			-128000, 128000, 0, 0);
	input_set_abs_params(*input, AKM_EVABS_GYR_Z,
			-128000, 128000, 0, 0);
	input_set_abs_params(*input, AKM_EVABS_GYR_ST,
			0, 3, 0, 0);
	/* Orientation (degree in Q6 format) */
	/*  yaw[0,360) pitch[-180,180) roll[-90,90) */
	input_set_abs_params(*input, AKM_EVABS_ORI_Y,
			0, 23040, 0, 0);
	input_set_abs_params(*input, AKM_EVABS_ORI_P,
			-11520, 11520, 0, 0);
	input_set_abs_params(*input, AKM_EVABS_ORI_R,
			-5760, 5760, 0, 0);
	input_set_abs_params(*input, AKM_EVABS_ORI_ST,
			0, 3, 0, 0);
	/* Gravity (720 x 2G) */
	input_set_abs_params(*input, AKM_EVABS_GRV_X,
			-1440, 1440, 0, 0);
	input_set_abs_params(*input, AKM_EVABS_GRV_Y,
			-1440, 1440, 0, 0);
	input_set_abs_params(*input, AKM_EVABS_GRV_Z,
			-1440, 1440, 0, 0);
	/* Linear acceleration (720 x 16G) */
	input_set_abs_params(*input, AKM_EVABS_LAC_X,
			-115200, 115200, 0, 0);
	input_set_abs_params(*input, AKM_EVABS_LAC_Y,
			-115200, 115200, 0, 0);
	input_set_abs_params(*input, AKM_EVABS_LAC_Z,
			-115200, 115200, 0, 0);
	/* Geomagnetic Rotation Vector [-1,+1] in Q14 format */
	input_set_abs_params(*input, AKM_EVABS_GEORV_X,
			-16384, 16384, 0, 0);
	input_set_abs_params(*input, AKM_EVABS_GEORV_Y,
			-16384, 16384, 0, 0);
	input_set_abs_params(*input, AKM_EVABS_GEORV_Z,
			-16384, 16384, 0, 0);
	input_set_abs_params(*input, AKM_EVABS_GEORV_W,
			-16384, 16384, 0, 0);
	/* estimated heading accuracy [0, 180] in Q6 format */
	input_set_abs_params(*input, AKM_EVABS_GEORV_ST,
			0, 11520, 0, 0);
	/* Magnetic field uncalibrated  (limited to 20bit) */
	input_set_abs_params(*input, AKM_EVABS_MUC_X,
			-524288, 524287, 0, 0);
	input_set_abs_params(*input, AKM_EVABS_MUC_Y,
			-524288, 524287, 0, 0);
	input_set_abs_params(*input, AKM_EVABS_MUC_Z,
			-524288, 524287, 0, 0);
	/* Bias of  Magnetic field uncalibrated  (limited to 20bit) */
	input_set_abs_params(*input, AKM_EVABS_MUB_X,
			-524288, 524287, 0, 0);
	input_set_abs_params(*input, AKM_EVABS_MUB_Y,
			-524288, 524287, 0, 0);
	input_set_abs_params(*input, AKM_EVABS_MUB_Z,
			-524288, 524287, 0, 0);

	/* Set name */
	(*input)->name = COMPASS_INPUT_DEV_NAME;

	/* Register */
	err = input_register_device(*input);
	if (err) {
		input_free_device(*input);
		return err;
	}

	return err;
}
/***** akm functions ************************************************/
static irqreturn_t akm_compass_irq(int irq, void *handle)
{
	struct akm_compass_data *akm = handle;
	uint8_t buffer[AKM_SENSOR_DATA_SIZE];
	int err;
	memset(buffer, 0, sizeof(buffer));

	/***** lock *****/
	mutex_lock(&akm->sensor_mutex);

	/* Read whole data */
	buffer[0] = AKM_REG_STATUS;
	err = akm_i2c_rxdata(akm->i2c, buffer, AKM_SENSOR_DATA_SIZE);
	if (err < 0) {
		compass_ERR("[COMPASS]IRQ I2C error.");
		akm->is_busy = 0;
		mutex_unlock(&akm->sensor_mutex);
		/***** unlock *****/

		return IRQ_HANDLED;
	}
	/* Check ST bit */
	if (!(AKM_DRDY_IS_HIGH(buffer[0])))
		goto work_func_none;

	memcpy(akm->sense_data, buffer, AKM_SENSOR_DATA_SIZE);
	akm->is_busy = 0;

	mutex_unlock(&akm->sensor_mutex);
	/***** unlock *****/

	atomic_set(&akm->drdy, 1);
	wake_up(&akm->drdy_wq);

	compass_FLOW("[COMPASS]IRQ handled.");
	return IRQ_HANDLED;

work_func_none:
	mutex_unlock(&akm->sensor_mutex);
	/***** unlock *****/

	compass_FLOW("[COMPASS]IRQ not handled.");
	return IRQ_NONE;
}

static int akm09911_i2c_check_device(
	struct i2c_client *client)
{
	/* AK09911 specific function */
	struct akm_compass_data *akm = i2c_get_clientdata(client);
	int err;

	akm->sense_info[0] = AK09911_REG_WIA1;
	err = akm_i2c_rxdata(client, akm->sense_info, AKM_SENSOR_INFO_SIZE);
	if (err < 0)
		return err;

	/* Set FUSE access mode */
	err = AKECS_SetMode(akm, AK09911_MODE_FUSE_ACCESS);
	if (err < 0)
		return err;

	akm->sense_conf[0] = AK09911_FUSE_ASAX;
	err = akm_i2c_rxdata(client, akm->sense_conf, AKM_SENSOR_CONF_SIZE);
	if (err < 0)
		return err;

	err = AKECS_SetMode(akm, AK09911_MODE_POWERDOWN);
	if (err < 0)
		return err;

	/* Check read data */
	if ((akm->sense_info[0] != AK09911_WIA1_VALUE) ||
			(akm->sense_info[1] != AK09911_WIA2_VALUE)){

		compass_ERR("[COMPASS]%s: The device is not AKM Compass.", __func__);
		return -ENXIO;
	}
	return err;
}

static int akm_compass_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	struct device_node *node = client->dev.of_node;
	int err = 0;
	int akm9911=0;
	int i;
	compass_FLOW("[COMPASS]start probing.");
	err = of_property_read_u32(node, "akm9911", &akm9911);
	if (err) {
		compass_ERR("[COMPASS]akm09911 read failed:%d\n", err);
		return  -ENXIO;
	}

	if(!akm9911)
	{
		compass_INFO("[COMPASS]%s: compass akm09911 device not exits\n", __func__);
		return  -ENXIO;	
	}

	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
		compass_ERR("[COMPASS]%s: check_functionality failed.", __func__);
		err = -ENODEV;
		goto exit0;
	}

	/* Allocate memory for driver data */
	s_akm = kzalloc(sizeof(struct akm_compass_data), GFP_KERNEL);
	if (!s_akm) {
		compass_ERR("[COMPASS]%s: memory allocation failed.", __func__);
		err = -ENOMEM;
		goto exit1;
	}

	/**** initialize variables in akm_compass_data *****/
	init_waitqueue_head(&s_akm->drdy_wq);
	init_waitqueue_head(&s_akm->open_wq);

	mutex_init(&s_akm->sensor_mutex);
	mutex_init(&s_akm->accel_mutex);
	mutex_init(&s_akm->val_mutex);

	atomic_set(&s_akm->active, 0);
	atomic_set(&s_akm->drdy, 0);

	s_akm->is_busy = 0;
	s_akm->enable_flag = 0;

	/* Set to 1G in Android coordination, AKSC format */
	s_akm->accel_data[0] = 0;
	s_akm->accel_data[1] = 0;
	s_akm->accel_data[2] = 720;

	for (i = 0; i < AKM_NUM_SENSORS; i++)
		s_akm->delay[i] = -1;

	err = of_property_read_u32(node, "compass_layout", (u32 *)&s_akm->layout);
	if (err) {
		compass_ERR("[COMPASS]compass_layout read failed:%d\n", err);
		goto exit1;
	}
	s_akm->gpio_rstn= of_get_named_gpio(node, "gpio_RSTN", 0);
	
	if (s_akm->gpio_rstn != 0) {
		compass_INFO("[COMPASS]fyh compass driver reset,gpio=%d,s_akm->gpio_rstn=%d\n",gpio_get_value(s_akm->gpio_rstn),s_akm->gpio_rstn);
		err=gpio_request(s_akm->gpio_rstn,NULL);
		if(err<0)
		{
			compass_INFO("[COMPASS]compass,gpio_request error!!!\n");
		}
		gpio_direction_output(s_akm->gpio_rstn, 1);
		msleep(10);
		gpio_direction_output(s_akm->gpio_rstn, 0);
		msleep(100);
		gpio_direction_output(s_akm->gpio_rstn, 1);
		msleep(10);
		/* No error is returned */
		compass_INFO("[COMPASS]compass driver reset2,gpio=%d\n",gpio_get_value(s_akm->gpio_rstn));
	} 
#if 0
	/***** Set platform information *****/
	pdata = client->dev.platform_data;
	if (pdata) {
		/* Platform data is available. copy its value to local. */
		s_akm->layout = pdata->layout;
		s_akm->gpio_rstn = pdata->gpio_RSTN;
	} else {
		/* Platform data is not available.
		   Layout and information should be set by each application. */
		compass_FLOW("[COMPASS]%s: No platform data.", __func__);
		s_akm->layout = 0;
		s_akm->gpio_rstn = 0;
	}
#endif
#if 0
       /* set regulator*/
	error = akm_regulator_configure(pdata, client);
	if (error) {
		compass_ERR("[COMPASS]Failed to intialize hardware\n");
		compass_INFO("[COMPASS]panben error gyr_regulator_configure\n");
	}
      akm_regulator_enable(pdata);
#endif

	/***** I2C initialization *****/
	s_akm->i2c = client;
	/* set client data */
	i2c_set_clientdata(client, s_akm);
	/* check connection */
	err = akm09911_i2c_check_device(client);
	if (err < 0)
		goto exit2;
	err = set_sensor_chip_info(COMPASS, "AKM AKM09911");
	if (err) {
		compass_ERR("[COMPASS]set_sensor_chip_info error\n");
	}
	/***** input *****/
	err = akm_compass_input_init(&s_akm->input);
	if (err) {
		compass_ERR("[COMPASS]%s: input_dev register failed", __func__);
		goto exit3;
	}
	input_set_drvdata(s_akm->input, s_akm);

	/***** IRQ setup *****/
	s_akm->irq = client->irq;
	compass_FLOW("[COMPASS]%s: IRQ is #%d.",__func__, s_akm->irq);

	if (s_akm->irq) {
		err = request_threaded_irq(
				s_akm->irq,
				NULL,
				akm_compass_irq,
				IRQF_TRIGGER_RISING|IRQF_ONESHOT,
				dev_name(&client->dev),
				s_akm);
		if (err < 0) {
			compass_ERR("[COMPASS]%s: request irq failed.", __func__);
			goto exit4;
		}
	}

	/***** misc *****/
	err = misc_register(&akm_compass_dev);
	if (err) {
		compass_ERR("[COMPASS]%s: akm_compass_dev register failed", __func__);
		goto exit5;
	}

	/***** sysfs *****/
	err = create_sysfs_interfaces(s_akm);
	if (0 > err) {
		compass_ERR("[COMPASS]%s: create sysfs failed.", __func__);
		goto exit6;
	}
	compass_INFO("[COMPASS]compass:akm09911 successfully probed.");
#ifdef CONFIG_HUAWEI_HW_DEV_DCT
	/* detect current device successful, set the flag as present */
	set_hw_dev_flag(DEV_I2C_COMPASS);
#endif
  #if defined (CONFIG_HUAWEI_DSM)
      akm09911_flag=1;
  #endif
	return 0;
exit6:
	misc_deregister(&akm_compass_dev);
exit5:
	if (s_akm->irq)
		free_irq(s_akm->irq, s_akm);
exit4:
	input_unregister_device(s_akm->input);
exit3:
exit2:
	kfree(s_akm);
exit1:
exit0:
	compass_INFO("[COMPASS]compass:akm09911 probe failed\n");
	return err;
}

static int akm_compass_remove(struct i2c_client *client)
{
	struct akm_compass_data *akm = i2c_get_clientdata(client);

	remove_sysfs_interfaces(akm);
	if (misc_deregister(&akm_compass_dev) < 0)
		compass_ERR("[COMPASS]misc deregister failed.");
	if (akm->irq)
		free_irq(akm->irq, akm);
	input_unregister_device(akm->input);
	kfree(akm);
	compass_INFO("[COMPASS]successfully removed.");
	return 0;
}

static const struct i2c_device_id akm_compass_id[] = {
	{"akm09911", 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, akm_compass_id);

static const struct of_device_id akm09911_of_match[] = {
	{.compatible = "huawei,akm09911",},
	{},
};
MODULE_DEVICE_TABLE(of, akm09911_of_match);

static struct i2c_driver akm_compass_driver = {
	.probe		= akm_compass_probe,
	.remove		= akm_compass_remove,
	.id_table	= akm_compass_id,
	.driver = {
		.name	= "akm09911",
		.of_match_table = akm09911_of_match,
	},
};

static int __init akm_compass_init(void)
{
	compass_INFO("[COMPASS]AKM compass driver: initialize.");
	return i2c_add_driver(&akm_compass_driver);
}

static void __exit akm_compass_exit(void)
{
	compass_INFO("[COMPASS]AKM compass driver: release.");
	i2c_del_driver(&akm_compass_driver);
}

module_init(akm_compass_init);
module_exit(akm_compass_exit);

MODULE_AUTHOR("ben pan<panben@huawei.com>");
MODULE_DESCRIPTION("AKM compass driver");
MODULE_LICENSE("GPL");

