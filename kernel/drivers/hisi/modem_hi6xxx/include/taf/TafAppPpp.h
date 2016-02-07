/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : TafAppPpp.h
******************************************************************************/

#ifndef __TAF_APP_PPP_H__
#define __TAF_APP_PPP_H__


#include "TafTypeDef.h"


#ifdef  __cplusplus
#if  __cplusplus
extern "C"{
#endif
#endif

#pragma pack(4)

#ifndef TAF_NTOHL
#if (TAF_BYTE_ORDER == TAF_BIG_ENDIAN)
#define TAF_NTOHL(x)    (x)
#define TAF_HTONL(x)    (x)
#define TAF_NTOHS(x)    (x)
#define TAF_HTONS(x)    (x)
#else
#define TAF_NTOHL(x)    ((((x) & 0x000000ff) << 24) | \
             (((x) & 0x0000ff00) <<  8) | \
             (((x) & 0x00ff0000) >>  8) | \
             (((x) & 0xff000000) >> 24))

#define TAF_HTONL(x)    ((((x) & 0x000000ff) << 24) | \
             (((x) & 0x0000ff00) <<  8) | \
             (((x) & 0x00ff0000) >>  8) | \
             (((x) & 0xff000000) >> 24))

#define TAF_NTOHS(x)    ((((x) & 0x00ff) << 8) | \
             (((x) & 0xff00) >> 8))

#define TAF_HTONS(x)    ((((x) & 0x00ff) << 8) | \
             (((x) & 0xff00) >> 8))

#endif  /* _BYTE_ORDER==_LITTLE_ENDIAN */

#endif


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

#endif /* end of TafAppPpp.h */
