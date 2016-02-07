/*
 *****************************************************************************
 * Copyright (c) 2001-2021, Huawei Tech. Co., Ltd. All rights reserved.
 *
 * File Name    : reset_secure.h
 * Author       : luyansheng 00212112
 * Created      : 2013/4/20
 * Description  : head file for image check, reference from secure.h
 *
 *****************************************************************************
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *****************************************************************************
 */
/*
 *****************************************************************************
 * 1 Other Header File Including
 *****************************************************************************
 */
#ifndef RESET_SECURE_H
#define RESET_SECURE_H

/*#include  ###*/

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*
 *****************************************************************************
 * 2 Macro Definition
 ****************************************************************************
 */
 /*tmp, need to make sure*/
#define DX_SB_HASH_LENGTH_IN_WORDS      (4)

/* 给每个ptable映像预留的最大的空间 */
#define PTABLE_MAX_SIZE         (1024*32)

/* VRL区的ptable区中的偏移 */
#define VRL_AREA1_OFFSET        (1024*256)

/* VRL备份区的ptable区中的偏移 */
#define VRL_AREA2_OFFSET        (1024*256 + 1024*128)

/* 给每个VRL预留的最大空间 */
#define VRL_SIZE                (1024*2)

/* 给workspace预留的空间,VRL大小加2MB(如C核的NV) */
#define WORKSPACE_SIZE          (VRL_SIZE + 1024*1024*2)

/* VRL中的区域长度定义 */
#define ADDITIONAL_DATA_SIZE    (128)
#define RSA_KEY_SIZE            (2048/8)
#define BARRETT_SIZE            (5*4)
#define SW_VER_SIZE             (8)
#define HASH_128_SIZE           (128/8)
#define HASH_256_SIZE           (256/8)
#define IMG_LOADADDR_SIZE       (4)
#define SECONDARY_VRLADDR_SIZE  (4)
#define IMG_INFO_SIZE           (8)

#ifndef CONFIG_ARM64
/* efuse操作相关的宏定义 */
/* 后续需要根据实际的APB时钟更新 */
#define APB_CLK             30000000
/* 烧写时序要求:
*  1.EFUSE_COUNT*Trefclk > 120ns,EFUSE_COUNT>=3. Trefclk = 1/APB_CLK,故EFUSE_COUNT取3
*  11us < (EFUSE_COUNT + PGM_COUNT) * Trefclk < 13us,取12us
*/
#define EFUSE_COUNT_CFG     3
#define PGM_COUNT_CFG       ((APB_CLK / 1000000) * 12 - EFUSE_COUNT_CFG)

/***************************** efuse register *****************************/
#define EFUSE_BASE_REG   (SOC_EFUSEC_BASE_ADDR)

#define EFUSEC_PG_EN     (1<<0)
#define EFUSEC_PRE_PG    (1<<1)
#define EFUSEC_RD_EN     (1<<2)
#define EFUSEC_SIG_SEL_INNER     (1<<3)

#define EFUSEC_PG_FIN (1<<0)
#define EFUSEC_RD_FIN (1<<1)

/* 烧写控制 */
#define EFUSEC_PG_CTRL_POS      0
#define EFUSEC_PG_CTRL_WIDTH    1
#define EFUSEC_PG_CTRL_MASK     (((1<<EFUSEC_PG_CTRL_WIDTH)-1)<<EFUSEC_PG_CTRL_POS)
#define EFUSEC_PG_CTRL_DIS      (1<<EFUSEC_PG_CTRL_POS)
/* 安全校验使能标志 */
#define EFUSEC_SEC_EN_POS       1
#define EFUSEC_SEC_EN_WIDTH     1
#define EFUSEC_SEC_EN_MASK      (((1<<EFUSEC_SEC_EN_WIDTH)-1)<<EFUSEC_SEC_EN_POS)
#define EFUSEC_SEC_EN           (1<<EFUSEC_SEC_EN_POS)
/* JTAG访问控制 */
#define EFUSEC_JTAG_CTRL_POS     2
#define EFUSEC_JTAG_CTRL_WIDTH   1
#define EFUSEC_JTAG_CTRL_MASK    (((1<<EFUSEC_JTAG_CTRL_WIDTH)-1)<<EFUSEC_JTAG_CTRL_POS)
#define EFUSEC_JTAG_CTRL_EN      (0<<EFUSEC_JTAG_CTRL_POS)

