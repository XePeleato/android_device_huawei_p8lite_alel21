
#ifndef __DRV_IPC_ENUM_H__
#define __DRV_IPC_ENUM_H__
/* 处理器类型*/
typedef enum tagIPC_INT_CORE_E
{
    IPC_CORE_ARM11 = 0x0,   /* P500上的IPC */
    IPC_CORE_A9,
    IPC_CORE_CEVA,
    IPC_CORE_TENS0,
    IPC_CORE_TENS1,
    IPC_CORE_DSP,

    IPC_CORE_APPARM = 0x0,  /* V7R1/V3R2上的IPC */
    IPC_CORE_COMARM,
    IPC_CORE_LTEDSP,
    IPC_CORE_VDSP,
    IPC_CORE_ZSP,
    IPC_CORE_DSP_GU,

    IPC_CORE_ACPU = 0x0,/* V9R1 SFT上的IPC */
    IPC_CORE_CCPU,
    IPC_CORE_MCU,
    IPC_CORE_HIFI,
    IPC_CORE_BBE16,

    IPC_CORE_ACORE=0x0,   /*P531,V7R2*/
    IPC_CORE_CCORE,
    IPC_CORE_MCORE,
    IPC_CORE_LDSP,
    IPC_CORE_HiFi,
    IPC_CORE_BBE,          /*以上5个核P531\V7R2都存在，以下三个核只在P531存在*/
    IPC_CORE0_A15,
    IPC_CORE1_A15,

    /* !!!!新增元素请添加到最后  */
    IPC_CORE_BUTTOM
}IPC_INT_CORE_E;

typedef enum tagIPC_INT_LEV_E
{
   /* 定义CCPU IPC跨核消息中断源bit位置 */
   IPC_CCPU_INT_SRC_HIFI_MSG                        = 1,      /*bit1,  邮箱消息，HIFI 发给CCPU 的IPC中断                                 */
   IPC_INT_DSP_MBX_RSD                              = 3,      /*bit3,  预留的DSP邮箱中断                                                 */
   IPC_INT_DSP_PS_PUB_MBX                           = 4,      /*bit4,  v7r2邮箱消息，PS共用邮箱中断,DSP发给CCPU的IPC中断                 */
   IPC_CCPU_INT_SRC_MCU_MSG                         = 5,      /*bit5,  邮箱消息，MCU发给CCPU 的IPC中断                                   */
   IPC_INT_DICC_UsRDATA                             = 6,      /*bit6,  V7R2 为csd业务保留，ACPU发送给CCPU的IPC中断                       */
   IPC_INT_DSP_PS_MAC_MBX                           = 7,      /*bit7,  v7r2 mac phy data ind中断                                         */
   IPC_INT_DSP_HALT                                 = 8,      /*bit8,  v7r2保留，DSP通知ARM睡眠，DSP发给CCPU的IPC中断                    */
   IPC_INT_DSP_RESUME                               = 9,      /*bit9,  v7r2保留，DSP通知ARM完成唤醒后的恢复操作,DSP发给CCPU的IPC中断     */
   IPC_INT_WAKE_GU                                  =11,      /*bit11, v7r2保留，主模唤醒从模中断，DSP发给CCPU的IPC中断                  */
   IPC_INT_SLEEP_GU                                 =12,      /*bit12, v7r2保留，从模睡眠中断，DSP发给CCPU的IPC中断                      */
   IPC_CCPU_INT_SRC_ICC                             =13,      /*bit13, v7r2 ICC通信，ACPU 发给CCPU 的IPC中断 ,ACPU发送给CCPU的IPC中断    */
   IPC_CCPU_INT_SRC_ICC_PRIVATE                     =14,      /*bit14, v7r2 ICC通信，icc专用发送消息使用  ,ACPU发送给CCPU的IPC中断       */
   IPC_CCPU_SRC_ACPU_DUMP                           =15,      /*bit15, v7r2 可维可测，ACPU 发送给CCPU 的IPC中断                          */
   IPC_INT_DICC_RELDATA                             =16,      /*原为7号，gu 已确认删除，但JB分支还在使用，7号被dsp占用，添加为编译通过   */
   IPC_CCPU_INT_SRC_MCPU                            =17,      /*bit17,   ICC消息, MCU发送给CCPU的  IPC中断                               */
   IPC_CCPU_INT_SRC_MCPU_WDT						=18,    /*运行trace时停wdt */

   IPC_INT_WAKE_GSM                                 = 19,   /* bit16, BBE16通知CCPU,唤醒GSM从模 */
   IPC_INT_WAKE_WCDMA                               = 20,   /* bit17, BBE16通知CCPU,唤醒WCDMA从模 */
   IPC_CCPU_INT_SRC_DSP_MNTN                        = 21,   /* bit21，BBE16可维可测中断通知mailbox，崔军强 */
   IPC_CCPU_INT_SRC_ACPU_RESET                      = 22,   /* bit22, A/C核在C核单独复位期间核间通信(唤醒C核),姚国才 */

   IPC_CCPU_INT_SRC_MCPU_FOR_TEST                   =29,      /*BIT29, FOR TEST interact with m3                                         */
   IPC_CCPU_INT_SRC_ACPU_TEST_ENABLE                =31,      /*bit31, IPC自动化测试中断使能时使用，后续IPC中断不够使用时考虑删除        */
   IPC_CCPU_INT_SRC_BUTT                            =32,

   /* 定义MCU IPC跨核消息中断源bit位置 */
    IPC_MCU_INT_SRC_ACPU_DRX                        = 0,      /*ACPU给CCPU发送IPC中断时，需要同时告知给M3                                */
    IPC_MCU_INT_SRC_CCPU_DRX                        = 1,      /*CCPU给ACPU发送IPC中断时，需要同时告知给M3                                */
    IPC_MCU_INT_SRC_ACPU_MSG                        = 4,      /* bit4,  邮箱消息，    ACPU发送给MCU的IPC中断                             */
    IPC_MCU_INT_SRC_CCPU_MSG                        = 5,      /* bit5,  邮箱消息，    CCPU发送给MCU的IPC中断                             */
    IPC_MCU_INT_SRC_HIFI_MSG                        = 6,      /* bit6,  邮箱消息，    HIFI发送给MCU的 IPC中断                            */
    IPC_MCU_INT_SRC_CCPU_IPF                        = 7,      /*bit7,   IPF恢复专用，CCPU发送给MCU的IPC中断                              */
    IPC_MCU_INT_SRC_ACPU_PD                         =16,      /*bit16,  POWERDOWN,   ACPU发送给MCU的IPC中断                              */
    IPC_MCU_INT_SRC_CCPU_PD                         =17,      /*bit17,  POWERDOWN,   CCPU发送给MCU的IPC中断                              */
    IPC_MCU_INT_SRC_HIFI_PD                         =18,      /*bit18,   POWERDOWN,  HIFI发送给MCU的IPC中断                              */
    IPC_MCU_INT_SRC_ICC                             =19,      /*bit19,   m3 icc公用的ipc中断                                             */
    IPC_MCU_INT_SRC_ICC_PRIVATE                     =20,      /*bit20,    m3 icc私有的ipc中断，高性能通道，启独立任务用                  */
    IPC_MCU_INT_SRC_DUMP                            =21,      /* bit21 m3 dump中断，ACORE发送给MCU的IPC中断*/
    IPC_MCU_INT_SRC_HIFI_PU                         =22,      /* bit22,   POWERUP,  HIFI发送给MCU的IPC中断                              */
    IPC_MCU_INT_SRC_HIFI_DDR_DFS_QOS                =23,      /*BIT23, HIFI对DDR调频投票*/
    IPC_MCU_INT_SRC_ACPU_I2S_REMOTE_SLOW            =24,      /*bit24, notify mcu into slow state*/
    IPC_MCU_INT_SRC_ACPU_I2S_REMOTE_SLEEP           =25,      /*bit25, notify mcu into sleep state*/
    IPC_MCU_INT_SRC_ACPU_I2S_REMOTE_INVALID         =26,      /*bit26, 通知MCU全系统下电 */
    IPC_MCU_INT_SRC_CCPU_START                      =27,      /*bit27, 通知MCU启动CCPU */
	IPC_MCU_INT_SRC_HIFI_DDR_VOTE          			=28,      /*HIFI DDR自刷新*/
    IPC_MCU_INT_SRC_TEST                            =31,      /*BIT31, for test a\c interact with m3                                     */

    IPC_MCU_INT_SRC_BUTT = 32,

    /* 定义ACPU IPC跨核消息中断源bit位置 */
    IPC_ACPU_INT_SRC_CCPU_MSG                       = 1,        /* bit1,  邮箱消息，    CCPU发送给ACPU的IPC中断                          */
    IPC_ACPU_INT_SRC_HIFI_MSG                       = 2,        /* bit2,  邮箱消息，    HIFI发送给ACPU的IPC中断                          */
    IPC_ACPU_INT_SRC_MCU_MSG                        = 3,        /* bit3,  邮箱消息，    MCU发送给ACPU的IPC中断                           */
    IPC_ACPU_INT_SRC_CCPU_NVIM                      = 4,        /* bit4,  NVIM，        CCPU发送给ACPU的IPC中断                          */
    IPC_INT_DICC_USRDATA                            = 6,        /* bit6,  V7R2 为csd业务保留，CCPU发送给ACPU的IPC中断                    */
    IPC_ACPU_INT_SRC_ICC                            = 9,        /* bit9,   v7r2 icc    公用，CCPU发送给ACPU的IPC中断                     */
    IPC_ACPU_INT_SRC_ICC_PRIVATE                    =10,        /* bit10,   v7r2 icc    私用，CCPU发送给ACPU的IPC中断                    */
    IPC_ACPU_SRC_CCPU_DUMP                          =15,        /* bit15, v7r2 可维可测，CCPU 发送给ACPU 的IPC中断                       */
    IPC_INT_DICC_RELDATA_ACPU                       =16,        /* 原为7号，gu 已确认删除，但JB分支还在使用，7号被dsp占用，添加为编译通过*/
    IPC_ACPU_INT_SRC_MCPU                           =17,        /*bit17,   ICC消息, MCU发送给ACPU的  IPC中断                             */
	IPC_ACPU_INT_SRC_MCPU_WDT                       =18,        /*运行trace时停wdt */
    IPC_ACPU_INT_MCU_SRC_DUMP                       = 19,       /* bin19   dump中断MCU发送给ACPU的IPC中断*/
    IPC_ACPU_INT_SRC_CCPU_RESET_IDLE                = 20,       /* bit20，A/C核在C核单独复位期间核间通信(master in idle)，姚国才 */
    IPC_ACPU_INT_SRC_CCPU_RESET_SUCC                = 21,       /* bit21，A/C核在C核单独复位期间核间通信(ccore reset ok)，姚国才 */
    IPC_ACPU_INT_SRC_MCU_FOR_TEST                   =29,        /*BIT29, test, m core to acore                                           */
    IPC_ACPU_INT_SRC_CCPU_TEST_ENABLE               =31,        /*bit31, IPC自动化测试中断使能时使用，后续IPC中断不够使用时考虑删除      */
    IPC_ACPU_INT_SRC_BUTT = 32,

    /* 定义HIFI IPC跨核消息中断源bit位置 */
    IPC_HIFI_INT_SRC_ACPU_MSG                       =0,        /* bit0, 邮箱消息，ACPU发送给HIFI的IPC中断                                */
    IPC_HIFI_INT_SRC_CCPU_MSG                       =1,        /* bit1, 邮箱消息，CCPU发送给HIFI的IPC中断                                */
    IPC_HIFI_INT_SRC_BBE_MSG                        =4,        /* bit4,  邮箱消息，BBE16发送给HIFI的IPC中断                              */
    IPC_HIFI_INT_SRC_MCU_MSG                        =6,        /* bit6,   邮箱消息，MCU发送给HIFI的IPC中断                               */
    IPC_HIFI_INT_SRC_BUTT=32,

   /* 定义DSP IPC跨核消息中断源bit位置 */
   IPC_INT_MSP_DSP_OM_MBX                           = 0,       /*bit0, V7R2使用，邮箱消息，CCPU发送给DSP的IPC中断                        */
   IPC_INT_PS_DSP_PUB_MBX                           = 1,       /*bit1, v7r2使用，  邮箱消息，CCPU发送给DSP的IPC中断                      */
   IPC_INT_PS_DSP_MAC_MBX                           = 2,       /*bit2,v7r2邮箱消息，专用邮箱中断预留, DSP发给CCPU的IPC中断               */
   IPC_INT_HIFI_DSP_MBX                             = 3,
   IPC_DSP_INT_SRC_CCPU_RESET                       = 4,       /*bit4,k3v3邮箱消息，C核单独复位发给BBE16让其进idle的IPC中断              */
   IPC_DSP_INT_SRC_BUTT                             =32,

    /* 定义BBE IPC跨核消息中断源bit位置 */
    IPC_BBE_INT_SRC_HIFI_MSG                        =0,        /* bit0,  邮箱消息，HIFI发送给BBE16的IPC中断                              */
    IPC_BBE_INT_SRC_BUTT=32,

    IPC_INT_BUTTOM=32
}IPC_INT_LEV_E;


typedef enum tagIPC_SEM_ID_E
{
    IPC_SEM_SPI0               =0,
    IPC_SEM_NV                 =1,
    IPC_SEM_DICC               =3,
    IPC_SEM_RFILE_LOG          =4,
    IPC_SEM_EMMC               =5,
    IPC_SEM_GPIO               =6,
    IPC_SEM_CLK                =7,
    IPC_SEM_PMU_HI6451         =8,
    IPC_SEM_PMU_HI6551         =9,
    IPC_SEM_MTCMOS             =10,
    IPC_SEM_MEM                =11,
#if (defined(BSP_CONFIG_HI3630))
    IPC_SEM_IPF_PWCTRL         =12,
#endif
    IPC_SEM_PMU_HI6559         =13,
    IPC_SEM_I2C_SWITHC         =23,
    IPC_SEM_SYNC               =25,    
    IPC_SEM_BBP                =26,
    IPC_SEM1_IPC_TEST          =28,/*IPC自动化测试使用*/
	IPC_SEM2_IPC_TEST          =29,/*IPC自动化测试使用*/
    IPC_SEM_SYSCTRL            =30,
    IPC_SEM_ZSP_HALT           =31,
    IPC_SEM_BUTTOM
}IPC_SEM_ID_E;
#endif

