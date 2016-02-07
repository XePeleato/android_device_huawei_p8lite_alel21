/*
 * rdr header file (RDR: kernel run data recorder.)
 *
 * Copyright (c) 2013 Hisilicon Technologies CO., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#ifndef	__RDR_H__
#define __RDR_H__

#include <linux/types.h>
#include <linux/huawei/rdr_area.h>
#include <linux/huawei/hisi_ringbuffer.h>

/*
 ***************************************************************************
 * rdr public interface
 ***************************************************************************
*/
/* trace interrupt in or exit flag: */
#define INT_IN_FLAG			0xAAAAUL
#define INT_EXIT_FLAG			0xBBBBUL
#define DUMP_INT_UNLOCK_FLAG		0xCCCCUL

#if 0
enum dump_reboot_reason_t {
	RDR_REASON_NORMAL	= 0xff, /*when normal reboot,set this reason*/
	RDR_REASON_UNDEF	= 0xdead0def /* when init rdr system,set this */
};
#endif

enum dump_arm_vec_t {
	DUMP_ARM_VEC_RESET	= 0x0,
	DUMP_ARM_VEC_UNDEF	= 0x4,
	DUMP_ARM_VEC_SWI	= 0x8,
	DUMP_ARM_VEC_PREFETCH	= 0xc,
	DUMP_ARM_VEC_DATA	= 0x10,
	DUMP_ARM_VEC_IRQ	= 0x18,
	DUMP_ARM_VEC_FIQ	= 0x1c,
	DUMP_ARM_VEC_UNKNOW	= 0xff
};

/* press reset key as exception: */
#define RDR_RESET_AS_EXCE 1

/*if ccore except, we save mem to emmc, dont reboot: */
#define CORE_EXCE_NOT_REBOOT 1

/**
 * rdr_modid_e
 * this enum has two depart:
 * high 8 bit: mean the cpu core;
 * low 24 bit: mean the reason of exception
 */
#define HISI_RDR_MOD_CORE_MASK      (0xff000000)
#define HISI_RDR_MOD_EXCE_MASK      (0x00ffffff)

#define RDR_NCP (0x80000000)
#define RDR_MOD_NCP_MASK            (0x0f000000)

enum rdr_except_core_e {
	RDR_MOD_CORE_AP     = 0x01000000,
	RDR_MOD_CORE_CP     = 0x02000000,
	RDR_MOD_CORE_BBE    = 0x03000000,
	RDR_MOD_CORE_LPM3   = 0x04000000,
	RDR_MOD_CORE_IOM3   = 0x05000000,
	RDR_MOD_CORE_HIFI   = 0x06000000,
	RDR_MOD_CORE_TEEOS  = 0x07000000
};

enum rdr_mod_core_e {
	HISI_RDR_MOD_CORE_AP     = RDR_NCP | 0x01000000,
	HISI_RDR_MOD_CORE_CP     = RDR_NCP | 0x02000000,
	HISI_RDR_MOD_CORE_BBE    = RDR_NCP | 0x03000000,
	HISI_RDR_MOD_CORE_LPM3   = RDR_NCP | 0x04000000,
	HISI_RDR_MOD_CORE_IOM3   = RDR_NCP | 0x05000000,
	HISI_RDR_MOD_CORE_HIFI   = RDR_NCP | 0x06000000,
	HISI_RDR_MOD_CORE_TEEOS  = RDR_NCP | 0x07000000
};

enum rdr_except_reason_e {
	HISI_RDR_MOD_EXCE_NORMAL    = 0x00000000,/*initictive call sys_error*/
	HISI_RDR_MOD_EXCE_PANIC     = 0x00000001,/*ARM except(eg:data abort)*/
	HISI_RDR_MOD_EXCE_STACKFLOW = 0x00000002,
	HISI_RDR_MOD_EXCE_WD        = 0x00000003,/*watchdog timeout*/
	HISI_RDR_MOD_EXCE_PMU       = 0x00000004,
	HISI_RDR_MOD_EXCE_REBOOT    = 0x00000005,
	HISI_RDR_MOD_EXCE_PRESS8S   = 0x00000006,
	HISI_RDR_MOD_EXCE_UNDEF     = 0x000000ff
};

enum rdr_modid_e {
	/*ap exec lists*/
	HISI_RDR_MOD_AP_NORMAL =  (HISI_RDR_MOD_CORE_AP),
	HISI_RDR_MOD_AP_PANIC,
	HISI_RDR_MOD_AP_STACKFLOW,
	HISI_RDR_MOD_AP_WD,
	HISI_RDR_MOD_AP_PMU,
	HISI_RDR_MOD_AP_REBOOT,
	HISI_RDR_MOD_AP_PRESS8S, /* press power key 8s*/

