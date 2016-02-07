#ifndef __IPF_TEST_H__
#define __IPF_TEST_H__

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <asm/io.h>
/*#include <asm/system.h>*/
#include <linux/semaphore.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>
#include <linux/spinlock.h>
#include <linux/string.h>
#include <linux/mm.h>
#include <linux/mman.h>
#include <linux/delay.h>
#include <bsp_mem.h>
#include "BSP.h"
#include "ipf_driver.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct tagIpfStFilterConfig_S
{
    BSP_BOOL bDataChain;
    IPF_MODE_E eIpfMode;
    BSP_U32 u32FilterNum;
    IPF_FILTER_CHAIN_TYPE_E eFilterChainType;
}IpfStFilterConfig_S;

typedef struct tagIpfStResultCheck_S
{
    /*RD属性*/
    BSP_U16 u16Attribute;
    BSP_U16 u16Len;
    BSP_U32 u32InPtr;
    BSP_U32 u32OutPtr;
    BSP_U16 u16Result;
    BSP_U16 u16UsrField1;
    BSP_U32 u32UsrField2;
    BSP_U32 u32UsrField3;
    /*RD个数*/
    BSP_U32 u32RdNum;
    /*每次处理完RD是否补充AD*/
    BSP_BOOL bAdFlag;
}IpfStResultCheck_S;

typedef struct tagStreamIPv4Header
{
    BSP_U32 TOL:16;
    BSP_U32 TOS:8;
    BSP_U32 HeaderLen:4;
    BSP_U32 Vers:4;
    BSP_U32 offset:13;
    BSP_U32 MF:1;
    BSP_U32 DF:1;
    BSP_U32 ReservFlag:1;
    BSP_U32 ID:16;
    BSP_U32 Checksum:16;
    BSP_U32 Prot:8;
    BSP_U32 TTL:8;
    BSP_U32 SourceAddr;
    BSP_U32 DestAddr;
}StreamIPv4Header;

typedef struct tagStreamUDP
{
    BSP_U32 DstPort:16;
    BSP_U32 SrcPort:16;
    BSP_U32 Checksum:16;
    BSP_U32 Length:16;
}StreamUDP;

typedef struct tagIpfChanState
{
    IPF_CHANNEL_TYPE_E chanType;
    BSP_BOOL chanFlag;
}IpfChanState;

typedef enum tagIpfStModemId_E
{
    IPF_MODEM_0 = 0,
    IPF_MODEM_1

    //IPF_MODEM_MAX
}IpfStModemId_E;

typedef struct _TTF_NODE_ST
{
    struct _TTF_NODE_ST    *pNext;
    struct _TTF_NODE_ST    *pPrev;
} TTF_NODE_ST;
enum TTF_BLK_MEM_STATE_ENUM
{
    TTF_BLK_MEM_STATE_FREE,       /*该内存未申请过或已释放*/
    TTF_BLK_MEM_STATE_ALLOC,      /*该内存已申请*/

    TTF_BLK_MEM_STATE_BUTT
};
typedef unsigned int TTF_BLK_MEM_STATE_ENUM_UINT32;

typedef struct
{
    TTF_BLK_MEM_STATE_ENUM_UINT32   enMemStateFlag;
    unsigned int                    ulAllocTick;        /* CPU tick when alloc or free */
    unsigned short                  usAllocFileId;      /* File ID when alloc or free */
    unsigned short                  usAllocLineNum;     /* File Line when alloc or free */
    unsigned short                  usTraceFileId;      /* File ID when traced */
    unsigned short                  usTraceLineNum;     /* File Line when traced */
    unsigned int                    ulTraceTick;        /* CPU tick when traced */
} TTF_BLK_MEM_DEBUG_ST;

