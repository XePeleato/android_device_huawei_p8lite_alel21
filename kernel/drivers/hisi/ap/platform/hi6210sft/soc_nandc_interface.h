

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/

#ifndef __SOC_NANDC_INTERFACE_H__
#define __SOC_NANDC_INTERFACE_H__

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
/* 寄存器说明：数据读写数目寄存器
   位域定义UNION结构:  SOC_NANDC_NfDataNum_UNION */
#define SOC_NANDC_NfDataNum_ADDR(base)                ((base) + (0x21000))

/* 寄存器说明：等待控制寄存器
   位域定义UNION结构:  SOC_NANDC_NfWSC_UNION */
#define SOC_NANDC_NfWSC_ADDR(base)                    ((base) + (0x21004))

/* 寄存器说明：器件配置寄存器。
   位域定义UNION结构:  SOC_NANDC_NfDeviceCfg_UNION */
#define SOC_NANDC_NfDeviceCfg_ADDR(base)              ((base) + (0x21008))

/* 寄存器说明：命令配置寄存器
   位域定义UNION结构:  SOC_NANDC_NfCmdCfg_UNION */
#define SOC_NANDC_NfCmdCfg_ADDR(base)                 ((base) + (0x2100C))

/* 寄存器说明：命令寄存器
   位域定义UNION结构:  SOC_NANDC_NfCmd_UNION */
#define SOC_NANDC_NfCmd_ADDR(base)                    ((base) + (0x21010))

/* 寄存器说明：低位地址寄存器
   位域定义UNION结构:  SOC_NANDC_NfAddrL_UNION */
#define SOC_NANDC_NfAddrL_ADDR(base)                  ((base) + (0x21014))

/* 寄存器说明：高位地址寄存器
   位域定义UNION结构:  SOC_NANDC_NfAddrH_UNION */
#define SOC_NANDC_NfAddrH_ADDR(base)                  ((base) + (0x21018))

/* 寄存器说明：NandC配置寄存器
   位域定义UNION结构:  SOC_NANDC_NfCfg_UNION */
#define SOC_NANDC_NfCfg_ADDR(base)                    ((base) + (0x2101C))

/* 寄存器说明：buffer读写控制寄存器
   位域定义UNION结构:  SOC_NANDC_NfBufReady_UNION */
#define SOC_NANDC_NfBufReady_ADDR(base)               ((base) + (0x21020))

/* 寄存器说明：中断使能寄存器
   位域定义UNION结构:  SOC_NANDC_NfIntEn_UNION */
#define SOC_NANDC_NfIntEn_ADDR(base)                  ((base) + (0x21024))

/* 寄存器说明：中断状态寄存器
   位域定义UNION结构:  SOC_NANDC_NfIntStatus_UNION */
#define SOC_NANDC_NfIntStatus_ADDR(base)              ((base) + (0x21028))

/* 寄存器说明：中断清除寄存器
   位域定义UNION结构:  SOC_NANDC_NfIntClr_UNION */
#define SOC_NANDC_NfIntClr_ADDR(base)                 ((base) + (0x2102C))

/* 寄存器说明：读启动寄存器
   位域定义UNION结构:  SOC_NANDC_StartRd_UNION */
#define SOC_NANDC_StartRd_ADDR(base)                  ((base) + (0x21030))

/* 寄存器说明：CS don’t care模式使能寄存器
   位域定义UNION结构:  SOC_NANDC_CsdnEn_UNION */
#define SOC_NANDC_CsdnEn_ADDR(base)                   ((base) + (0x21034))

/* 寄存器说明：FlashLock起始地址寄存器1
   位域定义UNION结构:  SOC_NANDC_NfLockStartAddr1_UNION */
#define SOC_NANDC_NfLockStartAddr1_ADDR(base)         ((base) + (0x21038))

/* 寄存器说明：FlashLock起始结束寄存器1
   位域定义UNION结构:  SOC_NANDC_NfLockEndAddr1_UNION */
#define SOC_NANDC_NfLockEndAddr1_ADDR(base)           ((base) + (0x2103C))

/* 寄存器说明：FlashLock起始地址寄存器2
   位域定义UNION结构:  SOC_NANDC_NfLockStartAddr2_UNION */
#define SOC_NANDC_NfLockStartAddr2_ADDR(base)         ((base) + (0x21040))

/* 寄存器说明：FlashLock起始结束寄存器2
   位域定义UNION结构:  SOC_NANDC_NfLockEndAddr2_UNION */
#define SOC_NANDC_NfLockEndAddr2_ADDR(base)           ((base) + (0x21044))

/* 寄存器说明：FlashLock起始地址寄存器3
   位域定义UNION结构:  SOC_NANDC_NfLockStartAddr3_UNION */
#define SOC_NANDC_NfLockStartAddr3_ADDR(base)         ((base) + (0x21048))

/* 寄存器说明：FlashLock起始结束寄存器3
   位域定义UNION结构:  SOC_NANDC_NfLockEndAddr3_UNION */
#define SOC_NANDC_NfLockEndAddr3_ADDR(base)           ((base) + (0x2104C))

/* 寄存器说明：FlashLock起始地址寄存器4
   位域定义UNION结构:  SOC_NANDC_NfLockStartAddr4_UNION */
#define SOC_NANDC_NfLockStartAddr4_ADDR(base)         ((base) + (0x21050))

/* 寄存器说明：FlashLock起始结束寄存器4
   位域定义UNION结构:  SOC_NANDC_NfLockEndAddr4_UNION */
#define SOC_NANDC_NfLockEndAddr4_ADDR(base)           ((base) + (0x21054))

/* 寄存器说明：FlashLock使能寄存器
   位域定义UNION结构:  SOC_NANDC_NfLockEn_UNION */
#define SOC_NANDC_NfLockEn_ADDR(base)                 ((base) + (0x21058))

/* 寄存器说明：FlashLock控制寄存器
   位域定义UNION结构:  SOC_NANDC_NfLockCont_UNION */
#define SOC_NANDC_NfLockCont_ADDR(base)               ((base) + (0x2105C))

/* 寄存器说明：FlashLock命令配置寄存器1
   位域定义UNION结构:  SOC_NANDC_NflockCommand1_UNION */
#define SOC_NANDC_NflockCommand1_ADDR(base)           ((base) + (0x21060))

/* 寄存器说明：FlashLock命令配置寄存器2
   位域定义UNION结构:  SOC_NANDC_NflockCommand2_UNION */
