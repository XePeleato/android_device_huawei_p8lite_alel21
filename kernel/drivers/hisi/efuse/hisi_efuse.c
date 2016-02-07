/*
  * hisilicon efuse driver, hisi_efuse.c
  *
  * Copyright (c) 2013 Hisilicon Technologies CO., Ltd.
  *
  */
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/err.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/io.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_device.h>
#include <linux/of_platform.h>
#include <linux/platform_device.h>
#include <linux/cpumask.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <asm/delay.h>
#include <asm/memory.h>
#include <linux/types.h>
#include <linux/proc_fs.h>
#include <linux/string.h>
#include <linux/sched.h>
#include <linux/interrupt.h>
#include <linux/smp.h>
#include <linux/hisi/hisi_efuse.h>
#include <linux/dma-mapping.h>
#include <linux/workqueue.h>
#include <linux/timer.h>
#include <linux/delay.h>

#ifndef CONFIG_ATFDRIVER
#include "../mailbox/hi6xxx_mailbox/bsp_mailbox.h"
#endif

#ifdef CONFIG_ATFDRIVER
#ifdef CONFIG_HI6XXX_EFUSE
#include "bsp_drv_ipc.h"
#endif
#include <atfdriver.h>
#endif

#define EFUSE_DEV_NAME "efuse"

static unsigned int g_efuse_group_max;

static unsigned char *efuse_buf;
static dma_addr_t g_phy_addr;
static EFUSE_LOG_LEVEL g_efuse_print_level = log_level_error;
static EFUSEC_DATA g_efusec_data;
static char* g_efusec_attributions[]={
            "efuse_mem_attr_huk",
            "efuse_mem_attr_scp",
            "efuse_mem_attr_authkey",
            "efuse_mem_attr_chipid",
            "efuse_mem_attr_tsensor_calibration",
            "efuse_mem_attr_huk_rd_disable",
            "efuse_mem_attr_authkey_rd_disable",
            "efuse_mem_attr_dbg_class_ctrl",
            "efuse_mem_attr_dieid",
            "efuse_mem_attr_max"
};
#define EFUSE_GROUP_MAX_STRING      "efuse_mem_attr_group_max"
#define GET_GROUP_NUM(width_bit)    (((width_bit) + 31)/32)
#define GET_BYTE_NUM(width_bit)     (GET_GROUP_NUM(width_bit)*4)

#define GET_BITS_WIDTH(width, attr_id) do { \
    width = g_efusec_data.efuse_attrs_parsed_from_dts[(unsigned int)attr_id].bits_width; \
}while(0)

#define efuse_print_info(level, fmt, ...) do {\
    if(level <= g_efuse_print_level){ \
        printk(KERN_ERR fmt, ##__VA_ARGS__); \
    } \
}while(0)

#ifndef CONFIG_ATFDRIVER
/*IFC 核间函数调用*/
IFC_GEN_CALL2(MAILBOX_IFC_ACPU_TO_CCPU_GET_EFUSE_DIEID, get_efuse_dieid_value,
	          IFC_BIVAR, unsigned char *, pu8Buf, 0,
	          IFC_INCNT, unsigned int, uiLength, 0)
IFC_GEN_CALL2(MAILBOX_IFC_ACPU_TO_CCPU_GET_EFUSE_CHIPID, get_efuse_chipid_value,
              IFC_BIVAR, unsigned char *, pu8Buf, 0,
              IFC_INCNT, unsigned int, uiLength, 0)
IFC_GEN_CALL2(MAILBOX_IFC_ACPU_TO_CCPU_SET_EFUSE_CHIPID, set_efuse_chipid_value,
              IFC_INVAR, unsigned char *, pu8Buf, 0,
              IFC_INCNT, unsigned int, uiLength, 0)

IFC_GEN_CALL2(MAILBOX_IFC_ACPU_TO_CCPU_SET_EFUSE_AUTHKEY, set_efuse_authkey_value,
              IFC_INVAR, unsigned char *, pu8Buf, 0,
              IFC_INCNT, unsigned int, uiLength, 0)

IFC_GEN_CALL2(MAILBOX_IFC_ACPU_TO_CCPU_GET_EFUSE_AUTHKEY, get_efuse_authkey_value,
              IFC_BIVAR, unsigned char *, pu8Buf, 0,
              IFC_INCNT, unsigned int, uiLength, 0)

IFC_GEN_CALL1(MAILBOX_IFC_ACPU_TO_CCPU_SET_EFUSE_DEBUGMODE, set_efuse_securitydebug_value,
              IFC_INVAR, unsigned char * , pu8Buf, 0)

IFC_GEN_CALL2(MAILBOX_IFC_ACPU_TO_CCPU_GET_EFUSE_DEBUGMODE, get_efuse_securitydebug_value,
              IFC_BIVAR, unsigned char *, pu8Buf, 0,
              IFC_INCNT, unsigned int, uiLengh, 0)

IFC_GEN_CALL2(MAILBOX_IFC_ACPU_TO_CCPU_GET_THERMAL, get_efuse_thermal_value,
              IFC_BIVAR, unsigned char *, pu8Buf, 0,
			  IFC_INCNT, unsigned int, uiLength, 0)

IFC_GEN_CALL2(MAILBOX_IFC_ACPU_TO_CCPU_GET_CARRIERID, get_efuse_carrierid_value,
              IFC_BIVAR, unsigned char *, pu8Buf, 0,
			  IFC_INCNT, unsigned int, uiLength, 0)


