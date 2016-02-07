/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : NetfilterExCtrl.c
  版 本 号   : 初稿
  作    者   : caikai
  生成日期   : 2011年11月22日
  最近修改   :
  功能描述   :
  函数列表   :

  修改历史   :

******************************************************************************/

/******************************************************************************
   1 头文件包含
******************************************************************************/
#include "v_typdef.h"
#include "PsTypeDef.h"
#include "om.h"
#include "OmApp.h"
#include "IpsMntn.h"

#if (FEATURE_ON == FEATURE_NFEXT)

#include "NetfilterEx.h"

/*****************************************************************************
    协议栈打印打点方式下的.C文件宏定义
*****************************************************************************/

#define THIS_FILE_ID PS_FILE_ID_ACPU_NFEX_CTRL_C


/*****************************************************************************
  2 宏定义
*****************************************************************************/

/*****************************************************************************
  3 全局变量声明
*****************************************************************************/
NF_EXT_ENTITY_STRU                  g_stExEntity;
NF_EXT_FLOW_CTRL_ENTITY             g_stExFlowCtrlEntity;

#if(NF_EXT_DBG == DBG_ON)
NF_EXT_STATS_STRU                   g_stNfExtStats = {{0}};
#endif

NF_EXT_NV_STRU                      g_stNfExtNv;

NF_EXT_HOOK_MASK_NV_STRU            g_stExHookMask;

/*****************************************************************************
  4 结构定义
*****************************************************************************/
/* 扩展netfilter开关映射表 */
#if (VOS_OS_VER == VOS_WIN32) || defined (__PC_LINT__)
struct module stModuleTmp;
struct module *THIS_MODULE  = &stModuleTmp;
/*lint -e570 -e64 -e34 -e651*/
NF_EXT_MASK_OPS_STRU g_stNfExtMaskOps[]    =
{
    {
        NF_EXT_BR_PRE_ROUTING_HOOK_ON_MASK,
        {0,0,0,0},
        {
            {0},
            NFExt_BrPreRoutingHook,
            &stModuleTmp,
            NFPROTO_BRIDGE,
            NF_BR_PRE_ROUTING,
            NF_BR_PRI_FILTER_OTHER
        }
    },
    {
        NF_EXT_BR_POST_ROUTING_HOOK_ON_MASK,
        {0,0,0,0},
        {
            {0},
            NFExt_BrPostRoutingHook,
            &stModuleTmp,
            NFPROTO_BRIDGE,
            NF_BR_POST_ROUTING,
            NF_BR_PRI_FILTER_OTHER
        }
    },
    {
        NF_EXT_BR_FORWARD_HOOK_ON_MASK,
        {0,0,0,0},
        {
            {0},
            NFExt_BrForwardHook,
            &stModuleTmp,
            NFPROTO_BRIDGE,
            NF_BR_FORWARD,
            NF_BR_PRI_FILTER_OTHER
        }
    },
    {
        NF_EXT_BR_LOCAL_IN_HOOK_ON_MASK,
        {0,0,0,0},
        {
            {0},
            NFExt_BrLocalInHook,
            &stModuleTmp,
            NFPROTO_BRIDGE,
            NF_BR_LOCAL_IN,
            NF_BR_PRI_FILTER_OTHER
        }
    },
    {
        NF_EXT_BR_LOCAL_OUT_HOOK_ON_MASK,
        {0,0,0,0},
        {
            {0},
            NFExt_BrLocalOutHook,
            &stModuleTmp,
            NFPROTO_BRIDGE,
            NF_BR_LOCAL_OUT,
            NF_BR_PRI_FILTER_OTHER
        }
    },
    {
        NF_EXT_ARP_LOCAL_IN_ON_MASK,
        {0,0,0,0},
        {
            {0},
            NFExt_ArpInHook,
            &stModuleTmp,
            NFPROTO_ARP,
            NF_ARP_IN,
            NF_IP_PRI_CONNTRACK
        }
    },
    {
        NF_EXT_ARP_LOCAL_OUT_ON_MASK,
        {0,0,0,0},
        {
            {0},
            NFExt_ArpOutHook,
            &stModuleTmp,
            NFPROTO_ARP,
            NF_ARP_OUT,
            NF_IP_PRI_CONNTRACK
        }
    },
    {
        NF_EXT_IP4_PRE_ROUTING_HOOK_ON_MASK,
        {0,0,0,0},
        {
            {0},
            NFExt_Ip4PreRoutingHook,
            &stModuleTmp,
            NFPROTO_IPV4,
            NF_INET_PRE_ROUTING,
            NF_IP_PRI_CONNTRACK
        }
    },
    {
        NF_EXT_IP4_POST_ROUTING_HOOK_ON_MASK,
        {0,0,0,0},
        {
            {0},
            NFExt_Ip4PostRoutingHook,
            &stModuleTmp,
            NFPROTO_IPV4,
            NF_INET_POST_ROUTING,
            NF_IP_PRI_CONNTRACK
        }
    },
    {
        NF_EXT_IP4_LOCAL_IN_HOOK_ON_MASK,
        {0,0,0,0},
        {
            {0},
            NFExt_Ip4LocalInHook,
            &stModuleTmp,
            NFPROTO_IPV4,
            NF_INET_LOCAL_IN,
            NF_IP_PRI_CONNTRACK
        }
    },
    {
        NF_EXT_IP4_LOCAL_OUT_HOOK_ON_MASK,
        {0,0,0,0},
        {
            {0},
            NFExt_Ip4LocalOutHook,
            &stModuleTmp,
            NFPROTO_IPV4,
            NF_INET_LOCAL_OUT,
            NF_IP_PRI_CONNTRACK
        }
    },
    {
        NF_EXT_IP4_FORWARD_HOOK_ON_MASK,
        {0,0,0,0},
        {
            {0},
            NFExt_Ip4ForwardHook,
            &stModuleTmp,
            NFPROTO_IPV4,
            NF_INET_FORWARD,
            NF_IP_PRI_CONNTRACK
        }
    },
    {
        NF_EXT_IP6_PRE_ROUTING_HOOK_ON_MASK,
        {0,0,0,0},
        {
            {0},
            NFExt_Ip6PreRoutingHook,
            &stModuleTmp,
            NFPROTO_IPV6,
            NF_INET_PRE_ROUTING,
            NF_IP6_PRI_CONNTRACK
        }
    },
    {
        NF_EXT_IP6_POST_ROUTING_HOOK_ON_MASK,
        {0,0,0,0},
        {
            {0},
            NFExt_Ip6PostRoutingHook,
            &stModuleTmp,
            NFPROTO_IPV6,
            NF_INET_POST_ROUTING,
            NF_IP6_PRI_CONNTRACK
        }
    },
    {
        NF_EXT_IP6_LOCAL_IN_HOOK_ON_MASK,
        {0,0,0,0},
        {
            {0},
            NFExt_Ip6LocalInHook,
            &stModuleTmp,
            NFPROTO_IPV6,
            NF_INET_LOCAL_IN,
            NF_IP6_PRI_CONNTRACK
        }
    },
    {
        NF_EXT_IP6_LOCAL_OUT_HOOK_ON_MASK,
        {0,0,0,0},
        {
            {0},
            NFExt_Ip6LocalOutHook,
            &stModuleTmp,
            NFPROTO_IPV6,
            NF_INET_LOCAL_OUT,
            NF_IP6_PRI_CONNTRACK,
        }
    },
    {
        NF_EXT_IP6_FORWARD_HOOK_ON_MASK,
        {0,0,0,0},
        {
            {0},
            NFExt_Ip6ForwardHook,
            &stModuleTmp,
            NFPROTO_IPV6,
            NF_INET_FORWARD,
            NF_IP6_PRI_CONNTRACK,
        }
    },
    {
        NF_EXT_BR_FORWARD_FLOW_CTRL_HOOK_ON_MASK,
        {0,0,0,0},
        {
            {0},
            NFExt_BrForwardFlowCtrlHook,
            &stModuleTmp,
            NFPROTO_BRIDGE,
            NF_BR_FORWARD,
            NF_BR_PRI_FILTER_BRIDGED,
        }
    }
};
/*lint +e570 +e64 +e34 +e651*/
#else
NF_EXT_MASK_OPS_STRU g_stNfExtMaskOps[]    =
{
    {
        NF_EXT_BR_PRE_ROUTING_HOOK_ON_MASK,
        {0,0,0,0},
        {
            .hook       = NFExt_BrPreRoutingHook,
            .owner      = THIS_MODULE,
            .pf         = NFPROTO_BRIDGE,
            .hooknum    = NF_BR_PRE_ROUTING,
            .priority   = NF_BR_PRI_FILTER_OTHER,       /* 网桥hook点的最低优先级 */
        }
    },
    {
        NF_EXT_BR_POST_ROUTING_HOOK_ON_MASK,
        {0,0,0,0},
        {
            .hook       = NFExt_BrPostRoutingHook,
            .owner      = THIS_MODULE,
            .pf         = NFPROTO_BRIDGE,
            .hooknum    = NF_BR_POST_ROUTING,
            .priority   = NF_BR_PRI_FILTER_OTHER,
        }
    },
    {
        NF_EXT_BR_FORWARD_HOOK_ON_MASK,
        {0,0,0,0},
        {
            .hook       = NFExt_BrForwardHook,
            .owner      = THIS_MODULE,
            .pf         = NFPROTO_BRIDGE,
            .hooknum    = NF_BR_FORWARD,
            .priority   = NF_BR_PRI_FILTER_OTHER,
        }
    },
    {
        NF_EXT_BR_LOCAL_IN_HOOK_ON_MASK,
        {0,0,0,0},
        {
            .hook       = NFExt_BrLocalInHook,
            .owner      = THIS_MODULE,
            .pf         = NFPROTO_BRIDGE,
            .hooknum    = NF_BR_LOCAL_IN,
            .priority   = NF_BR_PRI_FILTER_OTHER,
        }
    },
    {
        NF_EXT_BR_LOCAL_OUT_HOOK_ON_MASK,
        {0,0,0,0},
        {
            .hook       = NFExt_BrLocalOutHook,
            .owner      = THIS_MODULE,
            .pf         = NFPROTO_BRIDGE,
            .hooknum    = NF_BR_LOCAL_OUT,
            .priority   = NF_BR_PRI_FILTER_OTHER,
        }
    },
    {
        NF_EXT_ARP_LOCAL_IN_ON_MASK,
        {0,0,0,0},
        {
            .hook       = NFExt_ArpInHook,
            .owner      = THIS_MODULE,
            .pf         = NFPROTO_ARP,
            .hooknum    = NF_ARP_IN,
            .priority   = NF_IP_PRI_CONNTRACK,      /* ARP hook点的优先级 */
        }
    },
    {
        NF_EXT_ARP_LOCAL_OUT_ON_MASK,
        {0,0,0,0},
        {
            .hook       = NFExt_ArpOutHook,
            .owner      = THIS_MODULE,
            .pf         = NFPROTO_ARP,
            .hooknum    = NF_ARP_OUT,
            .priority   = NF_IP_PRI_CONNTRACK,
        }
    },
    {
        NF_EXT_IP4_PRE_ROUTING_HOOK_ON_MASK,
        {0,0,0,0},
        {
            .hook       = NFExt_Ip4PreRoutingHook,
            .owner      = THIS_MODULE,
            .pf         = NFPROTO_IPV4,
            .hooknum    = NF_INET_PRE_ROUTING,
            .priority   = NF_IP_PRI_MANGLE,         /* 高于DNAT hook点的优先级 */
        }
    },
    {
        NF_EXT_IP4_POST_ROUTING_HOOK_ON_MASK,
        {0,0,0,0},
        {
            .hook       = NFExt_Ip4PostRoutingHook,
            .owner      = THIS_MODULE,
            .pf         = NFPROTO_IPV4,
            .hooknum    = NF_INET_POST_ROUTING,
            .priority   = NF_IP_PRI_SELINUX_LAST,   /* 低于SNAT hook点的优先级 */
        }
    },
    {
        NF_EXT_IP4_LOCAL_IN_HOOK_ON_MASK,
        {0,0,0,0},
        {
            .hook       = NFExt_Ip4LocalInHook,
            .owner      = THIS_MODULE,
            .pf         = NFPROTO_IPV4,
            .hooknum    = NF_INET_LOCAL_IN,
            .priority   = NF_IP_PRI_SELINUX_LAST,
        }
    },
    {
        NF_EXT_IP4_LOCAL_OUT_HOOK_ON_MASK,
        {0,0,0,0},
        {
            .hook       = NFExt_Ip4LocalOutHook,
            .owner      = THIS_MODULE,
            .pf         = NFPROTO_IPV4,
            .hooknum    = NF_INET_LOCAL_OUT,
            .priority   = NF_IP_PRI_SELINUX_LAST,
        }
    },
    {
        NF_EXT_IP4_FORWARD_HOOK_ON_MASK,
        {0,0,0,0},
        {
            .hook       = NFExt_Ip4ForwardHook,
            .owner      = THIS_MODULE,
            .pf         = NFPROTO_IPV4,
            .hooknum    = NF_INET_FORWARD,
            .priority   = NF_IP_PRI_SELINUX_LAST,
        }
    },
    {
        NF_EXT_IP6_PRE_ROUTING_HOOK_ON_MASK,
        {0,0,0,0},
        {
            .hook       = NFExt_Ip6PreRoutingHook,
            .owner      = THIS_MODULE,
            .pf         = NFPROTO_IPV6,
            .hooknum    = NF_INET_PRE_ROUTING,
            .priority   = NF_IP_PRI_MANGLE,
        }
    },
    {
        NF_EXT_IP6_POST_ROUTING_HOOK_ON_MASK,
        {0,0,0,0},
        {
            .hook       = NFExt_Ip6PostRoutingHook,
            .owner      = THIS_MODULE,
            .pf         = NFPROTO_IPV6,
            .hooknum    = NF_INET_POST_ROUTING,
            .priority   = NF_IP_PRI_SELINUX_LAST,
        }
    },
    {
        NF_EXT_IP6_LOCAL_IN_HOOK_ON_MASK,
        {0,0,0,0},
        {
            .hook       = NFExt_Ip6LocalInHook,
            .owner      = THIS_MODULE,
            .pf         = NFPROTO_IPV6,
            .hooknum    = NF_INET_LOCAL_IN,
            .priority   = NF_IP_PRI_SELINUX_LAST,
        }
    },
    {
        NF_EXT_IP6_LOCAL_OUT_HOOK_ON_MASK,
        {0,0,0,0},
        {
            .hook       = NFExt_Ip6LocalOutHook,
            .owner      = THIS_MODULE,
            .pf         = NFPROTO_IPV6,
            .hooknum    = NF_INET_LOCAL_OUT,
            .priority   = NF_IP_PRI_SELINUX_LAST,
        }
    },
    {
        NF_EXT_IP6_FORWARD_HOOK_ON_MASK,
        {0,0,0,0},
        {
            .hook       = NFExt_Ip6ForwardHook,
            .owner      = THIS_MODULE,
            .pf         = NFPROTO_IPV6,
            .hooknum    = NF_INET_FORWARD,
            .priority   = NF_IP_PRI_SELINUX_LAST,
        }
    },
    {
        NF_EXT_BR_FORWARD_FLOW_CTRL_HOOK_ON_MASK,
        {0,0,0,0},
        {
            .hook       = NFExt_BrForwardFlowCtrlHook,
            .owner      = THIS_MODULE,
            .pf         = NFPROTO_BRIDGE,
            .hooknum    = NF_BR_FORWARD,
            .priority   = NF_BR_PRI_FILTER_BRIDGED,             /* 与包过滤优先级相同,优先级在这里不能为0 */
        }
    }
};
#endif


