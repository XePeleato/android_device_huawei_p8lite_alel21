/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : drv_sd.c
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


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 函数声明
*****************************************************************************/
#if (defined BSP_CORE_APP)
extern int sd_get_status(void);
extern int sd_get_capacity(void);
extern int sd_sg_init_table(const void *buf,unsigned int buflen);
extern int sd_multi_transfer(unsigned dev_addr, unsigned blocks, unsigned blksz, int write);
#endif

/*****************************************************************************
  3 函数实现
*****************************************************************************/

/*****************************************************************************
* 函 数 名  : DRV_SD_GET_STATUS
* 功能描述  : 卡在位查询
* 输入参数  : 无
* 输出参数  : 无
* 返 回 值  : 0 : 在位；-1: 不在位
*****************************************************************************/
int DRV_SD_GET_STATUS(void)
{
#if defined (BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
    return 0;   /* 打桩 */
#endif

#if (defined BSP_CORE_APP)
    return sd_get_status();
#endif
}

/*****************************************************************************
* 函 数 名  : DRV_SD_GET_CAPACITY
* 功能描述  : 卡容量查询
* 输入参数  : 无
* 输出参数  : 无
* 返 回 值  : 0 : 失败；>0: 卡容量
*****************************************************************************/
int DRV_SD_GET_CAPACITY(void)
{
#if defined (BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
    return 0;   /* 打桩 */
#endif

#if (defined BSP_CORE_APP)
    return sd_get_capacity();
#endif
}

/*****************************************************************************
* 函 数 名  : DRV_SD_TRANSFER
* 功能描述  : 数据传输
* 输入参数  : struct scatterlist *sg    待传输数据结构体指针
                            unsigned dev_addr   待写入的SD block 地址
                            unsigned blocks    待写入的block个数
                            unsigned blksz      每个block的大小，单位字节
                            int wrflags    读写标志位，写:WRFlAG ; 读:RDFlAG
* 输出参数  : 无
*
* 返 回 值  : 0 : 成功；其它: 失败
*****************************************************************************/
int DRV_SD_TRANSFER(struct scatterlist *sg, unsigned dev_addr,unsigned blocks, unsigned blksz, int wrflags)
{
#if defined (BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
    return 0;   /* 打桩 */
#endif

#if (defined BSP_CORE_APP)
    return -1;
    /* return sd_transfer(sg, dev_addr, blocks, blksz, wrflags); */
#endif
}

/*****************************************************************************
* 函 数 名  : DRV_SD_SG_INIT_TABLE
*
* 功能描述  : SD多块数据传输sg list初始化
*
* 输入参数  :  const void *buf        待操作的buffer地址
                             unsigned int buflen    待操作的buffer大小，小于32K，大小为512B的整数倍；
                                                            大于32K，大小为32KB的整数倍，最大buffer为128K。

* 输出参数  : NA
*
* 返 回 值  : 0 : 成功；其它: 失败
*
* 其它说明  : NA
*
*****************************************************************************/
int DRV_SD_SG_INIT_TABLE(const void *buf,unsigned int buflen)
{
#if defined (BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
    return 0;   /* 打桩 */
#endif

#if (defined BSP_CORE_APP)
    return sd_sg_init_table(buf, buflen);
#endif
}

/*****************************************************************************
* 函 数 名     : DRV_SD_MULTI_TRANSFER
*
* 功能描述  : SD多块数据传输。
*
* 输入参数  :  unsigned dev_addr   待写入的SD block 地址
               unsigned blocks     待写入的block个数
               unsigned blksz      每个block的大小，单位字节
               int write           读写标志位，写:1 ; 读:0
* 输出参数  : NA
*
* 返 回 值     : 0 : 成功；其它: 失败
*
* 其它说明  : NA
*
*****************************************************************************/
int DRV_SD_MULTI_TRANSFER(unsigned dev_addr, unsigned blocks, unsigned blksz, int write)
{
#if defined (BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
    return 0;   /* 打桩 */
#endif

#if (defined BSP_CORE_APP)
    return sd_multi_transfer(dev_addr, blocks, blksz, write);
#endif
}

