

/*****************************************************************************
  1 头文件包含
**************************************************************************** */
#include "OmSocketPpm.h"
#include "cpm.h"
#include "omrl.h"
#include "NVIM_Interface.h"

#if (VOS_WIN32 == VOS_OS_VER)
#include <winsock2.h>
#include <windows.h>
#endif

#if (VOS_LINUX == VOS_OS_VER)
#include "hisocket.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*lint -e767*/
#define  THIS_FILE_ID        PS_FILE_ID_OM_SOCKET_PPM_C
/*lint +e767*/

/* ****************************************************************************
  2 全局变量定义
**************************************************************************** */
COMM_SOCKET_CTRL_INFO_STRU g_astSockInfo[SOCKET_BUTT] ={{SOCK_NULL, VOS_NULL_PTR, CPM_WIFI_OM_IND_PORT, SOCK_PORT_NUM},
                                                        {SOCK_NULL, VOS_NULL_PTR, CPM_WIFI_OM_CFG_PORT, SOCK_PORT_NUM},
                                                        {SOCK_NULL, VOS_NULL_PTR, CPM_WIFI_AT_PORT,     SOCK_AT_SRC_PORT_NUM    }
                                                        };

/*****************************************************************************
  3 外部引用声明
*****************************************************************************/


/*****************************************************************************
  4 函数实现
*****************************************************************************/

/*****************************************************************************
 函 数 名  :
 功能描述  :
 输入参数  :
 输出参数  :
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年5月31日
    作    者   : XXXXXXXX
    修改内容   : V8R1 OM_Optimize项目新增

*****************************************************************************/
#if ((VOS_OS_VER == VOS_WIN32) || (FEATURE_HISOCKET == FEATURE_ON))

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/

/*保存当前SOCKET的状态*/
VOS_UINT32          g_ulSockState = SOCK_OK;

/* 自旋锁，用来作自处理任务的临界资源保护 */
VOS_SPINLOCK        g_stSockTaskSpinLock;

/*保存当前SOCKETOM的初始化状态*/
VOS_UINT32          g_ulSockOMInitState = VOS_FALSE;

/*保存当前SOCKETAT的初始化状态*/
VOS_UINT32          g_ulSockATInitState = VOS_FALSE;

/*****************************************************************************
 Prototype      : Sock_IsEnable
 Description    : SOCK功能是否支持
 Input          : void
 Output         : void
 Return Value   : void

 History        : ---
    Date        : 2012-05-24
    Author      : g47350
    Modification: Created function
*****************************************************************************/
VOS_BOOL PPM_SockIsEnable(VOS_VOID)
{
#if (VOS_LINUX == VOS_OS_VER)
    OM_CHANNLE_PORT_CFG_STRU    stPortCfg;

    /* 读取OM的物理输出通道 */
    if (NV_OK != NV_Read(en_NV_Item_Om_Port_Type, &stPortCfg, sizeof(OM_CHANNLE_PORT_CFG_STRU)))
    {
        return VOS_FALSE;
    }

    //stPortCfg.enPortNum += CPM_APP_PORT;

    /* 检测参数*/
    if (CPM_OM_PORT_TYPE_WIFI != stPortCfg.enPortNum)
    {
        return VOS_FALSE;
    }
#endif

    return VOS_TRUE;
}
#if 0
/*****************************************************************************
 Prototype      : Sock_ShutdownAll
 Description    : 关闭所有的SOCKET连接
 Input          : VOS_VOID

 Output         : VOS_VOID
 Return Value   : VOS_VOID

 History        : ---
    Date        : 2008-05-3
    Author      : g47350
    Modification: Created function
 *****************************************************************************/
