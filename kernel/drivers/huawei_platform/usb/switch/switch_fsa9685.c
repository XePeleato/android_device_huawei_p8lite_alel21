/************************************************************
*
* Copyright (C), 1988-1999, Huawei Tech. Co., Ltd.
* FileName: switch_fsa9685.c
* Author: lixiuna(00213837)       Version : 0.1      Date:  2013-11-06
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
*  Description:    .c file for switch chip
*  Version:
*  Function List:
*  History:
*  <author>  <time>   <version >   <desc>
***********************************************************/

#include <linux/i2c.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/timer.h>
#include <linux/param.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/workqueue.h>
#include <linux/slab.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_device.h>
#include <linux/of_gpio.h>
#include <linux/of_irq.h>
#include <asm/irq.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <huawei_platform/usb/switch/switch_usb.h>
#include "switch_chip.h"
#include <linux/huawei/usb/hisi_usb.h>
#ifdef CONFIG_HDMI_K3
#include <../video/k3/hdmi/k3_hdmi.h>
#endif
#ifdef CONFIG_HUAWEI_HW_DEV_DCT
#include <linux/hw_dev_dec.h>
#endif
#include <huawei_platform/log/hw_log.h>
#include <huawei_platform/usb/hw_rwswitch.h>
#include <huawei_platform/usb/switch/switch_fsa9685.h>
#include <huawei_platform/power/huawei_charger.h>

struct mutex fcp_detect_lock;

extern unsigned int get_boot_into_recovery_flag(void);
#define HWLOG_TAG switch_fsa9685
HWLOG_REGIST();

static int gpio = -1;
static struct i2c_client *this_client = NULL;
static struct work_struct   g_intb_work;
static struct delayed_work   detach_delayed_work;
#ifdef CONFIG_FSA9685_DEBUG_FS
static int reg_locked = 1;
static char chip_regs[0x5c] = { 0 };
#endif
int is_support_fcp(void);
static int fsa9685_write_reg(int reg, int val)
{
    int ret;
    if (NULL == this_client) {
        ret = -ERR_NO_DEV;
        hwlog_err("%s: this_client=NULL!!! ret=%d\n", __func__, ret);
        return ret;
    }
    ret = i2c_smbus_write_byte_data(this_client, reg, val);
    if (ret < 0)
        hwlog_info("%s: i2c write error!!! ret=%d\n", __func__, ret);

#ifdef CONFIG_FSA9685_DEBUG_FS
    chip_regs[reg] = val;
#endif
    return ret;
}

static int fsa9685_read_reg(int reg)
{
    int ret;
    if (NULL == this_client) {
        ret = -ERR_NO_DEV;
        hwlog_err("%s: this_client=NULL!!! ret=%d\n", __func__, ret);
        return ret;
    }
    ret = i2c_smbus_read_byte_data(this_client, reg);
    if (ret < 0)
        hwlog_info("%s: i2c read error!!! ret=%d\n", __func__, ret);

#ifdef CONFIG_FSA9685_DEBUG_FS
    chip_regs[reg] = ret;
#endif
    return ret;
}

int fsa9685_manual_sw(int input_select)
{
    int value = 0, ret = 0;
    if (NULL == this_client) {
        ret = -ERR_NO_DEV;
        hwlog_err("%s: this_client=NULL!!! ret=%d\n", __func__, ret);
        return ret;
    }

    hwlog_info("%s: input_select = %d", __func__, input_select);
    switch (input_select){
        case FSA9685_USB1_ID_TO_IDBYPASS:
            value = REG_VAL_FSA9685_USB1_ID_TO_IDBYPASS;
            break;
        case FSA9685_USB2_ID_TO_IDBYPASS:
            value = REG_VAL_FSA9685_USB2_ID_TO_IDBYPASS;
            break;
        case FSA9685_UART_ID_TO_IDBYPASS:
            value = REG_VAL_FSA9685_UART_ID_TO_IDBYPASS;
            break;
        case FSA9685_MHL_ID_TO_CBUS:
            value = REG_VAL_FSA9685_MHL_ID_TO_CBUS;
            break;
        case FSA9685_USB1_ID_TO_VBAT:
            value = REG_VAL_FSA9685_USB1_ID_TO_VBAT;
            break;
        case FSA9685_OPEN:
        default:
            value = REG_VAL_FSA9685_OPEN;
            break;
    }

    ret = fsa9685_write_reg(FSA9685_REG_MANUAL_SW_1, value);
    if ( ret < 0 ){
        ret = -ERR_FSA9685_REG_MANUAL_SW_1;
        hwlog_err("%s: write reg FSA9685_REG_MANUAL_SW_1 error!!! ret=%d\n", __func__, ret);
        return ret;
    }

    value = fsa9685_read_reg(FSA9685_REG_CONTROL);
    if (value < 0){
        ret = -ERR_FSA9685_READ_REG_CONTROL;
        hwlog_err("%s: read FSA9685_REG_CONTROL error!!! ret=%d\n", __func__, ret);
        return ret;
    }

    value &= (~FSA9685_MANUAL_SW); // 0: manual switching
    ret = fsa9685_write_reg(FSA9685_REG_CONTROL, value);
    if ( ret < 0 ){
        ret = -ERR_FSA9685_WRITE_REG_CONTROL;
        hwlog_err("%s: write FSA9685_REG_CONTROL error!!! ret=%d\n", __func__, ret);
        return ret;
    }

    return 0;

}
EXPORT_SYMBOL_GPL(fsa9685_manual_sw);

int fsa9685_manual_detach(void)
{
    int ret = 0;
    if (NULL == this_client){
        ret = -ERR_NO_DEV;
        hwlog_err("%s: this_client=NULL!!! ret=%d\n", __func__, ret);
        return ret;
    }

    schedule_delayed_work(&detach_delayed_work, msecs_to_jiffies(0));
    hwlog_info("%s: ------end.\n", __func__);
    return ret;
}
static void fsa9685_detach_work(struct work_struct *work)
{
    int ret;
    hwlog_info("%s: ------entry.\n", __func__);

    ret = fsa9685_read_reg(FSA9685_REG_DETACH_CONTROL);
    if ( ret < 0 ){
        hwlog_err("%s: read FSA9685_REG_DETACH_CONTROL error!!! ret=%d", __func__, ret);
        return;
    }

    ret = fsa9685_write_reg(FSA9685_REG_DETACH_CONTROL, 1);
    if ( ret < 0 ){
        hwlog_err("%s: write FSA9685_REG_DETACH_CONTROL error!!! ret=%d", __func__, ret);
        return;
    }

    hwlog_info("%s: ------end.\n", __func__);
    return;
}
EXPORT_SYMBOL_GPL(fsa9685_manual_detach);

static void fsa9685_intb_work(struct work_struct *work);
static irqreturn_t fsa9685_irq_handler(int irq, void *dev_id)
{
    int gpio_value = 0;
    gpio_value = gpio_get_value(gpio);
    if(gpio_value==1)
        hwlog_err("%s: intb high when interrupt occured!!!\n", __func__);

    schedule_work(&g_intb_work);

    hwlog_info("%s: ------end. gpio_value=%d\n", __func__, gpio_value);
    return IRQ_HANDLED;
}
/****************************************************************************
  Function:     is_fcp_charger_type
  Description:  after fcp detect ok,it will show it is fcp adapter
  Input:        void
  Output:       NA
  Return:        true:fcp adapter
                false: not fcp adapter
***************************************************************************/
static int is_fcp_charger_type(void)
{
    int reg_val = 0;

    if(is_support_fcp())
    {
        return false;
    }

    reg_val = fsa9685_read_reg(FSA9685_REG_DEVICE_TYPE_4);

    if(reg_val < 0)
    {
        hwlog_err("%s: read FSA9685_REG_DEVICE_TYPE_4 error!reg:%d\n", __func__,reg_val);
        return false;
    }
    if(reg_val & FSA9685_ACCP_CHARGER_DET)
    {
        return true ;
    }
    return false;
}
enum hisi_charger_type fsa9685_get_charger_type(void)
{
    enum hisi_charger_type charger_type = CHARGER_TYPE_NONE;
    int val = 0;

    if (NULL == this_client) {
        hwlog_info("%s: this_client=NULL! charger_type set to NONE\n", __func__);
        return charger_type;
    }

    val = fsa9685_read_reg(FSA9685_REG_DEVICE_TYPE_1);
    if (val < 0)
    {
        hwlog_err("%s: read REG[%d] erro, val = %d, charger_type set to NONE\n",
                  __func__, FSA9685_REG_DEVICE_TYPE_1, val);
        return charger_type;
    }

    if (val & FSA9685_USB_DETECTED)
        charger_type = CHARGER_TYPE_SDP;
    else if(val & FSA9685_CDP_DETECTED)
        charger_type = CHARGER_TYPE_CDP;
    else if(val & FSA9685_DCP_DETECTED)
        charger_type = CHARGER_TYPE_DCP;
    else
        charger_type = CHARGER_TYPE_NONE;

    if((charger_type == CHARGER_TYPE_NONE) && is_fcp_charger_type())
    {
       charger_type = CHARGER_TYPE_DCP;/*if is fcp ,report dcp,because when we detect fcp last time ,FSA9685_REG_DEVICE_TYPE_4 will be set */
       hwlog_info("%s:update charger type by device type4, charger type is:%d\n",__func__,charger_type);
    }

    return charger_type;
}

