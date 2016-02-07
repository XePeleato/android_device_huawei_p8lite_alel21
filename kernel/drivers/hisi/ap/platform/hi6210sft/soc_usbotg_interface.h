

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/

#ifndef __SOC_USBOTG_INTERFACE_H__
#define __SOC_USBOTG_INTERFACE_H__

#ifdef __cplusplus
    #if __cplusplus
        extern "C" {
    #endif
#endif



/*****************************************************************************
  2 宏定义
*****************************************************************************/

/****************************************************************************
                     (1/1) register_define
 ****************************************************************************/
/* 寄存器说明：全局控制和状态寄存器。
   位域定义UNION结构:  SOC_USBOTG_GOTGCTL_UNION */
#define SOC_USBOTG_GOTGCTL_ADDR(base)                 ((base) + (0x0000))

/* 寄存器说明：全局OTG中断状态寄存器。
   位域定义UNION结构:  SOC_USBOTG_GOTGINT_UNION */
#define SOC_USBOTG_GOTGINT_ADDR(base)                 ((base) + (0x0004))

/* 寄存器说明：全局AHB配置寄存器。
   位域定义UNION结构:  SOC_USBOTG_GAHBCFG_UNION */
#define SOC_USBOTG_GAHBCFG_ADDR(base)                 ((base) + (0x0008))

/* 寄存器说明：全局USB配置寄存器。
   位域定义UNION结构:  SOC_USBOTG_GUSBCFG_UNION */
#define SOC_USBOTG_GUSBCFG_ADDR(base)                 ((base) + (0x000C))

/* 寄存器说明：全局复位控制寄存器。
   位域定义UNION结构:  SOC_USBOTG_GRSTCTL_UNION */
#define SOC_USBOTG_GRSTCTL_ADDR(base)                 ((base) + (0x0010))

/* 寄存器说明：全局中断状态寄存器。
   位域定义UNION结构:  SOC_USBOTG_GINTSTS_UNION */
#define SOC_USBOTG_GINTSTS_ADDR(base)                 ((base) + (0x0014))

/* 寄存器说明：全局中断屏蔽寄存器。
   位域定义UNION结构:  SOC_USBOTG_GINTMSK_UNION */
#define SOC_USBOTG_GINTMSK_ADDR(base)                 ((base) + (0x0018))

/* 寄存器说明：全局接收FIFO状态读出寄存器。
   位域定义UNION结构:  SOC_USBOTG_GRXSTSR_UNION */
#define SOC_USBOTG_GRXSTSR_ADDR(base)                 ((base) + (0x001C))

/* 寄存器说明：全局接收FIFO状态弹出寄存器。
   位域定义UNION结构:  SOC_USBOTG_GRXSTSP_UNION */
#define SOC_USBOTG_GRXSTSP_ADDR(base)                 ((base) + (0x0020))

/* 寄存器说明：全局接收FIFO大小寄存器。
   位域定义UNION结构:  SOC_USBOTG_GRXFSIZ_UNION */
#define SOC_USBOTG_GRXFSIZ_ADDR(base)                 ((base) + (0x0024))

/* 寄存器说明：全局非周期发送FIFO大小寄存器。
   位域定义UNION结构:  SOC_USBOTG_GNPTXFSIZ_UNION */
#define SOC_USBOTG_GNPTXFSIZ_ADDR(base)               ((base) + (0x0028))

/* 寄存器说明：全局非周期发送FIFO状态寄存器。
   位域定义UNION结构:  SOC_USBOTG_GNPTXSTS_UNION */
#define SOC_USBOTG_GNPTXSTS_ADDR(base)                ((base) + (0x002C))

/* 寄存器说明：USB IP版本寄存器。
   位域定义UNION结构:  SOC_USBOTG_GSNPSID_UNION */
#define SOC_USBOTG_GSNPSID_ADDR(base)                 ((base) + (0x0040))

/* 寄存器说明：硬件配置寄存器1。
   位域定义UNION结构:  SOC_USBOTG_GHWCFG1_UNION */
#define SOC_USBOTG_GHWCFG1_ADDR(base)                 ((base) + (0x0044))

/* 寄存器说明：硬件配置寄存器2。
   位域定义UNION结构:  SOC_USBOTG_GHWCFG2_UNION */
#define SOC_USBOTG_GHWCFG2_ADDR(base)                 ((base) + (0x0048))

/* 寄存器说明：硬件配置寄存器3。
   位域定义UNION结构:  SOC_USBOTG_GHWCFG3_UNION */
#define SOC_USBOTG_GHWCFG3_ADDR(base)                 ((base) + (0x004C))

/* 寄存器说明：硬件配置寄存器4。
   位域定义UNION结构:  SOC_USBOTG_GHWCFG4_UNION */
#define SOC_USBOTG_GHWCFG4_ADDR(base)                 ((base) + (0x0050))

/* 寄存器说明：LPM配置寄存器。
   位域定义UNION结构:  SOC_USBOTG_GLPMCFG_UNION */
#define SOC_USBOTG_GLPMCFG_ADDR(base)                 ((base) + (0x0054))

/* 寄存器说明：冬眠模式掉电控制寄存器。
   位域定义UNION结构:  SOC_USBOTG_GPWRDN_UNION */
#define SOC_USBOTG_GPWRDN_ADDR(base)                  ((base) + (0x0058))

/* 寄存器说明：动态FIFO配置寄存器。
   位域定义UNION结构:  SOC_USBOTG_GDFIFOCFG_UNION */
#define SOC_USBOTG_GDFIFOCFG_ADDR(base)               ((base) + (0x005C))

/* 寄存器说明：ADP控制寄存器。
   位域定义UNION结构:  SOC_USBOTG_GADPCTL_UNION */
#define SOC_USBOTG_GADPCTL_ADDR(base)                 ((base) + (0x0060))

/* 寄存器说明：Host周期发送FIFO配置寄存器。
   位域定义UNION结构:  SOC_USBOTG_HPTXFSIZ_UNION */
#define SOC_USBOTG_HPTXFSIZ_ADDR(base)                ((base) + (0x0100))

/* 寄存器说明：Device IN端点1发送FIFO配置寄存器。
   位域定义UNION结构:  SOC_USBOTG_DIEPTXF1_UNION */
#define SOC_USBOTG_DIEPTXF1_ADDR(base)                ((base) + (0x0104))

/* 寄存器说明：Device IN端点2发送FIFO配置寄存器。
   位域定义UNION结构:  SOC_USBOTG_DIEPTXF2_UNION */
#define SOC_USBOTG_DIEPTXF2_ADDR(base)                ((base) + (0x0108))

/* 寄存器说明：Device IN端点3发送FIFO配置寄存器。
   位域定义UNION结构:  SOC_USBOTG_DIEPTXF3_UNION */
#define SOC_USBOTG_DIEPTXF3_ADDR(base)                ((base) + (0x010C))

/* 寄存器说明：Device IN端点4发送FIFO配置寄存器。
   位域定义UNION结构:  SOC_USBOTG_DIEPTXF4_UNION */
#define SOC_USBOTG_DIEPTXF4_ADDR(base)                ((base) + (0x0110))

/* 寄存器说明：Device IN端点5发送FIFO配置寄存器。
   位域定义UNION结构:  SOC_USBOTG_DIEPTXF5_UNION */
#define SOC_USBOTG_DIEPTXF5_ADDR(base)                ((base) + (0x0114))

/* 寄存器说明：Device IN端点6发送FIFO配置寄存器。
   位域定义UNION结构:  SOC_USBOTG_DIEPTXF6_UNION */
#define SOC_USBOTG_DIEPTXF6_ADDR(base)                ((base) + (0x0118))

/* 寄存器说明：Device IN端点7发送FIFO配置寄存器。
   位域定义UNION结构:  SOC_USBOTG_DIEPTXF7_UNION */
#define SOC_USBOTG_DIEPTXF7_ADDR(base)                ((base) + (0x011C))

/* 寄存器说明：Device IN端点8发送FIFO配置寄存器。
   位域定义UNION结构:  SOC_USBOTG_DIEPTXF8_UNION */
#define SOC_USBOTG_DIEPTXF8_ADDR(base)                ((base) + (0x0120))

/* 寄存器说明：Device IN端点9发送FIFO配置寄存器。
   位域定义UNION结构:  SOC_USBOTG_DIEPTXF9_UNION */
#define SOC_USBOTG_DIEPTXF9_ADDR(base)                ((base) + (0x0124))

/* 寄存器说明：Device IN端点10发送FIFO配置寄存器。
   位域定义UNION结构:  SOC_USBOTG_DIEPTXF10_UNION */
#define SOC_USBOTG_DIEPTXF10_ADDR(base)               ((base) + (0x0128))

/* 寄存器说明：Device IN端点11发送FIFO配置寄存器。
   位域定义UNION结构:  SOC_USBOTG_DIEPTXF11_UNION */
#define SOC_USBOTG_DIEPTXF11_ADDR(base)               ((base) + (0x012C))

/* 寄存器说明：Device IN端点12发送FIFO配置寄存器。
   位域定义UNION结构:  SOC_USBOTG_DIEPTXF12_UNION */
#define SOC_USBOTG_DIEPTXF12_ADDR(base)               ((base) + (0x0130))

/* 寄存器说明：Device IN端点13发送FIFO配置寄存器。
   位域定义UNION结构:  SOC_USBOTG_DIEPTXF13_UNION */
#define SOC_USBOTG_DIEPTXF13_ADDR(base)               ((base) + (0x0134))

/* 寄存器说明：Device IN端点14发送FIFO配置寄存器。
   位域定义UNION结构:  SOC_USBOTG_DIEPTXF14_UNION */
#define SOC_USBOTG_DIEPTXF14_ADDR(base)               ((base) + (0x0138))

/* 寄存器说明：Device IN端点15发送FIFO配置寄存器。
   位域定义UNION结构:  SOC_USBOTG_DIEPTXF15_UNION */
#define SOC_USBOTG_DIEPTXF15_ADDR(base)               ((base) + (0x013C))

/* 寄存器说明：Host配置寄存器。
   位域定义UNION结构:  SOC_USBOTG_HCFG_UNION */
#define SOC_USBOTG_HCFG_ADDR(base)                    ((base) + (0x0400))

/* 寄存器说明：Host帧间隔寄存器。
   位域定义UNION结构:  SOC_USBOTG_HFIR_UNION */
#define SOC_USBOTG_HFIR_ADDR(base)                    ((base) + (0x0404))

/* 寄存器说明：Host帧序号寄存器。
   位域定义UNION结构:  SOC_USBOTG_HFNUM_UNION */
#define SOC_USBOTG_HFNUM_ADDR(base)                   ((base) + (0x0408))

/* 寄存器说明：Host周期发送FIFO和队列状态寄存器。
   位域定义UNION结构:  SOC_USBOTG_HPTXSTS_UNION */
#define SOC_USBOTG_HPTXSTS_ADDR(base)                 ((base) + (0x0410))

/* 寄存器说明：Host所有通道中断状态寄存器。
   位域定义UNION结构:  SOC_USBOTG_HAINT_UNION */
#define SOC_USBOTG_HAINT_ADDR(base)                   ((base) + (0x0414))

/* 寄存器说明：Host所用通道中断屏蔽寄存器。
   位域定义UNION结构:  SOC_USBOTG_HAINTMSK_UNION */
#define SOC_USBOTG_HAINTMSK_ADDR(base)                ((base) + (0x0418))

/* 寄存器说明：帧列表基地址寄存器。
   位域定义UNION结构:  SOC_USBOTG_HFLBADDR_UNION */
#define SOC_USBOTG_HFLBADDR_ADDR(base)                ((base) + (0x041C))

/* 寄存器说明：Host端口控制和状态寄存器。
   位域定义UNION结构:  SOC_USBOTG_HPRT_UNION */
#define SOC_USBOTG_HPRT_ADDR(base)                    ((base) + (0x0440))

/* 寄存器说明：通道p特性寄存器。
   位域定义UNION结构:  SOC_USBOTG_HCCHARP_UNION */
#define SOC_USBOTG_HCCHARP_ADDR(base)                 ((base) + (0x0500+(0x20*i)))

/* 寄存器说明：通道p Split控制寄存器。
   位域定义UNION结构:  SOC_USBOTG_HCSPLTP_UNION */
#define SOC_USBOTG_HCSPLTP_ADDR(base)                 ((base) + (0x0504+(0x20*i)))

/* 寄存器说明：通道p中断状态寄存器。
   位域定义UNION结构:  SOC_USBOTG_HCINTP_UNION */
#define SOC_USBOTG_HCINTP_ADDR(base)                  ((base) + (0x0508+(0x20*i)))

/* 寄存器说明：通道p中断屏蔽寄存器。
   位域定义UNION结构:  SOC_USBOTG_HCINTMSKP_UNION */
#define SOC_USBOTG_HCINTMSKP_ADDR(base)               ((base) + (0x050C+(0x20*i)))

/* 寄存器说明：通道p传输长度寄存器。（scatter / gather DMA模式）
   位域定义UNION结构:  SOC_USBOTG_HCTSIZP_UNION */
#define SOC_USBOTG_HCTSIZP_ADDR(base)                 ((base) + (0x0510+(0x20*i)))

/* 寄存器说明：通道p传输长度寄存器。（非scatter / gather DMA模式）
   位域定义UNION结构:  SOC_USBOTG_HCTSIZP_NON_SG_UNION */
#define SOC_USBOTG_HCTSIZP_NON_SG_ADDR(base)          ((base) + (0x0510+(0x20*i)))

/* 寄存器说明：通道p DMA地址寄存器。
   位域定义UNION结构:  SOC_USBOTG_HCDMAP_UNION */
#define SOC_USBOTG_HCDMAP_ADDR(base)                  ((base) + (0x0514+(0x20*i)))

/* 寄存器说明：通道p DMA buffer地址寄存器。
   位域定义UNION结构:  SOC_USBOTG_HCDMABP_UNION */
#define SOC_USBOTG_HCDMABP_ADDR(base)                 ((base) + (0x051C+(0x20*i)))

/* 寄存器说明：device配置寄存器。
   位域定义UNION结构:  SOC_USBOTG_DCFG_UNION */
#define SOC_USBOTG_DCFG_ADDR(base)                    ((base) + (0x0800))

/* 寄存器说明：device控制寄存器。
   位域定义UNION结构:  SOC_USBOTG_DCTL_UNION */
#define SOC_USBOTG_DCTL_ADDR(base)                    ((base) + (0x0804))

/* 寄存器说明：device状态寄存器。
   位域定义UNION结构:  SOC_USBOTG_DSTS_UNION */
#define SOC_USBOTG_DSTS_ADDR(base)                    ((base) + (0x0808))

/* 寄存器说明：IN端点普通中断屏蔽寄存器。
   位域定义UNION结构:  SOC_USBOTG_DIEPMSK_UNION */
#define SOC_USBOTG_DIEPMSK_ADDR(base)                 ((base) + (0x0810))

/* 寄存器说明：OUT端点普通中断屏蔽寄存器。
   位域定义UNION结构:  SOC_USBOTG_DOEPMSK_UNION */
#define SOC_USBOTG_DOEPMSK_ADDR(base)                 ((base) + (0x0814))

/* 寄存器说明：所有端点中断状态寄存器。
   位域定义UNION结构:  SOC_USBOTG_DAINT_UNION */
#define SOC_USBOTG_DAINT_ADDR(base)                   ((base) + (0x0818))

/* 寄存器说明：所有端点中断屏蔽寄存器。
   位域定义UNION结构:  SOC_USBOTG_DAINTMSK_UNION */
#define SOC_USBOTG_DAINTMSK_ADDR(base)                ((base) + (0x081C))

/* 寄存器说明：Vbus放电时间配置寄存器。
   位域定义UNION结构:  SOC_USBOTG_DVBUSDIS_UNION */
#define SOC_USBOTG_DVBUSDIS_ADDR(base)                ((base) + (0x0828))

/* 寄存器说明：Vbus脉冲配置寄存器。
   位域定义UNION结构:  SOC_USBOTG_DVBUSPULSE_UNION */
#define SOC_USBOTG_DVBUSPULSE_ADDR(base)              ((base) + (0x082C))

/* 寄存器说明：device水线控制寄存器。
   位域定义UNION结构:  SOC_USBOTG_DTHRCTL_UNION */
#define SOC_USBOTG_DTHRCTL_ADDR(base)                 ((base) + (0x0830))

/* 寄存器说明：IN端点FIFO空中断屏蔽寄存器。
   位域定义UNION结构:  SOC_USBOTG_DIEPEMPMSK_UNION */
#define SOC_USBOTG_DIEPEMPMSK_ADDR(base)              ((base) + (0x0834))

/* 寄存器说明：IN端点0控制寄存器。
   位域定义UNION结构:  SOC_USBOTG_DIEPCTL0_UNION */
#define SOC_USBOTG_DIEPCTL0_ADDR(base)                ((base) + (0x0900))

/* 寄存器说明：IN端点n控制寄存器。
   位域定义UNION结构:  SOC_USBOTG_DIEPCTLN_UNION */
#define SOC_USBOTG_DIEPCTLN_ADDR(base, n)             ((base) + (0x0900+(0x20*(n))))

/* 寄存器说明：IN端点m中断状态寄存器。
   位域定义UNION结构:  SOC_USBOTG_DIEPINTM_UNION */
#define SOC_USBOTG_DIEPINTM_ADDR(base, m)             ((base) + (0x0908+(0x20*(m))))

/* 寄存器说明：IN端点0传输大小寄存器。
   位域定义UNION结构:  SOC_USBOTG_DIEPTSIZ0_UNION */
#define SOC_USBOTG_DIEPTSIZ0_ADDR(base)               ((base) + (0x0910))

/* 寄存器说明：IN端点n传输大小寄存器。
   位域定义UNION结构:  SOC_USBOTG_DIEPTSIZN_UNION */
#define SOC_USBOTG_DIEPTSIZN_ADDR(base, n)            ((base) + (0x0910+(0x20*(n))))

/* 寄存器说明：IN端点m DMA地址寄存器。
   位域定义UNION结构:  SOC_USBOTG_DIEPDMAM_UNION */
#define SOC_USBOTG_DIEPDMAM_ADDR(base, m)             ((base) + (0x0914+(0x20*(m))))

/* 寄存器说明：IN端点m发送FIFO状态寄存器。
   位域定义UNION结构:  SOC_USBOTG_DTXFSTSM_UNION */
#define SOC_USBOTG_DTXFSTSM_ADDR(base)                ((base) + (0x0918))

/* 寄存器说明：IN端点m DMA buffer地址寄存器。
   位域定义UNION结构:  SOC_USBOTG_DIEPDMABN_UNION */
#define SOC_USBOTG_DIEPDMABN_ADDR(base, m)            ((base) + (0x091C+(0x20*(m))))

/* 寄存器说明：OUT端点0控制寄存器。
   位域定义UNION结构:  SOC_USBOTG_DOEPCTL0_UNION */
#define SOC_USBOTG_DOEPCTL0_ADDR(base)                ((base) + (0x0B00))

/* 寄存器说明：OUT端点n控制寄存器。
   位域定义UNION结构:  SOC_USBOTG_DOEPCTLN_UNION */
#define SOC_USBOTG_DOEPCTLN_ADDR(base, n)             ((base) + (0x0B00+(0x20*(n))))

/* 寄存器说明：OUT端点m中断状态寄存器。
   位域定义UNION结构:  SOC_USBOTG_DOEPINTM_UNION */
#define SOC_USBOTG_DOEPINTM_ADDR(base, m)             ((base) + (0x0B08+(0x20*(m))))

/* 寄存器说明：OUT端点0传输大小寄存器。
   位域定义UNION结构:  SOC_USBOTG_DOEPTSIZ0_UNION */
#define SOC_USBOTG_DOEPTSIZ0_ADDR(base)               ((base) + (0x0B10))

/* 寄存器说明：OUT端点n传输大小寄存器。
   位域定义UNION结构:  SOC_USBOTG_DOEPTSIZN_UNION */
#define SOC_USBOTG_DOEPTSIZN_ADDR(base, n)            ((base) + (0x0B10+(0x20*(n))))

/* 寄存器说明：OUT端点m DMA地址寄存器。
   位域定义UNION结构:  SOC_USBOTG_DOEPDMAM_UNION */
#define SOC_USBOTG_DOEPDMAM_ADDR(base, m)             ((base) + (0x0B14+(0x20*(m))))

/* 寄存器说明：OUT端点m DMA buffer地址寄存器。
   位域定义UNION结构:  SOC_USBOTG_DOEPDMABM_UNION */
#define SOC_USBOTG_DOEPDMABM_ADDR(base, m)            ((base) + (0x0B1C+(0x20*(m))))

/* 寄存器说明：功耗和时钟门控控制寄存器。
   位域定义UNION结构:  SOC_USBOTG_PCGCCTL_UNION */
#define SOC_USBOTG_PCGCCTL_ADDR(base)                 ((base) + (0x0E00))





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

/****************************************************************************
                     (1/1) register_define
 ****************************************************************************/
/*****************************************************************************
 结构名    : SOC_USBOTG_GOTGCTL_UNION
 结构说明  : GOTGCTL 寄存器结构定义。地址偏移量:0x0000，初值:0x00010000，宽度:32
 寄存器说明: 全局控制和状态寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sesreqscs    : 1;  /* bit[0]    : 会话请求是否成功。
                                                        0：失败；
                                                        1：成功。 */
        unsigned int  sesreq       : 1;  /* bit[1]    : 会话请求。
                                                        0：无会话请求；
                                                        1：会话请求。 */
        unsigned int  vbvalidoven  : 1;  /* bit[2]    : 忽略PHY给出的vbusvalid信号使能。
                                                        0：不使能；
                                                        1：使能。 */
        unsigned int  vbvalidovval : 1;  /* bit[3]    : 当VbvalidOvEn＝1时，该bit用于设置vbusvalid值而忽略PHY给出的vbusvalid信号。
                                                        0：vbusvalid无效；
                                                        1：vbusvalid有效。 */
        unsigned int  avalidoven   : 1;  /* bit[4]    : 忽略PHY给出的avalid信号使能。
                                                        0：不使能；
                                                        1：使能。 */
        unsigned int  avalidovval  : 1;  /* bit[5]    : 当AvalidOvEn＝1时，该bit用于设置avalid值而忽略PHY给出的avalid信号。
                                                        0：avalid无效；
                                                        1：avalid有效。 */
        unsigned int  bvalidoven   : 1;  /* bit[6]    : 忽略PHY给出的bvalid信号使能。
                                                        0：不使能；
                                                        1：使能。 */
        unsigned int  bvalidovval  : 1;  /* bit[7]    : 当BvalidOvEn＝1时，该bit用于设置bvalid值而忽略PHY给出的bvalid信号。
                                                        0：bvalid无效；
                                                        1：bvalid有效。 */
        unsigned int  hstnegscs    : 1;  /* bit[8]    : 主机协商成功，对device模式有效。
                                                        0：失败；
                                                        1：成功。 */
        unsigned int  hnpreq       : 1;  /* bit[9]    : HNP请求，对device模式有效。软件配置该bit，向host发起HNP操作。在接收到GOTGINT.HstNegSucStsChng中断之后，软件清除该bit。
                                                        0：无请求；
                                                        1：有请求。 */
        unsigned int  hstsethnpen  : 1;  /* bit[10]   : HOST HNP使能，对Host模式有效。Host在发出SetFeature.SetHNPEnabled命令之后，软件配置该bit为1。
                                                        0：不使能；
                                                        1：使能。 */
        unsigned int  devhnpen     : 1;  /* bit[11]   : device HNP使能，对device模式有效。接收到SetFeature.SetHNPEnabled命令之后软件配置该bit为1。
                                                        0：不使能；
                                                        1：使能。 */
        unsigned int  reserved_0   : 4;  /* bit[12-15]: 保留。 */
        unsigned int  conidsts     : 1;  /* bit[16]   : 插座ID状态。
                                                        0：A设备模式；
                                                        1：B设备模式。 */
        unsigned int  dbnc_time    : 1;  /* bit[17]   : debounce时间选择。Debounce时间指协议上规定的检测到device连接状态之后到发送复位信号之间的延迟。
                                                        0：100ms+2.5us，用于物理连接；
                                                        1：2.5us，用于软连接。 */
        unsigned int  asessvld     : 1;  /* bit[18]   : Host模式收发器状态。
                                                        0：a_session无效；
                                                        1：a_session有效。 */
        unsigned int  bsesvld      : 1;  /* bit[19]   : 设备连接状态指示。
                                                        0：未连接；
                                                        1：连接。 */
        unsigned int  otg_version  : 1;  /* bit[20]   : OTG协议版本。
                                                        0：1.3。支持数据线和VBUS脉冲实现SRP协议；
                                                        1：2.0。只支持数据线脉冲实现SRP协议。 */
        unsigned int  reserved_1   : 1;  /* bit[21]   : 保留。 */
        unsigned int  multvaldidbc : 5;  /* bit[22-26]: ACA ID管脚状态。
                                                        00001：rid_c；
                                                        00010：rid_b；
                                                        00100：rid_a；
                                                        01000：rid_gnd；
                                                        10000：rid_float；
                                                        others：reserved。 */
        unsigned int  chirpen      : 1;  /* bit[27]   : chirpon使能。
                                                        该bit配置为1，控制器在发送chirp K之前置chirp_on信号为高电平。
                                                        0：不使能；
                                                        1：使能。 */
        unsigned int  reserved_2   : 4;  /* bit[28-31]: 保留 */
    } reg;
} SOC_USBOTG_GOTGCTL_UNION;
#endif
#define SOC_USBOTG_GOTGCTL_sesreqscs_START     (0)
#define SOC_USBOTG_GOTGCTL_sesreqscs_END       (0)
#define SOC_USBOTG_GOTGCTL_sesreq_START        (1)
#define SOC_USBOTG_GOTGCTL_sesreq_END          (1)
#define SOC_USBOTG_GOTGCTL_vbvalidoven_START   (2)
#define SOC_USBOTG_GOTGCTL_vbvalidoven_END     (2)
#define SOC_USBOTG_GOTGCTL_vbvalidovval_START  (3)
#define SOC_USBOTG_GOTGCTL_vbvalidovval_END    (3)
#define SOC_USBOTG_GOTGCTL_avalidoven_START    (4)
#define SOC_USBOTG_GOTGCTL_avalidoven_END      (4)
#define SOC_USBOTG_GOTGCTL_avalidovval_START   (5)
#define SOC_USBOTG_GOTGCTL_avalidovval_END     (5)
#define SOC_USBOTG_GOTGCTL_bvalidoven_START    (6)
#define SOC_USBOTG_GOTGCTL_bvalidoven_END      (6)
#define SOC_USBOTG_GOTGCTL_bvalidovval_START   (7)
#define SOC_USBOTG_GOTGCTL_bvalidovval_END     (7)
#define SOC_USBOTG_GOTGCTL_hstnegscs_START     (8)
#define SOC_USBOTG_GOTGCTL_hstnegscs_END       (8)
#define SOC_USBOTG_GOTGCTL_hnpreq_START        (9)
#define SOC_USBOTG_GOTGCTL_hnpreq_END          (9)
#define SOC_USBOTG_GOTGCTL_hstsethnpen_START   (10)
#define SOC_USBOTG_GOTGCTL_hstsethnpen_END     (10)
#define SOC_USBOTG_GOTGCTL_devhnpen_START      (11)
#define SOC_USBOTG_GOTGCTL_devhnpen_END        (11)
#define SOC_USBOTG_GOTGCTL_conidsts_START      (16)
#define SOC_USBOTG_GOTGCTL_conidsts_END        (16)
#define SOC_USBOTG_GOTGCTL_dbnc_time_START     (17)
#define SOC_USBOTG_GOTGCTL_dbnc_time_END       (17)
#define SOC_USBOTG_GOTGCTL_asessvld_START      (18)
#define SOC_USBOTG_GOTGCTL_asessvld_END        (18)
#define SOC_USBOTG_GOTGCTL_bsesvld_START       (19)
#define SOC_USBOTG_GOTGCTL_bsesvld_END         (19)
#define SOC_USBOTG_GOTGCTL_otg_version_START   (20)
#define SOC_USBOTG_GOTGCTL_otg_version_END     (20)
#define SOC_USBOTG_GOTGCTL_multvaldidbc_START  (22)
#define SOC_USBOTG_GOTGCTL_multvaldidbc_END    (26)
#define SOC_USBOTG_GOTGCTL_chirpen_START       (27)
#define SOC_USBOTG_GOTGCTL_chirpen_END         (27)


