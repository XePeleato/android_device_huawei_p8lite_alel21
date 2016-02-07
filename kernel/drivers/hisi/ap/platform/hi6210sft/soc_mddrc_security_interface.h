

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/

#ifndef __SOC_MDDRC_SECURITY_INTERFACE_H__
#define __SOC_MDDRC_SECURITY_INTERFACE_H__

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
/* 寄存器说明：代码配置信息
   位域定义UNION结构:  SOC_MDDRC_SECURITY_SEC_RTL_INF_UNION */
#define SOC_MDDRC_SECURITY_SEC_RTL_INF_ADDR(base)     ((base) + (0x000))

/* 寄存器说明：安全功能使能
   位域定义UNION结构:  SOC_MDDRC_SECURITY_SEC_BYPASS_UNION */
#define SOC_MDDRC_SECURITY_SEC_BYPASS_ADDR(base)      ((base) + (0x004))

/* 寄存器说明：锁定安全属性相关配置寄存器
            （当secur_boot_lock信号被拉高超过一个时钟周期时，此寄存器自身及其选定的相关寄存器被锁定为只读，只可通过系统复位解除锁定）
   位域定义UNION结构:  SOC_MDDRC_SECURITY_SEC_LOCKDOWN_SELECT_UNION */
#define SOC_MDDRC_SECURITY_SEC_LOCKDOWN_SELECT_ADDR(base) ((base) + (0x010))

/* 寄存器说明：中断使能
   位域定义UNION结构:  SOC_MDDRC_SECURITY_SEC_INT_EN_UNION */
#define SOC_MDDRC_SECURITY_SEC_INT_EN_ADDR(base)      ((base) + (0x020))

/* 寄存器说明：中断状态
   位域定义UNION结构:  SOC_MDDRC_SECURITY_SEC_INT_STATUS_UNION */
#define SOC_MDDRC_SECURITY_SEC_INT_STATUS_ADDR(base)  ((base) + (0x024))

/* 寄存器说明：中断清除
   位域定义UNION结构:  SOC_MDDRC_SECURITY_SEC_INT_CLEAR_UNION */
#define SOC_MDDRC_SECURITY_SEC_INT_CLEAR_ADDR(base)   ((base) + (0x028))

/* 寄存器说明：记录首个越权命令的信息0
   位域定义UNION结构:  SOC_MDDRC_SECURITY_SEC_FAIL_CMD_INF_0_UNION */
#define SOC_MDDRC_SECURITY_SEC_FAIL_CMD_INF_0_ADDR(base) ((base) + (0x080))

/* 寄存器说明：记录首个越权命令的信息1
   位域定义UNION结构:  SOC_MDDRC_SECURITY_SEC_FAIL_CMD_INF_1_UNION */
#define SOC_MDDRC_SECURITY_SEC_FAIL_CMD_INF_1_ADDR(base) ((base) + (0x084))

/* 寄存器说明：记录首个越权命令的信息2
   位域定义UNION结构:  SOC_MDDRC_SECURITY_SEC_FAIL_CMD_INF_2_UNION */
#define SOC_MDDRC_SECURITY_SEC_FAIL_CMD_INF_2_ADDR(base) ((base) + (0x088))

/* 寄存器说明：地址区域映射
            （被锁定条件：sucure_boot_lock信号被拉高过，且ld_region_map配置为1，且对应region在ld_regions的选定范围内。）
   位域定义UNION结构:  SOC_MDDRC_SECURITY_SEC_RGN_MAP_UNION */
#define SOC_MDDRC_SECURITY_SEC_RGN_MAP_ADDR(base, regions)  ((base) + (0x100+0x10*(regions)))

/* 寄存器说明：各地址区域的权限属性设置
            （被锁定条件：sucure_boot_lock信号被拉高过，且ld_region_attrib配置为1，且对应region在ld_regions的选定范围内。）
   位域定义UNION结构:  SOC_MDDRC_SECURITY_SEC_RGN_ATTRIB_UNION */
#define SOC_MDDRC_SECURITY_SEC_RGN_ATTRIB_ADDR(base, regions)  ((base) + (0x104+0x10*(regions)))

/* 寄存器说明：读命令匹配MasterID及其掩码
            （被锁定条件：sucure_boot_lock信号被拉高过，且ld_mid_wr配置为1，且对应region在ld_regions的选定范围内。）
   位域定义UNION结构:  SOC_MDDRC_SECURITY_SEC_MID_WR_UNION */
#define SOC_MDDRC_SECURITY_SEC_MID_WR_ADDR(base, regions)  ((base) + (0x108+0x10*(regions)))

/* 寄存器说明：读命令匹配MasterID及其掩码
            （被锁定条件：sucure_boot_lock信号被拉高过，且ld_mid_rd配置为1，且对应region在ld_regions的选定范围内。）
   位域定义UNION结构:  SOC_MDDRC_SECURITY_SEC_MID_RD_UNION */
#define SOC_MDDRC_SECURITY_SEC_MID_RD_ADDR(base, regions)  ((base) + (0x10C+0x10*(regions)))

/* 寄存器说明：集成测试控制寄存器
   位域定义UNION结构:  SOC_MDDRC_SECURITY_SEC_ITCRG_UNION */
#define SOC_MDDRC_SECURITY_SEC_ITCRG_ADDR(base)       ((base) + (0x3C0))

/* 寄存器说明：集成测试输入寄存器
   位域定义UNION结构:  SOC_MDDRC_SECURITY_SEC_ITIP_UNION */
#define SOC_MDDRC_SECURITY_SEC_ITIP_ADDR(base)        ((base) + (0x3C4))

/* 寄存器说明：集成测试输出寄存器
   位域定义UNION结构:  SOC_MDDRC_SECURITY_SEC_ITOP_UNION */
#define SOC_MDDRC_SECURITY_SEC_ITOP_ADDR(base)        ((base) + (0x3C8))





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
 结构名    : SOC_MDDRC_SECURITY_SEC_RTL_INF_UNION
 结构说明  : SEC_RTL_INF 寄存器结构定义。地址偏移量:0x000，初值:0x0000000F，宽度:32
 寄存器说明: 代码配置信息
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  no_of_rgns : 8;  /* bit[0-7]  : 代码宏定义的regions个数。
                                                      8'd0：reserved；
                                                      8'd1：reserved；
                                                      8'd2：2regions；
                                                      8'd3：3regions；
                                                      …… */
        unsigned int  subrgn_en  : 1;  /* bit[8]    : 代码宏定义的sub_region功能是否使能。
                                                      0：无sub_region功能；
                                                      1：有sub_region功能。 */
        unsigned int  reserved   : 19; /* bit[9-27] : 保留。 */
        unsigned int  version    : 4;  /* bit[28-31]: 4'b0010：安全模块第二版 */
    } reg;
} SOC_MDDRC_SECURITY_SEC_RTL_INF_UNION;
#endif
#define SOC_MDDRC_SECURITY_SEC_RTL_INF_no_of_rgns_START  (0)
#define SOC_MDDRC_SECURITY_SEC_RTL_INF_no_of_rgns_END    (7)
#define SOC_MDDRC_SECURITY_SEC_RTL_INF_subrgn_en_START   (8)
#define SOC_MDDRC_SECURITY_SEC_RTL_INF_subrgn_en_END     (8)
#define SOC_MDDRC_SECURITY_SEC_RTL_INF_version_START     (28)
#define SOC_MDDRC_SECURITY_SEC_RTL_INF_version_END       (31)


/*****************************************************************************
 结构名    : SOC_MDDRC_SECURITY_SEC_BYPASS_UNION
 结构说明  : SEC_BYPASS 寄存器结构定义。地址偏移量:0x004，初值:0x00000001，宽度:32
 寄存器说明: 安全功能使能
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  bypass   : 1;  /* bit[0]   : 安全模块bypass。
                                                   0：安全模块使能；
                                                   1：安全模块bypass。
                                                   被锁定条件：sucure_boot_lock信号被拉高过，且ld_bypass配置为1。 */
        unsigned int  reserved : 31; /* bit[1-31]: 保留。 */
    } reg;
} SOC_MDDRC_SECURITY_SEC_BYPASS_UNION;
#endif
#define SOC_MDDRC_SECURITY_SEC_BYPASS_bypass_START    (0)
#define SOC_MDDRC_SECURITY_SEC_BYPASS_bypass_END      (0)


/*****************************************************************************
 结构名    : SOC_MDDRC_SECURITY_SEC_LOCKDOWN_SELECT_UNION
 结构说明  : SEC_LOCKDOWN_SELECT 寄存器结构定义。地址偏移量:0x010，初值:0x00000000，宽度:32
 寄存器说明: 锁定安全属性相关配置寄存器
            （当secur_boot_lock信号被拉高超过一个时钟周期时，此寄存器自身及其选定的相关寄存器被锁定为只读，只可通过系统复位解除锁定）
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ld_rgn_map    : 1;  /* bit[0]    : 锁定选定region的SEC_REGION_MAP寄存器为只读。
                                                         0：不锁定；
                                                         1：锁定。
                                                         被锁定条件：sucure_boot_lock信号被拉高过。 */
        unsigned int  ld_rgn_attrib : 1;  /* bit[1]    : 锁定选定region的SEC_REGION_ATTRIB寄存器为只读。
                                                         0：不锁定；
                                                         1：锁定。
                                                         被锁定条件：sucure_boot_lock信号被拉高过。 */
        unsigned int  ld_mid_rd     : 1;  /* bit[2]    : 锁定选定region的SEC_MID_RD寄存器为只读。
                                                         0：不锁定；
                                                         1：锁定。
                                                         被锁定条件：sucure_boot_lock信号被拉高过。 */
        unsigned int  ld_mid_wr     : 1;  /* bit[3]    : 锁定选定region的SEC_MID_WR寄存器为只读。
                                                         0：不锁定；
                                                         1：锁定。
                                                         被锁定条件：sucure_boot_lock信号被拉高过。 */
        unsigned int  ld_bypass     : 1;  /* bit[4]    : 锁定SEC_BYPASS寄存器为只读。
                                                         0：不锁定；
                                                         1：锁定。
                                                         被锁定条件：sucure_boot_lock信号被拉高过。 */
        unsigned int  reserved_0    : 11; /* bit[5-15] : 保留。 */
        unsigned int  ld_rgns_sel   : 8;  /* bit[16-23]: 选择需将相关配置寄存器锁定为只读的regions范围。
                                                         ld_rgns_sel=N：从最高位起选定连续的N+1个rgn。
                                                         举例：
                                                         ld_rgns_sel=5：rgn15~rgn10的相关寄存器被选定（假设no_of_rgns=16）。
                                                         注意：
                                                         1.若选定的rgn个数大于实际rgn总数no_of_rgns，则选定全部rgn；
                                                         2.若某rgn被选定，则此rgn对应的如下相关配置寄存器皆被选定：
                                                         （1）SEC_rgn_MAP；
                                                         （2）SEC_rgn_ATTRIB；
                                                         （3）SEC_MID_WR；
                                                         （4）SEC_MID_RD；
                                                         被锁定条件：sucure_boot_lock信号被拉高过，且ld_rgn_register配置为1。 */
        unsigned int  reserved_1    : 8;  /* bit[24-31]: 保留。 */
    } reg;
} SOC_MDDRC_SECURITY_SEC_LOCKDOWN_SELECT_UNION;
#endif
#define SOC_MDDRC_SECURITY_SEC_LOCKDOWN_SELECT_ld_rgn_map_START     (0)
#define SOC_MDDRC_SECURITY_SEC_LOCKDOWN_SELECT_ld_rgn_map_END       (0)
#define SOC_MDDRC_SECURITY_SEC_LOCKDOWN_SELECT_ld_rgn_attrib_START  (1)
#define SOC_MDDRC_SECURITY_SEC_LOCKDOWN_SELECT_ld_rgn_attrib_END    (1)
#define SOC_MDDRC_SECURITY_SEC_LOCKDOWN_SELECT_ld_mid_rd_START      (2)
#define SOC_MDDRC_SECURITY_SEC_LOCKDOWN_SELECT_ld_mid_rd_END        (2)
#define SOC_MDDRC_SECURITY_SEC_LOCKDOWN_SELECT_ld_mid_wr_START      (3)
#define SOC_MDDRC_SECURITY_SEC_LOCKDOWN_SELECT_ld_mid_wr_END        (3)
#define SOC_MDDRC_SECURITY_SEC_LOCKDOWN_SELECT_ld_bypass_START      (4)
#define SOC_MDDRC_SECURITY_SEC_LOCKDOWN_SELECT_ld_bypass_END        (4)
#define SOC_MDDRC_SECURITY_SEC_LOCKDOWN_SELECT_ld_rgns_sel_START    (16)
#define SOC_MDDRC_SECURITY_SEC_LOCKDOWN_SELECT_ld_rgns_sel_END      (23)


/*****************************************************************************
 结构名    : SOC_MDDRC_SECURITY_SEC_INT_EN_UNION
 结构说明  : SEC_INT_EN 寄存器结构定义。地址偏移量:0x020，初值:0x00000001，宽度:32
 寄存器说明: 中断使能
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  int_en   : 1;  /* bit[0]   : 中断使能。
                                                   0：禁止；
                                                   1：使能。
                                                   注意：只是禁止中断输出，但相关寄存器还是会正常记录中断状态及越权命令信息。 */
        unsigned int  reserved : 31; /* bit[1-31]: 保留。 */
    } reg;
} SOC_MDDRC_SECURITY_SEC_INT_EN_UNION;
#endif
#define SOC_MDDRC_SECURITY_SEC_INT_EN_int_en_START    (0)
#define SOC_MDDRC_SECURITY_SEC_INT_EN_int_en_END      (0)


/*****************************************************************************
 结构名    : SOC_MDDRC_SECURITY_SEC_INT_STATUS_UNION
 结构说明  : SEC_INT_STATUS 寄存器结构定义。地址偏移量:0x024，初值:0x00000000，宽度:32
 寄存器说明: 中断状态
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  status   : 1;  /* bit[0]   : 中断状态指示
                                                   0：中断未产生；
                                                   1：中断已产生。
                                                   注意：从AXI_IF模块的AXI_INT_STATUS寄存器中，可识别出中断来自于哪个AXI端口的SEC模块 */
        unsigned int  overrun  : 1;  /* bit[1]   : 多次中断指示
                                                   0：中断未产生或只产生了一次；
                                                   1：中断已产生多次。 */
        unsigned int  reserved : 30; /* bit[2-31]: 保留。 */
    } reg;
} SOC_MDDRC_SECURITY_SEC_INT_STATUS_UNION;
#endif
#define SOC_MDDRC_SECURITY_SEC_INT_STATUS_status_START    (0)
#define SOC_MDDRC_SECURITY_SEC_INT_STATUS_status_END      (0)
#define SOC_MDDRC_SECURITY_SEC_INT_STATUS_overrun_START   (1)
#define SOC_MDDRC_SECURITY_SEC_INT_STATUS_overrun_END     (1)


/*****************************************************************************
 结构名    : SOC_MDDRC_SECURITY_SEC_INT_CLEAR_UNION
 结构说明  : SEC_INT_CLEAR 寄存器结构定义。地址偏移量:0x028，初值:0x00000000，宽度:32
 寄存器说明: 中断清除
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  clear : 32; /* bit[0-31]: 写入任意值，可清零中断状态寄存器int_status。 */
    } reg;
} SOC_MDDRC_SECURITY_SEC_INT_CLEAR_UNION;
#endif
#define SOC_MDDRC_SECURITY_SEC_INT_CLEAR_clear_START  (0)
#define SOC_MDDRC_SECURITY_SEC_INT_CLEAR_clear_END    (31)


/*****************************************************************************
 结构名    : SOC_MDDRC_SECURITY_SEC_FAIL_CMD_INF_0_UNION
 结构说明  : SEC_FAIL_CMD_INF_0 寄存器结构定义。地址偏移量:0x080，初值:0x00000000，宽度:32
 寄存器说明: 记录首个越权命令的信息0
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  address_low : 32; /* bit[0-31]: 中断状态清除前的首个越权命令的低32位地址 */
    } reg;
} SOC_MDDRC_SECURITY_SEC_FAIL_CMD_INF_0_UNION;
#endif
#define SOC_MDDRC_SECURITY_SEC_FAIL_CMD_INF_0_address_low_START  (0)
#define SOC_MDDRC_SECURITY_SEC_FAIL_CMD_INF_0_address_low_END    (31)


/*****************************************************************************
 结构名    : SOC_MDDRC_SECURITY_SEC_FAIL_CMD_INF_1_UNION
 结构说明  : SEC_FAIL_CMD_INF_1 寄存器结构定义。地址偏移量:0x084，初值:0x00000000，宽度:32
 寄存器说明: 记录首个越权命令的信息1
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  address_high : 8;  /* bit[0-7]  : 中断状态清除前的首个越权命令的高位地址（如果存在高位地址） */
        unsigned int  reserved_0   : 8;  /* bit[8-15] : 保留。 */
        unsigned int  privileged   : 1;  /* bit[16]   : 中断状态清除前首个越权命令的特权模式信息
                                                        0：unprivileged访问；
                                                        1：privileged访问。 */
        unsigned int  nonsecure    : 1;  /* bit[17]   : 中断状态清除前首个越权命令的安全模式信息
                                                        0：secure访问；
                                                        1：non-secure访问。 */
        unsigned int  reserved_1   : 2;  /* bit[18-19]: 保留。 */
        unsigned int  access_type  : 1;  /* bit[20]   : 中断状态清除前首个越权命令的读写模式信息
                                                        0：read访问；
                                                        1：write访问。 */
        unsigned int  reserved_2   : 11; /* bit[21-31]: 保留。 */
    } reg;
} SOC_MDDRC_SECURITY_SEC_FAIL_CMD_INF_1_UNION;
#endif
#define SOC_MDDRC_SECURITY_SEC_FAIL_CMD_INF_1_address_high_START  (0)
#define SOC_MDDRC_SECURITY_SEC_FAIL_CMD_INF_1_address_high_END    (7)
#define SOC_MDDRC_SECURITY_SEC_FAIL_CMD_INF_1_privileged_START    (16)
#define SOC_MDDRC_SECURITY_SEC_FAIL_CMD_INF_1_privileged_END      (16)
#define SOC_MDDRC_SECURITY_SEC_FAIL_CMD_INF_1_nonsecure_START     (17)
#define SOC_MDDRC_SECURITY_SEC_FAIL_CMD_INF_1_nonsecure_END       (17)
#define SOC_MDDRC_SECURITY_SEC_FAIL_CMD_INF_1_access_type_START   (20)
#define SOC_MDDRC_SECURITY_SEC_FAIL_CMD_INF_1_access_type_END     (20)


/*****************************************************************************
 结构名    : SOC_MDDRC_SECURITY_SEC_FAIL_CMD_INF_2_UNION
 结构说明  : SEC_FAIL_CMD_INF_2 寄存器结构定义。地址偏移量:0x088，初值:0x00000000，宽度:32
 寄存器说明: 记录首个越权命令的信息2
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  id  : 24; /* bit[0-23] : '中断状态清除前的首个越权命令的命令ID */
        unsigned int  mid : 8;  /* bit[24-31]: 中断状态清除前的首个越权命令的命令MasterID */
    } reg;
} SOC_MDDRC_SECURITY_SEC_FAIL_CMD_INF_2_UNION;
#endif
#define SOC_MDDRC_SECURITY_SEC_FAIL_CMD_INF_2_id_START   (0)
#define SOC_MDDRC_SECURITY_SEC_FAIL_CMD_INF_2_id_END     (23)
#define SOC_MDDRC_SECURITY_SEC_FAIL_CMD_INF_2_mid_START  (24)
#define SOC_MDDRC_SECURITY_SEC_FAIL_CMD_INF_2_mid_END    (31)


/*****************************************************************************
 结构名    : SOC_MDDRC_SECURITY_SEC_RGN_MAP_UNION
 结构说明  : SEC_RGN_MAP 寄存器结构定义。地址偏移量:0x100+0x10*(regions)，初值:0x00000000，宽度:32
 寄存器说明: 地址区域映射
            （被锁定条件：sucure_boot_lock信号被拉高过，且ld_region_map配置为1，且对应region在ld_regions的选定范围内。）
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  rgn_base_addr : 24; /* bit[0-23] : 各个区域的起始地址设置（高位部分）
                                                         设置地址的[39:16]位。
                                                         注意：
                                                         1.rgn0不可配置，默认覆盖整个地址空间；
                                                         2.需保证exclusive命令访问的地址区域有响应的访问权限，否则exclusive命令无法被正常监控。 */
        unsigned int  rgn_size      : 6;  /* bit[24-29]: 当前区域的大小。
                                                         6'd15： 64KB；
                                                         6'd16：128KB；此时rgn_base_addr[ 0]必须配置为零；
                                                         6'd17：256KB；此时rgn_base_addr[ 1:0]必须配置为零；
                                                         6'd18：512KB；此时rgn_base_addr[ 2:0]必须配置为零；
                                                         6'd19： 1MB；此时rgn_base_addr[ 3:0]必须配置为零；
                                                         6'd20： 2MB；此时rgn_base_addr[ 4:0]必须配置为零；
                                                         6'd21： 4MB；此时rgn_base_addr[ 5:0]必须配置为零；
                                                         6'd22： 8MB；此时rgn_base_addr[ 6:0]必须配置为零；
                                                         6'd23： 16MB；此时rgn_base_addr[ 7:0]必须配置为零；
                                                         6'd24： 32MB；此时rgn_base_addr[ 8:0]必须配置为零；
                                                         6'd25： 64MB；此时rgn_base_addr[ 9:0]必须配置为零；
                                                         6'd26：128MB；此时rgn_base_addr[10:0]必须配置为零；
                                                         6'd27：256MB；此时rgn_base_addr[11:0]必须配置为零；
                                                         6'd28：512MB；此时rgn_base_addr[12:0]必须配置为零；
                                                         6'd29： 1GB；此时rgn_base_addr[13:0]必须配置为零；
                                                         6'd30： 2GB；此时rgn_base_addr[14:0]必须配置为零；
                                                         6'd31： 4GB；此时rgn_base_addr[15:0]必须配置为零；
                                                         6'd32： 8GB；此时rgn_base_addr[16:0]必须配置为零；
                                                         6'd33： 16GB；此时rgn_base_addr[17:0]必须配置为零；
                                                         6'd34： 32GB；此时rgn_base_addr[18:0]必须配置为零；
                                                         6'd35： 64GB；此时rgn_base_addr[19:0]必须配置为零；
                                                         6'd36：128GB；此时rgn_base_addr[20:0]必须配置为零；
                                                         6'd37：256GB；此时rgn_base_addr[21:0]必须配置为零；
                                                         6'd38：512GB；此时rgn_base_addr[22:0]必须配置为零；
                                                         6'd39：1TB； 此时rgn_base_addr[23:0]必须配置为零。
                                                         注意：
                                                         1.rgn0不可配置，默认覆盖整个地址空间；
                                                         2.为每个区域配置的区域大小，累加上其区域起始地址后，不允许超出总的地址空间大小。 */
        unsigned int  reserved      : 1;  /* bit[30]   : 保留。 */
        unsigned int  rgn_en        : 1;  /* bit[31]   : 各个区域的使能控制
                                                         0：不使能；
                                                         1：使能；
                                                         注意：region0不可配置，默认使能。 */
    } reg;
} SOC_MDDRC_SECURITY_SEC_RGN_MAP_UNION;
#endif
#define SOC_MDDRC_SECURITY_SEC_RGN_MAP_rgn_base_addr_START  (0)
#define SOC_MDDRC_SECURITY_SEC_RGN_MAP_rgn_base_addr_END    (23)
#define SOC_MDDRC_SECURITY_SEC_RGN_MAP_rgn_size_START       (24)
#define SOC_MDDRC_SECURITY_SEC_RGN_MAP_rgn_size_END         (29)
#define SOC_MDDRC_SECURITY_SEC_RGN_MAP_rgn_en_START         (31)
#define SOC_MDDRC_SECURITY_SEC_RGN_MAP_rgn_en_END           (31)


/*****************************************************************************
 结构名    : SOC_MDDRC_SECURITY_SEC_RGN_ATTRIB_UNION
 结构说明  : SEC_RGN_ATTRIB 寄存器结构定义。地址偏移量:0x104+0x10*(regions)，初值:0x00000000，宽度:32
 寄存器说明: 各地址区域的权限属性设置
            （被锁定条件：sucure_boot_lock信号被拉高过，且ld_region_attrib配置为1，且对应region在ld_regions的选定范围内。）
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  sp             : 4;  /* bit[0-3]  : 当前区域的安全权限属性。
                                                          sp[3]：安全读属性；
                                                          sp[2]：安全写属性；
                                                          sp[1]：非安全读属性；
                                                          sp[0]：非安全写属性；
                                                          sp[n]=0：禁止访问；
                                                          sp[n]=1：允许访问。 */
        unsigned int  security_inv   : 1;  /* bit[4]    : 安全权限反转使能
                                                          0：禁止（若某region允许非安全的读/写，则也必定允许安全的读/写）
                                                          1：使能（非安全读/写和安全读/写权限之间无依赖关系，需各自设定） */
        unsigned int  reserved_0     : 3;  /* bit[5-7]  : 保留。 */
        unsigned int  mid_en         : 1;  /* bit[8]    : MasterID匹配功能使能。
                                                          0：禁止；
                                                          1：使能。 */
        unsigned int  mid_inv        : 1;  /* bit[9]    : MasterID匹配权限反转
                                                          0：MID匹配成功的命令才可获得访问权限；
                                                          1：MID匹配失败的命令才可获得访问权限。 */
        unsigned int  reserved_1     : 6;  /* bit[10-15]: 保留。 */
        unsigned int  subrgn_disable : 16; /* bit[16-31]: 当前区域的16个等大的子区域屏蔽。
                                                          subrgn_disable[n]=0：子区域n使能（当前子区域内的命令权限，取决于当前rgn）；
                                                          subrgn_disable[n]=1：子区域n屏蔽（当前子区域内的命令权限，取决于更低优先级rgn）。
                                                          注意：
                                                          1.rgn0不可设置子区域，默认覆盖整个地址空间；
                                                          2.若代码配置未使能subrgn功能，则此寄存器配置无效。 */
    } reg;
} SOC_MDDRC_SECURITY_SEC_RGN_ATTRIB_UNION;
#endif
#define SOC_MDDRC_SECURITY_SEC_RGN_ATTRIB_sp_START              (0)
#define SOC_MDDRC_SECURITY_SEC_RGN_ATTRIB_sp_END                (3)
#define SOC_MDDRC_SECURITY_SEC_RGN_ATTRIB_security_inv_START    (4)
#define SOC_MDDRC_SECURITY_SEC_RGN_ATTRIB_security_inv_END      (4)
#define SOC_MDDRC_SECURITY_SEC_RGN_ATTRIB_mid_en_START          (8)
#define SOC_MDDRC_SECURITY_SEC_RGN_ATTRIB_mid_en_END            (8)
#define SOC_MDDRC_SECURITY_SEC_RGN_ATTRIB_mid_inv_START         (9)
#define SOC_MDDRC_SECURITY_SEC_RGN_ATTRIB_mid_inv_END           (9)
#define SOC_MDDRC_SECURITY_SEC_RGN_ATTRIB_subrgn_disable_START  (16)
#define SOC_MDDRC_SECURITY_SEC_RGN_ATTRIB_subrgn_disable_END    (31)


/*****************************************************************************
 结构名    : SOC_MDDRC_SECURITY_SEC_MID_WR_UNION
 结构说明  : SEC_MID_WR 寄存器结构定义。地址偏移量:0x108+0x10*(regions)，初值:0xFFFFFFFF，宽度:32
 寄存器说明: 读命令匹配MasterID及其掩码
            （被锁定条件：sucure_boot_lock信号被拉高过，且ld_mid_wr配置为1，且对应region在ld_regions的选定范围内。）
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  mid_sel_wr : 32; /* bit[0-31]: MID的比特匹配选择（写命令）。
                                                     mid_sel_wr[n]=0：写命令MID的低五位为n时，此命令为MID匹配失败；
                                                     mid_sel_wr[n]=1：写命令MID的低五位为n时，此命令为MID匹配成功。 */
    } reg;
} SOC_MDDRC_SECURITY_SEC_MID_WR_UNION;
#endif
#define SOC_MDDRC_SECURITY_SEC_MID_WR_mid_sel_wr_START  (0)
#define SOC_MDDRC_SECURITY_SEC_MID_WR_mid_sel_wr_END    (31)


/*****************************************************************************
 结构名    : SOC_MDDRC_SECURITY_SEC_MID_RD_UNION
 结构说明  : SEC_MID_RD 寄存器结构定义。地址偏移量:0x10C+0x10*(regions)，初值:0xFFFFFFFF，宽度:32
 寄存器说明: 读命令匹配MasterID及其掩码
            （被锁定条件：sucure_boot_lock信号被拉高过，且ld_mid_rd配置为1，且对应region在ld_regions的选定范围内。）
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  mid_sel_rd : 32; /* bit[0-31]: MID的比特匹配选择（读命令）。
                                                     mid_sel_wr[n]=0：读命令MID的低五位为n时，此命令为MID匹配失败；
                                                     mid_sel_wr[n]=1：读命令MID的低五位为n时，此命令为MID匹配成功。 */
    } reg;
} SOC_MDDRC_SECURITY_SEC_MID_RD_UNION;
#endif
#define SOC_MDDRC_SECURITY_SEC_MID_RD_mid_sel_rd_START  (0)
#define SOC_MDDRC_SECURITY_SEC_MID_RD_mid_sel_rd_END    (31)


/*****************************************************************************
 结构名    : SOC_MDDRC_SECURITY_SEC_ITCRG_UNION
 结构说明  : SEC_ITCRG 寄存器结构定义。地址偏移量:0x3C0，初值:0x00000000，宽度:32
 寄存器说明: 集成测试控制寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  int_test_en : 1;  /* bit[0]   : 中断测试使能。
                                                      0：禁止；
                                                      1：使能。
                                                      注意：使能此寄存器后，模块的中断输出，由SEC_ITOP寄存器配置决定。 */
        unsigned int  reserved    : 31; /* bit[1-31]: 保留。 */
    } reg;
} SOC_MDDRC_SECURITY_SEC_ITCRG_UNION;
#endif
#define SOC_MDDRC_SECURITY_SEC_ITCRG_int_test_en_START  (0)
#define SOC_MDDRC_SECURITY_SEC_ITCRG_int_test_en_END    (0)


/*****************************************************************************
 结构名    : SOC_MDDRC_SECURITY_SEC_ITIP_UNION
 结构说明  : SEC_ITIP 寄存器结构定义。地址偏移量:0x3C4，初值:0x00000000，宽度:32
 寄存器说明: 集成测试输入寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  itip_secure_boot_lock : 1;  /* bit[0]   : 寄存器保护的锁定状态指示。
                                                                0：未锁定；
                                                                1：已锁定。 */
        unsigned int  reserved              : 31; /* bit[1-31]: 保留。 */
    } reg;
} SOC_MDDRC_SECURITY_SEC_ITIP_UNION;
#endif
#define SOC_MDDRC_SECURITY_SEC_ITIP_itip_secure_boot_lock_START  (0)
#define SOC_MDDRC_SECURITY_SEC_ITIP_itip_secure_boot_lock_END    (0)


/*****************************************************************************
 结构名    : SOC_MDDRC_SECURITY_SEC_ITOP_UNION
 结构说明  : SEC_ITOP 寄存器结构定义。地址偏移量:0x3C8，初值:0x00000000，宽度:32
 寄存器说明: 集成测试输出寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  itop_int : 1;  /* bit[0]   : 中断状态设置。
                                                   0：低电平；
                                                   1：高电平。
                                                   注意：只有当中断测试使能时，此寄存器设置有效。此时忽略模块内部产生的正常中断。 */
        unsigned int  reserved : 31; /* bit[1-31]: 保留。 */
    } reg;
} SOC_MDDRC_SECURITY_SEC_ITOP_UNION;
#endif
#define SOC_MDDRC_SECURITY_SEC_ITOP_itop_int_START  (0)
#define SOC_MDDRC_SECURITY_SEC_ITOP_itop_int_END    (0)






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

#endif /* end of soc_mddrc_security_interface.h */
