#ifndef __MDRV_ACORE_STUB_H__
#define __MDRV_ACORE_STUB_H__
#ifdef __cplusplus
extern "C"
{
#endif

#include <mdrv_memory_common.h>
#include "mdrv_misc.h"
#include "mdrv_udi.h"
#include "mdrv_pm.h"
#include "mdrv_chg.h"

#if defined(CHIP_BB_HI6210)
/*****************************************************************************
 * 函 数 名  : BSP_MALLOC
 *
 * 功能描述  : BSP 动态内存分配
 *
 * 输入参数  : sz: 分配的大小(byte)
 *             flags: 内存属性(暂不使用,预留)
 * 输出参数  : 无
 * 返 回 值  : 分配出来的内存指针
 *****************************************************************************/
static inline void* BSP_MALLOC(unsigned int u32Size, MEM_POOL_TYPE enFlags) {
    return 0;
}

/*****************************************************************************
 * 函 数 名  : BSP_FREE
 *
 * 功能描述  : BSP 动态内存释放
 *
 * 输入参数  : ptr: 动态内存指针
 * 输出参数  : 无
 * 返 回 值  : 无
 *****************************************************************************/
static inline void  BSP_FREE(void* pMem) {
}

/*****************************************************************
Function: free_mem_size_get
Description:
    get mem size
Input:
    N/A
Output:
    N/A
Return:
    free mem size
*******************************************************************/
static inline unsigned int FREE_MEM_SIZE_GET(void)
{
    return 0;
}

static inline int BSP_DLOAD_GetDloadType(void)
{
    return 1;
}


/*****************************************************************************
 Function   : BSP_PWC_SetTimer4WakeSrc
 Description: 设置timer4作为唤醒源
 Input      :
 Return     : void
 Other      :
*****************************************************************************/
static inline void BSP_PWC_SetTimer4WakeSrc(void)
{
    return;
}


/*****************************************************************************
 Function   : BSP_PWC_DelTimer4WakeSrc
 Description: 设置timer4不作为唤醒源
 Input      :
            :
 Return     : void
 Other      :
*****************************************************************************/
static inline void BSP_PWC_DelTimer4WakeSrc(void)
{
    return;
}

/*****************************************************************************
 函 数 名  : DRV_PWRCTRL_SLEEPVOTE_LOCK
 功能描述  : 外设禁止睡眠投票接口。
 输入参数  : enClientId:PWC_CLIENT_ID_E
 输出参数  : None
 返 回 值  : PWC_PARA_INVALID/PWC_SUCCESS
*****************************************************************************/
static inline unsigned int  DRV_PWRCTRL_SLEEPVOTE_LOCK(PWC_CLIENT_ID_E enClientId)
{
    return 0;
}

/*****************************************************************************
 函 数 名  : DRV_PWRCTRL_SLEEPVOTE_UNLOCK
 功能描述  : 外设允许睡眠投票接口。
 输入参数  : enClientId:PWC_CLIENT_ID_E
 输出参数  : None
 返 回 值  : PWC_PARA_INVALID/PWC_SUCCESS
*****************************************************************************/
static inline unsigned int  DRV_PWRCTRL_SLEEPVOTE_UNLOCK(PWC_CLIENT_ID_E enClientId)
{
    return 0;
}

/*****************************************************************************
 函 数 名  : BSP_PWRCTRL_StandbyStateCcpu/BSP_PWRCTRL_StandbyStateAcpu
 功能描述  : AT^PSTANDBY
 输入参数  :
 输出参数  :
 返回值：
*****************************************************************************/
static inline unsigned int DRV_PWRCTRL_STANDBYSTATEACPU(unsigned int ulStandbyTime, unsigned int ulSwitchtime)
{
    return 0;
}

/*dload begin*/

#if 1
/* Begin: DRV为OAM实现NV备份恢复流程而提供的接口 */


static inline int DRV_UPGRADE_NV_SET_RESULT(int status)
{
    return 0;
}


static inline int DRV_NV_FILE_SEC_CHECK(char * pnvFilePath)
{
    return 0;
}


static inline int DRV_NVBACKUP_READ(unsigned char *pRamAddr, unsigned int len)
{
    return 0;
}



static inline int DRV_NVBACKUP_WRITE(unsigned char *pRamAddr, unsigned int len)
{
    return 0;
}





#endif

static inline  unsigned int DRV_SDMMC_GET_OPRT_STATUS(void)
{
    return 0;
}

static inline unsigned int DRV_SDMMC_USB_STATUS(void)
{
    return 0;
}

/*原drv_wifi.h*/
unsigned int USB_ETH_DrvSetHostAssembleParam(unsigned int ulHostOUtTimeout);
unsigned int USB_ETH_DrvSetDeviceAssembleParam(unsigned int ulEthTxMinNum,
                                                unsigned int ulEthTxTimeout,
                                                unsigned int ulEthRxMinNum,
                                                unsigned int ulEthRxTimeout);


/*************************************************
 函 数 名   : BSP_MNTN_ProductTypeGet
 功能描述   : 返回当前产品类型
 输入参数   : 无
 输出参数   : 无
 返 回 值   :0:STICK
             1:MOD
             2:E5
             3:CPE
*************************************************/
extern unsigned int BSP_MNTN_ProductTypeGet(void);
#define DRV_PRODUCT_TYPE_GET()   BSP_MNTN_ProductTypeGet()

#if 1 /*drv_usb.h*/

typedef enum
{
    WWAN_WCDMA    = 1,/*WCDMA模式*/
    WWAN_CDMA    = 2  /*CDMA模式*/
}NDIS_WWAN_MODE;

/*管脚信号的定义*/
#define IO_CTRL_FC                      (0x02)
#define IO_CTRL_DSR                     (0x20)
#define IO_CTRL_DTR                     (0x01)
#define IO_CTRL_RFR                     (0x04)
#define IO_CTRL_CTS                     (0x10)
#define IO_CTRL_RI                      (0x40)
#define IO_CTRL_DCD                     (0x80)

/* 设备枚举最大端口个数 */
#define DYNAMIC_PID_MAX_PORT_NUM        17

/* NV项50091结构，代表设备将要枚举的端口形态 */
typedef struct
{
    unsigned int ulStatus;
    unsigned char aucFirstPortStyle[DYNAMIC_PID_MAX_PORT_NUM];  /* 设备切换前端口形态 */
    unsigned char aucRewindPortStyle[DYNAMIC_PID_MAX_PORT_NUM]; /* 设备切换后端口形态 */
    unsigned char reserved[22];
}DRV_DYNAMIC_PID_TYPE_STRU;

typedef unsigned int (*MNTN_ERRLOGREGFUN)(char * cFileName,unsigned int ulFileId, unsigned int ulLine,
                unsigned int ulErrNo, void * pBuf, unsigned int ulLen);

/*****************************************************************************
 函 数 名  : BSP_USB_SetPid
 功能描述  :
 输入参数  : 无。
 输出参数  : 无。
 返 回 值  : 无。
*****************************************************************************/
extern int BSP_USB_SetPid(unsigned char u2diagValue);
#define DRV_SET_PID(u2diagValue)    BSP_USB_SetPid(u2diagValue)

/*****************************************************************************
函数名：   BSP_USB_PortTypeQuery
功能描述:  查询当前的设备枚举的端口形态值
输入参数： stDynamicPidType  端口形态
输出参数： stDynamicPidType  端口形态
返回值：   0:    查询成功
           其他：查询失败
*****************************************************************************/
unsigned int BSP_USB_PortTypeQuery(DRV_DYNAMIC_PID_TYPE_STRU *pstDynamicPidType);
#define DRV_SET_PORT_QUIRY(a)   BSP_USB_PortTypeQuery(a)

/*****************************************************************************
函数名：   BSP_USB_PortTypeValidCheck
功能描述:  提供给上层查询设备端口形态配置合法性接口
           1、端口为已支持类型，2、包含PCUI口，3、无重复端口，4、端点数不超过16，
           5、第一个设备不为MASS类
输入参数： pucPortType  端口形态配置
           ulPortNum    端口形态个数
返回值：   0:    端口形态合法
           其他：端口形态非法
*****************************************************************************/
unsigned int BSP_USB_PortTypeValidCheck(unsigned char *pucPortType, unsigned int ulPortNum);
#define DRV_USB_PORT_TYPE_VALID_CHECK(pucPortType, ulPortNum)  \
                    BSP_USB_PortTypeValidCheck(pucPortType, ulPortNum)

/*****************************************************************************
函数名：   BSP_USB_GetAvailabePortType
功能描述:  提供给上层查询当前设备支持端口形态列表接口
输入参数： ulPortMax    协议栈支持最大端口形态个数
输出参数:  pucPortType  支持的端口形态列表
           pulPortNum   支持的端口形态个数
返回值：   0:    获取端口形态列表成功
           其他：获取端口形态列表失败
*****************************************************************************/
unsigned int BSP_USB_GetAvailabePortType(unsigned char *pucPortType,
                            unsigned int *pulPortNum, unsigned int ulPortMax);
#define DRV_USB_GET_AVAILABLE_PORT_TYPE(pucPortType, pulPortNum, ulPortMax)  \
                BSP_USB_GetAvailabePortType(pucPortType, pulPortNum, ulPortMax)

/*****************************************************************************
 函 数 名  : BSP_USB_GetDiagModeValue
 功能描述  : 获得设备类型。
 输入参数  : 无。
 输出参数  : ucDialmode:  0 - 使用Modem拨号; 1 - 使用NDIS拨号; 2 - Modem和NDIS共存
              ucCdcSpec:   0 - Modem/NDIS都符合CDC规范; 1 - Modem符合CDC规范;
                           2 - NDIS符合CDC规范;         3 - Modem/NDIS都符合CDC规范
 返 回 值  : VOS_OK/VOS_ERR
*****************************************************************************/
extern int BSP_USB_GetDiagModeValue(unsigned char *pucDialmode,
                             unsigned char *pucCdcSpec);
#define DRV_GET_DIAG_MODE_VALUE(pucDialmode, pucCdcSpec)    \
                             BSP_USB_GetDiagModeValue(pucDialmode, pucCdcSpec)

/*****************************************************************************
 函 数 名  : BSP_USB_GetPortMode
 功能描述  : 获取端口形态模式，网关对接需求，打桩。
 输入参数  : 。
 输出参数  :
 返 回 值  :
*****************************************************************************/
extern unsigned char BSP_USB_GetPortMode(char*PsBuffer, unsigned int*Length );
#define DRV_GET_PORT_MODE(PsBuffer, Length)    BSP_USB_GetPortMode(PsBuffer,Length)

/*****************************************************************************
 函 数 名  : BSP_USB_GetU2diagDefaultValue
 功能描述  : 获得端口默认u2diag值
 输入参数  : 无。
 输出参数  : 无。
 返回值：   u2diag值

*****************************************************************************/
extern unsigned int BSP_USB_GetU2diagDefaultValue(void);
#define DRV_GET_U2DIAG_DEFVALUE()    BSP_USB_GetU2diagDefaultValue()


/*extern void ErrlogRegFunc(MNTN_ERRLOGREGFUN pRegFunc);
 */
extern void MNTN_ERRLOG_REG_FUNC(MNTN_ERRLOGREGFUN pRegFunc);

/*****************************************************************************
* 函 数 名  : DRV_DSP_AHB_RESET_CANCEL
* 功能描述  : ZSP AHB总线解复位
* 输入参数  : 无
* 输出参数  : 无
* 返 回 值  : 无
* 其它说明  : 无
*****************************************************************************/
extern void DRV_DSP_AHB_RESET_CANCEL(void);

/*****************************************************************************
 函 数 名  : BSP_USB_UdiagValueCheck
 功能描述  : 本接口用于检查NV项中USB形态值的合法性.
 输入参数  : 无。
 输出参数  : 无。
 返 回 值  : 0：OK；
            -1：ERROR
*****************************************************************************/
extern int BSP_USB_UdiagValueCheck(unsigned int DiagValue);
#define DRV_UDIAG_VALUE_CHECK(DiagValue)     BSP_USB_UdiagValueCheck(DiagValue)
#define DRV_U2DIAG_VALUE_CHECK(DiagValue)     BSP_USB_UdiagValueCheck(DiagValue)

/*****************************************************************************
 函 数 名  : BSP_USB_GetLinuxSysType
 功能描述  : 本接口用于检查PC侧是否为Linux，以规避Linux后台二次拨号失败的问题.
 输入参数  : 无。
 输出参数  : 无。
 返 回 值  : 0：Linux；
            -1：非Linux。
*****************************************************************************/
extern int BSP_USB_GetLinuxSysType(void) ;
#define DRV_GET_LINUXSYSTYPE()    BSP_USB_GetLinuxSysType()

/********************************************************
函数说明：协议栈注册HSIC使能通知回调函输
函数功能:
输入参数：pFunc: HSIC使能回调函输指针
输出参数：无
输出参数：无
返回值：  0：成功
          1：失败
********************************************************/
typedef void (*HSIC_UDI_ENABLE_CB_T)(void);
extern unsigned int BSP_HSIC_RegUdiEnableCB(HSIC_UDI_ENABLE_CB_T pFunc);
#define DRV_HSIC_REGUDI_ENABLECB(x) BSP_HSIC_RegUdiEnableCB(x)

/********************************************************
函数说明：协议栈注册HSIC去使能通知回调函输
函数功能:
输入参数：pFunc: HSIC去使能回调函输指针
输出参数：无
输出参数：无
返回值：  0：成功
          1：失败
********************************************************/
typedef void (*HSIC_UDI_DISABLE_CB_T)(void);
extern unsigned int BSP_HSIC_RegUdiDisableCB(HSIC_UDI_DISABLE_CB_T pFunc);
#define DRV_HSIC_REGUDI_DISABLECB(x) BSP_HSIC_RegUdiDisableCB(x)
/********************************************************
函数说明：协议栈查询HSIC枚举状态
函数功能:
输入参数：无
输出参数：无
输出参数：无
返回值：  1：枚举完成
          0：枚举未完成
********************************************************/
extern unsigned int BSP_GetHsicEnumStatus(void);
#define DRV_GET_HSIC_ENUM_STATUS() BSP_GetHsicEnumStatus()

/*****************************************************************************
* 函 数 名  : BSP_NCM_Write_Ready
* 功能描述  : 准备发送数据
* 输入参数  : void
* 输出参数  :
* 返 回 值  : void
* 修改记录  :
*****************************************************************************/
void BSP_NCM_Write_Ready(void);

/********************************************************
函数说明： 返回当前NV项中的设备形态值，是否支持PCSC
函数功能:
输入参数： dev_type是设备形态值（OAM从NV中读出）

输出参数： 无。
返回值：
          0: dev_type不支持PCSC设备；
          1：dev_type支持PCSC设备
********************************************************/
/*unsigned int pcsc_info_quiry(unsigned int ulDevType);*/
#define DRV_USB_PCSC_INFO_QUIRY(a)            MDRV_OK

typedef unsigned int (*pFunAPDUProcess)(unsigned int CmdType, unsigned char *pApdu, unsigned int ApduLen);
typedef unsigned int (*GetCardStatus)(void);

typedef struct
{
    pFunAPDUProcess   pFuncApdu;
    GetCardStatus  pFuncGetCardStatus;
}BSP_CCID_REGFUNC;

/*****************************************************************************
 函 数 名  : pcsc_usim_int
 功能描述  : 注册PCSC命令接收函数
 输入参数  :
 输出参数  : None
 返 回 值  : void
*****************************************************************************/
extern void pcsc_usim_int(pFunAPDUProcess pFun1, GetCardStatus pFun2);
#define DRV_PCSC_REG_CALLBACK(pFun1, pFun2) pcsc_usim_int((pFun1), (pFun2))


/*****************************************************************************
 函 数 名  : pcsc_usim_ctrl_cmd
 功能描述  : PCSC命令回复函数
 输入参数  :
 输出参数  : None
 返 回 值  : void
*****************************************************************************/
extern unsigned int pcsc_usim_ctrl_cmd(unsigned int CmdTpye, unsigned int Result,
                                        unsigned char *Buf, unsigned int Length);
#define DRV_PCSC_SEND_DATA(CmdType, Result, Buffer, Length) pcsc_usim_ctrl_cmd(CmdType, Result, Buffer, Length)

/* Porting联编，暂时定义为空 */
#define DRV_COM_RCV_CALLBACK_REGI(uPortNo,pCallback)    MDRV_OK

/************************************************************************
 * FUNCTION
 *       rndis_app_event_dispatch
 * DESCRIPTION
 *       闪电卡版本控制应用进行拨号或断开拨号连接
 * INPUTS
 *       进行拨号或断开拨号指示
 * OUTPUTS
 *       NONE
 *************************************************************************/
extern void DRV_AT_SETAPPDAILMODE(unsigned int ulStatus);


/************************************************************************
 * FUNCTION
 *       PDP_ACT_DRV_CALLBACK
 * DESCRIPTION
 *       PDP激活后调用底软的回调函数，原本在闪电卡上用作加载符号表信息，现在打桩
 * INPUTS
 *       NONE
 * OUTPUTS
 *       NONE
 *************************************************************************/
extern int PDP_ACT_DRV_CALLBACK(void);

typedef void(* USB_NET_DEV_SWITCH_GATEWAY)
(
    void
);

/*****************************************************************************
 函 数 名  : BSP_USB_NASSwitchGatewayRegExtFunc
 功能描述  : 本接口用于NAS注册切换网关通知回调函数
 输入参数  :回调接口。
 输出参数  : 无。
 返 回 值  : 0：成功
                       非零:失败
*****************************************************************************/
extern int BSP_USB_NASSwitchGatewayRegFunc(USB_NET_DEV_SWITCH_GATEWAY switchGwMode) ;
#define DRV_USB_NAS_SWITCH_GATEWAY_REGFUNC(switchGwMode)    BSP_USB_NASSwitchGatewayRegFunc(switchGwMode)
/*闪电卡还未开发 先暂时API 打桩end*/

/************************************其余宏定义为空的函数******************************************/
/*****************************************************************************
 函 数 名  : UsbStatusCallbackRegister
 功能描述  : 记录pCallBack至全局变量中。SD卡功耗相关接口，V7先打桩
 输入参数  : 无。
 输出参数  : 无。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
*****************************************************************************/
 /*extern int  UsbStatusCallbackRegister(pUsbState pCallBack);*/
#define DRV_USB_STATUS_CALLBACK_REGI(pCallBack)              MDRV_OK

/********************************************************************************************************
 函 数 名  : USB_otg_switch_signal_set
 功能描述  : 用于BALONG和K3对接时USB通道切换GPIO引脚控制
 输入参数  : 组号、引脚号和值
 输出参数  : 无

 返 回 值  : 0:  操作成功；
             -1：操作失败。
********************************************************************************************************/
extern int USB_otg_switch_signal_set(unsigned char ucGroup, unsigned char ucPin, unsigned char ucValue);
#define DRV_GPIO_SET(group, pin, value) USB_otg_switch_signal_set(group, pin, value)

#define DRV_GPIO_USB_SWITCH     200
#define DRV_GPIO_HIGH           1
#define DRV_GPIO_LOW            0

/********************************************************************************************************
 函 数 名  : USB_otg_switch_set
 功能描述  : 用于BALONG和K3对接时MODEM侧开启或关闭USB PHY
 输入参数  : 开启或关闭
 输出参数  : 无

 返 回 值  : 0:  操作成功；
             -1：操作失败。
********************************************************************************************************/
extern int USB_otg_switch_set(unsigned char ucValue);
#define DRV_USB_PHY_SWITCH_SET(value) USB_otg_switch_set(value)

#define AT_USB_SWITCH_SET_VBUS_VALID            1
#define AT_USB_SWITCH_SET_VBUS_INVALID          2

#define USB_SWITCH_ON       1
#define USB_SWITCH_OFF      0

/********************************************************************************************************
 函 数 名  : USB_otg_switch_get
 功能描述  : 用于BALONG和K3对接时MODEM侧USB PHY状态查询
 输入参数  : 无
 输出参数  : 返回USB PHY开启状态

 返 回 值  : 0:  操作成功；
             -1：操作失败。
********************************************************************************************************/
extern int USB_otg_switch_get(unsigned char *pucValue);
#define DRV_USB_PHY_SWITCH_GET(value) USB_otg_switch_get(value)



#define UART_IOCTL_SET_WRITE_CB      0x7F001000
#define UART_IOCTL_SET_READ_CB       0x7F001001
#define UART_IOCTL_SET_EVT_CB        0x7F001002
#define UART_IOCTL_SET_FREE_CB       0x7F001003
#define UART_IOCTL_WRITE_ASYNC       0x7F001010
#define UART_IOCTL_GET_RD_BUFF       0x7F001011
#define UART_IOCTL_RETURN_BUFF       0x7F001012
#define UART_IOCTL_RELLOC_READ_BUFF  0x7F001013
#define UART_IOCTL_SEND_BUFF_CAN_DMA 0x7F001014
#define UART_IOCTL_SET_WRITE_TIMEOUT 0x7F001015
#define UART_IOCTL_SET_BAUD           0X7F001016
#define UART_IOCTL_SET_WLEN           0X7F001017
#define UART_IOCTL_SET_STP2           0X7F001018
#define UART_IOCTL_SET_EPS            0X7F001019
#define UART_IOCTL_IS_IMPORT_DONE    0x7F001020
#define UDI_UART_IOCTL_SET_READ_CB           UART_IOCTL_SET_READ_CB
#define UDI_UART_IOCTL_GET_READ_BUFFER_CB    UART_IOCTL_GET_RD_BUFF
#define UDI_UART_IOCTL_RETUR_BUFFER_CB       UART_IOCTL_RETURN_BUFF
#define UART_IOCTL_SET_MSC_READ_CB 0x7F001021
#define UART_IOCTL_MSC_WRITE_CMD 0x7F001022
#define UART_IOCTL_SWITCH_MODE_CB 0x7F001023
#define UART_IOCTL_SET_FLOW_CONTROL 0x7F001024
#define UART_IOCTL_SET_AC_SHELL 0x7F001025

/* UART UDI 错误码*/
#define UART_UDI_ERROR_BASE               0x80000000

#define UART_ERR_MODULE_NOT_INITED      ((int)(BSP_DEF_ERR(BSP_MODU_UART, (int) BSP_ERR_MODULE_NOT_INITED) | UART_UDI_ERROR_BASE))
#define UART_ERR_NULL_PTR      ((int)(BSP_DEF_ERR(BSP_MODU_UART, (int) BSP_ERR_NULL_PTR) | UART_UDI_ERROR_BASE))
#define UART_ERR_INVALID_PARA      ((int)(BSP_DEF_ERR(BSP_MODU_UART, (int) BSP_ERR_INVALID_PARA) | UART_UDI_ERROR_BASE))
#define UART_ERR_RETRY_TIMEOUT      ((int)(BSP_DEF_ERR(BSP_MODU_UART, (int) BSP_ERR_RETRY_TIMEOUT) | UART_UDI_ERROR_BASE))
#define UART_ERR_BUF_ALLOC_FAILED      ((int)(BSP_DEF_ERR(BSP_MODU_UART, (int) BSP_ERR_BUF_ALLOC_FAILED) | UART_UDI_ERROR_BASE))
#define UART_ERR_BUF_FREE_FAILED      ((int)(BSP_DEF_ERR(BSP_MODU_UART, (int) BSP_ERR_BUF_FREE_FAILED) | UART_UDI_ERROR_BASE))

#define UART_A_SHELL  (0x5A5A5A5A)
#define UART_C_SHELL  (0xA5A5A5A5)

/* UART数据位长*/
typedef enum
{
    WLEN_5_BITS,
    WLEN_6_BITS,
    WLEN_7_BITS,
    WLEN_8_BITS,
    WLEN_MAX
}UART_WLEN_ENUM;

/* UART停止位长*/
typedef enum
{
    STP2_OFF,
    STP2_ON,
    STP2_MAX
}uart_stp2_enum;

/* UART校验方式*/
typedef enum
{
    PARITY_NO_CHECK,
    PARITY_CHECK_ODD,        /*奇校验*/
    PARITY_CHECK_EVEN,        /*偶校验*/
    PARITY_CHECK_MARK,        /*校验位始终为1*/
    PARITY_CHECK_SPACE,       /*校验位始终为0*/
    PARITY_CHECK_MAX
}uart_parity_enum;

/* 异步数据收发结构 */
typedef struct tagUART_WR_ASYNC_INFO
{
    char* pBuffer;
    unsigned int u32Size;
    void* pDrvPriv;
}uart_wr_async_info;
/* 读buffer配置信息 */
typedef struct tagUART_READ_BUFF_INFO
{
    unsigned int u32BuffSize;
    unsigned int u32BuffNum;
}uart_read_buff_info;
/* 流控开关结构 */
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  reserved1 : 14;/* bit[0-13] : reversed */
        unsigned int  rtsen         : 1;/* bit[14] : rts enable, up stream flow ctrl  */
        unsigned int  ctsen         : 1;/* bit[15] : cts enable, down stream flow ctrl */
        unsigned int  reserved2 : 16;/* bit[16-31] : reversed */
    } reg;
} uart_flow_ctrl_union;

