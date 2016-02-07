

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/

#ifndef __SOC_L2C_INTERFACE_H__
#define __SOC_L2C_INTERFACE_H__

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
/* 寄存器说明：CACHE ID寄存器
   位域定义UNION结构:  SOC_L2C_CACHE_ID_UNION */
#define SOC_L2C_CACHE_ID_ADDR(base)                   ((base) + (0x0))

/* 寄存器说明：CACHE类型寄存器
   位域定义UNION结构:  SOC_L2C_CACHE_TYPE_UNION */
#define SOC_L2C_CACHE_TYPE_ADDR(base)                 ((base) + (0x4))

/* 寄存器说明：CACHE控制寄存器
   位域定义UNION结构:  SOC_L2C_CONTROL_UNION */
#define SOC_L2C_CONTROL_ADDR(base)                    ((base) + (0x100))

/* 寄存器说明：CACHE辅助控制寄存器
   位域定义UNION结构:  SOC_L2C_AUXCONTROL_UNION */
#define SOC_L2C_AUXCONTROL_ADDR(base)                 ((base) + (0x104))

/* 寄存器说明：TAG RAM控制寄存器
   位域定义UNION结构:  SOC_L2C_TAGRAM_CONTROL_UNION */
#define SOC_L2C_TAGRAM_CONTROL_ADDR(base)             ((base) + (0x108))

/* 寄存器说明：DATA RAM寄存器
   位域定义UNION结构:  SOC_L2C_DATARAM_CONTROL_UNION */
#define SOC_L2C_DATARAM_CONTROL_ADDR(base)            ((base) + (0x10C))

/* 寄存器说明：Event计数器控制寄存器
   位域定义UNION结构:  SOC_L2C_EV_COUNTER_CTRL_UNION */
#define SOC_L2C_EV_COUNTER_CTRL_ADDR(base)            ((base) + (0x200))

/* 寄存器说明：Event计数器1配置寄存器
   位域定义UNION结构:  SOC_L2C_EV_COUNTER1_CFG_UNION */
#define SOC_L2C_EV_COUNTER1_CFG_ADDR(base)            ((base) + (0x204))

/* 寄存器说明：Event计数器0配置寄存器
   位域定义UNION结构:  SOC_L2C_EV_COUNTER0_CFG_UNION */
#define SOC_L2C_EV_COUNTER0_CFG_ADDR(base)            ((base) + (0x208))

/* 寄存器说明：Event计数器1
   位域定义UNION结构:  SOC_L2C_EV_COUNTER1_UNION */
#define SOC_L2C_EV_COUNTER1_ADDR(base)                ((base) + (0x20C))

/* 寄存器说明：Event计数器0
   位域定义UNION结构:  SOC_L2C_EV_COUNTER0_UNION */
#define SOC_L2C_EV_COUNTER0_ADDR(base)                ((base) + (0x210))

/* 寄存器说明：中断屏蔽寄存器
   位域定义UNION结构:  SOC_L2C_INT_MASK_UNION */
#define SOC_L2C_INT_MASK_ADDR(base)                   ((base) + (0x214))

/* 寄存器说明：屏蔽后中断状态寄存器
   位域定义UNION结构:  SOC_L2C_INT_MASK_STATUS_UNION */
#define SOC_L2C_INT_MASK_STATUS_ADDR(base)            ((base) + (0x218))

/* 寄存器说明：屏蔽前中断状态寄存器
   位域定义UNION结构:  SOC_L2C_INT_RAW_STATUS_UNION */
#define SOC_L2C_INT_RAW_STATUS_ADDR(base)             ((base) + (0x21C))

/* 寄存器说明：中断清除寄存器
   位域定义UNION结构:  SOC_L2C_INT_CLEAR_UNION */
#define SOC_L2C_INT_CLEAR_ADDR(base)                  ((base) + (0x220))

/* 寄存器说明：Cache同步寄存器
   位域定义UNION结构:  SOC_L2C_CACHE_SYNC_UNION */
#define SOC_L2C_CACHE_SYNC_ADDR(base)                 ((base) + (0x730))

/* 寄存器说明：Invalidate Line by PA寄存器
   位域定义UNION结构:  SOC_L2C_INV_PA_UNION */
#define SOC_L2C_INV_PA_ADDR(base)                     ((base) + (0x770))

/* 寄存器说明：Invalidate by Way寄存器
   位域定义UNION结构:  SOC_L2C_INV_WAY_UNION */
#define SOC_L2C_INV_WAY_ADDR(base)                    ((base) + (0x77C))

/* 寄存器说明：Clean Line by PA寄存器
   位域定义UNION结构:  SOC_L2C_CLEAN_PA_UNION */
#define SOC_L2C_CLEAN_PA_ADDR(base)                   ((base) + (0x7B0))

/* 寄存器说明：Clean Line by Set/Way寄存器
   位域定义UNION结构:  SOC_L2C_CLEAN_INDEX_UNION */
#define SOC_L2C_CLEAN_INDEX_ADDR(base)                ((base) + (0x7B8))

/* 寄存器说明：Clean by Way寄存器
   位域定义UNION结构:  SOC_L2C_CLEAN_WAY_UNION */
#define SOC_L2C_CLEAN_WAY_ADDR(base)                  ((base) + (0x7BC))

/* 寄存器说明：Clean&amp;Invalidate Line by PA寄存器
   位域定义UNION结构:  SOC_L2C_CLEAN_INV_PA_UNION */
#define SOC_L2C_CLEAN_INV_PA_ADDR(base)               ((base) + (0x7F0))

/* 寄存器说明：Clean&amp;Invalidate Line by Set/Way寄存器
   位域定义UNION结构:  SOC_L2C_CLEAN_INV_INDEX_UNION */
#define SOC_L2C_CLEAN_INV_INDEX_ADDR(base)            ((base) + (0x7F8))

/* 寄存器说明：Clean&amp;Invalidate by Way寄存器
   位域定义UNION结构:  SOC_L2C_CLEAN_INV_WAY_UNION */
#define SOC_L2C_CLEAN_INV_WAY_ADDR(base)              ((base) + (0x7FC))

/* 寄存器说明：Data Lockdown0寄存器
   位域定义UNION结构:  SOC_L2C_D_LOCKDOWN0_UNION */
#define SOC_L2C_D_LOCKDOWN0_ADDR(base)                ((base) + (0x900))

/* 寄存器说明：指令Lockdown寄存器
   位域定义UNION结构:  SOC_L2C_I_LOCKDOWN0_UNION */
#define SOC_L2C_I_LOCKDOWN0_ADDR(base)                ((base) + (0x904))

/* 寄存器说明：Data Lockdown1寄存器
   位域定义UNION结构:  SOC_L2C_D_LOCKDOWN1_UNION */
#define SOC_L2C_D_LOCKDOWN1_ADDR(base)                ((base) + (0x908))

/* 寄存器说明：指令Lockdown1寄存器
   位域定义UNION结构:  SOC_L2C_I_LOCKDOWN1_UNION */
#define SOC_L2C_I_LOCKDOWN1_ADDR(base)                ((base) + (0x90C))

/* 寄存器说明：Data Lockdown2寄存器
   位域定义UNION结构:  SOC_L2C_D_LOCKDOWN2_UNION */
#define SOC_L2C_D_LOCKDOWN2_ADDR(base)                ((base) + (0x910))

/* 寄存器说明：指令Lockdown2寄存器
   位域定义UNION结构:  SOC_L2C_I_LOCKDOWN2_UNION */
#define SOC_L2C_I_LOCKDOWN2_ADDR(base)                ((base) + (0x914))

/* 寄存器说明：Data Lockdown3寄存器
   位域定义UNION结构:  SOC_L2C_D_LOCKDOWN3_UNION */
#define SOC_L2C_D_LOCKDOWN3_ADDR(base)                ((base) + (0x918))

/* 寄存器说明：指令Lockdown3寄存器
   位域定义UNION结构:  SOC_L2C_I_LOCKDOWN3_UNION */
#define SOC_L2C_I_LOCKDOWN3_ADDR(base)                ((base) + (0x91C))

/* 寄存器说明：Data Lockdown4寄存器
   位域定义UNION结构:  SOC_L2C_D_LOCKDOWN4_UNION */
#define SOC_L2C_D_LOCKDOWN4_ADDR(base)                ((base) + (0x920))

/* 寄存器说明：指令Lockdown4寄存器
   位域定义UNION结构:  SOC_L2C_I_LOCKDOWN4_UNION */
#define SOC_L2C_I_LOCKDOWN4_ADDR(base)                ((base) + (0x924))

/* 寄存器说明：Data Lockdown5寄存器
   位域定义UNION结构:  SOC_L2C_D_LOCKDOWN5_UNION */
#define SOC_L2C_D_LOCKDOWN5_ADDR(base)                ((base) + (0x928))

/* 寄存器说明：指令Lockdown5寄存器
   位域定义UNION结构:  SOC_L2C_I_LOCKDOWN5_UNION */
#define SOC_L2C_I_LOCKDOWN5_ADDR(base)                ((base) + (0x92C))

/* 寄存器说明：Data Lockdown6寄存器
   位域定义UNION结构:  SOC_L2C_D_LOCKDOWN6_UNION */
#define SOC_L2C_D_LOCKDOWN6_ADDR(base)                ((base) + (0x930))

/* 寄存器说明：指令Lockdown6寄存器
   位域定义UNION结构:  SOC_L2C_I_LOCKDOWN6_UNION */
#define SOC_L2C_I_LOCKDOWN6_ADDR(base)                ((base) + (0x934))

/* 寄存器说明：Data Lockdown7寄存器
   位域定义UNION结构:  SOC_L2C_D_LOCKDOWN7_UNION */
#define SOC_L2C_D_LOCKDOWN7_ADDR(base)                ((base) + (0x938))

/* 寄存器说明：指令Lockdown7寄存器
   位域定义UNION结构:  SOC_L2C_I_LOCKDOWN7_UNION */
#define SOC_L2C_I_LOCKDOWN7_ADDR(base)                ((base) + (0x93C))

/* 寄存器说明：Lockdown by line使能寄存器
   位域定义UNION结构:  SOC_L2C_LOCK_LINE_EN_UNION */
#define SOC_L2C_LOCK_LINE_EN_ADDR(base)               ((base) + (0x950))

/* 寄存器说明：unlock all lines by way寄存器
   位域定义UNION结构:  SOC_L2C_UNLOCK_WAY_UNION */
#define SOC_L2C_UNLOCK_WAY_ADDR(base)                 ((base) + (0x954))

/* 寄存器说明：地址过滤起始寄存器
   位域定义UNION结构:  SOC_L2C_ADDR_FIL_START_UNION */
#define SOC_L2C_ADDR_FIL_START_ADDR(base)             ((base) + (0xC00))

/* 寄存器说明：地址过滤结束寄存器
   位域定义UNION结构:  SOC_L2C_ADDR_FIL_END_UNION */
#define SOC_L2C_ADDR_FIL_END_ADDR(base)               ((base) + (0xC04))

/* 寄存器说明：DEBUG控制寄存器
   位域定义UNION结构:  SOC_L2C_DEBUG_CTRL_UNION */
#define SOC_L2C_DEBUG_CTRL_ADDR(base)                 ((base) + (0xF40))

/* 寄存器说明：Prefetch控制寄存器
   位域定义UNION结构:  SOC_L2C_PREFETCH_CTRL_UNION */
#define SOC_L2C_PREFETCH_CTRL_ADDR(base)              ((base) + (0xF60))

/* 寄存器说明：功耗控制寄存器
   位域定义UNION结构:  SOC_L2C_POWER_CTRL_UNION */
#define SOC_L2C_POWER_CTRL_ADDR(base)                 ((base) + (0xF80))





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
 结构名    : SOC_L2C_CACHE_ID_UNION
 结构说明  : CACHE_ID 寄存器结构定义。地址偏移量:0x0，初值:0x410000C8，宽度:32
 寄存器说明: CACHE ID寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  RTLRELEASE  : 6;  /* bit[0-5]  : 代码版本r3p2 */
        unsigned int  PARTNUM     : 4;  /* bit[6-9]  : IP名CoreLink Level2 Cache Controller L2C-310 */
        unsigned int  CACHEID     : 6;  /* bit[10-15]: Cache ID号为0 */
        unsigned int  reserved    : 8;  /* bit[16-23]:  */
        unsigned int  Implementer : 8;  /* bit[24-31]: 生产商ARM */
    } reg;
} SOC_L2C_CACHE_ID_UNION;
#endif
#define SOC_L2C_CACHE_ID_RTLRELEASE_START   (0)
#define SOC_L2C_CACHE_ID_RTLRELEASE_END     (5)
#define SOC_L2C_CACHE_ID_PARTNUM_START      (6)
#define SOC_L2C_CACHE_ID_PARTNUM_END        (9)
#define SOC_L2C_CACHE_ID_CACHEID_START      (10)
#define SOC_L2C_CACHE_ID_CACHEID_END        (15)
#define SOC_L2C_CACHE_ID_Implementer_START  (24)
#define SOC_L2C_CACHE_ID_Implementer_END    (31)


/*****************************************************************************
 结构名    : SOC_L2C_CACHE_TYPE_UNION
 结构说明  : CACHE_TYPE 寄存器结构定义。地址偏移量:0x4，初值:0x9E340340，宽度:32
 寄存器说明: CACHE类型寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  CACHELINELEN : 2;  /* bit[0-1]  : Cache line length
                                                        2'b00: 32 bytes */
        unsigned int  reserved_0   : 4;  /* bit[2-5]  :  */
        unsigned int  ASSOCATIVITY : 1;  /* bit[6]    : 0: 8-way
                                                        1: 16-way */
        unsigned int  reserved_1   : 1;  /* bit[7]    :  */
        unsigned int  Isize        : 3;  /* bit[8-10] : Instruction Cache way size
                                                        3'b011: 64KB */
        unsigned int  reserved_2   : 1;  /* bit[11]   :  */
        unsigned int  CACHELINELEN : 2;  /* bit[12-13]: Cache line length
                                                        2'b00: 32 bytes */
        unsigned int  reserved_3   : 4;  /* bit[14-17]:  */
        unsigned int  ASSOCATIVITY : 1;  /* bit[18]   : 0: 8-way
                                                        1: 16-way */
        unsigned int  reserved_4   : 1;  /* bit[19]   :  */
        unsigned int  Dsize        : 3;  /* bit[20-22]: Data Cache way size
                                                        3'b011: 64KB */
        unsigned int  reserved_5   : 1;  /* bit[23]   :  */
        unsigned int  H            : 1;  /* bit[24]   : 0: Unified
                                                        1: Harvard */
        unsigned int  CTYPE        : 4;  /* bit[25-28]: 4'b11xy
                                                        x:如果pl310_LOCKDOWN_BY_MASTER宏定义使能则为1，否则为0
                                                        y:如果pl310_LOCKDOWN_BY_LINE宏定义使能则为1，否则为0 */
        unsigned int  reserved_6   : 2;  /* bit[29-30]:  */
        unsigned int  DataBanking  : 1;  /* bit[31]   : 0:不支持data banking
                                                        1:支持data banking */
    } reg;
} SOC_L2C_CACHE_TYPE_UNION;
#endif
#define SOC_L2C_CACHE_TYPE_CACHELINELEN_START  (0)
#define SOC_L2C_CACHE_TYPE_CACHELINELEN_END    (1)
#define SOC_L2C_CACHE_TYPE_ASSOCATIVITY_START  (6)
#define SOC_L2C_CACHE_TYPE_ASSOCATIVITY_END    (6)
#define SOC_L2C_CACHE_TYPE_Isize_START         (8)
#define SOC_L2C_CACHE_TYPE_Isize_END           (10)
#define SOC_L2C_CACHE_TYPE_CACHELINELEN_START  (12)
#define SOC_L2C_CACHE_TYPE_CACHELINELEN_END    (13)
#define SOC_L2C_CACHE_TYPE_ASSOCATIVITY_START  (18)
#define SOC_L2C_CACHE_TYPE_ASSOCATIVITY_END    (18)
#define SOC_L2C_CACHE_TYPE_Dsize_START         (20)
#define SOC_L2C_CACHE_TYPE_Dsize_END           (22)
#define SOC_L2C_CACHE_TYPE_H_START             (24)
#define SOC_L2C_CACHE_TYPE_H_END               (24)
#define SOC_L2C_CACHE_TYPE_CTYPE_START         (25)
#define SOC_L2C_CACHE_TYPE_CTYPE_END           (28)
#define SOC_L2C_CACHE_TYPE_DataBanking_START   (31)
#define SOC_L2C_CACHE_TYPE_DataBanking_END     (31)


/*****************************************************************************
 结构名    : SOC_L2C_CONTROL_UNION
 结构说明  : CONTROL 寄存器结构定义。地址偏移量:0x100，初值:0x00000000，宽度:32
 寄存器说明: CACHE控制寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ENABLE   : 1;  /* bit[0]   : 0: L2 Cache disable
                                                   1: L2 Cache enable */
        unsigned int  reserved : 31; /* bit[1-31]: Reserved */
    } reg;
} SOC_L2C_CONTROL_UNION;
#endif
#define SOC_L2C_CONTROL_ENABLE_START    (0)
#define SOC_L2C_CONTROL_ENABLE_END      (0)


/*****************************************************************************
 结构名    : SOC_L2C_AUXCONTROL_UNION
 结构说明  : AUXCONTROL 寄存器结构定义。地址偏移量:0x104，初值:0x02020000，宽度:32
 寄存器说明: CACHE辅助控制寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  LineOfZero    : 1;  /* bit[0]    : Full Line of Zero Enable
                                                         0: Full line of write zero behavior disable
                                                         1: Full line of write zero behavior enable */
        unsigned int  reserved_0    : 9;  /* bit[1-9]  :  */
        unsigned int  HPrioSOReadEn : 1;  /* bit[10]   : High Priority for Strong-Ordered and Device reads Enable
                                                         0: Master端口上SO/Dev读的优先级低于cacheable访问
                                                         1: Master端口上SO/Dev读的优先级最高 */
        unsigned int  StrBufLimEn   : 1;  /* bit[11]   : Store Buffer device limitation Enable
                                                         0: disable. Device write can take all slots in store buffer
                                                         1: enable. Device write cannot take all slots in store buffer when connected to the A9 MP-Core.There is always one available slot to service Normal Memory */
        unsigned int  ExcCacheCfg   : 1;  /* bit[12]   : Exclusive cache configuration
                                                         0: Disable
                                                         1: Enable */
        unsigned int  ShAttInvEn    : 1;  /* bit[13]   : Shared Attribute Invalidate Enable
                                                         0: Shared invalidate behavior disable
                                                         1: Shared invalidate behavior enable */
        unsigned int  reserved_1    : 2;  /* bit[14-15]:  */
        unsigned int  ASSOCATIVITY  : 1;  /* bit[16]   : 0: 8-way
                                                         1: 16-way */
        unsigned int  WAYSIZE       : 3;  /* bit[17-19]: 3'b000 Reserved, internally mapped to 16KB.
                                                         3'b001 16KB
                                                         3'b010 32KB
                                                         3'b011 64KB
                                                         3'b100 128KB
                                                         3'b101 256KB
                                                         3'b110 512KB
                                                         3'b111 Reserved, internally mapped to 512 KB. */
        unsigned int  EVMONEN       : 1;  /* bit[20]   : Event Monitor bus enable
                                                         0: disable
                                                         1: enable */
        unsigned int  ParityEnable  : 1;  /* bit[21]   : Parity使能
                                                         0: Diable
                                                         1: Enable
                                                         V9R1不支持Parity。 */
        unsigned int  SharedAtt     : 1;  /* bit[22]   : Shared attribute override enable
                                                         0: 默认支持shared访问
                                                         1: 忽略shared attribute */
        unsigned int  ForceWriteAll : 2;  /* bit[23-24]: 强制分配写
                                                         2'b00: 使用AWCACHE配置Write Allocate
                                                         2'b01: 强制no allocate， WA bit为0
                                                         2'b10: 覆盖AWCACHE，WA始终为1，所有cacheable write miss为write allocate
                                                         2'b11: 内部map到00 */
        unsigned int  CACHEPOLICY   : 1;  /* bit[25]   : Cache替换策略选择
                                                         0: Pseudo-random
                                                         1: Round-robin */
        unsigned int  NSLOCKEN      : 1;  /* bit[26]   : Non-secure lockdown enable
                                                         0: Lockdown寄存器不能在非安全下访问
                                                         1: Lockdown寄存器能够在非安全下访问 */
        unsigned int  NSINTACCTRL   : 1;  /* bit[27]   : Non-secure interrupt access control
                                                         0: INT_CLEAR和INT_MASK寄存器只能在安全下才能访问
                                                         1: INT_CLEAR和INT_MASK寄存器能够在非安全或安全下访问 */
        unsigned int  DataPrefEn    : 1;  /* bit[28]   : 0: Data prefetching disable
                                                         1: Data prefetching enable
                                                         参考PREFETCH_CTRL寄存器 */
        unsigned int  InstrPrefEn   : 1;  /* bit[29]   : 0: Instruction prefetching disable
                                                         1: Instruction prefetching enable
                                                         参考PREFETCH_CTRL寄存器 */
        unsigned int  EarlyBREAPEN  : 1;  /* bit[30]   : L2C-310支持对于某些写操作，当store buffer接受了写地址之后立即返回BRESP。这与AXI协议不兼容。配置使能后，AWUSERSx[11]必须为1才生效。
                                                         0: Early BRESP disable
                                                         1: Early BRESP enable */
        unsigned int  reserved_2    : 1;  /* bit[31]   : Reserved */
    } reg;
} SOC_L2C_AUXCONTROL_UNION;
#endif
#define SOC_L2C_AUXCONTROL_LineOfZero_START     (0)
#define SOC_L2C_AUXCONTROL_LineOfZero_END       (0)
#define SOC_L2C_AUXCONTROL_HPrioSOReadEn_START  (10)
#define SOC_L2C_AUXCONTROL_HPrioSOReadEn_END    (10)
#define SOC_L2C_AUXCONTROL_StrBufLimEn_START    (11)
#define SOC_L2C_AUXCONTROL_StrBufLimEn_END      (11)
#define SOC_L2C_AUXCONTROL_ExcCacheCfg_START    (12)
#define SOC_L2C_AUXCONTROL_ExcCacheCfg_END      (12)
#define SOC_L2C_AUXCONTROL_ShAttInvEn_START     (13)
#define SOC_L2C_AUXCONTROL_ShAttInvEn_END       (13)
#define SOC_L2C_AUXCONTROL_ASSOCATIVITY_START   (16)
#define SOC_L2C_AUXCONTROL_ASSOCATIVITY_END     (16)
#define SOC_L2C_AUXCONTROL_WAYSIZE_START        (17)
#define SOC_L2C_AUXCONTROL_WAYSIZE_END          (19)
#define SOC_L2C_AUXCONTROL_EVMONEN_START        (20)
#define SOC_L2C_AUXCONTROL_EVMONEN_END          (20)
#define SOC_L2C_AUXCONTROL_ParityEnable_START   (21)
#define SOC_L2C_AUXCONTROL_ParityEnable_END     (21)
#define SOC_L2C_AUXCONTROL_SharedAtt_START      (22)
#define SOC_L2C_AUXCONTROL_SharedAtt_END        (22)
#define SOC_L2C_AUXCONTROL_ForceWriteAll_START  (23)
#define SOC_L2C_AUXCONTROL_ForceWriteAll_END    (24)
#define SOC_L2C_AUXCONTROL_CACHEPOLICY_START    (25)
#define SOC_L2C_AUXCONTROL_CACHEPOLICY_END      (25)
#define SOC_L2C_AUXCONTROL_NSLOCKEN_START       (26)
#define SOC_L2C_AUXCONTROL_NSLOCKEN_END         (26)
#define SOC_L2C_AUXCONTROL_NSINTACCTRL_START    (27)
#define SOC_L2C_AUXCONTROL_NSINTACCTRL_END      (27)
#define SOC_L2C_AUXCONTROL_DataPrefEn_START     (28)
#define SOC_L2C_AUXCONTROL_DataPrefEn_END       (28)
#define SOC_L2C_AUXCONTROL_InstrPrefEn_START    (29)
#define SOC_L2C_AUXCONTROL_InstrPrefEn_END      (29)
#define SOC_L2C_AUXCONTROL_EarlyBREAPEN_START   (30)
#define SOC_L2C_AUXCONTROL_EarlyBREAPEN_END     (30)


/*****************************************************************************
 结构名    : SOC_L2C_TAGRAM_CONTROL_UNION
 结构说明  : TAGRAM_CONTROL 寄存器结构定义。地址偏移量:0x108，初值:0x00000000，宽度:32
 寄存器说明: TAG RAM控制寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  SETLATENCY : 3;  /* bit[0-2]  : setup latency，默认根据pl310_TAG_SETUP_LAT宏定义
                                                      3'b000: 1cycle，没有额外的latency
                                                      3'b001: 2cycle
                                                      3'b010: 3cycle
                                                      3'b011: 4cycle
                                                      3'b100: 5cycle
                                                      3'b101: 6cycle
                                                      3'b110: 7cycle
                                                      3'b111: 8cycle */
        unsigned int  reserved_0 : 1;  /* bit[3]    :  */
        unsigned int  RDLATENCY  : 3;  /* bit[4-6]  : read access latency，默认根据pl310_TAG_READ_LAT宏定义
                                                      3'b000: 1cycle，没有额外的latency
                                                      3'b001: 2cycle
                                                      3'b010: 3cycle
                                                      3'b011: 4cycle
                                                      3'b100: 5cycle
                                                      3'b101: 6cycle
                                                      3'b110: 7cycle
                                                      3'b111: 8cycle */
        unsigned int  reserved_1 : 1;  /* bit[7]    :  */
        unsigned int  WALATENCY  : 3;  /* bit[8-10] : write access latency，默认根据pl310_TAG_WRITE_LAT宏定义
                                                      3'b000: 1cycle，没有额外的latency
                                                      3'b001: 2cycle
                                                      3'b010: 3cycle
                                                      3'b011: 4cycle
                                                      3'b100: 5cycle
                                                      3'b101: 6cycle
                                                      3'b110: 7cycle
                                                      3'b111: 8cycle */
        unsigned int  reserved_2 : 21; /* bit[11-31]: Reserved */
    } reg;
} SOC_L2C_TAGRAM_CONTROL_UNION;
#endif
#define SOC_L2C_TAGRAM_CONTROL_SETLATENCY_START  (0)
#define SOC_L2C_TAGRAM_CONTROL_SETLATENCY_END    (2)
#define SOC_L2C_TAGRAM_CONTROL_RDLATENCY_START   (4)
#define SOC_L2C_TAGRAM_CONTROL_RDLATENCY_END     (6)
#define SOC_L2C_TAGRAM_CONTROL_WALATENCY_START   (8)
#define SOC_L2C_TAGRAM_CONTROL_WALATENCY_END     (10)


/*****************************************************************************
 结构名    : SOC_L2C_DATARAM_CONTROL_UNION
 结构说明  : DATARAM_CONTROL 寄存器结构定义。地址偏移量:0x10C，初值:0x00000440，宽度:32
 寄存器说明: DATA RAM寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  SETLATENCY : 3;  /* bit[0-2]  : setup latency，默认根据pl310_DATA_SETUP_LAT宏定义
                                                      3'b000: 1cycle，没有额外的latency
                                                      3'b001: 2cycle
                                                      3'b010: 3cycle
                                                      3'b011: 4cycle
                                                      3'b100: 5cycle
                                                      3'b101: 6cycle
                                                      3'b110: 7cycle
                                                      3'b111: 8cycle */
        unsigned int  reserved_0 : 1;  /* bit[3]    :  */
        unsigned int  RDLATENCY  : 3;  /* bit[4-6]  : read access latency，默认根据pl310_DATA_READ_LAT宏定义
                                                      3'b000: 1cycle，没有额外的latency
                                                      3'b001: 2cycle
                                                      3'b010: 3cycle
                                                      3'b011: 4cycle
                                                      3'b100: 5cycle
                                                      3'b101: 6cycle
                                                      3'b110: 7cycle
                                                      3'b111: 8cycle */
        unsigned int  reserved_1 : 1;  /* bit[7]    :  */
        unsigned int  WALATENCY  : 3;  /* bit[8-10] : write access latency，默认根据pl310_DATA_WRITE_LAT宏定义
                                                      3'b000: 1cycle，没有额外的latency
                                                      3'b001: 2cycle
                                                      3'b010: 3cycle
                                                      3'b011: 4cycle
                                                      3'b100: 5cycle
                                                      3'b101: 6cycle
                                                      3'b110: 7cycle
                                                      3'b111: 8cycle */
        unsigned int  reserved_2 : 21; /* bit[11-31]: Reserved */
    } reg;
} SOC_L2C_DATARAM_CONTROL_UNION;
#endif
#define SOC_L2C_DATARAM_CONTROL_SETLATENCY_START  (0)
#define SOC_L2C_DATARAM_CONTROL_SETLATENCY_END    (2)
#define SOC_L2C_DATARAM_CONTROL_RDLATENCY_START   (4)
#define SOC_L2C_DATARAM_CONTROL_RDLATENCY_END     (6)
#define SOC_L2C_DATARAM_CONTROL_WALATENCY_START   (8)
#define SOC_L2C_DATARAM_CONTROL_WALATENCY_END     (10)


