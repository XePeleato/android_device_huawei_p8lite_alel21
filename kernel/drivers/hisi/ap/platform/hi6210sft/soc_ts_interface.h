

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/

#ifndef __SOC_TS_INTERFACE_H__
#define __SOC_TS_INTERFACE_H__

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
/* 寄存器说明：接口寄存器。
   位域定义UNION结构:  SOC_TS_DMX_INF_UNION */
#define SOC_TS_DMX_INF_ADDR(base)                     ((base) + (0x000))

/* 寄存器说明：所有包计数寄存器。
   位域定义UNION结构:  SOC_TS_COUNT_TSP_UNION */
#define SOC_TS_COUNT_TSP_ADDR(base)                   ((base) + (0x004))

/* 寄存器说明：所有包计数器控制寄存器。
   位域定义UNION结构:  SOC_TS_COUNT_TSP_CTRL_UNION */
#define SOC_TS_COUNT_TSP_CTRL_ADDR(base)              ((base) + (0x008))

/* 寄存器说明：错误包计数寄存器。
   位域定义UNION结构:  SOC_TS_COUNT_ETSP_UNION */
#define SOC_TS_COUNT_ETSP_ADDR(base)                  ((base) + (0x00C))

/* 寄存器说明：错误包计数器控制寄存器。
   位域定义UNION结构:  SOC_TS_COUNT_ETSP_CTRL_UNION */
#define SOC_TS_COUNT_ETSP_CTRL_ADDR(base)             ((base) + (0x010))

/* 寄存器说明：接口异步FIFO发生错误标志寄存器。
   位域定义UNION结构:  SOC_TS_AFIFO_WFULL_ERR_UNION */
#define SOC_TS_AFIFO_WFULL_ERR_ADDR(base)             ((base) + (0x014))

/* 寄存器说明：接口异步FIFO满标志寄存器。
   位域定义UNION结构:  SOC_TS_AFIFO_WFULL_STATUS_UNION */
#define SOC_TS_AFIFO_WFULL_STATUS_ADDR(base)          ((base) + (0x018))

/* 寄存器说明：调试寄存器1（内部使用）。
   位域定义UNION结构:  SOC_TS_TSSYNC_DEBUG1_UNION */
#define SOC_TS_TSSYNC_DEBUG1_ADDR(base)               ((base) + (0x020))

/* 寄存器说明：调试寄存器2（内部使用）(out_188_info)。
   位域定义UNION结构:  SOC_TS_TSSYNC_DEBUG2_UNION */
#define SOC_TS_TSSYNC_DEBUG2_ADDR(base)               ((base) + (0x024))

/* 寄存器说明：调试寄存器3（内部使用）(out_204_info)。
   位域定义UNION结构:  SOC_TS_TSSYNC_DEBUG3_UNION */
#define SOC_TS_TSSYNC_DEBUG3_ADDR(base)               ((base) + (0x028))

/* 寄存器说明：读内部FIFO状态机的当前状态：调试寄存器（内部使用）。
   位域定义UNION结构:  SOC_TS_READ_FIFO_FSM_UNION */
#define SOC_TS_READ_FIFO_FSM_ADDR(base)               ((base) + (0x02C))

/* 寄存器说明：同步检测状态机的状态：调试寄存器（内部使用）。
   位域定义UNION结构:  SOC_TS_SYNC_CHECK_FSM_UNION */
#define SOC_TS_SYNC_CHECK_FSM_ADDR(base)              ((base) + (0x030))

/* 寄存器说明：同步成功标志寄存器。
   位域定义UNION结构:  SOC_TS_SYNC_CHECK_FLAG_UNION */
#define SOC_TS_SYNC_CHECK_FLAG_ADDR(base)             ((base) + (0x034))

/* 寄存器说明：内部memory存储状况寄存器：调试寄存器（内部使用）。
   位域定义UNION结构:  SOC_TS_SWITCH_FB_INFO_UNION */
#define SOC_TS_SWITCH_FB_INFO_ADDR(base)              ((base) + (0x038))

/* 寄存器说明：包标识符检测清除寄存器：debug时使用。
   位域定义UNION结构:  SOC_TS_DBG_PID_CTRL_UNION */
#define SOC_TS_DBG_PID_CTRL_ADDR(base)                ((base) + (0x040))

/* 寄存器说明：第0种包标识符检测寄存器：debug时使用。
   位域定义UNION结构:  SOC_TS_DBG_PID0_UNION */
#define SOC_TS_DBG_PID0_ADDR(base)                    ((base) + (0x044))

/* 寄存器说明：第1种包标识符检测寄存器：debug时使用。
   位域定义UNION结构:  SOC_TS_DBG_PID1_UNION */
#define SOC_TS_DBG_PID1_ADDR(base)                    ((base) + (0x048))

/* 寄存器说明：第2种包标识符检测寄存器：debug时使用。
   位域定义UNION结构:  SOC_TS_DBG_PID2_UNION */
#define SOC_TS_DBG_PID2_ADDR(base)                    ((base) + (0x04C))

/* 寄存器说明：第3种包标识符检测寄存器：debug时使用。
   位域定义UNION结构:  SOC_TS_DBG_PID3_UNION */
#define SOC_TS_DBG_PID3_ADDR(base)                    ((base) + (0x050))

/* 寄存器说明：第4种包标识符检测寄存器：debug时使用。
   位域定义UNION结构:  SOC_TS_DBG_PID4_UNION */
#define SOC_TS_DBG_PID4_ADDR(base)                    ((base) + (0x054))

/* 寄存器说明：第5种包标识符检测寄存器：debug时使用。
   位域定义UNION结构:  SOC_TS_DBG_PID5_UNION */
#define SOC_TS_DBG_PID5_ADDR(base)                    ((base) + (0x058))

/* 寄存器说明：第6种包标识符检测寄存器：debug时使用。
   位域定义UNION结构:  SOC_TS_DBG_PID6_UNION */
#define SOC_TS_DBG_PID6_ADDR(base)                    ((base) + (0x05C))

/* 寄存器说明：第7种包标识符检测寄存器：debug时使用。
   位域定义UNION结构:  SOC_TS_DBG_PID7_UNION */
#define SOC_TS_DBG_PID7_ADDR(base)                    ((base) + (0x060))

/* 寄存器说明：同步指定包长设置寄存器。
   位域定义UNION结构:  SOC_TS_SYNC_LEN_UNION */
#define SOC_TS_SYNC_LEN_ADDR(base)                    ((base) + (0x064))

/* 寄存器说明：串行nosync模式下同步检测过程中计数器（该计数器每256byte加1）清除门限值寄存器。
   位域定义UNION结构:  SOC_TS_SYNC_CLR_TH_UNION */
#define SOC_TS_SYNC_CLR_TH_ADDR(base)                 ((base) + (0x068))

/* 寄存器说明：已处理数据量计数触发信号选择寄存器：用于debug TS流速率，即检测一段时间内接收了多少数据量，数据量统计由cnt_sig[255:0]中被选择用于触发的信号触发。
   位域定义UNION结构:  SOC_TS_DBG_CNT_SEL_UNION */
#define SOC_TS_DBG_CNT_SEL_ADDR(base)                 ((base) + (0x06C))

/* 寄存器说明：已处理数据量计数器计数值采样时间间隔配置寄存器：用于debug TS流速率。
   位域定义UNION结构:  SOC_TS_DBG_TIMERCFG_UNION */
#define SOC_TS_DBG_TIMERCFG_ADDR(base)                ((base) + (0x070))

/* 寄存器说明：当前已处理数据量计数器：用于debug TS流速率。
   位域定义UNION结构:  SOC_TS_DBG_RD_CNT_UNION */
#define SOC_TS_DBG_RD_CNT_ADDR(base)                  ((base) + (0x074))

/* 寄存器说明：当前时间值寄存器：用于debug TS流速率。
   位域定义UNION结构:  SOC_TS_DBG_RD_TIMER_UNION */
#define SOC_TS_DBG_RD_TIMER_ADDR(base)                ((base) + (0x078))

/* 寄存器说明：已处理数据量计数器计数值采样所得数值：用于debug TS流速率。
   位域定义UNION结构:  SOC_TS_DBG_RD_SAMPLE_UNION */
#define SOC_TS_DBG_RD_SAMPLE_ADDR(base)               ((base) + (0x07C))

/* 寄存器说明：串行输入下同步失败指示及相关计数寄存器：用于debug。
   位域定义UNION结构:  SOC_TS_DBG_UNIFY_0_UNION */
#define SOC_TS_DBG_UNIFY_0_ADDR(base)                 ((base) + (0x080))

/* 寄存器说明：TS端口模式及相关信号寄存器：用于debug。
   位域定义UNION结构:  SOC_TS_DBG_UNIFY_1_UNION */
#define SOC_TS_DBG_UNIFY_1_ADDR(base)                 ((base) + (0x084))

/* 寄存器说明：TS原始中断寄存器。
   位域定义UNION结构:  SOC_TS_INTR_RAW_UNION */
#define SOC_TS_INTR_RAW_ADDR(base)                    ((base) + (0x100))

/* 寄存器说明：TS中断屏蔽设置寄存器。
   位域定义UNION结构:  SOC_TS_INTR_MASK_UNION */
#define SOC_TS_INTR_MASK_ADDR(base)                   ((base) + (0x104))

/* 寄存器说明：TS屏蔽后中断状态寄存器。
   位域定义UNION结构:  SOC_TS_INTR_STATUS_UNION */
#define SOC_TS_INTR_STATUS_ADDR(base)                 ((base) + (0x108))

/* 寄存器说明：TS中断清除寄存器。
   位域定义UNION结构:  SOC_TS_INTR_CLR_UNION */
#define SOC_TS_INTR_CLR_ADDR(base)                    ((base) + (0x10C))

/* 寄存器说明：TS端口时钟反向选择寄存器。
   位域定义UNION结构:  SOC_TS_CLK_INV_SEL_UNION */
#define SOC_TS_CLK_INV_SEL_ADDR(base)                 ((base) + (0x110))

/* 寄存器说明：数据搬出控制寄存器。
   位域定义UNION结构:  SOC_TS_DATA_MV_CTRL_UNION */
#define SOC_TS_DATA_MV_CTRL_ADDR(base)                ((base) + (0x114))

/* 寄存器说明：内部memory阈值设置寄存器。
   位域定义UNION结构:  SOC_TS_MEM_THRESHOLD_UNION */
#define SOC_TS_MEM_THRESHOLD_ADDR(base)               ((base) + (0x118))

/* 寄存器说明：内部memory中数据量大小显示寄存器。
   位域定义UNION结构:  SOC_TS_MEM_COUNT_UNION */
