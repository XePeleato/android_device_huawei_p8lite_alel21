

#ifndef __SCCTX_H__
#define __SCCTX_H__

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "ScFactory.h"
#include "ScComm.h"
#include "ScApSec.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


#pragma pack(4)

/*****************************************************************************
  2 宏定义
*****************************************************************************/


/*****************************************************************************
  3 枚举定义
*****************************************************************************/


/*****************************************************************************
  4 全局变量声明
*****************************************************************************/


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
    VOS_CHAR                           *acSecretFileName[SC_SECRET_FILE_TYPE_BUTT];
    VOS_CHAR                           *acSignFileName[SC_SECRET_FILE_TYPE_BUTT];
}SC_COMM_GLOBAL_STRU;


typedef struct
{
    SC_AUTH_STATUS_ENUM_UINT8               enScFacAuthStatus;
    VOS_UINT8                               aucScFacRandData[SC_RAND_DATA_LEN];
    /* 发起产线鉴权标志 VOS_FALSE: 未发起产线鉴权; VOS_TRUE: 已发起产线鉴权 */
    VOS_UINT8                               ucIdentifyStartFlg;
    VOS_UINT8                               aucReserved[2];         /* 保留字节，用于四字节对齐 */
}SC_FAC_GLOBAL_STRU;

/*****************************************************************************
 结构名    : SC_APSEC_ITEM_STRU
 枚举说明  : 安全存储项在内存中保存的全局变量

  1.日    期   : 2012年8月27日
    作    者   : h59254
    修改内容   : V7R1C50 ISDB安全存储项目新增
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          ulItemLen;
    VOS_CHAR                           *pucContent;
    VOS_UINT8                           aucHash[SC_HASH_LEN];
}SC_APSEC_ITEM_STRU;

/* Added by h59254 for V7R1C50 ISDB Project, 2012/08/28, begin */
/*****************************************************************************
 结构名    : SC_APSEC_GLOBAL_STRU
 枚举说明  : 安全存储项在内存中保存的全局变量

  1.日    期   : 2012年8月27日
    作    者   : h59254
    修改内容   : V7R1C50 ISDB安全存储项目新增
*****************************************************************************/
typedef struct
{
    VOS_UINT8                                               ucSeqID;
    VOS_UINT8                                               aucReserved[2];
    VOS_UINT8                                               ucSecATkeyFlg;
    VOS_UINT8                                               aucSecATkey[SC_SEC_AT_KEY_LEN];
    SC_APSEC_FILE_STATE_INIT_ENUM_UINT32                    ulInitState;
    SC_APSEC_ITEM_STRU                                      astApSecItem[SC_SECURITY_ITEM_NUM];
    VOS_CHAR                                               *pApSecFileA;
    VOS_CHAR                                               *pApSecFileC;
}SC_APSEC_GLOBAL_STRU;
/* Added by h59254 for V7R1C50 ISDB Project, 2012/08/28, end */

typedef struct
{
    SC_COMM_GLOBAL_STRU                 stScCommGlobalVar;
    SC_FAC_GLOBAL_STRU                  stScFacGlobalVar;
    /* Added by h59254 for V7R1C50 ISDB Project, 2012/08/28, begin */
    SC_APSEC_GLOBAL_STRU                stScApSecGlobalVar;
    /* Added by h59254 for V7R1C50 ISDB Project, 2012/08/28, end */
}SC_CONTEXT_STRU;

/*****************************************************************************
  8 UNION定义
*****************************************************************************/


/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/


/*****************************************************************************
  10 函数声明
*****************************************************************************/


extern SC_CONTEXT_STRU* SC_CTX_GetScCtxAddr(VOS_VOID);


extern SC_COMM_GLOBAL_STRU*  SC_CTX_GetCommGlobalVarAddr( VOS_VOID );


extern VOS_VOID  SC_CTX_InitFacGlobalVar( VOS_VOID );