/*****************************************************************************
 结构名    : SOC_USBOTG_GOTGINT_UNION
 结构说明  : GOTGINT 寄存器结构定义。地址偏移量:0x0004，初值:0x00000000，宽度:32
 寄存器说明: 全局OTG中断状态寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  reserved_0       : 2;  /* bit[0-1]  : 保留。 */
        unsigned int  sesenddet        : 1;  /* bit[2]    : 检测到会话请求结束，即指示utmisrp_bvalid信号变为低电平。
                                                            0：无中断；
                                                            1：有中断。 */
        unsigned int  reserved_1       : 5;  /* bit[3-7]  : 保留。 */
        unsigned int  sesreqsucstschng : 1;  /* bit[8]    : 会话请求状态变化。当会话请求成功或者失败时，该bit置1。软件必须通过查询寄存器GPTGCTL[SesReqScs]来确定会话请求是成功还是失败。
                                                            0：无中断；
                                                            1：有中断。 */
        unsigned int  hstnegsucstschng : 1;  /* bit[9]    : 主机协商成功状态变化。
                                                            0：无中断；
                                                            1：有中断。 */
        unsigned int  reserved_2       : 7;  /* bit[10-16]: 保留。 */
        unsigned int  hstnegdet        : 1;  /* bit[17]   : 检测到主机协商请求。
                                                            0：无中断；
                                                            1：有中断。 */
        unsigned int  adevtoutchg      : 1;  /* bit[18]   : A设备等待B设备连接超时。
                                                            0：无中断；
                                                            1：有中断。 */
        unsigned int  dbncedone        : 1;  /* bit[19]   : debounce完成，对host模式有效。检测到device连接之后，延迟debounce时间，控制器置该bit有效。软件收到该中断之后，可以配置发出复位操作。该bit仅当GUSBCFG[HNPCap]或者GUSBCFG[SRPCap]配置之后有效。
                                                            0：无中断；
                                                            1：有中断。 */
        unsigned int  multvalidchng    : 1;  /* bit[20]   : 至少一个ACA管脚状态发生变化。
                                                            0：无中断；
                                                            1：有中断。 */
        unsigned int  reserved_3       : 11; /* bit[21-31]: 保留 */
    } reg;
} SOC_USBOTG_GOTGINT_UNION;
#endif
#define SOC_USBOTG_GOTGINT_sesenddet_START         (2)
#define SOC_USBOTG_GOTGINT_sesenddet_END           (2)
#define SOC_USBOTG_GOTGINT_sesreqsucstschng_START  (8)
#define SOC_USBOTG_GOTGINT_sesreqsucstschng_END    (8)
#define SOC_USBOTG_GOTGINT_hstnegsucstschng_START  (9)
#define SOC_USBOTG_GOTGINT_hstnegsucstschng_END    (9)
#define SOC_USBOTG_GOTGINT_hstnegdet_START         (17)
#define SOC_USBOTG_GOTGINT_hstnegdet_END           (17)
#define SOC_USBOTG_GOTGINT_adevtoutchg_START       (18)
#define SOC_USBOTG_GOTGINT_adevtoutchg_END         (18)
#define SOC_USBOTG_GOTGINT_dbncedone_START         (19)
#define SOC_USBOTG_GOTGINT_dbncedone_END           (19)
#define SOC_USBOTG_GOTGINT_multvalidchng_START     (20)
#define SOC_USBOTG_GOTGINT_multvalidchng_END       (20)


/*****************************************************************************
 结构名    : SOC_USBOTG_GAHBCFG_UNION
 结构说明  : GAHBCFG 寄存器结构定义。地址偏移量:0x0008，初值:0x00000000，宽度:32
 寄存器说明: 全局AHB配置寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  glblintrmsk       : 1;  /* bit[0]    : 全局中断屏蔽位。用于屏蔽所有中断的上报。
                                                             0：屏蔽；
                                                             1：不屏蔽。 */
        unsigned int  hbstlen           : 4;  /* bit[1-4]  : burst类型控制，配置DMA AHB总线传输burst类型。
                                                             0000：single；
                                                             0001：incr；
                                                             0011：incr4；
                                                             0101：incr8；
                                                             0111：incr16；
                                                             others：reserved。 */
        unsigned int  dmaen             : 1;  /* bit[5]    : DMA使能。
                                                             0：不使能；
                                                             1：使能。 */
        unsigned int  reserved_0        : 1;  /* bit[6]    : 保留。 */
        unsigned int  nptxfemplvl       : 1;  /* bit[7]    : 发送FIFO空水标，配置发送FIFO空中断GINTSTS[NPTxFEmp]的触发条件。
                                                             0：发送FIFO半空；
                                                             1：发送FIFO全空。 */
        unsigned int  reserved_1        : 13; /* bit[8-20] : 保留 */
        unsigned int  remmemsupp        : 1;  /* bit[21]   : 远端内存支持 */
        unsigned int  notialldmawrit    : 1;  /* bit[22]   : 只是DMA传输状态 */
        unsigned int  ahbsingle         : 1;  /* bit[23]   : AHB单笔传输支持 */
        unsigned int  invdescendianness : 1;  /* bit[24]   : 改变描述符的大端小端属性 */
        unsigned int  reserved_2        : 7;  /* bit[25-31]: 保留 */
    } reg;
} SOC_USBOTG_GAHBCFG_UNION;
#endif
#define SOC_USBOTG_GAHBCFG_glblintrmsk_START        (0)
#define SOC_USBOTG_GAHBCFG_glblintrmsk_END          (0)
#define SOC_USBOTG_GAHBCFG_hbstlen_START            (1)
#define SOC_USBOTG_GAHBCFG_hbstlen_END              (4)
#define SOC_USBOTG_GAHBCFG_dmaen_START              (5)
#define SOC_USBOTG_GAHBCFG_dmaen_END                (5)
#define SOC_USBOTG_GAHBCFG_nptxfemplvl_START        (7)
#define SOC_USBOTG_GAHBCFG_nptxfemplvl_END          (7)
#define SOC_USBOTG_GAHBCFG_remmemsupp_START         (21)
#define SOC_USBOTG_GAHBCFG_remmemsupp_END           (21)
#define SOC_USBOTG_GAHBCFG_notialldmawrit_START     (22)
#define SOC_USBOTG_GAHBCFG_notialldmawrit_END       (22)
#define SOC_USBOTG_GAHBCFG_ahbsingle_START          (23)
#define SOC_USBOTG_GAHBCFG_ahbsingle_END            (23)
#define SOC_USBOTG_GAHBCFG_invdescendianness_START  (24)
#define SOC_USBOTG_GAHBCFG_invdescendianness_END    (24)


/*****************************************************************************
 结构名    : SOC_USBOTG_GUSBCFG_UNION
 结构说明  : GUSBCFG 寄存器结构定义。地址偏移量:0x000C，初值:0x00001400，宽度:32
 寄存器说明: 全局USB配置寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  toutcal      : 3;  /* bit[0-2]  : 高速和全速超时时间调整。单位为PHY时钟周期，用于调整标准超时时间。 */
        unsigned int  phyif        : 1;  /* bit[3]    : PHY数据位宽选择。
                                                        0：8bit；
                                                        1：16bit。 */
        unsigned int  reserved_0   : 4;  /* bit[4-7]  : 保留。 */
        unsigned int  srpcap       : 1;  /* bit[8]    : SRP功能使能。
                                                        0：不使能；
                                                        1：使能。 */
        unsigned int  hnpcap       : 1;  /* bit[9]    : HNP功能使能。
                                                        0：不使能；
                                                        1：使能。 */
        unsigned int  usbtrdtim    : 4;  /* bit[10-13]: USB回转时间。指示从MAC发出请求到从FIFO中取出数据的时间，单位为PHY时钟周期。
                                                        必须配置成如下值：
                                                        0101：PHY数据位宽是16bit；
                                                        1001：PHY数据位宽是8bit。 */
        unsigned int  reserved_1   : 1;  /* bit[14]   : 保留。 */
        unsigned int  phypwrclksel : 1;  /* bit[15]   : PHY低功耗模式时钟选择。
                                                        0：480MHz内部时钟；
                                                        1：48MHz外部时钟。 */
        unsigned int  reserved_2   : 12; /* bit[16-27]: 保留。 */
        unsigned int  txenddelay   : 1;  /* bit[28]   : 发送结束延迟。配置该bit，按照UTMI 1.05版本使能发送结束timer。
                                                        0：不使能；
                                                        1：使能。 */
        unsigned int  forcehstmode : 1;  /* bit[29]   : 强制为Host模式。
                                                        0：正常模式；
                                                        1：host模式。 */
        unsigned int  forcedevmode : 1;  /* bit[30]   : 强制为device模式。
                                                        0：正常模式；
                                                        1：device模式。 */
        unsigned int  ctpttxpkt    : 1;  /* bit[31]   : 破坏包发送，用于debug，正常使用时不能配置该bit为1。
                                                        0：不发送破坏包；
                                                        1：发送破坏包。 */
    } reg;
} SOC_USBOTG_GUSBCFG_UNION;
#endif
#define SOC_USBOTG_GUSBCFG_toutcal_START       (0)
#define SOC_USBOTG_GUSBCFG_toutcal_END         (2)
#define SOC_USBOTG_GUSBCFG_phyif_START         (3)
#define SOC_USBOTG_GUSBCFG_phyif_END           (3)
#define SOC_USBOTG_GUSBCFG_srpcap_START        (8)
#define SOC_USBOTG_GUSBCFG_srpcap_END          (8)
#define SOC_USBOTG_GUSBCFG_hnpcap_START        (9)
#define SOC_USBOTG_GUSBCFG_hnpcap_END          (9)
#define SOC_USBOTG_GUSBCFG_usbtrdtim_START     (10)
#define SOC_USBOTG_GUSBCFG_usbtrdtim_END       (13)
#define SOC_USBOTG_GUSBCFG_phypwrclksel_START  (15)
#define SOC_USBOTG_GUSBCFG_phypwrclksel_END    (15)
#define SOC_USBOTG_GUSBCFG_txenddelay_START    (28)
#define SOC_USBOTG_GUSBCFG_txenddelay_END      (28)
#define SOC_USBOTG_GUSBCFG_forcehstmode_START  (29)
#define SOC_USBOTG_GUSBCFG_forcehstmode_END    (29)
#define SOC_USBOTG_GUSBCFG_forcedevmode_START  (30)
#define SOC_USBOTG_GUSBCFG_forcedevmode_END    (30)
#define SOC_USBOTG_GUSBCFG_ctpttxpkt_START     (31)
#define SOC_USBOTG_GUSBCFG_ctpttxpkt_END       (31)


/*****************************************************************************
 结构名    : SOC_USBOTG_GRSTCTL_UNION
 结构说明  : GRSTCTL 寄存器结构定义。地址偏移量:0x0010，初值:0x80000000，宽度:32
 寄存器说明: 全局复位控制寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  csftrst    : 1;  /* bit[0]    : 软复位请求。复位总线时钟域和phy时钟域。操作完成该bit自动清零。
                                                      0：软复位完成；
                                                      1：进行软复位。 */
        unsigned int  reserved_0 : 1;  /* bit[1]    : 保留。 */
        unsigned int  frmcntrrst : 1;  /* bit[2]    : 微帧号计数复位。
                                                      0：复位无效；
                                                      1：复位有效。 */
        unsigned int  reserved_1 : 1;  /* bit[3]    : 保留。 */
        unsigned int  rxfflsh    : 1;  /* bit[4]    : 清空接收FIFO。操作完成，该bit自动清零。配置该bit之前，必须保证对接收FIFO没有读写操作。
                                                      0：清空接收FIFO完成；
                                                      1：清空接收FIFO。 */
        unsigned int  txfflsh    : 1;  /* bit[5]    : 清空发送FIFO。用来清空TxFNum指定的发送FIFO。清空发送FIFO完成，该bit自动清零。
                                                      0：清空发送FIFO完成；
                                                      1：清空发送FIFO。
                                                      在配置该位之前，必须保证对TxFIFO没有读写操作正在进行。可以通过如下寄存器状态确认：
                                                      DIEPINTn[INEPNakEff]中断为1保证控制器没有读发送FIFO。
                                                      GRST[AHBIdle]为1保证控制器没有写发送FIFO。
                                                      建议在FIFO重新配置或者端点不使能时清空发送FIFO。 */
        unsigned int  txfnum     : 5;  /* bit[6-10] : 清空发送FIFO序号。指示用TxFFlsh命令清除的发送FIFO，在TxFFlsh位清零之前不能改变TxFNum值。
                                                      0x0：Host模式下清空非周期发送FIFO，Device模式下清空发送FIFO 0；
                                                      0x1：Host模式下清空周期发送FIFO，Device模式下清空发送FIFO 1；
                                                      0x2：Device模式下，清空发送FIFO 2；
                                                      ……
                                                      0xF：Device模式下，清空发送FIFO 15；
                                                      0x10：清空所有发送FIFO；
                                                      others：reserved。 */
        unsigned int  reserved_2 : 19; /* bit[11-29]: 保留。 */
        unsigned int  dmareq     : 1;  /* bit[30]   : DMA请求状态。指示是否正在处理DMA请求，用于debug。
                                                      0：无DMA请求；
                                                      1：DMA请求正在处理。 */
        unsigned int  ahbidle    : 1;  /* bit[31]   : AHB总线状态。指示AHB master状态机处于空闲状态。
                                                      0：不处于空闲状态；
                                                      1：处于空闲状态。 */
    } reg;
} SOC_USBOTG_GRSTCTL_UNION;
#endif
#define SOC_USBOTG_GRSTCTL_csftrst_START     (0)
#define SOC_USBOTG_GRSTCTL_csftrst_END       (0)
#define SOC_USBOTG_GRSTCTL_frmcntrrst_START  (2)
#define SOC_USBOTG_GRSTCTL_frmcntrrst_END    (2)
#define SOC_USBOTG_GRSTCTL_rxfflsh_START     (4)
#define SOC_USBOTG_GRSTCTL_rxfflsh_END       (4)
#define SOC_USBOTG_GRSTCTL_txfflsh_START     (5)
#define SOC_USBOTG_GRSTCTL_txfflsh_END       (5)
#define SOC_USBOTG_GRSTCTL_txfnum_START      (6)
#define SOC_USBOTG_GRSTCTL_txfnum_END        (10)
#define SOC_USBOTG_GRSTCTL_dmareq_START      (30)
#define SOC_USBOTG_GRSTCTL_dmareq_END        (30)
#define SOC_USBOTG_GRSTCTL_ahbidle_START     (31)
#define SOC_USBOTG_GRSTCTL_ahbidle_END       (31)


/*****************************************************************************
 结构名    : SOC_USBOTG_GINTSTS_UNION
 结构说明  : GINTSTS 寄存器结构定义。地址偏移量:0x0014，初值:0x14000020，宽度:32
 寄存器说明: 全局中断状态寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  curmode      : 1;  /* bit[0]  : 当前模式。
                                                      0：device模式；
                                                      1：host模式。 */
        unsigned int  modemis      : 1;  /* bit[1]  : 模式不匹配中断。如果当前为host模式，软件读写device寄存器；如果当前为device模式，软件读写host寄存器会触发该中断。
                                                      0：无中断；
                                                      1：有中断。 */
        unsigned int  otgint       : 1;  /* bit[2]  : OTG中断。软件需要查询寄存器GOTGINT确定中断类型。
                                                      0：无中断；
                                                      1：有中断。 */
        unsigned int  sof          : 1;  /* bit[3]  : SOF(Start Of microFrame)中断。指示接收到SOF令牌包。
                                                      0：无中断；
                                                      1：有中断。 */
        unsigned int  rxflvl       : 1;  /* bit[4]  : 接收FIFO非空中断。指示接收FIFO中至少1个包的数据。
                                                      0：无中断；
                                                      1：有中断。 */
        unsigned int  nptxfemp     : 1;  /* bit[5]  : 非周期发送FIFO空中断。
                                                      0：无中断；
                                                      1：有中断。 */
        unsigned int  ginnakeff    : 1;  /* bit[6]  : 非周期IN端点NAK有效中断。指示寄存器DCTL[SGNPInNak]设置有效，对所有的IN令牌响应NAK。该bit可以通过写DCTL[CGNPInNak]来清除。
                                                      0：无中断；
                                                      1：有中断。 */
        unsigned int  goutnakeff   : 1;  /* bit[7]  : OUT端点NAK有效中断。指示寄存器DCTL[SGOUTNak]设置有效，对所有的OUT令牌包响应NAK。该bit可以通过写寄存器DCTL[CGOUTNak]来清除。
                                                      0：无中断；
                                                      1：有中断。 */
        unsigned int  reserved_0   : 2;  /* bit[8-9]: 保留。 */
        unsigned int  erlysusp     : 1;  /* bit[10] : Early suspend中断。当检测到USB处于IDLE状态超过3ms会触发该中断。
                                                      0：无中断；
                                                      1：有中断。 */
        unsigned int  usbsusp      : 1;  /* bit[11] : suspend中断。指示进入suspend状态。
                                                      0：无中断；
                                                      1：有中断。 */
        unsigned int  usbrst       : 1;  /* bit[12] : USB复位中断。指示检测到USB复位操作。
                                                      0：无中断；
                                                      1：有中断。 */
        unsigned int  enumdone     : 1;  /* bit[13] : 枚举完成中断。
                                                      0：无中断；
                                                      1：有中断。 */
        unsigned int  isooutdrop   : 1;  /* bit[14] : Isochronous OUT包丢弃中断。当收到Isochronous OUT包，但是由于接收FIFO空间不够，USB会丢弃收到的包，并置位该中断。
                                                      0：无中断；
                                                      1：有中断。 */
        unsigned int  eopf         : 1;  /* bit[15] : EOPF（End of Period Frame）中断，指示在当前帧周期传输结束。
                                                      0：无中断；
                                                      1：有中断。 */
        unsigned int  rstrdoneint  : 1;  /* bit[16] : 寄存器恢复中断。指示退出冬眠模式后，寄存器恢复操作完成。
                                                      0：无中断；
                                                      1：有中断。 */
        unsigned int  reserved_1   : 1;  /* bit[17] : 保留。 */
        unsigned int  iepint       : 1;  /* bit[18] : IN端点中断。指示IN端点有中断产生。软件需要查询寄存器DAINT确定端点号，然后查询相应端点的中断状态寄存器确定中断类型。
                                                      0：无中断；
                                                      1：有中断。 */
        unsigned int  oepint       : 1;  /* bit[19] : OUT端点中断。指示OUT端点有中断产生。软件需要查询寄存器DAINT确定端点号，然后查询相应端点的中断状态寄存器确定中断类型。
                                                      0：无中断；
                                                      1：有中断。 */
        unsigned int  incompisoin  : 1;  /* bit[20] : Isochronous IN传输未完成中断。指示至少有1个IN端点的Isochronous传输在当前的微帧未完成。该中断与EOPF中断同时产生。
                                                      0：无中断
                                                      1：有中断 */
        unsigned int  incompisoout : 1;  /* bit[21] : Isochronous OUT传输未完成中断。指示至少有1个OUT端点的Isochronous传输在当前的微帧未完成。该中断与EOPF中断同时产生。
                                                      0：无中断；
                                                      1：有中断。 */
        unsigned int  fetsusp      : 1;  /* bit[22] : 取数挂起中断。仅在DMA模式时有效。该中断指示由于发送FIFO没有空间的原因，DMA停止给IN端点从memory读取数据。
                                                      0：无中断；
                                                      1：有中断。 */
        unsigned int  resetdet     : 1;  /* bit[23] : 检测到复位中断。当设备处于suspend状态，部分掉电时，检测到USB总线上有复位操作，会触发该中断。
                                                      0：无中断；
                                                      1：有中断。 */
        unsigned int  hprtint      : 1;  /* bit[24] : Host端口中断。软件需要通过HPRT寄存器确定具体中断。
                                                      0：无中断；
                                                      1：有中断。 */
        unsigned int  hchint       : 1;  /* bit[25] : Host通道中断。指示任何一个通道有中断触发，软件需要通过HCINTn寄存器确定具体中断。
                                                      0：无中断；
                                                      1：有中断。 */
        unsigned int  ptxfemp      : 1;  /* bit[26] : 周期发送FIFO空中断，对host模式有效。
                                                      0：无中断；
                                                      1：有中断。 */
        unsigned int  lpm_int      : 1;  /* bit[27] : LPM接收中断。当USB接收到LPM包，且向host响应非ERROR，该中断会触发。该位仅当LPM功能使能时有效。
                                                      0：无中断；
                                                      1：有中断。 */
        unsigned int  conidstschg  : 1;  /* bit[28] : ID状态变化中断。
                                                      0：无中断；
                                                      1：有中断。 */
        unsigned int  disconnint   : 1;  /* bit[29] : 断开连接中断，对host模式有效。
                                                      0：无中断；
                                                      1：有中断。 */
        unsigned int  sessreqint   : 1;  /* bit[30] : 检测到会话请求中断。当utmisrp_bvalid从低电平变成高电平，会产生该中断。
                                                      0：无中断；
                                                      1：有中断。 */
        unsigned int  wkupint      : 1;  /* bit[31] : Resume/Remote Wakwup中断。检测到host发出的resume操作，或者从L1状态唤醒会触发该中断。
                                                      0：无中断；
                                                      1：有中断。 */
    } reg;
} SOC_USBOTG_GINTSTS_UNION;
#endif
#define SOC_USBOTG_GINTSTS_curmode_START       (0)
#define SOC_USBOTG_GINTSTS_curmode_END         (0)
#define SOC_USBOTG_GINTSTS_modemis_START       (1)
#define SOC_USBOTG_GINTSTS_modemis_END         (1)
#define SOC_USBOTG_GINTSTS_otgint_START        (2)
#define SOC_USBOTG_GINTSTS_otgint_END          (2)
#define SOC_USBOTG_GINTSTS_sof_START           (3)
#define SOC_USBOTG_GINTSTS_sof_END             (3)
#define SOC_USBOTG_GINTSTS_rxflvl_START        (4)
#define SOC_USBOTG_GINTSTS_rxflvl_END          (4)
#define SOC_USBOTG_GINTSTS_nptxfemp_START      (5)
#define SOC_USBOTG_GINTSTS_nptxfemp_END        (5)
#define SOC_USBOTG_GINTSTS_ginnakeff_START     (6)
#define SOC_USBOTG_GINTSTS_ginnakeff_END       (6)
#define SOC_USBOTG_GINTSTS_goutnakeff_START    (7)
#define SOC_USBOTG_GINTSTS_goutnakeff_END      (7)
#define SOC_USBOTG_GINTSTS_erlysusp_START      (10)
#define SOC_USBOTG_GINTSTS_erlysusp_END        (10)
#define SOC_USBOTG_GINTSTS_usbsusp_START       (11)
#define SOC_USBOTG_GINTSTS_usbsusp_END         (11)
#define SOC_USBOTG_GINTSTS_usbrst_START        (12)
#define SOC_USBOTG_GINTSTS_usbrst_END          (12)
#define SOC_USBOTG_GINTSTS_enumdone_START      (13)
#define SOC_USBOTG_GINTSTS_enumdone_END        (13)
#define SOC_USBOTG_GINTSTS_isooutdrop_START    (14)
#define SOC_USBOTG_GINTSTS_isooutdrop_END      (14)
#define SOC_USBOTG_GINTSTS_eopf_START          (15)
#define SOC_USBOTG_GINTSTS_eopf_END            (15)
#define SOC_USBOTG_GINTSTS_rstrdoneint_START   (16)
#define SOC_USBOTG_GINTSTS_rstrdoneint_END     (16)
#define SOC_USBOTG_GINTSTS_iepint_START        (18)
#define SOC_USBOTG_GINTSTS_iepint_END          (18)
#define SOC_USBOTG_GINTSTS_oepint_START        (19)
#define SOC_USBOTG_GINTSTS_oepint_END          (19)
#define SOC_USBOTG_GINTSTS_incompisoin_START   (20)
#define SOC_USBOTG_GINTSTS_incompisoin_END     (20)
#define SOC_USBOTG_GINTSTS_incompisoout_START  (21)
#define SOC_USBOTG_GINTSTS_incompisoout_END    (21)
#define SOC_USBOTG_GINTSTS_fetsusp_START       (22)
#define SOC_USBOTG_GINTSTS_fetsusp_END         (22)
#define SOC_USBOTG_GINTSTS_resetdet_START      (23)
#define SOC_USBOTG_GINTSTS_resetdet_END        (23)
#define SOC_USBOTG_GINTSTS_hprtint_START       (24)
#define SOC_USBOTG_GINTSTS_hprtint_END         (24)
#define SOC_USBOTG_GINTSTS_hchint_START        (25)
#define SOC_USBOTG_GINTSTS_hchint_END          (25)
#define SOC_USBOTG_GINTSTS_ptxfemp_START       (26)
#define SOC_USBOTG_GINTSTS_ptxfemp_END         (26)
#define SOC_USBOTG_GINTSTS_lpm_int_START       (27)
#define SOC_USBOTG_GINTSTS_lpm_int_END         (27)
#define SOC_USBOTG_GINTSTS_conidstschg_START   (28)
#define SOC_USBOTG_GINTSTS_conidstschg_END     (28)
#define SOC_USBOTG_GINTSTS_disconnint_START    (29)
#define SOC_USBOTG_GINTSTS_disconnint_END      (29)
#define SOC_USBOTG_GINTSTS_sessreqint_START    (30)
#define SOC_USBOTG_GINTSTS_sessreqint_END      (30)
#define SOC_USBOTG_GINTSTS_wkupint_START       (31)
#define SOC_USBOTG_GINTSTS_wkupint_END         (31)


/*****************************************************************************
 结构名    : SOC_USBOTG_GINTMSK_UNION
 结构说明  : GINTMSK 寄存器结构定义。地址偏移量:0x0018，初值:0x00000000，宽度:32
 寄存器说明: 全局中断屏蔽寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  reserved_0      : 2;  /* bit[0-1]: 保留。 */
        unsigned int  otgintmsk       : 1;  /* bit[2]  : OTG中断屏蔽位。
                                                         0：屏蔽；
                                                         1：不屏蔽。 */
        unsigned int  sofmsk          : 1;  /* bit[3]  : sof(Start Of microFrame)中断屏蔽位。
                                                         0：屏蔽；
                                                         1：不屏蔽。 */
        unsigned int  rxflvlmsk       : 1;  /* bit[4]  : 接收FIFO非空中断屏蔽位。
                                                         0：屏蔽；
                                                         1：不屏蔽。 */
        unsigned int  nptxfempmsk     : 1;  /* bit[5]  : 非周期发送FIFO空中断屏蔽位。
                                                         0：屏蔽；
                                                         1：不屏蔽。 */
        unsigned int  ginnakeffmsk    : 1;  /* bit[6]  : IN端点NAK有效中断屏蔽位。
                                                         0：屏蔽；
                                                         1：不屏蔽。 */
        unsigned int  goutnakeffmsk   : 1;  /* bit[7]  : OUT端点NAK有效中断屏蔽位。
                                                         0：屏蔽；
                                                         1：不屏蔽。 */
        unsigned int  reserved_1      : 2;  /* bit[8-9]: 保留 */
        unsigned int  erlysuspmsk     : 1;  /* bit[10] : Early suspend中断屏蔽位。
                                                         0：屏蔽；
                                                         1：不屏蔽。 */
        unsigned int  usbsuspmsk      : 1;  /* bit[11] : suspend中断屏蔽位。
                                                         0：屏蔽；
                                                         1：不屏蔽。 */
        unsigned int  usbrstmsk       : 1;  /* bit[12] : USB复位中断屏蔽位。
                                                         0：屏蔽；
                                                         1：不屏蔽。 */
        unsigned int  enumdonemsk     : 1;  /* bit[13] : 枚举完成中断屏蔽位。
                                                         0：屏蔽；
                                                         1：不屏蔽。 */
        unsigned int  isooutdropmsk   : 1;  /* bit[14] : Isochronous OUT包丢弃中断屏蔽位。
                                                         0：屏蔽；
                                                         1：不屏蔽。 */
        unsigned int  eopfmsk         : 1;  /* bit[15] : EOPF（End of Period Frame）中断屏蔽位。
                                                         0：屏蔽；
                                                         1：不屏蔽。 */
        unsigned int  rstrdonemsk     : 1;  /* bit[16] : 寄存器恢复中断屏蔽位。
                                                         0：屏蔽；
                                                         1：不屏蔽。 */
        unsigned int  reserved_2      : 1;  /* bit[17] : 保留。 */
        unsigned int  iepintmsk       : 1;  /* bit[18] : IN端点中断屏蔽位。
                                                         0：屏蔽；
                                                         1：不屏蔽。 */
        unsigned int  oepintmsk       : 1;  /* bit[19] : OUT端点中断屏蔽位。
                                                         0：屏蔽；
                                                         1：不屏蔽。 */
        unsigned int  incompisoinmsk  : 1;  /* bit[20] : Isochronous IN传输未完成中断屏蔽位。
                                                         0：屏蔽；
                                                         1：不屏蔽。 */
        unsigned int  incompisooutmsk : 1;  /* bit[21] : Isochronous OUT传输未完成中断屏蔽位。
                                                         0：屏蔽；
                                                         1：不屏蔽。 */
        unsigned int  fetsuspmsk      : 1;  /* bit[22] : 取数挂起中断屏蔽位。
                                                         0：屏蔽；
                                                         1：不屏蔽。 */
        unsigned int  resetdetmsk     : 1;  /* bit[23] : 检测到复位中断屏蔽位。
                                                         0：屏蔽；
                                                         1：不屏蔽。 */
        unsigned int  hprtintmsk      : 1;  /* bit[24] : Host端口中断屏蔽位。
                                                         0：屏蔽；
                                                         1：不屏蔽。 */
        unsigned int  hchintmsk       : 1;  /* bit[25] : Host通道中断屏蔽位。
                                                         0：屏蔽；
                                                         1：不屏蔽。 */
        unsigned int  ptxfempmsk      : 1;  /* bit[26] : 周期发送FIFO空中断屏蔽位。
                                                         0：屏蔽；
                                                         1：不屏蔽。 */
        unsigned int  lpm_intmsk      : 1;  /* bit[27] : LPM接收中断屏蔽位。
                                                         0：屏蔽；
                                                         1：不屏蔽。 */
        unsigned int  conidstschngmsk : 1;  /* bit[28] : ID状态变化中断屏蔽位。
                                                         0：屏蔽；
                                                         1：不屏蔽。 */
        unsigned int  disconnintmsk   : 1;  /* bit[29] : 断开连接中断屏蔽位。
                                                         0：屏蔽；
                                                         1：不屏蔽。 */
        unsigned int  sessreqintmsk   : 1;  /* bit[30] : 检测到会话请求中断屏蔽位。
                                                         0：屏蔽；
                                                         1：不屏蔽。 */
        unsigned int  wkupintmsk      : 1;  /* bit[31] : Resume/Remote Wakwup中断屏蔽位。
                                                         0：屏蔽；
                                                         1：不屏蔽。 */
    } reg;
} SOC_USBOTG_GINTMSK_UNION;
#endif
#define SOC_USBOTG_GINTMSK_otgintmsk_START        (2)
#define SOC_USBOTG_GINTMSK_otgintmsk_END          (2)
#define SOC_USBOTG_GINTMSK_sofmsk_START           (3)
#define SOC_USBOTG_GINTMSK_sofmsk_END             (3)
#define SOC_USBOTG_GINTMSK_rxflvlmsk_START        (4)
#define SOC_USBOTG_GINTMSK_rxflvlmsk_END          (4)
#define SOC_USBOTG_GINTMSK_nptxfempmsk_START      (5)
#define SOC_USBOTG_GINTMSK_nptxfempmsk_END        (5)
#define SOC_USBOTG_GINTMSK_ginnakeffmsk_START     (6)
#define SOC_USBOTG_GINTMSK_ginnakeffmsk_END       (6)
#define SOC_USBOTG_GINTMSK_goutnakeffmsk_START    (7)
#define SOC_USBOTG_GINTMSK_goutnakeffmsk_END      (7)
#define SOC_USBOTG_GINTMSK_erlysuspmsk_START      (10)
#define SOC_USBOTG_GINTMSK_erlysuspmsk_END        (10)
#define SOC_USBOTG_GINTMSK_usbsuspmsk_START       (11)
#define SOC_USBOTG_GINTMSK_usbsuspmsk_END         (11)
#define SOC_USBOTG_GINTMSK_usbrstmsk_START        (12)
#define SOC_USBOTG_GINTMSK_usbrstmsk_END          (12)
#define SOC_USBOTG_GINTMSK_enumdonemsk_START      (13)
#define SOC_USBOTG_GINTMSK_enumdonemsk_END        (13)
#define SOC_USBOTG_GINTMSK_isooutdropmsk_START    (14)
#define SOC_USBOTG_GINTMSK_isooutdropmsk_END      (14)
#define SOC_USBOTG_GINTMSK_eopfmsk_START          (15)
#define SOC_USBOTG_GINTMSK_eopfmsk_END            (15)
#define SOC_USBOTG_GINTMSK_rstrdonemsk_START      (16)
#define SOC_USBOTG_GINTMSK_rstrdonemsk_END        (16)
#define SOC_USBOTG_GINTMSK_iepintmsk_START        (18)
#define SOC_USBOTG_GINTMSK_iepintmsk_END          (18)
#define SOC_USBOTG_GINTMSK_oepintmsk_START        (19)
#define SOC_USBOTG_GINTMSK_oepintmsk_END          (19)
#define SOC_USBOTG_GINTMSK_incompisoinmsk_START   (20)
#define SOC_USBOTG_GINTMSK_incompisoinmsk_END     (20)
#define SOC_USBOTG_GINTMSK_incompisooutmsk_START  (21)
#define SOC_USBOTG_GINTMSK_incompisooutmsk_END    (21)
#define SOC_USBOTG_GINTMSK_fetsuspmsk_START       (22)
#define SOC_USBOTG_GINTMSK_fetsuspmsk_END         (22)
#define SOC_USBOTG_GINTMSK_resetdetmsk_START      (23)
#define SOC_USBOTG_GINTMSK_resetdetmsk_END        (23)
#define SOC_USBOTG_GINTMSK_hprtintmsk_START       (24)
#define SOC_USBOTG_GINTMSK_hprtintmsk_END         (24)
#define SOC_USBOTG_GINTMSK_hchintmsk_START        (25)
#define SOC_USBOTG_GINTMSK_hchintmsk_END          (25)
#define SOC_USBOTG_GINTMSK_ptxfempmsk_START       (26)
#define SOC_USBOTG_GINTMSK_ptxfempmsk_END         (26)
#define SOC_USBOTG_GINTMSK_lpm_intmsk_START       (27)
#define SOC_USBOTG_GINTMSK_lpm_intmsk_END         (27)
#define SOC_USBOTG_GINTMSK_conidstschngmsk_START  (28)
#define SOC_USBOTG_GINTMSK_conidstschngmsk_END    (28)
#define SOC_USBOTG_GINTMSK_disconnintmsk_START    (29)
#define SOC_USBOTG_GINTMSK_disconnintmsk_END      (29)
#define SOC_USBOTG_GINTMSK_sessreqintmsk_START    (30)
#define SOC_USBOTG_GINTMSK_sessreqintmsk_END      (30)
#define SOC_USBOTG_GINTMSK_wkupintmsk_START       (31)
#define SOC_USBOTG_GINTMSK_wkupintmsk_END         (31)


/*****************************************************************************
 结构名    : SOC_USBOTG_GRXSTSR_UNION
 结构说明  : GRXSTSR 寄存器结构定义。地址偏移量:0x001C，初值:0x00000000，宽度:32
 寄存器说明: 全局接收FIFO状态读出寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  epnum    : 4;  /* bit[0-3]  : Host模式下指示通道号，device模式下指示端点号。 */
        unsigned int  bcnt     : 11; /* bit[4-14] : 数据包大小，以字节为单位。 */
        unsigned int  dpid     : 2;  /* bit[15-16]: 数据包PID。
                                                    00：DATA0；
                                                    10：DATA1；
                                                    01：DATA2；
                                                    11：MDATA。 */
        unsigned int  pktsts   : 4;  /* bit[17-20]: 包状态。指示接收到的数据包状态。
                                                    Host模式下：
                                                    0010：接收到IN数据包；
                                                    0011：IN传输完成；
                                                    0101：数据包PID翻转错误；
                                                    0111：通道停止；
                                                    others：reserved。
                                                    Device模式下：
                                                    0001：OUT NAK；
                                                    0010：接收到OUT数据包；
                                                    0011：OUT传输完成；
                                                    0100：Setup事务完成；
                                                    0110：接收到Setup包；
                                                    others：reserved。 */
        unsigned int  fn       : 4;  /* bit[21-24]: Host模式下，该域保留。
                                                    Device模式下，帧序号。指示帧序号的低4位。读该寄存器返回接收FIFO数据状态，而不弹出接收FIFO数据。 */
        unsigned int  reserved : 7;  /* bit[25-31]: 保留。 */
    } reg;
} SOC_USBOTG_GRXSTSR_UNION;
#endif
#define SOC_USBOTG_GRXSTSR_epnum_START     (0)
#define SOC_USBOTG_GRXSTSR_epnum_END       (3)
#define SOC_USBOTG_GRXSTSR_bcnt_START      (4)
#define SOC_USBOTG_GRXSTSR_bcnt_END        (14)
#define SOC_USBOTG_GRXSTSR_dpid_START      (15)
#define SOC_USBOTG_GRXSTSR_dpid_END        (16)
#define SOC_USBOTG_GRXSTSR_pktsts_START    (17)
#define SOC_USBOTG_GRXSTSR_pktsts_END      (20)
#define SOC_USBOTG_GRXSTSR_fn_START        (21)
#define SOC_USBOTG_GRXSTSR_fn_END          (24)


/*****************************************************************************
 结构名    : SOC_USBOTG_GRXSTSP_UNION
 结构说明  : GRXSTSP 寄存器结构定义。地址偏移量:0x0020，初值:0x00000000，宽度:32
 寄存器说明: 全局接收FIFO状态弹出寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  epnum    : 4;  /* bit[0-3]  : Host模式下指示通道号，device模式下指示端点号。 */
        unsigned int  bcnt     : 11; /* bit[4-14] : 数据包大小，以字节为单位。 */
        unsigned int  dpid     : 2;  /* bit[15-16]: 数据包PID。
                                                    00：DATA0；
                                                    10：DATA1；
                                                    01：DATA2；
                                                    11：MDATA。 */
        unsigned int  pktsts   : 4;  /* bit[17-20]: 包状态。指示接收到的数据包状态。
                                                    Host模式下：
                                                    0010：接收到IN数据包；
                                                    0011：IN传输完成；
                                                    0101：数据包PID翻转错误；
                                                    0111：通道停止；
                                                    others：reserved。
                                                    Device模式下：
                                                    0001：OUT NAK；
                                                    0010：接收到OUT数据包；
                                                    0011：OUT传输完成；
                                                    0100：Setup事务完成；
                                                    0110：接收到Setup包；
                                                    others：reserved。 */
        unsigned int  fn       : 4;  /* bit[21-24]: Host模式下，该域保留。
                                                    Device模式下，帧序号。指示帧序号的低4位。读该寄存器返回接收FIFO数据状态，同时弹出接收FIFO数据。 */
        unsigned int  reserved : 7;  /* bit[25-31]: 保留。 */
    } reg;
} SOC_USBOTG_GRXSTSP_UNION;
#endif
#define SOC_USBOTG_GRXSTSP_epnum_START     (0)
#define SOC_USBOTG_GRXSTSP_epnum_END       (3)
#define SOC_USBOTG_GRXSTSP_bcnt_START      (4)
#define SOC_USBOTG_GRXSTSP_bcnt_END        (14)
#define SOC_USBOTG_GRXSTSP_dpid_START      (15)
#define SOC_USBOTG_GRXSTSP_dpid_END        (16)
#define SOC_USBOTG_GRXSTSP_pktsts_START    (17)
#define SOC_USBOTG_GRXSTSP_pktsts_END      (20)
#define SOC_USBOTG_GRXSTSP_fn_START        (21)
#define SOC_USBOTG_GRXSTSP_fn_END          (24)


/*****************************************************************************
 结构名    : SOC_USBOTG_GRXFSIZ_UNION
 结构说明  : GRXFSIZ 寄存器结构定义。地址偏移量:0x0024，初值:0x00001000，宽度:32
 寄存器说明: 全局接收FIFO大小寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  rxfdep   : 16; /* bit[0-15] : 接收FIFO深度。以4字节为单位。 */
        unsigned int  reserved : 16; /* bit[16-31]: 保留。 */
    } reg;
} SOC_USBOTG_GRXFSIZ_UNION;
#endif
#define SOC_USBOTG_GRXFSIZ_rxfdep_START    (0)
#define SOC_USBOTG_GRXFSIZ_rxfdep_END      (15)


/*****************************************************************************
 结构名    : SOC_USBOTG_GNPTXFSIZ_UNION
 结构说明  : GNPTXFSIZ 寄存器结构定义。地址偏移量:0x0028，初值:0x10001000，宽度:32
 寄存器说明: 全局非周期发送FIFO大小寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  nptxfstaddr : 16; /* bit[0-15] : 给周期发送FIFO起始地址。以4字节为单位。 */
        unsigned int  gnptxfsiz   : 16; /* bit[16-31]: 非周期发送FIFO深度。以4字节为单位。 */
    } reg;
} SOC_USBOTG_GNPTXFSIZ_UNION;
#endif
#define SOC_USBOTG_GNPTXFSIZ_nptxfstaddr_START  (0)
#define SOC_USBOTG_GNPTXFSIZ_nptxfstaddr_END    (15)
#define SOC_USBOTG_GNPTXFSIZ_gnptxfsiz_START    (16)
#define SOC_USBOTG_GNPTXFSIZ_gnptxfsiz_END      (31)


/*****************************************************************************
 结构名    : SOC_USBOTG_GNPTXSTS_UNION
 结构说明  : GNPTXSTS 寄存器结构定义。地址偏移量:0x002C，初值:0x00081000，宽度:32
 寄存器说明: 全局非周期发送FIFO状态寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  nptxfspcavail : 16; /* bit[0-15] : 非周期发送FIFO剩余空间。以4字节为单位。 */
        unsigned int  nptxqspcavail : 8;  /* bit[16-23]: 非周期发送请求队列剩余空间。
                                                         非周期发送请求队列深度为8，该域指示发送请求队列还可以缓存几个发送请求。 */
        unsigned int  nptxqtop      : 7;  /* bit[24-30]: 等待处理的非周期发送请求。
                                                         Bit[30:27]:通道号；
                                                         Bit[26:25]
                                                         00：OUT令牌；
                                                         01：零长包；
                                                         10：PING/CSPLIT令牌；
                                                         11：通道停止命令。
                                                         Bit[24]:结束标志，指示该请求为当前通道的最后一个请求。 */
        unsigned int  reserved      : 1;  /* bit[31]   : 保留。 */
    } reg;
} SOC_USBOTG_GNPTXSTS_UNION;
#endif
#define SOC_USBOTG_GNPTXSTS_nptxfspcavail_START  (0)
#define SOC_USBOTG_GNPTXSTS_nptxfspcavail_END    (15)
#define SOC_USBOTG_GNPTXSTS_nptxqspcavail_START  (16)
#define SOC_USBOTG_GNPTXSTS_nptxqspcavail_END    (23)
#define SOC_USBOTG_GNPTXSTS_nptxqtop_START       (24)
#define SOC_USBOTG_GNPTXSTS_nptxqtop_END         (30)


/*****************************************************************************
 结构名    : SOC_USBOTG_GSNPSID_UNION
 结构说明  : GSNPSID 寄存器结构定义。地址偏移量:0x0040，初值:0x4F54293A，宽度:32
 寄存器说明: USB IP版本寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  userid : 32; /* bit[0-31]: USB IP版本寄存器。版本为2.93a。 */
    } reg;
} SOC_USBOTG_GSNPSID_UNION;
#endif
#define SOC_USBOTG_GSNPSID_userid_START  (0)
#define SOC_USBOTG_GSNPSID_userid_END    (31)


/*****************************************************************************
 结构名    : SOC_USBOTG_GHWCFG1_UNION
 结构说明  : GHWCFG1 寄存器结构定义。地址偏移量:0x0044，初值:0x00000000，宽度:32
 寄存器说明: 硬件配置寄存器1。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  epdir : 32; /* bit[0-31]: 端点方向寄存器。
                                                Bit[31:30]：端点15方向；
                                                bit[29:28]：端点14方向；
                                                ……
                                                bit[1:0]：端点0方向。
                                                各个端点值：
                                                00：双向端点；
                                                01：IN端点；
                                                10：OUT端点；
                                                11：保留。 */
    } reg;
} SOC_USBOTG_GHWCFG1_UNION;
#endif
#define SOC_USBOTG_GHWCFG1_epdir_START  (0)
#define SOC_USBOTG_GHWCFG1_epdir_END    (31)


/*****************************************************************************
 结构名    : SOC_USBOTG_GHWCFG2_UNION
 结构说明  : GHWCFG2 寄存器结构定义。地址偏移量:0x0048，初值:0x22ACFC50，宽度:32
 寄存器说明: 硬件配置寄存器2。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  otgmode         : 3;  /* bit[0-2]  : OTG模式。
                                                           000：HNP和SRP使能OTG,可作host和device；
                                                           001：SRP使能OTG,可作host和device；
                                                           010：HNP和SRP不使能OTG,可作host和device；
                                                           011：SRP使能device；
                                                           100：非OTG device；
                                                           101：SRP使能host；
                                                           110：非OTG host；
                                                           111：保留。 */
        unsigned int  otgarch         : 2;  /* bit[3-4]  : DMA模式配置。
                                                           00：非DMA模式，只有1个AHB slave接口；
                                                           01：外部DMA；
                                                           10：内部DMA
                                                           11：保留。 */
        unsigned int  singpnt         : 1;  /* bit[5]    : Point-to-Potint模式使能。该配置仅针对host有效。
                                                           0：不使能；
                                                           1：使能。 */
        unsigned int  hsphytype       : 2;  /* bit[6-7]  : 高速PHY接口类型选择。
                                                           00：不支持高速模式；
                                                           01：UTMI+；
                                                           10：ULPI；
                                                           11：UTMI+和ULPI都支持，软件可配置。 */
        unsigned int  fsphytype       : 2;  /* bit[8-9]  : 全速PHY接口类型。
                                                           00：不支持全速PHY接口；
                                                           01：专有全速接口；
                                                           10：全速管脚与UTMI+管脚复用；
                                                           11：全速管脚与ULPI管脚复用。 */
        unsigned int  numdeveps       : 4;  /* bit[10-13]: 除端点0之外支持的端点数目，范围为1~15。 */
        unsigned int  numhstchnl      : 4;  /* bit[14-17]: Host通道数目。仅支持host有效。 */
        unsigned int  periosupport    : 1;  /* bit[18]   : Host模式时，是否支持周期OUT通道。
                                                           0：不支持；
                                                           1：支持。 */
        unsigned int  dynfifosizing   : 1;  /* bit[19]   : 是否支持FIFO大小动态配置。
                                                           0：不支持；
                                                           1：支持。 */
        unsigned int  multiprocintrpt : 1;  /* bit[20]   : 是否支持多中断上报。
                                                           0：不支持；
                                                           1：支持。 */
        unsigned int  reserved        : 1;  /* bit[21]   : 保留。 */
        unsigned int  nptxqdepth      : 2;  /* bit[22-23]: 非周期请求队列深度。
                                                           00：2；
                                                           01：4；
                                                           10：8；
                                                           others：reserved。 */
        unsigned int  ptxqdepth       : 2;  /* bit[24-25]: host模式时，周期请求队列深度。
                                                           00：2；
                                                           01：4；
                                                           10：8；
                                                           others：reserved。 */
        unsigned int  tknqdepth       : 5;  /* bit[26-30]: device模式时，IN令牌序列深度。 */
        unsigned int  otg_ic_usb      : 1;  /* bit[31]   : 是否支持IC_USB模式。
                                                           0：不支持；
                                                           1：支持。 */
    } reg;
} SOC_USBOTG_GHWCFG2_UNION;
#endif
#define SOC_USBOTG_GHWCFG2_otgmode_START          (0)
#define SOC_USBOTG_GHWCFG2_otgmode_END            (2)
#define SOC_USBOTG_GHWCFG2_otgarch_START          (3)
#define SOC_USBOTG_GHWCFG2_otgarch_END            (4)
#define SOC_USBOTG_GHWCFG2_singpnt_START          (5)
#define SOC_USBOTG_GHWCFG2_singpnt_END            (5)
#define SOC_USBOTG_GHWCFG2_hsphytype_START        (6)
#define SOC_USBOTG_GHWCFG2_hsphytype_END          (7)
#define SOC_USBOTG_GHWCFG2_fsphytype_START        (8)
#define SOC_USBOTG_GHWCFG2_fsphytype_END          (9)
#define SOC_USBOTG_GHWCFG2_numdeveps_START        (10)
#define SOC_USBOTG_GHWCFG2_numdeveps_END          (13)
#define SOC_USBOTG_GHWCFG2_numhstchnl_START       (14)
#define SOC_USBOTG_GHWCFG2_numhstchnl_END         (17)
#define SOC_USBOTG_GHWCFG2_periosupport_START     (18)
#define SOC_USBOTG_GHWCFG2_periosupport_END       (18)
#define SOC_USBOTG_GHWCFG2_dynfifosizing_START    (19)
#define SOC_USBOTG_GHWCFG2_dynfifosizing_END      (19)
#define SOC_USBOTG_GHWCFG2_multiprocintrpt_START  (20)
#define SOC_USBOTG_GHWCFG2_multiprocintrpt_END    (20)
#define SOC_USBOTG_GHWCFG2_nptxqdepth_START       (22)
#define SOC_USBOTG_GHWCFG2_nptxqdepth_END         (23)
#define SOC_USBOTG_GHWCFG2_ptxqdepth_START        (24)
#define SOC_USBOTG_GHWCFG2_ptxqdepth_END          (25)
#define SOC_USBOTG_GHWCFG2_tknqdepth_START        (26)
#define SOC_USBOTG_GHWCFG2_tknqdepth_END          (30)
#define SOC_USBOTG_GHWCFG2_otg_ic_usb_START       (31)
#define SOC_USBOTG_GHWCFG2_otg_ic_usb_END         (31)


/*****************************************************************************
 结构名    : SOC_USBOTG_GHWCFG3_UNION
 结构说明  : GHWCFG3 寄存器结构定义。地址偏移量:0x004C，初值:0x0F80D4E8，宽度:32
 寄存器说明: 硬件配置寄存器3。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  xfersizewidth   : 4;  /* bit[0-3]  : 传输大小记数器宽度。
                                                           0000：11bit；
                                                           0001：12bit；
                                                           ……
                                                           1000：19bit；
                                                           others：reserved。 */
        unsigned int  pktsizewidth    : 3;  /* bit[4-6]  : 包大小记数器宽度。
                                                           000：4bit；
                                                           001：5bit；
                                                           010：6bit；
                                                           011：7bit；
                                                           100：8bit；
                                                           101：9bit；
                                                           110：10bit；
                                                           others：reserved。 */
        unsigned int  otgen           : 1;  /* bit[7]    : 是否使能OTG功能。该bit由OTG_MODE参数决定，当OTG_MODE配置为0，1或2时，OtgEn=1。
                                                           0：不使能；
                                                           1：使能。 */
        unsigned int  i2cintsel       : 1;  /* bit[8]    : 是否支持I2C接口。
                                                           0：不支持；
                                                           1：支持。 */
        unsigned int  vndctlsupt      : 1;  /* bit[9]    : 是否支持Vendor Control接口。
                                                           0：不支持；
                                                           1：支持。 */
        unsigned int  optfeature      : 1;  /* bit[10]   : 是否支持取下列特性：User ID寄存器，GPIO接口，SOF翻转信号和记数器接口。
                                                           0：不支持；
                                                           1：支持。 */
        unsigned int  rsttype         : 1;  /* bit[11]   : 在always语句中，复位的类型。
                                                           0：异步复位；
                                                           1：同步复位。 */
        unsigned int  otg_adp_support : 1;  /* bit[12]   : 是否支持ADP。
                                                           0：不支持；
                                                           1：支持。 */
        unsigned int  otg_enable_hsic : 1;  /* bit[13]   : 是否支持HSIC。
                                                           0：不支持；
                                                           1：支持。 */
        unsigned int  otg_bc_support  : 1;  /* bit[14]   : 是否支持BC。
                                                           0：不支持；
                                                           1：支持。 */
        unsigned int  otg_enable_lpm  : 1;  /* bit[15]   : 是否支持LPM。
                                                           0：不支持；
                                                           1：支持。 */
        unsigned int  dfifodepth      : 16; /* bit[16-31]: 数据FIFO的深度，以4字节为单位。发送FIFO和接收FIFO大小之和不能大于该值。 */
    } reg;
} SOC_USBOTG_GHWCFG3_UNION;
#endif
#define SOC_USBOTG_GHWCFG3_xfersizewidth_START    (0)
#define SOC_USBOTG_GHWCFG3_xfersizewidth_END      (3)
#define SOC_USBOTG_GHWCFG3_pktsizewidth_START     (4)
#define SOC_USBOTG_GHWCFG3_pktsizewidth_END       (6)
#define SOC_USBOTG_GHWCFG3_otgen_START            (7)
#define SOC_USBOTG_GHWCFG3_otgen_END              (7)
#define SOC_USBOTG_GHWCFG3_i2cintsel_START        (8)
#define SOC_USBOTG_GHWCFG3_i2cintsel_END          (8)
#define SOC_USBOTG_GHWCFG3_vndctlsupt_START       (9)
#define SOC_USBOTG_GHWCFG3_vndctlsupt_END         (9)
#define SOC_USBOTG_GHWCFG3_optfeature_START       (10)
#define SOC_USBOTG_GHWCFG3_optfeature_END         (10)
#define SOC_USBOTG_GHWCFG3_rsttype_START          (11)
#define SOC_USBOTG_GHWCFG3_rsttype_END            (11)
#define SOC_USBOTG_GHWCFG3_otg_adp_support_START  (12)
#define SOC_USBOTG_GHWCFG3_otg_adp_support_END    (12)
#define SOC_USBOTG_GHWCFG3_otg_enable_hsic_START  (13)
#define SOC_USBOTG_GHWCFG3_otg_enable_hsic_END    (13)
#define SOC_USBOTG_GHWCFG3_otg_bc_support_START   (14)
#define SOC_USBOTG_GHWCFG3_otg_bc_support_END     (14)
#define SOC_USBOTG_GHWCFG3_otg_enable_lpm_START   (15)
#define SOC_USBOTG_GHWCFG3_otg_enable_lpm_END     (15)
#define SOC_USBOTG_GHWCFG3_dfifodepth_START       (16)
#define SOC_USBOTG_GHWCFG3_dfifodepth_END         (31)


/*****************************************************************************
 结构名    : SOC_USBOTG_GHWCFG4_UNION
 结构说明  : GHWCFG4 寄存器结构定义。地址偏移量:0x0050，初值:0xFFF08060，宽度:32
 寄存器说明: 硬件配置寄存器4。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  numdeveps     : 4;  /* bit[0-3]  : 支持周期IN端点个数。该域仅当DedFifoMode＝0时有效。 */
        unsigned int  enablepwropt  : 1;  /* bit[4]    : 是否使能功耗优化。使能功耗优化，USB模块分成掉电域和不掉电域，并且加入时钟门控逻辑。
                                                         0：不使能；
                                                         1：使能。 */
        unsigned int  ahbfreq       : 1;  /* bit[5]    : 最小AHB频率是否小于60MHz。
                                                         0：不是；
                                                         1：是。 */
        unsigned int  enbhiber      : 1;  /* bit[6]    : 使能冬眠功能。
                                                         0：冬眠功能不使能；
                                                         1：冬眠功能使能。 */
        unsigned int  reserved_0    : 7;  /* bit[7-13] : 保留。 */
        unsigned int  phydatawidth  : 2;  /* bit[14-15]: PHY数据线宽度。
                                                         00：8bit；
                                                         01：16bit；
                                                         10：软件可配置为8bit或者16bit；
                                                         others：reserved。 */
        unsigned int  numctleps     : 4;  /* bit[16-19]: 除了端点0之外支持的控制端点数目。 */
        unsigned int  iddigfltr     : 1;  /* bit[20]   : 是否支持信号iddig滤毛刺。
                                                         0：不支持；
                                                         1：支持。 */
        unsigned int  vbusvalidfltr : 1;  /* bit[21]   : 是否支持信号vbus_valid滤毛刺。
                                                         0：不支持；
                                                         1：支持。 */
        unsigned int  avalidfltr    : 1;  /* bit[22]   : 是否支持信号avalid滤毛刺。
                                                         0：不支持；
                                                         1：支持。 */
        unsigned int  bvalidfltr    : 1;  /* bit[23]   : 是否支持信号bvalid滤毛刺。
                                                         0：不支持；
                                                         1：支持。 */
        unsigned int  sessendfltr   : 1;  /* bit[24]   : 是否支持信号session_end滤毛刺。
                                                         0：不支持；
                                                         1：支持。 */
        unsigned int  dedfifomode   : 1;  /* bit[25]   : 是否支持IN端点专有发送FIFO功能。
                                                         0：不支持；
                                                         1：支持。 */
        unsigned int  ineps         : 4;  /* bit[26-29]: 包括控制端点在内，IN端点的数目。
                                                         0000：1个；
                                                         0001：2个；
                                                         ……
                                                         1111：16个。 */
        unsigned int  sgdma         : 1;  /* bit[30]   : 是否支持Scatter/Gather DMA。
                                                         0：不支持；
                                                         1：支持。 */
        unsigned int  reserved_1    : 1;  /* bit[31]   : 保留。 */
    } reg;
} SOC_USBOTG_GHWCFG4_UNION;
#endif
#define SOC_USBOTG_GHWCFG4_numdeveps_START      (0)
#define SOC_USBOTG_GHWCFG4_numdeveps_END        (3)
#define SOC_USBOTG_GHWCFG4_enablepwropt_START   (4)
#define SOC_USBOTG_GHWCFG4_enablepwropt_END     (4)
#define SOC_USBOTG_GHWCFG4_ahbfreq_START        (5)
#define SOC_USBOTG_GHWCFG4_ahbfreq_END          (5)
#define SOC_USBOTG_GHWCFG4_enbhiber_START       (6)
#define SOC_USBOTG_GHWCFG4_enbhiber_END         (6)
#define SOC_USBOTG_GHWCFG4_phydatawidth_START   (14)
#define SOC_USBOTG_GHWCFG4_phydatawidth_END     (15)
#define SOC_USBOTG_GHWCFG4_numctleps_START      (16)
#define SOC_USBOTG_GHWCFG4_numctleps_END        (19)
#define SOC_USBOTG_GHWCFG4_iddigfltr_START      (20)
#define SOC_USBOTG_GHWCFG4_iddigfltr_END        (20)
#define SOC_USBOTG_GHWCFG4_vbusvalidfltr_START  (21)
#define SOC_USBOTG_GHWCFG4_vbusvalidfltr_END    (21)
#define SOC_USBOTG_GHWCFG4_avalidfltr_START     (22)
#define SOC_USBOTG_GHWCFG4_avalidfltr_END       (22)
#define SOC_USBOTG_GHWCFG4_bvalidfltr_START     (23)
#define SOC_USBOTG_GHWCFG4_bvalidfltr_END       (23)
#define SOC_USBOTG_GHWCFG4_sessendfltr_START    (24)
#define SOC_USBOTG_GHWCFG4_sessendfltr_END      (24)
#define SOC_USBOTG_GHWCFG4_dedfifomode_START    (25)
#define SOC_USBOTG_GHWCFG4_dedfifomode_END      (25)
#define SOC_USBOTG_GHWCFG4_ineps_START          (26)
#define SOC_USBOTG_GHWCFG4_ineps_END            (29)
#define SOC_USBOTG_GHWCFG4_sgdma_START          (30)
#define SOC_USBOTG_GHWCFG4_sgdma_END            (30)


/*****************************************************************************
 结构名    : SOC_USBOTG_GLPMCFG_UNION
 结构说明  : GLPMCFG 寄存器结构定义。地址偏移量:0x0054，初值:0x00000000，宽度:32
 寄存器说明: LPM配置寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  lpmcap           : 1;  /* bit[0]    : LPM使能。
                                                            0：不使能；
                                                            1：使能。 */
        unsigned int  appl1res         : 1;  /* bit[1]    : LPM响应配置，仅对device模式有效。且LPM必须使能。如果LPM不使能，则一直返回NYET。
                                                            0：NYET；
                                                            1：ACK。 */
        unsigned int  hird             : 4;  /* bit[2-5]  : Resume保持时间。
                                                            对于host模式，该域可读写。软件配置为LPM包传输的HIRD值。
                                                            对于device模式，该域只读。收到LPM包之后控制器更新该域。
                                                            0000：50us；
                                                            0001：125us；
                                                            0010：250us；
                                                            0011：275us；
                                                            0100：350us；
                                                            0101：425us；
                                                            0110：500us；
                                                            0111：575us；
                                                            1000：650us；
                                                            1001：725us；
                                                            1010：800us；
                                                            1011：875us；
                                                            1100：950us；
                                                            1101：1025us；
                                                            1110：1100us；
                                                            1111：1175us。 */
        unsigned int  bremotewake      : 1;  /* bit[6]    : 远程唤醒使能。当device收到bRemoteWake LPM包时，该域更新。
                                                            0：不使能；
                                                            1：使能。 */
        unsigned int  enblslpm         : 1;  /* bit[7]    : 使能utmi_sleep_n。当的处于L1状态时，软件设置该bit，使utmi_sleep_n信号置为有效。
                                                            0：不使能；
                                                            1：使能。 */
        unsigned int  hird_thres       : 5;  /* bit[8-12] : HIRD水线。最高bit为使能位，低4bit为水线值。
                                                            对于device模式，当HIRD时间超过水线值时，控制器置位SuspendM信号使PHY进入低功耗模式。
                                                            0000：60us；
                                                            0001：135us；
                                                            0010：210us；
                                                            0011：285us；
                                                            0100：360us；
                                                            0101：435us；
                                                            0110：510us；
                                                            0111：585us；
                                                            1000：660us；
                                                            1001：735us；
                                                            1010：810us；
                                                            1011：885us；
                                                            1100：960us；
                                                            others：reserved。 */
        unsigned int  corel1res        : 2;  /* bit[13-14]: LPM响应。
                                                            对于host模式，指示接收到的握手包；对于device模式，接收到LPM包之后按照该域发送握手包。
                                                            00：ERROR；
                                                            01：STALL；
                                                            10：NYET；
                                                            11：ACK。 */
        unsigned int  slpsts           : 1;  /* bit[15]   : sleep状态指示。
                                                            0：没有进入sleep状态；
                                                            1：进入sleep状态。 */
        unsigned int  l1resumeok       : 1;  /* bit[16]   : 指示是否可以从sleep状态唤醒。进入sleep状态50us之后，该比特置位。
                                                            0：不可以从sleep状态唤醒；
                                                            1：可以从sleep状态唤醒。 */
        unsigned int  lpm_chnl_index   : 4;  /* bit[17-20]: 发送LPM的通道号，对host模式有效。 */
        unsigned int  lpm_retry_cnt    : 3;  /* bit[21-23]: LPM Retry次数配置，对host模式有效。指示收到有效的响应之前，host可以重新发送LPM的次数。 */
        unsigned int  sndlpm           : 1;  /* bit[24]   : 发送LPM事务，对host模式有效。
                                                            软件配置该bit，控制发送扩展令牌包和LPM令牌包。接收到有效的响应(STALL，NYET或者ACK)之后，硬件清除该bit。
                                                            0：不使能；
                                                            1：使能。 */
        unsigned int  lpm_retrycnt_sts : 3;  /* bit[25-27]: LPM retry状态，对host模式有效。指示当前LPM序列剩余的retry数。 */
        unsigned int  enbesl           : 1;  /* bit[28]   : 使能LPM的BESL功能 */
        unsigned int  rstrslpsts       : 1;  /* bit[29]   : 仅在设备模式有效。基于BESL的值，使能LPM在掉电之后的的状态恢复 */
        unsigned int  hsiccon          : 1;  /* bit[30]   : 用于初始化HSIC的连接序列 */
        unsigned int  invselhsic       : 1;  /* bit[31]   : 控制HSIC使能/去使能，对该信号的意思取反 */
    } reg;
} SOC_USBOTG_GLPMCFG_UNION;
#endif
#define SOC_USBOTG_GLPMCFG_lpmcap_START            (0)
#define SOC_USBOTG_GLPMCFG_lpmcap_END              (0)
#define SOC_USBOTG_GLPMCFG_appl1res_START          (1)
#define SOC_USBOTG_GLPMCFG_appl1res_END            (1)
#define SOC_USBOTG_GLPMCFG_hird_START              (2)
#define SOC_USBOTG_GLPMCFG_hird_END                (5)
#define SOC_USBOTG_GLPMCFG_bremotewake_START       (6)
#define SOC_USBOTG_GLPMCFG_bremotewake_END         (6)
#define SOC_USBOTG_GLPMCFG_enblslpm_START          (7)
#define SOC_USBOTG_GLPMCFG_enblslpm_END            (7)
#define SOC_USBOTG_GLPMCFG_hird_thres_START        (8)
#define SOC_USBOTG_GLPMCFG_hird_thres_END          (12)
#define SOC_USBOTG_GLPMCFG_corel1res_START         (13)
#define SOC_USBOTG_GLPMCFG_corel1res_END           (14)
#define SOC_USBOTG_GLPMCFG_slpsts_START            (15)
#define SOC_USBOTG_GLPMCFG_slpsts_END              (15)
#define SOC_USBOTG_GLPMCFG_l1resumeok_START        (16)
#define SOC_USBOTG_GLPMCFG_l1resumeok_END          (16)
#define SOC_USBOTG_GLPMCFG_lpm_chnl_index_START    (17)
#define SOC_USBOTG_GLPMCFG_lpm_chnl_index_END      (20)
#define SOC_USBOTG_GLPMCFG_lpm_retry_cnt_START     (21)
#define SOC_USBOTG_GLPMCFG_lpm_retry_cnt_END       (23)
#define SOC_USBOTG_GLPMCFG_sndlpm_START            (24)
#define SOC_USBOTG_GLPMCFG_sndlpm_END              (24)
#define SOC_USBOTG_GLPMCFG_lpm_retrycnt_sts_START  (25)
#define SOC_USBOTG_GLPMCFG_lpm_retrycnt_sts_END    (27)
#define SOC_USBOTG_GLPMCFG_enbesl_START            (28)
#define SOC_USBOTG_GLPMCFG_enbesl_END              (28)
#define SOC_USBOTG_GLPMCFG_rstrslpsts_START        (29)
#define SOC_USBOTG_GLPMCFG_rstrslpsts_END          (29)
#define SOC_USBOTG_GLPMCFG_hsiccon_START           (30)
#define SOC_USBOTG_GLPMCFG_hsiccon_END             (30)
#define SOC_USBOTG_GLPMCFG_invselhsic_START        (31)
#define SOC_USBOTG_GLPMCFG_invselhsic_END          (31)


/*****************************************************************************
 结构名    : SOC_USBOTG_GPWRDN_UNION
 结构说明  : GPWRDN 寄存器结构定义。地址偏移量:0x0058，初值:0x00200010，宽度:32
 寄存器说明: 冬眠模式掉电控制寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  pmuintsel           : 1;  /* bit[0]    : PMU中断选择。该bit配置为1，将PMU产生的中断上报，控制器产生的中断屏蔽。
                                                               0：选择控制器产生的中断；
                                                               1：选择PMU产生的中断。 */
        unsigned int  pmuactv             : 1;  /* bit[1]    : 使能PMU功能。
                                                               0：不使能；
                                                               1：使能。 */
        unsigned int  restore             : 1;  /* bit[2]    : 寄存器恢复控制。
                                                               0：不使能；
                                                               1：使能。 */
        unsigned int  pwrdnclmp           : 1;  /* bit[3]    : 掉电箝位控制。软件配置该bit是控制器输出的信号固定在确定的状态。
                                                               0：不使能；
                                                               1：使能。 */
        unsigned int  pwrdnrst_n          : 1;  /* bit[4]    : 掉电复位控制。在退出冬眠模式或者ADP上电之前，软件必须配置该bit对控制器进行复位。
                                                               0：复位控制器；
                                                               1：不复位控制器。 */
        unsigned int  pwrdnswtch          : 1;  /* bit[5]    : 掉电开关。指示控制器电源关断状态。
                                                               0：ON；
                                                               1：OFF。
                                                               在正常操作状态下不能配置该bit。 */
        unsigned int  vbusoff             : 1;  /* bit[6]    : 在Host模式下，在进入冬眠模式之前：
                                                               0：HPRT[PrtPwr]配置没有配置为0；
                                                               1：HPRT[Prtpwr]配置为0。
                                                               在Device模式下，在对控制器掉电之前：
                                                               0：bvalid有效；
                                                               1：bvalid无效。
                                                               该bit仅当PMUActv＝1有效。 */
        unsigned int  lnstschng           : 1;  /* bit[7]    : 检测LineState变化中断。
                                                               0：无中断；
                                                               1：中断。 */
        unsigned int  lnstschngmsk        : 1;  /* bit[8]    : LnStsChng中断屏蔽位。
                                                               0：屏蔽；
                                                               1：不屏蔽。 */
        unsigned int  resetdetected       : 1;  /* bit[9]    : 检测到复位操作中断，对device模式有效。
                                                               0：无中断；
                                                               1：有中断。 */
        unsigned int  resetdetmsk         : 1;  /* bit[10]   : ResetDetcted中断屏蔽位，对device模式有效。
                                                               0：屏蔽；
                                                               1：不屏蔽。 */
        unsigned int  disconnectdetect    : 1;  /* bit[11]   : 断开连接中断，对host模式有效。
                                                               0：无中断；
                                                               1：有中断。 */
        unsigned int  disconnectdetectmsk : 1;  /* bit[12]   : DisconnectDetect中断屏蔽位。对host模式有效。
                                                               0：屏蔽；
                                                               1：不屏蔽。 */
        unsigned int  connectdet          : 1;  /* bit[13]   : 检测到连接中断。
                                                               0：无中断；
                                                               1：有中断。 */
        unsigned int  conndetmsk          : 1;  /* bit[14]   : ConnectDet中断屏蔽位。
                                                               0：屏蔽；
                                                               1：不屏蔽。 */
        unsigned int  srpdetect           : 1;  /* bit[15]   : 检测到SRP中断。对host模式有效。
                                                               0：有中断；
                                                               1：无中断。 */
        unsigned int  srpdetectmsk        : 1;  /* bit[16]   : SRPDetect中断屏蔽位。对host模式有效。
                                                               0：不屏蔽；
                                                               1：屏蔽。 */
        unsigned int  stschngint          : 1;  /* bit[17]   : StsChng中断。指示IDDIG或者bvalid状态变化。
                                                               0：无中断；
                                                               1：有中断。 */
        unsigned int  stschngmsk          : 1;  /* bit[18]   : StsChng中断屏蔽位。
                                                               0：屏蔽；
                                                               1：不屏蔽。 */
        unsigned int  linestate           : 2;  /* bit[19-20]: 当前linestate。
                                                               00：DM=0，DP=0；
                                                               01：DM=0，DP=1；
                                                               10：DM=1，DP=0；
                                                               11：reserved。 */
        unsigned int  iddig               : 1;  /* bit[21]   : IDDIG状态。当PMUActv＝1有效。
                                                               0：低电平；
                                                               1：高电平。 */
        unsigned int  bsessvld            : 1;  /* bit[22]   : bvalid有效状态。当PMUActv＝1有效。
                                                               0：无效；
                                                               1：有效。 */
        unsigned int  adpint              : 1;  /* bit[23]   : ADP中断状态。
                                                               0：无中断；
                                                               1：有中断。 */
        unsigned int  multvalidbc         : 5;  /* bit[24-28]: 在Baterry Charging的ACA模式下输入ID电阻阻值范围。
                                                               00001：rid_c阻值范围；
                                                               00010：rid_b阻值范围；
                                                               00100：rid_a阻值范围；
                                                               01000：rid_gnd阻值范围；
                                                               10000：rid_float阻值范围；
                                                               others：reserved。 */
        unsigned int  reserved            : 3;  /* bit[29-31]: 保留。 */
    } reg;
} SOC_USBOTG_GPWRDN_UNION;
#endif
#define SOC_USBOTG_GPWRDN_pmuintsel_START            (0)
#define SOC_USBOTG_GPWRDN_pmuintsel_END              (0)
#define SOC_USBOTG_GPWRDN_pmuactv_START              (1)
#define SOC_USBOTG_GPWRDN_pmuactv_END                (1)
#define SOC_USBOTG_GPWRDN_restore_START              (2)
#define SOC_USBOTG_GPWRDN_restore_END                (2)
#define SOC_USBOTG_GPWRDN_pwrdnclmp_START            (3)
#define SOC_USBOTG_GPWRDN_pwrdnclmp_END              (3)
#define SOC_USBOTG_GPWRDN_pwrdnrst_n_START           (4)
#define SOC_USBOTG_GPWRDN_pwrdnrst_n_END             (4)
#define SOC_USBOTG_GPWRDN_pwrdnswtch_START           (5)
#define SOC_USBOTG_GPWRDN_pwrdnswtch_END             (5)
#define SOC_USBOTG_GPWRDN_vbusoff_START              (6)
#define SOC_USBOTG_GPWRDN_vbusoff_END                (6)
#define SOC_USBOTG_GPWRDN_lnstschng_START            (7)
#define SOC_USBOTG_GPWRDN_lnstschng_END              (7)
#define SOC_USBOTG_GPWRDN_lnstschngmsk_START         (8)
#define SOC_USBOTG_GPWRDN_lnstschngmsk_END           (8)
#define SOC_USBOTG_GPWRDN_resetdetected_START        (9)
#define SOC_USBOTG_GPWRDN_resetdetected_END          (9)
#define SOC_USBOTG_GPWRDN_resetdetmsk_START          (10)
#define SOC_USBOTG_GPWRDN_resetdetmsk_END            (10)
#define SOC_USBOTG_GPWRDN_disconnectdetect_START     (11)
#define SOC_USBOTG_GPWRDN_disconnectdetect_END       (11)
#define SOC_USBOTG_GPWRDN_disconnectdetectmsk_START  (12)
#define SOC_USBOTG_GPWRDN_disconnectdetectmsk_END    (12)
#define SOC_USBOTG_GPWRDN_connectdet_START           (13)
#define SOC_USBOTG_GPWRDN_connectdet_END             (13)
#define SOC_USBOTG_GPWRDN_conndetmsk_START           (14)
#define SOC_USBOTG_GPWRDN_conndetmsk_END             (14)
#define SOC_USBOTG_GPWRDN_srpdetect_START            (15)
#define SOC_USBOTG_GPWRDN_srpdetect_END              (15)
#define SOC_USBOTG_GPWRDN_srpdetectmsk_START         (16)
#define SOC_USBOTG_GPWRDN_srpdetectmsk_END           (16)
#define SOC_USBOTG_GPWRDN_stschngint_START           (17)
#define SOC_USBOTG_GPWRDN_stschngint_END             (17)
#define SOC_USBOTG_GPWRDN_stschngmsk_START           (18)
#define SOC_USBOTG_GPWRDN_stschngmsk_END             (18)
#define SOC_USBOTG_GPWRDN_linestate_START            (19)
#define SOC_USBOTG_GPWRDN_linestate_END              (20)
#define SOC_USBOTG_GPWRDN_iddig_START                (21)
#define SOC_USBOTG_GPWRDN_iddig_END                  (21)
#define SOC_USBOTG_GPWRDN_bsessvld_START             (22)
#define SOC_USBOTG_GPWRDN_bsessvld_END               (22)
#define SOC_USBOTG_GPWRDN_adpint_START               (23)
#define SOC_USBOTG_GPWRDN_adpint_END                 (23)
#define SOC_USBOTG_GPWRDN_multvalidbc_START          (24)
#define SOC_USBOTG_GPWRDN_multvalidbc_END            (28)


/*****************************************************************************
 结构名    : SOC_USBOTG_GDFIFOCFG_UNION
 结构说明  : GDFIFOCFG 寄存器结构定义。地址偏移量:0x005C，初值:0x0F801000，宽度:32
 寄存器说明: 动态FIFO配置寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  gdfifocfg      : 16; /* bit[0-15] : 内部RAM深度，以4字节为单位。 */
        unsigned int  epinfobaseaddr : 16; /* bit[16-31]: 保存在内部RAM里的寄存器起始地址，以4字节为单位。注意这部分地址空间不能用作FIFO空间，且逻辑也没有做保护。 */
    } reg;
} SOC_USBOTG_GDFIFOCFG_UNION;
#endif
#define SOC_USBOTG_GDFIFOCFG_gdfifocfg_START       (0)
#define SOC_USBOTG_GDFIFOCFG_gdfifocfg_END         (15)
#define SOC_USBOTG_GDFIFOCFG_epinfobaseaddr_START  (16)
#define SOC_USBOTG_GDFIFOCFG_epinfobaseaddr_END    (31)


/*****************************************************************************
 结构名    : SOC_USBOTG_GADPCTL_UNION
 结构说明  : GADPCTL 寄存器结构定义。地址偏移量:0x0060，初值:0x00000000，宽度:32
 寄存器说明: ADP控制寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  prbdschng    : 2;  /* bit[0-1]  : Probe操作放电时间。
                                                        00：4毫秒；
                                                        01：8毫秒；
                                                        10：16毫秒；
                                                        11：32毫秒。 */
        unsigned int  prbdelta     : 2;  /* bit[2-3]  : RTIM时间刻度。
                                                        00：1个时钟周期；
                                                        01：2个时钟周期；
                                                        10：3个时钟周期；
                                                        11：4个时钟周期。 */
        unsigned int  prbper       : 2;  /* bit[4-5]  : Probe周期。
                                                        00：0.625秒到0.925秒（典型值0.775秒）；
                                                        01：1.25秒到1.85秒（典型值1.55秒）；
                                                        10：1.9秒到2.6秒（典型值2.275秒）；
                                                        11：reserved。 */
        unsigned int  rtim         : 11; /* bit[6-16] : VBUS从Vadp_sink上升到Vadp_prb的时间。以PrbDelta表示的32KHz时钟周期为单位。 */
        unsigned int  enaprb       : 1;  /* bit[17]   : AdpPrb使能。
                                                        0：不使能；
                                                        1：使能。 */
        unsigned int  enasns       : 1;  /* bit[18]   : AdpSns使能。
                                                        0：不使能；
                                                        1：使能。 */
        unsigned int  adpres       : 1;  /* bit[19]   : ADP复位控制。
                                                        0：不复位；
                                                        1：复位。 */
        unsigned int  adpen        : 1;  /* bit[20]   : ADP使能。
                                                        0：不使能；
                                                        1：使能。 */
        unsigned int  adpprbint    : 1;  /* bit[21]   : AdpPrb中断。当VBUS电压超过Vadpprb触发该中断。
                                                        0：无中断；
                                                        1：有中断。 */
        unsigned int  adpsnsint    : 1;  /* bit[22]   : AdpSns中断。当VBUS电压超过Vadpsns触发该中断。
                                                        0：无中断；
                                                        1：有中断。 */
        unsigned int  adptmoutint  : 1;  /* bit[23]   : ADP超时中断。
                                                        0：无中断；
                                                        1：有中断。 */
        unsigned int  adpprbintmsk : 1;  /* bit[24]   : AdpPrb中断屏蔽位。
                                                        0：屏蔽；
                                                        1：不屏蔽。 */
        unsigned int  adpsnsintmsk : 1;  /* bit[25]   : AdpSns中断屏蔽位。
                                                        0：屏蔽；
                                                        1：不屏蔽。 */
        unsigned int  adptmoutmsk  : 1;  /* bit[26]   : ADP超时中断屏蔽位。
                                                        0：屏蔽；
                                                        1：不屏蔽。 */
        unsigned int  ar           : 2;  /* bit[27-28]: 读写请求。
                                                        00：操作有效，控制器自动更新；
                                                        01：读操作；
                                                        10：写操作；
                                                        11：保留。 */
        unsigned int  reserved     : 3;  /* bit[29-31]: 保留。 */
    } reg;
} SOC_USBOTG_GADPCTL_UNION;
#endif
#define SOC_USBOTG_GADPCTL_prbdschng_START     (0)
#define SOC_USBOTG_GADPCTL_prbdschng_END       (1)
#define SOC_USBOTG_GADPCTL_prbdelta_START      (2)
#define SOC_USBOTG_GADPCTL_prbdelta_END        (3)
#define SOC_USBOTG_GADPCTL_prbper_START        (4)
#define SOC_USBOTG_GADPCTL_prbper_END          (5)
#define SOC_USBOTG_GADPCTL_rtim_START          (6)
#define SOC_USBOTG_GADPCTL_rtim_END            (16)
#define SOC_USBOTG_GADPCTL_enaprb_START        (17)
#define SOC_USBOTG_GADPCTL_enaprb_END          (17)
#define SOC_USBOTG_GADPCTL_enasns_START        (18)
#define SOC_USBOTG_GADPCTL_enasns_END          (18)
#define SOC_USBOTG_GADPCTL_adpres_START        (19)
#define SOC_USBOTG_GADPCTL_adpres_END          (19)
#define SOC_USBOTG_GADPCTL_adpen_START         (20)
#define SOC_USBOTG_GADPCTL_adpen_END           (20)
#define SOC_USBOTG_GADPCTL_adpprbint_START     (21)
#define SOC_USBOTG_GADPCTL_adpprbint_END       (21)
#define SOC_USBOTG_GADPCTL_adpsnsint_START     (22)
#define SOC_USBOTG_GADPCTL_adpsnsint_END       (22)
#define SOC_USBOTG_GADPCTL_adptmoutint_START   (23)
#define SOC_USBOTG_GADPCTL_adptmoutint_END     (23)
#define SOC_USBOTG_GADPCTL_adpprbintmsk_START  (24)
#define SOC_USBOTG_GADPCTL_adpprbintmsk_END    (24)
#define SOC_USBOTG_GADPCTL_adpsnsintmsk_START  (25)
#define SOC_USBOTG_GADPCTL_adpsnsintmsk_END    (25)
#define SOC_USBOTG_GADPCTL_adptmoutmsk_START   (26)
#define SOC_USBOTG_GADPCTL_adptmoutmsk_END     (26)
#define SOC_USBOTG_GADPCTL_ar_START            (27)
#define SOC_USBOTG_GADPCTL_ar_END              (28)


/*****************************************************************************
 结构名    : SOC_USBOTG_HPTXFSIZ_UNION
 结构说明  : HPTXFSIZ 寄存器结构定义。地址偏移量:0x0100，初值:0x00000000，宽度:32
 寄存器说明: Host周期发送FIFO配置寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ptxfstaddr : 16; /* bit[0-15] : Host周期发送FIFO起始地址，以4字节为单位。 */
        unsigned int  ptxfsize   : 16; /* bit[16-31]: Host周期发送FIFO深度，以4字节为单位。 */
    } reg;
} SOC_USBOTG_HPTXFSIZ_UNION;
#endif
#define SOC_USBOTG_HPTXFSIZ_ptxfstaddr_START  (0)
#define SOC_USBOTG_HPTXFSIZ_ptxfstaddr_END    (15)
#define SOC_USBOTG_HPTXFSIZ_ptxfsize_START    (16)
#define SOC_USBOTG_HPTXFSIZ_ptxfsize_END      (31)


/*****************************************************************************
 结构名    : SOC_USBOTG_DIEPTXF1_UNION
 结构说明  : DIEPTXF1 寄存器结构定义。地址偏移量:0x0104，初值:0x10002000，宽度:32
 寄存器说明: Device IN端点1发送FIFO配置寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  inepntxfstaddr : 16; /* bit[0-15] : IN端点发送FIFO起始地址，以4字节为单位。 */
        unsigned int  inepntxfdep    : 16; /* bit[16-31]: IN端点发送FIFO深度，以4字节为单位。 */
    } reg;
} SOC_USBOTG_DIEPTXF1_UNION;
#endif
#define SOC_USBOTG_DIEPTXF1_inepntxfstaddr_START  (0)
#define SOC_USBOTG_DIEPTXF1_inepntxfstaddr_END    (15)
#define SOC_USBOTG_DIEPTXF1_inepntxfdep_START     (16)
#define SOC_USBOTG_DIEPTXF1_inepntxfdep_END       (31)


/*****************************************************************************
 结构名    : SOC_USBOTG_DIEPTXF2_UNION
 结构说明  : DIEPTXF2 寄存器结构定义。地址偏移量:0x0108，初值:0x10003000，宽度:32
 寄存器说明: Device IN端点2发送FIFO配置寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  inepntxfstaddr : 16; /* bit[0-15] : IN端点发送FIFO起始地址，以4字节为单位。 */
        unsigned int  inepntxfdep    : 16; /* bit[16-31]: IN端点发送FIFO深度，以4字节为单位。 */
    } reg;
} SOC_USBOTG_DIEPTXF2_UNION;
#endif
#define SOC_USBOTG_DIEPTXF2_inepntxfstaddr_START  (0)
#define SOC_USBOTG_DIEPTXF2_inepntxfstaddr_END    (15)
#define SOC_USBOTG_DIEPTXF2_inepntxfdep_START     (16)
#define SOC_USBOTG_DIEPTXF2_inepntxfdep_END       (31)


