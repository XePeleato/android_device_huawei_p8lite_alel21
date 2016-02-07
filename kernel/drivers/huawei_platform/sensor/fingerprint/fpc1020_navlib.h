/* FPC1020 Touch sensor driver
 *
 * Copyright (c) 2014 Fingerprint Cards AB <tech@fingerprints.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License Version 2
 * as published by the Free Software Foundation.
 */

#ifndef LINUX_SPI_FPC1020_NAVLIB_H
#define LINUX_SPI_FPC1020_NAVLIB_H

#ifdef FPC1020_NAVLIB_EXTERNAL_DEBUG
typedef unsigned char u8;
#else
#include <linux/types.h>
#endif /* #ifdef FPC1020_NAVLIB_EXTERNAL_DEBUG */

/** Qualification time for click
	Android ViewConfiguration: 180 ms */
#define FPC1020_QUALIFICATION_TIME_NS_CLICK      (180000000)

/** Qualification time for long click
	Android ViewConfiguration: 500 ms */
#define FPC1020_QUALIFICATION_TIME_NS_LONGPRESS  (500000000)

/** Navigation speed and sensitivity to movement */
#define SENSOR_PIXEL_TO_REPORT_REL_DIVIDER 8
#define SENSOR_PIXEL_TOUCH_SLOP 8

/** Handle sensor placement and rotation */

/** Crop area for navigation source images, values for X and W needs to be multiples of ADC group size */

/**
 * Calculate translation vector between two different navigation images
 *
 * @param p_curr	Pointer to image buffer of latest navigation image
 * @param p_prev	Pointer to image buffer of previous navigation image
 * @param p_dx     	Out parameter for translation vector x
 * @param p_dy     	Out parameter for translation vector y
 */
void get_movement(const u8* p_curr, const u8* p_prev, int* p_dx, int* p_dy);

#endif /* LINUX_SPI_FPC1020_NAVLIB_H */