typedef void (*uart_free_cb_t)(char* pBuff);
typedef void (*uart_read_cb_t)(void);
typedef void (*uart_msc_read_cb_t)(MODEM_MSC_STRU* pModemMsc);
typedef void (*uart_switch_mode_cb_t)(void);



typedef struct tagNCM_PKT_S
{
    unsigned char *pBuffer;       /* buffer指针*/
    unsigned int  u32BufLen;      /* buffer长度 */
}NCM_PKT_S;

/* NCM设备类型枚举*/
typedef enum tagNCM_DEV_TYPE_E
{
    NCM_DEV_DATA_TYPE,      /* 数据通道类型，PS使用*/
    NCM_DEV_CTRL_TYPE       /* 控制通道，MSP传输AT命令使用*/
}NCM_DEV_TYPE_E;


/* 包封装释放函数指针 */
typedef void (*USBFreePktEncap)(void *PktEncap);




/* 包封装结构体,NCM_IOCTL_SET_PKT_ENCAP_INFO命令字对应参数结构体*/
typedef struct tagNCM_PKT_ENCAP_INFO_S
{
    int s32BufOft;      /* buf偏移量 */
    int s32LenOft;      /* len偏移量 */
    int s32NextOft;     /* next偏移量 */
}NCM_PKT_ENCAP_INFO_S;


