

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/

#ifndef __SOC_MDDRC_AXI_INTERFACE_H__
#define __SOC_MDDRC_AXI_INTERFACE_H__

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
   位域定义UNION结构:  SOC_MDDRC_AXI_AXI_RTL_INF_UNION */
#define SOC_MDDRC_AXI_AXI_RTL_INF_ADDR(base)          ((base) + (0x000))

/* 寄存器说明：配置寄存器锁定
   位域定义UNION结构:  SOC_MDDRC_AXI_AXI_CFG_LOCK_UNION */
#define SOC_MDDRC_AXI_AXI_CFG_LOCK_ADDR(base)         ((base) + (0x004))

/* 寄存器说明：模块的时钟门控
   位域定义UNION结构:  SOC_MDDRC_AXI_AXI_CKG_UNION */
#define SOC_MDDRC_AXI_AXI_CKG_ADDR(base)              ((base) + (0x008))

/* 寄存器说明：模块行为
   位域定义UNION结构:  SOC_MDDRC_AXI_AXI_ACTION_UNION */
#define SOC_MDDRC_AXI_AXI_ACTION_ADDR(base)           ((base) + (0x020))

/* 寄存器说明：读写仲裁禁止
   位域定义UNION结构:  SOC_MDDRC_AXI_AXI_RW_ARB_DIS_UNION */
#define SOC_MDDRC_AXI_AXI_RW_ARB_DIS_ADDR(base)       ((base) + (0x024))

/* 寄存器说明：地址区域映射
   位域定义UNION结构:  SOC_MDDRC_AXI_AXI_REGION_MAP_UNION */
#define SOC_MDDRC_AXI_AXI_REGION_MAP_ADDR(base, regions)  ((base) + (0x100+0x10*(regions)))

/* 寄存器说明：地址区域属性
   位域定义UNION结构:  SOC_MDDRC_AXI_AXI_REGION_ATTRIB_UNION */
#define SOC_MDDRC_AXI_AXI_REGION_ATTRIB_ADDR(base, regions)  ((base) + (0x104+0x10*(regions)))

/* 寄存器说明：地址区域地址扰码模式
   位域定义UNION结构:  SOC_MDDRC_AXI_AXI_REGION_SCRMBL_UNION */
#define SOC_MDDRC_AXI_AXI_REGION_SCRMBL_ADDR(base, regions)  ((base) + (0x108+0x10*(regions)))

/* 寄存器说明：命令优先级映射方式
   位域定义UNION结构:  SOC_MDDRC_AXI_AXI_QOS_MAP_UNION */
#define SOC_MDDRC_AXI_AXI_QOS_MAP_ADDR(base, ports)   ((base) + (0x200+0x10*(ports)))

/* 寄存器说明：写命令优先级映射表
   位域定义UNION结构:  SOC_MDDRC_AXI_AXI_QOS_WRPRI_UNION */
#define SOC_MDDRC_AXI_AXI_QOS_WRPRI_ADDR(base, ports)  ((base) + (0x204+0x10*(ports)))

/* 寄存器说明：读命令优先级映射表
   位域定义UNION结构:  SOC_MDDRC_AXI_AXI_QOS_RDPRI_UNION */
#define SOC_MDDRC_AXI_AXI_QOS_RDPRI_ADDR(base, ports)  ((base) + (0x208+0x10*(ports)))

/* 寄存器说明：优先级自适应周期映射表
   位域定义UNION结构:  SOC_MDDRC_AXI_AXI_QOS_ADPT_UNION */
#define SOC_MDDRC_AXI_AXI_QOS_ADPT_ADDR(base, ports)  ((base) + (0x20C+0x10*(ports)))

/* 寄存器说明：端口的命令OSTD限制
   位域定义UNION结构:  SOC_MDDRC_AXI_AXI_OSTD_PRT_UNION */
#define SOC_MDDRC_AXI_AXI_OSTD_PRT_ADDR(base, ports)  ((base) + (0x300+0x10*(ports)))

/* 寄存器说明：端口的命令OSTD统计状态
   位域定义UNION结构:  SOC_MDDRC_AXI_AXI_OSTD_PRT_ST_UNION */
#define SOC_MDDRC_AXI_AXI_OSTD_PRT_ST_ADDR(base, ports)  ((base) + (0x304+0x10*(ports)))

/* 寄存器说明：ECC回读回写命令配置
   位域定义UNION结构:  SOC_MDDRC_AXI_AXI_ECC_CMD_PRI_UNION */
#define SOC_MDDRC_AXI_AXI_ECC_CMD_PRI_ADDR(base, ports)  ((base) + (0x308+0x10*(ports)))

/* 寄存器说明：端口分组的命令OSTD限制
   位域定义UNION结构:  SOC_MDDRC_AXI_AXI_OSTD_GROUP_UNION */
#define SOC_MDDRC_AXI_AXI_OSTD_GROUP_ADDR(base, groups)  ((base) + (0x400+0x10*(groups)))

/* 寄存器说明：各端口分组内基于优先级的
            命令OSTD限制
   位域定义UNION结构:  SOC_MDDRC_AXI_AXI_OSTD_PRI0_UNION */
#define SOC_MDDRC_AXI_AXI_OSTD_PRI0_ADDR(base, groups)  ((base) + (0x404+0x10*(groups)))

/* 寄存器说明：各端口分组内基于优先级的
            命令OSTD限制
   位域定义UNION结构:  SOC_MDDRC_AXI_AXI_OSTD_PRI1_UNION */
#define SOC_MDDRC_AXI_AXI_OSTD_PRI1_ADDR(base, groups)  ((base) + (0x408+0x10*(groups)))

/* 寄存器说明：端口分组的命令OSTD统计状态
   位域定义UNION结构:  SOC_MDDRC_AXI_AXI_OSTD_GROUP_ST_UNION */
#define SOC_MDDRC_AXI_AXI_OSTD_GROUP_ST_ADDR(base, groups)  ((base) + (0x40C+0x10*(groups)))

/* 寄存器说明：写命令同MID命令优先级传递使能
   位域定义UNION结构:  SOC_MDDRC_AXI_AXI_PUSH_WRMID_UNION */
#define SOC_MDDRC_AXI_AXI_PUSH_WRMID_ADDR(base, ports)  ((base) + (0x500+0x10*(ports)))

/* 寄存器说明：读命令同MID命令优先级传递使能
   位域定义UNION结构:  SOC_MDDRC_AXI_AXI_PUSH_RDMID_UNION */
#define SOC_MDDRC_AXI_AXI_PUSH_RDMID_ADDR(base, ports)  ((base) + (0x504+0x10*(ports)))

/* 寄存器说明：各个端口的工作状态
   位域定义UNION结构:  SOC_MDDRC_AXI_AXI_STATUS_UNION */
#define SOC_MDDRC_AXI_AXI_STATUS_ADDR(base)           ((base) + (0x600))

/* 寄存器说明：中断状态
   位域定义UNION结构:  SOC_MDDRC_AXI_AXI_INT_STATUS_UNION */