VOS_VOID PPM_SockShutdownAll(VOS_VOID)
{
    VOS_UINT32 ulIndex;

    for (ulIndex = 0; ulIndex < SOCKET_BUTT; ulIndex++)
    {
        if (SOCK_NULL != g_astSockInfo[ulIndex].socket)
        {
            (VOS_VOID)VOS_SmP(g_astSockInfo[ulIndex].SmClose, 0);

            closesocket(g_astSockInfo[ulIndex].socket);

            g_astSockInfo[ulIndex].socket = SOCK_NULL;

            VOS_SmV(g_astSockInfo[ulIndex].SmClose);
        }
    }

    return;
}
#endif
#if 0 /* IND口和CFG口在PC上都走TCP */
/*****************************************************************************
 Prototype      : Sock_UdpInit
 Description    : 初始化IND通道的socket服务器端
 Input          : pstSockInfo  -- socket控制结构体
 Output         : void
 Return Value   : VOS_TRUE :初始化成功
                  VOS_FALSE:初始化失败

 History        : ---
    Date        : 2014-05-29
    Author      : h59254
    Modification: Created function
 *****************************************************************************/
VOS_BOOL PPM_SockUdpInit(COMM_SOCKET_CTRL_INFO_STRU *pstSockInfo)
{
#if (VOS_WIN32 == VOS_OS_VER)
    WORD wVersionRequested;
    WSADATA wsaData;

    wVersionRequested = MAKEWORD(2, 2);
    WSAStartup(wVersionRequested, &wsaData);
#endif

    pstSockInfo->socket = socket(AF_INET, SOCK_DGRAM, 0);

#if (VOS_LINUX == VOS_OS_VER)
    if (VOS_OK > pstSockInfo->socket)
#else
    if (INVALID_SOCKET == pstSockInfo->socket)
#endif
    {
        g_ulSockState = SOCK_UDP_INIT_ERR;

        closesocket(pstSockInfo->socket);

        return VOS_FALSE;
    }

    /* 创建SOCKET保护信号量 */
    if (CPM_WIFI_OM_IND_PORT == pstSockInfo->enPhyPort)
    {
        if(VOS_OK != VOS_SmMCreate("INDSOCK", VOS_SEMA4_PRIOR | VOS_SEMA4_INVERSION_SAFE, &pstSockInfo->SmClose))
        {
            closesocket(pstSockInfo->socket);
            return VOS_FALSE;
        }
    }
    else
    {
        return VOS_FALSE;
    }

    return VOS_TRUE;
}
#endif

/*****************************************************************************
 Prototype      : Sock_TcpInit
 Description    : 初始化Socket服务器端
 Input          : VOS_VOID
 Output         : void
 Return Value   : VOS_TRUE :初始化成功
                  VOS_FALSE:初始化失败

 History        : ---
    Date        : 2008-05-3
    Author      : g47350
    Modification: Created function
 *****************************************************************************/
VOS_BOOL PPM_SockTcpInit(COMM_SOCKET_CTRL_INFO_STRU *pstSockInfo)
{
    struct sockaddr_in  sAddr;
    VOS_INT             lAddLen;

#if (VOS_WIN32 == VOS_OS_VER)
    WORD wVersionRequested;
    WSADATA wsaData;

    wVersionRequested = MAKEWORD(2, 2);
    WSAStartup(wVersionRequested, &wsaData);
#endif

    /* 创建SOCKET保护信号量 */
    if (CPM_WIFI_OM_CFG_PORT == pstSockInfo->enPhyPort)
    {
        if(VOS_OK != VOS_SmMCreate("CFGSOCK", VOS_SEMA4_PRIOR | VOS_SEMA4_INVERSION_SAFE, &pstSockInfo->SmClose))
        {
            closesocket(pstSockInfo->socket);
            return VOS_FALSE;
        }
    }
    else if (CPM_WIFI_AT_PORT == pstSockInfo->enPhyPort)
    {
        if(VOS_OK != VOS_SmMCreate("ATSOCK", VOS_SEMA4_PRIOR | VOS_SEMA4_INVERSION_SAFE, &pstSockInfo->SmClose))
        {
            closesocket(pstSockInfo->socket);
            return VOS_FALSE;
        }
    }
    else
    {
        return VOS_FALSE;
    }

    pstSockInfo->socket = socket(AF_INET, SOCK_STREAM, 0);

#if (VOS_LINUX == VOS_OS_VER)
    if (VOS_OK > pstSockInfo->socket)
#else
    if (INVALID_SOCKET == pstSockInfo->socket)
#endif
    {
        g_ulSockState = SOCK_TCP_INIT_ERR;

        closesocket(pstSockInfo->socket);

        return VOS_FALSE;
    }

    VOS_MemSet(&sAddr, 0, sizeof(sAddr));
    sAddr.sin_family = AF_INET;

    sAddr.sin_addr.s_addr = 0;/*inet_addr(SOCK_WIFI_DEFAULT_ADDR);*/

    /* 监听的端口号 */
    sAddr.sin_port = htons(pstSockInfo->usPort);

    lAddLen = sizeof(struct sockaddr_in);

    /* 将监听Socket绑定到对应的端口上 */
    if (SOCKET_ERROR == bind(pstSockInfo->socket, (struct sockaddr *)&sAddr, lAddLen))
    {
        g_ulSockState = SOCK_BIND_ERR;

        closesocket(pstSockInfo->socket);

        return VOS_FALSE;
    }

    /* 设置服务器端监听的最大客户端数 */
    if (SOCKET_ERROR == listen(pstSockInfo->socket, SOCKET_NUM_MAX))
    {
        g_ulSockState = SOCK_LISTEN_ERR;

        closesocket(pstSockInfo->socket);

        return VOS_FALSE;
    }

    pstSockInfo->socket = accept(pstSockInfo->socket, NULL, 0);

    return VOS_TRUE;
}

/*****************************************************************************
 Prototype      : Sock_ServerProc
 Description    : 服务器线程，用来处理服务器端和客户端的所有请求。
 Input          : lpParameter - Unused.
 Output         : void
 Return Value   : void

 History        : ---
    Date        : 2008-05-3
    Author      : g47350
    Modification: Created function
*****************************************************************************/
VOS_VOID PPM_SockServerProc(COMM_SOCKET_CTRL_INFO_STRU *pstSockInfo)
{
    VOS_INT8                                acRcvBuf[OM_APP_SPLIT_MSG_LEN];
    VOS_INT                                 lRevSize = 0;
    VOS_ULONG                               ulLockLevel;

    for(;;)
    {
        lRevSize = recv(pstSockInfo->socket, acRcvBuf, OM_APP_SPLIT_MSG_LEN, 0);

        VOS_SpinLockIntLock(&g_stSockTaskSpinLock, ulLockLevel);

            /*客户端已经关闭，需要将其清空*/
        #if (VOS_LINUX == VOS_OS_VER)
            if (0 >= lRevSize)
        #else
            if ((SOCKET_ERROR == lRevSize) || (0 == lRevSize))
        #endif
            {
                (VOS_VOID)VOS_SmP(pstSockInfo->SmClose, 0);
                closesocket(pstSockInfo->socket);
                pstSockInfo->socket = SOCK_NULL;
                VOS_SmV(pstSockInfo->SmClose);

                VOS_SpinUnlockIntUnlock(&g_stSockTaskSpinLock, ulLockLevel);

                continue;
            }

        /*将接收到的数据提交给上层处理*/
        CPM_ComRcv(pstSockInfo->enPhyPort, acRcvBuf, lRevSize);

        VOS_SpinUnlockIntUnlock(&g_stSockTaskSpinLock, ulLockLevel);
    }

}

/*****************************************************************************
 Prototype      : Sock_OmServerTask
 Description    : 服务器线程，用来处理OM配置请求
 Input          : lpParameter - Unused.
 Output         : void
 Return Value   : void

 History        : ---
    Date        : 2014-05-29
    Author      : h59254
    Modification: Created function
*****************************************************************************/
VOS_VOID PPM_SockOmServerTask(VOS_VOID)
{
#if 0 /* IND口和CFG口在PC上都走TCP */
    /* 初始化IND口的SOCKET，采用UDP协议 */
    if (VOS_FALSE == PPM_SockUdpInit(g_astSockInfo + SOCKET_OM_IND))
    {
        return;
    }
#endif
    /* 初始化CFG口的SOCKET，采用TCP协议 */
    if (VOS_FALSE == PPM_SockTcpInit(g_astSockInfo + SOCKET_OM_CFG))
    {
        return;
    }

    g_ulSockState = SOCK_START;

    /* CFG口自处理循环处理入口 */
    PPM_SockServerProc(g_astSockInfo + SOCKET_OM_CFG);

    g_ulSockOMInitState = VOS_TRUE;

    return;
}

/*****************************************************************************
 Prototype      : Sock_AtServerTask
 Description    : 服务器线程，用来处理AT命令请求
 Input          : lpParameter - Unused.
 Output         : void
 Return Value   : void

 History        : ---
    Date        : 2014-05-29
    Author      : h59254
    Modification: Created function
*****************************************************************************/
VOS_VOID PPM_SockAtServerTask(VOS_VOID)
{
    /* 初始化SOCKET */
    if (VOS_FALSE == PPM_SockTcpInit(g_astSockInfo + SOCKET_AT))
    {
        return;
    }

    PPM_SockServerProc(g_astSockInfo + SOCKET_AT);

    g_ulSockATInitState = VOS_TRUE;

    return;
}
#if 0
/*****************************************************************************
 Prototype      : PPM_SockOMIndComSend
 Description    : 提供给上层OM 主动上报数据发送的接口.
 Input          : pucVirAddr:   数据虚地址
                  pucPhyAddr:   数据实地址
                  ulLength:     数据长度
 Output         : void
 Return Value   : VOS_ERR  - 发送失败
                  VOS_OK   - 发送成功

 History        : ---
    Date        : 2014-05-29
    Author      : h59254
    Modification: Created function
 *****************************************************************************/
VOS_INT32 PPM_SockOMIndComSend(VOS_UINT8 *pucVirAddr, VOS_UINT8 *pucPhyAddr, VOS_UINT16 uslength)
{
    SOCKET                              socket;
    VOS_INT                             nSndNum;
    struct sockaddr_in                  stToAddr;

    (VOS_VOID)VOS_SmP(g_astSockInfo[SOCKET_OM_IND].SmClose, 0);

    socket = g_astSockInfo[SOCKET_OM_IND].socket;

    if (SOCK_NULL == socket)
    {
        VOS_SmV(g_astSockInfo[SOCKET_OM_IND].SmClose);

        return VOS_ERR;
    }

    VOS_MemSet(&stToAddr, 0, sizeof(stToAddr));
    stToAddr.sin_family = AF_INET;
    stToAddr.sin_addr.s_addr = inet_addr(SOCK_WIFI_DEFAULT_ADDR);
    /* 目的端口 */
    stToAddr.sin_port = htons(SOCK_OM_IND_DST_PORT_NUM);

    /* 调用send将数据通过socket发送出去，走UDP */
    nSndNum = sendto(socket, pucVirAddr, uslength, 0, &stToAddr, sizeof(stToAddr));

    VOS_SmV(g_astSockInfo[SOCKET_OM_IND].SmClose);

    if (nSndNum != uslength)
    {
        return VOS_ERR;
    }

    return VOS_OK;
}
#endif

/*****************************************************************************
 Prototype      : PPM_SockOMCfgComSend
 Description    : 提供给上层OM CFG响应数据发送的接口.
 Input          : pucVirAddr:   数据虚地址
                  pucPhyAddr:   数据实地址
                  ulLength:     数据长度
 Output         : void
 Return Value   : VOS_ERR  - 发送失败
                  VOS_OK   - 发送成功

 History        : ---
    Date        : 2014-05-29
    Author      : h59254
    Modification: Created function
 *****************************************************************************/