#define SOC_TS_MEM_COUNT_ADDR(base)                   ((base) + (0x11C))

/* 寄存器说明：最后一次搬运无效数据量，以10bit为单位。
   位域定义UNION结构:  SOC_TS_LAST_INVALID_COUNT_UNION */
#define SOC_TS_LAST_INVALID_COUNT_ADDR(base)          ((base) + (0x120))

/* 寄存器说明：TS端口关闭后到允许时钟门控的主时钟周期个数配置寄存器。
   位域定义UNION结构:  SOC_TS_CLK_GT_TIMEOUT_VALUE_UNION */
#define SOC_TS_CLK_GT_TIMEOUT_VALUE_ADDR(base)        ((base) + (0x124))

/* 寄存器说明：最后一次搬运请求控制寄存器。
   位域定义UNION结构:  SOC_TS_LAST_REQ_CTRL_UNION */
#define SOC_TS_LAST_REQ_CTRL_ADDR(base)               ((base) + (0x128))

/* 寄存器说明：最后一次搬运所填充无效数据值配置寄存器。
   位域定义UNION结构:  SOC_TS_LAST_INVALID_DATA_UNION */
#define SOC_TS_LAST_INVALID_DATA_ADDR(base)           ((base) + (0x12C))

/* 寄存器说明：内部memory数据清除寄存器。
   位域定义UNION结构:  SOC_TS_MEM_DISABLE_UNION */
#define SOC_TS_MEM_DISABLE_ADDR(base)                 ((base) + (0x130))





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
 结构名    : SOC_TS_DMX_INF_UNION
 结构说明  : DMX_INF 寄存器结构定义。地址偏移量:0x000，初值:0x00100715，宽度:32
 寄存器说明: 接口寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sw_build_sync_188_th   : 3;  /* bit[0-2]  : 同步锁定判断的门限值。在nosync模式下有效。 */
        unsigned int  reserved_0             : 1;  /* bit[3]    : 保留。 */
        unsigned int  sw_lose_sync_188_th    : 2;  /* bit[4-5]  : 同步丢失判断的门限值。在nosync模式下有效。 */
        unsigned int  reserved_1             : 2;  /* bit[6-7]  : 保留。 */
        unsigned int  sw_afifo_rd_rate       : 3;  /* bit[8-10] : 软件配置的读异步FIFO的rd_rate值。 */
        unsigned int  sw_afifo_rd_mode       : 1;  /* bit[11]   : 软件配置TS IP内部异步FIFO读取模式选择：
                                                                  0：按照硬件固定rd_rate读异步FIFO；
                                                                  1：按照软件配置的sw_afifo_rd_rate读异步FIFO。 */
        unsigned int  sw_serial_2bit         : 1;  /* bit[12]   : 串行方式选择：
                                                                  0：1bit串行方式；
                                                                  1：2bit串行方式。 */
        unsigned int  sw_serial_2bit_rev     : 1;  /* bit[13]   : 2bit串行方式bit线反转选择：
                                                                  0：2bit数据线不反转；
                                                                  1：2bit数据线反转。
                                                                  只在serial_sel为1且ser_2bit_mode为1时有效。 */
        unsigned int  sw_serial_nosync       : 1;  /* bit[14]   : 串行nosync模式选择：
                                                                  0：sync线有效；
                                                                  1：sync线无效，自适应同步。
                                                                  只在serial_sel为1且ser_2bit_mode为1时有效。 */
        unsigned int  reserved_2             : 1;  /* bit[15]   : 保留。 */
        unsigned int  sw_nosync_fixed_204    : 1;  /* bit[16]   : 只在nosync_fixed模式下有效：
                                                                  0：指定进行188包长检测；
                                                                  1：指定进行204包长检测。 */
        unsigned int  sw_serial_syncbit      : 1;  /* bit[17]   : 0：采样串行sync标识的上升沿（极性处理之后）；
                                                                  1：采样串行sync标识的电平（极性处理之后）。 */
        unsigned int  sw_serial_syncbit_mode : 1;  /* bit[18]   : 串行sync标识的上升沿处理方式：
                                                                  0：使用计数器方式；
                                                                  1：使用bit延迟方式。 */
        unsigned int  reserved_3             : 1;  /* bit[19]   : 保留。 */
        unsigned int  sw_sync_mode           : 2;  /* bit[20-21]: 并行输入模式选择：
                                                                  00：sync/burst方式；
                                                                  01：sync/valid方式；
                                                                  10：nosync_fixed方式，由nosync_fixed_204指定检测包长；
                                                                  11：nosync_188_204方式，硬件需要判断是204还是188。 */
        unsigned int  sw_err_bypass          : 1;  /* bit[22]   : 端口err信号屏蔽选择。屏蔽后将不采样输入的err值，以无效计。
                                                                  0：err信号不屏蔽；
                                                                  1：err信号屏蔽。 */
        unsigned int  sw_pol_sync            : 1;  /* bit[23]   : 端口sync信号极性选择：
                                                                  0：sync信号高有效；
                                                                  1：sync信号低有效。 */
        unsigned int  sw_pol_vld             : 1;  /* bit[24]   : 端口valid信号极性选择：
                                                                  0：valid信号高有效；
                                                                  1：valid信号低有效。 */
        unsigned int  sw_pol_err             : 1;  /* bit[25]   : 端口err信号极性选择：
                                                                  0：err信号为高时表示数据错误；
                                                                  1：err信号为低时表示数据错误。 */
        unsigned int  sw_47_replace          : 1;  /* bit[26]   : 1：替换sync字节为0x47。
                                                                  0：不替换。 */
        unsigned int  sw_serial_lsb_1st      : 1;  /* bit[27]   : 1：串行最低有效为先输入。
                                                                  0：串行最高有效位先输入。 */
        unsigned int  sw_noworking           : 1;  /* bit[28]   : 1：软件配置TS IP不工作；
                                                                  0：软件没有配置TS IP不工作。 */
        unsigned int  sw_serial_in           : 1;  /* bit[29]   : 串行/并行输入选择：
                                                                  0：并行输入；
                                                                  1：串行输入。 */
        unsigned int  sw_bit_sel             : 1;  /* bit[30]   : 输入bit位选择：
                                                                  0：cdata[7]为并行MSB或单bit串行数据线；2bit串行模式时cdata[7:6]为数据线。
                                                                  1：cdata[0]为并行MSB或单bit串行数据线；2bit串行模式时cdata[0:1]为数据线。 */
        unsigned int  sw_port_open           : 1;  /* bit[31]   : 端口开启控制：
                                                                  0：端口关闭；
                                                                  1：端口开启。
                                                                  只有在关闭端口状态下，才能更改端口的配置参数。 */
    } reg;
} SOC_TS_DMX_INF_UNION;
#endif
#define SOC_TS_DMX_INF_sw_build_sync_188_th_START    (0)
#define SOC_TS_DMX_INF_sw_build_sync_188_th_END      (2)
#define SOC_TS_DMX_INF_sw_lose_sync_188_th_START     (4)
#define SOC_TS_DMX_INF_sw_lose_sync_188_th_END       (5)
#define SOC_TS_DMX_INF_sw_afifo_rd_rate_START        (8)
#define SOC_TS_DMX_INF_sw_afifo_rd_rate_END          (10)
#define SOC_TS_DMX_INF_sw_afifo_rd_mode_START        (11)
#define SOC_TS_DMX_INF_sw_afifo_rd_mode_END          (11)
#define SOC_TS_DMX_INF_sw_serial_2bit_START          (12)
#define SOC_TS_DMX_INF_sw_serial_2bit_END            (12)
#define SOC_TS_DMX_INF_sw_serial_2bit_rev_START      (13)
#define SOC_TS_DMX_INF_sw_serial_2bit_rev_END        (13)
#define SOC_TS_DMX_INF_sw_serial_nosync_START        (14)
#define SOC_TS_DMX_INF_sw_serial_nosync_END          (14)
#define SOC_TS_DMX_INF_sw_nosync_fixed_204_START     (16)
#define SOC_TS_DMX_INF_sw_nosync_fixed_204_END       (16)
#define SOC_TS_DMX_INF_sw_serial_syncbit_START       (17)
#define SOC_TS_DMX_INF_sw_serial_syncbit_END         (17)
#define SOC_TS_DMX_INF_sw_serial_syncbit_mode_START  (18)
#define SOC_TS_DMX_INF_sw_serial_syncbit_mode_END    (18)
#define SOC_TS_DMX_INF_sw_sync_mode_START            (20)
#define SOC_TS_DMX_INF_sw_sync_mode_END              (21)
#define SOC_TS_DMX_INF_sw_err_bypass_START           (22)
#define SOC_TS_DMX_INF_sw_err_bypass_END             (22)
#define SOC_TS_DMX_INF_sw_pol_sync_START             (23)
#define SOC_TS_DMX_INF_sw_pol_sync_END               (23)
#define SOC_TS_DMX_INF_sw_pol_vld_START              (24)
#define SOC_TS_DMX_INF_sw_pol_vld_END                (24)
#define SOC_TS_DMX_INF_sw_pol_err_START              (25)
#define SOC_TS_DMX_INF_sw_pol_err_END                (25)
#define SOC_TS_DMX_INF_sw_47_replace_START           (26)
#define SOC_TS_DMX_INF_sw_47_replace_END             (26)
#define SOC_TS_DMX_INF_sw_serial_lsb_1st_START       (27)
#define SOC_TS_DMX_INF_sw_serial_lsb_1st_END         (27)
#define SOC_TS_DMX_INF_sw_noworking_START            (28)
#define SOC_TS_DMX_INF_sw_noworking_END              (28)
#define SOC_TS_DMX_INF_sw_serial_in_START            (29)
#define SOC_TS_DMX_INF_sw_serial_in_END              (29)
#define SOC_TS_DMX_INF_sw_bit_sel_START              (30)
#define SOC_TS_DMX_INF_sw_bit_sel_END                (30)
#define SOC_TS_DMX_INF_sw_port_open_START            (31)
#define SOC_TS_DMX_INF_sw_port_open_END              (31)


/*****************************************************************************
 结构名    : SOC_TS_COUNT_TSP_UNION
 结构说明  : COUNT_TSP 寄存器结构定义。地址偏移量:0x004，初值:0x00000000，宽度:32
 寄存器说明: 所有包计数寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  count_tsp : 32; /* bit[0-31]: TS流输入所有包计数器。 */
    } reg;
} SOC_TS_COUNT_TSP_UNION;
#endif
#define SOC_TS_COUNT_TSP_count_tsp_START  (0)
#define SOC_TS_COUNT_TSP_count_tsp_END    (31)


/*****************************************************************************
 结构名    : SOC_TS_COUNT_TSP_CTRL_UNION
 结构说明  : COUNT_TSP_CTRL 寄存器结构定义。地址偏移量:0x008，初值:0x00000000，宽度:32
 寄存器说明: 所有包计数器控制寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  count_tsp_ctrl : 2;  /* bit[0-1] : 所有包计数控制：
                                                         00：统计计数器复位；
                                                         01：统计计数器使能；
                                                         10：统计计数器停止：
                                                         11：保留。 */
        unsigned int  reserved       : 30; /* bit[2-31]: 保留。 */
    } reg;
} SOC_TS_COUNT_TSP_CTRL_UNION;
#endif
#define SOC_TS_COUNT_TSP_CTRL_count_tsp_ctrl_START  (0)
#define SOC_TS_COUNT_TSP_CTRL_count_tsp_ctrl_END    (1)


/*****************************************************************************
 结构名    : SOC_TS_COUNT_ETSP_UNION
 结构说明  : COUNT_ETSP 寄存器结构定义。地址偏移量:0x00C，初值:0x00000000，宽度:32
 寄存器说明: 错误包计数寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  count_etsp : 32; /* bit[0-31]: TS流输入错误包计数器。 */
    } reg;
} SOC_TS_COUNT_ETSP_UNION;
#endif
#define SOC_TS_COUNT_ETSP_count_etsp_START  (0)
#define SOC_TS_COUNT_ETSP_count_etsp_END    (31)


/*****************************************************************************
 结构名    : SOC_TS_COUNT_ETSP_CTRL_UNION
 结构说明  : COUNT_ETSP_CTRL 寄存器结构定义。地址偏移量:0x010，初值:0x00000000，宽度:32
 寄存器说明: 错误包计数器控制寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  count_etsp_ctrl : 2;  /* bit[0-1] : 错误包计数控制：
                                                          00：统计计数器复位；
                                                          01：统计计数器使能；
                                                          10：统计计数器停止：
                                                          11：保留。 */
        unsigned int  reserved        : 30; /* bit[2-31]: 保留。 */
    } reg;
} SOC_TS_COUNT_ETSP_CTRL_UNION;
#endif
#define SOC_TS_COUNT_ETSP_CTRL_count_etsp_ctrl_START  (0)
#define SOC_TS_COUNT_ETSP_CTRL_count_etsp_ctrl_END    (1)


/*****************************************************************************
 结构名    : SOC_TS_AFIFO_WFULL_ERR_UNION
 结构说明  : AFIFO_WFULL_ERR 寄存器结构定义。地址偏移量:0x014，初值:0x00000000，宽度:32
 寄存器说明: 接口异步FIFO发生错误标志寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  wfull_err : 1;  /* bit[0]   : 接口异步FIFO是否曾发生过溢出错误。读清。 */
        unsigned int  reserved  : 31; /* bit[1-31]: 保留。 */
    } reg;
} SOC_TS_AFIFO_WFULL_ERR_UNION;
#endif
#define SOC_TS_AFIFO_WFULL_ERR_wfull_err_START  (0)
#define SOC_TS_AFIFO_WFULL_ERR_wfull_err_END    (0)


/*****************************************************************************
 结构名    : SOC_TS_AFIFO_WFULL_STATUS_UNION
 结构说明  : AFIFO_WFULL_STATUS 寄存器结构定义。地址偏移量:0x018，初值:0x00000000，宽度:32
 寄存器说明: 接口异步FIFO满标志寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  wfull_dmx : 1;  /* bit[0]   : 接口异步FIFO溢出标志：
                                                    0：输入FIFO未上溢出；
                                                    1：输入FIFO上溢出。 */
        unsigned int  reserved  : 31; /* bit[1-31]: 保留。 */
    } reg;
} SOC_TS_AFIFO_WFULL_STATUS_UNION;
#endif
#define SOC_TS_AFIFO_WFULL_STATUS_wfull_dmx_START  (0)
#define SOC_TS_AFIFO_WFULL_STATUS_wfull_dmx_END    (0)


/*****************************************************************************
 结构名    : SOC_TS_TSSYNC_DEBUG1_UNION
 结构说明  : TSSYNC_DEBUG1 寄存器结构定义。地址偏移量:0x020，初值:0x00000000，宽度:32
 寄存器说明: 调试寄存器1（内部使用）。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  select_188 : 1;  /* bit[0]   : 硬件自己判断是204还是188情况，完成188同步。 */
        unsigned int  select_204 : 1;  /* bit[1]   : 硬件自己判断是204还是188情况，完成204同步。 */
        unsigned int  reserved   : 30; /* bit[2-31]: 保留。 */
    } reg;
} SOC_TS_TSSYNC_DEBUG1_UNION;
#endif
#define SOC_TS_TSSYNC_DEBUG1_select_188_START  (0)
#define SOC_TS_TSSYNC_DEBUG1_select_188_END    (0)
#define SOC_TS_TSSYNC_DEBUG1_select_204_START  (1)
#define SOC_TS_TSSYNC_DEBUG1_select_204_END    (1)


/*****************************************************************************
 结构名    : SOC_TS_TSSYNC_DEBUG2_UNION
 结构说明  : TSSYNC_DEBUG2 寄存器结构定义。地址偏移量:0x024，初值:0x00000000，宽度:32
 寄存器说明: 调试寄存器2（内部使用）(out_188_info)。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  out_sync_off_ins     : 1;  /* bit[0]    : 188同步模块同步丢失指示脉冲信号 */
        unsigned int  out_sync_on_ins      : 1;  /* bit[1]    : 188同步模块同步建立指示脉冲信号 */
        unsigned int  out_valid_ins        : 1;  /* bit[2]    : 188同步模块输出给内部memory的有效信号 */
        unsigned int  out_sync_ins         : 1;  /* bit[3]    : 188同步模块输出给内部memory的同步信号 */
        unsigned int  case_cnt0_tsp188_end : 1;  /* bit[4]    : 188同步模块byte_count_0计数到8'd187指示 */
        unsigned int  case_cnt0_tsp_end    : 1;  /* bit[5]    : 188同步模块byte_count_0计数到同步指定包长指示 */
        unsigned int  state_cur_valid      : 1;  /* bit[6]    : 异步FIFO到188同步模块的有效信号状态 */
        unsigned int  state_cur_sync       : 1;  /* bit[7]    : 异步FIFO到188同步模块的同步信号状态 */
        unsigned int  reserved             : 8;  /* bit[8-15] : 保留。 */
        unsigned int  byte_count_0         : 8;  /* bit[16-23]: 188同步模块已处理一个TS包的字节数 */
        unsigned int  state_cur_data       : 8;  /* bit[24-31]: 当前从异步FIFO读到188同步模块的数据 */
    } reg;
} SOC_TS_TSSYNC_DEBUG2_UNION;
#endif
#define SOC_TS_TSSYNC_DEBUG2_out_sync_off_ins_START      (0)
#define SOC_TS_TSSYNC_DEBUG2_out_sync_off_ins_END        (0)
#define SOC_TS_TSSYNC_DEBUG2_out_sync_on_ins_START       (1)
#define SOC_TS_TSSYNC_DEBUG2_out_sync_on_ins_END         (1)
#define SOC_TS_TSSYNC_DEBUG2_out_valid_ins_START         (2)
#define SOC_TS_TSSYNC_DEBUG2_out_valid_ins_END           (2)
#define SOC_TS_TSSYNC_DEBUG2_out_sync_ins_START          (3)
#define SOC_TS_TSSYNC_DEBUG2_out_sync_ins_END            (3)
#define SOC_TS_TSSYNC_DEBUG2_case_cnt0_tsp188_end_START  (4)
#define SOC_TS_TSSYNC_DEBUG2_case_cnt0_tsp188_end_END    (4)
#define SOC_TS_TSSYNC_DEBUG2_case_cnt0_tsp_end_START     (5)
#define SOC_TS_TSSYNC_DEBUG2_case_cnt0_tsp_end_END       (5)
#define SOC_TS_TSSYNC_DEBUG2_state_cur_valid_START       (6)
#define SOC_TS_TSSYNC_DEBUG2_state_cur_valid_END         (6)
#define SOC_TS_TSSYNC_DEBUG2_state_cur_sync_START        (7)
#define SOC_TS_TSSYNC_DEBUG2_state_cur_sync_END          (7)
#define SOC_TS_TSSYNC_DEBUG2_byte_count_0_START          (16)
#define SOC_TS_TSSYNC_DEBUG2_byte_count_0_END            (23)
#define SOC_TS_TSSYNC_DEBUG2_state_cur_data_START        (24)
#define SOC_TS_TSSYNC_DEBUG2_state_cur_data_END          (31)


/*****************************************************************************
 结构名    : SOC_TS_TSSYNC_DEBUG3_UNION
 结构说明  : TSSYNC_DEBUG3 寄存器结构定义。地址偏移量:0x028，初值:0x00000000，宽度:32
 寄存器说明: 调试寄存器3（内部使用）(out_204_info)。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  out_sync_off_ins     : 1;  /* bit[0]    : 204同步模块同步丢失指示脉冲信号 */
        unsigned int  out_sync_on_ins      : 1;  /* bit[1]    : 204同步模块同步建立指示脉冲信号 */
        unsigned int  out_valid_ins        : 1;  /* bit[2]    : 204同步模块输出给内部memory的有效信号 */
        unsigned int  out_sync_ins         : 1;  /* bit[3]    : 204同步模块输出给内部memory的同步信号 */
        unsigned int  case_cnt0_tsp188_end : 1;  /* bit[4]    : 204同步模块byte_count_0计数到8'd187指示 */
        unsigned int  case_cnt0_tsp_end    : 1;  /* bit[5]    : 204同步模块byte_count_0计数到同步指定包长指示 */
        unsigned int  state_cur_valid      : 1;  /* bit[6]    : 异步FIFO到204同步模块的有效信号状态 */
        unsigned int  state_cur_sync       : 1;  /* bit[7]    : 异步FIFO到204同步模块的同步信号状态 */
        unsigned int  reserved             : 8;  /* bit[8-15] : 保留。 */
        unsigned int  byte_count_0         : 8;  /* bit[16-23]: 204同步模块已处理一个TS包的字节数 */
        unsigned int  state_cur_data       : 8;  /* bit[24-31]: 当前从异步FIFO读到204同步模块的数据 */
    } reg;
} SOC_TS_TSSYNC_DEBUG3_UNION;
#endif
#define SOC_TS_TSSYNC_DEBUG3_out_sync_off_ins_START      (0)
#define SOC_TS_TSSYNC_DEBUG3_out_sync_off_ins_END        (0)
#define SOC_TS_TSSYNC_DEBUG3_out_sync_on_ins_START       (1)
#define SOC_TS_TSSYNC_DEBUG3_out_sync_on_ins_END         (1)
#define SOC_TS_TSSYNC_DEBUG3_out_valid_ins_START         (2)
#define SOC_TS_TSSYNC_DEBUG3_out_valid_ins_END           (2)
#define SOC_TS_TSSYNC_DEBUG3_out_sync_ins_START          (3)
#define SOC_TS_TSSYNC_DEBUG3_out_sync_ins_END            (3)
#define SOC_TS_TSSYNC_DEBUG3_case_cnt0_tsp188_end_START  (4)
#define SOC_TS_TSSYNC_DEBUG3_case_cnt0_tsp188_end_END    (4)
#define SOC_TS_TSSYNC_DEBUG3_case_cnt0_tsp_end_START     (5)
#define SOC_TS_TSSYNC_DEBUG3_case_cnt0_tsp_end_END       (5)
#define SOC_TS_TSSYNC_DEBUG3_state_cur_valid_START       (6)
#define SOC_TS_TSSYNC_DEBUG3_state_cur_valid_END         (6)
#define SOC_TS_TSSYNC_DEBUG3_state_cur_sync_START        (7)
#define SOC_TS_TSSYNC_DEBUG3_state_cur_sync_END          (7)
#define SOC_TS_TSSYNC_DEBUG3_byte_count_0_START          (16)
#define SOC_TS_TSSYNC_DEBUG3_byte_count_0_END            (23)
#define SOC_TS_TSSYNC_DEBUG3_state_cur_data_START        (24)
#define SOC_TS_TSSYNC_DEBUG3_state_cur_data_END          (31)