/*****************************************************************************
 结构名    : SOC_USBOTG_DIEPTXF3_UNION
 结构说明  : DIEPTXF3 寄存器结构定义。地址偏移量:0x010C，初值:0x10004000，宽度:32
 寄存器说明: Device IN端点3发送FIFO配置寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  inepntxfstaddr : 16; /* bit[0-15] : IN端点发送FIFO起始地址，以4字节为单位。 */
        unsigned int  inepntxfdep    : 16; /* bit[16-31]: IN端点发送FIFO深度，以4字节为单位。 */
    } reg;
} SOC_USBOTG_DIEPTXF3_UNION;
#endif
#define SOC_USBOTG_DIEPTXF3_inepntxfstaddr_START  (0)
#define SOC_USBOTG_DIEPTXF3_inepntxfstaddr_END    (15)
#define SOC_USBOTG_DIEPTXF3_inepntxfdep_START     (16)
#define SOC_USBOTG_DIEPTXF3_inepntxfdep_END       (31)


/*****************************************************************************
 结构名    : SOC_USBOTG_DIEPTXF4_UNION
 结构说明  : DIEPTXF4 寄存器结构定义。地址偏移量:0x0110，初值:0x10005000，宽度:32
 寄存器说明: Device IN端点4发送FIFO配置寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  inepntxfstaddr : 16; /* bit[0-15] : IN端点发送FIFO起始地址，以4字节为单位。 */
        unsigned int  inepntxfdep    : 16; /* bit[16-31]: IN端点发送FIFO深度，以4字节为单位。 */
    } reg;
} SOC_USBOTG_DIEPTXF4_UNION;
#endif
#define SOC_USBOTG_DIEPTXF4_inepntxfstaddr_START  (0)
#define SOC_USBOTG_DIEPTXF4_inepntxfstaddr_END    (15)
#define SOC_USBOTG_DIEPTXF4_inepntxfdep_START     (16)
#define SOC_USBOTG_DIEPTXF4_inepntxfdep_END       (31)


/*****************************************************************************
 结构名    : SOC_USBOTG_DIEPTXF5_UNION
 结构说明  : DIEPTXF5 寄存器结构定义。地址偏移量:0x0114，初值:0x10006000，宽度:32
 寄存器说明: Device IN端点5发送FIFO配置寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  inepntxfstaddr : 16; /* bit[0-15] : IN端点发送FIFO起始地址，以4字节为单位。 */
        unsigned int  inepntxfdep    : 16; /* bit[16-31]: IN端点发送FIFO深度，以4字节为单位。 */
    } reg;
} SOC_USBOTG_DIEPTXF5_UNION;
#endif
#define SOC_USBOTG_DIEPTXF5_inepntxfstaddr_START  (0)
#define SOC_USBOTG_DIEPTXF5_inepntxfstaddr_END    (15)
#define SOC_USBOTG_DIEPTXF5_inepntxfdep_START     (16)
#define SOC_USBOTG_DIEPTXF5_inepntxfdep_END       (31)


/*****************************************************************************
 结构名    : SOC_USBOTG_DIEPTXF6_UNION
 结构说明  : DIEPTXF6 寄存器结构定义。地址偏移量:0x0118，初值:0x10007000，宽度:32
 寄存器说明: Device IN端点6发送FIFO配置寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  inepntxfstaddr : 16; /* bit[0-15] : IN端点发送FIFO起始地址，以4字节为单位。 */
        unsigned int  inepntxfdep    : 16; /* bit[16-31]: IN端点发送FIFO深度，以4字节为单位。 */
    } reg;
} SOC_USBOTG_DIEPTXF6_UNION;
#endif
#define SOC_USBOTG_DIEPTXF6_inepntxfstaddr_START  (0)
#define SOC_USBOTG_DIEPTXF6_inepntxfstaddr_END    (15)
#define SOC_USBOTG_DIEPTXF6_inepntxfdep_START     (16)
#define SOC_USBOTG_DIEPTXF6_inepntxfdep_END       (31)


/*****************************************************************************
 结构名    : SOC_USBOTG_DIEPTXF7_UNION
 结构说明  : DIEPTXF7 寄存器结构定义。地址偏移量:0x011C，初值:0x10008000，宽度:32
 寄存器说明: Device IN端点7发送FIFO配置寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  inepntxfstaddr : 16; /* bit[0-15] : IN端点发送FIFO起始地址，以4字节为单位。 */
        unsigned int  inepntxfdep    : 16; /* bit[16-31]: IN端点发送FIFO深度，以4字节为单位。 */
    } reg;
} SOC_USBOTG_DIEPTXF7_UNION;
#endif
#define SOC_USBOTG_DIEPTXF7_inepntxfstaddr_START  (0)
#define SOC_USBOTG_DIEPTXF7_inepntxfstaddr_END    (15)
#define SOC_USBOTG_DIEPTXF7_inepntxfdep_START     (16)
#define SOC_USBOTG_DIEPTXF7_inepntxfdep_END       (31)


/*****************************************************************************
 结构名    : SOC_USBOTG_DIEPTXF8_UNION
 结构说明  : DIEPTXF8 寄存器结构定义。地址偏移量:0x0120，初值:0x10009000，宽度:32
 寄存器说明: Device IN端点8发送FIFO配置寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  inepntxfstaddr : 16; /* bit[0-15] : IN端点发送FIFO起始地址，以4字节为单位。 */
        unsigned int  inepntxfdep    : 16; /* bit[16-31]: IN端点发送FIFO深度，以4字节为单位。 */
    } reg;
} SOC_USBOTG_DIEPTXF8_UNION;
#endif
#define SOC_USBOTG_DIEPTXF8_inepntxfstaddr_START  (0)
#define SOC_USBOTG_DIEPTXF8_inepntxfstaddr_END    (15)
#define SOC_USBOTG_DIEPTXF8_inepntxfdep_START     (16)
#define SOC_USBOTG_DIEPTXF8_inepntxfdep_END       (31)


/*****************************************************************************
 结构名    : SOC_USBOTG_DIEPTXF9_UNION
 结构说明  : DIEPTXF9 寄存器结构定义。地址偏移量:0x0124，初值:0x1000A000，宽度:32
 寄存器说明: Device IN端点9发送FIFO配置寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  inepntxfstaddr : 16; /* bit[0-15] : IN端点发送FIFO起始地址，以4字节为单位。 */
        unsigned int  inepntxfdep    : 16; /* bit[16-31]: IN端点发送FIFO深度，以4字节为单位。 */
    } reg;
} SOC_USBOTG_DIEPTXF9_UNION;
#endif
#define SOC_USBOTG_DIEPTXF9_inepntxfstaddr_START  (0)
#define SOC_USBOTG_DIEPTXF9_inepntxfstaddr_END    (15)
#define SOC_USBOTG_DIEPTXF9_inepntxfdep_START     (16)
#define SOC_USBOTG_DIEPTXF9_inepntxfdep_END       (31)


/*****************************************************************************
 结构名    : SOC_USBOTG_DIEPTXF10_UNION
 结构说明  : DIEPTXF10 寄存器结构定义。地址偏移量:0x0128，初值:0x1000B000，宽度:32
 寄存器说明: Device IN端点10发送FIFO配置寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  inepntxfstaddr : 16; /* bit[0-15] : IN端点发送FIFO起始地址，以4字节为单位。 */
        unsigned int  inepntxfdep    : 16; /* bit[16-31]: IN端点发送FIFO深度，以4字节为单位。 */
    } reg;
} SOC_USBOTG_DIEPTXF10_UNION;
#endif
#define SOC_USBOTG_DIEPTXF10_inepntxfstaddr_START  (0)
#define SOC_USBOTG_DIEPTXF10_inepntxfstaddr_END    (15)
#define SOC_USBOTG_DIEPTXF10_inepntxfdep_START     (16)
#define SOC_USBOTG_DIEPTXF10_inepntxfdep_END       (31)


/*****************************************************************************
 结构名    : SOC_USBOTG_DIEPTXF11_UNION
 结构说明  : DIEPTXF11 寄存器结构定义。地址偏移量:0x012C，初值:0x1000C000，宽度:32
 寄存器说明: Device IN端点11发送FIFO配置寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  inepntxfstaddr : 16; /* bit[0-15] : IN端点发送FIFO起始地址，以4字节为单位。 */
        unsigned int  inepntxfdep    : 16; /* bit[16-31]: IN端点发送FIFO深度，以4字节为单位。 */
    } reg;
} SOC_USBOTG_DIEPTXF11_UNION;
#endif
#define SOC_USBOTG_DIEPTXF11_inepntxfstaddr_START  (0)
#define SOC_USBOTG_DIEPTXF11_inepntxfstaddr_END    (15)
#define SOC_USBOTG_DIEPTXF11_inepntxfdep_START     (16)
#define SOC_USBOTG_DIEPTXF11_inepntxfdep_END       (31)


/*****************************************************************************
 结构名    : SOC_USBOTG_DIEPTXF12_UNION
 结构说明  : DIEPTXF12 寄存器结构定义。地址偏移量:0x0130，初值:0x1000D000，宽度:32
 寄存器说明: Device IN端点12发送FIFO配置寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  inepntxfstaddr : 16; /* bit[0-15] : IN端点发送FIFO起始地址，以4字节为单位。 */
        unsigned int  inepntxfdep    : 16; /* bit[16-31]: IN端点发送FIFO深度，以4字节为单位。 */
    } reg;
} SOC_USBOTG_DIEPTXF12_UNION;
#endif
#define SOC_USBOTG_DIEPTXF12_inepntxfstaddr_START  (0)
#define SOC_USBOTG_DIEPTXF12_inepntxfstaddr_END    (15)
#define SOC_USBOTG_DIEPTXF12_inepntxfdep_START     (16)
#define SOC_USBOTG_DIEPTXF12_inepntxfdep_END       (31)


/*****************************************************************************
 结构名    : SOC_USBOTG_DIEPTXF13_UNION
 结构说明  : DIEPTXF13 寄存器结构定义。地址偏移量:0x0134，初值:0x1000E000，宽度:32
 寄存器说明: Device IN端点13发送FIFO配置寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  inepntxfstaddr : 16; /* bit[0-15] : IN端点发送FIFO起始地址，以4字节为单位。 */
        unsigned int  inepntxfdep    : 16; /* bit[16-31]: IN端点发送FIFO深度，以4字节为单位。 */
    } reg;
} SOC_USBOTG_DIEPTXF13_UNION;
#endif
#define SOC_USBOTG_DIEPTXF13_inepntxfstaddr_START  (0)
#define SOC_USBOTG_DIEPTXF13_inepntxfstaddr_END    (15)
#define SOC_USBOTG_DIEPTXF13_inepntxfdep_START     (16)
#define SOC_USBOTG_DIEPTXF13_inepntxfdep_END       (31)


/*****************************************************************************
 结构名    : SOC_USBOTG_DIEPTXF14_UNION
 结构说明  : DIEPTXF14 寄存器结构定义。地址偏移量:0x0138，初值:0x1000F000，宽度:32
 寄存器说明: Device IN端点14发送FIFO配置寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  inepntxfstaddr : 16; /* bit[0-15] : IN端点发送FIFO起始地址，以4字节为单位。 */
        unsigned int  inepntxfdep    : 16; /* bit[16-31]: IN端点发送FIFO深度，以4字节为单位。 */
    } reg;
} SOC_USBOTG_DIEPTXF14_UNION;
#endif
#define SOC_USBOTG_DIEPTXF14_inepntxfstaddr_START  (0)
#define SOC_USBOTG_DIEPTXF14_inepntxfstaddr_END    (15)
#define SOC_USBOTG_DIEPTXF14_inepntxfdep_START     (16)
#define SOC_USBOTG_DIEPTXF14_inepntxfdep_END       (31)


/*****************************************************************************
 结构名    : SOC_USBOTG_DIEPTXF15_UNION
 结构说明  : DIEPTXF15 寄存器结构定义。地址偏移量:0x013C，初值:0x10000000，宽度:32
 寄存器说明: Device IN端点15发送FIFO配置寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  inepntxfstaddr : 16; /* bit[0-15] : IN端点发送FIFO起始地址，以4字节为单位。 */
        unsigned int  inepntxfdep    : 16; /* bit[16-31]: IN端点发送FIFO深度，以4字节为单位。 */
    } reg;
} SOC_USBOTG_DIEPTXF15_UNION;
#endif
#define SOC_USBOTG_DIEPTXF15_inepntxfstaddr_START  (0)
#define SOC_USBOTG_DIEPTXF15_inepntxfstaddr_END    (15)
#define SOC_USBOTG_DIEPTXF15_inepntxfdep_START     (16)
#define SOC_USBOTG_DIEPTXF15_inepntxfdep_END       (31)


/*****************************************************************************
 结构名    : SOC_USBOTG_HCFG_UNION
 结构说明  : HCFG 寄存器结构定义。地址偏移量:0x0400，初值:0x00000200，宽度:32
 寄存器说明: Host配置寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  fslspclksel : 2;  /* bit[0-1]  : FS/LS PHY接口时钟选择。
                                                       00：30/60MHz；
                                                       01：48MHz；
                                                       others：reserved。 */
        unsigned int  fslssupp    : 1;  /* bit[2]    : 仅支持FS和LS使能。
                                                       控制枚举速度。该bit使能，即使device支持HS，控制器也会枚举成FS Host。
                                                       0：不使能；
                                                       1：使能。 */
        unsigned int  reserved_0  : 4;  /* bit[3-6]  : 保留。 */
        unsigned int  ena32khzs   : 1;  /* bit[7]    : 使能32KHz suspend模式。改bit必须配置为0，不支持32KHz suspend模式。
                                                       0：不使能；
                                                       1：使能。 */
        unsigned int  resvalid    : 8;  /* bit[8-15] : Resume有效长度。
                                                       仅当HCFG[Ena32KHz]设置为1有效。控制器检测resume信号，超过ResValid个时钟周期认为resume有效。 */
        unsigned int  reserved_1  : 7;  /* bit[16-22]: 保留。 */
        unsigned int  descdma     : 1;  /* bit[23]   : 使能Scatter/Gather DMA模式。
                                                       0：不使能；
                                                       1：使能。 */
        unsigned int  frlisten    : 2;  /* bit[24-25]: 帧列表大小。仅对Scatter/Gather DMA模式有效。
                                                       00：8；
                                                       01：16；
                                                       10：32；
                                                       11：64。 */
        unsigned int  perschedena : 1;  /* bit[26]   : 使能周期传输。仅对Scatter/Gather DMA模式有效。
                                                       0：不使能；
                                                       1：使能。 */
        unsigned int  resered     : 4;  /* bit[27-30]: 保留。 */
        unsigned int  modechtimen : 1;  /* bit[31]   : 模式改变计数使能。
                                                       使能Host控制器在Resume结束等待200个PHY时钟周期，然后将opmode信号改变为00。
                                                       0：等待200个PHY时钟周期或者等待linestate变成SE0；
                                                       1：等待linestate变成SE0。 */
    } reg;
} SOC_USBOTG_HCFG_UNION;
#endif
#define SOC_USBOTG_HCFG_fslspclksel_START  (0)
#define SOC_USBOTG_HCFG_fslspclksel_END    (1)
#define SOC_USBOTG_HCFG_fslssupp_START     (2)
#define SOC_USBOTG_HCFG_fslssupp_END       (2)
#define SOC_USBOTG_HCFG_ena32khzs_START    (7)
#define SOC_USBOTG_HCFG_ena32khzs_END      (7)
#define SOC_USBOTG_HCFG_resvalid_START     (8)
#define SOC_USBOTG_HCFG_resvalid_END       (15)
#define SOC_USBOTG_HCFG_descdma_START      (23)
#define SOC_USBOTG_HCFG_descdma_END        (23)
#define SOC_USBOTG_HCFG_frlisten_START     (24)
#define SOC_USBOTG_HCFG_frlisten_END       (25)
#define SOC_USBOTG_HCFG_perschedena_START  (26)
#define SOC_USBOTG_HCFG_perschedena_END    (26)
#define SOC_USBOTG_HCFG_resered_START      (27)
#define SOC_USBOTG_HCFG_resered_END        (30)
#define SOC_USBOTG_HCFG_modechtimen_START  (31)
#define SOC_USBOTG_HCFG_modechtimen_END    (31)


/*****************************************************************************
 结构名    : SOC_USBOTG_HFIR_UNION
 结构说明  : HFIR 寄存器结构定义。地址偏移量:0x0404，初值:0x0000EA06，宽度:32
 寄存器说明: Host帧间隔寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  frint       : 16; /* bit[0-15] : 帧间隔。以PHY接口时钟周期为单位。Host间隔FrInt个时钟周期发送1个SOF。对于HS，需要配置的间隔长度为125us，对于FS/LS需要配置的间隔长度为1ms。该机存器在初始化时配置。 */
        unsigned int  hfirrldctrl : 1;  /* bit[16]   : 重新加载计数器控制器。该寄存器在初始化时配置。
                                                       0：FrInt计数器不可自动加载；
                                                       1：FrInt计数器可自动加载。 */
        unsigned int  reserved    : 15; /* bit[17-31]: 保留。 */
    } reg;
} SOC_USBOTG_HFIR_UNION;
#endif
#define SOC_USBOTG_HFIR_frint_START        (0)
#define SOC_USBOTG_HFIR_frint_END          (15)
#define SOC_USBOTG_HFIR_hfirrldctrl_START  (16)
#define SOC_USBOTG_HFIR_hfirrldctrl_END    (16)


/*****************************************************************************
 结构名    : SOC_USBOTG_HFNUM_UNION
 结构说明  : HFNUM 寄存器结构定义。地址偏移量:0x0408，初值:0x00003FFF，宽度:32
 寄存器说明: Host帧序号寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  frnum : 16; /* bit[0-15] : 帧序号。 */
        unsigned int  frrem : 16; /* bit[16-31]: 帧剩余时间记数器。从FrInt开始减计数，一直到0重新开始计数。 */
    } reg;
} SOC_USBOTG_HFNUM_UNION;
#endif
#define SOC_USBOTG_HFNUM_frnum_START  (0)
#define SOC_USBOTG_HFNUM_frnum_END    (15)
#define SOC_USBOTG_HFNUM_frrem_START  (16)
#define SOC_USBOTG_HFNUM_frrem_END    (31)


/*****************************************************************************
 结构名    : SOC_USBOTG_HPTXSTS_UNION
 结构说明  : HPTXSTS 寄存器结构定义。地址偏移量:0x0410，初值:0x00081000，宽度:32
 寄存器说明: Host周期发送FIFO和队列状态寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ptxspcavail  : 16; /* bit[0-15] : 周期发送FIFO剩余空间，以4字节为单位。 */
        unsigned int  ptxqspcavail : 8;  /* bit[16-23]: 周期发送请求队列剩余空间。
                                                        周期发送请求队列深度为8，该域指示发送请求队列还可以缓存几个发送请求。 */
        unsigned int  ptxqtop      : 8;  /* bit[24-31]: 周期发送请求队列当前请求。用于debug。 */
    } reg;
} SOC_USBOTG_HPTXSTS_UNION;
#endif
#define SOC_USBOTG_HPTXSTS_ptxspcavail_START   (0)
#define SOC_USBOTG_HPTXSTS_ptxspcavail_END     (15)
#define SOC_USBOTG_HPTXSTS_ptxqspcavail_START  (16)
#define SOC_USBOTG_HPTXSTS_ptxqspcavail_END    (23)
#define SOC_USBOTG_HPTXSTS_ptxqtop_START       (24)
#define SOC_USBOTG_HPTXSTS_ptxqtop_END         (31)


/*****************************************************************************
 结构名    : SOC_USBOTG_HAINT_UNION
 结构说明  : HAINT 寄存器结构定义。地址偏移量:0x0414，初值:0x00000000，宽度:32
 寄存器说明: Host所有通道中断状态寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  haint    : 16; /* bit[0-15] : 通道中断状态。
                                                    Bit0对应通道0，bit15对应通道15。
                                                    0：无中断；
                                                    1：有中断。 */
        unsigned int  reserved : 16; /* bit[16-31]: 保留。 */
    } reg;
} SOC_USBOTG_HAINT_UNION;
#endif
#define SOC_USBOTG_HAINT_haint_START     (0)
#define SOC_USBOTG_HAINT_haint_END       (15)


/*****************************************************************************
 结构名    : SOC_USBOTG_HAINTMSK_UNION
 结构说明  : HAINTMSK 寄存器结构定义。地址偏移量:0x0418，初值:0x00000000，宽度:32
 寄存器说明: Host所用通道中断屏蔽寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  haintmsk : 16; /* bit[0-15] : 通道中断屏蔽。
                                                    Bit0对应通道0，bit15对应通道15。
                                                    0：屏蔽。
                                                    1：不屏蔽。 */
        unsigned int  reserved : 16; /* bit[16-31]: 保留。 */
    } reg;
} SOC_USBOTG_HAINTMSK_UNION;
#endif
#define SOC_USBOTG_HAINTMSK_haintmsk_START  (0)
#define SOC_USBOTG_HAINTMSK_haintmsk_END    (15)


/*****************************************************************************
 结构名    : SOC_USBOTG_HFLBADDR_UNION
 结构说明  : HFLBADDR 寄存器结构定义。地址偏移量:0x041C，初值:0x00000000，宽度:32
 寄存器说明: 帧列表基地址寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  hflbaddr : 32; /* bit[0-31]: 帧列表基地址，对scatter/gather DMA模式有效。 */
    } reg;
} SOC_USBOTG_HFLBADDR_UNION;
#endif
#define SOC_USBOTG_HFLBADDR_hflbaddr_START  (0)
#define SOC_USBOTG_HFLBADDR_hflbaddr_END    (31)


/*****************************************************************************
 结构名    : SOC_USBOTG_HPRT_UNION
 结构说明  : HPRT 寄存器结构定义。地址偏移量:0x0440，初值:0x00000000，宽度:32
 寄存器说明: Host端口控制和状态寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  prtconnsts     : 1;  /* bit[0]    : 端口连接状态。
                                                          0：没有devie连接到端口；
                                                          1：有device连接到端口。 */
        unsigned int  prtconndet     : 1;  /* bit[1]    : 检测到端口连接中断。软件必须写1清除该bit。
                                                          0：无中断；
                                                          1：有中断。 */
        unsigned int  prtena         : 1;  /* bit[2]    : 端口使能。
                                                          复位完成之后，端口被控制器使能。过流，断开连接或者软件可能清除该bit。
                                                          0：不使能；
                                                          1：使能。 */
        unsigned int  prtenchng      : 1;  /* bit[3]    : 端口使能状态变化。
                                                          0：无变化；
                                                          1：有变化。 */
        unsigned int  prtovrcurract  : 1;  /* bit[4]    : 端口过流状态指示。
                                                          0：不过流；
                                                          1：过流。 */
        unsigned int  prtovrcurrchng : 1;  /* bit[5]    : 过流状态变化。指示PrtOvrCurrAct状态发生变化。
                                                          0：无变化；
                                                          1：有变化。 */
        unsigned int  prtres         : 1;  /* bit[6]    : 端口resume控制。控制在端口发出resume信号。
                                                          0：不resumne；
                                                          1：resume。 */
        unsigned int  prtsusp        : 1;  /* bit[7]    : 端口suspend控制。控制端口进入suspend状态。
                                                          0：不处于suspend状态；
                                                          1：处于suspend状态。 */
        unsigned int  prtrst         : 1;  /* bit[8]    : 端口复位控制。
                                                          设置该bit，控制器在端口上发出复位序列。软件必须计算复位时间的长度，并在复位完成之后清除该bit。对于HighSpeed设备，复位必须保持50ms，对于FullSpeed和LowSpeed设备，复位必须保持10ms。
                                                          0：不复位；
                                                          1：复位。 */
        unsigned int  reserved_0     : 1;  /* bit[9]    : 保留。 */
        unsigned int  prtlnsts       : 2;  /* bit[10-11]: USB总线状态。
                                                          bit[10]：D+的逻辑电平；
                                                          bit[11]：D-的逻辑电平。 */
        unsigned int  prtpwr         : 1;  /* bit[12]   : 端口功耗控制。出现过流的情况下，控制器自动清除该bit。
                                                          0：关断电源；
                                                          1：打开电源。 */
        unsigned int  prttstctl      : 4;  /* bit[13-16]: 端口测试模式控制。
                                                          0000：测试模式不使能；
                                                          0001：Test_J模式；
                                                          0010：Test_K模式；
                                                          0011：Test_SE0_NAK模式；
                                                          0100：Test_Packet模式；
                                                          0101：Test_Force_Enable；
                                                          others：reserved。 */
        unsigned int  prtspd         : 2;  /* bit[17-18]: 端口速度。
                                                          指示连接在该端口上的device速度。
                                                          00：High Speed；
                                                          01：Full Speed；
                                                          10：Low Speed；
                                                          11：reserved。 */
        unsigned int  reserved_1     : 13; /* bit[19-31]: 保留。 */
    } reg;
} SOC_USBOTG_HPRT_UNION;
#endif
#define SOC_USBOTG_HPRT_prtconnsts_START      (0)
#define SOC_USBOTG_HPRT_prtconnsts_END        (0)
#define SOC_USBOTG_HPRT_prtconndet_START      (1)
#define SOC_USBOTG_HPRT_prtconndet_END        (1)
#define SOC_USBOTG_HPRT_prtena_START          (2)
#define SOC_USBOTG_HPRT_prtena_END            (2)
#define SOC_USBOTG_HPRT_prtenchng_START       (3)
#define SOC_USBOTG_HPRT_prtenchng_END         (3)
#define SOC_USBOTG_HPRT_prtovrcurract_START   (4)
#define SOC_USBOTG_HPRT_prtovrcurract_END     (4)
#define SOC_USBOTG_HPRT_prtovrcurrchng_START  (5)
#define SOC_USBOTG_HPRT_prtovrcurrchng_END    (5)
#define SOC_USBOTG_HPRT_prtres_START          (6)
#define SOC_USBOTG_HPRT_prtres_END            (6)
#define SOC_USBOTG_HPRT_prtsusp_START         (7)
#define SOC_USBOTG_HPRT_prtsusp_END           (7)
#define SOC_USBOTG_HPRT_prtrst_START          (8)
#define SOC_USBOTG_HPRT_prtrst_END            (8)
#define SOC_USBOTG_HPRT_prtlnsts_START        (10)
#define SOC_USBOTG_HPRT_prtlnsts_END          (11)
#define SOC_USBOTG_HPRT_prtpwr_START          (12)
#define SOC_USBOTG_HPRT_prtpwr_END            (12)
#define SOC_USBOTG_HPRT_prttstctl_START       (13)
#define SOC_USBOTG_HPRT_prttstctl_END         (16)
#define SOC_USBOTG_HPRT_prtspd_START          (17)
#define SOC_USBOTG_HPRT_prtspd_END            (18)


/*****************************************************************************
 结构名    : SOC_USBOTG_HCCHARP_UNION
 结构说明  : HCCHARP 寄存器结构定义。地址偏移量:0x0500+(0x20*i)，初值:0x00000000，宽度:32
 寄存器说明: 通道p特性寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  mps      : 11; /* bit[0-10] : 最大包长。以字节为单位。 */
        unsigned int  epnum    : 4;  /* bit[11-14]: 端点号。 */
        unsigned int  epdir    : 1;  /* bit[15]   : 端点方向。
                                                    0：OUT；
                                                    1：IN。 */
        unsigned int  reserved : 1;  /* bit[16]   : 保留。 */
        unsigned int  lspddev  : 1;  /* bit[17]   : LowSpeed设备指示。
                                                    软件设置，指示该通道与LowSpeed设备通信。
                                                    0：不是LowSpeed设备；
                                                    1：是LowSpeed设备。 */
        unsigned int  eptype   : 2;  /* bit[18-19]: 端点类型。
                                                    00：control；
                                                    01：isochronous；
                                                    10：bulk；
                                                    11：interrupt。 */
        unsigned int  mc_ec    : 2;  /* bit[20-21]: 当split操作使能，对于周期传输，控制每个微帧的事务个数，对于非周期传输，控制内部DMA重新仲裁之前该通道传输的包个数。
                                                    00：保留；
                                                    01：1；
                                                    10：2；
                                                    11：3。
                                                    该域必须至少配置为2'b01。 */
        unsigned int  devaddr  : 7;  /* bit[22-28]: Device地址。 */
        unsigned int  oddfrm   : 1;  /* bit[29]   : 奇偶帧控制。
                                                    软件设置该bit，指示Host在奇数帧或者偶数帧发起周期传输。
                                                    0：偶数帧；
                                                    1：奇数帧。 */
        unsigned int  chdis    : 1;  /* bit[30]   : 通道不使能。
                                                    软件设置该bit停止发送或者接收数据。 */
        unsigned int  chena    : 1;  /* bit[31]   : 通道使能。
                                                    当Scatter/Gather DMA使能时：
                                                    0：指示描述符还没有准备好；
                                                    1：指示描述符和数据buffer准备好，通道可以读取描述符。
                                                    当Scatter/Gather DMA不使能时：
                                                    0：通道不使能；
                                                    1：通道使能。 */
    } reg;
} SOC_USBOTG_HCCHARP_UNION;
#endif
#define SOC_USBOTG_HCCHARP_mps_START       (0)
#define SOC_USBOTG_HCCHARP_mps_END         (10)
#define SOC_USBOTG_HCCHARP_epnum_START     (11)
#define SOC_USBOTG_HCCHARP_epnum_END       (14)
#define SOC_USBOTG_HCCHARP_epdir_START     (15)
#define SOC_USBOTG_HCCHARP_epdir_END       (15)
#define SOC_USBOTG_HCCHARP_lspddev_START   (17)
#define SOC_USBOTG_HCCHARP_lspddev_END     (17)
#define SOC_USBOTG_HCCHARP_eptype_START    (18)
#define SOC_USBOTG_HCCHARP_eptype_END      (19)
#define SOC_USBOTG_HCCHARP_mc_ec_START     (20)
#define SOC_USBOTG_HCCHARP_mc_ec_END       (21)
#define SOC_USBOTG_HCCHARP_devaddr_START   (22)
#define SOC_USBOTG_HCCHARP_devaddr_END     (28)
#define SOC_USBOTG_HCCHARP_oddfrm_START    (29)
#define SOC_USBOTG_HCCHARP_oddfrm_END      (29)
#define SOC_USBOTG_HCCHARP_chdis_START     (30)
#define SOC_USBOTG_HCCHARP_chdis_END       (30)
#define SOC_USBOTG_HCCHARP_chena_START     (31)
#define SOC_USBOTG_HCCHARP_chena_END       (31)


