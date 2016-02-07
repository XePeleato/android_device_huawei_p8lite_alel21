/*
 * cbp_spi.c
 *
 * VIA CBP SPI driver for Linux
 *
 * Copyright (C) 2009 VIA TELECOM Corporation, Inc.
 * Author: VIA TELECOM Corporation, Inc.
 *
 * This package is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * THIS PACKAGE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

#include <linux/module.h>
#include <linux/mod_devicetable.h>
#include <linux/platform_device.h>
#include <linux/irq.h>
#include <linux/circ_buf.h>
#include <linux/tty.h>
#include <linux/tty_flip.h>
#include <linux/kfifo.h>
#include <linux/slab.h>

#include <linux/delay.h>
#include <linux/timer.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/mutex.h>
#include <linux/mmc/host.h>
#include <linux/gpio.h>
#include <linux/wait.h>
#include <linux/of_gpio.h>
#include "../viatel.h"
#include "modem_spi.h"

#define VIACBP82D_CBP_SPI_DRIVER_NAME  "huawei,viacbp82d-cbp-spi"
#define MAX_WAIT_DATA_ACK_TIMES    (1)
#define WAIT_DATA_ACK_TIMEOUT_ONCE_IN_MS    (2000)
#define MAX_WAIT_HARDWARE_FLOW_CTRL_TIMES    (1)
#define WAIT_HARD_FLOW_CTRL_TIMEOUT_ONCE_IN_MS    (2000)

#define DRIVER_NAME "cbp"
unsigned char cbp_power_state = 0;
static atomic_t cbp_rst_ind_finished = ATOMIC_INIT(0);
extern void modem_reset_handler(void);
extern int respond_cflag80_packet_to_via_request_to_send(void);
extern void spi_tx_current_packet_print(void);
extern void spi_tx_last_packet_print(void);
extern void spi_rx_last_packet_print(void);
extern void spi_sflow_ctrl_disable(void);

/*----------------------- cbp platform driver ------------------------*/
static struct cbp_platform_data cbp_data = {
    .bus            =    "spi",
    .host_id        =    MDM_SPI_ID,
    .ipc_enable        =    false,
    .data_ack_enable    =    false,
    .flow_ctrl_enable    =    false,
    .tx_disable_irq        =    true,

    .gpio_sync_polar    =    GPIO_VIATEL_SPI_SYNC_POLAR,
    .gpio_data_ack_polar     =    GPIO_VIATEL_SPI_DATA_ACK_POLAR,
    .gpio_flow_ctrl_polar    =    GPIO_VIATEL_SPI_FLOW_CTRL_POLAR,
    .gpio_via_requst_to_send_polar    =    GPIO_VIATEL_SPI_VIA_REQUEST_TO_SEND_POLAR,

    .cbp_setup        =    modem_spi_init,
    .cbp_destroy        =    modem_spi_exit,
};

/*----------------------IPC functions-------------------*/
static int modem_spi_tx_notifier(int event, void *data);
static int modem_spi_rx_notifier(int event, void *data);

static struct asc_config spi_tx_handle ={
    .name = CBP_TX_HD_NAME,
};
static struct asc_infor spi_tx_user ={
    .name = CBP_TX_USER_NAME,
    .data = &spi_tx_handle,
    .notifier = modem_spi_tx_notifier,
};

static struct asc_config spi_rx_handle = {
    .name = SPI_RX_HD_NAME,
};
static struct asc_infor spi_rx_user = {
    .name = SPI_RX_USER_NAME,
    .data = &spi_rx_handle,
    .notifier = modem_spi_rx_notifier,
};


/*----------------------data_ack functions-------------------*/
static struct cbp_wait_event *cbp_data_ack = NULL;