#define SOC_NANDC_NflockCommand2_ADDR(base)           ((base) + (0x21064))





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
 结构名    : SOC_NANDC_NfDataNum_UNION
 结构说明  : NfDataNum 寄存器结构定义。地址偏移量:0x21000，初值:0x840，宽度:32
 寄存器说明: 数据读写数目寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  NfDataNum : 12; /* bit[0-11] : 读写byte的数目。0：读写0个byte。
                                                     缺省读写2112 (528*4) byte。 */
        unsigned int  reserved  : 20; /* bit[12-31]: 保留位。读时返回0。写时无影响。 */
    } reg;
} SOC_NANDC_NfDataNum_UNION;
#endif
#define SOC_NANDC_NfDataNum_NfDataNum_START  (0)
#define SOC_NANDC_NfDataNum_NfDataNum_END    (11)


/*****************************************************************************
 结构名    : SOC_NANDC_NfWSC_UNION
 结构说明  : NfWSC 寄存器结构定义。地址偏移量:0x21004，初值:0xF333，宽度:32
 寄存器说明: 等待控制寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  wt pulse          : 4;  /* bit[0-3]  : NandFlash写信号低电平宽度。
                                                             4’b0000，一个时钟周期
                                                             ～
                                                             4’b1111：16个时钟周期 */
        unsigned int  rd pulse          : 4;  /* bit[4-7]  : NandFlash读信号低电平宽度。
                                                             4’b0001，2个时钟周期
                                                             ～
                                                             4’b1111：16个时钟周期 */
        unsigned int  rd/wt high wide   : 4;  /* bit[8-11] : NandFlash读/写信号高电平宽度
                                                             4’b0001，2个时钟周期
                                                             ～
                                                             4’b1111：16个时钟周期
                                                             注：不能配置为0 */
        unsigned int  wait busy delay   : 4;  /* bit[12-15]: NandFlash命令或最后一个地址到busy状态的延时。
                                                             4’h0，2个时钟周期。
                                                             4’hF：32个时钟周期 */
        unsigned int  cs not care delay : 4;  /* bit[16-19]: cs don’t care情况下ready/busy信号拉低后到cs无效的延迟时间：
                                                             4’h0，0个时钟周期。
                                                             4’hF：15个时钟周期 */
        unsigned int  reserved          : 12; /* bit[20-31]: 保留位。读时返回0。写时无影响。 */
    } reg;
} SOC_NANDC_NfWSC_UNION;
#endif
#define SOC_NANDC_NfWSC_wt pulse_START           (0)
#define SOC_NANDC_NfWSC_wt pulse_END             (3)
#define SOC_NANDC_NfWSC_rd pulse_START           (4)
#define SOC_NANDC_NfWSC_rd pulse_END             (7)
#define SOC_NANDC_NfWSC_rd/wt high wide_START    (8)
#define SOC_NANDC_NfWSC_rd/wt high wide_END      (11)
#define SOC_NANDC_NfWSC_wait busy delay_START    (12)
#define SOC_NANDC_NfWSC_wait busy delay_END      (15)
#define SOC_NANDC_NfWSC_cs not care delay_START  (16)
#define SOC_NANDC_NfWSC_cs not care delay_END    (19)


/*****************************************************************************
 结构名    : SOC_NANDC_NfDeviceCfg_UNION
 结构说明  : NfDeviceCfg 寄存器结构定义。地址偏移量:0x21008，初值:-，宽度:32
 寄存器说明: 器件配置寄存器。
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  page size     : 1;  /* bit[0]   : NandFlash的页大小。
                                                        0：512 byte/page
                                                        1：2K byte/page。
                                                        复位值由管脚nand_page_size取反决定 */
        unsigned int  data bus wide : 1;  /* bit[1]   : NandFlash的数据线宽度。
                                                        0：8bit。1：16bit。
                                                        复位值由管脚nand_bus_wide决定 */
        unsigned int  reserved      : 30; /* bit[2-31]: 保留位。读时返回0。写时无影响。 */
    } reg;
} SOC_NANDC_NfDeviceCfg_UNION;
#endif
#define SOC_NANDC_NfDeviceCfg_page size_START      (0)
#define SOC_NANDC_NfDeviceCfg_page size_END        (0)
#define SOC_NANDC_NfDeviceCfg_data bus wide_START  (1)
#define SOC_NANDC_NfDeviceCfg_data bus wide_END    (1)


/*****************************************************************************
 结构名    : SOC_NANDC_NfCmdCfg_UNION
 结构说明  : NfCmdCfg 寄存器结构定义。地址偏移量:0x2100C，初值:-，宽度:32
 寄存器说明: 命令配置寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  address cycles         : 3;  /* bit[0-2]  : Controller发给NandFlash的地址拍数。
                                                                  缺省值由管脚nand_addr_num[1:0]决定，关系如下。
                                                                  nand_addr_num＝0——address cycles＝5；nand_addr_num＝1——address cycles＝4；nand_addr_num＝2——address cycles＝3 */
        unsigned int  reserved_0             : 1;  /* bit[3]    : 保留 */
        unsigned int  command address format : 1;  /* bit[4]    : Controller发给NandFlash的命令地址格式
                                                                  0：发送“comand1＋地址”
                                                                  1：发送“comand1＋地址＋command2”。
                                                                  复位值由管脚nand_page_size决定。nand_page_size等于0：复位值为0。nand_page_size等于1：复位值为1 */
        unsigned int  command only           : 1;  /* bit[5]    : 0：Controller发送命令地址后，根据Controller当前的读写状态，启动读写数据状态机。
                                                                  1：Controller只发送命令地址，不启动读写数据状态机。
                                                                  当CPU直接读写数据时，该位置应1 */
        unsigned int  read/write mode        : 1;  /* bit[6]    : 0：从NandFlash读数据
                                                                  1：向NandFlash写数据
                                                                  在boot模式下，该位被硬件置0，一定处于读状态 */
        unsigned int  Busy Not Care          : 1;  /* bit[7]    : 0：在busy状态下不能向nand flash发送命令；
                                                                  1：在busy状态下可以向nand flash发送命令 */
        unsigned int  CSSEL                  : 2;  /* bit[8-9]  : CS select。
                                                                  00：选择cs0_n；
                                                                  01：选择cs1_n；
                                                                  10：选择cs2_n；
                                                                  11：选择cs3_n */
        unsigned int  EBI_N                  : 1;  /* bit[10]   : 0：CS don’t care时EBI不拉低；
                                                                  1：CS don’t care时EBI也拉低。 */
        unsigned int  reserved_1             : 21; /* bit[11-31]: 保留位。读时返回0。写时无影响。 */
    } reg;
} SOC_NANDC_NfCmdCfg_UNION;
#endif
#define SOC_NANDC_NfCmdCfg_address cycles_START          (0)
#define SOC_NANDC_NfCmdCfg_address cycles_END            (2)
#define SOC_NANDC_NfCmdCfg_command address format_START  (4)
#define SOC_NANDC_NfCmdCfg_command address format_END    (4)
#define SOC_NANDC_NfCmdCfg_command only_START            (5)
#define SOC_NANDC_NfCmdCfg_command only_END              (5)
#define SOC_NANDC_NfCmdCfg_read/write mode_START         (6)
#define SOC_NANDC_NfCmdCfg_read/write mode_END           (6)
#define SOC_NANDC_NfCmdCfg_Busy Not Care_START           (7)
#define SOC_NANDC_NfCmdCfg_Busy Not Care_END             (7)
#define SOC_NANDC_NfCmdCfg_CSSEL_START                   (8)
#define SOC_NANDC_NfCmdCfg_CSSEL_END                     (9)
#define SOC_NANDC_NfCmdCfg_EBI_N_START                   (10)
#define SOC_NANDC_NfCmdCfg_EBI_N_END                     (10)


