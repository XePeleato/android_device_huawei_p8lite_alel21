

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/

#ifndef __SOC_VPP_INTERFACE_H__
#define __SOC_VPP_INTERFACE_H__

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
/* 寄存器说明：VPP控制寄存器。surface总线申请仲裁模式。
   位域定义UNION结构:  SOC_VPP_VOCTRL_UNION */
#define SOC_VPP_VOCTRL_ADDR(base)                     ((base) + (0x0000))

/* 寄存器说明：VPP中断状态，只读寄存器。
   位域定义UNION结构:  SOC_VPP_VOINTSTA_UNION */
#define SOC_VPP_VOINTSTA_ADDR(base)                   ((base) + (0x0004))

/* 寄存器说明：VPP经过Mask的中断状态寄存器。写1清零。
   位域定义UNION结构:  SOC_VPP_VOMSKINTSTA_UNION */
#define SOC_VPP_VOMSKINTSTA_ADDR(base)                ((base) + (0x0008))

/* 寄存器说明：VPP中断屏蔽寄存器。与VOINTSTA对应。相应比特为1表示中断打开，为0表示中断屏蔽。
   位域定义UNION结构:  SOC_VPP_VOINTMSK_UNION */
#define SOC_VPP_VOINTMSK_ADDR(base)                   ((base) + (0x000C))

/* 寄存器说明：VPP版本寄存器。
   位域定义UNION结构:  SOC_VPP_VOUVERSION1_UNION */
#define SOC_VPP_VOUVERSION1_ADDR(base)                ((base) + (0x0010))

/* 寄存器说明：VPP版本寄存器。
   位域定义UNION结构:  SOC_VPP_VOUVERSION2_UNION */
#define SOC_VPP_VOUVERSION2_ADDR(base)                ((base) + (0x0014))

/* 寄存器说明：仅仅用于验证环境
   位域定义UNION结构:  SOC_VPP_VODEBUG_UNION */
#define SOC_VPP_VODEBUG_ADDR(base)                    ((base) + (0x0020))

/* 寄存器说明：VPP链表控制寄存器
   位域定义UNION结构:  SOC_VPP_VOLNKLSTCTRL_UNION */
#define SOC_VPP_VOLNKLSTCTRL_ADDR(base)               ((base) + (0x0024))

/* 寄存器说明：VPP链表首地址寄存器
   位域定义UNION结构:  SOC_VPP_VOLNKLSTADDR_UNION */
#define SOC_VPP_VOLNKLSTADDR_ADDR(base)               ((base) + (0x0028))

/* 寄存器说明：VPP链表开始寄存器
   位域定义UNION结构:  SOC_VPP_VOLNKLSTSTRT_UNION */
#define SOC_VPP_VOLNKLSTSTRT_ADDR(base)               ((base) + (0x002C))

/* 寄存器说明：VPP链表pending状态寄存器1
   位域定义UNION结构:  SOC_VPP_VOLNKLSTPENDINGSTA_UNION */
#define SOC_VPP_VOLNKLSTPENDINGSTA_ADDR(base)         ((base) + (0x0030))

/* 寄存器说明：VPP链表Working状态寄存器1
   位域定义UNION结构:  SOC_VPP_VOLNKLSTWORKINGSTA_UNION */
#define SOC_VPP_VOLNKLSTWORKINGSTA_ADDR(base)         ((base) + (0x0034))

/* 寄存器说明：VPP链表end状态寄存器1
   位域定义UNION结构:  SOC_VPP_VOLNKLSTENDSTA_UNION */
#define SOC_VPP_VOLNKLSTENDSTA_ADDR(base)             ((base) + (0x0038))

/* 寄存器说明：VPP链表节点号状态寄存器寄存器
   位域定义UNION结构:  SOC_VPP_VOLNKLSTNUMSTA_UNION */
#define SOC_VPP_VOLNKLSTNUMSTA_ADDR(base)             ((base) + (0x003C))

/* 寄存器说明：缩放系数、LUT相关的寄存器更新使能寄存器。VPP的缩放系数是通过AXI Master配置的，软件通过Slave配置起始地址和参数需要更新的标志。
   位域定义UNION结构:  SOC_VPP_VOPARAUP_UNION */
#define SOC_VPP_VOPARAUP_ADDR(base)                   ((base) + (0x0040))

/* 寄存器说明：VHD水平亮度、水平色度滤波系数地址寄存器。
   位域定义UNION结构:  SOC_VPP_VHDHCOEFAD_UNION */
#define SOC_VPP_VHDHCOEFAD_ADDR(base)                 ((base) + (0x0044))

/* 寄存器说明：VHD垂直亮度、水平色度滤波系数地址寄存器。
   位域定义UNION结构:  SOC_VPP_VHDVCOEFAD_UNION */
#define SOC_VPP_VHDVCOEFAD_ADDR(base)                 ((base) + (0x0048))

/* 寄存器说明：VAD水平亮度、水平色度滤波系数地址寄存器。
   位域定义UNION结构:  SOC_VPP_VADHCOEFAD_UNION */
#define SOC_VPP_VADHCOEFAD_ADDR(base)                 ((base) + (0x004C))

/* 寄存器说明：VAD垂直亮度、水平色度滤波系数地址寄存器。
   位域定义UNION结构:  SOC_VPP_VADVCOEFAD_UNION */
#define SOC_VPP_VADVCOEFAD_ADDR(base)                 ((base) + (0x0050))

/* 寄存器说明：video通道ACC系数查找表地址寄存器。
   位域定义UNION结构:  SOC_VPP_ACCAD_UNION */
#define SOC_VPP_ACCAD_ADDR(base)                      ((base) + (0x0054))

/* 寄存器说明：该寄存器可以配置层的相关信息，为非即时寄存器。
   位域定义UNION结构:  SOC_VPP_VHDCTRL_UNION */
#define SOC_VPP_VHDCTRL_ADDR(base)                    ((base) + (0x0100))

/* 寄存器说明：VHD通道更新使能寄存器。
   位域定义UNION结构:  SOC_VPP_VHDUPD_UNION */
#define SOC_VPP_VHDUPD_ADDR(base)                     ((base) + (0x0104))

/* 寄存器说明：上一帧的地址寄存器。用于deinterace。对于package像素格式就是帧buffer地址；对于semi-planar像素格式是亮度帧buffer地址。
   位域定义UNION结构:  SOC_VPP_VHDLADDR_UNION */
#define SOC_VPP_VHDLADDR_ADDR(base)                   ((base) + (0x0108))

/* 寄存器说明：上一帧的色度地址寄存器。用于deinterace。对于package像素格式该地址无效；对于semi-planar像素格式是色度帧buffer地址。
   位域定义UNION结构:  SOC_VPP_VHDLCADDR_UNION */
#define SOC_VPP_VHDLCADDR_ADDR(base)                  ((base) + (0x010C))

/* 寄存器说明：当前帧的地址寄存器。对于package像素格式就是帧buffer地址；对于semi-planar像素格式是亮度帧buffer地址。
   位域定义UNION结构:  SOC_VPP_VHDCADDR_UNION */
#define SOC_VPP_VHDCADDR_ADDR(base)                   ((base) + (0x0110))

/* 寄存器说明：当前帧的色度地址寄存器。对于package像素格式该地址无效；对于semi-planar像素格式是色度帧buffer地址。DIE当前帧的地址。
   位域定义UNION结构:  SOC_VPP_VHDCCADDR_UNION */
#define SOC_VPP_VHDCCADDR_ADDR(base)                  ((base) + (0x0114))

/* 寄存器说明：下一帧的地址寄存器。用于deinterace。对于package像素格式就是帧buffer地址；对于semi-planar像素格式是亮度帧buffer地址。
   位域定义UNION结构:  SOC_VPP_VHDNADDR_UNION */
#define SOC_VPP_VHDNADDR_ADDR(base)                   ((base) + (0x0118))

/* 寄存器说明：下一帧的色度地址寄存器。用于deinterace。对于package像素格式该地址无效；对于semi-planar像素格式是色度帧buffer地址。
   位域定义UNION结构:  SOC_VPP_VHDNCADDR_UNION */
#define SOC_VPP_VHDNCADDR_ADDR(base)                  ((base) + (0x011C))

/* 寄存器说明：surface的stride寄存器。
   位域定义UNION结构:  SOC_VPP_VHDSTRIDE_UNION */
#define SOC_VPP_VHDSTRIDE_ADDR(base)                  ((base) + (0x0124))

/* 寄存器说明：输入分辨率寄存器，为非即时寄存器。
   位域定义UNION结构:  SOC_VPP_VHDIRESO_UNION */
#define SOC_VPP_VHDIRESO_ADDR(base)                   ((base) + (0x0128))

/* 寄存器说明：de-interlace历史信息buffer地址，为非即时寄存器。VDC_SD没有该寄存器。
   位域定义UNION结构:  SOC_VPP_VHDDIEADDR_UNION */
#define SOC_VPP_VHDDIEADDR_ADDR(base)                 ((base) + (0x0138))

/* 寄存器说明：VHD的数据回写buffer地址，为非即时寄存器。VDC_SD没有该寄存器。
   位域定义UNION结构:  SOC_VPP_VHDWBC1ADDR_UNION */
#define SOC_VPP_VHDWBC1ADDR_ADDR(base)                ((base) + (0x013C))

/* 寄存器说明：VHD的数据回写行间距，为非即时寄存器。VDC_SD没有该寄存器。
   位域定义UNION结构:  SOC_VPP_VHDWBC1STRD_UNION */
#define SOC_VPP_VHDWBC1STRD_ADDR(base)                ((base) + (0x0140))

/* 寄存器说明：de-interlace的统计信息回写地址，为非即时寄存器。VDC_SD没有该寄存器。
   位域定义UNION结构:  SOC_VPP_VHDDIESTADDR_UNION */
#define SOC_VPP_VHDDIESTADDR_ADDR(base)               ((base) + (0x0144))

/* 寄存器说明：Surface真实内容在显示窗口的起始位置（First POSition），以像素为单位，非即时寄存器。
   位域定义UNION结构:  SOC_VPP_VHDVFPOS_UNION */
#define SOC_VPP_VHDVFPOS_ADDR(base)                   ((base) + (0x0168))

/* 寄存器说明：Surface真实内容在显示窗口的结束位置（Last POSition），以像素为单位，非即时寄存器。
   位域定义UNION结构:  SOC_VPP_VHDVLPOS_UNION */
#define SOC_VPP_VHDVLPOS_ADDR(base)                   ((base) + (0x016C))

/* 寄存器说明：视频层的背景色寄存器。
   位域定义UNION结构:  SOC_VPP_VHDBK_UNION */
#define SOC_VPP_VHDBK_ADDR(base)                      ((base) + (0x0170))

/* 寄存器说明：色彩空间转换输入直流分量寄存器，为即时寄存器。
   位域定义UNION结构:  SOC_VPP_VHDCSCIDC_UNION */
#define SOC_VPP_VHDCSCIDC_ADDR(base)                  ((base) + (0x0180))

/* 寄存器说明：色彩空间转换输出直流分量寄存器，为即时寄存器。
   位域定义UNION结构:  SOC_VPP_VHDCSCODC_UNION */
#define SOC_VPP_VHDCSCODC_ADDR(base)                  ((base) + (0x0184))

/* 寄存器说明：色彩空间转换参数0，为即时寄存器。
   位域定义UNION结构:  SOC_VPP_VHDCSCP0_UNION */
#define SOC_VPP_VHDCSCP0_ADDR(base)                   ((base) + (0x0188))

/* 寄存器说明：色彩空间转换参数1，为即时寄存器。
   位域定义UNION结构:  SOC_VPP_VHDCSCP1_UNION */
#define SOC_VPP_VHDCSCP1_ADDR(base)                   ((base) + (0x018C))

/* 寄存器说明：色彩空间转换参数2，为即时寄存器。
   位域定义UNION结构:  SOC_VPP_VHDCSCP2_UNION */
#define SOC_VPP_VHDCSCP2_ADDR(base)                   ((base) + (0x0190))

/* 寄存器说明：色彩空间转换参数3，为即时寄存器。
   位域定义UNION结构:  SOC_VPP_VHDCSCP3_UNION */
#define SOC_VPP_VHDCSCP3_ADDR(base)                   ((base) + (0x0194))

/* 寄存器说明：色彩空间转换参数4，为即时寄存器。
   位域定义UNION结构:  SOC_VPP_VHDCSCP4_UNION */
#define SOC_VPP_VHDCSCP4_ADDR(base)                   ((base) + (0x0198))

/* 寄存器说明：ACM的控制寄存器。
   位域定义UNION结构:  SOC_VPP_VHDACM0_UNION */
#define SOC_VPP_VHDACM0_ADDR(base)                    ((base) + (0x01A0))

/* 寄存器说明：ACM寄存器。
   位域定义UNION结构:  SOC_VPP_VHDACM1_UNION */
#define SOC_VPP_VHDACM1_ADDR(base)                    ((base) + (0x01A4))

/* 寄存器说明：ACM寄存器。
   位域定义UNION结构:  SOC_VPP_VHDACM2_UNION */
#define SOC_VPP_VHDACM2_ADDR(base)                    ((base) + (0x01A8))

/* 寄存器说明：ACM寄存器。
   位域定义UNION结构:  SOC_VPP_VHDACM3_UNION */
#define SOC_VPP_VHDACM3_ADDR(base)                    ((base) + (0x01AC))

/* 寄存器说明：ACM寄存器。
   位域定义UNION结构:  SOC_VPP_VHDACM4_UNION */
#define SOC_VPP_VHDACM4_ADDR(base)                    ((base) + (0x01B0))

/* 寄存器说明：ACM寄存器。
   位域定义UNION结构:  SOC_VPP_VHDACM5_UNION */
#define SOC_VPP_VHDACM5_ADDR(base)                    ((base) + (0x01B4))

/* 寄存器说明：ACM寄存器。
   位域定义UNION结构:  SOC_VPP_VHDACM6_UNION */
#define SOC_VPP_VHDACM6_ADDR(base)                    ((base) + (0x01B8))

/* 寄存器说明：ACM寄存器。
   位域定义UNION结构:  SOC_VPP_VHDACM7_UNION */
#define SOC_VPP_VHDACM7_ADDR(base)                    ((base) + (0x01BC))

/* 寄存器说明：水平缩放参数配置（horizontal Scaling Parameter）寄存器。为非即时寄存器。
            缩放比率＝输入宽度/输出宽度。
   位域定义UNION结构:  SOC_VPP_VHDHSP_UNION */
#define SOC_VPP_VHDHSP_ADDR(base)                     ((base) + (0x01C0))

/* 寄存器说明：亮度水平位置偏移寄存器。用于pan-scan。为非即时寄存器。
   位域定义UNION结构:  SOC_VPP_VHDHLOFFSET_UNION */
#define SOC_VPP_VHDHLOFFSET_ADDR(base)                ((base) + (0x01C4))

/* 寄存器说明：色度水平位置偏移寄存器。用于pan-scan。为非即时寄存器。
   位域定义UNION结构:  SOC_VPP_VHDHCOFFSET_UNION */
#define SOC_VPP_VHDHCOFFSET_ADDR(base)                ((base) + (0x01C8))

/* 寄存器说明：缩放参数 (vertical Scaling Parameter)寄存器。
   位域定义UNION结构:  SOC_VPP_VHDVSP_UNION */
#define SOC_VPP_VHDVSP_ADDR(base)                     ((base) + (0x01D8))

/* 寄存器说明：垂直缩放倍数 (Luma Vertical Scaling Ratio)寄存器。缩放比率＝输入高度/输出高度。为非即时寄存器。
   位域定义UNION结构:  SOC_VPP_VHDVSR_UNION */
#define SOC_VPP_VHDVSR_ADDR(base)                     ((base) + (0x01DC))

/* 寄存器说明：垂直缩放偏移（Vertical Luma Offset）寄存器。垂直缩放偏移受两个因素影响：pan-scan和重复帧时的场偏移。当没有场偏移时，vluma_offset为offset_pan-scan的最低整数位+小数位；在YCbCr422格式时vchroma_offset与vluma_offset相等；在YCbCr420格式时，vchroma_offset = scaling_chroma/2-0.25。当需要场偏移时（如静帧或者重复帧），假设重复底场，则顶场配置的vluma_offset和vchroma_offset与上述无偏移时相同，底场配置需要考虑场偏移。
   位域定义UNION结构:  SOC_VPP_VHDVOFFSET_UNION */
#define SOC_VPP_VHDVOFFSET_ADDR(base)                 ((base) + (0x01E0))

/* 寄存器说明：缩放单元的输出分辨率寄存器，为非即时寄存器。
   位域定义UNION结构:  SOC_VPP_VHDZMEORESO_UNION */
#define SOC_VPP_VHDZMEORESO_ADDR(base)                ((base) + (0x01E4))

/* 寄存器说明：缩放单元的输出入辨率寄存器。为非即时寄存器。
   位域定义UNION结构:  SOC_VPP_VHDZMEIRESO_UNION */
#define SOC_VPP_VHDZMEIRESO_ADDR(base)                ((base) + (0x01E8))

/* 寄存器说明：VHD通道 ACC运算门限1
   位域定义UNION结构:  SOC_VPP_VHDACCTHD1_UNION */
#define SOC_VPP_VHDACCTHD1_ADDR(base)                 ((base) + (0x0200))

/* 寄存器说明：VHD通道 ACC运算门限2
   位域定义UNION结构:  SOC_VPP_VHDACCTHD2_UNION */
#define SOC_VPP_VHDACCTHD2_ADDR(base)                 ((base) + (0x0204))

/* 寄存器说明：ACC查找表，亮度等级为low。n=1~3
   位域定义UNION结构:  SOC_VPP_VHDACCLOWN_UNION */
#define SOC_VPP_VHDACCLOWN_ADDR(base)                 ((base) + (0x0210～0x0218))

/* 寄存器说明：ACC查找表，亮度等级为middle。n=1~3
   位域定义UNION结构:  SOC_VPP_VHDACCMEDN_UNION */
#define SOC_VPP_VHDACCMEDN_ADDR(base)                 ((base) + (0x0220～0x0228))

/* 寄存器说明：ACC查找表，亮度等级为high。n=1~3
   位域定义UNION结构:  SOC_VPP_VHDACCHIGHN_UNION */
#define SOC_VPP_VHDACCHIGHN_ADDR(base)                ((base) + (0x0230～0x0238))

/* 寄存器说明：ACC查找表，亮度等级为middle_low。n=1~3
   位域定义UNION结构:  SOC_VPP_VHDACCMLN_UNION */
#define SOC_VPP_VHDACCMLN_ADDR(base)                  ((base) + (0x0240～0x0248))

/* 寄存器说明：ACC查找表，亮度等级为middle_high。n=1~3
   位域定义UNION结构:  SOC_VPP_VHDACCMHN_UNION */
#define SOC_VPP_VHDACCMHN_ADDR(base)                  ((base) + (0x0250～0x0258))

/* 寄存器说明：ACC三区域的低门限亮度统计值，为只读寄存器
   位域定义UNION结构:  SOC_VPP_VHDACC3LOW_UNION */
#define SOC_VPP_VHDACC3LOW_ADDR(base)                 ((base) + (0x0260))

/* 寄存器说明：ACC三区域的中门限亮度统计值，为只读寄存器
   位域定义UNION结构:  SOC_VPP_VHDACC3MED_UNION */
#define SOC_VPP_VHDACC3MED_ADDR(base)                 ((base) + (0x0264))

/* 寄存器说明：ACC三区域的高门限亮度统计值，为只读寄存器
   位域定义UNION结构:  SOC_VPP_VHDACC3HIGH_UNION */
#define SOC_VPP_VHDACC3HIGH_ADDR(base)                ((base) + (0x0268))

/* 寄存器说明：ACC中间区域分成8段后，低门限亮度统计值，为只读寄存器
   位域定义UNION结构:  SOC_VPP_VHDACC8MLOW_UNION */
#define SOC_VPP_VHDACC8MLOW_ADDR(base)                ((base) + (0x026C))

/* 寄存器说明：ACC中间区域分成8段后，高门限亮度统计值，为只读寄存器
   位域定义UNION结构:  SOC_VPP_VHDACC8MHIGH_UNION */
#define SOC_VPP_VHDACC8MHIGH_ADDR(base)               ((base) + (0x0270))

/* 寄存器说明：ACC像素统计值，为只读寄存器
   位域定义UNION结构:  SOC_VPP_VHDACCTOTAL_UNION */
#define SOC_VPP_VHDACCTOTAL_ADDR(base)                ((base) + (0x0274))

/* 寄存器说明：IFIR滤波系数0和1
   位域定义UNION结构:  SOC_VPP_VHDIFIRCOEF01_UNION */
#define SOC_VPP_VHDIFIRCOEF01_ADDR(base)              ((base) + (0x0280))

/* 寄存器说明：IFIR滤波系数2和3
   位域定义UNION结构:  SOC_VPP_VHDIFIRCOEF23_UNION */
#define SOC_VPP_VHDIFIRCOEF23_ADDR(base)              ((base) + (0x0284))

/* 寄存器说明：IFIR滤波系数4和5
   位域定义UNION结构:  SOC_VPP_VHDIFIRCOEF45_UNION */
#define SOC_VPP_VHDIFIRCOEF45_ADDR(base)              ((base) + (0x0288))

/* 寄存器说明：IFIR滤波系数6和7
   位域定义UNION结构:  SOC_VPP_VHDIFIRCOEF67_UNION */
#define SOC_VPP_VHDIFIRCOEF67_ADDR(base)              ((base) + (0x028C))

/* 寄存器说明：de-interlace运算控制寄存器，为非即时寄存器。
   位域定义UNION结构:  SOC_VPP_VHDDIECTRL_UNION */
#define SOC_VPP_VHDDIECTRL_ADDR(base)                 ((base) + (0x0300))

/* 寄存器说明：de-interlace亮度运算控制寄存器0。
   位域定义UNION结构:  SOC_VPP_VHDDIELMA0_UNION */
#define SOC_VPP_VHDDIELMA0_ADDR(base)                 ((base) + (0x0308))

/* 寄存器说明：de-interlace亮度运算控制寄存器1。
   位域定义UNION结构:  SOC_VPP_VHDDIELMA1_UNION */
#define SOC_VPP_VHDDIELMA1_ADDR(base)                 ((base) + (0x030C))

/* 寄存器说明：de-interlace亮度运算控制寄存器2。
   位域定义UNION结构:  SOC_VPP_VHDDIELMA2_UNION */
#define SOC_VPP_VHDDIELMA2_ADDR(base)                 ((base) + (0x0310))

/* 寄存器说明：
   位域定义UNION结构:  SOC_VPP_VHDDIEINTEN_UNION */
#define SOC_VPP_VHDDIEINTEN_ADDR(base)                ((base) + (0x0314))

/* 寄存器说明：
   位域定义UNION结构:  SOC_VPP_VHDDIESCALE_UNION */
#define SOC_VPP_VHDDIESCALE_ADDR(base)                ((base) + (0x0318))

/* 寄存器说明：
   位域定义UNION结构:  SOC_VPP_VHDDIECHECK1_UNION */
#define SOC_VPP_VHDDIECHECK1_ADDR(base)               ((base) + (0x031C))

/* 寄存器说明：
   位域定义UNION结构:  SOC_VPP_VHDDIECHECK2_UNION */
#define SOC_VPP_VHDDIECHECK2_ADDR(base)               ((base) + (0x0320))

/* 寄存器说明：方向0～3乘数
   位域定义UNION结构:  SOC_VPP_VHDDIEDIR0_3_UNION */
#define SOC_VPP_VHDDIEDIR0_3_ADDR(base)               ((base) + (0x324))

/* 寄存器说明：方向4～7乘数
   位域定义UNION结构:  SOC_VPP_VHDDIEDIR4_7_UNION */
#define SOC_VPP_VHDDIEDIR4_7_ADDR(base)               ((base) + (0x328))

/* 寄存器说明：方向8～11乘数
   位域定义UNION结构:  SOC_VPP_VHDDIEDIR8_11_UNION */
#define SOC_VPP_VHDDIEDIR8_11_ADDR(base)              ((base) + (0x32C))

/* 寄存器说明：方向12～14乘数
   位域定义UNION结构:  SOC_VPP_VHDDIEDIR12_14_UNION */
#define SOC_VPP_VHDDIEDIR12_14_ADDR(base)             ((base) + (0x330))

/* 寄存器说明：
   位域定义UNION结构:  SOC_VPP_VHDDIESTA_UNION */
#define SOC_VPP_VHDDIESTA_ADDR(base)                  ((base) + (0x0338))

/* 寄存器说明：DIE历史信息寄存器
   位域定义UNION结构:  SOC_VPP_VHDDIESTKADDR_UNION */
#define SOC_VPP_VHDDIESTKADDR_ADDR(base)              ((base) + (0x03A4))

/* 寄存器说明：DIE历史信息寄存器
   位域定义UNION结构:  SOC_VPP_VHDDIESTLADDR_UNION */
#define SOC_VPP_VHDDIESTLADDR_ADDR(base)              ((base) + (0x03A8))

/* 寄存器说明：DIE历史信息寄存器
   位域定义UNION结构:  SOC_VPP_VHDDIESTMADDR_UNION */
#define SOC_VPP_VHDDIESTMADDR_ADDR(base)              ((base) + (0x03AC))

/* 寄存器说明：DIE历史信息寄存器
   位域定义UNION结构:  SOC_VPP_VHDDIESTNADDR_UNION */
#define SOC_VPP_VHDDIESTNADDR_ADDR(base)              ((base) + (0x03B0))

/* 寄存器说明：DIE历史信息寄存器
   位域定义UNION结构:  SOC_VPP_VHDDIESTSQTRADDR_UNION */
#define SOC_VPP_VHDDIESTSQTRADDR_ADDR(base)           ((base) + (0x03B4))

/* 寄存器说明：色度crosscolor寄存器0
   位域定义UNION结构:  SOC_VPP_VHDCCRSCLR0_UNION */
#define SOC_VPP_VHDCCRSCLR0_ADDR(base)                ((base) + (0x03B8))

/* 寄存器说明：色度crosscolor寄存器1
   位域定义UNION结构:  SOC_VPP_VHDCCRSCLR1_UNION */
#define SOC_VPP_VHDCCRSCLR1_ADDR(base)                ((base) + (0x03BC))

/* 寄存器说明：PullDown控制寄存器
   位域定义UNION结构:  SOC_VPP_VHDPDCTRL_UNION */
#define SOC_VPP_VHDPDCTRL_ADDR(base)                  ((base) + (0x0400))

/* 寄存器说明：PullDown分区座标0
   位域定义UNION结构:  SOC_VPP_VHDPDBLKPOS0_UNION */
#define SOC_VPP_VHDPDBLKPOS0_ADDR(base)               ((base) + (0x0404))

/* 寄存器说明：PullDown分区座标0
   位域定义UNION结构:  SOC_VPP_VHDPDBLKPOS1_UNION */
#define SOC_VPP_VHDPDBLKPOS1_ADDR(base)               ((base) + (0x0408))

/* 寄存器说明：PullDown静止区域门限
   位域定义UNION结构:  SOC_VPP_VHDPDBLKTHD_UNION */
#define SOC_VPP_VHDPDBLKTHD_ADDR(base)                ((base) + (0x040C))

/* 寄存器说明：PullDown直方图统计门限
   位域定义UNION结构:  SOC_VPP_VHDPDHISTTHD_UNION */
#define SOC_VPP_VHDPDHISTTHD_ADDR(base)               ((base) + (0x0410))

/* 寄存器说明：PullDown不期望运动统计门限
   位域定义UNION结构:  SOC_VPP_VHDPDUMTHD_UNION */
#define SOC_VPP_VHDPDUMTHD_ADDR(base)                 ((base) + (0x0414))

/* 寄存器说明：PullDown极性改变核心统计门限
   位域定义UNION结构:  SOC_VPP_VHDPDPCCCORING_UNION */
#define SOC_VPP_VHDPDPCCCORING_ADDR(base)             ((base) + (0x0418))

/* 寄存器说明：PullDown极性改变水平统计门限
   位域定义UNION结构:  SOC_VPP_VHDPDPCCHTHD_UNION */
#define SOC_VPP_VHDPDPCCHTHD_ADDR(base)               ((base) + (0x041C))

/* 寄存器说明：PullDown极性改变垂直统计门限
   位域定义UNION结构:  SOC_VPP_VHDPDPCCVTHD_UNION */
#define SOC_VPP_VHDPDPCCVTHD_ADDR(base)               ((base) + (0x0420))

/* 寄存器说明：Pulldown
   位域定义UNION结构:  SOC_VPP_VHDPDITDIFFVTHD_UNION */
#define SOC_VPP_VHDPDITDIFFVTHD_ADDR(base)            ((base) + (0x0424))

/* 寄存器说明：Pulldown拉丝统计门限
   位域定义UNION结构:  SOC_VPP_VHDPDLASITHD_UNION */
#define SOC_VPP_VHDPDLASITHD_ADDR(base)               ((base) + (0x0428))

/* 寄存器说明：Pulldown静止区域统计结果
   位域定义UNION结构:  SOC_VPP_VHDPDFRMITDIFF_UNION */
#define SOC_VPP_VHDPDFRMITDIFF_ADDR(base)             ((base) + (0x042C))

/* 寄存器说明：Pulldown静止区域SAD统计结果
   位域定义UNION结构:  SOC_VPP_VHDPDSTLBLKSAD_UNION */
#define SOC_VPP_VHDPDSTLBLKSAD_ADDR(base)             ((base) + (0x0430～0x046C))

/* 寄存器说明：Pulldown直方图统计结果
   位域定义UNION结构:  SOC_VPP_VHDPDHISTBIN_UNION */
#define SOC_VPP_VHDPDHISTBIN_ADDR(base)               ((base) + (0x0470～0x47C))

/* 寄存器说明：PullDown
   位域定义UNION结构:  SOC_VPP_VHDPDUMMATCH0_UNION */
#define SOC_VPP_VHDPDUMMATCH0_ADDR(base)              ((base) + (0x0480))

/* 寄存器说明：PullDown
   位域定义UNION结构:  SOC_VPP_VHDPDUMNOMATCH0_UNION */
#define SOC_VPP_VHDPDUMNOMATCH0_ADDR(base)            ((base) + (0x0484))

/* 寄存器说明：PullDown
   位域定义UNION结构:  SOC_VPP_VHDPDUMMATCH1_UNION */
#define SOC_VPP_VHDPDUMMATCH1_ADDR(base)              ((base) + (0x0488))

/* 寄存器说明：PullDown
   位域定义UNION结构:  SOC_VPP_VHDPDUMNOMATCH1_UNION */
#define SOC_VPP_VHDPDUMNOMATCH1_ADDR(base)            ((base) + (0x048C))

/* 寄存器说明：PullDown
   位域定义UNION结构:  SOC_VPP_VHDPDPCCFFWD_UNION */
#define SOC_VPP_VHDPDPCCFFWD_ADDR(base)               ((base) + (0x0490))

/* 寄存器说明：PullDown
   位域定义UNION结构:  SOC_VPP_VHDPDPCCFWD_UNION */
#define SOC_VPP_VHDPDPCCFWD_ADDR(base)                ((base) + (0x0494))

/* 寄存器说明：PullDown
   位域定义UNION结构:  SOC_VPP_VHDPDPCCBWD_UNION */
#define SOC_VPP_VHDPDPCCBWD_ADDR(base)                ((base) + (0x0498))

/* 寄存器说明：PullDown
   位域定义UNION结构:  SOC_VPP_VHDPDPCCCRSS_UNION */
#define SOC_VPP_VHDPDPCCCRSS_ADDR(base)               ((base) + (0x49C))

/* 寄存器说明：PullDown
   位域定义UNION结构:  SOC_VPP_VHDPDPCCPW_UNION */
#define SOC_VPP_VHDPDPCCPW_ADDR(base)                 ((base) + (0x4A0))

/* 寄存器说明：PullDown
   位域定义UNION结构:  SOC_VPP_VHDPDPCCFWDTKR_UNION */
#define SOC_VPP_VHDPDPCCFWDTKR_ADDR(base)             ((base) + (0x4A4))

/* 寄存器说明：PullDown
   位域定义UNION结构:  SOC_VPP_VHDPDPCCBWDTKR_UNION */
#define SOC_VPP_VHDPDPCCBWDTKR_ADDR(base)             ((base) + (0x4A8))

/* 寄存器说明：PullDown
   位域定义UNION结构:  SOC_VPP_VHDPDPCCBLKFWD_UNION */
#define SOC_VPP_VHDPDPCCBLKFWD_ADDR(base)             ((base) + (0x04AC～0x4CC))

/* 寄存器说明：PullDown
   位域定义UNION结构:  SOC_VPP_VHDPDPCCBLKBWD_UNION */
#define SOC_VPP_VHDPDPCCBLKBWD_ADDR(base)             ((base) + (0x04D0～0x4F0))

/* 寄存器说明：PullDown
   位域定义UNION结构:  SOC_VPP_VHDPDLASICNT14_UNION */
#define SOC_VPP_VHDPDLASICNT14_ADDR(base)             ((base) + (0x4F4))

/* 寄存器说明：PullDown
   位域定义UNION结构:  SOC_VPP_VHDPDLASICNT32_UNION */
#define SOC_VPP_VHDPDLASICNT32_ADDR(base)             ((base) + (0x4F8))

/* 寄存器说明：PullDown
   位域定义UNION结构:  SOC_VPP_VHDPDLASICNT34_UNION */
#define SOC_VPP_VHDPDLASICNT34_ADDR(base)             ((base) + (0x4FC))

/* 寄存器说明：VHD水平亮度缩放滤波系数寄存器。
            每个滤波器系数为10bit，由符号位和小数位组成，最高位为符号位，低9bit为小数位的绝对值。由于系数存放在片上Memory中，因此默认值为不定态。
            VHD水平亮度缩放滤波为8阶32相位，由于对称关系，共存储17组系数，即0～16相位的8阶系数。每个寄存器地址包含2个10比特系数，因此一个相位的8阶系数需要4个32比特寄存器地址。
   位域定义UNION结构:  SOC_VPP_VHDHLCOEF_UNION */
#define SOC_VPP_VHDHLCOEF_ADDR(base)                  ((base) + (0x2000～0x210C))

/* 寄存器说明：VHD水平色度缩放滤波系数寄存器。存储格式参考VHDHLCOEF。VHD水平色度缩放滤波为4阶32相位，由于对称关系，共存储17组系数，即0～16相位的4阶系数。每个寄存器地址包含2个10比特系数，因此一个相位的4阶系数需要2个32比特寄存器地址。
   位域定义UNION结构:  SOC_VPP_VHDHCCOEF_UNION */
#define SOC_VPP_VHDHCCOEF_ADDR(base)                  ((base) + (0x2200～0x2284))

/* 寄存器说明：VHD垂直亮度缩放滤波系数寄存器。存储格式参考VHDHLCOEF。VHD垂直亮度缩放滤波为6阶32相位，由于对称关系，共存储17组系数，即0～16相位的4阶系数。每个寄存器地址包含2个10比特系数，一个相位的6阶系数需要4个32比特寄存器地址。
   位域定义UNION结构:  SOC_VPP_VHDVLCOEF_UNION */
#define SOC_VPP_VHDVLCOEF_ADDR(base)                  ((base) + (0x2400～0x250C))

/* 寄存器说明：VHD垂直色度缩放滤波系数寄存器。存储格式参考VHDHLCOEF。VHD垂直色度缩放滤波为4阶32相位，由于对称关系，共存储17组系数，即0～16相位的4阶系数。每个寄存器地址包含2个10比特系数，因此一个相位的4阶系数需要2个32比特寄存器地址。
   位域定义UNION结构:  SOC_VPP_VHDVCCOEF_UNION */
#define SOC_VPP_VHDVCCOEF_ADDR(base)                  ((base) + (0x2600～0x2684))





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
 结构名    : SOC_VPP_VOCTRL_UNION
 结构说明  : VOCTRL 寄存器结构定义。地址偏移量:0x0000，初值:0x00000000，宽度:32
 寄存器说明: VPP控制寄存器。surface总线申请仲裁模式。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  reserved_0  : 4;  /* bit[0-3]  : 保留。 */
        unsigned int  outstd_rid1 : 4;  /* bit[4-7]  : AXI总线读ID1的outstanding。 */
        unsigned int  outstd_rid0 : 4;  /* bit[8-11] : AXI总线读ID0的outstanding。 */
        unsigned int  vo_id_sel   : 1;  /* bit[12]   : VPP VHD ID选择
                                                       0：VHD选择实时模式ID号（0）；&#13;1：VHD选择离线模式ID号（1）。 */
        unsigned int  reserved_1  : 3;  /* bit[13-15]: 保留。 */
        unsigned int  outstd_wid0 : 4;  /* bit[16-19]: AXI总线写ID0的outstanding */
        unsigned int  bus_dbg_en  : 2;  /* bit[20-21]: 总线测试使能
                                                       00：总线正常工作模式；
                                                       &#13;01：总线测试读写环回工作模式；&#13;10：总线测试写环回工作；
                                                       &#13;其他：保留。 */
        unsigned int  reserved_2  : 9;  /* bit[22-30]: 保留。 */
        unsigned int  vo_ck_gt_en : 1;  /* bit[31]   : VPP时钟门控使能：
                                                       0：时钟门控关闭
                                                       1：VPP的内部时钟门控打开 */
    } reg;
} SOC_VPP_VOCTRL_UNION;
#endif
#define SOC_VPP_VOCTRL_outstd_rid1_START  (4)
#define SOC_VPP_VOCTRL_outstd_rid1_END    (7)
#define SOC_VPP_VOCTRL_outstd_rid0_START  (8)
#define SOC_VPP_VOCTRL_outstd_rid0_END    (11)
#define SOC_VPP_VOCTRL_vo_id_sel_START    (12)
#define SOC_VPP_VOCTRL_vo_id_sel_END      (12)
#define SOC_VPP_VOCTRL_outstd_wid0_START  (16)
#define SOC_VPP_VOCTRL_outstd_wid0_END    (19)
#define SOC_VPP_VOCTRL_bus_dbg_en_START   (20)
#define SOC_VPP_VOCTRL_bus_dbg_en_END     (21)
#define SOC_VPP_VOCTRL_vo_ck_gt_en_START  (31)
#define SOC_VPP_VOCTRL_vo_ck_gt_en_END    (31)


/*****************************************************************************
 结构名    : SOC_VPP_VOINTSTA_UNION
 结构说明  : VOINTSTA 寄存器结构定义。地址偏移量:0x0004，初值:0x00000000，宽度:32
 寄存器说明: VPP中断状态，只读寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  reserved_0   : 9;  /* bit[0-8]  : 保留。 */
        unsigned int  vte_int      : 1;  /* bit[9]    : VHD任务完成中断。WBC1任务完成中断 */
        unsigned int  reserved_1   : 2;  /* bit[10-11]: 保留。 */
        unsigned int  lnk_tend_int : 1;  /* bit[12]   : 链表任务完成中断 */
        unsigned int  reserved_2   : 9;  /* bit[13-21]: 保留。 */
        unsigned int  vhdrr_int    : 1;  /* bit[22]   : VHD寄存器更新中断。 */
        unsigned int  reserved_3   : 8;  /* bit[23-30]: 保留。 */
        unsigned int  be_int       : 1;  /* bit[31]   : 总线错误中断。（AXI_Master） */
    } reg;
} SOC_VPP_VOINTSTA_UNION;
#endif
#define SOC_VPP_VOINTSTA_vte_int_START       (9)
#define SOC_VPP_VOINTSTA_vte_int_END         (9)
#define SOC_VPP_VOINTSTA_lnk_tend_int_START  (12)
#define SOC_VPP_VOINTSTA_lnk_tend_int_END    (12)
#define SOC_VPP_VOINTSTA_vhdrr_int_START     (22)
#define SOC_VPP_VOINTSTA_vhdrr_int_END       (22)
#define SOC_VPP_VOINTSTA_be_int_START        (31)
#define SOC_VPP_VOINTSTA_be_int_END          (31)


/*****************************************************************************
 结构名    : SOC_VPP_VOMSKINTSTA_UNION
 结构说明  : VOMSKINTSTA 寄存器结构定义。地址偏移量:0x0008，初值:0x00000000，宽度:32
 寄存器说明: VPP经过Mask的中断状态寄存器。写1清零。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  reserved_0   : 9;  /* bit[0-8]  : 保留。 */
        unsigned int  vte_int      : 1;  /* bit[9]    : VHD任务完成中断。 */
        unsigned int  reserved_1   : 2;  /* bit[10-11]: 保留。 */
        unsigned int  lnk_tend_int : 1;  /* bit[12]   : 链表任务完成中断 */
        unsigned int  reserved_2   : 9;  /* bit[13-21]: 保留。 */
        unsigned int  vhdrr_int    : 1;  /* bit[22]   : VHD寄存器更新中断。 */
        unsigned int  reserved_3   : 8;  /* bit[23-30]: 保留。 */
        unsigned int  be_int       : 1;  /* bit[31]   : 总线错误中断。（AXI_Master） */
    } reg;
} SOC_VPP_VOMSKINTSTA_UNION;
#endif
#define SOC_VPP_VOMSKINTSTA_vte_int_START       (9)
#define SOC_VPP_VOMSKINTSTA_vte_int_END         (9)
#define SOC_VPP_VOMSKINTSTA_lnk_tend_int_START  (12)
#define SOC_VPP_VOMSKINTSTA_lnk_tend_int_END    (12)
#define SOC_VPP_VOMSKINTSTA_vhdrr_int_START     (22)
#define SOC_VPP_VOMSKINTSTA_vhdrr_int_END       (22)
#define SOC_VPP_VOMSKINTSTA_be_int_START        (31)
#define SOC_VPP_VOMSKINTSTA_be_int_END          (31)


/*****************************************************************************
 结构名    : SOC_VPP_VOINTMSK_UNION
 结构说明  : VOINTMSK 寄存器结构定义。地址偏移量:0x000C，初值:0x00000000，宽度:32
 寄存器说明: VPP中断屏蔽寄存器。与VOINTSTA对应。相应比特为1表示中断打开，为0表示中断屏蔽。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  reserved_0      : 9;  /* bit[0-8]  : 保留。 */
        unsigned int  vte_intmsk      : 1;  /* bit[9]    : VHD任务完成中断。 */
        unsigned int  reserved_1      : 2;  /* bit[10-11]: 保留。 */
        unsigned int  lnk_tend_intmsk : 1;  /* bit[12]   : 链表任务完成中断 */
        unsigned int  reserved_2      : 9;  /* bit[13-21]: 保留。 */
        unsigned int  vhdrr_intmsk    : 1;  /* bit[22]   : VHD寄存器更新中断。 */
        unsigned int  reserved_3      : 8;  /* bit[23-30]: 保留。 */
        unsigned int  be_intmsk       : 1;  /* bit[31]   : 总线错误中断。（AXI_Master） */
    } reg;
} SOC_VPP_VOINTMSK_UNION;
#endif
#define SOC_VPP_VOINTMSK_vte_intmsk_START       (9)
#define SOC_VPP_VOINTMSK_vte_intmsk_END         (9)
#define SOC_VPP_VOINTMSK_lnk_tend_intmsk_START  (12)
#define SOC_VPP_VOINTMSK_lnk_tend_intmsk_END    (12)
#define SOC_VPP_VOINTMSK_vhdrr_intmsk_START     (22)
#define SOC_VPP_VOINTMSK_vhdrr_intmsk_END       (22)
#define SOC_VPP_VOINTMSK_be_intmsk_START        (31)
#define SOC_VPP_VOINTMSK_be_intmsk_END          (31)


/*****************************************************************************
 结构名    : SOC_VPP_VOUVERSION1_UNION
 结构说明  : VOUVERSION1 寄存器结构定义。地址偏移量:0x0010，初值:0x76756F76，宽度:32
 寄存器说明: VPP版本寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  vouversion0 : 32; /* bit[0-31]: VPP版本寄存器。 */
    } reg;
} SOC_VPP_VOUVERSION1_UNION;
#endif
#define SOC_VPP_VOUVERSION1_vouversion0_START  (0)
#define SOC_VPP_VOUVERSION1_vouversion0_END    (31)


/*****************************************************************************
 结构名    : SOC_VPP_VOUVERSION2_UNION
 结构说明  : VOUVERSION2 寄存器结构定义。地址偏移量:0x0014，初值:0x30303134，宽度:32
 寄存器说明: VPP版本寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  vouversion1 : 32; /* bit[0-31]: VPP版本寄存器。 */
    } reg;
} SOC_VPP_VOUVERSION2_UNION;
#endif
#define SOC_VPP_VOUVERSION2_vouversion1_START  (0)
#define SOC_VPP_VOUVERSION2_vouversion1_END    (31)


/*****************************************************************************
 结构名    : SOC_VPP_VODEBUG_UNION
 结构说明  : VODEBUG 寄存器结构定义。地址偏移量:0x0020，初值:0x00000000，宽度:32
 寄存器说明: 仅仅用于验证环境
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  rm_en_chn    : 4;  /* bit[0-3]  :  */
        unsigned int  dhd_ff_info  : 2;  /* bit[4-5]  :  */
        unsigned int  dsd_ff_info  : 2;  /* bit[6-7]  :  */
        unsigned int  wbc0_ff_info : 2;  /* bit[8-9]  :  */
        unsigned int  wbc1_ff_info : 2;  /* bit[10-11]:  */
        unsigned int  wbc2_ff_info : 2;  /* bit[12-13]:  */
        unsigned int  wbc_mode     : 4;  /* bit[14-17]:  */
        unsigned int  node_num     : 6;  /* bit[18-23]: 环境之间传递节点个数 */
        unsigned int  reserved     : 8;  /* bit[24-31]:  */
    } reg;
} SOC_VPP_VODEBUG_UNION;
#endif
#define SOC_VPP_VODEBUG_rm_en_chn_START     (0)
#define SOC_VPP_VODEBUG_rm_en_chn_END       (3)
#define SOC_VPP_VODEBUG_dhd_ff_info_START   (4)
#define SOC_VPP_VODEBUG_dhd_ff_info_END     (5)
#define SOC_VPP_VODEBUG_dsd_ff_info_START   (6)
#define SOC_VPP_VODEBUG_dsd_ff_info_END     (7)
#define SOC_VPP_VODEBUG_wbc0_ff_info_START  (8)
#define SOC_VPP_VODEBUG_wbc0_ff_info_END    (9)
#define SOC_VPP_VODEBUG_wbc1_ff_info_START  (10)
#define SOC_VPP_VODEBUG_wbc1_ff_info_END    (11)
#define SOC_VPP_VODEBUG_wbc2_ff_info_START  (12)
#define SOC_VPP_VODEBUG_wbc2_ff_info_END    (13)
#define SOC_VPP_VODEBUG_wbc_mode_START      (14)
#define SOC_VPP_VODEBUG_wbc_mode_END        (17)
#define SOC_VPP_VODEBUG_node_num_START      (18)
#define SOC_VPP_VODEBUG_node_num_END        (23)


/*****************************************************************************
 结构名    : SOC_VPP_VOLNKLSTCTRL_UNION
 结构说明  : VOLNKLSTCTRL 寄存器结构定义。地址偏移量:0x0024，初值:0x00000000，宽度:32
 寄存器说明: VPP链表控制寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  lnk_fst_num      : 8;  /* bit[0-7]  : 表示链表的第一个节点的需要更新的WORD数，比如，需要更新10个寄存器，需要4个WORD，加上1个WORD的下一个节点信息，占用一个WORD，所以配置5个WORD；配置值为实际值减1 */
        unsigned int  lnk_fst_node_num : 10; /* bit[8-17] : 链表的第一个节点的节点号 */
        unsigned int  reserved         : 13; /* bit[18-30]:  */
        unsigned int  lnk_en           : 1;  /* bit[31]   : 链表使能，即时寄存器
                                                            0：禁止
                                                            1：使能 */
    } reg;
} SOC_VPP_VOLNKLSTCTRL_UNION;
#endif
#define SOC_VPP_VOLNKLSTCTRL_lnk_fst_num_START       (0)
#define SOC_VPP_VOLNKLSTCTRL_lnk_fst_num_END         (7)
#define SOC_VPP_VOLNKLSTCTRL_lnk_fst_node_num_START  (8)
#define SOC_VPP_VOLNKLSTCTRL_lnk_fst_node_num_END    (17)
#define SOC_VPP_VOLNKLSTCTRL_lnk_en_START            (31)
#define SOC_VPP_VOLNKLSTCTRL_lnk_en_END              (31)


/*****************************************************************************
 结构名    : SOC_VPP_VOLNKLSTADDR_UNION
 结构说明  : VOLNKLSTADDR 寄存器结构定义。地址偏移量:0x0028，初值:0x00000000，宽度:32
 寄存器说明: VPP链表首地址寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  lnk_addr : 32; /* bit[0-31]: 链表首地址byte为单位，word对齐 */
    } reg;
} SOC_VPP_VOLNKLSTADDR_UNION;
#endif
#define SOC_VPP_VOLNKLSTADDR_lnk_addr_START  (0)
#define SOC_VPP_VOLNKLSTADDR_lnk_addr_END    (31)


/*****************************************************************************
 结构名    : SOC_VPP_VOLNKLSTSTRT_UNION
 结构说明  : VOLNKLSTSTRT 寄存器结构定义。地址偏移量:0x002C，初值:0x00000000，宽度:32
 寄存器说明: VPP链表开始寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  reserved  : 31; /* bit[0-30]: 保留。 */
        unsigned int  lnk_start : 1;  /* bit[31]  : 链表开始
                                                    0：禁止
                                                    1：使能 */
    } reg;
} SOC_VPP_VOLNKLSTSTRT_UNION;
#endif
#define SOC_VPP_VOLNKLSTSTRT_lnk_start_START  (31)
#define SOC_VPP_VOLNKLSTSTRT_lnk_start_END    (31)


/*****************************************************************************
 结构名    : SOC_VPP_VOLNKLSTPENDINGSTA_UNION
 结构说明  : VOLNKLSTPENDINGSTA 寄存器结构定义。地址偏移量:0x0030，初值:0x00000000，宽度:32
 寄存器说明: VPP链表pending状态寄存器1
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  addr_pendig : 32; /* bit[0-31]: 链表的Pending节点的首地址，
                                                      只读 */
    } reg;
} SOC_VPP_VOLNKLSTPENDINGSTA_UNION;
#endif
#define SOC_VPP_VOLNKLSTPENDINGSTA_addr_pendig_START  (0)
#define SOC_VPP_VOLNKLSTPENDINGSTA_addr_pendig_END    (31)


/*****************************************************************************
 结构名    : SOC_VPP_VOLNKLSTWORKINGSTA_UNION
 结构说明  : VOLNKLSTWORKINGSTA 寄存器结构定义。地址偏移量:0x0034，初值:0x00000000，宽度:32
 寄存器说明: VPP链表Working状态寄存器1
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  addr_working : 32; /* bit[0-31]: 链表的正在工作的节点的首地址，
                                                       只读 */
    } reg;
} SOC_VPP_VOLNKLSTWORKINGSTA_UNION;
#endif
#define SOC_VPP_VOLNKLSTWORKINGSTA_addr_working_START  (0)
#define SOC_VPP_VOLNKLSTWORKINGSTA_addr_working_END    (31)


/*****************************************************************************
 结构名    : SOC_VPP_VOLNKLSTENDSTA_UNION
 结构说明  : VOLNKLSTENDSTA 寄存器结构定义。地址偏移量:0x0038，初值:0x00000000，宽度:32
 寄存器说明: VPP链表end状态寄存器1
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  addr_end : 32; /* bit[0-31]: 链表的刚工作完成节点的首地址，
                                                   只读 */
    } reg;
} SOC_VPP_VOLNKLSTENDSTA_UNION;
#endif
#define SOC_VPP_VOLNKLSTENDSTA_addr_end_START  (0)
#define SOC_VPP_VOLNKLSTENDSTA_addr_end_END    (31)


/*****************************************************************************
 结构名    : SOC_VPP_VOLNKLSTNUMSTA_UNION
 结构说明  : VOLNKLSTNUMSTA 寄存器结构定义。地址偏移量:0x003C，初值:0x00000000，宽度:32
 寄存器说明: VPP链表节点号状态寄存器寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  node_num_pending : 8;  /* bit[0-7]  : 下一个要执行的任务的节点号 */
        unsigned int  node_num_working : 8;  /* bit[8-15] : 正在工作的任务的节点号 */
        unsigned int  node_num_end     : 8;  /* bit[16-23]: 最近工作完成的节点号 */
        unsigned int  reserved         : 8;  /* bit[24-31]: 保留。 */
    } reg;
} SOC_VPP_VOLNKLSTNUMSTA_UNION;
#endif
#define SOC_VPP_VOLNKLSTNUMSTA_node_num_pending_START  (0)
#define SOC_VPP_VOLNKLSTNUMSTA_node_num_pending_END    (7)
#define SOC_VPP_VOLNKLSTNUMSTA_node_num_working_START  (8)
#define SOC_VPP_VOLNKLSTNUMSTA_node_num_working_END    (15)
#define SOC_VPP_VOLNKLSTNUMSTA_node_num_end_START      (16)
#define SOC_VPP_VOLNKLSTNUMSTA_node_num_end_END        (23)


/*****************************************************************************
 结构名    : SOC_VPP_VOPARAUP_UNION
 结构说明  : VOPARAUP 寄存器结构定义。地址偏移量:0x0040，初值:0x00000000，宽度:32
 寄存器说明: 缩放系数、LUT相关的寄存器更新使能寄存器。VPP的缩放系数是通过AXI Master配置的，软件通过Slave配置起始地址和参数需要更新的标志。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  vhd_hcoef_upd : 1;  /* bit[0]   : VHD水平亮度、水平色度滤波系数是否需要更新。硬件更新后自动清零。
                                                        0：不更新；&#13;1：更新。 */
        unsigned int  vhd_vcoef_upd : 1;  /* bit[1]   : VHD垂直亮度、垂直色度滤波系数是否需要更新。硬件更新后自动清零。
                                                        0：不更新；&#13;1：更新。 */
        unsigned int  reserved_0    : 2;  /* bit[2-3] : 保留。 */
        unsigned int  video_acc_upd : 1;  /* bit[4]   : Video层 ACC运算查找表是否需要更新。硬件更新后自动清零。
                                                        0：不更新；&#13;1：更新。 */
        unsigned int  reserved_1    : 27; /* bit[5-31]: 保留。 */
    } reg;
} SOC_VPP_VOPARAUP_UNION;
#endif
#define SOC_VPP_VOPARAUP_vhd_hcoef_upd_START  (0)
#define SOC_VPP_VOPARAUP_vhd_hcoef_upd_END    (0)
#define SOC_VPP_VOPARAUP_vhd_vcoef_upd_START  (1)
#define SOC_VPP_VOPARAUP_vhd_vcoef_upd_END    (1)
#define SOC_VPP_VOPARAUP_video_acc_upd_START  (4)
#define SOC_VPP_VOPARAUP_video_acc_upd_END    (4)


/*****************************************************************************
 结构名    : SOC_VPP_VHDHCOEFAD_UNION
 结构说明  : VHDHCOEFAD 寄存器结构定义。地址偏移量:0x0044，初值:0x00000000，宽度:32
 寄存器说明: VHD水平亮度、水平色度滤波系数地址寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  coef_addr : 32; /* bit[0-31]: 系数放在Local Memory的起始地址。 */
    } reg;
} SOC_VPP_VHDHCOEFAD_UNION;
#endif
#define SOC_VPP_VHDHCOEFAD_coef_addr_START  (0)
#define SOC_VPP_VHDHCOEFAD_coef_addr_END    (31)


/*****************************************************************************
 结构名    : SOC_VPP_VHDVCOEFAD_UNION
 结构说明  : VHDVCOEFAD 寄存器结构定义。地址偏移量:0x0048，初值:0x00000000，宽度:32
 寄存器说明: VHD垂直亮度、水平色度滤波系数地址寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  coef_addr : 32; /* bit[0-31]: 系数放在Local Memory的起始地址。 */
    } reg;
} SOC_VPP_VHDVCOEFAD_UNION;
#endif
#define SOC_VPP_VHDVCOEFAD_coef_addr_START  (0)
#define SOC_VPP_VHDVCOEFAD_coef_addr_END    (31)


/*****************************************************************************
 结构名    : SOC_VPP_VADHCOEFAD_UNION
 结构说明  : VADHCOEFAD 寄存器结构定义。地址偏移量:0x004C，初值:0x00000000，宽度:32
 寄存器说明: VAD水平亮度、水平色度滤波系数地址寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  coef_addr : 32; /* bit[0-31]: 系数放在Local Memory的起始地址。 */
    } reg;
} SOC_VPP_VADHCOEFAD_UNION;
#endif
#define SOC_VPP_VADHCOEFAD_coef_addr_START  (0)
#define SOC_VPP_VADHCOEFAD_coef_addr_END    (31)


/*****************************************************************************
 结构名    : SOC_VPP_VADVCOEFAD_UNION
 结构说明  : VADVCOEFAD 寄存器结构定义。地址偏移量:0x0050，初值:0x00000000，宽度:32
 寄存器说明: VAD垂直亮度、水平色度滤波系数地址寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  coef_addr : 32; /* bit[0-31]: 系数放在Local Memory的起始地址。 */
    } reg;
} SOC_VPP_VADVCOEFAD_UNION;
#endif
#define SOC_VPP_VADVCOEFAD_coef_addr_START  (0)
#define SOC_VPP_VADVCOEFAD_coef_addr_END    (31)


/*****************************************************************************
 结构名    : SOC_VPP_ACCAD_UNION
 结构说明  : ACCAD 寄存器结构定义。地址偏移量:0x0054，初值:0x00000000，宽度:32
 寄存器说明: video通道ACC系数查找表地址寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  coef_addr : 32; /* bit[0-31]: 系数放在Local Memory的起始地址。 */
    } reg;
} SOC_VPP_ACCAD_UNION;
#endif
#define SOC_VPP_ACCAD_coef_addr_START  (0)
#define SOC_VPP_ACCAD_coef_addr_END    (31)


/*****************************************************************************
 结构名    : SOC_VPP_VHDCTRL_UNION
 结构说明  : VHDCTRL 寄存器结构定义。地址偏移量:0x0100，初值:0x00000000，宽度:32
 寄存器说明: 该寄存器可以配置层的相关信息，为非即时寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ifmt          : 4;  /* bit[0-3]  : 输入数据格式。
                                                         0x1：SPYCbCr400；
                                                         &#13;0x2：保留；
                                                         0x3：SPYCbCr420；
                                                         &#13;0x4：SPYCbCr422；
                                                         &#13;0x5：SPYCbCr444；
                                                         0x9：PackageUYVY(Cb0Y0Cr0Y1)；（仅VDC_HD不支持）；
                                                         0xA：PackageYUYV(Y0Cb0Y1Cr0)；（仅VDC_HD不支持）；
                                                         0xB：PackageYVYU(Y0Cr0Y1Cb0)；（仅VDC_HD不支持）。 */
        unsigned int  time_out      : 4;  /* bit[4-7]  :  */
        unsigned int  reserved_0    : 4;  /* bit[8-11] : 保留。 */
        unsigned int  chm_rmode     : 2;  /* bit[12-13]: 色度逐隔行读取模式。
                                                         00：保留；&#13;01：逐行读取帧buffer数据；&#13;10：隔行读取时，读入顶场（die使能时表示为当前场）；&#13;11：隔行读取时，读入底场（die使能时表示为当前场）。 */
        unsigned int  lm_rmode      : 2;  /* bit[14-15]: 亮度逐隔行读取模式。
                                                         00：保留；&#13;01：逐行读取帧buffer数据；&#13;10：隔行读取时，读入顶场（die使能时表示为当前场）；&#13;11：隔行读取时，读入底场（die使能时表示为当前场）。 */
        unsigned int  bfield_first  : 1;  /* bit[16]   : 底场优先（T：Top Field；B：Bottom Field) 。
                                                         0：顶场优先，T0B0T1B1...（T0B0为一帧）；
                                                         1：底场优先，B0T0B1T0...（B0T0为一帧）。 */
        unsigned int  vup_mode      : 1;  /* bit[17]   : VHD系数更新模式。
                                                         0：帧更新；
                                                         1：场更新； */
        unsigned int  ifir_mode     : 2;  /* bit[18-19]: 水平色度IFIR模式。
                                                         00：保留。
                                                         01：色度IFIR拷贝模式；
                                                         &#13;10：双线性插值；
                                                         11：6阶FIR。 */
        unsigned int  reserved_1    : 3;  /* bit[20-22]: 保留。 */
        unsigned int  vhd_sta_wr_en : 1;  /* bit[23]   : vhd的统计信息(die and acc)回写DDR使能控制
                                                         0：禁止
                                                         1：使能 */
        unsigned int  ofl_btm       : 1;  /* bit[24]   : VHD离线时，WBC1在VHD的数据提取点上隔行输出时的顶底场信息。
                                                         0：表示顶场；&#13;1：表示底场。 */
        unsigned int  ofl_inter     : 1;  /* bit[25]   : VHD离线时，WBC1在VHD的数据提取点上逐隔行信息。
                                                         0：表示逐行；&#13;1：表示隔行。 */
        unsigned int  wbc1_int_en   : 1;  /* bit[26]   : WBC1中断产生使能控制 */
        unsigned int  wbc1_en       : 1;  /* bit[27]   : VHD的回写使能，非即时寄存器。
                                                         0：禁止；&#13;1：使能。 */
        unsigned int  mute_en       : 1;  /* bit[28]   : VHD的mute使能，非即时寄存器。
                                                         0：禁止；&#13;1：使能。 */
        unsigned int  reserved_2    : 1;  /* bit[29]   : 保留。 */
        unsigned int  resource_sel  : 1;  /* bit[30]   : VHD输出给WBC0的通道模式配置。
                                                         0：在线模式（与显示时序同步）； &#13;1：离线模式（与显示时序无关）。&#13;注：该bit为即使寄存器，在配置改变该寄存器时，应该是WBC和VHD都关闭后才可以。&#13;VSD寄存器时：&#13;VSD的缩放连接控制（dfp_zme）。&#13;0：VSD的缩放连接到VSD上；&#13;1：VSD的缩放连接到WBC0上。 */
        unsigned int  surface_en    : 1;  /* bit[31]   : surface使能。非即时寄存器。
                                                         0：禁止；&#13;1：使能。 */
    } reg;
} SOC_VPP_VHDCTRL_UNION;
#endif
#define SOC_VPP_VHDCTRL_ifmt_START           (0)
#define SOC_VPP_VHDCTRL_ifmt_END             (3)
#define SOC_VPP_VHDCTRL_time_out_START       (4)
#define SOC_VPP_VHDCTRL_time_out_END         (7)
#define SOC_VPP_VHDCTRL_chm_rmode_START      (12)
#define SOC_VPP_VHDCTRL_chm_rmode_END        (13)
#define SOC_VPP_VHDCTRL_lm_rmode_START       (14)
#define SOC_VPP_VHDCTRL_lm_rmode_END         (15)
#define SOC_VPP_VHDCTRL_bfield_first_START   (16)
#define SOC_VPP_VHDCTRL_bfield_first_END     (16)
#define SOC_VPP_VHDCTRL_vup_mode_START       (17)
#define SOC_VPP_VHDCTRL_vup_mode_END         (17)
#define SOC_VPP_VHDCTRL_ifir_mode_START      (18)
#define SOC_VPP_VHDCTRL_ifir_mode_END        (19)
#define SOC_VPP_VHDCTRL_vhd_sta_wr_en_START  (23)
#define SOC_VPP_VHDCTRL_vhd_sta_wr_en_END    (23)
#define SOC_VPP_VHDCTRL_ofl_btm_START        (24)
#define SOC_VPP_VHDCTRL_ofl_btm_END          (24)
#define SOC_VPP_VHDCTRL_ofl_inter_START      (25)
#define SOC_VPP_VHDCTRL_ofl_inter_END        (25)
#define SOC_VPP_VHDCTRL_wbc1_int_en_START    (26)
#define SOC_VPP_VHDCTRL_wbc1_int_en_END      (26)
#define SOC_VPP_VHDCTRL_wbc1_en_START        (27)
#define SOC_VPP_VHDCTRL_wbc1_en_END          (27)
#define SOC_VPP_VHDCTRL_mute_en_START        (28)
#define SOC_VPP_VHDCTRL_mute_en_END          (28)
#define SOC_VPP_VHDCTRL_resource_sel_START   (30)
#define SOC_VPP_VHDCTRL_resource_sel_END     (30)
#define SOC_VPP_VHDCTRL_surface_en_START     (31)
#define SOC_VPP_VHDCTRL_surface_en_END       (31)


/*****************************************************************************
 结构名    : SOC_VPP_VHDUPD_UNION
 结构说明  : VHDUPD 寄存器结构定义。地址偏移量:0x0104，初值:0x00000000，宽度:32
 寄存器说明: VHD通道更新使能寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  regup    : 1;  /* bit[0]   : surface的寄存器更新。当软件配置完该层寄存器后写1更新，更新完成后，硬件自动清零。 */
        unsigned int  reserved : 31; /* bit[1-31]: 保留。 */
    } reg;
} SOC_VPP_VHDUPD_UNION;
#endif
#define SOC_VPP_VHDUPD_regup_START     (0)
#define SOC_VPP_VHDUPD_regup_END       (0)


/*****************************************************************************
 结构名    : SOC_VPP_VHDLADDR_UNION
 结构说明  : VHDLADDR 寄存器结构定义。地址偏移量:0x0108，初值:0x00000000，宽度:32
 寄存器说明: 上一帧的地址寄存器。用于deinterace。对于package像素格式就是帧buffer地址；对于semi-planar像素格式是亮度帧buffer地址。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  surface_laddr : 32; /* bit[0-31]: DIE上一帧的地址。 */
    } reg;
} SOC_VPP_VHDLADDR_UNION;
#endif
#define SOC_VPP_VHDLADDR_surface_laddr_START  (0)
#define SOC_VPP_VHDLADDR_surface_laddr_END    (31)


/*****************************************************************************
 结构名    : SOC_VPP_VHDLCADDR_UNION
 结构说明  : VHDLCADDR 寄存器结构定义。地址偏移量:0x010C，初值:0x00000000，宽度:32
 寄存器说明: 上一帧的色度地址寄存器。用于deinterace。对于package像素格式该地址无效；对于semi-planar像素格式是色度帧buffer地址。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  surface_lcaddr : 32; /* bit[0-31]: DIE上一帧的色度地址。 */
    } reg;
} SOC_VPP_VHDLCADDR_UNION;
#endif
#define SOC_VPP_VHDLCADDR_surface_lcaddr_START  (0)
#define SOC_VPP_VHDLCADDR_surface_lcaddr_END    (31)


/*****************************************************************************
 结构名    : SOC_VPP_VHDCADDR_UNION
 结构说明  : VHDCADDR 寄存器结构定义。地址偏移量:0x0110，初值:0x00000000，宽度:32
 寄存器说明: 当前帧的地址寄存器。对于package像素格式就是帧buffer地址；对于semi-planar像素格式是亮度帧buffer地址。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  surface_caddr : 32; /* bit[0-31]: DIE当前帧的地址。 */
    } reg;
} SOC_VPP_VHDCADDR_UNION;
#endif
#define SOC_VPP_VHDCADDR_surface_caddr_START  (0)
#define SOC_VPP_VHDCADDR_surface_caddr_END    (31)


/*****************************************************************************
 结构名    : SOC_VPP_VHDCCADDR_UNION
 结构说明  : VHDCCADDR 寄存器结构定义。地址偏移量:0x0114，初值:0x00000000，宽度:32
 寄存器说明: 当前帧的色度地址寄存器。对于package像素格式该地址无效；对于semi-planar像素格式是色度帧buffer地址。DIE当前帧的地址。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  surface_ccaddr : 32; /* bit[0-31]: DIE当前帧的色度地址。 */
    } reg;
} SOC_VPP_VHDCCADDR_UNION;
#endif
#define SOC_VPP_VHDCCADDR_surface_ccaddr_START  (0)
#define SOC_VPP_VHDCCADDR_surface_ccaddr_END    (31)


/*****************************************************************************
 结构名    : SOC_VPP_VHDNADDR_UNION
 结构说明  : VHDNADDR 寄存器结构定义。地址偏移量:0x0118，初值:0x00000000，宽度:32
 寄存器说明: 下一帧的地址寄存器。用于deinterace。对于package像素格式就是帧buffer地址；对于semi-planar像素格式是亮度帧buffer地址。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  surface_naddr : 32; /* bit[0-31]: DIE下一帧的地址。 */
    } reg;
} SOC_VPP_VHDNADDR_UNION;
#endif
#define SOC_VPP_VHDNADDR_surface_naddr_START  (0)
#define SOC_VPP_VHDNADDR_surface_naddr_END    (31)


/*****************************************************************************
 结构名    : SOC_VPP_VHDNCADDR_UNION
 结构说明  : VHDNCADDR 寄存器结构定义。地址偏移量:0x011C，初值:0x00000000，宽度:32
 寄存器说明: 下一帧的色度地址寄存器。用于deinterace。对于package像素格式该地址无效；对于semi-planar像素格式是色度帧buffer地址。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  surface_ncaddr : 32; /* bit[0-31]: DIE下一帧的色度地址。 */
    } reg;
} SOC_VPP_VHDNCADDR_UNION;
#endif
#define SOC_VPP_VHDNCADDR_surface_ncaddr_START  (0)
#define SOC_VPP_VHDNCADDR_surface_ncaddr_END    (31)


/*****************************************************************************
 结构名    : SOC_VPP_VHDSTRIDE_UNION
 结构说明  : VHDSTRIDE 寄存器结构定义。地址偏移量:0x0124，初值:0x00000000，宽度:32
 寄存器说明: surface的stride寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  surface_stride  : 16; /* bit[0-15] : 帧buffer的stride。（对于semi-planaer格式，指亮度的stride），word的为单位。 */
        unsigned int  surface_cstride : 16; /* bit[16-31]: 色度帧buffer的stride（对于semi-planaer格式有用），word的为单位。 */
    } reg;
} SOC_VPP_VHDSTRIDE_UNION;
#endif
#define SOC_VPP_VHDSTRIDE_surface_stride_START   (0)
#define SOC_VPP_VHDSTRIDE_surface_stride_END     (15)
#define SOC_VPP_VHDSTRIDE_surface_cstride_START  (16)
#define SOC_VPP_VHDSTRIDE_surface_cstride_END    (31)


/*****************************************************************************
 结构名    : SOC_VPP_VHDIRESO_UNION
 结构说明  : VHDIRESO 寄存器结构定义。地址偏移量:0x0128，初值:0x00000000，宽度:32
 寄存器说明: 输入分辨率寄存器，为非即时寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  iw       : 12; /* bit[0-11] : 宽度，像素单位。实际宽度减1。 */
        unsigned int  ih       : 12; /* bit[12-23]: 高度，行为单位。实际高度减1。
                                                    以帧高度为参考，以行为单位。 */
        unsigned int  reserved : 8;  /* bit[24-31]: 保留。 */
    } reg;
} SOC_VPP_VHDIRESO_UNION;
#endif
#define SOC_VPP_VHDIRESO_iw_START        (0)
#define SOC_VPP_VHDIRESO_iw_END          (11)
#define SOC_VPP_VHDIRESO_ih_START        (12)
#define SOC_VPP_VHDIRESO_ih_END          (23)


/*****************************************************************************
 结构名    : SOC_VPP_VHDDIEADDR_UNION
 结构说明  : VHDDIEADDR 寄存器结构定义。地址偏移量:0x0138，初值:0x00000000，宽度:32
 寄存器说明: de-interlace历史信息buffer地址，为非即时寄存器。VDC_SD没有该寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  dieaddr : 32; /* bit[0-31]: de-interlace历史信息buffer地址。 */
    } reg;
} SOC_VPP_VHDDIEADDR_UNION;
#endif
#define SOC_VPP_VHDDIEADDR_dieaddr_START  (0)
#define SOC_VPP_VHDDIEADDR_dieaddr_END    (31)


/*****************************************************************************
 结构名    : SOC_VPP_VHDWBC1ADDR_UNION
 结构说明  : VHDWBC1ADDR 寄存器结构定义。地址偏移量:0x013C，初值:0x00000000，宽度:32
 寄存器说明: VHD的数据回写buffer地址，为非即时寄存器。VDC_SD没有该寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  wbc1addr : 32; /* bit[0-31]: VHD的数据回写buffer地址。 */
    } reg;
} SOC_VPP_VHDWBC1ADDR_UNION;
#endif
#define SOC_VPP_VHDWBC1ADDR_wbc1addr_START  (0)
#define SOC_VPP_VHDWBC1ADDR_wbc1addr_END    (31)


/*****************************************************************************
 结构名    : SOC_VPP_VHDWBC1STRD_UNION
 结构说明  : VHDWBC1STRD 寄存器结构定义。地址偏移量:0x0140，初值:0x00000000，宽度:32
 寄存器说明: VHD的数据回写行间距，为非即时寄存器。VDC_SD没有该寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  wbc1strd     : 16; /* bit[0-15] : VHD的数据回写行间距。 */
        unsigned int  req_interval : 10; /* bit[16-25]: WBC0总线申请最小间隔，表示第n个申请结束到第n+1个申请之间的间隔有req_interval个时钟周期。 */
        unsigned int  reserved     : 4;  /* bit[26-29]: 保留。 */
        unsigned int  wbc1_dft     : 2;  /* bit[30-31]: WBC1的输出数据格式。
                                                        00：PackageUYVY(Cb0Y0Cr0Y1)；&#13;01：PackageYUYV(Y0Cb0Y1Cr0)；&#13;10：PackageYVYU(Y0Cr0Y1Cb0)；&#13;顺序为0～31。 */
    } reg;
} SOC_VPP_VHDWBC1STRD_UNION;
#endif
#define SOC_VPP_VHDWBC1STRD_wbc1strd_START      (0)
#define SOC_VPP_VHDWBC1STRD_wbc1strd_END        (15)
#define SOC_VPP_VHDWBC1STRD_req_interval_START  (16)
#define SOC_VPP_VHDWBC1STRD_req_interval_END    (25)
#define SOC_VPP_VHDWBC1STRD_wbc1_dft_START      (30)
#define SOC_VPP_VHDWBC1STRD_wbc1_dft_END        (31)


/*****************************************************************************
 结构名    : SOC_VPP_VHDDIESTADDR_UNION
 结构说明  : VHDDIESTADDR 寄存器结构定义。地址偏移量:0x0144，初值:0x00000000，宽度:32
 寄存器说明: de-interlace的统计信息回写地址，为非即时寄存器。VDC_SD没有该寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  diestaddr : 32; /* bit[0-31]: de-interlace历史信息buffer地址。 */
    } reg;
} SOC_VPP_VHDDIESTADDR_UNION;
#endif
#define SOC_VPP_VHDDIESTADDR_diestaddr_START  (0)
#define SOC_VPP_VHDDIESTADDR_diestaddr_END    (31)


/*****************************************************************************
 结构名    : SOC_VPP_VHDVFPOS_UNION
 结构说明  : VHDVFPOS 寄存器结构定义。地址偏移量:0x0168，初值:0x00000000，宽度:32
 寄存器说明: Surface真实内容在显示窗口的起始位置（First POSition），以像素为单位，非即时寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  video_xfpos : 12; /* bit[0-11] : 视频内容行起始坐标。 */
        unsigned int  video_yfpos : 12; /* bit[12-23]: 视频内容列起始坐标。
                                                       以帧高度为参考，以行为单位。 */
        unsigned int  reserved    : 8;  /* bit[24-31]: 保留。 */
    } reg;
} SOC_VPP_VHDVFPOS_UNION;
#endif
#define SOC_VPP_VHDVFPOS_video_xfpos_START  (0)
#define SOC_VPP_VHDVFPOS_video_xfpos_END    (11)
#define SOC_VPP_VHDVFPOS_video_yfpos_START  (12)
#define SOC_VPP_VHDVFPOS_video_yfpos_END    (23)


/*****************************************************************************
 结构名    : SOC_VPP_VHDVLPOS_UNION
 结构说明  : VHDVLPOS 寄存器结构定义。地址偏移量:0x016C，初值:0x00000000，宽度:32
 寄存器说明: Surface真实内容在显示窗口的结束位置（Last POSition），以像素为单位，非即时寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  video_xlpos : 12; /* bit[0-11] : 视频内容行结束坐标。 */
        unsigned int  video_ylpos : 12; /* bit[12-23]: 视频内容列结束坐标。
                                                       以帧高度为参考，以行为单位。 */
        unsigned int  reserved    : 8;  /* bit[24-31]: 保留。 */
    } reg;
} SOC_VPP_VHDVLPOS_UNION;
#endif
#define SOC_VPP_VHDVLPOS_video_xlpos_START  (0)
#define SOC_VPP_VHDVLPOS_video_xlpos_END    (11)
#define SOC_VPP_VHDVLPOS_video_ylpos_START  (12)
#define SOC_VPP_VHDVLPOS_video_ylpos_END    (23)


/*****************************************************************************
 结构名    : SOC_VPP_VHDBK_UNION
 结构说明  : VHDBK 寄存器结构定义。地址偏移量:0x0170，初值:0x00000000，宽度:32
 寄存器说明: 视频层的背景色寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  vbk_cr    : 8;  /* bit[0-7]  : Cr分量。 */
        unsigned int  vbk_cb    : 8;  /* bit[8-15] : Cb分量。 */
        unsigned int  vbk_y     : 8;  /* bit[16-23]: Y分量。 */
        unsigned int  vbk_alpha : 8;  /* bit[24-31]: 视频层的背景填充色0～128级。 */
    } reg;
} SOC_VPP_VHDBK_UNION;
#endif
#define SOC_VPP_VHDBK_vbk_cr_START     (0)
#define SOC_VPP_VHDBK_vbk_cr_END       (7)
#define SOC_VPP_VHDBK_vbk_cb_START     (8)
#define SOC_VPP_VHDBK_vbk_cb_END       (15)
#define SOC_VPP_VHDBK_vbk_y_START      (16)
#define SOC_VPP_VHDBK_vbk_y_END        (23)
#define SOC_VPP_VHDBK_vbk_alpha_START  (24)
#define SOC_VPP_VHDBK_vbk_alpha_END    (31)


/*****************************************************************************
 结构名    : SOC_VPP_VHDCSCIDC_UNION
 结构说明  : VHDCSCIDC 寄存器结构定义。地址偏移量:0x0180，初值:0x00000000，宽度:32
 寄存器说明: 色彩空间转换输入直流分量寄存器，为即时寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  cscidc0  : 9;  /* bit[0-8]  : 输入分量0直流参数，MSB为符号位。补码表示。 */
        unsigned int  cscidc1  : 9;  /* bit[9-17] : 输入分量1直流参数，MSB为符号位。补码表示。 */
        unsigned int  cscidc2  : 9;  /* bit[18-26]: 输入分量2直流参数，MSB为符号位。补码表示。 */
        unsigned int  csc_en   : 1;  /* bit[27]   : CSC使能控制信号。
                                                    0：CSC不使能；&#13;1：CSC使能。 */
        unsigned int  reserved : 4;  /* bit[28-31]: 保留。 */
    } reg;
} SOC_VPP_VHDCSCIDC_UNION;
#endif
#define SOC_VPP_VHDCSCIDC_cscidc0_START   (0)
#define SOC_VPP_VHDCSCIDC_cscidc0_END     (8)
#define SOC_VPP_VHDCSCIDC_cscidc1_START   (9)
#define SOC_VPP_VHDCSCIDC_cscidc1_END     (17)
#define SOC_VPP_VHDCSCIDC_cscidc2_START   (18)
#define SOC_VPP_VHDCSCIDC_cscidc2_END     (26)
#define SOC_VPP_VHDCSCIDC_csc_en_START    (27)
#define SOC_VPP_VHDCSCIDC_csc_en_END      (27)


/*****************************************************************************
 结构名    : SOC_VPP_VHDCSCODC_UNION
 结构说明  : VHDCSCODC 寄存器结构定义。地址偏移量:0x0184，初值:0x00000000，宽度:32
 寄存器说明: 色彩空间转换输出直流分量寄存器，为即时寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  cscodc0  : 9;  /* bit[0-8]  : 输出分量0直流参数，MSB为符号位。补码表示。 */
        unsigned int  cscodc1  : 9;  /* bit[9-17] : 输出分量1直流参数，MSB为符号位。补码表示。 */
        unsigned int  cscodc2  : 9;  /* bit[18-26]: 输出分量2直流参数，MSB为符号位。补码表示。 */
        unsigned int  reserved : 5;  /* bit[27-31]: 保留。 */
    } reg;
} SOC_VPP_VHDCSCODC_UNION;
#endif
#define SOC_VPP_VHDCSCODC_cscodc0_START   (0)
#define SOC_VPP_VHDCSCODC_cscodc0_END     (8)
#define SOC_VPP_VHDCSCODC_cscodc1_START   (9)
#define SOC_VPP_VHDCSCODC_cscodc1_END     (17)
#define SOC_VPP_VHDCSCODC_cscodc2_START   (18)
#define SOC_VPP_VHDCSCODC_cscodc2_END     (26)


/*****************************************************************************
 结构名    : SOC_VPP_VHDCSCP0_UNION
 结构说明  : VHDCSCP0 寄存器结构定义。地址偏移量:0x0188，初值:0x00000000，宽度:32
 寄存器说明: 色彩空间转换参数0，为即时寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  cscp00   : 13; /* bit[0-12] : 5.8数据格式：1bit符号位、4bit整数位、8bit小数位。补码表示。 */
        unsigned int  reserved_0: 3;  /* bit[13-15]: 保留。 */
        unsigned int  cscp01   : 13; /* bit[16-28]: 5.8数据格式：1bit符号位、4bit整数位、8bit小数位。补码表示。 */
        unsigned int  reserved_1: 3;  /* bit[29-31]: 保留。 */
    } reg;
} SOC_VPP_VHDCSCP0_UNION;
#endif
#define SOC_VPP_VHDCSCP0_cscp00_START    (0)
#define SOC_VPP_VHDCSCP0_cscp00_END      (12)
#define SOC_VPP_VHDCSCP0_cscp01_START    (16)
#define SOC_VPP_VHDCSCP0_cscp01_END      (28)


/*****************************************************************************
 结构名    : SOC_VPP_VHDCSCP1_UNION
 结构说明  : VHDCSCP1 寄存器结构定义。地址偏移量:0x018C，初值:0x00000000，宽度:32
 寄存器说明: 色彩空间转换参数1，为即时寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  cscp02   : 13; /* bit[0-12] : 5.8数据格式：1bit符号位、4bit整数位、8bit小数位。补码表示。 */
        unsigned int  reserved_0: 3;  /* bit[13-15]: 保留。 */
        unsigned int  cscp10   : 13; /* bit[16-28]: 5.8数据格式：1bit符号位、4bit整数位、8bit小数位。补码表示。 */
        unsigned int  reserved_1: 3;  /* bit[29-31]: 保留。 */
    } reg;
} SOC_VPP_VHDCSCP1_UNION;
#endif
#define SOC_VPP_VHDCSCP1_cscp02_START    (0)
#define SOC_VPP_VHDCSCP1_cscp02_END      (12)
#define SOC_VPP_VHDCSCP1_cscp10_START    (16)
#define SOC_VPP_VHDCSCP1_cscp10_END      (28)


/*****************************************************************************
 结构名    : SOC_VPP_VHDCSCP2_UNION
 结构说明  : VHDCSCP2 寄存器结构定义。地址偏移量:0x0190，初值:0x00000000，宽度:32
 寄存器说明: 色彩空间转换参数2，为即时寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  cscp11   : 13; /* bit[0-12] : 5.8数据格式：1bit符号位、4bit整数位、8bit小数位。补码表示。 */
        unsigned int  reserved_0: 3;  /* bit[13-15]: 保留。 */
        unsigned int  cscp12   : 13; /* bit[16-28]: 5.8数据格式：1bit符号位、4bit整数位、8bit小数位。补码表示。 */
        unsigned int  reserved_1: 3;  /* bit[29-31]: 保留。 */
    } reg;
} SOC_VPP_VHDCSCP2_UNION;
#endif
#define SOC_VPP_VHDCSCP2_cscp11_START    (0)
#define SOC_VPP_VHDCSCP2_cscp11_END      (12)
#define SOC_VPP_VHDCSCP2_cscp12_START    (16)
#define SOC_VPP_VHDCSCP2_cscp12_END      (28)


/*****************************************************************************
 结构名    : SOC_VPP_VHDCSCP3_UNION
 结构说明  : VHDCSCP3 寄存器结构定义。地址偏移量:0x0194，初值:0x00000000，宽度:32
 寄存器说明: 色彩空间转换参数3，为即时寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  cscp20   : 13; /* bit[0-12] : 5.8数据格式：1bit符号位、4bit整数位、8bit小数位。补码表示。 */
        unsigned int  reserved_0: 3;  /* bit[13-15]: 保留。 */
        unsigned int  cscp21   : 13; /* bit[16-28]: 5.8数据格式：1bit符号位、4bit整数位、8bit小数位。补码表示。 */
        unsigned int  reserved_1: 3;  /* bit[29-31]: 保留。 */
    } reg;
} SOC_VPP_VHDCSCP3_UNION;
#endif
#define SOC_VPP_VHDCSCP3_cscp20_START    (0)
#define SOC_VPP_VHDCSCP3_cscp20_END      (12)
#define SOC_VPP_VHDCSCP3_cscp21_START    (16)
#define SOC_VPP_VHDCSCP3_cscp21_END      (28)


/*****************************************************************************
 结构名    : SOC_VPP_VHDCSCP4_UNION
 结构说明  : VHDCSCP4 寄存器结构定义。地址偏移量:0x0198，初值:0x00000000，宽度:32
 寄存器说明: 色彩空间转换参数4，为即时寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  cscp22   : 13; /* bit[0-12] : 5.8数据格式：1bit符号位、4bit整数位、8bit小数位。补码表示。 */
        unsigned int  reserved : 19; /* bit[13-31]: 保留。 */
    } reg;
} SOC_VPP_VHDCSCP4_UNION;
#endif
#define SOC_VPP_VHDCSCP4_cscp22_START    (0)
#define SOC_VPP_VHDCSCP4_cscp22_END      (12)


/*****************************************************************************
 结构名    : SOC_VPP_VHDACM0_UNION
 结构说明  : VHDACM0 寄存器结构定义。地址偏移量:0x01A0，初值:0x00000000，宽度:32
 寄存器说明: ACM的控制寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  acm_a_u_off : 5;  /* bit[0-4]  :  */
        unsigned int  acm_b_u_off : 5;  /* bit[5-9]  :  */
        unsigned int  acm_c_u_off : 5;  /* bit[10-14]:  */
        unsigned int  acm_d_u_off : 5;  /* bit[15-19]:  */
        unsigned int  acm_fir_blk : 4;  /* bit[20-23]:  */
        unsigned int  acm_sec_blk : 4;  /* bit[24-27]:  */
        unsigned int  acm0_en     : 1;  /* bit[28]   : acm 0 使能。 */
        unsigned int  acm1_en     : 1;  /* bit[29]   : acm 1 使能。 */
        unsigned int  acm2_en     : 1;  /* bit[30]   : acm 2 使能。 */
        unsigned int  acm3_en     : 1;  /* bit[31]   : acm 3 使能。 */
    } reg;
} SOC_VPP_VHDACM0_UNION;
#endif
#define SOC_VPP_VHDACM0_acm_a_u_off_START  (0)
#define SOC_VPP_VHDACM0_acm_a_u_off_END    (4)
#define SOC_VPP_VHDACM0_acm_b_u_off_START  (5)
#define SOC_VPP_VHDACM0_acm_b_u_off_END    (9)
#define SOC_VPP_VHDACM0_acm_c_u_off_START  (10)
#define SOC_VPP_VHDACM0_acm_c_u_off_END    (14)
#define SOC_VPP_VHDACM0_acm_d_u_off_START  (15)
#define SOC_VPP_VHDACM0_acm_d_u_off_END    (19)
#define SOC_VPP_VHDACM0_acm_fir_blk_START  (20)
#define SOC_VPP_VHDACM0_acm_fir_blk_END    (23)
#define SOC_VPP_VHDACM0_acm_sec_blk_START  (24)
#define SOC_VPP_VHDACM0_acm_sec_blk_END    (27)
#define SOC_VPP_VHDACM0_acm0_en_START      (28)
#define SOC_VPP_VHDACM0_acm0_en_END        (28)
#define SOC_VPP_VHDACM0_acm1_en_START      (29)
#define SOC_VPP_VHDACM0_acm1_en_END        (29)
#define SOC_VPP_VHDACM0_acm2_en_START      (30)
#define SOC_VPP_VHDACM0_acm2_en_END        (30)
#define SOC_VPP_VHDACM0_acm3_en_START      (31)
#define SOC_VPP_VHDACM0_acm3_en_END        (31)


/*****************************************************************************
 结构名    : SOC_VPP_VHDACM1_UNION
 结构说明  : VHDACM1 寄存器结构定义。地址偏移量:0x01A4，初值:0x00000000，宽度:32
 寄存器说明: ACM寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  acm_a_v_off : 5;  /* bit[0-4]  :  */
        unsigned int  acm_b_v_off : 5;  /* bit[5-9]  :  */
        unsigned int  acm_c_v_off : 5;  /* bit[10-14]:  */
        unsigned int  acm_d_v_off : 5;  /* bit[15-19]:  */
        unsigned int  acm_test_en : 1;  /* bit[20]   : ACM对比测试使能。
                                                       1：左边不做，右边做ACM；&#13;0：正常做。 */
        unsigned int  reserved    : 11; /* bit[21-31]: 保留。 */
    } reg;
} SOC_VPP_VHDACM1_UNION;
#endif
#define SOC_VPP_VHDACM1_acm_a_v_off_START  (0)
#define SOC_VPP_VHDACM1_acm_a_v_off_END    (4)
#define SOC_VPP_VHDACM1_acm_b_v_off_START  (5)
#define SOC_VPP_VHDACM1_acm_b_v_off_END    (9)
#define SOC_VPP_VHDACM1_acm_c_v_off_START  (10)
#define SOC_VPP_VHDACM1_acm_c_v_off_END    (14)
#define SOC_VPP_VHDACM1_acm_d_v_off_START  (15)
#define SOC_VPP_VHDACM1_acm_d_v_off_END    (19)
#define SOC_VPP_VHDACM1_acm_test_en_START  (20)
#define SOC_VPP_VHDACM1_acm_test_en_END    (20)


/*****************************************************************************
 结构名    : SOC_VPP_VHDACM2_UNION
 结构说明  : VHDACM2 寄存器结构定义。地址偏移量:0x01A8，初值:0x00000000，宽度:32
 寄存器说明: ACM寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  acm_a_u_off : 5;  /* bit[0-4]  :  */
        unsigned int  acm_b_u_off : 5;  /* bit[5-9]  :  */
        unsigned int  acm_c_u_off : 5;  /* bit[10-14]:  */
        unsigned int  acm_d_u_off : 5;  /* bit[15-19]:  */
        unsigned int  acm_fir_blk : 4;  /* bit[20-23]:  */
        unsigned int  acm_sec_blk : 4;  /* bit[24-27]:  */
        unsigned int  reserved    : 4;  /* bit[28-31]: 保留。 */
    } reg;
} SOC_VPP_VHDACM2_UNION;
#endif
#define SOC_VPP_VHDACM2_acm_a_u_off_START  (0)
#define SOC_VPP_VHDACM2_acm_a_u_off_END    (4)
#define SOC_VPP_VHDACM2_acm_b_u_off_START  (5)
#define SOC_VPP_VHDACM2_acm_b_u_off_END    (9)
#define SOC_VPP_VHDACM2_acm_c_u_off_START  (10)
#define SOC_VPP_VHDACM2_acm_c_u_off_END    (14)
#define SOC_VPP_VHDACM2_acm_d_u_off_START  (15)
#define SOC_VPP_VHDACM2_acm_d_u_off_END    (19)
#define SOC_VPP_VHDACM2_acm_fir_blk_START  (20)
#define SOC_VPP_VHDACM2_acm_fir_blk_END    (23)
#define SOC_VPP_VHDACM2_acm_sec_blk_START  (24)
#define SOC_VPP_VHDACM2_acm_sec_blk_END    (27)


/*****************************************************************************
 结构名    : SOC_VPP_VHDACM3_UNION
 结构说明  : VHDACM3 寄存器结构定义。地址偏移量:0x01AC，初值:0x00000000，宽度:32
 寄存器说明: ACM寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  acm_a_v_off : 5;  /* bit[0-4]  :  */
        unsigned int  acm_b_v_off : 5;  /* bit[5-9]  :  */
        unsigned int  acm_c_v_off : 5;  /* bit[10-14]:  */
        unsigned int  acm_d_v_off : 5;  /* bit[15-19]:  */
        unsigned int  reserved    : 12; /* bit[20-31]: 保留。 */
    } reg;
} SOC_VPP_VHDACM3_UNION;
#endif
#define SOC_VPP_VHDACM3_acm_a_v_off_START  (0)
#define SOC_VPP_VHDACM3_acm_a_v_off_END    (4)
#define SOC_VPP_VHDACM3_acm_b_v_off_START  (5)
#define SOC_VPP_VHDACM3_acm_b_v_off_END    (9)
#define SOC_VPP_VHDACM3_acm_c_v_off_START  (10)
#define SOC_VPP_VHDACM3_acm_c_v_off_END    (14)
#define SOC_VPP_VHDACM3_acm_d_v_off_START  (15)
#define SOC_VPP_VHDACM3_acm_d_v_off_END    (19)


/*****************************************************************************
 结构名    : SOC_VPP_VHDACM4_UNION
 结构说明  : VHDACM4 寄存器结构定义。地址偏移量:0x01B0，初值:0x00000000，宽度:32
 寄存器说明: ACM寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  acm_a_u_off : 5;  /* bit[0-4]  :  */
        unsigned int  acm_b_u_off : 5;  /* bit[5-9]  :  */
        unsigned int  acm_c_u_off : 5;  /* bit[10-14]:  */
        unsigned int  acm_d_u_off : 5;  /* bit[15-19]:  */
        unsigned int  acm_fir_blk : 4;  /* bit[20-23]:  */
        unsigned int  acm_sec_blk : 4;  /* bit[24-27]:  */
        unsigned int  reserved    : 4;  /* bit[28-31]: 保留。 */
    } reg;
} SOC_VPP_VHDACM4_UNION;
#endif
#define SOC_VPP_VHDACM4_acm_a_u_off_START  (0)
#define SOC_VPP_VHDACM4_acm_a_u_off_END    (4)
#define SOC_VPP_VHDACM4_acm_b_u_off_START  (5)
#define SOC_VPP_VHDACM4_acm_b_u_off_END    (9)
#define SOC_VPP_VHDACM4_acm_c_u_off_START  (10)
#define SOC_VPP_VHDACM4_acm_c_u_off_END    (14)
#define SOC_VPP_VHDACM4_acm_d_u_off_START  (15)
#define SOC_VPP_VHDACM4_acm_d_u_off_END    (19)
#define SOC_VPP_VHDACM4_acm_fir_blk_START  (20)
#define SOC_VPP_VHDACM4_acm_fir_blk_END    (23)
#define SOC_VPP_VHDACM4_acm_sec_blk_START  (24)
#define SOC_VPP_VHDACM4_acm_sec_blk_END    (27)


/*****************************************************************************
 结构名    : SOC_VPP_VHDACM5_UNION
 结构说明  : VHDACM5 寄存器结构定义。地址偏移量:0x01B4，初值:0x00000000，宽度:32
 寄存器说明: ACM寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  acm_a_v_off : 5;  /* bit[0-4]  :  */
        unsigned int  acm_b_v_off : 5;  /* bit[5-9]  :  */
        unsigned int  acm_c_v_off : 5;  /* bit[10-14]:  */
        unsigned int  acm_d_v_off : 5;  /* bit[15-19]:  */
        unsigned int  reserved    : 12; /* bit[20-31]: 保留。 */
    } reg;
} SOC_VPP_VHDACM5_UNION;
#endif
#define SOC_VPP_VHDACM5_acm_a_v_off_START  (0)
#define SOC_VPP_VHDACM5_acm_a_v_off_END    (4)
#define SOC_VPP_VHDACM5_acm_b_v_off_START  (5)
#define SOC_VPP_VHDACM5_acm_b_v_off_END    (9)
#define SOC_VPP_VHDACM5_acm_c_v_off_START  (10)
#define SOC_VPP_VHDACM5_acm_c_v_off_END    (14)
#define SOC_VPP_VHDACM5_acm_d_v_off_START  (15)
#define SOC_VPP_VHDACM5_acm_d_v_off_END    (19)


/*****************************************************************************
 结构名    : SOC_VPP_VHDACM6_UNION
 结构说明  : VHDACM6 寄存器结构定义。地址偏移量:0x01B8，初值:0x00000000，宽度:32
 寄存器说明: ACM寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  acm_a_u_off : 5;  /* bit[0-4]  :  */
        unsigned int  acm_b_u_off : 5;  /* bit[5-9]  :  */
        unsigned int  acm_c_u_off : 5;  /* bit[10-14]:  */
        unsigned int  acm_d_u_off : 5;  /* bit[15-19]:  */
        unsigned int  acm_fir_blk : 4;  /* bit[20-23]:  */
        unsigned int  acm_sec_blk : 4;  /* bit[24-27]:  */
        unsigned int  reserved    : 4;  /* bit[28-31]: 保留。 */
    } reg;
} SOC_VPP_VHDACM6_UNION;
#endif
#define SOC_VPP_VHDACM6_acm_a_u_off_START  (0)
#define SOC_VPP_VHDACM6_acm_a_u_off_END    (4)
#define SOC_VPP_VHDACM6_acm_b_u_off_START  (5)
#define SOC_VPP_VHDACM6_acm_b_u_off_END    (9)
#define SOC_VPP_VHDACM6_acm_c_u_off_START  (10)
#define SOC_VPP_VHDACM6_acm_c_u_off_END    (14)
#define SOC_VPP_VHDACM6_acm_d_u_off_START  (15)
#define SOC_VPP_VHDACM6_acm_d_u_off_END    (19)
#define SOC_VPP_VHDACM6_acm_fir_blk_START  (20)
#define SOC_VPP_VHDACM6_acm_fir_blk_END    (23)
#define SOC_VPP_VHDACM6_acm_sec_blk_START  (24)
#define SOC_VPP_VHDACM6_acm_sec_blk_END    (27)


/*****************************************************************************
 结构名    : SOC_VPP_VHDACM7_UNION
 结构说明  : VHDACM7 寄存器结构定义。地址偏移量:0x01BC，初值:0x00000000，宽度:32
 寄存器说明: ACM寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  acm_a_v_off : 5;  /* bit[0-4]  :  */
        unsigned int  acm_b_v_off : 5;  /* bit[5-9]  :  */
        unsigned int  acm_c_v_off : 5;  /* bit[10-14]:  */
        unsigned int  acm_d_v_off : 5;  /* bit[15-19]:  */
        unsigned int  reserved    : 12; /* bit[20-31]: 保留。 */
    } reg;
} SOC_VPP_VHDACM7_UNION;
#endif
#define SOC_VPP_VHDACM7_acm_a_v_off_START  (0)
#define SOC_VPP_VHDACM7_acm_a_v_off_END    (4)
#define SOC_VPP_VHDACM7_acm_b_v_off_START  (5)
#define SOC_VPP_VHDACM7_acm_b_v_off_END    (9)
#define SOC_VPP_VHDACM7_acm_c_v_off_START  (10)
#define SOC_VPP_VHDACM7_acm_c_v_off_END    (14)
#define SOC_VPP_VHDACM7_acm_d_v_off_START  (15)
#define SOC_VPP_VHDACM7_acm_d_v_off_END    (19)


/*****************************************************************************
 结构名    : SOC_VPP_VHDHSP_UNION
 结构说明  : VHDHSP 寄存器结构定义。地址偏移量:0x01C0，初值:0x00000000，宽度:32
 寄存器说明: 水平缩放参数配置（horizontal Scaling Parameter）寄存器。为非即时寄存器。
            缩放比率＝输入宽度/输出宽度。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  hratio     : 16; /* bit[0-15] : 水平缩放倍数，(u,4,12)格式。 */
        unsigned int  reserved_0 : 3;  /* bit[16-18]: 保留。 */
        unsigned int  hfir_order : 1;  /* bit[19]   : 水平缩放位置。
                                                      0：水平缩放在垂直缩放前面；&#13;1：水平缩放在垂直缩放后面。 */
        unsigned int  reserved_1 : 3;  /* bit[20-22]: 保留。 */
        unsigned int  hchfir_en  : 1;  /* bit[23]   : 水平色度缩放模式。
                                                      0：复制模式（滤波不使能）；&#13;1：滤波模式（滤波使能）。 */
        unsigned int  hlfir_en   : 1;  /* bit[24]   : 水平亮度缩放模式。
                                                      0：复制模式（滤波不使能）；&#13;1：滤波模式（滤波使能）。 */
        unsigned int  reserved_2 : 3;  /* bit[25-27]: 保留。 */
        unsigned int  hchmid_en  : 1;  /* bit[28]   : 水平色度缩放中值滤波使能（当hchfir_en无效时，该比特不起作用）。
                                                      0：禁止；&#13;1：使能。 */
        unsigned int  hlmid_en   : 1;  /* bit[29]   : 水平亮度缩放中值滤波使能（当hlfir_en无效时，该比特不起作用）。
                                                      0：禁止；&#13;1：使能。 */
        unsigned int  hchmsc_en  : 1;  /* bit[30]   : 水平色度缩放使能。
                                                      0：禁止；&#13;1：使能。 */
        unsigned int  hlmsc_en   : 1;  /* bit[31]   : 水平亮度缩放使能。
                                                      0：禁止；&#13;1：使能。 */
    } reg;
} SOC_VPP_VHDHSP_UNION;
#endif
#define SOC_VPP_VHDHSP_hratio_START      (0)
#define SOC_VPP_VHDHSP_hratio_END        (15)
#define SOC_VPP_VHDHSP_hfir_order_START  (19)
#define SOC_VPP_VHDHSP_hfir_order_END    (19)
#define SOC_VPP_VHDHSP_hchfir_en_START   (23)
#define SOC_VPP_VHDHSP_hchfir_en_END     (23)
#define SOC_VPP_VHDHSP_hlfir_en_START    (24)
#define SOC_VPP_VHDHSP_hlfir_en_END      (24)
#define SOC_VPP_VHDHSP_hchmid_en_START   (28)
#define SOC_VPP_VHDHSP_hchmid_en_END     (28)
#define SOC_VPP_VHDHSP_hlmid_en_START    (29)
#define SOC_VPP_VHDHSP_hlmid_en_END      (29)
#define SOC_VPP_VHDHSP_hchmsc_en_START   (30)
#define SOC_VPP_VHDHSP_hchmsc_en_END     (30)
#define SOC_VPP_VHDHSP_hlmsc_en_START    (31)
#define SOC_VPP_VHDHSP_hlmsc_en_END      (31)


/*****************************************************************************
 结构名    : SOC_VPP_VHDHLOFFSET_UNION
 结构说明  : VHDHLOFFSET 寄存器结构定义。地址偏移量:0x01C4，初值:0x00000000，宽度:32
 寄存器说明: 亮度水平位置偏移寄存器。用于pan-scan。为非即时寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  hor_loffset : 17; /* bit[0-16] : 水平亮度位置偏移，(s,5,12)格式。补码表示。 */
        unsigned int  reserved    : 15; /* bit[17-31]: 保留。 */
    } reg;
} SOC_VPP_VHDHLOFFSET_UNION;
#endif
#define SOC_VPP_VHDHLOFFSET_hor_loffset_START  (0)
#define SOC_VPP_VHDHLOFFSET_hor_loffset_END    (16)


/*****************************************************************************
 结构名    : SOC_VPP_VHDHCOFFSET_UNION
 结构说明  : VHDHCOFFSET 寄存器结构定义。地址偏移量:0x01C8，初值:0x00000000，宽度:32
 寄存器说明: 色度水平位置偏移寄存器。用于pan-scan。为非即时寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  hor_coffset : 17; /* bit[0-16] : 水平色度位置偏移，(s,5,12)格式。补码表示。 */
        unsigned int  reserved    : 15; /* bit[17-31]: 保留。 */
    } reg;
} SOC_VPP_VHDHCOFFSET_UNION;
#endif
#define SOC_VPP_VHDHCOFFSET_hor_coffset_START  (0)
#define SOC_VPP_VHDHCOFFSET_hor_coffset_END    (16)


/*****************************************************************************
 结构名    : SOC_VPP_VHDVSP_UNION
 结构说明  : VHDVSP 寄存器结构定义。地址偏移量:0x01D8，初值:0x00000000，宽度:32
 寄存器说明: 缩放参数 (vertical Scaling Parameter)寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  reserved       : 23; /* bit[0-22]: 保留。 */
        unsigned int  vchfir_en      : 1;  /* bit[23]  : 垂直色度缩放模式。
                                                         0：复制模式（滤波不使能）；&#13;1：滤波模式（滤波使能）。 */
        unsigned int  vlfir_en       : 1;  /* bit[24]  : 垂直亮度缩放模式。
                                                         0：复制模式（滤波不使能）；&#13;1：滤波模式（滤波使能）。 */
        unsigned int  chroma_type    : 1;  /* bit[25]  : 亮度和色度在垂直方向的比例。
                                                         0：422；&#13;1：420。 */
        unsigned int  vsc_chroma_tap : 1;  /* bit[26]  : 垂直色度缩放阶数。
                                                         0：4阶FIR；&#13;1：2阶FIR。 */
        unsigned int  vsc_luma_tap   : 1;  /* bit[27]  : 垂直亮度缩放阶数。
                                                         0：4阶FIR；&#13;1：2阶FIR； */
        unsigned int  vchmid_en      : 1;  /* bit[28]  : 垂直色度缩放中值滤波使能（当vchfir_en无效时，该比特不起作用）。
                                                         0：禁止；&#13;1：使能。 */
        unsigned int  vlmid_en       : 1;  /* bit[29]  : 垂直亮度缩放中值滤波使能（当vlfir_en无效时，该比特不起作用）。
                                                         0：禁止；&#13;1：使能。 */
        unsigned int  vchmsc_en      : 1;  /* bit[30]  : 垂直色度缩放使能。
                                                         0：禁止；&#13;1：使能。 */
        unsigned int  vlmsc_en       : 1;  /* bit[31]  : 垂直亮度缩放使能。
                                                         0：禁止；&#13;1：使能。 */
    } reg;
} SOC_VPP_VHDVSP_UNION;
#endif
#define SOC_VPP_VHDVSP_vchfir_en_START       (23)
#define SOC_VPP_VHDVSP_vchfir_en_END         (23)
#define SOC_VPP_VHDVSP_vlfir_en_START        (24)
#define SOC_VPP_VHDVSP_vlfir_en_END          (24)
#define SOC_VPP_VHDVSP_chroma_type_START     (25)
#define SOC_VPP_VHDVSP_chroma_type_END       (25)
#define SOC_VPP_VHDVSP_vsc_chroma_tap_START  (26)
#define SOC_VPP_VHDVSP_vsc_chroma_tap_END    (26)
#define SOC_VPP_VHDVSP_vsc_luma_tap_START    (27)
#define SOC_VPP_VHDVSP_vsc_luma_tap_END      (27)
#define SOC_VPP_VHDVSP_vchmid_en_START       (28)
#define SOC_VPP_VHDVSP_vchmid_en_END         (28)
#define SOC_VPP_VHDVSP_vlmid_en_START        (29)
#define SOC_VPP_VHDVSP_vlmid_en_END          (29)
#define SOC_VPP_VHDVSP_vchmsc_en_START       (30)
#define SOC_VPP_VHDVSP_vchmsc_en_END         (30)
#define SOC_VPP_VHDVSP_vlmsc_en_START        (31)
#define SOC_VPP_VHDVSP_vlmsc_en_END          (31)


/*****************************************************************************
 结构名    : SOC_VPP_VHDVSR_UNION
 结构说明  : VHDVSR 寄存器结构定义。地址偏移量:0x01DC，初值:0x00001000，宽度:32
 寄存器说明: 垂直缩放倍数 (Luma Vertical Scaling Ratio)寄存器。缩放比率＝输入高度/输出高度。为非即时寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  vratio   : 16; /* bit[0-15] : 垂直缩放倍数，(u,4,12)格式。 */
        unsigned int  reserved : 16; /* bit[16-31]: 保留。 */
    } reg;
} SOC_VPP_VHDVSR_UNION;
#endif
#define SOC_VPP_VHDVSR_vratio_START    (0)
#define SOC_VPP_VHDVSR_vratio_END      (15)


/*****************************************************************************
 结构名    : SOC_VPP_VHDVOFFSET_UNION
 结构说明  : VHDVOFFSET 寄存器结构定义。地址偏移量:0x01E0，初值:0x00000000，宽度:32
 寄存器说明: 垂直缩放偏移（Vertical Luma Offset）寄存器。垂直缩放偏移受两个因素影响：pan-scan和重复帧时的场偏移。当没有场偏移时，vluma_offset为offset_pan-scan的最低整数位+小数位；在YCbCr422格式时vchroma_offset与vluma_offset相等；在YCbCr420格式时，vchroma_offset = scaling_chroma/2-0.25。当需要场偏移时（如静帧或者重复帧），假设重复底场，则顶场配置的vluma_offset和vchroma_offset与上述无偏移时相同，底场配置需要考虑场偏移。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  vchroma_offset : 16; /* bit[0-15] : 垂直色度偏移，补码表示，(s,4,12)格式。 */
        unsigned int  vluma_offset   : 16; /* bit[16-31]: 垂直亮度偏移，补码表示，(s,4,12)格式。 */
    } reg;
} SOC_VPP_VHDVOFFSET_UNION;
#endif
#define SOC_VPP_VHDVOFFSET_vchroma_offset_START  (0)
#define SOC_VPP_VHDVOFFSET_vchroma_offset_END    (15)
#define SOC_VPP_VHDVOFFSET_vluma_offset_START    (16)
#define SOC_VPP_VHDVOFFSET_vluma_offset_END      (31)


/*****************************************************************************
 结构名    : SOC_VPP_VHDZMEORESO_UNION
 结构说明  : VHDZMEORESO 寄存器结构定义。地址偏移量:0x01E4，初值:0x00000000，宽度:32
 寄存器说明: 缩放单元的输出分辨率寄存器，为非即时寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ow       : 12; /* bit[0-11] : 宽度，像素单位。实际宽度减1。
                                                    注意：层的实际宽度必须是偶数。 */
        unsigned int  oh       : 12; /* bit[12-23]: 高度，行为单位。实际高度减1。
                                                    逐行时，以帧高度为参考，以行为单位，&#13;隔行时，以场高度为参考，以行为单位。 */
        unsigned int  reserved : 8;  /* bit[24-31]: 保留。 */
    } reg;
} SOC_VPP_VHDZMEORESO_UNION;
#endif
#define SOC_VPP_VHDZMEORESO_ow_START        (0)
#define SOC_VPP_VHDZMEORESO_ow_END          (11)
#define SOC_VPP_VHDZMEORESO_oh_START        (12)
#define SOC_VPP_VHDZMEORESO_oh_END          (23)


/*****************************************************************************
 结构名    : SOC_VPP_VHDZMEIRESO_UNION
 结构说明  : VHDZMEIRESO 寄存器结构定义。地址偏移量:0x01E8，初值:0x00000000，宽度:32
 寄存器说明: 缩放单元的输出入辨率寄存器。为非即时寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  iw       : 12; /* bit[0-11] : 宽度，像素单位。实际宽度减1。
                                                    注意：层的实际宽度必须是偶数。 */
        unsigned int  ih       : 12; /* bit[12-23]: 高度，行为单位。实际高度减1。
                                                    逐行时，以帧高度为参考，以行为单位，&#13;隔行时，以场高度为参考，以行为单位。 */
        unsigned int  reserved : 8;  /* bit[24-31]: 保留。 */
    } reg;
} SOC_VPP_VHDZMEIRESO_UNION;
#endif
#define SOC_VPP_VHDZMEIRESO_iw_START        (0)
#define SOC_VPP_VHDZMEIRESO_iw_END          (11)
#define SOC_VPP_VHDZMEIRESO_ih_START        (12)
#define SOC_VPP_VHDZMEIRESO_ih_END          (23)


/*****************************************************************************
 结构名    : SOC_VPP_VHDACCTHD1_UNION
 结构说明  : VHDACCTHD1 寄存器结构定义。地址偏移量:0x0200，初值:0x00000000，宽度:32
 寄存器说明: VHD通道 ACC运算门限1
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  thd_low     : 10; /* bit[0-9]  : 亮度等级low门限值 */
        unsigned int  thd_high    : 10; /* bit[10-19]: 亮度等级high门限值。 */
        unsigned int  thd_med_low : 10; /* bit[20-29]: 亮度等级med_low门限值。 */
        unsigned int  acc_mode    : 1;  /* bit[30]   : 输出ACC校正模式，即时寄存器。
                                                       0：ACC表由硬件产生；
                                                       1：ACC表由软件配置。 */
        unsigned int  acc_en      : 1;  /* bit[31]   : acc使能。非即时寄存器。
                                                       0：禁止；
                                                       1：使能。 */
    } reg;
} SOC_VPP_VHDACCTHD1_UNION;
#endif
#define SOC_VPP_VHDACCTHD1_thd_low_START      (0)
#define SOC_VPP_VHDACCTHD1_thd_low_END        (9)
#define SOC_VPP_VHDACCTHD1_thd_high_START     (10)
#define SOC_VPP_VHDACCTHD1_thd_high_END       (19)
#define SOC_VPP_VHDACCTHD1_thd_med_low_START  (20)
#define SOC_VPP_VHDACCTHD1_thd_med_low_END    (29)
#define SOC_VPP_VHDACCTHD1_acc_mode_START     (30)
#define SOC_VPP_VHDACCTHD1_acc_mode_END       (30)
#define SOC_VPP_VHDACCTHD1_acc_en_START       (31)
#define SOC_VPP_VHDACCTHD1_acc_en_END         (31)


/*****************************************************************************
 结构名    : SOC_VPP_VHDACCTHD2_UNION
 结构说明  : VHDACCTHD2 寄存器结构定义。地址偏移量:0x0204，初值:0x00000000，宽度:32
 寄存器说明: VHD通道 ACC运算门限2
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  thd_med_high : 10; /* bit[0-9]  : 亮度等级med_high门限值 */
        unsigned int  acc_multiple : 8;  /* bit[10-17]: ACC运算乘数 */
        unsigned int  acc_rst      : 1;  /* bit[18]   : ACC复位寄存器 */
        unsigned int  reserved     : 13; /* bit[19-31]: 保留。 */
    } reg;
} SOC_VPP_VHDACCTHD2_UNION;
#endif
#define SOC_VPP_VHDACCTHD2_thd_med_high_START  (0)
#define SOC_VPP_VHDACCTHD2_thd_med_high_END    (9)
#define SOC_VPP_VHDACCTHD2_acc_multiple_START  (10)
#define SOC_VPP_VHDACCTHD2_acc_multiple_END    (17)
#define SOC_VPP_VHDACCTHD2_acc_rst_START       (18)
#define SOC_VPP_VHDACCTHD2_acc_rst_END         (18)


/*****************************************************************************
 结构名    : SOC_VPP_VHDACCLOWN_UNION
 结构说明  : VHDACCLOWN 寄存器结构定义。地址偏移量:0x0210～0x0218，初值:0x00000000，宽度:32
 寄存器说明: ACC查找表，亮度等级为low。n=1~3
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  table_data1n : 10; /* bit[0-9]  : ACC亮度等级low的查找表数据1 */
        unsigned int  table_data2n : 10; /* bit[10-19]: ACC亮度等级low的查找表数据2 */
        unsigned int  table_data3n : 10; /* bit[20-29]: ACC亮度等级low的查找表数据3 */
        unsigned int  reserved     : 2;  /* bit[30-31]: 保留。 */
    } reg;
} SOC_VPP_VHDACCLOWN_UNION;
#endif
#define SOC_VPP_VHDACCLOWN_table_data1n_START  (0)
#define SOC_VPP_VHDACCLOWN_table_data1n_END    (9)
#define SOC_VPP_VHDACCLOWN_table_data2n_START  (10)
#define SOC_VPP_VHDACCLOWN_table_data2n_END    (19)
#define SOC_VPP_VHDACCLOWN_table_data3n_START  (20)
#define SOC_VPP_VHDACCLOWN_table_data3n_END    (29)


/*****************************************************************************
 结构名    : SOC_VPP_VHDACCMEDN_UNION
 结构说明  : VHDACCMEDN 寄存器结构定义。地址偏移量:0x0220～0x0228，初值:0x00000000，宽度:32
 寄存器说明: ACC查找表，亮度等级为middle。n=1~3
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  table_data1n : 10; /* bit[0-9]  : ACC亮度等级middle的查找表数据1 */
        unsigned int  table_data2n : 10; /* bit[10-19]: ACC亮度等级middle的查找表数据2 */
        unsigned int  table_data3n : 10; /* bit[20-29]: ACC亮度等级middle的查找表数据3 */
        unsigned int  reserved     : 2;  /* bit[30-31]: 保留。 */
    } reg;
} SOC_VPP_VHDACCMEDN_UNION;
#endif
#define SOC_VPP_VHDACCMEDN_table_data1n_START  (0)
#define SOC_VPP_VHDACCMEDN_table_data1n_END    (9)
#define SOC_VPP_VHDACCMEDN_table_data2n_START  (10)
#define SOC_VPP_VHDACCMEDN_table_data2n_END    (19)
#define SOC_VPP_VHDACCMEDN_table_data3n_START  (20)
#define SOC_VPP_VHDACCMEDN_table_data3n_END    (29)


/*****************************************************************************
 结构名    : SOC_VPP_VHDACCHIGHN_UNION
 结构说明  : VHDACCHIGHN 寄存器结构定义。地址偏移量:0x0230～0x0238，初值:0x00000000，宽度:32
 寄存器说明: ACC查找表，亮度等级为high。n=1~3
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  table_data1n : 10; /* bit[0-9]  : ACC亮度等级high的查找表数据1 */
        unsigned int  table_data2n : 10; /* bit[10-19]: ACC亮度等级high的查找表数据2 */
        unsigned int  table_data3n : 10; /* bit[20-29]: ACC亮度等级high的查找表数据3 */
        unsigned int  reserved     : 2;  /* bit[30-31]: 保留。 */
    } reg;
} SOC_VPP_VHDACCHIGHN_UNION;
#endif
#define SOC_VPP_VHDACCHIGHN_table_data1n_START  (0)
#define SOC_VPP_VHDACCHIGHN_table_data1n_END    (9)
#define SOC_VPP_VHDACCHIGHN_table_data2n_START  (10)
#define SOC_VPP_VHDACCHIGHN_table_data2n_END    (19)
#define SOC_VPP_VHDACCHIGHN_table_data3n_START  (20)
#define SOC_VPP_VHDACCHIGHN_table_data3n_END    (29)