typedef struct tagNCM_IPV6_DNS_NTF_S
{
    unsigned char* pu8Ipv6DnsNtf;
    unsigned int u32Length;
} NCM_AT_IPV6_DNS_NTF_S;


/*  下传上传、下载包的各种统计信息，
    不发送给PC，被动等待PC的读取,NCM_IOCTL_SET_PKT_STATICS命令字对应参数结构*/
typedef struct tagNCM_IOCTL_PKT_STATISTICS_S
{
    unsigned int     u32TxOKCount;         /*发送包数*/
    unsigned int     u32RxOKCount;         /*接收包数*/
    unsigned int     u32TxErrCount;        /*发送错误*/
    unsigned int     u32RxErrCount;        /*接收错误*/
    unsigned int     u32TxOverFlowCount;   /*发送溢出丢包*/
    unsigned int     u32RxOverFlowCount;   /*接收溢出丢包*/
    unsigned int     u32CurrentTx;         /*发送速率*/
    unsigned int     u32CurrentRx;         /*接收速率*/
} NCM_IOCTL_PKT_STATISTICS_S;


typedef struct tagNCM_PKT_INFO_S
{
    unsigned int u32PsRcvPktNum;              /* 收包送到PS的包个数*/
    unsigned int u32RcvUnusedNum;             /* 收包不符合PS要求丢弃包个数*/
    unsigned int u32NcmSendPktNum;            /* 发包个数*/
}NCM_PKT_INFO_S;