/******************************************************************************
   5 函数实现
******************************************************************************/
/*****************************************************************************
 函 数 名  : NFExt_UnregHooks
 功能描述  : 根据需要停止抓包模块的掩码，将抓包的钩子函数解除内核注册
 输入参数  : VOS_UINT32 ulMask    钩子函数掩码
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年11月22日
    作    者   : caikai
    修改内容   : Created
*****************************************************************************/
VOS_VOID  NFExt_UnregHooks(VOS_UINT32 ulMask)
{
    VOS_UINT i;

    for ( i = 0; i < ARRAY_SIZE(g_stNfExtMaskOps); i++ )
    {
        if ( g_stNfExtMaskOps[i].ulHookMask == (ulMask & g_stNfExtMaskOps[i].ulHookMask) )
        {
            /*卸载钩子函数*/
            nf_unregister_hook(&(g_stNfExtMaskOps[i].stNfExtOps));

            /* 重置相应的掩码位 */
            g_stExEntity.ulCurHookOnMask &= ~g_stNfExtMaskOps[i].ulHookMask;
        }
    }

}

/*****************************************************************************
 函 数 名  : NFExt_RegHooks
 功能描述  : 根据需要开启抓包模块的掩码，将抓包的钩子函数注册到内核
 输入参数  : VOS_UINT32 ulMask        钩子函数掩码
 输出参数  : 无
 返 回 值  : VOS_INT
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年11月22日
    作    者   : caikai
    修改内容   : Created
*****************************************************************************/
VOS_INT  NFExt_RegHooks(VOS_UINT32 ulMask)
{
    VOS_INT iRet;
    VOS_UINT i;

    for ( i = 0; i < ARRAY_SIZE(g_stNfExtMaskOps); i++ )
    {

        if ( g_stNfExtMaskOps[i].ulHookMask != (ulMask & g_stNfExtMaskOps[i].ulHookMask) )
        {
            continue;
        }

        /*注册相应的钩子函数*/
        iRet = nf_register_hook(&(g_stNfExtMaskOps[i].stNfExtOps));
        if ( 0 != iRet )
        {
            vos_printf("register_hook error!!\n");

            /*若有一个注册失败则卸载当前所有已经注册上的钩子函数*/
            NFExt_UnregHooks(g_stExEntity.ulCurHookOnMask);
            return iRet;
        }

        g_stExEntity.ulCurHookOnMask |= g_stNfExtMaskOps[i].ulHookMask;
    }

    return 0;
}