static void fsa9685_intb_work(struct work_struct *work)
{
    int reg_ctl, reg_intrpt, reg_adc, reg_dev_type1, reg_dev_type2, reg_dev_type3, vbus_status;
    int ret = -1;
    int id_valid_status = ID_VALID;
    static int invalid_times = 0;
    static int otg_attach = 0;
    static int pedestal_attach = 0;
    reg_intrpt = fsa9685_read_reg(FSA9685_REG_INTERRUPT);
    vbus_status = fsa9685_read_reg(FSA9685_REG_VBUS_STATUS);
    hwlog_info("%s: read FSA9685_REG_INTERRUPT. reg_intrpt=0x%x\n", __func__, reg_intrpt);

    /* if supprot fcp read /clear interrupt */
    if(0 == is_support_fcp())
    {
        hwlog_info("%s : read ACCP interrupt,reg[0x59]=0x%x,reg[0x5A]=0x%x\n",__func__,
            fsa9685_read_reg(FSA9685_REG_ACCP_INTERRUPT1), fsa9685_read_reg(FSA9685_REG_ACCP_INTERRUPT2));
    }
    if (unlikely(reg_intrpt < 0)) {
        hwlog_err("%s: read FSA9685_REG_INTERRUPT error!!!\n", __func__);
    } else if (unlikely(reg_intrpt == 0)) {
        hwlog_err("%s: read FSA9685_REG_INTERRUPT, and no intr!!!\n", __func__);
    } else {
        if (reg_intrpt & FSA9685_ATTACH){
            hwlog_info("%s: FSA9685_ATTACH\n", __func__);
            reg_dev_type1 = fsa9685_read_reg(FSA9685_REG_DEVICE_TYPE_1);
            reg_dev_type2 = fsa9685_read_reg(FSA9685_REG_DEVICE_TYPE_2);
            reg_dev_type3 = fsa9685_read_reg(FSA9685_REG_DEVICE_TYPE_3);
            hwlog_info("%s: reg_dev_type1=0x%X, reg_dev_type2=0x%X, reg_dev_type3= 0x%X\n", __func__,
                reg_dev_type1, reg_dev_type2, reg_dev_type3);
            if (reg_dev_type1 & FSA9685_FC_USB_DETECTED) {
                hwlog_info("%s: FSA9685_FC_USB_DETECTED\n", __func__);
            }
            if (reg_dev_type1 & FSA9685_USB_DETECTED){
                hwlog_info("%s: FSA9685_USB_DETECTED\n", __func__);
                if (FSA9685_USB2_ID_TO_IDBYPASS == get_swstate_value()){
                    switch_usb2_access_through_ap();
                    hwlog_info("%s: fsa9685 switch to USB2 by setvalue\n", __func__);
                }
            }
            if (reg_dev_type1 & FSA9685_UART_DETECTED) {
                hwlog_info("%s: FSA9685_UART_DETECTED\n", __func__);
            }
            if (reg_dev_type1 & FSA9685_MHL_DETECTED) {
                hwlog_info("%s: FSA9685_MHL_DETECTED\n", __func__);
            }
            if (reg_dev_type1 & FSA9685_CDP_DETECTED) {
                hwlog_info("%s: FSA9685_CDP_DETECTED\n", __func__);
            }
            if (reg_dev_type1 & FSA9685_DCP_DETECTED) {
                hwlog_info("%s: FSA9685_DCP_DETECTED\n", __func__);
            }
            if (reg_dev_type1 & FSA9685_USBOTG_DETECTED) {
                hwlog_info("%s: FSA9685_USBOTG_DETECTED\n", __func__);
                otg_attach = 1;
                hisi_usb_id_change(ID_FALL_EVENT);
            }
            if (reg_dev_type1 & FSA9685_DEVICE_TYPE1_UNAVAILABLE) {
                id_valid_status = ID_INVALID;
                hwlog_info("%s: FSA9685_DEVICE_TYPE1_UNAVAILABLE_DETECTED\n", __func__);
            }
            if (reg_dev_type2 & FSA9685_JIG_UART) {
                hwlog_info("%s: FSA9685_JIG_UART\n", __func__);
            }
            if (reg_dev_type2 & FSA9685_DEVICE_TYPE2_UNAVAILABLE) {
                id_valid_status = ID_INVALID;
                hwlog_info("%s: FSA9685_DEVICE_TYPE2_UNAVAILABLE_DETECTED\n", __func__);
            }
            if (reg_dev_type3 & FSA9685_CUSTOMER_ACCESSORY7) {
                fsa9685_manual_sw(FSA9685_USB1_ID_TO_IDBYPASS);
                ret = usb_port_switch_request(INDEX_USB_REWORK_SN);
                hwlog_info("%s: FSA9685_CUSTOMER_ACCESSORY7 USB_REWORK_SN ret %d\n", __func__, ret);
            }
            if (reg_dev_type3 & FSA9685_CUSTOMER_ACCESSORY5) {
                hwlog_info("%s: FSA9685_CUSTOMER_ACCESSORY5, 365K\n", __func__);
                fsa9685_manual_sw(FSA9685_USB1_ID_TO_IDBYPASS);
                pedestal_attach = 1;
                usb_custom_acc5_event(pedestal_attach);
            }
            if (reg_dev_type3 & FSA9685_FM8_ACCESSORY) {
                hwlog_info("%s: FSA9685_FM8_DETECTED\n", __func__);
                fsa9685_manual_sw(FSA9685_USB1_ID_TO_IDBYPASS);
            }
            if (reg_dev_type3 & FSA9685_DEVICE_TYPE3_UNAVAILABLE) {
                id_valid_status = ID_INVALID;
                if (reg_intrpt & FSA9685_VBUS_CHANGE) {
                    fsa9685_manual_sw(FSA9685_USB1_ID_TO_IDBYPASS);
                }
                hwlog_info("%s: FSA9685_DEVICE_TYPE3_UNAVAILABLE_DETECTED\n", __func__);
            }
        }

        if (reg_intrpt & FSA9685_RESERVED_ATTACH) {
            id_valid_status = ID_INVALID;
            if (reg_intrpt & FSA9685_VBUS_CHANGE) {
                fsa9685_manual_sw(FSA9685_USB1_ID_TO_IDBYPASS);
            }
            hwlog_info("%s: FSA9685_RESERVED_ATTACH\n", __func__);
        }

        if (reg_intrpt & FSA9685_DETACH) {
            hwlog_info("%s: FSA9685_DETACH\n", __func__);
            /* check control register, if manual switch, reset to auto switch */
            reg_ctl = fsa9685_read_reg(FSA9685_REG_CONTROL);
            reg_dev_type2 = fsa9685_read_reg(FSA9685_REG_DEVICE_TYPE_2);
            hwlog_info("%s: reg_ctl=0x%x\n", __func__, reg_ctl);
            if (reg_ctl < 0){
                hwlog_err("%s: read FSA9685_REG_CONTROL error!!! reg_ctl=%d\n", __func__, reg_ctl);
                goto OUT;
            }
            if (0 == (reg_ctl & FSA9685_MANUAL_SW))
            {
                reg_ctl |= FSA9685_MANUAL_SW;
                ret = fsa9685_write_reg(FSA9685_REG_CONTROL, reg_ctl);
                if (ret < 0) {
                    hwlog_err("%s: write FSA9685_REG_CONTROL error!!!\n", __func__);
                    goto OUT;
                }
            }
            if (otg_attach == 1) {
                hwlog_info("%s: FSA9685_USBOTG_DETACH\n", __func__);
                hisi_usb_id_change(ID_RISE_EVENT);
                otg_attach = 0;
            }
            if (pedestal_attach ==1) {
                hwlog_info("%s: FSA9685_CUSTOMER_ACCESSORY5_DETACH\n", __func__);
                pedestal_attach = 0;
                usb_custom_acc5_event(pedestal_attach);
            }
            if (reg_dev_type2 & FSA9685_JIG_UART) {
                hwlog_info("%s: FSA9685_JIG_UART\n", __func__);
            }
        }
        if (reg_intrpt & FSA9685_VBUS_CHANGE) {
            hwlog_info("%s: FSA9685_VBUS_CHANGE\n", __func__);
        }
        if (reg_intrpt & FSA9685_ADC_CHANGE) {
            reg_adc = fsa9685_read_reg(FSA9685_REG_ADC);
            hwlog_info("%s: FSA9685_ADC_CHANGE. reg_adc=%d\n", __func__, reg_adc);
            if (reg_adc < 0) {
                hwlog_err("%s: read FSA9685_ADC_CHANGE error!!! reg_adc=%d\n", __func__, reg_adc);
            }
            /* do user specific handle */
        }
    }

    if ((ID_INVALID == id_valid_status) &&
                (reg_intrpt & (FSA9685_ATTACH | FSA9685_RESERVED_ATTACH))) {
        invalid_times++;
        hwlog_info("%s: invalid time:%d reset fsa9685 work\n", __func__, invalid_times);

        if (invalid_times < MAX_DETECTION_TIMES) {
            hwlog_info("%s: start schedule delayed work\n", __func__);
            schedule_delayed_work(&detach_delayed_work, msecs_to_jiffies(0));
        } else {
            invalid_times = 0;
        }
    } else if ((ID_VALID == id_valid_status) &&
                (reg_intrpt & (FSA9685_ATTACH | FSA9685_RESERVED_ATTACH))) {
        invalid_times = 0;
    }

OUT:
    hwlog_info("%s: ------end.\n", __func__);
    return;
}

