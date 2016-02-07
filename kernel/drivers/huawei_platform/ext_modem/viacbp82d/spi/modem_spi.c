/*
 * drivers/mmc/card/modem_spi.c
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
#include <linux/spi/spi.h>
#include <linux/platform_device.h>
#include <linux/irq.h>
#include <linux/circ_buf.h>
#include <linux/tty.h>
#include <linux/tty_flip.h>
#include <linux/kfifo.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/delay.h>
#include <linux/timer.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/mutex.h>
#include <linux/gpio.h>
#include <linux/wait.h>
#include <linux/workqueue.h>
#include "modem_spi.h"
#include "../viatel.h"
#include <linux/kthread.h>
#include <linux/wakelock.h>
#include<huawei_platform/dsm/dsm_pub.h>
#include <huawei_platform/log/log_switch.h>

static unsigned int spi_tx_cnt = 0;
static unsigned int spi_rx_cnt = 0;
static unsigned int spi_is_current_tx = 0; // 1 is tx, 0 is rx
static unsigned int spi_is_last_tx = 0; // 1 is tx, 0 is rx
static unsigned char spi_tx_current_packet_dump[32] = {0};
static unsigned char spi_tx_last_packet_dump[32] = {0};
static unsigned char spi_rx_current_packet_dump[32] = {0};
static unsigned char spi_rx_last_packet_dump[32] = {0};
static unsigned char spi_print_buff[1024] = {0};
static unsigned int spi_print_buff_pos = 0;
static unsigned int spi_tx_cnt_on_channel[SPI_TTY_NR + 1] = {0}; //channel count = tty count + 1
static unsigned int spi_tx_byte_count_on_channel[SPI_TTY_NR + 1] = {0};
static unsigned int spi_rx_cnt_on_channel[SPI_TTY_NR + 1] = {0};
static unsigned int spi_rx_byte_count_on_channel[SPI_TTY_NR + 1] = {0};
static int print_next_packet_once_after_resume = 0;

#define SPI_WAKEUP_CHARS        8*256

#define TRANSMIT_SHIFT    (10)
#define TRANSMIT_BUFFER_SIZE    (1UL << TRANSMIT_SHIFT)
#define TRANSMIT_MAX_SIZE     ((1UL << TRANSMIT_SHIFT)  - 4)

#define VIA_SPI_SPEED      (12000000)
#define VIA_SPI_PACKET_HEADER_SIZE         (4)
#define VIA_SPI_PACKET_END_DUMMY_SIZE         (4)
#define VIA_SPI_PACKET_MAX_LENGTH    (4096)
#define VIA_SPI_PACKET_MAX_LENGTH_IN_WORD    (1024)
#define VIA_SPI_START_BUF_SIZE         (60)
#define VIA_SPI_FIFO_SIZE         (512)
#define VIA_SPI_PACKET_PAYLOAD_SIZE    (1024 - VIA_SPI_PACKET_HEADER_SIZE)
#define SPI_TRANS_MAX_TRY_TIMES    (10)
#define MAX_WAIT_SOFT_FLOW_CTRL_TIMES    (10)
#define MAX_WAIT_DATA_ACK_RETRY         (6)
//for SPI FIFO size
//#define FIFO_SIZE    (VIA_SPI_FIFO_SIZE - VIA_SPI_PACKET_HEADER_SIZE)
#define FIFO_SIZE    8*PAGE_SIZE   //PAGE_SIZE is 4096
#define WAIT_DATA_ACK_TIMEOUT_ONCE_IN_MS (1000)

static char *via_spi_start_buf;
static char *via_spi_follow_buf;
static int remain_packet_len = 0;
static int msg_buf_pos = 0;
//static int transmit_fifo_pos = 0;

static unsigned int spi_read_timeout_cnt = 0;
static unsigned int spi_write_timeout_cnt = 0;
static unsigned int respond_cflag80_timeout_cnt = 0;
#define CURRENT_SEGMENT_DUMP_CHAR_SIZE (VIA_SPI_FIFO_SIZE*4) //print format xx-xx-xx-,every byte occupy 3 char
static unsigned char spi_current_segment_dump[CURRENT_SEGMENT_DUMP_CHAR_SIZE] = {0};
static unsigned int spi_cur_segment_buf_pos = 0;

static struct tty_driver *modem_spi_tty_driver;
static struct cbp_platform_data *cbp_pdata;
static struct spi_modem_port *spi_modem_table[SPI_TTY_NR] = {NULL};
static DEFINE_SPINLOCK(spi_modem_table_lock);

//channel index
#define SPI_ETS_LOG_CHANNEL_INDEX       1

#define SPI_AT_CHANNEL_NUM            4
#define SPI_AT_CHANNEL_FT_NUM_7        7 //ttySPI6 is for ProjectMenu
#define SPI_RILD_CTRL_CHANNEL_NUM    8 //ttySPI7 transfer rild control command which are hex, not ASCI char.
#define SPI_AT_CHANNEL_UICC_NUM_9    9 //ttySPI8 is for UICC
#define SPI_AT_CHANNEL_NUM_10            10
#define SPI_AT_CHANNEL_NUM_11            11
#define SPI_AT_CHANNEL_LOGTOOL_NUM_12    12 //ttySPI11 is for modemlogcat_via
extern unsigned char cbp_power_state;
static struct spi_modem *via_modem = NULL;

static unsigned int dtr_value = 0;
static unsigned int dcd_state = 0;

static struct work_struct dtr_work;
static struct work_struct dcd_query_work;

static unsigned int modem_remove = 1;
static spinlock_t modem_remove_lock;

struct dsm_client *spi_dsm_client = NULL;
static void spi_read_port_work(struct work_struct *work);
static void spi_tx_rx_packets_dump(void);
#define     FALSE       0
#define     TRUE         1

#define SK_BUFFER_RESERVE    (12)
#define MAX_PDN_ID            (10)
#define CDMA_PDNID            (99)

#define SPI_DATA_DEBUT     (0)
/*This lenth is come from balong, for them, they will preserved the count of packege is 512*/
#define MAX_QUEUE_LENGTH              (512)
#define VIA_SPI_IP_PACKET_MAX_SIZE    (1501)//Ip package max size 1500  + PDN ID 1 = 1501.

static struct sdio_read_func read_func[MAX_PDN_ID] = {{0}};

#define    DOWN_MODEM_SEMAPHORE_TIMEOUT_MS        (2000)//2s
#define    DOWN_MODEM_SEMAPHORE_RETRY_TIMES    150
#define    DOWN_MODEM_SEMAPHORE_PRINT_LOG        15//print log per 3 timeout

#define    DOWN_MODEM_SW_FLOW_COTR_RETRY_TIMES    150
//Copy from balong RNIC
enum RNIC_RESULT_ENUM
{
    RNIC_OK             = 0,                                                    /* 正常返回 */
    RNIC_NOTSTARTED     = 1,                                                    /* 未开始 */
    RNIC_INPROGRESS     = 2,                                                    /* 运行中 */
    RNIC_PERM           = 3,
    RNIC_NOENT          = 4,
    RNIC_IO             = 5,
    RNIC_NXIO           = 6,
    RNIC_NOMEM          = 7,                                                    /* 未申请到内存 */
    RNIC_BUSY           = 8,                                                    /* RNIC网卡设备忙 */
    RNIC_NODEV          = 9,                                                    /* 无设备 */
    RNIC_INVAL          = 10,                                                   /* 非法设备 */
    RNIC_NOTSUPP        = 11,                                                   /* 操作不支持 */
    RNIC_TIMEDOUT       = 12,                                                   /* 超时 */
    RNIC_SUSPENDED      = 13,                                                   /* 挂起 */
    RNIC_UNKNOWN        = 14,                                                   /* 未知错误 */
    RNIC_TEST_FAILED    = 15,                                                   /* 测试失败 */
    RNIC_STATE          = 16,                                                   /* 状态错误 */
    RNIC_STALLED        = 17,                                                   /* 失速 */
    RNIC_PARAM          = 18,                                                   /* 参数错误 */
    RNIC_ABORTED        = 19,                                                   /* 请求取消 */
    RNIC_SHORT          = 20,                                                   /* 资源不足 */
    RNIC_EXPIRED        = 21,                                                   /* 溢出 */

    RNIC_ADDR_INVALID   = 22,                                                   /* 无法分配地址 */
    RNIC_OUT_RANGE      = 23,                                                   /* 不在有效范围内 */
    RNIC_PKT_TYPE_INVAL = 24,                                        /* 无效ip类型 */
    RNIC_ADDMAC_FAIL = 25,                                         /* 添加mac头失败 */
    RNIC_RX_PKT_FAIL = 26,                                         /* 调用内核接口接收数据失败 */
    RNIC_NO_REG_FUNCTION = 27,                                    /* blong unregist the function. if some data transfer delayed when function unregest, it will produce this err*/
    RNIC_REG_FUNCTION_MORE = 28,                                    /* blong regist more than one function.*/
    RNIC_INVAL_PDNID = 29,
    RNIC_INVAL_LENTH = 30,
    RNIC_ERROR          = 0xff,                                                 /* RNIC返回失败 */
    RNIC_BUTT
};

typedef struct RnicDataFrame{
     struct list_head list_head_node;
     unsigned int list_size;
     spinlock_t slock;
     wait_queue_head_t wait;
     int overflow_flag;
}tRnicDataFrame;
static tRnicDataFrame rnic_frame;
static struct wake_lock rnic_tx_wk;

static struct dsm_dev dsm_modem_spi = {
    .name = "dsm_modem_spi",
    .fops = NULL,
    .buff_size = 1024,
};

static int rnic_send_packet(struct sk_buff *pstData, unsigned char ucPDNId);
static void printf_data(char* buff, int len);

extern int is_via_modem_state_ready(void);

static void hw_spi_dsm_client_notify(const char* content,  int ret, int err_numb) {
    if(spi_dsm_client && !dsm_client_ocuppy(spi_dsm_client)) {
        if(content) {
            dsm_client_record(spi_dsm_client, "%s %d\n", content, ret);
        }
        dsm_client_notify(spi_dsm_client, err_numb);
        hwlog_info("spi dsm_client_notify success\n");
    } else {
        hwlog_err("spi dsm_client_notify failed\n");
    }
}

/**
 * spi_tx_rx_printk - print spi tx and rx data, when log level is LOG_NOTICE or larger
 * @buf: the point of data buffer
 * @type: print type, 0:rx  1:tx
 *
 * no return
 */
static void spi_tx_rx_printk(const void *buf, unsigned char type)
{
    unsigned int count;
    const unsigned char *print_buf = (const unsigned char *)buf;
    int i;
    unsigned char spi_channel = 0;
    static unsigned int print_channel = 0;

    spi_print_buff_pos = 0;
    memset(spi_print_buff, 0, sizeof(spi_print_buff));
    print_channel = (print_channel + 1) % 16; //channel id is 0 -15
    spi_channel = print_buf[1];
    count =(((*(print_buf+2) & 0x0F) << 8) | (*(print_buf+3) & 0xFF));
    if(type == 1){
        spi_print_buff_pos += snprintf(&spi_print_buff[spi_print_buff_pos], (1024 - spi_print_buff_pos), \
                    "write %d to channel %d tx/rx cnt %d/%d tx/rx/80 timeout cnt %d/%d/%d>>", count, *(print_buf+1), spi_tx_cnt, spi_rx_cnt, \
                    spi_write_timeout_cnt, spi_read_timeout_cnt, respond_cflag80_timeout_cnt);
    }
    else{
        spi_print_buff_pos += snprintf(&spi_print_buff[spi_print_buff_pos], (1024 - spi_print_buff_pos), \
                    "read %d from channel %d tx/rx cnt %d/%d tx/rx/80 timeout cnt %d/%d/%d<<", count, *(print_buf+1), spi_tx_cnt, spi_rx_cnt, \
                    spi_write_timeout_cnt, spi_read_timeout_cnt, respond_cflag80_timeout_cnt);
    }

    if(count > 20)
        count = 20;
    for(i = 0; i < count + 4; i++)
    {
        if ((0 == get_logctl_flag()) \
            && ((SPI_RILD_CTRL_CHANNEL_NUM == spi_channel) \
            || (SPI_AT_CHANNEL_UICC_NUM_9 == spi_channel) \
            || (SPI_AT_CHANNEL_NUM_10 == spi_channel) \
            || (SPI_AT_CHANNEL_NUM_11 == spi_channel))) {
            break;
        }
        if ((SPI_AT_CHANNEL_NUM == spi_channel) \
            || (SPI_AT_CHANNEL_FT_NUM_7 == spi_channel) \
            || (SPI_AT_CHANNEL_UICC_NUM_9 == spi_channel) \
            || (SPI_AT_CHANNEL_NUM_10 == spi_channel) \
            || (SPI_AT_CHANNEL_NUM_11 == spi_channel) \
            || (SPI_AT_CHANNEL_LOGTOOL_NUM_12 == spi_channel)) {
            if (i < 4)  {
                spi_print_buff_pos += snprintf(&spi_print_buff[spi_print_buff_pos], (1024 - spi_print_buff_pos), \
                    "%x-", *(print_buf+i));
            } else {
                if ( ('\n' == *(print_buf+i)) || ('\r' == *(print_buf+i))) {
                    spi_print_buff_pos += snprintf(&spi_print_buff[spi_print_buff_pos], (1024 - spi_print_buff_pos), \
                        "-");
                } else {
                    spi_print_buff_pos += snprintf(&spi_print_buff[spi_print_buff_pos], (1024 - spi_print_buff_pos), \
                        "%c", *(print_buf+i));
                }
            }
        } else {
            spi_print_buff_pos += snprintf(&spi_print_buff[spi_print_buff_pos], (1024 - spi_print_buff_pos), \
                        "%x-", *(print_buf+i));
        }
    }

    //channel is from 0 to 15, channel count = tty count + 1
    if (1 == type) {
        spi_print_buff_pos += snprintf(&spi_print_buff[spi_print_buff_pos], (1024 - spi_print_buff_pos), \
                        "; tx count of channel 0-15:[");
        for (i = 0; i < SPI_TTY_NR; i++) {
            spi_print_buff_pos += snprintf(&spi_print_buff[spi_print_buff_pos], (1024 - spi_print_buff_pos), \
                        "%x,", spi_tx_cnt_on_channel[i]);
        }
        spi_print_buff_pos += snprintf(&spi_print_buff[spi_print_buff_pos], (1024 - spi_print_buff_pos), \
                        "%x]\n", spi_tx_cnt_on_channel[SPI_TTY_NR]);
    } else {
        spi_print_buff_pos += snprintf(&spi_print_buff[spi_print_buff_pos], (1024 - spi_print_buff_pos), \
                        "; rx count of channel 0-15:[");
        for (i = 0; i < SPI_TTY_NR; i++) {
            spi_print_buff_pos += snprintf(&spi_print_buff[spi_print_buff_pos], (1024 - spi_print_buff_pos), \
                        "%x,", spi_rx_cnt_on_channel[i]);
        }
        spi_print_buff_pos += snprintf(&spi_print_buff[spi_print_buff_pos], (1024 - spi_print_buff_pos), \
                        "%x]\n", spi_rx_cnt_on_channel[SPI_TTY_NR]);
    }

    hwlog_info("%s", spi_print_buff);
}

void spi_tx_current_packet_print(void)
{
    hwlog_info("print current tx packet:\n");
    spi_tx_rx_printk(spi_tx_current_packet_dump, 1);
}
EXPORT_SYMBOL(spi_tx_current_packet_print);

void spi_tx_last_packet_print(void)
{
    hwlog_info("print last tx packet:\n");
    spi_tx_rx_printk(spi_tx_last_packet_dump, 1);
}
EXPORT_SYMBOL(spi_tx_last_packet_print);

void spi_rx_current_packet_print(void)
{
    hwlog_info("print current rx packet:\n");
    spi_tx_rx_printk(spi_rx_current_packet_dump, 0);
}
EXPORT_SYMBOL(spi_rx_current_packet_print);

void spi_rx_last_packet_print(void)
{
    hwlog_info("print last rx packet:\n");
    spi_tx_rx_printk(spi_rx_last_packet_dump, 0);
}
EXPORT_SYMBOL(spi_rx_last_packet_print);

void spi_tx_rx_packets_dump(void)
{
    if (spi_is_current_tx) {
        spi_tx_current_packet_print();
    } else {
        spi_rx_current_packet_print();
    }

    if (spi_is_last_tx) {
        spi_tx_last_packet_print();
    } else {
        spi_rx_last_packet_print();
    }
}

void modem_dtr_send(struct work_struct *work);

static struct spi_modem_port *spi_modem_port_get(unsigned int index)
{
    struct spi_modem_port *port;
    unsigned long flags = 0;

    if (index >= SPI_TTY_NR) {
        hwlog_err("%s %d: tty port index=%d is invalid.\n", __func__, __LINE__, index);
        return NULL;
    }

    spin_lock_irqsave(&spi_modem_table_lock, flags);
    port = spi_modem_table[index];
    spin_unlock_irqrestore(&spi_modem_table_lock, flags);

    return port;
}
static void spi_modem_port_destroy(struct kref *kref)
{
    struct spi_modem_port *port = NULL;
    int index;

    if(kref){
        port = container_of(kref, struct spi_modem_port, kref);
        index = port->index;
        hwlog_info("%s %d: index = %d .\n", __func__, __LINE__, index);
        kfifo_free(&port->transmit_fifo);
        kfree(port);
    }else{
        hwlog_err("%s %d: invalid port.\n", __func__, __LINE__);
    }
}

static void spi_modem_port_put(struct spi_modem_port *port)
{
    if(port){
        hwlog_info("%s %d: port %d.\n", __func__, __LINE__, port->index);
        kref_put(&port->kref, spi_modem_port_destroy);
    }else{
        hwlog_err("%s %d: invalid port.\n", __func__, __LINE__);
    }
}

static int check_port(struct spi_modem_port *port) {
    if (!port){
        hwlog_err("%s %d: cbp_power_state=%d, port can not be found.\n", __func__, __LINE__,cbp_power_state);
        return -ENODEV;
    }
    if (!port->spi || (cbp_power_state ==0)){
        hwlog_err("%s %d: cbp_power_state=%d, port index is %d.\n", __func__, __LINE__,cbp_power_state, port->index);
        return -ENODEV;
    }

    //for the reason if VIA modem reset, many userspace applications die loop repeatly call check_port, which result system busy loop.
    //ttySPI0 for respond cflag80 packet, ttySPI2 for flashlessd, ttySPI10 for rild and statusd wait CBP VPUP.
    //rild use ttySPI7 to send command after receive VPUP, but before set modem_state to MODEM_STATE_READY!
    //ttySPI1 is used by ETS log, which has too much ETS log before MODEM_STATE_READY, may result software flow ctrl.
    if ((port->index != 0) \
        && (port->index != 1) \
        && (port->index != 2) \
        && (port->index != 7) \
        && (port->index != 8) \
        && (port->index != 9) \
        && (port->index != 10) \
        && (0 == is_via_modem_state_ready())) {
        hwlog_err("%s %d: Flashlessd is downloading cp.rom and fsm images, port index is %d.\n", __func__, __LINE__, port->index);
        return -1;
    }
    return 0;
}

static int check_port_for_write(struct spi_modem_port *port) {
	if (!port){
		hwlog_err("%s %d: cbp_power_state=%d, port can not be found.\n", __func__, __LINE__,cbp_power_state);
		return -ENODEV;
	}
	if (!port->spi || (cbp_power_state ==0)){
		hwlog_err("%s %d: cbp_power_state=%d, port index is %d.\n", __func__, __LINE__,cbp_power_state, port->index);
		return -ENODEV;
	}
	if ((port->index != 0) \
		&& (port->index != 2) \
		&& (port->index != 7) \
		&& (port->index != 8) \
		&& (port->index != 9) \
		&& (port->index != 10) \
		&& (0 == is_via_modem_state_ready())) {
		hwlog_err("%s %d: Flashlessd is downloading cp.rom and fsm images, port index is %d.\n", __func__, __LINE__, port->index);
		return -1;
	}
	return 0;
}
static void modem_spi_write(struct spi_modem *modem, int addr,
        void *buf,  unsigned int len);

/* CBP control message type */
enum cbp_contrl_message_type {
    CHAN_ONOFF_MSG_ID = 0,
    MDM_STATUS_IND_MSG_ID,
    MDM_STATUS_QUERY_MSG_ID,
    CHAN_SWITCH_REQ_MSG_ID,
    CHAN_STATUS_QUERY_MSG_ID,
    FLOW_CONTROL_MSG_ID,
    CHAN_LOOPBACK_TST_MSG_ID,
    MESSAGE_COUNT,
};

typedef enum {
    OPT_LOOPBACK_NON  = 0,        //no operation, default 0
    OPT_LOOPBACK_OPEN  = 1,        //open loopback test
    OPT_LOOPBACK_CLOSE  = 2,        //close loopback test
    OPT_LOOPBACK_QUERY = 3,        //query loopback test
    OPT_LOOPBACK_NUM
}IOP_OPT_LOOPBACK;

typedef enum {
    RSLT_LOOPBACK_SUCCESS  = 0,    //operation succed
    RSLT_LOOPBACK_WORK = 1,        //loopback testing
    RSLT_LOOPBACK_CLOSED = 2,        //no loopback test
    RSLT_LOOPBACK_INVALID = 3,    //invalid operation
    RSLT_LOOPBACK_FAIL = 4,        //loopback test failed
    RSLT_LOOPBACK_NUM
}IOP_RSLT_LOOPBACK;

static void respond_cflag80_packet_work(struct work_struct *work);
static void spi_write_port_work(struct work_struct *work);
static int check_regist_function(void);


int respond_cflag80_packet_to_via_request_to_send(void)
{
    struct spi_modem_port *port;

    hwlog_debug("%s %d: enter\n", __func__, __LINE__);

    port = spi_modem_port_get(0);
    //NOTE!!! gpio request to send interrupt is earlier than rst_ind interrupt
    if (0 == check_port(port)) { //should allow happen to down cp.rom through SPI
        queue_work(port->respond_cflag80_q, &port->respond_cflag80_work);
        return 0;
    } else {
        hwlog_err("%s %d VIA modem is POWER OFF, ERROR exit, port->index is %d.\n", __func__,__LINE__, port->index);
        return -1;
    }
}
EXPORT_SYMBOL(respond_cflag80_packet_to_via_request_to_send);

static int contruct_ctrl_chan_msg(struct spi_modem_ctrl_port *ctrl_port , int msg,
        unsigned char chan_num, unsigned char opt)
{
    if (unlikely(ctrl_port == NULL)){
        hwlog_err("%s %d: control channel is null.\n", __func__, __LINE__);
        return -EINVAL;
    }

    ctrl_port->chan_ctrl_msg.head.start_flag    = 0xFE;
    ctrl_port->chan_ctrl_msg.head.chanInfo    = 0;
    ctrl_port->chan_ctrl_msg.head.tranHi        = 0;            /*High byte of the following payload length*/
    ctrl_port->chan_ctrl_msg.head.tranLow    = 4;            /*Low byte of the following payload length*/
    ctrl_port->chan_ctrl_msg.id_hi            = ( (msg >> 8) & 0xFF);/*High byte of control message ID,for onoff request ID=CHAN_ONOFF_MSG_ID*/
    ctrl_port->chan_ctrl_msg.id_low            = (msg & 0xFF);/*Low byte of control message ID,for onoff request ID=CHAN_ONOFF_MSG_ID*/
    ctrl_port->chan_ctrl_msg.chan_num        = chan_num;            /*ChanNum ,same as ChanInfo*/
    ctrl_port->chan_ctrl_msg.option            = opt;

    return 0;
}
static unsigned char loop_back[12];

int modem_on_off_ctrl_chan(unsigned char on)
{
    struct spi_modem *modem;
    struct spi_modem_port *port;
    struct spi_modem_ctrl_port *ctrl_port;
    unsigned char msg_len = 0;
    int ret = 0;
    unsigned int remain_time = 0;
    unsigned int wait_soft_flow_ctrl_times = 0;

    hwlog_info("%s %d: enter, on = %d \n", __func__, __LINE__, on);
    port = spi_modem_port_get(0);
    ret = check_port(port);
    if (ret < 0){
        hwlog_err("%s %d check_port failed\n", __func__,__LINE__);
        goto down_out;
    }
    modem = port->modem;
    ctrl_port = modem->ctrl_port;
    do {
        remain_time = wait_event_timeout(ctrl_port->sflow_ctrl_wait_q, (SFLOW_CTRL_DISABLE == atomic_read(&ctrl_port->sflow_ctrl_state) \
            ||(cbp_power_state==0)), msecs_to_jiffies(200));
        if (0 == remain_time) {
            hwlog_err("%s %d: wait CBP software flow control disable timeout in 200ms.\n", __func__, __LINE__);
            wait_soft_flow_ctrl_times++;
            if (wait_soft_flow_ctrl_times > MAX_WAIT_SOFT_FLOW_CTRL_TIMES) {
                hwlog_err("%s %d: wait CBP software flow control disable has reached MAX WAIT TIMES.\n", __func__, __LINE__);
                return -1;
            }
        }
    } while (0 == remain_time);

    if(down_interruptible(&modem->spi_sem)){
        hwlog_err("%s %d down_interruptible failed.\n", __func__,__LINE__);
        ret =  -ERESTARTSYS;
        goto down_out;
    }

    ret = contruct_ctrl_chan_msg(ctrl_port, CHAN_ONOFF_MSG_ID, 0, on);
    if (ret < 0){
        hwlog_err("%s %d: contruct_ctrl_chan_msg failed\n", __func__, __LINE__);
        goto up_sem;
    }
    msg_len = sizeof(struct ctrl_port_msg);
    msg_len = (msg_len + 3) & ~0x03;  /* Round up to nearest multiple of 4 */
    modem_spi_write(modem, 0x00, &(ctrl_port->chan_ctrl_msg), msg_len);
    spi_tx_cnt_on_channel[0]++;
    spi_tx_byte_count_on_channel[0] += msg_len;

up_sem:
    up(&modem->spi_sem);
down_out:
    return ret;
}
EXPORT_SYMBOL(modem_on_off_ctrl_chan);

void modem_dtr_set(int on, int low_latency)
{
    unsigned long flags;

    spin_lock_irqsave(&modem_remove_lock, flags);
    if(!modem_remove) {
        spin_unlock_irqrestore(&modem_remove_lock, flags);
        dtr_value = on;
        if(low_latency)
            modem_dtr_send(&dtr_work);
        else
            schedule_work(&dtr_work);
    }
    else{
        spin_unlock_irqrestore(&modem_remove_lock, flags);
    }
}
EXPORT_SYMBOL(modem_dtr_set);

void modem_dtr_send(struct work_struct *work)
{
    struct spi_modem *modem;
    struct spi_modem_port *port;
    struct spi_modem_ctrl_port *ctrl_port;
    unsigned char control_signal=0;
    unsigned char msg_len = 0;
    int ret = 0;
    unsigned int on = 0;
    long remain_time = 0;
    unsigned int wait_soft_flow_ctrl_times = 0;

    on = dtr_value;
    hwlog_info("%s %d: enter, on =%d\n", __func__, __LINE__, on);
    port = spi_modem_port_get(0);
    ret = check_port(port);
    if (ret < 0){
        hwlog_err("%s %d check_port failed\n", __func__,__LINE__);
        goto down_out;
    }
    modem = port->modem;
    ctrl_port = modem->ctrl_port;
    do {
        remain_time = wait_event_timeout(ctrl_port->sflow_ctrl_wait_q, (SFLOW_CTRL_DISABLE == atomic_read(&ctrl_port->sflow_ctrl_state) \
            ||(cbp_power_state==0)), msecs_to_jiffies(200));
        if (0 == remain_time) {
            hwlog_err("%s %d: wait CBP software flow control disable timeout in 200ms.\n", __func__, __LINE__);
            wait_soft_flow_ctrl_times++;
            if (wait_soft_flow_ctrl_times > MAX_WAIT_SOFT_FLOW_CTRL_TIMES) {
                hwlog_err("%s %d: wait CBP software flow control disable has reached MAX WAIT TIMES.\n", __func__, __LINE__);
                return;
            }
        }
    } while (0 == remain_time);

    if(down_interruptible(&modem->spi_sem)){
        hwlog_err("%s %d down_interruptible failed.\n", __func__,__LINE__);
        ret =  -ERESTARTSYS;
        goto down_out;
    }

    if(ctrl_port->chan_state == 1){
        if(on){
            control_signal |= 0x04;
        }
        else{
            control_signal &= 0xFB;
        }

        ret = contruct_ctrl_chan_msg(ctrl_port, MDM_STATUS_IND_MSG_ID, 2, control_signal);
        if (ret < 0){
            hwlog_err("%s %d: contruct_ctrl_chan_msg failed\n", __func__, __LINE__);
            goto up_sem;
        }
        msg_len = sizeof(struct ctrl_port_msg);
        msg_len = (msg_len + 3) & ~0x03;  /* Round up to nearest multiple of 4 */
        modem_spi_write(modem, 0x00, &(ctrl_port->chan_ctrl_msg), msg_len);
        spi_tx_cnt_on_channel[0]++;
        spi_tx_byte_count_on_channel[0] += msg_len;
    }
    else{
        ret = -1;
        hwlog_err("%s %d: ctrl channel is off, please turn on first\n", __func__, __LINE__);
    }

up_sem:
    up(&modem->spi_sem);
down_out:
    return;
}

int modem_dcd_state(void)
{
    unsigned long flags;

    spin_lock_irqsave(&modem_remove_lock, flags);
    if(!modem_remove) {
        spin_unlock_irqrestore(&modem_remove_lock, flags);
        schedule_work(&dcd_query_work);
    } else {
        spin_unlock_irqrestore(&modem_remove_lock, flags);
        dcd_state = 0;
    }
    return dcd_state;
}
EXPORT_SYMBOL(modem_dcd_state);

void modem_dcd_query(struct work_struct *work)
{
    struct spi_modem *modem;
    struct spi_modem_port *port;
    struct spi_modem_ctrl_port *ctrl_port;
    unsigned char msg_len = 0;
    int ret = 0;
    long remain_time = 0;
    unsigned int wait_soft_flow_ctrl_times = 0;

    hwlog_info("%s %d: enter\n", __func__, __LINE__);
    port = spi_modem_port_get(0);
    ret = check_port(port);
    if (ret < 0){
        hwlog_err("%s %d check_port failed\n", __func__,__LINE__);
        goto down_out;
    }
    modem = port->modem;
    ctrl_port = modem->ctrl_port;

    if(ctrl_port->chan_state == 1){
        do {
            remain_time = wait_event_timeout(ctrl_port->sflow_ctrl_wait_q, (SFLOW_CTRL_DISABLE == atomic_read(&ctrl_port->sflow_ctrl_state) \
                ||(cbp_power_state==0)), msecs_to_jiffies(200));
            if (0 == remain_time) {
                hwlog_err("%s %d: wait CBP software flow control disable timeout in 200ms.\n", __func__, __LINE__);
                wait_soft_flow_ctrl_times++;
                if (wait_soft_flow_ctrl_times > MAX_WAIT_SOFT_FLOW_CTRL_TIMES) {
                    hwlog_err("%s %d: wait CBP software flow control disable has reached MAX WAIT TIMES.\n", __func__, __LINE__);
                    return;
                }
            }
        } while (0 == remain_time);

        if(down_interruptible(&modem->spi_sem)){
            hwlog_err("%s %d down_interruptible failed.\n", __func__,__LINE__);
            ret =  -ERESTARTSYS;
            goto down_out;
        }

        ret = contruct_ctrl_chan_msg(ctrl_port, MDM_STATUS_QUERY_MSG_ID, 2, 0);
        if (ret < 0){
            hwlog_err("%s %d: contruct_ctrl_chan_msg failed\n", __func__, __LINE__);
            goto up_sem;
        }
        msg_len = sizeof(struct ctrl_port_msg);
        msg_len = (msg_len + 3) & ~0x03;  /* Round up to nearest multiple of 4 */
        modem_spi_write(modem, 0x00, &(ctrl_port->chan_ctrl_msg), msg_len);
        spi_tx_cnt_on_channel[0]++;
        spi_tx_byte_count_on_channel[0] += msg_len;
up_sem:
        up(&modem->spi_sem);
        msleep(10);
        dcd_state = port->dtr_state;
    }
    else{
        dcd_state = 0;
        hwlog_err("%s: ctrl channel is off, please turn on first\n", __func__);
    }
down_out:
    return;
}

int modem_loop_back_chan(unsigned char chan_num, unsigned char opt)
{
    struct spi_modem *modem;
    struct spi_modem_port *port;
    struct spi_modem_ctrl_port *ctrl_port;
    unsigned char msg_len = 0;
    int ret = 0;
    long remain_time = 0;
    unsigned int wait_soft_flow_ctrl_times = 0;

    hwlog_info("%s %d: enter\n", __func__, __LINE__);
    port = spi_modem_port_get(0);
    ret = check_port(port);
    if (ret < 0){
        hwlog_err("%s %d check_port failed\n", __func__,__LINE__);
        goto down_out;
    }
    modem = port->modem;
    ctrl_port = modem->ctrl_port;
    do {
        remain_time = wait_event_timeout(ctrl_port->sflow_ctrl_wait_q, (SFLOW_CTRL_DISABLE == atomic_read(&ctrl_port->sflow_ctrl_state) \
            ||(cbp_power_state==0)), msecs_to_jiffies(200));
        if (0 == remain_time) {
            hwlog_err("%s %d: wait CBP software flow control disable timeout in 200ms.\n", __func__, __LINE__);
            wait_soft_flow_ctrl_times++;
            if (wait_soft_flow_ctrl_times > MAX_WAIT_SOFT_FLOW_CTRL_TIMES) {
                hwlog_err("%s %d: wait CBP software flow control disable has reached MAX WAIT TIMES.\n", __func__, __LINE__);
                return -1;
            }
        }
    } while (0 == remain_time);

    if(down_interruptible(&modem->spi_sem)){
        hwlog_err("%s %d down_interruptible failed.\n", __func__,__LINE__);
        ret =  -ERESTARTSYS;
        goto down_out;
    }

    if(ctrl_port->chan_state == 1){
        loop_back[0]        = 0xFE;
        loop_back[1]        = 0;
        loop_back[2]        = 0;            /*High byte of the following payload length*/
        loop_back[3]        = 6;            /*Low byte of the following payload length*/
        loop_back[4]        = 0x00;        /*High byte of control message ID,for onoff request ID=CHAN_ONOFF_MSG_ID*/
        loop_back[5]        = 0x06;        /*Low byte of control message ID,for onoff request ID=CHAN_ONOFF_MSG_ID*/
        loop_back[6]        = 3;            /*device id spi = 3*/
        loop_back[7]        = opt;
        loop_back[8]        = chan_num;/*ChanNum ,same as ChanInfo*/
        loop_back[9]        = 0;

        msg_len = 12;
        msg_len = (msg_len + 3) & ~0x03;  /* Round up to nearest multiple of 4 */
        modem_spi_write(modem, 0x00, &(loop_back[0]), msg_len);
        spi_tx_cnt_on_channel[0]++;
        spi_tx_byte_count_on_channel[0] += msg_len;
    }
    else{
        ret = -1;
        hwlog_err("%s %d: ctrl channel is off, please turn on first\n", __func__, __LINE__);
    }
    up(&modem->spi_sem);
down_out:
    return ret;
}
EXPORT_SYMBOL(modem_loop_back_chan);

static int ctrl_msg_analyze(struct spi_modem *modem)
{
    struct spi_modem_ctrl_port *ctrl_port;
    const unsigned char msg_id_high     = *modem->msg->buffer;
    const unsigned char msg_id_low     = *(modem->msg->buffer+1);
    const unsigned int msg_id             = (msg_id_high << 8) + msg_id_low;
    unsigned char option=*(modem->msg->buffer+3);
    struct spi_modem_port *port;
    unsigned char chan_num;
    unsigned char res;

    ctrl_port = modem->ctrl_port;

    switch(msg_id)
    {
        case CHAN_ONOFF_MSG_ID:
            if(option == 1){
                ctrl_port->chan_state = 1;
                hwlog_info("%s %d: ctrl channel is open\n", __func__, __LINE__);
            }
            else if(option == 0){
                ctrl_port->chan_state = 0;
                hwlog_info("%s %d: ctrl channel is close\n", __func__, __LINE__);
            }
            else{
                hwlog_err("%s %d: err option value = %d\n",
                        __func__, __LINE__, option);
            }
            break;
        case MDM_STATUS_IND_MSG_ID:
            port = modem->port[0];
            if(option & 0x80)/*connetc*/
            {
                port->dtr_state = 1;
            }
            else/*disconnetc*/
            {
                port->dtr_state = 0;
            }
            break;
        case MDM_STATUS_QUERY_MSG_ID:
            port = modem->port[0];
            if(option & 0x80)/*connetc*/
            {
                port->dtr_state = 1;
            }
            else/*disconnetc*/
            {
                port->dtr_state = 0;
            }
            /*to be contionue*/
            break;
        case CHAN_LOOPBACK_TST_MSG_ID:
            {
                chan_num = *(modem->msg->buffer+4);
                res = *(modem->msg->buffer+5);
                if(option == OPT_LOOPBACK_OPEN)/*open*/
                {
                    hwlog_info("%s %d: open chan %d, result = %d\n",
                            __func__, __LINE__,chan_num, res);
                }
                else if(option == OPT_LOOPBACK_CLOSE)/*close*/
                {
                    hwlog_info("%s %d: close chan %d, result = %d\n",
                            __func__, __LINE__,chan_num, res);
                }
                else if(option == OPT_LOOPBACK_QUERY)/*close*/
                {
                    hwlog_info("%s %d: query chan %d, result = %d\n",
                            __func__, __LINE__,chan_num, res);
                }
                else
                {
                    hwlog_err("%s %d: unknow option %d\n", __func__, __LINE__, option);
                }
            }
            break;
        case FLOW_CONTROL_MSG_ID:
            chan_num = *(modem->msg->buffer+2);
            if(chan_num > 0 && chan_num < (SPI_TTY_NR + 1)){
                chan_num = chan_num - 1;
                port = modem->port[chan_num];
                if(option == 1){
                    hwlog_info("%s %d: channel%d soft flow ctrl enable!\n", __func__, __LINE__, (port->index + 1));
                    atomic_set(&port->sflow_ctrl_state, SFLOW_CTRL_ENABLE);
                    if( SPI_ETS_LOG_CHANNEL_INDEX == chan_num )
                    {
                        hwlog_err("%s:%d soft flow ctrl channel index:%d, use backup gpio reset via modem\n", __func__, __LINE__, chan_num);
                        modem_err_indication_usr(1);
                    }
                }
                else if(option == 0){
                    hwlog_info("%s %d: channel%d soft flow ctrl disable!\n", __func__, __LINE__, (port->index + 1));
                    atomic_set(&port->sflow_ctrl_state, SFLOW_CTRL_DISABLE);
                    wake_up(&port->sflow_ctrl_wait_q);
                }
            }
            else if(chan_num == 0){
                if(option == 1){
                    hwlog_info("%s %d: ctrl channel soft flow ctrl enable!\n", __func__, __LINE__);
                    atomic_set(&ctrl_port->sflow_ctrl_state, SFLOW_CTRL_ENABLE);
                }
                else if(option == 0){
                    hwlog_info("%s %d: ctrl channel soft flow ctrl disable!\n", __func__, __LINE__);
                    atomic_set(&ctrl_port->sflow_ctrl_state, SFLOW_CTRL_DISABLE);
                    wake_up(&ctrl_port->sflow_ctrl_wait_q);
                }
            }else{
                hwlog_err("%s %d: unkown channel num%d!\n", __func__, __LINE__, chan_num);
            }
            break;
        case CHAN_SWITCH_REQ_MSG_ID:
            /*to be contionue*/
            break;
        case CHAN_STATUS_QUERY_MSG_ID:
            /*to be contionue*/
            break;
        default:
            hwlog_err("%s %d: unknow control message received\n", __func__, __LINE__);
            goto err_wrong_format;
    }
    return 0;

err_wrong_format:
    return -1;
}

static void spi_buffer_in_print(struct spi_modem_port *port, struct spi_buf_in_packet *packet)
{
    unsigned int count;
    int i;
    unsigned char spi_channel = 0;

    spi_channel = port->index + 1;
    if ((0 == get_logctl_flag()) \
       && ((SPI_RILD_CTRL_CHANNEL_NUM == spi_channel) \
       || (SPI_AT_CHANNEL_UICC_NUM_9 == spi_channel) \
       || (SPI_AT_CHANNEL_NUM_10 == spi_channel) \
       || (SPI_AT_CHANNEL_NUM_11 == spi_channel))) {
       return;
    }
    spi_print_buff_pos = 0;
    memset(spi_print_buff, 0, sizeof(spi_print_buff));
    count = packet->size;

    if( count > 20){
        count = 20;
    }
    for(i = 0; i < count; i++)
    {
        spi_print_buff_pos += snprintf(&spi_print_buff[spi_print_buff_pos], (1024 - spi_print_buff_pos), \
            "%x-", *(packet->buffer+i));
    }
    spi_print_buff_pos += snprintf(&spi_print_buff[spi_print_buff_pos], (1024 - spi_print_buff_pos), \
            "\n");

    hwlog_info("%s", spi_print_buff);
}

static void spi_buf_in_tty_work(struct spi_modem_port *port)
{
    struct spi_buf_in_packet *packet = NULL;
    struct tty_struct *tty;
    int room;

    tty = tty_port_tty_get(&port->port);
    if(tty){
        while(!list_empty(&port->spi_buf_in_list)){
            packet = list_first_entry(&port->spi_buf_in_list, struct spi_buf_in_packet, node);
            if(!packet){
                hwlog_err("%s %d: the packet is empty!\n", __func__,__LINE__);
                port->spi_buf_in_num--;
                continue ;
            }
            room = tty_buffer_request_room(&port->port, packet->size);
            if(room < packet->size){
                hwlog_err("%s %d: port %d no room in tty rx buffer!\n", __func__,__LINE__, port->index);
            }
            else{
                room = tty_insert_flip_string(&port->port, packet->buffer, packet->size);
                if(room < packet->size){
                    hwlog_err("%s %d: port %d couldn't insert all characters (TTY is full?)!\n", __func__,__LINE__, port->index);
                }
                else{
                    tty_flip_buffer_push(&port->port);
                }
            }

            spi_buffer_in_print(port, packet);

            list_del(&packet->node);
            port->spi_buf_in_size -= packet->size;
            kfree(packet->buffer);
            kfree(packet);
            port->spi_buf_in_num--;
        }
    }
    tty_kref_put(tty);
}

/*****************************************************************************
 * tty driver interface functions
 *****************************************************************************/
/**
 *    spi_uart_install    -    install method
 *    @driver: the driver in use (spi_uart in our case)
 *    @tty: the tty being bound
 *
 *    Look up and bind the tty and the driver together. Initialize
 *    any needed private data (in our case the termios)
 */

static int modem_tty_install(struct tty_driver *driver, struct tty_struct *tty)
{
    struct spi_modem_port *port ;
    int idx = tty->index;
    int ret;

    port = spi_modem_port_get(idx);
    if (!port) {
        tty->driver_data = NULL;
        hwlog_err("%s %d can't find spi modem port.\n", __func__, __LINE__);
        return -ENODEV;
    }

    hwlog_info("%s %d: port %d.\n", __func__, __LINE__, port->index);
    kref_get(&port->kref);

    ret = tty_init_termios(tty);
    if (ret) {
        hwlog_err("%s %d: port %d tty_init_termios return error, ret=%d.\n", __func__, __LINE__, port->index, ret);
        return ret;
    }

    tty_driver_kref_get(driver);
    tty->count++;
    driver->ttys[tty->index] = tty;
    /* This is the ref spi_uart_port get provided */
    tty->driver_data = port;
    return ret;
}

/**
 *    spi_uart_cleanup    -    called on the last tty kref drop
 *    @tty: the tty being destroyed
 *
 *    Called asynchronously when the last reference to the tty is dropped.
 *    We cannot destroy the tty->driver_data port kref until this point
 */

static void modem_tty_cleanup(struct tty_struct *tty)
{
    struct spi_modem_port *port = tty->driver_data;
    tty->driver_data = NULL;    /* Bug trap */
    if(port){
        hwlog_info("%s %d: port %d.\n", __func__, __LINE__, port->index);
        spi_modem_port_put(port);
    }else{
        hwlog_info("%s %d: invalid port.\n", __func__, __LINE__);
    }
}

static int modem_tty_open(struct tty_struct *tty, struct file *filp)
{
    struct spi_modem_port *port = tty->driver_data;
    hwlog_info("%s %d: port %d.\n", __func__, __LINE__, port->index);
    return tty_port_open(&port->port, tty, filp);
}

static void modem_tty_close(struct tty_struct *tty, struct file * filp)
{
    unsigned long flags = 0;
    struct spi_modem_port *port = tty->driver_data;

    hwlog_info("%s %d: port %d.\n", __func__, __LINE__, port->index);
    if (port) {
        if (0 ==  is_via_modem_state_ready()){
            hwlog_info("we should clean the buff %s %d: port %d.\n", __func__, __LINE__,port->index);
            spin_lock_irqsave(&port->write_lock, flags);
            kfifo_reset(&port->transmit_fifo);
            spin_unlock_irqrestore(&port->write_lock, flags);
        }
    }
    tty_port_close(&port->port, tty, filp);
}

static void modem_tty_hangup(struct tty_struct *tty)
{
    struct spi_modem_port *port = tty->driver_data;
    hwlog_info("%s %d: port %d.\n", __func__, __LINE__, port->index);
    tty_port_hangup(&port->port);
}

static int modem_tty_write(struct tty_struct *tty, const unsigned char *buf,
        int count)
{
    struct spi_modem_port *port = tty->driver_data;
    int ret=0;
    long remain_time;
    int nDownRetryTimes = 0;

	ret = check_port_for_write(port);
    if (ret < 0){
        hwlog_err("%s %d: port %d check_port failed\n", __func__,__LINE__, port->index);
        return ret;
    }

    if(port->inception){
        hwlog_err("%s %d: port %d port is inception\n", __func__,__LINE__, port->index);
        return -EBUSY;
    }

    if(count > FIFO_SIZE){
        hwlog_err("%s %d: port %d FIFO size is not enough!\n", __func__,__LINE__, port->index);
        return -1;
    }

    do {
        remain_time = wait_event_timeout(port->sflow_ctrl_wait_q, (SFLOW_CTRL_DISABLE == atomic_read(&port->sflow_ctrl_state) \
            ||(0 == cbp_power_state)), msecs_to_jiffies(2000));
        if (0 == cbp_power_state) {
            hwlog_err("%s %d: port %d wait software contrl disable but VIA CBP reset happened!\n", __func__, __LINE__, port->index);
            goto down_out;
        } else if (0 == remain_time) {
            hwlog_err("%s %d: port %d wait CBP software flow control disable timeout in 2s.\n", __func__, __LINE__, port->index);

            //only when it is ets log channel, has signal pending, and flow control enable, return to directly
            if( signal_pending(current) && ( SPI_ETS_LOG_CHANNEL_INDEX == port->index ) )
            {
                   hwlog_err("%s %d: port %d wait software contrl disable but has signal pending!\n", __func__, __LINE__, port->index);
                   ret = -ERESTARTSYS;
                   goto down_out;
            }
        }
    } while (0 == remain_time);

    nDownRetryTimes = 0;
    do{
        ret = down_timeout(&port->modem->spi_sem, msecs_to_jiffies(DOWN_MODEM_SEMAPHORE_TIMEOUT_MS));
        if( ret ){
            if( signal_pending(current) || (cbp_power_state==0) )
            {
                hwlog_err("%s %d  port:%d down_timeout(modem->spi_sem) return:%d and signal_pending(current) is true or cbp_pwer_state:%d, return\n", __func__,__LINE__, port->index, ret, cbp_power_state);
                if(signal_pending(current))
                {
                    ret =  -ERESTARTSYS;
                }
                if( cbp_power_state==0 )
                {
                    ret = 0;
                }

                goto down_out;
            }

            nDownRetryTimes++;
            if(( nDownRetryTimes % DOWN_MODEM_SEMAPHORE_PRINT_LOG) == 0)
            {
                hwlog_err("%s %d  port:%d down_timeout(modem->spi_sem) return:%d, timeout times=%d, print log and continue\n", __func__,__LINE__, port->index, ret, nDownRetryTimes);
            }
        } else{
            //get the semaphore
            break;
        }
    }while(nDownRetryTimes <= DOWN_MODEM_SEMAPHORE_RETRY_TIMES);

    if((nDownRetryTimes > 0) && (nDownRetryTimes < DOWN_MODEM_SEMAPHORE_PRINT_LOG))
    {
        hwlog_err("%s %d  port:%d down_timeout(modem->spi_sem) timeout times=%d\n", __func__,__LINE__, port->index, nDownRetryTimes);
    }
    else if((nDownRetryTimes >= DOWN_MODEM_SEMAPHORE_PRINT_LOG) && (nDownRetryTimes <= DOWN_MODEM_SEMAPHORE_RETRY_TIMES))
    {
        hwlog_err("%s %d  port:%d down_timeout(modem->spi_sem) timeout times=%d\n", __func__,__LINE__, port->index, nDownRetryTimes);
        //if timeout more then 15 times, run notify.
        hw_spi_dsm_client_notify("down_timeout error: timeout times=", nDownRetryTimes, DSM_SPI_DOWN_ERROR_NO);
    }
    else if(nDownRetryTimes > DOWN_MODEM_SEMAPHORE_RETRY_TIMES )
    {
        hwlog_err("%s %d  port:%d down_timeout(modem->spi_sem) timeout exceed %d return\n", __func__,__LINE__, port->index, DOWN_MODEM_SEMAPHORE_RETRY_TIMES);
              hw_spi_dsm_client_notify("down_timeout timeout exceed. port index =", port->index, DSM_SPI_DOWN_RETRY_MAX_NO);
        goto down_out;
    }

    hwlog_debug("%s %d: port %d modem_tty_write got semaphore.\n", __func__, __LINE__, port->index);

    ret = kfifo_in_locked(&port->transmit_fifo, buf, count, &port->write_lock);

    spi_write_port_work(&port->write_work);

    up(&port->modem->spi_sem);
    hwlog_debug("%s %d: port %d modem_tty_write release semaphore.\n", __func__, __LINE__, port->index);

down_out:

    hwlog_debug("%s %d: port%d\n", __func__,__LINE__, port->index);

    return ret;
}

static int modem_tty_write_room(struct tty_struct *tty)
{
    struct spi_modem_port *port = tty->driver_data;
    unsigned long flags = 0;
    unsigned int data_len = 0;
    int ret;

    ret = check_port(port);
    if (ret < 0){
        hwlog_err("%s %d: port %d check_port failed\n", __func__, __LINE__, port->index);
        return ret;
    }

    spin_lock_irqsave(&port->write_lock, flags);
    data_len = FIFO_SIZE - kfifo_len(&port->transmit_fifo);
    spin_unlock_irqrestore(&port->write_lock, flags);

    hwlog_debug("%s %d: port %d free size %d.\n", __func__, __LINE__, port->index, data_len);
    return data_len;
}

static int modem_tty_chars_in_buffer(struct tty_struct *tty)
{
    struct spi_modem_port *port = tty->driver_data;
    unsigned long flags = 0;
    unsigned int data_len = 0;

    spin_lock_irqsave(&port->write_lock, flags);
    data_len = kfifo_len(&port->transmit_fifo);
    spin_unlock_irqrestore(&port->write_lock, flags);

    hwlog_debug("%s %d: port %d chars in buffer %d.\n", __func__, __LINE__, port->index, data_len);
    return data_len;
}

static void modem_tty_set_termios(struct tty_struct *tty,
        struct ktermios *old_termios)
{
    struct spi_modem_port *port = tty->driver_data;
    int ret;

    ret = check_port(port);
    if (ret < 0){
        hwlog_err("%s %d: port %d ret=%d\n", __func__, __LINE__, port->index, ret);
        return ;
    }

    tty_termios_copy_hw(&(tty->termios), old_termios);
}

static int modem_tty_tiocmget(struct tty_struct *tty)
{
    struct spi_modem_port *port = tty->driver_data;
    int ret;

    ret = check_port(port);
    if (ret < 0){
        hwlog_err("%s %d: port %d ret=%d\n", __func__, __LINE__, port->index, ret);
        return ret;
    }
    return 0;
}

static int modem_tty_tiocmset(struct tty_struct *tty,
        unsigned int set, unsigned int clear)
{
    struct spi_modem_port *port = tty->driver_data;
    int ret;

    ret = check_port(port);
    if (ret < 0){
        hwlog_err("%s %d: port %d ret=%d\n", __func__, __LINE__, port->index, ret);
        return ret;
    }
    return 0;
}

static int spi_modem_activate(struct tty_port *tport, struct tty_struct *tty)
{
    struct spi_modem_port *port = NULL;

    port = container_of(tport, struct spi_modem_port, port);
    hwlog_info("%s %d: port %d enter.\n", __func__, __LINE__, port->index);

    kfifo_reset(&port->transmit_fifo);
    mutex_lock(&port->spi_buf_in_mutex);
    if (port->spi_buf_in == 1){
        spi_buf_in_tty_work(port);
        port->spi_buf_in = 0;
    }
    mutex_unlock(&port->spi_buf_in_mutex);
    hwlog_info("%s %d: port %d Leave.\n", __func__, __LINE__, port->index);
    return 0;
}

static int modem_spi_sync(struct spi_device    *spi, struct spi_message *msg) {
    unsigned int spi_failed_times = 0;
    int status = 0;

    if (NULL == msg || NULL == spi) {
        hwlog_err("%s - sync error: pMsg or spi = null", __func__);
        return -1;
    }

    do {
        status = spi_sync(spi, msg);
        if (0 == status) {
            break;
        } else {
            hwlog_err("%s - sync error: status=%d", __func__, status);
            if (spi_failed_times == 0) { // only notify one times.
                hw_spi_dsm_client_notify("read/80 sync error: status=", status, DSM_SPI_SYNC_ERROR_NO);
            }
            msleep(2000);
            spi_failed_times++;
        }
    } while (spi_failed_times < SPI_TRANS_MAX_TRY_TIMES);

    return status;
}

static void spi_modem_shutdown(struct tty_port *tport)
{
    struct spi_modem_port *port = NULL;
    struct spi_buf_in_packet *packet = NULL;

    if(NULL == tport) {
        hwlog_err("%s - tport is NULL.\n", __func__);
        return;
    }

    port = container_of(tport, struct spi_modem_port, port);
    hwlog_info("%s %d: port %d enter.\n", __func__, __LINE__, port->index);
    mutex_lock(&port->spi_buf_in_mutex);
    while(!list_empty(&port->spi_buf_in_list)){
        packet = list_first_entry(&port->spi_buf_in_list, struct spi_buf_in_packet, node);
        if(!packet){
            hwlog_info("%s %d: packet  has been not found,try again.\n", __func__, __LINE__);
            continue ;
        }
        list_del(&packet->node);
        kfree(packet->buffer);
        kfree(packet);
    }
    mutex_unlock(&port->spi_buf_in_mutex);
    hwlog_info("%s %d: port %d Leave.\n", __func__, __LINE__, port->index);
}


