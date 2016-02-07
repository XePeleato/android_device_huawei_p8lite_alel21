/******************************************************************************

    Copyright(C)2008,Hisilicon Co. LTD.

 ******************************************************************************
  File Name       : diag_cmd_param_plugin.h
  Description     : diag_cmd_param_plugin.h header file
  History         :
     1.w00182550       2012-11-20     Draft Enact
     2.
******************************************************************************/

#ifndef __DIAG_CMD_PARAM_PLUGIN_H__
#define __DIAG_CMD_PARAM_PLUGIN_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  1 Include Headfile
*****************************************************************************/
#include  "vos.h"
#include  "msp_nv_def.h"
#include  "msp_nv_id.h"
#include  "diag_cmd_id.h"
#include  "mdrv.h"

#pragma pack(4)

/*****************************************************************************
  2 macro
*****************************************************************************/


/*****************************************************************************
  3 Massage Declare
*****************************************************************************/


/*****************************************************************************
  4 Enum
*****************************************************************************/

/* 数采类型枚举*/
typedef enum
{
    BBP_SAMPLE_COUNTER_MODE =0, /*单次数采*/
    BBP_SAMPLE_SWITCH_MODE     /*循环数采*/

}BBP_SAMPLE_TYPE;

/*LOG采数开始或结束寄存器标志*/
typedef enum
{
    BBP_LOG_SAMPLE_START = 0,
    BBP_LOG_SAMPLE_STOP
}BBP_LOG_SAMPLE_TYPE;

/*数采开始或结束寄存器标志*/
typedef enum
{
    BBP_DATA_SAMPLE_START = 0,
    BBP_DATA_SAMPLE_STOP
}BBP_DATA_SAMPLE_TYPE;


/*****************************************************************************
   5 STRUCT
*****************************************************************************/

/*****************************************************************************
 描述 :一级头: SOCP头
*****************************************************************************/
typedef struct
{
    VOS_UINT8 usSId;  /* 下面两个字节统称Service ID*/
    VOS_UINT8 usSSId; /* 高四位*/
    VOS_UINT8 ucServiceSessionId;
    VOS_UINT8 ucMsgType;
    VOS_UINT32 ulMsgTransId;
    VOS_UINT8 aucTimeData[8];
} MSP_SOCP_HEAD_STRU;

/*****************************************************************************
 描述 :二级头: DIAG头
*****************************************************************************/
typedef struct
{
    VOS_UINT32 ulID;         /* 结构化ID*/
    VOS_UINT32 ulDataSize;   /* 紧随的ucData的长度*/
    VOS_UINT8  ucData[0];    /* 参数的数据*/
} MSP_DIAG_HEAD_STRU;

/*****************************************************************************
 描述 :三级头: DIAG 头
 REQ:MSP_DIAG_DATA_REQ_STRU
 CNF:MSP_DIAG_DATA_CNF_STRU
*****************************************************************************/
typedef struct
{
    VOS_UINT32 ulAuid;         /* 原AUID*/
    VOS_UINT32 ulSn;            /* HSO分发，插件命令管理*/
    VOS_UINT8  ucData[0];      /* 参数的数据*/
} MSP_DIAG_DATA_REQ_STRU;

typedef MSP_DIAG_DATA_REQ_STRU MSP_DIAG_DATA_CNF_STRU;

typedef struct
{
  VOS_UINT32 ulRet;
}DIAG_CMD_COMMON_CNF;

typedef struct
{
    VOS_UINT32 ulRc;       /* 0: SUCCESS OTHER: FAILUE*/
} DIAG_CMD_DSP_CNF_STRU;

/*****************************************************************************
 描述 : 针对模块的打印开关命令,支持多个命令参数
ID   : DIAG_CMD_LOG_CAT_PRINT
REQ : DIAG_CMD_LOG_CAT_PRINT_REQ_STRU
CNF : DIAG_CMD_LOG_CAT_PRINT_CNF_STRU
IND : DIAG_CMD_LOG_PRINT_RAW_TXT_IND_STRU
*****************************************************************************/

typedef struct
{
    VOS_UINT32 ulModuleId;   /* 如果为0xFFFFFFFF表示为所有模块的Level开关*/
    VOS_UINT32 ulLevelFilter;    /* ERROR|WARNING|NORMAL|INFO 1: OPEN, 0：CLOSE*/
} DIAG_CMD_LOG_CAT_PRINT_REQ_STRU;

typedef struct
{
    VOS_UINT32 ulRc; /* 结果码*/
    VOS_UINT32 ulModuleId;
} DIAG_CMD_LOG_CAT_PRINT_CNF_STRU;

typedef struct
{
    VOS_UINT32 ulModule; /*  打印信息所在的模块ID */
    VOS_UINT32 ulLevel;  /* 输出级别*/
    VOS_UINT32 ulNo;     /*  IND标号*/
    VOS_INT8 szText[0]; /*  所有打印文本内容，可能包括文件和行号,以'\0'结尾*/
} DIAG_CMD_LOG_PRINT_RAW_TXT_IND_STRU;


/*****************************************************************************
描述 : 针对模块的层间开关,支持多个命令参数
ID   : DIAG_CMD_LOG_CAT_LAYER
REQ : DIAG_CMD_LOG_CAT_LAYER_REQ_STRU
CNF : DIAG_CMD_LOG_CAT_LAYER_CNF_STRU
说明: 如果多包命令参数的情况，一个模块错误，就会返回失败，但是正确的模块参数依然会存储
IND : DIAG_CMD_LOG_LAYER_IND_STRU
*****************************************************************************/

typedef struct
{
    VOS_UINT32 ulModuleId;       /* 模块ID*/
    VOS_UINT32 ulIsDestModule;   /* 0: 表示Source Module， 1：表示Dest Module*/
    VOS_UINT32 ulSwitch;         /* (0x00000000): 关；(0x80000000)：开*/
} DIAG_CMD_LOG_CAT_LAYER_REQ_STRU;

typedef struct
{
    VOS_UINT32 ulRc; /* 结果码*/
    VOS_UINT32 ulModuleId;
} DIAG_CMD_LOG_CAT_LAYER_CNF_STRU;

typedef struct
{
    VOS_UINT32 ulModule;     /* 源模块ID*/
    VOS_UINT32 ulDestMod;    /* 目的模块ID*/
    VOS_UINT32 ulNo;         /* 序号*/
    VOS_UINT32 ulId;         /* ID*/
    VOS_INT8 aucDta[0];     /* 用户数据缓存区*/
} DIAG_CMD_LOG_LAYER_IND_STRU;

/*****************************************************************************
描述 : 针对AIR的总开关,支持多个命令参数
ID   : DIAG_CMD_LOG_CAT_AIR
REQ : DIAG_CMD_LOG_CAT_AIR_REQ_STRU
CNF : DIAG_CMD_LOG_CAT_AIR_CNF_STRU
IND : DIAG_CMD_LOG_AIR_IND_STRU
*****************************************************************************/
typedef struct
{
    VOS_UINT32 ulSwitch; /* (0x00000000): 关；(0x80000000)：开*/
    VOS_UINT32 ulGuSwitch;   /*  1开，0关*/
} DIAG_CMD_LOG_CAT_AIR_REQ_STRU;

typedef struct
{
    VOS_UINT32 ulRc; /* 结果码*/
} DIAG_CMD_LOG_CAT_AIR_CNF_STRU;

typedef struct
{
    VOS_UINT32 ulModule;     /* 源模块ID*/
    VOS_UINT32 ulSide;       /* 1: NET-->UE, 2: UE-->NET*/
    VOS_UINT32 ulNo;         /* 序号*/
    VOS_UINT32 ulId;         /* ID*/
    VOS_INT8 aucDta[0];     /* 用户数据缓存区*/
} DIAG_CMD_LOG_AIR_IND_STRU;

/*****************************************************************************
描述 : 针对VoLTE的总开关,支持多个命令参数
ID   : DIAG_CMD_LOG_CAT_EVENT
REQ : DIAG_CMD_LOG_CAT_EVENT_REQ_STRU
CNF : DIAG_CMD_LOG_CAT_EVENT_CNF_STRU
IND : DIAG_CMD_LOG_EVENT_IND_STRU
*****************************************************************************/
typedef struct
{
    VOS_UINT32 ulModule;     /* 源模块ID*/
    VOS_UINT32 ulSide;       /* 1: NET-->UE, 2: UE-->NET*/
    VOS_UINT32 ulNo;         /* 序号*/
    VOS_UINT32 ulId;         /* ID*/
    VOS_INT8   aucDta[0];    /* 用户数据缓存区*/
} DIAG_CMD_LOG_VoLTE_IND_STRU;
/*****************************************************************************
描述 : 针对EVENT的总开关,支持多个命令参数
ID   : DIAG_CMD_LOG_CAT_EVENT
REQ : DIAG_CMD_LOG_CAT_EVENT_REQ_STRU
CNF : DIAG_CMD_LOG_CAT_EVENT_CNF_STRU
IND : DIAG_CMD_LOG_EVENT_IND_STRU
*****************************************************************************/
typedef struct
{
    VOS_UINT32 ulSwitch;
} DIAG_CMD_LOG_CAT_EVENT_REQ_STRU;

typedef struct
{
    VOS_UINT32 ulRc; /* 结果码*/
    VOS_UINT32 ulSwitch;
} DIAG_CMD_LOG_CAT_EVENT_CNF_STRU;

typedef struct
{
    VOS_UINT32 ulNo;      /* 序号*/
    VOS_UINT32 ulId;      /* 消息或者事件ID,主要针对消息,空口,事件,普通打印输出时该成员为零*/
} DIAG_CMD_LOG_EVENT_IND_STRU;

/*****************************************************************************
描述 : 针对UserPlane 消息ID开关,支持多个命令参数
ID   : DIAG_CMD_LOG_CAT_USERPLANE
REQ : DIAG_CMD_LOG_CAT_USERPLANE_REQ_STRU
CNF : DIAG_CMD_LOG_CAT_USERPLANE_CNF_STRU
说明:   如果多包命令参数的情况，一包MSG ID错误，就会返回失败,但是正确的参数依然会存储;
IND : DIAG_CMD_LOG_USERPLANE_IND_STRU
*****************************************************************************/
typedef struct
{
    VOS_UINT32 ulMsgId;  /* 消息ID*/
    VOS_UINT32 ulSwitch; /* (0x00000000): 关；(0x80000000)：开*/
} DIAG_CMD_LOG_CAT_USERPLANE_REQ_STRU;

typedef struct
{
    VOS_UINT32 ulRc; /* 结果码*/
    VOS_UINT32 ulMsgId;  /* 消息ID*/
} DIAG_CMD_LOG_CAT_USERPLANE_CNF_STRU;

typedef struct
{
    VOS_UINT32 ulNo;         /* 序号*/
    VOS_UINT32 ulId;         /* ID*/
    VOS_INT8 aucDta[0];     /* 用户数据缓存区*/
} DIAG_CMD_LOG_USERPLANE_IND_STRU;