#ifdef CONFIG_FSA9685_DEBUG_FS
static ssize_t dump_regs_show(struct device *dev, struct device_attribute *attr,
                char *buf)
{
    const int regaddrs[] = {0x01, 0x02, 0x03, 0x04, 0x5, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d};
    const char str[] = "0123456789abcdef";
    int i = 0, index;
    char val = 0;
    for (i=0; i<0x60; i++) {
        if ((i%3)==2)
            buf[i]=' ';
        else
            buf[i] = 'x';
    }
    buf[0x5d] = '\n';
    buf[0x5e] = 0;
    buf[0x5f] = 0;
    if ( reg_locked != 0 ) {
        for (i=0; i<ARRAY_SIZE(regaddrs); i++) {
            if (regaddrs[i] != 0x03) {
                val = fsa9685_read_reg(regaddrs[i]);
                chip_regs[regaddrs[i]] = val;
            }
        }
    }

    for (i=0; i<ARRAY_SIZE(regaddrs); i++) {
        index = regaddrs[i];
        val = chip_regs[index];
            buf[3*index] = str[(val&0xf0)>>4];
        buf[3*index+1] = str[val&0x0f];
        buf[3*index+2] = ' ';
    }

    return 0x60;
}
static DEVICE_ATTR(dump_regs, S_IRUGO, dump_regs_show, NULL);
#endif

static ssize_t switchctrl_store(struct device *dev,
                          struct device_attribute *attr, const char *buf, size_t size)
{
    int action = 0;
    if (sscanf(buf, "%d", &action) != 1) {
        hwlog_err("%s:write regs failed, invalid input!\n", __func__);
        return -1;
    }
    switch (action) {
        case MANUAL_DETACH:
            hwlog_info("%s:manual_detach\n", __func__);
            fsa9685_manual_detach();
            break;
        case MANUAL_SWITCH:
            hwlog_info("%s:manual_switch\n", __func__);
            fsa9685_manual_sw(FSA9685_USB1_ID_TO_VBAT);
            break;
        default:
            hwlog_err("%s:Wrong input action!\n", __func__);
            return -1;
    }
    return 0x60;
}

static ssize_t switchctrl_show(struct device *dev, struct device_attribute *attr,
                char *buf)
{
    int device_type1 = 0, device_type2 = 0, device_type3 = 0, mode = -1, tmp = 0;
    device_type1 = fsa9685_read_reg(FSA9685_REG_DEVICE_TYPE_1);
    if (device_type1 < 0) {
        hwlog_err("%s: read FSA9685_REG_DEVICE_TYPE_1 error!!! reg=%d.\n", __func__, device_type1);
        goto read_reg_failed;
    }
    device_type2 = fsa9685_read_reg(FSA9685_REG_DEVICE_TYPE_2);
    if (device_type2 < 0) {
        hwlog_err("%s: read FSA9685_REG_DEVICE_TYPE_2 error!!! reg=%d.\n", __func__, device_type2);
        goto read_reg_failed;
    }
    device_type3 = fsa9685_read_reg(FSA9685_REG_DEVICE_TYPE_3);
    if (device_type3 < 0) {
        hwlog_err("%s: read FSA9685_REG_DEVICE_TYPE_3 error!!! reg=%d.\n", __func__, device_type3);
        goto read_reg_failed;
    }
    tmp = device_type3 << 16 | device_type2 << 8 | device_type1;
    mode = 0;
    while (tmp >> mode)
        mode++;
read_reg_failed:
    return sprintf(buf, "%d\n", mode);
}

static DEVICE_ATTR(switchctrl, S_IRUGO | S_IWUSR, switchctrl_show, switchctrl_store);

/****************************************************************************
  Function:     fcp_adapter_reg_read
  Description:  read adapter register
  Input:        reg:register's num
                val:the value of register
  Output:       NA
  Return:        0: success
                -1: fail
***************************************************************************/
int fcp_adapter_reg_read(int* val, int reg)
{
    int reg_val = 0;

    fsa9685_write_reg(FSA9685_REG_ACCP_CMD, FCP_CMD_SBRRD);
    fsa9685_write_reg(FSA9685_REG_ACCP_ADDR, reg);
    fsa9685_write_reg(FSA9685_REG_ACCP_CNTL, FSA9685_ACCP_IS_ENABLE | FAS9685_ACCP_SENDCMD);
    mdelay(50);
    /*only show err,not deal with*/
    reg_val = fsa9685_read_reg(FSA9685_REG_ACCP_INTERRUPT1);
    if (reg_val & FAS9685_CRCPAR){
        hwlog_err("%s:crc or par err by not responding,reg[0x59]=%d.\n", __func__, reg_val);
    }
    reg_val = fsa9685_read_reg(FSA9685_REG_ACCP_INTERRUPT2);
    if (reg_val & (FAS9685_CRCRX | FAS9685_PARRX)){
        hwlog_err("%s:crc or par err from slave,reg[0x5a]=%d.\n", __func__, reg_val);
    }

    *val = fsa9685_read_reg(FSA9685_REG_ACCP_DATA);
    return 0;
}

