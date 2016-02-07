/*******************************************************************************
  Copyright   : 2005-2010, Huawei Tech. Co., Ltd.
  File name   : NetWInterface.h
  Version     : V300R002
  Date        : 2011-11-23
  Description :
  History     :
  1. Date:2011-11-23
     Author: caikai
     Modification:Create
*******************************************************************************/

#ifndef _NETWINTERFACE_H_
#define _NETWINTERFACE_H_

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#if (VOS_OS_VER == VOS_WIN32) || defined (__PC_LINT__)
#include "linuxstub.h"
#else
#include <linux/types.h>
#endif


/*****************************************************************************
  2 宏定义
*****************************************************************************/
/*================================================*/
/* 数值宏定义 */
/*================================================*/
#define NW_INQUIRE_IFCONFIG_ON_MASK      (1)        /*查询Ifconfig掩码*/
#define NW_INQUIRE_ARP_ON_MASK           (1 << 1)   /*查询Arp掩码*/
#define NW_INQUIRE_ROUTE_ON_MASK         (1 << 2)   /*查询Route掩码*/
#define NW_INQUIRE_NETSTATE_ON_MASK      (1 << 3)   /*查询Netstate掩码*/
#define NW_INQUIRE_NAT_ON_MASK           (1 << 4)   /*查询Nat掩码*/

#define NW_INQUIRE_CFG_ALL_MASK          ( NW_INQUIRE_IFCONFIG_ON_MASK \
                                            | NW_INQUIRE_ARP_ON_MASK \
                                            | NW_INQUIRE_ROUTE_ON_MASK \
                                            | NW_INQUIRE_NETSTATE_ON_MASK \
                                            | NW_INQUIRE_NAT_ON_MASK)

#define DEVICE_NAME                      "NetfilterExtDev"     /*字符设备名*/
#define CLASS_NAME                       "NetfilterExtClass"
#define DEV_BUFF_LEN                     (sizeof(NETWORK_INQUIRE_CONFIG_STRU))
#define DEVICE_PATH_LEN                  (100)                 /*设备所在路径的长度*/
#define DEVICE_NUM                       (1)                   /*注册字符设备的数目*/
#define MINOR_NO_START                   (500)                 /*次设备的起始号*/


/*******************************************************************************
  3 枚举定义
*******************************************************************************/

/*****************************************************************************
  4 结构定义
*****************************************************************************/
/*********************************************
 结构体名   :NETWORK_INQUIRE_CONFIG_STRU
 协议表格 :
 结构体说明 :network inquire配置信息
*********************************************/
typedef struct
{
    unsigned int ulOnOffMask;                  /* 功能开关掩码 */
    unsigned int ulTimerLen;                   /* 上报周期定时器长度 */
}NETWORK_INQUIRE_CONFIG_STRU;


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif

