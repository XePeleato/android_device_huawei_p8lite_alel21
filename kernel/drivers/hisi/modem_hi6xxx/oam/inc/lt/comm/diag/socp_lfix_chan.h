

#include <product_config.h>
#include "vos.h"
#if (FEATURE_SAMPLE_LTE_CHAN == FEATURE_ON)
#include "SOCPInterface.h"
#include "BSP_SOCP_DRV.h"
#include "soc_baseaddr_interface.h"
#if((VOS_OS_VER == VOS_VXWORKS) || (VOS_OS_VER == VOS_RTOSCK))
#include "TLPhyInterface.h"
#endif


#ifndef __SOCP_LFIXCH_H__
#define __SOCP_LFIXCH_H__

/*****************************************************************************
  2 宏定义
*****************************************************************************/

#define SOCP_DSPLOG_CHN             (SOCP_CODER_SRC_LDSP1)         // 14
#define SOCP_BBPLOG_CHN             (SOCP_DSPLOG_CHN + 2)         // 16开始，共8个
#define SOCP_BBPDS_CHN              (SOCP_BBPLOG_CHN + 8)  // 24
#define SOCP_FIXCHN_BASE            (SOCP_DSPLOG_CHN)
#define SOCP_FIXCHN_END             (SOCP_BBPDS_CHN)

#define DDR_SOCP_ADDR               (GLOBAL_MEM_SOCP_ADDR)
#define DDR_SOCP_SIZE               (GLOBAL_MEM_SOCP_SIZE)

#define SOCP_FIXEDID_BASE           (SOCP_DSPLOG_CHN - SOCP_FIXCHN_BASE)

#define SOCP_DSPLOG_DST_BUFID       ((VOS_INT32) 1)
#define SOCP_BBPLOG_DST_BUFID       ((VOS_INT32) 1)
#define SOCP_BBPDS_DST_BUFID        ((VOS_INT32) 1)

#define BBP_REGISTER_SIZE               (0x500)
#define SOCP_DSPLOG_CHN_SIZE            (0x2000)
#define SOCP_BBPLOG_CHN_SIZE            (0x2000)

#define BBP_MAX_CHN (0x09)
#define BBP_MAX_LOG_CHN (0x08)
#define DSP_MAX_CHN (0x02)
#define RSV_MAX_CHN (0x08)
#define SOCP_FIXED_MAX_CHAN     (BBP_MAX_CHN+DSP_MAX_CHN+RSV_MAX_CHN)

//#define SOCP_REG_BASE                   0x500A9000
#define BBP_REG_ARM_BASEADDR            0xFD200000
#define TENSILICA_CORE0_DRAM_ADDR       LPHY_CORE0_BINARY_LOAD_ADDR /*0xE0FC0000*/


#if((VOS_OS_VER == VOS_VXWORKS) || (VOS_OS_VER == VOS_RTOSCK))
#define BBP_REG_WRITE(reg, data)                COMM_DRV_REG_WRITE(BBP_REG_ARM_BASEADDR,     reg, data)
#define BBP_REG_READ(reg, result)               COMM_DRV_REG_READ(BBP_REG_ARM_BASEADDR,      reg, result)
#define BBP_REG_SETBITS(reg, pos, bits, val)    BSP_REG_SETBITS(BBP_REG_ARM_BASEADDR,   reg, pos, bits, val)
#define BBP_REG_GETBITS(reg, pos, bits)         BSP_REG_GETBITS(BBP_REG_ARM_BASEADDR,   reg, pos, bits)
#elif(VOS_OS_VER == VOS_LINUX)
#define BBP_REG_WRITE(reg, data)                COMM_DRV_REG_WRITE(g_armBaseAddr,    reg, data)
#define BBP_REG_READ(reg, result)               COMM_DRV_REG_READ(g_armBaseAddr,     reg, result)
#define BBP_REG_SETBITS(reg, pos, bits, val)    BSP_REG_SETBITS(g_armBaseAddr,  reg, pos, bits, val)
#define BBP_REG_GETBITS(reg, pos, bits)         BSP_REG_GETBITS(g_armBaseAddr,  reg, pos, bits)
#endif


/*
* 通道打开宏定义，适用于编码源通道和解码目的通道
*/
#ifndef SOCP_CHN_ALLOCATED
#define SOCP_CHN_ALLOCATED              ((VOS_INT32) 1)    /*通道已经分配*/
#define SOCP_CHN_UNALLOCATED            ((VOS_INT32) 0)    /*通道没有分配*/
#endif