/****************************************************************************
  Function:     fcp_adapter_reg_write
  Description:  write value into the adapter register
  Input:        reg:register's num
                val:the value of register
  Output:       NA
  Return:        0: success
                -1: fail
***************************************************************************/
int fcp_adapter_reg_write(int val, int reg)
{
    int reg_val = 0;

    fsa9685_write_reg(FSA9685_REG_ACCP_CMD, FCP_CMD_SBRWR);
    fsa9685_write_reg(FSA9685_REG_ACCP_ADDR, reg);
    fsa9685_write_reg(FSA9685_REG_ACCP_DATA, val);
    fsa9685_write_reg(FSA9685_REG_ACCP_CNTL, FSA9685_ACCP_IS_ENABLE | FAS9685_ACCP_SENDCMD);
    mdelay(50);

   /*only show err,not deal with*/
    reg_val = fsa9685_read_reg(FSA9685_REG_ACCP_INTERRUPT1);
    if (reg_val & FAS9685_CRCPAR){
        hwlog_err("%s:crc or par err by not responding,reg[0x59]=%d.\n", __func__, reg_val);
    }
    reg_val = fsa9685_read_reg(FSA9685_REG_ACCP_INTERRUPT2);
    if (reg_val & (FAS9685_CRCRX | FAS9685_PARRX)){
        hwlog_err("%s:crc or par err from slave,reg[0x5a]=%d.\n", __func__, reg_val);
    }
    return 0;
}

/****************************************************************************
  Function:     fcp_adapter_detect
  Description:  detect fcp adapter
  Input:        NA
  Output:       NA
  Return:        0: success
                -1: other fail
                1:fcp adapter but detect fail
***************************************************************************/
int fcp_adapter_detect(void)
{
    int reg_val1 = 0;
    int reg_val2 = 0;
    int i = 0;
    if(NULL == this_client)
    {
        return FCP_ADAPTER_DETECT_OTHER;
    }

    mutex_lock(&fcp_detect_lock);
    /*check accp status*/
    reg_val1 = fsa9685_read_reg(FSA9685_REG_DEVICE_TYPE_4);
    reg_val2 = fsa9685_read_reg(FSA9685_REG_ACCP_STATUS);
    if((reg_val1 & FSA9685_ACCP_CHARGER_DET)
        && (FSA9688_ACCP_STATUS_SLAVE_GOOD == (reg_val2 & FSA9688_ACCP_STATUS_MASK )))
    {
        mutex_unlock(&fcp_detect_lock);
        hwlog_info("fcp adapter detect ok.\n");
        return FCP_ADAPTER_DETECT_SUCC;
    }

    /* enable accp */
    reg_val1 = fsa9685_read_reg(FSA9685_REG_CONTROL2);
    reg_val1 |= (FSA9685_ACCP_AUTO_ENABLE | FSA9685_ACCP_ENABLE);
    fsa9685_write_reg(FSA9685_REG_CONTROL2, reg_val1);

    /*detect hisi fcp charger*/
    for(i=0; i < FCP_DETECT_MAX_COUT; i++)
    {
        reg_val1 = fsa9685_read_reg(FSA9685_REG_DEVICE_TYPE_4);
        reg_val2 = fsa9685_read_reg(FSA9685_REG_ACCP_STATUS);
        if((reg_val1 & FSA9685_ACCP_CHARGER_DET)
            && (FSA9688_ACCP_STATUS_SLAVE_GOOD == (reg_val2 & FSA9688_ACCP_STATUS_MASK )))
        {
            break;
        }
        msleep(FCP_POLL_TIME);
    }
    /*clear accp interrupt*/
    hwlog_info("%s : read ACCP interrupt,reg[0x59]=0x%x,reg[0x5A]=0x%x\n",__func__,
        fsa9685_read_reg(FSA9685_REG_ACCP_INTERRUPT1), fsa9685_read_reg(FSA9685_REG_ACCP_INTERRUPT2));
    if(FCP_DETECT_MAX_COUT == i )
    {
        mutex_unlock(&fcp_detect_lock);
        hwlog_info("not fcp adapter ,reg[0x%x]=0x%x,reg[0x%x]=0x%x \n",FSA9685_REG_DEVICE_TYPE_4,reg_val1,FSA9685_REG_ACCP_STATUS,reg_val2);
        if(reg_val1 & FSA9685_ACCP_CHARGER_DET)
        {
            return FCP_ADAPTER_DETECT_FAIL;/*fcp adapter but detect fail */
        }
        return FCP_ADAPTER_DETECT_OTHER;/*not fcp adapter*/

    }
    hwlog_info("fcp adapter detect ok,take %d ms \n",i*FCP_POLL_TIME);
    mutex_unlock(&fcp_detect_lock);
    return FCP_ADAPTER_DETECT_SUCC;

}
/**********************************************************
*  Function:       switch_dump_register
*  Discription:    print the register value
*  Parameters:   void
*  return value:  void
**********************************************************/
void switch_dump_register(void)
{
    const int regaddrs[] = {0x01, 0x02, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x19, 0x40, 0x41, 0x44, 0x47, 0x48, 0x59, 0x5A, 0x5B, 0x5C};
    int i = 0;
    u8 val = 0;
    hwlog_info("dump switch register:");
    for(i = 0;i < sizeof(regaddrs)/sizeof(int);i++)
    {
        val = fsa9685_read_reg(regaddrs[i]);
        hwlog_info("reg[%x],val:%x,",regaddrs[i],val);
        val = 0;
    }
    hwlog_info("\n");
    return ;
}

