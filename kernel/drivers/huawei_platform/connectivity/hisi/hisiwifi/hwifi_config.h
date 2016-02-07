

#ifndef __HWIFI_INI_CTRL_H__
#define __HWIFI_INI_CTRL_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif




/*
 * 1 Other Header File Including
 */

/*
 * 2 Macro Definition
 */


/*
 * 3 Enum Type Definition
 */
enum HISI_CONFIG_WIFI
{
    HISI_CONFIG_WIFI_COUNTRY_CODE = 0,
    HISI_CONFIG_WIFI_DATA_RATE_DOWN,
    HISI_CONFIG_WIFI_DATA_RATE_UP,
    HISI_CONFIG_WIFI_BA_MAX_TX,
    HISI_CONFIG_WIFI_BA_MAX_RX,
    HISI_CONFIG_WIFI_BA_MAX_BUFFERSIZE,
    HISI_CONFIG_WIFI_HCC_ASSEMBLE_COUNT,

    HISI_CONFIG_WIFI_HCC_RX_LO_QUEUE,
    HISI_CONFIG_WIFI_HCC_RX_TCP_DATA_QUEUE,
    HISI_CONFIG_WIFI_HCC_RX_TCP_ACK_QUEUE,
    HISI_CONFIG_WIFI_TX_ACK_LIMIT,
    HISI_CONFIG_WIFI_HCC_TX_LO_QUEUE,
    HISI_CONFIG_WIFI_HCC_TX_TCP_DATA_QUEUE,
    HISI_CONFIG_WIFI_HCC_TX_TCP_ACK_QUEUE,
    HISI_CONFIG_WIFI_RX_ACK_LIMIT,

    HISI_CONFIG_WIFI_WMM_DROP_CNT_PER_TIME,
    HISI_CONFIG_WIFI_WMM_DROP_INTERVAL,
    HISI_CONFIG_WIFI_QUE_LEN_FOR_DROP,
    HISI_CONFIG_WIFI_WMM_AC_OPT,

    HISI_CONFIG_WIFI_CUR_CPUFREQ,
    HISI_CONFIG_WIFI_SDIO_RATE_40M,
    HISI_CONFIG_WIFI_SDIO_RATE_80M,
    HISI_CONFIG_WIFI_SDIO_RATE_160M,
    HISI_CONFIG_WIFI_POWERMGMT_SWITCH,
    HISI_CONFIG_WIFI_WATCHDOG_TIMEOUT,

    HISI_CONFIG_WIFI_BAND_5G_ENABLEED,

    HISI_CONFIG_WIFI_LOGLEVEL,

    HISI_CONFIG_WIFI_PANIC_DEBUG,

    HISI_CONFIG_WIFI_TXRX_THREAD_TPYE,

    HISI_CONFIG_WIFI_AP_DATA_RATE_DOWN,
    HISI_CONFIG_WIFI_AP_DATA_RATE_UP,

    HISI_CONFIG_WIFI_EXCEPTION_ENABLE,

    HISI_CONFIG_WIFI_HB_WORK_TIMEOUT,

    HISI_CONFIG_WIFI_HB_SLEEP_TIMEOUT,

    HISI_CONFIG_WIFI_COUNTRYCODE_CHANGE_BY_IE,

    HISI_CONFIG_WIFI_ROAM_SWITCH,

    HISI_CONFIG_WIFI_AP_COEX_2040M_ENABLED,

    HISI_CONFIG_WIFI_STA_2040M_ENABLED,

    HISI_CONFIG_WIFI_AP_2040M_ENABLED,

    HISI_CONFIG_WIFI_ROAM_TRIGGER_RSSI_THRESHOLD,

    HISI_CONFIG_WIFI_ROAM_TRIGGER_COUNT_THRESHOLD,

    HISI_CONFIG_WIFI_BUTT
};


/*
 * 4 Global Variable Declaring
 */

/*
 * 5 Message Header Definition
 */


/*
 * 6 Message Definition
 */


/*
 * 7 STRUCT Type Definition
 */
 struct hwifi_config_cmd  {
      int8              *name;
      int32              case_entry;
 };


/*
 * 8 UNION Type Definition
 */


/*
 * 9 OTHERS Definition
 */


/*
 * 10 Function Declare
 */
extern int32 hwifi_get_mac_addr(uint8 *buf);
extern int32 hwifi_config_init(void);
extern int32 hwifi_get_param_int(int32 config_id);
extern int8* hwifi_grec_point(void);


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of hwifi_config.h */







