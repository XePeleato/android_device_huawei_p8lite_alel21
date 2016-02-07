/*************************************************************************
*   版权所有(C) 1987-2011, 深圳华为技术有限公司.
*
*   文 件 名 :  version.h
*
*   作    者 :  yangzhi
*
*   描    述 :  本文件命名为"mdrv.h", 给出V7R1底软和协议栈之间的API接口统计
*
*   修改记录 :  2011年1月18日  v1.00  yangzhi创建
*************************************************************************/
#ifndef _MDRV_ACORE_VERSION_H_
#define _MDRV_ACORE_VERSION_H_

#ifdef _cplusplus
extern "C"
{
#endif


typedef enum{
     PROTOCOL_TYPE_LTE_ONLY    = 0,
     PROTOCOL_TYPE_GUL
}PROTOCOL_TYPE_E;



typedef enum{
	 BOARD_TYPE_BBIT    = 0,
	 BOARD_TYPE_SFT,
	 BOARD_TYPE_ASIC,
	 BOARD_TYPE_MAX
}BOARD_ACTUAL_TYPE_E;

typedef enum{
     PV500_CHIP             = 0,
     V7R1_CHIP              = 1,
     PV500_PILOT_CHIP,
     V7R1_PILOT_CHIP,
     V7R2_CHIP              = 5,
     V8R1_PILOT_CHIP        = 0x8,
}BSP_CHIP_TYPE_E;

typedef struct  tagMPRODUCT_INFO_S
{
	unsigned char *  productname;  /*芯片名称 */ /* BSP*/
	unsigned int   productnamelen;
	BSP_CHIP_TYPE_E echiptype;  /* PV500_CHIP V7R1_CHIP? */
	PROTOCOL_TYPE_E  eprotype;  /* GUL,ONLY LTE ? */
	unsigned int  platforminfo ;     /* FPGA ASIC?*/
	BOARD_ACTUAL_TYPE_E eboardatype;   /*BBIT SFT ASIC FOR */
} MPRODUCT_INFO_S;

#define BUILD_DATE_LEN  12
#define BUILD_TIME_LEN  12

/* 芯片Modem 的软件版本信息*/
typedef  struct  tagMSW_VER_INFO_S
{
	unsigned short  ulVVerNO;
	unsigned short  ulRVerNO;
	unsigned short  ulCVerNO;
	unsigned short  ulBVerNO;
	unsigned short  ulSpcNO;
	unsigned short  ulCustomNOv;
	unsigned int    ulProductNo;
	unsigned char  acBuildDate[BUILD_DATE_LEN];
	unsigned char  acBuildTime[BUILD_TIME_LEN];
} MSW_VER_INFO_S;

typedef  struct  tagMHW_VER_INFO_S
{
	int  hwindex;  /* HKADC检测到的硬件ID号(板级）*/
	unsigned int  hwidsub;  /* HKADC检测到的子版本号*/
	unsigned char *  hwfullver;  /* 硬件版本信息 MBB使用 */
	unsigned int  hwfullverlen;
	unsigned char *  hwname;  /* 外部产品名称  MBB使用 CPE? */
	unsigned int  hwnamelen;
	unsigned char *  hwinname;  /* 内部产品名称  MBB使用*/
	unsigned int  hwinnamelen;
} MHW_VER_INFO_S;


typedef struct  tagMODEM_VER_INFO_S
{
	MPRODUCT_INFO_S  stproductinfo;
	MSW_VER_INFO_S  stswverinfo;
	MHW_VER_INFO_S  sthwverinfo;
} MODEM_VER_INFO_S;


typedef enum
{
	VER_BOOTLOAD = 0,
	VER_BOOTROM = 1,
	VER_NV = 2,
	VER_VXWORKS = 3,
	VER_DSP = 4,
	VER_PRODUCT_ID = 5,
	VER_WBBP = 6,
	VER_PS = 7,
	VER_OAM = 8,
	VER_GBBP = 9,
	VER_SOC = 10,
	VER_HARDWARE = 11,
	VER_SOFTWARE = 12,
	VER_MEDIA = 13,
	VER_APP = 14,
	VER_ASIC = 15,
	VER_RF = 16,
	VER_PMU = 17,
	VER_PDM = 18,
	VER_PRODUCT_INNER_ID = 19,
	VER_INFO_NUM = 20
} COMP_TYPE_E;

/*****************************************************************************
 函 数 名  : mdrv_ver_get_memberinfo
 功能描述  : 查询所有组件的版本号。
 输入参数  : ppVersionInfo：待保存的版本信息地址。
 输出参数  : ucLength：待返回的数据的字节数。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
*****************************************************************************/
int mdrv_ver_get_memberinfo(void ** ppVersionInfo, unsigned int * ulLength);


/*****************************************************************************
 函 数 名  : mdrv_ver_memberctrl
 功能描述  : 组件版本读写接口。
 输入参数  : pcData：当Mode为读的时候，为调用者待保存返回的组件版本信息的内存地址；
                     当Mode为写的时候，为调用者准备写入的组件版本信息的地址。
             ucLength：当Mode为读的时候，为调用者待保存返回的组件版本信息的内存大小；
                       当Mode为写的时候，为调用者准备写入的组件版本信息的字符数（不包括'\0'）。
             ucType：版本信息ID
             ucMode：0：读取指定ID的组件版本信息；1：写入指定ID的组件版本信息。
 输出参数  : 无。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
*****************************************************************************/
int mdrv_ver_memberctrl(char *pcData, unsigned char ucLength, COMP_TYPE_E ucType, unsigned char ucMode);

/*****************************************************************************
 函 数 名  : mdrv_ver_get_info
 功能描述  : 返回modem版本信息
 输入参数  :
 输出参数  :
 返 回 值  : Modem 版本信息
 注意事项  : 对于v8，返回的全局变量中仅成员
             MODEM_VER_INFO_S->MPRODUCT_INFO_S.PROTOCOL_TYPE_E
             MODEM_VER_INFO_S->MSW_VER_INFO_S
             有效，其他成员值打桩
****************************************************************************/
const MODEM_VER_INFO_S * mdrv_ver_get_info(void);

#ifdef _cplusplus
}
#endif
#endif

