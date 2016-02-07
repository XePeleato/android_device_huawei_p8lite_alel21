

#ifndef __TTF_COMM_H_
#define __TTF_COMM_H_

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/******************************************************************************
  1 其他头文件包含
******************************************************************************/
#include "vos.h"
#include "PsLogdef.h"
#include "PsCommonDef.h"
#include "PsLib.h"
#include "PsTypeDef.h"
#include "LPSCommon.h"
#include "product_config.h"

/******************************************************************************
  2 宏定义
******************************************************************************/

#define PS_UE_CAP_2507_SUPPT                  (PS_CAP_SUPPORT)
#define PS_UE_CAP_1144_SUPPT                  (PS_CAP_SUPPORT)
#define PS_UE_CAP_V42_SUPPT                   (PS_CAP_SUPPORT)

#define     STATIC  static


/*移位宏*/
#define TTF_BIT_MOVE_24  (24)
#define TTF_BIT_MOVE_16  (16)
#define TTF_BIT_MOVE_8   (8)

/*位运算宏*/
#define TTF_BIT_OPR_8    (0xFF)
#define TTF_BIT_OPR_16   (0xFFFF)
#define TTF_BIT_OPR_24   (0xFFFFFF)
#define TTF_BIT_OPR_32   (0xFFFFFFFF)

/*把4个的UINT8型组成UINT32, UINT8从高位到低位*/
#define TTF_CONN_UINT32_FROM_UINT8(ulNum, ucNum1, ucNum2, ucNum3, ucNum4)   \
{       \
    (ulNum) = 0;        \
    (ulNum) |= (VOS_UINT32)(((VOS_UINT32)(ucNum1)) << TTF_BIT_MOVE_24);     \
    (ulNum) |= (VOS_UINT32)(((VOS_UINT32)(ucNum2)) << TTF_BIT_MOVE_16);     \
    (ulNum) |= (VOS_UINT32)(((VOS_UINT32)(ucNum3)) << TTF_BIT_MOVE_8);     \
    (ulNum) |= (VOS_UINT32)(ucNum4);     \
}

#define TTF_MOD_GET(x, y)                       ((x) % (y))
#ifdef _lint
#define TTF_MOD_ADD_GU(x, y, z)                    (((x) + (y)) % (z))
#define TTF_MOD_SUB_GU(x, y, z)                    ((((x) + (z)) - (y)) % (z))

#define TTF_MOD_ADD(x, y, z)                    (((x) + (y)) % (z))
#define TTF_MOD_SUB(x, y, z)                    ((((x) + (z)) - (y)) % (z))
#else
#define TTF_MOD_ADD_GU(x, y, z)                 (((x) + (y)) % (z))
#define TTF_MOD_SUB_GU(x, y, z)                 ((((x) + (z)) - (y)) % (z))

#define TTF_MOD_ADD(x, y, z)                    ((((x) + (y)) < (z))? ((x) + (y)):(((x) + (y)) - (z)))
#define TTF_MOD_SUB(x, y, z)                    (((x) < (y))?(((x) + (z)) - (y)):((x) - (y)))
#endif


/* X除以Y，向上取整 */
#define TTF_CEIL(x, y) \
            /*lint -e{961} */ \
            ((0==(x)%(y))?((x)/(y)):((x)/(y)+1))

#define TTF_MIN(x, y)                           (((x)<(y))?(x):(y))
#define TTF_MAX(x, y)                           (((x)>(y))?(x):(y))

/* SN在[lower, upper]区间外 */
#define TTF_SN_OUTSIDE_WINDOW(sn, lowerEdge, upperEdge, module)\
                                                            ((TTF_MOD_SUB(sn, lowerEdge, module) > TTF_MOD_SUB(sn, upperEdge, module))&&(sn != upperEdge))
/* SN在[lower, upper]区间内，含lower upper上下边界 */
#define TTF_SN_WITHIN_WINDOW(sn, lowerEdge, upperEdge, module)\
            /*lint -e{961} */ \
            ((TTF_MOD_SUB(sn, lowerEdge, module) < TTF_MOD_SUB(sn, upperEdge, module))||(sn == upperEdge))

#define TTF_SN1_BELOW_SN2_WITHIN_WINDOW(sn1, sn2, upperEdge, module)\
            (TTF_MOD_SUB(upperEdge, sn1, module) > TTF_MOD_SUB(upperEdge, sn2, module))