/*****************************************************************************
 函 数 名  : NFExt_ReRegHooks
 功能描述  : 重新注册钩子函数
 输入参数  : VOS_UINT32 ulMask    钩子函数掩码
 输出参数  : 无
 返 回 值  : VOS_INT
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年11月22日
    作    者   : caikai
    修改内容   : Created
*****************************************************************************/
VOS_INT  NFExt_ReRegHooks(VOS_UINT32 ulMask)
{
    VOS_INT iRet;

    /*重新注册前先卸载当前所有的钩子函数*/
    if ( 0 != g_stExEntity.ulCurHookOnMask )
    {
        NFExt_UnregHooks(g_stExEntity.ulCurHookOnMask);
    }

    iRet = NFExt_RegHooks(ulMask);

    return iRet;
}

/*****************************************************************************
 函 数 名  : NFExt_ConfigEffective
 功能描述  : 根据配置注册对应的钩子函数
 输入参数  : NF_EXT_TRACE_CONFIG_REQ_STRU *pRcvMsg
 输出参数  : 无
 返 回 值  : PS_BOOL_ENUM_UINT8
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年11月22日
    作    者   : caikai
    修改内容   : Created
*****************************************************************************/
PS_BOOL_ENUM_UINT8 NFExt_ConfigEffective(IPS_MNTN_TRACE_CONFIG_REQ_STRU *pRcvMsg)
{
    VOS_INT             iRet;
    VOS_UINT32          ulMask = 0;

    /* 流控hook，默认挂上 */
    ulMask |= NF_EXT_DEF_FLOW_CTRL_HOOK_ON_MASK;

    if ( (pRcvMsg->stBridgeArpTraceCfg.ulChoice > IPS_MNTN_TRACE_NULL_CHOSEN)
        && (pRcvMsg->stBridgeArpTraceCfg.ulChoice <= IPS_MNTN_TRACE_WHOLE_DATA_LEN_CHOSEN) )
    {
        ulMask |= NF_EXT_DEF_BR_ARP_HOOK_ON_MASK;
    }

    if ( (pRcvMsg->stPreRoutingTraceCfg.ulChoice > IPS_MNTN_TRACE_NULL_CHOSEN)
        && (pRcvMsg->stPreRoutingTraceCfg.ulChoice <= IPS_MNTN_TRACE_WHOLE_DATA_LEN_CHOSEN))
    {
        ulMask |= NF_EXT_DEF_PRE_ROUTING_HOOK_ON_MASK;
    }

    if ( (pRcvMsg->stPostRoutingTraceCfg.ulChoice > IPS_MNTN_TRACE_NULL_CHOSEN)
        && (pRcvMsg->stPostRoutingTraceCfg.ulChoice <= IPS_MNTN_TRACE_WHOLE_DATA_LEN_CHOSEN))
    {
        ulMask |= NF_EXT_DEF_POST_ROUTING_HOOK_ON_MASK;
    }

    iRet = NFExt_ReRegHooks(ulMask);

    if (iRet < 0)
    {
        vos_printf("NFExt_ConfigEffective: NFExt_ReRegHooks failed!\n");
        return PS_FALSE;
    }

    return PS_TRUE;
}
VOS_UINT32  NFExt_Get1stInetIpv4Addr(struct net_device *pstDev)
{
    struct in_device   *pinDev;

    /* 使用Linux内核结构，使用Linux风格 */
    if (NULL == pstDev)
    {
        return 0;
    }

    if (NULL == pstDev->ip_ptr)
    {
        return 0;
    }

    pinDev = (struct in_device*)(pstDev->ip_ptr);

    if (NULL == pinDev->ifa_list)
    {
        return 0;
    }

    return (VOS_UINT32)htonl((VOS_ULONG)pinDev->ifa_list->ifa_address);
}