/*****************************************************************************
 结构名    : SOC_USBOTG_HCSPLTP_UNION
 结构说明  : HCSPLTP 寄存器结构定义。地址偏移量:0x0504+(0x20*i)，初值:0x00000000，宽度:32
 寄存器说明: 通道p Split控制寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  prtaddr  : 7;  /* bit[0-6]  : 端口地址。 */
        unsigned int  hubaddr  : 7;  /* bit[7-13] : Hub地址。 */
        unsigned int  xactpos  : 2;  /* bit[14-15]: 传输位置。
                                                    配置OUT传输是否发送all，first，middle或者last payload。
                                                    11：All。传输所有的payload（小于或等于188字节）；
                                                    10：Begin。传输First payload（大于188字节）；
                                                    01：End。传输last payload（大于188字节）；
                                                    00：Mid。传输middle payload（大于188字节）。 */
        unsigned int  compsplt : 1;  /* bit[16]   : Split完成操作请求。
                                                    软件设置，请求控制器进行split完成传输。
                                                    0：无请求；
                                                    1：有请求。 */
        unsigned int  reserved : 14; /* bit[17-30]: 保留。 */
        unsigned int  spltena  : 1;  /* bit[31]   : Split使能。
                                                    使能该通道进行split传输。
                                                    0：不使能；
                                                    1：使能。 */
    } reg;
} SOC_USBOTG_HCSPLTP_UNION;
#endif
#define SOC_USBOTG_HCSPLTP_prtaddr_START   (0)
#define SOC_USBOTG_HCSPLTP_prtaddr_END     (6)
#define SOC_USBOTG_HCSPLTP_hubaddr_START   (7)
#define SOC_USBOTG_HCSPLTP_hubaddr_END     (13)
#define SOC_USBOTG_HCSPLTP_xactpos_START   (14)
#define SOC_USBOTG_HCSPLTP_xactpos_END     (15)
#define SOC_USBOTG_HCSPLTP_compsplt_START  (16)
#define SOC_USBOTG_HCSPLTP_compsplt_END    (16)
#define SOC_USBOTG_HCSPLTP_spltena_START   (31)
#define SOC_USBOTG_HCSPLTP_spltena_END     (31)


/*****************************************************************************
 结构名    : SOC_USBOTG_HCINTP_UNION
 结构说明  : HCINTP 寄存器结构定义。地址偏移量:0x0508+(0x20*i)，初值:0x00000000，宽度:32
 寄存器说明: 通道p中断状态寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  xfercompl         : 1;  /* bit[0]    : 传输结束中断。
                                                             对于scatter/gather DMA模式，指示处理完IOC标志有效的描述符。
                                                             对于非scatter/gather DMA模式，指示传输正常结束。
                                                             0：无中断；
                                                             1：有中断。 */
        unsigned int  chhlted           : 1;  /* bit[1]    : 通道停止中断。
                                                             对于非scatter/gather DMA模式，指示传输异常结束。
                                                             对于scatter/gather DMA模式，指示通道由于以下原因结束：描述符的EOL标志有效，AHB总线错误，验证的事务错误，软件配置通道不使能，babble，stall响应。
                                                             0：无中断；
                                                             1：有中断。 */
        unsigned int  ahberr            : 1;  /* bit[2]    : AHB总线错误中断。
                                                             0：无中断；
                                                             1：有中断。 */
        unsigned int  stall             : 1;  /* bit[3]    : 接收到STALL响应中断。
                                                             对于scatter/gather DMA模式，控制器屏蔽该中断。
                                                             0：无中断；
                                                             1：有中断。 */
        unsigned int  nak               : 1;  /* bit[4]    : 接收到NAK响应中断。
                                                             对于scatter/gather DMA模式，控制器屏蔽该中断。
                                                             0：无中断；
                                                             1：有中断。 */
        unsigned int  ack               : 1;  /* bit[5]    : 接收或者发送ACK响应中断。
                                                             对于scatter/gather DMA模式，控制器屏蔽该中断。
                                                             0：无中断；
                                                             1：有中断。 */
        unsigned int  nyet              : 1;  /* bit[6]    : 接收到NYET响应中断。
                                                             对于scatter/gather DMA模式，控制器屏蔽该中断。
                                                             0：无中断；
                                                             1：有中断。 */
        unsigned int  xacterr           : 1;  /* bit[7]    : 事务错误中断。
                                                             指示发生以下几种错误情况之一：
                                                             CRC校验错误，超时，位填充错误，虚假的EOP。
                                                             对于scatter/gather DMA模式，控制器屏蔽该中断。
                                                             0：无中断；
                                                             1：有中断。 */
        unsigned int  bblerr            : 1;  /* bit[8]    : Babble错误中断。
                                                             对于scatter/gather DMA模式，控制器屏蔽该中断。
                                                             0：无中断；
                                                             1：有中断。 */
        unsigned int  frmovrun          : 1;  /* bit[9]    : 帧计数翻转中断。
                                                             对于scatter/gather DMA模式，控制器屏蔽该中断。
                                                             0：无中断；
                                                             1：有中断。 */
        unsigned int  datatglerr        : 1;  /* bit[10]   : 数据PID翻转错误中断。
                                                             对于scatter/gather DMA模式，控制器屏蔽该中断。
                                                             0：无中断；
                                                             1：有中断。 */
        unsigned int  bnaintr           : 1;  /* bit[11]   : buffer无效中断。
                                                             对scatter/gather DMA模式有效。
                                                             0：无中断；
                                                             1：有中断。 */
        unsigned int  xcs_xact_err      : 1;  /* bit[12]   : 严重的事务错误中断。
                                                             对scatter/gather DMA模式有效。指示连续3个事务发生错误。
                                                             0：无中断；
                                                             1：有中断。 */
        unsigned int  desc_lst_rollintr : 1;  /* bit[13]   : 描述符翻转中断。
                                                             对scatter/gather DMA模式有效。
                                                             0：无中断；
                                                             1：有中断。 */
        unsigned int  reserved          : 18; /* bit[14-31]: 保留。 */
    } reg;
} SOC_USBOTG_HCINTP_UNION;
#endif
#define SOC_USBOTG_HCINTP_xfercompl_START          (0)
#define SOC_USBOTG_HCINTP_xfercompl_END            (0)
#define SOC_USBOTG_HCINTP_chhlted_START            (1)
#define SOC_USBOTG_HCINTP_chhlted_END              (1)
#define SOC_USBOTG_HCINTP_ahberr_START             (2)
#define SOC_USBOTG_HCINTP_ahberr_END               (2)
#define SOC_USBOTG_HCINTP_stall_START              (3)
#define SOC_USBOTG_HCINTP_stall_END                (3)
#define SOC_USBOTG_HCINTP_nak_START                (4)
#define SOC_USBOTG_HCINTP_nak_END                  (4)
#define SOC_USBOTG_HCINTP_ack_START                (5)
#define SOC_USBOTG_HCINTP_ack_END                  (5)
#define SOC_USBOTG_HCINTP_nyet_START               (6)
#define SOC_USBOTG_HCINTP_nyet_END                 (6)
#define SOC_USBOTG_HCINTP_xacterr_START            (7)
#define SOC_USBOTG_HCINTP_xacterr_END              (7)
#define SOC_USBOTG_HCINTP_bblerr_START             (8)
#define SOC_USBOTG_HCINTP_bblerr_END               (8)
#define SOC_USBOTG_HCINTP_frmovrun_START           (9)
#define SOC_USBOTG_HCINTP_frmovrun_END             (9)
#define SOC_USBOTG_HCINTP_datatglerr_START         (10)
#define SOC_USBOTG_HCINTP_datatglerr_END           (10)
#define SOC_USBOTG_HCINTP_bnaintr_START            (11)
#define SOC_USBOTG_HCINTP_bnaintr_END              (11)
#define SOC_USBOTG_HCINTP_xcs_xact_err_START       (12)
#define SOC_USBOTG_HCINTP_xcs_xact_err_END         (12)
#define SOC_USBOTG_HCINTP_desc_lst_rollintr_START  (13)
#define SOC_USBOTG_HCINTP_desc_lst_rollintr_END    (13)


/*****************************************************************************
 结构名    : SOC_USBOTG_HCINTMSKP_UNION
 结构说明  : HCINTMSKP 寄存器结构定义。地址偏移量:0x050C+(0x20*i)，初值:0x00000000，宽度:32
 寄存器说明: 通道p中断屏蔽寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  xfercompl         : 1;  /* bit[0]    : 传输结束中断屏蔽位。
                                                             0：屏蔽；
                                                             1：不屏蔽。 */
        unsigned int  chhlted           : 1;  /* bit[1]    : 通道停止中断屏蔽位。
                                                             0：屏蔽；
                                                             1：不屏蔽。 */
        unsigned int  ahberr            : 1;  /* bit[2]    : AHB总线错误中断屏蔽位。
                                                             0：屏蔽；
                                                             1：不屏蔽。 */
        unsigned int  stall             : 1;  /* bit[3]    : 接收到STALL响应中断屏蔽位。
                                                             0：屏蔽；
                                                             1：不屏蔽。 */
        unsigned int  nak               : 1;  /* bit[4]    : 接收到NAK响应中断屏蔽位。
                                                             0：屏蔽；
                                                             1：不屏蔽。 */
        unsigned int  ack               : 1;  /* bit[5]    : 接收或者发送ACK响应中断屏蔽位。
                                                             0：屏蔽；
                                                             1：不屏蔽。 */
        unsigned int  nyet              : 1;  /* bit[6]    : 接收到NYET响应中断屏蔽位。
                                                             0：屏蔽；
                                                             1：不屏蔽。 */
        unsigned int  xacterr           : 1;  /* bit[7]    : 事务错误中断屏蔽位。
                                                             0：屏蔽；
                                                             1：不屏蔽。 */
        unsigned int  bblerr            : 1;  /* bit[8]    : Babble错误中断屏蔽位。
                                                             0：屏蔽；
                                                             1：不屏蔽。 */
        unsigned int  frmovrun          : 1;  /* bit[9]    : 帧计数翻转中断屏蔽位。
                                                             0：屏蔽；
                                                             1：不屏蔽。 */
        unsigned int  datatglerr        : 1;  /* bit[10]   : 数据PID翻转错误中断屏蔽位。
                                                             0：屏蔽；
                                                             1：不屏蔽。 */
        unsigned int  bnaintr           : 1;  /* bit[11]   : buffer无效中断屏蔽位。
                                                             0：屏蔽；
                                                             1：不屏蔽。 */
        unsigned int  xcs_xact_err      : 1;  /* bit[12]   : 严重的事务错误中断屏蔽位。
                                                             0：屏蔽；
                                                             1：不屏蔽。 */
        unsigned int  desc_lst_rollintr : 1;  /* bit[13]   : 描述符翻转中断屏蔽位。
                                                             0：屏蔽；
                                                             1：不屏蔽。 */
        unsigned int  reserved          : 18; /* bit[14-31]: 保留。 */
    } reg;
} SOC_USBOTG_HCINTMSKP_UNION;
#endif
#define SOC_USBOTG_HCINTMSKP_xfercompl_START          (0)
#define SOC_USBOTG_HCINTMSKP_xfercompl_END            (0)
#define SOC_USBOTG_HCINTMSKP_chhlted_START            (1)
#define SOC_USBOTG_HCINTMSKP_chhlted_END              (1)
#define SOC_USBOTG_HCINTMSKP_ahberr_START             (2)
#define SOC_USBOTG_HCINTMSKP_ahberr_END               (2)
#define SOC_USBOTG_HCINTMSKP_stall_START              (3)
#define SOC_USBOTG_HCINTMSKP_stall_END                (3)
#define SOC_USBOTG_HCINTMSKP_nak_START                (4)
#define SOC_USBOTG_HCINTMSKP_nak_END                  (4)
#define SOC_USBOTG_HCINTMSKP_ack_START                (5)
#define SOC_USBOTG_HCINTMSKP_ack_END                  (5)
#define SOC_USBOTG_HCINTMSKP_nyet_START               (6)
#define SOC_USBOTG_HCINTMSKP_nyet_END                 (6)
#define SOC_USBOTG_HCINTMSKP_xacterr_START            (7)
#define SOC_USBOTG_HCINTMSKP_xacterr_END              (7)
#define SOC_USBOTG_HCINTMSKP_bblerr_START             (8)
#define SOC_USBOTG_HCINTMSKP_bblerr_END               (8)
#define SOC_USBOTG_HCINTMSKP_frmovrun_START           (9)
#define SOC_USBOTG_HCINTMSKP_frmovrun_END             (9)
#define SOC_USBOTG_HCINTMSKP_datatglerr_START         (10)
#define SOC_USBOTG_HCINTMSKP_datatglerr_END           (10)
#define SOC_USBOTG_HCINTMSKP_bnaintr_START            (11)
#define SOC_USBOTG_HCINTMSKP_bnaintr_END              (11)
#define SOC_USBOTG_HCINTMSKP_xcs_xact_err_START       (12)
#define SOC_USBOTG_HCINTMSKP_xcs_xact_err_END         (12)
#define SOC_USBOTG_HCINTMSKP_desc_lst_rollintr_START  (13)
#define SOC_USBOTG_HCINTMSKP_desc_lst_rollintr_END    (13)


/*****************************************************************************
 结构名    : SOC_USBOTG_HCTSIZP_UNION
 结构说明  : HCTSIZP 寄存器结构定义。地址偏移量:0x0510+(0x20*i)，初值:0x00000000，宽度:32
 寄存器说明: 通道p传输长度寄存器。（scatter / gather DMA模式）
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sched_info : 8;  /* bit[0-7]  : 调度信息。
                                                      仅对周期传输有效。每bit指示相应的微帧调度控制。Bit[0]对应第1个微帧，bit[7]对应第8个微帧。
                                                      8'b11111111指示在该帧内，该通道每个微帧调度一次，发起相应的令牌包。8'b10101010指示隔一个微帧调度一次。 */
        unsigned int  ntd        : 8;  /* bit[8-15] : 对于scatter/gather DMA模式，指示描述符个数。 */
        unsigned int  reserved   : 13; /* bit[16-28]: 保留。 */
        unsigned int  pid        : 2;  /* bit[29-30]: PID设置。
                                                      设置起始传输的PID。控制器维护传输中其它PID。
                                                      00：DATA0；
                                                      01：DATA2；
                                                      10：DATA1；
                                                      11：MDATA。 */
        unsigned int  dopng      : 1;  /* bit[31]   : PING操作设置。
                                                      该Bit仅对OUT传输有效。设置该bit，host按照协议进行PING操作。
                                                      对IN传输设置该bit，会导致通道不使能。 */
    } reg;
} SOC_USBOTG_HCTSIZP_UNION;
#endif
#define SOC_USBOTG_HCTSIZP_sched_info_START  (0)
#define SOC_USBOTG_HCTSIZP_sched_info_END    (7)
#define SOC_USBOTG_HCTSIZP_ntd_START         (8)
#define SOC_USBOTG_HCTSIZP_ntd_END           (15)
#define SOC_USBOTG_HCTSIZP_pid_START         (29)
#define SOC_USBOTG_HCTSIZP_pid_END           (30)
#define SOC_USBOTG_HCTSIZP_dopng_START       (31)
#define SOC_USBOTG_HCTSIZP_dopng_END         (31)


/*****************************************************************************
 结构名    : SOC_USBOTG_HCTSIZP_NON_SG_UNION
 结构说明  : HCTSIZP_NON_SG 寄存器结构定义。地址偏移量:0x0510+(0x20*i)，初值:0x00000000，宽度:32
 寄存器说明: 通道p传输长度寄存器。（非scatter / gather DMA模式）
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  xfersize : 19; /* bit[0-18] : 传输大小，以字节为单位。
                                                    对于OUT传输，表示Host在这次传输中要传输的字节数。
                                                    对于IN传输，表示这个传输中剩余的字节数。软件需要将该域配置为最大保护包长的整数倍。 */
        unsigned int  pktcnt   : 10; /* bit[19-28]: 包个数配置。 */
        unsigned int  pid      : 2;  /* bit[29-30]: PID设置。
                                                    设置起始传输的PID。控制器维护传输中其它PID。
                                                    00：DATA0；
                                                    01：DATA2；
                                                    10：DATA1；
                                                    11：MDATA（非控制端点）/setup（控制端点）。 */
        unsigned int  dopng    : 1;  /* bit[31]   : PING操作设置。
                                                    该Bit仅对OUT传输有效。设置该bit，host按照协议进行PING操作。
                                                    对IN传输设置该bit，会导致通道不使能。 */
    } reg;
} SOC_USBOTG_HCTSIZP_NON_SG_UNION;
#endif
#define SOC_USBOTG_HCTSIZP_NON_SG_xfersize_START  (0)
#define SOC_USBOTG_HCTSIZP_NON_SG_xfersize_END    (18)
#define SOC_USBOTG_HCTSIZP_NON_SG_pktcnt_START    (19)
#define SOC_USBOTG_HCTSIZP_NON_SG_pktcnt_END      (28)
#define SOC_USBOTG_HCTSIZP_NON_SG_pid_START       (29)
#define SOC_USBOTG_HCTSIZP_NON_SG_pid_END         (30)
#define SOC_USBOTG_HCTSIZP_NON_SG_dopng_START     (31)
#define SOC_USBOTG_HCTSIZP_NON_SG_dopng_END       (31)


/*****************************************************************************
 结构名    : SOC_USBOTG_HCDMAP_UNION
 结构说明  : HCDMAP 寄存器结构定义。地址偏移量:0x0514+(0x20*i)，初值:0x00000000，宽度:32
 寄存器说明: 通道p DMA地址寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  dmaaddr : 32; /* bit[0-31]: DMA起始地址。 */
    } reg;
} SOC_USBOTG_HCDMAP_UNION;
#endif
#define SOC_USBOTG_HCDMAP_dmaaddr_START  (0)
#define SOC_USBOTG_HCDMAP_dmaaddr_END    (31)


/*****************************************************************************
 结构名    : SOC_USBOTG_HCDMABP_UNION
 结构说明  : HCDMABP 寄存器结构定义。地址偏移量:0x051C+(0x20*i)，初值:0x00000000，宽度:32
 寄存器说明: 通道p DMA buffer地址寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  hcdma : 32; /* bit[0-31]: 当前buffer地址。对scatter/gather DMA模式有效。 */
    } reg;
} SOC_USBOTG_HCDMABP_UNION;
#endif
#define SOC_USBOTG_HCDMABP_hcdma_START  (0)
#define SOC_USBOTG_HCDMABP_hcdma_END    (31)


/*****************************************************************************
 结构名    : SOC_USBOTG_DCFG_UNION
 结构说明  : DCFG 寄存器结构定义。地址偏移量:0x0800，初值:0x08200000，宽度:32
 寄存器说明: device配置寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  devspd       : 2;  /* bit[0-1]  : Device速度模式配置。
                                                        00：High-speed；
                                                        01：Full-speed；
                                                        others：reserved。 */
        unsigned int  nzstsouthshk : 1;  /* bit[2]    : 非0长度的OUT握手策略选择。在控制传输的状态阶段，当收到非0长度的数据包时，需要返回1个握手包。配置该bit选择送出什么握手包。
                                                        0：将收到的数据包送给软件，并按照相应端点控制寄存器的NAK和STALL位送出握手包；
                                                        1：送出STALL握手包，并丢弃收到的数据包。 */
        unsigned int  reserved_0   : 1;  /* bit[3]    : 保留。 */
        unsigned int  devaddr      : 7;  /* bit[4-10] : device地址。每次收到SetAddr命令后，软件必须将device地址写入该域。 */
        unsigned int  perfrint     : 2;  /* bit[11-12]: 周期帧间隔。在1个微帧内，当周期帧间隔达到该域的值，上报周期帧结束中断。
                                                        00：微帧长度的80％；
                                                        01：微帧长度的85％；
                                                        10：微帧长度的90％；
                                                        11：微帧长度的95％。 */
        unsigned int  reserved_1   : 10; /* bit[13-22]: 保留。 */
        unsigned int  descdma      : 1;  /* bit[23]   : Scatter/Gather DMA使能。
                                                        0：不使能；
                                                        1：使能。 */
        unsigned int  perschintvl  : 2;  /* bit[24-25]: 周期调度间隔。配置DMA分配给搬移周期IN端点数据的时间。如果有周期端点被激活，DMA必须分配该域配置的时间用于搬移非周期IN端点数据；否则，DMA为非周期端点服务。该域配置的时间之后，DMA开始搬移非周期端点数据。
                                                        00：微帧长度的25%；
                                                        01：微帧长度的50%；
                                                        10：微帧长度的75%；
                                                        11：保留。 */
        unsigned int  reserved_2   : 6;  /* bit[26-31]: 保留。 */
    } reg;
} SOC_USBOTG_DCFG_UNION;
#endif
#define SOC_USBOTG_DCFG_devspd_START        (0)
#define SOC_USBOTG_DCFG_devspd_END          (1)
#define SOC_USBOTG_DCFG_nzstsouthshk_START  (2)
#define SOC_USBOTG_DCFG_nzstsouthshk_END    (2)
#define SOC_USBOTG_DCFG_devaddr_START       (4)
#define SOC_USBOTG_DCFG_devaddr_END         (10)
#define SOC_USBOTG_DCFG_perfrint_START      (11)
#define SOC_USBOTG_DCFG_perfrint_END        (12)
#define SOC_USBOTG_DCFG_descdma_START       (23)
#define SOC_USBOTG_DCFG_descdma_END         (23)
#define SOC_USBOTG_DCFG_perschintvl_START   (24)
#define SOC_USBOTG_DCFG_perschintvl_END     (25)


/*****************************************************************************
 结构名    : SOC_USBOTG_DCTL_UNION
 结构说明  : DCTL 寄存器结构定义。地址偏移量:0x0804，初值:0x00000000，宽度:32
 寄存器说明: device控制寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  rmtwkupsig   : 1;  /* bit[0]    : 远程唤醒控制。
                                                        配置该bit，控制器远程唤醒host。软件通过配置该bit，使device退出suspend状态。根据协议，软件必须在配置该bit之后，1～15ms内清除该bit。
                                                        当LPM使能且device处于L1(sleep)状态，软件通过设置该bit唤醒host，并使device退出L1状态。硬件在50us之后自动清除该bit。
                                                        0：不进行远程唤醒操作；
                                                        1：进行远程唤醒操作。 */
        unsigned int  sftdiscon    : 1;  /* bit[1]    : 软件断开连接控制。
                                                        0：正常操作；
                                                        1：控制器驱动UTMI+接口的opmode信号为2'b01，在USB host端产生device断开连接事件。 */
        unsigned int  gnpinnaksts  : 1;  /* bit[2]    : 全局非周期IN传输NAK状态。
                                                        0：根据发送FIFO中数据发送握手包；
                                                        1：发送NAK。 */
        unsigned int  goutnaksts   : 1;  /* bit[3]    : 全局非周期OUT传输NAK状态。
                                                        0：握手包已经发送；
                                                        1：发送NAK，并忽略接收到的数据。 */
        unsigned int  tstctl       : 3;  /* bit[4-6]  : 测试模式控制。
                                                        000：测试模式不使能；
                                                        001：测试J模式；
                                                        010：测试K模式；
                                                        011：测试SE0_NAK模式；
                                                        100：测试包模式；
                                                        101：测试Force_Enable模式；
                                                        others：保留。 */
        unsigned int  sgnpinnak    : 1;  /* bit[7]    : 全局非周期IN传输NAK设置。软件对该bit写“1”后，device对所有的非周期IN传输响应NAK。软件必须在GINTSTS[GINNakEff]清除后才能配置该bit。
                                                        0：不设置NAK；
                                                        1：设置NAK。 */
        unsigned int  cgnpinnak    : 1;  /* bit[8]    : 清除全局非周期IN传输NAK设置。对该bit的写“1”清除全局IN NAK。
                                                        0：不清除；
                                                        1：清除。 */
        unsigned int  sgoutnak     : 1;  /* bit[9]    : 全局非周期OUT传输NAK设置。软件对该bit写“1”后，device对所有的非周期OUT传输响应NAK。软件必须在GINTSTS[GOUTNakEff]清除后才能配置该bit。
                                                        0：不设置NAK；
                                                        1：设置NAK。 */
        unsigned int  cgoutnak     : 1;  /* bit[10]   : 清除全局非周期IN传输NAK设置。对该bit的写“1”清除全局OUT NAK。
                                                        0：不清除；
                                                        1：清除。 */
        unsigned int  pwronprgdone : 1;  /* bit[11]   : 上电编程完毕。软件配置该bit，指示退出Power Down模式后，软件配置寄存器完毕。
                                                        0：未完成；
                                                        1：完成。 */
        unsigned int  reserved_0   : 1;  /* bit[12]   : 保留。 */
        unsigned int  gmc          : 2;  /* bit[13-14]: 在Scatter/Gather DMA模式下，配置DMA服务下1个端点之前，服务本端点的包数量，仅针对非周期端点。
                                                        00：reserved；
                                                        01：1个包；
                                                        10：2个包；
                                                        11：3个包。 */
        unsigned int  ignrfrmnum   : 1;  /* bit[15]   : 在Scatter/Gather DMA模式下，忽略同步端点的帧序号。该bit使能，描述符中的帧序号比当前帧序号小，也不清除发送FIFO中的数据。
                                                        0：不使能；
                                                        1：使能。 */
        unsigned int  nakonbble    : 1;  /* bit[16]   : 设置收到babble之后自动回复NAK。
                                                        0：不使能；
                                                        1：使能。 */
        unsigned int  reserved_1   : 15; /* bit[17-31]: 保留。 */
    } reg;
} SOC_USBOTG_DCTL_UNION;
#endif
#define SOC_USBOTG_DCTL_rmtwkupsig_START    (0)
#define SOC_USBOTG_DCTL_rmtwkupsig_END      (0)
#define SOC_USBOTG_DCTL_sftdiscon_START     (1)
#define SOC_USBOTG_DCTL_sftdiscon_END       (1)
#define SOC_USBOTG_DCTL_gnpinnaksts_START   (2)
#define SOC_USBOTG_DCTL_gnpinnaksts_END     (2)
#define SOC_USBOTG_DCTL_goutnaksts_START    (3)
#define SOC_USBOTG_DCTL_goutnaksts_END      (3)
#define SOC_USBOTG_DCTL_tstctl_START        (4)
#define SOC_USBOTG_DCTL_tstctl_END          (6)
#define SOC_USBOTG_DCTL_sgnpinnak_START     (7)
#define SOC_USBOTG_DCTL_sgnpinnak_END       (7)
#define SOC_USBOTG_DCTL_cgnpinnak_START     (8)
#define SOC_USBOTG_DCTL_cgnpinnak_END       (8)
#define SOC_USBOTG_DCTL_sgoutnak_START      (9)
#define SOC_USBOTG_DCTL_sgoutnak_END        (9)
#define SOC_USBOTG_DCTL_cgoutnak_START      (10)
#define SOC_USBOTG_DCTL_cgoutnak_END        (10)
#define SOC_USBOTG_DCTL_pwronprgdone_START  (11)
#define SOC_USBOTG_DCTL_pwronprgdone_END    (11)
#define SOC_USBOTG_DCTL_gmc_START           (13)
#define SOC_USBOTG_DCTL_gmc_END             (14)
#define SOC_USBOTG_DCTL_ignrfrmnum_START    (15)
#define SOC_USBOTG_DCTL_ignrfrmnum_END      (15)
#define SOC_USBOTG_DCTL_nakonbble_START     (16)
#define SOC_USBOTG_DCTL_nakonbble_END       (16)


