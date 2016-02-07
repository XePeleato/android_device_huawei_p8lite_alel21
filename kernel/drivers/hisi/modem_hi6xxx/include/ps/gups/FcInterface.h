/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : FcInterface.h
  版 本 号   : 初稿
  作    者   : g45205
  生成日期   : 2011年12月2日
  最近修改   :
  功能描述   : FcInterface.h 的头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2011年12月2日
    作    者   : g45205
    修改内容   : 创建文件

******************************************************************************/

#ifndef __FCINTERFACE_H__
#define __FCINTERFACE_H__


/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include  "vos.h"
#include  "PsTypeDef.h"
#include  "TtfNvInterface.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#pragma pack(4)

/*****************************************************************************
  2 宏定义
*****************************************************************************/
/* 流控策略掩码，对应位设置，表示某种策略 */
#define FC_POLICY_MASK_MEM              ( ((VOS_UINT32)1) << FC_POLICY_ID_MEM )
#define FC_POLICY_MASK_CPU_A            ( ((VOS_UINT32)1) << FC_POLICY_ID_CPU_A )
#define FC_POLICY_MASK_CDS              ( ((VOS_UINT32)1) << FC_POLICY_ID_CDS )
#define FC_POLICY_MASK_CST              ( ((VOS_UINT32)1) << FC_POLICY_ID_CST )
#define FC_POLICY_MASK_GPRS             ( ((VOS_UINT32)1) << FC_POLICY_ID_GPRS )
#define FC_POLICY_MASK_TMP              ( ((VOS_UINT32)1) << FC_POLICY_ID_TMP )
#define FC_POLICY_MASK_CPU_C            ( ((VOS_UINT32)1) << FC_POLICY_ID_CPU_C)


/*****************************************************************************
  3 枚举定义
*****************************************************************************/
/*====================================*/ /* 流控策略ID */
enum FC_POLICY_ID_ENUM
{
    FC_POLICY_ID_MEM                    = 0,
    FC_POLICY_ID_CPU_A,
    FC_POLICY_ID_CDS,
    FC_POLICY_ID_CST,
    FC_POLICY_ID_GPRS,
    FC_POLICY_ID_TMP,
    FC_POLICY_ID_CPU_C,
    FC_POLICY_ID_BUTT
};
typedef VOS_UINT8 FC_POLICY_ID_ENUM_UINT8;


/*====================================*/ /* 流控优先级定义 */
/* 流控优先级定义，数值越大，优先级越高，越晚流控 */
enum FC_PRI_ENUM
{
    FC_PRI_NULL                             = 0,                                /* 流控内部处理使用，外部不使用 */
    FC_PRI_1,
    FC_PRI_2,
    FC_PRI_3,
    FC_PRI_4,
    FC_PRI_5,
    FC_PRI_6,
    FC_PRI_7,
    FC_PRI_8,
    FC_PRI_9,
    FC_PRI_BUTT
};
typedef VOS_UINT8  FC_PRI_ENUM_UINT8;


/*====================================*/ /* 流控点定义 */
enum FC_ID_ENUM
{
    FC_ID_BRIDGE_FORWARD_DISACRD    = 0,/* 网桥，E5使用*/
    FC_ID_USB_ETH,                      /* USB网卡，E5使用*/
    FC_ID_WIFI_ETH,                     /* WIFI网卡，E5使用*/
    FC_ID_NIC_1,                        /* NDIS网卡1，以下相同 */
    FC_ID_NIC_2,
    FC_ID_NIC_3,
    FC_ID_NIC_4,
    FC_ID_NIC_5,
    FC_ID_NIC_6,
    FC_ID_NIC_7,
    FC_ID_NIC_8,
    FC_ID_NIC_9,
    FC_ID_NIC_10,
    FC_ID_NIC_11,
    FC_ID_MODEM,                        /* Modem */
    FC_ID_DIPC_1,                       /* DIPC使用通道，对应设备为UDI_ACM_HSIC_ACM1_ID */
    FC_ID_DIPC_2,                       /* DIPC使用通道，对应设备为UDI_ACM_HSIC_ACM3_ID */
    FC_ID_DIPC_3,                       /* DIPC使用通道，对应设备为UDI_ACM_HSIC_ACM5_ID */
    FC_ID_UL_RATE_1_FOR_CPU,            /* C核CPU流控使用的上行速率控制，以下同 */
    FC_ID_UL_RATE_2_FOR_CPU,
    FC_ID_UL_RATE_3_FOR_CPU,
    FC_ID_UL_RATE_4_FOR_CPU,
    FC_ID_UL_RATE_5_FOR_CPU,
    FC_ID_UL_RATE_6_FOR_CPU,
    FC_ID_UL_RATE_7_FOR_CPU,
    FC_ID_UL_RATE_8_FOR_CPU,
    FC_ID_UL_RATE_9_FOR_CPU,
    FC_ID_UL_RATE_10_FOR_CPU,
    FC_ID_UL_RATE_11_FOR_CPU,
    FC_ID_UL_RATE_1_FOR_TMP,            /* C核温保流控使用的上行速率控制，以下同 */
    FC_ID_UL_RATE_2_FOR_TMP,
    FC_ID_UL_RATE_3_FOR_TMP,
    FC_ID_UL_RATE_4_FOR_TMP,
    FC_ID_UL_RATE_5_FOR_TMP,
    FC_ID_UL_RATE_6_FOR_TMP,
    FC_ID_UL_RATE_7_FOR_TMP,
    FC_ID_UL_RATE_8_FOR_TMP,
    FC_ID_UL_RATE_9_FOR_TMP,
    FC_ID_UL_RATE_10_FOR_TMP,
    FC_ID_UL_RATE_11_FOR_TMP,
    FC_ID_BUTT
};
typedef VOS_UINT8  FC_ID_ENUM_UINT8;