IFC_GEN_CALL2(MAILBOX_IFC_ACPU_TO_CCPU_TEST_EFUSE_DISPLAY,  test_efuse_display,
              IFC_BIVAR, unsigned char *, pu8Buf, 0,
			  IFC_INCNT, unsigned int, uiLength, 0)


#if HISI_EFUSE_DEBUG == 1
IFC_GEN_CALL0(MAILBOX_IFC_ACPU_TO_CCPU_TEST_EFUSE_WR, test_efuse_wr)
#endif

#else
static int (*invoke_efuse_fn)(u64, u64, u64, u64) = NULL;

#define EFUSE_FN_GET_DIEID                                       0xc5000001
#define EFUSE_FN_GET_CHIPID                                      0xc5000002
#define EFUSE_FN_SET_CHIPID                                      0xc5000003
#define EFUSE_FN_GET_AUTHKEY                                     0xc5000004
#define EFUSE_FN_SET_AUTHKEY                                     0xc5000005
#define EFUSE_FN_GET_SECURITYDEBUG                               0xc5000006
#define EFUSE_FN_SET_SECURITYDEBUG                               0xc5000007
#define EFUSE_FN_GET_THERMAL                                     0xc5000008
#define EFUSE_FN_TEST_DISPLAY                                    0xc5000009
#define EFUSE_FN_TEST_WR                                         0xc500000A
#define EFUSE_FN_GET_KCE                                         0xc500000B
#define EFUSE_FN_SET_KCE                                         0xc500000C
#define EFUSE_FN_GET_CARRIERID                                   0xc500000D
#define EFUSE_FN_GET_FREQ                                        0xc500000E


int get_efuse_dieid_value(unsigned char *pu8Buffer, unsigned int u32Length, unsigned int timeout)
{
    int ret;
    if(NULL == pu8Buffer)
    {
        efuse_print_info(log_level_error, "%s: %d: pu8Buffer is NULL.\n", __func__, __LINE__);
       return -EFAULT;
    }

    #ifdef CONFIG_HI6XXX_EFUSE
        BSP_IPC_SpinLock(IPC_SEM_EFUSE);
    #endif
    ret = invoke_efuse_fn(EFUSE_FN_GET_DIEID, (u64)g_phy_addr, u32Length, timeout);
    #ifdef CONFIG_HI6XXX_EFUSE
        BSP_IPC_SpinUnLock(IPC_SEM_EFUSE);
    #endif
    if(OK == ret)
    {
        memmove((void*)pu8Buffer, (void*)efuse_buf, u32Length);
    }
    return ret;
}
int get_efuse_chipid_value(unsigned char *pu8Buffer, unsigned int u32Length, unsigned int timeout)
{
    int ret;
    if(NULL == pu8Buffer)
    {
        efuse_print_info(log_level_error, "%s: %d: pu8Buffer is NULL.\n", __func__, __LINE__);
       return -EFAULT;
    }

    #ifdef CONFIG_HI6XXX_EFUSE
        BSP_IPC_SpinLock(IPC_SEM_EFUSE);
    #endif
    ret = invoke_efuse_fn(EFUSE_FN_GET_CHIPID, (u64)g_phy_addr, u32Length, timeout);
    #ifdef CONFIG_HI6XXX_EFUSE
        BSP_IPC_SpinUnLock(IPC_SEM_EFUSE);
    #endif
    if(OK == ret)
    {
        memmove((void*)pu8Buffer, (void*)efuse_buf, u32Length);
    }
    return ret;
}
int set_efuse_chipid_value(unsigned char *pu8Buffer, unsigned int u32Length, unsigned int timeout)
{
    int ret;
    if(NULL == pu8Buffer)
    {
        efuse_print_info(log_level_error, "%s: %d: pu8Buffer is NULL.\n", __func__, __LINE__);
       return -EFAULT;
    }

    memmove((void*)efuse_buf, (void*)pu8Buffer, u32Length);
    #ifdef CONFIG_HI6XXX_EFUSE
        BSP_IPC_SpinLock(IPC_SEM_EFUSE);
    #endif
    ret = invoke_efuse_fn(EFUSE_FN_SET_CHIPID, (u64)g_phy_addr, u32Length, timeout);
    #ifdef CONFIG_HI6XXX_EFUSE
        BSP_IPC_SpinUnLock(IPC_SEM_EFUSE);
    #endif

    return ret;
}
int get_efuse_authkey_value(unsigned char *pu8Buffer, unsigned int u32Length, unsigned int timeout)
{
    int ret;
    if(NULL == pu8Buffer)
    {
        efuse_print_info(log_level_error, "%s: %d: pu8Buffer is NULL.\n", __func__, __LINE__);
       return -EFAULT;
    }

    #ifdef CONFIG_HI6XXX_EFUSE
        BSP_IPC_SpinLock(IPC_SEM_EFUSE);
    #endif
    ret = invoke_efuse_fn(EFUSE_FN_GET_AUTHKEY, (u64)g_phy_addr, u32Length, timeout);
    #ifdef CONFIG_HI6XXX_EFUSE
        BSP_IPC_SpinUnLock(IPC_SEM_EFUSE);
    #endif
    if(OK == ret)
    {
        memmove((void*)pu8Buffer, (void*)efuse_buf, u32Length);
    }
    return ret;
}
int set_efuse_authkey_value(unsigned char *pu8Buffer, unsigned int u32Length, unsigned int timeout)
{
    int ret;
    if(NULL == pu8Buffer)
    {
        efuse_print_info(log_level_error, "%s: %d: pu8Buffer is NULL.\n", __func__, __LINE__);
       return -EFAULT;
    }

    memmove((void*)efuse_buf, (void*)pu8Buffer, u32Length);
    #ifdef CONFIG_HI6XXX_EFUSE
        BSP_IPC_SpinLock(IPC_SEM_EFUSE);
    #endif
    ret = invoke_efuse_fn(EFUSE_FN_SET_AUTHKEY, (u64)g_phy_addr, u32Length, timeout);
    #ifdef CONFIG_HI6XXX_EFUSE
        BSP_IPC_SpinUnLock(IPC_SEM_EFUSE);
    #endif

    return ret;
}
int get_efuse_securitydebug_value(unsigned char *pu8Buffer, unsigned int u32Length, unsigned int timeout)
{
    int ret;
    if(NULL == pu8Buffer)
    {
        efuse_print_info(log_level_error, "%s: %d: pu8Buffer is NULL.\n", __func__, __LINE__);
       return -EFAULT;
    }

    #ifdef CONFIG_HI6XXX_EFUSE
        BSP_IPC_SpinLock(IPC_SEM_EFUSE);
    #endif
    ret = invoke_efuse_fn(EFUSE_FN_GET_SECURITYDEBUG, (u64)g_phy_addr, u32Length, timeout);
    #ifdef CONFIG_HI6XXX_EFUSE
        BSP_IPC_SpinUnLock(IPC_SEM_EFUSE);
    #endif
    if(OK == ret)
    {
        memmove((void*)pu8Buffer, (void*)efuse_buf, u32Length);
    }
    return ret;
}
int set_efuse_securitydebug_value(unsigned char *pu8Buffer, unsigned int timeout)
{
    int ret;
    if(NULL == pu8Buffer)
    {
        efuse_print_info(log_level_error, "%s: %d: pu8Buffer is NULL.\n", __func__, __LINE__);
       return -EFAULT;
    }

    memmove((void*)efuse_buf, (void*)pu8Buffer, 4);
    #ifdef CONFIG_HI6XXX_EFUSE
        BSP_IPC_SpinLock(IPC_SEM_EFUSE);
    #endif
    ret = invoke_efuse_fn(EFUSE_FN_SET_SECURITYDEBUG, (u64)g_phy_addr, 0, timeout);
    #ifdef CONFIG_HI6XXX_EFUSE
        BSP_IPC_SpinUnLock(IPC_SEM_EFUSE);
    #endif

    return ret;
}
int get_efuse_thermal_value(unsigned char *pu8Buffer, unsigned int u32Length, unsigned int timeout)
{
    int ret;
    if(NULL == pu8Buffer)
    {
        efuse_print_info(log_level_error, "%s: %d: pu8Buffer is NULL.\n", __func__, __LINE__);
       return -EFAULT;
    }

    #ifdef CONFIG_HI6XXX_EFUSE
        BSP_IPC_SpinLock(IPC_SEM_EFUSE);
    #endif
    ret = invoke_efuse_fn(EFUSE_FN_GET_THERMAL, (u64)g_phy_addr, u32Length, timeout);
    #ifdef CONFIG_HI6XXX_EFUSE
        BSP_IPC_SpinUnLock(IPC_SEM_EFUSE);
    #endif
     if(OK == ret)
    {
        memmove((void*)pu8Buffer, (void*)efuse_buf, u32Length);
    }
    return ret;
}

