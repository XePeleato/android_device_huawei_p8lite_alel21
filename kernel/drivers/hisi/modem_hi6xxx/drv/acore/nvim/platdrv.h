

#ifndef __PLATDRV_H__
#define __PLATRV_H__

#include <BSP.h>

#define OSAL_OK					(0)
#define OSAL_ERROR				(-1)

#define DRV_FUNCTION_TRACE  	50

#define DRV_PRIM_SEND_CONF_REQ  0x9001
#define DRV_PRIM_SEND_CONF_RSP  0x9002
#define DRV_PRIM_MCU_SWITCH     0x9510

#define INVALID_PRIM_ID         0x0000

#define PRIM_SEND_PERMIT        0x0001
#define PRIM_SEND_NOT_PERMIT    0x0000

#define DRV_MODULE_ID_BASE 		0x8101

typedef enum
{
    DRV_MODULE_ID_AMTS = DRV_MODULE_ID_BASE,
    DRV_MODULE_ID_AUDIO,
    DRV_MODULE_ID_BSP,
    DRV_MODULE_ID_CAMERA,
    DRV_MODULE_ID_DM,
    DRV_MODULE_ID_DMA,
    DRV_MODULE_ID_DSPC,
    DRV_MODULE_ID_END,
    DRV_MODULE_ID_FDI,
    DRV_MODULE_ID_FLASH,
    DRV_MODULE_ID_FM,
    DRV_MODULE_ID_GPIO,
    DRV_MODULE_ID_HICONTRL,
    DRV_MODULE_ID_HKADC,
    DRV_MODULE_ID_I2C,
    DRV_MODULE_ID_KEYPAD,
    DRV_MODULE_ID_LCD,
    DRV_MODULE_ID_LOGGER,
    DRV_MODULE_ID_PM,
    DRV_MODULE_ID_RTC,
    DRV_MODULE_ID_SCI,
    DRV_MODULE_ID_SDMMC,
    DRV_MODULE_ID_SPI,
    DRV_MODULE_ID_USB,
    DRV_MODULE_ID_WDT,
    DRV_MODULE_ID_NVIM,
    DRV_MODULE_ID_SYSCTRL,
    DRV_MODULE_ID_MNTN,
    DRV_MODULE_ID_TIMER,
    DRV_MODULE_ID_VIC,
    DRV_MODULE_ID_YAFFS,
    DRV_MODULE_ID_SDIO,
    DRV_MODULE_ID_WIFIEND,
    DRV_MODULE_ID_WIFIMVL,
    DRV_MODULE_ID_OLED,
    DRV_MODULE_ID_ONOFF,
    DRV_MODULE_ID_WIFI_SDIO,
    DRV_MODULE_ID_WIFI_DHD,
    DRV_MODULE_ID_WIFI_SD,
    DRV_MODULE_ID_WIFI_TRACE,
    DRV_MODULE_ID_WIFI_WL,
    DRV_MODULE_ID_WIFI_WPASUPPLICANT,
    DRV_MODULE_ID_ICC,
    DRV_MODULE_ID_MCU_MSG,
}LOG_MODULE_ID_EN;

typedef enum trace_type
{
    PWRCRL_CPULOAD 			= 0xa101,
    PWRCRL_SLEEP   			= 0xa102,
    PWRCRL_VOTE    			= 0xa103,
}PWRCTRL_TRACE_ENUM;

typedef enum trace_type_config
{
    PWRCRL_CPULOAD_SWTICH   = 0xa001,
    PWRCRL_SLEEP_SWTICH     = 0xa002,
}PWRCTRL_TRACE_CONFIG_ENUM;

/*om message header definition*/
#define OM_MSG_HEADER	BSP_U32 ulSenderCpuId;  \
						BSP_U32 ulSenderPid;    \
						BSP_U32 ulReceiverCpuId;\
						BSP_U32 ulReceiverPid;  \
						BSP_U32 ulLength;


typedef unsigned int	UINT_32;
typedef unsigned short 	UINT_16;
typedef unsigned char 	UINT_8;

typedef struct
{
    BSP_U8         		ucModuleId;
    BSP_U8         		ucReserved;
    BSP_U16        		usLength;
    BSP_U8         		aucFeedback[4];
}OM_RSP_PACKET_STRU;