#define SOC_MDDRC_AXI_AXI_INT_STATUS_ADDR(base)       ((base) + (0x610))





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
 结构名    : SOC_MDDRC_AXI_AXI_RTL_INF_UNION
 结构说明  : AXI_RTL_INF 寄存器结构定义。地址偏移量:0x000，初值:0x00000000，宽度:32
 寄存器说明: 代码配置信息
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  addr_width       : 6;  /* bit[0-5]  : AXI总线地址位宽。
                                                            0x00-0x1F：reserved；
                                                            0x20:32bits；
                                                            0x21:33bits；
                                                            0x22:34bits；
                                                            0x23:35bits；
                                                            0x24:36bits；
                                                            0x25:37bits；
                                                            0x26:38bits；
                                                            0x27:39bits；
                                                            0x28:40bits。 */
        unsigned int  no_of_chs        : 2;  /* bit[6-7]  : 代码宏定义的DMC（通道）个数
                                                            0x0：配置了1个DMC；
                                                            ……
                                                            0x3：配置了4个DMC。 */
        unsigned int  no_of_prts       : 4;  /* bit[8-11] : 代码宏定义的AXI端口个数：
                                                            0x0：配置了1个AXI端口；
                                                            ……
                                                            0xB：配置了12个AXI端口；
                                                            其它：reserved */
        unsigned int  no_of_exclus     : 8;  /* bit[12-19]: 可同时监视的不同exclusive命令的最大个数：
                                                            0x00：不支持exclusive命令；
                                                            0x01：1个；
                                                            0x02：2个；
                                                            ……
                                                            0x1F：31个；
                                                            0x20：32个；
                                                            其它：reserved */
        unsigned int  ecc_status       : 1;  /* bit[20]   : DMC的ECC校验功能状态：
                                                            0：关闭；
                                                            1：使能。 */
        unsigned int  reserved_0       : 3;  /* bit[21-23]: 保留。 */
        unsigned int  no_of_addr_rgns  : 4;  /* bit[24-27]: 代码宏定义的用于地址区域映射的regions个数。
                                                            0x0：配置了1个region；
                                                            0x1：配置了2个region；
                                                            ……
                                                            0xF：配置了16个region。 */
        unsigned int  no_of_ostd_group : 2;  /* bit[28-29]: 代码宏定义的用于ostd统计的端口分组的个数。
                                                            0x0：配置了1个ostd端口分组；
                                                            ……
                                                            0x3：配置了4个ostd端口分组。 */
        unsigned int  reserved_1       : 2;  /* bit[30-31]: 保留。 */
    } reg;
} SOC_MDDRC_AXI_AXI_RTL_INF_UNION;
#endif
#define SOC_MDDRC_AXI_AXI_RTL_INF_addr_width_START        (0)
#define SOC_MDDRC_AXI_AXI_RTL_INF_addr_width_END          (5)
#define SOC_MDDRC_AXI_AXI_RTL_INF_no_of_chs_START         (6)
#define SOC_MDDRC_AXI_AXI_RTL_INF_no_of_chs_END           (7)
#define SOC_MDDRC_AXI_AXI_RTL_INF_no_of_prts_START        (8)
#define SOC_MDDRC_AXI_AXI_RTL_INF_no_of_prts_END          (11)
#define SOC_MDDRC_AXI_AXI_RTL_INF_no_of_exclus_START      (12)
#define SOC_MDDRC_AXI_AXI_RTL_INF_no_of_exclus_END        (19)
#define SOC_MDDRC_AXI_AXI_RTL_INF_ecc_status_START        (20)
#define SOC_MDDRC_AXI_AXI_RTL_INF_ecc_status_END          (20)
#define SOC_MDDRC_AXI_AXI_RTL_INF_no_of_addr_rgns_START   (24)
#define SOC_MDDRC_AXI_AXI_RTL_INF_no_of_addr_rgns_END     (27)
#define SOC_MDDRC_AXI_AXI_RTL_INF_no_of_ostd_group_START  (28)
#define SOC_MDDRC_AXI_AXI_RTL_INF_no_of_ostd_group_END    (29)


/*****************************************************************************
 结构名    : SOC_MDDRC_AXI_AXI_CFG_LOCK_UNION
 结构说明  : AXI_CFG_LOCK 寄存器结构定义。地址偏移量:0x004，初值:0x00000000，宽度:32
 寄存器说明: 配置寄存器锁定
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  apb_cfg_lock : 1;  /* bit[0]   : 锁定MDDRC内所有的寄存器配置模块(AXI_IF/SEC/DMC/PUB)：
                                                       0：不锁定；
                                                       1：锁定（除本寄存器外的所有AXI_IF配置寄存器皆无法访问，可降低功耗）。 */
        unsigned int  reserved     : 31; /* bit[1-31]: 保留。 */
    } reg;
} SOC_MDDRC_AXI_AXI_CFG_LOCK_UNION;
#endif
#define SOC_MDDRC_AXI_AXI_CFG_LOCK_apb_cfg_lock_START  (0)
#define SOC_MDDRC_AXI_AXI_CFG_LOCK_apb_cfg_lock_END    (0)


/*****************************************************************************
 结构名    : SOC_MDDRC_AXI_AXI_CKG_UNION
 结构说明  : AXI_CKG 寄存器结构定义。地址偏移量:0x008，初值:0x000F1FFF，宽度:32
 寄存器说明: 模块的时钟门控
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  dyn_ckg_axi : 12; /* bit[0-11] : 各个AXI端口的动态时钟门控：
                                                       dyn_ckg_axi[n]=0：AXI端口n的时钟常开；
                                                       dyn_ckg_axi[n]=1：AXI端口n内部各模块空闲时，自动关闭时钟。 */
        unsigned int  dyn_ckg_rdr : 1;  /* bit[12]   : REORDER模块的动态时钟门控：
                                                       0：时钟常开；
                                                       1：模块空闲时，自动关闭时钟。 */
        unsigned int  reserved_0  : 3;  /* bit[13-15]: 保留。 */
        unsigned int  sta_ckg_dmc : 4;  /* bit[16-19]: 各个DMC的静态时钟门控：
                                                       sta_ckg_dmc[n]=0：关闭DMCn及对应pub的时钟；
                                                       sta_ckg_dmc[n]=1：打开DMCn及对应pub的时钟。 */
        unsigned int  reserved_1  : 12; /* bit[20-31]: 保留。 */
    } reg;
} SOC_MDDRC_AXI_AXI_CKG_UNION;
#endif
#define SOC_MDDRC_AXI_AXI_CKG_dyn_ckg_axi_START  (0)
#define SOC_MDDRC_AXI_AXI_CKG_dyn_ckg_axi_END    (11)
#define SOC_MDDRC_AXI_AXI_CKG_dyn_ckg_rdr_START  (12)
#define SOC_MDDRC_AXI_AXI_CKG_dyn_ckg_rdr_END    (12)
#define SOC_MDDRC_AXI_AXI_CKG_sta_ckg_dmc_START  (16)
#define SOC_MDDRC_AXI_AXI_CKG_sta_ckg_dmc_END    (19)


/*****************************************************************************
 结构名    : SOC_MDDRC_AXI_AXI_ACTION_UNION
 结构说明  : AXI_ACTION 寄存器结构定义。地址偏移量:0x020，初值:0x00000007，宽度:32
 寄存器说明: 模块行为
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  exclusive_en     : 1;  /* bit[0]    : exclusive命令使能：
                                                            0：禁止；
                                                            1：使能。 */
        unsigned int  rd_wrap_split_en : 1;  /* bit[1]    : wrap读命令拆分使能：
                                                            0：不拆分，由DMC完成wrap地址卷绕；
                                                            1：拆分。 */
        unsigned int  ecc_rmw_en       : 1;  /* bit[2]    : ECC“读-改-写”功能使能：
                                                            0：禁止；
                                                            1：使能。
                                                            注意：
                                                            若代码未配置使能ECC功能，则忽略此寄存器配置。 */
        unsigned int  reserved_0       : 5;  /* bit[3-7]  : 保留。 */
        unsigned int  wr_rcv_mode      : 12; /* bit[8-19] : 防死锁模式使能：
                                                            wr_rcv_mode[n]=0：端口n的防死锁模式关闭；
                                                            wr_rcv_mode[n]=1：端口n的防死锁模式使能。
                                                            注意：
                                                            1.防死锁模式使能与否取决于总线构架的需求；
                                                            2.防死锁模式使能后，对于AXI端口的写命令outstanding被限制为1。 */
        unsigned int  reserved_1       : 12; /* bit[20-31]: 保留。 */
    } reg;
} SOC_MDDRC_AXI_AXI_ACTION_UNION;
#endif
#define SOC_MDDRC_AXI_AXI_ACTION_exclusive_en_START      (0)
#define SOC_MDDRC_AXI_AXI_ACTION_exclusive_en_END        (0)
#define SOC_MDDRC_AXI_AXI_ACTION_rd_wrap_split_en_START  (1)
#define SOC_MDDRC_AXI_AXI_ACTION_rd_wrap_split_en_END    (1)
#define SOC_MDDRC_AXI_AXI_ACTION_ecc_rmw_en_START        (2)
#define SOC_MDDRC_AXI_AXI_ACTION_ecc_rmw_en_END          (2)
#define SOC_MDDRC_AXI_AXI_ACTION_wr_rcv_mode_START       (8)
#define SOC_MDDRC_AXI_AXI_ACTION_wr_rcv_mode_END         (19)