int get_efuse_freq_value(unsigned char *pu8Buffer, unsigned int u32Length)
{
    int ret;
    if(NULL == pu8Buffer)
    {
        efuse_print_info(log_level_error, "%s: %d: pu8Buffer is NULL.\n", __func__, __LINE__);
       return -EFAULT;
    }


    ret = invoke_efuse_fn(EFUSE_FN_GET_FREQ, (u64)g_phy_addr, 4, 0);
    if(OK == ret)
    {
        memmove((void*)pu8Buffer, (void*)efuse_buf, u32Length);
    }
    return ret;
}


static int get_efuse_kce_value(unsigned char *pu8Buffer, unsigned int u32Length, unsigned int timeout)
{
    int ret;
    if(NULL == pu8Buffer)
    {
        efuse_print_info(log_level_error, "%s: %d: pu8Buffer is NULL.\n", __func__, __LINE__);
       return -EFAULT;
    }

    #ifdef CONFIG_HI6XXX_EFUSE
        BSP_IPC_SpinLock(IPC_SEM_EFUSE);
    #endif
    ret = invoke_efuse_fn(EFUSE_FN_GET_KCE, (u64)g_phy_addr, u32Length, timeout);
    #ifdef CONFIG_HI6XXX_EFUSE
        BSP_IPC_SpinUnLock(IPC_SEM_EFUSE);
    #endif
    if(OK == ret)
    {
        memmove((void*)pu8Buffer, (void*)efuse_buf, u32Length);
    }
    return ret;
}