typedef struct
{
    BSP_U8      		ucFuncType;
    BSP_U8      		ucReserve;
    BSP_U16     		usLength;
    BSP_U16     		usPrimId;
    BSP_U16     		usReserve;
    BSP_U8      		aucData[4];
}DRV_TRACE_REQ_STRU;

typedef struct
{
    BSP_U8      		ucModuleId;
    BSP_U8      		ucReserved;
    BSP_U16     		usLength;
    BSP_U8      		aucFeedback[4];
}OM_REQ_PACKET_STRU;

typedef struct
{
    BSP_U8  			ucFuncType;
    BSP_U8  			ucReserve;
    BSP_U16 			usLength;
    BSP_U32 			ulSn;
    BSP_U32 			ulTimeStamp;
    BSP_U16 			usPrimId;
    BSP_U16 			usReserve;
    BSP_U32 			ulModuleId;
}DRV_TRACE_IND_STRU;

typedef struct
{
	OM_MSG_HEADER
	BSP_U8 				aucValue[4];
}DRV_REPALY_IND_STRU;

typedef struct
{
    BSP_U16 			usVal;
    BSP_U16 			usPrimId;
}DRV_TRACE_PERMISSION;

typedef BSP_U32 OM_RSP_FUNC(OM_RSP_PACKET_STRU *pRspPacket, BSP_U16 usLength);
typedef BSP_VOID OM_REQUEST_PROCEDURE(OM_REQ_PACKET_STRU *pReqPacket,OM_RSP_FUNC *pRspFuncPtr);
typedef BSP_U32 OM_RegRqsProc(BSP_U8 ucModuleId, OM_REQUEST_PROCEDURE *pReqPro);
typedef BSP_VOID OM_AddSNTimeReg (BSP_U32 *pulSN, BSP_U32 *pulTimeStamp);
typedef BSP_U32 OM_PrintWithModule(BSP_U32 ulModuleId,BSP_U32 ulLevel, BSP_CHAR *pcformat, ...);
typedef BSP_U32 OM_Print(BSP_CHAR *pcformat, ...);
typedef BSP_U32 OM_PrintGetModuleIdLev(BSP_U32 ulModuleId);
typedef BSP_U32 OM_nv_read_fun(BSP_U16 usModemID, BSP_U16 usID, BSP_VOID *pItem, BSP_U32 ulLength);
typedef BSP_U32 OM_nv_write_fun(BSP_U16 usModemID, BSP_U16 usID, BSP_VOID *pItem, BSP_U32 ulLength);

typedef struct tagMSP_PROC_S
{
    OM_RegRqsProc			*OM_RegisterRequestProcedure;
	OM_AddSNTimeReg			*OM_AddSNTime;
    OM_PrintWithModule		*OM_PrintfWithModule;
    OM_Print				*OM_Printf;
    OM_PrintGetModuleIdLev 	*OM_PrintfGetModuleIdLev;
    OM_nv_read_fun			*OM_read_nv;
    OM_nv_write_fun			*OM_write_nv;
    MNTN_ERRLOGREGFUN		*OM_Errlog_func;
}MPS_PROC_S;

typedef struct
{
    unsigned short     		ccpu;
    unsigned short     		acpu;
}DRV_PWRCTRL_CPULOAD_SWITCH_STRU;

typedef struct
{
    unsigned short     		ccpu;
    unsigned short     		acpu;
}DRV_PWRCTRL_SLEEP_SWITCH_STRU;

BSP_VOID BSP_MNTN_OmDrvTraceReqEntry(OM_REQ_PACKET_STRU *pReqPacket,  OM_RSP_FUNC *pRsqFuncPtr);
BSP_S32 BSP_MNTN_OmDrvTraceSend(BSP_U16 usPrimId, BSP_U32 ulModuleId, BSP_U8 *buffer, BSP_U32 ulLength);
BSP_S32 BSP_MNTN_OmDrvReplayTraceSend(BSP_U16 usPrimId, BSP_U32 ulModuleId, BSP_U8 *buffer, BSP_U32 ulLength);
BSP_S32 BSP_MNTN_DrvPrimCanSendOrNot(BSP_U16 usPrimId);

#endif
