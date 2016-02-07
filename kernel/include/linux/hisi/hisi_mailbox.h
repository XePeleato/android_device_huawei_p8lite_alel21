/* hisi_mailbox.h */

#ifndef __HISI_MAILBOX_H__
#define __HISI_MAILBOX_H__

#include <linux/workqueue.h>
#include <linux/device.h>
#include <linux/kfifo.h>
#include <linux/notifier.h>
#include <linux/interrupt.h>
#include <linux/completion.h>
#include <linux/spinlock.h>
#include <linux/mutex.h>
#include <linux/time.h>
#include <linux/errno.h>

/* mailbox devices resources */
#include <linux/hisi/hisi_mailbox_dev.h>

#define EMDEVCLEAN 1
#define EMDEVDIRTY  2
#define ETIMEOUT  3

#define RPUNCERTAIN	2
#define RPACCESSIBLE	1
#define RPUNACCESSIBLE	0

#define MDEV_SYNC_SENDING	(1 << 0)
#define MDEV_ASYNC_ENQUEUE	(1 << 1)
#define MDEV_DEACTIVATED	(1 << 2)

#ifdef CONFIG_HISI_MAILBOX_PERFORMANCE_DEBUG
enum {
	PERFORMANCE_DEBUG_OFF = 0,
	PERFORMANCE_DEBUG_ON
};

enum {
	IPC_START = 0,
	IPC_SEND,
	IPC_RECEIVE,
	IPC_BH,
	IPC_COMPLETE,
	IPC_TTS_MAX
};

#define MBOX_DEBUG_ON(mbox)	(mbox->perf_debug = PERFORMANCE_DEBUG_ON)
#define MBOX_DEBUG_OFF(mbox)	(mbox->perf_debug = PERFORMANCE_DEBUG_OFF)
#define MBOX_IS_DEBUG_ON(mbox)	(mbox->perf_debug == PERFORMANCE_DEBUG_ON ? 1 : 0)
#else
#define MBOX_DEBUG_ON(mbox)	do {} while (0)
#define MBOX_DEBUG_OFF(mbox)	do {} while (0)
#define MBOX_IS_DEBUG_ON(mbox)	(0)
#endif

typedef enum {
	MANUAL_ACK = 0,
	AUTO_ACK,
	NO_ACK
} mbox_ack_type_t;

typedef enum {
	TX_MAIL = 0,
	RX_MAIL,
	MAIL_TYPE_MAX,
} mbox_mail_type_t;

typedef enum {
    SEND_MSG = 0,
    MSG_SCHE,
    DEAL_START,
    DEAL_END,
    SEND_ACK,
    ACK_SCHE,
    FREE_CHAN,

    /*最大值修改需同步调整共享内存*/
    TRACK_MAX
}mbox_track_type_t;

struct hisi_mbox_task;
typedef u32 mbox_msg_t;
typedef int mbox_msg_len_t;
typedef void (*mbox_complete_t)(struct hisi_mbox_task *task);
typedef int (*mbox_irq_handler_t)(int irq, void *p);

struct hisi_mbox_task {
	/* alloc by mailbox users */
	mbox_msg_t			*tx_buffer;
	/* alloc by mailbox core, shouldn't be free when a tx task complete by mailbox users */
	mbox_msg_t			*ack_buffer;
	mbox_msg_len_t			tx_buffer_len;
	mbox_msg_len_t			ack_buffer_len;
	int				need_auto_ack;
	const char			*rp_name;
	mbox_complete_t			complete_fn;
	void				*context;
	int				tx_error;

	/* for performance */
#ifdef CONFIG_HISI_MAILBOX_PERFORMANCE_DEBUG
	int				perf_debug;
	struct timespec			tts[IPC_TTS_MAX];
#endif
};

struct hisi_mbox_device {
	const char			*name;
	int				configured;
	struct device			*dev;
	void				*priv;
	struct hisi_mbox_dev_ops	*ops;
	int				cur_task;
	int				cur_irq;
	volatile unsigned int		status;
	spinlock_t			status_lock;
	mbox_irq_handler_t	irq_handler;

	/* tx attributes */
	spinlock_t			fifo_lock;
	struct kfifo			fifo;
	struct mutex			dev_lock;
	struct completion		complete;
	spinlock_t			complete_lock;
	int				completed;
	struct work_struct		tx_work;
	struct hisi_mbox_task		*tx_task;

	/* rx attributes */
	mbox_msg_t			*rx_buffer;
	mbox_msg_t			*ack_buffer;
	struct atomic_notifier_head	notifier;
	struct tasklet_struct		rx_bh;
};

struct hisi_mbox_dev_ops {
	/* get ready */
	int		(*startup)(struct hisi_mbox_device *mdev);
	void		(*shutdown)(struct hisi_mbox_device *mdev);
	int		(*ability)(struct hisi_mbox_device *mdev,
				mbox_mail_type_t mtype, const char *rp_name);

	/* communication */
	mbox_msg_len_t	(*recv)(struct hisi_mbox_device *mdev, mbox_msg_t **msg);
	int		(*send)(struct hisi_mbox_device *mdev,
				const char *rp_name,
				mbox_msg_t *msg,
				mbox_msg_len_t len,
				int ack_mode);
	void		(*ack)(struct hisi_mbox_device *mdev,
				mbox_msg_t *msg,
				mbox_msg_len_t len);
	void		(*refresh)(struct hisi_mbox_device *mdev);

	/* irq */
	int		(*request_irq)(struct hisi_mbox_device *mdev, mbox_irq_handler_t handler, void *p);
	void		(*free_irq)(struct hisi_mbox_device *mdev, void *p);
	void		(*enable_irq)(struct hisi_mbox_device *mdev);
	void		(*disable_irq)(struct hisi_mbox_device *mdev);
	struct hisi_mbox_device *(*irq_to_mdev)(struct hisi_mbox_device *mdev, struct hisi_mbox_device **list, int irq);
	int		(*is_ack)(struct hisi_mbox_device *mdev);
	int		(*is_ipc)(struct hisi_mbox_device *mdev);

	/* mntn */
	void		(*status)(struct hisi_mbox_device *mdev);
	void		(*track)(struct hisi_mbox_device *mdev, unsigned int phase);
};

struct hisi_mbox {
	const char			*name;
	struct hisi_mbox_device		*tx;
	struct hisi_mbox_device		*rx;
	struct notifier_block		*nb;

#ifdef CONFIG_HISI_MAILBOX_PERFORMANCE_DEBUG
	int				perf_debug;
#endif
};

extern void hisi_mbox_task_free(struct hisi_mbox_task **tx_task);
extern struct hisi_mbox_task *hisi_mbox_task_alloc(struct hisi_mbox *mbox,
					const char *rp_name,
					mbox_msg_t *tx_buffer,
					mbox_msg_len_t tx_buffer_len,
					int need_auto_ack,
					mbox_complete_t complete_fn,
					void *context);

/*
 * might sleep function
 * guarantee function called not in atomic context
 */
extern int hisi_mbox_msg_send_sync(struct hisi_mbox *mbox,
				const char *rp_name,
				mbox_msg_t *tx_buffer,
				mbox_msg_len_t tx_buffer_len,
				int need_auto_ack,
				mbox_msg_t *ack_buffer,
				mbox_msg_len_t ack_buffer_len);

/*
 * atomic context function
 */
extern int hisi_mbox_msg_send_async(struct hisi_mbox *mbox, struct hisi_mbox_task *tx_task);

extern struct hisi_mbox *hisi_mbox_get(const char *name, struct notifier_block *nb);
extern void hisi_mbox_put(struct hisi_mbox **mbox);

extern void hisi_mbox_device_activate(struct hisi_mbox_device **mdevs);
extern void hisi_mbox_device_deactivate(struct hisi_mbox_device **mdevs);

extern int hisi_mbox_device_register(struct device *parent, struct hisi_mbox_device **mdevs);
extern int hisi_mbox_device_unregister(void);

#endif /* __HISI_MAILBOX_H__ */