/*****************************************************************************
 结构名    : SOC_NANDC_NfCmd_UNION
 结构说明  : NfCmd 寄存器结构定义。地址偏移量:0x21010，初值:0x3000，宽度:32
 寄存器说明: 命令寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  command 1 : 8;  /* bit[0-7]  : Controller发给NandFlash的第一个命令 */
        unsigned int  command 2 : 8;  /* bit[8-15] : Controller发给NandFlash的第二个命令 */
        unsigned int  reserved  : 16; /* bit[16-31]: 保留位。读时返回0。写时无影响。 */
    } reg;
} SOC_NANDC_NfCmd_UNION;
#endif
#define SOC_NANDC_NfCmd_command 1_START  (0)
#define SOC_NANDC_NfCmd_command 1_END    (7)
#define SOC_NANDC_NfCmd_command 2_START  (8)
#define SOC_NANDC_NfCmd_command 2_END    (15)


/*****************************************************************************
 结构名    : SOC_NANDC_NfAddrL_UNION
 结构说明  : NfAddrL 寄存器结构定义。地址偏移量:0x21014，初值:0x0，宽度:32
 寄存器说明: 低位地址寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  low address : 32; /* bit[0-31]: NandFlash低32bit地址 */
    } reg;
} SOC_NANDC_NfAddrL_UNION;
#endif
#define SOC_NANDC_NfAddrL_low address_START  (0)
#define SOC_NANDC_NfAddrL_low address_END    (31)


/*****************************************************************************
 结构名    : SOC_NANDC_NfAddrH_UNION
 结构说明  : NfAddrH 寄存器结构定义。地址偏移量:0x21018，初值:0x0，宽度:32
 寄存器说明: 高位地址寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  high address : 8;  /* bit[0-7] : NandFlash高8bit地址 */
        unsigned int  reserved     : 24; /* bit[8-31]: 保留位。读时返回0。写时无影响。 */
    } reg;
} SOC_NANDC_NfAddrH_UNION;
#endif
#define SOC_NANDC_NfAddrH_high address_START  (0)
#define SOC_NANDC_NfAddrH_high address_END    (7)


/*****************************************************************************
 结构名    : SOC_NANDC_NfCfg_UNION
 结构说明  : NfCfg 寄存器结构定义。地址偏移量:0x2101C，初值:0x0，宽度:32
 寄存器说明: NandC配置寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  boot mode disable         : 1;  /* bit[0]   : 0：NandFlash Controller处于boot模式。
                                                                    1：NandFlash Controller处于normal模式 */
        unsigned int  ECC bypass                : 1;  /* bit[1]   : 0：ECC校验使能。1：ECC校验禁止。 */
        unsigned int  SECC bypass               : 1;  /* bit[2]   : 0：LSN数据ECC校验使能。
                                                                    1：LSN数据ECC校验禁止。 */
        unsigned int  ECC error correct disable : 1;  /* bit[3]   : 0：ECC 1bit纠错使能。
                                                                    1：ECC 1bit纠错禁止 */
        unsigned int  reserved                  : 28; /* bit[4-31]: 保留位。读时返回0。写时无影响。 */
    } reg;
} SOC_NANDC_NfCfg_UNION;
#endif
#define SOC_NANDC_NfCfg_boot mode disable_START          (0)
#define SOC_NANDC_NfCfg_boot mode disable_END            (0)
#define SOC_NANDC_NfCfg_ECC bypass_START                 (1)
#define SOC_NANDC_NfCfg_ECC bypass_END                   (1)
#define SOC_NANDC_NfCfg_SECC bypass_START                (2)
#define SOC_NANDC_NfCfg_SECC bypass_END                  (2)
#define SOC_NANDC_NfCfg_ECC error correct disable_START  (3)
#define SOC_NANDC_NfCfg_ECC error correct disable_END    (3)


/*****************************************************************************
 结构名    : SOC_NANDC_NfBufReady_UNION
 结构说明  : NfBufReady 寄存器结构定义。地址偏移量:0x21020，初值:0xF，宽度:32
 寄存器说明: buffer读写控制寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  buffer ready : 4;  /* bit[0-3] : 每个bit对应一个buffer。
                                                       读数据时，从NandFlash读取的数据，写到从bit0开始，第一个状态为1的buffer，到最后一个状态为1的buffer。
                                                       写数据时，从bit0开始，第一个bit为1的buffer的数据写到NandFlash.
                                                       该寄存器只对512 byte page size器件有效 */
        unsigned int  reserved     : 28; /* bit[4-31]: 保留位。读时返回0。写时无影响。 */
    } reg;
} SOC_NANDC_NfBufReady_UNION;
#endif
#define SOC_NANDC_NfBufReady_buffer ready_START  (0)
#define SOC_NANDC_NfBufReady_buffer ready_END    (3)