/*****************************************************************************
 结构名    : SOC_TS_READ_FIFO_FSM_UNION
 结构说明  : READ_FIFO_FSM 寄存器结构定义。地址偏移量:0x02C，初值:0x00000000，宽度:32
 寄存器说明: 读内部FIFO状态机的当前状态：调试寄存器（内部使用）。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  read_fifo_curr_state : 4;  /* bit[0-3] : 读内部FIFO状态机的当前状态：当不断变化时说明有数据接收，否则一般停留在0、7、15位置。 */
        unsigned int  reserved             : 28; /* bit[4-31]: 保留。 */
    } reg;
} SOC_TS_READ_FIFO_FSM_UNION;
#endif
#define SOC_TS_READ_FIFO_FSM_read_fifo_curr_state_START  (0)
#define SOC_TS_READ_FIFO_FSM_read_fifo_curr_state_END    (3)


/*****************************************************************************
 结构名    : SOC_TS_SYNC_CHECK_FSM_UNION
 结构说明  : SYNC_CHECK_FSM 寄存器结构定义。地址偏移量:0x030，初值:0x00000000，宽度:32
 寄存器说明: 同步检测状态机的状态：调试寄存器（内部使用）。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sync_204_curr_state : 4;  /* bit[0-3]  : 204同步模块同步检测状态机的当前状态：
                                                               0x0：WAIT_SYNC；
                                                               0x1~0x7：SYNC_1~SYNC_7；
                                                               0x8：SYNC_ON；
                                                               0x9~0xB：LOSE_SYNC_1~3；
                                                               非0x8~0xB则认为是失步状态；
                                                               其他：保留。 */
        unsigned int  reserved_0          : 4;  /* bit[4-7]  : 保留。 */
        unsigned int  sync_188_curr_state : 4;  /* bit[8-11] : 188同步模块同步检测状态机的当前状态：
                                                               0x0：WAIT_SYNC；
                                                               0x1~0x7：SYNC_1~SYNC_7；
                                                               0x8：SYNC_ON；
                                                               0x9~0xB：LOSE_SYNC_1~3；
                                                               非0x8~0xB则认为是失步状态；
                                                               其他：保留。 */
        unsigned int  reserved_1          : 4;  /* bit[12-15]: 保留。 */
        unsigned int  switch2inf_info     : 3;  /* bit[16-18]: 内部memory存储情况：{clr,full,afull} */
        unsigned int  reserved_2          : 5;  /* bit[19-23]: 保留。 */
        unsigned int  period_sync_on      : 1;  /* bit[24]   : 同步建立指示电平信号 */
        unsigned int  reserved_3          : 3;  /* bit[25-27]: 保留。 */
        unsigned int  period_sync_off     : 1;  /* bit[28]   : 同步丢失指示电平信号 */
        unsigned int  reserved_4          : 3;  /* bit[29-31]: 保留。 */
    } reg;
} SOC_TS_SYNC_CHECK_FSM_UNION;
#endif
#define SOC_TS_SYNC_CHECK_FSM_sync_204_curr_state_START  (0)
#define SOC_TS_SYNC_CHECK_FSM_sync_204_curr_state_END    (3)
#define SOC_TS_SYNC_CHECK_FSM_sync_188_curr_state_START  (8)
#define SOC_TS_SYNC_CHECK_FSM_sync_188_curr_state_END    (11)
#define SOC_TS_SYNC_CHECK_FSM_switch2inf_info_START      (16)
#define SOC_TS_SYNC_CHECK_FSM_switch2inf_info_END        (18)
#define SOC_TS_SYNC_CHECK_FSM_period_sync_on_START       (24)
#define SOC_TS_SYNC_CHECK_FSM_period_sync_on_END         (24)
#define SOC_TS_SYNC_CHECK_FSM_period_sync_off_START      (28)
#define SOC_TS_SYNC_CHECK_FSM_period_sync_off_END        (28)


/*****************************************************************************
 结构名    : SOC_TS_SYNC_CHECK_FLAG_UNION
 结构说明  : SYNC_CHECK_FLAG 寄存器结构定义。地址偏移量:0x034，初值:0x00000000，宽度:32
 寄存器说明: 同步成功标志寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  check_204_success : 1;  /* bit[0]   : 204同步模块同步成功指示：
                                                            0：204同步检测不成功；
                                                            1：204同步检测成功。 */
        unsigned int  check_188_success : 1;  /* bit[1]   : 188同步模块同步成功指示：
                                                            0：188同步检测不成功；
                                                            1：188同步检测成功。 */
        unsigned int  reserved          : 30; /* bit[2-31]: 保留。 */
    } reg;
} SOC_TS_SYNC_CHECK_FLAG_UNION;
#endif
#define SOC_TS_SYNC_CHECK_FLAG_check_204_success_START  (0)
#define SOC_TS_SYNC_CHECK_FLAG_check_204_success_END    (0)
#define SOC_TS_SYNC_CHECK_FLAG_check_188_success_START  (1)
#define SOC_TS_SYNC_CHECK_FLAG_check_188_success_END    (1)


/*****************************************************************************
 结构名    : SOC_TS_SWITCH_FB_INFO_UNION
 结构说明  : SWITCH_FB_INFO 寄存器结构定义。地址偏移量:0x038，初值:0x00000000，宽度:32
 寄存器说明: 内部memory存储状况寄存器：调试寄存器（内部使用）。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  dmx_buf_afull : 1;  /* bit[0]   : 内部memory almost full状态 */
        unsigned int  dmx_buf_full  : 1;  /* bit[1]   : 内部memory full状态 */
        unsigned int  dmx_buf_clr   : 1;  /* bit[2]   : 内部memory clr状态 */
        unsigned int  reserved      : 29; /* bit[3-31]: 保留。 */
    } reg;
} SOC_TS_SWITCH_FB_INFO_UNION;
#endif
#define SOC_TS_SWITCH_FB_INFO_dmx_buf_afull_START  (0)
#define SOC_TS_SWITCH_FB_INFO_dmx_buf_afull_END    (0)
#define SOC_TS_SWITCH_FB_INFO_dmx_buf_full_START   (1)
#define SOC_TS_SWITCH_FB_INFO_dmx_buf_full_END     (1)
#define SOC_TS_SWITCH_FB_INFO_dmx_buf_clr_START    (2)
#define SOC_TS_SWITCH_FB_INFO_dmx_buf_clr_END      (2)


/*****************************************************************************
 结构名    : SOC_TS_DBG_PID_CTRL_UNION
 结构说明  : DBG_PID_CTRL 寄存器结构定义。地址偏移量:0x040，初值:0x00000000，宽度:32
 寄存器说明: 包标识符检测清除寄存器：debug时使用。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sw_pid_detect_clr : 1;  /* bit[0]   : 软件配置包标识符检测状态机到IDLE态，清除前面已寄存的包标识符值 */
        unsigned int  reserved          : 31; /* bit[1-31]: 保留。 */
    } reg;
} SOC_TS_DBG_PID_CTRL_UNION;
#endif
#define SOC_TS_DBG_PID_CTRL_sw_pid_detect_clr_START  (0)
#define SOC_TS_DBG_PID_CTRL_sw_pid_detect_clr_END    (0)


/*****************************************************************************
 结构名    : SOC_TS_DBG_PID0_UNION
 结构说明  : DBG_PID0 寄存器结构定义。地址偏移量:0x044，初值:0x00000000，宽度:32
 寄存器说明: 第0种包标识符检测寄存器：debug时使用。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  dbg_pid0_value : 13; /* bit[0-12] : 第0种标识符的数值 */
        unsigned int  reserved_0     : 3;  /* bit[13-15]: 保留。 */
        unsigned int  dbg_pid0_vld   : 1;  /* bit[16]   : 第0种包标识符计数允许 */
        unsigned int  reserved_1     : 7;  /* bit[17-23]: 保留。 */
        unsigned int  dbg_pid0_cnt   : 8;  /* bit[24-31]: 第0种包标识符的计数器 */
    } reg;
} SOC_TS_DBG_PID0_UNION;
#endif
#define SOC_TS_DBG_PID0_dbg_pid0_value_START  (0)
#define SOC_TS_DBG_PID0_dbg_pid0_value_END    (12)
#define SOC_TS_DBG_PID0_dbg_pid0_vld_START    (16)
#define SOC_TS_DBG_PID0_dbg_pid0_vld_END      (16)
#define SOC_TS_DBG_PID0_dbg_pid0_cnt_START    (24)
#define SOC_TS_DBG_PID0_dbg_pid0_cnt_END      (31)


