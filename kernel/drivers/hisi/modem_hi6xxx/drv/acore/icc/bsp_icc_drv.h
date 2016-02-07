/*************************************************************************
*   版权所有(C) 1987-2011, 深圳华为技术有限公司.
*
*   文 件 名 :  BSP_ICC_DRV.h
*
*   作    者 :  z67193
*
*   描    述 :  ICC模块驱动头文件
*
*   修改记录 :  2010年01月08日  v1.00  z67193  创建
*
*************************************************************************/

#ifndef _BSP_ICC_DRV_H
#define _BSP_ICC_DRV_H

#ifdef __cplusplus
extern "C"
{
#endif


/**************************************************************************
  头文件包含
**************************************************************************/

#include <linux/spinlock.h>
#include <asm/cacheflush.h>
#include "MemoryLayout.h"
#include "BSP.h"


/**************************************************************************
  宏定义
**************************************************************************/
#define MEM_ICC_CTRL_CHAN_ADDR  (HISI_VA_ADDRESS(MEMORY_AXI_ICC_ADDR))
#define MEM_ICC_CTRL_CHAN_SIZE  (MEMORY_AXI_ICC_SIZE)

#define ICC_CACHEABLE

#define ICC_PACKET_HEADER       0x71abcdef

#define  ICC_DEBUG_printk printk

#if(FEATURE_ICC_DEBUG == FEATURE_ON)
#define ICC_PACKET_HEADER_LEN   12
#define  ICC_DEBUG_PRINTK printk
#else
#define ICC_PACKET_HEADER_LEN   8
#define  ICC_DEBUG_PRINTK
#endif

#ifdef BSP_CORE_MODEM
#define ICC_HOST            0
#define ICC_TAGET           1
#else
#define ICC_HOST            1
#define ICC_TAGET           0
#endif

// 添加列表节点方式
#define ICC_LIST_ADD_PRIORITY	0
#define ICC_LIST_ADD_HEAD		1
#define ICC_LIST_ADD_TAIL		2

// 通道信号定义
#define ICC_CHAN_SIGNAL_OPEN	    (0x1 << 0)
#define ICC_CHAN_SIGNAL_CLOSE	    (0x1 << 1)
#define ICC_CHAN_SIGNAL_RESET	    (0x1 << 2)
#define ICC_CHAN_SIGNAL_RECOVER     (0x1 << 3)
#define ICC_CHAN_SIGNAL_DATA        (0x1 << 4)
#define ICC_CHAN_SIGNAL_ENOUGH      (0x1 << 5)


/**************************************************************************
  枚举定义
**************************************************************************/
typedef enum tagICC_CMD_E
{
	ICC_CMD_OPEN       = 0,
	ICC_CMD_CLOSE,
	ICC_CMD_WRITE,
	ICC_CMD_READ,
	ICC_CMD_IOCTRL,
	ICC_CMD_BUTT
}ICC_CMD_E;


/**************************************************************************
  STRUCT定义
**************************************************************************/
typedef struct tagICC_PACKET_S
{
	BSP_U32             data;
	BSP_S32             size;
}ICC_PACKET_S;

#if(FEATURE_ICC_DEBUG == FEATURE_ON)
typedef struct tagICC_DEBUG_S
{
	BSP_U32			u32SendTimes;
	BSP_U32			u32SendBytes;
	BSP_U32			u32RecvTimes;
	BSP_U32			u32RecvBytes;
	BSP_U32			u32RecvInt;
	BSP_U32			u32LastHaveBytes;
	BSP_U32			u32LastReadBytes;
    BSP_U32             u32NoticeEnough;
    BSP_U32             u32RecvEnough;
	BSP_U32			u32IdleSize;
}ICC_DEBUG_S;

#endif
/*core share mem struct, don't use pointer or long*/
typedef struct tagICC_SUB_CHAN_S
{
	BSP_U32			u32SubChanState;
	BSP_U32			u32Signal;
	BSP_U32			u32WriteNeedSize;
	BSP_U32			u32SpinLock;
	BSP_U32			u32FIFOsize;
	BSP_U32			u32PhyStart;
	BSP_U32			u32PhyEnd;
	BSP_U32			u32PhyWrite;
	BSP_U32			u32PhyRead;
}ICC_SUB_CHAN_S;

/*core share mem struct, don't use pointer or long*/
typedef struct tagICC_CHAN_S
{
	ICC_CHAN_STATE_E	enState;
	BSP_BOOL			bAlloc;
	BSP_U32				u32Priority;
	BSP_U32				u32SpinLock;
	ICC_CHAN_MODE_E		enMode;
	BSP_U32			u32SubChanInfo[2];
#if(FEATURE_ICC_DEBUG == FEATURE_ON)
    BSP_U32             openBeginTime[2];   /*通道创建起始时间*/
    BSP_U32             openEndTime[2];     /*通道创建结束时间*/
	BSP_BOOL 			bLogSwitch;         /*log保存开关*/

#endif
}ICC_CHAN_S;


typedef struct tagICC_CTRL_CHAN_S
{
	BSP_BOOL		bInit[2];
	BSP_BOOL		bConnect;
	BSP_U32			u32SpinLock;
	ICC_CHAN_S		astChanInfo[ICC_CHAN_NUM_MAX];
	BSP_BOOL		bLog_level;
}ICC_CTRL_CHAN_S;

typedef struct tagICC_CHAN_DEV_S
{
	BSP_U32			u32ChanID;
	BSP_U32			u32Priority;
	ICC_CHAN_MODE_E		enMode;
	BSP_U32			*pSpinLock;
	ICC_SUB_CHAN_S	*ptxSubChanInfo;
	ICC_SUB_CHAN_S	*prxSubChanInfo;
	icc_event_cb		event_cb;
	icc_read_cb		read_cb;
	icc_write_cb		write_cb;
	struct semaphore	semSync;
	struct semaphore	semOpen;
	struct semaphore	semRead;
	ICC_PACKET_S        packet;
#if(FEATURE_ICC_DEBUG == FEATURE_ON)
	ICC_DEBUG_S         sDebug;
#endif
	struct tagICC_CHAN_DEV_S *next;
}ICC_CHAN_DEV_S;


typedef struct tagICC_DEV_S
{
	BSP_BOOL            bInit;
}ICC_DEV_S;

typedef struct tagICC_LOG_DATA
{
        struct work_struct ICC_work;
        void *buf;
        int buf_length;
}ICC_LOG_DATA;

typedef struct __IccUplayerChannelCB
{
        FUNCPTR_1 routine;
        int param;
}IccUplayerChannelCB;

/**************************************************************************
  函数声明
**************************************************************************/
/*Modify_for_c_reset, l00212112,20130511, starts*/
extern BSP_VOID BSP_ICC_ResetFun(void);
/*Modify_for_c_reset, l00212112,20130511, ends*/


/**************************************************************************
  全局变量声明
**************************************************************************/
/*
#define ICC_FLUSH_CACHE(ptr, size) \
    cacheFlush(DATA_CACHE, (void  *)(((BSP_U32)(ptr)) & (~0x1F)), (((size) + (((BSP_U32)(ptr)) & 0x1f) + 31)>>5)<<5)

#define ICC_INVALID_CACHE(ptr, size) \
    cacheInvalidate(DATA_CACHE, (void *)((BSP_U32)(ptr) & (~0x1F)), ((size + ((BSP_U32)(ptr) & 0x1f) + 31)>>5)<<5);
*/
#ifdef CONFIG_ARM64
/*ARM64 stub*/
#define ICC_FLUSH_CACHE(ptr, size) \
	__flush_dcache_area((void  *)(((unsigned long)(ptr)) & (~0x1F)), (((size) + (((unsigned long)(ptr)) & 0x1f) + 31)>>5)<<5)

#define ICC_INVALID_CACHE(ptr, size) /*\
	invalidate_kernel_vmap_range((void *)((unsigned long)(ptr) & (~0x1F)), (((size) + ((unsigned long)(ptr) & 0x1f) + 31)>>5)<<5);*/
#else
#define ICC_FLUSH_CACHE(ptr, size) \
	flush_kernel_vmap_range((void  *)(((unsigned long)(ptr)) & (~0x1F)), (((size) + (((unsigned long)(ptr)) & 0x1f) + 31)>>5)<<5)

#define ICC_INVALID_CACHE(ptr, size) \
	invalidate_kernel_vmap_range((void *)((unsigned long)(ptr) & (~0x1F)), (((size) + ((unsigned long)(ptr) & 0x1f) + 31)>>5)<<5);
#endif

/**************************************************************************
  错误码定义
**************************************************************************/



#ifdef __cplusplus
}
#endif

#endif