#define TTF_SN_IN_WINDOW(sn, lowerEdge, winSize, module) \
            ( TTF_MOD_SUB((sn), (lowerEdge), (module)) < (winSize) )
/*根据块号usSn，获取它在窗口中的存储位置索引:
usSns:块号的模
usWndSize:窗口大小
usHeader:窗口中某一块号为usHeaderSn的节点，在窗口中对应的存储位置索引
usHeaderSn:窗口中某一节点的块号
usSn:当前期望获取其存储位置的块的块号*/
#define TTF_GET_WND_POS(usSns, usWndSize, usHeader, usHeaderSn, usSn)   \
    TTF_MOD_GET(((usHeader) + TTF_MOD_SUB((usSn), (usHeaderSn), (usSns))), (usWndSize))

/* change bit to byte */
#define TTF_BIT2BYTE(BitLen)        (((BitLen) + 7) >> 3)
/* change byte to bit */
#define TTF_BYTE2BIT(ByteLen)       (VOS_UINT32)(((VOS_UINT32)(ByteLen)) << 3)

/*=============*/   /*获取结构体成员变量offset的办法*/
#define TTF_OFFSET_OF(s, m) /*lint -e(413)*/((VOS_SIZE_T)(VOS_UINT_PTR)(&(((s *)0)->m)))

#define TTF_GET_OFFSET(offset, s, m) \
{ \
    /*lint -e545*/ \
    /*lint -e413*/ \
    /*lint -e831*/ \
    offset  = TTF_OFFSET_OF(s, m); \
    /*lint +e831*/ \
    /*lint +e413*/ \
    /*lint +e545*/ \
}

/* 将一个整型数据ucSrc，填入另一个整型数据ucDest中从bitstart(取值范围0到7)开始的bitlen(取值范围1到8)长度的IE中,
   输入的有效性由输入者保证,各个模块可以类似定义设置值的宏。 ucDest在使用前需要清零。*/
#define TTF_SET_UC_BIT(ucSrc, ucDest, bitstart, bitlen) \
((ucDest) |= (VOS_UINT8)( (VOS_UINT8)((VOS_UINT32)(ucSrc) << (8 - bitlen)) >> ((8 - bitstart) - bitlen) ))

/* 将一个整型数据ucSrc，填入另一个整型数据ucDest中从最高位开始的bitlen(取值范围1到8)长度的IE中,
   输入的有效性由输入者保证,各个模块可以类似定义设置值的宏。 ucDest在使用前需要清零。*/
#define TTF_SET_UC_HIGH_BIT(ucSrc, ucDest, bitlen) \
((ucDest) |= (VOS_UINT8)( (VOS_UINT8)((VOS_UINT32)(ucSrc) << (8 - bitlen)) ))

/* 将一个整型数据usSrc，填入另一个整型数据usDest中从bitstart(取值范围0到15)开始的bitlen(取值范围1到16)长度的IE中,
   输入的有效性由输入者保证,各个模块可以类似定义设置值的宏。 usDest在使用前需要清零。*/
#define TTF_SET_US_BIT(usSrc, usDest, bitstart, bitlen) \
((usDest) |= (VOS_UINT16)( (VOS_UINT16)((VOS_UINT32)(usSrc) << (16 - bitlen)) >> ((16 - bitstart) - bitlen) ))

/* 将一个整型数据usSrc，填入另一个整型数据usDest中从最高位开始的bitlen(取值范围1到16)长度的IE中,
   输入的有效性由输入者保证,各个模块可以类似定义设置值的宏。 ucDest在使用前需要清零。*/
#define TTF_SET_US_HIGH_BIT(usSrc, usDest, bitlen) \
((usDest) |= (VOS_UINT16)( (VOS_UINT16)((VOS_UINT32)(usSrc) << (16 - bitlen)) ))

/* 从一个US型数据中获取从bitstart(取值范围0到15)开始的bitlen(取值范围1到16)长度的值,
   输入的有效性由输入者保证,各个模块可以类似定义设置值的宏 */
#define TTF_GET_US_BIT(us, bitstart, bitlen) \
((VOS_UINT16)((VOS_UINT32)(us) << ((16 - (bitstart)) - (bitlen))) >> (16 - (bitlen)))

