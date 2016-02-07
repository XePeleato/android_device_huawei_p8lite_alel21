

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/

#ifndef __SOC_ASP_INTERFACE_H__
#define __SOC_ASP_INTERFACE_H__

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
/* 寄存器说明：发送通道TX0控制寄存器。
   位域定义UNION结构:  SOC_ASP_TX0_UNION */
#define SOC_ASP_TX0_ADDR(base)                        ((base) + (0x000))

/* 寄存器说明：发送通道TX1控制寄存器。
   位域定义UNION结构:  SOC_ASP_TX1_UNION */
#define SOC_ASP_TX1_ADDR(base)                        ((base) + (0x004))

/* 寄存器说明：发送通道TX2控制寄存器。
   位域定义UNION结构:  SOC_ASP_TX2_UNION */
#define SOC_ASP_TX2_ADDR(base)                        ((base) + (0x008))

/* 寄存器说明：发送通道TX3控制寄存器。
   位域定义UNION结构:  SOC_ASP_TX3_UNION */
#define SOC_ASP_TX3_ADDR(base)                        ((base) + (0x00C))

/* 寄存器说明：接受通道RX控制寄存器。
   位域定义UNION结构:  SOC_ASP_RX_UNION */
#define SOC_ASP_RX_ADDR(base)                         ((base) + (0x010))

/* 寄存器说明：ASP DMA使能寄存器。
   位域定义UNION结构:  SOC_ASP_DER_UNION */
#define SOC_ASP_DER_ADDR(base)                        ((base) + (0x014))

/* 寄存器说明：ASP DMA停止使能寄存器。
   位域定义UNION结构:  SOC_ASP_DSTOP_UNION */
#define SOC_ASP_DSTOP_ADDR(base)                      ((base) + (0x018))

/* 寄存器说明：ASP全局中断状态寄存器。
   位域定义UNION结构:  SOC_ASP_ASS_ISR_UNION */
#define SOC_ASP_ASS_ISR_ADDR(base)                    ((base) + (0x01C))

/* 寄存器说明：ASP原始状态寄存器。
   位域定义UNION结构:  SOC_ASP_IRSR_UNION */
#define SOC_ASP_IRSR_ADDR(base)                       ((base) + (0x020))

/* 寄存器说明：ASP中断使能寄存器。
   位域定义UNION结构:  SOC_ASP_IER_UNION */
#define SOC_ASP_IER_ADDR(base)                        ((base) + (0x024))

/* 寄存器说明：ASP屏蔽后状态寄存器。
   位域定义UNION结构:  SOC_ASP_IMSR_UNION */
#define SOC_ASP_IMSR_ADDR(base)                       ((base) + (0x028))

/* 寄存器说明：ASP中断清零寄存器。
   位域定义UNION结构:  SOC_ASP_ICR_UNION */
#define SOC_ASP_ICR_ADDR(base)                        ((base) + (0x02C))

/* 寄存器说明：通道新歌起始寄存器。
   位域定义UNION结构:  SOC_ASP_PCMNSSR_UNION */
#define SOC_ASP_PCMNSSR_ADDR(base)                    ((base) + (0x030))

/* 寄存器说明：TX0重采样寄存器。
   位域定义UNION结构:  SOC_ASP_P0RSRR_UNION */
#define SOC_ASP_P0RSRR_ADDR(base)                     ((base) + (0x034))

/* 寄存器说明：TX1重采样寄存器。
   位域定义UNION结构:  SOC_ASP_P1RSRR_UNION */
#define SOC_ASP_P1RSRR_ADDR(base)                     ((base) + (0x038))

/* 寄存器说明：TX2重采样寄存器。
   位域定义UNION结构:  SOC_ASP_P2RSRR_UNION */
#define SOC_ASP_P2RSRR_ADDR(base)                     ((base) + (0x03C))

/* 寄存器说明：TX3重采样寄存器。
   位域定义UNION结构:  SOC_ASP_P3RSRR_UNION */
#define SOC_ASP_P3RSRR_ADDR(base)                     ((base) + (0x040))

/* 寄存器说明：TX0淡入使能寄存器。
   位域定义UNION结构:  SOC_ASP_FADEINEN0_UNION */
#define SOC_ASP_FADEINEN0_ADDR(base)                  ((base) + (0x044))

/* 寄存器说明：TX0淡出使能寄存器。
   位域定义UNION结构:  SOC_ASP_FADEOUTEN0_UNION */
#define SOC_ASP_FADEOUTEN0_ADDR(base)                 ((base) + (0x048))

/* 寄存器说明：TX0淡入淡出速率。
   位域定义UNION结构:  SOC_ASP_FADERATE0_UNION */
#define SOC_ASP_FADERATE0_ADDR(base)                  ((base) + (0x04C))

/* 寄存器说明：TX1淡入使能寄存器。
   位域定义UNION结构:  SOC_ASP_FADEINEN1_UNION */
#define SOC_ASP_FADEINEN1_ADDR(base)                  ((base) + (0x050))

/* 寄存器说明：TX1淡出使能寄存器。
   位域定义UNION结构:  SOC_ASP_FADEOUTEN1_UNION */
#define SOC_ASP_FADEOUTEN1_ADDR(base)                 ((base) + (0x054))

/* 寄存器说明：TX1淡入淡出速率。
   位域定义UNION结构:  SOC_ASP_FADERATE1_UNION */
#define SOC_ASP_FADERATE1_ADDR(base)                  ((base) + (0x058))

/* 寄存器说明：TX2淡入使能寄存器。
   位域定义UNION结构:  SOC_ASP_FADEINEN2_UNION */
#define SOC_ASP_FADEINEN2_ADDR(base)                  ((base) + (0x05C))

/* 寄存器说明：TX2淡出使能寄存器。
   位域定义UNION结构:  SOC_ASP_FADEOUTEN2_UNION */
#define SOC_ASP_FADEOUTEN2_ADDR(base)                 ((base) + (0x060))

/* 寄存器说明：TX2淡入淡出速率。
   位域定义UNION结构:  SOC_ASP_FADERATE2_UNION */
#define SOC_ASP_FADERATE2_ADDR(base)                  ((base) + (0x064))

/* 寄存器说明：TX3淡入使能寄存器。
   位域定义UNION结构:  SOC_ASP_FADEINEN3_UNION */
#define SOC_ASP_FADEINEN3_ADDR(base)                  ((base) + (0x068))

/* 寄存器说明：TX3淡出使能寄存器。
   位域定义UNION结构:  SOC_ASP_FADEOUTEN3_UNION */
#define SOC_ASP_FADEOUTEN3_ADDR(base)                 ((base) + (0x06C))

/* 寄存器说明：TX3淡入淡出速率。
   位域定义UNION结构:  SOC_ASP_FADERATE3_UNION */
#define SOC_ASP_FADERATE3_ADDR(base)                  ((base) + (0x070))

/* 寄存器说明：PCM0通道左声道增益寄存器。
   位域定义UNION结构:  SOC_ASP_P0LCGR_UNION */
#define SOC_ASP_P0LCGR_ADDR(base)                     ((base) + (0x074))

/* 寄存器说明：PCM0通道右声道增益寄存器。
   位域定义UNION结构:  SOC_ASP_P0RCGR_UNION */
#define SOC_ASP_P0RCGR_ADDR(base)                     ((base) + (0x078))

/* 寄存器说明：PCM1通道左声道增益寄存器。
   位域定义UNION结构:  SOC_ASP_P1LCGR_UNION */
#define SOC_ASP_P1LCGR_ADDR(base)                     ((base) + (0x07C))

/* 寄存器说明：PCM1通道右声道增益寄存器。
   位域定义UNION结构:  SOC_ASP_P1RCGR_UNION */
#define SOC_ASP_P1RCGR_ADDR(base)                     ((base) + (0x080))

/* 寄存器说明：PCM2通道左声道增益寄存器。
   位域定义UNION结构:  SOC_ASP_P2LCGR_UNION */
#define SOC_ASP_P2LCGR_ADDR(base)                     ((base) + (0x084))

/* 寄存器说明：PCM2通道右声道增益寄存器。
   位域定义UNION结构:  SOC_ASP_P2RCGR_UNION */
#define SOC_ASP_P2RCGR_ADDR(base)                     ((base) + (0x088))

/* 寄存器说明：PCM3通道0声道增益寄存器。
   位域定义UNION结构:  SOC_ASP_P30CGR_UNION */
#define SOC_ASP_P30CGR_ADDR(base)                     ((base) + (0x08C))

/* 寄存器说明：PCM3通道1声道增益寄存器。
   位域定义UNION结构:  SOC_ASP_P31CGR_UNION */
#define SOC_ASP_P31CGR_ADDR(base)                     ((base) + (0x090))

/* 寄存器说明：PCM3通道2声道增益寄存器。
   位域定义UNION结构:  SOC_ASP_P32CGR_UNION */
#define SOC_ASP_P32CGR_ADDR(base)                     ((base) + (0x094))

/* 寄存器说明：PCM3通道3声道增益寄存器。
   位域定义UNION结构:  SOC_ASP_P33CGR_UNION */
#define SOC_ASP_P33CGR_ADDR(base)                     ((base) + (0x098))

/* 寄存器说明：PCM3通道4声道增益寄存器。
   位域定义UNION结构:  SOC_ASP_P34CGR_UNION */
#define SOC_ASP_P34CGR_ADDR(base)                     ((base) + (0x09C))

/* 寄存器说明：PCM3通道5声道增益寄存器。
   位域定义UNION结构:  SOC_ASP_P35CGR_UNION */
#define SOC_ASP_P35CGR_ADDR(base)                     ((base) + (0x0A0))

/* 寄存器说明：PCM3通道6声道增益寄存器。
   位域定义UNION结构:  SOC_ASP_P36CGR_UNION */
#define SOC_ASP_P36CGR_ADDR(base)                     ((base) + (0x0A4))

/* 寄存器说明：PCM3通道7声道增益寄存器。
   位域定义UNION结构:  SOC_ASP_P37CGR_UNION */
#define SOC_ASP_P37CGR_ADDR(base)                     ((base) + (0x0A8))

/* 寄存器说明：混音配置寄存器。
   位域定义UNION结构:  SOC_ASP_MIXCTL_UNION */
#define SOC_ASP_MIXCTL_ADDR(base)                     ((base) + (0x0AC))

/* 寄存器说明：PCMRXA通道搬运起始地址寄存器。
   位域定义UNION结构:  SOC_ASP_PRASAR_UNION */
#define SOC_ASP_PRASAR_ADDR(base)                     ((base) + (0x0B0))

/* 寄存器说明：PCMRXA通道搬运长度寄存器。
   位域定义UNION结构:  SOC_ASP_PRADLR_UNION */
#define SOC_ASP_PRADLR_ADDR(base)                     ((base) + (0x0B4))

/* 寄存器说明：PCMRXB通道搬运起始地址寄存器。
   位域定义UNION结构:  SOC_ASP_PRBSAR_UNION */
#define SOC_ASP_PRBSAR_ADDR(base)                     ((base) + (0x0B8))

/* 寄存器说明：PCMRXB通道搬运长度寄存器。
   位域定义UNION结构:  SOC_ASP_PRBDLR_UNION */
#define SOC_ASP_PRBDLR_ADDR(base)                     ((base) + (0x0BC))

/* 寄存器说明：PCM0A通道搬运起始地址寄存器。
   位域定义UNION结构:  SOC_ASP_P0ASAR_UNION */
#define SOC_ASP_P0ASAR_ADDR(base)                     ((base) + (0x0C0))

/* 寄存器说明：PCM0A通道搬运长度寄存器。
   位域定义UNION结构:  SOC_ASP_P0ADLR_UNION */
#define SOC_ASP_P0ADLR_ADDR(base)                     ((base) + (0x0C4))

/* 寄存器说明：PCM0B通道搬运起始地址寄存器。
   位域定义UNION结构:  SOC_ASP_P0BSAR_UNION */
#define SOC_ASP_P0BSAR_ADDR(base)                     ((base) + (0x0C8))

/* 寄存器说明：PCM0B通道搬运长度寄存器。
   位域定义UNION结构:  SOC_ASP_P0BDLR_UNION */
#define SOC_ASP_P0BDLR_ADDR(base)                     ((base) + (0x0CC))

/* 寄存器说明：PCM1A通道搬运起始地址寄存器。
   位域定义UNION结构:  SOC_ASP_P1ASAR_UNION */
#define SOC_ASP_P1ASAR_ADDR(base)                     ((base) + (0x0D0))

/* 寄存器说明：PCM1A通道搬运长度寄存器。
   位域定义UNION结构:  SOC_ASP_P1ADLR_UNION */
#define SOC_ASP_P1ADLR_ADDR(base)                     ((base) + (0x0D4))

/* 寄存器说明：PCM1B通道搬运起始地址寄存器
   位域定义UNION结构:  SOC_ASP_P1BSAR_UNION */
#define SOC_ASP_P1BSAR_ADDR(base)                     ((base) + (0x0D8))

/* 寄存器说明：PCM1B通道搬运长度寄存器。
   位域定义UNION结构:  SOC_ASP_P1BDLR_UNION */
#define SOC_ASP_P1BDLR_ADDR(base)                     ((base) + (0x0DC))

/* 寄存器说明：PCM2A通道搬运起始地址寄存器。
   位域定义UNION结构:  SOC_ASP_P2ASAR_UNION */
#define SOC_ASP_P2ASAR_ADDR(base)                     ((base) + (0x0E0))

/* 寄存器说明：PCM2A通道搬运长度寄存器。
   位域定义UNION结构:  SOC_ASP_P2ADLR_UNION */
#define SOC_ASP_P2ADLR_ADDR(base)                     ((base) + (0x0E4))

/* 寄存器说明：PCM2B通道搬运起始地址寄存器。
   位域定义UNION结构:  SOC_ASP_P2BSAR_UNION */
#define SOC_ASP_P2BSAR_ADDR(base)                     ((base) + (0x0E8))

/* 寄存器说明：PCM2B通道搬运长度寄存器。
   位域定义UNION结构:  SOC_ASP_P2BDLR_UNION */
#define SOC_ASP_P2BDLR_ADDR(base)                     ((base) + (0x0EC))

/* 寄存器说明：PCM3A通道搬运起始地址寄存器。
   位域定义UNION结构:  SOC_ASP_P3ASAR_UNION */
#define SOC_ASP_P3ASAR_ADDR(base)                     ((base) + (0x0F0))

/* 寄存器说明：PCM3A通道搬运长度寄存器。
   位域定义UNION结构:  SOC_ASP_P3ADLR_UNION */
#define SOC_ASP_P3ADLR_ADDR(base)                     ((base) + (0x0F4))

/* 寄存器说明：PCM3B通道搬运起始地址寄存器。
   位域定义UNION结构:  SOC_ASP_P3BSAR_UNION */
#define SOC_ASP_P3BSAR_ADDR(base)                     ((base) + (0x0F8))

/* 寄存器说明：PCM3B通道搬运长度寄存器。
   位域定义UNION结构:  SOC_ASP_P3BDLR_UNION */
#define SOC_ASP_P3BDLR_ADDR(base)                     ((base) + (0x0FC))

/* 寄存器说明：混音数据播放接口选择寄存器。
   位域定义UNION结构:  SOC_ASP_SPDIFSEL_UNION */
#define SOC_ASP_SPDIFSEL_ADDR(base)                   ((base) + (0x100))

/* 寄存器说明：DMA停止时PCM0A通道搬运长度状态寄存器。
   位域定义UNION结构:  SOC_ASP_P0ADLS_UNION */
