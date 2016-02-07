#ifndef _FTM_MNTN_H
#define _FTM_MNTN_H

#ifdef __cplusplus
extern "C"{
#endif

#include "v_lib.h"
#include "v_msg.h"

#define FTM_MNTN_PAYLOAD_LEN            28
#define FTM_DUMP_SIZE_4K                0x1000
#define FTM_DUMP_SIZE_12K               0x3000
#define FTM_PARSE_VERSION               0x0

typedef struct
{
    VOS_UINT32      front;
    VOS_UINT32      rear;
    VOS_UINT32      size;
    VOS_UINT32      reserved;
    VOS_UINT8       data[0];
}ftm_queue_t;

typedef struct ftm_mntn_msg
{
	VOS_UINT32 msgId;
    VOS_UINT32 length;
    VOS_UINT32 stamp;
	VOS_UINT8  payload[FTM_MNTN_PAYLOAD_LEN];
}FTM_Mntn_Msg_Stru;

/* at-ftm layer message */
typedef struct ftm_layer_msg
{
    VOS_UINT32 magic;
    VOS_UINT32 length;
    VOS_UINT32 stamp;
	VOS_UINT32 msgId;
	VOS_UINT8  payload[24];
}FTM_Layer_Msg_Stru;

/* ftm-mb message */
typedef struct ftm_mb_msg
{
    VOS_UINT32          magic;
    VOS_UINT32          length;
    VOS_UINT32			stamp;
    VOS_UINT32          msgId;
	VOS_UINT8			payload[24];

}FTM_MB_Msg_Stru;

VOS_INT32 ftm_mntn_dump_init(VOS_VOID);
VOS_INT32 ftm_mntn_queue_init(VOS_VOID);

VOS_INT32 ftm_queue_t_init(ftm_queue_t *Q, VOS_UINT32 size);
VOS_INT32 ftm_queue_t_in(ftm_queue_t *Q, VOS_VOID *data, VOS_UINT32 length);

VOS_INT32 ftm_mntn_printf(const VOS_CHAR *fmt, ... );

VOS_INT32 ftm_mntn_msg_save(VOS_VOID *dest, VOS_UINT32 ulMsgId, VOS_UINT8 *payload, VOS_UINT32 ulLen);
VOS_INT32 ftm_mntn_mb2ftm_save(VOS_UINT32 ulMsgId,VOS_UINT8 *payload, VOS_UINT32 ulLen);
VOS_INT32 ftm_mntn_ftm2mb_save(VOS_UINT8 *payload, VOS_UINT32 ulLen);
VOS_INT32 ftm_mntn_at2ftm_save(VOS_UINT32 ulMsgId, VOS_UINT8 *payload, VOS_UINT32 ulLen);
VOS_INT32 ftm_mntn_ftm2at_save(VOS_UINT32 ulMsgId, VOS_UINT8 *payload, VOS_UINT32 ulLen);

VOS_VOID  ftm_mntn_datahook(VOS_UINT32 pid1, VOS_UINT32 pid2, VOS_UINT32 length, VOS_UINT8 *data);



#ifdef __cplusplus
}
#endif

#endif

