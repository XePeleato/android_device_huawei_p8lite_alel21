

#ifndef _ACPULOAD_
#define _ACPULOAD_

#ifdef __cplusplus
#if __cplusplus
    extern "C" {
#endif
#endif

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "vos.h"
#include "product_config.h"


#pragma pack(4)

/*****************************************************************************
  2 枚举定义
*****************************************************************************/


/*****************************************************************************
  3 宏定义
*****************************************************************************/

#define CPULOAD_MAX_HOOK_NUM        (2)     /* CPU负载上报最大勾子数 */

#define CPULOAD_REGULAR_TMR_DEF_LEN (400)   /* CPU统计默认间隔时间，单位毫秒 */
#define CPULOAD_REGULAR_TMR_MIN_LEN (100)   /* CPU统计最小间隔时间，单位毫秒 */
#define CPULOAD_REGULAR_TMR_MAX_LEN (1000)  /* CPU统计最大间隔时间，单位毫秒 */

#define CPULOAD_REGULAR_TMR_NAME    (1)

#define CPULOAD_CPU_IDLE            (0)     /* V9R1中OM调用CPULOAD_GetCpuLoad时，该函数打桩处理，返回空闲 */


/*****************************************************************************
  4 STRUCT定义
*****************************************************************************/
/*****************************************************************************
结构名    : CPULOAD_STAT_RECORD_STRU
协议表格  : 无
ASN.1描述 : 无
结构说明  : 记录CPU统计数据
*****************************************************************************/
typedef struct
{

    VOS_UINT32                      ulIdleTime;          /* 空闲任务运行时间 */
    VOS_UINT32                      ulTotalTime;         /* 总运行时间 */

}CPULOAD_STAT_RECORD_STRU;

/*****************************************************************************
结构名    : CPULOAD_STAT_INFO_STRU
协议表格  : 无
ASN.1描述 : 无
结构说明  : 计算CPU负载所需信息
*****************************************************************************/
typedef struct
{
	CPULOAD_STAT_RECORD_STRU    stCurrRecord;   /* 记录当前CPU统计信息 */
	CPULOAD_STAT_RECORD_STRU    stPrevRecord;   /* 记录上一次CPU统计信息 */

	VOS_UINT8                   ucCpuId;        /* the CPU ID number */
	VOS_UINT8                   aucRsv[3];
	VOS_UINT32                  ulCpuLoad;      /* 记录当前CPU负载 */

} CPULOAD_STAT_INFO_STRU;



/******************************************************************************
   5 函数声明
******************************************************************************/


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

#endif /* CpuLoad.h */