/*函数回调指针定义*/
typedef  void  (*FUNC_USB_LP_NOTIFY)(int iStatus);
/*****************************************************************************
 函 数 名  : DRV_USB_LP_CB_REGISTER
 功能描述  : L2状态进入退出通知回调函数注册接口
 输入参数  : FUNC_USB_LP_NOTIFY *pUSBLPFunc回调函数指针
 输出参数  : 无
 返 回 值  : 0:    注册成功
           其他：注册失败
*****************************************************************************/
int l2_notify_register(FUNC_USB_LP_NOTIFY pUSBLPFunc);
#define DRV_USB_LP_CB_REGISTER(pUSBLPFunc)  \
                l2_notify_register(pUSBLPFunc)

/*TCP/IP协议栈可维可测捕获的消息标识*/
enum IPS_MNTN_TRACE_IP_MSG_TYPE_ENUM
{
    /* IP 数据包可维可测上报 */
    ID_IPS_TRACE_IP_ADS_UL                  = 0xD030,
    ID_IPS_TRACE_IP_ADS_DL                  = 0xD031,
    ID_IPS_TRACE_IP_USB_UL                  = 0xD032,
    ID_IPS_TRACE_IP_USB_DL                  = 0xD033,

    ID_IPS_TRACE_IP_MSG_TYPE_BUTT
};


extern int BSP_Modem_OS_Status_Switch(int enable);
#define DRV_OS_STATUS_SWITCH(enable) BSP_Modem_OS_Status_Switch(enable)

struct sk_buff;
typedef void (*USB_IPS_MNTN_TRACE_CB_T)(struct sk_buff *skb,unsigned short usType);
extern unsigned int BSP_USB_RegIpsTraceCB(USB_IPS_MNTN_TRACE_CB_T pFunc);
#define DRV_USB_REG_IPS_TRACECB(x) BSP_USB_RegIpsTraceCB(x)


unsigned int USB_ETH_DrvSetRxFlowCtrl(unsigned int ulParam1, unsigned int ulParam2);

#endif
#if 1

#define UDI_CAPA_BLOCK_READ    ((unsigned int)(1<<0)) /* 该设备的read接口为阻塞接口 */
#define UDI_CAPA_BLOCK_WRITE   ((unsigned int)(1<<1)) /* 该设备的write接口为阻塞接口 */
#define UDI_CAPA_READ_CB       ((unsigned int)(1<<2)) /* 该设备支持read 回调函数 */
#define UDI_CAPA_WRITE_CB      ((unsigned int)(1<<3)) /* 该设备支持write 回调函数 */
#define UDI_CAPA_BUFFER_LIST   ((unsigned int)(1<<4)) /* 该设备的读写buffer为内存链表结构(默认为普通内存块) */
#define UDI_CAPA_CTRL_OPT      ((unsigned int)(1<<5)) /* 该设备支持控制操作 */