static const struct tty_port_operations spi_modem_port_ops = {
    .shutdown = spi_modem_shutdown,
    .activate = spi_modem_activate,
};

static const struct tty_operations modem_tty_ops = {
    .open            = modem_tty_open,
    .close            = modem_tty_close,
    .write            = modem_tty_write,
    .write_room        = modem_tty_write_room,
    .chars_in_buffer    = modem_tty_chars_in_buffer,
    .set_termios        = modem_tty_set_termios,
    .tiocmget        = modem_tty_tiocmget,
    .tiocmset        = modem_tty_tiocmset,
    .hangup            = modem_tty_hangup,
    .install        = modem_tty_install,
    .cleanup        = modem_tty_cleanup,
};

static void spi_port_work_name(const char    *name, int index, char *p)
{
    snprintf(p, PAGE_SIZE, "%s%d", name, index);
}

static void respond_cflag80_packet_work(struct work_struct *work)
{
    struct spi_modem_port *port;
    struct spi_modem *modem;
    int ret = 0;
    int status = 0;
    int err_flag = 0;
    int retry_times = 0;
    int nDownRetryTimes = 0;

        struct spi_transfer t = {
                .tx_buf     = via_spi_start_buf,
                .rx_buf     = NULL,
                .len        = VIA_SPI_START_BUF_SIZE,
            };

        struct spi_message    m;

    port = container_of(work, struct spi_modem_port, respond_cflag80_work);
    modem = port->modem;

    nDownRetryTimes = 0;
    do{
        ret = down_timeout(&modem->spi_sem, msecs_to_jiffies(DOWN_MODEM_SEMAPHORE_TIMEOUT_MS));
        if( ret ){
            if( signal_pending(current) || (cbp_power_state==0) )
            {
                hwlog_err("%s %d  down_timeout(modem->spi_sem) return:%d and signal_pending(current) is true or cbp_power_state:%d, return\n", __func__,__LINE__, ret, cbp_power_state);

                if(signal_pending(current))
                {
                    ret =  -ERESTARTSYS;
                }
                if( cbp_power_state==0 )
                {
                    ret = 0;
                }

                goto down_timeout;
            }

            nDownRetryTimes++;
            if(( nDownRetryTimes % DOWN_MODEM_SEMAPHORE_PRINT_LOG) == 0)
            {
                hwlog_err("%s %d  down_timeout(modem->spi_sem) return:%d, print log and continue\n", __func__,__LINE__, ret);
            }
            if (nDownRetryTimes == 1) {
                //if the first time timeout, run notify.
                hw_spi_dsm_client_notify("down_timeout error: status=", ret, DSM_SPI_DOWN_ERROR_NO);
            }
        }else{
            //get the semaphore
            break;
        }
    }while(nDownRetryTimes <= DOWN_MODEM_SEMAPHORE_RETRY_TIMES);
    if(nDownRetryTimes > DOWN_MODEM_SEMAPHORE_RETRY_TIMES )
    {
        hwlog_err("%s %d  down_timeout(modem->spi_sem) exceed %d return\n", __func__,__LINE__, DOWN_MODEM_SEMAPHORE_RETRY_TIMES);
              hw_spi_dsm_client_notify("cflag80 down_timeout timeout exceed. ", 0, DSM_SPI_DOWN_RETRY_MAX_NO);
        goto down_timeout;
    }

    hwlog_debug("%s %d: respond_cflag80_packet_work got semaphore.\n", __func__, __LINE__);

    atomic_set(&modem->cbp_data->cbp_data_ack->state, MODEM_ST_TX_RX);

    /*
     * -------------------------------------------------------------------------
     * |0xFE|ChanInfo(1Byte)|ThanHi(1Byte)|TranLow(1Byte)|Dummy|Paylaod|Padding|
     * -------------------------------------------------------------------------
     */
        spi_tx_cnt++; //also count cflag80 packet.

        via_spi_start_buf[0] = 0xFE;
        via_spi_start_buf[1] = 0x80;
        via_spi_start_buf[2] = 0x0;
        via_spi_start_buf[3] = 0x0;
        via_spi_start_buf[4] = (char)(spi_tx_cnt & 0x000000FF);
        via_spi_start_buf[5] = (char)((spi_tx_cnt & 0x0000FF00) >> 8);
        via_spi_start_buf[6] = (char)((spi_tx_cnt & 0x00FF0000) >> 16);
        via_spi_start_buf[7] = (char)((spi_tx_cnt & 0xFF000000) >> 24);
        via_spi_start_buf[8] = (char)(spi_rx_cnt & 0x000000FF);
        via_spi_start_buf[9] = (char)((spi_rx_cnt & 0x0000FF00) >> 8);
        via_spi_start_buf[10] = (char)((spi_rx_cnt & 0x00FF0000) >> 16);
        via_spi_start_buf[11] = (char)((spi_rx_cnt & 0xFF000000) >> 24);

        memset(spi_tx_current_packet_dump, 0, sizeof(spi_tx_current_packet_dump));
        memcpy(spi_tx_current_packet_dump, via_spi_start_buf, 24);
        spi_is_current_tx = 1;

    do {
        modem->spi_tx_state = TRANS_STATUS_WAITING_INIT_SEGMENT;
        //for ACK always quick than spi sync, so first change spi_state before spi_sync.
        //After send C=1 packet, change SPI state machine to RX state.
        modem->spi_state = SPI_STATE_RX;
        //Change RX substate to INIT.
        modem->spi_rx_state = TRANS_STATUS_WAITING_INIT_SEGMENT;

        spi_message_init(&m);
        spi_message_add_tail(&t, &m);
             status = modem_spi_sync(port->modem->spi, &m);

             if (status) {
                 hwlog_err("%s %d: sync error has reached MAX FAILed times.\n", __func__, __LINE__);
                 hw_spi_dsm_client_notify("read/80 sync error: status=", status, DSM_SPI_SYNC_ERR_MAX_NO);
                 goto down_sem_fail;
             }

        modem->spi_tx_state = TRANS_STATUS_FINISHED;


        hwlog_debug("%s %d: wait_event ack begin, current ack gpio level is %d\n", \
                __func__, __LINE__, oem_gpio_get_value(modem->cbp_data->cbp_data_ack->wait_gpio));
        ret = modem->cbp_data->data_ack_wait_event(modem->cbp_data->cbp_data_ack);
        if (0 == ret) { //success
            if (retry_times > 0) {
                hw_spi_dsm_client_notify("spi_cflag80 Ack timeout retrytimes=", retry_times, DSM_SPI_80_ACK_ERROR_NO);
            }
            break;
        } else if (-1 == ret) {
            respond_cflag80_timeout_cnt++;
            hwlog_err("%s %d: respond_cflag80_packet_work has wait data ack timeout.\n", __func__, __LINE__);
            retry_times++;
            if (retry_times >= MAX_WAIT_DATA_ACK_RETRY) {
                err_flag = 1;
                spi_tx_rx_packets_dump();
                goto down_sem_fail;
            }
        } else if (-2 == ret) {
            hwlog_err("%s %d: VIA reset happened, just return.\n", __func__, __LINE__);
            spi_tx_rx_packets_dump();
            goto down_sem_fail;
        } else {
            hwlog_err("%s %d: wait data ack fail ret=%d.\n ", __func__, __LINE__, ret);
            goto down_sem_fail;
        }
        hwlog_debug("%s %d: wait_event ack end, current ack gpio level is %d\n", \
                __func__, __LINE__, oem_gpio_get_value(modem->cbp_data->cbp_data_ack->wait_gpio));
    } while (1);

    memset(spi_tx_last_packet_dump, 0, sizeof(spi_tx_last_packet_dump));
    memcpy(spi_tx_last_packet_dump, spi_tx_current_packet_dump, 24);
    spi_is_last_tx = 1;

        spi_read_port_work(work);

down_sem_fail:
    up(&modem->spi_sem);

down_timeout:
    hwlog_debug("%s %d: respond_cflag80_packet_work release semaphore.\n", __func__, __LINE__);
    if (err_flag != 0) {
        hwlog_err("%s %d: wait data ack for response cflag80 packet timeout, so notify CBP to produce ramdump.\n", __func__, __LINE__);
        modem_err_indication_usr(1);
    }
}

static void ap_cp_state_adjust(struct cbp_platform_data *pdata)
{
    if (pdata == NULL) {
        return;
    }

    if (asc_tx_check_ready(pdata->tx_handle->name) == 1
        && oem_gpio_get_value(pdata->gpio_cp_ready) == 1)  {
        asc_tx_set_sleep(pdata->tx_handle->name);
    }
}

static int modem_fc_flag = 0;
static void spi_write_port_work(struct work_struct *work)
{
    struct spi_modem_port *port;
    struct spi_modem *modem;
    struct tty_struct *tty;
    unsigned int count;
    unsigned int left, todo, frmlen = 0;
    unsigned int write_len;
    unsigned int fifo_size;
    unsigned long flags = 0;
    unsigned char spi_channel = 0;
    int ret_wait_ack = 0;
    int ret_wait_hardware_flow_ctrl = 0;
    int err_flag = 0;
    int i = 0;
    int retry_times = 0;

    port = container_of(work, struct spi_modem_port, write_work);
    modem = port->modem;

    atomic_set(&modem->cbp_data->cbp_data_ack->state, MODEM_ST_TX_RX);

    spin_lock_irqsave(&port->write_lock, flags);
    count = kfifo_len(&port->transmit_fifo);
    spin_unlock_irqrestore(&port->write_lock, flags);

    spi_channel = (0x0F & (port->index + 1));

    //for AT command problem of /r;
    if(count == 0){
        if( (SPI_AT_CHANNEL_NUM == spi_channel ) \
            || (SPI_AT_CHANNEL_FT_NUM_7 == spi_channel) \
            || (SPI_RILD_CTRL_CHANNEL_NUM == spi_channel) \
            || (SPI_AT_CHANNEL_UICC_NUM_9 == spi_channel) \
            || (SPI_AT_CHANNEL_NUM_10 == spi_channel ) \
            || (SPI_AT_CHANNEL_NUM_11 == spi_channel ) \
            || (SPI_AT_CHANNEL_LOGTOOL_NUM_12 == spi_channel)) {
            hwlog_err("%s %d: port%d -> SPI chanel:%d (count==0)\n", __func__, __LINE__,port->index, spi_channel);
        }

        goto down_out;
    }

    if(modem->cbp_data->flow_ctrl_enable){
        if(oem_gpio_get_value(modem->cbp_data->cbp_flow_ctrl->wait_gpio)!=modem->cbp_data->gpio_flow_ctrl_polar){
            if(FLOW_CTRL_ENABLE == atomic_read(&modem->cbp_data->cbp_flow_ctrl->state))
                atomic_set(&modem->cbp_data->cbp_flow_ctrl->state, FLOW_CTRL_DISABLE);
        }else{
            while(1){/*print added for testing,to be removed*/
                if(cbp_power_state==0){
                    hwlog_err("%s %d: card is removed when channel%d flow is enable,data is dropped\n", __func__, __LINE__, (port->index + 1));
                    goto down_out;
                }

                if(modem_fc_flag < MODEM_FC_PRINT_MAX)
                    hwlog_err("%s %d: channel%d flow ctrl before!\n", __func__, __LINE__, (port->index + 1));
                atomic_set(&modem->cbp_data->cbp_flow_ctrl->state, FLOW_CTRL_ENABLE);

                hwlog_info("%s %d: port %d spi_write_port_work wait flow_ctrl begin, current flow_ctrl gpio level is %d\n", \
                        __func__, __LINE__, port->index, oem_gpio_get_value(modem->cbp_data->cbp_flow_ctrl->wait_gpio));
                ret_wait_hardware_flow_ctrl = modem->cbp_data->flow_ctrl_wait_event(modem->cbp_data->cbp_flow_ctrl);
                if (ret_wait_hardware_flow_ctrl) {
                    if (-1 == ret_wait_hardware_flow_ctrl) {
                        err_flag = 1; //notify CBP to produce ramdump
                        hwlog_err("%s %d: port %d has wait hardware flow ctrl disable timeout.\n", __func__, __LINE__, port->index);
                    } else if (-2 == ret_wait_hardware_flow_ctrl) {
                        hwlog_err("%s %d: port %d VIA reset has happened, just return.\n", __func__, __LINE__, port->index);
                    }
                    spi_tx_rx_packets_dump();
                    goto down_out;
                }
                hwlog_info("%s %d: port %d spi_write_port_work wait flow_ctrl end, current flow_ctrl gpio level is %d\n", \
                        __func__, __LINE__, port->index, oem_gpio_get_value(modem->cbp_data->cbp_flow_ctrl->wait_gpio));

                if(modem_fc_flag < MODEM_FC_PRINT_MAX){
                    hwlog_err("%s %d: channel%d flow ctrl after!\n", __func__, __LINE__, (port->index + 1));
                    modem_fc_flag++;
                }
                if(oem_gpio_get_value(modem->cbp_data->cbp_flow_ctrl->wait_gpio)!=modem->cbp_data->gpio_flow_ctrl_polar){
                    hwlog_err("%s %d: channel%d flow ctrl ok!\n", __func__, __LINE__, (port->index + 1));
                    atomic_set(&modem->cbp_data->cbp_flow_ctrl->state, FLOW_CTRL_DISABLE);
                    modem_fc_flag = 0;
                    break;
                }
            }
        }
    }

    left = count;
    modem->spi_state = SPI_STATE_TX;
    modem->spi_tx_state = TRANS_STATUS_WAITING_INIT_SEGMENT;

    do{

        atomic_set(&modem->cbp_data->cbp_data_ack->state, MODEM_ST_TX_RX);

        if(0 == frmlen) {
            todo = left;
                     //if the chanel 15, it will used to send IP package. and in this condition, we shouldn't split the package.
                     if (port->index == SPI_TTY_NR - 1
                        && check_regist_function() != -1) {
                        frmlen = count;
                     }
                     else if(todo > VIA_SPI_PACKET_PAYLOAD_SIZE) {
                frmlen = VIA_SPI_PACKET_PAYLOAD_SIZE; //1020 bytes
            } else if(todo > 0) {
                frmlen = todo;
            }
        } else {
            todo = frmlen; //reassign todo to small frame
        }

        if (TRANS_STATUS_WAITING_INIT_SEGMENT == modem->spi_tx_state){
            if(todo > (VIA_SPI_FIFO_SIZE - 4)) {
                todo = VIA_SPI_FIFO_SIZE - 4;
            }
        } else if (TRANS_STATUS_WAITING_MORE_SEGMENT == modem->spi_tx_state) {
            if(todo > VIA_SPI_FIFO_SIZE) {
                todo = VIA_SPI_FIFO_SIZE;
            }
        }

/*
 * -------------------------------------------------------------------------
 * |0xFE|ChanInfo(1Byte)|ThanHi(1Byte)|TranLow(1Byte)|Dummy|Paylaod|Padding|
 * -------------------------------------------------------------------------
 */
         if (TRANS_STATUS_WAITING_INIT_SEGMENT == modem->spi_tx_state){
            *modem->trans_buffer = 0xFE;
            *(modem->trans_buffer + 1) = 0x0F & (port->index + 1);
            //this packet length is left, not todo as todo has changed.
            *(modem->trans_buffer + 2) = 0x0F & (frmlen >> 8);
            *(modem->trans_buffer + 3) = 0xFF & frmlen;

            //spin_lock_irqsave(&port->write_lock, flags);
            fifo_size = kfifo_out_locked(&port->transmit_fifo, modem->trans_buffer + 4, todo, &port->write_lock);
            //spin_unlock_irqrestore(&port->write_lock, flags);
            if(todo != fifo_size){
                hwlog_err("%s %d: port%d todo[%d] !=  kfifo lock out size[%d].\n", __func__, __LINE__,port->index, todo, fifo_size);
                todo = fifo_size;
            }
            write_len = (todo + 4 + 3) & ~0x03;  /* Round up to nearest multiple of 4 */
            if((SPI_AT_CHANNEL_NUM == (port->index + 1) ) \
                || (SPI_AT_CHANNEL_FT_NUM_7 == (port->index + 1)) \
                || (SPI_RILD_CTRL_CHANNEL_NUM == (port->index + 1)) \
                || (SPI_AT_CHANNEL_UICC_NUM_9 == (port->index + 1)) \
                || (SPI_AT_CHANNEL_NUM_10 == (port->index + 1) ) \
                || (SPI_AT_CHANNEL_NUM_11 == (port->index + 1) ) \
                || (SPI_AT_CHANNEL_LOGTOOL_NUM_12 == (port->index + 1)) \
                || (print_next_packet_once_after_resume != 0)){
                print_next_packet_once_after_resume = 0;
                spi_tx_rx_printk(modem->trans_buffer, 1);
            }
            spi_tx_cnt_on_channel[port->index + 1]++;
            spi_tx_byte_count_on_channel[port->index + 1] += frmlen;
        } else  if ((TRANS_STATUS_WAITING_MORE_SEGMENT == modem->spi_tx_state) \
            || (TRANS_STATUS_WAITING_LAST_SEGMENT == modem->spi_tx_state)) {
            //spin_lock_irqsave(&port->write_lock, flags);
            fifo_size = kfifo_out_locked(&port->transmit_fifo, modem->trans_buffer, todo, &port->write_lock);
            //spin_unlock_irqrestore(&port->write_lock, flags);
            if(todo != fifo_size){
                hwlog_err("%s %d: port%d todo[%d] !=  kfifo lock out size[%d].\n", __func__, __LINE__,port->index, todo, fifo_size);
                todo = fifo_size;
            }
            write_len = (todo + 3) & ~0x03;  /* Round up to nearest multiple of 4 */
        } else {
            hwlog_err("%s %d: port %d modem tx transfer status is WROING!\n", __func__, __LINE__, port->index);
            goto down_out;
        }

        memset(spi_tx_current_packet_dump, 0, sizeof(spi_tx_current_packet_dump));
        if (write_len < 24) {
            memcpy(spi_tx_current_packet_dump, modem->trans_buffer, write_len);
        } else {
            memcpy(spi_tx_current_packet_dump, modem->trans_buffer, 24);
        }
        spi_is_current_tx = 1;

                do {
                    if(write_len > 0) {
                        modem_spi_write(modem, 0x00, modem->trans_buffer, write_len);
                    }

                    hwlog_debug("spi_write_port_work wait ack begin left packet length is %d, current ack gpio level is %d\n", \
                                    left, oem_gpio_get_value(modem->cbp_data->cbp_data_ack->wait_gpio));
                    ret_wait_ack = modem->cbp_data->data_ack_wait_event(modem->cbp_data->cbp_data_ack);
                    if (-1 == ret_wait_ack) {
                        spi_write_timeout_cnt++;
                        hwlog_err("%s: spi_write_port_work has wait data ack timeout %d times. data_lenth = %d, todo = %d, left include todo= %d\n", \
                            __func__, spi_write_timeout_cnt, count, todo, left);
                        if (retry_times++ < MAX_WAIT_DATA_ACK_RETRY) {
                            ap_cp_state_adjust(modem->cbp_data);
                        }
                        else {
                            err_flag = 1;
                            spi_tx_rx_packets_dump();
                            spi_cur_segment_buf_pos = 0;
                            memset(spi_current_segment_dump, 0, sizeof(spi_current_segment_dump));
                            for (i = 0; i < write_len; i++) {
                                spi_cur_segment_buf_pos += snprintf(&spi_current_segment_dump[spi_cur_segment_buf_pos], (CURRENT_SEGMENT_DUMP_CHAR_SIZE- spi_cur_segment_buf_pos), \
                                    "%x-", via_spi_follow_buf[i]);
                            }
                            hwlog_err("current write segment data is %s\n", spi_current_segment_dump);
                            goto down_out;
                        }
                    } else if (-2 == ret_wait_ack) {
                        hwlog_err("%s: VIA reset has happeded, just return.\n", __func__);
                        spi_tx_rx_packets_dump();
                        goto down_out;
                    }
                    else if (0 ==ret_wait_ack) {
                         if (retry_times > 0) {
                             hw_spi_dsm_client_notify("spi_write Ack timeout retrytimes=", retry_times, DSM_SPI_WRITE_ACK_ERROR_NO);
                         }
                         break;
                    }
                    else {
                        goto down_out;
                    }
                }while (1);

        left -= todo;
        frmlen -= todo;

        if (frmlen > VIA_SPI_FIFO_SIZE){
            modem->spi_tx_state = TRANS_STATUS_WAITING_MORE_SEGMENT;
            modem->spi_state = SPI_STATE_TX;
        } else if(frmlen > 0) {
            modem->spi_tx_state = TRANS_STATUS_WAITING_LAST_SEGMENT;
            modem->spi_state = SPI_STATE_TX;
        } else if(0 == frmlen) {
            spi_tx_cnt++; //count really successfully sent packets,  a frame is a packet.
            if(left > 0) {
                modem->spi_tx_state = TRANS_STATUS_WAITING_INIT_SEGMENT;
                modem->spi_state = SPI_STATE_TX;
            } else {
                modem->spi_tx_state = TRANS_STATUS_FINISHED;
                modem->spi_state = SPI_STATE_IDLE;
            }
        }

        hwlog_debug("%s %d: port %d spi_write_port_work wait ack end left packet length is %d, current ack gpio level is %d\n", \
                        __func__, __LINE__, port->index, left, oem_gpio_get_value(modem->cbp_data->cbp_data_ack->wait_gpio));
    }while(left);

    memset(spi_tx_last_packet_dump, 0, sizeof(spi_tx_last_packet_dump));
    if (write_len < 24) {
        memcpy(spi_tx_last_packet_dump, spi_tx_current_packet_dump, write_len);
    } else {
        memcpy(spi_tx_last_packet_dump, spi_tx_current_packet_dump, 24);
    }
    spi_is_last_tx = 1;

    spin_lock_irqsave(&port->write_lock, flags);
    count = kfifo_len(&port->transmit_fifo);
    spin_unlock_irqrestore(&port->write_lock, flags);

    if (count < SPI_WAKEUP_CHARS) {
        tty = tty_port_tty_get(&port->port);
        if(tty){
            tty_wakeup(tty);
            tty_kref_put(tty);
        }
    }

down_out:
    if (err_flag != 0) {
        hwlog_err("%s %d: channel=%d write error notify VIA CBP to produce ramdump.\n", __func__, __LINE__, spi_channel);
        modem_err_indication_usr(1);
    }
    return;
}