/****************************************************************************
  Function:     fcp_get_adapter_output_vol
  Description:  get fcp output vol
  Input:        NA.
  Output:       fcp output vol(5V/9V/12V)
  Return:        0: success
                -1: fail
***************************************************************************/
int fcp_get_adapter_output_vol(int *vol)
{
    int num = 0;
    int output_vol = 0;
    if(NULL == this_client)
    {
        return -1;
    }

    /*get adapter vol list number,exclude 5V*/
    fcp_adapter_reg_read(&num, FCP_SLAVE_REG_DISCRETE_CAPABILITIES);
    /*currently,fcp only support three out vol config(5v/9v/12v)*/
    if (num > 2){
        hwlog_err("%s: vout list support err, reg[0x21] = %d.\n", __func__, num);
        return -1;
    }

    /*get max out vol value*/
    fcp_adapter_reg_read(&output_vol, FCP_SLAVE_REG_DISCRETE_OUT_V(num));
    *vol = output_vol/FCP_VOL_SETP;
    hwlog_info("%s: get adapter max out vol = %d,num= %d.\n", __func__, output_vol,num);
    return 0;
}


/****************************************************************************
  Function:     fcp_set_adapter_output_vol
  Description:  set fcp adapter output vol
  Input:        NA
  Output:       NA
  Return:        0: success
                -1: fail
***************************************************************************/
int fcp_set_adapter_output_vol(void)
{
    int val = 0;
    int vol = 0;
    int i = 0;

    if(NULL == this_client)
    {
        return -1;
    }

    /*read ID OUTI , for identify huawei adapter*/
    fcp_adapter_reg_read(&val, FCP_SLAVE_REG_ID_OUT0);
    hwlog_info("%s: id out reg[0x4] = %d.\n", __func__, val);
    /*get adapter vol list number*/

    /*currently,fcp only suport two out vol config(5v/9v)*/
    /*get max out vol value*/
    for(i=0; i<FCP_RETRY_MAX_TIMES; i++)
    {
        if(0 == fcp_get_adapter_output_vol(&vol))
        {
            break;
        }
    }
    if(FCP_RETRY_MAX_TIMES == i)
    {
        hwlog_err("%s: fcp get output vol err.\n", __func__);
        return -1;
    }

    /* PLK only support 5V/9V */
    if(vol > FCP_OUTPUT_VOL_9V)
    {
        vol = FCP_OUTPUT_VOL_9V;
    }
    vol *=FCP_VOL_SETP;
    /*try 3 times if write fail */
    for (i=0; i<FCP_RETRY_MAX_TIMES; i++)
    {
        fcp_adapter_reg_write(vol, FCP_SLAVE_REG_VOUT_CONFIG);
        fcp_adapter_reg_read(&val, FCP_SLAVE_REG_VOUT_CONFIG);
        hwlog_info("%s: vout config reg[0x2c] = %d.\n", __func__, val);
        if (val == vol){
            break;
        }
    }
    /*if try 3 times return fail */
    if (FCP_RETRY_MAX_TIMES == i){
        hwlog_err("%s:out vol config err, reg[0x2c] = %d.\n", __func__, val);
        return -1;
    }
    fcp_adapter_reg_write(FCP_SLAVE_SET_VOUT, FCP_SLAVE_REG_OUTPUT_CONTROL);
    hwlog_info("fcp adapter output vol set ok.\n");
    return 0;
}

/****************************************************************************
  Function:     fcp_get_adapter_max_power
  Description:  get fcp adpter max power
  Input:        NA.
  Output:       NA
  Return:       MAX POWER(W)
***************************************************************************/
int fcp_get_adapter_max_power(void)
{
    int reg_val = 0;
    if(NULL == this_client)
    {
        return -1;
    }

    /*read max power*/
    fcp_adapter_reg_read(&reg_val, FCP_SLAVE_REG_MAX_PWR);
    hwlog_info("%s: max power reg[0x22] = %d.\n", __func__, reg_val);
    return (reg_val >> 1);
}

/**********************************************************
*  Function:       fcp_get_adapter_output_current
*  Discription:    fcp get the output current from adapter max power and output vol
*  Parameters:     NA
*  return value:  input_current(MA)
**********************************************************/
int fcp_get_adapter_output_current(void)
{
    int output_current = 0;
    int output_vol     = 0;
    if(NULL == this_client)
    {
        return -1;
    }

    fcp_get_adapter_output_vol(&output_vol);
    if (output_vol){
        output_current = fcp_get_adapter_max_power()*1000/output_vol;
    }
    hwlog_info("%s: output current = %d.\n", __func__, output_current);
    return output_current;
}

/**********************************************************
*  Function:       is_support_fcp
*  Discription:    check whether support fcp
*  Parameters:     NA
*  return value:   0:support
                  -1:not support
**********************************************************/
int is_support_fcp(void)
{
    int reg_val = 0;
    static int flag_result = -EINVAL;
    if(NULL == this_client)
    {
        return -1;
    }

    if(flag_result != -EINVAL)
    {
        return flag_result;
    }
    reg_val = fsa9685_read_reg(FSA9685_REG_DEVICE_ID);
    if (FSA9688_VERSION_ID != ((reg_val & FAS9685_VERSION_ID_BIT_MASK) >> FAS9685_VERSION_ID_BIT_SHIFT)){
        hwlog_err("%s:no fsa9688,no support fcp, reg[0x1]=%d.\n", __func__, reg_val);
        flag_result = -1;
    }
    else
    {
        flag_result = 0;
    }

    return flag_result;
}
/**********************************************************
*  Function:       fcp_switch_status_check
*  Discription:    when in fcp status ,it will check the reg status of 9685
*  Parameters:     NA
*  return value:   -1:status error 0:status ok
**********************************************************/
int fcp_read_switch_status(void)
{
    int reg_val1 = 0;
    /*check accp status*/
    reg_val1 = fsa9685_read_reg(FSA9685_REG_ACCP_STATUS);
    if((FSA9688_ACCP_STATUS_SLAVE_GOOD != (reg_val1 & FSA9688_ACCP_STATUS_MASK )))
    {
         return -1;
    }
    return 0;
}
/**********************************************************
*  Function:       fcp_adapter_status_check
*  Discription:    when in fcp status ,it will check adapter reg status
*  Parameters:     NA
*  return value: 0:status ok ;FCP_ADAPTER_OTEMP:over temp;FCP_ADAPTER_OCURRENT: over current;FCP_ADAPTER_OVLT: over ovl;
**********************************************************/
int fcp_read_adapter_status (void)
{
    int val = 0;
    fcp_adapter_reg_read(&val, FCP_ADAPTER_STATUS);
    if( FCP_ADAPTER_OVLT == (val & FCP_ADAPTER_OVLT))
    {
       return FCP_ADAPTER_OVLT;
    }

    if( FCP_ADAPTER_OCURRENT == (val & FCP_ADAPTER_OCURRENT))
    {
        return FCP_ADAPTER_OCURRENT;
    }

    if( FCP_ADAPTER_OTEMP == (val & FCP_ADAPTER_OTEMP))
    {
        return FCP_ADAPTER_OTEMP;
    }
    return 0;
}