/*****************************************************************************
 结构名    : SOC_NANDC_NfIntEn_UNION
 结构说明  : NfIntEn 寄存器结构定义。地址偏移量:0x21024，初值:0x0，宽度:32
 寄存器说明: 中断使能寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  command done CS0    : 1;  /* bit[0]    : controller发送完命令和地址后，产生中断 */
        unsigned int  read done CS0       : 1;  /* bit[1]    : 读操作结束，产生中断 */
        unsigned int  write done CS0      : 1;  /* bit[2]    : 写操作结束，产生中断 */
        unsigned int  reserved_0          : 1;  /* bit[3]    : 保留位 */
        unsigned int  command done CS1    : 1;  /* bit[4]    : controller发送完命令和地址后，产生中断 */
        unsigned int  read done CS1       : 1;  /* bit[5]    : 读操作结束，产生中断 */
        unsigned int  write done CS1      : 1;  /* bit[6]    : 写操作结束，产生中断 */
        unsigned int  reserved_1          : 1;  /* bit[7]    : 保留位 */
        unsigned int  command done CS2    : 1;  /* bit[8]    : controller发送完命令和地址后，产生中断 */
        unsigned int  read done CS2       : 1;  /* bit[9]    : 读操作结束，产生中断 */
        unsigned int  write done CS2      : 1;  /* bit[10]   : 写操作结束，产生中断 */
        unsigned int  reserved_2          : 1;  /* bit[11]   : 保留位 */
        unsigned int  command done CS3    : 1;  /* bit[12]   : controller发送完命令和地址后，产生中断 */
        unsigned int  read done CS3       : 1;  /* bit[13]   : 读操作结束，产生中断 */
        unsigned int  write done CS3      : 1;  /* bit[14]   : Ecc 2bits 报错中断：
                                                               如果bit[21]为1，指示1bit中断，那么bit[14]同时指示为0，才是真正的1bit error中断；
                                                               如果bit[21]为1，指示1bit中断，而bit[14]也为1，那么不是1bit error的情况，而是2bits error报错为1bit error的情况 */
        unsigned int  reserved_3          : 1;  /* bit[15]   : 保留位 */
        unsigned int  all buffer ready    : 1;  /* bit[16]   : 读状态下，所有buffer数据有效 */
        unsigned int  buffer1 ready       : 1;  /* bit[17]   : 读状态下，buffer1数据有效，产生中断。只对512 byte page size器件有效 */
        unsigned int  buffer2 ready       : 1;  /* bit[18]   : 读状态下，buffer1数据有效，产生中断。只对512 byte page size器件有效 */
        unsigned int  buffer3 ready       : 1;  /* bit[19]   : 读状态下，buffer1数据有效，产生中断。只对512 byte page size器件有效 */
        unsigned int  buffer4 ready       : 1;  /* bit[20]   : 读状态下，buffer1数据有效，产生中断。只对512 byte page size器件有效 */
        unsigned int  data ecc 1bit error : 1;  /* bit[21]   : ecc校验1bit出错，产生中断 */
        unsigned int  data ecc 2bit error : 1;  /* bit[22]   : ecc校验2bit以上出错，产生中断 */
        unsigned int  lsn ecc 1bit error  : 1;  /* bit[23]   : lsn 数据ecc校验1bit出错，产生中断 */
        unsigned int  lsn ecc 2bit error  : 1;  /* bit[24]   : lsn 数据ecc校验2bit以上出错，产生中断 */
        unsigned int  flashlock error     : 1;  /* bit[25]   : flashlock未解锁时，往命令寄存器写命令。 */
        unsigned int  reserved_4          : 6;  /* bit[26-31]: 保留位。读时返回0。写时无影响。 */
    } reg;
} SOC_NANDC_NfIntEn_UNION;
#endif
#define SOC_NANDC_NfIntEn_command done CS0_START     (0)
#define SOC_NANDC_NfIntEn_command done CS0_END       (0)
#define SOC_NANDC_NfIntEn_read done CS0_START        (1)
#define SOC_NANDC_NfIntEn_read done CS0_END          (1)
#define SOC_NANDC_NfIntEn_write done CS0_START       (2)
#define SOC_NANDC_NfIntEn_write done CS0_END         (2)
#define SOC_NANDC_NfIntEn_command done CS1_START     (4)
#define SOC_NANDC_NfIntEn_command done CS1_END       (4)
#define SOC_NANDC_NfIntEn_read done CS1_START        (5)
#define SOC_NANDC_NfIntEn_read done CS1_END          (5)
#define SOC_NANDC_NfIntEn_write done CS1_START       (6)
#define SOC_NANDC_NfIntEn_write done CS1_END         (6)
#define SOC_NANDC_NfIntEn_command done CS2_START     (8)
#define SOC_NANDC_NfIntEn_command done CS2_END       (8)
#define SOC_NANDC_NfIntEn_read done CS2_START        (9)
#define SOC_NANDC_NfIntEn_read done CS2_END          (9)
#define SOC_NANDC_NfIntEn_write done CS2_START       (10)
#define SOC_NANDC_NfIntEn_write done CS2_END         (10)
#define SOC_NANDC_NfIntEn_command done CS3_START     (12)
#define SOC_NANDC_NfIntEn_command done CS3_END       (12)
#define SOC_NANDC_NfIntEn_read done CS3_START        (13)
#define SOC_NANDC_NfIntEn_read done CS3_END          (13)
#define SOC_NANDC_NfIntEn_write done CS3_START       (14)
#define SOC_NANDC_NfIntEn_write done CS3_END         (14)
#define SOC_NANDC_NfIntEn_all buffer ready_START     (16)
#define SOC_NANDC_NfIntEn_all buffer ready_END       (16)
#define SOC_NANDC_NfIntEn_buffer1 ready_START        (17)
#define SOC_NANDC_NfIntEn_buffer1 ready_END          (17)
#define SOC_NANDC_NfIntEn_buffer2 ready_START        (18)
#define SOC_NANDC_NfIntEn_buffer2 ready_END          (18)
#define SOC_NANDC_NfIntEn_buffer3 ready_START        (19)
#define SOC_NANDC_NfIntEn_buffer3 ready_END          (19)
#define SOC_NANDC_NfIntEn_buffer4 ready_START        (20)
#define SOC_NANDC_NfIntEn_buffer4 ready_END          (20)
#define SOC_NANDC_NfIntEn_data ecc 1bit error_START  (21)
#define SOC_NANDC_NfIntEn_data ecc 1bit error_END    (21)
#define SOC_NANDC_NfIntEn_data ecc 2bit error_START  (22)
#define SOC_NANDC_NfIntEn_data ecc 2bit error_END    (22)
#define SOC_NANDC_NfIntEn_lsn ecc 1bit error_START   (23)
#define SOC_NANDC_NfIntEn_lsn ecc 1bit error_END     (23)
#define SOC_NANDC_NfIntEn_lsn ecc 2bit error_START   (24)
#define SOC_NANDC_NfIntEn_lsn ecc 2bit error_END     (24)
#define SOC_NANDC_NfIntEn_flashlock error_START      (25)
#define SOC_NANDC_NfIntEn_flashlock error_END        (25)


/*****************************************************************************
 结构名    : SOC_NANDC_NfIntStatus_UNION
 结构说明  : NfIntStatus 寄存器结构定义。地址偏移量:0x21028，初值:0x0，宽度:32
 寄存器说明: 中断状态寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  command done CS0    : 1;  /* bit[0]    : controller发送完命令和地址后，产生中断 */
        unsigned int  read done CS0       : 1;  /* bit[1]    : 读操作结束，产生中断 */
        unsigned int  write done CS0      : 1;  /* bit[2]    : 写操作结束，产生中断 */
        unsigned int  reserved_0          : 1;  /* bit[3]    : 保留位 */
        unsigned int  command done CS1    : 1;  /* bit[4]    : controller发送完命令和地址后，产生中断 */
        unsigned int  read done CS1       : 1;  /* bit[5]    : 读操作结束，产生中断 */
        unsigned int  write done CS1      : 1;  /* bit[6]    : 写操作结束，产生中断 */
        unsigned int  reserved_1          : 1;  /* bit[7]    : 保留位 */
        unsigned int  command done CS2    : 1;  /* bit[8]    : controller发送完命令和地址后，产生中断 */
        unsigned int  read done CS2       : 1;  /* bit[9]    : 读操作结束，产生中断 */
        unsigned int  write done CS2      : 1;  /* bit[10]   : 写操作结束，产生中断 */
        unsigned int  reserved_2          : 1;  /* bit[11]   : 保留位 */
        unsigned int  command done CS3    : 1;  /* bit[12]   : controller发送完命令和地址后，产生中断 */
        unsigned int  read done CS3       : 1;  /* bit[13]   : 读操作结束，产生中断 */
        unsigned int  write done CS3      : 1;  /* bit[14]   : Ecc 2bits 报错中断：
                                                               如果bit[21]为1，指示1bit中断，那么bit[14]同时指示为0，才是真正的1bit error中断；
                                                               如果bit[21]为1，指示1bit中断，而bit[14]也为1，那么不是1bit error的情况，而是2bits error报错为1bit error的情况 */
        unsigned int  reserved_3          : 1;  /* bit[15]   : 保留位 */
        unsigned int  all buffer ready    : 1;  /* bit[16]   : 读状态下，所有buffer数据有效 */
        unsigned int  buffer1 ready       : 1;  /* bit[17]   : 读状态下，buffer1数据有效，产生中断。只对512 byte page size器件有效 */
        unsigned int  buffer2 ready       : 1;  /* bit[18]   : 读状态下，buffer1数据有效，产生中断。只对512 byte page size器件有效 */
        unsigned int  buffer3 ready       : 1;  /* bit[19]   : 读状态下，buffer1数据有效，产生中断。只对512 byte page size器件有效 */
        unsigned int  buffer4 ready       : 1;  /* bit[20]   : 读状态下，buffer1数据有效，产生中断。只对512 byte page size器件有效 */
        unsigned int  data ecc 1bit error : 1;  /* bit[21]   : ecc校验1bit出错，产生中断 */
        unsigned int  data ecc 2bit error : 1;  /* bit[22]   : ecc校验2bit以上出错，产生中断 */
        unsigned int  lsn ecc 1bit error  : 1;  /* bit[23]   : lsn 数据ecc校验1bit出错，产生中断 */
        unsigned int  lsn ecc 2bit error  : 1;  /* bit[24]   : lsn 数据ecc校验2bit以上出错，产生中断 */
        unsigned int  flashlock error     : 1;  /* bit[25]   : flashlock未解锁时，往命令寄存器写命令。 */
        unsigned int  reserved_4          : 6;  /* bit[26-31]: 保留位。读时返回0。写时无影响。 */
    } reg;
} SOC_NANDC_NfIntStatus_UNION;
#endif
#define SOC_NANDC_NfIntStatus_command done CS0_START     (0)
#define SOC_NANDC_NfIntStatus_command done CS0_END       (0)
#define SOC_NANDC_NfIntStatus_read done CS0_START        (1)
#define SOC_NANDC_NfIntStatus_read done CS0_END          (1)
#define SOC_NANDC_NfIntStatus_write done CS0_START       (2)
#define SOC_NANDC_NfIntStatus_write done CS0_END         (2)
#define SOC_NANDC_NfIntStatus_command done CS1_START     (4)
#define SOC_NANDC_NfIntStatus_command done CS1_END       (4)
#define SOC_NANDC_NfIntStatus_read done CS1_START        (5)
#define SOC_NANDC_NfIntStatus_read done CS1_END          (5)
#define SOC_NANDC_NfIntStatus_write done CS1_START       (6)
#define SOC_NANDC_NfIntStatus_write done CS1_END         (6)
#define SOC_NANDC_NfIntStatus_command done CS2_START     (8)
#define SOC_NANDC_NfIntStatus_command done CS2_END       (8)
#define SOC_NANDC_NfIntStatus_read done CS2_START        (9)
#define SOC_NANDC_NfIntStatus_read done CS2_END          (9)
#define SOC_NANDC_NfIntStatus_write done CS2_START       (10)
#define SOC_NANDC_NfIntStatus_write done CS2_END         (10)
#define SOC_NANDC_NfIntStatus_command done CS3_START     (12)
#define SOC_NANDC_NfIntStatus_command done CS3_END       (12)
#define SOC_NANDC_NfIntStatus_read done CS3_START        (13)
#define SOC_NANDC_NfIntStatus_read done CS3_END          (13)
#define SOC_NANDC_NfIntStatus_write done CS3_START       (14)
#define SOC_NANDC_NfIntStatus_write done CS3_END         (14)
#define SOC_NANDC_NfIntStatus_all buffer ready_START     (16)
#define SOC_NANDC_NfIntStatus_all buffer ready_END       (16)
#define SOC_NANDC_NfIntStatus_buffer1 ready_START        (17)
#define SOC_NANDC_NfIntStatus_buffer1 ready_END          (17)
#define SOC_NANDC_NfIntStatus_buffer2 ready_START        (18)
#define SOC_NANDC_NfIntStatus_buffer2 ready_END          (18)
#define SOC_NANDC_NfIntStatus_buffer3 ready_START        (19)
#define SOC_NANDC_NfIntStatus_buffer3 ready_END          (19)
#define SOC_NANDC_NfIntStatus_buffer4 ready_START        (20)
#define SOC_NANDC_NfIntStatus_buffer4 ready_END          (20)
#define SOC_NANDC_NfIntStatus_data ecc 1bit error_START  (21)
#define SOC_NANDC_NfIntStatus_data ecc 1bit error_END    (21)
#define SOC_NANDC_NfIntStatus_data ecc 2bit error_START  (22)
#define SOC_NANDC_NfIntStatus_data ecc 2bit error_END    (22)
#define SOC_NANDC_NfIntStatus_lsn ecc 1bit error_START   (23)
#define SOC_NANDC_NfIntStatus_lsn ecc 1bit error_END     (23)
#define SOC_NANDC_NfIntStatus_lsn ecc 2bit error_START   (24)
#define SOC_NANDC_NfIntStatus_lsn ecc 2bit error_END     (24)
#define SOC_NANDC_NfIntStatus_flashlock error_START      (25)
#define SOC_NANDC_NfIntStatus_flashlock error_END        (25)


/*****************************************************************************
 结构名    : SOC_NANDC_NfIntClr_UNION
 结构说明  : NfIntClr 寄存器结构定义。地址偏移量:0x2102C，初值:-，宽度:32
 寄存器说明: 中断清除寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  command done CS0    : 1;  /* bit[0]    : controller发送完命令和地址后，产生中断 */
        unsigned int  read done CS0       : 1;  /* bit[1]    : 读操作结束，产生中断 */
        unsigned int  write done CS0      : 1;  /* bit[2]    : 写操作结束，产生中断 */
        unsigned int  reserved_0          : 1;  /* bit[3]    : 保留位 */
        unsigned int  command done CS1    : 1;  /* bit[4]    : controller发送完命令和地址后，产生中断 */
        unsigned int  read done CS1       : 1;  /* bit[5]    : 读操作结束，产生中断 */
        unsigned int  write done CS1      : 1;  /* bit[6]    : 写操作结束，产生中断 */
        unsigned int  reserved_1          : 1;  /* bit[7]    : 保留位 */
        unsigned int  command done CS2    : 1;  /* bit[8]    : controller发送完命令和地址后，产生中断 */
        unsigned int  read done CS2       : 1;  /* bit[9]    : 读操作结束，产生中断 */
        unsigned int  write done CS2      : 1;  /* bit[10]   : 写操作结束，产生中断 */
        unsigned int  reserved_2          : 1;  /* bit[11]   : 保留位 */
        unsigned int  command done CS3    : 1;  /* bit[12]   : controller发送完命令和地址后，产生中断 */
        unsigned int  read done CS3       : 1;  /* bit[13]   : 读操作结束，产生中断 */
        unsigned int  write done CS3      : 1;  /* bit[14]   : Ecc 2bits 报错中断：
                                                               如果bit[21]为1，指示1bit中断，那么bit[14]同时指示为0，才是真正的1bit error中断；
                                                               如果bit[21]为1，指示1bit中断，而bit[14]也为1，那么不是1bit error的情况，而是2bits error报错为1bit error的情况 */
        unsigned int  reserved_3          : 1;  /* bit[15]   : 保留位 */
        unsigned int  all buffer ready    : 1;  /* bit[16]   : 读状态下，所有buffer数据有效 */
        unsigned int  buffer1 ready       : 1;  /* bit[17]   : 读状态下，buffer1数据有效，产生中断。只对512 byte page size器件有效 */
        unsigned int  buffer2 ready       : 1;  /* bit[18]   : 读状态下，buffer1数据有效，产生中断。只对512 byte page size器件有效 */
        unsigned int  buffer3 ready       : 1;  /* bit[19]   : 读状态下，buffer1数据有效，产生中断。只对512 byte page size器件有效 */
        unsigned int  buffer4 ready       : 1;  /* bit[20]   : 读状态下，buffer1数据有效，产生中断。只对512 byte page size器件有效 */
        unsigned int  data ecc 1bit error : 1;  /* bit[21]   : ecc校验1bit出错，产生中断 */
        unsigned int  data ecc 2bit error : 1;  /* bit[22]   : ecc校验2bit以上出错，产生中断 */
        unsigned int  lsn ecc 1bit error  : 1;  /* bit[23]   : lsn 数据ecc校验1bit出错，产生中断 */
        unsigned int  lsn ecc 2bit error  : 1;  /* bit[24]   : lsn 数据ecc校验2bit以上出错，产生中断 */
        unsigned int  flashlock error     : 1;  /* bit[25]   : flashlock未解锁时，往命令寄存器写命令。 */
        unsigned int  reserved_4          : 6;  /* bit[26-31]: 保留位。读时返回0。写时无影响。 */
    } reg;
} SOC_NANDC_NfIntClr_UNION;
#endif
#define SOC_NANDC_NfIntClr_command done CS0_START     (0)
#define SOC_NANDC_NfIntClr_command done CS0_END       (0)
#define SOC_NANDC_NfIntClr_read done CS0_START        (1)
#define SOC_NANDC_NfIntClr_read done CS0_END          (1)
#define SOC_NANDC_NfIntClr_write done CS0_START       (2)
#define SOC_NANDC_NfIntClr_write done CS0_END         (2)
#define SOC_NANDC_NfIntClr_command done CS1_START     (4)
#define SOC_NANDC_NfIntClr_command done CS1_END       (4)
#define SOC_NANDC_NfIntClr_read done CS1_START        (5)
#define SOC_NANDC_NfIntClr_read done CS1_END          (5)
#define SOC_NANDC_NfIntClr_write done CS1_START       (6)
#define SOC_NANDC_NfIntClr_write done CS1_END         (6)
#define SOC_NANDC_NfIntClr_command done CS2_START     (8)
#define SOC_NANDC_NfIntClr_command done CS2_END       (8)
#define SOC_NANDC_NfIntClr_read done CS2_START        (9)
#define SOC_NANDC_NfIntClr_read done CS2_END          (9)
#define SOC_NANDC_NfIntClr_write done CS2_START       (10)
#define SOC_NANDC_NfIntClr_write done CS2_END         (10)
#define SOC_NANDC_NfIntClr_command done CS3_START     (12)
#define SOC_NANDC_NfIntClr_command done CS3_END       (12)
#define SOC_NANDC_NfIntClr_read done CS3_START        (13)
#define SOC_NANDC_NfIntClr_read done CS3_END          (13)
#define SOC_NANDC_NfIntClr_write done CS3_START       (14)
#define SOC_NANDC_NfIntClr_write done CS3_END         (14)
#define SOC_NANDC_NfIntClr_all buffer ready_START     (16)
#define SOC_NANDC_NfIntClr_all buffer ready_END       (16)
#define SOC_NANDC_NfIntClr_buffer1 ready_START        (17)
#define SOC_NANDC_NfIntClr_buffer1 ready_END          (17)
#define SOC_NANDC_NfIntClr_buffer2 ready_START        (18)
#define SOC_NANDC_NfIntClr_buffer2 ready_END          (18)
#define SOC_NANDC_NfIntClr_buffer3 ready_START        (19)
#define SOC_NANDC_NfIntClr_buffer3 ready_END          (19)
#define SOC_NANDC_NfIntClr_buffer4 ready_START        (20)
#define SOC_NANDC_NfIntClr_buffer4 ready_END          (20)
#define SOC_NANDC_NfIntClr_data ecc 1bit error_START  (21)
#define SOC_NANDC_NfIntClr_data ecc 1bit error_END    (21)
#define SOC_NANDC_NfIntClr_data ecc 2bit error_START  (22)
#define SOC_NANDC_NfIntClr_data ecc 2bit error_END    (22)
#define SOC_NANDC_NfIntClr_lsn ecc 1bit error_START   (23)
#define SOC_NANDC_NfIntClr_lsn ecc 1bit error_END     (23)
#define SOC_NANDC_NfIntClr_lsn ecc 2bit error_START   (24)
#define SOC_NANDC_NfIntClr_lsn ecc 2bit error_END     (24)
#define SOC_NANDC_NfIntClr_flashlock error_START      (25)
#define SOC_NANDC_NfIntClr_flashlock error_END        (25)


/*****************************************************************************
 结构名    : SOC_NANDC_StartRd_UNION
 结构说明  : StartRd 寄存器结构定义。地址偏移量:0x21030，初值:-，宽度:32
 寄存器说明: 读启动寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  Start Read : 1;  /* bit[0]   : 启动读数据状态机 */
        unsigned int  reserved   : 31; /* bit[1-31]: 保留位。读时返回0。写时无影响。 */
    } reg;
} SOC_NANDC_StartRd_UNION;
#endif
#define SOC_NANDC_StartRd_Start Read_START  (0)
#define SOC_NANDC_StartRd_Start Read_END    (0)


/*****************************************************************************
 结构名    : SOC_NANDC_CsdnEn_UNION
 结构说明  : CsdnEn 寄存器结构定义。地址偏移量:0x21034，初值:0x0，宽度:32
 寄存器说明: CS don’t care模式使能寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  CS don’t care Enable : 1;  /* bit[0]   : 使能nand flash controller的cs don’t care工作模式
                                                                注：本功能不能同ready/busy don’t care功能同时使用，否则可能会导致nand传输错误！ */
        unsigned int  reserved              : 31; /* bit[1-31]: 保留位。读时返回0。写时无影响。 */
    } reg;
} SOC_NANDC_CsdnEn_UNION;
#endif
#define SOC_NANDC_CsdnEn_CS don’t care Enable_START  (0)
#define SOC_NANDC_CsdnEn_CS don’t care Enable_END    (0)


/*****************************************************************************
 结构名    : SOC_NANDC_NfLockStartAddr1_UNION
 结构说明  : NfLockStartAddr1 寄存器结构定义。地址偏移量:0x21038，初值:0x0，宽度:32
 寄存器说明: FlashLock起始地址寄存器1
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  flashlock start addr1 : 26; /* bit[0-25] : 表示flashlock起始地址，高2比特表示flash片选，低24比特表示nandflash的row地址 */
        unsigned int  reserved              : 6;  /* bit[26-31]: 保留位。读时返回0。写时无影响。 */
    } reg;
} SOC_NANDC_NfLockStartAddr1_UNION;
#endif
#define SOC_NANDC_NfLockStartAddr1_flashlock start addr1_START  (0)
#define SOC_NANDC_NfLockStartAddr1_flashlock start addr1_END    (25)


/*****************************************************************************
 结构名    : SOC_NANDC_NfLockEndAddr1_UNION
 结构说明  : NfLockEndAddr1 寄存器结构定义。地址偏移量:0x2103C，初值:0x0，宽度:32
 寄存器说明: FlashLock起始结束寄存器1
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  flashlock end addr1 : 26; /* bit[0-25] : 表示flashlock结束地址，高2比特表示flash片选，低24比特表示nandflash的row地址 */
        unsigned int  reserved            : 6;  /* bit[26-31]: 保留位。读时返回0。写时无影响。 */
    } reg;
} SOC_NANDC_NfLockEndAddr1_UNION;
#endif
#define SOC_NANDC_NfLockEndAddr1_flashlock end addr1_START  (0)
#define SOC_NANDC_NfLockEndAddr1_flashlock end addr1_END    (25)


/*****************************************************************************
 结构名    : SOC_NANDC_NfLockStartAddr2_UNION
 结构说明  : NfLockStartAddr2 寄存器结构定义。地址偏移量:0x21040，初值:0x0，宽度:32
 寄存器说明: FlashLock起始地址寄存器2
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  flashlock start addr2 : 26; /* bit[0-25] : 表示flashlock起始地址，高2比特表示flash片选，低24比特表示nandflash的row地址 */
        unsigned int  reserved              : 6;  /* bit[26-31]: 保留位。读时返回0。写时无影响。 */
    } reg;
} SOC_NANDC_NfLockStartAddr2_UNION;
#endif
#define SOC_NANDC_NfLockStartAddr2_flashlock start addr2_START  (0)
#define SOC_NANDC_NfLockStartAddr2_flashlock start addr2_END    (25)


/*****************************************************************************
 结构名    : SOC_NANDC_NfLockEndAddr2_UNION
 结构说明  : NfLockEndAddr2 寄存器结构定义。地址偏移量:0x21044，初值:0x0，宽度:32
 寄存器说明: FlashLock起始结束寄存器2
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  flashlock end addr2 : 26; /* bit[0-25] : 表示flashlock结束地址，高2比特表示flash片选，低24比特表示nandflash的row地址 */
        unsigned int  reserved            : 6;  /* bit[26-31]: 保留位。读时返回0。写时无影响。 */
    } reg;
} SOC_NANDC_NfLockEndAddr2_UNION;
#endif
#define SOC_NANDC_NfLockEndAddr2_flashlock end addr2_START  (0)
#define SOC_NANDC_NfLockEndAddr2_flashlock end addr2_END    (25)


/*****************************************************************************
 结构名    : SOC_NANDC_NfLockStartAddr3_UNION
 结构说明  : NfLockStartAddr3 寄存器结构定义。地址偏移量:0x21048，初值:0x0，宽度:32
 寄存器说明: FlashLock起始地址寄存器3
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  flashlock start addr3 : 26; /* bit[0-25] : 表示flashlock起始地址，高2比特表示flash片选，低24比特表示nandflash的row地址 */
        unsigned int  reserved              : 6;  /* bit[26-31]: 保留位。读时返回0。写时无影响。 */
    } reg;
} SOC_NANDC_NfLockStartAddr3_UNION;
#endif
#define SOC_NANDC_NfLockStartAddr3_flashlock start addr3_START  (0)
#define SOC_NANDC_NfLockStartAddr3_flashlock start addr3_END    (25)


/*****************************************************************************
 结构名    : SOC_NANDC_NfLockEndAddr3_UNION
 结构说明  : NfLockEndAddr3 寄存器结构定义。地址偏移量:0x2104C，初值:0x0，宽度:32
 寄存器说明: FlashLock起始结束寄存器3
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  flashlock end addr3 : 26; /* bit[0-25] : 表示flashlock结束地址，高2比特表示flash片选，低24比特表示nandflash的row地址 */
        unsigned int  reserved            : 6;  /* bit[26-31]: 保留位。读时返回0。写时无影响。 */
    } reg;
} SOC_NANDC_NfLockEndAddr3_UNION;
#endif
#define SOC_NANDC_NfLockEndAddr3_flashlock end addr3_START  (0)
#define SOC_NANDC_NfLockEndAddr3_flashlock end addr3_END    (25)


/*****************************************************************************
 结构名    : SOC_NANDC_NfLockStartAddr4_UNION
 结构说明  : NfLockStartAddr4 寄存器结构定义。地址偏移量:0x21050，初值:0x0，宽度:32
 寄存器说明: FlashLock起始地址寄存器4
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  flashlock start addr4 : 26; /* bit[0-25] : 表示flashlock起始地址，高2比特表示flash片选，低24比特表示nandflash的row地址 */
        unsigned int  reserved              : 6;  /* bit[26-31]: 保留位。读时返回0。写时无影响。 */
    } reg;
} SOC_NANDC_NfLockStartAddr4_UNION;
#endif
#define SOC_NANDC_NfLockStartAddr4_flashlock start addr4_START  (0)
#define SOC_NANDC_NfLockStartAddr4_flashlock start addr4_END    (25)


/*****************************************************************************
 结构名    : SOC_NANDC_NfLockEndAddr4_UNION
 结构说明  : NfLockEndAddr4 寄存器结构定义。地址偏移量:0x21054，初值:0x0，宽度:32
 寄存器说明: FlashLock起始结束寄存器4
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  flashlock end addr4 : 26; /* bit[0-25] : 表示flashlock结束地址，高2比特表示flash片选，低24比特表示nandflash的row地址 */
        unsigned int  reserved            : 6;  /* bit[26-31]: 保留位。读时返回0。写时无影响。 */
    } reg;
} SOC_NANDC_NfLockEndAddr4_UNION;
#endif
#define SOC_NANDC_NfLockEndAddr4_flashlock end addr4_START  (0)
#define SOC_NANDC_NfLockEndAddr4_flashlock end addr4_END    (25)


/*****************************************************************************
 结构名    : SOC_NANDC_NfLockEn_UNION
 结构说明  : NfLockEn 寄存器结构定义。地址偏移量:0x21058，初值:0x3，宽度:32
 寄存器说明: FlashLock使能寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  flashlock whole enable : 1;  /* bit[0]   : FlashLock全局使能/禁止。
                                                                 0：禁止；
                                                                 1：使能（默认值）。 */
        unsigned int  flashlock enable       : 1;  /* bit[1]   : FlashLock使能/禁止。
                                                                 0：禁止；
                                                                 1：使能（默认值） */
        unsigned int  reserved               : 30; /* bit[2-31]: 保留位。读时返回0。写时无影响。 */
    } reg;
} SOC_NANDC_NfLockEn_UNION;
#endif
#define SOC_NANDC_NfLockEn_flashlock whole enable_START  (0)
#define SOC_NANDC_NfLockEn_flashlock whole enable_END    (0)
#define SOC_NANDC_NfLockEn_flashlock enable_START        (1)
#define SOC_NANDC_NfLockEn_flashlock enable_END          (1)


/*****************************************************************************
 结构名    : SOC_NANDC_NfLockCont_UNION
 结构说明  : NfLockCont 寄存器结构定义。地址偏移量:0x2105C，初值:0x0，宽度:32
 寄存器说明: FlashLock控制寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  flashlock control : 1;  /* bit[0]   : 表示flashlock模式：
                                                            0表示：lock模式；
                                                            1表示：lock-down模式
                                                            该比特一次性写1配置 */
        unsigned int  reserved          : 31; /* bit[1-31]: 保留位。读时返回0。写时无影响。 */
    } reg;
} SOC_NANDC_NfLockCont_UNION;
#endif
#define SOC_NANDC_NfLockCont_flashlock control_START  (0)
#define SOC_NANDC_NfLockCont_flashlock control_END    (0)


/*****************************************************************************
 结构名    : SOC_NANDC_NflockCommand1_UNION
 结构说明  : NflockCommand1 寄存器结构定义。地址偏移量:0x21060，初值:0x85816060，宽度:32
 寄存器说明: FlashLock命令配置寄存器1
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  Block_command1 : 8;  /* bit[0-7]  : 表示flashlock block操作命令寄存器1 */
        unsigned int  Block_command2 : 8;  /* bit[8-15] : 表示flashlock block操作命令寄存器2 */
        unsigned int  Page_command1  : 8;  /* bit[16-23]: 表示flashlock page操作命令寄存器1 */
        unsigned int  Page_command2  : 8;  /* bit[24-31]: 表示flashlock page操作命令寄存器2 */
    } reg;
} SOC_NANDC_NflockCommand1_UNION;
#endif
#define SOC_NANDC_NflockCommand1_Block_command1_START  (0)
#define SOC_NANDC_NflockCommand1_Block_command1_END    (7)
#define SOC_NANDC_NflockCommand1_Block_command2_START  (8)
#define SOC_NANDC_NflockCommand1_Block_command2_END    (15)
#define SOC_NANDC_NflockCommand1_Page_command1_START   (16)
#define SOC_NANDC_NflockCommand1_Page_command1_END     (23)
#define SOC_NANDC_NflockCommand1_Page_command2_START   (24)
#define SOC_NANDC_NflockCommand1_Page_command2_END     (31)


/*****************************************************************************
 结构名    : SOC_NANDC_NflockCommand2_UNION
 结构说明  : NflockCommand2 寄存器结构定义。地址偏移量:0x21064，初值:0x85858585，宽度:32
 寄存器说明: FlashLock命令配置寄存器2
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  Page_command3 : 8;  /* bit[0-7]  : 表示flashlock page操作命令寄存器3 */
        unsigned int  Page_command4 : 8;  /* bit[8-15] : 表示flashlock page操作命令寄存器4 */
        unsigned int  Page_command5 : 8;  /* bit[16-23]: 表示flashlock page操作命令寄存器5 */
        unsigned int  Page_command6 : 8;  /* bit[24-31]: 表示flashlock page操作命令寄存器6 */
    } reg;
} SOC_NANDC_NflockCommand2_UNION;
#endif
#define SOC_NANDC_NflockCommand2_Page_command3_START  (0)
#define SOC_NANDC_NflockCommand2_Page_command3_END    (7)
#define SOC_NANDC_NflockCommand2_Page_command4_START  (8)
#define SOC_NANDC_NflockCommand2_Page_command4_END    (15)
#define SOC_NANDC_NflockCommand2_Page_command5_START  (16)
#define SOC_NANDC_NflockCommand2_Page_command5_END    (23)
#define SOC_NANDC_NflockCommand2_Page_command6_START  (24)
#define SOC_NANDC_NflockCommand2_Page_command6_END    (31)






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

#endif /* end of soc_nandc_interface.h */