/*****************************************************************************
描述 : 针对消息ID/命令ID开关,支持多个命令参数
ID   : DIAG_CMD_LOG_CAT_CFG
REQ : DIAG_CMD_LOG_CAT_CFG_REQ_STRU
CNF : DIAG_CMD_LOG_CAT_CFG_CNF_STRU
说明:   如果多包命令参数的情况，一包MSG ID错误，就会返回失败,但是正确的参数依然会存储;
*****************************************************************************/
typedef struct
{
    VOS_UINT32 ulId;       /* 消息ID/命令ID*/
    VOS_UINT32 ulCategory; /* 取值为 DIAG_CMD_LOG_CATETORY_XXX*/
    VOS_UINT32 ulSwitch;
} DIAG_CMD_LOG_CAT_CFG_REQ_STRU;

typedef struct
{
    VOS_UINT32 ulRc;       /* 结果码*/
    VOS_UINT32 ulId;       /* 消息ID/命令ID*/
} DIAG_CMD_LOG_CAT_CFG_CNF_STRU;


/*****************************************************************************
描述 : 针对消息ID/命令ID开关,支持多个命令参数
ID   : DIAG_CMD_GTR_SET
REQ : DIAG_CMD_GTR_SET_REQ_STRU
CNF : DIAG_CMD_GTR_SET_CNF_STRU
*****************************************************************************/

typedef struct
{
    VOS_UINT32 ulGtrDtaSize;
    VOS_UINT8 aucDta[0];
} DIAG_CMD_GTR_SET_REQ_STRU;

typedef struct
{
    VOS_UINT32 ulRc; /* 结果码*/
} DIAG_CMD_GTR_SET_CNF_STRU;

/*****************************************************************************
描述 : 读指定寄存器数据
ID   : DIAG_CMD_REG_RD
REQ : DIAG_CMD_REG_RD_REQ_STRU
CNF : DIAG_CMD_REG_RD_CNF_STRU
*****************************************************************************/
typedef struct
{
    VOS_UINT32 ulRegAddr[0];          /* the address to read.*/
} DIAG_CMD_REG_RD_REQ_STRU;

typedef struct
{
    VOS_UINT32 ulRc;
    VOS_UINT32 ulAddr;
    VOS_UINT32 ulRegValue;
} DIAG_CMD_REG_RD_CNF_PARA_STRU;

/*****************************************************************************
描述 : 写指定寄存器数据
ID   : DIAG_CMD_REG_WR
REQ : DIAG_CMD_REG_WR_REQ_STRU
CNF : DIAG_CMD_REG_WR_CNF_STRU
*****************************************************************************/
typedef struct
{
    VOS_UINT32 ulAddr;            /* 地址*/
    VOS_UINT32 ulRegValue;        /* 参数*/
    VOS_UINT16 ucBeginBit;	      /* 起始的有效位*/
    VOS_UINT16 ucEndBit;		  /* 结束的有效位*/
    VOS_UINT32 ulTimeSleep;	      /* 时间间隔，暂时不用，后续扩展*/
} DIAG_CMD_REG_WR_PARA_STRU;

typedef struct
{
    DIAG_CMD_REG_WR_PARA_STRU ast[0];
} DIAG_CMD_REG_WR_REQ_STRU;

typedef struct
{
    VOS_UINT32 ulRc;
    VOS_UINT32 ulAddr;
} DIAG_CMD_REG_WR_CNF_PARA_STRU;


/*****************************************************************************
描述 : 读NV
ID   : DIAG_CMD_NV_RD
REQ : DIAG_CMD_NV_QRY_REQ_STRU
CNF : DIAG_CMD_NV_QRY_CNF_STRU
*****************************************************************************/
typedef struct
{
    VOS_UINT32 ulNVId;                       /* 待获取的NV项Id*/
} DIAG_CMD_NV_QRY_REQ_STRU;

typedef struct
{
    VOS_UINT32 ulRc;                         /* 表明执行结果是否成功, 0表示成功，其他的为错误码*/
    VOS_UINT32 ulNVId;                       /* 获取的NV项Id*/
    VOS_UINT32 ulDataSize;                   /* 获取的NV项数据的大小*/
    VOS_UINT8  aucData[0];                   /* 获取的NV项数据*/
} DIAG_CMD_NV_QRY_CNF_STRU;

/*****************************************************************************
描述 : 写NV
ID   : DIAG_CMD_NV_WR
REQ : DIAG_CMD_NV_WR_REQ_STRU
CNF : DIAG_CMD_NV_WR_CNF_STRU
*****************************************************************************/
typedef struct
{
    VOS_UINT32 ulNVId;                       /* 需要写入的NV ID*/
    VOS_UINT32 ulDataSize;                   /* 需要写入的NV项数据的大小*/
    VOS_UINT8  aucData[0];                   /* 数据缓冲区*/
} DIAG_CMD_NV_WR_REQ_STRU;

typedef struct
{
    VOS_UINT32 ulRc;                         /* 表明执行结果是否成功,0表示成功，其他的为错误码*/
    VOS_UINT32 ulNVId;
} DIAG_CMD_NV_WR_CNF_STRU;

/*****************************************************************************
描述 : 导入NV
ID   : DIAG_CMD_NV_IMPORT
REQ : DIAG_CMD_NV_FILE_LOAD_REQ_STRU
CNF : DIAG_CMD_NV_FILE_LOAD_CNF_STRU
*****************************************************************************/
typedef struct
{
    VOS_UINT16 ulBlockIndex;                 /* 写入的文件块序列号，从0开始*/
    VOS_UINT16 ulTotalBlockCnt;              /* 总块数,总块数不大于MSP_BIN_FILE_MAX_SIZE/DIAG_CMD_MEM_RD_MAX_NUM +1*/
    VOS_UINT32 ulTotalSize;                  /* 文件总大小，小于126K*/
    VOS_UINT32 ulBlockSize;                  /* 本次写入的数据大小*/
    VOS_UINT8  aucData[0];                   /* 数据缓冲区*/
} DIAG_CMD_NV_FILE_LOAD_REQ_STRU;


typedef struct
{
    /* 返回上个传入块的执行结果*/
    /* 如整个文件写入完毕，则返回整个文件的执行结果*/
    VOS_UINT32 ulRc;

    /*该值填入的是期望HSO传入的下一个BLOCK INDEX*/
    /* 如所有块均已成功写入，则INDEX为DIAG_CMD_TOTAL_FILE(0xFFFFFFFF)*/
    /* 如传入INDEX为0，则丢弃前面写入的块，认为文件为新传入*/
    VOS_UINT32 ulIndex;
} DIAG_CMD_NV_FILE_LOAD_CNF_STRU;


/*****************************************************************************
描述 : 导出NV
ID   : DIAG_CMD_NV_EXPORT
REQ : DIAG_CMD_NV_EXPORT_FILE_REQ_STRU
CNF : DIAG_CMD_NV_EXPORT_FILE_CNF_STRU
*****************************************************************************/
typedef struct
{
    VOS_UINT32 ulOffset;   /* 请求偏移*/
    VOS_UINT32 ulType;     /* 0: 导出当前的初始NV文件; 1: 导出当前工作的NV文件*/
} DIAG_CMD_NV_EXPORT_FILE_REQ_STRU;

typedef struct
{
    VOS_UINT32 ulRst;
    VOS_UINT32 ulOffset;       /* 请求偏移,REQ结构里面的请求偏移*/
    VOS_UINT32 ulType;         /* 到处文件类型, 0: 导出当前的初始NV文件; 1: 导出当前工作的NV文件*/
    VOS_UINT32 ulRemainSize;   /* 剩余NV文件大小*/
    VOS_UINT32 ulSize;         /* 返回的实际大小, 不超过 DIAG_TRANSFER_BLOCK_MAX_SIZE*/
    VOS_UINT8  aucData[0];
} DIAG_CMD_NV_EXPORT_FILE_CNF_STRU;

/*****************************************************************************
描述 : LOG数采
ID   : DIAG_CMD_DRX_LOG_SAMPLE_REG_WR
REQ : DIAG_CMD_DRX_LOG_SAMPLE_REG_WR_REQ_STRU
CNF : DIAG_CMD_DRX_LOG_SAMPLE_REG_WR_REQ_STRU
IND : DIAG_CMD_DRX_REG_WR_IND_STRU
*****************************************************************************/
/*保存每条下发寄存器内容结构*/
typedef struct
{
    VOS_UINT32  ulAddr;            /* 地址*/
    VOS_UINT32  ulRegValue;        /* 寄存器值，对于查询数采是否结束寄存器的原语，当寄存
                                                              器值为该值时，停止数采*/
    VOS_UINT16  ucBeginBit;	      /* 起始的有效位*/
    VOS_UINT16  ucEndBit;		  /* 结束的有效位*/
    VOS_UINT32  ulSleepTime;       /* 执行该条原语后下条原语前的休眠时间，用来保证寄存器配置满足时序要求，单位设定为ms，根据实际测试情况调整*/
    VOS_UINT32  ulSn;              /* 保存界面对应行列值, Sn = RowID * GRID_COLUMN_COUNT + ulColID;*/

} DIAG_CMD_DRX_REG_WR_PARA_STRU;

/* 低功耗Log类型数据采集下发原语结构*/
typedef struct
{
    BBP_LOG_SAMPLE_TYPE  enBBPLogCtrl;        /*log采数时用来区分启动还是结束log采数*/
    VOS_UINT32  ulLogChannelIndex;            /*采数模块对应的Log通道序号，在配置文件中配置通道号，限定为0~7*/
    VOS_UINT32  ulTotalRegCount;              /* 下发所有寄存器个数*/
    VOS_UINT32  ulLogStartRegIndex;           /* 下发寄存器中，启动寄存器起始序号*/
    VOS_UINT32  ulLogStopRegIndex;            /* 下发寄存器中，停止寄存器起始序号*/
    VOS_UINT32  ulSampleStatusRegIndex;       /* 下发寄存器中，状态寄存器起始序号*/
    VOS_UINT32  ulAccumlateRegIndex;          /* 单板端发送包数计数寄存器起始序号*/
    DIAG_CMD_DRX_REG_WR_PARA_STRU  stDrxRegPara[0];/*下发寄存器起始地址*/
} DIAG_CMD_DRX_LOG_SAMPLE_REG_WR_REQ_STRU;

/* DIAG_CMD_DRX_LOG_SAMPLE_REG_WR_REQ_STRU命令对应的CNF，底软Sleep模块接收到该命令后即可返回成功*/
typedef struct
{
    VOS_UINT32   ulRet;  /*命令执行返回值，成功返回0，失败返回-1*/
} DIAG_CMD_DRX_LOG_SAMPLE_REG_WR_CNF_STRU;

/* 低功耗数采类型数据采集下发原语结构*/
/*****************************************************************************
描述 : DATA数采
ID   : DIAG_CMD_DRX_DATA_SAMPLE_REG_WR
REQ : DIAG_CMD_DRX_DATA_SAMPLE_REG_WR_REQ_STRU
CNF : DIAG_CMD_DRX_DATA_SAMPLE_REG_WR_CNF_STRU
IND : DIAG_CMD_DRX_REG_WR_IND_STRU
*****************************************************************************/
typedef struct
{
    BBP_DATA_SAMPLE_TYPE  enBBPDataSampleCtrl;  /*启动或结束数采标识*/
    BBP_SAMPLE_TYPE enSampleType;               /*低功耗采数类型*/
    VOS_UINT32  ulTotalRegCount;               /*下发所有寄存器个数*/
    VOS_UINT32  ulStartSampleRegIndex;         /*下发寄存器中，启动数采寄存器起始序号*/
    VOS_UINT32  ulStopSampleRegIndex;          /*下发寄存器中，结束数采寄存器起始序号*/
    VOS_UINT32  ulCheckSampleFinishRegIndex;   /*下发寄存器中，查询数采是否结束寄存器起始序号*/
    VOS_UINT32  ulSampleStatusRegIndex;        /*下发寄存器中，数采状态寄存器起始序号*/
    DIAG_CMD_DRX_REG_WR_PARA_STRU  stDrxRegPara[0];/*下发寄存器起始地址*/
} DIAG_CMD_DRX_DATA_SAMPLE_REG_WR_REQ_STRU;

/* DIAG_CMD_DRX_DATA_SAMPLE_REG_WR_REQ命令对应的CNF，底软Sleep模块接收到该命令后即可返回成功*/
typedef struct
{
    VOS_UINT32   ulRet;  /*命令执行返回值，成功返回0，失败返回-1*/
} DIAG_CMD_DRX_DATA_SAMPLE_REG_WR_CNF_STRU;


/* 返回状态信息，由底软Sleep模块上报，上报BBP采集数据的包数以及标志位状态*/
typedef struct
{
    VOS_UINT32  ulTotalRegCount;                       /*上报所有寄存器个数*/
    DIAG_CMD_DRX_REG_WR_PARA_STRU stDrxRegPara[0];     /*上报寄存器*/
}DIAG_CMD_DRX_REG_WR_IND_STRU;

/*新增的低功耗数采结构体，data 和log 完全统一*/
/*****************************************************************************
描述 :
ID   : DIAG_CMD_DRX_SAMPLE_REG_WR
REQ : DIAG_CMD_DRX_SAMPLE_REG_WR_REQ_STRU
CNF : DIAG_CMD_DRX_SAMPLE_REG_WR_CNF_STRU
IND : DIAG_CMD_DRX_REG_WR_IND_STRU
*****************************************************************************/
typedef struct
{
    VOS_UINT32 ulOmDrxSampleId;
	VOS_UINT16 usOpid;
	VOS_UINT16 usPowrDomainBitmap;
	VOS_UINT8  ucDrxRegData[0];
}DIAG_CMD_DRX_SAMPLE_REG_WR_REQ_STRU;

typedef struct
{
    VOS_UINT32   ulRet;  /*命令执行返回值，成功返回0，失败返回-1*/
} DIAG_CMD_DRX_SAMPLE_REG_WR_CNF_STRU;

/*****************************************************************************
描述 : 获取SOCP\BBP DMA基地址等
ID   : DIAG_CMD_DRX_SAMPLE_REG_WR
REQ : DIAG_CMD_DRX_SAMPLE_GET_ADDR_REQ_STRU
CNF : DIAG_CMD_DRX_SAMPLE_GET_ADDR_CNF_STRU
IND : DIAG_CMD_DRX_REG_WR_IND_STRU
*****************************************************************************/
typedef enum
{
	DRX_SAMPLE_BBP_DMA_BASE_ADDR = 0x00,
	DRX_SAMPLE_BBP_DBG_BASE_ADDR ,
	DRX_SAMPLE_BBP_SRC_BASE_ADDR ,
	DRX_SAMPLE_POW_ONOFF_CLK_BASE_ADDR ,
	DRX_SAMPLE_SOCP_BASE_ADDR
}DIAG_CMD_DRX_SAMPLE_ADDR_TYPE_E;

typedef struct
{
	DIAG_CMD_DRX_SAMPLE_ADDR_TYPE_E eDiagDrxSampleAddr;
}DIAG_CMD_DRX_SAMPLE_GET_ADDR_REQ_STRU;

typedef struct
{
    VOS_UINT32   ulDrxSampleType;
    VOS_UINT32   ulDrxSampleAddr;
    VOS_UINT32   ulRet;  /*命令执行返回值，成功返回0，失败返回-1*/
} DIAG_CMD_DRX_SAMPLE_GET_ADDR_CNF_STRU;

/*****************************************************************************
描述 : 配置SOCP 基地址
ID   : DIAG_CMD_DRX_SAMPLE_REG_WR
REQ : DIAG_CMD_DRX_SAMPLE_CFG_CHNADDR_REQ_STRU
CNF : DIAG_CMD_DRX_SAMPLE_CFG_CHNADDR_CNF_STRU
IND : DIAG_CMD_DRX_REG_WR_IND_STRU
*****************************************************************************/
#define DIAG_PRODUCT_VERSION_LENGTH  (16)

typedef struct
{
    VOS_UINT32  ulAddrType; /* config here */
}DIAG_CMD_DRX_SAMPLE_GET_VERSION_REQ_STRU;

typedef struct
{
	VOS_UINT8   ulProductName[DIAG_PRODUCT_VERSION_LENGTH];
	VOS_UINT8   ulSolutiongName[DIAG_PRODUCT_VERSION_LENGTH];
    VOS_UINT32   ulRet;  /*命令执行返回值，成功返回0，失败返回-1*/
} DIAG_CMD_DRX_SAMPLE_GET_VERSION_CNF_STRU;


/*****************************************************************************
描述 : 获取SOCP\BBP DMA基地址等
ID   : DIAG_CMD_DRX_SAMPLE_REG_WR
REQ : DIAG_CMD_DRX_SAMPLE_GET_ADDR_REQ_STRU
CNF : DIAG_CMD_DRX_SAMPLE_GET_ADDR_CNF_STRU
IND : DIAG_CMD_DRX_REG_WR_IND_STRU
*****************************************************************************/
typedef enum
{
	DRX_SAMPLE_BBP_DMA_LOG0_CHNSIZE = 0x00,
	DRX_SAMPLE_BBP_DMA_LOG1_CHNSIZE ,
	DRX_SAMPLE_BBP_DMA_LOG2_CHNSIZE ,
	DRX_SAMPLE_BBP_DMA_LOG3_CHNSIZE ,
	DRX_SAMPLE_BBP_DMA_LOG4_CHNSIZE ,
	DRX_SAMPLE_BBP_DMA_LOG5_CHNSIZE ,
	DRX_SAMPLE_BBP_DMA_LOG6_CHNSIZE ,
	DRX_SAMPLE_BBP_DMA_LOG7_CHNSIZE ,
	DRX_SAMPLE_BBP_DMA_DATA_CHNSIZE
}DIAG_CMD_DRX_SAMPLE_CHNSIZE_E;

typedef enum
{
	SOCP_BBP_DMA_LOG0_CHNSIZE = 0x10000,
	SOCP_BBP_DMA_LOG1_CHNSIZE = 0x0,
	SOCP_BBP_DMA_LOG_COM_CHNSIZE = 0x2000,
}DIAG_SOCP_SAMPLE_CHNSIZE_E;


typedef struct
{
	DIAG_CMD_DRX_SAMPLE_CHNSIZE_E eDiagDrxSampleChnSize;
}DIAG_CMD_DRX_SAMPLE_GET_CHNSIZE_REQ_STRU;

/* DIAG_CMD_DRX_DATA_SAMPLE_REG_WR_REQ命令对应的CNF，底软Sleep模块接收到该命令后即可返回成功*/
typedef struct
{
    VOS_UINT32   ulChnType;  /*通道类型*/
    VOS_UINT32   ulChnAddr;  /*通道内存起始地址*/
	VOS_UINT32   ulChnSize;  /*通道大小*/
    VOS_UINT32   ulRet;      /*命令执行返回值，成功返回0，失败返回-1*/
} DIAG_CMD_DRX_SAMPLE_GET_CHNSIZE_CNF_STRU;


/*****************************************************************************
描述 : 使能SOCP 通道
ID   : DIAG_CMD_DRX_SAMPLE_REG_WR
REQ : DIAG_CMD_DRX_SAMPLE_ABLE_CHN_REQ_STRU
CNF : DIAG_CMD_DRX_SAMPLE_ABLE_CHN_CNF_STRU
IND : DIAG_CMD_DRX_REG_WR_IND_STRU
*****************************************************************************/
typedef enum
{
	DRX_SAMPLE_SOCP_CHN_ENABLE   = 0x00,
	DRX_SAMPLE_SOCP_CHN_DISABLE  = 0x01
}DIAG_CMD_DRX_SAMPLE_ABLE_CHN_E;

typedef struct
{
	DIAG_CMD_DRX_SAMPLE_ABLE_CHN_E eDiagDrxSampleAbleChn;
}DIAG_CMD_DRX_SAMPLE_ABLE_CHN_REQ_STRU;

typedef struct
{
    VOS_UINT32   ulRet;  /*命令执行返回值，成功返回0，失败返回-1*/
} DIAG_CMD_DRX_SAMPLE_ABLE_CHN_CNF_STRU;

/*****************************************************************************
 描述 : HSO连接UE设备
ID   : DIAG_CMD_HOST_CONNECT
REQ  : DIAG_CMD_HOST_CONNECT_REQ_STRU
CNF  : DIAG_CMD_HOST_CONNECT_CNF_STRU
*****************************************************************************/
typedef struct
{
    VOS_UINT32 ulMajorMinorVersion; /* 主版本号.次版本号*/
    VOS_UINT32 ulRevisionVersion;   /* 修正版本号*/
    VOS_UINT32 ulBuildVersion;      /* 内部版本号*/
} DIAG_CMD_UE_SOFT_VERSION_STRU;

typedef struct
{
    VOS_UINT16 usVVerNo;           /* V部分*/
    VOS_UINT16 usRVerNo;           /* R部分*/
    VOS_UINT16 usCVerNo;           /* C部分*/
    VOS_UINT16 usBVerNo;           /* B部分*/
    VOS_UINT16 usSpcNo;            /* SPC部分*/
    VOS_UINT16 usHardwareVerNo;    /* 硬件PCB号和印制板版本号*/
    VOS_UINT32 ulProductNo;        /* 产品类型编号，即不同外设组合的硬件平台*/
} DIAG_CMD_UE_BUILD_VER_STRU; /* 内部版本*/

typedef struct
{
    VOS_UINT32  ulDrxControlFlag:1; /*DRX部分*/
    VOS_UINT32  ulPortFlag      :1; /*Port flag 0:old,1:new*/
    VOS_UINT32  ulReserved      :30;
}DIAG_CONTROLFLAG_STRU;

typedef struct
{
    VOS_UINT32 ulRc; /* 结果码*/
    VOS_CHAR szImei[16];
    DIAG_CMD_UE_SOFT_VERSION_STRU stUeSoftVersion;
    DIAG_CMD_UE_BUILD_VER_STRU stBuildVersion;
    VOS_UINT32 ulChipBaseAddr;
    union
    {
        VOS_UINT32              UintValue;
        DIAG_CONTROLFLAG_STRU   CtrlFlag;
    } diag_cfg;/* B135新增，标示低功耗特性版本: 1:低功耗版本；0：正常版本；0xFFFFFFFF:MSP读取NV项失败，HSO会认为连接不成功并给出提示，要求重新进行连接*/
    VOS_UINT32 ulLpdMode;
    NV_ITEM_AGENT_FLAG_STRU stAgentFlag;
    VOS_CHAR szProduct[64];
} DIAG_CMD_HOST_CONNECT_CNF_STRU;

/*****************************************************************************
 描述 : HSO断开连接UE设备
ID   : DIAG_CMD_HOST_DISCONNECT
REQ  : DIAG_CMD_HOST_DISCONNECT_REQ_STRU
CNF  : DIAG_CMD_HOST_DISCONNECT_CNF_STRU
*****************************************************************************/
typedef struct
{
    VOS_UINT32 ulRc; /* 结果码*/
} DIAG_CMD_HOST_DISCONNECT_CNF_STRU;

/******************************************************************************
描述 : 该数据是发送给HSO作为某个命令超时时返回
ID   : DIAG_CMD_TIMER_OUT_IND
IND  : DIAG_CMD_TIMER_OUT_IND_STRU
*****************************************************************************/
typedef struct
{
    VOS_UINT32 ulCmdID;      /* 未返回的超时命令字*/
} DIAG_CMD_TIMER_OUT_IND_STRU;

/*****************************************************************************
 描述 : 该数据是用来设置单板LOG是存储卡还是通过USB上报
ID   : DIAG_CMD_SET_LOG_MODE
REQ : DIAG_CMD_SET_LPD_MODE_REQ_STRU
CNF : DIAG_CMD_SET_LPD_MODE_CNF_STRU
*****************************************************************************/
typedef struct
{
    VOS_UINT32 ulSDFileSize;
    VOS_UINT32 ulSDFileSaveTime;
    VOS_UINT8  ucSDFileName[256];
    VOS_UINT32 ulLpdMode;
    VOS_UINT32 ulStartEnd;
} DIAG_CMD_SET_LPD_MODE_REQ_STRU;

typedef struct
{
    VOS_UINT32 ulRc;
    VOS_UINT32 ulSDFileSize;
    VOS_UINT32 ulSDFileSaveTime;
    VOS_UINT8  ucSDFileName[256];
    VOS_UINT32 ulLpdMode;
} DIAG_CMD_SET_LPD_MODE_CNF_STRU;

/*****************************************************************************
 描述 : 该命令用来遍历每个文件或文件夹信息
ID   : DIAG_CMD_FS_SCAN_DIR
REQ : DIAG_CMD_FS_SCAN_DIR_REQ
CNF : DIAG_CMD_FS_SCAN_DIR_CNF
*****************************************************************************/
typedef struct
{
    VOS_CHAR   aucDirName[DIAG_CMD_FILE_NAME_LEN];  /* dir or file name*/
    VOS_INT32      st_mode;                         /* Mode of file */
    VOS_UINT64     st_size;                       /* File size in bytes */
    VOS_INT32      st_atime;                        /* Time of last access */
    VOS_INT32      st_mtime;                        /* Time of last modification */
    VOS_INT32      st_ctime;                        /* Time of last status change */
    VOS_UINT32    ulItemType;                       /*file or folder*/
} DIAG_DIR_FILE_INFO_STRU;

typedef struct
{
    VOS_CHAR szDirectory[MSP_DF_DIR_MAX_LEN];/*扫描文件夹名*/
}DIAG_CMD_FS_SCAN_DIR_REQ;

typedef struct
{
    VOS_UINT32  ulRet;                      /*结果码*/
    VOS_UINT32  ulDirNum;                   /*指定目录下文件/文件夹数*/
    VOS_UINT32  ulDirInfoLen;               /*指定目录下文件/文件夹长度*/
    DIAG_DIR_FILE_INFO_STRU stDirInfo[0];    /*指定目录下文件/文件夹属性*/
}DIAG_CMD_FS_SCAN_DIR_CNF;

/*****************************************************************************
 描述 : 该命令用来遍历每个文件或文件夹信息
ID   : DIAG_CMD_FS_MAKE_DIR
REQ : DIAG_CMD_FS_MAKE_DIR_REQ
CNF : DIAG_CMD_FS_MAKE_DIR_CNF
*****************************************************************************/
typedef struct
{
    VOS_CHAR szDirectory[MSP_DF_DIR_MAX_LEN];  /*创建文件夹名*/
}DIAG_CMD_FS_MAKE_DIR_REQ;

typedef struct
{
    VOS_UINT32  ulRet;              /*结果码*/
}DIAG_CMD_FS_MAKE_DIR_CNF;

/*****************************************************************************
 描述 : 该命令用来遍历每个文件或文件夹信息
ID   : DIAG_CMD_FS_DELETE
REQ : DIAG_CMD_FS_DELETE_REQ
CNF : DIAG_CMD_FS_DELETE_CNF
*****************************************************************************/
typedef struct
{
    VOS_UINT32  ulItemType;                   /*删除文件还是文件夹*/
    VOS_CHAR szDirectory[MSP_DF_DIR_MAX_LEN]; /*删除文件/文件夹名*/
}DIAG_CMD_FS_DELETE_REQ;

typedef struct
{
    VOS_UINT32  ulRet;                  /*结果码*/
}DIAG_CMD_FS_DELETE_CNF;

/*****************************************************************************
 描述 : 该命令用来查询根目录
ID   : DIAG_CMD_FS_QUERY_DIR
REQ : DIAG_CMD_FS_QUERY_REQ
CNF : DIAG_CMD_FS_QUERY_CNF
*****************************************************************************/
typedef struct
{
    VOS_UINT32  ulFolderType;                   /*查询的是根目录还是其他LOG目录*/
}DIAG_CMD_FS_QUERY_REQ;

typedef struct
{
    VOS_UINT32  ulRet;                          /*结果码*/
    VOS_UINT32  ulLength;                       /*目录长度*/
    VOS_UINT8   aucDirPath[DIAG_CMD_FILE_NAME_LEN];                  /*目录名*/
}DIAG_CMD_FS_QUERY_CNF;

/*****************************************************************************
 描述 : 该命令用来打开或创建文件
ID   : DIAG_CMD_FS_OPEN
REQ : DIAG_CMD_FS_OPEN_REQ
CNF : DIAG_CMD_FS_OPEN_CNF
*****************************************************************************/
typedef struct
{
    VOS_UINT32  ulMode;                         /*打开方式*/
    VOS_CHAR   szDirectory[MSP_DF_DIR_MAX_LEN]; /*打开路径*/
}DIAG_CMD_FS_OPEN_REQ;

typedef struct
{
    VOS_UINT32  ulRet;                          /*结果码*/
}DIAG_CMD_FS_OPEN_CNF;

/*****************************************************************************
 描述 : 该命令用来导出文件
ID   : DIAG_CMD_FS_EXPORT
REQ : DIAG_CMD_FS_EXPORT_REQ
CNF : DIAG_CMD_FS_EXPORT_CNF
*****************************************************************************/


typedef struct
{
    VOS_UINT32 ulRet;           /*结果码*/
    VOS_UINT32 ulSize;         /* 返回的实际大小, 不超过 DIAG_TRANSFER_BLOCK_MAX_SIZE*/
    VOS_UINT8  aucData[0];     /*导出内容*/
}DIAG_CMD_FS_EXPORT_CNF;

/*****************************************************************************
 描述 : 该命令用来导入文件
ID   : DIAG_CMD_FS_IMPORT
REQ : DIAG_CMD_FS_IMPORT_REQ
CNF : DIAG_CMD_FS_IMPORT_CNF
*****************************************************************************/
typedef struct
{
    VOS_UINT32  ulSize;                         /*待导入长度*/
    VOS_CHAR    acContent[0];                   /*导入内容*/
}DIAG_CMD_FS_IMPORT_REQ;

typedef struct
{
    VOS_UINT32  ulRet;/**/
}DIAG_CMD_FS_IMPORT_CNF;

/*****************************************************************************
 描述 : 该命令用来查询剩余及使用空间
ID   : DIAG_CMD_FS_SPACE
REQ : DIAG_CMD_FS_SPACE_REQ
CNF : DIAG_CMD_FS_SPACE_CNF
*****************************************************************************/
typedef struct
{
    VOS_CHAR    szDirectory[MSP_DF_DIR_MAX_LEN];  /*导入内容*/
}DIAG_CMD_FS_SPACE_REQ;

typedef struct
{
    VOS_UINT32        ulRet;            /*Result of operation*/
    VOS_UINT32        ulDiskSpace;
    VOS_UINT32        ulUsedSpace;
    VOS_UINT32        ulValidSpace;
}DIAG_CMD_FS_SPACE_CNF;

typedef struct
{
    VOS_UINT32 ulReserved;/*保留*/
} DIAG_CMD_REPLAY_SET_REQ_STRU;
typedef DIAG_CMD_COMMON_CNF DIAG_CMD_REPLAY_SET_CNF_STRU;
typedef struct
{
    VOS_UINT32 print_level;
} DIAG_bsp_log_swt_cfg_s;

typedef struct
{
    VOS_UINT32 ulRet;
} DIAG_BSP_PRINT_LOG_SWT_CNF_STRU;

typedef struct
{
    VOS_UINT32 trace_type;
    VOS_UINT32 trace_swt;
    VOS_UINT32 period;
} DIAG_BSP_SYVIEW_SWT_CFG_STRU;

typedef struct
{
    VOS_UINT32                task_id;
    VOS_UINT8                  name[12];
} DIAG_BSP_TASK_INFO_STRU;

typedef struct
{
    VOS_UINT32 trace_type;
    VOS_UINT32 ulRet;
    VOS_UINT32 ullen;
    DIAG_BSP_TASK_INFO_STRU st_task_info[0];
} DIAG_BSP_SYVIEW_SWT_CNF_STRU;



/*****************************************************************************
  6 UNION
*****************************************************************************/


/*****************************************************************************
  7 Extern Global Variable
*****************************************************************************/


/*****************************************************************************
  8 Fuction Extern
*****************************************************************************/


/*****************************************************************************
  9 OTHERS
*****************************************************************************/










#if (VOS_OS_VER == VOS_WIN32)
#pragma pack()
#else
#pragma pack(0)
#endif




#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of diag_cmd_param_plugin.h */
