/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : InterpeakMntn.c
  版 本 号   : 初稿
  作    者   : caikai
  生成日期   : 2013年7月22日
  最近修改   :
  功能描述   : INTERPEAK MNTN模块实现
  函数列表   :
  修改历史   :
  1.日    期   : 2013年7月22日
    作    者   : caikai
    修改内容   : 创建文件

******************************************************************************/

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif



/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "vos.h"
#include <string.h>
#include "product_config.h"

#if( FEATURE_ON == FEATURE_IMS )
#if ((VOS_OS_VER == VOS_RTOSCK) || ((VOS_OS_VER == VOS_WIN32) && (VOS_PC_TEST == VOS_RTOSCK)))

#else

#if (!defined(__UT_CENTER__))
#include "ipcom_type.h"
#include "ipcom_clib.h"
#include "ipcom_sysctl.h"
#include "ipcom_pkt.h"
#include "ipcom_netif.h"
#include "ipcom_cmd.h"
#include "ipnet_routesock.h"

#endif
#include "InterpeakMntnOut.h"





/*****************************************************************************
    协议栈打印打点方式下的.C文件宏定义
*****************************************************************************/


/*****************************************************************************
  2 全局变量定义
*****************************************************************************/
#define IP_AF_INET_6                          (28)



/*****************************************************************************
  3 函数实现
*****************************************************************************/
/*lint -save -e958 */


/*****************************************************************************
 函 数 名  : IPS_MNTN_CCORE_GetSysInfo
 功能描述  : 获取Interpeak系统信息
 输入参数  : name           -   系统参数
             pucData        -   数据指针
             pulTotalLen    -   可维可测信息总长度
 输出参数  : 无
 返 回 值  : 成功:IPS_MNTN_OK，失败为IPS_MNTN_ERROR
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年8月3日
    作    者   : 冯家麟
    修改内容   : 新生成函数
  2.日    期   : 2013年7月30日
    作    者   : caikai
    修改内容   : VoLTE特性需要将其移植至V9

*****************************************************************************/
IP_PUBLIC int IPS_MNTN_CCORE_GetSysInfo
(
    int                                *alName,
    Ip_u8                              *pucData,
    Ip_u32                             *pulTotalLen
)
{
    Ip_size_t               ulSysDataLen;
    Ip_u32                  ulLen;

    ulLen           =   0;
    ulSysDataLen    =   0;

    /*根据系统参数，获得预取系统信息长度*/
    if (ipcom_sysctl(alName, 6, IP_NULL, &ulSysDataLen, IP_NULL, 0) < 0)
    {
        ipcom_printf("IPS_MNTN_GetSysInfo: ipnet_sysctl() failed: %s"IP_LF, ipcom_strerror(ipcom_errno));
        return IPS_MNTN_CCORE_ERROR;
    }

    /*如果加入该项信息，可维可测信息总长度超预期，则获取系统信息失败*/
    ulLen           =   *pulTotalLen + ulSysDataLen + IPS_MNTN_CCORE_INFO_LEN_OFFSET;
    if(IPS_MNTN_CCORE_INFO_LEN < ulLen)
    {
        ipcom_printf("IPS_MNTN_GetSysInfo: len is over expect."IP_LF);
        return IPS_MNTN_CCORE_ERROR;
    }

    /*填充系统信息长度*/
    ipcom_memcpy(pucData, &ulSysDataLen, IPS_MNTN_CCORE_INFO_LEN_OFFSET);
    pucData                 +=  IPS_MNTN_CCORE_INFO_LEN_OFFSET;

    if (ipcom_sysctl(alName, 6, pucData, &ulSysDataLen, IP_NULL, 0) < 0)
    {
        ipcom_printf("IPS_MNTN_GetSysInfo: ipnet_sysctl() failed: %s"IP_LF, ipcom_strerror(ipcom_errno));
        return IPS_MNTN_CCORE_ERROR;
    }

    /*刷新可维可测信息总长度*/
    (*pulTotalLen)          +=  (ulSysDataLen + IPS_MNTN_CCORE_INFO_LEN_OFFSET);

    return IPS_MNTN_CCORE_OK;
}