/*****************************************************************************
 函 数 名  : NFExt_RcvNfExtInfoCfgReq
 功能描述  : 接收到OM配置可维可测信息捕获配置请求
 输入参数  : VOS_VOID *pMsg
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年11月22日
    作    者   : caikai
    修改内容   : Created
*****************************************************************************/
VOS_VOID NFExt_RcvNfExtInfoCfgReq(VOS_VOID *pMsg)
{
    OM_PS_PARAM_REQ_STRU                    *pstOmMsg;
    IPS_MNTN_INFO_CFG_REQ_STRU              *pstNfExtCfgReq;
    IPS_MNTN_INFO_CFG_CNF_STRU               stNfExtCfgCnf;
    IPS_MNTN_RESULT_TYPE_ENUM_UINT16         enResult;

    pstOmMsg                = (OM_PS_PARAM_REQ_STRU *)pMsg;
    pstNfExtCfgReq          = (IPS_MNTN_INFO_CFG_REQ_STRU *)(pstOmMsg->aucData);

    enResult            = IPS_MNTN_RESULT_OK;

    /*================================*/
    /*构建回复消息*/
    /*================================*/

    /* 填充回复OM申请的确认信息 */
    stNfExtCfgCnf.enCommand       = pstNfExtCfgReq->enCommand;
    stNfExtCfgCnf.enRslt          = enResult;

    /* 发送OM透明消息 */
    IPS_MNTN_SndCfgCnf2Om( ID_IPS_OM_MNTN_INFO_CONFIG_CNF,
        sizeof(IPS_MNTN_INFO_CFG_CNF_STRU), &stNfExtCfgCnf );
}
VOS_VOID NFExt_SelfTaskInit(VOS_VOID)
{
    g_stExEntity.pRingBufferId = OM_RingBufferCreate(NF_EXT_RING_BUF_SIZE);

    if ( VOS_NULL_PTR == g_stExEntity.pRingBufferId )
    {
        vos_printf("NFExt_SelfTaskInit : ERROR : Create ring buffer Failed!" );

        return;
    }

    /* 初始化自处理任务的等待队列头 */
    init_waitqueue_head(&g_stExEntity.stWaitHeadTxTask);
    spin_lock_init(&g_stExEntity.stLockTxTask);
}
VOS_INT NFExt_RingBufferPut( OM_RING_ID rngId, VOS_CHAR *buffer, VOS_INT nbytes )
{
    VOS_ULONG   ulFlags = 0UL;
    VOS_INT     iRst;

    iRst = 0;

    spin_lock_irqsave(&g_stExEntity.stLockTxTask, ulFlags);
    if ((VOS_UINT32)OM_RingBufferFreeBytes(g_stExEntity.pRingBufferId) >= sizeof(NF_EXT_DATA_RING_BUF_STRU) )
    {
        iRst = OM_RingBufferPut(rngId, buffer, nbytes);
    }
    spin_unlock_irqrestore(&g_stExEntity.stLockTxTask, ulFlags);

    return iRst;
}


VOS_INT NFExt_RingBufferGet( OM_RING_ID rngId, VOS_CHAR *buffer, VOS_INT maxbytes )
{
    VOS_ULONG   ulFlags = 0UL;
    VOS_INT     iRst;

    iRst = 0;

    spin_lock_irqsave(&g_stExEntity.stLockTxTask, ulFlags);
    if (!OM_RingBufferIsEmpty(rngId))
    {
        iRst = OM_RingBufferGet(rngId, buffer, maxbytes );
    }
    spin_unlock_irqrestore(&g_stExEntity.stLockTxTask, ulFlags);

    return iRst;
}


VOS_VOID NFExt_FlushRingBuffer(OM_RING_ID rngId)
{
    NF_EXT_DATA_RING_BUF_STRU   stData;
    VOS_ULONG                   ulFlags = 0UL;
    VOS_INT                     iRst = 0;

    while (!OM_RingBufferIsEmpty(rngId))
    {
        iRst = NFExt_RingBufferGet(rngId, (VOS_CHAR*)(&stData), sizeof(NF_EXT_DATA_RING_BUF_STRU));
        if (0 != iRst)
        {
            NF_EXT_MEM_FREE(ACPU_PID_NFEXT, stData.pData);
        }
    }

    spin_lock_irqsave(&g_stExEntity.stLockTxTask, ulFlags);
    OM_RingBufferFlush(rngId);
    spin_unlock_irqrestore(&g_stExEntity.stLockTxTask, ulFlags);
}



VOS_UINT32 NFExt_AddDataToRingBuf(NF_EXT_DATA_RING_BUF_STRU *pstData)
{
    VOS_UINT32                  ulRst;
    NF_EXT_DATA_RING_BUF_STRU   stData;

    if ( VOS_NULL_PTR == g_stExEntity.pRingBufferId )
    {
        vos_printf("NFExt_AddDataToRingBuf: Warning : g_stExEntity.pRingBufferId is null!\n");

        return VOS_ERR;
    }

    ulRst = (VOS_UINT32)NFExt_RingBufferPut(g_stExEntity.pRingBufferId, (VOS_CHAR *)pstData, (VOS_INT)(sizeof(NF_EXT_DATA_RING_BUF_STRU)));

    if (sizeof(NF_EXT_DATA_RING_BUF_STRU) == ulRst)
    {
        ulRst = VOS_OK;
    }
    else if (0 == ulRst)
    {
        NF_EXT_STATS_INC(1, NF_EXT_STATS_BUF_FULL_DROP);

        ulRst = VOS_ERR;
    }
    else
    {
        vos_printf("RingBufferPut Fail found ulRst = %u, sizeof=%u \r\n", ulRst, sizeof(NF_EXT_DATA_RING_BUF_STRU));
        NFExt_RingBufferGet(g_stExEntity.pRingBufferId, (VOS_CHAR*)(&stData), (VOS_INT)ulRst);

        NF_EXT_STATS_INC(1, NF_EXT_STATS_PUT_BUF_FAIL);

        ulRst = VOS_ERR;
    }

    wake_up_interruptible(&g_stExEntity.stWaitHeadTxTask);

    return ulRst;
}

