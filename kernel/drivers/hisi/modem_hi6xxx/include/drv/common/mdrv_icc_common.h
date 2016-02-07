#ifndef __MDRV_COMMON_ICC_H__
#define __MDRV_COMMON_ICC_H__
#ifdef __cplusplus
extern "C"
{
#endif


#define ICC_MAGIC_VALUE                (0x5A5A5A5A)
#define ICC_CHANNEL_ID_SCPU_OFFSET         (24)
#define ICC_CHANNEL_ID_DCPU_OFFSET         (16)
#define ICC_CHANNEL_ID_CHANNEL_OFFSET      (8)
#define ICC_CHANNEL_ID(src, dest, channel, func)                  \
    ( ((unsigned int)(src) << ICC_CHANNEL_ID_SCPU_OFFSET)         \
    | ((unsigned int)(dst) << ICC_CHANNEL_ID_DCPU_OFFSET)         \
    | ((unsigned int)(channel) << ICC_CHANNEL_ID_CHANNEL_OFFSET)  \
    | ((unsigned int)(func)) )

#define ICC_PADDING_LEN_ALIGNMENT(len, align) ((align - (len & (align-1)) & (align-1))
#define ICC_PADDING_LEN(len) ICC_PADDING_LEN_ALIGNMENT(len, 4)

#if defined(CHIP_BB_HI6210)
enum ICC_CPUID_ENUM
{
    ICC_CPU_MIN         =   IPC_CORE_ACPU,
    ICC_CPUID_ACPU      =   IPC_CORE_ACPU,
    ICC_CPUID_CCPU      =   IPC_CORE_CCPU,
    ICC_CPUID_MCU       =   IPC_CORE_MCU,
    ICC_CPUID_HIFI      =   IPC_CORE_HIFI,
    ICC_CPUID_BBE16     =   IPC_CORE_BBE16,
    ICC_CPUID_MAX
};

#else
enum ICC_CPUID_ENUM
{
    ICC_CPU_MIN         =   IPC_CORE_ACORE,
    ICC_CPUID_ACPU      =   IPC_CORE_ACORE,
    ICC_CPUID_CCPU      =   IPC_CORE_CCORE,
    ICC_CPUID_MCU       =   IPC_CORE_MCORE,
    ICC_CPUID_BBE16     =   IPC_CORE_LDSP,
    ICC_CPUID_HIFI      =   IPC_CORE_HiFi,
    ICC_CPUID_MAX
};
#endif

/* 通道id分配 */
enum ICC_CHN_ID
{
        /* acore 与 ccore 之间的 ICC 物理通道 */
        ICC_CHN_ACORE_CCORE_MIN=0,                 /* add for modem reset : A/C核通道开始标志 */
        ICC_CHN_GUOM0 = ICC_CHN_ACORE_CCORE_MIN,   /* acore与ccore之间的GUOM0的物理通道 */
        ICC_CHN_GUOM4,                             /* acore与ccore之间的GUOM4的物理通道 */
        ICC_CHN_IFC,                               /* acore与ccore之间的IFC的物理通道 */
        ICC_CHN_RFILE,                             /* acore与ccore之间的RFILE的物理通道 */
        ICC_CHN_NV,                                /* acore与ccore之间的NV的物理通道 */
        ICC_CHN_CSHELL,                            /* acore与ccore之间的CSHELL的物理通道 */
        ICC_CHN_ACORE_CCORE_MAX=ICC_CHN_CSHELL,    /* add for modem reset : A/C核通道结束标志 */
        /* acore 与 mcore 之间的 ICC 物理通道, 将来使用 rpmsg 替代 */
        ICC_CHN_ACORE_MCORE,  /* mcore与acore之间的唯一的物理通道 */
        /* acore 与 HIFI 之间的 ICC 物理通道 */
        ICC_CHN_ACORE_HIFI_MSG,

        /* ccore 与 mcore 之间的 ICC 物理通道 */
        ICC_CHN_CCORE_MCORE,               /* mcore与ccore之间的唯一的物理通道 */
        ICC_CHN_CCORE_HIFI_MSG,            /* ccore与hifi之间的唯一的物理通道 */

        /* ccore 与 BBE16 之间的 ICC 物理通道 */
        ICC_CHN_HIFI_BBE16_MSG,           /* hifi 与 bbe16 之间的 icc msg 物理通道 */

        ICC_CHN_ID_MAX
};

#define ICC_CHN_SIZE_GUOM0_ACPU_TO_CCPU         (16*1024)
#define ICC_CHN_SIZE_GUOM0_CCPU_TO_ACPU         (16*1024)
#define ICC_CHN_SIZE_GUOM4_ACPU_TO_CCPU         (128*1024)
#define ICC_CHN_SIZE_GUOM4_CCPU_TO_ACPU         (128*1024)
#define ICC_CHN_SIZE_IFC_ACPU_TO_CCPU           (4*1024)
#define ICC_CHN_SIZE_IFC_CCPU_TO_ACPU           (4*1024)
#define ICC_CHN_SIZE_RFILE_ACPU_TO_CCPU         (4*1024)
#define ICC_CHN_SIZE_RFILE_CCPU_TO_ACPU         (4*1024)
#define ICC_CHN_SIZE_NV_ACPU_TO_CCPU            (4*1024)
#define ICC_CHN_SIZE_NV_CCPU_TO_ACPU            (4*1024)
#define ICC_CHN_SIZE_CSHELL_ACPU_TO_CCPU        (8*1024)
#define ICC_CHN_SIZE_CSHELL_CCPU_TO_ACPU        (8*1024)
#define ICC_CHN_SIZE_MSG_ACPU_TO_MCORE          (512)
#define ICC_CHN_SIZE_MSG_CCPU_TO_MCORE          (512)
#define ICC_CHN_SIZE_MSG_ACPU_TO_HIFI           (6*1024)
#define ICC_CHN_SIZE_MSG_HIFI_TO_ACPU           (6*1024)
#define ICC_CHN_SIZE_MSG_CCPU_TO_HIFI           (6*1024)
#define ICC_CHN_SIZE_MSG_HIFI_TO_CCPU           (6*1024)
#define ICC_CHN_SIZE_MSG_HIFI_TO_BBE16          (6*1024)
#define ICC_CHN_SIZE_MSG_BBE16_TO_HIFI          (6*1024)

struct icc_channel_ctrl
{
    unsigned int magic;     /* channel magic number, for memory corruption protection */
    unsigned int size;      /* channel size */
    unsigned int write;     /* channel writer position in the fifo */
    unsigned int read;      /* channel reader position in the fifo */
} ICC_CHANNEL_CTRL_STRU;

struct icc_data_head
{
    unsigned int channel_id; /* composed with scpu + dcpu + channelID+ funcID */
    unsigned int len;        /* packet data len, does not include data head and padding data */
    unsigned int timestamp;  /* timestamp for sending the packet */
    unsigned int parameter;  /* for debug use */
} ICC_DATA_HEAD_STRU;


/* the following data structure and API is only used by Acore and Ccore */
typedef enum tagICC_IOCTL_CMD_E{
    ICC_IOCTL_SET_WRITE_CB  = 0,
    ICC_IOCTL_SET_READ_CB,
    ICC_IOCTL_SET_EVENT_CB,
    ICC_IOCTL_GET_STATE,
    ICC_IOCTL_SET_BUTT
}ICC_IOCTL_CMD_E;

typedef enum tagICC_CHAN_MODE_E{
    ICC_CHAN_MODE_STREAM    = 0,
    ICC_CHAN_MODE_PACKET,
    ICC_CHAN_MODE_BUTT
}ICC_CHAN_MODE_E;

typedef unsigned int tagUDI_DEVICE_ID_UINT32;
typedef unsigned int (*icc_event_cb)(unsigned int u32ChanID, unsigned int u32Event, void* Param);
typedef unsigned int (*icc_write_cb)(unsigned int u32ChanID);
typedef unsigned int (*icc_read_cb)(unsigned int u32ChanID, int u32Size);

typedef struct tagICC_CHAN_ATTR_S{
    unsigned int             u32FIFOInSize;
    unsigned int             u32FIFOOutSize;
    unsigned int             u32Priority;
    ICC_CHAN_MODE_E     enChanMode;
    unsigned int             u32TimeOut;
    icc_event_cb        event_cb;
    icc_write_cb        write_cb;
    icc_read_cb         read_cb;
}ICC_CHAN_ATTR_S;

int mdrv_icc_open(unsigned int u32ChanId, ICC_CHAN_ATTR_S *pChanAttr);

int mdrv_icc_read(unsigned int u32ChanId, unsigned char* pData, int s32Size);

int mdrv_icc_write(unsigned int u32ChanId, unsigned char* pData, int s32Size);

int BSP_ICC_Open(unsigned int u32ChanId, ICC_CHAN_ATTR_S *pChanAttr);
#define DRV_ICC_OPEN(u32ChanId, pChanAttr)  BSP_ICC_Open(u32ChanId, pChanAttr)

int BSP_ICC_Read(unsigned int u32ChanId, unsigned char* pData, int s32Size);
#define DRV_ICC_READ(u32ChanId,pData,s32Size)  BSP_ICC_Read(u32ChanId,pData,s32Size)

int BSP_ICC_Write(unsigned int u32ChanId, unsigned char* pData, int s32Size);
#define DRV_ICC_WRITE(u32ChanId,pData,s32Size)  BSP_ICC_Write(u32ChanId,pData,s32Size)


#define BSP_ERR_ICC_BASE                (int)(0x80000000 | 0x10180000)

/*C核发生复位*/
#define BSP_ERR_ICC_CCORE_RESETTING    (BSP_ERR_ICC_BASE + 0x12)

#ifdef __cplusplus
}
#endif
#endif
