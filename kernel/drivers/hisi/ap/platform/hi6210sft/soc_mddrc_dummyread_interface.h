

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/

#ifndef __SOC_MDDRC_DUMMYREAD_INTERFACE_H__
#define __SOC_MDDRC_DUMMYREAD_INTERFACE_H__

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
/* 寄存器说明：DDRC_DUMRD_CTRL是DDRC中dummy read控制寄存器。
   位域定义UNION结构:  SOC_MDDRC_dummyread_DDRC_DUMRD_CTRL_UNION */
#define SOC_MDDRC_dummyread_DDRC_DUMRD_CTRL_ADDR(base) ((base) + (0x0))

/* 寄存器说明：DDRC_DUMRD_CNT是DDRC的dummy read间隔计数寄存器。
   位域定义UNION结构:  SOC_MDDRC_dummyread_DDRC_DUMRD_CNT_UNION */
#define SOC_MDDRC_dummyread_DDRC_DUMRD_CNT_ADDR(base) ((base) + (0x4))

/* 寄存器说明：DDRC_DUMRD_NUM是DDRC的dummy read命令个数配置寄存器。
   位域定义UNION结构:  SOC_MDDRC_dummyread_DDRC_DUMRD_NUM_UNION */
#define SOC_MDDRC_dummyread_DDRC_DUMRD_NUM_ADDR(base) ((base) + (0x8))

/* 寄存器说明：DDRC_DUMRD_RNKADD是DDRC的RANK地址访问配置寄存器。
   位域定义UNION结构:  SOC_MDDRC_dummyread_DDRC_DUMRD_RNKADD_UNION */
#define SOC_MDDRC_dummyread_DDRC_DUMRD_RNKADD_ADDR(base, rnks)  ((base) + (0xC+0x4*(rnks)))

/* 寄存器说明：DDRC_DUMRD_QOS是DUMRD读命令的arqos的优先级配置
   位域定义UNION结构:  SOC_MDDRC_dummyread_DDRC_DUMRD_QOS_UNION */
#define SOC_MDDRC_dummyread_DDRC_DUMRD_QOS_ADDR(base) ((base) + (0x020))

/* 寄存器说明：DDRC_DUMRD_XID是DUMRD读命令的MID和ID配置寄存器
   位域定义UNION结构:  SOC_MDDRC_dummyread_DDRC_DUMRD_XID_UNION */
#define SOC_MDDRC_dummyread_DDRC_DUMRD_XID_ADDR(base) ((base) + (0x024))





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
 结构名    : SOC_MDDRC_dummyread_DDRC_DUMRD_CTRL_UNION
 结构说明  : DDRC_DUMRD_CTRL 寄存器结构定义。地址偏移量:0x0，初值:0x00000000，宽度:32
 寄存器说明: DDRC_DUMRD_CTRL是DDRC中dummy read控制寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  dummy_rd_en : 1;  /* bit[0]   : DDR Dummy read功能使能。
                                                      1：dummy read功能使能；
                                                      0：dummy read功能关闭。 */
        unsigned int  reserved    : 31; /* bit[1-31]: 保留。 */
    } reg;
} SOC_MDDRC_dummyread_DDRC_DUMRD_CTRL_UNION;
#endif
#define SOC_MDDRC_dummyread_DDRC_DUMRD_CTRL_dummy_rd_en_START  (0)
#define SOC_MDDRC_dummyread_DDRC_DUMRD_CTRL_dummy_rd_en_END    (0)


/*****************************************************************************
 结构名    : SOC_MDDRC_dummyread_DDRC_DUMRD_CNT_UNION
 结构说明  : DDRC_DUMRD_CNT 寄存器结构定义。地址偏移量:0x4，初值:0x00000000，宽度:32
 寄存器说明: DDRC_DUMRD_CNT是DDRC的dummy read间隔计数寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  dummy_rd_cnt : 32; /* bit[0-31]: dummy read间隔计数器，即当该间隔到了后，会插入N个读命令。 */
    } reg;
} SOC_MDDRC_dummyread_DDRC_DUMRD_CNT_UNION;
#endif
#define SOC_MDDRC_dummyread_DDRC_DUMRD_CNT_dummy_rd_cnt_START  (0)
#define SOC_MDDRC_dummyread_DDRC_DUMRD_CNT_dummy_rd_cnt_END    (31)