VOS_VOID NFExt_CtrlTxMsgTask(VOS_VOID)
{
    NF_EXT_DATA_RING_BUF_STRU   stData;
    VOS_UINT32                  ulRst;

/* 解决UT死循环问题 */
#ifdef __UT_CENTER__
    VOS_UINT32              i;
#endif

/* 解决UT死循环问题 */
#ifndef __UT_CENTER__
    for ( ; ; )
#else
    for (i = 0; i < 1; i++)
#endif
    {
        if (VOS_NULL_PTR == g_stExEntity.pRingBufferId)
        {
            PS_LOG(ACPU_PID_NFEXT, 0, PS_PRINT_WARNING,
                "NFExt_CtrlSendTask : ERROR : pRingBufferId is NULL!" );
            break;
        }

        /*lint -e{522,666} */
        wait_event_interruptible(g_stExEntity.stWaitHeadTxTask, (VOS_TRUE != OM_RingBufferIsEmpty(g_stExEntity.pRingBufferId)));

        ulRst = (VOS_UINT32)NFExt_RingBufferGet(g_stExEntity.pRingBufferId, (VOS_CHAR *)&stData,
            sizeof(NF_EXT_DATA_RING_BUF_STRU));

        if ((0 != ulRst) && (ulRst != sizeof(NF_EXT_DATA_RING_BUF_STRU)))
        {
            PS_LOG(ACPU_PID_NFEXT, 0, PS_PRINT_WARNING,
                "NFExt_CtrlSendTask : ERROR : Get data error from ring buffer!" );

            NF_EXT_STATS_INC(1, NF_EXT_STATS_GET_BUF_FAIL);

            NFExt_FlushRingBuffer(g_stExEntity.pRingBufferId);

            continue;
        }

        if (0 != ulRst)
        {
            OM_AcpuSendData((OM_RSP_PACKET_STRU*)stData.pData, stData.usDataLen);

            NF_EXT_MEM_FREE(ACPU_PID_NFEXT, stData.pData);
        }
    }
}
VOS_UINT32 NFExt_ReadNvCfg(VOS_VOID)
{
    VOS_UINT32                     ulRet;

    /* 读取钩子函数注册点掩码 */
    ulRet = NV_Read (en_NV_Item_NETFILTER_HOOK_MASK, &g_stNfExtNv , sizeof(NF_EXT_NV_STRU));
    if (NV_OK != ulRet)
    {
        vos_printf("NFExt_ReadNvCfg Fail, Read NV FAIL, Error Code %d\n", (VOS_INT32)ulRet);
        return VOS_ERR;
    }

    /* 将NV结构中读出的掩码赋给g_stExHookMask*/
    g_stExHookMask.ulBrArpHookValue         = g_stNfExtNv.ulNetfilterPara1;
    g_stExHookMask.ulInHookValue            = g_stNfExtNv.ulNetfilterPara2;
    g_stExHookMask.ulOutHookValue           = g_stNfExtNv.ulNetfilterPara3;
    g_stExHookMask.ulFlowCtrlHookValue      = g_stNfExtNv.ulNetfilterPara4;

    return VOS_OK;
}


VOS_VOID NFExt_SetDefaultNvCfg(VOS_VOID)
{
    g_stExHookMask.ulBrArpHookValue      = ( NF_EXT_ARP_LOCAL_IN_ON_MASK \
                                           | NF_EXT_ARP_LOCAL_OUT_ON_MASK );

    g_stExHookMask.ulInHookValue        = ( NF_EXT_IP4_PRE_ROUTING_HOOK_ON_MASK \
                                           | NF_EXT_IP6_PRE_ROUTING_HOOK_ON_MASK );

    g_stExHookMask.ulOutHookValue       = ( NF_EXT_IP4_POST_ROUTING_HOOK_ON_MASK \
                                           | NF_EXT_IP6_POST_ROUTING_HOOK_ON_MASK );

    g_stExHookMask.ulFlowCtrlHookValue  = 0;
}

/*****************************************************************************
 函 数 名  : NFExt_EntityInit
 功能描述  : NFExt模块实体全局变量初始化
 输入参数  : void
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年11月22日
    作    者   : caikai
    修改内容   : Created
*****************************************************************************/
VOS_VOID NFExt_EntityInit(VOS_VOID)
{
    g_stExEntity.ulCurHookOnMask    = 0;
    g_stExEntity.ulIsBlkflag        = NF_EXT_FLAG_BLOCKED;
    g_stExEntity.ulIsDeviceOpen     = 0;
    g_stExEntity.ulOmIp             = 0;
}

/*****************************************************************************
 函 数 名  : NFExt_Init
 功能描述  : 模块初始化函数
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年11月22日
    作    者   : caikai
    修改内容   : Created
*****************************************************************************/
VOS_INT NFExt_Init(VOS_VOID)
{
    VOS_UINT32  ulRet;

    ulRet = NFExt_ReadNvCfg();
    if (VOS_ERR == ulRet)
    {
        NFExt_SetDefaultNvCfg();
    }

    /* NFExt模块实体全局变量初始化 */
    NFExt_EntityInit();

    /*自处理任务初始化*/
    NFExt_SelfTaskInit();

    /* 流控信息初始化 */
    NFExt_FlowCtrlInit();

    return 0;
}

/*****************************************************************************
 函 数 名  : NFExt_Uninit
 功能描述  : 模块卸载函数
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年11月22日
    作    者   : caikai
    修改内容   : Created
*****************************************************************************/
void NFExt_Uninit(VOS_VOID)
{
    /*去注册钩子函数*/
    NFExt_UnregHooks(g_stExEntity.ulCurHookOnMask);
}

/*****************************************************************************
                        流控功能
*****************************************************************************/

VOS_VOID NFExt_FlowCtrlInit(VOS_VOID)
{
    if (NFExt_RegHooks(NF_EXT_DEF_FLOW_CTRL_HOOK_ON_MASK))
    {
        vos_printf("NFExt_FlowCtrlInit NFExt_RegHooks fail");

        return;
    }

    g_stExFlowCtrlEntity.ulFlowCtrlMsk = 0;
    g_stExFlowCtrlEntity.pstBrDev      = NULL;

    PS_MEM_SET(g_stExFlowCtrlEntity.aulTxBytesCnt, 0, sizeof(g_stExFlowCtrlEntity.aulTxBytesCnt));
}