#define TTF_GET_US_HIGH_BIT(us, bitlen) \
((VOS_UINT16)((us)  >> (16 - (bitlen))))

/* 从一个UC型数据中获取从bitstart(取值范围0到7)开始的bitlen(取值范围1到8)长度的值,
   输入的有效性由输入者保证,各个模块可以类似定义设置值的宏 */
#define TTF_GET_UC_BIT(uc, bitstart, bitlen) \
((VOS_UINT8)((VOS_UINT32)(uc) << ((8 - (bitstart)) - (bitlen))) >> (8 - (bitlen)))

/* 从一个整型数据uc中，获取从最高位开始的bitlen(取值范围1到8)长度的IE,
   输入的有效性由输入者保证,各个模块可以类似定义设置值的宏。 */
#define TTF_GET_UC_HIGH_BIT(uc, bitlen) \
((VOS_UINT8)(uc) >> (8 - (bitlen)))

#define TTF_GET_HIGH_16BIT_FROM_32BIT(ul32BitValue) ((VOS_UINT16)((ul32BitValue) >> 16))
#define TTF_GET_LOW_16BIT_FROM_32BIT(ul32BitValue)  ((VOS_UINT16)((ul32BitValue) & 0xFFFF))
#define TTF_MAKE_32BIT(usHighVal, usLowVal)         ( (((VOS_UINT32)(usHighVal))<<16) | ((VOS_UINT32)((usLowVal) & 0xFFFF)) )
#define TTF_MAKE_16BIT(ucHighVal, ucLowVal)         ( (VOS_UINT16)(((VOS_UINT8)(ucLowVal)) | (((VOS_UINT16)((VOS_UINT8)(ucHighVal)))<<8)) )

#define TTF_SET_HIGH_16BIT_TO_32BIT(ul32BitVal, usHigh16BitVal) \
    ( (((VOS_UINT32)ul32BitVal) & 0xFFFF) | (((VOS_UINT32)usHigh16BitVal) << 16) )

#define TTF_GET_4BYTE_ALIGN_VALUE(ulSize)   ( ((ulSize) + 0x03) & (~0x03) )

#define TTF_GET_8BYTE_ALIGN_VALUE(ulSize)   ( ((ulSize) + 0x07) & (~0x07) )


/* 设置标志比特位的值为1，bitpos:[0..31] */
#define TTF_SET_A_BIT(value, bitpos)    ((value) |= (1UL << (bitpos)))

/* 清除标志比特位的值为0，bitpos:[0..31] */
#define TTF_CLEAR_A_BIT(value, bitpos)  ((value) &= (~(1UL << (bitpos))))

/* 获取标志比特位的值，结果返回1和0，bitpos:[0..31] */
#define TTF_GET_A_BIT(value, bitpos)    (((value) & (1UL << (bitpos))) >> (bitpos))

/* 获取数组的大小 */
#define TTF_GET_ARRAYSIZE(array)        (sizeof(array)/sizeof(array[0]))


/* 从一个消息中获取消息类型的宏，输入参数的有效性由输入者保证 */
#define TTF_GET_MSGTYPE(pMsg) \
    (*(VOS_UINT16*)((VOS_UINT8*)(pMsg) + VOS_MSG_HEAD_LENGTH))

#if (OSA_CPU_CCPU == VOS_OSA_CPU)
#ifndef __PS_WIN32_RECUR__
#define TTF_TRACE_MSG(pMsg) OM_TraceMsgHook(pMsg)
#else
#define TTF_TRACE_MSG(pMsg) TTF_TraceMsgHook(pMsg)
#endif
#else
#define TTF_TRACE_MSG(pMsg)
#endif
/* 回放时标识是否回放ST工程抓的Trace,UT工程也需要 */
#define RECUR_ST_TRACE      (1)
#define RECUR_OTHER_TRACE   (0)

/*避免与其它模块在相同时间点导出全局变量,所以设一个特殊值
  现有导出全局变量模块，NAS每一分钟导一次，GRM与LLC导出时间见如下设置
 */
#define GRM_MNTN_TIMER_PC_RECUR_LEN     60533
#define LLC_MNTN_TIMER_PC_RECUR_LEN     60503