#define MIN_UDI_HANDLE     0x5a0000
#define MAX_UDI_HANDLE     0x5a003f
#endif

#if 1  /*drv_uart_if.h*/
typedef enum _E_UART_CONSUMER_ID_
{
    CBP_UART_PORT_ID = 1,
    UART_CONSUMER_BUTT
}UART_CONSUMER_ID;

/*****************************************************************************
 函 数 名  : DRV_UART_SEND
 功能描述  : UART口发送数据
             本函数只支持任务上下文同步阻塞类数据发送
 输入参数  : uPortNo
             pDataBuffer
             uslength
 输出参数  : 无
 返 回 值  :  0  发送成功;  1  发送失败;
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年12月19日
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
extern int uart_core_send(UART_CONSUMER_ID uPortNo, unsigned char * pDataBuffer, unsigned int ulLength);
#define DRV_UART_SEND(uPortNo,pDataBuffer,ulLength) \
        uart_core_send(uPortNo,pDataBuffer,ulLength)
/*****************************************************************************
 函 数 名  : DRV_UART_RCV_CALLBACK_REGI
 功能描述  : UART口数据接受上层应用回调函数注册接口
 输入参数  : uPortNo
             pCallback

 输出参数  : 无
 返 回 值  :  0 :注册成功;  -1 ：注册失败
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年12月19日
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
typedef int (*pUARTRecv)(UART_CONSUMER_ID uPortNo,unsigned char * pData, unsigned int ulLength);
extern int uart_core_recv_handler_register(UART_CONSUMER_ID uPortNo, pUARTRecv pCallback);
#define DRV_UART_RCV_CALLBACK_REGI(uPortNo,pCallback)\
        uart_core_recv_handler_register(uPortNo,pCallback)
#endif

/*audio*/
static inline int DRV_CODEC_OPEN(const char *devname, int flags, int mode)
{
    return 0;
}

static inline void BSP_OM_SoftReboot(void)
{
    return ;
}

static inline int DRV_SDMMC_GET_STATUS(void)
{
    return 0;
}

/*om use begin*/
/*VERSIONINFO_I数据结构中版本字符串最大有效字符长度*/
#define VER_MAX_LENGTH                  30

typedef struct
{
    unsigned char CompId;              /* 组件号：参见COMP_TYPE */
    unsigned char CompVer[VER_MAX_LENGTH+1];         /* 最大版本长度 30 字符+ \0 */
}VERSIONINFO_I;
typedef enum{
     ADDRTYPE8BIT,
     ADDRTYPE16BIT,
     ADDRTYPE32BIT
}ENADDRTYPE;

extern int BSP_OM_RegRead(unsigned int* u32RegAddr,
    ENADDRTYPE enAddrType, unsigned int *pu32Value);

extern int BSP_OM_RegWrite(unsigned int* u32RegAddr,
    ENADDRTYPE enAddrType, unsigned int u32Value);
typedef struct tagBSP_OM_NET_S
{
    unsigned int u32NetRxStatOverFlow;       /* 接收FIFO溢出统计计数 */
    unsigned int u32NetRxStatPktErr;         /* 接收总错包计数 */
    unsigned int u32NetRxStatCrcErr;         /* 接收CRC错包计数 */
    unsigned int u32NetRxStatLenErr;         /* 接收无效长度包计数 */
    unsigned int u32NetRxNoBufInt;           /* 接收没有BUFFER中断计数 */
    unsigned int u32NetRxStopInt;            /* 接收停止中断计数 */
    unsigned int u32NetRxDescErr;            /* 接收描述符错误 */

    unsigned int u32NetTxStatUnderFlow;      /* 发送FIFO下溢统计计数 */
    unsigned int u32NetTxUnderFlowInt;       /* 发送FIFO下溢中断计数 */
    unsigned int u32NetTxStopInt;            /* 发送停止中断计数 */
    unsigned int u32NetTxDescErrPs;          /* 发送描述符错误(Ps) */
    unsigned int u32NetTxDescErrOs;          /* 发送描述符错误(Os) */
    unsigned int u32NetTxDescErrMsp;         /* 发送描述符错误(Msp) */

    unsigned int u32NetFatalBusErrInt;      /* 总线错误*/
}BSP_OM_NET_S;

/*om use end*/

/*taf used*/
enum SECURE_ENABLE_STATUS_I
{
    SECURE_DISABLE = 0,
    SECURE_ENABLE = 1
};

static inline int DRV_FILE_GET_DISKSPACE(const char *path,unsigned int *DskSpc,unsigned int *UsdSpc,  unsigned int *VldSpc)
{
    return 0;
}

#if 1  /*drv_sec.h*/
/*************************************************
 函 数 名       : DRV_SECURE_SUPPORT
 功能描述   : 当前版本是否支持安全启动
 输入参数   : unsigned char *pData
 输出参数   : unsigned char *pData
 返 回 值      : OK/ERROR
*************************************************/
extern int DRV_SECURE_SUPPORT(unsigned char *pu8Data);

/*************************************************
 函 数 名       : DRV_SECURE_ALREADY_USE
 功能描述   : 查询当前版本是否已经启用安全启动
 输入参数   : unsigned char *pData
 输出参数   : unsigned char *pData
 返 回 值      : OK/ERROR
*************************************************/
extern int DRV_SECURE_ALREADY_USE(unsigned char *pu8Data);

/*************************************************
 函 数 名       : DRV_START_SECURE
 功能描述   : 启用安全启动
 输入参数   :
 输出参数   :
 返 回 值      : OK/ERROR
*************************************************/
extern int DRV_START_SECURE(void);

/*****************************************************************************
* 函 数 名  : mdrv_crypto_check_sec
*
* 功能描述  : 安全镜像判断
*
* 输入参数  : void
* 输出参数  :
*
* 返 回 值  : 1，安全镜像；0，非安全镜像
*
* 其它说明  :
*
*****************************************************************************/
unsigned int mdrv_crypto_check_sec (void);
#endif

#if 1  /*drv_pmu.h*/
/*****************************************************************************
 函 数 名  : DRV_GET_PMU_STATE
 功能描述  : 获取PMU模块开机方式、充电状态、电池电量、电池在位状态。
 输入参数  : 无。
 输出参数  : Pmu_State :开机方式、充电状态、电池电量、电池在位状态。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
 注意事项　：该接口仅在PS的PC工程中使用，目前没有应用，暂保留。
*****************************************************************************/
extern unsigned int DRV_GET_PMU_STATE(void*  Pmu_State);

/*****************************************************************************
函数名：   BSP_USB_PortTypeValidCheck
功能描述:  提供给上层查询设备端口形态配置合法性接口
           1、端口为已支持类型，2、包含PCUI口，3、无重复端口，4、端点数不超过16，
           5、第一个设备不为MASS类
输入参数： pucPortType  端口形态配置
           ulPortNum    端口形态个数
返回值：   0:    端口形态合法
           其他：端口形态非法
*****************************************************************************/
unsigned int BSP_USB_PortTypeValidCheck(unsigned char *pucPortType, unsigned int ulPortNum);
#define DRV_USB_PORT_TYPE_VALID_CHECK(pucPortType, ulPortNum)  \
                    BSP_USB_PortTypeValidCheck(pucPortType, ulPortNum)

