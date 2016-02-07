/******************************************************************************

              Copyright (C), 2001-2011, Huawei Tech. Co., Ltd.

 ******************************************************************************
  File name   : SiTypeDef.h
  Version     : V200R001
  Author      : H59254
  Date        : 2008-10-10
  Description : This file contains general types and macros that are of use
                to all modules.
  History     :
  1.Date         : 2008-10-10
    Author       : H59254
    Modification : Create
******************************************************************************/
#ifndef  SI_TYPE_DEF_H
#define  SI_TYPE_DEF_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/**/
/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/

#pragma pack(4)
#include "v_msg.h"

/*****************************************************************************
  2 宏定义
*****************************************************************************/
/*SI API数据类型*/
typedef int                 SI_INT;
typedef signed   char       SI_INT8;
typedef unsigned char       SI_UINT8;

typedef signed   short      SI_INT16;
typedef unsigned short      SI_UINT16;

typedef signed   int        SI_INT32;
typedef unsigned int        SI_UINT32;

typedef char                SI_CHAR;
typedef unsigned char       SI_UCHAR;

typedef void                SI_VOID;

typedef unsigned int        SI_BOOL;

#define  SI_NULL_PTR                   0L                    /* null pointer */

typedef SI_UINT16 SI_CLIENT_ID_T ;


/*******************************************************************************
  3 枚举定义
*******************************************************************************/
enum
{
    SI_FALSE = 0,
    SI_TRUE  = 1
};

enum SI_CLIENT_TYPE_ENUM
{
    SI_APP_CLIENT,
    SI_AT_CLIENT,
    SI_CLIENT_TYPE_BUTT
};
typedef SI_UINT8 SI_CLIENT_TYPE;


/*****************************************************************************
  4 全局变量声明
*****************************************************************************/


/*****************************************************************************
  5 STRUCT定义
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER
    VOS_UINT32      ulMsgName;
}PS_SI_MSG_STRU;

typedef struct
{
    VOS_MSG_HEADER
    VOS_UINT32      ulMsgName;
    VOS_UINT8       aucData[4];
}SI_HOOKMSG_STRU;

#if ((VOS_OS_VER == VOS_WIN32) || (VOS_OS_VER == VOS_NUCLEUS))
#pragma pack()
#else
#pragma pack(0)
#endif

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* TAF_TYPE_DEF_H */