static int check_regist_function(void)
{
    int i = 0;
    for (i = 0; i < MAX_PDN_ID; i++) {
        if (read_func[i].cbp_read_buff != NULL) {
            return i;
        }
    }
    hwlog_err("%s : cannot find regist function.\n", __func__);
    return -1;
}
static int check_regist_function_count(void)
{
    int i = 0;
    int count = 0;
    for (i = 0; i < MAX_PDN_ID; i++) {
        if (read_func[i].cbp_read_buff != NULL) {
            count ++;
        }
    }
    return count;
}
static int rnic_read_packet(unsigned char *buffer, unsigned int datalen)
{
    struct sk_buff *skb = NULL;
    unsigned int packge_size = 0;
    unsigned char pdnId = 0;
    unsigned char *data_ptr = NULL;
    int ret = 0;
    int pdnIdIndex = -1;

    //hwlog_info("RNIC__ %s data_length = %d\n",__func__, datalen);
    //For PDN id will use 1 Byte, so we should get rid off it in the buff.
    //0xFE|ChanInfo(1Byte)|ThanHi(1Byte)|TranLow(1Byte)|PDN-ID(1BYTE)|Payload|Padding| befor "payload", we add 1byte for PDN-ID
    if (datalen > 1) {
        packge_size = datalen - 1;
        pdnId = buffer[0];
        //if pdnId == CDMA_PDNID, that means this condition is evdo, we'll use the registed Pdn id.
        if (pdnId == CDMA_PDNID) {
            pdnIdIndex = check_regist_function();
            if (pdnIdIndex == -1)  {
                hwlog_err("RNIC__ %s don't find the regist function\n",__func__);
                ret = RNIC_NO_REG_FUNCTION;
                goto err;
            }
            pdnId = (unsigned char)pdnIdIndex;
            if (check_regist_function_count() > 1) {
                hwlog_err("RNIC__ %s find more than once regist in CDMA Mode\n",__func__);
                ret = RNIC_REG_FUNCTION_MORE;
            }
        }
        else if (pdnId >= MAX_PDN_ID) {
            hwlog_err("RNIC__ %s invalide PDN ID from VIA modem pdn id = %d\n",__func__, pdnId);
            ret = RNIC_INVAL_PDNID;
            goto err;
        }
        hwlog_debug("RNIC__ %s pdnId = %d  data_length = %d \n",__func__,pdnId, datalen);
    }
    else {
        hwlog_err("RNIC__ %s %d data lenth not valued\n", __func__,__LINE__);
        ret = RNIC_INVAL_PDNID;
        goto err;
    }

    printf_data(buffer,datalen);
    if (read_func[pdnId].cbp_read_buff != NULL) {
        skb = dev_alloc_skb(datalen + SK_BUFFER_RESERVE);
        if (skb != NULL) {
             unsigned long ret = 0;

            skb_reserve(skb, SK_BUFFER_RESERVE);
            data_ptr = (u8 *)skb_put(skb, datalen);

            memcpy(data_ptr, buffer + 1, packge_size);
            ret = read_func[pdnId].cbp_read_buff(pdnId, skb);
            if (RNIC_OK == ret) {
                hwlog_debug("RNIC__ %s read OK\n",__func__);
            }
            else {
                hwlog_err("RNIC__ %s RNIC read error,the error number is %lu\n",__func__,ret);
                if ( (RNIC_PKT_TYPE_INVAL != ret) && (RNIC_ADDMAC_FAIL != ret)){
                    hw_spi_dsm_client_notify(" RNIC read error: ret=", ret, DSM_SPI_DATA_READ_ERROR_NO);
                }
            }
        }
        else {
            hwlog_err("RNIC__ %s cannot alloc skb\n",__func__);
            hw_spi_dsm_client_notify("cannot alloc skb: datalen=", datalen, DSM_SPI_ALLOC_SKB_FAILED_NO);
            ret = RNIC_NOMEM;
        }
    }
    else {
        hwlog_err("RNIC__ %s don't find the regist function\n",__func__);
        ret = RNIC_NO_REG_FUNCTION;
    }

err:
    return ret;
}

//FIXME unsigned long
unsigned long SDIO_DL_RegDataCallback(unsigned char ucPDNId, RCV_C_DL_DATA_FUNC pFunc)
{
    if (ucPDNId >= MAX_PDN_ID) {
        hwlog_err("RNIC__ %s invalid PDN ID pdn id = %d\n",__func__, ucPDNId);
        return RNIC_OUT_RANGE;
    }
    if (pFunc == NULL) {
        hwlog_info("RNIC__ %s unregist call back pdn id = %d\n",__func__, ucPDNId);
    }
    else {
        hwlog_info("RNIC__ %s  Regist call back pdn id = %d\n",__func__, ucPDNId);
    }
    read_func[ucPDNId].cbp_read_buff = pFunc;
    read_func[ucPDNId].ucPDNId = ucPDNId;
    return RNIC_OK;
}
EXPORT_SYMBOL(SDIO_DL_RegDataCallback);

//When show the data, we'll use this function.
static void printf_data(char* buff, int len)
{
    if (SPI_DATA_DEBUT) {
        int i=0;
        for (i=4; i<len; i++) {
            if (buff[i] > 0xf)
            {
                hwlog_info("%x",buff[i]);
            }
            else
            {
                hwlog_info("0%x", buff[i]);
            }
        }
        hwlog_info("\n");
    }
}

static int check_list_empty(tRnicDataFrame *pTx)
{
    unsigned long flags = 0;
    int ret = 0;

    spin_lock_irqsave(&pTx->slock, flags);
    ret = list_empty(&pTx->list_head_node);
    spin_unlock_irqrestore(&pTx->slock, flags);

    return ret;
}

static void del_all_list_note(tRnicDataFrame *pTx)
{
    unsigned long flags = 0;
    struct rnic_buf_in_packet *packet = NULL;

    while (1) {
        spin_lock_irqsave(&pTx->slock, flags);
        if (!list_empty(&pTx->list_head_node)) {
            packet = list_first_entry(&pTx->list_head_node, struct rnic_buf_in_packet, node);
            if(packet){
                list_del(&packet->node);
                dev_kfree_skb(packet->buffer);
                kfree(packet);
            }
        }
        else {
            spin_unlock_irqrestore(&pTx->slock, flags);
            break;
        }
        spin_unlock_irqrestore(&pTx->slock, flags);
    }
    pTx->list_size = 0;
}

static int rnic_send_packet_thread(void *u)
{
    int ret = 0;
    long remain_time = 0;
    struct task_struct  *thread = current;
    struct rnic_buf_in_packet *packet = NULL;
    unsigned long flags = 0;
    struct spi_modem *modem = spi_get_drvdata((struct spi_device *) u);

    hwlog_info("RNIC__ %s:Enter function SendPacket thread\n", __func__);
    while (1) {
        wait_event(rnic_frame.wait, (!check_list_empty(&rnic_frame) || (!thread)));

        /*thread is existed*/
        if(!thread){
            hwlog_err("RNIC__ %s:thread existed\n", __func__);
            del_all_list_note(&rnic_frame);
            break;
        }

        packet = list_first_entry(&rnic_frame.list_head_node, struct rnic_buf_in_packet, node);

        if(packet){
            wake_lock(&rnic_tx_wk);
            spin_lock_irqsave(&rnic_frame.slock, flags);
            list_del(&packet->node);
            if (rnic_frame.list_size > 0) {
                rnic_frame.list_size --;
            }
            else {
                hwlog_info("RNIC__ %s:WARNING:: list_size don't match size = %d\n", __func__,  rnic_frame.list_size);
            }
            spin_unlock_irqrestore(&rnic_frame.slock, flags);
            do {
                remain_time = wait_event_timeout(modem->cbp_data->wait_spi_resume_q, \
                    ON_RESUME == atomic_read(&modem->cbp_data->spi_resumed), \
                    msecs_to_jiffies(WAIT_DATA_ACK_TIMEOUT_ONCE_IN_MS));
                 if (0 == remain_time) {
                    hwlog_err("%s %d: SPI not resume!!!\n", __func__, __LINE__);
                 }
            } while (0 == remain_time);
            ret = rnic_send_packet(packet->buffer, packet->id);
            kfree(packet);
            wake_unlock(&rnic_tx_wk);
        }
        else {
            hwlog_err("RNIC__ %s:can get the right packet\n", __func__);
            return -1;
        }
    }
    return ret;
}

static void rnic_rtate_init(tRnicDataFrame *pTx, struct spi_device *spi)
{
    INIT_LIST_HEAD(&pTx->list_head_node);
    init_waitqueue_head(&pTx->wait);
    spin_lock_init(&pTx->slock);
    wake_lock_init(&rnic_tx_wk, WAKE_LOCK_SUSPEND, "rnic_tx_task");
    pTx->list_size = 0;
    pTx->overflow_flag = 0;
    memset(read_func, 0, sizeof(struct sdio_read_func) * MAX_PDN_ID);
    kthread_run(rnic_send_packet_thread, spi, "SpiSendDataTask");
}

//FIXME unsigned long
unsigned long SDIO_UL_SendPacket(struct sk_buff *pstData, unsigned char ucPDNId)
{
    struct rnic_buf_in_packet *packet = NULL;
    unsigned long flags = 0;
    struct spi_modem_port *port = spi_modem_table[SPI_TTY_NR - 1];

    hwlog_debug("RNIC__ %s: enter, SDIO_UL_SendPacket\n", __func__);

    if (NULL == pstData) {
        hwlog_err("RNIC__ %s: the pstData is NULL\n", __func__);
        return RNIC_PARAM;
    }

    packet = kzalloc(sizeof(struct rnic_buf_in_packet), GFP_KERNEL);
    if (NULL == packet) {
        hwlog_err("RNIC__ %s:can not kzalloc memory\n", __func__);
        return RNIC_NOMEM;
    }

    packet->buffer = pstData;
    packet->id = ucPDNId;

    spin_lock_irqsave(&rnic_frame.slock, flags);
    if (rnic_frame.list_size >= MAX_QUEUE_LENGTH) {
        spin_unlock_irqrestore(&rnic_frame.slock, flags);
        kfree(packet);
        if (!rnic_frame.overflow_flag) {
            rnic_frame.overflow_flag = TRUE;
            if ((port != NULL) &&  (SFLOW_CTRL_DISABLE == atomic_read(&port->sflow_ctrl_state)) ) {
                hw_spi_dsm_client_notify("drop the packet: PDNID=", ucPDNId, DSM_SPI_DATA_DROP_NO);
            }
        }
        hwlog_err("RNIC__ %s:drop the packet\n", __func__);
        return RNIC_EXPIRED;
    }
    else {
        if (rnic_frame.overflow_flag) {
            rnic_frame.overflow_flag = FALSE;
        }
        list_add_tail(&packet->node, &rnic_frame.list_head_node);
        rnic_frame.list_size ++;
    }
    spin_unlock_irqrestore(&rnic_frame.slock, flags);
    wake_lock_timeout(&rnic_tx_wk, HZ);
    wake_up(&rnic_frame.wait);

    return RNIC_OK;
}

void spi_sflow_ctrl_disable(void)
{
    struct spi_modem_port *port = NULL;
    struct spi_modem *modem = NULL;
    int index = 0;

    hwlog_info("RNIC__ %s: set sw flow ctrl disable", __func__);
    for (index = 0; index < SPI_TTY_NR; index++) {
        port =spi_modem_table[index];
        if (port != NULL) {
            atomic_set(&port->sflow_ctrl_state, SFLOW_CTRL_DISABLE);
            modem = port->modem;
        }
    }
    if (modem != NULL) {
        atomic_set(&modem->ctrl_port->sflow_ctrl_state, SFLOW_CTRL_DISABLE);
    }
}
EXPORT_SYMBOL(spi_sflow_ctrl_disable);

