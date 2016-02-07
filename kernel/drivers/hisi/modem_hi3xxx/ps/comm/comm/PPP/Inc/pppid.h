#ifndef __PPPID_H__
#define __PPPID_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#include "PppInterface.h"
#include "vos.h"


#pragma pack(4)
#if 0
typedef VOS_UINT16 PPP_ID;
#endif

/*某PPP ID被分配出去的标志*/
#define PPP_ID_ALLO_fLAG 0xffff

/*最后一个空闲PPP ID的标志*/
#define PPP_ID_TAIL_FLAG 0

extern PPP_ID*    pgPppId;

#define PPP_ID_ALLOCED(i) (pgPppId[i] == PPP_ID_ALLO_fLAG)


extern VOS_VOID   PppIdInit(VOS_VOID);
extern PPP_ID     PppGetId(VOS_VOID);
extern VOS_VOID   PppFreeId(PPP_ID id_be_free);

extern PPP_ID     PppGetAllocedId(VOS_VOID);
extern VOS_UINT32 PppIsIdValid ( PPP_ID usPppId);
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

#endif