static irqreturn_t gpio_irq_via_spi_tx_rx_fifo_empty_ack(int irq, void *data)
{
    struct cbp_wait_event *cbp_data_ack = (struct cbp_wait_event *)data;
    int level;
    int ret = -1;

    ret = irq_set_irq_type(irq, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING);

    level = !!oem_gpio_get_value(cbp_data_ack->wait_gpio);
    hwlog_debug("%s enter, level = %d.\n", __func__, level);

    atomic_set(&cbp_data_ack->state, MODEM_ST_READY);
    wake_up(&cbp_data_ack->wait_q);

    oem_gpio_irq_unmask(cbp_data_ack->wait_gpio);

    return IRQ_HANDLED;
}

static int data_ack_wait_event(struct cbp_wait_event *pdata_ack)
{
    struct cbp_wait_event *cbp_data_ack = (struct cbp_wait_event *)pdata_ack;
    long remain_time;
    unsigned int failed_times = 0;
       int tx_wake = oem_gpio_get_value(spi_tx_handle.gpio_wake);
       int tx_ready = oem_gpio_get_value(spi_tx_handle.gpio_ready);
       int rx_wake = oem_gpio_get_value(spi_rx_handle.gpio_wake);
       int rx_ready = oem_gpio_get_value(spi_rx_handle.gpio_ready);
       int Rts = oem_gpio_get_value(cbp_data.gpio_via_requst_to_send);

    do {
        remain_time = wait_event_timeout(cbp_data_ack->wait_q, ((MODEM_ST_READY == atomic_read(&cbp_data_ack->state))||
            (cbp_power_state==0)), msecs_to_jiffies(WAIT_DATA_ACK_TIMEOUT_ONCE_IN_MS));
        if (0 == cbp_power_state) {
            hwlog_err("%s: VIA CBP reset happened!.\n", __func__);
            return -2;
        } else if(0 == remain_time) {
            hwlog_err("%s: wait CBP response data ack timeout 2s. data ack:%d->%d, AP2CP:%d->%d, CP_ready:%d->%d, CP2AP:%d->%d, AP_ready:%d->%d, Rts:%d->%d", __func__, \
                                        cbp_data.saved_last_data_ack_gpio_level, oem_gpio_get_value(cbp_data_ack->wait_gpio), \
                                        tx_wake, oem_gpio_get_value(spi_tx_handle.gpio_wake), \
                                        tx_ready, oem_gpio_get_value(spi_tx_handle.gpio_ready), \
                                        rx_wake, oem_gpio_get_value(spi_rx_handle.gpio_wake),  \
                                        rx_ready, oem_gpio_get_value(spi_rx_handle.gpio_ready), \
                                        Rts, oem_gpio_get_value(cbp_data.gpio_via_requst_to_send));
            failed_times++;
            if (failed_times >= MAX_WAIT_DATA_ACK_TIMES) {
                return -1;
            }
        }
    } while (0 == remain_time);
        cbp_data.saved_last_data_ack_gpio_level = oem_gpio_get_value(cbp_data_ack->wait_gpio);
    return 0;
}

/*----------------------flow control functions-------------------*/
unsigned long long hr_t1=0;
unsigned long long hr_t2=0;

static struct cbp_wait_event *cbp_flow_ctrl = NULL;

static irqreturn_t gpio_irq_flow_ctrl(int irq, void *data)
{
    struct cbp_wait_event *cbp_flow_ctrl = (struct cbp_wait_event *)data;
    int level;

    level = !!oem_gpio_get_value(cbp_flow_ctrl->wait_gpio);
    oem_gpio_irq_unmask(cbp_flow_ctrl->wait_gpio);

    if(level == cbp_flow_ctrl->wait_polar){
        atomic_set(&cbp_flow_ctrl->state, FLOW_CTRL_ENABLE);
    }
    else{
        atomic_set(&cbp_flow_ctrl->state, FLOW_CTRL_DISABLE);
        wake_up(&cbp_flow_ctrl->wait_q);
    }

    return IRQ_HANDLED;
}