static int rnic_send_packet(struct sk_buff *pstData, unsigned char ucPDNId)
{
    struct spi_modem_port *port = spi_modem_table[SPI_TTY_NR - 1];
    unsigned char* pSource = NULL;
    unsigned int count = 0;
    unsigned char * buff = NULL;
    int ret = -1;
    long remain_time;
    int nDownRetryTimes = 0;
    unsigned int nFlowCtrRetryTimes = 0;

    if (pstData == NULL) {
        hwlog_err("RNIC__ %s %d pstData is null\n", __func__,__LINE__);
        return ret;
    }
    pSource = pstData->data;
    count = pstData->len;

    if (pSource == NULL) {
        hwlog_err("RNIC__ %s %d pstData->data is null\n", __func__,__LINE__);
        ret =  -1;
        goto down_out;
    }

    ret = check_port(port);
    if (ret < 0){
        hwlog_err("RNIC__ %s %d check_port failed\n", __func__,__LINE__);
        ret = RNIC_NOTSTARTED;
        goto down_out;
    }

    if(port->inception){
        hwlog_err("RNIC__ %s %d port is inception\n", __func__,__LINE__);
        hw_spi_dsm_client_notify("port is inception: port=", port->index, DSM_SPI_PORT_BUSY_NO);
        ret = RNIC_BUSY;
        goto down_out;
    }

    if (check_regist_function() == -1) {
        hwlog_err("RNIC__ %s %d RNIC read function don't regist\n", __func__,__LINE__);
        ret =  RNIC_NO_REG_FUNCTION;
        goto down_out;
    }

    if(count + 1> VIA_SPI_IP_PACKET_MAX_SIZE){
        hwlog_err("RNIC__ %s %d IP package size overflow!\n", __func__,__LINE__);
        ret =  -1;
        goto down_out;
    }

    if(count == 0){
        hwlog_err("RNIC__ %s %d count is don't avaluable!\n", __func__,__LINE__);
        ret =  -1;
        goto down_out;
    }

    //The fist addr in the buff is used to write PDN ID.
    buff = kzalloc(pstData->len + 1, GFP_KERNEL);
    if (NULL == buff) {
        hwlog_err("RNIC__ %s %d kralloc failed\n", __func__,__LINE__);
        hw_spi_dsm_client_notify("kralloc failed: size=", pstData->len, DSM_SPI_KZALLOC_FAILED_NO);
        ret = RNIC_NOMEM;
        goto down_out;
    }
    buff[0] = ucPDNId;
    memcpy(buff + 1, pSource, count);

    do {
        remain_time = wait_event_timeout(port->sflow_ctrl_wait_q, (SFLOW_CTRL_DISABLE == atomic_read(&port->sflow_ctrl_state) \
            ||(0 == cbp_power_state)), msecs_to_jiffies(2000));
        if (0 == cbp_power_state) {
            hwlog_err("%s %d: port %d wait software contrl disable but VIA CBP reset happened!\n", __func__, __LINE__, port->index);
            goto down_err;
        } else if (0 == remain_time) {
            hwlog_err("%s %d: port %d wait CBP software flow control disable timeout in 2s.\n", __func__, __LINE__, port->index);
            nFlowCtrRetryTimes ++;
            if (DOWN_MODEM_SW_FLOW_COTR_RETRY_TIMES <= nFlowCtrRetryTimes) {
                hwlog_err("%s %d: port %d wait CBP software flow control disable over 5m, then reset Modem.\n", __func__, __LINE__, port->index);
                modem_err_indication_usr(1);
                goto down_err;
            }
        }
    } while (0 == remain_time);

    hwlog_debug("RNIC__ %s: port%d, packge size = %d\n", __func__, port->index, rnic_frame.list_size);


    nDownRetryTimes = 0;
    do{
        ret = down_timeout(&port->modem->spi_sem, msecs_to_jiffies(DOWN_MODEM_SEMAPHORE_TIMEOUT_MS));
        if( ret ){
            if( signal_pending(current) || (cbp_power_state==0) )
            {
                hwlog_err("RNIC %s %d  down_timeout(modem->spi_sem) return:%d and signal_pending(current) is true or cbp_power_state:%d, return\n", __func__,__LINE__, ret, cbp_power_state);
                if(signal_pending(current))
                {
                    ret =  -ERESTARTSYS;
                }
                if( cbp_power_state==0 )
                {
                    ret = 0;
                }

                goto down_err;
            }

            nDownRetryTimes++;
            if(( nDownRetryTimes % DOWN_MODEM_SEMAPHORE_PRINT_LOG) == 0)
            {
                hwlog_err("RNIC %s %d  down_timeout(modem->spi_sem) return:%d, print log and continue\n", __func__,__LINE__, ret);
            }
            if (nDownRetryTimes == 1) {
                //if the first time timeout, run notify.
                hw_spi_dsm_client_notify("down_timeout error: status=", ret, DSM_SPI_DOWN_ERROR_NO);
            }
        }
        else    {
            //get the semaphore
            break;
        }
    }while(nDownRetryTimes <= DOWN_MODEM_SEMAPHORE_RETRY_TIMES);
    if(nDownRetryTimes > DOWN_MODEM_SEMAPHORE_RETRY_TIMES)
    {
        hwlog_err("RNIC %s %d  down_timeout(modem->spi_sem) exceed:%d return\n", __func__,__LINE__, DOWN_MODEM_SEMAPHORE_RETRY_TIMES);
              hw_spi_dsm_client_notify("down_timeout timeout exceed. port index =", port->index, DSM_SPI_DOWN_RETRY_MAX_NO);
        goto down_err;
    }

    kfifo_in_locked(&port->transmit_fifo, buff, count + 1, &port->write_lock);

    spi_write_port_work(&port->write_work);

    up(&port->modem->spi_sem);

down_err:
    kfree(buff);
down_out:
    dev_kfree_skb(pstData);
    return ret;
}
EXPORT_SYMBOL(SDIO_UL_SendPacket);

/*
 * This SPI interrupt handler.
 */
static void spi_read_port_work(struct work_struct *work)
{
    struct spi_modem *modem;
    struct spi_modem_port *port;
    int ret = 0;
    struct tty_struct *tty;
    unsigned char index = 0;
    unsigned char header_retry = 0;
    unsigned char payload_offset = 0;
    struct spi_buf_in_packet *packet = NULL;
       unsigned int total_len = 0;
    int status = 0;
    unsigned int spi_failed_times = 0;
    int ret_read_wait_ack = 0;
    int i = 0;
    int err_flag = 0;
    unsigned int cur_valid_data_len = 0;

    struct spi_transfer xfer = {
            .tx_buf = NULL,
            .rx_buf = via_spi_follow_buf,
            .len = VIA_SPI_FIFO_SIZE,
    };
    struct spi_message  m;
    port = container_of(work, struct spi_modem_port, respond_cflag80_work);

    modem = spi_get_drvdata(port->modem->spi);
    if (!modem) {
        hwlog_err("%s can not get drvdate.\n", __func__);
        return ;
    }
    //only clear head and buffer for first packet.
    modem->msg->head.start_flag = 0;
    modem->msg->head.chanInfo = 0;
    modem->msg->head.tranHi = 0;
    modem->msg->head.tranLow = 0;
    memset(modem->msg->buffer, 0, sizeof(modem->msg->buffer));

    //for ACK always quick than spi sync, so first change spi_state before spi_sync.
    //After send C=1 packet, change SPI state machine to RX state.
    modem->spi_state = SPI_STATE_RX;

    xfer.len = VIA_SPI_PACKET_HEADER_SIZE;

    do {
            spi_message_init(&m);
            spi_message_add_tail(&xfer, &m);
            //READ data from SPI
            status = modem_spi_sync(port->modem->spi, &m);

            if (status) {
             hwlog_err("%s %d: sync error has reached MAX FAILed times.\n", __func__, __LINE__);
             hw_spi_dsm_client_notify("read sync error: status=", status, DSM_SPI_SYNC_ERR_MAX_NO);
             return;
            }

            memcpy(modem->msg, via_spi_follow_buf, VIA_SPI_PACKET_HEADER_SIZE);
            msg_buf_pos = 0;
            total_len= (((modem->msg->head.tranHi & 0x0F) << 8) |
                                (modem->msg->head.tranLow & 0xFF));
        if ((0xFE == modem->msg->head.start_flag) && \
                (modem->msg->head.chanInfo <= SPI_TTY_NR) && (total_len > 0)) {
            break;
        } else {
            hwlog_err("%s %d: packet header is invalid, chan=%d, length=%d\n", \
            __func__, __LINE__, modem->msg->head.chanInfo, total_len);
            mdelay(100);
        }
        header_retry++;
    }while (header_retry < SPI_TRANS_MAX_TRY_TIMES);

    if (header_retry >= SPI_TRANS_MAX_TRY_TIMES) {
        hwlog_err("%s %d: read invalid packet header reach MAX retry times\n", __func__, __LINE__);
        err_flag = 1;
        spi_tx_rx_packets_dump();
        goto out;
    }

    remain_packet_len = (total_len + 3) & ~0x03;  /* Round up to nearest multiple of 4 */

    do{
        /* the first segment of packet has max payload length 508 bytes */
        if (ret_read_wait_ack && (cur_valid_data_len < xfer.len)) { /* only retry read remain data if had, avoid xfer.len=0 */
            xfer.len -= cur_valid_data_len;
        } else if ((TRANS_STATUS_WAITING_INIT_SEGMENT == modem->spi_rx_state) \
            && (remain_packet_len > (VIA_SPI_FIFO_SIZE - 4))) {
            xfer.len = (VIA_SPI_FIFO_SIZE - 4);
        } else if(remain_packet_len > VIA_SPI_FIFO_SIZE) {
            xfer.len = VIA_SPI_FIFO_SIZE;
        } else {
            xfer.len = remain_packet_len;
        }

        atomic_set(&modem->cbp_data->cbp_data_ack->state, MODEM_ST_TX_RX);

            spi_message_init(&m);
            spi_message_add_tail(&xfer, &m);
                     status = modem_spi_sync(port->modem->spi, &m);

                     if (status) {
                         hwlog_err("%s %d: sync error has reached MAX FAILed times.\n", __func__, __LINE__);
                         hw_spi_dsm_client_notify("read sync error: status=", status, DSM_SPI_SYNC_ERR_MAX_NO);
                         return;
                     }

        memcpy(modem->msg->buffer + msg_buf_pos, via_spi_follow_buf, xfer.len);
        msg_buf_pos += xfer.len;

        memset(spi_rx_current_packet_dump, 0, sizeof(spi_rx_current_packet_dump));
        if (xfer.len< 24) {
            memcpy(spi_rx_current_packet_dump, via_spi_follow_buf, xfer.len);
        } else {
            memcpy(spi_rx_current_packet_dump, via_spi_follow_buf, 24);
        }
        spi_is_current_tx = 0;

        hwlog_debug("%s %d: wait ack begin left packet length is %d, current ack gpio level is %d\n", \
        __func__, __LINE__, remain_packet_len, oem_gpio_get_value(modem->cbp_data->cbp_data_ack->wait_gpio));
        ret_read_wait_ack = modem->cbp_data->data_ack_wait_event(modem->cbp_data->cbp_data_ack);
        if (0 == ret_read_wait_ack) {
            if (spi_failed_times > 0) {
                hw_spi_dsm_client_notify("spi_read Ack timeout retrytimes=", spi_failed_times, DSM_SPI_READ_ACK_ERROR_NO);
            }
            /* return success, do nothing */
        } else if (-1 == ret_read_wait_ack) {
            spi_read_timeout_cnt++;
            hwlog_err("%s %d: channel %d has wait data ack timeout %d times. xfer.len = %d, total length=%d, remain length=%d\n", \
            __func__, __LINE__, modem->msg->head.chanInfo, spi_read_timeout_cnt, xfer.len, total_len, remain_packet_len);
            spi_tx_rx_packets_dump();
            spi_cur_segment_buf_pos = 0;
            memset(spi_current_segment_dump, 0, sizeof(spi_current_segment_dump));
            for (i = 0; i < xfer.len; i++) {
                spi_cur_segment_buf_pos += snprintf(&spi_current_segment_dump[spi_cur_segment_buf_pos], (CURRENT_SEGMENT_DUMP_CHAR_SIZE- spi_cur_segment_buf_pos), \
                                                                "%x-", via_spi_follow_buf[i]);
            }
            hwlog_err("current read segment data is %s\n", spi_current_segment_dump);
        } else {
            hwlog_err("%s %d: wait data ack ret=%d\n", __func__, __LINE__, ret_read_wait_ack);
            spi_tx_rx_packets_dump();
            goto out;
        }

        if (ret_read_wait_ack) {
            cur_valid_data_len = 0;
            spi_failed_times++;
        } else {
            cur_valid_data_len = xfer.len;
            spi_failed_times = 0;
        }

        if (spi_failed_times >= MAX_WAIT_DATA_ACK_RETRY) {
            hwlog_err("%s %d: had retry read reached MAX retry times channel=%d\n", __func__, __LINE__, modem->msg->head.chanInfo);
            err_flag = 1;
            goto out;
        }

        /* when wait data ack timeout, calculate received valid data length, and will retry read remain data */
        if (0 == cur_valid_data_len) {
            for (i = 0; i < xfer.len; i += 4) {
                if ((0xFF == via_spi_follow_buf[i]) && (0xFF == via_spi_follow_buf[i + 1]) \
                    && (0xFF == via_spi_follow_buf[i + 2]) && (0xFF == via_spi_follow_buf[i + 3])) {
                    break;
                }
            }
            cur_valid_data_len = i;
            hwlog_err("%s %d: receive data error, current read valid data len=%d, retry read remain if need\n", \
                __func__, __LINE__, cur_valid_data_len);
        }

        remain_packet_len -= cur_valid_data_len;

        if (remain_packet_len > VIA_SPI_FIFO_SIZE) {
            modem->spi_rx_state = TRANS_STATUS_WAITING_MORE_SEGMENT;
        } else if (remain_packet_len > 0) {
            modem->spi_rx_state = TRANS_STATUS_WAITING_LAST_SEGMENT;
        } else {
            modem->spi_rx_state = TRANS_STATUS_FINISHED;
        }

        hwlog_debug("%s %d: wait ack end left packet length is %d, current ack gpio level is %d\n", \
            __func__, __LINE__, remain_packet_len, oem_gpio_get_value(modem->cbp_data->cbp_data_ack->wait_gpio));
    } while(remain_packet_len > 0);

    if((SPI_AT_CHANNEL_NUM == modem->msg->head.chanInfo) \
        || (SPI_AT_CHANNEL_FT_NUM_7 == modem->msg->head.chanInfo) \
        || (SPI_RILD_CTRL_CHANNEL_NUM == modem->msg->head.chanInfo) \
        || (SPI_AT_CHANNEL_UICC_NUM_9 == modem->msg->head.chanInfo) \
        || (SPI_AT_CHANNEL_NUM_10 == modem->msg->head.chanInfo) \
        || (SPI_AT_CHANNEL_NUM_11 == modem->msg->head.chanInfo) \
        || (SPI_AT_CHANNEL_LOGTOOL_NUM_12 == modem->msg->head.chanInfo) \
        || (print_next_packet_once_after_resume != 0)){
        print_next_packet_once_after_resume = 0;
        spi_tx_rx_printk(modem->msg, 0);
    }
    memset(spi_rx_last_packet_dump, 0, sizeof(spi_rx_last_packet_dump));
    if (total_len < 24) {
        memcpy(spi_rx_last_packet_dump, spi_rx_current_packet_dump, total_len);
    } else {
        memcpy(spi_rx_last_packet_dump, spi_rx_current_packet_dump, 24);
    }
    spi_is_last_tx = 0;

    if (modem->msg->head.start_flag != 0xFE){
        hwlog_err("%s %d: start_flag != 0xFE and value is 0x%x, go out.\n",
                __func__, __LINE__, modem->msg->head.start_flag);
        goto out;
    }

    //only count right received packet.
    spi_rx_cnt_on_channel[modem->msg->head.chanInfo]++;
    spi_rx_byte_count_on_channel[modem->msg->head.chanInfo] += total_len;

    if (modem->msg->head.chanInfo > 0 && modem->msg->head.chanInfo < (SPI_TTY_NR + 1))
    {
      if(TRANS_STATUS_FINISHED == modem->spi_rx_state) {
        index = modem->msg->head.chanInfo -1;
        payload_offset = ((modem->msg->head.tranHi & 0xC0) >> 6);
        if (payload_offset)
        {
            hwlog_debug("%s %d: port %d payload_offset = %d.\n", __func__, __LINE__, index, payload_offset);
        }
        modem->data_length =(((modem->msg->head.tranHi & 0x0F) << 8) |
                (modem->msg->head.tranLow & 0xFF));
        if(modem->data_length == 0){
            hwlog_err("%s %d: port %d data_length is 0\n", __func__, __LINE__, index);
            goto out;
        }
        port = modem->port[index];
        ret = check_port(port);
        if (ret < 0)
        {
            hwlog_err("%s %d: port %d check port error\n", __func__, __LINE__, index);
            goto out;
        }

              if (index == SPI_TTY_NR - 1 && check_regist_function() != -1) {
                  mutex_lock(&port->spi_buf_in_mutex);
                  hwlog_debug("%s %d: index = %d data buffered! \n", __func__,__LINE__, index);
                  rnic_read_packet((modem->msg->buffer + payload_offset), (modem->data_length - payload_offset));
                  mutex_unlock(&port->spi_buf_in_mutex);
                  goto out;
              }

        if(port->inception){
            //  rawbulk_push_upstream_buffer(index,(modem->msg->buffer + payload_offset), (modem->data_length - payload_offset));
        } else {
            tty = tty_port_tty_get(&port->port);
            if(!tty)
            {
                mutex_lock(&port->spi_buf_in_mutex);
                port->spi_buf_in_size += (modem->data_length - payload_offset);
                if(port->spi_buf_in_size > SPI_BUF_IN_MAX_SIZE){
                    port->spi_buf_in_size -= (modem->data_length - payload_offset);
                    mutex_unlock(&port->spi_buf_in_mutex);
                    hwlog_err("%s %d: ttySPI%d data buffer overrun!\n", __func__,__LINE__, index);
                }
                else{
                    packet = kzalloc(sizeof(struct spi_buf_in_packet), GFP_KERNEL);
                    if(!packet){
                        hwlog_err("%s %d: port %d kzalloc packet error\n",__func__,__LINE__, index);
                        ret = -ENOMEM;
                        mutex_unlock(&port->spi_buf_in_mutex);
                        tty_kref_put(tty);
                        goto out;
                    }
                    packet->size = modem->data_length - payload_offset;
                    packet->buffer = kzalloc(packet->size, GFP_KERNEL);
                    if(!packet->buffer){
                        hwlog_err("%s %d: port %d kzalloc packet buffer error\n",__func__,__LINE__, index);
                        ret = -ENOMEM;
                        kfree(packet);
                        mutex_unlock(&port->spi_buf_in_mutex);
                        tty_kref_put(tty);
                        goto out;
                    }
                    memcpy(packet->buffer, (modem->msg->buffer + payload_offset), packet->size);

                    if(port->spi_buf_in_num < port->spi_buf_in_max_num){
                        list_add_tail(&packet->node, &port->spi_buf_in_list);
                        port->spi_buf_in_num++;
                    }
                    else{
                        struct spi_buf_in_packet *old_packet = NULL;
                        old_packet = list_first_entry(&port->spi_buf_in_list, struct spi_buf_in_packet, node);
                        if (!old_packet) {
                            hwlog_err("%s %d: the  packet cannot be found\n",__func__,__LINE__);
                            ret = -ENOMEM;
                            kfree(packet);
                            mutex_unlock(&port->spi_buf_in_mutex);
                            tty_kref_put(tty);
                            goto out;
                        }
                        list_del(&old_packet->node);
                        port->spi_buf_in_size -= old_packet->size;
                        kfree(old_packet->buffer);
                        kfree(old_packet);
                        list_add_tail(&packet->node, &port->spi_buf_in_list);
                    }
                    port->spi_buf_in = 1;
                    mutex_unlock(&port->spi_buf_in_mutex);
                    hwlog_debug("%s %d: ttySPI%d data buffered! \n", __func__,__LINE__, index);
                }
            }

            if (tty && modem->data_length) {
                if(port->spi_buf_in ==1){
                    mutex_lock(&port->spi_buf_in_mutex);/*make sure data in list bufeer had been pushed to tty buffer*/
                    mutex_unlock(&port->spi_buf_in_mutex);
                }
                ret = tty_buffer_request_room(&port->port, (modem->data_length - payload_offset));
                if(ret < (modem->data_length - payload_offset)){
                    hwlog_err("%s %d: ttySPI%d no room in tty rx buffer!\n", __func__,__LINE__, index);
                }
                else{
                    ret = tty_insert_flip_string(&port->port, (modem->msg->buffer + payload_offset), (modem->data_length - payload_offset));
                    if(ret < (modem->data_length - payload_offset)){
                        hwlog_err("%s %d: ttySPI%d couldn't insert all characters (TTY is full?)!\n", __func__,__LINE__, index);
                    }
                    else{
                        tty_flip_buffer_push(&port->port);
                    }
                }
            }
            tty_kref_put(tty);
        }
      }
    }
    else if(modem->msg->head.chanInfo ==0)/*control message analyze*/
    {
        ctrl_msg_analyze(modem);
    }
    else{
        hwlog_err("%s %d: error chanInfo is %d, go out.\n",
                __func__, __LINE__, modem->msg->head.chanInfo);
        goto out;
    }

    spi_rx_cnt++; //count really successfully received packets.

out:
    if (err_flag != 0) {
        hwlog_err("%s %d: channel=%d read error notify VIA CBP to produce ramdump.\n", __func__, __LINE__, modem->msg->head.chanInfo);
        modem_err_indication_usr(1);
    }
    return;
}


