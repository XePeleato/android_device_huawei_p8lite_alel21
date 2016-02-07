
#define MERROR_LEVEL    1
#define MWARNING_LEVEL  1
#define MNOTICE_LEVEL   1
#define MINFO_LEVEL     1
#define MDEBUG_LEVEL    0
#define MLOG_TAG        "watchpoint"


#include <linux/slab.h>
#include <linux/dma-mapping.h>
#include <linux/vmalloc.h>
#include "watchpoint_if.h"
#include "watchpoint_common.h"
#include "watchpoint_acore.h"
#include "watchpoint_test.h"
#include "mntn_log.h"

#define WP_TST_MAX_NUM_ADDR         (16)
#define WP_TST_ADDR_MAX_SIZE(msk)    (0x01 << (msk > 2 ? msk : 3))
static unsigned int wp_tst_addr_saved[WP_TST_MAX_NUM_ADDR] = {0};
static unsigned int wp_tst_addr_index = 0;

extern void wp_tst_clear_watchpoint_all(void);
extern int wp_tst_watchpoint(unsigned int rw_type, unsigned int op_type, unsigned int addr_type, 
                unsigned int addr_mask, unsigned int op_offset, unsigned int cb);
extern void msleep(unsigned int msecs);

char *wp_tst_get_mem(unsigned int type, unsigned int osize, unsigned int *psize)
{
    char *tst_mem = NULL;
    static dma_addr_t addr;
    unsigned int tsize = WP_TST_ADDR_MAX_SIZE(osize);

    if (0 == type) {
        tst_mem = kmalloc(tsize, GFP_KERNEL);
        memset(tst_mem, 0, tsize);
    } else if (1 == type) {
        tst_mem = vmalloc(tsize);
        memset(tst_mem, 0, tsize);
    } else if (2 == type) {
        tsize = 4096;
        tst_mem = dma_alloc_coherent(NULL, tsize, &addr, GFP_ATOMIC);
        memset(tst_mem, 0, tsize);
    } else {
    }

    if (psize != NULL) {
        *psize = tsize;
    }

    if (NULL == tst_mem) {
       mlog_e("type=%d, size=0x%x, memory get failed", type, tsize);
       return NULL;
    }

    mlog_i("type=%d, size=0x%x, memory tested is 0x%p", type, tsize, tst_mem);

    return tst_mem;
}

void wp_tst_free_mem(char *mem_to_free, int type)
{
    if (NULL == mem_to_free) {
        return;
    }
    
    if (0 == type) {
        kfree(mem_to_free);
        mem_to_free = NULL;
    } else if (1 == type) {
        vfree(mem_to_free);
        mem_to_free = NULL;
    } else if (2 == type) {
//        dma_free_coherent();
    } else {
    }

}

static int wp_tst_cb_func(unsigned int addr, void* regs, void* user_param)
{
    mlog_w("cb called");

    return 0;
}

static int wp_tst_set_watchpoint(unsigned int addr, unsigned int type, 
                          unsigned int mask, unsigned int cpu, unsigned int cb)
{
    watchpoint_t wp;
    int ret = 0;

    wp.addr_mask_bits = mask;
    wp.point_type = type & 0x0F;
    if (1 == cb) {
        wp.user_param = NULL;
        wp.watchpoint_callback = wp_tst_cb_func;
    } else {
        wp.user_param = NULL;
        wp.watchpoint_callback = NULL;
    }
    wp.addr = addr;
    if ((type & 0xF0) == 0x00) {
        /* 设全CPU */
        ret = wp_set_watchpoints_ex(&wp, ACPUMAX);
    } else {
        /* 设一个CPU */
        ret = wp_set_watchpoints_ex(&wp, cpu);
    }

    if (ret < 0) {
        mlog_e("ret=%d, cpu=0x%x, addr=0x%x, type=0x%x, mask=0x%x", ret, cpu, addr, type, mask);
        return ret;
    }

    mlog_i("cpu=%d, addr=0x%x, type=0x%x, mask=0x%x", cpu, addr, type, mask);

    return 0;
}

unsigned int wp_tst_read_data(unsigned int tst_addr)
{
    unsigned int value = 0;

    mlog_i("tst_addr=0x%x, cpu=0x%x", tst_addr, raw_smp_processor_id());
    value = *(volatile unsigned int*)tst_addr;

    return value;
}

void wp_tst_write_data(unsigned int tst_addr, unsigned int val)
{
    mlog_i("tst_addr=0x%x, val=0x%x, cpu=0x%x", tst_addr, val, raw_smp_processor_id());
    *(volatile unsigned int *)(tst_addr) = val;
}


void wp_tst_clear_watchpoint_all(void)
{
    unsigned int i;
    
    mlog_i("wp_tst_addr_index=%d", wp_tst_addr_index);
    for (i = 0; i < wp_tst_addr_index; i++) {
        mlog_i("i=%d, wp_tst_addr_saved=0x%X", i, wp_tst_addr_saved[i]);
        wp_clear_watchpoints(wp_tst_addr_saved[i], ACPUMAX);
        wp_tst_addr_saved[i] = 0;
    }

    wp_tst_addr_index = 0;
}

/*
测试用函数
rw_type : 
  watchpoint r/w type[bit0-3]
    0 - reserved
    1 - read 
    2 - write
    3 - r/w
  watchpoint all cpu[bit4-8]
    0x00 - all cpu
    0x10 - one cpu
op_type :
  data access type
    0 - clear
    1 - read 
    2 - write
    others - none
addr_type :
    0 - kmalloc
    1 - vmalloc
    2 - dma
addr_mask :
    0       - no mask
    1, 2    - reserved
    3 to 31 - 2^addr_mask, addr range watched
            - for kmalloc, the value cannot exceed 23
op_offset :
    0 to range watched - offset from which to access data
cb :
    0 - no callback
    1 - with callback
*/
int wp_tst_watchpoint(unsigned int rw_type, unsigned int op_type, unsigned int addr_type, 
                unsigned int addr_mask, unsigned int op_offset, unsigned int cb)
{
    int ret;
    unsigned int cpu;
    char *tst_addr = NULL;

    mlog_i("Begin rw_typ=%d, op_typ=%d, addr_typ=%d, addr_msk=0x%x, offs=%d, cb=%d", 
            rw_type, op_type, addr_type, addr_mask, op_offset, cb);

    tst_addr = wp_tst_get_mem(addr_type, addr_mask, NULL);
    if (NULL == tst_addr) {
        mlog_e("get mem(osize=0x%x) failed", addr_mask);
        return -1;
    }
    
    cpu = raw_smp_processor_id();
    ret = wp_tst_set_watchpoint((unsigned int)tst_addr, rw_type, addr_mask, cpu, cb);
    if (ret < 0) {
        wp_tst_free_mem(tst_addr, addr_type);
        mlog_e("set watchpoint fail");
        return -1;
    }

    /* keep addr tested, used to clear the watchpoint */
    if (wp_tst_addr_index < WP_TST_MAX_NUM_ADDR) {
        wp_tst_addr_saved[wp_tst_addr_index] = (unsigned int)tst_addr;
        wp_tst_addr_index++;
    } else {
        mlog_e("wp_tst_addr_saved exceed(%d)", wp_tst_addr_index);
    }

    /* 显示一个CPU 
    show_watchpoint_info(cpu, NULL);
    show_watchpoint_regs(cpu, NULL);*/

    if (1/*read*/ == op_type) {
        wp_tst_read_data((unsigned int)tst_addr + op_offset);

        /* test end, free memory
           memory cannot be freed until watchpoint was cleared
           otherwise, the memory may be allcated to others to use
           and then exception may happend */
//         wp_tst_free_mem(tst_addr, addr_type);

    } else if (2/*write*/ == op_type) {
        wp_tst_write_data((unsigned int)tst_addr + op_offset, 0);
        
        /* test end, free memory
           memory cannot be freed until watchpoint was cleared
           otherwise, the memory may be allcated to others to use
           and then exception may happend */
//         wp_tst_free_mem(tst_addr, addr_type);
    } else if (0/*clear*/ == op_type) {
        if ((rw_type & 0xF0) == 0x00) {
            /* 全CPU */
            wp_clear_watchpoints((unsigned int)tst_addr, ACPUMAX);
        } else {
            /* 一个CPU */
            wp_clear_watchpoints((unsigned int)tst_addr, cpu);
        }

        /* clear addr tested */
        if (wp_tst_addr_index > 0) {
            wp_tst_addr_index--;
            wp_tst_addr_saved[wp_tst_addr_index] = 0;
        }
        
        /* test end, free memory
           memory cannot be freed until watchpoint was cleared after a moment
           otherwise, the memory may be allcated to others to use
           and then exception may happend */
//        wp_tst_free_mem(tst_addr, addr_type);

    } else if (3/*just set*/ == op_type) {
        /* set a watchpoint, do not free memory.
           it's a mem-leak.
           another mem-leak is addr_type 2, pay attention to use it.
        */

    } else {
        /* do nothing */
    }

    /* 显示一个CPU 
    show_watchpoint_info(cpu, NULL);
    show_watchpoint_regs(cpu, NULL);*/

    mlog_i("End, cpu=%d/%d", cpu, raw_smp_processor_id());

    return 0;
}