/* 16bit 32 bit 寄存器读写 */
#define TTF_WRITE_32REG(pAddr, value)       ( *((volatile VOS_UINT32 *)(pAddr)) = (value) )
#define TTF_WRITE_16REG(pAddr, value)       ( *((volatile VOS_UINT16 *)(pAddr)) = (value) )
#define TTF_READ_32REG(pAddr)               ( *((volatile VOS_UINT32 *)(pAddr)) )
#define TTF_READ_16REG(pAddr)               ( *((volatile VOS_UINT16 *)(pAddr)) )

/* 32 bit 寄存器屏蔽 */
#define TTF_WRITE_32REG_MASK(pAddr,ulMask)  ( *(volatile VOS_UINT32 *)(pAddr) |= (ulMask) )

/* 32 bit 寄存器清除 */
#define TTF_CLEAR_32REG_MASK(pAddr,ulMask)  ( *(volatile VOS_UINT32 *)(pAddr) &= ~(ulMask) )

/* 从一个消息中获取消息类型的宏，输入参数的有效性由输入者保证 */
#define TTF_GET_MSG_NAME(pRcvMsg)           (*((VOS_UINT32 *)(VOS_VOID *)((VOS_UINT8 *)(pRcvMsg) + VOS_MSG_HEAD_LENGTH)))
#define TTF_GET_MSG_SENDER_PID(pRcvMsg)     (*((VOS_UINT32 *)(VOS_VOID *)(pRcvMsg) + 1))
#define TTF_GET_MSG_RECV_PID(pRcvMsg)       (*((VOS_UINT32 *)(VOS_VOID *)(pRcvMsg) + 3))
#define TTF_GET_MSG_MSG_LEN(pRcvMsg)        (*((VOS_UINT32 *)(VOS_VOID *)(pRcvMsg) + 4))
#define TTF_GET_MSG_OPID(pRcvMsg)      (*(VOS_UINT16 *)(VOS_VOID*)((VOS_UINT8 *)pRcvMsg + 24))


/*设置消息头的数据*/
#define TTF_SET_MSG_SENDER_PID(pRcvMsg, ulPid)          \
                                            (*((VOS_UINT32 *)(VOS_VOID *)(pRcvMsg) + 1) = (ulPid))
#define TTF_SET_MSG_RECV_PID(pRcvMsg, ulPid)          \
                                            (*((VOS_UINT32 *)(VOS_VOID *)(pRcvMsg) + 3) = (ulPid))
#define TTF_SET_MSG_LEN(pRcvMsg, ulLen)            \
                                            (*((VOS_UINT32 *)(VOS_VOID *)(pRcvMsg) + 4) = (ulLen))


#define  ETH_MAC_HEADER_LEN             (14)      /*以太网帧头长度*/
/*Ethenet*/
#define ETH_MAC_ADDR_LEN                6         /*以太网帧MAC地址长度*/
#define ETH_MIN_FRAME_SIZE              60        /*以太网帧最小长度*/
#define ETH_CRC_LEN                     4
#define ETH_MAX_FRAME_SIZE              (1514)    /*以太网最大长度*/

/*ARP*/
#define ETH_ARP_REQ_TYPE                0x0100      /*ARP Request*/
#define ETH_ARP_RSP_TYPE                0x0200      /*ARP Reply*/
#define ETH_ARP_FIXED_MSG_LEN           8           /*ARP固定部分长度*/

/*IP*/
/*#define IPPACKET_MIN_LEN                (1500)
 */
#define IPV4_FIX_HDR_LEN                (20)       /* IPV4固定头长度*/
#define IPV4_HDR_TTL                    (128)      /* IPV4 头中的TTL字段默认值*/

#define IP_IPV4_VERSION                 (4)        /* IP V4版本号      */
#define IP_PROTOCOL_UDP                 (0x11)     /* IP承载协议是UDP  */
#define IP_PROTOCOL_TCP                 (0x06)     /* IP承载协议是TCP  */
#define IP_PROTOCOL_ICMP                (0x01)     /* IP承载协议是ICMP */

/*DHCP报文目的端口67,定义为网络字节序*/
#define UDP_DHCP_SERVICE_PORT           (0x4300)   /* DHCP服务的端口号 */
#define UDP_DHCP_CLIENT_PORT            (0x4400)   /* DHCP Client的端口号*/