static void modem_spi_write(struct spi_modem *modem, int addr,
        void *buf, unsigned int len)
{
    struct spi_device *spi = modem->spi;
    unsigned char *print_buf = (unsigned char *)buf;
    unsigned char index = *(print_buf + 1);
    int status;
    int ret = 0;
    unsigned int spi_write_failed_times = 0;

    struct spi_transfer xfer = {
        .tx_buf = via_spi_follow_buf,
        .rx_buf = NULL,
        .len = VIA_SPI_START_BUF_SIZE,
    };
    struct spi_message  m;

    if(cbp_power_state==0){
        hwlog_err("%s %d: card is removed when channel%d flow is enable,data is dropped\n", __func__, __LINE__, index);
        spi_tx_rx_printk(buf, 1);
        goto terminate;
    }


    atomic_set(&modem->cbp_data->cbp_data_ack->state, MODEM_ST_TX_RX);

    //for ACK always quick than spi sync, so first change spi_state before spi_sync.
    //After send C=1 packet, change SPI state machine to RX state.
    modem->spi_state = SPI_STATE_TX;

    //use actual length
    memcpy(via_spi_follow_buf, buf, len);

    xfer.len = len;

    if (TRANS_STATUS_WAITING_INIT_SEGMENT == modem->spi_tx_state){
        if(len > VIA_SPI_START_BUF_SIZE) {
            xfer.len = len;
        } else {
            //for VIA RX FIFO interrupt watermark set to 60.
            xfer.len = VIA_SPI_START_BUF_SIZE;
        }
    } else {
        xfer.len = len;
    }

    spi_message_init(&m);
    spi_message_add_tail(&xfer, &m);
    spi_write_failed_times = 0;
    do {
            if(modem->cbp_data->ipc_enable){
                ret = asc_tx_auto_ready(modem->cbp_data->tx_handle->name, 1);
                if(ret) {
                    hw_spi_dsm_client_notify("can NOT wake up CBP ret=", ret, DSM_SPI_AUTO_READY_ERROR_NO);
                    hwlog_err("%s %d: modem_spi_write can NOT wake up CBP ret=%d!\n", __func__,__LINE__, ret);
                }
            }

        status = spi_sync(spi, &m);
            if (0 == status) {
                break;
            } else {
                hwlog_err("%s - sync error: status=%d", __func__, status);
                if (spi_write_failed_times == 0) { // only notify one times.
                    hw_spi_dsm_client_notify("Write sync error: status=", status, DSM_SPI_SYNC_ERROR_NO);
                }
                msleep(2000);
                spi_write_failed_times++;
            }
        } while (spi_write_failed_times < SPI_TRANS_MAX_TRY_TIMES);

        if (status) {
            hwlog_err("%s %d: sync error has reached MAX FAILed times.\n", __func__, __LINE__);
            hw_spi_dsm_client_notify("write sync error: status=", status, DSM_SPI_SYNC_ERR_MAX_NO);
        }

terminate:
    return;
}

static void modem_port_remove(struct spi_modem *modem)
{
    struct spi_modem_port *port;
    struct tty_struct *tty;
    unsigned long flags = 0;
    int index;
    hwlog_info("%s %d: Enter.\n", __func__, __LINE__);

    for (index = 0; index < SPI_TTY_NR; index++) {
        port = modem->port[index];
        atomic_set(&port->sflow_ctrl_state, SFLOW_CTRL_DISABLE);
        wake_up(&port->sflow_ctrl_wait_q);
        atomic_set(&modem->ctrl_port->sflow_ctrl_state, SFLOW_CTRL_DISABLE);
        wake_up(&modem->ctrl_port->sflow_ctrl_wait_q);
        atomic_set(&modem->cbp_data->cbp_data_ack->state, MODEM_ST_READY);
        wake_up(&modem->cbp_data->cbp_data_ack->wait_q);
        if(port->write_q){
            hwlog_info(    "%s %d: port%d destroy queue before.\n", __func__, __LINE__, index);
            cancel_work_sync(&port->write_work);
            destroy_workqueue(port->write_q);
            hwlog_info(    "%s %d: port%d destroy queue after.\n", __func__, __LINE__, index);
        }
        if(port->respond_cflag80_q){
            hwlog_info(    "%s %d: port%d destroy queue before.\n", __func__, __LINE__, index);
            cancel_work_sync(&port->respond_cflag80_work);
            destroy_workqueue(port->respond_cflag80_q);
            hwlog_info(    "%s %d: port%d destroy queue after.\n", __func__, __LINE__, index);
        }
        if(port->read_q){
            hwlog_info(    "%s %d: port%d destroy queue before.\n", __func__, __LINE__, index);
            cancel_work_sync(&port->read_work);
            destroy_workqueue(port->read_q);
            hwlog_info(    "%s %d: port%d destroy queue after.\n", __func__, __LINE__, index);
        }
        BUG_ON(spi_modem_table[port->index] != port);

        spin_lock_irqsave(&spi_modem_table_lock, flags);
        spi_modem_table[index] = NULL;
        spin_unlock_irqrestore(&spi_modem_table_lock, flags);
        hwlog_info(    "%s %d: spi_modem_table cleared.\n", __func__, __LINE__);
        mutex_lock(&port->port.mutex);
        port->spi = NULL;
        tty = tty_port_tty_get(&port->port);
        /* tty_hangup is async so is this safe as is ?? */
        if (tty) {
            hwlog_info("%s %d destory tty,index=%d port->index=%d\n",__func__,__LINE__,index,port->index);
            tty_hangup(tty);
            tty_kref_put(tty);
        }
        mutex_unlock(&port->port.mutex);

        spi_modem_port_put(port);
    }
    hwlog_info("%s %d: Leave.\n", __func__, __LINE__);
}

static void spi_buffer_in_set_max_len(struct spi_modem_port *port)
{
    unsigned int    index = port->index;
    switch(index)
    {
        case 0:
            port->spi_buf_in_max_num = SPI_PPP_BUF_IN_MAX_NUM;
            break;
        case 1:
            port->spi_buf_in_max_num = SPI_ETS_BUF_IN_MAX_NUM;
            break;
        case 2:
            port->spi_buf_in_max_num = SPI_IFS_BUF_IN_MAX_NUM;
            break;
        case 3:
            port->spi_buf_in_max_num = SPI_AT_BUF_IN_MAX_NUM;
            break;
        case 4:
            port->spi_buf_in_max_num = SPI_PCV_BUF_IN_MAX_NUM;
            break;
        default:
            port->spi_buf_in_max_num = SPI_DEF_BUF_IN_MAX_NUM;
            break;
    }
}

static int spi_modem_port_init(struct spi_modem_port *port, int index)
{
    int ret = 0;
    unsigned long flags = 0;

    kref_init(&port->kref);
    spin_lock_init(&port->write_lock);

    if (kfifo_alloc(&port->transmit_fifo, FIFO_SIZE, GFP_KERNEL)){
        hwlog_err("%s %d: port %d Couldn't allocate transmit_fifo\n",__func__,__LINE__, index);
        return -ENOMEM;
    }

    /* create port's write work queue */
    port->name = "modem_spi_write_wq";
    spi_port_work_name(port->name, index, port->work_name);
    port->write_q = create_singlethread_workqueue(port->work_name);
    if (port->write_q == NULL) {
        hwlog_err("%s %d: port %d error creat write workqueue \n",__func__, __LINE__, index);
        return -ENOMEM;
    }
    port->respond_cflag80_q = create_singlethread_workqueue(port->work_name);
    if (port->respond_cflag80_q == NULL) {
        hwlog_err("%s %d: port %d error creat start_read workqueue \n",__func__, __LINE__, index);
        return -ENOMEM;
    }
    port->read_q = create_singlethread_workqueue(port->work_name);
    if (port->read_q == NULL) {
        hwlog_err("%s %d: port %d error creat read workqueue \n",__func__, __LINE__, index);
        return -ENOMEM;
    }
    INIT_WORK(&port->write_work, spi_write_port_work);
    INIT_WORK(&port->respond_cflag80_work, respond_cflag80_packet_work);
    INIT_WORK(&port->read_work, spi_read_port_work);

    spin_lock_irqsave(&spi_modem_table_lock, flags);
    if (!spi_modem_table[index]) {
        port->index = index;
        spi_modem_table[index] = port;
    }
    spin_unlock_irqrestore(&spi_modem_table_lock, flags);

    mutex_init(&port->spi_buf_in_mutex);
    INIT_LIST_HEAD(&port->spi_buf_in_list);
    port->spi_buf_in = 0;
    port->spi_buf_in_num = 0;
    port->spi_buf_in_size = 0;
    spi_buffer_in_set_max_len(port);

    init_waitqueue_head(&port->sflow_ctrl_wait_q);
    atomic_set(&port->sflow_ctrl_state, SFLOW_CTRL_DISABLE);

    return ret;
}

static ssize_t modem_refer_show(struct kobject *kobj, struct kobj_attribute *attr,
        char *buf)
{
    char *s = buf;
    ssize_t len = 0;
    len += snprintf(s, PAGE_SIZE, "Tx:  times %d\n", spi_tx_cnt);
    len += snprintf(s + len, PAGE_SIZE - len, "\n");
    len += snprintf(s +len, PAGE_SIZE - len, "Rx:  times %d\n", spi_rx_cnt);
    return len;
}

static ssize_t modem_refer_store(struct kobject *kobj, struct kobj_attribute *attr,
        const char *buf, size_t n)
{
    return n;
}

static ssize_t modem_ctrl_on_show(struct kobject *kobj, struct kobj_attribute *attr,
        char *buf)
{
    struct spi_modem_ctrl_port *ctrl_port;
    struct spi_modem *modem;
    struct spi_modem_port *port;
    char *s = buf;
    int ret=-1;
    ssize_t len = 0;

    hwlog_info("%s: enter\n", __func__);

    port = spi_modem_port_get(0);
    ret = check_port(port);
    if (ret < 0){
        hwlog_err("%s %d check_port failed\n", __func__,__LINE__);
        goto out;
    }
    modem = port->modem;
    ctrl_port = modem->ctrl_port;

    len += snprintf(s, PAGE_SIZE, "ctrl state: %s\n", ctrl_port->chan_state ?"enable":"disable");

    return len;

out:
    return ret;
}

static ssize_t modem_ctrl_on_store(struct kobject *kobj, struct kobj_attribute *attr,
        const char *buf, size_t n)
{
    int val;
    if (kstrtoint(buf, 10, &val)) {
        hwlog_err("%s %d: strict_strtoul return error.\n", __func__, __LINE__);
        return -EINVAL;
    }

    if (val < 0) {
        hwlog_err("%s %d: value is invalid.\n", __func__, __LINE__);
        return -EINVAL;
    }

    if (val){
        modem_on_off_ctrl_chan(1);
    }
    else{
        modem_on_off_ctrl_chan(0);
    }
    return n;
}
static ssize_t modem_dtr_send_show(struct kobject *kobj, struct kobj_attribute *attr,
        char *buf)
{
    int status=-1,ret=-1;
    char *s = buf;

    hwlog_info("%s %d: enter\n", __func__, __LINE__);

    status=modem_dcd_state();
    if(ret <0){
        hwlog_info(    "query cp ctrl channel state failed ret=%d\n",ret);
    }

    return snprintf(s, PAGE_SIZE, "ctrl state: %d\n", status);
}

static ssize_t modem_dtr_send_store(struct kobject *kobj, struct kobj_attribute *attr,
        const char *buf, size_t n)
{
    int val;

    if (kstrtoint(buf, 10, &val)) {
        hwlog_err("%s %d: strict_strtoul return error.\n", __func__, __LINE__);
        return -EINVAL;
    }

    if (val < 0) {
        hwlog_err("%s %d: value is invalid.\n", __func__, __LINE__);
        return -EINVAL;
    }

    dtr_value = val;
    modem_dtr_set(val, 1);

    return n;
}

static ssize_t modem_dtr_query_show(struct kobject *kobj, struct kobj_attribute *attr,
        char *buf)
{
    int status=-1,ret=-1;
    char *s = buf;
    ssize_t len = 0;

    hwlog_info("%s %d: enter\n", __func__, __LINE__);

    status = modem_dcd_state();
    if(status <0){
        hwlog_info(    "query cp ctrl channel state failed ret=%d\n",ret);
        len += snprintf(s, PAGE_SIZE, "ctrl state: %s\n","N/A");
    }else{
        len += snprintf(s, PAGE_SIZE, "ctrl state: %d\n", status);
    }
    return len;
}

static ssize_t modem_dtr_query_store(struct kobject *kobj, struct kobj_attribute *attr,
        const char *buf, size_t n)
{
    int val;

    if (kstrtoint(buf, 10, &val)) {
        hwlog_err("%s %d: strict_strtoul return error.\n", __func__, __LINE__);
        return -EINVAL;
    }

    if (val < 0) {
        hwlog_err("%s %d: value is invalid.\n", __func__, __LINE__);
        return -EINVAL;
    }

    modem_dcd_state();
    return n;
}

static unsigned char loop_back_chan = 0;
static ssize_t modem_loop_back_chan_show(struct kobject *kobj, struct kobj_attribute *attr,
        char *buf)
{
    return 0;
}

static ssize_t modem_loop_back_chan_store(struct kobject *kobj, struct kobj_attribute *attr,
        const char *buf, size_t n)
{
    int val;
    if (kstrtoint(buf, 10, &val)) {
        hwlog_err("%s %d: strict_strtoul return error.\n", __func__, __LINE__);
        return -EINVAL;
    }

    if (val < 0) {
        hwlog_err("%s %d: value is invalid.\n", __func__, __LINE__);
        return -EINVAL;
    }
    if (val < 6){
        loop_back_chan = val;
    }
    else{
        hwlog_err("%s %d error channel select, please < 6!\n", __func__,__LINE__);
    }

    return n;
}

static ssize_t modem_loop_back_mod_show(struct kobject *kobj, struct kobj_attribute *attr,
        char *buf)
{
    return 0;
}

static ssize_t modem_loop_back_mod_store(struct kobject *kobj, struct kobj_attribute *attr,
        const char *buf, size_t n)
{
    int val;
    if (kstrtoint(buf, 10, &val)) {
        hwlog_err("%s %d: strict_strtoul return error.\n", __func__, __LINE__);
        return -EINVAL;
    }

    if (val < 0) {
        hwlog_err("%s %d: value is invalid.\n", __func__, __LINE__);
        return -EINVAL;
    }

    if (val < 4){
        modem_loop_back_chan(loop_back_chan, val);
    }
    else{
        hwlog_err("%s %d error channel select, please check the option!\n", __func__,__LINE__);
    }
    return n;
}

static ssize_t modem_ack_show(struct kobject *kobj, struct kobj_attribute *attr,
        char *buf)
{
    char *s = buf;
    struct spi_modem *modem;
    struct spi_modem_port *port;
    int ret =-1;
    ssize_t len = 0;

    if((cbp_pdata!=NULL) && (cbp_pdata->cbp_data_ack!=NULL )){
        len += snprintf(s, PAGE_SIZE, "gpio[%d]\t state:[%d]\t polar[%d]\t ",
                cbp_pdata->cbp_data_ack->wait_gpio ,atomic_read(&cbp_pdata->cbp_data_ack->state),cbp_pdata->cbp_data_ack->wait_polar);
        port = spi_modem_port_get(0);
        ret = check_port(port);
        if (ret < 0){
            hwlog_err("%s %d check_port failed\n", __func__,__LINE__);
            goto out;
        }
        modem = port->modem;
        len += snprintf(s + len, PAGE_SIZE - len, "stored:[%d]\n", oem_gpio_get_value(modem->cbp_data->cbp_data_ack->wait_gpio));
    }
out:
    return len;
}

static ssize_t modem_ack_store(struct kobject *kobj, struct kobj_attribute *attr,
        const char *buf, size_t n)
{

    return n;
}

static ssize_t modem_flw_show(struct kobject *kobj, struct kobj_attribute *attr,
        char *buf)
{
    char *s = buf;
    struct spi_modem *modem;
    struct spi_modem_port *port;
    int ret =-1;
    ssize_t len =0;

    if((cbp_pdata!=NULL) && (cbp_pdata->cbp_flow_ctrl!=NULL )){
        len += snprintf(buf, PAGE_SIZE, "gpio[%d] \tstate:[%d]\t polar[%d]\t ",
                cbp_pdata->cbp_flow_ctrl->wait_gpio,atomic_read(&cbp_pdata->cbp_flow_ctrl->state),cbp_pdata->cbp_flow_ctrl->wait_polar);
        port = spi_modem_port_get(0);
        ret = check_port(port);
        if (ret < 0){
            hwlog_err("%s %d check_port failed\n", __func__,__LINE__);
            goto out;
        }
        modem = port->modem;
        len += snprintf(buf + len, PAGE_SIZE - len, "stored:[%d]\n", oem_gpio_get_value(modem->cbp_data->cbp_flow_ctrl->wait_gpio));
    }
out:
    return len;
}

static ssize_t modem_flw_store(struct kobject *kobj, struct kobj_attribute *attr,
        const char *buf, size_t n)
{

    return n;
}

static ssize_t modem_sflowctrl_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    char *s = buf;
    int i = 0;
    struct spi_modem_port *port = NULL;
    ssize_t len =0;

    len += snprintf(s, PAGE_SIZE, "soft flow ctrl state\nport\t\tsoft flow ctrl\n");

    for( i = 0; i< SPI_TTY_NR; i++ )
    {
        port = spi_modem_port_get(i);
        if( !port )
        {
            continue;
        }

        len += snprintf(s+len, PAGE_SIZE-len, "%d\t\t%d\n", i, atomic_read(&port->sflow_ctrl_state));
    }

    return len;
}


