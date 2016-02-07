/*
 * rdr header file (RDR: kernel run data recorder.)
 *
 * Copyright (c) 2013 Hisilicon Technologies CO., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#ifndef	__RDR_INTERNAL_H__
#define __RDR_INTERNAL_H__

#include <linux/types.h>
#include <asm/traps.h>
#include <linux/huawei/rdr_area.h>
#include <linux/huawei/hisi_ringbuffer.h>

/*
 ***************************************************************************
 * rdr internel
 ***************************************************************************
*/

#define DUMP_INIT_FLAG_PHASE1	(0x5B5B0000)
#define DUMP_INIT_FLAG_PHASE2	(0x5B5B0001)
#define DUMP_INIT_FLAG		(0x5B5B0002)
#define DUMP_INIT_FLAG_WAIT	(0x5C5C5C5C)

/* rdr file save_flag(state): */
enum dump_save_flag_t {
	DUMP_SAVE_FILE_NORMAL = 0x0,
	DUMP_SAVE_FILE_NEED = 0xAABB,
	DUMP_SAVE_FILE_END = 0xCCDD
};

#define DUMP_SAVE_SUCCESS 0xA4A4A4A4UL

enum dump_start_flag_e {
	DUMP_START_POWER_ON = 0,
	DUMP_START_REBOOT = 0x5A5A1111,
	DUMP_START_EXCH   = 0x5A5A2222,
	DUMP_START_CRASH  = 0x5A5A3333
};

/* rdr internel control state: */
struct dump_core_internal_s {
	u32 init_flag;
	u32 start_flag;
	u32 save_flag;
	u32 ext_save_flag;

	u32 in_interrupt;
	u32 task_switch_start;
	u32 int_switch_start;

	enum dump_save_flag_t save_file_flag;
	u32 start_wait_flag;
};

struct rdr_global_internal_s {
	struct dump_core_internal_s app;
	struct dump_core_internal_s comm;
	struct dump_core_internal_s m3;
	struct dump_core_internal_s hifi3;
	u32 hso_connect_flag;  /* HIDS connect status --by yangzhi */
};

#define DUMP_BUFF_MAX_SIZE   1024
#define ARCH_NAME_MAX 512
#define RDR_FNAME_LEN 128 /*63*/
#define RDR_FILE_MAX_CNT 3

#define OM_PARENTROOT_PATH	"/data/hisi_logs/"
#define OM_ROOT_PATH		"/data/hisi_logs/memorydump/"
#define OM_DUMP_HEAD		"rdr_"
#define RDR_BOOT_DUMP_HEAD	"xrdr_"
#define RDR_RESET_LOG		"/data/hisi_logs/memorydump/reset.log"
#define RDR_RESET_LOG_MAX        2048

#define RDR_DFX_BIN		"dfx_"
#define RDR_MODEM_BIN		"/data/hisi_logs/memorydump/modem.bin"
#define RDR_MODEM_SHARE_BIN	"/data/hisi_logs/memorydump/modem_share.bin"
#define RDR_SOCP_BIN		"/data/hisi_logs/memorydump/debug_socp.bin"
#define RDR_HIFI_BIN		"/data/hisi_logs/memorydump/hifi3.bin"
#define RDR_HIFI_TCM        "/data/hisi_logs/memorydump/hifi3_tcm.bin"
#define RDR_HIFI_OCRAM        "/data/hisi_logs/memorydump/hifi3_ocram.bin"
#define RDR_MODEM_SRAM96K_BIN	"/data/hisi_logs/memorydump/modem_sram.bin"
#define RDR_LPM3_SRAM96K_BIN	"/data/hisi_logs/memorydump/lpm3_sram.bin"

//cp apr path
#define APR_CP_CSHELL_PATH_1 "/data/hwzd_logs/cp_log/modemlog/modemkmsg-log"
#define APR_CP_CSHELL_PATH_2 "/data/hwzd_logs/cp_log/modemlog/modemkmsg-log.1"
#define APR_CP_ETS_PATH_1 "/sdcard/log/modem/viacbp82d/via001.modemlog.trx.bin"
#define APR_CP_ETS_PATH_2 "/sdcard/log/modem/viacbp82d/via002.modemlog.trx.bin"

//cplog dir path 	96
#define BALONGLTE_LOG_DIR_PATH "/sdcard/log/modem/balonglte/"

/* control the trace, whether it is been enabled */
struct dump_nv_s {
	union {
		u32 uint_value;
		struct {
			u32 dump_switch:2;/* 00: exc, 01: usb, 1x: nodev*/
			u32 arm_exc:1; /* 2 arm exception switch */
			u32 stack_flow:1; /* 3 stack overflow detect switch */
			u32 task_switch:1; /* 4 task switch*/
			u32 int_switch:1; /* 5 interrupts switch*/
			u32 suspend_resume:1;/* suspend or resume switch*/
			u32 int_lock:1;
			u32 app_reg_save1:1;
			u32 app_reg_save2:1;
			u32 app_reg_save3:1;
			u32 comm_reg_save1:1;
			u32 comm_reg_save2:1;
			u32 comm_reg_save3:1;
			u32 sys_err_reboot:1; /* 13 system error reset*/
			u32 reset_log:1; /* 14 no use */
			u32 fetal_err:1; /* 15 no use */
			u32 reserved1:15;
		} bits;
	} dump_cfg;
};
extern struct dump_nv_s g_dump_cfg;
extern struct rdr_struct_s *rdr_tmp_pbb;

void dump_task_switch_hook(const void *old_tcb, void *new_tcb);
void dump_int_switch_hook(u32 exit, u32 oldvec, u32 newvec);
extern atomic_t syscall_hook_on;
int rdr_save_file(const char *dir, const char *file_header,
			const void *address, u32 length);
void dump_save(void);

extern struct kobject *rdr_kobj;
s32 rdr_early_init(void);
void dump_config_init(void);
int  sys_add_rdr_node(void);
void sys_del_rdr_node(void);
int  sys_add_rdr_test(void);
void sys_del_rdr_test(void);

int sys_add_rdr_ut(void);

int hisi_rdr_ipc_notify_asp(u32 *msg, unsigned long len);
int hisi_rdr_ipc_notify_iom3(u32 *msg, unsigned long len);
int hisi_rdr_ipc_notify_lpm3(u32 *msg, unsigned long len);
void hisi_rdr_ipc_notify_cp(void);

void hisi_rdr_nmi_notify_iom3(void);
void hisi_rdr_nmi_notify_lpm3(void);
void hisi_rdr_nmi_notify_cp(void);
#ifdef CONFIG_HISI_REBOOT_TYPE
void set_panic_resetflag(void);
#endif
void emergency_restart(void);
int rdr_append_file(char *filename, void *address, u32 length, u32 max_size);

#define rdr_offset(type, member) ((u32)&(((type *)0)->member))

int rdr_remove_file(char *filename);

void rdr_looprw_set_state(int in);

int rdr_loopwrite_open(char *filename, int *fd);
int rdr_loopwrite_append(int fd, void *address, u32 length);
int rdr_loopwrite_close(int fd);

int rdr_loopread_open(char *filename, int *fd);
int rdr_loopread_read(int fd, void *address, u32 length);
int rdr_loopread_close(int fd);
int rdr_wait4partition(char *path, int timeouts);
int wait_for_fs_ready(void);
int rdr_is_cp_modid(u32 mod);
int rdr_is_cp_inap_modid(u32 mod);
int rdr_is_ap_reboot_cp_modid(u32 mod);
int raw_write_8m_to_dfx(void);
int del_old_file(const char *dirname, const char *file_header,
		unsigned int fd, const char *header);
int rdr_open_dir(const char *dirname);
void rdr_file_close(int fd);
int rdr_create_dir(const char *path);

int rdr_set_xtime(struct rdr_struct_s *bb);
int rdr_get_xtime(struct rdr_struct_s *bb, char *out_buf, size_t len);
int rdr_save_8m(const char *dir, const char *name, char *postfix,
			const void *address, u32 length);
void rdr_upload_and_archive(char *path, char *timedir, char *ecore);
void rdr_upload(char *path, char *timedir, char *ecore);
int rdr_compress(void *in, void *out, size_t inlen, size_t outlen);
int rdr_uncompress(void *in, void *out, size_t inlen, size_t outlen);
int rdr_compr_init(void);
void rdr_compr_exit(void);
void get_time_stamp(char *timestamp_buf, unsigned int len);
char *rdr_get_ecore(struct rdr_struct_s *bb);
void rdr_rm_over3_file(char *path);
struct timespec current_kernel_time(void);
int inquiry_rtc_init_ok(void);
#ifdef _HIFI_WD_DEBUG
int reset_hifi_sec(void);
#endif
#ifndef RDR_SIMPLE_DEBUG
/*#define RDR_SIMPLE_DEBUG*/
#endif

#ifdef RDR_SIMPLE_DEBUG
#define rdr_debug(fmt, args ...)	\
	({				\
		printk(KERN_INFO "rdr:%s(%d)%s:" fmt "\n", \
			__func__, __LINE__, __FILE__, ##args); \
	})
#else
#define rdr_debug(fmt, args ...)	\
	({				\
		; \
	})
#endif

#ifdef RDR_DEBUG
#define DUMP_LOG(a) pr_err("[FILE]%s[line:%d] %d\n", __FILE__, __LINE__, a)
#define RDR_PRINT "\033[31mRDR: \033[0m"
#define rdr_plog(format, args ...)		\
	({				\
	pr_info(RDR_PRINT "@%s(%d)%s", __func__, __LINE__, __FILE__);\
	pr_info(format, ##args);	\
	pr_info("\n");		\
	})
#define rdr_spin_lock(lock) \
	do {	\
		pr_info("rdr:%s(), line:%d spin_lock\n", __func__, __LINE__);\
		spin_lock(lock);	\
	} while (0)

#define rdr_spin_unlock(lock) \
	do {	\
		pr_info("rdr:%s(), line:%d spin_unlock\n", __func__, __LINE__);\
		spin_unlock(lock);	\
	} while (0)
#else
#define DUMP_LOG(a)
#define rdr_plog(format, args ...)
#define rdr_spin_lock(lock) spin_lock(lock)
#define rdr_spin_unlock(lock) spin_unlock(lock)
#endif

#endif	/* End #define __RDR_INTERNAL_H__ */
