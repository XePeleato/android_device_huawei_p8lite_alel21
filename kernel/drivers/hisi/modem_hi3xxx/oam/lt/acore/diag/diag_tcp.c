/******************************************************************************

   Copyright(C)2008,Hisilicon Co. LTD.

 ******************************************************************************
  File Name       : diag_tcp.c
  Description     :
  History         :
     1.w00182550       2013-2-21   Draft Enact

******************************************************************************/


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  1 Include HeadFile
*****************************************************************************/

#include "vos.h"
#if(VOS_WIN32 == VOS_OS_VER)
#include <winsock2.h>
#include <windows.h>
#else
#if (VOS_VXWORKS == VOS_OS_VER)
#include <sockLib.h>
#include <inetLib.h>
#include <selectLib.h>
#elif (VOS_LINUX == VOS_OS_VER)
/*lint -save -e322 -e7*//*系统头文件找不到，属于环境问题*/
#include "hisocket.h"
/*lint -restore +e322 +e7*/
#endif
#endif
#include <msp_errno.h>
/*lint -save -e322*/
#include "msp_nv_def.h"
/*lint -restore*/
#include "msp_nv_id.h"
#include "diag_debug.h"
#include "SCMProc.h"
#include "diag_port.h"
/*lint --e{409, 718, 746} */

#if (FEATURE_HISOCKET == FEATURE_ON)

/*****************************************************************************
  2 Declare the Global Variable
*****************************************************************************/
typedef struct
{
	VOS_SOCKET	  Tcp;	   // TCP socket ID for the channel;
	VOS_SOCKET	  listener;// TCP socket ID for the channel;
	VOS_UINT32 aulPortNum;
	VOS_SEM       hSem;
	VOS_UINT32 ulConnectSta;
	VOS_UINT32 ulIsReCon;
} DIAG_TCPIP_PORT_STRU;


typedef struct
{
	DIAG_TCPIP_PORT_STRU astTcpIpPort[EN_TCP_PORT_FULL];
} DIAG_TCPIP_CTRL_INFO_STRU;

/*TCP */
#define INVALID_SOCKET  ERROR
#define SOCKET_ERROR    ERROR
#define closesocket(s)  shutdown(s, SHUT_RDWR)

#define IP_NULL         0
#define Ip_sockaddr     sockaddr
#define Ip_socklen_t    int
#define Ip_fd           int
#define Ip_fd_set       hi_fd_set
#define Ip_timeval      timeval

/*#define socket(domain,type,protocol)        hi_socket(domain, type, protocol)*/
#define shutdown(fd,how)                    do{\
													hi_shutdown((Ip_fd)(fd), (how));\
													hi_close((Ip_fd)(fd));\
												}while(0)
#define bind(fd,addr,addrlen)               hi_bind((Ip_fd)(fd), (struct Ip_sockaddr *)(addr), (Ip_socklen_t)(addrlen))
#define accept(fd,addr,addrlenp)            hi_accept((Ip_fd)(fd), (struct Ip_sockaddr *)(addr), (Ip_socklen_t *)(addrlenp))
#define listen(fd,backlog)                  hi_listen((Ip_fd)(fd), (backlog))
#define select(nfds,rf,wf,ef,t)             hi_select((nfds), (Ip_fd_set *)(rf), (Ip_fd_set *)(wf), IP_NULL, (t))
#define recv(fd,buf,len,flags)              hi_recv((Ip_fd)(fd), (void *)(buf), (len), (flags))
#define send(fd,msg,len,flags)              hi_send((Ip_fd)(fd), (void *)(msg), (len), (flags))
#define inet_addr(cp)                       hi_inet_addr(cp)

#if (VOS_WIN32== VOS_OS_VER)
	static DIAG_TCPIP_CTRL_INFO_STRU g_stDiagTcpIpInfo = {
		{{INVALID_SOCKET,INVALID_SOCKET,20248,NULL,0,0},{INVALID_SOCKET,INVALID_SOCKET,20249,0,0,0}}
	};