static ssize_t modem_sflowctrl_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t n)
{
#define     S_FLOW_CTRL_BUF_LEN     64
    char* str = NULL;
    int i = 0;
    int portIndex = -1;
    int flag = -1;
    struct spi_modem_port *port = NULL;
    char temp[S_FLOW_CTRL_BUF_LEN] = {0};

    snprintf(temp, S_FLOW_CTRL_BUF_LEN-1, "%s", buf);

    for(i = 0; i < S_FLOW_CTRL_BUF_LEN-1; i++ )
    {
        if(',' == temp[i] )
        {
            str = &temp[i];
            break;
        }
    }

    if( str == NULL )
    {
        return 0;
    }
    if( i >= (S_FLOW_CTRL_BUF_LEN-1) )
    {
        return 0;
    }

    *str = '\0';

    if (kstrtoint(temp, 10, &portIndex)) {
        hwlog_err("%s %d: strict_strtoul return error.\n", __func__, __LINE__);
        return -EINVAL;
    }

    if (kstrtoint(str+1, 10, &flag)) {
        hwlog_err("%s %d: strict_strtoul return error.\n", __func__, __LINE__);
        return -EINVAL;
    }

    if( portIndex < 0 || portIndex >= SPI_TTY_NR)
    {
        return -EINVAL;
    }

    port = spi_modem_port_get(portIndex);
    if( !port )
    {
        return -EINVAL;
    }

    hwlog_err("%s %d: software flow ctrl set port:%d to flow ctrl:%d.\n", __func__, __LINE__, portIndex, flag);

    if( flag )
    {
        atomic_set(&port->sflow_ctrl_state, SFLOW_CTRL_ENABLE);
    }
    else
    {
        atomic_set(&port->sflow_ctrl_state, SFLOW_CTRL_DISABLE);
    }

    return n;
}


#define modem_attr(_name) \
    static struct kobj_attribute _name##_attr = {    \
        .attr    = {                \
            .name = __stringify(_name),    \
            .mode = 0660,            \
        },                    \
        .show    = modem_##_name##_show,            \
        .store    = modem_##_name##_store,        \
    }
modem_attr(refer);
modem_attr(ctrl_on);
modem_attr(dtr_send);
modem_attr(dtr_query);
modem_attr(loop_back_chan);
modem_attr(loop_back_mod);
modem_attr(ack);
modem_attr(flw);
modem_attr(sflowctrl);

static struct attribute * modem_spi_attr[] = {
    &refer_attr.attr,
    &ctrl_on_attr.attr,
    &dtr_send_attr.attr,
    &dtr_query_attr.attr,
    &loop_back_chan_attr.attr,
    &loop_back_mod_attr.attr,
    &ack_attr.attr,
    &flw_attr.attr,
    &sflowctrl_attr.attr,
    NULL,
};
static struct kobject *modem_spi_kobj;
static struct attribute_group g_modem_attr_group = {
    .attrs =modem_spi_attr,
};

int modem_buffer_push(int port_num, void *buf, int count)
{
    int ret, data_len;
    unsigned long flags;
    struct spi_modem_port *port = spi_modem_port_get(port_num);

    if(port_num >= 2)
        port_num++;

    if (NULL == port){
        hwlog_err("%s %d: port is NULL!\n", __FUNCTION__,__LINE__);
        return -1;
    }
    ret = check_port(port);
    if (ret < 0){
        hwlog_err("%s %d: invalid port %d\n", __FUNCTION__,__LINE__, port->index);
        return ret;
    }

    if (count == 0)
        return 0;

    spin_lock_irqsave(&port->write_lock, flags);
    data_len = FIFO_SIZE - kfifo_len(&port->transmit_fifo);
    spin_unlock_irqrestore(&port->write_lock, flags);
    if(data_len < count) {
        hwlog_debug("%s %d: port %d SPI driver buffer is full!\n", __FUNCTION__,__LINE__, port->index);
        return -ENOMEM;
    }

    spin_lock_irqsave(&modem_remove_lock, flags);
    if(modem_remove == 0){
        spin_unlock_irqrestore(&modem_remove_lock, flags);
        ret = kfifo_in_locked(&port->transmit_fifo, buf, count, &port->write_lock);
        queue_work(port->write_q, &port->write_work);
    }
    else{
        spin_unlock_irqrestore(&modem_remove_lock, flags);
        hwlog_info("%s %d: port%d is removed!\n", __func__, __LINE__, port->index);
    }

    return 0;
}
EXPORT_SYMBOL_GPL(modem_buffer_push);
extern void VIA_trigger_signal(int i_on);

static int modem_spi_probe(struct spi_device *spi)
{
    struct spi_modem *modem = NULL;
    struct spi_modem_port *port = NULL;
    int ret = 0;
    int index = 0;
    unsigned long flags;

    hwlog_info("%s %d: enter.\n", __func__, __LINE__);
    modem = kzalloc(sizeof(struct spi_modem), GFP_KERNEL);
    if (!modem){
        hwlog_err("%s %d kzalloc spi_modem failed.\n", __func__, __LINE__);
        ret = -ENOMEM;
        goto err_kzalloc_spi_modem;
    }

    modem->ctrl_port = kzalloc(sizeof(struct spi_modem_ctrl_port), GFP_KERNEL);
    if (!modem->ctrl_port) {
        hwlog_err("%s %d kzalloc ctrl_port failed \n",__func__, __LINE__);
        ret =  -ENOMEM;
        goto err_kzalloc_ctrl_port;
    }

    modem->msg = kzalloc(sizeof(struct spi_msg), GFP_KERNEL);
    if (!modem->msg){
        hwlog_err("%s %d kzalloc spi_msg failed \n",__func__, __LINE__);
        ret = -ENOMEM;
        goto err_kzalloc_spi_msg;
    }

    modem->trans_buffer = kzalloc(TRANSMIT_BUFFER_SIZE, GFP_KERNEL);
    if (!modem->trans_buffer) {
        hwlog_err("%s %d kzalloc trans_buffer failed \n",__func__, __LINE__);
        ret =  -ENOMEM;
        goto err_kzalloc_trans_buffer;
    }

    modem->spi = spi;
    sema_init(&modem->sem, 1);
    spi_set_drvdata(spi, modem);
    modem->cbp_data = cbp_pdata;
    modem->ctrl_port->chan_state = 0;
    init_waitqueue_head(&modem->ctrl_port->sflow_ctrl_wait_q);
    atomic_set(&modem->ctrl_port->sflow_ctrl_state, SFLOW_CTRL_DISABLE);
    sema_init(&modem->spi_sem, 1);

    /* step 3: setup spi */
    spi->mode = SPI_MODE_1;
    spi->max_speed_hz = VIA_SPI_SPEED;
    spi->bits_per_word = 8;
    ret = spi_setup(spi);
    if (ret<0) {
        hwlog_err("%s %d: probe - setup spi error", __func__, __LINE__);
        goto err_enable_func;
    }

    via_spi_start_buf = (char *)__get_free_pages(GFP_KERNEL, 0);
    if(!via_spi_start_buf) {
        hwlog_err("%s %d: probe - spi request via_spi_start_buf memory error", __func__, __LINE__);
        goto err_enable_func;
    }

    via_spi_follow_buf = (char *)__get_free_pages(GFP_KERNEL, 0);
    if(!via_spi_follow_buf) {
        hwlog_err("%s %d: probe - via_spi_follow_buf request memory error", __func__, __LINE__);
        goto err_free_spi_start_buf;
    }

    modem->spi_state = SPI_STATE_IDLE;
    atomic_set(&modem->ack_tx_state, 0);

    for (index = 0; index < SPI_TTY_NR; index++) {
        port = kzalloc(sizeof(struct spi_modem_port), GFP_KERNEL);
        if (!port)
        {
            hwlog_err("%s %d kzalloc spi_modem_port %d failed.\n",
                    __func__, __LINE__, index);
            ret = -ENOMEM;
            goto err_kazlloc_spi_modem_port;
        }

        tty_port_init(&port->port);
        port->spi = spi;
        port->port.ops = &spi_modem_port_ops;
        port->modem = modem;
        modem->port[index] = port;
        spin_lock_init(&port->inception_lock);
        port->inception = false;
    }

    for (index = 0; index < SPI_TTY_NR; index++)
    {
        port = modem->port[index];
        ret = spi_modem_port_init(port, index);
        if (ret){
            hwlog_err("%s %d: spi add port %d failed.\n",__func__, __LINE__, index);
            goto err_spi_modem_port_init;
        }
        else {
            struct device *dev;

            //dev = tty_register_device(modem_spi_tty_driver,
            //        port->index, &spi->dev);
            dev = tty_port_register_device(&port->port, modem_spi_tty_driver,
                    port->index, &spi->dev);
            if (IS_ERR(dev)) {
                ret = PTR_ERR(dev);
                hwlog_err("%s %d: port %d tty register failed \n",__func__,__LINE__, port->index);
                goto err_spi_modem_port_init;
            }
        }
    }

    modem_spi_kobj = viatel_kobject_add("modem_spi");
    if (!modem_spi_kobj){
        hwlog_err("%s %d: viatel_kobject_add return NULL.\n", __func__, __LINE__);
        ret = -ENOMEM;
        goto err_create_kobj;
    }
    hwlog_info("%s %d: exit.\n", __func__, __LINE__);

    cbp_power_state = 1;

    spin_lock_init(&modem_remove_lock);
    INIT_WORK(&dtr_work, modem_dtr_send);
    INIT_WORK(&dcd_query_work, modem_dcd_query);
    spin_lock_irqsave(&modem_remove_lock, flags);
    modem_remove = 0;
    spin_unlock_irqrestore(&modem_remove_lock, flags);
    via_modem = modem;

    rnic_rtate_init(&rnic_frame, spi);
       spi_dsm_client = dsm_register_client(&dsm_modem_spi);
       if (spi_dsm_client == NULL) {
            hwlog_err("%s %d: dsm_register_client register fialed.\n", __func__, __LINE__);
       }
    //VIA_trigger_signal(2);
    return sysfs_create_group(modem_spi_kobj, &g_modem_attr_group);;
err_create_kobj:
err_spi_modem_port_init:
    modem_port_remove(modem);
err_kazlloc_spi_modem_port:
    for (index = 0; index < SPI_TTY_NR; index++) {
        port = modem->port[index];
        if(port){
            kfree(port);
        }
    }

    free_pages((unsigned long)via_spi_follow_buf, 0);
err_free_spi_start_buf:
    free_pages((unsigned long) via_spi_start_buf, 0);
err_enable_func:
    kfree(modem->trans_buffer);
err_kzalloc_trans_buffer:
    kfree(modem->msg);
err_kzalloc_spi_msg:
    kfree(modem->ctrl_port);
err_kzalloc_ctrl_port:
    kfree(modem);
err_kzalloc_spi_modem:
    hwlog_err("%s %d: probe failed with ret=%d.\n", __func__, __LINE__, ret);
    return ret;
}

void modem_reset_handler(void)
{
    struct spi_modem *modem = via_modem;
    struct spi_modem_port *port;
    int index;
    unsigned long flags;

    hwlog_info("%s %d: Enter.\n", __func__, __LINE__);
    if(modem == NULL){
        hwlog_info("%s %d: modem is NULL.\n", __func__, __LINE__);
        goto out;
    }

    for (index= 0; index< SPI_TTY_NR; index++) {
        port = modem->port[index];
        tty_unregister_device(modem_spi_tty_driver, port->index);
        hwlog_info("%s %d: port%d rawbulk_unbind_spi_channel before.\n", __func__, __LINE__, index);
        //rawbulk_unbind_spi_channel(index);
        hwlog_info("%s %d: port%d rawbulk_unbind_spi_channel after.\n", __func__, __LINE__, index);
    }

    spin_lock_irqsave(&modem_remove_lock, flags);
    modem_remove = 1;
    spin_unlock_irqrestore(&modem_remove_lock, flags);
    hwlog_info("%s %d: cancel_work_sync(&dtr_work) before.\n", __func__, __LINE__);
    cancel_work_sync(&dtr_work);
    hwlog_info("%s %d: cancel_work_sync(&dtr_work) after.\n", __func__, __LINE__);
    cancel_work_sync(&dcd_query_work);
    hwlog_info("%s %d: cancel_work_sync(&dcd_query_work) after.\n", __func__, __LINE__);
    dcd_state = 0;

    modem_port_remove(modem);

out:
    hwlog_info("%s %d: Leave.\n", __func__, __LINE__);
}
EXPORT_SYMBOL_GPL(modem_reset_handler);

static int modem_spi_remove(struct spi_device *spi)
{
    struct spi_modem *modem = spi_get_drvdata(spi);

    hwlog_info("%s %d: Enter.\n", __func__, __LINE__);
    modem_reset_handler();
    sysfs_remove_group(modem_spi_kobj, &g_modem_attr_group);
    kobject_put(modem_spi_kobj);

    if(via_spi_start_buf) {
        free_pages((unsigned long) via_spi_start_buf, 0);
    }
    if(via_spi_follow_buf) {
        free_pages((unsigned long)via_spi_follow_buf, 0);
    }

    wake_lock_destroy(&rnic_tx_wk);

    via_modem = NULL;
    if (!modem) {
        hwlog_info("%s %d: cannot get spi_modem,erro.\n", __func__, __LINE__);
        return -ENODEV;
    }
    kfree(modem->trans_buffer);
    kfree(modem->ctrl_port);
    kfree(modem->msg);
    kfree(modem);
    hwlog_info("%s %d: Leave.\n", __func__, __LINE__);

    return 0;
}

static int modem_spi_suspend(struct spi_device *spi, pm_message_t mesg)
{
    struct spi_modem *modem = spi_get_drvdata(spi);

    hwlog_info("%s %d: Enter.\n", __func__, __LINE__);
    if(NULL == modem ) {
        hwlog_info("%s %d: modem is NULL.\n", __func__, __LINE__);
        return 0;
    }
    atomic_set(&modem->cbp_data->spi_resumed, ON_SUSPEND);
    spi_tx_last_packet_print();
    spi_rx_last_packet_print();
    hwlog_info("%s %d: Leave.\n", __func__, __LINE__);
    return 0;
}

static int modem_spi_resume(struct spi_device *spi)
{
    struct spi_modem *modem = spi_get_drvdata(spi);

    hwlog_info("%s %d: Enter.\n", __func__, __LINE__);
    if(NULL == modem ) {
        hwlog_info("%s %d: modem is NULL.\n", __func__, __LINE__);
        return 0;
    }
    print_next_packet_once_after_resume = 1;
    atomic_set(&modem->cbp_data->spi_resumed, ON_RESUME);
    wake_up(&modem->cbp_data->wait_spi_resume_q);
    hwlog_info("%s %d: Leave.\n", __func__, __LINE__);
    return 0;
}

#define SPI_VENDOR_ID_CBP        0x0296
#define SPI_DEVICE_ID_CBP        0x5347

static const struct of_device_id modem_spi_ids[] = {
    { .compatible = "spi_dev4" },
    {},
};

MODULE_DEVICE_TABLE(of, modem_spi_ids);

static struct spi_driver modem_spi_driver = {
    .driver = {
        .name = "spi_dev4",
        .owner = THIS_MODULE,
        .of_match_table = of_match_ptr(modem_spi_ids),
    },
    .probe        = modem_spi_probe,
    .remove        = modem_spi_remove,
    .suspend        = modem_spi_suspend,
    .resume        = modem_spi_resume,
};

int  modem_spi_init(struct cbp_platform_data *pdata)
{
    int ret;
    struct tty_driver *tty_drv;

    modem_spi_tty_driver = tty_drv = alloc_tty_driver(SPI_TTY_NR);
    cbp_pdata = pdata;
    if (!tty_drv) {
        hwlog_err("%s %d: alloc_tty_driver failed, no memory!.\n", __func__, __LINE__);
        return -ENOMEM;
    }

    tty_drv->owner = THIS_MODULE;
    tty_drv->driver_name = "modem_spi";
    tty_drv->name =   "ttySPI";
    tty_drv->major = 0;  // dynamically allocated
    tty_drv->minor_start = 0;
    tty_drv->type = TTY_DRIVER_TYPE_SERIAL;
    tty_drv->subtype = SERIAL_TYPE_NORMAL;
    tty_drv->flags = TTY_DRIVER_REAL_RAW | TTY_DRIVER_DYNAMIC_DEV;
    tty_drv->init_termios = tty_std_termios;
    tty_drv->init_termios.c_cflag = B9600 | CS8 | CREAD | HUPCL | CLOCAL;
    tty_drv->init_termios.c_cflag &= ~(CSIZE | CSTOPB | PARENB | PARODD);
    tty_drv->init_termios.c_cflag |= CREAD | CLOCAL | CS8 ;
    tty_drv->init_termios.c_cflag &= ~(CRTSCTS);
    tty_drv->init_termios.c_lflag &= ~(ICANON | ECHO | ECHOE | ECHOK | ECHONL | ISIG);
    tty_drv->init_termios.c_iflag &= ~(INPCK | IGNPAR | PARMRK | ISTRIP | IXANY | ICRNL);
    tty_drv->init_termios.c_iflag &= ~(IXON | IXOFF);
    tty_drv->init_termios.c_oflag &= ~(OPOST | OCRNL);
    tty_drv->init_termios.c_ispeed = 9600;
    tty_drv->init_termios.c_ospeed = 9600;
    tty_set_operations(tty_drv, &modem_tty_ops);

    ret = tty_register_driver(tty_drv);
    if (ret) {
        hwlog_err("%s %d: tty_register_driver failed.\n", __func__, __LINE__);
        goto exit_reg_driver;
    }

    ret = spi_register_driver(&modem_spi_driver);
    if (ret) {
        hwlog_err("%s %d: spi_register_driver failed.\n", __func__, __LINE__);
        goto exit_tty;
    }

    hwlog_info("%s %d: spi driver is initialized successfully!\n", __func__, __LINE__);
    return ret;
exit_tty:
    hwlog_err("%s %d: error exit result call tty_unregister_driver.\n", __func__, __LINE__);
    tty_unregister_driver(tty_drv);
exit_reg_driver:
    hwlog_err("%s %d: returning with error %d\n", __func__, __LINE__, ret);
    put_tty_driver(tty_drv);
    return ret;
}

void  modem_spi_exit(void)
{
    spi_unregister_driver(&modem_spi_driver);
    tty_unregister_driver(modem_spi_tty_driver);
    put_tty_driver(modem_spi_tty_driver);
}