/*****************************************************************************
 结构名    : SOC_L2C_EV_COUNTER_CTRL_UNION
 结构说明  : EV_COUNTER_CTRL 寄存器结构定义。地址偏移量:0x200，初值:0x00000000，宽度:32
 寄存器说明: Event计数器控制寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  COUNTENABLE : 1;  /* bit[0]   : 计数器使能
                                                      0: Disable
                                                      1: Enable */
        unsigned int  COUNTRESET  : 2;  /* bit[1-2] : 计数器复位，回读值始终为0，写1，对应的计数器复位
                                                      bit1: Event counter0
                                                      bit2: Event counter1 */
        unsigned int  reserved    : 29; /* bit[3-31]: Reserved */
    } reg;
} SOC_L2C_EV_COUNTER_CTRL_UNION;
#endif
#define SOC_L2C_EV_COUNTER_CTRL_COUNTENABLE_START  (0)
#define SOC_L2C_EV_COUNTER_CTRL_COUNTENABLE_END    (0)
#define SOC_L2C_EV_COUNTER_CTRL_COUNTRESET_START   (1)
#define SOC_L2C_EV_COUNTER_CTRL_COUNTRESET_END     (2)


/*****************************************************************************
 结构名    : SOC_L2C_EV_COUNTER1_CFG_UNION
 结构说明  : EV_COUNTER1_CFG 寄存器结构定义。地址偏移量:0x204，初值:0x00000000，宽度:32
 寄存器说明: Event计数器1配置寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  INTGEN   : 2;  /* bit[0-1] : Event计数器中断生成
                                                   2'b00: disable
                                                   2'b01: enable: increment condition
                                                   2'b10: enable: overflow condition
                                                   2'b11: disable
                                                   注意：当SPNIDEN为低，event计数器只针对非安全event，安全event只有当SPNIDEN为高时计数 */
        unsigned int  EVSOURCE : 4;  /* bit[2-5] : Event源选择
                                                   4'b0000: Counter disable 4'b0001: CO
                                                   4'b0010: DRHIT 4'b0011: DRREQ 4'b0100: DWHIT
                                                   4'b0101: DWREQ 4'b0110: DWTREQ 4'b0111: IRHIT
                                                   4'b1000: IRREQ 4'b1001: WA 4'b1010: IPFALLOC
                                                   4'b1011: EPFHIT 4'b1100: EPFALLOC 4'b1101: SRRCVD
                                                   4'b1110: SRCONF 4'b1111: EPFRCVD */
        unsigned int  reserved : 26; /* bit[6-31]: Reserved */
    } reg;
} SOC_L2C_EV_COUNTER1_CFG_UNION;
#endif
#define SOC_L2C_EV_COUNTER1_CFG_INTGEN_START    (0)
#define SOC_L2C_EV_COUNTER1_CFG_INTGEN_END      (1)
#define SOC_L2C_EV_COUNTER1_CFG_EVSOURCE_START  (2)
#define SOC_L2C_EV_COUNTER1_CFG_EVSOURCE_END    (5)


/*****************************************************************************
 结构名    : SOC_L2C_EV_COUNTER0_CFG_UNION
 结构说明  : EV_COUNTER0_CFG 寄存器结构定义。地址偏移量:0x208，初值:0x00000000，宽度:32
 寄存器说明: Event计数器0配置寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  INTGEN   : 2;  /* bit[0-1] : Event计数器中断生成
                                                   2'b00: disable
                                                   2'b01: enable: increment condition
                                                   2'b10: enable: overflow condition
                                                   2'b11: disable
                                                   注意：当SPNIDEN为低，event计数器只针对非安全event，安全event只有当SPNIDEN为高时计数 */
        unsigned int  EVSOURCE : 4;  /* bit[2-5] : Event源选择
                                                   4'b0000: Counter disable 4'b0001: CO
                                                   4'b0010: DRHIT 4'b0011: DRREQ 4'b0100: DWHIT
                                                   4'b0101: DWREQ 4'b0110: DWTREQ 4'b0111: IRHIT
                                                   4'b1000: IRREQ 4'b1001: WA 4'b1010: IPFALLOC
                                                   4'b1011: EPFHIT 4'b1100: EPFALLOC 4'b1101: SRRCVD
                                                   4'b1110: SRCONF 4'b1111: EPFRCVD */
        unsigned int  reserved : 26; /* bit[6-31]: Reserved */
    } reg;
} SOC_L2C_EV_COUNTER0_CFG_UNION;
#endif
#define SOC_L2C_EV_COUNTER0_CFG_INTGEN_START    (0)
#define SOC_L2C_EV_COUNTER0_CFG_INTGEN_END      (1)
#define SOC_L2C_EV_COUNTER0_CFG_EVSOURCE_START  (2)
#define SOC_L2C_EV_COUNTER0_CFG_EVSOURCE_END    (5)


/*****************************************************************************
 结构名    : SOC_L2C_EV_COUNTER1_UNION
 结构说明  : EV_COUNTER1 寄存器结构定义。地址偏移量:0x20C，初值:0x00000000，宽度:32
 寄存器说明: Event计数器1
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  VALUE : 32; /* bit[0-31]: Counter value
                                                计数器计数值 */
    } reg;
} SOC_L2C_EV_COUNTER1_UNION;
#endif
#define SOC_L2C_EV_COUNTER1_VALUE_START  (0)
#define SOC_L2C_EV_COUNTER1_VALUE_END    (31)


/*****************************************************************************
 结构名    : SOC_L2C_EV_COUNTER0_UNION
 结构说明  : EV_COUNTER0 寄存器结构定义。地址偏移量:0x210，初值:0x00000000，宽度:32
 寄存器说明: Event计数器0
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  VALUE : 32; /* bit[0-31]: Counter value
                                                计数器计数值 */
    } reg;
} SOC_L2C_EV_COUNTER0_UNION;
#endif
#define SOC_L2C_EV_COUNTER0_VALUE_START  (0)
#define SOC_L2C_EV_COUNTER0_VALUE_END    (31)


/*****************************************************************************
 结构名    : SOC_L2C_INT_MASK_UNION
 结构说明  : INT_MASK 寄存器结构定义。地址偏移量:0x214，初值:0x0000，宽度:32
 寄存器说明: 中断屏蔽寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ECNTR    : 1;  /* bit[0]   : Event Counter1 and Counter0 Overflow Increment
                                                   0: Masked
                                                   1: Enabled */
        unsigned int  PARRT    : 1;  /* bit[1]   : Parity Error on L2 tag RAM read
                                                   0: Masked
                                                   1: Enabled */
        unsigned int  PARRD    : 1;  /* bit[2]   : Parity Error on L2 data RAM read
                                                   0: Masked
                                                   1: Enabled */
        unsigned int  ERRWT    : 1;  /* bit[3]   : Error on L2 tag RAM write
                                                   0: Masked
                                                   1: Enabled */
        unsigned int  ERRWD    : 1;  /* bit[4]   : Error on L2 data RAM write
                                                   0: Masked
                                                   1: Enabled */
        unsigned int  ERRRT    : 1;  /* bit[5]   : Error on L2 tag RAM read
                                                   0: Masked
                                                   1: Enabled */
        unsigned int  ERRRD    : 1;  /* bit[6]   : Error on L2 data RAM read
                                                   0: Masked
                                                   1: Enabled */
        unsigned int  SLVERR   : 1;  /* bit[7]   : SLVERR from L3
                                                   0: Masked
                                                   1: Enabled */
        unsigned int  DECERR   : 1;  /* bit[8]   : DECERR from L3
                                                   0: Masked
                                                   1: Enabled */
        unsigned int  reserved : 23; /* bit[9-31]:  */
    } reg;
} SOC_L2C_INT_MASK_UNION;
#endif
#define SOC_L2C_INT_MASK_ECNTR_START     (0)
#define SOC_L2C_INT_MASK_ECNTR_END       (0)
#define SOC_L2C_INT_MASK_PARRT_START     (1)
#define SOC_L2C_INT_MASK_PARRT_END       (1)
#define SOC_L2C_INT_MASK_PARRD_START     (2)
#define SOC_L2C_INT_MASK_PARRD_END       (2)
#define SOC_L2C_INT_MASK_ERRWT_START     (3)
#define SOC_L2C_INT_MASK_ERRWT_END       (3)
#define SOC_L2C_INT_MASK_ERRWD_START     (4)
#define SOC_L2C_INT_MASK_ERRWD_END       (4)
#define SOC_L2C_INT_MASK_ERRRT_START     (5)
#define SOC_L2C_INT_MASK_ERRRT_END       (5)
#define SOC_L2C_INT_MASK_ERRRD_START     (6)
#define SOC_L2C_INT_MASK_ERRRD_END       (6)
#define SOC_L2C_INT_MASK_SLVERR_START    (7)
#define SOC_L2C_INT_MASK_SLVERR_END      (7)
#define SOC_L2C_INT_MASK_DECERR_START    (8)
#define SOC_L2C_INT_MASK_DECERR_END      (8)


/*****************************************************************************
 结构名    : SOC_L2C_INT_MASK_STATUS_UNION
 结构说明  : INT_MASK_STATUS 寄存器结构定义。地址偏移量:0x218，初值:0x0000，宽度:32
 寄存器说明: 屏蔽后中断状态寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ECNTR    : 1;  /* bit[0]   : Event Counter1 and Counter0 Overflow Increment
                                                   0: 无中断
                                                   1: 有中断 */
        unsigned int  PARRT    : 1;  /* bit[1]   : Parity Error on L2 tag RAM read
                                                   0: 无中断
                                                   1: 有中断 */
        unsigned int  PARRD    : 1;  /* bit[2]   : Parity Error on L2 data RAM read
                                                   0: 无中断
                                                   1: 有中断 */
        unsigned int  ERRWT    : 1;  /* bit[3]   : Error on L2 tag RAM write
                                                   0: 无中断
                                                   1: 有中断 */
        unsigned int  ERRWD    : 1;  /* bit[4]   : Error on L2 data RAM write
                                                   0: 无中断
                                                   1: 有中断 */
        unsigned int  ERRRT    : 1;  /* bit[5]   : Error on L2 tag RAM read
                                                   0: 无中断
                                                   1: 有中断 */
        unsigned int  ERRRD    : 1;  /* bit[6]   : Error on L2 data RAM read
                                                   0: 无中断
                                                   1: 有中断 */
        unsigned int  SLVERR   : 1;  /* bit[7]   : SLVERR from L3
                                                   0: 无中断
                                                   1: 有中断 */
        unsigned int  DECERR   : 1;  /* bit[8]   : DECERR from L3
                                                   0: 无中断
                                                   1: 有中断 */
        unsigned int  reserved : 23; /* bit[9-31]:  */
    } reg;
} SOC_L2C_INT_MASK_STATUS_UNION;
#endif
#define SOC_L2C_INT_MASK_STATUS_ECNTR_START     (0)
#define SOC_L2C_INT_MASK_STATUS_ECNTR_END       (0)
#define SOC_L2C_INT_MASK_STATUS_PARRT_START     (1)
#define SOC_L2C_INT_MASK_STATUS_PARRT_END       (1)
#define SOC_L2C_INT_MASK_STATUS_PARRD_START     (2)
#define SOC_L2C_INT_MASK_STATUS_PARRD_END       (2)
#define SOC_L2C_INT_MASK_STATUS_ERRWT_START     (3)
#define SOC_L2C_INT_MASK_STATUS_ERRWT_END       (3)
#define SOC_L2C_INT_MASK_STATUS_ERRWD_START     (4)
#define SOC_L2C_INT_MASK_STATUS_ERRWD_END       (4)
#define SOC_L2C_INT_MASK_STATUS_ERRRT_START     (5)
#define SOC_L2C_INT_MASK_STATUS_ERRRT_END       (5)
#define SOC_L2C_INT_MASK_STATUS_ERRRD_START     (6)
#define SOC_L2C_INT_MASK_STATUS_ERRRD_END       (6)
#define SOC_L2C_INT_MASK_STATUS_SLVERR_START    (7)
#define SOC_L2C_INT_MASK_STATUS_SLVERR_END      (7)
#define SOC_L2C_INT_MASK_STATUS_DECERR_START    (8)
#define SOC_L2C_INT_MASK_STATUS_DECERR_END      (8)


/*****************************************************************************
 结构名    : SOC_L2C_INT_RAW_STATUS_UNION
 结构说明  : INT_RAW_STATUS 寄存器结构定义。地址偏移量:0x21C，初值:0x0000，宽度:32
 寄存器说明: 屏蔽前中断状态寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ECNTR    : 1;  /* bit[0]   : Event Counter1 and Counter0 Overflow Increment
                                                   0: 无中断
                                                   1: 有中断 */
        unsigned int  PARRT    : 1;  /* bit[1]   : Parity Error on L2 tag RAM read
                                                   0: 无中断
                                                   1: 有中断 */
        unsigned int  PARRD    : 1;  /* bit[2]   : Parity Error on L2 data RAM read
                                                   0: 无中断
                                                   1: 有中断 */
        unsigned int  ERRWT    : 1;  /* bit[3]   : Error on L2 tag RAM write
                                                   0: 无中断
                                                   1: 有中断 */
        unsigned int  ERRWD    : 1;  /* bit[4]   : Error on L2 data RAM write
                                                   0: 无中断
                                                   1: 有中断 */
        unsigned int  ERRRT    : 1;  /* bit[5]   : Error on L2 tag RAM read
                                                   0: 无中断
                                                   1: 有中断 */
        unsigned int  ERRRD    : 1;  /* bit[6]   : Error on L2 data RAM read
                                                   0: 无中断
                                                   1: 有中断 */
        unsigned int  SLVERR   : 1;  /* bit[7]   : SLVERR from L3
                                                   0: 无中断
                                                   1: 有中断 */
        unsigned int  DECERR   : 1;  /* bit[8]   : DECERR from L3
                                                   0: 无中断
                                                   1: 有中断 */
        unsigned int  reserved : 23; /* bit[9-31]:  */
    } reg;
} SOC_L2C_INT_RAW_STATUS_UNION;
#endif
#define SOC_L2C_INT_RAW_STATUS_ECNTR_START     (0)
#define SOC_L2C_INT_RAW_STATUS_ECNTR_END       (0)
#define SOC_L2C_INT_RAW_STATUS_PARRT_START     (1)
#define SOC_L2C_INT_RAW_STATUS_PARRT_END       (1)
#define SOC_L2C_INT_RAW_STATUS_PARRD_START     (2)
#define SOC_L2C_INT_RAW_STATUS_PARRD_END       (2)
#define SOC_L2C_INT_RAW_STATUS_ERRWT_START     (3)
#define SOC_L2C_INT_RAW_STATUS_ERRWT_END       (3)
#define SOC_L2C_INT_RAW_STATUS_ERRWD_START     (4)
#define SOC_L2C_INT_RAW_STATUS_ERRWD_END       (4)
#define SOC_L2C_INT_RAW_STATUS_ERRRT_START     (5)
#define SOC_L2C_INT_RAW_STATUS_ERRRT_END       (5)
#define SOC_L2C_INT_RAW_STATUS_ERRRD_START     (6)
#define SOC_L2C_INT_RAW_STATUS_ERRRD_END       (6)
#define SOC_L2C_INT_RAW_STATUS_SLVERR_START    (7)
#define SOC_L2C_INT_RAW_STATUS_SLVERR_END      (7)
#define SOC_L2C_INT_RAW_STATUS_DECERR_START    (8)
#define SOC_L2C_INT_RAW_STATUS_DECERR_END      (8)


/*****************************************************************************
 结构名    : SOC_L2C_INT_CLEAR_UNION
 结构说明  : INT_CLEAR 寄存器结构定义。地址偏移量:0x220，初值:0x0000，宽度:32
 寄存器说明: 中断清除寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ECNTR    : 1;  /* bit[0]   : Event Counter1 and Counter0 Overflow Increment
                                                   0: 无效
                                                   1: 清除中断 */
        unsigned int  PARRT    : 1;  /* bit[1]   : Parity Error on L2 tag RAM read
                                                   0: 无效
                                                   1: 清除中断 */
        unsigned int  PARRD    : 1;  /* bit[2]   : Parity Error on L2 data RAM read
                                                   0: 无效
                                                   1: 清除中断 */
        unsigned int  ERRWT    : 1;  /* bit[3]   : Error on L2 tag RAM write
                                                   0: 无效
                                                   1: 清除中断 */
        unsigned int  ERRWD    : 1;  /* bit[4]   : Error on L2 data RAM write
                                                   0: 无效
                                                   1: 清除中断 */
        unsigned int  ERRRT    : 1;  /* bit[5]   : Error on L2 tag RAM read
                                                   0: 无效
                                                   1: 清除中断 */
        unsigned int  ERRRD    : 1;  /* bit[6]   : Error on L2 data RAM read
                                                   0: 无效
                                                   1: 清除中断 */
        unsigned int  SLVERR   : 1;  /* bit[7]   : SLVERR from L3
                                                   0: 无效
                                                   1: 清除中断 */
        unsigned int  DECERR   : 1;  /* bit[8]   : DECERR from L3
                                                   0: 无效
                                                   1: 清除中断 */
        unsigned int  reserved : 23; /* bit[9-31]:  */
    } reg;
} SOC_L2C_INT_CLEAR_UNION;
#endif
#define SOC_L2C_INT_CLEAR_ECNTR_START     (0)
#define SOC_L2C_INT_CLEAR_ECNTR_END       (0)
#define SOC_L2C_INT_CLEAR_PARRT_START     (1)
#define SOC_L2C_INT_CLEAR_PARRT_END       (1)
#define SOC_L2C_INT_CLEAR_PARRD_START     (2)
#define SOC_L2C_INT_CLEAR_PARRD_END       (2)
#define SOC_L2C_INT_CLEAR_ERRWT_START     (3)
#define SOC_L2C_INT_CLEAR_ERRWT_END       (3)
#define SOC_L2C_INT_CLEAR_ERRWD_START     (4)
#define SOC_L2C_INT_CLEAR_ERRWD_END       (4)
#define SOC_L2C_INT_CLEAR_ERRRT_START     (5)
#define SOC_L2C_INT_CLEAR_ERRRT_END       (5)
#define SOC_L2C_INT_CLEAR_ERRRD_START     (6)
#define SOC_L2C_INT_CLEAR_ERRRD_END       (6)
#define SOC_L2C_INT_CLEAR_SLVERR_START    (7)
#define SOC_L2C_INT_CLEAR_SLVERR_END      (7)
#define SOC_L2C_INT_CLEAR_DECERR_START    (8)
#define SOC_L2C_INT_CLEAR_DECERR_END      (8)


/*****************************************************************************
 结构名    : SOC_L2C_CACHE_SYNC_UNION
 结构说明  : CACHE_SYNC 寄存器结构定义。地址偏移量:0x730，初值:0x0000，宽度:32
 寄存器说明: Cache同步寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  C        : 1;  /* bit[0]   : 排空所有Buffer使能
                                                   0: 写无效
                                                   1: 使能
                                                   注意：当完成操作后，该bit自清零 */
        unsigned int  reserved : 31; /* bit[1-31]:  */
    } reg;
} SOC_L2C_CACHE_SYNC_UNION;
#endif
#define SOC_L2C_CACHE_SYNC_C_START         (0)
#define SOC_L2C_CACHE_SYNC_C_END           (0)


/*****************************************************************************
 结构名    : SOC_L2C_INV_PA_UNION
 结构说明  : INV_PA 寄存器结构定义。地址偏移量:0x770，初值:0x0000，宽度:32
 寄存器说明: Invalidate Line by PA寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  C        : 1;  /* bit[0]    : 使能
                                                    0: 写无效
                                                    1: 使能
                                                    注意：当完成操作后，该bit自清零 */
        unsigned int  reserved : 4;  /* bit[1-4]  :  */
        unsigned int  INDEX    : 11; /* bit[5-15] : Index */
        unsigned int  TAG      : 16; /* bit[16-31]: TAG */
    } reg;
} SOC_L2C_INV_PA_UNION;
#endif
#define SOC_L2C_INV_PA_C_START         (0)
#define SOC_L2C_INV_PA_C_END           (0)
#define SOC_L2C_INV_PA_INDEX_START     (5)
#define SOC_L2C_INV_PA_INDEX_END       (15)
#define SOC_L2C_INV_PA_TAG_START       (16)
#define SOC_L2C_INV_PA_TAG_END         (31)


/*****************************************************************************
 结构名    : SOC_L2C_INV_WAY_UNION
 结构说明  : INV_WAY 寄存器结构定义。地址偏移量:0x77C，初值:0x0000，宽度:32
 寄存器说明: Invalidate by Way寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  WAY      : 16; /* bit[0-15] : 使能，每bit对应一个way
                                                    0: 写无效
                                                    1: 使能
                                                    注意：当完成操作后，该bit自清零 */
        unsigned int  reserved : 16; /* bit[16-31]:  */
    } reg;
} SOC_L2C_INV_WAY_UNION;
#endif
#define SOC_L2C_INV_WAY_WAY_START       (0)
#define SOC_L2C_INV_WAY_WAY_END         (15)


/*****************************************************************************
 结构名    : SOC_L2C_CLEAN_PA_UNION
 结构说明  : CLEAN_PA 寄存器结构定义。地址偏移量:0x7B0，初值:0x0000，宽度:32
 寄存器说明: Clean Line by PA寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  C        : 1;  /* bit[0]    : 使能
                                                    0: 写无效
                                                    1: 使能
                                                    注意：当完成操作后，该bit自清零 */
        unsigned int  reserved : 4;  /* bit[1-4]  :  */
        unsigned int  INDEX    : 11; /* bit[5-15] : Index */
        unsigned int  TAG      : 16; /* bit[16-31]: TAG */
    } reg;
} SOC_L2C_CLEAN_PA_UNION;
#endif
#define SOC_L2C_CLEAN_PA_C_START         (0)
#define SOC_L2C_CLEAN_PA_C_END           (0)
#define SOC_L2C_CLEAN_PA_INDEX_START     (5)
#define SOC_L2C_CLEAN_PA_INDEX_END       (15)
#define SOC_L2C_CLEAN_PA_TAG_START       (16)
#define SOC_L2C_CLEAN_PA_TAG_END         (31)


/*****************************************************************************
 结构名    : SOC_L2C_CLEAN_INDEX_UNION
 结构说明  : CLEAN_INDEX 寄存器结构定义。地址偏移量:0x7B8，初值:0x0000，宽度:32
 寄存器说明: Clean Line by Set/Way寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  C        : 1;  /* bit[0]    : 使能
                                                    0: 写无效
                                                    1: 使能
                                                    注意：当完成操作后，该bit自清零 */
        unsigned int  reserved_0: 4;  /* bit[1-4]  :  */
        unsigned int  INDEX    : 11; /* bit[5-15] : Index */
        unsigned int  reserved_1: 12; /* bit[16-27]:  */
        unsigned int  WAY      : 4;  /* bit[28-31]: WAY */
    } reg;
} SOC_L2C_CLEAN_INDEX_UNION;
#endif
#define SOC_L2C_CLEAN_INDEX_C_START         (0)
#define SOC_L2C_CLEAN_INDEX_C_END           (0)
#define SOC_L2C_CLEAN_INDEX_INDEX_START     (5)
#define SOC_L2C_CLEAN_INDEX_INDEX_END       (15)
#define SOC_L2C_CLEAN_INDEX_WAY_START       (28)
#define SOC_L2C_CLEAN_INDEX_WAY_END         (31)


/*****************************************************************************
 结构名    : SOC_L2C_CLEAN_WAY_UNION
 结构说明  : CLEAN_WAY 寄存器结构定义。地址偏移量:0x7BC，初值:0x0000，宽度:32
 寄存器说明: Clean by Way寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  WAY      : 16; /* bit[0-15] : 使能，每bit对应一个way
                                                    0: 写无效
                                                    1: 使能
                                                    注意：当完成操作后，该bit自清零 */
        unsigned int  reserved : 16; /* bit[16-31]:  */
    } reg;
} SOC_L2C_CLEAN_WAY_UNION;
#endif
#define SOC_L2C_CLEAN_WAY_WAY_START       (0)
#define SOC_L2C_CLEAN_WAY_WAY_END         (15)


/*****************************************************************************
 结构名    : SOC_L2C_CLEAN_INV_PA_UNION
 结构说明  : CLEAN_INV_PA 寄存器结构定义。地址偏移量:0x7F0，初值:0x0000，宽度:32
 寄存器说明: Clean&amp;Invalidate Line by PA寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  C        : 1;  /* bit[0]    : 使能
                                                    0: 写无效
                                                    1: 使能
                                                    注意：当完成操作后，该bit自清零 */
        unsigned int  reserved : 4;  /* bit[1-4]  :  */
        unsigned int  INDEX    : 11; /* bit[5-15] : Index */
        unsigned int  TAG      : 16; /* bit[16-31]: TAG */
    } reg;
} SOC_L2C_CLEAN_INV_PA_UNION;
#endif
#define SOC_L2C_CLEAN_INV_PA_C_START         (0)
#define SOC_L2C_CLEAN_INV_PA_C_END           (0)
#define SOC_L2C_CLEAN_INV_PA_INDEX_START     (5)
#define SOC_L2C_CLEAN_INV_PA_INDEX_END       (15)
#define SOC_L2C_CLEAN_INV_PA_TAG_START       (16)
#define SOC_L2C_CLEAN_INV_PA_TAG_END         (31)


/*****************************************************************************
 结构名    : SOC_L2C_CLEAN_INV_INDEX_UNION
 结构说明  : CLEAN_INV_INDEX 寄存器结构定义。地址偏移量:0x7F8，初值:0x0000，宽度:32
 寄存器说明: Clean&amp;Invalidate Line by Set/Way寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  C        : 1;  /* bit[0]    : 使能
                                                    0: 写无效
                                                    1: 使能
                                                    注意：当完成操作后，该bit自清零 */
        unsigned int  reserved_0: 4;  /* bit[1-4]  :  */
        unsigned int  INDEX    : 11; /* bit[5-15] : Index */
        unsigned int  reserved_1: 12; /* bit[16-27]:  */
        unsigned int  WAY      : 4;  /* bit[28-31]: WAY */
    } reg;
} SOC_L2C_CLEAN_INV_INDEX_UNION;
#endif
#define SOC_L2C_CLEAN_INV_INDEX_C_START         (0)
#define SOC_L2C_CLEAN_INV_INDEX_C_END           (0)
#define SOC_L2C_CLEAN_INV_INDEX_INDEX_START     (5)
#define SOC_L2C_CLEAN_INV_INDEX_INDEX_END       (15)
#define SOC_L2C_CLEAN_INV_INDEX_WAY_START       (28)
#define SOC_L2C_CLEAN_INV_INDEX_WAY_END         (31)


/*****************************************************************************
 结构名    : SOC_L2C_CLEAN_INV_WAY_UNION
 结构说明  : CLEAN_INV_WAY 寄存器结构定义。地址偏移量:0x7FC，初值:0x0000，宽度:32
 寄存器说明: Clean&amp;Invalidate by Way寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  WAY      : 16; /* bit[0-15] : 使能，每bit对应一个way
                                                    0: 写无效
                                                    1: 使能
                                                    注意：当完成操作后，该bit自清零 */
        unsigned int  reserved : 16; /* bit[16-31]:  */
    } reg;
} SOC_L2C_CLEAN_INV_WAY_UNION;
#endif
#define SOC_L2C_CLEAN_INV_WAY_WAY_START       (0)
#define SOC_L2C_CLEAN_INV_WAY_WAY_END         (15)


/*****************************************************************************
 结构名    : SOC_L2C_D_LOCKDOWN0_UNION
 结构说明  : D_LOCKDOWN0 寄存器结构定义。地址偏移量:0x900，初值:0x0000，宽度:32
 寄存器说明: Data Lockdown0寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  DATALOCK000 : 16; /* bit[0-15] : DATA Lock使能，当AyUSERSx[7:5]=3'b000。每bit对应每个way
                                                       0: 对应的way支持allocate
                                                       1: 对应的way不支持allocate */
        unsigned int  reserved    : 16; /* bit[16-31]:  */
    } reg;
} SOC_L2C_D_LOCKDOWN0_UNION;
#endif
#define SOC_L2C_D_LOCKDOWN0_DATALOCK000_START  (0)
#define SOC_L2C_D_LOCKDOWN0_DATALOCK000_END    (15)


/*****************************************************************************
 结构名    : SOC_L2C_I_LOCKDOWN0_UNION
 结构说明  : I_LOCKDOWN0 寄存器结构定义。地址偏移量:0x904，初值:0x0000，宽度:32
 寄存器说明: 指令Lockdown寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  INSLOCK000 : 16; /* bit[0-15] : Istruction Lock使能，当AyUSERSx[7:5]=3'b000。每bit对应每个way
                                                      0: 对应的way支持allocate
                                                      1: 对应的way不支持allocate */
        unsigned int  reserved   : 16; /* bit[16-31]:  */
    } reg;
} SOC_L2C_I_LOCKDOWN0_UNION;
#endif
#define SOC_L2C_I_LOCKDOWN0_INSLOCK000_START  (0)
#define SOC_L2C_I_LOCKDOWN0_INSLOCK000_END    (15)


/*****************************************************************************
 结构名    : SOC_L2C_D_LOCKDOWN1_UNION
 结构说明  : D_LOCKDOWN1 寄存器结构定义。地址偏移量:0x908，初值:0x0000，宽度:32
 寄存器说明: Data Lockdown1寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  DATALOCK001 : 16; /* bit[0-15] : DATA Lock使能，当AyUSERSx[7:5]=3'b001。每bit对应每个way
                                                       0: 对应的way支持allocate
                                                       1: 对应的way不支持allocate */
        unsigned int  reserved    : 16; /* bit[16-31]:  */
    } reg;
} SOC_L2C_D_LOCKDOWN1_UNION;
#endif
#define SOC_L2C_D_LOCKDOWN1_DATALOCK001_START  (0)
#define SOC_L2C_D_LOCKDOWN1_DATALOCK001_END    (15)


/*****************************************************************************
 结构名    : SOC_L2C_I_LOCKDOWN1_UNION
 结构说明  : I_LOCKDOWN1 寄存器结构定义。地址偏移量:0x90C，初值:0x0000，宽度:32
 寄存器说明: 指令Lockdown1寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  INSLOCK001 : 16; /* bit[0-15] : Istruction Lock使能，当AyUSERSx[7:5]=3'b001。每bit对应每个way
                                                      0: 对应的way支持allocate
                                                      1: 对应的way不支持allocate */
        unsigned int  reserved   : 16; /* bit[16-31]:  */
    } reg;
} SOC_L2C_I_LOCKDOWN1_UNION;
#endif
#define SOC_L2C_I_LOCKDOWN1_INSLOCK001_START  (0)
#define SOC_L2C_I_LOCKDOWN1_INSLOCK001_END    (15)


/*****************************************************************************
 结构名    : SOC_L2C_D_LOCKDOWN2_UNION
 结构说明  : D_LOCKDOWN2 寄存器结构定义。地址偏移量:0x910，初值:0x0000，宽度:32
 寄存器说明: Data Lockdown2寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  DATALOCK002 : 16; /* bit[0-15] : DATA Lock使能，当AyUSERSx[7:5]=3'b010。每bit对应每个way
                                                       0: 对应的way支持allocate
                                                       1: 对应的way不支持allocate */
        unsigned int  reserved    : 16; /* bit[16-31]:  */
    } reg;
} SOC_L2C_D_LOCKDOWN2_UNION;
#endif
#define SOC_L2C_D_LOCKDOWN2_DATALOCK002_START  (0)
#define SOC_L2C_D_LOCKDOWN2_DATALOCK002_END    (15)


/*****************************************************************************
 结构名    : SOC_L2C_I_LOCKDOWN2_UNION
 结构说明  : I_LOCKDOWN2 寄存器结构定义。地址偏移量:0x914，初值:0x0000，宽度:32
 寄存器说明: 指令Lockdown2寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  INSLOCK002 : 16; /* bit[0-15] : Istruction Lock使能，当AyUSERSx[7:5]=3'b010。每bit对应每个way
                                                      0: 对应的way支持allocate
                                                      1: 对应的way不支持allocate */
        unsigned int  reserved   : 16; /* bit[16-31]:  */
    } reg;
} SOC_L2C_I_LOCKDOWN2_UNION;
#endif
#define SOC_L2C_I_LOCKDOWN2_INSLOCK002_START  (0)
#define SOC_L2C_I_LOCKDOWN2_INSLOCK002_END    (15)


/*****************************************************************************
 结构名    : SOC_L2C_D_LOCKDOWN3_UNION
 结构说明  : D_LOCKDOWN3 寄存器结构定义。地址偏移量:0x918，初值:0x0000，宽度:32
 寄存器说明: Data Lockdown3寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  DATALOCK003 : 16; /* bit[0-15] : DATA Lock使能，当AyUSERSx[7:5]=3'b011。每bit对应每个way
                                                       0: 对应的way支持allocate
                                                       1: 对应的way不支持allocate */
        unsigned int  reserved    : 16; /* bit[16-31]:  */
    } reg;
} SOC_L2C_D_LOCKDOWN3_UNION;
#endif
#define SOC_L2C_D_LOCKDOWN3_DATALOCK003_START  (0)
#define SOC_L2C_D_LOCKDOWN3_DATALOCK003_END    (15)


/*****************************************************************************
 结构名    : SOC_L2C_I_LOCKDOWN3_UNION
 结构说明  : I_LOCKDOWN3 寄存器结构定义。地址偏移量:0x91C，初值:0x0000，宽度:32
 寄存器说明: 指令Lockdown3寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  INSLOCK003 : 16; /* bit[0-15] : Istruction Lock使能，当AyUSERSx[7:5]=3'b011。每bit对应每个way
                                                      0: 对应的way支持allocate
                                                      1: 对应的way不支持allocate */
        unsigned int  reserved   : 16; /* bit[16-31]:  */
    } reg;
} SOC_L2C_I_LOCKDOWN3_UNION;
#endif
#define SOC_L2C_I_LOCKDOWN3_INSLOCK003_START  (0)
#define SOC_L2C_I_LOCKDOWN3_INSLOCK003_END    (15)


/*****************************************************************************
 结构名    : SOC_L2C_D_LOCKDOWN4_UNION
 结构说明  : D_LOCKDOWN4 寄存器结构定义。地址偏移量:0x920，初值:0x0000，宽度:32
 寄存器说明: Data Lockdown4寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  DATALOCK004 : 16; /* bit[0-15] : DATA Lock使能，当AyUSERSx[7:5]=3'b100。每bit对应每个way
                                                       0: 对应的way支持allocate
                                                       1: 对应的way不支持allocate */
        unsigned int  reserved    : 16; /* bit[16-31]:  */
    } reg;
} SOC_L2C_D_LOCKDOWN4_UNION;
#endif
#define SOC_L2C_D_LOCKDOWN4_DATALOCK004_START  (0)
#define SOC_L2C_D_LOCKDOWN4_DATALOCK004_END    (15)


/*****************************************************************************
 结构名    : SOC_L2C_I_LOCKDOWN4_UNION
 结构说明  : I_LOCKDOWN4 寄存器结构定义。地址偏移量:0x924，初值:0x0000，宽度:32
 寄存器说明: 指令Lockdown4寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  INSLOCK004 : 16; /* bit[0-15] : Istruction Lock使能，当AyUSERSx[7:5]=3'b100。每bit对应每个way
                                                      0: 对应的way支持allocate
                                                      1: 对应的way不支持allocate */
        unsigned int  reserved   : 16; /* bit[16-31]:  */
    } reg;
} SOC_L2C_I_LOCKDOWN4_UNION;
#endif
#define SOC_L2C_I_LOCKDOWN4_INSLOCK004_START  (0)
#define SOC_L2C_I_LOCKDOWN4_INSLOCK004_END    (15)


/*****************************************************************************
 结构名    : SOC_L2C_D_LOCKDOWN5_UNION
 结构说明  : D_LOCKDOWN5 寄存器结构定义。地址偏移量:0x928，初值:0x0000，宽度:32
 寄存器说明: Data Lockdown5寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  DATALOCK005 : 16; /* bit[0-15] : DATA Lock使能，当AyUSERSx[7:5]=3'b101。每bit对应每个way
                                                       0: 对应的way支持allocate
                                                       1: 对应的way不支持allocate */
        unsigned int  reserved    : 16; /* bit[16-31]:  */
    } reg;
} SOC_L2C_D_LOCKDOWN5_UNION;
#endif
#define SOC_L2C_D_LOCKDOWN5_DATALOCK005_START  (0)
#define SOC_L2C_D_LOCKDOWN5_DATALOCK005_END    (15)


/*****************************************************************************
 结构名    : SOC_L2C_I_LOCKDOWN5_UNION
 结构说明  : I_LOCKDOWN5 寄存器结构定义。地址偏移量:0x92C，初值:0x0000，宽度:32
 寄存器说明: 指令Lockdown5寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  INSLOCK005 : 16; /* bit[0-15] : Istruction Lock使能，当AyUSERSx[7:5]=3'b101。每bit对应每个way
                                                      0: 对应的way支持allocate
                                                      1: 对应的way不支持allocate */
        unsigned int  reserved   : 16; /* bit[16-31]:  */
    } reg;
} SOC_L2C_I_LOCKDOWN5_UNION;
#endif
#define SOC_L2C_I_LOCKDOWN5_INSLOCK005_START  (0)
#define SOC_L2C_I_LOCKDOWN5_INSLOCK005_END    (15)


/*****************************************************************************
 结构名    : SOC_L2C_D_LOCKDOWN6_UNION
 结构说明  : D_LOCKDOWN6 寄存器结构定义。地址偏移量:0x930，初值:0x0000，宽度:32
 寄存器说明: Data Lockdown6寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  DATALOCK006 : 16; /* bit[0-15] : DATA Lock使能，当AyUSERSx[7:5]=3'b110。每bit对应每个way
                                                       0: 对应的way支持allocate
                                                       1: 对应的way不支持allocate */
        unsigned int  reserved    : 16; /* bit[16-31]:  */
    } reg;
} SOC_L2C_D_LOCKDOWN6_UNION;
#endif
#define SOC_L2C_D_LOCKDOWN6_DATALOCK006_START  (0)
#define SOC_L2C_D_LOCKDOWN6_DATALOCK006_END    (15)


/*****************************************************************************
 结构名    : SOC_L2C_I_LOCKDOWN6_UNION
 结构说明  : I_LOCKDOWN6 寄存器结构定义。地址偏移量:0x934，初值:0x0000，宽度:32
 寄存器说明: 指令Lockdown6寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  INSLOCK006 : 16; /* bit[0-15] : Istruction Lock使能，当AyUSERSx[7:5]=3'b110。每bit对应每个way
                                                      0: 对应的way支持allocate
                                                      1: 对应的way不支持allocate */
        unsigned int  reserved   : 16; /* bit[16-31]:  */
    } reg;
} SOC_L2C_I_LOCKDOWN6_UNION;
#endif
#define SOC_L2C_I_LOCKDOWN6_INSLOCK006_START  (0)
#define SOC_L2C_I_LOCKDOWN6_INSLOCK006_END    (15)


/*****************************************************************************
 结构名    : SOC_L2C_D_LOCKDOWN7_UNION
 结构说明  : D_LOCKDOWN7 寄存器结构定义。地址偏移量:0x938，初值:0x0000，宽度:32
 寄存器说明: Data Lockdown7寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  DATALOCK007 : 16; /* bit[0-15] : DATA Lock使能，当AyUSERSx[7:5]=3'b111。每bit对应每个way
                                                       0: 对应的way支持allocate
                                                       1: 对应的way不支持allocate */
        unsigned int  reserved    : 16; /* bit[16-31]:  */
    } reg;
} SOC_L2C_D_LOCKDOWN7_UNION;
#endif
#define SOC_L2C_D_LOCKDOWN7_DATALOCK007_START  (0)
#define SOC_L2C_D_LOCKDOWN7_DATALOCK007_END    (15)


/*****************************************************************************
 结构名    : SOC_L2C_I_LOCKDOWN7_UNION
 结构说明  : I_LOCKDOWN7 寄存器结构定义。地址偏移量:0x93C，初值:0x0000，宽度:32
 寄存器说明: 指令Lockdown7寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  INSLOCK007 : 16; /* bit[0-15] : Istruction Lock使能，当AyUSERSx[7:5]=3'b111。每bit对应每个way
                                                      0: 对应的way支持allocate
                                                      1: 对应的way不支持allocate */
        unsigned int  reserved   : 16; /* bit[16-31]:  */
    } reg;
} SOC_L2C_I_LOCKDOWN7_UNION;
#endif
#define SOC_L2C_I_LOCKDOWN7_INSLOCK007_START  (0)
#define SOC_L2C_I_LOCKDOWN7_INSLOCK007_END    (15)


/*****************************************************************************
 结构名    : SOC_L2C_LOCK_LINE_EN_UNION
 结构说明  : LOCK_LINE_EN 寄存器结构定义。地址偏移量:0x950，初值:0x0000，宽度:32
 寄存器说明: Lockdown by line使能寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ENABLE   : 1;  /* bit[0]   : Lockdown by line enable
                                                   0: diable
                                                   1: enable */
        unsigned int  reserved : 31; /* bit[1-31]:  */
    } reg;
} SOC_L2C_LOCK_LINE_EN_UNION;
#endif
#define SOC_L2C_LOCK_LINE_EN_ENABLE_START    (0)
#define SOC_L2C_LOCK_LINE_EN_ENABLE_END      (0)


/*****************************************************************************
 结构名    : SOC_L2C_UNLOCK_WAY_UNION
 结构说明  : UNLOCK_WAY 寄存器结构定义。地址偏移量:0x954，初值:0x0000，宽度:32
 寄存器说明: unlock all lines by way寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ENABLE   : 16; /* bit[0-15] : Unlock all lines by way operation。每bit针对每个way
                                                    0: disable
                                                    1: unlock操作使能 */
        unsigned int  reserved : 16; /* bit[16-31]:  */
    } reg;
} SOC_L2C_UNLOCK_WAY_UNION;
#endif
#define SOC_L2C_UNLOCK_WAY_ENABLE_START    (0)
#define SOC_L2C_UNLOCK_WAY_ENABLE_END      (15)


/*****************************************************************************
 结构名    : SOC_L2C_ADDR_FIL_START_UNION
 结构说明  : ADDR_FIL_START 寄存器结构定义。地址偏移量:0xC00，初值:0x0000，宽度:32
 寄存器说明: 地址过滤起始寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ENABLE   : 1;  /* bit[0]    : Address filtering使能
                                                    0: disable
                                                    1: enable */
        unsigned int  reserved : 19; /* bit[1-19] :  */
        unsigned int  ADDSTART : 12; /* bit[20-31]: Address filtering起始地址的高位
                                                    注意：ARM建议先配置END寄存器，再配置START寄存器 */
    } reg;
} SOC_L2C_ADDR_FIL_START_UNION;
#endif
#define SOC_L2C_ADDR_FIL_START_ENABLE_START    (0)
#define SOC_L2C_ADDR_FIL_START_ENABLE_END      (0)
#define SOC_L2C_ADDR_FIL_START_ADDSTART_START  (20)
#define SOC_L2C_ADDR_FIL_START_ADDSTART_END    (31)


/*****************************************************************************
 结构名    : SOC_L2C_ADDR_FIL_END_UNION
 结构说明  : ADDR_FIL_END 寄存器结构定义。地址偏移量:0xC04，初值:0x0000，宽度:32
 寄存器说明: 地址过滤结束寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  reserved : 20; /* bit[0-19] :  */
        unsigned int  ADDEND   : 12; /* bit[20-31]: Address filtering结束地址高位
                                                    注意：ARM建议先配置END寄存器，再配置START寄存器 */
    } reg;
} SOC_L2C_ADDR_FIL_END_UNION;
#endif
#define SOC_L2C_ADDR_FIL_END_ADDEND_START    (20)
#define SOC_L2C_ADDR_FIL_END_ADDEND_END      (31)


/*****************************************************************************
 结构名    : SOC_L2C_DEBUG_CTRL_UNION
 结构说明  : DEBUG_CTRL 寄存器结构定义。地址偏移量:0xF40，初值:0x0000，宽度:32
 寄存器说明: DEBUG控制寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  DCL      : 1;  /* bit[0]   : Disable cache linefill
                                                   0: enable cache linefills
                                                   1: disable cache linefills */
        unsigned int  DWB      : 1;  /* bit[1]   : Disable write-back, force write through
                                                   0: Enable write-back
                                                   1: force write-through */
        unsigned int  SPNIDEN  : 1;  /* bit[2]   : 返回SPNDIEN的值 */
        unsigned int  reserved : 29; /* bit[3-31]:  */
    } reg;
} SOC_L2C_DEBUG_CTRL_UNION;
#endif
#define SOC_L2C_DEBUG_CTRL_DCL_START       (0)
#define SOC_L2C_DEBUG_CTRL_DCL_END         (0)
#define SOC_L2C_DEBUG_CTRL_DWB_START       (1)
#define SOC_L2C_DEBUG_CTRL_DWB_END         (1)
#define SOC_L2C_DEBUG_CTRL_SPNIDEN_START   (2)
#define SOC_L2C_DEBUG_CTRL_SPNIDEN_END     (2)


/*****************************************************************************
 结构名    : SOC_L2C_PREFETCH_CTRL_UNION
 结构说明  : PREFETCH_CTRL 寄存器结构定义。地址偏移量:0xF60，初值:0x0000，宽度:32
 寄存器说明: Prefetch控制寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  PO       : 5;  /* bit[0-4]  : Prefetch offset
                                                    只能使用0-7,15,23,31这几个值，L2C-310不支持其他值 */
        unsigned int  reserved_0: 16; /* bit[5-20] :  */
        unsigned int  NSIDESE  : 1;  /* bit[21]   : Not same ID on exclusive sequence enable
                                                    0: non-cacheable exclusive的读写操作有同一个AXI ID
                                                    1: non-cacheable exclusive的读写操作有不同的AXI ID */
        unsigned int  reserved_1: 1;  /* bit[22]   :  */
        unsigned int  IDLE     : 1;  /* bit[23]   : Increment double linefill enable
                                                    0: L2CC不支持INCR8的64-bit read burst
                                                    1: L2CC支持INCR8的64-bit read burst */
        unsigned int  PDE      : 1;  /* bit[24]   : Prefetch drop enable
                                                    0: L2CC不丢弃prefetch read
                                                    1: L2CC在发生资源冲突的时候丢弃prefetch操作 */
        unsigned int  reserved_2: 2;  /* bit[25-26]:  */
        unsigned int  DLWRD    : 1;  /* bit[27]   : Double linefill on WRAP read disable
                                                    0: enable
                                                    1: disable */
        unsigned int  DPE      : 1;  /* bit[28]   : Data prefetch enable
                                                    0: disable
                                                    1: enable */
        unsigned int  IPE      : 1;  /* bit[29]   : Instruction prefetch enable
                                                    0: disable
                                                    1: enable */
        unsigned int  DLE      : 1;  /* bit[30]   : Double linefill enable
                                                    0: L2CC始终在read miss的时候发出4个64-bit read burst
                                                    1: L2CC在read miss的时候发出8个64-bit read burst */
        unsigned int  reserved_3: 1;  /* bit[31]   :  */
    } reg;
} SOC_L2C_PREFETCH_CTRL_UNION;
#endif
#define SOC_L2C_PREFETCH_CTRL_PO_START        (0)
#define SOC_L2C_PREFETCH_CTRL_PO_END          (4)
#define SOC_L2C_PREFETCH_CTRL_NSIDESE_START   (21)
#define SOC_L2C_PREFETCH_CTRL_NSIDESE_END     (21)
#define SOC_L2C_PREFETCH_CTRL_IDLE_START      (23)
#define SOC_L2C_PREFETCH_CTRL_IDLE_END        (23)
#define SOC_L2C_PREFETCH_CTRL_PDE_START       (24)
#define SOC_L2C_PREFETCH_CTRL_PDE_END         (24)
#define SOC_L2C_PREFETCH_CTRL_DLWRD_START     (27)
#define SOC_L2C_PREFETCH_CTRL_DLWRD_END       (27)
#define SOC_L2C_PREFETCH_CTRL_DPE_START       (28)
#define SOC_L2C_PREFETCH_CTRL_DPE_END         (28)
#define SOC_L2C_PREFETCH_CTRL_IPE_START       (29)
#define SOC_L2C_PREFETCH_CTRL_IPE_END         (29)
#define SOC_L2C_PREFETCH_CTRL_DLE_START       (30)
#define SOC_L2C_PREFETCH_CTRL_DLE_END         (30)


/*****************************************************************************
 结构名    : SOC_L2C_POWER_CTRL_UNION
 结构说明  : POWER_CTRL 寄存器结构定义。地址偏移量:0xF80，初值:0x0000，宽度:32
 寄存器说明: 功耗控制寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  SDBMODEEN : 1;  /* bit[0]   : standby mode enable
                                                    0: disable
                                                    1: enable */
        unsigned int  DCGEN     : 1;  /* bit[1]   : dynamic clock gating enable
                                                    0: disable
                                                    1: enable */
        unsigned int  reserved  : 30; /* bit[2-31]:  */
    } reg;
} SOC_L2C_POWER_CTRL_UNION;
#endif
#define SOC_L2C_POWER_CTRL_SDBMODEEN_START  (0)
#define SOC_L2C_POWER_CTRL_SDBMODEEN_END    (0)
#define SOC_L2C_POWER_CTRL_DCGEN_START      (1)
#define SOC_L2C_POWER_CTRL_DCGEN_END        (1)






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

#endif /* end of soc_l2c_interface.h */