#else
	static DIAG_TCPIP_CTRL_INFO_STRU g_stDiagTcpIpInfo = {
	{{INVALID_SOCKET,INVALID_SOCKET,20248,0,0,0},
	{INVALID_SOCKET,INVALID_SOCKET,20249,0,0,0}}
    };
#endif
static VOS_UINT8 g_aucRecvBuf[DIAG_PORT_READ_LEN] = {0,};

VOS_UINT32 g_diagTcpPortLock =0,g_diagTcpPortUnLock=0;
VOS_INT32 g_fdCount=0;
VOS_UINT32 g_dmsPrintFlag=0;


/*****************************************************************************
  3 Function
*****************************************************************************/
VOS_UINT32 diag_GetTcpPort(VOS_UINT32 ulPort[EN_TCP_PORT_FULL])
{
    VOS_UINT32 ret = ERR_MSP_SUCCESS;

    ulPort[EN_TCP_PORT_DIAG] = 20248;
    ulPort[EN_TCP_PORT_AT]   = 20249;

    return ret;
}

DIAG_TCPIP_CTRL_INFO_STRU* diag_GetTcpInfo(VOS_VOID)
{
    return &g_stDiagTcpIpInfo;
}


VOS_VOID diag_SocketInfo_Init(VOS_VOID)
{
	//DIAG通道
	g_diagPort[EN_DIAG_SOCKET_BEARER_TCP_20248].ucChanStat         =ACM_EVT_DEV_SUSPEND ;
	g_diagPort[EN_DIAG_SOCKET_BEARER_TCP_20248].ucHdlcFlag         = EN_HDLC_DATA;
	g_diagPort[EN_DIAG_SOCKET_BEARER_TCP_20248].ulCodeDesChanId    = SOCP_CODER_DST_LOM_IND;
	g_diagPort[EN_DIAG_SOCKET_BEARER_TCP_20248].ulDecodeSrcChanId  =SOCP_DECODER_SRC_LOM ;
	g_diagPort[EN_DIAG_SOCKET_BEARER_TCP_20248].ulRecivBufLen      = 0;
    g_diagPort[EN_DIAG_SOCKET_BEARER_TCP_20248].slPortHandle       = UDI_INVALID_HANDLE;
}


VOS_UINT32 diag_SocketInit(VOS_VOID)
{
    VOS_UINT32 ret = ERR_MSP_SUCCESS;
    VOS_UINT8 ulTcpPort = 0;
    /*lint -save -e958*/
    DIAG_TCPIP_CTRL_INFO_STRU *pstDiagTcpInfo;
    /*lint -restore*/
    VOS_UINT32 aulPort[EN_TCP_PORT_FULL] = {0,};

    diag_SocketInfo_Init();

    if(BSP_MODULE_SUPPORT != DRV_GET_WIFI_SUPPORT())
    {
        return ERR_MSP_SUCCESS;
    }

    diag_GetTcpPort(aulPort);

    pstDiagTcpInfo = diag_GetTcpInfo();

    for (ulTcpPort = 0; ulTcpPort < EN_TCP_PORT_FULL; ulTcpPort++)
    {
        pstDiagTcpInfo->astTcpIpPort[ulTcpPort].listener= INVALID_SOCKET;
        pstDiagTcpInfo->astTcpIpPort[ulTcpPort].Tcp = INVALID_SOCKET;

        ret = VOS_SmBCreate(NULL, 1, VOS_SEMA4_FIFO,&pstDiagTcpInfo->astTcpIpPort[ulTcpPort].hSem);
        if ((pstDiagTcpInfo->astTcpIpPort[ulTcpPort].hSem == (VOS_UINT32)NULL)||
            (ret != ERR_MSP_SUCCESS))
        {
            ret = ERR_MSP_FAILURE;
            break;
        }
        pstDiagTcpInfo->astTcpIpPort[ulTcpPort].aulPortNum = aulPort[ulTcpPort];
    }
    DIAG_PORT_INIT_STATE_SWITCH(EN_DIAG_SOCKET_BEARER_TCP_20248,EN_PORT_INIT_SUCC);

    return ret;
}