/*****************************************************************************
 结构名    : SOC_USBOTG_DSTS_UNION
 结构说明  : DSTS 寄存器结构定义。地址偏移量:0x0808，初值:0x0007FF02，宽度:32
 寄存器说明: device状态寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  suspsts   : 1;  /* bit[0]    : suspend状态指示。
                                                     0：不处于suspend状态；
                                                     1：处于suspend状态。 */
        unsigned int  enumspd   : 2;  /* bit[1-2]  : 枚举速度。
                                                     00：高速（PHY时钟为30或60MHz）；
                                                     01：全速（PHY时钟为30或60MHz）；
                                                     others：保留。 */
        unsigned int  errticerr : 1;  /* bit[3]    : 用于指示任何UTMI+上接收到的不确定的错误。产生这种错误时，控制器进入suspend状态，并上报中断GINTSTS[ErlySusp]。
                                                     0：没有UTMI+接口错误；
                                                     1：有UTMI+接口错误。 */
        unsigned int  reserved_0: 4;  /* bit[4-7]  : 保留。 */
        unsigned int  soffn     : 14; /* bit[8-21] : 当前帧或者微帧的帧序号。 */
        unsigned int  devlnsts  : 2;  /* bit[22-23]: 只是linestatus的polarity */
        unsigned int  reserved_1: 8;  /* bit[24-31]: 保留。 */
    } reg;
} SOC_USBOTG_DSTS_UNION;
#endif
#define SOC_USBOTG_DSTS_suspsts_START    (0)
#define SOC_USBOTG_DSTS_suspsts_END      (0)
#define SOC_USBOTG_DSTS_enumspd_START    (1)
#define SOC_USBOTG_DSTS_enumspd_END      (2)
#define SOC_USBOTG_DSTS_errticerr_START  (3)
#define SOC_USBOTG_DSTS_errticerr_END    (3)
#define SOC_USBOTG_DSTS_soffn_START      (8)
#define SOC_USBOTG_DSTS_soffn_END        (21)
#define SOC_USBOTG_DSTS_devlnsts_START   (22)
#define SOC_USBOTG_DSTS_devlnsts_END     (23)


/*****************************************************************************
 结构名    : SOC_USBOTG_DIEPMSK_UNION
 结构说明  : DIEPMSK 寄存器结构定义。地址偏移量:0x0810，初值:0x00000000，宽度:32
 寄存器说明: IN端点普通中断屏蔽寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  xfercomplmsk   : 1;  /* bit[0]    : 传输完成中断屏蔽。
                                                          0：屏蔽；
                                                          1：不屏蔽。 */
        unsigned int  epdisbldmsk    : 1;  /* bit[1]    : 端点不使能中断屏蔽。
                                                          0：屏蔽；
                                                          1：不屏蔽。 */
        unsigned int  ahberrmsk      : 1;  /* bit[2]    : AHB总线错误中断屏蔽。
                                                          0：屏蔽；
                                                          1：不屏蔽。 */
        unsigned int  timeoutmsk     : 1;  /* bit[3]    : 超时中断屏蔽。
                                                          0：屏蔽；
                                                          1：不屏蔽。 */
        unsigned int  intkntxfempmsk : 1;  /* bit[4]    : 接收到IN令牌，但是发送FIFO空中断屏蔽。
                                                          0：屏蔽；
                                                          1：不屏蔽。 */
        unsigned int  reserved_0     : 1;  /* bit[5]    : 保留。 */
        unsigned int  inepnakeffmsk  : 1;  /* bit[6]    : IN端点NAK有效屏蔽。
                                                          0：屏蔽；
                                                          1：不屏蔽。 */
        unsigned int  reserved_1     : 1;  /* bit[7]    : 保留。 */
        unsigned int  txfifoundrnmsk : 1;  /* bit[8]    : 发送FIFO下溢中断屏蔽。
                                                          0：屏蔽；
                                                          1：不屏蔽。 */
        unsigned int  bnainintrmsk   : 1;  /* bit[9]    : IN端点BNA中断屏蔽。
                                                          0：屏蔽；
                                                          1：不屏蔽。 */
        unsigned int  reserved_2     : 3;  /* bit[10-12]: 保留。 */
        unsigned int  nakmsk         : 1;  /* bit[13]   : IN端点NAK中断屏蔽位。
                                                          0：屏蔽；
                                                          1：不屏蔽。 */
        unsigned int  reserved_3     : 18; /* bit[14-31]: 保留。 */
    } reg;
} SOC_USBOTG_DIEPMSK_UNION;
#endif
#define SOC_USBOTG_DIEPMSK_xfercomplmsk_START    (0)
#define SOC_USBOTG_DIEPMSK_xfercomplmsk_END      (0)
#define SOC_USBOTG_DIEPMSK_epdisbldmsk_START     (1)
#define SOC_USBOTG_DIEPMSK_epdisbldmsk_END       (1)
#define SOC_USBOTG_DIEPMSK_ahberrmsk_START       (2)
#define SOC_USBOTG_DIEPMSK_ahberrmsk_END         (2)
#define SOC_USBOTG_DIEPMSK_timeoutmsk_START      (3)
#define SOC_USBOTG_DIEPMSK_timeoutmsk_END        (3)
#define SOC_USBOTG_DIEPMSK_intkntxfempmsk_START  (4)
#define SOC_USBOTG_DIEPMSK_intkntxfempmsk_END    (4)
#define SOC_USBOTG_DIEPMSK_inepnakeffmsk_START   (6)
#define SOC_USBOTG_DIEPMSK_inepnakeffmsk_END     (6)
#define SOC_USBOTG_DIEPMSK_txfifoundrnmsk_START  (8)
#define SOC_USBOTG_DIEPMSK_txfifoundrnmsk_END    (8)
#define SOC_USBOTG_DIEPMSK_bnainintrmsk_START    (9)
#define SOC_USBOTG_DIEPMSK_bnainintrmsk_END      (9)
#define SOC_USBOTG_DIEPMSK_nakmsk_START          (13)
#define SOC_USBOTG_DIEPMSK_nakmsk_END            (13)


/*****************************************************************************
 结构名    : SOC_USBOTG_DOEPMSK_UNION
 结构说明  : DOEPMSK 寄存器结构定义。地址偏移量:0x0814，初值:0x00000000，宽度:32
 寄存器说明: OUT端点普通中断屏蔽寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  xfercomplmsk   : 1;  /* bit[0]    : 传输完成中断屏蔽。
                                                          0：屏蔽；
                                                          1：不屏蔽。 */
        unsigned int  epdisbldmsk    : 1;  /* bit[1]    : 端点不使能中断屏蔽。
                                                          0：屏蔽；
                                                          1：不屏蔽。 */
        unsigned int  ahberrmsk      : 1;  /* bit[2]    : AHB总线错误中断屏蔽。
                                                          0：屏蔽；
                                                          1：不屏蔽。 */
        unsigned int  setupmsk       : 1;  /* bit[3]    : setup阶段完成中断屏蔽。
                                                          0：屏蔽；
                                                          1：不屏蔽。 */
        unsigned int  outtknepdismsk : 1;  /* bit[4]    : 接收到OUT令牌，但端点没有使能中断屏蔽。
                                                          0：屏蔽；
                                                          1：不屏蔽。 */
        unsigned int  reserved_0     : 1;  /* bit[5]    : 保留。 */
        unsigned int  back2backsetup : 1;  /* bit[6]    : 背靠背SETUP包接收中断。
                                                          0：屏蔽；
                                                          1：不屏蔽。 */
        unsigned int  reserved_1     : 1;  /* bit[7]    : 保留。 */
        unsigned int  outpkterrmsk   : 1;  /* bit[8]    : OUT包错误中断屏蔽。
                                                          0：屏蔽；
                                                          1：不屏蔽。 */
        unsigned int  bnaoutintrmsk  : 1;  /* bit[9]    : OUT端点BNA中断屏蔽。
                                                          0：屏蔽；
                                                          1：不屏蔽。 */
        unsigned int  reserved_2     : 2;  /* bit[10-11]: 保留。 */
        unsigned int  bbleerrmsk     : 1;  /* bit[12]   : babble错误中断屏蔽。
                                                          0：屏蔽；
                                                          1：不屏蔽。 */
        unsigned int  nakmsk         : 1;  /* bit[13]   : NAK中断屏蔽位。
                                                          0：屏蔽
                                                          1：不屏蔽 */
        unsigned int  nyetmsk        : 1;  /* bit[14]   : NYET中断屏蔽。
                                                          0：屏蔽；
                                                          1：不屏蔽。 */
        unsigned int  reserved_3     : 17; /* bit[15-31]: 保留。 */
    } reg;
} SOC_USBOTG_DOEPMSK_UNION;
#endif
#define SOC_USBOTG_DOEPMSK_xfercomplmsk_START    (0)
#define SOC_USBOTG_DOEPMSK_xfercomplmsk_END      (0)
#define SOC_USBOTG_DOEPMSK_epdisbldmsk_START     (1)
#define SOC_USBOTG_DOEPMSK_epdisbldmsk_END       (1)
#define SOC_USBOTG_DOEPMSK_ahberrmsk_START       (2)
#define SOC_USBOTG_DOEPMSK_ahberrmsk_END         (2)
#define SOC_USBOTG_DOEPMSK_setupmsk_START        (3)
#define SOC_USBOTG_DOEPMSK_setupmsk_END          (3)
#define SOC_USBOTG_DOEPMSK_outtknepdismsk_START  (4)
#define SOC_USBOTG_DOEPMSK_outtknepdismsk_END    (4)
#define SOC_USBOTG_DOEPMSK_back2backsetup_START  (6)
#define SOC_USBOTG_DOEPMSK_back2backsetup_END    (6)
#define SOC_USBOTG_DOEPMSK_outpkterrmsk_START    (8)
#define SOC_USBOTG_DOEPMSK_outpkterrmsk_END      (8)
#define SOC_USBOTG_DOEPMSK_bnaoutintrmsk_START   (9)
#define SOC_USBOTG_DOEPMSK_bnaoutintrmsk_END     (9)
#define SOC_USBOTG_DOEPMSK_bbleerrmsk_START      (12)
#define SOC_USBOTG_DOEPMSK_bbleerrmsk_END        (12)
#define SOC_USBOTG_DOEPMSK_nakmsk_START          (13)
#define SOC_USBOTG_DOEPMSK_nakmsk_END            (13)
#define SOC_USBOTG_DOEPMSK_nyetmsk_START         (14)
#define SOC_USBOTG_DOEPMSK_nyetmsk_END           (14)


/*****************************************************************************
 结构名    : SOC_USBOTG_DAINT_UNION
 结构说明  : DAINT 寄存器结构定义。地址偏移量:0x0818，初值:0x00000000，宽度:32
 寄存器说明: 所有端点中断状态寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  inepint  : 16; /* bit[0-15] : IN端点中断。每bit对应1个端点，bit[0]对应端点0，bit[15]对应端点15。 */
        unsigned int  outepint : 16; /* bit[16-31]: OUT端点中断。每bit对应1个端点，bit[16对应端点0，bit[31]对应端点15。 */
    } reg;
} SOC_USBOTG_DAINT_UNION;
#endif
#define SOC_USBOTG_DAINT_inepint_START   (0)
#define SOC_USBOTG_DAINT_inepint_END     (15)
#define SOC_USBOTG_DAINT_outepint_START  (16)
#define SOC_USBOTG_DAINT_outepint_END    (31)


/*****************************************************************************
 结构名    : SOC_USBOTG_DAINTMSK_UNION
 结构说明  : DAINTMSK 寄存器结构定义。地址偏移量:0x081C，初值:0x00000000，宽度:32
 寄存器说明: 所有端点中断屏蔽寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  inepmsk  : 16; /* bit[0-15] : IN端点的中断屏蔽。每bit对应一个端点，bit[0]对应端点0，bit[15]对应端点15。
                                                    0：屏蔽；
                                                    1：不屏蔽。 */
        unsigned int  outepmsk : 16; /* bit[16-31]: OUT端点的中断屏蔽。每bit对应一个端点，bit[16]对应端点0，bit[31]对应端点15。
                                                    0：屏蔽；
                                                    1：不屏蔽。 */
    } reg;
} SOC_USBOTG_DAINTMSK_UNION;
#endif
#define SOC_USBOTG_DAINTMSK_inepmsk_START   (0)
#define SOC_USBOTG_DAINTMSK_inepmsk_END     (15)
#define SOC_USBOTG_DAINTMSK_outepmsk_START  (16)
#define SOC_USBOTG_DAINTMSK_outepmsk_END    (31)


/*****************************************************************************
 结构名    : SOC_USBOTG_DVBUSDIS_UNION
 结构说明  : DVBUSDIS 寄存器结构定义。地址偏移量:0x0828，初值:0x000017D7，宽度:32
 寄存器说明: Vbus放电时间配置寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  dvbusdis : 16; /* bit[0-15] : Vbus放电时间。配置在SRP过程中Vbus的放电时间。该域的值等于Vbus放电时间的PHY时钟周期数/1024。 */
        unsigned int  reserved : 16; /* bit[16-31]: 保留。 */
    } reg;
} SOC_USBOTG_DVBUSDIS_UNION;
#endif
#define SOC_USBOTG_DVBUSDIS_dvbusdis_START  (0)
#define SOC_USBOTG_DVBUSDIS_dvbusdis_END    (15)


/*****************************************************************************
 结构名    : SOC_USBOTG_DVBUSPULSE_UNION
 结构说明  : DVBUSPULSE 寄存器结构定义。地址偏移量:0x082C，初值:0x000005B8，宽度:32
 寄存器说明: Vbus脉冲配置寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  dvbuspulse : 16; /* bit[0-15] : Vbus脉冲时间。配置在SRP过程中Vbus脉冲时间。该域的值等于Vbus脉冲时间的PHY时钟周期数/1024。 */
        unsigned int  reserved   : 16; /* bit[16-31]: 保留。 */
    } reg;
} SOC_USBOTG_DVBUSPULSE_UNION;
#endif
#define SOC_USBOTG_DVBUSPULSE_dvbuspulse_START  (0)
#define SOC_USBOTG_DVBUSPULSE_dvbuspulse_END    (15)


/*****************************************************************************
 结构名    : SOC_USBOTG_DTHRCTL_UNION
 结构说明  : DTHRCTL 寄存器结构定义。地址偏移量:0x0830，初值:0x0C100020，宽度:32
 寄存器说明: device水线控制寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  nonisothren : 1;  /* bit[0]    : 非Isochronous IN端点水线使能。
                                                       0：不使能；
                                                       1：使能。 */
        unsigned int  isothren    : 1;  /* bit[1]    : Isochronous IN端点水线使能。
                                                       0：不使能；
                                                       1：使能。 */
        unsigned int  txthrlen    : 9;  /* bit[2-10] : 发送FIFO水线，该域同时也是MAC水线。单位为32bit。当发送FIFO中数据大于水线，向USB发送数据。
                                                       当AHBThrRatio=2'b00时，水线至少配置为8；当AHBThrRation不为0时，该域配置的值必须保证AHB水线不小于8。
                                                       该域对isochronous和非isochronous传输都有效。
                                                       建议配置值与配置的AHB burst长度相等。 */
        unsigned int  ahbthrratio : 2;  /* bit[11-12]: 发送数据时，AHB水线与MAC水线的比例。
                                                       00：AHB水线＝MAC水线；
                                                       01：AHB水线＝MAC水线/2;
                                                       10：AHB水线＝MAC水线/4；
                                                       11：AHB水线＝MAC水线/8 */
        unsigned int  reserved_0  : 3;  /* bit[13-15]: 保留。 */
        unsigned int  rxthren     : 1;  /* bit[16]   : 接收水线使能。
                                                       0：不使能；
                                                       1：使能。 */
        unsigned int  rxthrlen    : 9;  /* bit[17-25]: 接收FIFO水线。单位为32bit。当从USB总线上接收到的数据大于该域配置值时，启动AHB传输。该域配置值不能小于8。建议该域配置值等于AHB burst长度。 */
        unsigned int  reserved_1  : 1;  /* bit[26]   : 保留。 */
        unsigned int  arbpken     : 1;  /* bit[27]   : DMA仲裁配置。该bit配置IN端点的DMA仲裁。当该bit使能，如果某IN端点接收到IN令牌，该IN端点会一直得到DMA的仲裁。该bit可以防止发送FIFO下溢。
                                                       0：不使能；
                                                       1：使能。 */
        unsigned int  reserved_2  : 4;  /* bit[28-31]: 保留。 */
    } reg;
} SOC_USBOTG_DTHRCTL_UNION;
#endif
#define SOC_USBOTG_DTHRCTL_nonisothren_START  (0)
#define SOC_USBOTG_DTHRCTL_nonisothren_END    (0)
#define SOC_USBOTG_DTHRCTL_isothren_START     (1)
#define SOC_USBOTG_DTHRCTL_isothren_END       (1)
#define SOC_USBOTG_DTHRCTL_txthrlen_START     (2)
#define SOC_USBOTG_DTHRCTL_txthrlen_END       (10)
#define SOC_USBOTG_DTHRCTL_ahbthrratio_START  (11)
#define SOC_USBOTG_DTHRCTL_ahbthrratio_END    (12)
#define SOC_USBOTG_DTHRCTL_rxthren_START      (16)
#define SOC_USBOTG_DTHRCTL_rxthren_END        (16)
#define SOC_USBOTG_DTHRCTL_rxthrlen_START     (17)
#define SOC_USBOTG_DTHRCTL_rxthrlen_END       (25)
#define SOC_USBOTG_DTHRCTL_arbpken_START      (27)
#define SOC_USBOTG_DTHRCTL_arbpken_END        (27)


/*****************************************************************************
 结构名    : SOC_USBOTG_DIEPEMPMSK_UNION
 结构说明  : DIEPEMPMSK 寄存器结构定义。地址偏移量:0x0834，初值:0x00000000，宽度:32
 寄存器说明: IN端点FIFO空中断屏蔽寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ineptxfempmsk : 16; /* bit[0-15] : IN端点发送FIFO空中断屏蔽寄存器。每bit对应一个端点，bit[0]对应端点0，bit[15]对应端点15。
                                                         每bit含义：
                                                         0：屏蔽；
                                                         1：不屏蔽。 */
        unsigned int  reserved      : 16; /* bit[16-31]: 保留。 */
    } reg;
} SOC_USBOTG_DIEPEMPMSK_UNION;
#endif
#define SOC_USBOTG_DIEPEMPMSK_ineptxfempmsk_START  (0)
#define SOC_USBOTG_DIEPEMPMSK_ineptxfempmsk_END    (15)


/*****************************************************************************
 结构名    : SOC_USBOTG_DIEPCTL0_UNION
 结构说明  : DIEPCTL0 寄存器结构定义。地址偏移量:0x0900，初值:0x00008000，宽度:32
 寄存器说明: IN端点0控制寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  mps      : 2;  /* bit[0-1]  : 最大包大小。对IN端点和OUT端点都有效。
                                                    00：64字节；
                                                    01：32字节；
                                                    10：16字节；
                                                    11：8字节。 */
        unsigned int  reserved_0: 13; /* bit[2-14] : 保留。 */
        unsigned int  usbactep : 1;  /* bit[15]   : 端点是否激活。该bit一直为1，表示端点0一直处于激活状态。 */
        unsigned int  reserved_1: 1;  /* bit[16]   : 保留。 */
        unsigned int  naksts   : 1;  /* bit[17]   : NAK状态。
                                                    0：根据FIFO状态发送握手包；
                                                    1：对该端点的传输请求一直发送NAK握手包。 */
        unsigned int  eptype   : 2;  /* bit[18-19]: 端点类型。一直为0，表示端点0是控制端点。 */
        unsigned int  reserved_2: 1;  /* bit[20]   : 保留。 */
        unsigned int  stall    : 1;  /* bit[21]   : STALL握手设置。软件配置该bit使能，硬件收到setup包会清除该bit。该bit使能，对该端点的请求一直响应STALL握手。该bit的优先级高于NAK设置。
                                                    0：不使能；
                                                    1：使能。 */
        unsigned int  txfnum   : 4;  /* bit[22-25]: 端点0的发送FIFO序号。 */
        unsigned int  cnak     : 1;  /* bit[26]   : 清除NAK设置。向该bit写1清除NAK设置。
                                                    0：不清除；
                                                    1：清除。 */
        unsigned int  snak     : 1;  /* bit[27]   : NAK设置。设置该bit后，即使发送FIFO有数据，硬件也不向外发送。并对接收到的setup包返回NAK握手。
                                                    0：不使能；
                                                    1：使能。 */
        unsigned int  reserved_3: 2;  /* bit[28-29]: 保留。 */
        unsigned int  epdis    : 1;  /* bit[30]   : 端点使能清除。
                                                    0：不清除；
                                                    1：清除。 */
        unsigned int  epena    : 1;  /* bit[31]   : 端点使能。
                                                    0：不使能；
                                                    1：使能。 */
    } reg;
} SOC_USBOTG_DIEPCTL0_UNION;
#endif
#define SOC_USBOTG_DIEPCTL0_mps_START       (0)
#define SOC_USBOTG_DIEPCTL0_mps_END         (1)
#define SOC_USBOTG_DIEPCTL0_usbactep_START  (15)
#define SOC_USBOTG_DIEPCTL0_usbactep_END    (15)
#define SOC_USBOTG_DIEPCTL0_naksts_START    (17)
#define SOC_USBOTG_DIEPCTL0_naksts_END      (17)
#define SOC_USBOTG_DIEPCTL0_eptype_START    (18)
#define SOC_USBOTG_DIEPCTL0_eptype_END      (19)
#define SOC_USBOTG_DIEPCTL0_stall_START     (21)
#define SOC_USBOTG_DIEPCTL0_stall_END       (21)
#define SOC_USBOTG_DIEPCTL0_txfnum_START    (22)
#define SOC_USBOTG_DIEPCTL0_txfnum_END      (25)
#define SOC_USBOTG_DIEPCTL0_cnak_START      (26)
#define SOC_USBOTG_DIEPCTL0_cnak_END        (26)
#define SOC_USBOTG_DIEPCTL0_snak_START      (27)
#define SOC_USBOTG_DIEPCTL0_snak_END        (27)
#define SOC_USBOTG_DIEPCTL0_epdis_START     (30)
#define SOC_USBOTG_DIEPCTL0_epdis_END       (30)
#define SOC_USBOTG_DIEPCTL0_epena_START     (31)
#define SOC_USBOTG_DIEPCTL0_epena_END       (31)


/*****************************************************************************
 结构名    : SOC_USBOTG_DIEPCTLN_UNION
 结构说明  : DIEPCTLN 寄存器结构定义。地址偏移量:0x0900+(0x20*(n))，初值:0x00000000，宽度:32
 寄存器说明: IN端点n控制寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  mps      : 11; /* bit[0-10] : 最大包长。单位为字节。 */
        unsigned int  reserved_0: 4;  /* bit[11-14]: 保留。 */
        unsigned int  usbactep : 1;  /* bit[15]   : 端点激活。
                                                    0：没有激活；
                                                    1：激活。 */
        unsigned int  dpid     : 1;  /* bit[16]   : 数据包PID状态。仅对interrupt和bulk传输有效。
                                                    0：DATA0；
                                                    1：DATA1。 */
        unsigned int  naksts   : 1;  /* bit[17]   : NAK状态。
                                                    0：根据FIFO状态发送握手包；
                                                    1：对该端点的传输请求一直发送NAK握手包。 */
        unsigned int  eptype   : 2;  /* bit[18-19]: 端点类型。
                                                    00：control；
                                                    01：isochronous；
                                                    10：bulk；
                                                    11：interrupt。 */
        unsigned int  reserved_1: 1;  /* bit[20]   : 保留。 */
        unsigned int  stall    : 1;  /* bit[21]   : Stall握手设置。
                                                    对于非control和非isochronous端点，使能该bit后，对所有的传输请求返回stall握手包。优先级比NAK设置高。
                                                    0：不使能；
                                                    1：使能。 */
        unsigned int  txfnum   : 4;  /* bit[22-25]: 对应该端点的发送FIFO序号。 */
        unsigned int  cnak     : 1;  /* bit[26]   : 清除NAK设置。向该bit写1清除NAK设置。
                                                    0：不清除；
                                                    1：清除。 */
        unsigned int  snak     : 1;  /* bit[27]   : NAK设置。设置该bit后，即使发送FIFO有数据，硬件也不向外发送。并对接收到的setup包返回NAK握手。
                                                    0：根据FIFO状态发送握手包；
                                                    1：对该端点的传输请求一直发送NAK握手包。 */
        unsigned int  setd0pid : 1;  /* bit[28]   : 设置数据包PID为data0。
                                                    0：不设置；
                                                    1：设置。 */
        unsigned int  setd1pid : 1;  /* bit[29]   : 设置数据包PID为data1。
                                                    0：不设置；
                                                    1：设置。 */
        unsigned int  epdis    : 1;  /* bit[30]   : 设置端点不使能。
                                                    0：不设置；
                                                    1：设置。 */
        unsigned int  epena    : 1;  /* bit[31]   : 设置端点使能。
                                                    0：不设置；
                                                    1：设置。 */
    } reg;
} SOC_USBOTG_DIEPCTLN_UNION;
#endif
#define SOC_USBOTG_DIEPCTLN_mps_START       (0)
#define SOC_USBOTG_DIEPCTLN_mps_END         (10)
#define SOC_USBOTG_DIEPCTLN_usbactep_START  (15)
#define SOC_USBOTG_DIEPCTLN_usbactep_END    (15)
#define SOC_USBOTG_DIEPCTLN_dpid_START      (16)
#define SOC_USBOTG_DIEPCTLN_dpid_END        (16)
#define SOC_USBOTG_DIEPCTLN_naksts_START    (17)
#define SOC_USBOTG_DIEPCTLN_naksts_END      (17)
#define SOC_USBOTG_DIEPCTLN_eptype_START    (18)
#define SOC_USBOTG_DIEPCTLN_eptype_END      (19)
#define SOC_USBOTG_DIEPCTLN_stall_START     (21)
#define SOC_USBOTG_DIEPCTLN_stall_END       (21)
#define SOC_USBOTG_DIEPCTLN_txfnum_START    (22)
#define SOC_USBOTG_DIEPCTLN_txfnum_END      (25)
#define SOC_USBOTG_DIEPCTLN_cnak_START      (26)
#define SOC_USBOTG_DIEPCTLN_cnak_END        (26)
#define SOC_USBOTG_DIEPCTLN_snak_START      (27)
#define SOC_USBOTG_DIEPCTLN_snak_END        (27)
#define SOC_USBOTG_DIEPCTLN_setd0pid_START  (28)
#define SOC_USBOTG_DIEPCTLN_setd0pid_END    (28)
#define SOC_USBOTG_DIEPCTLN_setd1pid_START  (29)
#define SOC_USBOTG_DIEPCTLN_setd1pid_END    (29)
#define SOC_USBOTG_DIEPCTLN_epdis_START     (30)
#define SOC_USBOTG_DIEPCTLN_epdis_END       (30)
#define SOC_USBOTG_DIEPCTLN_epena_START     (31)
#define SOC_USBOTG_DIEPCTLN_epena_END       (31)