/*
* 通道使能宏定义，同时适用于编码源通道和解码源通道
*/
#ifndef SOCP_CHN_ENABLE
#define SOCP_CHN_ENABLE                 ( (VOS_INT32) 1)   /*通道打开*/
#define SOCP_CHN_DISABLE                ( (VOS_INT32) 0)   /*通道关闭*/
#endif


#define SOCP_CHECK_ENCSRC_CHAN_ID(id) \
    do {\
        if ((id >= SOCP_CODER_SRC_BUTT))\
        {\
            vos_printf("the Chan Id is invalid!\n"); \
            return BSP_ERR_SOCP_INVALID_CHAN; \
        } \
    } while (0)
#define SOCP_CHECK_ENCSRC_ALLOC(id) \
    do {\
        if (SOCP_CHN_ALLOCATED != g_stEncSrcFixChan[id].u32AllocStat)\
        {\
            vos_printf("encoder src[%d] not allocated!\n", id); \
            return BSP_ERR_SOCP_INVALID_CHAN; \
        } \
    } while (0)



typedef struct tagSOCP_ENCSRC_FIXCHAN_S
{
    VOS_UINT                 u32ChanID;
    VOS_UINT                 u32ChanEn;
    VOS_UINT                 u32DestChanID;
    VOS_UINT                 u32BypassEn;
    VOS_UINT                 u32AllocStat;  /* 通道已经或没有分配的标识*/
    VOS_UINT                 u32LastRdSize;
    VOS_UINT                 u32RdThreshold;
    SOCP_ENCSRC_CHNMODE_ENUM_UIN32   eChnMode;      /* 数据结构类型 */
    SOCP_CHAN_PRIORITY_ENUM_UIN32    ePriority;
    SOCP_DATA_TYPE_ENUM_UIN32        eDataType;
    SOCP_DATA_TYPE_EN_ENUM_UIN32     eDataTypeEn;
    SOCP_ENC_DEBUG_EN_ENUM_UIN32     eDebugEn;
    SOCP_RING_BUF_STRU      sEncSrcBuf;
    SOCP_RING_BUF_STRU      sRdBuf;
    socp_event_cb           event_cb;
    socp_rd_cb              rd_cb;
}SOCP_ENCSRC_FIXCHAN_S;



VOS_UINT  DRV_SOCP_INIT_LTE_DSP(VOS_UINT ulChanId,VOS_UINT ulPhyAddr,VOS_UINT ulSize);

VOS_UINT  DRV_SOCP_INIT_LTE_BBP_LOG(VOS_UINT ulChanId,VOS_UINT ulPhyAddr,VOS_UINT ulSize);

VOS_UINT  DRV_SOCP_INIT_LTE_BBP_DS(VOS_UINT ulChanId,VOS_UINT ulPhyAddr,VOS_UINT ulSize);

VOS_VOID DRV_SOCP_ENABLE_LTE_BBP_DSP(VOS_UINT ulChanId);

VOS_VOID  DRV_SOCP_DSPCHN_START(VOS_VOID);
VOS_VOID  DRV_SOCP_DSPCHN_STOP(VOS_VOID);

/*****************************************************************************
 函 数 名  : DRV_SOCP_STOP
 功能描述  : 此接口用于源通道，停止编码或者解码。
 输入参数  : u32SrcChanID:源通道ID。
 输出参数  : 无。
 返 回 值  : SOCP_OK:编码或解码停止成功。
             SOCP_ERROR:编码或解码停止失败。
*****************************************************************************/
#define DRV_SOCP_STOP(u32SrcChanID)   BSP_SOCP_Stop(u32SrcChanID)

/*****************************************************************************
 函 数 名  : DRV_SOCP_START
 功能描述  : 此接口用于源通道，启动编码或者解码。
 输入参数  : u32SrcChanID:源通道ID。
 输出参数  : 无。
 返 回 值  : SOCP_OK:编码或解码启动成功。
             SOCP_ERROR:编码或解码启动失败。
*****************************************************************************/
#define DRV_SOCP_START(u32SrcChanID)   BSP_SOCP_Start(u32SrcChanID)



#endif /*__SOCP_LFIXCH_H__*/

#endif