#define EFUSE_MAX_SIZE  512     /* Efuse大小 */
#define EFUSE_HAD_WRITED 1      /*Efuse 已经烧写*/
#define EFUSE_NOT_WRITED 0      /*Efuse 没有烧写*/

#define EFUSE_GRP_CFG			(EFUSE_MAX_SIZE/4 - 1)

#define EFUSE_OP_TIMEOUT_TIME   0x100000    /* Efuse操作超时最大延时时间 */


#define EFUSEC_CFG(BASE_ADDR)              ((BASE_ADDR)+0x0)
#define EFUSEC_PG_EN_MASK       (1<<0)
#define EFUSEC_PRE_PG_MASK      (1<<1)
#define EFUSEC_RD_EN_MASK       (1<<2)
#define EFUSEC_SIG_SEL_MASK     (1<<3)

#define EFUSEC_STATUS(BASE_ADDR)           ((BASE_ADDR)+0x04)
#define EFUSEC_PG_STATUS        (1<<0)
#define EFUSEC_RD_STATUS        (1<<1)
#define EFUSEC_PRE_PG_FIN       (1<<2)

#define EFUSE_GROUP(BASE_ADDR)             ((BASE_ADDR)+0x08)
#define EFUSE_GP_MASK           ((1<<7)-1)

#define EFUSE_PG_VALUE(BASE_ADDR)          ((BASE_ADDR)+0x0C)

#define EFUSEC_COUNT(BASE_ADDR)            ((BASE_ADDR)+0x10)
#define EFUSE_COUNT_MASK        ((1<<8)-1)

#define EFUSEC_PGM_COUNT(BASE_ADDR)        ((BASE_ADDR)+0x14)
#define EFUSE_PGM_COUNT_MASK    ((1<<16)-1)

#define EFUSEC_DATA(BASE_ADDR)             ((BASE_ADDR)+0x18)

#define EFUSEC_HW_CFG(BASE_ADDR)           ((BASE_ADDR)+0x1C)
/* 烧写控制 */
#define EFUSEC_PG_DIS           0x1

/* 启动设备选择 */
#define EFUSEC_BOOTMODE_WIDTH   3
#define EFUSEC_BOOTMODE_POS     3
#define EFUSEC_BOOTMODE_MASK     (((1<<EFUSEC_BOOTMODE_WIDTH)-1)<<EFUSEC_BOOTMODE_POS)

#define EFUSE_SECURITY_ENABLE_BIT  	(1UL << 26)

#define EFUSE_CARRIER_ID_BIT_START         (800)
#define EFUSE_OEM_SEL_BIT_START             (1024)
#define EFUSE_OEM_SEL_BIT_SIZE              (8)
#define EFUSE_CARRIER_ID_BIT_SIZE           (16)
#define EFUSE_HW_ID_BIT_SIZE                (16)
#define EFUSE_CARRIER_HW_BIT1_CNT_SIZE      (8)

#else
#define EFUSE_CARRIER_ID_BYTE_LENGTH       (2)
#endif

/* register control */
#define READ_REGISTER_UCHAR(addr) (*(volatile unsigned char *)(addr))
#define READ_REGISTER_USHORT(addr) (*(volatile unsigned short *)(addr))
#define READ_REGISTER_UINT(addr) (*(volatile unsigned int *)(addr))
#define READ_REGISTER_ULONG(addr) (*(volatile unsigned long *)(addr))

#define WRITE_REGISTER_UCHAR(addr,val) (*(volatile unsigned char *)(addr) = (val))
#define WRITE_REGISTER_USHORT(addr,val) (*(volatile unsigned short *)(addr) = (val))
#define WRITE_REGISTER_UINT(addr,val) (*(volatile unsigned int *)(addr) = (val))
#define WRITE_REGISTER_ULONG(addr,val) (*(volatile unsigned long *)(addr) = (val))

#define INREG8(x)           READ_REGISTER_UCHAR(x)
#define OUTREG8(x, y)       WRITE_REGISTER_UCHAR((x), (unsigned char)(y))
#define SETREG8(x, y)       OUTREG8((x), INREG8(x)|(y))
#define CLRREG8(x, y)       OUTREG8((x), INREG8(x)&~(y))
#define SETBITVALUE8(addr, mask, value) \
                OUTREG8(addr, (INREG8(addr)&(~mask)) | (value&mask))

#define INREG16(x)          READ_REGISTER_USHORT(x)
#define OUTREG16(x, y)      WRITE_REGISTER_USHORT(x,(unsigned short)(y))
#define SETREG16(x, y)      OUTREG16(x, INREG16(x)|(y))
#define CLRREG16(x, y)      OUTREG16(x, INREG16(x)&~(y))
#define SETBITVALUE16(addr, mask, value) \
                OUTREG16(addr, (INREG16(addr)&(~mask)) | (value&mask))

#define INREG32(x)          READ_REGISTER_UINT(x)
#define OUTREG32(x, y)      WRITE_REGISTER_UINT((x), (unsigned int)(y))
#define SETREG32(x, y)      OUTREG32((x), INREG32(x)|(y))
#define CLRREG32(x, y)      OUTREG32((x), INREG32(x)&~(y))
#define SETBITVALUE32(addr, mask, value)  OUTREG32((addr), (INREG32(addr)&(~(mask))) | ((value)&(mask)))

/*
 *****************************************************************************
 * 3 Enum Type Definition
 *****************************************************************************
 */

/* 各个映像的VRL id定义，用于在VRL区中索引各个映像的VRL */
typedef enum _vrl_id
{
    ID_NONE         = -3,
    ID_FASTBOOT1    = -2,
    ID_FASTBOOT2    = -1,
    ID_PTABLE       = 0,   /* 1~8 reserved for ptables with different emmc size */
    ID_MCU_EXE      = 9,
    ID_CP_OS        = 10,
    ID_CP_NV        = 11,
    ID_DSP_BIN      = 12,
    ID_HIFI_BIN     = 13,
    ID_TEE_OS       = 14,
    ID_RECOVERY_IMG = 15,
    ID_BOOT_IMG     = 16,
    ID_BUTTOM
}vrl_id;


/*
 *****************************************************************************
 * 4 Global Variable Declaring
 *****************************************************************************
 */

extern struct vrl_parse_stru g_vrl_parse_st;

extern unsigned int g_image_load_addr;

/*
 *****************************************************************************
 * 5 STRUCT Type Definition
 *****************************************************************************
 */

/* 防止串货的结构体 */
struct oem_stru {
    unsigned int flag;
    unsigned int carrier_id;
    unsigned int hw_id;
};

/* 从VRL解析出来的VRL长度以及映像长度 */
struct vrl_parse_stru {
    unsigned int vrl1_size;
    unsigned int vrl2_size;
    unsigned int image_size;
};

  //#define DX_NULL                        0UL

  /*typedef unsigned int           DxUint_t;
typedef unsigned char          DxUint8_t;
typedef unsigned short         DxUint16_t;
typedef unsigned long          DxUint32_t;
typedef unsigned long long     DxUint64_t;

typedef int                    DxInt_t;
typedef char                   DxInt8_t;
typedef short                  DxInt16_t;
typedef long                   DxInt32_t;
typedef long long              DxInt64_t;

typedef char                    DxChar_t;
typedef short                   DxWideChar_t;


//typedef unsigned char          uint8_t;

typedef unsigned char          uint8_t; */

//typedef unsigned long          uint32_t;

#define INT32_MAX              0x7FFFFFFFL

  //typedef DxUint8_t               DxByte_t;

/*typedef enum {
    DX_FALSE = 0,
    DX_TRUE = 1
    } EDxBool;*/

  //Typedef DxUint32_t              DxBool_t;
  //typedef DxUint32_t              DxStatus;

  //#define DxError_t               DxStatus
  //#define DX_SUCCESS              0UL
  //#define DX_FAIL			1UL
  //#define DX_INFINITE             0xFFFFFFFF


/*RSA defenitions*/
#define SB_RSA_MOD_SIZE_IN_WORDS 64
#define SB_RSA_HW_PKI_PKA_BARRETT_MOD_TAG_SIZE_IN_WORDS 5

/* VRL Flag offset */
#define SB_VRL_FLAG_OFFSET_IN_WORDS 2



/********* End of VRL structure definitions ***********/


/********* Supported algorithms definitions ***********/

/* HASH supported algorithms */
typedef enum {
	/* HASH SHA 256 output */
	HASH_SHA256_Alg_Output__ = 0x01,

	/* HASH SHA 256 output truncated to 128 bits */
	HASH_SHA256_Alg_128_Output__ = 0x02,

	/* Last */
	HASH_Last__              = 0x7FFFFFFF

}VRL_HashAlg_t__;


/* RSA supported algorithms */
typedef enum {
	/* RSA PSS 2048 after HASH SHA 256 */
	RSA_PSS_2048__           = 0x01,
	RSA_PKCS15_2048__	       = 0x02,

	/* Last */
	RSA_Last__               = 0x7FFFFFFF

}VRL_RSAAlg_t__;

/********* End of Supported algorithms definitions ***********/


/********* Function pointer definitions ***********/

/* Flash Read function pointer defintion, this function is used inside the secure boot APIs
   To read data from the Flash */

//typedef DxUint32_t (*DX_SB_FlashRead) (DxUint32_t flashAddress,	/* Flash address to read from */
//				       DxUint8_t *memDst,	/* memory destination to read the data to */
//				       DxUint32_t sizeToRead,	/* size to read from Flash (in bytes) */
//				       void* context);		/* context for user's needs */


/* Flash Write functions pointer definition, this function is used inside the secure boot APIs
   to write data to the Flash */
//typedef DxUint32_t (*DX_SB_FlashWrite) (DxUint32_t flashAddress, /* Flash address to write to */
//					DxUint8_t *memSrc,	 /* memory source to write the data from */
//					DxUint32_t sizeToWrite,	 /* size to write to Flash (in bytes) */
//					void* context);		 /* context for user's needs */

/* Get OTP HASH function pointer definition, this function is used to retrieve the OEM key HASH
   from OTP */
//typedef DxUint32_t (*Dx_SB_GetOTPHASH) (DxUint32_t hwBaseAddress, /* CC base address*/
//					DxUint32_t keyIndex, /* The OTP index*/
//					DxUint32_t *hashBuffer /* The HASH output*/);

/********* End of Function pointer definitions ***********/

/* VRL list MAX size */
#define VRL_LIST_MAX_SIZE                       20

/* If this value is set in the OTP index field than the HASH comparison is done with external HASH
   (and not HASH taken from the OTP) */
#define DX_SB_HASH_INDEX_NOT_USE_INTERNAL_HASH  0xFFFFFFFF






/* HASH minimum size */
#define DX_HASH_MIN_SIZE_IN_BYTES               64


/* Indication to load or not the component to memory */
#define DX_SW_COMP_NO_MEM_LOAD_INDICATION       0xFFFFFFFFUL

#define DX_ROM_PUBKEY_ARRAY_START_ADDR          0x1000

#ifdef DX_SB_ADDITIONAL_DATA_SUPPORTED
#define ADDITIONAL_DATA_SIZE_IN_BYTES           128
#define ADDITIONAL_DATA_SIZE_IN_WORDS		ADDITIONAL_DATA_SIZE_IN_BYTES/sizeof(DxUint32_t)
#endif


/* Definition for PKA offset (if required) */
#define DX_NOT_TO_SET_PKA_OFFSET	0xFFFFFFFF

/*
 *****************************************************************************
 * 6 UNION Type Definition
 *****************************************************************************
 */


/*
 *****************************************************************************
 * 7 OTHERS Definition
 *****************************************************************************
 */


/*
 *****************************************************************************
 * 8 Function Declare
 *****************************************************************************
 */

extern int check_secure_mode(void);
int check_image_in_emmc(int secure_image_id);
extern int check_image_in_ram(unsigned int *image_buf);
int secure_write_Image(void *pt, unsigned int data_addr, unsigned int size);
unsigned int get_fastboot1_size(void);
int parse_vrl(unsigned int * vrl_data,
               unsigned int *vrl_size,
               unsigned int *image_num,
               unsigned int *image_size);
int update_sw_min_version(void);
int check_fastboot_vrl(unsigned int image_buf);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
