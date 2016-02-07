

#ifndef __OM_SOCKET_PPM_H__
#define __OM_SOCKET_PPM_H__

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "vos.h"
#include "omnvinterface.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#pragma pack(4)

/*****************************************************************************
  2 宏定义
*****************************************************************************/

#define SOCKET_NUM_MAX          (1)                   /* OM、AT在各自的端口上只监听接收一个链接 */

#define SOCK_WIFI_ADDR_LEN      (16)                  /*WIFI网关的地址的最大长度*/

#define SOCK_WIFI_DEFAULT_ADDR  ("127.0.0.1")         /*WIFI通信的默认IP地址*/

#define SOCK_PORT_NUM           (3000)                /*与PC间SOCKET通信的端口号*/

#define SOCK_OM_IND_SRC_PORT_NUM    (20253)           /* OM IND源端口号，UDP */
#define SOCK_OM_IND_DST_PORT_NUM    (20254)           /* OM IND目的端口号，UDP */

#define SOCK_OM_CFG_SRC_PORT_NUM    (20248)           /* OM CFG源端口号，TCP */
#define SOCK_AT_SRC_PORT_NUM        (20249)           /* AT源端口号，TCP */

#if (VOS_VXWORKS == VOS_OS_VER)
#define SOCK_NULL               (0)

#define INVALID_SOCKET  VOS_ERROR
#define SOCKET_ERROR    VOS_ERROR
#define closesocket(s)  shutdown(s, SHUT_RDWR)

#define IP_NULL         0
#define Ip_sockaddr     sockaddr
#define Ip_socklen_t    int
#define Ip_fd_set       fd_set
#define Ip_timeval      timeval

#define htons(xx)       (unsigned short)(((((xx) << 8) & 0xff00) | (((xx) >> 8) & 0x00ff)))

#define socket(domain,type,protocol)       ipcom_socket(domain,type,protocol)
#define shutdown(fd,how)                   ipcom_shutdown((Ip_fd)fd,how)
#define bind(fd,addr,addrlen)              ipcom_bind((Ip_fd)fd,(struct Ip_sockaddr *)addr,(Ip_socklen_t)addrlen)
#define accept(fd,addr,addrlenp)           ipcom_accept((Ip_fd)fd,(struct Ip_sockaddr *)addr,(Ip_socklen_t *)addrlenp)
#define listen(fd,backlog)                 ipcom_listen((Ip_fd)fd,backlog)
#define select(nfds,rf,wf,ef,t)            ipcom_socketselect(nfds,rf,wf,IP_NULL,t)
#define recv(fd,buf,len,flags)             ipcom_recv((Ip_fd)fd,(void *)buf,(Ip_size_t)len,flags)
#define send(fd,msg,len,flags)             ipcom_send((Ip_fd)fd,(void *)msg,(Ip_size_t)len,flags)
#define inet_addr(cp)                      ipcom_inet_addr(cp)

#elif (VOS_LINUX == VOS_OS_VER)
#define SOCK_NULL               (-1)

#define INVALID_SOCKET  VOS_ERROR
#define SOCKET_ERROR    VOS_ERROR
#define closesocket(s)  shutdown(s, SHUT_RDWR)

#define IP_NULL         0
#define Ip_sockaddr     sockaddr
#define Ip_socklen_t    int
#define Ip_fd           int
#define Ip_fd_set       hi_fd_set
#define Ip_timeval      timeval

#define SOCK_FD_ISSET(d, set)     (HI_FD_ISSET(d, set))
#define SOCK_FD_SET(d, set)       (HI_FD_SET(d, set))
#define SOCK_FD_CLR(d, set)       (HI_FD_CLR(d, set))
#define SOCK_FD_ZERO(set)         (HI_FD_ZERO(set))

/* #define htons(xx)       (unsigned short)(((((xx) << 8) & 0xff00) | (((xx) >> 8) & 0x00ff))) */

#define socket(domain,type,protocol)        hi_socket(domain, type, protocol)
#define shutdown(fd,how)                    do{\
                                                hi_shutdown((Ip_fd)fd, how);\
                                                hi_close((Ip_fd)fd);\
                                            }while(0)