#define SOC_ASP_P0ADLS_ADDR(base)                     ((base) + (0x104))

/* 寄存器说明：DMA停止时PCM0B通道搬运长度状态寄存器。
   位域定义UNION结构:  SOC_ASP_P0BDLS_UNION */
#define SOC_ASP_P0BDLS_ADDR(base)                     ((base) + (0x108))

/* 寄存器说明：DMA停止时PCM1A通道搬运长度状态寄存器。
   位域定义UNION结构:  SOC_ASP_P1ADLS_UNION */
#define SOC_ASP_P1ADLS_ADDR(base)                     ((base) + (0x10C))

/* 寄存器说明：DMA停止时PCM1B通道搬运长度状态寄存器。
   位域定义UNION结构:  SOC_ASP_P1BDLS_UNION */
#define SOC_ASP_P1BDLS_ADDR(base)                     ((base) + (0x110))

/* 寄存器说明：DMA停止时PCM2A通道搬运长度状态寄存器。
   位域定义UNION结构:  SOC_ASP_P2ADLS_UNION */
#define SOC_ASP_P2ADLS_ADDR(base)                     ((base) + (0x114))

/* 寄存器说明：DMA停止时PCM2B通道搬运长度状态寄存器。
   位域定义UNION结构:  SOC_ASP_P2BDLS_UNION */
#define SOC_ASP_P2BDLS_ADDR(base)                     ((base) + (0x118))

/* 寄存器说明：DMA停止时PCM3A通道搬运长度状态寄存器。
   位域定义UNION结构:  SOC_ASP_P3ADLS_UNION */
#define SOC_ASP_P3ADLS_ADDR(base)                     ((base) + (0x11C))

/* 寄存器说明：DMA停止时PCM3B通道搬运长度状态寄存器。
   位域定义UNION结构:  SOC_ASP_P3BDLS_UNION */
#define SOC_ASP_P3BDLS_ADDR(base)                     ((base) + (0x120))

/* 寄存器说明：DMA停止时PCMRXA通道搬运长度状态寄存器。
   位域定义UNION结构:  SOC_ASP_PRADLS_UNION */
#define SOC_ASP_PRADLS_ADDR(base)                     ((base) + (0x124))

/* 寄存器说明：DMA停止时PCMRXB通道搬运长度状态寄存器。
   位域定义UNION结构:  SOC_ASP_PRBDLS_UNION */
#define SOC_ASP_PRBDLS_ADDR(base)                     ((base) + (0x128))

/* 寄存器说明：DMA搬运数据缓存FIFO状态寄存器。
   位域定义UNION结构:  SOC_ASP_FDS_UNION */
#define SOC_ASP_FDS_ADDR(base)                        ((base) + (0x12C))

/* 寄存器说明：PCM0A通道DMA搬运剩余长度状态寄存器。
   位域定义UNION结构:  SOC_ASP_P0ADL_UNION */
#define SOC_ASP_P0ADL_ADDR(base)                      ((base) + (0x130))

/* 寄存器说明：PCM0B通道DMA搬运剩余长度状态寄存器。
   位域定义UNION结构:  SOC_ASP_P0BDL_UNION */
#define SOC_ASP_P0BDL_ADDR(base)                      ((base) + (0x134))

/* 寄存器说明：PCM1A通道DMA搬运剩余长度状态寄存器。
   位域定义UNION结构:  SOC_ASP_P1ADL_UNION */
#define SOC_ASP_P1ADL_ADDR(base)                      ((base) + (0x138))

/* 寄存器说明：PCM1B通道DMA搬运剩余长度状态寄存器。
   位域定义UNION结构:  SOC_ASP_P1BDL_UNION */
#define SOC_ASP_P1BDL_ADDR(base)                      ((base) + (0x13C))

/* 寄存器说明：PCM2A通道DMA搬运剩余长度状态寄存器。
   位域定义UNION结构:  SOC_ASP_P2ADL_UNION */
#define SOC_ASP_P2ADL_ADDR(base)                      ((base) + (0x140))

/* 寄存器说明：PCM2B通道DMA搬运剩余长度状态寄存器。
   位域定义UNION结构:  SOC_ASP_P2BDL_UNION */
#define SOC_ASP_P2BDL_ADDR(base)                      ((base) + (0x144))

/* 寄存器说明：PCM3A通道DMA搬运剩余长度状态寄存器。
   位域定义UNION结构:  SOC_ASP_P3ADL_UNION */
#define SOC_ASP_P3ADL_ADDR(base)                      ((base) + (0x148))

/* 寄存器说明：PCM3B通道DMA搬运剩余长度状态寄存器。
   位域定义UNION结构:  SOC_ASP_P3BDL_UNION */
#define SOC_ASP_P3BDL_ADDR(base)                      ((base) + (0x14C))

/* 寄存器说明：PCMRXA通道DMA搬运剩余长度状态寄存器。
   位域定义UNION结构:  SOC_ASP_PRADL_UNION */
#define SOC_ASP_PRADL_ADDR(base)                      ((base) + (0x150))

/* 寄存器说明：PCMRXB通道DMA搬运剩余长度状态寄存器。
   位域定义UNION结构:  SOC_ASP_PRBDL_UNION */
#define SOC_ASP_PRBDL_ADDR(base)                      ((base) + (0x154))





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
 结构名    : SOC_ASP_TX0_UNION
 结构说明  : TX0 寄存器结构定义。地址偏移量:0x000，初值:0x00000000，宽度:32
 寄存器说明: 发送通道TX0控制寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  p0dt     : 3;  /* bit[0-2] : TX0通道音频数据格式选择。
                                                   000－stereo 16bit；
                                                   001－mono 16bit；
                                                   010－stereo 8bit；
                                                   011－mono 8bit；
                                                   100- stereo 18bit；
                                                   101-stereo 20bit；
                                                   110和其他-stereo 24bit。 */
        unsigned int  p0rhp    : 1;  /* bit[3]   : TX0通道音频数据左右声道位置选择。
                                                   0：高位为左声道数据；
                                                   1：高位为右声道数据。 */
        unsigned int  p0bdian  : 1;  /* bit[4]   : TX0通道音频数据大小端选择。
                                                   0：小端；
                                                   1：大端。 */
        unsigned int  p08bit   : 1;  /* bit[5]   : TX0 8bit数据的符号支持。
                                                   0：有符号数；
                                                   1：无符号数。 */
        unsigned int  pcm0en   : 1;  /* bit[6]   : TX0通道使能。
                                                   0：不使能；
                                                   1：使能。 */
        unsigned int  pcm0ge   : 1;  /* bit[7]   : PCM0通道增益使能。
                                                   0：不使能；
                                                   1：使能。 */
        unsigned int  odsr     : 1;  /* bit[8]   : Resample模块输出数据采样率选择。
                                                   0：48KHz；
                                                   1：96KHz。 */
        unsigned int  reserved : 23; /* bit[9-31]: 保留。 */
    } reg;
} SOC_ASP_TX0_UNION;
#endif
#define SOC_ASP_TX0_p0dt_START      (0)
#define SOC_ASP_TX0_p0dt_END        (2)
#define SOC_ASP_TX0_p0rhp_START     (3)
#define SOC_ASP_TX0_p0rhp_END       (3)
#define SOC_ASP_TX0_p0bdian_START   (4)
#define SOC_ASP_TX0_p0bdian_END     (4)
#define SOC_ASP_TX0_p08bit_START    (5)
#define SOC_ASP_TX0_p08bit_END      (5)
#define SOC_ASP_TX0_pcm0en_START    (6)
#define SOC_ASP_TX0_pcm0en_END      (6)
#define SOC_ASP_TX0_pcm0ge_START    (7)
#define SOC_ASP_TX0_pcm0ge_END      (7)
#define SOC_ASP_TX0_odsr_START      (8)
#define SOC_ASP_TX0_odsr_END        (8)


/*****************************************************************************
 结构名    : SOC_ASP_TX1_UNION
 结构说明  : TX1 寄存器结构定义。地址偏移量:0x004，初值:0x00000000，宽度:32
 寄存器说明: 发送通道TX1控制寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  p1dt     : 3;  /* bit[0-2]  : TX1通道音频数据格式选择。
                                                    000－stereo 16bit；
                                                    001－mono 16bit；
                                                    010－stereo 8bit；
                                                    011－mono 8bit；
                                                    100- stereo 18bit；
                                                    101-stereo 20bit；
                                                    110和其他-stereo 24bit。 */
        unsigned int  p1rhp    : 1;  /* bit[3]    : TX1通道音频数据左右声道位置选择。
                                                    0：高位为左声道数据；
                                                    1：高位为右声道数据。 */
        unsigned int  p1bdian  : 1;  /* bit[4]    : TX1通道音频数据大小端选择。
                                                    0：小端；
                                                    1：大端。 */
        unsigned int  p18bit   : 1;  /* bit[5]    : TX1 8bit数据的符号支持。
                                                    0：有符号数；
                                                    1：无符号数。 */
        unsigned int  pcm1en   : 1;  /* bit[6]    : TX1通道使能。
                                                    0：不使能；
                                                    1：使能。 */
        unsigned int  pcm1ge   : 1;  /* bit[7]    : TX1通道增益使能。
                                                    0：不使能；
                                                    1：使能。 */
        unsigned int  mixrate  : 2;  /* bit[8-9]  : TX1/2/3混音后输出数据采样率选择。
                                                    00：48KHz；
                                                    01：96KHz；
                                                    10~11：192KHz。 */
        unsigned int  reserved : 22; /* bit[10-31]: 保留。 */
    } reg;
} SOC_ASP_TX1_UNION;
#endif
#define SOC_ASP_TX1_p1dt_START      (0)
#define SOC_ASP_TX1_p1dt_END        (2)
#define SOC_ASP_TX1_p1rhp_START     (3)
#define SOC_ASP_TX1_p1rhp_END       (3)
#define SOC_ASP_TX1_p1bdian_START   (4)
#define SOC_ASP_TX1_p1bdian_END     (4)
#define SOC_ASP_TX1_p18bit_START    (5)
#define SOC_ASP_TX1_p18bit_END      (5)
#define SOC_ASP_TX1_pcm1en_START    (6)
#define SOC_ASP_TX1_pcm1en_END      (6)
#define SOC_ASP_TX1_pcm1ge_START    (7)
#define SOC_ASP_TX1_pcm1ge_END      (7)
#define SOC_ASP_TX1_mixrate_START   (8)
#define SOC_ASP_TX1_mixrate_END     (9)


/*****************************************************************************
 结构名    : SOC_ASP_TX2_UNION
 结构说明  : TX2 寄存器结构定义。地址偏移量:0x008，初值:0x00000000，宽度:32
 寄存器说明: 发送通道TX2控制寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  p2dt     : 3;  /* bit[0-2] : TX2通道音频数据格式选择。
                                                   000－stereo 16bit；
                                                   001－mono 16bit；
                                                   010－stereo 8bit；
                                                   011－mono 8bit；
                                                   100- stereo 18bit；
                                                   101-stereo 20bit；
                                                   110和其他-stereo 24bit。 */
        unsigned int  p2rhp    : 1;  /* bit[3]   : TX2通道音频数据左右声道位置选择。
                                                   0：高位为左声道数据；
                                                   1：高位为右声道数据。 */
        unsigned int  p2bdian  : 1;  /* bit[4]   : TX2通道音频数据大小端选择。
                                                   0：小端；
                                                   1：大端。 */
        unsigned int  p28bit   : 1;  /* bit[5]   : TX2 8bit数据的符号支持。
                                                   0：有符号数；
                                                   1：无符号数。 */
        unsigned int  pcm2en   : 1;  /* bit[6]   : TX2通道使能。
                                                   0：不使能；
                                                   1：使能。 */
        unsigned int  pcm2ge   : 1;  /* bit[7]   : TX2通道增益使能。
                                                   0：不使能；
                                                   1：使能。 */
        unsigned int  pcm2tsen : 1;  /* bit[8]   : TX2 透传使能。
                                                   0：不使能；
                                                   1：使能。 */
        unsigned int  reserved : 23; /* bit[9-31]: 保留。 */
    } reg;
} SOC_ASP_TX2_UNION;
#endif
#define SOC_ASP_TX2_p2dt_START      (0)
#define SOC_ASP_TX2_p2dt_END        (2)
#define SOC_ASP_TX2_p2rhp_START     (3)
#define SOC_ASP_TX2_p2rhp_END       (3)
#define SOC_ASP_TX2_p2bdian_START   (4)
#define SOC_ASP_TX2_p2bdian_END     (4)
#define SOC_ASP_TX2_p28bit_START    (5)
#define SOC_ASP_TX2_p28bit_END      (5)
#define SOC_ASP_TX2_pcm2en_START    (6)
#define SOC_ASP_TX2_pcm2en_END      (6)
#define SOC_ASP_TX2_pcm2ge_START    (7)
#define SOC_ASP_TX2_pcm2ge_END      (7)
#define SOC_ASP_TX2_pcm2tsen_START  (8)
#define SOC_ASP_TX2_pcm2tsen_END    (8)


/*****************************************************************************
 结构名    : SOC_ASP_TX3_UNION
 结构说明  : TX3 寄存器结构定义。地址偏移量:0x00C，初值:0x00000000，宽度:32
 寄存器说明: 发送通道TX3控制寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  p3dt     : 2;  /* bit[0-1] : TX3通道音频数据格式选择。
                                                   00－stereo 16bit；
                                                   01－stereo 18bit；
                                                   10－stereo 20bit；
                                                   11－stereo 24bit。 */
        unsigned int  pcm3en   : 1;  /* bit[2]   : TX3通道使能。
                                                   0：不使能；
                                                   1：使能。 */
        unsigned int  pcm3ge   : 1;  /* bit[3]   : TX3通道增益使能。
                                                   0：不使能；
                                                   1：使能。 */
        unsigned int  pcm3tsen : 1;  /* bit[4]   : TX3 透传使能。
                                                   0：不使能；
                                                   1：使能。 */
        unsigned int  reserved : 27; /* bit[5-31]: 保留。 */
    } reg;
} SOC_ASP_TX3_UNION;
#endif
#define SOC_ASP_TX3_p3dt_START      (0)
#define SOC_ASP_TX3_p3dt_END        (1)
#define SOC_ASP_TX3_pcm3en_START    (2)
#define SOC_ASP_TX3_pcm3en_END      (2)
#define SOC_ASP_TX3_pcm3ge_START    (3)
#define SOC_ASP_TX3_pcm3ge_END      (3)
#define SOC_ASP_TX3_pcm3tsen_START  (4)
#define SOC_ASP_TX3_pcm3tsen_END    (4)


/*****************************************************************************
 结构名    : SOC_ASP_RX_UNION
 结构说明  : RX 寄存器结构定义。地址偏移量:0x010，初值:0x00000000，宽度:32
 寄存器说明: 接受通道RX控制寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  rxen     : 1;  /* bit[0]   : RX通道使能。
                                                   0：不使能；
                                                   1：使能。 */
        unsigned int  reserved : 31; /* bit[1-31]: 保留。 */
    } reg;
} SOC_ASP_RX_UNION;
#endif
#define SOC_ASP_RX_rxen_START      (0)
#define SOC_ASP_RX_rxen_END        (0)


