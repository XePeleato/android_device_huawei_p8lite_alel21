/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : tds_nv_interface.h
  版 本 号   : 初稿
  作    者   : baizhongjin
  生成日期   : 2013年3月25日
  最近修改   :
  功能描述   : TDS nv 头文件
  函数列表   :

  修改历史   :
  1.日    期   : 2013年3月25日
    作    者   :
    修改内容   : 创建文件

******************************************************************************/

/******************************************************************************/
#ifndef __TDS_NV_INTERFACE_H__
#define __TDS_NV_INTERFACE_H__

#ifdef __cplusplus
extern "C" {
#endif



//#include "type_define.h"
#include "lt_nv_common_interface.h"
#include "tds_dsp_equip_interface.h"

#define RFIC_VERSION_V210               (210)

#define TDS_RF_SUPPORT_BAND_NUM  	  		 2
#define TDS_MAX_ANT_MIPI_CMD_NUM  	  		 4/*mipi接口天线开关切换指令的最大个数*/
#define TDS_MAX_PA_MIPI_CMD_NUM  	  		 4/*mipi接口pa切换指令的最大个数*/
#define TDS_MAX_TURNER_MIPI_CMD_NUM  	  	 4/*mipi接口turner切换指令的最大个数*/
#define TDS_MAX_MIPI_INIT_CMD_NUM  	  		 8/*mipi器件初始化表的个数*/
/* BEGIN: Added by w00180842, 2013/8/29   PN:tuner*/
#define TDS_TUNER_SUPPORT_SENSOR_NUM  	  	 8
#define TDS_TUNER_DAC_MAX                    6
#define TDS_TUNER_CONTROL_MAX                4
#define TDS_TUNER_CMD_MAX      (TDS_TUNER_DAC_MAX + TDS_TUNER_CONTROL_MAX)
/* END:   Added by w00180842, 2013/8/29 */
/*前端相关NV begin*/
/*天线开关bitmap*/
typedef enum
{
    TDS_ANTSEL_INDEX_RX0    = 0x0000000,
    TDS_ANTSEL_INDEX_RX1    = 0x1000000,
    TDS_ANTSEL_INDEX_TX     = 0x2000000,
    TDS_ANTSEL_INDEX_3      = 0x3000000
}TDS_ANTSEL_INDEX_ENUM;


typedef struct
{
    UINT16 usRxMainLnaSel; //射频主通道LNA选择
    UINT16 usRxDvtyLnaSel;
    UINT16 usTxLnaSel;
    UINT16 usResv;
}TDS_RF_PRIVATE_CHANNEL_CFG_STRU;


typedef struct
{
    UINT32 ulAbb0RstBitMap;
    UINT32 ulAbb0RxAEnBitMap;
    UINT32 ulAbb0RxBEnBitMap;
    UINT32 ulAbb0TxEnBitMap;
    UINT32 ulRxABSwapBitMap;
    UINT32 ulRssiCalBitMap;
    UINT32 ulAdOn1BitMap;

}TDS_ABB_LINECTRL_BITMAP_STRU;


typedef struct
{
    UINT16 usCoreSel;    //接收通道Core选择
    UINT16 usDivider;    //接收通道分频选择
    //UINT16 usTxCoreSel;
    //UINT16 usTxDivider;
}TDS_RF_PRIVATE_CORE_DIV_CFG_STRU;
typedef enum
{
    TDS_RF_CH0,
    TDS_RF_CH1,
}TDS_RF_CH_SEL_ENUM;
typedef UINT16 TDS_RF_CH_SEL_ENUM_UINT16;


typedef struct
{
    UINT16 usRfRxOnAdvanceTime;
    UINT16 usRfTxOnAdvanceTime;
    UINT16 usFreqSWAdvanceTime;
    UINT16 usFirRxEnAdvanceTime;
    UINT16 usFirTxEnAdvanceTime;
    UINT16 usMidAdvanceTime;
    UINT16 usABBAdOnAdvanceTime;
    UINT16 usABBDaOnAdvanceTime;
    UINT16 usPaEnAdvanceTime;
    UINT16 usTxEnAdvanceTime;
    UINT16 usAntSelAdvanceTime;
    TDS_RF_CH_SEL_ENUM_UINT16 enChannelSel;
}TDS_NV_RF_ADVANCE_TIME_STU;

typedef struct
{
    UINT32 ulAntSelCfg0;
    UINT32 ulAntSelCfg1;
    UINT32 ulAntSelCfg2;
    UINT32 ulAntSelCfg3;
}TDS_ANTSEL_ALLOT_STRU;

typedef struct
{

    UINT8  aucPaVal[4];/*支持的pa档位的配置值 默认数组0 1 2代表高中低*/
}TDS_PA_LEVEL_STRU;

/* tds mipi begin*/


typedef struct
{
    UINT32                            ucValidflg:1;         /*当前指令是否有效*/
    UINT32                            ucExWFlg:1;           /*是否采用扩展写发送方式,1是*/
    UINT32                            ucWatTime:4;          /*发送完等待时间*/
    UINT32                            ucSlaveid:4;          /*设备USID*/
    UINT32                            ucMipiAddr:8;         /*在MIPI设备中的偏移地址*/
    UINT32                            ucByteCnt:3;          /*如果是扩展指令，指明CNT*/
    UINT32                            usAdvanceTime:11;      /*此指令提前于接收数据起点的时间 1/8chip单位*/
} TDS_NV_MIPI_CMD_CTRL_STRU;

typedef struct
{
    TDS_NV_MIPI_CMD_CTRL_STRU        stMipiCtrl;
    UINT8                            ucCmdByte0;
    UINT8                            ucCmdByte1;
    UINT8                            ucCmdByte2;
    UINT8                            ucCmdByte3;
}TDS_MIPI_CMD_STRU;


typedef struct
{
    UINT16             usAntMipiValidFlg; /*此band的天线开关是否需要mipi指令支持*/
    UINT16             usANtMipiCmdNum;
    TDS_MIPI_CMD_STRU  stAntMipiCmd[TDS_MAX_ANT_MIPI_CMD_NUM];
}TDS_ANT_MIPI_CMD_STRU;

/*天线开关的提前量要限定到0-15chips*/
typedef struct
{
    TDS_ANT_MIPI_CMD_STRU stRxMipiCmd;
    TDS_ANT_MIPI_CMD_STRU stTxMipiCmd;
    TDS_ANT_MIPI_CMD_STRU stNotchTxMipiCmd;
}TDS_ANT_ALLOT_MIPI_CMD_STRU;

typedef struct
{
    UINT16             usPaMipiValidFlg; /*此band的pa是否需要mipi指令支持*/
    UINT16             usPaMipiCmdNumPreLevel;/*每个pa档位的mipi指令数*/
    UINT16             usPaOffNum;
    UINT16             usRes;
    TDS_MIPI_CMD_STRU  stPaHighMipiCmd[TDS_MAX_PA_MIPI_CMD_NUM];
    TDS_MIPI_CMD_STRU  stPaMidMipiCmd[TDS_MAX_PA_MIPI_CMD_NUM];
    TDS_MIPI_CMD_STRU  stPaLowMipiCmd[TDS_MAX_PA_MIPI_CMD_NUM];
    TDS_MIPI_CMD_STRU  stPaOffMipiCmd[TDS_MAX_PA_MIPI_CMD_NUM];
}TDS_PA_MIPI_CMD_STRU;

/*所有的mipi器件上电初始化*/
typedef struct
{
    UINT16             usMipiValidFlg; /*此band的pa是否需要mipi指令支持*/
    UINT16             usMipiInitCmdNum;
    TDS_MIPI_CMD_STRU  astMipiInitCmd[TDS_MAX_MIPI_INIT_CMD_NUM];
}TDS_MIPI_INIT_CMD_STRU;

/*线控生效配置bitmap，该map根据硬件连接而确定*/
typedef struct
{
    TDS_MIPI_INIT_CMD_STRU stMipiInit;
    TDS_ANT_ALLOT_MIPI_CMD_STRU astAntMipiCmd[TDS_RF_SUPPORT_BAND_NUM];
    TDS_PA_MIPI_CMD_STRU astPaMipiCmd[TDS_RF_SUPPORT_BAND_NUM];
}TDS_NV_MIPI_CTRL_STRU;
typedef struct
{
    UINT32 aulCtuPaMaxConfig[8];
    UINT32 ulBandNumSupt;  //支持band数目
    UINT32 aulBandList[TDS_RF_SUPPORT_BAND_NUM];     //支持band列表
    UINT32 aulPaEnAllot[TDS_RF_SUPPORT_BAND_NUM]; /*pa en 映射的bit位置*/
    UINT32 aulPaAtt0Allot[TDS_RF_SUPPORT_BAND_NUM];/*pa att0 映射的bit位置*/
    UINT32 aulPaAtt1Allot[TDS_RF_SUPPORT_BAND_NUM];/*pa att1 映射的bit位置*/
    TDS_PA_LEVEL_STRU astPaVal[TDS_RF_SUPPORT_BAND_NUM];/*不同ｂａｎｄ不同档位的真值*/
    TDS_ANTSEL_ALLOT_STRU astAntSelAllot[TDS_RF_SUPPORT_BAND_NUM];
}TDS_NV_LINECTRL_ALLOT_BY_HARDWARE_STRU;

typedef struct
{
    UINT16      ulRficVersion;    //单板形态 备用
    UINT16      ulBandNumSupt;  //支持band数目
    UINT16      aulBandList[TDS_RF_SUPPORT_BAND_NUM];     //支持band列表
    TDS_RF_PRIVATE_CHANNEL_CFG_STRU  astRfChanCfg[TDS_RF_SUPPORT_BAND_NUM];//射频通道配置
    TDS_RF_PRIVATE_CORE_DIV_CFG_STRU astRfCoreAndDivSel[TDS_RF_SUPPORT_BAND_NUM];
}TDS_NV_RFIC_CFG_STRU;
/*前端相关NV end*/


    /* rx tx  nv  start   */
typedef struct
{
    TDS_BAND_COMMON_PARA_STRU astCommon[TDS_RF_SUPPORT_BAND_NUM];
    TDS_BAND_RXPATH_PARA_STRU astRxPara[TDS_RF_SUPPORT_BAND_NUM];
    TDS_BAND_TXPATH_PARA_STRU astTxPara[TDS_RF_SUPPORT_BAND_NUM];
}TDS_NV_RXTX_PARA_STRU;

    /* rx tx nv end   */

/* BEGIN: Added by w00180842, 2013/8/16   PN:tuner*/

 /*MIPI写命令格式如下
31--29  001  ---->指令类型
28--25  0111 ---->SLAVE ADDRESS
24--22  010  ---->cmd
21--17         ---->addr 例如0x1c,0x00等
16                ---->17--24的校验位
15--8           ---->配置的数据
7                  ---->15---8的校验位
6--0             ---->保留位
 aulControlCfgPara[i] 需按照上述BIT位全部写入
 usDacCmd    15--0依次对应上述 31--16BIT位写入
 ausDacData[j][k]15---0依次对应上述15--0BIT位写入
 */
 #if 0
typedef struct
{
    UINT16 usDacCmd;
    UINT16 usRev;
    UINT16 ausDacData[TDS_RF_SUPPORT_BAND_NUM][TDS_TUNER_SUPPORT_SENSOR_NUM];//BAND顺序与BAND列表相同
}TDS_TUNER_DAC_CFG_PARA_STRU;
typedef struct
{
    UINT16 usDacCfgNum;//必须与ox00寄存器中DAC使能的个数一致
    UINT16 usRev;
    TDS_TUNER_DAC_CFG_PARA_STRU stDacCfgPara[TDS_TUNER_DAC_MAX];
}TDS_TUNER_DAC_CFG_STRU;
 typedef struct
 {
     UINT16 usControlCfgNum;//最大值为4，SLEEP MODE 目前默认配置一个(0x1c)，activeMODE 默认配置3个
     UINT16 usRev;
     UINT32 aulControlCfgPara[TDS_TUNER_CONTROL_MAX];//配置时从0--5依次配置，0-->0x1c, 1-->0x10, 2-->0x0，其他依次放置
 }TDS_TUNER_CONTROL_CFG_STRU;
typedef struct
{
    TDS_TUNER_CONTROL_CFG_STRU stControlCfg;
    TDS_TUNER_DAC_CFG_STRU stDacCfg;
}TDS_TUNER_MIPI_ACTIVE_MODE_CMD_STRU;
typedef struct
{
    TDS_TUNER_CONTROL_CFG_STRU stControlCfg;
}TDS_TUNER_MIPI_SLEEP_MODE_CMD_STRU;
#endif
typedef struct
{
    UINT16             usTurnerMipiValidFlg; /*此band的turner是否需要mipi指令支持*/
    UINT16             usTurnerMipiCmdNum;
    TDS_MIPI_CMD_STRU  astMipiCmd[TDS_MAX_TURNER_MIPI_CMD_NUM];

}TDS_TUNER_MIPI_PARA_STRU;
typedef struct
{
    TDS_TUNER_MIPI_PARA_STRU stTurnerInitCmd;

    TDS_TUNER_MIPI_PARA_STRU astTurnerMipiCmd[TDS_RF_SUPPORT_BAND_NUM];
}TDS_NV_TUNER_PARA_STRU;

typedef struct
{
    UINT16 usTdsDrxIdleEnableFlag;
    UINT16 usTdsDrxSwitch32kTime;
    UINT16 usTdsClkMeasParaIndex;
    UINT16 usTdsPresyncStartGate;
    UINT16 usTdsDrxIdleWaitTaskTime;
    UINT16 usTdsDrxIdleClkMeasStubFlag;
    UINT16 usTdsDrxIdleSleepStubFlag;
    UINT16 usTdsDrxSlaveEnableFlag;
    UINT16 usRsvd[4];
}TDS_NV_DRX_PARA_STRU;

typedef struct
{
    UINT16 usVrampFlag;
    UINT16 usTdsBand34Vol;
    UINT16 usTdsBand39Vol;
    UINT16 usResv;
}TDS_NV_VRAMP_PARA_STRU;

/*l00253980+tas 20140915 begin*/
typedef struct
{
	UINT32 NV_ID_TDS_AS_CTRL;
    UINT32 NV_ID_TDS_AS_ALG_PARA0; //Byte0：alpha 滤波遗忘因子（量纲扩大16倍），默认值为2。
	UINT32 NV_ID_TDS_AS_ALG_PARA1; //Byte1：N1 默认值 1
	UINT32 NV_ID_TDS_AS_ALG_PARA2; //Byte2：N2 默认值 5
	UINT32 NV_ID_TDS_AS_ALG_PARA3; //Byte3：N3 默认值 7
	UINT32 NV_ID_TDS_AS_ALG_PARA4; //Byte4：SW_THR1 默认值 9dB
	UINT32 NV_ID_TDS_AS_ALG_PARA5; //Byte5：SW_THR2 默认值 6dB
	UINT32 NV_ID_TDS_AS_ALG_PARA6; //Byte6：SW_THR3 默认值 3dB
	UINT32 NV_ID_TDS_AS_ALG_PARA7; //Byte7：SW_HYST 默认值 2s

    //+TAS250 20140928 begin
	UINT32         NV_ID_TDS_PrachFailNum; // 2
	UINT16         NV_ID_TDS_NGI; // 1
	UINT16         NV_ID_TDS_NPI; // 1
	INT16          NV_ID_TDS_GOOD_RSRP_THR;// -60
	INT16          NV_ID_TDS_POOR_RSRP_THR;// -95
    UINT32         NV_ID_TDS_GOOD_SCALE_THR;// 500
	UINT32         NV_ID_TDS_POOR_SCALE_THR;// 20
	UINT16         NV_ID_TDS_IDLE_HAPPY_THR;// 2
	UINT16         NV_ID_TDS_IDLE_UNHAPPY_THR;// 2

	UINT32         NV_ID_TDS_TAS250_ENA;// 0
	//+tas250 end
}TDS_NV_TAS_PARA_STRU;

typedef struct
{
    UINT32 ulTdsDpdtDefault0;
    UINT32 ulTdsSwitchMap0;
	UINT32 ulTdsDpdtDefault1;
	UINT32 ulTdsSwitchMap1;

    UINT32 ulTdsTasDpdtEna;
}TDS_NV_TAS_RF_PARA_STRU;
/*l00253980+tas 20140915 end*/
typedef struct
{
    TDS_NV_RFIC_CFG_STRU stRficCfg;
    TDS_NV_LINECTRL_ALLOT_BY_HARDWARE_STRU stLineCtrlMapped;
    TDS_NV_MIPI_CTRL_STRU  stLineMipi;
    TDS_NV_RF_ADVANCE_TIME_STU stTdsTimer;
    TDS_NV_RXTX_PARA_STRU stRxTxPara;
    /* BEGIN: Added by w00180842, 2013/8/16   PN:tuner*/
    TDS_NV_TUNER_PARA_STRU stTunerPara;
    /* END:   Added by w00180842, 2013/8/16 */
    /* BEGIN: Added by x00228734, 2013/9/30*/
    TDS_NV_DRX_PARA_STRU stTdsDrxPara;
    /* END: Added by x00228734, 2013/9/30*/

    /*l00253980+tas 20140915 begin*/
    TDS_NV_TAS_PARA_STRU stTdsTasPara;
    TDS_NV_TAS_RF_PARA_STRU stTdsTasRfPara;
    UINT16 stTasCtrlEna;
    /*l00253980+tas 20140915 begin*/

    UINT16 usVctcxoInit;
    UINT16 usResv;
    TDS_NV_VRAMP_PARA_STRU stTdsVrampPara;
}TDS_NV_STRU;

/*私有nv*/
typedef struct
{
    UINT32 ulRes;
}TDS_NV_PRIVATE_STRU;

typedef struct
{
    UINT16      usCurrBand;
    UINT16      usLineCfgIndex;    //单板形态 备用
    UINT16      usRfCfgIndex;  //支持band数目
    UINT16      usResv;
}TDS_NV_INDEX_STRU;


extern TDS_NV_STRU * gpstTdsV9NvPara;
extern TDS_NV_INDEX_STRU            gstNvCfgInd;

TDS_BAND_TXPATH_PARA_STRU * TDS_GetTxNv(void);
TDS_PA_LEVEL_THRE_STRU * TDS_GetPaTreNv(void);
TDS_BAND_RXPATH_PARA_STRU * TDS_GetRxNv(void);
TDS_TEMPERATURE_SENSOR_STRU * TDS_GetSensorNv(void);
TDS_AGC_BAND_PARA_STRU * TDS_GetAgcBandNv(void);
UINT16  TDS_GetRxMainLnaSel(UINT32 ulBandIdx);
UINT16  TDS_GetRxDvtyLnaSel(UINT32 ulBandIdx);
UINT16  TDS_GetTxLnaSel(UINT32 ulBandIdx);
TDS_RF_PRIVATE_CORE_DIV_CFG_STRU*  TDS_GetCoreDivCfg(UINT32 ulBandIdx);
TDS_RF_TXIQ_CAL_STRU* TDS_GetTxiqFromNv(void);
UINT16 TDS_GetDcocComFromNv(UINT16 usGainCode);
TDS_NV_TUNER_PARA_STRU * TDS_GetTunerNv(void);
TDS_NV_DRX_PARA_STRU * TDS_GetTdsDrxNv(void);
/*c00203521  noisetool*/
UINT16 TDS_GetCTBandIndex(UINT16 usBand);
UINT16 gusTdsEquipBandIndex;
static inline TDS_RF_CH_SEL_ENUM_UINT16 TDS_MEAS_GetTdsCtuChNum(void)
{
    return gpstTdsV9NvPara->stTdsTimer.enChannelSel;
}
void TDS_NV_BtInit(void);

#ifdef __cplusplus
}/*end of extern C*/
#endif
#endif