#define bind(fd,addr,addrlen)               hi_bind((Ip_fd)fd, (struct Ip_sockaddr *)addr, (Ip_socklen_t)addrlen)
#define accept(fd,addr,addrlenp)            hi_accept((Ip_fd)fd, (struct Ip_sockaddr *)addr, (Ip_socklen_t *)addrlenp)
#define listen(fd,backlog)                  hi_listen((Ip_fd)fd, backlog)
#define select(nfds,rf,wf,ef,t)             hi_select(nfds, (Ip_fd_set *)rf, (Ip_fd_set *)wf, IP_NULL, t)
#define recv(fd,buf,len,flags)              hi_recv((Ip_fd)fd, (void *)buf, len, flags)
#define send(fd,msg,len,flags)              hi_send((Ip_fd)fd, (void *)msg, len, flags)
#define inet_addr(cp)                       hi_inet_addr(cp)

#else
#define SOCK_NULL               (0)

#define Ip_fd_set       fd_set

#define SOCK_FD_ZERO(set)         FD_ZERO(set)
#define SOCK_FD_ISSET(d, set)     FD_ISSET(d, set)
#define SOCK_FD_SET(d, set)       FD_SET(d, set)
#define SOCK_FD_CLR(d, set)       FD_CLR(d, set)

#endif

/*****************************************************************************
  3 枚举定义
*****************************************************************************/

enum
{
    SOCK_OK = 0,
    SOCK_NO_START,
    SOCK_START,
    SOCK_UDP_INIT_ERR,
    SOCK_TCP_INIT_ERR,
    SOCK_BIND_ERR,
    SOCK_LISTEN_ERR,
    SOCK_SELECT_ERR,
    SOCK_ACCEPT_ERR,
    SOCK_SEND_ERR,
    SOCK_BUTT
};

enum OM_SOCKET_TYPE_ENUM
{
    SOCKET_OM_IND,
    SOCKET_OM_CFG,
    SOCKET_AT,
    SOCKET_BUTT
};

typedef VOS_UINT32 OM_SOCKET_TYPE_ENUM_U32;

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

/* WindRiver的源代码 */
#if (VOS_VXWORKS == VOS_OS_VER)

typedef int             Ip_fd;

typedef Ip_fd           SOCKET;

struct in_addr {
    unsigned long       s_addr;
};

struct sockaddr_in {
    unsigned char       sin_len;
    unsigned char       sin_family;
    unsigned short      sin_port;
    struct  in_addr     sin_addr;
    char                sin_zero[8];
};

typedef unsigned int    Ip_size_t;

typedef long int        Ip_ssize_t;

#elif (VOS_LINUX == VOS_OS_VER)

typedef int    SOCKET;

#else

typedef unsigned int    SOCKET;

#endif

typedef struct
{
    SOCKET                              socket;
    VOS_SEM                             SmClose;
    CPM_PHY_PORT_ENUM_UINT32            enPhyPort;
    VOS_UINT16                          usPort;
}COMM_SOCKET_CTRL_INFO_STRU;

/*****************************************************************************
  8 UNION定义
*****************************************************************************/




/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/


/*****************************************************************************
  10 函数声明
*****************************************************************************/

#if (VOS_VXWORKS == VOS_OS_VER)

extern Ip_fd    ipcom_socket(int domain, int type, int protocol);
extern int      ipcom_shutdown(Ip_fd fd, int how);
extern int      ipcom_bind(Ip_fd fd,  struct Ip_sockaddr *addr, Ip_socklen_t addrlen);
extern Ip_fd    ipcom_accept(Ip_fd fd, struct Ip_sockaddr *addr, Ip_socklen_t *addrlenp);
extern int      ipcom_listen(Ip_fd fd, int backlog);
extern int      ipcom_socketselect(int nfds, Ip_fd_set *readfds,
                    Ip_fd_set *writeds, Ip_fd_set *exceptds, struct Ip_timeval *timeout);
extern Ip_ssize_t    ipcom_recv(Ip_fd fd, void *buf, Ip_size_t len, int flags);
extern Ip_ssize_t    ipcom_send(Ip_fd fd, void *msg, Ip_size_t len, int flags);
extern unsigned long ipcom_inet_addr(const char *cp);

#endif

VOS_VOID  Sock_ShutdownAll(VOS_VOID);

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

#endif /* end of OmCommonPpm.h */