/*====================================*/ /* 内存流控使用的优先级 */
#define FC_PRI_FOR_MEM_LEV_1                    (FC_PRI_1)
#define FC_PRI_FOR_MEM_LEV_2                    (FC_PRI_2)
#define FC_PRI_FOR_MEM_LEV_3                    (FC_PRI_3)
#define FC_PRI_FOR_MEM_LEV_4                    (FC_PRI_4)

/* 网桥流控点优先级 */
#define FC_PRI_FOR_BRIDGE_FORWARD_DISCARD       (FC_PRI_FOR_MEM_LEV_1)

/* LTE PDN/GU QoS使用的优先级 */
#define FC_PRI_FOR_PDN_LOWEST                   (FC_PRI_FOR_MEM_LEV_2)
#define FC_PRI_FOR_PDN_NONGBR                   (FC_PRI_FOR_MEM_LEV_3)
#define FC_PRI_FOR_PDN_GBR                      (FC_PRI_FOR_MEM_LEV_4)


/*****************************************************************************
  4 全局变量声明
*****************************************************************************/


/*****************************************************************************
  5 消息头定义
*****************************************************************************/


/*****************************************************************************
  6 消息定义
*****************************************************************************/
/* 流控和解除流控函数 */
typedef VOS_UINT32 (*FC_SET_FUNC)(VOS_UINT32 ulParam1, VOS_UINT32 ulParam2);
typedef VOS_UINT32 (*FC_CLR_FUNC)(VOS_UINT32 ulParam1, VOS_UINT32 ulParam2);
typedef VOS_UINT32 (*FC_RST_FUNC)(VOS_UINT32 ulParam1, VOS_UINT32 ulParam2);


typedef struct
{
    MODEM_ID_ENUM_UINT16                enModemId;  /* _H2ASN_Replace VOS_UINT16  enModemId; */
    FC_POLICY_ID_ENUM_UINT8             enPolicyId; /* 该流控点影响到的流控策略 */
    FC_ID_ENUM_UINT8                    enFcId;
    FC_PRI_ENUM_UINT8                   enFcPri;    /* 该流控点在该流控策略里的优先级 */
    VOS_UINT8                           aucRsv[3];
    VOS_UINT32                          ulParam1;   /* 保留给流控点使用，调用流控和解除流控函数时，作为函数入参 */
    VOS_UINT32                          ulParam2;   /* 保留给流控点使用，调用流控和解除流控函数时，作为函数入参 */
    FC_SET_FUNC                         pSetFunc;   /* 流控函数 */  /* _H2ASN_Replace VOS_UINT32  pSetFunc; */
    FC_CLR_FUNC                         pClrFunc;   /* 解流控函数 */ /* _H2ASN_Replace VOS_UINT32  pClrFunc; */
    FC_RST_FUNC                         pRstFunc;   /* 新增的复位处理与恢复接口 */ /* _H2ASN_Replace VOS_UINT32  pRstFunc; */
} FC_REG_POINT_STRU;



/*****************************************************************************
  7 STRUCT定义
*****************************************************************************/


/*****************************************************************************
  8 UNION定义
*****************************************************************************/


/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/


/*****************************************************************************
  10 函数声明
*****************************************************************************/
extern VOS_UINT32  FC_RegPoint
(
    FC_REG_POINT_STRU                  *pstFcRegPoint
);

extern VOS_UINT32  FC_DeRegPoint
(
    FC_ID_ENUM_UINT8                   enFcId,
    MODEM_ID_ENUM_UINT16                enModemId
);

extern VOS_UINT32  FC_ChangePoint
(
    FC_ID_ENUM_UINT8                   enFcId,
    FC_POLICY_ID_ENUM_UINT8             enPolicyId,
    FC_PRI_ENUM_UINT8                  enPri,
    MODEM_ID_ENUM_UINT16                enModemId
);

extern VOS_VOID FC_ChannelMapCreate (
    FC_ID_ENUM_UINT8                   enFcId,
    VOS_UINT8                           ucRabId,
    MODEM_ID_ENUM_UINT16                enModemId
);

extern VOS_VOID FC_ChannelMapDelete (
    VOS_UINT8                           ucRabId,
    MODEM_ID_ENUM_UINT16                enModemId
);


typedef VOS_UINT32 (*FC_ACORE_DRV_ASSEMBLE_PARA_FUNC)(FC_DRV_ASSEM_PARA_STRU *pstFcDrvAssemPara);

extern VOS_UINT32 FC_ACORE_RegDrvAssemFunc
(
    FC_ACORE_DRV_ASSEMBLE_PARA_FUNC pFcDrvSetAssemParaFuncUe,
    FC_ACORE_DRV_ASSEMBLE_PARA_FUNC pFcDrvSetAssemParaFuncPc
);



#if (VOS_OS_VER == VOS_WIN32)
#pragma pack()
#else
#pragma pack(0)
#endif


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of FcInterface.h */