	/*cp exec lists*/
	HISI_RDR_MOD_CP_NORMAL = (HISI_RDR_MOD_CORE_CP),
	HISI_RDR_MOD_CP_PANIC,
	HISI_RDR_MOD_CP_STACKFLOW,
	HISI_RDR_MOD_CP_WD,
	HISI_RDR_MOD_CP_PMU,
	HISI_RDR_MOD_CP_REBOOT,
	HISI_RDR_MOD_CP_DO_RESET,
	HISI_RDR_MOD_CP_RILD,
	HISI_RDR_MOD_CP_3RD,

	/*BBE16 exec lists*/
	HISI_RDR_MOD_BBE_NORMAL = (HISI_RDR_MOD_CORE_BBE),
	HISI_RDR_MOD_BBE_PANIC,
	HISI_RDR_MOD_BBE_STACKFLOW,
	HISI_RDR_MOD_BBE_WD,
	HISI_RDR_MOD_BBE_PMU,
	HISI_RDR_MOD_BBE_REBOOT,

	/*LPM3 exec lists*/
	HISI_RDR_MOD_LPM3_NORMAL = (HISI_RDR_MOD_CORE_LPM3),
	HISI_RDR_MOD_LPM3_PANIC,
	HISI_RDR_MOD_LPM3_STACKFLOW,
	HISI_RDR_MOD_LPM3_WD,
	HISI_RDR_MOD_LPM3_PMU,
	HISI_RDR_MOD_LPM3_REBOOT,

	/*IOM3 exec lists*/
	HISI_RDR_MOD_IOM3_NORMAL = (HISI_RDR_MOD_CORE_IOM3),
	HISI_RDR_MOD_IOM3_PANIC,
	HISI_RDR_MOD_IOM3_STACKFLOW,
	HISI_RDR_MOD_IOM3_WD,
	HISI_RDR_MOD_IOM3_PMU,
	HISI_RDR_MOD_IOM3_REBOOT,

	/*HIFI exec lists*/
	HISI_RDR_MOD_HIFI_NORMAL = (HISI_RDR_MOD_CORE_HIFI),
	HISI_RDR_MOD_HIFI_PANIC,
	HISI_RDR_MOD_HIFI_STACKFLOW,
	HISI_RDR_MOD_HIFI_WD,
	HISI_RDR_MOD_HIFI_PMU,
	HISI_RDR_MOD_HIFI_REBOOT,
	HISI_RDR_MOD_HIFI_6402_ERR,

	/*TEEOS exec lists*/
	HISI_RDR_MOD_TEEOS_NORMAL = (HISI_RDR_MOD_CORE_TEEOS),
	HISI_RDR_MOD_TEEOS_PANIC,
	HISI_RDR_MOD_TEEOS_STACKFLOW,
	HISI_RDR_MOD_TEEOS_WD,
	HISI_RDR_MOD_TEEOS_PMU,
	HISI_RDR_MOD_TEEOS_REBOOT,

	HISI_RDR_MOD_UNDEF = 0xffffffff
};

#define RDR_STR_CORE_AP      "AP"
#define RDR_STR_CORE_CP      "CP"
#define RDR_STR_CORE_IOM3    "IOM3"
#define RDR_STR_CORE_LPM3    "LPM3"
#define RDR_STR_CORE_HIFI    "HIFI"
#define RDR_STR_CORE_BBE     "BBE"
#define RDR_STR_CORE_TEEOS   "TEEOS"
#define RDR_STR_CORE_UNKNOWN "UNKNOWN_COREMOD"

#define RDR_STR_TASKNAME_RILD "RILD"
#define RDR_STR_TASKNAME_3RD_CP "3RD_CP"

#define RDR_DUMP_CP_INFO_MAX_LEN 1024
void hisi_system_error(enum rdr_modid_e mod_id,
			u32 arg1, u32 arg2, char *data, u32 length);

#define FILE_MAGIC 0xdead5a5a
#ifdef CONFIG_SMP
#define RDR_SMP_FLAG 1
#else
#define RDR_SMP_FLAG 0
#endif
#define RDR_VERSION ((RDR_SMP_FLAG << 16) | (0x112 << 0)) /* v1.12 2014.5.14 */
#define RDR_PRODUCT PRODUCT_NAME /* "hi3630_udp" */
//#define RDR_PRODUCT "hi3635_fpga"
//#define RDR_PRODUCT_VERSION "K300V300C00B000"
#define RDR_PRODUCT_VERSION PRODUCT_VERSION_STR /* "K300V300C00B000" */
#define RDR_BUILD_DATE __DATE__
#define RDR_BUILD_TIME __TIME__

