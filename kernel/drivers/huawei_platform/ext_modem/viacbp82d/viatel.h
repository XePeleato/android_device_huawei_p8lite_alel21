#ifndef VIATEL_H
#define VIATEL_H

#include <linux/interrupt.h>
#include <huawei_platform/log/hw_log.h>
#include "core.h"

#define GPIO_OEM_UNKNOW (-1)
#define GPIO_OEM_VALID(gpio) ((gpio == GPIO_OEM_UNKNOW) ? 0 : 1)

#define MDM_SPI_ID "spi1"

#undef HWLOG_TAG
#define HWLOG_TAG viacbp82d_3rd_modem
HWLOG_REGIST();
//HWLOG_REGIST_LEVEL(HWLOG_ERR|HWLOG_WARNING | HWLOG_INFO|HWLOG_DEBUG|HWLOG_DEBUG1|HWLOG_DEBUG2|HWLOG_DEBUG3|HWLOG_DEBUG4);
//////////////////////////////////////////////////////////////////////////////////
/*******************************  Gpio Config ***********************************/
//////////////////////////////////////////////////////////////////////////////////

extern int cbp_rst_gpio;
extern int cbp_pwr_en_gpio;
extern int cbp_rst_ind_gpio;


#define GPIO_VIATEL_MDM_RST_IND_POLAR 0
#define GPIO_VIATEL_SPI_SYNC_POLAR 0

#ifdef GPIO_VIA_CBP_BCKUP_1
#define GPIO_VIATEL_CRASH_CBP GPIO_VIA_CBP_BCKUP_1
#else
#define GPIO_VIATEL_CRASH_CBP GPIO_OEM_UNKNOW
#endif

#define GPIO_VIATEL_SPI_DATA_ACK_POLAR 0

#define GPIO_VIATEL_SPI_FLOW_CTRL_POLAR 1

#define GPIO_VIATEL_SPI_VIA_REQUEST_TO_SEND_POLAR 1

//////////////////////////////////////////////////////////////////////////////////
/****************************** Gpio Function   *********************************/
//////////////////////////////////////////////////////////////////////////////////
int oem_gpio_request(int gpio, const char *label);
void oem_gpio_free(int gpio);
/*config the gpio to be input for irq if the SOC need*/
int oem_gpio_direction_input_for_irq(int gpio);
int oem_gpio_direction_output(int gpio, int value);
int oem_gpio_get_value(int gpio);
int oem_gpio_get_value_cansleep(int gpio);
int oem_gpio_to_irq(int gpio);
int oem_irq_to_gpio(int irq);
int oem_gpio_set_irq_type(int gpio, unsigned int type);
int oem_gpio_request_irq(int gpio, irq_handler_t handler, unsigned long flags, const char *name, void *dev);
int oem_gpio_request_threaded_irq(int gpio, irq_handler_t handler, unsigned long flags, const char *name, void *dev);
void oem_gpio_irq_mask(int gpio);
void oem_gpio_irq_unmask(int gpio);
int oem_get_gpio_number(char *str, int *gpio);
//////////////////////////////////////////////////////////////////////////////////
/*******************************  Sync Control **********************************/
//////////////////////////////////////////////////////////////////////////////////
/* notifer events */
#define ASC_NTF_TX_READY      0x0001 /*notifie CBP is ready to work*/
#define ASC_NTF_TX_UNREADY    0x0002 /*notifie CBP is not ready to work*/
#define ASC_NTF_RX_PREPARE    0x1001 /* notifier the device active to receive data from CBP*/
#define ASC_NTF_RX_POST       0x1002 /* notifer the device CBP stop tx data*/

#define ASC_NAME_LEN   (64)

/*used to register handle*/
struct asc_config{
    int gpio_ready;
    int gpio_wake;
    /*the level which indicate ap is ready*/
    int polar;
    char name[ASC_NAME_LEN];
};

/*Used to registe user accoring to handle*/
struct asc_infor {
    void *data;
    int (*notifier)(int, void *);
    char name[ASC_NAME_LEN];
};

#define CBP_TX_HD_NAME "TxHdCbp"
#define CBP_TX_USER_NAME "cbp"

#define SPI_RX_HD_NAME "RxHdSpi"
#define SPI_RX_USER_NAME "spi"

#define RAWBULK_RX_USER_NAME "rawbulk"

#define ASC_PATH(hd, user) hd"."user

int asc_tx_register_handle(struct asc_config *cfg);
int asc_tx_set_sleep(const char *name);
int asc_tx_add_user(const char *name, struct asc_infor *infor);
void asc_tx_del_user(const char *path);
int asc_tx_get_ready(const char *path, int sync);
int asc_tx_put_ready(const char *path, int sync);
int asc_tx_auto_ready(const char *name, int sync);
int asc_tx_check_ready(const char *name);
int asc_tx_set_auto_delay(const char *name, int delay);
int asc_tx_user_count(const char *path);
void asc_tx_reset(const char *name);

int asc_rx_register_handle(struct asc_config *cfg);
int asc_rx_add_user(const char *name, struct asc_infor *infor);
void asc_rx_del_user(const char *path);
int asc_rx_confirm_ready(const char *name, int ready);
void asc_rx_reset(const char *name);
int asc_rx_check_on_start(const char *name);

//////////////////////////////////////////////////////////////////////////////////
/*******************************  Power Control *********************************/
//////////////////////////////////////////////////////////////////////////////////
/* modem event notification values */
enum clock_event_nofitiers {
    MDM_EVT_NOTIFY_POWER_ON = 0,
    MDM_EVT_NOTIFY_POWER_OFF,
    MDM_EVT_NOTIFY_RESET_ON,
    MDM_EVT_NOTIFY_RESET_OFF,
    MDM_EVT_NOTIFY_HD_ERR,
    MDM_EVT_NOTIFY_HD_ENHANCE,
    MDM_EVT_NOTIFY_NUM
};

void modem_notify_event(int event);
int modem_register_notifier(struct notifier_block *nb);
int modem_unregister_notifier(struct notifier_block *nb);
#endif
