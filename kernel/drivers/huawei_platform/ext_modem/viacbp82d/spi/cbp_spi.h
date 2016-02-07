/*
 * drivers/spi/cbp_spi.h
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
#ifndef CBP_SPI_H
#define CBP_SPI_H

#include <linux/init.h>
#include <linux/kernel.h>

#include "../viatel.h"

struct cbp_wait_event{
    wait_queue_head_t wait_q;
    atomic_t state;
    int wait_gpio;
    int wait_polar;
};

struct cbp_platform_data {
    char *bus;
    char *host_id;

    bool ipc_enable;
    bool data_ack_enable;
    bool flow_ctrl_enable;
    bool tx_disable_irq;
    struct asc_config *tx_handle;

    int gpio_ap_wkup_cp;
    int gpio_cp_ready;
    int gpio_cp_wkup_ap;
    int gpio_ap_ready;
    int gpio_sync_polar;
    atomic_t spi_resumed;
    wait_queue_head_t wait_spi_resume_q;

    int gpio_data_ack;
    int gpio_data_ack_polar;

    int gpio_via_requst_to_send;
    int gpio_via_requst_to_send_polar;
    int gpio_ap_abort_spi_txrx;
    int gpio_ap_abort_spi_txrx_polar;

    int gpio_flow_ctrl;
    int gpio_flow_ctrl_polar;

    int gpio_pwr_on;
    int gpio_rst;
        int saved_last_data_ack_gpio_level;
        unsigned int gpio_data_ack_irq_count;

    struct cbp_wait_event *cbp_data_ack;
    int (*data_ack_wait_event)(struct cbp_wait_event *pdata_ack);
    struct cbp_wait_event *cbp_flow_ctrl;
    int (*flow_ctrl_wait_event)(struct cbp_wait_event *pflow_ctrl);

    int (*cbp_setup)(struct cbp_platform_data *pdata);
    void (*cbp_destroy)(void);
};

typedef enum{
    MODEM_ST_READY = 0, /*modem ready*/
    MODEM_ST_TX_RX,
    MODEM_ST_UNKNOW
}data_ack_state;

typedef enum{
    FLOW_CTRL_DISABLE = 0,
    FLOW_CTRL_ENABLE
}flow_ctrl_state;
#endif
