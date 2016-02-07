/*
 * rdr header file (RDR: kernel run data recorder.)
 *
 * Copyright (c) 2013 Hisilicon Technologies CO., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#ifndef	__RDR_PRIVATE_H__
#define __RDR_PRIVATE_H__

#include <linux/types.h>
#include <asm/traps.h>
#include <linux/huawei/rdr_area.h>
#include <linux/huawei/hisi_ringbuffer.h>



/*
 ***************************************************************************
 * rdr private interface
 ***************************************************************************
*/

#include <linux/spinlock.h>
#include <linux/atomic.h>

extern spinlock_t g_rdr_dump_stack_lock;
extern atomic_t rdr_crit_dump_stack;
s32 rdr_mem_init(void);


extern u32 *g_rdr_dump_stack;
extern int g_rdr_dump_stack_len;
extern int *bsp_dump_log_buf_len;

typedef void (*rdr_funcptr_0)(void);
typedef void (*rdr_funcptr_1)(u64);
typedef void (*rdr_funcptr_2)(unsigned long, unsigned long);
typedef void (*rdr_funcptr_3)(unsigned long, unsigned long, unsigned long);
int  task_switch_hook_del(rdr_funcptr_2 switch_hook);
void int_switch_hook_delete(void);
void exc_hook_delete(void);

int  task_switch_hook_add(rdr_funcptr_2 p_hook_func);
void int_switch_hook_add(rdr_funcptr_3 p_hook_func);
void exc_hook_add(rdr_funcptr_3 p_hook_func);
void dump_stack_bl(struct task_struct *tsk);
int rdr_syslog_print_all(char *buf, int size);
void interrupts_list_hook_add(rdr_funcptr_3 p_hook_func);
asmlinkage void rdr_syscall_hook(int number);
int rdr_show_interrupts(char *buffer, int size);
void suspend_resume_hook_add(rdr_funcptr_2 p_hook_func);
void rdr_hung_task_hook_add(rdr_funcptr_2 p_hook_func);
void rdr_cpu_on_off_hook_add(rdr_funcptr_3 p_hook_func);
void dump_stack_add_hook(rdr_funcptr_1 fn);
int rdr_get_not_dump_stack(void);


#endif	/* End #define __RDR_PRIVATE_H__ */