/*****************************************************************************
 结构名    : SOC_USBOTG_DIEPINTM_UNION
 结构说明  : DIEPINTM 寄存器结构定义。地址偏移量:0x0908+(0x20*(m))，初值:0x00000080，宽度:32
 寄存器说明: IN端点m中断状态寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  xfercompl   : 1;  /* bit[0]    : 传输完成中断。
                                                       0：无中断；
                                                       1：有中断。 */
        unsigned int  epdisbld    : 1;  /* bit[1]    : 端点没有使能中断。指示对于软件的请求，该端点没有被使能。
                                                       0：无中断；
                                                       1：有中断。 */
        unsigned int  ahberr      : 1;  /* bit[2]    : AHB错误中断。指示AHB总线错误。软件可以通过相应端点的DMA地址寄存器。得到出错的地址。
                                                       0：无中断；
                                                       1：有中断。 */
        unsigned int  reserved_0  : 1;  /* bit[3]    : 保留。 */
        unsigned int  intkntxfemp : 1;  /* bit[4]    : 接收到IN令牌时，发送FIFO空中断。
                                                       0：无中断；
                                                       1：有中断。 */
        unsigned int  reserved_1  : 1;  /* bit[5]    : 保留。 */
        unsigned int  inepnakeff  : 1;  /* bit[6]    : NAK设置有效指示。
                                                       0：无中断；
                                                       1：有中断。 */
        unsigned int  txfemp      : 1;  /* bit[7]    : 发送FIFO空中断。指示发送FIFO半空或者全空。是指示半空还是全空状态，由GAHB[NPTxFEmpLvl]的配置值决定。
                                                       0：无中断；
                                                       1：有中断。 */
        unsigned int  txfifoundrn : 1;  /* bit[8]    : 发送FIFO下溢中断。
                                                       0：无中断；
                                                       1：有中断。 */
        unsigned int  bnaintr     : 1;  /* bit[9]    : BNA(Buffer Not Available)中断。仅当Scatter/Gather DMA使能时有效。
                                                       0：无中断；
                                                       1：有中断。 */
        unsigned int  reserved_2  : 2;  /* bit[10-11]: 保留。 */
        unsigned int  bbleerr     : 1;  /* bit[12]   : Bable错误中断。 */
        unsigned int  nakintrpt   : 1;  /* bit[13]   : NAK中断。指示端点接收到NAK握手包。
                                                       0：无中断；
                                                       1：有中断。 */
        unsigned int  reserved_3  : 18; /* bit[14-31]: 保留。 */
    } reg;
} SOC_USBOTG_DIEPINTM_UNION;
#endif
#define SOC_USBOTG_DIEPINTM_xfercompl_START    (0)
#define SOC_USBOTG_DIEPINTM_xfercompl_END      (0)
#define SOC_USBOTG_DIEPINTM_epdisbld_START     (1)
#define SOC_USBOTG_DIEPINTM_epdisbld_END       (1)
#define SOC_USBOTG_DIEPINTM_ahberr_START       (2)
#define SOC_USBOTG_DIEPINTM_ahberr_END         (2)
#define SOC_USBOTG_DIEPINTM_intkntxfemp_START  (4)
#define SOC_USBOTG_DIEPINTM_intkntxfemp_END    (4)
#define SOC_USBOTG_DIEPINTM_inepnakeff_START   (6)
#define SOC_USBOTG_DIEPINTM_inepnakeff_END     (6)
#define SOC_USBOTG_DIEPINTM_txfemp_START       (7)
#define SOC_USBOTG_DIEPINTM_txfemp_END         (7)
#define SOC_USBOTG_DIEPINTM_txfifoundrn_START  (8)
#define SOC_USBOTG_DIEPINTM_txfifoundrn_END    (8)
#define SOC_USBOTG_DIEPINTM_bnaintr_START      (9)
#define SOC_USBOTG_DIEPINTM_bnaintr_END        (9)
#define SOC_USBOTG_DIEPINTM_bbleerr_START      (12)
#define SOC_USBOTG_DIEPINTM_bbleerr_END        (12)
#define SOC_USBOTG_DIEPINTM_nakintrpt_START    (13)
#define SOC_USBOTG_DIEPINTM_nakintrpt_END      (13)


/*****************************************************************************
 结构名    : SOC_USBOTG_DIEPTSIZ0_UNION
 结构说明  : DIEPTSIZ0 寄存器结构定义。地址偏移量:0x0910，初值:0x00000000，宽度:32
 寄存器说明: IN端点0传输大小寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  xfersize : 7;  /* bit[0-6]  : 传输大小。单位为byte。该域计数到0会触发传输完成中断。每有1个包的数据写入发送FIFO时，该域会减去包长并更新。 */
        unsigned int  reserved_0: 12; /* bit[7-18] : 保留。 */
        unsigned int  pktcnt   : 2;  /* bit[19-20]: 包数量。每有1个包从发送FIFO读出时，该域进行递减。 */
        unsigned int  reserved_1: 11; /* bit[21-31]: 保留。 */
    } reg;
} SOC_USBOTG_DIEPTSIZ0_UNION;
#endif
#define SOC_USBOTG_DIEPTSIZ0_xfersize_START  (0)
#define SOC_USBOTG_DIEPTSIZ0_xfersize_END    (6)
#define SOC_USBOTG_DIEPTSIZ0_pktcnt_START    (19)
#define SOC_USBOTG_DIEPTSIZ0_pktcnt_END      (20)


/*****************************************************************************
 结构名    : SOC_USBOTG_DIEPTSIZN_UNION
 结构说明  : DIEPTSIZN 寄存器结构定义。地址偏移量:0x0910+(0x20*(n))，初值:0x00000000，宽度:32
 寄存器说明: IN端点n传输大小寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  xfersize : 19; /* bit[0-18] : 传输大小。单位为byte。该域计数到0会触发传输完成中断。每有1个包的数据写入发送FIFO时，该域会减去包长并更新。 */
        unsigned int  pktcnt   : 10; /* bit[19-28]: 包数量。指示该传输的数据包个数，每有1个包从发送FIFO读出时，该域进行递减。 */
        unsigned int  mc       : 2;  /* bit[29-30]: 对于周期端点，该域指示每个微帧内必须传输的包数量。硬件根据该域的值产生IN数据包的PID。
                                                    对于非周期传输，该域指示切换到下1个端点前，当前端点必须传输的包数量。 */
        unsigned int  reserved : 1;  /* bit[31]   : 保留。 */
    } reg;
} SOC_USBOTG_DIEPTSIZN_UNION;
#endif
#define SOC_USBOTG_DIEPTSIZN_xfersize_START  (0)
#define SOC_USBOTG_DIEPTSIZN_xfersize_END    (18)
#define SOC_USBOTG_DIEPTSIZN_pktcnt_START    (19)
#define SOC_USBOTG_DIEPTSIZN_pktcnt_END      (28)
#define SOC_USBOTG_DIEPTSIZN_mc_START        (29)
#define SOC_USBOTG_DIEPTSIZN_mc_END          (30)


/*****************************************************************************
 结构名    : SOC_USBOTG_DIEPDMAM_UNION
 结构说明  : DIEPDMAM 寄存器结构定义。地址偏移量:0x0914+(0x20*(m))，初值:0x00000000，宽度:32
 寄存器说明: IN端点m DMA地址寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  dmaaddr : 32; /* bit[0-31]: DMA地址。当Scatter/Gather使能时，该域描述符起始地址。必须32bit对齐。 */
    } reg;
} SOC_USBOTG_DIEPDMAM_UNION;
#endif
#define SOC_USBOTG_DIEPDMAM_dmaaddr_START  (0)
#define SOC_USBOTG_DIEPDMAM_dmaaddr_END    (31)


/*****************************************************************************
 结构名    : SOC_USBOTG_DTXFSTSM_UNION
 结构说明  : DTXFSTSM 寄存器结构定义。地址偏移量:0x0918，初值:0x00001000，宽度:32
 寄存器说明: IN端点m发送FIFO状态寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ineptxfspcavail : 16; /* bit[0-15] : 发送FIFO可用空间，单位为32bit。 */
        unsigned int  reserved        : 16; /* bit[16-31]: 保留。 */
    } reg;
} SOC_USBOTG_DTXFSTSM_UNION;
#endif
#define SOC_USBOTG_DTXFSTSM_ineptxfspcavail_START  (0)
#define SOC_USBOTG_DTXFSTSM_ineptxfspcavail_END    (15)


/*****************************************************************************
 结构名    : SOC_USBOTG_DIEPDMABN_UNION
 结构说明  : DIEPDMABN 寄存器结构定义。地址偏移量:0x091C+(0x20*(m))，初值:0x00000000，宽度:32
 寄存器说明: IN端点m DMA buffer地址寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  dmabufferaddr : 32; /* bit[0-31]: DMA buffer地址。仅当Scatter/Gather使能时有效。指示当前buffer的地址。 */
    } reg;
} SOC_USBOTG_DIEPDMABN_UNION;
#endif
#define SOC_USBOTG_DIEPDMABN_dmabufferaddr_START  (0)
#define SOC_USBOTG_DIEPDMABN_dmabufferaddr_END    (31)


/*****************************************************************************
 结构名    : SOC_USBOTG_DOEPCTL0_UNION
 结构说明  : DOEPCTL0 寄存器结构定义。地址偏移量:0x0B00，初值:0x00008000，宽度:32
 寄存器说明: OUT端点0控制寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  mps      : 2;  /* bit[0-1]  : 最大包大小。与IN端点0最大包长一样。
                                                    00：64字节；
                                                    01：32字节；
                                                    10：16字节；
                                                    11：8字节。 */
        unsigned int  reserved_0: 13; /* bit[2-14] : 保留。 */
        unsigned int  usbactep : 1;  /* bit[15]   : 端点是否激活。该bit一直为1，表示端点0一直处于激活状态。 */
        unsigned int  reserved_1: 1;  /* bit[16]   : 保留。 */
        unsigned int  naksts   : 1;  /* bit[17]   : NAK状态。
                                                    0：根据FIFO状态发送握手包；
                                                    1：对该端点的传输请求一直发送NAK握手包。 */
        unsigned int  eptype   : 2;  /* bit[18-19]: 端点类型。一直为0，表示端点0是控制端点。 */
        unsigned int  snp      : 1;  /* bit[20]   : 探听模式设置。使能该模式，硬件不检查OUT包，直接将接收到的数据写到memory中。
                                                    0：不使能；
                                                    1：使能。 */
        unsigned int  stall    : 1;  /* bit[21]   : STALL握手设置。软件配置该bit使能，硬件收到setup包会清除该bit。该bit使能，对该端点的请求一直响应STALL握手。该bit的优先级高于NAK设置。
                                                    0：不使能；
                                                    1：使能。 */
        unsigned int  txfnum   : 4;  /* bit[22-25]: 端点0的发送FIFO序号。 */
        unsigned int  cnak     : 1;  /* bit[26]   : 清除NAK设置。向该bit写1清除NAK设置。
                                                    0：不清除；
                                                    1：清除。 */
        unsigned int  snak     : 1;  /* bit[27]   : NAK设置。设置该bit后，硬件会丢弃所有接收到的数据，即使接收FIFO有空间。
                                                    0：根据FIFO状态发送握手包；
                                                    1：对该端点的传输请求一直发送NAK握手包。 */
        unsigned int  reserved_2: 2;  /* bit[28-29]: 保留。 */
        unsigned int  epdis    : 1;  /* bit[30]   : 端点使能清除。
                                                    0：不清除；
                                                    1：清除。 */
        unsigned int  epena    : 1;  /* bit[31]   : 端点使能。软件在如下条件下会清除该位：控制传输setup阶段结束；端点使能清除；传输结束。
                                                    0：不使能；
                                                    1：使能。 */
    } reg;
} SOC_USBOTG_DOEPCTL0_UNION;
#endif
#define SOC_USBOTG_DOEPCTL0_mps_START       (0)
#define SOC_USBOTG_DOEPCTL0_mps_END         (1)
#define SOC_USBOTG_DOEPCTL0_usbactep_START  (15)
#define SOC_USBOTG_DOEPCTL0_usbactep_END    (15)
#define SOC_USBOTG_DOEPCTL0_naksts_START    (17)
#define SOC_USBOTG_DOEPCTL0_naksts_END      (17)
#define SOC_USBOTG_DOEPCTL0_eptype_START    (18)
#define SOC_USBOTG_DOEPCTL0_eptype_END      (19)
#define SOC_USBOTG_DOEPCTL0_snp_START       (20)
#define SOC_USBOTG_DOEPCTL0_snp_END         (20)
#define SOC_USBOTG_DOEPCTL0_stall_START     (21)
#define SOC_USBOTG_DOEPCTL0_stall_END       (21)
#define SOC_USBOTG_DOEPCTL0_txfnum_START    (22)
#define SOC_USBOTG_DOEPCTL0_txfnum_END      (25)
#define SOC_USBOTG_DOEPCTL0_cnak_START      (26)
#define SOC_USBOTG_DOEPCTL0_cnak_END        (26)
#define SOC_USBOTG_DOEPCTL0_snak_START      (27)
#define SOC_USBOTG_DOEPCTL0_snak_END        (27)
#define SOC_USBOTG_DOEPCTL0_epdis_START     (30)
#define SOC_USBOTG_DOEPCTL0_epdis_END       (30)
#define SOC_USBOTG_DOEPCTL0_epena_START     (31)
#define SOC_USBOTG_DOEPCTL0_epena_END       (31)


/*****************************************************************************
 结构名    : SOC_USBOTG_DOEPCTLN_UNION
 结构说明  : DOEPCTLN 寄存器结构定义。地址偏移量:0x0B00+(0x20*(n))，初值:0x00000000，宽度:32
 寄存器说明: OUT端点n控制寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  mps      : 11; /* bit[0-10] : 最大包长。单位为字节。 */
        unsigned int  reserved : 4;  /* bit[11-14]: 保留。 */
        unsigned int  usbactep : 1;  /* bit[15]   : 端点激活。
                                                    1：激活；
                                                    0：没有激活。 */
        unsigned int  dpid     : 1;  /* bit[16]   : 数据包PID状态。仅对intterupt和bulk传输有效。
                                                    0：DATA0；
                                                    1：DATA1。 */
        unsigned int  naksts   : 1;  /* bit[17]   : NAK状态。
                                                    0：根据FIFO状态发送握手包；
                                                    1：对该端点的传输请求一直发送NAK握手包。 */
        unsigned int  eptype   : 2;  /* bit[18-19]: 端点类型。
                                                    00：control；
                                                    01：isochronous；
                                                    10：bulk；
                                                    11：interrupt。 */
        unsigned int  snp      : 1;  /* bit[20]   : 探听模式设置。使能该模式，硬件不检查OUT包，直接将接收到的数据写到memory中。
                                                    0：不使能；
                                                    1：使能。 */
        unsigned int  stall    : 1;  /* bit[21]   : Stall握手设置。
                                                    对于非control和非isochronous端点，使能该bit后，对所有的传输请求返回stall握手包。优先级比NAK设置高。
                                                    0：不使能；
                                                    1：使能。 */
        unsigned int  txfnum   : 4;  /* bit[22-25]: 对应该端点的发送FIFO序号。 */
        unsigned int  cnak     : 1;  /* bit[26]   : 清除NAK设置。向该bit写1清除NAK设置。
                                                    0：不清除；
                                                    1：清除。 */
        unsigned int  snak     : 1;  /* bit[27]   : NAK设置。设置该bit后，硬件会丢弃所有接收到的数据，即使接收FIFO有空间。
                                                    0：根据FIFO状态发送握手包；
                                                    1：对该端点的传输请求一直发送NAK握手包。 */
        unsigned int  setd0pid : 1;  /* bit[28]   : 设置数据包PID为data0。
                                                    0：不设置；
                                                    1：设置。 */
        unsigned int  setd1pid : 1;  /* bit[29]   : 设置数据包PID为data1。
                                                    0：不设置；
                                                    1：设置。 */
        unsigned int  epdis    : 1;  /* bit[30]   : 设置端点不使能。
                                                    0：不设置；
                                                    1：设置。 */
        unsigned int  epena    : 1;  /* bit[31]   : 设置端点使能。
                                                    0：不设置；
                                                    1：设置。 */
    } reg;
} SOC_USBOTG_DOEPCTLN_UNION;
#endif
#define SOC_USBOTG_DOEPCTLN_mps_START       (0)
#define SOC_USBOTG_DOEPCTLN_mps_END         (10)
#define SOC_USBOTG_DOEPCTLN_usbactep_START  (15)
#define SOC_USBOTG_DOEPCTLN_usbactep_END    (15)
#define SOC_USBOTG_DOEPCTLN_dpid_START      (16)
#define SOC_USBOTG_DOEPCTLN_dpid_END        (16)
#define SOC_USBOTG_DOEPCTLN_naksts_START    (17)
#define SOC_USBOTG_DOEPCTLN_naksts_END      (17)
#define SOC_USBOTG_DOEPCTLN_eptype_START    (18)
#define SOC_USBOTG_DOEPCTLN_eptype_END      (19)
#define SOC_USBOTG_DOEPCTLN_snp_START       (20)
#define SOC_USBOTG_DOEPCTLN_snp_END         (20)
#define SOC_USBOTG_DOEPCTLN_stall_START     (21)
#define SOC_USBOTG_DOEPCTLN_stall_END       (21)
#define SOC_USBOTG_DOEPCTLN_txfnum_START    (22)
#define SOC_USBOTG_DOEPCTLN_txfnum_END      (25)
#define SOC_USBOTG_DOEPCTLN_cnak_START      (26)
#define SOC_USBOTG_DOEPCTLN_cnak_END        (26)
#define SOC_USBOTG_DOEPCTLN_snak_START      (27)
#define SOC_USBOTG_DOEPCTLN_snak_END        (27)
#define SOC_USBOTG_DOEPCTLN_setd0pid_START  (28)
#define SOC_USBOTG_DOEPCTLN_setd0pid_END    (28)
#define SOC_USBOTG_DOEPCTLN_setd1pid_START  (29)
#define SOC_USBOTG_DOEPCTLN_setd1pid_END    (29)
#define SOC_USBOTG_DOEPCTLN_epdis_START     (30)
#define SOC_USBOTG_DOEPCTLN_epdis_END       (30)
#define SOC_USBOTG_DOEPCTLN_epena_START     (31)
#define SOC_USBOTG_DOEPCTLN_epena_END       (31)


/*****************************************************************************
 结构名    : SOC_USBOTG_DOEPINTM_UNION
 结构说明  : DOEPINTM 寄存器结构定义。地址偏移量:0x0B08+(0x20*(m))，初值:0x00000000，宽度:32
 寄存器说明: OUT端点m中断状态寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  xfercompl   : 1;  /* bit[0]    : 传输完成中断。
                                                       0：无中断；
                                                       1：有中断。 */
        unsigned int  epdisbld    : 1;  /* bit[1]    : 端点没有使能中断。指示对于软件的请求，该端点没有被使能。
                                                       0：无中断；
                                                       1：有中断。 */
        unsigned int  ahberr      : 1;  /* bit[2]    : AHB错误中断。指示AHB总线错误。软件可以通过相应端点的DMA地址寄存器得到出错的地址。
                                                       0：无中断；
                                                       1：有中断。 */
        unsigned int  reserved_0  : 1;  /* bit[3]    : 保留。 */
        unsigned int  intkntxfemp : 1;  /* bit[4]    : 接收到IN令牌时，发送FIFO空中断。
                                                       0：无中断；
                                                       1：有中断。 */
        unsigned int  reserved_1  : 1;  /* bit[5]    : 保留。 */
        unsigned int  inepnakeff  : 1;  /* bit[6]    : NAK设置有效指示。
                                                       0：无中断；
                                                       1：有中断。 */
        unsigned int  reserved_2  : 1;  /* bit[7]    : 保留。 */
        unsigned int  outpkterr   : 1;  /* bit[8]    : OUT包错误。指示接收FIFO溢出或者OUT包有CRC错误。该bit仅水线使能时有效。
                                                       0：无中断；
                                                       1：有中断。 */
        unsigned int  bnaintr     : 1;  /* bit[9]    : BNA(Buffer Not Available)中断。仅当Scatter/Gather DMA使能时有效。
                                                       0：无中断；
                                                       1：有中断。 */
        unsigned int  reserved_3  : 1;  /* bit[10]   : 保留。 */
        unsigned int  pktdrpsts   : 1;  /* bit[11]   : 包丢弃状态。指示有1个isochronous包被丢弃。该bit没有屏蔽位，且不会产生中断上报。仅当Scatter/Gather DMA模式不使能时有效。
                                                       0：无中断；
                                                       1：有中断。 */
        unsigned int  bbleerr     : 1;  /* bit[12]   : Bable错误中断。
                                                       0：无中断；
                                                       1：有中断。 */
        unsigned int  nakintrpt   : 1;  /* bit[13]   : NAK中断。指示端点接收到NAK握手包。
                                                       0：无中断；
                                                       1：有中断。 */
        unsigned int  nyet        : 1;  /* bit[14]   : NYET中断。该端点作为非isochronous端点时，NYET握手包发送会触发该中断。
                                                       0：无中断；
                                                       1：有中断。 */
        unsigned int  reserved_4  : 17; /* bit[15-31]: 保留。 */
    } reg;
} SOC_USBOTG_DOEPINTM_UNION;
#endif
#define SOC_USBOTG_DOEPINTM_xfercompl_START    (0)
#define SOC_USBOTG_DOEPINTM_xfercompl_END      (0)
#define SOC_USBOTG_DOEPINTM_epdisbld_START     (1)
#define SOC_USBOTG_DOEPINTM_epdisbld_END       (1)
#define SOC_USBOTG_DOEPINTM_ahberr_START       (2)
#define SOC_USBOTG_DOEPINTM_ahberr_END         (2)
#define SOC_USBOTG_DOEPINTM_intkntxfemp_START  (4)
#define SOC_USBOTG_DOEPINTM_intkntxfemp_END    (4)
#define SOC_USBOTG_DOEPINTM_inepnakeff_START   (6)
#define SOC_USBOTG_DOEPINTM_inepnakeff_END     (6)
#define SOC_USBOTG_DOEPINTM_outpkterr_START    (8)
#define SOC_USBOTG_DOEPINTM_outpkterr_END      (8)
#define SOC_USBOTG_DOEPINTM_bnaintr_START      (9)
#define SOC_USBOTG_DOEPINTM_bnaintr_END        (9)
#define SOC_USBOTG_DOEPINTM_pktdrpsts_START    (11)
#define SOC_USBOTG_DOEPINTM_pktdrpsts_END      (11)
#define SOC_USBOTG_DOEPINTM_bbleerr_START      (12)
#define SOC_USBOTG_DOEPINTM_bbleerr_END        (12)
#define SOC_USBOTG_DOEPINTM_nakintrpt_START    (13)
#define SOC_USBOTG_DOEPINTM_nakintrpt_END      (13)
#define SOC_USBOTG_DOEPINTM_nyet_START         (14)
#define SOC_USBOTG_DOEPINTM_nyet_END           (14)


/*****************************************************************************
 结构名    : SOC_USBOTG_DOEPTSIZ0_UNION
 结构说明  : DOEPTSIZ0 寄存器结构定义。地址偏移量:0x0B10，初值:0x00000000，宽度:32
 寄存器说明: OUT端点0传输大小寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  xfersize : 7;  /* bit[0-6]  : 传输大小。单位为byte。该域计数到0会触发传输完成中断。每有1个包的数据从接收FIFO读出写入外部memory中时，该域会减去包长并更新。 */
        unsigned int  reserved_0: 12; /* bit[7-18] : 保留。 */
        unsigned int  pktcnt   : 2;  /* bit[19-20]: 包数量。每有1个包写入接收FIFO中时，该域进行递减。 */
        unsigned int  reserved_1: 8;  /* bit[21-28]: 保留。 */
        unsigned int  supcnt   : 2;  /* bit[29-30]: setup包数量。该域指示能连续接收的setup包的数量。 */
        unsigned int  reserved_2: 1;  /* bit[31]   : 保留。 */
    } reg;
} SOC_USBOTG_DOEPTSIZ0_UNION;
#endif
#define SOC_USBOTG_DOEPTSIZ0_xfersize_START  (0)
#define SOC_USBOTG_DOEPTSIZ0_xfersize_END    (6)
#define SOC_USBOTG_DOEPTSIZ0_pktcnt_START    (19)
#define SOC_USBOTG_DOEPTSIZ0_pktcnt_END      (20)
#define SOC_USBOTG_DOEPTSIZ0_supcnt_START    (29)
#define SOC_USBOTG_DOEPTSIZ0_supcnt_END      (30)


/*****************************************************************************
 结构名    : SOC_USBOTG_DOEPTSIZN_UNION
 结构说明  : DOEPTSIZN 寄存器结构定义。地址偏移量:0x0B10+(0x20*(n))，初值:0x00000000，宽度:32
 寄存器说明: OUT端点n传输大小寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  xfersize : 19; /* bit[0-18] : 传输大小。单位为byte。该域计数到0会触发传输完成中断。每有1个包的数据从接收FIFO读出写入外部memory中时，该域会减去包长并更新。 */
        unsigned int  pktcnt   : 10; /* bit[19-28]: 包数量。每有1个包写入接收FIFO中时，该域进行递减。 */
        unsigned int  rxdpid   : 2;  /* bit[29-30]: 对于isochronous端点，指示接收到的数据包PID。
                                                    00：DATA0；
                                                    01：DATA1；
                                                    10：DATA2；
                                                    11：MDATA。
                                                    对于控制端点，指示可以接收的背靠背setup包个数。
                                                    00：reserved；
                                                    01：1个；
                                                    10：2个；
                                                    11：3个。 */
        unsigned int  reserved : 1;  /* bit[31]   : 保留。 */
    } reg;
} SOC_USBOTG_DOEPTSIZN_UNION;
#endif
#define SOC_USBOTG_DOEPTSIZN_xfersize_START  (0)
#define SOC_USBOTG_DOEPTSIZN_xfersize_END    (18)
#define SOC_USBOTG_DOEPTSIZN_pktcnt_START    (19)
#define SOC_USBOTG_DOEPTSIZN_pktcnt_END      (28)
#define SOC_USBOTG_DOEPTSIZN_rxdpid_START    (29)
#define SOC_USBOTG_DOEPTSIZN_rxdpid_END      (30)


/*****************************************************************************
 结构名    : SOC_USBOTG_DOEPDMAM_UNION
 结构说明  : DOEPDMAM 寄存器结构定义。地址偏移量:0x0B14+(0x20*(m))，初值:0x00000000，宽度:32
 寄存器说明: OUT端点m DMA地址寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  dmaaddr : 32; /* bit[0-31]: DMA地址。当Scatter/Gather使能时，该域为描述符起始地址。 */
    } reg;
} SOC_USBOTG_DOEPDMAM_UNION;
#endif
#define SOC_USBOTG_DOEPDMAM_dmaaddr_START  (0)
#define SOC_USBOTG_DOEPDMAM_dmaaddr_END    (31)


/*****************************************************************************
 结构名    : SOC_USBOTG_DOEPDMABM_UNION
 结构说明  : DOEPDMABM 寄存器结构定义。地址偏移量:0x0B1C+(0x20*(m))，初值:0x00000000，宽度:32
 寄存器说明: OUT端点m DMA buffer地址寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  dmabufferaddr : 32; /* bit[0-31]: DMA buffer地址。仅当Scatter/Gather使能时有效。指示当前buffer的地址。 */
    } reg;
} SOC_USBOTG_DOEPDMABM_UNION;
#endif
#define SOC_USBOTG_DOEPDMABM_dmabufferaddr_START  (0)
#define SOC_USBOTG_DOEPDMABM_dmabufferaddr_END    (31)


/*****************************************************************************
 结构名    : SOC_USBOTG_PCGCCTL_UNION
 结构说明  : PCGCCTL 寄存器结构定义。地址偏移量:0x0E00，初值:0xA80A8000，宽度:32
 寄存器说明: 功耗和时钟门控控制寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  reserved     : 14; /* bit[0-13] : 保留。 */
        unsigned int  restorevalue : 18; /* bit[14-31]: 恢复值。
                                                        Bit[31]：device模式。
                                                        0：host模式；
                                                        1：device模式。
                                                        Bit[30:29]；端口速度。
                                                        00：高速；
                                                        01：全速；
                                                        others：reserved。
                                                        Bit[26:20]：device地址。
                                                        Bit[19]：mac_termselect。 
                                                        0：HS_TERM；
                                                        1：FS_TERM。
                                                        Bit[18:17]：mac_xcvrselect。
                                                        00：HS_XCVR；
                                                        01：FS_XCVR；
                                                        10：LS_XCVR；
                                                        11：LFS_XCVR。
                                                        Bit[16]：端口电源控制。
                                                        0：端口电源不使能；
                                                        1：端口电源使能。
                                                        bit[15:14]：prt_clk_sel。 */
    } reg;
} SOC_USBOTG_PCGCCTL_UNION;
#endif
#define SOC_USBOTG_PCGCCTL_restorevalue_START  (14)
#define SOC_USBOTG_PCGCCTL_restorevalue_END    (31)






/*****************************************************************************
  8 OTHERS定义
*****************************************************************************/



/*****************************************************************************
  9 全局变量声明
*****************************************************************************/


/*****************************************************************************
  10 函数声明
*****************************************************************************/


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of soc_usbotg_interface.h */