#define ARM_REGS_NUM 17

enum rdr_reboot_ctx_e {
	DUMP_CTX_TASK	= 0x0,
	DUMP_CTX_INT	= 0x1
};

struct dump_base_info_s {
	u32 reboot_context;
	u32 reboot_task;
	u8  task_name[16];
	u32 reboot_int;

	u32 modid;
	u32 arg1;
	u32 arg2;
	u32 arg3;
	u32 arg3_length;

	u32 vec;
	u32 cpsr;
	u32 regset[ARM_REGS_NUM];

	u64 current_task;
	u32 current_int;

	u32 cpu_usage;
	u32 mem_free;
	u32 axi_dlock_reg[3];  /*0x94 -- AXI deadlock regist0x624,0x628,0x658*/
};

#define RDR_PHY_SIZE (8 * 1024 * 1024 - 32 * 1024)

#define RDR_BUILD_DATE_TIME_LEN 16
#define RDR_TIME_LEN 16
struct rdr_top_head_s {
	u32 magic;
	u32 version;
	u32 area_number;
	u32 reserve;
	u8 product_name[16];/* the pruduct name of rdr data */
	u8 product_version[16];/* the pruduct version of rdr data */
	u8 build_time[RDR_BUILD_DATE_TIME_LEN];
	u32 except_core;
	u32 except_reason;
	union {
		u8 padding1[0x100 - 0x40 - 8];
		struct {
			char datetime[RDR_TIME_LEN];
			char uptime[RDR_TIME_LEN];
		} con;
	} top_resv;
};

struct rdr_area_s {
	u32 offset; /* offset from area, unit is bytes(1 bytes) */
	u32 length; /* unit is bytes */
};

#define RDR_AREA_MAX 8

struct rdr_struct_s {
	struct rdr_top_head_s top_head;
	struct rdr_area_s area_info[RDR_AREA_MAX];
	u8 padding2[0x200 - 0x140];
	u32 area[(RDR_PHY_SIZE -
		sizeof(struct rdr_area_s) * RDR_AREA_MAX -
		sizeof(struct rdr_top_head_s) - (0x200 - 0x140)) / sizeof(u32)];
};

enum RDR_AREA {
	RDR_ACORE = 0,
	RDR_AREA_RESERVE,
	RDR_CCORE,
	RDR_TEEOS,
	RDR_HIFI,
	RDR_LPM3,
	RDR_IOM3
};
enum RDR_AREA_OFFSET {
	RDR_ACORE_OFFSET	= 0x200,
	RDR_AREA_RESERVE_OFFSET = 0x400000,
	RDR_CCORE_OFFSET	= 0x480000,
	RDR_TEEOS_OFFSET	= 0x600000,
	RDR_HIFI_OFFSET		= 0x680000,
	RDR_LPM3_OFFSET		= 0x700000,
	RDR_IOM3_OFFSET		= 0x7f8000
};

#define RDR_ACORE_MAGIC 0xac0eac0e
#define RDR_ACORE_NAME "AP"
#define RDR_ACORE_VERSION "AP_V112"

struct rdr_area_head_s {
	u32 magic;
	u32 field_number;
	u8 name[8];
	u8 version[16]; /* area format version */
};

/* TYPE,reference for parser: */
enum rdr_type_e {
	RDR_U32, /* int    */
	RDR_ARR, /* array  */
	RDR_Q,   /* queue  */
	RDR_STR  /* string */
};

#define RDR_FIELD_TYPE(type) (type << 0) /* use 2 bits */
#define RDR_FIELD_USED       (1 << 2)    /* use 1 bits */

struct rdr_field_s {
	u32 id;
	u32 offset;  /* offset from rdr_a0_struct_s, unit is byte */
	u32 size;    /* unit is bytes. */
	u32 reserve; /* type/registed etc. */
};

struct rdr_a0_struct_s {
	struct rdr_area_head_s head;
	u32 oam_reg[(RDR_AREA_0 - sizeof(struct rdr_area_head_s))
							/ sizeof(u32)];
};

extern struct rdr_struct_s *pbb;