VOS_INT32 PPM_SockOMCfgComSend(VOS_UINT8* pucVirAddr, VOS_UINT8 *pucPhyAddr, VOS_UINT16 uslength)
{
    SOCKET socket;
    VOS_INT nSndNum;

    if (VOS_FALSE == g_ulSockOMInitState)
    {
        return VOS_ERR;
    }

    (VOS_VOID)VOS_SmP(g_astSockInfo[SOCKET_OM_CFG].SmClose, 0);

    socket = g_astSockInfo[SOCKET_OM_CFG].socket;

    if (SOCK_NULL == socket)
    {
        VOS_SmV(g_astSockInfo[SOCKET_OM_CFG].SmClose);

        return VOS_ERR;
    }

    /* 调用send将数据通过socket发送出去，走TCP */
    nSndNum = send(socket, pucVirAddr, uslength, 0);

    VOS_SmV(g_astSockInfo[SOCKET_OM_CFG].SmClose);

    if (nSndNum != uslength)
    {
        return VOS_ERR;
    }

    return VOS_OK;
}

/*****************************************************************************
 Prototype      : PPM_SockATComSend
 Description    : 提供给上层AT发送数据的接口.
 Input          : pucVirAddr:   数据虚地址
                  pucPhyAddr:   数据实地址
                  ulLength:     数据长度
 Output         : void
 Return Value   : VOS_ERR  - 发送失败
                  VOS_OK   - 发送成功

 History        : ---
    Date        : 2014-05-29
    Author      : h59254
    Modification: Created function
 *****************************************************************************/
VOS_INT32 PPM_SockATComSend(VOS_UINT8* pucVirAddr, VOS_UINT8 *pucPhyAddr, VOS_UINT16 uslength)
{
    SOCKET socket;
    VOS_INT nSndNum;

    if (VOS_FALSE == g_ulSockATInitState)
    {
        return VOS_ERR;
    }

    (VOS_VOID)VOS_SmP(g_astSockInfo[SOCKET_AT].SmClose, 0);

    socket = g_astSockInfo[SOCKET_AT].socket;

    if (SOCK_NULL == socket)
    {
        VOS_SmV(g_astSockInfo[SOCKET_AT].SmClose);

        return VOS_ERR;
    }

    /* 调用send将数据通过socket发送出去，走TCP */
    nSndNum = send(socket, pucVirAddr, uslength, 0);

    VOS_SmV(g_astSockInfo[SOCKET_AT].SmClose);

    if (nSndNum != uslength)
    {
        return VOS_ERR;
    }

    return VOS_OK;
}
VOS_UINT32 PPM_SockPortInit(VOS_VOID)
{
#if (VOS_OS_VER != VOS_WIN32)
    if ((BSP_MODULE_SUPPORT == mdrv_misc_support_check(BSP_MODULE_TYPE_WIFI))
        && (VOS_TRUE == PPM_SockIsEnable()))
    {
        /*CPM_PhySendReg(CPM_WIFI_OM_IND_PORT,    (CPM_SEND_FUNC)PPM_SockOMIndComSend);*/
        CPM_PhySendReg(CPM_WIFI_OM_IND_PORT,    (CPM_SEND_FUNC)PPM_SockOMCfgComSend);
        CPM_PhySendReg(CPM_WIFI_OM_CFG_PORT,    (CPM_SEND_FUNC)PPM_SockOMCfgComSend);
        CPM_PhySendReg(CPM_WIFI_AT_PORT,        (CPM_SEND_FUNC)PPM_SockATComSend);
    }
#endif

    VOS_SpinLockInit(&g_stSockTaskSpinLock);

    return VOS_OK;
}

#endif


#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif




