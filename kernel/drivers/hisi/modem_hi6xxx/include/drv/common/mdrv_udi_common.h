#ifndef __MDRV_UDI_COMMON_H__
#define __MDRV_UDI_COMMON_H__


/*待徐成将icc替换成udi后删除*/
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

int BSP_ICC_Open(unsigned int u32ChanId, ICC_CHAN_ATTR_S *pChanAttr);
#define DRV_ICC_OPEN(u32ChanId, pChanAttr)  BSP_ICC_Open(u32ChanId, pChanAttr)

int BSP_ICC_Read(unsigned int u32ChanId, unsigned char* pData, int s32Size);
#define DRV_ICC_READ(u32ChanId,pData,s32Size)  BSP_ICC_Read(u32ChanId,pData,s32Size)

int BSP_ICC_Write(unsigned int u32ChanId, unsigned char* pData, int s32Size);
#define DRV_ICC_WRITE(u32ChanId,pData,s32Size)  BSP_ICC_Write(u32ChanId,pData,s32Size)


#define BSP_ERR_ICC_BASE                (int)(0x80000000 | 0x10180000)

/*C核发生复位*/
#define BSP_ERR_ICC_CCORE_RESETTING    (BSP_ERR_ICC_BASE + 0x12)

#define ACM_IOCTL_FLOW_CONTROL          0x7F001035
typedef enum tagACM_IOCTL_FLOW_CONTROL_E
{
    ACM_IOCTL_FLOW_CONTROL_DISABLE = 0,      /* resume receiving data from ACM port */
    ACM_IOCTL_FLOW_CONTROL_ENABLE      /* stop receiving data from ACM port */
}ACM_IOCTL_FLOW_CONTROL_E;

#define UDI_INVALID_HANDLE     (-1)

#endif
