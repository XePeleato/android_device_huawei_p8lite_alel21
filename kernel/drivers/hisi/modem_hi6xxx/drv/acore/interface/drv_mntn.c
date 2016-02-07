/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : drv_mntn.c
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
#include "BSP.h"
#include <excDrv.h>


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 函数声明
*****************************************************************************/

/*****************************************************************************
  3 函数实现
*****************************************************************************/

/*******************************************************************************
 函 数 名: DRV_EXCH_CUST_FUNC_REG
 功能描述: 临终遗言模块提供对外接口，供上层注册回调，用于复位时保存上层指定信息
 输入参数: cb 上层指定回调接口
 输出参数: 无
 返 回 值: 0-添加成功，其他-添加失败
 说明    :  pucData :指定记录信息地址，请注意不要指定栈等临时数据区，堆空间数据，
                     使用者自己维护，包括空间使用完，自己释放。建议全局变量。
            aucName :名称
            ulDataLen: 数据长度

规格约束:
            缓冲区大小:64K。共分8个通道，每个通道均值最大不得超过8k.
            注册函数返回值为:
                正常:    ulDataLen data数据长度；
                异常:    0

返回值列表:
    CUST_REG_OK = 0,                        添加成功
    CUST_REG_PARM_NULL,                     入参为空
    CUST_REG_EXCH_NOT_READY,                EXCH模块尚未初始化
    CUST_REG_EXCH_REBOOT_CONTEXT,           已经进入复位流程
    CUST_REG_EXCH_QUEUE_NOT_INIT,           注册队列尚未初始化
    CUST_REG_EXCH_QUEUE_FULL,               队列已满
    CUST_REG_EXCH_ALREADY_IN_QUEUE,         重复注册
    CUST_REG_EXCH_MUTEX_REJECT,             MUTEX REJECT
    CUST_REG_EXCH_QUEUE_IN_ERR              入队列失败

请通过返回值定位解决错误

*******************************************************************************/
extern int echc_cust_funt_func_reg(exch_CBReg cb);
int DRV_EXCH_CUST_FUNC_REG(exchCBReg cb)
{
    return echc_cust_funt_func_reg((exch_CBReg)cb);
}
/*****************************************************************************
 函 数 名  : DRV_MEM_READ
 功能描述  : 按32位宽度查询内存，输出4字节数据。可维可测，V7先打桩
 输入参数  : ulAddress：查询地址空间地址，地址需在内存范围内，否则返回-1。
 输出参数  : pulData：指向查询内容的指针。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
*****************************************************************************/
int DRV_MEM_READ (unsigned int ulAddress, unsigned int* pulData)
{
#if defined (BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
    return BSP_MEM_READ(ulAddress, pulData);
#endif

#if (defined BSP_CORE_APP)
    return 0;   /* 打桩 */
#endif
}

/*****************************************************************************
 函 数 名  : mdrv_sysboot_restart
 功能描述  : 复位Modem子系统
 输入参数  : 无
 输出参数  :
 返 回 值  : 无
*****************************************************************************/
void mdrv_sysboot_restart(void)
{
    return ;
}

/*****************************************************************************
 函 数 名  : mdrv_sysboot_shutdown
 功能描述  : 单板关机
 输入参数  : 无
 输出参数  :
 返 回 值  : 无
*****************************************************************************/
void mdrv_sysboot_shutdown(void)
{
    return;
}

/*****************************************************************************
* 函 数 名  : DRV_MSP_PROC_REG
*
* 功能描述  : DRV提供给OM的注册函数
*
* 输入参数  : MSP_PROC_ID_E eFuncID, BSP_MspProc pFunc
* 输出参数  : NA
*
* 返 回 值  : NA
*
* 其它说明  : 可维可测接口函数
*****************************************************************************/
void DRV_MSP_PROC_REG(MSP_PROC_ID_E eFuncID, BSP_MspProc pFunc)
{
    return;
}


unsigned int mdrv_crypto_check_sec(void)
{
    return 0;
}

/*************************************************
 函 数 名       : mdrv_crypto_secboot_supported
 功能描述   : 当前版本是否支持安全启动
 输入参数   : unsigned char *pData
 输出参数   : unsigned char *pData
 返 回 值      : OK/ERROR
*************************************************/
int mdrv_crypto_secboot_supported(unsigned char *pu8Data)
{
    return 0;
}

/*************************************************
 函 数 名       : mdrv_crypto_sec_started
 功能描述   : 查询当前版本是否已经启用安全启动
 输入参数   : unsigned char *pData
 输出参数   : unsigned char *pData
 返 回 值      : OK/ERROR
*************************************************/
int mdrv_crypto_sec_started(unsigned char *pu8Data)
{
    return 0;   /* 打桩 */
}

/*************************************************
 函 数 名       : mdrv_crypto_start_secure
 功能描述   : 启用安全启动
 输入参数   :
 输出参数   :
 返 回 值      : OK/ERROR
*************************************************/
int mdrv_crypto_start_secure(void)
{
    return 0;   /* 打桩 */
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

