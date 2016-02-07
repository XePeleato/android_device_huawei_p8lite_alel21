/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : InterpeakMntn.h
  版 本 号   : 初稿
  作    者   : caikai
  生成日期   : 2013年7月30日
  最近修改   :
  功能描述   : InterpeakMntn.c 的头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2013年7月30日
    作    者   : caikai
    修改内容   : 创建文件

******************************************************************************/

#ifndef __INTERPEAKMNTN_H__
#define __INTERPEAKMNTN_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "InterpeakMntnInterface.h"
#if (defined(__UT_CENTER__))
#include  "VxWorksStub.h"
#else
#include "ipnet_routesock.h"
#endif



/*****************************************************************************
  2 宏定义
*****************************************************************************/
#define     IPS_MNTN_CCORE_MAX_LOOP_CNT     (100000)

/*****************************************************************************
  3 枚举定义
*****************************************************************************/


/*****************************************************************************
  4 全局变量声明
*****************************************************************************/
extern IPS_MNTN_CCORE_CAPTURE_CB   g_astCallbackTbl[IPS_MNTN_CCORE_CB_TYPE_BUTT];

/*****************************************************************************
  5 消息头定义
*****************************************************************************/


/*****************************************************************************
  6 消息定义
*****************************************************************************/


/*****************************************************************************
  7 STRUCT定义
*****************************************************************************/

typedef struct
{
    unsigned char               ucInnerStates;
    unsigned char               ucOuterStates;
    unsigned char               ucProtocol;         /* 协议类型 */
    unsigned char               ucRsv;
    unsigned long int           ulTimeToAlive;      /* 该NAT Mapping还能生效的时间 */
    unsigned long int           ulSrcAddr;          /* 源地址 */
    unsigned long int           ulDstAddr;          /* 目的地址 */
    unsigned long int           ulNatTranSrcAddr;   /* 经过转换后的源地址 */
    unsigned short int          usSrcPort;          /* 源端口 */
    unsigned short int          usDstPort;          /* 目的端口 */
    unsigned short int          usNatTransSrcPort;  /* 经过转换后的源端口 */
    unsigned char               aucRsv[2];
    unsigned long int           ulSeqStart;         /* Initial sequence number */
    unsigned long int           ulCurrDelta;        /* Current delta sequence number */
    unsigned long int           ulPrevDelta;        /* Previous delta sequence number */
}IPS_NAT_MAPPING_TRACE_STRU;

/*****************************************************************************
  8 UNION定义
*****************************************************************************/


/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/


/*****************************************************************************
  10 函数声明
*****************************************************************************/
extern IP_GLOBAL void
/*lint -esym(752,ipnet_cmd_init_addrs)*/
ipnet_cmd_init_addrs(void *msg, Ip_u32 addrs, struct Ip_sockaddr **sas);


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of InterpeakMntn.h */