/*****************************************************************************
 结构名    : SOC_MDDRC_dummyread_DDRC_DUMRD_NUM_UNION
 结构说明  : DDRC_DUMRD_NUM 寄存器结构定义。地址偏移量:0x8，初值:0x00000000，宽度:32
 寄存器说明: DDRC_DUMRD_NUM是DDRC的dummy read命令个数配置寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  dummy_rd_num : 8;  /* bit[0-7]  : dummy read间隔计数后到后，向对应的rank发送的读命令的个数。 */
        unsigned int  dummy_rd_gap : 8;  /* bit[8-15] : 每两个读命令之间的间隔。
                                                        0：连续发送；
                                                        1~255：两个命令之间等待N个周期 */
        unsigned int  reserved     : 16; /* bit[16-31]: 保留。 */
    } reg;
} SOC_MDDRC_dummyread_DDRC_DUMRD_NUM_UNION;
#endif
#define SOC_MDDRC_dummyread_DDRC_DUMRD_NUM_dummy_rd_num_START  (0)
#define SOC_MDDRC_dummyread_DDRC_DUMRD_NUM_dummy_rd_num_END    (7)
#define SOC_MDDRC_dummyread_DDRC_DUMRD_NUM_dummy_rd_gap_START  (8)
#define SOC_MDDRC_dummyread_DDRC_DUMRD_NUM_dummy_rd_gap_END    (15)


/*****************************************************************************
 结构名    : SOC_MDDRC_dummyread_DDRC_DUMRD_RNKADD_UNION
 结构说明  : DDRC_DUMRD_RNKADD 寄存器结构定义。地址偏移量:0xC+0x4*(rnks)，初值:0x00000000，宽度:32
 寄存器说明: DDRC_DUMRD_RNKADD是DDRC的RANK地址访问配置寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  dummy_rd_rnk_addr : 32; /* bit[0-31]: 该寄存器配置的地址指向对应的rank。比如对于rank0,该地址要配置成rank0中的某一个地址。 */
    } reg;
} SOC_MDDRC_dummyread_DDRC_DUMRD_RNKADD_UNION;
#endif
#define SOC_MDDRC_dummyread_DDRC_DUMRD_RNKADD_dummy_rd_rnk_addr_START  (0)
#define SOC_MDDRC_dummyread_DDRC_DUMRD_RNKADD_dummy_rd_rnk_addr_END    (31)


/*****************************************************************************
 结构名    : SOC_MDDRC_dummyread_DDRC_DUMRD_QOS_UNION
 结构说明  : DDRC_DUMRD_QOS 寄存器结构定义。地址偏移量:0x020，初值:0x00000000，宽度:32
 寄存器说明: DDRC_DUMRD_QOS是DUMRD读命令的arqos的优先级配置
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  dummy_rd_qos : 4;  /* bit[0-3] : dummy_read读命令的arqos配置，应该按照最高优先级配置 */
        unsigned int  reserved     : 28; /* bit[4-31]: 保留。 */
    } reg;
} SOC_MDDRC_dummyread_DDRC_DUMRD_QOS_UNION;
#endif
#define SOC_MDDRC_dummyread_DDRC_DUMRD_QOS_dummy_rd_qos_START  (0)
#define SOC_MDDRC_dummyread_DDRC_DUMRD_QOS_dummy_rd_qos_END    (3)


/*****************************************************************************
 结构名    : SOC_MDDRC_dummyread_DDRC_DUMRD_XID_UNION
 结构说明  : DDRC_DUMRD_XID 寄存器结构定义。地址偏移量:0x024，初值:0x00000000，宽度:32
 寄存器说明: DDRC_DUMRD_XID是DUMRD读命令的MID和ID配置寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  dummy_rd_id  : 16; /* bit[0-15] : dummy_read读命令的id配置，在选用id映射优先级时，要保证dummy_read映射的优先级最高 */
        unsigned int  dummy_rd_mid : 16; /* bit[16-31]: dummy_read读命令的mid配置 */
    } reg;
} SOC_MDDRC_dummyread_DDRC_DUMRD_XID_UNION;
#endif
#define SOC_MDDRC_dummyread_DDRC_DUMRD_XID_dummy_rd_id_START   (0)
#define SOC_MDDRC_dummyread_DDRC_DUMRD_XID_dummy_rd_id_END     (15)
#define SOC_MDDRC_dummyread_DDRC_DUMRD_XID_dummy_rd_mid_START  (16)
#define SOC_MDDRC_dummyread_DDRC_DUMRD_XID_dummy_rd_mid_END    (31)






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

#endif /* end of soc_mddrc_dummyread_interface.h */