static int flow_ctrl_wait_event(struct cbp_wait_event *pflow_ctrl)
{
    struct cbp_wait_event *cbp_flow_ctrl = (struct cbp_wait_event *)pflow_ctrl;
    long remain_time;
    unsigned int wait_hardware_flow_ctrl_times = 0;

    do {
        remain_time = wait_event_timeout(cbp_flow_ctrl->wait_q, (FLOW_CTRL_DISABLE == atomic_read(&cbp_flow_ctrl->state)||(cbp_power_state==0)),msecs_to_jiffies(2000));
        if (0 == cbp_power_state) {
            hwlog_err("%s: VIA CBP reset happened!.\n", __func__);
            return -2;
        } else if(0 == remain_time) {
            hwlog_err("%s: wait CBP hardware flow control disable timeout in 2s. ctrl value = %d\n", __func__, oem_gpio_get_value(cbp_flow_ctrl->wait_gpio));
            wait_hardware_flow_ctrl_times++;
            if (wait_hardware_flow_ctrl_times >= MAX_WAIT_HARDWARE_FLOW_CTRL_TIMES) {
                return -1;
            }
        }
    } while (0 == remain_time);

    return 0;
}

static irqreturn_t gpio_irq_via_rts(int irq, void *data)
{
    hwlog_debug("gpio_irq_via_rts entered, cbp_rst_ind_finished = %d\n", atomic_read(&cbp_rst_ind_finished));
    if(0 == atomic_read(&cbp_rst_ind_finished)) {
        hwlog_err("unexpected rts interrupt before cbp reset indication finished.\n");
        return IRQ_HANDLED;
    }

    respond_cflag80_packet_to_via_request_to_send();

    return IRQ_HANDLED;
}


static int modem_spi_tx_notifier(int event, void *data)
{
    return 0;
}
#ifdef WAKE_HOST_BY_SYNC/*wake up spi host by four wire sync mechanis*/
extern void VIA_trigger_signal(int i_on);
#endif

static int modem_spi_rx_notifier(int event, void *data)
{
    struct asc_config *rx_config  = (struct asc_config *)data;
    int ret = 0;
    hwlog_debug("%s event=%d\n", __func__,event);
    switch(event){
        case ASC_NTF_RX_PREPARE:
#ifdef WAKE_HOST_BY_SYNC/*wake up spi host by four wire sync mechanis*/
            if(cbp_power_state){
                wait_event(cbp_data.wait_spi_resume_q, ((ON_RESUME == atomic_read(&cbp_data.spi_resumed)) \
                    || (0 == cbp_power_state)));
            }
            else
                hwlog_err("ignor asc event to resume spi host\n");
#endif
            asc_rx_confirm_ready(rx_config->name, 1);
            break;
        case ASC_NTF_RX_POST:
#ifdef WAKE_HOST_BY_SYNC/*wake up spi host by four wire sync mechanis*/
            if(cbp_power_state){
                VIA_trigger_signal(0);
            }
            else
                hwlog_err("ignor asc event to suspend spi host\n");
#endif
            break;
        default:
            hwlog_err("%s: ignor unknow evernt!!\n", __func__);
            break;
    }
    return ret;
}


/*if you need to wake spi host and system up by using via's four wire sync mechanis,
*  please define WAKE_HOST_BY_SYNC  in Makefile;
*  WAKE_HOST_BY_SYNC is defined in default.
*/

void gpio_irq_cbp_rst_ind(int level)
{
    hwlog_info("%s %d set cbp_power_state=%d. \n",__func__,__LINE__, cbp_power_state);
    atomic_set(&cbp_rst_ind_finished, 1);
    if (level ) { /*1:cbp reset happened*/
        cbp_power_state = 0;
            spi_sflow_ctrl_disable();
        hwlog_info("%s %d set cbp_power_state=%d.\n",__func__,__LINE__, cbp_power_state);
        wake_up(&cbp_data_ack->wait_q);
        wake_up(&cbp_flow_ctrl->wait_q);
    } else {
        cbp_power_state = 1;
        hwlog_info("%s %d set cbp_power_state=%d.\n",__func__,__LINE__, cbp_power_state);
    }
}
EXPORT_SYMBOL(gpio_irq_cbp_rst_ind);