/*lint -e550*/
VOS_VOID  NFExt_BrSetFlowCtrl(VOS_VOID)
{
    VOS_ULONG       ulFlags = 0UL;

    local_irq_save(ulFlags);
    g_stExFlowCtrlEntity.ulFlowCtrlMsk |= NF_EXT_BR_FORWARD_FLOW_CTRL_MASK;
    local_irq_restore(ulFlags);

    IPS_MNTN_FlowCtrl(NF_EXT_BR_FORWARD_FLOW_CTRL_MASK, ID_IPS_TRACE_BR_FORWARD_FLOW_CTRL_START);
}
/*lint +e550*/


/*lint -e550*/
VOS_VOID  NFExt_BrStopFlowCtrl(VOS_VOID)
{
    VOS_ULONG       ulFlags = 0UL;

    local_irq_save(ulFlags);
    g_stExFlowCtrlEntity.ulFlowCtrlMsk &= ~NF_EXT_BR_FORWARD_FLOW_CTRL_MASK;
    local_irq_restore(ulFlags);

    IPS_MNTN_FlowCtrl(NF_EXT_BR_FORWARD_FLOW_CTRL_MASK, ID_IPS_TRACE_BR_FORWARD_FLOW_CTRL_STOP);
}
/*lint +e550*/


VOS_UINT32 NFExt_GetBrBytesCnt(VOS_VOID)
{
#if (FEATURE_ON == FEATURE_SKB_EXP)
    struct net_device_stats *dev_stats = NULL;


    /* 在第一次调用本API的时候才会去获取网桥设备，因为协议栈会比网桥设备先启起来 */
    if (NULL == g_stExFlowCtrlEntity.pstBrDev)
    {
        if (VOS_ERR == NFExt_SaveBrDev())
        {
            return 0;
        }
    }

    dev_stats =  dev_get_stats(g_stExFlowCtrlEntity.pstBrDev);

    if (NULL == dev_stats)
    {
        return 0;
    }

    return dev_stats->tx_bytes + dev_stats->rx_bytes + g_stExFlowCtrlEntity.aulTxBytesCnt[NF_EXT_TX_BYTES_CNT_BR];
#else
    return 0;
#endif
}
VOS_UINT32 NFExt_SaveBrDev(VOS_VOID)
{
    struct net_device   *dev;
    dev = dev_get_by_name(&init_net, NF_EXT_BR_NAME);

    if (NULL == dev)
    {
        return VOS_ERR;
    }

    g_stExFlowCtrlEntity.pstBrDev   = dev;

    /* 只获取第一个IPv4地址 */
    g_stExEntity.ulOmIp             = NFExt_Get1stInetIpv4Addr(dev);

    return VOS_OK;
}

#if(NF_EXT_DBG == DBG_ON)
VOS_VOID NFExt_StatsShow(VOS_VOID)
{
    vos_printf("网桥forward流控丢掉的数据量 %ld \n", g_stNfExtStats.aulStats[NF_EXT_STATS_BR_FC_DROP]);
    vos_printf("进入网桥forward hook的数据量 %ld \n", g_stNfExtStats.aulStats[NF_EXT_STATS_BR_FC_ENTER]);
    vos_printf("环形buf满之后导致丢包数量 %ld \n", g_stNfExtStats.aulStats[NF_EXT_STATS_BUF_FULL_DROP]);
    vos_printf("入环形buf失败次数 %ld \n", g_stNfExtStats.aulStats[NF_EXT_STATS_PUT_BUF_FAIL]);
    vos_printf("出环形buf失败次数 %ld \n", g_stNfExtStats.aulStats[NF_EXT_STATS_GET_BUF_FAIL]);
    vos_printf("申请内存失败次数 %ld \n", g_stNfExtStats.aulStats[NF_EXT_STATS_ALLOC_MEM_FAIL]);

    vos_printf("当前环形缓存中未处理的数据量 %ld \n", ((VOS_UINT32)OM_RingBufferNBytes(g_stExEntity.pRingBufferId) / sizeof(NF_EXT_DATA_RING_BUF_STRU)));
    vos_printf("当前的Hook Mask %ld \n", g_stExEntity.ulCurHookOnMask);
    vos_printf("当前的流控状态Mask %ld \n", g_stExFlowCtrlEntity.ulFlowCtrlMsk);
    vos_printf("当前OM WIFI所使用的IP地址 %x \n", g_stExEntity.ulOmIp);
    vos_printf("当前网桥转发字节数 %x \n", g_stExFlowCtrlEntity.aulTxBytesCnt[NF_EXT_TX_BYTES_CNT_BR]);
}


VOS_VOID NFExt_ResetPri(VOS_UINT32 ulHookNode, VOS_INT32 iPri)
{
    VOS_UINT32  ulCurHookMask = 0;

    if (ulHookNode > (ARRAY_SIZE(g_stNfExtMaskOps) - 1))
    {
        return;
    }

    ulCurHookMask = g_stExEntity.ulCurHookOnMask;
    g_stNfExtMaskOps[ulHookNode].stNfExtOps.priority = iPri;

    NFExt_ReRegHooks(ulCurHookMask);

    vos_printf("Current ulHookNode %d pri = %d\n", ulHookNode, iPri);
}
#endif

#endif      /* #if (FEATURE_ON == FEATURE_NFEXT) */