#define TTF_L1_CACHE_SHIFT              (5)
#define TTF_L1_CACHE_BYTES              (1UL << TTF_L1_CACHE_SHIFT)


#define TTF_SMP_CACHE_BYTES             (TTF_L1_CACHE_BYTES)        /* Linux版本发生改变时需要重新check该值 */

#define TTF_DATA_ALIGN(X)	            (((X) + (TTF_SMP_CACHE_BYTES - 1)) & \
                                        ~(TTF_SMP_CACHE_BYTES - 1))

#if (FEATURE_ON == FEATURE_TCP_ACK_IN_FRONT)
#define IP_VER_MASK                     (0xF0)    /* 用于取IP协议版本的掩码 */
#define IP_HEADER_LEN_MASK              (0x0F)    /* 用于取IP首部长度的掩码 */
#define IPV4_VER_VAL                    (0x40)    /* IP v4 */
#define IPV6_VER_VAL                    (0x60)    /* IP v6 */

#define PROTOCOL_POS                    (9)       /* 协议标识相对IP头部首字节Protocal 的偏移 */
#define NEXT_HEADER_POS                 (6)       /* 协议标识相对IP头部首字节Next header 的偏移 */

#define IPV4_HEAD_NORMAL_LEN            (20)      /* IPV4头长典型值 */
#define IPV6_HEAD_NORMAL_LEN            (40)      /* IPV6头长典型值 */

#define TCP_PROTOCOL_VAL                (0x06)    /* TCP协议标识的值 */
#define TCP_LEN_POS                     (12)      /* TCP头部长度字段相对TCP头部首字节的偏移 */
#define TCP_LEN_MASK                    (0xF0)    /* 用于取TCP头部长度字段值的掩码 */

#define TCP_ACK_IPV4_MIN_LEN            (40)        /* TCP ACK最小长度, 字节为单位, typically IP header 20 bytes and TCP header 20 bytes */
#define TCP_ACK_IPV4_MAX_LEN            (80)        /* TCP ACK最大长度, 字节为单位, typically IP header 20 bytes, and TCP header max size 60 bytes */

#define TCP_ACK_IPV6_MIN_LEN            (60)        /* TCP ACK最小长度, 字节为单位, typically IP header 40 bytes and TCP header 20 bytes */
#define TCP_ACK_IPV6_MAX_LEN            (80)        /* ?? */
#define IP_VER_VAL                      (0x40)     /* IP v4 */
#define TCP_IP_OFFSET_LEN_POS           (32)       /* TCP头部长度字段相对IP头部首字节的偏移 */

#define IP_VER_VAL_V6                   (0x60)     /* IP v6 */

#define PROTOCOL_POS_V6                 (6)        /* 协议标识相对IP头部首字节的偏移 */
#define IP_HEAD_NORMAL_LEN              (20)       /* IP头长典型值 */
#define IP_HEAD_NORMAL_LEN_V6           (40)       /* IP头长典型值 */

#define TCP_PROTOCOL_VAL_V6             (0x06)     /* TCP协议标识的值 */

#define TCP_LEN_POS_V6                  (52)       /* TCP头部长度字段相对IP头部首字节的偏移 */

#define TCP_ACK_MIN_LEN                 (40)       /* TCP ACK最小长度, 字节为单位, typically IP header 20 bytes and TCP header 20 bytes */
#define TCP_ACK_MIN_LEN_V6              (60)       /* TCP ACK最小长度, 字节为单位, typically IP header 40 bytes and TCP header 20 bytes */
#define TCP_ACK_MAX_LEN                 (80)       /* TCP ACK最大长度, 字节为单位, typically IP header 20 bytes, and TCP header max size 60 bytes */
#define TCP_ACK_MAX_LEN_V6              (100)       /* TCP ACK最大长度, 字节为单位, typically IP header 40 bytes, and TCP header max size 60 bytes */
#define ICMP_PROTOCOL_VAL               (0x01)     /* ICMP协议标识的值 */
#define ICMP_PROTOCOL_VAL_V6            (0x3A)     /* ICMP协议标识的值 */

#endif

/******************************************************************************
  3 枚举定义
******************************************************************************/
/*****************************************************************************
 枚举名    :
 协议表格  :
 ASN.1描述 :
 枚举说明  :
*****************************************************************************/
enum APPITF_RTN_CODE_ENUM
{
    APPITF_SUCC                             = 0,
    APPITF_FAIL                             = 1,

