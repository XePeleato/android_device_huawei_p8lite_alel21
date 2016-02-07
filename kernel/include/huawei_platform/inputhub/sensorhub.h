/*
 *  Sensor Hub driver
 *
 * Copyright (C) 2012 Huawei, Inc.
 * Author: qindiwen <qindiwen@huawei.com>
 *
 */

#ifndef __LINUX_SENSORHUB_H__
#define __LINUX_SENSORHUB_H__
#include <linux/ioctl.h>

/* ioctl cmd define */
#define SHBIO                         0xB1

#define SHB_IOCTL_APP_ENABLE_SENSOR      _IOW(SHBIO, 0x01, short)
#define SHB_IOCTL_APP_DISABLE_SENSOR    _IOW(SHBIO, 0x02, short)

#define SHB_IOCTL_APP_DELAY_ACCEL             _IOW(SHBIO, 0x03, short)
#define SHB_IOCTL_APP_DELAY_LIGHT             _IOW(SHBIO, 0x04, short)
#define SHB_IOCTL_APP_DELAY_PROXI             _IOW(SHBIO, 0x05, short)
#define SHB_IOCTL_APP_DELAY_GYRO             _IOW(SHBIO, 0x06, short)
#define SHB_IOCTL_APP_DELAY_GRAVITY             _IOW(SHBIO, 0x07, short)
#define SHB_IOCTL_APP_DELAY_MAGNETIC             _IOW(SHBIO, 0x08, short)
#define SHB_IOCTL_APP_DELAY_ROTATESCREEN          _IOW(SHBIO, 0x09, short)
#define SHB_IOCTL_APP_DELAY_LINEARACCELERATE    _IOW(SHBIO, 0x0A, short)
#define SHB_IOCTL_APP_DELAY_ORIENTATION             _IOW(SHBIO, 0x0B, short)
#define SHB_IOCTL_APP_DELAY_ROTATEVECTOR         _IOW(SHBIO, 0x0C, short)

#define SHB_IOCTL_APP_DELAY_PRESSURE            _IOW(SHBIO, 0x0D, short)
#define SHB_IOCTL_APP_DELAY_TEMPERATURE         _IOW(SHBIO, 0x0E, short)
#define SHB_IOCTL_APP_DELAY_RELATIVE_HUMIDITY   _IOW(SHBIO, 0x0F, short)
#define SHB_IOCTL_APP_DELAY_AMBIENT_TEMPERATURE _IOW(SHBIO, 0x10, short)
#define SHB_IOCTL_APP_DELAY_MCU_LABC            _IOW(SHBIO, 0x11, short)
#define SHB_IOCTL_APP_DELAY_HALL                _IOW(SHBIO, 0x12, short)

#define SHB_IOCTL_APP_DELAY_MAGNETIC_FIELD_UNCALIBRATED         _IOW(SHBIO, 0x13, short)
#define SHB_IOCTL_APP_DELAY_GAME_ROTATION_VECTOR                _IOW(SHBIO, 0x14, short)
#define SHB_IOCTL_APP_DELAY_GYROSCOPE_UNCALIBRATED              _IOW(SHBIO, 0x15, short)
#define SHB_IOCTL_APP_DELAY_SIGNIFICANT_MOTION                  _IOW(SHBIO, 0x16, short)
#define SHB_IOCTL_APP_DELAY_STEP_DETECTOR                       _IOW(SHBIO, 0x17, short)
#define SHB_IOCTL_APP_DELAY_STEP_COUNTER                        _IOW(SHBIO, 0x18, short)
#define SHB_IOCTL_APP_DELAY_GEOMAGNETIC_ROTATION_VECTOR         _IOW(SHBIO, 0x19, short)
#define SHB_IOCTL_APP_DELAY_AIRPRESS                _IOW(SHBIO, 0x1A, short)
#define SHB_IOCTL_APP_DELAY_HANDPRESS             _IOW(SHBIO, 0x1B, short)
#define SHB_IOCTL_APP_DELAY_CAP_PROX              _IOW(SHBIO, 0x1C, short)

/*begin huangwen 20120706*/
/*get sensor MCU mode */
#define SHB_IOCTL_APP_GET_SENSOR_MCU_MODE         _IOR(SHBIO, 0x51, short)
/*end huangwen 20120706*/
/*
  * Warning notes:
  * The below sensor values  is used by mcu and hal sensorhub module,
  * it must be careful to modify the below values.
  */
#define SENSORHUB_TYPE_ACCELEROMETER 0x01
#define SENSORHUB_TYPE_GYROSCOPE 0x02
#define SENSORHUB_TYPE_MAGNETIC 0x03
#define SENSORHUB_TYPE_LIGHT 0x04
#define SENSORHUB_TYPE_PROXIMITY 0x05
#define SENSORHUB_TYPE_ROTATESCREEN 0x06
#define SENSORHUB_TYPE_LINEARACCELERATE 0x07
#define SENSORHUB_TYPE_GRAVITY 0x08
#define SENSORHUB_TYPE_ORIENTATION 0x09
#define SENSORHUB_TYPE_ROTATEVECTOR 0x0A
#define SENSORHUB_TYPE_PRESSURE            0x0B
#define SENSORHUB_TYPE_TEMPERATURE         0x0C
#define SENSORHUB_TYPE_RELATIVE_HUMIDITY   0x0D
#define SENSORHUB_TYPE_AMBIENT_TEMPERATURE 0x0E
#define SENSORHUB_TYPE_MCU_LABC            0x0F
#define SENSORHUB_TYPE_HALL                0x10

#define SENSORHUB_TYPE_MAGNETIC_FIELD_UNCALIBRATED     0x11
#define SENSORHUB_TYPE_GAME_ROTATION_VECTOR            0x12
#define SENSORHUB_TYPE_GYROSCOPE_UNCALIBRATED          0x13
#define SENSORHUB_TYPE_SIGNIFICANT_MOTION              0x14
#define SENSORHUB_TYPE_STEP_DETECTOR                   0x15
#define SENSORHUB_TYPE_STEP_COUNTER                    0x16
#define SENSORHUB_TYPE_GEOMAGNETIC_ROTATION_VECTOR     0x17
#define SENSORHUB_TYPE_AIRPRESS                         0x18
#define SENSORHUB_TYPE_HANDPRESS                        0x19
#define SENSORHUB_TYPE_CAP_PROX                         0x1A
/*begin huangwen 20120706*/
extern int getSensorMcuMode(void);
/*end huangwen 20120706*/
extern int set_backlight_brightness(int brightness);
#endif /* __LINUX_SENSORHUB_H__ */