/*****************************************************************************
函数名：   BSP_USB_GetAvailabePortType
功能描述:  提供给上层查询当前设备支持端口形态列表接口
输入参数： ulPortMax    协议栈支持最大端口形态个数
输出参数:  pucPortType  支持的端口形态列表
           pulPortNum   支持的端口形态个数
返回值：   0:    获取端口形态列表成功
           其他：获取端口形态列表失败
*****************************************************************************/
unsigned int BSP_USB_GetAvailabePortType(unsigned char *pucPortType,
                            unsigned int *pulPortNum, unsigned int ulPortMax);
#define DRV_USB_GET_AVAILABLE_PORT_TYPE(pucPortType, pulPortNum, ulPortMax)  \
                BSP_USB_GetAvailabePortType(pucPortType, pulPortNum, ulPortMax)
#endif

#if 1  /*drv_oled.h*/

/*****************************************************************************
*  Function:  DRV_OLED_CLEAR_WHOLE_SCREEN
*  Description: oled clear *
*  Called By:AP
*  Table Accessed:
*  Table Updated:
*  Input:
*         N/A
*  Output:
*         N/A
*  Return:
*         N/A
*****************************************************************************/
extern void DRV_OLED_CLEAR_WHOLE_SCREEN(void);

/*****************************************************************************
*  Function:  DRV_OLED_UPDATE_STATE_DISPLAY
*  Description: oled display right or not right  *
*  Called By:AP
*  Table Accessed:
*  Table Updated:
*  Input:
*         N/A
*  Output:
*         N/A
*  Return:
*         N/A
*****************************************************************************/
extern  void DRV_OLED_UPDATE_STATE_DISPLAY(int UpdateStatus);

/*****************************************************************************
 函 数 名  : DRV_OLED_UPDATE_DISPLAY
 功能描述  : SD卡烧片版本升级，OLED显示
 输入参数  : BOOL UpdateStatus
 输出参数  : 无。
 返回值：   无
 其它:  升级成功，NV备份完成后调用该函数打勾
        升级失败后调用该函数打叉
*****************************************************************************/
extern void DRV_OLED_UPDATE_DISPLAY(int UpdateStatus);

/*****************************************************************************
 函 数 名  : DRV_OLED_STRING_DISPLAY
 功能描述  : oled字符串显示函数
 输入参数  :
 *           要显示字符串的起始坐标
 *           要显示的字符串
 输出参数  : none
 返 回 值  : void
*****************************************************************************/
extern void DRV_OLED_STRING_DISPLAY(unsigned char ucX, unsigned char ucY, unsigned char *pucStr);

#endif

#if 1  /*drv_nve.h*/
#define BSP_NVE_NAME_LENGTH          8       /*NV name maximum length*/
#define BSP_NVE_NV_DATA_SIZE        104     /*NV data maximum length*/

#define BSP_NV_READ                 1       /*NV read  operation*/
#define BSP_NV_WRITE                0       /*NV write operation*/

typedef struct nve_info_stru {
	unsigned int nv_operation;              /*0-write,1-read*/
	unsigned int nv_number;                 /*NV number you want to visit*/
	char nv_name[BSP_NVE_NAME_LENGTH];
	unsigned int valid_size;
	unsigned char nv_data[BSP_NVE_NV_DATA_SIZE];
}NVE_INFO_S;

/*****************************************************************************
* 函 数 名  : DRV_NVE_ACCESS
*
* 功能描述  : 访问LINUX NVE
*
* 输入参数  : nve  ID
*
* 输出参数  : 无
*
* 返 回 值  : OK& ERROR
*
* 修改记录 :  2013年6月27日  v1.00  yuanqinshun  创建
*****************************************************************************/
int DRV_NVE_ACCESS(NVE_INFO_S *nve);
#endif

#if 1  /*drv_nv.h*/

extern int DRV_COPY_NVUSE_TO_NVBACKUP ( void );



extern int DRV_COPY_NVUPGRADE_TO_NVUSE( void );
#endif

typedef enum tagMSP_PROC_ID_E
{
    OM_REQUEST_PROC = 0,
    OM_ADDSNTIME_PROC = 1,
    OM_PRINTF_WITH_MODULE = 2,
    OM_PRINTF = 3,

    OM_PRINTF_GET_MODULE_IDLEV = 4,
    OM_READ_NV_PROC = 5,
    OM_WRITE_NV_PROC = 6,
    OM_MNTN_ERRLOG = 7,

    MSP_PROC_REG_ID_MAX
}MSP_PROC_ID_E;

typedef void (*BSP_MspProc)(void);
#define EXCH_CB_NAME_SIZE           (32)
typedef struct
{
    char   aucName[EXCH_CB_NAME_SIZE];
    unsigned char *pucData;
    unsigned int  ulDataLen;
}cb_buf_t;

typedef int  (*exchCBReg)(cb_buf_t *);

/*liuyi++ 该接口在文档中未体现 ++*/
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
extern int DRV_EXCH_CUST_FUNC_REG(exchCBReg cb);
/*liuyi++ 该接口在文档中未体现 --*/

/*liuyi++ 该接口在文档中未体现*/
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
*
*****************************************************************************/
extern void DRV_MSP_PROC_REG(MSP_PROC_ID_E eFuncID, BSP_MspProc pFunc);
/*liuyi++ 该接口在文档中未体现--*/
#else
/*****************************************************************************
 * 函 数 名  : BSP_MALLOC
 *
 * 功能描述  : BSP 动态内存分配
 *
 * 输入参数  : sz: 分配的大小(byte)
 *             flags: 内存属性(暂不使用,预留)
 * 输出参数  : 无
 * 返 回 值  : 分配出来的内存指针
 *****************************************************************************/
static inline void* BSP_MALLOC(unsigned int u32Size, MEM_POOL_TYPE enFlags) {
    return 0;
}

/*****************************************************************************
 * 函 数 名  : BSP_FREE
 *
 * 功能描述  : BSP 动态内存释放
 *
 * 输入参数  : ptr: 动态内存指针
 * 输出参数  : 无
 * 返 回 值  : 无
 *****************************************************************************/
static inline void  BSP_FREE(void* pMem) {
}

/*****************************************************************
Function: free_mem_size_get
Description:
    get mem size
Input:
    N/A
Output:
    N/A
Return:
    free mem size
*******************************************************************/
static inline unsigned int FREE_MEM_SIZE_GET(void)
{
    return 0;
}

static inline int BSP_DLOAD_GetDloadType(void)
{
    return 1;
}

/*liuyi++ only for nas marenkun temp*/

/*****************************************************************************
 Function   : BSP_PWC_SetTimer4WakeSrc
 Description: 设置timer4作为唤醒源
 Input      :
 Return     : void
 Other      :
*****************************************************************************/
static inline void BSP_PWC_SetTimer4WakeSrc(void)
{
    return;
}


/*****************************************************************************
 Function   : BSP_PWC_DelTimer4WakeSrc
 Description: 设置timer4不作为唤醒源
 Input      :
            :
 Return     : void
 Other      :
*****************************************************************************/
static inline void BSP_PWC_DelTimer4WakeSrc(void)
{
    return;
}

/*****************************************************************************
 函 数 名  : DRV_PWRCTRL_SLEEPVOTE_LOCK
 功能描述  : 外设禁止睡眠投票接口。
 输入参数  : enClientId:PWC_CLIENT_ID_E
 输出参数  : None
 返 回 值  : PWC_PARA_INVALID/PWC_SUCCESS
*****************************************************************************/
static inline unsigned int  DRV_PWRCTRL_SLEEPVOTE_LOCK(PWC_CLIENT_ID_E enClientId)
{
    return 0;
}

/*****************************************************************************
 函 数 名  : DRV_PWRCTRL_SLEEPVOTE_UNLOCK
 功能描述  : 外设允许睡眠投票接口。
 输入参数  : enClientId:PWC_CLIENT_ID_E
 输出参数  : None
 返 回 值  : PWC_PARA_INVALID/PWC_SUCCESS
*****************************************************************************/
static inline unsigned int  DRV_PWRCTRL_SLEEPVOTE_UNLOCK(PWC_CLIENT_ID_E enClientId)
{
    return 0;
}