struct fcp_adapter_device_ops fcp_fsa9688_ops = {
    .get_adapter_output_current = fcp_get_adapter_output_current,
    .set_adapter_output_vol     = fcp_set_adapter_output_vol,
    .detect_adapter             = fcp_adapter_detect,
    .is_support_fcp             = is_support_fcp,
};

/**********************************************************
*  Function:       fcp_mmi_show
*  Discription:    file node for mmi testing fast charge protocol
*  Parameters:     NA
*  return value:   0:success
*                  1:fail
*                  2:not support
**********************************************************/
static ssize_t fcp_mmi_show(struct device *dev,struct device_attribute *attr, char *buf)
{
    int result = FCP_TEST_FAIL;
    enum hisi_charger_type type = hisi_get_charger_type();

    /* judge whether support fcp */
    if(is_support_fcp())
    {
        result = FCP_NOT_SUPPORT;
        return snprintf(buf,PAGE_SIZE,"%d\n",result);
    }
    /*to avoid the usb port cutoff when CTS test*/
    if ((type == CHARGER_TYPE_SDP) || (type == CHARGER_TYPE_CDP))
    {
        result = FCP_TEST_FAIL;
        hwlog_err("fcp detect fail 1,charge type is %d \n",type);
        return snprintf(buf,PAGE_SIZE,"%d\n",result);
    }
    /* fcp adapter detect */
    if( fcp_adapter_detect())
    {
        /* 0:sdp 1:cdp 2:dcp 3:unknow 4:none*/
        hwlog_err("fcp detect fail 2,charge type is %d \n",fsa9685_get_charger_type());
        result = FCP_TEST_FAIL;
    }
    else
    {
        result = FCP_TEST_SUCC;
    }
    hwlog_info("%s: fcp mmi result  %d\n",__func__,result);
    return snprintf(buf,PAGE_SIZE,"%d\n",result);
}

static DEVICE_ATTR(fcp_mmi, S_IRUGO, fcp_mmi_show, NULL);

#ifdef CONFIG_OF
static const struct of_device_id switch_fsa9685_ids[] = {
    { .compatible = "huawei,fairchild_fsa9685" },
    {},
};
MODULE_DEVICE_TABLE(of, switch_fsa9685_ids);
#endif

static int fsa9685_probe(
    struct i2c_client *client, const struct i2c_device_id *id)
{
    int ret = 0, reg_ctl, gpio_value;
    struct device_node *node = client->dev.of_node;
#ifdef CONFIG_FSA9685_DEBUG_FS
    struct class *switch_class = NULL;
    struct device * new_dev = NULL;
#endif

    hwlog_info("%s: ------entry.\n", __func__);

    if (!i2c_check_functionality(client->adapter, I2C_FUNC_SMBUS_BYTE_DATA)) {
        hwlog_err("%s: i2c_check_functionality error!!!\n", __func__);
        ret = -ERR_NO_DEV;
        this_client = NULL;
        goto err_i2c_check_functionality;
    }
    this_client = client;

#ifdef CONFIG_FSA9685_DEBUG_FS
    ret = device_create_file(&client->dev, &dev_attr_dump_regs);
    if (ret < 0) {
        hwlog_err("%s: device_create_file error!!! ret=%d.\n", __func__, ret);
        ret = -ERR_SWITCH_USB_DEV_REGISTER;
        goto err_i2c_check_functionality;
    }
#endif
/*create a node for phone-off current drain test*/
    ret = device_create_file(&client->dev, &dev_attr_switchctrl);
    if (ret < 0) {
        hwlog_err("%s: device_create_file error!!! ret=%d.\n", __func__, ret);
        ret = -ERR_SWITCH_USB_DEV_REGISTER;
        goto err_get_named_gpio;
    }

    mutex_init(&fcp_detect_lock);
    ret = device_create_file(&client->dev, &dev_attr_fcp_mmi);
    if (ret < 0) {
        hwlog_err("%s: device_create_file error!!! ret=%d.\n", __func__, ret);
        ret = -ERR_SWITCH_USB_DEV_REGISTER;
        goto err_create_fcp_mmi_failed;
    }

    switch_class = class_create(THIS_MODULE, "usb_switch");
    if (IS_ERR(switch_class)) {
        hwlog_err("%s:create switch class failed!\n", __func__);
        goto err_create_link_failed;
    }
    new_dev = device_create(switch_class, NULL, 0, NULL, "switch_ctrl");
    if (NULL == new_dev) {
        hwlog_err("%s:device create failed!\n", __func__);
        goto err_create_link_failed;
    }
    ret = sysfs_create_link(&new_dev->kobj, &client->dev.kobj, "manual_ctrl");
    if (ret < 0) {
        hwlog_err("%s:create link to switch failed!\n", __func__);
        goto err_create_link_failed;
    }
/*create link end*/
    gpio = of_get_named_gpio(node, "fairchild_fsa9685,gpio-intb", 0);
    if (gpio < 0) {
        hwlog_err("%s: of_get_named_gpio error!!! ret=%d, gpio=%d.\n", __func__, ret, gpio);
        ret = -ERR_OF_GET_NAME_GPIO;
        goto err_get_named_gpio;
    }

    client->irq = gpio_to_irq(gpio);

