/******************************************************************************

    Copyright(C)2008,Hisilicon Co. LTD.

 ******************************************************************************
  File Name       : diag_Common.h
  Description     : diag_Common.c header file
  History         :
     1.w00182550       2012-11-20     Draft Enact
     2. 
******************************************************************************/

#ifndef __DIAG_COMMON_H__
#define __DIAG_COMMON_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  1 Include Headfile                            
*****************************************************************************/
#include  "vos.h"  
#include  "diag_buf_ctrl.h"
#include  "msp_diag.h"
#include  "gen_msg.h"
#include  "OmCommon.h"
#include  "msp_nvim.h"
#include <diag_message_id.h>


#if(VOS_OS_VER == VOS_LINUX)
#include  "blist.h" 
#include <linux/slab.h>
#endif
#pragma pack(4)

/*****************************************************************************
  2 macro
*****************************************************************************/
#define DIAG_SOCP_DECODER_PACKET_HEAD_LEN     8

/*DIAG超时时长*/
#define DIAG_CMD_TIMEOUT_TIME_LEN                     (3000)

#define DIAG_CMD_NV_LOAD_TIMEOUT_TIME_LEN             (30000)
#define DIAG_CMD_FILE_OPS_TIME_OUT_LEN                (10000)

/*yuyangyang  nv 接口 begin*/
#define DIAG_NV_IMEI_LEN                             15
/*yuyangyang  nv 接口 end*/

/*****************************************************************************
0.	HSO连接管理类（0x5000-0x50ff ）
1.	寄存器操作类（0x5100-0x51ff ）
2.	数采操作类  （0x5200-0x52ff ）
3.	开关设置类  （0x5300-0x53ff ）
4.	自动化类    （0x5400-0x54ff ）
5.	NV操作类    （0x5500-0x55ff ） 
*****************************************************************************/
#define DIAG_CATEGORY_CONN             (0)
#define DIAG_CATEGORY_PS               (1)
#define DIAG_CATEGORY_BBP              (2)
#define DIAG_CATEGORY_CFG              (3)
#define DIAG_CATEGORY_GTR              (4)
#define DIAG_CATEGORY_NV               (5)
#define DIAG_CATEGORY_FS               (6)

#define MSP_STRU_ID_28_31_CMD_GROUP(ulCmdId)            (ulCmdId >> 28)
#define MSP_STRU_ID_16_23_CMD_TYPE(ulCmdId)             ((ulCmdId&0xffffff) >> 16)
#define MSP_STRU_ID_8_11_CMD_CATEGORY(ulCmdId)          ((ulCmdId&0xfff) >> 8)
#define MSP_STRU_ID_0_15_CMD_CATEGORY(ulCmdId)          (ulCmdId&0xffff)

#define MSP_STRU_ID_GET_28_31_FIELD(ulGroupId)          (ulGroupId << 28)
#define MSP_STRU_ID_GET_16_23_FIELD(ulTypeId)           (ulTypeId<< 16)
#define MSP_STRU_ID_GET_0_15_FIELD(ulCmdId)             (ulCmdId)

#define DIAG_GET_MODULE_ID(id)              (id & 0x00000FFF)
#define DIAG_GET_PRINTF_LEVEL(id)           ((id & 0xF000)>>12)

#if(VOS_OS_VER ==VOS_VXWORKS)
#define MSP_SOCP_HIGH4BIT_SSID (MSP_SOCP_HIGH4BIT_SSID_MCPU << 4)
#else
#define MSP_SOCP_HIGH4BIT_SSID (MSP_SOCP_HIGH4BIT_SSID_ACPU << 4)
#endif

#define DIAG_OFFSET_CMDSN_GET_SOCP_PKT(pstReq) (pstReq + sizeof(DIAG_CMD_FW_TO_AGENT_REQ_STRU)) 

#define DIAG_OFFSET_SOCP_GET_DIAG_HEAD(pstReq) (pstReq + sizeof(MSP_SOCP_HEAD_STRU))  

#define DIAG_OFFSET_SOCP_DIAG_GET_DIAG_DATA(pstReq) (pstReq + sizeof(MSP_SOCP_HEAD_STRU)+sizeof(MSP_DIAG_HEAD_STRU))  

#define DIAG_OFFSET_HEAD_GET_DATA(pstReq) (pstReq + sizeof(MSP_SOCP_HEAD_STRU) + \
                                        sizeof(MSP_DIAG_HEAD_STRU) + sizeof(MSP_DIAG_DATA_REQ_STRU))

#define DIAG_FULL_HEAD_LEN      (sizeof(MSP_SOCP_HEAD_STRU) + sizeof(MSP_DIAG_HEAD_STRU)+ \
                                   sizeof(MSP_DIAG_DATA_CNF_STRU))

#ifdef DIAG_UT
	#define INLINE 
#endif

#define DIAG_GET_PRINT_CFG_SWT(swt)                 (swt>>24)

#define DIAG_GET_CFG_SWT(swt)           ((DIAG_CMD_REQ_LOG_CFG_FILTER_SWT_OPEN == swt)?DIAG_CFG_SWT_OPEN:\
                                         (GU_DIAG_CMD_REQ_LOG_CFG_FILTER_SWT_OPEN == swt)?DIAG_CFG_SWT_OPEN:\
                                         DIAG_CFG_SWT_CLOSE )