/*****************************************************************************
 函 数 名  : BSP_PWRCTRL_StandbyStateCcpu/BSP_PWRCTRL_StandbyStateAcpu
 功能描述  : AT^PSTANDBY
 输入参数  :
 输出参数  :
 返回值：
*****************************************************************************/
static inline unsigned int DRV_PWRCTRL_STANDBYSTATEACPU(unsigned int ulStandbyTime, unsigned int ulSwitchtime)
{
    return 0;
}

/*dload begin*/

#if 1
/* Begin: DRV为OAM实现NV备份恢复流程而提供的接口 */


static inline int DRV_UPGRADE_NV_SET_RESULT(int status)
{
    return 0;
}


static inline int DRV_NV_FILE_SEC_CHECK(char * pnvFilePath)
{
    return 0;
}


static inline int DRV_NVBACKUP_READ(unsigned char *pRamAddr, unsigned int len)
{
    return 0;
}



static inline int DRV_NVBACKUP_WRITE(unsigned char *pRamAddr, unsigned int len)
{
    return 0;
}





#endif


#define UART_IOCTL_GET_RD_BUFF       0x7F001011
#define UART_IOCTL_SET_READ_CB       0x7F001001

typedef enum
{
    WWAN_WCDMA    = 1,/*WCDMA模式*/
    WWAN_CDMA    = 2  /*CDMA模式*/
}NDIS_WWAN_MODE;

/*管脚信号的定义*/
#define IO_CTRL_FC                      (0x02)
#define IO_CTRL_DSR                     (0x20)
#define IO_CTRL_DTR                     (0x01)
#define IO_CTRL_RFR                     (0x04)
#define IO_CTRL_CTS                     (0x10)
#define IO_CTRL_RI                      (0x40)
#define IO_CTRL_DCD                     (0x80)

#define DRV_GPIO_USB_SWITCH     200
#define DRV_GPIO_HIGH           1
#define DRV_GPIO_LOW            0

#define AT_USB_SWITCH_SET_VBUS_INVALID          2

/* 设备枚举最大端口个数 */
#define DYNAMIC_PID_MAX_PORT_NUM        17

/* NV项50091结构，代表设备将要枚举的端口形态 */
typedef struct
{
    unsigned int ulStatus;
    unsigned char aucFirstPortStyle[DYNAMIC_PID_MAX_PORT_NUM];  /* 设备切换前端口形态 */
    unsigned char aucRewindPortStyle[DYNAMIC_PID_MAX_PORT_NUM]; /* 设备切换后端口形态 */
    unsigned char reserved[22];
}DRV_DYNAMIC_PID_TYPE_STRU;

typedef void(* USB_NET_DEV_SWITCH_GATEWAY)(void);

static inline int DRV_OS_STATUS_SWITCH(int enable)
{
    return 0;
}

static inline int DRV_GET_LINUXSYSTYPE(void)
{
    return -1;
}

static inline DRV_SET_PORT_QUIRY(DRV_DYNAMIC_PID_TYPE_STRU *pstDynamicPidType)
{
    return 0;
}

static inline int DRV_USB_NAS_SWITCH_GATEWAY_REGFUNC(USB_NET_DEV_SWITCH_GATEWAY switchGwMode)
{
    return 0;
}

static inline unsigned int DRV_GET_U2DIAG_DEFVALUE(void)
{
    return 0;
}

static inline int DRV_GET_DIAG_MODE_VALUE(unsigned char *pucDialmode,
                                          unsigned char *pucCdcSpec)
{
    return 1;
}

static inline int DRV_USB_PHY_SWITCH_SET(unsigned char ucValue)
{
    return -1;
}

static inline int DRV_USB_PORT_TYPE_VALID_CHECK(unsigned char *pucPortType,
                                                unsigned int ulPortNum)
{
    return 0;
}

static inline int DRV_UDIAG_VALUE_CHECK(unsigned int DiagValue)
{
    return 1;
}

static inline int DRV_U2DIAG_VALUE_CHECK(unsigned int DiagValue)
{
    return 1;
}

static inline unsigned char DRV_GET_PORT_MODE(char*PsBuffer, unsigned int*Length)
{
    return 1;
}

static inline int DRV_SET_PID(unsigned char u2diagValue)
{
    return 1;
}

static inline int DRV_GPIO_SET(unsigned char ucGroup, unsigned char ucPin, unsigned char ucValue)
{
    return -1;
}

static inline void DRV_AT_SETAPPDAILMODE(unsigned int ulStatus)
{
    return;
}
/*************************************************
 函 数 名   : BSP_MNTN_ProductTypeGet
 功能描述   : 返回当前产品类型
 输入参数   : 无
 输出参数   : 无
 返 回 值   :0:STICK
             1:MOD
             2:E5
             3:CPE
*************************************************/
static inline unsigned int BSP_MNTN_ProductTypeGet(void)
{
	return 0;
}
#define DRV_PRODUCT_TYPE_GET()   BSP_MNTN_ProductTypeGet()

static inline int DRV_SDMMC_GET_STATUS(void)
{
    return 0;
}

/*om use begin*/
/*VERSIONINFO_I数据结构中版本字符串最大有效字符长度*/
#define VER_MAX_LENGTH                  30

typedef struct
{
    unsigned char CompId;              /* 组件号：参见COMP_TYPE */
    unsigned char CompVer[VER_MAX_LENGTH+1];         /* 最大版本长度 30 字符+ \0 */
}VERSIONINFO_I;

typedef enum{
    ADDRTYPE8BIT,
    ADDRTYPE16BIT,
    ADDRTYPE32BIT
}ENADDRTYPE;

typedef struct tagBSP_OM_NET_S
{
    unsigned int u32NetRxStatOverFlow;       /* 接收FIFO溢出统计计数 */
    unsigned int u32NetRxStatPktErr;         /* 接收总错包计数 */
    unsigned int u32NetRxStatCrcErr;         /* 接收CRC错包计数 */
    unsigned int u32NetRxStatLenErr;         /* 接收无效长度包计数 */
    unsigned int u32NetRxNoBufInt;           /* 接收没有BUFFER中断计数 */
    unsigned int u32NetRxStopInt;            /* 接收停止中断计数 */
    unsigned int u32NetRxDescErr;            /* 接收描述符错误 */

    unsigned int u32NetTxStatUnderFlow;      /* 发送FIFO下溢统计计数 */
    unsigned int u32NetTxUnderFlowInt;       /* 发送FIFO下溢中断计数 */
    unsigned int u32NetTxStopInt;            /* 发送停止中断计数 */
    unsigned int u32NetTxDescErrPs;          /* 发送描述符错误(Ps) */
    unsigned int u32NetTxDescErrOs;          /* 发送描述符错误(Os) */
    unsigned int u32NetTxDescErrMsp;         /* 发送描述符错误(Msp) */

    unsigned int u32NetFatalBusErrInt;      /* 总线错误*/
}BSP_OM_NET_S;

/*om use end*/

/*taf used*/
enum SECURE_ENABLE_STATUS_I
{
    SECURE_DISABLE = 0,
    SECURE_ENABLE = 1
};

static inline int DRV_FILE_GET_DISKSPACE(const char *path,unsigned int *DskSpc,unsigned int *UsdSpc,  unsigned int *VldSpc)
{
    return 0;
}
/*liuyi--*/

#if 1  /*drv_timer.h*/
extern unsigned int omTimerGet(void);
#define DRV_GET_SLICE() omTimerGet()
#endif

#if 1  /*drv_oled.h*/

