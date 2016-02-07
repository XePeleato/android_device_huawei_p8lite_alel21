
#ifdef  __cplusplus
  #if  __cplusplus
  extern "C"{
  #endif
#endif

/******************************************************************************
   1 头文件包含
******************************************************************************/
#include "RnicEntity.h"
#include "RnicDebug.h"
#include "RnicCtx.h"
#include "RnicDev.h"

/*****************************************************************************
    协议栈打印打点方式下的.C文件宏定义
*****************************************************************************/

#define THIS_FILE_ID PS_FILE_ID_RNIC_DEV_C

/******************************************************************************
   2 外部函数变量声明
******************************************************************************/

/******************************************************************************
   3 私有定义
******************************************************************************/

/******************************************************************************
   4 全局变量定义
*****************************************************************************/
/* 将网卡的操作进行静态映射 */
#if (VOS_OS_VER == VOS_WIN32)
    static struct net_device_ops rnic_ops;
#else
    static const struct net_device_ops rnic_ops = {
           .ndo_stop                = RNIC_StopNetCard,
           .ndo_open                = RNIC_OpenNetCard,
           .ndo_start_xmit          = RNIC_StartXmit,
           .ndo_set_mac_address     = RNIC_SetMacAddress,
           .ndo_change_mtu          = RNIC_ChangeMtu,
           .ndo_tx_timeout          = RNIC_ProcTxTimeout,
           .ndo_do_ioctl            = RNIC_Ioctrl,
           .ndo_get_stats           = RNIC_GetNetCardStats,
    };
#endif

#if (FEATURE_ON == FEATURE_RMNET_CUSTOM)
#define RNIC_DEV_NAME_PREFIX            "eth_x"
#else
#define RNIC_DEV_NAME_PREFIX            "rmnet"
#endif
    const RNIC_NETCARD_ELEMENT_TAB_STRU           g_astRnicManageTbl[RNIC_NET_ID_MAX_NUM] = {
        {
#if (FEATURE_ON == FEATURE_RMNET_CUSTOM)
        "",
#else
        "0",
#endif
          {{0x58, 0x02, 0x03, 0x04, 0x05, 0x06}, {0x00, 0x11, 0x09, 0x64, 0x01, 0x01}, 0x0008, {0, 0}},
          {{0x58, 0x02, 0x03, 0x04, 0x05, 0x06}, {0x00, 0x11, 0x09, 0x64, 0x01, 0x01}, 0xDD86, {0, 0}},
          MODEM_ID_0, RNIC_RM_NET_ID_0, {0, 0, 0, 0, 0}},

        { "1",
          {{0x58, 0x02, 0x03, 0x04, 0x05, 0x07}, {0x00, 0x11, 0x09, 0x64, 0x01, 0x02}, 0x0008, {0, 0}},
          {{0x58, 0x02, 0x03, 0x04, 0x05, 0x07}, {0x00, 0x11, 0x09, 0x64, 0x01, 0x02}, 0xDD86, {0, 0}},
          MODEM_ID_0, RNIC_RM_NET_ID_1, {0, 0, 0, 0, 0}},

        { "2",
          {{0x58, 0x02, 0x03, 0x04, 0x05, 0x08}, {0x00, 0x11, 0x09, 0x64, 0x01, 0x03}, 0x0008, {0, 0}},
          {{0x58, 0x02, 0x03, 0x04, 0x05, 0x08}, {0x00, 0x11, 0x09, 0x64, 0x01, 0x03}, 0xDD86, {0, 0}},
          MODEM_ID_0, RNIC_RM_NET_ID_2, {0, 0, 0, 0, 0}},
#if (FEATURE_ON == FEATURE_MULTI_MODEM)
        { "3",
          {{0x58, 0x02, 0x03, 0x04, 0x05, 0x09}, {0x00, 0x11, 0x09, 0x64, 0x01, 0x04}, 0x0008, {0, 0}},
          {{0x58, 0x02, 0x03, 0x04, 0x05, 0x09}, {0x00, 0x11, 0x09, 0x64, 0x01, 0x04}, 0xDD86, {0, 0}},
          MODEM_ID_1, RNIC_RM_NET_ID_3, {0, 0, 0, 0, 0}},

        { "4",
          {{0x58, 0x02, 0x03, 0x04, 0x05, 0x0a}, {0x00, 0x11, 0x09, 0x64, 0x01, 0x05}, 0x0008, {0, 0}},
          {{0x58, 0x02, 0x03, 0x04, 0x05, 0x0a}, {0x00, 0x11, 0x09, 0x64, 0x01, 0x05}, 0xDD86, {0, 0}},
          MODEM_ID_1, RNIC_RM_NET_ID_4, {0, 0, 0, 0, 0}},
#endif

        {
          "_ims",
          {{0x58, 0x02, 0x03, 0x04, 0x05, 0x0b}, {0x00, 0x11, 0x09, 0x64, 0x01, 0x06}, 0x0008, {0, 0}},
          {{0x58, 0x02, 0x03, 0x04, 0x05, 0x0b}, {0x00, 0x11, 0x09, 0x64, 0x01, 0x06}, 0xDD86, {0, 0}},
          MODEM_ID_0, RNIC_RM_NET_ID_VT, {0, 0, 0, 0, 0}},

    };

/******************************************************************************
   5 函数实现
******************************************************************************/
/*****************************************************************************
 函 数 名  : RNIC_StopNetCard
 功能描述  : RNIC被关闭时的处理
 输出参数  : pstNetDev:网卡设备指针
 输出参数  : 无
 返 回 值  : VOS_INT:RNIC_OK, RNIC_ERROR
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年12月06日
   作    者   : 范晶
   修改内容   : 新生成函数
*****************************************************************************/
VOS_INT RNIC_StopNetCard(
    struct net_device                  *pstNetDev
)
{
    RNIC_NETCARD_DEV_INFO_STRU         *pstPriv;

    /* 如果网卡为空，则返回错误 */
    if (VOS_NULL_PTR == pstNetDev)
    {
        RNIC_DEV_ERR_PRINTK("RNIC_StopNetCard:net ptr is Null!");
        return RNIC_ERROR;
    }

    /* 获取网卡私有数据 */
    pstPriv = (RNIC_NETCARD_DEV_INFO_STRU *)netdev_priv(pstNetDev);
    if (VOS_NULL_PTR == pstPriv)
    {
        RNIC_DEV_ERR_PRINTK("RNIC_StopNetCard:priv ptr is Null!");
        return RNIC_ERROR;
    }

    /* 使用netif_stop_queue()停止网卡的数据收发 */
    netif_stop_queue(pstNetDev);

    /* 将私有数据中的网卡状态标志置为关闭 */
    pstPriv->enStatus = RNIC_NETCARD_STATUS_CLOSED;

    return RNIC_OK;

}

/*****************************************************************************
 函 数 名  : RNIC_OpenNetCard
 功能描述  : RNIC被打开时的处理
 输入参数  : pstNetDev:网卡设备指针
 输出参数  : 无
 返 回 值  : VOS_INT:RNIC_OK, RNIC_ERR, RNIC_BUSY
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年12月06日
   作    者   : 范晶
   修改内容   : 新生成函数
*****************************************************************************/
VOS_INT RNIC_OpenNetCard(
    struct net_device                  *pstNetDev
)
{
    RNIC_NETCARD_DEV_INFO_STRU         *pstPriv;

    /* 如果网卡为空，则返回错误 */
    if (VOS_NULL_PTR == pstNetDev)
    {
        RNIC_DEV_ERR_PRINTK("RNIC_OpenNetCard:net ptr is Null!");
        return RNIC_ERROR;
    }

    /* 获取网卡私有数据 */
    pstPriv = (RNIC_NETCARD_DEV_INFO_STRU *)netdev_priv(pstNetDev);
    if (VOS_NULL_PTR == pstPriv)
    {
        RNIC_DEV_ERR_PRINTK("RNIC_OpenNetCard:priv ptr is Null!");
        return RNIC_ERROR;
    }

    /* 该设备如果已经打开 */
    if (RNIC_NETCARD_STATUS_OPENED == pstPriv->enStatus)
    {
        RNIC_DEV_ERR_PRINTK("RNIC_OpenNetCard:NetCard is opened");
        /* 返回错误码EBUSY */
        return RNIC_BUSY;
    }

    /* 启动网卡接收数据 */
    netif_start_queue(pstNetDev);

    /* 将私有数据中的网卡状态标志置为打开 */
    pstPriv->enStatus = RNIC_NETCARD_STATUS_OPENED;

    return RNIC_OK;

}


netdev_tx_t RNIC_StartXmit(
    struct sk_buff                     *pstSkb,
    struct net_device                  *pstNetDev
)
{
    RNIC_NETCARD_DEV_INFO_STRU         *pstPriv = VOS_NULL_PTR;
    VOS_UINT8                           ucNetIndex;


    /* 检查数据有效性 */
    if (VOS_NULL == pstSkb)
    {
        /* 告警信息 */
        RNIC_DEV_ERR_PRINTK("RNIC_StartXmit:skb is invalid!");

        return NETDEV_TX_OK;
    }

    /* 检查网卡是否为空 */
    if (VOS_NULL == pstNetDev)
    {
        /* 该处记录的是网卡错误丢包个数,如果网卡错误了是不知道哪个网卡错误的
           目前将这种丢包记录在网卡0上面 */
        RNIC_DBG_NETCAED_UL_DISCARD_NUM(1, RNIC_RM_NET_ID_0);

        /* 释放内存 */
        IMM_ZcFree((IMM_ZC_STRU *)pstSkb);

        /* 告警信息 */
        RNIC_DEV_ERR_PRINTK("RNIC_StartXmit:net ptr is Null!");

        return NETDEV_TX_OK;
    }
    /* 根据网卡设备的私有数据获取对应网卡ID */
    pstPriv    = (RNIC_NETCARD_DEV_INFO_STRU *)netdev_priv(pstNetDev);

    ucNetIndex = pstPriv->enRmNetId;

    if (ucNetIndex >= RNIC_NET_ID_MAX_NUM)
    {
        /* 该处记录的是网卡错误丢包个数,如果网卡错误了是不知道哪个网卡错误的
           目前将这种丢包记录在网卡0上面 */
        RNIC_DBG_NET_ID_UL_DISCARD_NUM(1, RNIC_RM_NET_ID_0);

        /* 释放内存 */
        IMM_ZcFree((IMM_ZC_STRU *)pstSkb);

        /* 告警信息 */
        RNIC_DEV_ERR_PRINTK("RNIC_StartXmit:net index is Null!");

        return NETDEV_TX_OK;
    }
    /* Modified by l60609 for L-C互操作项目, 2014-01-06, Begin */

    /* 打印收到TCP/IP协议栈的数据 */
    /* RNIC 上行数据打印开关，打印收到TCP/IP协议栈的数据 */
    if (VOS_TRUE == g_ulRnicPrintUlDataFlg)
    {
        RNIC_ShowDataFromIpStack(pstSkb);
    }

    skb_pull(pstSkb, RNIC_ETH_HDR_SIZE);

    /* 打印去mac头的数据 */
    /* RNIC 上行数据打印开关，打印去mac头的数据 */
    if (VOS_TRUE == g_ulRnicPrintUlDataFlg)
    {
        RNIC_ShowDataDelMacHead(pstSkb);
    }

    if (RNIC_MODEM_TYPE_INSIDE == RNIC_GET_SPEC_NET_MODEM_TYPE(ucNetIndex))
    {
        return RNIC_RcvInsideModemUlData(pstSkb, pstNetDev, pstPriv, ucNetIndex);
    }
#if (FEATURE_ON == FEATURE_CL_INTERWORK)
    else if (RNIC_MODEM_TYPE_OUTSIDE == RNIC_GET_SPEC_NET_MODEM_TYPE(ucNetIndex))
    {
        return RNIC_RcvOutsideModemUlData(pstSkb, pstNetDev, pstPriv, ucNetIndex);
    }
#endif
    else
    {
        /* 释放内存 */
        IMM_ZcFree((IMM_ZC_STRU *)pstSkb);
        /* 是否添加数据统计 */
        return NETDEV_TX_OK;
    }

    /* Modified by l60609 for L-C互操作项目, 2014-01-06, End */
}

/*****************************************************************************
 函 数 名  : RNIC_SetMacAddress
 功能描述  : RNIC的重新设置MAC地址的处理
 输入参数  : pstNetDev:网卡设备指针
             pMacAddr :MAC地址
 输出参数  : 无
 返 回 值  : VOS_INT:RNIC_OK, RNIC_ADDR_INVALID
 调用函数  :
 被调函数  :

 修改历史     :
 1.日    期   : 2011年12月06日
   作    者   : 范晶
   修改内容   : 新生成函数
*****************************************************************************/
VOS_INT RNIC_SetMacAddress(
    struct net_device                  *pstNetDev,
    void                               *pMacAddr
)
{
    struct sockaddr                    *pstAddr;
    VOS_INT                             lisValid;

    /* 类型转换 */
    pstAddr                             = (struct sockaddr *)pMacAddr;

    /* 网卡指针为空 */
    if (VOS_NULL_PTR == pstNetDev)
    {
        RNIC_DEV_ERR_PRINTK("RNIC_SetMacAddress:net ptr is Null!");

        return RNIC_ERROR;
    }

    /* MAC地址为无效值 */
    lisValid = is_valid_ether_addr((VOS_UINT8 *)pstAddr->sa_data);
    if (VOS_FALSE == lisValid)
    {
        RNIC_DEV_ERR_PRINTK("RNIC_SetMacAddress:Ether addr is invalid!");

        return RNIC_ADDR_INVALID;
    }

    /* 网卡指针的dev_addr为空 */
    if (VOS_NULL_PTR == pstNetDev->dev_addr)
    {
        RNIC_DEV_ERR_PRINTK("RNIC_SetMacAddress:Net dev addr is null!");

        return RNIC_ERROR;
    }

    /* 将MAC地址赋值到网卡 */
    memcpy(pstNetDev->dev_addr, pstAddr->sa_data, pstNetDev->addr_len);

    return RNIC_OK;

}

/*****************************************************************************
 函 数 名  : RNIC_ChangeMtu
 功能描述  : RNIC重新设置MTU大小
 输入参数  : pstNetDev:网卡设备指针
             lNewMtu  :MTU值
 输出参数  : 无
 返 回 值  : VOS_INT:RNIC_OK, RNIC_ERROR, RNIC_OUT_RANGE
 调用函数  :
 被调函数  :

 修改历史     :
 1.日    期   : 2011年12月06日
   作    者   : 范晶
   修改内容   : 新生成函数
*****************************************************************************/
VOS_INT RNIC_ChangeMtu(
    struct net_device                  *pstNetDev,
    VOS_INT                             lNewMtu
)
{
    /* 网卡指针为空 */
    if (VOS_NULL_PTR == pstNetDev)
    {
        RNIC_DEV_ERR_PRINTK("RNIC_ChangeMtu:net ptr is Null!");

        return RNIC_ERROR;
    }

    /* new_mtu长度不在有效范围内 */
    if (lNewMtu > RNIC_MAX_PACKET)
    {
        RNIC_DEV_ERR_PRINTK("RNIC_ChangeMtu:Mtu out of range!");

        return RNIC_OUT_RANGE;
    }

    /* 网卡mtu完成赋值 */
    pstNetDev->mtu = (VOS_UINT)lNewMtu;

    return RNIC_OK;

}

/*****************************************************************************
 函 数 名  : RNIC_Tx_Timeout
 功能描述  : RNIC发送超时处理函数,目前认为不会出现发送超时情况,暂不作任何处理
             留到以后拓展使用
 输入参数  : pstNetDev:网卡设备指针
 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年12月06日
   作    者   : 范晶
   修改内容   : 新生成函数
*****************************************************************************/
VOS_VOID RNIC_ProcTxTimeout(
    struct net_device                  *pstNetDev
)
{
    if (VOS_NULL_PTR == pstNetDev)
    {
        RNIC_DEV_ERR_PRINTK("RNIC_ProcTxTimeout:net ptr is Null!");

        return;
    }

    return;
}

/*****************************************************************************
 函 数 名  : RNIC_Ioctrl
 功能描述  : RNIC的ioctrl处理,目前没有特殊的ioctrl,留作以后扩展
 输入参数  : pstNetDev:网卡设备指针
             pstIfr   :用户请求
             lCmd     :操作命令
 输出参数  : 无
 返 回 值  : 操作不支持  RNIC_NOTSUPP
 调用函数  :
 被调函数  :

 修改历史      :
 1.日    期   : 2011年12月06日
   作    者   : 范晶
   修改内容   : 新生成函数
*****************************************************************************/
VOS_INT RNIC_Ioctrl(
    struct net_device                  *pstNetDev,
    struct ifreq                       *pstIfr,
    VOS_INT                             lCmd
)
{
    return RNIC_NOTSUPP;
}

/*****************************************************************************
 函 数 名  : RNIC_GetNetCardStats
 功能描述  : 获取RNIC网卡统计信息
 输入参数  : pstNetDev:网卡设备指针
 输出参数  : 无
 返 回 值  : net_device_stats
 调用函数  :
 被调函数  :

 修改历史     :
 1.日    期   : 2011年12月06日
   作    者   : 范晶
   修改内容   : 新生成函数
*****************************************************************************/
struct net_device_stats *RNIC_GetNetCardStats(
    struct net_device                  *pstNetDev
)

{
    RNIC_NETCARD_DEV_INFO_STRU         *pstPriv ;

    if (VOS_NULL_PTR == pstNetDev)
    {
        RNIC_DEV_ERR_PRINTK("RNIC_GetNetCardStats:net ptr is Null!");

        return VOS_NULL_PTR;
    }

    /* 获取私有数据 */
    pstPriv = (RNIC_NETCARD_DEV_INFO_STRU *)netdev_priv(pstNetDev);
    if (VOS_NULL_PTR == pstPriv)
    {
        RNIC_DEV_ERR_PRINTK("RNIC_GetNetCardStats:Priv ptr is Null!");

        return VOS_NULL_PTR;
    }

    /* 将设备的统计信息返回 */
    return &pstPriv->stStats;
}


VOS_VOID RNIC_DeinitNetCard(
    VOS_VOID                           *pNetDev
)
{
    struct net_device                  *pstNetDev;
    RNIC_NETCARD_DEV_INFO_STRU         *pstPriv;

    pstNetDev                           = (struct net_device *)pNetDev;

    if (VOS_NULL_PTR == pstNetDev)
    {
        RNIC_DEV_ERR_PRINTK("RNIC_DeinitNetCard:net ptr is Null!");

        return;
    }

    /* 获取网卡私有数据 */
    pstPriv = (RNIC_NETCARD_DEV_INFO_STRU *)netdev_priv(pstNetDev);
    if (pstPriv->enRmNetId >= RNIC_NET_ID_MAX_NUM)
    {
        return;
    }

    /* 关闭载波 */
    netif_carrier_off(pstNetDev);

    /* 停止网卡接收数据 */
    netif_stop_queue(pstNetDev);

    /* 去注册网卡设备 */
    unregister_netdev(pstNetDev);

    /* 释放网卡设备 */
    RNIC_SetSpecNetCardPrivate(VOS_NULL_PTR, pstPriv->enRmNetId);

    free_netdev(pstNetDev);

    return;

}
VOS_INT __init RNIC_InitNetCard(VOS_VOID)
{
    struct net_device                  *pstNetDev   = VOS_NULL_PTR;
    VOS_INT                             lRegRst;
    RNIC_NETCARD_DEV_INFO_STRU         *pstPriv ;
    RNIC_NETCARD_DEV_INFO_STRU         *pstNetCardPrivate ;
    VOS_UINT8                           ucIndex;
    RNIC_CTX_STRU                      *pstRnicCtx;

#if (FEATURE_ON == FEATURE_MULTI_MODEM)
    VOS_UINT8                           aucAddr[RNIC_NET_ID_MAX_NUM][RNIC_MAC_ADDR_LEN] = {
                                                {0x58,0x02,0x03,0x04,0x05,0x06},
                                                {0x58,0x02,0x03,0x04,0x05,0x07},
                                                {0x58,0x02,0x03,0x04,0x05,0x08},
                                                {0x58,0x02,0x03,0x04,0x05,0x09},
                                                {0x58,0x02,0x03,0x04,0x05,0x0a},
                                                {0x58,0x02,0x03,0x04,0x05,0x0b}
                                               };
#else
    VOS_UINT8                           aucAddr[RNIC_NET_ID_MAX_NUM][RNIC_MAC_ADDR_LEN] = {
                                               {0x58,0x02,0x03,0x04,0x05,0x06},
                                               {0x58,0x02,0x03,0x04,0x05,0x07},
                                               {0x58,0x02,0x03,0x04,0x05,0x08},
                                               {0x58,0x02,0x03,0x04,0x05,0x0b}
                                               };
#endif

    printk("RNIC_InitNetCard,entry,%u",VOS_GetSlice());

    pstRnicCtx                          = VOS_NULL_PTR;
    pstRnicCtx = RNIC_GetRnicCtxAddr();
    for (ucIndex = 0 ; ucIndex < RNIC_NET_ID_MAX_NUM ; ucIndex++)
    {
        /* 初始化网卡私有信息 */
        pstRnicCtx->astSpecCtx[ucIndex].pstNetDevInfo    = VOS_NULL_PTR;
    }

    for (ucIndex = 0 ; ucIndex < RNIC_NET_ID_MAX_NUM ; ucIndex++)
    {
        /* 创建网卡设备netdev和私有数据 */
        /* alloc_etherdev这个接口分配了net_device和 RNIC_NETCARD_DEV_INFO_STRU两个接口的内存
           RNIC_NETCARD_DEV_INFO_STRU结构挂接在net_device的后面 */
        pstNetDev = alloc_etherdev(sizeof(RNIC_NETCARD_DEV_INFO_STRU));

        /*创建网卡失败，返回设备指针为NULL */
        if (VOS_NULL_PTR == pstNetDev)
        {
            RNIC_DEV_ERR_PRINTK("RNIC_InitNetCard:net ptr is Null!");
            return RNIC_NOMEM;
        }

        /* 网卡设备地址为空时 */
        if (VOS_NULL_PTR == pstNetDev->dev_addr)
        {
            RNIC_DEV_ERR_PRINTK("RNIC_InitNetCard:net addr ptr is Null!");
            free_netdev(pstNetDev);
            return RNIC_NOMEM;
        }

        /* 给设备配置MAC地址,以后需要填固定值 */
        VOS_MemCpy(pstNetDev->dev_addr, aucAddr[ucIndex], RNIC_MAC_ADDR_LEN);

        /* 设置默认的MTU值 */
        pstNetDev->mtu = RNIC_DEFAULT_MTU;

#if (VOS_OS_VER == VOS_WIN32)
        sprintf(pstNetDev->name, "%s%s",
            RNIC_DEV_NAME_PREFIX, g_astRnicManageTbl[ucIndex].pucRnicNetCardName);
#else
        snprintf(pstNetDev->name, sizeof(pstNetDev->name),
            "%s%s",
            RNIC_DEV_NAME_PREFIX, g_astRnicManageTbl[ucIndex].pucRnicNetCardName);
#endif

        /* 对申请到的net_device结构的设备指针的函数域进行赋值 */
#if (VOS_OS_VER == VOS_WIN32)

#else
        pstNetDev->netdev_ops = &rnic_ops;
#endif

        /* 获取网卡私有数据虚存地址，保存网卡上下文中 */
        pstPriv = (RNIC_NETCARD_DEV_INFO_STRU *)netdev_priv(pstNetDev);
        pstPriv->enRmNetId = ucIndex;
        RNIC_SetSpecNetCardPrivate(pstPriv, ucIndex);

        /* 将网卡虚存地址保存在私有数据中 */
        pstNetCardPrivate = RNIC_GetSpecNetCardPrivateAddr(ucIndex);
        pstNetCardPrivate->pstNetDev = pstNetDev;

        /* 关闭载波 */
        netif_carrier_off(pstNetDev);

        /* 停止网卡接收数据 */
        netif_stop_queue(pstNetDev);

        /* 调用register_netdev函数对Netcard进行注册 */
        lRegRst = register_netdev(pstNetDev);
        if (VOS_OK != lRegRst)
        {
            /*  注册失败释放创建的RNIC设备 */
            free_netdev(pstNetDev);

            RNIC_DEV_ERR_PRINTK("RNIC_InitNetCard:Register netdev failed!");

            return RNIC_STATE;
        }

        /* 打开载波 */
        netif_carrier_on(pstNetDev);

        /* 启动网卡接收数据 */
        netif_start_queue(pstNetDev);
    }

    printk("RNIC_InitNetCard,eixt,%u",VOS_GetSlice());

    /* 返回成功 */
    return RNIC_OK;
}

#if (VOS_LINUX == VOS_OS_VER)
#if (FEATURE_ON == FEATURE_DELAY_MODEM_INIT)
module_init(RNIC_InitNetCard);
#endif
#endif



#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif


