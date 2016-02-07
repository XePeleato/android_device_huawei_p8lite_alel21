/* hisi_rproc.h */

#ifndef __HISI_RPROC_H__
#define __HISI_RPROC_H__

#include <linux/hisi/hisi_mailbox.h>
#include <linux/errno.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

enum {
	HISI_RPROC_LPM3 = 0,
	HISI_RPROC_LPM3_FOR_REGULATOR,
	HISI_RPROC_LPM3_FOR_DEBUG,
	HISI_RPROC_LPM3_FOR_RDR,
	HISI_RPROC_IOM3,
	HISI_RPROC_HIFI,

	HISI_RPROC_MAX = 0xFF
};
typedef unsigned char rproc_id_t;

#define RPROC_SYNC_SEND(rproc_id, msg, len,				\
		is_sync_msg, ack_buffer, ack_buffer_len)		\
hisi_rproc_xfer_sync(rproc_id, msg, len,				\
		is_sync_msg, ack_buffer, ack_buffer_len)

#define RPROC_ASYNC_SEND(rproc_id, msg, len,				\
		is_sync_msg, complete_fn, data)				\
hisi_rproc_xfer_async(rproc_id, msg, len,				\
		is_sync_msg, complete_fn, data)

#define RPROC_MONITOR_REGISTER(rproc_id, nb)				\
hisi_rproc_rx_register(rproc_id, nb)

#define RPROC_MONITOR_UNREGISTER(rproc_id, nb)			\
hisi_rproc_rx_unregister(rproc_id, nb)

typedef enum {
	ASYNC_MSG = 0,
	SYNC_MSG
} rproc_msg_type_t;

typedef mbox_msg_t rproc_msg_t;
typedef mbox_msg_len_t rproc_msg_len_t;
typedef void (*rproc_complete_t)(rproc_msg_t *ack_buffer,
				rproc_msg_len_t ack_buffer_len,
				int error,
				void *data);

extern int hisi_rproc_xfer_sync(rproc_id_t rproc_id,
				rproc_msg_t *msg,
				rproc_msg_len_t len,
				rproc_msg_type_t is_sync_msg,
				rproc_msg_t *ack_buffer,
				rproc_msg_len_t ack_buffer_len);
extern int hisi_rproc_xfer_async(rproc_id_t rproc_id,
				rproc_msg_t *msg,
				rproc_msg_len_t len,
				rproc_msg_type_t is_sync_msg,
				rproc_complete_t complete_fn,
				void *data);
extern int
hisi_rproc_rx_register(rproc_id_t rproc_id, struct notifier_block *nb);
extern int
hisi_rproc_rx_unregister(rproc_id_t rproc_id, struct notifier_block *nb);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* __HISI_RPROC_H__ */