/*****************************************************************************
 结构名    : SOC_VPP_VHDACCMLN_UNION
 结构说明  : VHDACCMLN 寄存器结构定义。地址偏移量:0x0240～0x0248，初值:0x00000000，宽度:32
 寄存器说明: ACC查找表，亮度等级为middle_low。n=1~3
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  table_data1n : 10; /* bit[0-9]  : ACC亮度等级middle_low的查找表数据1 */
        unsigned int  table_data2n : 10; /* bit[10-19]: ACC亮度等级middle_low的查找表数据2 */
        unsigned int  table_data3n : 10; /* bit[20-29]: ACC亮度等级middle_low的查找表数据3 */
        unsigned int  reserved     : 2;  /* bit[30-31]: 保留。 */
    } reg;
} SOC_VPP_VHDACCMLN_UNION;
#endif
#define SOC_VPP_VHDACCMLN_table_data1n_START  (0)
#define SOC_VPP_VHDACCMLN_table_data1n_END    (9)
#define SOC_VPP_VHDACCMLN_table_data2n_START  (10)
#define SOC_VPP_VHDACCMLN_table_data2n_END    (19)
#define SOC_VPP_VHDACCMLN_table_data3n_START  (20)
#define SOC_VPP_VHDACCMLN_table_data3n_END    (29)


/*****************************************************************************
 结构名    : SOC_VPP_VHDACCMHN_UNION
 结构说明  : VHDACCMHN 寄存器结构定义。地址偏移量:0x0250～0x0258，初值:0x00000000，宽度:32
 寄存器说明: ACC查找表，亮度等级为middle_high。n=1~3
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  table_data1n : 10; /* bit[0-9]  : ACC亮度等级middle_high的查找表数据1 */
        unsigned int  table_data2n : 10; /* bit[10-19]: ACC亮度等级middle_high的查找表数据2 */
        unsigned int  table_data3n : 10; /* bit[20-29]: ACC亮度等级middle_high的查找表数据3 */
        unsigned int  reserved     : 2;  /* bit[30-31]: 保留。 */
    } reg;
} SOC_VPP_VHDACCMHN_UNION;
#endif
#define SOC_VPP_VHDACCMHN_table_data1n_START  (0)
#define SOC_VPP_VHDACCMHN_table_data1n_END    (9)
#define SOC_VPP_VHDACCMHN_table_data2n_START  (10)
#define SOC_VPP_VHDACCMHN_table_data2n_END    (19)
#define SOC_VPP_VHDACCMHN_table_data3n_START  (20)
#define SOC_VPP_VHDACCMHN_table_data3n_END    (29)


/*****************************************************************************
 结构名    : SOC_VPP_VHDACC3LOW_UNION
 结构说明  : VHDACC3LOW 寄存器结构定义。地址偏移量:0x0260，初值:0x00000000，宽度:32
 寄存器说明: ACC三区域的低门限亮度统计值，为只读寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  cnt3_low : 21; /* bit[0-20] : 三区域的低门限亮度统计值 */
        unsigned int  reserved : 11; /* bit[21-31]: 保留。 */
    } reg;
} SOC_VPP_VHDACC3LOW_UNION;
#endif
#define SOC_VPP_VHDACC3LOW_cnt3_low_START  (0)
#define SOC_VPP_VHDACC3LOW_cnt3_low_END    (20)


/*****************************************************************************
 结构名    : SOC_VPP_VHDACC3MED_UNION
 结构说明  : VHDACC3MED 寄存器结构定义。地址偏移量:0x0264，初值:0x00000000，宽度:32
 寄存器说明: ACC三区域的中门限亮度统计值，为只读寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  cnt3_med : 21; /* bit[0-20] : 三区域的中门限亮度统计值 */
        unsigned int  reserved : 11; /* bit[21-31]: 保留。 */
    } reg;
} SOC_VPP_VHDACC3MED_UNION;
#endif
#define SOC_VPP_VHDACC3MED_cnt3_med_START  (0)
#define SOC_VPP_VHDACC3MED_cnt3_med_END    (20)


/*****************************************************************************
 结构名    : SOC_VPP_VHDACC3HIGH_UNION
 结构说明  : VHDACC3HIGH 寄存器结构定义。地址偏移量:0x0268，初值:0x00000000，宽度:32
 寄存器说明: ACC三区域的高门限亮度统计值，为只读寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  cnt3_high : 21; /* bit[0-20] : 三区域的高门限亮度统计值 */
        unsigned int  reserved  : 11; /* bit[21-31]: 保留。 */
    } reg;
} SOC_VPP_VHDACC3HIGH_UNION;
#endif
#define SOC_VPP_VHDACC3HIGH_cnt3_high_START  (0)
#define SOC_VPP_VHDACC3HIGH_cnt3_high_END    (20)


/*****************************************************************************
 结构名    : SOC_VPP_VHDACC8MLOW_UNION
 结构说明  : VHDACC8MLOW 寄存器结构定义。地址偏移量:0x026C，初值:0x00000000，宽度:32
 寄存器说明: ACC中间区域分成8段后，低门限亮度统计值，为只读寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  cnt8_med_low : 21; /* bit[0-20] : 中间区域的低门限亮度统计值 */
        unsigned int  reserved     : 11; /* bit[21-31]: 保留。 */
    } reg;
} SOC_VPP_VHDACC8MLOW_UNION;
#endif
#define SOC_VPP_VHDACC8MLOW_cnt8_med_low_START  (0)
#define SOC_VPP_VHDACC8MLOW_cnt8_med_low_END    (20)


/*****************************************************************************
 结构名    : SOC_VPP_VHDACC8MHIGH_UNION
 结构说明  : VHDACC8MHIGH 寄存器结构定义。地址偏移量:0x0270，初值:0x00000000，宽度:32
 寄存器说明: ACC中间区域分成8段后，高门限亮度统计值，为只读寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  cnt8_med_high : 21; /* bit[0-20] : 中间区域的高门限亮度统计值 */
        unsigned int  reserved      : 11; /* bit[21-31]: 保留。 */
    } reg;
} SOC_VPP_VHDACC8MHIGH_UNION;
#endif
#define SOC_VPP_VHDACC8MHIGH_cnt8_med_high_START  (0)
#define SOC_VPP_VHDACC8MHIGH_cnt8_med_high_END    (20)


/*****************************************************************************
 结构名    : SOC_VPP_VHDACCTOTAL_UNION
 结构说明  : VHDACCTOTAL 寄存器结构定义。地址偏移量:0x0274，初值:0x00000000，宽度:32
 寄存器说明: ACC像素统计值，为只读寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  acc_pix_total : 21; /* bit[0-20] : ACC总象素统计值 */
        unsigned int  reserved      : 11; /* bit[21-31]: 保留。 */
    } reg;
} SOC_VPP_VHDACCTOTAL_UNION;
#endif
#define SOC_VPP_VHDACCTOTAL_acc_pix_total_START  (0)
#define SOC_VPP_VHDACCTOTAL_acc_pix_total_END    (20)


/*****************************************************************************
 结构名    : SOC_VPP_VHDIFIRCOEF01_UNION
 结构说明  : VHDIFIRCOEF01 寄存器结构定义。地址偏移量:0x0280，初值:0x00000000，宽度:32
 寄存器说明: IFIR滤波系数0和1
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  coef0    : 10; /* bit[0-9]  : IFIR滤波系数0。&#13; */
        unsigned int  reserved_0: 6;  /* bit[10-15]: 保留。 */
        unsigned int  coef1    : 10; /* bit[16-25]: IFIR滤波系数1。&#13; */
        unsigned int  reserved_1: 6;  /* bit[26-31]: 保留。 */
    } reg;
} SOC_VPP_VHDIFIRCOEF01_UNION;
#endif
#define SOC_VPP_VHDIFIRCOEF01_coef0_START     (0)
#define SOC_VPP_VHDIFIRCOEF01_coef0_END       (9)
#define SOC_VPP_VHDIFIRCOEF01_coef1_START     (16)
#define SOC_VPP_VHDIFIRCOEF01_coef1_END       (25)


/*****************************************************************************
 结构名    : SOC_VPP_VHDIFIRCOEF23_UNION
 结构说明  : VHDIFIRCOEF23 寄存器结构定义。地址偏移量:0x0284，初值:0x00000000，宽度:32
 寄存器说明: IFIR滤波系数2和3
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  coef2    : 10; /* bit[0-9]  : IFIR滤波系数2。&#13; */
        unsigned int  reserved_0: 6;  /* bit[10-15]: 保留。 */
        unsigned int  coef3    : 10; /* bit[16-25]: IFIR滤波系数3。&#13; */
        unsigned int  reserved_1: 6;  /* bit[26-31]: 保留。 */
    } reg;
} SOC_VPP_VHDIFIRCOEF23_UNION;
#endif
#define SOC_VPP_VHDIFIRCOEF23_coef2_START     (0)
#define SOC_VPP_VHDIFIRCOEF23_coef2_END       (9)
#define SOC_VPP_VHDIFIRCOEF23_coef3_START     (16)
#define SOC_VPP_VHDIFIRCOEF23_coef3_END       (25)


/*****************************************************************************
 结构名    : SOC_VPP_VHDIFIRCOEF45_UNION
 结构说明  : VHDIFIRCOEF45 寄存器结构定义。地址偏移量:0x0288，初值:0x00000000，宽度:32
 寄存器说明: IFIR滤波系数4和5
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  coef4    : 10; /* bit[0-9]  : IFIR滤波系数4。&#13; */
        unsigned int  reserved_0: 6;  /* bit[10-15]: 保留。 */
        unsigned int  coef5    : 10; /* bit[16-25]: IFIR滤波系数5。&#13; */
        unsigned int  reserved_1: 6;  /* bit[26-31]: 保留。 */
    } reg;
} SOC_VPP_VHDIFIRCOEF45_UNION;
#endif
#define SOC_VPP_VHDIFIRCOEF45_coef4_START     (0)
#define SOC_VPP_VHDIFIRCOEF45_coef4_END       (9)
#define SOC_VPP_VHDIFIRCOEF45_coef5_START     (16)
#define SOC_VPP_VHDIFIRCOEF45_coef5_END       (25)


/*****************************************************************************
 结构名    : SOC_VPP_VHDIFIRCOEF67_UNION
 结构说明  : VHDIFIRCOEF67 寄存器结构定义。地址偏移量:0x028C，初值:0x00000000，宽度:32
 寄存器说明: IFIR滤波系数6和7
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  coef6    : 10; /* bit[0-9]  : IFIR滤波系数6。&#13; */
        unsigned int  reserved_0: 6;  /* bit[10-15]: 保留。 */
        unsigned int  coef7    : 10; /* bit[16-25]: IFIR滤波系数7。&#13; */
        unsigned int  reserved_1: 6;  /* bit[26-31]: 保留。 */
    } reg;
} SOC_VPP_VHDIFIRCOEF67_UNION;
#endif
#define SOC_VPP_VHDIFIRCOEF67_coef6_START     (0)
#define SOC_VPP_VHDIFIRCOEF67_coef6_END       (9)
#define SOC_VPP_VHDIFIRCOEF67_coef7_START     (16)
#define SOC_VPP_VHDIFIRCOEF67_coef7_END       (25)


/*****************************************************************************
 结构名    : SOC_VPP_VHDDIECTRL_UNION
 结构说明  : VHDDIECTRL 寄存器结构定义。地址偏移量:0x0300，初值:0x00000000，宽度:32
 寄存器说明: de-interlace运算控制寄存器，为非即时寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  reserved_0    : 16; /* bit[0-15] : 保留。 */
        unsigned int  stinfo_stop   : 1;  /* bit[16]   : 静止次数更新暂停
                                                         0：静止次数正常更新；
                                                         &#13;1：静止次数暂停更新。 */
        unsigned int  die_rst       : 1;  /* bit[17]   : De-interlace复位使能。
                                                         0：die复位无效；
                                                         1：die复位有效。 */
        unsigned int  reserved_1    : 6;  /* bit[18-23]: 保留。 */
        unsigned int  die_chmmode   : 2;  /* bit[24-25]: De-interlace色度运算模式。
                                                         00：5场场模式
                                                         &#13;01：4场方式。
                                                         10：3场模式
                                                         11：保留 */
        unsigned int  die_lmmode    : 2;  /* bit[26-27]: De-interlace亮度运算模式。
                                                         00：5场场模式
                                                         &#13;01：4场方式。
                                                         10：3场模式
                                                         11：保留 */
        unsigned int  die_out_sel_c : 1;  /* bit[28]   : DIE色度输出选择。
                                                         0：输出插值模式数据；
                                                         1：输出间插模式数据。 */
        unsigned int  die_out_sel_l : 1;  /* bit[29]   : DIE亮度输出选择。
                                                         0：输出插值模式数据；
                                                         1：输出间插模式数据。 */
        unsigned int  die_chroma_en : 1;  /* bit[30]   : De-interlace色度使能控制。
                                                         0：禁止；&#13;1：使能。 */
        unsigned int  die_luma_en   : 1;  /* bit[31]   : De-interlace亮度使能控制。
                                                         0：禁止；
                                                         1：使能。 */
    } reg;
} SOC_VPP_VHDDIECTRL_UNION;
#endif
#define SOC_VPP_VHDDIECTRL_stinfo_stop_START    (16)
#define SOC_VPP_VHDDIECTRL_stinfo_stop_END      (16)
#define SOC_VPP_VHDDIECTRL_die_rst_START        (17)
#define SOC_VPP_VHDDIECTRL_die_rst_END          (17)
#define SOC_VPP_VHDDIECTRL_die_chmmode_START    (24)
#define SOC_VPP_VHDDIECTRL_die_chmmode_END      (25)
#define SOC_VPP_VHDDIECTRL_die_lmmode_START     (26)
#define SOC_VPP_VHDDIECTRL_die_lmmode_END       (27)
#define SOC_VPP_VHDDIECTRL_die_out_sel_c_START  (28)
#define SOC_VPP_VHDDIECTRL_die_out_sel_c_END    (28)
#define SOC_VPP_VHDDIECTRL_die_out_sel_l_START  (29)
#define SOC_VPP_VHDDIECTRL_die_out_sel_l_END    (29)
#define SOC_VPP_VHDDIECTRL_die_chroma_en_START  (30)
#define SOC_VPP_VHDDIECTRL_die_chroma_en_END    (30)
#define SOC_VPP_VHDDIECTRL_die_luma_en_START    (31)
#define SOC_VPP_VHDDIECTRL_die_luma_en_END      (31)


/*****************************************************************************
 结构名    : SOC_VPP_VHDDIELMA0_UNION
 结构说明  : VHDDIELMA0 寄存器结构定义。地址偏移量:0x0308，初值:0x00000000，宽度:32
 寄存器说明: de-interlace亮度运算控制寄存器0。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  luma_qtbl1      : 8;  /* bit[0-7]  : 亮度量化表1，表示象素在[0,luma_qtbl1)范围内被量化成00 */
        unsigned int  luma_qtbl2      : 8;  /* bit[8-15] : 亮度量化表2，表示象素在[luma_qtbl1,luma_qtbl2)范围内被量化成01 */
        unsigned int  luma_qtbl3      : 8;  /* bit[16-23]: 亮度量化表3，表示象素在[luma_qtbl2,luma_qtbl3)范围内被量化成10，
                                                           在[luma_qtbl3,256)范围内被量化成11。 */
        unsigned int  scale_ratio_ppd : 6;  /* bit[24-29]: 亮度本地信息权重，U6.4格式 */
        unsigned int  reserved        : 2;  /* bit[30-31]: 保留。 */
    } reg;
} SOC_VPP_VHDDIELMA0_UNION;
#endif
#define SOC_VPP_VHDDIELMA0_luma_qtbl1_START       (0)
#define SOC_VPP_VHDDIELMA0_luma_qtbl1_END         (7)
#define SOC_VPP_VHDDIELMA0_luma_qtbl2_START       (8)
#define SOC_VPP_VHDDIELMA0_luma_qtbl2_END         (15)
#define SOC_VPP_VHDDIELMA0_luma_qtbl3_START       (16)
#define SOC_VPP_VHDDIELMA0_luma_qtbl3_END         (23)
#define SOC_VPP_VHDDIELMA0_scale_ratio_ppd_START  (24)
#define SOC_VPP_VHDDIELMA0_scale_ratio_ppd_END    (29)


/*****************************************************************************
 结构名    : SOC_VPP_VHDDIELMA1_UNION
 结构说明  : VHDDIELMA1 寄存器结构定义。地址偏移量:0x030C，初值:0x00000000，宽度:32
 寄存器说明: de-interlace亮度运算控制寄存器1。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  luma_reqtbl0 : 8;  /* bit[0-7]  : 亮度历史信息反量化表，当mh＝0时，对应的值 */
        unsigned int  luma_reqtbl1 : 8;  /* bit[8-15] : 亮度历史信息反量化表，当mh＝1时，对应的值 */
        unsigned int  luma_reqtbl2 : 8;  /* bit[16-23]: 亮度历史信息反量化表，当mh＝2时，对应的值 */
        unsigned int  luma_reqtbl3 : 8;  /* bit[24-31]: 亮度历史信息反量化表，当mh＝3时，对应的值 */
    } reg;
} SOC_VPP_VHDDIELMA1_UNION;
#endif
#define SOC_VPP_VHDDIELMA1_luma_reqtbl0_START  (0)
#define SOC_VPP_VHDDIELMA1_luma_reqtbl0_END    (7)
#define SOC_VPP_VHDDIELMA1_luma_reqtbl1_START  (8)
#define SOC_VPP_VHDDIELMA1_luma_reqtbl1_END    (15)
#define SOC_VPP_VHDDIELMA1_luma_reqtbl2_START  (16)
#define SOC_VPP_VHDDIELMA1_luma_reqtbl2_END    (23)
#define SOC_VPP_VHDDIELMA1_luma_reqtbl3_START  (24)
#define SOC_VPP_VHDDIELMA1_luma_reqtbl3_END    (31)


/*****************************************************************************
 结构名    : SOC_VPP_VHDDIELMA2_UNION
 结构说明  : VHDDIELMA2 寄存器结构定义。地址偏移量:0x0310，初值:0x00000000，宽度:32
 寄存器说明: de-interlace亮度运算控制寄存器2。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  luma_win_size   : 2;  /* bit[0-1]  : 亮度ma的水平窗口大小：
                                                           00：单点（复位值）
                                                           01：半径为1的窗口
                                                           10：半径为2的窗口
                                                           11：保留 */
        unsigned int  luma_scesdf_max : 1;  /* bit[2]    : 亮度SAD时的SCE和SDF选择：
                                                           0：最小值
                                                           1：最大值 */
        unsigned int  die_st_qrst_en  : 1;  /* bit[3]    : 亮度历史信息qr或st使能
                                                           0：禁止
                                                           1：使能 */
        unsigned int  die_st_n_en     : 1;  /* bit[4]    : 亮度历史信息N使能
                                                           0：禁止
                                                           1：使能 */
        unsigned int  die_st_m_en     : 1;  /* bit[5]    : 亮度历史信息M使能
                                                           0：禁止
                                                           1：使能 */
        unsigned int  die_st_l_en     : 1;  /* bit[6]    : 亮度历史信息L使能
                                                           0：禁止
                                                           1：使能 */
        unsigned int  die_st_k_en     : 1;  /* bit[7]    : 读亮度历史信息K使能
                                                           0：禁止
                                                           1：使能 */
        unsigned int  luma_qrst_max   : 1;  /* bit[8]    : 亮度历史信息QR或ST的最大值选择
                                                           0：表示最小值
                                                           1：表示最大值 */
        unsigned int  luma_mf_max     : 1;  /* bit[9]    : 亮度动静判决结果
                                                           0：亮度mf为统计值
                                                           1：亮度mf固定为最大值 */
        unsigned int  chroma_mf_max   : 1;  /* bit[10]   : 色度动静判决结果
                                                           0：色度mf为统计值
                                                           1：色度mf固定为最大值 */
        unsigned int  reserved        : 18; /* bit[11-28]: 保留。 */
        unsigned int  die_st_mode     : 3;  /* bit[29-31]: 验证环境专用，st模式寄存器 */
    } reg;
} SOC_VPP_VHDDIELMA2_UNION;
#endif
#define SOC_VPP_VHDDIELMA2_luma_win_size_START    (0)
#define SOC_VPP_VHDDIELMA2_luma_win_size_END      (1)
#define SOC_VPP_VHDDIELMA2_luma_scesdf_max_START  (2)
#define SOC_VPP_VHDDIELMA2_luma_scesdf_max_END    (2)
#define SOC_VPP_VHDDIELMA2_die_st_qrst_en_START   (3)
#define SOC_VPP_VHDDIELMA2_die_st_qrst_en_END     (3)
#define SOC_VPP_VHDDIELMA2_die_st_n_en_START      (4)
#define SOC_VPP_VHDDIELMA2_die_st_n_en_END        (4)
#define SOC_VPP_VHDDIELMA2_die_st_m_en_START      (5)
#define SOC_VPP_VHDDIELMA2_die_st_m_en_END        (5)
#define SOC_VPP_VHDDIELMA2_die_st_l_en_START      (6)
#define SOC_VPP_VHDDIELMA2_die_st_l_en_END        (6)
#define SOC_VPP_VHDDIELMA2_die_st_k_en_START      (7)
#define SOC_VPP_VHDDIELMA2_die_st_k_en_END        (7)
#define SOC_VPP_VHDDIELMA2_luma_qrst_max_START    (8)
#define SOC_VPP_VHDDIELMA2_luma_qrst_max_END      (8)
#define SOC_VPP_VHDDIELMA2_luma_mf_max_START      (9)
#define SOC_VPP_VHDDIELMA2_luma_mf_max_END        (9)
#define SOC_VPP_VHDDIELMA2_chroma_mf_max_START    (10)
#define SOC_VPP_VHDDIELMA2_chroma_mf_max_END      (10)
#define SOC_VPP_VHDDIELMA2_die_st_mode_START      (29)
#define SOC_VPP_VHDDIELMA2_die_st_mode_END        (31)


/*****************************************************************************
 结构名    : SOC_VPP_VHDDIEINTEN_UNION
 结构说明  : VHDDIEINTEN 寄存器结构定义。地址偏移量:0x0314，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  dir_inten     : 4;  /* bit[0-3]  :  */
        unsigned int  reserved_0    : 3;  /* bit[4-6]  :  */
        unsigned int  dir_ck_enh    : 1;  /* bit[7]    : 方向插值时，bound check增强使能。
                                                         0：增强不使能（MPW版本插值方式）；
                                                         1：增强使能（pilot版本插值方式）； */
        unsigned int  dir_inten_ver : 4;  /* bit[8-11] :  */
        unsigned int  reserved_1    : 4;  /* bit[12-15]:  */
        unsigned int  ver_min_inten : 16; /* bit[16-31]:  */
    } reg;
} SOC_VPP_VHDDIEINTEN_UNION;
#endif
#define SOC_VPP_VHDDIEINTEN_dir_inten_START      (0)
#define SOC_VPP_VHDDIEINTEN_dir_inten_END        (3)
#define SOC_VPP_VHDDIEINTEN_dir_ck_enh_START     (7)
#define SOC_VPP_VHDDIEINTEN_dir_ck_enh_END       (7)
#define SOC_VPP_VHDDIEINTEN_dir_inten_ver_START  (8)
#define SOC_VPP_VHDDIEINTEN_dir_inten_ver_END    (11)
#define SOC_VPP_VHDDIEINTEN_ver_min_inten_START  (16)
#define SOC_VPP_VHDDIEINTEN_ver_min_inten_END    (31)


/*****************************************************************************
 结构名    : SOC_VPP_VHDDIESCALE_UNION
 结构说明  : VHDDIESCALE 寄存器结构定义。地址偏移量:0x0318，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  range_scale : 8;  /* bit[0-7] :  */
        unsigned int  reserved    : 24; /* bit[8-31]:  */
    } reg;
} SOC_VPP_VHDDIESCALE_UNION;
#endif
#define SOC_VPP_VHDDIESCALE_range_scale_START  (0)
#define SOC_VPP_VHDDIESCALE_range_scale_END    (7)


