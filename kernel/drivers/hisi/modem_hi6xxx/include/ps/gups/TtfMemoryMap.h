

#ifndef __TTFMEMORYMAP_H__
#define __TTFMEMORYMAP_H__


/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "product_config.h"
#include "vos.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif




/*****************************************************************************
  2 宏定义
*****************************************************************************/
/* 32 字节对齐 */
#define TTF_GET_32BYTE_ALIGN_VALUE(ulAddr)   (((VOS_UINT_PTR)(ulAddr) + 0x1FUL) & (~0x1FUL))

    /*********Uncache mem(20M) deploy begin*********/
    /*->startAddr|-------------------------------|   */
    /*           |HDLC_DEF_INPUT_PARA_LINK_BUF   |   */
    /*           |-----------------------------  |   */
    /*           |      HDLC_DEF_RPT_BUF         |   */
    /*           |-----------------------------  |   */
    /*           |     HDLC_DEF_OUTPUT_BUF       |   */
    /*           |-----------------------------  |   */
    /*           |HDLC_FRM_INPUT_PARA_LINK_BUF   |   */
    /*           |-----------------------------  |   */
    /*           |    HDLC_FRM_RPT_BUF_ADDR      |   */
    /*           |-----------------------------  |   */
    /*           |HDLC_FRM_OUTPUT_PARA_LINK_BUF  |   */
    /*           |-----------------------------  |   */
    /*           |         PAD (0-31B)           |   */
    /*           |-----------------------------  |   */
    /*           |UL CIPHER PARA NODE(35k)       |   */
    /*           |-------------------------------|   */
    /*           |UL CIPHER SRC SEG DATA (51k)   |   */
    /*           |-------------------------------|   */
    /*           |UL CIPHER DST SEG DATA (51k)   |   */
    /*           |-------------------------------|   */
    /*           |   UL CIPHER KEY (1k)          |   */
    /*           |-------------------------------|   */
    /*           |DL CIPHER PARA NODE(11k)       |   */
    /*           |-------------------------------|   */
    /*           |DL CIPHER MAC SEG DATA(155k)   |   */
    /*           |-------------------------------|   */
    /*           |DL CIPHER RLC SEG DATA(155k)   |   */
    /*           |-------------------------------|   */
    /*           |   DL CIPHER KEY (1k)          |   */
    /*           |-------------------------------|   */
    /*           |TTF_LL_CIPHER_PARA_NODE(1k)    |   */
    /*           |-------------------------------|   */
    /*           |TTF_LL_CIPHER_KEY(16B)         |   */
    /*           |-------------------------------|   */
    /*           |TTF_LL_CIPHER_ARRAY(1560B)     |   */
    /*           |-------------------------------|   */
    /*           |I1_TTF_LL_CIPHER_PARA_NODE(1k) |   */
    /*           |-------------------------------|   */
    /*           |I1_TTF_LL_CIPHER_KEY(16B)      |   */
    /*           |-------------------------------|   */
    /*           |I1_TTF_LL_CIPHER_ARRAY(1560B)  |   */
    /*           |-------------------------------|   */
    /*           |         PAD (0-31B)           |   */
    /*           |-----------------------------  |   */
    /*           |                               |   */
    /*           |     TTF_BBPMASTER_MEM         |   */
    /*           |                               |   */
    /*           |-----------------------------  |   */
    /*           |                               |   */
    /*           |     TTF_ACORE_POOL_MEM        |   */
    /*           |                               |   */
    /*           |-----------------------------  |   */
    /*           |         PAD (0-31B)           |   */
    /*           |-----------------------------  |   */
    /*           |                               |   */
    /*           |     TTF_CCORE_POOL_MEM        |   */
    /*           |                               |   */
    /*           |-----------------------------  |   */


/* 分配给TTF的总内存大小和基地址 */

/* 各节点参数所需的内存 */

/* HDLC 设备使用起始地址 */
#define TTF_HDLC_MASTER_START_ADDR(BaseAddr) \
            (TTF_MEMCTRL_GetTypeAddr((TTF_GET_32BYTE_ALIGN_VALUE(BaseAddr)), TTF_MEMCTRL_HDLC_MASTER_ADDR_TYPE))

/* CICOM 设备使用起始地址*/
#define TTF_UL_CIPHER_PARA_NODE_ADDR(BaseAddr) \
            (TTF_MEMCTRL_GetTypeAddr((TTF_GET_32BYTE_ALIGN_VALUE(BaseAddr)), TTF_MEMCTRL_CIPHER_MASTER_ADDR_TYPE))

/* BBP Master使用内存基地址 */
#define TTF_BBP_MASTER_START_ADDR(BaseAddr) \
            (TTF_MEMCTRL_GetTypeAddr((TTF_GET_32BYTE_ALIGN_VALUE(BaseAddr)), TTF_MEMCTRL_BBP_MASTER_ADDR_TYPE))

/* A CPU 不可cache内存基地址 */
#define TTF_ACORE_POOL_MEM_START_ADDR(BaseAddr) \
            (TTF_MEMCTRL_GetTypeAddr((TTF_GET_32BYTE_ALIGN_VALUE(BaseAddr)), TTF_MEMCTRL_ACORE_POOL_ADDR_TYPE))

/* C CPU 不可cache内存基地址 */
#define TTF_CCORE_POOL_MEM_START_ADDR(BaseAddr) \
            (TTF_MEMCTRL_GetTypeAddr((TTF_GET_32BYTE_ALIGN_VALUE(BaseAddr)), TTF_MEMCTRL_CCORE_POOL_ADDR_TYPE))


/* ----------------HDLC优化新增内存 START---------------- */

/* HDLC Master设备使用内存基地址 */
#if 0
/* HDLC 设备使用起始地址 */
#define TTF_HDLC_MASTER_START_ADDR(BaseAddr)                TTF_GET_8BYTE_ALIGN_VALUE(BaseAddr)
#endif

/* 解封装单次配置最大输入参数节点个数 */
#define TTF_HDLC_DEF_INPUT_PARA_LINK_MAX_NUM        (20)

/* 解封装输入参数内存大小和基地址
   HDLC_DEF_INPUT_PARA_LINK_MAX_NUM * sizeof(HDLC_PARA_LINK_NODE_STRU) */
#define TTF_HDLC_DEF_INPUT_PARA_LINK_BUF_LEN        (TTF_HDLC_DEF_INPUT_PARA_LINK_MAX_NUM * 12)

/* 解封装单次配置最大输入数据总长度 */
#define TTF_HDLC_DEF_INPUT_PARA_LINK_MAX_SIZE       (13*1024)


/* 一次解封装输出IP包/简单PPP帧最大个数 = 一次解封装最大输入长度/最小IP包长度(20B) */
#define TTF_HDLC_DEF_OUTPUT_MAX_NUM                 (TTF_HDLC_DEF_INPUT_PARA_LINK_MAX_SIZE/20)

/* 解封装信息上报最大个数 */
#define TTF_HDLC_DEF_RPT_MAX_NUM                    (TTF_HDLC_DEF_OUTPUT_MAX_NUM)

/* 解封装信息上报空间内存大小和基地址
   一次解封装输出的最大IP包/简单PPP帧个数*sizeof(HDLC_FRM_RPT_NODE) */
#define TTF_HDLC_DEF_RPT_BUF_LEN                    (TTF_HDLC_DEF_RPT_MAX_NUM*8)

/* ((TTF_HDLC_DEF_INPUT_PARA_LINK_MAX_SIZE / 6 + 1) * 8)
 * 6为占用最大空间的最小有效帧
 * 8为四字节对齐
 * +1 为小数向上取整
*/

#define TTF_HDLC_DEF_OUTPUT_DATA_BUF_LEN            (18 * 1024)

    /* 供HDLC MASTER设备使用的内存总长度，当结构体HDLC_DEF_BUFF_INFO_STRU变化时，
        该宏应作相应变化 */
#define TTF_HDLC_MASTER_DEF_BUF_LEN                 (TTF_HDLC_DEF_INPUT_PARA_LINK_BUF_LEN + \
                                                        TTF_HDLC_DEF_RPT_BUF_LEN + \
                                                        TTF_HDLC_DEF_OUTPUT_DATA_BUF_LEN)

/* 封装单次配置最大输入参数节点个数 */
#define TTF_HDLC_FRM_INPUT_PARA_LINK_MAX_NUM        (20)

/* 封装单次配置最大输入数据总长度 */
#define TTF_HDLC_FRM_INPUT_PARA_LINK_MAX_SIZE       (15*1024)

/* 封装输入参数内存大小 = TTF_HDLC_FRM_INPUT_PARA_LINK_MAX_NUM * sizeof(HDLC_PARA_LINK_NODE_STRU) */
#define TTF_HDLC_FRM_INPUT_PARA_LINK_BUF_LEN        (TTF_HDLC_FRM_INPUT_PARA_LINK_MAX_NUM * 12)

/* 封装信息最大个数 */
#define TTF_HDLC_FRM_RPT_MAX_NUM                    (TTF_HDLC_FRM_INPUT_PARA_LINK_MAX_NUM)

/* 封装信息上报空间内存大小和基地址
   TTF_HDLC_FRM_RPT_MAX_NUM * sizeof(HDLC_DEF_RPT_NODE) */
#define TTF_HDLC_FRM_RPT_BUF_LEN                    (TTF_HDLC_FRM_RPT_MAX_NUM * 8)

/* 封装单次配置最大输出参数节点个数 */
#define TTF_HDLC_FRM_OUTPUT_PARA_LINK_MAX_NUM       (40)

/* 封装输出参数内存大小和基地址
   TTF_HDLC_FRM_OUTPUT_PARA_LINK_MAX_NUM * sizeof(HDLC_PARA_LINK_NODE_STRU) */
#define TTF_HDLC_FRM_OUTPUT_PARA_LINK_BUF_LEN       (TTF_HDLC_FRM_OUTPUT_PARA_LINK_MAX_NUM * 12)

/* 供HDLC MASTER设备使用的内存总长度，当结构体HDLC_FRM_BUFF_INFO_STRU变化时，
    该宏应作相应变化 */
#define TTF_HDLC_MASTER_FRM_BUF_LEN                 (TTF_HDLC_FRM_INPUT_PARA_LINK_BUF_LEN + \
                                                        TTF_HDLC_FRM_RPT_BUF_LEN + \
                                                        TTF_HDLC_FRM_OUTPUT_PARA_LINK_BUF_LEN)

#if (FEATURE_ON == FEATURE_PPP)
/* 供HDLC MASTER设备使用的内存总长度 */
#define TTF_HDLC_MASTER_LINK_TOTAL_LEN              (TTF_GET_32BYTE_ALIGN_VALUE((TTF_HDLC_MASTER_DEF_BUF_LEN + \
                                                        TTF_HDLC_MASTER_FRM_BUF_LEN)))
#else
#define TTF_HDLC_MASTER_LINK_TOTAL_LEN              (0)
#endif

/* ----------------HDLC优化新增内存 END---------------- */

#if 0
/* HDLC Master设备使用内存尾地址 */
#define TTF_HDLC_MASTER_END_ADDR(BaseAddr)\
           (TTF_HDLC_MASTER_START_ADDR(BaseAddr) + TTF_HDLC_MASTER_LINK_LEN)

/* ----------------HDLC优化新增内存 END---------------- */
#endif

/* ----------------CICOM设备内存 START---------------- */

/*1096(WTTF_MAX_PDU_NUM_PER_UL_LOCH) * 32Byte(RLC_PDU_SEG_CIPHER_PARA_STRU) ≈ 35KByte*/
#define TTF_UL_CIPHER_PARA_NODE_LEN         (35*1024)

#define TTF_UL_CIPHER_SRC_SEG_DATA_ADDR(BaseAddr)\
           (TTF_UL_CIPHER_PARA_NODE_ADDR(BaseAddr) + TTF_UL_CIPHER_PARA_NODE_LEN)

#define TTF_UL_CIPHER_SRC_SEG_DATA_LEN      (51*1024)

#define TTF_UL_CIPHER_DST_SEG_DATA_ADDR(BaseAddr)\
            (TTF_UL_CIPHER_SRC_SEG_DATA_ADDR(BaseAddr) + TTF_UL_CIPHER_SRC_SEG_DATA_LEN)

