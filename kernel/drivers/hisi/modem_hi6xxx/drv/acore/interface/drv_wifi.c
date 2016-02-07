/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : drv_wifi.c
  版 本 号   : 初稿
  作    者   : c61362
  生成日期   : 2012年3月2日
  最近修改   :
  功能描述   : 底软给上层软件封装的接口层
  修改历史   :
  1.日    期   : 2012年3月2日
    作    者   : c61362
    修改内容   : 新建Drvinterface.c

  2.日    期   : 2013年2月19日
    作    者   : 蔡喜 220237
    修改内容   : 由Drvinterface.c拆分所得

******************************************************************************/

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#if (defined BSP_CORE_APP)
#include <linux/module.h>
#endif
#include "BSP.h"
#if defined(BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
#include "sys_config.h"
#endif


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

extern int printf(const char *format, ...);

/*****************************************************************************
  2 函数声明
*****************************************************************************/

/*****************************************************************************
  3 函数实现
*****************************************************************************/

/*****************************************************************************
 函 数 名  : DRV_AT_GET_USER_EXIST_FLAG
 功能描述  : 返回当前是否有USB连接或者WIFI用户连接
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 1 : 有
             0 : 无
*****************************************************************************/
unsigned int DRV_AT_GET_USER_EXIST_FLAG(void)
{
#if defined(BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
    DRV_PRINTF("DRV_AT_GET_USER_EXIST_FLAG ++!\n");
    DRV_PRINTF("DRV_AT_GET_USER_EXIST_FLAG --!\n");
	return 0;
#endif

#if (defined BSP_CORE_APP)
    return BSP_TRUE;
#endif
}

/*****************************************************************************
 函 数 名  : WIFI_GET_TCMD_MODE
 功能描述  : 获取测试命令的模式
 输入参数  : 无
 输出参数  : 16：校准测试模式
             17：表示处于发射模式
             18：表示接收模式
 返回值    ：函数执行的状态结果值
*****************************************************************************/
int WIFI_GET_TCMD_MODE(void)
{
    return 0;
#if 0
#if defined (BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
    return 0;   /* 打桩 */
#endif

#if (defined BSP_CORE_APP)
#if (FEATURE_WIFI == FEATURE_ON)
    return wifi_get_tcmd_mode();
#else
    DRV_PRINTF("Function WIFI_GET_TCMD_MODE unsupport!\n");
    return 0;
#endif
#endif
#endif
}

/*****************************************************************************
 函 数 名  : WIFI_POWER_START
 功能描述  : WIFI上电
 输入参数  : 无
 输出参数  : 无
 返回值    ： 0: execute ok
              1: execute failed
*****************************************************************************/
int WIFI_POWER_START(void)
{
    return BSP_ERROR;
#if 0
#if defined (BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
    return 0;   /* 打桩 */
#endif

#if (defined BSP_CORE_APP)
#if (FEATURE_WIFI == FEATURE_ON)
    return wifi_power_on();
#else
    DRV_PRINTF("Function WIFI_POWER_START unsupport!\n");
    return 1;
#endif
#endif
#endif
}

/*****************************************************************************
 函 数 名  : WIFI_TEST_CMD
 功能描述  : 测试命令
 输入参数  : cmdStr；命令字符串
 输出参数  : 无
 返回值    ：无
*****************************************************************************/
void WIFI_TEST_CMD(char * cmdStr)
{
    return;
#if 0
#if defined (BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
    return;   /* 打桩 */
#endif

#if (defined BSP_CORE_APP)
#if (FEATURE_WIFI == FEATURE_ON)
    wifi_tcmd(cmdStr);
#else
    DRV_PRINTF("Function WIFI_TEST_CMD unsupport!\n");
#endif
#endif
#endif
}

/*****************************************************************************
 函 数 名  : WIFI_POWER_SHUTDOWN
 功能描述  : WIFI下电
 输入参数  : 无
 输出参数  : 无
 返回值    ： 0: execute ok
              1: execute failed
*****************************************************************************/
int WIFI_POWER_SHUTDOWN(void)
{
    return BSP_ERROR;
#if 0
#if defined (BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
    return 0;   /* 打桩 */
#endif

#if (defined BSP_CORE_APP)
#if (FEATURE_WIFI == FEATURE_ON)
    return wifi_power_off();
#else
    DRV_PRINTF("Function WIFI_POWER_SHUTDOWN unsupport!\n");
    return 1;
#endif
#endif
#endif
}

/*****************************************************************************
 函 数 名  : WIFI_GET_STATUS
 功能描述  : WIFI状态获取
 输入参数  : 无
 输出参数  : 无
 返回值    ： 0: wifi is off
              1: wifi is in normal mode
              2: wifi is in tcmd mode
*****************************************************************************/
int WIFI_GET_STATUS(void)
{
    return 0;
#if 0
#if defined (BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
    return 0;   /* 打桩 */
#endif

#if (defined BSP_CORE_APP)
#if (FEATURE_WIFI == FEATURE_ON)
    return wifi_get_status();
#else
    DRV_PRINTF("Function WIFI_GET_STATUS unsupport!\n");
    return 0;
#endif
#endif
#endif
}