u32 *rdr_core_addr_fun(char *daddr, int core_id);
/*#define rdr_core_addr(daddr, core_id) \
	((u32 *)(daddr) + (daddr)->area_info[(core_id)].offset / sizeof(u32))
*/
#define rdr_core_addr(daddr, core_id) \
	rdr_core_addr_fun((char *)daddr, core_id)

#define RDR_ACORE_ADDR \
	((struct rdr_a0_struct_s *)(rdr_core_addr(pbb, RDR_ACORE)))


/* this 3 interfaces are used for area 0 only: */
#define field_addr(type, field) ( \
	(type *)((u32 *)(RDR_ACORE_ADDR) + \
	(((struct rdr_field_s *)(RDR_ACORE_ADDR->oam_reg)) + field)->offset / \
							sizeof(u32)))

#define rdr_field_size(field) ( \
	(int)((((struct rdr_field_s *) \
	(RDR_ACORE_ADDR->oam_reg)) + field)->size))

#define field_set(type, field, value) ({ \
	*(type *)((u32 *)(RDR_ACORE_ADDR) + \
	(((struct rdr_field_s *)(RDR_ACORE_ADDR->oam_reg)) + field)->offset / \
							sizeof(u32)) = value; })


#define field_get(type, field)  (*field_addr(type, field))

#define RDR_ZIP_FILE_MAGIC 0xBCCBA3BA
#define RDR_ZIP_FNAME_LEN 64
#define RDR_ZIP_MAX_BLOCK 16
struct rdr_zip_head {
	char filename[RDR_ZIP_FNAME_LEN];
	struct rdr_z_s {
		u32 orig_len;
		u32 zip_len;
		u32 off;
	} z[RDR_ZIP_MAX_BLOCK];
};

struct rdr_zip_file {
	u32 zip_file_magic;
	u32 file_number;
	struct rdr_zip_head head;
};

struct rdr_id_name_s {
	u32 id;
	u8 name[4];
};

#define RDR_GLOBAL_INTERNAL_SIZE 256
struct rdr_a1_reserve_s {
	union {
		u8 padding[RDR_AREA_1 - (64 + 64) * 1024];
		struct {
#ifdef WINDOWSVER
			int a;
#endif
		} con;
	} ap;
	union {
		u8 padding[64 * 1024];
		struct {
			u8 rdr_global_internal[RDR_GLOBAL_INTERNAL_SIZE];
			u8 nv[128];
			u8 resetlog[256];
		} content;
	} ap_cp_share;
	union {
		u8 padding[64 * 1024];
		struct {
			u8 rdr_bin_address[16 * sizeof(u32)];/*save 16 address*/
            u8 rdr_cpinfo[RDR_DUMP_CP_INFO_MAX_LEN];/*cp info*/
		} content;
	} cp_reserve;
};

#define RDR_AREA_RESERVE_ADDR \
	((struct rdr_a1_reserve_s *)(rdr_core_addr(pbb, RDR_AREA_RESERVE)))


/*acore field id is used by rdr itself, but some other modules need
  the id number either. So add the tag, used by rdr module itself: */
#define RDR_FID_ACO_TAG 0x00010000

enum rdr_field_id_e {
	RDR_FID_BASE_INFO     = (RDR_MOD_CORE_AP | RDR_FID_ACO_TAG | 0x01),
	RDR_FID_TASK_SWITCH   = (RDR_MOD_CORE_AP | RDR_FID_ACO_TAG | 0x03),
	RDR_FID_INT_SWITCH    = (RDR_MOD_CORE_AP | RDR_FID_ACO_TAG | 0x05),
	RDR_FID_SYSCALL       = (RDR_MOD_CORE_AP | RDR_FID_ACO_TAG | 0x07),
	RDR_FID_REBOOT_TIME   = (RDR_MOD_CORE_AP | RDR_FID_ACO_TAG | 0x09),
	RDR_FID_PRINTK        = (RDR_MOD_CORE_AP | RDR_FID_ACO_TAG | 0x0b),
	RDR_FID_BUDDY_FAIL    = (RDR_MOD_CORE_AP | RDR_FID_ACO_TAG | 0x0d),
	RDR_FID_WARN          = (RDR_MOD_CORE_AP | RDR_FID_ACO_TAG | 0x0f),
	RDR_FID_HUNG_TASK     = (RDR_MOD_CORE_AP | RDR_FID_ACO_TAG | 0x11),
	RDR_FID_ACPI          = (RDR_MOD_CORE_AP | RDR_FID_ACO_TAG | 0x13),
#ifdef CONFIG_CPU_FREQ
	RDR_FID_CPU_ONOFF     = (RDR_MOD_CORE_AP | RDR_FID_ACO_TAG | 0x15),
#endif
	RDR_FID_INT_LIST      = (RDR_MOD_CORE_AP | RDR_FID_ACO_TAG | 0x17),
	RDR_FID_ALLTASK_STACK = (RDR_MOD_CORE_AP | RDR_FID_ACO_TAG | 0x19),
	RDR_FID_EXCTASK_STACK = (RDR_MOD_CORE_AP | RDR_FID_ACO_TAG | 0x1a),
	RDR_FID_ALLTASK_DUMP  = (RDR_MOD_CORE_AP | RDR_FID_ACO_TAG | 0x1b),
	RDR_FID_TASKNAME_TAB  = (RDR_MOD_CORE_AP | RDR_FID_ACO_TAG | 0x1d),
	RDR_FID_PROC_INT_TAB  = (RDR_MOD_CORE_AP | RDR_FID_ACO_TAG | 0x1f),
	RDR_FID_USR_DATA      = (RDR_MOD_CORE_AP | RDR_FID_ACO_TAG | 0x21),
	RDR_FID_USR_LEN       = (RDR_MOD_CORE_AP | RDR_FID_ACO_TAG | 0x23),

	RDR_LOG_BUF_INFO      = (RDR_MOD_CORE_AP | RDR_FID_ACO_TAG | 0x25),
	RDR_LOG_KERNEL_BUF    = (RDR_MOD_CORE_AP | RDR_FID_ACO_TAG | 0x27),
	RDR_LOG_MAIN_BUF      = (RDR_MOD_CORE_AP | RDR_FID_ACO_TAG | 0x29),
	RDR_LOG_EVENTS_BUF    = (RDR_MOD_CORE_AP | RDR_FID_ACO_TAG | 0x2b),
	RDR_LOG_RADIO_BUF     = (RDR_MOD_CORE_AP | RDR_FID_ACO_TAG | 0x2d),
	RDR_LOG_SYSTEM_BUF    = (RDR_MOD_CORE_AP | RDR_FID_ACO_TAG | 0x2f),
	RDR_LOG_EXCEPTION_BUF = (RDR_MOD_CORE_AP | RDR_FID_ACO_TAG | 0x31),
	RDR_LOG_SIMHOTPLUG_BUF = (RDR_MOD_CORE_AP | RDR_FID_ACO_TAG | 0x32)
};

struct rdr_nv_s {
	union {
		u32 uint_value;
		struct {
			u32 switch_def:1;
			u32 rdr:1;
			u32 rdr_loop:1;
			u32 modem_sram:1;
			u32 modem_mem:1;
			u32 modem_share:1;
			u32 hifi:1;
			u32 hifi_ocram:1;
			u32 hifi_tcm:1;
			u32 resetlog:1;
			u32 lpm3_tcm:1;
			u32 socp:1;
			u32 reserved:20;
		} bits;
	} rdr_nv_cfg;
	atomic_t ready;
    atomic_t update;
};

#define RDR_NV_NUM 321
#define RDR_NV_NAME "DUMPCTL"

enum rdr_nv_e {
	RDR_NV_RDR = 4,
	RDR_NV_MODEM_SRAM = 5,
	RDR_NV_MODEM_MEM,
	RDR_NV_MODEM_SHARE,
	RDR_NV_HIFI = 8,
	RDR_NV_HIFI_OCRAM,
	RDR_NV_HIFI_TCM,
	RDR_NV_RESETLOG = 11,
	RDR_NV_LPM3_TCM = 12,
	RDR_NV_SOCP = 13
};

int rdr_f_register_new(u32 *field, enum rdr_field_id_e id, int type, int size);

/**
 * \fn rdr_balong_reg_field This func is wrapperd for modem.
 * \param field_id high 8 bit is core id, low 24 bit is field id.
 * \param len the BYTES that the field request.
 * \return field address
 */
u8 *rdr_balong_reg_field(u32 field_id, u32 len);

/**
 * field: an u32 integer. this is the field id, out param.
 * id   : 4 BYTES, the value is specified by programmer.Top 8 bit is core id.
 * type : the field type
 * size : the BYTES of the field memory used.
 */
#define rdr_afreg(field, id, type, size) \
	rdr_f_register_new((u32 *)&(field), id, type, size)

s32 rdr_dump_register_hook(int field_id, void *func);
void rdr_modem_reset_dumpmem(void);

#define RDR_ASSERT(ret) \
	do { \
		if ((ret) != 0) \
			return -1; \
	} while (0)

#endif	/* End #define __RDR_H__ */
