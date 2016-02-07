

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/

#ifndef __SOC_MODEM_NOC_INTERFACE_H__
#define __SOC_MODEM_NOC_INTERFACE_H__

#ifdef __cplusplus
    #if __cplusplus
        extern "C" {
    #endif
#endif



/*****************************************************************************
  2 宏定义
*****************************************************************************/

/****************************************************************************
                     (1/1) SERVICE
 ****************************************************************************/
/* 寄存器说明：MDMNOC Err probe域ID
   位域定义UNION结构:  SOC_MODEM_NOC_MDMNOC_ERR_PROBE0_ID_COREID_UNION */
#define SOC_MODEM_NOC_MDMNOC_ERR_PROBE0_ID_COREID_ADDR(base) ((base) + (0x0000))

/* 寄存器说明：总线代码版本寄存器
   位域定义UNION结构:  SOC_MODEM_NOC_MDMNOC_ERR_PROBE0_ID_REVISIONID_UNION */
#define SOC_MODEM_NOC_MDMNOC_ERR_PROBE0_ID_REVISIONID_ADDR(base) ((base) + (0x0004))

/* 寄存器说明：MDMNOC Err probe使能
   位域定义UNION结构:  SOC_MODEM_NOC_MDMNOC_ERR_PROBE0_FAULTEN_UNION */
#define SOC_MODEM_NOC_MDMNOC_ERR_PROBE0_FAULTEN_ADDR(base) ((base) + (0x0008))

/* 寄存器说明：Error log有效指示寄存器
   位域定义UNION结构:  SOC_MODEM_NOC_MDMNOC_ERR_PROBE0_ERRVLD_UNION */
#define SOC_MODEM_NOC_MDMNOC_ERR_PROBE0_ERRVLD_ADDR(base) ((base) + (0x000C))

/* 寄存器说明：中断清除寄存器
   位域定义UNION结构:  SOC_MODEM_NOC_MDMNOC_ERR_PROBE0_ERRCLR_UNION */
#define SOC_MODEM_NOC_MDMNOC_ERR_PROBE0_ERRCLR_ADDR(base) ((base) + (0x0010))

/* 寄存器说明：Error log0
   位域定义UNION结构:  SOC_MODEM_NOC_MDMNOC_ERR_PROBE0_ERRLOG0_UNION */
#define SOC_MODEM_NOC_MDMNOC_ERR_PROBE0_ERRLOG0_ADDR(base) ((base) + (0x0014))

/* 寄存器说明：Error log1
   位域定义UNION结构:  SOC_MODEM_NOC_MDMNOC_ERR_PROBE0_ERRLOG1_UNION */
#define SOC_MODEM_NOC_MDMNOC_ERR_PROBE0_ERRLOG1_ADDR(base) ((base) + (0x0018))

/* 寄存器说明：Error log3
   位域定义UNION结构:  SOC_MODEM_NOC_MDMNOC_ERR_PROBE0_ERRLOG3_UNION */
#define SOC_MODEM_NOC_MDMNOC_ERR_PROBE0_ERRLOG3_ADDR(base) ((base) + (0x0020))

/* 寄存器说明：Error log7
   位域定义UNION结构:  SOC_MODEM_NOC_MDMNOC_ERR_PROBE0_ERRLOG7_UNION */
#define SOC_MODEM_NOC_MDMNOC_ERR_PROBE0_ERRLOG7_ADDR(base) ((base) + (0x0030))

/* 寄存器说明：MDMNOC Err probe域ID
   位域定义UNION结构:  SOC_MODEM_NOC_MDMNOC_ERR_PROBE1_ID_COREID_UNION */
#define SOC_MODEM_NOC_MDMNOC_ERR_PROBE1_ID_COREID_ADDR(base) ((base) + (0x0080))

/* 寄存器说明：总线代码版本寄存器
   位域定义UNION结构:  SOC_MODEM_NOC_MDMNOC_ERR_PROBE1_ID_REVISIONID_UNION */
#define SOC_MODEM_NOC_MDMNOC_ERR_PROBE1_ID_REVISIONID_ADDR(base) ((base) + (0x0084))

/* 寄存器说明：MDMNOC Err probe使能
   位域定义UNION结构:  SOC_MODEM_NOC_MDMNOC_ERR_PROBE1_FAULTEN_UNION */
#define SOC_MODEM_NOC_MDMNOC_ERR_PROBE1_FAULTEN_ADDR(base) ((base) + (0x0088))

/* 寄存器说明：Error log有效指示寄存器
   位域定义UNION结构:  SOC_MODEM_NOC_MDMNOC_ERR_PROBE1_ERRVLD_UNION */
#define SOC_MODEM_NOC_MDMNOC_ERR_PROBE1_ERRVLD_ADDR(base) ((base) + (0x008C))

/* 寄存器说明：中断清除寄存器
   位域定义UNION结构:  SOC_MODEM_NOC_MDMNOC_ERR_PROBE1_ERRCLR_UNION */
#define SOC_MODEM_NOC_MDMNOC_ERR_PROBE1_ERRCLR_ADDR(base) ((base) + (0x0090))

/* 寄存器说明：Error log0
   位域定义UNION结构:  SOC_MODEM_NOC_MDMNOC_ERR_PROBE1_ERRLOG0_UNION */
#define SOC_MODEM_NOC_MDMNOC_ERR_PROBE1_ERRLOG0_ADDR(base) ((base) + (0x0094))

/* 寄存器说明：Error log1
   位域定义UNION结构:  SOC_MODEM_NOC_MDMNOC_ERR_PROBE1_ERRLOG1_UNION */
#define SOC_MODEM_NOC_MDMNOC_ERR_PROBE1_ERRLOG1_ADDR(base) ((base) + (0x0098))

/* 寄存器说明：Error log3
   位域定义UNION结构:  SOC_MODEM_NOC_MDMNOC_ERR_PROBE1_ERRLOG3_UNION */
#define SOC_MODEM_NOC_MDMNOC_ERR_PROBE1_ERRLOG3_ADDR(base) ((base) + (0x00A0))

/* 寄存器说明：Error log7
   位域定义UNION结构:  SOC_MODEM_NOC_MDMNOC_ERR_PROBE1_ERRLOG7_UNION */
#define SOC_MODEM_NOC_MDMNOC_ERR_PROBE1_ERRLOG7_ADDR(base) ((base) + (0x00B0))

/* 寄存器说明：BBE16 QoS Generator 的域ID
   位域定义UNION结构:  SOC_MODEM_NOC_BBE16_QOSGENERATOR_ID_COREID_UNION */
#define SOC_MODEM_NOC_BBE16_QOSGENERATOR_ID_COREID_ADDR(base) ((base) + (0x0100))

/* 寄存器说明：BBE16 QoS Generator 总线代码版本寄存器
   位域定义UNION结构:  SOC_MODEM_NOC_BBE16_QOSGENERATOR_ID_REVISIONID_UNION */
#define SOC_MODEM_NOC_BBE16_QOSGENERATOR_ID_REVISIONID_ADDR(base) ((base) + (0x0104))

/* 寄存器说明：BBE16 QoS Generator 优先级寄存器
   位域定义UNION结构:  SOC_MODEM_NOC_BBE16_QOSGENERATOR_PRIORITY_UNION */
#define SOC_MODEM_NOC_BBE16_QOSGENERATOR_PRIORITY_ADDR(base) ((base) + (0x0108))

/* 寄存器说明：BBE16 QoS Generator 模式寄存器
   位域定义UNION结构:  SOC_MODEM_NOC_BBE16_QOSGENERATOR_MODE_UNION */
#define SOC_MODEM_NOC_BBE16_QOSGENERATOR_MODE_ADDR(base) ((base) + (0x010C))

/* 寄存器说明：BBE16 QoS Generator 外部控制寄存器
   位域定义UNION结构:  SOC_MODEM_NOC_BBE16_QOSGENERATOR_EXTCONTROL_UNION */
#define SOC_MODEM_NOC_BBE16_QOSGENERATOR_EXTCONTROL_ADDR(base) ((base) + (0x0118))





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
                     (1/1) SERVICE
 ****************************************************************************/
/*****************************************************************************
 结构名    : SOC_MODEM_NOC_MDMNOC_ERR_PROBE0_ID_COREID_UNION
 结构说明  : MDMNOC_ERR_PROBE0_ID_COREID 寄存器结构定义。地址偏移量:0x0000，初值:0xC3DE650D，宽度:32
 寄存器说明: MDMNOC Err probe域ID
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  core_type_id_logger0   : 8;  /* bit[0-7] : MDMNOC Err probe的域ID */
        unsigned int  core_check_sum_logger0 : 24; /* bit[8-31]: MDMNOC Err probe的参数检验和 */
    } reg;
} SOC_MODEM_NOC_MDMNOC_ERR_PROBE0_ID_COREID_UNION;
#endif
#define SOC_MODEM_NOC_MDMNOC_ERR_PROBE0_ID_COREID_core_type_id_logger0_START    (0)
#define SOC_MODEM_NOC_MDMNOC_ERR_PROBE0_ID_COREID_core_type_id_logger0_END      (7)
#define SOC_MODEM_NOC_MDMNOC_ERR_PROBE0_ID_COREID_core_check_sum_logger0_START  (8)
#define SOC_MODEM_NOC_MDMNOC_ERR_PROBE0_ID_COREID_core_check_sum_logger0_END    (31)


/*****************************************************************************
 结构名    : SOC_MODEM_NOC_MDMNOC_ERR_PROBE0_ID_REVISIONID_UNION
 结构说明  : MDMNOC_ERR_PROBE0_ID_REVISIONID 寄存器结构定义。地址偏移量:0x0004，初值:0x012D5300，宽度:32
 寄存器说明: 总线代码版本寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  user_id_logger0    : 8;  /* bit[0-7] : 用户自定义值，不在 IP 内使用 */
        unsigned int  flexnoc_id_logger0 : 24; /* bit[8-31]: 总线代码版本寄存器 */
    } reg;
} SOC_MODEM_NOC_MDMNOC_ERR_PROBE0_ID_REVISIONID_UNION;
#endif
#define SOC_MODEM_NOC_MDMNOC_ERR_PROBE0_ID_REVISIONID_user_id_logger0_START     (0)
#define SOC_MODEM_NOC_MDMNOC_ERR_PROBE0_ID_REVISIONID_user_id_logger0_END       (7)
#define SOC_MODEM_NOC_MDMNOC_ERR_PROBE0_ID_REVISIONID_flexnoc_id_logger0_START  (8)
#define SOC_MODEM_NOC_MDMNOC_ERR_PROBE0_ID_REVISIONID_flexnoc_id_logger0_END    (31)


/*****************************************************************************
 结构名    : SOC_MODEM_NOC_MDMNOC_ERR_PROBE0_FAULTEN_UNION
 结构说明  : MDMNOC_ERR_PROBE0_FAULTEN 寄存器结构定义。地址偏移量:0x0008，初值:0x00000000，宽度:32
 寄存器说明: MDMNOC Err probe使能
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  faulten_logger0 : 1;  /* bit[0]   : 使能输出中断，高有效。 */
        unsigned int  reserved        : 31; /* bit[1-31]: 保留 */
    } reg;
} SOC_MODEM_NOC_MDMNOC_ERR_PROBE0_FAULTEN_UNION;
#endif
#define SOC_MODEM_NOC_MDMNOC_ERR_PROBE0_FAULTEN_faulten_logger0_START  (0)
#define SOC_MODEM_NOC_MDMNOC_ERR_PROBE0_FAULTEN_faulten_logger0_END    (0)


/*****************************************************************************
 结构名    : SOC_MODEM_NOC_MDMNOC_ERR_PROBE0_ERRVLD_UNION
 结构说明  : MDMNOC_ERR_PROBE0_ERRVLD 寄存器结构定义。地址偏移量:0x000C，初值:0x00000000，宽度:32
 寄存器说明: Error log有效指示寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  errvld_logger0 : 1;  /* bit[0]   : Error log有效指示状态寄存器，高有效。 */
        unsigned int  reserved       : 31; /* bit[1-31]: 保留 */
    } reg;
} SOC_MODEM_NOC_MDMNOC_ERR_PROBE0_ERRVLD_UNION;
#endif
#define SOC_MODEM_NOC_MDMNOC_ERR_PROBE0_ERRVLD_errvld_logger0_START  (0)
#define SOC_MODEM_NOC_MDMNOC_ERR_PROBE0_ERRVLD_errvld_logger0_END    (0)


/*****************************************************************************
 结构名    : SOC_MODEM_NOC_MDMNOC_ERR_PROBE0_ERRCLR_UNION
 结构说明  : MDMNOC_ERR_PROBE0_ERRCLR 寄存器结构定义。地址偏移量:0x0010，初值:0x00000000，宽度:32
 寄存器说明: 中断清除寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  errclr_logger0 : 1;  /* bit[0]   : 1.写1'b1清零ErrVld寄存器（如果中断使能，同时也清除中断）；
                                                         2.写1'b1并不能存储在ErrClr寄存器中，读该寄存器，返回总是1'b0。 */
        unsigned int  reserved       : 31; /* bit[1-31]: 保留 */
    } reg;
} SOC_MODEM_NOC_MDMNOC_ERR_PROBE0_ERRCLR_UNION;
#endif
#define SOC_MODEM_NOC_MDMNOC_ERR_PROBE0_ERRCLR_errclr_logger0_START  (0)
#define SOC_MODEM_NOC_MDMNOC_ERR_PROBE0_ERRCLR_errclr_logger0_END    (0)


/*****************************************************************************
 结构名    : SOC_MODEM_NOC_MDMNOC_ERR_PROBE0_ERRLOG0_UNION
 结构说明  : MDMNOC_ERR_PROBE0_ERRLOG0 寄存器结构定义。地址偏移量:0x0014，初值:0x80000000，宽度:32
 寄存器说明: Error log0
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  lock_logger0    : 1;  /* bit[0]    : 指示Transaction是否为Lock操作，高有效； */
        unsigned int  opc_logger0     : 4;  /* bit[1-4]  : Transaction操作类型：
                                                           0&#45;->RD：INCR读操作；
                                                           1&#45;->RDW：WRAP读操作；
                                                           2&#45;->RDL：Exclusive读操作；
                                                           3&#45;->RDX：Lock读操作；
                                                           4&#45;->WR：INCR写操作；
                                                           5&#45;->WRW：WRAP写操作；
                                                           6&#45;->WRC：Exclusive写操作；
                                                           8&#45;->PRE：FIXED BURST；
                                                           9&#45;->URG：urgency packet，Error Probe不会检测到此类packet；
                                                           其它&#45;->Reserveed：非法操作； */
        unsigned int  reserved_0      : 3;  /* bit[5-7]  : 保留 */
        unsigned int  errcode_logger0 : 3;  /* bit[8-10] : 错误类型：
                                                           0&#45;->被访问slave返回Error Response；
                                                           1&#45;->master访问了总线的Reserve地址空间；
                                                           2&#45;->master发送了slave不支持的Burst类型（当前总线不会出现这种场景）；
                                                           3&#45;->master访问了掉电区域；
                                                           4&#45;->访问权限报错；
                                                           5&#45;->master访问时，收到Firewall的Hide Error Response；
                                                           6&#45;->被访问slave TimeOut，并返回Error Response；
                                                           7&#45;->none。 */
        unsigned int  reserved_1      : 5;  /* bit[11-15]: 保留 */
        unsigned int  len1_logger0    : 8;  /* bit[16-23]: 表示出错Transaction的包长，单位为Byte */
        unsigned int  reserved_2      : 7;  /* bit[24-30]: 保留 */
        unsigned int  format_logger0  : 1;  /* bit[31]   : 用于表示Error log0的格式，FlexNoC2.7以后版本固定为1'b1； */
    } reg;
} SOC_MODEM_NOC_MDMNOC_ERR_PROBE0_ERRLOG0_UNION;
#endif
#define SOC_MODEM_NOC_MDMNOC_ERR_PROBE0_ERRLOG0_lock_logger0_START     (0)
#define SOC_MODEM_NOC_MDMNOC_ERR_PROBE0_ERRLOG0_lock_logger0_END       (0)
#define SOC_MODEM_NOC_MDMNOC_ERR_PROBE0_ERRLOG0_opc_logger0_START      (1)
#define SOC_MODEM_NOC_MDMNOC_ERR_PROBE0_ERRLOG0_opc_logger0_END        (4)
#define SOC_MODEM_NOC_MDMNOC_ERR_PROBE0_ERRLOG0_errcode_logger0_START  (8)
#define SOC_MODEM_NOC_MDMNOC_ERR_PROBE0_ERRLOG0_errcode_logger0_END    (10)
#define SOC_MODEM_NOC_MDMNOC_ERR_PROBE0_ERRLOG0_len1_logger0_START     (16)
#define SOC_MODEM_NOC_MDMNOC_ERR_PROBE0_ERRLOG0_len1_logger0_END       (23)
#define SOC_MODEM_NOC_MDMNOC_ERR_PROBE0_ERRLOG0_format_logger0_START   (31)
#define SOC_MODEM_NOC_MDMNOC_ERR_PROBE0_ERRLOG0_format_logger0_END     (31)


/*****************************************************************************
 结构名    : SOC_MODEM_NOC_MDMNOC_ERR_PROBE0_ERRLOG1_UNION
 结构说明  : MDMNOC_ERR_PROBE0_ERRLOG1 寄存器结构定义。地址偏移量:0x0018，初值:0x00000000，宽度:32
 寄存器说明: Error log1
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  errlog1_logger0 : 17; /* bit[0-16] : NTTP packet header RouteId：路由信息，各bit含义如下
                                                           [16:13]&#45;->Initiator Flow ID，用于表示发起访问的master；
                                                           [12:9]&#45;->target Flow ID，用于表示被访问的slave；
                                                           [8:5]&#45;->Target SubRange，表示Target NIU的不同区间地址段；
                                                           [4:0]&#45;->Seq ID，Transaction标记； */
        unsigned int  reserved        : 15; /* bit[17-31]: 保留 */
    } reg;
} SOC_MODEM_NOC_MDMNOC_ERR_PROBE0_ERRLOG1_UNION;
#endif
#define SOC_MODEM_NOC_MDMNOC_ERR_PROBE0_ERRLOG1_errlog1_logger0_START  (0)
#define SOC_MODEM_NOC_MDMNOC_ERR_PROBE0_ERRLOG1_errlog1_logger0_END    (16)


/*****************************************************************************
 结构名    : SOC_MODEM_NOC_MDMNOC_ERR_PROBE0_ERRLOG3_UNION
 结构说明  : MDMNOC_ERR_PROBE0_ERRLOG3 寄存器结构定义。地址偏移量:0x0020，初值:0x00000000，宽度:32
 寄存器说明: Error log3
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  errlog3_logger0 : 32; /* bit[0-31]: NTTP packet header 地址信息（偏移地址） */
    } reg;
} SOC_MODEM_NOC_MDMNOC_ERR_PROBE0_ERRLOG3_UNION;
#endif
#define SOC_MODEM_NOC_MDMNOC_ERR_PROBE0_ERRLOG3_errlog3_logger0_START  (0)
#define SOC_MODEM_NOC_MDMNOC_ERR_PROBE0_ERRLOG3_errlog3_logger0_END    (31)


/*****************************************************************************
 结构名    : SOC_MODEM_NOC_MDMNOC_ERR_PROBE0_ERRLOG7_UNION
 结构说明  : MDMNOC_ERR_PROBE0_ERRLOG7 寄存器结构定义。地址偏移量:0x0030，初值:0x00000000，宽度:32
 寄存器说明: Error log7
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  errlog7_logger0 : 1;  /* bit[0]   : NTTP packet header 安全信号值 */
        unsigned int  reserved        : 31; /* bit[1-31]: 保留 */
    } reg;
} SOC_MODEM_NOC_MDMNOC_ERR_PROBE0_ERRLOG7_UNION;
#endif
#define SOC_MODEM_NOC_MDMNOC_ERR_PROBE0_ERRLOG7_errlog7_logger0_START  (0)
#define SOC_MODEM_NOC_MDMNOC_ERR_PROBE0_ERRLOG7_errlog7_logger0_END    (0)


/*****************************************************************************
 结构名    : SOC_MODEM_NOC_MDMNOC_ERR_PROBE1_ID_COREID_UNION
 结构说明  : MDMNOC_ERR_PROBE1_ID_COREID 寄存器结构定义。地址偏移量:0x0080，初值:0x1A5D840D，宽度:32
 寄存器说明: MDMNOC Err probe域ID
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  core_type_id_logger1   : 8;  /* bit[0-7] : MDMNOC Err probe的域ID */
        unsigned int  core_check_sum_logger1 : 24; /* bit[8-31]: MDMNOC Err probe的参数检验和 */
    } reg;
} SOC_MODEM_NOC_MDMNOC_ERR_PROBE1_ID_COREID_UNION;
#endif
#define SOC_MODEM_NOC_MDMNOC_ERR_PROBE1_ID_COREID_core_type_id_logger1_START    (0)
#define SOC_MODEM_NOC_MDMNOC_ERR_PROBE1_ID_COREID_core_type_id_logger1_END      (7)
#define SOC_MODEM_NOC_MDMNOC_ERR_PROBE1_ID_COREID_core_check_sum_logger1_START  (8)
#define SOC_MODEM_NOC_MDMNOC_ERR_PROBE1_ID_COREID_core_check_sum_logger1_END    (31)


/*****************************************************************************
 结构名    : SOC_MODEM_NOC_MDMNOC_ERR_PROBE1_ID_REVISIONID_UNION
 结构说明  : MDMNOC_ERR_PROBE1_ID_REVISIONID 寄存器结构定义。地址偏移量:0x0084，初值:0x012D5300，宽度:32
 寄存器说明: 总线代码版本寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  user_id_logger1    : 8;  /* bit[0-7] : 用户自定义值，不在 IP 内使用 */
        unsigned int  flexnoc_id_logger1 : 24; /* bit[8-31]: 总线代码版本寄存器 */
    } reg;
} SOC_MODEM_NOC_MDMNOC_ERR_PROBE1_ID_REVISIONID_UNION;
#endif
#define SOC_MODEM_NOC_MDMNOC_ERR_PROBE1_ID_REVISIONID_user_id_logger1_START     (0)
#define SOC_MODEM_NOC_MDMNOC_ERR_PROBE1_ID_REVISIONID_user_id_logger1_END       (7)
#define SOC_MODEM_NOC_MDMNOC_ERR_PROBE1_ID_REVISIONID_flexnoc_id_logger1_START  (8)
#define SOC_MODEM_NOC_MDMNOC_ERR_PROBE1_ID_REVISIONID_flexnoc_id_logger1_END    (31)


/*****************************************************************************
 结构名    : SOC_MODEM_NOC_MDMNOC_ERR_PROBE1_FAULTEN_UNION
 结构说明  : MDMNOC_ERR_PROBE1_FAULTEN 寄存器结构定义。地址偏移量:0x0088，初值:0x00000000，宽度:32
 寄存器说明: MDMNOC Err probe使能
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  faulten_logger1 : 1;  /* bit[0]   : 使能输出中断，高有效。 */
        unsigned int  reserved        : 31; /* bit[1-31]: 保留 */
    } reg;
} SOC_MODEM_NOC_MDMNOC_ERR_PROBE1_FAULTEN_UNION;
#endif
#define SOC_MODEM_NOC_MDMNOC_ERR_PROBE1_FAULTEN_faulten_logger1_START  (0)
#define SOC_MODEM_NOC_MDMNOC_ERR_PROBE1_FAULTEN_faulten_logger1_END    (0)


/*****************************************************************************
 结构名    : SOC_MODEM_NOC_MDMNOC_ERR_PROBE1_ERRVLD_UNION
 结构说明  : MDMNOC_ERR_PROBE1_ERRVLD 寄存器结构定义。地址偏移量:0x008C，初值:0x00000000，宽度:32
 寄存器说明: Error log有效指示寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  errvld_logger1 : 1;  /* bit[0]   : Error log有效指示状态寄存器，高有效。 */
        unsigned int  reserved       : 31; /* bit[1-31]: 保留 */
    } reg;
} SOC_MODEM_NOC_MDMNOC_ERR_PROBE1_ERRVLD_UNION;
#endif
#define SOC_MODEM_NOC_MDMNOC_ERR_PROBE1_ERRVLD_errvld_logger1_START  (0)
#define SOC_MODEM_NOC_MDMNOC_ERR_PROBE1_ERRVLD_errvld_logger1_END    (0)


/*****************************************************************************
 结构名    : SOC_MODEM_NOC_MDMNOC_ERR_PROBE1_ERRCLR_UNION
 结构说明  : MDMNOC_ERR_PROBE1_ERRCLR 寄存器结构定义。地址偏移量:0x0090，初值:0x00000000，宽度:32
 寄存器说明: 中断清除寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  errclr_logger1 : 1;  /* bit[0-0] : 1.写1'b1清零ErrVld寄存器（如果中断使能，同时也清除中断）；
                                                         2.写1'b1并不能存储在ErrClr寄存器中，读该寄存器，返回总是1'b0。 */
        unsigned int  reserved       : 31; /* bit[1-31]: 保留 */
    } reg;
} SOC_MODEM_NOC_MDMNOC_ERR_PROBE1_ERRCLR_UNION;
#endif
#define SOC_MODEM_NOC_MDMNOC_ERR_PROBE1_ERRCLR_errclr_logger1_START  (0)
#define SOC_MODEM_NOC_MDMNOC_ERR_PROBE1_ERRCLR_errclr_logger1_END    (0)


/*****************************************************************************
 结构名    : SOC_MODEM_NOC_MDMNOC_ERR_PROBE1_ERRLOG0_UNION
 结构说明  : MDMNOC_ERR_PROBE1_ERRLOG0 寄存器结构定义。地址偏移量:0x0094，初值:0x80000000，宽度:32
 寄存器说明: Error log0
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  lock_logger1    : 1;  /* bit[0]    : 指示Transaction是否为Lock操作，高有效； */
        unsigned int  opc_logger1     : 4;  /* bit[1-4]  : Transaction操作类型：
                                                           0&#45;->RD：INCR读操作；
                                                           1&#45;->RDW：WRAP读操作；
                                                           2&#45;->RDL：Exclusive读操作；
                                                           3&#45;->RDX：Lock读操作；
                                                           4&#45;->WR：INCR写操作；
                                                           5&#45;->WRW：WRAP写操作；
                                                           6&#45;->WRC：Exclusive写操作；
                                                           8&#45;->PRE：FIXED BURST；
                                                           9&#45;->URG：urgency packet，Error Probe不会检测到此类packet；
                                                           其它&#45;->Reserveed：非法操作； */
        unsigned int  reserved_0      : 3;  /* bit[5-7]  : 保留 */
        unsigned int  errcode_logger1 : 3;  /* bit[8-10] : 错误类型：
                                                           0&#45;->被访问slave返回Error Response；
                                                           1&#45;->master访问了总线的Reserve地址空间；
                                                           2&#45;->master发送了slave不支持的Burst类型（当前总线不会出现这种场景）；
                                                           3&#45;->master访问了掉电区域；
                                                           4&#45;->访问权限报错；
                                                           5&#45;->master访问时，收到Firewall的Hide Error Response；
                                                           6&#45;->被访问slave TimeOut，并返回Error Response；
                                                           7&#45;->none。 */
        unsigned int  reserved_1      : 5;  /* bit[11-15]: 保留 */
        unsigned int  len1_logger1    : 8;  /* bit[16-23]: 表示出错Transaction的包长，单位为Byte */
        unsigned int  reserved_2      : 7;  /* bit[24-30]: 保留 */
        unsigned int  format_logger1  : 1;  /* bit[31]   : 用于表示Error log0的格式，FlexNoC2.7以后版本固定为1'b1； */
    } reg;
} SOC_MODEM_NOC_MDMNOC_ERR_PROBE1_ERRLOG0_UNION;
#endif
#define SOC_MODEM_NOC_MDMNOC_ERR_PROBE1_ERRLOG0_lock_logger1_START     (0)
#define SOC_MODEM_NOC_MDMNOC_ERR_PROBE1_ERRLOG0_lock_logger1_END       (0)
#define SOC_MODEM_NOC_MDMNOC_ERR_PROBE1_ERRLOG0_opc_logger1_START      (1)
#define SOC_MODEM_NOC_MDMNOC_ERR_PROBE1_ERRLOG0_opc_logger1_END        (4)
#define SOC_MODEM_NOC_MDMNOC_ERR_PROBE1_ERRLOG0_errcode_logger1_START  (8)
#define SOC_MODEM_NOC_MDMNOC_ERR_PROBE1_ERRLOG0_errcode_logger1_END    (10)
#define SOC_MODEM_NOC_MDMNOC_ERR_PROBE1_ERRLOG0_len1_logger1_START     (16)
#define SOC_MODEM_NOC_MDMNOC_ERR_PROBE1_ERRLOG0_len1_logger1_END       (23)
#define SOC_MODEM_NOC_MDMNOC_ERR_PROBE1_ERRLOG0_format_logger1_START   (31)
#define SOC_MODEM_NOC_MDMNOC_ERR_PROBE1_ERRLOG0_format_logger1_END     (31)


/*****************************************************************************
 结构名    : SOC_MODEM_NOC_MDMNOC_ERR_PROBE1_ERRLOG1_UNION
 结构说明  : MDMNOC_ERR_PROBE1_ERRLOG1 寄存器结构定义。地址偏移量:0x0098，初值:0x00000000，宽度:32
 寄存器说明: Error log1
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  errlog1_logger1 : 17; /* bit[0-16] : NTTP packet header RouteId：路由信息，各bit含义如下
                                                           [16:13]&#45;->Initiator Flow ID，用于表示发起访问的master；
                                                           [12:9]&#45;->target Flow ID，用于表示被访问的slave；
                                                           [8:5]&#45;->Target SubRange，表示Target NIU的不同区间地址段；
                                                           [4:0]&#45;->Seq ID，Transaction标记； */
        unsigned int  reserved        : 15; /* bit[17-31]: 保留 */
    } reg;
} SOC_MODEM_NOC_MDMNOC_ERR_PROBE1_ERRLOG1_UNION;
#endif
#define SOC_MODEM_NOC_MDMNOC_ERR_PROBE1_ERRLOG1_errlog1_logger1_START  (0)
#define SOC_MODEM_NOC_MDMNOC_ERR_PROBE1_ERRLOG1_errlog1_logger1_END    (16)


/*****************************************************************************
 结构名    : SOC_MODEM_NOC_MDMNOC_ERR_PROBE1_ERRLOG3_UNION
 结构说明  : MDMNOC_ERR_PROBE1_ERRLOG3 寄存器结构定义。地址偏移量:0x00A0，初值:0x00000000，宽度:32
 寄存器说明: Error log3
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  errlog3_logger1 : 32; /* bit[0-31]: NTTP packet header 地址信息 */
    } reg;
} SOC_MODEM_NOC_MDMNOC_ERR_PROBE1_ERRLOG3_UNION;
#endif
#define SOC_MODEM_NOC_MDMNOC_ERR_PROBE1_ERRLOG3_errlog3_logger1_START  (0)
#define SOC_MODEM_NOC_MDMNOC_ERR_PROBE1_ERRLOG3_errlog3_logger1_END    (31)


/*****************************************************************************
 结构名    : SOC_MODEM_NOC_MDMNOC_ERR_PROBE1_ERRLOG7_UNION
 结构说明  : MDMNOC_ERR_PROBE1_ERRLOG7 寄存器结构定义。地址偏移量:0x00B0，初值:0x00000000，宽度:32
 寄存器说明: Error log7
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  errlog7_logger1 : 1;  /* bit[0]   : NTTP packet header 安全信号值 */
        unsigned int  reserved        : 31; /* bit[1-31]: 保留 */
    } reg;
} SOC_MODEM_NOC_MDMNOC_ERR_PROBE1_ERRLOG7_UNION;
#endif
#define SOC_MODEM_NOC_MDMNOC_ERR_PROBE1_ERRLOG7_errlog7_logger1_START  (0)
#define SOC_MODEM_NOC_MDMNOC_ERR_PROBE1_ERRLOG7_errlog7_logger1_END    (0)


/*****************************************************************************
 结构名    : SOC_MODEM_NOC_BBE16_QOSGENERATOR_ID_COREID_UNION
 结构说明  : BBE16_QOSGENERATOR_ID_COREID 寄存器结构定义。地址偏移量:0x0100，初值:0x10B8DD04，宽度:32
 寄存器说明: BBE16 QoS Generator 的域ID
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  core_type_id   : 8;  /* bit[0-7] : BBE16 QoS Generator 的域ID */
        unsigned int  core_check_sum : 24; /* bit[8-31]: BBE16 QoS Generator 的参数检验和 */
    } reg;
} SOC_MODEM_NOC_BBE16_QOSGENERATOR_ID_COREID_UNION;
#endif
#define SOC_MODEM_NOC_BBE16_QOSGENERATOR_ID_COREID_core_type_id_START    (0)
#define SOC_MODEM_NOC_BBE16_QOSGENERATOR_ID_COREID_core_type_id_END      (7)
#define SOC_MODEM_NOC_BBE16_QOSGENERATOR_ID_COREID_core_check_sum_START  (8)
#define SOC_MODEM_NOC_BBE16_QOSGENERATOR_ID_COREID_core_check_sum_END    (31)


/*****************************************************************************
 结构名    : SOC_MODEM_NOC_BBE16_QOSGENERATOR_ID_REVISIONID_UNION
 结构说明  : BBE16_QOSGENERATOR_ID_REVISIONID 寄存器结构定义。地址偏移量:0x0104，初值:0x012D5300，宽度:32
 寄存器说明: BBE16 QoS Generator 总线代码版本寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  user_id    : 8;  /* bit[0-7] : 用户自定义值，不在 IP 内使用 */
        unsigned int  flexnoc_id : 24; /* bit[8-31]: 总线代码版本寄存器 */
    } reg;
} SOC_MODEM_NOC_BBE16_QOSGENERATOR_ID_REVISIONID_UNION;
#endif
#define SOC_MODEM_NOC_BBE16_QOSGENERATOR_ID_REVISIONID_user_id_START     (0)
#define SOC_MODEM_NOC_BBE16_QOSGENERATOR_ID_REVISIONID_user_id_END       (7)
#define SOC_MODEM_NOC_BBE16_QOSGENERATOR_ID_REVISIONID_flexnoc_id_START  (8)
#define SOC_MODEM_NOC_BBE16_QOSGENERATOR_ID_REVISIONID_flexnoc_id_END    (31)


/*****************************************************************************
 结构名    : SOC_MODEM_NOC_BBE16_QOSGENERATOR_PRIORITY_UNION
 结构说明  : BBE16_QOSGENERATOR_PRIORITY 寄存器结构定义。地址偏移量:0x0108，初值:0x80000000，宽度:32
 寄存器说明: BBE16 QoS Generator 优先级寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  p0       : 3;  /* bit[0-2]  : 在 Regulator 模式，定义了低 hurry 值；在 Fixed/limiter 模式，定义了写 urgency 值 */
        unsigned int  reserved_0: 5;  /* bit[3-7]  : 保留 */
        unsigned int  p1       : 3;  /* bit[8-10] : 在 Regulator 模式，定义了高 hurry 值；在 Fixed/limiter 模式，定义了读 urgency 值 */
        unsigned int  reserved_1: 20; /* bit[11-30]: 保留 */
        unsigned int  mark     : 1;  /* bit[31]   : 向后兼容标志（为0时） */
    } reg;
} SOC_MODEM_NOC_BBE16_QOSGENERATOR_PRIORITY_UNION;
#endif
#define SOC_MODEM_NOC_BBE16_QOSGENERATOR_PRIORITY_p0_START        (0)
#define SOC_MODEM_NOC_BBE16_QOSGENERATOR_PRIORITY_p0_END          (2)
#define SOC_MODEM_NOC_BBE16_QOSGENERATOR_PRIORITY_p1_START        (8)
#define SOC_MODEM_NOC_BBE16_QOSGENERATOR_PRIORITY_p1_END          (10)
#define SOC_MODEM_NOC_BBE16_QOSGENERATOR_PRIORITY_mark_START      (31)
#define SOC_MODEM_NOC_BBE16_QOSGENERATOR_PRIORITY_mark_END        (31)


/*****************************************************************************
 结构名    : SOC_MODEM_NOC_BBE16_QOSGENERATOR_MODE_UNION
 结构说明  : BBE16_QOSGENERATOR_MODE 寄存器结构定义。地址偏移量:0x010C，初值:0x00000000，宽度:32
 寄存器说明: BBE16 QoS Generator 模式寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  mode     : 2;  /* bit[0-1] : 模式选择：0—Fixed；1—Limiter；2—Bypass；3—Regulator（此处只能配为0或2） */
        unsigned int  reserved : 30; /* bit[2-31]: 保留 */
    } reg;
} SOC_MODEM_NOC_BBE16_QOSGENERATOR_MODE_UNION;
#endif
#define SOC_MODEM_NOC_BBE16_QOSGENERATOR_MODE_mode_START      (0)
#define SOC_MODEM_NOC_BBE16_QOSGENERATOR_MODE_mode_END        (1)


/*****************************************************************************
 结构名    : SOC_MODEM_NOC_BBE16_QOSGENERATOR_EXTCONTROL_UNION
 结构说明  : BBE16_QOSGENERATOR_EXTCONTROL 寄存器结构定义。地址偏移量:0x0118，初值:0x00000000，宽度:32
 寄存器说明: BBE16 QoS Generator 外部控制寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  socket_qos_en : 1;  /* bit[0]   : 决定采用 socket 的 QoS 值还是 QoS Generator 的 QoS 值 */
        unsigned int  ext_thr_en    : 1;  /* bit[1]   : 外部阈值代替带宽寄存器设置的阈值控制高低优先级 */
        unsigned int  int_clk_en    : 1;  /* bit[2]   : 本地时钟（NIU时钟）替代参考时钟进行带宽计算 */
        unsigned int  reserved      : 29; /* bit[3-31]: 保留 */
    } reg;
} SOC_MODEM_NOC_BBE16_QOSGENERATOR_EXTCONTROL_UNION;
#endif
#define SOC_MODEM_NOC_BBE16_QOSGENERATOR_EXTCONTROL_socket_qos_en_START  (0)
#define SOC_MODEM_NOC_BBE16_QOSGENERATOR_EXTCONTROL_socket_qos_en_END    (0)
#define SOC_MODEM_NOC_BBE16_QOSGENERATOR_EXTCONTROL_ext_thr_en_START     (1)
#define SOC_MODEM_NOC_BBE16_QOSGENERATOR_EXTCONTROL_ext_thr_en_END       (1)
#define SOC_MODEM_NOC_BBE16_QOSGENERATOR_EXTCONTROL_int_clk_en_START     (2)
#define SOC_MODEM_NOC_BBE16_QOSGENERATOR_EXTCONTROL_int_clk_en_END       (2)






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

#endif /* end of soc_modem_noc_interface.h */
