/******************************************************************************

                 版权所有 (C), 2001-2014, 华为技术有限公司

 ******************************************************************************
  文 件 名   : bbp_comm_interface.h
  版 本 号   : 初稿
  作    者   : Excel2Code
  生成日期   : 2014-01-28 10:41:24
  最近修改   :
  功能描述   : 接口头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2014年1月28日
    作    者   : w56557
    修改内容   : 从《副本Hi6210V100 MODEM寄存器手册_COMM.xml》自动生成

******************************************************************************/

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "soc_baseaddr_interface.h"

#ifndef __BBP_COMM_INTERFACE_H__
#define __BBP_COMM_INTERFACE_H__

#ifdef __cplusplus
    #if __cplusplus
        extern "C" {
    #endif
#endif



/*****************************************************************************
  2 宏定义
*****************************************************************************/

/***======================================================================***
                     (1/5) register_define_ctu
 ***======================================================================***/
/* 寄存器说明：通道0对应主模（主模0）的模式指示寄存器
 bit[31:3]   保留
 bit[2:0]    通道0对应的主模（定义为主模0）的模式指示，表示LTE，W，Gm，Gs，TDS谁是主模0。
             000：LTE 主模。
             001：W主模。
             010：TDS主模。
             011：Gm主模；
             100：Gs主模；
             101:C主模；
             110~111：无效。
             配置时刻：各模根据通信场景配置
   UNION结构:  BBPCOMM_C0_PRIMARY_MODE_IND_UNION */
#define BBPCOMM_C0_PRIMARY_MODE_IND_ADDR              (SOC_BBP_COMM_BASE_ADDR + 0x0)

/* 寄存器说明：主模0对应的从模（从模0）的模式指示寄存器
 bit[31:3]   保留
 bit[2:0]    主模0对应的从模（定义为从模0）的模式指示，表示当前LTE/W/Gm/Gs/TDS谁是从模0。
             000：LTE从模。
             001：W 从模。
             010：TDS从模。
             011：Gm从模；
             100：Gs从模；
             101:C从模；
             110~111：无效。
             配置时刻：各模根据通信场景配置
             注：约定主模0固定走通道0，主模1固定走通道1。
   UNION结构:  BBPCOMM_C0_SECOND_MODE_IND_UNION */
#define BBPCOMM_C0_SECOND_MODE_IND_ADDR               (SOC_BBP_COMM_BASE_ADDR + 0x4)

/* 寄存器说明：寄存器
   详      述：AFC0值传递
            [15:0]:AFC积分器值
            [16]:AFC锁定指示
            
            配置时刻：DSP从主模切换为从模时配置；DSP从从模切换为主模时读取并清除锁定指示
            生效时刻：立即生效
   UNION结构 ：无 */
#define BBPCOMM_AFC0_VALUE_INTER_RAT_ADDR             (SOC_BBP_COMM_BASE_ADDR + 0xC)

/* 寄存器说明：寄存器
 bit[31:3]   保留
 bit[2:0]    通道0 AFC控制信号来源。
             3'b000，受主模控制；
             3'b001，强制受L模控制；
             3'b010，强制受W模控制；
             3'b011，强制受TDS模控制；
             3'b100，强制受Gm模控制；
             3'b101，强制受Gs模控制；
             3'b110，强制受C模控制；
             3'b111，无效。
             调试用。
   UNION结构:  BBPCOMM_C0_AFC_PDM_SELECT_MASK_UNION */
#define BBPCOMM_C0_AFC_PDM_SELECT_MASK_ADDR           (SOC_BBP_COMM_BASE_ADDR + 0x10)

/* 寄存器说明：寄存器
 bit[31:3]   保留
 bit[2:0]    通道0 天线开关和PA控制信号来源。
             3'b000，受主模控制；
             3'b001，强制受L模控制；
             3'b010，强制受W模控制；
             3'b011，强制受TDS模控制；
             3'b100，强制受Gm模控制；
             3'b101，强制受Gs模控制；
             3'b110，强制受C模控制；
             3'b111，无效。
             调试用。
   UNION结构:  BBPCOMM_C0_ANT_SWITCH_PA_SELECT_MASK_UNION */
#define BBPCOMM_C0_ANT_SWITCH_PA_SELECT_MASK_ADDR     (SOC_BBP_COMM_BASE_ADDR + 0x14)

/* 寄存器说明：寄存器
 bit[31:3]   保留
 bit[2:0]    通道0 RFIC控制信号来源（包括线控和SSI）。
             3'b000，受主模控制；
             3'b001，强制受L模控制；
             3'b010，强制受W模控制；
             3'b011，强制受TDS模控制；
             3'b100，强制受Gm模控制；
             3'b101，强制受Gs模控制；
             3'b110，强制受C模控制；
             3'b111，无效。
             调试用。
   UNION结构:  BBPCOMM_C0_RFIC_SELECT_MASK_UNION */
#define BBPCOMM_C0_RFIC_SELECT_MASK_ADDR              (SOC_BBP_COMM_BASE_ADDR + 0x18)

/* 寄存器说明：寄存器
 bit[31:3]   保留
 bit[2:0]    通道0 MIPI控制信号来源。
             3'b000，受主模控制；
             3'b001，强制受L模控制；
             3'b010，强制受W模控制；
             3'b011，强制受TDS模控制；
             3'b100，强制受Gm模控制；
             3'b101，强制受Gs模控制；
             3'b110，强制受C模控制；
             其他，无效。
             调试用。
   UNION结构:  BBPCOMM_C0_MIPI_SELECT_MASK_UNION */
#define BBPCOMM_C0_MIPI_SELECT_MASK_ADDR              (SOC_BBP_COMM_BASE_ADDR + 0x1C)

/* 寄存器说明：寄存器
 bit[31:3]   保留
 bit[2:0]    通道0 ABB控制信号来源。
             3'b000，受主模控制；
             3'b001，强制受L模控制；
             3'b010，强制受W模控制；
             3'b011，强制受TDS模控制；
             3'b100，强制受Gm模控制；
             3'b101，强制受Gs模控制；
             3'b110，强制受C模控制；
             其他，无效。
             调试用。
   UNION结构:  BBPCOMM_C0_ABB_SELECT_MASK_UNION */
#define BBPCOMM_C0_ABB_SELECT_MASK_ADDR               (SOC_BBP_COMM_BASE_ADDR + 0x20)

/* 寄存器说明：寄存器
 bit[31:3]   保留
 bit[2:0]    通道0 PMU控制信号来源。
             3'b000，受主模控制；
             3'b001，强制受L模控制；
             3'b010，强制受W模控制；
             3'b011，强制受TDS模控制；
             3'b100，强制受Gm模控制；
             3'b101，强制受Gs模控制；
             3'b110，强制受C模控制；
             3'b111，无效。
             调试用。
   UNION结构:  BBPCOMM_C0_PMU_SELECT_MASK_UNION */
#define BBPCOMM_C0_PMU_SELECT_MASK_ADDR               (SOC_BBP_COMM_BASE_ADDR + 0x24)

/* 寄存器说明：寄存器
 bit[31:3]   保留
 bit[2:0]    当前通道0（交叉之前）输出的强配模式。
             注：其适用于各个外围器件（ABB、RF、PA、mipi等）公共的交叉强配。
             调试用。
   UNION结构:  BBPCOMM_DSP_C0_SEL_MASK_UNION */
#define BBPCOMM_DSP_C0_SEL_MASK_ADDR                  (SOC_BBP_COMM_BASE_ADDR + 0x28)

/* 寄存器说明：寄存器
 bit[31:1]   保留
 bit[0:0]    软件配置的RFIC0复位信号，低有效。
             0：复位，1：解复位。
             上电初始化配置为0，根据RFIC手册要求，维持一段时间后配置为1。
             V3R3，该寄存器功能改在寄存器0x0334的bit[8]实现，默认值改为0，此处删除。
             V7R2复位值为0。
             V8R1：该寄存器功能删除。
   UNION结构:  BBPCOMM_DSP_RFIC0_RSTN_UNION */
#define BBPCOMM_DSP_RFIC0_RSTN_ADDR                   (SOC_BBP_COMM_BASE_ADDR + 0x2C)

/* 寄存器说明：寄存器
 bit[31:1]   保留
 bit[0]      0:通道0从模使用通道0主模afc控制字
             1:通道0从模使用通道0从模afc控制字
   UNION结构:  BBPCOMM_C0_SECOND_AFC_MODE_UNION */
#define BBPCOMM_C0_SECOND_AFC_MODE_ADDR               (SOC_BBP_COMM_BASE_ADDR + 0x30)

/* 寄存器说明：寄存器
   详      述：备份bbp_sys_0control。
            GU不涉及。
   UNION结构 ：无 */
#define BBPCOMM_BBP_SYS_0CONTROL_ADDR                 (SOC_BBP_COMM_BASE_ADDR + 0x34)

/* 寄存器说明：寄存器
 bit[31:1]   保留
 bit[0]      DSP等待回读结果超时标志位
             1:超时  
   UNION结构:  BBPCOMM_MIPI0_RD_OVERTIME_FLAG_DSP_UNION */
#define BBPCOMM_MIPI0_RD_OVERTIME_FLAG_DSP_ADDR       (SOC_BBP_COMM_BASE_ADDR + 0x38)

/* 寄存器说明：寄存器
 bit[31:1]   保留
 bit[0]      DSP等待回读结果超时标志位
             1:超时  
   UNION结构:  BBPCOMM_SSI0_RD_OVERTIME_FLAG_DSP_UNION */
#define BBPCOMM_SSI0_RD_OVERTIME_FLAG_DSP_ADDR        (SOC_BBP_COMM_BASE_ADDR + 0x3C)

/* 寄存器说明：寄存器
 bit[31:1]   保留
 bit[0]      DSP等待回读结果超时后,清抄时标志位脉冲
   UNION结构:  BBPCOMM_MIPI0_RD_OVERTIME_CLR_UNION */
#define BBPCOMM_MIPI0_RD_OVERTIME_CLR_ADDR            (SOC_BBP_COMM_BASE_ADDR + 0x40)

/* 寄存器说明：寄存器
 bit[31:1]   保留
 bit[0]      DSP等待回读结果超时后,清抄时标志位脉冲
   UNION结构:  BBPCOMM_SSI0_RD_OVERTIME_CLR_UNION */
#define BBPCOMM_SSI0_RD_OVERTIME_CLR_ADDR             (SOC_BBP_COMM_BASE_ADDR + 0x44)

/* 寄存器说明：寄存器
   详      述：xbbp向SSI0 master发出回读类指令时,DSP读取SSI回读结果,回读结果放置在此16寄存器位置由dsp配置,回读数据是否有效参考标志位
   UNION结构 ：无 */
#define BBPCOMM_RD_RESULT_RFIC0_1_ADDR                (SOC_BBP_COMM_BASE_ADDR + 0x48)

/* 寄存器说明：寄存器
   详      述：同上
   UNION结构 ：无 */
#define BBPCOMM_RD_RESULT_RFIC0_2_ADDR                (SOC_BBP_COMM_BASE_ADDR + 0x4C)

/* 寄存器说明：寄存器
   详      述：同上
   UNION结构 ：无 */
#define BBPCOMM_RD_RESULT_RFIC0_3_ADDR                (SOC_BBP_COMM_BASE_ADDR + 0x50)

/* 寄存器说明：寄存器
   详      述：同上
   UNION结构 ：无 */
#define BBPCOMM_RD_RESULT_RFIC0_4_ADDR                (SOC_BBP_COMM_BASE_ADDR + 0x54)

/* 寄存器说明：寄存器
   详      述：同上
   UNION结构 ：无 */
#define BBPCOMM_RD_RESULT_RFIC0_5_ADDR                (SOC_BBP_COMM_BASE_ADDR + 0x58)

/* 寄存器说明：寄存器
   详      述：同上
   UNION结构 ：无 */
#define BBPCOMM_RD_RESULT_RFIC0_6_ADDR                (SOC_BBP_COMM_BASE_ADDR + 0x5C)

/* 寄存器说明：寄存器
   详      述：同上
   UNION结构 ：无 */
#define BBPCOMM_RD_RESULT_RFIC0_7_ADDR                (SOC_BBP_COMM_BASE_ADDR + 0x60)

/* 寄存器说明：寄存器
   详      述：同上
   UNION结构 ：无 */
#define BBPCOMM_RD_RESULT_RFIC0_8_ADDR                (SOC_BBP_COMM_BASE_ADDR + 0x64)

/* 寄存器说明：寄存器
   详      述：同上
   UNION结构 ：无 */
#define BBPCOMM_RD_RESULT_RFIC0_9_ADDR                (SOC_BBP_COMM_BASE_ADDR + 0x68)

/* 寄存器说明：寄存器
   详      述：同上
   UNION结构 ：无 */
#define BBPCOMM_RD_RESULT_RFIC0_10_ADDR               (SOC_BBP_COMM_BASE_ADDR + 0x6C)

/* 寄存器说明：寄存器
   详      述：同上
   UNION结构 ：无 */
#define BBPCOMM_RD_RESULT_RFIC0_11_ADDR               (SOC_BBP_COMM_BASE_ADDR + 0x70)

/* 寄存器说明：寄存器
   详      述：同上
   UNION结构 ：无 */
#define BBPCOMM_RD_RESULT_RFIC0_12_ADDR               (SOC_BBP_COMM_BASE_ADDR + 0x74)

/* 寄存器说明：寄存器
   详      述：同上
   UNION结构 ：无 */
#define BBPCOMM_RD_RESULT_RFIC0_13_ADDR               (SOC_BBP_COMM_BASE_ADDR + 0x78)

/* 寄存器说明：寄存器
   详      述：同上
   UNION结构 ：无 */
#define BBPCOMM_RD_RESULT_RFIC0_14_ADDR               (SOC_BBP_COMM_BASE_ADDR + 0x7C)

/* 寄存器说明：寄存器
   详      述：同上
   UNION结构 ：无 */
#define BBPCOMM_RD_RESULT_RFIC0_15_ADDR               (SOC_BBP_COMM_BASE_ADDR + 0x80)

/* 寄存器说明：寄存器
   详      述：同上
   UNION结构 ：无 */
#define BBPCOMM_RD_RESULT_RFIC0_16_ADDR               (SOC_BBP_COMM_BASE_ADDR + 0x84)

/* 寄存器说明：寄存器
 bit[31:1]   保留
 bit[0]      所有回读指令执行完毕,可以取走回读数据
             1:所有回读指令发送完毕,DSP可以按位置读取SSI回读结果
   UNION结构:  BBPCOMM_RD_RESULT_FLAG_RFIC0_UNION */
#define BBPCOMM_RD_RESULT_FLAG_RFIC0_ADDR             (SOC_BBP_COMM_BASE_ADDR + 0x88)

/* 寄存器说明：寄存器
 bit[31:1]   保留
 bit[0]      1:DSP取走所有回读数据后,写入clr信号清标志位
   UNION结构:  BBPCOMM_RD_RESULT_FLAG_RFIC0_CLR_UNION */
#define BBPCOMM_RD_RESULT_FLAG_RFIC0_CLR_ADDR         (SOC_BBP_COMM_BASE_ADDR + 0x8C)

/* 寄存器说明：寄存器
   详      述：xbbp向MIPI0 master发出回读类指令时,DSP读取SSI回读结果,回读结果放置在此16寄存器位置由dsp配置,回读数据是否有效参考标志位
   UNION结构 ：无 */
#define BBPCOMM_RD_RESULT_MIPI0_1_ADDR                (SOC_BBP_COMM_BASE_ADDR + 0x90)

/* 寄存器说明：寄存器
   详      述：同上
   UNION结构 ：无 */
#define BBPCOMM_RD_RESULT_MIPI0_2_ADDR                (SOC_BBP_COMM_BASE_ADDR + 0x94)

/* 寄存器说明：寄存器
   详      述：同上
   UNION结构 ：无 */
#define BBPCOMM_RD_RESULT_MIPI0_3_ADDR                (SOC_BBP_COMM_BASE_ADDR + 0x98)

/* 寄存器说明：寄存器
   详      述：同上
   UNION结构 ：无 */
#define BBPCOMM_RD_RESULT_MIPI0_4_ADDR                (SOC_BBP_COMM_BASE_ADDR + 0x9C)

/* 寄存器说明：寄存器
   详      述：同上
   UNION结构 ：无 */
#define BBPCOMM_RD_RESULT_MIPI0_5_ADDR                (SOC_BBP_COMM_BASE_ADDR + 0xA0)

/* 寄存器说明：寄存器
   详      述：同上
   UNION结构 ：无 */
#define BBPCOMM_RD_RESULT_MIPI0_6_ADDR                (SOC_BBP_COMM_BASE_ADDR + 0xA4)

/* 寄存器说明：寄存器
   详      述：同上
   UNION结构 ：无 */
#define BBPCOMM_RD_RESULT_MIPI0_7_ADDR                (SOC_BBP_COMM_BASE_ADDR + 0xA8)

/* 寄存器说明：寄存器
   详      述：同上
   UNION结构 ：无 */
#define BBPCOMM_RD_RESULT_MIPI0_8_ADDR                (SOC_BBP_COMM_BASE_ADDR + 0xAC)

/* 寄存器说明：寄存器
   详      述：同上
   UNION结构 ：无 */
#define BBPCOMM_RD_RESULT_MIPI0_9_ADDR                (SOC_BBP_COMM_BASE_ADDR + 0xB0)

/* 寄存器说明：寄存器
   详      述：同上
   UNION结构 ：无 */
#define BBPCOMM_RD_RESULT_MIPI0_10_ADDR               (SOC_BBP_COMM_BASE_ADDR + 0xB4)

/* 寄存器说明：寄存器
   详      述：同上
   UNION结构 ：无 */
#define BBPCOMM_RD_RESULT_MIPI0_11_ADDR               (SOC_BBP_COMM_BASE_ADDR + 0xB8)

/* 寄存器说明：寄存器
   详      述：同上
   UNION结构 ：无 */
#define BBPCOMM_RD_RESULT_MIPI0_12_ADDR               (SOC_BBP_COMM_BASE_ADDR + 0xBC)

/* 寄存器说明：寄存器
   详      述：同上
   UNION结构 ：无 */
#define BBPCOMM_RD_RESULT_MIPI0_13_ADDR               (SOC_BBP_COMM_BASE_ADDR + 0xC0)

/* 寄存器说明：寄存器
   详      述：同上
   UNION结构 ：无 */
#define BBPCOMM_RD_RESULT_MIPI0_14_ADDR               (SOC_BBP_COMM_BASE_ADDR + 0xC4)

/* 寄存器说明：寄存器
   详      述：同上
   UNION结构 ：无 */
#define BBPCOMM_RD_RESULT_MIPI0_15_ADDR               (SOC_BBP_COMM_BASE_ADDR + 0xC8)

/* 寄存器说明：寄存器
   详      述：同上
   UNION结构 ：无 */
#define BBPCOMM_RD_RESULT_MIPI0_16_ADDR               (SOC_BBP_COMM_BASE_ADDR + 0xCC)

/* 寄存器说明：寄存器
 bit[31:1]   保留
 bit[0]      所有回读指令执行完毕,可以取走回读数据
             1:所有回读指令发送完毕,DSP可以按位置读取SSI回读结果,与下述的软件直接控制MIPI标志位不同
   UNION结构:  BBPCOMM_RD_RESULT_FLAG_MIPI0_UNION */
#define BBPCOMM_RD_RESULT_FLAG_MIPI0_ADDR             (SOC_BBP_COMM_BASE_ADDR + 0xD0)

/* 寄存器说明：寄存器
 bit[31:1]   保留
 bit[0]      1:DSP取走所有回读数据后,写入clr信号清标志位,与下述的软件直接控制MIPI标志位不同
   UNION结构:  BBPCOMM_RD_RESULT_FLAG_MIPI0_CLR_UNION */
#define BBPCOMM_RD_RESULT_FLAG_MIPI0_CLR_ADDR         (SOC_BBP_COMM_BASE_ADDR + 0xD4)

/* 寄存器说明：寄存器
   详      述：由软件直接控制MIPI0 master,不借助xbbp发送回读指令时,回读指令存放低32bit.
   UNION结构 ：无 */
#define BBPCOMM_MIPI0_RD_DATA_LOW_SOFT_ADDR           (SOC_BBP_COMM_BASE_ADDR + 0xD8)

/* 寄存器说明：寄存器
   详      述：由软件直接控制MIPI0 master,不借助xbbp发送回读指令时,回读指令存放高32bit.
   UNION结构 ：无 */
#define BBPCOMM_MIPI0_RD_DATA_HIGH_SOFT_ADDR          (SOC_BBP_COMM_BASE_ADDR + 0xDC)

/* 寄存器说明：寄存器
 bit[31:1]   保留
 bit[0]      由软件直接控制MIPI0 master,不借助xbbp发送回读指令时,回读数据有效指示
   UNION结构:  BBPCOMM_RD_END_FLAG_MIPI0_SOFT_UNION */
#define BBPCOMM_RD_END_FLAG_MIPI0_SOFT_ADDR           (SOC_BBP_COMM_BASE_ADDR + 0xE0)

/* 寄存器说明：寄存器
 bit[31:1]   保留
 bit[0]      由软件直接控制MIPI0 master
             1:软件可以发送CFG_IND启动mipi0
             0:软件等待mipi0授权使用
   UNION结构:  BBPCOMM_MIPI0_GRANT_DSP_UNION */
#define BBPCOMM_MIPI0_GRANT_DSP_ADDR                  (SOC_BBP_COMM_BASE_ADDR + 0xE4)

/* 寄存器说明：寄存器
   详      述：由软件直接控制MIPI0 master,不借助xbbp发送写指令时,写指令低32bit。
            在MIPI动态使能mipi_dyn_exchg_en有效时，MIPI扩展指令时，dsp_mipi0_wdata_low[0]应配置为0
   UNION结构 ：无 */
#define BBPCOMM_DSP_MIPI0_WDATA_LOW_ADDR              (SOC_BBP_COMM_BASE_ADDR + 0xE8)

/* 寄存器说明：寄存器
   详      述：由软件直接控制MIPI0 master,不借助xbbp发送写指令时,写指令高32bit。
            在MIPI动态使能mipi_dyn_exchg_en有效，MIPI非扩展指令时，dsp_mipi0_wdata_high[0]应配置为0
   UNION结构 ：无 */
#define BBPCOMM_DSP_MIPI0_WDATA_HIGH_ADDR             (SOC_BBP_COMM_BASE_ADDR + 0xEC)

/* 寄存器说明：寄存器
 bit[31:1]   保留
 bit[0]      由软件直接控制MIPI0 master,需首先拉高此指示,屏蔽xbbp对MIPI0的控制
   UNION结构:  BBPCOMM_DSP_MIPI0_EN_IMI_UNION */
#define BBPCOMM_DSP_MIPI0_EN_IMI_ADDR                 (SOC_BBP_COMM_BASE_ADDR + 0xF0)

/* 寄存器说明：寄存器
 bit[31:1]   保留
 bit[0]      由软件直接控制MIPI0 master,拉高此dsp_mipi0_en后,产生软件启动脉冲
   UNION结构:  BBPCOMM_DSP_MIPI0_CFG_IND_IMI_UNION */
#define BBPCOMM_DSP_MIPI0_CFG_IND_IMI_ADDR            (SOC_BBP_COMM_BASE_ADDR + 0xF4)

/* 寄存器说明：寄存器
 bit[31:1]   保留
 bit[0]      由软件直接控制MIPI0 master,取走回读数据后,清标志位脉冲
   UNION结构:  BBPCOMM_DSP_MIPI0_RD_CLR_UNION */
#define BBPCOMM_DSP_MIPI0_RD_CLR_ADDR                 (SOC_BBP_COMM_BASE_ADDR + 0xF8)

/* 寄存器说明：寄存器
 bit[31:9]   保留
 bit[8:8]    软件强配模式abb0_rxb_blk_en.
 bit[7:7]    软件强配模式abb0_rxa_blk_en.
 bit[6:6]    软件强配模式abb0_rxb_en.
 bit[5:5]    软件强配模式abb0_rxa_en.
 bit[4:4]    软件强配模式abb0_tx_en.
 bit[3:1]    软件强配模式abb0_mode_sel.
 bit[0:0]    ABB0线控强配模式：
             1：表示软件强配，此时上述各个强配才生效；
             0：表示由CTU根据各BBP产生。
             默认值为0。
             测试用。
   UNION结构:  BBPCOMM_ABB0_LINE_CONTROL_CMD_UNION */
#define BBPCOMM_ABB0_LINE_CONTROL_CMD_ADDR            (SOC_BBP_COMM_BASE_ADDR + 0xFC)

/* 寄存器说明：
 bit[31:3]   保留
 bit[2:0]    V9R1版本，软件不配置。
   UNION结构:  BBPCOMM_CH0_BBP_SEL_UNION */
#define BBPCOMM_CH0_BBP_SEL_ADDR                      (SOC_BBP_COMM_BASE_ADDR + 0x100)

/* 寄存器说明：
 bit[31:1]   保留
 bit[0]      cpu配置mipi功能,配置为1才能正常工作, 测试时配置为0
   UNION结构:  BBPCOMM_CPU_MIPI0_FUNC_SEL_UNION */
#define BBPCOMM_CPU_MIPI0_FUNC_SEL_ADDR               (SOC_BBP_COMM_BASE_ADDR + 0x104)

/* 寄存器说明：
 bit[31:1]   保留
 bit[0]      cpu配置mipi功能,测试用
   UNION结构:  BBPCOMM_CPU_MIPI0_TEST_FUNC_UNION */
#define BBPCOMM_CPU_MIPI0_TEST_FUNC_ADDR              (SOC_BBP_COMM_BASE_ADDR + 0x108)

/* 寄存器说明：
 bit[31:1]   保留
 bit[0]      cpu配置mipi功能, 测试用
   UNION结构:  BBPCOMM_CPU_MIPI0_SCLK_TEST_UNION */
#define BBPCOMM_CPU_MIPI0_SCLK_TEST_ADDR              (SOC_BBP_COMM_BASE_ADDR + 0x10C)

/* 寄存器说明：
 bit[31:1]   保留
 bit[0]      cpu配置mipi功能, 测试用
   UNION结构:  BBPCOMM_CPU_MIPI0_SDATA_TEST_UNION */
#define BBPCOMM_CPU_MIPI0_SDATA_TEST_ADDR             (SOC_BBP_COMM_BASE_ADDR + 0x110)

/* 寄存器说明：
 bit[31:1]   保留
 bit[0]      cpu配置mipi功能,mipi0master清异常信号
   UNION结构:  BBPCOMM_DSP_MIPI0_CLR_IND_IMI_UNION */
#define BBPCOMM_DSP_MIPI0_CLR_IND_IMI_ADDR            (SOC_BBP_COMM_BASE_ADDR + 0x114)

/* 寄存器说明：
 bit[31:1]   保留
 bit[0]      mipi0存储待发送数据同步FIFO复位信号
   UNION结构:  BBPCOMM_CPU_MIPI0_FIFO_CLR_IMI_UNION */
#define BBPCOMM_CPU_MIPI0_FIFO_CLR_IMI_ADDR           (SOC_BBP_COMM_BASE_ADDR + 0x118)

/* 寄存器说明：
 bit[31:1]   保留
 bit[0]      ssi0存储待发送数据同步FIFO复位信号
   UNION结构:  BBPCOMM_CPU_SSI0_FIFO_CLR_IMI_UNION */
#define BBPCOMM_CPU_SSI0_FIFO_CLR_IMI_ADDR            (SOC_BBP_COMM_BASE_ADDR + 0x11C)

/* 寄存器说明：
 bit[31:12]  保留
 bit[11]     C模通道0 TX ABB随路时钟门控旁路信号
 bit[10]     C模通道0 RX ABB随路时钟门控旁路信号
 bit[9]      G2通道0 TX ABB随路时钟门控旁路信号
 bit[8]      G1通道0 TX ABB随路时钟门控旁路信号
 bit[7]      W模通道0 TX ABB随路时钟门控旁路信号
 bit[6]      TDS通道0 TX ABB随路时钟门控旁路信号
 bit[5]      L模通道0 TX ABB随路时钟门控旁路信号
 bit[4]      G2通道0 RX ABB随路时钟门控旁路信号
 bit[3]      G1通道0 RX ABB随路时钟门控旁路信号
 bit[2]      W模通道0 RX ABB随路时钟门控旁路信号
 bit[1]      TDS通道0 RX ABB随路时钟门控旁路信号
 bit[0]      L模通道0 RX ABB随路时钟门控旁路信号
   UNION结构:  BBPCOMM_CPU_RX0_TX0_CKG_BYPASS_UNION */
#define BBPCOMM_CPU_RX0_TX0_CKG_BYPASS_ADDR           (SOC_BBP_COMM_BASE_ADDR + 0x120)

/* 寄存器说明：通道1对应主模（主模1）的模式指示寄存器
 bit[31:3]   保留
 bit[2:0]    通道1对应的主模（定义为主模1）的模式指示，表示LTE，W，Gm，Gs，TDS谁是主模1。
             000：LTE 主模。
             001：W主模。
             010：TDS主模。
             011：Gm主模；
             100：Gs主模；
             101：C主模；
             110~111：无效。
             配置时刻：各模根据通信场景配置
   UNION结构:  BBPCOMM_C1_PRIMARY_MODE_IND_UNION */
#define BBPCOMM_C1_PRIMARY_MODE_IND_ADDR              (SOC_BBP_COMM_BASE_ADDR + 0x300)

/* 寄存器说明：主模1对应的从模（从模1）的模式指示寄存器
 bit[31:3]   保留
 bit[2:0]    主模1对应的从模（定义为从模1）的模式指示，表示LTE/W/Gm/Gs/TDS谁是从模1。
             000：LTE从模。
             001：W 从模。
             010：TDS从模。
             011：Gm从模；
             100：Gs从模；
             101：C从模；
             110~111：无效。
             配置时刻：各模根据通信场景配置.
             注：约定主模0固定走通道0，主模1固定走通道1。
   UNION结构:  BBPCOMM_C1_SECOND_MODE_IND_UNION */
#define BBPCOMM_C1_SECOND_MODE_IND_ADDR               (SOC_BBP_COMM_BASE_ADDR + 0x304)

/* 寄存器说明：寄存器
   详      述：AFC1值传递
            [15:0]:AFC积分器值
            [16]:AFC锁定指示
            
            配置时刻：DSP从主模切换为从模时配置；DSP从从模切换为主模时读取并清除锁定指示
            生效时刻：立即生效
   UNION结构 ：无 */
#define BBPCOMM_AFC1_VALUE_INTER_RAT_ADDR             (SOC_BBP_COMM_BASE_ADDR + 0x30C)

/* 寄存器说明：寄存器
 bit[31:3]   保留
 bit[2:0]    通道1 AFC控制信号来源。
             3'b000，受主模控制；
             3'b001，强制受L模控制；
             3'b010，强制受W模控制；
             3'b011，强制受TDS模控制；
             3'b100，强制受Gm模控制；
             3'b101，强制受Gs模控制；
             3'b110，强制受C模控制；
             3'b111，无效。
             调试用。
   UNION结构:  BBPCOMM_C1_AFC_PDM_SELECT_MASK_UNION */
#define BBPCOMM_C1_AFC_PDM_SELECT_MASK_ADDR           (SOC_BBP_COMM_BASE_ADDR + 0x310)

/* 寄存器说明：寄存器
 bit[31:3]   保留
 bit[2:0]    通道1 天线开关和PA控制信号来源。
             3'b000，受主模控制；
             3'b001，强制受L模控制；
             3'b010，强制受W模控制；
             3'b011，强制受TDS模控制；
             3'b100，强制受Gm模控制；
             3'b101，强制受Gs模控制；
             3'b110，强制受C模控制；
             3'b111，无效。
             调试用。
   UNION结构:  BBPCOMM_C1_ANT_SWITCH_PA_SELECT_MASK_UNION */
#define BBPCOMM_C1_ANT_SWITCH_PA_SELECT_MASK_ADDR     (SOC_BBP_COMM_BASE_ADDR + 0x314)

/* 寄存器说明：寄存器
 bit[31:3]   保留
 bit[2:0]    通道1 RFIC控制信号来源（包括线控和SSI）。
             3'b000，受主模控制；
             3'b001，强制受L模控制；
             3'b010，强制受W模控制；
             3'b011，强制受TDS模控制；
             3'b100，强制受Gm模控制；
             3'b101，强制受Gs模控制；
             3'b110，强制受C模控制；
             3'b111，无效。
             调试用。
   UNION结构:  BBPCOMM_C1_RFIC_SELECT_MASK_UNION */
#define BBPCOMM_C1_RFIC_SELECT_MASK_ADDR              (SOC_BBP_COMM_BASE_ADDR + 0x318)

/* 寄存器说明：寄存器
 bit[31:3]   保留
 bit[2:0]    通道1 MIPI控制信号来源。
             3'b000，受主模控制；
             3'b001，强制受L模控制；
             3'b010，强制受W模控制；
             3'b011，强制受TDS模控制；
             3'b100，强制受Gm模控制；
             3'b101，强制受Gs模控制；
             3'b110，强制受C模控制；
             3'b111，无效。
             调试用。
   UNION结构:  BBPCOMM_C1_MIPI_SELECT_MASK_UNION */
#define BBPCOMM_C1_MIPI_SELECT_MASK_ADDR              (SOC_BBP_COMM_BASE_ADDR + 0x31C)

/* 寄存器说明：寄存器
 bit[31:3]   保留
 bit[2:0]    通道1 ABB控制信号来源。
             3'b000，受主模控制；
             3'b001，强制受L模控制；
             3'b010，强制受W模控制；
             3'b011，强制受TDS模控制；
             3'b100，强制受Gm模控制；
             3'b101，强制受Gs模控制；
             3'b110，强制受C模控制；
             3'b111，无效。
             调试用。
   UNION结构:  BBPCOMM_C1_ABB_SELECT_MASK_UNION */
#define BBPCOMM_C1_ABB_SELECT_MASK_ADDR               (SOC_BBP_COMM_BASE_ADDR + 0x320)

/* 寄存器说明：寄存器
 bit[31:3]   保留
 bit[2:0]    通道1 PMU控制信号来源。
             3'b000，受主模控制；
             3'b001，强制受L模控制；
             3'b010，强制受W模控制；
             3'b011，强制受TDS模控制；
             3'b100，强制受Gm模控制；
             3'b101，强制受Gs模控制；
             3'b110，强制受C模控制；
             3'b111，无效。
             调试用。
   UNION结构:  BBPCOMM_C1_PMU_SELECT_MASK_UNION */
#define BBPCOMM_C1_PMU_SELECT_MASK_ADDR               (SOC_BBP_COMM_BASE_ADDR + 0x324)

/* 寄存器说明：寄存器
 bit[31:3]   保留
 bit[2:0]    当前通道1（交叉之前） 输出的强配模式。
             注：其适用于各个外围器件（ABB、RF、PA、mipi等）公共的交叉强配。
             调试用。
   UNION结构:  BBPCOMM_DSP_C1_SEL_MASK_UNION */
#define BBPCOMM_DSP_C1_SEL_MASK_ADDR                  (SOC_BBP_COMM_BASE_ADDR + 0x328)

/* 寄存器说明：寄存器
 bit[31:1]   保留
 bit[0:0]    软件配置的RFIC1复位信号。 低有效。 
             0：复位，1：解复位。
             上电初始化配置为0，根据RFIC手册要求，维持一段时间后配置为1。
             
             V7R2复位值为0。
             V8R1：该寄存器功能删除。
   UNION结构:  BBPCOMM_DSP_RFIC1_RSTN_UNION */
#define BBPCOMM_DSP_RFIC1_RSTN_ADDR                   (SOC_BBP_COMM_BASE_ADDR + 0x32C)

/* 寄存器说明：寄存器
 bit[31:1]   保留
 bit[0]      0:通道1从模使用通道1主模afc控制字
             1:通道1从模使用通道1从模afc控制字
   UNION结构:  BBPCOMM_C1_SECOND_AFC_MODE_UNION */
#define BBPCOMM_C1_SECOND_AFC_MODE_ADDR               (SOC_BBP_COMM_BASE_ADDR + 0x330)

/* 寄存器说明：寄存器
   详      述：bit[1:0]，数据采集模块采数模式配置。
            2'bx0，采集WBBP数据；
            2'b01，采集GBBP1数据；
            2'b11，采集GBBP2数据。
            bit[7:2]，保留。
            bit[8]，V3R3，dsp_rfic0_rstn。软件配置的RFIC0复位信号，低有效。0：复位，1：解复位。上电初始化配置为0，根据RFIC手册要求，维持一段时间后配置为1。V9R1，保留。
            bit[31:9]，保留。
   UNION结构 ：无 */
#define BBPCOMM_BBP_SYS_1CONTROL_ADDR                 (SOC_BBP_COMM_BASE_ADDR + 0x334)

/* 寄存器说明：寄存器
 bit[31:1]   保留
 bit[0]      DSP等待回读结果超时标志位
             1:超时  
   UNION结构:  BBPCOMM_MIPI1_RD_OVERTIME_FLAG_DSP_UNION */
#define BBPCOMM_MIPI1_RD_OVERTIME_FLAG_DSP_ADDR       (SOC_BBP_COMM_BASE_ADDR + 0x338)

/* 寄存器说明：寄存器
 bit[31:1]   保留
 bit[0]      DSP等待回读结果超时标志位
             1:超时  
   UNION结构:  BBPCOMM_SSI1_RD_OVERTIME_FLAG_DSP_UNION */
#define BBPCOMM_SSI1_RD_OVERTIME_FLAG_DSP_ADDR        (SOC_BBP_COMM_BASE_ADDR + 0x33C)

/* 寄存器说明：寄存器
 bit[31:1]   保留
 bit[0]      DSP等待回读结果超时后,清抄时标志位脉冲
   UNION结构:  BBPCOMM_MIPI1_RD_OVERTIME_CLR_UNION */
#define BBPCOMM_MIPI1_RD_OVERTIME_CLR_ADDR            (SOC_BBP_COMM_BASE_ADDR + 0x340)

/* 寄存器说明：寄存器
 bit[31:1]   保留
 bit[0]      DSP等待回读结果超时后,清抄时标志位脉冲
   UNION结构:  BBPCOMM_SSI1_RD_OVERTIME_CLR_UNION */
#define BBPCOMM_SSI1_RD_OVERTIME_CLR_ADDR             (SOC_BBP_COMM_BASE_ADDR + 0x344)

/* 寄存器说明：寄存器
   详      述：xbbp向SSI1 master发出回读类指令时,DSP读取SSI回读结果,回读结果放置在此16寄存器位置由dsp配置,回读数据是否有效参考标志位
   UNION结构 ：无 */
#define BBPCOMM_RD_RESULT_RFIC1_1_ADDR                (SOC_BBP_COMM_BASE_ADDR + 0x348)

/* 寄存器说明：寄存器
   详      述：同上
   UNION结构 ：无 */
#define BBPCOMM_RD_RESULT_RFIC1_2_ADDR                (SOC_BBP_COMM_BASE_ADDR + 0x34C)

/* 寄存器说明：寄存器
   详      述：同上
   UNION结构 ：无 */
#define BBPCOMM_RD_RESULT_RFIC1_3_ADDR                (SOC_BBP_COMM_BASE_ADDR + 0x350)

/* 寄存器说明：寄存器
   详      述：同上
   UNION结构 ：无 */
#define BBPCOMM_RD_RESULT_RFIC1_4_ADDR                (SOC_BBP_COMM_BASE_ADDR + 0x354)

/* 寄存器说明：寄存器
   详      述：同上
   UNION结构 ：无 */
#define BBPCOMM_RD_RESULT_RFIC1_5_ADDR                (SOC_BBP_COMM_BASE_ADDR + 0x358)

/* 寄存器说明：寄存器
   详      述：同上
   UNION结构 ：无 */
#define BBPCOMM_RD_RESULT_RFIC1_6_ADDR                (SOC_BBP_COMM_BASE_ADDR + 0x35C)

/* 寄存器说明：寄存器
   详      述：同上
   UNION结构 ：无 */
#define BBPCOMM_RD_RESULT_RFIC1_7_ADDR                (SOC_BBP_COMM_BASE_ADDR + 0x360)

/* 寄存器说明：寄存器
   详      述：同上
   UNION结构 ：无 */
#define BBPCOMM_RD_RESULT_RFIC1_8_ADDR                (SOC_BBP_COMM_BASE_ADDR + 0x364)

/* 寄存器说明：寄存器
   详      述：同上
   UNION结构 ：无 */
#define BBPCOMM_RD_RESULT_RFIC1_9_ADDR                (SOC_BBP_COMM_BASE_ADDR + 0x368)

/* 寄存器说明：寄存器
   详      述：同上
   UNION结构 ：无 */
#define BBPCOMM_RD_RESULT_RFIC1_10_ADDR               (SOC_BBP_COMM_BASE_ADDR + 0x36C)

/* 寄存器说明：寄存器
   详      述：同上
   UNION结构 ：无 */
#define BBPCOMM_RD_RESULT_RFIC1_11_ADDR               (SOC_BBP_COMM_BASE_ADDR + 0x370)

/* 寄存器说明：寄存器
   详      述：同上
   UNION结构 ：无 */
#define BBPCOMM_RD_RESULT_RFIC1_12_ADDR               (SOC_BBP_COMM_BASE_ADDR + 0x374)

/* 寄存器说明：寄存器
   详      述：同上
   UNION结构 ：无 */
#define BBPCOMM_RD_RESULT_RFIC1_13_ADDR               (SOC_BBP_COMM_BASE_ADDR + 0x378)

/* 寄存器说明：寄存器
   详      述：同上
   UNION结构 ：无 */
#define BBPCOMM_RD_RESULT_RFIC1_14_ADDR               (SOC_BBP_COMM_BASE_ADDR + 0x37C)

/* 寄存器说明：寄存器
   详      述：同上
   UNION结构 ：无 */
#define BBPCOMM_RD_RESULT_RFIC1_15_ADDR               (SOC_BBP_COMM_BASE_ADDR + 0x380)

/* 寄存器说明：寄存器
   详      述：同上
   UNION结构 ：无 */
#define BBPCOMM_RD_RESULT_RFIC1_16_ADDR               (SOC_BBP_COMM_BASE_ADDR + 0x384)

/* 寄存器说明：寄存器
 bit[31:1]   保留
 bit[0]      所有回读指令执行完毕,可以取走回读数据
             1:所有回读指令发送完毕,DSP可以按位置读取SSI回读结果
   UNION结构:  BBPCOMM_RD_RESULT_FLAG_RFIC1_UNION */
#define BBPCOMM_RD_RESULT_FLAG_RFIC1_ADDR             (SOC_BBP_COMM_BASE_ADDR + 0x388)

/* 寄存器说明：寄存器
 bit[31:1]   保留
 bit[0]      1:DSP取走所有回读数据后,写入clr信号清标志位
   UNION结构:  BBPCOMM_RD_RESULT_FLAG_RFIC1_CLR_UNION */
#define BBPCOMM_RD_RESULT_FLAG_RFIC1_CLR_ADDR         (SOC_BBP_COMM_BASE_ADDR + 0x38C)

/* 寄存器说明：寄存器
   详      述：xbbp向MIPI1 master发出回读类指令时,DSP读取SSI回读结果,回读结果放置在此16寄存器位置由dsp配置
   UNION结构 ：无 */
#define BBPCOMM_RD_RESULT_MIPI1_1_ADDR                (SOC_BBP_COMM_BASE_ADDR + 0x390)

/* 寄存器说明：寄存器
   详      述：同上
   UNION结构 ：无 */
#define BBPCOMM_RD_RESULT_MIPI1_2_ADDR                (SOC_BBP_COMM_BASE_ADDR + 0x394)

/* 寄存器说明：寄存器
   详      述：同上
   UNION结构 ：无 */
#define BBPCOMM_RD_RESULT_MIPI1_3_ADDR                (SOC_BBP_COMM_BASE_ADDR + 0x398)

/* 寄存器说明：寄存器
   详      述：同上
   UNION结构 ：无 */
#define BBPCOMM_RD_RESULT_MIPI1_4_ADDR                (SOC_BBP_COMM_BASE_ADDR + 0x39C)

/* 寄存器说明：寄存器
   详      述：同上
   UNION结构 ：无 */
#define BBPCOMM_RD_RESULT_MIPI1_5_ADDR                (SOC_BBP_COMM_BASE_ADDR + 0x3A0)

/* 寄存器说明：寄存器
   详      述：同上
   UNION结构 ：无 */
#define BBPCOMM_RD_RESULT_MIPI1_6_ADDR                (SOC_BBP_COMM_BASE_ADDR + 0x3A4)

/* 寄存器说明：寄存器
   详      述：同上
   UNION结构 ：无 */
#define BBPCOMM_RD_RESULT_MIPI1_7_ADDR                (SOC_BBP_COMM_BASE_ADDR + 0x3A8)

/* 寄存器说明：寄存器
   详      述：同上
   UNION结构 ：无 */
#define BBPCOMM_RD_RESULT_MIPI1_8_ADDR                (SOC_BBP_COMM_BASE_ADDR + 0x3AC)

/* 寄存器说明：寄存器
   详      述：同上
   UNION结构 ：无 */
#define BBPCOMM_RD_RESULT_MIPI1_9_ADDR                (SOC_BBP_COMM_BASE_ADDR + 0x3B0)

/* 寄存器说明：寄存器
   详      述：同上
   UNION结构 ：无 */
#define BBPCOMM_RD_RESULT_MIPI1_10_ADDR               (SOC_BBP_COMM_BASE_ADDR + 0x3B4)

/* 寄存器说明：寄存器
   详      述：同上
   UNION结构 ：无 */
#define BBPCOMM_RD_RESULT_MIPI1_11_ADDR               (SOC_BBP_COMM_BASE_ADDR + 0x3B8)

/* 寄存器说明：寄存器
   详      述：同上
   UNION结构 ：无 */
#define BBPCOMM_RD_RESULT_MIPI1_12_ADDR               (SOC_BBP_COMM_BASE_ADDR + 0x3BC)

/* 寄存器说明：寄存器
   详      述：同上
   UNION结构 ：无 */
#define BBPCOMM_RD_RESULT_MIPI1_13_ADDR               (SOC_BBP_COMM_BASE_ADDR + 0x3C0)

/* 寄存器说明：寄存器
   详      述：同上
   UNION结构 ：无 */
#define BBPCOMM_RD_RESULT_MIPI1_14_ADDR               (SOC_BBP_COMM_BASE_ADDR + 0x3C4)

/* 寄存器说明：寄存器
   详      述：同上
   UNION结构 ：无 */
#define BBPCOMM_RD_RESULT_MIPI1_15_ADDR               (SOC_BBP_COMM_BASE_ADDR + 0x3C8)

/* 寄存器说明：寄存器
   详      述：同上
   UNION结构 ：无 */
#define BBPCOMM_RD_RESULT_MIPI1_16_ADDR               (SOC_BBP_COMM_BASE_ADDR + 0x3CC)

/* 寄存器说明：寄存器
 bit[31:1]   保留
 bit[0]      所有回读指令执行完毕,可以取走回读数据
             1:所有回读指令发送完毕,DSP可以按位置读取SSI回读结果,与下述的软件直接控制MIPI标志位不同
   UNION结构:  BBPCOMM_RD_RESULT_FLAG_MIPI1_UNION */
#define BBPCOMM_RD_RESULT_FLAG_MIPI1_ADDR             (SOC_BBP_COMM_BASE_ADDR + 0x3D0)

/* 寄存器说明：寄存器
 bit[31:1]   保留
 bit[0]      1:DSP取走所有回读数据后,写入clr信号清标志位,与下述的软件直接控制MIPI标志位不同
   UNION结构:  BBPCOMM_RD_RESULT_FLAG_MIPI1_CLR_UNION */
#define BBPCOMM_RD_RESULT_FLAG_MIPI1_CLR_ADDR         (SOC_BBP_COMM_BASE_ADDR + 0x3D4)

/* 寄存器说明：寄存器
   详      述：由软件直接控制MIPI1 master,不借助xbbp发送回读指令时,回读指令存放低32bit.
   UNION结构 ：无 */
#define BBPCOMM_MIPI1_RD_DATA_LOW_SOFT_ADDR           (SOC_BBP_COMM_BASE_ADDR + 0x3D8)

/* 寄存器说明：寄存器
   详      述：由软件直接控制MIPI1 master,不借助xbbp发送回读指令时,回读指令存放高32bit.
   UNION结构 ：无 */
#define BBPCOMM_MIPI1_RD_DATA_HIGH_SOFT_ADDR          (SOC_BBP_COMM_BASE_ADDR + 0x3DC)

/* 寄存器说明：寄存器
 bit[31:1]   保留
 bit[0]      由软件直接控制MIPI1 master,不借助xbbp发送回读指令时,回读数据有效指示
   UNION结构:  BBPCOMM_RD_END_FLAG_MIPI1_SOFT_UNION */
#define BBPCOMM_RD_END_FLAG_MIPI1_SOFT_ADDR           (SOC_BBP_COMM_BASE_ADDR + 0x3E0)

/* 寄存器说明：寄存器
 bit[31:1]   保留
 bit[0]      由软件直接控制MIPI1 master
             1:软件可以发送CFG_IND启动mipi1
             0:软件等待mipi1授权使用
   UNION结构:  BBPCOMM_MIPI1_GRANT_DSP_UNION */
#define BBPCOMM_MIPI1_GRANT_DSP_ADDR                  (SOC_BBP_COMM_BASE_ADDR + 0x3E4)

/* 寄存器说明：寄存器
   详      述：由软件直接控制MIPI1 master,不借助xbbp发送写指令时,写指令高32bit。
            在MIPI动态使能mipi_dyn_exchg_en有效，在MIPI扩展指令时
            dsp_mipi1_wdata_low[0]应配置为0
   UNION结构 ：无 */
#define BBPCOMM_DSP_MIPI1_WDATA_LOW_ADDR              (SOC_BBP_COMM_BASE_ADDR + 0x3E8)

/* 寄存器说明：寄存器
   详      述：由软件直接控制MIPI1 master,不借助xbbp发送写指令时,写指令高32bit。
            在MIPI动态使能mipi_dyn_exchg_en有效，在MIPI非扩展指令时
            dsp_mipi1_wdata_high[0]应配置为0
   UNION结构 ：无 */
#define BBPCOMM_DSP_MIPI1_WDATA_HIGH_ADDR             (SOC_BBP_COMM_BASE_ADDR + 0x3EC)

/* 寄存器说明：寄存器
 bit[31:1]   保留
 bit[0]      由软件直接控制MIPI1 master,需首先拉高此指示,屏蔽xbbp对MIPI1的控制
   UNION结构:  BBPCOMM_DSP_MIPI1_EN_IMI_UNION */
#define BBPCOMM_DSP_MIPI1_EN_IMI_ADDR                 (SOC_BBP_COMM_BASE_ADDR + 0x3F0)

/* 寄存器说明：寄存器
 bit[31:1]   保留
 bit[0]      由软件直接控制MIPI1 master,拉高此dsp_mipi1_en后,产生软件启动脉冲
   UNION结构:  BBPCOMM_DSP_MIPI1_CFG_IND_IMI_UNION */
#define BBPCOMM_DSP_MIPI1_CFG_IND_IMI_ADDR            (SOC_BBP_COMM_BASE_ADDR + 0x3F4)

/* 寄存器说明：寄存器
 bit[31:1]   保留
 bit[0]      由软件直接控制MIPI1 master,取走回读数据后,清标志位脉冲
   UNION结构:  BBPCOMM_DSP_MIPI1_RD_CLR_UNION */
#define BBPCOMM_DSP_MIPI1_RD_CLR_ADDR                 (SOC_BBP_COMM_BASE_ADDR + 0x3F8)

/* 寄存器说明：寄存器
 bit[31:9]   保留
 bit[8:8]    软件强配模式abb1_rxb_blk_en.
 bit[7:7]    软件强配模式abb1_rxa_blk_en.
 bit[6:6]    软件强配模式abb1_rxb_en.
 bit[5:5]    软件强配模式abb1_rxa_en.
 bit[4:4]    软件强配模式abb1_tx_en.
 bit[3:1]    软件强配模式abb1_mode_sel.
 bit[0:0]    ABB1线控强配模式：
             1：表示软件强配，此时上述各个强配才生效；
             0：表示由CTU根据各BBP产生。
             默认值为0。
             测试用。
   UNION结构:  BBPCOMM_ABB1_LINE_CONTROL_CMD_UNION */
#define BBPCOMM_ABB1_LINE_CONTROL_CMD_ADDR            (SOC_BBP_COMM_BASE_ADDR + 0x3FC)

/* 寄存器说明：
 bit[31:3]   保留
 bit[2:0]    V9R1版本，软件不配置。
   UNION结构:  BBPCOMM_CH1_BBP_SEL_UNION */
#define BBPCOMM_CH1_BBP_SEL_ADDR                      (SOC_BBP_COMM_BASE_ADDR + 0x400)

/* 寄存器说明：
 bit[31:1]   保留
 bit[0]      cpu配置mipi功能,配置为1才能正常工作, 测试时配置为0
   UNION结构:  BBPCOMM_CPU_MIPI1_FUNC_SEL_UNION */
#define BBPCOMM_CPU_MIPI1_FUNC_SEL_ADDR               (SOC_BBP_COMM_BASE_ADDR + 0x404)

/* 寄存器说明：
 bit[31:1]   保留
 bit[0]      cpu配置mipi功能,测试用
   UNION结构:  BBPCOMM_CPU_MIPI1_TEST_FUNC_UNION */
#define BBPCOMM_CPU_MIPI1_TEST_FUNC_ADDR              (SOC_BBP_COMM_BASE_ADDR + 0x408)

/* 寄存器说明：
 bit[31:1]   保留
 bit[0]      cpu配置mipi功能,测试用
   UNION结构:  BBPCOMM_CPU_MIPI1_SCLK_TEST_UNION */
#define BBPCOMM_CPU_MIPI1_SCLK_TEST_ADDR              (SOC_BBP_COMM_BASE_ADDR + 0x40C)

/* 寄存器说明：
 bit[31:1]   保留
 bit[0]      cpu配置mipi功能,测试用
   UNION结构:  BBPCOMM_CPU_MIPI1_SDATA_TEST_UNION */
#define BBPCOMM_CPU_MIPI1_SDATA_TEST_ADDR             (SOC_BBP_COMM_BASE_ADDR + 0x410)

/* 寄存器说明：
 bit[31:1]   保留
 bit[0]      cpu配置mipi功能,mipi1master清异常信号
   UNION结构:  BBPCOMM_DSP_MIPI1_CLR_IND_IMI_UNION */
#define BBPCOMM_DSP_MIPI1_CLR_IND_IMI_ADDR            (SOC_BBP_COMM_BASE_ADDR + 0x414)

/* 寄存器说明：
 bit[31:1]   保留
 bit[0]      mipi1存储待发送数据同步FIFO复位信号
   UNION结构:  BBPCOMM_CPU_MIPI1_FIFO_CLR_IMI_UNION */
#define BBPCOMM_CPU_MIPI1_FIFO_CLR_IMI_ADDR           (SOC_BBP_COMM_BASE_ADDR + 0x418)

/* 寄存器说明：
 bit[31:1]   保留
 bit[0]      ssi1存储待发送数据同步FIFO复位信号
   UNION结构:  BBPCOMM_CPU_SSI1_FIFO_CLR_IMI_UNION */
#define BBPCOMM_CPU_SSI1_FIFO_CLR_IMI_ADDR            (SOC_BBP_COMM_BASE_ADDR + 0x41C)

/* 寄存器说明：
 bit[31:12]  保留
 bit[11]     C模通道1 TX ABB随路时钟门控旁路信号
 bit[10]     C模通道1 RX ABB随路时钟门控旁路信号
 bit[9]      G2通道1 TX ABB随路时钟门控旁路信号
 bit[8]      G1通道1 TX ABB随路时钟门控旁路信号
 bit[7]      W模通道1 TX ABB随路时钟门控旁路信号
 bit[6]      TDS通道1 TX ABB随路时钟门控旁路信号
 bit[5]      L模通道1 TX ABB随路时钟门控旁路信号
 bit[4]      G2通道1 RX ABB随路时钟门控旁路信号
 bit[3]      G1通道1 RX ABB随路时钟门控旁路信号
 bit[2]      W模通道1 RX ABB随路时钟门控旁路信号
 bit[1]      TDS通道1 RX ABB随路时钟门控旁路信号
 bit[0]      L模通道1 RX ABB随路时钟门控旁路信号
   UNION结构:  BBPCOMM_CPU_RX1_TX1_CKG_BYPASS_UNION */
#define BBPCOMM_CPU_RX1_TX1_CKG_BYPASS_ADDR           (SOC_BBP_COMM_BASE_ADDR + 0x420)

/* 寄存器说明：定时锁存使能指示寄存器
 bit[31:6]   保留
 bit[5:4]    表示timing_get_mode=1时bbe读取C模bbp计数器结果1x、evdo还是base_cnt。   00：1x定时，01：evdo定时，10： base_cnt ，11：无效。
             timing_get_mode=0时，不需配置，无意义
 bit[3:1]    保留
 bit[0]      逻辑自清零。
             timing_get_mode=0，DSP需要获取定时关系时配置生效，CTU逻辑根据此信号锁存LTE/W/Gm/Gs/TDS/C的系统定时，CTU的内部定时供DSP查询。
             timing_get_mode=1，bbe16需要获取定时关系时配置生效，CTU逻辑根据此信号锁存LTE/W/Gm/Gs/TDS/C的系统定时，CTU的内部定时供bbe16查询。
             配置时刻：软件需要锁存定时
             生效时刻：立即生效
   UNION结构:  BBPCOMM_TIMING_GET_IND_UNION */
#define BBPCOMM_TIMING_GET_IND_ADDR                   (SOC_BBP_COMM_BASE_ADDR + 0x610)

/* 寄存器说明：定时标志清除寄存器
 bit[31:2]   保留
 bit[1]      逻辑自清零。
             32公共定时信息的标志清除，用于清除timing_valid标志。
             配置时刻：DSP读取定时完毕后配置
             生效时刻：立即生效
             （因为32K定时信息的上报延时可能远大于其他系统的定时信息，所以单独处理）
             （timing_get_mode=0，1套查询接口；
             timing_get_mode=1，2套查询接口）
 bit[0]      逻辑自清零。
             定时标志清除，用于清除timing_valid标志。
             配置时刻：DSP读取定时完毕后配置
             生效时刻：立即生效
             （timing_get_mode=0，1套查询接口；
             timing_get_mode=1，2套查询接口）
   UNION结构:  BBPCOMM_TIMING_CLEAR_UNION */
#define BBPCOMM_TIMING_CLEAR_ADDR                     (SOC_BBP_COMM_BASE_ADDR + 0x614)

/* 寄存器说明：定时锁存延迟寄存器
 bit[31:5]   保留
 bit[4:0]    CTU检测到timing_get_ind后固定延迟delay_cnt个CTU时钟周期后拉高timing_valid信号。
             配置时刻：上电初始化
             生效时刻：立即生效
   UNION结构:  BBPCOMM_DELAY_CNT_UNION */
#define BBPCOMM_DELAY_CNT_ADDR                        (SOC_BBP_COMM_BASE_ADDR + 0x618)

/* 寄存器说明：中断延迟寄存器
 bit[31:16]  保留
 bit[15:0]   CTU中断上报等待时间，单位为一个CLK周期。
             DSP清除中断标志寄存器的某种类型bit后，CTU上报中断电平需要拉低，如果此时中断标志寄存器还有其它中断类型有效，则需等待int_wait_cnt个时钟周期后再拉高CTU上报中断。
             配置时刻：上电初始化
             生效时刻：立即生效
   UNION结构:  BBPCOMM_INT_WAIT_CNT_UNION */
#define BBPCOMM_INT_WAIT_CNT_ADDR                     (SOC_BBP_COMM_BASE_ADDR + 0x61C)

/* 寄存器说明：寄存器
 bit[31:2]   保留
 bit[1:0]    电平信号，低有效。
             当LTE主模需要做异系统测量时需配置本寄存器为3，异系统测量结束后配置本寄存器为0。
             bit1:1’b0: lte_intrasys_valid输入无效，被强制为1。
             bit0:1’b0:
             lte_intersys_ingap_pre_ind输入无效，被强制为0。
             bit[1:0] = 11b: 让lte_intrasys_valid和lte_intersys_ingap_pre_ind输入有效。
             
             配置时刻：LTE主模需要做异系统测量时需配置本寄存器为3，异系统测量结束后配置本寄存器为0。
             生效时刻：立即生效
   UNION结构:  BBPCOMM_LTEBBP_INPUT_MASK_UNION */
#define BBPCOMM_LTEBBP_INPUT_MASK_ADDR                (SOC_BBP_COMM_BASE_ADDR + 0x620)

/* 寄存器说明：寄存器
 bit[31:2]   保留
 bit[1:0]    电平信号，低有效。
             当W主模需要做异系统测量时需配置本寄存器为3，异系统测量结束后配置本寄存器为0。
             bit1:1’b0:
              让w_intrasys_valid输入无效，被强制为1。
             bit0:1’b0:
             w_intersys_ingap_pre_ind输入无效，被强制为0。
             bit[1:0] = 11b: 让w_intrasys_valid和w_intersys_ingap_pre_ind输入有效。
             
             配置时刻：W主模需要做异系统测量时需配置本寄存器为3，异系统测量结束后配置本寄存器为0。
             生效时刻：立即生效
   UNION结构:  BBPCOMM_WBBP_INPUT_MASK_UNION */
#define BBPCOMM_WBBP_INPUT_MASK_ADDR                  (SOC_BBP_COMM_BASE_ADDR + 0x624)

/* 寄存器说明：寄存器
 bit[31:2]   保留
 bit[1:0]    电平信号，低有效。
             当Gm主模需要做异系统测量时需配置本寄存器为3，异系统测量结束后配置本寄存器为0。
             bit1:1’b0:
              让gm_intrasys_valid输入无效，被强制为1。
             bit0:1’b0:
             gm_intersys_ingap_pre_ind输入无效，被强制为0。
             bit[1:0] = 11b: 让gm_intrasys_valid和gm_intersys_ingap_pre_ind输入有效。
             
             配置时刻：Gm主模需要做异系统测量时需配置本寄存器为3，异系统测量结束后配置本寄存器为0。
             生效时刻：立即生效
   UNION结构:  BBPCOMM_GMBBP_INPUT_MASK_UNION */
#define BBPCOMM_GMBBP_INPUT_MASK_ADDR                 (SOC_BBP_COMM_BASE_ADDR + 0x628)

/* 寄存器说明：寄存器
 bit[31:2]   保留
 bit[1:0]    电平信号，低有效。
             当Gs主模需要做异系统测量时需配置本寄存器为3，异系统测量结束后配置本寄存器为0。
             bit1:1’b0:
              让gs_intrasys_valid输入无效，被强制为1。
             bit0:1’b0:
             gs_intersys_ingap_pre_ind输入无效，被强制为0。
             bit[1:0] = 11b: 让gs_intrasys_valid和gs_intersys_ingap_pre_ind输入有效。
             
             配置时刻：Gs主模需要做异系统测量时需配置本寄存器为3，异系统测量结束后配置本寄存器为0。
             生效时刻：立即生效
   UNION结构:  BBPCOMM_GSBBP_INPUT_MASK_UNION */
#define BBPCOMM_GSBBP_INPUT_MASK_ADDR                 (SOC_BBP_COMM_BASE_ADDR + 0x62C)

/* 寄存器说明：寄存器
 bit[31:2]   保留
 bit[1:0]    电平信号，低有效。
             当TDS主模需要做异系统测量时需配置本寄存器为3，异系统测量结束后配置本寄存器为0。
             bit1:1’b0:
              让tds_intrasys_valid输入无效，被强制为1。
             bit0:1’b0:
             tds_intersys_ingap_pre_ind输入无效，被强制为0。
             bit[1:0] = 11b: 让tds_intrasys_valid和tds_intersys_ingap_pre_ind输入有效。
             
             配置时刻：TDS主模需要做异系统测量时需配置本寄存器为3，异系统测量结束后配置本寄存器为0。
             生效时刻：立即生效
   UNION结构:  BBPCOMM_TDSBBP_INPUT_MASK_UNION */
#define BBPCOMM_TDSBBP_INPUT_MASK_ADDR                (SOC_BBP_COMM_BASE_ADDR + 0x630)

/* 寄存器说明：寄存器
   详      述：用于提供LTE的复帧号，由LDSP更新。取值范围是0～1223。
            
            配置时刻：
            生效时刻：立即生效
   UNION结构 ：无 */
#define BBPCOMM_LTE_MUX_FRAME_NUM_ADDR                (SOC_BBP_COMM_BASE_ADDR + 0x634)

/* 寄存器说明：寄存器
   详      述：用于提供TDS的复帧号，由TDSP更新。取值范围是0～1223。
            
            配置时刻：
            生效时刻：立即生效
   UNION结构 ：无 */
#define BBPCOMM_TDS_MUX_FRAME_NUM_ADDR                (SOC_BBP_COMM_BASE_ADDR + 0x638)

/* 寄存器说明：寄存器
   详      述：用于提供w的复帧号，由wDSP更新。
            配置时刻：
            生效时刻：立即生效
   UNION结构 ：无 */
#define BBPCOMM_W_MUX_FRAME_NUM_ADDR                  (SOC_BBP_COMM_BASE_ADDR + 0x63C)

/* 寄存器说明：寄存器
   详      述：W/TD/GM/GS/C为主模时写此寄存器，L为从模时读取此寄存器。此寄存器由DSP之间进行读写，BBP仅提供接口。
   UNION结构 ：无 */
#define BBPCOMM_MEAS_FLAG_L_RD_ADDR                   (SOC_BBP_COMM_BASE_ADDR + 0x640)

/* 寄存器说明：寄存器
   详      述：L/TD/GM/GS/C为主模时写此寄存器，W为从模时读取此寄存器。此寄存器由DSP之间进行读写，BBP仅提供接口。
   UNION结构 ：无 */
#define BBPCOMM_MEAS_FLAG_W_RD_ADDR                   (SOC_BBP_COMM_BASE_ADDR + 0x644)

/* 寄存器说明：寄存器
   详      述：W/L/GM/GS为/C主模时写此寄存器，TD为从模时读取此寄存器。此寄存器由DSP之间进行读写，BBP仅提供接口。
   UNION结构 ：无 */
#define BBPCOMM_MEAS_FLAG_T_RD_ADDR                   (SOC_BBP_COMM_BASE_ADDR + 0x648)

/* 寄存器说明：寄存器
   详      述：W/TD/L/GS/C为主模时写此寄存器，GM为从模时读取此寄存器。此寄存器由DSP之间进行读写，BBP仅提供接口。
   UNION结构 ：无 */
#define BBPCOMM_MEAS_FLAG_GM_RD_ADDR                  (SOC_BBP_COMM_BASE_ADDR + 0x64C)

/* 寄存器说明：寄存器
   详      述：W/TD/GM/L/C为主模时写此寄存器，GS为从模时读取此寄存器。此寄存器由DSP之间进行读写，BBP仅提供接口。
   UNION结构 ：无 */
#define BBPCOMM_MEAS_FLAG_GS_RD_ADDR                  (SOC_BBP_COMM_BASE_ADDR + 0x650)

/* 寄存器说明：寄存器
   详      述：L为从模时写此寄存器，W/TD/GM/GS/C主模读取此寄存器。此寄存器由DSP之间进行读写，BBP仅提供接口。
   UNION结构 ：无 */
#define BBPCOMM_WAKE_FLAG_L_WR_ADDR                   (SOC_BBP_COMM_BASE_ADDR + 0x654)

/* 寄存器说明：寄存器
   详      述：W为从模时写此寄存器，L/TD/GM/GS/C主模读取此寄存器。此寄存器由DSP之间进行读写，BBP仅提供接口。
   UNION结构 ：无 */
#define BBPCOMM_WAKE_FLAG_W_WR_ADDR                   (SOC_BBP_COMM_BASE_ADDR + 0x658)

/* 寄存器说明：寄存器
   详      述：TD为从模时写此寄存器，W/L/GM/GS/C主模读取此寄存器。此寄存器由DSP之间进行读写，BBP仅提供接口。
   UNION结构 ：无 */
#define BBPCOMM_WAKE_FLAG_T_WR_ADDR                   (SOC_BBP_COMM_BASE_ADDR + 0x65C)

/* 寄存器说明：寄存器
   详      述：GM为从模时写此寄存器，W/TD/L/GS/C主模读取此寄存器。此寄存器由DSP之间进行读写，BBP仅提供接口。
   UNION结构 ：无 */
#define BBPCOMM_WAKE_FLAG_GM_WR_ADDR                  (SOC_BBP_COMM_BASE_ADDR + 0x660)

/* 寄存器说明：寄存器
   详      述：GS为从模时写此寄存器，W/TD/GM/L/C主模读取此寄存器。此寄存器由DSP之间进行读写，BBP仅提供接口。
   UNION结构 ：无 */
#define BBPCOMM_WAKE_FLAG_GS_WR_ADDR                  (SOC_BBP_COMM_BASE_ADDR + 0x664)

/* 寄存器说明：寄存器
 bit[31:6]   保留
 bit[5:4]    表示o_timing_valid有效时，上报c模的计数器给bbe16的是1x、evdo还是base_cnt
             00：1x定时
             01： evdo定时
             10： basecent
 bit[3]      保留
 bit[2]      表示bbe16读取定时信息超时（在超时保护时间内未给出o_timing_clear信号）
 bit[1]      32K公共定时上报有效标志，高有效。
             DSP配置timing_get_ind后，查询此标志，如果为高，则可以获取32K公共定时器的定时信息。
             DSP读取定时完毕后配置comm_32k_timing_clear寄存器，comm_32k_timing_valid拉低为无效。
             32K定时锁存稳定后拉高comm_32k_timing_valid
             配置时刻：
             生效时刻：立即生效
             （timing_get_mode=0，1套查询接口；
             timing_get_mode=1，2套查询接口）
 bit[0]      定时上报有效标志，高有效。
             DSP配置timing_get_ind后，查询此标志，如果为高，则可以获取LTE，W，Gm，Gs, TDS的系统定时。
             DSP读取定时完毕后配置timing_clear寄存器，timing_valid拉低为无效。
             CTU检测到timing_get_ind后固定延迟delay_cnt后拉高timing_valid。
             
             配置时刻：
             生效时刻：立即生效
             （timing_get_mode=0，dsp控制，
             timing_get_mode=1，bbe16控制）
   UNION结构:  BBPCOMM_TIMING_VALID_UNION */
#define BBPCOMM_TIMING_VALID_ADDR                     (SOC_BBP_COMM_BASE_ADDR + 0x668)

/* 寄存器说明：寄存器
 bit[31:20]  W的系统定时信息，由DSP 配置timing_get_ind触发上报。
             上报时刻：timing_valid为高时上报有效.
             [31:20]表示W的SFN计数（源自w_timing_b[11:0]），范围0~4095。
 bit[19:16]  [19:16]为时隙计数，范围0~14
 bit[15:4]   [15:4]为时隙内chip计数，范围0~2559
 bit[3:0]    [3:0]为chip内时钟计数，范围0~15
   UNION结构:  BBPCOMM_W_TIMING_DSP_UNION */
#define BBPCOMM_W_TIMING_DSP_ADDR                     (SOC_BBP_COMM_BASE_ADDR + 0x66C)

/* 寄存器说明：寄存器
 bit[31:22]  保留
 bit[21:0]   GM的FN计数（源自gm_timing_a[21:0]），由DSP 配置timing_get_ind触发上报。其中[10:0]表示fn_low,范围0~1325；[21:11]表示fn_hign,范围0~2047。
             上报时刻：timing_valid为高时上报有效
   UNION结构:  BBPCOMM_GM_FN_DSP_UNION */
#define BBPCOMM_GM_FN_DSP_ADDR                        (SOC_BBP_COMM_BASE_ADDR + 0x670)

/* 寄存器说明：寄存器
 bit[31:13]  保留
 bit[12:0]   GM的系统定时Qb信息，由DSP 配置timing_get_ind触发上报。
              [12:0]表示 G系统定时帧内Qb计数（源自gm_timing_b[12:0]），范围0~4999。
             上报时刻：timing_valid为高时上报有效
   UNION结构:  BBPCOMM_GM_QB_DSP_UNION */
#define BBPCOMM_GM_QB_DSP_ADDR                        (SOC_BBP_COMM_BASE_ADDR + 0x674)

/* 寄存器说明：寄存器
 bit[31:22]  保留
 bit[21:0]   GS的FN计数（源自gs_timing_a[21:0]），由DSP 配置timing_get_ind触发上报。其中[10:0]表示fn_low,范围0~1325；[21:11]表示fn_hign,范围0~2047。
             上报时刻：timing_valid为高时上报有效
   UNION结构:  BBPCOMM_GS_FN_DSP_UNION */
#define BBPCOMM_GS_FN_DSP_ADDR                        (SOC_BBP_COMM_BASE_ADDR + 0x678)

/* 寄存器说明：寄存器
 bit[31:13]  保留
 bit[12:0]   GS的系统定时Qb信息，由DSP 配置timing_get_ind触发上报。
              [12:0]表示 GS系统定时帧内Qb计数（源自gs_timing_b[12:0]），范围0~4999。
             上报时刻：timing_valid为高时上报有效
   UNION结构:  BBPCOMM_GS_QB_DSP_UNION */
#define BBPCOMM_GS_QB_DSP_ADDR                        (SOC_BBP_COMM_BASE_ADDR + 0x67C)

/* 寄存器说明：寄存器
 bit[31:30]  保留
 bit[29:20]  LTE的系统定时。由DSP 配置timing_get_ind触发上报。上报时刻：timing_valid为高时上报有效。
             其中:
             [29:20]表示SFN计数，范围0~1023
 bit[19:16]  [19:16]是子帧计数
 bit[15:15]  保留
 bit[14:0]   [14:0]是子帧内计数。 
   UNION结构:  BBPCOMM_LTE_TIMING_DSP_UNION */
#define BBPCOMM_LTE_TIMING_DSP_ADDR                   (SOC_BBP_COMM_BASE_ADDR + 0x680)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:0]   TDS的系统定时。由DSP 配置timing_get_ind触发上报。本寄存器上报的是TDS的tcfsr系统子帧号0~65535循环计数，TDS实际有效子帧范围为0~8191
             上报时刻：timing_valid为高时上报有效
   UNION结构:  BBPCOMM_TDS_TIMING1_DSP_UNION */
#define BBPCOMM_TDS_TIMING1_DSP_ADDR                  (SOC_BBP_COMM_BASE_ADDR + 0x684)

/* 寄存器说明：寄存器
 bit[31:20]  保留
 bit[19:0]   TDS的系统定时。由DSP 配置timing_get_ind触发上报。本寄存器上报的是是TDS的framc帧计数
             上报时刻：timing_valid为高时上报有效
   UNION结构:  BBPCOMM_TDS_TIMING2_DSP_UNION */
#define BBPCOMM_TDS_TIMING2_DSP_ADDR                  (SOC_BBP_COMM_BASE_ADDR + 0x688)

/* 寄存器说明：寄存器
   详      述：CTU内部定时，由DSP 配置timing_get_ind触发上报。计数器使用总线钟
            上报时刻：timing_valid为高时上报有效
   UNION结构 ：无 */
#define BBPCOMM_CTU_TIMING_DSP_ADDR                   (SOC_BBP_COMM_BASE_ADDR + 0x68C)

/* 寄存器说明：寄存器
   详      述：CTU内部定时计数器，有时钟就计数，计满从头开始。计数器使用总线钟
            上报时刻：随时读取。
   UNION结构 ：无 */
#define BBPCOMM_CTU_BASECNT1_DSP_ADDR                 (SOC_BBP_COMM_BASE_ADDR + 0x690)

/* 寄存器说明：寄存器
   详      述：CTU内部定时。使用clk_19m_bbp_soc时钟计数。
            各主模在int0、int1、int2的时刻触发上报。
   UNION结构 ：无 */
#define BBPCOMM_CTU_BASECNT2_DSP_ADDR                 (SOC_BBP_COMM_BASE_ADDR + 0x694)

/* 寄存器说明：寄存器
 bit[31:2]   保留
 bit[1]      w主模下实际控制RF指示，电平信号。为1代表W控制RF配置通道，为0代表从模可以控制RF通道。用于单RFIC时射频控制相关信号的选通。
             w_intrasys_valid_delay的上升沿比w_intrasys_valid上升沿滞后，下降沿一致。
 bit[0]      w主模下控制RF指示，电平信号。
             1’b0: W放弃RF控制权。
             1’b1: W享有控制RF权（但实际配置RF需w_intrasys_valid_delay信号为1）。
   UNION结构:  BBPCOMM_W_INTRASYS_VALID_UNION */
#define BBPCOMM_W_INTRASYS_VALID_ADDR                 (SOC_BBP_COMM_BASE_ADDR + 0x698)

/* 寄存器说明：寄存器
 bit[31:2]   保留
 bit[1]      gm主模下实际控制RF指示，电平信号。为1代表gm控制RF配置通道，为0代表从模可以控制RF通道。用于单RFIC时射频控制相关信号的选通。
             gm_intrasys_valid_delay的上升沿比gm_intrasys_valid上升沿滞后，下降沿一致。
 bit[0]      gm主模下控制RF指示，电平信号。
             1’b0: gm放弃RF控制权。
             1’b1: gm享有控制RF权（但实际配置RF需gm_intrasys_valid_delay信号为1）。
   UNION结构:  BBPCOMM_GM_INTRASYS_VALID_UNION */
#define BBPCOMM_GM_INTRASYS_VALID_ADDR                (SOC_BBP_COMM_BASE_ADDR + 0x69C)

/* 寄存器说明：寄存器
 bit[31:2]   保留
 bit[1]      gs主模下实际控制RF指示，电平信号。为1代表gs控制RF配置通道，为0代表从模可以控制RF通道。用于单RFIC时射频控制相关信号的选通。
             gs_intrasys_valid_delay的上升沿比gs_intrasys_valid上升沿滞后，下降沿一致。
 bit[0]      gs主模下控制RF指示，电平信号。
             1’b0: gs放弃RF控制权。
             1’b1: gs享有控制RF权（但实际配置RF需gs_intrasys_valid_delay信号为1）。
   UNION结构:  BBPCOMM_GS_INTRASYS_VALID_UNION */
#define BBPCOMM_GS_INTRASYS_VALID_ADDR                (SOC_BBP_COMM_BASE_ADDR + 0x6A0)

/* 寄存器说明：寄存器
 bit[31:2]   保留
 bit[1]      LTE主模下实际控制RF指示，电平信号。为1代表LTE控制RF配置通道，为0代表从模可以控制RF通道。用于单RFIC时射频控制相关信号的选通。
             lte_intrasys_valid_delay的上升沿比lte_intrasys_valid上升沿滞后，下降沿一致。
 bit[0]      Lte主模下控制RF指示，电平信号。
             1’b0: LTE放弃RF控制权。
             1’b1: LTE享有控制RF权（但实际配置RF需lte_intrasys_valid_delay信号为1）。
   UNION结构:  BBPCOMM_LTE_INTRASYS_VALID_UNION */
#define BBPCOMM_LTE_INTRASYS_VALID_ADDR               (SOC_BBP_COMM_BASE_ADDR + 0x6A4)

/* 寄存器说明：寄存器
 bit[31:2]   保留
 bit[1]      TDS主模下实际控制RF指示，电平信号。为1代表TDS控制RF配置通道，为0代表从模可以控制RF通道。用于单RFIC时射频控制相关信号的选通。
             tds_intrasys_valid_delay的上升沿比tds_intrasys_valid上升沿滞后，下降沿一致。
 bit[0]      TDS主模下控制RF指示，电平信号。
             1’b0: TDS放弃RF控制权。
             1’b1: TDS享有控制RF权（但实际配置RF需tds_intrasys_valid_delay信号为1）。
   UNION结构:  BBPCOMM_TDS_INTRASYS_VALID_UNION */
#define BBPCOMM_TDS_INTRASYS_VALID_ADDR               (SOC_BBP_COMM_BASE_ADDR + 0x6A8)

/* 寄存器说明：寄存器
 bit[31:6]   保留
 bit[5:3]    SSI0master等待时间
 bit[2:0]    SSI1master等待时间
   UNION结构:  BBPCOMM_RFIC_CK_CFG_UNION */
#define BBPCOMM_RFIC_CK_CFG_ADDR                      (SOC_BBP_COMM_BASE_ADDR + 0x6AC)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:8]   通过xbbp向SSI master发出回读类指令时,DSP等待回读结果长度配置
 bit[7:0]    通过xbbp向MIPI master发出回读类指令时,DSP等待回读结果长度配置
   UNION结构:  BBPCOMM_DSP_WAITTIME_UNION */
#define BBPCOMM_DSP_WAITTIME_ADDR                     (SOC_BBP_COMM_BASE_ADDR + 0x6B4)

/* 寄存器说明：寄存器
   详      述：CTU内部19.2M钟驱动定时计数器上报结果
   UNION结构 ：无 */
#define BBPCOMM_CTU_BASECNT_DSP_ADDR                  (SOC_BBP_COMM_BASE_ADDR + 0x6B8)

/* 寄存器说明：寄存器
 bit[31:1]   保留
 bit[0]      CTU内部19.2M钟驱动定时计数器结果读取使能
   UNION结构:  BBPCOMM_DSP_CTU_TIMING_GET_IND_UNION */
#define BBPCOMM_DSP_CTU_TIMING_GET_IND_ADDR           (SOC_BBP_COMM_BASE_ADDR + 0x6BC)

/* 寄存器说明：寄存器
 bit[31:1]   保留
 bit[0]      CTU内部19.2M钟驱动定时计数器结果读取有效指示
   UNION结构:  BBPCOMM_CTU_TIMING_VALID_DSP_UNION */
#define BBPCOMM_CTU_TIMING_VALID_DSP_ADDR             (SOC_BBP_COMM_BASE_ADDR + 0x6C0)

/* 寄存器说明：寄存器
 bit[31:1]   保留
 bit[0]      CTU内部19.2M钟驱动定时计数器结果读取后清零使能
   UNION结构:  BBPCOMM_DSP_CTU_TIMING_CLR_UNION */
#define BBPCOMM_DSP_CTU_TIMING_CLR_ADDR               (SOC_BBP_COMM_BASE_ADDR + 0x6C4)

/* 寄存器说明：寄存器
 bit[31:4]   保留
 bit[3:0]    异系统测量类型。
             4’b0：测量3G。
             4’b1: 3G背景搜；
             4’d2: 3G ANR。
             其它预留。
             生效时刻：立即生效
   UNION结构:  BBPCOMM_W_INTERSYS_MEASURE_TYPE_UNION */
#define BBPCOMM_W_INTERSYS_MEASURE_TYPE_ADDR          (SOC_BBP_COMM_BASE_ADDR + 0x6C8)

/* 寄存器说明：寄存器
 bit[31:1]   保留
 bit[0:0]    单频点测量周期到指示，L或者TDS/Gm/Gs作为主模，通知W DSP W测量的单频点周期到达。
             只在连接态下需要配置。
             电平信号，高有效。
             生效时刻：立即生效
   UNION结构:  BBPCOMM_W_MEASURE_REPORT_VALID_UNION */
#define BBPCOMM_W_MEASURE_REPORT_VALID_ADDR           (SOC_BBP_COMM_BASE_ADDR + 0x6CC)

/* 寄存器说明：寄存器
   详      述：预处理中断int0到int1之间的offset信息，单位us（向下取整）。。
            配置时刻：主模启动W从模异系统测量时配置
            生效时刻：立即生效
   UNION结构 ：无 */
#define BBPCOMM_W_PREINT_OFFSET_TIME_ADDR             (SOC_BBP_COMM_BASE_ADDR + 0x6D0)

/* 寄存器说明：寄存器
   详      述：W作为从模时，主模分配给W用于异系统测量时间长度，告知WDSP。单位us，向下取整到us。
            配置时刻：主模启动W从模异系统测量时配置
            生效时刻：立即生效
   UNION结构 ：无 */
#define BBPCOMM_W_INTERSYS_MEASURE_TIME_ADDR          (SOC_BBP_COMM_BASE_ADDR + 0x6D4)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:0]   逻辑自清零。
             中断标志清除指示，高有效。用于清除INT_TYPE_W_RCV_LTE[15:0]对应bit位
             生效时刻：立即生效
   UNION结构:  BBPCOMM_INT_CLEAR_W_RCV_LTE_UNION */
#define BBPCOMM_INT_CLEAR_W_RCV_LTE_ADDR              (SOC_BBP_COMM_BASE_ADDR + 0x6D8)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:0]   逻辑自清零。
             中断标志清除指示，高有效。用于清除INT_TYPE_W_RCV_TDS[15:0]对应bit位
             生效时刻：立即生效
   UNION结构:  BBPCOMM_INT_CLEAR_W_RCV_TDS_UNION */
#define BBPCOMM_INT_CLEAR_W_RCV_TDS_ADDR              (SOC_BBP_COMM_BASE_ADDR + 0x6DC)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:0]   逻辑自清零。
             中断标志清除指示，高有效。用于清除INT_TYPE_W_RCV_GM[15:0]对应bit位
             生效时刻：立即生效
   UNION结构:  BBPCOMM_INT_CLEAR_W_RCV_GM_UNION */
#define BBPCOMM_INT_CLEAR_W_RCV_GM_ADDR               (SOC_BBP_COMM_BASE_ADDR + 0x6E0)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:0]   逻辑自清零。
             中断标志清除指示，高有效。用于清除INT_TYPE_W_RCV_GS[15:0]对应bit位
             生效时刻：立即生效
   UNION结构:  BBPCOMM_INT_CLEAR_W_RCV_GS_UNION */
#define BBPCOMM_INT_CLEAR_W_RCV_GS_ADDR               (SOC_BBP_COMM_BASE_ADDR + 0x6E4)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:0]   逻辑自清零。
             中断标志清除指示，高有效。用于清除INT_TYPE_W_RCV_W[15:0]对应bit位
             生效时刻：立即生效
   UNION结构:  BBPCOMM_INT_CLEAR_W_RCV_W_UNION */
#define BBPCOMM_INT_CLEAR_W_RCV_W_ADDR                (SOC_BBP_COMM_BASE_ADDR + 0x6E8)

/* 寄存器说明：寄存器
 bit[31:3]   保留
 bit[2:0]    逻辑自清零。
             中断标志清除指示，高有效。用于清除INT_TYPE_W_INT012[2:0]对应bit位
             生效时刻：立即生效
   UNION结构:  BBPCOMM_INT_CLEAR_W_INT012_UNION */
#define BBPCOMM_INT_CLEAR_W_INT012_ADDR               (SOC_BBP_COMM_BASE_ADDR + 0x6EC)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:0]   中断屏蔽标志。用于屏蔽INT_TYPE_W_RCV_LTE[15:0]对应bit中断。
             0：屏蔽中断上报；
             1：使能中断上报。
             配置时刻：初始化
             生效时刻：立即生效
   UNION结构:  BBPCOMM_INT_MASK_W_RCV_LTE_UNION */
#define BBPCOMM_INT_MASK_W_RCV_LTE_ADDR               (SOC_BBP_COMM_BASE_ADDR + 0x6F0)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:0]   中断屏蔽标志。用于屏蔽INT_TYPE_W_RCV_TDS[15:0]对应bit中断。
             0：屏蔽中断上报；
             1：使能中断上报。
             配置时刻：初始化
             生效时刻：立即生效
   UNION结构:  BBPCOMM_INT_MASK_W_RCV_TDS_UNION */
#define BBPCOMM_INT_MASK_W_RCV_TDS_ADDR               (SOC_BBP_COMM_BASE_ADDR + 0x6F4)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:0]   中断屏蔽标志。用于屏蔽INT_TYPE_W_RCV_GM[15:0]对应bit中断。
             0：屏蔽中断上报；
             1：使能中断上报。
             配置时刻：初始化
             生效时刻：立即生效
   UNION结构:  BBPCOMM_INT_MASK_W_RCV_GM_UNION */
#define BBPCOMM_INT_MASK_W_RCV_GM_ADDR                (SOC_BBP_COMM_BASE_ADDR + 0x6F8)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:0]   中断屏蔽标志。用于屏蔽INT_TYPE_W_RCV_GS[15:0]对应bit中断。
             0：屏蔽中断上报；
             1：使能中断上报。
             配置时刻：初始化
             生效时刻：立即生效
   UNION结构:  BBPCOMM_INT_MASK_W_RCV_GS_UNION */
#define BBPCOMM_INT_MASK_W_RCV_GS_ADDR                (SOC_BBP_COMM_BASE_ADDR + 0x6FC)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:0]   中断屏蔽标志。用于屏蔽INT_TYPE_W_RCV_W[15:0]对应bit中断。
             0：屏蔽中断上报；
             1：使能中断上报。
             配置时刻：初始化
             生效时刻：立即生效
   UNION结构:  BBPCOMM_INT_MASK_W_RCV_W_UNION */
#define BBPCOMM_INT_MASK_W_RCV_W_ADDR                 (SOC_BBP_COMM_BASE_ADDR + 0x700)

/* 寄存器说明：寄存器
 bit[31:3]   保留
 bit[2:0]    中断屏蔽标志。用于屏蔽INT_TYPE_W_INT012[2:0]对应bit中断。
             0：屏蔽中断上报；
             1：使能中断上报。
             配置时刻：初始化
             生效时刻：立即生效
   UNION结构:  BBPCOMM_INT_MASK_W_INT012_UNION */
#define BBPCOMM_INT_MASK_W_INT012_ADDR                (SOC_BBP_COMM_BASE_ADDR + 0x704)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:0]   中断状态标志。各bit单独对应一种中断类型，高有效。
             [0]: 中断类型0。 LTE（主模）提前通知W异系统测量启动中断。
             [1]: 中断类型1。 LTE（主模）进GAP中断，通知W进行异系统测量射频通道配置，天线切换等操作。
             [2]: 中断类型2。 LTE（主模）出GAP中断。
             [3]: 中断类型3。LTE（从模）测量结束中断。
             [4]: 中断类型4。LTE（主模）强制W退出异系统测量中断。
             [5]: 中断类型5。LTE（主模）通知W上报本次测量结果。
             [6]: 保留。
             [15:7]: 对应中断类型[15:7]，预留。
             软件读取该中断状态上报寄存器后，需要和0x06F0的int_mask_w_rcv_lte相与，再判断是否需要处理。
   UNION结构:  BBPCOMM_INT_TYPE_W_RCV_LTE_UNION */
#define BBPCOMM_INT_TYPE_W_RCV_LTE_ADDR               (SOC_BBP_COMM_BASE_ADDR + 0x708)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:0]   中断状态标志0，1，2只有当TDS处于主模时才能产生。
             
             [0]: 中断类型0。 TDS（主模）提前通知W异系统测量启动中断。
             [1]: 中断类型1。 TDS（主模）进GAP中断，通知W进行异系统测量射频通道配置，天线切换等操作。
             [2]: 中断类型2。 TDS（主模）出GAP中断。
             [3]: 中断类型3。TDS（从模）测量结束中断。
             [4]: 中断类型4。TDS（主模）强制W退出异系统测量中断。
             [5]: 中断类型5。TDS（主模）通知W上报本次测量结果。
             [6]: 中断类型6。保留。
             [15:7]: 对应中断类型[15:7]，预留。
             软件读取该中断状态上报寄存器后，需要和0x06F4的int_mask_w_rcv_tds相与，再判断是否需要处理。
   UNION结构:  BBPCOMM_INT_TYPE_W_RCV_TDS_UNION */
#define BBPCOMM_INT_TYPE_W_RCV_TDS_ADDR               (SOC_BBP_COMM_BASE_ADDR + 0x70C)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:0]   中断状态标志。各bit单独对应一种中断类型，高有效。
             [0]: 中断类型0。GM（主模）提前通知W异系统测量启动中断。
             [1]: 中断类型1。GM（主模）进GAP中断，通知W进行异系统测量射频通道配置，天线切换等操作。
             [2]: 中断类型2。GM（主模）出GAP中断。
             [3]: 中断类型3。GM（从模）测量结束中断。
             [4]: 中断类型4。GM（主模）强制W退出异系统测量中断。
             [5]: 中断类型5。GM（主模）通知W上报本次测量结果。
             [6]: 中断类型6。保留。
             [15:7]: 对应中断类型[15:7]，预留。
             中断状态标志0，1，2只有当GM处于主模时才能产生。
             软件读取该中断状态上报寄存器后，需要和0x06F8的int_mask_w_rcv_gm相与，再判断是否需要处理。
   UNION结构:  BBPCOMM_INT_TYPE_W_RCV_GM_UNION */
#define BBPCOMM_INT_TYPE_W_RCV_GM_ADDR                (SOC_BBP_COMM_BASE_ADDR + 0x710)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:0]   [0]: 中断类型0。GS（主模）提前通知W异系统测量启动中断。
             [1]: 中断类型1。GS（主模）进GAP中断，通知W进行异系统测量射频通道配置，天线切换等操作。
             [2]: 中断类型2。GS（主模）出GAP中断。
             [3]: 中断类型3。GS（从模）测量结束中断。
             [4]: 中断类型4。GS（主模）强制W退出异系统测量中断。
             [5]: 中断类型5。GS（主模）通知W上报本次测量结果。
             [6]: 中断类型6。保留。
             [15:7]: 对应中断类型[15:7]，预留。
             中断状态标志0，1，2只有当TDS处于主模时才能产生。
             软件读取该中断状态上报寄存器后，需要和0x06FC的int_mask_w_rcv_gs相与，再判断是否需要处理。
   UNION结构:  BBPCOMM_INT_TYPE_W_RCV_GS_UNION */
#define BBPCOMM_INT_TYPE_W_RCV_GS_ADDR                (SOC_BBP_COMM_BASE_ADDR + 0x714)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:0]   中断状态标志。各bit单独对应一种中断类型，高有效。
             [0]: 中断类型0。W（主模）提前通知W异频测量启动中断。
             [1]: 中断类型1。W（主模）进GAP中断，通知W进行异频测量射频通道配置，天线切换等操作。
             [2]: 中断类型2。W（主模）出GAP中断。
             [3]: 中断类型3。W（从模，即异频）测量结束中断。
             [4]: 中断类型4。W（主模）强制W退出异频测量中断。
             [5]: 中断类型5。W（主模）通知W上报本次测量结果。
             [6]: 中断类型6。保留。
             [15:7]: 对应中断类型[15:7]，预留。
             中断状态标志0，1，2只有当W处于主模时才能产生。
             软件读取该中断状态上报寄存器后，需要和0x0700的int_mask_w_rcv_w相与，再判断是否需要处理。
   UNION结构:  BBPCOMM_INT_TYPE_W_RCV_W_UNION */
#define BBPCOMM_INT_TYPE_W_RCV_W_ADDR                 (SOC_BBP_COMM_BASE_ADDR + 0x718)

/* 寄存器说明：寄存器
 bit[31:3]   保留
 bit[2:0]    增加主模产生int0、1、2的上报（(包括异频、异系统场景)），高有效。
             [0]: W主模时，CTU告诉通知WDSP产生int0中断。
             [1]: W主模时，CTU告诉通知WDSP产生int1中断。
             [2]: W主模时，CTU告诉通知WDSP产生int2中断。
             软件读取该中断状态上报寄存器后，需要和0x0704的int_mask_w_int012相与，再判断是否需要处理。
   UNION结构:  BBPCOMM_INT_TYPE_W_INT012_UNION */
#define BBPCOMM_INT_TYPE_W_INT012_ADDR                (SOC_BBP_COMM_BASE_ADDR + 0x71C)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:3]   逻辑自清零。高有效。
             
             bit15~7：对应w_snd_lte_int_15 ~ 7。
             W DSP通过此寄存器触发类型15~7中断（int_type_lte_rcv_w[15:7]）给LDSP。
             生效时刻：立即生效
             
             bit6：对应w_snd_lte_int_6。
             WDSP通过此寄存器触发类型6中断（int_type_lte_rcv_w[6]）给LDSP。
             生效时刻：立即生效
             
             bit5：对应w_snd_lte_int_5。
             W主模时通知LDSP L测量周期到达，高有效。
             W DSP通过此寄存器触发类型5中断（int_type_lte_rcv_w[5]）给LDSP。
             生效时刻：立即生效
             
             bit4：对应w_snd_lte_int_4。
             W主模时强制退出异系统测量指示。高有效。
             W DSP通过此寄存器触发类型4中断（int_type_lte_rcv_w[4]）给LDSP。
             生效时刻：立即生效
             
             bit3：对应w_snd_lte_int_3。W测量结束指示，高有效。
             用于触发中断类型3 （int_type_lte_rcv_w[3]）给LDSP。L主模下对W进行异系统测量时，如果W测量结束，W DSP配置此标志为1。 
             生效时刻：立即生效
 bit[2:0]    保留
   UNION结构:  BBPCOMM_W_SND_LTE_INT_13_UNION */
#define BBPCOMM_W_SND_LTE_INT_13_ADDR                 (SOC_BBP_COMM_BASE_ADDR + 0x720)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:3]   逻辑自清零。高有效。
             
             bit15~7：对应w_snd_tds_int_15 ~ 7。
             W DSP通过此寄存器触发类型15~7中断（int_type_tds_rcv_w[15:7]）给TDSP。
             生效时刻：立即生效
             
             bit6：对应w_snd_tds_int_6。
             WDSP通过此寄存器触发类型6中断（int_type_tds_rcv_w[6]）给TDSP。
             生效时刻：立即生效
             
             bit5：对应w_snd_tds_int_5。
             W主模时通知TDSP T测量周期到达，高有效。
             W DSP通过此寄存器触发类型5中断（int_type_tds_rcv_w[5]）给TDSP。
             生效时刻：立即生效
             
             bit4：对应w_snd_tds_int_4。
             W主模时强制退出异系统测量指示。高有效。
             W DSP通过此寄存器触发类型4中断（int_type_tds_rcv_w[4]）给TDSP。
             生效时刻：立即生效
             
             bit3：对应w_snd_tds_int_3。W测量结束指示，高有效。
             用于触发中断类型3 （int_type_tds_rcv_w[3]）给TDSP。T主模下对W进行异系统测量时，如果W测量结束，W DSP配置此标志为1。 
             生效时刻：立即生效
 bit[2:0]    保留
   UNION结构:  BBPCOMM_W_SND_TDS_INT_13_UNION */
#define BBPCOMM_W_SND_TDS_INT_13_ADDR                 (SOC_BBP_COMM_BASE_ADDR + 0x724)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:3]   逻辑自清零。高有效。
             
             bit15~7：对应w_snd_gm_int_15 ~ 7。
             W DSP通过此寄存器触发类型15~7中断（int_type_gm_rcv_w[15:7]）给GMDSP。
             生效时刻：立即生效
             
             bit6：对应w_snd_gm_int_6。
             WDSP通过此寄存器触发类型6中断（int_type_gm_rcv_w[6]）给GMDSP。
             生效时刻：立即生效
             
             bit5：对应w_snd_gm_int_5。
             W主模时通知GMDSP GM测量周期到达，高有效。
             W DSP通过此寄存器触发类型5中断（int_type_gm_rcv_w[5]）给GMDSP。
             生效时刻：立即生效
             
             bit4：对应w_snd_gm_int_4。
             W主模时强制退出异系统测量指示。高有效。
             W DSP通过此寄存器触发类型4中断（int_type_gm_rcv_w[4]）给GMDSP。
             生效时刻：立即生效
             
             bit3：对应w_snd_gm_int_3。W测量结束指示，高有效。
             用于触发中断类型3 （int_type_gm_rcv_w[3]）给GMDSP。GM主模下对W进行异系统测量时，如果W测量结束，W DSP配置此标志为1。 
             生效时刻：立即生效
 bit[2:0]    保留
   UNION结构:  BBPCOMM_W_SND_GM_INT_13_UNION */
#define BBPCOMM_W_SND_GM_INT_13_ADDR                  (SOC_BBP_COMM_BASE_ADDR + 0x728)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:3]   逻辑自清零。高有效。
             
             bit15~7：对应w_snd_gs_int_15 ~ 7。
             W DSP通过此寄存器触发类型15~7中断（int_type_gs_rcv_w[15:7]）给GSDSP。
             生效时刻：立即生效
             
             bit6：对应w_snd_gs_int_6。
             WDSP通过此寄存器触发类型6中断（int_type_gs_rcv_w[6]）给GSDSP。
             生效时刻：立即生效
             
             bit5：对应w_snd_gs_int_5。
             W主模时通知GSDSP GS测量周期到达，高有效。
             W DSP通过此寄存器触发类型5中断（int_type_gs_rcv_w[5]）给GSDSP。
             生效时刻：立即生效
             
             bit4：对应w_snd_gs_int_4。
             W主模时强制退出异系统测量指示。高有效。
             W DSP通过此寄存器触发类型4中断（int_type_gs_rcv_w[4]）给GSDSP。
             生效时刻：立即生效
             
             bit3：对应w_snd_gs_int_3。W测量结束指示，高有效。
             用于触发中断类型3 （int_type_gs_rcv_w[3]）给GSDSP。GS主模下对W进行异系统测量时，如果W测量结束，W DSP配置此标志为1。 
             生效时刻：立即生效
 bit[2:0]    保留
   UNION结构:  BBPCOMM_W_SND_GS_INT_13_UNION */
#define BBPCOMM_W_SND_GS_INT_13_ADDR                  (SOC_BBP_COMM_BASE_ADDR + 0x72C)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:3]   逻辑自清零。高有效。
             
             bit15~7：对应w_snd_w_int_15 ~ 7。
             W DSP通过此寄存器触发类型15~7中断（int_type_w_rcv_w[15:7]）给WDSP。
             生效时刻：立即生效
             
             bit6：对应w_snd_w_int_6。
             WDSP通过此寄存器触发类型6中断（int_type_w_rcv_w[6]）给WDSP。
             生效时刻：立即生效
             
             bit5：对应w_snd_w_int_5。
             W主模时通知WDSP W测量周期到达，高有效。
             W DSP通过此寄存器触发类型5中断（int_type_w_rcv_w[5]）给WDSP。
             生效时刻：立即生效
             
             bit4：对应w_snd_w_int_4。
             W主模时强制退出异系统测量指示。高有效。
             W DSP通过此寄存器触发类型4中断（int_type_w_rcv_w[4]）给WDSP。
             生效时刻：立即生效
             
             bit3：对应w_snd_w_int_3。W测量结束指示，高有效。
             用于触发中断类型3 （int_type_w_rcv_w[3]）给WDSP。W主模下对W进行异系统测量时，如果W测量结束，W DSP配置此标志为1。 
             生效时刻：立即生效
 bit[2:0]    保留
   UNION结构:  BBPCOMM_W_SND_W_INT_13_UNION */
#define BBPCOMM_W_SND_W_INT_13_ADDR                   (SOC_BBP_COMM_BASE_ADDR + 0x730)

/* 寄存器说明：寄存器
 bit[31:4]   保留
 bit[3:0]    异系统测量类型。
             4’b0：测量LTE。
             4’b1：背景搜LTE。
             其它预留
             生效时刻：立即生效
   UNION结构:  BBPCOMM_LTE_INTERSYS_MEASURE_TYPE_UNION */
#define BBPCOMM_LTE_INTERSYS_MEASURE_TYPE_ADDR        (SOC_BBP_COMM_BASE_ADDR + 0x734)

/* 寄存器说明：寄存器
 bit[31:1]   保留
 bit[0]      单频点测量周期到指示，W/Gm/Gs或者TDS作为主模，通知L DSP 测量的单频点周期到达。
             只在连接态下需要配置，且Gm测L时不需要配置；
             电平信号，高有效。
             
             生效时刻：立即生效
   UNION结构:  BBPCOMM_LTE_MEASURE_REPORT_VALID_UNION */
#define BBPCOMM_LTE_MEASURE_REPORT_VALID_ADDR         (SOC_BBP_COMM_BASE_ADDR + 0x738)

/* 寄存器说明：寄存器
   详      述：预处理中断int0到int1之间的offset信息，单位us（向下取整）。
            配置时刻：每次启动LTE从模异系统测量时配置
            生效时刻：立即生效
   UNION结构 ：无 */
#define BBPCOMM_LTE_PREINT_OFFSET_TIME_ADDR           (SOC_BBP_COMM_BASE_ADDR + 0x73C)

/* 寄存器说明：寄存器
   详      述：LTE作为从模时，主模主模分配给LTE用于异系统测量时间长度,告知LTE DSP。单位us，向下取整
            配置时刻：每次启动LTE从模异系统测量时配置
            生效时刻：立即生效
   UNION结构 ：无 */
#define BBPCOMM_LTE_INTERSYS_MEASURE_TIME_ADDR        (SOC_BBP_COMM_BASE_ADDR + 0x740)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:0]   逻辑自清零。
             中断标志清除指示，高有效。用于清除INT_TYPE_LTE_RCV_LTE[15:0]对应bit位
             生效时刻：立即生效
   UNION结构:  BBPCOMM_INT_CLEAR_LTE_RCV_LTE_UNION */
#define BBPCOMM_INT_CLEAR_LTE_RCV_LTE_ADDR            (SOC_BBP_COMM_BASE_ADDR + 0x744)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:0]   逻辑自清零。
             中断标志清除指示，高有效。用于清除INT_TYPE_LTE_RCV_TDS[15:0]对应bit位
             生效时刻：立即生效
   UNION结构:  BBPCOMM_INT_CLEAR_LTE_RCV_TDS_UNION */
#define BBPCOMM_INT_CLEAR_LTE_RCV_TDS_ADDR            (SOC_BBP_COMM_BASE_ADDR + 0x748)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:0]   逻辑自清零。
             中断标志清除指示，高有效。用于清除INT_TYPE_LTE_RCV_GM[15:0]对应bit位
             生效时刻：立即生效
   UNION结构:  BBPCOMM_INT_CLEAR_LTE_RCV_GM_UNION */
#define BBPCOMM_INT_CLEAR_LTE_RCV_GM_ADDR             (SOC_BBP_COMM_BASE_ADDR + 0x74C)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:0]   逻辑自清零。
             中断标志清除指示，高有效。用于清除INT_TYPE_LTE_RCV_GS[15:0]对应bit位
             生效时刻：立即生效
   UNION结构:  BBPCOMM_INT_CLEAR_LTE_RCV_GS_UNION */
#define BBPCOMM_INT_CLEAR_LTE_RCV_GS_ADDR             (SOC_BBP_COMM_BASE_ADDR + 0x750)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:0]   逻辑自清零。
             中断标志清除指示，高有效。用于清除INT_TYPE_LTE_RCV_W[15:0]对应bit位
             生效时刻：立即生效
   UNION结构:  BBPCOMM_INT_CLEAR_LTE_RCV_W_UNION */
#define BBPCOMM_INT_CLEAR_LTE_RCV_W_ADDR              (SOC_BBP_COMM_BASE_ADDR + 0x754)

/* 寄存器说明：寄存器
 bit[31:3]   保留
 bit[2:0]    逻辑自清零。
             中断标志清除指示，高有效。用于清除INT_TYPE_LTE_INT012[2:0]对应bit位
             生效时刻：立即生效
   UNION结构:  BBPCOMM_INT_CLEAR_LTE_INT012_UNION */
#define BBPCOMM_INT_CLEAR_LTE_INT012_ADDR             (SOC_BBP_COMM_BASE_ADDR + 0x758)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:0]   中断屏蔽标志。用于屏蔽INT_TYPE_LTE_RCV_LTE[15:0]对应bit中断。
             0：屏蔽中断上报；
             1：使能中断上报。
             配置时刻：初始化
             生效时刻：立即生效
   UNION结构:  BBPCOMM_INT_MASK_LTE_RCV_LTE_UNION */
#define BBPCOMM_INT_MASK_LTE_RCV_LTE_ADDR             (SOC_BBP_COMM_BASE_ADDR + 0x75C)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:0]   中断屏蔽标志。用于屏蔽INT_TYPE_LTE_RCV_TDS[15:0]对应bit中断。
             0：屏蔽中断上报；
             1：使能中断上报。
             配置时刻：初始化
             生效时刻：立即生效
   UNION结构:  BBPCOMM_INT_MASK_LTE_RCV_TDS_UNION */
#define BBPCOMM_INT_MASK_LTE_RCV_TDS_ADDR             (SOC_BBP_COMM_BASE_ADDR + 0x760)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:0]   中断屏蔽标志。用于屏蔽INT_TYPE_LTE_RCV_GM[15:0]对应bit中断。
             0：屏蔽中断上报；
             1：使能中断上报。
             配置时刻：初始化
             生效时刻：立即生效
   UNION结构:  BBPCOMM_INT_MASK_LTE_RCV_GM_UNION */
#define BBPCOMM_INT_MASK_LTE_RCV_GM_ADDR              (SOC_BBP_COMM_BASE_ADDR + 0x764)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:0]   中断屏蔽标志。用于屏蔽INT_TYPE_LTE_RCV_GS[15:0]对应bit中断。
             0：屏蔽中断上报；
             1：使能中断上报。
             配置时刻：初始化
             生效时刻：立即生效
   UNION结构:  BBPCOMM_INT_MASK_LTE_RCV_GS_UNION */
#define BBPCOMM_INT_MASK_LTE_RCV_GS_ADDR              (SOC_BBP_COMM_BASE_ADDR + 0x768)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:0]   中断屏蔽标志。用于屏蔽INT_TYPE_LTE_RCV_W[15:0]对应bit中断。
             0：屏蔽中断上报；
             1：使能中断上报。
             配置时刻：初始化
             生效时刻：立即生效
   UNION结构:  BBPCOMM_INT_MASK_LTE_RCV_W_UNION */
#define BBPCOMM_INT_MASK_LTE_RCV_W_ADDR               (SOC_BBP_COMM_BASE_ADDR + 0x76C)

/* 寄存器说明：寄存器
 bit[31:3]   保留
 bit[2:0]    中断屏蔽标志。用于屏蔽INT_TYPE_LTE_INT012[2:0]对应bit中断。
             0：屏蔽中断上报；
             1：使能中断上报。
             配置时刻：初始化
             生效时刻：立即生效
   UNION结构:  BBPCOMM_INT_MASK_LTE_INT012_UNION */
#define BBPCOMM_INT_MASK_LTE_INT012_ADDR              (SOC_BBP_COMM_BASE_ADDR + 0x770)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:0]   中断状态标志。各bit单独对应一种中断类型，高有效。
             [0]: 中断类型0。 W（主模）提前通知LTE异系统测量启动中断。
             [1]: 中断类型1。 W（主模）进GAP中断，通知LTE进行异系统测量射频通道配置，天线切换等操作。
             [2]: 中断类型2。 W（主模）出GAP中断。
             [3]: 中断类型3。W（从模）测量结束中断。
             [4]: 中断类型4。W（主模）强制LTE退出异系统测量中断。
             [5]: 中断类型5。W（主模）通知LTE上报本次测量结果。
             [6]: 保留。
             [15:7]: 对应中断类型[15:7]，预留。
             软件读取该中断状态上报寄存器后，需要和0x076C的int_mask_lte_rcv_w相与，再判断是否需要处理。
   UNION结构:  BBPCOMM_INT_TYPE_LTE_RCV_W_UNION */
#define BBPCOMM_INT_TYPE_LTE_RCV_W_ADDR               (SOC_BBP_COMM_BASE_ADDR + 0x774)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:0]   中断状态标志0，1，2只有当TDS处于主模时才能产生。
             [0]: 中断类型0。 TDS（主模）提前通知LTE异系统测量启动中断。
             [1]: 中断类型1。 TDS（主模）进GAP中断，通知LTE进行异系统测量射频通道配置，天线切换等操作。
             [2]: 中断类型2。 TDS（主模）出GAP中断。
             [3]: 中断类型3。TDS（从模）测量结束中断。
             [4]: 中断类型4。TDS（主模）强制LTE退出异系统测量中断。
             [5]: 中断类型5。TDS（主模）通知LTE上报本次测量结果。
             [6]: 中断类型6。保留。
             [15:7]: 对应中断类型[15:7]，预留。
             软件读取该中断状态上报寄存器后，需要和0x0760的int_mask_lte_rcv_tds相与，再判断是否需要处理。
   UNION结构:  BBPCOMM_INT_TYPE_LTE_RCV_TDS_UNION */
#define BBPCOMM_INT_TYPE_LTE_RCV_TDS_ADDR             (SOC_BBP_COMM_BASE_ADDR + 0x778)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:0]   中断状态标志。各bit单独对应一种中断类型，高有效。
             [0]: 中断类型0。GM（主模）提前通知LTE异系统测量启动中断。
             [1]: 中断类型1。GM（主模）进GAP中断，通知LTE进行异系统测量射频通道配置，天线切换等操作。
             [2]: 中断类型2。GM（主模）出GAP中断。
             [3]: 中断类型3。GM（从模）测量结束中断。
             [4]: 中断类型4。GM（主模）强制LTE退出异系统测量中断。
             [5]: 中断类型5。GM（主模）通知LTE上报本次测量结果。
             [6]: 中断类型6。保留。
             [15:7]: 对应中断类型[15:7]，预留。
             中断状态标志0，1，2只有当GM处于主模时才能产生。
             软件读取该中断状态上报寄存器后，需要和0x0764的int_mask_lte_rcv_gm相与，再判断是否需要处理。
   UNION结构:  BBPCOMM_INT_TYPE_LTE_RCV_GM_UNION */
#define BBPCOMM_INT_TYPE_LTE_RCV_GM_ADDR              (SOC_BBP_COMM_BASE_ADDR + 0x77C)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:0]   [0]: 中断类型0。GS（主模）提前通知LTE异系统测量启动中断。
             [1]: 中断类型1。GS（主模）进GAP中断，通知LTE进行异系统测量射频通道配置，天线切换等操作。
             [2]: 中断类型2。GS（主模）出GAP中断。
             [3]: 中断类型3。GS（从模）测量结束中断。
             [4]: 中断类型4。GS（主模）强制LTE退出异系统测量中断。
             [5]: 中断类型5。GS（主模）通知LTE上报本次测量结果。
             [6]: 中断类型6。保留。
             [15:7]: 对应中断类型[15:7]，预留。
             中断状态标志0，1，2只有当TDS处于主模时才能产生。
             软件读取该中断状态上报寄存器后，需要和0x0768的int_mask_lte_rcv_gs相与，再判断是否需要处理。
   UNION结构:  BBPCOMM_INT_TYPE_LTE_RCV_GS_UNION */
#define BBPCOMM_INT_TYPE_LTE_RCV_GS_ADDR              (SOC_BBP_COMM_BASE_ADDR + 0x780)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:0]   中断状态标志。各bit单独对应一种中断类型，高有效。
             [0]: 中断类型0。LTE（主模）提前通知LTE异频测量启动中断。
             [1]: 中断类型1。LTE（主模）进GAP中断，通知LTE进行异频测量射频通道配置，天线切换等操作。
             [2]: 中断类型2。LTE（主模）出GAP中断。
             [3]: 中断类型3。LTE（从模，即异频）测量结束中断。
             [4]: 中断类型4。LTE（主模）强制LTE退出异频测量中断。
             [5]: 中断类型5。LTE（主模）通知LTE上报本次测量结果。
             [6]: 中断类型6。保留。
             [15:7]: 对应中断类型[15:7]，预留。
             中断状态标志0，1，2只有当LTE处于主模时才能产生。
             软件读取该中断状态上报寄存器后，需要和0x075C的int_mask_lte_rcv_lte相与，再判断是否需要处理。
   UNION结构:  BBPCOMM_INT_TYPE_LTE_RCV_LTE_UNION */
#define BBPCOMM_INT_TYPE_LTE_RCV_LTE_ADDR             (SOC_BBP_COMM_BASE_ADDR + 0x784)

/* 寄存器说明：寄存器
 bit[31:3]   保留
 bit[2:0]    增加主模产生int0、1、2的上报（(包括异频、异系统场景)），高有效。
             [0]: L主模时，CTU告诉通知LDSP产生int0中断。
             [1]: L主模时，CTU告诉通知LDSP产生int1中断。
             [2]: L主模时，CTU告诉通知LDSP产生int2中断。
             软件读取该中断状态上报寄存器后，需要和0x0770的int_mask_lte_int012相与，再判断是否需要处理。
   UNION结构:  BBPCOMM_INT_TYPE_LTE_INT012_UNION */
#define BBPCOMM_INT_TYPE_LTE_INT012_ADDR              (SOC_BBP_COMM_BASE_ADDR + 0x788)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:3]   逻辑自清零。高有效。
             
             bit15~7：对应lte_snd_lte_int_15 ~ 7。
             L DSP通过此寄存器触发类型15~7中断（int_type_lte_rcv_lte[15:7]）给LDSP。
             生效时刻：立即生效
             
             bit6：对应lte_snd_lte_int_6。
             LDSP通过此寄存器触发类型6中断（int_type_lte_rcv_lte[6]）给LDSP。
             生效时刻：立即生效
             
             bit5：对应lte_snd_lte_int_5。
             L主模时通知LDSP L测量周期到达，高有效。
             L DSP通过此寄存器触发类型5中断（int_type_lte_rcv_lte[5]）给LDSP。
             生效时刻：立即生效
             
             bit4：对应lte_snd_lte_int_4。
             L主模时强制退出异系统测量指示。高有效。
             L DSP通过此寄存器触发类型4中断（int_type_lte_rcv_lte[4]）给LDSP。
             生效时刻：立即生效
             
             bit3：对应lte_snd_lte_int_3。L测量结束指示，高有效。
             用于触发中断类型3 （int_type_lte_rcv_lte[3]）给LDSP。L主模下对L进行异系统测量时，如果L测量结束，L DSP配置此标志为1。 
             生效时刻：立即生效
 bit[2:0]    保留
   UNION结构:  BBPCOMM_LTE_SND_LTE_INT_13_UNION */
#define BBPCOMM_LTE_SND_LTE_INT_13_ADDR               (SOC_BBP_COMM_BASE_ADDR + 0x78C)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:3]   逻辑自清零。高有效。
             
             bit15~7：对应lte_snd_tds_int_15 ~ 7。
             L DSP通过此寄存器触发类型15~7中断（int_type_tds_rcv_lte[15:7]）给TDSDSP。
             生效时刻：立即生效
             
             bit6：对应lte_snd_tds_int_6。
             LDSP通过此寄存器触发类型6中断（int_type_tds_rcv_lte[6]）给TDSDSP。
             生效时刻：立即生效
             
             bit5：对应lte_snd_tds_int_5。
             L主模时通知TDSDSP TDS测量周期到达，高有效。
             L DSP通过此寄存器触发类型5中断（int_type_tds_rcv_lte[5]）给TDSDSP。
             生效时刻：立即生效
             
             bit4：对应lte_snd_tds_int_4。
             L主模时强制退出异系统测量指示。高有效。
             L DSP通过此寄存器触发类型4中断（int_type_tds_rcv_lte[4]）给TDSDSP。
             生效时刻：立即生效
             
             bit3：对应lte_snd_tds_int_3。L测量结束指示，高有效。
             用于触发中断类型3 （int_type_tds_rcv_lte[3]）给TDSDSP。TDS主模下对L进行异系统测量时，如果L测量结束，L DSP配置此标志为1。 
             生效时刻：立即生效
 bit[2:0]    保留
   UNION结构:  BBPCOMM_LTE_SND_TDS_INT_13_UNION */
#define BBPCOMM_LTE_SND_TDS_INT_13_ADDR               (SOC_BBP_COMM_BASE_ADDR + 0x790)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:3]   逻辑自清零。高有效。
             
             bit15~7：对应lte_snd_gm_int_15 ~ 7。
             L DSP通过此寄存器触发类型15~7中断（int_type_gm_rcv_lte[15:7]）给GMDSP。
             生效时刻：立即生效
             
             bit6：对应lte_snd_gm_int_6。
             LDSP通过此寄存器触发类型6中断（iint_type_gm_rcv_lte[6]）给GMDSP。
             生效时刻：立即生效
             
             bit5：对应lte_snd_gm_int_5。
             L主模时通知GMDSP GM测量周期到达，高有效。
             L DSP通过此寄存器触发类型5中断（int_type_gm_rcv_lte[5]）给GMDSP。
             生效时刻：立即生效
             
             bit4：对应lte_snd_gm_int_4。
             L主模时强制退出异系统测量指示。高有效。
             L DSP通过此寄存器触发类型4中断（int_type_gm_rcv_lte[4]）给GMDSP。
             生效时刻：立即生效
             
             bit3：对应lte_snd_gm_int_3。L测量结束指示，高有效。
             用于触发中断类型3 （int_type_gm_rcv_lte[3]）给GMDSP。GM主模下对L进行异系统测量时，如果L测量结束，L DSP配置此标志为1。 
             生效时刻：立即生效
 bit[2:0]    保留
   UNION结构:  BBPCOMM_LTE_SND_GM_INT_13_UNION */
#define BBPCOMM_LTE_SND_GM_INT_13_ADDR                (SOC_BBP_COMM_BASE_ADDR + 0x794)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:3]   逻辑自清零。高有效。
             
             bit15~7：对应lte_snd_gs_int_15 ~ 7。
             L DSP通过此寄存器触发类型15~7中断（int_type_gs_rcv_lte[15:7]）给GSDSP。
             生效时刻：立即生效
             
             bit6：对应lte_snd_gs_int_6。
             LDSP通过此寄存器触发类型6中断（int_type_gs_rcv_lte[6]）给GSDSP。
             生效时刻：立即生效
             
             bit5：对应lte_snd_gs_int_5。
             L主模时通知GSDSP GS测量周期到达，高有效。
             L DSP通过此寄存器触发类型5中断（int_type_gs_rcv_lte[5]）给GSDSP。
             生效时刻：立即生效
             
             bit4：对应lte_snd_gs_int_4。
             L主模时强制退出异系统测量指示。高有效。
             L DSP通过此寄存器触发类型4中断（int_type_gs_rcv_lte[4]）给GSDSP。
             生效时刻：立即生效
             
             bit3：对应lte_snd_gs_int_3。L测量结束指示，高有效。
             用于触发中断类型3 （int_type_gs_rcv_lte[3]）给GSDSP。GS主模下对L进行异系统测量时，如果L测量结束，L DSP配置此标志为1。 
             生效时刻：立即生效
 bit[2:0]    保留
   UNION结构:  BBPCOMM_LTE_SND_GS_INT_13_UNION */
#define BBPCOMM_LTE_SND_GS_INT_13_ADDR                (SOC_BBP_COMM_BASE_ADDR + 0x798)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:3]   逻辑自清零。高有效。
             
             bit15~7：对应lte_snd_w_int_15 ~ 7。
             L DSP通过此寄存器触发类型15~7中断（int_type_w_rcv_lte[15:7]）给WDSP。
             生效时刻：立即生效
             
             bit6：对应lte_snd_w_int_6。
             LDSP通过此寄存器触发类型6中断（int_type_w_rcv_lte[6]）给WDSP。
             生效时刻：立即生效
             
             bit5：对应lte_snd_w_int_5。
             L主模时通知WDSP W测量周期到达，高有效。
             L DSP通过此寄存器触发类型5中断（int_type_w_rcv_lte[5]）给WDSP。
             生效时刻：立即生效
             
             bit4：对应lte_snd_w_int_4。
             L主模时强制退出异系统测量指示。高有效。
             L DSP通过此寄存器触发类型4中断（int_type_w_rcv_lte[4]）给WDSP。
             生效时刻：立即生效
             
             bit3：对应lte_snd_w_int_3。L测量结束指示，高有效。
             用于触发中断类型3 （int_type_w_rcv_lte[3]）给WDSP。W主模下对L进行异系统测量时，如果L测量结束，L DSP配置此标志为1。 
             生效时刻：立即生效
 bit[2:0]    保留
   UNION结构:  BBPCOMM_LTE_SND_W_INT_13_UNION */
#define BBPCOMM_LTE_SND_W_INT_13_ADDR                 (SOC_BBP_COMM_BASE_ADDR + 0x79C)

/* 寄存器说明：寄存器
 bit[31:4]   保留
 bit[3:0]    异系统测量类型 。
             4’b0： 测量TDS
             4’b1： 背景搜TDS
             其它预留
             
             生效时刻：立即生效
   UNION结构:  BBPCOMM_TDS_INTERSYS_MEASURE_TYPE_UNION */
#define BBPCOMM_TDS_INTERSYS_MEASURE_TYPE_ADDR        (SOC_BBP_COMM_BASE_ADDR + 0x7A0)

/* 寄存器说明：寄存器
 bit[31:1]   保留
 bit[0]      单频点测量周期到指示，U/Gm/Gs或者L作为主模，通知T DSP W测量的单频点周期到达。
             只在连接态下需要配置，且G测TDS时不需要配置；
             电平信号，高有效。
             
             生效时刻：立即生效
   UNION结构:  BBPCOMM_TDS_MEASURE_REPORT_VALID_UNION */
#define BBPCOMM_TDS_MEASURE_REPORT_VALID_ADDR         (SOC_BBP_COMM_BASE_ADDR + 0x7A4)

/* 寄存器说明：寄存器
   详      述：预处理中断int0到int1之间的offset信息，单位us（向下取整）。
            配置时刻：主模启动TDS从模异系统测量时配置
            生效时刻：立即生效
   UNION结构 ：无 */
#define BBPCOMM_TDS_PREINT_OFFSET_TIME_ADDR           (SOC_BBP_COMM_BASE_ADDR + 0x7A8)

/* 寄存器说明：寄存器
   详      述：测量时间长度，告知TDS DSP。单位us，向下取整到us。
            配置时刻：主模启动TDS从模异系统测量时配置
            生效时刻：立即生效
   UNION结构 ：无 */
#define BBPCOMM_TDS_INTERSYS_MEASURE_TIME_ADDR        (SOC_BBP_COMM_BASE_ADDR + 0x7AC)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:0]   逻辑自清零。
             中断标志清除指示，高有效。用于清除INT_TYPE_TDS_RCV_LTE[15:0]对应bit位
             生效时刻：立即生效
   UNION结构:  BBPCOMM_INT_CLEAR_TDS_RCV_LTE_UNION */
#define BBPCOMM_INT_CLEAR_TDS_RCV_LTE_ADDR            (SOC_BBP_COMM_BASE_ADDR + 0x7B0)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:0]   逻辑自清零。
             中断标志清除指示，高有效。用于清除INT_TYPE_TDS_RCV_TDS[15:0]对应bit位
             生效时刻：立即生效
   UNION结构:  BBPCOMM_INT_CLEAR_TDS_RCV_TDS_UNION */
#define BBPCOMM_INT_CLEAR_TDS_RCV_TDS_ADDR            (SOC_BBP_COMM_BASE_ADDR + 0x7B4)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:0]   逻辑自清零。
             中断标志清除指示，高有效。用于清除INT_TYPE_TDS_RCV_GM[15:0]对应bit位
             生效时刻：立即生效
   UNION结构:  BBPCOMM_INT_CLEAR_TDS_RCV_GM_UNION */
#define BBPCOMM_INT_CLEAR_TDS_RCV_GM_ADDR             (SOC_BBP_COMM_BASE_ADDR + 0x7B8)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:0]   逻辑自清零。
             中断标志清除指示，高有效。用于清除INT_TYPE_TDS_RCV_GS[15:0]对应bit位
             生效时刻：立即生效
   UNION结构:  BBPCOMM_INT_CLEAR_TDS_RCV_GS_UNION */
#define BBPCOMM_INT_CLEAR_TDS_RCV_GS_ADDR             (SOC_BBP_COMM_BASE_ADDR + 0x7BC)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:0]   逻辑自清零。
             中断标志清除指示，高有效。用于清除INT_TYPE_TDS_RCV_W[15:0]对应bit位
             生效时刻：立即生效
   UNION结构:  BBPCOMM_INT_CLEAR_TDS_RCV_W_UNION */
#define BBPCOMM_INT_CLEAR_TDS_RCV_W_ADDR              (SOC_BBP_COMM_BASE_ADDR + 0x7C0)

/* 寄存器说明：寄存器
 bit[31:3]   保留
 bit[2:0]    逻辑自清零。
             中断标志清除指示，高有效。用于清除INT_TYPE_TDS_INT012[2:0]对应bit位
             生效时刻：立即生效
   UNION结构:  BBPCOMM_INT_CLEAR_TDS_INT012_UNION */
#define BBPCOMM_INT_CLEAR_TDS_INT012_ADDR             (SOC_BBP_COMM_BASE_ADDR + 0x7C4)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:0]   中断屏蔽标志。用于屏蔽INT_TYPE_TDS_RCV_LTE[15:0]对应bit中断。
             0：屏蔽中断上报；
             1：使能中断上报。
             配置时刻：初始化
             生效时刻：立即生效
   UNION结构:  BBPCOMM_INT_MASK_TDS_RCV_LTE_UNION */
#define BBPCOMM_INT_MASK_TDS_RCV_LTE_ADDR             (SOC_BBP_COMM_BASE_ADDR + 0x7C8)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:0]   中断屏蔽标志。用于屏蔽INT_TYPE_TDS_RCV_TDS[15:0]对应bit中断。
             0：屏蔽中断上报；
             1：使能中断上报。
             配置时刻：初始化
             生效时刻：立即生效
   UNION结构:  BBPCOMM_INT_MASK_TDS_RCV_TDS_UNION */
#define BBPCOMM_INT_MASK_TDS_RCV_TDS_ADDR             (SOC_BBP_COMM_BASE_ADDR + 0x7CC)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:0]   中断屏蔽标志。用于屏蔽INT_TYPE_TDS_RCV_GM[15:0]对应bit中断。
             0：屏蔽中断上报；
             1：使能中断上报。
             配置时刻：初始化
             生效时刻：立即生效
   UNION结构:  BBPCOMM_INT_MASK_TDS_RCV_GM_UNION */
#define BBPCOMM_INT_MASK_TDS_RCV_GM_ADDR              (SOC_BBP_COMM_BASE_ADDR + 0x7D0)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:0]   中断屏蔽标志。用于屏蔽INT_TYPE_TDS_RCV_GS[15:0]对应bit中断。
             0：屏蔽中断上报；
             1：使能中断上报。
             配置时刻：初始化
             生效时刻：立即生效
   UNION结构:  BBPCOMM_INT_MASK_TDS_RCV_GS_UNION */
#define BBPCOMM_INT_MASK_TDS_RCV_GS_ADDR              (SOC_BBP_COMM_BASE_ADDR + 0x7D4)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:0]   中断屏蔽标志。用于屏蔽INT_TYPE_TDS_RCV_W[15:0]对应bit中断。
             0：屏蔽中断上报；
             1：使能中断上报。
             配置时刻：初始化
             生效时刻：立即生效
   UNION结构:  BBPCOMM_INT_MASK_TDS_RCV_W_UNION */
#define BBPCOMM_INT_MASK_TDS_RCV_W_ADDR               (SOC_BBP_COMM_BASE_ADDR + 0x7D8)

/* 寄存器说明：寄存器
 bit[31:3]   保留
 bit[2:0]    中断屏蔽标志。用于屏蔽INT_TYPE_TDS_INT012[2:0]对应bit中断。
             0：屏蔽中断上报；
             1：使能中断上报。
             配置时刻：初始化
             生效时刻：立即生效
   UNION结构:  BBPCOMM_INT_MASK_TDS_INT012_UNION */
#define BBPCOMM_INT_MASK_TDS_INT012_ADDR              (SOC_BBP_COMM_BASE_ADDR + 0x7DC)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:0]   中断状态标志。各bit单独对应一种中断类型，高有效。
             [0]: 中断类型0。 LTE（主模）提前通知TDS异系统测量启动中断。
             [1]: 中断类型1。 LTE（主模）进GAP中断，通知TDS进行异系统测量射频通道配置，天线切换等操作。
             [2]: 中断类型2。 LTE（主模）出GAP中断。
             [3]: 中断类型3。LTE（从模）测量结束中断。
             [4]: 中断类型4。LTE（主模）强制TDS退出异系统测量中断。
             [5]: 中断类型5。LTE（主模）通知TDS上报本次测量结果。
             [6]: 保留。
             [15:7]: 对应中断类型[15:7]，预留。
             软件读取该中断状态上报寄存器后，需要和0x07C8的int_mask_tds_rcv_lte相与，再判断是否需要处理。
   UNION结构:  BBPCOMM_INT_TYPE_TDS_RCV_LTE_UNION */
#define BBPCOMM_INT_TYPE_TDS_RCV_LTE_ADDR             (SOC_BBP_COMM_BASE_ADDR + 0x7E0)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:0]   中断状态标志0，1，2只有当TDS处于主模时才能产生。
             
             [0]: 中断类型0。 TDS（主模）提前通知TDS异系统测量启动中断。
             [1]: 中断类型1。 TDS（主模）进GAP中断，通知TDS进行异系统测量射频通道配置，天线切换等操作。
             [2]: 中断类型2。 TDS（主模）出GAP中断。
             [3]: 中断类型3。TDS（从模）测量结束中断。
             [4]: 中断类型4。TDS（主模）强制TDS退出异系统测量中断。
             [5]: 中断类型5。TDS（主模）通知TDS上报本次测量结果。
             [6]: 中断类型6。保留。
             [15:7]: 对应中断类型[15:7]，预留。
             软件读取该中断状态上报寄存器后，需要和0x07D8的int_mask_tds_rcv_w相与，再判断是否需要处理。
   UNION结构:  BBPCOMM_INT_TYPE_TDS_RCV_W_UNION */
#define BBPCOMM_INT_TYPE_TDS_RCV_W_ADDR               (SOC_BBP_COMM_BASE_ADDR + 0x7E4)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:0]   中断状态标志。各bit单独对应一种中断类型，高有效。
             [0]: 中断类型0。GM（主模）提前通知TDS异系统测量启动中断。
             [1]: 中断类型1。GM（主模）进GAP中断，通知TDS进行异系统测量射频通道配置，天线切换等操作。
             [2]: 中断类型2。GM（主模）出GAP中断。
             [3]: 中断类型3。GM（从模）测量结束中断。
             [4]: 中断类型4。GM（主模）强制TDS退出异系统测量中断。
             [5]: 中断类型5。GM（主模）通知TDS上报本次测量结果。
             [6]: 中断类型6。保留。
             [15:7]: 对应中断类型[15:7]，预留。
             中断状态标志0，1，2只有当GM处于主模时才能产生。
             软件读取该中断状态上报寄存器后，需要和0x07D0的int_mask_tds_rcv_gm相与，再判断是否需要处理。
   UNION结构:  BBPCOMM_INT_TYPE_TDS_RCV_GM_UNION */
#define BBPCOMM_INT_TYPE_TDS_RCV_GM_ADDR              (SOC_BBP_COMM_BASE_ADDR + 0x7E8)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:0]   [0]: 中断类型0。GS（主模）提前通知TDS异系统测量启动中断。
             [1]: 中断类型1。GS（主模）进GAP中断，通知TDS进行异系统测量射频通道配置，天线切换等操作。
             [2]: 中断类型2。GS（主模）出GAP中断。
             [3]: 中断类型3。GS（从模）测量结束中断。
             [4]: 中断类型4。GS（主模）强制TDS退出异系统测量中断。
             [5]: 中断类型5。GS（主模）通知TDS上报本次测量结果。
             [6]: 中断类型6。保留。
             [15:7]: 对应中断类型[15:7]，预留。
             中断状态标志0，1，2只有当TDS处于主模时才能产生。
             软件读取该中断状态上报寄存器后，需要和0x07D4的int_mask_tds_rcv_gs相与，再判断是否需要处理。
   UNION结构:  BBPCOMM_INT_TYPE_TDS_RCV_GS_UNION */
#define BBPCOMM_INT_TYPE_TDS_RCV_GS_ADDR              (SOC_BBP_COMM_BASE_ADDR + 0x7EC)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:0]   中断状态标志。各bit单独对应一种中断类型，高有效。
             [0]: 中断类型0。TDS（主模）提前通知TDS异频测量启动中断。
             [1]: 中断类型1。TDS（主模）进GAP中断，通知TDS进行异频测量射频通道配置，天线切换等操作。
             [2]: 中断类型2。TDS（主模）出GAP中断。
             [3]: 中断类型3。TDS（从模，即异频）测量结束中断。
             [4]: 中断类型4。TDS（主模）强制TDS退出异频测量中断。
             [5]: 中断类型5。TDS（主模）通知TDS上报本次测量结果。
             [6]: 中断类型6。保留。
             [15:7]: 对应中断类型[15:7]，预留。
             中断状态标志0，1，2只有当TDS处于主模时才能产生。
             软件读取该中断状态上报寄存器后，需要和0x07CC的int_mask_tds_rcv_tds相与，再判断是否需要处理。
   UNION结构:  BBPCOMM_INT_TYPE_TDS_RCV_TDS_UNION */
#define BBPCOMM_INT_TYPE_TDS_RCV_TDS_ADDR             (SOC_BBP_COMM_BASE_ADDR + 0x7F0)

/* 寄存器说明：寄存器
 bit[31:3]   保留
 bit[2:0]    增加主模产生int0、1、2的上报（(包括异频、异系统场景)），高有效。
             [0]: T主模时，CTU告诉通知TDSP产生int0中断。
             [1]: T主模时，CTU告诉通知TDSP产生int1中断。
             [2]: T主模时，CTU告诉通知TDSP产生int2中断。
             软件读取该中断状态上报寄存器后，需要和0x07DC的int_mask_tds_int012相与，再判断是否需要处理。
   UNION结构:  BBPCOMM_INT_TYPE_TDS_INT012_UNION */
#define BBPCOMM_INT_TYPE_TDS_INT012_ADDR              (SOC_BBP_COMM_BASE_ADDR + 0x7F4)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:3]   逻辑自清零。高有效。
             
             bit15~7：对应tds_snd_lte_int_15 ~ 7。
             TDS DSP通过此寄存器触发类型15~7中断（int_type_lte_rcv_tds[15:7]）给LDSP。
             生效时刻：立即生效
             
             bit6：对应tds_snd_lte_int_6。
             TDSDSP通过此寄存器触发类型6中断（int_type_lte_rcv_tds[6]）给LDSP。
             生效时刻：立即生效
             
             bit5：对应tds_snd_lte_int_5。
             TDS主模时通知LDSP L测量周期到达，高有效。
             TDS DSP通过此寄存器触发类型5中断（int_type_lte_rcv_tds[5]）给LDSP。
             生效时刻：立即生效
             
             bit4：对应tds_snd_lte_int_4。
             TDS主模时强制退出异系统测量指示。高有效。
             TDS DSP通过此寄存器触发类型4中断（int_type_lte_rcv_tds[4]）给LDSP。
             生效时刻：立即生效
             
             bit3：对应tds_snd_lte_int_3。TDS测量结束指示，高有效。
             用于触发中断类型3 （int_type_lte_rcv_tds[3]）给LDSP。L主模下对TDS进行异系统测量时，如果TDS测量结束，TDS DSP配置此标志为1。 
             生效时刻：立即生效
 bit[2:0]    保留
   UNION结构:  BBPCOMM_TDS_SND_LTE_INT_13_UNION */
#define BBPCOMM_TDS_SND_LTE_INT_13_ADDR               (SOC_BBP_COMM_BASE_ADDR + 0x7F8)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:3]   逻辑自清零。高有效。
             
             bit15~7：对应tds_snd_tds_int_15 ~ 7。
             TDS DSP通过此寄存器触发类型15~7中断（int_type_tds_rcv_tds[15:7]）给TDSP。
             生效时刻：立即生效
             
             bit6：对应tds_snd_tds_int_6。
             TDSDSP通过此寄存器触发类型6中断（int_type_tds_rcv_tds[6]）给TDSP。
             生效时刻：立即生效
             
             bit5：对应tds_snd_tds_int_5。
             TDS主模时通知TDSP T测量周期到达，高有效。
             TDS DSP通过此寄存器触发类型5中断（int_type_tds_rcv_tds[5]）给TDSP。
             生效时刻：立即生效
             
             bit4：对应tds_snd_tds_int_4。
             TDS主模时强制退出异系统测量指示。高有效。
             TDS DSP通过此寄存器触发类型4中断（int_type_tds_rcv_tds[4]）给TDSP。
             生效时刻：立即生效
             
             bit3：对应tds_snd_tds_int_3。TDS测量结束指示，高有效。
             用于触发中断类型3 （int_type_tds_rcv_tds[3]）给TDSP。T主模下对TDS进行异系统测量时，如果TDS测量结束，TDS DSP配置此标志为1。 
             生效时刻：立即生效
 bit[2:0]    保留
   UNION结构:  BBPCOMM_TDS_SND_TDS_INT_13_UNION */
#define BBPCOMM_TDS_SND_TDS_INT_13_ADDR               (SOC_BBP_COMM_BASE_ADDR + 0x7FC)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:3]   逻辑自清零。高有效。
             
             bit15~7：对应tds_snd_gm_int_15 ~ 7。
             TDS DSP通过此寄存器触发类型15~7中断（int_type_gm_rcv_tds[15:7]）给GMDSP。
             生效时刻：立即生效
             
             bit6：对应tds_snd_gm_int_6。
             TDSDSP通过此寄存器触发类型6中断（int_type_gm_rcv_tds[6]）给GMDSP。
             生效时刻：立即生效
             
             bit5：对应tds_snd_gm_int_5。
             TDS主模时通知GMDSP GM测量周期到达，高有效。
             TDS DSP通过此寄存器触发类型5中断（int_type_gm_rcv_tds[5]）给GMDSP。
             生效时刻：立即生效
             
             bit4：对应tds_snd_gm_int_4。
             TDS主模时强制退出异系统测量指示。高有效。
             TDS DSP通过此寄存器触发类型4中断（int_type_gm_rcv_tds[4]）给GMDSP。
             生效时刻：立即生效
             
             bit3：对应tds_snd_gm_int_3。TDS测量结束指示，高有效。
             用于触发中断类型3 （int_type_gm_rcv_tds[3]）给GMDSP。GM主模下对TDS进行异系统测量时，如果TDS测量结束，TDS DSP配置此标志为1。 
             生效时刻：立即生效
 bit[2:0]    保留
   UNION结构:  BBPCOMM_TDS_SND_GM_INT_13_UNION */
#define BBPCOMM_TDS_SND_GM_INT_13_ADDR                (SOC_BBP_COMM_BASE_ADDR + 0x800)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:3]   逻辑自清零。高有效。
             
             bit15~7：对应tds_snd_gs_int_15 ~ 7。
             TDS DSP通过此寄存器触发类型15~7中断（int_type_gs_rcv_tds[15:7]）给GSDSP。
             生效时刻：立即生效
             
             bit6：对应tds_snd_gs_int_6。
             TDSDSP通过此寄存器触发类型6中断（int_type_gs_rcv_tds[6]）给GSDSP。
             生效时刻：立即生效
             
             bit5：对应tds_snd_gs_int_5。
             TDS主模时通知GSDSP GS测量周期到达，高有效。
             TDS DSP通过此寄存器触发类型5中断（int_type_gs_rcv_tds[5]）给GSDSP。
             生效时刻：立即生效
             
             bit4：对应tds_snd_gs_int_4。
             TDS主模时强制退出异系统测量指示。高有效。
             TDS DSP通过此寄存器触发类型4中断（int_type_gs_rcv_tds[4]）给GSDSP。
             生效时刻：立即生效
             
             bit3：对应tds_snd_gs_int_3。TDS测量结束指示，高有效。
             用于触发中断类型3 （int_type_gs_rcv_tds[3]）给GSDSP。GS主模下对TDS进行异系统测量时，如果TDS测量结束，TDS DSP配置此标志为1。 
             生效时刻：立即生效
 bit[2:0]    保留
   UNION结构:  BBPCOMM_TDS_SND_GS_INT_13_UNION */
#define BBPCOMM_TDS_SND_GS_INT_13_ADDR                (SOC_BBP_COMM_BASE_ADDR + 0x804)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:3]   逻辑自清零。高有效。
             
             bit15~7：对应tds_snd_w_int_15 ~ 7。
             TDS DSP通过此寄存器触发类型15~7中断（int_type_w_rcv_tds[15:7]）给WDSP。
             生效时刻：立即生效
             
             bit6：对应tds_snd_w_int_6。
             TDSDSP通过此寄存器触发类型6中断（int_type_w_rcv_tds[6]）给WDSP。
             生效时刻：立即生效
             
             bit5：对应tds_snd_w_int_5。
             TDS主模时通知WDSP W测量周期到达，高有效。
             TDS DSP通过此寄存器触发类型5中断（int_type_w_rcv_tds[5]）给WDSP。
             生效时刻：立即生效
             
             bit4：对应tds_snd_w_int_4。
             TDS主模时强制退出异系统测量指示。高有效。
             TDS DSP通过此寄存器触发类型4中断（int_type_w_rcv_tds[4]）给WDSP。
             生效时刻：立即生效
             
             bit3：对应tds_snd_w_int_3。TDS测量结束指示，高有效。
             用于触发中断类型3 （int_type_w_rcv_tds[3]）给WDSP。W主模下对TDS进行异系统测量时，如果TDS测量结束，TDS DSP配置此标志为1。 
             生效时刻：立即生效
 bit[2:0]    保留
   UNION结构:  BBPCOMM_TDS_SND_W_INT_13_UNION */
#define BBPCOMM_TDS_SND_W_INT_13_ADDR                 (SOC_BBP_COMM_BASE_ADDR + 0x808)

/* 寄存器说明：寄存器
 bit[31:4]   保留
 bit[3:0]    异系统测量类型。
             4’d0：测量GM RSSI。
             4’d1: 测量GM bsic verification。
             4’d2: 测量GM bsic confirm。
             4’d3: 背景搜GM；
             4’d4: 2G ANR。
             其它预留
             生效时刻：立即生效
   UNION结构:  BBPCOMM_GM_INTERSYS_MEASURE_TYPE_UNION */
#define BBPCOMM_GM_INTERSYS_MEASURE_TYPE_ADDR         (SOC_BBP_COMM_BASE_ADDR + 0x80C)

/* 寄存器说明：寄存器
 bit[31:1]   保留
 bit[0]      单频点测量周期到指示，L或者TDS/W/Gs作为主模，通知GM DSP GM测量的单频点周期到达。
             只在连接态下需要配置。
             电平信号，高有效。
             生效时刻：立即生效
   UNION结构:  BBPCOMM_GM_MEASURE_REPORT_VALID_UNION */
#define BBPCOMM_GM_MEASURE_REPORT_VALID_ADDR          (SOC_BBP_COMM_BASE_ADDR + 0x810)

/* 寄存器说明：寄存器
   详      述：预处理中断int0到int1之间的offset信息，单位us（向下取整）。。
            配置时刻：主模启动GM从模异系统测量时配置
            生效时刻：立即生效
   UNION结构 ：无 */
#define BBPCOMM_GM_PREINT_OFFSET_TIME_ADDR            (SOC_BBP_COMM_BASE_ADDR + 0x814)

/* 寄存器说明：寄存器
   详      述：GM作为从模时，主模分配给GM用于异系统测量时间长度，告知GMDSP。单位us，向下取整到us。
            配置时刻：主模启动GM从模异系统测量时配置
            生效时刻：立即生效
   UNION结构 ：无 */
#define BBPCOMM_GM_INTERSYS_MEASURE_TIME_ADDR         (SOC_BBP_COMM_BASE_ADDR + 0x818)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:0]   逻辑自清零。
             中断标志清除指示，高有效。用于清除INT_TYPE_GM_RCV_LTE[15:0]对应bit位
             生效时刻：立即生效
   UNION结构:  BBPCOMM_INT_CLEAR_GM_RCV_LTE_UNION */
#define BBPCOMM_INT_CLEAR_GM_RCV_LTE_ADDR             (SOC_BBP_COMM_BASE_ADDR + 0x81C)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:0]   逻辑自清零。
             中断标志清除指示，高有效。用于清除INT_TYPE_GM_RCV_TDS[15:0]对应bit位
             生效时刻：立即生效
   UNION结构:  BBPCOMM_INT_CLEAR_GM_RCV_TDS_UNION */
#define BBPCOMM_INT_CLEAR_GM_RCV_TDS_ADDR             (SOC_BBP_COMM_BASE_ADDR + 0x820)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:0]   逻辑自清零。
             中断标志清除指示，高有效。用于清除INT_TYPE_GM_RCV_GM[15:0]对应bit位
             生效时刻：立即生效
   UNION结构:  BBPCOMM_INT_CLEAR_GM_RCV_GM_UNION */
#define BBPCOMM_INT_CLEAR_GM_RCV_GM_ADDR              (SOC_BBP_COMM_BASE_ADDR + 0x824)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:0]   逻辑自清零。
             中断标志清除指示，高有效。用于清除INT_TYPE_GM_RCV_GS[15:0]对应bit位
             生效时刻：立即生效
   UNION结构:  BBPCOMM_INT_CLEAR_GM_RCV_GS_UNION */
#define BBPCOMM_INT_CLEAR_GM_RCV_GS_ADDR              (SOC_BBP_COMM_BASE_ADDR + 0x828)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:0]   逻辑自清零。
             中断标志清除指示，高有效。用于清除INT_TYPE_GM_RCV_W[15:0]对应bit位
             生效时刻：立即生效
   UNION结构:  BBPCOMM_INT_CLEAR_GM_RCV_W_UNION */
#define BBPCOMM_INT_CLEAR_GM_RCV_W_ADDR               (SOC_BBP_COMM_BASE_ADDR + 0x82C)

/* 寄存器说明：寄存器
 bit[31:3]   保留
 bit[2:0]    逻辑自清零。
             中断标志清除指示，高有效。用于清除INT_TYPE_GM_INT012[2:0]对应bit位
             生效时刻：立即生效
   UNION结构:  BBPCOMM_INT_CLEAR_GM_INT012_UNION */
#define BBPCOMM_INT_CLEAR_GM_INT012_ADDR              (SOC_BBP_COMM_BASE_ADDR + 0x830)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:0]   中断屏蔽标志。用于屏蔽INT_TYPE_GM_RCV_LTE[15:0]对应bit中断。
             0：屏蔽中断上报；
             1：使能中断上报。
             配置时刻：初始化
             生效时刻：立即生效
   UNION结构:  BBPCOMM_INT_MASK_GM_RCV_LTE_UNION */
#define BBPCOMM_INT_MASK_GM_RCV_LTE_ADDR              (SOC_BBP_COMM_BASE_ADDR + 0x834)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:0]   中断屏蔽标志。用于屏蔽INT_TYPE_GM_RCV_TDS[15:0]对应bit中断。
             0：屏蔽中断上报；
             1：使能中断上报。
             配置时刻：初始化
             生效时刻：立即生效
   UNION结构:  BBPCOMM_INT_MASK_GM_RCV_TDS_UNION */
#define BBPCOMM_INT_MASK_GM_RCV_TDS_ADDR              (SOC_BBP_COMM_BASE_ADDR + 0x838)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:0]   中断屏蔽标志。用于屏蔽INT_TYPE_GM_RCV_GM[15:0]对应bit中断。
             0：屏蔽中断上报；
             1：使能中断上报。
             配置时刻：初始化
             生效时刻：立即生效
   UNION结构:  BBPCOMM_INT_MASK_GM_RCV_GM_UNION */
#define BBPCOMM_INT_MASK_GM_RCV_GM_ADDR               (SOC_BBP_COMM_BASE_ADDR + 0x83C)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:0]   中断屏蔽标志。用于屏蔽INT_TYPE_GM_RCV_GS[15:0]对应bit中断。
             0：屏蔽中断上报；
             1：使能中断上报。
             配置时刻：初始化
             生效时刻：立即生效
   UNION结构:  BBPCOMM_INT_MASK_GM_RCV_GS_UNION */
#define BBPCOMM_INT_MASK_GM_RCV_GS_ADDR               (SOC_BBP_COMM_BASE_ADDR + 0x840)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:0]   中断屏蔽标志。用于屏蔽INT_TYPE_GM_RCV_W[15:0]对应bit中断。
             0：屏蔽中断上报；
             1：使能中断上报。
             配置时刻：初始化
             生效时刻：立即生效
   UNION结构:  BBPCOMM_INT_MASK_GM_RCV_W_UNION */
#define BBPCOMM_INT_MASK_GM_RCV_W_ADDR                (SOC_BBP_COMM_BASE_ADDR + 0x844)

/* 寄存器说明：寄存器
 bit[31:3]   保留
 bit[2:0]    中断屏蔽标志。用于屏蔽INT_TYPE_GM_INT012[2:0]对应bit中断。
             0：屏蔽中断上报；
             1：使能中断上报。
             配置时刻：初始化
             生效时刻：立即生效
   UNION结构:  BBPCOMM_INT_MASK_GM_INT012_UNION */
#define BBPCOMM_INT_MASK_GM_INT012_ADDR               (SOC_BBP_COMM_BASE_ADDR + 0x848)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:0]   中断状态标志。各bit单独对应一种中断类型，高有效。
             [0]: 中断类型0。 LTE（主模）提前通知GM异系统测量启动中断。
             [1]: 中断类型1。 LTE（主模）进GAP中断，通知GM进行异系统测量射频通道配置，天线切换等操作。
             [2]: 中断类型2。 LTE（主模）出GAP中断。
             [3]: 中断类型3。LTE（从模）测量结束中断。
             [4]: 中断类型4。LTE（主模）强制GM退出异系统测量中断。
             [5]: 中断类型5。LTE（主模）通知GM上报本次测量结果。
             [6]: 保留。
             [15:7]: 对应中断类型[15:7]，预留。
             软件读取该中断状态上报寄存器后，需要和0x0834的int_mask_gm_rcv_lte相与，再判断是否需要处理。
   UNION结构:  BBPCOMM_INT_TYPE_GM_RCV_LTE_UNION */
#define BBPCOMM_INT_TYPE_GM_RCV_LTE_ADDR              (SOC_BBP_COMM_BASE_ADDR + 0x84C)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:0]   中断状态标志0，1，2只有当TDS处于主模时才能产生。
             
             [0]: 中断类型0。 TDS（主模）提前通知GM异系统测量启动中断。
             [1]: 中断类型1。 TDS（主模）进GAP中断，通知GM进行异系统测量射频通道配置，天线切换等操作。
             [2]: 中断类型2。 TDS（主模）出GAP中断。
             [3]: 中断类型3。TDS（从模）测量结束中断。
             [4]: 中断类型4。TDS（主模）强制GM退出异系统测量中断。
             [5]: 中断类型5。TDS（主模）通知GM上报本次测量结果。
             [6]: 中断类型6。保留。
             [15:7]: 对应中断类型[15:7]，预留。
             软件读取该中断状态上报寄存器后，需要和0x0838的int_mask_gm_rcv_tds相与，再判断是否需要处理。
   UNION结构:  BBPCOMM_INT_TYPE_GM_RCV_TDS_UNION */
#define BBPCOMM_INT_TYPE_GM_RCV_TDS_ADDR              (SOC_BBP_COMM_BASE_ADDR + 0x850)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:0]   中断状态标志。各bit单独对应一种中断类型，高有效。
             [0]: 中断类型0。W（主模）提前通知GM异系统测量启动中断。
             [1]: 中断类型1。W（主模）进GAP中断，通知GM进行异系统测量射频通道配置，天线切换等操作。
             [2]: 中断类型2。W（主模）出GAP中断。
             [3]: 中断类型3。W（从模）测量结束中断。
             [4]: 中断类型4。W（主模）强制GM退出异系统测量中断。
             [5]: 中断类型5。W（主模）通知GM上报本次测量结果。
             [6]: 中断类型6。保留。
             [15:7]: 对应中断类型[15:7]，预留。
             中断状态标志0，1，2只有当W处于主模时才能产生。
             软件读取该中断状态上报寄存器后，需要和0x0844的int_mask_gm_rcv_w相与，再判断是否需要处理。
   UNION结构:  BBPCOMM_INT_TYPE_GM_RCV_W_UNION */
#define BBPCOMM_INT_TYPE_GM_RCV_W_ADDR                (SOC_BBP_COMM_BASE_ADDR + 0x854)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:0]   [0]: 中断类型0。GS（主模）提前通知GM异系统测量启动中断。
             [1]: 中断类型1。GS（主模）进GAP中断，通知GM进行异系统测量射频通道配置，天线切换等操作。
             [2]: 中断类型2。GS（主模）出GAP中断。
             [3]: 中断类型3。GS（从模）测量结束中断。
             [4]: 中断类型4。GS（主模）强制GM退出异系统测量中断。
             [5]: 中断类型5。GS（主模）通知GM上报本次测量结果。
             [6]: 中断类型6。保留。
             [15:7]: 对应中断类型[15:7]，预留。
             中断状态标志0，1，2只有当TDS处于主模时才能产生。
             软件读取该中断状态上报寄存器后，需要和0x0840的int_mask_gm_rcv_gs相与，再判断是否需要处理。
   UNION结构:  BBPCOMM_INT_TYPE_GM_RCV_GS_UNION */
#define BBPCOMM_INT_TYPE_GM_RCV_GS_ADDR               (SOC_BBP_COMM_BASE_ADDR + 0x858)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:0]   中断状态标志。各bit单独对应一种中断类型，高有效。
             [0]: 中断类型0。GM（主模）提前通知GM异频测量启动中断。
             [1]: 中断类型1。GM（主模）进GAP中断，通知GM进行异频测量射频通道配置，天线切换等操作。
             [2]: 中断类型2。GM（主模）出GAP中断。
             [3]: 中断类型3。GM（从模，即异频）测量结束中断。
             [4]: 中断类型4。GM（主模）强制GM退出异频测量中断。
             [5]: 中断类型5。GM（主模）通知GM上报本次测量结果。
             [6]: 中断类型6。保留。
             [15:7]: 对应中断类型[15:7]，预留。
             中断状态标志0，1，2只有当GM处于主模时才能产生。
             软件读取该中断状态上报寄存器后，需要和0x083C的int_mask_gm_rcv_gm相与，再判断是否需要处理。
   UNION结构:  BBPCOMM_INT_TYPE_GM_RCV_GM_UNION */
#define BBPCOMM_INT_TYPE_GM_RCV_GM_ADDR               (SOC_BBP_COMM_BASE_ADDR + 0x85C)

/* 寄存器说明：寄存器
 bit[31:3]   保留
 bit[2:0]    增加主模产生int0、1、2的上报（(包括异频、异系统场景)），高有效。
             [0]: GM主模时，CTU告诉通知GMDSP产生int0中断。
             [1]: GM主模时，CTU告诉通知GMDSP产生int1中断。
             [2]: GM主模时，CTU告诉通知GMDSP产生int2中断。
             软件读取该中断状态上报寄存器后，需要和0x0848的int_mask_gm_int012相与，再判断是否需要处理。
   UNION结构:  BBPCOMM_INT_TYPE_GM_INT012_UNION */
#define BBPCOMM_INT_TYPE_GM_INT012_ADDR               (SOC_BBP_COMM_BASE_ADDR + 0x860)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:3]   逻辑自清零。高有效。
             
             bit15~7：对应gm_snd_lte_int_15 ~ 7。
             GM DSP通过此寄存器触发类型15~7中断（int_type_lte_rcv_gm[15:7]）给LTEDSP。
             生效时刻：立即生效
             
             bit6：对应gm_snd_lte_int_6。
             GMDSP通过此寄存器触发类型6中断（int_type_lte_rcv_gm[6]）给LTEDSP。
             生效时刻：立即生效
             
             bit5：对应gm_snd_lte_int_5。
             GM主模时通知LTEDSP LTE测量周期到达，高有效。
             GM DSP通过此寄存器触发类型5中断（int_type_lte_rcv_gm[5]）给LTEDSP。
             生效时刻：立即生效
             
             bit4：对应gm_snd_lte_int_4。
             GM主模时强制退出异系统测量指示。高有效。
             GM DSP通过此寄存器触发类型4中断（int_type_lte_rcv_gm[4]）给LTEDSP。
             生效时刻：立即生效
             
             bit3：对应gm_snd_lte_int_3。GM测量结束指示，高有效。
             用于触发中断类型3 （int_type_lte_rcv_gm[3]）给LTEDSP。LTE主模下对GM进行异系统测量时，如果GM测量结束，GM DSP配置此标志为1。 
             生效时刻：立即生效
 bit[2:0]    保留
   UNION结构:  BBPCOMM_GM_SND_LTE_INT_13_UNION */
#define BBPCOMM_GM_SND_LTE_INT_13_ADDR                (SOC_BBP_COMM_BASE_ADDR + 0x864)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:3]   逻辑自清零。高有效。
             
             bit15~7：对应gm_snd_tds_int_15 ~ 7。
             GM DSP通过此寄存器触发类型15~7中断（int_type_tds_rcv_gm[15:7]）给TDSDSP。
             生效时刻：立即生效
             
             bit6：对应gm_snd_tds_int_6。
             GMDSP通过此寄存器触发类型6中断（int_type_tds_rcv_gm[6]）给TDSDSP。
             生效时刻：立即生效
             
             bit5：对应gm_snd_tds_int_5。
             GM主模时通知TDSDSP tds测量周期到达，高有效。
             GM DSP通过此寄存器触发类型5中断（int_type_tds_rcv_gm[5]）给TDSDSP。
             生效时刻：立即生效
             
             bit4：对应gm_snd_tds_int_4。
             GM主模时强制退出异系统测量指示。高有效。
             GM DSP通过此寄存器触发类型4中断（int_type_tds_rcv_gm[4]）给TDSDSP。
             生效时刻：立即生效
             
             bit3：对应gm_snd_tds_int_3。GM测量结束指示，高有效。
             用于触发中断类型3 （int_type_tds_rcv_gm[3]）给TDSDSP。tds主模下对GM进行异系统测量时，如果GM测量结束，GM DSP配置此标志为1。 
             生效时刻：立即生效
 bit[2:0]    保留
   UNION结构:  BBPCOMM_GM_SND_TDS_INT_13_UNION */
#define BBPCOMM_GM_SND_TDS_INT_13_ADDR                (SOC_BBP_COMM_BASE_ADDR + 0x868)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:3]   逻辑自清零。高有效。
             
             bit15~7：对应gm_snd_gm_int_15 ~ 7。
             GM DSP通过此寄存器触发类型15~7中断（int_type_gm_rcv_gm[15:7]）给GMDSP。
             生效时刻：立即生效
             
             bit6：对应gm_snd_gm_int_6。
             GMDSP通过此寄存器触发类型6中断（int_type_gm_rcv_gm[6]）给GMDSP。
             生效时刻：立即生效
             
             bit5：对应gm_snd_gm_int_5。
             GM主模时通知GMDSP GM测量周期到达，高有效。
             GM DSP通过此寄存器触发类型5中断（int_type_gm_rcv_gm[5]）给GMDSP。
             生效时刻：立即生效
             
             bit4：对应gm_snd_gm_int_4。
             GM主模时强制退出异系统测量指示。高有效。
             GM DSP通过此寄存器触发类型4中断（int_type_gm_rcv_gm[4]）给GMDSP。
             生效时刻：立即生效
             
             bit3：对应gm_snd_gm_int_3。GM测量结束指示，高有效。
             用于触发中断类型3 （int_type_gm_rcv_gm[3]）给GMDSP。GM主模下对GM进行异系统测量时，如果GM测量结束，GM DSP配置此标志为1。 
             生效时刻：立即生效
 bit[2:0]    保留
   UNION结构:  BBPCOMM_GM_SND_GM_INT_13_UNION */
#define BBPCOMM_GM_SND_GM_INT_13_ADDR                 (SOC_BBP_COMM_BASE_ADDR + 0x86C)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:3]   逻辑自清零。高有效。
             
             bit15~7：对应gm_snd_gs_int_15 ~ 7。
             GM DSP通过此寄存器触发类型15~7中断（int_type_gs_rcv_gm[15:7]）给GSDSP。
             生效时刻：立即生效
             
             bit6：对应gm_snd_gs_int_6。
             GMDSP通过此寄存器触发类型6中断（int_type_gs_rcv_gm[6]）给GSDSP。
             生效时刻：立即生效
             
             bit5：对应gm_snd_gs_int_5。
             GM主模时通知GSDSP GS测量周期到达，高有效。
             GM DSP通过此寄存器触发类型5中断（int_type_gs_rcv_gm[5]）给GSDSP。
             生效时刻：立即生效
             
             bit4：对应gm_snd_gs_int_4。
             GM主模时强制退出异系统测量指示。高有效。
             GM DSP通过此寄存器触发类型4中断（int_type_gs_rcv_gm[4]）给GSDSP。
             生效时刻：立即生效
             
             bit3：对应gm_snd_gs_int_3。GM测量结束指示，高有效。
             用于触发中断类型3 （int_type_gs_rcv_gm[3]）给GSDSP。GS主模下对GM进行异系统测量时，如果GM测量结束，GM DSP配置此标志为1。 
             生效时刻：立即生效
 bit[2:0]    保留
   UNION结构:  BBPCOMM_GM_SND_GS_INT_13_UNION */
#define BBPCOMM_GM_SND_GS_INT_13_ADDR                 (SOC_BBP_COMM_BASE_ADDR + 0x870)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:3]   逻辑自清零。高有效。
             
             bit15~7：对应gm_snd_lte_int_15 ~ 7。
             GM DSP通过此寄存器触发类型15~7中断（int_type_lte_rcv_gm[15:7]）给LTEDSP。
             生效时刻：立即生效
             
             bit6：对应gm_snd_lte_int_6。
             GMDSP通过此寄存器触发类型6中断（int_type_lte_rcv_gm[6]）给LTEDSP。
             生效时刻：立即生效
             
             bit5：对应gm_snd_lte_int_5。
             GM主模时通知LTEDSP LTE测量周期到达，高有效。
             GM DSP通过此寄存器触发类型5中断（int_type_lte_rcv_gm[5]）给LTEDSP。
             生效时刻：立即生效
             
             bit4：对应gm_snd_lte_int_4。
             GM主模时强制退出异系统测量指示。高有效。
             GM DSP通过此寄存器触发类型4中断（int_type_lte_rcv_gm[4]）给LTEDSP。
             生效时刻：立即生效
             
             bit3：对应gm_snd_lte_int_3。GM测量结束指示，高有效。
             用于触发中断类型3 （int_type_lte_rcv_gm[3]）给LTEDSP。LTE主模下对GM进行异系统测量时，如果GM测量结束，GM DSP配置此标志为1。 
             生效时刻：立即生效
 bit[2:0]    保留
   UNION结构:  BBPCOMM_GM_SND_W_INT_13_UNION */
#define BBPCOMM_GM_SND_W_INT_13_ADDR                  (SOC_BBP_COMM_BASE_ADDR + 0x874)

/* 寄存器说明：寄存器
 bit[31:4]   保留
 bit[3:0]    异系统测量类型。
             4’d0：测量GS RSSI。
             4’d1: 测量GS bsic verification。
             4’d2: 测量GS bsic confirm。
             4’d3: 背景搜GS
             4’d4: 2G ANR。
             其它预留
             生效时刻：立即生效
   UNION结构:  BBPCOMM_GS_INTERSYS_MEASURE_TYPE_UNION */
#define BBPCOMM_GS_INTERSYS_MEASURE_TYPE_ADDR         (SOC_BBP_COMM_BASE_ADDR + 0x878)

/* 寄存器说明：寄存器
 bit[31:1]   保留
 bit[0]      单频点测量周期到指示，L或者TDS/W/Gm作为主模，通知GS DSP GS测量的单频点周期到达。
             只在连接态下需要配置。
             电平信号，高有效。
             生效时刻：立即生效
   UNION结构:  BBPCOMM_GS_MEASURE_REPORT_VALID_UNION */
#define BBPCOMM_GS_MEASURE_REPORT_VALID_ADDR          (SOC_BBP_COMM_BASE_ADDR + 0x87C)

/* 寄存器说明：寄存器
   详      述：预处理中断int0到int1之间的offset信息，单位us（向下取整）。。
            配置时刻：主模启动GS从模异系统测量时配置
            生效时刻：立即生效
   UNION结构 ：无 */
#define BBPCOMM_GS_PREINT_OFFSET_TIME_ADDR            (SOC_BBP_COMM_BASE_ADDR + 0x880)

/* 寄存器说明：寄存器
   详      述：GS作为从模时，主模分配给GS用于异系统测量时间长度，告知GSDSP。单位us，向下取整到us。
            配置时刻：主模启动GS从模异系统测量时配置
            生效时刻：立即生效
   UNION结构 ：无 */
#define BBPCOMM_GS_INTERSYS_MEASURE_TIME_ADDR         (SOC_BBP_COMM_BASE_ADDR + 0x884)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:0]   逻辑自清零。
             中断标志清除指示，高有效。用于清除INT_TYPE_GS_RCV_LTE[15:0]对应bit位
             生效时刻：立即生效
   UNION结构:  BBPCOMM_INT_CLEAR_GS_RCV_LTE_UNION */
#define BBPCOMM_INT_CLEAR_GS_RCV_LTE_ADDR             (SOC_BBP_COMM_BASE_ADDR + 0x888)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:0]   逻辑自清零。
             中断标志清除指示，高有效。用于清除INT_TYPE_GS_RCV_TDS[15:0]对应bit位
             生效时刻：立即生效
   UNION结构:  BBPCOMM_INT_CLEAR_GS_RCV_TDS_UNION */
#define BBPCOMM_INT_CLEAR_GS_RCV_TDS_ADDR             (SOC_BBP_COMM_BASE_ADDR + 0x88C)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:0]   逻辑自清零。
             中断标志清除指示，高有效。用于清除INT_TYPE_GS_RCV_GM[15:0]对应bit位
             生效时刻：立即生效
   UNION结构:  BBPCOMM_INT_CLEAR_GS_RCV_GM_UNION */
#define BBPCOMM_INT_CLEAR_GS_RCV_GM_ADDR              (SOC_BBP_COMM_BASE_ADDR + 0x890)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:0]   逻辑自清零。
             中断标志清除指示，高有效。用于清除INT_TYPE_GS_RCV_GS[15:0]对应bit位
             生效时刻：立即生效
   UNION结构:  BBPCOMM_INT_CLEAR_GS_RCV_GS_UNION */
#define BBPCOMM_INT_CLEAR_GS_RCV_GS_ADDR              (SOC_BBP_COMM_BASE_ADDR + 0x894)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:0]   逻辑自清零。
             中断标志清除指示，高有效。用于清除INT_TYPE_GS_RCV_W[15:0]对应bit位
             生效时刻：立即生效
   UNION结构:  BBPCOMM_INT_CLEAR_GS_RCV_W_UNION */
#define BBPCOMM_INT_CLEAR_GS_RCV_W_ADDR               (SOC_BBP_COMM_BASE_ADDR + 0x898)

/* 寄存器说明：寄存器
 bit[31:3]   保留
 bit[2:0]    逻辑自清零。
             中断标志清除指示，高有效。用于清除INT_TYPE_GS_INT012[2:0]对应bit位
             生效时刻：立即生效
   UNION结构:  BBPCOMM_INT_CLEAR_GS_INT012_UNION */
#define BBPCOMM_INT_CLEAR_GS_INT012_ADDR              (SOC_BBP_COMM_BASE_ADDR + 0x89C)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:0]   中断屏蔽标志。用于屏蔽INT_TYPE_GS_RCV_LTE[15:0]对应bit中断。
             0：屏蔽中断上报；
             1：使能中断上报。
             配置时刻：初始化
             生效时刻：立即生效
   UNION结构:  BBPCOMM_INT_MASK_GS_RCV_LTE_UNION */
#define BBPCOMM_INT_MASK_GS_RCV_LTE_ADDR              (SOC_BBP_COMM_BASE_ADDR + 0x8A0)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:0]   中断屏蔽标志。用于屏蔽INT_TYPE_GS_RCV_TDS[15:0]对应bit中断。
             0：屏蔽中断上报；
             1：使能中断上报。
             配置时刻：初始化
             生效时刻：立即生效
   UNION结构:  BBPCOMM_INT_MASK_GS_RCV_TDS_UNION */
#define BBPCOMM_INT_MASK_GS_RCV_TDS_ADDR              (SOC_BBP_COMM_BASE_ADDR + 0x8A4)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:0]   中断屏蔽标志。用于屏蔽INT_TYPE_GS_RCV_GM[15:0]对应bit中断。
             0：屏蔽中断上报；
             1：使能中断上报。
             配置时刻：初始化
             生效时刻：立即生效
   UNION结构:  BBPCOMM_INT_MASK_GS_RCV_GM_UNION */
#define BBPCOMM_INT_MASK_GS_RCV_GM_ADDR               (SOC_BBP_COMM_BASE_ADDR + 0x8A8)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:0]   中断屏蔽标志。用于屏蔽INT_TYPE_GS_RCV_GS[15:0]对应bit中断。
             0：屏蔽中断上报；
             1：使能中断上报。
             配置时刻：初始化
             生效时刻：立即生效
   UNION结构:  BBPCOMM_INT_MASK_GS_RCV_GS_UNION */
#define BBPCOMM_INT_MASK_GS_RCV_GS_ADDR               (SOC_BBP_COMM_BASE_ADDR + 0x8AC)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:0]   中断屏蔽标志。用于屏蔽INT_TYPE_GS_RCV_W[15:0]对应bit中断。
             0：屏蔽中断上报；
             1：使能中断上报。
             配置时刻：初始化
             生效时刻：立即生效
   UNION结构:  BBPCOMM_INT_MASK_GS_RCV_W_UNION */
#define BBPCOMM_INT_MASK_GS_RCV_W_ADDR                (SOC_BBP_COMM_BASE_ADDR + 0x8B0)

/* 寄存器说明：寄存器
 bit[31:3]   保留
 bit[2:0]    中断屏蔽标志。用于屏蔽INT_TYPE_GS_INT012[2:0]对应bit中断。
             0：屏蔽中断上报；
             1：使能中断上报。
             配置时刻：初始化
             生效时刻：立即生效
   UNION结构:  BBPCOMM_INT_MASK_GS_INT012_UNION */
#define BBPCOMM_INT_MASK_GS_INT012_ADDR               (SOC_BBP_COMM_BASE_ADDR + 0x8B4)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:0]   中断状态标志。各bit单独对应一种中断类型，高有效。
             [0]: 中断类型0。 LTE（主模）提前通知GS异系统测量启动中断。
             [1]: 中断类型1。 LTE（主模）进GAP中断，通知GS进行异系统测量射频通道配置，天线切换等操作。
             [2]: 中断类型2。 LTE（主模）出GAP中断。
             [3]: 中断类型3。LTE（从模）测量结束中断。
             [4]: 中断类型4。LTE（主模）强制GS退出异系统测量中断。
             [5]: 中断类型5。LTE（主模）通知GS上报本次测量结果。
             [6]: 保留。
             [15:7]: 对应中断类型[15:7]，预留。
             软件读取该中断状态上报寄存器后，需要和0x08A0的int_mask_gs_rcv_lte相与，再判断是否需要处理。
   UNION结构:  BBPCOMM_INT_TYPE_GS_RCV_LTE_UNION */
#define BBPCOMM_INT_TYPE_GS_RCV_LTE_ADDR              (SOC_BBP_COMM_BASE_ADDR + 0x8B8)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:0]   中断状态标志0，1，2只有当TDS处于主模时才能产生。
             
             [0]: 中断类型0。 TDS（主模）提前通知GS异系统测量启动中断。
             [1]: 中断类型1。 TDS（主模）进GAP中断，通知GS进行异系统测量射频通道配置，天线切换等操作。
             [2]: 中断类型2。 TDS（主模）出GAP中断。
             [3]: 中断类型3。TDS（从模）测量结束中断。
             [4]: 中断类型4。TDS（主模）强制GS退出异系统测量中断。
             [5]: 中断类型5。TDS（主模）通知GS上报本次测量结果。
             [6]: 中断类型6。保留。
             [15:7]: 对应中断类型[15:7]，预留。
             软件读取该中断状态上报寄存器后，需要和0x08A4的int_mask_gs_rcv_tds相与，再判断是否需要处理。
   UNION结构:  BBPCOMM_INT_TYPE_GS_RCV_TDS_UNION */
#define BBPCOMM_INT_TYPE_GS_RCV_TDS_ADDR              (SOC_BBP_COMM_BASE_ADDR + 0x8BC)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:0]   中断状态标志。各bit单独对应一种中断类型，高有效。
             [0]: 中断类型0。GM（主模）提前通知GS异系统测量启动中断。
             [1]: 中断类型1。GM（主模）进GAP中断，通知GS进行异系统测量射频通道配置，天线切换等操作。
             [2]: 中断类型2。GM（主模）出GAP中断。
             [3]: 中断类型3。GM（从模）测量结束中断。
             [4]: 中断类型4。GM（主模）强制GS退出异系统测量中断。
             [5]: 中断类型5。GM（主模）通知GS上报本次测量结果。
             [6]: 中断类型6。保留。
             [15:7]: 对应中断类型[15:7]，预留。
             中断状态标志0，1，2只有当GM处于主模时才能产生。
             软件读取该中断状态上报寄存器后，需要和0x08A8的int_mask_gs_rcv_gm相与，再判断是否需要处理。
   UNION结构:  BBPCOMM_INT_TYPE_GS_RCV_GM_UNION */
#define BBPCOMM_INT_TYPE_GS_RCV_GM_ADDR               (SOC_BBP_COMM_BASE_ADDR + 0x8C0)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:0]   [0]: 中断类型0。GS（主模）提前通知GS异系统测量启动中断。
             [1]: 中断类型1。GS（主模）进GAP中断，通知GS进行异系统测量射频通道配置，天线切换等操作。
             [2]: 中断类型2。GS（主模）出GAP中断。
             [3]: 中断类型3。GS（从模）测量结束中断。
             [4]: 中断类型4。GS（主模）强制GS退出异系统测量中断。
             [5]: 中断类型5。GS（主模）通知GS上报本次测量结果。
             [6]: 中断类型6。保留。
             [15:7]: 对应中断类型[15:7]，预留。
             中断状态标志0，1，2只有当TDS处于主模时才能产生。
             软件读取该中断状态上报寄存器后，需要和0x08AC的int_mask_gs_rcv_gs相与，再判断是否需要处理。
   UNION结构:  BBPCOMM_INT_TYPE_GS_RCV_GS_UNION */
#define BBPCOMM_INT_TYPE_GS_RCV_GS_ADDR               (SOC_BBP_COMM_BASE_ADDR + 0x8C4)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:0]   中断状态标志。各bit单独对应一种中断类型，高有效。
             [0]: 中断类型0。W（主模）提前通知GS异频测量启动中断。
             [1]: 中断类型1。W（主模）进GAP中断，通知GS进行异频测量射频通道配置，天线切换等操作。
             [2]: 中断类型2。W（主模）出GAP中断。
             [3]: 中断类型3。W（从模，即异频）测量结束中断。
             [4]: 中断类型4。W（主模）强制GS退出异频测量中断。
             [5]: 中断类型5。W（主模）通知GS上报本次测量结果。
             [6]: 中断类型6。保留。
             [15:7]: 对应中断类型[15:7]，预留。
             中断状态标志0，1，2只有当W处于主模时才能产生。
             软件读取该中断状态上报寄存器后，需要和0x08B0的int_mask_gs_rcv_w相与，再判断是否需要处理。
   UNION结构:  BBPCOMM_INT_TYPE_GS_RCV_W_UNION */
#define BBPCOMM_INT_TYPE_GS_RCV_W_ADDR                (SOC_BBP_COMM_BASE_ADDR + 0x8C8)

/* 寄存器说明：寄存器
 bit[31:3]   保留
 bit[2:0]    主模产生int0、1、2的上报（(包括异频、异系统场景)），高有效。
             [0]: GS主模时，CTU告诉通知GSDSP产生int0中断。
             [1]: GS主模时，CTU告诉通知GSDSP产生int1中断。
             [2]: GS主模时，CTU告诉通知GSDSP产生int2中断。
             软件读取该中断状态上报寄存器后，需要和0x08B4的int_mask_gs_int012相与，再判断是否需要处理。
   UNION结构:  BBPCOMM_INT_TYPE_GS_INT012_UNION */
#define BBPCOMM_INT_TYPE_GS_INT012_ADDR               (SOC_BBP_COMM_BASE_ADDR + 0x8CC)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:3]   逻辑自清零。高有效。
             
             bit15~7：对应gs_snd_lte_int_15 ~ 7。
             GS DSP通过此寄存器触发类型15~7中断（int_type_lte_rcv_gs[15:7]）给LTEDSP。
             生效时刻：立即生效
             
             bit6：对应gs_snd_lte_int_6。
             GSDSP通过此寄存器触发类型6中断（int_type_lte_rcv_gs[6]）给LTEDSP。
             生效时刻：立即生效
             
             bit5：对应gs_snd_lte_int_5。
             GS主模时通知LTEDSP LTE测量周期到达，高有效。
             GS DSP通过此寄存器触发类型5中断（int_type_lte_rcv_gs[5]）给LTEDSP。
             生效时刻：立即生效
             
             bit4：对应gs_snd_lte_int_4。
             GS主模时强制退出异系统测量指示。高有效。
             GS DSP通过此寄存器触发类型4中断（int_type_lte_rcv_gs[4]）给LTEDSP。
             生效时刻：立即生效
             
             bit3：对应gs_snd_lte_int_3。GS测量结束指示，高有效。
             用于触发中断类型3 （int_type_lte_rcv_gs[3]）给LTEDSP。LTE主模下对GS进行异系统测量时，如果GS测量结束，GS DSP配置此标志为1。 
             生效时刻：立即生效
 bit[2:0]    保留
   UNION结构:  BBPCOMM_GS_SND_LTE_INT_13_UNION */
#define BBPCOMM_GS_SND_LTE_INT_13_ADDR                (SOC_BBP_COMM_BASE_ADDR + 0x8D0)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:3]   逻辑自清零。高有效。
             
             bit15~7：对应gs_snd_tds_int_15 ~ 7。
             GS DSP通过此寄存器触发类型15~7中断（int_type_tds_rcv_gs[15:7]）给TDSDSP。
             生效时刻：立即生效
             
             bit6：对应gs_snd_tds_int_6。
             GSDSP通过此寄存器触发类型6中断（int_type_tds_rcv_gs[6]）给TDSDSP。
             生效时刻：立即生效
             
             bit5：对应gs_snd_tds_int_5。
             GS主模时通知TDSDSP TDS测量周期到达，高有效。
             GS DSP通过此寄存器触发类型5中断（int_type_tds_rcv_gs[5]）给TDSDSP。
             生效时刻：立即生效
             
             bit4：对应gs_snd_tds_int_4。
             GS主模时强制退出异系统测量指示。高有效。
             GS DSP通过此寄存器触发类型4中断（int_type_tds_rcv_gs[4]）给TDSDSP。
             生效时刻：立即生效
             
             bit3：对应gs_snd_tds_int_3。GS测量结束指示，高有效。
             用于触发中断类型3 （int_type_tds_rcv_gs[3]）给TDSDSP。TDS主模下对GS进行异系统测量时，如果GS测量结束，GS DSP配置此标志为1。 
             生效时刻：立即生效
 bit[2:0]    保留
   UNION结构:  BBPCOMM_GS_SND_TDS_INT_13_UNION */
#define BBPCOMM_GS_SND_TDS_INT_13_ADDR                (SOC_BBP_COMM_BASE_ADDR + 0x8D4)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:3]   逻辑自清零。高有效。
             
             bit15~7：对应gs_snd_gm_int_15 ~ 7。
             GS DSP通过此寄存器触发类型15~7中断（int_type_gm_rcv_gs[15:7]）给GMDSP。
             生效时刻：立即生效
             
             bit6：对应gs_snd_gm_int_6。
             GSDSP通过此寄存器触发类型6中断（int_type_gm_rcv_gs[6]）给GMDSP。
             生效时刻：立即生效
             
             bit5：对应gs_snd_gm_int_5。
             GS主模时通知GMDSP GM测量周期到达，高有效。
             GS DSP通过此寄存器触发类型5中断（int_type_gm_rcv_gs[5]）给GMDSP。
             生效时刻：立即生效
             
             bit4：对应gs_snd_gm_int_4。
             GS主模时强制退出异系统测量指示。高有效。
             GS DSP通过此寄存器触发类型4中断（int_type_gm_rcv_gs[4]）给GMDSP。
             生效时刻：立即生效
             
             bit3：对应gs_snd_gm_int_3。GS测量结束指示，高有效。
             用于触发中断类型3 （int_type_gm_rcv_gs[3]）给GMDSP。GM主模下对GS进行异系统测量时，如果GS测量结束，GS DSP配置此标志为1。 
             生效时刻：立即生效
 bit[2:0]    保留
   UNION结构:  BBPCOMM_GS_SND_GM_INT_13_UNION */
#define BBPCOMM_GS_SND_GM_INT_13_ADDR                 (SOC_BBP_COMM_BASE_ADDR + 0x8D8)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:3]   逻辑自清零。高有效。
             
             bit15~7：对应gs_snd_gs_int_15 ~ 7。
             GS DSP通过此寄存器触发类型15~7中断（int_type_gs_rcv_gs[15:7]）给GSDSP。
             生效时刻：立即生效
             
             bit6：对应gs_snd_gs_int_6。
             GSDSP通过此寄存器触发类型6中断（int_type_gs_rcv_gs[6]）给GSDSP。
             生效时刻：立即生效
             
             bit5：对应gs_snd_gs_int_5。
             GS主模时通知GSDSP GS测量周期到达，高有效。
             GS DSP通过此寄存器触发类型5中断（int_type_gs_rcv_gs[5]）给GSDSP。
             生效时刻：立即生效
             
             bit4：对应gs_snd_gs_int_4。
             GS主模时强制退出异系统测量指示。高有效。
             GS DSP通过此寄存器触发类型4中断（int_type_gs_rcv_gs[4]）给GSDSP。
             生效时刻：立即生效
             
             bit3：对应gs_snd_gs_int_3。GS测量结束指示，高有效。
             用于触发中断类型3 （int_type_gs_rcv_gs[3]）给GSDSP。GS主模下对GS进行异系统测量时，如果GS测量结束，GS DSP配置此标志为1。 
             生效时刻：立即生效
 bit[2:0]    保留
   UNION结构:  BBPCOMM_GS_SND_GS_INT_13_UNION */
#define BBPCOMM_GS_SND_GS_INT_13_ADDR                 (SOC_BBP_COMM_BASE_ADDR + 0x8DC)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:3]   逻辑自清零。高有效。
             
             bit15~7：对应gs_snd_w_int_15 ~ 7。
             GS DSP通过此寄存器触发类型15~7中断（int_type_w_rcv_gs[15:7]）给WDSP。
             生效时刻：立即生效
             
             bit6：对应gs_snd_w_int_6。
             GSDSP通过此寄存器触发类型6中断（int_type_w_rcv_gs[6]）给WDSP。
             生效时刻：立即生效
             
             bit5：对应gs_snd_w_int_5。
             GS主模时通知WDSP W测量周期到达，高有效。
             GS DSP通过此寄存器触发类型5中断（int_type_w_rcv_gs[5]）给WDSP。
             生效时刻：立即生效
             
             bit4：对应gs_snd_w_int_4。
             GS主模时强制退出异系统测量指示。高有效。
             GS DSP通过此寄存器触发类型4中断（int_type_w_rcv_gs[4]）给WDSP。
             生效时刻：立即生效
             
             bit3：对应gs_snd_w_int_3。GS测量结束指示，高有效。
             用于触发中断类型3 （int_type_w_rcv_gs[3]）给WDSP。W主模下对GS进行异系统测量时，如果GS测量结束，GS DSP配置此标志为1。 
             生效时刻：立即生效
 bit[2:0]    保留
   UNION结构:  BBPCOMM_GS_SND_W_INT_13_UNION */
#define BBPCOMM_GS_SND_W_INT_13_ADDR                  (SOC_BBP_COMM_BASE_ADDR + 0x8E0)

/* 寄存器说明：
 bit[31:2]   保留
 bit[1:0]    V9R1版本，软件不配置。
   UNION结构:  BBPCOMM_GBBP1_19M_SEL_UNION */
#define BBPCOMM_GBBP1_19M_SEL_ADDR                    (SOC_BBP_COMM_BASE_ADDR + 0x8E4)

/* 寄存器说明：
 bit[31:2]   保留
 bit[1:0]    V9R1版本，软件不配置。
   UNION结构:  BBPCOMM_GBBP2_19M_SEL_UNION */
#define BBPCOMM_GBBP2_19M_SEL_ADDR                    (SOC_BBP_COMM_BASE_ADDR + 0x8E8)

/* 寄存器说明：
 bit[31:2]   保留
 bit[1:0]    V9R1版本，软件不配置。
   UNION结构:  BBPCOMM_WBBP_19M_SEL_UNION */
#define BBPCOMM_WBBP_19M_SEL_ADDR                     (SOC_BBP_COMM_BASE_ADDR + 0x8EC)

/* 寄存器说明：
 bit[31:2]   保留
 bit[1:0]    V9R1版本，软件不配置。
   UNION结构:  BBPCOMM_LBBP_19M_SEL_UNION */
#define BBPCOMM_LBBP_19M_SEL_ADDR                     (SOC_BBP_COMM_BASE_ADDR + 0x8F0)

/* 寄存器说明：
 bit[31:2]   保留
 bit[1:0]    V9R1版本，软件不配置。
   UNION结构:  BBPCOMM_TBBP1_19M_SEL_UNION */
#define BBPCOMM_TBBP1_19M_SEL_ADDR                    (SOC_BBP_COMM_BASE_ADDR + 0x8F4)

/* 寄存器说明：
 bit[31:2]   保留
 bit[1:0]    V9R1版本，软件不配置。
   UNION结构:  BBPCOMM_GBBP1_CLK_SEL_UNION */
#define BBPCOMM_GBBP1_CLK_SEL_ADDR                    (SOC_BBP_COMM_BASE_ADDR + 0x8F8)

/* 寄存器说明：
 bit[31:2]   保留
 bit[1:0]    V9R1版本，软件不配置。
   UNION结构:  BBPCOMM_GBBP2_CLK_SEL_UNION */
#define BBPCOMM_GBBP2_CLK_SEL_ADDR                    (SOC_BBP_COMM_BASE_ADDR + 0x8FC)

/* 寄存器说明：
 bit[31:2]   保留
 bit[1:0]    V9R1版本，软件不配置。
   UNION结构:  BBPCOMM_WBBP_CLK_SEL_UNION */
#define BBPCOMM_WBBP_CLK_SEL_ADDR                     (SOC_BBP_COMM_BASE_ADDR + 0x900)

/* 寄存器说明：
 bit[31:2]   保留
 bit[1:0]    V9R1版本，软件不配置。
   UNION结构:  BBPCOMM_LBBP_CLK_SEL_UNION */
#define BBPCOMM_LBBP_CLK_SEL_ADDR                     (SOC_BBP_COMM_BASE_ADDR + 0x904)

/* 寄存器说明：
 bit[31:2]   保留
 bit[1:0]    V9R1版本，软件不配置。
   UNION结构:  BBPCOMM_TBBP_CLK_SEL_UNION */
#define BBPCOMM_TBBP_CLK_SEL_ADDR                     (SOC_BBP_COMM_BASE_ADDR + 0x908)

/* 寄存器说明：寄存器
 bit[31:10]  保留
 bit[9:9]    1'b1：LTE的TX通路I路数据取反
             1'b0：LTE的TX通路I路数据不取反
 bit[8:8]    1'b1：LTE的TX通路Q路数据取反
             1'b0：LTE的TX通路Q路数据不取反
 bit[7:7]    1'b1：LTE的TX通路IQ路数据交换
             1'b0：LTE的TX通路IQ路数据不交换
             V3R3不支持IQ交换功能。
 bit[6:6]    1'b1：LTE的RXA通路I路数据取反
             1'b0：LTE的RXA通路I路数据不取反
 bit[5:5]    1'b1：LTE的RXA通路Q路数据取反
             1'b0：LTE的RXA通路Q路数据不取反
 bit[4:4]    1'b1：LTE的RXA通路IQ路数据交换
             1'b0：LTE的RXA通路IQ路数据不交换
             V3R3不支持IQ交换功能。
 bit[3:3]    1'b1：LTE的RXB通路I路数据取反
             1'b0：LTE的RXB通路I路数据不取反
 bit[2:2]    1'b1：LTE的RXB通路Q路数据取反
             1'b0：LTE的RXB通路Q路数据不取反
 bit[1:1]    1'b1：LTE的RXB通路IQ路数据交换
             1'b0：LTE的RXB通路IQ路数据不交换
             V3R3不支持IQ交换功能。
 bit[0:0]    保留
   UNION结构:  BBPCOMM_LTE_ABBIF_FMT_UNION */
#define BBPCOMM_LTE_ABBIF_FMT_ADDR                    (SOC_BBP_COMM_BASE_ADDR + 0x90C)

/* 寄存器说明：寄存器
 bit[31:10]  保留
 bit[9:9]    1'b1：W的TX通路I路数据取反
             1'b0：W的TX通路I路数据不取反。
 bit[8:8]    1'b1：W的TX通路Q路数据取反
             1'b0：W的TX通路Q路数据不取反
 bit[7:7]    1'b1：W的TX通路IQ路数据交换
             1'b0：W的TX通路IQ路数据不交换
             V3R3不支持IQ交换功能。
 bit[6:6]    1'b1：W的RXA通路I路数据取反
             1'b0：W的RXA通路I路数据不取反
 bit[5:5]    1'b1：W的RXA通路Q路数据取反
             1'b0：W的RXA通路Q路数据不取反
 bit[4:4]    1'b1：W的RXA通路IQ路数据交换
             1'b0：W的RXA通路IQ路数据不交换
             V3R3不支持IQ交换功能。
 bit[3:3]    1'b1：W的RXB通路I路数据取反
             1'b0：W的RXB通路I路数据不取反
 bit[2:2]    1'b1：W的RXB通路Q路数据取反
             1'b0：W的RXB通路Q路数据不取反
 bit[1:1]    1'b1：W的RXB通路IQ路数据交换
             1'b0：W的RXB通路IQ路数据不交换
             V3R3不支持IQ交换功能。
 bit[0:0]    保留
   UNION结构:  BBPCOMM_W_ABBIF_FMT_UNION */
#define BBPCOMM_W_ABBIF_FMT_ADDR                      (SOC_BBP_COMM_BASE_ADDR + 0x910)

/* 寄存器说明：寄存器
 bit[31:10]  保留
 bit[9:9]    1'b1：TDS的TX通路I路数据取反
             1'b0：TDS的TX通路I路数据不取反
 bit[8:8]    1'b1：TDS的TX通路Q路数据取反
             1'b0：TDS的TX通路Q路数据不取反
 bit[7:7]    1'b1：TDS的TX通路IQ路数据交换
             1'b0：TDS的TX通路IQ路数据不交换
             V3R3不支持IQ交换功能。
 bit[6:6]    1'b1：TDS的RXA通路I路数据取反
             1'b0：TDS的RXA通路I路数据不取反
 bit[5:5]    1'b1：TDS的RXA通路Q路数据取反
             1'b0：TDS的RXA通路Q路数据不取反
 bit[4:4]    1'b1：TDS的RXA通路IQ路数据交换
             1'b0：TDS的RXA通路IQ路数据不交换
             V3R3不支持IQ交换功能。
 bit[3:3]    1'b1：TDS的RXB通路I路数据取反
             1'b0：TDS的RXB通路I路数据不取反
 bit[2:2]    1'b1：TDS的RXB通路Q路数据取反
             1'b0：TDS的RXB通路Q路数据不取反
 bit[1:1]    1'b1：TDS的RXB通路IQ路数据交换
             1'b0：TDS的RXB通路IQ路数据不交换
             V3R3不支持IQ交换功能。
 bit[0:0]    保留
   UNION结构:  BBPCOMM_TDS_ABBIF_FMT_UNION */
#define BBPCOMM_TDS_ABBIF_FMT_ADDR                    (SOC_BBP_COMM_BASE_ADDR + 0x914)

/* 寄存器说明：寄存器
 bit[31:10]  保留
 bit[9:9]    1'b1：GM的TX通路I路数据取反
             1'b0：GM的TX通路I路数据不取反
 bit[8:8]    1'b1：GM的TX通路Q路数据取反
             1'b0：GM的TX通路Q路数据不取反
 bit[7:7]    1'b1：GM的TX通路IQ路数据交换
             1'b0：GM的TX通路IQ路数据不交换
             V3R3不支持IQ交换功能。
 bit[6:6]    1'b1：GM的RXA通路I路数据取反
             1'b0：GM的RXA通路I路数据不取反
 bit[5:5]    1'b1：GM的RXA通路Q路数据取反
             1'b0：GM的RXA通路Q路数据不取反
 bit[4:4]    1'b1：GM的RXA通路IQ路数据交换
             1'b0：GM的RXA通路IQ路数据不交换
             V3R3不支持IQ交换功能。
 bit[3:3]    1'b1：GM的RXB通路I路数据取反
             1'b0：GM的RXB通路I路数据不取反
 bit[2:2]    1'b1：GM的RXB通路Q路数据取反
             1'b0：GM的RXB通路Q路数据不取反
 bit[1:1]    1'b1：GM的RXB通路IQ路数据交换
             1'b0：GM的RXB通路IQ路数据不交换
             V3R3不支持IQ交换功能。
 bit[0:0]    保留
   UNION结构:  BBPCOMM_GM_ABBIF_FMT_UNION */
#define BBPCOMM_GM_ABBIF_FMT_ADDR                     (SOC_BBP_COMM_BASE_ADDR + 0x918)

/* 寄存器说明：寄存器
 bit[31:10]  保留
 bit[9:9]    1'b1：GS的TX通路I路数据取反
             1'b0：GS的TX通路I路数据不取反
 bit[8:8]    1'b1：GS的TX通路Q路数据取反
             1'b0：GS的TX通路Q路数据不取反
 bit[7:7]    1'b1：GS的TX通路IQ路数据交换
             1'b0：GS的TX通路IQ路数据不交换
             V3R3不支持IQ交换功能。
 bit[6:6]    1'b1：GS的RXA通路I路数据取反
             1'b0：GS的RXA通路I路数据不取反
 bit[5:5]    1'b1：GS的RXA通路Q路数据取反
             1'b0：GS的RXA通路Q路数据不取反
 bit[4:4]    1'b1：GS的RXA通路IQ路数据交换
             1'b0：GS的RXA通路IQ路数据不交换
             V3R3不支持IQ交换功能。
 bit[3:3]    1'b1：GS的RXB通路I路数据取反
             1'b0：GS的RXB通路I路数据不取反
 bit[2:2]    1'b1：GS的RXB通路Q路数据取反
             1'b0：GS的RXB通路Q路数据不取反
 bit[1:1]    1'b1：GS的RXB通路IQ路数据交换
             1'b0：GS的RXB通路IQ路数据不交换
             V3R3不支持IQ交换功能。
 bit[0:0]    保留
   UNION结构:  BBPCOMM_GS_ABBIF_FMT_UNION */
#define BBPCOMM_GS_ABBIF_FMT_ADDR                     (SOC_BBP_COMM_BASE_ADDR + 0x91C)

/* 寄存器说明：寄存器
   详      述：寄存器3，预留。供DSP之间传递信息。
   UNION结构 ：无 */
#define BBPCOMM_REG0_FOR_USE_ADDR                     (SOC_BBP_COMM_BASE_ADDR + 0x920)

/* 寄存器说明：寄存器
   详      述：寄存器4，预留。供DSP之间传递信息。
   UNION结构 ：无 */
#define BBPCOMM_REG1_FOR_USE_ADDR                     (SOC_BBP_COMM_BASE_ADDR + 0x924)

/* 寄存器说明：寄存器
   详      述：寄存器4，预留。供DSP之间传递信息。
   UNION结构 ：无 */
#define BBPCOMM_REG2_FOR_USE_ADDR                     (SOC_BBP_COMM_BASE_ADDR + 0x928)

/* 寄存器说明：寄存器
   详      述：寄存器6，预留。供DSP之间传递信息。
   UNION结构 ：无 */
#define BBPCOMM_REG3_FOR_USE_ADDR                     (SOC_BBP_COMM_BASE_ADDR + 0x92C)

/* 寄存器说明：寄存器
   详      述：寄存器7，预留。供DSP之间传递信息。
   UNION结构 ：无 */
#define BBPCOMM_REG4_FOR_USE_ADDR                     (SOC_BBP_COMM_BASE_ADDR + 0x930)

/* 寄存器说明：寄存器
   详      述：寄存器8，预留。供DSP之间传递信息。
   UNION结构 ：无 */
#define BBPCOMM_REG5_FOR_USE_ADDR                     (SOC_BBP_COMM_BASE_ADDR + 0x934)

/* 寄存器说明：寄存器
   详      述：寄存器9，预留。供DSP之间传递信息。
   UNION结构 ：无 */
#define BBPCOMM_REG6_FOR_USE_ADDR                     (SOC_BBP_COMM_BASE_ADDR + 0x938)

/* 寄存器说明：寄存器
   详      述：寄存器7，预留。供DSP之间传递信息。
   UNION结构 ：无 */
#define BBPCOMM_REG7_FOR_USE_ADDR                     (SOC_BBP_COMM_BASE_ADDR + 0x93C)

/* 寄存器说明：寄存器
   详      述：寄存器8，预留。供DSP之间传递信息。
   UNION结构 ：无 */
#define BBPCOMM_REG8_FOR_USE_ADDR                     (SOC_BBP_COMM_BASE_ADDR + 0x940)

/* 寄存器说明：寄存器
   详      述：寄存器9，预留。供DSP之间传递信息。
   UNION结构 ：无 */
#define BBPCOMM_REG9_FOR_USE_ADDR                     (SOC_BBP_COMM_BASE_ADDR + 0x944)

/* 寄存器说明：寄存器
   详      述：寄存器10，预留。供DSP之间传递信息。
   UNION结构 ：无 */
#define BBPCOMM_REG10_FOR_USE_ADDR                    (SOC_BBP_COMM_BASE_ADDR + 0x948)

/* 寄存器说明：寄存器
   详      述：寄存器11，预留。供DSP之间传递信息。
   UNION结构 ：无 */
#define BBPCOMM_REG11_FOR_USE_ADDR                    (SOC_BBP_COMM_BASE_ADDR + 0x94C)

/* 寄存器说明：寄存器
   详      述：寄存器12，预留。供DSP之间传递信息。
   UNION结构 ：无 */
#define BBPCOMM_REG12_FOR_USE_ADDR                    (SOC_BBP_COMM_BASE_ADDR + 0x950)

/* 寄存器说明：寄存器
   详      述：寄存器13，预留。供DSP之间传递信息。
   UNION结构 ：无 */
#define BBPCOMM_REG13_FOR_USE_ADDR                    (SOC_BBP_COMM_BASE_ADDR + 0x954)

/* 寄存器说明：寄存器
   详      述：寄存器14，预留。供DSP之间传递信息。
   UNION结构 ：无 */
#define BBPCOMM_REG14_FOR_USE_ADDR                    (SOC_BBP_COMM_BASE_ADDR + 0x958)

/* 寄存器说明：寄存器
   详      述：寄存器15，预留。供DSP之间传递信息。
   UNION结构 ：无 */
#define BBPCOMM_REG15_FOR_USE_ADDR                    (SOC_BBP_COMM_BASE_ADDR + 0x95C)

/* 寄存器说明：寄存器
   详      述：寄存器16，预留。供DSP之间传递信息。
   UNION结构 ：无 */
#define BBPCOMM_REG16_FOR_USE_ADDR                    (SOC_BBP_COMM_BASE_ADDR + 0x960)

/* 寄存器说明：寄存器
   详      述：寄存器17，预留。供DSP之间传递信息。
   UNION结构 ：无 */
#define BBPCOMM_REG17_FOR_USE_ADDR                    (SOC_BBP_COMM_BASE_ADDR + 0x964)

/* 寄存器说明：寄存器
   详      述：寄存器18，预留。供DSP之间传递信息。
   UNION结构 ：无 */
#define BBPCOMM_REG18_FOR_USE_ADDR                    (SOC_BBP_COMM_BASE_ADDR + 0x968)

/* 寄存器说明：寄存器
   详      述：寄存器19，预留。供DSP之间传递信息。
   UNION结构 ：无 */
#define BBPCOMM_REG19_FOR_USE_ADDR                    (SOC_BBP_COMM_BASE_ADDR + 0x96C)

/* 寄存器说明：寄存器
   详      述：寄存器20，预留。供DSP之间传递信息。
   UNION结构 ：无 */
#define BBPCOMM_REG20_FOR_USE_ADDR                    (SOC_BBP_COMM_BASE_ADDR + 0x970)

/* 寄存器说明：寄存器
   详      述：寄存器21，预留。供DSP之间传递信息。
   UNION结构 ：无 */
#define BBPCOMM_REG21_FOR_USE_ADDR                    (SOC_BBP_COMM_BASE_ADDR + 0x974)

/* 寄存器说明：寄存器
   详      述：寄存器22，预留。供DSP之间传递信息。
   UNION结构 ：无 */
#define BBPCOMM_REG22_FOR_USE_ADDR                    (SOC_BBP_COMM_BASE_ADDR + 0x978)

/* 寄存器说明：寄存器
   详      述：寄存器23，预留。供DSP之间传递信息。
   UNION结构 ：无 */
#define BBPCOMM_REG23_FOR_USE_ADDR                    (SOC_BBP_COMM_BASE_ADDR + 0x97C)

/* 寄存器说明：寄存器
 bit[31:1]   寄存器24，预留。供DSP之间传递信息。
 bit[0]      GSM1跨通道异频测量模式使能：
             1'b0：不使能
             1'b1：使能
             
             V7R2新增。
   UNION结构:  BBPCOMM_REG24_FOR_USE_UNION */
#define BBPCOMM_REG24_FOR_USE_ADDR                    (SOC_BBP_COMM_BASE_ADDR + 0x980)

/* 寄存器说明：寄存器
 bit[31:1]   寄存器25，预留。供DSP之间传递信息。
 bit[0]      GSM2跨通道异频测量模式使能：
             1'b0：不使能
             1'b1：使能
             
             V7R2新增。
   UNION结构:  BBPCOMM_REG25_FOR_USE_UNION */
#define BBPCOMM_REG25_FOR_USE_ADDR                    (SOC_BBP_COMM_BASE_ADDR + 0x984)

/* 寄存器说明：寄存器
 bit[31:3]   寄存器26，预留。供DSP之间传递信息。
 bit[2]      通道1映射ssi器件关系：
             1'b0：通道1映射到ssi0
             1'b1：通道1映射到ssi1
             
             V7R2新增。
 bit[1]      通道0映射ssi器件关系：
             1'b0：通道0映射到ssi0
             1'b1：通道0映射到ssi1
             
             V7R2新增。
 bit[0]      ssi通道不绑定使能：
             1'b0：通道绑定
             1'b1：通道不绑定
             
             V7R2新增。
   UNION结构:  BBPCOMM_REG26_FOR_USE_UNION */
#define BBPCOMM_REG26_FOR_USE_ADDR                    (SOC_BBP_COMM_BASE_ADDR + 0x988)

/* 寄存器说明：寄存器
 bit[31:1]   寄存器27，预留。供DSP之间传递信息。
 bit[0]      输出给RF的TCVR_ON 双通道一致使能
             1：任意通道的TCVR_ON拉高，则2个通道的TCVR_ON都拉高；
             0：根据各自通道状态拉高
             
             V7R2新增。
   UNION结构:  BBPCOMM_REG27_FOR_USE_UNION */
#define BBPCOMM_REG27_FOR_USE_ADDR                    (SOC_BBP_COMM_BASE_ADDR + 0x98C)

/* 寄存器说明：寄存器
 bit[31:4]   寄存器28，预留。供DSP之间传递信息。
 bit[3:1]    输出给ABB_CFG模块的选通信号ch0_sel_abb强制时的配置值
             
             V7R2新增。
 bit[0]      输出给ABB_CFG模块的选通信号ch1_sel_abb是否强制使能
             1：强制，强制值为ch0_abb_sel_force_value
             0：不强制，逻辑自控
             
             V7R2新增。
   UNION结构:  BBPCOMM_REG28_FOR_USE_UNION */
#define BBPCOMM_REG28_FOR_USE_ADDR                    (SOC_BBP_COMM_BASE_ADDR + 0x990)

/* 寄存器说明：寄存器
 bit[31:4]   寄存器29，预留。供DSP之间传递信息。
 bit[3:1]    输出给ABB_CFG模块的选通信号ch1_sel_abb强制时的配置值
             
             V7R2新增。
 bit[0]      输出给ABB_CFG模块的选通信号ch1_sel_abb是否强制使能
             1：强制，强制值为ch1_abb_sel_force_value
             0：不强制，逻辑自控
             
             V7R2新增。
   UNION结构:  BBPCOMM_REG29_FOR_USE_UNION */
#define BBPCOMM_REG29_FOR_USE_ADDR                    (SOC_BBP_COMM_BASE_ADDR + 0x994)

/* 寄存器说明：寄存器
 bit[31:12]  寄存器30，预留。供DSP之间传递信息。
 bit[11:0]   ch0的com2abb_line_ctrl_mode切换时，com2abb_rxa/b_en强制拉低的时间。单位：总线时钟
             
             V7R2新增。
   UNION结构:  BBPCOMM_REG30_FOR_USE_UNION */
#define BBPCOMM_REG30_FOR_USE_ADDR                    (SOC_BBP_COMM_BASE_ADDR + 0x998)

/* 寄存器说明：寄存器
 bit[31:12]  寄存器31，预留。供DSP之间传递信息。
 bit[11:0]   ch1的com2abb_line_ctrl_mode切换时，com2abb_rxa/b_en强制拉低的时间。单位：总线时钟
             
             V7R2新增。
   UNION结构:  BBPCOMM_REG31_FOR_USE_UNION */
#define BBPCOMM_REG31_FOR_USE_ADDR                    (SOC_BBP_COMM_BASE_ADDR + 0x99C)

/* 寄存器说明：寄存器
 bit[31:5]   寄存器32，预留。供DSP之间传递信息。
 bit[4]      
 bit[3:1]    保留
 bit[0]      
   UNION结构:  BBPCOMM_REG32_FOR_USE_UNION */
#define BBPCOMM_REG32_FOR_USE_ADDR                    (SOC_BBP_COMM_BASE_ADDR + 0x9A0)

/* 寄存器说明：寄存器
 bit[31:1]   寄存器33，预留。供DSP之间传递信息。
 bit[0]      
   UNION结构:  BBPCOMM_REG33_FOR_USE_UNION */
#define BBPCOMM_REG33_FOR_USE_ADDR                    (SOC_BBP_COMM_BASE_ADDR + 0x9A4)

/* 寄存器说明：寄存器
   详      述：寄存器34，预留。供DSP之间传递信息。
   UNION结构 ：无 */
#define BBPCOMM_REG34_FOR_USE_ADDR                    (SOC_BBP_COMM_BASE_ADDR + 0x9A8)

/* 寄存器说明：寄存器
   详      述：寄存器35，预留。供DSP之间传递信息。
   UNION结构 ：无 */
#define BBPCOMM_REG35_FOR_USE_ADDR                    (SOC_BBP_COMM_BASE_ADDR + 0x9AC)

/* 寄存器说明：寄存器
   详      述：寄存器36，预留。供DSP之间传递信息。
   UNION结构 ：无 */
#define BBPCOMM_REG36_FOR_USE_ADDR                    (SOC_BBP_COMM_BASE_ADDR + 0x9B0)

/* 寄存器说明：寄存器
   详      述：寄存器37，预留。供DSP之间传递信息。
   UNION结构 ：无 */
#define BBPCOMM_REG37_FOR_USE_ADDR                    (SOC_BBP_COMM_BASE_ADDR + 0x9B4)

/* 寄存器说明：寄存器
   详      述：寄存器38，预留。供DSP之间传递信息。
   UNION结构 ：无 */
#define BBPCOMM_REG38_FOR_USE_ADDR                    (SOC_BBP_COMM_BASE_ADDR + 0x9B8)

/* 寄存器说明：寄存器
   详      述：寄存器39，预留。供DSP之间传递信息。
   UNION结构 ：无 */
#define BBPCOMM_REG39_FOR_USE_ADDR                    (SOC_BBP_COMM_BASE_ADDR + 0x9BC)

/* 寄存器说明：寄存器
   详      述：寄存器40，预留。供DSP之间传递信息。
   UNION结构 ：无 */
#define BBPCOMM_REG40_FOR_USE_ADDR                    (SOC_BBP_COMM_BASE_ADDR + 0x9C0)

/* 寄存器说明：寄存器
   详      述：寄存器41，预留。供DSP之间传递信息。
   UNION结构 ：无 */
#define BBPCOMM_REG41_FOR_USE_ADDR                    (SOC_BBP_COMM_BASE_ADDR + 0x9C4)

/* 寄存器说明：寄存器
   详      述：寄存器42，预留。供DSP之间传递信息。
   UNION结构 ：无 */
#define BBPCOMM_REG42_FOR_USE_ADDR                    (SOC_BBP_COMM_BASE_ADDR + 0x9C8)

/* 寄存器说明：寄存器
   详      述：寄存器43，预留。供DSP之间传递信息。
   UNION结构 ：无 */
#define BBPCOMM_REG43_FOR_USE_ADDR                    (SOC_BBP_COMM_BASE_ADDR + 0x9CC)

/* 寄存器说明：寄存器
   详      述：寄存器44，预留。供DSP之间传递信息。
   UNION结构 ：无 */
#define BBPCOMM_REG44_FOR_USE_ADDR                    (SOC_BBP_COMM_BASE_ADDR + 0x9D0)

/* 寄存器说明：寄存器
   详      述：寄存器45，预留。供DSP之间传递信息。
   UNION结构 ：无 */
#define BBPCOMM_REG45_FOR_USE_ADDR                    (SOC_BBP_COMM_BASE_ADDR + 0x9D4)

/* 寄存器说明：寄存器
   详      述：寄存器46，预留。供DSP之间传递信息。
   UNION结构 ：无 */
#define BBPCOMM_REG46_FOR_USE_ADDR                    (SOC_BBP_COMM_BASE_ADDR + 0x9D8)

/* 寄存器说明：寄存器
   详      述：寄存器47，预留。供DSP之间传递信息。
   UNION结构 ：无 */
#define BBPCOMM_REG47_FOR_USE_ADDR                    (SOC_BBP_COMM_BASE_ADDR + 0x9DC)

/* 寄存器说明：寄存器
   详      述：
   UNION结构 ：无 */
#define BBPCOMM_DBG_REG0_CPU_ADDR                     (SOC_BBP_COMM_BASE_ADDR + 0x9E0)

/* 寄存器说明：寄存器
   详      述：
   UNION结构 ：无 */
#define BBPCOMM_DBG_REG1_CPU_ADDR                     (SOC_BBP_COMM_BASE_ADDR + 0x9E4)

/* 寄存器说明：寄存器
   详      述：
   UNION结构 ：无 */
#define BBPCOMM_DBG_REG2_CPU_ADDR                     (SOC_BBP_COMM_BASE_ADDR + 0x9E8)

/* 寄存器说明：寄存器
   详      述：
   UNION结构 ：无 */
#define BBPCOMM_DBG_REG3_CPU_ADDR                     (SOC_BBP_COMM_BASE_ADDR + 0x9EC)

/* 寄存器说明：寄存器
   详      述：
   UNION结构 ：无 */
#define BBPCOMM_DBG_REG4_CPU_ADDR                     (SOC_BBP_COMM_BASE_ADDR + 0x9F0)

/* 寄存器说明：寄存器
   详      述：
   UNION结构 ：无 */
#define BBPCOMM_DBG_REG5_CPU_ADDR                     (SOC_BBP_COMM_BASE_ADDR + 0x9F4)

/* 寄存器说明：寄存器
   详      述：
   UNION结构 ：无 */
#define BBPCOMM_DBG_REG6_CPU_ADDR                     (SOC_BBP_COMM_BASE_ADDR + 0x9F8)

/* 寄存器说明：寄存器
   详      述：
   UNION结构 ：无 */
#define BBPCOMM_DBG_REG7_CPU_ADDR                     (SOC_BBP_COMM_BASE_ADDR + 0x9FC)

/* 寄存器说明：寄存器
   详      述：
   UNION结构 ：无 */
#define BBPCOMM_DBG_REG8_CPU_ADDR                     (SOC_BBP_COMM_BASE_ADDR + 0xA00)

/* 寄存器说明：寄存器
   详      述：
   UNION结构 ：无 */
#define BBPCOMM_DBG_REG9_CPU_ADDR                     (SOC_BBP_COMM_BASE_ADDR + 0xA04)

/* 寄存器说明：寄存器
   详      述：
   UNION结构 ：无 */
#define BBPCOMM_DBG_REG10_CPU_ADDR                    (SOC_BBP_COMM_BASE_ADDR + 0xA08)

/* 寄存器说明：寄存器
   详      述：
   UNION结构 ：无 */
#define BBPCOMM_DBG_REG11_CPU_ADDR                    (SOC_BBP_COMM_BASE_ADDR + 0xA0C)

/* 寄存器说明：寄存器
   详      述：
   UNION结构 ：无 */
#define BBPCOMM_DBG_REG12_CPU_ADDR                    (SOC_BBP_COMM_BASE_ADDR + 0xA10)

/* 寄存器说明：寄存器
   详      述：
   UNION结构 ：无 */
#define BBPCOMM_DBG_REG13_CPU_ADDR                    (SOC_BBP_COMM_BASE_ADDR + 0xA14)

/* 寄存器说明：寄存器
   详      述：
   UNION结构 ：无 */
#define BBPCOMM_DBG_REG14_CPU_ADDR                    (SOC_BBP_COMM_BASE_ADDR + 0xA18)

/* 寄存器说明：寄存器
   详      述：
   UNION结构 ：无 */
#define BBPCOMM_DBG_REG15_CPU_ADDR                    (SOC_BBP_COMM_BASE_ADDR + 0xA1C)

/* 寄存器说明：寄存器
   详      述：
   UNION结构 ：无 */
#define BBPCOMM_DBG_REG16_CPU_ADDR                    (SOC_BBP_COMM_BASE_ADDR + 0xA20)

/* 寄存器说明：寄存器
   详      述：
   UNION结构 ：无 */
#define BBPCOMM_DBG_REG17_CPU_ADDR                    (SOC_BBP_COMM_BASE_ADDR + 0xA24)

/* 寄存器说明：寄存器
   详      述：
   UNION结构 ：无 */
#define BBPCOMM_DBG_REG18_CPU_ADDR                    (SOC_BBP_COMM_BASE_ADDR + 0xA28)

/* 寄存器说明：寄存器
   详      述：
   UNION结构 ：无 */
#define BBPCOMM_DBG_REG19_CPU_ADDR                    (SOC_BBP_COMM_BASE_ADDR + 0xA2C)

/* 寄存器说明：门控时钟配置寄存器
 bit[31:2]   保留
 bit[1:1]    bbp_comm_dbg模块时钟bypass配置信号：
             1：直接使用输入时钟，不进行门控处理，此时dsp_dbg_clk_gate_en配置无效；
             0：表示根据dsp_dbg_clk_gate_en进行门控处理。
 bit[0:0]    bbp_comm_dbg模块门控时钟使能信号：
             1：直接使用输入时钟，未进行门控；
             0：输入时钟无效，此时进行低功耗状态。
   UNION结构:  BBPCOMM_DBG_CLK_GATE_EN_UNION */
#define BBPCOMM_DBG_CLK_GATE_EN_ADDR                  (SOC_BBP_COMM_BASE_ADDR + 0xA30)

/* 寄存器说明：
 bit[31:24]  保留
 bit[23]     C MIPI0忙标志。
 bit[22]     C MIPI1忙标志。
 bit[21]     C SSI0忙标志。
 bit[20]     C SSI1忙标志。
 bit[19]     G1 MIPI0忙标志。
 bit[18]     G1 MIPI1忙标志。
 bit[17]     G2 MIPI0忙标志。
 bit[16]     G2 MIPI1忙标志。
 bit[15]     LTE MIPI0忙标志。
 bit[14]     LTE MIPI1忙标志。
 bit[13]     W MIPI0忙标志。
 bit[12]     W MIPI1忙标志。
 bit[11]     TDS MIPI0忙标志。
 bit[10]     TDS MIPI1忙标志。
 bit[9]      G1 SSI0忙标志。
 bit[8]      G1 SSI1忙标志。
 bit[7]      G2 SSI0忙标志。
 bit[6]      G2 SSI1忙标志。
 bit[5]      LTE SSI0忙标志。
 bit[4]      LTE SSI1忙标志。
 bit[3]      W SSI0忙标志。
 bit[2]      W SSI1忙标志。
 bit[1]      TDS SSI0忙标志。
 bit[0]      TDS SSI1忙标志。
   UNION结构:  BBPCOMM_DBG_REG20_CPU_UNION */
#define BBPCOMM_DBG_REG20_CPU_ADDR                    (SOC_BBP_COMM_BASE_ADDR + 0xA34)

/* 寄存器说明：寄存器
 bit[31:1]   保留
 bit[0:0]    bbp_comm_dbg使能信号：
             1：使能；
             0：不使能。
   UNION结构:  BBPCOMM_DBG_COMM_EN_UNION */
#define BBPCOMM_DBG_COMM_EN_ADDR                      (SOC_BBP_COMM_BASE_ADDR + 0xA38)

/* 寄存器说明：W为从模时对应通道的指示寄存器
 bit[31:2]   保留
 bit[1:1]    W为从模时对应的通道选择信号生效方式。
             0：在int1生效；
             1：立即生效。
 bit[0:0]    W为从模时对应的通道选择信号。
             0：选择通道0；
             1：选择通道1。
             配置时刻：各模根据通信场景配置
   UNION结构:  BBPCOMM_S_W_CH_NUM_IND_UNION */
#define BBPCOMM_S_W_CH_NUM_IND_ADDR                   (SOC_BBP_COMM_BASE_ADDR + 0xA40)

/* 寄存器说明：LTE为从模时对应通道的指示寄存器
 bit[31:2]   保留
 bit[1:1]    LTE为从模时对应的通道选择信号生效方式。
             0：在int1生效；
             1：立即生效。
 bit[0:0]    LTE为从模时对应的通道选择信号。
             0：选择通道0；
             1：选择通道1。
             配置时刻：各模根据通信场景配置
   UNION结构:  BBPCOMM_S_LTE_CH_NUM_IND_UNION */
#define BBPCOMM_S_LTE_CH_NUM_IND_ADDR                 (SOC_BBP_COMM_BASE_ADDR + 0xA44)

/* 寄存器说明：TDS为从模时对应通道的指示寄存器
 bit[31:2]   保留
 bit[1:1]    TDS为从模时对应的通道选择信号生效方式。
             0：在int1生效；
             1：立即生效。
 bit[0:0]    TDS为从模时对应的通道选择信号。
             0：选择通道0；
             1：选择通道1。
             配置时刻：各模根据通信场景配置
   UNION结构:  BBPCOMM_S_TDS_CH_NUM_IND_UNION */
#define BBPCOMM_S_TDS_CH_NUM_IND_ADDR                 (SOC_BBP_COMM_BASE_ADDR + 0xA48)

/* 寄存器说明：GM为从模时对应通道的指示寄存器
 bit[31:2]   保留
 bit[1:1]    GM为从模时对应的通道选择信号生效方式。
             0：在int1生效；
             1：立即生效。
 bit[0:0]    GM为从模时对应的通道选择信号。
             0：选择通道0；
             1：选择通道1。
             配置时刻：各模根据通信场景配置
   UNION结构:  BBPCOMM_S_GM_CH_NUM_IND_UNION */
#define BBPCOMM_S_GM_CH_NUM_IND_ADDR                  (SOC_BBP_COMM_BASE_ADDR + 0xA4C)

/* 寄存器说明：GS为从模时对应通道的指示寄存器
 bit[31:2]   保留
 bit[1:1]    GS为从模时对应的通道选择信号生效方式。
             0：在int1生效；
             1：立即生效。
 bit[0:0]    GS为从模时对应的通道选择信号。
             0：选择通道0；
             1：选择通道1。
             配置时刻：各模根据通信场景配置
   UNION结构:  BBPCOMM_S_GS_CH_NUM_IND_UNION */
#define BBPCOMM_S_GS_CH_NUM_IND_ADDR                  (SOC_BBP_COMM_BASE_ADDR + 0xA50)

/* 寄存器说明：
 bit[31:6]   保留
 bit[5]      apt相关信号是否交换：
             1'b0：0/1通道不交换
             1'b1：0/1通道交换
 bit[4]      pmu相关信号是否交换：
             1'b0：0/1通道不交换
             1'b1：0/1通道交换
 bit[3]      abb rx相关信号是否交换：
             1'b0：0/1通道不交换
             1'b1：0/1通道交换
 bit[2]      abb tx相关信号是否交换：
             1'b0：0/1通道不交换
             1'b1：0/1通道交换
 bit[1]      mipi相关信号是否交换：
             1'b0：0/1通道不交换
             1'b1：0/1通道交换
 bit[0]      rf相关信号是否交换：
             1'b0：0/1通道不交换
             1'b1：0/1通道交换
   UNION结构:  BBPCOMM_L_CH_SW_UNION */
#define BBPCOMM_L_CH_SW_ADDR                          (SOC_BBP_COMM_BASE_ADDR + 0xB00)

/* 寄存器说明：
 bit[31:6]   保留
 bit[5]      apt相关信号是否交换：
             1'b0：0/1通道不交换
             1'b1：0/1通道交换
 bit[4]      pmu相关信号是否交换：
             1'b0：0/1通道不交换
             1'b1：0/1通道交换
 bit[3]      abb rx相关信号是否交换：
             1'b0：0/1通道不交换
             1'b1：0/1通道交换
 bit[2]      abb tx相关信号是否交换：
             1'b0：0/1通道不交换
             1'b1：0/1通道交换
 bit[1]      mipi相关信号是否交换：
             1'b0：0/1通道不交换
             1'b1：0/1通道交换
 bit[0]      rf相关信号是否交换：
             1'b0：0/1通道不交换
             1'b1：0/1通道交换
   UNION结构:  BBPCOMM_W_CH_SW_UNION */
#define BBPCOMM_W_CH_SW_ADDR                          (SOC_BBP_COMM_BASE_ADDR + 0xB04)

/* 寄存器说明：
 bit[31:6]   保留
 bit[5]      apt相关信号是否交换：
             1'b0：0/1通道不交换
             1'b1：0/1通道交换
 bit[4]      pmu相关信号是否交换：
             1'b0：0/1通道不交换
             1'b1：0/1通道交换
 bit[3]      abb rx相关信号是否交换：
             1'b0：0/1通道不交换
             1'b1：0/1通道交换
 bit[2]      abb tx相关信号是否交换：
             1'b0：0/1通道不交换
             1'b1：0/1通道交换
 bit[1]      mipi相关信号是否交换：
             1'b0：0/1通道不交换
             1'b1：0/1通道交换
 bit[0]      rf相关信号是否交换：
             1'b0：0/1通道不交换
             1'b1：0/1通道交换
   UNION结构:  BBPCOMM_T_CH_SW_UNION */
#define BBPCOMM_T_CH_SW_ADDR                          (SOC_BBP_COMM_BASE_ADDR + 0xB08)

/* 寄存器说明：
 bit[31:6]   保留
 bit[5]      apt相关信号是否交换：
             1'b0：0/1通道不交换
             1'b1：0/1通道交换
 bit[4]      pmu相关信号是否交换：
             1'b0：0/1通道不交换
             1'b1：0/1通道交换
 bit[3]      abb rx相关信号是否交换：
             1'b0：0/1通道不交换
             1'b1：0/1通道交换
 bit[2]      abb tx相关信号是否交换：
             1'b0：0/1通道不交换
             1'b1：0/1通道交换
 bit[1]      mipi相关信号是否交换：
             1'b0：0/1通道不交换
             1'b1：0/1通道交换
 bit[0]      rf相关信号是否交换：
             1'b0：0/1通道不交换
             1'b1：0/1通道交换
   UNION结构:  BBPCOMM_GM_CH_SW_UNION */
#define BBPCOMM_GM_CH_SW_ADDR                         (SOC_BBP_COMM_BASE_ADDR + 0xB0C)

/* 寄存器说明：
 bit[31:6]   保留
 bit[5]      apt相关信号是否交换：
             1'b0：0/1通道不交换
             1'b1：0/1通道交换
 bit[4]      pmu相关信号是否交换：
             1'b0：0/1通道不交换
             1'b1：0/1通道交换
 bit[3]      abb rx相关信号是否交换：
             1'b0：0/1通道不交换
             1'b1：0/1通道交换
 bit[2]      abb tx相关信号是否交换：
             1'b0：0/1通道不交换
             1'b1：0/1通道交换
 bit[1]      mipi相关信号是否交换：
             1'b0：0/1通道不交换
             1'b1：0/1通道交换
 bit[0]      rf相关信号是否交换：
             1'b0：0/1通道不交换
             1'b1：0/1通道交换
   UNION结构:  BBPCOMM_GS_CH_SW_UNION */
#define BBPCOMM_GS_CH_SW_ADDR                         (SOC_BBP_COMM_BASE_ADDR + 0xB10)

/* 寄存器说明：
 bit[31:1]   保留
 bit[0]      lte模输出的通道0的tcvr_on信号来源：
             1'b0：来自于lte bbp
             1'b1：来自于dsp配置值
   UNION结构:  BBPCOMM_L_TCVR_SEL0_UNION */
#define BBPCOMM_L_TCVR_SEL0_ADDR                      (SOC_BBP_COMM_BASE_ADDR + 0xB20)

/* 寄存器说明：
 bit[31:1]   保留
 bit[0]      lte模输出的通道1的tcvr_on信号来源：
             1'b0：来自于lte bbp
             1'b1：来自于dsp配置值
   UNION结构:  BBPCOMM_L_TCVR_SEL1_UNION */
#define BBPCOMM_L_TCVR_SEL1_ADDR                      (SOC_BBP_COMM_BASE_ADDR + 0xB24)

/* 寄存器说明：
 bit[31:1]   保留
 bit[0]      tds模输出的通道0的tcvr_on信号来源：
             1'b0：来自于tds bbp
             1'b1：来自于dsp配置值
   UNION结构:  BBPCOMM_T_TCVR_SEL0_UNION */
#define BBPCOMM_T_TCVR_SEL0_ADDR                      (SOC_BBP_COMM_BASE_ADDR + 0xB28)

/* 寄存器说明：
 bit[31:1]   保留
 bit[0]      tds模输出的通道1的tcvr_on信号来源：
             1'b0：来自于tds bbp
             1'b1：来自于dsp配置值
   UNION结构:  BBPCOMM_T_TCVR_SEL1_UNION */
#define BBPCOMM_T_TCVR_SEL1_ADDR                      (SOC_BBP_COMM_BASE_ADDR + 0xB2C)

/* 寄存器说明：
 bit[31:1]   保留
 bit[0]      wcdma模输出的通道0的tcvr_on信号来源：
             1'b0：来自于wcdma bbp
             1'b1：来自于dsp配置值
   UNION结构:  BBPCOMM_W_TCVR_SEL0_UNION */
#define BBPCOMM_W_TCVR_SEL0_ADDR                      (SOC_BBP_COMM_BASE_ADDR + 0xB30)

/* 寄存器说明：
 bit[31:1]   保留
 bit[0]      wcdma模输出的通道1的tcvr_on信号来源：：
             1'b0：来自于wcdma bbp
             1'b1：来自于dsp配置值
   UNION结构:  BBPCOMM_W_TCVR_SEL1_UNION */
#define BBPCOMM_W_TCVR_SEL1_ADDR                      (SOC_BBP_COMM_BASE_ADDR + 0xB34)

/* 寄存器说明：
 bit[31:1]   保留
 bit[0]      gsm（m）输出的通道0的tcvr_on信号来源：
             1'b0：来自于gsm（m） bbp
             1'b1：来自于dsp配置值
   UNION结构:  BBPCOMM_GM_TCVR_SEL0_UNION */
#define BBPCOMM_GM_TCVR_SEL0_ADDR                     (SOC_BBP_COMM_BASE_ADDR + 0xB38)

/* 寄存器说明：
 bit[31:1]   保留
 bit[0]      gsm（m）输出的通道1的tcvr_on信号来源：
             1'b0：来自于gsm（m） bbp
             1'b1：来自于dsp配置值
   UNION结构:  BBPCOMM_GM_TCVR_SEL1_UNION */
#define BBPCOMM_GM_TCVR_SEL1_ADDR                     (SOC_BBP_COMM_BASE_ADDR + 0xB3C)

/* 寄存器说明：
 bit[31:1]   保留
 bit[0]      gsm（s）输出的通道0的tcvr_on信号来源：
             1'b0：来自于gsm（s） bbp
             1'b1：来自于dsp配置值
   UNION结构:  BBPCOMM_GS_TCVR_SEL0_UNION */
#define BBPCOMM_GS_TCVR_SEL0_ADDR                     (SOC_BBP_COMM_BASE_ADDR + 0xB40)

/* 寄存器说明：
 bit[31:1]   保留
 bit[0]      gsm（s）输出的通道1的tcvr_on信号来源：
             1'b0：来自于gsm（s） bbp
             1'b1：来自于dsp配置值
   UNION结构:  BBPCOMM_GS_TCVR_SEL1_UNION */
#define BBPCOMM_GS_TCVR_SEL1_ADDR                     (SOC_BBP_COMM_BASE_ADDR + 0xB44)

/* 寄存器说明：
 bit[31:1]   保留
 bit[0]      dsp配置的lte模送给通道0 的tcvr_on值
             
   UNION结构:  BBPCOMM_L_TCVR_VALUE0_UNION */
#define BBPCOMM_L_TCVR_VALUE0_ADDR                    (SOC_BBP_COMM_BASE_ADDR + 0xB48)

/* 寄存器说明：
 bit[31:1]   保留
 bit[0]      dsp配置的lte模送给通道1 的tcvr_on值
             
   UNION结构:  BBPCOMM_L_TCVR_VALUE1_UNION */
#define BBPCOMM_L_TCVR_VALUE1_ADDR                    (SOC_BBP_COMM_BASE_ADDR + 0xB4C)

/* 寄存器说明：
 bit[31:1]   保留
 bit[0]      dsp配置的tds模送给通道0 的tcvr_on值
             
   UNION结构:  BBPCOMM_T_TCVR_VALUE0_UNION */
#define BBPCOMM_T_TCVR_VALUE0_ADDR                    (SOC_BBP_COMM_BASE_ADDR + 0xB50)

/* 寄存器说明：
 bit[31:1]   保留
 bit[0]      dsp配置的tds模送给通道1 的tcvr_on值
             
   UNION结构:  BBPCOMM_T_TCVR_VALUE1_UNION */
#define BBPCOMM_T_TCVR_VALUE1_ADDR                    (SOC_BBP_COMM_BASE_ADDR + 0xB54)

/* 寄存器说明：
 bit[31:1]   保留
 bit[0]      dsp配置的wcdma模送给通道0 的tcvr_on值
             
   UNION结构:  BBPCOMM_W_TCVR_VALUE0_UNION */
#define BBPCOMM_W_TCVR_VALUE0_ADDR                    (SOC_BBP_COMM_BASE_ADDR + 0xB58)

/* 寄存器说明：
 bit[31:1]   保留
 bit[0]      dsp配置的wcdma模送给通道1 的tcvr_on值
             
   UNION结构:  BBPCOMM_W_TCVR_VALUE1_UNION */
#define BBPCOMM_W_TCVR_VALUE1_ADDR                    (SOC_BBP_COMM_BASE_ADDR + 0xB5C)

/* 寄存器说明：
 bit[31:1]   保留
 bit[0]      dsp配置的gsm（m）模送给通道0 的tcvr_on值
             
   UNION结构:  BBPCOMM_GM_TCVR_VALUE0_UNION */
#define BBPCOMM_GM_TCVR_VALUE0_ADDR                   (SOC_BBP_COMM_BASE_ADDR + 0xB60)

/* 寄存器说明：
 bit[31:1]   保留
 bit[0]      dsp配置的gsm（m）模送给通道1的tcvr_on值
             
   UNION结构:  BBPCOMM_GM_TCVR_VALUE1_UNION */
#define BBPCOMM_GM_TCVR_VALUE1_ADDR                   (SOC_BBP_COMM_BASE_ADDR + 0xB64)

/* 寄存器说明：
 bit[31:1]   保留
 bit[0]      dsp配置的gsm（s）模送给通道0 的tcvr_on值
             
   UNION结构:  BBPCOMM_GS_TCVR_VALUE0_UNION */
#define BBPCOMM_GS_TCVR_VALUE0_ADDR                   (SOC_BBP_COMM_BASE_ADDR + 0xB68)

/* 寄存器说明：
 bit[31:1]   保留
 bit[0]      dsp配置的gsm（s）模送给通道1 的tcvr_on值
             
   UNION结构:  BBPCOMM_GS_TCVR_VALUE1_UNION */
#define BBPCOMM_GS_TCVR_VALUE1_ADDR                   (SOC_BBP_COMM_BASE_ADDR + 0xB6C)

/* 寄存器说明：
 bit[31:18]  保留
 bit[17]     通道1 最终输出的tcvr_on信号状态
             
 bit[16]     通道0 最终输出的tcvr_on信号状态
             
 bit[15:11]  保留
 bit[10]     c bbp输入的通道0 tcvr_on信号状态
             
 bit[9]      保留
 bit[8]      gsm（s） bbp输入的通道0 tcvr_on信号状态
             
 bit[7]      保留
 bit[6]      gsm（m） bbp输入的通道0 tcvr_on信号状态
             
 bit[5]      保留
 bit[4]      wcdma bbp输入的通道0 tcvr_on信号状态
             
 bit[3]      保留
 bit[2]      tds bbp输入的通道0 tcvr_on信号状态
             
 bit[1]      lte bbp输入的通道1 tcvr_on信号状态
             
 bit[0]      lte bbp输入的通道0 tcvr_on信号状态
             
   UNION结构:  BBPCOMM_RF_TCVR_STATE_UNION */
#define BBPCOMM_RF_TCVR_STATE_ADDR                    (SOC_BBP_COMM_BASE_ADDR + 0xB70)

/* 寄存器说明：
 bit[31:1]   保留
 bit[0]      mipi通道不绑定使能：
             1'b0：通道绑定
             1'b1：通道不绑定
   UNION结构:  BBPCOMM_MIPI_UNBIND_EN_UNION */
#define BBPCOMM_MIPI_UNBIND_EN_ADDR                   (SOC_BBP_COMM_BASE_ADDR + 0xB90)

/* 寄存器说明：
 bit[31:1]   保留
 bit[0]      通道0映射mipi器件关系：
             1'b0：通道0映射到mipi0
             1'b1：通道0映射到mipi1
   UNION结构:  BBPCOMM_MIPI_CH_SEL0_UNION */
#define BBPCOMM_MIPI_CH_SEL0_ADDR                     (SOC_BBP_COMM_BASE_ADDR + 0xB94)

/* 寄存器说明：
 bit[31:1]   保留
 bit[0]      通道1映射mipi器件关系：
             1'b0：通道1映射到mipi0
             1'b1：通道1映射到mipi1
   UNION结构:  BBPCOMM_MIPI_CH_SEL1_UNION */
#define BBPCOMM_MIPI_CH_SEL1_ADDR                     (SOC_BBP_COMM_BASE_ADDR + 0xB98)

/* 寄存器说明：
 bit[31:1]   保留
 bit[0]      保留
   UNION结构:  BBPCOMM_APT_CH_SEL0_UNION */
#define BBPCOMM_APT_CH_SEL0_ADDR                      (SOC_BBP_COMM_BASE_ADDR + 0xB9C)

/* 寄存器说明：
 bit[31:1]   保留
 bit[0]      保留
   UNION结构:  BBPCOMM_APT_CH_SEL1_UNION */
#define BBPCOMM_APT_CH_SEL1_ADDR                      (SOC_BBP_COMM_BASE_ADDR + 0xBA0)

/* 寄存器说明：
 bit[31:1]   保留
 bit[0]      afc pdm0信号来源：
             1'b0：来自于通道0
             1'b1：来自于通道1
   UNION结构:  BBPCOMM_AFC_CH_SEL0_UNION */
#define BBPCOMM_AFC_CH_SEL0_ADDR                      (SOC_BBP_COMM_BASE_ADDR + 0xBA4)

/* 寄存器说明：
 bit[31:1]   保留
 bit[0]      afc pdm1信号来源：
             1'b0：来自于通道0
             1'b1：来自于通道1
   UNION结构:  BBPCOMM_AFC_CH_SEL1_UNION */
#define BBPCOMM_AFC_CH_SEL1_ADDR                      (SOC_BBP_COMM_BASE_ADDR + 0xBA8)

/* 寄存器说明：
 bit[31:1]   保留
 bit[0]      保留
   UNION结构:  BBPCOMM_ABB_TX_CH_SEL0_UNION */
#define BBPCOMM_ABB_TX_CH_SEL0_ADDR                   (SOC_BBP_COMM_BASE_ADDR + 0xBAC)

/* 寄存器说明：
 bit[31:1]   保留
 bit[0]      保留
   UNION结构:  BBPCOMM_ABB_TX_CH_SEL1_UNION */
#define BBPCOMM_ABB_TX_CH_SEL1_ADDR                   (SOC_BBP_COMM_BASE_ADDR + 0xBB0)

/* 寄存器说明：
 bit[31:1]   保留
 bit[0]      保留
   UNION结构:  BBPCOMM_PMU_CH_SEL0_UNION */
#define BBPCOMM_PMU_CH_SEL0_ADDR                      (SOC_BBP_COMM_BASE_ADDR + 0xBB4)

/* 寄存器说明：
 bit[31:1]   保留
 bit[0]      保留
   UNION结构:  BBPCOMM_PMU_CH_SEL1_UNION */
#define BBPCOMM_PMU_CH_SEL1_ADDR                      (SOC_BBP_COMM_BASE_ADDR + 0xBB8)

/* 寄存器说明：
 bit[31:16]  保留
 bit[15:0]   保留
   UNION结构:  BBPCOMM_VALID_NUM_UNION */
#define BBPCOMM_VALID_NUM_ADDR                        (SOC_BBP_COMM_BASE_ADDR + 0xBC0)

/* 寄存器说明：
 bit[31:16]  保留
 bit[15:0]   保留
   UNION结构:  BBPCOMM_VALID_NUM1_UNION */
#define BBPCOMM_VALID_NUM1_ADDR                       (SOC_BBP_COMM_BASE_ADDR + 0xBC4)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:0]   逻辑自清零。
             中断标志清除指示，高有效。用于清除INT_TYPE_LTE_RCV_C[15:0]对应bit位
             生效时刻：立即生效
   UNION结构:  BBPCOMM_INT_CLEAR_LTE_RCV_C_UNION */
#define BBPCOMM_INT_CLEAR_LTE_RCV_C_ADDR              (SOC_BBP_COMM_BASE_ADDR + 0xC00)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:0]   中断屏蔽标志。用于屏蔽INT_TYPE_LTE_RCV_C[15:0]对应bit中断。
             0：屏蔽中断上报；
             1：使能中断上报。
             配置时刻：初始化
             生效时刻：立即生效
   UNION结构:  BBPCOMM_INT_MASK_LTE_RCV_C_UNION */
#define BBPCOMM_INT_MASK_LTE_RCV_C_ADDR               (SOC_BBP_COMM_BASE_ADDR + 0xC04)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:0]   中断状态标志。各bit单独对应一种中断类型，高有效。
             [0]: 中断类型0。 C（主模）提前通知LTE异系统测量启动中断。
             [1]: 中断类型1。 C（主模）进GAP中断，通知LTE进行异系统测量射频通道配置，天线切换等操作。
             [2]: 中断类型2。C（主模）出GAP中断。
             [3]: 中断类型3。C（从模）测量结束中断。
             [4]: 中断类型4。C（主模）强制C退出异系统测量中断。
             [5]: 中断类型5。C（主模）通知C上报本次测量结果。
             [6]: 保留。
             [15:7]: 对应中断类型[15:7]，预留。
             软件读取该中断状态上报寄存器后，需要和0x0A84的int_mask_lte_rcv_c相与，再判断是否需要处理。
   UNION结构:  BBPCOMM_INT_TYPE_LTE_RCV_C_UNION */
#define BBPCOMM_INT_TYPE_LTE_RCV_C_ADDR               (SOC_BBP_COMM_BASE_ADDR + 0xC08)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:3]   逻辑自清零。高有效。
             
             bit15~7：对应lte_snd_c_int_15 ~ 7。
             LPHY通过此寄存器触发类型15~7中断（int_type_c_rcv_lte[15:7]）给CPHY。
             生效时刻：立即生效
             
             bit6：对应lte_snd_c_int_6。
             LPHY通过此寄存器触发类型6中断（int_type_lte_rcv_c[6]）给LPHY。
             生效时刻：立即生效
             
             bit5：对应c_snd_lte_int_5。
             L主模时通知LPHY L测量周期到达，高有效。
             LPHY通过此寄存器触发类型5中断（int_type_c_rcv_lte[5]）给LPHY。
             生效时刻：立即生效
             
             bit4：对应lte_snd_c_int_4。
             L主模时强制退出异系统测量指示。高有效。
             LPHY通过此寄存器触发类型4中断（int_type_c_rcv_lte[4]）给CPHY。
             生效时刻：立即生效
             
             bit3：对应lte_snd_c_int_3。L测量结束指示，高有效。
             用于触发中断类型3 （int_type_c_rcv_lte[3]）给CPHY。C主模下对L进行异系统测量时，如果L测量结束，LPHY配置此标志为1。 
             生效时刻：立即生效
 bit[2:0]    保留
   UNION结构:  BBPCOMM_LTE_SND_C_INT_13_UNION */
#define BBPCOMM_LTE_SND_C_INT_13_ADDR                 (SOC_BBP_COMM_BASE_ADDR + 0xC0C)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:3]   逻辑自清零。高有效。
             
             bit15~7：对应c_snd_c_int_15 ~ 7。
             C DSP通过此寄存器触发类型15~7中断（int_type_c_rcv_c[15:7]）给C DSP。
             生效时刻：立即生效
             
             bit6：对应c_snd_c_int_6。
             CDSP通过此寄存器触发类型6中断（int_type_c_rcv_c[6]）给CDSP。
             生效时刻：立即生效
             
             bit5：对应c_snd_c_int_5。
             C主模时通知CDSP C测量周期到达，高有效。
             C DSP通过此寄存器触发类型5中断（int_type_c_rcv_c[5]）给CDSP。
             生效时刻：立即生效
             
             bit4：对应c_snd_c_int_4。
             C主模时强制退出异系统测量指示。高有效。
             C DSP通过此寄存器触发类型4中断（int_type_c_rcv_c[4]）给CDSP。
             生效时刻：立即生效
             
             bit3：对应c_snd_c_int_3。C测量结束指示，高有效。
             用于触发中断类型3 （int_type_c_rcv_c[3]）给CDSP。C主模下对C进行异系统测量时，如果C测量结束，C DSP配置此标志为1。 
             生效时刻：立即生效
 bit[2:0]    保留
   UNION结构:  BBPCOMM_C_SND_C_INT_13_UNION */
#define BBPCOMM_C_SND_C_INT_13_ADDR                   (SOC_BBP_COMM_BASE_ADDR + 0xC10)

/* 寄存器说明：寄存器
 bit[31:2]   保留
 bit[1:0]    电平信号，低有效。
             当C主模需要做异系统测量时需配置本寄存器为3，异系统测量结束后配置本寄存器为0。
             bit1:1’b0:
              让c_intrasys_valid输入无效，被强制为1。
             bit0:1’b0:
             c_intersys_ingap_pre_ind输入无效，被强制为0。
             bit[1:0] = 11b: 让c_intrasys_valid和c_intersys_ingap_pre_ind输入有效。
             配置时刻：TDS主模需要做异系统测量时需配置本寄存器为3，异系统测量结束后配置本寄存器为0。
             生效时刻：立即生效
   UNION结构:  BBPCOMM_CBBP_INPUT_MASK_UNION */
#define BBPCOMM_CBBP_INPUT_MASK_ADDR                  (SOC_BBP_COMM_BASE_ADDR + 0xC14)

/* 寄存器说明：寄存器
   详      述：W/TD/GM/L/GS为主模时写此寄存器，GS为从模时读取此寄存器。此寄存器由DSP之间进行读写，BBP仅提供接口。
   UNION结构 ：无 */
#define BBPCOMM_MEAS_FLAG_C_RD_ADDR                   (SOC_BBP_COMM_BASE_ADDR + 0xC18)

/* 寄存器说明：寄存器
   详      述：C为从模时写此寄存器，W/TD/GM/L/GS主模读取此寄存器。此寄存器由DSP之间进行读写，BBP仅提供接口。
   UNION结构 ：无 */
#define BBPCOMM_WAKE_FLAG_C_WR_ADDR                   (SOC_BBP_COMM_BASE_ADDR + 0xC1C)

/* 寄存器说明：寄存器
 bit[31:30]  保留
 bit[29:0]   C模定时信息上报。 
   UNION结构:  BBPCOMM_C_TIMING_DSP_UNION */
#define BBPCOMM_C_TIMING_DSP_ADDR                     (SOC_BBP_COMM_BASE_ADDR + 0xC20)

/* 寄存器说明：寄存器
 bit[31:2]   保留
 bit[1]      C主模下实际控制RF指示，电平信号。为1代表W控制RF配置通道，为0代表从模可以控制RF通道。用于单RFIC时射频控制相关信号的选通。
             c_intrasys_valid_delay的上升沿比c_intrasys_valid上升沿滞后，下降沿一致。
 bit[0]      C主模下控制RF指示，电平信号。
             1’b0: C放弃RF控制权。
             1’b1: C享有控制RF权（但实际配置RF需c_intrasys_valid_delay信号为1）。
   UNION结构:  BBPCOMM_C_INTRASYS_VALID_UNION */
#define BBPCOMM_C_INTRASYS_VALID_ADDR                 (SOC_BBP_COMM_BASE_ADDR + 0xC24)

/* 寄存器说明：寄存器
 bit[31:4]   保留
 bit[3:0]    异系统测量类型。
             4’b0：测量C。
             4’b1: C背景搜；
             其它预留。
             生效时刻：立即生效
   UNION结构:  BBPCOMM_C_INTERSYS_MEASURE_TYPE_UNION */
#define BBPCOMM_C_INTERSYS_MEASURE_TYPE_ADDR          (SOC_BBP_COMM_BASE_ADDR + 0xC28)

/* 寄存器说明：寄存器
 bit[31:1]   保留
 bit[0:0]    单频点测量周期到指示，L或者TDS/Gm/Gs/U作为主模，通知CPHY W测量的单频点周期到达。
             只在连接态下需要配置。
             电平信号，高有效。
             生效时刻：立即生效
   UNION结构:  BBPCOMM_C_MEASURE_REPORT_VALID_UNION */
#define BBPCOMM_C_MEASURE_REPORT_VALID_ADDR           (SOC_BBP_COMM_BASE_ADDR + 0xC2C)

/* 寄存器说明：寄存器
   详      述：预处理中断int0到int1之间的offset信息，单位us（向下取整）。。
            配置时刻：主模启动C从模异系统测量时配置
            生效时刻：立即生效
   UNION结构 ：无 */
#define BBPCOMM_C_PREINT_OFFSET_TIME_ADDR             (SOC_BBP_COMM_BASE_ADDR + 0xC30)

/* 寄存器说明：寄存器
   详      述：C作为从模时，主模分配给C用于异系统测量时间长度，告知CPHY。单位us，向下取整到us。
            配置时刻：主模启动C从模异系统测量时配置
            生效时刻：立即生效
   UNION结构 ：无 */
#define BBPCOMM_C_INTERSYS_MEASURE_TIME_ADDR          (SOC_BBP_COMM_BASE_ADDR + 0xC34)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:0]   逻辑自清零。
             中断标志清除指示，高有效。用于清除INT_TYPE_C_RCV_LTE[15:0]对应bit位
             生效时刻：立即生效
   UNION结构:  BBPCOMM_INT_CLEAR_C_RCV_LTE_UNION */
#define BBPCOMM_INT_CLEAR_C_RCV_LTE_ADDR              (SOC_BBP_COMM_BASE_ADDR + 0xC38)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:0]   逻辑自清零。
             中断标志清除指示，高有效。用于清除INT_TYPE_C_RCV_C[15:0]对应bit位
             生效时刻：立即生效
   UNION结构:  BBPCOMM_INT_CLEAR_C_RCV_C_UNION */
#define BBPCOMM_INT_CLEAR_C_RCV_C_ADDR                (SOC_BBP_COMM_BASE_ADDR + 0xC3C)

/* 寄存器说明：寄存器
 bit[31:3]   保留
 bit[2:0]    逻辑自清零。
             中断标志清除指示，高有效。用于清除INT_TYPE_C_INT012[2:0]对应bit位
             生效时刻：立即生效
   UNION结构:  BBPCOMM_INT_CLEAR_C_INT012_UNION */
#define BBPCOMM_INT_CLEAR_C_INT012_ADDR               (SOC_BBP_COMM_BASE_ADDR + 0xC40)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:0]   中断屏蔽标志。用于屏蔽INT_TYPE_C_RCV_LTE[15:0]对应bit中断。
             0：屏蔽中断上报；
             1：使能中断上报。
             配置时刻：初始化
             生效时刻：立即生效
   UNION结构:  BBPCOMM_INT_MASK_C_RCV_LTE_UNION */
#define BBPCOMM_INT_MASK_C_RCV_LTE_ADDR               (SOC_BBP_COMM_BASE_ADDR + 0xC44)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:0]   中断屏蔽标志。用于屏蔽INT_TYPE_C_RCV_C[15:0]对应bit中断。
             0：屏蔽中断上报；
             1：使能中断上报。
             配置时刻：初始化
             生效时刻：立即生效
   UNION结构:  BBPCOMM_INT_MASK_C_RCV_C_UNION */
#define BBPCOMM_INT_MASK_C_RCV_C_ADDR                 (SOC_BBP_COMM_BASE_ADDR + 0xC48)

/* 寄存器说明：寄存器
 bit[31:3]   保留
 bit[2:0]    中断屏蔽标志。用于屏蔽INT_TYPE_C_INT012[2:0]对应bit中断。
             0：屏蔽中断上报；
             1：使能中断上报。
             配置时刻：初始化
             生效时刻：立即生效
   UNION结构:  BBPCOMM_INT_MASK_C_INT012_UNION */
#define BBPCOMM_INT_MASK_C_INT012_ADDR                (SOC_BBP_COMM_BASE_ADDR + 0xC4C)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:0]   中断状态标志。各bit单独对应一种中断类型，高有效。
             [0]: 中断类型0。 LTE（主模）提前通知C异系统测量启动中断。
             [1]: 中断类型1。 LTE（主模）进GAP中断，通知C进行异系统测量射频通道配置，天线切换等操作。
             [2]: 中断类型2。 LTE（主模）出GAP中断。
             [3]: 中断类型3。LTE（从模）测量结束中断。
             [4]: 中断类型4。LTE（主模）强制C退出异系统测量中断。
             [5]: 中断类型5。LTE（主模）通知C上报本次测量结果。
             [6]: 保留。
             [15:7]: 对应中断类型[15:7]，预留。
             软件读取该中断状态上报寄存器后，需要和0x0B34的int_mask_c_rcv_lte相与，再判断是否需要处理。
   UNION结构:  BBPCOMM_INT_TYPE_C_RCV_LTE_UNION */
#define BBPCOMM_INT_TYPE_C_RCV_LTE_ADDR               (SOC_BBP_COMM_BASE_ADDR + 0xC50)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:0]   中断状态标志。各bit单独对应一种中断类型，高有效。
             [0]: 中断类型0。C（主模）提前通知LTE异频测量启动中断。
             [1]: 中断类型1。C（主模）进GAP中断，通知C进行异频测量射频通道配置，天线切换等操作。
             [2]: 中断类型2。C（主模）出GAP中断。
             [3]: 中断类型3。C（从模，即异频）测量结束中断。
             [4]: 中断类型4。C（主模）强制C退出异频测量中断。
             [5]: 中断类型5。C（主模）通知C上报本次测量结果。
             [6]: 中断类型6。保留。
             [15:7]: 对应中断类型[15:7]，预留。
             中断状态标志0，1，2只有当LTE处于主模时才能产生。
             软件读取该中断状态上报寄存器后，需要和0x0B38的int_mask_c_rcv_c相与，再判断是否需要处理。
   UNION结构:  BBPCOMM_INT_TYPE_C_RCV_C_UNION */
#define BBPCOMM_INT_TYPE_C_RCV_C_ADDR                 (SOC_BBP_COMM_BASE_ADDR + 0xC54)

/* 寄存器说明：寄存器
 bit[31:3]   保留
 bit[2:0]    增加主模产生int0、1、2的上报（(包括异频、异系统场景)），高有效。
             [0]: C主模时，CTU告诉通知CPHY产生int0中断。
             [1]: C主模时，CTU告诉通知CPHY产生int1中断。
             [2]:CW主模时，CTU告诉通知CPHY产生int2中断。
             软件读取该中断状态上报寄存器后，需要和0x0704的int_mask_c_int012相与，再判断是否需要处理。
   UNION结构:  BBPCOMM_INT_TYPE_C_INT012_UNION */
#define BBPCOMM_INT_TYPE_C_INT012_ADDR                (SOC_BBP_COMM_BASE_ADDR + 0xC58)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:3]   逻辑自清零。高有效。
             
             bit15~7：对应c_snd_lte_int_15 ~ 7。
             CPHY通过此寄存器触发类型15~7中断（int_type_lte_rcv_c[15:7]）给LPHY。
             生效时刻：立即生效
             
             bit6：对应c_snd_lte_int_6。
             CPHY通过此寄存器触发类型6中断（int_type_lte_rcv_c[6]）给LPHY。
             生效时刻：立即生效
             
             bit5：对应c_snd_lte_int_5。
             C主模时通知CPHY L测量周期到达，高有效。
             CPHY通过此寄存器触发类型5中断（int_type_lte_rcv_c[5]）给CPHY。
             生效时刻：立即生效
             
             bit4：对应c_snd_lte_int_4。
             C主模时强制退出异系统测量指示。高有效。
             CPHY通过此寄存器触发类型4中断（int_type_lte_rcv_c[4]）给LPHY。
             生效时刻：立即生效
             
             bit3：对应c_snd_lte_int_3。C测量结束指示，高有效。
             用于触发中断类型3 （int_type_lte_rcv_c[3]）给LPHY。L主模下对C进行异系统测量时，如果C测量结束，CPHY配置此标志为1。 
             生效时刻：立即生效
 bit[2:0]    保留
   UNION结构:  BBPCOMM_C_SND_LTE_INT_13_UNION */
#define BBPCOMM_C_SND_LTE_INT_13_ADDR                 (SOC_BBP_COMM_BASE_ADDR + 0xC5C)

/* 寄存器说明：寄存器
 bit[31:10]  保留
 bit[9:9]    1'b1：C的TX通路I路数据取反
             1'b0：C的TX通路I路数据不取反
 bit[8:8]    1'b1：C的TX通路Q路数据取反
             1'b0：C的TX通路Q路数据不取反
 bit[7:7]    1'b1：C的TX通路IQ路数据交换
             1'b0：C的TX通路IQ路数据不交换
             V3R3不支持IQ交换功能。
 bit[6:6]    1'b1：C的RXA通路I路数据取反
             1'b0：C的RXA通路I路数据不取反
 bit[5:5]    1'b1：C的RXA通路Q路数据取反
             1'b0：C的RXA通路Q路数据不取反
 bit[4:4]    1'b1：C的RXA通路IQ路数据交换
             1'b0：C的RXA通路IQ路数据不交换
             V3R3不支持IQ交换功能。
 bit[3:3]    1'b1：C的RXB通路I路数据取反
             1'b0：C的RXB通路I路数据不取反
 bit[2:2]    1'b1：C的RXB通路Q路数据取反
             1'b0：C的RXB通路Q路数据不取反
 bit[1:1]    1'b1：C的RXB通路IQ路数据交换
             1'b0：C的RXB通路IQ路数据不交换
             V3R3不支持IQ交换功能。
 bit[0:0]    保留
   UNION结构:  BBPCOMM_C_ABBIF_FMT_UNION */
#define BBPCOMM_C_ABBIF_FMT_ADDR                      (SOC_BBP_COMM_BASE_ADDR + 0xC60)

/* 寄存器说明：寄存器
 bit[31:1]   保留
 bit[0]      1'b0：TDS CTU中断输出至物理层；
             1'b1：C CTU中断输出至物理层。
   UNION结构:  BBPCOMM_TDS_C_CTU_INT_SEL_UNION */
#define BBPCOMM_TDS_C_CTU_INT_SEL_ADDR                (SOC_BBP_COMM_BASE_ADDR + 0xC64)

/* 寄存器说明：GS为从模时对应通道的指示寄存器
 bit[31:2]   保留
 bit[1:1]    C为从模时对应的通道选择信号生效方式。
             0：在int1生效；
             1：立即生效。
 bit[0:0]    C为从模时对应的通道选择信号。
             0：选择通道0；
             1：选择通道1。
             配置时刻：各模根据通信场景配置
   UNION结构:  BBPCOMM_S_C_CH_NUM_IND_UNION */
#define BBPCOMM_S_C_CH_NUM_IND_ADDR                   (SOC_BBP_COMM_BASE_ADDR + 0xC68)

/* 寄存器说明：
 bit[31:6]   保留
 bit[5]      apt相关信号是否交换：
             1'b0：0/1通道不交换
             1'b1：0/1通道交换
 bit[4]      pmu相关信号是否交换：
             1'b0：0/1通道不交换
             1'b1：0/1通道交换
 bit[3]      abb rx相关信号是否交换：
             1'b0：0/1通道不交换
             1'b1：0/1通道交换
 bit[2]      abb tx相关信号是否交换：
             1'b0：0/1通道不交换
             1'b1：0/1通道交换
 bit[1]      mipi相关信号是否交换：
             1'b0：0/1通道不交换
             1'b1：0/1通道交换
 bit[0]      rf相关信号是否交换：
             1'b0：0/1通道不交换
             1'b1：0/1通道交换
   UNION结构:  BBPCOMM_C_CH_SW_UNION */
#define BBPCOMM_C_CH_SW_ADDR                          (SOC_BBP_COMM_BASE_ADDR + 0xC6C)

/* 寄存器说明：
 bit[31:1]   保留
 bit[0]      C输出的通道0的tcvr_on信号来源：
             1'b0：来自于c bbp
             1'b1：来自于dsp配置值
   UNION结构:  BBPCOMM_C_TCVR_SEL0_UNION */
#define BBPCOMM_C_TCVR_SEL0_ADDR                      (SOC_BBP_COMM_BASE_ADDR + 0xC70)

/* 寄存器说明：
 bit[31:1]   保留
 bit[0]      C输出的通道1的tcvr_on信号来源：
             1'b0：来自于c bbp
             1'b1：来自于dsp配置值
   UNION结构:  BBPCOMM_C_TCVR_SEL1_UNION */
#define BBPCOMM_C_TCVR_SEL1_ADDR                      (SOC_BBP_COMM_BASE_ADDR + 0xC74)

/* 寄存器说明：
 bit[31:1]   保留
 bit[0]      dsp配置的c模送给通道0 的tcvr_on值
             
   UNION结构:  BBPCOMM_C_TCVR_VALUE0_UNION */
#define BBPCOMM_C_TCVR_VALUE0_ADDR                    (SOC_BBP_COMM_BASE_ADDR + 0xC78)

/* 寄存器说明：
 bit[31:1]   保留
 bit[0]      dsp配置的c模送给通道1 的tcvr_on值
             
   UNION结构:  BBPCOMM_C_TCVR_VALUE1_UNION */
#define BBPCOMM_C_TCVR_VALUE1_ADDR                    (SOC_BBP_COMM_BASE_ADDR + 0xC7C)

/* 寄存器说明：寄存器
   详      述：32K公共定时器的定时信息，由DSP 配置timing_get_ind触发上报。
            
            上报时刻：comm_32k_timing_valid为高时上报有效
   UNION结构 ：无 */
#define BBPCOMM_COMM_32K_TIMING_RPT1_ADDR             (SOC_BBP_COMM_BASE_ADDR + 0xF00)

/* 寄存器说明：寄存器
 bit[31:24]  保留
 bit[23:0]   32K校准定时器的定时信息，由DSP 配置timing_get_ind触发上报。
             测试用
             上报时刻：comm_32k_timing_valid为高时上报有效
   UNION结构:  BBPCOMM_COMM_32K_TIMING_RPT2_UNION */
#define BBPCOMM_COMM_32K_TIMING_RPT2_ADDR             (SOC_BBP_COMM_BASE_ADDR + 0xF04)

/* 寄存器说明：寄存器
 bit[31:17]  保留
 bit[16]     ABB CH0 RXB通道和CH1 RXB通道互锁功能使能信号，1表示有效
 bit[15:0]   ABB CH0 RXB和CH1 RXB通道互锁延迟计数值，即通道切换的互锁时间，计数周期为总线时钟（150M），配置值必须大于等于1。
   UNION结构:  BBPCOMM_ABB_RXB_LOCK_MODE_UNION */
#define BBPCOMM_ABB_RXB_LOCK_MODE_ADDR                (SOC_BBP_COMM_BASE_ADDR + 0xF08)

/* 寄存器说明：寄存器
 bit[31:16]  可维可测的包个数
 bit[15:2]   保留
 bit[1]      可维可测数据上报模式：
             'b0：触发后继续采样dbg_pkg_num个数据包后自动结束；
             'b1：只要dbg_en为1就上报，没有上报个数限制。
 bit[0]      可维可测使能信号，当dbg_rpt_mode为0时逻辑自清，否则需要写清。
             1'b0：可维可测功能不使能                             1'b1：可维可测功能使能
   UNION结构:  BBPCOMM_DBG_EN_UNION */
#define BBPCOMM_DBG_EN_ADDR                           (SOC_BBP_COMM_BASE_ADDR + 0xF80)

/* 寄存器说明：寄存器
   详      述：可维可测上报数据包ID号基址
   UNION结构 ：无 */
#define BBPCOMM_DBG_ID_ADDR                           (SOC_BBP_COMM_BASE_ADDR + 0xF84)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:0]   可维可测内部分组指示：
             1：上报ssi通道0数据
             2：上报ssi通道1数据
             3：上报mipi通道0数据
             4：上报mipi通道1数据
             其它：保留
   UNION结构:  BBPCOMM_DBG_FLT_UNION */
#define BBPCOMM_DBG_FLT_ADDR                          (SOC_BBP_COMM_BASE_ADDR + 0xF88)

/* 寄存器说明：寄存器
 bit[31:20]  保留
 bit[19:0]   可维可测轮询请求上报信号使能，dbg_req_en[19:0]对应reg[19:0],如dbg_req_en[19]为1，reg[19]请求有效，dbg_req_en[19]为0，reg[19]请求无效
   UNION结构:  BBPCOMM_DBG_REQ_EN_UNION */
#define BBPCOMM_DBG_REQ_EN_ADDR                       (SOC_BBP_COMM_BASE_ADDR + 0xF8C)

/* 寄存器说明：寄存器
 bit[31:2]   保留
 bit[1]      MIPI轮询请求清零信号，1：清除清零信号，0：无效
 bit[0]      MIPI通道动态切换使能，1：动态切换开启，0：动态切换关闭
   UNION结构:  BBPCOMM_MIPI_DYN_EXCHG_UNION */
#define BBPCOMM_MIPI_DYN_EXCHG_ADDR                   (SOC_BBP_COMM_BASE_ADDR + 0xF90)

/* 寄存器说明：寄存器
 bit[31:16]  保留
 bit[15:0]   dsp读取定时信息超时保护时间，默认为0x4b00（1ms）
   UNION结构:  BBPCOMM_TIMING_CLR_DELAY_CNT_UNION */
#define BBPCOMM_TIMING_CLR_DELAY_CNT_ADDR             (SOC_BBP_COMM_BASE_ADDR + 0xF94)

/* 寄存器说明：寄存器
 bit[31:1]   保留
 bit[0]      定时信息上报模式选择,
             0：只使用一套配置和上报接口获取定时信息
             1：使用2套配置和上报接口，bbe16和arm各用1套接口获取定时信息
   UNION结构:  BBPCOMM_TIMING_GET_MODE_UNION */
#define BBPCOMM_TIMING_GET_MODE_ADDR                  (SOC_BBP_COMM_BASE_ADDR + 0xF98)

/* 寄存器说明：寄存器
 bit[31:2]   保留
 bit[1]      逻辑自清零。
             32公共定时信息的标志清除，用于清除timing_valid1标志。
             配置时刻：DSP读取定时完毕后配置
             生效时刻：立即生效
             （因为32K定时信息的上报延时远大于其他系统的定时信息，所以单独处理）
             （timing_get_mode=0，1套查询接口；
             timing_get_mode=1，2套查询接口）
 bit[0]      逻辑自清零。
             定时标志清除，用于清除timing_valid1标志。
             配置时刻：DSP读取定时完毕后配置
             生效时刻：立即生效。
             （timing_get_mode=1，arm控制）
   UNION结构:  BBPCOMM_TIMING_CLEAR1_UNION */
#define BBPCOMM_TIMING_CLEAR1_ADDR                    (SOC_BBP_COMM_BASE_ADDR + 0xF9C)

/* 寄存器说明：寄存器
 bit[31:6]   保留
 bit[5:4]    表示timing_get_mode=1,arm读取c模bbp计数器结果1x、evdo还是base_cnt。   00：1x定时，01：evdo定时，10： base_cnt ，11：无效。
             timing_get_mode=0，不需配置，无意义
 bit[3:1]    保留
 bit[0]      逻辑自清零。
             timing_get_mode=1，arm需要获取定时关系时配置生效，CTU逻辑根据此信号锁存LTE/W/Gm/Gs/TDS/C的系统定时，CTU的内部定时供arm查询。
             配置时刻：软件需要锁存定时
             生效时刻：立即生效
   UNION结构:  BBPCOMM_TIMING_GET_IND1_UNION */
#define BBPCOMM_TIMING_GET_IND1_ADDR                  (SOC_BBP_COMM_BASE_ADDR + 0xF10)

/* 寄存器说明：寄存器
 bit[31:6]   保留
 bit[5:4]    表示o_timing_valid有效时，上报的计数器给arm的是c模bbp的1x、evdo还是base_cnt
             00：1x定时
             01： evdo定时
             10： basecent
 bit[3]      保留
 bit[2]      表示arm读取定时信息超时（在超时保护时间内未给出o_timing_clear信号）
 bit[1]      32K公共定时上报有效标志，高有效。
             DSP配置timing_get_ind1后，查询此标志，如果为高，则可以获取32K公共定时器的定时信息。
             DSP读取定时完毕后配置comm_32k_timing_clear1寄存器，comm_32k_timing_valid1拉低为无效。
             32K定时锁存稳定后拉高comm_32k_timing_valid1
             配置时刻：
             生效时刻：立即生效
             （timing_get_mode=1，arm控制）
 bit[0]      定时上报有效标志，高有效。
             DSP配置timing_get_ind1后，查询此标志，如果为高，则可以获取LTE，W，Gm，Gs, TDS的系统定时。
             DSP读取定时完毕后配置timing_clear1寄存器，timing_valid1拉低为无效。
             CTU检测到timing_get_ind1后固定延迟delay_cnt后拉高timing_valid1。
             
             配置时刻：
             生效时刻：立即生效
             （timing_get_mode=1，arm控制）
   UNION结构:  BBPCOMM_TINMG_VALID1_UNION */
#define BBPCOMM_TINMG_VALID1_ADDR                     (SOC_BBP_COMM_BASE_ADDR + 0xF14)

/* 寄存器说明：
 bit[31:2]   保留
 bit[1:0]    表示timing_get_mode=1时bbe读取W模bbp计数器结果sys_cnt还是base_cnt。00（二进制）：sys_cnt，01：base_cnt 10：无效 11：无效
             timing_get_mode=0，不需配置，无意义
   UNION结构:  BBPCOMM_TIMING_GET_W_SEL_UNION */
#define BBPCOMM_TIMING_GET_W_SEL_ADDR                 (SOC_BBP_COMM_BASE_ADDR + 0xF18)

/* 寄存器说明：
 bit[31:2]   保留
 bit[1:0]    表示o_timing_valid有效时，上报w模的计数器给bbe16的是sys_cnt还是base_cnt
             00（二进制）： sys_cnt
             01： basecent 10：无效  11：无效 
   UNION结构:  BBPCOMM_TIMING_VALID_W_SEL_UNION */
#define BBPCOMM_TIMING_VALID_W_SEL_ADDR               (SOC_BBP_COMM_BASE_ADDR + 0xF1C)

/* 寄存器说明：
 bit[31:2]   保留
 bit[1:0]    表示timing_get_mode=1时arm读取w模计数器结果sys_cnt还是base_cnt。   00（二进制）：sys_cnt，01：base_cnt， 10：无效 11 无效
             timing_get_mode=0时，不需配置，无意义
   UNION结构:  BBPCOMM_TIMING_GET_W_SEL1_UNION */
#define BBPCOMM_TIMING_GET_W_SEL1_ADDR                (SOC_BBP_COMM_BASE_ADDR + 0xF28)

/* 寄存器说明：
 bit[31:2]   保留
 bit[1:0]    表示o_timing_valid有效时，上报的计数器给arm的是w模bbp的sys_cnt还是base_cnt
             00：sys_cnt定时
             01：base_cnt定时 
             10：无效
             11：无效
             
   UNION结构:  BBPCOMM_TIMING_VALID_W_SEL1_UNION */
#define BBPCOMM_TIMING_VALID_W_SEL1_ADDR              (SOC_BBP_COMM_BASE_ADDR + 0xF2C)

/* 寄存器说明：
 bit[31:1]   保留
 bit[0]      comm控制sc_pll使能，1：通过comm控制sc_pll，0：不通过comm控制sc_pll
   UNION结构:  BBPCOMM_CPU_CFG_SC_EN_UNION */
#define BBPCOMM_CPU_CFG_SC_EN_ADDR                    (SOC_BBP_COMM_BASE_ADDR + 0xF30)

/* 寄存器说明：
 bit[31:1]   保留
 bit[0]      comm向sc_pll发送中断1使能，1：有效，0无效
   UNION结构:  BBPCOMM_CPU_INT1_CFG_SC_EN_UNION */
#define BBPCOMM_CPU_INT1_CFG_SC_EN_ADDR               (SOC_BBP_COMM_BASE_ADDR + 0xF34)

/* 寄存器说明：
 bit[31:1]   保留
 bit[0]      comm向sc_pll发送中断2使能，1：有效，0无效
   UNION结构:  BBPCOMM_CPU_INT2_CFG_SC_EN_UNION */
#define BBPCOMM_CPU_INT2_CFG_SC_EN_ADDR               (SOC_BBP_COMM_BASE_ADDR + 0xF38)

/* 寄存器说明：
 bit[31:1]   保留
 bit[0]      软件向sc_pll配置的主模的ind信号
   UNION结构:  BBPCOMM_CPU_PRO_SOFT_CFG_SC_IND_UNION */
#define BBPCOMM_CPU_PRO_SOFT_CFG_SC_IND_ADDR          (SOC_BBP_COMM_BASE_ADDR + 0xF3C)

/* 寄存器说明：
 bit[31:1]   保留
 bit[0]      软件向sc_pll配置的从模的ind信号
   UNION结构:  BBPCOMM_CPU_SEC_SOFT_CFG_SC_IND_UNION */
#define BBPCOMM_CPU_SEC_SOFT_CFG_SC_IND_ADDR          (SOC_BBP_COMM_BASE_ADDR + 0xF40)

/* 寄存器说明：
 bit[31:1]   保留
 bit[0]      1：根据通道1主从模产生配置sc_pll信号
             0：根据通道0主从模产生配置sc_pll信号
   UNION结构:  BBPCOMM_CPU_SC_CH_SEL_UNION */
#define BBPCOMM_CPU_SC_CH_SEL_ADDR                    (SOC_BBP_COMM_BASE_ADDR + 0xF44)

/* 寄存器说明：
 bit[31:1]   保留
 bit[0]      ctu中两套定时上报轮询请求清零信号，1：清除清零信号，0：无效
   UNION结构:  BBPCOMM_CTU_TIMING_ROBIN_CLR_IND_UNION */
#define BBPCOMM_CTU_TIMING_ROBIN_CLR_IND_ADDR         (SOC_BBP_COMM_BASE_ADDR + 0xF4C)

/* 寄存器说明：
   详      述：保留
   UNION结构 ：无 */
#define BBPCOMM_BBC_REV0_ADDR                         (SOC_BBP_COMM_BASE_ADDR + 0xFE0)

/* 寄存器说明：
   详      述：保留
   UNION结构 ：无 */
#define BBPCOMM_BBC_REV1_ADDR                         (SOC_BBP_COMM_BASE_ADDR + 0xFE4)

/* 寄存器说明：
   详      述：保留
   UNION结构 ：无 */
#define BBPCOMM_BBC_REV2_ADDR                         (SOC_BBP_COMM_BASE_ADDR + 0xFE8)

/* 寄存器说明：
   详      述：保留
   UNION结构 ：无 */
#define BBPCOMM_BBC_REV3_ADDR                         (SOC_BBP_COMM_BASE_ADDR + 0xFEC)

/* 寄存器说明：
   详      述：保留
   UNION结构 ：无 */
#define BBPCOMM_BBC_REV4_ADDR                         (SOC_BBP_COMM_BASE_ADDR + 0xFF0)

/* 寄存器说明：
 bit[31:3]   保留
 bit[2]      W SDR模式下译码时钟选择。0，译码采用122m时钟；1，译码采用245m时钟。
 bit[1]      W SDR模式开关使能。
 bit[0]      G SDR模式开关使能。
   UNION结构:  BBPCOMM_GU_SDR_EN_UNION */
#define BBPCOMM_GU_SDR_EN_ADDR                        (SOC_BBP_COMM_BASE_ADDR + 0xFFC)



/***======================================================================***
                     (2/5) register_define_bbpmst
 ***======================================================================***/
/* 寄存器说明：
 bit[31:3]   保留
 bit[2:0]    该写通道的工作模式：每个通道可以由最多3个不同时发生的数据流复用；bbpmst_wr_ch0_mode代表写通道0的工作模式
             1）3'b000 ：通道非使能，此时该通道向前级的BBP输出valid/ready为0，向后级的axi接口不再发出新的读/写请求，如果所有的读写通道都没有新请求了，且axi口以及总线被释放（是否被释放的状态在cpu接口有上报），软件结合该状态可启动门控或掉电流程
             2）3'b001：通道配置给第一组接口使用
             3）3'b010：通道配置给第二组接口使用
             4）3'b100：通道配置给第三组接口使用
             
   UNION结构:  BBPCOMM_BBPMST_WR_CH0_MODE_UNION */
#define BBPCOMM_BBPMST_WR_CH0_MODE_ADDR               (SOC_BBP_COMM_BASE_ADDR + 0xA800)

/* 寄存器说明：
 bit[31:3]   保留
 bit[2:0]    该写通道的输入时钟域选择：
             1）3’b001：该写通道的输入接口使用第一种时钟域
             2）3’b010：该写通道的输入接口使用第二种时钟域
             3）3’b100：该写通道的输入接口使用第三种时钟域
             在v8r1中的第一种接口时钟为：104MHz，为g模的工作时钟
             在v8r1中的第二种接口时钟为：122.88MHz，为C模以及U模中除译码外的逻辑的工作时钟，且各模的cpu接口也使用该时钟域
             在v8r1中的第三种接口时钟为：245.76MHz，为U模中译码的工作时钟
             
             
   UNION结构:  BBPCOMM_BBPMST_WR_CH0_CLK_SEL_UNION */
#define BBPCOMM_BBPMST_WR_CH0_CLK_SEL_ADDR            (SOC_BBP_COMM_BASE_ADDR + 0xA804)

/* 寄存器说明：
   详      述：写通道支持循环空间写，本寄存器为该通道在DDR中对应的循环地址空间的基地址（以Byte为单位）
   UNION结构 ：无 */
#define BBPCOMM_BBPMST_WR_CH0_BASE_ADDR_ADDR          (SOC_BBP_COMM_BASE_ADDR + 0xA808)

/* 寄存器说明：
   详      述：写通道支持循环空间写，本寄存器为该通道在DDR中对应的循环地址空间的结束地址（以Byte为单位）；软件确保基地址和结束地址之间的该空间必须是8byte的整数倍，当数据达到结束地址时master将自动绕回到基地址并写入新的数据
   UNION结构 ：无 */
#define BBPCOMM_BBPMST_WR_CH0_END_ADDR_ADDR           (SOC_BBP_COMM_BASE_ADDR + 0xA80C)

/* 寄存器说明：
 bit[31:4]   保留
 bit[3:0]    用Master方式完成DPA上报时，单个burst的类型，含义如下：
             burst_beat==0， burst1
             burst_beat==3， burst4
             burst_beat==7， burst8
             burst_beat==15，burst16
             如果决定使用burst8， 则此种配置下硬件在读写DDR时会固定向总线发出burst8请求，同时硬件支持数据块的尾部不是一个完整的burst8 
             
   UNION结构:  BBPCOMM_BBPMST_WR_CH0_BURST_BEAT_UNION */
#define BBPCOMM_BBPMST_WR_CH0_BURST_BEAT_ADDR         (SOC_BBP_COMM_BASE_ADDR + 0xA810)

/* 寄存器说明：
 bit[31:1]   保留
 bit[0]      本通道所选择使用的master口：
             1'b0：该通道使用master口1
             1'b1：该通道使用master口2
             
   UNION结构:  BBPCOMM_BBPMST_WR_CH0_AXI_SEL_UNION */
#define BBPCOMM_BBPMST_WR_CH0_AXI_SEL_ADDR            (SOC_BBP_COMM_BASE_ADDR + 0xA814)

/* 寄存器说明：
 bit[31:3]   保留
 bit[2:0]    该写通道的工作模式：每个通道可以由最多3个不同时发生的数据流复用；bbpmst_wr_ch0_mode代表写通道0的工作模式
             1）3'b000 ：通道非使能，此时该通道向前级的BBP输出valid/ready为0，向后级的axi接口不再发出新的读/写请求，如果所有的读写通道都没有新请求了，且axi口以及总线被释放（是否被释放的状态在cpu接口有上报），软件结合该状态可启动门控或掉电流程
             2）3'b001：通道配置给第一组接口使用
             3）3'b010：通道配置给第二组接口使用
             4）3'b100：通道配置给第三组接口使用
             
   UNION结构:  BBPCOMM_BBPMST_WR_CH1_MODE_UNION */
#define BBPCOMM_BBPMST_WR_CH1_MODE_ADDR               (SOC_BBP_COMM_BASE_ADDR + 0xA820)

/* 寄存器说明：
 bit[31:3]   保留
 bit[2:0]    该写通道的输入时钟域选择：
             1）3’b001：该写通道的输入接口使用第一种时钟域
             2）3’b010：该写通道的输入接口使用第二种时钟域
             3）3’b100：该写通道的输入接口使用第三种时钟域
             在v8r1中的第一种接口时钟为：104MHz，为g模的工作时钟
             在v8r1中的第二种接口时钟为：122.88MHz，为C模以及U模中除译码外的逻辑的工作时钟，且各模的cpu接口也使用该时钟域
             在v8r1中的第三种接口时钟为：245.76MHz，为U模中译码的工作时钟
             
             
   UNION结构:  BBPCOMM_BBPMST_WR_CH1_CLK_SEL_UNION */
#define BBPCOMM_BBPMST_WR_CH1_CLK_SEL_ADDR            (SOC_BBP_COMM_BASE_ADDR + 0xA824)

/* 寄存器说明：
   详      述：写通道支持循环空间写，本寄存器为该通道在DDR中对应的循环地址空间的基地址（以Byte为单位）
   UNION结构 ：无 */
#define BBPCOMM_BBPMST_WR_CH1_BASE_ADDR_ADDR          (SOC_BBP_COMM_BASE_ADDR + 0xA828)

/* 寄存器说明：
   详      述：写通道支持循环空间写，本寄存器为该通道在DDR中对应的循环地址空间的结束地址（以Byte为单位）；软件确保基地址和结束地址之间的该空间必须是8byte的整数倍，当数据达到结束地址时master将自动绕回到基地址并写入新的数据
   UNION结构 ：无 */
#define BBPCOMM_BBPMST_WR_CH1_END_ADDR_ADDR           (SOC_BBP_COMM_BASE_ADDR + 0xA82C)

/* 寄存器说明：
 bit[31:4]   保留
 bit[3:0]    用Master方式完成DPA上报时，单个burst的类型，含义如下：
             burst_beat==0， burst1
             burst_beat==3， burst4
             burst_beat==7， burst8
             burst_beat==15，burst16
             如果决定使用burst8， 则此种配置下硬件在读写DDR时会固定向总线发出burst8请求，同时硬件支持数据块的尾部不是一个完整的burst8 
             
   UNION结构:  BBPCOMM_BBPMST_WR_CH1_BURST_BEAT_UNION */
#define BBPCOMM_BBPMST_WR_CH1_BURST_BEAT_ADDR         (SOC_BBP_COMM_BASE_ADDR + 0xA830)

/* 寄存器说明：
 bit[31:1]   保留
 bit[0]      本通道所选择使用的master口：
             1'b0：该通道使用master口1
             1'b1：该通道使用master口2
   UNION结构:  BBPCOMM_BBPMST_WR_CH1_AXI_SEL_UNION */
#define BBPCOMM_BBPMST_WR_CH1_AXI_SEL_ADDR            (SOC_BBP_COMM_BASE_ADDR + 0xA834)

/* 寄存器说明：
 bit[31:3]   保留
 bit[2:0]    该写通道的工作模式：每个通道可以由最多3个不同时发生的数据流复用；bbpmst_wr_ch0_mode代表写通道0的工作模式
             1）3'b000 ：通道非使能，此时该通道向前级的BBP输出valid/ready为0，向后级的axi接口不再发出新的读/写请求，如果所有的读写通道都没有新请求了，且axi口以及总线被释放（是否被释放的状态在cpu接口有上报），软件结合该状态可启动门控或掉电流程
             2）3'b001：通道配置给第一组接口使用
             3）3'b010：通道配置给第二组接口使用
             4）3'b100：通道配置给第三组接口使用
             
   UNION结构:  BBPCOMM_BBPMST_WR_CH2_MODE_UNION */
#define BBPCOMM_BBPMST_WR_CH2_MODE_ADDR               (SOC_BBP_COMM_BASE_ADDR + 0xA840)

/* 寄存器说明：
 bit[31:3]   保留
 bit[2:0]    该写通道的输入时钟域选择：
             1）3’b001：该写通道的输入接口使用第一种时钟域
             2）3’b010：该写通道的输入接口使用第二种时钟域
             3）3’b100：该写通道的输入接口使用第三种时钟域
             在v8r1中的第一种接口时钟为：104MHz，为g模的工作时钟
             在v8r1中的第二种接口时钟为：122.88MHz，为C模以及U模中除译码外的逻辑的工作时钟，且各模的cpu接口也使用该时钟域
             在v8r1中的第三种接口时钟为：245.76MHz，为U模中译码的工作时钟
             
             
   UNION结构:  BBPCOMM_BBPMST_WR_CH2_CLK_SEL_UNION */
#define BBPCOMM_BBPMST_WR_CH2_CLK_SEL_ADDR            (SOC_BBP_COMM_BASE_ADDR + 0xA844)

/* 寄存器说明：
   详      述：写通道支持循环空间写，本寄存器为该通道在DDR中对应的循环地址空间的基地址（以Byte为单位）
   UNION结构 ：无 */
#define BBPCOMM_BBPMST_WR_CH2_BASE_ADDR_ADDR          (SOC_BBP_COMM_BASE_ADDR + 0xA848)

/* 寄存器说明：
   详      述：写通道支持循环空间写，本寄存器为该通道在DDR中对应的循环地址空间的结束地址（以Byte为单位）；软件确保基地址和结束地址之间的该空间必须是8byte的整数倍，当数据达到结束地址时master将自动绕回到基地址并写入新的数据
   UNION结构 ：无 */
#define BBPCOMM_BBPMST_WR_CH2_END_ADDR_ADDR           (SOC_BBP_COMM_BASE_ADDR + 0xA84C)

/* 寄存器说明：
 bit[31:4]   保留
 bit[3:0]    用Master方式完成DPA上报时，单个burst的类型，含义如下：burst_beat==0， burst1
             burst_beat==3， burst4
             burst_beat==7， burst8
             burst_beat==15，burst16
             如果决定使用burst8， 则此种配置下硬件在读写DDR时会固定向总线发出burst8请求，同时硬件支持数据块的尾部不是一个完整的burst8 
             
   UNION结构:  BBPCOMM_BBPMST_WR_CH2_BURST_BEAT_UNION */
#define BBPCOMM_BBPMST_WR_CH2_BURST_BEAT_ADDR         (SOC_BBP_COMM_BASE_ADDR + 0xA850)

/* 寄存器说明：
 bit[31:1]   保留
 bit[0]      本通道所选择使用的master口：
             1'b0：该通道使用master口1
             1'b1：该通道使用master口2
             
   UNION结构:  BBPCOMM_BBPMST_WR_CH2_AXI_SEL_UNION */
#define BBPCOMM_BBPMST_WR_CH2_AXI_SEL_ADDR            (SOC_BBP_COMM_BASE_ADDR + 0xA854)

/* 寄存器说明：
 bit[31:3]   保留
 bit[2:0]    该写通道的工作模式：每个通道可以由最多3个不同时发生的数据流复用；bbpmst_wr_ch0_mode代表写通道0的工作模式
             1）3'b000 ：通道非使能，此时该通道向前级的BBP输出valid/ready为0，向后级的axi接口不再发出新的读/写请求，如果所有的读写通道都没有新请求了，且axi口以及总线被释放（是否被释放的状态在cpu接口有上报），软件结合该状态可启动门控或掉电流程
             2）3'b001：通道配置给第一组接口使用
             3）3'b010：通道配置给第二组接口使用
             4）3'b100：通道配置给第三组接口使用
             
   UNION结构:  BBPCOMM_BBPMST_WR_CH3_MODE_UNION */
#define BBPCOMM_BBPMST_WR_CH3_MODE_ADDR               (SOC_BBP_COMM_BASE_ADDR + 0xA860)

/* 寄存器说明：
 bit[31:3]   保留
 bit[2:0]    该写通道的输入时钟域选择：
             1）3’b001：该写通道的输入接口使用第一种时钟域
             2）3’b010：该写通道的输入接口使用第二种时钟域
             3）3’b100：该写通道的输入接口使用第三种时钟域
             在v8r1中的第一种接口时钟为：104MHz，为g模的工作时钟
             在v8r1中的第二种接口时钟为：122.88MHz，为C模以及U模中除译码外的逻辑的工作时钟，且各模的cpu接口也使用该时钟域
             在v8r1中的第三种接口时钟为：245.76MHz，为U模中译码的工作时钟
             
             
   UNION结构:  BBPCOMM_BBPMST_WR_CH3_CLK_SEL_UNION */
#define BBPCOMM_BBPMST_WR_CH3_CLK_SEL_ADDR            (SOC_BBP_COMM_BASE_ADDR + 0xA864)

/* 寄存器说明：
   详      述：写通道支持循环空间写，本寄存器为该通道在DDR中对应的循环地址空间的基地址（以Byte为单位）
   UNION结构 ：无 */
#define BBPCOMM_BBPMST_WR_CH3_BASE_ADDR_ADDR          (SOC_BBP_COMM_BASE_ADDR + 0xA868)

/* 寄存器说明：
   详      述：写通道支持循环空间写，本寄存器为该通道在DDR中对应的循环地址空间的结束地址（以Byte为单位）；软件确保基地址和结束地址之间的该空间必须是8byte的整数倍，当数据达到结束地址时master将自动绕回到基地址并写入新的数据
   UNION结构 ：无 */
#define BBPCOMM_BBPMST_WR_CH3_END_ADDR_ADDR           (SOC_BBP_COMM_BASE_ADDR + 0xA86C)

/* 寄存器说明：
 bit[31:4]   保留
 bit[3:0]    用Master方式完成DPA上报时，单个burst的类型，含义如下：
             burst_beat==0， burst1
             burst_beat==3， burst4
             burst_beat==7， burst8
             burst_beat==15，burst16
             如果决定使用burst8， 则此种配置下硬件在读写DDR时会固定向总线发出burst8请求，同时硬件支持数据块的尾部不是一个完整的burst8 
             
   UNION结构:  BBPCOMM_BBPMST_WR_CH3_BURST_BEAT_UNION */
#define BBPCOMM_BBPMST_WR_CH3_BURST_BEAT_ADDR         (SOC_BBP_COMM_BASE_ADDR + 0xA870)

/* 寄存器说明：
 bit[31:1]   保留
 bit[0]      本通道所选择使用的master口：
             1'b0：该通道使用master口1
             1'b1：该通道使用master口2
             
   UNION结构:  BBPCOMM_BBPMST_WR_CH3_AXI_SEL_UNION */
#define BBPCOMM_BBPMST_WR_CH3_AXI_SEL_ADDR            (SOC_BBP_COMM_BASE_ADDR + 0xA874)

/* 寄存器说明：
 bit[31:3]   保留
 bit[2:0]    该写通道的工作模式：每个通道可以由最多3个不同时发生的数据流复用；bbpmst_wr_ch0_mode代表写通道0的工作模式
             1）3'b000 ：通道非使能，此时该通道向前级的BBP输出valid/ready为0，向后级的axi接口不再发出新的读/写请求，如果所有的读写通道都没有新请求了，且axi口以及总线被释放（是否被释放的状态在cpu接口有上报），软件结合该状态可启动门控或掉电流程
             2）3'b001：通道配置给第一组接口使用
             3）3'b010：通道配置给第二组接口使用
             4）3'b100：通道配置给第三组接口使用
             
   UNION结构:  BBPCOMM_BBPMST_WR_CH4_MODE_UNION */
#define BBPCOMM_BBPMST_WR_CH4_MODE_ADDR               (SOC_BBP_COMM_BASE_ADDR + 0xA880)

/* 寄存器说明：
 bit[31:3]   保留
 bit[2:0]    该写通道的输入时钟域选择：
             1）3’b001：该写通道的输入接口使用第一种时钟域
             2）3’b010：该写通道的输入接口使用第二种时钟域
             3）3’b100：该写通道的输入接口使用第三种时钟域
             在v8r1中的第一种接口时钟为：104MHz，为g模的工作时钟
             在v8r1中的第二种接口时钟为：122.88MHz，为C模以及U模中除译码外的逻辑的工作时钟，且各模的cpu接口也使用该时钟域
             在v8r1中的第三种接口时钟为：245.76MHz，为U模中译码的工作时钟
             
             
   UNION结构:  BBPCOMM_BBPMST_WR_CH4_CLK_SEL_UNION */
#define BBPCOMM_BBPMST_WR_CH4_CLK_SEL_ADDR            (SOC_BBP_COMM_BASE_ADDR + 0xA884)

/* 寄存器说明：
   详      述：写通道支持循环空间写，本寄存器为该通道在DDR中对应的循环地址空间的基地址（以Byte为单位）
   UNION结构 ：无 */
#define BBPCOMM_BBPMST_WR_CH4_BASE_ADDR_ADDR          (SOC_BBP_COMM_BASE_ADDR + 0xA888)

/* 寄存器说明：
   详      述：写通道支持循环空间写，本寄存器为该通道在DDR中对应的循环地址空间的结束地址（以Byte为单位）；软件确保基地址和结束地址之间的该空间必须是8byte的整数倍，当数据达到结束地址时master将自动绕回到基地址并写入新的数据
   UNION结构 ：无 */
#define BBPCOMM_BBPMST_WR_CH4_END_ADDR_ADDR           (SOC_BBP_COMM_BASE_ADDR + 0xA88C)

/* 寄存器说明：
 bit[31:4]   保留
 bit[3:0]    用Master方式完成DPA上报时，单个burst的类型，含义如下：
             burst_beat==0， burst1
             burst_beat==3， burst4
             burst_beat==7， burst8
             burst_beat==15，burst16
             如果决定使用burst8， 则此种配置下硬件在读写DDR时会固定向总线发出burst8请求，同时硬件支持数据块的尾部不是一个完整的burst8 
             
   UNION结构:  BBPCOMM_BBPMST_WR_CH4_BURST_BEAT_UNION */
#define BBPCOMM_BBPMST_WR_CH4_BURST_BEAT_ADDR         (SOC_BBP_COMM_BASE_ADDR + 0xA890)

/* 寄存器说明：
 bit[31:1]   保留
 bit[0]      本通道所选择使用的master口：
             1'b0：该通道使用master口1
             1'b1：该通道使用master口2
             
   UNION结构:  BBPCOMM_BBPMST_WR_CH4_AXI_SEL_UNION */
#define BBPCOMM_BBPMST_WR_CH4_AXI_SEL_ADDR            (SOC_BBP_COMM_BASE_ADDR + 0xA894)

/* 寄存器说明：
 bit[31:20]  保留
 bit[19]     读通道3的空闲/忙碌状态：
             1'b0：通道已排空，处于空闲态
             1'b1：通道处于忙碌状态
 bit[18]     读通道2的空闲/忙碌状态：
             1'b0：通道已排空，处于空闲态
             1'b1：通道处于忙碌状态
 bit[17]     读通道1的空闲/忙碌状态：
             1'b0：通道已排空，处于空闲态
             1'b1：通道处于忙碌状态
 bit[16]     读通道0的空闲/忙碌状态：
             1'b0：通道已排空，处于空闲态
             1'b1：通道处于忙碌状态
 bit[15:5]   保留
 bit[4]      写通道4的空闲/忙碌状态：
             1'b0：通道已排空，处于空闲态
             1'b1：通道处于忙碌状态
 bit[3]      写通道3的空闲/忙碌状态：
             1'b0：通道已排空，处于空闲态
             1'b1：通道处于忙碌状态
 bit[2]      写通道2的空闲/忙碌状态：
             1'b0：通道已排空，处于空闲态
             1'b1：通道处于忙碌状态
 bit[1]      写通道1的空闲/忙碌状态：
             1'b0：通道已排空，处于空闲态
             1'b1：通道处于忙碌状态
 bit[0]      写通道0的空闲/忙碌状态：
             1'b0：通道已排空，处于空闲态
             1'b1：通道处于忙碌状态
   UNION结构:  BBPCOMM_BBPMST_CH_BUSY_UNION */
#define BBPCOMM_BBPMST_CH_BUSY_ADDR                   (SOC_BBP_COMM_BASE_ADDR + 0xA898)

/* 寄存器说明：
 bit[31:3]   保留
 bit[2:0]    该读通道的工作模式：每个通道可以由最多3个不同时发生的数据流复用；bbpmst_rd_ch0_mode代表读通道0的工作模式
             1）3'b000 ：通道非使能，此时该通道向前级的BBP输出valid/ready为0，向后级的读写控制不再发出新的读/写请求，如果所有的读写通道都没有新请求，则master口被释放，总线状态在cpu接口上报，软件结合该状态可启动门控或掉电流程
             2）3'b001：通道配置给第一组接口使用
             3）3'b010：通道配置给第二组接口使用
             4）3'b100：通道配置给第三组接口使用
             
   UNION结构:  BBPCOMM_BBPMST_RD_CH0_MODE_UNION */
#define BBPCOMM_BBPMST_RD_CH0_MODE_ADDR               (SOC_BBP_COMM_BASE_ADDR + 0xA900)

/* 寄存器说明：
 bit[31:3]   保留
 bit[2:0]    该读通道的输入时钟域选择：
             1）3’b001：该读通道的输入接口使用第一种时钟域
             2）3’b010：该读通道的输入接口使用第二种时钟域
             3）3’b100：该读通道的输入接口使用第三种时钟域
             在v8r1中的第一种接口时钟为：104MHz，为g模的工作时钟
             在v8r1中的第二种接口时钟为：122.88MHz，为C模以及U模中除译码外的逻辑的工作时钟，且各模的cpu接口也使用该时钟域
             在v8r1中的第三种接口时钟为：245.76MHz，为U模中译码的工作时钟
             
   UNION结构:  BBPCOMM_BBPMST_RD_CH0_CLK_SEL_UNION */
#define BBPCOMM_BBPMST_RD_CH0_CLK_SEL_ADDR            (SOC_BBP_COMM_BASE_ADDR + 0xA904)

/* 寄存器说明：
   详      述：该读通道在DDR中对应的地址空间的基地址（以Byte为单位），读通道不支持循环读，无需配置空间结束地址，由于读取数据的长度也由软件决定，故软件需确保读通道对应的地址空间不被其他软件进程误操作
   UNION结构 ：无 */
#define BBPCOMM_BBPMST_RD_CH0_INIT_ADDR_ADDR          (SOC_BBP_COMM_BASE_ADDR + 0xA908)

/* 寄存器说明：
 bit[31:4]   保留
 bit[3:0]    用Master方式完成DPA上报时，单个burst的类型，含义如下：
             burst_beat==0， burst1
             burst_beat==3， burst4
             burst_beat==7， burst8
             burst_beat==15，burst16
             如果决定使用burst8， 则此种配置下硬件在读写DDR时会固定向总线发出burst8请求，同时硬件支持数据块的尾部不是一个完整的burst8 
             
   UNION结构:  BBPCOMM_BBPMST_RD_CH0_BURST_BEAT_UNION */
#define BBPCOMM_BBPMST_RD_CH0_BURST_BEAT_ADDR         (SOC_BBP_COMM_BASE_ADDR + 0xA90C)

/* 寄存器说明：
 bit[31:1]   保留
 bit[0]      本通道所选择使用的master口：
             1'b0：该通道使用master口1
             1'b1：该通道使用master口2
             
             
   UNION结构:  BBPCOMM_BBPMST_RD_CH0_AXI_SEL_UNION */
#define BBPCOMM_BBPMST_RD_CH0_AXI_SEL_ADDR            (SOC_BBP_COMM_BASE_ADDR + 0xA910)

/* 寄存器说明：
 bit[31:3]   保留
 bit[2:0]    该读通道的工作模式：每个通道可以由最多3个不同时发生的数据流复用；bbpmst_rd_ch0_mode代表读通道0的工作模式
             1）3'b000 ：通道非使能，此时该通道向前级的BBP输出valid/ready为0，向后级的读写控制不再发出新的读/写请求，如果所有的读写通道都没有新请求，则master口被释放，总线状态在cpu接口上报，软件结合该状态可启动门控或掉电流程
             2）3'b001：通道配置给第一组接口使用
             3）3'b010：通道配置给第二组接口使用
             4）3'b100：通道配置给第三组接口使用
             
   UNION结构:  BBPCOMM_BBPMST_RD_CH1_MODE_UNION */
#define BBPCOMM_BBPMST_RD_CH1_MODE_ADDR               (SOC_BBP_COMM_BASE_ADDR + 0xA920)

/* 寄存器说明：
 bit[31:3]   保留
 bit[2:0]    该读通道的输入时钟域选择：
             1）3’b001：该读通道的输入接口使用第一种时钟域
             2）3’b010：该读通道的输入接口使用第二种时钟域
             3）3’b100：该读通道的输入接口使用第三种时钟域
             在v8r1中的第一种接口时钟为：104MHz，为g模的工作时钟
             在v8r1中的第二种接口时钟为：122.88MHz，为C模以及U模中除译码外的逻辑的工作时钟，且各模的cpu接口也使用该时钟域
             在v8r1中的第三种接口时钟为：245.76MHz，为U模中译码的工作时钟
             
   UNION结构:  BBPCOMM_BBPMST_RD_CH1_CLK_SEL_UNION */
#define BBPCOMM_BBPMST_RD_CH1_CLK_SEL_ADDR            (SOC_BBP_COMM_BASE_ADDR + 0xA924)

/* 寄存器说明：
   详      述：该读通道在DDR中对应的地址空间的基地址（以Byte为单位），读通道不支持循环读，无需配置空间结束地址，由于读取数据的长度也由软件决定，故软件需确保读通道对应的地址空间不被其他软件进程误操作
   UNION结构 ：无 */
#define BBPCOMM_BBPMST_RD_CH1_INIT_ADDR_ADDR          (SOC_BBP_COMM_BASE_ADDR + 0xA928)

/* 寄存器说明：
 bit[31:4]   保留
 bit[3:0]    用Master方式完成DPA上报时，单个burst的类型，含义如下：
             burst_beat==0， burst1
             burst_beat==3， burst4
             burst_beat==7， burst8
             burst_beat==15，burst16
             如果决定使用burst8， 则此种配置下硬件在读写DDR时会固定向总线发出burst8请求，同时硬件支持数据块的尾部不是一个完整的burst8 
             
   UNION结构:  BBPCOMM_BBPMST_RD_CH1_BURST_BEAT_UNION */
#define BBPCOMM_BBPMST_RD_CH1_BURST_BEAT_ADDR         (SOC_BBP_COMM_BASE_ADDR + 0xA92C)

/* 寄存器说明：
 bit[31:1]   保留
 bit[0]      本通道所选择使用的master口：
             1'b0：该通道使用master口1
             1'b1：该通道使用master口2
             
   UNION结构:  BBPCOMM_BBPMST_RD_CH1_AXI_SEL_UNION */
#define BBPCOMM_BBPMST_RD_CH1_AXI_SEL_ADDR            (SOC_BBP_COMM_BASE_ADDR + 0xA930)

/* 寄存器说明：
 bit[31:3]   保留
 bit[2:0]    该读通道的工作模式：每个通道可以由最多3个不同时发生的数据流复用；bbpmst_rd_ch0_mode代表读通道0的工作模式
             1）3'b000 ：通道非使能，此时该通道向前级的BBP输出valid/ready为0，向后级的读写控制不再发出新的读/写请求，如果所有的读写通道都没有新请求，则master口被释放，总线状态在cpu接口上报，软件结合该状态可启动门控或掉电流程
             2）3'b001：通道配置给第一组接口使用
             3）3'b010：通道配置给第二组接口使用
             4）3'b100：通道配置给第三组接口使用
             
   UNION结构:  BBPCOMM_BBPMST_RD_CH2_MODE_UNION */
#define BBPCOMM_BBPMST_RD_CH2_MODE_ADDR               (SOC_BBP_COMM_BASE_ADDR + 0xA940)

/* 寄存器说明：
 bit[31:3]   保留
 bit[2:0]    该读通道的输入时钟域选择：
             1）3’b001：该读通道的输入接口使用第一种时钟域
             2）3’b010：该读通道的输入接口使用第二种时钟域
             3）3’b100：该读通道的输入接口使用第三种时钟域
             在v8r1中的第一种接口时钟为：104MHz，为g模的工作时钟
             在v8r1中的第二种接口时钟为：122.88MHz，为C模以及U模中除译码外的逻辑的工作时钟，且各模的cpu接口也使用该时钟域
             在v8r1中的第三种接口时钟为：245.76MHz，为U模中译码的工作时钟
             
   UNION结构:  BBPCOMM_BBPMST_RD_CH2_CLK_SEL_UNION */
#define BBPCOMM_BBPMST_RD_CH2_CLK_SEL_ADDR            (SOC_BBP_COMM_BASE_ADDR + 0xA944)

/* 寄存器说明：
   详      述：该读通道在DDR中对应的地址空间的基地址（以Byte为单位），读通道不支持循环读，无需配置空间结束地址，由于读取数据的长度也由软件决定，故软件需确保读通道对应的地址空间不被其他软件进程误操作
   UNION结构 ：无 */
#define BBPCOMM_BBPMST_RD_CH2_INIT_ADDR_ADDR          (SOC_BBP_COMM_BASE_ADDR + 0xA948)

/* 寄存器说明：
 bit[31:4]   保留
 bit[3:0]    用Master方式完成DPA上报时，单个burst的类型，含义如下：
             burst_beat==0， burst1
             burst_beat==3， burst4
             burst_beat==7， burst8
             burst_beat==15，burst16
             如果决定使用burst8， 则此种配置下硬件在读写DDR时会固定向总线发出burst8请求，同时硬件支持数据块的尾部不是一个完整的burst8 
             
   UNION结构:  BBPCOMM_BBPMST_RD_CH2_BURST_BEAT_UNION */
#define BBPCOMM_BBPMST_RD_CH2_BURST_BEAT_ADDR         (SOC_BBP_COMM_BASE_ADDR + 0xA94C)

/* 寄存器说明：
 bit[31:1]   保留
 bit[0]      本通道所选择使用的master口：
             1'b0：该通道使用master口1
             1'b1：该通道使用master口2
             
   UNION结构:  BBPCOMM_BBPMST_RD_CH2_AXI_SEL_UNION */
#define BBPCOMM_BBPMST_RD_CH2_AXI_SEL_ADDR            (SOC_BBP_COMM_BASE_ADDR + 0xA950)

/* 寄存器说明：
 bit[31:3]   保留
 bit[2:0]    该读通道的工作模式：每个通道可以由最多3个不同时发生的数据流复用；bbpmst_rd_ch0_mode代表读通道0的工作模式
             1）3'b000 ：通道非使能，此时该通道向前级的BBP输出valid/ready为0，向后级的读写控制不再发出新的读/写请求，如果所有的读写通道都没有新请求，则master口被释放，总线状态在cpu接口上报，软件结合该状态可启动门控或掉电流程
             2）3'b001：通道配置给第一组接口使用
             3）3'b010：通道配置给第二组接口使用
             4）3'b100：通道配置给第三组接口使用
             
   UNION结构:  BBPCOMM_BBPMST_RD_CH3_MODE_UNION */
#define BBPCOMM_BBPMST_RD_CH3_MODE_ADDR               (SOC_BBP_COMM_BASE_ADDR + 0xA960)

/* 寄存器说明：
 bit[31:3]   保留
 bit[2:0]    该读通道的输入时钟域选择：
             1）3’b001：该读通道的输入接口使用第一种时钟域
             2）3’b010：该读通道的输入接口使用第二种时钟域
             3）3’b100：该读通道的输入接口使用第三种时钟域
             在v8r1中的第一种接口时钟为：104MHz，为g模的工作时钟
             在v8r1中的第二种接口时钟为：122.88MHz，为C模以及U模中除译码外的逻辑的工作时钟，且各模的cpu接口也使用该时钟域
             在v8r1中的第三种接口时钟为：245.76MHz，为U模中译码的工作时钟
             
   UNION结构:  BBPCOMM_BBPMST_RD_CH3_CLK_SEL_UNION */
#define BBPCOMM_BBPMST_RD_CH3_CLK_SEL_ADDR            (SOC_BBP_COMM_BASE_ADDR + 0xA964)

/* 寄存器说明：
   详      述：该读通道在DDR中对应的地址空间的基地址（以Byte为单位），读通道不支持循环读，无需配置空间结束地址，由于读取数据的长度也由软件决定，故软件需确保读通道对应的地址空间不被其他软件进程误操作
   UNION结构 ：无 */
#define BBPCOMM_BBPMST_RD_CH3_INIT_ADDR_ADDR          (SOC_BBP_COMM_BASE_ADDR + 0xA968)

/* 寄存器说明：
 bit[31:4]   保留
 bit[3:0]    用Master方式完成DPA上报时，单个burst的类型，含义如下：
             burst_beat==0， burst1
             burst_beat==3， burst4
             burst_beat==7， burst8
             burst_beat==15，burst16
             如果决定使用burst8， 则此种配置下硬件在读写DDR时会固定向总线发出burst8请求，同时硬件支持数据块的尾部不是一个完整的burst8 
             
   UNION结构:  BBPCOMM_BBPMST_RD_CH3_BURST_BEAT_UNION */
#define BBPCOMM_BBPMST_RD_CH3_BURST_BEAT_ADDR         (SOC_BBP_COMM_BASE_ADDR + 0xA96C)

/* 寄存器说明：
 bit[31:1]   保留
 bit[0]      本通道所选择使用的master口：
             1'b0：该通道使用master口1
             1'b1：该通道使用master口2
             
   UNION结构:  BBPCOMM_BBPMST_RD_CH3_AXI_SEL_UNION */
#define BBPCOMM_BBPMST_RD_CH3_AXI_SEL_ADDR            (SOC_BBP_COMM_BASE_ADDR + 0xA970)

/* 寄存器说明：
 bit[31:2]   保留
 bit[1:0]    读通道0每个64比特读数据有效数据个数指示：0，表示每个64比特读数据含10个有效数据；1，表示每个64比特读数据含16个有效数据；2，表示每个64比特读数据含1个有效数据；
   UNION结构:  BBPCOMM_BBPMST_RD_CH0_BIT_WIDTH_IND_UNION */
#define BBPCOMM_BBPMST_RD_CH0_BIT_WIDTH_IND_ADDR      (SOC_BBP_COMM_BASE_ADDR + 0xA974)

/* 寄存器说明：
 bit[31:2]   保留
 bit[1:0]    读通道1每个64比特读数据有效数据个数指示：0，表示每个64比特读数据含10个有效数据；1，表示每个64比特读数据含16个有效数据；2，表示每个64比特读数据含1个有效数据；
   UNION结构:  BBPCOMM_BBPMST_RD_CH1_BIT_WIDTH_IND_UNION */
#define BBPCOMM_BBPMST_RD_CH1_BIT_WIDTH_IND_ADDR      (SOC_BBP_COMM_BASE_ADDR + 0xA978)

/* 寄存器说明：
 bit[31:2]   保留
 bit[1:0]    读通道2每个64比特读数据有效数据个数指示：0，表示每个64比特读数据含10个有效数据；1，表示每个64比特读数据含16个有效数据；2，表示每个64比特读数据含16个有效数据；
   UNION结构:  BBPCOMM_BBPMST_RD_CH2_BIT_WIDTH_IND_UNION */
#define BBPCOMM_BBPMST_RD_CH2_BIT_WIDTH_IND_ADDR      (SOC_BBP_COMM_BASE_ADDR + 0xA97C)

/* 寄存器说明：
 bit[31:2]   保留
 bit[1:0]    读通道3每个64比特读数据有效数据个数指示：0，表示每个64比特读数据含10个有效数据；1，表示每个64比特读数据含16个有效数据；2，表示每个64比特读数据含1个有效数据；
   UNION结构:  BBPCOMM_BBPMST_RD_CH3_BIT_WIDTH_IND_UNION */
#define BBPCOMM_BBPMST_RD_CH3_BIT_WIDTH_IND_ADDR      (SOC_BBP_COMM_BASE_ADDR + 0xA980)

/* 寄存器说明：
 bit[31:3]   保留
 bit[2]      axi口0写通道的空闲状态标志： 软件可根据各个axi口的空闲状态决定是否能发起掉电或其他低功耗流程
             1’b0：空闲，总线已被释放掉
             1’b1：非空闲，总线未释放，此状态下master不可以复位/关时钟/掉电
             
 bit[1]      axi口0读通道的空闲状态标志： 软件可根据各个axi口的空闲状态决定是否能发起掉电或其他低功耗流程
             1’b0：空闲，总线已被释放掉
             1’b1：非空闲，总线未释放，此状态下master不可以复位/关时钟/掉电
             
 bit[0]      axi口0的空闲状态标志： 软件可根据各个axi口的空闲状态决定是否能发起掉电或其他低功耗流程
             1’b0：空闲，总线已被释放掉
             1’b1：非空闲，总线未释放，此状态下master不可以复位/关时钟/掉电
             
   UNION结构:  BBPCOMM_BBPMST_AXI0_STATUS_UNION */
#define BBPCOMM_BBPMST_AXI0_STATUS_ADDR               (SOC_BBP_COMM_BASE_ADDR + 0xAB00)

/* 寄存器说明：
 bit[31:3]   保留
 bit[2]      axi口1的写通道状态标志： 软件可根据各个axi口的空闲状态决定是否能发起掉电或其他低功耗流程
             1’b0：空闲，总线已被释放掉
             1’b1：非空闲，总线未释放，此状态下master不可以复位/关时钟/掉电
             
 bit[1]      axi口1的读通道状态标志： 软件可根据各个axi口的空闲状态决定是否能发起掉电或其他低功耗流程
             1’b0：空闲，总线已被释放掉
             1’b1：非空闲，总线未释放，此状态下master不可以复位/关时钟/掉电
             
 bit[0]      axi口1的空闲状态标志： 软件可根据各个axi口的空闲状态决定是否能发起掉电或其他低功耗流程
             1’b0：空闲，总线已被释放掉
             1’b1：非空闲，总线未释放，此状态下master不可以复位/关时钟/掉电
             
   UNION结构:  BBPCOMM_BBPMST_AXI1_STATUS_UNION */
#define BBPCOMM_BBPMST_AXI1_STATUS_ADDR               (SOC_BBP_COMM_BASE_ADDR + 0xAB04)

/* 寄存器说明：
 bit[31:4]   保留
 bit[3:0]    当bbpmst写error发生时，临近的burst的类型0
   UNION结构:  BBPCOMM_BBPMST_ERROR_BURST_BEAT0_UNION */
#define BBPCOMM_BBPMST_ERROR_BURST_BEAT0_ADDR         (SOC_BBP_COMM_BASE_ADDR + 0xAB08)

/* 寄存器说明：
 bit[31:4]   保留
 bit[3:0]    当bbpmst写error发生时，临近的burst的类型1
   UNION结构:  BBPCOMM_BBPMST_ERROR_BURST_BEAT1_UNION */
#define BBPCOMM_BBPMST_ERROR_BURST_BEAT1_ADDR         (SOC_BBP_COMM_BASE_ADDR + 0xAB0C)

/* 寄存器说明：
 bit[31:4]   保留
 bit[3:0]    当bbpmst写error发生时，临近的burst的类型2
   UNION结构:  BBPCOMM_BBPMST_ERROR_BURST_BEAT2_UNION */
#define BBPCOMM_BBPMST_ERROR_BURST_BEAT2_ADDR         (SOC_BBP_COMM_BASE_ADDR + 0xAB10)

/* 寄存器说明：
 bit[31:4]   保留
 bit[3:0]    当bbpmst写error发生时，临近的burst的类型3
   UNION结构:  BBPCOMM_BBPMST_ERROR_BURST_BEAT3_UNION */
#define BBPCOMM_BBPMST_ERROR_BURST_BEAT3_ADDR         (SOC_BBP_COMM_BASE_ADDR + 0xAB14)

/* 寄存器说明：
   详      述：当bbpmst写error发生时，临近的burst的基地址0
   UNION结构 ：无 */
#define BBPCOMM_BBPMST_BURST_ERROR_INIT_ADDR0_ADDR    (SOC_BBP_COMM_BASE_ADDR + 0xAB18)

/* 寄存器说明：
   详      述：当bbpmst写error发生时，临近的burst的基地址1
   UNION结构 ：无 */
#define BBPCOMM_BBPMST_BURST_ERROR_INIT_ADDR1_ADDR    (SOC_BBP_COMM_BASE_ADDR + 0xAB1C)

/* 寄存器说明：
   详      述：当bbpmst写error发生时，临近的burst的基地址2
   UNION结构 ：无 */
#define BBPCOMM_BBPMST_BURST_ERROR_INIT_ADDR2_ADDR    (SOC_BBP_COMM_BASE_ADDR + 0xAB20)

/* 寄存器说明：
   详      述：当bbpmst写error发生时，临近的burst的基地址3
   UNION结构 ：无 */
#define BBPCOMM_BBPMST_BURST_ERROR_INIT_ADDR3_ADDR    (SOC_BBP_COMM_BASE_ADDR + 0xAB24)

/* 寄存器说明：
 bit[31:1]   保留
 bit[0]      bbpmst读写错误中断清除寄存器。当BBPMST读写发生错误时，bbpmst_error_int会直接拉成电平送给BBE16\CCPU，软件收到中断后配置该寄存器清除中断电平。
   UNION结构:  BBPCOMM_BBPMST_ERROR_INT_CLR_UNION */
#define BBPCOMM_BBPMST_ERROR_INT_CLR_ADDR             (SOC_BBP_COMM_BASE_ADDR + 0xAB28)

/* 寄存器说明：
 bit[31:16]  1w2r双端口ram延时控制。
 bit[15:0]   单端口ram延时控制。
   UNION结构:  BBPCOMM_MEM_CTRL0_BBPMST_UNION */
#define BBPCOMM_MEM_CTRL0_BBPMST_ADDR                 (SOC_BBP_COMM_BASE_ADDR + 0xAB2C)

/* 寄存器说明：
 bit[31:24]  保留
 bit[23:16]  rom延时控制。
 bit[15:0]   1rw2rw双端口ram延时控制。
   UNION结构:  BBPCOMM_MEM_CTRL1_BBPMST_UNION */
#define BBPCOMM_MEM_CTRL1_BBPMST_ADDR                 (SOC_BBP_COMM_BASE_ADDR + 0xAB30)

/* 寄存器说明：
   详      述：AXI口0对所有BBPMST写通道写请求的响应个数统计
   UNION结构 ：无 */
#define BBPCOMM_AXI_WR0_ACK_CNT_ADDR                  (SOC_BBP_COMM_BASE_ADDR + 0xAB34)

/* 寄存器说明：
   详      述：AXI口0对所有BBPMST读通道读请求的响应个数统计
   UNION结构 ：无 */
#define BBPCOMM_AXI_RD0_ACK_CNT_ADDR                  (SOC_BBP_COMM_BASE_ADDR + 0xAB38)

/* 寄存器说明：
   详      述：AXI口1对所有BBPMST写通道写请求的响应个数统计
   UNION结构 ：无 */
#define BBPCOMM_AXI_WR1_ACK_CNT_ADDR                  (SOC_BBP_COMM_BASE_ADDR + 0xAB3C)

/* 寄存器说明：
   详      述：AXI口1对所有BBPMST读通道读请求的响应个数统计
   UNION结构 ：无 */
#define BBPCOMM_AXI_RD1_ACK_CNT_ADDR                  (SOC_BBP_COMM_BASE_ADDR + 0xAB40)

/* 寄存器说明：
 bit[31:11]  保留
 bit[10:8]   AXI口0当前写响应ID
 bit[7:5]    保留
 bit[4]      AXI口0当前写响应数据有效指示
 bit[3:1]    保留
 bit[0]      AXI口0当前写响应可接收数据指示
   UNION结构:  BBPCOMM_AXI_WR0_STATE_UNION */
#define BBPCOMM_AXI_WR0_STATE_ADDR                    (SOC_BBP_COMM_BASE_ADDR + 0xAB44)

/* 寄存器说明：
 bit[31:11]  保留
 bit[10:8]   AXI口0当前读响应ID
 bit[7:5]    保留
 bit[4]      AXI口0当前读响应数据有效指示
 bit[3:1]    保留
 bit[0]      AXI口0当前读响应可接收数据指示
   UNION结构:  BBPCOMM_AXI_RD0_STATE_UNION */
#define BBPCOMM_AXI_RD0_STATE_ADDR                    (SOC_BBP_COMM_BASE_ADDR + 0xAB48)

/* 寄存器说明：
 bit[31:11]  保留
 bit[10:8]   AXI口1当前写响应ID
 bit[7:5]    保留
 bit[4]      AXI口1当前写响应数据有效指示
 bit[3:1]    保留
 bit[0]      AXI口1当前写响应可接收数据指示
   UNION结构:  BBPCOMM_AXI_WR1_STATE_UNION */
#define BBPCOMM_AXI_WR1_STATE_ADDR                    (SOC_BBP_COMM_BASE_ADDR + 0xAB4C)

/* 寄存器说明：
 bit[31:11]  保留
 bit[10:8]   AXI口1当前读响应ID
 bit[7:5]    保留
 bit[4]      AXI口1当前读响应数据有效指示
 bit[3:1]    保留
 bit[0]      AXI口1当前读响应可接收数据指示
   UNION结构:  BBPCOMM_AXI_RD1_STATE_UNION */
#define BBPCOMM_AXI_RD1_STATE_ADDR                    (SOC_BBP_COMM_BASE_ADDR + 0xAB50)

/* 寄存器说明：
   详      述：AXI口0对所有BBPMST写通道数据传输个数统计
   UNION结构 ：无 */
#define BBPCOMM_AXI_WR0_DATA_CNT_ADDR                 (SOC_BBP_COMM_BASE_ADDR + 0xAB54)

/* 寄存器说明：
   详      述：AXI口0对所有BBPMST读通道数据传输个数统计
   UNION结构 ：无 */
#define BBPCOMM_AXI_RD0_DATA_CNT_ADDR                 (SOC_BBP_COMM_BASE_ADDR + 0xAB58)

/* 寄存器说明：
   详      述：AXI口1对所有BBPMST写通道数据传输个数统计
   UNION结构 ：无 */
#define BBPCOMM_AXI_WR1_DATA_CNT_ADDR                 (SOC_BBP_COMM_BASE_ADDR + 0xAB5C)

/* 寄存器说明：
   详      述：AXI口1对所有BBPMST读通道数据传输个数统计
   UNION结构 ：无 */
#define BBPCOMM_AXI_RD1_DATA_CNT_ADDR                 (SOC_BBP_COMM_BASE_ADDR + 0xAB60)

/* 寄存器说明：
 bit[31:1]   保留
 bit[0]      BBPMST读写错误中断使能
   UNION结构:  BBPCOMM_BBPMST_ERROR_INT_EN_UNION */
#define BBPCOMM_BBPMST_ERROR_INT_EN_ADDR              (SOC_BBP_COMM_BASE_ADDR + 0xAB64)

/* 寄存器说明：
 bit[31:1]   保留
 bit[0]      BBPMST读写错误中断屏蔽信号
   UNION结构:  BBPCOMM_BBPMST_ERROR_INT_MASK_UNION */
#define BBPCOMM_BBPMST_ERROR_INT_MASK_ADDR            (SOC_BBP_COMM_BASE_ADDR + 0xAB68)



/***======================================================================***
                     (3/5) register_define_irm_cs
 ***======================================================================***/
/* 寄存器说明：
 bit[31:3]   保留
 bit[2:0]    用于irm_cs模式选择，影响irm_cs内部时钟切换和模式切换，默认都为0。
             3'b001，UMTS模式（包括均衡，译码R99相关RAM）；
             3'b010，LTE模式（上行HARQ部分）；
             3'b100，CDMA模式；
             3'b000，无（default）。
   UNION结构:  BBPCOMM_IRM_CS_RAM_CTRL_UNION */
#define BBPCOMM_IRM_CS_RAM_CTRL_ADDR                  (SOC_BBP_COMM_BASE_ADDR + 0xA000)

/* 寄存器说明：
 bit[31:1]   保留
 bit[0]      irm_cs模块时钟门控旁路开关。默认为0，即开启时钟门控。配置1则开启旁路功能，时钟门控失效。
   UNION结构:  BBPCOMM_IRM_CS_CLK_BYPASS_UNION */
#define BBPCOMM_IRM_CS_CLK_BYPASS_ADDR                (SOC_BBP_COMM_BASE_ADDR + 0xA004)

/* 寄存器说明：
 bit[31:16]  1w2r双端口ram延时控制。
 bit[15:0]   单端口ram延时控制。
   UNION结构:  BBPCOMM_MEM_CTRL0_CS_UNION */
#define BBPCOMM_MEM_CTRL0_CS_ADDR                     (SOC_BBP_COMM_BASE_ADDR + 0xA00C)

/* 寄存器说明：
 bit[31:24]  保留
 bit[23:16]  rom延时控制。
 bit[15:0]   1rw2rw双端口ram延时控制。
   UNION结构:  BBPCOMM_MEM_CTRL1_CS_UNION */
#define BBPCOMM_MEM_CTRL1_CS_ADDR                     (SOC_BBP_COMM_BASE_ADDR + 0xA010)



/***======================================================================***
                     (4/5) register_define_irm_ps
 ***======================================================================***/
/* 寄存器说明：
 bit[31:6]   保留
 bit[5:0]    用于irm_ps网络模式选择，影响irm_ps内部时钟切换和模式切换，默认都为0。.
             6'b000001，GBBP1模式；
             6'b000110，UMTS模式；
             6'b001000，LTE模式；
             6'b010000，TDS-CDMA模式；
             6'b100000，CDMA模式。
             注：GBBP1固定为主卡（G0/U/L/T）。
   UNION结构:  BBPCOMM_IRM_PS_RAM_CTRL_UNION */
#define BBPCOMM_IRM_PS_RAM_CTRL_ADDR                  (SOC_BBP_COMM_BASE_ADDR + 0xB000)

/* 寄存器说明：
 bit[31:1]   保留
 bit[0]      GBBP2，默认为0，1为使能。
             注：GBBP2固定为辅卡。
             V8R1不使用。
   UNION结构:  BBPCOMM_IRM_PS_GSM2_CTRL_UNION */
#define BBPCOMM_IRM_PS_GSM2_CTRL_ADDR                 (SOC_BBP_COMM_BASE_ADDR + 0xB004)

/* 寄存器说明：
 bit[31:1]   保留
 bit[0]      irm_ps模块时钟门控旁路开关。默认为0，即开启时钟门控。配置1则开启旁路功能，时钟门控失效。
   UNION结构:  BBPCOMM_IRM_PS_CLK_BYPASS_UNION */
#define BBPCOMM_IRM_PS_CLK_BYPASS_ADDR                (SOC_BBP_COMM_BASE_ADDR + 0xB008)

/* 寄存器说明：
 bit[31:16]  1w2r双端口ram延时控制。
 bit[15:0]   单端口ram延时控制。
   UNION结构:  BBPCOMM_MEM_CTRL0_TURBO_UNION */
#define BBPCOMM_MEM_CTRL0_TURBO_ADDR                  (SOC_BBP_COMM_BASE_ADDR + 0xB010)

/* 寄存器说明：
 bit[31:24]  保留
 bit[23:16]  rom延时控制。
 bit[15:0]   1rw2rw双端口ram延时控制。
   UNION结构:  BBPCOMM_MEM_CTRL1_TURBO_UNION */
#define BBPCOMM_MEM_CTRL1_TURBO_ADDR                  (SOC_BBP_COMM_BASE_ADDR + 0xB014)

/* 寄存器说明：
 bit[31:16]  1w2r双端口ram延时控制。
 bit[15:0]   单端口ram延时控制。
   UNION结构:  BBPCOMM_MEM_CTRL0_PS_UNION */
#define BBPCOMM_MEM_CTRL0_PS_ADDR                     (SOC_BBP_COMM_BASE_ADDR + 0xB020)

/* 寄存器说明：
 bit[31:24]  保留
 bit[23:16]  rom延时控制。
 bit[15:0]   1rw2rw双端口ram延时控制。
   UNION结构:  BBPCOMM_MEM_CTRL1_PS_UNION */
#define BBPCOMM_MEM_CTRL1_PS_ADDR                     (SOC_BBP_COMM_BASE_ADDR + 0xB024)

/* 寄存器说明：
 bit[31:16]  turbo译码子模块级门控旁路使能，高电平有效。
 bit[15:0]   turbo译码子模块级门控旁路使能，高电平有效。
   UNION结构:  BBPCOMM_TUBO_CKG_UNION */
#define BBPCOMM_TUBO_CKG_ADDR                         (SOC_BBP_COMM_BASE_ADDR + 0xB028)

/* 寄存器说明：
 bit[31:1]   保留
 bit[0]      U模/L模算法选择信号。
   UNION结构:  BBPCOMM_U_L_TYPE_UNION */
#define BBPCOMM_U_L_TYPE_ADDR                         (SOC_BBP_COMM_BASE_ADDR + 0xB02C)

/* 寄存器说明：
 bit[31:1]   保留
 bit[0]      U模/T模接口选择信号。
   UNION结构:  BBPCOMM_U_T_TYPE_UNION */
#define BBPCOMM_U_T_TYPE_ADDR                         (SOC_BBP_COMM_BASE_ADDR + 0xB030)

/* 寄存器说明：
 bit[31:1]   保留
 bit[0]      U模/C模接口选择信号。
   UNION结构:  BBPCOMM_U_C_TYPE_UNION */
#define BBPCOMM_U_C_TYPE_ADDR                         (SOC_BBP_COMM_BASE_ADDR + 0xB034)

/* 寄存器说明：
 bit[31:16]  保留
 bit[15:14]  SOC AP配置给ps ram的occupy配置信号
 bit[13:12]  
 bit[11:10]  
 bit[9:8]    
 bit[7:6]    
 bit[5:4]    
 bit[3:2]    
 bit[1:0]    
   UNION结构:  BBPCOMM_SOC_AP_OCCUPY_GRP_UNION */
#define BBPCOMM_SOC_AP_OCCUPY_GRP_ADDR                (SOC_BBP_COMM_BASE_ADDR + 0xB038)

/* 寄存器说明：
 bit[31:2]   保留
 bit[1]      W SDR模式下译码时钟选择。0，译码采用122m时钟；1，译码采用245m时钟。
 bit[0]      W SDR模式开关使能。
   UNION结构:  BBPCOMM_PS_W_SDR_EN_UNION */
#define BBPCOMM_PS_W_SDR_EN_ADDR                      (SOC_BBP_COMM_BASE_ADDR + 0xB03C)



/***======================================================================***
                     (5/5) register_define_abb_cfg
 ***======================================================================***/
/* 寄存器说明：软复位
 bit[31:1]   保留
 bit[0]      复位寄存器：
             1'b0：解除复位
             1'b1:进入复位
             配置时刻：任意
             生效时刻：立即
   UNION结构:  BBPCOMM_SOFT_RST_UNION */
#define BBPCOMM_SOFT_RST_ADDR                         (SOC_BBP_COMM_BASE_ADDR + 0xE000)

/* 寄存器说明：abb配置模式
 bit[31:2]   保留
 bit[1]      abb配置模式：
             1'b0:软件间接配置abb寄存器
             1'b1:软件直接配置abb寄存器
 bit[0]      仲裁使能：
             1'b0:屏蔽仲裁功能
             1'b1:使能仲裁功能
   UNION结构:  BBPCOMM_CFG_CTRL0_UNION */
#define BBPCOMM_CFG_CTRL0_ADDR                        (SOC_BBP_COMM_BASE_ADDR + 0xE004)

/* 寄存器说明：abb配置模式
 bit[31:1]   保留
 bit[0]      软件读写标志：
             1'b0:写
             1'b1:读
   UNION结构:  BBPCOMM_CFG_CTRL1_UNION */
#define BBPCOMM_CFG_CTRL1_ADDR                        (SOC_BBP_COMM_BASE_ADDR + 0xE008)

/* 寄存器说明：abb配置模式
 bit[31:6]   保留
 bit[5:3]    接收fifo水线，当接收fifo当前存储深度大于该水线时，产生接收fifo中断。
             3'b000:1
             3'b001:2
             3'b010:3
             3'b011:4
             3'b100:5
             3'b101:6
             3'b110:7
             3'b111:8
 bit[2:0]    发送fifo水线，当发送fifo当前存储深度小于该水线时，产生发送fifo中断。
             3'b000:1
             3'b001:2
             3'b010:3
             3'b011:4
             3'b100:5
             3'b101:6
             3'b110:7
             3'b111:8
   UNION结构:  BBPCOMM_CFG_CTRL2_UNION */
#define BBPCOMM_CFG_CTRL2_ADDR                        (SOC_BBP_COMM_BASE_ADDR + 0xE00C)

/* 寄存器说明：非定时模式下软件配置ABB的数据
   详      述：非定时模式下软件配置ABB的数据，
            写格式：
            bit[12:0]：写地址
            bit[20:13]:写数据
            其余保留
            读格式：
            bit[12:0]：读地址
            bit[20:13]:回读数据
            其余保留
   UNION结构 ：无 */
#define BBPCOMM_CFG_DATA_ADDR                         (SOC_BBP_COMM_BASE_ADDR + 0xE010)

/* 寄存器说明：中断屏蔽寄存器
 bit[31:4]   保留
 bit[3]      发送fifo中断屏蔽位
             1'b0:屏蔽
             1'b1:不屏蔽
 bit[2]      接收fifo中断屏蔽位
             1'b0:屏蔽
             1'b1:不屏蔽
 bit[1]      保留
 bit[0]      接收fifo溢出中断屏蔽位
             1'b0:屏蔽
             1'b1:不屏蔽
   UNION结构:  BBPCOMM_CFG_INT_MASK_UNION */
#define BBPCOMM_CFG_INT_MASK_ADDR                     (SOC_BBP_COMM_BASE_ADDR + 0xE030)

/* 寄存器说明：中断清除寄存器
 bit[31:4]   保留
 bit[3]      发送fifo中断清除位
             1'b0:不清除
             1'b1:清除
 bit[2]      接收fifo中断清除位
             1'b0:不清除
             1'b1:清除
 bit[1]      保留
 bit[0]      接收fifo溢出中断清除位
             1'b0:不清除
             1'b1:清除
   UNION结构:  BBPCOMM_CFG_INT_CLR_UNION */
#define BBPCOMM_CFG_INT_CLR_ADDR                      (SOC_BBP_COMM_BASE_ADDR + 0xE034)

/* 寄存器说明：中断告警寄存器
 bit[31:4]   保留
 bit[3]      发送fifo中断告警寄存器，当新的中断来临时，老的中断还没有清除，产生此告警信号，写1清除。
 bit[2]      接收fifo中断告警寄存器，当新的中断来临时，老的中断还没有清除，产生此告警信号，写1清除。
 bit[1]      保留
 bit[0]      接收fifo溢出中断告警寄存器，当新的中断来临时，老的中断还没有清除，产生此告警信号，写1清除。
             
   UNION结构:  BBPCOMM_CFG_INT_ALM_UNION */
#define BBPCOMM_CFG_INT_ALM_ADDR                      (SOC_BBP_COMM_BASE_ADDR + 0xE038)

/* 寄存器说明：FIFO时钟门控
 bit[31:1]   保留
 bit[0]      FIFO时钟门控使能信号
             1'b0 : 不进行时钟门控
             1'b1 : 进行时钟门控
   UNION结构:  BBPCOMM_MEM_CTRL_UNION */
#define BBPCOMM_MEM_CTRL_ADDR                         (SOC_BBP_COMM_BASE_ADDR + 0xE03C)

/* 寄存器说明：BBP配置使能
 bit[31:5]   保留
 bit[4:0]    软件屏蔽各BBP配置ABB线控信号，
             5'b1_1110:屏蔽LTE模；
             5'b1_1101:屏蔽W模；
             5'b1_1011:屏蔽TDS模；
             5'b1_0111:屏蔽G1模；
             5'b0_1111:屏蔽G2模；
             5'b0_0000:全屏蔽
             5'b1_1111:全不屏蔽（默认）
             
   UNION结构:  BBPCOMM_BBP_CFG_EN_UNION */
#define BBPCOMM_BBP_CFG_EN_ADDR                       (SOC_BBP_COMM_BASE_ADDR + 0xE0FC)

/* 寄存器说明：LTE模式定时操作控制寄存器
 bit[31:4]   保留
 bit[3:1]    lte模式定时操作读写寄存器个数：
             0：1
             1：2
             ...
             7: 8
 bit[0]      lte模式定时操作读写控制：
             1'b0：写操作
             1'b1：读操作
   UNION结构:  BBPCOMM_L_CFG_CTRL_UNION */
#define BBPCOMM_L_CFG_CTRL_ADDR                       (SOC_BBP_COMM_BASE_ADDR + 0xE100)

/* 寄存器说明：TDS模式定时操作控制寄存器
 bit[31:4]   保留
 bit[3:1]    tds模式定时操作读写寄存器个数：
             0：1
             1：2
             ...
             7: 8
 bit[0]      tds模式定时操作读写控制：
             1'b0：写操作
             1'b1：读操作
   UNION结构:  BBPCOMM_T_CFG_CTRL_UNION */
#define BBPCOMM_T_CFG_CTRL_ADDR                       (SOC_BBP_COMM_BASE_ADDR + 0xE104)

/* 寄存器说明：W模式定时操作控制寄存器
 bit[31:4]   保留
 bit[3:1]    wcdma模式定时操作读写寄存器个数：
             0：1
             1：2
             ...
             7: 8
 bit[0]      wcdma模式定时操作读写控制：
             1'b0：写操作
             1'b1：读操作
   UNION结构:  BBPCOMM_W_CFG_CTRL_UNION */
#define BBPCOMM_W_CFG_CTRL_ADDR                       (SOC_BBP_COMM_BASE_ADDR + 0xE108)

/* 寄存器说明：G1模式定时操作控制寄存器
 bit[31:4]   保留
 bit[3:1]    gsm1模式定时操作读写寄存器个数：
             0：1
             1：2
             ...
             7: 8
 bit[0]      gsm1模式定时操作读写控制：
             1'b0：写操作
             1'b1：读操作
   UNION结构:  BBPCOMM_G1_CFG_CTRL_UNION */
#define BBPCOMM_G1_CFG_CTRL_ADDR                      (SOC_BBP_COMM_BASE_ADDR + 0xE10C)

/* 寄存器说明：G2模式定时操作控制寄存器
 bit[31:4]   保留
 bit[3:1]    gsm2模式定时操作读写寄存器个数：
             0：1
             1：2
             ...
             7: 8
 bit[0]      gsm2模式定时操作读写控制：
             1'b0：写操作
             1'b1：读操作
   UNION结构:  BBPCOMM_G2_CFG_CTRL_UNION */
#define BBPCOMM_G2_CFG_CTRL_ADDR                      (SOC_BBP_COMM_BASE_ADDR + 0xE110)

/* 寄存器说明：定时模式下lte bbp配置ABB的数据
   详      述：定时模式下lte bbp配置ABB的数据，
            写格式：
            bit[12:0]：写地址
            bit[20:13]:写数据
            其余保留
            读格式：
            bit[12:0]：读地址
            bit[20:13]:回读数据
            其余保留
   UNION结构 ：无 */
#define BBPCOMM_L_CFG_DATA0_ADDR                      (SOC_BBP_COMM_BASE_ADDR + 0xE160)

/* 寄存器说明：定时模式下lte bbp配置ABB的数据
   详      述：定时模式下lte bbp配置ABB的数据，
            写格式：
            bit[12:0]：写地址
            bit[20:13]:写数据
            其余保留
            读格式：
            bit[12:0]：读地址
            bit[20:13]:回读数据
            其余保留
   UNION结构 ：无 */
#define BBPCOMM_L_CFG_DATA1_ADDR                      (SOC_BBP_COMM_BASE_ADDR + 0xE164)

/* 寄存器说明：定时模式下lte bbp配置ABB的数据
   详      述：定时模式下lte bbp配置ABB的数据，
            写格式：
            bit[12:0]：写地址
            bit[20:13]:写数据
            其余保留
            读格式：
            bit[12:0]：读地址
            bit[20:13]:回读数据
            其余保留
   UNION结构 ：无 */
#define BBPCOMM_L_CFG_DATA2_ADDR                      (SOC_BBP_COMM_BASE_ADDR + 0xE168)

/* 寄存器说明：定时模式下lte bbp配置ABB的数据
   详      述：定时模式下lte bbp配置ABB的数据，
            写格式：
            bit[12:0]：写地址
            bit[20:13]:写数据
            其余保留
            读格式：
            bit[12:0]：读地址
            bit[20:13]:回读数据
            其余保留
   UNION结构 ：无 */
#define BBPCOMM_L_CFG_DATA3_ADDR                      (SOC_BBP_COMM_BASE_ADDR + 0xE16C)

/* 寄存器说明：定时模式下lte bbp配置ABB的数据
   详      述：定时模式下lte bbp配置ABB的数据，
            写格式：
            bit[12:0]：写地址
            bit[20:13]:写数据
            其余保留
            读格式：
            bit[12:0]：读地址
            bit[20:13]:回读数据
            其余保留
   UNION结构 ：无 */
#define BBPCOMM_L_CFG_DATA4_ADDR                      (SOC_BBP_COMM_BASE_ADDR + 0xE170)

/* 寄存器说明：定时模式下lte bbp配置ABB的数据
   详      述：定时模式下lte bbp配置ABB的数据，
            写格式：
            bit[12:0]：写地址
            bit[20:13]:写数据
            其余保留
            读格式：
            bit[12:0]：读地址
            bit[20:13]:回读数据
            其余保留
   UNION结构 ：无 */
#define BBPCOMM_L_CFG_DATA5_ADDR                      (SOC_BBP_COMM_BASE_ADDR + 0xE174)

/* 寄存器说明：定时模式下lte bbp配置ABB的数据
   详      述：定时模式下lte bbp配置ABB的数据，
            写格式：
            bit[12:0]：写地址
            bit[20:13]:写数据
            其余保留
            读格式：
            bit[12:0]：读地址
            bit[20:13]:回读数据
            其余保留
   UNION结构 ：无 */
#define BBPCOMM_L_CFG_DATA6_ADDR                      (SOC_BBP_COMM_BASE_ADDR + 0xE178)

/* 寄存器说明：定时模式下lte bbp配置ABB的数据
   详      述：定时模式下lte bbp配置ABB的数据，
            写格式：
            bit[12:0]：写地址
            bit[20:13]:写数据
            其余保留
            读格式：
            bit[12:0]：读地址
            bit[20:13]:回读数据
            其余保留
   UNION结构 ：无 */
#define BBPCOMM_L_CFG_DATA7_ADDR                      (SOC_BBP_COMM_BASE_ADDR + 0xE17C)

/* 寄存器说明：定时模式下tds bbp配置ABB的数据
   详      述：定时模式下tds bbp配置ABB的数据，
            写格式：
            bit[12:0]：写地址
            bit[20:13]:写数据
            其余保留
            读格式：
            bit[12:0]：读地址
            bit[20:13]:回读数据
            其余保留
   UNION结构 ：无 */
#define BBPCOMM_T_CFG_DATA0_ADDR                      (SOC_BBP_COMM_BASE_ADDR + 0xE180)

/* 寄存器说明：定时模式下tds bbp配置ABB的数据
   详      述：定时模式下tds bbp配置ABB的数据，
            写格式：
            bit[12:0]：写地址
            bit[20:13]:写数据
            其余保留
            读格式：
            bit[12:0]：读地址
            bit[20:13]:回读数据
            其余保留
   UNION结构 ：无 */
#define BBPCOMM_T_CFG_DATA1_ADDR                      (SOC_BBP_COMM_BASE_ADDR + 0xE184)

/* 寄存器说明：定时模式下tds bbp配置ABB的数据
   详      述：定时模式下tds bbp配置ABB的数据，
            写格式：
            bit[12:0]：写地址
            bit[20:13]:写数据
            其余保留
            读格式：
            bit[12:0]：读地址
            bit[20:13]:回读数据
            其余保留
   UNION结构 ：无 */
#define BBPCOMM_T_CFG_DATA2_ADDR                      (SOC_BBP_COMM_BASE_ADDR + 0xE188)

/* 寄存器说明：定时模式下tds bbp配置ABB的数据
   详      述：定时模式下tds bbp配置ABB的数据，
            写格式：
            bit[12:0]：写地址
            bit[20:13]:写数据
            其余保留
            读格式：
            bit[12:0]：读地址
            bit[20:13]:回读数据
            其余保留
   UNION结构 ：无 */
#define BBPCOMM_T_CFG_DATA3_ADDR                      (SOC_BBP_COMM_BASE_ADDR + 0xE18C)

/* 寄存器说明：定时模式下tds bbp配置ABB的数据
   详      述：定时模式下tds bbp配置ABB的数据，
            写格式：
            bit[12:0]：写地址
            bit[20:13]:写数据
            其余保留
            读格式：
            bit[12:0]：读地址
            bit[20:13]:回读数据
            其余保留
   UNION结构 ：无 */
#define BBPCOMM_T_CFG_DATA4_ADDR                      (SOC_BBP_COMM_BASE_ADDR + 0xE190)

/* 寄存器说明：定时模式下tds bbp配置ABB的数据
   详      述：定时模式下tds bbp配置ABB的数据，
            写格式：
            bit[12:0]：写地址
            bit[20:13]:写数据
            其余保留
            读格式：
            bit[12:0]：读地址
            bit[20:13]:回读数据
            其余保留
   UNION结构 ：无 */
#define BBPCOMM_T_CFG_DATA5_ADDR                      (SOC_BBP_COMM_BASE_ADDR + 0xE194)

/* 寄存器说明：定时模式下tds bbp配置ABB的数据
   详      述：定时模式下tds bbp配置ABB的数据，
            写格式：
            bit[12:0]：写地址
            bit[20:13]:写数据
            其余保留
            读格式：
            bit[12:0]：读地址
            bit[20:13]:回读数据
            其余保留
   UNION结构 ：无 */
#define BBPCOMM_T_CFG_DATA6_ADDR                      (SOC_BBP_COMM_BASE_ADDR + 0xE198)

/* 寄存器说明：定时模式下tds bbp配置ABB的数据
   详      述：定时模式下tds bbp配置ABB的数据，
            写格式：
            bit[12:0]：写地址
            bit[20:13]:写数据
            其余保留
            读格式：
            bit[12:0]：读地址
            bit[20:13]:回读数据
            其余保留
   UNION结构 ：无 */
#define BBPCOMM_T_CFG_DATA7_ADDR                      (SOC_BBP_COMM_BASE_ADDR + 0xE19C)

/* 寄存器说明：定时模式下w bbp配置ABB的数据
   详      述：定时模式下wcdma bbp配置ABB的数据，
            写格式：
            bit[12:0]：写地址
            bit[20:13]:写数据
            其余保留
            读格式：
            bit[12:0]：读地址
            bit[20:13]:回读数据
            其余保留
   UNION结构 ：无 */
#define BBPCOMM_W_CFG_DATA0_ADDR                      (SOC_BBP_COMM_BASE_ADDR + 0xE1A0)

/* 寄存器说明：定时模式下w bbp配置ABB的数据
   详      述：定时模式下wcdma bbp配置ABB的数据，
            写格式：
            bit[12:0]：写地址
            bit[20:13]:写数据
            其余保留
            读格式：
            bit[12:0]：读地址
            bit[20:13]:回读数据
            其余保留
   UNION结构 ：无 */
#define BBPCOMM_W_CFG_DATA1_ADDR                      (SOC_BBP_COMM_BASE_ADDR + 0xE1A4)

/* 寄存器说明：定时模式下w bbp配置ABB的数据
   详      述：定时模式下wcdma bbp配置ABB的数据，
            写格式：
            bit[12:0]：写地址
            bit[20:13]:写数据
            其余保留
            读格式：
            bit[12:0]：读地址
            bit[20:13]:回读数据
            其余保留
   UNION结构 ：无 */
#define BBPCOMM_W_CFG_DATA2_ADDR                      (SOC_BBP_COMM_BASE_ADDR + 0xE1A8)

/* 寄存器说明：定时模式下w bbp配置ABB的数据
   详      述：定时模式下wcdma bbp配置ABB的数据，
            写格式：
            bit[12:0]：写地址
            bit[20:13]:写数据
            其余保留
            读格式：
            bit[12:0]：读地址
            bit[20:13]:回读数据
            其余保留
   UNION结构 ：无 */
#define BBPCOMM_W_CFG_DATA3_ADDR                      (SOC_BBP_COMM_BASE_ADDR + 0xE1AC)

/* 寄存器说明：定时模式下w bbp配置ABB的数据
   详      述：定时模式下wcdma bbp配置ABB的数据，
            写格式：
            bit[12:0]：写地址
            bit[20:13]:写数据
            其余保留
            读格式：
            bit[12:0]：读地址
            bit[20:13]:回读数据
            其余保留
   UNION结构 ：无 */
#define BBPCOMM_W_CFG_DATA4_ADDR                      (SOC_BBP_COMM_BASE_ADDR + 0xE1B0)

/* 寄存器说明：定时模式下w bbp配置ABB的数据
   详      述：定时模式下wcdma bbp配置ABB的数据，
            写格式：
            bit[12:0]：写地址
            bit[20:13]:写数据
            其余保留
            读格式：
            bit[12:0]：读地址
            bit[20:13]:回读数据
            其余保留
   UNION结构 ：无 */
#define BBPCOMM_W_CFG_DATA5_ADDR                      (SOC_BBP_COMM_BASE_ADDR + 0xE1B4)

/* 寄存器说明：定时模式下w bbp配置ABB的数据
   详      述：定时模式下wcdma bbp配置ABB的数据，
            写格式：
            bit[12:0]：写地址
            bit[20:13]:写数据
            其余保留
            读格式：
            bit[12:0]：读地址
            bit[20:13]:回读数据
            其余保留
   UNION结构 ：无 */
#define BBPCOMM_W_CFG_DATA6_ADDR                      (SOC_BBP_COMM_BASE_ADDR + 0xE1B8)

/* 寄存器说明：定时模式下w bbp配置ABB的数据
   详      述：定时模式下wcdma bbp配置ABB的数据，
            写格式：
            bit[12:0]：写地址
            bit[20:13]:写数据
            其余保留
            读格式：
            bit[12:0]：读地址
            bit[20:13]:回读数据
            其余保留
   UNION结构 ：无 */
#define BBPCOMM_W_CFG_DATA7_ADDR                      (SOC_BBP_COMM_BASE_ADDR + 0xE1BC)

/* 寄存器说明：定时模式下gsm1 bbp配置ABB的数据
   详      述：定时模式下gsm1 bbp配置ABB的数据，
            写格式：
            bit[12:0]：写地址
            bit[20:13]:写数据
            其余保留
            读格式：
            bit[12:0]：读地址
            bit[20:13]:回读数据
            其余保留
   UNION结构 ：无 */
#define BBPCOMM_G1_CFG_DATA0_ADDR                     (SOC_BBP_COMM_BASE_ADDR + 0xE1C0)

/* 寄存器说明：定时模式下gsm1 bbp配置ABB的数据
   详      述：定时模式下gsm1 bbp配置ABB的数据，
            写格式：
            bit[12:0]：写地址
            bit[20:13]:写数据
            其余保留
            读格式：
            bit[12:0]：读地址
            bit[20:13]:回读数据
            其余保留
   UNION结构 ：无 */
#define BBPCOMM_G1_CFG_DATA1_ADDR                     (SOC_BBP_COMM_BASE_ADDR + 0xE1C4)

/* 寄存器说明：定时模式下gsm1 bbp配置ABB的数据
   详      述：定时模式下gsm1 bbp配置ABB的数据，
            写格式：
            bit[12:0]：写地址
            bit[20:13]:写数据
            其余保留
            读格式：
            bit[12:0]：读地址
            bit[20:13]:回读数据
            其余保留
   UNION结构 ：无 */
#define BBPCOMM_G1_CFG_DATA2_ADDR                     (SOC_BBP_COMM_BASE_ADDR + 0xE1C8)

/* 寄存器说明：定时模式下gsm1 bbp配置ABB的数据
   详      述：定时模式下gsm1 bbp配置ABB的数据，
            写格式：
            bit[12:0]：写地址
            bit[20:13]:写数据
            其余保留
            读格式：
            bit[12:0]：读地址
            bit[20:13]:回读数据
            其余保留
   UNION结构 ：无 */
#define BBPCOMM_G1_CFG_DATA3_ADDR                     (SOC_BBP_COMM_BASE_ADDR + 0xE1CC)

/* 寄存器说明：定时模式下gsm1 bbp配置ABB的数据
   详      述：定时模式下gsm1 bbp配置ABB的数据，
            写格式：
            bit[12:0]：写地址
            bit[20:13]:写数据
            其余保留
            读格式：
            bit[12:0]：读地址
            bit[20:13]:回读数据
            其余保留
   UNION结构 ：无 */
#define BBPCOMM_G1_CFG_DATA4_ADDR                     (SOC_BBP_COMM_BASE_ADDR + 0xE1D0)

/* 寄存器说明：定时模式下gsm1 bbp配置ABB的数据
   详      述：定时模式下gsm1 bbp配置ABB的数据，
            写格式：
            bit[12:0]：写地址
            bit[20:13]:写数据
            其余保留
            读格式：
            bit[12:0]：读地址
            bit[20:13]:回读数据
            其余保留
   UNION结构 ：无 */
#define BBPCOMM_G1_CFG_DATA5_ADDR                     (SOC_BBP_COMM_BASE_ADDR + 0xE1D4)

/* 寄存器说明：定时模式下gsm1 bbp配置ABB的数据
   详      述：定时模式下gsm1 bbp配置ABB的数据，
            写格式：
            bit[12:0]：写地址
            bit[20:13]:写数据
            其余保留
            读格式：
            bit[12:0]：读地址
            bit[20:13]:回读数据
            其余保留
   UNION结构 ：无 */
#define BBPCOMM_G1_CFG_DATA6_ADDR                     (SOC_BBP_COMM_BASE_ADDR + 0xE1D8)

/* 寄存器说明：定时模式下gsm1 bbp配置ABB的数据
   详      述：定时模式下gsm1 bbp配置ABB的数据，
            写格式：
            bit[12:0]：写地址
            bit[20:13]:写数据
            其余保留
            读格式：
            bit[12:0]：读地址
            bit[20:13]:回读数据
            其余保留
   UNION结构 ：无 */
#define BBPCOMM_G1_CFG_DATA7_ADDR                     (SOC_BBP_COMM_BASE_ADDR + 0xE1DC)

/* 寄存器说明：定时模式下gsm2 bbp配置ABB的数据
   详      述：定时模式下gsm2 bbp配置ABB的数据，
            写格式：
            bit[12:0]：写地址
            bit[20:13]:写数据
            其余保留
            读格式：
            bit[12:0]：读地址
            bit[20:13]:回读数据
            其余保留
   UNION结构 ：无 */
#define BBPCOMM_G2_CFG_DATA0_ADDR                     (SOC_BBP_COMM_BASE_ADDR + 0xE1E0)

/* 寄存器说明：定时模式下gsm2 bbp配置ABB的数据
   详      述：定时模式下gsm2 bbp配置ABB的数据，
            写格式：
            bit[12:0]：写地址
            bit[20:13]:写数据
            其余保留
            读格式：
            bit[12:0]：读地址
            bit[20:13]:回读数据
            其余保留
   UNION结构 ：无 */
#define BBPCOMM_G2_CFG_DATA1_ADDR                     (SOC_BBP_COMM_BASE_ADDR + 0xE1E4)

/* 寄存器说明：定时模式下gsm2 bbp配置ABB的数据
   详      述：定时模式下gsm2 bbp配置ABB的数据，
            写格式：
            bit[12:0]：写地址
            bit[20:13]:写数据
            其余保留
            读格式：
            bit[12:0]：读地址
            bit[20:13]:回读数据
            其余保留
   UNION结构 ：无 */
#define BBPCOMM_G2_CFG_DATA2_ADDR                     (SOC_BBP_COMM_BASE_ADDR + 0xE1E8)

/* 寄存器说明：定时模式下gsm2 bbp配置ABB的数据
   详      述：定时模式下gsm2 bbp配置ABB的数据，
            写格式：
            bit[12:0]：写地址
            bit[20:13]:写数据
            其余保留
            读格式：
            bit[12:0]：读地址
            bit[20:13]:回读数据
            其余保留
   UNION结构 ：无 */
#define BBPCOMM_G2_CFG_DATA3_ADDR                     (SOC_BBP_COMM_BASE_ADDR + 0xE1EC)

/* 寄存器说明：定时模式下gsm2 bbp配置ABB的数据
   详      述：定时模式下gsm2 bbp配置ABB的数据，
            写格式：
            bit[12:0]：写地址
            bit[20:13]:写数据
            其余保留
            读格式：
            bit[12:0]：读地址
            bit[20:13]:回读数据
            其余保留
   UNION结构 ：无 */
#define BBPCOMM_G2_CFG_DATA4_ADDR                     (SOC_BBP_COMM_BASE_ADDR + 0xE1F0)

/* 寄存器说明：定时模式下gsm2 bbp配置ABB的数据
   详      述：定时模式下gsm2 bbp配置ABB的数据，
            写格式：
            bit[12:0]：写地址
            bit[20:13]:写数据
            其余保留
            读格式：
            bit[12:0]：读地址
            bit[20:13]:回读数据
            其余保留
   UNION结构 ：无 */
#define BBPCOMM_G2_CFG_DATA5_ADDR                     (SOC_BBP_COMM_BASE_ADDR + 0xE1F4)

/* 寄存器说明：定时模式下gsm2 bbp配置ABB的数据
   详      述：定时模式下gsm2 bbp配置ABB的数据，
            写格式：
            bit[12:0]：写地址
            bit[20:13]:写数据
            其余保留
            读格式：
            bit[12:0]：读地址
            bit[20:13]:回读数据
            其余保留
   UNION结构 ：无 */
#define BBPCOMM_G2_CFG_DATA6_ADDR                     (SOC_BBP_COMM_BASE_ADDR + 0xE1F8)

/* 寄存器说明：定时模式下gsm2 bbp配置ABB的数据
   详      述：定时模式下gsm2 bbp配置ABB的数据，
            写格式：
            bit[12:0]：写地址
            bit[20:13]:写数据
            其余保留
            读格式：
            bit[12:0]：读地址
            bit[20:13]:回读数据
            其余保留
   UNION结构 ：无 */
#define BBPCOMM_G2_CFG_DATA7_ADDR                     (SOC_BBP_COMM_BASE_ADDR + 0xE1FC)

/* 寄存器说明：中断状态存器
 bit[31:4]   保留
 bit[3]      发送fifo中断标志：
             1'b0:无中断
             1'b1:有中断
 bit[2]      接收fifo中断标志：
             1'b0:无中断
             1'b1:有中断
 bit[1]      保留
 bit[0]      接收fifo溢出中断标志：
             1'b0:无中断
             1'b1:有中断
   UNION结构:  BBPCOMM_CFG_INT_STATE_UNION */
#define BBPCOMM_CFG_INT_STATE_ADDR                    (SOC_BBP_COMM_BASE_ADDR + 0xE200)

/* 寄存器说明：FIFO状态存器
 bit[31:12]  保留
 bit[11:8]   rxfifo中数据个数
 bit[7:4]    txfifo中数据个数
 bit[3]      接收fifo满标志：
             1'b0:非满
             1'b1:满
 bit[2]      接收fifo空标志：
             1'b0:非空
             1'b1:空
 bit[1]      发送fifo满标志：
             1'b0:非满
             1'b1:满
 bit[0]      发送fifo空标志：
             1'b0:非空
             1'b1:空
   UNION结构:  BBPCOMM_FIFO_STATE_UNION */
#define BBPCOMM_FIFO_STATE_ADDR                       (SOC_BBP_COMM_BASE_ADDR + 0xE204)

/* 寄存器说明：BBP当前配置状态指示
 bit[31:5]   保留
 bit[4]      g2模当前配置ABB状态指示：
             0：当前没有配置，可以启动新的配置
             1：当前正在配置，不能启动新的配置
 bit[3]      g1模当前配置ABB状态指示：
             0：当前没有配置，可以启动新的配置
             1：当前正在配置，不能启动新的配置
 bit[2]      t模当前配置ABB状态指示：
             0：当前没有配置，可以启动新的配置
             1：当前正在配置，不能启动新的配置
 bit[1]      w模当前配置ABB状态指示：
             0：当前没有配置，可以启动新的配置
             1：当前正在配置，不能启动新的配置
 bit[0]      l模当前配置ABB状态指示：
             0：当前没有配置，可以启动新的配置
             1：当前正在配置，不能启动新的配置
   UNION结构:  BBPCOMM_BBP_RW_STATE_UNION */
#define BBPCOMM_BBP_RW_STATE_ADDR                     (SOC_BBP_COMM_BASE_ADDR + 0xE208)





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

/***======================================================================***
                     (1/5) register_define_ctu
 ***======================================================================***/
/*****************************************************************************
 结构名    : BBPCOMM_C0_PRIMARY_MODE_IND_UNION
 结构说明  : C0_PRIMARY_MODE_IND 寄存器结构定义。地址偏移量:0x0000，初值:0x00000007，宽度:32
 寄存器说明: 通道0对应主模（主模0）的模式指示寄存器
*****************************************************************************/
typedef union
{
    unsigned long      c0_primary_mode_ind_reg;
    struct
    {
        unsigned long  c0_primary_mode_ind : 3;  /* bit[0-2] : 通道0对应的主模（定义为主模0）的模式指示，表示LTE，W，Gm，Gs，TDS谁是主模0。
                                                               000：LTE 主模。
                                                               001：W主模。
                                                               010：TDS主模。
                                                               011：Gm主模；
                                                               100：Gs主模；
                                                               101:C主模；
                                                               110~111：无效。
                                                               配置时刻：各模根据通信场景配置 */
        unsigned long  reserved            : 29; /* bit[3-31]: 保留 */
    } reg;
} BBPCOMM_C0_PRIMARY_MODE_IND_UNION;
#define BBPCOMM_C0_PRIMARY_MODE_IND_c0_primary_mode_ind_START  (0)
#define BBPCOMM_C0_PRIMARY_MODE_IND_c0_primary_mode_ind_END    (2)


/*****************************************************************************
 结构名    : BBPCOMM_C0_SECOND_MODE_IND_UNION
 结构说明  : C0_SECOND_MODE_IND 寄存器结构定义。地址偏移量:0x0004，初值:0x00000007，宽度:32
 寄存器说明: 主模0对应的从模（从模0）的模式指示寄存器
*****************************************************************************/
typedef union
{
    unsigned long      c0_second_mode_ind_reg;
    struct
    {
        unsigned long  c0_second_mode_ind : 3;  /* bit[0-2] : 主模0对应的从模（定义为从模0）的模式指示，表示当前LTE/W/Gm/Gs/TDS谁是从模0。
                                                              000：LTE从模。
                                                              001：W 从模。
                                                              010：TDS从模。
                                                              011：Gm从模；
                                                              100：Gs从模；
                                                              101:C从模；
                                                              110~111：无效。
                                                              配置时刻：各模根据通信场景配置
                                                              注：约定主模0固定走通道0，主模1固定走通道1。 */
        unsigned long  reserved           : 29; /* bit[3-31]: 保留 */
    } reg;
} BBPCOMM_C0_SECOND_MODE_IND_UNION;
#define BBPCOMM_C0_SECOND_MODE_IND_c0_second_mode_ind_START  (0)
#define BBPCOMM_C0_SECOND_MODE_IND_c0_second_mode_ind_END    (2)


/*****************************************************************************
 结构名    : BBPCOMM_C0_AFC_PDM_SELECT_MASK_UNION
 结构说明  : C0_AFC_PDM_SELECT_MASK 寄存器结构定义。地址偏移量:0x0010，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      c0_afc_pdm_select_mask_reg;
    struct
    {
        unsigned long  c0_afc_pdm_select_mask : 3;  /* bit[0-2] : 通道0 AFC控制信号来源。
                                                                  3'b000，受主模控制；
                                                                  3'b001，强制受L模控制；
                                                                  3'b010，强制受W模控制；
                                                                  3'b011，强制受TDS模控制；
                                                                  3'b100，强制受Gm模控制；
                                                                  3'b101，强制受Gs模控制；
                                                                  3'b110，强制受C模控制；
                                                                  3'b111，无效。
                                                                  调试用。 */
        unsigned long  reserved               : 29; /* bit[3-31]: 保留 */
    } reg;
} BBPCOMM_C0_AFC_PDM_SELECT_MASK_UNION;
#define BBPCOMM_C0_AFC_PDM_SELECT_MASK_c0_afc_pdm_select_mask_START  (0)
#define BBPCOMM_C0_AFC_PDM_SELECT_MASK_c0_afc_pdm_select_mask_END    (2)


/*****************************************************************************
 结构名    : BBPCOMM_C0_ANT_SWITCH_PA_SELECT_MASK_UNION
 结构说明  : C0_ANT_SWITCH_PA_SELECT_MASK 寄存器结构定义。地址偏移量:0x0014，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      c0_ant_switch_pa_select_mask_reg;
    struct
    {
        unsigned long  c0_ant_switch_pa_select_mask : 3;  /* bit[0-2] : 通道0 天线开关和PA控制信号来源。
                                                                        3'b000，受主模控制；
                                                                        3'b001，强制受L模控制；
                                                                        3'b010，强制受W模控制；
                                                                        3'b011，强制受TDS模控制；
                                                                        3'b100，强制受Gm模控制；
                                                                        3'b101，强制受Gs模控制；
                                                                        3'b110，强制受C模控制；
                                                                        3'b111，无效。
                                                                        调试用。 */
        unsigned long  reserved                     : 29; /* bit[3-31]: 保留 */
    } reg;
} BBPCOMM_C0_ANT_SWITCH_PA_SELECT_MASK_UNION;
#define BBPCOMM_C0_ANT_SWITCH_PA_SELECT_MASK_c0_ant_switch_pa_select_mask_START  (0)
#define BBPCOMM_C0_ANT_SWITCH_PA_SELECT_MASK_c0_ant_switch_pa_select_mask_END    (2)


/*****************************************************************************
 结构名    : BBPCOMM_C0_RFIC_SELECT_MASK_UNION
 结构说明  : C0_RFIC_SELECT_MASK 寄存器结构定义。地址偏移量:0x0018，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      c0_rfic_select_mask_reg;
    struct
    {
        unsigned long  c0_rfic_select_mask : 3;  /* bit[0-2] : 通道0 RFIC控制信号来源（包括线控和SSI）。
                                                               3'b000，受主模控制；
                                                               3'b001，强制受L模控制；
                                                               3'b010，强制受W模控制；
                                                               3'b011，强制受TDS模控制；
                                                               3'b100，强制受Gm模控制；
                                                               3'b101，强制受Gs模控制；
                                                               3'b110，强制受C模控制；
                                                               3'b111，无效。
                                                               调试用。 */
        unsigned long  reserved            : 29; /* bit[3-31]: 保留 */
    } reg;
} BBPCOMM_C0_RFIC_SELECT_MASK_UNION;
#define BBPCOMM_C0_RFIC_SELECT_MASK_c0_rfic_select_mask_START  (0)
#define BBPCOMM_C0_RFIC_SELECT_MASK_c0_rfic_select_mask_END    (2)


/*****************************************************************************
 结构名    : BBPCOMM_C0_MIPI_SELECT_MASK_UNION
 结构说明  : C0_MIPI_SELECT_MASK 寄存器结构定义。地址偏移量:0x001C，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      c0_mipi_select_mask_reg;
    struct
    {
        unsigned long  c0_mipi_select_mask : 3;  /* bit[0-2] : 通道0 MIPI控制信号来源。
                                                               3'b000，受主模控制；
                                                               3'b001，强制受L模控制；
                                                               3'b010，强制受W模控制；
                                                               3'b011，强制受TDS模控制；
                                                               3'b100，强制受Gm模控制；
                                                               3'b101，强制受Gs模控制；
                                                               3'b110，强制受C模控制；
                                                               其他，无效。
                                                               调试用。 */
        unsigned long  reserved            : 29; /* bit[3-31]: 保留 */
    } reg;
} BBPCOMM_C0_MIPI_SELECT_MASK_UNION;
#define BBPCOMM_C0_MIPI_SELECT_MASK_c0_mipi_select_mask_START  (0)
#define BBPCOMM_C0_MIPI_SELECT_MASK_c0_mipi_select_mask_END    (2)


/*****************************************************************************
 结构名    : BBPCOMM_C0_ABB_SELECT_MASK_UNION
 结构说明  : C0_ABB_SELECT_MASK 寄存器结构定义。地址偏移量:0x0020，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      c0_abb_select_mask_reg;
    struct
    {
        unsigned long  c0_abb_select_mask : 3;  /* bit[0-2] : 通道0 ABB控制信号来源。
                                                              3'b000，受主模控制；
                                                              3'b001，强制受L模控制；
                                                              3'b010，强制受W模控制；
                                                              3'b011，强制受TDS模控制；
                                                              3'b100，强制受Gm模控制；
                                                              3'b101，强制受Gs模控制；
                                                              3'b110，强制受C模控制；
                                                              其他，无效。
                                                              调试用。 */
        unsigned long  reserved           : 29; /* bit[3-31]: 保留 */
    } reg;
} BBPCOMM_C0_ABB_SELECT_MASK_UNION;
#define BBPCOMM_C0_ABB_SELECT_MASK_c0_abb_select_mask_START  (0)
#define BBPCOMM_C0_ABB_SELECT_MASK_c0_abb_select_mask_END    (2)


/*****************************************************************************
 结构名    : BBPCOMM_C0_PMU_SELECT_MASK_UNION
 结构说明  : C0_PMU_SELECT_MASK 寄存器结构定义。地址偏移量:0x0024，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      c0_pmu_select_mask_reg;
    struct
    {
        unsigned long  c0_pmu_select_mask : 3;  /* bit[0-2] : 通道0 PMU控制信号来源。
                                                              3'b000，受主模控制；
                                                              3'b001，强制受L模控制；
                                                              3'b010，强制受W模控制；
                                                              3'b011，强制受TDS模控制；
                                                              3'b100，强制受Gm模控制；
                                                              3'b101，强制受Gs模控制；
                                                              3'b110，强制受C模控制；
                                                              3'b111，无效。
                                                              调试用。 */
        unsigned long  reserved           : 29; /* bit[3-31]: 保留 */
    } reg;
} BBPCOMM_C0_PMU_SELECT_MASK_UNION;
#define BBPCOMM_C0_PMU_SELECT_MASK_c0_pmu_select_mask_START  (0)
#define BBPCOMM_C0_PMU_SELECT_MASK_c0_pmu_select_mask_END    (2)


/*****************************************************************************
 结构名    : BBPCOMM_DSP_C0_SEL_MASK_UNION
 结构说明  : DSP_C0_SEL_MASK 寄存器结构定义。地址偏移量:0x0028，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      dsp_c0_sel_mask_reg;
    struct
    {
        unsigned long  dsp_c0_sel_mask : 3;  /* bit[0-2] : 当前通道0（交叉之前）输出的强配模式。
                                                           注：其适用于各个外围器件（ABB、RF、PA、mipi等）公共的交叉强配。
                                                           调试用。 */
        unsigned long  reserved        : 29; /* bit[3-31]: 保留 */
    } reg;
} BBPCOMM_DSP_C0_SEL_MASK_UNION;
#define BBPCOMM_DSP_C0_SEL_MASK_dsp_c0_sel_mask_START  (0)
#define BBPCOMM_DSP_C0_SEL_MASK_dsp_c0_sel_mask_END    (2)


/*****************************************************************************
 结构名    : BBPCOMM_DSP_RFIC0_RSTN_UNION
 结构说明  : DSP_RFIC0_RSTN 寄存器结构定义。地址偏移量:0x002C，初值:0x00000001，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      dsp_rfic0_rstn_reg;
    struct
    {
        unsigned long  dsp_rfic0_rstn : 1;  /* bit[0-0] : 软件配置的RFIC0复位信号，低有效。
                                                          0：复位，1：解复位。
                                                          上电初始化配置为0，根据RFIC手册要求，维持一段时间后配置为1。
                                                          V3R3，该寄存器功能改在寄存器0x0334的bit[8]实现，默认值改为0，此处删除。
                                                          V7R2复位值为0。
                                                          V8R1：该寄存器功能删除。 */
        unsigned long  reserved       : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_DSP_RFIC0_RSTN_UNION;
#define BBPCOMM_DSP_RFIC0_RSTN_dsp_rfic0_rstn_START  (0)
#define BBPCOMM_DSP_RFIC0_RSTN_dsp_rfic0_rstn_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_C0_SECOND_AFC_MODE_UNION
 结构说明  : C0_SECOND_AFC_MODE 寄存器结构定义。地址偏移量:0x0030，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      c0_second_afc_mode_reg;
    struct
    {
        unsigned long  c0_second_afc_mode : 1;  /* bit[0]   : 0:通道0从模使用通道0主模afc控制字
                                                              1:通道0从模使用通道0从模afc控制字 */
        unsigned long  reserved           : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_C0_SECOND_AFC_MODE_UNION;
#define BBPCOMM_C0_SECOND_AFC_MODE_c0_second_afc_mode_START  (0)
#define BBPCOMM_C0_SECOND_AFC_MODE_c0_second_afc_mode_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_MIPI0_RD_OVERTIME_FLAG_DSP_UNION
 结构说明  : MIPI0_RD_OVERTIME_FLAG_DSP 寄存器结构定义。地址偏移量:0x0038，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      mipi0_rd_overtime_flag_dsp_reg;
    struct
    {
        unsigned long  mipi0_rd_overtime_flag_dsp : 1;  /* bit[0]   : DSP等待回读结果超时标志位
                                                                      1:超时   */
        unsigned long  reserved                   : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_MIPI0_RD_OVERTIME_FLAG_DSP_UNION;
#define BBPCOMM_MIPI0_RD_OVERTIME_FLAG_DSP_mipi0_rd_overtime_flag_dsp_START  (0)
#define BBPCOMM_MIPI0_RD_OVERTIME_FLAG_DSP_mipi0_rd_overtime_flag_dsp_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_SSI0_RD_OVERTIME_FLAG_DSP_UNION
 结构说明  : SSI0_RD_OVERTIME_FLAG_DSP 寄存器结构定义。地址偏移量:0x003C，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      ssi0_rd_overtime_flag_dsp_reg;
    struct
    {
        unsigned long  ssi0_rd_overtime_flag_dsp : 1;  /* bit[0]   : DSP等待回读结果超时标志位
                                                                     1:超时   */
        unsigned long  reserved                  : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_SSI0_RD_OVERTIME_FLAG_DSP_UNION;
#define BBPCOMM_SSI0_RD_OVERTIME_FLAG_DSP_ssi0_rd_overtime_flag_dsp_START  (0)
#define BBPCOMM_SSI0_RD_OVERTIME_FLAG_DSP_ssi0_rd_overtime_flag_dsp_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_MIPI0_RD_OVERTIME_CLR_UNION
 结构说明  : MIPI0_RD_OVERTIME_CLR 寄存器结构定义。地址偏移量:0x0040，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      mipi0_rd_overtime_clr_reg;
    struct
    {
        unsigned long  mipi0_rd_overtime_clr_imi : 1;  /* bit[0]   : DSP等待回读结果超时后,清抄时标志位脉冲 */
        unsigned long  reserved                  : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_MIPI0_RD_OVERTIME_CLR_UNION;
#define BBPCOMM_MIPI0_RD_OVERTIME_CLR_mipi0_rd_overtime_clr_imi_START  (0)
#define BBPCOMM_MIPI0_RD_OVERTIME_CLR_mipi0_rd_overtime_clr_imi_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_SSI0_RD_OVERTIME_CLR_UNION
 结构说明  : SSI0_RD_OVERTIME_CLR 寄存器结构定义。地址偏移量:0x0044，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      ssi0_rd_overtime_clr_reg;
    struct
    {
        unsigned long  ssi0_rd_overtime_clr_imi : 1;  /* bit[0]   : DSP等待回读结果超时后,清抄时标志位脉冲 */
        unsigned long  reserved                 : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_SSI0_RD_OVERTIME_CLR_UNION;
#define BBPCOMM_SSI0_RD_OVERTIME_CLR_ssi0_rd_overtime_clr_imi_START  (0)
#define BBPCOMM_SSI0_RD_OVERTIME_CLR_ssi0_rd_overtime_clr_imi_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_RD_RESULT_FLAG_RFIC0_UNION
 结构说明  : RD_RESULT_FLAG_RFIC0 寄存器结构定义。地址偏移量:0x0088，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      rd_result_flag_rfic0_reg;
    struct
    {
        unsigned long  rd_result_flag_rfic0 : 1;  /* bit[0]   : 所有回读指令执行完毕,可以取走回读数据
                                                                1:所有回读指令发送完毕,DSP可以按位置读取SSI回读结果 */
        unsigned long  reserved             : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_RD_RESULT_FLAG_RFIC0_UNION;
#define BBPCOMM_RD_RESULT_FLAG_RFIC0_rd_result_flag_rfic0_START  (0)
#define BBPCOMM_RD_RESULT_FLAG_RFIC0_rd_result_flag_rfic0_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_RD_RESULT_FLAG_RFIC0_CLR_UNION
 结构说明  : RD_RESULT_FLAG_RFIC0_CLR 寄存器结构定义。地址偏移量:0x008C，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      rd_result_flag_rfic0_clr_reg;
    struct
    {
        unsigned long  rd_result_flag_rfic0_clr : 1;  /* bit[0]   : 1:DSP取走所有回读数据后,写入clr信号清标志位 */
        unsigned long  reserved                 : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_RD_RESULT_FLAG_RFIC0_CLR_UNION;
#define BBPCOMM_RD_RESULT_FLAG_RFIC0_CLR_rd_result_flag_rfic0_clr_START  (0)
#define BBPCOMM_RD_RESULT_FLAG_RFIC0_CLR_rd_result_flag_rfic0_clr_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_RD_RESULT_FLAG_MIPI0_UNION
 结构说明  : RD_RESULT_FLAG_MIPI0 寄存器结构定义。地址偏移量:0x00D0，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      rd_result_flag_mipi0_reg;
    struct
    {
        unsigned long  rd_result_flag_mipi0 : 1;  /* bit[0]   : 所有回读指令执行完毕,可以取走回读数据
                                                                1:所有回读指令发送完毕,DSP可以按位置读取SSI回读结果,与下述的软件直接控制MIPI标志位不同 */
        unsigned long  reserved             : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_RD_RESULT_FLAG_MIPI0_UNION;
#define BBPCOMM_RD_RESULT_FLAG_MIPI0_rd_result_flag_mipi0_START  (0)
#define BBPCOMM_RD_RESULT_FLAG_MIPI0_rd_result_flag_mipi0_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_RD_RESULT_FLAG_MIPI0_CLR_UNION
 结构说明  : RD_RESULT_FLAG_MIPI0_CLR 寄存器结构定义。地址偏移量:0x00D4，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      rd_result_flag_mipi0_clr_reg;
    struct
    {
        unsigned long  rd_result_flag_mipi0_clr : 1;  /* bit[0]   : 1:DSP取走所有回读数据后,写入clr信号清标志位,与下述的软件直接控制MIPI标志位不同 */
        unsigned long  reserved                 : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_RD_RESULT_FLAG_MIPI0_CLR_UNION;
#define BBPCOMM_RD_RESULT_FLAG_MIPI0_CLR_rd_result_flag_mipi0_clr_START  (0)
#define BBPCOMM_RD_RESULT_FLAG_MIPI0_CLR_rd_result_flag_mipi0_clr_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_RD_END_FLAG_MIPI0_SOFT_UNION
 结构说明  : RD_END_FLAG_MIPI0_SOFT 寄存器结构定义。地址偏移量:0x00E0，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      rd_end_flag_mipi0_soft_reg;
    struct
    {
        unsigned long  rd_end_flag_mipi0_soft : 1;  /* bit[0]   : 由软件直接控制MIPI0 master,不借助xbbp发送回读指令时,回读数据有效指示 */
        unsigned long  reserved               : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_RD_END_FLAG_MIPI0_SOFT_UNION;
#define BBPCOMM_RD_END_FLAG_MIPI0_SOFT_rd_end_flag_mipi0_soft_START  (0)
#define BBPCOMM_RD_END_FLAG_MIPI0_SOFT_rd_end_flag_mipi0_soft_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_MIPI0_GRANT_DSP_UNION
 结构说明  : MIPI0_GRANT_DSP 寄存器结构定义。地址偏移量:0x00E4，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      mipi0_grant_dsp_reg;
    struct
    {
        unsigned long  mipi0_grant_dsp : 1;  /* bit[0]   : 由软件直接控制MIPI0 master
                                                           1:软件可以发送CFG_IND启动mipi0
                                                           0:软件等待mipi0授权使用 */
        unsigned long  reserved        : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_MIPI0_GRANT_DSP_UNION;
#define BBPCOMM_MIPI0_GRANT_DSP_mipi0_grant_dsp_START  (0)
#define BBPCOMM_MIPI0_GRANT_DSP_mipi0_grant_dsp_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_DSP_MIPI0_EN_IMI_UNION
 结构说明  : DSP_MIPI0_EN_IMI 寄存器结构定义。地址偏移量:0x00F0，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      dsp_mipi0_en_imi_reg;
    struct
    {
        unsigned long  dsp_mipi0_en_imi : 1;  /* bit[0]   : 由软件直接控制MIPI0 master,需首先拉高此指示,屏蔽xbbp对MIPI0的控制 */
        unsigned long  reserved         : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_DSP_MIPI0_EN_IMI_UNION;
#define BBPCOMM_DSP_MIPI0_EN_IMI_dsp_mipi0_en_imi_START  (0)
#define BBPCOMM_DSP_MIPI0_EN_IMI_dsp_mipi0_en_imi_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_DSP_MIPI0_CFG_IND_IMI_UNION
 结构说明  : DSP_MIPI0_CFG_IND_IMI 寄存器结构定义。地址偏移量:0x00F4，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      dsp_mipi0_cfg_ind_imi_reg;
    struct
    {
        unsigned long  dsp_mipi0_cfg_ind_imi : 1;  /* bit[0]   : 由软件直接控制MIPI0 master,拉高此dsp_mipi0_en后,产生软件启动脉冲 */
        unsigned long  reserved              : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_DSP_MIPI0_CFG_IND_IMI_UNION;
#define BBPCOMM_DSP_MIPI0_CFG_IND_IMI_dsp_mipi0_cfg_ind_imi_START  (0)
#define BBPCOMM_DSP_MIPI0_CFG_IND_IMI_dsp_mipi0_cfg_ind_imi_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_DSP_MIPI0_RD_CLR_UNION
 结构说明  : DSP_MIPI0_RD_CLR 寄存器结构定义。地址偏移量:0x00F8，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      dsp_mipi0_rd_clr_reg;
    struct
    {
        unsigned long  dsp_mipi0_rd_clr : 1;  /* bit[0]   : 由软件直接控制MIPI0 master,取走回读数据后,清标志位脉冲 */
        unsigned long  reserved         : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_DSP_MIPI0_RD_CLR_UNION;
#define BBPCOMM_DSP_MIPI0_RD_CLR_dsp_mipi0_rd_clr_START  (0)
#define BBPCOMM_DSP_MIPI0_RD_CLR_dsp_mipi0_rd_clr_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_ABB0_LINE_CONTROL_CMD_UNION
 结构说明  : ABB0_LINE_CONTROL_CMD 寄存器结构定义。地址偏移量:0x00FC，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      abb0_line_control_cmd_reg;
    struct
    {
        unsigned long  abb0_line_control_cmd_sel : 1;  /* bit[0-0] : ABB0线控强配模式：
                                                                     1：表示软件强配，此时上述各个强配才生效；
                                                                     0：表示由CTU根据各BBP产生。
                                                                     默认值为0。
                                                                     测试用。 */
        unsigned long  abb0_mode_sel_cmd         : 3;  /* bit[1-3] : 软件强配模式abb0_mode_sel. */
        unsigned long  abb0_tx_en_cmd            : 1;  /* bit[4-4] : 软件强配模式abb0_tx_en. */
        unsigned long  abb0_rxa_en_cmd           : 1;  /* bit[5-5] : 软件强配模式abb0_rxa_en. */
        unsigned long  abb0_rxb_en_cmd           : 1;  /* bit[6-6] : 软件强配模式abb0_rxb_en. */
        unsigned long  abb0_blka_en_cmd          : 1;  /* bit[7-7] : 软件强配模式abb0_rxa_blk_en. */
        unsigned long  abb0_blkb_en_cmd          : 1;  /* bit[8-8] : 软件强配模式abb0_rxb_blk_en. */
        unsigned long  reserved                  : 23; /* bit[9-31]: 保留 */
    } reg;
} BBPCOMM_ABB0_LINE_CONTROL_CMD_UNION;
#define BBPCOMM_ABB0_LINE_CONTROL_CMD_abb0_line_control_cmd_sel_START  (0)
#define BBPCOMM_ABB0_LINE_CONTROL_CMD_abb0_line_control_cmd_sel_END    (0)
#define BBPCOMM_ABB0_LINE_CONTROL_CMD_abb0_mode_sel_cmd_START          (1)
#define BBPCOMM_ABB0_LINE_CONTROL_CMD_abb0_mode_sel_cmd_END            (3)
#define BBPCOMM_ABB0_LINE_CONTROL_CMD_abb0_tx_en_cmd_START             (4)
#define BBPCOMM_ABB0_LINE_CONTROL_CMD_abb0_tx_en_cmd_END               (4)
#define BBPCOMM_ABB0_LINE_CONTROL_CMD_abb0_rxa_en_cmd_START            (5)
#define BBPCOMM_ABB0_LINE_CONTROL_CMD_abb0_rxa_en_cmd_END              (5)
#define BBPCOMM_ABB0_LINE_CONTROL_CMD_abb0_rxb_en_cmd_START            (6)
#define BBPCOMM_ABB0_LINE_CONTROL_CMD_abb0_rxb_en_cmd_END              (6)
#define BBPCOMM_ABB0_LINE_CONTROL_CMD_abb0_blka_en_cmd_START           (7)
#define BBPCOMM_ABB0_LINE_CONTROL_CMD_abb0_blka_en_cmd_END             (7)
#define BBPCOMM_ABB0_LINE_CONTROL_CMD_abb0_blkb_en_cmd_START           (8)
#define BBPCOMM_ABB0_LINE_CONTROL_CMD_abb0_blkb_en_cmd_END             (8)


/*****************************************************************************
 结构名    : BBPCOMM_CH0_BBP_SEL_UNION
 结构说明  : CH0_BBP_SEL 寄存器结构定义。地址偏移量:0x0100，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      ch0_bbp_sel_reg;
    struct
    {
        unsigned long  ch0_bbp_sel : 3;  /* bit[0-2] : V9R1版本，软件不配置。 */
        unsigned long  reserved    : 29; /* bit[3-31]: 保留 */
    } reg;
} BBPCOMM_CH0_BBP_SEL_UNION;
#define BBPCOMM_CH0_BBP_SEL_ch0_bbp_sel_START  (0)
#define BBPCOMM_CH0_BBP_SEL_ch0_bbp_sel_END    (2)


/*****************************************************************************
 结构名    : BBPCOMM_CPU_MIPI0_FUNC_SEL_UNION
 结构说明  : CPU_MIPI0_FUNC_SEL 寄存器结构定义。地址偏移量:0x0104，初值:0x00000001，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      cpu_mipi0_func_sel_reg;
    struct
    {
        unsigned long  cpu_mipi0_func_sel : 1;  /* bit[0]   : cpu配置mipi功能,配置为1才能正常工作, 测试时配置为0 */
        unsigned long  reserved           : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_CPU_MIPI0_FUNC_SEL_UNION;
#define BBPCOMM_CPU_MIPI0_FUNC_SEL_cpu_mipi0_func_sel_START  (0)
#define BBPCOMM_CPU_MIPI0_FUNC_SEL_cpu_mipi0_func_sel_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_CPU_MIPI0_TEST_FUNC_UNION
 结构说明  : CPU_MIPI0_TEST_FUNC 寄存器结构定义。地址偏移量:0x0108，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      cpu_mipi0_test_func_reg;
    struct
    {
        unsigned long  cpu_mipi0_test_func : 1;  /* bit[0]   : cpu配置mipi功能,测试用 */
        unsigned long  reserved            : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_CPU_MIPI0_TEST_FUNC_UNION;
#define BBPCOMM_CPU_MIPI0_TEST_FUNC_cpu_mipi0_test_func_START  (0)
#define BBPCOMM_CPU_MIPI0_TEST_FUNC_cpu_mipi0_test_func_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_CPU_MIPI0_SCLK_TEST_UNION
 结构说明  : CPU_MIPI0_SCLK_TEST 寄存器结构定义。地址偏移量:0x010C，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      cpu_mipi0_sclk_test_reg;
    struct
    {
        unsigned long  cpu_mipi0_sclk_test : 1;  /* bit[0]   : cpu配置mipi功能, 测试用 */
        unsigned long  reserved            : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_CPU_MIPI0_SCLK_TEST_UNION;
#define BBPCOMM_CPU_MIPI0_SCLK_TEST_cpu_mipi0_sclk_test_START  (0)
#define BBPCOMM_CPU_MIPI0_SCLK_TEST_cpu_mipi0_sclk_test_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_CPU_MIPI0_SDATA_TEST_UNION
 结构说明  : CPU_MIPI0_SDATA_TEST 寄存器结构定义。地址偏移量:0x0110，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      cpu_mipi0_sdata_test_reg;
    struct
    {
        unsigned long  cpu_mipi0_sdata_test : 1;  /* bit[0]   : cpu配置mipi功能, 测试用 */
        unsigned long  reserved             : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_CPU_MIPI0_SDATA_TEST_UNION;
#define BBPCOMM_CPU_MIPI0_SDATA_TEST_cpu_mipi0_sdata_test_START  (0)
#define BBPCOMM_CPU_MIPI0_SDATA_TEST_cpu_mipi0_sdata_test_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_DSP_MIPI0_CLR_IND_IMI_UNION
 结构说明  : DSP_MIPI0_CLR_IND_IMI 寄存器结构定义。地址偏移量:0x0114，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      dsp_mipi0_clr_ind_imi_reg;
    struct
    {
        unsigned long  dsp_mipi0_clr_ind_imi : 1;  /* bit[0]   : cpu配置mipi功能,mipi0master清异常信号 */
        unsigned long  reserved              : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_DSP_MIPI0_CLR_IND_IMI_UNION;
#define BBPCOMM_DSP_MIPI0_CLR_IND_IMI_dsp_mipi0_clr_ind_imi_START  (0)
#define BBPCOMM_DSP_MIPI0_CLR_IND_IMI_dsp_mipi0_clr_ind_imi_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_CPU_MIPI0_FIFO_CLR_IMI_UNION
 结构说明  : CPU_MIPI0_FIFO_CLR_IMI 寄存器结构定义。地址偏移量:0x0118，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      cpu_mipi0_fifo_clr_imi_reg;
    struct
    {
        unsigned long  cpu_mipi0_fifo_clr_imi : 1;  /* bit[0]   : mipi0存储待发送数据同步FIFO复位信号 */
        unsigned long  reserved               : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_CPU_MIPI0_FIFO_CLR_IMI_UNION;
#define BBPCOMM_CPU_MIPI0_FIFO_CLR_IMI_cpu_mipi0_fifo_clr_imi_START  (0)
#define BBPCOMM_CPU_MIPI0_FIFO_CLR_IMI_cpu_mipi0_fifo_clr_imi_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_CPU_SSI0_FIFO_CLR_IMI_UNION
 结构说明  : CPU_SSI0_FIFO_CLR_IMI 寄存器结构定义。地址偏移量:0x011C，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      cpu_ssi0_fifo_clr_imi_reg;
    struct
    {
        unsigned long  cpu_ssi0_fifo_clr_imi : 1;  /* bit[0]   : ssi0存储待发送数据同步FIFO复位信号 */
        unsigned long  reserved              : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_CPU_SSI0_FIFO_CLR_IMI_UNION;
#define BBPCOMM_CPU_SSI0_FIFO_CLR_IMI_cpu_ssi0_fifo_clr_imi_START  (0)
#define BBPCOMM_CPU_SSI0_FIFO_CLR_IMI_cpu_ssi0_fifo_clr_imi_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_CPU_RX0_TX0_CKG_BYPASS_UNION
 结构说明  : CPU_RX0_TX0_CKG_BYPASS 寄存器结构定义。地址偏移量:0x0120，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      cpu_rx0_tx0_ckg_bypass_reg;
    struct
    {
        unsigned long  cpu_rx0_lte_ckg_bypass : 1;  /* bit[0]    : L模通道0 RX ABB随路时钟门控旁路信号 */
        unsigned long  cpu_rx0_tds_ckg_bypass : 1;  /* bit[1]    : TDS通道0 RX ABB随路时钟门控旁路信号 */
        unsigned long  cpu_rx0_w_ckg_bypass   : 1;  /* bit[2]    : W模通道0 RX ABB随路时钟门控旁路信号 */
        unsigned long  cpu_rx0_g1_ckg_bypass  : 1;  /* bit[3]    : G1通道0 RX ABB随路时钟门控旁路信号 */
        unsigned long  cpu_rx0_g2_ckg_bypass  : 1;  /* bit[4]    : G2通道0 RX ABB随路时钟门控旁路信号 */
        unsigned long  cpu_tx0_lte_ckg_bypass : 1;  /* bit[5]    : L模通道0 TX ABB随路时钟门控旁路信号 */
        unsigned long  cpu_tx0_tds_ckg_bypass : 1;  /* bit[6]    : TDS通道0 TX ABB随路时钟门控旁路信号 */
        unsigned long  cpu_tx0_w_ckg_bypass   : 1;  /* bit[7]    : W模通道0 TX ABB随路时钟门控旁路信号 */
        unsigned long  cpu_tx0_g1_ckg_bypass  : 1;  /* bit[8]    : G1通道0 TX ABB随路时钟门控旁路信号 */
        unsigned long  cpu_tx0_g2_ckg_bypass  : 1;  /* bit[9]    : G2通道0 TX ABB随路时钟门控旁路信号 */
        unsigned long  cpu_rx0_c_ckg_bypass   : 1;  /* bit[10]   : C模通道0 RX ABB随路时钟门控旁路信号 */
        unsigned long  cpu_tx0_c_ckg_bypass   : 1;  /* bit[11]   : C模通道0 TX ABB随路时钟门控旁路信号 */
        unsigned long  reserved               : 20; /* bit[12-31]: 保留 */
    } reg;
} BBPCOMM_CPU_RX0_TX0_CKG_BYPASS_UNION;
#define BBPCOMM_CPU_RX0_TX0_CKG_BYPASS_cpu_rx0_lte_ckg_bypass_START  (0)
#define BBPCOMM_CPU_RX0_TX0_CKG_BYPASS_cpu_rx0_lte_ckg_bypass_END    (0)
#define BBPCOMM_CPU_RX0_TX0_CKG_BYPASS_cpu_rx0_tds_ckg_bypass_START  (1)
#define BBPCOMM_CPU_RX0_TX0_CKG_BYPASS_cpu_rx0_tds_ckg_bypass_END    (1)
#define BBPCOMM_CPU_RX0_TX0_CKG_BYPASS_cpu_rx0_w_ckg_bypass_START    (2)
#define BBPCOMM_CPU_RX0_TX0_CKG_BYPASS_cpu_rx0_w_ckg_bypass_END      (2)
#define BBPCOMM_CPU_RX0_TX0_CKG_BYPASS_cpu_rx0_g1_ckg_bypass_START   (3)
#define BBPCOMM_CPU_RX0_TX0_CKG_BYPASS_cpu_rx0_g1_ckg_bypass_END     (3)
#define BBPCOMM_CPU_RX0_TX0_CKG_BYPASS_cpu_rx0_g2_ckg_bypass_START   (4)
#define BBPCOMM_CPU_RX0_TX0_CKG_BYPASS_cpu_rx0_g2_ckg_bypass_END     (4)
#define BBPCOMM_CPU_RX0_TX0_CKG_BYPASS_cpu_tx0_lte_ckg_bypass_START  (5)
#define BBPCOMM_CPU_RX0_TX0_CKG_BYPASS_cpu_tx0_lte_ckg_bypass_END    (5)
#define BBPCOMM_CPU_RX0_TX0_CKG_BYPASS_cpu_tx0_tds_ckg_bypass_START  (6)
#define BBPCOMM_CPU_RX0_TX0_CKG_BYPASS_cpu_tx0_tds_ckg_bypass_END    (6)
#define BBPCOMM_CPU_RX0_TX0_CKG_BYPASS_cpu_tx0_w_ckg_bypass_START    (7)
#define BBPCOMM_CPU_RX0_TX0_CKG_BYPASS_cpu_tx0_w_ckg_bypass_END      (7)
#define BBPCOMM_CPU_RX0_TX0_CKG_BYPASS_cpu_tx0_g1_ckg_bypass_START   (8)
#define BBPCOMM_CPU_RX0_TX0_CKG_BYPASS_cpu_tx0_g1_ckg_bypass_END     (8)
#define BBPCOMM_CPU_RX0_TX0_CKG_BYPASS_cpu_tx0_g2_ckg_bypass_START   (9)
#define BBPCOMM_CPU_RX0_TX0_CKG_BYPASS_cpu_tx0_g2_ckg_bypass_END     (9)
#define BBPCOMM_CPU_RX0_TX0_CKG_BYPASS_cpu_rx0_c_ckg_bypass_START    (10)
#define BBPCOMM_CPU_RX0_TX0_CKG_BYPASS_cpu_rx0_c_ckg_bypass_END      (10)
#define BBPCOMM_CPU_RX0_TX0_CKG_BYPASS_cpu_tx0_c_ckg_bypass_START    (11)
#define BBPCOMM_CPU_RX0_TX0_CKG_BYPASS_cpu_tx0_c_ckg_bypass_END      (11)


/*****************************************************************************
 结构名    : BBPCOMM_C1_PRIMARY_MODE_IND_UNION
 结构说明  : C1_PRIMARY_MODE_IND 寄存器结构定义。地址偏移量:0x0300，初值:0x00000007，宽度:32
 寄存器说明: 通道1对应主模（主模1）的模式指示寄存器
*****************************************************************************/
typedef union
{
    unsigned long      c1_primary_mode_ind_reg;
    struct
    {
        unsigned long  c1_primary_mode_ind : 3;  /* bit[0-2] : 通道1对应的主模（定义为主模1）的模式指示，表示LTE，W，Gm，Gs，TDS谁是主模1。
                                                               000：LTE 主模。
                                                               001：W主模。
                                                               010：TDS主模。
                                                               011：Gm主模；
                                                               100：Gs主模；
                                                               101：C主模；
                                                               110~111：无效。
                                                               配置时刻：各模根据通信场景配置 */
        unsigned long  reserved            : 29; /* bit[3-31]: 保留 */
    } reg;
} BBPCOMM_C1_PRIMARY_MODE_IND_UNION;
#define BBPCOMM_C1_PRIMARY_MODE_IND_c1_primary_mode_ind_START  (0)
#define BBPCOMM_C1_PRIMARY_MODE_IND_c1_primary_mode_ind_END    (2)


/*****************************************************************************
 结构名    : BBPCOMM_C1_SECOND_MODE_IND_UNION
 结构说明  : C1_SECOND_MODE_IND 寄存器结构定义。地址偏移量:0x0304，初值:0x00000007，宽度:32
 寄存器说明: 主模1对应的从模（从模1）的模式指示寄存器
*****************************************************************************/
typedef union
{
    unsigned long      c1_second_mode_ind_reg;
    struct
    {
        unsigned long  c1_second_mode_ind : 3;  /* bit[0-2] : 主模1对应的从模（定义为从模1）的模式指示，表示LTE/W/Gm/Gs/TDS谁是从模1。
                                                              000：LTE从模。
                                                              001：W 从模。
                                                              010：TDS从模。
                                                              011：Gm从模；
                                                              100：Gs从模；
                                                              101：C从模；
                                                              110~111：无效。
                                                              配置时刻：各模根据通信场景配置.
                                                              注：约定主模0固定走通道0，主模1固定走通道1。 */
        unsigned long  reserved           : 29; /* bit[3-31]: 保留 */
    } reg;
} BBPCOMM_C1_SECOND_MODE_IND_UNION;
#define BBPCOMM_C1_SECOND_MODE_IND_c1_second_mode_ind_START  (0)
#define BBPCOMM_C1_SECOND_MODE_IND_c1_second_mode_ind_END    (2)


/*****************************************************************************
 结构名    : BBPCOMM_C1_AFC_PDM_SELECT_MASK_UNION
 结构说明  : C1_AFC_PDM_SELECT_MASK 寄存器结构定义。地址偏移量:0x0310，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      c1_afc_pdm_select_mask_reg;
    struct
    {
        unsigned long  c1_afc_pdm_select_mask : 3;  /* bit[0-2] : 通道1 AFC控制信号来源。
                                                                  3'b000，受主模控制；
                                                                  3'b001，强制受L模控制；
                                                                  3'b010，强制受W模控制；
                                                                  3'b011，强制受TDS模控制；
                                                                  3'b100，强制受Gm模控制；
                                                                  3'b101，强制受Gs模控制；
                                                                  3'b110，强制受C模控制；
                                                                  3'b111，无效。
                                                                  调试用。 */
        unsigned long  reserved               : 29; /* bit[3-31]: 保留 */
    } reg;
} BBPCOMM_C1_AFC_PDM_SELECT_MASK_UNION;
#define BBPCOMM_C1_AFC_PDM_SELECT_MASK_c1_afc_pdm_select_mask_START  (0)
#define BBPCOMM_C1_AFC_PDM_SELECT_MASK_c1_afc_pdm_select_mask_END    (2)


/*****************************************************************************
 结构名    : BBPCOMM_C1_ANT_SWITCH_PA_SELECT_MASK_UNION
 结构说明  : C1_ANT_SWITCH_PA_SELECT_MASK 寄存器结构定义。地址偏移量:0x0314，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      c1_ant_switch_pa_select_mask_reg;
    struct
    {
        unsigned long  c1_ant_switch_pa_select_mask : 3;  /* bit[0-2] : 通道1 天线开关和PA控制信号来源。
                                                                        3'b000，受主模控制；
                                                                        3'b001，强制受L模控制；
                                                                        3'b010，强制受W模控制；
                                                                        3'b011，强制受TDS模控制；
                                                                        3'b100，强制受Gm模控制；
                                                                        3'b101，强制受Gs模控制；
                                                                        3'b110，强制受C模控制；
                                                                        3'b111，无效。
                                                                        调试用。 */
        unsigned long  reserved                     : 29; /* bit[3-31]: 保留 */
    } reg;
} BBPCOMM_C1_ANT_SWITCH_PA_SELECT_MASK_UNION;
#define BBPCOMM_C1_ANT_SWITCH_PA_SELECT_MASK_c1_ant_switch_pa_select_mask_START  (0)
#define BBPCOMM_C1_ANT_SWITCH_PA_SELECT_MASK_c1_ant_switch_pa_select_mask_END    (2)


/*****************************************************************************
 结构名    : BBPCOMM_C1_RFIC_SELECT_MASK_UNION
 结构说明  : C1_RFIC_SELECT_MASK 寄存器结构定义。地址偏移量:0x0318，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      c1_rfic_select_mask_reg;
    struct
    {
        unsigned long  c1_rfic_select_mask : 3;  /* bit[0-2] : 通道1 RFIC控制信号来源（包括线控和SSI）。
                                                               3'b000，受主模控制；
                                                               3'b001，强制受L模控制；
                                                               3'b010，强制受W模控制；
                                                               3'b011，强制受TDS模控制；
                                                               3'b100，强制受Gm模控制；
                                                               3'b101，强制受Gs模控制；
                                                               3'b110，强制受C模控制；
                                                               3'b111，无效。
                                                               调试用。 */
        unsigned long  reserved            : 29; /* bit[3-31]: 保留 */
    } reg;
} BBPCOMM_C1_RFIC_SELECT_MASK_UNION;
#define BBPCOMM_C1_RFIC_SELECT_MASK_c1_rfic_select_mask_START  (0)
#define BBPCOMM_C1_RFIC_SELECT_MASK_c1_rfic_select_mask_END    (2)


/*****************************************************************************
 结构名    : BBPCOMM_C1_MIPI_SELECT_MASK_UNION
 结构说明  : C1_MIPI_SELECT_MASK 寄存器结构定义。地址偏移量:0x031C，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      c1_mipi_select_mask_reg;
    struct
    {
        unsigned long  c1_mipi_select_mask : 3;  /* bit[0-2] : 通道1 MIPI控制信号来源。
                                                               3'b000，受主模控制；
                                                               3'b001，强制受L模控制；
                                                               3'b010，强制受W模控制；
                                                               3'b011，强制受TDS模控制；
                                                               3'b100，强制受Gm模控制；
                                                               3'b101，强制受Gs模控制；
                                                               3'b110，强制受C模控制；
                                                               3'b111，无效。
                                                               调试用。 */
        unsigned long  reserved            : 29; /* bit[3-31]: 保留 */
    } reg;
} BBPCOMM_C1_MIPI_SELECT_MASK_UNION;
#define BBPCOMM_C1_MIPI_SELECT_MASK_c1_mipi_select_mask_START  (0)
#define BBPCOMM_C1_MIPI_SELECT_MASK_c1_mipi_select_mask_END    (2)


/*****************************************************************************
 结构名    : BBPCOMM_C1_ABB_SELECT_MASK_UNION
 结构说明  : C1_ABB_SELECT_MASK 寄存器结构定义。地址偏移量:0x0320，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      c1_abb_select_mask_reg;
    struct
    {
        unsigned long  c1_abb_select_mask : 3;  /* bit[0-2] : 通道1 ABB控制信号来源。
                                                              3'b000，受主模控制；
                                                              3'b001，强制受L模控制；
                                                              3'b010，强制受W模控制；
                                                              3'b011，强制受TDS模控制；
                                                              3'b100，强制受Gm模控制；
                                                              3'b101，强制受Gs模控制；
                                                              3'b110，强制受C模控制；
                                                              3'b111，无效。
                                                              调试用。 */
        unsigned long  reserved           : 29; /* bit[3-31]: 保留 */
    } reg;
} BBPCOMM_C1_ABB_SELECT_MASK_UNION;
#define BBPCOMM_C1_ABB_SELECT_MASK_c1_abb_select_mask_START  (0)
#define BBPCOMM_C1_ABB_SELECT_MASK_c1_abb_select_mask_END    (2)


/*****************************************************************************
 结构名    : BBPCOMM_C1_PMU_SELECT_MASK_UNION
 结构说明  : C1_PMU_SELECT_MASK 寄存器结构定义。地址偏移量:0x0324，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      c1_pmu_select_mask_reg;
    struct
    {
        unsigned long  c1_pmu_select_mask : 3;  /* bit[0-2] : 通道1 PMU控制信号来源。
                                                              3'b000，受主模控制；
                                                              3'b001，强制受L模控制；
                                                              3'b010，强制受W模控制；
                                                              3'b011，强制受TDS模控制；
                                                              3'b100，强制受Gm模控制；
                                                              3'b101，强制受Gs模控制；
                                                              3'b110，强制受C模控制；
                                                              3'b111，无效。
                                                              调试用。 */
        unsigned long  reserved           : 29; /* bit[3-31]: 保留 */
    } reg;
} BBPCOMM_C1_PMU_SELECT_MASK_UNION;
#define BBPCOMM_C1_PMU_SELECT_MASK_c1_pmu_select_mask_START  (0)
#define BBPCOMM_C1_PMU_SELECT_MASK_c1_pmu_select_mask_END    (2)


/*****************************************************************************
 结构名    : BBPCOMM_DSP_C1_SEL_MASK_UNION
 结构说明  : DSP_C1_SEL_MASK 寄存器结构定义。地址偏移量:0x0328，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      dsp_c1_sel_mask_reg;
    struct
    {
        unsigned long  dsp_c1_sel_mask : 3;  /* bit[0-2] : 当前通道1（交叉之前） 输出的强配模式。
                                                           注：其适用于各个外围器件（ABB、RF、PA、mipi等）公共的交叉强配。
                                                           调试用。 */
        unsigned long  reserved        : 29; /* bit[3-31]: 保留 */
    } reg;
} BBPCOMM_DSP_C1_SEL_MASK_UNION;
#define BBPCOMM_DSP_C1_SEL_MASK_dsp_c1_sel_mask_START  (0)
#define BBPCOMM_DSP_C1_SEL_MASK_dsp_c1_sel_mask_END    (2)


/*****************************************************************************
 结构名    : BBPCOMM_DSP_RFIC1_RSTN_UNION
 结构说明  : DSP_RFIC1_RSTN 寄存器结构定义。地址偏移量:0x032C，初值:0x00000001，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      dsp_rfic1_rstn_reg;
    struct
    {
        unsigned long  dsp_rfic1_rstn : 1;  /* bit[0-0] : 软件配置的RFIC1复位信号。 低有效。 
                                                          0：复位，1：解复位。
                                                          上电初始化配置为0，根据RFIC手册要求，维持一段时间后配置为1。
                                                          
                                                          V7R2复位值为0。
                                                          V8R1：该寄存器功能删除。 */
        unsigned long  reserved       : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_DSP_RFIC1_RSTN_UNION;
#define BBPCOMM_DSP_RFIC1_RSTN_dsp_rfic1_rstn_START  (0)
#define BBPCOMM_DSP_RFIC1_RSTN_dsp_rfic1_rstn_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_C1_SECOND_AFC_MODE_UNION
 结构说明  : C1_SECOND_AFC_MODE 寄存器结构定义。地址偏移量:0x0330，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      c1_second_afc_mode_reg;
    struct
    {
        unsigned long  c1_second_afc_mode : 1;  /* bit[0]   : 0:通道1从模使用通道1主模afc控制字
                                                              1:通道1从模使用通道1从模afc控制字 */
        unsigned long  reserved           : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_C1_SECOND_AFC_MODE_UNION;
#define BBPCOMM_C1_SECOND_AFC_MODE_c1_second_afc_mode_START  (0)
#define BBPCOMM_C1_SECOND_AFC_MODE_c1_second_afc_mode_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_MIPI1_RD_OVERTIME_FLAG_DSP_UNION
 结构说明  : MIPI1_RD_OVERTIME_FLAG_DSP 寄存器结构定义。地址偏移量:0x0338，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      mipi1_rd_overtime_flag_dsp_reg;
    struct
    {
        unsigned long  mipi1_rd_overtime_flag_dsp : 1;  /* bit[0]   : DSP等待回读结果超时标志位
                                                                      1:超时   */
        unsigned long  reserved                   : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_MIPI1_RD_OVERTIME_FLAG_DSP_UNION;
#define BBPCOMM_MIPI1_RD_OVERTIME_FLAG_DSP_mipi1_rd_overtime_flag_dsp_START  (0)
#define BBPCOMM_MIPI1_RD_OVERTIME_FLAG_DSP_mipi1_rd_overtime_flag_dsp_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_SSI1_RD_OVERTIME_FLAG_DSP_UNION
 结构说明  : SSI1_RD_OVERTIME_FLAG_DSP 寄存器结构定义。地址偏移量:0x033C，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      ssi1_rd_overtime_flag_dsp_reg;
    struct
    {
        unsigned long  ssi1_rd_overtime_flag_dsp : 1;  /* bit[0]   : DSP等待回读结果超时标志位
                                                                     1:超时   */
        unsigned long  reserved                  : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_SSI1_RD_OVERTIME_FLAG_DSP_UNION;
#define BBPCOMM_SSI1_RD_OVERTIME_FLAG_DSP_ssi1_rd_overtime_flag_dsp_START  (0)
#define BBPCOMM_SSI1_RD_OVERTIME_FLAG_DSP_ssi1_rd_overtime_flag_dsp_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_MIPI1_RD_OVERTIME_CLR_UNION
 结构说明  : MIPI1_RD_OVERTIME_CLR 寄存器结构定义。地址偏移量:0x0340，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      mipi1_rd_overtime_clr_reg;
    struct
    {
        unsigned long  mipi1_rd_overtime_clr_imi : 1;  /* bit[0]   : DSP等待回读结果超时后,清抄时标志位脉冲 */
        unsigned long  reserved                  : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_MIPI1_RD_OVERTIME_CLR_UNION;
#define BBPCOMM_MIPI1_RD_OVERTIME_CLR_mipi1_rd_overtime_clr_imi_START  (0)
#define BBPCOMM_MIPI1_RD_OVERTIME_CLR_mipi1_rd_overtime_clr_imi_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_SSI1_RD_OVERTIME_CLR_UNION
 结构说明  : SSI1_RD_OVERTIME_CLR 寄存器结构定义。地址偏移量:0x0344，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      ssi1_rd_overtime_clr_reg;
    struct
    {
        unsigned long  ssi1_rd_overtime_clr_imi : 1;  /* bit[0]   : DSP等待回读结果超时后,清抄时标志位脉冲 */
        unsigned long  reserved                 : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_SSI1_RD_OVERTIME_CLR_UNION;
#define BBPCOMM_SSI1_RD_OVERTIME_CLR_ssi1_rd_overtime_clr_imi_START  (0)
#define BBPCOMM_SSI1_RD_OVERTIME_CLR_ssi1_rd_overtime_clr_imi_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_RD_RESULT_FLAG_RFIC1_UNION
 结构说明  : RD_RESULT_FLAG_RFIC1 寄存器结构定义。地址偏移量:0x0388，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      rd_result_flag_rfic1_reg;
    struct
    {
        unsigned long  rd_result_flag_rfic1 : 1;  /* bit[0]   : 所有回读指令执行完毕,可以取走回读数据
                                                                1:所有回读指令发送完毕,DSP可以按位置读取SSI回读结果 */
        unsigned long  reserved             : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_RD_RESULT_FLAG_RFIC1_UNION;
#define BBPCOMM_RD_RESULT_FLAG_RFIC1_rd_result_flag_rfic1_START  (0)
#define BBPCOMM_RD_RESULT_FLAG_RFIC1_rd_result_flag_rfic1_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_RD_RESULT_FLAG_RFIC1_CLR_UNION
 结构说明  : RD_RESULT_FLAG_RFIC1_CLR 寄存器结构定义。地址偏移量:0x038C，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      rd_result_flag_rfic1_clr_reg;
    struct
    {
        unsigned long  rd_result_flag_rfic1_clr : 1;  /* bit[0]   : 1:DSP取走所有回读数据后,写入clr信号清标志位 */
        unsigned long  reserved                 : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_RD_RESULT_FLAG_RFIC1_CLR_UNION;
#define BBPCOMM_RD_RESULT_FLAG_RFIC1_CLR_rd_result_flag_rfic1_clr_START  (0)
#define BBPCOMM_RD_RESULT_FLAG_RFIC1_CLR_rd_result_flag_rfic1_clr_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_RD_RESULT_FLAG_MIPI1_UNION
 结构说明  : RD_RESULT_FLAG_MIPI1 寄存器结构定义。地址偏移量:0x03D0，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      rd_result_flag_mipi1_reg;
    struct
    {
        unsigned long  rd_result_flag_mipi1 : 1;  /* bit[0]   : 所有回读指令执行完毕,可以取走回读数据
                                                                1:所有回读指令发送完毕,DSP可以按位置读取SSI回读结果,与下述的软件直接控制MIPI标志位不同 */
        unsigned long  reserved             : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_RD_RESULT_FLAG_MIPI1_UNION;
#define BBPCOMM_RD_RESULT_FLAG_MIPI1_rd_result_flag_mipi1_START  (0)
#define BBPCOMM_RD_RESULT_FLAG_MIPI1_rd_result_flag_mipi1_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_RD_RESULT_FLAG_MIPI1_CLR_UNION
 结构说明  : RD_RESULT_FLAG_MIPI1_CLR 寄存器结构定义。地址偏移量:0x03D4，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      rd_result_flag_mipi1_clr_reg;
    struct
    {
        unsigned long  rd_result_flag_mipi1_clr : 1;  /* bit[0]   : 1:DSP取走所有回读数据后,写入clr信号清标志位,与下述的软件直接控制MIPI标志位不同 */
        unsigned long  reserved                 : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_RD_RESULT_FLAG_MIPI1_CLR_UNION;
#define BBPCOMM_RD_RESULT_FLAG_MIPI1_CLR_rd_result_flag_mipi1_clr_START  (0)
#define BBPCOMM_RD_RESULT_FLAG_MIPI1_CLR_rd_result_flag_mipi1_clr_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_RD_END_FLAG_MIPI1_SOFT_UNION
 结构说明  : RD_END_FLAG_MIPI1_SOFT 寄存器结构定义。地址偏移量:0x03E0，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      rd_end_flag_mipi1_soft_reg;
    struct
    {
        unsigned long  rd_end_flag_mipi1_soft : 1;  /* bit[0]   : 由软件直接控制MIPI1 master,不借助xbbp发送回读指令时,回读数据有效指示 */
        unsigned long  reserved               : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_RD_END_FLAG_MIPI1_SOFT_UNION;
#define BBPCOMM_RD_END_FLAG_MIPI1_SOFT_rd_end_flag_mipi1_soft_START  (0)
#define BBPCOMM_RD_END_FLAG_MIPI1_SOFT_rd_end_flag_mipi1_soft_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_MIPI1_GRANT_DSP_UNION
 结构说明  : MIPI1_GRANT_DSP 寄存器结构定义。地址偏移量:0x03E4，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      mipi1_grant_dsp_reg;
    struct
    {
        unsigned long  mipi1_grant_dsp : 1;  /* bit[0]   : 由软件直接控制MIPI1 master
                                                           1:软件可以发送CFG_IND启动mipi1
                                                           0:软件等待mipi1授权使用 */
        unsigned long  reserved        : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_MIPI1_GRANT_DSP_UNION;
#define BBPCOMM_MIPI1_GRANT_DSP_mipi1_grant_dsp_START  (0)
#define BBPCOMM_MIPI1_GRANT_DSP_mipi1_grant_dsp_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_DSP_MIPI1_EN_IMI_UNION
 结构说明  : DSP_MIPI1_EN_IMI 寄存器结构定义。地址偏移量:0x03F0，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      dsp_mipi1_en_imi_reg;
    struct
    {
        unsigned long  dsp_mipi1_en_imi : 1;  /* bit[0]   : 由软件直接控制MIPI1 master,需首先拉高此指示,屏蔽xbbp对MIPI1的控制 */
        unsigned long  reserved         : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_DSP_MIPI1_EN_IMI_UNION;
#define BBPCOMM_DSP_MIPI1_EN_IMI_dsp_mipi1_en_imi_START  (0)
#define BBPCOMM_DSP_MIPI1_EN_IMI_dsp_mipi1_en_imi_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_DSP_MIPI1_CFG_IND_IMI_UNION
 结构说明  : DSP_MIPI1_CFG_IND_IMI 寄存器结构定义。地址偏移量:0x03F4，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      dsp_mipi1_cfg_ind_imi_reg;
    struct
    {
        unsigned long  dsp_mipi1_cfg_ind_imi : 1;  /* bit[0]   : 由软件直接控制MIPI1 master,拉高此dsp_mipi1_en后,产生软件启动脉冲 */
        unsigned long  reserved              : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_DSP_MIPI1_CFG_IND_IMI_UNION;
#define BBPCOMM_DSP_MIPI1_CFG_IND_IMI_dsp_mipi1_cfg_ind_imi_START  (0)
#define BBPCOMM_DSP_MIPI1_CFG_IND_IMI_dsp_mipi1_cfg_ind_imi_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_DSP_MIPI1_RD_CLR_UNION
 结构说明  : DSP_MIPI1_RD_CLR 寄存器结构定义。地址偏移量:0x03F8，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      dsp_mipi1_rd_clr_reg;
    struct
    {
        unsigned long  dsp_mipi1_rd_clr : 1;  /* bit[0]   : 由软件直接控制MIPI1 master,取走回读数据后,清标志位脉冲 */
        unsigned long  reserved         : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_DSP_MIPI1_RD_CLR_UNION;
#define BBPCOMM_DSP_MIPI1_RD_CLR_dsp_mipi1_rd_clr_START  (0)
#define BBPCOMM_DSP_MIPI1_RD_CLR_dsp_mipi1_rd_clr_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_ABB1_LINE_CONTROL_CMD_UNION
 结构说明  : ABB1_LINE_CONTROL_CMD 寄存器结构定义。地址偏移量:0x03FC，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      abb1_line_control_cmd_reg;
    struct
    {
        unsigned long  abb1_line_control_cmd_sel : 1;  /* bit[0-0] : ABB1线控强配模式：
                                                                     1：表示软件强配，此时上述各个强配才生效；
                                                                     0：表示由CTU根据各BBP产生。
                                                                     默认值为0。
                                                                     测试用。 */
        unsigned long  abb1_mode_sel_cmd         : 3;  /* bit[1-3] : 软件强配模式abb1_mode_sel. */
        unsigned long  abb1_tx_en_cmd            : 1;  /* bit[4-4] : 软件强配模式abb1_tx_en. */
        unsigned long  abb1_rxa_en_cmd           : 1;  /* bit[5-5] : 软件强配模式abb1_rxa_en. */
        unsigned long  abb1_rxb_en_cmd           : 1;  /* bit[6-6] : 软件强配模式abb1_rxb_en. */
        unsigned long  abb1_blka_en_cmd          : 1;  /* bit[7-7] : 软件强配模式abb1_rxa_blk_en. */
        unsigned long  abb1_blkb_en_cmd          : 1;  /* bit[8-8] : 软件强配模式abb1_rxb_blk_en. */
        unsigned long  reserved                  : 23; /* bit[9-31]: 保留 */
    } reg;
} BBPCOMM_ABB1_LINE_CONTROL_CMD_UNION;
#define BBPCOMM_ABB1_LINE_CONTROL_CMD_abb1_line_control_cmd_sel_START  (0)
#define BBPCOMM_ABB1_LINE_CONTROL_CMD_abb1_line_control_cmd_sel_END    (0)
#define BBPCOMM_ABB1_LINE_CONTROL_CMD_abb1_mode_sel_cmd_START          (1)
#define BBPCOMM_ABB1_LINE_CONTROL_CMD_abb1_mode_sel_cmd_END            (3)
#define BBPCOMM_ABB1_LINE_CONTROL_CMD_abb1_tx_en_cmd_START             (4)
#define BBPCOMM_ABB1_LINE_CONTROL_CMD_abb1_tx_en_cmd_END               (4)
#define BBPCOMM_ABB1_LINE_CONTROL_CMD_abb1_rxa_en_cmd_START            (5)
#define BBPCOMM_ABB1_LINE_CONTROL_CMD_abb1_rxa_en_cmd_END              (5)
#define BBPCOMM_ABB1_LINE_CONTROL_CMD_abb1_rxb_en_cmd_START            (6)
#define BBPCOMM_ABB1_LINE_CONTROL_CMD_abb1_rxb_en_cmd_END              (6)
#define BBPCOMM_ABB1_LINE_CONTROL_CMD_abb1_blka_en_cmd_START           (7)
#define BBPCOMM_ABB1_LINE_CONTROL_CMD_abb1_blka_en_cmd_END             (7)
#define BBPCOMM_ABB1_LINE_CONTROL_CMD_abb1_blkb_en_cmd_START           (8)
#define BBPCOMM_ABB1_LINE_CONTROL_CMD_abb1_blkb_en_cmd_END             (8)


/*****************************************************************************
 结构名    : BBPCOMM_CH1_BBP_SEL_UNION
 结构说明  : CH1_BBP_SEL 寄存器结构定义。地址偏移量:0x0400，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      ch1_bbp_sel_reg;
    struct
    {
        unsigned long  ch1_bbp_sel : 3;  /* bit[0-2] : V9R1版本，软件不配置。 */
        unsigned long  reserved    : 29; /* bit[3-31]: 保留 */
    } reg;
} BBPCOMM_CH1_BBP_SEL_UNION;
#define BBPCOMM_CH1_BBP_SEL_ch1_bbp_sel_START  (0)
#define BBPCOMM_CH1_BBP_SEL_ch1_bbp_sel_END    (2)


/*****************************************************************************
 结构名    : BBPCOMM_CPU_MIPI1_FUNC_SEL_UNION
 结构说明  : CPU_MIPI1_FUNC_SEL 寄存器结构定义。地址偏移量:0x0404，初值:0x00000001，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      cpu_mipi1_func_sel_reg;
    struct
    {
        unsigned long  cpu_mipi1_func_sel : 1;  /* bit[0]   : cpu配置mipi功能,配置为1才能正常工作, 测试时配置为0 */
        unsigned long  reserved           : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_CPU_MIPI1_FUNC_SEL_UNION;
#define BBPCOMM_CPU_MIPI1_FUNC_SEL_cpu_mipi1_func_sel_START  (0)
#define BBPCOMM_CPU_MIPI1_FUNC_SEL_cpu_mipi1_func_sel_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_CPU_MIPI1_TEST_FUNC_UNION
 结构说明  : CPU_MIPI1_TEST_FUNC 寄存器结构定义。地址偏移量:0x0408，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      cpu_mipi1_test_func_reg;
    struct
    {
        unsigned long  cpu_mipi1_test_func : 1;  /* bit[0]   : cpu配置mipi功能,测试用 */
        unsigned long  reserved            : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_CPU_MIPI1_TEST_FUNC_UNION;
#define BBPCOMM_CPU_MIPI1_TEST_FUNC_cpu_mipi1_test_func_START  (0)
#define BBPCOMM_CPU_MIPI1_TEST_FUNC_cpu_mipi1_test_func_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_CPU_MIPI1_SCLK_TEST_UNION
 结构说明  : CPU_MIPI1_SCLK_TEST 寄存器结构定义。地址偏移量:0x040C，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      cpu_mipi1_sclk_test_reg;
    struct
    {
        unsigned long  cpu_mipi1_sclk_test : 1;  /* bit[0]   : cpu配置mipi功能,测试用 */
        unsigned long  reserved            : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_CPU_MIPI1_SCLK_TEST_UNION;
#define BBPCOMM_CPU_MIPI1_SCLK_TEST_cpu_mipi1_sclk_test_START  (0)
#define BBPCOMM_CPU_MIPI1_SCLK_TEST_cpu_mipi1_sclk_test_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_CPU_MIPI1_SDATA_TEST_UNION
 结构说明  : CPU_MIPI1_SDATA_TEST 寄存器结构定义。地址偏移量:0x0410，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      cpu_mipi1_sdata_test_reg;
    struct
    {
        unsigned long  cpu_mipi1_sdata_test : 1;  /* bit[0]   : cpu配置mipi功能,测试用 */
        unsigned long  reserved             : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_CPU_MIPI1_SDATA_TEST_UNION;
#define BBPCOMM_CPU_MIPI1_SDATA_TEST_cpu_mipi1_sdata_test_START  (0)
#define BBPCOMM_CPU_MIPI1_SDATA_TEST_cpu_mipi1_sdata_test_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_DSP_MIPI1_CLR_IND_IMI_UNION
 结构说明  : DSP_MIPI1_CLR_IND_IMI 寄存器结构定义。地址偏移量:0x0414，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      dsp_mipi1_clr_ind_imi_reg;
    struct
    {
        unsigned long  dsp_mipi1_clr_ind_imi : 1;  /* bit[0]   : cpu配置mipi功能,mipi1master清异常信号 */
        unsigned long  reserved              : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_DSP_MIPI1_CLR_IND_IMI_UNION;
#define BBPCOMM_DSP_MIPI1_CLR_IND_IMI_dsp_mipi1_clr_ind_imi_START  (0)
#define BBPCOMM_DSP_MIPI1_CLR_IND_IMI_dsp_mipi1_clr_ind_imi_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_CPU_MIPI1_FIFO_CLR_IMI_UNION
 结构说明  : CPU_MIPI1_FIFO_CLR_IMI 寄存器结构定义。地址偏移量:0x0418，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      cpu_mipi1_fifo_clr_imi_reg;
    struct
    {
        unsigned long  cpu_mipi1_fifo_clr_imi : 1;  /* bit[0]   : mipi1存储待发送数据同步FIFO复位信号 */
        unsigned long  reserved               : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_CPU_MIPI1_FIFO_CLR_IMI_UNION;
#define BBPCOMM_CPU_MIPI1_FIFO_CLR_IMI_cpu_mipi1_fifo_clr_imi_START  (0)
#define BBPCOMM_CPU_MIPI1_FIFO_CLR_IMI_cpu_mipi1_fifo_clr_imi_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_CPU_SSI1_FIFO_CLR_IMI_UNION
 结构说明  : CPU_SSI1_FIFO_CLR_IMI 寄存器结构定义。地址偏移量:0x041C，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      cpu_ssi1_fifo_clr_imi_reg;
    struct
    {
        unsigned long  cpu_ssi1_fifo_clr_imi : 1;  /* bit[0]   : ssi1存储待发送数据同步FIFO复位信号 */
        unsigned long  reserved              : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_CPU_SSI1_FIFO_CLR_IMI_UNION;
#define BBPCOMM_CPU_SSI1_FIFO_CLR_IMI_cpu_ssi1_fifo_clr_imi_START  (0)
#define BBPCOMM_CPU_SSI1_FIFO_CLR_IMI_cpu_ssi1_fifo_clr_imi_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_CPU_RX1_TX1_CKG_BYPASS_UNION
 结构说明  : CPU_RX1_TX1_CKG_BYPASS 寄存器结构定义。地址偏移量:0x0420，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      cpu_rx1_tx1_ckg_bypass_reg;
    struct
    {
        unsigned long  cpu_rx1_lte_ckg_bypass : 1;  /* bit[0]    : L模通道1 RX ABB随路时钟门控旁路信号 */
        unsigned long  cpu_rx1_tds_ckg_bypass : 1;  /* bit[1]    : TDS通道1 RX ABB随路时钟门控旁路信号 */
        unsigned long  cpu_rx1_w_ckg_bypass   : 1;  /* bit[2]    : W模通道1 RX ABB随路时钟门控旁路信号 */
        unsigned long  cpu_rx1_g1_ckg_bypass  : 1;  /* bit[3]    : G1通道1 RX ABB随路时钟门控旁路信号 */
        unsigned long  cpu_rx1_g2_ckg_bypass  : 1;  /* bit[4]    : G2通道1 RX ABB随路时钟门控旁路信号 */
        unsigned long  cpu_tx1_lte_ckg_bypass : 1;  /* bit[5]    : L模通道1 TX ABB随路时钟门控旁路信号 */
        unsigned long  cpu_tx1_tds_ckg_bypass : 1;  /* bit[6]    : TDS通道1 TX ABB随路时钟门控旁路信号 */
        unsigned long  cpu_tx1_w_ckg_bypass   : 1;  /* bit[7]    : W模通道1 TX ABB随路时钟门控旁路信号 */
        unsigned long  cpu_tx1_g1_ckg_bypass  : 1;  /* bit[8]    : G1通道1 TX ABB随路时钟门控旁路信号 */
        unsigned long  cpu_tx1_g2_ckg_bypass  : 1;  /* bit[9]    : G2通道1 TX ABB随路时钟门控旁路信号 */
        unsigned long  cpu_rx1_c_ckg_bypass   : 1;  /* bit[10]   : C模通道1 RX ABB随路时钟门控旁路信号 */
        unsigned long  cpu_tx1_c_ckg_bypass   : 1;  /* bit[11]   : C模通道1 TX ABB随路时钟门控旁路信号 */
        unsigned long  reserved               : 20; /* bit[12-31]: 保留 */
    } reg;
} BBPCOMM_CPU_RX1_TX1_CKG_BYPASS_UNION;
#define BBPCOMM_CPU_RX1_TX1_CKG_BYPASS_cpu_rx1_lte_ckg_bypass_START  (0)
#define BBPCOMM_CPU_RX1_TX1_CKG_BYPASS_cpu_rx1_lte_ckg_bypass_END    (0)
#define BBPCOMM_CPU_RX1_TX1_CKG_BYPASS_cpu_rx1_tds_ckg_bypass_START  (1)
#define BBPCOMM_CPU_RX1_TX1_CKG_BYPASS_cpu_rx1_tds_ckg_bypass_END    (1)
#define BBPCOMM_CPU_RX1_TX1_CKG_BYPASS_cpu_rx1_w_ckg_bypass_START    (2)
#define BBPCOMM_CPU_RX1_TX1_CKG_BYPASS_cpu_rx1_w_ckg_bypass_END      (2)
#define BBPCOMM_CPU_RX1_TX1_CKG_BYPASS_cpu_rx1_g1_ckg_bypass_START   (3)
#define BBPCOMM_CPU_RX1_TX1_CKG_BYPASS_cpu_rx1_g1_ckg_bypass_END     (3)
#define BBPCOMM_CPU_RX1_TX1_CKG_BYPASS_cpu_rx1_g2_ckg_bypass_START   (4)
#define BBPCOMM_CPU_RX1_TX1_CKG_BYPASS_cpu_rx1_g2_ckg_bypass_END     (4)
#define BBPCOMM_CPU_RX1_TX1_CKG_BYPASS_cpu_tx1_lte_ckg_bypass_START  (5)
#define BBPCOMM_CPU_RX1_TX1_CKG_BYPASS_cpu_tx1_lte_ckg_bypass_END    (5)
#define BBPCOMM_CPU_RX1_TX1_CKG_BYPASS_cpu_tx1_tds_ckg_bypass_START  (6)
#define BBPCOMM_CPU_RX1_TX1_CKG_BYPASS_cpu_tx1_tds_ckg_bypass_END    (6)
#define BBPCOMM_CPU_RX1_TX1_CKG_BYPASS_cpu_tx1_w_ckg_bypass_START    (7)
#define BBPCOMM_CPU_RX1_TX1_CKG_BYPASS_cpu_tx1_w_ckg_bypass_END      (7)
#define BBPCOMM_CPU_RX1_TX1_CKG_BYPASS_cpu_tx1_g1_ckg_bypass_START   (8)
#define BBPCOMM_CPU_RX1_TX1_CKG_BYPASS_cpu_tx1_g1_ckg_bypass_END     (8)
#define BBPCOMM_CPU_RX1_TX1_CKG_BYPASS_cpu_tx1_g2_ckg_bypass_START   (9)
#define BBPCOMM_CPU_RX1_TX1_CKG_BYPASS_cpu_tx1_g2_ckg_bypass_END     (9)
#define BBPCOMM_CPU_RX1_TX1_CKG_BYPASS_cpu_rx1_c_ckg_bypass_START    (10)
#define BBPCOMM_CPU_RX1_TX1_CKG_BYPASS_cpu_rx1_c_ckg_bypass_END      (10)
#define BBPCOMM_CPU_RX1_TX1_CKG_BYPASS_cpu_tx1_c_ckg_bypass_START    (11)
#define BBPCOMM_CPU_RX1_TX1_CKG_BYPASS_cpu_tx1_c_ckg_bypass_END      (11)


/*****************************************************************************
 结构名    : BBPCOMM_TIMING_GET_IND_UNION
 结构说明  : TIMING_GET_IND 寄存器结构定义。地址偏移量:0x0610，初值:0x00000030，宽度:32
 寄存器说明: 定时锁存使能指示寄存器
*****************************************************************************/
typedef union
{
    unsigned long      timing_get_ind_reg;
    struct
    {
        unsigned long  timing_get_ind   : 1;  /* bit[0]   : 逻辑自清零。
                                                            timing_get_mode=0，DSP需要获取定时关系时配置生效，CTU逻辑根据此信号锁存LTE/W/Gm/Gs/TDS/C的系统定时，CTU的内部定时供DSP查询。
                                                            timing_get_mode=1，bbe16需要获取定时关系时配置生效，CTU逻辑根据此信号锁存LTE/W/Gm/Gs/TDS/C的系统定时，CTU的内部定时供bbe16查询。
                                                            配置时刻：软件需要锁存定时
                                                            生效时刻：立即生效 */
        unsigned long  reserved_0       : 3;  /* bit[1-3] : 保留 */
        unsigned long  timing_get_c_sel : 2;  /* bit[4-5] : 表示timing_get_mode=1时bbe读取C模bbp计数器结果1x、evdo还是base_cnt。   00：1x定时，01：evdo定时，10： base_cnt ，11：无效。
                                                            timing_get_mode=0时，不需配置，无意义 */
        unsigned long  reserved_1       : 26; /* bit[6-31]: 保留 */
    } reg;
} BBPCOMM_TIMING_GET_IND_UNION;
#define BBPCOMM_TIMING_GET_IND_timing_get_ind_START    (0)
#define BBPCOMM_TIMING_GET_IND_timing_get_ind_END      (0)
#define BBPCOMM_TIMING_GET_IND_timing_get_c_sel_START  (4)
#define BBPCOMM_TIMING_GET_IND_timing_get_c_sel_END    (5)


/*****************************************************************************
 结构名    : BBPCOMM_TIMING_CLEAR_UNION
 结构说明  : TIMING_CLEAR 寄存器结构定义。地址偏移量:0x0614，初值:0x00000000，宽度:32
 寄存器说明: 定时标志清除寄存器
*****************************************************************************/
typedef union
{
    unsigned long      timing_clear_reg;
    struct
    {
        unsigned long  timing_clear          : 1;  /* bit[0]   : 逻辑自清零。
                                                                 定时标志清除，用于清除timing_valid标志。
                                                                 配置时刻：DSP读取定时完毕后配置
                                                                 生效时刻：立即生效
                                                                 （timing_get_mode=0，1套查询接口；
                                                                 timing_get_mode=1，2套查询接口） */
        unsigned long  comm_32k_timing_clear : 1;  /* bit[1]   : 逻辑自清零。
                                                                 32公共定时信息的标志清除，用于清除timing_valid标志。
                                                                 配置时刻：DSP读取定时完毕后配置
                                                                 生效时刻：立即生效
                                                                 （因为32K定时信息的上报延时可能远大于其他系统的定时信息，所以单独处理）
                                                                 （timing_get_mode=0，1套查询接口；
                                                                 timing_get_mode=1，2套查询接口） */
        unsigned long  reserved              : 30; /* bit[2-31]: 保留 */
    } reg;
} BBPCOMM_TIMING_CLEAR_UNION;
#define BBPCOMM_TIMING_CLEAR_timing_clear_START           (0)
#define BBPCOMM_TIMING_CLEAR_timing_clear_END             (0)
#define BBPCOMM_TIMING_CLEAR_comm_32k_timing_clear_START  (1)
#define BBPCOMM_TIMING_CLEAR_comm_32k_timing_clear_END    (1)


/*****************************************************************************
 结构名    : BBPCOMM_DELAY_CNT_UNION
 结构说明  : DELAY_CNT 寄存器结构定义。地址偏移量:0x0618，初值:0x0000000A，宽度:32
 寄存器说明: 定时锁存延迟寄存器
*****************************************************************************/
typedef union
{
    unsigned long      delay_cnt_reg;
    struct
    {
        unsigned long  delay_cnt : 5;  /* bit[0-4] : CTU检测到timing_get_ind后固定延迟delay_cnt个CTU时钟周期后拉高timing_valid信号。
                                                     配置时刻：上电初始化
                                                     生效时刻：立即生效 */
        unsigned long  reserved  : 27; /* bit[5-31]: 保留 */
    } reg;
} BBPCOMM_DELAY_CNT_UNION;
#define BBPCOMM_DELAY_CNT_delay_cnt_START  (0)
#define BBPCOMM_DELAY_CNT_delay_cnt_END    (4)


/*****************************************************************************
 结构名    : BBPCOMM_INT_WAIT_CNT_UNION
 结构说明  : INT_WAIT_CNT 寄存器结构定义。地址偏移量:0x061C，初值:0x0000001E，宽度:32
 寄存器说明: 中断延迟寄存器
*****************************************************************************/
typedef union
{
    unsigned long      int_wait_cnt_reg;
    struct
    {
        unsigned long  int_wait_cnt : 16; /* bit[0-15] : CTU中断上报等待时间，单位为一个CLK周期。
                                                         DSP清除中断标志寄存器的某种类型bit后，CTU上报中断电平需要拉低，如果此时中断标志寄存器还有其它中断类型有效，则需等待int_wait_cnt个时钟周期后再拉高CTU上报中断。
                                                         配置时刻：上电初始化
                                                         生效时刻：立即生效 */
        unsigned long  reserved     : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_INT_WAIT_CNT_UNION;
#define BBPCOMM_INT_WAIT_CNT_int_wait_cnt_START  (0)
#define BBPCOMM_INT_WAIT_CNT_int_wait_cnt_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_LTEBBP_INPUT_MASK_UNION
 结构说明  : LTEBBP_INPUT_MASK 寄存器结构定义。地址偏移量:0x0620，初值:0x00000003，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      ltebbp_input_mask_reg;
    struct
    {
        unsigned long  ltebbp_input_mask : 2;  /* bit[0-1] : 电平信号，低有效。
                                                             当LTE主模需要做异系统测量时需配置本寄存器为3，异系统测量结束后配置本寄存器为0。
                                                             bit1:1’b0: lte_intrasys_valid输入无效，被强制为1。
                                                             bit0:1’b0:
                                                             lte_intersys_ingap_pre_ind输入无效，被强制为0。
                                                             bit[1:0] = 11b: 让lte_intrasys_valid和lte_intersys_ingap_pre_ind输入有效。
                                                             
                                                             配置时刻：LTE主模需要做异系统测量时需配置本寄存器为3，异系统测量结束后配置本寄存器为0。
                                                             生效时刻：立即生效 */
        unsigned long  reserved          : 30; /* bit[2-31]: 保留 */
    } reg;
} BBPCOMM_LTEBBP_INPUT_MASK_UNION;
#define BBPCOMM_LTEBBP_INPUT_MASK_ltebbp_input_mask_START  (0)
#define BBPCOMM_LTEBBP_INPUT_MASK_ltebbp_input_mask_END    (1)


/*****************************************************************************
 结构名    : BBPCOMM_WBBP_INPUT_MASK_UNION
 结构说明  : WBBP_INPUT_MASK 寄存器结构定义。地址偏移量:0x0624，初值:0x00000003，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      wbbp_input_mask_reg;
    struct
    {
        unsigned long  wbbp_input_mask : 2;  /* bit[0-1] : 电平信号，低有效。
                                                           当W主模需要做异系统测量时需配置本寄存器为3，异系统测量结束后配置本寄存器为0。
                                                           bit1:1’b0:
                                                            让w_intrasys_valid输入无效，被强制为1。
                                                           bit0:1’b0:
                                                           w_intersys_ingap_pre_ind输入无效，被强制为0。
                                                           bit[1:0] = 11b: 让w_intrasys_valid和w_intersys_ingap_pre_ind输入有效。
                                                           
                                                           配置时刻：W主模需要做异系统测量时需配置本寄存器为3，异系统测量结束后配置本寄存器为0。
                                                           生效时刻：立即生效 */
        unsigned long  reserved        : 30; /* bit[2-31]: 保留 */
    } reg;
} BBPCOMM_WBBP_INPUT_MASK_UNION;
#define BBPCOMM_WBBP_INPUT_MASK_wbbp_input_mask_START  (0)
#define BBPCOMM_WBBP_INPUT_MASK_wbbp_input_mask_END    (1)


/*****************************************************************************
 结构名    : BBPCOMM_GMBBP_INPUT_MASK_UNION
 结构说明  : GMBBP_INPUT_MASK 寄存器结构定义。地址偏移量:0x0628，初值:0x00000003，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      gmbbp_input_mask_reg;
    struct
    {
        unsigned long  gmbbp_input_mask : 2;  /* bit[0-1] : 电平信号，低有效。
                                                            当Gm主模需要做异系统测量时需配置本寄存器为3，异系统测量结束后配置本寄存器为0。
                                                            bit1:1’b0:
                                                             让gm_intrasys_valid输入无效，被强制为1。
                                                            bit0:1’b0:
                                                            gm_intersys_ingap_pre_ind输入无效，被强制为0。
                                                            bit[1:0] = 11b: 让gm_intrasys_valid和gm_intersys_ingap_pre_ind输入有效。
                                                            
                                                            配置时刻：Gm主模需要做异系统测量时需配置本寄存器为3，异系统测量结束后配置本寄存器为0。
                                                            生效时刻：立即生效 */
        unsigned long  reserved         : 30; /* bit[2-31]: 保留 */
    } reg;
} BBPCOMM_GMBBP_INPUT_MASK_UNION;
#define BBPCOMM_GMBBP_INPUT_MASK_gmbbp_input_mask_START  (0)
#define BBPCOMM_GMBBP_INPUT_MASK_gmbbp_input_mask_END    (1)


/*****************************************************************************
 结构名    : BBPCOMM_GSBBP_INPUT_MASK_UNION
 结构说明  : GSBBP_INPUT_MASK 寄存器结构定义。地址偏移量:0x062C，初值:0x00000003，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      gsbbp_input_mask_reg;
    struct
    {
        unsigned long  gsbbp_input_mask : 2;  /* bit[0-1] : 电平信号，低有效。
                                                            当Gs主模需要做异系统测量时需配置本寄存器为3，异系统测量结束后配置本寄存器为0。
                                                            bit1:1’b0:
                                                             让gs_intrasys_valid输入无效，被强制为1。
                                                            bit0:1’b0:
                                                            gs_intersys_ingap_pre_ind输入无效，被强制为0。
                                                            bit[1:0] = 11b: 让gs_intrasys_valid和gs_intersys_ingap_pre_ind输入有效。
                                                            
                                                            配置时刻：Gs主模需要做异系统测量时需配置本寄存器为3，异系统测量结束后配置本寄存器为0。
                                                            生效时刻：立即生效 */
        unsigned long  reserved         : 30; /* bit[2-31]: 保留 */
    } reg;
} BBPCOMM_GSBBP_INPUT_MASK_UNION;
#define BBPCOMM_GSBBP_INPUT_MASK_gsbbp_input_mask_START  (0)
#define BBPCOMM_GSBBP_INPUT_MASK_gsbbp_input_mask_END    (1)


/*****************************************************************************
 结构名    : BBPCOMM_TDSBBP_INPUT_MASK_UNION
 结构说明  : TDSBBP_INPUT_MASK 寄存器结构定义。地址偏移量:0x0630，初值:0x00000003，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      tdsbbp_input_mask_reg;
    struct
    {
        unsigned long  tdsbbp_input_mask : 2;  /* bit[0-1] : 电平信号，低有效。
                                                             当TDS主模需要做异系统测量时需配置本寄存器为3，异系统测量结束后配置本寄存器为0。
                                                             bit1:1’b0:
                                                              让tds_intrasys_valid输入无效，被强制为1。
                                                             bit0:1’b0:
                                                             tds_intersys_ingap_pre_ind输入无效，被强制为0。
                                                             bit[1:0] = 11b: 让tds_intrasys_valid和tds_intersys_ingap_pre_ind输入有效。
                                                             
                                                             配置时刻：TDS主模需要做异系统测量时需配置本寄存器为3，异系统测量结束后配置本寄存器为0。
                                                             生效时刻：立即生效 */
        unsigned long  reserved          : 30; /* bit[2-31]: 保留 */
    } reg;
} BBPCOMM_TDSBBP_INPUT_MASK_UNION;
#define BBPCOMM_TDSBBP_INPUT_MASK_tdsbbp_input_mask_START  (0)
#define BBPCOMM_TDSBBP_INPUT_MASK_tdsbbp_input_mask_END    (1)


/*****************************************************************************
 结构名    : BBPCOMM_TIMING_VALID_UNION
 结构说明  : TIMING_VALID 寄存器结构定义。地址偏移量:0x0668，初值:0x00000030，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      timing_valid_reg;
    struct
    {
        unsigned long  o_timing_valid        : 1;  /* bit[0]   : 定时上报有效标志，高有效。
                                                                 DSP配置timing_get_ind后，查询此标志，如果为高，则可以获取LTE，W，Gm，Gs, TDS的系统定时。
                                                                 DSP读取定时完毕后配置timing_clear寄存器，timing_valid拉低为无效。
                                                                 CTU检测到timing_get_ind后固定延迟delay_cnt后拉高timing_valid。
                                                                 
                                                                 配置时刻：
                                                                 生效时刻：立即生效
                                                                 （timing_get_mode=0，dsp控制，
                                                                 timing_get_mode=1，bbe16控制） */
        unsigned long  comm_32k_timing_valid : 1;  /* bit[1]   : 32K公共定时上报有效标志，高有效。
                                                                 DSP配置timing_get_ind后，查询此标志，如果为高，则可以获取32K公共定时器的定时信息。
                                                                 DSP读取定时完毕后配置comm_32k_timing_clear寄存器，comm_32k_timing_valid拉低为无效。
                                                                 32K定时锁存稳定后拉高comm_32k_timing_valid
                                                                 配置时刻：
                                                                 生效时刻：立即生效
                                                                 （timing_get_mode=0，1套查询接口；
                                                                 timing_get_mode=1，2套查询接口） */
        unsigned long  overtime_ind          : 1;  /* bit[2]   : 表示bbe16读取定时信息超时（在超时保护时间内未给出o_timing_clear信号） */
        unsigned long  reserved_0            : 1;  /* bit[3]   : 保留 */
        unsigned long  timing_valid_c_sel    : 2;  /* bit[4-5] : 表示o_timing_valid有效时，上报c模的计数器给bbe16的是1x、evdo还是base_cnt
                                                                 00：1x定时
                                                                 01： evdo定时
                                                                 10： basecent */
        unsigned long  reserved_1            : 26; /* bit[6-31]: 保留 */
    } reg;
} BBPCOMM_TIMING_VALID_UNION;
#define BBPCOMM_TIMING_VALID_o_timing_valid_START         (0)
#define BBPCOMM_TIMING_VALID_o_timing_valid_END           (0)
#define BBPCOMM_TIMING_VALID_comm_32k_timing_valid_START  (1)
#define BBPCOMM_TIMING_VALID_comm_32k_timing_valid_END    (1)
#define BBPCOMM_TIMING_VALID_overtime_ind_START           (2)
#define BBPCOMM_TIMING_VALID_overtime_ind_END             (2)
#define BBPCOMM_TIMING_VALID_timing_valid_c_sel_START     (4)
#define BBPCOMM_TIMING_VALID_timing_valid_c_sel_END       (5)


/*****************************************************************************
 结构名    : BBPCOMM_W_TIMING_DSP_UNION
 结构说明  : W_TIMING_DSP 寄存器结构定义。地址偏移量:0x066C，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      w_timing_dsp_reg;
    struct
    {
        unsigned long  w_clk_cnt_dsp  : 4;  /* bit[0-3]  : [3:0]为chip内时钟计数，范围0~15 */
        unsigned long  w_chip_cnt_dsp : 12; /* bit[4-15] : [15:4]为时隙内chip计数，范围0~2559 */
        unsigned long  w_slot_cnt_dsp : 4;  /* bit[16-19]: [19:16]为时隙计数，范围0~14 */
        unsigned long  w_sfn_dsp      : 12; /* bit[20-31]: W的系统定时信息，由DSP 配置timing_get_ind触发上报。
                                                           上报时刻：timing_valid为高时上报有效.
                                                           [31:20]表示W的SFN计数（源自w_timing_b[11:0]），范围0~4095。 */
    } reg;
} BBPCOMM_W_TIMING_DSP_UNION;
#define BBPCOMM_W_TIMING_DSP_w_clk_cnt_dsp_START   (0)
#define BBPCOMM_W_TIMING_DSP_w_clk_cnt_dsp_END     (3)
#define BBPCOMM_W_TIMING_DSP_w_chip_cnt_dsp_START  (4)
#define BBPCOMM_W_TIMING_DSP_w_chip_cnt_dsp_END    (15)
#define BBPCOMM_W_TIMING_DSP_w_slot_cnt_dsp_START  (16)
#define BBPCOMM_W_TIMING_DSP_w_slot_cnt_dsp_END    (19)
#define BBPCOMM_W_TIMING_DSP_w_sfn_dsp_START       (20)
#define BBPCOMM_W_TIMING_DSP_w_sfn_dsp_END         (31)


/*****************************************************************************
 结构名    : BBPCOMM_GM_FN_DSP_UNION
 结构说明  : GM_FN_DSP 寄存器结构定义。地址偏移量:0x0670，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      gm_fn_dsp_reg;
    struct
    {
        unsigned long  o_gm_fn_dsp : 22; /* bit[0-21] : GM的FN计数（源自gm_timing_a[21:0]），由DSP 配置timing_get_ind触发上报。其中[10:0]表示fn_low,范围0~1325；[21:11]表示fn_hign,范围0~2047。
                                                        上报时刻：timing_valid为高时上报有效 */
        unsigned long  reserved    : 10; /* bit[22-31]: 保留 */
    } reg;
} BBPCOMM_GM_FN_DSP_UNION;
#define BBPCOMM_GM_FN_DSP_o_gm_fn_dsp_START  (0)
#define BBPCOMM_GM_FN_DSP_o_gm_fn_dsp_END    (21)


/*****************************************************************************
 结构名    : BBPCOMM_GM_QB_DSP_UNION
 结构说明  : GM_QB_DSP 寄存器结构定义。地址偏移量:0x0674，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      gm_qb_dsp_reg;
    struct
    {
        unsigned long  o_gm_qb_dsp : 13; /* bit[0-12] : GM的系统定时Qb信息，由DSP 配置timing_get_ind触发上报。
                                                         [12:0]表示 G系统定时帧内Qb计数（源自gm_timing_b[12:0]），范围0~4999。
                                                        上报时刻：timing_valid为高时上报有效 */
        unsigned long  reserved    : 19; /* bit[13-31]: 保留 */
    } reg;
} BBPCOMM_GM_QB_DSP_UNION;
#define BBPCOMM_GM_QB_DSP_o_gm_qb_dsp_START  (0)
#define BBPCOMM_GM_QB_DSP_o_gm_qb_dsp_END    (12)


/*****************************************************************************
 结构名    : BBPCOMM_GS_FN_DSP_UNION
 结构说明  : GS_FN_DSP 寄存器结构定义。地址偏移量:0x0678，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      gs_fn_dsp_reg;
    struct
    {
        unsigned long  gs_fn_dsp : 22; /* bit[0-21] : GS的FN计数（源自gs_timing_a[21:0]），由DSP 配置timing_get_ind触发上报。其中[10:0]表示fn_low,范围0~1325；[21:11]表示fn_hign,范围0~2047。
                                                      上报时刻：timing_valid为高时上报有效 */
        unsigned long  reserved  : 10; /* bit[22-31]: 保留 */
    } reg;
} BBPCOMM_GS_FN_DSP_UNION;
#define BBPCOMM_GS_FN_DSP_gs_fn_dsp_START  (0)
#define BBPCOMM_GS_FN_DSP_gs_fn_dsp_END    (21)


/*****************************************************************************
 结构名    : BBPCOMM_GS_QB_DSP_UNION
 结构说明  : GS_QB_DSP 寄存器结构定义。地址偏移量:0x067C，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      gs_qb_dsp_reg;
    struct
    {
        unsigned long  o_gs_qb_dsp : 13; /* bit[0-12] : GS的系统定时Qb信息，由DSP 配置timing_get_ind触发上报。
                                                         [12:0]表示 GS系统定时帧内Qb计数（源自gs_timing_b[12:0]），范围0~4999。
                                                        上报时刻：timing_valid为高时上报有效 */
        unsigned long  reserved    : 19; /* bit[13-31]: 保留 */
    } reg;
} BBPCOMM_GS_QB_DSP_UNION;
#define BBPCOMM_GS_QB_DSP_o_gs_qb_dsp_START  (0)
#define BBPCOMM_GS_QB_DSP_o_gs_qb_dsp_END    (12)


/*****************************************************************************
 结构名    : BBPCOMM_LTE_TIMING_DSP_UNION
 结构说明  : LTE_TIMING_DSP 寄存器结构定义。地址偏移量:0x0680，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      lte_timing_dsp_reg;
    struct
    {
        unsigned long  o_lte_sys_ts_cnt_dsp     : 15; /* bit[0-14] : [14:0]是子帧内计数。  */
        unsigned long  reserved_0               : 1;  /* bit[15-15]: 保留 */
        unsigned long  o_lte_sys_subfrm_cnt_dsp : 4;  /* bit[16-19]: [19:16]是子帧计数 */
        unsigned long  o_lte_sfn_dsp            : 10; /* bit[20-29]: LTE的系统定时。由DSP 配置timing_get_ind触发上报。上报时刻：timing_valid为高时上报有效。
                                                                     其中:
                                                                     [29:20]表示SFN计数，范围0~1023 */
        unsigned long  reserved_1               : 2;  /* bit[30-31]: 保留 */
    } reg;
} BBPCOMM_LTE_TIMING_DSP_UNION;
#define BBPCOMM_LTE_TIMING_DSP_o_lte_sys_ts_cnt_dsp_START      (0)
#define BBPCOMM_LTE_TIMING_DSP_o_lte_sys_ts_cnt_dsp_END        (14)
#define BBPCOMM_LTE_TIMING_DSP_o_lte_sys_subfrm_cnt_dsp_START  (16)
#define BBPCOMM_LTE_TIMING_DSP_o_lte_sys_subfrm_cnt_dsp_END    (19)
#define BBPCOMM_LTE_TIMING_DSP_o_lte_sfn_dsp_START             (20)
#define BBPCOMM_LTE_TIMING_DSP_o_lte_sfn_dsp_END               (29)


/*****************************************************************************
 结构名    : BBPCOMM_TDS_TIMING1_DSP_UNION
 结构说明  : TDS_TIMING1_DSP 寄存器结构定义。地址偏移量:0x0684，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      tds_timing1_dsp_reg;
    struct
    {
        unsigned long  o_tds_timing1_dsp : 16; /* bit[0-15] : TDS的系统定时。由DSP 配置timing_get_ind触发上报。本寄存器上报的是TDS的tcfsr系统子帧号0~65535循环计数，TDS实际有效子帧范围为0~8191
                                                              上报时刻：timing_valid为高时上报有效 */
        unsigned long  reserved          : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_TDS_TIMING1_DSP_UNION;
#define BBPCOMM_TDS_TIMING1_DSP_o_tds_timing1_dsp_START  (0)
#define BBPCOMM_TDS_TIMING1_DSP_o_tds_timing1_dsp_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_TDS_TIMING2_DSP_UNION
 结构说明  : TDS_TIMING2_DSP 寄存器结构定义。地址偏移量:0x0688，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      tds_timing2_dsp_reg;
    struct
    {
        unsigned long  o_tds_timing2_dsp : 20; /* bit[0-19] : TDS的系统定时。由DSP 配置timing_get_ind触发上报。本寄存器上报的是是TDS的framc帧计数
                                                              上报时刻：timing_valid为高时上报有效 */
        unsigned long  reserved          : 12; /* bit[20-31]: 保留 */
    } reg;
} BBPCOMM_TDS_TIMING2_DSP_UNION;
#define BBPCOMM_TDS_TIMING2_DSP_o_tds_timing2_dsp_START  (0)
#define BBPCOMM_TDS_TIMING2_DSP_o_tds_timing2_dsp_END    (19)


/*****************************************************************************
 结构名    : BBPCOMM_W_INTRASYS_VALID_UNION
 结构说明  : W_INTRASYS_VALID 寄存器结构定义。地址偏移量:0x0698，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      w_intrasys_valid_reg;
    struct
    {
        unsigned long  o_w_intrasys_valid       : 1;  /* bit[0]   : w主模下控制RF指示，电平信号。
                                                                    1’b0: W放弃RF控制权。
                                                                    1’b1: W享有控制RF权（但实际配置RF需w_intrasys_valid_delay信号为1）。 */
        unsigned long  o_w_intrasys_valid_delay : 1;  /* bit[1]   : w主模下实际控制RF指示，电平信号。为1代表W控制RF配置通道，为0代表从模可以控制RF通道。用于单RFIC时射频控制相关信号的选通。
                                                                    w_intrasys_valid_delay的上升沿比w_intrasys_valid上升沿滞后，下降沿一致。 */
        unsigned long  reserved                 : 30; /* bit[2-31]: 保留 */
    } reg;
} BBPCOMM_W_INTRASYS_VALID_UNION;
#define BBPCOMM_W_INTRASYS_VALID_o_w_intrasys_valid_START        (0)
#define BBPCOMM_W_INTRASYS_VALID_o_w_intrasys_valid_END          (0)
#define BBPCOMM_W_INTRASYS_VALID_o_w_intrasys_valid_delay_START  (1)
#define BBPCOMM_W_INTRASYS_VALID_o_w_intrasys_valid_delay_END    (1)


/*****************************************************************************
 结构名    : BBPCOMM_GM_INTRASYS_VALID_UNION
 结构说明  : GM_INTRASYS_VALID 寄存器结构定义。地址偏移量:0x069C，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      gm_intrasys_valid_reg;
    struct
    {
        unsigned long  o_gm_intrasys_valid       : 1;  /* bit[0]   : gm主模下控制RF指示，电平信号。
                                                                     1’b0: gm放弃RF控制权。
                                                                     1’b1: gm享有控制RF权（但实际配置RF需gm_intrasys_valid_delay信号为1）。 */
        unsigned long  o_gm_intrasys_valid_delay : 1;  /* bit[1]   : gm主模下实际控制RF指示，电平信号。为1代表gm控制RF配置通道，为0代表从模可以控制RF通道。用于单RFIC时射频控制相关信号的选通。
                                                                     gm_intrasys_valid_delay的上升沿比gm_intrasys_valid上升沿滞后，下降沿一致。 */
        unsigned long  reserved                  : 30; /* bit[2-31]: 保留 */
    } reg;
} BBPCOMM_GM_INTRASYS_VALID_UNION;
#define BBPCOMM_GM_INTRASYS_VALID_o_gm_intrasys_valid_START        (0)
#define BBPCOMM_GM_INTRASYS_VALID_o_gm_intrasys_valid_END          (0)
#define BBPCOMM_GM_INTRASYS_VALID_o_gm_intrasys_valid_delay_START  (1)
#define BBPCOMM_GM_INTRASYS_VALID_o_gm_intrasys_valid_delay_END    (1)


/*****************************************************************************
 结构名    : BBPCOMM_GS_INTRASYS_VALID_UNION
 结构说明  : GS_INTRASYS_VALID 寄存器结构定义。地址偏移量:0x06A0，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      gs_intrasys_valid_reg;
    struct
    {
        unsigned long  o_gs_intrasys_valid       : 1;  /* bit[0]   : gs主模下控制RF指示，电平信号。
                                                                     1’b0: gs放弃RF控制权。
                                                                     1’b1: gs享有控制RF权（但实际配置RF需gs_intrasys_valid_delay信号为1）。 */
        unsigned long  o_gs_intrasys_valid_delay : 1;  /* bit[1]   : gs主模下实际控制RF指示，电平信号。为1代表gs控制RF配置通道，为0代表从模可以控制RF通道。用于单RFIC时射频控制相关信号的选通。
                                                                     gs_intrasys_valid_delay的上升沿比gs_intrasys_valid上升沿滞后，下降沿一致。 */
        unsigned long  reserved                  : 30; /* bit[2-31]: 保留 */
    } reg;
} BBPCOMM_GS_INTRASYS_VALID_UNION;
#define BBPCOMM_GS_INTRASYS_VALID_o_gs_intrasys_valid_START        (0)
#define BBPCOMM_GS_INTRASYS_VALID_o_gs_intrasys_valid_END          (0)
#define BBPCOMM_GS_INTRASYS_VALID_o_gs_intrasys_valid_delay_START  (1)
#define BBPCOMM_GS_INTRASYS_VALID_o_gs_intrasys_valid_delay_END    (1)


/*****************************************************************************
 结构名    : BBPCOMM_LTE_INTRASYS_VALID_UNION
 结构说明  : LTE_INTRASYS_VALID 寄存器结构定义。地址偏移量:0x06A4，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      lte_intrasys_valid_reg;
    struct
    {
        unsigned long  o_lte_intrasys_valid       : 1;  /* bit[0]   : Lte主模下控制RF指示，电平信号。
                                                                      1’b0: LTE放弃RF控制权。
                                                                      1’b1: LTE享有控制RF权（但实际配置RF需lte_intrasys_valid_delay信号为1）。 */
        unsigned long  o_lte_intrasys_valid_delay : 1;  /* bit[1]   : LTE主模下实际控制RF指示，电平信号。为1代表LTE控制RF配置通道，为0代表从模可以控制RF通道。用于单RFIC时射频控制相关信号的选通。
                                                                      lte_intrasys_valid_delay的上升沿比lte_intrasys_valid上升沿滞后，下降沿一致。 */
        unsigned long  reserved                   : 30; /* bit[2-31]: 保留 */
    } reg;
} BBPCOMM_LTE_INTRASYS_VALID_UNION;
#define BBPCOMM_LTE_INTRASYS_VALID_o_lte_intrasys_valid_START        (0)
#define BBPCOMM_LTE_INTRASYS_VALID_o_lte_intrasys_valid_END          (0)
#define BBPCOMM_LTE_INTRASYS_VALID_o_lte_intrasys_valid_delay_START  (1)
#define BBPCOMM_LTE_INTRASYS_VALID_o_lte_intrasys_valid_delay_END    (1)


/*****************************************************************************
 结构名    : BBPCOMM_TDS_INTRASYS_VALID_UNION
 结构说明  : TDS_INTRASYS_VALID 寄存器结构定义。地址偏移量:0x06A8，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      tds_intrasys_valid_reg;
    struct
    {
        unsigned long  o_tds_intrasys_valid       : 1;  /* bit[0]   : TDS主模下控制RF指示，电平信号。
                                                                      1’b0: TDS放弃RF控制权。
                                                                      1’b1: TDS享有控制RF权（但实际配置RF需tds_intrasys_valid_delay信号为1）。 */
        unsigned long  o_tds_intrasys_valid_delay : 1;  /* bit[1]   : TDS主模下实际控制RF指示，电平信号。为1代表TDS控制RF配置通道，为0代表从模可以控制RF通道。用于单RFIC时射频控制相关信号的选通。
                                                                      tds_intrasys_valid_delay的上升沿比tds_intrasys_valid上升沿滞后，下降沿一致。 */
        unsigned long  reserved                   : 30; /* bit[2-31]: 保留 */
    } reg;
} BBPCOMM_TDS_INTRASYS_VALID_UNION;
#define BBPCOMM_TDS_INTRASYS_VALID_o_tds_intrasys_valid_START        (0)
#define BBPCOMM_TDS_INTRASYS_VALID_o_tds_intrasys_valid_END          (0)
#define BBPCOMM_TDS_INTRASYS_VALID_o_tds_intrasys_valid_delay_START  (1)
#define BBPCOMM_TDS_INTRASYS_VALID_o_tds_intrasys_valid_delay_END    (1)


/*****************************************************************************
 结构名    : BBPCOMM_RFIC_CK_CFG_UNION
 结构说明  : RFIC_CK_CFG 寄存器结构定义。地址偏移量:0x06AC，初值:0x00000009，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      rfic_ck_cfg_reg;
    struct
    {
        unsigned long  cpu_rfic0_ck_cfg : 3;  /* bit[0-2] : SSI1master等待时间 */
        unsigned long  cpu_rfic1_ck_cfg : 3;  /* bit[3-5] : SSI0master等待时间 */
        unsigned long  reserved         : 26; /* bit[6-31]: 保留 */
    } reg;
} BBPCOMM_RFIC_CK_CFG_UNION;
#define BBPCOMM_RFIC_CK_CFG_cpu_rfic0_ck_cfg_START  (0)
#define BBPCOMM_RFIC_CK_CFG_cpu_rfic0_ck_cfg_END    (2)
#define BBPCOMM_RFIC_CK_CFG_cpu_rfic1_ck_cfg_START  (3)
#define BBPCOMM_RFIC_CK_CFG_cpu_rfic1_ck_cfg_END    (5)


/*****************************************************************************
 结构名    : BBPCOMM_DSP_WAITTIME_UNION
 结构说明  : DSP_WAITTIME 寄存器结构定义。地址偏移量:0x06B4，初值:0x0000FFFF，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      dsp_waittime_reg;
    struct
    {
        unsigned long  dsp_mipi_wait_time : 8;  /* bit[0-7]  : 通过xbbp向MIPI master发出回读类指令时,DSP等待回读结果长度配置 */
        unsigned long  dsp_ssi_wait_time  : 8;  /* bit[8-15] : 通过xbbp向SSI master发出回读类指令时,DSP等待回读结果长度配置 */
        unsigned long  reserved           : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_DSP_WAITTIME_UNION;
#define BBPCOMM_DSP_WAITTIME_dsp_mipi_wait_time_START  (0)
#define BBPCOMM_DSP_WAITTIME_dsp_mipi_wait_time_END    (7)
#define BBPCOMM_DSP_WAITTIME_dsp_ssi_wait_time_START   (8)
#define BBPCOMM_DSP_WAITTIME_dsp_ssi_wait_time_END     (15)


/*****************************************************************************
 结构名    : BBPCOMM_DSP_CTU_TIMING_GET_IND_UNION
 结构说明  : DSP_CTU_TIMING_GET_IND 寄存器结构定义。地址偏移量:0x06BC，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      dsp_ctu_timing_get_ind_reg;
    struct
    {
        unsigned long  dsp_ctu_timing_get_ind : 1;  /* bit[0]   : CTU内部19.2M钟驱动定时计数器结果读取使能 */
        unsigned long  reserved               : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_DSP_CTU_TIMING_GET_IND_UNION;
#define BBPCOMM_DSP_CTU_TIMING_GET_IND_dsp_ctu_timing_get_ind_START  (0)
#define BBPCOMM_DSP_CTU_TIMING_GET_IND_dsp_ctu_timing_get_ind_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_CTU_TIMING_VALID_DSP_UNION
 结构说明  : CTU_TIMING_VALID_DSP 寄存器结构定义。地址偏移量:0x06C0，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      ctu_timing_valid_dsp_reg;
    struct
    {
        unsigned long  ctu_timing_valid_dsp : 1;  /* bit[0]   : CTU内部19.2M钟驱动定时计数器结果读取有效指示 */
        unsigned long  reserved             : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_CTU_TIMING_VALID_DSP_UNION;
#define BBPCOMM_CTU_TIMING_VALID_DSP_ctu_timing_valid_dsp_START  (0)
#define BBPCOMM_CTU_TIMING_VALID_DSP_ctu_timing_valid_dsp_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_DSP_CTU_TIMING_CLR_UNION
 结构说明  : DSP_CTU_TIMING_CLR 寄存器结构定义。地址偏移量:0x06C4，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      dsp_ctu_timing_clr_reg;
    struct
    {
        unsigned long  dsp_ctu_timing_clr : 1;  /* bit[0]   : CTU内部19.2M钟驱动定时计数器结果读取后清零使能 */
        unsigned long  reserved           : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_DSP_CTU_TIMING_CLR_UNION;
#define BBPCOMM_DSP_CTU_TIMING_CLR_dsp_ctu_timing_clr_START  (0)
#define BBPCOMM_DSP_CTU_TIMING_CLR_dsp_ctu_timing_clr_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_W_INTERSYS_MEASURE_TYPE_UNION
 结构说明  : W_INTERSYS_MEASURE_TYPE 寄存器结构定义。地址偏移量:0x06C8，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      w_intersys_measure_type_reg;
    struct
    {
        unsigned long  w_intersys_measure_type : 4;  /* bit[0-3] : 异系统测量类型。
                                                                   4’b0：测量3G。
                                                                   4’b1: 3G背景搜；
                                                                   4’d2: 3G ANR。
                                                                   其它预留。
                                                                   生效时刻：立即生效 */
        unsigned long  reserved                : 28; /* bit[4-31]: 保留 */
    } reg;
} BBPCOMM_W_INTERSYS_MEASURE_TYPE_UNION;
#define BBPCOMM_W_INTERSYS_MEASURE_TYPE_w_intersys_measure_type_START  (0)
#define BBPCOMM_W_INTERSYS_MEASURE_TYPE_w_intersys_measure_type_END    (3)


/*****************************************************************************
 结构名    : BBPCOMM_W_MEASURE_REPORT_VALID_UNION
 结构说明  : W_MEASURE_REPORT_VALID 寄存器结构定义。地址偏移量:0x06CC，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      w_measure_report_valid_reg;
    struct
    {
        unsigned long  w_measure_report_valid : 1;  /* bit[0-0] : 单频点测量周期到指示，L或者TDS/Gm/Gs作为主模，通知W DSP W测量的单频点周期到达。
                                                                  只在连接态下需要配置。
                                                                  电平信号，高有效。
                                                                  生效时刻：立即生效 */
        unsigned long  reserved               : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_W_MEASURE_REPORT_VALID_UNION;
#define BBPCOMM_W_MEASURE_REPORT_VALID_w_measure_report_valid_START  (0)
#define BBPCOMM_W_MEASURE_REPORT_VALID_w_measure_report_valid_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_INT_CLEAR_W_RCV_LTE_UNION
 结构说明  : INT_CLEAR_W_RCV_LTE 寄存器结构定义。地址偏移量:0x06D8，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      int_clear_w_rcv_lte_reg;
    struct
    {
        unsigned long  int_clear_w_rcv_lte : 16; /* bit[0-15] : 逻辑自清零。
                                                                中断标志清除指示，高有效。用于清除INT_TYPE_W_RCV_LTE[15:0]对应bit位
                                                                生效时刻：立即生效 */
        unsigned long  reserved            : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_INT_CLEAR_W_RCV_LTE_UNION;
#define BBPCOMM_INT_CLEAR_W_RCV_LTE_int_clear_w_rcv_lte_START  (0)
#define BBPCOMM_INT_CLEAR_W_RCV_LTE_int_clear_w_rcv_lte_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_INT_CLEAR_W_RCV_TDS_UNION
 结构说明  : INT_CLEAR_W_RCV_TDS 寄存器结构定义。地址偏移量:0x06DC，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      int_clear_w_rcv_tds_reg;
    struct
    {
        unsigned long  int_clear_w_rcv_tds : 16; /* bit[0-15] : 逻辑自清零。
                                                                中断标志清除指示，高有效。用于清除INT_TYPE_W_RCV_TDS[15:0]对应bit位
                                                                生效时刻：立即生效 */
        unsigned long  reserved            : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_INT_CLEAR_W_RCV_TDS_UNION;
#define BBPCOMM_INT_CLEAR_W_RCV_TDS_int_clear_w_rcv_tds_START  (0)
#define BBPCOMM_INT_CLEAR_W_RCV_TDS_int_clear_w_rcv_tds_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_INT_CLEAR_W_RCV_GM_UNION
 结构说明  : INT_CLEAR_W_RCV_GM 寄存器结构定义。地址偏移量:0x06E0，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      int_clear_w_rcv_gm_reg;
    struct
    {
        unsigned long  int_clear_w_rcv_gm : 16; /* bit[0-15] : 逻辑自清零。
                                                               中断标志清除指示，高有效。用于清除INT_TYPE_W_RCV_GM[15:0]对应bit位
                                                               生效时刻：立即生效 */
        unsigned long  reserved           : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_INT_CLEAR_W_RCV_GM_UNION;
#define BBPCOMM_INT_CLEAR_W_RCV_GM_int_clear_w_rcv_gm_START  (0)
#define BBPCOMM_INT_CLEAR_W_RCV_GM_int_clear_w_rcv_gm_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_INT_CLEAR_W_RCV_GS_UNION
 结构说明  : INT_CLEAR_W_RCV_GS 寄存器结构定义。地址偏移量:0x06E4，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      int_clear_w_rcv_gs_reg;
    struct
    {
        unsigned long  int_clear_w_rcv_gs : 16; /* bit[0-15] : 逻辑自清零。
                                                               中断标志清除指示，高有效。用于清除INT_TYPE_W_RCV_GS[15:0]对应bit位
                                                               生效时刻：立即生效 */
        unsigned long  reserved           : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_INT_CLEAR_W_RCV_GS_UNION;
#define BBPCOMM_INT_CLEAR_W_RCV_GS_int_clear_w_rcv_gs_START  (0)
#define BBPCOMM_INT_CLEAR_W_RCV_GS_int_clear_w_rcv_gs_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_INT_CLEAR_W_RCV_W_UNION
 结构说明  : INT_CLEAR_W_RCV_W 寄存器结构定义。地址偏移量:0x06E8，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      int_clear_w_rcv_w_reg;
    struct
    {
        unsigned long  int_clear_w_rcv_w : 16; /* bit[0-15] : 逻辑自清零。
                                                              中断标志清除指示，高有效。用于清除INT_TYPE_W_RCV_W[15:0]对应bit位
                                                              生效时刻：立即生效 */
        unsigned long  reserved          : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_INT_CLEAR_W_RCV_W_UNION;
#define BBPCOMM_INT_CLEAR_W_RCV_W_int_clear_w_rcv_w_START  (0)
#define BBPCOMM_INT_CLEAR_W_RCV_W_int_clear_w_rcv_w_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_INT_CLEAR_W_INT012_UNION
 结构说明  : INT_CLEAR_W_INT012 寄存器结构定义。地址偏移量:0x06EC，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      int_clear_w_int012_reg;
    struct
    {
        unsigned long  int_clear_w_int012 : 3;  /* bit[0-2] : 逻辑自清零。
                                                              中断标志清除指示，高有效。用于清除INT_TYPE_W_INT012[2:0]对应bit位
                                                              生效时刻：立即生效 */
        unsigned long  reserved           : 29; /* bit[3-31]: 保留 */
    } reg;
} BBPCOMM_INT_CLEAR_W_INT012_UNION;
#define BBPCOMM_INT_CLEAR_W_INT012_int_clear_w_int012_START  (0)
#define BBPCOMM_INT_CLEAR_W_INT012_int_clear_w_int012_END    (2)


/*****************************************************************************
 结构名    : BBPCOMM_INT_MASK_W_RCV_LTE_UNION
 结构说明  : INT_MASK_W_RCV_LTE 寄存器结构定义。地址偏移量:0x06F0，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      int_mask_w_rcv_lte_reg;
    struct
    {
        unsigned long  int_mask_w_rcv_lte : 16; /* bit[0-15] : 中断屏蔽标志。用于屏蔽INT_TYPE_W_RCV_LTE[15:0]对应bit中断。
                                                               0：屏蔽中断上报；
                                                               1：使能中断上报。
                                                               配置时刻：初始化
                                                               生效时刻：立即生效 */
        unsigned long  reserved           : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_INT_MASK_W_RCV_LTE_UNION;
#define BBPCOMM_INT_MASK_W_RCV_LTE_int_mask_w_rcv_lte_START  (0)
#define BBPCOMM_INT_MASK_W_RCV_LTE_int_mask_w_rcv_lte_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_INT_MASK_W_RCV_TDS_UNION
 结构说明  : INT_MASK_W_RCV_TDS 寄存器结构定义。地址偏移量:0x06F4，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      int_mask_w_rcv_tds_reg;
    struct
    {
        unsigned long  int_mask_w_rcv_tds : 16; /* bit[0-15] : 中断屏蔽标志。用于屏蔽INT_TYPE_W_RCV_TDS[15:0]对应bit中断。
                                                               0：屏蔽中断上报；
                                                               1：使能中断上报。
                                                               配置时刻：初始化
                                                               生效时刻：立即生效 */
        unsigned long  reserved           : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_INT_MASK_W_RCV_TDS_UNION;
#define BBPCOMM_INT_MASK_W_RCV_TDS_int_mask_w_rcv_tds_START  (0)
#define BBPCOMM_INT_MASK_W_RCV_TDS_int_mask_w_rcv_tds_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_INT_MASK_W_RCV_GM_UNION
 结构说明  : INT_MASK_W_RCV_GM 寄存器结构定义。地址偏移量:0x06F8，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      int_mask_w_rcv_gm_reg;
    struct
    {
        unsigned long  int_mask_w_rcv_gm : 16; /* bit[0-15] : 中断屏蔽标志。用于屏蔽INT_TYPE_W_RCV_GM[15:0]对应bit中断。
                                                              0：屏蔽中断上报；
                                                              1：使能中断上报。
                                                              配置时刻：初始化
                                                              生效时刻：立即生效 */
        unsigned long  reserved          : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_INT_MASK_W_RCV_GM_UNION;
#define BBPCOMM_INT_MASK_W_RCV_GM_int_mask_w_rcv_gm_START  (0)
#define BBPCOMM_INT_MASK_W_RCV_GM_int_mask_w_rcv_gm_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_INT_MASK_W_RCV_GS_UNION
 结构说明  : INT_MASK_W_RCV_GS 寄存器结构定义。地址偏移量:0x06FC，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      int_mask_w_rcv_gs_reg;
    struct
    {
        unsigned long  int_mask_w_rcv_gs : 16; /* bit[0-15] : 中断屏蔽标志。用于屏蔽INT_TYPE_W_RCV_GS[15:0]对应bit中断。
                                                              0：屏蔽中断上报；
                                                              1：使能中断上报。
                                                              配置时刻：初始化
                                                              生效时刻：立即生效 */
        unsigned long  reserved          : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_INT_MASK_W_RCV_GS_UNION;
#define BBPCOMM_INT_MASK_W_RCV_GS_int_mask_w_rcv_gs_START  (0)
#define BBPCOMM_INT_MASK_W_RCV_GS_int_mask_w_rcv_gs_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_INT_MASK_W_RCV_W_UNION
 结构说明  : INT_MASK_W_RCV_W 寄存器结构定义。地址偏移量:0x0700，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      int_mask_w_rcv_w_reg;
    struct
    {
        unsigned long  int_mask_w_rcv_w : 16; /* bit[0-15] : 中断屏蔽标志。用于屏蔽INT_TYPE_W_RCV_W[15:0]对应bit中断。
                                                             0：屏蔽中断上报；
                                                             1：使能中断上报。
                                                             配置时刻：初始化
                                                             生效时刻：立即生效 */
        unsigned long  reserved         : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_INT_MASK_W_RCV_W_UNION;
#define BBPCOMM_INT_MASK_W_RCV_W_int_mask_w_rcv_w_START  (0)
#define BBPCOMM_INT_MASK_W_RCV_W_int_mask_w_rcv_w_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_INT_MASK_W_INT012_UNION
 结构说明  : INT_MASK_W_INT012 寄存器结构定义。地址偏移量:0x0704，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      int_mask_w_int012_reg;
    struct
    {
        unsigned long  int_mask_w_int012 : 3;  /* bit[0-2] : 中断屏蔽标志。用于屏蔽INT_TYPE_W_INT012[2:0]对应bit中断。
                                                             0：屏蔽中断上报；
                                                             1：使能中断上报。
                                                             配置时刻：初始化
                                                             生效时刻：立即生效 */
        unsigned long  reserved          : 29; /* bit[3-31]: 保留 */
    } reg;
} BBPCOMM_INT_MASK_W_INT012_UNION;
#define BBPCOMM_INT_MASK_W_INT012_int_mask_w_int012_START  (0)
#define BBPCOMM_INT_MASK_W_INT012_int_mask_w_int012_END    (2)


/*****************************************************************************
 结构名    : BBPCOMM_INT_TYPE_W_RCV_LTE_UNION
 结构说明  : INT_TYPE_W_RCV_LTE 寄存器结构定义。地址偏移量:0x0708，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      int_type_w_rcv_lte_reg;
    struct
    {
        unsigned long  o_int_type_w_rcv_lte : 16; /* bit[0-15] : 中断状态标志。各bit单独对应一种中断类型，高有效。
                                                                 [0]: 中断类型0。 LTE（主模）提前通知W异系统测量启动中断。
                                                                 [1]: 中断类型1。 LTE（主模）进GAP中断，通知W进行异系统测量射频通道配置，天线切换等操作。
                                                                 [2]: 中断类型2。 LTE（主模）出GAP中断。
                                                                 [3]: 中断类型3。LTE（从模）测量结束中断。
                                                                 [4]: 中断类型4。LTE（主模）强制W退出异系统测量中断。
                                                                 [5]: 中断类型5。LTE（主模）通知W上报本次测量结果。
                                                                 [6]: 保留。
                                                                 [15:7]: 对应中断类型[15:7]，预留。
                                                                 软件读取该中断状态上报寄存器后，需要和0x06F0的int_mask_w_rcv_lte相与，再判断是否需要处理。 */
        unsigned long  reserved             : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_INT_TYPE_W_RCV_LTE_UNION;
#define BBPCOMM_INT_TYPE_W_RCV_LTE_o_int_type_w_rcv_lte_START  (0)
#define BBPCOMM_INT_TYPE_W_RCV_LTE_o_int_type_w_rcv_lte_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_INT_TYPE_W_RCV_TDS_UNION
 结构说明  : INT_TYPE_W_RCV_TDS 寄存器结构定义。地址偏移量:0x070C，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      int_type_w_rcv_tds_reg;
    struct
    {
        unsigned long  o_int_type_w_rcv_tds : 16; /* bit[0-15] : 中断状态标志0，1，2只有当TDS处于主模时才能产生。
                                                                 
                                                                 [0]: 中断类型0。 TDS（主模）提前通知W异系统测量启动中断。
                                                                 [1]: 中断类型1。 TDS（主模）进GAP中断，通知W进行异系统测量射频通道配置，天线切换等操作。
                                                                 [2]: 中断类型2。 TDS（主模）出GAP中断。
                                                                 [3]: 中断类型3。TDS（从模）测量结束中断。
                                                                 [4]: 中断类型4。TDS（主模）强制W退出异系统测量中断。
                                                                 [5]: 中断类型5。TDS（主模）通知W上报本次测量结果。
                                                                 [6]: 中断类型6。保留。
                                                                 [15:7]: 对应中断类型[15:7]，预留。
                                                                 软件读取该中断状态上报寄存器后，需要和0x06F4的int_mask_w_rcv_tds相与，再判断是否需要处理。 */
        unsigned long  reserved             : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_INT_TYPE_W_RCV_TDS_UNION;
#define BBPCOMM_INT_TYPE_W_RCV_TDS_o_int_type_w_rcv_tds_START  (0)
#define BBPCOMM_INT_TYPE_W_RCV_TDS_o_int_type_w_rcv_tds_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_INT_TYPE_W_RCV_GM_UNION
 结构说明  : INT_TYPE_W_RCV_GM 寄存器结构定义。地址偏移量:0x0710，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      int_type_w_rcv_gm_reg;
    struct
    {
        unsigned long  o_int_type_w_rcv_gm : 16; /* bit[0-15] : 中断状态标志。各bit单独对应一种中断类型，高有效。
                                                                [0]: 中断类型0。GM（主模）提前通知W异系统测量启动中断。
                                                                [1]: 中断类型1。GM（主模）进GAP中断，通知W进行异系统测量射频通道配置，天线切换等操作。
                                                                [2]: 中断类型2。GM（主模）出GAP中断。
                                                                [3]: 中断类型3。GM（从模）测量结束中断。
                                                                [4]: 中断类型4。GM（主模）强制W退出异系统测量中断。
                                                                [5]: 中断类型5。GM（主模）通知W上报本次测量结果。
                                                                [6]: 中断类型6。保留。
                                                                [15:7]: 对应中断类型[15:7]，预留。
                                                                中断状态标志0，1，2只有当GM处于主模时才能产生。
                                                                软件读取该中断状态上报寄存器后，需要和0x06F8的int_mask_w_rcv_gm相与，再判断是否需要处理。 */
        unsigned long  reserved            : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_INT_TYPE_W_RCV_GM_UNION;
#define BBPCOMM_INT_TYPE_W_RCV_GM_o_int_type_w_rcv_gm_START  (0)
#define BBPCOMM_INT_TYPE_W_RCV_GM_o_int_type_w_rcv_gm_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_INT_TYPE_W_RCV_GS_UNION
 结构说明  : INT_TYPE_W_RCV_GS 寄存器结构定义。地址偏移量:0x0714，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      int_type_w_rcv_gs_reg;
    struct
    {
        unsigned long  o_int_type_w_rcv_gs : 16; /* bit[0-15] : [0]: 中断类型0。GS（主模）提前通知W异系统测量启动中断。
                                                                [1]: 中断类型1。GS（主模）进GAP中断，通知W进行异系统测量射频通道配置，天线切换等操作。
                                                                [2]: 中断类型2。GS（主模）出GAP中断。
                                                                [3]: 中断类型3。GS（从模）测量结束中断。
                                                                [4]: 中断类型4。GS（主模）强制W退出异系统测量中断。
                                                                [5]: 中断类型5。GS（主模）通知W上报本次测量结果。
                                                                [6]: 中断类型6。保留。
                                                                [15:7]: 对应中断类型[15:7]，预留。
                                                                中断状态标志0，1，2只有当TDS处于主模时才能产生。
                                                                软件读取该中断状态上报寄存器后，需要和0x06FC的int_mask_w_rcv_gs相与，再判断是否需要处理。 */
        unsigned long  reserved            : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_INT_TYPE_W_RCV_GS_UNION;
#define BBPCOMM_INT_TYPE_W_RCV_GS_o_int_type_w_rcv_gs_START  (0)
#define BBPCOMM_INT_TYPE_W_RCV_GS_o_int_type_w_rcv_gs_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_INT_TYPE_W_RCV_W_UNION
 结构说明  : INT_TYPE_W_RCV_W 寄存器结构定义。地址偏移量:0x0718，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      int_type_w_rcv_w_reg;
    struct
    {
        unsigned long  o_int_type_w_rcv_w : 16; /* bit[0-15] : 中断状态标志。各bit单独对应一种中断类型，高有效。
                                                               [0]: 中断类型0。W（主模）提前通知W异频测量启动中断。
                                                               [1]: 中断类型1。W（主模）进GAP中断，通知W进行异频测量射频通道配置，天线切换等操作。
                                                               [2]: 中断类型2。W（主模）出GAP中断。
                                                               [3]: 中断类型3。W（从模，即异频）测量结束中断。
                                                               [4]: 中断类型4。W（主模）强制W退出异频测量中断。
                                                               [5]: 中断类型5。W（主模）通知W上报本次测量结果。
                                                               [6]: 中断类型6。保留。
                                                               [15:7]: 对应中断类型[15:7]，预留。
                                                               中断状态标志0，1，2只有当W处于主模时才能产生。
                                                               软件读取该中断状态上报寄存器后，需要和0x0700的int_mask_w_rcv_w相与，再判断是否需要处理。 */
        unsigned long  reserved           : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_INT_TYPE_W_RCV_W_UNION;
#define BBPCOMM_INT_TYPE_W_RCV_W_o_int_type_w_rcv_w_START  (0)
#define BBPCOMM_INT_TYPE_W_RCV_W_o_int_type_w_rcv_w_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_INT_TYPE_W_INT012_UNION
 结构说明  : INT_TYPE_W_INT012 寄存器结构定义。地址偏移量:0x071C，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      int_type_w_int012_reg;
    struct
    {
        unsigned long  o_int_type_w_int012 : 3;  /* bit[0-2] : 增加主模产生int0、1、2的上报（(包括异频、异系统场景)），高有效。
                                                               [0]: W主模时，CTU告诉通知WDSP产生int0中断。
                                                               [1]: W主模时，CTU告诉通知WDSP产生int1中断。
                                                               [2]: W主模时，CTU告诉通知WDSP产生int2中断。
                                                               软件读取该中断状态上报寄存器后，需要和0x0704的int_mask_w_int012相与，再判断是否需要处理。 */
        unsigned long  reserved            : 29; /* bit[3-31]: 保留 */
    } reg;
} BBPCOMM_INT_TYPE_W_INT012_UNION;
#define BBPCOMM_INT_TYPE_W_INT012_o_int_type_w_int012_START  (0)
#define BBPCOMM_INT_TYPE_W_INT012_o_int_type_w_int012_END    (2)


/*****************************************************************************
 结构名    : BBPCOMM_W_SND_LTE_INT_13_UNION
 结构说明  : W_SND_LTE_INT_13 寄存器结构定义。地址偏移量:0x0720，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      w_snd_lte_int_13_reg;
    struct
    {
        unsigned long  reserved_0       : 3;  /* bit[0-2]  : 保留 */
        unsigned long  w_snd_lte_int_13 : 13; /* bit[3-15] : 逻辑自清零。高有效。
                                                             
                                                             bit15~7：对应w_snd_lte_int_15 ~ 7。
                                                             W DSP通过此寄存器触发类型15~7中断（int_type_lte_rcv_w[15:7]）给LDSP。
                                                             生效时刻：立即生效
                                                             
                                                             bit6：对应w_snd_lte_int_6。
                                                             WDSP通过此寄存器触发类型6中断（int_type_lte_rcv_w[6]）给LDSP。
                                                             生效时刻：立即生效
                                                             
                                                             bit5：对应w_snd_lte_int_5。
                                                             W主模时通知LDSP L测量周期到达，高有效。
                                                             W DSP通过此寄存器触发类型5中断（int_type_lte_rcv_w[5]）给LDSP。
                                                             生效时刻：立即生效
                                                             
                                                             bit4：对应w_snd_lte_int_4。
                                                             W主模时强制退出异系统测量指示。高有效。
                                                             W DSP通过此寄存器触发类型4中断（int_type_lte_rcv_w[4]）给LDSP。
                                                             生效时刻：立即生效
                                                             
                                                             bit3：对应w_snd_lte_int_3。W测量结束指示，高有效。
                                                             用于触发中断类型3 （int_type_lte_rcv_w[3]）给LDSP。L主模下对W进行异系统测量时，如果W测量结束，W DSP配置此标志为1。 
                                                             生效时刻：立即生效 */
        unsigned long  reserved_1       : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_W_SND_LTE_INT_13_UNION;
#define BBPCOMM_W_SND_LTE_INT_13_w_snd_lte_int_13_START  (3)
#define BBPCOMM_W_SND_LTE_INT_13_w_snd_lte_int_13_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_W_SND_TDS_INT_13_UNION
 结构说明  : W_SND_TDS_INT_13 寄存器结构定义。地址偏移量:0x0724，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      w_snd_tds_int_13_reg;
    struct
    {
        unsigned long  reserved_0       : 3;  /* bit[0-2]  : 保留 */
        unsigned long  w_snd_tds_int_13 : 13; /* bit[3-15] : 逻辑自清零。高有效。
                                                             
                                                             bit15~7：对应w_snd_tds_int_15 ~ 7。
                                                             W DSP通过此寄存器触发类型15~7中断（int_type_tds_rcv_w[15:7]）给TDSP。
                                                             生效时刻：立即生效
                                                             
                                                             bit6：对应w_snd_tds_int_6。
                                                             WDSP通过此寄存器触发类型6中断（int_type_tds_rcv_w[6]）给TDSP。
                                                             生效时刻：立即生效
                                                             
                                                             bit5：对应w_snd_tds_int_5。
                                                             W主模时通知TDSP T测量周期到达，高有效。
                                                             W DSP通过此寄存器触发类型5中断（int_type_tds_rcv_w[5]）给TDSP。
                                                             生效时刻：立即生效
                                                             
                                                             bit4：对应w_snd_tds_int_4。
                                                             W主模时强制退出异系统测量指示。高有效。
                                                             W DSP通过此寄存器触发类型4中断（int_type_tds_rcv_w[4]）给TDSP。
                                                             生效时刻：立即生效
                                                             
                                                             bit3：对应w_snd_tds_int_3。W测量结束指示，高有效。
                                                             用于触发中断类型3 （int_type_tds_rcv_w[3]）给TDSP。T主模下对W进行异系统测量时，如果W测量结束，W DSP配置此标志为1。 
                                                             生效时刻：立即生效 */
        unsigned long  reserved_1       : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_W_SND_TDS_INT_13_UNION;
#define BBPCOMM_W_SND_TDS_INT_13_w_snd_tds_int_13_START  (3)
#define BBPCOMM_W_SND_TDS_INT_13_w_snd_tds_int_13_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_W_SND_GM_INT_13_UNION
 结构说明  : W_SND_GM_INT_13 寄存器结构定义。地址偏移量:0x0728，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      w_snd_gm_int_13_reg;
    struct
    {
        unsigned long  reserved_0      : 3;  /* bit[0-2]  : 保留 */
        unsigned long  w_snd_gm_int_13 : 13; /* bit[3-15] : 逻辑自清零。高有效。
                                                            
                                                            bit15~7：对应w_snd_gm_int_15 ~ 7。
                                                            W DSP通过此寄存器触发类型15~7中断（int_type_gm_rcv_w[15:7]）给GMDSP。
                                                            生效时刻：立即生效
                                                            
                                                            bit6：对应w_snd_gm_int_6。
                                                            WDSP通过此寄存器触发类型6中断（int_type_gm_rcv_w[6]）给GMDSP。
                                                            生效时刻：立即生效
                                                            
                                                            bit5：对应w_snd_gm_int_5。
                                                            W主模时通知GMDSP GM测量周期到达，高有效。
                                                            W DSP通过此寄存器触发类型5中断（int_type_gm_rcv_w[5]）给GMDSP。
                                                            生效时刻：立即生效
                                                            
                                                            bit4：对应w_snd_gm_int_4。
                                                            W主模时强制退出异系统测量指示。高有效。
                                                            W DSP通过此寄存器触发类型4中断（int_type_gm_rcv_w[4]）给GMDSP。
                                                            生效时刻：立即生效
                                                            
                                                            bit3：对应w_snd_gm_int_3。W测量结束指示，高有效。
                                                            用于触发中断类型3 （int_type_gm_rcv_w[3]）给GMDSP。GM主模下对W进行异系统测量时，如果W测量结束，W DSP配置此标志为1。 
                                                            生效时刻：立即生效 */
        unsigned long  reserved_1      : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_W_SND_GM_INT_13_UNION;
#define BBPCOMM_W_SND_GM_INT_13_w_snd_gm_int_13_START  (3)
#define BBPCOMM_W_SND_GM_INT_13_w_snd_gm_int_13_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_W_SND_GS_INT_13_UNION
 结构说明  : W_SND_GS_INT_13 寄存器结构定义。地址偏移量:0x072C，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      w_snd_gs_int_13_reg;
    struct
    {
        unsigned long  reserved_0      : 3;  /* bit[0-2]  : 保留 */
        unsigned long  w_snd_gs_int_13 : 13; /* bit[3-15] : 逻辑自清零。高有效。
                                                            
                                                            bit15~7：对应w_snd_gs_int_15 ~ 7。
                                                            W DSP通过此寄存器触发类型15~7中断（int_type_gs_rcv_w[15:7]）给GSDSP。
                                                            生效时刻：立即生效
                                                            
                                                            bit6：对应w_snd_gs_int_6。
                                                            WDSP通过此寄存器触发类型6中断（int_type_gs_rcv_w[6]）给GSDSP。
                                                            生效时刻：立即生效
                                                            
                                                            bit5：对应w_snd_gs_int_5。
                                                            W主模时通知GSDSP GS测量周期到达，高有效。
                                                            W DSP通过此寄存器触发类型5中断（int_type_gs_rcv_w[5]）给GSDSP。
                                                            生效时刻：立即生效
                                                            
                                                            bit4：对应w_snd_gs_int_4。
                                                            W主模时强制退出异系统测量指示。高有效。
                                                            W DSP通过此寄存器触发类型4中断（int_type_gs_rcv_w[4]）给GSDSP。
                                                            生效时刻：立即生效
                                                            
                                                            bit3：对应w_snd_gs_int_3。W测量结束指示，高有效。
                                                            用于触发中断类型3 （int_type_gs_rcv_w[3]）给GSDSP。GS主模下对W进行异系统测量时，如果W测量结束，W DSP配置此标志为1。 
                                                            生效时刻：立即生效 */
        unsigned long  reserved_1      : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_W_SND_GS_INT_13_UNION;
#define BBPCOMM_W_SND_GS_INT_13_w_snd_gs_int_13_START  (3)
#define BBPCOMM_W_SND_GS_INT_13_w_snd_gs_int_13_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_W_SND_W_INT_13_UNION
 结构说明  : W_SND_W_INT_13 寄存器结构定义。地址偏移量:0x0730，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      w_snd_w_int_13_reg;
    struct
    {
        unsigned long  reserved_0     : 3;  /* bit[0-2]  : 保留 */
        unsigned long  w_snd_w_int_13 : 13; /* bit[3-15] : 逻辑自清零。高有效。
                                                           
                                                           bit15~7：对应w_snd_w_int_15 ~ 7。
                                                           W DSP通过此寄存器触发类型15~7中断（int_type_w_rcv_w[15:7]）给WDSP。
                                                           生效时刻：立即生效
                                                           
                                                           bit6：对应w_snd_w_int_6。
                                                           WDSP通过此寄存器触发类型6中断（int_type_w_rcv_w[6]）给WDSP。
                                                           生效时刻：立即生效
                                                           
                                                           bit5：对应w_snd_w_int_5。
                                                           W主模时通知WDSP W测量周期到达，高有效。
                                                           W DSP通过此寄存器触发类型5中断（int_type_w_rcv_w[5]）给WDSP。
                                                           生效时刻：立即生效
                                                           
                                                           bit4：对应w_snd_w_int_4。
                                                           W主模时强制退出异系统测量指示。高有效。
                                                           W DSP通过此寄存器触发类型4中断（int_type_w_rcv_w[4]）给WDSP。
                                                           生效时刻：立即生效
                                                           
                                                           bit3：对应w_snd_w_int_3。W测量结束指示，高有效。
                                                           用于触发中断类型3 （int_type_w_rcv_w[3]）给WDSP。W主模下对W进行异系统测量时，如果W测量结束，W DSP配置此标志为1。 
                                                           生效时刻：立即生效 */
        unsigned long  reserved_1     : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_W_SND_W_INT_13_UNION;
#define BBPCOMM_W_SND_W_INT_13_w_snd_w_int_13_START  (3)
#define BBPCOMM_W_SND_W_INT_13_w_snd_w_int_13_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_LTE_INTERSYS_MEASURE_TYPE_UNION
 结构说明  : LTE_INTERSYS_MEASURE_TYPE 寄存器结构定义。地址偏移量:0x0734，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      lte_intersys_measure_type_reg;
    struct
    {
        unsigned long  lte_intersys_measure_type : 4;  /* bit[0-3] : 异系统测量类型。
                                                                     4’b0：测量LTE。
                                                                     4’b1：背景搜LTE。
                                                                     其它预留
                                                                     生效时刻：立即生效 */
        unsigned long  reserved                  : 28; /* bit[4-31]: 保留 */
    } reg;
} BBPCOMM_LTE_INTERSYS_MEASURE_TYPE_UNION;
#define BBPCOMM_LTE_INTERSYS_MEASURE_TYPE_lte_intersys_measure_type_START  (0)
#define BBPCOMM_LTE_INTERSYS_MEASURE_TYPE_lte_intersys_measure_type_END    (3)


/*****************************************************************************
 结构名    : BBPCOMM_LTE_MEASURE_REPORT_VALID_UNION
 结构说明  : LTE_MEASURE_REPORT_VALID 寄存器结构定义。地址偏移量:0x0738，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      lte_measure_report_valid_reg;
    struct
    {
        unsigned long  lte_measure_report_valid : 1;  /* bit[0]   : 单频点测量周期到指示，W/Gm/Gs或者TDS作为主模，通知L DSP 测量的单频点周期到达。
                                                                    只在连接态下需要配置，且Gm测L时不需要配置；
                                                                    电平信号，高有效。
                                                                    
                                                                    生效时刻：立即生效 */
        unsigned long  reserved                 : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_LTE_MEASURE_REPORT_VALID_UNION;
#define BBPCOMM_LTE_MEASURE_REPORT_VALID_lte_measure_report_valid_START  (0)
#define BBPCOMM_LTE_MEASURE_REPORT_VALID_lte_measure_report_valid_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_INT_CLEAR_LTE_RCV_LTE_UNION
 结构说明  : INT_CLEAR_LTE_RCV_LTE 寄存器结构定义。地址偏移量:0x0744，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      int_clear_lte_rcv_lte_reg;
    struct
    {
        unsigned long  int_clear_lte_rcv_lte : 16; /* bit[0-15] : 逻辑自清零。
                                                                  中断标志清除指示，高有效。用于清除INT_TYPE_LTE_RCV_LTE[15:0]对应bit位
                                                                  生效时刻：立即生效 */
        unsigned long  reserved              : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_INT_CLEAR_LTE_RCV_LTE_UNION;
#define BBPCOMM_INT_CLEAR_LTE_RCV_LTE_int_clear_lte_rcv_lte_START  (0)
#define BBPCOMM_INT_CLEAR_LTE_RCV_LTE_int_clear_lte_rcv_lte_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_INT_CLEAR_LTE_RCV_TDS_UNION
 结构说明  : INT_CLEAR_LTE_RCV_TDS 寄存器结构定义。地址偏移量:0x0748，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      int_clear_lte_rcv_tds_reg;
    struct
    {
        unsigned long  int_clear_lte_rcv_tds : 16; /* bit[0-15] : 逻辑自清零。
                                                                  中断标志清除指示，高有效。用于清除INT_TYPE_LTE_RCV_TDS[15:0]对应bit位
                                                                  生效时刻：立即生效 */
        unsigned long  reserved              : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_INT_CLEAR_LTE_RCV_TDS_UNION;
#define BBPCOMM_INT_CLEAR_LTE_RCV_TDS_int_clear_lte_rcv_tds_START  (0)
#define BBPCOMM_INT_CLEAR_LTE_RCV_TDS_int_clear_lte_rcv_tds_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_INT_CLEAR_LTE_RCV_GM_UNION
 结构说明  : INT_CLEAR_LTE_RCV_GM 寄存器结构定义。地址偏移量:0x074C，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      int_clear_lte_rcv_gm_reg;
    struct
    {
        unsigned long  int_clear_lte_rcv_gm : 16; /* bit[0-15] : 逻辑自清零。
                                                                 中断标志清除指示，高有效。用于清除INT_TYPE_LTE_RCV_GM[15:0]对应bit位
                                                                 生效时刻：立即生效 */
        unsigned long  reserved             : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_INT_CLEAR_LTE_RCV_GM_UNION;
#define BBPCOMM_INT_CLEAR_LTE_RCV_GM_int_clear_lte_rcv_gm_START  (0)
#define BBPCOMM_INT_CLEAR_LTE_RCV_GM_int_clear_lte_rcv_gm_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_INT_CLEAR_LTE_RCV_GS_UNION
 结构说明  : INT_CLEAR_LTE_RCV_GS 寄存器结构定义。地址偏移量:0x0750，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      int_clear_lte_rcv_gs_reg;
    struct
    {
        unsigned long  int_clear_lte_rcv_gs : 16; /* bit[0-15] : 逻辑自清零。
                                                                 中断标志清除指示，高有效。用于清除INT_TYPE_LTE_RCV_GS[15:0]对应bit位
                                                                 生效时刻：立即生效 */
        unsigned long  reserved             : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_INT_CLEAR_LTE_RCV_GS_UNION;
#define BBPCOMM_INT_CLEAR_LTE_RCV_GS_int_clear_lte_rcv_gs_START  (0)
#define BBPCOMM_INT_CLEAR_LTE_RCV_GS_int_clear_lte_rcv_gs_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_INT_CLEAR_LTE_RCV_W_UNION
 结构说明  : INT_CLEAR_LTE_RCV_W 寄存器结构定义。地址偏移量:0x0754，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      int_clear_lte_rcv_w_reg;
    struct
    {
        unsigned long  int_clear_lte_rcv_w : 16; /* bit[0-15] : 逻辑自清零。
                                                                中断标志清除指示，高有效。用于清除INT_TYPE_LTE_RCV_W[15:0]对应bit位
                                                                生效时刻：立即生效 */
        unsigned long  reserved            : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_INT_CLEAR_LTE_RCV_W_UNION;
#define BBPCOMM_INT_CLEAR_LTE_RCV_W_int_clear_lte_rcv_w_START  (0)
#define BBPCOMM_INT_CLEAR_LTE_RCV_W_int_clear_lte_rcv_w_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_INT_CLEAR_LTE_INT012_UNION
 结构说明  : INT_CLEAR_LTE_INT012 寄存器结构定义。地址偏移量:0x0758，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      int_clear_lte_int012_reg;
    struct
    {
        unsigned long  int_clear_lte_int012 : 3;  /* bit[0-2] : 逻辑自清零。
                                                                中断标志清除指示，高有效。用于清除INT_TYPE_LTE_INT012[2:0]对应bit位
                                                                生效时刻：立即生效 */
        unsigned long  reserved             : 29; /* bit[3-31]: 保留 */
    } reg;
} BBPCOMM_INT_CLEAR_LTE_INT012_UNION;
#define BBPCOMM_INT_CLEAR_LTE_INT012_int_clear_lte_int012_START  (0)
#define BBPCOMM_INT_CLEAR_LTE_INT012_int_clear_lte_int012_END    (2)


/*****************************************************************************
 结构名    : BBPCOMM_INT_MASK_LTE_RCV_LTE_UNION
 结构说明  : INT_MASK_LTE_RCV_LTE 寄存器结构定义。地址偏移量:0x075C，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      int_mask_lte_rcv_lte_reg;
    struct
    {
        unsigned long  int_mask_lte_rcv_lte : 16; /* bit[0-15] : 中断屏蔽标志。用于屏蔽INT_TYPE_LTE_RCV_LTE[15:0]对应bit中断。
                                                                 0：屏蔽中断上报；
                                                                 1：使能中断上报。
                                                                 配置时刻：初始化
                                                                 生效时刻：立即生效 */
        unsigned long  reserved             : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_INT_MASK_LTE_RCV_LTE_UNION;
#define BBPCOMM_INT_MASK_LTE_RCV_LTE_int_mask_lte_rcv_lte_START  (0)
#define BBPCOMM_INT_MASK_LTE_RCV_LTE_int_mask_lte_rcv_lte_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_INT_MASK_LTE_RCV_TDS_UNION
 结构说明  : INT_MASK_LTE_RCV_TDS 寄存器结构定义。地址偏移量:0x0760，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      int_mask_lte_rcv_tds_reg;
    struct
    {
        unsigned long  int_mask_lte_rcv_tds : 16; /* bit[0-15] : 中断屏蔽标志。用于屏蔽INT_TYPE_LTE_RCV_TDS[15:0]对应bit中断。
                                                                 0：屏蔽中断上报；
                                                                 1：使能中断上报。
                                                                 配置时刻：初始化
                                                                 生效时刻：立即生效 */
        unsigned long  reserved             : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_INT_MASK_LTE_RCV_TDS_UNION;
#define BBPCOMM_INT_MASK_LTE_RCV_TDS_int_mask_lte_rcv_tds_START  (0)
#define BBPCOMM_INT_MASK_LTE_RCV_TDS_int_mask_lte_rcv_tds_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_INT_MASK_LTE_RCV_GM_UNION
 结构说明  : INT_MASK_LTE_RCV_GM 寄存器结构定义。地址偏移量:0x0764，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      int_mask_lte_rcv_gm_reg;
    struct
    {
        unsigned long  int_mask_lte_rcv_gm : 16; /* bit[0-15] : 中断屏蔽标志。用于屏蔽INT_TYPE_LTE_RCV_GM[15:0]对应bit中断。
                                                                0：屏蔽中断上报；
                                                                1：使能中断上报。
                                                                配置时刻：初始化
                                                                生效时刻：立即生效 */
        unsigned long  reserved            : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_INT_MASK_LTE_RCV_GM_UNION;
#define BBPCOMM_INT_MASK_LTE_RCV_GM_int_mask_lte_rcv_gm_START  (0)
#define BBPCOMM_INT_MASK_LTE_RCV_GM_int_mask_lte_rcv_gm_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_INT_MASK_LTE_RCV_GS_UNION
 结构说明  : INT_MASK_LTE_RCV_GS 寄存器结构定义。地址偏移量:0x0768，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      int_mask_lte_rcv_gs_reg;
    struct
    {
        unsigned long  int_mask_lte_rcv_gs : 16; /* bit[0-15] : 中断屏蔽标志。用于屏蔽INT_TYPE_LTE_RCV_GS[15:0]对应bit中断。
                                                                0：屏蔽中断上报；
                                                                1：使能中断上报。
                                                                配置时刻：初始化
                                                                生效时刻：立即生效 */
        unsigned long  reserved            : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_INT_MASK_LTE_RCV_GS_UNION;
#define BBPCOMM_INT_MASK_LTE_RCV_GS_int_mask_lte_rcv_gs_START  (0)
#define BBPCOMM_INT_MASK_LTE_RCV_GS_int_mask_lte_rcv_gs_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_INT_MASK_LTE_RCV_W_UNION
 结构说明  : INT_MASK_LTE_RCV_W 寄存器结构定义。地址偏移量:0x076C，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      int_mask_lte_rcv_w_reg;
    struct
    {
        unsigned long  int_mask_lte_rcv_w : 16; /* bit[0-15] : 中断屏蔽标志。用于屏蔽INT_TYPE_LTE_RCV_W[15:0]对应bit中断。
                                                               0：屏蔽中断上报；
                                                               1：使能中断上报。
                                                               配置时刻：初始化
                                                               生效时刻：立即生效 */
        unsigned long  reserved           : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_INT_MASK_LTE_RCV_W_UNION;
#define BBPCOMM_INT_MASK_LTE_RCV_W_int_mask_lte_rcv_w_START  (0)
#define BBPCOMM_INT_MASK_LTE_RCV_W_int_mask_lte_rcv_w_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_INT_MASK_LTE_INT012_UNION
 结构说明  : INT_MASK_LTE_INT012 寄存器结构定义。地址偏移量:0x0770，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      int_mask_lte_int012_reg;
    struct
    {
        unsigned long  int_mask_lte_int012 : 3;  /* bit[0-2] : 中断屏蔽标志。用于屏蔽INT_TYPE_LTE_INT012[2:0]对应bit中断。
                                                               0：屏蔽中断上报；
                                                               1：使能中断上报。
                                                               配置时刻：初始化
                                                               生效时刻：立即生效 */
        unsigned long  reserved            : 29; /* bit[3-31]: 保留 */
    } reg;
} BBPCOMM_INT_MASK_LTE_INT012_UNION;
#define BBPCOMM_INT_MASK_LTE_INT012_int_mask_lte_int012_START  (0)
#define BBPCOMM_INT_MASK_LTE_INT012_int_mask_lte_int012_END    (2)


/*****************************************************************************
 结构名    : BBPCOMM_INT_TYPE_LTE_RCV_W_UNION
 结构说明  : INT_TYPE_LTE_RCV_W 寄存器结构定义。地址偏移量:0x0774，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      int_type_lte_rcv_w_reg;
    struct
    {
        unsigned long  o_int_type_lte_rcv_w : 16; /* bit[0-15] : 中断状态标志。各bit单独对应一种中断类型，高有效。
                                                                 [0]: 中断类型0。 W（主模）提前通知LTE异系统测量启动中断。
                                                                 [1]: 中断类型1。 W（主模）进GAP中断，通知LTE进行异系统测量射频通道配置，天线切换等操作。
                                                                 [2]: 中断类型2。 W（主模）出GAP中断。
                                                                 [3]: 中断类型3。W（从模）测量结束中断。
                                                                 [4]: 中断类型4。W（主模）强制LTE退出异系统测量中断。
                                                                 [5]: 中断类型5。W（主模）通知LTE上报本次测量结果。
                                                                 [6]: 保留。
                                                                 [15:7]: 对应中断类型[15:7]，预留。
                                                                 软件读取该中断状态上报寄存器后，需要和0x076C的int_mask_lte_rcv_w相与，再判断是否需要处理。 */
        unsigned long  reserved             : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_INT_TYPE_LTE_RCV_W_UNION;
#define BBPCOMM_INT_TYPE_LTE_RCV_W_o_int_type_lte_rcv_w_START  (0)
#define BBPCOMM_INT_TYPE_LTE_RCV_W_o_int_type_lte_rcv_w_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_INT_TYPE_LTE_RCV_TDS_UNION
 结构说明  : INT_TYPE_LTE_RCV_TDS 寄存器结构定义。地址偏移量:0x0778，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      int_type_lte_rcv_tds_reg;
    struct
    {
        unsigned long  o_int_type_lte_rcv_tds : 16; /* bit[0-15] : 中断状态标志0，1，2只有当TDS处于主模时才能产生。
                                                                   [0]: 中断类型0。 TDS（主模）提前通知LTE异系统测量启动中断。
                                                                   [1]: 中断类型1。 TDS（主模）进GAP中断，通知LTE进行异系统测量射频通道配置，天线切换等操作。
                                                                   [2]: 中断类型2。 TDS（主模）出GAP中断。
                                                                   [3]: 中断类型3。TDS（从模）测量结束中断。
                                                                   [4]: 中断类型4。TDS（主模）强制LTE退出异系统测量中断。
                                                                   [5]: 中断类型5。TDS（主模）通知LTE上报本次测量结果。
                                                                   [6]: 中断类型6。保留。
                                                                   [15:7]: 对应中断类型[15:7]，预留。
                                                                   软件读取该中断状态上报寄存器后，需要和0x0760的int_mask_lte_rcv_tds相与，再判断是否需要处理。 */
        unsigned long  reserved               : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_INT_TYPE_LTE_RCV_TDS_UNION;
#define BBPCOMM_INT_TYPE_LTE_RCV_TDS_o_int_type_lte_rcv_tds_START  (0)
#define BBPCOMM_INT_TYPE_LTE_RCV_TDS_o_int_type_lte_rcv_tds_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_INT_TYPE_LTE_RCV_GM_UNION
 结构说明  : INT_TYPE_LTE_RCV_GM 寄存器结构定义。地址偏移量:0x077C，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      int_type_lte_rcv_gm_reg;
    struct
    {
        unsigned long  o_int_type_lte_rcv_gm : 16; /* bit[0-15] : 中断状态标志。各bit单独对应一种中断类型，高有效。
                                                                  [0]: 中断类型0。GM（主模）提前通知LTE异系统测量启动中断。
                                                                  [1]: 中断类型1。GM（主模）进GAP中断，通知LTE进行异系统测量射频通道配置，天线切换等操作。
                                                                  [2]: 中断类型2。GM（主模）出GAP中断。
                                                                  [3]: 中断类型3。GM（从模）测量结束中断。
                                                                  [4]: 中断类型4。GM（主模）强制LTE退出异系统测量中断。
                                                                  [5]: 中断类型5。GM（主模）通知LTE上报本次测量结果。
                                                                  [6]: 中断类型6。保留。
                                                                  [15:7]: 对应中断类型[15:7]，预留。
                                                                  中断状态标志0，1，2只有当GM处于主模时才能产生。
                                                                  软件读取该中断状态上报寄存器后，需要和0x0764的int_mask_lte_rcv_gm相与，再判断是否需要处理。 */
        unsigned long  reserved              : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_INT_TYPE_LTE_RCV_GM_UNION;
#define BBPCOMM_INT_TYPE_LTE_RCV_GM_o_int_type_lte_rcv_gm_START  (0)
#define BBPCOMM_INT_TYPE_LTE_RCV_GM_o_int_type_lte_rcv_gm_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_INT_TYPE_LTE_RCV_GS_UNION
 结构说明  : INT_TYPE_LTE_RCV_GS 寄存器结构定义。地址偏移量:0x0780，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      int_type_lte_rcv_gs_reg;
    struct
    {
        unsigned long  o_int_type_lte_rcv_gs : 16; /* bit[0-15] : [0]: 中断类型0。GS（主模）提前通知LTE异系统测量启动中断。
                                                                  [1]: 中断类型1。GS（主模）进GAP中断，通知LTE进行异系统测量射频通道配置，天线切换等操作。
                                                                  [2]: 中断类型2。GS（主模）出GAP中断。
                                                                  [3]: 中断类型3。GS（从模）测量结束中断。
                                                                  [4]: 中断类型4。GS（主模）强制LTE退出异系统测量中断。
                                                                  [5]: 中断类型5。GS（主模）通知LTE上报本次测量结果。
                                                                  [6]: 中断类型6。保留。
                                                                  [15:7]: 对应中断类型[15:7]，预留。
                                                                  中断状态标志0，1，2只有当TDS处于主模时才能产生。
                                                                  软件读取该中断状态上报寄存器后，需要和0x0768的int_mask_lte_rcv_gs相与，再判断是否需要处理。 */
        unsigned long  reserved              : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_INT_TYPE_LTE_RCV_GS_UNION;
#define BBPCOMM_INT_TYPE_LTE_RCV_GS_o_int_type_lte_rcv_gs_START  (0)
#define BBPCOMM_INT_TYPE_LTE_RCV_GS_o_int_type_lte_rcv_gs_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_INT_TYPE_LTE_RCV_LTE_UNION
 结构说明  : INT_TYPE_LTE_RCV_LTE 寄存器结构定义。地址偏移量:0x0784，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      int_type_lte_rcv_lte_reg;
    struct
    {
        unsigned long  o_int_type_lte_rcv_lte : 16; /* bit[0-15] : 中断状态标志。各bit单独对应一种中断类型，高有效。
                                                                   [0]: 中断类型0。LTE（主模）提前通知LTE异频测量启动中断。
                                                                   [1]: 中断类型1。LTE（主模）进GAP中断，通知LTE进行异频测量射频通道配置，天线切换等操作。
                                                                   [2]: 中断类型2。LTE（主模）出GAP中断。
                                                                   [3]: 中断类型3。LTE（从模，即异频）测量结束中断。
                                                                   [4]: 中断类型4。LTE（主模）强制LTE退出异频测量中断。
                                                                   [5]: 中断类型5。LTE（主模）通知LTE上报本次测量结果。
                                                                   [6]: 中断类型6。保留。
                                                                   [15:7]: 对应中断类型[15:7]，预留。
                                                                   中断状态标志0，1，2只有当LTE处于主模时才能产生。
                                                                   软件读取该中断状态上报寄存器后，需要和0x075C的int_mask_lte_rcv_lte相与，再判断是否需要处理。 */
        unsigned long  reserved               : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_INT_TYPE_LTE_RCV_LTE_UNION;
#define BBPCOMM_INT_TYPE_LTE_RCV_LTE_o_int_type_lte_rcv_lte_START  (0)
#define BBPCOMM_INT_TYPE_LTE_RCV_LTE_o_int_type_lte_rcv_lte_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_INT_TYPE_LTE_INT012_UNION
 结构说明  : INT_TYPE_LTE_INT012 寄存器结构定义。地址偏移量:0x0788，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      int_type_lte_int012_reg;
    struct
    {
        unsigned long  o_int_type_lte_int012 : 3;  /* bit[0-2] : 增加主模产生int0、1、2的上报（(包括异频、异系统场景)），高有效。
                                                                 [0]: L主模时，CTU告诉通知LDSP产生int0中断。
                                                                 [1]: L主模时，CTU告诉通知LDSP产生int1中断。
                                                                 [2]: L主模时，CTU告诉通知LDSP产生int2中断。
                                                                 软件读取该中断状态上报寄存器后，需要和0x0770的int_mask_lte_int012相与，再判断是否需要处理。 */
        unsigned long  reserved              : 29; /* bit[3-31]: 保留 */
    } reg;
} BBPCOMM_INT_TYPE_LTE_INT012_UNION;
#define BBPCOMM_INT_TYPE_LTE_INT012_o_int_type_lte_int012_START  (0)
#define BBPCOMM_INT_TYPE_LTE_INT012_o_int_type_lte_int012_END    (2)


/*****************************************************************************
 结构名    : BBPCOMM_LTE_SND_LTE_INT_13_UNION
 结构说明  : LTE_SND_LTE_INT_13 寄存器结构定义。地址偏移量:0x078C，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      lte_snd_lte_int_13_reg;
    struct
    {
        unsigned long  reserved_0         : 3;  /* bit[0-2]  : 保留 */
        unsigned long  lte_snd_lte_int_13 : 13; /* bit[3-15] : 逻辑自清零。高有效。
                                                               
                                                               bit15~7：对应lte_snd_lte_int_15 ~ 7。
                                                               L DSP通过此寄存器触发类型15~7中断（int_type_lte_rcv_lte[15:7]）给LDSP。
                                                               生效时刻：立即生效
                                                               
                                                               bit6：对应lte_snd_lte_int_6。
                                                               LDSP通过此寄存器触发类型6中断（int_type_lte_rcv_lte[6]）给LDSP。
                                                               生效时刻：立即生效
                                                               
                                                               bit5：对应lte_snd_lte_int_5。
                                                               L主模时通知LDSP L测量周期到达，高有效。
                                                               L DSP通过此寄存器触发类型5中断（int_type_lte_rcv_lte[5]）给LDSP。
                                                               生效时刻：立即生效
                                                               
                                                               bit4：对应lte_snd_lte_int_4。
                                                               L主模时强制退出异系统测量指示。高有效。
                                                               L DSP通过此寄存器触发类型4中断（int_type_lte_rcv_lte[4]）给LDSP。
                                                               生效时刻：立即生效
                                                               
                                                               bit3：对应lte_snd_lte_int_3。L测量结束指示，高有效。
                                                               用于触发中断类型3 （int_type_lte_rcv_lte[3]）给LDSP。L主模下对L进行异系统测量时，如果L测量结束，L DSP配置此标志为1。 
                                                               生效时刻：立即生效 */
        unsigned long  reserved_1         : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_LTE_SND_LTE_INT_13_UNION;
#define BBPCOMM_LTE_SND_LTE_INT_13_lte_snd_lte_int_13_START  (3)
#define BBPCOMM_LTE_SND_LTE_INT_13_lte_snd_lte_int_13_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_LTE_SND_TDS_INT_13_UNION
 结构说明  : LTE_SND_TDS_INT_13 寄存器结构定义。地址偏移量:0x0790，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      lte_snd_tds_int_13_reg;
    struct
    {
        unsigned long  reserved_0         : 3;  /* bit[0-2]  : 保留 */
        unsigned long  lte_snd_tds_int_13 : 13; /* bit[3-15] : 逻辑自清零。高有效。
                                                               
                                                               bit15~7：对应lte_snd_tds_int_15 ~ 7。
                                                               L DSP通过此寄存器触发类型15~7中断（int_type_tds_rcv_lte[15:7]）给TDSDSP。
                                                               生效时刻：立即生效
                                                               
                                                               bit6：对应lte_snd_tds_int_6。
                                                               LDSP通过此寄存器触发类型6中断（int_type_tds_rcv_lte[6]）给TDSDSP。
                                                               生效时刻：立即生效
                                                               
                                                               bit5：对应lte_snd_tds_int_5。
                                                               L主模时通知TDSDSP TDS测量周期到达，高有效。
                                                               L DSP通过此寄存器触发类型5中断（int_type_tds_rcv_lte[5]）给TDSDSP。
                                                               生效时刻：立即生效
                                                               
                                                               bit4：对应lte_snd_tds_int_4。
                                                               L主模时强制退出异系统测量指示。高有效。
                                                               L DSP通过此寄存器触发类型4中断（int_type_tds_rcv_lte[4]）给TDSDSP。
                                                               生效时刻：立即生效
                                                               
                                                               bit3：对应lte_snd_tds_int_3。L测量结束指示，高有效。
                                                               用于触发中断类型3 （int_type_tds_rcv_lte[3]）给TDSDSP。TDS主模下对L进行异系统测量时，如果L测量结束，L DSP配置此标志为1。 
                                                               生效时刻：立即生效 */
        unsigned long  reserved_1         : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_LTE_SND_TDS_INT_13_UNION;
#define BBPCOMM_LTE_SND_TDS_INT_13_lte_snd_tds_int_13_START  (3)
#define BBPCOMM_LTE_SND_TDS_INT_13_lte_snd_tds_int_13_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_LTE_SND_GM_INT_13_UNION
 结构说明  : LTE_SND_GM_INT_13 寄存器结构定义。地址偏移量:0x0794，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      lte_snd_gm_int_13_reg;
    struct
    {
        unsigned long  reserved_0        : 3;  /* bit[0-2]  : 保留 */
        unsigned long  lte_snd_gm_int_13 : 13; /* bit[3-15] : 逻辑自清零。高有效。
                                                              
                                                              bit15~7：对应lte_snd_gm_int_15 ~ 7。
                                                              L DSP通过此寄存器触发类型15~7中断（int_type_gm_rcv_lte[15:7]）给GMDSP。
                                                              生效时刻：立即生效
                                                              
                                                              bit6：对应lte_snd_gm_int_6。
                                                              LDSP通过此寄存器触发类型6中断（iint_type_gm_rcv_lte[6]）给GMDSP。
                                                              生效时刻：立即生效
                                                              
                                                              bit5：对应lte_snd_gm_int_5。
                                                              L主模时通知GMDSP GM测量周期到达，高有效。
                                                              L DSP通过此寄存器触发类型5中断（int_type_gm_rcv_lte[5]）给GMDSP。
                                                              生效时刻：立即生效
                                                              
                                                              bit4：对应lte_snd_gm_int_4。
                                                              L主模时强制退出异系统测量指示。高有效。
                                                              L DSP通过此寄存器触发类型4中断（int_type_gm_rcv_lte[4]）给GMDSP。
                                                              生效时刻：立即生效
                                                              
                                                              bit3：对应lte_snd_gm_int_3。L测量结束指示，高有效。
                                                              用于触发中断类型3 （int_type_gm_rcv_lte[3]）给GMDSP。GM主模下对L进行异系统测量时，如果L测量结束，L DSP配置此标志为1。 
                                                              生效时刻：立即生效 */
        unsigned long  reserved_1        : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_LTE_SND_GM_INT_13_UNION;
#define BBPCOMM_LTE_SND_GM_INT_13_lte_snd_gm_int_13_START  (3)
#define BBPCOMM_LTE_SND_GM_INT_13_lte_snd_gm_int_13_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_LTE_SND_GS_INT_13_UNION
 结构说明  : LTE_SND_GS_INT_13 寄存器结构定义。地址偏移量:0x0798，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      lte_snd_gs_int_13_reg;
    struct
    {
        unsigned long  reserved_0        : 3;  /* bit[0-2]  : 保留 */
        unsigned long  lte_snd_gs_int_13 : 13; /* bit[3-15] : 逻辑自清零。高有效。
                                                              
                                                              bit15~7：对应lte_snd_gs_int_15 ~ 7。
                                                              L DSP通过此寄存器触发类型15~7中断（int_type_gs_rcv_lte[15:7]）给GSDSP。
                                                              生效时刻：立即生效
                                                              
                                                              bit6：对应lte_snd_gs_int_6。
                                                              LDSP通过此寄存器触发类型6中断（int_type_gs_rcv_lte[6]）给GSDSP。
                                                              生效时刻：立即生效
                                                              
                                                              bit5：对应lte_snd_gs_int_5。
                                                              L主模时通知GSDSP GS测量周期到达，高有效。
                                                              L DSP通过此寄存器触发类型5中断（int_type_gs_rcv_lte[5]）给GSDSP。
                                                              生效时刻：立即生效
                                                              
                                                              bit4：对应lte_snd_gs_int_4。
                                                              L主模时强制退出异系统测量指示。高有效。
                                                              L DSP通过此寄存器触发类型4中断（int_type_gs_rcv_lte[4]）给GSDSP。
                                                              生效时刻：立即生效
                                                              
                                                              bit3：对应lte_snd_gs_int_3。L测量结束指示，高有效。
                                                              用于触发中断类型3 （int_type_gs_rcv_lte[3]）给GSDSP。GS主模下对L进行异系统测量时，如果L测量结束，L DSP配置此标志为1。 
                                                              生效时刻：立即生效 */
        unsigned long  reserved_1        : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_LTE_SND_GS_INT_13_UNION;
#define BBPCOMM_LTE_SND_GS_INT_13_lte_snd_gs_int_13_START  (3)
#define BBPCOMM_LTE_SND_GS_INT_13_lte_snd_gs_int_13_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_LTE_SND_W_INT_13_UNION
 结构说明  : LTE_SND_W_INT_13 寄存器结构定义。地址偏移量:0x079C，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      lte_snd_w_int_13_reg;
    struct
    {
        unsigned long  reserved_0       : 3;  /* bit[0-2]  : 保留 */
        unsigned long  lte_snd_w_int_13 : 13; /* bit[3-15] : 逻辑自清零。高有效。
                                                             
                                                             bit15~7：对应lte_snd_w_int_15 ~ 7。
                                                             L DSP通过此寄存器触发类型15~7中断（int_type_w_rcv_lte[15:7]）给WDSP。
                                                             生效时刻：立即生效
                                                             
                                                             bit6：对应lte_snd_w_int_6。
                                                             LDSP通过此寄存器触发类型6中断（int_type_w_rcv_lte[6]）给WDSP。
                                                             生效时刻：立即生效
                                                             
                                                             bit5：对应lte_snd_w_int_5。
                                                             L主模时通知WDSP W测量周期到达，高有效。
                                                             L DSP通过此寄存器触发类型5中断（int_type_w_rcv_lte[5]）给WDSP。
                                                             生效时刻：立即生效
                                                             
                                                             bit4：对应lte_snd_w_int_4。
                                                             L主模时强制退出异系统测量指示。高有效。
                                                             L DSP通过此寄存器触发类型4中断（int_type_w_rcv_lte[4]）给WDSP。
                                                             生效时刻：立即生效
                                                             
                                                             bit3：对应lte_snd_w_int_3。L测量结束指示，高有效。
                                                             用于触发中断类型3 （int_type_w_rcv_lte[3]）给WDSP。W主模下对L进行异系统测量时，如果L测量结束，L DSP配置此标志为1。 
                                                             生效时刻：立即生效 */
        unsigned long  reserved_1       : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_LTE_SND_W_INT_13_UNION;
#define BBPCOMM_LTE_SND_W_INT_13_lte_snd_w_int_13_START  (3)
#define BBPCOMM_LTE_SND_W_INT_13_lte_snd_w_int_13_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_TDS_INTERSYS_MEASURE_TYPE_UNION
 结构说明  : TDS_INTERSYS_MEASURE_TYPE 寄存器结构定义。地址偏移量:0x07A0，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      tds_intersys_measure_type_reg;
    struct
    {
        unsigned long  tds_intersys_measure_type : 4;  /* bit[0-3] : 异系统测量类型 。
                                                                     4’b0： 测量TDS
                                                                     4’b1： 背景搜TDS
                                                                     其它预留
                                                                     
                                                                     生效时刻：立即生效 */
        unsigned long  reserved                  : 28; /* bit[4-31]: 保留 */
    } reg;
} BBPCOMM_TDS_INTERSYS_MEASURE_TYPE_UNION;
#define BBPCOMM_TDS_INTERSYS_MEASURE_TYPE_tds_intersys_measure_type_START  (0)
#define BBPCOMM_TDS_INTERSYS_MEASURE_TYPE_tds_intersys_measure_type_END    (3)


/*****************************************************************************
 结构名    : BBPCOMM_TDS_MEASURE_REPORT_VALID_UNION
 结构说明  : TDS_MEASURE_REPORT_VALID 寄存器结构定义。地址偏移量:0x07A4，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      tds_measure_report_valid_reg;
    struct
    {
        unsigned long  tds_measure_report_valid : 1;  /* bit[0]   : 单频点测量周期到指示，U/Gm/Gs或者L作为主模，通知T DSP W测量的单频点周期到达。
                                                                    只在连接态下需要配置，且G测TDS时不需要配置；
                                                                    电平信号，高有效。
                                                                    
                                                                    生效时刻：立即生效 */
        unsigned long  reserved                 : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_TDS_MEASURE_REPORT_VALID_UNION;
#define BBPCOMM_TDS_MEASURE_REPORT_VALID_tds_measure_report_valid_START  (0)
#define BBPCOMM_TDS_MEASURE_REPORT_VALID_tds_measure_report_valid_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_INT_CLEAR_TDS_RCV_LTE_UNION
 结构说明  : INT_CLEAR_TDS_RCV_LTE 寄存器结构定义。地址偏移量:0x07B0，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      int_clear_tds_rcv_lte_reg;
    struct
    {
        unsigned long  int_clear_tds_rcv_lte : 16; /* bit[0-15] : 逻辑自清零。
                                                                  中断标志清除指示，高有效。用于清除INT_TYPE_TDS_RCV_LTE[15:0]对应bit位
                                                                  生效时刻：立即生效 */
        unsigned long  reserved              : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_INT_CLEAR_TDS_RCV_LTE_UNION;
#define BBPCOMM_INT_CLEAR_TDS_RCV_LTE_int_clear_tds_rcv_lte_START  (0)
#define BBPCOMM_INT_CLEAR_TDS_RCV_LTE_int_clear_tds_rcv_lte_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_INT_CLEAR_TDS_RCV_TDS_UNION
 结构说明  : INT_CLEAR_TDS_RCV_TDS 寄存器结构定义。地址偏移量:0x07B4，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      int_clear_tds_rcv_tds_reg;
    struct
    {
        unsigned long  int_clear_tds_rcv_tds : 16; /* bit[0-15] : 逻辑自清零。
                                                                  中断标志清除指示，高有效。用于清除INT_TYPE_TDS_RCV_TDS[15:0]对应bit位
                                                                  生效时刻：立即生效 */
        unsigned long  reserved              : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_INT_CLEAR_TDS_RCV_TDS_UNION;
#define BBPCOMM_INT_CLEAR_TDS_RCV_TDS_int_clear_tds_rcv_tds_START  (0)
#define BBPCOMM_INT_CLEAR_TDS_RCV_TDS_int_clear_tds_rcv_tds_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_INT_CLEAR_TDS_RCV_GM_UNION
 结构说明  : INT_CLEAR_TDS_RCV_GM 寄存器结构定义。地址偏移量:0x07B8，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      int_clear_tds_rcv_gm_reg;
    struct
    {
        unsigned long  int_clear_tds_rcv_gm : 16; /* bit[0-15] : 逻辑自清零。
                                                                 中断标志清除指示，高有效。用于清除INT_TYPE_TDS_RCV_GM[15:0]对应bit位
                                                                 生效时刻：立即生效 */
        unsigned long  reserved             : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_INT_CLEAR_TDS_RCV_GM_UNION;
#define BBPCOMM_INT_CLEAR_TDS_RCV_GM_int_clear_tds_rcv_gm_START  (0)
#define BBPCOMM_INT_CLEAR_TDS_RCV_GM_int_clear_tds_rcv_gm_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_INT_CLEAR_TDS_RCV_GS_UNION
 结构说明  : INT_CLEAR_TDS_RCV_GS 寄存器结构定义。地址偏移量:0x07BC，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      int_clear_tds_rcv_gs_reg;
    struct
    {
        unsigned long  int_clear_tds_rcv_gs : 16; /* bit[0-15] : 逻辑自清零。
                                                                 中断标志清除指示，高有效。用于清除INT_TYPE_TDS_RCV_GS[15:0]对应bit位
                                                                 生效时刻：立即生效 */
        unsigned long  reserved             : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_INT_CLEAR_TDS_RCV_GS_UNION;
#define BBPCOMM_INT_CLEAR_TDS_RCV_GS_int_clear_tds_rcv_gs_START  (0)
#define BBPCOMM_INT_CLEAR_TDS_RCV_GS_int_clear_tds_rcv_gs_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_INT_CLEAR_TDS_RCV_W_UNION
 结构说明  : INT_CLEAR_TDS_RCV_W 寄存器结构定义。地址偏移量:0x07C0，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      int_clear_tds_rcv_w_reg;
    struct
    {
        unsigned long  int_clear_tds_rcv_w : 16; /* bit[0-15] : 逻辑自清零。
                                                                中断标志清除指示，高有效。用于清除INT_TYPE_TDS_RCV_W[15:0]对应bit位
                                                                生效时刻：立即生效 */
        unsigned long  reserved            : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_INT_CLEAR_TDS_RCV_W_UNION;
#define BBPCOMM_INT_CLEAR_TDS_RCV_W_int_clear_tds_rcv_w_START  (0)
#define BBPCOMM_INT_CLEAR_TDS_RCV_W_int_clear_tds_rcv_w_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_INT_CLEAR_TDS_INT012_UNION
 结构说明  : INT_CLEAR_TDS_INT012 寄存器结构定义。地址偏移量:0x07C4，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      int_clear_tds_int012_reg;
    struct
    {
        unsigned long  int_clear_tds_int012 : 3;  /* bit[0-2] : 逻辑自清零。
                                                                中断标志清除指示，高有效。用于清除INT_TYPE_TDS_INT012[2:0]对应bit位
                                                                生效时刻：立即生效 */
        unsigned long  reserved             : 29; /* bit[3-31]: 保留 */
    } reg;
} BBPCOMM_INT_CLEAR_TDS_INT012_UNION;
#define BBPCOMM_INT_CLEAR_TDS_INT012_int_clear_tds_int012_START  (0)
#define BBPCOMM_INT_CLEAR_TDS_INT012_int_clear_tds_int012_END    (2)


/*****************************************************************************
 结构名    : BBPCOMM_INT_MASK_TDS_RCV_LTE_UNION
 结构说明  : INT_MASK_TDS_RCV_LTE 寄存器结构定义。地址偏移量:0x07C8，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      int_mask_tds_rcv_lte_reg;
    struct
    {
        unsigned long  int_mask_tds_rcv_lte : 16; /* bit[0-15] : 中断屏蔽标志。用于屏蔽INT_TYPE_TDS_RCV_LTE[15:0]对应bit中断。
                                                                 0：屏蔽中断上报；
                                                                 1：使能中断上报。
                                                                 配置时刻：初始化
                                                                 生效时刻：立即生效 */
        unsigned long  reserved             : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_INT_MASK_TDS_RCV_LTE_UNION;
#define BBPCOMM_INT_MASK_TDS_RCV_LTE_int_mask_tds_rcv_lte_START  (0)
#define BBPCOMM_INT_MASK_TDS_RCV_LTE_int_mask_tds_rcv_lte_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_INT_MASK_TDS_RCV_TDS_UNION
 结构说明  : INT_MASK_TDS_RCV_TDS 寄存器结构定义。地址偏移量:0x07CC，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      int_mask_tds_rcv_tds_reg;
    struct
    {
        unsigned long  int_mask_tds_rcv_tds : 16; /* bit[0-15] : 中断屏蔽标志。用于屏蔽INT_TYPE_TDS_RCV_TDS[15:0]对应bit中断。
                                                                 0：屏蔽中断上报；
                                                                 1：使能中断上报。
                                                                 配置时刻：初始化
                                                                 生效时刻：立即生效 */
        unsigned long  reserved             : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_INT_MASK_TDS_RCV_TDS_UNION;
#define BBPCOMM_INT_MASK_TDS_RCV_TDS_int_mask_tds_rcv_tds_START  (0)
#define BBPCOMM_INT_MASK_TDS_RCV_TDS_int_mask_tds_rcv_tds_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_INT_MASK_TDS_RCV_GM_UNION
 结构说明  : INT_MASK_TDS_RCV_GM 寄存器结构定义。地址偏移量:0x07D0，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      int_mask_tds_rcv_gm_reg;
    struct
    {
        unsigned long  int_mask_tds_rcv_gm : 16; /* bit[0-15] : 中断屏蔽标志。用于屏蔽INT_TYPE_TDS_RCV_GM[15:0]对应bit中断。
                                                                0：屏蔽中断上报；
                                                                1：使能中断上报。
                                                                配置时刻：初始化
                                                                生效时刻：立即生效 */
        unsigned long  reserved            : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_INT_MASK_TDS_RCV_GM_UNION;
#define BBPCOMM_INT_MASK_TDS_RCV_GM_int_mask_tds_rcv_gm_START  (0)
#define BBPCOMM_INT_MASK_TDS_RCV_GM_int_mask_tds_rcv_gm_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_INT_MASK_TDS_RCV_GS_UNION
 结构说明  : INT_MASK_TDS_RCV_GS 寄存器结构定义。地址偏移量:0x07D4，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      int_mask_tds_rcv_gs_reg;
    struct
    {
        unsigned long  int_mask_tds_rcv_gs : 16; /* bit[0-15] : 中断屏蔽标志。用于屏蔽INT_TYPE_TDS_RCV_GS[15:0]对应bit中断。
                                                                0：屏蔽中断上报；
                                                                1：使能中断上报。
                                                                配置时刻：初始化
                                                                生效时刻：立即生效 */
        unsigned long  reserved            : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_INT_MASK_TDS_RCV_GS_UNION;
#define BBPCOMM_INT_MASK_TDS_RCV_GS_int_mask_tds_rcv_gs_START  (0)
#define BBPCOMM_INT_MASK_TDS_RCV_GS_int_mask_tds_rcv_gs_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_INT_MASK_TDS_RCV_W_UNION
 结构说明  : INT_MASK_TDS_RCV_W 寄存器结构定义。地址偏移量:0x07D8，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      int_mask_tds_rcv_w_reg;
    struct
    {
        unsigned long  int_mask_tds_rcv_w : 16; /* bit[0-15] : 中断屏蔽标志。用于屏蔽INT_TYPE_TDS_RCV_W[15:0]对应bit中断。
                                                               0：屏蔽中断上报；
                                                               1：使能中断上报。
                                                               配置时刻：初始化
                                                               生效时刻：立即生效 */
        unsigned long  reserved           : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_INT_MASK_TDS_RCV_W_UNION;
#define BBPCOMM_INT_MASK_TDS_RCV_W_int_mask_tds_rcv_w_START  (0)
#define BBPCOMM_INT_MASK_TDS_RCV_W_int_mask_tds_rcv_w_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_INT_MASK_TDS_INT012_UNION
 结构说明  : INT_MASK_TDS_INT012 寄存器结构定义。地址偏移量:0x07DC，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      int_mask_tds_int012_reg;
    struct
    {
        unsigned long  int_mask_tds_int012 : 3;  /* bit[0-2] : 中断屏蔽标志。用于屏蔽INT_TYPE_TDS_INT012[2:0]对应bit中断。
                                                               0：屏蔽中断上报；
                                                               1：使能中断上报。
                                                               配置时刻：初始化
                                                               生效时刻：立即生效 */
        unsigned long  reserved            : 29; /* bit[3-31]: 保留 */
    } reg;
} BBPCOMM_INT_MASK_TDS_INT012_UNION;
#define BBPCOMM_INT_MASK_TDS_INT012_int_mask_tds_int012_START  (0)
#define BBPCOMM_INT_MASK_TDS_INT012_int_mask_tds_int012_END    (2)


/*****************************************************************************
 结构名    : BBPCOMM_INT_TYPE_TDS_RCV_LTE_UNION
 结构说明  : INT_TYPE_TDS_RCV_LTE 寄存器结构定义。地址偏移量:0x07E0，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      int_type_tds_rcv_lte_reg;
    struct
    {
        unsigned long  o_int_type_tds_rcv_lte : 16; /* bit[0-15] : 中断状态标志。各bit单独对应一种中断类型，高有效。
                                                                   [0]: 中断类型0。 LTE（主模）提前通知TDS异系统测量启动中断。
                                                                   [1]: 中断类型1。 LTE（主模）进GAP中断，通知TDS进行异系统测量射频通道配置，天线切换等操作。
                                                                   [2]: 中断类型2。 LTE（主模）出GAP中断。
                                                                   [3]: 中断类型3。LTE（从模）测量结束中断。
                                                                   [4]: 中断类型4。LTE（主模）强制TDS退出异系统测量中断。
                                                                   [5]: 中断类型5。LTE（主模）通知TDS上报本次测量结果。
                                                                   [6]: 保留。
                                                                   [15:7]: 对应中断类型[15:7]，预留。
                                                                   软件读取该中断状态上报寄存器后，需要和0x07C8的int_mask_tds_rcv_lte相与，再判断是否需要处理。 */
        unsigned long  reserved               : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_INT_TYPE_TDS_RCV_LTE_UNION;
#define BBPCOMM_INT_TYPE_TDS_RCV_LTE_o_int_type_tds_rcv_lte_START  (0)
#define BBPCOMM_INT_TYPE_TDS_RCV_LTE_o_int_type_tds_rcv_lte_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_INT_TYPE_TDS_RCV_W_UNION
 结构说明  : INT_TYPE_TDS_RCV_W 寄存器结构定义。地址偏移量:0x07E4，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      int_type_tds_rcv_w_reg;
    struct
    {
        unsigned long  o_int_type_tds_rcv_w : 16; /* bit[0-15] : 中断状态标志0，1，2只有当TDS处于主模时才能产生。
                                                                 
                                                                 [0]: 中断类型0。 TDS（主模）提前通知TDS异系统测量启动中断。
                                                                 [1]: 中断类型1。 TDS（主模）进GAP中断，通知TDS进行异系统测量射频通道配置，天线切换等操作。
                                                                 [2]: 中断类型2。 TDS（主模）出GAP中断。
                                                                 [3]: 中断类型3。TDS（从模）测量结束中断。
                                                                 [4]: 中断类型4。TDS（主模）强制TDS退出异系统测量中断。
                                                                 [5]: 中断类型5。TDS（主模）通知TDS上报本次测量结果。
                                                                 [6]: 中断类型6。保留。
                                                                 [15:7]: 对应中断类型[15:7]，预留。
                                                                 软件读取该中断状态上报寄存器后，需要和0x07D8的int_mask_tds_rcv_w相与，再判断是否需要处理。 */
        unsigned long  reserved             : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_INT_TYPE_TDS_RCV_W_UNION;
#define BBPCOMM_INT_TYPE_TDS_RCV_W_o_int_type_tds_rcv_w_START  (0)
#define BBPCOMM_INT_TYPE_TDS_RCV_W_o_int_type_tds_rcv_w_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_INT_TYPE_TDS_RCV_GM_UNION
 结构说明  : INT_TYPE_TDS_RCV_GM 寄存器结构定义。地址偏移量:0x07E8，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      int_type_tds_rcv_gm_reg;
    struct
    {
        unsigned long  o_int_type_tds_rcv_gm : 16; /* bit[0-15] : 中断状态标志。各bit单独对应一种中断类型，高有效。
                                                                  [0]: 中断类型0。GM（主模）提前通知TDS异系统测量启动中断。
                                                                  [1]: 中断类型1。GM（主模）进GAP中断，通知TDS进行异系统测量射频通道配置，天线切换等操作。
                                                                  [2]: 中断类型2。GM（主模）出GAP中断。
                                                                  [3]: 中断类型3。GM（从模）测量结束中断。
                                                                  [4]: 中断类型4。GM（主模）强制TDS退出异系统测量中断。
                                                                  [5]: 中断类型5。GM（主模）通知TDS上报本次测量结果。
                                                                  [6]: 中断类型6。保留。
                                                                  [15:7]: 对应中断类型[15:7]，预留。
                                                                  中断状态标志0，1，2只有当GM处于主模时才能产生。
                                                                  软件读取该中断状态上报寄存器后，需要和0x07D0的int_mask_tds_rcv_gm相与，再判断是否需要处理。 */
        unsigned long  reserved              : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_INT_TYPE_TDS_RCV_GM_UNION;
#define BBPCOMM_INT_TYPE_TDS_RCV_GM_o_int_type_tds_rcv_gm_START  (0)
#define BBPCOMM_INT_TYPE_TDS_RCV_GM_o_int_type_tds_rcv_gm_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_INT_TYPE_TDS_RCV_GS_UNION
 结构说明  : INT_TYPE_TDS_RCV_GS 寄存器结构定义。地址偏移量:0x07EC，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      int_type_tds_rcv_gs_reg;
    struct
    {
        unsigned long  o_int_type_tds_rcv_gs : 16; /* bit[0-15] : [0]: 中断类型0。GS（主模）提前通知TDS异系统测量启动中断。
                                                                  [1]: 中断类型1。GS（主模）进GAP中断，通知TDS进行异系统测量射频通道配置，天线切换等操作。
                                                                  [2]: 中断类型2。GS（主模）出GAP中断。
                                                                  [3]: 中断类型3。GS（从模）测量结束中断。
                                                                  [4]: 中断类型4。GS（主模）强制TDS退出异系统测量中断。
                                                                  [5]: 中断类型5。GS（主模）通知TDS上报本次测量结果。
                                                                  [6]: 中断类型6。保留。
                                                                  [15:7]: 对应中断类型[15:7]，预留。
                                                                  中断状态标志0，1，2只有当TDS处于主模时才能产生。
                                                                  软件读取该中断状态上报寄存器后，需要和0x07D4的int_mask_tds_rcv_gs相与，再判断是否需要处理。 */
        unsigned long  reserved              : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_INT_TYPE_TDS_RCV_GS_UNION;
#define BBPCOMM_INT_TYPE_TDS_RCV_GS_o_int_type_tds_rcv_gs_START  (0)
#define BBPCOMM_INT_TYPE_TDS_RCV_GS_o_int_type_tds_rcv_gs_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_INT_TYPE_TDS_RCV_TDS_UNION
 结构说明  : INT_TYPE_TDS_RCV_TDS 寄存器结构定义。地址偏移量:0x07F0，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      int_type_tds_rcv_tds_reg;
    struct
    {
        unsigned long  o_int_type_tds_rcv_tds : 16; /* bit[0-15] : 中断状态标志。各bit单独对应一种中断类型，高有效。
                                                                   [0]: 中断类型0。TDS（主模）提前通知TDS异频测量启动中断。
                                                                   [1]: 中断类型1。TDS（主模）进GAP中断，通知TDS进行异频测量射频通道配置，天线切换等操作。
                                                                   [2]: 中断类型2。TDS（主模）出GAP中断。
                                                                   [3]: 中断类型3。TDS（从模，即异频）测量结束中断。
                                                                   [4]: 中断类型4。TDS（主模）强制TDS退出异频测量中断。
                                                                   [5]: 中断类型5。TDS（主模）通知TDS上报本次测量结果。
                                                                   [6]: 中断类型6。保留。
                                                                   [15:7]: 对应中断类型[15:7]，预留。
                                                                   中断状态标志0，1，2只有当TDS处于主模时才能产生。
                                                                   软件读取该中断状态上报寄存器后，需要和0x07CC的int_mask_tds_rcv_tds相与，再判断是否需要处理。 */
        unsigned long  reserved               : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_INT_TYPE_TDS_RCV_TDS_UNION;
#define BBPCOMM_INT_TYPE_TDS_RCV_TDS_o_int_type_tds_rcv_tds_START  (0)
#define BBPCOMM_INT_TYPE_TDS_RCV_TDS_o_int_type_tds_rcv_tds_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_INT_TYPE_TDS_INT012_UNION
 结构说明  : INT_TYPE_TDS_INT012 寄存器结构定义。地址偏移量:0x07F4，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      int_type_tds_int012_reg;
    struct
    {
        unsigned long  o_int_type_tds_int012 : 3;  /* bit[0-2] : 增加主模产生int0、1、2的上报（(包括异频、异系统场景)），高有效。
                                                                 [0]: T主模时，CTU告诉通知TDSP产生int0中断。
                                                                 [1]: T主模时，CTU告诉通知TDSP产生int1中断。
                                                                 [2]: T主模时，CTU告诉通知TDSP产生int2中断。
                                                                 软件读取该中断状态上报寄存器后，需要和0x07DC的int_mask_tds_int012相与，再判断是否需要处理。 */
        unsigned long  reserved              : 29; /* bit[3-31]: 保留 */
    } reg;
} BBPCOMM_INT_TYPE_TDS_INT012_UNION;
#define BBPCOMM_INT_TYPE_TDS_INT012_o_int_type_tds_int012_START  (0)
#define BBPCOMM_INT_TYPE_TDS_INT012_o_int_type_tds_int012_END    (2)


/*****************************************************************************
 结构名    : BBPCOMM_TDS_SND_LTE_INT_13_UNION
 结构说明  : TDS_SND_LTE_INT_13 寄存器结构定义。地址偏移量:0x07F8，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      tds_snd_lte_int_13_reg;
    struct
    {
        unsigned long  reserved_0         : 3;  /* bit[0-2]  : 保留 */
        unsigned long  tds_snd_lte_int_13 : 13; /* bit[3-15] : 逻辑自清零。高有效。
                                                               
                                                               bit15~7：对应tds_snd_lte_int_15 ~ 7。
                                                               TDS DSP通过此寄存器触发类型15~7中断（int_type_lte_rcv_tds[15:7]）给LDSP。
                                                               生效时刻：立即生效
                                                               
                                                               bit6：对应tds_snd_lte_int_6。
                                                               TDSDSP通过此寄存器触发类型6中断（int_type_lte_rcv_tds[6]）给LDSP。
                                                               生效时刻：立即生效
                                                               
                                                               bit5：对应tds_snd_lte_int_5。
                                                               TDS主模时通知LDSP L测量周期到达，高有效。
                                                               TDS DSP通过此寄存器触发类型5中断（int_type_lte_rcv_tds[5]）给LDSP。
                                                               生效时刻：立即生效
                                                               
                                                               bit4：对应tds_snd_lte_int_4。
                                                               TDS主模时强制退出异系统测量指示。高有效。
                                                               TDS DSP通过此寄存器触发类型4中断（int_type_lte_rcv_tds[4]）给LDSP。
                                                               生效时刻：立即生效
                                                               
                                                               bit3：对应tds_snd_lte_int_3。TDS测量结束指示，高有效。
                                                               用于触发中断类型3 （int_type_lte_rcv_tds[3]）给LDSP。L主模下对TDS进行异系统测量时，如果TDS测量结束，TDS DSP配置此标志为1。 
                                                               生效时刻：立即生效 */
        unsigned long  reserved_1         : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_TDS_SND_LTE_INT_13_UNION;
#define BBPCOMM_TDS_SND_LTE_INT_13_tds_snd_lte_int_13_START  (3)
#define BBPCOMM_TDS_SND_LTE_INT_13_tds_snd_lte_int_13_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_TDS_SND_TDS_INT_13_UNION
 结构说明  : TDS_SND_TDS_INT_13 寄存器结构定义。地址偏移量:0x07FC，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      tds_snd_tds_int_13_reg;
    struct
    {
        unsigned long  reserved_0         : 3;  /* bit[0-2]  : 保留 */
        unsigned long  tds_snd_tds_int_13 : 13; /* bit[3-15] : 逻辑自清零。高有效。
                                                               
                                                               bit15~7：对应tds_snd_tds_int_15 ~ 7。
                                                               TDS DSP通过此寄存器触发类型15~7中断（int_type_tds_rcv_tds[15:7]）给TDSP。
                                                               生效时刻：立即生效
                                                               
                                                               bit6：对应tds_snd_tds_int_6。
                                                               TDSDSP通过此寄存器触发类型6中断（int_type_tds_rcv_tds[6]）给TDSP。
                                                               生效时刻：立即生效
                                                               
                                                               bit5：对应tds_snd_tds_int_5。
                                                               TDS主模时通知TDSP T测量周期到达，高有效。
                                                               TDS DSP通过此寄存器触发类型5中断（int_type_tds_rcv_tds[5]）给TDSP。
                                                               生效时刻：立即生效
                                                               
                                                               bit4：对应tds_snd_tds_int_4。
                                                               TDS主模时强制退出异系统测量指示。高有效。
                                                               TDS DSP通过此寄存器触发类型4中断（int_type_tds_rcv_tds[4]）给TDSP。
                                                               生效时刻：立即生效
                                                               
                                                               bit3：对应tds_snd_tds_int_3。TDS测量结束指示，高有效。
                                                               用于触发中断类型3 （int_type_tds_rcv_tds[3]）给TDSP。T主模下对TDS进行异系统测量时，如果TDS测量结束，TDS DSP配置此标志为1。 
                                                               生效时刻：立即生效 */
        unsigned long  reserved_1         : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_TDS_SND_TDS_INT_13_UNION;
#define BBPCOMM_TDS_SND_TDS_INT_13_tds_snd_tds_int_13_START  (3)
#define BBPCOMM_TDS_SND_TDS_INT_13_tds_snd_tds_int_13_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_TDS_SND_GM_INT_13_UNION
 结构说明  : TDS_SND_GM_INT_13 寄存器结构定义。地址偏移量:0x0800，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      tds_snd_gm_int_13_reg;
    struct
    {
        unsigned long  reserved_0        : 3;  /* bit[0-2]  : 保留 */
        unsigned long  tds_snd_gm_int_13 : 13; /* bit[3-15] : 逻辑自清零。高有效。
                                                              
                                                              bit15~7：对应tds_snd_gm_int_15 ~ 7。
                                                              TDS DSP通过此寄存器触发类型15~7中断（int_type_gm_rcv_tds[15:7]）给GMDSP。
                                                              生效时刻：立即生效
                                                              
                                                              bit6：对应tds_snd_gm_int_6。
                                                              TDSDSP通过此寄存器触发类型6中断（int_type_gm_rcv_tds[6]）给GMDSP。
                                                              生效时刻：立即生效
                                                              
                                                              bit5：对应tds_snd_gm_int_5。
                                                              TDS主模时通知GMDSP GM测量周期到达，高有效。
                                                              TDS DSP通过此寄存器触发类型5中断（int_type_gm_rcv_tds[5]）给GMDSP。
                                                              生效时刻：立即生效
                                                              
                                                              bit4：对应tds_snd_gm_int_4。
                                                              TDS主模时强制退出异系统测量指示。高有效。
                                                              TDS DSP通过此寄存器触发类型4中断（int_type_gm_rcv_tds[4]）给GMDSP。
                                                              生效时刻：立即生效
                                                              
                                                              bit3：对应tds_snd_gm_int_3。TDS测量结束指示，高有效。
                                                              用于触发中断类型3 （int_type_gm_rcv_tds[3]）给GMDSP。GM主模下对TDS进行异系统测量时，如果TDS测量结束，TDS DSP配置此标志为1。 
                                                              生效时刻：立即生效 */
        unsigned long  reserved_1        : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_TDS_SND_GM_INT_13_UNION;
#define BBPCOMM_TDS_SND_GM_INT_13_tds_snd_gm_int_13_START  (3)
#define BBPCOMM_TDS_SND_GM_INT_13_tds_snd_gm_int_13_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_TDS_SND_GS_INT_13_UNION
 结构说明  : TDS_SND_GS_INT_13 寄存器结构定义。地址偏移量:0x0804，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      tds_snd_gs_int_13_reg;
    struct
    {
        unsigned long  reserved_0        : 3;  /* bit[0-2]  : 保留 */
        unsigned long  tds_snd_gs_int_13 : 13; /* bit[3-15] : 逻辑自清零。高有效。
                                                              
                                                              bit15~7：对应tds_snd_gs_int_15 ~ 7。
                                                              TDS DSP通过此寄存器触发类型15~7中断（int_type_gs_rcv_tds[15:7]）给GSDSP。
                                                              生效时刻：立即生效
                                                              
                                                              bit6：对应tds_snd_gs_int_6。
                                                              TDSDSP通过此寄存器触发类型6中断（int_type_gs_rcv_tds[6]）给GSDSP。
                                                              生效时刻：立即生效
                                                              
                                                              bit5：对应tds_snd_gs_int_5。
                                                              TDS主模时通知GSDSP GS测量周期到达，高有效。
                                                              TDS DSP通过此寄存器触发类型5中断（int_type_gs_rcv_tds[5]）给GSDSP。
                                                              生效时刻：立即生效
                                                              
                                                              bit4：对应tds_snd_gs_int_4。
                                                              TDS主模时强制退出异系统测量指示。高有效。
                                                              TDS DSP通过此寄存器触发类型4中断（int_type_gs_rcv_tds[4]）给GSDSP。
                                                              生效时刻：立即生效
                                                              
                                                              bit3：对应tds_snd_gs_int_3。TDS测量结束指示，高有效。
                                                              用于触发中断类型3 （int_type_gs_rcv_tds[3]）给GSDSP。GS主模下对TDS进行异系统测量时，如果TDS测量结束，TDS DSP配置此标志为1。 
                                                              生效时刻：立即生效 */
        unsigned long  reserved_1        : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_TDS_SND_GS_INT_13_UNION;
#define BBPCOMM_TDS_SND_GS_INT_13_tds_snd_gs_int_13_START  (3)
#define BBPCOMM_TDS_SND_GS_INT_13_tds_snd_gs_int_13_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_TDS_SND_W_INT_13_UNION
 结构说明  : TDS_SND_W_INT_13 寄存器结构定义。地址偏移量:0x0808，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      tds_snd_w_int_13_reg;
    struct
    {
        unsigned long  reserved_0       : 3;  /* bit[0-2]  : 保留 */
        unsigned long  tds_snd_w_int_13 : 13; /* bit[3-15] : 逻辑自清零。高有效。
                                                             
                                                             bit15~7：对应tds_snd_w_int_15 ~ 7。
                                                             TDS DSP通过此寄存器触发类型15~7中断（int_type_w_rcv_tds[15:7]）给WDSP。
                                                             生效时刻：立即生效
                                                             
                                                             bit6：对应tds_snd_w_int_6。
                                                             TDSDSP通过此寄存器触发类型6中断（int_type_w_rcv_tds[6]）给WDSP。
                                                             生效时刻：立即生效
                                                             
                                                             bit5：对应tds_snd_w_int_5。
                                                             TDS主模时通知WDSP W测量周期到达，高有效。
                                                             TDS DSP通过此寄存器触发类型5中断（int_type_w_rcv_tds[5]）给WDSP。
                                                             生效时刻：立即生效
                                                             
                                                             bit4：对应tds_snd_w_int_4。
                                                             TDS主模时强制退出异系统测量指示。高有效。
                                                             TDS DSP通过此寄存器触发类型4中断（int_type_w_rcv_tds[4]）给WDSP。
                                                             生效时刻：立即生效
                                                             
                                                             bit3：对应tds_snd_w_int_3。TDS测量结束指示，高有效。
                                                             用于触发中断类型3 （int_type_w_rcv_tds[3]）给WDSP。W主模下对TDS进行异系统测量时，如果TDS测量结束，TDS DSP配置此标志为1。 
                                                             生效时刻：立即生效 */
        unsigned long  reserved_1       : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_TDS_SND_W_INT_13_UNION;
#define BBPCOMM_TDS_SND_W_INT_13_tds_snd_w_int_13_START  (3)
#define BBPCOMM_TDS_SND_W_INT_13_tds_snd_w_int_13_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_GM_INTERSYS_MEASURE_TYPE_UNION
 结构说明  : GM_INTERSYS_MEASURE_TYPE 寄存器结构定义。地址偏移量:0x080C，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      gm_intersys_measure_type_reg;
    struct
    {
        unsigned long  gm_intersys_measure_type : 4;  /* bit[0-3] : 异系统测量类型。
                                                                    4’d0：测量GM RSSI。
                                                                    4’d1: 测量GM bsic verification。
                                                                    4’d2: 测量GM bsic confirm。
                                                                    4’d3: 背景搜GM；
                                                                    4’d4: 2G ANR。
                                                                    其它预留
                                                                    生效时刻：立即生效 */
        unsigned long  reserved                 : 28; /* bit[4-31]: 保留 */
    } reg;
} BBPCOMM_GM_INTERSYS_MEASURE_TYPE_UNION;
#define BBPCOMM_GM_INTERSYS_MEASURE_TYPE_gm_intersys_measure_type_START  (0)
#define BBPCOMM_GM_INTERSYS_MEASURE_TYPE_gm_intersys_measure_type_END    (3)


/*****************************************************************************
 结构名    : BBPCOMM_GM_MEASURE_REPORT_VALID_UNION
 结构说明  : GM_MEASURE_REPORT_VALID 寄存器结构定义。地址偏移量:0x0810，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      gm_measure_report_valid_reg;
    struct
    {
        unsigned long  gm_measure_report_valid : 1;  /* bit[0]   : 单频点测量周期到指示，L或者TDS/W/Gs作为主模，通知GM DSP GM测量的单频点周期到达。
                                                                   只在连接态下需要配置。
                                                                   电平信号，高有效。
                                                                   生效时刻：立即生效 */
        unsigned long  reserved                : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_GM_MEASURE_REPORT_VALID_UNION;
#define BBPCOMM_GM_MEASURE_REPORT_VALID_gm_measure_report_valid_START  (0)
#define BBPCOMM_GM_MEASURE_REPORT_VALID_gm_measure_report_valid_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_INT_CLEAR_GM_RCV_LTE_UNION
 结构说明  : INT_CLEAR_GM_RCV_LTE 寄存器结构定义。地址偏移量:0x081C，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      int_clear_gm_rcv_lte_reg;
    struct
    {
        unsigned long  int_clear_gm_rcv_lte : 16; /* bit[0-15] : 逻辑自清零。
                                                                 中断标志清除指示，高有效。用于清除INT_TYPE_GM_RCV_LTE[15:0]对应bit位
                                                                 生效时刻：立即生效 */
        unsigned long  reserved             : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_INT_CLEAR_GM_RCV_LTE_UNION;
#define BBPCOMM_INT_CLEAR_GM_RCV_LTE_int_clear_gm_rcv_lte_START  (0)
#define BBPCOMM_INT_CLEAR_GM_RCV_LTE_int_clear_gm_rcv_lte_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_INT_CLEAR_GM_RCV_TDS_UNION
 结构说明  : INT_CLEAR_GM_RCV_TDS 寄存器结构定义。地址偏移量:0x0820，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      int_clear_gm_rcv_tds_reg;
    struct
    {
        unsigned long  int_clear_gm_rcv_tds : 16; /* bit[0-15] : 逻辑自清零。
                                                                 中断标志清除指示，高有效。用于清除INT_TYPE_GM_RCV_TDS[15:0]对应bit位
                                                                 生效时刻：立即生效 */
        unsigned long  reserved             : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_INT_CLEAR_GM_RCV_TDS_UNION;
#define BBPCOMM_INT_CLEAR_GM_RCV_TDS_int_clear_gm_rcv_tds_START  (0)
#define BBPCOMM_INT_CLEAR_GM_RCV_TDS_int_clear_gm_rcv_tds_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_INT_CLEAR_GM_RCV_GM_UNION
 结构说明  : INT_CLEAR_GM_RCV_GM 寄存器结构定义。地址偏移量:0x0824，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      int_clear_gm_rcv_gm_reg;
    struct
    {
        unsigned long  int_clear_gm_rcv_gm : 16; /* bit[0-15] : 逻辑自清零。
                                                                中断标志清除指示，高有效。用于清除INT_TYPE_GM_RCV_GM[15:0]对应bit位
                                                                生效时刻：立即生效 */
        unsigned long  reserved            : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_INT_CLEAR_GM_RCV_GM_UNION;
#define BBPCOMM_INT_CLEAR_GM_RCV_GM_int_clear_gm_rcv_gm_START  (0)
#define BBPCOMM_INT_CLEAR_GM_RCV_GM_int_clear_gm_rcv_gm_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_INT_CLEAR_GM_RCV_GS_UNION
 结构说明  : INT_CLEAR_GM_RCV_GS 寄存器结构定义。地址偏移量:0x0828，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      int_clear_gm_rcv_gs_reg;
    struct
    {
        unsigned long  int_clear_gm_rcv_gs : 16; /* bit[0-15] : 逻辑自清零。
                                                                中断标志清除指示，高有效。用于清除INT_TYPE_GM_RCV_GS[15:0]对应bit位
                                                                生效时刻：立即生效 */
        unsigned long  reserved            : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_INT_CLEAR_GM_RCV_GS_UNION;
#define BBPCOMM_INT_CLEAR_GM_RCV_GS_int_clear_gm_rcv_gs_START  (0)
#define BBPCOMM_INT_CLEAR_GM_RCV_GS_int_clear_gm_rcv_gs_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_INT_CLEAR_GM_RCV_W_UNION
 结构说明  : INT_CLEAR_GM_RCV_W 寄存器结构定义。地址偏移量:0x082C，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      int_clear_gm_rcv_w_reg;
    struct
    {
        unsigned long  int_clear_gm_rcv_w : 16; /* bit[0-15] : 逻辑自清零。
                                                               中断标志清除指示，高有效。用于清除INT_TYPE_GM_RCV_W[15:0]对应bit位
                                                               生效时刻：立即生效 */
        unsigned long  reserved           : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_INT_CLEAR_GM_RCV_W_UNION;
#define BBPCOMM_INT_CLEAR_GM_RCV_W_int_clear_gm_rcv_w_START  (0)
#define BBPCOMM_INT_CLEAR_GM_RCV_W_int_clear_gm_rcv_w_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_INT_CLEAR_GM_INT012_UNION
 结构说明  : INT_CLEAR_GM_INT012 寄存器结构定义。地址偏移量:0x0830，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      int_clear_gm_int012_reg;
    struct
    {
        unsigned long  int_clear_gm_int012 : 3;  /* bit[0-2] : 逻辑自清零。
                                                               中断标志清除指示，高有效。用于清除INT_TYPE_GM_INT012[2:0]对应bit位
                                                               生效时刻：立即生效 */
        unsigned long  reserved            : 29; /* bit[3-31]: 保留 */
    } reg;
} BBPCOMM_INT_CLEAR_GM_INT012_UNION;
#define BBPCOMM_INT_CLEAR_GM_INT012_int_clear_gm_int012_START  (0)
#define BBPCOMM_INT_CLEAR_GM_INT012_int_clear_gm_int012_END    (2)


/*****************************************************************************
 结构名    : BBPCOMM_INT_MASK_GM_RCV_LTE_UNION
 结构说明  : INT_MASK_GM_RCV_LTE 寄存器结构定义。地址偏移量:0x0834，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      int_mask_gm_rcv_lte_reg;
    struct
    {
        unsigned long  int_mask_gm_rcv_lte : 16; /* bit[0-15] : 中断屏蔽标志。用于屏蔽INT_TYPE_GM_RCV_LTE[15:0]对应bit中断。
                                                                0：屏蔽中断上报；
                                                                1：使能中断上报。
                                                                配置时刻：初始化
                                                                生效时刻：立即生效 */
        unsigned long  reserved            : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_INT_MASK_GM_RCV_LTE_UNION;
#define BBPCOMM_INT_MASK_GM_RCV_LTE_int_mask_gm_rcv_lte_START  (0)
#define BBPCOMM_INT_MASK_GM_RCV_LTE_int_mask_gm_rcv_lte_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_INT_MASK_GM_RCV_TDS_UNION
 结构说明  : INT_MASK_GM_RCV_TDS 寄存器结构定义。地址偏移量:0x0838，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      int_mask_gm_rcv_tds_reg;
    struct
    {
        unsigned long  int_mask_gm_rcv_tds : 16; /* bit[0-15] : 中断屏蔽标志。用于屏蔽INT_TYPE_GM_RCV_TDS[15:0]对应bit中断。
                                                                0：屏蔽中断上报；
                                                                1：使能中断上报。
                                                                配置时刻：初始化
                                                                生效时刻：立即生效 */
        unsigned long  reserved            : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_INT_MASK_GM_RCV_TDS_UNION;
#define BBPCOMM_INT_MASK_GM_RCV_TDS_int_mask_gm_rcv_tds_START  (0)
#define BBPCOMM_INT_MASK_GM_RCV_TDS_int_mask_gm_rcv_tds_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_INT_MASK_GM_RCV_GM_UNION
 结构说明  : INT_MASK_GM_RCV_GM 寄存器结构定义。地址偏移量:0x083C，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      int_mask_gm_rcv_gm_reg;
    struct
    {
        unsigned long  int_mask_gm_rcv_gm : 16; /* bit[0-15] : 中断屏蔽标志。用于屏蔽INT_TYPE_GM_RCV_GM[15:0]对应bit中断。
                                                               0：屏蔽中断上报；
                                                               1：使能中断上报。
                                                               配置时刻：初始化
                                                               生效时刻：立即生效 */
        unsigned long  reserved           : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_INT_MASK_GM_RCV_GM_UNION;
#define BBPCOMM_INT_MASK_GM_RCV_GM_int_mask_gm_rcv_gm_START  (0)
#define BBPCOMM_INT_MASK_GM_RCV_GM_int_mask_gm_rcv_gm_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_INT_MASK_GM_RCV_GS_UNION
 结构说明  : INT_MASK_GM_RCV_GS 寄存器结构定义。地址偏移量:0x0840，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      int_mask_gm_rcv_gs_reg;
    struct
    {
        unsigned long  int_mask_gm_rcv_gs : 16; /* bit[0-15] : 中断屏蔽标志。用于屏蔽INT_TYPE_GM_RCV_GS[15:0]对应bit中断。
                                                               0：屏蔽中断上报；
                                                               1：使能中断上报。
                                                               配置时刻：初始化
                                                               生效时刻：立即生效 */
        unsigned long  reserved           : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_INT_MASK_GM_RCV_GS_UNION;
#define BBPCOMM_INT_MASK_GM_RCV_GS_int_mask_gm_rcv_gs_START  (0)
#define BBPCOMM_INT_MASK_GM_RCV_GS_int_mask_gm_rcv_gs_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_INT_MASK_GM_RCV_W_UNION
 结构说明  : INT_MASK_GM_RCV_W 寄存器结构定义。地址偏移量:0x0844，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      int_mask_gm_rcv_w_reg;
    struct
    {
        unsigned long  int_mask_gm_rcv_w : 16; /* bit[0-15] : 中断屏蔽标志。用于屏蔽INT_TYPE_GM_RCV_W[15:0]对应bit中断。
                                                              0：屏蔽中断上报；
                                                              1：使能中断上报。
                                                              配置时刻：初始化
                                                              生效时刻：立即生效 */
        unsigned long  reserved          : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_INT_MASK_GM_RCV_W_UNION;
#define BBPCOMM_INT_MASK_GM_RCV_W_int_mask_gm_rcv_w_START  (0)
#define BBPCOMM_INT_MASK_GM_RCV_W_int_mask_gm_rcv_w_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_INT_MASK_GM_INT012_UNION
 结构说明  : INT_MASK_GM_INT012 寄存器结构定义。地址偏移量:0x0848，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      int_mask_gm_int012_reg;
    struct
    {
        unsigned long  int_mask_gm_int012 : 3;  /* bit[0-2] : 中断屏蔽标志。用于屏蔽INT_TYPE_GM_INT012[2:0]对应bit中断。
                                                              0：屏蔽中断上报；
                                                              1：使能中断上报。
                                                              配置时刻：初始化
                                                              生效时刻：立即生效 */
        unsigned long  reserved           : 29; /* bit[3-31]: 保留 */
    } reg;
} BBPCOMM_INT_MASK_GM_INT012_UNION;
#define BBPCOMM_INT_MASK_GM_INT012_int_mask_gm_int012_START  (0)
#define BBPCOMM_INT_MASK_GM_INT012_int_mask_gm_int012_END    (2)


/*****************************************************************************
 结构名    : BBPCOMM_INT_TYPE_GM_RCV_LTE_UNION
 结构说明  : INT_TYPE_GM_RCV_LTE 寄存器结构定义。地址偏移量:0x084C，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      int_type_gm_rcv_lte_reg;
    struct
    {
        unsigned long  o_int_type_gm_rcv_lte : 16; /* bit[0-15] : 中断状态标志。各bit单独对应一种中断类型，高有效。
                                                                  [0]: 中断类型0。 LTE（主模）提前通知GM异系统测量启动中断。
                                                                  [1]: 中断类型1。 LTE（主模）进GAP中断，通知GM进行异系统测量射频通道配置，天线切换等操作。
                                                                  [2]: 中断类型2。 LTE（主模）出GAP中断。
                                                                  [3]: 中断类型3。LTE（从模）测量结束中断。
                                                                  [4]: 中断类型4。LTE（主模）强制GM退出异系统测量中断。
                                                                  [5]: 中断类型5。LTE（主模）通知GM上报本次测量结果。
                                                                  [6]: 保留。
                                                                  [15:7]: 对应中断类型[15:7]，预留。
                                                                  软件读取该中断状态上报寄存器后，需要和0x0834的int_mask_gm_rcv_lte相与，再判断是否需要处理。 */
        unsigned long  reserved              : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_INT_TYPE_GM_RCV_LTE_UNION;
#define BBPCOMM_INT_TYPE_GM_RCV_LTE_o_int_type_gm_rcv_lte_START  (0)
#define BBPCOMM_INT_TYPE_GM_RCV_LTE_o_int_type_gm_rcv_lte_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_INT_TYPE_GM_RCV_TDS_UNION
 结构说明  : INT_TYPE_GM_RCV_TDS 寄存器结构定义。地址偏移量:0x0850，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      int_type_gm_rcv_tds_reg;
    struct
    {
        unsigned long  o_int_type_gm_rcv_tds : 16; /* bit[0-15] : 中断状态标志0，1，2只有当TDS处于主模时才能产生。
                                                                  
                                                                  [0]: 中断类型0。 TDS（主模）提前通知GM异系统测量启动中断。
                                                                  [1]: 中断类型1。 TDS（主模）进GAP中断，通知GM进行异系统测量射频通道配置，天线切换等操作。
                                                                  [2]: 中断类型2。 TDS（主模）出GAP中断。
                                                                  [3]: 中断类型3。TDS（从模）测量结束中断。
                                                                  [4]: 中断类型4。TDS（主模）强制GM退出异系统测量中断。
                                                                  [5]: 中断类型5。TDS（主模）通知GM上报本次测量结果。
                                                                  [6]: 中断类型6。保留。
                                                                  [15:7]: 对应中断类型[15:7]，预留。
                                                                  软件读取该中断状态上报寄存器后，需要和0x0838的int_mask_gm_rcv_tds相与，再判断是否需要处理。 */
        unsigned long  reserved              : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_INT_TYPE_GM_RCV_TDS_UNION;
#define BBPCOMM_INT_TYPE_GM_RCV_TDS_o_int_type_gm_rcv_tds_START  (0)
#define BBPCOMM_INT_TYPE_GM_RCV_TDS_o_int_type_gm_rcv_tds_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_INT_TYPE_GM_RCV_W_UNION
 结构说明  : INT_TYPE_GM_RCV_W 寄存器结构定义。地址偏移量:0x0854，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      int_type_gm_rcv_w_reg;
    struct
    {
        unsigned long  o_int_type_gm_rcv_w : 16; /* bit[0-15] : 中断状态标志。各bit单独对应一种中断类型，高有效。
                                                                [0]: 中断类型0。W（主模）提前通知GM异系统测量启动中断。
                                                                [1]: 中断类型1。W（主模）进GAP中断，通知GM进行异系统测量射频通道配置，天线切换等操作。
                                                                [2]: 中断类型2。W（主模）出GAP中断。
                                                                [3]: 中断类型3。W（从模）测量结束中断。
                                                                [4]: 中断类型4。W（主模）强制GM退出异系统测量中断。
                                                                [5]: 中断类型5。W（主模）通知GM上报本次测量结果。
                                                                [6]: 中断类型6。保留。
                                                                [15:7]: 对应中断类型[15:7]，预留。
                                                                中断状态标志0，1，2只有当W处于主模时才能产生。
                                                                软件读取该中断状态上报寄存器后，需要和0x0844的int_mask_gm_rcv_w相与，再判断是否需要处理。 */
        unsigned long  reserved            : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_INT_TYPE_GM_RCV_W_UNION;
#define BBPCOMM_INT_TYPE_GM_RCV_W_o_int_type_gm_rcv_w_START  (0)
#define BBPCOMM_INT_TYPE_GM_RCV_W_o_int_type_gm_rcv_w_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_INT_TYPE_GM_RCV_GS_UNION
 结构说明  : INT_TYPE_GM_RCV_GS 寄存器结构定义。地址偏移量:0x0858，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      int_type_gm_rcv_gs_reg;
    struct
    {
        unsigned long  o_int_type_gm_rcv_gs : 16; /* bit[0-15] : [0]: 中断类型0。GS（主模）提前通知GM异系统测量启动中断。
                                                                 [1]: 中断类型1。GS（主模）进GAP中断，通知GM进行异系统测量射频通道配置，天线切换等操作。
                                                                 [2]: 中断类型2。GS（主模）出GAP中断。
                                                                 [3]: 中断类型3。GS（从模）测量结束中断。
                                                                 [4]: 中断类型4。GS（主模）强制GM退出异系统测量中断。
                                                                 [5]: 中断类型5。GS（主模）通知GM上报本次测量结果。
                                                                 [6]: 中断类型6。保留。
                                                                 [15:7]: 对应中断类型[15:7]，预留。
                                                                 中断状态标志0，1，2只有当TDS处于主模时才能产生。
                                                                 软件读取该中断状态上报寄存器后，需要和0x0840的int_mask_gm_rcv_gs相与，再判断是否需要处理。 */
        unsigned long  reserved             : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_INT_TYPE_GM_RCV_GS_UNION;
#define BBPCOMM_INT_TYPE_GM_RCV_GS_o_int_type_gm_rcv_gs_START  (0)
#define BBPCOMM_INT_TYPE_GM_RCV_GS_o_int_type_gm_rcv_gs_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_INT_TYPE_GM_RCV_GM_UNION
 结构说明  : INT_TYPE_GM_RCV_GM 寄存器结构定义。地址偏移量:0x085C，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      int_type_gm_rcv_gm_reg;
    struct
    {
        unsigned long  o_int_type_gm_rcv_gm : 16; /* bit[0-15] : 中断状态标志。各bit单独对应一种中断类型，高有效。
                                                                 [0]: 中断类型0。GM（主模）提前通知GM异频测量启动中断。
                                                                 [1]: 中断类型1。GM（主模）进GAP中断，通知GM进行异频测量射频通道配置，天线切换等操作。
                                                                 [2]: 中断类型2。GM（主模）出GAP中断。
                                                                 [3]: 中断类型3。GM（从模，即异频）测量结束中断。
                                                                 [4]: 中断类型4。GM（主模）强制GM退出异频测量中断。
                                                                 [5]: 中断类型5。GM（主模）通知GM上报本次测量结果。
                                                                 [6]: 中断类型6。保留。
                                                                 [15:7]: 对应中断类型[15:7]，预留。
                                                                 中断状态标志0，1，2只有当GM处于主模时才能产生。
                                                                 软件读取该中断状态上报寄存器后，需要和0x083C的int_mask_gm_rcv_gm相与，再判断是否需要处理。 */
        unsigned long  reserved             : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_INT_TYPE_GM_RCV_GM_UNION;
#define BBPCOMM_INT_TYPE_GM_RCV_GM_o_int_type_gm_rcv_gm_START  (0)
#define BBPCOMM_INT_TYPE_GM_RCV_GM_o_int_type_gm_rcv_gm_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_INT_TYPE_GM_INT012_UNION
 结构说明  : INT_TYPE_GM_INT012 寄存器结构定义。地址偏移量:0x0860，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      int_type_gm_int012_reg;
    struct
    {
        unsigned long  o_int_type_gm_int012 : 3;  /* bit[0-2] : 增加主模产生int0、1、2的上报（(包括异频、异系统场景)），高有效。
                                                                [0]: GM主模时，CTU告诉通知GMDSP产生int0中断。
                                                                [1]: GM主模时，CTU告诉通知GMDSP产生int1中断。
                                                                [2]: GM主模时，CTU告诉通知GMDSP产生int2中断。
                                                                软件读取该中断状态上报寄存器后，需要和0x0848的int_mask_gm_int012相与，再判断是否需要处理。 */
        unsigned long  reserved             : 29; /* bit[3-31]: 保留 */
    } reg;
} BBPCOMM_INT_TYPE_GM_INT012_UNION;
#define BBPCOMM_INT_TYPE_GM_INT012_o_int_type_gm_int012_START  (0)
#define BBPCOMM_INT_TYPE_GM_INT012_o_int_type_gm_int012_END    (2)


/*****************************************************************************
 结构名    : BBPCOMM_GM_SND_LTE_INT_13_UNION
 结构说明  : GM_SND_LTE_INT_13 寄存器结构定义。地址偏移量:0x0864，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      gm_snd_lte_int_13_reg;
    struct
    {
        unsigned long  reserved_0        : 3;  /* bit[0-2]  : 保留 */
        unsigned long  gm_snd_lte_int_13 : 13; /* bit[3-15] : 逻辑自清零。高有效。
                                                              
                                                              bit15~7：对应gm_snd_lte_int_15 ~ 7。
                                                              GM DSP通过此寄存器触发类型15~7中断（int_type_lte_rcv_gm[15:7]）给LTEDSP。
                                                              生效时刻：立即生效
                                                              
                                                              bit6：对应gm_snd_lte_int_6。
                                                              GMDSP通过此寄存器触发类型6中断（int_type_lte_rcv_gm[6]）给LTEDSP。
                                                              生效时刻：立即生效
                                                              
                                                              bit5：对应gm_snd_lte_int_5。
                                                              GM主模时通知LTEDSP LTE测量周期到达，高有效。
                                                              GM DSP通过此寄存器触发类型5中断（int_type_lte_rcv_gm[5]）给LTEDSP。
                                                              生效时刻：立即生效
                                                              
                                                              bit4：对应gm_snd_lte_int_4。
                                                              GM主模时强制退出异系统测量指示。高有效。
                                                              GM DSP通过此寄存器触发类型4中断（int_type_lte_rcv_gm[4]）给LTEDSP。
                                                              生效时刻：立即生效
                                                              
                                                              bit3：对应gm_snd_lte_int_3。GM测量结束指示，高有效。
                                                              用于触发中断类型3 （int_type_lte_rcv_gm[3]）给LTEDSP。LTE主模下对GM进行异系统测量时，如果GM测量结束，GM DSP配置此标志为1。 
                                                              生效时刻：立即生效 */
        unsigned long  reserved_1        : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_GM_SND_LTE_INT_13_UNION;
#define BBPCOMM_GM_SND_LTE_INT_13_gm_snd_lte_int_13_START  (3)
#define BBPCOMM_GM_SND_LTE_INT_13_gm_snd_lte_int_13_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_GM_SND_TDS_INT_13_UNION
 结构说明  : GM_SND_TDS_INT_13 寄存器结构定义。地址偏移量:0x0868，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      gm_snd_tds_int_13_reg;
    struct
    {
        unsigned long  reserved_0        : 3;  /* bit[0-2]  : 保留 */
        unsigned long  gm_snd_tds_int_13 : 13; /* bit[3-15] : 逻辑自清零。高有效。
                                                              
                                                              bit15~7：对应gm_snd_tds_int_15 ~ 7。
                                                              GM DSP通过此寄存器触发类型15~7中断（int_type_tds_rcv_gm[15:7]）给TDSDSP。
                                                              生效时刻：立即生效
                                                              
                                                              bit6：对应gm_snd_tds_int_6。
                                                              GMDSP通过此寄存器触发类型6中断（int_type_tds_rcv_gm[6]）给TDSDSP。
                                                              生效时刻：立即生效
                                                              
                                                              bit5：对应gm_snd_tds_int_5。
                                                              GM主模时通知TDSDSP tds测量周期到达，高有效。
                                                              GM DSP通过此寄存器触发类型5中断（int_type_tds_rcv_gm[5]）给TDSDSP。
                                                              生效时刻：立即生效
                                                              
                                                              bit4：对应gm_snd_tds_int_4。
                                                              GM主模时强制退出异系统测量指示。高有效。
                                                              GM DSP通过此寄存器触发类型4中断（int_type_tds_rcv_gm[4]）给TDSDSP。
                                                              生效时刻：立即生效
                                                              
                                                              bit3：对应gm_snd_tds_int_3。GM测量结束指示，高有效。
                                                              用于触发中断类型3 （int_type_tds_rcv_gm[3]）给TDSDSP。tds主模下对GM进行异系统测量时，如果GM测量结束，GM DSP配置此标志为1。 
                                                              生效时刻：立即生效 */
        unsigned long  reserved_1        : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_GM_SND_TDS_INT_13_UNION;
#define BBPCOMM_GM_SND_TDS_INT_13_gm_snd_tds_int_13_START  (3)
#define BBPCOMM_GM_SND_TDS_INT_13_gm_snd_tds_int_13_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_GM_SND_GM_INT_13_UNION
 结构说明  : GM_SND_GM_INT_13 寄存器结构定义。地址偏移量:0x086C，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      gm_snd_gm_int_13_reg;
    struct
    {
        unsigned long  reserved_0       : 3;  /* bit[0-2]  : 保留 */
        unsigned long  gm_snd_gm_int_13 : 13; /* bit[3-15] : 逻辑自清零。高有效。
                                                             
                                                             bit15~7：对应gm_snd_gm_int_15 ~ 7。
                                                             GM DSP通过此寄存器触发类型15~7中断（int_type_gm_rcv_gm[15:7]）给GMDSP。
                                                             生效时刻：立即生效
                                                             
                                                             bit6：对应gm_snd_gm_int_6。
                                                             GMDSP通过此寄存器触发类型6中断（int_type_gm_rcv_gm[6]）给GMDSP。
                                                             生效时刻：立即生效
                                                             
                                                             bit5：对应gm_snd_gm_int_5。
                                                             GM主模时通知GMDSP GM测量周期到达，高有效。
                                                             GM DSP通过此寄存器触发类型5中断（int_type_gm_rcv_gm[5]）给GMDSP。
                                                             生效时刻：立即生效
                                                             
                                                             bit4：对应gm_snd_gm_int_4。
                                                             GM主模时强制退出异系统测量指示。高有效。
                                                             GM DSP通过此寄存器触发类型4中断（int_type_gm_rcv_gm[4]）给GMDSP。
                                                             生效时刻：立即生效
                                                             
                                                             bit3：对应gm_snd_gm_int_3。GM测量结束指示，高有效。
                                                             用于触发中断类型3 （int_type_gm_rcv_gm[3]）给GMDSP。GM主模下对GM进行异系统测量时，如果GM测量结束，GM DSP配置此标志为1。 
                                                             生效时刻：立即生效 */
        unsigned long  reserved_1       : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_GM_SND_GM_INT_13_UNION;
#define BBPCOMM_GM_SND_GM_INT_13_gm_snd_gm_int_13_START  (3)
#define BBPCOMM_GM_SND_GM_INT_13_gm_snd_gm_int_13_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_GM_SND_GS_INT_13_UNION
 结构说明  : GM_SND_GS_INT_13 寄存器结构定义。地址偏移量:0x0870，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      gm_snd_gs_int_13_reg;
    struct
    {
        unsigned long  reserved_0       : 3;  /* bit[0-2]  : 保留 */
        unsigned long  gm_snd_gs_int_13 : 13; /* bit[3-15] : 逻辑自清零。高有效。
                                                             
                                                             bit15~7：对应gm_snd_gs_int_15 ~ 7。
                                                             GM DSP通过此寄存器触发类型15~7中断（int_type_gs_rcv_gm[15:7]）给GSDSP。
                                                             生效时刻：立即生效
                                                             
                                                             bit6：对应gm_snd_gs_int_6。
                                                             GMDSP通过此寄存器触发类型6中断（int_type_gs_rcv_gm[6]）给GSDSP。
                                                             生效时刻：立即生效
                                                             
                                                             bit5：对应gm_snd_gs_int_5。
                                                             GM主模时通知GSDSP GS测量周期到达，高有效。
                                                             GM DSP通过此寄存器触发类型5中断（int_type_gs_rcv_gm[5]）给GSDSP。
                                                             生效时刻：立即生效
                                                             
                                                             bit4：对应gm_snd_gs_int_4。
                                                             GM主模时强制退出异系统测量指示。高有效。
                                                             GM DSP通过此寄存器触发类型4中断（int_type_gs_rcv_gm[4]）给GSDSP。
                                                             生效时刻：立即生效
                                                             
                                                             bit3：对应gm_snd_gs_int_3。GM测量结束指示，高有效。
                                                             用于触发中断类型3 （int_type_gs_rcv_gm[3]）给GSDSP。GS主模下对GM进行异系统测量时，如果GM测量结束，GM DSP配置此标志为1。 
                                                             生效时刻：立即生效 */
        unsigned long  reserved_1       : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_GM_SND_GS_INT_13_UNION;
#define BBPCOMM_GM_SND_GS_INT_13_gm_snd_gs_int_13_START  (3)
#define BBPCOMM_GM_SND_GS_INT_13_gm_snd_gs_int_13_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_GM_SND_W_INT_13_UNION
 结构说明  : GM_SND_W_INT_13 寄存器结构定义。地址偏移量:0x0874，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      gm_snd_w_int_13_reg;
    struct
    {
        unsigned long  reserved_0      : 3;  /* bit[0-2]  : 保留 */
        unsigned long  gm_snd_w_int_13 : 13; /* bit[3-15] : 逻辑自清零。高有效。
                                                            
                                                            bit15~7：对应gm_snd_lte_int_15 ~ 7。
                                                            GM DSP通过此寄存器触发类型15~7中断（int_type_lte_rcv_gm[15:7]）给LTEDSP。
                                                            生效时刻：立即生效
                                                            
                                                            bit6：对应gm_snd_lte_int_6。
                                                            GMDSP通过此寄存器触发类型6中断（int_type_lte_rcv_gm[6]）给LTEDSP。
                                                            生效时刻：立即生效
                                                            
                                                            bit5：对应gm_snd_lte_int_5。
                                                            GM主模时通知LTEDSP LTE测量周期到达，高有效。
                                                            GM DSP通过此寄存器触发类型5中断（int_type_lte_rcv_gm[5]）给LTEDSP。
                                                            生效时刻：立即生效
                                                            
                                                            bit4：对应gm_snd_lte_int_4。
                                                            GM主模时强制退出异系统测量指示。高有效。
                                                            GM DSP通过此寄存器触发类型4中断（int_type_lte_rcv_gm[4]）给LTEDSP。
                                                            生效时刻：立即生效
                                                            
                                                            bit3：对应gm_snd_lte_int_3。GM测量结束指示，高有效。
                                                            用于触发中断类型3 （int_type_lte_rcv_gm[3]）给LTEDSP。LTE主模下对GM进行异系统测量时，如果GM测量结束，GM DSP配置此标志为1。 
                                                            生效时刻：立即生效 */
        unsigned long  reserved_1      : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_GM_SND_W_INT_13_UNION;
#define BBPCOMM_GM_SND_W_INT_13_gm_snd_w_int_13_START  (3)
#define BBPCOMM_GM_SND_W_INT_13_gm_snd_w_int_13_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_GS_INTERSYS_MEASURE_TYPE_UNION
 结构说明  : GS_INTERSYS_MEASURE_TYPE 寄存器结构定义。地址偏移量:0x0878，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      gs_intersys_measure_type_reg;
    struct
    {
        unsigned long  gs_intersys_measure_type : 4;  /* bit[0-3] : 异系统测量类型。
                                                                    4’d0：测量GS RSSI。
                                                                    4’d1: 测量GS bsic verification。
                                                                    4’d2: 测量GS bsic confirm。
                                                                    4’d3: 背景搜GS
                                                                    4’d4: 2G ANR。
                                                                    其它预留
                                                                    生效时刻：立即生效 */
        unsigned long  reserved                 : 28; /* bit[4-31]: 保留 */
    } reg;
} BBPCOMM_GS_INTERSYS_MEASURE_TYPE_UNION;
#define BBPCOMM_GS_INTERSYS_MEASURE_TYPE_gs_intersys_measure_type_START  (0)
#define BBPCOMM_GS_INTERSYS_MEASURE_TYPE_gs_intersys_measure_type_END    (3)


/*****************************************************************************
 结构名    : BBPCOMM_GS_MEASURE_REPORT_VALID_UNION
 结构说明  : GS_MEASURE_REPORT_VALID 寄存器结构定义。地址偏移量:0x087C，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      gs_measure_report_valid_reg;
    struct
    {
        unsigned long  gs_measure_report_valid : 1;  /* bit[0]   : 单频点测量周期到指示，L或者TDS/W/Gm作为主模，通知GS DSP GS测量的单频点周期到达。
                                                                   只在连接态下需要配置。
                                                                   电平信号，高有效。
                                                                   生效时刻：立即生效 */
        unsigned long  reserved                : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_GS_MEASURE_REPORT_VALID_UNION;
#define BBPCOMM_GS_MEASURE_REPORT_VALID_gs_measure_report_valid_START  (0)
#define BBPCOMM_GS_MEASURE_REPORT_VALID_gs_measure_report_valid_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_INT_CLEAR_GS_RCV_LTE_UNION
 结构说明  : INT_CLEAR_GS_RCV_LTE 寄存器结构定义。地址偏移量:0x0888，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      int_clear_gs_rcv_lte_reg;
    struct
    {
        unsigned long  int_clear_gs_rcv_lte : 16; /* bit[0-15] : 逻辑自清零。
                                                                 中断标志清除指示，高有效。用于清除INT_TYPE_GS_RCV_LTE[15:0]对应bit位
                                                                 生效时刻：立即生效 */
        unsigned long  reserved             : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_INT_CLEAR_GS_RCV_LTE_UNION;
#define BBPCOMM_INT_CLEAR_GS_RCV_LTE_int_clear_gs_rcv_lte_START  (0)
#define BBPCOMM_INT_CLEAR_GS_RCV_LTE_int_clear_gs_rcv_lte_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_INT_CLEAR_GS_RCV_TDS_UNION
 结构说明  : INT_CLEAR_GS_RCV_TDS 寄存器结构定义。地址偏移量:0x088C，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      int_clear_gs_rcv_tds_reg;
    struct
    {
        unsigned long  int_clear_gs_rcv_tds : 16; /* bit[0-15] : 逻辑自清零。
                                                                 中断标志清除指示，高有效。用于清除INT_TYPE_GS_RCV_TDS[15:0]对应bit位
                                                                 生效时刻：立即生效 */
        unsigned long  reserved             : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_INT_CLEAR_GS_RCV_TDS_UNION;
#define BBPCOMM_INT_CLEAR_GS_RCV_TDS_int_clear_gs_rcv_tds_START  (0)
#define BBPCOMM_INT_CLEAR_GS_RCV_TDS_int_clear_gs_rcv_tds_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_INT_CLEAR_GS_RCV_GM_UNION
 结构说明  : INT_CLEAR_GS_RCV_GM 寄存器结构定义。地址偏移量:0x0890，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      int_clear_gs_rcv_gm_reg;
    struct
    {
        unsigned long  int_clear_gs_rcv_gm : 16; /* bit[0-15] : 逻辑自清零。
                                                                中断标志清除指示，高有效。用于清除INT_TYPE_GS_RCV_GM[15:0]对应bit位
                                                                生效时刻：立即生效 */
        unsigned long  reserved            : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_INT_CLEAR_GS_RCV_GM_UNION;
#define BBPCOMM_INT_CLEAR_GS_RCV_GM_int_clear_gs_rcv_gm_START  (0)
#define BBPCOMM_INT_CLEAR_GS_RCV_GM_int_clear_gs_rcv_gm_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_INT_CLEAR_GS_RCV_GS_UNION
 结构说明  : INT_CLEAR_GS_RCV_GS 寄存器结构定义。地址偏移量:0x0894，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      int_clear_gs_rcv_gs_reg;
    struct
    {
        unsigned long  int_clear_gs_rcv_gs : 16; /* bit[0-15] : 逻辑自清零。
                                                                中断标志清除指示，高有效。用于清除INT_TYPE_GS_RCV_GS[15:0]对应bit位
                                                                生效时刻：立即生效 */
        unsigned long  reserved            : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_INT_CLEAR_GS_RCV_GS_UNION;
#define BBPCOMM_INT_CLEAR_GS_RCV_GS_int_clear_gs_rcv_gs_START  (0)
#define BBPCOMM_INT_CLEAR_GS_RCV_GS_int_clear_gs_rcv_gs_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_INT_CLEAR_GS_RCV_W_UNION
 结构说明  : INT_CLEAR_GS_RCV_W 寄存器结构定义。地址偏移量:0x0898，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      int_clear_gs_rcv_w_reg;
    struct
    {
        unsigned long  int_clear_gs_rcv_w : 16; /* bit[0-15] : 逻辑自清零。
                                                               中断标志清除指示，高有效。用于清除INT_TYPE_GS_RCV_W[15:0]对应bit位
                                                               生效时刻：立即生效 */
        unsigned long  reserved           : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_INT_CLEAR_GS_RCV_W_UNION;
#define BBPCOMM_INT_CLEAR_GS_RCV_W_int_clear_gs_rcv_w_START  (0)
#define BBPCOMM_INT_CLEAR_GS_RCV_W_int_clear_gs_rcv_w_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_INT_CLEAR_GS_INT012_UNION
 结构说明  : INT_CLEAR_GS_INT012 寄存器结构定义。地址偏移量:0x089C，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      int_clear_gs_int012_reg;
    struct
    {
        unsigned long  int_clear_gs_int012 : 3;  /* bit[0-2] : 逻辑自清零。
                                                               中断标志清除指示，高有效。用于清除INT_TYPE_GS_INT012[2:0]对应bit位
                                                               生效时刻：立即生效 */
        unsigned long  reserved            : 29; /* bit[3-31]: 保留 */
    } reg;
} BBPCOMM_INT_CLEAR_GS_INT012_UNION;
#define BBPCOMM_INT_CLEAR_GS_INT012_int_clear_gs_int012_START  (0)
#define BBPCOMM_INT_CLEAR_GS_INT012_int_clear_gs_int012_END    (2)


/*****************************************************************************
 结构名    : BBPCOMM_INT_MASK_GS_RCV_LTE_UNION
 结构说明  : INT_MASK_GS_RCV_LTE 寄存器结构定义。地址偏移量:0x08A0，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      int_mask_gs_rcv_lte_reg;
    struct
    {
        unsigned long  int_mask_gs_rcv_lte : 16; /* bit[0-15] : 中断屏蔽标志。用于屏蔽INT_TYPE_GS_RCV_LTE[15:0]对应bit中断。
                                                                0：屏蔽中断上报；
                                                                1：使能中断上报。
                                                                配置时刻：初始化
                                                                生效时刻：立即生效 */
        unsigned long  reserved            : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_INT_MASK_GS_RCV_LTE_UNION;
#define BBPCOMM_INT_MASK_GS_RCV_LTE_int_mask_gs_rcv_lte_START  (0)
#define BBPCOMM_INT_MASK_GS_RCV_LTE_int_mask_gs_rcv_lte_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_INT_MASK_GS_RCV_TDS_UNION
 结构说明  : INT_MASK_GS_RCV_TDS 寄存器结构定义。地址偏移量:0x08A4，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      int_mask_gs_rcv_tds_reg;
    struct
    {
        unsigned long  int_mask_gs_rcv_tds : 16; /* bit[0-15] : 中断屏蔽标志。用于屏蔽INT_TYPE_GS_RCV_TDS[15:0]对应bit中断。
                                                                0：屏蔽中断上报；
                                                                1：使能中断上报。
                                                                配置时刻：初始化
                                                                生效时刻：立即生效 */
        unsigned long  reserved            : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_INT_MASK_GS_RCV_TDS_UNION;
#define BBPCOMM_INT_MASK_GS_RCV_TDS_int_mask_gs_rcv_tds_START  (0)
#define BBPCOMM_INT_MASK_GS_RCV_TDS_int_mask_gs_rcv_tds_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_INT_MASK_GS_RCV_GM_UNION
 结构说明  : INT_MASK_GS_RCV_GM 寄存器结构定义。地址偏移量:0x08A8，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      int_mask_gs_rcv_gm_reg;
    struct
    {
        unsigned long  int_mask_gs_rcv_gm : 16; /* bit[0-15] : 中断屏蔽标志。用于屏蔽INT_TYPE_GS_RCV_GM[15:0]对应bit中断。
                                                               0：屏蔽中断上报；
                                                               1：使能中断上报。
                                                               配置时刻：初始化
                                                               生效时刻：立即生效 */
        unsigned long  reserved           : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_INT_MASK_GS_RCV_GM_UNION;
#define BBPCOMM_INT_MASK_GS_RCV_GM_int_mask_gs_rcv_gm_START  (0)
#define BBPCOMM_INT_MASK_GS_RCV_GM_int_mask_gs_rcv_gm_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_INT_MASK_GS_RCV_GS_UNION
 结构说明  : INT_MASK_GS_RCV_GS 寄存器结构定义。地址偏移量:0x08AC，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      int_mask_gs_rcv_gs_reg;
    struct
    {
        unsigned long  int_mask_gs_rcv_gs : 16; /* bit[0-15] : 中断屏蔽标志。用于屏蔽INT_TYPE_GS_RCV_GS[15:0]对应bit中断。
                                                               0：屏蔽中断上报；
                                                               1：使能中断上报。
                                                               配置时刻：初始化
                                                               生效时刻：立即生效 */
        unsigned long  reserved           : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_INT_MASK_GS_RCV_GS_UNION;
#define BBPCOMM_INT_MASK_GS_RCV_GS_int_mask_gs_rcv_gs_START  (0)
#define BBPCOMM_INT_MASK_GS_RCV_GS_int_mask_gs_rcv_gs_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_INT_MASK_GS_RCV_W_UNION
 结构说明  : INT_MASK_GS_RCV_W 寄存器结构定义。地址偏移量:0x08B0，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      int_mask_gs_rcv_w_reg;
    struct
    {
        unsigned long  int_mask_gs_rcv_w : 16; /* bit[0-15] : 中断屏蔽标志。用于屏蔽INT_TYPE_GS_RCV_W[15:0]对应bit中断。
                                                              0：屏蔽中断上报；
                                                              1：使能中断上报。
                                                              配置时刻：初始化
                                                              生效时刻：立即生效 */
        unsigned long  reserved          : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_INT_MASK_GS_RCV_W_UNION;
#define BBPCOMM_INT_MASK_GS_RCV_W_int_mask_gs_rcv_w_START  (0)
#define BBPCOMM_INT_MASK_GS_RCV_W_int_mask_gs_rcv_w_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_INT_MASK_GS_INT012_UNION
 结构说明  : INT_MASK_GS_INT012 寄存器结构定义。地址偏移量:0x08B4，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      int_mask_gs_int012_reg;
    struct
    {
        unsigned long  int_mask_gs_int012 : 3;  /* bit[0-2] : 中断屏蔽标志。用于屏蔽INT_TYPE_GS_INT012[2:0]对应bit中断。
                                                              0：屏蔽中断上报；
                                                              1：使能中断上报。
                                                              配置时刻：初始化
                                                              生效时刻：立即生效 */
        unsigned long  reserved           : 29; /* bit[3-31]: 保留 */
    } reg;
} BBPCOMM_INT_MASK_GS_INT012_UNION;
#define BBPCOMM_INT_MASK_GS_INT012_int_mask_gs_int012_START  (0)
#define BBPCOMM_INT_MASK_GS_INT012_int_mask_gs_int012_END    (2)


/*****************************************************************************
 结构名    : BBPCOMM_INT_TYPE_GS_RCV_LTE_UNION
 结构说明  : INT_TYPE_GS_RCV_LTE 寄存器结构定义。地址偏移量:0x08B8，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      int_type_gs_rcv_lte_reg;
    struct
    {
        unsigned long  o_int_type_gs_rcv_lte : 16; /* bit[0-15] : 中断状态标志。各bit单独对应一种中断类型，高有效。
                                                                  [0]: 中断类型0。 LTE（主模）提前通知GS异系统测量启动中断。
                                                                  [1]: 中断类型1。 LTE（主模）进GAP中断，通知GS进行异系统测量射频通道配置，天线切换等操作。
                                                                  [2]: 中断类型2。 LTE（主模）出GAP中断。
                                                                  [3]: 中断类型3。LTE（从模）测量结束中断。
                                                                  [4]: 中断类型4。LTE（主模）强制GS退出异系统测量中断。
                                                                  [5]: 中断类型5。LTE（主模）通知GS上报本次测量结果。
                                                                  [6]: 保留。
                                                                  [15:7]: 对应中断类型[15:7]，预留。
                                                                  软件读取该中断状态上报寄存器后，需要和0x08A0的int_mask_gs_rcv_lte相与，再判断是否需要处理。 */
        unsigned long  reserved              : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_INT_TYPE_GS_RCV_LTE_UNION;
#define BBPCOMM_INT_TYPE_GS_RCV_LTE_o_int_type_gs_rcv_lte_START  (0)
#define BBPCOMM_INT_TYPE_GS_RCV_LTE_o_int_type_gs_rcv_lte_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_INT_TYPE_GS_RCV_TDS_UNION
 结构说明  : INT_TYPE_GS_RCV_TDS 寄存器结构定义。地址偏移量:0x08BC，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      int_type_gs_rcv_tds_reg;
    struct
    {
        unsigned long  o_int_type_gs_rcv_tds : 16; /* bit[0-15] : 中断状态标志0，1，2只有当TDS处于主模时才能产生。
                                                                  
                                                                  [0]: 中断类型0。 TDS（主模）提前通知GS异系统测量启动中断。
                                                                  [1]: 中断类型1。 TDS（主模）进GAP中断，通知GS进行异系统测量射频通道配置，天线切换等操作。
                                                                  [2]: 中断类型2。 TDS（主模）出GAP中断。
                                                                  [3]: 中断类型3。TDS（从模）测量结束中断。
                                                                  [4]: 中断类型4。TDS（主模）强制GS退出异系统测量中断。
                                                                  [5]: 中断类型5。TDS（主模）通知GS上报本次测量结果。
                                                                  [6]: 中断类型6。保留。
                                                                  [15:7]: 对应中断类型[15:7]，预留。
                                                                  软件读取该中断状态上报寄存器后，需要和0x08A4的int_mask_gs_rcv_tds相与，再判断是否需要处理。 */
        unsigned long  reserved              : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_INT_TYPE_GS_RCV_TDS_UNION;
#define BBPCOMM_INT_TYPE_GS_RCV_TDS_o_int_type_gs_rcv_tds_START  (0)
#define BBPCOMM_INT_TYPE_GS_RCV_TDS_o_int_type_gs_rcv_tds_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_INT_TYPE_GS_RCV_GM_UNION
 结构说明  : INT_TYPE_GS_RCV_GM 寄存器结构定义。地址偏移量:0x08C0，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      int_type_gs_rcv_gm_reg;
    struct
    {
        unsigned long  o_int_type_gs_rcv_gm : 16; /* bit[0-15] : 中断状态标志。各bit单独对应一种中断类型，高有效。
                                                                 [0]: 中断类型0。GM（主模）提前通知GS异系统测量启动中断。
                                                                 [1]: 中断类型1。GM（主模）进GAP中断，通知GS进行异系统测量射频通道配置，天线切换等操作。
                                                                 [2]: 中断类型2。GM（主模）出GAP中断。
                                                                 [3]: 中断类型3。GM（从模）测量结束中断。
                                                                 [4]: 中断类型4。GM（主模）强制GS退出异系统测量中断。
                                                                 [5]: 中断类型5。GM（主模）通知GS上报本次测量结果。
                                                                 [6]: 中断类型6。保留。
                                                                 [15:7]: 对应中断类型[15:7]，预留。
                                                                 中断状态标志0，1，2只有当GM处于主模时才能产生。
                                                                 软件读取该中断状态上报寄存器后，需要和0x08A8的int_mask_gs_rcv_gm相与，再判断是否需要处理。 */
        unsigned long  reserved             : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_INT_TYPE_GS_RCV_GM_UNION;
#define BBPCOMM_INT_TYPE_GS_RCV_GM_o_int_type_gs_rcv_gm_START  (0)
#define BBPCOMM_INT_TYPE_GS_RCV_GM_o_int_type_gs_rcv_gm_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_INT_TYPE_GS_RCV_GS_UNION
 结构说明  : INT_TYPE_GS_RCV_GS 寄存器结构定义。地址偏移量:0x08C4，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      int_type_gs_rcv_gs_reg;
    struct
    {
        unsigned long  o_int_type_gs_rcv_gs : 16; /* bit[0-15] : [0]: 中断类型0。GS（主模）提前通知GS异系统测量启动中断。
                                                                 [1]: 中断类型1。GS（主模）进GAP中断，通知GS进行异系统测量射频通道配置，天线切换等操作。
                                                                 [2]: 中断类型2。GS（主模）出GAP中断。
                                                                 [3]: 中断类型3。GS（从模）测量结束中断。
                                                                 [4]: 中断类型4。GS（主模）强制GS退出异系统测量中断。
                                                                 [5]: 中断类型5。GS（主模）通知GS上报本次测量结果。
                                                                 [6]: 中断类型6。保留。
                                                                 [15:7]: 对应中断类型[15:7]，预留。
                                                                 中断状态标志0，1，2只有当TDS处于主模时才能产生。
                                                                 软件读取该中断状态上报寄存器后，需要和0x08AC的int_mask_gs_rcv_gs相与，再判断是否需要处理。 */
        unsigned long  reserved             : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_INT_TYPE_GS_RCV_GS_UNION;
#define BBPCOMM_INT_TYPE_GS_RCV_GS_o_int_type_gs_rcv_gs_START  (0)
#define BBPCOMM_INT_TYPE_GS_RCV_GS_o_int_type_gs_rcv_gs_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_INT_TYPE_GS_RCV_W_UNION
 结构说明  : INT_TYPE_GS_RCV_W 寄存器结构定义。地址偏移量:0x08C8，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      int_type_gs_rcv_w_reg;
    struct
    {
        unsigned long  o_int_type_gs_rcv_w : 16; /* bit[0-15] : 中断状态标志。各bit单独对应一种中断类型，高有效。
                                                                [0]: 中断类型0。W（主模）提前通知GS异频测量启动中断。
                                                                [1]: 中断类型1。W（主模）进GAP中断，通知GS进行异频测量射频通道配置，天线切换等操作。
                                                                [2]: 中断类型2。W（主模）出GAP中断。
                                                                [3]: 中断类型3。W（从模，即异频）测量结束中断。
                                                                [4]: 中断类型4。W（主模）强制GS退出异频测量中断。
                                                                [5]: 中断类型5。W（主模）通知GS上报本次测量结果。
                                                                [6]: 中断类型6。保留。
                                                                [15:7]: 对应中断类型[15:7]，预留。
                                                                中断状态标志0，1，2只有当W处于主模时才能产生。
                                                                软件读取该中断状态上报寄存器后，需要和0x08B0的int_mask_gs_rcv_w相与，再判断是否需要处理。 */
        unsigned long  reserved            : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_INT_TYPE_GS_RCV_W_UNION;
#define BBPCOMM_INT_TYPE_GS_RCV_W_o_int_type_gs_rcv_w_START  (0)
#define BBPCOMM_INT_TYPE_GS_RCV_W_o_int_type_gs_rcv_w_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_INT_TYPE_GS_INT012_UNION
 结构说明  : INT_TYPE_GS_INT012 寄存器结构定义。地址偏移量:0x08CC，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      int_type_gs_int012_reg;
    struct
    {
        unsigned long  o_int_type_gs_int012 : 3;  /* bit[0-2] : 主模产生int0、1、2的上报（(包括异频、异系统场景)），高有效。
                                                                [0]: GS主模时，CTU告诉通知GSDSP产生int0中断。
                                                                [1]: GS主模时，CTU告诉通知GSDSP产生int1中断。
                                                                [2]: GS主模时，CTU告诉通知GSDSP产生int2中断。
                                                                软件读取该中断状态上报寄存器后，需要和0x08B4的int_mask_gs_int012相与，再判断是否需要处理。 */
        unsigned long  reserved             : 29; /* bit[3-31]: 保留 */
    } reg;
} BBPCOMM_INT_TYPE_GS_INT012_UNION;
#define BBPCOMM_INT_TYPE_GS_INT012_o_int_type_gs_int012_START  (0)
#define BBPCOMM_INT_TYPE_GS_INT012_o_int_type_gs_int012_END    (2)


/*****************************************************************************
 结构名    : BBPCOMM_GS_SND_LTE_INT_13_UNION
 结构说明  : GS_SND_LTE_INT_13 寄存器结构定义。地址偏移量:0x08D0，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      gs_snd_lte_int_13_reg;
    struct
    {
        unsigned long  reserved_0        : 3;  /* bit[0-2]  : 保留 */
        unsigned long  gs_snd_lte_int_13 : 13; /* bit[3-15] : 逻辑自清零。高有效。
                                                              
                                                              bit15~7：对应gs_snd_lte_int_15 ~ 7。
                                                              GS DSP通过此寄存器触发类型15~7中断（int_type_lte_rcv_gs[15:7]）给LTEDSP。
                                                              生效时刻：立即生效
                                                              
                                                              bit6：对应gs_snd_lte_int_6。
                                                              GSDSP通过此寄存器触发类型6中断（int_type_lte_rcv_gs[6]）给LTEDSP。
                                                              生效时刻：立即生效
                                                              
                                                              bit5：对应gs_snd_lte_int_5。
                                                              GS主模时通知LTEDSP LTE测量周期到达，高有效。
                                                              GS DSP通过此寄存器触发类型5中断（int_type_lte_rcv_gs[5]）给LTEDSP。
                                                              生效时刻：立即生效
                                                              
                                                              bit4：对应gs_snd_lte_int_4。
                                                              GS主模时强制退出异系统测量指示。高有效。
                                                              GS DSP通过此寄存器触发类型4中断（int_type_lte_rcv_gs[4]）给LTEDSP。
                                                              生效时刻：立即生效
                                                              
                                                              bit3：对应gs_snd_lte_int_3。GS测量结束指示，高有效。
                                                              用于触发中断类型3 （int_type_lte_rcv_gs[3]）给LTEDSP。LTE主模下对GS进行异系统测量时，如果GS测量结束，GS DSP配置此标志为1。 
                                                              生效时刻：立即生效 */
        unsigned long  reserved_1        : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_GS_SND_LTE_INT_13_UNION;
#define BBPCOMM_GS_SND_LTE_INT_13_gs_snd_lte_int_13_START  (3)
#define BBPCOMM_GS_SND_LTE_INT_13_gs_snd_lte_int_13_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_GS_SND_TDS_INT_13_UNION
 结构说明  : GS_SND_TDS_INT_13 寄存器结构定义。地址偏移量:0x08D4，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      gs_snd_tds_int_13_reg;
    struct
    {
        unsigned long  reserved_0        : 3;  /* bit[0-2]  : 保留 */
        unsigned long  gs_snd_tds_int_13 : 13; /* bit[3-15] : 逻辑自清零。高有效。
                                                              
                                                              bit15~7：对应gs_snd_tds_int_15 ~ 7。
                                                              GS DSP通过此寄存器触发类型15~7中断（int_type_tds_rcv_gs[15:7]）给TDSDSP。
                                                              生效时刻：立即生效
                                                              
                                                              bit6：对应gs_snd_tds_int_6。
                                                              GSDSP通过此寄存器触发类型6中断（int_type_tds_rcv_gs[6]）给TDSDSP。
                                                              生效时刻：立即生效
                                                              
                                                              bit5：对应gs_snd_tds_int_5。
                                                              GS主模时通知TDSDSP TDS测量周期到达，高有效。
                                                              GS DSP通过此寄存器触发类型5中断（int_type_tds_rcv_gs[5]）给TDSDSP。
                                                              生效时刻：立即生效
                                                              
                                                              bit4：对应gs_snd_tds_int_4。
                                                              GS主模时强制退出异系统测量指示。高有效。
                                                              GS DSP通过此寄存器触发类型4中断（int_type_tds_rcv_gs[4]）给TDSDSP。
                                                              生效时刻：立即生效
                                                              
                                                              bit3：对应gs_snd_tds_int_3。GS测量结束指示，高有效。
                                                              用于触发中断类型3 （int_type_tds_rcv_gs[3]）给TDSDSP。TDS主模下对GS进行异系统测量时，如果GS测量结束，GS DSP配置此标志为1。 
                                                              生效时刻：立即生效 */
        unsigned long  reserved_1        : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_GS_SND_TDS_INT_13_UNION;
#define BBPCOMM_GS_SND_TDS_INT_13_gs_snd_tds_int_13_START  (3)
#define BBPCOMM_GS_SND_TDS_INT_13_gs_snd_tds_int_13_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_GS_SND_GM_INT_13_UNION
 结构说明  : GS_SND_GM_INT_13 寄存器结构定义。地址偏移量:0x08D8，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      gs_snd_gm_int_13_reg;
    struct
    {
        unsigned long  reserved_0       : 3;  /* bit[0-2]  : 保留 */
        unsigned long  gs_snd_gm_int_13 : 13; /* bit[3-15] : 逻辑自清零。高有效。
                                                             
                                                             bit15~7：对应gs_snd_gm_int_15 ~ 7。
                                                             GS DSP通过此寄存器触发类型15~7中断（int_type_gm_rcv_gs[15:7]）给GMDSP。
                                                             生效时刻：立即生效
                                                             
                                                             bit6：对应gs_snd_gm_int_6。
                                                             GSDSP通过此寄存器触发类型6中断（int_type_gm_rcv_gs[6]）给GMDSP。
                                                             生效时刻：立即生效
                                                             
                                                             bit5：对应gs_snd_gm_int_5。
                                                             GS主模时通知GMDSP GM测量周期到达，高有效。
                                                             GS DSP通过此寄存器触发类型5中断（int_type_gm_rcv_gs[5]）给GMDSP。
                                                             生效时刻：立即生效
                                                             
                                                             bit4：对应gs_snd_gm_int_4。
                                                             GS主模时强制退出异系统测量指示。高有效。
                                                             GS DSP通过此寄存器触发类型4中断（int_type_gm_rcv_gs[4]）给GMDSP。
                                                             生效时刻：立即生效
                                                             
                                                             bit3：对应gs_snd_gm_int_3。GS测量结束指示，高有效。
                                                             用于触发中断类型3 （int_type_gm_rcv_gs[3]）给GMDSP。GM主模下对GS进行异系统测量时，如果GS测量结束，GS DSP配置此标志为1。 
                                                             生效时刻：立即生效 */
        unsigned long  reserved_1       : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_GS_SND_GM_INT_13_UNION;
#define BBPCOMM_GS_SND_GM_INT_13_gs_snd_gm_int_13_START  (3)
#define BBPCOMM_GS_SND_GM_INT_13_gs_snd_gm_int_13_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_GS_SND_GS_INT_13_UNION
 结构说明  : GS_SND_GS_INT_13 寄存器结构定义。地址偏移量:0x08DC，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      gs_snd_gs_int_13_reg;
    struct
    {
        unsigned long  reserved_0       : 3;  /* bit[0-2]  : 保留 */
        unsigned long  gs_snd_gs_int_13 : 13; /* bit[3-15] : 逻辑自清零。高有效。
                                                             
                                                             bit15~7：对应gs_snd_gs_int_15 ~ 7。
                                                             GS DSP通过此寄存器触发类型15~7中断（int_type_gs_rcv_gs[15:7]）给GSDSP。
                                                             生效时刻：立即生效
                                                             
                                                             bit6：对应gs_snd_gs_int_6。
                                                             GSDSP通过此寄存器触发类型6中断（int_type_gs_rcv_gs[6]）给GSDSP。
                                                             生效时刻：立即生效
                                                             
                                                             bit5：对应gs_snd_gs_int_5。
                                                             GS主模时通知GSDSP GS测量周期到达，高有效。
                                                             GS DSP通过此寄存器触发类型5中断（int_type_gs_rcv_gs[5]）给GSDSP。
                                                             生效时刻：立即生效
                                                             
                                                             bit4：对应gs_snd_gs_int_4。
                                                             GS主模时强制退出异系统测量指示。高有效。
                                                             GS DSP通过此寄存器触发类型4中断（int_type_gs_rcv_gs[4]）给GSDSP。
                                                             生效时刻：立即生效
                                                             
                                                             bit3：对应gs_snd_gs_int_3。GS测量结束指示，高有效。
                                                             用于触发中断类型3 （int_type_gs_rcv_gs[3]）给GSDSP。GS主模下对GS进行异系统测量时，如果GS测量结束，GS DSP配置此标志为1。 
                                                             生效时刻：立即生效 */
        unsigned long  reserved_1       : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_GS_SND_GS_INT_13_UNION;
#define BBPCOMM_GS_SND_GS_INT_13_gs_snd_gs_int_13_START  (3)
#define BBPCOMM_GS_SND_GS_INT_13_gs_snd_gs_int_13_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_GS_SND_W_INT_13_UNION
 结构说明  : GS_SND_W_INT_13 寄存器结构定义。地址偏移量:0x08E0，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      gs_snd_w_int_13_reg;
    struct
    {
        unsigned long  reserved_0      : 3;  /* bit[0-2]  : 保留 */
        unsigned long  gs_snd_w_int_13 : 13; /* bit[3-15] : 逻辑自清零。高有效。
                                                            
                                                            bit15~7：对应gs_snd_w_int_15 ~ 7。
                                                            GS DSP通过此寄存器触发类型15~7中断（int_type_w_rcv_gs[15:7]）给WDSP。
                                                            生效时刻：立即生效
                                                            
                                                            bit6：对应gs_snd_w_int_6。
                                                            GSDSP通过此寄存器触发类型6中断（int_type_w_rcv_gs[6]）给WDSP。
                                                            生效时刻：立即生效
                                                            
                                                            bit5：对应gs_snd_w_int_5。
                                                            GS主模时通知WDSP W测量周期到达，高有效。
                                                            GS DSP通过此寄存器触发类型5中断（int_type_w_rcv_gs[5]）给WDSP。
                                                            生效时刻：立即生效
                                                            
                                                            bit4：对应gs_snd_w_int_4。
                                                            GS主模时强制退出异系统测量指示。高有效。
                                                            GS DSP通过此寄存器触发类型4中断（int_type_w_rcv_gs[4]）给WDSP。
                                                            生效时刻：立即生效
                                                            
                                                            bit3：对应gs_snd_w_int_3。GS测量结束指示，高有效。
                                                            用于触发中断类型3 （int_type_w_rcv_gs[3]）给WDSP。W主模下对GS进行异系统测量时，如果GS测量结束，GS DSP配置此标志为1。 
                                                            生效时刻：立即生效 */
        unsigned long  reserved_1      : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_GS_SND_W_INT_13_UNION;
#define BBPCOMM_GS_SND_W_INT_13_gs_snd_w_int_13_START  (3)
#define BBPCOMM_GS_SND_W_INT_13_gs_snd_w_int_13_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_GBBP1_19M_SEL_UNION
 结构说明  : GBBP1_19M_SEL 寄存器结构定义。地址偏移量:0x08E4，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      gbbp1_19m_sel_reg;
    struct
    {
        unsigned long  gbbp1_19m_sel : 2;  /* bit[0-1] : V9R1版本，软件不配置。 */
        unsigned long  reserved      : 30; /* bit[2-31]: 保留 */
    } reg;
} BBPCOMM_GBBP1_19M_SEL_UNION;
#define BBPCOMM_GBBP1_19M_SEL_gbbp1_19m_sel_START  (0)
#define BBPCOMM_GBBP1_19M_SEL_gbbp1_19m_sel_END    (1)


/*****************************************************************************
 结构名    : BBPCOMM_GBBP2_19M_SEL_UNION
 结构说明  : GBBP2_19M_SEL 寄存器结构定义。地址偏移量:0x08E8，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      gbbp2_19m_sel_reg;
    struct
    {
        unsigned long  gbbp2_19m_sel : 2;  /* bit[0-1] : V9R1版本，软件不配置。 */
        unsigned long  reserved      : 30; /* bit[2-31]: 保留 */
    } reg;
} BBPCOMM_GBBP2_19M_SEL_UNION;
#define BBPCOMM_GBBP2_19M_SEL_gbbp2_19m_sel_START  (0)
#define BBPCOMM_GBBP2_19M_SEL_gbbp2_19m_sel_END    (1)


/*****************************************************************************
 结构名    : BBPCOMM_WBBP_19M_SEL_UNION
 结构说明  : WBBP_19M_SEL 寄存器结构定义。地址偏移量:0x08EC，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      wbbp_19m_sel_reg;
    struct
    {
        unsigned long  wbbp_19m_sel : 2;  /* bit[0-1] : V9R1版本，软件不配置。 */
        unsigned long  reserved     : 30; /* bit[2-31]: 保留 */
    } reg;
} BBPCOMM_WBBP_19M_SEL_UNION;
#define BBPCOMM_WBBP_19M_SEL_wbbp_19m_sel_START  (0)
#define BBPCOMM_WBBP_19M_SEL_wbbp_19m_sel_END    (1)


/*****************************************************************************
 结构名    : BBPCOMM_LBBP_19M_SEL_UNION
 结构说明  : LBBP_19M_SEL 寄存器结构定义。地址偏移量:0x08F0，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      lbbp_19m_sel_reg;
    struct
    {
        unsigned long  lbbp_19m_sel : 2;  /* bit[0-1] : V9R1版本，软件不配置。 */
        unsigned long  reserved     : 30; /* bit[2-31]: 保留 */
    } reg;
} BBPCOMM_LBBP_19M_SEL_UNION;
#define BBPCOMM_LBBP_19M_SEL_lbbp_19m_sel_START  (0)
#define BBPCOMM_LBBP_19M_SEL_lbbp_19m_sel_END    (1)


/*****************************************************************************
 结构名    : BBPCOMM_TBBP1_19M_SEL_UNION
 结构说明  : TBBP1_19M_SEL 寄存器结构定义。地址偏移量:0x08F4，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      tbbp1_19m_sel_reg;
    struct
    {
        unsigned long  tbbp_19m_sel : 2;  /* bit[0-1] : V9R1版本，软件不配置。 */
        unsigned long  reserved     : 30; /* bit[2-31]: 保留 */
    } reg;
} BBPCOMM_TBBP1_19M_SEL_UNION;
#define BBPCOMM_TBBP1_19M_SEL_tbbp_19m_sel_START  (0)
#define BBPCOMM_TBBP1_19M_SEL_tbbp_19m_sel_END    (1)


/*****************************************************************************
 结构名    : BBPCOMM_GBBP1_CLK_SEL_UNION
 结构说明  : GBBP1_CLK_SEL 寄存器结构定义。地址偏移量:0x08F8，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      gbbp1_clk_sel_reg;
    struct
    {
        unsigned long  gbbp1_clk_sel : 2;  /* bit[0-1] : V9R1版本，软件不配置。 */
        unsigned long  reserved      : 30; /* bit[2-31]: 保留 */
    } reg;
} BBPCOMM_GBBP1_CLK_SEL_UNION;
#define BBPCOMM_GBBP1_CLK_SEL_gbbp1_clk_sel_START  (0)
#define BBPCOMM_GBBP1_CLK_SEL_gbbp1_clk_sel_END    (1)


/*****************************************************************************
 结构名    : BBPCOMM_GBBP2_CLK_SEL_UNION
 结构说明  : GBBP2_CLK_SEL 寄存器结构定义。地址偏移量:0x08FC，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      gbbp2_clk_sel_reg;
    struct
    {
        unsigned long  gbbp2_clk_sel : 2;  /* bit[0-1] : V9R1版本，软件不配置。 */
        unsigned long  reserved      : 30; /* bit[2-31]: 保留 */
    } reg;
} BBPCOMM_GBBP2_CLK_SEL_UNION;
#define BBPCOMM_GBBP2_CLK_SEL_gbbp2_clk_sel_START  (0)
#define BBPCOMM_GBBP2_CLK_SEL_gbbp2_clk_sel_END    (1)


/*****************************************************************************
 结构名    : BBPCOMM_WBBP_CLK_SEL_UNION
 结构说明  : WBBP_CLK_SEL 寄存器结构定义。地址偏移量:0x0900，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      wbbp_clk_sel_reg;
    struct
    {
        unsigned long  wbbp_clk_sel : 2;  /* bit[0-1] : V9R1版本，软件不配置。 */
        unsigned long  reserved     : 30; /* bit[2-31]: 保留 */
    } reg;
} BBPCOMM_WBBP_CLK_SEL_UNION;
#define BBPCOMM_WBBP_CLK_SEL_wbbp_clk_sel_START  (0)
#define BBPCOMM_WBBP_CLK_SEL_wbbp_clk_sel_END    (1)


/*****************************************************************************
 结构名    : BBPCOMM_LBBP_CLK_SEL_UNION
 结构说明  : LBBP_CLK_SEL 寄存器结构定义。地址偏移量:0x0904，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      lbbp_clk_sel_reg;
    struct
    {
        unsigned long  lbbp_clk_sel : 2;  /* bit[0-1] : V9R1版本，软件不配置。 */
        unsigned long  reserved     : 30; /* bit[2-31]: 保留 */
    } reg;
} BBPCOMM_LBBP_CLK_SEL_UNION;
#define BBPCOMM_LBBP_CLK_SEL_lbbp_clk_sel_START  (0)
#define BBPCOMM_LBBP_CLK_SEL_lbbp_clk_sel_END    (1)


/*****************************************************************************
 结构名    : BBPCOMM_TBBP_CLK_SEL_UNION
 结构说明  : TBBP_CLK_SEL 寄存器结构定义。地址偏移量:0x0908，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      tbbp_clk_sel_reg;
    struct
    {
        unsigned long  tbbp_clk_sel : 2;  /* bit[0-1] : V9R1版本，软件不配置。 */
        unsigned long  reserved     : 30; /* bit[2-31]: 保留 */
    } reg;
} BBPCOMM_TBBP_CLK_SEL_UNION;
#define BBPCOMM_TBBP_CLK_SEL_tbbp_clk_sel_START  (0)
#define BBPCOMM_TBBP_CLK_SEL_tbbp_clk_sel_END    (1)


/*****************************************************************************
 结构名    : BBPCOMM_LTE_ABBIF_FMT_UNION
 结构说明  : LTE_ABBIF_FMT 寄存器结构定义。地址偏移量:0x090C，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      lte_abbif_fmt_reg;
    struct
    {
        unsigned long  reserved_0          : 1;  /* bit[0-0]  : 保留 */
        unsigned long  lte_rxb_iq_exchange : 1;  /* bit[1-1]  : 1'b1：LTE的RXB通路IQ路数据交换
                                                                1'b0：LTE的RXB通路IQ路数据不交换
                                                                V3R3不支持IQ交换功能。 */
        unsigned long  lte_rxb_q_inv       : 1;  /* bit[2-2]  : 1'b1：LTE的RXB通路Q路数据取反
                                                                1'b0：LTE的RXB通路Q路数据不取反 */
        unsigned long  lte_rxb_i_inv       : 1;  /* bit[3-3]  : 1'b1：LTE的RXB通路I路数据取反
                                                                1'b0：LTE的RXB通路I路数据不取反 */
        unsigned long  lte_rxa_iq_exchange : 1;  /* bit[4-4]  : 1'b1：LTE的RXA通路IQ路数据交换
                                                                1'b0：LTE的RXA通路IQ路数据不交换
                                                                V3R3不支持IQ交换功能。 */
        unsigned long  lte_rxa_q_inv       : 1;  /* bit[5-5]  : 1'b1：LTE的RXA通路Q路数据取反
                                                                1'b0：LTE的RXA通路Q路数据不取反 */
        unsigned long  lte_rxa_i_inv       : 1;  /* bit[6-6]  : 1'b1：LTE的RXA通路I路数据取反
                                                                1'b0：LTE的RXA通路I路数据不取反 */
        unsigned long  lte_tx_iq_exchange  : 1;  /* bit[7-7]  : 1'b1：LTE的TX通路IQ路数据交换
                                                                1'b0：LTE的TX通路IQ路数据不交换
                                                                V3R3不支持IQ交换功能。 */
        unsigned long  lte_tx_q_inv        : 1;  /* bit[8-8]  : 1'b1：LTE的TX通路Q路数据取反
                                                                1'b0：LTE的TX通路Q路数据不取反 */
        unsigned long  lte_tx_i_inv        : 1;  /* bit[9-9]  : 1'b1：LTE的TX通路I路数据取反
                                                                1'b0：LTE的TX通路I路数据不取反 */
        unsigned long  reserved_1          : 22; /* bit[10-31]: 保留 */
    } reg;
} BBPCOMM_LTE_ABBIF_FMT_UNION;
#define BBPCOMM_LTE_ABBIF_FMT_lte_rxb_iq_exchange_START  (1)
#define BBPCOMM_LTE_ABBIF_FMT_lte_rxb_iq_exchange_END    (1)
#define BBPCOMM_LTE_ABBIF_FMT_lte_rxb_q_inv_START        (2)
#define BBPCOMM_LTE_ABBIF_FMT_lte_rxb_q_inv_END          (2)
#define BBPCOMM_LTE_ABBIF_FMT_lte_rxb_i_inv_START        (3)
#define BBPCOMM_LTE_ABBIF_FMT_lte_rxb_i_inv_END          (3)
#define BBPCOMM_LTE_ABBIF_FMT_lte_rxa_iq_exchange_START  (4)
#define BBPCOMM_LTE_ABBIF_FMT_lte_rxa_iq_exchange_END    (4)
#define BBPCOMM_LTE_ABBIF_FMT_lte_rxa_q_inv_START        (5)
#define BBPCOMM_LTE_ABBIF_FMT_lte_rxa_q_inv_END          (5)
#define BBPCOMM_LTE_ABBIF_FMT_lte_rxa_i_inv_START        (6)
#define BBPCOMM_LTE_ABBIF_FMT_lte_rxa_i_inv_END          (6)
#define BBPCOMM_LTE_ABBIF_FMT_lte_tx_iq_exchange_START   (7)
#define BBPCOMM_LTE_ABBIF_FMT_lte_tx_iq_exchange_END     (7)
#define BBPCOMM_LTE_ABBIF_FMT_lte_tx_q_inv_START         (8)
#define BBPCOMM_LTE_ABBIF_FMT_lte_tx_q_inv_END           (8)
#define BBPCOMM_LTE_ABBIF_FMT_lte_tx_i_inv_START         (9)
#define BBPCOMM_LTE_ABBIF_FMT_lte_tx_i_inv_END           (9)


/*****************************************************************************
 结构名    : BBPCOMM_W_ABBIF_FMT_UNION
 结构说明  : W_ABBIF_FMT 寄存器结构定义。地址偏移量:0x0910，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      w_abbif_fmt_reg;
    struct
    {
        unsigned long  reserved_0        : 1;  /* bit[0-0]  : 保留 */
        unsigned long  w_rxb_iq_exchange : 1;  /* bit[1-1]  : 1'b1：W的RXB通路IQ路数据交换
                                                              1'b0：W的RXB通路IQ路数据不交换
                                                              V3R3不支持IQ交换功能。 */
        unsigned long  w_rxb_q_inv       : 1;  /* bit[2-2]  : 1'b1：W的RXB通路Q路数据取反
                                                              1'b0：W的RXB通路Q路数据不取反 */
        unsigned long  w_rxb_i_inv       : 1;  /* bit[3-3]  : 1'b1：W的RXB通路I路数据取反
                                                              1'b0：W的RXB通路I路数据不取反 */
        unsigned long  w_rxa_iq_exchange : 1;  /* bit[4-4]  : 1'b1：W的RXA通路IQ路数据交换
                                                              1'b0：W的RXA通路IQ路数据不交换
                                                              V3R3不支持IQ交换功能。 */
        unsigned long  w_rxa_q_inv       : 1;  /* bit[5-5]  : 1'b1：W的RXA通路Q路数据取反
                                                              1'b0：W的RXA通路Q路数据不取反 */
        unsigned long  w_rxa_i_inv       : 1;  /* bit[6-6]  : 1'b1：W的RXA通路I路数据取反
                                                              1'b0：W的RXA通路I路数据不取反 */
        unsigned long  w_tx_iq_exchange  : 1;  /* bit[7-7]  : 1'b1：W的TX通路IQ路数据交换
                                                              1'b0：W的TX通路IQ路数据不交换
                                                              V3R3不支持IQ交换功能。 */
        unsigned long  w_tx_q_inv        : 1;  /* bit[8-8]  : 1'b1：W的TX通路Q路数据取反
                                                              1'b0：W的TX通路Q路数据不取反 */
        unsigned long  w_tx_i_inv        : 1;  /* bit[9-9]  : 1'b1：W的TX通路I路数据取反
                                                              1'b0：W的TX通路I路数据不取反。 */
        unsigned long  reserved_1        : 22; /* bit[10-31]: 保留 */
    } reg;
} BBPCOMM_W_ABBIF_FMT_UNION;
#define BBPCOMM_W_ABBIF_FMT_w_rxb_iq_exchange_START  (1)
#define BBPCOMM_W_ABBIF_FMT_w_rxb_iq_exchange_END    (1)
#define BBPCOMM_W_ABBIF_FMT_w_rxb_q_inv_START        (2)
#define BBPCOMM_W_ABBIF_FMT_w_rxb_q_inv_END          (2)
#define BBPCOMM_W_ABBIF_FMT_w_rxb_i_inv_START        (3)
#define BBPCOMM_W_ABBIF_FMT_w_rxb_i_inv_END          (3)
#define BBPCOMM_W_ABBIF_FMT_w_rxa_iq_exchange_START  (4)
#define BBPCOMM_W_ABBIF_FMT_w_rxa_iq_exchange_END    (4)
#define BBPCOMM_W_ABBIF_FMT_w_rxa_q_inv_START        (5)
#define BBPCOMM_W_ABBIF_FMT_w_rxa_q_inv_END          (5)
#define BBPCOMM_W_ABBIF_FMT_w_rxa_i_inv_START        (6)
#define BBPCOMM_W_ABBIF_FMT_w_rxa_i_inv_END          (6)
#define BBPCOMM_W_ABBIF_FMT_w_tx_iq_exchange_START   (7)
#define BBPCOMM_W_ABBIF_FMT_w_tx_iq_exchange_END     (7)
#define BBPCOMM_W_ABBIF_FMT_w_tx_q_inv_START         (8)
#define BBPCOMM_W_ABBIF_FMT_w_tx_q_inv_END           (8)
#define BBPCOMM_W_ABBIF_FMT_w_tx_i_inv_START         (9)
#define BBPCOMM_W_ABBIF_FMT_w_tx_i_inv_END           (9)


/*****************************************************************************
 结构名    : BBPCOMM_TDS_ABBIF_FMT_UNION
 结构说明  : TDS_ABBIF_FMT 寄存器结构定义。地址偏移量:0x0914，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      tds_abbif_fmt_reg;
    struct
    {
        unsigned long  reserved_0          : 1;  /* bit[0-0]  : 保留 */
        unsigned long  tds_rxb_iq_exchange : 1;  /* bit[1-1]  : 1'b1：TDS的RXB通路IQ路数据交换
                                                                1'b0：TDS的RXB通路IQ路数据不交换
                                                                V3R3不支持IQ交换功能。 */
        unsigned long  tds_rxb_q_inv       : 1;  /* bit[2-2]  : 1'b1：TDS的RXB通路Q路数据取反
                                                                1'b0：TDS的RXB通路Q路数据不取反 */
        unsigned long  tds_rxb_i_inv       : 1;  /* bit[3-3]  : 1'b1：TDS的RXB通路I路数据取反
                                                                1'b0：TDS的RXB通路I路数据不取反 */
        unsigned long  tds_rxa_iq_exchange : 1;  /* bit[4-4]  : 1'b1：TDS的RXA通路IQ路数据交换
                                                                1'b0：TDS的RXA通路IQ路数据不交换
                                                                V3R3不支持IQ交换功能。 */
        unsigned long  tds_rxa_q_inv       : 1;  /* bit[5-5]  : 1'b1：TDS的RXA通路Q路数据取反
                                                                1'b0：TDS的RXA通路Q路数据不取反 */
        unsigned long  tds_rxa_i_inv       : 1;  /* bit[6-6]  : 1'b1：TDS的RXA通路I路数据取反
                                                                1'b0：TDS的RXA通路I路数据不取反 */
        unsigned long  tds_tx_iq_exchange  : 1;  /* bit[7-7]  : 1'b1：TDS的TX通路IQ路数据交换
                                                                1'b0：TDS的TX通路IQ路数据不交换
                                                                V3R3不支持IQ交换功能。 */
        unsigned long  tds_tx_q_inv        : 1;  /* bit[8-8]  : 1'b1：TDS的TX通路Q路数据取反
                                                                1'b0：TDS的TX通路Q路数据不取反 */
        unsigned long  tds_tx_i_inv        : 1;  /* bit[9-9]  : 1'b1：TDS的TX通路I路数据取反
                                                                1'b0：TDS的TX通路I路数据不取反 */
        unsigned long  reserved_1          : 22; /* bit[10-31]: 保留 */
    } reg;
} BBPCOMM_TDS_ABBIF_FMT_UNION;
#define BBPCOMM_TDS_ABBIF_FMT_tds_rxb_iq_exchange_START  (1)
#define BBPCOMM_TDS_ABBIF_FMT_tds_rxb_iq_exchange_END    (1)
#define BBPCOMM_TDS_ABBIF_FMT_tds_rxb_q_inv_START        (2)
#define BBPCOMM_TDS_ABBIF_FMT_tds_rxb_q_inv_END          (2)
#define BBPCOMM_TDS_ABBIF_FMT_tds_rxb_i_inv_START        (3)
#define BBPCOMM_TDS_ABBIF_FMT_tds_rxb_i_inv_END          (3)
#define BBPCOMM_TDS_ABBIF_FMT_tds_rxa_iq_exchange_START  (4)
#define BBPCOMM_TDS_ABBIF_FMT_tds_rxa_iq_exchange_END    (4)
#define BBPCOMM_TDS_ABBIF_FMT_tds_rxa_q_inv_START        (5)
#define BBPCOMM_TDS_ABBIF_FMT_tds_rxa_q_inv_END          (5)
#define BBPCOMM_TDS_ABBIF_FMT_tds_rxa_i_inv_START        (6)
#define BBPCOMM_TDS_ABBIF_FMT_tds_rxa_i_inv_END          (6)
#define BBPCOMM_TDS_ABBIF_FMT_tds_tx_iq_exchange_START   (7)
#define BBPCOMM_TDS_ABBIF_FMT_tds_tx_iq_exchange_END     (7)
#define BBPCOMM_TDS_ABBIF_FMT_tds_tx_q_inv_START         (8)
#define BBPCOMM_TDS_ABBIF_FMT_tds_tx_q_inv_END           (8)
#define BBPCOMM_TDS_ABBIF_FMT_tds_tx_i_inv_START         (9)
#define BBPCOMM_TDS_ABBIF_FMT_tds_tx_i_inv_END           (9)


/*****************************************************************************
 结构名    : BBPCOMM_GM_ABBIF_FMT_UNION
 结构说明  : GM_ABBIF_FMT 寄存器结构定义。地址偏移量:0x0918，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      gm_abbif_fmt_reg;
    struct
    {
        unsigned long  reserved_0         : 1;  /* bit[0-0]  : 保留 */
        unsigned long  gm_rxb_iq_exchange : 1;  /* bit[1-1]  : 1'b1：GM的RXB通路IQ路数据交换
                                                               1'b0：GM的RXB通路IQ路数据不交换
                                                               V3R3不支持IQ交换功能。 */
        unsigned long  gm_rxb_q_inv       : 1;  /* bit[2-2]  : 1'b1：GM的RXB通路Q路数据取反
                                                               1'b0：GM的RXB通路Q路数据不取反 */
        unsigned long  gm_rxb_i_inv       : 1;  /* bit[3-3]  : 1'b1：GM的RXB通路I路数据取反
                                                               1'b0：GM的RXB通路I路数据不取反 */
        unsigned long  gm_rxa_iq_exchange : 1;  /* bit[4-4]  : 1'b1：GM的RXA通路IQ路数据交换
                                                               1'b0：GM的RXA通路IQ路数据不交换
                                                               V3R3不支持IQ交换功能。 */
        unsigned long  gm_rxa_q_inv       : 1;  /* bit[5-5]  : 1'b1：GM的RXA通路Q路数据取反
                                                               1'b0：GM的RXA通路Q路数据不取反 */
        unsigned long  gm_rxa_i_inv       : 1;  /* bit[6-6]  : 1'b1：GM的RXA通路I路数据取反
                                                               1'b0：GM的RXA通路I路数据不取反 */
        unsigned long  gm_tx_iq_exchange  : 1;  /* bit[7-7]  : 1'b1：GM的TX通路IQ路数据交换
                                                               1'b0：GM的TX通路IQ路数据不交换
                                                               V3R3不支持IQ交换功能。 */
        unsigned long  gm_tx_q_inv        : 1;  /* bit[8-8]  : 1'b1：GM的TX通路Q路数据取反
                                                               1'b0：GM的TX通路Q路数据不取反 */
        unsigned long  gm_tx_i_inv        : 1;  /* bit[9-9]  : 1'b1：GM的TX通路I路数据取反
                                                               1'b0：GM的TX通路I路数据不取反 */
        unsigned long  reserved_1         : 22; /* bit[10-31]: 保留 */
    } reg;
} BBPCOMM_GM_ABBIF_FMT_UNION;
#define BBPCOMM_GM_ABBIF_FMT_gm_rxb_iq_exchange_START  (1)
#define BBPCOMM_GM_ABBIF_FMT_gm_rxb_iq_exchange_END    (1)
#define BBPCOMM_GM_ABBIF_FMT_gm_rxb_q_inv_START        (2)
#define BBPCOMM_GM_ABBIF_FMT_gm_rxb_q_inv_END          (2)
#define BBPCOMM_GM_ABBIF_FMT_gm_rxb_i_inv_START        (3)
#define BBPCOMM_GM_ABBIF_FMT_gm_rxb_i_inv_END          (3)
#define BBPCOMM_GM_ABBIF_FMT_gm_rxa_iq_exchange_START  (4)
#define BBPCOMM_GM_ABBIF_FMT_gm_rxa_iq_exchange_END    (4)
#define BBPCOMM_GM_ABBIF_FMT_gm_rxa_q_inv_START        (5)
#define BBPCOMM_GM_ABBIF_FMT_gm_rxa_q_inv_END          (5)
#define BBPCOMM_GM_ABBIF_FMT_gm_rxa_i_inv_START        (6)
#define BBPCOMM_GM_ABBIF_FMT_gm_rxa_i_inv_END          (6)
#define BBPCOMM_GM_ABBIF_FMT_gm_tx_iq_exchange_START   (7)
#define BBPCOMM_GM_ABBIF_FMT_gm_tx_iq_exchange_END     (7)
#define BBPCOMM_GM_ABBIF_FMT_gm_tx_q_inv_START         (8)
#define BBPCOMM_GM_ABBIF_FMT_gm_tx_q_inv_END           (8)
#define BBPCOMM_GM_ABBIF_FMT_gm_tx_i_inv_START         (9)
#define BBPCOMM_GM_ABBIF_FMT_gm_tx_i_inv_END           (9)


/*****************************************************************************
 结构名    : BBPCOMM_GS_ABBIF_FMT_UNION
 结构说明  : GS_ABBIF_FMT 寄存器结构定义。地址偏移量:0x091C，初值:0x00000001，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      gs_abbif_fmt_reg;
    struct
    {
        unsigned long  reserved_0         : 1;  /* bit[0-0]  : 保留 */
        unsigned long  gs_rxb_iq_exchange : 1;  /* bit[1-1]  : 1'b1：GS的RXB通路IQ路数据交换
                                                               1'b0：GS的RXB通路IQ路数据不交换
                                                               V3R3不支持IQ交换功能。 */
        unsigned long  gs_rxb_q_inv       : 1;  /* bit[2-2]  : 1'b1：GS的RXB通路Q路数据取反
                                                               1'b0：GS的RXB通路Q路数据不取反 */
        unsigned long  gs_rxb_i_inv       : 1;  /* bit[3-3]  : 1'b1：GS的RXB通路I路数据取反
                                                               1'b0：GS的RXB通路I路数据不取反 */
        unsigned long  gs_rxa_iq_exchange : 1;  /* bit[4-4]  : 1'b1：GS的RXA通路IQ路数据交换
                                                               1'b0：GS的RXA通路IQ路数据不交换
                                                               V3R3不支持IQ交换功能。 */
        unsigned long  gs_rxa_q_inv       : 1;  /* bit[5-5]  : 1'b1：GS的RXA通路Q路数据取反
                                                               1'b0：GS的RXA通路Q路数据不取反 */
        unsigned long  gs_rxa_i_inv       : 1;  /* bit[6-6]  : 1'b1：GS的RXA通路I路数据取反
                                                               1'b0：GS的RXA通路I路数据不取反 */
        unsigned long  gs_tx_iq_exchange  : 1;  /* bit[7-7]  : 1'b1：GS的TX通路IQ路数据交换
                                                               1'b0：GS的TX通路IQ路数据不交换
                                                               V3R3不支持IQ交换功能。 */
        unsigned long  gs_tx_q_inv        : 1;  /* bit[8-8]  : 1'b1：GS的TX通路Q路数据取反
                                                               1'b0：GS的TX通路Q路数据不取反 */
        unsigned long  gs_tx_i_inv        : 1;  /* bit[9-9]  : 1'b1：GS的TX通路I路数据取反
                                                               1'b0：GS的TX通路I路数据不取反 */
        unsigned long  reserved_1         : 22; /* bit[10-31]: 保留 */
    } reg;
} BBPCOMM_GS_ABBIF_FMT_UNION;
#define BBPCOMM_GS_ABBIF_FMT_gs_rxb_iq_exchange_START  (1)
#define BBPCOMM_GS_ABBIF_FMT_gs_rxb_iq_exchange_END    (1)
#define BBPCOMM_GS_ABBIF_FMT_gs_rxb_q_inv_START        (2)
#define BBPCOMM_GS_ABBIF_FMT_gs_rxb_q_inv_END          (2)
#define BBPCOMM_GS_ABBIF_FMT_gs_rxb_i_inv_START        (3)
#define BBPCOMM_GS_ABBIF_FMT_gs_rxb_i_inv_END          (3)
#define BBPCOMM_GS_ABBIF_FMT_gs_rxa_iq_exchange_START  (4)
#define BBPCOMM_GS_ABBIF_FMT_gs_rxa_iq_exchange_END    (4)
#define BBPCOMM_GS_ABBIF_FMT_gs_rxa_q_inv_START        (5)
#define BBPCOMM_GS_ABBIF_FMT_gs_rxa_q_inv_END          (5)
#define BBPCOMM_GS_ABBIF_FMT_gs_rxa_i_inv_START        (6)
#define BBPCOMM_GS_ABBIF_FMT_gs_rxa_i_inv_END          (6)
#define BBPCOMM_GS_ABBIF_FMT_gs_tx_iq_exchange_START   (7)
#define BBPCOMM_GS_ABBIF_FMT_gs_tx_iq_exchange_END     (7)
#define BBPCOMM_GS_ABBIF_FMT_gs_tx_q_inv_START         (8)
#define BBPCOMM_GS_ABBIF_FMT_gs_tx_q_inv_END           (8)
#define BBPCOMM_GS_ABBIF_FMT_gs_tx_i_inv_START         (9)
#define BBPCOMM_GS_ABBIF_FMT_gs_tx_i_inv_END           (9)


/*****************************************************************************
 结构名    : BBPCOMM_REG24_FOR_USE_UNION
 结构说明  : REG24_FOR_USE 寄存器结构定义。地址偏移量:0x0980，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      reg24_for_use_reg;
    struct
    {
        unsigned long  g1_overch_inter_meas_en : 1;  /* bit[0]   : GSM1跨通道异频测量模式使能：
                                                                   1'b0：不使能
                                                                   1'b1：使能
                                                                   
                                                                   V7R2新增。 */
        unsigned long  reg24_for_use           : 31; /* bit[1-31]: 寄存器24，预留。供DSP之间传递信息。 */
    } reg;
} BBPCOMM_REG24_FOR_USE_UNION;
#define BBPCOMM_REG24_FOR_USE_g1_overch_inter_meas_en_START  (0)
#define BBPCOMM_REG24_FOR_USE_g1_overch_inter_meas_en_END    (0)
#define BBPCOMM_REG24_FOR_USE_reg24_for_use_START            (1)
#define BBPCOMM_REG24_FOR_USE_reg24_for_use_END              (31)


/*****************************************************************************
 结构名    : BBPCOMM_REG25_FOR_USE_UNION
 结构说明  : REG25_FOR_USE 寄存器结构定义。地址偏移量:0x0984，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      reg25_for_use_reg;
    struct
    {
        unsigned long  g2_overch_inter_meas_en : 1;  /* bit[0]   : GSM2跨通道异频测量模式使能：
                                                                   1'b0：不使能
                                                                   1'b1：使能
                                                                   
                                                                   V7R2新增。 */
        unsigned long  reg25_for_use           : 31; /* bit[1-31]: 寄存器25，预留。供DSP之间传递信息。 */
    } reg;
} BBPCOMM_REG25_FOR_USE_UNION;
#define BBPCOMM_REG25_FOR_USE_g2_overch_inter_meas_en_START  (0)
#define BBPCOMM_REG25_FOR_USE_g2_overch_inter_meas_en_END    (0)
#define BBPCOMM_REG25_FOR_USE_reg25_for_use_START            (1)
#define BBPCOMM_REG25_FOR_USE_reg25_for_use_END              (31)


/*****************************************************************************
 结构名    : BBPCOMM_REG26_FOR_USE_UNION
 结构说明  : REG26_FOR_USE 寄存器结构定义。地址偏移量:0x0988，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      reg26_for_use_reg;
    struct
    {
        unsigned long  ssi_unbind_en : 1;  /* bit[0]   : ssi通道不绑定使能：
                                                         1'b0：通道绑定
                                                         1'b1：通道不绑定
                                                         
                                                         V7R2新增。 */
        unsigned long  ssi_ch_sel0   : 1;  /* bit[1]   : 通道0映射ssi器件关系：
                                                         1'b0：通道0映射到ssi0
                                                         1'b1：通道0映射到ssi1
                                                         
                                                         V7R2新增。 */
        unsigned long  ssi_ch_sel1   : 1;  /* bit[2]   : 通道1映射ssi器件关系：
                                                         1'b0：通道1映射到ssi0
                                                         1'b1：通道1映射到ssi1
                                                         
                                                         V7R2新增。 */
        unsigned long  reg26_for_use : 29; /* bit[3-31]: 寄存器26，预留。供DSP之间传递信息。 */
    } reg;
} BBPCOMM_REG26_FOR_USE_UNION;
#define BBPCOMM_REG26_FOR_USE_ssi_unbind_en_START  (0)
#define BBPCOMM_REG26_FOR_USE_ssi_unbind_en_END    (0)
#define BBPCOMM_REG26_FOR_USE_ssi_ch_sel0_START    (1)
#define BBPCOMM_REG26_FOR_USE_ssi_ch_sel0_END      (1)
#define BBPCOMM_REG26_FOR_USE_ssi_ch_sel1_START    (2)
#define BBPCOMM_REG26_FOR_USE_ssi_ch_sel1_END      (2)
#define BBPCOMM_REG26_FOR_USE_reg26_for_use_START  (3)
#define BBPCOMM_REG26_FOR_USE_reg26_for_use_END    (31)


/*****************************************************************************
 结构名    : BBPCOMM_REG27_FOR_USE_UNION
 结构说明  : REG27_FOR_USE 寄存器结构定义。地址偏移量:0x098C，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      reg27_for_use_reg;
    struct
    {
        unsigned long  rf_tcvr_on_or_en : 1;  /* bit[0]   : 输出给RF的TCVR_ON 双通道一致使能
                                                            1：任意通道的TCVR_ON拉高，则2个通道的TCVR_ON都拉高；
                                                            0：根据各自通道状态拉高
                                                            
                                                            V7R2新增。 */
        unsigned long  reg27_for_use    : 31; /* bit[1-31]: 寄存器27，预留。供DSP之间传递信息。 */
    } reg;
} BBPCOMM_REG27_FOR_USE_UNION;
#define BBPCOMM_REG27_FOR_USE_rf_tcvr_on_or_en_START  (0)
#define BBPCOMM_REG27_FOR_USE_rf_tcvr_on_or_en_END    (0)
#define BBPCOMM_REG27_FOR_USE_reg27_for_use_START     (1)
#define BBPCOMM_REG27_FOR_USE_reg27_for_use_END       (31)


/*****************************************************************************
 结构名    : BBPCOMM_REG28_FOR_USE_UNION
 结构说明  : REG28_FOR_USE 寄存器结构定义。地址偏移量:0x0990，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      reg28_for_use_reg;
    struct
    {
        unsigned long  ch0_abb_sel_force_en    : 1;  /* bit[0]   : 输出给ABB_CFG模块的选通信号ch1_sel_abb是否强制使能
                                                                   1：强制，强制值为ch0_abb_sel_force_value
                                                                   0：不强制，逻辑自控
                                                                   
                                                                   V7R2新增。 */
        unsigned long  ch0_abb_sel_force_value : 3;  /* bit[1-3] : 输出给ABB_CFG模块的选通信号ch0_sel_abb强制时的配置值
                                                                   
                                                                   V7R2新增。 */
        unsigned long  reg28_for_use           : 28; /* bit[4-31]: 寄存器28，预留。供DSP之间传递信息。 */
    } reg;
} BBPCOMM_REG28_FOR_USE_UNION;
#define BBPCOMM_REG28_FOR_USE_ch0_abb_sel_force_en_START     (0)
#define BBPCOMM_REG28_FOR_USE_ch0_abb_sel_force_en_END       (0)
#define BBPCOMM_REG28_FOR_USE_ch0_abb_sel_force_value_START  (1)
#define BBPCOMM_REG28_FOR_USE_ch0_abb_sel_force_value_END    (3)
#define BBPCOMM_REG28_FOR_USE_reg28_for_use_START            (4)
#define BBPCOMM_REG28_FOR_USE_reg28_for_use_END              (31)


/*****************************************************************************
 结构名    : BBPCOMM_REG29_FOR_USE_UNION
 结构说明  : REG29_FOR_USE 寄存器结构定义。地址偏移量:0x0994，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      reg29_for_use_reg;
    struct
    {
        unsigned long  ch1_abb_sel_force_en    : 1;  /* bit[0]   : 输出给ABB_CFG模块的选通信号ch1_sel_abb是否强制使能
                                                                   1：强制，强制值为ch1_abb_sel_force_value
                                                                   0：不强制，逻辑自控
                                                                   
                                                                   V7R2新增。 */
        unsigned long  ch1_abb_sel_force_value : 3;  /* bit[1-3] : 输出给ABB_CFG模块的选通信号ch1_sel_abb强制时的配置值
                                                                   
                                                                   V7R2新增。 */
        unsigned long  reg29_for_use           : 28; /* bit[4-31]: 寄存器29，预留。供DSP之间传递信息。 */
    } reg;
} BBPCOMM_REG29_FOR_USE_UNION;
#define BBPCOMM_REG29_FOR_USE_ch1_abb_sel_force_en_START     (0)
#define BBPCOMM_REG29_FOR_USE_ch1_abb_sel_force_en_END       (0)
#define BBPCOMM_REG29_FOR_USE_ch1_abb_sel_force_value_START  (1)
#define BBPCOMM_REG29_FOR_USE_ch1_abb_sel_force_value_END    (3)
#define BBPCOMM_REG29_FOR_USE_reg29_for_use_START            (4)
#define BBPCOMM_REG29_FOR_USE_reg29_for_use_END              (31)


/*****************************************************************************
 结构名    : BBPCOMM_REG30_FOR_USE_UNION
 结构说明  : REG30_FOR_USE 寄存器结构定义。地址偏移量:0x0998，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      reg30_for_use_reg;
    struct
    {
        unsigned long  ch0_rx_en_force_low_cnt : 12; /* bit[0-11] : ch0的com2abb_line_ctrl_mode切换时，com2abb_rxa/b_en强制拉低的时间。单位：总线时钟
                                                                    
                                                                    V7R2新增。 */
        unsigned long  reg30_for_use           : 20; /* bit[12-31]: 寄存器30，预留。供DSP之间传递信息。 */
    } reg;
} BBPCOMM_REG30_FOR_USE_UNION;
#define BBPCOMM_REG30_FOR_USE_ch0_rx_en_force_low_cnt_START  (0)
#define BBPCOMM_REG30_FOR_USE_ch0_rx_en_force_low_cnt_END    (11)
#define BBPCOMM_REG30_FOR_USE_reg30_for_use_START            (12)
#define BBPCOMM_REG30_FOR_USE_reg30_for_use_END              (31)


/*****************************************************************************
 结构名    : BBPCOMM_REG31_FOR_USE_UNION
 结构说明  : REG31_FOR_USE 寄存器结构定义。地址偏移量:0x099C，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      reg31_for_use_reg;
    struct
    {
        unsigned long  ch1_rx_en_force_low_cnt : 12; /* bit[0-11] : ch1的com2abb_line_ctrl_mode切换时，com2abb_rxa/b_en强制拉低的时间。单位：总线时钟
                                                                    
                                                                    V7R2新增。 */
        unsigned long  reg31_for_use           : 20; /* bit[12-31]: 寄存器31，预留。供DSP之间传递信息。 */
    } reg;
} BBPCOMM_REG31_FOR_USE_UNION;
#define BBPCOMM_REG31_FOR_USE_ch1_rx_en_force_low_cnt_START  (0)
#define BBPCOMM_REG31_FOR_USE_ch1_rx_en_force_low_cnt_END    (11)
#define BBPCOMM_REG31_FOR_USE_reg31_for_use_START            (12)
#define BBPCOMM_REG31_FOR_USE_reg31_for_use_END              (31)


/*****************************************************************************
 结构名    : BBPCOMM_REG32_FOR_USE_UNION
 结构说明  : REG32_FOR_USE 寄存器结构定义。地址偏移量:0x09A0，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      reg32_for_use_reg;
    struct
    {
        unsigned long  ssi_cnt_sel   : 1;  /* bit[0]   :  */
        unsigned long  reserved      : 3;  /* bit[1-3] : 保留 */
        unsigned long  mipi_cnt_sel  : 1;  /* bit[4]   :  */
        unsigned long  reg32_for_use : 27; /* bit[5-31]: 寄存器32，预留。供DSP之间传递信息。 */
    } reg;
} BBPCOMM_REG32_FOR_USE_UNION;
#define BBPCOMM_REG32_FOR_USE_ssi_cnt_sel_START    (0)
#define BBPCOMM_REG32_FOR_USE_ssi_cnt_sel_END      (0)
#define BBPCOMM_REG32_FOR_USE_mipi_cnt_sel_START   (4)
#define BBPCOMM_REG32_FOR_USE_mipi_cnt_sel_END     (4)
#define BBPCOMM_REG32_FOR_USE_reg32_for_use_START  (5)
#define BBPCOMM_REG32_FOR_USE_reg32_for_use_END    (31)


/*****************************************************************************
 结构名    : BBPCOMM_REG33_FOR_USE_UNION
 结构说明  : REG33_FOR_USE 寄存器结构定义。地址偏移量:0x09A4，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      reg33_for_use_reg;
    struct
    {
        unsigned long  ch_tx_clk_debug : 1;  /* bit[0]   :  */
        unsigned long  reg33_for_use   : 31; /* bit[1-31]: 寄存器33，预留。供DSP之间传递信息。 */
    } reg;
} BBPCOMM_REG33_FOR_USE_UNION;
#define BBPCOMM_REG33_FOR_USE_ch_tx_clk_debug_START  (0)
#define BBPCOMM_REG33_FOR_USE_ch_tx_clk_debug_END    (0)
#define BBPCOMM_REG33_FOR_USE_reg33_for_use_START    (1)
#define BBPCOMM_REG33_FOR_USE_reg33_for_use_END      (31)


/*****************************************************************************
 结构名    : BBPCOMM_DBG_CLK_GATE_EN_UNION
 结构说明  : DBG_CLK_GATE_EN 寄存器结构定义。地址偏移量:0x0A30，初值:0x00000000，宽度:32
 寄存器说明: 门控时钟配置寄存器
*****************************************************************************/
typedef union
{
    unsigned long      dbg_clk_gate_en_reg;
    struct
    {
        unsigned long  dsp_dbg_clk_gate_en : 1;  /* bit[0-0] : bbp_comm_dbg模块门控时钟使能信号：
                                                               1：直接使用输入时钟，未进行门控；
                                                               0：输入时钟无效，此时进行低功耗状态。 */
        unsigned long  dsp_dbg_clk_bypass  : 1;  /* bit[1-1] : bbp_comm_dbg模块时钟bypass配置信号：
                                                               1：直接使用输入时钟，不进行门控处理，此时dsp_dbg_clk_gate_en配置无效；
                                                               0：表示根据dsp_dbg_clk_gate_en进行门控处理。 */
        unsigned long  reserved            : 30; /* bit[2-31]: 保留 */
    } reg;
} BBPCOMM_DBG_CLK_GATE_EN_UNION;
#define BBPCOMM_DBG_CLK_GATE_EN_dsp_dbg_clk_gate_en_START  (0)
#define BBPCOMM_DBG_CLK_GATE_EN_dsp_dbg_clk_gate_en_END    (0)
#define BBPCOMM_DBG_CLK_GATE_EN_dsp_dbg_clk_bypass_START   (1)
#define BBPCOMM_DBG_CLK_GATE_EN_dsp_dbg_clk_bypass_END     (1)


/*****************************************************************************
 结构名    : BBPCOMM_DBG_REG20_CPU_UNION
 结构说明  : DBG_REG20_CPU 寄存器结构定义。地址偏移量:0x0A34，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      dbg_reg20_cpu_reg;
    struct
    {
        unsigned long  com2tds_busy_rf_ssi1 : 1;  /* bit[0]    : TDS SSI1忙标志。 */
        unsigned long  com2tds_busy_rf_ssi0 : 1;  /* bit[1]    : TDS SSI0忙标志。 */
        unsigned long  com2w_busy_rf_ssi1   : 1;  /* bit[2]    : W SSI1忙标志。 */
        unsigned long  com2w_busy_rf_ssi0   : 1;  /* bit[3]    : W SSI0忙标志。 */
        unsigned long  com2lte_busy_rf_ssi1 : 1;  /* bit[4]    : LTE SSI1忙标志。 */
        unsigned long  com2lte_busy_rf_ssi0 : 1;  /* bit[5]    : LTE SSI0忙标志。 */
        unsigned long  com2g2_busy_rf_ssi1  : 1;  /* bit[6]    : G2 SSI1忙标志。 */
        unsigned long  com2g2_busy_rf_ssi0  : 1;  /* bit[7]    : G2 SSI0忙标志。 */
        unsigned long  com2g1_busy_rf_ssi1  : 1;  /* bit[8]    : G1 SSI1忙标志。 */
        unsigned long  com2g1_busy_rf_ssi0  : 1;  /* bit[9]    : G1 SSI0忙标志。 */
        unsigned long  com2tds_busy_mipi1   : 1;  /* bit[10]   : TDS MIPI1忙标志。 */
        unsigned long  com2tds_busy_mipi0   : 1;  /* bit[11]   : TDS MIPI0忙标志。 */
        unsigned long  com2w_busy_mipi1     : 1;  /* bit[12]   : W MIPI1忙标志。 */
        unsigned long  com2w_busy_mipi0     : 1;  /* bit[13]   : W MIPI0忙标志。 */
        unsigned long  com2lte_busy_mipi1   : 1;  /* bit[14]   : LTE MIPI1忙标志。 */
        unsigned long  com2lte_busy_mipi0   : 1;  /* bit[15]   : LTE MIPI0忙标志。 */
        unsigned long  com2g2_busy_mipi1    : 1;  /* bit[16]   : G2 MIPI1忙标志。 */
        unsigned long  com2g2_busy_mipi0    : 1;  /* bit[17]   : G2 MIPI0忙标志。 */
        unsigned long  com2g1_busy_mipi1    : 1;  /* bit[18]   : G1 MIPI1忙标志。 */
        unsigned long  com2g1_busy_mipi0    : 1;  /* bit[19]   : G1 MIPI0忙标志。 */
        unsigned long  com2c_busy_rf_ssi1   : 1;  /* bit[20]   : C SSI1忙标志。 */
        unsigned long  com2c_busy_rf_ssi0   : 1;  /* bit[21]   : C SSI0忙标志。 */
        unsigned long  com2c_busy_mipi1     : 1;  /* bit[22]   : C MIPI1忙标志。 */
        unsigned long  com2c_busy_mipi0     : 1;  /* bit[23]   : C MIPI0忙标志。 */
        unsigned long  reserved             : 8;  /* bit[24-31]: 保留 */
    } reg;
} BBPCOMM_DBG_REG20_CPU_UNION;
#define BBPCOMM_DBG_REG20_CPU_com2tds_busy_rf_ssi1_START  (0)
#define BBPCOMM_DBG_REG20_CPU_com2tds_busy_rf_ssi1_END    (0)
#define BBPCOMM_DBG_REG20_CPU_com2tds_busy_rf_ssi0_START  (1)
#define BBPCOMM_DBG_REG20_CPU_com2tds_busy_rf_ssi0_END    (1)
#define BBPCOMM_DBG_REG20_CPU_com2w_busy_rf_ssi1_START    (2)
#define BBPCOMM_DBG_REG20_CPU_com2w_busy_rf_ssi1_END      (2)
#define BBPCOMM_DBG_REG20_CPU_com2w_busy_rf_ssi0_START    (3)
#define BBPCOMM_DBG_REG20_CPU_com2w_busy_rf_ssi0_END      (3)
#define BBPCOMM_DBG_REG20_CPU_com2lte_busy_rf_ssi1_START  (4)
#define BBPCOMM_DBG_REG20_CPU_com2lte_busy_rf_ssi1_END    (4)
#define BBPCOMM_DBG_REG20_CPU_com2lte_busy_rf_ssi0_START  (5)
#define BBPCOMM_DBG_REG20_CPU_com2lte_busy_rf_ssi0_END    (5)
#define BBPCOMM_DBG_REG20_CPU_com2g2_busy_rf_ssi1_START   (6)
#define BBPCOMM_DBG_REG20_CPU_com2g2_busy_rf_ssi1_END     (6)
#define BBPCOMM_DBG_REG20_CPU_com2g2_busy_rf_ssi0_START   (7)
#define BBPCOMM_DBG_REG20_CPU_com2g2_busy_rf_ssi0_END     (7)
#define BBPCOMM_DBG_REG20_CPU_com2g1_busy_rf_ssi1_START   (8)
#define BBPCOMM_DBG_REG20_CPU_com2g1_busy_rf_ssi1_END     (8)
#define BBPCOMM_DBG_REG20_CPU_com2g1_busy_rf_ssi0_START   (9)
#define BBPCOMM_DBG_REG20_CPU_com2g1_busy_rf_ssi0_END     (9)
#define BBPCOMM_DBG_REG20_CPU_com2tds_busy_mipi1_START    (10)
#define BBPCOMM_DBG_REG20_CPU_com2tds_busy_mipi1_END      (10)
#define BBPCOMM_DBG_REG20_CPU_com2tds_busy_mipi0_START    (11)
#define BBPCOMM_DBG_REG20_CPU_com2tds_busy_mipi0_END      (11)
#define BBPCOMM_DBG_REG20_CPU_com2w_busy_mipi1_START      (12)
#define BBPCOMM_DBG_REG20_CPU_com2w_busy_mipi1_END        (12)
#define BBPCOMM_DBG_REG20_CPU_com2w_busy_mipi0_START      (13)
#define BBPCOMM_DBG_REG20_CPU_com2w_busy_mipi0_END        (13)
#define BBPCOMM_DBG_REG20_CPU_com2lte_busy_mipi1_START    (14)
#define BBPCOMM_DBG_REG20_CPU_com2lte_busy_mipi1_END      (14)
#define BBPCOMM_DBG_REG20_CPU_com2lte_busy_mipi0_START    (15)
#define BBPCOMM_DBG_REG20_CPU_com2lte_busy_mipi0_END      (15)
#define BBPCOMM_DBG_REG20_CPU_com2g2_busy_mipi1_START     (16)
#define BBPCOMM_DBG_REG20_CPU_com2g2_busy_mipi1_END       (16)
#define BBPCOMM_DBG_REG20_CPU_com2g2_busy_mipi0_START     (17)
#define BBPCOMM_DBG_REG20_CPU_com2g2_busy_mipi0_END       (17)
#define BBPCOMM_DBG_REG20_CPU_com2g1_busy_mipi1_START     (18)
#define BBPCOMM_DBG_REG20_CPU_com2g1_busy_mipi1_END       (18)
#define BBPCOMM_DBG_REG20_CPU_com2g1_busy_mipi0_START     (19)
#define BBPCOMM_DBG_REG20_CPU_com2g1_busy_mipi0_END       (19)
#define BBPCOMM_DBG_REG20_CPU_com2c_busy_rf_ssi1_START    (20)
#define BBPCOMM_DBG_REG20_CPU_com2c_busy_rf_ssi1_END      (20)
#define BBPCOMM_DBG_REG20_CPU_com2c_busy_rf_ssi0_START    (21)
#define BBPCOMM_DBG_REG20_CPU_com2c_busy_rf_ssi0_END      (21)
#define BBPCOMM_DBG_REG20_CPU_com2c_busy_mipi1_START      (22)
#define BBPCOMM_DBG_REG20_CPU_com2c_busy_mipi1_END        (22)
#define BBPCOMM_DBG_REG20_CPU_com2c_busy_mipi0_START      (23)
#define BBPCOMM_DBG_REG20_CPU_com2c_busy_mipi0_END        (23)


/*****************************************************************************
 结构名    : BBPCOMM_DBG_COMM_EN_UNION
 结构说明  : DBG_COMM_EN 寄存器结构定义。地址偏移量:0x0A38，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      dbg_comm_en_reg;
    struct
    {
        unsigned long  dbg_comm_en : 1;  /* bit[0-0] : bbp_comm_dbg使能信号：
                                                       1：使能；
                                                       0：不使能。 */
        unsigned long  reserved    : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_DBG_COMM_EN_UNION;
#define BBPCOMM_DBG_COMM_EN_dbg_comm_en_START  (0)
#define BBPCOMM_DBG_COMM_EN_dbg_comm_en_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_S_W_CH_NUM_IND_UNION
 结构说明  : S_W_CH_NUM_IND 寄存器结构定义。地址偏移量:0x0A40，初值:0x00000000，宽度:32
 寄存器说明: W为从模时对应通道的指示寄存器
*****************************************************************************/
typedef union
{
    unsigned long      s_w_ch_num_ind_reg;
    struct
    {
        unsigned long  s_w_ch_num_ind     : 1;  /* bit[0-0] : W为从模时对应的通道选择信号。
                                                              0：选择通道0；
                                                              1：选择通道1。
                                                              配置时刻：各模根据通信场景配置 */
        unsigned long  s_w_ch_num_ind_sel : 1;  /* bit[1-1] : W为从模时对应的通道选择信号生效方式。
                                                              0：在int1生效；
                                                              1：立即生效。 */
        unsigned long  reserved           : 30; /* bit[2-31]: 保留 */
    } reg;
} BBPCOMM_S_W_CH_NUM_IND_UNION;
#define BBPCOMM_S_W_CH_NUM_IND_s_w_ch_num_ind_START      (0)
#define BBPCOMM_S_W_CH_NUM_IND_s_w_ch_num_ind_END        (0)
#define BBPCOMM_S_W_CH_NUM_IND_s_w_ch_num_ind_sel_START  (1)
#define BBPCOMM_S_W_CH_NUM_IND_s_w_ch_num_ind_sel_END    (1)


/*****************************************************************************
 结构名    : BBPCOMM_S_LTE_CH_NUM_IND_UNION
 结构说明  : S_LTE_CH_NUM_IND 寄存器结构定义。地址偏移量:0x0A44，初值:0x00000000，宽度:32
 寄存器说明: LTE为从模时对应通道的指示寄存器
*****************************************************************************/
typedef union
{
    unsigned long      s_lte_ch_num_ind_reg;
    struct
    {
        unsigned long  s_lte_ch_num_ind     : 1;  /* bit[0-0] : LTE为从模时对应的通道选择信号。
                                                                0：选择通道0；
                                                                1：选择通道1。
                                                                配置时刻：各模根据通信场景配置 */
        unsigned long  s_lte_ch_num_ind_sel : 1;  /* bit[1-1] : LTE为从模时对应的通道选择信号生效方式。
                                                                0：在int1生效；
                                                                1：立即生效。 */
        unsigned long  reserved             : 30; /* bit[2-31]: 保留 */
    } reg;
} BBPCOMM_S_LTE_CH_NUM_IND_UNION;
#define BBPCOMM_S_LTE_CH_NUM_IND_s_lte_ch_num_ind_START      (0)
#define BBPCOMM_S_LTE_CH_NUM_IND_s_lte_ch_num_ind_END        (0)
#define BBPCOMM_S_LTE_CH_NUM_IND_s_lte_ch_num_ind_sel_START  (1)
#define BBPCOMM_S_LTE_CH_NUM_IND_s_lte_ch_num_ind_sel_END    (1)


/*****************************************************************************
 结构名    : BBPCOMM_S_TDS_CH_NUM_IND_UNION
 结构说明  : S_TDS_CH_NUM_IND 寄存器结构定义。地址偏移量:0x0A48，初值:0x00000000，宽度:32
 寄存器说明: TDS为从模时对应通道的指示寄存器
*****************************************************************************/
typedef union
{
    unsigned long      s_tds_ch_num_ind_reg;
    struct
    {
        unsigned long  s_tds_ch_num_ind     : 1;  /* bit[0-0] : TDS为从模时对应的通道选择信号。
                                                                0：选择通道0；
                                                                1：选择通道1。
                                                                配置时刻：各模根据通信场景配置 */
        unsigned long  s_tds_ch_num_ind_sel : 1;  /* bit[1-1] : TDS为从模时对应的通道选择信号生效方式。
                                                                0：在int1生效；
                                                                1：立即生效。 */
        unsigned long  reserved             : 30; /* bit[2-31]: 保留 */
    } reg;
} BBPCOMM_S_TDS_CH_NUM_IND_UNION;
#define BBPCOMM_S_TDS_CH_NUM_IND_s_tds_ch_num_ind_START      (0)
#define BBPCOMM_S_TDS_CH_NUM_IND_s_tds_ch_num_ind_END        (0)
#define BBPCOMM_S_TDS_CH_NUM_IND_s_tds_ch_num_ind_sel_START  (1)
#define BBPCOMM_S_TDS_CH_NUM_IND_s_tds_ch_num_ind_sel_END    (1)


/*****************************************************************************
 结构名    : BBPCOMM_S_GM_CH_NUM_IND_UNION
 结构说明  : S_GM_CH_NUM_IND 寄存器结构定义。地址偏移量:0x0A4C，初值:0x00000000，宽度:32
 寄存器说明: GM为从模时对应通道的指示寄存器
*****************************************************************************/
typedef union
{
    unsigned long      s_gm_ch_num_ind_reg;
    struct
    {
        unsigned long  s_gm_ch_num_ind     : 1;  /* bit[0-0] : GM为从模时对应的通道选择信号。
                                                               0：选择通道0；
                                                               1：选择通道1。
                                                               配置时刻：各模根据通信场景配置 */
        unsigned long  s_gm_ch_num_ind_sel : 1;  /* bit[1-1] : GM为从模时对应的通道选择信号生效方式。
                                                               0：在int1生效；
                                                               1：立即生效。 */
        unsigned long  reserved            : 30; /* bit[2-31]: 保留 */
    } reg;
} BBPCOMM_S_GM_CH_NUM_IND_UNION;
#define BBPCOMM_S_GM_CH_NUM_IND_s_gm_ch_num_ind_START      (0)
#define BBPCOMM_S_GM_CH_NUM_IND_s_gm_ch_num_ind_END        (0)
#define BBPCOMM_S_GM_CH_NUM_IND_s_gm_ch_num_ind_sel_START  (1)
#define BBPCOMM_S_GM_CH_NUM_IND_s_gm_ch_num_ind_sel_END    (1)


/*****************************************************************************
 结构名    : BBPCOMM_S_GS_CH_NUM_IND_UNION
 结构说明  : S_GS_CH_NUM_IND 寄存器结构定义。地址偏移量:0x0A50，初值:0x00000000，宽度:32
 寄存器说明: GS为从模时对应通道的指示寄存器
*****************************************************************************/
typedef union
{
    unsigned long      s_gs_ch_num_ind_reg;
    struct
    {
        unsigned long  s_gs_ch_num_ind     : 1;  /* bit[0-0] : GS为从模时对应的通道选择信号。
                                                               0：选择通道0；
                                                               1：选择通道1。
                                                               配置时刻：各模根据通信场景配置 */
        unsigned long  s_gs_ch_num_ind_sel : 1;  /* bit[1-1] : GS为从模时对应的通道选择信号生效方式。
                                                               0：在int1生效；
                                                               1：立即生效。 */
        unsigned long  reserved            : 30; /* bit[2-31]: 保留 */
    } reg;
} BBPCOMM_S_GS_CH_NUM_IND_UNION;
#define BBPCOMM_S_GS_CH_NUM_IND_s_gs_ch_num_ind_START      (0)
#define BBPCOMM_S_GS_CH_NUM_IND_s_gs_ch_num_ind_END        (0)
#define BBPCOMM_S_GS_CH_NUM_IND_s_gs_ch_num_ind_sel_START  (1)
#define BBPCOMM_S_GS_CH_NUM_IND_s_gs_ch_num_ind_sel_END    (1)


/*****************************************************************************
 结构名    : BBPCOMM_L_CH_SW_UNION
 结构说明  : L_CH_SW 寄存器结构定义。地址偏移量:0x0B00，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      l_ch_sw_reg;
    struct
    {
        unsigned long  l_ch_sw_rf     : 1;  /* bit[0]   : rf相关信号是否交换：
                                                          1'b0：0/1通道不交换
                                                          1'b1：0/1通道交换 */
        unsigned long  l_ch_sw_mipi   : 1;  /* bit[1]   : mipi相关信号是否交换：
                                                          1'b0：0/1通道不交换
                                                          1'b1：0/1通道交换 */
        unsigned long  l_ch_sw_abb_tx : 1;  /* bit[2]   : abb tx相关信号是否交换：
                                                          1'b0：0/1通道不交换
                                                          1'b1：0/1通道交换 */
        unsigned long  l_ch_sw_abb_rx : 1;  /* bit[3]   : abb rx相关信号是否交换：
                                                          1'b0：0/1通道不交换
                                                          1'b1：0/1通道交换 */
        unsigned long  l_ch_sw_pmu    : 1;  /* bit[4]   : pmu相关信号是否交换：
                                                          1'b0：0/1通道不交换
                                                          1'b1：0/1通道交换 */
        unsigned long  l_ch_sw_apt    : 1;  /* bit[5]   : apt相关信号是否交换：
                                                          1'b0：0/1通道不交换
                                                          1'b1：0/1通道交换 */
        unsigned long  reserved       : 26; /* bit[6-31]: 保留 */
    } reg;
} BBPCOMM_L_CH_SW_UNION;
#define BBPCOMM_L_CH_SW_l_ch_sw_rf_START      (0)
#define BBPCOMM_L_CH_SW_l_ch_sw_rf_END        (0)
#define BBPCOMM_L_CH_SW_l_ch_sw_mipi_START    (1)
#define BBPCOMM_L_CH_SW_l_ch_sw_mipi_END      (1)
#define BBPCOMM_L_CH_SW_l_ch_sw_abb_tx_START  (2)
#define BBPCOMM_L_CH_SW_l_ch_sw_abb_tx_END    (2)
#define BBPCOMM_L_CH_SW_l_ch_sw_abb_rx_START  (3)
#define BBPCOMM_L_CH_SW_l_ch_sw_abb_rx_END    (3)
#define BBPCOMM_L_CH_SW_l_ch_sw_pmu_START     (4)
#define BBPCOMM_L_CH_SW_l_ch_sw_pmu_END       (4)
#define BBPCOMM_L_CH_SW_l_ch_sw_apt_START     (5)
#define BBPCOMM_L_CH_SW_l_ch_sw_apt_END       (5)


/*****************************************************************************
 结构名    : BBPCOMM_W_CH_SW_UNION
 结构说明  : W_CH_SW 寄存器结构定义。地址偏移量:0x0B04，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      w_ch_sw_reg;
    struct
    {
        unsigned long  w_ch_sw_rf     : 1;  /* bit[0]   : rf相关信号是否交换：
                                                          1'b0：0/1通道不交换
                                                          1'b1：0/1通道交换 */
        unsigned long  w_ch_sw_mipi   : 1;  /* bit[1]   : mipi相关信号是否交换：
                                                          1'b0：0/1通道不交换
                                                          1'b1：0/1通道交换 */
        unsigned long  w_ch_sw_abb_tx : 1;  /* bit[2]   : abb tx相关信号是否交换：
                                                          1'b0：0/1通道不交换
                                                          1'b1：0/1通道交换 */
        unsigned long  w_ch_sw_abb_rx : 1;  /* bit[3]   : abb rx相关信号是否交换：
                                                          1'b0：0/1通道不交换
                                                          1'b1：0/1通道交换 */
        unsigned long  w_ch_sw_pmu    : 1;  /* bit[4]   : pmu相关信号是否交换：
                                                          1'b0：0/1通道不交换
                                                          1'b1：0/1通道交换 */
        unsigned long  w_ch_sw_apt    : 1;  /* bit[5]   : apt相关信号是否交换：
                                                          1'b0：0/1通道不交换
                                                          1'b1：0/1通道交换 */
        unsigned long  reserved       : 26; /* bit[6-31]: 保留 */
    } reg;
} BBPCOMM_W_CH_SW_UNION;
#define BBPCOMM_W_CH_SW_w_ch_sw_rf_START      (0)
#define BBPCOMM_W_CH_SW_w_ch_sw_rf_END        (0)
#define BBPCOMM_W_CH_SW_w_ch_sw_mipi_START    (1)
#define BBPCOMM_W_CH_SW_w_ch_sw_mipi_END      (1)
#define BBPCOMM_W_CH_SW_w_ch_sw_abb_tx_START  (2)
#define BBPCOMM_W_CH_SW_w_ch_sw_abb_tx_END    (2)
#define BBPCOMM_W_CH_SW_w_ch_sw_abb_rx_START  (3)
#define BBPCOMM_W_CH_SW_w_ch_sw_abb_rx_END    (3)
#define BBPCOMM_W_CH_SW_w_ch_sw_pmu_START     (4)
#define BBPCOMM_W_CH_SW_w_ch_sw_pmu_END       (4)
#define BBPCOMM_W_CH_SW_w_ch_sw_apt_START     (5)
#define BBPCOMM_W_CH_SW_w_ch_sw_apt_END       (5)


/*****************************************************************************
 结构名    : BBPCOMM_T_CH_SW_UNION
 结构说明  : T_CH_SW 寄存器结构定义。地址偏移量:0x0B08，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      t_ch_sw_reg;
    struct
    {
        unsigned long  t_ch_sw_rf     : 1;  /* bit[0]   : rf相关信号是否交换：
                                                          1'b0：0/1通道不交换
                                                          1'b1：0/1通道交换 */
        unsigned long  t_ch_sw_mipi   : 1;  /* bit[1]   : mipi相关信号是否交换：
                                                          1'b0：0/1通道不交换
                                                          1'b1：0/1通道交换 */
        unsigned long  t_ch_sw_abb_tx : 1;  /* bit[2]   : abb tx相关信号是否交换：
                                                          1'b0：0/1通道不交换
                                                          1'b1：0/1通道交换 */
        unsigned long  t_ch_sw_abb_rx : 1;  /* bit[3]   : abb rx相关信号是否交换：
                                                          1'b0：0/1通道不交换
                                                          1'b1：0/1通道交换 */
        unsigned long  t_ch_sw_pmu    : 1;  /* bit[4]   : pmu相关信号是否交换：
                                                          1'b0：0/1通道不交换
                                                          1'b1：0/1通道交换 */
        unsigned long  t_ch_sw_apt    : 1;  /* bit[5]   : apt相关信号是否交换：
                                                          1'b0：0/1通道不交换
                                                          1'b1：0/1通道交换 */
        unsigned long  reserved       : 26; /* bit[6-31]: 保留 */
    } reg;
} BBPCOMM_T_CH_SW_UNION;
#define BBPCOMM_T_CH_SW_t_ch_sw_rf_START      (0)
#define BBPCOMM_T_CH_SW_t_ch_sw_rf_END        (0)
#define BBPCOMM_T_CH_SW_t_ch_sw_mipi_START    (1)
#define BBPCOMM_T_CH_SW_t_ch_sw_mipi_END      (1)
#define BBPCOMM_T_CH_SW_t_ch_sw_abb_tx_START  (2)
#define BBPCOMM_T_CH_SW_t_ch_sw_abb_tx_END    (2)
#define BBPCOMM_T_CH_SW_t_ch_sw_abb_rx_START  (3)
#define BBPCOMM_T_CH_SW_t_ch_sw_abb_rx_END    (3)
#define BBPCOMM_T_CH_SW_t_ch_sw_pmu_START     (4)
#define BBPCOMM_T_CH_SW_t_ch_sw_pmu_END       (4)
#define BBPCOMM_T_CH_SW_t_ch_sw_apt_START     (5)
#define BBPCOMM_T_CH_SW_t_ch_sw_apt_END       (5)


/*****************************************************************************
 结构名    : BBPCOMM_GM_CH_SW_UNION
 结构说明  : GM_CH_SW 寄存器结构定义。地址偏移量:0x0B0C，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      gm_ch_sw_reg;
    struct
    {
        unsigned long  gm_ch_sw_rf     : 1;  /* bit[0]   : rf相关信号是否交换：
                                                           1'b0：0/1通道不交换
                                                           1'b1：0/1通道交换 */
        unsigned long  gm_ch_sw_mipi   : 1;  /* bit[1]   : mipi相关信号是否交换：
                                                           1'b0：0/1通道不交换
                                                           1'b1：0/1通道交换 */
        unsigned long  gm_ch_sw_abb_tx : 1;  /* bit[2]   : abb tx相关信号是否交换：
                                                           1'b0：0/1通道不交换
                                                           1'b1：0/1通道交换 */
        unsigned long  gm_ch_sw_abb_rx : 1;  /* bit[3]   : abb rx相关信号是否交换：
                                                           1'b0：0/1通道不交换
                                                           1'b1：0/1通道交换 */
        unsigned long  gm_ch_sw_pmu    : 1;  /* bit[4]   : pmu相关信号是否交换：
                                                           1'b0：0/1通道不交换
                                                           1'b1：0/1通道交换 */
        unsigned long  gm_ch_sw_apt    : 1;  /* bit[5]   : apt相关信号是否交换：
                                                           1'b0：0/1通道不交换
                                                           1'b1：0/1通道交换 */
        unsigned long  reserved        : 26; /* bit[6-31]: 保留 */
    } reg;
} BBPCOMM_GM_CH_SW_UNION;
#define BBPCOMM_GM_CH_SW_gm_ch_sw_rf_START      (0)
#define BBPCOMM_GM_CH_SW_gm_ch_sw_rf_END        (0)
#define BBPCOMM_GM_CH_SW_gm_ch_sw_mipi_START    (1)
#define BBPCOMM_GM_CH_SW_gm_ch_sw_mipi_END      (1)
#define BBPCOMM_GM_CH_SW_gm_ch_sw_abb_tx_START  (2)
#define BBPCOMM_GM_CH_SW_gm_ch_sw_abb_tx_END    (2)
#define BBPCOMM_GM_CH_SW_gm_ch_sw_abb_rx_START  (3)
#define BBPCOMM_GM_CH_SW_gm_ch_sw_abb_rx_END    (3)
#define BBPCOMM_GM_CH_SW_gm_ch_sw_pmu_START     (4)
#define BBPCOMM_GM_CH_SW_gm_ch_sw_pmu_END       (4)
#define BBPCOMM_GM_CH_SW_gm_ch_sw_apt_START     (5)
#define BBPCOMM_GM_CH_SW_gm_ch_sw_apt_END       (5)


/*****************************************************************************
 结构名    : BBPCOMM_GS_CH_SW_UNION
 结构说明  : GS_CH_SW 寄存器结构定义。地址偏移量:0x0B10，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      gs_ch_sw_reg;
    struct
    {
        unsigned long  gs_ch_sw_rf     : 1;  /* bit[0]   : rf相关信号是否交换：
                                                           1'b0：0/1通道不交换
                                                           1'b1：0/1通道交换 */
        unsigned long  gs_ch_sw_mipi   : 1;  /* bit[1]   : mipi相关信号是否交换：
                                                           1'b0：0/1通道不交换
                                                           1'b1：0/1通道交换 */
        unsigned long  gs_ch_sw_abb_tx : 1;  /* bit[2]   : abb tx相关信号是否交换：
                                                           1'b0：0/1通道不交换
                                                           1'b1：0/1通道交换 */
        unsigned long  gs_ch_sw_abb_rx : 1;  /* bit[3]   : abb rx相关信号是否交换：
                                                           1'b0：0/1通道不交换
                                                           1'b1：0/1通道交换 */
        unsigned long  gs_ch_sw_pmu    : 1;  /* bit[4]   : pmu相关信号是否交换：
                                                           1'b0：0/1通道不交换
                                                           1'b1：0/1通道交换 */
        unsigned long  gs_ch_sw_apt    : 1;  /* bit[5]   : apt相关信号是否交换：
                                                           1'b0：0/1通道不交换
                                                           1'b1：0/1通道交换 */
        unsigned long  reserved        : 26; /* bit[6-31]: 保留 */
    } reg;
} BBPCOMM_GS_CH_SW_UNION;
#define BBPCOMM_GS_CH_SW_gs_ch_sw_rf_START      (0)
#define BBPCOMM_GS_CH_SW_gs_ch_sw_rf_END        (0)
#define BBPCOMM_GS_CH_SW_gs_ch_sw_mipi_START    (1)
#define BBPCOMM_GS_CH_SW_gs_ch_sw_mipi_END      (1)
#define BBPCOMM_GS_CH_SW_gs_ch_sw_abb_tx_START  (2)
#define BBPCOMM_GS_CH_SW_gs_ch_sw_abb_tx_END    (2)
#define BBPCOMM_GS_CH_SW_gs_ch_sw_abb_rx_START  (3)
#define BBPCOMM_GS_CH_SW_gs_ch_sw_abb_rx_END    (3)
#define BBPCOMM_GS_CH_SW_gs_ch_sw_pmu_START     (4)
#define BBPCOMM_GS_CH_SW_gs_ch_sw_pmu_END       (4)
#define BBPCOMM_GS_CH_SW_gs_ch_sw_apt_START     (5)
#define BBPCOMM_GS_CH_SW_gs_ch_sw_apt_END       (5)


/*****************************************************************************
 结构名    : BBPCOMM_L_TCVR_SEL0_UNION
 结构说明  : L_TCVR_SEL0 寄存器结构定义。地址偏移量:0x0B20，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      l_tcvr_sel0_reg;
    struct
    {
        unsigned long  l_tcvr_sel0 : 1;  /* bit[0]   : lte模输出的通道0的tcvr_on信号来源：
                                                       1'b0：来自于lte bbp
                                                       1'b1：来自于dsp配置值 */
        unsigned long  reserved    : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_L_TCVR_SEL0_UNION;
#define BBPCOMM_L_TCVR_SEL0_l_tcvr_sel0_START  (0)
#define BBPCOMM_L_TCVR_SEL0_l_tcvr_sel0_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_L_TCVR_SEL1_UNION
 结构说明  : L_TCVR_SEL1 寄存器结构定义。地址偏移量:0x0B24，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      l_tcvr_sel1_reg;
    struct
    {
        unsigned long  l_tcvr_sel1 : 1;  /* bit[0]   : lte模输出的通道1的tcvr_on信号来源：
                                                       1'b0：来自于lte bbp
                                                       1'b1：来自于dsp配置值 */
        unsigned long  reserved    : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_L_TCVR_SEL1_UNION;
#define BBPCOMM_L_TCVR_SEL1_l_tcvr_sel1_START  (0)
#define BBPCOMM_L_TCVR_SEL1_l_tcvr_sel1_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_T_TCVR_SEL0_UNION
 结构说明  : T_TCVR_SEL0 寄存器结构定义。地址偏移量:0x0B28，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      t_tcvr_sel0_reg;
    struct
    {
        unsigned long  t_tcvr_sel0 : 1;  /* bit[0]   : tds模输出的通道0的tcvr_on信号来源：
                                                       1'b0：来自于tds bbp
                                                       1'b1：来自于dsp配置值 */
        unsigned long  reserved    : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_T_TCVR_SEL0_UNION;
#define BBPCOMM_T_TCVR_SEL0_t_tcvr_sel0_START  (0)
#define BBPCOMM_T_TCVR_SEL0_t_tcvr_sel0_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_T_TCVR_SEL1_UNION
 结构说明  : T_TCVR_SEL1 寄存器结构定义。地址偏移量:0x0B2C，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      t_tcvr_sel1_reg;
    struct
    {
        unsigned long  t_tcvr_sel1 : 1;  /* bit[0]   : tds模输出的通道1的tcvr_on信号来源：
                                                       1'b0：来自于tds bbp
                                                       1'b1：来自于dsp配置值 */
        unsigned long  reserved    : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_T_TCVR_SEL1_UNION;
#define BBPCOMM_T_TCVR_SEL1_t_tcvr_sel1_START  (0)
#define BBPCOMM_T_TCVR_SEL1_t_tcvr_sel1_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_W_TCVR_SEL0_UNION
 结构说明  : W_TCVR_SEL0 寄存器结构定义。地址偏移量:0x0B30，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      w_tcvr_sel0_reg;
    struct
    {
        unsigned long  w_tcvr_sel0 : 1;  /* bit[0]   : wcdma模输出的通道0的tcvr_on信号来源：
                                                       1'b0：来自于wcdma bbp
                                                       1'b1：来自于dsp配置值 */
        unsigned long  reserved    : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_W_TCVR_SEL0_UNION;
#define BBPCOMM_W_TCVR_SEL0_w_tcvr_sel0_START  (0)
#define BBPCOMM_W_TCVR_SEL0_w_tcvr_sel0_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_W_TCVR_SEL1_UNION
 结构说明  : W_TCVR_SEL1 寄存器结构定义。地址偏移量:0x0B34，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      w_tcvr_sel1_reg;
    struct
    {
        unsigned long  w_tcvr_sel1 : 1;  /* bit[0]   : wcdma模输出的通道1的tcvr_on信号来源：：
                                                       1'b0：来自于wcdma bbp
                                                       1'b1：来自于dsp配置值 */
        unsigned long  reserved    : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_W_TCVR_SEL1_UNION;
#define BBPCOMM_W_TCVR_SEL1_w_tcvr_sel1_START  (0)
#define BBPCOMM_W_TCVR_SEL1_w_tcvr_sel1_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_GM_TCVR_SEL0_UNION
 结构说明  : GM_TCVR_SEL0 寄存器结构定义。地址偏移量:0x0B38，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      gm_tcvr_sel0_reg;
    struct
    {
        unsigned long  gm_tcvr_sel0 : 1;  /* bit[0]   : gsm（m）输出的通道0的tcvr_on信号来源：
                                                        1'b0：来自于gsm（m） bbp
                                                        1'b1：来自于dsp配置值 */
        unsigned long  reserved     : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_GM_TCVR_SEL0_UNION;
#define BBPCOMM_GM_TCVR_SEL0_gm_tcvr_sel0_START  (0)
#define BBPCOMM_GM_TCVR_SEL0_gm_tcvr_sel0_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_GM_TCVR_SEL1_UNION
 结构说明  : GM_TCVR_SEL1 寄存器结构定义。地址偏移量:0x0B3C，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      gm_tcvr_sel1_reg;
    struct
    {
        unsigned long  gm_tcvr_sel1 : 1;  /* bit[0]   : gsm（m）输出的通道1的tcvr_on信号来源：
                                                        1'b0：来自于gsm（m） bbp
                                                        1'b1：来自于dsp配置值 */
        unsigned long  reserved     : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_GM_TCVR_SEL1_UNION;
#define BBPCOMM_GM_TCVR_SEL1_gm_tcvr_sel1_START  (0)
#define BBPCOMM_GM_TCVR_SEL1_gm_tcvr_sel1_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_GS_TCVR_SEL0_UNION
 结构说明  : GS_TCVR_SEL0 寄存器结构定义。地址偏移量:0x0B40，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      gs_tcvr_sel0_reg;
    struct
    {
        unsigned long  gs_tcvr_sel0 : 1;  /* bit[0]   : gsm（s）输出的通道0的tcvr_on信号来源：
                                                        1'b0：来自于gsm（s） bbp
                                                        1'b1：来自于dsp配置值 */
        unsigned long  reserved     : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_GS_TCVR_SEL0_UNION;
#define BBPCOMM_GS_TCVR_SEL0_gs_tcvr_sel0_START  (0)
#define BBPCOMM_GS_TCVR_SEL0_gs_tcvr_sel0_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_GS_TCVR_SEL1_UNION
 结构说明  : GS_TCVR_SEL1 寄存器结构定义。地址偏移量:0x0B44，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      gs_tcvr_sel1_reg;
    struct
    {
        unsigned long  gs_tcvr_sel1 : 1;  /* bit[0]   : gsm（s）输出的通道1的tcvr_on信号来源：
                                                        1'b0：来自于gsm（s） bbp
                                                        1'b1：来自于dsp配置值 */
        unsigned long  reserved     : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_GS_TCVR_SEL1_UNION;
#define BBPCOMM_GS_TCVR_SEL1_gs_tcvr_sel1_START  (0)
#define BBPCOMM_GS_TCVR_SEL1_gs_tcvr_sel1_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_L_TCVR_VALUE0_UNION
 结构说明  : L_TCVR_VALUE0 寄存器结构定义。地址偏移量:0x0B48，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      l_tcvr_value0_reg;
    struct
    {
        unsigned long  l_tcvr_value0 : 1;  /* bit[0]   : dsp配置的lte模送给通道0 的tcvr_on值 */
        unsigned long  reserved      : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_L_TCVR_VALUE0_UNION;
#define BBPCOMM_L_TCVR_VALUE0_l_tcvr_value0_START  (0)
#define BBPCOMM_L_TCVR_VALUE0_l_tcvr_value0_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_L_TCVR_VALUE1_UNION
 结构说明  : L_TCVR_VALUE1 寄存器结构定义。地址偏移量:0x0B4C，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      l_tcvr_value1_reg;
    struct
    {
        unsigned long  l_tcvr_value1 : 1;  /* bit[0]   : dsp配置的lte模送给通道1 的tcvr_on值 */
        unsigned long  reserved      : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_L_TCVR_VALUE1_UNION;
#define BBPCOMM_L_TCVR_VALUE1_l_tcvr_value1_START  (0)
#define BBPCOMM_L_TCVR_VALUE1_l_tcvr_value1_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_T_TCVR_VALUE0_UNION
 结构说明  : T_TCVR_VALUE0 寄存器结构定义。地址偏移量:0x0B50，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      t_tcvr_value0_reg;
    struct
    {
        unsigned long  t_tcvr_value0 : 1;  /* bit[0]   : dsp配置的tds模送给通道0 的tcvr_on值 */
        unsigned long  reserved      : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_T_TCVR_VALUE0_UNION;
#define BBPCOMM_T_TCVR_VALUE0_t_tcvr_value0_START  (0)
#define BBPCOMM_T_TCVR_VALUE0_t_tcvr_value0_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_T_TCVR_VALUE1_UNION
 结构说明  : T_TCVR_VALUE1 寄存器结构定义。地址偏移量:0x0B54，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      t_tcvr_value1_reg;
    struct
    {
        unsigned long  t_tcvr_value1 : 1;  /* bit[0]   : dsp配置的tds模送给通道1 的tcvr_on值 */
        unsigned long  reserved      : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_T_TCVR_VALUE1_UNION;
#define BBPCOMM_T_TCVR_VALUE1_t_tcvr_value1_START  (0)
#define BBPCOMM_T_TCVR_VALUE1_t_tcvr_value1_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_W_TCVR_VALUE0_UNION
 结构说明  : W_TCVR_VALUE0 寄存器结构定义。地址偏移量:0x0B58，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      w_tcvr_value0_reg;
    struct
    {
        unsigned long  w_tcvr_value0 : 1;  /* bit[0]   : dsp配置的wcdma模送给通道0 的tcvr_on值 */
        unsigned long  reserved      : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_W_TCVR_VALUE0_UNION;
#define BBPCOMM_W_TCVR_VALUE0_w_tcvr_value0_START  (0)
#define BBPCOMM_W_TCVR_VALUE0_w_tcvr_value0_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_W_TCVR_VALUE1_UNION
 结构说明  : W_TCVR_VALUE1 寄存器结构定义。地址偏移量:0x0B5C，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      w_tcvr_value1_reg;
    struct
    {
        unsigned long  w_tcvr_value1 : 1;  /* bit[0]   : dsp配置的wcdma模送给通道1 的tcvr_on值 */
        unsigned long  reserved      : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_W_TCVR_VALUE1_UNION;
#define BBPCOMM_W_TCVR_VALUE1_w_tcvr_value1_START  (0)
#define BBPCOMM_W_TCVR_VALUE1_w_tcvr_value1_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_GM_TCVR_VALUE0_UNION
 结构说明  : GM_TCVR_VALUE0 寄存器结构定义。地址偏移量:0x0B60，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      gm_tcvr_value0_reg;
    struct
    {
        unsigned long  gm_tcvr_value0 : 1;  /* bit[0]   : dsp配置的gsm（m）模送给通道0 的tcvr_on值 */
        unsigned long  reserved       : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_GM_TCVR_VALUE0_UNION;
#define BBPCOMM_GM_TCVR_VALUE0_gm_tcvr_value0_START  (0)
#define BBPCOMM_GM_TCVR_VALUE0_gm_tcvr_value0_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_GM_TCVR_VALUE1_UNION
 结构说明  : GM_TCVR_VALUE1 寄存器结构定义。地址偏移量:0x0B64，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      gm_tcvr_value1_reg;
    struct
    {
        unsigned long  gm_tcvr_value1 : 1;  /* bit[0]   : dsp配置的gsm（m）模送给通道1的tcvr_on值 */
        unsigned long  reserved       : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_GM_TCVR_VALUE1_UNION;
#define BBPCOMM_GM_TCVR_VALUE1_gm_tcvr_value1_START  (0)
#define BBPCOMM_GM_TCVR_VALUE1_gm_tcvr_value1_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_GS_TCVR_VALUE0_UNION
 结构说明  : GS_TCVR_VALUE0 寄存器结构定义。地址偏移量:0x0B68，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      gs_tcvr_value0_reg;
    struct
    {
        unsigned long  gs_tcvr_value0 : 1;  /* bit[0]   : dsp配置的gsm（s）模送给通道0 的tcvr_on值 */
        unsigned long  reserved       : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_GS_TCVR_VALUE0_UNION;
#define BBPCOMM_GS_TCVR_VALUE0_gs_tcvr_value0_START  (0)
#define BBPCOMM_GS_TCVR_VALUE0_gs_tcvr_value0_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_GS_TCVR_VALUE1_UNION
 结构说明  : GS_TCVR_VALUE1 寄存器结构定义。地址偏移量:0x0B6C，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      gs_tcvr_value1_reg;
    struct
    {
        unsigned long  gs_tcvr_value1 : 1;  /* bit[0]   : dsp配置的gsm（s）模送给通道1 的tcvr_on值 */
        unsigned long  reserved       : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_GS_TCVR_VALUE1_UNION;
#define BBPCOMM_GS_TCVR_VALUE1_gs_tcvr_value1_START  (0)
#define BBPCOMM_GS_TCVR_VALUE1_gs_tcvr_value1_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_RF_TCVR_STATE_UNION
 结构说明  : RF_TCVR_STATE 寄存器结构定义。地址偏移量:0x0B70，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      rf_tcvr_state_reg;
    struct
    {
        unsigned long  l_tcvr_state0  : 1;  /* bit[0]    : lte bbp输入的通道0 tcvr_on信号状态 */
        unsigned long  l_tcvr_state1  : 1;  /* bit[1]    : lte bbp输入的通道1 tcvr_on信号状态 */
        unsigned long  t_tcvr_state0  : 1;  /* bit[2]    : tds bbp输入的通道0 tcvr_on信号状态 */
        unsigned long  reserved_0     : 1;  /* bit[3]    : 保留 */
        unsigned long  w_tcvr_state0  : 1;  /* bit[4]    : wcdma bbp输入的通道0 tcvr_on信号状态 */
        unsigned long  reserved_1     : 1;  /* bit[5]    : 保留 */
        unsigned long  gm_tcvr_state0 : 1;  /* bit[6]    : gsm（m） bbp输入的通道0 tcvr_on信号状态 */
        unsigned long  reserved_2     : 1;  /* bit[7]    : 保留 */
        unsigned long  gs_tcvr_state0 : 1;  /* bit[8]    : gsm（s） bbp输入的通道0 tcvr_on信号状态 */
        unsigned long  reserved_3     : 1;  /* bit[9]    : 保留 */
        unsigned long  c_tcvr_state0  : 1;  /* bit[10]   : c bbp输入的通道0 tcvr_on信号状态 */
        unsigned long  reserved_4     : 5;  /* bit[11-15]: 保留 */
        unsigned long  ch_tcvr_state0 : 1;  /* bit[16]   : 通道0 最终输出的tcvr_on信号状态 */
        unsigned long  ch_tcvr_state1 : 1;  /* bit[17]   : 通道1 最终输出的tcvr_on信号状态 */
        unsigned long  reserved_5     : 14; /* bit[18-31]: 保留 */
    } reg;
} BBPCOMM_RF_TCVR_STATE_UNION;
#define BBPCOMM_RF_TCVR_STATE_l_tcvr_state0_START   (0)
#define BBPCOMM_RF_TCVR_STATE_l_tcvr_state0_END     (0)
#define BBPCOMM_RF_TCVR_STATE_l_tcvr_state1_START   (1)
#define BBPCOMM_RF_TCVR_STATE_l_tcvr_state1_END     (1)
#define BBPCOMM_RF_TCVR_STATE_t_tcvr_state0_START   (2)
#define BBPCOMM_RF_TCVR_STATE_t_tcvr_state0_END     (2)
#define BBPCOMM_RF_TCVR_STATE_w_tcvr_state0_START   (4)
#define BBPCOMM_RF_TCVR_STATE_w_tcvr_state0_END     (4)
#define BBPCOMM_RF_TCVR_STATE_gm_tcvr_state0_START  (6)
#define BBPCOMM_RF_TCVR_STATE_gm_tcvr_state0_END    (6)
#define BBPCOMM_RF_TCVR_STATE_gs_tcvr_state0_START  (8)
#define BBPCOMM_RF_TCVR_STATE_gs_tcvr_state0_END    (8)
#define BBPCOMM_RF_TCVR_STATE_c_tcvr_state0_START   (10)
#define BBPCOMM_RF_TCVR_STATE_c_tcvr_state0_END     (10)
#define BBPCOMM_RF_TCVR_STATE_ch_tcvr_state0_START  (16)
#define BBPCOMM_RF_TCVR_STATE_ch_tcvr_state0_END    (16)
#define BBPCOMM_RF_TCVR_STATE_ch_tcvr_state1_START  (17)
#define BBPCOMM_RF_TCVR_STATE_ch_tcvr_state1_END    (17)


/*****************************************************************************
 结构名    : BBPCOMM_MIPI_UNBIND_EN_UNION
 结构说明  : MIPI_UNBIND_EN 寄存器结构定义。地址偏移量:0x0B90，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      mipi_unbind_en_reg;
    struct
    {
        unsigned long  mipi_unbind_en : 1;  /* bit[0]   : mipi通道不绑定使能：
                                                          1'b0：通道绑定
                                                          1'b1：通道不绑定 */
        unsigned long  reserved       : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_MIPI_UNBIND_EN_UNION;
#define BBPCOMM_MIPI_UNBIND_EN_mipi_unbind_en_START  (0)
#define BBPCOMM_MIPI_UNBIND_EN_mipi_unbind_en_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_MIPI_CH_SEL0_UNION
 结构说明  : MIPI_CH_SEL0 寄存器结构定义。地址偏移量:0x0B94，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      mipi_ch_sel0_reg;
    struct
    {
        unsigned long  mipi_ch_sel0 : 1;  /* bit[0]   : 通道0映射mipi器件关系：
                                                        1'b0：通道0映射到mipi0
                                                        1'b1：通道0映射到mipi1 */
        unsigned long  reserved     : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_MIPI_CH_SEL0_UNION;
#define BBPCOMM_MIPI_CH_SEL0_mipi_ch_sel0_START  (0)
#define BBPCOMM_MIPI_CH_SEL0_mipi_ch_sel0_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_MIPI_CH_SEL1_UNION
 结构说明  : MIPI_CH_SEL1 寄存器结构定义。地址偏移量:0x0B98，初值:0x00000001，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      mipi_ch_sel1_reg;
    struct
    {
        unsigned long  mipi_ch_sel1 : 1;  /* bit[0]   : 通道1映射mipi器件关系：
                                                        1'b0：通道1映射到mipi0
                                                        1'b1：通道1映射到mipi1 */
        unsigned long  reserved     : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_MIPI_CH_SEL1_UNION;
#define BBPCOMM_MIPI_CH_SEL1_mipi_ch_sel1_START  (0)
#define BBPCOMM_MIPI_CH_SEL1_mipi_ch_sel1_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_APT_CH_SEL0_UNION
 结构说明  : APT_CH_SEL0 寄存器结构定义。地址偏移量:0x0B9C，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      apt_ch_sel0_reg;
    struct
    {
        unsigned long  apt_ch_sel0 : 1;  /* bit[0]   : 保留 */
        unsigned long  reserved    : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_APT_CH_SEL0_UNION;
#define BBPCOMM_APT_CH_SEL0_apt_ch_sel0_START  (0)
#define BBPCOMM_APT_CH_SEL0_apt_ch_sel0_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_APT_CH_SEL1_UNION
 结构说明  : APT_CH_SEL1 寄存器结构定义。地址偏移量:0x0BA0，初值:0x00000001，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      apt_ch_sel1_reg;
    struct
    {
        unsigned long  apt_ch_sel1 : 1;  /* bit[0]   : 保留 */
        unsigned long  reserved    : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_APT_CH_SEL1_UNION;
#define BBPCOMM_APT_CH_SEL1_apt_ch_sel1_START  (0)
#define BBPCOMM_APT_CH_SEL1_apt_ch_sel1_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_AFC_CH_SEL0_UNION
 结构说明  : AFC_CH_SEL0 寄存器结构定义。地址偏移量:0x0BA4，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      afc_ch_sel0_reg;
    struct
    {
        unsigned long  afc_ch_sel0 : 1;  /* bit[0]   : afc pdm0信号来源：
                                                       1'b0：来自于通道0
                                                       1'b1：来自于通道1 */
        unsigned long  reserved    : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_AFC_CH_SEL0_UNION;
#define BBPCOMM_AFC_CH_SEL0_afc_ch_sel0_START  (0)
#define BBPCOMM_AFC_CH_SEL0_afc_ch_sel0_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_AFC_CH_SEL1_UNION
 结构说明  : AFC_CH_SEL1 寄存器结构定义。地址偏移量:0x0BA8，初值:0x00000001，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      afc_ch_sel1_reg;
    struct
    {
        unsigned long  afc_ch_sel1 : 1;  /* bit[0]   : afc pdm1信号来源：
                                                       1'b0：来自于通道0
                                                       1'b1：来自于通道1 */
        unsigned long  reserved    : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_AFC_CH_SEL1_UNION;
#define BBPCOMM_AFC_CH_SEL1_afc_ch_sel1_START  (0)
#define BBPCOMM_AFC_CH_SEL1_afc_ch_sel1_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_ABB_TX_CH_SEL0_UNION
 结构说明  : ABB_TX_CH_SEL0 寄存器结构定义。地址偏移量:0x0BAC，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      abb_tx_ch_sel0_reg;
    struct
    {
        unsigned long  abb_tx_ch_sel0 : 1;  /* bit[0]   : 保留 */
        unsigned long  reserved       : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_ABB_TX_CH_SEL0_UNION;
#define BBPCOMM_ABB_TX_CH_SEL0_abb_tx_ch_sel0_START  (0)
#define BBPCOMM_ABB_TX_CH_SEL0_abb_tx_ch_sel0_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_ABB_TX_CH_SEL1_UNION
 结构说明  : ABB_TX_CH_SEL1 寄存器结构定义。地址偏移量:0x0BB0，初值:0x00000001，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      abb_tx_ch_sel1_reg;
    struct
    {
        unsigned long  abb_tx_ch_sel1 : 1;  /* bit[0]   : 保留 */
        unsigned long  reserved       : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_ABB_TX_CH_SEL1_UNION;
#define BBPCOMM_ABB_TX_CH_SEL1_abb_tx_ch_sel1_START  (0)
#define BBPCOMM_ABB_TX_CH_SEL1_abb_tx_ch_sel1_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_PMU_CH_SEL0_UNION
 结构说明  : PMU_CH_SEL0 寄存器结构定义。地址偏移量:0x0BB4，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      pmu_ch_sel0_reg;
    struct
    {
        unsigned long  pmu_ch_sel0 : 1;  /* bit[0]   : 保留 */
        unsigned long  reserved    : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_PMU_CH_SEL0_UNION;
#define BBPCOMM_PMU_CH_SEL0_pmu_ch_sel0_START  (0)
#define BBPCOMM_PMU_CH_SEL0_pmu_ch_sel0_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_PMU_CH_SEL1_UNION
 结构说明  : PMU_CH_SEL1 寄存器结构定义。地址偏移量:0x0BB8，初值:0x00000001，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      pmu_ch_sel1_reg;
    struct
    {
        unsigned long  pmu_ch_sel1 : 1;  /* bit[0]   : 保留 */
        unsigned long  reserved    : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_PMU_CH_SEL1_UNION;
#define BBPCOMM_PMU_CH_SEL1_pmu_ch_sel1_START  (0)
#define BBPCOMM_PMU_CH_SEL1_pmu_ch_sel1_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_VALID_NUM_UNION
 结构说明  : VALID_NUM 寄存器结构定义。地址偏移量:0x0BC0，初值:0x00000001，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      valid_num_reg;
    struct
    {
        unsigned long  valid_num : 16; /* bit[0-15] : 保留 */
        unsigned long  reserved  : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_VALID_NUM_UNION;
#define BBPCOMM_VALID_NUM_valid_num_START  (0)
#define BBPCOMM_VALID_NUM_valid_num_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_VALID_NUM1_UNION
 结构说明  : VALID_NUM1 寄存器结构定义。地址偏移量:0x0BC4，初值:0x00010001，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      valid_num1_reg;
    struct
    {
        unsigned long  valid_num1 : 16; /* bit[0-15] : 保留 */
        unsigned long  valid_num2 : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_VALID_NUM1_UNION;
#define BBPCOMM_VALID_NUM1_valid_num1_START  (0)
#define BBPCOMM_VALID_NUM1_valid_num1_END    (15)
#define BBPCOMM_VALID_NUM1_valid_num2_START  (16)
#define BBPCOMM_VALID_NUM1_valid_num2_END    (31)


/*****************************************************************************
 结构名    : BBPCOMM_INT_CLEAR_LTE_RCV_C_UNION
 结构说明  : INT_CLEAR_LTE_RCV_C 寄存器结构定义。地址偏移量:0x0C00，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      int_clear_lte_rcv_c_reg;
    struct
    {
        unsigned long  int_clear_lte_rcv_c : 16; /* bit[0-15] : 逻辑自清零。
                                                                中断标志清除指示，高有效。用于清除INT_TYPE_LTE_RCV_C[15:0]对应bit位
                                                                生效时刻：立即生效 */
        unsigned long  reserved            : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_INT_CLEAR_LTE_RCV_C_UNION;
#define BBPCOMM_INT_CLEAR_LTE_RCV_C_int_clear_lte_rcv_c_START  (0)
#define BBPCOMM_INT_CLEAR_LTE_RCV_C_int_clear_lte_rcv_c_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_INT_MASK_LTE_RCV_C_UNION
 结构说明  : INT_MASK_LTE_RCV_C 寄存器结构定义。地址偏移量:0x0C04，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      int_mask_lte_rcv_c_reg;
    struct
    {
        unsigned long  int_mask_lte_rcv_c : 16; /* bit[0-15] : 中断屏蔽标志。用于屏蔽INT_TYPE_LTE_RCV_C[15:0]对应bit中断。
                                                               0：屏蔽中断上报；
                                                               1：使能中断上报。
                                                               配置时刻：初始化
                                                               生效时刻：立即生效 */
        unsigned long  reserved           : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_INT_MASK_LTE_RCV_C_UNION;
#define BBPCOMM_INT_MASK_LTE_RCV_C_int_mask_lte_rcv_c_START  (0)
#define BBPCOMM_INT_MASK_LTE_RCV_C_int_mask_lte_rcv_c_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_INT_TYPE_LTE_RCV_C_UNION
 结构说明  : INT_TYPE_LTE_RCV_C 寄存器结构定义。地址偏移量:0x0C08，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      int_type_lte_rcv_c_reg;
    struct
    {
        unsigned long  o_int_type_lte_rcv_c : 16; /* bit[0-15] : 中断状态标志。各bit单独对应一种中断类型，高有效。
                                                                 [0]: 中断类型0。 C（主模）提前通知LTE异系统测量启动中断。
                                                                 [1]: 中断类型1。 C（主模）进GAP中断，通知LTE进行异系统测量射频通道配置，天线切换等操作。
                                                                 [2]: 中断类型2。C（主模）出GAP中断。
                                                                 [3]: 中断类型3。C（从模）测量结束中断。
                                                                 [4]: 中断类型4。C（主模）强制C退出异系统测量中断。
                                                                 [5]: 中断类型5。C（主模）通知C上报本次测量结果。
                                                                 [6]: 保留。
                                                                 [15:7]: 对应中断类型[15:7]，预留。
                                                                 软件读取该中断状态上报寄存器后，需要和0x0A84的int_mask_lte_rcv_c相与，再判断是否需要处理。 */
        unsigned long  reserved             : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_INT_TYPE_LTE_RCV_C_UNION;
#define BBPCOMM_INT_TYPE_LTE_RCV_C_o_int_type_lte_rcv_c_START  (0)
#define BBPCOMM_INT_TYPE_LTE_RCV_C_o_int_type_lte_rcv_c_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_LTE_SND_C_INT_13_UNION
 结构说明  : LTE_SND_C_INT_13 寄存器结构定义。地址偏移量:0x0C0C，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      lte_snd_c_int_13_reg;
    struct
    {
        unsigned long  reserved_0       : 3;  /* bit[0-2]  : 保留 */
        unsigned long  lte_snd_c_int_13 : 13; /* bit[3-15] : 逻辑自清零。高有效。
                                                             
                                                             bit15~7：对应lte_snd_c_int_15 ~ 7。
                                                             LPHY通过此寄存器触发类型15~7中断（int_type_c_rcv_lte[15:7]）给CPHY。
                                                             生效时刻：立即生效
                                                             
                                                             bit6：对应lte_snd_c_int_6。
                                                             LPHY通过此寄存器触发类型6中断（int_type_lte_rcv_c[6]）给LPHY。
                                                             生效时刻：立即生效
                                                             
                                                             bit5：对应c_snd_lte_int_5。
                                                             L主模时通知LPHY L测量周期到达，高有效。
                                                             LPHY通过此寄存器触发类型5中断（int_type_c_rcv_lte[5]）给LPHY。
                                                             生效时刻：立即生效
                                                             
                                                             bit4：对应lte_snd_c_int_4。
                                                             L主模时强制退出异系统测量指示。高有效。
                                                             LPHY通过此寄存器触发类型4中断（int_type_c_rcv_lte[4]）给CPHY。
                                                             生效时刻：立即生效
                                                             
                                                             bit3：对应lte_snd_c_int_3。L测量结束指示，高有效。
                                                             用于触发中断类型3 （int_type_c_rcv_lte[3]）给CPHY。C主模下对L进行异系统测量时，如果L测量结束，LPHY配置此标志为1。 
                                                             生效时刻：立即生效 */
        unsigned long  reserved_1       : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_LTE_SND_C_INT_13_UNION;
#define BBPCOMM_LTE_SND_C_INT_13_lte_snd_c_int_13_START  (3)
#define BBPCOMM_LTE_SND_C_INT_13_lte_snd_c_int_13_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_C_SND_C_INT_13_UNION
 结构说明  : C_SND_C_INT_13 寄存器结构定义。地址偏移量:0x0C10，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      c_snd_c_int_13_reg;
    struct
    {
        unsigned long  reserved_0     : 3;  /* bit[0-2]  : 保留 */
        unsigned long  c_snd_c_int_13 : 13; /* bit[3-15] : 逻辑自清零。高有效。
                                                           
                                                           bit15~7：对应c_snd_c_int_15 ~ 7。
                                                           C DSP通过此寄存器触发类型15~7中断（int_type_c_rcv_c[15:7]）给C DSP。
                                                           生效时刻：立即生效
                                                           
                                                           bit6：对应c_snd_c_int_6。
                                                           CDSP通过此寄存器触发类型6中断（int_type_c_rcv_c[6]）给CDSP。
                                                           生效时刻：立即生效
                                                           
                                                           bit5：对应c_snd_c_int_5。
                                                           C主模时通知CDSP C测量周期到达，高有效。
                                                           C DSP通过此寄存器触发类型5中断（int_type_c_rcv_c[5]）给CDSP。
                                                           生效时刻：立即生效
                                                           
                                                           bit4：对应c_snd_c_int_4。
                                                           C主模时强制退出异系统测量指示。高有效。
                                                           C DSP通过此寄存器触发类型4中断（int_type_c_rcv_c[4]）给CDSP。
                                                           生效时刻：立即生效
                                                           
                                                           bit3：对应c_snd_c_int_3。C测量结束指示，高有效。
                                                           用于触发中断类型3 （int_type_c_rcv_c[3]）给CDSP。C主模下对C进行异系统测量时，如果C测量结束，C DSP配置此标志为1。 
                                                           生效时刻：立即生效 */
        unsigned long  reserved_1     : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_C_SND_C_INT_13_UNION;
#define BBPCOMM_C_SND_C_INT_13_c_snd_c_int_13_START  (3)
#define BBPCOMM_C_SND_C_INT_13_c_snd_c_int_13_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_CBBP_INPUT_MASK_UNION
 结构说明  : CBBP_INPUT_MASK 寄存器结构定义。地址偏移量:0x0C14，初值:0x00000003，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      cbbp_input_mask_reg;
    struct
    {
        unsigned long  cbbp_input_mask : 2;  /* bit[0-1] : 电平信号，低有效。
                                                           当C主模需要做异系统测量时需配置本寄存器为3，异系统测量结束后配置本寄存器为0。
                                                           bit1:1’b0:
                                                            让c_intrasys_valid输入无效，被强制为1。
                                                           bit0:1’b0:
                                                           c_intersys_ingap_pre_ind输入无效，被强制为0。
                                                           bit[1:0] = 11b: 让c_intrasys_valid和c_intersys_ingap_pre_ind输入有效。
                                                           配置时刻：TDS主模需要做异系统测量时需配置本寄存器为3，异系统测量结束后配置本寄存器为0。
                                                           生效时刻：立即生效 */
        unsigned long  reserved        : 30; /* bit[2-31]: 保留 */
    } reg;
} BBPCOMM_CBBP_INPUT_MASK_UNION;
#define BBPCOMM_CBBP_INPUT_MASK_cbbp_input_mask_START  (0)
#define BBPCOMM_CBBP_INPUT_MASK_cbbp_input_mask_END    (1)


/*****************************************************************************
 结构名    : BBPCOMM_C_TIMING_DSP_UNION
 结构说明  : C_TIMING_DSP 寄存器结构定义。地址偏移量:0x0C20，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      c_timing_dsp_reg;
    struct
    {
        unsigned long  o_c_timing1_dsp : 30; /* bit[0-29] : C模定时信息上报。  */
        unsigned long  reserved        : 2;  /* bit[30-31]: 保留 */
    } reg;
} BBPCOMM_C_TIMING_DSP_UNION;
#define BBPCOMM_C_TIMING_DSP_o_c_timing1_dsp_START  (0)
#define BBPCOMM_C_TIMING_DSP_o_c_timing1_dsp_END    (29)


/*****************************************************************************
 结构名    : BBPCOMM_C_INTRASYS_VALID_UNION
 结构说明  : C_INTRASYS_VALID 寄存器结构定义。地址偏移量:0x0C24，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      c_intrasys_valid_reg;
    struct
    {
        unsigned long  o_c_intrasys_valid       : 1;  /* bit[0]   : C主模下控制RF指示，电平信号。
                                                                    1’b0: C放弃RF控制权。
                                                                    1’b1: C享有控制RF权（但实际配置RF需c_intrasys_valid_delay信号为1）。 */
        unsigned long  o_c_intrasys_valid_delay : 1;  /* bit[1]   : C主模下实际控制RF指示，电平信号。为1代表W控制RF配置通道，为0代表从模可以控制RF通道。用于单RFIC时射频控制相关信号的选通。
                                                                    c_intrasys_valid_delay的上升沿比c_intrasys_valid上升沿滞后，下降沿一致。 */
        unsigned long  reserved                 : 30; /* bit[2-31]: 保留 */
    } reg;
} BBPCOMM_C_INTRASYS_VALID_UNION;
#define BBPCOMM_C_INTRASYS_VALID_o_c_intrasys_valid_START        (0)
#define BBPCOMM_C_INTRASYS_VALID_o_c_intrasys_valid_END          (0)
#define BBPCOMM_C_INTRASYS_VALID_o_c_intrasys_valid_delay_START  (1)
#define BBPCOMM_C_INTRASYS_VALID_o_c_intrasys_valid_delay_END    (1)


/*****************************************************************************
 结构名    : BBPCOMM_C_INTERSYS_MEASURE_TYPE_UNION
 结构说明  : C_INTERSYS_MEASURE_TYPE 寄存器结构定义。地址偏移量:0x0C28，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      c_intersys_measure_type_reg;
    struct
    {
        unsigned long  c_intersys_measure_type : 4;  /* bit[0-3] : 异系统测量类型。
                                                                   4’b0：测量C。
                                                                   4’b1: C背景搜；
                                                                   其它预留。
                                                                   生效时刻：立即生效 */
        unsigned long  reserved                : 28; /* bit[4-31]: 保留 */
    } reg;
} BBPCOMM_C_INTERSYS_MEASURE_TYPE_UNION;
#define BBPCOMM_C_INTERSYS_MEASURE_TYPE_c_intersys_measure_type_START  (0)
#define BBPCOMM_C_INTERSYS_MEASURE_TYPE_c_intersys_measure_type_END    (3)


/*****************************************************************************
 结构名    : BBPCOMM_C_MEASURE_REPORT_VALID_UNION
 结构说明  : C_MEASURE_REPORT_VALID 寄存器结构定义。地址偏移量:0x0C2C，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      c_measure_report_valid_reg;
    struct
    {
        unsigned long  c_measure_report_valid : 1;  /* bit[0-0] : 单频点测量周期到指示，L或者TDS/Gm/Gs/U作为主模，通知CPHY W测量的单频点周期到达。
                                                                  只在连接态下需要配置。
                                                                  电平信号，高有效。
                                                                  生效时刻：立即生效 */
        unsigned long  reserved               : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_C_MEASURE_REPORT_VALID_UNION;
#define BBPCOMM_C_MEASURE_REPORT_VALID_c_measure_report_valid_START  (0)
#define BBPCOMM_C_MEASURE_REPORT_VALID_c_measure_report_valid_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_INT_CLEAR_C_RCV_LTE_UNION
 结构说明  : INT_CLEAR_C_RCV_LTE 寄存器结构定义。地址偏移量:0x0C38，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      int_clear_c_rcv_lte_reg;
    struct
    {
        unsigned long  int_clear_c_rcv_lte : 16; /* bit[0-15] : 逻辑自清零。
                                                                中断标志清除指示，高有效。用于清除INT_TYPE_C_RCV_LTE[15:0]对应bit位
                                                                生效时刻：立即生效 */
        unsigned long  reserved            : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_INT_CLEAR_C_RCV_LTE_UNION;
#define BBPCOMM_INT_CLEAR_C_RCV_LTE_int_clear_c_rcv_lte_START  (0)
#define BBPCOMM_INT_CLEAR_C_RCV_LTE_int_clear_c_rcv_lte_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_INT_CLEAR_C_RCV_C_UNION
 结构说明  : INT_CLEAR_C_RCV_C 寄存器结构定义。地址偏移量:0x0C3C，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      int_clear_c_rcv_c_reg;
    struct
    {
        unsigned long  int_clear_c_rcv_c : 16; /* bit[0-15] : 逻辑自清零。
                                                              中断标志清除指示，高有效。用于清除INT_TYPE_C_RCV_C[15:0]对应bit位
                                                              生效时刻：立即生效 */
        unsigned long  reserved          : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_INT_CLEAR_C_RCV_C_UNION;
#define BBPCOMM_INT_CLEAR_C_RCV_C_int_clear_c_rcv_c_START  (0)
#define BBPCOMM_INT_CLEAR_C_RCV_C_int_clear_c_rcv_c_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_INT_CLEAR_C_INT012_UNION
 结构说明  : INT_CLEAR_C_INT012 寄存器结构定义。地址偏移量:0x0C40，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      int_clear_c_int012_reg;
    struct
    {
        unsigned long  int_clear_c_int012 : 3;  /* bit[0-2] : 逻辑自清零。
                                                              中断标志清除指示，高有效。用于清除INT_TYPE_C_INT012[2:0]对应bit位
                                                              生效时刻：立即生效 */
        unsigned long  reserved           : 29; /* bit[3-31]: 保留 */
    } reg;
} BBPCOMM_INT_CLEAR_C_INT012_UNION;
#define BBPCOMM_INT_CLEAR_C_INT012_int_clear_c_int012_START  (0)
#define BBPCOMM_INT_CLEAR_C_INT012_int_clear_c_int012_END    (2)


/*****************************************************************************
 结构名    : BBPCOMM_INT_MASK_C_RCV_LTE_UNION
 结构说明  : INT_MASK_C_RCV_LTE 寄存器结构定义。地址偏移量:0x0C44，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      int_mask_c_rcv_lte_reg;
    struct
    {
        unsigned long  int_mask_c_rcv_lte : 16; /* bit[0-15] : 中断屏蔽标志。用于屏蔽INT_TYPE_C_RCV_LTE[15:0]对应bit中断。
                                                               0：屏蔽中断上报；
                                                               1：使能中断上报。
                                                               配置时刻：初始化
                                                               生效时刻：立即生效 */
        unsigned long  reserved           : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_INT_MASK_C_RCV_LTE_UNION;
#define BBPCOMM_INT_MASK_C_RCV_LTE_int_mask_c_rcv_lte_START  (0)
#define BBPCOMM_INT_MASK_C_RCV_LTE_int_mask_c_rcv_lte_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_INT_MASK_C_RCV_C_UNION
 结构说明  : INT_MASK_C_RCV_C 寄存器结构定义。地址偏移量:0x0C48，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      int_mask_c_rcv_c_reg;
    struct
    {
        unsigned long  int_mask_c_rcv_c : 16; /* bit[0-15] : 中断屏蔽标志。用于屏蔽INT_TYPE_C_RCV_C[15:0]对应bit中断。
                                                             0：屏蔽中断上报；
                                                             1：使能中断上报。
                                                             配置时刻：初始化
                                                             生效时刻：立即生效 */
        unsigned long  reserved         : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_INT_MASK_C_RCV_C_UNION;
#define BBPCOMM_INT_MASK_C_RCV_C_int_mask_c_rcv_c_START  (0)
#define BBPCOMM_INT_MASK_C_RCV_C_int_mask_c_rcv_c_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_INT_MASK_C_INT012_UNION
 结构说明  : INT_MASK_C_INT012 寄存器结构定义。地址偏移量:0x0C4C，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      int_mask_c_int012_reg;
    struct
    {
        unsigned long  int_mask_c_int012 : 3;  /* bit[0-2] : 中断屏蔽标志。用于屏蔽INT_TYPE_C_INT012[2:0]对应bit中断。
                                                             0：屏蔽中断上报；
                                                             1：使能中断上报。
                                                             配置时刻：初始化
                                                             生效时刻：立即生效 */
        unsigned long  reserved          : 29; /* bit[3-31]: 保留 */
    } reg;
} BBPCOMM_INT_MASK_C_INT012_UNION;
#define BBPCOMM_INT_MASK_C_INT012_int_mask_c_int012_START  (0)
#define BBPCOMM_INT_MASK_C_INT012_int_mask_c_int012_END    (2)


/*****************************************************************************
 结构名    : BBPCOMM_INT_TYPE_C_RCV_LTE_UNION
 结构说明  : INT_TYPE_C_RCV_LTE 寄存器结构定义。地址偏移量:0x0C50，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      int_type_c_rcv_lte_reg;
    struct
    {
        unsigned long  o_int_type_c_rcv_lte : 16; /* bit[0-15] : 中断状态标志。各bit单独对应一种中断类型，高有效。
                                                                 [0]: 中断类型0。 LTE（主模）提前通知C异系统测量启动中断。
                                                                 [1]: 中断类型1。 LTE（主模）进GAP中断，通知C进行异系统测量射频通道配置，天线切换等操作。
                                                                 [2]: 中断类型2。 LTE（主模）出GAP中断。
                                                                 [3]: 中断类型3。LTE（从模）测量结束中断。
                                                                 [4]: 中断类型4。LTE（主模）强制C退出异系统测量中断。
                                                                 [5]: 中断类型5。LTE（主模）通知C上报本次测量结果。
                                                                 [6]: 保留。
                                                                 [15:7]: 对应中断类型[15:7]，预留。
                                                                 软件读取该中断状态上报寄存器后，需要和0x0B34的int_mask_c_rcv_lte相与，再判断是否需要处理。 */
        unsigned long  reserved             : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_INT_TYPE_C_RCV_LTE_UNION;
#define BBPCOMM_INT_TYPE_C_RCV_LTE_o_int_type_c_rcv_lte_START  (0)
#define BBPCOMM_INT_TYPE_C_RCV_LTE_o_int_type_c_rcv_lte_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_INT_TYPE_C_RCV_C_UNION
 结构说明  : INT_TYPE_C_RCV_C 寄存器结构定义。地址偏移量:0x0C54，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      int_type_c_rcv_c_reg;
    struct
    {
        unsigned long  o_int_type_c_rcv_c : 16; /* bit[0-15] : 中断状态标志。各bit单独对应一种中断类型，高有效。
                                                               [0]: 中断类型0。C（主模）提前通知LTE异频测量启动中断。
                                                               [1]: 中断类型1。C（主模）进GAP中断，通知C进行异频测量射频通道配置，天线切换等操作。
                                                               [2]: 中断类型2。C（主模）出GAP中断。
                                                               [3]: 中断类型3。C（从模，即异频）测量结束中断。
                                                               [4]: 中断类型4。C（主模）强制C退出异频测量中断。
                                                               [5]: 中断类型5。C（主模）通知C上报本次测量结果。
                                                               [6]: 中断类型6。保留。
                                                               [15:7]: 对应中断类型[15:7]，预留。
                                                               中断状态标志0，1，2只有当LTE处于主模时才能产生。
                                                               软件读取该中断状态上报寄存器后，需要和0x0B38的int_mask_c_rcv_c相与，再判断是否需要处理。 */
        unsigned long  reserved           : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_INT_TYPE_C_RCV_C_UNION;
#define BBPCOMM_INT_TYPE_C_RCV_C_o_int_type_c_rcv_c_START  (0)
#define BBPCOMM_INT_TYPE_C_RCV_C_o_int_type_c_rcv_c_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_INT_TYPE_C_INT012_UNION
 结构说明  : INT_TYPE_C_INT012 寄存器结构定义。地址偏移量:0x0C58，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      int_type_c_int012_reg;
    struct
    {
        unsigned long  o_int_type_c_int012 : 3;  /* bit[0-2] : 增加主模产生int0、1、2的上报（(包括异频、异系统场景)），高有效。
                                                               [0]: C主模时，CTU告诉通知CPHY产生int0中断。
                                                               [1]: C主模时，CTU告诉通知CPHY产生int1中断。
                                                               [2]:CW主模时，CTU告诉通知CPHY产生int2中断。
                                                               软件读取该中断状态上报寄存器后，需要和0x0704的int_mask_c_int012相与，再判断是否需要处理。 */
        unsigned long  reserved            : 29; /* bit[3-31]: 保留 */
    } reg;
} BBPCOMM_INT_TYPE_C_INT012_UNION;
#define BBPCOMM_INT_TYPE_C_INT012_o_int_type_c_int012_START  (0)
#define BBPCOMM_INT_TYPE_C_INT012_o_int_type_c_int012_END    (2)


/*****************************************************************************
 结构名    : BBPCOMM_C_SND_LTE_INT_13_UNION
 结构说明  : C_SND_LTE_INT_13 寄存器结构定义。地址偏移量:0x0C5C，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      c_snd_lte_int_13_reg;
    struct
    {
        unsigned long  reserved_0       : 3;  /* bit[0-2]  : 保留 */
        unsigned long  c_snd_lte_int_13 : 13; /* bit[3-15] : 逻辑自清零。高有效。
                                                             
                                                             bit15~7：对应c_snd_lte_int_15 ~ 7。
                                                             CPHY通过此寄存器触发类型15~7中断（int_type_lte_rcv_c[15:7]）给LPHY。
                                                             生效时刻：立即生效
                                                             
                                                             bit6：对应c_snd_lte_int_6。
                                                             CPHY通过此寄存器触发类型6中断（int_type_lte_rcv_c[6]）给LPHY。
                                                             生效时刻：立即生效
                                                             
                                                             bit5：对应c_snd_lte_int_5。
                                                             C主模时通知CPHY L测量周期到达，高有效。
                                                             CPHY通过此寄存器触发类型5中断（int_type_lte_rcv_c[5]）给CPHY。
                                                             生效时刻：立即生效
                                                             
                                                             bit4：对应c_snd_lte_int_4。
                                                             C主模时强制退出异系统测量指示。高有效。
                                                             CPHY通过此寄存器触发类型4中断（int_type_lte_rcv_c[4]）给LPHY。
                                                             生效时刻：立即生效
                                                             
                                                             bit3：对应c_snd_lte_int_3。C测量结束指示，高有效。
                                                             用于触发中断类型3 （int_type_lte_rcv_c[3]）给LPHY。L主模下对C进行异系统测量时，如果C测量结束，CPHY配置此标志为1。 
                                                             生效时刻：立即生效 */
        unsigned long  reserved_1       : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_C_SND_LTE_INT_13_UNION;
#define BBPCOMM_C_SND_LTE_INT_13_c_snd_lte_int_13_START  (3)
#define BBPCOMM_C_SND_LTE_INT_13_c_snd_lte_int_13_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_C_ABBIF_FMT_UNION
 结构说明  : C_ABBIF_FMT 寄存器结构定义。地址偏移量:0x0C60，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      c_abbif_fmt_reg;
    struct
    {
        unsigned long  reserved_0        : 1;  /* bit[0-0]  : 保留 */
        unsigned long  c_rxb_iq_exchange : 1;  /* bit[1-1]  : 1'b1：C的RXB通路IQ路数据交换
                                                              1'b0：C的RXB通路IQ路数据不交换
                                                              V3R3不支持IQ交换功能。 */
        unsigned long  c_rxb_q_inv       : 1;  /* bit[2-2]  : 1'b1：C的RXB通路Q路数据取反
                                                              1'b0：C的RXB通路Q路数据不取反 */
        unsigned long  c_rxb_i_inv       : 1;  /* bit[3-3]  : 1'b1：C的RXB通路I路数据取反
                                                              1'b0：C的RXB通路I路数据不取反 */
        unsigned long  c_rxa_iq_exchange : 1;  /* bit[4-4]  : 1'b1：C的RXA通路IQ路数据交换
                                                              1'b0：C的RXA通路IQ路数据不交换
                                                              V3R3不支持IQ交换功能。 */
        unsigned long  c_rxa_q_inv       : 1;  /* bit[5-5]  : 1'b1：C的RXA通路Q路数据取反
                                                              1'b0：C的RXA通路Q路数据不取反 */
        unsigned long  c_rxa_i_inv       : 1;  /* bit[6-6]  : 1'b1：C的RXA通路I路数据取反
                                                              1'b0：C的RXA通路I路数据不取反 */
        unsigned long  c_tx_iq_exchange  : 1;  /* bit[7-7]  : 1'b1：C的TX通路IQ路数据交换
                                                              1'b0：C的TX通路IQ路数据不交换
                                                              V3R3不支持IQ交换功能。 */
        unsigned long  c_tx_q_inv        : 1;  /* bit[8-8]  : 1'b1：C的TX通路Q路数据取反
                                                              1'b0：C的TX通路Q路数据不取反 */
        unsigned long  c_tx_i_inv        : 1;  /* bit[9-9]  : 1'b1：C的TX通路I路数据取反
                                                              1'b0：C的TX通路I路数据不取反 */
        unsigned long  reserved_1        : 22; /* bit[10-31]: 保留 */
    } reg;
} BBPCOMM_C_ABBIF_FMT_UNION;
#define BBPCOMM_C_ABBIF_FMT_c_rxb_iq_exchange_START  (1)
#define BBPCOMM_C_ABBIF_FMT_c_rxb_iq_exchange_END    (1)
#define BBPCOMM_C_ABBIF_FMT_c_rxb_q_inv_START        (2)
#define BBPCOMM_C_ABBIF_FMT_c_rxb_q_inv_END          (2)
#define BBPCOMM_C_ABBIF_FMT_c_rxb_i_inv_START        (3)
#define BBPCOMM_C_ABBIF_FMT_c_rxb_i_inv_END          (3)
#define BBPCOMM_C_ABBIF_FMT_c_rxa_iq_exchange_START  (4)
#define BBPCOMM_C_ABBIF_FMT_c_rxa_iq_exchange_END    (4)
#define BBPCOMM_C_ABBIF_FMT_c_rxa_q_inv_START        (5)
#define BBPCOMM_C_ABBIF_FMT_c_rxa_q_inv_END          (5)
#define BBPCOMM_C_ABBIF_FMT_c_rxa_i_inv_START        (6)
#define BBPCOMM_C_ABBIF_FMT_c_rxa_i_inv_END          (6)
#define BBPCOMM_C_ABBIF_FMT_c_tx_iq_exchange_START   (7)
#define BBPCOMM_C_ABBIF_FMT_c_tx_iq_exchange_END     (7)
#define BBPCOMM_C_ABBIF_FMT_c_tx_q_inv_START         (8)
#define BBPCOMM_C_ABBIF_FMT_c_tx_q_inv_END           (8)
#define BBPCOMM_C_ABBIF_FMT_c_tx_i_inv_START         (9)
#define BBPCOMM_C_ABBIF_FMT_c_tx_i_inv_END           (9)


/*****************************************************************************
 结构名    : BBPCOMM_TDS_C_CTU_INT_SEL_UNION
 结构说明  : TDS_C_CTU_INT_SEL 寄存器结构定义。地址偏移量:0x0C64，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      tds_c_ctu_int_sel_reg;
    struct
    {
        unsigned long  tds_c_ctu_int_sel : 1;  /* bit[0]   : 1'b0：TDS CTU中断输出至物理层；
                                                             1'b1：C CTU中断输出至物理层。 */
        unsigned long  reserved          : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_TDS_C_CTU_INT_SEL_UNION;
#define BBPCOMM_TDS_C_CTU_INT_SEL_tds_c_ctu_int_sel_START  (0)
#define BBPCOMM_TDS_C_CTU_INT_SEL_tds_c_ctu_int_sel_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_S_C_CH_NUM_IND_UNION
 结构说明  : S_C_CH_NUM_IND 寄存器结构定义。地址偏移量:0x0C68，初值:0x00000000，宽度:32
 寄存器说明: GS为从模时对应通道的指示寄存器
*****************************************************************************/
typedef union
{
    unsigned long      s_c_ch_num_ind_reg;
    struct
    {
        unsigned long  s_c_ch_num_ind     : 1;  /* bit[0-0] : C为从模时对应的通道选择信号。
                                                              0：选择通道0；
                                                              1：选择通道1。
                                                              配置时刻：各模根据通信场景配置 */
        unsigned long  s_c_ch_num_ind_sel : 1;  /* bit[1-1] : C为从模时对应的通道选择信号生效方式。
                                                              0：在int1生效；
                                                              1：立即生效。 */
        unsigned long  reserved           : 30; /* bit[2-31]: 保留 */
    } reg;
} BBPCOMM_S_C_CH_NUM_IND_UNION;
#define BBPCOMM_S_C_CH_NUM_IND_s_c_ch_num_ind_START      (0)
#define BBPCOMM_S_C_CH_NUM_IND_s_c_ch_num_ind_END        (0)
#define BBPCOMM_S_C_CH_NUM_IND_s_c_ch_num_ind_sel_START  (1)
#define BBPCOMM_S_C_CH_NUM_IND_s_c_ch_num_ind_sel_END    (1)


/*****************************************************************************
 结构名    : BBPCOMM_C_CH_SW_UNION
 结构说明  : C_CH_SW 寄存器结构定义。地址偏移量:0x0C6C，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      c_ch_sw_reg;
    struct
    {
        unsigned long  c_ch_sw_rf     : 1;  /* bit[0]   : rf相关信号是否交换：
                                                          1'b0：0/1通道不交换
                                                          1'b1：0/1通道交换 */
        unsigned long  c_ch_sw_mipi   : 1;  /* bit[1]   : mipi相关信号是否交换：
                                                          1'b0：0/1通道不交换
                                                          1'b1：0/1通道交换 */
        unsigned long  c_ch_sw_abb_tx : 1;  /* bit[2]   : abb tx相关信号是否交换：
                                                          1'b0：0/1通道不交换
                                                          1'b1：0/1通道交换 */
        unsigned long  c_ch_sw_abb_rx : 1;  /* bit[3]   : abb rx相关信号是否交换：
                                                          1'b0：0/1通道不交换
                                                          1'b1：0/1通道交换 */
        unsigned long  c_ch_sw_pmu    : 1;  /* bit[4]   : pmu相关信号是否交换：
                                                          1'b0：0/1通道不交换
                                                          1'b1：0/1通道交换 */
        unsigned long  c_ch_sw_apt    : 1;  /* bit[5]   : apt相关信号是否交换：
                                                          1'b0：0/1通道不交换
                                                          1'b1：0/1通道交换 */
        unsigned long  reserved       : 26; /* bit[6-31]: 保留 */
    } reg;
} BBPCOMM_C_CH_SW_UNION;
#define BBPCOMM_C_CH_SW_c_ch_sw_rf_START      (0)
#define BBPCOMM_C_CH_SW_c_ch_sw_rf_END        (0)
#define BBPCOMM_C_CH_SW_c_ch_sw_mipi_START    (1)
#define BBPCOMM_C_CH_SW_c_ch_sw_mipi_END      (1)
#define BBPCOMM_C_CH_SW_c_ch_sw_abb_tx_START  (2)
#define BBPCOMM_C_CH_SW_c_ch_sw_abb_tx_END    (2)
#define BBPCOMM_C_CH_SW_c_ch_sw_abb_rx_START  (3)
#define BBPCOMM_C_CH_SW_c_ch_sw_abb_rx_END    (3)
#define BBPCOMM_C_CH_SW_c_ch_sw_pmu_START     (4)
#define BBPCOMM_C_CH_SW_c_ch_sw_pmu_END       (4)
#define BBPCOMM_C_CH_SW_c_ch_sw_apt_START     (5)
#define BBPCOMM_C_CH_SW_c_ch_sw_apt_END       (5)


/*****************************************************************************
 结构名    : BBPCOMM_C_TCVR_SEL0_UNION
 结构说明  : C_TCVR_SEL0 寄存器结构定义。地址偏移量:0x0C70，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      c_tcvr_sel0_reg;
    struct
    {
        unsigned long  c_tcvr_sel0 : 1;  /* bit[0]   : C输出的通道0的tcvr_on信号来源：
                                                       1'b0：来自于c bbp
                                                       1'b1：来自于dsp配置值 */
        unsigned long  reserved    : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_C_TCVR_SEL0_UNION;
#define BBPCOMM_C_TCVR_SEL0_c_tcvr_sel0_START  (0)
#define BBPCOMM_C_TCVR_SEL0_c_tcvr_sel0_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_C_TCVR_SEL1_UNION
 结构说明  : C_TCVR_SEL1 寄存器结构定义。地址偏移量:0x0C74，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      c_tcvr_sel1_reg;
    struct
    {
        unsigned long  c_tcvr_sel1 : 1;  /* bit[0]   : C输出的通道1的tcvr_on信号来源：
                                                       1'b0：来自于c bbp
                                                       1'b1：来自于dsp配置值 */
        unsigned long  reserved    : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_C_TCVR_SEL1_UNION;
#define BBPCOMM_C_TCVR_SEL1_c_tcvr_sel1_START  (0)
#define BBPCOMM_C_TCVR_SEL1_c_tcvr_sel1_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_C_TCVR_VALUE0_UNION
 结构说明  : C_TCVR_VALUE0 寄存器结构定义。地址偏移量:0x0C78，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      c_tcvr_value0_reg;
    struct
    {
        unsigned long  c_tcvr_value0 : 1;  /* bit[0]   : dsp配置的c模送给通道0 的tcvr_on值 */
        unsigned long  reserved      : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_C_TCVR_VALUE0_UNION;
#define BBPCOMM_C_TCVR_VALUE0_c_tcvr_value0_START  (0)
#define BBPCOMM_C_TCVR_VALUE0_c_tcvr_value0_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_C_TCVR_VALUE1_UNION
 结构说明  : C_TCVR_VALUE1 寄存器结构定义。地址偏移量:0x0C7C，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      c_tcvr_value1_reg;
    struct
    {
        unsigned long  c_tcvr_value1 : 1;  /* bit[0]   : dsp配置的c模送给通道1 的tcvr_on值 */
        unsigned long  reserved      : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_C_TCVR_VALUE1_UNION;
#define BBPCOMM_C_TCVR_VALUE1_c_tcvr_value1_START  (0)
#define BBPCOMM_C_TCVR_VALUE1_c_tcvr_value1_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_COMM_32K_TIMING_RPT2_UNION
 结构说明  : COMM_32K_TIMING_RPT2 寄存器结构定义。地址偏移量:0x0F04，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      comm_32k_timing_rpt2_reg;
    struct
    {
        unsigned long  o_adjust_timing : 24; /* bit[0-23] : 32K校准定时器的定时信息，由DSP 配置timing_get_ind触发上报。
                                                            测试用
                                                            上报时刻：comm_32k_timing_valid为高时上报有效 */
        unsigned long  reserved        : 8;  /* bit[24-31]: 保留 */
    } reg;
} BBPCOMM_COMM_32K_TIMING_RPT2_UNION;
#define BBPCOMM_COMM_32K_TIMING_RPT2_o_adjust_timing_START  (0)
#define BBPCOMM_COMM_32K_TIMING_RPT2_o_adjust_timing_END    (23)


/*****************************************************************************
 结构名    : BBPCOMM_ABB_RXB_LOCK_MODE_UNION
 结构说明  : ABB_RXB_LOCK_MODE 寄存器结构定义。地址偏移量:0x0F08，初值:0x00000001，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      abb_rxb_lock_mode_reg;
    struct
    {
        unsigned long  cpu_abb_mask_cnt_num : 16; /* bit[0-15] : ABB CH0 RXB和CH1 RXB通道互锁延迟计数值，即通道切换的互锁时间，计数周期为总线时钟（150M），配置值必须大于等于1。 */
        unsigned long  cpu_abb_lock_en      : 1;  /* bit[16]   : ABB CH0 RXB通道和CH1 RXB通道互锁功能使能信号，1表示有效 */
        unsigned long  reserved             : 15; /* bit[17-31]: 保留 */
    } reg;
} BBPCOMM_ABB_RXB_LOCK_MODE_UNION;
#define BBPCOMM_ABB_RXB_LOCK_MODE_cpu_abb_mask_cnt_num_START  (0)
#define BBPCOMM_ABB_RXB_LOCK_MODE_cpu_abb_mask_cnt_num_END    (15)
#define BBPCOMM_ABB_RXB_LOCK_MODE_cpu_abb_lock_en_START       (16)
#define BBPCOMM_ABB_RXB_LOCK_MODE_cpu_abb_lock_en_END         (16)


/*****************************************************************************
 结构名    : BBPCOMM_DBG_EN_UNION
 结构说明  : DBG_EN 寄存器结构定义。地址偏移量:0x0F80，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      dbg_en_reg;
    struct
    {
        unsigned long  dbg_en       : 1;  /* bit[0]    : 可维可测使能信号，当dbg_rpt_mode为0时逻辑自清，否则需要写清。
                                                         1'b0：可维可测功能不使能                             1'b1：可维可测功能使能 */
        unsigned long  dbg_rpt_mode : 1;  /* bit[1]    : 可维可测数据上报模式：
                                                         'b0：触发后继续采样dbg_pkg_num个数据包后自动结束；
                                                         'b1：只要dbg_en为1就上报，没有上报个数限制。 */
        unsigned long  reserved     : 14; /* bit[2-15] : 保留 */
        unsigned long  dbg_pkg_num  : 16; /* bit[16-31]: 可维可测的包个数 */
    } reg;
} BBPCOMM_DBG_EN_UNION;
#define BBPCOMM_DBG_EN_dbg_en_START        (0)
#define BBPCOMM_DBG_EN_dbg_en_END          (0)
#define BBPCOMM_DBG_EN_dbg_rpt_mode_START  (1)
#define BBPCOMM_DBG_EN_dbg_rpt_mode_END    (1)
#define BBPCOMM_DBG_EN_dbg_pkg_num_START   (16)
#define BBPCOMM_DBG_EN_dbg_pkg_num_END     (31)


/*****************************************************************************
 结构名    : BBPCOMM_DBG_FLT_UNION
 结构说明  : DBG_FLT 寄存器结构定义。地址偏移量:0x0F88，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      dbg_flt_reg;
    struct
    {
        unsigned long  dbg_flt  : 16; /* bit[0-15] : 可维可测内部分组指示：
                                                     1：上报ssi通道0数据
                                                     2：上报ssi通道1数据
                                                     3：上报mipi通道0数据
                                                     4：上报mipi通道1数据
                                                     其它：保留 */
        unsigned long  reserved : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_DBG_FLT_UNION;
#define BBPCOMM_DBG_FLT_dbg_flt_START   (0)
#define BBPCOMM_DBG_FLT_dbg_flt_END     (15)


/*****************************************************************************
 结构名    : BBPCOMM_DBG_REQ_EN_UNION
 结构说明  : DBG_REQ_EN 寄存器结构定义。地址偏移量:0x0F8C，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      dbg_req_en_reg;
    struct
    {
        unsigned long  dbg_req_en : 20; /* bit[0-19] : 可维可测轮询请求上报信号使能，dbg_req_en[19:0]对应reg[19:0],如dbg_req_en[19]为1，reg[19]请求有效，dbg_req_en[19]为0，reg[19]请求无效 */
        unsigned long  reserved   : 12; /* bit[20-31]: 保留 */
    } reg;
} BBPCOMM_DBG_REQ_EN_UNION;
#define BBPCOMM_DBG_REQ_EN_dbg_req_en_START  (0)
#define BBPCOMM_DBG_REQ_EN_dbg_req_en_END    (19)


/*****************************************************************************
 结构名    : BBPCOMM_MIPI_DYN_EXCHG_UNION
 结构说明  : MIPI_DYN_EXCHG 寄存器结构定义。地址偏移量:0x0F90，初值:0x00000001，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      mipi_dyn_exchg_reg;
    struct
    {
        unsigned long  mipi_dyn_exchg_en      : 1;  /* bit[0]   : MIPI通道动态切换使能，1：动态切换开启，0：动态切换关闭 */
        unsigned long  mipi_robin_clr_ind_imi : 1;  /* bit[1]   : MIPI轮询请求清零信号，1：清除清零信号，0：无效 */
        unsigned long  reserved               : 30; /* bit[2-31]: 保留 */
    } reg;
} BBPCOMM_MIPI_DYN_EXCHG_UNION;
#define BBPCOMM_MIPI_DYN_EXCHG_mipi_dyn_exchg_en_START       (0)
#define BBPCOMM_MIPI_DYN_EXCHG_mipi_dyn_exchg_en_END         (0)
#define BBPCOMM_MIPI_DYN_EXCHG_mipi_robin_clr_ind_imi_START  (1)
#define BBPCOMM_MIPI_DYN_EXCHG_mipi_robin_clr_ind_imi_END    (1)


/*****************************************************************************
 结构名    : BBPCOMM_TIMING_CLR_DELAY_CNT_UNION
 结构说明  : TIMING_CLR_DELAY_CNT 寄存器结构定义。地址偏移量:0x0F94，初值:0x00004B00，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      timing_clr_delay_cnt_reg;
    struct
    {
        unsigned long  timing_clr_delay_cnt : 16; /* bit[0-15] : dsp读取定时信息超时保护时间，默认为0x4b00（1ms） */
        unsigned long  reserved             : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_TIMING_CLR_DELAY_CNT_UNION;
#define BBPCOMM_TIMING_CLR_DELAY_CNT_timing_clr_delay_cnt_START  (0)
#define BBPCOMM_TIMING_CLR_DELAY_CNT_timing_clr_delay_cnt_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_TIMING_GET_MODE_UNION
 结构说明  : TIMING_GET_MODE 寄存器结构定义。地址偏移量:0x0F98，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      timing_get_mode_reg;
    struct
    {
        unsigned long  timing_get_mode : 1;  /* bit[0]   : 定时信息上报模式选择,
                                                           0：只使用一套配置和上报接口获取定时信息
                                                           1：使用2套配置和上报接口，bbe16和arm各用1套接口获取定时信息 */
        unsigned long  reserved        : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_TIMING_GET_MODE_UNION;
#define BBPCOMM_TIMING_GET_MODE_timing_get_mode_START  (0)
#define BBPCOMM_TIMING_GET_MODE_timing_get_mode_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_TIMING_CLEAR1_UNION
 结构说明  : TIMING_CLEAR1 寄存器结构定义。地址偏移量:0x0F9C，初值:0x00000000，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      timing_clear1_reg;
    struct
    {
        unsigned long  timing_clear1          : 1;  /* bit[0]   : 逻辑自清零。
                                                                  定时标志清除，用于清除timing_valid1标志。
                                                                  配置时刻：DSP读取定时完毕后配置
                                                                  生效时刻：立即生效。
                                                                  （timing_get_mode=1，arm控制） */
        unsigned long  comm_32k_timing_clear1 : 1;  /* bit[1]   : 逻辑自清零。
                                                                  32公共定时信息的标志清除，用于清除timing_valid1标志。
                                                                  配置时刻：DSP读取定时完毕后配置
                                                                  生效时刻：立即生效
                                                                  （因为32K定时信息的上报延时远大于其他系统的定时信息，所以单独处理）
                                                                  （timing_get_mode=0，1套查询接口；
                                                                  timing_get_mode=1，2套查询接口） */
        unsigned long  reserved               : 30; /* bit[2-31]: 保留 */
    } reg;
} BBPCOMM_TIMING_CLEAR1_UNION;
#define BBPCOMM_TIMING_CLEAR1_timing_clear1_START           (0)
#define BBPCOMM_TIMING_CLEAR1_timing_clear1_END             (0)
#define BBPCOMM_TIMING_CLEAR1_comm_32k_timing_clear1_START  (1)
#define BBPCOMM_TIMING_CLEAR1_comm_32k_timing_clear1_END    (1)


/*****************************************************************************
 结构名    : BBPCOMM_TIMING_GET_IND1_UNION
 结构说明  : TIMING_GET_IND1 寄存器结构定义。地址偏移量:0x0F10，初值:0x00000030，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      timing_get_ind1_reg;
    struct
    {
        unsigned long  timing_get_ind1   : 1;  /* bit[0]   : 逻辑自清零。
                                                             timing_get_mode=1，arm需要获取定时关系时配置生效，CTU逻辑根据此信号锁存LTE/W/Gm/Gs/TDS/C的系统定时，CTU的内部定时供arm查询。
                                                             配置时刻：软件需要锁存定时
                                                             生效时刻：立即生效 */
        unsigned long  reserved_0        : 3;  /* bit[1-3] : 保留 */
        unsigned long  timing_get_c_sel1 : 2;  /* bit[4-5] : 表示timing_get_mode=1,arm读取c模bbp计数器结果1x、evdo还是base_cnt。   00：1x定时，01：evdo定时，10： base_cnt ，11：无效。
                                                             timing_get_mode=0，不需配置，无意义 */
        unsigned long  reserved_1        : 26; /* bit[6-31]: 保留 */
    } reg;
} BBPCOMM_TIMING_GET_IND1_UNION;
#define BBPCOMM_TIMING_GET_IND1_timing_get_ind1_START    (0)
#define BBPCOMM_TIMING_GET_IND1_timing_get_ind1_END      (0)
#define BBPCOMM_TIMING_GET_IND1_timing_get_c_sel1_START  (4)
#define BBPCOMM_TIMING_GET_IND1_timing_get_c_sel1_END    (5)


/*****************************************************************************
 结构名    : BBPCOMM_TINMG_VALID1_UNION
 结构说明  : TINMG_VALID1 寄存器结构定义。地址偏移量:0x0F14，初值:0x00000030，宽度:32
 寄存器说明: 寄存器
*****************************************************************************/
typedef union
{
    unsigned long      tinmg_valid1_reg;
    struct
    {
        unsigned long  o_timing_valid1        : 1;  /* bit[0]   : 定时上报有效标志，高有效。
                                                                  DSP配置timing_get_ind1后，查询此标志，如果为高，则可以获取LTE，W，Gm，Gs, TDS的系统定时。
                                                                  DSP读取定时完毕后配置timing_clear1寄存器，timing_valid1拉低为无效。
                                                                  CTU检测到timing_get_ind1后固定延迟delay_cnt后拉高timing_valid1。
                                                                  
                                                                  配置时刻：
                                                                  生效时刻：立即生效
                                                                  （timing_get_mode=1，arm控制） */
        unsigned long  comm_32k_timing_valid1 : 1;  /* bit[1]   : 32K公共定时上报有效标志，高有效。
                                                                  DSP配置timing_get_ind1后，查询此标志，如果为高，则可以获取32K公共定时器的定时信息。
                                                                  DSP读取定时完毕后配置comm_32k_timing_clear1寄存器，comm_32k_timing_valid1拉低为无效。
                                                                  32K定时锁存稳定后拉高comm_32k_timing_valid1
                                                                  配置时刻：
                                                                  生效时刻：立即生效
                                                                  （timing_get_mode=1，arm控制） */
        unsigned long  overtime_ind1          : 1;  /* bit[2]   : 表示arm读取定时信息超时（在超时保护时间内未给出o_timing_clear信号） */
        unsigned long  reserved_0             : 1;  /* bit[3]   : 保留 */
        unsigned long  timing_valid_c_sel1    : 2;  /* bit[4-5] : 表示o_timing_valid有效时，上报的计数器给arm的是c模bbp的1x、evdo还是base_cnt
                                                                  00：1x定时
                                                                  01： evdo定时
                                                                  10： basecent */
        unsigned long  reserved_1             : 26; /* bit[6-31]: 保留 */
    } reg;
} BBPCOMM_TINMG_VALID1_UNION;
#define BBPCOMM_TINMG_VALID1_o_timing_valid1_START         (0)
#define BBPCOMM_TINMG_VALID1_o_timing_valid1_END           (0)
#define BBPCOMM_TINMG_VALID1_comm_32k_timing_valid1_START  (1)
#define BBPCOMM_TINMG_VALID1_comm_32k_timing_valid1_END    (1)
#define BBPCOMM_TINMG_VALID1_overtime_ind1_START           (2)
#define BBPCOMM_TINMG_VALID1_overtime_ind1_END             (2)
#define BBPCOMM_TINMG_VALID1_timing_valid_c_sel1_START     (4)
#define BBPCOMM_TINMG_VALID1_timing_valid_c_sel1_END       (5)


/*****************************************************************************
 结构名    : BBPCOMM_TIMING_GET_W_SEL_UNION
 结构说明  : TIMING_GET_W_SEL 寄存器结构定义。地址偏移量:0x0F18，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      timing_get_w_sel_reg;
    struct
    {
        unsigned long  timing_get_w_sel : 2;  /* bit[0-1] : 表示timing_get_mode=1时bbe读取W模bbp计数器结果sys_cnt还是base_cnt。00（二进制）：sys_cnt，01：base_cnt 10：无效 11：无效
                                                            timing_get_mode=0，不需配置，无意义 */
        unsigned long  reserved         : 30; /* bit[2-31]: 保留 */
    } reg;
} BBPCOMM_TIMING_GET_W_SEL_UNION;
#define BBPCOMM_TIMING_GET_W_SEL_timing_get_w_sel_START  (0)
#define BBPCOMM_TIMING_GET_W_SEL_timing_get_w_sel_END    (1)


/*****************************************************************************
 结构名    : BBPCOMM_TIMING_VALID_W_SEL_UNION
 结构说明  : TIMING_VALID_W_SEL 寄存器结构定义。地址偏移量:0x0F1C，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      timing_valid_w_sel_reg;
    struct
    {
        unsigned long  timing_valid_w_sel : 2;  /* bit[0-1] : 表示o_timing_valid有效时，上报w模的计数器给bbe16的是sys_cnt还是base_cnt
                                                              00（二进制）： sys_cnt
                                                              01： basecent 10：无效  11：无效  */
        unsigned long  reserved           : 30; /* bit[2-31]: 保留 */
    } reg;
} BBPCOMM_TIMING_VALID_W_SEL_UNION;
#define BBPCOMM_TIMING_VALID_W_SEL_timing_valid_w_sel_START  (0)
#define BBPCOMM_TIMING_VALID_W_SEL_timing_valid_w_sel_END    (1)


/*****************************************************************************
 结构名    : BBPCOMM_TIMING_GET_W_SEL1_UNION
 结构说明  : TIMING_GET_W_SEL1 寄存器结构定义。地址偏移量:0x0F28，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      timing_get_w_sel1_reg;
    struct
    {
        unsigned long  timing_get_w_sel1 : 2;  /* bit[0-1] : 表示timing_get_mode=1时arm读取w模计数器结果sys_cnt还是base_cnt。   00（二进制）：sys_cnt，01：base_cnt， 10：无效 11 无效
                                                             timing_get_mode=0时，不需配置，无意义 */
        unsigned long  reserved          : 30; /* bit[2-31]: 保留 */
    } reg;
} BBPCOMM_TIMING_GET_W_SEL1_UNION;
#define BBPCOMM_TIMING_GET_W_SEL1_timing_get_w_sel1_START  (0)
#define BBPCOMM_TIMING_GET_W_SEL1_timing_get_w_sel1_END    (1)


/*****************************************************************************
 结构名    : BBPCOMM_TIMING_VALID_W_SEL1_UNION
 结构说明  : TIMING_VALID_W_SEL1 寄存器结构定义。地址偏移量:0x0F2C，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      timing_valid_w_sel1_reg;
    struct
    {
        unsigned long  timing_valid_w_sel1 : 2;  /* bit[0-1] : 表示o_timing_valid有效时，上报的计数器给arm的是w模bbp的sys_cnt还是base_cnt
                                                               00：sys_cnt定时
                                                               01：base_cnt定时 
                                                               10：无效
                                                               11：无效 */
        unsigned long  reserved            : 30; /* bit[2-31]: 保留 */
    } reg;
} BBPCOMM_TIMING_VALID_W_SEL1_UNION;
#define BBPCOMM_TIMING_VALID_W_SEL1_timing_valid_w_sel1_START  (0)
#define BBPCOMM_TIMING_VALID_W_SEL1_timing_valid_w_sel1_END    (1)


/*****************************************************************************
 结构名    : BBPCOMM_CPU_CFG_SC_EN_UNION
 结构说明  : CPU_CFG_SC_EN 寄存器结构定义。地址偏移量:0x0F30，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      cpu_cfg_sc_en_reg;
    struct
    {
        unsigned long  cpu_cfg_sc_en : 1;  /* bit[0]   : comm控制sc_pll使能，1：通过comm控制sc_pll，0：不通过comm控制sc_pll */
        unsigned long  reserved      : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_CPU_CFG_SC_EN_UNION;
#define BBPCOMM_CPU_CFG_SC_EN_cpu_cfg_sc_en_START  (0)
#define BBPCOMM_CPU_CFG_SC_EN_cpu_cfg_sc_en_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_CPU_INT1_CFG_SC_EN_UNION
 结构说明  : CPU_INT1_CFG_SC_EN 寄存器结构定义。地址偏移量:0x0F34，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      cpu_int1_cfg_sc_en_reg;
    struct
    {
        unsigned long  cpu_int1_cfg_sc_en : 1;  /* bit[0]   : comm向sc_pll发送中断1使能，1：有效，0无效 */
        unsigned long  reserved           : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_CPU_INT1_CFG_SC_EN_UNION;
#define BBPCOMM_CPU_INT1_CFG_SC_EN_cpu_int1_cfg_sc_en_START  (0)
#define BBPCOMM_CPU_INT1_CFG_SC_EN_cpu_int1_cfg_sc_en_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_CPU_INT2_CFG_SC_EN_UNION
 结构说明  : CPU_INT2_CFG_SC_EN 寄存器结构定义。地址偏移量:0x0F38，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      cpu_int2_cfg_sc_en_reg;
    struct
    {
        unsigned long  cpu_int2_cfg_sc_en : 1;  /* bit[0]   : comm向sc_pll发送中断2使能，1：有效，0无效 */
        unsigned long  reserved           : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_CPU_INT2_CFG_SC_EN_UNION;
#define BBPCOMM_CPU_INT2_CFG_SC_EN_cpu_int2_cfg_sc_en_START  (0)
#define BBPCOMM_CPU_INT2_CFG_SC_EN_cpu_int2_cfg_sc_en_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_CPU_PRO_SOFT_CFG_SC_IND_UNION
 结构说明  : CPU_PRO_SOFT_CFG_SC_IND 寄存器结构定义。地址偏移量:0x0F3C，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      cpu_pro_soft_cfg_sc_ind_reg;
    struct
    {
        unsigned long  cpu_pro_sfot_cfg_sc_ind : 1;  /* bit[0]   : 软件向sc_pll配置的主模的ind信号 */
        unsigned long  reserved                : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_CPU_PRO_SOFT_CFG_SC_IND_UNION;
#define BBPCOMM_CPU_PRO_SOFT_CFG_SC_IND_cpu_pro_sfot_cfg_sc_ind_START  (0)
#define BBPCOMM_CPU_PRO_SOFT_CFG_SC_IND_cpu_pro_sfot_cfg_sc_ind_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_CPU_SEC_SOFT_CFG_SC_IND_UNION
 结构说明  : CPU_SEC_SOFT_CFG_SC_IND 寄存器结构定义。地址偏移量:0x0F40，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      cpu_sec_soft_cfg_sc_ind_reg;
    struct
    {
        unsigned long  cpu_sec_sfot_cfg_sc_ind : 1;  /* bit[0]   : 软件向sc_pll配置的从模的ind信号 */
        unsigned long  reserved                : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_CPU_SEC_SOFT_CFG_SC_IND_UNION;
#define BBPCOMM_CPU_SEC_SOFT_CFG_SC_IND_cpu_sec_sfot_cfg_sc_ind_START  (0)
#define BBPCOMM_CPU_SEC_SOFT_CFG_SC_IND_cpu_sec_sfot_cfg_sc_ind_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_CPU_SC_CH_SEL_UNION
 结构说明  : CPU_SC_CH_SEL 寄存器结构定义。地址偏移量:0x0F44，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      cpu_sc_ch_sel_reg;
    struct
    {
        unsigned long  cpu_sc_ch_sel : 1;  /* bit[0]   : 1：根据通道1主从模产生配置sc_pll信号
                                                         0：根据通道0主从模产生配置sc_pll信号 */
        unsigned long  reserved      : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_CPU_SC_CH_SEL_UNION;
#define BBPCOMM_CPU_SC_CH_SEL_cpu_sc_ch_sel_START  (0)
#define BBPCOMM_CPU_SC_CH_SEL_cpu_sc_ch_sel_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_CTU_TIMING_ROBIN_CLR_IND_UNION
 结构说明  : CTU_TIMING_ROBIN_CLR_IND 寄存器结构定义。地址偏移量:0x0F4C，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      ctu_timing_robin_clr_ind_reg;
    struct
    {
        unsigned long  ctu_timing_robin_clr_ind : 1;  /* bit[0]   : ctu中两套定时上报轮询请求清零信号，1：清除清零信号，0：无效 */
        unsigned long  reserved                 : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_CTU_TIMING_ROBIN_CLR_IND_UNION;
#define BBPCOMM_CTU_TIMING_ROBIN_CLR_IND_ctu_timing_robin_clr_ind_START  (0)
#define BBPCOMM_CTU_TIMING_ROBIN_CLR_IND_ctu_timing_robin_clr_ind_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_GU_SDR_EN_UNION
 结构说明  : GU_SDR_EN 寄存器结构定义。地址偏移量:0x0FFC，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      gu_sdr_en_reg;
    struct
    {
        unsigned long  g_sdr_en          : 1;  /* bit[0]   : G SDR模式开关使能。 */
        unsigned long  w_sdr_en          : 1;  /* bit[1]   : W SDR模式开关使能。 */
        unsigned long  w_sdr_dec_clk_sel : 1;  /* bit[2]   : W SDR模式下译码时钟选择。0，译码采用122m时钟；1，译码采用245m时钟。 */
        unsigned long  reserved          : 29; /* bit[3-31]: 保留 */
    } reg;
} BBPCOMM_GU_SDR_EN_UNION;
#define BBPCOMM_GU_SDR_EN_g_sdr_en_START           (0)
#define BBPCOMM_GU_SDR_EN_g_sdr_en_END             (0)
#define BBPCOMM_GU_SDR_EN_w_sdr_en_START           (1)
#define BBPCOMM_GU_SDR_EN_w_sdr_en_END             (1)
#define BBPCOMM_GU_SDR_EN_w_sdr_dec_clk_sel_START  (2)
#define BBPCOMM_GU_SDR_EN_w_sdr_dec_clk_sel_END    (2)


/***======================================================================***
                     (2/5) register_define_bbpmst
 ***======================================================================***/
/*****************************************************************************
 结构名    : BBPCOMM_BBPMST_WR_CH0_MODE_UNION
 结构说明  : BBPMST_WR_CH0_MODE 寄存器结构定义。地址偏移量:0x000，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      bbpmst_wr_ch0_mode_reg;
    struct
    {
        unsigned long  bbpmst_wr_ch0_mode : 3;  /* bit[0-2] : 该写通道的工作模式：每个通道可以由最多3个不同时发生的数据流复用；bbpmst_wr_ch0_mode代表写通道0的工作模式
                                                              1）3'b000 ：通道非使能，此时该通道向前级的BBP输出valid/ready为0，向后级的axi接口不再发出新的读/写请求，如果所有的读写通道都没有新请求了，且axi口以及总线被释放（是否被释放的状态在cpu接口有上报），软件结合该状态可启动门控或掉电流程
                                                              2）3'b001：通道配置给第一组接口使用
                                                              3）3'b010：通道配置给第二组接口使用
                                                              4）3'b100：通道配置给第三组接口使用 */
        unsigned long  reserved           : 29; /* bit[3-31]: 保留 */
    } reg;
} BBPCOMM_BBPMST_WR_CH0_MODE_UNION;
#define BBPCOMM_BBPMST_WR_CH0_MODE_bbpmst_wr_ch0_mode_START  (0)
#define BBPCOMM_BBPMST_WR_CH0_MODE_bbpmst_wr_ch0_mode_END    (2)


/*****************************************************************************
 结构名    : BBPCOMM_BBPMST_WR_CH0_CLK_SEL_UNION
 结构说明  : BBPMST_WR_CH0_CLK_SEL 寄存器结构定义。地址偏移量:0x0004，初值:0x00000001，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      bbpmst_wr_ch0_clk_sel_reg;
    struct
    {
        unsigned long  bbpmst_wr_ch0_clk_sel : 3;  /* bit[0-2] : 该写通道的输入时钟域选择：
                                                                 1）3’b001：该写通道的输入接口使用第一种时钟域
                                                                 2）3’b010：该写通道的输入接口使用第二种时钟域
                                                                 3）3’b100：该写通道的输入接口使用第三种时钟域
                                                                 在v8r1中的第一种接口时钟为：104MHz，为g模的工作时钟
                                                                 在v8r1中的第二种接口时钟为：122.88MHz，为C模以及U模中除译码外的逻辑的工作时钟，且各模的cpu接口也使用该时钟域
                                                                 在v8r1中的第三种接口时钟为：245.76MHz，为U模中译码的工作时钟 */
        unsigned long  reserved              : 29; /* bit[3-31]: 保留 */
    } reg;
} BBPCOMM_BBPMST_WR_CH0_CLK_SEL_UNION;
#define BBPCOMM_BBPMST_WR_CH0_CLK_SEL_bbpmst_wr_ch0_clk_sel_START  (0)
#define BBPCOMM_BBPMST_WR_CH0_CLK_SEL_bbpmst_wr_ch0_clk_sel_END    (2)


/*****************************************************************************
 结构名    : BBPCOMM_BBPMST_WR_CH0_BURST_BEAT_UNION
 结构说明  : BBPMST_WR_CH0_BURST_BEAT 寄存器结构定义。地址偏移量:0x0010，初值:0x00000008，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      bbpmst_wr_ch0_burst_beat_reg;
    struct
    {
        unsigned long  bbpmst_wr_ch0_burst_beat : 4;  /* bit[0-3] : 用Master方式完成DPA上报时，单个burst的类型，含义如下：
                                                                    burst_beat==0， burst1
                                                                    burst_beat==3， burst4
                                                                    burst_beat==7， burst8
                                                                    burst_beat==15，burst16
                                                                    如果决定使用burst8， 则此种配置下硬件在读写DDR时会固定向总线发出burst8请求，同时硬件支持数据块的尾部不是一个完整的burst8  */
        unsigned long  reserved                 : 28; /* bit[4-31]: 保留 */
    } reg;
} BBPCOMM_BBPMST_WR_CH0_BURST_BEAT_UNION;
#define BBPCOMM_BBPMST_WR_CH0_BURST_BEAT_bbpmst_wr_ch0_burst_beat_START  (0)
#define BBPCOMM_BBPMST_WR_CH0_BURST_BEAT_bbpmst_wr_ch0_burst_beat_END    (3)


/*****************************************************************************
 结构名    : BBPCOMM_BBPMST_WR_CH0_AXI_SEL_UNION
 结构说明  : BBPMST_WR_CH0_AXI_SEL 寄存器结构定义。地址偏移量:0x0014，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      bbpmst_wr_ch0_axi_sel_reg;
    struct
    {
        unsigned long  bbpmst_wr_ch0_axi_sel : 1;  /* bit[0]   : 本通道所选择使用的master口：
                                                                 1'b0：该通道使用master口1
                                                                 1'b1：该通道使用master口2 */
        unsigned long  reserved              : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_BBPMST_WR_CH0_AXI_SEL_UNION;
#define BBPCOMM_BBPMST_WR_CH0_AXI_SEL_bbpmst_wr_ch0_axi_sel_START  (0)
#define BBPCOMM_BBPMST_WR_CH0_AXI_SEL_bbpmst_wr_ch0_axi_sel_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_BBPMST_WR_CH1_MODE_UNION
 结构说明  : BBPMST_WR_CH1_MODE 寄存器结构定义。地址偏移量:0x020，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      bbpmst_wr_ch1_mode_reg;
    struct
    {
        unsigned long  bbpmst_wr_ch1_mode : 3;  /* bit[0-2] : 该写通道的工作模式：每个通道可以由最多3个不同时发生的数据流复用；bbpmst_wr_ch0_mode代表写通道0的工作模式
                                                              1）3'b000 ：通道非使能，此时该通道向前级的BBP输出valid/ready为0，向后级的axi接口不再发出新的读/写请求，如果所有的读写通道都没有新请求了，且axi口以及总线被释放（是否被释放的状态在cpu接口有上报），软件结合该状态可启动门控或掉电流程
                                                              2）3'b001：通道配置给第一组接口使用
                                                              3）3'b010：通道配置给第二组接口使用
                                                              4）3'b100：通道配置给第三组接口使用 */
        unsigned long  reserved           : 29; /* bit[3-31]: 保留 */
    } reg;
} BBPCOMM_BBPMST_WR_CH1_MODE_UNION;
#define BBPCOMM_BBPMST_WR_CH1_MODE_bbpmst_wr_ch1_mode_START  (0)
#define BBPCOMM_BBPMST_WR_CH1_MODE_bbpmst_wr_ch1_mode_END    (2)


/*****************************************************************************
 结构名    : BBPCOMM_BBPMST_WR_CH1_CLK_SEL_UNION
 结构说明  : BBPMST_WR_CH1_CLK_SEL 寄存器结构定义。地址偏移量:0x0024，初值:0x00000001，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      bbpmst_wr_ch1_clk_sel_reg;
    struct
    {
        unsigned long  bbpmst_wr_ch1_clk_sel : 3;  /* bit[0-2] : 该写通道的输入时钟域选择：
                                                                 1）3’b001：该写通道的输入接口使用第一种时钟域
                                                                 2）3’b010：该写通道的输入接口使用第二种时钟域
                                                                 3）3’b100：该写通道的输入接口使用第三种时钟域
                                                                 在v8r1中的第一种接口时钟为：104MHz，为g模的工作时钟
                                                                 在v8r1中的第二种接口时钟为：122.88MHz，为C模以及U模中除译码外的逻辑的工作时钟，且各模的cpu接口也使用该时钟域
                                                                 在v8r1中的第三种接口时钟为：245.76MHz，为U模中译码的工作时钟 */
        unsigned long  reserved              : 29; /* bit[3-31]: 保留 */
    } reg;
} BBPCOMM_BBPMST_WR_CH1_CLK_SEL_UNION;
#define BBPCOMM_BBPMST_WR_CH1_CLK_SEL_bbpmst_wr_ch1_clk_sel_START  (0)
#define BBPCOMM_BBPMST_WR_CH1_CLK_SEL_bbpmst_wr_ch1_clk_sel_END    (2)


/*****************************************************************************
 结构名    : BBPCOMM_BBPMST_WR_CH1_BURST_BEAT_UNION
 结构说明  : BBPMST_WR_CH1_BURST_BEAT 寄存器结构定义。地址偏移量:0x0030，初值:0x00000008，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      bbpmst_wr_ch1_burst_beat_reg;
    struct
    {
        unsigned long  bbpmst_wr_ch1_burst_beat : 4;  /* bit[0-3] : 用Master方式完成DPA上报时，单个burst的类型，含义如下：
                                                                    burst_beat==0， burst1
                                                                    burst_beat==3， burst4
                                                                    burst_beat==7， burst8
                                                                    burst_beat==15，burst16
                                                                    如果决定使用burst8， 则此种配置下硬件在读写DDR时会固定向总线发出burst8请求，同时硬件支持数据块的尾部不是一个完整的burst8  */
        unsigned long  reserved                 : 28; /* bit[4-31]: 保留 */
    } reg;
} BBPCOMM_BBPMST_WR_CH1_BURST_BEAT_UNION;
#define BBPCOMM_BBPMST_WR_CH1_BURST_BEAT_bbpmst_wr_ch1_burst_beat_START  (0)
#define BBPCOMM_BBPMST_WR_CH1_BURST_BEAT_bbpmst_wr_ch1_burst_beat_END    (3)


/*****************************************************************************
 结构名    : BBPCOMM_BBPMST_WR_CH1_AXI_SEL_UNION
 结构说明  : BBPMST_WR_CH1_AXI_SEL 寄存器结构定义。地址偏移量:0x0034，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      bbpmst_wr_ch1_axi_sel_reg;
    struct
    {
        unsigned long  bbpmst_wr_ch1_axi_sel : 1;  /* bit[0]   : 本通道所选择使用的master口：
                                                                 1'b0：该通道使用master口1
                                                                 1'b1：该通道使用master口2 */
        unsigned long  reserved              : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_BBPMST_WR_CH1_AXI_SEL_UNION;
#define BBPCOMM_BBPMST_WR_CH1_AXI_SEL_bbpmst_wr_ch1_axi_sel_START  (0)
#define BBPCOMM_BBPMST_WR_CH1_AXI_SEL_bbpmst_wr_ch1_axi_sel_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_BBPMST_WR_CH2_MODE_UNION
 结构说明  : BBPMST_WR_CH2_MODE 寄存器结构定义。地址偏移量:0x0040，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      bbpmst_wr_ch2_mode_reg;
    struct
    {
        unsigned long  bbpmst_wr_ch2_mode : 3;  /* bit[0-2] : 该写通道的工作模式：每个通道可以由最多3个不同时发生的数据流复用；bbpmst_wr_ch0_mode代表写通道0的工作模式
                                                              1）3'b000 ：通道非使能，此时该通道向前级的BBP输出valid/ready为0，向后级的axi接口不再发出新的读/写请求，如果所有的读写通道都没有新请求了，且axi口以及总线被释放（是否被释放的状态在cpu接口有上报），软件结合该状态可启动门控或掉电流程
                                                              2）3'b001：通道配置给第一组接口使用
                                                              3）3'b010：通道配置给第二组接口使用
                                                              4）3'b100：通道配置给第三组接口使用 */
        unsigned long  reserved           : 29; /* bit[3-31]: 保留 */
    } reg;
} BBPCOMM_BBPMST_WR_CH2_MODE_UNION;
#define BBPCOMM_BBPMST_WR_CH2_MODE_bbpmst_wr_ch2_mode_START  (0)
#define BBPCOMM_BBPMST_WR_CH2_MODE_bbpmst_wr_ch2_mode_END    (2)


/*****************************************************************************
 结构名    : BBPCOMM_BBPMST_WR_CH2_CLK_SEL_UNION
 结构说明  : BBPMST_WR_CH2_CLK_SEL 寄存器结构定义。地址偏移量:0x0044，初值:0x00000001，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      bbpmst_wr_ch2_clk_sel_reg;
    struct
    {
        unsigned long  bbpmst_wr_ch2_clk_sel : 3;  /* bit[0-2] : 该写通道的输入时钟域选择：
                                                                 1）3’b001：该写通道的输入接口使用第一种时钟域
                                                                 2）3’b010：该写通道的输入接口使用第二种时钟域
                                                                 3）3’b100：该写通道的输入接口使用第三种时钟域
                                                                 在v8r1中的第一种接口时钟为：104MHz，为g模的工作时钟
                                                                 在v8r1中的第二种接口时钟为：122.88MHz，为C模以及U模中除译码外的逻辑的工作时钟，且各模的cpu接口也使用该时钟域
                                                                 在v8r1中的第三种接口时钟为：245.76MHz，为U模中译码的工作时钟 */
        unsigned long  reserved              : 29; /* bit[3-31]: 保留 */
    } reg;
} BBPCOMM_BBPMST_WR_CH2_CLK_SEL_UNION;
#define BBPCOMM_BBPMST_WR_CH2_CLK_SEL_bbpmst_wr_ch2_clk_sel_START  (0)
#define BBPCOMM_BBPMST_WR_CH2_CLK_SEL_bbpmst_wr_ch2_clk_sel_END    (2)


/*****************************************************************************
 结构名    : BBPCOMM_BBPMST_WR_CH2_BURST_BEAT_UNION
 结构说明  : BBPMST_WR_CH2_BURST_BEAT 寄存器结构定义。地址偏移量:0x0050，初值:0x00000008，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      bbpmst_wr_ch2_burst_beat_reg;
    struct
    {
        unsigned long  bbpmst_wr_ch2_burst_beat : 4;  /* bit[0-3] : 用Master方式完成DPA上报时，单个burst的类型，含义如下：burst_beat==0， burst1
                                                                    burst_beat==3， burst4
                                                                    burst_beat==7， burst8
                                                                    burst_beat==15，burst16
                                                                    如果决定使用burst8， 则此种配置下硬件在读写DDR时会固定向总线发出burst8请求，同时硬件支持数据块的尾部不是一个完整的burst8  */
        unsigned long  reserved                 : 28; /* bit[4-31]: 保留 */
    } reg;
} BBPCOMM_BBPMST_WR_CH2_BURST_BEAT_UNION;
#define BBPCOMM_BBPMST_WR_CH2_BURST_BEAT_bbpmst_wr_ch2_burst_beat_START  (0)
#define BBPCOMM_BBPMST_WR_CH2_BURST_BEAT_bbpmst_wr_ch2_burst_beat_END    (3)


/*****************************************************************************
 结构名    : BBPCOMM_BBPMST_WR_CH2_AXI_SEL_UNION
 结构说明  : BBPMST_WR_CH2_AXI_SEL 寄存器结构定义。地址偏移量:0x0054，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      bbpmst_wr_ch2_axi_sel_reg;
    struct
    {
        unsigned long  bbpmst_wr_ch2_axi_sel : 1;  /* bit[0]   : 本通道所选择使用的master口：
                                                                 1'b0：该通道使用master口1
                                                                 1'b1：该通道使用master口2 */
        unsigned long  reserved              : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_BBPMST_WR_CH2_AXI_SEL_UNION;
#define BBPCOMM_BBPMST_WR_CH2_AXI_SEL_bbpmst_wr_ch2_axi_sel_START  (0)
#define BBPCOMM_BBPMST_WR_CH2_AXI_SEL_bbpmst_wr_ch2_axi_sel_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_BBPMST_WR_CH3_MODE_UNION
 结构说明  : BBPMST_WR_CH3_MODE 寄存器结构定义。地址偏移量:0x0060，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      bbpmst_wr_ch3_mode_reg;
    struct
    {
        unsigned long  bbpmst_wr_ch3_mode : 3;  /* bit[0-2] : 该写通道的工作模式：每个通道可以由最多3个不同时发生的数据流复用；bbpmst_wr_ch0_mode代表写通道0的工作模式
                                                              1）3'b000 ：通道非使能，此时该通道向前级的BBP输出valid/ready为0，向后级的axi接口不再发出新的读/写请求，如果所有的读写通道都没有新请求了，且axi口以及总线被释放（是否被释放的状态在cpu接口有上报），软件结合该状态可启动门控或掉电流程
                                                              2）3'b001：通道配置给第一组接口使用
                                                              3）3'b010：通道配置给第二组接口使用
                                                              4）3'b100：通道配置给第三组接口使用 */
        unsigned long  reserved           : 29; /* bit[3-31]: 保留 */
    } reg;
} BBPCOMM_BBPMST_WR_CH3_MODE_UNION;
#define BBPCOMM_BBPMST_WR_CH3_MODE_bbpmst_wr_ch3_mode_START  (0)
#define BBPCOMM_BBPMST_WR_CH3_MODE_bbpmst_wr_ch3_mode_END    (2)


/*****************************************************************************
 结构名    : BBPCOMM_BBPMST_WR_CH3_CLK_SEL_UNION
 结构说明  : BBPMST_WR_CH3_CLK_SEL 寄存器结构定义。地址偏移量:0x0064，初值:0x00000001，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      bbpmst_wr_ch3_clk_sel_reg;
    struct
    {
        unsigned long  bbpmst_wr_ch3_clk_sel : 3;  /* bit[0-2] : 该写通道的输入时钟域选择：
                                                                 1）3’b001：该写通道的输入接口使用第一种时钟域
                                                                 2）3’b010：该写通道的输入接口使用第二种时钟域
                                                                 3）3’b100：该写通道的输入接口使用第三种时钟域
                                                                 在v8r1中的第一种接口时钟为：104MHz，为g模的工作时钟
                                                                 在v8r1中的第二种接口时钟为：122.88MHz，为C模以及U模中除译码外的逻辑的工作时钟，且各模的cpu接口也使用该时钟域
                                                                 在v8r1中的第三种接口时钟为：245.76MHz，为U模中译码的工作时钟 */
        unsigned long  reserved              : 29; /* bit[3-31]: 保留 */
    } reg;
} BBPCOMM_BBPMST_WR_CH3_CLK_SEL_UNION;
#define BBPCOMM_BBPMST_WR_CH3_CLK_SEL_bbpmst_wr_ch3_clk_sel_START  (0)
#define BBPCOMM_BBPMST_WR_CH3_CLK_SEL_bbpmst_wr_ch3_clk_sel_END    (2)


/*****************************************************************************
 结构名    : BBPCOMM_BBPMST_WR_CH3_BURST_BEAT_UNION
 结构说明  : BBPMST_WR_CH3_BURST_BEAT 寄存器结构定义。地址偏移量:0x0070，初值:0x00000008，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      bbpmst_wr_ch3_burst_beat_reg;
    struct
    {
        unsigned long  bbpmst_wr_ch3_burst_beat : 4;  /* bit[0-3] : 用Master方式完成DPA上报时，单个burst的类型，含义如下：
                                                                    burst_beat==0， burst1
                                                                    burst_beat==3， burst4
                                                                    burst_beat==7， burst8
                                                                    burst_beat==15，burst16
                                                                    如果决定使用burst8， 则此种配置下硬件在读写DDR时会固定向总线发出burst8请求，同时硬件支持数据块的尾部不是一个完整的burst8  */
        unsigned long  reserved                 : 28; /* bit[4-31]: 保留 */
    } reg;
} BBPCOMM_BBPMST_WR_CH3_BURST_BEAT_UNION;
#define BBPCOMM_BBPMST_WR_CH3_BURST_BEAT_bbpmst_wr_ch3_burst_beat_START  (0)
#define BBPCOMM_BBPMST_WR_CH3_BURST_BEAT_bbpmst_wr_ch3_burst_beat_END    (3)


/*****************************************************************************
 结构名    : BBPCOMM_BBPMST_WR_CH3_AXI_SEL_UNION
 结构说明  : BBPMST_WR_CH3_AXI_SEL 寄存器结构定义。地址偏移量:0x0074，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      bbpmst_wr_ch3_axi_sel_reg;
    struct
    {
        unsigned long  bbpmst_wr_ch3_axi_sel : 1;  /* bit[0]   : 本通道所选择使用的master口：
                                                                 1'b0：该通道使用master口1
                                                                 1'b1：该通道使用master口2 */
        unsigned long  reserved              : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_BBPMST_WR_CH3_AXI_SEL_UNION;
#define BBPCOMM_BBPMST_WR_CH3_AXI_SEL_bbpmst_wr_ch3_axi_sel_START  (0)
#define BBPCOMM_BBPMST_WR_CH3_AXI_SEL_bbpmst_wr_ch3_axi_sel_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_BBPMST_WR_CH4_MODE_UNION
 结构说明  : BBPMST_WR_CH4_MODE 寄存器结构定义。地址偏移量:0x0080，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      bbpmst_wr_ch4_mode_reg;
    struct
    {
        unsigned long  bbpmst_wr_ch4_mode : 3;  /* bit[0-2] : 该写通道的工作模式：每个通道可以由最多3个不同时发生的数据流复用；bbpmst_wr_ch0_mode代表写通道0的工作模式
                                                              1）3'b000 ：通道非使能，此时该通道向前级的BBP输出valid/ready为0，向后级的axi接口不再发出新的读/写请求，如果所有的读写通道都没有新请求了，且axi口以及总线被释放（是否被释放的状态在cpu接口有上报），软件结合该状态可启动门控或掉电流程
                                                              2）3'b001：通道配置给第一组接口使用
                                                              3）3'b010：通道配置给第二组接口使用
                                                              4）3'b100：通道配置给第三组接口使用 */
        unsigned long  reserved           : 29; /* bit[3-31]: 保留 */
    } reg;
} BBPCOMM_BBPMST_WR_CH4_MODE_UNION;
#define BBPCOMM_BBPMST_WR_CH4_MODE_bbpmst_wr_ch4_mode_START  (0)
#define BBPCOMM_BBPMST_WR_CH4_MODE_bbpmst_wr_ch4_mode_END    (2)


/*****************************************************************************
 结构名    : BBPCOMM_BBPMST_WR_CH4_CLK_SEL_UNION
 结构说明  : BBPMST_WR_CH4_CLK_SEL 寄存器结构定义。地址偏移量:0x0084，初值:0x00000001，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      bbpmst_wr_ch4_clk_sel_reg;
    struct
    {
        unsigned long  bbpmst_wr_ch4_clk_sel : 3;  /* bit[0-2] : 该写通道的输入时钟域选择：
                                                                 1）3’b001：该写通道的输入接口使用第一种时钟域
                                                                 2）3’b010：该写通道的输入接口使用第二种时钟域
                                                                 3）3’b100：该写通道的输入接口使用第三种时钟域
                                                                 在v8r1中的第一种接口时钟为：104MHz，为g模的工作时钟
                                                                 在v8r1中的第二种接口时钟为：122.88MHz，为C模以及U模中除译码外的逻辑的工作时钟，且各模的cpu接口也使用该时钟域
                                                                 在v8r1中的第三种接口时钟为：245.76MHz，为U模中译码的工作时钟 */
        unsigned long  reserved              : 29; /* bit[3-31]: 保留 */
    } reg;
} BBPCOMM_BBPMST_WR_CH4_CLK_SEL_UNION;
#define BBPCOMM_BBPMST_WR_CH4_CLK_SEL_bbpmst_wr_ch4_clk_sel_START  (0)
#define BBPCOMM_BBPMST_WR_CH4_CLK_SEL_bbpmst_wr_ch4_clk_sel_END    (2)


/*****************************************************************************
 结构名    : BBPCOMM_BBPMST_WR_CH4_BURST_BEAT_UNION
 结构说明  : BBPMST_WR_CH4_BURST_BEAT 寄存器结构定义。地址偏移量:0x0090，初值:0x00000008，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      bbpmst_wr_ch4_burst_beat_reg;
    struct
    {
        unsigned long  bbpmst_wr_ch4_burst_beat : 4;  /* bit[0-3] : 用Master方式完成DPA上报时，单个burst的类型，含义如下：
                                                                    burst_beat==0， burst1
                                                                    burst_beat==3， burst4
                                                                    burst_beat==7， burst8
                                                                    burst_beat==15，burst16
                                                                    如果决定使用burst8， 则此种配置下硬件在读写DDR时会固定向总线发出burst8请求，同时硬件支持数据块的尾部不是一个完整的burst8  */
        unsigned long  reserved                 : 28; /* bit[4-31]: 保留 */
    } reg;
} BBPCOMM_BBPMST_WR_CH4_BURST_BEAT_UNION;
#define BBPCOMM_BBPMST_WR_CH4_BURST_BEAT_bbpmst_wr_ch4_burst_beat_START  (0)
#define BBPCOMM_BBPMST_WR_CH4_BURST_BEAT_bbpmst_wr_ch4_burst_beat_END    (3)


/*****************************************************************************
 结构名    : BBPCOMM_BBPMST_WR_CH4_AXI_SEL_UNION
 结构说明  : BBPMST_WR_CH4_AXI_SEL 寄存器结构定义。地址偏移量:0x0094，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      bbpmst_wr_ch4_axi_sel_reg;
    struct
    {
        unsigned long  bbpmst_wr_ch4_axi_sel : 1;  /* bit[0]   : 本通道所选择使用的master口：
                                                                 1'b0：该通道使用master口1
                                                                 1'b1：该通道使用master口2 */
        unsigned long  reserved              : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_BBPMST_WR_CH4_AXI_SEL_UNION;
#define BBPCOMM_BBPMST_WR_CH4_AXI_SEL_bbpmst_wr_ch4_axi_sel_START  (0)
#define BBPCOMM_BBPMST_WR_CH4_AXI_SEL_bbpmst_wr_ch4_axi_sel_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_BBPMST_CH_BUSY_UNION
 结构说明  : BBPMST_CH_BUSY 寄存器结构定义。地址偏移量:0x0098，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      bbpmst_ch_busy_reg;
    struct
    {
        unsigned long  wr_ch0_busy : 1;  /* bit[0]    : 写通道0的空闲/忙碌状态：
                                                        1'b0：通道已排空，处于空闲态
                                                        1'b1：通道处于忙碌状态 */
        unsigned long  wr_ch1_busy : 1;  /* bit[1]    : 写通道1的空闲/忙碌状态：
                                                        1'b0：通道已排空，处于空闲态
                                                        1'b1：通道处于忙碌状态 */
        unsigned long  wr_ch2_busy : 1;  /* bit[2]    : 写通道2的空闲/忙碌状态：
                                                        1'b0：通道已排空，处于空闲态
                                                        1'b1：通道处于忙碌状态 */
        unsigned long  wr_ch3_busy : 1;  /* bit[3]    : 写通道3的空闲/忙碌状态：
                                                        1'b0：通道已排空，处于空闲态
                                                        1'b1：通道处于忙碌状态 */
        unsigned long  wr_ch4_busy : 1;  /* bit[4]    : 写通道4的空闲/忙碌状态：
                                                        1'b0：通道已排空，处于空闲态
                                                        1'b1：通道处于忙碌状态 */
        unsigned long  reserved_0  : 11; /* bit[5-15] : 保留 */
        unsigned long  rd_ch0_busy : 1;  /* bit[16]   : 读通道0的空闲/忙碌状态：
                                                        1'b0：通道已排空，处于空闲态
                                                        1'b1：通道处于忙碌状态 */
        unsigned long  rd_ch1_busy : 1;  /* bit[17]   : 读通道1的空闲/忙碌状态：
                                                        1'b0：通道已排空，处于空闲态
                                                        1'b1：通道处于忙碌状态 */
        unsigned long  rd_ch2_busy : 1;  /* bit[18]   : 读通道2的空闲/忙碌状态：
                                                        1'b0：通道已排空，处于空闲态
                                                        1'b1：通道处于忙碌状态 */
        unsigned long  rd_ch3_busy : 1;  /* bit[19]   : 读通道3的空闲/忙碌状态：
                                                        1'b0：通道已排空，处于空闲态
                                                        1'b1：通道处于忙碌状态 */
        unsigned long  reserved_1  : 12; /* bit[20-31]: 保留 */
    } reg;
} BBPCOMM_BBPMST_CH_BUSY_UNION;
#define BBPCOMM_BBPMST_CH_BUSY_wr_ch0_busy_START  (0)
#define BBPCOMM_BBPMST_CH_BUSY_wr_ch0_busy_END    (0)
#define BBPCOMM_BBPMST_CH_BUSY_wr_ch1_busy_START  (1)
#define BBPCOMM_BBPMST_CH_BUSY_wr_ch1_busy_END    (1)
#define BBPCOMM_BBPMST_CH_BUSY_wr_ch2_busy_START  (2)
#define BBPCOMM_BBPMST_CH_BUSY_wr_ch2_busy_END    (2)
#define BBPCOMM_BBPMST_CH_BUSY_wr_ch3_busy_START  (3)
#define BBPCOMM_BBPMST_CH_BUSY_wr_ch3_busy_END    (3)
#define BBPCOMM_BBPMST_CH_BUSY_wr_ch4_busy_START  (4)
#define BBPCOMM_BBPMST_CH_BUSY_wr_ch4_busy_END    (4)
#define BBPCOMM_BBPMST_CH_BUSY_rd_ch0_busy_START  (16)
#define BBPCOMM_BBPMST_CH_BUSY_rd_ch0_busy_END    (16)
#define BBPCOMM_BBPMST_CH_BUSY_rd_ch1_busy_START  (17)
#define BBPCOMM_BBPMST_CH_BUSY_rd_ch1_busy_END    (17)
#define BBPCOMM_BBPMST_CH_BUSY_rd_ch2_busy_START  (18)
#define BBPCOMM_BBPMST_CH_BUSY_rd_ch2_busy_END    (18)
#define BBPCOMM_BBPMST_CH_BUSY_rd_ch3_busy_START  (19)
#define BBPCOMM_BBPMST_CH_BUSY_rd_ch3_busy_END    (19)


/*****************************************************************************
 结构名    : BBPCOMM_BBPMST_RD_CH0_MODE_UNION
 结构说明  : BBPMST_RD_CH0_MODE 寄存器结构定义。地址偏移量:0x0100，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      bbpmst_rd_ch0_mode_reg;
    struct
    {
        unsigned long  bbpmst_rd_ch0_mode : 3;  /* bit[0-2] : 该读通道的工作模式：每个通道可以由最多3个不同时发生的数据流复用；bbpmst_rd_ch0_mode代表读通道0的工作模式
                                                              1）3'b000 ：通道非使能，此时该通道向前级的BBP输出valid/ready为0，向后级的读写控制不再发出新的读/写请求，如果所有的读写通道都没有新请求，则master口被释放，总线状态在cpu接口上报，软件结合该状态可启动门控或掉电流程
                                                              2）3'b001：通道配置给第一组接口使用
                                                              3）3'b010：通道配置给第二组接口使用
                                                              4）3'b100：通道配置给第三组接口使用 */
        unsigned long  reserved           : 29; /* bit[3-31]: 保留 */
    } reg;
} BBPCOMM_BBPMST_RD_CH0_MODE_UNION;
#define BBPCOMM_BBPMST_RD_CH0_MODE_bbpmst_rd_ch0_mode_START  (0)
#define BBPCOMM_BBPMST_RD_CH0_MODE_bbpmst_rd_ch0_mode_END    (2)


/*****************************************************************************
 结构名    : BBPCOMM_BBPMST_RD_CH0_CLK_SEL_UNION
 结构说明  : BBPMST_RD_CH0_CLK_SEL 寄存器结构定义。地址偏移量:0x0104，初值:0x00000001，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      bbpmst_rd_ch0_clk_sel_reg;
    struct
    {
        unsigned long  bbpmst_rd_ch0_clk_sel : 3;  /* bit[0-2] : 该读通道的输入时钟域选择：
                                                                 1）3’b001：该读通道的输入接口使用第一种时钟域
                                                                 2）3’b010：该读通道的输入接口使用第二种时钟域
                                                                 3）3’b100：该读通道的输入接口使用第三种时钟域
                                                                 在v8r1中的第一种接口时钟为：104MHz，为g模的工作时钟
                                                                 在v8r1中的第二种接口时钟为：122.88MHz，为C模以及U模中除译码外的逻辑的工作时钟，且各模的cpu接口也使用该时钟域
                                                                 在v8r1中的第三种接口时钟为：245.76MHz，为U模中译码的工作时钟 */
        unsigned long  reserved              : 29; /* bit[3-31]: 保留 */
    } reg;
} BBPCOMM_BBPMST_RD_CH0_CLK_SEL_UNION;
#define BBPCOMM_BBPMST_RD_CH0_CLK_SEL_bbpmst_rd_ch0_clk_sel_START  (0)
#define BBPCOMM_BBPMST_RD_CH0_CLK_SEL_bbpmst_rd_ch0_clk_sel_END    (2)


/*****************************************************************************
 结构名    : BBPCOMM_BBPMST_RD_CH0_BURST_BEAT_UNION
 结构说明  : BBPMST_RD_CH0_BURST_BEAT 寄存器结构定义。地址偏移量:0x010C，初值:0x00000008，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      bbpmst_rd_ch0_burst_beat_reg;
    struct
    {
        unsigned long  bbpmst_rd_ch0_burst_beat : 4;  /* bit[0-3] : 用Master方式完成DPA上报时，单个burst的类型，含义如下：
                                                                    burst_beat==0， burst1
                                                                    burst_beat==3， burst4
                                                                    burst_beat==7， burst8
                                                                    burst_beat==15，burst16
                                                                    如果决定使用burst8， 则此种配置下硬件在读写DDR时会固定向总线发出burst8请求，同时硬件支持数据块的尾部不是一个完整的burst8  */
        unsigned long  reserved                 : 28; /* bit[4-31]: 保留 */
    } reg;
} BBPCOMM_BBPMST_RD_CH0_BURST_BEAT_UNION;
#define BBPCOMM_BBPMST_RD_CH0_BURST_BEAT_bbpmst_rd_ch0_burst_beat_START  (0)
#define BBPCOMM_BBPMST_RD_CH0_BURST_BEAT_bbpmst_rd_ch0_burst_beat_END    (3)


/*****************************************************************************
 结构名    : BBPCOMM_BBPMST_RD_CH0_AXI_SEL_UNION
 结构说明  : BBPMST_RD_CH0_AXI_SEL 寄存器结构定义。地址偏移量:0x0110，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      bbpmst_rd_ch0_axi_sel_reg;
    struct
    {
        unsigned long  bbpmst_rd_ch0_axi_sel : 1;  /* bit[0]   : 本通道所选择使用的master口：
                                                                 1'b0：该通道使用master口1
                                                                 1'b1：该通道使用master口2 */
        unsigned long  reserved              : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_BBPMST_RD_CH0_AXI_SEL_UNION;
#define BBPCOMM_BBPMST_RD_CH0_AXI_SEL_bbpmst_rd_ch0_axi_sel_START  (0)
#define BBPCOMM_BBPMST_RD_CH0_AXI_SEL_bbpmst_rd_ch0_axi_sel_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_BBPMST_RD_CH1_MODE_UNION
 结构说明  : BBPMST_RD_CH1_MODE 寄存器结构定义。地址偏移量:0x0120，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      bbpmst_rd_ch1_mode_reg;
    struct
    {
        unsigned long  bbpmst_rd_ch1_mode : 3;  /* bit[0-2] : 该读通道的工作模式：每个通道可以由最多3个不同时发生的数据流复用；bbpmst_rd_ch0_mode代表读通道0的工作模式
                                                              1）3'b000 ：通道非使能，此时该通道向前级的BBP输出valid/ready为0，向后级的读写控制不再发出新的读/写请求，如果所有的读写通道都没有新请求，则master口被释放，总线状态在cpu接口上报，软件结合该状态可启动门控或掉电流程
                                                              2）3'b001：通道配置给第一组接口使用
                                                              3）3'b010：通道配置给第二组接口使用
                                                              4）3'b100：通道配置给第三组接口使用 */
        unsigned long  reserved           : 29; /* bit[3-31]: 保留 */
    } reg;
} BBPCOMM_BBPMST_RD_CH1_MODE_UNION;
#define BBPCOMM_BBPMST_RD_CH1_MODE_bbpmst_rd_ch1_mode_START  (0)
#define BBPCOMM_BBPMST_RD_CH1_MODE_bbpmst_rd_ch1_mode_END    (2)


/*****************************************************************************
 结构名    : BBPCOMM_BBPMST_RD_CH1_CLK_SEL_UNION
 结构说明  : BBPMST_RD_CH1_CLK_SEL 寄存器结构定义。地址偏移量:0x0124，初值:0x00000001，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      bbpmst_rd_ch1_clk_sel_reg;
    struct
    {
        unsigned long  bbpmst_rd_ch1_clk_sel : 3;  /* bit[0-2] : 该读通道的输入时钟域选择：
                                                                 1）3’b001：该读通道的输入接口使用第一种时钟域
                                                                 2）3’b010：该读通道的输入接口使用第二种时钟域
                                                                 3）3’b100：该读通道的输入接口使用第三种时钟域
                                                                 在v8r1中的第一种接口时钟为：104MHz，为g模的工作时钟
                                                                 在v8r1中的第二种接口时钟为：122.88MHz，为C模以及U模中除译码外的逻辑的工作时钟，且各模的cpu接口也使用该时钟域
                                                                 在v8r1中的第三种接口时钟为：245.76MHz，为U模中译码的工作时钟 */
        unsigned long  reserved              : 29; /* bit[3-31]: 保留 */
    } reg;
} BBPCOMM_BBPMST_RD_CH1_CLK_SEL_UNION;
#define BBPCOMM_BBPMST_RD_CH1_CLK_SEL_bbpmst_rd_ch1_clk_sel_START  (0)
#define BBPCOMM_BBPMST_RD_CH1_CLK_SEL_bbpmst_rd_ch1_clk_sel_END    (2)


/*****************************************************************************
 结构名    : BBPCOMM_BBPMST_RD_CH1_BURST_BEAT_UNION
 结构说明  : BBPMST_RD_CH1_BURST_BEAT 寄存器结构定义。地址偏移量:0x012C，初值:0x00000008，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      bbpmst_rd_ch1_burst_beat_reg;
    struct
    {
        unsigned long  bbpmst_rd_ch1_burst_beat : 4;  /* bit[0-3] : 用Master方式完成DPA上报时，单个burst的类型，含义如下：
                                                                    burst_beat==0， burst1
                                                                    burst_beat==3， burst4
                                                                    burst_beat==7， burst8
                                                                    burst_beat==15，burst16
                                                                    如果决定使用burst8， 则此种配置下硬件在读写DDR时会固定向总线发出burst8请求，同时硬件支持数据块的尾部不是一个完整的burst8  */
        unsigned long  reserved                 : 28; /* bit[4-31]: 保留 */
    } reg;
} BBPCOMM_BBPMST_RD_CH1_BURST_BEAT_UNION;
#define BBPCOMM_BBPMST_RD_CH1_BURST_BEAT_bbpmst_rd_ch1_burst_beat_START  (0)
#define BBPCOMM_BBPMST_RD_CH1_BURST_BEAT_bbpmst_rd_ch1_burst_beat_END    (3)


/*****************************************************************************
 结构名    : BBPCOMM_BBPMST_RD_CH1_AXI_SEL_UNION
 结构说明  : BBPMST_RD_CH1_AXI_SEL 寄存器结构定义。地址偏移量:0x0130，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      bbpmst_rd_ch1_axi_sel_reg;
    struct
    {
        unsigned long  bbpmst_rd_ch1_axi_sel : 1;  /* bit[0]   : 本通道所选择使用的master口：
                                                                 1'b0：该通道使用master口1
                                                                 1'b1：该通道使用master口2 */
        unsigned long  reserved              : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_BBPMST_RD_CH1_AXI_SEL_UNION;
#define BBPCOMM_BBPMST_RD_CH1_AXI_SEL_bbpmst_rd_ch1_axi_sel_START  (0)
#define BBPCOMM_BBPMST_RD_CH1_AXI_SEL_bbpmst_rd_ch1_axi_sel_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_BBPMST_RD_CH2_MODE_UNION
 结构说明  : BBPMST_RD_CH2_MODE 寄存器结构定义。地址偏移量:0x0140，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      bbpmst_rd_ch2_mode_reg;
    struct
    {
        unsigned long  bbpmst_rd_ch2_mode : 3;  /* bit[0-2] : 该读通道的工作模式：每个通道可以由最多3个不同时发生的数据流复用；bbpmst_rd_ch0_mode代表读通道0的工作模式
                                                              1）3'b000 ：通道非使能，此时该通道向前级的BBP输出valid/ready为0，向后级的读写控制不再发出新的读/写请求，如果所有的读写通道都没有新请求，则master口被释放，总线状态在cpu接口上报，软件结合该状态可启动门控或掉电流程
                                                              2）3'b001：通道配置给第一组接口使用
                                                              3）3'b010：通道配置给第二组接口使用
                                                              4）3'b100：通道配置给第三组接口使用 */
        unsigned long  reserved           : 29; /* bit[3-31]: 保留 */
    } reg;
} BBPCOMM_BBPMST_RD_CH2_MODE_UNION;
#define BBPCOMM_BBPMST_RD_CH2_MODE_bbpmst_rd_ch2_mode_START  (0)
#define BBPCOMM_BBPMST_RD_CH2_MODE_bbpmst_rd_ch2_mode_END    (2)


/*****************************************************************************
 结构名    : BBPCOMM_BBPMST_RD_CH2_CLK_SEL_UNION
 结构说明  : BBPMST_RD_CH2_CLK_SEL 寄存器结构定义。地址偏移量:0x0144，初值:0x00000001，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      bbpmst_rd_ch2_clk_sel_reg;
    struct
    {
        unsigned long  bbpmst_rd_ch2_clk_sel : 3;  /* bit[0-2] : 该读通道的输入时钟域选择：
                                                                 1）3’b001：该读通道的输入接口使用第一种时钟域
                                                                 2）3’b010：该读通道的输入接口使用第二种时钟域
                                                                 3）3’b100：该读通道的输入接口使用第三种时钟域
                                                                 在v8r1中的第一种接口时钟为：104MHz，为g模的工作时钟
                                                                 在v8r1中的第二种接口时钟为：122.88MHz，为C模以及U模中除译码外的逻辑的工作时钟，且各模的cpu接口也使用该时钟域
                                                                 在v8r1中的第三种接口时钟为：245.76MHz，为U模中译码的工作时钟 */
        unsigned long  reserved              : 29; /* bit[3-31]: 保留 */
    } reg;
} BBPCOMM_BBPMST_RD_CH2_CLK_SEL_UNION;
#define BBPCOMM_BBPMST_RD_CH2_CLK_SEL_bbpmst_rd_ch2_clk_sel_START  (0)
#define BBPCOMM_BBPMST_RD_CH2_CLK_SEL_bbpmst_rd_ch2_clk_sel_END    (2)


/*****************************************************************************
 结构名    : BBPCOMM_BBPMST_RD_CH2_BURST_BEAT_UNION
 结构说明  : BBPMST_RD_CH2_BURST_BEAT 寄存器结构定义。地址偏移量:0x014C，初值:0x00000008，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      bbpmst_rd_ch2_burst_beat_reg;
    struct
    {
        unsigned long  bbpmst_rd_ch2_burst_beat : 4;  /* bit[0-3] : 用Master方式完成DPA上报时，单个burst的类型，含义如下：
                                                                    burst_beat==0， burst1
                                                                    burst_beat==3， burst4
                                                                    burst_beat==7， burst8
                                                                    burst_beat==15，burst16
                                                                    如果决定使用burst8， 则此种配置下硬件在读写DDR时会固定向总线发出burst8请求，同时硬件支持数据块的尾部不是一个完整的burst8  */
        unsigned long  reserved                 : 28; /* bit[4-31]: 保留 */
    } reg;
} BBPCOMM_BBPMST_RD_CH2_BURST_BEAT_UNION;
#define BBPCOMM_BBPMST_RD_CH2_BURST_BEAT_bbpmst_rd_ch2_burst_beat_START  (0)
#define BBPCOMM_BBPMST_RD_CH2_BURST_BEAT_bbpmst_rd_ch2_burst_beat_END    (3)


/*****************************************************************************
 结构名    : BBPCOMM_BBPMST_RD_CH2_AXI_SEL_UNION
 结构说明  : BBPMST_RD_CH2_AXI_SEL 寄存器结构定义。地址偏移量:0x0150，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      bbpmst_rd_ch2_axi_sel_reg;
    struct
    {
        unsigned long  bbpmst_rd_ch2_axi_sel : 1;  /* bit[0]   : 本通道所选择使用的master口：
                                                                 1'b0：该通道使用master口1
                                                                 1'b1：该通道使用master口2 */
        unsigned long  reserved              : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_BBPMST_RD_CH2_AXI_SEL_UNION;
#define BBPCOMM_BBPMST_RD_CH2_AXI_SEL_bbpmst_rd_ch2_axi_sel_START  (0)
#define BBPCOMM_BBPMST_RD_CH2_AXI_SEL_bbpmst_rd_ch2_axi_sel_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_BBPMST_RD_CH3_MODE_UNION
 结构说明  : BBPMST_RD_CH3_MODE 寄存器结构定义。地址偏移量:0x0160，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      bbpmst_rd_ch3_mode_reg;
    struct
    {
        unsigned long  bbpmst_rd_ch3_mode : 3;  /* bit[0-2] : 该读通道的工作模式：每个通道可以由最多3个不同时发生的数据流复用；bbpmst_rd_ch0_mode代表读通道0的工作模式
                                                              1）3'b000 ：通道非使能，此时该通道向前级的BBP输出valid/ready为0，向后级的读写控制不再发出新的读/写请求，如果所有的读写通道都没有新请求，则master口被释放，总线状态在cpu接口上报，软件结合该状态可启动门控或掉电流程
                                                              2）3'b001：通道配置给第一组接口使用
                                                              3）3'b010：通道配置给第二组接口使用
                                                              4）3'b100：通道配置给第三组接口使用 */
        unsigned long  reserved           : 29; /* bit[3-31]: 保留 */
    } reg;
} BBPCOMM_BBPMST_RD_CH3_MODE_UNION;
#define BBPCOMM_BBPMST_RD_CH3_MODE_bbpmst_rd_ch3_mode_START  (0)
#define BBPCOMM_BBPMST_RD_CH3_MODE_bbpmst_rd_ch3_mode_END    (2)


/*****************************************************************************
 结构名    : BBPCOMM_BBPMST_RD_CH3_CLK_SEL_UNION
 结构说明  : BBPMST_RD_CH3_CLK_SEL 寄存器结构定义。地址偏移量:0x0164，初值:0x00000001，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      bbpmst_rd_ch3_clk_sel_reg;
    struct
    {
        unsigned long  bbpmst_rd_ch3_clk_sel : 3;  /* bit[0-2] : 该读通道的输入时钟域选择：
                                                                 1）3’b001：该读通道的输入接口使用第一种时钟域
                                                                 2）3’b010：该读通道的输入接口使用第二种时钟域
                                                                 3）3’b100：该读通道的输入接口使用第三种时钟域
                                                                 在v8r1中的第一种接口时钟为：104MHz，为g模的工作时钟
                                                                 在v8r1中的第二种接口时钟为：122.88MHz，为C模以及U模中除译码外的逻辑的工作时钟，且各模的cpu接口也使用该时钟域
                                                                 在v8r1中的第三种接口时钟为：245.76MHz，为U模中译码的工作时钟 */
        unsigned long  reserved              : 29; /* bit[3-31]: 保留 */
    } reg;
} BBPCOMM_BBPMST_RD_CH3_CLK_SEL_UNION;
#define BBPCOMM_BBPMST_RD_CH3_CLK_SEL_bbpmst_rd_ch3_clk_sel_START  (0)
#define BBPCOMM_BBPMST_RD_CH3_CLK_SEL_bbpmst_rd_ch3_clk_sel_END    (2)


/*****************************************************************************
 结构名    : BBPCOMM_BBPMST_RD_CH3_BURST_BEAT_UNION
 结构说明  : BBPMST_RD_CH3_BURST_BEAT 寄存器结构定义。地址偏移量:0x016C，初值:0x00000008，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      bbpmst_rd_ch3_burst_beat_reg;
    struct
    {
        unsigned long  bbpmst_rd_ch3_burst_beat : 4;  /* bit[0-3] : 用Master方式完成DPA上报时，单个burst的类型，含义如下：
                                                                    burst_beat==0， burst1
                                                                    burst_beat==3， burst4
                                                                    burst_beat==7， burst8
                                                                    burst_beat==15，burst16
                                                                    如果决定使用burst8， 则此种配置下硬件在读写DDR时会固定向总线发出burst8请求，同时硬件支持数据块的尾部不是一个完整的burst8  */
        unsigned long  reserved                 : 28; /* bit[4-31]: 保留 */
    } reg;
} BBPCOMM_BBPMST_RD_CH3_BURST_BEAT_UNION;
#define BBPCOMM_BBPMST_RD_CH3_BURST_BEAT_bbpmst_rd_ch3_burst_beat_START  (0)
#define BBPCOMM_BBPMST_RD_CH3_BURST_BEAT_bbpmst_rd_ch3_burst_beat_END    (3)


/*****************************************************************************
 结构名    : BBPCOMM_BBPMST_RD_CH3_AXI_SEL_UNION
 结构说明  : BBPMST_RD_CH3_AXI_SEL 寄存器结构定义。地址偏移量:0x0170，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      bbpmst_rd_ch3_axi_sel_reg;
    struct
    {
        unsigned long  bbpmst_rd_ch3_axi_sel : 1;  /* bit[0]   : 本通道所选择使用的master口：
                                                                 1'b0：该通道使用master口1
                                                                 1'b1：该通道使用master口2 */
        unsigned long  reserved              : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_BBPMST_RD_CH3_AXI_SEL_UNION;
#define BBPCOMM_BBPMST_RD_CH3_AXI_SEL_bbpmst_rd_ch3_axi_sel_START  (0)
#define BBPCOMM_BBPMST_RD_CH3_AXI_SEL_bbpmst_rd_ch3_axi_sel_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_BBPMST_RD_CH0_BIT_WIDTH_IND_UNION
 结构说明  : BBPMST_RD_CH0_BIT_WIDTH_IND 寄存器结构定义。地址偏移量:0x0174，初值:0x00000002，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      bbpmst_rd_ch0_bit_width_ind_reg;
    struct
    {
        unsigned long  bbpmst_rd_ch0_bit_width_ind : 2;  /* bit[0-1] : 读通道0每个64比特读数据有效数据个数指示：0，表示每个64比特读数据含10个有效数据；1，表示每个64比特读数据含16个有效数据；2，表示每个64比特读数据含1个有效数据； */
        unsigned long  reserved                    : 30; /* bit[2-31]: 保留 */
    } reg;
} BBPCOMM_BBPMST_RD_CH0_BIT_WIDTH_IND_UNION;
#define BBPCOMM_BBPMST_RD_CH0_BIT_WIDTH_IND_bbpmst_rd_ch0_bit_width_ind_START  (0)
#define BBPCOMM_BBPMST_RD_CH0_BIT_WIDTH_IND_bbpmst_rd_ch0_bit_width_ind_END    (1)


/*****************************************************************************
 结构名    : BBPCOMM_BBPMST_RD_CH1_BIT_WIDTH_IND_UNION
 结构说明  : BBPMST_RD_CH1_BIT_WIDTH_IND 寄存器结构定义。地址偏移量:0x0178，初值:0x00000002，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      bbpmst_rd_ch1_bit_width_ind_reg;
    struct
    {
        unsigned long  bbpmst_rd_ch1_bit_width_ind : 2;  /* bit[0-1] : 读通道1每个64比特读数据有效数据个数指示：0，表示每个64比特读数据含10个有效数据；1，表示每个64比特读数据含16个有效数据；2，表示每个64比特读数据含1个有效数据； */
        unsigned long  reserved                    : 30; /* bit[2-31]: 保留 */
    } reg;
} BBPCOMM_BBPMST_RD_CH1_BIT_WIDTH_IND_UNION;
#define BBPCOMM_BBPMST_RD_CH1_BIT_WIDTH_IND_bbpmst_rd_ch1_bit_width_ind_START  (0)
#define BBPCOMM_BBPMST_RD_CH1_BIT_WIDTH_IND_bbpmst_rd_ch1_bit_width_ind_END    (1)


/*****************************************************************************
 结构名    : BBPCOMM_BBPMST_RD_CH2_BIT_WIDTH_IND_UNION
 结构说明  : BBPMST_RD_CH2_BIT_WIDTH_IND 寄存器结构定义。地址偏移量:0x017C，初值:0x00000002，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      bbpmst_rd_ch2_bit_width_ind_reg;
    struct
    {
        unsigned long  bbpmst_rd_ch2_bit_width_ind : 2;  /* bit[0-1] : 读通道2每个64比特读数据有效数据个数指示：0，表示每个64比特读数据含10个有效数据；1，表示每个64比特读数据含16个有效数据；2，表示每个64比特读数据含16个有效数据； */
        unsigned long  reserved                    : 30; /* bit[2-31]: 保留 */
    } reg;
} BBPCOMM_BBPMST_RD_CH2_BIT_WIDTH_IND_UNION;
#define BBPCOMM_BBPMST_RD_CH2_BIT_WIDTH_IND_bbpmst_rd_ch2_bit_width_ind_START  (0)
#define BBPCOMM_BBPMST_RD_CH2_BIT_WIDTH_IND_bbpmst_rd_ch2_bit_width_ind_END    (1)


/*****************************************************************************
 结构名    : BBPCOMM_BBPMST_RD_CH3_BIT_WIDTH_IND_UNION
 结构说明  : BBPMST_RD_CH3_BIT_WIDTH_IND 寄存器结构定义。地址偏移量:0x0180，初值:0x00000002，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      bbpmst_rd_ch3_bit_width_ind_reg;
    struct
    {
        unsigned long  bbpmst_rd_ch3_bit_width_ind : 2;  /* bit[0-1] : 读通道3每个64比特读数据有效数据个数指示：0，表示每个64比特读数据含10个有效数据；1，表示每个64比特读数据含16个有效数据；2，表示每个64比特读数据含1个有效数据； */
        unsigned long  reserved                    : 30; /* bit[2-31]: 保留 */
    } reg;
} BBPCOMM_BBPMST_RD_CH3_BIT_WIDTH_IND_UNION;
#define BBPCOMM_BBPMST_RD_CH3_BIT_WIDTH_IND_bbpmst_rd_ch3_bit_width_ind_START  (0)
#define BBPCOMM_BBPMST_RD_CH3_BIT_WIDTH_IND_bbpmst_rd_ch3_bit_width_ind_END    (1)


/*****************************************************************************
 结构名    : BBPCOMM_BBPMST_AXI0_STATUS_UNION
 结构说明  : BBPMST_AXI0_STATUS 寄存器结构定义。地址偏移量:0x0300，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      bbpmst_axi0_status_reg;
    struct
    {
        unsigned long  bbpmst_axi0_status    : 1;  /* bit[0]   : axi口0的空闲状态标志： 软件可根据各个axi口的空闲状态决定是否能发起掉电或其他低功耗流程
                                                                 1’b0：空闲，总线已被释放掉
                                                                 1’b1：非空闲，总线未释放，此状态下master不可以复位/关时钟/掉电 */
        unsigned long  bbpmst_axi0_rd_status : 1;  /* bit[1]   : axi口0读通道的空闲状态标志： 软件可根据各个axi口的空闲状态决定是否能发起掉电或其他低功耗流程
                                                                 1’b0：空闲，总线已被释放掉
                                                                 1’b1：非空闲，总线未释放，此状态下master不可以复位/关时钟/掉电 */
        unsigned long  bbpmst_axi0_wr_status : 1;  /* bit[2]   : axi口0写通道的空闲状态标志： 软件可根据各个axi口的空闲状态决定是否能发起掉电或其他低功耗流程
                                                                 1’b0：空闲，总线已被释放掉
                                                                 1’b1：非空闲，总线未释放，此状态下master不可以复位/关时钟/掉电 */
        unsigned long  reserved              : 29; /* bit[3-31]: 保留 */
    } reg;
} BBPCOMM_BBPMST_AXI0_STATUS_UNION;
#define BBPCOMM_BBPMST_AXI0_STATUS_bbpmst_axi0_status_START     (0)
#define BBPCOMM_BBPMST_AXI0_STATUS_bbpmst_axi0_status_END       (0)
#define BBPCOMM_BBPMST_AXI0_STATUS_bbpmst_axi0_rd_status_START  (1)
#define BBPCOMM_BBPMST_AXI0_STATUS_bbpmst_axi0_rd_status_END    (1)
#define BBPCOMM_BBPMST_AXI0_STATUS_bbpmst_axi0_wr_status_START  (2)
#define BBPCOMM_BBPMST_AXI0_STATUS_bbpmst_axi0_wr_status_END    (2)


/*****************************************************************************
 结构名    : BBPCOMM_BBPMST_AXI1_STATUS_UNION
 结构说明  : BBPMST_AXI1_STATUS 寄存器结构定义。地址偏移量:0x0304，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      bbpmst_axi1_status_reg;
    struct
    {
        unsigned long  bbpmst_axi1_status    : 1;  /* bit[0]   : axi口1的空闲状态标志： 软件可根据各个axi口的空闲状态决定是否能发起掉电或其他低功耗流程
                                                                 1’b0：空闲，总线已被释放掉
                                                                 1’b1：非空闲，总线未释放，此状态下master不可以复位/关时钟/掉电 */
        unsigned long  bbpmst_axi1_rd_status : 1;  /* bit[1]   : axi口1的读通道状态标志： 软件可根据各个axi口的空闲状态决定是否能发起掉电或其他低功耗流程
                                                                 1’b0：空闲，总线已被释放掉
                                                                 1’b1：非空闲，总线未释放，此状态下master不可以复位/关时钟/掉电 */
        unsigned long  bbpmst_axi1_wr_status : 1;  /* bit[2]   : axi口1的写通道状态标志： 软件可根据各个axi口的空闲状态决定是否能发起掉电或其他低功耗流程
                                                                 1’b0：空闲，总线已被释放掉
                                                                 1’b1：非空闲，总线未释放，此状态下master不可以复位/关时钟/掉电 */
        unsigned long  reserved              : 29; /* bit[3-31]: 保留 */
    } reg;
} BBPCOMM_BBPMST_AXI1_STATUS_UNION;
#define BBPCOMM_BBPMST_AXI1_STATUS_bbpmst_axi1_status_START     (0)
#define BBPCOMM_BBPMST_AXI1_STATUS_bbpmst_axi1_status_END       (0)
#define BBPCOMM_BBPMST_AXI1_STATUS_bbpmst_axi1_rd_status_START  (1)
#define BBPCOMM_BBPMST_AXI1_STATUS_bbpmst_axi1_rd_status_END    (1)
#define BBPCOMM_BBPMST_AXI1_STATUS_bbpmst_axi1_wr_status_START  (2)
#define BBPCOMM_BBPMST_AXI1_STATUS_bbpmst_axi1_wr_status_END    (2)


/*****************************************************************************
 结构名    : BBPCOMM_BBPMST_ERROR_BURST_BEAT0_UNION
 结构说明  : BBPMST_ERROR_BURST_BEAT0 寄存器结构定义。地址偏移量:0x0308，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      bbpmst_error_burst_beat0_reg;
    struct
    {
        unsigned long  bbpmst_error_burst_beat0 : 4;  /* bit[0-3] : 当bbpmst写error发生时，临近的burst的类型0 */
        unsigned long  reserved                 : 28; /* bit[4-31]: 保留 */
    } reg;
} BBPCOMM_BBPMST_ERROR_BURST_BEAT0_UNION;
#define BBPCOMM_BBPMST_ERROR_BURST_BEAT0_bbpmst_error_burst_beat0_START  (0)
#define BBPCOMM_BBPMST_ERROR_BURST_BEAT0_bbpmst_error_burst_beat0_END    (3)


/*****************************************************************************
 结构名    : BBPCOMM_BBPMST_ERROR_BURST_BEAT1_UNION
 结构说明  : BBPMST_ERROR_BURST_BEAT1 寄存器结构定义。地址偏移量:0x030C，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      bbpmst_error_burst_beat1_reg;
    struct
    {
        unsigned long  bbpmst_error_burst_beat1 : 4;  /* bit[0-3] : 当bbpmst写error发生时，临近的burst的类型1 */
        unsigned long  reserved                 : 28; /* bit[4-31]: 保留 */
    } reg;
} BBPCOMM_BBPMST_ERROR_BURST_BEAT1_UNION;
#define BBPCOMM_BBPMST_ERROR_BURST_BEAT1_bbpmst_error_burst_beat1_START  (0)
#define BBPCOMM_BBPMST_ERROR_BURST_BEAT1_bbpmst_error_burst_beat1_END    (3)


/*****************************************************************************
 结构名    : BBPCOMM_BBPMST_ERROR_BURST_BEAT2_UNION
 结构说明  : BBPMST_ERROR_BURST_BEAT2 寄存器结构定义。地址偏移量:0x0310，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      bbpmst_error_burst_beat2_reg;
    struct
    {
        unsigned long  bbpmst_error_burst_beat2 : 4;  /* bit[0-3] : 当bbpmst写error发生时，临近的burst的类型2 */
        unsigned long  reserved                 : 28; /* bit[4-31]: 保留 */
    } reg;
} BBPCOMM_BBPMST_ERROR_BURST_BEAT2_UNION;
#define BBPCOMM_BBPMST_ERROR_BURST_BEAT2_bbpmst_error_burst_beat2_START  (0)
#define BBPCOMM_BBPMST_ERROR_BURST_BEAT2_bbpmst_error_burst_beat2_END    (3)


/*****************************************************************************
 结构名    : BBPCOMM_BBPMST_ERROR_BURST_BEAT3_UNION
 结构说明  : BBPMST_ERROR_BURST_BEAT3 寄存器结构定义。地址偏移量:0x0314，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      bbpmst_error_burst_beat3_reg;
    struct
    {
        unsigned long  bbpmst_error_burst_beat3 : 4;  /* bit[0-3] : 当bbpmst写error发生时，临近的burst的类型3 */
        unsigned long  reserved                 : 28; /* bit[4-31]: 保留 */
    } reg;
} BBPCOMM_BBPMST_ERROR_BURST_BEAT3_UNION;
#define BBPCOMM_BBPMST_ERROR_BURST_BEAT3_bbpmst_error_burst_beat3_START  (0)
#define BBPCOMM_BBPMST_ERROR_BURST_BEAT3_bbpmst_error_burst_beat3_END    (3)


/*****************************************************************************
 结构名    : BBPCOMM_BBPMST_ERROR_INT_CLR_UNION
 结构说明  : BBPMST_ERROR_INT_CLR 寄存器结构定义。地址偏移量:0x0328，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      bbpmst_error_int_clr_reg;
    struct
    {
        unsigned long  bbpmst_error_int_clr : 1;  /* bit[0]   : bbpmst读写错误中断清除寄存器。当BBPMST读写发生错误时，bbpmst_error_int会直接拉成电平送给BBE16\CCPU，软件收到中断后配置该寄存器清除中断电平。 */
        unsigned long  reserved             : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_BBPMST_ERROR_INT_CLR_UNION;
#define BBPCOMM_BBPMST_ERROR_INT_CLR_bbpmst_error_int_clr_START  (0)
#define BBPCOMM_BBPMST_ERROR_INT_CLR_bbpmst_error_int_clr_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_MEM_CTRL0_BBPMST_UNION
 结构说明  : MEM_CTRL0_BBPMST 寄存器结构定义。地址偏移量:0x032C，初值:0x0C9B0C9B，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      mem_ctrl0_bbpmst_reg;
    struct
    {
        unsigned long  mem_ctrl_s_bbpmst      : 16; /* bit[0-15] : 单端口ram延时控制。 */
        unsigned long  mem_ctrl_d_1w2r_bbpmst : 16; /* bit[16-31]: 1w2r双端口ram延时控制。 */
    } reg;
} BBPCOMM_MEM_CTRL0_BBPMST_UNION;
#define BBPCOMM_MEM_CTRL0_BBPMST_mem_ctrl_s_bbpmst_START       (0)
#define BBPCOMM_MEM_CTRL0_BBPMST_mem_ctrl_s_bbpmst_END         (15)
#define BBPCOMM_MEM_CTRL0_BBPMST_mem_ctrl_d_1w2r_bbpmst_START  (16)
#define BBPCOMM_MEM_CTRL0_BBPMST_mem_ctrl_d_1w2r_bbpmst_END    (31)


/*****************************************************************************
 结构名    : BBPCOMM_MEM_CTRL1_BBPMST_UNION
 结构说明  : MEM_CTRL1_BBPMST 寄存器结构定义。地址偏移量:0x0330，初值:0x00230C9B，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      mem_ctrl1_bbpmst_reg;
    struct
    {
        unsigned long  mem_ctrl_d_1rw2rw_bbpmst : 16; /* bit[0-15] : 1rw2rw双端口ram延时控制。 */
        unsigned long  rom_ctrl_bbpmst          : 8;  /* bit[16-23]: rom延时控制。 */
        unsigned long  reserved                 : 8;  /* bit[24-31]: 保留 */
    } reg;
} BBPCOMM_MEM_CTRL1_BBPMST_UNION;
#define BBPCOMM_MEM_CTRL1_BBPMST_mem_ctrl_d_1rw2rw_bbpmst_START  (0)
#define BBPCOMM_MEM_CTRL1_BBPMST_mem_ctrl_d_1rw2rw_bbpmst_END    (15)
#define BBPCOMM_MEM_CTRL1_BBPMST_rom_ctrl_bbpmst_START           (16)
#define BBPCOMM_MEM_CTRL1_BBPMST_rom_ctrl_bbpmst_END             (23)


/*****************************************************************************
 结构名    : BBPCOMM_AXI_WR0_STATE_UNION
 结构说明  : AXI_WR0_STATE 寄存器结构定义。地址偏移量:0x0344，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      axi_wr0_state_reg;
    struct
    {
        unsigned long  axi_wr0_ready  : 1;  /* bit[0]    : AXI口0当前写响应可接收数据指示 */
        unsigned long  reserved_0     : 3;  /* bit[1-3]  : 保留 */
        unsigned long  axi_wr0_valid  : 1;  /* bit[4]    : AXI口0当前写响应数据有效指示 */
        unsigned long  reserved_1     : 3;  /* bit[5-7]  : 保留 */
        unsigned long  axi_wr0_cur_id : 3;  /* bit[8-10] : AXI口0当前写响应ID */
        unsigned long  reserved_2     : 21; /* bit[11-31]: 保留 */
    } reg;
} BBPCOMM_AXI_WR0_STATE_UNION;
#define BBPCOMM_AXI_WR0_STATE_axi_wr0_ready_START   (0)
#define BBPCOMM_AXI_WR0_STATE_axi_wr0_ready_END     (0)
#define BBPCOMM_AXI_WR0_STATE_axi_wr0_valid_START   (4)
#define BBPCOMM_AXI_WR0_STATE_axi_wr0_valid_END     (4)
#define BBPCOMM_AXI_WR0_STATE_axi_wr0_cur_id_START  (8)
#define BBPCOMM_AXI_WR0_STATE_axi_wr0_cur_id_END    (10)


/*****************************************************************************
 结构名    : BBPCOMM_AXI_RD0_STATE_UNION
 结构说明  : AXI_RD0_STATE 寄存器结构定义。地址偏移量:0x0348，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      axi_rd0_state_reg;
    struct
    {
        unsigned long  axi_rd0_ready  : 1;  /* bit[0]    : AXI口0当前读响应可接收数据指示 */
        unsigned long  reserved_0     : 3;  /* bit[1-3]  : 保留 */
        unsigned long  axi_rd0_valid  : 1;  /* bit[4]    : AXI口0当前读响应数据有效指示 */
        unsigned long  reserved_1     : 3;  /* bit[5-7]  : 保留 */
        unsigned long  axi_rd0_cur_id : 3;  /* bit[8-10] : AXI口0当前读响应ID */
        unsigned long  reserved_2     : 21; /* bit[11-31]: 保留 */
    } reg;
} BBPCOMM_AXI_RD0_STATE_UNION;
#define BBPCOMM_AXI_RD0_STATE_axi_rd0_ready_START   (0)
#define BBPCOMM_AXI_RD0_STATE_axi_rd0_ready_END     (0)
#define BBPCOMM_AXI_RD0_STATE_axi_rd0_valid_START   (4)
#define BBPCOMM_AXI_RD0_STATE_axi_rd0_valid_END     (4)
#define BBPCOMM_AXI_RD0_STATE_axi_rd0_cur_id_START  (8)
#define BBPCOMM_AXI_RD0_STATE_axi_rd0_cur_id_END    (10)


/*****************************************************************************
 结构名    : BBPCOMM_AXI_WR1_STATE_UNION
 结构说明  : AXI_WR1_STATE 寄存器结构定义。地址偏移量:0x034C，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      axi_wr1_state_reg;
    struct
    {
        unsigned long  axi_wr1_ready  : 1;  /* bit[0]    : AXI口1当前写响应可接收数据指示 */
        unsigned long  reserved_0     : 3;  /* bit[1-3]  : 保留 */
        unsigned long  axi_wr1_valid  : 1;  /* bit[4]    : AXI口1当前写响应数据有效指示 */
        unsigned long  reserved_1     : 3;  /* bit[5-7]  : 保留 */
        unsigned long  axi_wr1_cur_id : 3;  /* bit[8-10] : AXI口1当前写响应ID */
        unsigned long  reserved_2     : 21; /* bit[11-31]: 保留 */
    } reg;
} BBPCOMM_AXI_WR1_STATE_UNION;
#define BBPCOMM_AXI_WR1_STATE_axi_wr1_ready_START   (0)
#define BBPCOMM_AXI_WR1_STATE_axi_wr1_ready_END     (0)
#define BBPCOMM_AXI_WR1_STATE_axi_wr1_valid_START   (4)
#define BBPCOMM_AXI_WR1_STATE_axi_wr1_valid_END     (4)
#define BBPCOMM_AXI_WR1_STATE_axi_wr1_cur_id_START  (8)
#define BBPCOMM_AXI_WR1_STATE_axi_wr1_cur_id_END    (10)


/*****************************************************************************
 结构名    : BBPCOMM_AXI_RD1_STATE_UNION
 结构说明  : AXI_RD1_STATE 寄存器结构定义。地址偏移量:0x0350，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      axi_rd1_state_reg;
    struct
    {
        unsigned long  axi_rd1_ready  : 1;  /* bit[0]    : AXI口1当前读响应可接收数据指示 */
        unsigned long  reserved_0     : 3;  /* bit[1-3]  : 保留 */
        unsigned long  axi_rd1_valid  : 1;  /* bit[4]    : AXI口1当前读响应数据有效指示 */
        unsigned long  reserved_1     : 3;  /* bit[5-7]  : 保留 */
        unsigned long  axi_rd1_cur_id : 3;  /* bit[8-10] : AXI口1当前读响应ID */
        unsigned long  reserved_2     : 21; /* bit[11-31]: 保留 */
    } reg;
} BBPCOMM_AXI_RD1_STATE_UNION;
#define BBPCOMM_AXI_RD1_STATE_axi_rd1_ready_START   (0)
#define BBPCOMM_AXI_RD1_STATE_axi_rd1_ready_END     (0)
#define BBPCOMM_AXI_RD1_STATE_axi_rd1_valid_START   (4)
#define BBPCOMM_AXI_RD1_STATE_axi_rd1_valid_END     (4)
#define BBPCOMM_AXI_RD1_STATE_axi_rd1_cur_id_START  (8)
#define BBPCOMM_AXI_RD1_STATE_axi_rd1_cur_id_END    (10)


/*****************************************************************************
 结构名    : BBPCOMM_BBPMST_ERROR_INT_EN_UNION
 结构说明  : BBPMST_ERROR_INT_EN 寄存器结构定义。地址偏移量:0x0364，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      bbpmst_error_int_en_reg;
    struct
    {
        unsigned long  bbpmst_error_int_en : 1;  /* bit[0]   : BBPMST读写错误中断使能 */
        unsigned long  reserved            : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_BBPMST_ERROR_INT_EN_UNION;
#define BBPCOMM_BBPMST_ERROR_INT_EN_bbpmst_error_int_en_START  (0)
#define BBPCOMM_BBPMST_ERROR_INT_EN_bbpmst_error_int_en_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_BBPMST_ERROR_INT_MASK_UNION
 结构说明  : BBPMST_ERROR_INT_MASK 寄存器结构定义。地址偏移量:0x0368，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      bbpmst_error_int_mask_reg;
    struct
    {
        unsigned long  bbpmst_error_int_mask : 1;  /* bit[0]   : BBPMST读写错误中断屏蔽信号 */
        unsigned long  reserved              : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_BBPMST_ERROR_INT_MASK_UNION;
#define BBPCOMM_BBPMST_ERROR_INT_MASK_bbpmst_error_int_mask_START  (0)
#define BBPCOMM_BBPMST_ERROR_INT_MASK_bbpmst_error_int_mask_END    (0)


/***======================================================================***
                     (3/5) register_define_irm_cs
 ***======================================================================***/
/*****************************************************************************
 结构名    : BBPCOMM_IRM_CS_RAM_CTRL_UNION
 结构说明  : IRM_CS_RAM_CTRL 寄存器结构定义。地址偏移量:0x000，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      irm_cs_ram_ctrl_reg;
    struct
    {
        unsigned long  irm_cs_ram_ctrl : 3;  /* bit[0-2] : 用于irm_cs模式选择，影响irm_cs内部时钟切换和模式切换，默认都为0。
                                                           3'b001，UMTS模式（包括均衡，译码R99相关RAM）；
                                                           3'b010，LTE模式（上行HARQ部分）；
                                                           3'b100，CDMA模式；
                                                           3'b000，无（default）。 */
        unsigned long  reserved        : 29; /* bit[3-31]: 保留 */
    } reg;
} BBPCOMM_IRM_CS_RAM_CTRL_UNION;
#define BBPCOMM_IRM_CS_RAM_CTRL_irm_cs_ram_ctrl_START  (0)
#define BBPCOMM_IRM_CS_RAM_CTRL_irm_cs_ram_ctrl_END    (2)


/*****************************************************************************
 结构名    : BBPCOMM_IRM_CS_CLK_BYPASS_UNION
 结构说明  : IRM_CS_CLK_BYPASS 寄存器结构定义。地址偏移量:0x004，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      irm_cs_clk_bypass_reg;
    struct
    {
        unsigned long  irm_cs_clk_bypass : 1;  /* bit[0]   : irm_cs模块时钟门控旁路开关。默认为0，即开启时钟门控。配置1则开启旁路功能，时钟门控失效。 */
        unsigned long  reserved          : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_IRM_CS_CLK_BYPASS_UNION;
#define BBPCOMM_IRM_CS_CLK_BYPASS_irm_cs_clk_bypass_START  (0)
#define BBPCOMM_IRM_CS_CLK_BYPASS_irm_cs_clk_bypass_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_MEM_CTRL0_CS_UNION
 结构说明  : MEM_CTRL0_CS 寄存器结构定义。地址偏移量:0x00C，初值:0x0C9B0C9B，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      mem_ctrl0_cs_reg;
    struct
    {
        unsigned long  mem_ctrl_s_cs      : 16; /* bit[0-15] : 单端口ram延时控制。 */
        unsigned long  mem_ctrl_d_1w2r_cs : 16; /* bit[16-31]: 1w2r双端口ram延时控制。 */
    } reg;
} BBPCOMM_MEM_CTRL0_CS_UNION;
#define BBPCOMM_MEM_CTRL0_CS_mem_ctrl_s_cs_START       (0)
#define BBPCOMM_MEM_CTRL0_CS_mem_ctrl_s_cs_END         (15)
#define BBPCOMM_MEM_CTRL0_CS_mem_ctrl_d_1w2r_cs_START  (16)
#define BBPCOMM_MEM_CTRL0_CS_mem_ctrl_d_1w2r_cs_END    (31)


/*****************************************************************************
 结构名    : BBPCOMM_MEM_CTRL1_CS_UNION
 结构说明  : MEM_CTRL1_CS 寄存器结构定义。地址偏移量:0x010，初值:0x00230C9B，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      mem_ctrl1_cs_reg;
    struct
    {
        unsigned long  mem_ctrl_d_1rw2rw_cs : 16; /* bit[0-15] : 1rw2rw双端口ram延时控制。 */
        unsigned long  rom_ctrl_cs          : 8;  /* bit[16-23]: rom延时控制。 */
        unsigned long  reserved             : 8;  /* bit[24-31]: 保留 */
    } reg;
} BBPCOMM_MEM_CTRL1_CS_UNION;
#define BBPCOMM_MEM_CTRL1_CS_mem_ctrl_d_1rw2rw_cs_START  (0)
#define BBPCOMM_MEM_CTRL1_CS_mem_ctrl_d_1rw2rw_cs_END    (15)
#define BBPCOMM_MEM_CTRL1_CS_rom_ctrl_cs_START           (16)
#define BBPCOMM_MEM_CTRL1_CS_rom_ctrl_cs_END             (23)


/***======================================================================***
                     (4/5) register_define_irm_ps
 ***======================================================================***/
/*****************************************************************************
 结构名    : BBPCOMM_IRM_PS_RAM_CTRL_UNION
 结构说明  : IRM_PS_RAM_CTRL 寄存器结构定义。地址偏移量:0x000，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      irm_ps_ram_ctrl_reg;
    struct
    {
        unsigned long  irm_ps_ram_ctrl : 6;  /* bit[0-5] : 用于irm_ps网络模式选择，影响irm_ps内部时钟切换和模式切换，默认都为0。.
                                                           6'b000001，GBBP1模式；
                                                           6'b000110，UMTS模式；
                                                           6'b001000，LTE模式；
                                                           6'b010000，TDS-CDMA模式；
                                                           6'b100000，CDMA模式。
                                                           注：GBBP1固定为主卡（G0/U/L/T）。 */
        unsigned long  reserved        : 26; /* bit[6-31]: 保留 */
    } reg;
} BBPCOMM_IRM_PS_RAM_CTRL_UNION;
#define BBPCOMM_IRM_PS_RAM_CTRL_irm_ps_ram_ctrl_START  (0)
#define BBPCOMM_IRM_PS_RAM_CTRL_irm_ps_ram_ctrl_END    (5)


/*****************************************************************************
 结构名    : BBPCOMM_IRM_PS_GSM2_CTRL_UNION
 结构说明  : IRM_PS_GSM2_CTRL 寄存器结构定义。地址偏移量:0x004，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      irm_ps_gsm2_ctrl_reg;
    struct
    {
        unsigned long  irm_ps_gsm2_ctrl : 1;  /* bit[0]   : GBBP2，默认为0，1为使能。
                                                            注：GBBP2固定为辅卡。
                                                            V8R1不使用。 */
        unsigned long  reserved         : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_IRM_PS_GSM2_CTRL_UNION;
#define BBPCOMM_IRM_PS_GSM2_CTRL_irm_ps_gsm2_ctrl_START  (0)
#define BBPCOMM_IRM_PS_GSM2_CTRL_irm_ps_gsm2_ctrl_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_IRM_PS_CLK_BYPASS_UNION
 结构说明  : IRM_PS_CLK_BYPASS 寄存器结构定义。地址偏移量:0x008，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      irm_ps_clk_bypass_reg;
    struct
    {
        unsigned long  irm_ps_clk_bypass : 1;  /* bit[0]   : irm_ps模块时钟门控旁路开关。默认为0，即开启时钟门控。配置1则开启旁路功能，时钟门控失效。 */
        unsigned long  reserved          : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_IRM_PS_CLK_BYPASS_UNION;
#define BBPCOMM_IRM_PS_CLK_BYPASS_irm_ps_clk_bypass_START  (0)
#define BBPCOMM_IRM_PS_CLK_BYPASS_irm_ps_clk_bypass_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_MEM_CTRL0_TURBO_UNION
 结构说明  : MEM_CTRL0_TURBO 寄存器结构定义。地址偏移量:0x010，初值:0x0C9B0C9B，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      mem_ctrl0_turbo_reg;
    struct
    {
        unsigned long  mem_ctrl_s_turbo      : 16; /* bit[0-15] : 单端口ram延时控制。 */
        unsigned long  mem_ctrl_d_1w2r_turbo : 16; /* bit[16-31]: 1w2r双端口ram延时控制。 */
    } reg;
} BBPCOMM_MEM_CTRL0_TURBO_UNION;
#define BBPCOMM_MEM_CTRL0_TURBO_mem_ctrl_s_turbo_START       (0)
#define BBPCOMM_MEM_CTRL0_TURBO_mem_ctrl_s_turbo_END         (15)
#define BBPCOMM_MEM_CTRL0_TURBO_mem_ctrl_d_1w2r_turbo_START  (16)
#define BBPCOMM_MEM_CTRL0_TURBO_mem_ctrl_d_1w2r_turbo_END    (31)


/*****************************************************************************
 结构名    : BBPCOMM_MEM_CTRL1_TURBO_UNION
 结构说明  : MEM_CTRL1_TURBO 寄存器结构定义。地址偏移量:0x014，初值:0x00230C9B，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      mem_ctrl1_turbo_reg;
    struct
    {
        unsigned long  mem_ctrl_d_1rw2rw_turbo : 16; /* bit[0-15] : 1rw2rw双端口ram延时控制。 */
        unsigned long  rom_ctrl_turbo          : 8;  /* bit[16-23]: rom延时控制。 */
        unsigned long  reserved                : 8;  /* bit[24-31]: 保留 */
    } reg;
} BBPCOMM_MEM_CTRL1_TURBO_UNION;
#define BBPCOMM_MEM_CTRL1_TURBO_mem_ctrl_d_1rw2rw_turbo_START  (0)
#define BBPCOMM_MEM_CTRL1_TURBO_mem_ctrl_d_1rw2rw_turbo_END    (15)
#define BBPCOMM_MEM_CTRL1_TURBO_rom_ctrl_turbo_START           (16)
#define BBPCOMM_MEM_CTRL1_TURBO_rom_ctrl_turbo_END             (23)


/*****************************************************************************
 结构名    : BBPCOMM_MEM_CTRL0_PS_UNION
 结构说明  : MEM_CTRL0_PS 寄存器结构定义。地址偏移量:0x020，初值:0x0C9B0C9B，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      mem_ctrl0_ps_reg;
    struct
    {
        unsigned long  mem_ctrl_s_ps      : 16; /* bit[0-15] : 单端口ram延时控制。 */
        unsigned long  mem_ctrl_d_1w2r_ps : 16; /* bit[16-31]: 1w2r双端口ram延时控制。 */
    } reg;
} BBPCOMM_MEM_CTRL0_PS_UNION;
#define BBPCOMM_MEM_CTRL0_PS_mem_ctrl_s_ps_START       (0)
#define BBPCOMM_MEM_CTRL0_PS_mem_ctrl_s_ps_END         (15)
#define BBPCOMM_MEM_CTRL0_PS_mem_ctrl_d_1w2r_ps_START  (16)
#define BBPCOMM_MEM_CTRL0_PS_mem_ctrl_d_1w2r_ps_END    (31)


/*****************************************************************************
 结构名    : BBPCOMM_MEM_CTRL1_PS_UNION
 结构说明  : MEM_CTRL1_PS 寄存器结构定义。地址偏移量:0x024，初值:0x00230C9B，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      mem_ctrl1_ps_reg;
    struct
    {
        unsigned long  mem_ctrl_d_1rw2rw_ps : 16; /* bit[0-15] : 1rw2rw双端口ram延时控制。 */
        unsigned long  rom_ctrl_ps          : 8;  /* bit[16-23]: rom延时控制。 */
        unsigned long  reserved             : 8;  /* bit[24-31]: 保留 */
    } reg;
} BBPCOMM_MEM_CTRL1_PS_UNION;
#define BBPCOMM_MEM_CTRL1_PS_mem_ctrl_d_1rw2rw_ps_START  (0)
#define BBPCOMM_MEM_CTRL1_PS_mem_ctrl_d_1rw2rw_ps_END    (15)
#define BBPCOMM_MEM_CTRL1_PS_rom_ctrl_ps_START           (16)
#define BBPCOMM_MEM_CTRL1_PS_rom_ctrl_ps_END             (23)


/*****************************************************************************
 结构名    : BBPCOMM_TUBO_CKG_UNION
 结构说明  : TUBO_CKG 寄存器结构定义。地址偏移量:0x028，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      tubo_ckg_reg;
    struct
    {
        unsigned long  tubo_ram_ckg_en : 16; /* bit[0-15] : turbo译码子模块级门控旁路使能，高电平有效。 */
        unsigned long  tubo_mod_ckg_en : 16; /* bit[16-31]: turbo译码子模块级门控旁路使能，高电平有效。 */
    } reg;
} BBPCOMM_TUBO_CKG_UNION;
#define BBPCOMM_TUBO_CKG_tubo_ram_ckg_en_START  (0)
#define BBPCOMM_TUBO_CKG_tubo_ram_ckg_en_END    (15)
#define BBPCOMM_TUBO_CKG_tubo_mod_ckg_en_START  (16)
#define BBPCOMM_TUBO_CKG_tubo_mod_ckg_en_END    (31)


/*****************************************************************************
 结构名    : BBPCOMM_U_L_TYPE_UNION
 结构说明  : U_L_TYPE 寄存器结构定义。地址偏移量:0x02C，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      u_l_type_reg;
    struct
    {
        unsigned long  u_l_type : 1;  /* bit[0]   : U模/L模算法选择信号。 */
        unsigned long  reserved : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_U_L_TYPE_UNION;
#define BBPCOMM_U_L_TYPE_u_l_type_START  (0)
#define BBPCOMM_U_L_TYPE_u_l_type_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_U_T_TYPE_UNION
 结构说明  : U_T_TYPE 寄存器结构定义。地址偏移量:0x030，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      u_t_type_reg;
    struct
    {
        unsigned long  u_t_type : 1;  /* bit[0]   : U模/T模接口选择信号。 */
        unsigned long  reserved : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_U_T_TYPE_UNION;
#define BBPCOMM_U_T_TYPE_u_t_type_START  (0)
#define BBPCOMM_U_T_TYPE_u_t_type_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_U_C_TYPE_UNION
 结构说明  : U_C_TYPE 寄存器结构定义。地址偏移量:0x034，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      u_c_type_reg;
    struct
    {
        unsigned long  u_c_type : 1;  /* bit[0]   : U模/C模接口选择信号。 */
        unsigned long  reserved : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_U_C_TYPE_UNION;
#define BBPCOMM_U_C_TYPE_u_c_type_START  (0)
#define BBPCOMM_U_C_TYPE_u_c_type_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_SOC_AP_OCCUPY_GRP_UNION
 结构说明  : SOC_AP_OCCUPY_GRP 寄存器结构定义。地址偏移量:0x038，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      soc_ap_occupy_grp_reg;
    struct
    {
        unsigned long  soc_ap_occupy_grp1 : 2;  /* bit[0-1]  :  */
        unsigned long  soc_ap_occupy_grp2 : 2;  /* bit[2-3]  :  */
        unsigned long  soc_ap_occupy_grp3 : 2;  /* bit[4-5]  :  */
        unsigned long  soc_ap_occupy_grp4 : 2;  /* bit[6-7]  :  */
        unsigned long  soc_ap_occupy_grp5 : 2;  /* bit[8-9]  :  */
        unsigned long  soc_ap_occupy_grp6 : 2;  /* bit[10-11]:  */
        unsigned long  soc_ap_occupy_grp7 : 2;  /* bit[12-13]:  */
        unsigned long  soc_ap_occupy_grp8 : 2;  /* bit[14-15]: SOC AP配置给ps ram的occupy配置信号 */
        unsigned long  reserved           : 16; /* bit[16-31]: 保留 */
    } reg;
} BBPCOMM_SOC_AP_OCCUPY_GRP_UNION;
#define BBPCOMM_SOC_AP_OCCUPY_GRP_soc_ap_occupy_grp1_START  (0)
#define BBPCOMM_SOC_AP_OCCUPY_GRP_soc_ap_occupy_grp1_END    (1)
#define BBPCOMM_SOC_AP_OCCUPY_GRP_soc_ap_occupy_grp2_START  (2)
#define BBPCOMM_SOC_AP_OCCUPY_GRP_soc_ap_occupy_grp2_END    (3)
#define BBPCOMM_SOC_AP_OCCUPY_GRP_soc_ap_occupy_grp3_START  (4)
#define BBPCOMM_SOC_AP_OCCUPY_GRP_soc_ap_occupy_grp3_END    (5)
#define BBPCOMM_SOC_AP_OCCUPY_GRP_soc_ap_occupy_grp4_START  (6)
#define BBPCOMM_SOC_AP_OCCUPY_GRP_soc_ap_occupy_grp4_END    (7)
#define BBPCOMM_SOC_AP_OCCUPY_GRP_soc_ap_occupy_grp5_START  (8)
#define BBPCOMM_SOC_AP_OCCUPY_GRP_soc_ap_occupy_grp5_END    (9)
#define BBPCOMM_SOC_AP_OCCUPY_GRP_soc_ap_occupy_grp6_START  (10)
#define BBPCOMM_SOC_AP_OCCUPY_GRP_soc_ap_occupy_grp6_END    (11)
#define BBPCOMM_SOC_AP_OCCUPY_GRP_soc_ap_occupy_grp7_START  (12)
#define BBPCOMM_SOC_AP_OCCUPY_GRP_soc_ap_occupy_grp7_END    (13)
#define BBPCOMM_SOC_AP_OCCUPY_GRP_soc_ap_occupy_grp8_START  (14)
#define BBPCOMM_SOC_AP_OCCUPY_GRP_soc_ap_occupy_grp8_END    (15)


/*****************************************************************************
 结构名    : BBPCOMM_PS_W_SDR_EN_UNION
 结构说明  : PS_W_SDR_EN 寄存器结构定义。地址偏移量:0x03C，初值:0x00000000，宽度:32
 寄存器说明: 
*****************************************************************************/
typedef union
{
    unsigned long      ps_w_sdr_en_reg;
    struct
    {
        unsigned long  ps_w_sdr_en          : 1;  /* bit[0]   : W SDR模式开关使能。 */
        unsigned long  ps_w_sdr_dec_clk_sel : 1;  /* bit[1]   : W SDR模式下译码时钟选择。0，译码采用122m时钟；1，译码采用245m时钟。 */
        unsigned long  reserved             : 30; /* bit[2-31]: 保留 */
    } reg;
} BBPCOMM_PS_W_SDR_EN_UNION;
#define BBPCOMM_PS_W_SDR_EN_ps_w_sdr_en_START           (0)
#define BBPCOMM_PS_W_SDR_EN_ps_w_sdr_en_END             (0)
#define BBPCOMM_PS_W_SDR_EN_ps_w_sdr_dec_clk_sel_START  (1)
#define BBPCOMM_PS_W_SDR_EN_ps_w_sdr_dec_clk_sel_END    (1)


/***======================================================================***
                     (5/5) register_define_abb_cfg
 ***======================================================================***/
/*****************************************************************************
 结构名    : BBPCOMM_SOFT_RST_UNION
 结构说明  : SOFT_RST 寄存器结构定义。地址偏移量:0x0000，初值:0x00000000，宽度:32
 寄存器说明: 软复位
*****************************************************************************/
typedef union
{
    unsigned long      soft_rst_reg;
    struct
    {
        unsigned long  soft_rst : 1;  /* bit[0]   : 复位寄存器：
                                                    1'b0：解除复位
                                                    1'b1:进入复位
                                                    配置时刻：任意
                                                    生效时刻：立即 */
        unsigned long  reserved : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_SOFT_RST_UNION;
#define BBPCOMM_SOFT_RST_soft_rst_START  (0)
#define BBPCOMM_SOFT_RST_soft_rst_END    (0)


/*****************************************************************************
 结构名    : BBPCOMM_CFG_CTRL0_UNION
 结构说明  : CFG_CTRL0 寄存器结构定义。地址偏移量:0x0004，初值:0x00000003，宽度:32
 寄存器说明: abb配置模式
*****************************************************************************/
typedef union
{
    unsigned long      cfg_ctrl0_reg;
    struct
    {
        unsigned long  abt_en   : 1;  /* bit[0]   : 仲裁使能：
                                                    1'b0:屏蔽仲裁功能
                                                    1'b1:使能仲裁功能 */
        unsigned long  cfg_mode : 1;  /* bit[1]   : abb配置模式：
                                                    1'b0:软件间接配置abb寄存器
                                                    1'b1:软件直接配置abb寄存器 */
        unsigned long  reserved : 30; /* bit[2-31]: 保留 */
    } reg;
} BBPCOMM_CFG_CTRL0_UNION;
#define BBPCOMM_CFG_CTRL0_abt_en_START    (0)
#define BBPCOMM_CFG_CTRL0_abt_en_END      (0)
#define BBPCOMM_CFG_CTRL0_cfg_mode_START  (1)
#define BBPCOMM_CFG_CTRL0_cfg_mode_END    (1)


/*****************************************************************************
 结构名    : BBPCOMM_CFG_CTRL1_UNION
 结构说明  : CFG_CTRL1 寄存器结构定义。地址偏移量:0x0008，初值:0x00000000，宽度:32
 寄存器说明: abb配置模式
*****************************************************************************/
typedef union
{
    unsigned long      cfg_ctrl1_reg;
    struct
    {
        unsigned long  cfg_rw   : 1;  /* bit[0]   : 软件读写标志：
                                                    1'b0:写
                                                    1'b1:读 */
        unsigned long  reserved : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_CFG_CTRL1_UNION;
#define BBPCOMM_CFG_CTRL1_cfg_rw_START    (0)
#define BBPCOMM_CFG_CTRL1_cfg_rw_END      (0)


/*****************************************************************************
 结构名    : BBPCOMM_CFG_CTRL2_UNION
 结构说明  : CFG_CTRL2 寄存器结构定义。地址偏移量:0x000C，初值:0x00000000，宽度:32
 寄存器说明: abb配置模式
*****************************************************************************/
typedef union
{
    unsigned long      cfg_ctrl2_reg;
    struct
    {
        unsigned long  txfifo_wl : 3;  /* bit[0-2] : 发送fifo水线，当发送fifo当前存储深度小于该水线时，产生发送fifo中断。
                                                     3'b000:1
                                                     3'b001:2
                                                     3'b010:3
                                                     3'b011:4
                                                     3'b100:5
                                                     3'b101:6
                                                     3'b110:7
                                                     3'b111:8 */
        unsigned long  rxfifo_wl : 3;  /* bit[3-5] : 接收fifo水线，当接收fifo当前存储深度大于该水线时，产生接收fifo中断。
                                                     3'b000:1
                                                     3'b001:2
                                                     3'b010:3
                                                     3'b011:4
                                                     3'b100:5
                                                     3'b101:6
                                                     3'b110:7
                                                     3'b111:8 */
        unsigned long  reserved  : 26; /* bit[6-31]: 保留 */
    } reg;
} BBPCOMM_CFG_CTRL2_UNION;
#define BBPCOMM_CFG_CTRL2_txfifo_wl_START  (0)
#define BBPCOMM_CFG_CTRL2_txfifo_wl_END    (2)
#define BBPCOMM_CFG_CTRL2_rxfifo_wl_START  (3)
#define BBPCOMM_CFG_CTRL2_rxfifo_wl_END    (5)


/*****************************************************************************
 结构名    : BBPCOMM_CFG_INT_MASK_UNION
 结构说明  : CFG_INT_MASK 寄存器结构定义。地址偏移量:0x0030，初值:0x00000000，宽度:32
 寄存器说明: 中断屏蔽寄存器
*****************************************************************************/
typedef union
{
    unsigned long      cfg_int_mask_reg;
    struct
    {
        unsigned long  rorim    : 1;  /* bit[0]   : 接收fifo溢出中断屏蔽位
                                                    1'b0:屏蔽
                                                    1'b1:不屏蔽 */
        unsigned long  reserved_0: 1;  /* bit[1]   : 保留 */
        unsigned long  rxim     : 1;  /* bit[2]   : 接收fifo中断屏蔽位
                                                    1'b0:屏蔽
                                                    1'b1:不屏蔽 */
        unsigned long  txim     : 1;  /* bit[3]   : 发送fifo中断屏蔽位
                                                    1'b0:屏蔽
                                                    1'b1:不屏蔽 */
        unsigned long  reserved_1: 28; /* bit[4-31]: 保留 */
    } reg;
} BBPCOMM_CFG_INT_MASK_UNION;
#define BBPCOMM_CFG_INT_MASK_rorim_START     (0)
#define BBPCOMM_CFG_INT_MASK_rorim_END       (0)
#define BBPCOMM_CFG_INT_MASK_rxim_START      (2)
#define BBPCOMM_CFG_INT_MASK_rxim_END        (2)
#define BBPCOMM_CFG_INT_MASK_txim_START      (3)
#define BBPCOMM_CFG_INT_MASK_txim_END        (3)


/*****************************************************************************
 结构名    : BBPCOMM_CFG_INT_CLR_UNION
 结构说明  : CFG_INT_CLR 寄存器结构定义。地址偏移量:0x0034，初值:0x00000000，宽度:32
 寄存器说明: 中断清除寄存器
*****************************************************************************/
typedef union
{
    unsigned long      cfg_int_clr_reg;
    struct
    {
        unsigned long  roric    : 1;  /* bit[0]   : 接收fifo溢出中断清除位
                                                    1'b0:不清除
                                                    1'b1:清除 */
        unsigned long  reserved_0: 1;  /* bit[1]   : 保留 */
        unsigned long  rxic     : 1;  /* bit[2]   : 接收fifo中断清除位
                                                    1'b0:不清除
                                                    1'b1:清除 */
        unsigned long  txic     : 1;  /* bit[3]   : 发送fifo中断清除位
                                                    1'b0:不清除
                                                    1'b1:清除 */
        unsigned long  reserved_1: 28; /* bit[4-31]: 保留 */
    } reg;
} BBPCOMM_CFG_INT_CLR_UNION;
#define BBPCOMM_CFG_INT_CLR_roric_START     (0)
#define BBPCOMM_CFG_INT_CLR_roric_END       (0)
#define BBPCOMM_CFG_INT_CLR_rxic_START      (2)
#define BBPCOMM_CFG_INT_CLR_rxic_END        (2)
#define BBPCOMM_CFG_INT_CLR_txic_START      (3)
#define BBPCOMM_CFG_INT_CLR_txic_END        (3)


/*****************************************************************************
 结构名    : BBPCOMM_CFG_INT_ALM_UNION
 结构说明  : CFG_INT_ALM 寄存器结构定义。地址偏移量:0x0038，初值:0x00000000，宽度:32
 寄存器说明: 中断告警寄存器
*****************************************************************************/
typedef union
{
    unsigned long      cfg_int_alm_reg;
    struct
    {
        unsigned long  roria    : 1;  /* bit[0]   : 接收fifo溢出中断告警寄存器，当新的中断来临时，老的中断还没有清除，产生此告警信号，写1清除。 */
        unsigned long  reserved_0: 1;  /* bit[1]   : 保留 */
        unsigned long  rxia     : 1;  /* bit[2]   : 接收fifo中断告警寄存器，当新的中断来临时，老的中断还没有清除，产生此告警信号，写1清除。 */
        unsigned long  txia     : 1;  /* bit[3]   : 发送fifo中断告警寄存器，当新的中断来临时，老的中断还没有清除，产生此告警信号，写1清除。 */
        unsigned long  reserved_1: 28; /* bit[4-31]: 保留 */
    } reg;
} BBPCOMM_CFG_INT_ALM_UNION;
#define BBPCOMM_CFG_INT_ALM_roria_START     (0)
#define BBPCOMM_CFG_INT_ALM_roria_END       (0)
#define BBPCOMM_CFG_INT_ALM_rxia_START      (2)
#define BBPCOMM_CFG_INT_ALM_rxia_END        (2)
#define BBPCOMM_CFG_INT_ALM_txia_START      (3)
#define BBPCOMM_CFG_INT_ALM_txia_END        (3)


/*****************************************************************************
 结构名    : BBPCOMM_MEM_CTRL_UNION
 结构说明  : MEM_CTRL 寄存器结构定义。地址偏移量:0x003C，初值:0x00000000，宽度:32
 寄存器说明: FIFO时钟门控
*****************************************************************************/
typedef union
{
    unsigned long      mem_ctrl_reg;
    struct
    {
        unsigned long  clk_sel  : 1;  /* bit[0]   : FIFO时钟门控使能信号
                                                    1'b0 : 不进行时钟门控
                                                    1'b1 : 进行时钟门控 */
        unsigned long  reserved : 31; /* bit[1-31]: 保留 */
    } reg;
} BBPCOMM_MEM_CTRL_UNION;
#define BBPCOMM_MEM_CTRL_clk_sel_START   (0)
#define BBPCOMM_MEM_CTRL_clk_sel_END     (0)


/*****************************************************************************
 结构名    : BBPCOMM_BBP_CFG_EN_UNION
 结构说明  : BBP_CFG_EN 寄存器结构定义。地址偏移量:0x00FC，初值:0x0000001F，宽度:32
 寄存器说明: BBP配置使能
*****************************************************************************/
typedef union
{
    unsigned long      bbp_cfg_en_reg;
    struct
    {
        unsigned long  bbp_cfg_en : 5;  /* bit[0-4] : 软件屏蔽各BBP配置ABB线控信号，
                                                      5'b1_1110:屏蔽LTE模；
                                                      5'b1_1101:屏蔽W模；
                                                      5'b1_1011:屏蔽TDS模；
                                                      5'b1_0111:屏蔽G1模；
                                                      5'b0_1111:屏蔽G2模；
                                                      5'b0_0000:全屏蔽
                                                      5'b1_1111:全不屏蔽（默认） */
        unsigned long  reserved   : 27; /* bit[5-31]: 保留 */
    } reg;
} BBPCOMM_BBP_CFG_EN_UNION;
#define BBPCOMM_BBP_CFG_EN_bbp_cfg_en_START  (0)
#define BBPCOMM_BBP_CFG_EN_bbp_cfg_en_END    (4)


/*****************************************************************************
 结构名    : BBPCOMM_L_CFG_CTRL_UNION
 结构说明  : L_CFG_CTRL 寄存器结构定义。地址偏移量:0x100，初值:0x00000000，宽度:32
 寄存器说明: LTE模式定时操作控制寄存器
*****************************************************************************/
typedef union
{
    unsigned long      l_cfg_ctrl_reg;
    struct
    {
        unsigned long  l_cfg_rw  : 1;  /* bit[0]   : lte模式定时操作读写控制：
                                                     1'b0：写操作
                                                     1'b1：读操作 */
        unsigned long  l_cfg_num : 3;  /* bit[1-3] : lte模式定时操作读写寄存器个数：
                                                     0：1
                                                     1：2
                                                     ...
                                                     7: 8 */
        unsigned long  reserved  : 28; /* bit[4-31]: 保留 */
    } reg;
} BBPCOMM_L_CFG_CTRL_UNION;
#define BBPCOMM_L_CFG_CTRL_l_cfg_rw_START   (0)
#define BBPCOMM_L_CFG_CTRL_l_cfg_rw_END     (0)
#define BBPCOMM_L_CFG_CTRL_l_cfg_num_START  (1)
#define BBPCOMM_L_CFG_CTRL_l_cfg_num_END    (3)


/*****************************************************************************
 结构名    : BBPCOMM_T_CFG_CTRL_UNION
 结构说明  : T_CFG_CTRL 寄存器结构定义。地址偏移量:0x104，初值:0x00000000，宽度:32
 寄存器说明: TDS模式定时操作控制寄存器
*****************************************************************************/
typedef union
{
    unsigned long      t_cfg_ctrl_reg;
    struct
    {
        unsigned long  t_cfg_rw  : 1;  /* bit[0]   : tds模式定时操作读写控制：
                                                     1'b0：写操作
                                                     1'b1：读操作 */
        unsigned long  t_cfg_num : 3;  /* bit[1-3] : tds模式定时操作读写寄存器个数：
                                                     0：1
                                                     1：2
                                                     ...
                                                     7: 8 */
        unsigned long  reserved  : 28; /* bit[4-31]: 保留 */
    } reg;
} BBPCOMM_T_CFG_CTRL_UNION;
#define BBPCOMM_T_CFG_CTRL_t_cfg_rw_START   (0)
#define BBPCOMM_T_CFG_CTRL_t_cfg_rw_END     (0)
#define BBPCOMM_T_CFG_CTRL_t_cfg_num_START  (1)
#define BBPCOMM_T_CFG_CTRL_t_cfg_num_END    (3)


/*****************************************************************************
 结构名    : BBPCOMM_W_CFG_CTRL_UNION
 结构说明  : W_CFG_CTRL 寄存器结构定义。地址偏移量:0x108，初值:0x00000000，宽度:32
 寄存器说明: W模式定时操作控制寄存器
*****************************************************************************/
typedef union
{
    unsigned long      w_cfg_ctrl_reg;
    struct
    {
        unsigned long  w_cfg_rw  : 1;  /* bit[0]   : wcdma模式定时操作读写控制：
                                                     1'b0：写操作
                                                     1'b1：读操作 */
        unsigned long  w_cfg_num : 3;  /* bit[1-3] : wcdma模式定时操作读写寄存器个数：
                                                     0：1
                                                     1：2
                                                     ...
                                                     7: 8 */
        unsigned long  reserved  : 28; /* bit[4-31]: 保留 */
    } reg;
} BBPCOMM_W_CFG_CTRL_UNION;
#define BBPCOMM_W_CFG_CTRL_w_cfg_rw_START   (0)
#define BBPCOMM_W_CFG_CTRL_w_cfg_rw_END     (0)
#define BBPCOMM_W_CFG_CTRL_w_cfg_num_START  (1)
#define BBPCOMM_W_CFG_CTRL_w_cfg_num_END    (3)


/*****************************************************************************
 结构名    : BBPCOMM_G1_CFG_CTRL_UNION
 结构说明  : G1_CFG_CTRL 寄存器结构定义。地址偏移量:0x10C，初值:0x00000000，宽度:32
 寄存器说明: G1模式定时操作控制寄存器
*****************************************************************************/
typedef union
{
    unsigned long      g1_cfg_ctrl_reg;
    struct
    {
        unsigned long  g1_cfg_rw  : 1;  /* bit[0]   : gsm1模式定时操作读写控制：
                                                      1'b0：写操作
                                                      1'b1：读操作 */
        unsigned long  g1_cfg_num : 3;  /* bit[1-3] : gsm1模式定时操作读写寄存器个数：
                                                      0：1
                                                      1：2
                                                      ...
                                                      7: 8 */
        unsigned long  reserved   : 28; /* bit[4-31]: 保留 */
    } reg;
} BBPCOMM_G1_CFG_CTRL_UNION;
#define BBPCOMM_G1_CFG_CTRL_g1_cfg_rw_START   (0)
#define BBPCOMM_G1_CFG_CTRL_g1_cfg_rw_END     (0)
#define BBPCOMM_G1_CFG_CTRL_g1_cfg_num_START  (1)
#define BBPCOMM_G1_CFG_CTRL_g1_cfg_num_END    (3)


/*****************************************************************************
 结构名    : BBPCOMM_G2_CFG_CTRL_UNION
 结构说明  : G2_CFG_CTRL 寄存器结构定义。地址偏移量:0x110，初值:0x00000000，宽度:32
 寄存器说明: G2模式定时操作控制寄存器
*****************************************************************************/
typedef union
{
    unsigned long      g2_cfg_ctrl_reg;
    struct
    {
        unsigned long  g2_cfg_rw  : 1;  /* bit[0]   : gsm2模式定时操作读写控制：
                                                      1'b0：写操作
                                                      1'b1：读操作 */
        unsigned long  g2_cfg_num : 3;  /* bit[1-3] : gsm2模式定时操作读写寄存器个数：
                                                      0：1
                                                      1：2
                                                      ...
                                                      7: 8 */
        unsigned long  reserved   : 28; /* bit[4-31]: 保留 */
    } reg;
} BBPCOMM_G2_CFG_CTRL_UNION;
#define BBPCOMM_G2_CFG_CTRL_g2_cfg_rw_START   (0)
#define BBPCOMM_G2_CFG_CTRL_g2_cfg_rw_END     (0)
#define BBPCOMM_G2_CFG_CTRL_g2_cfg_num_START  (1)
#define BBPCOMM_G2_CFG_CTRL_g2_cfg_num_END    (3)


/*****************************************************************************
 结构名    : BBPCOMM_CFG_INT_STATE_UNION
 结构说明  : CFG_INT_STATE 寄存器结构定义。地址偏移量:0x0200，初值:0x00000000，宽度:32
 寄存器说明: 中断状态存器
*****************************************************************************/
typedef union
{
    unsigned long      cfg_int_state_reg;
    struct
    {
        unsigned long  roris    : 1;  /* bit[0]   : 接收fifo溢出中断标志：
                                                    1'b0:无中断
                                                    1'b1:有中断 */
        unsigned long  reserved_0: 1;  /* bit[1]   : 保留 */
        unsigned long  rxis     : 1;  /* bit[2]   : 接收fifo中断标志：
                                                    1'b0:无中断
                                                    1'b1:有中断 */
        unsigned long  txis     : 1;  /* bit[3]   : 发送fifo中断标志：
                                                    1'b0:无中断
                                                    1'b1:有中断 */
        unsigned long  reserved_1: 28; /* bit[4-31]: 保留 */
    } reg;
} BBPCOMM_CFG_INT_STATE_UNION;
#define BBPCOMM_CFG_INT_STATE_roris_START     (0)
#define BBPCOMM_CFG_INT_STATE_roris_END       (0)
#define BBPCOMM_CFG_INT_STATE_rxis_START      (2)
#define BBPCOMM_CFG_INT_STATE_rxis_END        (2)
#define BBPCOMM_CFG_INT_STATE_txis_START      (3)
#define BBPCOMM_CFG_INT_STATE_txis_END        (3)


/*****************************************************************************
 结构名    : BBPCOMM_FIFO_STATE_UNION
 结构说明  : FIFO_STATE 寄存器结构定义。地址偏移量:0x0204，初值:0x00000000，宽度:32
 寄存器说明: FIFO状态存器
*****************************************************************************/
typedef union
{
    unsigned long      fifo_state_reg;
    struct
    {
        unsigned long  txfifo_empty : 1;  /* bit[0]    : 发送fifo空标志：
                                                         1'b0:非空
                                                         1'b1:空 */
        unsigned long  txfifo_full  : 1;  /* bit[1]    : 发送fifo满标志：
                                                         1'b0:非满
                                                         1'b1:满 */
        unsigned long  rxfifo_empty : 1;  /* bit[2]    : 接收fifo空标志：
                                                         1'b0:非空
                                                         1'b1:空 */
        unsigned long  rxfifo_full  : 1;  /* bit[3]    : 接收fifo满标志：
                                                         1'b0:非满
                                                         1'b1:满 */
        unsigned long  txfifo_cnt   : 4;  /* bit[4-7]  : txfifo中数据个数 */
        unsigned long  rxfifo_cnt   : 4;  /* bit[8-11] : rxfifo中数据个数 */
        unsigned long  reserved     : 20; /* bit[12-31]: 保留 */
    } reg;
} BBPCOMM_FIFO_STATE_UNION;
#define BBPCOMM_FIFO_STATE_txfifo_empty_START  (0)
#define BBPCOMM_FIFO_STATE_txfifo_empty_END    (0)
#define BBPCOMM_FIFO_STATE_txfifo_full_START   (1)
#define BBPCOMM_FIFO_STATE_txfifo_full_END     (1)
#define BBPCOMM_FIFO_STATE_rxfifo_empty_START  (2)
#define BBPCOMM_FIFO_STATE_rxfifo_empty_END    (2)
#define BBPCOMM_FIFO_STATE_rxfifo_full_START   (3)
#define BBPCOMM_FIFO_STATE_rxfifo_full_END     (3)
#define BBPCOMM_FIFO_STATE_txfifo_cnt_START    (4)
#define BBPCOMM_FIFO_STATE_txfifo_cnt_END      (7)
#define BBPCOMM_FIFO_STATE_rxfifo_cnt_START    (8)
#define BBPCOMM_FIFO_STATE_rxfifo_cnt_END      (11)


/*****************************************************************************
 结构名    : BBPCOMM_BBP_RW_STATE_UNION
 结构说明  : BBP_RW_STATE 寄存器结构定义。地址偏移量:0x0208，初值:0x00000000，宽度:32
 寄存器说明: BBP当前配置状态指示
*****************************************************************************/
typedef union
{
    unsigned long      bbp_rw_state_reg;
    struct
    {
        unsigned long  l_rw_state  : 1;  /* bit[0]   : l模当前配置ABB状态指示：
                                                       0：当前没有配置，可以启动新的配置
                                                       1：当前正在配置，不能启动新的配置 */
        unsigned long  w_rw_state  : 1;  /* bit[1]   : w模当前配置ABB状态指示：
                                                       0：当前没有配置，可以启动新的配置
                                                       1：当前正在配置，不能启动新的配置 */
        unsigned long  t_rw_state  : 1;  /* bit[2]   : t模当前配置ABB状态指示：
                                                       0：当前没有配置，可以启动新的配置
                                                       1：当前正在配置，不能启动新的配置 */
        unsigned long  g1_rw_state : 1;  /* bit[3]   : g1模当前配置ABB状态指示：
                                                       0：当前没有配置，可以启动新的配置
                                                       1：当前正在配置，不能启动新的配置 */
        unsigned long  g2_rw_state : 1;  /* bit[4]   : g2模当前配置ABB状态指示：
                                                       0：当前没有配置，可以启动新的配置
                                                       1：当前正在配置，不能启动新的配置 */
        unsigned long  reserved    : 27; /* bit[5-31]: 保留 */
    } reg;
} BBPCOMM_BBP_RW_STATE_UNION;
#define BBPCOMM_BBP_RW_STATE_l_rw_state_START   (0)
#define BBPCOMM_BBP_RW_STATE_l_rw_state_END     (0)
#define BBPCOMM_BBP_RW_STATE_w_rw_state_START   (1)
#define BBPCOMM_BBP_RW_STATE_w_rw_state_END     (1)
#define BBPCOMM_BBP_RW_STATE_t_rw_state_START   (2)
#define BBPCOMM_BBP_RW_STATE_t_rw_state_END     (2)
#define BBPCOMM_BBP_RW_STATE_g1_rw_state_START  (3)
#define BBPCOMM_BBP_RW_STATE_g1_rw_state_END    (3)
#define BBPCOMM_BBP_RW_STATE_g2_rw_state_START  (4)
#define BBPCOMM_BBP_RW_STATE_g2_rw_state_END    (4)




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

#endif /* end of bbp_comm_interface.h */