#define TTF_UL_CIPHER_DST_SEG_DATA_LEN      (51*1024)

#define TTF_UL_CIPHER_KEY_ADDR(BaseAddr)\
            (TTF_UL_CIPHER_DST_SEG_DATA_ADDR(BaseAddr) + TTF_UL_CIPHER_DST_SEG_DATA_LEN)

/* 上行加密密钥使用内存长度和基地址
   3(RLC_CPH_CFG_NUM) * 16Byte(WUE_CIPHER_KEY_LEN) = 48Byte 取整为1K */
#define TTF_UL_CIPHER_KEY_LEN               (1*1024)

#define TTF_DL_CIPHER_PARA_NODE_ADDR(BaseAddr)\
           (TTF_UL_CIPHER_KEY_ADDR(BaseAddr) + TTF_UL_CIPHER_KEY_LEN)

/* 下行解密参数节点使用内存长度和基地址
    350(WTTF_MAX_PDU_NUM_PER_DL_LOCH) * 32Byte(RLC_PDU_SEG_CIPHER_PARA_STRU) ≈ 11KByte */
#define TTF_DL_CIPHER_PARA_NODE_LEN         (11*1024)

/* Modified Mem Reduce End */
#define TTF_DL_CIPHER_MAC_SEG_DATA_ADDR(BaseAddr)\
           (TTF_DL_CIPHER_PARA_NODE_ADDR(BaseAddr) + TTF_DL_CIPHER_PARA_NODE_LEN)

/* 下行MAC-ehs SDU分段使用内存长度和基地址 */
/*13200(WTTF_MAC_EHS_SDU_SEG_DATA_NUM) * 12Byte(SEG_DATA_STRU大小) ≈ 155KByte */
#define TTF_DL_CIPHER_MAC_SEG_DATA_LEN      (155*1024)

#define TTF_DL_CIPHER_SEG_DATA_ADDR(BaseAddr)\
           (TTF_DL_CIPHER_MAC_SEG_DATA_ADDR(BaseAddr) + TTF_DL_CIPHER_MAC_SEG_DATA_LEN)

#define TTF_DL_RLC_CIPHER_SEG_DATA_LEN      (155*1024)

#define TTF_DL_CIPHER_KEY_ADDR(BaseAddr)\
           (TTF_DL_CIPHER_SEG_DATA_ADDR(BaseAddr) + TTF_DL_RLC_CIPHER_SEG_DATA_LEN)

/* 下行解密密钥使用内存长度和基地址
   3(RLC_CPH_CFG_NUM) * 16Byte(WUE_CIPHER_KEY_LEN) = 48Byte 取整为1K */
#define TTF_DL_CIPHER_KEY_LEN               (1*1024)

#define TTF_UL_CIPHER_PDU_NODE_TOTAL_LEN    \
    (TTF_UL_CIPHER_PARA_NODE_LEN + TTF_UL_CIPHER_SRC_SEG_DATA_LEN + TTF_UL_CIPHER_DST_SEG_DATA_LEN)

#define TTF_DL_CIPHER_PDU_NODE_TOTAL_LEN    \
    (TTF_DL_CIPHER_PARA_NODE_LEN + TTF_DL_CIPHER_MAC_SEG_DATA_LEN + TTF_DL_RLC_CIPHER_SEG_DATA_LEN)

/* W模 供MASTER设备使用的内存总长度 */
#define TTF_CIPHER_MASTER_LINK_LEN          (TTF_UL_CIPHER_PDU_NODE_TOTAL_LEN + TTF_UL_CIPHER_KEY_LEN + \
                                             TTF_DL_CIPHER_PDU_NODE_TOTAL_LEN + TTF_DL_CIPHER_KEY_LEN)

/* G模加解密使用的内存 */
#define TTF_LL_CIPHER_PARA_NODE_ADDR(BaseAddr) \
           (TTF_UL_CIPHER_PARA_NODE_ADDR(BaseAddr) + TTF_CIPHER_MASTER_LINK_LEN)