/*----------------------cbp sys interface --------------------------*/
#if 0
static void sys_power_on_cbp(void)
{
    oem_gpio_direction_output(cbp_rst_gpio, 0);
    oem_gpio_direction_output(cbp_pwr_en_gpio, 0);

    oem_gpio_direction_output(cbp_rst_gpio, 1);
    oem_gpio_direction_output(cbp_pwr_en_gpio, 1);
    msleep(400);

    oem_gpio_direction_output(cbp_rst_gpio, 0); //MDM_RST
}


static void sys_power_off_cbp(void)
{
    oem_gpio_direction_output(cbp_rst_gpio, 0);
    oem_gpio_direction_output(cbp_pwr_en_gpio, 0);
    oem_gpio_direction_output(cbp_rst_gpio, 1);
    msleep(500);
    msleep(600);
    oem_gpio_direction_output(cbp_rst_gpio, 0);
    atomic_set(&cbp_rst_ind_finished, 0);
}

static void sys_reset_cbp(void)
{
    oem_gpio_direction_output(cbp_pwr_en_gpio, 1);
    msleep(10);
    oem_gpio_direction_output(cbp_rst_gpio, 1);
    msleep(100);
    msleep(300);
    oem_gpio_direction_output(cbp_rst_gpio, 0); //MDM_RST
    atomic_set(&cbp_rst_ind_finished, 0);
}
#endif

static ssize_t cbp_power_show(struct kobject *kobj, struct kobj_attribute *attr,
                 char *buf)
{
    return snprintf(buf, PAGE_SIZE, "%d\n", cbp_power_state);
}

static ssize_t cbp_power_store(struct kobject *kobj, struct kobj_attribute *attr,
               const char *buf, size_t n)
{
    int val;

    if (kstrtoint(buf, 10, &val))
        return -EINVAL;

    if (val < 0)
        return -EINVAL;

    if (val){
        if(cbp_power_state == 0){
            //sys_power_on_cbp();
            hwlog_debug("AP power on CBP.\n");
        }
        else{
            hwlog_err("%s: CBP is already power on.\n", __func__);
        }
    }
    else{
        if(cbp_power_state == 1){
            //sys_power_off_cbp();
            hwlog_debug("AP power off CBP.\n");
            cbp_power_state = 0;
        }
        else{
            hwlog_err("%s: CBP is already power off.\n", __func__);
        }
    }

    return n;
}

static ssize_t cbp_reset_show(struct kobject *kobj, struct kobj_attribute *attr,
                 char *buf)
{
    return 0;
}

static ssize_t cbp_reset_store(struct kobject *kobj, struct kobj_attribute *attr,
               const char *buf, size_t n)
{
    int val;

    if (kstrtoint(buf, 10, &val))
        return -EINVAL;

    if (val < 0)
        return -EINVAL;

    if (val){
        //sys_reset_cbp();
        hwlog_debug("AP reset CBP.\n");
    }
    else{
        hwlog_err("%s: reset cbp use value 1.\n", __func__);
    }

    return n;
}

#define cbp_attr(_name) \
static struct kobj_attribute _name##_attr = {    \
    .attr    = {                \
        .name = __stringify(_name),    \
        .mode = 0660,            \
    },                    \
    .show    = cbp_##_name##_show,            \
    .store    = cbp_##_name##_store,        \
}
cbp_attr(power);
cbp_attr(reset);

static struct attribute * cbp_power_attr[] = {
    &power_attr.attr,
    &reset_attr.attr,
    NULL,
};

