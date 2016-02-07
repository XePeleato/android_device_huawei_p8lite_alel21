

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/

#ifndef __SOC_BASEADDR_INTERFACE_H__
#define __SOC_BASEADDR_INTERFACE_H__

#ifdef __cplusplus
    #if __cplusplus
        extern "C" {
    #endif
#endif
#if defined(BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
#ifdef BUILD_ASM
.globl	g_bbp_comm_base_addr
.globl	g_bbp_comm_on_base_addr
.globl	g_bbp_gsm0_on_base_addr
.globl	g_bbp_gsm1_on_base_addr
.globl	g_bbp_wcdma_on_base_addr
.globl	g_bbp_cmdm_on_base_addr
.globl	g_bbp_lte_on_base_addr
.globl	g_bbp_tds_on_base_addr
.globl	g_bbp_glb_on_base_addr
.globl	g_abb_cfg_base_addr
#else
extern unsigned int g_bbp_comm_base_addr ;
extern unsigned int g_bbp_comm_on_base_addr ;
extern unsigned int g_bbp_gsm0_on_base_addr ;
extern unsigned int g_bbp_gsm1_on_base_addr ;
extern unsigned int g_bbp_wcdma_on_base_addr ;
extern unsigned int g_bbp_cmdm_on_base_addr ;
extern unsigned int g_bbp_lte_on_base_addr ;
extern unsigned int g_bbp_tds_on_base_addr ;
extern unsigned int g_bbp_glb_on_base_addr ;
extern unsigned int g_abb_cfg_base_addr ;
#endif
#endif
/*****************************************************************************
  2 宏定义
*****************************************************************************/

/*  */
#define SOC_BOOTROM_BASE_ADDR                         (0xFFFF0000)

/*  */
#define SOC_SRAM_OFF_BASE_ADDR                        (0xFFF80000)

/*  */
#define SOC_Modem_SRAM_BASE_ADDR                      (0xFFB00000)

/*  */
#define SOC_Modem_A7_Private_BASE_ADDR                (0xFF400000)

/*  */
#define SOC_Tensilica_BBE16_DTCM_BASE_ADDR            (0xFF280000)

/*  */
#define SOC_Tensilica_BBE16_ITCM_BASE_ADDR            (0xFF200000)

/*  */
#define SOC_MODEMNOC_SERVICE_BASE_ADDR                (0xFF060000)

/*  */
#define SOC_UPACC_BASE_ADDR                           (0xFF051000)

/*  */
#define SOC_GUCipher_BASE_ADDR                        (0xFF050000)

/*  */
#define SOC_LCipher_BASE_ADDR                         (0xFF040000)

/*  */
#define SOC_Modem_DMAC_BASE_ADDR                      (0xFF032000)

/*  */
#define SOC_MUART0_BASE_ADDR                          (0xFF030000)

/*  */
#define SOC_MODEM_SCTRL_BASE_ADDR                     (0xFF00E000)

/*  */
#define SOC_MWatchDog1_BASE_ADDR                      (0xFF00D000)

/*  */
#define SOC_SCI1_BASE_ADDR                            (0xFF00C000)

/*  */
#define SOC_SCI0_BASE_ADDR                            (0xFF00B000)

/*  */
#define SOC_MWatchDog0_BASE_ADDR                      (0xFF00A000)

/*  */
#define SOC_MTimer9_BASE_ADDR                         (0xFF009000)

/*  */
#define SOC_MTimer8_BASE_ADDR                         (0xFF008000)

/*  */
#define SOC_MTimer7_BASE_ADDR                         (0xFF007000)

/*  */
#define SOC_MTimer6_BASE_ADDR                         (0xFF006000)

/*  */
#define SOC_MTimer5_BASE_ADDR                         (0xFF005000)

/*  */
#define SOC_MTimer4_BASE_ADDR                         (0xFF004000)

/*  */
#define SOC_MTimer3_BASE_ADDR                         (0xFF003000)

/*  */
#define SOC_MTimer2_BASE_ADDR                         (0xFF002000)

/*  */
#define SOC_MTimer1_BASE_ADDR                         (0xFF001000)

/*  */
#define SOC_MTimer0_BASE_ADDR                         (0xFF000000)

#if defined(BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
/*  */
#define SOC_BBP_COMM_BASE_ADDR                        (g_bbp_comm_base_addr)

/*  */
#define SOC_BBP_COMM_ON_BASE_ADDR                     (g_bbp_comm_on_base_addr)
#endif
/*  */
#define SOC_BBP_GSM0_BASE_ADDR                        (0xFD800000)
#if defined(BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
/*  */
#define SOC_BBP_GSM0_ON_BASE_ADDR                     (g_bbp_gsm0_on_base_addr)
#endif
/*  */
#define SOC_BBP_GSM1_BASE_ADDR                        (0xFD880000)
#if defined(BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
/*  */
#define SOC_BBP_GSM1_ON_BASE_ADDR                     (g_bbp_gsm1_on_base_addr)
#endif
/*  */
#define SOC_BBP_WCDMA_BASE_ADDR                       (0xFD900000)
#if defined(BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
/*  */
#define SOC_BBP_WCDMA_ON_BASE_ADDR                    (g_bbp_wcdma_on_base_addr)
#endif
/*  */
#define SOC_BBP_LTE_BASE_ADDR                         (0xFD000000)

/*  */
#define SOC_BBP_LTE0_BASE_ADDR                        (0xFD000000)

/*  */
#define SOC_BBP_LTE1_BASE_ADDR                        (0xFDA00000)

#if defined(BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
/*  */
#define SOC_BBP_LTE_ON_BASE_ADDR                      (g_bbp_lte_on_base_addr)
#define SOC_BBP_TDS_ON_BASE_ADDR                      (g_bbp_tds_on_base_addr)
#endif
#define SOC_BBP_INT_BASE_ADDR                         (0xFD700000)

/*  */
#define SOC_BBP_TDS_BASE_ADDR                         (0xFDD00000)
#if defined(BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
/*  */
#define SOC_ABB_CFG_BASE_ADDR                         (g_abb_cfg_base_addr)
#endif
/*  */
#define SOC_BBP_CDMA_BASE_ADDR                        (0xFD9E0000)
#if defined(BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
/*  */
#define SOC_BBP_CDMA_ON_BASE_ADDR                     (g_bbp_cdma_on_base_addr)

/*  */
#define SOC_BBP_GLB_ON_BASE_ADDR                      (g_bbp_glb_on_base_addr)
#endif
/*  */
#define SOC_HIFI2_ITCM_BASE_ADDR                      (0xFA080000)

/*  */
#define SOC_HIFI2_DTCM_BASE_ADDR                      (0xFA000000)

/*  */
#define SOC_XG2RAM0_HARQ_BASE_ADDR                    (0xF9800000)

/*  */
#define SOC_SYSNOC_SERVICE_AO_BASE_ADDR               (0xF900A000)

/*  */
#define SOC_SYSNOC_SERVICE_DMA_BASE_ADDR              (0xF9008000)

/*  */
#define SOC_SYSNOC_SERVICE_BASE_ADDR                  (0xF9000000)

/*  */
#define SOC_UART0_BASE_ADDR                           (0xF8015000)

/*  */
#define SOC_GPIO3_BASE_ADDR                           (0xF8014000)

/*  */
#define SOC_GPIO2_BASE_ADDR                           (0xF8013000)

/*  */
#define SOC_GPIO1_BASE_ADDR                           (0xF8012000)

/*  */
#define SOC_GPIO0_BASE_ADDR                           (0xF8011000)

/*  */
#define SOC_Timer8_BASE_ADDR                          (0xF8010000)

/*  */
#define SOC_Timer7_BASE_ADDR                          (0xF800F000)

/*  */
#define SOC_Timer6_BASE_ADDR                          (0xF800E000)

/*  */
#define SOC_Timer5_BASE_ADDR                          (0xF800D000)

/*  */
#define SOC_Timer4_BASE_ADDR                          (0xF800C000)

/*  */
#define SOC_Timer3_BASE_ADDR                          (0xF800B000)

/*  */
#define SOC_Timer2_BASE_ADDR                          (0xF800A000)

/*  */
#define SOC_Timer1_BASE_ADDR                          (0xF8009000)

/*  */
#define SOC_Timer0_BASE_ADDR                          (0xF8008000)

/*  */
#define SOC_Watchdog2_BASE_ADDR                       (0xF8007000)

/*  */
#define SOC_Watchdog1_BASE_ADDR                       (0xF8006000)

/*  */
#define SOC_Watchdog0_BASE_ADDR                       (0xF8005000)

/*  */
#define SOC_RTC1_BASE_ADDR                            (0xF8004000)

/*  */
#define SOC_RTC0_BASE_ADDR                            (0xF8003000)

/*  */
#define SOC_TZPC_BASE_ADDR                            (0xF8002000)

/*  */
#define SOC_IOC_ON_BASE_ADDR                          (0xF8001000)

/*  */
#define SOC_PMUSSI_BASE_ADDR                          (0xF8000000)

/*  */
#define SOC_EFUSEC_BASE_ADDR                          (0xF7802000)

/*  */
#define SOC_AO_SCTRL_BASE_ADDR                        (0xF7800000)

/*  */
#define SOC_IPC_S_BASE_ADDR                           (0xF7510000)

/*  */
#define SOC_IPC_BASE_ADDR                             (0xF7500000)

/*  */
#define SOC_AP_DMAC_BASE_ADDR                         (0xF7370000)

/*  */
#define SOC_SECENG_BASE_ADDR                          (0xF7360000)

/*  */
#define SOC_SOCP_BASE_ADDR                            (0xF7350000)

/*  */
#define SOC_IPF_BASE_ADDR                             (0xF7340000)

/*  */
#define SOC_USB_BASE_ADDR                             (0xF72C0000)

/*  */
#define SOC_NANDC_BASE_ADDR                           (0xF7280000)

/*  */
#define SOC_SDIOMMC_BASE_ADDR                         (0xF723F000)

/*  */
#define SOC_SDMMC_BASE_ADDR                           (0xF723E000)

/*  */
#define SOC_eMMC_BASE_ADDR                            (0xF723D000)

/*  */
#define SOC_DDRT_BASE_ADDR                            (0xF7130000)

/*  */
#define SOC_MDDRC_AXI_BASE_ADDR                        (0xF7120000)

/*  */
#define SOC_DIGACODEC_BASE_ADDR                       (0xF7118000)

/*  */
#define SOC_CODEC_SSI_BASE_ADDR                       (0xF7115000)

/*  */
#define SOC_UART4_BASE_ADDR                           (0xF7114000)

/*  */
#define SOC_UART3_BASE_ADDR                           (0xF7113000)

/*  */
#define SOC_UART2_BASE_ADDR                           (0xF7112000)

/*  */
#define SOC_UART1_BASE_ADDR                           (0xF7111000)

/*  */
#define SOC_BLPWM_BASE_ADDR                           (0xF7107000)

/*  */
#define SOC_SSP_BASE_ADDR                             (0xF7106000)

/*  */
#define SOC_I2C3_BASE_ADDR                            (0xF7103000)

/*  */
#define SOC_I2C2_BASE_ADDR                            (0xF7102000)

/*  */
#define SOC_I2C1_BASE_ADDR                            (0xF7101000)

/*  */
#define SOC_I2C0_BASE_ADDR                            (0xF7100000)

/*  */
#define SOC_PMCTRL_BASE_ADDR                          (0xF7032000)

/*  */
#define SOC_PERI_SCTRL_BASE_ADDR                      (0xF7030000)

/*  */
#define SOC_GPIO19_BASE_ADDR                          (0xF702F000)

/*  */
#define SOC_GPIO18_BASE_ADDR                          (0xF702E000)

/*  */
#define SOC_GPIO17_BASE_ADDR                          (0xF702D000)

/*  */
#define SOC_GPIO16_BASE_ADDR                          (0xF702C000)

/*  */
#define SOC_GPIO15_BASE_ADDR                          (0xF702B000)

/*  */
#define SOC_GPIO14_BASE_ADDR                          (0xF702A000)

/*  */
#define SOC_GPIO13_BASE_ADDR                          (0xF7029000)

/*  */
#define SOC_GPIO12_BASE_ADDR                          (0xF7028000)

/*  */
#define SOC_GPIO11_BASE_ADDR                          (0xF7027000)

/*  */
#define SOC_GPIO10_BASE_ADDR                          (0xF7026000)

/*  */
#define SOC_GPIO9_BASE_ADDR                           (0xF7025000)

/*  */
#define SOC_GPIO8_BASE_ADDR                           (0xF7024000)

/*  */
#define SOC_GPIO7_BASE_ADDR                           (0xF7023000)

/*  */
#define SOC_GPIO6_BASE_ADDR                           (0xF7022000)

/*  */
#define SOC_GPIO5_BASE_ADDR                           (0xF7021000)

/*  */
#define SOC_GPIO4_BASE_ADDR                           (0xF7020000)

/*  */
#define SOC_HKADC_BASE_ADDR                           (0xF7013000)

/*  */
#define SOC_PWM_BASE_ADDR                             (0xF7011000)

/*  */
#define SOC_PMUSSI1_BASE_ADDR                         (0xF7012000)

/*  */
#define SOC_IOC_OFF_BASE_ADDR                         (0xF7010000)

/*  */
#define SOC_ACPU_CCI_BASE_ADDR                        (0xF6E00000)

/*  */
#define SOC_ACPU_GIC_BASE_ADDR                        (0xF6800000)

/*  */
#define SOC_MODEM_A7_ROM_TABLE_BASE_ADDR              (0xF6600000)

/*  */
#define SOC_ACPU_CLUSTER1_BASE_ADDR                   (0xF65C0000)

/*  */
#define SOC_ACPU_CLUSTER0_BASE_ADDR                   (0xF6580000)

/*适配v8r1 v8r2 begin*/
/*  */
#define SOC_HI6220_ACPU_SCTRL_BASE_ADDR               (0xF6504000)

/*  */
#define SOC_HI6210_ACPU_SCTRL_BASE_ADDR               (0xF65A0000)
/*适配v8r1 v8r2 end*/
/*  */
#define SOC_ACPU_FUNNEL_BASE_ADDR                     (0xF6501000)

/*  */
#define SOC_ACPU_ROM_TABLE_BASE_ADDR                  (0xF6500000)

/*  */
#define SOC_CS_SYS_TPIU_BASE_ADDR                     (0xF6405000)

/*  */
#define SOC_CS_SYS_ETR_BASE_ADDR                      (0xF6404000)

/*  */
#define SOC_CS_SYS_CTI_BASE_ADDR                      (0xF6403000)

/*  */
#define SOC_CS_SYS_ETF_BASE_ADDR                      (0xF6402000)

/*  */
#define SOC_CS_SYS_FUNNEL_BASE_ADDR                   (0xF6401000)

/*  */
#define SOC_CS_SYS_ROM_TABLE_BASE_ADDR                (0xF6400000)

/*  */
#define SOC_CM3MTimer1_BASE_ADDR                      (0xF6185000)

/*  */
#define SOC_CM3Watchdog1_BASE_ADDR                    (0xF6183000)

/*  */
#define SOC_CM3Watchdog0_BASE_ADDR                    (0xF6182000)

/*  */
#define SOC_CM3MTimer0_BASE_ADDR                      (0xF6181000)

/*  */
#define SOC_SRAM_M3_BASE_ADDR                         (0xF6000000)

/*  */
#define SOC_MEDIA_XG2RAM_ADE_BASE_ADDR                (0xF5000000)

/*  */
#define SOC_MEDIA_XG2RAM_HARQ_BASE_ADDR               (0xF5000000)

/*  */
#define SOC_MEDIANOC_SERVICE_VIDEO_BASE_ADDR          (0xF4530000)

/*  */
#define SOC_MEDIANOC_SERVICE_ADE_BASE_ADDR            (0xF4520000)

/*  */
#define SOC_CSI_BASE_ADDR                             (0xF4510000)

/*  */
#define SOC_MEDIA_SCTRL_BASE_ADDR                     (0xF4410000)

/*  */
#define SOC_VPU_S_BASE_ADDR                           (0xF4311000)

/*  */
#define SOC_JPU_S_BASE_ADDR                           (0xF4310000)

/*  */
#define SOC_SMMU_S_BASE_ADDR                          (0xF4210000)

/*  */
#define SOC_ADE_S_BASE_ADDR                           (0xF4100000)

/*  */
#define SOC_G3D_S_BASE_ADDR                           (0xF4080000)

/*  */
#define SOC_ISP_S_BASE_ADDR                           (0xF4000000)

/*  */
#define SOC_DRAM_BASE_ADDR                            (0x00000000)

/*  */
#define SOC_MDDRC_SECURITY_BASE_ADDR                  (0xF7121000)

/*  */
#define SOC_MDDRC_QOS_BASE_ADDR                       (0xF7124000)

/*  */
#define SOC_MDDRC_DMC_BASE_ADDR                       (0xF7128000)

/*  */
#define SOC_MDDRC_PACK_BASE_ADDR                      (0xF712C000)

/*  */
#define SOC_MDDRC_dummyread_BASE_ADDR                 (0xF7120800)

/*  */
#define SOC_MDDRC_TEST_BASE_ADDR                      (0xF7120C00)





/*****************************************************************************
  3 枚举定义
*****************************************************************************/



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



/*****************************************************************************
  9 全局变量声明
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
extern unsigned int g_soc_acpu_base_addr;

/*****************************************************************************
  10 函数声明
*****************************************************************************/

/**/
#define SOC_ACPU_SCTRL_BASE_ADDR                      (g_soc_acpu_base_addr)
#endif
#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of soc_baseaddr_interface.h */
