/************************************************************************
  Copyright   : 2005-2007, Huawei Tech. Co., Ltd.
  File name   : TafAppPPPoE.h
  Author      : L47619
  Version     : V200R001
  Date        : 2006-05-06
  Description : 该头文件定义了TAF提供给APP所使用的数据结构和函数接口

  History        :
  1. Date        : 2006-05-06
     Author      : L47619
     Modification: Create
  2. Date:2006-08-09
     Author: j60010247
     Modification:根据问题单A32D03479，在PC机上实现时将#pragma pack(0)和#pragma pack()加编译开关
************************************************************************/

#ifndef __TAF_APP_PPPOE_H__
#define __TAF_APP_PPPOE_H__


/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "TafTypeDef.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


#pragma pack(4)


/******************APP-API所需数据类型定义开始****************/
#ifndef __VOS_CHANGE_TO_TAF__
#define __VOS_CHANGE_TO_TAF__
#if 0
typedef signed   char       TAF_INT8;
typedef unsigned char       TAF_UINT8;

typedef signed   short      TAF_INT16;
typedef unsigned short      TAF_UINT16;

typedef signed   long       TAF_INT32;
typedef unsigned long       TAF_UINT32;

typedef char                TAF_CHAR;
typedef unsigned char       TAF_UCHAR;

typedef void                TAF_VOID;
#endif
#endif

/******************APP-API所需数据类型定义结束****************/


/*****************************************************************************
  2 宏定义
*****************************************************************************/

/*******************************************************************************
  3 枚举定义
*******************************************************************************/
typedef enum
{
    TAF_PPPOE_CID_DEFAULT,                             /*使用默认CID*/
    TAF_PPPOE_CID_MANUAL_SELECT,                       /*用户自行选择CID*/

    TAF_PPPOE_CID_BUTT
}TAF_PPPOE_CID_SELECT_MODE_ENUM;          /*选择CID的方式*/

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


/*****************************************************************************
  8 UNION定义
*****************************************************************************/




/*****************************************************************************
  9 函数接口定义
*****************************************************************************/
/*****************************************************************************
 Prototype      : Taf_PPPoESetCid
 Description    : 提供给用户的接口，用于设置PPPoE所使用的CID
 Input          : CidSelectMode      选择CID的方式，TAF_PPPOE_CID_DEFAULT方式下，第二个参数ucCid可填任意值。
                  ucCid              用户所指定的CID，取值[0,11],只对TAF_PPPOE_CID_MANUAL_SELECT方式有效
 Output         :
 Return Value   : TAF_SUCCESS        CID设置成功
                  TAF_FAILURE        CID设置失败
 Calls          :
 Called By      : APP

 History        :
  1.Date        : 2006-05-16
    Author      : L47619
    Modification: Created function
*****************************************************************************/
extern TAF_UINT32 Taf_PPPoESetCid(TAF_PPPOE_CID_SELECT_MODE_ENUM CidSelectMode, TAF_UINT8 ucCid);





#if ((TAF_OS_VER == TAF_WIN32) || (TAF_OS_VER == TAF_NUCLEUS))
#pragma pack()
#else
#pragma pack(0)
#endif

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of TafAppPPPoE.h*/