static struct kobject *cbp_power_kobj;
static struct attribute_group g_power_attr_group = {
    .attrs = cbp_power_attr,
};

static int cbp_probe(struct platform_device *pdev)
{
    struct cbp_platform_data *plat = NULL;
    int ret = -1;
    struct device *dev = &pdev->dev;
    struct device_node *np = dev->of_node;

    ret = platform_device_add_data(pdev, &cbp_data, sizeof(struct cbp_platform_data));
    if (ret < 0) {
        hwlog_err("%s: add platform_data failed!\n", __func__);
        goto err_add_data_to_platform_device;
    }

    plat = &cbp_data;//pdev->dev.platform_data; //for platform_data is a memory copy of cbp_data, and not used now

    plat->gpio_data_ack = of_get_named_gpio(np, "via_data_ack", 0);
    hwlog_info(">>>> %s get via_data_ack gpio %d\n", __func__, plat->gpio_data_ack);
    if(GPIO_OEM_VALID(plat->gpio_data_ack)){
        cbp_data_ack = kzalloc(sizeof(struct cbp_wait_event), GFP_KERNEL);
        if (!cbp_data_ack){
            ret = -ENOMEM;
            hwlog_err("%s %d kzalloc cbp_data_ack failed \n",__func__, __LINE__);
            goto err_kzalloc_cbp_data_ack;
        }

        init_waitqueue_head(&cbp_data_ack->wait_q);
        atomic_set(&cbp_data_ack->state, MODEM_ST_UNKNOW);
        cbp_data_ack->wait_gpio = plat->gpio_data_ack;
        cbp_data_ack->wait_polar = plat->gpio_data_ack_polar;
        hwlog_info("cbp_data_ack->wait_gpio=%d\n",cbp_data_ack->wait_gpio);
        hwlog_info("cbp_data_ack->wait_polar=%d\n",cbp_data_ack->wait_polar);
        ret = oem_gpio_request(plat->gpio_data_ack, DRIVER_NAME "(data_ack)");
        if(ret < 0){
            hwlog_err("%s: %d fail to requset data_ack gpio %d ret =%d!!\n",
                __func__, __LINE__, plat->gpio_data_ack, ret);
            goto err_req_data_ack;
        }
        oem_gpio_irq_mask(plat->gpio_data_ack);
        oem_gpio_direction_input_for_irq(plat->gpio_data_ack);
        //on powerup, there is ack low level to high level jump.
        oem_gpio_set_irq_type(plat->gpio_data_ack, IRQF_TRIGGER_FALLING);
        ret = oem_gpio_request_irq(plat->gpio_data_ack, gpio_irq_via_spi_tx_rx_fifo_empty_ack,
                IRQF_SHARED | IRQF_TRIGGER_FALLING, DRIVER_NAME "(data_ack)", cbp_data_ack);
        oem_gpio_irq_unmask(plat->gpio_data_ack);
        if (ret < 0) {
            hwlog_err("%s: %d fail to request irq for data_ack!!\n", __func__, __LINE__);
            goto err_req_irq_data_ack;
        }
        plat->cbp_data_ack = cbp_data_ack;
        plat->data_ack_wait_event = data_ack_wait_event;
        plat->data_ack_enable =true;
                plat->saved_last_data_ack_gpio_level = oem_gpio_get_value(plat->gpio_data_ack);
                plat->gpio_data_ack_irq_count = 0;
    }

    plat->gpio_flow_ctrl = of_get_named_gpio(np, "via_flow_ctrl", 0);
    hwlog_info(">>>> %s get via_flow_ctrl gpio %d\n", __func__, plat->gpio_flow_ctrl);
    if(GPIO_OEM_VALID(plat->gpio_flow_ctrl)){
        cbp_flow_ctrl = kzalloc(sizeof(struct cbp_wait_event), GFP_KERNEL);
        if (!cbp_flow_ctrl){
            ret = -ENOMEM;
            hwlog_err("%s %d kzalloc cbp_flow_ctrl failed \n",__func__, __LINE__);
            goto err_kzalloc_cbp_flow_ctrl;
        }

        init_waitqueue_head(&cbp_flow_ctrl->wait_q);
        atomic_set(&cbp_flow_ctrl->state, FLOW_CTRL_DISABLE);
        cbp_flow_ctrl->wait_gpio = plat->gpio_flow_ctrl;
        cbp_flow_ctrl->wait_polar = plat->gpio_flow_ctrl_polar;
        hwlog_info("cbp_flow_ctrl->wait_gpio=%d\n",cbp_flow_ctrl->wait_gpio);
        hwlog_info("cbp_flow_ctrl->wait_polar=%d\n",cbp_flow_ctrl->wait_polar);
        ret = oem_gpio_request(plat->gpio_flow_ctrl, DRIVER_NAME "(flow_ctrl)");
        if(ret < 0){
            hwlog_err("%s: %d fail to requset flow_ctrl gpio %d ret =%d!!\n",
                __func__, __LINE__, plat->gpio_flow_ctrl, ret);
            goto err_req_flow_ctrl;
        }
        oem_gpio_irq_mask(plat->gpio_flow_ctrl);
        oem_gpio_direction_input_for_irq(plat->gpio_flow_ctrl);
        oem_gpio_set_irq_type(plat->gpio_flow_ctrl, IRQF_TRIGGER_FALLING);
        ret = oem_gpio_request_irq(plat->gpio_flow_ctrl, gpio_irq_flow_ctrl,
                IRQF_SHARED | IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING,
                DRIVER_NAME "(flow_ctrl)", cbp_flow_ctrl);
        oem_gpio_irq_unmask(plat->gpio_flow_ctrl);
        if (ret < 0) {
            hwlog_err("%s: %d fail to request irq for flow_ctrl!!\n", __func__, __LINE__);
            goto err_req_irq_flow_ctrl;
        }

        plat->cbp_flow_ctrl= cbp_flow_ctrl;
        plat->flow_ctrl_wait_event = flow_ctrl_wait_event;
        plat->flow_ctrl_enable = true;
    }

    plat->gpio_via_requst_to_send = of_get_named_gpio(np, "via_requst_to_send", 0);
    hwlog_info(">>>> %s get via_requst_to_send gpio %d\n", __func__, plat->gpio_via_requst_to_send);
    if(GPIO_OEM_VALID(plat->gpio_via_requst_to_send)){
        ret = oem_gpio_request(plat->gpio_via_requst_to_send, DRIVER_NAME "(via_requst_to_send)");
        if(ret < 0){
            hwlog_err("%s: %d fail to requset flow_ctrl gpio %d ret =%d!!\n",
                __func__, __LINE__, plat->gpio_via_requst_to_send, ret);
            goto err_req_via_requst_to_send;
        }
        oem_gpio_irq_mask(plat->gpio_via_requst_to_send);
        oem_gpio_direction_input_for_irq(plat->gpio_via_requst_to_send);
        oem_gpio_set_irq_type(plat->gpio_via_requst_to_send, IRQF_TRIGGER_FALLING);
        ret = oem_gpio_request_irq(plat->gpio_via_requst_to_send, gpio_irq_via_rts,
                IRQF_SHARED | IRQF_TRIGGER_RISING,
                DRIVER_NAME "(via_requst_to_send)", &cbp_data); //api request the fifth argu NOT NULL.
        oem_gpio_irq_unmask(plat->gpio_via_requst_to_send);
        if (ret < 0) {
            hwlog_err("%s: %d fail to request irq for via_requst_to_send!!\n", __func__, __LINE__);
            goto err_req_irq_via_requst_to_send;
        }
    }

    if (of_find_property(np, "no_hi6402_irq", NULL))
        atomic_set(&cbp_rst_ind_finished, 1);
    else
        atomic_set(&cbp_rst_ind_finished, 0);

    plat->gpio_ap_wkup_cp= of_get_named_gpio(np, "via_ap_wkup_cp", 0);
    hwlog_info(">>>> %s get ap_wkup_cp gpio %d\n", __func__, plat->gpio_ap_wkup_cp);

    plat->gpio_cp_ready= of_get_named_gpio(np, "via_cp_ready", 0);
    hwlog_info(">>>> %s get cp_ready gpio %d\n", __func__, plat->gpio_cp_ready);

    plat->gpio_cp_wkup_ap= of_get_named_gpio(np, "via_cp_wkup_ap", 0);
    hwlog_info(">>>> %s get cp_wkup_ap gpio %d\n", __func__, plat->gpio_cp_wkup_ap);

    plat->gpio_ap_ready= of_get_named_gpio(np, "via_ap_ready", 0);
    hwlog_info(">>>> %s get ap_ready gpio %d\n", __func__, plat->gpio_ap_ready);

    init_waitqueue_head(&cbp_data.wait_spi_resume_q);
    atomic_set(&cbp_data.spi_resumed, ON_RESUME); //on power up, spi controller is powered up.
    if((GPIO_OEM_VALID(plat->gpio_ap_wkup_cp)) && (GPIO_OEM_VALID(plat->gpio_cp_ready)) &&
        (GPIO_OEM_VALID(plat->gpio_cp_wkup_ap))  && (GPIO_OEM_VALID(plat->gpio_ap_ready))){
        spi_tx_handle.gpio_wake = plat->gpio_ap_wkup_cp;
        spi_tx_handle.gpio_ready = plat->gpio_cp_ready;
        spi_tx_handle.polar = plat->gpio_sync_polar;
        ret = asc_tx_register_handle(&spi_tx_handle);
        if(ret){
            hwlog_err("%s %d asc_tx_register_handle failed.\n",__FUNCTION__,__LINE__);
            goto err_ipc;
        }
        ret = asc_tx_add_user(spi_tx_handle.name, &spi_tx_user);
        if(ret){
            hwlog_err("%s %d asc_tx_add_user failed.\n",__FUNCTION__,__LINE__);
            goto err_ipc;
        }

        spi_rx_handle.gpio_wake = plat->gpio_cp_wkup_ap;
        spi_rx_handle.gpio_ready = plat->gpio_ap_ready;
        spi_rx_handle.polar = plat->gpio_sync_polar;
        ret = asc_rx_register_handle(&spi_rx_handle);
        if(ret){
            hwlog_err("%s %d asc_rx_register_handle failed.\n",__FUNCTION__,__LINE__);
            goto err_ipc;
        }
        ret = asc_rx_add_user(spi_rx_handle.name, &spi_rx_user);
        if(ret){
            hwlog_err("%s %d asc_rx_add_user failed.\n",__FUNCTION__,__LINE__);
            goto err_ipc;
        }
        plat->ipc_enable = true;
        plat->tx_handle = &spi_tx_handle;
    }

    ret = plat->cbp_setup(plat);
    if(ret){
        hwlog_err("%s: host %s setup failed!\n", __func__, plat->host_id);
        goto err_ipc;
    }

    cbp_power_kobj = viatel_kobject_add("power");
    if (!cbp_power_kobj){
        hwlog_err("error viatel_kobject_add!\n");
        ret = -ENOMEM;
        goto err_create_kobj;
    }

    hwlog_debug(" cbp initialized on host %s successfully, bus is %s !\n", plat->host_id, plat->bus);
    return sysfs_create_group(cbp_power_kobj, &g_power_attr_group);

err_create_kobj:
    plat->cbp_destroy();
err_ipc:
    if(GPIO_OEM_VALID(plat->gpio_via_requst_to_send)){
        free_irq(oem_gpio_to_irq(plat->gpio_via_requst_to_send), NULL);
    }
err_req_irq_via_requst_to_send:
    if(GPIO_OEM_VALID(plat->gpio_via_requst_to_send)){
        oem_gpio_free(plat->gpio_via_requst_to_send);
    }
err_req_via_requst_to_send:
    if(GPIO_OEM_VALID(plat->gpio_data_ack)){
        free_irq(oem_gpio_to_irq(plat->gpio_flow_ctrl), cbp_flow_ctrl);
    }
err_req_irq_flow_ctrl:
    if(GPIO_OEM_VALID(plat->gpio_flow_ctrl)){
        oem_gpio_free(plat->gpio_flow_ctrl);
    }
err_req_flow_ctrl:
    if(GPIO_OEM_VALID(plat->gpio_flow_ctrl)){
        kfree(cbp_flow_ctrl);
    }
err_kzalloc_cbp_flow_ctrl:
    if(GPIO_OEM_VALID(plat->gpio_data_ack)){
        free_irq(oem_gpio_to_irq(plat->gpio_data_ack), cbp_data_ack);
    }
err_req_irq_data_ack:
    if(GPIO_OEM_VALID(plat->gpio_data_ack)){
        oem_gpio_free(plat->gpio_data_ack);
    }
err_req_data_ack:
    if(GPIO_OEM_VALID(plat->gpio_data_ack)){
        kfree(cbp_data_ack);
    }
err_kzalloc_cbp_data_ack:
err_add_data_to_platform_device:
    return ret;
}

static int cbp_remove(struct platform_device *pdev)
{
    struct cbp_platform_data *plat = &cbp_data; //pdev->dev.platform_data; //for platform_data is a memory copy of cbp_data, and not used now

    if(plat->data_ack_enable && (GPIO_OEM_VALID(plat->gpio_data_ack))){
        free_irq(oem_gpio_to_irq(plat->gpio_data_ack), cbp_data_ack);
        oem_gpio_free(plat->gpio_data_ack);
        kfree(cbp_data_ack);
    }

    if(plat->flow_ctrl_enable && (GPIO_OEM_VALID(plat->gpio_flow_ctrl))){
        free_irq(oem_gpio_to_irq(plat->gpio_flow_ctrl), cbp_flow_ctrl);
        oem_gpio_free(plat->gpio_flow_ctrl);
        kfree(cbp_flow_ctrl);
    }

    plat->cbp_destroy();
    sysfs_remove_group(cbp_power_kobj, &g_power_attr_group);
    kobject_put(cbp_power_kobj);
    hwlog_debug(" cbp removed on host %s, bus is %s!\n", plat->host_id, plat->bus);
    return 0;
}

static const struct of_device_id viacbp82d_cbp_spi_match_table[] = {
    {
        .compatible = VIACBP82D_CBP_SPI_DRIVER_NAME,
        .data = NULL,
    },
    {
    },
};
MODULE_DEVICE_TABLE(of, viacbp82d_cbp_spi_match_table);

static struct platform_driver cbp_driver = {
    .driver = {
        .name ="huawei,viacbp82d-cbp-spi",
        .owner = THIS_MODULE,
        .of_match_table = of_match_ptr(viacbp82d_cbp_spi_match_table),
    },
    .probe        = cbp_probe,
    .remove        = cbp_remove,
};

static int __init cbp_init(void)
{
    int ret;

    ret = platform_driver_register(&cbp_driver);
    if (ret) {
        hwlog_err("platform_driver_register failed\n");
        goto err_platform_driver_register;
    }
    return ret;
err_platform_driver_register:
    return ret;
}

late_initcall(cbp_init);

static void __exit cbp_exit(void)
{
    platform_driver_unregister(&cbp_driver);
}

module_exit(cbp_exit);

MODULE_DESCRIPTION("Via CBP SPI driver");