/*****************************************************************************
 结构名    : SOC_ASP_DER_UNION
 结构说明  : DER 寄存器结构定义。地址偏移量:0x014，初值:0x00000000，宽度:32
 寄存器说明: ASP DMA使能寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  pcmrxade : 1;  /* bit[0]    : PCMRX通道A组DMA使能。
                                                    0：DMA不使能；
                                                    1：DMA使能。
                                                    注意：该寄存器软件写“1”，硬件清零，软件写“0”无效。 */
        unsigned int  pcmrxbde : 1;  /* bit[1]    : PCMRX通道B组DMA使能。
                                                    0：DMA不使能；
                                                    1：DMA使能。
                                                    注意：该寄存器软件写“1”，硬件清零，软件写“0”无效。 */
        unsigned int  pcm0ade  : 1;  /* bit[2]    : PCM0通道A组DMA使能。
                                                    0：DMA不使能；
                                                    1：DMA使能。
                                                    注意：该寄存器软件写“1”，硬件清零，软件写“0”无效。 */
        unsigned int  pcm0bde  : 1;  /* bit[3]    : PCM0通道B组DMA使能。
                                                    0：DMA不使能；
                                                    1：DMA使能。
                                                    注意：该寄存器软件写“1”，硬件清零，软件写“0”无效。 */
        unsigned int  pcm1ade  : 1;  /* bit[4]    : PCM1通道A组DMA使能。
                                                    0：DMA不使能；
                                                    1：DMA使能。
                                                    注意：该寄存器软件写“1”，硬件清零，软件写“0”无效。 */
        unsigned int  pcm1bde  : 1;  /* bit[5]    : PCM1通道B组DMA使能。
                                                    0：DMA不使能；
                                                    1：DMA使能。
                                                    注意：该寄存器软件写“1”，硬件清零，软件写“0”无效。 */
        unsigned int  pcm2ade  : 1;  /* bit[6]    : PCM2通道A组DMA使能。
                                                    0：DMA不使能；
                                                    1：DMA使能。
                                                    注意：该寄存器软件写“1”，硬件清零，软件写“0”无效。 */
        unsigned int  pcm2bde  : 1;  /* bit[7]    : PCM2通道B组DMA使能。
                                                    0：DMA不使能；
                                                    1：DMA使能。
                                                    注意：该寄存器软件写“1”，硬件清零，软件写“0”无效。 */
        unsigned int  pcm3ade  : 1;  /* bit[8]    : PCM3通道A组DMA使能。
                                                    0：DMA不使能；
                                                    1：DMA使能。
                                                    注意：该寄存器软件写“1”，硬件清零，软件写“0”无效。 */
        unsigned int  pcm3bde  : 1;  /* bit[9]    : PCM3通道B组DMA使能。
                                                    0：DMA不使能；
                                                    1：DMA使能。
                                                    注意：该寄存器软件写“1”，硬件清零，软件写“0”无效。 */
        unsigned int  reserved : 22; /* bit[10-31]: 保留。 */
    } reg;
} SOC_ASP_DER_UNION;
#endif
#define SOC_ASP_DER_pcmrxade_START  (0)
#define SOC_ASP_DER_pcmrxade_END    (0)
#define SOC_ASP_DER_pcmrxbde_START  (1)
#define SOC_ASP_DER_pcmrxbde_END    (1)
#define SOC_ASP_DER_pcm0ade_START   (2)
#define SOC_ASP_DER_pcm0ade_END     (2)
#define SOC_ASP_DER_pcm0bde_START   (3)
#define SOC_ASP_DER_pcm0bde_END     (3)
#define SOC_ASP_DER_pcm1ade_START   (4)
#define SOC_ASP_DER_pcm1ade_END     (4)
#define SOC_ASP_DER_pcm1bde_START   (5)
#define SOC_ASP_DER_pcm1bde_END     (5)
#define SOC_ASP_DER_pcm2ade_START   (6)
#define SOC_ASP_DER_pcm2ade_END     (6)
#define SOC_ASP_DER_pcm2bde_START   (7)
#define SOC_ASP_DER_pcm2bde_END     (7)
#define SOC_ASP_DER_pcm3ade_START   (8)
#define SOC_ASP_DER_pcm3ade_END     (8)
#define SOC_ASP_DER_pcm3bde_START   (9)
#define SOC_ASP_DER_pcm3bde_END     (9)


/*****************************************************************************
 结构名    : SOC_ASP_DSTOP_UNION
 结构说明  : DSTOP 寄存器结构定义。地址偏移量:0x018，初值:0x00000000，宽度:32
 寄存器说明: ASP DMA停止使能寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  pcmrxads : 1;  /* bit[0]    : PCMRX通道A组DMA停止使能。
                                                    0：DMA停止不使能；
                                                    1：DMA停止。
                                                    注意：该寄存器软件写“1”，硬件清零，软件写“0”无效。 */
        unsigned int  pcmrxbds : 1;  /* bit[1]    : PCMRX通道B组DMA停止使能。
                                                    0：DMA停止不使能；
                                                    1：DMA停止。
                                                    注意：该寄存器软件写“1”，硬件清零，软件写“0”无效。 */
        unsigned int  pcm0ads  : 1;  /* bit[2]    : PCM0通道A组DMA停止使能。
                                                    0：DMA停止不使能；
                                                    1：DMA停止。
                                                    注意：该寄存器软件写“1”，硬件清零，软件写“0”无效。 */
        unsigned int  pcm0bds  : 1;  /* bit[3]    : PCM0通道B组DMA停止使能。
                                                    0：DMA停止不使能；
                                                    1：DMA停止。
                                                    注意：该寄存器软件写“1”，硬件清零，软件写“0”无效。 */
        unsigned int  pcm1ads  : 1;  /* bit[4]    : PCM1通道A组DMA停止使能。
                                                    0：DMA停止不使能；
                                                    1：DMA停止。
                                                    注意：该寄存器软件写“1”，硬件清零，软件写“0”无效。 */
        unsigned int  pcm1bds  : 1;  /* bit[5]    : PCM1通道B组DMA停止使能。
                                                    0：DMA停止不使能；
                                                    1：DMA停止。
                                                    注意：该寄存器软件写“1”，硬件清零，软件写“0”无效。 */
        unsigned int  pcm2ads  : 1;  /* bit[6]    : PCM2通道A组DMA停止使能。
                                                    0：DMA停止不使能；
                                                    1：DMA停止。
                                                    注意：该寄存器软件写“1”，硬件清零，软件写“0”无效。 */
        unsigned int  pcm2bds  : 1;  /* bit[7]    : PCM2通道B组DMA停止使能。
                                                    0：DMA停止不使能；
                                                    1：DMA停止。
                                                    注意：该寄存器软件写“1”，硬件清零，软件写“0”无效。 */
        unsigned int  pcm3ads  : 1;  /* bit[8]    : PCM3通道A组DMA停止使能。
                                                    0：DMA停止不使能；
                                                    1：DMA停止。
                                                    注意：该寄存器软件写“1”，硬件清零，软件写“0”无效。 */
        unsigned int  pcm3bds  : 1;  /* bit[9]    : PCM3通道B组DMA停止使能。
                                                    0：DMA停止不使能；
                                                    1：DMA停止。
                                                    注意：该寄存器软件写“1”，硬件清零，软件写“0”无效。 */
        unsigned int  reserved : 22; /* bit[10-31]: 保留。 */
    } reg;
} SOC_ASP_DSTOP_UNION;
#endif
#define SOC_ASP_DSTOP_pcmrxads_START  (0)
#define SOC_ASP_DSTOP_pcmrxads_END    (0)
#define SOC_ASP_DSTOP_pcmrxbds_START  (1)
#define SOC_ASP_DSTOP_pcmrxbds_END    (1)
#define SOC_ASP_DSTOP_pcm0ads_START   (2)
#define SOC_ASP_DSTOP_pcm0ads_END     (2)
#define SOC_ASP_DSTOP_pcm0bds_START   (3)
#define SOC_ASP_DSTOP_pcm0bds_END     (3)
#define SOC_ASP_DSTOP_pcm1ads_START   (4)
#define SOC_ASP_DSTOP_pcm1ads_END     (4)
#define SOC_ASP_DSTOP_pcm1bds_START   (5)
#define SOC_ASP_DSTOP_pcm1bds_END     (5)
#define SOC_ASP_DSTOP_pcm2ads_START   (6)
#define SOC_ASP_DSTOP_pcm2ads_END     (6)
#define SOC_ASP_DSTOP_pcm2bds_START   (7)
#define SOC_ASP_DSTOP_pcm2bds_END     (7)
#define SOC_ASP_DSTOP_pcm3ads_START   (8)
#define SOC_ASP_DSTOP_pcm3ads_END     (8)
#define SOC_ASP_DSTOP_pcm3bds_START   (9)
#define SOC_ASP_DSTOP_pcm3bds_END     (9)


/*****************************************************************************
 结构名    : SOC_ASP_ASS_ISR_UNION
 结构说明  : ASS_ISR 寄存器结构定义。地址偏移量:0x01C，初值:0x00000000，宽度:32
 寄存器说明: ASP全局中断状态寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sio0is   : 1;  /* bit[0]   : 音频SIO0接口部分模块中断状态。
                                                   0：无中断；
                                                   1：有中断。 */
        unsigned int  sio1is   : 1;  /* bit[1]   : 音频SIO1接口部分模块中断状态。
                                                   0：无中断；
                                                   1：有中断。 */
        unsigned int  spdifis  : 1;  /* bit[2]   : 音频SPDIF接口部分模块中断状态。
                                                   0：无中断；
                                                   1：有中断。 */
        unsigned int  aspis    : 1;  /* bit[3]   : ASP部分模块中断状态。
                                                   0：无中断；
                                                   1：有中断。 */
        unsigned int  reserved : 28; /* bit[4-31]: 保留。 */
    } reg;
} SOC_ASP_ASS_ISR_UNION;
#endif
#define SOC_ASP_ASS_ISR_sio0is_START    (0)
#define SOC_ASP_ASS_ISR_sio0is_END      (0)
#define SOC_ASP_ASS_ISR_sio1is_START    (1)
#define SOC_ASP_ASS_ISR_sio1is_END      (1)
#define SOC_ASP_ASS_ISR_spdifis_START   (2)
#define SOC_ASP_ASS_ISR_spdifis_END     (2)
#define SOC_ASP_ASS_ISR_aspis_START     (3)
#define SOC_ASP_ASS_ISR_aspis_END       (3)


/*****************************************************************************
 结构名    : SOC_ASP_IRSR_UNION
 结构说明  : IRSR 寄存器结构定义。地址偏移量:0x020，初值:0x00000000，宽度:32
 寄存器说明: ASP原始状态寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  pradfirs   : 1;  /* bit[0]    : PCMRX通道A组DMA结束原始中断。
                                                      0：无中断；
                                                      1：有中断。 */
        unsigned int  prbdfirs   : 1;  /* bit[1]    : PCMRX通道B组DMA结束原始中断。
                                                      0：无中断；
                                                      1：有中断。 */
        unsigned int  p0adfirs   : 1;  /* bit[2]    : PCM0通道A组DMA结束原始中断。
                                                      0：无中断；
                                                      1：有中断。 */
        unsigned int  p0bdfirs   : 1;  /* bit[3]    : PCM0通道B组DMA结束原始中断。
                                                      0：无中断；
                                                      1：有中断。 */
        unsigned int  p1adfirs   : 1;  /* bit[4]    : PCM1通道A组DMA结束原始中断。
                                                      0：无中断；
                                                      1：有中断。 */
        unsigned int  p1bdfirs   : 1;  /* bit[5]    : PCM1通道B组DMA结束原始中断。
                                                      0：无中断；
                                                      1：有中断。 */
        unsigned int  p2adfirs   : 1;  /* bit[6]    : PCM2通道A组DMA结束原始中断。
                                                      0：无中断；
                                                      1：有中断。 */
        unsigned int  p2bdfirs   : 1;  /* bit[7]    : PCM2通道B组DMA结束原始中断。
                                                      0：无中断；
                                                      1：有中断。 */
        unsigned int  p3adfirs   : 1;  /* bit[8]    : PCM3通道A组DMA结束原始中断。
                                                      0：无中断；
                                                      1：有中断。 */
        unsigned int  p3bdfirs   : 1;  /* bit[9]    : PCM3通道B组DMA结束原始中断。
                                                      0：无中断；
                                                      1：有中断。 */
        unsigned int  p0fadeins  : 1;  /* bit[10]   : TX0 fade in完成原始中断。
                                                      0：无中断；
                                                      1：有中断； */
        unsigned int  p1fadeins  : 1;  /* bit[11]   : TX1 fade in完成原始中断。
                                                      0：无中断；
                                                      1：有中断； */
        unsigned int  p2fadeins  : 1;  /* bit[12]   : TX2 fade in完成原始中断。
                                                      0：无中断；
                                                      1：有中断； */
        unsigned int  p3fadeins  : 1;  /* bit[13]   : TX3 fade in完成原始中断。
                                                      0：无中断；
                                                      1：有中断； */
        unsigned int  p0fadeouts : 1;  /* bit[14]   : TX0 fade out完成原始中断。
                                                      0：无中断；
                                                      1：有中断； */
        unsigned int  p1fadeouts : 1;  /* bit[15]   : TX1 fade out完成原始中断。
                                                      0：无中断；
                                                      1：有中断； */
        unsigned int  p2fadeouts : 1;  /* bit[16]   : TX2 fade out完成原始中断。
                                                      0：无中断；
                                                      1：有中断； */
        unsigned int  p3fadeouts : 1;  /* bit[17]   : TX3 fade out完成原始中断。
                                                      0：无中断；
                                                      1：有中断； */
        unsigned int  aberirs    : 1;  /* bit[18]   : ASP模块总线error响应原始中断。
                                                      0：无中断；
                                                      1：有中断。
                                                      注意：ASP的AHB master发出总线访问请求，总线上的Slave返回HRESP响应类型为ERROR时，产生该中断。例如，向RO类型地址作写操作等。 */
        unsigned int  reserved   : 13; /* bit[19-31]: 保留。 */
    } reg;
} SOC_ASP_IRSR_UNION;
#endif
#define SOC_ASP_IRSR_pradfirs_START    (0)
#define SOC_ASP_IRSR_pradfirs_END      (0)
#define SOC_ASP_IRSR_prbdfirs_START    (1)
#define SOC_ASP_IRSR_prbdfirs_END      (1)
#define SOC_ASP_IRSR_p0adfirs_START    (2)
#define SOC_ASP_IRSR_p0adfirs_END      (2)
#define SOC_ASP_IRSR_p0bdfirs_START    (3)
#define SOC_ASP_IRSR_p0bdfirs_END      (3)
#define SOC_ASP_IRSR_p1adfirs_START    (4)
#define SOC_ASP_IRSR_p1adfirs_END      (4)
#define SOC_ASP_IRSR_p1bdfirs_START    (5)
#define SOC_ASP_IRSR_p1bdfirs_END      (5)
#define SOC_ASP_IRSR_p2adfirs_START    (6)
#define SOC_ASP_IRSR_p2adfirs_END      (6)
#define SOC_ASP_IRSR_p2bdfirs_START    (7)
#define SOC_ASP_IRSR_p2bdfirs_END      (7)
#define SOC_ASP_IRSR_p3adfirs_START    (8)
#define SOC_ASP_IRSR_p3adfirs_END      (8)
#define SOC_ASP_IRSR_p3bdfirs_START    (9)
#define SOC_ASP_IRSR_p3bdfirs_END      (9)
#define SOC_ASP_IRSR_p0fadeins_START   (10)
#define SOC_ASP_IRSR_p0fadeins_END     (10)
#define SOC_ASP_IRSR_p1fadeins_START   (11)
#define SOC_ASP_IRSR_p1fadeins_END     (11)
#define SOC_ASP_IRSR_p2fadeins_START   (12)
#define SOC_ASP_IRSR_p2fadeins_END     (12)
#define SOC_ASP_IRSR_p3fadeins_START   (13)
#define SOC_ASP_IRSR_p3fadeins_END     (13)
#define SOC_ASP_IRSR_p0fadeouts_START  (14)
#define SOC_ASP_IRSR_p0fadeouts_END    (14)
#define SOC_ASP_IRSR_p1fadeouts_START  (15)
#define SOC_ASP_IRSR_p1fadeouts_END    (15)
#define SOC_ASP_IRSR_p2fadeouts_START  (16)
#define SOC_ASP_IRSR_p2fadeouts_END    (16)
#define SOC_ASP_IRSR_p3fadeouts_START  (17)
#define SOC_ASP_IRSR_p3fadeouts_END    (17)
#define SOC_ASP_IRSR_aberirs_START     (18)
#define SOC_ASP_IRSR_aberirs_END       (18)


/*****************************************************************************
 结构名    : SOC_ASP_IER_UNION
 结构说明  : IER 寄存器结构定义。地址偏移量:0x024，初值:0x00000000，宽度:32
 寄存器说明: ASP中断使能寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  pradfie     : 1;  /* bit[0]    : PCMRX通道A组DMA结束中断使能。
                                                       0：不使能；
                                                       1：使能。 */
        unsigned int  prbdfie     : 1;  /* bit[1]    : PCMRX通道B组DMA结束中断使能。
                                                       0：不使能；
                                                       1：使能。 */
        unsigned int  p0adfie     : 1;  /* bit[2]    : PCM0通道A组DMA结束中断使能。
                                                       0：不使能；
                                                       1：使能。 */
        unsigned int  p0bdfie     : 1;  /* bit[3]    : PCM0通道B组DMA结束中断使能。
                                                       0：不使能；
                                                       1：使能。 */
        unsigned int  p1adfie     : 1;  /* bit[4]    : PCM1通道A组DMA结束中断使能。
                                                       0：不使能；
                                                       1：使能。 */
        unsigned int  p1bdfie     : 1;  /* bit[5]    : PCM1通道B组DMA结束中断使能。
                                                       0：不使能；
                                                       1：使能。 */
        unsigned int  p2adfie     : 1;  /* bit[6]    : PCM2通道A组DMA结束中断使能。
                                                       0：不使能；
                                                       1：使能。 */
        unsigned int  p2bdfie     : 1;  /* bit[7]    : PCM2通道B组DMA结束中断使能。
                                                       0：不使能；
                                                       1：使能。 */
        unsigned int  p3adfie     : 1;  /* bit[8]    : PCM3通道A组DMA结束中断使能。
                                                       0：不使能；
                                                       1：使能。 */
        unsigned int  p3bdfie     : 1;  /* bit[9]    : PCM3通道B组DMA结束中断使能。
                                                       0：不使能；
                                                       1：使能。 */
        unsigned int  p0fadeinie  : 1;  /* bit[10]   : TX0 fade in完成中断使能。
                                                       0：不使能；
                                                       1：使能。 */
        unsigned int  p1fadeinie  : 1;  /* bit[11]   : TX1 fade in完成中断使能。
                                                       0：不使能；
                                                       1：使能。 */
        unsigned int  p2fadeinie  : 1;  /* bit[12]   : TX2 fade in完成中断使能。
                                                       0：不使能；
                                                       1：使能。 */
        unsigned int  p3fadeinie  : 1;  /* bit[13]   : TX3 fade in完成中断使能。
                                                       0：不使能；
                                                       1：使能。 */
        unsigned int  p0fadeoutie : 1;  /* bit[14]   : TX0 fade out完成中断使能。
                                                       0：不使能；
                                                       1：使能。 */
        unsigned int  p1fadeoutie : 1;  /* bit[15]   : TX1 fade out完成中断使能。
                                                       0：不使能；
                                                       1：使能。 */
        unsigned int  p2fadeoutie : 1;  /* bit[16]   : TX2 fade out完成中断使能。
                                                       0：不使能；
                                                       1：使能。 */
        unsigned int  p3fadeoutie : 1;  /* bit[17]   : TX3 fade out完成中断使能。
                                                       0：不使能；
                                                       1：使能。 */
        unsigned int  aberie      : 1;  /* bit[18]   : ASP模块总线error响应中断使能。
                                                       0：不使能；
                                                       1：使能。 */
        unsigned int  reserved    : 13; /* bit[19-31]: 保留。 */
    } reg;
} SOC_ASP_IER_UNION;
#endif
#define SOC_ASP_IER_pradfie_START      (0)
#define SOC_ASP_IER_pradfie_END        (0)
#define SOC_ASP_IER_prbdfie_START      (1)
#define SOC_ASP_IER_prbdfie_END        (1)
#define SOC_ASP_IER_p0adfie_START      (2)
#define SOC_ASP_IER_p0adfie_END        (2)
#define SOC_ASP_IER_p0bdfie_START      (3)
#define SOC_ASP_IER_p0bdfie_END        (3)
#define SOC_ASP_IER_p1adfie_START      (4)
#define SOC_ASP_IER_p1adfie_END        (4)
#define SOC_ASP_IER_p1bdfie_START      (5)
#define SOC_ASP_IER_p1bdfie_END        (5)
#define SOC_ASP_IER_p2adfie_START      (6)
#define SOC_ASP_IER_p2adfie_END        (6)
#define SOC_ASP_IER_p2bdfie_START      (7)
#define SOC_ASP_IER_p2bdfie_END        (7)
#define SOC_ASP_IER_p3adfie_START      (8)
#define SOC_ASP_IER_p3adfie_END        (8)
#define SOC_ASP_IER_p3bdfie_START      (9)
#define SOC_ASP_IER_p3bdfie_END        (9)
#define SOC_ASP_IER_p0fadeinie_START   (10)
#define SOC_ASP_IER_p0fadeinie_END     (10)
#define SOC_ASP_IER_p1fadeinie_START   (11)
#define SOC_ASP_IER_p1fadeinie_END     (11)
#define SOC_ASP_IER_p2fadeinie_START   (12)
#define SOC_ASP_IER_p2fadeinie_END     (12)
#define SOC_ASP_IER_p3fadeinie_START   (13)
#define SOC_ASP_IER_p3fadeinie_END     (13)
#define SOC_ASP_IER_p0fadeoutie_START  (14)
#define SOC_ASP_IER_p0fadeoutie_END    (14)
#define SOC_ASP_IER_p1fadeoutie_START  (15)
#define SOC_ASP_IER_p1fadeoutie_END    (15)
#define SOC_ASP_IER_p2fadeoutie_START  (16)
#define SOC_ASP_IER_p2fadeoutie_END    (16)
#define SOC_ASP_IER_p3fadeoutie_START  (17)
#define SOC_ASP_IER_p3fadeoutie_END    (17)
#define SOC_ASP_IER_aberie_START       (18)
#define SOC_ASP_IER_aberie_END         (18)


/*****************************************************************************
 结构名    : SOC_ASP_IMSR_UNION
 结构说明  : IMSR 寄存器结构定义。地址偏移量:0x028，初值:0x00000000，宽度:32
 寄存器说明: ASP屏蔽后状态寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  pradfims     : 1;  /* bit[0]    : PCMRX通道A组DMA结束屏蔽后中断。
                                                        0：无中断；
                                                        1：有中断。 */
        unsigned int  prbdfims     : 1;  /* bit[1]    : PCMRX通道B组DMA结束屏蔽后中断。
                                                        0：无中断；
                                                        1：有中断。 */
        unsigned int  p0adfims     : 1;  /* bit[2]    : PCM0通道A组DMA结束屏蔽后中断。
                                                        0：无中断；
                                                        1：有中断。 */
        unsigned int  p0bdfims     : 1;  /* bit[3]    : PCM0通道B组DMA结束屏蔽后中断。
                                                        0：无中断；
                                                        1：有中断。 */
        unsigned int  p1adfims     : 1;  /* bit[4]    : PCM1通道A组DMA结束屏蔽后中断。
                                                        0：无中断；
                                                        1：有中断。 */
        unsigned int  p1bdfims     : 1;  /* bit[5]    : PCM1通道B组DMA结束屏蔽后中断。
                                                        0：无中断；
                                                        1：有中断。 */
        unsigned int  p2adfims     : 1;  /* bit[6]    : PCM2通道A组DMA结束屏蔽后中断。
                                                        0：无中断；
                                                        1：有中断。 */
        unsigned int  p2bdfims     : 1;  /* bit[7]    : PCM2通道B组DMA结束屏蔽后中断。
                                                        0：无中断；
                                                        1：有中断。 */
        unsigned int  p3adfims     : 1;  /* bit[8]    : PCM3通道A组DMA结束屏蔽后中断。
                                                        0：无中断；
                                                        1：有中断。 */
        unsigned int  p3bdfims     : 1;  /* bit[9]    : PCM3通道B组DMA结束屏蔽后中断。
                                                        0：无中断；
                                                        1：有中断。 */
        unsigned int  p0fadeinims  : 1;  /* bit[10]   : TX0 fade in完成屏蔽后中断。
                                                        0：无中断；
                                                        1：有中断。 */
        unsigned int  p1fadeinims  : 1;  /* bit[11]   : TX1 fade in完成屏蔽后中断。
                                                        0：无中断；
                                                        1：有中断。 */
        unsigned int  p2fadeinims  : 1;  /* bit[12]   : TX2 fade in完成屏蔽后中断。
                                                        0：无中断；
                                                        1：有中断。 */
        unsigned int  p3fadeinims  : 1;  /* bit[13]   : TX3 fade in完成屏蔽后中断。
                                                        0：无中断；
                                                        1：有中断。 */
        unsigned int  p0fadeoutims : 1;  /* bit[14]   : TX0 fade out完成屏蔽后中断。
                                                        0：无中断；
                                                        1：有中断。 */
        unsigned int  p1fadeoutims : 1;  /* bit[15]   : TX1 fade out完成屏蔽后中断。
                                                        0：无中断；
                                                        1：有中断。 */
        unsigned int  p2fadeoutims : 1;  /* bit[16]   : TX2 fade out完成屏蔽后中断。
                                                        0：无中断；
                                                        1：有中断。 */
        unsigned int  p3fadeoutims : 1;  /* bit[17]   : TX3 fade out完成屏蔽后中断。
                                                        0：无中断；
                                                        1：有中断。 */
        unsigned int  aberims      : 1;  /* bit[18]   : ASP模块总线error响应屏蔽后中断。
                                                        0：无中断；
                                                        1：有中断。 */
        unsigned int  reserved     : 13; /* bit[19-31]: 保留。 */
    } reg;
} SOC_ASP_IMSR_UNION;
#endif
#define SOC_ASP_IMSR_pradfims_START      (0)
#define SOC_ASP_IMSR_pradfims_END        (0)
#define SOC_ASP_IMSR_prbdfims_START      (1)
#define SOC_ASP_IMSR_prbdfims_END        (1)
#define SOC_ASP_IMSR_p0adfims_START      (2)
#define SOC_ASP_IMSR_p0adfims_END        (2)
#define SOC_ASP_IMSR_p0bdfims_START      (3)
#define SOC_ASP_IMSR_p0bdfims_END        (3)
#define SOC_ASP_IMSR_p1adfims_START      (4)
#define SOC_ASP_IMSR_p1adfims_END        (4)
#define SOC_ASP_IMSR_p1bdfims_START      (5)
#define SOC_ASP_IMSR_p1bdfims_END        (5)
#define SOC_ASP_IMSR_p2adfims_START      (6)
#define SOC_ASP_IMSR_p2adfims_END        (6)
#define SOC_ASP_IMSR_p2bdfims_START      (7)
#define SOC_ASP_IMSR_p2bdfims_END        (7)
#define SOC_ASP_IMSR_p3adfims_START      (8)
#define SOC_ASP_IMSR_p3adfims_END        (8)
#define SOC_ASP_IMSR_p3bdfims_START      (9)
#define SOC_ASP_IMSR_p3bdfims_END        (9)
#define SOC_ASP_IMSR_p0fadeinims_START   (10)
#define SOC_ASP_IMSR_p0fadeinims_END     (10)
#define SOC_ASP_IMSR_p1fadeinims_START   (11)
#define SOC_ASP_IMSR_p1fadeinims_END     (11)
#define SOC_ASP_IMSR_p2fadeinims_START   (12)
#define SOC_ASP_IMSR_p2fadeinims_END     (12)
#define SOC_ASP_IMSR_p3fadeinims_START   (13)
#define SOC_ASP_IMSR_p3fadeinims_END     (13)
#define SOC_ASP_IMSR_p0fadeoutims_START  (14)
#define SOC_ASP_IMSR_p0fadeoutims_END    (14)
#define SOC_ASP_IMSR_p1fadeoutims_START  (15)
#define SOC_ASP_IMSR_p1fadeoutims_END    (15)
#define SOC_ASP_IMSR_p2fadeoutims_START  (16)
#define SOC_ASP_IMSR_p2fadeoutims_END    (16)
#define SOC_ASP_IMSR_p3fadeoutims_START  (17)
#define SOC_ASP_IMSR_p3fadeoutims_END    (17)
#define SOC_ASP_IMSR_aberims_START       (18)
#define SOC_ASP_IMSR_aberims_END         (18)


/*****************************************************************************
 结构名    : SOC_ASP_ICR_UNION
 结构说明  : ICR 寄存器结构定义。地址偏移量:0x02C，初值:0x00000000，宽度:32
 寄存器说明: ASP中断清零寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  pradficr     : 1;  /* bit[0]    : PCMRX通道A组DMA结束中断清零。
                                                        0：不清零；
                                                        1：清零。
                                                        注意：该寄存器软件写“1”才能清中断，软件写“0”无效。 */
        unsigned int  prbdficr     : 1;  /* bit[1]    : PCMRX通道B组DMA结束中断清零。
                                                        0：不清零；
                                                        1：清零。
                                                        注意：该寄存器软件写“1”才能清中断，软件写“0”无效。 */
        unsigned int  p0adficr     : 1;  /* bit[2]    : PCM0通道A组DMA结束中断清零。
                                                        0：不清零；
                                                        1：清零。
                                                        注意：该寄存器软件写“1”才能清中断，软件写“0”无效。 */
        unsigned int  p0bdficr     : 1;  /* bit[3]    : PCM0通道B组DMA结束中断清零。
                                                        0：不清零；
                                                        1：清零。
                                                        注意：该寄存器软件写“1”才能清中断，软件写“0”无效。 */
        unsigned int  p1adficr     : 1;  /* bit[4]    : PCM1通道A组DMA结束中断清零。
                                                        0：不清零；
                                                        1：清零。
                                                        注意：该寄存器软件写“1”才能清中断，软件写“0”无效。 */
        unsigned int  p1bdficr     : 1;  /* bit[5]    : PCM1通道B组DMA结束中断清零。
                                                        0：不清零；
                                                        1：清零。
                                                        注意：该寄存器软件写“1”才能清中断，软件写“0”无效。 */
        unsigned int  p2adficr     : 1;  /* bit[6]    : PCM2通道A组DMA结束中断清零。
                                                        0：不清零；
                                                        1：清零。
                                                        注意：该寄存器软件写“1”才能清中断，软件写“0”无效。 */
        unsigned int  p2bdficr     : 1;  /* bit[7]    : PCM2通道B组DMA结束中断清零。
                                                        0：不清零；
                                                        1：清零。
                                                        注意：该寄存器软件写“1”才能清中断，软件写“0”无效。 */
        unsigned int  p3adficr     : 1;  /* bit[8]    : PCM3通道A组DMA结束中断清零。
                                                        0：不清零；
                                                        1：清零。
                                                        注意：该寄存器软件写“1”才能清中断，软件写“0”无效。 */
        unsigned int  p3bdficr     : 1;  /* bit[9]    : PCM3通道B组DMA结束中断清零。
                                                        0：不清零；
                                                        1：清零。
                                                        注意：该寄存器软件写“1”才能清中断，软件写“0”无效。 */
        unsigned int  p0fadeinicr  : 1;  /* bit[10]   : TX0 fade in完成中断清零。
                                                        0：不清零；
                                                        1：清零。
                                                        注意：该寄存器软件写“1”才能清中断，软件写“0”无效。 */
        unsigned int  p1fadeinicr  : 1;  /* bit[11]   : TX1 fade in完成中断清零。
                                                        0：不清零；
                                                        1：清零。
                                                        注意：该寄存器软件写“1”才能清中断，软件写“0”无效。 */
        unsigned int  p2fadeinicr  : 1;  /* bit[12]   : TX2 fade in完成中断清零。
                                                        0：不清零；
                                                        1：清零。
                                                        注意：该寄存器软件写“1”才能清中断，软件写“0”无效。 */
        unsigned int  p3fadeinicr  : 1;  /* bit[13]   : TX3 fade in完成中断清零。
                                                        0：不清零；
                                                        1：清零。
                                                        注意：该寄存器软件写“1”才能清中断，软件写“0”无效。 */
        unsigned int  p0fadeouticr : 1;  /* bit[14]   : TX0 fade out完成中断清零。
                                                        0：不清零；
                                                        1：清零。
                                                        注意：该寄存器软件写“1”才能清中断，软件写“0”无效。 */
        unsigned int  p1fadeouticr : 1;  /* bit[15]   : TX1 fade out完成中断清零。
                                                        0：不清零；
                                                        1：清零。
                                                        注意：该寄存器软件写“1”才能清中断，软件写“0”无效。 */
        unsigned int  p2fadeouticr : 1;  /* bit[16]   : TX2 fade out完成中断清零。
                                                        0：不清零；
                                                        1：清零。
                                                        注意：该寄存器软件写“1”才能清中断，软件写“0”无效。 */
        unsigned int  p3fadeouticr : 1;  /* bit[17]   : TX3 fade out完成中断清零。
                                                        0：不清零；
                                                        1：清零。
                                                        注意：该寄存器软件写“1”才能清中断，软件写“0”无效。 */
        unsigned int  abericr      : 1;  /* bit[18]   : ASP模块总线error响应中断清零。
                                                        0：不清零；
                                                        1：清零。
                                                        注意：该寄存器软件写“1”才能清中断，软件写“0”无效。 */
        unsigned int  reserved     : 13; /* bit[19-31]: 保留。 */
    } reg;
} SOC_ASP_ICR_UNION;
#endif
#define SOC_ASP_ICR_pradficr_START      (0)
#define SOC_ASP_ICR_pradficr_END        (0)
#define SOC_ASP_ICR_prbdficr_START      (1)
#define SOC_ASP_ICR_prbdficr_END        (1)
#define SOC_ASP_ICR_p0adficr_START      (2)
#define SOC_ASP_ICR_p0adficr_END        (2)
#define SOC_ASP_ICR_p0bdficr_START      (3)
#define SOC_ASP_ICR_p0bdficr_END        (3)
#define SOC_ASP_ICR_p1adficr_START      (4)
#define SOC_ASP_ICR_p1adficr_END        (4)
#define SOC_ASP_ICR_p1bdficr_START      (5)
#define SOC_ASP_ICR_p1bdficr_END        (5)
#define SOC_ASP_ICR_p2adficr_START      (6)
#define SOC_ASP_ICR_p2adficr_END        (6)
#define SOC_ASP_ICR_p2bdficr_START      (7)
#define SOC_ASP_ICR_p2bdficr_END        (7)
#define SOC_ASP_ICR_p3adficr_START      (8)
#define SOC_ASP_ICR_p3adficr_END        (8)
#define SOC_ASP_ICR_p3bdficr_START      (9)
#define SOC_ASP_ICR_p3bdficr_END        (9)
#define SOC_ASP_ICR_p0fadeinicr_START   (10)
#define SOC_ASP_ICR_p0fadeinicr_END     (10)
#define SOC_ASP_ICR_p1fadeinicr_START   (11)
#define SOC_ASP_ICR_p1fadeinicr_END     (11)
#define SOC_ASP_ICR_p2fadeinicr_START   (12)
#define SOC_ASP_ICR_p2fadeinicr_END     (12)
#define SOC_ASP_ICR_p3fadeinicr_START   (13)
#define SOC_ASP_ICR_p3fadeinicr_END     (13)
#define SOC_ASP_ICR_p0fadeouticr_START  (14)
#define SOC_ASP_ICR_p0fadeouticr_END    (14)
#define SOC_ASP_ICR_p1fadeouticr_START  (15)
#define SOC_ASP_ICR_p1fadeouticr_END    (15)
#define SOC_ASP_ICR_p2fadeouticr_START  (16)
#define SOC_ASP_ICR_p2fadeouticr_END    (16)
#define SOC_ASP_ICR_p3fadeouticr_START  (17)
#define SOC_ASP_ICR_p3fadeouticr_END    (17)
#define SOC_ASP_ICR_abericr_START       (18)
#define SOC_ASP_ICR_abericr_END         (18)


/*****************************************************************************
 结构名    : SOC_ASP_PCMNSSR_UNION
 结构说明  : PCMNSSR 寄存器结构定义。地址偏移量:0x030，初值:0x00000000，宽度:32
 寄存器说明: 通道新歌起始寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  p0dsr    : 1;  /* bit[0]   : PCM0通道新歌起始。
                                                   0：无效；
                                                   1：新歌起始。 */
        unsigned int  p1dsr    : 1;  /* bit[1]   : PCM1通道新歌起始。
                                                   0：无效；
                                                   1：新歌起始。 */
        unsigned int  p2dsr    : 1;  /* bit[2]   : PCM2通道新歌起始。
                                                   0：无效；
                                                   1：新歌起始。 */
        unsigned int  p3dsr    : 1;  /* bit[3]   : PCM3通道新歌起始。
                                                   0：无效；
                                                   1：新歌起始。 */
        unsigned int  reserved : 28; /* bit[4-31]: 保留。 */
    } reg;
} SOC_ASP_PCMNSSR_UNION;
#endif
#define SOC_ASP_PCMNSSR_p0dsr_START     (0)
#define SOC_ASP_PCMNSSR_p0dsr_END       (0)
#define SOC_ASP_PCMNSSR_p1dsr_START     (1)
#define SOC_ASP_PCMNSSR_p1dsr_END       (1)
#define SOC_ASP_PCMNSSR_p2dsr_START     (2)
#define SOC_ASP_PCMNSSR_p2dsr_END       (2)
#define SOC_ASP_PCMNSSR_p3dsr_START     (3)
#define SOC_ASP_PCMNSSR_p3dsr_END       (3)


/*****************************************************************************
 结构名    : SOC_ASP_P0RSRR_UNION
 结构说明  : P0RSRR 寄存器结构定义。地址偏移量:0x034，初值:0x00000000，宽度:32
 寄存器说明: TX0重采样寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  p0rsr    : 4;  /* bit[0-3] : PCM0通道重采样频率选择。
                                                   0000－8kHz；
                                                   0001－11.025kHz；
                                                   0010－12kHz；
                                                   0011－16kHz；
                                                   0100－22.05kHz；
                                                   0101－24kHz；
                                                   0110－32kHz；
                                                   0111－44.1kHz；
                                                   1000-48KHz；
                                                   1001-88.2kHz；
                                                   1010 or others－96kHz。 */
        unsigned int  reserved : 28; /* bit[4-31]: 保留。 */
    } reg;
} SOC_ASP_P0RSRR_UNION;
#endif
#define SOC_ASP_P0RSRR_p0rsr_START     (0)
#define SOC_ASP_P0RSRR_p0rsr_END       (3)


/*****************************************************************************
 结构名    : SOC_ASP_P1RSRR_UNION
 结构说明  : P1RSRR 寄存器结构定义。地址偏移量:0x038，初值:0x00000000，宽度:32
 寄存器说明: TX1重采样寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  p1rsr    : 4;  /* bit[0-3] : PCM1通道重采样频率选择。
                                                   0000－8kHz；
                                                   0001－11.025kHz；
                                                   0010－12kHz；
                                                   0011－16kHz；
                                                   0100－22.05kHz；
                                                   0101－24kHz；
                                                   0110－32kHz；
                                                   0111－44.1kHz；
                                                   1000-48KHz；
                                                   1001-88.2kHz；
                                                   1010-96kHz；
                                                   1011-176.4kHz；
                                                   1100 or others－192kHz。 */
        unsigned int  reserved : 28; /* bit[4-31]: 保留。 */
    } reg;
} SOC_ASP_P1RSRR_UNION;
#endif
#define SOC_ASP_P1RSRR_p1rsr_START     (0)
#define SOC_ASP_P1RSRR_p1rsr_END       (3)


/*****************************************************************************
 结构名    : SOC_ASP_P2RSRR_UNION
 结构说明  : P2RSRR 寄存器结构定义。地址偏移量:0x03C，初值:0x00000000，宽度:32
 寄存器说明: TX2重采样寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  p2rsr    : 4;  /* bit[0-3] : PCM2通道重采样频率选择。
                                                   0000－8kHz；
                                                   0001－11.025kHz；
                                                   0010－12kHz；
                                                   0011－16kHz；
                                                   0100－22.05kHz；
                                                   0101－24kHz；
                                                   0110－32kHz；
                                                   0111－44.1kHz；
                                                   1000-48KHz；
                                                   1001-88.2kHz；
                                                   1010-96kHz；
                                                   1011-176.4kHz；
                                                   1100 or others－192kHz。 */
        unsigned int  reserved : 28; /* bit[4-31]: 保留。 */
    } reg;
} SOC_ASP_P2RSRR_UNION;
#endif
#define SOC_ASP_P2RSRR_p2rsr_START     (0)
#define SOC_ASP_P2RSRR_p2rsr_END       (3)


/*****************************************************************************
 结构名    : SOC_ASP_P3RSRR_UNION
 结构说明  : P3RSRR 寄存器结构定义。地址偏移量:0x040，初值:0x00000000，宽度:32
 寄存器说明: TX3重采样寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  p3rsr    : 4;  /* bit[0-3] : PCM3通道重采样频率选择。
                                                   0000－8kHz；
                                                   0001－11.025kHz；
                                                   0010－12kHz；
                                                   0011－16kHz；
                                                   0100－22.05kHz；
                                                   0101－24kHz；
                                                   0110－32kHz；
                                                   0111－44.1kHz；
                                                   1000-48KHz；
                                                   1001-88.2kHz；
                                                   1010-96kHz；
                                                   1011-176.4kHz；
                                                   1100 or others－192kHz。 */
        unsigned int  reserved : 28; /* bit[4-31]: 保留。 */
    } reg;
} SOC_ASP_P3RSRR_UNION;
#endif
#define SOC_ASP_P3RSRR_p3rsr_START     (0)
#define SOC_ASP_P3RSRR_p3rsr_END       (3)


/*****************************************************************************
 结构名    : SOC_ASP_FADEINEN0_UNION
 结构说明  : FADEINEN0 寄存器结构定义。地址偏移量:0x044，初值:0x00000000，宽度:32
 寄存器说明: TX0淡入使能寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  tx0_fadeien : 1;  /* bit[0]   : TX0通道淡入使能。
                                                      0：不使能；
                                                      1：使能。
                                                      注意：该寄存器软件写“1”，硬件清零，软件写“0”无效。 */
        unsigned int  reserved    : 31; /* bit[1-31]: 保留。 */
    } reg;
} SOC_ASP_FADEINEN0_UNION;
#endif
#define SOC_ASP_FADEINEN0_tx0_fadeien_START  (0)
#define SOC_ASP_FADEINEN0_tx0_fadeien_END    (0)


/*****************************************************************************
 结构名    : SOC_ASP_FADEOUTEN0_UNION
 结构说明  : FADEOUTEN0 寄存器结构定义。地址偏移量:0x048，初值:0x00000000，宽度:32
 寄存器说明: TX0淡出使能寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  tx0_fadeoen : 1;  /* bit[0]   : TX0通道淡出使能。
                                                      0：不使能；
                                                      1：使能。
                                                      注意：该寄存器软件写“1”，硬件清零，软件写“0”无效。 */
        unsigned int  reserved    : 31; /* bit[1-31]: 保留。 */
    } reg;
} SOC_ASP_FADEOUTEN0_UNION;
#endif
#define SOC_ASP_FADEOUTEN0_tx0_fadeoen_START  (0)
#define SOC_ASP_FADEOUTEN0_tx0_fadeoen_END    (0)


/*****************************************************************************
 结构名    : SOC_ASP_FADERATE0_UNION
 结构说明  : FADERATE0 寄存器结构定义。地址偏移量:0x04C，初值:0x00000000，宽度:32
 寄存器说明: TX0淡入淡出速率。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  tx0_faderate : 16; /* bit[0-15] : TX0淡入淡出速率。表示多少个音频数据样点降低或升高1个dB，实际值为配置值+1。 */
        unsigned int  reserved     : 16; /* bit[16-31]: 保留。 */
    } reg;
} SOC_ASP_FADERATE0_UNION;
#endif
#define SOC_ASP_FADERATE0_tx0_faderate_START  (0)
#define SOC_ASP_FADERATE0_tx0_faderate_END    (15)


/*****************************************************************************
 结构名    : SOC_ASP_FADEINEN1_UNION
 结构说明  : FADEINEN1 寄存器结构定义。地址偏移量:0x050，初值:0x00000000，宽度:32
 寄存器说明: TX1淡入使能寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  tx1_fadeien : 1;  /* bit[0]   : TX1通道淡入使能。
                                                      0：不使能；
                                                      1：使能。
                                                      注意：该寄存器软件写“1”，硬件清零，软件写“0”无效。 */
        unsigned int  reserved    : 31; /* bit[1-31]: 保留。 */
    } reg;
} SOC_ASP_FADEINEN1_UNION;
#endif
#define SOC_ASP_FADEINEN1_tx1_fadeien_START  (0)
#define SOC_ASP_FADEINEN1_tx1_fadeien_END    (0)


/*****************************************************************************
 结构名    : SOC_ASP_FADEOUTEN1_UNION
 结构说明  : FADEOUTEN1 寄存器结构定义。地址偏移量:0x054，初值:0x00000000，宽度:32
 寄存器说明: TX1淡出使能寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  tx1_fadeoen : 1;  /* bit[0]   : TX1通道淡出使能。
                                                      0：不使能；
                                                      1：使能。
                                                      注意：该寄存器软件写“1”，硬件清零，软件写“0”无效。 */
        unsigned int  reserved    : 31; /* bit[1-31]: 保留。 */
    } reg;
} SOC_ASP_FADEOUTEN1_UNION;
#endif
#define SOC_ASP_FADEOUTEN1_tx1_fadeoen_START  (0)
#define SOC_ASP_FADEOUTEN1_tx1_fadeoen_END    (0)


/*****************************************************************************
 结构名    : SOC_ASP_FADERATE1_UNION
 结构说明  : FADERATE1 寄存器结构定义。地址偏移量:0x058，初值:0x00000000，宽度:32
 寄存器说明: TX1淡入淡出速率。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  tx1_faderate : 16; /* bit[0-15] : TX1淡入淡出速率。表示多少个音频数据样点降低或升高1个dB，实际值为配置值+1。 */
        unsigned int  reserved     : 16; /* bit[16-31]: 保留。 */
    } reg;
} SOC_ASP_FADERATE1_UNION;
#endif
#define SOC_ASP_FADERATE1_tx1_faderate_START  (0)
#define SOC_ASP_FADERATE1_tx1_faderate_END    (15)


/*****************************************************************************
 结构名    : SOC_ASP_FADEINEN2_UNION
 结构说明  : FADEINEN2 寄存器结构定义。地址偏移量:0x05C，初值:0x00000000，宽度:32
 寄存器说明: TX2淡入使能寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  tx2_fadeien : 1;  /* bit[0]   : TX2通道淡入使能。
                                                      0：不使能；
                                                      1：使能。
                                                      注意：该寄存器软件写“1”，硬件清零，软件写“0”无效。 */
        unsigned int  reserved    : 31; /* bit[1-31]: 保留。 */
    } reg;
} SOC_ASP_FADEINEN2_UNION;
#endif
#define SOC_ASP_FADEINEN2_tx2_fadeien_START  (0)
#define SOC_ASP_FADEINEN2_tx2_fadeien_END    (0)


/*****************************************************************************
 结构名    : SOC_ASP_FADEOUTEN2_UNION
 结构说明  : FADEOUTEN2 寄存器结构定义。地址偏移量:0x060，初值:0x00000000，宽度:32
 寄存器说明: TX2淡出使能寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  tx2_fadeoen : 1;  /* bit[0]   : TX2通道淡出使能。
                                                      0：不使能；
                                                      1：使能。
                                                      注意：该寄存器软件写“1”，硬件清零，软件写“0”无效。 */
        unsigned int  reserved    : 31; /* bit[1-31]: 保留。 */
    } reg;
} SOC_ASP_FADEOUTEN2_UNION;
#endif
#define SOC_ASP_FADEOUTEN2_tx2_fadeoen_START  (0)
#define SOC_ASP_FADEOUTEN2_tx2_fadeoen_END    (0)


/*****************************************************************************
 结构名    : SOC_ASP_FADERATE2_UNION
 结构说明  : FADERATE2 寄存器结构定义。地址偏移量:0x064，初值:0x00000000，宽度:32
 寄存器说明: TX2淡入淡出速率。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  tx2_faderate : 16; /* bit[0-15] : TX2淡入淡出速率。表示多少个音频数据样点降低或升高1个dB，实际值为配置值+1。 */
        unsigned int  reserved     : 16; /* bit[16-31]: 保留。 */
    } reg;
} SOC_ASP_FADERATE2_UNION;
#endif
#define SOC_ASP_FADERATE2_tx2_faderate_START  (0)
#define SOC_ASP_FADERATE2_tx2_faderate_END    (15)


/*****************************************************************************
 结构名    : SOC_ASP_FADEINEN3_UNION
 结构说明  : FADEINEN3 寄存器结构定义。地址偏移量:0x068，初值:0x00000000，宽度:32
 寄存器说明: TX3淡入使能寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  tx3_fadeien : 1;  /* bit[0]   : TX3通道淡入使能。
                                                      0：不使能；
                                                      1：使能。
                                                      注意：该寄存器软件写“1”，硬件清零，软件写“0”无效。 */
        unsigned int  reserved    : 31; /* bit[1-31]: 保留。 */
    } reg;
} SOC_ASP_FADEINEN3_UNION;
#endif
#define SOC_ASP_FADEINEN3_tx3_fadeien_START  (0)
#define SOC_ASP_FADEINEN3_tx3_fadeien_END    (0)


/*****************************************************************************
 结构名    : SOC_ASP_FADEOUTEN3_UNION
 结构说明  : FADEOUTEN3 寄存器结构定义。地址偏移量:0x06C，初值:0x00000000，宽度:32
 寄存器说明: TX3淡出使能寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  tx3_fadeoen : 1;  /* bit[0]   : TX3通道淡出使能。
                                                      0：不使能；
                                                      1：使能。
                                                      注意：该寄存器软件写“1”，硬件清零，软件写“0”无效。 */
        unsigned int  reserved    : 31; /* bit[1-31]: 保留。 */
    } reg;
} SOC_ASP_FADEOUTEN3_UNION;
#endif
#define SOC_ASP_FADEOUTEN3_tx3_fadeoen_START  (0)
#define SOC_ASP_FADEOUTEN3_tx3_fadeoen_END    (0)


/*****************************************************************************
 结构名    : SOC_ASP_FADERATE3_UNION
 结构说明  : FADERATE3 寄存器结构定义。地址偏移量:0x070，初值:0x00000000，宽度:32
 寄存器说明: TX3淡入淡出速率。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  tx3_faderate : 16; /* bit[0-15] : TX3淡入淡出速率。表示多少个音频数据样点降低或升高1个dB，实际值为配置值+1。 */
        unsigned int  reserved     : 16; /* bit[16-31]: 保留。 */
    } reg;
} SOC_ASP_FADERATE3_UNION;
#endif
#define SOC_ASP_FADERATE3_tx3_faderate_START  (0)
#define SOC_ASP_FADERATE3_tx3_faderate_END    (15)


/*****************************************************************************
 结构名    : SOC_ASP_P0LCGR_UNION
 结构说明  : P0LCGR 寄存器结构定义。地址偏移量:0x074，初值:0x00000000，宽度:32
 寄存器说明: PCM0通道左声道增益寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  p0lcgr   : 16; /* bit[0-15] : PCM0通道左声道增益大小设置。 */
        unsigned int  reserved : 16; /* bit[16-31]: 保留。 */
    } reg;
} SOC_ASP_P0LCGR_UNION;
#endif
#define SOC_ASP_P0LCGR_p0lcgr_START    (0)
#define SOC_ASP_P0LCGR_p0lcgr_END      (15)


/*****************************************************************************
 结构名    : SOC_ASP_P0RCGR_UNION
 结构说明  : P0RCGR 寄存器结构定义。地址偏移量:0x078，初值:0x00000000，宽度:32
 寄存器说明: PCM0通道右声道增益寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  p0rcgr   : 16; /* bit[0-15] : PCM0通道右声道增益大小设置。 */
        unsigned int  reserved : 16; /* bit[16-31]: 保留。 */
    } reg;
} SOC_ASP_P0RCGR_UNION;
#endif
#define SOC_ASP_P0RCGR_p0rcgr_START    (0)
#define SOC_ASP_P0RCGR_p0rcgr_END      (15)


/*****************************************************************************
 结构名    : SOC_ASP_P1LCGR_UNION
 结构说明  : P1LCGR 寄存器结构定义。地址偏移量:0x07C，初值:0x00000000，宽度:32
 寄存器说明: PCM1通道左声道增益寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  p1lcgr   : 16; /* bit[0-15] : PCM1通道左声道增益大小设置。 */
        unsigned int  reserved : 16; /* bit[16-31]: 保留。 */
    } reg;
} SOC_ASP_P1LCGR_UNION;
#endif
#define SOC_ASP_P1LCGR_p1lcgr_START    (0)
#define SOC_ASP_P1LCGR_p1lcgr_END      (15)


/*****************************************************************************
 结构名    : SOC_ASP_P1RCGR_UNION
 结构说明  : P1RCGR 寄存器结构定义。地址偏移量:0x080，初值:0x00000000，宽度:32
 寄存器说明: PCM1通道右声道增益寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  p1rcgr   : 16; /* bit[0-15] : PCM1通道右声道增益大小设置。 */
        unsigned int  reserved : 16; /* bit[16-31]: 保留。 */
    } reg;
} SOC_ASP_P1RCGR_UNION;
#endif
#define SOC_ASP_P1RCGR_p1rcgr_START    (0)
#define SOC_ASP_P1RCGR_p1rcgr_END      (15)


/*****************************************************************************
 结构名    : SOC_ASP_P2LCGR_UNION
 结构说明  : P2LCGR 寄存器结构定义。地址偏移量:0x084，初值:0x00000000，宽度:32
 寄存器说明: PCM2通道左声道增益寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  p2lcgr   : 16; /* bit[0-15] : PCM2通道左声道增益大小设置。 */
        unsigned int  reserved : 16; /* bit[16-31]: 保留。 */
    } reg;
} SOC_ASP_P2LCGR_UNION;
#endif
#define SOC_ASP_P2LCGR_p2lcgr_START    (0)
#define SOC_ASP_P2LCGR_p2lcgr_END      (15)


/*****************************************************************************
 结构名    : SOC_ASP_P2RCGR_UNION
 结构说明  : P2RCGR 寄存器结构定义。地址偏移量:0x088，初值:0x00000000，宽度:32
 寄存器说明: PCM2通道右声道增益寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  p2rcgr   : 16; /* bit[0-15] : PCM2通道右声道增益大小设置。 */
        unsigned int  reserved : 16; /* bit[16-31]: 保留。 */
    } reg;
} SOC_ASP_P2RCGR_UNION;
#endif
#define SOC_ASP_P2RCGR_p2rcgr_START    (0)
#define SOC_ASP_P2RCGR_p2rcgr_END      (15)


/*****************************************************************************
 结构名    : SOC_ASP_P30CGR_UNION
 结构说明  : P30CGR 寄存器结构定义。地址偏移量:0x08C，初值:0x00000000，宽度:32
 寄存器说明: PCM3通道0声道增益寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  p30cgr   : 16; /* bit[0-15] : PCM3通道0声道增益大小设置。 */
        unsigned int  reserved : 16; /* bit[16-31]: 保留。 */
    } reg;
} SOC_ASP_P30CGR_UNION;
#endif
#define SOC_ASP_P30CGR_p30cgr_START    (0)
#define SOC_ASP_P30CGR_p30cgr_END      (15)


/*****************************************************************************
 结构名    : SOC_ASP_P31CGR_UNION
 结构说明  : P31CGR 寄存器结构定义。地址偏移量:0x090，初值:0x00000000，宽度:32
 寄存器说明: PCM3通道1声道增益寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  p31cgr   : 16; /* bit[0-15] : PCM3通道1声道增益大小设置。 */
        unsigned int  reserved : 16; /* bit[16-31]: 保留。 */
    } reg;
} SOC_ASP_P31CGR_UNION;
#endif
#define SOC_ASP_P31CGR_p31cgr_START    (0)
#define SOC_ASP_P31CGR_p31cgr_END      (15)


/*****************************************************************************
 结构名    : SOC_ASP_P32CGR_UNION
 结构说明  : P32CGR 寄存器结构定义。地址偏移量:0x094，初值:0x00000000，宽度:32
 寄存器说明: PCM3通道2声道增益寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  p32cgr   : 16; /* bit[0-15] : PCM3通道2声道增益大小设置。 */
        unsigned int  reserved : 16; /* bit[16-31]: 保留。 */
    } reg;
} SOC_ASP_P32CGR_UNION;
#endif
#define SOC_ASP_P32CGR_p32cgr_START    (0)
#define SOC_ASP_P32CGR_p32cgr_END      (15)


/*****************************************************************************
 结构名    : SOC_ASP_P33CGR_UNION
 结构说明  : P33CGR 寄存器结构定义。地址偏移量:0x098，初值:0x00000000，宽度:32
 寄存器说明: PCM3通道3声道增益寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  p33cgr   : 16; /* bit[0-15] : PCM3通道3声道增益大小设置。 */
        unsigned int  reserved : 16; /* bit[16-31]: 保留。 */
    } reg;
} SOC_ASP_P33CGR_UNION;
#endif
#define SOC_ASP_P33CGR_p33cgr_START    (0)
#define SOC_ASP_P33CGR_p33cgr_END      (15)


/*****************************************************************************
 结构名    : SOC_ASP_P34CGR_UNION
 结构说明  : P34CGR 寄存器结构定义。地址偏移量:0x09C，初值:0x00000000，宽度:32
 寄存器说明: PCM3通道4声道增益寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  p34cgr   : 16; /* bit[0-15] : PCM3通道4声道增益大小设置。 */
        unsigned int  reserved : 16; /* bit[16-31]: 保留。 */
    } reg;
} SOC_ASP_P34CGR_UNION;
#endif
#define SOC_ASP_P34CGR_p34cgr_START    (0)
#define SOC_ASP_P34CGR_p34cgr_END      (15)


/*****************************************************************************
 结构名    : SOC_ASP_P35CGR_UNION
 结构说明  : P35CGR 寄存器结构定义。地址偏移量:0x0A0，初值:0x00000000，宽度:32
 寄存器说明: PCM3通道5声道增益寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  p35cgr   : 16; /* bit[0-15] : PCM3通道5声道增益大小设置。 */
        unsigned int  reserved : 16; /* bit[16-31]: 保留。 */
    } reg;
} SOC_ASP_P35CGR_UNION;
#endif
#define SOC_ASP_P35CGR_p35cgr_START    (0)
#define SOC_ASP_P35CGR_p35cgr_END      (15)


/*****************************************************************************
 结构名    : SOC_ASP_P36CGR_UNION
 结构说明  : P36CGR 寄存器结构定义。地址偏移量:0x0A4，初值:0x00000000，宽度:32
 寄存器说明: PCM3通道6声道增益寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  p36cgr   : 16; /* bit[0-15] : PCM3通道6声道增益大小设置。 */
        unsigned int  reserved : 16; /* bit[16-31]: 保留。 */
    } reg;
} SOC_ASP_P36CGR_UNION;
#endif
#define SOC_ASP_P36CGR_p36cgr_START    (0)
#define SOC_ASP_P36CGR_p36cgr_END      (15)


/*****************************************************************************
 结构名    : SOC_ASP_P37CGR_UNION
 结构说明  : P37CGR 寄存器结构定义。地址偏移量:0x0A8，初值:0x00000000，宽度:32
 寄存器说明: PCM3通道7声道增益寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  p37cgr   : 16; /* bit[0-15] : PCM3通道7声道增益大小设置。 */
        unsigned int  reserved : 16; /* bit[16-31]: 保留。 */
    } reg;
} SOC_ASP_P37CGR_UNION;
#endif
#define SOC_ASP_P37CGR_p37cgr_START    (0)
#define SOC_ASP_P37CGR_p37cgr_END      (15)


/*****************************************************************************
 结构名    : SOC_ASP_MIXCTL_UNION
 结构说明  : MIXCTL 寄存器结构定义。地址偏移量:0x0AC，初值:0x00000000，宽度:32
 寄存器说明: 混音配置寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  mix_ctrl : 2;  /* bit[0-1] : TX3与TX1/TX2混音控制寄存器。
                                                   00：TX3中声道0，声道1与TX1/TX2的左声道，右声道混音；
                                                   01：TX3中声道2，声道3与TX1/TX2的左声道，右声道混音；
                                                   10：TX3中声道4，声道5与TX1/TX2的左声道，右声道混音；
                                                   11：TX3中声道6，声道7与TX1/TX2的左声道，右声道混音。 */
        unsigned int  reserved : 30; /* bit[2-31]: 保留。 */
    } reg;
} SOC_ASP_MIXCTL_UNION;
#endif
#define SOC_ASP_MIXCTL_mix_ctrl_START  (0)
#define SOC_ASP_MIXCTL_mix_ctrl_END    (1)


/*****************************************************************************
 结构名    : SOC_ASP_PRASAR_UNION
 结构说明  : PRASAR 寄存器结构定义。地址偏移量:0x0B0，初值:0x00000000，宽度:32
 寄存器说明: PCMRXA通道搬运起始地址寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  prasar : 32; /* bit[0-31]: PCMRXA通道DMA起始地址。
                                                 注意：bit[1:0]必须配置为2'b0，推荐bit[3:0]配置为4'h0。 */
    } reg;
} SOC_ASP_PRASAR_UNION;
#endif
#define SOC_ASP_PRASAR_prasar_START  (0)
#define SOC_ASP_PRASAR_prasar_END    (31)


/*****************************************************************************
 结构名    : SOC_ASP_PRADLR_UNION
 结构说明  : PRADLR 寄存器结构定义。地址偏移量:0x0B4，初值:0x00000000，宽度:32
 寄存器说明: PCMRXA通道搬运长度寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  pradlr   : 20; /* bit[0-19] : PCMRXA通道A组DMA长度，以字节为单位。
                                                    注意：bit[2:0]必须配置为3'b0。 */
        unsigned int  reserved : 12; /* bit[20-31]: 保留。 */
    } reg;
} SOC_ASP_PRADLR_UNION;
#endif
#define SOC_ASP_PRADLR_pradlr_START    (0)
#define SOC_ASP_PRADLR_pradlr_END      (19)


/*****************************************************************************
 结构名    : SOC_ASP_PRBSAR_UNION
 结构说明  : PRBSAR 寄存器结构定义。地址偏移量:0x0B8，初值:0x00000000，宽度:32
 寄存器说明: PCMRXB通道搬运起始地址寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  prbsar : 32; /* bit[0-31]: PCMRXB通道DMA起始地址。
                                                 注意：bit[1:0]必须配置为2'b0，推荐bit[3:0]配置为4'h0。 */
    } reg;
} SOC_ASP_PRBSAR_UNION;
#endif
#define SOC_ASP_PRBSAR_prbsar_START  (0)
#define SOC_ASP_PRBSAR_prbsar_END    (31)


/*****************************************************************************
 结构名    : SOC_ASP_PRBDLR_UNION
 结构说明  : PRBDLR 寄存器结构定义。地址偏移量:0x0BC，初值:0x00000000，宽度:32
 寄存器说明: PCMRXB通道搬运长度寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  prbdlr   : 20; /* bit[0-19] : PCMX通道B组DMA长度，以字节为单位。
                                                    注意：bit[2:0]必须配置为3'b0。 */
        unsigned int  reserved : 12; /* bit[20-31]: 保留。 */
    } reg;
} SOC_ASP_PRBDLR_UNION;
#endif
#define SOC_ASP_PRBDLR_prbdlr_START    (0)
#define SOC_ASP_PRBDLR_prbdlr_END      (19)


/*****************************************************************************
 结构名    : SOC_ASP_P0ASAR_UNION
 结构说明  : P0ASAR 寄存器结构定义。地址偏移量:0x0C0，初值:0x00000000，宽度:32
 寄存器说明: PCM0A通道搬运起始地址寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  p0asar : 32; /* bit[0-31]: PCM0通道A组DMA起始地址。
                                                 注意：bit[1:0]必须配置为2'b0，推荐bit[3:0]配置为4'h0。 */
    } reg;
} SOC_ASP_P0ASAR_UNION;
#endif
#define SOC_ASP_P0ASAR_p0asar_START  (0)
#define SOC_ASP_P0ASAR_p0asar_END    (31)


/*****************************************************************************
 结构名    : SOC_ASP_P0ADLR_UNION
 结构说明  : P0ADLR 寄存器结构定义。地址偏移量:0x0C4，初值:0x00000000，宽度:32
 寄存器说明: PCM0A通道搬运长度寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  p0adlr   : 20; /* bit[0-19] : PCM0通道A组DMA长度，以字节为单位。
                                                    注意：bit[2:0]必须配置为3'b0。 */
        unsigned int  reserved : 12; /* bit[20-31]: 保留。 */
    } reg;
} SOC_ASP_P0ADLR_UNION;
#endif
#define SOC_ASP_P0ADLR_p0adlr_START    (0)
#define SOC_ASP_P0ADLR_p0adlr_END      (19)


/*****************************************************************************
 结构名    : SOC_ASP_P0BSAR_UNION
 结构说明  : P0BSAR 寄存器结构定义。地址偏移量:0x0C8，初值:0x00000000，宽度:32
 寄存器说明: PCM0B通道搬运起始地址寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  p0bsar : 32; /* bit[0-31]: PCM0通道B组DMA起始地址。
                                                 注意：bit[1:0]必须配置为2'b0，推荐bit[3:0]配置为4'h0。 */
    } reg;
} SOC_ASP_P0BSAR_UNION;
#endif
#define SOC_ASP_P0BSAR_p0bsar_START  (0)
#define SOC_ASP_P0BSAR_p0bsar_END    (31)


/*****************************************************************************
 结构名    : SOC_ASP_P0BDLR_UNION
 结构说明  : P0BDLR 寄存器结构定义。地址偏移量:0x0CC，初值:0x00000000，宽度:32
 寄存器说明: PCM0B通道搬运长度寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  p0bdlr   : 20; /* bit[0-19] : PCM1通道B组DMA长度，以字节为单位。
                                                    注意：bit[2:0]必须配置为3'b0。 */
        unsigned int  reserved : 12; /* bit[20-31]: 保留。 */
    } reg;
} SOC_ASP_P0BDLR_UNION;
#endif
#define SOC_ASP_P0BDLR_p0bdlr_START    (0)
#define SOC_ASP_P0BDLR_p0bdlr_END      (19)


/*****************************************************************************
 结构名    : SOC_ASP_P1ASAR_UNION
 结构说明  : P1ASAR 寄存器结构定义。地址偏移量:0x0D0，初值:0x00000000，宽度:32
 寄存器说明: PCM1A通道搬运起始地址寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  p1asar : 32; /* bit[0-31]: PCM1通道A组DMA起始地址。
                                                 注意：bit[1:0]必须配置为2'b0，推荐bit[3:0]配置为4'h0。 */
    } reg;
} SOC_ASP_P1ASAR_UNION;
#endif
#define SOC_ASP_P1ASAR_p1asar_START  (0)
#define SOC_ASP_P1ASAR_p1asar_END    (31)


/*****************************************************************************
 结构名    : SOC_ASP_P1ADLR_UNION
 结构说明  : P1ADLR 寄存器结构定义。地址偏移量:0x0D4，初值:0x00000000，宽度:32
 寄存器说明: PCM1A通道搬运长度寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  p1adlr   : 20; /* bit[0-19] : PCM1通道A组DMA长度，以字节为单位。
                                                    注意：bit[2:0]必须配置为3'b0。 */
        unsigned int  reserved : 12; /* bit[20-31]: 保留。 */
    } reg;
} SOC_ASP_P1ADLR_UNION;
#endif
#define SOC_ASP_P1ADLR_p1adlr_START    (0)
#define SOC_ASP_P1ADLR_p1adlr_END      (19)


/*****************************************************************************
 结构名    : SOC_ASP_P1BSAR_UNION
 结构说明  : P1BSAR 寄存器结构定义。地址偏移量:0x0D8，初值:0x00000000，宽度:32
 寄存器说明: PCM1B通道搬运起始地址寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  p1bsar : 32; /* bit[0-31]: PCM1通道B组DMA起始地址。
                                                 注意：bit[1:0]必须配置为2'b0，推荐bit[3:0]配置为4'h0。 */
    } reg;
} SOC_ASP_P1BSAR_UNION;
#endif
#define SOC_ASP_P1BSAR_p1bsar_START  (0)
#define SOC_ASP_P1BSAR_p1bsar_END    (31)


/*****************************************************************************
 结构名    : SOC_ASP_P1BDLR_UNION
 结构说明  : P1BDLR 寄存器结构定义。地址偏移量:0x0DC，初值:0x00000000，宽度:32
 寄存器说明: PCM1B通道搬运长度寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  p1bdlr   : 20; /* bit[0-19] : PCM1通道B组DMA长度，以字节为单位。
                                                    注意：bit[2:0]必须配置为3'b0。 */
        unsigned int  reserved : 12; /* bit[20-31]: 保留。 */
    } reg;
} SOC_ASP_P1BDLR_UNION;
#endif
#define SOC_ASP_P1BDLR_p1bdlr_START    (0)
#define SOC_ASP_P1BDLR_p1bdlr_END      (19)


/*****************************************************************************
 结构名    : SOC_ASP_P2ASAR_UNION
 结构说明  : P2ASAR 寄存器结构定义。地址偏移量:0x0E0，初值:0x00000000，宽度:32
 寄存器说明: PCM2A通道搬运起始地址寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  p2asar : 32; /* bit[0-31]: PCM2通道A组DMA起始地址。
                                                 注意：bit[1:0]必须配置为2'b0，推荐bit[3:0]配置为4'h0。 */
    } reg;
} SOC_ASP_P2ASAR_UNION;
#endif
#define SOC_ASP_P2ASAR_p2asar_START  (0)
#define SOC_ASP_P2ASAR_p2asar_END    (31)


/*****************************************************************************
 结构名    : SOC_ASP_P2ADLR_UNION
 结构说明  : P2ADLR 寄存器结构定义。地址偏移量:0x0E4，初值:0x00000000，宽度:32
 寄存器说明: PCM2A通道搬运长度寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  p2adlr   : 20; /* bit[0-19] : PCM2通道A组DMA长度，以字节为单位。
                                                    注意：bit[2:0]必须配置为3'b0。 */
        unsigned int  reserved : 12; /* bit[20-31]: 保留。 */
    } reg;
} SOC_ASP_P2ADLR_UNION;
#endif
#define SOC_ASP_P2ADLR_p2adlr_START    (0)
#define SOC_ASP_P2ADLR_p2adlr_END      (19)


/*****************************************************************************
 结构名    : SOC_ASP_P2BSAR_UNION
 结构说明  : P2BSAR 寄存器结构定义。地址偏移量:0x0E8，初值:0x00000000，宽度:32
 寄存器说明: PCM2B通道搬运起始地址寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  p2bsar : 32; /* bit[0-31]: PCM2通道B组DMA起始地址。
                                                 注意：bit[1:0]必须配置为2'b0，推荐bit[3:0]配置为4'h0。 */
    } reg;
} SOC_ASP_P2BSAR_UNION;
#endif
#define SOC_ASP_P2BSAR_p2bsar_START  (0)
#define SOC_ASP_P2BSAR_p2bsar_END    (31)


/*****************************************************************************
 结构名    : SOC_ASP_P2BDLR_UNION
 结构说明  : P2BDLR 寄存器结构定义。地址偏移量:0x0EC，初值:0x00000000，宽度:32
 寄存器说明: PCM2B通道搬运长度寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  p2bdlr   : 20; /* bit[0-19] : PCM2通道B组DMA长度，以字节为单位。
                                                    注意：bit[2:0]必须配置为3'b0。 */
        unsigned int  reserved : 12; /* bit[20-31]: 保留。 */
    } reg;
} SOC_ASP_P2BDLR_UNION;
#endif
#define SOC_ASP_P2BDLR_p2bdlr_START    (0)
#define SOC_ASP_P2BDLR_p2bdlr_END      (19)


/*****************************************************************************
 结构名    : SOC_ASP_P3ASAR_UNION
 结构说明  : P3ASAR 寄存器结构定义。地址偏移量:0x0F0，初值:0x00000000，宽度:32
 寄存器说明: PCM3A通道搬运起始地址寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  p3asar : 32; /* bit[0-31]: PCM3通道A组DMA起始地址。
                                                 注意：bit[1:0]必须配置为2'b0，推荐bit[3:0]配置为4'h0。 */
    } reg;
} SOC_ASP_P3ASAR_UNION;
#endif
#define SOC_ASP_P3ASAR_p3asar_START  (0)
#define SOC_ASP_P3ASAR_p3asar_END    (31)


/*****************************************************************************
 结构名    : SOC_ASP_P3ADLR_UNION
 结构说明  : P3ADLR 寄存器结构定义。地址偏移量:0x0F4，初值:0x00000000，宽度:32
 寄存器说明: PCM3A通道搬运长度寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  p3adlr   : 20; /* bit[0-19] : PCM3通道A组DMA长度，以字节为单位。
                                                    注意：bit[4:0]必须配置为5'b0。 */
        unsigned int  reserved : 12; /* bit[20-31]: 保留。 */
    } reg;
} SOC_ASP_P3ADLR_UNION;
#endif
#define SOC_ASP_P3ADLR_p3adlr_START    (0)
#define SOC_ASP_P3ADLR_p3adlr_END      (19)


/*****************************************************************************
 结构名    : SOC_ASP_P3BSAR_UNION
 结构说明  : P3BSAR 寄存器结构定义。地址偏移量:0x0F8，初值:0x00000000，宽度:32
 寄存器说明: PCM3B通道搬运起始地址寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  p3bsar : 32; /* bit[0-31]: PCM3通道B组DMA起始地址。
                                                 注意：bit[1:0]必须配置为2'b0，推荐bit[3:0]配置为4'h0。 */
    } reg;
} SOC_ASP_P3BSAR_UNION;
#endif
#define SOC_ASP_P3BSAR_p3bsar_START  (0)
#define SOC_ASP_P3BSAR_p3bsar_END    (31)


/*****************************************************************************
 结构名    : SOC_ASP_P3BDLR_UNION
 结构说明  : P3BDLR 寄存器结构定义。地址偏移量:0x0FC，初值:0x00000000，宽度:32
 寄存器说明: PCM3B通道搬运长度寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  p3bdlr   : 20; /* bit[0-19] : PCM3通道B组DMA长度，以字节为单位。
                                                    注意：bit[4:0]必须配置为5'b0。 */
        unsigned int  reserved : 12; /* bit[20-31]: 保留。 */
    } reg;
} SOC_ASP_P3BDLR_UNION;
#endif
#define SOC_ASP_P3BDLR_p3bdlr_START    (0)
#define SOC_ASP_P3BDLR_p3bdlr_END      (19)


/*****************************************************************************
 结构名    : SOC_ASP_SPDIFSEL_UNION
 结构说明  : SPDIFSEL 寄存器结构定义。地址偏移量:0x100，初值:0x00000000，宽度:32
 寄存器说明: 混音数据播放接口选择寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  spdifsel : 1;  /* bit[0]   : 混音播放接口选择。
                                                   0：HDMI接口；
                                                   1：SPDIF接口。 */
        unsigned int  reserved : 31; /* bit[1-31]: 保留。 */
    } reg;
} SOC_ASP_SPDIFSEL_UNION;
#endif
#define SOC_ASP_SPDIFSEL_spdifsel_START  (0)
#define SOC_ASP_SPDIFSEL_spdifsel_END    (0)


/*****************************************************************************
 结构名    : SOC_ASP_P0ADLS_UNION
 结构说明  : P0ADLS 寄存器结构定义。地址偏移量:0x104，初值:0x00000000，宽度:32
 寄存器说明: DMA停止时PCM0A通道搬运长度状态寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  p0adls   : 20; /* bit[0-19] : PCM0通道A组DMA停止使能时，剩余未完成搬运的DMA长度信息，单位为字节。 */
        unsigned int  reserved : 12; /* bit[20-31]: 保留。 */
    } reg;
} SOC_ASP_P0ADLS_UNION;
#endif
#define SOC_ASP_P0ADLS_p0adls_START    (0)
#define SOC_ASP_P0ADLS_p0adls_END      (19)