/*****************************************************************************
 函 数 名  : WIFI_GET_RX_DETAIL_REPORT
 功能描述  : get result of rx report: totalPkt, GoodPkt, ErrorPkt
 输入参数  : 无
 输出参数  : totalPkt、goodPkt、badPkt
 返回值    ：无
*****************************************************************************/
void WIFI_GET_RX_DETAIL_REPORT(int* totalPkt,int* goodPkt,int* badPkt)
{
    return;
#if 0
#if defined (BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
    return;   /* 打桩 */
#endif

#if (defined BSP_CORE_APP)
#if (FEATURE_WIFI == FEATURE_ON)
    wifi_get_rx_detail_report(totalPkt, goodPkt, badPkt);
#else
    DRV_PRINTF("Function WIFI_GET_RX_DATAIL_REPORT unsupport!\n");
#endif
#endif
#endif
}

/*****************************************************************************
 函 数 名  : WIFI_GET_RX_PACKET_REPORT
 功能描述  : get result of rx ucast&mcast packets
 输入参数  : 无
 输出参数  : ucastPkts、mcastPkts
 返回值    ：无
*****************************************************************************/
void WIFI_GET_RX_PACKET_REPORT(unsigned int *ucastPkts, unsigned int *mcastPkts)
{
    return;
#if 0
#if defined (BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
    return;   /* 打桩 */
#endif

#if (defined BSP_CORE_APP)
#if (FEATURE_WIFI == FEATURE_ON)
    wifi_get_rx_packet_report(ucastPkts, mcastPkts);
#else
    DRV_PRINTF("Function WIFI_GET_RX_PACKET_REPORT unsupport!\n");
#endif
#endif
#endif
}

/*****************************************************************************
 函 数 名  : WIFI_GET_PA_CUR_MODE
 功能描述  : get the currrent PA mode of the wifi chip
 输入参数  : 无
 输出参数  : 无
 返回值    ：0:  (WIFI_ONLY_PA_MODE) WIFI chip is in PA mode
             1:  (WIFI_ONLY_NOPA_MODE) WIFI chip is in no PA mode
             -1: wifi chip is in abnormal mode
*****************************************************************************/
int WIFI_GET_PA_CUR_MODE(void)
{
    return BSP_ERROR;
#if 0
#if defined (BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
    return 0;   /* 打桩 */
#endif

#if (defined BSP_CORE_APP)
#if (FEATURE_WIFI == FEATURE_ON)
    wifi_get_pa_curMode();
#else
    DRV_PRINTF("Function WIFI_GET_PA_CUR_MODE unsupport!\n");
    return -1;
#endif
#endif
#endif
}

/*****************************************************************************
 函 数 名  : WIFI_GET_PA_MODE
 功能描述  : get the support PA mode of wifi chip
 输入参数  : 无
 输出参数  : 无
 返回值    ：0:  (WIFI_ONLY_PA_MODE) WIFI suppport only PA mode
             1:  (WIFI_ONLY_NOPA_MODE) WIFI suppport only no PA mode
             2:  (WIFI_PA_NOPA_MODE) WIFI suppport both PA &  no PA mode
             -1: failed
*****************************************************************************/
int WIFI_GET_PA_MODE(void)
{
    return BSP_ERROR;
#if 0
#if defined (BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
    return 0;   /* 打桩 */
#endif

#if (defined BSP_CORE_APP)
#if (FEATURE_WIFI == FEATURE_ON)
    wifi_get_pa_mode();
#else
    DRV_PRINTF("Function WIFI_GET_PA_MODE unsupport!\n");
    return -1;
#endif
#endif
#endif
}

/*****************************************************************************
 函 数 名  : WIFI_SET_PA_MODE
 功能描述  : set the PA mode of wifi chip
 输入参数  : 0:  (WIFI_ONLY_PA_MODE) WIFI suppport only PA mode
             1:  (WIFI_ONLY_NOPA_MODE) WIFI suppport only no PA mode
             2:  (WIFI_PA_NOPA_MODE) WIFI suppport both PA &  no PA mode
 输出参数  : 无
 返回值    ：0: success
             -1: failed
*****************************************************************************/
int WIFI_SET_PA_MODE(int wifiPaMode)
{
    return BSP_ERROR;
#if 0
#if defined (BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
    return 0;   /* 打桩 */
#endif

#if (defined BSP_CORE_APP)
#if (FEATURE_WIFI == FEATURE_ON)
    wifi_set_pa_mode(wifiPaMode);
#else
    DRV_PRINTF("Function WIFI_SET_PA_MODE unsupport!\n");
    return -1;
#endif
#endif
#endif
}