    if (client->irq < 0) {
        hwlog_err("%s: gpio_to_irq error!!! ret=%d, gpio=%d, client->irq=%d.\n", __func__, ret, gpio, client->irq);
        ret = -ERR_GPIO_TO_IRQ;
        goto err_get_named_gpio;
    }

    ret = gpio_request(gpio, "fsa9685_int");
    if (ret < 0) {
        hwlog_err("%s: gpio_request error!!! ret=%d. gpio=%d.\n", __func__, ret, gpio);
        ret = -ERR_GPIO_REQUEST;
        goto err_get_named_gpio;
    }

    ret = gpio_direction_input(gpio);
    if (ret < 0) {
        hwlog_err("%s: gpio_direction_input error!!! ret=%d. gpio=%d.\n", __func__, ret, gpio);
        ret = -ERR_GPIO_DIRECTION_INPUT;
        goto err_gpio_direction_input;
    }

    INIT_DELAYED_WORK(&detach_delayed_work, fsa9685_detach_work);
    ret = fsa9685_write_reg(FSA9685_REG_DETACH_CONTROL, 1);
    if ( ret < 0 ){
        hwlog_err("%s: write FSA9685_REG_DETACH_CONTROL error!!! ret=%d", __func__, ret);
    }

    /* interrupt register */
    INIT_WORK(&g_intb_work, fsa9685_intb_work);

    ret = request_irq(client->irq,
               fsa9685_irq_handler,
               IRQF_NO_SUSPEND | IRQF_TRIGGER_FALLING,
               "fsa9685_int", client);
    if (ret < 0) {
        hwlog_err("%s: request_irq error!!! ret=%d.\n", __func__, ret);
        ret = -ERR_REQUEST_THREADED_IRQ;
        goto err_gpio_direction_input;
    }
    /* clear INT MASK */
    reg_ctl = fsa9685_read_reg(FSA9685_REG_CONTROL);
    if ( reg_ctl < 0 ) {
        hwlog_err("%s: read FSA9685_REG_CONTROL error!!! reg_ctl=%d.\n", __func__, reg_ctl);
        goto err_gpio_direction_input;
    }
    hwlog_info("%s: read FSA9685_REG_CONTROL. reg_ctl=0x%x.\n", __func__, reg_ctl);

    reg_ctl &= (~FSA9685_INT_MASK);
    ret = fsa9685_write_reg(FSA9685_REG_CONTROL, reg_ctl);
    if ( ret < 0 ) {
        hwlog_err("%s: write FSA9685_REG_CONTROL error!!! reg_ctl=%d.\n", __func__, reg_ctl);
        goto err_gpio_direction_input;
    }
    hwlog_info("%s: write FSA9685_REG_CONTROL. reg_ctl=0x%x.\n", __func__, reg_ctl);

    ret = fsa9685_write_reg(FSA9685_REG_DCD, 0x0c);
    if ( ret < 0 ) {
        hwlog_err("%s: write FSA9685_REG_DCD error!!! reg_DCD=0x%x.\n", __func__, 0x08);
        goto err_gpio_direction_input;
    }
    hwlog_info("%s: write FSA9685_REG_DCD. reg_DCD=0x%x.\n", __func__, 0x0c);

    gpio_value = gpio_get_value(gpio);
    hwlog_info("%s: intb=%d after clear MASK.\n", __func__, gpio_value);

    if (gpio_value == 0) {
        schedule_work(&g_intb_work);
    }

    /* if chip support fcp ,register fcp adapter ops */
    if( 0 == is_support_fcp() && 0 ==fcp_adapter_ops_register(&fcp_fsa9688_ops))
    {
        hwlog_info(" fcp adapter ops register success!\n");
    }

#ifdef CONFIG_HUAWEI_HW_DEV_DCT
    /* detect current device successful, set the flag as present */
    set_hw_dev_flag(DEV_I2C_USB_SWITCH);
#endif
    hwlog_info("%s: ------end. ret = %d.\n", __func__, ret);
    return ret;

err_gpio_direction_input:
    gpio_free(gpio);
err_create_link_failed:
    device_remove_file(&client->dev, &dev_attr_fcp_mmi);
err_create_fcp_mmi_failed:
    device_remove_file(&client->dev, &dev_attr_switchctrl);
err_get_named_gpio:
    device_remove_file(&client->dev, &dev_attr_dump_regs);
err_i2c_check_functionality:
    this_client = NULL;

    hwlog_err("%s: ------FAIL!!! end. ret = %d.\n", __func__, ret);
    return ret;
}

static int fsa9685_remove(struct i2c_client *client)
{
    device_remove_file(&client->dev, &dev_attr_dump_regs);
    device_remove_file(&client->dev, &dev_attr_switchctrl);
    free_irq(client->irq, client);
    gpio_free(gpio);
    return 0;
}

static const struct i2c_device_id fsa9685_i2c_id[] = {
    { "fsa9685", 0 },
    { }
};

static struct i2c_driver fsa9685_i2c_driver = {
    .driver = {
        .name = "fsa9685",
        .owner = THIS_MODULE,
        .of_match_table = of_match_ptr(switch_fsa9685_ids),
    },
    .probe    = fsa9685_probe,
    .remove   = fsa9685_remove,
    .id_table = fsa9685_i2c_id,
};

static __init int fsa9685_i2c_init(void)
{
    int ret = 0;
    hwlog_info("%s: ------entry.\n", __func__);
    ret = i2c_add_driver(&fsa9685_i2c_driver);
    if(ret)
        hwlog_err("%s: i2c_add_driver error!!!\n", __func__);

    hwlog_info("%s: ------end.\n", __func__);
    return ret;
}

static __exit void fsa9685_i2c_exit(void)
{
    i2c_del_driver(&fsa9685_i2c_driver);
}

module_init(fsa9685_i2c_init);
module_exit(fsa9685_i2c_exit);

MODULE_AUTHOR("Lixiuna<lixiuna@huawei.com>");
MODULE_DESCRIPTION("I2C bus driver for FSA9685 USB Accesory Detection Switch");
MODULE_LICENSE("GPL v2");