/*****************************************************************************
 结构名    : SOC_VPP_VHDDIECHECK1_UNION
 结构说明  : VHDDIECHECK1 寄存器结构定义。地址偏移量:0x031C，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ck_max_range  : 6;  /* bit[0-5]  :  */
        unsigned int  reserved_0    : 2;  /* bit[6-7]  :  */
        unsigned int  ck_range_gain : 4;  /* bit[8-11] :  */
        unsigned int  reserved_1    : 4;  /* bit[12-15]:  */
        unsigned int  ck_gain       : 4;  /* bit[16-19]:  */
        unsigned int  reserved_2    : 12; /* bit[20-31]:  */
    } reg;
} SOC_VPP_VHDDIECHECK1_UNION;
#endif
#define SOC_VPP_VHDDIECHECK1_ck_max_range_START   (0)
#define SOC_VPP_VHDDIECHECK1_ck_max_range_END     (5)
#define SOC_VPP_VHDDIECHECK1_ck_range_gain_START  (8)
#define SOC_VPP_VHDDIECHECK1_ck_range_gain_END    (11)
#define SOC_VPP_VHDDIECHECK1_ck_gain_START        (16)
#define SOC_VPP_VHDDIECHECK1_ck_gain_END          (19)


/*****************************************************************************
 结构名    : SOC_VPP_VHDDIECHECK2_UNION
 结构说明  : VHDDIECHECK2 寄存器结构定义。地址偏移量:0x0320，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ck_max_range  : 6;  /* bit[0-5]  :  */
        unsigned int  reserved_0    : 2;  /* bit[6-7]  :  */
        unsigned int  ck_range_gain : 4;  /* bit[8-11] :  */
        unsigned int  reserved_1    : 4;  /* bit[12-15]:  */
        unsigned int  ck_gain       : 4;  /* bit[16-19]:  */
        unsigned int  reserved_2    : 12; /* bit[20-31]:  */
    } reg;
} SOC_VPP_VHDDIECHECK2_UNION;
#endif
#define SOC_VPP_VHDDIECHECK2_ck_max_range_START   (0)
#define SOC_VPP_VHDDIECHECK2_ck_max_range_END     (5)
#define SOC_VPP_VHDDIECHECK2_ck_range_gain_START  (8)
#define SOC_VPP_VHDDIECHECK2_ck_range_gain_END    (11)
#define SOC_VPP_VHDDIECHECK2_ck_gain_START        (16)
#define SOC_VPP_VHDDIECHECK2_ck_gain_END          (19)


/*****************************************************************************
 结构名    : SOC_VPP_VHDDIEDIR0_3_UNION
 结构说明  : VHDDIEDIR0_3 寄存器结构定义。地址偏移量:0x324，初值:0x00000000，宽度:32
 寄存器说明: 方向0～3乘数
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  dir0_mult : 6;  /* bit[0-5]  : 方向0乘数 */
        unsigned int  reserved_0: 2;  /* bit[6-7]  : 保留 */
        unsigned int  dir1_mult : 6;  /* bit[8-13] : 方向1乘数 */
        unsigned int  reserved_1: 2;  /* bit[14-15]: 保留 */
        unsigned int  dir2_mult : 6;  /* bit[16-21]: 方向2乘数 */
        unsigned int  reserved_2: 2;  /* bit[22-23]: 保留 */
        unsigned int  dir3_mult : 6;  /* bit[24-29]: 方向3乘数 */
        unsigned int  reserved_3: 2;  /* bit[30-31]: 保留 */
    } reg;
} SOC_VPP_VHDDIEDIR0_3_UNION;
#endif
#define SOC_VPP_VHDDIEDIR0_3_dir0_mult_START  (0)
#define SOC_VPP_VHDDIEDIR0_3_dir0_mult_END    (5)
#define SOC_VPP_VHDDIEDIR0_3_dir1_mult_START  (8)
#define SOC_VPP_VHDDIEDIR0_3_dir1_mult_END    (13)
#define SOC_VPP_VHDDIEDIR0_3_dir2_mult_START  (16)
#define SOC_VPP_VHDDIEDIR0_3_dir2_mult_END    (21)
#define SOC_VPP_VHDDIEDIR0_3_dir3_mult_START  (24)
#define SOC_VPP_VHDDIEDIR0_3_dir3_mult_END    (29)


/*****************************************************************************
 结构名    : SOC_VPP_VHDDIEDIR4_7_UNION
 结构说明  : VHDDIEDIR4_7 寄存器结构定义。地址偏移量:0x328，初值:0x00000000，宽度:32
 寄存器说明: 方向4～7乘数
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  dir4_mult : 6;  /* bit[0-5]  : 方向4乘数 */
        unsigned int  reserved_0: 2;  /* bit[6-7]  : 保留 */
        unsigned int  dir5_mult : 6;  /* bit[8-13] : 方向5乘数 */
        unsigned int  reserved_1: 2;  /* bit[14-15]: 保留 */
        unsigned int  dir6_mult : 6;  /* bit[16-21]: 方向6乘数 */
        unsigned int  reserved_2: 2;  /* bit[22-23]: 保留 */
        unsigned int  dir7_mult : 6;  /* bit[24-29]: 方向7乘数 */
        unsigned int  reserved_3: 2;  /* bit[30-31]: 保留 */
    } reg;
} SOC_VPP_VHDDIEDIR4_7_UNION;
#endif
#define SOC_VPP_VHDDIEDIR4_7_dir4_mult_START  (0)
#define SOC_VPP_VHDDIEDIR4_7_dir4_mult_END    (5)
#define SOC_VPP_VHDDIEDIR4_7_dir5_mult_START  (8)
#define SOC_VPP_VHDDIEDIR4_7_dir5_mult_END    (13)
#define SOC_VPP_VHDDIEDIR4_7_dir6_mult_START  (16)
#define SOC_VPP_VHDDIEDIR4_7_dir6_mult_END    (21)
#define SOC_VPP_VHDDIEDIR4_7_dir7_mult_START  (24)
#define SOC_VPP_VHDDIEDIR4_7_dir7_mult_END    (29)


/*****************************************************************************
 结构名    : SOC_VPP_VHDDIEDIR8_11_UNION
 结构说明  : VHDDIEDIR8_11 寄存器结构定义。地址偏移量:0x32C，初值:0x00000000，宽度:32
 寄存器说明: 方向8～11乘数
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  dir8_mult  : 6;  /* bit[0-5]  : 方向8乘数 */
        unsigned int  reserved_0 : 2;  /* bit[6-7]  : 保留 */
        unsigned int  dir9_mult  : 6;  /* bit[8-13] : 方向9乘数 */
        unsigned int  reserved_1 : 2;  /* bit[14-15]: 保留 */
        unsigned int  dir10_mult : 6;  /* bit[16-21]: 方向10乘数 */
        unsigned int  reserved_2 : 2;  /* bit[22-23]: 保留 */
        unsigned int  dir11_mult : 6;  /* bit[24-29]: 方向11乘数 */
        unsigned int  reserved_3 : 2;  /* bit[30-31]: 保留 */
    } reg;
} SOC_VPP_VHDDIEDIR8_11_UNION;
#endif
#define SOC_VPP_VHDDIEDIR8_11_dir8_mult_START   (0)
#define SOC_VPP_VHDDIEDIR8_11_dir8_mult_END     (5)
#define SOC_VPP_VHDDIEDIR8_11_dir9_mult_START   (8)
#define SOC_VPP_VHDDIEDIR8_11_dir9_mult_END     (13)
#define SOC_VPP_VHDDIEDIR8_11_dir10_mult_START  (16)
#define SOC_VPP_VHDDIEDIR8_11_dir10_mult_END    (21)
#define SOC_VPP_VHDDIEDIR8_11_dir11_mult_START  (24)
#define SOC_VPP_VHDDIEDIR8_11_dir11_mult_END    (29)


/*****************************************************************************
 结构名    : SOC_VPP_VHDDIEDIR12_14_UNION
 结构说明  : VHDDIEDIR12_14 寄存器结构定义。地址偏移量:0x330，初值:0x00000000，宽度:32
 寄存器说明: 方向12～14乘数
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  dir12_mult : 6;  /* bit[0-5]  : 方向12乘数 */
        unsigned int  reserved_0 : 2;  /* bit[6-7]  : 保留 */
        unsigned int  dir13_mult : 6;  /* bit[8-13] : 方向13乘数 */
        unsigned int  reserved_1 : 2;  /* bit[14-15]: 保留 */
        unsigned int  dir14_mult : 6;  /* bit[16-21]: 方向14乘数 */
        unsigned int  reserved_2 : 10; /* bit[22-31]: 保留 */
    } reg;
} SOC_VPP_VHDDIEDIR12_14_UNION;
#endif
#define SOC_VPP_VHDDIEDIR12_14_dir12_mult_START  (0)
#define SOC_VPP_VHDDIEDIR12_14_dir12_mult_END    (5)
#define SOC_VPP_VHDDIEDIR12_14_dir13_mult_START  (8)
#define SOC_VPP_VHDDIEDIR12_14_dir13_mult_END    (13)
#define SOC_VPP_VHDDIEDIR12_14_dir14_mult_START  (16)
#define SOC_VPP_VHDDIEDIR12_14_dir14_mult_END    (21)


/*****************************************************************************
 结构名    : SOC_VPP_VHDDIESTA_UNION
 结构说明  : VHDDIESTA 寄存器结构定义。地址偏移量:0x0338，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  die_ref_field : 1;  /* bit[0]   :  */
        unsigned int  reserved      : 31; /* bit[1-31]:  */
    } reg;
} SOC_VPP_VHDDIESTA_UNION;
#endif
#define SOC_VPP_VHDDIESTA_die_ref_field_START  (0)
#define SOC_VPP_VHDDIESTA_die_ref_field_END    (0)


/*****************************************************************************
 结构名    : SOC_VPP_VHDDIESTKADDR_UNION
 结构说明  : VHDDIESTKADDR 寄存器结构定义。地址偏移量:0x03A4，初值:0x00000000，宽度:32
 寄存器说明: DIE历史信息寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  die_k_addr : 32; /* bit[0-31]: k的历史信息起始地址，分配空间大小范围为(w+15)/16 * 1080(32bit) */
    } reg;
} SOC_VPP_VHDDIESTKADDR_UNION;
#endif
#define SOC_VPP_VHDDIESTKADDR_die_k_addr_START  (0)
#define SOC_VPP_VHDDIESTKADDR_die_k_addr_END    (31)


/*****************************************************************************
 结构名    : SOC_VPP_VHDDIESTLADDR_UNION
 结构说明  : VHDDIESTLADDR 寄存器结构定义。地址偏移量:0x03A8，初值:0x00000000，宽度:32
 寄存器说明: DIE历史信息寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  die_l_addr : 32; /* bit[0-31]: l的历史信息起始地址，分配空间大小范围为(w+15)/16 * 1080(32bit) */
    } reg;
} SOC_VPP_VHDDIESTLADDR_UNION;
#endif
#define SOC_VPP_VHDDIESTLADDR_die_l_addr_START  (0)
#define SOC_VPP_VHDDIESTLADDR_die_l_addr_END    (31)


/*****************************************************************************
 结构名    : SOC_VPP_VHDDIESTMADDR_UNION
 结构说明  : VHDDIESTMADDR 寄存器结构定义。地址偏移量:0x03AC，初值:0x00000000，宽度:32
 寄存器说明: DIE历史信息寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  die_m_addr : 32; /* bit[0-31]: m的历史信息起始地址，分配空间大小范围为(w+15)/16 * 1080(32bit) */
    } reg;
} SOC_VPP_VHDDIESTMADDR_UNION;
#endif
#define SOC_VPP_VHDDIESTMADDR_die_m_addr_START  (0)
#define SOC_VPP_VHDDIESTMADDR_die_m_addr_END    (31)


/*****************************************************************************
 结构名    : SOC_VPP_VHDDIESTNADDR_UNION
 结构说明  : VHDDIESTNADDR 寄存器结构定义。地址偏移量:0x03B0，初值:0x00000000，宽度:32
 寄存器说明: DIE历史信息寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  die_n_addr : 32; /* bit[0-31]: n的历史信息起始地址，分配空间大小范围为(w+15)/16 * 1080(32bit) */
    } reg;
} SOC_VPP_VHDDIESTNADDR_UNION;
#endif
#define SOC_VPP_VHDDIESTNADDR_die_n_addr_START  (0)
#define SOC_VPP_VHDDIESTNADDR_die_n_addr_END    (31)


/*****************************************************************************
 结构名    : SOC_VPP_VHDDIESTSQTRADDR_UNION
 结构说明  : VHDDIESTSQTRADDR 寄存器结构定义。地址偏移量:0x03B4，初值:0x00000000，宽度:32
 寄存器说明: DIE历史信息寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  die_sqtr_addr : 32; /* bit[0-31]: sqtr的历史信息起始地址，分配空间大小范围为(w+15)/16 * 1080(32bit) */
    } reg;
} SOC_VPP_VHDDIESTSQTRADDR_UNION;
#endif
#define SOC_VPP_VHDDIESTSQTRADDR_die_sqtr_addr_START  (0)
#define SOC_VPP_VHDDIESTSQTRADDR_die_sqtr_addr_END    (31)


/*****************************************************************************
 结构名    : SOC_VPP_VHDCCRSCLR0_UNION
 结构说明  : VHDCCRSCLR0 寄存器结构定义。地址偏移量:0x03B8，初值:0x00000000，宽度:32
 寄存器说明: 色度crosscolor寄存器0
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  no_ccr_detect_thd : 8;  /* bit[0-7]  : 亮串色检测门限 */
        unsigned int  no_ccr_detect_max : 8;  /* bit[8-15] : 亮串色检测最大值 */
        unsigned int  chroma_ma_offset  : 8;  /* bit[16-23]: 色度运动信息相对亮度运动偏移值 */
        unsigned int  chroma_ccr_en     : 1;  /* bit[24]   : 去亮色串扰使能
                                                             0：禁止
                                                             1：使能 */
        unsigned int  reserved          : 7;  /* bit[25-31]: 保留。 */
    } reg;
} SOC_VPP_VHDCCRSCLR0_UNION;
#endif
#define SOC_VPP_VHDCCRSCLR0_no_ccr_detect_thd_START  (0)
#define SOC_VPP_VHDCCRSCLR0_no_ccr_detect_thd_END    (7)
#define SOC_VPP_VHDCCRSCLR0_no_ccr_detect_max_START  (8)
#define SOC_VPP_VHDCCRSCLR0_no_ccr_detect_max_END    (15)
#define SOC_VPP_VHDCCRSCLR0_chroma_ma_offset_START   (16)
#define SOC_VPP_VHDCCRSCLR0_chroma_ma_offset_END     (23)
#define SOC_VPP_VHDCCRSCLR0_chroma_ccr_en_START      (24)
#define SOC_VPP_VHDCCRSCLR0_chroma_ccr_en_END        (24)


/*****************************************************************************
 结构名    : SOC_VPP_VHDCCRSCLR1_UNION
 结构说明  : VHDCCRSCLR1 寄存器结构定义。地址偏移量:0x03BC，初值:0x00000000，宽度:32
 寄存器说明: 色度crosscolor寄存器1
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  max_xchroma         : 8;  /* bit[0-7]  : 色度允许改变最大值 */
        unsigned int  no_ccr_detect_blend : 4;  /* bit[8-11] : 亮串色检测混合比例 */
        unsigned int  reserved            : 4;  /* bit[12-15]: 保留。 */
        unsigned int  similar_thd         : 8;  /* bit[16-23]: 相似度门限 */
        unsigned int  similar_max         : 8;  /* bit[24-31]: 相似度最大值 */
    } reg;
} SOC_VPP_VHDCCRSCLR1_UNION;
#endif
#define SOC_VPP_VHDCCRSCLR1_max_xchroma_START          (0)
#define SOC_VPP_VHDCCRSCLR1_max_xchroma_END            (7)
#define SOC_VPP_VHDCCRSCLR1_no_ccr_detect_blend_START  (8)
#define SOC_VPP_VHDCCRSCLR1_no_ccr_detect_blend_END    (11)
#define SOC_VPP_VHDCCRSCLR1_similar_thd_START          (16)
#define SOC_VPP_VHDCCRSCLR1_similar_thd_END            (23)
#define SOC_VPP_VHDCCRSCLR1_similar_max_START          (24)
#define SOC_VPP_VHDCCRSCLR1_similar_max_END            (31)


/*****************************************************************************
 结构名    : SOC_VPP_VHDPDCTRL_UNION
 结构说明  : VHDPDCTRL 寄存器结构定义。地址偏移量:0x0400，初值:0x00000000，宽度:32
 寄存器说明: PullDown控制寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  reserved_0        : 20; /* bit[0-19] : 保留。 */
        unsigned int  edge_smooth_ratio : 8;  /* bit[20-27]: 边缘平滑乘数 */
        unsigned int  reserved_1        : 1;  /* bit[28]   : 保留。 */
        unsigned int  dir_mch_c         : 1;  /* bit[29]   : 色度匹配场方向参数
                                                             0：选择参考场的前一场
                                                             1：选择参考场的后一场 */
        unsigned int  edge_smooth_en    : 1;  /* bit[30]   : 边缘平滑使能，当Pulldown使能时（die_out_sel选择间插模式）有效：
                                                             0：不使能；
                                                             1：使能； */
        unsigned int  dir_mch_l         : 1;  /* bit[31]   : 亮度匹配场方向参数
                                                             0：选择参考场的前一场
                                                             1：选择参考场的后一场 */
    } reg;
} SOC_VPP_VHDPDCTRL_UNION;
#endif
#define SOC_VPP_VHDPDCTRL_edge_smooth_ratio_START  (20)
#define SOC_VPP_VHDPDCTRL_edge_smooth_ratio_END    (27)
#define SOC_VPP_VHDPDCTRL_dir_mch_c_START          (29)
#define SOC_VPP_VHDPDCTRL_dir_mch_c_END            (29)
#define SOC_VPP_VHDPDCTRL_edge_smooth_en_START     (30)
#define SOC_VPP_VHDPDCTRL_edge_smooth_en_END       (30)
#define SOC_VPP_VHDPDCTRL_dir_mch_l_START          (31)
#define SOC_VPP_VHDPDCTRL_dir_mch_l_END            (31)


/*****************************************************************************
 结构名    : SOC_VPP_VHDPDBLKPOS0_UNION
 结构说明  : VHDPDBLKPOS0 寄存器结构定义。地址偏移量:0x0404，初值:0x00000000，宽度:32
 寄存器说明: PullDown分区座标0
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  blk_x    : 12; /* bit[0-11] :  */
        unsigned int  blk_y    : 12; /* bit[12-23]:  */
        unsigned int  reserved : 8;  /* bit[24-31]: 保留。 */
    } reg;
} SOC_VPP_VHDPDBLKPOS0_UNION;
#endif
#define SOC_VPP_VHDPDBLKPOS0_blk_x_START     (0)
#define SOC_VPP_VHDPDBLKPOS0_blk_x_END       (11)
#define SOC_VPP_VHDPDBLKPOS0_blk_y_START     (12)
#define SOC_VPP_VHDPDBLKPOS0_blk_y_END       (23)


/*****************************************************************************
 结构名    : SOC_VPP_VHDPDBLKPOS1_UNION
 结构说明  : VHDPDBLKPOS1 寄存器结构定义。地址偏移量:0x0408，初值:0x00000000，宽度:32
 寄存器说明: PullDown分区座标0
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  blk_x    : 12; /* bit[0-11] :  */
        unsigned int  blk_y    : 12; /* bit[12-23]:  */
        unsigned int  reserved : 8;  /* bit[24-31]: 保留。 */
    } reg;
} SOC_VPP_VHDPDBLKPOS1_UNION;
#endif
#define SOC_VPP_VHDPDBLKPOS1_blk_x_START     (0)
#define SOC_VPP_VHDPDBLKPOS1_blk_x_END       (11)
#define SOC_VPP_VHDPDBLKPOS1_blk_y_START     (12)
#define SOC_VPP_VHDPDBLKPOS1_blk_y_END       (23)


/*****************************************************************************
 结构名    : SOC_VPP_VHDPDBLKTHD_UNION
 结构说明  : VHDPDBLKTHD 寄存器结构定义。地址偏移量:0x040C，初值:0x00000000，宽度:32
 寄存器说明: PullDown静止区域门限
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  stillblk_thd    : 8;  /* bit[0-7]  :  */
        unsigned int  diff_movblk_thd : 8;  /* bit[8-15] :  */
        unsigned int  reserved        : 16; /* bit[16-31]: 保留。 */
    } reg;
} SOC_VPP_VHDPDBLKTHD_UNION;
#endif
#define SOC_VPP_VHDPDBLKTHD_stillblk_thd_START     (0)
#define SOC_VPP_VHDPDBLKTHD_stillblk_thd_END       (7)
#define SOC_VPP_VHDPDBLKTHD_diff_movblk_thd_START  (8)
#define SOC_VPP_VHDPDBLKTHD_diff_movblk_thd_END    (15)


/*****************************************************************************
 结构名    : SOC_VPP_VHDPDHISTTHD_UNION
 结构说明  : VHDPDHISTTHD 寄存器结构定义。地址偏移量:0x0410，初值:0x00000000，宽度:32
 寄存器说明: PullDown直方图统计门限
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  hist_thd0 : 8;  /* bit[0-7]  :  */
        unsigned int  hist_thd1 : 8;  /* bit[8-15] :  */
        unsigned int  hist_thd2 : 8;  /* bit[16-23]:  */
        unsigned int  hist_thd3 : 8;  /* bit[24-31]:  */
    } reg;
} SOC_VPP_VHDPDHISTTHD_UNION;
#endif
#define SOC_VPP_VHDPDHISTTHD_hist_thd0_START  (0)
#define SOC_VPP_VHDPDHISTTHD_hist_thd0_END    (7)
#define SOC_VPP_VHDPDHISTTHD_hist_thd1_START  (8)
#define SOC_VPP_VHDPDHISTTHD_hist_thd1_END    (15)
#define SOC_VPP_VHDPDHISTTHD_hist_thd2_START  (16)
#define SOC_VPP_VHDPDHISTTHD_hist_thd2_END    (23)
#define SOC_VPP_VHDPDHISTTHD_hist_thd3_START  (24)
#define SOC_VPP_VHDPDHISTTHD_hist_thd3_END    (31)


/*****************************************************************************
 结构名    : SOC_VPP_VHDPDUMTHD_UNION
 结构说明  : VHDPDUMTHD 寄存器结构定义。地址偏移量:0x0414，初值:0x00000000，宽度:32
 寄存器说明: PullDown不期望运动统计门限
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  um_thd0  : 8;  /* bit[0-7]  :  */
        unsigned int  um_thd1  : 8;  /* bit[8-15] :  */
        unsigned int  um_thd2  : 8;  /* bit[16-23]:  */
        unsigned int  reserved : 8;  /* bit[24-31]: 保留。 */
    } reg;
} SOC_VPP_VHDPDUMTHD_UNION;
#endif
#define SOC_VPP_VHDPDUMTHD_um_thd0_START   (0)
#define SOC_VPP_VHDPDUMTHD_um_thd0_END     (7)
#define SOC_VPP_VHDPDUMTHD_um_thd1_START   (8)
#define SOC_VPP_VHDPDUMTHD_um_thd1_END     (15)
#define SOC_VPP_VHDPDUMTHD_um_thd2_START   (16)
#define SOC_VPP_VHDPDUMTHD_um_thd2_END     (23)


/*****************************************************************************
 结构名    : SOC_VPP_VHDPDPCCCORING_UNION
 结构说明  : VHDPDPCCCORING 寄存器结构定义。地址偏移量:0x0418，初值:0x00000000，宽度:32
 寄存器说明: PullDown极性改变核心统计门限
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  coring_tkr  : 8;  /* bit[0-7]  :  */
        unsigned int  coring_norm : 8;  /* bit[8-15] :  */
        unsigned int  coring_blk  : 8;  /* bit[16-23]:  */
        unsigned int  reserved    : 8;  /* bit[24-31]: 保留。 */
    } reg;
} SOC_VPP_VHDPDPCCCORING_UNION;
#endif
#define SOC_VPP_VHDPDPCCCORING_coring_tkr_START   (0)
#define SOC_VPP_VHDPDPCCCORING_coring_tkr_END     (7)
#define SOC_VPP_VHDPDPCCCORING_coring_norm_START  (8)
#define SOC_VPP_VHDPDPCCCORING_coring_norm_END    (15)
#define SOC_VPP_VHDPDPCCCORING_coring_blk_START   (16)
#define SOC_VPP_VHDPDPCCCORING_coring_blk_END     (23)


