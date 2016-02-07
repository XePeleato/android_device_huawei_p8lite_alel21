/************************************************************************
  Copyright    : 2005-2007, Huawei Tech. Co., Ltd.
  File name    : ppp_id.c
  Author       : ---
  Version      : V200R001
  Date         : 2005-11-17
  Description  : 该C文件给出了PPP模块的ppp id管理函数。
  Function List:
        ---pppid_init
        ---ppp_get_id
        ---ppp_free_id
  History      :
  1. Date:2005-11-17
     Author: ---fanzhibin 49086
     Modification:Create
 ************************************************************************/


#include "ppp_public.h"
#include "pppid.h"
/*****************************************************************************
    协议栈打印打点方式下的.C文件宏定义
*****************************************************************************/
/*lint -e767  修改人: z57034; 检视人: g45205 原因简述: 打点日志文件宏ID定义 */
#define    THIS_FILE_ID        PS_FILE_ID_PPPID_C
/*lint +e767  修改人: z57034; 检视人: g45205 */

#if(FEATURE_ON == FEATURE_PPP)
/*所有PPP ID*/
PPP_ID* pgPppId     = VOS_NULL_PTR;


/*****************************************************************************
 Prototype      : pppid_init
 Description    : PPP模块初始化函数，在系统初始化时需要系统分配一个PPP_MAX_ID_NUM+1
                  个字节的空间，并将首地址传给该函数
 Input          : ---ppp_id
 Output         : ---
 Return Value   : ---VOS_VOID
 Calls          : ---
 Called By      : ---

 History        : ---
  1.Date        : 2005-11-17
    Author      : ---
    Modification: Created function
  2. 2006-03-13 MODIFY BY F49086 FOR A32D02473
*****************************************************************************/
VOS_VOID PppIdInit(VOS_VOID)
{
    PPP_ID i;

    for(i = 0;i < PPP_MAX_ID_NUM;i++ )
    {
        pgPppId[i] = ( i + 1 );
    }
    pgPppId[i]  = PPP_ID_TAIL_FLAG;

    return;
}


/*****************************************************************************
 Prototype      : ppp_get_id
 Description    : PPP模块取空闲PPP ID的函数，如果有空闲的未被分配的PPP ID
                  返回该PPP ID，否则返回为0
 Input          : ---ppp_id
 Output         : ---
 Return Value   : ---VOS_UINT16
 Calls          : ---
 Called By      : ---

 History        : ---
  1.Date        : 2005-11-17
    Author      : ---
    Modification: Created function
*****************************************************************************/
PPP_ID PppGetId(VOS_VOID)
{
    PPP_ID return_val = 0;

    if(pgPppId[0] != PPP_ID_TAIL_FLAG)
    {
        return_val  = pgPppId[0];
        pgPppId[0]  = pgPppId[return_val];
        pgPppId[return_val] = PPP_ID_ALLO_fLAG;
    }
    return return_val;
}

/*****************************************************************************
 Prototype      : ppp_free_id
 Description    : PPP模块释放PPP ID的函数，如果该PPP ID是空闲的，直接返回；否则
                  将该PPP ID插入到空闲链中，然后返回
 Input          : ---ppp_id
                  ---id_be_free
 Output         : ---
 Return Value   : ---VOS_UINT16
 Calls          : ---
 Called By      : ---

 History        : ---
  1.Date        : 2005-11-17
    Author      : ---
    Modification: Created function
*****************************************************************************/
VOS_VOID PppFreeId(PPP_ID id_be_free)
{
    PPP_ID top_val;

    if ( (id_be_free > PPP_MAX_ID_NUM)
        || (0 == id_be_free) )
    {
        return;
    }

    if(pgPppId[id_be_free] == PPP_ID_ALLO_fLAG)
    {
        top_val = pgPppId[0];
        pgPppId[id_be_free] = top_val;
        pgPppId[0] = id_be_free;
    }

    return;
}


PPP_ID PppGetAllocedId(VOS_VOID)
{
    PPP_ID i;

    for(i = 0; i <= PPP_MAX_ID_NUM; i++ )
    {
        if ( pgPppId[i] == PPP_ID_ALLO_fLAG)
        {
            return i;
        }
    }

    return PPP_ID_TAIL_FLAG;
}


VOS_UINT32 PppIsIdValid ( PPP_ID usPppId)
{
    if((usPppId < 1)
        ||(usPppId > PPP_MAX_ID_NUM))
    {
        PPP_MNTN_LOG(PS_PID_APP_PPP, 0, PS_PRINT_NORMAL,"PppId out of rage!\r\n");
        return VOS_ERR;
    }

    if ( !PPP_ID_ALLOCED(usPppId) )
    {
        PPP_MNTN_LOG(PS_PID_APP_PPP, 0, PS_PRINT_NORMAL,"PppId not allocated!\r\n");
        return VOS_ERR;
    }

    return VOS_OK;
}

#endif /* #if(FEATURE_ON == FEATURE_PPP) */

