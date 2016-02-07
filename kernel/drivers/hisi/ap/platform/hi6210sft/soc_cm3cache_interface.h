

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/

#ifndef __SOC_CM3CACHE_INTERFACE_H__
#define __SOC_CM3CACHE_INTERFACE_H__

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
/* 寄存器说明：Cache控制寄存器
   位域定义UNION结构:  SOC_CM3CACHE_CACHE_CTRL_UNION */
#define SOC_CM3CACHE_CACHE_CTRL_ADDR(base)            ((base) + (0x0))

/* 寄存器说明：Cache状态寄存器
   位域定义UNION结构:  SOC_CM3CACHE_CACHE_STAT_UNION */
#define SOC_CM3CACHE_CACHE_STAT_ADDR(base)            ((base) + (0x4))

/* 寄存器说明：Cache清除寄存器
   位域定义UNION结构:  SOC_CM3CACHE_CACHE_CLR_UNION */
#define SOC_CM3CACHE_CACHE_CLR_ADDR(base)             ((base) + (0x8))

/* 寄存器说明：PU控制寄存器
   位域定义UNION结构:  SOC_CM3CACHE_PU_CTRL_UNION */
#define SOC_CM3CACHE_PU_CTRL_ADDR(base)               ((base) + (0xC))

/* 寄存器说明：PU Cacheable控制寄存器
   位域定义UNION结构:  SOC_CM3CACHE_PU_CACHEABLE_UNION */
#define SOC_CM3CACHE_PU_CACHEABLE_ADDR(base)          ((base) + (0x10))

/* 寄存器说明：PU Bufferable控制寄存器
   位域定义UNION结构:  SOC_CM3CACHE_PU_BUFFERABLE_UNION */
#define SOC_CM3CACHE_PU_BUFFERABLE_ADDR(base)         ((base) + (0x14))

/* 寄存器说明：PU Protection控制寄存器
   位域定义UNION结构:  SOC_CM3CACHE_PU_PROTECTION_UNION */
#define SOC_CM3CACHE_PU_PROTECTION_ADDR(base)         ((base) + (0x18))

/* 寄存器说明：PU地址段设置寄存器0
   位域定义UNION结构:  SOC_CM3CACHE_PU_AREA0_UNION */
#define SOC_CM3CACHE_PU_AREA0_ADDR(base)              ((base) + (0x1C))

/* 寄存器说明：PU地址段设置寄存器1
   位域定义UNION结构:  SOC_CM3CACHE_PU_AREA1_UNION */
#define SOC_CM3CACHE_PU_AREA1_ADDR(base)              ((base) + (0x20))

/* 寄存器说明：PU地址段设置寄存器2
   位域定义UNION结构:  SOC_CM3CACHE_PU_AREA2_UNION */
#define SOC_CM3CACHE_PU_AREA2_ADDR(base)              ((base) + (0x24))

/* 寄存器说明：PU地址段设置寄存器3
   位域定义UNION结构:  SOC_CM3CACHE_PU_AREA3_UNION */
#define SOC_CM3CACHE_PU_AREA3_ADDR(base)              ((base) + (0x28))





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
 结构名    : SOC_CM3CACHE_CACHE_CTRL_UNION
 结构说明  : CACHE_CTRL 寄存器结构定义。地址偏移量:0x0，初值:0x00000000，宽度:32
 寄存器说明: Cache控制寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  CACHEEN      : 1;  /* bit[0]   : Cache使能位
                                                       1：Cache使能
                                                       0：Cache未使能 */
        unsigned int  CACHEFLUSH   : 1;  /* bit[1]   : Cache Flush或Clear控制位
                                                       1：读Cache_CLR寄存器进行Flush操作
                                                       0：读Cache_CLR寄存器为Clear操作 */
        unsigned int  LOCKABORTDIS : 1;  /* bit[2]   : SWP使能位
                                                       1：SWP在任何地址均可执行
                                                       0：Non-cacheable和Non-bufferable地址的SWP操作可以正常执行，但Cacheable或Bufferable的SWP操作将被中止并报错 */
        unsigned int  CACHEABORT   : 1;  /* bit[3]   : 寄存器中止位，表示CPU上一次对寄存器的访问为非法操作，该位需要软件清零，此外，该位可软件置位。 */
        unsigned int  LOCKABORT    : 1;  /* bit[4]   : LOCK中止位，表示CPU上一次SWP操作非法，即地址为Cacheable或Bufferable且SWP使能未打开，该位需要软件清零，此外，该位可软件置位。 */
        unsigned int  USERABORT    : 1;  /* bit[5]   : 保护中止位，表示CPU上一次访问地址非法，即CPU在用户模式访问被保护的特权模式地址，该位需要软件清零，此外，该位可软件置位。 */
        unsigned int  AHBABORT     : 1;  /* bit[6]   : AHB中止位，表示CPU上一次对外设地址的访问为非法操作，AHB总线的访问返回信息为错误，该位需要软件清零，此外，该位可软件置位。 */
        unsigned int  reserved     : 25; /* bit[7-31]: Reserved */
    } reg;
} SOC_CM3CACHE_CACHE_CTRL_UNION;
#endif
#define SOC_CM3CACHE_CACHE_CTRL_CACHEEN_START       (0)
#define SOC_CM3CACHE_CACHE_CTRL_CACHEEN_END         (0)
#define SOC_CM3CACHE_CACHE_CTRL_CACHEFLUSH_START    (1)
#define SOC_CM3CACHE_CACHE_CTRL_CACHEFLUSH_END      (1)
#define SOC_CM3CACHE_CACHE_CTRL_LOCKABORTDIS_START  (2)
#define SOC_CM3CACHE_CACHE_CTRL_LOCKABORTDIS_END    (2)
#define SOC_CM3CACHE_CACHE_CTRL_CACHEABORT_START    (3)
#define SOC_CM3CACHE_CACHE_CTRL_CACHEABORT_END      (3)
#define SOC_CM3CACHE_CACHE_CTRL_LOCKABORT_START     (4)
#define SOC_CM3CACHE_CACHE_CTRL_LOCKABORT_END       (4)
#define SOC_CM3CACHE_CACHE_CTRL_USERABORT_START     (5)
#define SOC_CM3CACHE_CACHE_CTRL_USERABORT_END       (5)
#define SOC_CM3CACHE_CACHE_CTRL_AHBABORT_START      (6)
#define SOC_CM3CACHE_CACHE_CTRL_AHBABORT_END        (6)


/*****************************************************************************
 结构名    : SOC_CM3CACHE_CACHE_STAT_UNION
 结构说明  : CACHE_STAT 寄存器结构定义。地址偏移量:0x4，初值:0x4，宽度:32
 寄存器说明: Cache状态寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  CACHESIZE : 3;  /* bit[0-2] : 指示当前Cache的大小
                                                    3'b010: 4KB
                                                    3'b011: 8KB
                                                    3'b100: 16KB */
        unsigned int  reserved  : 29; /* bit[3-31]: Reserved */
    } reg;
} SOC_CM3CACHE_CACHE_STAT_UNION;
#endif
#define SOC_CM3CACHE_CACHE_STAT_CACHESIZE_START  (0)
#define SOC_CM3CACHE_CACHE_STAT_CACHESIZE_END    (2)


/*****************************************************************************
 结构名    : SOC_CM3CACHE_CACHE_CLR_UNION
 结构说明  : CACHE_CLR 寄存器结构定义。地址偏移量:0x8，初值:0x00000000，宽度:32
 寄存器说明: Cache清除寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  FLUSHEDWAY  : 2;  /* bit[0-1] :  */
        unsigned int  FLUSHEDLINE : 7;  /* bit[2-8] : Cache清除寄存器，该寄存器可读写。
                                                      写该寄存器表示设置对Cache进行清除的起始地址。读该寄存器产生实际的清除操作，该清除操作完成时可自动增一，即表示设置好初始地址后不断对其进行读操作即可以完成Cache的Flush或Clear操作。
                                                      Clear操作在一次读操作完成某特定Line的四路全清除，即Clear操作只需要给出Line地址，Way地址无效。假设Cache大小为16KB，则需要128次读操作完成整个Cache的Clear，该操作在Reset后必须进行。
                                                      Flush操作在一次读操作完成某特定Line的某一路的全写出，该操作判断被选择的数据块是否Valid和是否Dirty，如果Valid且Dirty，则将该数据块写入主存。Flush操作需要给出Line地址和Way地址，Way地址为最低两位。假设Cache大小为16KB，则需要512次读操作完成整个Cache的Flush。为保证数据的一致性，该操作在Cache 使能关闭前必须进行。
                                                      Clear操作完成后，被操作的4路数据块的Valid位均置零，Dirty位置零，LRU位清零，即第一次替换时，被替换数据为第四路。
                                                      Flush操作完成后，被操作的数据块的Valid位置零，Dirty位置零，LRU位也清零。 */
        unsigned int  reserved    : 23; /* bit[9-31]: Reserved */
    } reg;
} SOC_CM3CACHE_CACHE_CLR_UNION;
#endif
#define SOC_CM3CACHE_CACHE_CLR_FLUSHEDWAY_START   (0)
#define SOC_CM3CACHE_CACHE_CLR_FLUSHEDWAY_END     (1)
#define SOC_CM3CACHE_CACHE_CLR_FLUSHEDLINE_START  (2)
#define SOC_CM3CACHE_CACHE_CLR_FLUSHEDLINE_END    (8)


/*****************************************************************************
 结构名    : SOC_CM3CACHE_PU_CTRL_UNION
 结构说明  : PU_CTRL 寄存器结构定义。地址偏移量:0xC，初值:0x00000000，宽度:32
 寄存器说明: PU控制寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  PUENABLE : 1;  /* bit[0]   : PU使能位，该寄存器可读写，使能位使能时，PU将对数据进行Cacheable、Bufferable、Protection进行判定，如将该位关闭，则PU将不进行判定，所有地址都是Non-cacheable、Non-bufferable，非Protection的操作。 */
        unsigned int  PUAMBUF  : 1;  /* bit[1]   : PU未命中Bufferable控制位，PU所有Area段均未命中时PU的Bufferable机制，该位默认为零，此时PU如果未命中则为Non-bufferable地址，置一时未命中为Bufferable地址。 */
        unsigned int  PUAMCAC  : 1;  /* bit[2]   : PU未命中Cacheable控制位，PU所有Area段均未命中时PU的Cacheable机制，该位默认为零，此时PU如果未命中则为Non-cacheable，置一时未命中为Cacheable地址。 */
        unsigned int  PUAMPRO  : 1;  /* bit[3]   : PU未命中保护控制位，PU所有Area段均未命中时PU的保护机制，该位默认为零，此时PU如果未命中则为非保护地址，置一时未命中为保护地址。 */
        unsigned int  reserved : 28; /* bit[4-31]: Reserved */
    } reg;
} SOC_CM3CACHE_PU_CTRL_UNION;
#endif
#define SOC_CM3CACHE_PU_CTRL_PUENABLE_START  (0)
#define SOC_CM3CACHE_PU_CTRL_PUENABLE_END    (0)
#define SOC_CM3CACHE_PU_CTRL_PUAMBUF_START   (1)
#define SOC_CM3CACHE_PU_CTRL_PUAMBUF_END     (1)
#define SOC_CM3CACHE_PU_CTRL_PUAMCAC_START   (2)
#define SOC_CM3CACHE_PU_CTRL_PUAMCAC_END     (2)
#define SOC_CM3CACHE_PU_CTRL_PUAMPRO_START   (3)
#define SOC_CM3CACHE_PU_CTRL_PUAMPRO_END     (3)


/*****************************************************************************
 结构名    : SOC_CM3CACHE_PU_CACHEABLE_UNION
 结构说明  : PU_CACHEABLE 寄存器结构定义。地址偏移量:0x10，初值:0x00000000，宽度:32
 寄存器说明: PU Cacheable控制寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  CACHEABLE0 : 1;  /* bit[0]   :  */
        unsigned int  CACHEABLE1 : 1;  /* bit[1]   :  */
        unsigned int  CACHEABLE2 : 1;  /* bit[2]   :  */
        unsigned int  CACHEABLE3 : 1;  /* bit[3]   : Cacheable设置，可读写。4个标志位各对应一个地址端，表示该地址段是否为Cacheable地址段。 */
        unsigned int  reserved   : 28; /* bit[4-31]: Reserved */
    } reg;
} SOC_CM3CACHE_PU_CACHEABLE_UNION;
#endif
#define SOC_CM3CACHE_PU_CACHEABLE_CACHEABLE0_START  (0)
#define SOC_CM3CACHE_PU_CACHEABLE_CACHEABLE0_END    (0)
#define SOC_CM3CACHE_PU_CACHEABLE_CACHEABLE1_START  (1)
#define SOC_CM3CACHE_PU_CACHEABLE_CACHEABLE1_END    (1)
#define SOC_CM3CACHE_PU_CACHEABLE_CACHEABLE2_START  (2)
#define SOC_CM3CACHE_PU_CACHEABLE_CACHEABLE2_END    (2)
#define SOC_CM3CACHE_PU_CACHEABLE_CACHEABLE3_START  (3)
#define SOC_CM3CACHE_PU_CACHEABLE_CACHEABLE3_END    (3)


/*****************************************************************************
 结构名    : SOC_CM3CACHE_PU_BUFFERABLE_UNION
 结构说明  : PU_BUFFERABLE 寄存器结构定义。地址偏移量:0x14，初值:0x00000000，宽度:32
 寄存器说明: PU Bufferable控制寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  BUFFERABLE0 : 1;  /* bit[0]   :  */
        unsigned int  BUFFERABLE1 : 1;  /* bit[1]   :  */
        unsigned int  BUFFERABLE2 : 1;  /* bit[2]   :  */
        unsigned int  BUFFERABLE3 : 1;  /* bit[3]   : Bufferable设置，可读写。4个标志位各对应一个地址端，表示该地址段是否为Bufferable地址段。 */
        unsigned int  reserved    : 28; /* bit[4-31]: Reserved */
    } reg;
} SOC_CM3CACHE_PU_BUFFERABLE_UNION;
#endif
#define SOC_CM3CACHE_PU_BUFFERABLE_BUFFERABLE0_START  (0)
#define SOC_CM3CACHE_PU_BUFFERABLE_BUFFERABLE0_END    (0)
#define SOC_CM3CACHE_PU_BUFFERABLE_BUFFERABLE1_START  (1)
#define SOC_CM3CACHE_PU_BUFFERABLE_BUFFERABLE1_END    (1)
#define SOC_CM3CACHE_PU_BUFFERABLE_BUFFERABLE2_START  (2)
#define SOC_CM3CACHE_PU_BUFFERABLE_BUFFERABLE2_END    (2)
#define SOC_CM3CACHE_PU_BUFFERABLE_BUFFERABLE3_START  (3)
#define SOC_CM3CACHE_PU_BUFFERABLE_BUFFERABLE3_END    (3)


/*****************************************************************************
 结构名    : SOC_CM3CACHE_PU_PROTECTION_UNION
 结构说明  : PU_PROTECTION 寄存器结构定义。地址偏移量:0x18，初值:0x00000000，宽度:32
 寄存器说明: PU Protection控制寄存器
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  PERMISSION0 : 1;  /* bit[0]   :  */
        unsigned int  PERMISSION1 : 1;  /* bit[1]   :  */
        unsigned int  PERMISSION2 : 1;  /* bit[2]   :  */
        unsigned int  PERMISSION3 : 1;  /* bit[3]   : Protection设置，可读写。4个标志位各对应一个地址端，表示该地址段是否需要在用户模式下进行保护。 */
        unsigned int  reserved    : 28; /* bit[4-31]: Reserved */
    } reg;
} SOC_CM3CACHE_PU_PROTECTION_UNION;
#endif
#define SOC_CM3CACHE_PU_PROTECTION_PERMISSION0_START  (0)
#define SOC_CM3CACHE_PU_PROTECTION_PERMISSION0_END    (0)
#define SOC_CM3CACHE_PU_PROTECTION_PERMISSION1_START  (1)
#define SOC_CM3CACHE_PU_PROTECTION_PERMISSION1_END    (1)
#define SOC_CM3CACHE_PU_PROTECTION_PERMISSION2_START  (2)
#define SOC_CM3CACHE_PU_PROTECTION_PERMISSION2_END    (2)
#define SOC_CM3CACHE_PU_PROTECTION_PERMISSION3_START  (3)
#define SOC_CM3CACHE_PU_PROTECTION_PERMISSION3_END    (3)


/*****************************************************************************
 结构名    : SOC_CM3CACHE_PU_AREA0_UNION
 结构说明  : PU_AREA0 寄存器结构定义。地址偏移量:0x1C，初值:0x00000000，宽度:32
 寄存器说明: PU地址段设置寄存器0
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  AREAENABLE : 1;  /* bit[0]    : 使能位,表示该地址端是否进行PU判定 */
        unsigned int  AREASIZE   : 5;  /* bit[1-5]  : 地址大小5 bits配置意义如下表所示，Size值必须大于01011，如果小于01010则置为01010。设置地址的大小和基地址必须对齐，例如，当Size设置为16KB时，0x8000是合法的基地址，0x5000是非法地址，将被按位与为0x4000。
                                                      5'b01011: 4KB 5'b01100: 8KB 5'b01101: 16KB
                                                      5'b01110:32KB 5'b01111: 64KB 5'b10000:128KB
                                                      5'b10001:256KB 5'b10010:512KB 5'b10011:1MB
                                                      5'b10100:2MB 5'b10101:4MB 5'b10110:8MB
                                                      5'b10111:16MB 5'b11000:32MB 5'b11001:64MB
                                                      5'b11010:128MB 5'b11011:256MB 5'b11100:512MB
                                                      5'b11101:1GB 5'b11110:2GB 5'b11111:4GB */
        unsigned int  reserved   : 6;  /* bit[6-11] : reserved */
        unsigned int  BASE       : 20; /* bit[12-31]: 地址段的基地址设置只能设置地址的高20位，即最小跨度为4KB。 */
    } reg;
} SOC_CM3CACHE_PU_AREA0_UNION;
#endif
#define SOC_CM3CACHE_PU_AREA0_AREAENABLE_START  (0)
#define SOC_CM3CACHE_PU_AREA0_AREAENABLE_END    (0)
#define SOC_CM3CACHE_PU_AREA0_AREASIZE_START    (1)
#define SOC_CM3CACHE_PU_AREA0_AREASIZE_END      (5)
#define SOC_CM3CACHE_PU_AREA0_BASE_START        (12)
#define SOC_CM3CACHE_PU_AREA0_BASE_END          (31)


/*****************************************************************************
 结构名    : SOC_CM3CACHE_PU_AREA1_UNION
 结构说明  : PU_AREA1 寄存器结构定义。地址偏移量:0x20，初值:0x00000000，宽度:32
 寄存器说明: PU地址段设置寄存器1
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  AREAENABLE : 1;  /* bit[0]    : 使能位,表示该地址端是否进行PU判定 */
        unsigned int  AREASIZE   : 5;  /* bit[1-5]  : 地址大小5 bits配置意义如下表所示，Size值必须大于01011，如果小于01010则置为01010。设置地址的大小和基地址必须对齐，例如，当Size设置为16KB时，0x8000是合法的基地址，0x5000是非法地址，将被按位与为0x4000。
                                                      5'b01011: 4KB 5'b01100: 8KB 5'b01101: 16KB
                                                      5'b01110:32KB 5'b01111: 64KB 5'b10000:128KB
                                                      5'b10001:256KB 5'b10010:512KB 5'b10011:1MB
                                                      5'b10100:2MB 5'b10101:4MB 5'b10110:8MB
                                                      5'b10111:16MB 5'b11000:32MB 5'b11001:64MB
                                                      5'b11010:128MB 5'b11011:256MB 5'b11100:512MB
                                                      5'b11101:1GB 5'b11110:2GB 5'b11111:4GB */
        unsigned int  reserved   : 6;  /* bit[6-11] : reserved */
        unsigned int  BASE       : 20; /* bit[12-31]: 地址段的基地址设置只能设置地址的高20位，即最小跨度为4KB。 */
    } reg;
} SOC_CM3CACHE_PU_AREA1_UNION;
#endif
#define SOC_CM3CACHE_PU_AREA1_AREAENABLE_START  (0)
#define SOC_CM3CACHE_PU_AREA1_AREAENABLE_END    (0)
#define SOC_CM3CACHE_PU_AREA1_AREASIZE_START    (1)
#define SOC_CM3CACHE_PU_AREA1_AREASIZE_END      (5)
#define SOC_CM3CACHE_PU_AREA1_BASE_START        (12)
#define SOC_CM3CACHE_PU_AREA1_BASE_END          (31)


/*****************************************************************************
 结构名    : SOC_CM3CACHE_PU_AREA2_UNION
 结构说明  : PU_AREA2 寄存器结构定义。地址偏移量:0x24，初值:0x00000000，宽度:32
 寄存器说明: PU地址段设置寄存器2
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  AREAENABLE : 1;  /* bit[0]    : 使能位,表示该地址端是否进行PU判定 */
        unsigned int  AREASIZE   : 5;  /* bit[1-5]  : 地址大小5 bits配置意义如下表所示，Size值必须大于01011，如果小于01010则置为01010。设置地址的大小和基地址必须对齐，例如，当Size设置为16KB时，0x8000是合法的基地址，0x5000是非法地址，将被按位与为0x4000。
                                                      5'b01011: 4KB 5'b01100: 8KB 5'b01101: 16KB
                                                      5'b01110:32KB 5'b01111: 64KB 5'b10000:128KB
                                                      5'b10001:256KB 5'b10010:512KB 5'b10011:1MB
                                                      5'b10100:2MB 5'b10101:4MB 5'b10110:8MB
                                                      5'b10111:16MB 5'b11000:32MB 5'b11001:64MB
                                                      5'b11010:128MB 5'b11011:256MB 5'b11100:512MB
                                                      5'b11101:1GB 5'b11110:2GB 5'b11111:4GB */
        unsigned int  reserved   : 6;  /* bit[6-11] : reserved */
        unsigned int  BASE       : 20; /* bit[12-31]: 地址段的基地址设置只能设置地址的高20位，即最小跨度为4KB。 */
    } reg;
} SOC_CM3CACHE_PU_AREA2_UNION;
#endif
#define SOC_CM3CACHE_PU_AREA2_AREAENABLE_START  (0)
#define SOC_CM3CACHE_PU_AREA2_AREAENABLE_END    (0)
#define SOC_CM3CACHE_PU_AREA2_AREASIZE_START    (1)
#define SOC_CM3CACHE_PU_AREA2_AREASIZE_END      (5)
#define SOC_CM3CACHE_PU_AREA2_BASE_START        (12)
#define SOC_CM3CACHE_PU_AREA2_BASE_END          (31)


/*****************************************************************************
 结构名    : SOC_CM3CACHE_PU_AREA3_UNION
 结构说明  : PU_AREA3 寄存器结构定义。地址偏移量:0x28，初值:0x00000000，宽度:32
 寄存器说明: PU地址段设置寄存器3
*****************************************************************************/
#ifndef __SOC_H_FOR_ASM__
typedef union
{
    unsigned int      value;
    struct
    {
        unsigned int  AREAENABLE : 1;  /* bit[0]    : 使能位,表示该地址端是否进行PU判定 */
        unsigned int  AREASIZE   : 5;  /* bit[1-5]  : 地址大小5 bits配置意义如下表所示，Size值必须大于01011，如果小于01010则置为01010。设置地址的大小和基地址必须对齐，例如，当Size设置为16KB时，0x8000是合法的基地址，0x5000是非法地址，将被按位与为0x4000。
                                                      5'b01011: 4KB 5'b01100: 8KB 5'b01101: 16KB
                                                      5'b01110:32KB 5'b01111: 64KB 5'b10000:128KB
                                                      5'b10001:256KB 5'b10010:512KB 5'b10011:1MB
                                                      5'b10100:2MB 5'b10101:4MB 5'b10110:8MB
                                                      5'b10111:16MB 5'b11000:32MB 5'b11001:64MB
                                                      5'b11010:128MB 5'b11011:256MB 5'b11100:512MB
                                                      5'b11101:1GB 5'b11110:2GB 5'b11111:4GB */
        unsigned int  reserved   : 6;  /* bit[6-11] : reserved */
        unsigned int  BASE       : 20; /* bit[12-31]: 地址段的基地址设置只能设置地址的高20位，即最小跨度为4KB。 */
    } reg;
} SOC_CM3CACHE_PU_AREA3_UNION;
#endif
#define SOC_CM3CACHE_PU_AREA3_AREAENABLE_START  (0)
#define SOC_CM3CACHE_PU_AREA3_AREAENABLE_END    (0)
#define SOC_CM3CACHE_PU_AREA3_AREASIZE_START    (1)
#define SOC_CM3CACHE_PU_AREA3_AREASIZE_END      (5)
#define SOC_CM3CACHE_PU_AREA3_BASE_START        (12)
#define SOC_CM3CACHE_PU_AREA3_BASE_END          (31)






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

#endif /* end of soc_cm3cache_interface.h */