/*****************************************************************************
 函 数 名  : DRV_WIFI_DATA_RESERVED_TAIL
 功能描述  : WIFI计算需要数据块内存
 输入参数  : usLen - 用户申请数据长度Len
 输出参数  : 无
 返 回 值  : 数据区的尾部预留的长度
*****************************************************************************/
unsigned int DRV_WIFI_DATA_RESERVED_TAIL(unsigned int len)
{
    return 0;
#if 0
#if defined (BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
    return 0;   /* 打桩 */
#endif

#if (defined BSP_CORE_APP)
#if (FEATURE_WIFI == FEATURE_ON)
    return BSP_WifiDataReservedTail(len);
#else
    DRV_PRINTF("Function DRV_WIFI_DATA_RESERVED_TAIL unsupport!\n");
    return 0;
#endif
#endif
#endif
}

/*****************************************************************************
 函 数 名  : DRV_WIFI_CLR_RX_FCTL
 功能描述  : 清除WIFI接收流控标识
 输入参数  : para1、para2
 输出参数  : 无
 返 回 值  : 1 : 有
             0 : 无
*****************************************************************************/
unsigned int DRV_WIFI_CLR_RX_FCTL(unsigned int para1, unsigned int para2)
{
    return (unsigned int)BSP_ERROR;
#if 0
#if defined (BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
    return 0;   /* 打桩 */
#endif

#if (defined BSP_CORE_APP)
#if (FEATURE_WIFI == FEATURE_ON)
    return WiFi_DrvClearRxFlowCtrl(para1, para2);
#else
    DRV_PRINTF("Function DRV_WIFI_CLR_RX_FCL unsupport!\n");
    return (unsigned long)BSP_ERROR;
#endif
#endif
#endif
}

/*****************************************************************************
 函 数 名  : WiFi_DrvSetRxFlowCtrl
 功能描述  : 设置WIFI接收流控标识
 输入参数  : para1、para2
 输出参数  : 无
 返 回 值  : BSP_OK/BSP_ERROR
*****************************************************************************/
unsigned int DRV_WIFI_SET_RX_FCTL(unsigned int para1, unsigned int para2)
{
    return (unsigned int)BSP_ERROR;
#if 0
#if defined (BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
    return 0;   /* 打桩 */
#endif

#if (defined BSP_CORE_APP)
#if (FEATURE_WIFI == FEATURE_ON)
    return WiFi_DrvSetRxFlowCtrl(para1, para2);
#else
    DRV_PRINTF("Function DRV_WIFI_SET_RX_FCTL unsupport!\n");
    return (unsigned long)BSP_ERROR;
#endif
#endif
#endif
}

/*********************************************************************
  Function：        USB_ETH_DrvSetDeviceAssembleParam
  Description：     TTF根据当前CPU调节TX组包个数函数接口
  Input：           ctx                扩展参数用
                    ulEthTxMinNum      TX组包个数
                    ulEthTxTimeout     TX组包超时时间
                    ulEthRxMinNum      RX组包个数
                    ulEthRxTimeout     RX组包超时时间
  Output:           NA;
  Return：          UWE_OK
**********************************************************************/
unsigned int USB_ETH_DrvSetDeviceAssembleParam(
                unsigned int ulEthTxMinNum,
                unsigned int ulEthTxTimeout,
                unsigned int ulEthRxMinNum,
                unsigned int ulEthRxTimeout)
{
#if defined (BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
    return 0;   /* 打桩 */
#endif

#if (defined BSP_CORE_APP)
    return BSP_ERROR;
#endif
}

/*********************************************************************
  Function：        USB_ETH_DrvSetHostAssembleParam
  Description：     TTF根据当前CPU调节TX组包个数函数接口
  Input：           ctx                扩展参数用
                    ulHostOutTimeout   驱动上行组包时间
                    ulEthTxMinNum      TX组包个数
                    ulEthTxTimeout     TX组包超时时间
                    ulEthRxMinNum      RX组包个数
                    ulEthRxTimeout     RX组包超时时间
  Output:           NA;
  Return：          UWE_OK
**********************************************************************/
unsigned int USB_ETH_DrvSetHostAssembleParam(unsigned int ulHostOutTimeout)
{
#if defined (BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
    return 0;   /* 打桩 */
#endif

#if (defined BSP_CORE_APP)
    return BSP_ERROR;
#endif
}

#if (defined BSP_CORE_APP)
EXPORT_SYMBOL(DRV_WIFI_SET_RX_FCTL);
EXPORT_SYMBOL(DRV_WIFI_CLR_RX_FCTL);
EXPORT_SYMBOL(DRV_WIFI_DATA_RESERVED_TAIL);
EXPORT_SYMBOL(WIFI_TEST_CMD);
EXPORT_SYMBOL(WIFI_GET_TCMD_MODE);
EXPORT_SYMBOL(WIFI_POWER_START);
EXPORT_SYMBOL(WIFI_POWER_SHUTDOWN);
EXPORT_SYMBOL(WIFI_GET_STATUS);
EXPORT_SYMBOL(WIFI_GET_RX_DETAIL_REPORT);
EXPORT_SYMBOL(WIFI_GET_RX_PACKET_REPORT);
EXPORT_SYMBOL(WIFI_GET_PA_CUR_MODE);
EXPORT_SYMBOL(WIFI_GET_PA_MODE);
EXPORT_SYMBOL(WIFI_SET_PA_MODE);
#endif



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

