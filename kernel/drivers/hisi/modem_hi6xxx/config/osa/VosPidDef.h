/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : VosPidDef.h
  版 本 号   : 初稿
  作    者   : x51137
  生成日期   : 2011年7月26日
  最近修改   :
  功能描述   : 公共头文件, 定义VOS全系统的PID
  函数列表   :
  修改历史   :
  1.日    期   : 2011年7月26日
    作    者   : x51137
    修改内容   : 创建文件

******************************************************************************/

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#ifndef _VOS_PID_DEF_H
#define _VOS_PID_DEF_H

#include "product_config.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */

/*****************************************************************************
  2 宏定义
*****************************************************************************/
/* 支持互相发送消息的多核的最大数目 */
#define VOS_SUPPORT_CPU_NUM_MAX     (5)

/* CPU0 系统PID定义结束标志 */
#define VOS_PID_CPU_ID_0_DOPRAEND   (20)

/* macro defined for PID management */
#define BEGIN_CPU_ID_0_PID_DEFINITION() \
    enum VOS_CPU_ID_0_Process_ID \
    { \
        VOS_CPU_ID_0_PID_RESERVED = VOS_PID_CPU_ID_0_DOPRAEND - 1,

#define END_CPU_ID_0_PID_DEFINITION() \
        VOS_CPU_ID_0_PID_BUTT \
    };

/* CPU1 系统PID定义结束标志 */
#define VOS_PID_CPU_ID_1_DOPRAEND   (256)

/* macro defined for PID management */
#define BEGIN_CPU_ID_1_PID_DEFINITION() \
    enum VOS_CPU_ID_1_Process_ID \
    { \
        VOS_CPU_ID_1_PID_RESERVED = VOS_PID_CPU_ID_1_DOPRAEND - 1,

#define END_CPU_ID_1_PID_DEFINITION() \
        VOS_CPU_ID_1_PID_BUTT \
    };

/* CPU2 系统PID定义结束标志 */
#define VOS_PID_CPU_ID_2_DOPRAEND   (512)


/* macro defined for PID management */
#define BEGIN_CPU_ID_2_PID_DEFINITION() \
    enum VOS_CPU_ID_2_Process_ID \
    { \
        VOS_CPU_ID_2_PID_RESERVED = VOS_PID_CPU_ID_2_DOPRAEND - 1,

#define END_CPU_ID_2_PID_DEFINITION() \
        VOS_CPU_ID_2_PID_BUTT \
    };

/* CPU3 系统PID定义结束标志 */
#define VOS_PID_CPU_ID_3_DOPRAEND   (768)


/* macro defined for PID management */
#define BEGIN_CPU_ID_3_PID_DEFINITION() \
    enum VOS_CPU_ID_3_Process_ID \
    { \
        VOS_CPU_ID_3_PID_RESERVED = VOS_PID_CPU_ID_3_DOPRAEND - 1,

#define END_CPU_ID_3_PID_DEFINITION() \
        VOS_CPU_ID_3_PID_BUTT \
    };

/* CPU4 系统PID定义结束标志 */
#define VOS_PID_CPU_ID_4_DOPRAEND   (1024)


/* macro defined for PID management */
#define BEGIN_CPU_ID_4_PID_DEFINITION() \
    enum VOS_CPU_ID_4_Process_ID \
    { \
        VOS_CPU_ID_4_PID_RESERVED = VOS_PID_CPU_ID_4_DOPRAEND - 1,

#define END_CPU_ID_4_PID_DEFINITION() \
        VOS_CPU_ID_4_PID_BUTT \
    };

/* CPU5 系统PID定义结束标志 */
#define VOS_PID_CPU_ID_5_DOPRAEND   (1280)

/* macro defined for PID management */
#define BEGIN_CPU_ID_5_PID_DEFINITION() \
    enum VOS_CPU_ID_5_Process_ID \
    { \
        VOS_CPU_ID_5_PID_RESERVED = VOS_PID_CPU_ID_5_DOPRAEND - 1,

#define END_CPU_ID_5_PID_DEFINITION() \
        VOS_CPU_ID_5_PID_BUTT \
    };

/* 注意: 从0x8000开始的PID号已经归外部模块使用 */

/* 具体申明一个PID */
#define DEFINE_PID(pid) pid,

/* 根据PID 得到 CPU ID */
#define VOS_GetCpuId(pid)      (((pid)>>0x08)&0x0F)

/* macro defined for PID management */
#define END_DECLARE_PID_BELONG_TO_FID() \
    }

/*****************************************************************************
  3 枚举定义
*****************************************************************************/

#if  ( FEATURE_MULTI_MODEM == FEATURE_ON )
enum MODEM_ID_ENUM
{
    MODEM_ID_0 = 0,
    MODEM_ID_1,
    MODEM_ID_BUTT
};
typedef VOS_UINT16 MODEM_ID_ENUM_UINT16;
#else
enum MODEM_ID_ENUM
{
    MODEM_ID_0 = 0,
    MODEM_ID_BUTT,
    MODEM_ID_1
};
typedef VOS_UINT16 MODEM_ID_ENUM_UINT16;
#endif


/*****************************************************************************
 枚举名    :VOS_RATMODE_ENUM_UINT32
 协议表格  :
 ASN.1描述 :
 枚举说明  :接入模式定义
            注意；双卡双待的枚举值定义与协议栈和DRV协商为GULT;
            需要和DrvInterface.h中的PWC_COMM_MODE_E保持一致

*****************************************************************************/
enum VOS_RATMODE_ENUM
{
    VOS_RATMODE_GSM                         = 0,
    VOS_RATMODE_WCDMA                       = 1,
    VOS_RATMODE_LTE                         = 2,
    VOS_RATMODE_TDS                         = 3,
    VOS_RATMODE_1X                          = 4,
    VOS_RATMODE_EVDO                        = 5,
    VOS_RATMODE_BUTT
};
typedef VOS_UINT32 VOS_RATMODE_ENUM_UINT32;


/* CPU ID 的定义 0 是CCPU 1 是ACPU 2 是ZSP500,GU DSP 3 是Hifi,Med */
enum VOS_CPU_ID_ENUM
{
    VOS_CPU_ID_CCPU = 0,
    VOS_CPU_ID_ACPU,
    VOS_CPU_ID_GUDSP,
    VOS_CPU_ID_MEDDSP,
    VOS_CPU_ID_MCU,
    VOS_CPU_ID_BBE16,
    VOS_CPU_ID_BUTT
};

BEGIN_CPU_ID_0_PID_DEFINITION()

/*卡1的PID定义*/
    DEFINE_PID(I1_WUEPS_PID_USIM)       /*20*/
    DEFINE_PID(I1_MAPS_STK_PID)
    DEFINE_PID(I1_MAPS_PIH_PID)
    DEFINE_PID(I1_MAPS_PB_PID)
    DEFINE_PID(I1_UEPS_PID_GRM)
    DEFINE_PID(I1_UEPS_PID_DL)
    DEFINE_PID(I1_UEPS_PID_LL)
    DEFINE_PID(I1_UEPS_PID_SN)
    DEFINE_PID(I1_UEPS_PID_GAS)
    DEFINE_PID(I1_WUEPS_PID_MM)
    DEFINE_PID(I1_WUEPS_PID_MMC)
    DEFINE_PID(I1_WUEPS_PID_GMM)
    DEFINE_PID(I1_WUEPS_PID_MMA)
    DEFINE_PID(I1_WUEPS_PID_CC)
    DEFINE_PID(I1_WUEPS_PID_SS)
    DEFINE_PID(I1_WUEPS_PID_TC)
    DEFINE_PID(I1_WUEPS_PID_SMS)
    DEFINE_PID(I1_WUEPS_PID_RABM)
    DEFINE_PID(I1_WUEPS_PID_SM)
    DEFINE_PID(I1_WUEPS_PID_ADMIN)
    DEFINE_PID(I1_WUEPS_PID_TAF)
    DEFINE_PID(I1_WUEPS_PID_VC)
    DEFINE_PID(I1_WUEPS_PID_DRV_AGENT)
    DEFINE_PID(I1_UEPS_PID_MTA)
    DEFINE_PID(I1_DSP_PID_GPHY)
    DEFINE_PID(I1_DSP_PID_DRX)
    DEFINE_PID(I1_DSP_PID_IDLE)
    DEFINE_PID(I1_DSP_PID_APM )
    DEFINE_PID(I1_WUEPS_PID_SLEEP)

/* 卡2的PID定义不能超过98 */
    DEFINE_PID(PC_PID_TOOLRFU = 99)               /* = 99  工具预留 */
    DEFINE_PID(PC_PID_TOOL = 100)                 /* = 100 工具预留 */

/*卡0的PID定义*/
    DEFINE_PID(I0_WUEPS_PID_USIM)           /* = 101 */
    DEFINE_PID(I0_MAPS_STK_PID)             /* = 102 */
    DEFINE_PID(I0_MAPS_PIH_PID)             /* = 103 */
    DEFINE_PID(I0_MAPS_PB_PID)              /* = 104 */
    DEFINE_PID(WUEPS_PID_OM)                /* = 105 */
    DEFINE_PID(NVIM_WRITE_DATA_PID)         /* = 106 */
    DEFINE_PID(WUEPS_PID_AGING)             /* = 107 */
    DEFINE_PID(WUEPS_PID_DIAG)              /* = 108 */
    DEFINE_PID(WUEPS_PID_RFA)               /* = 109 */
    DEFINE_PID(WUEPS_PID_REG)               /* = 110 */
    DEFINE_PID(UEPS_PID_BASTET)             /* = 111 */
    DEFINE_PID(PID_RESERVED_0)              /* Reserved */
    DEFINE_PID(MSP_PID_DIAG_AGENT)          /* = 113 */
    DEFINE_PID(PID_RESERVED_1)              /* Reserved */
    DEFINE_PID(MSP_L4_L4A_PID)              /* = 115 */
    DEFINE_PID(MSP_L4_AT_FW_PID)            /* = 116 */
    DEFINE_PID(MSP_SYS_FTM_PID)             /* = 117 */
    DEFINE_PID(PID_RESERVED_2)              /* Reserved */
    DEFINE_PID(MSP_LDSP_PID)                /* = 119 */
    DEFINE_PID(MSP_L4_AT_LTE_PID)           /* = 120 */
    DEFINE_PID(MSP_L4_AT_GU_PID)            /* = 121 */
    DEFINE_PID(UEPS_PID_GHPA)               /* = 122 */
    DEFINE_PID(WUEPS_PID_HPA)               /* = 123 */
    DEFINE_PID(UEPS_PID_SHPA)               /* = 124 */
    DEFINE_PID(UEPS_PID_GHPA_NPP)           /* = 125 */
    DEFINE_PID(UEPS_PID_APM)                /* = 126 */
    DEFINE_PID(WUEPS_PID_OM_CALLBACK)       /* = 127 */
    DEFINE_PID(I0_UEPS_PID_GAS)             /* = 128 */
    DEFINE_PID(I0_UEPS_PID_DL)              /* = 129 */
    DEFINE_PID(I0_UEPS_PID_GRM)             /* = 130 */
    DEFINE_PID(I0_UEPS_PID_LL)              /* = 131 */
    DEFINE_PID(I0_UEPS_PID_SN)              /* = 132 */
    DEFINE_PID(WUEPS_PID_WRR)               /* = 133 */
    DEFINE_PID(WUEPS_PID_WCOM)              /* = 134 */
    DEFINE_PID(UEPS_PID_CST)                /* = 135 */
    DEFINE_PID(WUEPS_PID_BMC)               /* = 136 */
    DEFINE_PID(WUEPS_PID_PDCP)              /* = 137 */
    DEFINE_PID(WUEPS_PID_RLC)               /* = 138 */
    DEFINE_PID(WUEPS_PID_MAC)               /* = 139 */
    DEFINE_PID(I0_WUEPS_PID_RABM)           /* = 140 */
    DEFINE_PID(I0_WUEPS_PID_MMC)            /* = 141 */
    DEFINE_PID(I0_WUEPS_PID_MM )            /* = 142 */
    DEFINE_PID(I0_WUEPS_PID_GMM)            /* = 143 */
    DEFINE_PID(I0_WUEPS_PID_CC)             /* = 144 */
    DEFINE_PID(I0_WUEPS_PID_SM)             /* = 145 */
    DEFINE_PID(I0_WUEPS_PID_SMS)            /* = 146 */
    DEFINE_PID(I0_WUEPS_PID_SS)             /* = 147 */
    DEFINE_PID(I0_WUEPS_PID_TC)             /* = 148 */
    DEFINE_PID(WUEPS_PID_SMT)               /* = 149 */
    DEFINE_PID(I0_WUEPS_PID_ADMIN)          /* = 150 */
    DEFINE_PID(WUEPS_PID_MUX)               /* = 151 */
    DEFINE_PID(I0_WUEPS_PID_TAF)            /* = 152 */
    DEFINE_PID(WUEPS_PID_PPP)               /* = 153 */
    DEFINE_PID(WUEAPP_PID_MUX)              /* = 154 */
    DEFINE_PID(PID_RESERVED_3)              /* Reserved */
    DEFINE_PID(PID_RESERVED_4)              /* Reserved */
    DEFINE_PID(I0_WUEPS_PID_MMA)            /* = 157 */
    DEFINE_PID(I0_WUEPS_PID_SLEEP)          /* = 158 */
    DEFINE_PID(WUEPS_PID_CBS)               /* = 159 */
    DEFINE_PID(WUEPS_PID_MONITOR)           /* = 160 */
    DEFINE_PID(PID_RESERVED_5)              /* Reserved */
    DEFINE_PID(PID_RESERVED_6)              /* Reserved */
    DEFINE_PID(WUEPS_PID_APCID1)            /* = 163 */
    DEFINE_PID(WUEPS_PID_SPY)               /* = 164 */
    DEFINE_PID(I0_WUEPS_PID_VC)             /* = 165 */
    DEFINE_PID(PID_RESERVED_7)              /* Reserved */
    DEFINE_PID(WUEPS_PID_APCID2)            /* = 167 */
    DEFINE_PID(WUEPS_PID_APCID3)            /* = 168 */
    DEFINE_PID(PID_RESERVED_8)              /* Reserved */
    DEFINE_PID(PID_RESERVED_9)              /* Reserved */
    DEFINE_PID(PS_PID_ERRC)                 /* = 171 */
    DEFINE_PID(PS_PID_ERMM)                 /* = 172 */
    DEFINE_PID(PS_PID_MM)                   /* = 173 */
    DEFINE_PID(PS_PID_ESM)                  /* = 174 */
    DEFINE_PID(PS_PID_RABM)                 /* = 175 */
    DEFINE_PID(PS_PID_TC)                   /* = 176 */
    DEFINE_PID(PS_PID_IP)                   /* = 177 */
    DEFINE_PID(PS_PID_OM)                   /* = 178 */
    DEFINE_PID(PS_PID_MAC_UL)               /* = 179 */
    DEFINE_PID(PS_PID_RLC_UL)               /* = 180 */
    DEFINE_PID(PS_PID_MAC_DL)               /* = 181 */
    DEFINE_PID(PS_PID_RLC_DL)               /* = 182 */
    DEFINE_PID(PS_PID_PDCP_UL)              /* = 183 */
    DEFINE_PID(PS_PID_PDCP_DL)              /* = 184 */
    DEFINE_PID(PS_PID_HPA)                  /* = 185 */
    DEFINE_PID(PS_PID_MUX)                  /* = 186 */
    DEFINE_PID(PS_PID_MUXOM)                /* = 187 */
    DEFINE_PID(CCPU_PID_OMAGENT)            /* = 188 */
    DEFINE_PID(UEPS_PID_FLOWCTRL_C)         /* = 189 */
    DEFINE_PID(WUEPS_PID_MACDL)             /* = 190 */
    DEFINE_PID(WUEPS_PID_RLCDL)             /* = 191 */
    DEFINE_PID(WUEPS_PID_NOSIG)             /* = 192 */
    DEFINE_PID(UEPS_PID_DICC_C)             /* = 193 */
    /* Added by L60609 for AT Project，2011-10-03,  Begin*/
    DEFINE_PID(I0_WUEPS_PID_DRV_AGENT)      /* = 194 */
    /* Added by L60609 for AT Project，2011-10-03,  End*/
    /* added by hujianbo for PTT 20111201 begin*/
    DEFINE_PID(PS_PID_PTT)                  /* = 195 */
    /* added by hujianbo for PTT 20111201 end*/
    DEFINE_PID(UEPS_PID_TTF_MEM_RB_FREE)    /* = 196 */
    DEFINE_PID(UEPS_PID_CDS)                /* = 197 */
    DEFINE_PID(UEPS_PID_NDCLIENT)           /* = 198 */
    DEFINE_PID(WUEPS_PID_SAR)               /* = 199 */
    DEFINE_PID(PID_RESERVED_A)              /* Reserved */
    /* DSP 上移 */
    DEFINE_PID(I0_DSP_PID_APM)              /* = 201 */
    DEFINE_PID(DSP_PID_UPA)                 /* = 202 */
    DEFINE_PID(DSP_PID_WPHY)                /* = 203 */
    DEFINE_PID(DSP_PID_WMEAS)               /* = 204 */
    DEFINE_PID(DSP_PID_WBUTT = DSP_PID_WMEAS)
    DEFINE_PID(I0_DSP_PID_GPHY)             /* = 205 */
    DEFINE_PID(PID_RESERVED_B)              /* Reserved */
    DEFINE_PID(I0_DSP_PID_DRX)              /* = 207 */
    DEFINE_PID(I0_DSP_PID_IDLE)             /* = 208 */
    /* DSP 上移 end */
    DEFINE_PID(I0_UEPS_PID_MTA)             /* = 209 */
    DEFINE_PID(WUEPS_PID_NVIM_FLUSH)        /* = 210 */
    /* TDS begin */
    DEFINE_PID(HL1_PID_SNDCMD)              /* = 211 */
    DEFINE_PID(PID_RESERVED_C)              /* Reserved */
    DEFINE_PID(PID_RESERVED_D)              /* Reserved */
    DEFINE_PID(TPS_PID_RLC)                 /* = 214 */
    DEFINE_PID(TPS_PID_MAC)                 /* = 215 */
    DEFINE_PID(TPS_PID_PDC)                 /* = 216 */
    /* TDS end */
    DEFINE_PID(TPS_PID_RRC)                 /* = 217 */
    DEFINE_PID(MSP_PID_BBP_AGENT)           /* = 218 */
    DEFINE_PID(UEPS_PID_MTC)                /* = 219 */
    DEFINE_PID(WUEPS_PID_CBPCA)             /* = 220 */
    DEFINE_PID(WUEPS_PID_CSIMA)             /* = 221 */
    /*added by sunbing for VoLTE 20130715 begin*/
    DEFINE_PID(PS_PID_IMSA)                 /* = 222 */
    DEFINE_PID(PS_PID_IMSVA)                /* = 223 */
    /*added by sunbing for VoLTE 20130715 end*/
    /* add for VoLTE start */
    DEFINE_PID(UEPS_PID_IPS_CCORE)          /* = 224 */
    /* add for VoLTE end */
    DEFINE_PID(UEPS_PID_RRM)                /* = 225 */
    /* Modified by l60609 for L-C互操作项目, 2014-01-23, Begin */
    DEFINE_PID(WUEPS_PID_CMMCA)             /* = 226 */
    /* Modified by l60609 for L-C互操作项目, 2014-01-23, End */
    DEFINE_PID(WUEPS_PID_OM_BBP_MASTER)     /* = 227 */
    DEFINE_PID(DSP_PID_RCM)                 /* = 228 */
    DEFINE_PID(DSP_PID_STARTUP)             /* = 229 */
    DEFINE_PID(MSP_PID_DRX)                 /* = 230 */
    /*add by fuxin00221597 for volte*/
    DEFINE_PID(VOLTE_PID_DIAG)              /* = 231 */
    DEFINE_PID(MSPS_PID_1X_RLAC)            /* = 232 */
    DEFINE_PID(MSPS_PID_1X_RMAC)            /* = 233 */
    DEFINE_PID(MSPS_PID_1X_RRLP)            /* = 234 */
    DEFINE_PID(MSPS_PID_1X_MARKOV)          /* = 235 */
    DEFINE_PID(MSPS_PID_1X_LOOPBACK)        /* = 236 */
    DEFINE_PID(MSPS_PID_1X_FLAC)            /* = 237 */
    DEFINE_PID(MSPS_PID_1X_FMAC)            /* = 238 */
    DEFINE_PID(MSPS_PID_1X_FRLP)            /* = 239 */
    /* y58851 csdr 1x begin */
    DEFINE_PID(DSP_PID_PROCSTUB)            /* = 240 */
    DEFINE_PID(CPROC_PID_1X_CM)             /* = 241 */
    DEFINE_PID(CPROC_PID_1X_SM)             /* = 242 */
    DEFINE_PID(CPROC_PID_1X_DSP)            /* = 243 */
    DEFINE_PID(CPROC_PID_EVDO)              /* = 244 */
    /* y58851 csdr 1x end */
    DEFINE_PID(UEPS_PID_1XCASM)             /* = 245 */
    DEFINE_PID(UEPS_PID_1XCMEAS)            /* = 246 */
    DEFINE_PID(UEPS_PID_1XCSRCH)            /* = 247 */
    DEFINE_PID(UEPS_PID_XREG)               /* = 248 */
    DEFINE_PID(UEPS_PID_XSD)                /* = 249 */
    DEFINE_PID(UEPS_PID_XCC)                /* = 250 */
    DEFINE_PID(CCPU_PID_CBT)                /* = 251 */
    DEFINE_PID(CCPU_PID_PAM_OM)             /* = 252 */
    DEFINE_PID(DSP_PID_TLPHY)               /* = 253 */
    /* CCPU PID最大不能超过254,使用255以及255以上的PID会造成单板异常复位 */
/******************************************************************************
    上面是单板侧的PID
    下面是PC侧的PID
*******************************************************************************/

#if (VOS_WIN32 == VOS_OS_VER)
    DEFINE_PID(WUEPS_PID_ITTSTUB1)
    DEFINE_PID(WUEPS_PID_ITTSTUB2)
    DEFINE_PID(WUEPS_PID_WTTFTEST)
    DEFINE_PID(WUEPS_PID_OAMTEST)
    DEFINE_PID(WUEPS_PID_XMLDECODE)
#endif
#ifdef HPA_ITT
    DEFINE_PID(MAPS_STUB_RCVMSG_PID)
    DEFINE_PID(MAPS_STUB_SENDMSG_PID)
    DEFINE_PID(MAPS_STUB_GTRMSG_PROC_PID)
#endif

END_CPU_ID_0_PID_DEFINITION()

/* ACPU PID start */
BEGIN_CPU_ID_1_PID_DEFINITION()
    DEFINE_PID(MSP_APP_DS_MUX_PID)
    DEFINE_PID(PS_PID_APP_NDIS)
    DEFINE_PID(PS_PID_APP_PPP)
    DEFINE_PID(ACPU_PID_OM)
    DEFINE_PID(MSP_APP_DS_DIAG_PID)
    DEFINE_PID(ACPU_PID_PCSC)
    DEFINE_PID(ACPU_PID_OM_CALLBACK)
    DEFINE_PID(ACPU_PID_PCVOICE)
    DEFINE_PID(ACPU_PID_DIAG)
    DEFINE_PID(ACPU_PID_PB)
    DEFINE_PID(ACPU_PID_OMAGENT)
    DEFINE_PID(UEPS_PID_DICC_A)
    DEFINE_PID(ACPU_PID_RNIC)
    DEFINE_PID(WUEPS_PID_AT)
    DEFINE_PID(ACPU_PID_NFEXT)
    /* Added by L60609 for PS Project,2011-12-06,  Begin*/
    DEFINE_PID(ACPU_PID_ADS_UL)
    DEFINE_PID(ACPU_PID_ADS_DL)
    DEFINE_PID(ACPU_PID_CSD)
    DEFINE_PID(ACPU_PID_TAFAGENT)
    DEFINE_PID(UEPS_PID_FLOWCTRL_A)
    DEFINE_PID(ACPU_PID_CPULOAD)
    DEFINE_PID(UEPS_PID_IMM_RB_FREE)
    DEFINE_PID(UEPS_PID_NDSERVER)

    /*added by x59651 for balong V7R1 C50,20120221,begin*/
    DEFINE_PID(PS_PID_APP_DIPC)
    /*added by x59651 for balong V7R1 C50,20120221,end*/

    DEFINE_PID(PS_PID_APP_MUX)

    /* Voice in ACPU start */
    DEFINE_PID(DSP_PID_VOICE_RT_ACPU)
    DEFINE_PID(DSP_PID_VOICE_ACPU)
    /* Voice in ACPU end */
    DEFINE_PID(MSP_PID_DIAG_FW)
    DEFINE_PID(MSP_PID_DIAG_APP_AGENT)
    DEFINE_PID(ACPU_PID_CBT)
    DEFINE_PID(ACPU_PID_PAM_OM)
    DEFINE_PID(ACPU_PID_BASTET_COMM)
END_CPU_ID_1_PID_DEFINITION()
/* ACPU PID end */

/* GU DSP 的PID 定义 */
BEGIN_CPU_ID_2_PID_DEFINITION()

END_CPU_ID_2_PID_DEFINITION()



BEGIN_CPU_ID_3_PID_DEFINITION()
    DEFINE_PID(DSP_PID_VOICE_RESERVED)
    DEFINE_PID(DSP_PID_VOICE_RT_HIFI)
    DEFINE_PID(DSP_PID_VOICE_HIFI)
    DEFINE_PID(DSP_PID_AUDIO_RT)
    DEFINE_PID(DSP_PID_AUDIO)
    DEFINE_PID(DSP_PID_HIFI_OM)
    DEFINE_PID(DSP_PID_VOICEPP)
END_CPU_ID_3_PID_DEFINITION()

#if (FEATURE_VOICE_UP == FEATURE_ON)
/* 语音软件上移时, 使用ACPU的PID */
#define DSP_PID_VOICE_RT    DSP_PID_VOICE_RT_ACPU
#define DSP_PID_VOICE       DSP_PID_VOICE_ACPU
#else
/* 语音软件不上移时, 使用HIFI的PID */
#define DSP_PID_VOICE_RT    DSP_PID_VOICE_RT_HIFI
#define DSP_PID_VOICE       DSP_PID_VOICE_HIFI
#endif

/* MED HiFi的PID 定义，为保证其它组件编译通过，暂时保留MED_RT等PID */
#define DSP_PID_MED_RT  DSP_PID_VOICE_RT
#define DSP_PID_MED     DSP_PID_VOICE
#define DSP_PID_MED_OM  DSP_PID_HIFI_OM


/* MCU的PID 定义 */
BEGIN_CPU_ID_4_PID_DEFINITION()
    DEFINE_PID(ACPU_PID_MCU_OM)
END_CPU_ID_4_PID_DEFINITION()

/* BBE16的PID */
BEGIN_CPU_ID_5_PID_DEFINITION()
    DEFINE_PID(DSP_PID_TDPHY_RESERVED)
    DEFINE_PID(DSP_PID_TDPHY)
END_CPU_ID_5_PID_DEFINITION()


/*****************************************************************************
  4 消息头定义
*****************************************************************************/


/*****************************************************************************
  5 消息定义
*****************************************************************************/


/*****************************************************************************
  6 STRUCT定义
*****************************************************************************/


/*****************************************************************************
  7 UNION定义
*****************************************************************************/


/*****************************************************************************
  8 OTHERS定义
*****************************************************************************/
#if defined (INSTANCE_1)
#define WUEPS_PID_USIM  I1_WUEPS_PID_USIM
#define MAPS_STK_PID    I1_MAPS_STK_PID
#define MAPS_PIH_PID    I1_MAPS_PIH_PID
#define MAPS_PB_PID     I1_MAPS_PB_PID
#define WUEPS_PID_SLEEP I1_WUEPS_PID_SLEEP

#define UEPS_PID_DL     I1_UEPS_PID_DL
#define UEPS_PID_LL     I1_UEPS_PID_LL
#define UEPS_PID_SN     I1_UEPS_PID_SN
#define UEPS_PID_GRM    I1_UEPS_PID_GRM

#define DSP_PID_GPHY    I1_DSP_PID_GPHY
#define DSP_PID_DRX     I1_DSP_PID_DRX
#define DSP_PID_IDLE    I1_DSP_PID_IDLE
#define DSP_PID_APM     I1_DSP_PID_APM

#define UEPS_PID_GAS    I1_UEPS_PID_GAS

#define WUEPS_PID_MM    I1_WUEPS_PID_MM
#define WUEPS_PID_MMC   I1_WUEPS_PID_MMC
#define WUEPS_PID_GMM   I1_WUEPS_PID_GMM
#define WUEPS_PID_MMA   I1_WUEPS_PID_MMA
#define WUEPS_PID_CC    I1_WUEPS_PID_CC
#define WUEPS_PID_SS    I1_WUEPS_PID_SS
#define WUEPS_PID_TC    I1_WUEPS_PID_TC
#define WUEPS_PID_SMS   I1_WUEPS_PID_SMS
#define WUEPS_PID_RABM  I1_WUEPS_PID_RABM
#define WUEPS_PID_SM    I1_WUEPS_PID_SM
#define WUEPS_PID_ADMIN I1_WUEPS_PID_ADMIN
#define WUEPS_PID_TAF   I1_WUEPS_PID_TAF
#define WUEPS_PID_VC    I1_WUEPS_PID_VC
#define UEPS_PID_MTA    I1_UEPS_PID_MTA
#define WUEPS_PID_DRV_AGENT   I1_WUEPS_PID_DRV_AGENT
#else
#define WUEPS_PID_USIM  I0_WUEPS_PID_USIM
#define MAPS_STK_PID    I0_MAPS_STK_PID
#define MAPS_PIH_PID    I0_MAPS_PIH_PID
#define MAPS_PB_PID     I0_MAPS_PB_PID
#define WUEPS_PID_SLEEP I0_WUEPS_PID_SLEEP

#define UEPS_PID_GAS    I0_UEPS_PID_GAS

#define UEPS_PID_DL     I0_UEPS_PID_DL
#define UEPS_PID_LL     I0_UEPS_PID_LL
#define UEPS_PID_SN     I0_UEPS_PID_SN
#define UEPS_PID_GRM    I0_UEPS_PID_GRM

#define DSP_PID_GPHY    I0_DSP_PID_GPHY
#define DSP_PID_DRX     I0_DSP_PID_DRX
#define DSP_PID_IDLE    I0_DSP_PID_IDLE
#define DSP_PID_APM     I0_DSP_PID_APM

#define WUEPS_PID_MM    I0_WUEPS_PID_MM
#define WUEPS_PID_MMC   I0_WUEPS_PID_MMC
#define WUEPS_PID_GMM   I0_WUEPS_PID_GMM
#define WUEPS_PID_MMA   I0_WUEPS_PID_MMA
#define WUEPS_PID_CC    I0_WUEPS_PID_CC
#define WUEPS_PID_SS    I0_WUEPS_PID_SS
#define WUEPS_PID_TC    I0_WUEPS_PID_TC
#define WUEPS_PID_SMS   I0_WUEPS_PID_SMS
#define WUEPS_PID_RABM  I0_WUEPS_PID_RABM
#define WUEPS_PID_SM    I0_WUEPS_PID_SM
#define WUEPS_PID_ADMIN I0_WUEPS_PID_ADMIN
#define WUEPS_PID_TAF   I0_WUEPS_PID_TAF
#define WUEPS_PID_VC    I0_WUEPS_PID_VC
#define UEPS_PID_MTA    I0_UEPS_PID_MTA
#define WUEPS_PID_DRV_AGENT   I0_WUEPS_PID_DRV_AGENT
#endif


/*****************************************************************************
  9 全局变量声明
*****************************************************************************/


/*****************************************************************************
  10 函数声明
*****************************************************************************/


#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#endif /* _VOS_PID_DEF_H */
