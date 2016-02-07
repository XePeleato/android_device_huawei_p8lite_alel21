/*****************************************************************************************
* Filename:	coulomb_interface.h
*
* Discription:  coulometer interface functions.
* Copyright:	(C) 2014 huawei.
*
* revision history:
* 
*
******************************************************************************************/
#ifndef __COULOMB_TEST_INTERFACE
#define __COULOMB_TEST_INTERFACE

#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/of_address.h>
#include "hisi_hi6421v300_coul.h"

#define COULOMB_REG_READ(regAddr)             coulomb_reg_read(regAddr)
#define COULOMB_REG_WRITE(regAddr,regval)     coulomb_reg_write((unsigned int)(regAddr),( unsigned char)regval)

#define COULOMB_REGS_READ(regAddr,buf,size)   coulomb_regs_read((unsigned int)(regAddr),(unsigned char*)(buf),(int)(size))
#define COULOMB_REGS_WRITE(regAddr,buf,size)  coulomb_regs_write((unsigned int)(regAddr),(unsigned char*)(buf),(int)(size))

#define COULOMB_COUL_VERSION_ADDR    0x0
#define COULOMB_DEBUG_REG    0x1bf
#define COULOMB_BATTERY_MOVE_ADDR    0x1c1
#define COULOMB_VOL_OFFSET_MOD    0x1ce
#define COULOMB_CUR_OFFSET_MOD    0x1cc
#define COULOMB_COUL_CTRL_REG    0x14d


struct coulomb_test_interface_info{
    int low_int;
    int v_offset_a;
    int v_offset_b;
};

extern struct coulomb_test_interface_info *g_coulomb_test_interface_info;

#endif