/*****************************************************************************
*  Function:  DRV_OLED_CLEAR_WHOLE_SCREEN
*  Description: oled clear *
*  Called By:AP
*  Table Accessed:
*  Table Updated:
*  Input:
*         N/A
*  Output:
*         N/A
*  Return:
*         N/A
*****************************************************************************/
extern void DRV_OLED_CLEAR_WHOLE_SCREEN(void);

/*****************************************************************************
*  Function:  DRV_OLED_UPDATE_STATE_DISPLAY
*  Description: oled display right or not right  *
*  Called By:AP
*  Table Accessed:
*  Table Updated:
*  Input:
*         N/A
*  Output:
*         N/A
*  Return:
*         N/A
*****************************************************************************/
extern  void DRV_OLED_UPDATE_STATE_DISPLAY(int UpdateStatus);

/*****************************************************************************
 函 数 名  : DRV_OLED_UPDATE_DISPLAY
 功能描述  : SD卡烧片版本升级，OLED显示
 输入参数  : BOOL UpdateStatus
 输出参数  : 无。
 返回值：   无
 其它:  升级成功，NV备份完成后调用该函数打勾
        升级失败后调用该函数打叉
*****************************************************************************/
extern void DRV_OLED_UPDATE_DISPLAY(int UpdateStatus);

/*****************************************************************************
 函 数 名  : DRV_OLED_STRING_DISPLAY
 功能描述  : oled字符串显示函数
 输入参数  :
 *           要显示字符串的起始坐标
 *           要显示的字符串
 输出参数  : none
 返 回 值  : void
*****************************************************************************/
extern void DRV_OLED_STRING_DISPLAY(unsigned char ucX, unsigned char ucY, unsigned char *pucStr);

#endif

#if 1  /*drv_nve.h*/
#define BSP_NVE_NAME_LENGTH          8       /*NV name maximum length*/
#define BSP_NVE_NV_DATA_SIZE        104     /*NV data maximum length*/

#define BSP_NV_READ                 1       /*NV read  operation*/
#define BSP_NV_WRITE                0       /*NV write operation*/

typedef struct nve_info_stru {
	unsigned int nv_operation;              /*0-write,1-read*/
	unsigned int nv_number;                 /*NV number you want to visit*/
	char nv_name[BSP_NVE_NAME_LENGTH];
	unsigned int valid_size;
	unsigned char nv_data[BSP_NVE_NV_DATA_SIZE];
}NVE_INFO_S;

/*****************************************************************************
* 函 数 名  : DRV_NVE_ACCESS
*
* 功能描述  : 访问LINUX NVE
*
* 输入参数  : nve  ID
*
* 输出参数  : 无
*
* 返 回 值  : OK& ERROR
*
* 修改记录 :  2013年6月27日  v1.00  yuanqinshun  创建
*****************************************************************************/
int DRV_NVE_ACCESS(NVE_INFO_S *nve);
#endif

#if 1  /*drv_nv.h*/

extern int DRV_COPY_NVUSE_TO_NVBACKUP ( void );



extern int DRV_COPY_NVUPGRADE_TO_NVUSE( void );
#endif

typedef enum tagMSP_PROC_ID_E
{
    OM_REQUEST_PROC = 0,
    OM_ADDSNTIME_PROC = 1,
    OM_PRINTF_WITH_MODULE = 2,
    OM_PRINTF = 3,

    OM_PRINTF_GET_MODULE_IDLEV = 4,
    OM_READ_NV_PROC = 5,
    OM_WRITE_NV_PROC = 6,
    OM_MNTN_ERRLOG = 7,

    MSP_PROC_REG_ID_MAX
}MSP_PROC_ID_E;

typedef void (*BSP_MspProc)(void);
#define EXCH_CB_NAME_SIZE           (32)
typedef struct
{
    char   aucName[EXCH_CB_NAME_SIZE];
    unsigned char *pucData;
    unsigned int  ulDataLen;
}cb_buf_t;

typedef int  (*exchCBReg)(cb_buf_t *);

/*liuyi++ 该接口在文档中未体现 ++*/
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
extern int DRV_EXCH_CUST_FUNC_REG(exchCBReg cb);
/*liuyi++ 该接口在文档中未体现 --*/

/*liuyi++ 该接口在文档中未体现*/
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
*
*****************************************************************************/
#define DRV_MSP_PROC_REG(eFuncID, pFunc)
/*liuyi++ 该接口在文档中未体现--*/


/************************* 底软不用结构体(后续删除) ***************************/

/*TCP/IP协议栈可维可测捕获的消息标识*/
enum IPS_MNTN_TRACE_IP_MSG_TYPE_ENUM
{
    /* IP 数据包可维可测上报 */
    ID_IPS_TRACE_IP_ADS_UL                  = 0xD030,
    ID_IPS_TRACE_IP_ADS_DL                  = 0xD031,
    ID_IPS_TRACE_IP_USB_UL                  = 0xD032,
    ID_IPS_TRACE_IP_USB_DL                  = 0xD033,

    ID_IPS_TRACE_IP_MSG_TYPE_BUTT
};

static inline unsigned long USB_ETH_DrvSetHostAssembleParam(unsigned long ulHostOutTimeout)
{
    return 0;
}

static inline unsigned long USB_ETH_DrvSetDeviceAssembleParam(
    unsigned long ulEthTxMinNum,
    unsigned long ulEthTxTimeout,
    unsigned long ulEthRxMinNum,
    unsigned long ulEthRxTimeout)
{
    return 0;
}

static inline unsigned int BSP_USB_GetAvailabePortType(unsigned char *pucPortType,
                            unsigned long *pulPortNum, unsigned long ulPortMax)
{
    return 0;
}
#define DRV_USB_GET_AVAILABLE_PORT_TYPE(pucPortType, pulPortNum, ulPortMax)  \
                BSP_USB_GetAvailabePortType(pucPortType, pulPortNum, ulPortMax)


static inline int USB_otg_switch_get(unsigned char *pucValue)
{
    return 0;
}
#define DRV_USB_PHY_SWITCH_GET(value) USB_otg_switch_get(value)
#define AT_USB_SWITCH_SET_VBUS_VALID            1
#define AT_USB_SWITCH_SET_VBUS_INVALID          2
#define USB_SWITCH_ON       1
#define USB_SWITCH_OFF      0


typedef void (*HSIC_UDI_ENABLE_CB_T)(void);
static inline unsigned int BSP_HSIC_RegUdiEnableCB(HSIC_UDI_ENABLE_CB_T pFunc)
{
    return 0;
}
#define DRV_HSIC_REGUDI_ENABLECB(x) BSP_HSIC_RegUdiEnableCB(x)

typedef void (*HSIC_UDI_DISABLE_CB_T)(void);
static inline unsigned int BSP_HSIC_RegUdiDisableCB(HSIC_UDI_DISABLE_CB_T pFunc)
{
    return 0;
}
#define DRV_HSIC_REGUDI_DISABLECB(x) BSP_HSIC_RegUdiDisableCB(x)

static inline unsigned int BSP_GetHsicEnumStatus(void)
{
    return 0;
}
#define DRV_GET_HSIC_ENUM_STATUS() BSP_GetHsicEnumStatus()


/************************* 底软不用结构体 END *********************************/

//struct sk_buff;
typedef void (*USB_IPS_MNTN_TRACE_CB_T)(struct sk_buff *skb,unsigned short usType);
static inline unsigned int DRV_USB_REG_IPS_TRACECB(USB_IPS_MNTN_TRACE_CB_T pFunc) {
    return 0;
}

unsigned int DRV_UTRACE_START(unsigned char * pData);
unsigned int DRV_UTRACE_TERMINATE(unsigned char * pData);
#endif

#ifdef __cplusplus
}
#endif
#endif