    APPITF_PTR_NULL                         = 2,    /*空指针*/
    APPITF_PARA_ERR                         = 3,    /*参数错误*/
    APPITF_STATE_ERR                        = 4,    /*状态错误*/
    APPITF_MODE_ERR                         = 5,    /*模式错误*/
    APPITF_SCOPE_ERR                        = 6,    /*范围错误*/
    APPITF_MEM_ALLOC_FAIL                   = 7,    /*内存分配失败*/
    APPITF_MSG_ALLOC_FAIL                   = 8,    /*消息分配失败*/
    APPITF_MSG_SEND_FAIL                    = 9,    /*消息发送失败*/
    APPITF_TIMER_ERR                        = 10,   /*定时器错误*/
    APPITF_TIMER_OUT                        = 11,   /*定时器超时*/
    APPITF_QUE_FULL                         = 12,   /*队列满*/
    APPITF_QUE_EMPTY                        = 13,   /*队列空*/

    APPITF_SNDTOETH_FAIL                    = 14,
    APPITF_CPYMEM_ERR                       = 15,
    APPITF_TOOBIGIPPACKET                   = 16,

    APP_RTN_CODE_BUTT
};
typedef VOS_UINT32   APP_RTN_CODE_ENUM_UINT32;

/*****************************************************************************
 枚举名    : TTF_SOFT_RESET_CAUSE_VALUE_ENUM
 协议表格  :
 ASN.1描述 :
 枚举说明  : TTF  软复位原因值
*****************************************************************************/
enum TTF_SOFT_RESET_CAUSE_VALUE_ENUM
{
    TTF_SOFT_RESET_CAUSE_NORMAL                      = 0x30000000,  /* 新增主动软复位，目前只使用该值，日后可拓展，以0x3*******开头 */
    TTF_SOFT_RESET_CAUSE_BUTT
};
typedef VOS_INT32 TTF_SOFT_RESET_CAUSE_VALUE_ENUM_INT32;


/******************************************************************************
  4 全局变量声明
******************************************************************************/


/******************************************************************************
  5 消息头定义
******************************************************************************/


/******************************************************************************
  6 消息定义
******************************************************************************/


/******************************************************************************
  7 STRUCT定义
******************************************************************************/
#pragma pack(4)

typedef struct
{
    VOS_MSG_HEADER                      /* 消息头 */

    VOS_UINT16 usMsgType;               /* 消息类型 */
    VOS_UINT16 usTransId;               /* 子进程标识,用于定位子进程 */
#define TTF_MSG_HEAD    4               /* 公共消息中头部附加信息长度(usMsgType和usTransId) */

    VOS_UINT8  astMsg[4];               /* 消息结构指针，指向真正携带的消息 */
}TTF_COMM_MSG;

typedef struct
{
    VOS_MSG_HEADER                      /* 消息头 */

    VOS_UINT16 usMsgType;               /* 消息类型 */
    VOS_UINT8  astMsg[2];               /* 消息结构指针，指向真正携带的消息 */
}WTF_COMM_MSG;


/* 消息处理函数定义
   pMsg:模块收到的输入消息指针
   pSelfDefinePara:模块自定义的输入参数指针 */
typedef VOS_UINT32 (*PTTFMSGPROC)(MsgBlock *pMsg, void *pSelfDefinePara);

typedef struct
{
    VOS_UINT16 usMsgType;               /* 收到的消息ID */
    VOS_UINT8  ucState;                 /* 收到该消息时的状态 */
    VOS_UINT8  ucRsv;                   /* 预留扩展 */
    PTTFMSGPROC pProcFun;               /* 该状态下收到消息的处理函数 */
}TTF_COMM_STATUS;

#if ((VOS_OS_VER == VOS_WIN32) || (VOS_OS_VER == VOS_NUCLEUS))
#pragma pack()
#else
#pragma pack(0)
#endif
/******************************************************************************
  8 UNION定义
******************************************************************************/


/******************************************************************************
  9 OTHERS定义
******************************************************************************/
extern VOS_UINT32   OM_TraceMsgHook(VOS_VOID *pMsg);


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* TTFComm.h */


