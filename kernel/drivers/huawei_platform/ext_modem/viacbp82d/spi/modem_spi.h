/*
 * drivers/mmc/card/modem_spi.h
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
#ifndef MODEM_SPI_H
#define MODEM_SPI_H

#define SPI_TTY_NR        15    /* Number of UARTs this driver can handle */
#include "cbp_spi.h"
#include <linux/skbuff.h>

/*SPI buffer in lens*/
#define SPI_PPP_BUF_IN_MAX_NUM        100
#define SPI_ETS_BUF_IN_MAX_NUM        500
#define SPI_IFS_BUF_IN_MAX_NUM        100
#define SPI_AT_BUF_IN_MAX_NUM        100
#define SPI_PCV_BUF_IN_MAX_NUM        100
#define SPI_DEF_BUF_IN_MAX_NUM        100
#define SPI_BUF_IN_MAX_SIZE            64512     /*max buffer in size = 63K*/

#define MODEM_FC_PRINT_MAX            3

struct spi_msg_head
{
    unsigned char start_flag;       /*start flag(1), little endian*/
    unsigned char chanInfo;        /*channel id*/
    unsigned char tranHi;           /*len High byte, little endian*/
    unsigned char tranLow;          /*len low byte, little endian*/
};

struct spi_msg
{
    struct spi_msg_head  head;
    unsigned char buffer[4096];
};

typedef enum{
    SFLOW_CTRL_DISABLE = 0,
    SFLOW_CTRL_ENABLE
}Sflow_ctrl_state;

typedef enum {
    SPI_STATE_IDLE,
    SPI_STATE_TX,
    SPI_STATE_RX,
    SPI_STATE_NUM
} spi_state ;

typedef enum {
    TRANS_STATUS_WAITING_INIT_SEGMENT = 0,
    TRANS_STATUS_WAITING_MORE_SEGMENT,
    TRANS_STATUS_WAITING_LAST_SEGMENT,
    TRANS_STATUS_FINISHED,
    TRANS_STATUS_ERROR,
    TRANS_STATUS_NUM
} trans_status;

enum SPI_RESUME_ENUM
{
    ON_SUSPEND = 0,
    ON_RESUME ,
};

struct spi_modem_port {
    struct spi_modem *modem;
    struct spi_device    *spi;
    struct tty_port    port;
    struct kref        kref;
    spinlock_t        write_lock;
    struct tty_struct    *tty;
    unsigned int        index;

    struct kfifo        transmit_fifo;
    const char  *name;
    char work_name[64];
    struct workqueue_struct *write_q;
    struct workqueue_struct *respond_cflag80_q;
    struct workqueue_struct *read_q;
    struct work_struct    write_work;
    struct work_struct    respond_cflag80_work;
    struct work_struct read_work;
    struct device dev;

    struct mutex        spi_buf_in_mutex;
    struct list_head     spi_buf_in_list;
    unsigned char        spi_buf_in;
    unsigned int        spi_buf_in_num;    /*buffer in list num*/
    unsigned int        spi_buf_in_max_num;    /*buffer in list max num*/
    unsigned int        spi_buf_in_size;    /*buffer in size*/

    wait_queue_head_t sflow_ctrl_wait_q;
    atomic_t sflow_ctrl_state;

    /* Settings for the port */
    int rts_state;    /* Handshaking pins (outputs) */
    int dtr_state;
    int cts_state;    /* Handshaking pins (inputs) */
    int dsr_state;
    spinlock_t inception_lock;
    int inception;
};

struct spi_buf_in_packet{
    struct list_head     node;
    unsigned int        size;
    unsigned char        *buffer;
};

struct ctrl_port_msg{
    struct spi_msg_head  head;
    unsigned char id_hi;
    unsigned char id_low;
    unsigned char chan_num;
    unsigned char option;
};

struct spi_modem_ctrl_port{
    struct ctrl_port_msg chan_ctrl_msg;
    unsigned int chan_state;
    unsigned int msg_id;
    wait_queue_head_t sflow_ctrl_wait_q;
    atomic_t sflow_ctrl_state;
};

struct spi_modem{
    struct spi_modem_port *port[SPI_TTY_NR];
    struct spi_modem_ctrl_port *ctrl_port;
    struct spi_device    *spi;
    struct spi_msg *msg;
    unsigned char *trans_buffer;
    struct cbp_platform_data *cbp_data;
    struct semaphore sem;
    spi_state spi_state;
    trans_status spi_tx_state;
    trans_status spi_rx_state;
    atomic_t ack_tx_state;
    unsigned int data_length;
    struct semaphore spi_sem;
};

extern int  modem_spi_init(struct cbp_platform_data *pdata);
extern void  modem_spi_exit(void);
extern int modem_err_indication_usr(int revocery);

struct rnic_buf_in_packet{
    struct list_head  node;
    unsigned char    id;
    struct sk_buff *buffer;
};

//FIXME unsigned long
struct sdio_read_func {
       unsigned char ucPDNId;
       unsigned long (*cbp_read_buff)(unsigned char ucPDNId, struct sk_buff *pstData);
};
typedef unsigned long (*RCV_C_DL_DATA_FUNC)(unsigned char ucPDNId, struct sk_buff *pstData);
#endif