/*****************************************************************************
 结构名    : SOC_VPP_VHDPDPCCHTHD_UNION
 结构说明  : VHDPDPCCHTHD 寄存器结构定义。地址偏移量:0x041C，初值:0x00000000，宽度:32
 寄存器说明: PullDown极性改变水平统计门限
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  pcc_hthd : 8;  /* bit[0-7] :  */
        unsigned int  reserved : 24; /* bit[8-31]: 保留。 */
    } reg;
} SOC_VPP_VHDPDPCCHTHD_UNION;
#endif
#define SOC_VPP_VHDPDPCCHTHD_pcc_hthd_START  (0)
#define SOC_VPP_VHDPDPCCHTHD_pcc_hthd_END    (7)


/*****************************************************************************
 结构名    : SOC_VPP_VHDPDPCCVTHD_UNION
 结构说明  : VHDPDPCCVTHD 寄存器结构定义。地址偏移量:0x0420，初值:0x00000000，宽度:32
 寄存器说明: PullDown极性改变垂直统计门限
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  pcc_vthd0 : 8;  /* bit[0-7]  :  */
        unsigned int  pcc_vthd1 : 8;  /* bit[8-15] :  */
        unsigned int  pcc_vthd2 : 8;  /* bit[16-23]:  */
        unsigned int  pcc_vthd3 : 8;  /* bit[24-31]: 保留。 */
    } reg;
} SOC_VPP_VHDPDPCCVTHD_UNION;
#endif
#define SOC_VPP_VHDPDPCCVTHD_pcc_vthd0_START  (0)
#define SOC_VPP_VHDPDPCCVTHD_pcc_vthd0_END    (7)
#define SOC_VPP_VHDPDPCCVTHD_pcc_vthd1_START  (8)
#define SOC_VPP_VHDPDPCCVTHD_pcc_vthd1_END    (15)
#define SOC_VPP_VHDPDPCCVTHD_pcc_vthd2_START  (16)
#define SOC_VPP_VHDPDPCCVTHD_pcc_vthd2_END    (23)
#define SOC_VPP_VHDPDPCCVTHD_pcc_vthd3_START  (24)
#define SOC_VPP_VHDPDPCCVTHD_pcc_vthd3_END    (31)


/*****************************************************************************
 结构名    : SOC_VPP_VHDPDITDIFFVTHD_UNION
 结构说明  : VHDPDITDIFFVTHD 寄存器结构定义。地址偏移量:0x0424，初值:0x00000000，宽度:32
 寄存器说明: Pulldown
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  itdiff_vthd0 : 8;  /* bit[0-7]  :  */
        unsigned int  itdiff_vthd1 : 8;  /* bit[8-15] :  */
        unsigned int  itdiff_vthd2 : 8;  /* bit[16-23]:  */
        unsigned int  itdiff_vthd3 : 8;  /* bit[24-31]: 保留。 */
    } reg;
} SOC_VPP_VHDPDITDIFFVTHD_UNION;
#endif
#define SOC_VPP_VHDPDITDIFFVTHD_itdiff_vthd0_START  (0)
#define SOC_VPP_VHDPDITDIFFVTHD_itdiff_vthd0_END    (7)
#define SOC_VPP_VHDPDITDIFFVTHD_itdiff_vthd1_START  (8)
#define SOC_VPP_VHDPDITDIFFVTHD_itdiff_vthd1_END    (15)
#define SOC_VPP_VHDPDITDIFFVTHD_itdiff_vthd2_START  (16)
#define SOC_VPP_VHDPDITDIFFVTHD_itdiff_vthd2_END    (23)
#define SOC_VPP_VHDPDITDIFFVTHD_itdiff_vthd3_START  (24)
#define SOC_VPP_VHDPDITDIFFVTHD_itdiff_vthd3_END    (31)


/*****************************************************************************
 结构名    : SOC_VPP_VHDPDLASITHD_UNION
 结构说明  : VHDPDLASITHD 寄存器结构定义。地址偏移量:0x0428，初值:0x00000000，宽度:32
 寄存器说明: Pulldown拉丝统计门限
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  lasi_thd : 8;  /* bit[0-7]  :  */
        unsigned int  edge_thd : 8;  /* bit[8-15] :  */
        unsigned int  reserved : 16; /* bit[16-31]: 保留。 */
    } reg;
} SOC_VPP_VHDPDLASITHD_UNION;
#endif
#define SOC_VPP_VHDPDLASITHD_lasi_thd_START  (0)
#define SOC_VPP_VHDPDLASITHD_lasi_thd_END    (7)
#define SOC_VPP_VHDPDLASITHD_edge_thd_START  (8)
#define SOC_VPP_VHDPDLASITHD_edge_thd_END    (15)


/*****************************************************************************
 结构名    : SOC_VPP_VHDPDFRMITDIFF_UNION
 结构说明  : VHDPDFRMITDIFF 寄存器结构定义。地址偏移量:0x042C，初值:0x00000000，宽度:32
 寄存器说明: Pulldown静止区域统计结果
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  frm_it_diff : 32; /* bit[0-31]:  */
    } reg;
} SOC_VPP_VHDPDFRMITDIFF_UNION;
#endif
#define SOC_VPP_VHDPDFRMITDIFF_frm_it_diff_START  (0)
#define SOC_VPP_VHDPDFRMITDIFF_frm_it_diff_END    (31)


/*****************************************************************************
 结构名    : SOC_VPP_VHDPDSTLBLKSAD_UNION
 结构说明  : VHDPDSTLBLKSAD 寄存器结构定义。地址偏移量:0x0430～0x046C，初值:0x00000000，宽度:32
 寄存器说明: Pulldown静止区域SAD统计结果
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  stlblk_sad : 32; /* bit[0-31]:  */
    } reg;
} SOC_VPP_VHDPDSTLBLKSAD_UNION;
#endif
#define SOC_VPP_VHDPDSTLBLKSAD_stlblk_sad_START  (0)
#define SOC_VPP_VHDPDSTLBLKSAD_stlblk_sad_END    (31)


/*****************************************************************************
 结构名    : SOC_VPP_VHDPDHISTBIN_UNION
 结构说明  : VHDPDHISTBIN 寄存器结构定义。地址偏移量:0x0470～0x47C，初值:0x00000000，宽度:32
 寄存器说明: Pulldown直方图统计结果
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  hsit_bin : 32; /* bit[0-31]:  */
    } reg;
} SOC_VPP_VHDPDHISTBIN_UNION;
#endif
#define SOC_VPP_VHDPDHISTBIN_hsit_bin_START  (0)
#define SOC_VPP_VHDPDHISTBIN_hsit_bin_END    (31)


/*****************************************************************************
 结构名    : SOC_VPP_VHDPDUMMATCH0_UNION
 结构说明  : VHDPDUMMATCH0 寄存器结构定义。地址偏移量:0x0480，初值:0x00000000，宽度:32
 寄存器说明: PullDown
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  match_um : 32; /* bit[0-31]:  */
    } reg;
} SOC_VPP_VHDPDUMMATCH0_UNION;
#endif
#define SOC_VPP_VHDPDUMMATCH0_match_um_START  (0)
#define SOC_VPP_VHDPDUMMATCH0_match_um_END    (31)


/*****************************************************************************
 结构名    : SOC_VPP_VHDPDUMNOMATCH0_UNION
 结构说明  : VHDPDUMNOMATCH0 寄存器结构定义。地址偏移量:0x0484，初值:0x00000000，宽度:32
 寄存器说明: PullDown
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  nomatch_um : 32; /* bit[0-31]:  */
    } reg;
} SOC_VPP_VHDPDUMNOMATCH0_UNION;
#endif
#define SOC_VPP_VHDPDUMNOMATCH0_nomatch_um_START  (0)
#define SOC_VPP_VHDPDUMNOMATCH0_nomatch_um_END    (31)


/*****************************************************************************
 结构名    : SOC_VPP_VHDPDUMMATCH1_UNION
 结构说明  : VHDPDUMMATCH1 寄存器结构定义。地址偏移量:0x0488，初值:0x00000000，宽度:32
 寄存器说明: PullDown
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  match_um : 32; /* bit[0-31]:  */
    } reg;
} SOC_VPP_VHDPDUMMATCH1_UNION;
#endif
#define SOC_VPP_VHDPDUMMATCH1_match_um_START  (0)
#define SOC_VPP_VHDPDUMMATCH1_match_um_END    (31)


/*****************************************************************************
 结构名    : SOC_VPP_VHDPDUMNOMATCH1_UNION
 结构说明  : VHDPDUMNOMATCH1 寄存器结构定义。地址偏移量:0x048C，初值:0x00000000，宽度:32
 寄存器说明: PullDown
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  nomatch_um : 32; /* bit[0-31]:  */
    } reg;
} SOC_VPP_VHDPDUMNOMATCH1_UNION;
#endif
#define SOC_VPP_VHDPDUMNOMATCH1_nomatch_um_START  (0)
#define SOC_VPP_VHDPDUMNOMATCH1_nomatch_um_END    (31)


/*****************************************************************************
 结构名    : SOC_VPP_VHDPDPCCFFWD_UNION
 结构说明  : VHDPDPCCFFWD 寄存器结构定义。地址偏移量:0x0490，初值:0x00000000，宽度:32
 寄存器说明: PullDown
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  pcc_ffwd : 32; /* bit[0-31]:  */
    } reg;
} SOC_VPP_VHDPDPCCFFWD_UNION;
#endif
#define SOC_VPP_VHDPDPCCFFWD_pcc_ffwd_START  (0)
#define SOC_VPP_VHDPDPCCFFWD_pcc_ffwd_END    (31)


/*****************************************************************************
 结构名    : SOC_VPP_VHDPDPCCFWD_UNION
 结构说明  : VHDPDPCCFWD 寄存器结构定义。地址偏移量:0x0494，初值:0x00000000，宽度:32
 寄存器说明: PullDown
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  pcc_fwd : 32; /* bit[0-31]:  */
    } reg;
} SOC_VPP_VHDPDPCCFWD_UNION;
#endif
#define SOC_VPP_VHDPDPCCFWD_pcc_fwd_START  (0)
#define SOC_VPP_VHDPDPCCFWD_pcc_fwd_END    (31)


/*****************************************************************************
 结构名    : SOC_VPP_VHDPDPCCBWD_UNION
 结构说明  : VHDPDPCCBWD 寄存器结构定义。地址偏移量:0x0498，初值:0x00000000，宽度:32
 寄存器说明: PullDown
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  pcc_bwd : 32; /* bit[0-31]:  */
    } reg;
} SOC_VPP_VHDPDPCCBWD_UNION;
#endif
#define SOC_VPP_VHDPDPCCBWD_pcc_bwd_START  (0)
#define SOC_VPP_VHDPDPCCBWD_pcc_bwd_END    (31)


/*****************************************************************************
 结构名    : SOC_VPP_VHDPDPCCCRSS_UNION
 结构说明  : VHDPDPCCCRSS 寄存器结构定义。地址偏移量:0x49C，初值:0x00000000，宽度:32
 寄存器说明: PullDown
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  pcc_crss : 32; /* bit[0-31]:  */
    } reg;
} SOC_VPP_VHDPDPCCCRSS_UNION;
#endif
#define SOC_VPP_VHDPDPCCCRSS_pcc_crss_START  (0)
#define SOC_VPP_VHDPDPCCCRSS_pcc_crss_END    (31)


/*****************************************************************************
 结构名    : SOC_VPP_VHDPDPCCPW_UNION
 结构说明  : VHDPDPCCPW 寄存器结构定义。地址偏移量:0x4A0，初值:0x00000000，宽度:32
 寄存器说明: PullDown
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  pcc_pw : 32; /* bit[0-31]:  */
    } reg;
} SOC_VPP_VHDPDPCCPW_UNION;
#endif
#define SOC_VPP_VHDPDPCCPW_pcc_pw_START  (0)
#define SOC_VPP_VHDPDPCCPW_pcc_pw_END    (31)


/*****************************************************************************
 结构名    : SOC_VPP_VHDPDPCCFWDTKR_UNION
 结构说明  : VHDPDPCCFWDTKR 寄存器结构定义。地址偏移量:0x4A4，初值:0x00000000，宽度:32
 寄存器说明: PullDown
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  pcc_fwd_tkr : 32; /* bit[0-31]:  */
    } reg;
} SOC_VPP_VHDPDPCCFWDTKR_UNION;
#endif
#define SOC_VPP_VHDPDPCCFWDTKR_pcc_fwd_tkr_START  (0)
#define SOC_VPP_VHDPDPCCFWDTKR_pcc_fwd_tkr_END    (31)


/*****************************************************************************
 结构名    : SOC_VPP_VHDPDPCCBWDTKR_UNION
 结构说明  : VHDPDPCCBWDTKR 寄存器结构定义。地址偏移量:0x4A8，初值:0x00000000，宽度:32
 寄存器说明: PullDown
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  pcc_bwd_tkr : 32; /* bit[0-31]:  */
    } reg;
} SOC_VPP_VHDPDPCCBWDTKR_UNION;
#endif
#define SOC_VPP_VHDPDPCCBWDTKR_pcc_bwd_tkr_START  (0)
#define SOC_VPP_VHDPDPCCBWDTKR_pcc_bwd_tkr_END    (31)


/*****************************************************************************
 结构名    : SOC_VPP_VHDPDPCCBLKFWD_UNION
 结构说明  : VHDPDPCCBLKFWD 寄存器结构定义。地址偏移量:0x04AC～0x4CC，初值:0x00000000，宽度:32
 寄存器说明: PullDown
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  pcc_blk_fwd : 32; /* bit[0-31]:  */
    } reg;
} SOC_VPP_VHDPDPCCBLKFWD_UNION;
#endif
#define SOC_VPP_VHDPDPCCBLKFWD_pcc_blk_fwd_START  (0)
#define SOC_VPP_VHDPDPCCBLKFWD_pcc_blk_fwd_END    (31)


/*****************************************************************************
 结构名    : SOC_VPP_VHDPDPCCBLKBWD_UNION
 结构说明  : VHDPDPCCBLKBWD 寄存器结构定义。地址偏移量:0x04D0～0x4F0，初值:0x00000000，宽度:32
 寄存器说明: PullDown
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  pcc_blk_bwd : 32; /* bit[0-31]:  */
    } reg;
} SOC_VPP_VHDPDPCCBLKBWD_UNION;
#endif
#define SOC_VPP_VHDPDPCCBLKBWD_pcc_blk_bwd_START  (0)
#define SOC_VPP_VHDPDPCCBLKBWD_pcc_blk_bwd_END    (31)


/*****************************************************************************
 结构名    : SOC_VPP_VHDPDLASICNT14_UNION
 结构说明  : VHDPDLASICNT14 寄存器结构定义。地址偏移量:0x4F4，初值:0x00000000，宽度:32
 寄存器说明: PullDown
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  lasi_cnt : 32; /* bit[0-31]:  */
    } reg;
} SOC_VPP_VHDPDLASICNT14_UNION;
#endif
#define SOC_VPP_VHDPDLASICNT14_lasi_cnt_START  (0)
#define SOC_VPP_VHDPDLASICNT14_lasi_cnt_END    (31)


/*****************************************************************************
 结构名    : SOC_VPP_VHDPDLASICNT32_UNION
 结构说明  : VHDPDLASICNT32 寄存器结构定义。地址偏移量:0x4F8，初值:0x00000000，宽度:32
 寄存器说明: PullDown
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  lasi_cnt : 32; /* bit[0-31]:  */
    } reg;
} SOC_VPP_VHDPDLASICNT32_UNION;
#endif
#define SOC_VPP_VHDPDLASICNT32_lasi_cnt_START  (0)
#define SOC_VPP_VHDPDLASICNT32_lasi_cnt_END    (31)


/*****************************************************************************
 结构名    : SOC_VPP_VHDPDLASICNT34_UNION
 结构说明  : VHDPDLASICNT34 寄存器结构定义。地址偏移量:0x4FC，初值:0x00000000，宽度:32
 寄存器说明: PullDown
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  lasi_cnt : 32; /* bit[0-31]:  */
    } reg;
} SOC_VPP_VHDPDLASICNT34_UNION;
#endif
#define SOC_VPP_VHDPDLASICNT34_lasi_cnt_START  (0)
#define SOC_VPP_VHDPDLASICNT34_lasi_cnt_END    (31)


/*****************************************************************************
 结构名    : SOC_VPP_VHDHLCOEF_UNION
 结构说明  : VHDHLCOEF 寄存器结构定义。地址偏移量:0x2000～0x210C，初值:0x00000000，宽度:32
 寄存器说明: VHD水平亮度缩放滤波系数寄存器。
            每个滤波器系数为10bit，由符号位和小数位组成，最高位为符号位，低9bit为小数位的绝对值。由于系数存放在片上Memory中，因此默认值为不定态。
            VHD水平亮度缩放滤波为8阶32相位，由于对称关系，共存储17组系数，即0～16相位的8阶系数。每个寄存器地址包含2个10比特系数，因此一个相位的8阶系数需要4个32比特寄存器地址。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  hlcoefn1 : 10; /* bit[0-9]  : 水平亮度缩放滤波系数。
                                                    当寄存器地址[3:0]比特为0x0时，表示第1阶系数；&#13;当寄存器地址[3:0]比特为0x4时，表示第3阶系数；&#13;当寄存器地址[3:0]比特为0x8时，表示第5阶系数；&#13;当寄存器地址[3:0]比特为0xC时，表示第7阶系数。 */
        unsigned int  reserved_0: 6;  /* bit[10-15]: 保留。 */
        unsigned int  hlcoefn2 : 10; /* bit[16-25]: 水平亮度缩放滤波系数。
                                                    当寄存器地址[3:0]比特为0x0时，表示第2阶系数；&#13;当寄存器地址[3:0]比特为0x4时，表示第4阶系数；&#13;当寄存器地址[3:0]比特为0x8时，表示第6阶系数；&#13;当寄存器地址[3:0]比特为0xC时，表示第8阶系数。 */
        unsigned int  reserved_1: 6;  /* bit[26-31]: 保留。 */
    } reg;
} SOC_VPP_VHDHLCOEF_UNION;
#endif
#define SOC_VPP_VHDHLCOEF_hlcoefn1_START  (0)
#define SOC_VPP_VHDHLCOEF_hlcoefn1_END    (9)
#define SOC_VPP_VHDHLCOEF_hlcoefn2_START  (16)
#define SOC_VPP_VHDHLCOEF_hlcoefn2_END    (25)


/*****************************************************************************
 结构名    : SOC_VPP_VHDHCCOEF_UNION
 结构说明  : VHDHCCOEF 寄存器结构定义。地址偏移量:0x2200～0x2284，初值:0x00000000，宽度:32
 寄存器说明: VHD水平色度缩放滤波系数寄存器。存储格式参考VHDHLCOEF。VHD水平色度缩放滤波为4阶32相位，由于对称关系，共存储17组系数，即0～16相位的4阶系数。每个寄存器地址包含2个10比特系数，因此一个相位的4阶系数需要2个32比特寄存器地址。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  hccoefn1 : 10; /* bit[0-9]  : 水平色度缩放滤波系数。
                                                    当寄存器地址[3:0]比特为0x0或者0x8时，表示第1阶系数；&#13;当寄存器地址[3:0]比特为0x4或者0xC时，表示第3阶系数； */
        unsigned int  reserved_0: 6;  /* bit[10-15]: 保留 */
        unsigned int  hccoefn2 : 10; /* bit[16-25]: 水平色度缩放滤波系数。
                                                    当寄存器地址[3:0]比特为0x0或者0x8时，表示第2阶系数；&#13;当寄存器地址[3:0]比特为0x4或者0xC时，表示第4阶系数； */
        unsigned int  reserved_1: 6;  /* bit[26-31]: 保留 */
    } reg;
} SOC_VPP_VHDHCCOEF_UNION;
#endif
#define SOC_VPP_VHDHCCOEF_hccoefn1_START  (0)
#define SOC_VPP_VHDHCCOEF_hccoefn1_END    (9)
#define SOC_VPP_VHDHCCOEF_hccoefn2_START  (16)
#define SOC_VPP_VHDHCCOEF_hccoefn2_END    (25)


/*****************************************************************************
 结构名    : SOC_VPP_VHDVLCOEF_UNION
 结构说明  : VHDVLCOEF 寄存器结构定义。地址偏移量:0x2400～0x250C，初值:0x00000000，宽度:32
 寄存器说明: VHD垂直亮度缩放滤波系数寄存器。存储格式参考VHDHLCOEF。VHD垂直亮度缩放滤波为6阶32相位，由于对称关系，共存储17组系数，即0～16相位的4阶系数。每个寄存器地址包含2个10比特系数，一个相位的6阶系数需要4个32比特寄存器地址。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  vlcoefn1 : 10; /* bit[0-9]  : 垂直亮度缩放滤波系数。
                                                    当寄存器地址[3:0]比特为0x0时，表示第1阶系数；&#13;当寄存器地址[3:0]比特为0x4时，表示第3阶系数；&#13;当寄存器地址[3:0]比特为0x8时，表示第5阶系数；&#13;当寄存器地址[3:0]比特为0xC时，为保留位。 */
        unsigned int  reserved_0: 6;  /* bit[10-15]: 保留。 */
        unsigned int  vlcoefn2 : 10; /* bit[16-25]: 垂直亮度缩放滤波系数。
                                                    当寄存器地址[3:0]比特为0x0时，表示第2阶系数；&#13;当寄存器地址[3:0]比特为0x4时，表示第4阶系数；&#13;当寄存器地址[3:0]比特为0x8时，表示第6阶系数；&#13;当寄存器地址[3:0]比特为0xC时，为保留位。 */
        unsigned int  reserved_1: 6;  /* bit[26-31]: 保留。 */
    } reg;
} SOC_VPP_VHDVLCOEF_UNION;
#endif
#define SOC_VPP_VHDVLCOEF_vlcoefn1_START  (0)
#define SOC_VPP_VHDVLCOEF_vlcoefn1_END    (9)
#define SOC_VPP_VHDVLCOEF_vlcoefn2_START  (16)
#define SOC_VPP_VHDVLCOEF_vlcoefn2_END    (25)


/*****************************************************************************
 结构名    : SOC_VPP_VHDVCCOEF_UNION
 结构说明  : VHDVCCOEF 寄存器结构定义。地址偏移量:0x2600～0x2684，初值:0x00000000，宽度:32
 寄存器说明: VHD垂直色度缩放滤波系数寄存器。存储格式参考VHDHLCOEF。VHD垂直色度缩放滤波为4阶32相位，由于对称关系，共存储17组系数，即0～16相位的4阶系数。每个寄存器地址包含2个10比特系数，因此一个相位的4阶系数需要2个32比特寄存器地址。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  vccoefn1 : 10; /* bit[0-9]  : 垂直亮度缩放滤波系数。
                                                    当寄存器地址[3:0]比特为0x0或者0x8时，表示第1阶系数；&#13;当寄存器地址[3:0]比特为0x4或者0xC时，表示第3阶系数。 */
        unsigned int  reserved_0: 6;  /* bit[10-15]: 保留。 */
        unsigned int  vccoefn2 : 10; /* bit[16-25]: 垂直亮度缩放滤波系数。
                                                    当寄存器地址[3:0]比特为0x0或者0x8时，表示第2阶系数；&#13;当寄存器地址[3:0]比特为0x4或者0xC时，表示第4阶系数。 */
        unsigned int  reserved_1: 6;  /* bit[26-31]: 保留。 */
    } reg;
} SOC_VPP_VHDVCCOEF_UNION;
#endif
#define SOC_VPP_VHDVCCOEF_vccoefn1_START  (0)
#define SOC_VPP_VHDVCCOEF_vccoefn1_END    (9)
#define SOC_VPP_VHDVCCOEF_vccoefn2_START  (16)
#define SOC_VPP_VHDVCCOEF_vccoefn2_END    (25)






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

#endif /* end of soc_vpp_interface.h */