/*****************************************************************************
 结构名    : SOC_ASP_P0BDLS_UNION
 结构说明  : P0BDLS 寄存器结构定义。地址偏移量:0x108，初值:0x00000000，宽度:32
 寄存器说明: DMA停止时PCM0B通道搬运长度状态寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  p0bdls   : 20; /* bit[0-19] : PCM0通道B组DMA停止使能时，剩余未完成搬运的DMA长度信息，单位为字节。 */
        unsigned int  reserved : 12; /* bit[20-31]: 保留。 */
    } reg;
} SOC_ASP_P0BDLS_UNION;
#endif
#define SOC_ASP_P0BDLS_p0bdls_START    (0)
#define SOC_ASP_P0BDLS_p0bdls_END      (19)


/*****************************************************************************
 结构名    : SOC_ASP_P1ADLS_UNION
 结构说明  : P1ADLS 寄存器结构定义。地址偏移量:0x10C，初值:0x00000000，宽度:32
 寄存器说明: DMA停止时PCM1A通道搬运长度状态寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  p1adls   : 20; /* bit[0-19] : PCM1通道A组DMA停止使能时，剩余未完成搬运的DMA长度信息，单位为字节。 */
        unsigned int  reserved : 12; /* bit[20-31]: 保留。 */
    } reg;
} SOC_ASP_P1ADLS_UNION;
#endif
#define SOC_ASP_P1ADLS_p1adls_START    (0)
#define SOC_ASP_P1ADLS_p1adls_END      (19)


/*****************************************************************************
 结构名    : SOC_ASP_P1BDLS_UNION
 结构说明  : P1BDLS 寄存器结构定义。地址偏移量:0x110，初值:0x00000000，宽度:32
 寄存器说明: DMA停止时PCM1B通道搬运长度状态寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  p1bdls   : 20; /* bit[0-19] : PCM1通道B组DMA停止使能时，剩余未完成搬运的DMA长度信息，单位为字节。 */
        unsigned int  reserved : 12; /* bit[20-31]: 保留。 */
    } reg;
} SOC_ASP_P1BDLS_UNION;
#endif
#define SOC_ASP_P1BDLS_p1bdls_START    (0)
#define SOC_ASP_P1BDLS_p1bdls_END      (19)


/*****************************************************************************
 结构名    : SOC_ASP_P2ADLS_UNION
 结构说明  : P2ADLS 寄存器结构定义。地址偏移量:0x114，初值:0x00000000，宽度:32
 寄存器说明: DMA停止时PCM2A通道搬运长度状态寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  p2adls   : 20; /* bit[0-19] : PCM2通道A组DMA停止使能时，剩余未完成搬运的DMA长度信息，单位为字节。 */
        unsigned int  reserved : 12; /* bit[20-31]: 保留。 */
    } reg;
} SOC_ASP_P2ADLS_UNION;
#endif
#define SOC_ASP_P2ADLS_p2adls_START    (0)
#define SOC_ASP_P2ADLS_p2adls_END      (19)


/*****************************************************************************
 结构名    : SOC_ASP_P2BDLS_UNION
 结构说明  : P2BDLS 寄存器结构定义。地址偏移量:0x118，初值:0x00000000，宽度:32
 寄存器说明: DMA停止时PCM2B通道搬运长度状态寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  p2bdls   : 20; /* bit[0-19] : PCM2通道B组DMA停止使能时，剩余未完成搬运的DMA长度信息，单位为字节。 */
        unsigned int  reserved : 12; /* bit[20-31]: 保留。 */
    } reg;
} SOC_ASP_P2BDLS_UNION;
#endif
#define SOC_ASP_P2BDLS_p2bdls_START    (0)
#define SOC_ASP_P2BDLS_p2bdls_END      (19)


/*****************************************************************************
 结构名    : SOC_ASP_P3ADLS_UNION
 结构说明  : P3ADLS 寄存器结构定义。地址偏移量:0x11C，初值:0x00000000，宽度:32
 寄存器说明: DMA停止时PCM3A通道搬运长度状态寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  p3adls   : 20; /* bit[0-19] : PCM3通道A组DMA停止使能时，剩余未完成搬运的DMA长度信息，单位为字节。 */
        unsigned int  reserved : 12; /* bit[20-31]: 保留。 */
    } reg;
} SOC_ASP_P3ADLS_UNION;
#endif
#define SOC_ASP_P3ADLS_p3adls_START    (0)
#define SOC_ASP_P3ADLS_p3adls_END      (19)


/*****************************************************************************
 结构名    : SOC_ASP_P3BDLS_UNION
 结构说明  : P3BDLS 寄存器结构定义。地址偏移量:0x120，初值:0x00000000，宽度:32
 寄存器说明: DMA停止时PCM3B通道搬运长度状态寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  p3bdls   : 20; /* bit[0-19] : PCM3通道B组DMA停止使能时，剩余未完成搬运的DMA长度信息，单位为字节。 */
        unsigned int  reserved : 12; /* bit[20-31]: 保留。 */
    } reg;
} SOC_ASP_P3BDLS_UNION;
#endif
#define SOC_ASP_P3BDLS_p3bdls_START    (0)
#define SOC_ASP_P3BDLS_p3bdls_END      (19)


/*****************************************************************************
 结构名    : SOC_ASP_PRADLS_UNION
 结构说明  : PRADLS 寄存器结构定义。地址偏移量:0x124，初值:0x00000000，宽度:32
 寄存器说明: DMA停止时PCMRXA通道搬运长度状态寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  pradls   : 20; /* bit[0-19] : PCMRX通道A组DMA停止使能时，剩余未完成搬运的DMA长度信息，单位为字节。 */
        unsigned int  reserved : 12; /* bit[20-31]: 保留。 */
    } reg;
} SOC_ASP_PRADLS_UNION;
#endif
#define SOC_ASP_PRADLS_pradls_START    (0)
#define SOC_ASP_PRADLS_pradls_END      (19)


/*****************************************************************************
 结构名    : SOC_ASP_PRBDLS_UNION
 结构说明  : PRBDLS 寄存器结构定义。地址偏移量:0x128，初值:0x00000000，宽度:32
 寄存器说明: DMA停止时PCMRXB通道搬运长度状态寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  prbdls   : 20; /* bit[0-19] : PCMRX通道B组DMA停止使能时，剩余未完成搬运的DMA长度信息，单位为字节。 */
        unsigned int  reserved : 12; /* bit[20-31]: 保留。 */
    } reg;
} SOC_ASP_PRBDLS_UNION;
#endif
#define SOC_ASP_PRBDLS_prbdls_START    (0)
#define SOC_ASP_PRBDLS_prbdls_END      (19)


/*****************************************************************************
 结构名    : SOC_ASP_FDS_UNION
 结构说明  : FDS 寄存器结构定义。地址偏移量:0x12C，初值:0x00000000，宽度:32
 寄存器说明: DMA搬运数据缓存FIFO状态寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  pcm0_fds : 5;  /* bit[0-4]  : PCM0通道DMA搬运数据缓存FIFO深度状态信息。 */
        unsigned int  reserved_0: 3;  /* bit[5-7]  : 保留。 */
        unsigned int  pcm1_fds : 6;  /* bit[8-13] : PCM1通道DMA搬运数据缓存FIFO深度状态信息。 */
        unsigned int  reserved_1: 2;  /* bit[14-15]: 保留。 */
        unsigned int  pcm2_fds : 6;  /* bit[16-21]: PCM2通道DMA搬运数据缓存FIFO深度状态信息。 */
        unsigned int  reserved_2: 2;  /* bit[22-23]: 保留。 */
        unsigned int  pcm3_fds : 6;  /* bit[24-29]: PCM3通道DMA搬运数据缓存FIFO深度状态信息。 */
        unsigned int  reserved_3: 2;  /* bit[30-31]: 保留。 */
    } reg;
} SOC_ASP_FDS_UNION;
#endif
#define SOC_ASP_FDS_pcm0_fds_START  (0)
#define SOC_ASP_FDS_pcm0_fds_END    (4)
#define SOC_ASP_FDS_pcm1_fds_START  (8)
#define SOC_ASP_FDS_pcm1_fds_END    (13)
#define SOC_ASP_FDS_pcm2_fds_START  (16)
#define SOC_ASP_FDS_pcm2_fds_END    (21)
#define SOC_ASP_FDS_pcm3_fds_START  (24)
#define SOC_ASP_FDS_pcm3_fds_END    (29)


/*****************************************************************************
 结构名    : SOC_ASP_P0ADL_UNION
 结构说明  : P0ADL 寄存器结构定义。地址偏移量:0x130，初值:0x00000000，宽度:32
 寄存器说明: PCM0A通道DMA搬运剩余长度状态寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  p0adl    : 20; /* bit[0-19] : PCM0通道A组DMA剩余未搬运的DMA长度信息，单位为字节。 */
        unsigned int  reserved : 12; /* bit[20-31]: 保留。 */
    } reg;
} SOC_ASP_P0ADL_UNION;
#endif
#define SOC_ASP_P0ADL_p0adl_START     (0)
#define SOC_ASP_P0ADL_p0adl_END       (19)


/*****************************************************************************
 结构名    : SOC_ASP_P0BDL_UNION
 结构说明  : P0BDL 寄存器结构定义。地址偏移量:0x134，初值:0x00000000，宽度:32
 寄存器说明: PCM0B通道DMA搬运剩余长度状态寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  p0bdl    : 20; /* bit[0-19] : PCM0通道B组DMA剩余未搬运的DMA长度信息，单位为字节。 */
        unsigned int  reserved : 12; /* bit[20-31]: 保留。 */
    } reg;
} SOC_ASP_P0BDL_UNION;
#endif
#define SOC_ASP_P0BDL_p0bdl_START     (0)
#define SOC_ASP_P0BDL_p0bdl_END       (19)


/*****************************************************************************
 结构名    : SOC_ASP_P1ADL_UNION
 结构说明  : P1ADL 寄存器结构定义。地址偏移量:0x138，初值:0x00000000，宽度:32
 寄存器说明: PCM1A通道DMA搬运剩余长度状态寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  p1adl    : 20; /* bit[0-19] : PCM1通道A组DMA剩余未搬运的DMA长度信息，单位为字节。 */
        unsigned int  reserved : 12; /* bit[20-31]: 保留。 */
    } reg;
} SOC_ASP_P1ADL_UNION;
#endif
#define SOC_ASP_P1ADL_p1adl_START     (0)
#define SOC_ASP_P1ADL_p1adl_END       (19)


/*****************************************************************************
 结构名    : SOC_ASP_P1BDL_UNION
 结构说明  : P1BDL 寄存器结构定义。地址偏移量:0x13C，初值:0x00000000，宽度:32
 寄存器说明: PCM1B通道DMA搬运剩余长度状态寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  p1bdl    : 20; /* bit[0-19] : PCM1通道B组DMA剩余未搬运的DMA长度信息，单位为字节。 */
        unsigned int  reserved : 12; /* bit[20-31]: 保留。 */
    } reg;
} SOC_ASP_P1BDL_UNION;
#endif
#define SOC_ASP_P1BDL_p1bdl_START     (0)
#define SOC_ASP_P1BDL_p1bdl_END       (19)


/*****************************************************************************
 结构名    : SOC_ASP_P2ADL_UNION
 结构说明  : P2ADL 寄存器结构定义。地址偏移量:0x140，初值:0x00000000，宽度:32
 寄存器说明: PCM2A通道DMA搬运剩余长度状态寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  p2adl    : 20; /* bit[0-19] : PCM2通道A组DMA剩余未搬运的DMA长度信息，单位为字节。 */
        unsigned int  reserved : 12; /* bit[20-31]: 保留。 */
    } reg;
} SOC_ASP_P2ADL_UNION;
#endif
#define SOC_ASP_P2ADL_p2adl_START     (0)
#define SOC_ASP_P2ADL_p2adl_END       (19)


/*****************************************************************************
 结构名    : SOC_ASP_P2BDL_UNION
 结构说明  : P2BDL 寄存器结构定义。地址偏移量:0x144，初值:0x00000000，宽度:32
 寄存器说明: PCM2B通道DMA搬运剩余长度状态寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  p2bdl    : 20; /* bit[0-19] : PCM2通道B组DMA剩余未搬运的DMA长度信息，单位为字节。 */
        unsigned int  reserved : 12; /* bit[20-31]: 保留。 */
    } reg;
} SOC_ASP_P2BDL_UNION;
#endif
#define SOC_ASP_P2BDL_p2bdl_START     (0)
#define SOC_ASP_P2BDL_p2bdl_END       (19)


/*****************************************************************************
 结构名    : SOC_ASP_P3ADL_UNION
 结构说明  : P3ADL 寄存器结构定义。地址偏移量:0x148，初值:0x00000000，宽度:32
 寄存器说明: PCM3A通道DMA搬运剩余长度状态寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  p3adl    : 20; /* bit[0-19] : PCM3通道A组DMA剩余未搬运的DMA长度信息，单位为字节。 */
        unsigned int  reserved : 12; /* bit[20-31]: 保留。 */
    } reg;
} SOC_ASP_P3ADL_UNION;
#endif
#define SOC_ASP_P3ADL_p3adl_START     (0)
#define SOC_ASP_P3ADL_p3adl_END       (19)


/*****************************************************************************
 结构名    : SOC_ASP_P3BDL_UNION
 结构说明  : P3BDL 寄存器结构定义。地址偏移量:0x14C，初值:0x00000000，宽度:32
 寄存器说明: PCM3B通道DMA搬运剩余长度状态寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  p3bdl    : 20; /* bit[0-19] : PCM3通道B组DMA剩余未搬运的DMA长度信息，单位为字节。 */
        unsigned int  reserved : 12; /* bit[20-31]: 保留。 */
    } reg;
} SOC_ASP_P3BDL_UNION;
#endif
#define SOC_ASP_P3BDL_p3bdl_START     (0)
#define SOC_ASP_P3BDL_p3bdl_END       (19)


/*****************************************************************************
 结构名    : SOC_ASP_PRADL_UNION
 结构说明  : PRADL 寄存器结构定义。地址偏移量:0x150，初值:0x00000000，宽度:32
 寄存器说明: PCMRXA通道DMA搬运剩余长度状态寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  pradl    : 20; /* bit[0-19] : PCMRX通道A组DMA剩余未搬运的DMA长度信息，单位为字节。 */
        unsigned int  reserved : 12; /* bit[20-31]: 保留。 */
    } reg;
} SOC_ASP_PRADL_UNION;
#endif
#define SOC_ASP_PRADL_pradl_START     (0)
#define SOC_ASP_PRADL_pradl_END       (19)


/*****************************************************************************
 结构名    : SOC_ASP_PRBDL_UNION
 结构说明  : PRBDL 寄存器结构定义。地址偏移量:0x154，初值:0x00000000，宽度:32
 寄存器说明: PCMRXB通道DMA搬运剩余长度状态寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  prbdl    : 20; /* bit[0-19] : PCMRX通道B组DMA剩余未搬运的DMA长度信息，单位为字节。 */
        unsigned int  reserved : 12; /* bit[20-31]: 保留。 */
    } reg;
} SOC_ASP_PRBDL_UNION;
#endif
#define SOC_ASP_PRBDL_prbdl_START     (0)
#define SOC_ASP_PRBDL_prbdl_END       (19)






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

#endif /* end of soc_asp_interface.h */