/*****************************************************************************
 结构名    : SOC_TS_DBG_PID1_UNION
 结构说明  : DBG_PID1 寄存器结构定义。地址偏移量:0x048，初值:0x00000000，宽度:32
 寄存器说明: 第1种包标识符检测寄存器：debug时使用。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  dbg_pid1_value : 13; /* bit[0-12] : 第1种标识符的数值 */
        unsigned int  reserved_0     : 3;  /* bit[13-15]: 保留。 */
        unsigned int  dbg_pid1_vld   : 1;  /* bit[16]   : 第1种包标识符计数允许 */
        unsigned int  reserved_1     : 7;  /* bit[17-23]: 保留。 */
        unsigned int  dbg_pid1_cnt   : 8;  /* bit[24-31]: 第1种包标识符的计数器 */
    } reg;
} SOC_TS_DBG_PID1_UNION;
#endif
#define SOC_TS_DBG_PID1_dbg_pid1_value_START  (0)
#define SOC_TS_DBG_PID1_dbg_pid1_value_END    (12)
#define SOC_TS_DBG_PID1_dbg_pid1_vld_START    (16)
#define SOC_TS_DBG_PID1_dbg_pid1_vld_END      (16)
#define SOC_TS_DBG_PID1_dbg_pid1_cnt_START    (24)
#define SOC_TS_DBG_PID1_dbg_pid1_cnt_END      (31)


/*****************************************************************************
 结构名    : SOC_TS_DBG_PID2_UNION
 结构说明  : DBG_PID2 寄存器结构定义。地址偏移量:0x04C，初值:0x00000000，宽度:32
 寄存器说明: 第2种包标识符检测寄存器：debug时使用。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  dbg_pid2_value : 13; /* bit[0-12] : 第2种标识符的数值 */
        unsigned int  reserved_0     : 3;  /* bit[13-15]: 保留。 */
        unsigned int  dbg_pid2_vld   : 1;  /* bit[16]   : 第2种包标识符计数允许 */
        unsigned int  reserved_1     : 7;  /* bit[17-23]: 保留。 */
        unsigned int  dbg_pid2_cnt   : 8;  /* bit[24-31]: 第2种包标识符的计数器 */
    } reg;
} SOC_TS_DBG_PID2_UNION;
#endif
#define SOC_TS_DBG_PID2_dbg_pid2_value_START  (0)
#define SOC_TS_DBG_PID2_dbg_pid2_value_END    (12)
#define SOC_TS_DBG_PID2_dbg_pid2_vld_START    (16)
#define SOC_TS_DBG_PID2_dbg_pid2_vld_END      (16)
#define SOC_TS_DBG_PID2_dbg_pid2_cnt_START    (24)
#define SOC_TS_DBG_PID2_dbg_pid2_cnt_END      (31)


/*****************************************************************************
 结构名    : SOC_TS_DBG_PID3_UNION
 结构说明  : DBG_PID3 寄存器结构定义。地址偏移量:0x050，初值:0x00000000，宽度:32
 寄存器说明: 第3种包标识符检测寄存器：debug时使用。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  dbg_pid3_value : 13; /* bit[0-12] : 第3种标识符的数值 */
        unsigned int  reserved_0     : 3;  /* bit[13-15]: 保留。 */
        unsigned int  dbg_pid3_vld   : 1;  /* bit[16]   : 第3种包标识符计数允许 */
        unsigned int  reserved_1     : 7;  /* bit[17-23]: 保留。 */
        unsigned int  dbg_pid3_cnt   : 8;  /* bit[24-31]: 第3种包标识符的计数器 */
    } reg;
} SOC_TS_DBG_PID3_UNION;
#endif
#define SOC_TS_DBG_PID3_dbg_pid3_value_START  (0)
#define SOC_TS_DBG_PID3_dbg_pid3_value_END    (12)
#define SOC_TS_DBG_PID3_dbg_pid3_vld_START    (16)
#define SOC_TS_DBG_PID3_dbg_pid3_vld_END      (16)
#define SOC_TS_DBG_PID3_dbg_pid3_cnt_START    (24)
#define SOC_TS_DBG_PID3_dbg_pid3_cnt_END      (31)


/*****************************************************************************
 结构名    : SOC_TS_DBG_PID4_UNION
 结构说明  : DBG_PID4 寄存器结构定义。地址偏移量:0x054，初值:0x00000000，宽度:32
 寄存器说明: 第4种包标识符检测寄存器：debug时使用。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  dbg_pid4_value : 13; /* bit[0-12] : 第4种标识符的数值 */
        unsigned int  reserved_0     : 3;  /* bit[13-15]: 保留。 */
        unsigned int  dbg_pid4_vld   : 1;  /* bit[16]   : 第4种包标识符计数允许 */
        unsigned int  reserved_1     : 7;  /* bit[17-23]: 保留。 */
        unsigned int  dbg_pid4_cnt   : 8;  /* bit[24-31]: 第4种包标识符的计数器 */
    } reg;
} SOC_TS_DBG_PID4_UNION;
#endif
#define SOC_TS_DBG_PID4_dbg_pid4_value_START  (0)
#define SOC_TS_DBG_PID4_dbg_pid4_value_END    (12)
#define SOC_TS_DBG_PID4_dbg_pid4_vld_START    (16)
#define SOC_TS_DBG_PID4_dbg_pid4_vld_END      (16)
#define SOC_TS_DBG_PID4_dbg_pid4_cnt_START    (24)
#define SOC_TS_DBG_PID4_dbg_pid4_cnt_END      (31)


/*****************************************************************************
 结构名    : SOC_TS_DBG_PID5_UNION
 结构说明  : DBG_PID5 寄存器结构定义。地址偏移量:0x058，初值:0x00000000，宽度:32
 寄存器说明: 第5种包标识符检测寄存器：debug时使用。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  dbg_pid5_value : 13; /* bit[0-12] : 第5种标识符的数值 */
        unsigned int  reserved_0     : 3;  /* bit[13-15]: 保留。 */
        unsigned int  dbg_pid5_vld   : 1;  /* bit[16]   : 第5种包标识符计数允许 */
        unsigned int  reserved_1     : 7;  /* bit[17-23]: 保留。 */
        unsigned int  dbg_pid5_cnt   : 8;  /* bit[24-31]: 第5种包标识符的计数器 */
    } reg;
} SOC_TS_DBG_PID5_UNION;
#endif
#define SOC_TS_DBG_PID5_dbg_pid5_value_START  (0)
#define SOC_TS_DBG_PID5_dbg_pid5_value_END    (12)
#define SOC_TS_DBG_PID5_dbg_pid5_vld_START    (16)
#define SOC_TS_DBG_PID5_dbg_pid5_vld_END      (16)
#define SOC_TS_DBG_PID5_dbg_pid5_cnt_START    (24)
#define SOC_TS_DBG_PID5_dbg_pid5_cnt_END      (31)


/*****************************************************************************
 结构名    : SOC_TS_DBG_PID6_UNION
 结构说明  : DBG_PID6 寄存器结构定义。地址偏移量:0x05C，初值:0x00000000，宽度:32
 寄存器说明: 第6种包标识符检测寄存器：debug时使用。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  dbg_pid6_value : 13; /* bit[0-12] : 第6种标识符的数值 */
        unsigned int  reserved_0     : 3;  /* bit[13-15]: 保留。 */
        unsigned int  dbg_pid6_vld   : 1;  /* bit[16]   : 第6种包标识符计数允许 */
        unsigned int  reserved_1     : 7;  /* bit[17-23]: 保留。 */
        unsigned int  dbg_pid6_cnt   : 8;  /* bit[24-31]: 第6种包标识符的计数器 */
    } reg;
} SOC_TS_DBG_PID6_UNION;
#endif
#define SOC_TS_DBG_PID6_dbg_pid6_value_START  (0)
#define SOC_TS_DBG_PID6_dbg_pid6_value_END    (12)
#define SOC_TS_DBG_PID6_dbg_pid6_vld_START    (16)
#define SOC_TS_DBG_PID6_dbg_pid6_vld_END      (16)
#define SOC_TS_DBG_PID6_dbg_pid6_cnt_START    (24)
#define SOC_TS_DBG_PID6_dbg_pid6_cnt_END      (31)


/*****************************************************************************
 结构名    : SOC_TS_DBG_PID7_UNION
 结构说明  : DBG_PID7 寄存器结构定义。地址偏移量:0x060，初值:0x00000000，宽度:32
 寄存器说明: 第7种包标识符检测寄存器：debug时使用。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  dbg_pid7_value : 13; /* bit[0-12] : 第7种标识符的数值 */
        unsigned int  reserved_0     : 3;  /* bit[13-15]: 保留。 */
        unsigned int  dbg_pid7_vld   : 1;  /* bit[16]   : 第7种包标识符计数允许 */
        unsigned int  reserved_1     : 7;  /* bit[17-23]: 保留。 */
        unsigned int  dbg_pid7_cnt   : 8;  /* bit[24-31]: 第7种包标识符的计数器 */
    } reg;
} SOC_TS_DBG_PID7_UNION;
#endif
#define SOC_TS_DBG_PID7_dbg_pid7_value_START  (0)
#define SOC_TS_DBG_PID7_dbg_pid7_value_END    (12)
#define SOC_TS_DBG_PID7_dbg_pid7_vld_START    (16)
#define SOC_TS_DBG_PID7_dbg_pid7_vld_END      (16)
#define SOC_TS_DBG_PID7_dbg_pid7_cnt_START    (24)
#define SOC_TS_DBG_PID7_dbg_pid7_cnt_END      (31)


/*****************************************************************************
 结构名    : SOC_TS_SYNC_LEN_UNION
 结构说明  : SYNC_LEN 寄存器结构定义。地址偏移量:0x064，初值:0x0000CCBC，宽度:32
 寄存器说明: 同步指定包长设置寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sw_188_tsp_len : 8;  /* bit[0-7]  : 188同步指定包长设置值，nosync模式下使用 */
        unsigned int  sw_204_tsp_len : 8;  /* bit[8-15] : 204同步指定包长设置值，nosync模式下使用 */
        unsigned int  reserved       : 16; /* bit[16-31]: 保留。 */
    } reg;
} SOC_TS_SYNC_LEN_UNION;
#endif
#define SOC_TS_SYNC_LEN_sw_188_tsp_len_START  (0)
#define SOC_TS_SYNC_LEN_sw_188_tsp_len_END    (7)
#define SOC_TS_SYNC_LEN_sw_204_tsp_len_START  (8)
#define SOC_TS_SYNC_LEN_sw_204_tsp_len_END    (15)


/*****************************************************************************
 结构名    : SOC_TS_SYNC_CLR_TH_UNION
 结构说明  : SYNC_CLR_TH 寄存器结构定义。地址偏移量:0x068，初值:0x0000000A，宽度:32
 寄存器说明: 串行nosync模式下同步检测过程中计数器（该计数器每256byte加1）清除门限值寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sw_nosync_clrth : 4;  /* bit[0-3] : 串行nosync模式下同步检测过程中计数器（该计数器每256byte加1）清除门限值，也是串并转换状态机中BIT_X间隔跳转的条件 */
        unsigned int  reserved        : 28; /* bit[4-31]: 保留。 */
    } reg;
} SOC_TS_SYNC_CLR_TH_UNION;
#endif
#define SOC_TS_SYNC_CLR_TH_sw_nosync_clrth_START  (0)
#define SOC_TS_SYNC_CLR_TH_sw_nosync_clrth_END    (3)


/*****************************************************************************
 结构名    : SOC_TS_DBG_CNT_SEL_UNION
 结构说明  : DBG_CNT_SEL 寄存器结构定义。地址偏移量:0x06C，初值:0xFFFFFF00，宽度:32
 寄存器说明: 已处理数据量计数触发信号选择寄存器：用于debug TS流速率，即检测一段时间内接收了多少数据量，数据量统计由cnt_sig[255:0]中被选择用于触发的信号触发。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sw_cnt_sel0 : 8;  /* bit[0-7]  : 选择cnt_sig[255:0]中该值所对应bit的信号作为cnt0计数触发条件：
                                                       现在cnt_sig[255:0]中实际有效的只有最低5bit，{cnt_o_timer_clr,unify2fifo_sync,unify2fifo_valid,inf2parse_sync,inf2parse_valid}，另外该值为255时选择进位信号carry_low0，254时用于cnt0计数清除 */
        unsigned int  sw_cnt_sel1 : 8;  /* bit[8-15] : 选择cnt_sig[255:0]中该值所对应bit的信号作为cnt1计数触发条件：
                                                       现在cnt_sig[255:0]中实际有效的只有最低5bit，{cnt_o_timer_clr,unify2fifo_sync,unify2fifo_valid,inf2parse_sync,inf2parse_valid}，另外该值为255时选择进位信号carry_low1，254时用于cnt1计数清除 */
        unsigned int  sw_cnt_sel2 : 8;  /* bit[16-23]: 选择cnt_sig[255:0]中该值所对应bit的信号作为cnt2计数触发条件：
                                                       现在cnt_sig[255:0]中实际有效的只有最低5bit，{cnt_o_timer_clr,unify2fifo_sync,unify2fifo_valid,inf2parse_sync,inf2parse_valid}，另外该值为255时选择进位信号carry_low2，254时用于cnt2计数清除 */
        unsigned int  sw_cnt_sel3 : 8;  /* bit[24-31]: 选择cnt_sig[255:0]中该值所对应bit的信号作为cnt3计数触发条件：
                                                       现在cnt_sig[255:0]中实际有效的只有最低5bit，{cnt_o_timer_clr,unify2fifo_sync,unify2fifo_valid,inf2parse_sync,inf2parse_valid}，另外该值为255时选择进位信号carry_low3，254时用于cnt3计数清除 */
    } reg;
} SOC_TS_DBG_CNT_SEL_UNION;
#endif
#define SOC_TS_DBG_CNT_SEL_sw_cnt_sel0_START  (0)
#define SOC_TS_DBG_CNT_SEL_sw_cnt_sel0_END    (7)
#define SOC_TS_DBG_CNT_SEL_sw_cnt_sel1_START  (8)
#define SOC_TS_DBG_CNT_SEL_sw_cnt_sel1_END    (15)
#define SOC_TS_DBG_CNT_SEL_sw_cnt_sel2_START  (16)
#define SOC_TS_DBG_CNT_SEL_sw_cnt_sel2_END    (23)
#define SOC_TS_DBG_CNT_SEL_sw_cnt_sel3_START  (24)
#define SOC_TS_DBG_CNT_SEL_sw_cnt_sel3_END    (31)


/*****************************************************************************
 结构名    : SOC_TS_DBG_TIMERCFG_UNION
 结构说明  : DBG_TIMERCFG 寄存器结构定义。地址偏移量:0x070，初值:0x0CC16180，宽度:32
 寄存器说明: 已处理数据量计数器计数值采样时间间隔配置寄存器：用于debug TS流速率。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sw_timer_cfg : 32; /* bit[0-31]: 计数器计数值采样时间间隔配置寄存器 */
    } reg;
} SOC_TS_DBG_TIMERCFG_UNION;
#endif
#define SOC_TS_DBG_TIMERCFG_sw_timer_cfg_START  (0)
#define SOC_TS_DBG_TIMERCFG_sw_timer_cfg_END    (31)


/*****************************************************************************
 结构名    : SOC_TS_DBG_RD_CNT_UNION
 结构说明  : DBG_RD_CNT 寄存器结构定义。地址偏移量:0x074，初值:0x00000000，宽度:32
 寄存器说明: 当前已处理数据量计数器：用于debug TS流速率。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  cnt_o_value : 32; /* bit[0-31]: 等于{cnt3,cnt2,cnt1,cnt0}，由四个计数器合并而成，cnt0会向cnt1进位，cnt1会向cnt2进位，cnt2会向cnt3进位 */
    } reg;
} SOC_TS_DBG_RD_CNT_UNION;
#endif
#define SOC_TS_DBG_RD_CNT_cnt_o_value_START  (0)
#define SOC_TS_DBG_RD_CNT_cnt_o_value_END    (31)


/*****************************************************************************
 结构名    : SOC_TS_DBG_RD_TIMER_UNION
 结构说明  : DBG_RD_TIMER 寄存器结构定义。地址偏移量:0x078，初值:0x00000000，宽度:32
 寄存器说明: 当前时间值寄存器：用于debug TS流速率。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  cnt_o_timer : 32; /* bit[0-31]: 从sw_timer_cfg开始每个周期减1 */
    } reg;
} SOC_TS_DBG_RD_TIMER_UNION;
#endif
#define SOC_TS_DBG_RD_TIMER_cnt_o_timer_START  (0)
#define SOC_TS_DBG_RD_TIMER_cnt_o_timer_END    (31)


/*****************************************************************************
 结构名    : SOC_TS_DBG_RD_SAMPLE_UNION
 结构说明  : DBG_RD_SAMPLE 寄存器结构定义。地址偏移量:0x07C，初值:0x00000000，宽度:32
 寄存器说明: 已处理数据量计数器计数值采样所得数值：用于debug TS流速率。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  cnt_o_sample : 32; /* bit[0-31]: 从sw_timer_cfg开始每个周期减1，当减到0时，将{cnt3,cnt2,cnt1,cnt0}寄存 */
    } reg;
} SOC_TS_DBG_RD_SAMPLE_UNION;
#endif
#define SOC_TS_DBG_RD_SAMPLE_cnt_o_sample_START  (0)
#define SOC_TS_DBG_RD_SAMPLE_cnt_o_sample_END    (31)


/*****************************************************************************
 结构名    : SOC_TS_DBG_UNIFY_0_UNION
 结构说明  : DBG_UNIFY_0 寄存器结构定义。地址偏移量:0x080，初值:0x00000000，宽度:32
 寄存器说明: 串行输入下同步失败指示及相关计数寄存器：用于debug。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  c_s_cntb                  : 8;  /* bit[0-7]  : 串行nosync输入模式下输入有效字节数计数器 */
        unsigned int  c_s_cntp                  : 4;  /* bit[8-11] : 串行nosync输入模式下输入256个字节的个数计数器，case_serial_2step脉冲会将其清零 */
        unsigned int  dbg_cnt_clr               : 4;  /* bit[12-15]: 串行nosync输入模式下同步建立前case_serial_2step脉冲个数计数器 */
        unsigned int  dbg_cnt_serial_in_valid   : 5;  /* bit[16-20]: 串行输入下valid信号为高个数计数器，sync信号会将其清零 */
        unsigned int  reserved_0                : 3;  /* bit[21-23]: 保留。 */
        unsigned int  dbg_serial_nosync_failure : 1;  /* bit[24]   : 串行nosync输入模式下同步失败指示 */
        unsigned int  reserved_1                : 7;  /* bit[25-31]: 保留。 */
    } reg;
} SOC_TS_DBG_UNIFY_0_UNION;
#endif
#define SOC_TS_DBG_UNIFY_0_c_s_cntb_START                   (0)
#define SOC_TS_DBG_UNIFY_0_c_s_cntb_END                     (7)
#define SOC_TS_DBG_UNIFY_0_c_s_cntp_START                   (8)
#define SOC_TS_DBG_UNIFY_0_c_s_cntp_END                     (11)
#define SOC_TS_DBG_UNIFY_0_dbg_cnt_clr_START                (12)
#define SOC_TS_DBG_UNIFY_0_dbg_cnt_clr_END                  (15)
#define SOC_TS_DBG_UNIFY_0_dbg_cnt_serial_in_valid_START    (16)
#define SOC_TS_DBG_UNIFY_0_dbg_cnt_serial_in_valid_END      (20)
#define SOC_TS_DBG_UNIFY_0_dbg_serial_nosync_failure_START  (24)
#define SOC_TS_DBG_UNIFY_0_dbg_serial_nosync_failure_END    (24)


/*****************************************************************************
 结构名    : SOC_TS_DBG_UNIFY_1_UNION
 结构说明  : DBG_UNIFY_1 寄存器结构定义。地址偏移量:0x084，初值:0x00000000，宽度:32
 寄存器说明: TS端口模式及相关信号寄存器：用于debug。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  c_port_mode          : 16; /* bit[0-15] : TS端口模式，来自于DMX_INF寄存器中部分信号的组合：{sw_serial_syncbit[1:0],sw_serial_lsb_1st,sw_47_replace,sw_serial_nosync,sw_serial_2bit_rev,sw_serial_2bit,sw_port_open,sw_bit_sel,sw_serial_in,sw_pol_err,sw_pol_vld,sw_pol_sync,sw_err_bypass,sw_sync_mode} */
        unsigned int  port_datapassed_flag : 1;  /* bit[16]   : TS端口已有数据通过标记：
                                                                并行sync/burst输入模式下在sync为高后置1，其他模式下解复位后就置1 */
        unsigned int  cerr_n               : 1;  /* bit[17]   : TS端口输入的err信号 */
        unsigned int  reserved             : 14; /* bit[18-31]: 保留。 */
    } reg;
} SOC_TS_DBG_UNIFY_1_UNION;
#endif
#define SOC_TS_DBG_UNIFY_1_c_port_mode_START           (0)
#define SOC_TS_DBG_UNIFY_1_c_port_mode_END             (15)
#define SOC_TS_DBG_UNIFY_1_port_datapassed_flag_START  (16)
#define SOC_TS_DBG_UNIFY_1_port_datapassed_flag_END    (16)
#define SOC_TS_DBG_UNIFY_1_cerr_n_START                (17)
#define SOC_TS_DBG_UNIFY_1_cerr_n_END                  (17)


/*****************************************************************************
 结构名    : SOC_TS_INTR_RAW_UNION
 结构说明  : INTR_RAW 寄存器结构定义。地址偏移量:0x100，初值:0x00000000，宽度:32
 寄存器说明: TS原始中断寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ts_sync_on_raw       : 1;  /* bit[0]   : nosync模式下TS同步建立原始中断：
                                                               0：无原始中断
                                                               1：有原始中断 */
        unsigned int  ts_sync_off_raw      : 1;  /* bit[1]   : nosync模式下TS同步丢失原始中断：
                                                               0：无原始中断
                                                               1：有原始中断 */
        unsigned int  ts_rx_intr_raw       : 1;  /* bit[2]   : 内部meory中数据高于阈值原始中断：
                                                               0：无原始中断
                                                               1：有原始中断 */
        unsigned int  ts_mem_over_raw      : 1;  /* bit[3]   : 内部memory溢出原始中断：
                                                               0：无原始中断
                                                               1：有原始中断 */
        unsigned int  p_close_cnt_ceil_raw : 1;  /* bit[4]   : TS端口关闭后时钟周期数达到req_ref_memwr_value时原始中断：
                                                               0：无原始中断
                                                               1：有原始中断 */
        unsigned int  m_no_wr_cnt_ceil_raw : 1;  /* bit[5]   : 对内部memory无写操作时钟周期数达到req_ref_port_value时原始中断：
                                                               0：无原始中断
                                                               1：有原始中断 */
        unsigned int  ts_inf2mem_err_raw   : 1;  /* bit[6]   : 有TS包为错误包原始中断：
                                                               0：无原始中断
                                                               1：有原始中断 */
        unsigned int  reserved             : 25; /* bit[7-31]: 保留。 */
    } reg;
} SOC_TS_INTR_RAW_UNION;
#endif
#define SOC_TS_INTR_RAW_ts_sync_on_raw_START        (0)
#define SOC_TS_INTR_RAW_ts_sync_on_raw_END          (0)
#define SOC_TS_INTR_RAW_ts_sync_off_raw_START       (1)
#define SOC_TS_INTR_RAW_ts_sync_off_raw_END         (1)
#define SOC_TS_INTR_RAW_ts_rx_intr_raw_START        (2)
#define SOC_TS_INTR_RAW_ts_rx_intr_raw_END          (2)
#define SOC_TS_INTR_RAW_ts_mem_over_raw_START       (3)
#define SOC_TS_INTR_RAW_ts_mem_over_raw_END         (3)
#define SOC_TS_INTR_RAW_p_close_cnt_ceil_raw_START  (4)
#define SOC_TS_INTR_RAW_p_close_cnt_ceil_raw_END    (4)
#define SOC_TS_INTR_RAW_m_no_wr_cnt_ceil_raw_START  (5)
#define SOC_TS_INTR_RAW_m_no_wr_cnt_ceil_raw_END    (5)
#define SOC_TS_INTR_RAW_ts_inf2mem_err_raw_START    (6)
#define SOC_TS_INTR_RAW_ts_inf2mem_err_raw_END      (6)


/*****************************************************************************
 结构名    : SOC_TS_INTR_MASK_UNION
 结构说明  : INTR_MASK 寄存器结构定义。地址偏移量:0x104，初值:0x00000000，宽度:32
 寄存器说明: TS中断屏蔽设置寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ts_sync_on_mask       : 1;  /* bit[0]   : nosync模式下TS同步建立中断屏蔽：
                                                                0：禁止中断上报
                                                                1：允许中断上报 */
        unsigned int  ts_sync_off_mask      : 1;  /* bit[1]   : nosync模式下TS同步丢失中断屏蔽：
                                                                0：禁止中断上报
                                                                1：允许中断上报 */
        unsigned int  ts_rx_intr_mask       : 1;  /* bit[2]   : 内部meory中数据高于阈值中断屏蔽：
                                                                0：禁止中断上报
                                                                1：允许中断上报 */
        unsigned int  ts_mem_over_mask      : 1;  /* bit[3]   : 内部memory溢出中断屏蔽设置：
                                                                0：禁止中断上报
                                                                1：允许中断上报 */
        unsigned int  p_close_cnt_ceil_mask : 1;  /* bit[4]   : TS端口关闭后时钟周期数达到req_ref_memwr_value时中断屏蔽：
                                                                0：禁止中断上报
                                                                1：允许中断上报 */
        unsigned int  m_no_wr_cnt_ceil_mask : 1;  /* bit[5]   : 对内部memory无写操作时钟周期数达到req_ref_port_value时中断屏蔽：
                                                                0：禁止中断上报
                                                                1：允许中断上报 */
        unsigned int  ts_inf2mem_err_mask   : 1;  /* bit[6]   : 有TS包为错误包中断屏蔽设置：
                                                                0：禁止中断上报
                                                                1：允许中断上报 */
        unsigned int  reserved              : 25; /* bit[7-31]: 保留。 */
    } reg;
} SOC_TS_INTR_MASK_UNION;
#endif
#define SOC_TS_INTR_MASK_ts_sync_on_mask_START        (0)
#define SOC_TS_INTR_MASK_ts_sync_on_mask_END          (0)
#define SOC_TS_INTR_MASK_ts_sync_off_mask_START       (1)
#define SOC_TS_INTR_MASK_ts_sync_off_mask_END         (1)
#define SOC_TS_INTR_MASK_ts_rx_intr_mask_START        (2)
#define SOC_TS_INTR_MASK_ts_rx_intr_mask_END          (2)
#define SOC_TS_INTR_MASK_ts_mem_over_mask_START       (3)
#define SOC_TS_INTR_MASK_ts_mem_over_mask_END         (3)
#define SOC_TS_INTR_MASK_p_close_cnt_ceil_mask_START  (4)
#define SOC_TS_INTR_MASK_p_close_cnt_ceil_mask_END    (4)
#define SOC_TS_INTR_MASK_m_no_wr_cnt_ceil_mask_START  (5)
#define SOC_TS_INTR_MASK_m_no_wr_cnt_ceil_mask_END    (5)
#define SOC_TS_INTR_MASK_ts_inf2mem_err_mask_START    (6)
#define SOC_TS_INTR_MASK_ts_inf2mem_err_mask_END      (6)


/*****************************************************************************
 结构名    : SOC_TS_INTR_STATUS_UNION
 结构说明  : INTR_STATUS 寄存器结构定义。地址偏移量:0x108，初值:0x00000000，宽度:32
 寄存器说明: TS屏蔽后中断状态寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ts_sync_on_status       : 1;  /* bit[0]   : nosync模式下TS同步建立屏蔽后中断状态：
                                                                  0：无屏蔽后中断
                                                                  1：有屏蔽后中断 */
        unsigned int  ts_sync_off_status      : 1;  /* bit[1]   : nosync模式下TS同步丢失屏蔽后中断状态：
                                                                  0：无屏蔽后中断
                                                                  1：有屏蔽后中断 */
        unsigned int  ts_rx_intr_status       : 1;  /* bit[2]   : 内部meory中数据高于阈值屏蔽后中断状态：
                                                                  0：无屏蔽后中断
                                                                  1：有屏蔽后中断 */
        unsigned int  ts_mem_over_status      : 1;  /* bit[3]   : 内部memory溢出屏蔽后中断状态：
                                                                  0：无屏蔽后中断
                                                                  1：有屏蔽后中断 */
        unsigned int  p_close_cnt_ceil_status : 1;  /* bit[4]   : TS端口关闭后时钟周期数达到req_ref_memwr_value时屏蔽后中断状态：
                                                                  0：无屏蔽后中断
                                                                  1：有屏蔽后中断 */
        unsigned int  m_no_wr_cnt_ceil_status : 1;  /* bit[5]   : 对内部memory无写操作时钟周期数达到req_ref_port_value时屏蔽后中断状态：
                                                                  0：无屏蔽后中断
                                                                  1：有屏蔽后中断 */
        unsigned int  ts_inf2mem_err_status   : 1;  /* bit[6]   : 有TS包为错误包屏蔽后中断状态：
                                                                  0：无屏蔽后中断
                                                                  1：有屏蔽后中断 */
        unsigned int  reserved                : 25; /* bit[7-31]: 保留。 */
    } reg;
} SOC_TS_INTR_STATUS_UNION;
#endif
#define SOC_TS_INTR_STATUS_ts_sync_on_status_START        (0)
#define SOC_TS_INTR_STATUS_ts_sync_on_status_END          (0)
#define SOC_TS_INTR_STATUS_ts_sync_off_status_START       (1)
#define SOC_TS_INTR_STATUS_ts_sync_off_status_END         (1)
#define SOC_TS_INTR_STATUS_ts_rx_intr_status_START        (2)
#define SOC_TS_INTR_STATUS_ts_rx_intr_status_END          (2)
#define SOC_TS_INTR_STATUS_ts_mem_over_status_START       (3)
#define SOC_TS_INTR_STATUS_ts_mem_over_status_END         (3)
#define SOC_TS_INTR_STATUS_p_close_cnt_ceil_status_START  (4)
#define SOC_TS_INTR_STATUS_p_close_cnt_ceil_status_END    (4)
#define SOC_TS_INTR_STATUS_m_no_wr_cnt_ceil_status_START  (5)
#define SOC_TS_INTR_STATUS_m_no_wr_cnt_ceil_status_END    (5)
#define SOC_TS_INTR_STATUS_ts_inf2mem_err_status_START    (6)
#define SOC_TS_INTR_STATUS_ts_inf2mem_err_status_END      (6)


/*****************************************************************************
 结构名    : SOC_TS_INTR_CLR_UNION
 结构说明  : INTR_CLR 寄存器结构定义。地址偏移量:0x10C，初值:0x00000000，宽度:32
 寄存器说明: TS中断清除寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ts_sync_on_clr       : 1;  /* bit[0]   : nosync模式下TS同步建立屏蔽后中断清除，写1清除，写0不清 */
        unsigned int  ts_sync_off_clr      : 1;  /* bit[1]   : nosync模式下TS同步丢失屏蔽后中断清除，写1清除，写0不清 */
        unsigned int  ts_rx_intr_clr       : 1;  /* bit[2]   : 内部meory中数据高于阈值中断清除，写1清除，写0不清 */
        unsigned int  ts_mem_over_clr      : 1;  /* bit[3]   : 内部memory溢出中断清除，写1清除，写0不清 */
        unsigned int  p_close_cnt_ceil_clr : 1;  /* bit[4]   : TS端口关闭后时钟周期数达到req_ref_memwr_value中断清除，写1清除，写0不清 */
        unsigned int  m_no_wr_cnt_ceil_clr : 1;  /* bit[5]   : 对内部memory无写操作时钟周期数达到req_ref_port_value时中断清除，写1清除，写0不清 */
        unsigned int  ts_inf2mem_err_clr   : 1;  /* bit[6]   : 有TS包为错误包中断清除，写1清除，写0不清 */
        unsigned int  reserved             : 25; /* bit[7-31]: 保留。 */
    } reg;
} SOC_TS_INTR_CLR_UNION;
#endif
#define SOC_TS_INTR_CLR_ts_sync_on_clr_START        (0)
#define SOC_TS_INTR_CLR_ts_sync_on_clr_END          (0)
#define SOC_TS_INTR_CLR_ts_sync_off_clr_START       (1)
#define SOC_TS_INTR_CLR_ts_sync_off_clr_END         (1)
#define SOC_TS_INTR_CLR_ts_rx_intr_clr_START        (2)
#define SOC_TS_INTR_CLR_ts_rx_intr_clr_END          (2)
#define SOC_TS_INTR_CLR_ts_mem_over_clr_START       (3)
#define SOC_TS_INTR_CLR_ts_mem_over_clr_END         (3)
#define SOC_TS_INTR_CLR_p_close_cnt_ceil_clr_START  (4)
#define SOC_TS_INTR_CLR_p_close_cnt_ceil_clr_END    (4)
#define SOC_TS_INTR_CLR_m_no_wr_cnt_ceil_clr_START  (5)
#define SOC_TS_INTR_CLR_m_no_wr_cnt_ceil_clr_END    (5)
#define SOC_TS_INTR_CLR_ts_inf2mem_err_clr_START    (6)
#define SOC_TS_INTR_CLR_ts_inf2mem_err_clr_END      (6)


/*****************************************************************************
 结构名    : SOC_TS_CLK_INV_SEL_UNION
 结构说明  : CLK_INV_SEL 寄存器结构定义。地址偏移量:0x110，初值:0x00000000，宽度:32
 寄存器说明: TS端口时钟反向选择寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ts_clk_inv_sel : 1;  /* bit[0]   : TS端口时钟反向选择寄存器：
                                                         0：不反向
                                                         1：反向 */
        unsigned int  reserved       : 31; /* bit[1-31]: 保留。 */
    } reg;
} SOC_TS_CLK_INV_SEL_UNION;
#endif
#define SOC_TS_CLK_INV_SEL_ts_clk_inv_sel_START  (0)
#define SOC_TS_CLK_INV_SEL_ts_clk_inv_sel_END    (0)


/*****************************************************************************
 结构名    : SOC_TS_DATA_MV_CTRL_UNION
 结构说明  : DATA_MV_CTRL 寄存器结构定义。地址偏移量:0x114，初值:0x00000002，宽度:32
 寄存器说明: 数据搬出控制寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  data_with_sync_err : 1;  /* bit[0]   : 输出数据是否带有TS随路信号sync和err：
                                                             0：不带sync和err
                                                             1：带有sync和err */
        unsigned int  data_mv_by_dma     : 1;  /* bit[1]   : 数据搬运是由DMA还是CPU搬出：
                                                             0：由CPU搬出
                                                             1：由DMA搬出 */
        unsigned int  reserved           : 30; /* bit[2-31]: 保留。 */
    } reg;
} SOC_TS_DATA_MV_CTRL_UNION;
#endif
#define SOC_TS_DATA_MV_CTRL_data_with_sync_err_START  (0)
#define SOC_TS_DATA_MV_CTRL_data_with_sync_err_END    (0)
#define SOC_TS_DATA_MV_CTRL_data_mv_by_dma_START      (1)
#define SOC_TS_DATA_MV_CTRL_data_mv_by_dma_END        (1)


/*****************************************************************************
 结构名    : SOC_TS_MEM_THRESHOLD_UNION
 结构说明  : MEM_THRESHOLD 寄存器结构定义。地址偏移量:0x118，初值:0x00000000，宽度:32
 寄存器说明: 内部memory阈值设置寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ts_mem_threshold : 16; /* bit[0-15] : 内部memory阈值设置寄存器 */
        unsigned int  reserved         : 16; /* bit[16-31]: 保留。 */
    } reg;
} SOC_TS_MEM_THRESHOLD_UNION;
#endif
#define SOC_TS_MEM_THRESHOLD_ts_mem_threshold_START  (0)
#define SOC_TS_MEM_THRESHOLD_ts_mem_threshold_END    (15)


/*****************************************************************************
 结构名    : SOC_TS_MEM_COUNT_UNION
 结构说明  : MEM_COUNT 寄存器结构定义。地址偏移量:0x11C，初值:0x00000000，宽度:32
 寄存器说明: 内部memory中数据量大小显示寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ts_mem_count : 16; /* bit[0-15] : 内部memory中数据量大小显示寄存器 */
        unsigned int  reserved     : 16; /* bit[16-31]: 保留。 */
    } reg;
} SOC_TS_MEM_COUNT_UNION;
#endif
#define SOC_TS_MEM_COUNT_ts_mem_count_START  (0)
#define SOC_TS_MEM_COUNT_ts_mem_count_END    (15)


/*****************************************************************************
 结构名    : SOC_TS_LAST_INVALID_COUNT_UNION
 结构说明  : LAST_INVALID_COUNT 寄存器结构定义。地址偏移量:0x120，初值:0x00000000，宽度:32
 寄存器说明: 最后一次搬运无效数据量，以10bit为单位。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ts_last_invalid_count : 5;  /* bit[0-4] : 最后一次搬运无效数据量，以10bit为单位 */
        unsigned int  reserved              : 27; /* bit[5-31]: 保留。 */
    } reg;
} SOC_TS_LAST_INVALID_COUNT_UNION;
#endif
#define SOC_TS_LAST_INVALID_COUNT_ts_last_invalid_count_START  (0)
#define SOC_TS_LAST_INVALID_COUNT_ts_last_invalid_count_END    (4)


/*****************************************************************************
 结构名    : SOC_TS_CLK_GT_TIMEOUT_VALUE_UNION
 结构说明  : CLK_GT_TIMEOUT_VALUE 寄存器结构定义。地址偏移量:0x124，初值:0x00000040，宽度:32
 寄存器说明: TS端口关闭后到允许时钟门控的主时钟周期个数配置寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ts_clk_gt_timeout_value : 12; /* bit[0-11] : TS端口关闭后，主时钟域下一个计数器开始计数(端口打开后归0，计到ts_clk_gt_timeout_value后保持)，当计数到ts_clk_gt_timeout_value时，如果内部memory中没有数据，那么时钟门控条件信号busy_condition_meet成0。 */
        unsigned int  reserved                : 20; /* bit[12-31]: 保留。 */
    } reg;
} SOC_TS_CLK_GT_TIMEOUT_VALUE_UNION;
#endif
#define SOC_TS_CLK_GT_TIMEOUT_VALUE_ts_clk_gt_timeout_value_START  (0)
#define SOC_TS_CLK_GT_TIMEOUT_VALUE_ts_clk_gt_timeout_value_END    (11)


/*****************************************************************************
 结构名    : SOC_TS_LAST_REQ_CTRL_UNION
 结构说明  : LAST_REQ_CTRL 寄存器结构定义。地址偏移量:0x128，初值:0x00400040，宽度:32
 寄存器说明: 最后一次搬运请求控制寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  req_ref_memwr_value : 12; /* bit[0-11] : 没有对内部memory进行写操作后，主时钟域下一个计数器开始计数(有些操作归0，计到req_ref_memwr_value后保持)，当计数到req_ref_memwr_value时，如果内部memory中有数据且不足水线，发起DMA传输请求。 */
        unsigned int  reserved_0          : 4;  /* bit[12-15]: 保留。 */
        unsigned int  req_ref_port_value  : 12; /* bit[16-27]: TS端口关闭后，主时钟域下一个计数器开始计数(端口打开后归0，计到req_ref_port_value后保持)，当计数到req_ref_port_value时，如果内部memory中有数据且不足水线，发起DMA传输请求。 */
        unsigned int  req_ref_port_close  : 1;  /* bit[28]   : 0：以req_ref_memwr_value长时间没有对内部memory进行写操作作为发起DMA传输请求的条件
                                                               1：以TS端口关闭后req_ref_port_value长时间作为发起DMA传输请求的条件 */
        unsigned int  reserved_1          : 3;  /* bit[29-31]: 保留。 */
    } reg;
} SOC_TS_LAST_REQ_CTRL_UNION;
#endif
#define SOC_TS_LAST_REQ_CTRL_req_ref_memwr_value_START  (0)
#define SOC_TS_LAST_REQ_CTRL_req_ref_memwr_value_END    (11)
#define SOC_TS_LAST_REQ_CTRL_req_ref_port_value_START   (16)
#define SOC_TS_LAST_REQ_CTRL_req_ref_port_value_END     (27)
#define SOC_TS_LAST_REQ_CTRL_req_ref_port_close_START   (28)
#define SOC_TS_LAST_REQ_CTRL_req_ref_port_close_END     (28)


/*****************************************************************************
 结构名    : SOC_TS_LAST_INVALID_DATA_UNION
 结构说明  : LAST_INVALID_DATA 寄存器结构定义。地址偏移量:0x12C，初值:0x00000000，宽度:32
 寄存器说明: 最后一次搬运所填充无效数据值配置寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ts_last_invalid_data : 10; /* bit[0-9]  : 最后一次搬运所填充无效数据值配置寄存器 */
        unsigned int  reserved             : 22; /* bit[10-31]: 保留。 */
    } reg;
} SOC_TS_LAST_INVALID_DATA_UNION;
#endif
#define SOC_TS_LAST_INVALID_DATA_ts_last_invalid_data_START  (0)
#define SOC_TS_LAST_INVALID_DATA_ts_last_invalid_data_END    (9)


/*****************************************************************************
 结构名    : SOC_TS_MEM_DISABLE_UNION
 结构说明  : MEM_DISABLE 寄存器结构定义。地址偏移量:0x130，初值:0x00000000，宽度:32
 寄存器说明: 内部memory数据清除寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ts_mem_disable : 1;  /* bit[0]   : 写1清除内部memory数据后，再写0 */
        unsigned int  reserved       : 31; /* bit[1-31]: 保留。 */
    } reg;
} SOC_TS_MEM_DISABLE_UNION;
#endif
#define SOC_TS_MEM_DISABLE_ts_mem_disable_START  (0)
#define SOC_TS_MEM_DISABLE_ts_mem_disable_END    (0)






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

#endif /* end of soc_ts_interface.h */