static int set_efuse_kce_value(unsigned char *pu8Buffer, unsigned int u32Length, unsigned int timeout)
{
    int ret;
    if(NULL == pu8Buffer)
    {
        efuse_print_info(log_level_error, "%s: %d: pu8Buffer is NULL.\n", __func__, __LINE__);
       return -EFAULT;
    }

    memmove((void*)efuse_buf, (void*)pu8Buffer, u32Length);
    #ifdef CONFIG_HI6XXX_EFUSE
        BSP_IPC_SpinLock(IPC_SEM_EFUSE);
    #endif
    ret = invoke_efuse_fn(EFUSE_FN_SET_KCE, (u64)g_phy_addr, u32Length, timeout);
    #ifdef CONFIG_HI6XXX_EFUSE
        BSP_IPC_SpinUnLock(IPC_SEM_EFUSE);
    #endif

    return ret;
}

int get_efuse_carrierid_value(unsigned char *pu8Buffer, unsigned int u32Length, unsigned int timeout)
{
    int ret;
    if(NULL == pu8Buffer)
    {
        efuse_print_info(log_level_error, "%s: %d: pu8Buffer is NULL.\n", __func__, __LINE__);
       return -EFAULT;
    }

    #ifdef CONFIG_HI6XXX_EFUSE
        BSP_IPC_SpinLock(IPC_SEM_EFUSE);
    #endif
    ret = invoke_efuse_fn(EFUSE_FN_GET_CARRIERID, (u64)g_phy_addr, u32Length, timeout);
    #ifdef CONFIG_HI6XXX_EFUSE
        BSP_IPC_SpinUnLock(IPC_SEM_EFUSE);
    #endif
    if(OK == ret)
    {
        memmove((void*)pu8Buffer, (void*)efuse_buf, u32Length);
    }
    return ret;
}

#ifdef CONFIG_HI3XXX_EFUSE
int bsp_efuse_read(unsigned int * pBuf, const unsigned int group, const unsigned int num)
{
    int ret;
    int bytes;

    if(NULL == pBuf)
    {
        efuse_print_info(log_level_error, "%s: %d: pBuf is NULL.\n", __func__, __LINE__);
        return -EFAULT;
    }
    switch(group)
    {
        case 57: //CHIPID
            if(2 != num)
            {
                efuse_print_info(log_level_error, "bsp_efuse_read, invalid parameters,group=%d, num=%d.\n", group, num);
                return ERROR;
            }
            bytes = num*4;
            ret = get_efuse_chipid_value(efuse_buf, bytes, 1000);
    		if(OK != ret )
    		{
    			efuse_print_info(log_level_error, "%s: %d: bsp_efuse_read: get_efuse_chipid_value failed.\n", __func__, __LINE__);
    			return ret;
    		}

    		memcpy(pBuf, efuse_buf, bytes);
            break;

        case 32: //DIEID
            if(5 != num)
            {
                efuse_print_info(log_level_error, "bsp_efuse_read, invalid parameters,group=%d, num=%d.\n", group, num);
                return ERROR;
            }
            bytes = num*4;
            ret = get_efuse_dieid_value(efuse_buf, bytes, 1000);
    		if(OK != ret )
    		{
    			efuse_print_info(log_level_error, "%s: %d: bsp_efuse_read: get_efuse_dieid_value failed.\n", __func__, __LINE__);
    			return ret;
    		}

    		memcpy(pBuf, efuse_buf, bytes);
            break;

        case 28: //CARRIER ENCRYPTION KEY
            if(4 != num)
            {
                efuse_print_info(log_level_error, "bsp_efuse_read, invalid parameters,group=%d, num=%d.\n", group, num);
                return ERROR;
            }
            bytes = num*4;
            ret = get_efuse_kce_value(efuse_buf, bytes, 1000);
    		if(OK != ret )
    		{
    			efuse_print_info(log_level_error, "%s: %d: bsp_efuse_read: get_efuse_dieid_value failed.\n", __func__, __LINE__);
    			return ret;
    		}

    		memcpy(pBuf, efuse_buf, bytes);
            break;

        default:
            efuse_print_info(log_level_error, "bsp_efuse_read, invalid group=%d\n", group);
            ret = OK;
            break;
    }
    return ret;
}

int bsp_efuse_write(unsigned int *pBuf, const unsigned int group, const unsigned int num)
{
    int ret;
    int bytes;

    if(NULL == pBuf)
    {
        efuse_print_info(log_level_error, "%s: %d: pBuf is NULL.\n", __func__, __LINE__);
        return -EFAULT;
    }
    switch(group)
    {
        case 28: //CARRIER ENCRYPTION KEY
            if(4 != num)
            {
                efuse_print_info(log_level_error, "bsp_efuse_write, invalid parameters,group=%d, num=%d.\n", group, num);
                return ERROR;
            }
            bytes = num*4;
            memcpy(efuse_buf, pBuf, bytes);
            ret = set_efuse_kce_value(efuse_buf, bytes, 1000);
    		if(OK != ret )
    		{
    			efuse_print_info(log_level_error, "%s: %d: bsp_efuse_write: get_efuse_dieid_value failed.\n", __func__, __LINE__);
    		}

            break;
        default:
            efuse_print_info(log_level_error, "bsp_efuse_write, invalid group=%d\n", group);
            ret = OK;
            break;
    }
    return ret;
}


#endif