unsigned int BSP_SDMMC_UsbGetStatus(void)
{
    return 0;
}

void sdmmc_ClearWholeScreen(void)
{
    return ;
}

#if 0
/*****************************************************************************
 函 数 名  : BSP_SDMMC_ATProcess
 功能描述  : at^sd,SD卡操作，写，擦除，格式化操作
 输入参数  : 操作类型 ulOp:
            0  格式化SD卡
            1  擦除整个SD卡内容；
            2  用于指定地址内容的擦除操作，指定擦除的内容长度为512字节。擦除后的地址中写全1
            3  写数据到SD卡的指定地址中，需要带第二个和第三个参数
            4  读取dev_addr指定的地址(dev_addr*512)的512个字节的内容到pucBuffer中

            dev_addr < address >  地址，以512BYTE为一个单位，用数字n表示

            ulData
             < data >            数据内容，表示512BYTE的内容，每个字节的内容均相同。
             0       字节内容为0x00
             1       字节内容为0x55
             2       字节内容为0xAA
             3       字节内容为0xFF

 输出参数  : pulErr
 返 回 值  : 0 ：OK  非 0 ：Error

            具体的错误值填充在*pulErr中
            0 表示SD卡不在位
            1 表示SD卡初始化失败
            2 表示<opr>参数非法，对应操作不支持(该错误由AT使用,不需要底软使用)
            3 表示<address>地址非法，超过SD卡本身容量
            4 其他未知错误
*****************************************************************************/
unsigned int  BSP_SDMMC_ATProcess(SD_MMC_OPRT_ENUM_UINT32 ulOp,
    unsigned int dev_addr,  unsigned int ulData,unsigned char *pucBuffer,unsigned int *pulErr)
{
#if defined (BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
    return 0;   /* 打桩 */
#endif

#if (defined BSP_CORE_APP)
    if (BSP_NULL != pulErr) {
        *pulErr = 2;
    }
    return BSP_ERROR;
#endif
}
#endif

/*****************************************************************************
 函 数 名  : BSP_SDMMC_GetOprtStatus
 功能描述  : at^sd,SD卡当前操作状态
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
            0: 未操作或操作已经完成;
            1: 操作中
            2: 上次操作失败
*****************************************************************************/
unsigned int BSP_SDMMC_GetOprtStatus(void)
{
#if defined (BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
    return 0;   /* 打桩 */
#endif

#if (defined BSP_CORE_APP)
    return 0;
#endif
}

unsigned int mdrv_sd_get_opptstatus(void)
{
    return 0;
}

unsigned int mdrv_sd_at_process(unsigned int ulOp, unsigned int ulAddr,
            unsigned int ulData, unsigned char *pucBuffer, unsigned int *pulErr)
{
    if (NULL != pulErr) {
        *pulErr = 2;
    }
    return -1;
}

/*****************************************************************************
* 函 数 名  : mdrv_sd_get_status
* 功能描述  : 卡在位查询
* 输入参数  : 无
* 输出参数  : 无
* 返 回 值  : 0 : 在位；-1: 不在位
*****************************************************************************/
int mdrv_sd_get_status(void)
{
    return DRV_SD_GET_STATUS();
}

int mdrv_sd_get_capacity(void)
{
    return DRV_SD_GET_CAPACITY();
}
int mdrv_sd_sg_init_table(const void *buf,unsigned int buflen)
{
    return DRV_SD_SG_INIT_TABLE(buf, buflen);
}

int mdrv_sd_multi_transfer(unsigned int dev_addr, unsigned int blocks,
                               unsigned int blksz,int write)
{
    return DRV_SD_MULTI_TRANSFER(dev_addr, blocks, blksz,write);
}



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