/*****************************************************************************
 函 数 名  : NFExt_RcvOmMsg
 功能描述  : NFExt接收到来自OM模块的消息处理
 输入参数  : VOS_VOID *pMsg
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年11月22日
    作    者   : caikai
    修改内容   : Created
*****************************************************************************/
VOS_VOID NFExt_RcvOmMsg(VOS_VOID *pMsg)
{
    VOS_UINT16          usMsgType;

    usMsgType = *((VOS_UINT16 *)((VOS_UINT8 *)(pMsg) + VOS_MSG_HEAD_LENGTH));

    switch ( usMsgType )
    {
        #if (FEATURE_ON == FEATURE_NFEXT)
        case ID_OM_IPS_ADVANCED_TRACE_CONFIG_REQ:
            IPS_MNTN_TraceAdvancedCfgReq(pMsg);
            break;

        case ID_OM_IPS_MNTN_INFO_CONFIG_REQ:
            NFExt_RcvNfExtInfoCfgReq(pMsg);
            break;
        #endif

        case ID_OM_IPS_MNTN_IP_INFO_CONFIG_REQ:
            IPS_MNTN_IPInfoCfgReq(pMsg);
            break;

        default:
            vos_printf("NFExt_RcvConfig:Receive Unkown Type Message !\n");
            break;
    }
    return;
}

/*****************************************************************************
 函 数 名  : NFExt_MsgProc
 功能描述  : NFExt可维可测控制消息处理函数
 输入参数  : struct MsgCB * pMsg
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年11月22日
    作    者   : caikai
    修改内容   : Created
*****************************************************************************/
VOS_VOID NFExt_MsgProc( struct MsgCB * pMsg )
{
    if ( VOS_NULL_PTR == pMsg )
    {
        vos_printf("NFExt_MsgProc: Message is NULL !" );
        return;
    }

    switch ( pMsg->ulSenderPid )
    {
        case WUEPS_PID_OM:      /* 来自OM的消息处理 */
            NFExt_RcvOmMsg( (void *)pMsg );
            break;

        default:
            break;
    }

    return;
}

/*****************************************************************************
 函 数 名  : NFExt_PidInit
 功能描述  : NFExt_Pid初始化
 输入参数  : ip - 初始化状态
 输出参数  : 无
 返 回 值  : 成功VOS_OK, 失败VOS_ERR
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年11月22日
    作    者   : caikai
    修改内容   : Created
*****************************************************************************/
VOS_UINT32 NFExt_PidInit( enum VOS_INIT_PHASE_DEFINE ip )
{
    switch ( ip )
    {
        case VOS_IP_LOAD_CONFIG:

            break;
        case VOS_IP_FARMALLOC:
        case VOS_IP_INITIAL:
        case VOS_IP_ENROLLMENT:
        case VOS_IP_LOAD_DATA:
        case VOS_IP_FETCH_DATA:
        case VOS_IP_STARTUP:
        case VOS_IP_RIVAL:
        case VOS_IP_KICKOFF:
        case VOS_IP_STANDBY:
        case VOS_IP_BROADCAST_STATE:
        case VOS_IP_RESTART:
            break;
        default:
            break;
    }

    return VOS_OK;
}


/*****************************************************************************
 函 数 名  : NFExt_FidInit
 功能描述  : NFExt_Fid初始化
 输入参数  : ip - 初始化状态
 输出参数  : 无
 返 回 值  : 成功VOS_OK, 失败VOS_ERR
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年11月22日
    作    者   : caikai
    修改内容   : Created
*****************************************************************************/
VOS_UINT32 NFExt_FidInit ( enum VOS_INIT_PHASE_DEFINE ip )
{
    VOS_UINT32                          ulRslt;
    #if (FEATURE_ON == FEATURE_NFEXT)
    VOS_INT                             iRet;
    #endif

    switch ( ip )
    {
        case   VOS_IP_LOAD_CONFIG:

            #if (FEATURE_ON == FEATURE_NFEXT)
            /* 先完成模块初始化 */
            iRet    = NFExt_Init();

            if ( 0 != iRet )
            {
                vos_printf("NFExt_FidInit: NFExt_Init FAIL!\n");
                return PS_FAIL;
            }
            #endif

            /* 可维可测模块注册PID */
            ulRslt = VOS_RegisterPIDInfo(ACPU_PID_NFEXT,
                                (Init_Fun_Type)NFExt_PidInit,
                                (Msg_Fun_Type)NFExt_MsgProc);

            if( VOS_OK != ulRslt )
            {
                vos_printf("reg UEPS_PID_NFEXT VOS_RegisterPIDInfo FAIL!\n");
                return PS_FAIL;
            }

            ulRslt = VOS_RegisterMsgTaskPrio(ACPU_FID_NFEXT, VOS_PRIORITY_M4);
            if( VOS_OK != ulRslt )
            {
                vos_printf("VOS_RegisterTaskPrio Failed!\n");
                return PS_FAIL;
            }

            #if (FEATURE_ON == FEATURE_NFEXT)
            ulRslt = VOS_RegisterSelfTask(ACPU_FID_NFEXT,
                                    (VOS_TASK_ENTRY_TYPE)NFExt_CtrlTxMsgTask,
                                    VOS_PRIORITY_BASE,
                                    NF_TX_MSG_TASK_STACK_SIZE);

            if ( VOS_NULL_BYTE == ulRslt )
            {
                vos_printf("Self Task Regist fail!\n");
            }
            #endif

            break;

        case   VOS_IP_FARMALLOC:
        case   VOS_IP_INITIAL:
        case   VOS_IP_ENROLLMENT:
        case   VOS_IP_LOAD_DATA:
        case   VOS_IP_FETCH_DATA:
        case   VOS_IP_STARTUP:
        case   VOS_IP_RIVAL:
        case   VOS_IP_KICKOFF:
        case   VOS_IP_STANDBY:
        case   VOS_IP_BROADCAST_STATE:
        case   VOS_IP_RESTART:
        case   VOS_IP_BUTT:
            break;

        default:
            break;
    }

    return PS_SUCC;
}