int test_efuse_display(unsigned char *pu8Buffer, unsigned int u32Length, unsigned int timeout)
{
    int ret;
    if(NULL == pu8Buffer)
    {
        efuse_print_info(log_level_error, "%s: %d: pu8Buffer is NULL.\n", __func__, __LINE__);
        return -EFAULT;
    }
    efuse_print_info(log_level_error, "test_efuse_display, before spinlock...\n");
    #ifdef CONFIG_HI6XXX_EFUSE
        BSP_IPC_SpinLock(IPC_SEM_EFUSE);
    #endif
     efuse_print_info(log_level_error, "test_efuse_display, after spinlock...\n");

    ret = invoke_efuse_fn(EFUSE_FN_TEST_DISPLAY, (u64)g_phy_addr, u32Length, timeout);
    #ifdef CONFIG_HI6XXX_EFUSE
        BSP_IPC_SpinUnLock(IPC_SEM_EFUSE);
    #endif
    if(OK == ret)
    {
        memmove((void*)pu8Buffer, (void*)efuse_buf, u32Length);
    }
    return ret;
}

#if HISI_EFUSE_DEBUG == 1
int test_efuse_wr(unsigned int timeout)
{
    int ret;
    #ifdef CONFIG_HI6XXX_EFUSE
        BSP_IPC_SpinLock(IPC_SEM_EFUSE);
    #endif
    ret = invoke_efuse_fn(EFUSE_FN_TEST_WR, 0, 0, timeout);
    #ifdef CONFIG_HI6XXX_EFUSE
        BSP_IPC_SpinUnLock(IPC_SEM_EFUSE);
    #endif
    return ret;
}
#endif

#endif
/*
 * Function name:efusec_ioctl.
 * Discription:complement read/write efuse by terms of sending command-words.
 * return value:
 *          @ 0 - success.
 *          @ -1- failure.
 */