typedef struct _TTF_MEM_ST
{
    TTF_NODE_ST                     stNode;
    unsigned char                   ucPoolId;       /*本内存属于哪一个内存池 */
    unsigned char                   ucClusterId;    /*本内存是属于哪一个级别*/
    unsigned char                   ucReserve[2];
    struct _TTF_MEM_ST             *pNext;          /* 该数据的下一段 */
    unsigned short                  usType;         /* 内存类型，DYN、BLK、EXT类型 */
    unsigned short                  usLen;          /* 申请数据的总长度，但不一定全部被使用 */
    unsigned short                  usUsed;         /* 已经使用的数据长度   */
    unsigned short                  usApp;          /* 使用者可以使用的区域 */

    TTF_BLK_MEM_DEBUG_ST           *pstDbgInfo;

    unsigned char                  *pOrigData;      /* 记录数据的原始指针 */
    unsigned char                  *pData;          /* 存放数据的指针，物理上指向结构体的连续内存 */
    void                           *pExtBuffAddr;   /* 保存extern类型的内存地址用于释放 */
    unsigned int                   ulForCds[1];    /*预留给CDS使用，GU模不用初始化*/

#if( FEATURE_LTE == FEATURE_ON )
    unsigned int                   ulForLte[6];    /*预留给LTE使用，GU模不用初始化*/
#endif
} TTF_MEM_ST;
/*****************************************************************************
  10 函数声明
*****************************************************************************/
extern BSP_S32 BSP_IPF_ST_001(BSP_VOID);
extern BSP_S32 BSP_IPF_ST_002(BSP_VOID);
extern BSP_S32 BSP_IPF_ST_003(BSP_U32 u32Count);
extern BSP_S32 BSP_IPF_ST_004(IPF_AD_TYPE_E eAdType);
extern BSP_S32 BSP_IPF_ST_101(BSP_U32 u32PacketNum);
extern BSP_S32 BSP_IPF_ST_102(BSP_VOID);
extern BSP_S32 BSP_IPF_ST_103(BSP_VOID);
extern BSP_S32 BSP_IPF_ST_104(BSP_VOID);
extern BSP_S32 BSP_IPF_ST_105(BSP_VOID);
extern BSP_S32 BSP_IPF_ST_108(BSP_U32 u32PacketNum);
extern BSP_S32 BSP_IPF_ST_109(BSP_U32 u32PacketNum);
extern BSP_S32 BSP_IPF_ST_110(BSP_U32 u32PacketNum);
extern BSP_S32 BSP_IPF_ST_301(BSP_U32 u32PacketNum);
extern BSP_S32 BSP_IPF_ST_302(BSP_VOID);
extern BSP_S32 BSP_IPF_ST_303(BSP_VOID);
extern BSP_S32 BSP_IPF_ST_304(BSP_VOID);
extern BSP_S32 BSP_IPF_ST_305(BSP_VOID);
extern BSP_S32 BSP_IPF_ST_308(BSP_U32 u32PacketNum);
extern BSP_S32 BSP_IPF_ST_309(BSP_U32 u32PacketNum);
extern BSP_S32 BSP_IPF_ST_310(BSP_U32 u32PacketNum);
extern BSP_S32 BSP_IPF_ST_401(BSP_U32 u32Count, BSP_U32 u32Tick);
extern BSP_S32 BSP_IPF_ST_401_Ex(BSP_U32 u32Count, BSP_U32 u32Tick);
extern BSP_S32 BSP_IPF_ST_402(BSP_U32 u32Count, BSP_U32 u32Tick);
extern BSP_S32 BSP_IPF_ST_402_Ex(BSP_U32 u32Count, BSP_U32 u32Tick);
extern BSP_S32 BSP_IPF_ST_501(BSP_U32 u32PacketNum);
extern BSP_S32 BSP_IPF_ST_502(BSP_U32 u32PacketNum);
extern BSP_S32 BSP_IPF_ST_503(BSP_U32 u32PacketNum);
extern BSP_S32 BSP_IPF_ST_504(BSP_U32 u32PacketNum);
extern BSP_S32 BSP_IPF_ST_505_Step1(BSP_U32 u32PacketNum);
extern BSP_S32 BSP_IPF_ST_505_Step2(BSP_U32 u32PacketNum);
extern BSP_S32 BSP_IPF_ST_506_Step1(BSP_U32 u32PacketNum);
extern BSP_S32 BSP_IPF_ST_506_Step2(BSP_U32 u32PacketNum);
extern BSP_S32 BSP_IPF_ST_601(BSP_VOID);
extern BSP_S32 BSP_IPF_ST_602(BSP_VOID);
extern BSP_S32 BSP_IPF_ST_109_603(BSP_U32 u32PacketNum);
extern BSP_S32 BSP_IPF_ST_309_604(BSP_U32 u32PacketNum);
extern BSP_S32 BSP_IPF_ST_401_605(BSP_U32 u32Count, BSP_U32 u32Tick);

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif
#endif/*__IPF_TEST_H_)*/