/*1(每次只处理一块LLC PDU，目前先定为LLC PDU在一块TTF MEM中) * 32Byte(sizeof(LLC_PDU_SEG_CIPHER_PARA_STRU)) = 32Byte*/
#define TTF_LL_CIPHER_PARA_NODE_LEN     (1*1024)

/* 存放CK的内存起始地址 */
#define TTF_LL_CIPHER_KEY_ADDR(BaseAddr)\
           (TTF_LL_CIPHER_PARA_NODE_ADDR(BaseAddr) + TTF_LL_CIPHER_PARA_NODE_LEN)

/* 密钥为1组，为16字节 */
#define TTF_CIPHER_KEY_LEN              (1*16)

/* 存放计算出的加解密序列的内存 */
#define TTF_LL_CIPHER_ARRAY_ADDR(BaseAddr)\
           (TTF_LL_CIPHER_KEY_ADDR(BaseAddr) + TTF_CIPHER_KEY_LEN)

/* 加解密序列最长为1560Bytes*/
#define TTF_CIPHER_KEY_ARRAY_LEN        (1560)

/* Modem1的GSM加解密使用的内存 */
#define I1_TTF_LL_CIPHER_PARA_NODE_ADDR(BaseAddr) \
            (TTF_LL_CIPHER_ARRAY_ADDR(BaseAddr) + TTF_CIPHER_KEY_ARRAY_LEN)

/* 存放CK的内存起始地址 */
#define I1_TTF_LL_CIPHER_KEY_ADDR(BaseAddr)\
            (I1_TTF_LL_CIPHER_PARA_NODE_ADDR(BaseAddr) + TTF_LL_CIPHER_PARA_NODE_LEN)

/* 存放计算出的加解密序列的内存 */
#define I1_TTF_LL_CIPHER_ARRAY_ADDR(BaseAddr)\
            (I1_TTF_LL_CIPHER_KEY_ADDR(BaseAddr) + TTF_CIPHER_KEY_LEN)

/* Modem0的G模LLC MASTER设备使用的内存总长度 */
#define I0_TTF_LL_CIPHER_MASTER_LINK_LEN   \
            (TTF_LL_CIPHER_PARA_NODE_LEN + TTF_CIPHER_KEY_LEN + TTF_CIPHER_KEY_ARRAY_LEN)

/* Modem1的G模LLC MASTER设备使用的内存总长度 */
#define I1_TTF_LL_CIPHER_MASTER_LINK_LEN   \
            (TTF_LL_CIPHER_PARA_NODE_LEN + TTF_CIPHER_KEY_LEN + TTF_CIPHER_KEY_ARRAY_LEN)

/* 两个G模LLC MASTER设备使用的内存总长度 */
#define TTF_LL_CIPHER_MASTER_LINK_LEN   \
            (I0_TTF_LL_CIPHER_MASTER_LINK_LEN + I1_TTF_LL_CIPHER_MASTER_LINK_LEN )

/*
    TTFMem.c中的g_ulTtfActiveMemTotalLen记录了各平台中ECS_TTF_SIZE中实际使用的总大小
    当修改加密参数内存或内存池大小时，请使用COMM_CODE_GU/Config目录对应平台的FeatureXXX.h文件，
    覆盖COMM_CODE_GU\Balong_GU_Inc\win32目录下的文件，然后运行UT工程，获得g_ulTtfActiveMemTotalLen
    确保g_ulTtfActiveMemTotalLen不超过MemoryMap.h中底软划分的ECS_TTF_SIZE
*/
#define TTF_PARA_MEM_TOTAL_LEN    \
            (TTF_GET_32BYTE_ALIGN_VALUE(TTF_CIPHER_MASTER_LINK_LEN + TTF_LL_CIPHER_MASTER_LINK_LEN))


#if 0
/* CIPHER Master设备使用内存尾地址 */
#define TTF_CIPHER_MASTER_END_ADDR(BaseAddr)\
           (I1_TTF_LL_CIPHER_ARRAY_ADDR(BaseAddr) + TTF_CIPHER_KEY_ARRAY_LEN)
#endif

/* ----------------CICOM设备内存 END---------------- */