static long efusec_ioctl(struct file *file, u_int cmd, u_long arg)
{
	int ret = OK;
	void __user *argp = (void __user *)arg;
	int i = 0;
	unsigned int bits_width, bytes;

#if HISI_EFUSE_DEBUG == 1
    struct cpumask new_mask = {.bits = {0}};
    pid_t curr_pid;

    cpumask_set_cpu(0, &new_mask);
    curr_pid = __task_pid_nr_ns(current, PIDTYPE_PID, NULL);
    if(curr_pid > 0)
    {
       efuse_print_info(log_level_error, "__task_pid_nr_ns success, pid=%d.\n", (int)curr_pid);
       sched_setaffinity(curr_pid, &new_mask);
       msleep(100);
    }
#endif

    efuse_print_info(log_level_error, "%s: %d: cmd=0x%x.\n", __func__, __LINE__, cmd);

	switch (cmd) {
	case HISI_EFUSE_READ_CHIPID:
	    GET_BITS_WIDTH(bits_width, efuse_mem_attr_chipid);
		bytes = GET_BYTE_NUM(bits_width);
		ret = get_efuse_chipid_value(efuse_buf, bytes, 1000);
		if (OK != ret )
		{
			efuse_print_info(log_level_error, "%s: %d: efusec_ioctl: get_efuse_chipid_value failed.\n", __func__, __LINE__);
			break;
		}

		for(i = 0; i < bytes; i++)
		{
			efuse_print_info(log_level_debug, "line%d, get_efuse_chipid_value[%d] = %2x.\n", __LINE__, i, efuse_buf[i]);
		}

		/*send back to user*/
		if (copy_to_user(argp, efuse_buf, bytes))
		{
			ret = -EFAULT;
		}

		break;
	case HISI_EFUSE_WRITE_CHIPID:
		/*get data from user*/
		if(copy_from_user(efuse_buf, argp, sizeof(efuse_buf)))
		{
			ret = -EFAULT;
			break;
		}
		GET_BITS_WIDTH(bits_width, efuse_mem_attr_chipid);
		bytes = GET_BYTE_NUM(bits_width);
		for(i = 0; i < bytes; i++)
		{
			efuse_print_info(log_level_debug, "line%d, set_efuse_chipid_value[%d] = %2x.\n", __LINE__, i, efuse_buf[i]);
		}
		ret = set_efuse_chipid_value(efuse_buf, bytes, 1000);
		if (OK != ret )
		{
			efuse_print_info(log_level_error, "%s: %d: efusec_ioctl: efuse_set_chipID failed.\n", __func__, __LINE__);
		}

		break;
	case HISI_EFUSE_READ_CHIPIDLEN:
        GET_BITS_WIDTH(bits_width, efuse_mem_attr_chipid);

		/*send back to user*/
		if (copy_to_user(argp, &bits_width, sizeof(bits_width)))
		{
			ret = -EFAULT;
		}

		break;
	case HISI_EFUSE_READ_AUTHKEY:
	    GET_BITS_WIDTH(bits_width, efuse_mem_attr_authkey);
		bytes = GET_BYTE_NUM(bits_width);
		ret = get_efuse_authkey_value(efuse_buf, bytes, 1000);
		if (OK != ret)
		{
            efuse_print_info(log_level_error, "%s: %d: efusec_ioctl: get_efuse_authkey_value failed.\n", __func__, __LINE__);
			break;
		}

        for(i = 0; i < bytes; i++)
        {
            efuse_print_info(log_level_debug, "line%d, get_efuse_authkey_value[%d] = %2x.\n", __LINE__, i, efuse_buf[i]);
        }

		/*send back to user*/
        if (copy_to_user(argp, efuse_buf, bytes))
		{
            ret = -EFAULT;
        }

		break;
	case HISI_EFUSE_WRITE_AUTHKEY:
		/*get data from user*/
		if (copy_from_user(efuse_buf, argp, sizeof(efuse_buf))){
			ret = -EFAULT;
			break;
		}
	    GET_BITS_WIDTH(bits_width, efuse_mem_attr_authkey);
		bytes = GET_BYTE_NUM(bits_width);
		for(i = 0; i < bytes; i++)
		{
			efuse_print_info(log_level_debug, "line%d, set_efuse_authkey_value[%d] = %2x.\n", __LINE__, i, efuse_buf[i]);
		}

		ret = set_efuse_authkey_value(efuse_buf, bytes, 1000);
		if (OK != ret)
		{
			efuse_print_info(log_level_error, "%s: %d: efusec_ioctl: set_efuse_authkey_value failed.\n", __func__, __LINE__);
		}
		break;
	case HISI_EFUSE_READ_DEBUGMODE:
	    GET_BITS_WIDTH(bits_width, efuse_mem_attr_huk_rd_disable);
		bytes = GET_BYTE_NUM(bits_width);
		ret = get_efuse_securitydebug_value(efuse_buf, bytes, 1000);
		if (OK != ret)
		{
            efuse_print_info(log_level_error, "%s: %d: efusec_ioctl: efuse_get_SecureDebugMode failed.\n", __func__, __LINE__);
            break;
        }

        efuse_print_info(log_level_info, "line%d: get_efuse_securitydebug_value = %2x.\n", __LINE__, *((int *)efuse_buf));

		/*send back to user*/
		if (copy_to_user(argp, efuse_buf, bytes))
		{
			ret = -EFAULT;
			break;
		}

		break;
	case HISI_EFUSE_WRITE_DEBUGMODE:
	    GET_BITS_WIDTH(bits_width, efuse_mem_attr_huk_rd_disable);
	    bytes = GET_BYTE_NUM(bits_width);
		/*get data from user*/
		if (copy_from_user(efuse_buf, argp, bytes))
		{
			ret = -EFAULT;
			break;
		}

		efuse_print_info(log_level_debug, "line%d: set_efuse_securitydebug_value = %2x.\n", __LINE__, *((int *)efuse_buf));

		ret = set_efuse_securitydebug_value(efuse_buf, 1000);
		if (OK != ret)
		{
			efuse_print_info(log_level_error, "%s: %d: efusec_ioctl: efuse_set_SecureDebugMode failed.\n", __func__, __LINE__);
		}

		break;
    case HISI_EFUSE_READ_DIEID:
	    GET_BITS_WIDTH(bits_width, efuse_mem_attr_dieid);
		bytes = GET_BYTE_NUM(bits_width);
        ret = get_efuse_dieid_value(efuse_buf, bytes, 1000);
        if (OK != ret)
		{
            efuse_print_info(log_level_error, "%s: %d: efusec_ioctl: get_efuse_dieid_value failed.\n", __func__, __LINE__);
            break;
        }

        for(i = 0; i < bytes ; i++)
        {
            efuse_print_info(log_level_debug, "line%d, get_efuse_dieid_vlaue[%d] = %2x.\n", __LINE__, i, efuse_buf[i]);
        }

        /*send back to user*/
        if (copy_to_user(argp, efuse_buf, bytes))
		{
            ret = -EFAULT;
        }

        break;

#if HISI_EFUSE_DEBUG == 1
#ifdef CONFIG_HI3XXX_EFUSE
	case HISI_EFUSE_TEST_READ_CHIPID:
	    GET_BITS_WIDTH(bits_width, efuse_mem_attr_chipid);
		bytes = GET_BYTE_NUM(bits_width);
		ret = bsp_efuse_read(efuse_buf, 57, 2);
		if (OK != ret )
		{
			efuse_print_info(log_level_error, "%s: %d: efusec_ioctl: bsp_efuse_read failed.\n", __func__, __LINE__);
			break;
		}

		for(i = 0; i < bytes; i++)
		{
			efuse_print_info(log_level_debug, "line%d, get_efuse_chipid_value[%d] = %2x.\n", __LINE__, i, efuse_buf[i]);
		}

		/*send back to user*/
		if (copy_to_user(argp, efuse_buf, bytes))
		{
			ret = -EFAULT;
		}

		break;

    case HISI_EFUSE_TEST_READ_DIEID:
	    GET_BITS_WIDTH(bits_width, efuse_mem_attr_dieid);
		bytes = GET_BYTE_NUM(bits_width);
        ret = bsp_efuse_read(efuse_buf, 32, 5);
        if (OK != ret)
		{
            efuse_print_info(log_level_error, "%s: %d: efusec_ioctl: bsp_efuse_read failed.\n", __func__, __LINE__);
            break;
        }

        for(i = 0; i < bytes ; i++)
        {
            efuse_print_info(log_level_debug, "line%d, get_efuse_dieid_vlaue[%d] = %2x.\n", __LINE__, i, efuse_buf[i]);
        }

        /*send back to user*/
        if (copy_to_user(argp, efuse_buf, bytes))
		{
            ret = -EFAULT;
        }

        break;

      case HISI_EFUSE_TEST_READ_KCE:
        ret = bsp_efuse_read(efuse_buf, 28, 4);
        if (OK != ret)
		{
            efuse_print_info(log_level_error, "%s: %d: efusec_ioctl: bsp_efuse_read failed.\n", __func__, __LINE__);
            break;
        }


        /*send back to user*/
        if (copy_to_user(argp, efuse_buf, 16))
		{
            ret = -EFAULT;
        }

        break;

    case HISI_EFUSE_TEST_WRITE_KCE:
        /*get data from user*/
		if (copy_from_user(efuse_buf, argp, 16))
		{
			ret = -EFAULT;
			break;
		}
        ret = bsp_efuse_write(efuse_buf, 28, 4);
        if (OK != ret)
		{
            efuse_print_info(log_level_error, "%s: %d: efusec_ioctl: bsp_efuse_write failed.\n", __func__, __LINE__);
            break;
        }

        break;
#endif
    case HISI_EFUSE_READ_THERMAL:
	    GET_BITS_WIDTH(bits_width, efuse_mem_attr_tsensor_calibration);
		bytes = GET_BYTE_NUM(bits_width);
		ret = get_efuse_thermal_value(efuse_buf, bytes, 1000);
		if (OK != ret)
		{
            efuse_print_info(log_level_error, "%s: %d: efusec_ioctl: get_efuse_thermal_value failed.\n", __func__, __LINE__);
            break;
        }

        efuse_print_info(log_level_info, "line%d: get_efuse_thermal_value = %x.\n", __LINE__, *((int *)efuse_buf));

		/*send back to user*/
		if (copy_to_user(argp, efuse_buf, bytes))
		{
			ret = -EFAULT;
		}

		break;


    case HISI_EFUSE_TEST_WR:
        ret = test_efuse_wr(1000);
        if (OK != ret)
        {
            efuse_print_info(log_level_error, "%s: efusec_ioctl: test_efuse_wr failed.\n", __func__);
        }
        break;
#endif
    case HISI_EFUSE_TEST_DISPLAY:
        bytes = g_efuse_group_max << 2;
        ret = test_efuse_display(efuse_buf, bytes, 1000);
        if (OK != ret)
        {
             efuse_print_info(log_level_error, "%s: efusec_ioctl: test_efuse_display failed, max groups count = %d.\n", __func__, g_efuse_group_max);
             break;
        }
        efuse_print_info(log_level_debug, "print the Efuse IP memory layout, max groups count=%d\n", g_efuse_group_max);
        for(i = 0; i < g_efuse_group_max; i++)
        {
            efuse_print_info(log_level_debug, "%4d: %4x\n", i, *((unsigned int *)(efuse_buf+i*4)));
        }
        /*send back to user*/
        if (copy_to_user(argp, efuse_buf, bytes))
        {
            ret = -EFAULT;
        }
        break;

	default:
	    efuse_print_info(log_level_error, "[EFUSE][%s] Unknow command!\n", __func__);
		ret = -ENOTTY;
		break;
	}

	return ret;
}


#if HISI_EFUSE_DEBUG == 1
int efuse_test_debug(unsigned int test_case)
{
    unsigned int bytes;
    int ret = OK;
	int i;
    if(test_case >= 4)
    {
        efuse_print_info(log_level_error,"[EFUSE][%s] the input test case is invalid, %d!\n", __func__, test_case);
    }
    switch(test_case)
    {
    case 0:
        for(i = 0; i < g_efuse_group_max; i++)
       {
           efuse_print_info(log_level_debug, "%4d: %4x\n", i, *((unsigned int *)(efuse_buf+i*4)));
       }
        bytes = g_efuse_group_max << 2;
        ret = test_efuse_display(efuse_buf, bytes, 1000);
        if (OK != ret)
        {
             efuse_print_info(log_level_error, "%s: efusec_ioctl: test_efuse_display failed, max groups count = %d.\n", __func__, g_efuse_group_max);
             break;
        }
        efuse_print_info(log_level_debug, "print the Efuse IP memory layout, max groups count=%d\n", g_efuse_group_max);
        for(i = 0; i < g_efuse_group_max; i++)
        {
            efuse_print_info(log_level_debug, "%4d: %4x\n", i, *((unsigned int *)(efuse_buf+i*4)));
        }
        break;
    case 1:
        ret = test_efuse_wr(1000);
        if (OK != ret)
        {
             efuse_print_info(log_level_error, "%s: efuse_test_debug: test_efuse_wr failed, ret=%d\n", __func__, ret);
        }
        break;
    case 2:
        ret = get_efuse_carrierid_value(efuse_buf, 2, 1000);
        efuse_print_info(log_level_debug, "print get_efuse_carrierid_value\n");
        for(i = 0; i < 1; i++)
        {
            efuse_print_info(log_level_debug, "%4d: %4x\n", i, *((unsigned int *)(efuse_buf+i*4)));
        }
        break;
    default:
        efuse_print_info(log_level_error, "[EFUSE][%s] Unknow command, %d!\n", __func__, test_case);
        ret = -ENOTTY;
        break;

    }
    return ret;
}

EXPORT_SYMBOL_GPL(efuse_test_debug);

struct delayed_work  efuse_monitor_work;
u64 smc_arg0 = 0;
void hi6xxx_smc_test(void)
{
    printk(KERN_ERR "hi6xxx_smc_test enter.\n");
    efuse_test_debug(smc_arg0);
    return;
}
void hi6xxx_smc_start_work(u64 a0)
{
    smc_arg0 = a0;
    printk(KERN_ERR "schedule smc work.\n");
    schedule_delayed_work_on(0,&efuse_monitor_work, msecs_to_jiffies(1000));
    return;
}