/*****************************************************************************
 结构名    : SOC_MDDRC_AXI_AXI_RW_ARB_DIS_UNION
 结构说明  : AXI_RW_ARB_DIS 寄存器结构定义。地址偏移量:0x024，初值:0x00000000，宽度:32
 寄存器说明: 读写仲裁禁止
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  rw_arb_dis : 12; /* bit[0-11] : 读写命令仲裁禁止：
                                                      0：表示禁止取消。
                                                      1：表示禁止，即反压AXI命令；
                                                      
                                                      注意，该寄存器的位数与具体项目的axi配置个数相等。 */
        unsigned int  reserved   : 20; /* bit[12-31]: 保留。 */
    } reg;
} SOC_MDDRC_AXI_AXI_RW_ARB_DIS_UNION;
#endif
#define SOC_MDDRC_AXI_AXI_RW_ARB_DIS_rw_arb_dis_START  (0)
#define SOC_MDDRC_AXI_AXI_RW_ARB_DIS_rw_arb_dis_END    (11)


/*****************************************************************************
 结构名    : SOC_MDDRC_AXI_AXI_REGION_MAP_UNION
 结构说明  : AXI_REGION_MAP 寄存器结构定义。地址偏移量:0x100+0x10*(regions)，初值:0x00000000，宽度:32
 寄存器说明: 地址区域映射
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  rgn_base_addr : 8;  /* bit[0-7]  : 当前地址区域的基地址（高八位）。
                                                         若rgn_size=0x1，则rgn_base_addr[0] 必须配置为零；
                                                         若rgn_size=0x2，则rgn_base_addr[1:0]必须配置为零；
                                                         若rgn_size=0x3，则rgn_base_addr[2:0]必须配置为零；
                                                         若rgn_size=0x4，则rgn_base_addr[3:0]必须配置为零；
                                                         若rgn_size=0x5，则rgn_base_addr[4:0]必须配置为零；
                                                         若rgn_size=0x6，则rgn_base_addr[5:0]必须配置为零；
                                                         若rgn_size=0x7，则rgn_base_addr[6:0]必须配置为零。
                                                         注意：由于只能设置地址的高八位，因此区域地址对齐的粒度会因地址位宽不同而不同。例如：
                                                         对于32bits地址，地址区域的基地址低24位为零，区域地址按16MB对齐；
                                                         对于40bits地址，地址区域的基地址低32位为零，区域地址按4GB对齐。 */
        unsigned int  rgn_size      : 3;  /* bit[8-10] : 当前地址区域的大小。
                                                         0x0： 16MB；
                                                         0x1： 32MB；
                                                         0x2： 64MB；
                                                         0x3：128MB；
                                                         0x4：256MB；
                                                         0x5：512MB；
                                                         0x6： 1GB；
                                                         0x7： 2GB；
                                                         注意：
                                                         1.以上配置以32bits地址为例，若地址位宽不同则配置含义不同。例如：
                                                         对于40bits地址：
                                                         0x3： 32GB；
                                                         2.双通道交织时，rgn_size不可配置为0；四通道交织时，rgn_size不可配置为0或1。 */
        unsigned int  reserved_0    : 1;  /* bit[11]   : 保留。 */
        unsigned int  rgn_en        : 1;  /* bit[12]   : 使能当前地址区域。
                                                         0：禁止；
                                                         1：使能。 */
        unsigned int  reserved_1    : 19; /* bit[13-31]: 保留。 */
    } reg;
} SOC_MDDRC_AXI_AXI_REGION_MAP_UNION;
#endif
#define SOC_MDDRC_AXI_AXI_REGION_MAP_rgn_base_addr_START  (0)
#define SOC_MDDRC_AXI_AXI_REGION_MAP_rgn_base_addr_END    (7)
#define SOC_MDDRC_AXI_AXI_REGION_MAP_rgn_size_START       (8)
#define SOC_MDDRC_AXI_AXI_REGION_MAP_rgn_size_END         (10)
#define SOC_MDDRC_AXI_AXI_REGION_MAP_rgn_en_START         (12)
#define SOC_MDDRC_AXI_AXI_REGION_MAP_rgn_en_END           (12)