/* ----------------BBPMASTER新增内存 START---------------- */
#if 0
/* ----------------BBPMASTER新增内存 START---------------- */
/* BBP Master使用内存基地址 */
#define TTF_BBP_MASTER_START_ADDR(BaseAddr)                 (TTF_GET_32BYTE_ALIGN_VALUE(TTF_CIPHER_MASTER_END_ADDR(BaseAddr)))
#endif

/* 与硬件的规格约定,片上SRAM头需要64 WORD详细结构如上表。按照V9R1芯片规格，TB Head大小由256字节裁剪为128字节 */
#define WTTF_MAC_HSDPA_TB_HEAD_SIZE             (128) /* 32 WORD = 128 Byte */

/* 单位:字节,HSPA+能力上报为等级14，
   对应协议上定义的MAC-EHS PDU最大为42192 bit,即5274个字节
   考虑8字节的整数倍，取5280个字节,也即1320字(32bit)
   由于采用硬件解码，所有存储TB块数据需增加额外的空间，按照WTTFPHY_MAC_EHS_SDU_4BYTE_LEN计算，为5408字节
*/
#define WTTF_MAC_HSDPA_TB_DATA_SIZE             (5408)

#if (FEATURE_ON == FEATURE_BBP_MASTER)
/* BBPMASTER DATA BUFF LENTH (与WTTFPHY_MAC_HSDPA_TB_DATA_SIZE一致) */
#define TTF_BBPMST_BUFF_TOTAL_LEN               (WTTF_MAC_HSDPA_TB_DATA_SIZE)
#else
#define TTF_BBPMST_BUFF_TOTAL_LEN               (0)
#endif

#if 0
/* HDLC Master设备使用内存尾地址 */
#define TTF_BBP_MASTER_END_ADDR(BaseAddr)\
           (TTF_CIPHER_MASTER_END_ADDR(BaseAddr) + TTF_BBPMST_BUFF_LEN)
#endif
/* ----------------BBPMASTER新增内存 END---------------- */


/* ----------------A CPU 不可cache内存 START---------------- */
/* 控制内存池块数 */
#if ( FEATURE_ON == FEATURE_LTE )
/* LTE所需内存总长度:   WithWifi      12,854,528 字节
                        WithOutWifi   12,620,608 字节 */
/* ==================================================== */
/* IMM MEM 控制内存池块数 */
#define IMM_MEM_CTRL_MEM_CNT                (2103)

/*******************************************************************************
const IMM_MEM_CLUSTER_CFG_INFO_STRU   g_astImmMemSharePoolClusterTableInfo[] =
{
        {864,                           1427},
#if(FEATURE_WIFI == FEATURE_ON )
        {1952,                          1887}
#else
        {1888,                          1887}
#endif
};

内存池总长度: WithWifi      4,826,240字节
              WithOutWifi   4,707,520字节
*******************************************************************************/
/* IMM MEM 共享内存各档位内存块大小 */
#define IMM_MEM_SHARE_POOL_SIZE_CLUSTER_0           (864)
#if(FEATURE_WIFI == FEATURE_ON)
#define IMM_MEM_SHARE_POOL_SIZE_CLUSTER_1           (1952)
#else
#define IMM_MEM_SHARE_POOL_SIZE_CLUSTER_1           (1888)
#endif

/* IMM MEM 共享内存各档位内存块数 */
#define IMM_MEM_SHARE_POOL_CNT_CLUSTER_0            (1427)
#define IMM_MEM_SHARE_POOL_CNT_CLUSTER_1            (1887)

/* IMM MEM 共享内存总块数 */
#define IMM_MEM_SHARE_POOL_MEM_CNT                  (IMM_MEM_SHARE_POOL_CNT_CLUSTER_0 + IMM_MEM_SHARE_POOL_CNT_CLUSTER_1)

/* IMM MEM 共享内存总大小 */
#define IMM_MEM_SHARE_POOL_TOTAL_LEN                ((IMM_MEM_SHARE_POOL_SIZE_CLUSTER_0 * IMM_MEM_SHARE_POOL_CNT_CLUSTER_0) + \
                                                     (IMM_MEM_SHARE_POOL_SIZE_CLUSTER_1 * IMM_MEM_SHARE_POOL_CNT_CLUSTER_1))

/* TTF MEM 控制内存池块数 */
#define TTF_MEM_CTRL_MEM_CNT                        (IMM_MEM_SHARE_POOL_MEM_CNT)

#else

/* LTE所需内存总长度:   WithWifi      9,789,888 字节
                        WithOutWifi   9,656,448 字节 */
/* IMM MEM 控制内存池块数 */
#define IMM_MEM_CTRL_MEM_CNT                        (533)

/*******************************************************************************
const IMM_MEM_CLUSTER_CFG_INFO_STRU    g_astImmMemSharePoolClusterTableInfo[] =
{
        {864,                           1427},
#if(FEATURE_WIFI == FEATURE_ON )
        {1952,                          1887}
#else
        {1888,                          1887}
#endif
};


内存池总长度: WithWifi      4,826,240字节
              WithOutWifi   4,707,520字节
*******************************************************************************/
/* IMM MEM 共享内存各档位内存块大小 */
#define IMM_MEM_SHARE_POOL_SIZE_CLUSTER_0           (864)
#if(FEATURE_WIFI == FEATURE_ON)
#define IMM_MEM_SHARE_POOL_SIZE_CLUSTER_1           (1952)
#else
#define IMM_MEM_SHARE_POOL_SIZE_CLUSTER_1           (1888)
#endif

/* IMM MEM 共享内存各档位内存块数 */
#define IMM_MEM_SHARE_POOL_CNT_CLUSTER_0            (1510)
#define IMM_MEM_SHARE_POOL_CNT_CLUSTER_1            (1440)

/* IMM MEM 共享内存总块数 */
#define IMM_MEM_SHARE_POOL_MEM_CNT                  (IMM_MEM_SHARE_POOL_CNT_CLUSTER_0 + IMM_MEM_SHARE_POOL_CNT_CLUSTER_1)

/* IMM MEM 共享内存总大小 */
#define IMM_MEM_SHARE_POOL_TOTAL_LEN                ((IMM_MEM_SHARE_POOL_SIZE_CLUSTER_0 * IMM_MEM_SHARE_POOL_CNT_CLUSTER_0) + \
                                                     (IMM_MEM_SHARE_POOL_SIZE_CLUSTER_1 * IMM_MEM_SHARE_POOL_CNT_CLUSTER_1))

/* TTF MEM 控制内存池块数 */
#define TTF_MEM_CTRL_MEM_CNT                        (IMM_MEM_SHARE_POOL_MEM_CNT)
#endif

#if 0
#if (FEATURE_ON == FEATURE_BBP_MASTER)
/* A CPU 不可cache内存基地址 */
#define TTF_ACORE_POOL_MEM_START_ADDR(BaseAddr) (TTF_GET_32BYTE_ALIGN_VALUE(TTF_BBP_MASTER_END_ADDR(BaseAddr)))
#else
/* A CPU 不可cache内存基地址 */
#define TTF_ACORE_POOL_MEM_START_ADDR(BaseAddr) (TTF_GET_32BYTE_ALIGN_VALUE(TTF_CIPHER_MASTER_END_ADDR(BaseAddr)))
#endif
#endif

/* IMM MEM 所有内存所需控制头总块数*/
#define IMM_MEM_CTRL_MEM_TOTAL_CNT              (IMM_MEM_SHARE_POOL_MEM_CNT + IMM_MEM_CTRL_MEM_CNT)

/* A CPU IMM内存池基地址 */
#define IMM_MEM_POOL_BASE_ADDR(BaseAddr)        ((TTF_ACORE_POOL_MEM_START_ADDR(BaseAddr)))

/* A CPU 共享内存池起始地址 */
#define IMM_SHARE_MEM_POOL_ADDR(BaseAddr)       (IMM_MEM_POOL_BASE_ADDR(BaseAddr))

#if (FEATURE_ON == FEATURE_SKB_EXP)   /* V3R3版本 */
/* A CPU 所有内存池总大小 */
#define TTF_ACORE_POOL_MEM_TOTAL_LEN            (TTF_GET_32BYTE_ALIGN_VALUE(IMM_MEM_SHARE_POOL_TOTAL_LEN))
#else
#define TTF_ACORE_POOL_MEM_TOTAL_LEN            (0)
#endif


#if 0
/* A CPU 不可cache内存尾地址 */
#define TTF_ACORE_POOL_MEM_END_ADDR(BaseAddr)   (TTF_ACORE_POOL_MEM_START_ADDR(BaseAddr) + TTF_ACORE_POOL_MEM_TOTAL_LEN)

/* C CPU 不可cache内存基地址 */
#define TTF_CCORE_POOL_MEM_START_ADDR(BaseAddr) (TTF_GET_32BYTE_ALIGN_VALUE(TTF_ACORE_POOL_MEM_END_ADDR(BaseAddr)))
#endif



/* 用户面融合内存释放共享队列内存分配 */
#define TTF_MEM_FREE_AREA_SIZE                  (16*1024)
#define TTF_MEM_FREE_QUE_LEN                    (4000)

#define IMM_MEM_FREE_AREA_SIZE                  (16*1024)
#define IMM_MEM_FREE_QUE_LEN                    (4000)

#if 0
/* 调用TTF_GET_32BYTE_ALIGN_VALUE可能导致内存地址往后最多偏31字节，但是又没有计算在长度中
   在计算C核内存池基地前共调用了三次，分别在TTF_HDLC_DEF_INPUT_PARA_LINK_BUF_ADDR、
   TTF_ACORE_POOL_MEM_START_ADDR、TTF_CCORE_POOL_MEM_START_ADDR */
#define TTF_MEM_32BYTE_ALIGN_PADDING_LEN       (3*32)
#endif

/* ----------------A CPU 不可cache内存 END---------------- */


/* ----------------C CPU 不可cache内存 START--------------- */

/* C CPU TTF MEM内存池基地址 */
#define TTF_MEM_POOL_BASE_ADDR(BaseAddr)        (TTF_CCORE_POOL_MEM_START_ADDR(BaseAddr))

#define TTF_CCORE_POOL_MEM_TOTAL_LEN            (12035808) /* C-CPU 内存池总长度，32字节对齐 */

/* TTF内存池大小,参照V7R2 memoryMap.h，定义在TtfMemoryMap.h  */
#define TTF_MEM_POOL_SIZE                       (0x00860000)

/* ----------------C CPU 不可cache内存 END---------------- */


/*****************************************************************************
  3 枚举定义
*****************************************************************************/
enum TTF_MEMCTRL_ADDR_TYPE_ENUM
{
    TTF_MEMCTRL_HDLC_MASTER_ADDR_TYPE   = 0,
    TTF_MEMCTRL_CIPHER_MASTER_ADDR_TYPE,
    TTF_MEMCTRL_BBP_MASTER_ADDR_TYPE,
    TTF_MEMCTRL_ACORE_POOL_ADDR_TYPE,
    TTF_MEMCTRL_CCORE_POOL_ADDR_TYPE,
    TTF_MEMCTRL_ADDR_TYPE_BUTT
};
typedef unsigned int TTF_MEMCTRL_ADDR_TYPE_ENUM_UINT32;

/*****************************************************************************
  8 UNION定义
*****************************************************************************/


/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/



/*****************************************************************************
  10 函数声明
*****************************************************************************/
extern VOS_UINT32       TTF_MEMCTRL_ACORE_AddrInit(VOS_VOID);
extern VOS_UINT32       TTF_MEMCTRL_CCORE_AddrInit(VOS_VOID);
extern VOS_UINT32       TTF_MEMCTRL_GetOffsetBaseLen(VOS_UINT32  ulType);
extern VOS_UINT_PTR     TTF_MEMCTRL_GetTypeAddr(VOS_UINT_PTR ulAddr, VOS_UINT32 ulType);
extern VOS_UINT32       TTF_MEMCTRL_DynamicAddrInit(VOS_VOID);
extern VOS_VOID         TTF_MEMCTRL_CheckTTFDynamicMem(VOS_UINT32 ulPid);
extern VOS_UINT_PTR     TTF_MEMCTRL_GetTtfInternalMemBaseAddr(VOS_VOID);
extern VOS_VOID         TTF_MEMCTRL_AddrPrint(VOS_VOID);

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of TtfMemoryMap.h */

