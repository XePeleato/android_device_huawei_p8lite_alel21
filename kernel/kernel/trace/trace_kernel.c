

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include <linux/init.h>
#include <linux/module.h>
#include <linux/uaccess.h>
#include <linux/fcntl.h>
#include <linux/syscalls.h>
#include <linux/debugfs.h>
#include <linux/slab.h>
#include <asm/memory.h>
#include <trace/trace_kernel.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif



/*****************************************************************************
  2 全局变量定义
*****************************************************************************/
#define MAX_LEN        (128)
#ifdef CONFIG_TRACE_DOT
#define POINTMAX      (30)

static unsigned int *g_perf_para = NULL;
static char tracedot_name[MODULE_ID_MAX+1][10] = {
    {"tp"},
    {"rec"},
    {"cam"},
    {"stf"},
    {"nup"},
    {"vdec"},
    {"venc"},
    {"vpp"},
    {"aply"},
    {"arec"},
    {"apcm"},
    {"adsp"},
    {"aasp"},
    {"gpu"},
    {"sf"},
    {"mhl"},
    {"reserved"},
};

typedef struct it1_para{
    unsigned int gpu_perf_enable;
    unsigned int sf_perf_enable;
    unsigned int mhl_perf_enable;
}it1_para_type;

typedef struct it2_para{
    unsigned int rec_perf_enable;
    unsigned int stf_perf_enable;
    unsigned int nup_perf_enable;
    unsigned int vdec_perf_enable;
    unsigned int venc_perf_enable;
    unsigned int vpp_perf_enable;
}it2_para_type;

typedef struct codec_para{
   unsigned int aply_perf_enable;
   unsigned int arec_perf_enable;
   unsigned int apcm_perf_enable;
   unsigned int adsp_perf_enable;
   unsigned int aasp_perf_enable;
}codec_para_type;

typedef struct drv4_para{
   unsigned int tp_perf_enable;
}drv4_para_type;

typedef struct perf_para{
    drv4_para_type drv4_para;
    it2_para_type it2_para;
    codec_para_type codec_para;
    it1_para_type it1_para;
}perf_para_type;

static int perf_para_mmap(struct file * file, struct vm_area_struct * vma)
{
    unsigned int paddr = 0;
    unsigned int pfn = 0;
    
    if(!g_perf_para)
    {
        printk(KERN_ERR "trace_kernel[E] perf_para_mmap: g_perf_para is NULL !\n");
        return -EINVAL;
    }
    
    paddr = __pa(g_perf_para);
    pfn = __phys_to_pfn(paddr);
    
    printk(KERN_INFO "trace_kernel[I] perf_para_mmap, paddr=0x%x pfn=0x%x\n", paddr, pfn);
    
    return remap_pfn_range(vma, vma->vm_start, pfn, 1, vma->vm_page_prot);
}

static const struct file_operations kernel_trace_fops = {
    .mmap = perf_para_mmap,
};
#endif /* CONFIG_TRACE_DOT */

/*****************************************************************************
  3 函数实现
*****************************************************************************/
/*****************************************************************************
 函 数 名  : trace_begin
 功能描述  : Add log with the "begin format" to trace_marker which will
             be dumped by systrace.
 输入参数  : i
 输出参数  : 无
 返 回 值  : i
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年5月29日
    作    者   : shiwanglai
    修改内容   : 新生成函数

*****************************************************************************/
void trace_begin(int tag, const char* name)
{
    char buf[MAX_LEN];

    snprintf(buf, MAX_LEN, "B|%ld|%s", sys_getpid(), name);
    trace_printk(buf);

}

/*****************************************************************************
 函 数 名  : trace_end
 功能描述  : Add log with the "end format" to trace_marker which will be
             dumped by systrace.
 输入参数  : i
 输出参数  : 无
 返 回 值  : i
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年5月29日
    作    者   : shiwanglai
    修改内容   : 新生成函数

*****************************************************************************/
void trace_end(int tag)
{
    char* buf = "E";

    trace_printk(buf);
}

/*****************************************************************************
 函 数 名  : trace_counter
 功能描述  : Add log with the "counter format" to trace_marker which will
             be dumped by systrace.
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年5月29日
    作    者   : shiwanglai
    修改内容   : 新生成函数

*****************************************************************************/
void trace_counter(int tag, const char*name, int value)
{
    char buf[MAX_LEN];

    snprintf(buf, MAX_LEN, "C|%ld|%s|%d", sys_getpid(), name, value);
    trace_printk(buf);

}


#ifdef CONFIG_TRACE_DOT
/*****************************************************************************
 函 数 名  : trace_dot
 功能描述  : 在trace_counter的基础上加入了打点开关控制.
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年6月18日
    作    者   : 蒋孝伟
    修改内容   : 新生成函数

*****************************************************************************/
void trace_dot(unsigned int id, const char* point, int value)
{
#if defined(_DRV_LLT_) ||defined(MED_UT)  ||defined(PC_UT_TEST_ON)
    return;
#endif

    char buf[MAX_LEN];
    unsigned int pt = 0;

    pt = (unsigned int)simple_strtoul(point, NULL, 10);

    if ((id >= MODULE_ID_MAX) || (pt > POINTMAX)) {
        printk(KERN_ERR "trace_kernel[E] trace_dot id=%d, pt=%d\n", id, pt);
        return;
    }

    if((*(g_perf_para + id) & (1 << pt)) && (*(g_perf_para + id) & (1 << 31)))
	{
        snprintf(buf, MAX_LEN, "%s%u", tracedot_name[id], pt);
        trace_counter(0xff, buf, value);
    }
}

static int __init kernel_trace_init(void)
{
    struct dentry *d;
    struct dentry *perf_para_root;
    int i;

    /*In order to mmap g_perf_para to userspace, the memory size should be at least one page.*/
    g_perf_para = (unsigned int *)get_zeroed_page(GFP_KERNEL);
    if (NULL == g_perf_para)
    {
        printk(KERN_ERR "trace_kernel[E] kernel_trace_init g_perf_para=NULL\n");
        return -ENOMEM;
    }
    d = debugfs_create_dir("perf_para", NULL);
    perf_para_root = d;
    d = debugfs_create_file("perf_para", S_IWUSR|S_IWGRP|S_IRUGO, perf_para_root, NULL, &kernel_trace_fops);

    for (i=0; i<MODULE_ID_MAX; i++)
    {
        debugfs_create_x32(tracedot_name[i], S_IWUSR|S_IWGRP|S_IRUGO, perf_para_root, (unsigned int *)(g_perf_para + i));
    }

    return 0;
}

static void __exit kernel_trace_exit(void)
{
    return;
}

#else /* CONFIG_TRACE_DOT */

void trace_dot(unsigned int id, const char* point, int value)
{
    return;
}

static int __init kernel_trace_init(void)
{
    return 0;
}

static void __exit kernel_trace_exit(void)
{
    return;
}
#endif /* CONFIG_TRACE_DOT */

module_init(kernel_trace_init);
module_exit(kernel_trace_exit);

MODULE_DESCRIPTION("Kernel Trace Driver");
MODULE_LICENSE("GPL");

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