VOS_VOID diag_TcpPortLock(DIAG_TCPIP_PORT_ENUM enTcpPort)
{
    DIAG_TCPIP_CTRL_INFO_STRU *pstDiagTcpInfo;
    VOS_UINT32 ret;

    pstDiagTcpInfo = diag_GetTcpInfo();

    ret = VOS_SmP(pstDiagTcpInfo->astTcpIpPort[enTcpPort].hSem,0);
    if(ret != ERR_MSP_SUCCESS)
    {
    }
	g_diagTcpPortLock ++;

}

VOS_VOID diag_TcpPortUnLock(DIAG_TCPIP_PORT_ENUM enTcpPort)
{
    DIAG_TCPIP_CTRL_INFO_STRU *pstDiagTcpInfo;
    VOS_UINT32 ret;

    pstDiagTcpInfo = diag_GetTcpInfo();

    ret = VOS_SmV(pstDiagTcpInfo->astTcpIpPort[enTcpPort].hSem);
    if(ret != ERR_MSP_SUCCESS)
    {
    }

	g_diagTcpPortUnLock++;
}


/*lint -save -e101*/
 VOS_INT32 diag_ListenDoneNeedAcceptSet(Ip_fd_set *fdListen, VOS_SOCKET *maxSocket, DIAG_TCPIP_PORT_STRU *astTcpPort)
{
/*lint -restore*/
    VOS_INT32 fd_count = 0;
    VOS_UINT8 ulTcpPort;

    /* initialize socket set to monitor incoming connections */
    if(maxSocket != (VOS_SOCKET *)VOS_NULL)
    {
        *maxSocket = 0;
    }
    else
    {
       return ERR_MSP_INIT_FAILURE;
    }
    if(astTcpPort == VOS_NULL)
    {
        return ERR_MSP_INIT_FAILURE;
    }

    for (ulTcpPort = 0; ulTcpPort < EN_TCP_PORT_FULL; ulTcpPort++)
    {
        if (astTcpPort[ulTcpPort].listener == INVALID_SOCKET)  /* invalid listener */
        {
            continue;
        }

        /* add listener to monitor list*/
        /*lint -save -e718 -e746*/
        DIAG_FD_SET(astTcpPort[ulTcpPort].listener, fdListen);
        fd_count++;
        if (*maxSocket < astTcpPort[ulTcpPort].listener)
        {
            *maxSocket = astTcpPort[ulTcpPort].listener;
        }
        /*lint -restore*/
    }

    return fd_count;
}
/*lint -save -e101*/
static VOS_INT32 diag_AcceptDoneNeedRecvSet(Ip_fd_set *fdReceive, VOS_SOCKET *maxSocket, DIAG_TCPIP_PORT_STRU *astTcpPort)
{
/*lint -restore*/
    VOS_INT32 fd_count = 0;
    VOS_UINT8 ucTcpIpPort;

    /* initialize socket set to monitor incoming traffic*/
    *maxSocket = 0;
    for (ucTcpIpPort = 0; ucTcpIpPort < EN_TCP_PORT_FULL; ucTcpIpPort++)
    {
        if (astTcpPort[ucTcpIpPort].Tcp == INVALID_SOCKET)
        {
            continue;
        }

        /* add socket id to the reading set*/
        DIAG_FD_SET(astTcpPort[ucTcpIpPort].Tcp, fdReceive);
        fd_count++;
        if (*maxSocket < astTcpPort[ucTcpIpPort].Tcp)
        {
            *maxSocket = astTcpPort[ucTcpIpPort].Tcp;
        }
    }

    return fd_count;
}



static VOS_VOID diag_AcceptOnSocket(DIAG_TCPIP_PORT_ENUM ucTcpPort , DIAG_TCPIP_PORT_STRU *astTcpPort)
{
    /*lint -save -e565*/
    struct sockaddr_in from;
    /*lint -restore*/
    VOS_SOCKET newSocket;
    VOS_SOCKET *ListenerSocket;
    VOS_INT32 len;
    DIAG_PORT_CONNECT_STA_PFN pfnConn = DIAG_PORT_GET_CONN_CALLBACK();

    ListenerSocket = &(astTcpPort[ucTcpPort].listener);
    /*lint -save -e84 -e565 -e740*/
    len = sizeof(struct sockaddr_in);
    newSocket = accept(*ListenerSocket, (struct sockaddr *)&from, (int *)&len);
    /*lint -restore*/

    if (INVALID_SOCKET == newSocket)
    {
        closesocket(*ListenerSocket);
        *ListenerSocket = INVALID_SOCKET;
		diag_printf("diag_AcceptOnSocket INVALID_SOCKET == newSocket!\n");
        return;
    }

    /*增加Diag通道的重连处理流程*/
    else
    {
        if(astTcpPort[ucTcpPort].Tcp !=INVALID_SOCKET)
        {
            closesocket(astTcpPort[ucTcpPort].Tcp);
            astTcpPort[ucTcpPort].Tcp = INVALID_SOCKET;
            astTcpPort[ucTcpPort].ulIsReCon= 1;
			diag_printf("diag_AcceptOnSocket astTcpPort[ucTcpPort].ulIsReCon= 1;!\n");
            DIAG_DEBUG_SDM_FUN(EN_DIAG_TCP_TCP_RECONNECT, (VOS_UINT32)newSocket, 0, 0);
        }
    }

    DIAG_DEBUG_SDM_FUN(EN_DIAG_TCP_SOCKET_ACP, (VOS_UINT32)newSocket, 0, 0);

    diag_TcpPortLock(ucTcpPort);
    astTcpPort[ucTcpPort].Tcp = newSocket;
    diag_TcpPortUnLock(ucTcpPort);

#if (VOS_WIN32== VOS_OS_VER)
    /* close listening socket (allow one connection per channel)*/
  if(ucTcpPort == EN_TCP_PORT_AT)
    {
        closesocket(*ListenerSocket);
        *ListenerSocket = INVALID_SOCKET;
    }
#endif

    /* 记录该连接状态*/
	if(ucTcpPort == EN_TCP_PORT_DIAG)
	{

        DIAG_PORT_PORT_SWITCH(EN_DIAG_SOCKET_BEARER_TCP_20248);
        if (pfnConn != NULL)
        {
            pfnConn(DIAG_CONN);
			diag_printf("diag_AcceptOnSocket pfnConn(EN_DMS_CHANNEL_DIAG, DIAG_DISCONN)!\n");
        }
    }

    /* 记录该连接状态*/
    astTcpPort[ucTcpPort].ulConnectSta = DIAG_CONN;
}


VOS_VOID diag_CloseSocket(DIAG_TCPIP_PORT_ENUM enTcpPort)
{
    DIAG_TCPIP_CTRL_INFO_STRU *pstDiagTcpInfo;

    pstDiagTcpInfo = diag_GetTcpInfo();

    diag_TcpPortLock(enTcpPort);

    if (INVALID_SOCKET != pstDiagTcpInfo->astTcpIpPort[enTcpPort].Tcp)
    {
        shutdown(pstDiagTcpInfo->astTcpIpPort[enTcpPort].Tcp, SHUT_RDWR);
        closesocket(pstDiagTcpInfo->astTcpIpPort[enTcpPort].Tcp);

        DIAG_DEBUG_SDM_FUN(EN_DIAG_TCP_SOCKET_CLOSE, (VOS_UINT32)(pstDiagTcpInfo->astTcpIpPort[enTcpPort].Tcp), 0, 0);

        pstDiagTcpInfo->astTcpIpPort[enTcpPort].Tcp = INVALID_SOCKET;
    }

    diag_TcpPortUnLock(enTcpPort);
}




static VOS_VOID diag_TcpRcvData(VOS_SOCKET rxSocket, DIAG_TCPIP_PORT_ENUM enTcpPort,
                         DIAG_PORT_CONNECT_STA_PFN pfnConn)
{
	DIAG_TCPIP_CTRL_INFO_STRU *pstDiagTcpInfo;
    VOS_INT32 len = -1;
    VOS_UINT32 Ret;

	DIAG_DEBUG_SDM_FUN(EN_DIAG_TCP_SOCKET_RECV, (VOS_UINT32)len, 0, 0);

    len = recv((int) rxSocket, (char*)g_aucRecvBuf, (int) DIAG_PORT_READ_LEN, 0);
    if ((SOCKET_ERROR == len) || (0 == len))
    {
    	diag_printf("diag_TcpRcvData  recv error !len=%d \n", len);
        DIAG_DEBUG_SDM_FUN(EN_DIAG_TCP_SOCKET_RECV_ERROR, (VOS_UINT32)len, 0, 0);
        diag_CloseSocket(enTcpPort);


        /* reset channel port connect as disconnect,when recv() error*/
        if (enTcpPort == EN_TCP_PORT_DIAG)
        {
        	DIAG_PORT_PORT_SWITCH(EN_DIAG_SOCKET_BEARER_TCP_20248);
            if (pfnConn != NULL)
            {
            	diag_printf("diag_TcpRcvData pfnConn(EN_DMS_CHANNEL_DIAG, DIAG_DISCONN) \n");
                pfnConn(DIAG_DISCONN);
            }
        }

		pstDiagTcpInfo = diag_GetTcpInfo();
		pstDiagTcpInfo->astTcpIpPort[enTcpPort].ulConnectSta = DIAG_DISCONN;
        return;
    }
    else
    {
        if(g_dmsPrintFlag == 1)
        {
    	    diag_printf("diag_TcpRcvData  recv ok ! len=%d ulTcpPort=%d \n" , len , enTcpPort);
        }
        /* send recv data to cdm decoder*/
        if (enTcpPort == EN_TCP_PORT_DIAG)
        {
           if(g_diagPort[EN_DIAG_SOCKET_BEARER_TCP_20248].ucHdlcFlag ==1)
            {


				Ret = SCM_SendDecoderSrc(g_diagPort[EN_DIAG_USB_BEARER_DIAG_CTRL].ulDecodeSrcChanId, g_aucRecvBuf, (VOS_UINT32)len );
                if(g_dmsPrintFlag == 1)
                {
				    diag_printf("diag_TcpRcvData  111 Ret= %d \n" , Ret);
                }
                return ;
            }
        }

#if (VOS_WIN32== VOS_OS_VER)
        else if (enTcpPort == EN_TCP_PORT_AT)
        {
            if(g_diagPort[EN_DIAG_SOCKET_BEARER_TCP_20249].ucHdlcFlag ==1)
            {
                return ;
            }
			else
			{
			}
        }
#endif
    }
}


VOS_UINT32 diag_TcpSendData(DIAG_TCPIP_PORT_ENUM enTcpPort,VOS_UINT8 *pcData, VOS_UINT32 usDataLen, VOS_UINT32 * pusWrtLen)
{
    VOS_INT32 len = SOCKET_ERROR;
    DIAG_TCPIP_CTRL_INFO_STRU* pstTcpIpInfo = diag_GetTcpInfo();
    DIAG_PORT_CONNECT_STA_PFN pfnConn = DIAG_PORT_GET_CONN_CALLBACK();

	DIAG_DEBUG_SDM_FUN(EN_DIAG_TCP_SOCKET_SEND, (VOS_UINT32)enTcpPort, (VOS_UINT32)len, 0);

    if(pstTcpIpInfo->astTcpIpPort[enTcpPort].Tcp ==INVALID_SOCKET)
    {
    	/*diag_printf("diag_TcpSendData invalid socket!\n");*/
		DIAG_DEBUG_SDM_FUN(EN_DIAG_TCP_SOCKET_SEND_ERROR, (VOS_UINT32)len, 0, 0);
        return ERR_MSP_FAILURE;
    }
    /*lint -save -e84 -e115*/
	DIAG_INVALID_CACHE(pcData , usDataLen);
    /*lint -restore*/
	len = send((int)(pstTcpIpInfo->astTcpIpPort[enTcpPort].Tcp), (VOS_CHAR*)pcData, (int)usDataLen, 0);
    if(g_dmsPrintFlag == 1)
    {
       diag_printf("***********diag_TcpSendData !! usDataLen = %d,enTcpPort=%d,pcData = 0x%p\n" ,usDataLen ,enTcpPort, pcData);
       diag_printf("***********diag_TcpSendData !! len=%d,astTcpIpPort[enTcpPort].Tcp=%d \n" ,len,pstTcpIpInfo->astTcpIpPort[enTcpPort].Tcp);
    }

    if (SOCKET_ERROR == len)
    {
        /*如果是重连之后的第一次发送，可能由于任务调度的原因导致socket获取错误；
 */
        /*此类错误不可以关闭socket；
 */
        if(pstTcpIpInfo->astTcpIpPort[enTcpPort].ulIsReCon==1)
        {
            diag_printf("pstTcpIpInfo->astTcpIpPort[enTcpPort].ulIsReCon==1\n");
            DIAG_DEBUG_SDM_FUN(EN_DIAG_TCP_SOCKET_SEND_ERROR, (VOS_UINT32)len, 0, 1);
            return ERR_MSP_FAILURE;
        }

		/*关闭socket
 */
        diag_CloseSocket(enTcpPort);

		/*置连接状态为断开
 */
        if (enTcpPort == EN_TCP_PORT_DIAG)
        {
        	DIAG_PORT_PORT_SWITCH(EN_DIAG_SOCKET_BEARER_TCP_20248);
            if (pfnConn != NULL)
            {
            	DIAG_DEBUG_SDM_FUN(EN_DIAG_TCP_SOCKET_SEND_ERROR, (VOS_UINT32)len, 0, 2);
                pfnConn(DIAG_DISCONN);
            }
        }

		pstTcpIpInfo = diag_GetTcpInfo();
		pstTcpIpInfo->astTcpIpPort[enTcpPort].ulConnectSta = DIAG_DISCONN;
        return ERR_MSP_FAILURE;
    }

    /*后续的发送失败就会直接关闭socket
 */
    pstTcpIpInfo->astTcpIpPort[enTcpPort].ulIsReCon = 0;

    *pusWrtLen = (VOS_UINT32)len;

    return ERR_MSP_SUCCESS;
}


 VOS_VOID diag_BindListenTcpSocket(VOS_VOID)
{
    struct sockaddr_in local;
    DIAG_TCPIP_CTRL_INFO_STRU *pstDiagTcpInfo;
    VOS_UINT32 ulTcpPort = 0;
	VOS_SOCKET *ListenerSocket=NULL;
	VOS_SOCKET *TcpSocket=NULL;

    pstDiagTcpInfo = diag_GetTcpInfo();
	ListenerSocket = &((pstDiagTcpInfo->astTcpIpPort[ulTcpPort]).listener);
	TcpSocket = &((pstDiagTcpInfo->astTcpIpPort[ulTcpPort]).Tcp);

    /* create listener sockets if necessary*/
    /*diag_printf("BindListenTcpSocket \n");*/
    for (ulTcpPort = 0; ulTcpPort < EN_TCP_PORT_FULL; ulTcpPort++)
    {
        if ((*ListenerSocket != INVALID_SOCKET) ||
            ((*TcpSocket != INVALID_SOCKET)))
        {
        	diag_printf("BindListenTcpSocket 11111\n");
            continue;
        }

        /* create listener socket*/
        *ListenerSocket = hi_socket(AF_INET, SOCK_STREAM, 0);
        if (INVALID_SOCKET == *ListenerSocket)
        {
        	diag_printf("BindListenTcpSocket 2222222\n");
            continue;
        }

        if(g_dmsPrintFlag == 1)
        {
		    diag_printf("BindListenTcpSocket aulPortNum = %d \n" , (VOS_UINT16)(pstDiagTcpInfo->astTcpIpPort[ulTcpPort]).aulPortNum);
        }

        /* initialize socket address */
        /*lint -save -e115*/
        local.sin_family      = AF_INET;
        local.sin_addr.s_addr = htonl(INADDR_ANY);
        local.sin_port        = htons((VOS_UINT16)((pstDiagTcpInfo->astTcpIpPort[ulTcpPort]).aulPortNum));
        /*local.sin_len 		  = sizeof(struct sockaddr_in);*/
        /*lint -restore*/
        /*lint -save -e740 -e84*/
        /* bind listener socket*/
        if (bind(*ListenerSocket, (struct sockaddr*)&local, sizeof(struct sockaddr_in)) ==
            SOCKET_ERROR)
        {
        /*lint -restore*/
        	diag_printf("BindListenTcpSocket 333333333333\n");
            closesocket(*ListenerSocket);
            *ListenerSocket = INVALID_SOCKET;
            continue;
        }

        /* listen on listener socket*/
        if (listen(*ListenerSocket, 1) == SOCKET_ERROR)
        {
        	diag_printf("BindListenTcpSocket 44444444444444\n");
            closesocket(*ListenerSocket);
            *ListenerSocket = INVALID_SOCKET;
            continue;
        }
    }
}
 VOS_VOID diag_AcceptRecvFromSocket(VOS_VOID)
{
    VOS_SOCKET maxAcceptSock;
    VOS_SOCKET maxRecvSock;
    VOS_SOCKET maxSocket;
    VOS_SOCKET rxSocket;
    Ip_fd_set fdRxSet,ReadySet,ErrorSet;
    VOS_INT32 ret;
    DIAG_TCPIP_CTRL_INFO_STRU *pstDiagTcpInfo;
    DIAG_TCPIP_PORT_ENUM enTcpPort = (DIAG_TCPIP_PORT_ENUM)0;
    DIAG_PORT_CONNECT_STA_PFN pfnConn = DIAG_PORT_GET_CONN_CALLBACK();

    pstDiagTcpInfo = diag_GetTcpInfo();

    DIAG_FD_ZERO(&fdRxSet);
	DIAG_FD_ZERO(&ReadySet);
    /*lint -save -e515 -e516*/
    g_fdCount = diag_ListenDoneNeedAcceptSet(&fdRxSet, &maxAcceptSock, pstDiagTcpInfo->astTcpIpPort);
    g_fdCount += diag_AcceptDoneNeedRecvSet(&fdRxSet, &maxRecvSock, pstDiagTcpInfo->astTcpIpPort);
    /*lint -restore*/
    if (0 == g_fdCount)
    {
        /* socket set empty, nothing to be monitored*/
        diag_printf("diag_AcceptRecvFromSocket 0 ==fd_count! \n");
        return;
    }

    /* monitor incoming connection/data for TCP sockets*/
    maxSocket = (maxAcceptSock > maxRecvSock) ? maxAcceptSock : maxRecvSock;

	/*select操作子从fdRxSet中选取已经准备好可读的描述符集合ReadySet*/
    /*lint -save -e419 -e420*/
    memcpy(&ReadySet, &fdRxSet, sizeof(Ip_fd_set));
    memcpy(&ErrorSet, &fdRxSet, sizeof(Ip_fd_set));
    /*lint -restore*/
    /*lint -save -e26*/
    ret = select((int) maxSocket + 1, &ReadySet, NULL, &ErrorSet, NULL);
    /*lint -restore*/
    if (SOCKET_ERROR == ret)
    {
    	diag_printf("diag_AcceptRecvFromSocket 111 \n");
        return;
    }

    /* accept/receive on TCP sockets*/
    for (enTcpPort = (DIAG_TCPIP_PORT_ENUM)0; enTcpPort < EN_TCP_PORT_FULL; enTcpPort++)
    {
        /* check and accept incoming connection*/
        rxSocket = pstDiagTcpInfo->astTcpIpPort[enTcpPort].listener;
        if ((rxSocket != INVALID_SOCKET) && (DIAG_FD_ISSET(rxSocket, &ReadySet)))
        {
            if(g_dmsPrintFlag == 1)
            {
			    diag_printf("diag_AcceptRecvFromSocket acceptOnSocket \n");
            }
	     	diag_AcceptOnSocket(enTcpPort, pstDiagTcpInfo->astTcpIpPort);
            continue;  /* either accept() or recv(), not both*/
        }

        /* check and receive incoming traffic */
        rxSocket = pstDiagTcpInfo->astTcpIpPort[enTcpPort].Tcp;
        if ((rxSocket != INVALID_SOCKET) && (DIAG_FD_ISSET(rxSocket, &ReadySet)))
        {
            if(g_dmsPrintFlag == 1)
            {
        	    diag_printf("diag_AcceptRecvFromSocket tcpRcvData \n");
            }
            diag_TcpRcvData(rxSocket, enTcpPort, pfnConn);
        }
    }
}

VOS_VOID diag_SetPrintFlag(VOS_UINT32 Flag)
{
    g_dmsPrintFlag = Flag;
}

VOS_VOID diag_TcpHelp(VOS_VOID)
{
	DIAG_TCPIP_CTRL_INFO_STRU *pstDiagTcpInfo;
	pstDiagTcpInfo = diag_GetTcpInfo();

    printk("打印使能开关 .................=%d\n",     (VOS_INT)g_dmsPrintFlag);

	printk("TCP端口锁次数 ................=%d\n",     (VOS_INT)g_diagTcpPortLock);
	printk("TCP端口解锁次数...............=%d\n",     (VOS_INT)g_diagTcpPortUnLock);
	printk("fdcount.......................=%d\n",     (VOS_INT)g_fdCount);

	printk("DIAG端口连接状态: ............=%d\n",     (VOS_INT)pstDiagTcpInfo->astTcpIpPort[EN_TCP_PORT_DIAG].ulConnectSta);
	printk("DIAG端口重连状态: ............=%d\n",     (VOS_INT)pstDiagTcpInfo->astTcpIpPort[EN_TCP_PORT_DIAG].ulIsReCon);

	printk("DIAG对应端口号: ..............=%d\n",     (VOS_INT)pstDiagTcpInfo->astTcpIpPort[EN_TCP_PORT_DIAG].aulPortNum);

	printk("DIAG端口 listen 描述符:.......=%d\n",     (VOS_INT)pstDiagTcpInfo->astTcpIpPort[EN_TCP_PORT_DIAG].listener);
	printk("DIAG端口 accept 描述符:.......=%d\n",     (VOS_INT)pstDiagTcpInfo->astTcpIpPort[EN_TCP_PORT_DIAG].Tcp);

}


#else

VOS_UINT32 diag_SocketInit(VOS_VOID)
{
    return 0;
}

VOS_UINT32 diag_TcpSendData(DIAG_TCPIP_PORT_ENUM enTcpPort,VOS_UINT8 *pcData, VOS_UINT32 usDataLen,VOS_UINT32 * pusWrtLen)
{
    return 0;
}

#endif







#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif






/*_END_C_FILE_
 */