/*****************************************************************************
 函 数 名  : IPS_MNTN_CCORE_GetIfconfigInfo
 功能描述  : 获取虚拟网卡的信息
 输入参数  : pucData        -   数据指针
             pulTotalLen    -   总信息长度
 输出参数  : 无
 返 回 值  : 成功:IPS_MNTN_OK，失败为IPS_MNTN_ERROR
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年8月3日
    作    者   : 冯家麟
    修改内容   : 新生成函数
  2.日    期   : 2013年7月30日
    作    者   : caikai
    修改内容   : VoLTE特性需要将其移植至V9

*****************************************************************************/
int IPS_MNTN_CCORE_GetIfconfigInfo
(
    Ip_u8                              *pucData,
    Ip_u32                             *pulTotalLen,
    Ip_u16                              usType
)
{
    int                         alName[6];
    int                         usRslt;
    Ip_u8                      *pucDataCur;
    struct Ipnet_if_msghdr     *pstIfm;
    struct Ipnet_ifa_msghdr    *pstIfa;
    Ip_u16                      usCount;
    struct Ip_sockaddr_in      *addrs[IPNET_RTAX_MAX];
    Ip_size_t                   if_data_len;
    Ip_size_t                   offset;

    /*配置系统参数*/
    alName[0] = IP_CTL_NET;
    alName[1] = IP_AF_ROUTE;
    alName[2] = 0;    /* Always 0 */
    alName[3] = 0;
    alName[4] = IP_NET_RT_IFLIST;
    alName[5] = 0;

    /*填充消息类型*/
    ipcom_memcpy(pucData, &usType, IPS_MNTN_CCORE_INFO_TYPE_LEN);
    pucData                 +=  IPS_MNTN_CCORE_INFO_TYPE_OFFSET;
    (*pulTotalLen)          +=  IPS_MNTN_CCORE_INFO_TYPE_OFFSET;

    /*获取虚拟网卡的信息*/
    usRslt = IPS_MNTN_CCORE_GetSysInfo(alName, pucData, pulTotalLen);

    if(IPS_MNTN_CCORE_ERROR == usRslt)
    {
        ipcom_printf("IPS_MNTN_CCORE_GetIfconfigInfo: IPS_MNTN_CCORE_GetSysInfo failed"IP_LF);
        return usRslt;
    }

    offset          = 0;
    if_data_len     = (*pulTotalLen) - IPS_MNTN_CCORE_INFO_TYPE_OFFSET - IPS_MNTN_CCORE_INFO_LEN_OFFSET;
    pucDataCur      = pucData + IPS_MNTN_CCORE_INFO_LEN_OFFSET;

    /*为了ASN可以完整解出可维可测内容，需要勾出虚拟网卡下配置了几个IP地址*/
    while (if_data_len > (offset + sizeof(struct Ipnet_if_msghdr)))
    {
        pstIfm      = (struct Ipnet_if_msghdr *) (pucDataCur + offset);
        offset      += pstIfm->ifm_msglen;
        usCount     = 0;

        /*越界保护*/
        if(offset > if_data_len)
        {
            break;
        }

        while (offset < if_data_len)
        {
            pstIfa = (struct Ipnet_ifa_msghdr *) (pucDataCur + offset);

            ipnet_cmd_init_addrs(pstIfa + 1, pstIfa->ifam_addrs, (struct Ip_sockaddr **) addrs);

            if (pstIfa->ifam_type != IPNET_RTM_NEWADDR || (addrs[IPNET_RTAX_IFA]->sin_family != IP_AF_INET && addrs[IPNET_RTAX_IFA]->sin_family != IP_AF_INET_6))
            {
                break;
            }

            offset += pstIfa->ifam_msglen;
            usCount++;
        }

        pstIfm->ifm_msglen =   usCount;
    }

    return usRslt;
}

/*****************************************************************************
 函 数 名  : IPS_MNTN_CCORE_GetNetstatRoutes
 功能描述  : 获取路由信息
 输入参数  : pucData        -   数据指针
             pulTotalLen    -   总信息长度
 输出参数  : 无
 返 回 值  : 成功:IPS_MNTN_OK，失败为IPS_MNTN_ERROR
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年8月3日
    作    者   : 冯家麟
    修改内容   : 新生成函数
  2.日    期   : 2013年7月30日
    作    者   : caikai
    修改内容   : VoLTE特性需要将其移植至V9

*****************************************************************************/
int IPS_MNTN_CCORE_GetNetstatRoutes
(
    Ip_u8                              *pucData,
    Ip_u32                             *pulTotalLen,
    Ip_u16                              usType
)
{
    int                     alName[6];
    int                     usRslt;

    /*配置系统参数*/
    alName[0] = IP_CTL_NET;
    alName[1] = IP_AF_ROUTE;
    alName[2] = 0;
    alName[3] = 0;
    alName[4] = IP_NET_RT_DUMP;
    alName[5] = 0;

    /*填充消息类型*/
    ipcom_memcpy(pucData, &usType, IPS_MNTN_CCORE_INFO_TYPE_LEN);
    pucData                 +=  IPS_MNTN_CCORE_INFO_TYPE_OFFSET;
    (*pulTotalLen)          +=  IPS_MNTN_CCORE_INFO_TYPE_OFFSET;

    /*获取路由信息*/
    usRslt = IPS_MNTN_CCORE_GetSysInfo(alName, pucData, pulTotalLen);

    return usRslt;
}

/*****************************************************************************
 函 数 名  : IPS_MNTN_CCORE_GetIpsInfo
 功能描述  : 获取Interpeak协议栈基础信息
 输入参数  : pucData       获取数据指针
             pusLen        获取数据长度
 输出参数  : 无
 返 回 值  : 成功:IPS_MNTN_OK，失败为IPS_MNTN_ERROR
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年8月3日
    作    者   : 冯家麟
    修改内容   : 新生成函数
  2.日    期   : 2013年7月30日
    作    者   : caikai
    修改内容   : VoLTE特性需要将其移植至V9

*****************************************************************************/
int IPS_MNTN_CCORE_GetIpsInfo
(
    unsigned char                      *pucData,
    unsigned int                       *pulLen,
    unsigned short                      usType
)
{
    (*pulLen)   = 0;

    switch(usType)
    {
        case ID_IPS_MNTN_CCORE_IFCONFIG_INFO :
            IPS_MNTN_CCORE_GetIfconfigInfo((Ip_u8 *)pucData, (Ip_u32 *)pulLen, (Ip_u16)usType);
            break;
        case ID_IPS_MNTN_CCORE_NETSTAT_ROUTES_INFO :
            IPS_MNTN_CCORE_GetNetstatRoutes((Ip_u8 *)pucData, (Ip_u32 *)pulLen, (Ip_u16)usType);
            break;
        case ID_IPS_MNTN_CCORE_NETSTAT_ENTRIES_INFO :
            IPS_MNTN_CCORE_GetNetstatEntries((Ip_u8 *)pucData, (Ip_u32 *)pulLen, (Ip_u16)usType);
            break;
        case ID_IPS_MNTN_CCORE_NAT_RULES_INFO :
            IPS_MNTN_CCORE_GetNatRules((Ip_u8 *)pucData, (Ip_u32 *)pulLen, (Ip_u16)usType);
            break;
        case ID_IPS_MNTN_CCORE_NAT_STATS_INFO :
            IPS_MNTN_CCORE_GetNatStats((Ip_u8 *)pucData, (Ip_u32 *)pulLen, (Ip_u16)usType);
            break;
        case ID_IPS_MNTN_CCORE_NAT_PROXIES_INFO:
            IPS_MNTN_CCORE_GetNatProxies((Ip_u8 *)pucData, (Ip_u32 *)pulLen, (Ip_u16)usType);
            break;
        default:
            break;
    }

    return IPS_MNTN_CCORE_OK;
}
/*lint -restore */

#endif
#endif  /*FEATURE_ON == FEATURE_IMS*/

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif




