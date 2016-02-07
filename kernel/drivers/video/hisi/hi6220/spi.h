/*******************************************************************************
 * Copyright:   Copyright (c) 2007. Hisilicon Technologies, CO., LTD.
 * Version:     V300R001B04
 * Filename:    spi.h
 * Description: spi总线驱动头文件
 * History:
                1.Created by wubin on 2008/02/13
*******************************************************************************/

#ifndef __spi_H__
#define __spi_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "hi_type.h"
#include <linux/delay.h>
#include "soc_baseaddr_interface.h"

/* modify by gongyu lcd + spi0 */
#define SPI0_BASE                     (SOC_SSP_BASE_ADDR)


#define SPI2CR0         0x04c7  //scr取4，
                                //SPH、SPO根据Hi6421V100电源管理芯片用户指南2.2.9节都取1，
                                //字长度为8bit
#define SPI3CR0         0x04c7  //scr取4，
                                //SPH、SPO根据Hi6421V100电源管理芯片用户指南2.2.9节都取1，
                                //字长度为8bit
#define SPI3CR1         0x0008  //0x0009:Loopback模式；0x0008:禁止spi3，设为master

//在FPGA版本中，SPI的参考时钟是34MHz
#define SPI3CPSR        0x2     //cpsdvsr必须为偶数，取2，则分频比为2*（1+4）=10


#define READBIT         1
#define WRITEBIT        0
#define DUMMYDATA       0x00

//------------------------------------------------------------------------------
// REGISTER BIT FIELD POSITIONS (LEFT SHIFT)
//------------------------------------------------------------------------------
#define CSPI_SSPCR0_DSS_LSH         0
#define CSPI_SSPCR0_FRF_LSH         4
#define CSPI_SSPCR0_SPO_LSH         6
#define CSPI_SSPCR0_SPH_LSH         7
#define CSPI_SSPCR0_SCR_LSH         8

#define CSPI_SSPCR1_LBM_LSH         0
#define CSPI_SSPCR1_SSE_LSH         1
#define CSPI_SSPCR1_MS_LSH          2
#define CSPI_SSPCR1_SOD_LSH         3

#define CSPI_SSPDR_DATA_LSH         0

#define CSPI_SSPSR_TFE_LSH          0
#define CSPI_SSPSR_TNF_LSH          1
#define CSPI_SSPSR_RNE_LSH          2
#define CSPI_SSPSR_RFF_LSH          3
#define CSPI_SSPSR_BSY_LSH          4

#define CSPI_SSPCPSR_CPSDVSR_LSH    0

#define CSPI_SSPIMSC_RORIM_LSH      0
#define CSPI_SSPIMSC_RTIM_LSH       1
#define CSPI_SSPIMSC_RXIM_LSH       2
#define CSPI_SSPIMSC_TXIM_LSH       3

#define CSPI_SSPRIS_RORRIS_LSH      0
#define CSPI_SSPRIS_RTRIS_LSH       1
#define CSPI_SSPRIS_RXRIS_LSH       2
#define CSPI_SSPRIS_TXRIS_LSH       3

#define CSPI_SSPMIS_RORMIS_LSH      0
#define CSPI_SSPMIS_RTMIS_LSH       1
#define CSPI_SSPMIS_RXMIS_LSH       2
#define CSPI_SSPMIS_TXMIS_LSH       3

#define CSPI_SSPICR_RORIC_LSH       0
#define CSPI_SSPICR_RTIC_LSH        1

#define CSPI_SSPDMACR_RXDMAE_LSH    0
#define CSPI_SSPDMACR_TXDMAE_LSH    1

//------------------------------------------------------------------------------
// REGISTER BIT FIELD WIDTHS
//------------------------------------------------------------------------------
#define CSPI_SSPCR0_DSS_WID         4
#define CSPI_SSPCR0_FRF_WID         2
#define CSPI_SSPCR0_SPO_WID         1
#define CSPI_SSPCR0_SPH_WID         1
#define CSPI_SSPCR0_SCR_WID         8

#define CSPI_SSPCR1_LBM_WID         1
#define CSPI_SSPCR1_SSE_WID         1
#define CSPI_SSPCR1_MS_WID          1
#define CSPI_SSPCR1_SOD_WID         1

#define CSPI_SSPDR_DATA_WID         16

#define CSPI_SSPSR_TFE_WID          1
#define CSPI_SSPSR_TNF_WID          1
#define CSPI_SSPSR_RNE_WID          1
#define CSPI_SSPSR_RFF_WID          1
#define CSPI_SSPSR_BSY_WID          1

#define CSPI_SSPCPSR_CPSDVSR_WID    8

#define CSPI_SSPIMSC_RORIM_WID      1
#define CSPI_SSPIMSC_RTIM_WID       1
#define CSPI_SSPIMSC_RXIM_WID       1
#define CSPI_SSPIMSC_TXIM_WID       1

#define CSPI_SSPRIS_RORRIS_WID      1
#define CSPI_SSPRIS_RTRIS_WID       1
#define CSPI_SSPRIS_RXRIS_WID       1
#define CSPI_SSPRIS_TXRIS_WID       1

#define CSPI_SSPMIS_RORMIS_WID      1
#define CSPI_SSPMIS_RTMIS_WID       1
#define CSPI_SSPMIS_RXMIS_WID       1
#define CSPI_SSPMIS_TXMIS_WID       1

#define CSPI_SSPICR_RORIC_WID       1
#define CSPI_SSPICR_RTIC_WID        1

#define CSPI_SSPDMACR_RXDMAE_WID    1
#define CSPI_SSPDMACR_TXDMAE_WID    1




//------------------------------------------------------------------------------
// REGISTER BIT WRITE VALUES
//------------------------------------------------------------------------------
//SSPCR0
#define CSPI_SSPCR0_DSS_4BIT            0x03    // 4-bit transfer
#define CSPI_SSPCR0_DSS_5BIT            0x04    // 5-bit transfer
#define CSPI_SSPCR0_DSS_6BIT            0x05    // 6-bit transfer
#define CSPI_SSPCR0_DSS_7BIT            0x06    // 7-bit transfer
#define CSPI_SSPCR0_DSS_8BIT            0x07    // 8-bit transfer
#define CSPI_SSPCR0_DSS_9BIT            0x08    // 9-bit transfer
#define CSPI_SSPCR0_DSS_10BIT           0x09    // 10-bit transfer
#define CSPI_SSPCR0_DSS_11BIT           0x0A    // 11-bit transfer
#define CSPI_SSPCR0_DSS_12BIT           0x0B    // 12-bit transfer
#define CSPI_SSPCR0_DSS_13BIT           0x0C    // 13-bit transfer
#define CSPI_SSPCR0_DSS_14BIT           0x0D    // 14-bit transfer
#define CSPI_SSPCR0_DSS_15BIT           0x0E    // 15-bit transfer
#define CSPI_SSPCR0_DSS_16BIT           0x0F    // 16-bit transfer

#define CSPI_SSPCR0_FRF_SPI             0x00    // Motorola SPI frame format
#define CSPI_SSPCR0_FRF_TI              0x01    // TI synchronous serial frame format
#define CSPI_SSPCR0_FRF_NM              0x02    // National Microwire frame format
#define CSPI_SSPCR0_FRF_RESERVED        0x03    // Reserved

#define CSPI_SSPCR0_SPO_ACTIVE_LOW      0x0     // Active low polarity
#define CSPI_SSPCR0_SPO_ACTIVE_HIGH     0x1     // Active high polarity

#define CSPI_SSPCR0_SPH_TRAILING        0x0     // Phase 0 operation
#define CSPI_SSPCR0_SPH_LEADING         0x1     // Phase 1 operation

//SSPCR1
#define CSPI_SSPCR1_LBM_DISABLE         0x0     // Loop back mode disabled
#define CSPI_SSPCR1_LBM_ENABLE          0x1     // Loop back mode enabled

#define CSPI_SSPCR1_SSE_DISABLE         0x0     // SSP disabled
#define CSPI_SSPCR1_SSE_ENABLE          0x1     // SSP enabled

#define CSPI_SSPCR1_MS_MASTER           0x0     // Master mode
#define CSPI_SSPCR1_MS_SLAVE            0x1     // Slave mode

#define CSPI_SSPCR1_SOD_ENABLE          0x0     // Enable drive out in slave mode
#define CSPI_SSPCR1_SOD_DISABLE         0x1     // Disable drive out in slave mode

//SSPIMSC
#define CSPI_SSPIMSC_RORIM_DISABLE      0x0     // Disable Receive FIFO overrun interrupt
#define CSPI_SSPIMSC_RORIM_ENABLE       0x1     // Enable Receive FIFO overrun interrupt

#define CSPI_SSPIMSC_RTIM_DISABLE       0x0     // Disable receive overtime interrupt
#define CSPI_SSPIMSC_RTIM_ENABLE        0x1     // Enable receive overtime interrupt

#define CSPI_SSPIMSC_RXIM_DISABLE       0x0     // Disable RxFIFO half interrupt
#define CSPI_SSPIMSC_RXIM_ENABLE        0x1     // Enable RxFIFO half interrupt

#define CSPI_SSPIMSC_TXIM_DISABLE       0x0     // Disable TxFIFO half interrupt
#define CSPI_SSPIMSC_TXIM_ENABLE        0x1     // Enable TxFIFO half interrupt



// Bitfield macros that use rely on bitfield width/shift information
// defined in CSP header files
#define CSP_BITFMASK(bit) (((1U << (bit ## _WID)) - 1) << (bit ## _LSH))
#define CSP_BITFVAL(bit, val) ((val) << (bit ## _LSH))

// Undefine previous implementations of peripheral access macros since
// we want to "own" the definitions and avoid redefinition warnings
// resulting from source code that includes oal_io.h
#undef INREG8
#undef OUTREG8
#undef SETREG8
#undef CLRREG8
#undef INREG16
#undef OUTREG16
#undef SETREG16
#undef CLRREG16
#undef INREG32
#undef OUTREG32
#undef SETREG32
#undef CLRREG32

#define INREG32(reg)  (*(unsigned int *)(reg))
#define OUTREG32(reg, value) (*((unsigned int *)(reg)) = (value))
#define INREG16(reg)  (*(unsigned short *)(reg))
#define OUTREG16(reg, value) (*((unsigned short *)(reg)) = (value))
#define INREG8(reg)  (*(unsigned char *)(reg))
#define OUTREG8(reg, value) (*((unsigned char *)(reg)) = (value))


// Macros for accessing peripheral registers using DDK macros/functions
//#define INREG8(x)           READ_REGISTER_UCHAR((UCHAR*)(x))
//#define OUTREG8(x, y)       WRITE_REGISTER_UCHAR((UCHAR*)(x), (UCHAR)(y))
#define SETREG8(x, y)       OUTREG8(x, INREG8(x)|(y))
#define CLRREG8(x, y)       OUTREG8(x, INREG8(x)&~(y))
#define INSREG8(addr, mask, val) OUTREG8(addr, ((INREG8(addr)&(~(mask))) | val))
#define EXTREG8(addr, mask, lsh) ((INREG8(addr) & mask) >> lsh)
#define EXTREG8BF(addr, bit) (EXTREG8(addr, CSP_BITFMASK(bit), (bit ## _LSH)))
#define INSREG8BF(addr, bit, val) (INSREG8(addr, CSP_BITFMASK(bit), CSP_BITFVAL(bit, val)))

//#define INREG16(x)          READ_REGISTER_USHORT((USHORT*)(x))
//#define OUTREG16(x, y)      WRITE_REGISTER_USHORT((USHORT*)(x),(USHORT)(y))
#define SETREG16(x, y)      OUTREG16(x, INREG16(x)|(y))
#define CLRREG16(x, y)      OUTREG16(x, INREG16(x)&~(y))
#define INSREG16(addr, mask, val) OUTREG16(addr, ((INREG16(addr)&(~(mask))) | val))
#define EXTREG16(addr, mask, lsh) ((INREG16(addr) & mask) >> lsh)
#define EXTREG16BF(addr, bit) (EXTREG16(addr, CSP_BITFMASK(bit), (bit ## _LSH)))
#define INSREG16BF(addr, bit, val) (INSREG16(addr, CSP_BITFMASK(bit), CSP_BITFVAL(bit, val)))

//#define INREG32(x)          READ_REGISTER_ULONG((ULONG*)(x))
//#define OUTREG32(x, y)      WRITE_REGISTER_ULONG((ULONG*)(x), (ULONG)(y))
#define SETREG32(x, y)      OUTREG32(x, INREG32(x)|(y))
#define CLRREG32(x, y)      OUTREG32(x, INREG32(x)&~(y))
#define INSREG32(addr, mask, val) OUTREG32(addr, ((INREG32(addr)&(~(mask))) | val))
#define EXTREG32(addr, mask, lsh) ((INREG32(addr) & mask) >> lsh)
#define EXTREG32BF(addr, bit) (EXTREG32(addr, CSP_BITFMASK(bit), (bit ## _LSH)))
#define INSREG32BF(addr, bit, val) (INSREG32(addr, CSP_BITFMASK(bit), CSP_BITFVAL(bit, val)))




//------------------------------------------------------------------------------
// REGISTER LAYOUT
//------------------------------------------------------------------------------
typedef struct
{
    UINT32 SSPCR0;
    UINT32 SSPCR1;
    UINT32 SSPDR;
    UINT32 SSPSR;
    UINT32 SSPCPSR;
    UINT32 SSPIMSC;
    UINT32 SSPRIS;
    UINT32 reserv;      //SSPMIS; //modify by gongyu
    UINT32 SSPICR;
    UINT32 SSPDMACR;
} SPI_REG, *PSPI_REG;




#ifdef __cplusplus
}
#endif


#endif // __CSPICLASS_H__