EXPORT_SYMBOL_GPL(hi6xxx_smc_start_work);
#endif


static const struct file_operations efusec_fops = {
	.owner		= THIS_MODULE,
	.unlocked_ioctl = efusec_ioctl,
};

static int __init hisi_efusec_init(void)
{
	int ret = 0;
	int major = 0;
	struct class *efuse_class;
    struct device *pdevice;
	struct device_node *np = NULL;
	unsigned int bit_width;
	unsigned int i;


	memset(&g_efusec_data, 0, sizeof(g_efusec_data));

    /*default consider the Balong case*/
    g_efuse_group_max = 32;
    g_phy_addr = 0;
    efuse_buf = NULL;

    np = of_find_compatible_node(NULL, NULL, "hisilicon, efuse");
    if (!np)
    {
        efuse_print_info(log_level_error, "hisi efuse: No efusec compatible node found.\n");
        return -ENODEV;
    }
    if(OK != (ret = of_property_read_u32(np, EFUSE_GROUP_MAX_STRING, &g_efuse_group_max)))
    {
        if(-EINVAL == ret)
        {
            efuse_print_info(log_level_error, "%s in efusec compatible node does not exist\n", EFUSE_GROUP_MAX_STRING);
        }
        else
        {
            efuse_print_info(log_level_error, "%s in efusec compatible node has invalid value\n", EFUSE_GROUP_MAX_STRING);
        }
        return ret;
    }

    /* currently, parse the dts configurations follow the strategy:
     * 1.Assume the values in correspondings attribution is correct both for Balong and K3V3+ platfrom, so the values in DTS must be filled correctly;
     * 2.Assume to parse the attributions one by one according to g_efusec_attributions[] until one does not exist, treat it as failure, so these attributions
     * contained in g_efusec_attributions[] must be configured in dts;
     * 3.if the value are 0xffffffff, indicate the attribution is reserved for future use, it's process depend on software. A example is the dieid attribution.
     */
    for(i = 0; i < (unsigned int)efuse_mem_attr_max; i++)
    {
        if(OK != (ret = of_property_read_u32(np, g_efusec_attributions[i], &bit_width)))
        {
            if(-EINVAL == ret)
            {
                efuse_print_info(log_level_error, "%d: %s in efusec compatible node does not exist\n", i, g_efusec_attributions[i]);
            }
            else
            {
                efuse_print_info(log_level_error, "%d: %s in efusec compatible node has invalid value\n", i, g_efusec_attributions[i]);
            }
            return ret;
        }
        efuse_print_info(log_level_debug, "%d: %s in efusec compatible node value::%d\n", i, g_efusec_attributions[i], bit_width);
        g_efusec_data.efuse_attrs_parsed_from_dts[i].bits_width = bit_width;
    }

    efuse_print_info(log_level_debug, "g_efuse_group_max=%d, parse the dts configuration success\n", g_efuse_group_max);

	/*创建字符型设备文件节点*/
	major = register_chrdev(0, EFUSE_DEV_NAME, &efusec_fops);
	if (major <= 0)
	{
		ret = -EFAULT;
		efuse_print_info(log_level_error, "hisi efuse: unable to get major for memory devs.\n");
	}

	efuse_class = class_create(THIS_MODULE, EFUSE_DEV_NAME);
	if (IS_ERR(efuse_class))
	{
		ret = -EFAULT;
		efuse_print_info(log_level_error, "hisi efuse: class_create error.\n");
		goto error1;
	}

	pdevice = device_create(efuse_class, NULL, MKDEV(major, 0), NULL, EFUSE_DEV_NAME);
	if (IS_ERR(pdevice))
	{
		ret = -EFAULT;
		efuse_print_info(log_level_error, "hisi efuse: device_create error.\n");
		goto error2;
	}

#ifdef CONFIG_ATFDRIVER
	invoke_efuse_fn = atfd_hisi_fn_smc;
#if HISI_EFUSE_DEBUG == 1
	INIT_DELAYED_WORK(&efuse_monitor_work, hi6xxx_smc_test);
#endif
#endif

/*
 * in 64bit arch, the same memory block between kernel and bl31 exist the cache coherence problem.So in 64bit arch, the memory
 * block allocated by dma_alloc_coherent.*/
#ifndef CONFIG_ATFDRIVER
    efuse_buf = (unsigned char *)kmalloc(g_efuse_group_max << 2, GFP_KERNEL);
#else
    efuse_buf = (unsigned char *)dma_alloc_coherent(pdevice, (g_efuse_group_max << 2), &g_phy_addr, GFP_KERNEL);
#endif
    if(NULL == efuse_buf)
    {
        ret = -EFAULT;
        efuse_print_info(log_level_error, "%s: %d: allocate memory for efuse_buf failed.\n", __func__, __LINE__);
        goto error2;
    }

	efuse_print_info(log_level_debug, "g_phy_addr=0x%lx, efuse init success", (unsigned long)g_phy_addr);

	return ret;
error2:
	class_destroy(efuse_class);
error1:
	unregister_chrdev(major, EFUSE_DEV_NAME);
	return ret;

}


rootfs_initcall(hisi_efusec_init);

MODULE_DESCRIPTION("Hisilicon efuse module");
MODULE_AUTHOR("lvtaolong@huawei.com.sh");
MODULE_LICENSE("GPL");