/*****************************************************************************
 结构名    : SOC_MDDRC_AXI_AXI_REGION_ATTRIB_UNION
 结构说明  : AXI_REGION_ATTRIB 寄存器结构定义。地址偏移量:0x104+0x10*(regions)，初值:0x00010004，宽度:32
 寄存器说明: 地址区域属性
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ch_start     : 2;  /* bit[0-1]  : 当前地址区域的映射起始通道：
                                                        0x0：通道0；
                                                        0x1：通道1；
                                                        0x2：通道2；
                                                        0x3：通道3。
                                                        注意：
                                                        当映射模式为单通道时，映射起始通道可为任意通道；
                                                        当映射模式为双通道时，映射起始通道必须为通道0（0/1通道交织）或通道2（2/3通道地址交织）；
                                                        当映射模式为四通道时，映射起始通道必须为通道0（0/1/2/3通道地址交织）。 */
        unsigned int  ch_mode      : 2;  /* bit[2-3]  : 当前地址区域的通道映射模式：
                                                        0x0：不映射；
                                                        0x1：映射到单通道，地址独立；
                                                        0x2：映射到双通道，地址交织；
                                                        0x3：映射到四通道，地址交织。
                                                        注意：
                                                        试图访问未映射的地址区域的命令，会被当做错误命令处理：上报中断、记录命令信息、AXI总线返回SLVERR响应。 */
        unsigned int  ch_intlv     : 2;  /* bit[4-5]  : 当前地址区域的地址交织粒度（单通道地址独立模式下，此配置无效）：
                                                        0x0： 64Byte；
                                                        0x1：128Byte；
                                                        0x2：256Byte；
                                                        0x3： 4KByte；
                                                        注意：
                                                        1.命令以此粒度为边界，被交替分发到多个通道；
                                                        2.通道交织时，地址交织粒度必须大于等于地址对齐粒度(addr_aligned)。 */
        unsigned int  reserved_0   : 2;  /* bit[6-7]  : 保留。 */
        unsigned int  ch_offset    : 8;  /* bit[8-15] : 当前地址区域在通道内的偏移地址（高八位）。
                                                        (1)ch_mode=1，即单通道地址独立时：
                                                        若rgn_size=0x1，则ch_offset[0] 必须配置为零；
                                                        若rgn_size=0x2，则ch_offset[1:0]必须配置为零；
                                                        若rgn_size=0x3，则ch_offset[2:0]必须配置为零；
                                                        若rgn_size=0x4，则ch_offset[3:0]必须配置为零；
                                                        若rgn_size=0x5，则ch_offset[4:0]必须配置为零；
                                                        若rgn_size=0x6，则ch_offset[5:0]必须配置为零；
                                                        若rgn_size=0x7，则ch_offset[6:0]必须配置为零。
                                                        (2)ch_mode=2，即双通道地址交织时：
                                                        若rgn_size=0x2，则ch_offset[0] 必须配置为零；
                                                        若rgn_size=0x3，则ch_offset[1:0]必须配置为零；
                                                        若rgn_size=0x4，则ch_offset[2:0]必须配置为零；
                                                        若rgn_size=0x5，则ch_offset[3:0]必须配置为零；
                                                        若rgn_size=0x6，则ch_offset[4:0]必须配置为零；
                                                        若rgn_size=0x7，则ch_offset[5:0]必须配置为零；
                                                        (2)ch_mode=3，即四通道地址交织时：
                                                        若rgn_size=0x3，则ch_offset[0] 必须配置为零；
                                                        若rgn_size=0x4，则ch_offset[1:0]必须配置为零；
                                                        若rgn_size=0x5，则ch_offset[2:0]必须配置为零；
                                                        若rgn_size=0x6，则ch_offset[3:0]必须配置为零；
                                                        若rgn_size=0x7，则ch_offset[4:0]必须配置为零；
                                                        注意：
                                                        此地址用于替换通道内地址的高八位，以实现映射地址在通道内的偏移，会因地址位宽不同而不同。例如：
                                                        对于32bits地址，可替换地址ch_addr[31:24]；
                                                        对于40bits地址，可替换地址ch_addr[39:32]； */
        unsigned int  addr_aligned : 3;  /* bit[16-18]: 当前地址区域的地址边界对齐粒度：
                                                        0x0： 8Byte(只有当DMC数据位宽为64bit时，才可选择此项)；
                                                        0x1： 16Byte；
                                                        0x2： 32Byte；
                                                        0x3： 64Byte；
                                                        0x4：128Byte；
                                                        0x5：256Byte；
                                                        0x6：512Byte；
                                                        0x7： 1KByte；
                                                        注意：
                                                        1.跨越此地址对齐边界的命令会被拆分；
                                                        2.地址对齐粒度必须小于等于bank交织粒度(bnk_mod)；
                                                        3.地址交织时，地址对齐粒度必须小于等于地址交织粒度(ch_intlv)；
                                                        4.地址独立时，地址对齐粒度必须小于等于DDR器件的PAGE大小。 */
        unsigned int  reserved_1   : 1;  /* bit[19]   : 保留。 */
        unsigned int  ecc_byp      : 1;  /* bit[20]   : 当前地址区域的ECC校验BYPASS使能：
                                                        0：ECC BYPASS禁止；
                                                        1：ECC BYPASS使能。
                                                        注意：
                                                        若代码未配置使能ECC功能，则忽略此寄存器配置。 */
        unsigned int  reserved_2   : 3;  /* bit[21-23]: 保留。 */
        unsigned int  rnk_mod      : 2;  /* bit[24-25]: 当前地址区域的通道内RANK交织模式：
                                                        0x0：使用DMC内部配置；
                                                        0x1：单RANK地址独立；
                                                        0x2：双RANK地址交织；
                                                        0x3：四RANK地址交织。 */
        unsigned int  reserved_3   : 2;  /* bit[26-27]: 保留。 */
        unsigned int  bnk_mod      : 3;  /* bit[28-30]: 当前地址区域的通道内BANK交织粒度（交织粒度随器件不同而不同）：
                                                        0~7： 8Byte~1KByte( 8bit器件)；
                                                        0~7：16Byte~2KByte(16bit器件)；
                                                        0~7：32Byte~4KByte(32bit器件)；
                                                        0~7：64Byte~8KByte(64bit器件)。
                                                        注意：
                                                        bank交织粒度必须大于等于地址对齐粒度(addr_aligned)。 */
        unsigned int  reserved_4   : 1;  /* bit[31]   : 保留。 */
    } reg;
} SOC_MDDRC_AXI_AXI_REGION_ATTRIB_UNION;
#endif
#define SOC_MDDRC_AXI_AXI_REGION_ATTRIB_ch_start_START      (0)
#define SOC_MDDRC_AXI_AXI_REGION_ATTRIB_ch_start_END        (1)
#define SOC_MDDRC_AXI_AXI_REGION_ATTRIB_ch_mode_START       (2)
#define SOC_MDDRC_AXI_AXI_REGION_ATTRIB_ch_mode_END         (3)
#define SOC_MDDRC_AXI_AXI_REGION_ATTRIB_ch_intlv_START      (4)
#define SOC_MDDRC_AXI_AXI_REGION_ATTRIB_ch_intlv_END        (5)
#define SOC_MDDRC_AXI_AXI_REGION_ATTRIB_ch_offset_START     (8)
#define SOC_MDDRC_AXI_AXI_REGION_ATTRIB_ch_offset_END       (15)
#define SOC_MDDRC_AXI_AXI_REGION_ATTRIB_addr_aligned_START  (16)
#define SOC_MDDRC_AXI_AXI_REGION_ATTRIB_addr_aligned_END    (18)
#define SOC_MDDRC_AXI_AXI_REGION_ATTRIB_ecc_byp_START       (20)
#define SOC_MDDRC_AXI_AXI_REGION_ATTRIB_ecc_byp_END         (20)
#define SOC_MDDRC_AXI_AXI_REGION_ATTRIB_rnk_mod_START       (24)
#define SOC_MDDRC_AXI_AXI_REGION_ATTRIB_rnk_mod_END         (25)
#define SOC_MDDRC_AXI_AXI_REGION_ATTRIB_bnk_mod_START       (28)
#define SOC_MDDRC_AXI_AXI_REGION_ATTRIB_bnk_mod_END         (30)


/*****************************************************************************
 结构名    : SOC_MDDRC_AXI_AXI_REGION_SCRMBL_UNION
 结构说明  : AXI_REGION_SCRMBL 寄存器结构定义。地址偏移量:0x108+0x10*(regions)，初值:0x00000000，宽度:32
 寄存器说明: 地址区域地址扰码模式
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  bnk_scrmbl : 4;  /* bit[0-3]  : BANK地址扰码模式：
                                                      0：禁止；
                                                      1~15：扰码模式1~15。
                                                      注意：
                                                      此处只是扰码模式的选择，具体的加扰方式为项目定制，在RTL模块AXI_IF_SCRMBL中定制实现。 */
        unsigned int  reserved_0 : 4;  /* bit[4-7]  : 保留。 */
        unsigned int  ch_scrmbl  : 4;  /* bit[8-11] : 通道选择扰码模式：
                                                      0：禁止；
                                                      1~15：扰码模式1~15。
                                                      注意：
                                                      此处只是扰码模式的选择，具体的加扰方式为项目定制，在RTL模块AXI_IF_SCRMBL中定制实现。 */
        unsigned int  reserved_1 : 20; /* bit[12-31]: 保留。 */
    } reg;
} SOC_MDDRC_AXI_AXI_REGION_SCRMBL_UNION;
#endif
#define SOC_MDDRC_AXI_AXI_REGION_SCRMBL_bnk_scrmbl_START  (0)
#define SOC_MDDRC_AXI_AXI_REGION_SCRMBL_bnk_scrmbl_END    (3)
#define SOC_MDDRC_AXI_AXI_REGION_SCRMBL_ch_scrmbl_START   (8)
#define SOC_MDDRC_AXI_AXI_REGION_SCRMBL_ch_scrmbl_END     (11)


/*****************************************************************************
 结构名    : SOC_MDDRC_AXI_AXI_QOS_MAP_UNION
 结构说明  : AXI_QOS_MAP 寄存器结构定义。地址偏移量:0x200+0x10*(ports)，初值:0x00000000，宽度:32
 寄存器说明: 命令优先级映射方式
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  id_map_idx   : 12; /* bit[0-11] : 通过命令ID映射优先级时，命令ID选择：
                                                        id_map_idx[11:8] ：选取命令ID低16位中的任意1bit，作为idx[2]；
                                                        id_map_idx[7:4] ：选取命令ID低16位中的任意1bit，作为idx[1]；
                                                        id_map_idx[3:0] ：选取命令ID低16位中的任意1bit，作为idx[0]。
                                                        例如：
                                                        id_map_idx配置为 0x3A0，则命令ID的{ID[3],ID[10],ID[0]}拼合成索引idx[2:0]，用于从优先级查找表中映射出命令优先级。
                                                        注意：
                                                        此处的命令ID为命令的原始ID，不包含低位扩展的端口ID信息。 */
        unsigned int  pri_map_mode : 1;  /* bit[12]   : 读写命令优先级映射模式：
                                                        0：采用AxQOS[2:0]映射；
                                                        1：采用命令ID低16位中的任意3bits映射。
                                                        说明：
                                                        使用上述两种方法之一，得到映射的索引，再从优先级查找表中映射出命令优先级 */
        unsigned int  reserved_0   : 3;  /* bit[13-15]: 保留。 */
        unsigned int  pri_push_en  : 1;  /* bit[16]   : 端口优先级传递使能：
                                                        0：禁止；
                                                        1：使能。
                                                        注意：
                                                        优先级传递使能后，将端口流水线上以及外部送入的多个命令中的最高优先级，传递给当前正参与端口仲裁的命令，以帮助其快速赢得仲裁，但不改变被传递命令的自身优先级。 */
        unsigned int  push_qos_en  : 1;  /* bit[17]   : Pushed优先级传递使能：
                                                        0：禁止；
                                                        1：使能。
                                                        注意：
                                                        1.NOC总线的Pressure信号，可在命令总线反压时传递优先级，参与AXI_IF内部的优先级传递；
                                                        2.当pri_push_en==1时，此寄存器有效。 */
        unsigned int  hurry_qos_en : 1;  /* bit[18]   : Hurry优先级传递使能：
                                                        0：禁止；
                                                        1：使能。
                                                        注意：
                                                        1.NOC总线的Hurry信号，可在任何时刻传递优先级，参与AXI_IF内部的优先级传递；
                                                        2.当pri_push_en==1时，此寄存器有效。 */
        unsigned int  reserved_1   : 1;  /* bit[19]   : 保留。 */
        unsigned int  rw_arb_mode  : 1;  /* bit[20]   : 同一端口内读写命令仲裁模式：
                                                        0：基于命令读写类型：
                                                         a.先到先仲裁；
                                                         b.同时到达（或被同时被反压）时，按照先读命令后写命令的顺序轮流仲裁；
                                                        1：基于命令优先级：
                                                         a.先到先仲裁；
                                                         b.同时到达（或被同时被反压）时，则按照命令优先级仲裁；
                                                         c.若优先级相同，则按照LRU原则仲裁给上一次未获得仲裁的命令。 */
        unsigned int  qos_rever    : 1;  /* bit[21]   : QoS反转使能（用于同一端口内读写命令的优先级仲裁）：
                                                        0：axqos=0时，为最低优先级；
                                                        1：axqos=0时，为最高优先级。
                                                        注意：
                                                        当rw_arb_mode==1时，此寄存器有效。 */
        unsigned int  reserved_2   : 10; /* bit[22-31]: 保留。 */
    } reg;
} SOC_MDDRC_AXI_AXI_QOS_MAP_UNION;
#endif
#define SOC_MDDRC_AXI_AXI_QOS_MAP_id_map_idx_START    (0)
#define SOC_MDDRC_AXI_AXI_QOS_MAP_id_map_idx_END      (11)
#define SOC_MDDRC_AXI_AXI_QOS_MAP_pri_map_mode_START  (12)
#define SOC_MDDRC_AXI_AXI_QOS_MAP_pri_map_mode_END    (12)
#define SOC_MDDRC_AXI_AXI_QOS_MAP_pri_push_en_START   (16)
#define SOC_MDDRC_AXI_AXI_QOS_MAP_pri_push_en_END     (16)
#define SOC_MDDRC_AXI_AXI_QOS_MAP_push_qos_en_START   (17)
#define SOC_MDDRC_AXI_AXI_QOS_MAP_push_qos_en_END     (17)
#define SOC_MDDRC_AXI_AXI_QOS_MAP_hurry_qos_en_START  (18)
#define SOC_MDDRC_AXI_AXI_QOS_MAP_hurry_qos_en_END    (18)
#define SOC_MDDRC_AXI_AXI_QOS_MAP_rw_arb_mode_START   (20)
#define SOC_MDDRC_AXI_AXI_QOS_MAP_rw_arb_mode_END     (20)
#define SOC_MDDRC_AXI_AXI_QOS_MAP_qos_rever_START     (21)
#define SOC_MDDRC_AXI_AXI_QOS_MAP_qos_rever_END       (21)


/*****************************************************************************
 结构名    : SOC_MDDRC_AXI_AXI_QOS_WRPRI_UNION
 结构说明  : AXI_QOS_WRPRI 寄存器结构定义。地址偏移量:0x204+0x10*(ports)，初值:0x00000000，宽度:32
 寄存器说明: 写命令优先级映射表
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  wr_pri0  : 3;  /* bit[0-2]  : 当pri_map_mode==0时：wr_pri0是awqos[2:0]为0时，映射出的写命令优先级；
                                                    当pri_map_mode==1时：wr_pri0是id_map方式得到的ID索引为0时，映射出的写命令优先级。
                                                    在DDRC内部：
                                                    0x0：最高优先级；
                                                    ……
                                                    0x7：最低优先级。 */
        unsigned int  reserved_0: 1;  /* bit[3]    : 保留。 */
        unsigned int  wr_pri1  : 3;  /* bit[4-6]  : 同wr_pri0的配置 */
        unsigned int  reserved_1: 1;  /* bit[7]    : 保留。 */
        unsigned int  wr_pri2  : 3;  /* bit[8-10] : 同wr_pri0的配置 */
        unsigned int  reserved_2: 1;  /* bit[11]   : 保留。 */
        unsigned int  wr_pri3  : 3;  /* bit[12-14]: 同wr_pri0的配置 */
        unsigned int  reserved_3: 1;  /* bit[15]   : 保留。 */
        unsigned int  wr_pri4  : 3;  /* bit[16-18]: 同wr_pri0的配置 */
        unsigned int  reserved_4: 1;  /* bit[19]   : 保留。 */
        unsigned int  wr_pri5  : 3;  /* bit[20-22]: 同wr_pri0的配置 */
        unsigned int  reserved_5: 1;  /* bit[23]   : 保留。 */
        unsigned int  wr_pri6  : 3;  /* bit[24-26]: 同wr_pri0的配置 */
        unsigned int  reserved_6: 1;  /* bit[27]   : 保留。 */
        unsigned int  wr_pri7  : 3;  /* bit[28-30]: 同wr_pri0的配置 */
        unsigned int  reserved_7: 1;  /* bit[31]   : 保留。 */
    } reg;
} SOC_MDDRC_AXI_AXI_QOS_WRPRI_UNION;
#endif
#define SOC_MDDRC_AXI_AXI_QOS_WRPRI_wr_pri0_START   (0)
#define SOC_MDDRC_AXI_AXI_QOS_WRPRI_wr_pri0_END     (2)
#define SOC_MDDRC_AXI_AXI_QOS_WRPRI_wr_pri1_START   (4)
#define SOC_MDDRC_AXI_AXI_QOS_WRPRI_wr_pri1_END     (6)
#define SOC_MDDRC_AXI_AXI_QOS_WRPRI_wr_pri2_START   (8)
#define SOC_MDDRC_AXI_AXI_QOS_WRPRI_wr_pri2_END     (10)
#define SOC_MDDRC_AXI_AXI_QOS_WRPRI_wr_pri3_START   (12)
#define SOC_MDDRC_AXI_AXI_QOS_WRPRI_wr_pri3_END     (14)
#define SOC_MDDRC_AXI_AXI_QOS_WRPRI_wr_pri4_START   (16)
#define SOC_MDDRC_AXI_AXI_QOS_WRPRI_wr_pri4_END     (18)
#define SOC_MDDRC_AXI_AXI_QOS_WRPRI_wr_pri5_START   (20)
#define SOC_MDDRC_AXI_AXI_QOS_WRPRI_wr_pri5_END     (22)
#define SOC_MDDRC_AXI_AXI_QOS_WRPRI_wr_pri6_START   (24)
#define SOC_MDDRC_AXI_AXI_QOS_WRPRI_wr_pri6_END     (26)
#define SOC_MDDRC_AXI_AXI_QOS_WRPRI_wr_pri7_START   (28)
#define SOC_MDDRC_AXI_AXI_QOS_WRPRI_wr_pri7_END     (30)


/*****************************************************************************
 结构名    : SOC_MDDRC_AXI_AXI_QOS_RDPRI_UNION
 结构说明  : AXI_QOS_RDPRI 寄存器结构定义。地址偏移量:0x208+0x10*(ports)，初值:0x00000000，宽度:32
 寄存器说明: 读命令优先级映射表
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  rd_pri0  : 3;  /* bit[0-2]  : 当pri_map_mode==0时：rd_pri0是arqos[2:0]为0时，映射出的读命令优先级；
                                                    当pri_map_mode==1时：rd_pri0是id_map方式得到的ID索引为0时，映射出的读命令优先级。
                                                    在DDRC内部：
                                                    0x0：最高优先级；
                                                    ……
                                                    0x7：最低优先级。 */
        unsigned int  reserved_0: 1;  /* bit[3]    : 保留。 */
        unsigned int  rd_pri1  : 3;  /* bit[4-6]  : 同rd_pri0的配置 */
        unsigned int  reserved_1: 1;  /* bit[7]    : 保留。 */
        unsigned int  rd_pri2  : 3;  /* bit[8-10] : 同rd_pri0的配置 */
        unsigned int  reserved_2: 1;  /* bit[11]   : 保留。 */
        unsigned int  rd_pri3  : 3;  /* bit[12-14]: 同rd_pri0的配置 */
        unsigned int  reserved_3: 1;  /* bit[15]   : 保留。 */
        unsigned int  rd_pri4  : 3;  /* bit[16-18]: 同rd_pri0的配置 */
        unsigned int  reserved_4: 1;  /* bit[19]   : 保留。 */
        unsigned int  rd_pri5  : 3;  /* bit[20-22]: 同rd_pri0的配置 */
        unsigned int  reserved_5: 1;  /* bit[23]   : 保留。 */
        unsigned int  rd_pri6  : 3;  /* bit[24-26]: 同rd_pri0的配置 */
        unsigned int  reserved_6: 1;  /* bit[27]   : 保留。 */
        unsigned int  rd_pri7  : 3;  /* bit[28-30]: 同rd_pri0的配置 */
        unsigned int  reserved_7: 1;  /* bit[31]   : 保留。 */
    } reg;
} SOC_MDDRC_AXI_AXI_QOS_RDPRI_UNION;
#endif
#define SOC_MDDRC_AXI_AXI_QOS_RDPRI_rd_pri0_START   (0)
#define SOC_MDDRC_AXI_AXI_QOS_RDPRI_rd_pri0_END     (2)
#define SOC_MDDRC_AXI_AXI_QOS_RDPRI_rd_pri1_START   (4)
#define SOC_MDDRC_AXI_AXI_QOS_RDPRI_rd_pri1_END     (6)
#define SOC_MDDRC_AXI_AXI_QOS_RDPRI_rd_pri2_START   (8)
#define SOC_MDDRC_AXI_AXI_QOS_RDPRI_rd_pri2_END     (10)
#define SOC_MDDRC_AXI_AXI_QOS_RDPRI_rd_pri3_START   (12)
#define SOC_MDDRC_AXI_AXI_QOS_RDPRI_rd_pri3_END     (14)
#define SOC_MDDRC_AXI_AXI_QOS_RDPRI_rd_pri4_START   (16)
#define SOC_MDDRC_AXI_AXI_QOS_RDPRI_rd_pri4_END     (18)
#define SOC_MDDRC_AXI_AXI_QOS_RDPRI_rd_pri5_START   (20)
#define SOC_MDDRC_AXI_AXI_QOS_RDPRI_rd_pri5_END     (22)
#define SOC_MDDRC_AXI_AXI_QOS_RDPRI_rd_pri6_START   (24)
#define SOC_MDDRC_AXI_AXI_QOS_RDPRI_rd_pri6_END     (26)
#define SOC_MDDRC_AXI_AXI_QOS_RDPRI_rd_pri7_START   (28)
#define SOC_MDDRC_AXI_AXI_QOS_RDPRI_rd_pri7_END     (30)


/*****************************************************************************
 结构名    : SOC_MDDRC_AXI_AXI_QOS_ADPT_UNION
 结构说明  : AXI_QOS_ADPT 寄存器结构定义。地址偏移量:0x20C+0x10*(ports)，初值:0x00000000，宽度:32
 寄存器说明: 优先级自适应周期映射表
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  rd_adpt_low  : 4;  /* bit[0-3]  : 读命令优先级自适应周期配置（低档位）
                                                        0x0：禁止优先级自适应功能；
                                                        0x1～0xF：N×16个时钟周期。
                                                        说明：
                                                        每当自适应周期到达后，命令优先级自动提升一级，直到提升至次最高优先级。 */
        unsigned int  rd_adpt_lvl  : 3;  /* bit[4-6]  : 读命令优先级自适应周期配置水线
                                                        0x0～0x1：reserved；
                                                        0x2～0x7：命令优先级高于等于此水线时，使用高档位配置，否则使用低档位配置。
                                                        举例：
                                                        0x5：优先级2～5的读命令使用rd_pri_adpt_high，优先级6～7的读命令使用rd_pri_adpt_low。 */
        unsigned int  reserved_0   : 1;  /* bit[7]    : 保留。 */
        unsigned int  rd_adpt_high : 4;  /* bit[8-11] : 读命令优先级自适应周期配置（高档位）
                                                        同rd_pri_adpt_low的配置。 */
        unsigned int  reserved_1   : 4;  /* bit[12-15]: 保留。 */
        unsigned int  wr_adpt_low  : 4;  /* bit[16-19]: 写命令优先级自适应周期配置（低档位）
                                                        同rd_pri_adpt_low的配置。 */
        unsigned int  wr_adpt_lvl  : 3;  /* bit[20-22]: 写命令优先级自适应周期配置水线
                                                        同rd_pri_adpt_lvl的配置。 */
        unsigned int  reserved_2   : 1;  /* bit[23]   : 保留。 */
        unsigned int  wr_adpt_high : 4;  /* bit[24-27]: 写命令优先级自适应周期配置（高档位）
                                                        同rd_pri_adpt_low的配置。 */
        unsigned int  reserved_3   : 4;  /* bit[28-31]: 保留。 */
    } reg;
} SOC_MDDRC_AXI_AXI_QOS_ADPT_UNION;
#endif
#define SOC_MDDRC_AXI_AXI_QOS_ADPT_rd_adpt_low_START   (0)
#define SOC_MDDRC_AXI_AXI_QOS_ADPT_rd_adpt_low_END     (3)
#define SOC_MDDRC_AXI_AXI_QOS_ADPT_rd_adpt_lvl_START   (4)
#define SOC_MDDRC_AXI_AXI_QOS_ADPT_rd_adpt_lvl_END     (6)
#define SOC_MDDRC_AXI_AXI_QOS_ADPT_rd_adpt_high_START  (8)
#define SOC_MDDRC_AXI_AXI_QOS_ADPT_rd_adpt_high_END    (11)
#define SOC_MDDRC_AXI_AXI_QOS_ADPT_wr_adpt_low_START   (16)
#define SOC_MDDRC_AXI_AXI_QOS_ADPT_wr_adpt_low_END     (19)
#define SOC_MDDRC_AXI_AXI_QOS_ADPT_wr_adpt_lvl_START   (20)
#define SOC_MDDRC_AXI_AXI_QOS_ADPT_wr_adpt_lvl_END     (22)
#define SOC_MDDRC_AXI_AXI_QOS_ADPT_wr_adpt_high_START  (24)
#define SOC_MDDRC_AXI_AXI_QOS_ADPT_wr_adpt_high_END    (27)


/*****************************************************************************
 结构名    : SOC_MDDRC_AXI_AXI_OSTD_PRT_UNION
 结构说明  : AXI_OSTD_PRT 寄存器结构定义。地址偏移量:0x300+0x10*(ports)，初值:0x00000000，宽度:32
 寄存器说明: 端口的命令OSTD限制
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  prt_ostd_lvl : 7;  /* bit[0-6] : 端口的命令OSTD水线：
                                                       0x00：不限制本端口的命令OSTD个数；
                                                       0x01～0x7F：当前端口允许的最大命令OSTD个数。
                                                       注意：
                                                       1.此处的OSTD个数，本质上式后级模块QOSBUF中的命令个数（下同）；
                                                       2.水线设置不能大于参数配置的QOSBUF的命令队列深度（下同）。 */
        unsigned int  reserved     : 25; /* bit[7-31]: 保留。 */
    } reg;
} SOC_MDDRC_AXI_AXI_OSTD_PRT_UNION;
#endif
#define SOC_MDDRC_AXI_AXI_OSTD_PRT_prt_ostd_lvl_START  (0)
#define SOC_MDDRC_AXI_AXI_OSTD_PRT_prt_ostd_lvl_END    (6)


/*****************************************************************************
 结构名    : SOC_MDDRC_AXI_AXI_OSTD_PRT_ST_UNION
 结构说明  : AXI_OSTD_PRT_ST 寄存器结构定义。地址偏移量:0x304+0x10*(ports)，初值:0x00000000，宽度:32
 寄存器说明: 端口的命令OSTD统计状态
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  prt_ostd_st : 7;  /* bit[0-6] : 端口的命令OSTD统计状态。 */
        unsigned int  reserved    : 25; /* bit[7-31]: 保留。 */
    } reg;
} SOC_MDDRC_AXI_AXI_OSTD_PRT_ST_UNION;
#endif
#define SOC_MDDRC_AXI_AXI_OSTD_PRT_ST_prt_ostd_st_START  (0)
#define SOC_MDDRC_AXI_AXI_OSTD_PRT_ST_prt_ostd_st_END    (6)


/*****************************************************************************
 结构名    : SOC_MDDRC_AXI_AXI_ECC_CMD_PRI_UNION
 结构说明  : AXI_ECC_CMD_PRI 寄存器结构定义。地址偏移量:0x308+0x10*(ports)，初值:0x00000000，宽度:32
 寄存器说明: ECC回读回写命令配置
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  ecc_cmd_pri : 3;  /* bit[0-2] : ECC回读优先级配置。该寄存器会作用于非对齐ECC的回读和回写，同时还作用于narrow ECC的回读。Narrow命令的回写使用命令本身的优先级。 */
        unsigned int  reserved    : 1;  /* bit[3]   : 保留。 */
        unsigned int  ecc_cmd_id  : 28; /* bit[4-31]: ECC回读命令的ID的配置(不包括mid和port id)。
                                                      
                                                      注意：axi的id最多支持到24bit,具体多少根据define的配置而定。 */
    } reg;
} SOC_MDDRC_AXI_AXI_ECC_CMD_PRI_UNION;
#endif
#define SOC_MDDRC_AXI_AXI_ECC_CMD_PRI_ecc_cmd_pri_START  (0)
#define SOC_MDDRC_AXI_AXI_ECC_CMD_PRI_ecc_cmd_pri_END    (2)
#define SOC_MDDRC_AXI_AXI_ECC_CMD_PRI_ecc_cmd_id_START   (4)
#define SOC_MDDRC_AXI_AXI_ECC_CMD_PRI_ecc_cmd_id_END     (31)


/*****************************************************************************
 结构名    : SOC_MDDRC_AXI_AXI_OSTD_GROUP_UNION
 结构说明  : AXI_OSTD_GROUP 寄存器结构定义。地址偏移量:0x400+0x10*(groups)，初值:0x00000000，宽度:32
 寄存器说明: 端口分组的命令OSTD限制
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  group_ostd_lvl : 7;  /* bit[0-6]  : 分组的命令OSTD水线：
                                                          0x00：不限制本分组的命令OSTD个数；
                                                          0x01～0x7F：当前分组允许的最大累积命令OSTD个数。
                                                          注意：
                                                          1.分组命令OSTD是当前分组所选定端口的命令OSTD之和；
                                                          2.超出水线后，分组内所有端口都会被阻塞。 */
        unsigned int  reserved_0     : 9;  /* bit[7-15] : 保留。 */
        unsigned int  group_ostd_sel : 12; /* bit[16-27]: 分组的端口选择：
                                                          group0_ostd_sel[n]=0：端口n未被选定；
                                                          group0_ostd_sel[n]=1：端口n被选定。
                                                          注意：
                                                          只有被选定的端口才参与分组的OSTD统计。 */
        unsigned int  reserved_1     : 4;  /* bit[28-31]: 保留。 */
    } reg;
} SOC_MDDRC_AXI_AXI_OSTD_GROUP_UNION;
#endif
#define SOC_MDDRC_AXI_AXI_OSTD_GROUP_group_ostd_lvl_START  (0)
#define SOC_MDDRC_AXI_AXI_OSTD_GROUP_group_ostd_lvl_END    (6)
#define SOC_MDDRC_AXI_AXI_OSTD_GROUP_group_ostd_sel_START  (16)
#define SOC_MDDRC_AXI_AXI_OSTD_GROUP_group_ostd_sel_END    (27)


/*****************************************************************************
 结构名    : SOC_MDDRC_AXI_AXI_OSTD_PRI0_UNION
 结构说明  : AXI_OSTD_PRI0 寄存器结构定义。地址偏移量:0x404+0x10*(groups)，初值:0x00000000，宽度:32
 寄存器说明: 各端口分组内基于优先级的
            命令OSTD限制
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  reserved_0    : 8;  /* bit[0-7]  : 保留。 */
        unsigned int  pri1_ostd_lvl : 7;  /* bit[8-14] : 优先级1的命令OSTD水线：
                                                         0x00：不限制选定分组的累积命令OSTD个数；
                                                         0x01～0x7F：选定分组的累积命令OSTD个数达到此水线时，只允许优先级高于1的命令通过。
                                                         注意：
                                                         1.命令OSTD的统计和限制都是基于端口分组的；
                                                         2.若使能优先级传递功能，则低优先级命令可能会因为优先级被提升而不再被阻塞。 */
        unsigned int  reserved_1    : 1;  /* bit[15]   : 保留。 */
        unsigned int  pri2_ostd_lvl : 7;  /* bit[16-22]: 优先级2的命令OSTD水线：
                                                         配置方式同pri0_ostd_lvl。 */
        unsigned int  reserved_2    : 1;  /* bit[23]   : 保留。 */
        unsigned int  pri3_ostd_lvl : 7;  /* bit[24-30]: 优先级3的命令OSTD水线：
                                                         配置方式同pri0_ostd_lvl。 */
        unsigned int  reserved_3    : 1;  /* bit[31]   : 保留。 */
    } reg;
} SOC_MDDRC_AXI_AXI_OSTD_PRI0_UNION;
#endif
#define SOC_MDDRC_AXI_AXI_OSTD_PRI0_pri1_ostd_lvl_START  (8)
#define SOC_MDDRC_AXI_AXI_OSTD_PRI0_pri1_ostd_lvl_END    (14)
#define SOC_MDDRC_AXI_AXI_OSTD_PRI0_pri2_ostd_lvl_START  (16)
#define SOC_MDDRC_AXI_AXI_OSTD_PRI0_pri2_ostd_lvl_END    (22)
#define SOC_MDDRC_AXI_AXI_OSTD_PRI0_pri3_ostd_lvl_START  (24)
#define SOC_MDDRC_AXI_AXI_OSTD_PRI0_pri3_ostd_lvl_END    (30)


/*****************************************************************************
 结构名    : SOC_MDDRC_AXI_AXI_OSTD_PRI1_UNION
 结构说明  : AXI_OSTD_PRI1 寄存器结构定义。地址偏移量:0x408+0x10*(groups)，初值:0x00000000，宽度:32
 寄存器说明: 各端口分组内基于优先级的
            命令OSTD限制
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  pri4_ostd_lvl : 7;  /* bit[0-6]  : 优先级4的命令OSTD水线：
                                                         配置方式同pri0_ostd_lvl。 */
        unsigned int  reserved_0    : 1;  /* bit[7]    : 保留。 */
        unsigned int  pri5_ostd_lvl : 7;  /* bit[8-14] : 优先级5的命令OSTD水线：
                                                         配置方式同pri0_ostd_lvl。 */
        unsigned int  reserved_1    : 1;  /* bit[15]   : 保留。 */
        unsigned int  pri6_ostd_lvl : 7;  /* bit[16-22]: 优先级6的命令OSTD水线：
                                                         配置方式同pri0_ostd_lvl。 */
        unsigned int  reserved_2    : 1;  /* bit[23]   : 保留。 */
        unsigned int  pri7_ostd_lvl : 7;  /* bit[24-30]: 优先级7的命令OSTD水线：
                                                         配置方式同pri0_ostd_lvl。 */
        unsigned int  reserved_3    : 1;  /* bit[31]   : 保留。 */
    } reg;
} SOC_MDDRC_AXI_AXI_OSTD_PRI1_UNION;
#endif
#define SOC_MDDRC_AXI_AXI_OSTD_PRI1_pri4_ostd_lvl_START  (0)
#define SOC_MDDRC_AXI_AXI_OSTD_PRI1_pri4_ostd_lvl_END    (6)
#define SOC_MDDRC_AXI_AXI_OSTD_PRI1_pri5_ostd_lvl_START  (8)
#define SOC_MDDRC_AXI_AXI_OSTD_PRI1_pri5_ostd_lvl_END    (14)
#define SOC_MDDRC_AXI_AXI_OSTD_PRI1_pri6_ostd_lvl_START  (16)
#define SOC_MDDRC_AXI_AXI_OSTD_PRI1_pri6_ostd_lvl_END    (22)
#define SOC_MDDRC_AXI_AXI_OSTD_PRI1_pri7_ostd_lvl_START  (24)
#define SOC_MDDRC_AXI_AXI_OSTD_PRI1_pri7_ostd_lvl_END    (30)


/*****************************************************************************
 结构名    : SOC_MDDRC_AXI_AXI_OSTD_GROUP_ST_UNION
 结构说明  : AXI_OSTD_GROUP_ST 寄存器结构定义。地址偏移量:0x40C+0x10*(groups)，初值:0x00000000，宽度:32
 寄存器说明: 端口分组的命令OSTD统计状态
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  group_ostd_st : 7;  /* bit[0-6] : 端口分组的命令OSTD统计状态。 */
        unsigned int  reserved      : 25; /* bit[7-31]: 保留。 */
    } reg;
} SOC_MDDRC_AXI_AXI_OSTD_GROUP_ST_UNION;
#endif
#define SOC_MDDRC_AXI_AXI_OSTD_GROUP_ST_group_ostd_st_START  (0)
#define SOC_MDDRC_AXI_AXI_OSTD_GROUP_ST_group_ostd_st_END    (6)


/*****************************************************************************
 结构名    : SOC_MDDRC_AXI_AXI_PUSH_WRMID_UNION
 结构说明  : AXI_PUSH_WRMID 寄存器结构定义。地址偏移量:0x500+0x10*(ports)，初值:0x00000000，宽度:32
 寄存器说明: 写命令同MID命令优先级传递使能
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  wr_mid_sel : 32; /* bit[0-31]: 写命令MID（最低五位）的比特匹配选择。
                                                     wr_mid_sel[n]=0：对于MID=n的命令，在QOSBUF中禁止同MID写命令优先级传递功能；
                                                     wr_mid_sel[n]=1：对于MID=n的命令，在QOSBUF中使能同MID写命令优先级传递功能。 */
    } reg;
} SOC_MDDRC_AXI_AXI_PUSH_WRMID_UNION;
#endif
#define SOC_MDDRC_AXI_AXI_PUSH_WRMID_wr_mid_sel_START  (0)
#define SOC_MDDRC_AXI_AXI_PUSH_WRMID_wr_mid_sel_END    (31)


/*****************************************************************************
 结构名    : SOC_MDDRC_AXI_AXI_PUSH_RDMID_UNION
 结构说明  : AXI_PUSH_RDMID 寄存器结构定义。地址偏移量:0x504+0x10*(ports)，初值:0x00000000，宽度:32
 寄存器说明: 读命令同MID命令优先级传递使能
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  rd_mid_sel : 32; /* bit[0-31]: 读命令MID（最低五位）的比特匹配选择。
                                                     rd_mid_sel[n]=0：对于MID=n的命令，在QOSBUF中禁止同MID读命令优先级传递功能；
                                                     rd_mid_sel[n]=1：对于MID=n的命令，在QOSBUF中使能同MID读命令优先级传递功能。 */
    } reg;
} SOC_MDDRC_AXI_AXI_PUSH_RDMID_UNION;
#endif
#define SOC_MDDRC_AXI_AXI_PUSH_RDMID_rd_mid_sel_START  (0)
#define SOC_MDDRC_AXI_AXI_PUSH_RDMID_rd_mid_sel_END    (31)


/*****************************************************************************
 结构名    : SOC_MDDRC_AXI_AXI_STATUS_UNION
 结构说明  : AXI_STATUS 寄存器结构定义。地址偏移量:0x600，初值:0x00000000，宽度:32
 寄存器说明: 各个端口的工作状态
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  axi_if_busy : 12; /* bit[0-11] : 各个AXI端口的工作状态：
                                                       axi_prt_busy[n]=0：AXI端口n空闲；
                                                       axi_prt_busy[n]=1：AXI端口n正在处理命令或数据； */
        unsigned int  reserved    : 20; /* bit[12-31]: 保留。 */
    } reg;
} SOC_MDDRC_AXI_AXI_STATUS_UNION;
#endif
#define SOC_MDDRC_AXI_AXI_STATUS_axi_if_busy_START  (0)
#define SOC_MDDRC_AXI_AXI_STATUS_axi_if_busy_END    (11)


/*****************************************************************************
 结构名    : SOC_MDDRC_AXI_AXI_INT_STATUS_UNION
 结构说明  : AXI_INT_STATUS 寄存器结构定义。地址偏移量:0x610，初值:0x00000000，宽度:32
 寄存器说明: 中断状态
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  int_ports : 12; /* bit[0-11] : 中断源指示：
                                                     interrupt_ports[n]=1：中断来自于端口n。
                                                     注意：
                                                     当命令未通过安全权限检查且中断使能时，相应端口会产生中断。 */
        unsigned int  reserved  : 20; /* bit[12-31]: 保留。 */
    } reg;
} SOC_MDDRC_AXI_AXI_INT_STATUS_UNION;
#endif
#define SOC_MDDRC_AXI_AXI_INT_STATUS_int_ports_START  (0)
#define SOC_MDDRC_AXI_AXI_INT_STATUS_int_ports_END    (11)






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

#endif /* end of soc_mddrc_axi_interface.h */