#if (VOS_OS_VER == VOS_WIN32)
#define DIAG_MALLOC(ulBytes)         malloc(ulBytes)
#define DIAG_FREE(ptr)               free(ptr)
#elif (VOS_OS_VER == VOS_LINUX)
#define DIAG_MALLOC(ulBytes)         kmalloc(ulBytes, GFP_KERNEL)
#define DIAG_FREE(ptr)               kfree(ptr)
#elif (VOS_OS_VER == VOS_VXWORKS)
#define DIAG_MALLOC(ulBytes)         malloc(ulBytes)
#define DIAG_FREE(ptr)               free(ptr)
#endif

#ifdef DIAG_UT
#define DIAG_MALLOC(ulBytes)         malloc(ulBytes)
#define DIAG_FREE(ptr)               free(ptr)
#endif

/*****************************************************************************
  3 Massage Declare 
*****************************************************************************/


/*****************************************************************************
  4 Enum 
*****************************************************************************/
typedef enum DIAG_TMR_ID
{
    DIAG_TMR_NV_LOAD_CMD_PARAM          = 0x1230,     /*下行10ms周期性定时器*/
    DIAG_TMR_DATA_SAMPLE_TIMEOUT_PARAM,
    DIAG_TMR_LOG_SAMPLE_TIMEOUT_PARAM,  
    DIAG_TMR_FILE_OP_TIMEOUT_PARAM,  
    DIAG_TMR_PARAM_BUTT
}DIAG_TMR_TIMEOUT_PARAM_ENUM;
            

/*****************************************************************************
   5 STRUCT 
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER
    VOS_UINT32              ulMsgId;           /* 消息名 */
    VOS_UINT32              ulLen;             /* 数据长度 */
    VOS_UINT8               pContext[0];       /* 数据其实地址*/
}DIAG_DATA_MSG_STRU;

typedef APP_OM_MSG_STRU DIAG_PS_MSG_STRU;

typedef struct
{
   VOS_UINT32 ulCmdId;
   VOS_UINT8 aucData[0];
}DIAG_CMD_AGENT_TO_FW_CNF_STRU;

typedef DIAG_CMD_AGENT_TO_FW_CNF_STRU DIAG_CMD_FW_TO_AGENT_REQ_STRU;

typedef struct 
{
    VOS_UINT32 ulPrintTxtNum;
    VOS_UINT32 ulAirNum;
    VOS_UINT32 ulVoLTENum;
    VOS_UINT32 ulLayerNum;
    VOS_UINT32 ulUserNum;
    VOS_UINT32 ulEventNum;
    VOS_UINT32 ulIndNum;
} DIAG_LOG_PKT_NUM_ACC_STRU;

#if(VOS_OS_VER == VOS_LINUX)
typedef struct
{
    LIST_S      dataList;   /* 链表指针 */
    VOS_UINT32  port;       /* 数据端口 */
    VOS_UINT8 * dataBuf;    /* 数据缓存 */
    VOS_UINT32  dataLen;    /* 数据长度 */
} DIAG_PORT_DATA_BUF_STRU;
#endif

enum DIAG_CONNECT_CMD_ENUM
{
    DIAG_CONNECT_CMD,       /* 连接命令 */
    DIAG_DISCONNECT_CMD,    /* 断开连接命令 */
    DIAG_CONNECT_CMD_BUTT
};
typedef unsigned int DIAG_CONNECT_CMD_ENUM_U32;

/*****************************************************************************
  6 UNION
*****************************************************************************/


/*****************************************************************************
  7 Extern Global Variable
*****************************************************************************/
extern VOS_UINT32 g_ulDiagCfgInfo;

extern DIAG_LOG_PKT_NUM_ACC_STRU g_DiagLogPktNum;

/*****************************************************************************
  8 Fuction Extern
*****************************************************************************/


/*****************************************************************************
  9 OTHERS
*****************************************************************************/



extern VOS_UINT64 diag_GetFrameTime(VOS_VOID);
extern VOS_UINT32 diag_SendMsg(VOS_UINT32 ulSenderId, VOS_UINT32 ulRecverId, VOS_UINT32 ulMsgId, VOS_UINT8* pDta, VOS_UINT32 dtaSize);
extern VOS_UINT32 diag_SendPsTransMsg(VOS_UINT32 ulRecverId, VOS_UINT8* pDta, VOS_UINT32 dtaSize);

extern VOS_UINT32 diag_AgentCnfFun(VOS_UINT8* pstCmdCnf,VOS_UINT32 ulCmdId,VOS_UINT32 ulDataSize );

extern VOS_UINT32 diag_GetAgentPid(VOS_VOID);

extern VOS_INT32 diag_RegRead(VOS_UINT32 u32RegAddr,ENADDRTYPE enAddrType, VOS_UINT32 *pu32Value);
extern VOS_INT32 diag_RegWrite(VOS_UINT32 u32RegAddr,ENADDRTYPE enAddrType, VOS_UINT32 u32Value);


/*yuyangyang  nv 接口 begin*/
extern VOS_UINT32 diag_GetImei(VOS_CHAR szimei [ 16 ]);
/*yuyangyang  nv 接口 end*/

#if(FEATURE_SOCP_ON_DEMAND == FEATURE_ON)
#if(VOS_OS_VER == VOS_LINUX)
extern DIAG_PORT_DATA_BUF_STRU * diag_PortGetNodeFromDataBuf(VOS_VOID);
extern VOS_VOID diag_PortSendDataToSocp(DIAG_PORT_DATA_BUF_STRU * dataNode);
extern VOS_UINT32 g_diagSocpIsEnable;
extern VOS_VOID diag_AgentConnectCmdProc(DIAG_CONNECT_CMD_ENUM_U32 connCmd);
#endif
#endif

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

#endif /* end of diag_Common.h */