extern SC_AUTH_STATUS_ENUM_UINT8  SC_CTX_GetFacAuthStatus( VOS_VOID );


extern VOS_VOID  SC_CTX_SetFacAuthStatus( SC_AUTH_STATUS_ENUM_UINT8 enScAuthStatus );


VOS_UINT8  SC_CTX_GetIdentifyStartFlg( VOS_VOID );


VOS_VOID  SC_CTX_SetIdentifyStartFlg( VOS_UINT8 ucIdentifyStartFlg );


extern VOS_UINT8*  SC_CTX_GetFacRandDataAddr( VOS_VOID );

/*****************************************************************************
 函 数 名  : SC_CTX_GetApSecInitStateAddr
 功能描述  : 获取AP Security双备份文件初始化状态地址
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 返回双备份文件初始化状态地址
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年08月30日
    作    者   : h59254
    修改内容   : 新生成函数
*****************************************************************************/
extern VOS_UINT32*  SC_CTX_GetApSecInitStateAddr(VOS_VOID);

/*****************************************************************************
 函 数 名  : SC_CTX_GetApSecItemBaseAddr
 功能描述  : 获取AP Security双备份文件内容在内存中的起始地址
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 返回AP Security双备份文件内容在内存中的起始地址
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年08月30日
    作    者   : h59254
    修改内容   : 新生成函数
*****************************************************************************/
extern SC_APSEC_ITEM_STRU*  SC_CTX_GetApSecItemBaseAddr(VOS_VOID);

/*****************************************************************************
 函 数 名  : SC_CTX_GetSecATkeyFlg
 功能描述  : 返回安全AT key的标志位
 输入参数  : VOS_VOID
 输出参数  : 无
 返 回 值  : VOS_UINT8
                VOS_TRUE    : 当前已经设置安全AT key
                VOS_FALSE   : 当前未设置安全AT key
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年9月2日
    作    者   : 李紫剑/00198894
    修改内容   : 新生成函数

*****************************************************************************/
extern VOS_UINT8 SC_CTX_GetSecATkeyFlg( VOS_VOID );

/*****************************************************************************
 函 数 名  : SC_CTX_SetSecATkeyFlg
 功能描述  : 设置安全AT key的标志位
 输入参数  : VOS_UINT8 ucSecATkeyFlg   - 安全AT key的标志位
                 VOS_TRUE    : 当前已经设置安全AT key
                 VOS_FALSE   : 当前未设置安全AT key
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年9月2日
    作    者   : 李紫剑/00198894
    修改内容   : 新生成函数

*****************************************************************************/
extern VOS_VOID SC_CTX_SetSecATkeyFlg( VOS_UINT8 ucSecATkeyFlg );

/*****************************************************************************
 函 数 名  : SC_CTX_GetSecATkeyAddr
 功能描述  : 返回安全AT key的首地址
 输入参数  : VOS_VOID
 输出参数  : 无
 返 回 值  : VOS_UINT8* -- 安全AT key的首地址
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年9月2日
    作    者   : 李紫剑/00198894
    修改内容   : 新生成函数

*****************************************************************************/
extern VOS_UINT8* SC_CTX_GetSecATkeyAddr( VOS_VOID );

/*****************************************************************************
 函 数 名  : SC_CTX_GetApSecSeqID
 功能描述  : 返回SeqID
 输入参数  : VOS_VOID
 输出参数  : 无
 返 回 值  : VOS_UINT8  -- SeqID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年9月2日
    作    者   : 李紫剑/00198894
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT8 SC_CTX_GetApSecSeqID(VOS_VOID);

/*****************************************************************************
 函 数 名  : SC_CTX_SetApSecSeqID
 功能描述  : 设置SeqID
 输入参数  : VOS_UINT8 ucSeqID   - SeqID
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年9月2日
    作    者   : 李紫剑/00198894
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID SC_CTX_SetApSecSeqID(VOS_UINT8 ucSeqID);




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

#endif
