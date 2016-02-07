#include <linux/hisi/pm/pwrctrl_multi_dfs.h>
#include <linux/hisi/pm/hi6xxx-power-common.h>
#include <linux/hisi/pm/hi6xxx-power-dfs.h>
#include <linux/hisi/pm/hi6xxx-power-mca.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/notifier.h>
#include <linux/pm_qos.h>
/*#include <mali_balong_pmm.h>*/
#include "soc_ao_sctrl_interface.h"
#include "soc_peri_sctrl_interface.h"
#include <linux/hisi/pm/pwrctrl_multi_memcfg.h>
#include <linux/wakelock.h>
#include <asm/io.h>
/*#include "drv_ipc_enum.h"*/
#include "../ipc_hi6xxx/bsp_drv_ipc.h"
#include <linux/of.h>
#include <linux/semaphore.h>
#include <linux/hisi/hi6xxx-iomap.h>
#include <linux/hisi/ipc_msg.h>

/*===tele_mntn===*/
#if defined (CONFIG_HISILICON_PLATFORM_TELE_MNTN)
#if defined (CONFIG_HISILICON_PLATFORM_POWER_CONTROL)
#include <linux/sched.h>
#include <linux/hisi/pm/hi6xxx-power-common.h>
#include <linux/hisi/hi6xxx_tele_mntn.h>
extern unsigned int omTimerGet(void);
#endif
#endif
/*===tele_mntn===*/

/* CPU 当前频率值保存,避免频繁发消息获取 0-3字节:acpu 4-7字节:DDR 8-11字节:GPU*/
/*****************************************************************************
  2 全局变量定义
*****************************************************************************/
typedef struct
{
    unsigned int cur_acpu_freq;
    unsigned int ddr_max_freq_limit;
    unsigned int ddr_min_freq_limit;
}ACPU_DDR_FREQ_LINK_ATTR;

typedef struct ACPU_DDR_FREQLINK_DATA
{
	ACPU_DDR_FREQ_LINK_ATTR acpu_ddr_freqlink_cfg[10];
	unsigned int acpu_ddr_minfreq_req_id;
	unsigned int acpu_ddr_maxfreq_req_id;
	int acpu_not_suspend;
       int acpu_ddr_freq_switch;
       int acpu_ddr_link_min_running_flag;
       int acpu_ddr_link_max_running_flag;
}ACPU_DDR_FREQLINK_DATA_STRU;
local_t struct wake_lock acpu_freq_lock;
/*DVFS功能开关变量*/
ST_PWC_SWITCH_STRU * g_stPwcSwitch = NULL;
local_t s32_t ddr_maxfreq_qos_req_id = DFS_INVALID_ID;
local_t s32_t ddr_minfreq_qos_req_id = DFS_INVALID_ID;
local_t void __iomem *g_dfs_data_addr;

/*default ddr qos*/
struct pm_qos_request ddr_min_req;
struct pm_qos_request ddr_max_req;
int g_adfs_module = 0;
extern int pwrctrl_dfs_qos_update(unsigned int dev_id, int* req_id, unsigned int req_value);
extern ACPU_DDR_FREQLINK_DATA_STRU g_acpu_ddr_freqlink;

/*****************************************************************************
  3 函数实现
*****************************************************************************/

/*****************************************************************************
 函 数 名  : pwrctrl_dfs_cmd_rqqos
 功能描述  : 请求DFS QoS
 输入参数  : qos_id - QoS ID
             req_value - 请求值
 输出参数  : req_id - 请求ID
 返 回 值  : DFS_RET_OK - Success
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年8月15日
    作    者   : 黄星宇 212111
    修改内容   : 新生成函数

*****************************************************************************/
local_t dfs_ret pwrctrl_dfs_cmd_rqqos(s32_t qos_id, u32_t req_value, s32_t* req_id)
{
    union mca_udata_req req;
    union mca_udata_rsp rsp;

    /*printk(KERN_NOTICE "%s qos_id:%d req_value:%d\n", __FUNCTION__, qos_id, req_value);*/
    if(!req_id)
        return RET_ERR_PARAM_NULL;

    req.dfs_rqqos_req.qos_id    = qos_id;
    req.dfs_rqqos_req.req_value = req_value;
    req.dfs_rqqos_req.core_id   = DFS_CORE_ACPU;

    if(mca_send(MCA_CMD_DFS_RQQOS, req, &rsp, PWRCTRL_DFS_CMD_TIMEOUT))
    {
        printk(KERN_NOTICE "%s error!!! mailbox timeout req_id:%d\n", __FUNCTION__, *req_id);
        return RET_ERR_CONNECT_FAIL;
    }

    if(0 == rsp.dfs_rqqos_rsp.ret)
    {
        *req_id = rsp.dfs_rqqos_rsp.req_id;
    }

    /*printk(KERN_NOTICE "%s req_id:%d\n", __FUNCTION__, rsp.dfs_rqqos_rsp.req_id);*/

    return rsp.dfs_rqqos_rsp.ret;
}

/*****************************************************************************

 函 数 名  : pwrctrl_dfs_cmd_rlqos
 功能描述  : 释放DFS QoS
 输入参数  : qos_id - QoS ID
             req_id - 请求ID
 输出参数  : 无
 返 回 值  : DFS_RET_OK - Success
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年8月15日
    作    者   : 黄星宇 212111
    修改内容   : 新生成函数

*****************************************************************************/
local_t dfs_ret pwrctrl_dfs_cmd_rlqos(s32_t qos_id, s32_t* req_id)
{
    union mca_udata_req req;
    union mca_udata_rsp rsp;

    if(!req_id)
        return RET_ERR_PARAM_NULL;

    /*printk(KERN_NOTICE "%s qos_id:%d req_id:%d\n", __FUNCTION__, qos_id, *req_id);*/
    req.dfs_rlqos_req.qos_id  = qos_id;
    req.dfs_rlqos_req.req_id  = *req_id;
    req.dfs_rlqos_req.core_id = DFS_CORE_ACPU;

    if(mca_send(MCA_CMD_DFS_RLQOS, req, &rsp, PWRCTRL_DFS_CMD_TIMEOUT))
    {
        /*w00176398 just workaround*/
        printk(KERN_NOTICE "%s error!!! mailbox timeout req_id:%d\n", __FUNCTION__, *req_id);
        *req_id = DFS_INVALID_ID;
	return RET_ERR_CONNECT_FAIL;
    }

    *req_id = rsp.dfs_rlqos_rsp.req_id;

    /*printk(KERN_NOTICE "%s req_id:%d ret:%d\n", __FUNCTION__, *req_id, rsp.dfs_rlqos_rsp.ret);*/
    return rsp.dfs_rlqos_rsp.ret;
}

/*****************************************************************************
 函 数 名  : pwrctrl_dfs_cmd_udqos
 功能描述  : 更新DFS QoS
 输入参数  : qos_id - QoS ID
             req_id - 请求ID
 输出参数  : 无
 返 回 值  : DFS_RET_OK - Success
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年8月15日
    作    者   : 黄星宇 212111
    修改内容   : 新生成函数

*****************************************************************************/
local_t dfs_ret pwrctrl_dfs_cmd_udqos(s32_t qos_id, s32_t req_id, u32_t req_value)
{
    union mca_udata_req req;
    union mca_udata_rsp rsp;

    /*printk(KERN_NOTICE "%s qos_id:%d req_id:%d req_value:%d\n", __FUNCTION__,
        qos_id, req_id, req_value);*/
    req.dfs_udqos_req.qos_id    = qos_id;
    req.dfs_udqos_req.req_id    = req_id;
    req.dfs_udqos_req.req_value = req_value;
    req.dfs_udqos_req.core_id   = DFS_CORE_ACPU;
    memset((void*)&rsp, 0x0, sizeof(union mca_udata_rsp));
    if(mca_send(MCA_CMD_DFS_UDQOS, req, &rsp, 0))
    {
        printk(KERN_NOTICE "%s error!!! mailbox timeout req_id:%d\n", __FUNCTION__, req_id);
        return RET_ERR_CONNECT_FAIL;
    }
    /*printk(KERN_NOTICE "%s ret:%d\n", __FUNCTION__, rsp.dfs_udqos_rsp.ret);*/
    return rsp.dfs_udqos_rsp.ret;
}

/*****************************************************************************
 函 数 名  : pwrctrl_dfs_cmd_set_policy
 功能描述  : 设置DFS QoS policy
 输入参数  : policy_id - policy ID
             policy_buf - policy buffer
 输出参数  : 无
 返 回 值  : DFS_RET_OK - Success
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年4月26日
    作    者   : 王振伟 176398
    修改内容   : 新生成函数

*****************************************************************************/
#if 0
local_t dfs_ret pwrctrl_dfs_cmd_set_policy(s32_t policy_id, void *policy_buf, unsigned int len)
{
#if 0
    union mca_udata_req req;
    union mca_udata_rsp rsp;

    if(len > POLICY_MAX_TOTAL_SIZE)
    {
        printk(KERN_ERR "%s policy_id:%d len:%d exceed max value(%d) error!!!\n", __FUNCTION__, policy_id, len, POLICY_MAX_TOTAL_SIZE);
        return -1;
    }

    printk(KERN_NOTICE "%s policy_id:%d len:%d\n", __FUNCTION__, policy_id, len);

    req.dfs_setpli_req.policy_id = policy_id;
    memcpy(req.dfs_setpli_req.pli_buf, policy_buf, len);

    if(mca_send(MCA_CMD_DFS_SETPLI, &req, &rsp, PWRCTRL_DFS_CMD_TIMEOUT))
        return RET_ERR_CONNECT_FAIL;

    printk(KERN_NOTICE "%s ret:%d\n", __FUNCTION__, rsp.dfs_setpli_rsp.ret);
    return rsp.dfs_setpli_rsp.ret;
#endif
    unsigned int poli_id = 0;
    unsigned int cur_poli_addr = 0;

    if(NULL == policy_buf)
    {
        printk(KERN_ERR "%s policy buf is null,policy_id:%d len:%d!!!\n", __FUNCTION__, policy_id, len);
        return -1;
    }

    poli_id = *(unsigned int *)policy_buf;

    printk(KERN_INFO"%s policy_id:%d set policy:%d!!!\n", __FUNCTION__, policy_id, poli_id);

    if(DFS_POLICY_ID_DDR == policy_id)
    {
        cur_poli_addr = (unsigned int)ioremap(DDR_POLICY_CUR_POLICY, 0x1000);
    }
    else
    {
        printk(KERN_ERR"pwrctrl_dfs_cmd_set_policy don't support %d\n", policy_id);
        return 0;
    }

    writel(poli_id, cur_poli_addr);

    return 0;
}
#endif


/*****************************************************************************
 函 数 名  : pwrctrl_dfs_cmd_set_profile
 功能描述  : 设置DFS QoS profile
 输入参数  : policy_id - policy ID
             policy_buf - policy buffer
 输出参数  : 无
 返 回 值  : DFS_RET_OK - Success
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年4月26日
    作    者   : 王振伟 176398
    修改内容   : 新生成函数

*****************************************************************************/
#if 0
local_t dfs_ret pwrctrl_dfs_cmd_set_profile(s32_t dev_id, void *profile_buf, unsigned int len)
{
    union mca_udata_req req;
    union mca_udata_rsp rsp;

    if(len > PROFILE_MAX_TOTAL_SIZE)
    {
        printk(KERN_ERR "%s dev_id:%d len:%d exceed max value(%d) error!!!\n", __FUNCTION__, dev_id, len, PROFILE_MAX_TOTAL_SIZE);
        return -1;
    }

    printk(KERN_NOTICE "%s dev_id:%d len:%d\n", __FUNCTION__, dev_id, len);
    req.dfs_set_prof_req.dev_id = dev_id;
    memcpy(req.dfs_set_prof_req.prof_buf, profile_buf, len);

    if(mca_send(MCA_CMD_DFS_SETPROFILE, &req, &rsp, PWRCTRL_DFS_CMD_TIMEOUT))
        return RET_ERR_CONNECT_FAIL;
    printk(KERN_NOTICE "%s ret:%d\n", __FUNCTION__, rsp.dfs_set_prof_rsp.ret);
    return rsp.dfs_set_prof_rsp.ret;
}
#endif

/*****************************************************************************
 函 数 名  : pwrctrl_dfs_cmd_lock
 功能描述  : 锁频DFS QoS
 输入参数  : dev_id - Dev ID
             prof_id - profile ID
 输出参数  : 无
 返 回 值  : DFS_RET_OK - Success
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年4月26日
    作    者   : 王振伟 176398
    修改内容   : 新生成函数

*****************************************************************************/
local_t dfs_ret pwrctrl_dfs_cmd_lock(s32_t dev_id, u32_t prof_id)
{
    union mca_udata_req req;
    union mca_udata_rsp rsp;

    /*printk(KERN_NOTICE "%s dev_id:%d prof_id:%d\n", __FUNCTION__, dev_id, prof_id);*/
    req.dfs_lock_req.dev_id = dev_id;
    req.dfs_lock_req.prof_id = prof_id;

    if(mca_send(MCA_CMD_DFS_LOCK, req, &rsp, PWRCTRL_DFS_CMD_TIMEOUT))
    {
        printk(KERN_NOTICE "%s error!!! mailbox timeout dev_id:%d\n", __FUNCTION__, dev_id);
        return RET_ERR_CONNECT_FAIL;
    }
    /*printk(KERN_NOTICE "%s ret:%d\n", __FUNCTION__, rsp.dfs_lock_rsp.ret);*/
    return rsp.dfs_lock_rsp.ret;
}


/*****************************************************************************
 函 数 名  : pwrctrl_dfs_cmd_unlock
 功能描述  : 解锁频DFS QoS
 输入参数  : dev_id - Dev ID
 输出参数  : 无
 返 回 值  : DFS_RET_OK - Success
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年4月26日
    作    者   : 王振伟 176398
    修改内容   : 新生成函数

*****************************************************************************/
local_t dfs_ret pwrctrl_dfs_cmd_unlock(s32_t dev_id)
{
    union mca_udata_req req;
    union mca_udata_rsp rsp;

    /*printk(KERN_NOTICE "%s dev_id:%d\n", __FUNCTION__, dev_id);*/
    req.dfs_unlock_req.dev_id = dev_id;

    if(mca_send(MCA_CMD_DFS_UNLOCK, req, &rsp, PWRCTRL_DFS_CMD_TIMEOUT))
    {
        printk(KERN_NOTICE "%s error!!! mailbox timeout dev_id:%d\n", __FUNCTION__, dev_id);
        return RET_ERR_CONNECT_FAIL;
    }
    /*printk(KERN_NOTICE "%s ret:%d\n", __FUNCTION__, rsp.dfs_unlock_rsp.ret);*/
    return rsp.dfs_unlock_rsp.ret;
}

/*****************************************************************************
 函 数 名  : pwrctrl_dfs_cmd_current
 功能描述  : 锁频DFS QoS
 输入参数  : dev_id - Dev ID
             prof_id - profile ID
 输出参数  : 无
 返 回 值  : DFS_RET_OK - Success
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年4月26日
    作    者   : 王振伟 176398
    修改内容   : 新生成函数

*****************************************************************************/
#if 0
local_t dfs_ret pwrctrl_dfs_cmd_current(s32_t dev_id, u32_t *prof_id)
{
    union mca_udata_req req = {0};
    union mca_udata_rsp rsp = {0};

    PRINT_PWC_DBG(PWC_SWITCH_DEBPT, "%s dev_id:%d\n", __FUNCTION__, dev_id);
    req.dfs_current_req.dev_id = dev_id;

    if(mca_send(MCA_CMD_DFS_CURRENT, req, &rsp, PWRCTRL_DFS_CMD_TIMEOUT))
        return RET_ERR_CONNECT_FAIL;

    *prof_id = rsp.dfs_current_rsp.prof_id;

    PRINT_PWC_DBG(PWC_SWITCH_DEBPT, "%s prof_id:%d ret:%d\n", __FUNCTION__, *prof_id, rsp.dfs_current_rsp.ret);
    return rsp.dfs_current_rsp.ret;
}
#endif


local_t int dfs_qos_update(s32_t qos_id, s32_t* req_id, u32_t req_value)
{
    if(req_value == PM_QOS_PWRCTRL_DFS_DEFAULT_VALUE) {
        if(*req_id == DFS_INVALID_ID)
            return RET_OK;
        else
            return pwrctrl_dfs_cmd_rlqos(qos_id, req_id);
    } else {
        if(*req_id == DFS_INVALID_ID)
            return pwrctrl_dfs_cmd_rqqos(qos_id, req_value, req_id);
        else
            return pwrctrl_dfs_cmd_udqos(qos_id, *req_id, req_value);
    }
}


/*
 * pwrctrl switch
 */
local_t int set_pwc_switch_value(unsigned int object, unsigned int func)
{
    int ret = 0;

    if(NULL == g_stPwcSwitch)
    {
        return PWC_FUNC_INVALID;
    }

    printk(KERN_NOTICE "%s object:%d func:0x%x status:0x%x\n", __FUNCTION__, object,
        func, *(unsigned int *)g_stPwcSwitch);

    if (MCA_OBJ_CPU == object) {
		if(func & PWC_FUNC_AVS_ENABLE)
		{
            g_stPwcSwitch->aAvs = 1;
		}

		if(func & PWC_FUNC_DFS_ENABLE)
		{
            g_stPwcSwitch->aDfs = 1;
		}

        if(func & PWC_FUNC_DVFS_ENABLE)
		{
            g_stPwcSwitch->aDfs = 1;
            g_stPwcSwitch->aAvs = 1;
		}

		if(func & PWC_FUNC_DVFS_DISABLE)
		{
		    g_stPwcSwitch->aAvs = 0;
            g_stPwcSwitch->aDfs = 0;
		}
	} else if (MCA_OBJ_GPU == object) {
		if(func & PWC_FUNC_AVS_ENABLE)
		{
            g_stPwcSwitch->gAvs = 1;
		}

		if(func & PWC_FUNC_DFS_ENABLE)
		{
            g_stPwcSwitch->gDfs = 1;
		}

        if(func & PWC_FUNC_DVFS_ENABLE)
		{
            g_stPwcSwitch->gDfs = 1;
            g_stPwcSwitch->gAvs = 1;
		}

		if(func & PWC_FUNC_DVFS_DISABLE)
		{
		    g_stPwcSwitch->gAvs = 0;
            g_stPwcSwitch->gDfs = 0;
		}
	} else if (MCA_OBJ_DDR == object) {
		if(func & PWC_FUNC_AVS_ENABLE)
		{
            g_stPwcSwitch->dAvs = 1;
		}

		if(func & PWC_FUNC_DFS_ENABLE)
		{
            g_stPwcSwitch->dDfs = 1;
		}

        if(func & PWC_FUNC_DVFS_ENABLE)
		{
            g_stPwcSwitch->dDfs = 1;
            g_stPwcSwitch->dAvs = 1;
		}

		if(func & PWC_FUNC_DVFS_DISABLE)
		{
		    g_stPwcSwitch->dAvs = 0;
            g_stPwcSwitch->dDfs = 0;
		}
	}
	else if(MCA_OBJ_BUS == object){
        if(func & PWC_FUNC_DFS_ENABLE)
        {
            g_stPwcSwitch->bDfs = 1;
        }
        if(func & PWC_FUNC_DFS_DISABLE)
        {
            g_stPwcSwitch->bDfs = 0;
        }

	}
	else if(MCA_OBJ_BUSLOW == object){
        if(func & PWC_FUNC_DFS_ENABLE)
        {
            g_stPwcSwitch->bLowDfs = 1;
        }
        if(func & PWC_FUNC_DFS_DISABLE)
        {
            g_stPwcSwitch->bLowDfs = 0;
        }

	}

    printk(KERN_NOTICE "%s new status:0x%x\n", __FUNCTION__, *(unsigned int *)g_stPwcSwitch);
	return ret;

}

local_t int get_pwc_switch_value(unsigned int object, unsigned int *func)
{
    int ret = 0;
    unsigned int local_func = 0;

    if(NULL == g_stPwcSwitch)
    {
        return PWC_FUNC_INVALID;
    }

    printk(KERN_NOTICE "%s object:%d status:0x%x\n", __FUNCTION__, object, *(unsigned int*)g_stPwcSwitch);

    if (MCA_OBJ_CPU == object) {
		if(g_stPwcSwitch->aAvs)
		{
            local_func = PWC_FUNC_AVS_ENABLE;
		}

		if(g_stPwcSwitch->aDfs)
		{
            local_func |= PWC_FUNC_DFS_ENABLE;
		}
	} else if (MCA_OBJ_GPU == object) {
		if(g_stPwcSwitch->gAvs)
		{
            local_func = PWC_FUNC_AVS_ENABLE;
		}

		if(g_stPwcSwitch->gDfs)
		{
            local_func |= PWC_FUNC_DFS_ENABLE;
		}
	} else if (MCA_OBJ_DDR == object) {
		if(g_stPwcSwitch->dAvs)
		{
            local_func = PWC_FUNC_AVS_ENABLE;
		}

		if(g_stPwcSwitch->dDfs)
		{
            local_func |= PWC_FUNC_DFS_ENABLE;
		}
	}
	else if(MCA_OBJ_BUS == object){
        if(g_stPwcSwitch->bDfs)
        {
            local_func = PWC_FUNC_DFS_ENABLE;
        }
	}
	else if(MCA_OBJ_BUSLOW == object){
        if(g_stPwcSwitch->bLowDfs)
        {
            local_func = PWC_FUNC_DFS_ENABLE;
        }
	}

    if(0 == local_func)
    {
        local_func = PWC_FUNC_DVFS_DISABLE;
    }

    *func = local_func;

    printk(KERN_NOTICE "%s ret:%d func:0x%x\n", __FUNCTION__, ret, *func);

	return ret;

}

#if 0
local_t void test_get_pwc_value(unsigned int object)
{
    unsigned int func = 0;
    int ret = 0;

    ret = get_pwc_switch_value(object, &func);

    printk("ret:%d func:0x%x\n", ret, func);
}
#endif

/*
 * ddr related interface
 */
int ddr_set_policy_handle(void *policy, unsigned int len)
{
    /*return pwrctrl_dfs_cmd_set_policy(DFS_POLICY_ID_DDR, policy, len);*/
    return -1;
}

int ddr_set_profile_handle(void *profile, unsigned int len)
{
    /*return pwrctrl_dfs_cmd_set_profile(DFS_DEVICE_ID_DDR, profile, len);*/
    /*stub*/
    return -1;
}

int ddr_lockfreq_handle( u32_t prof_id)
{
    if(UNLOCK_PROFILE == prof_id) {
        return pwrctrl_dfs_cmd_unlock(DFS_DEVICE_ID_DDR);
    } else {
        return pwrctrl_dfs_cmd_lock(DFS_DEVICE_ID_DDR,  prof_id);
    }
}

dfs_ret ddr_cur_profile_handle(u32_t *prof_id)
{
   /*return pwrctrl_dfs_cmd_current(DFS_DEVICE_ID_DDR, prof_id);*/
   /**prof_id = *(unsigned int*)(ioremap((MEMORY_AXI_CUR_FREQ_ADDR+4), 0x1000));*/
   return 0;
}

int ddr_get_func_handle(unsigned int *func)
{
    return get_pwc_switch_value(MCA_OBJ_DDR, func);
}

int ddr_set_func_handle(unsigned int func)
{
    return set_pwc_switch_value(MCA_OBJ_DDR, func);
}

int ddr_minfreq_handle(unsigned int req_value)
{
    return dfs_qos_update(DFS_QOS_ID_DDR_MINFREQ, &ddr_minfreq_qos_req_id, req_value);
}

/*local_t int ddr_minfreq_notify(struct notifier_block *b, unsigned long req_value,    void *v)
{
    return dfs_qos_update(DFS_QOS_ID_DDR_MINFREQ, &ddr_minfreq_qos_req_id, req_value);
}*/

int ddr_maxfreq_handle(unsigned int req_value)
{
    return dfs_qos_update(DFS_QOS_ID_DDR_MAXFREQ, &ddr_maxfreq_qos_req_id, req_value);
}

int ddr_safefreq_handle(unsigned int freq)
{
    return dfs_qos_update(DFS_QOS_ID_DDR_MAXFREQ, &ddr_maxfreq_qos_req_id, freq);
}


/*local_t int ddr_maxfreq_notify(struct notifier_block *b, unsigned long req_value,    void *v)
{
    return dfs_qos_update(DFS_QOS_ID_DDR_MAXFREQ, &ddr_maxfreq_qos_req_id, req_value);
}*/

/*local_t struct notifier_block ddr_minfreq_notifier = {
    .notifier_call = ddr_minfreq_notify,
};*/

/*local_t struct notifier_block ddr_maxfreq_notifier = {
    .notifier_call = ddr_maxfreq_notify,
};*/

/*
 * ddr related interface
 */
int buslow_get_func_handle(unsigned int *func)
{
    return get_pwc_switch_value(MCA_OBJ_BUSLOW, func);
}

int bus_get_func_handle(unsigned int *func)
{
    return get_pwc_switch_value(MCA_OBJ_BUS, func);
}

int buslow_set_func_handle(unsigned int func)
{
    return set_pwc_switch_value(MCA_OBJ_BUSLOW, func);
}

int bus_set_func_handle(unsigned int func)
{
    return set_pwc_switch_value(MCA_OBJ_BUS, func);
}

/*
 * SYSNOC调频，ACPU侧实现
 * 如果支持多档调频，需要通过软件调频框架，在MCU侧实现调频流程，A/C核进行投票，通过消息发送至MCU
 * 如果只支持两档，通过硬件投票，A/C核在本地通过配置投票寄存器实现DFS
 */
#ifdef BUS_DFS_MULTI_LEVELS_FREQ
local_t s32_t buslow_minfreq_qos_req_id = DFS_INVALID_ID;
local_t s32_t bus_minfreq_qos_req_id = DFS_INVALID_ID;

int buslow_minfreq_handle(unsigned int req_value)
{
    return dfs_qos_update(DFS_QOS_ID_BUSLOW_MINFREQ, &buslow_minfreq_qos_req_id, req_value);
}

int bus_minfreq_handle(unsigned int req_value)
{
    return dfs_qos_update(DFS_QOS_ID_BUS_MINFREQ, &bus_minfreq_qos_req_id, req_value);
}

#else
int buslow_minfreq_handle(unsigned int req_value)
{
    unsigned int reg_val;
    void __iomem *reg_addr;
    u32 busdfs = 0;
    struct device_node *root;

	root = of_find_compatible_node(NULL,NULL,"hisilicon,hi6210");
	if (!root){
		pr_err("[%s] find root node fail!\n",
			__func__);
		return RET_ERR;
	}

	if (of_property_read_u32(root, "power,busdfs", &busdfs)) {
		pr_err("[%s] node doesn't have this property!\n",
			__func__);
		return RET_ERR;
	}

	if (busdfs == 0){
		return RET_OK;
	}else{
        /*get dfs switch flag*/

        if(DFS_BUSLOW_FREQ_MIN >= req_value)
        {
            reg_addr = (void __iomem *)HISI_VA_ADDRESS(SOC_AO_SCTRL_SC_MCU_VOTE2DIS_ADDR(SOC_AO_SCTRL_BASE_ADDR));
            /*set SC_MCU_VOTE2DIS bit,vote lock,enable bus100M dfs*/
            reg_val = readl(reg_addr);
            writel((reg_val | BIT(BUS_DFS_BIT_ACPU)), reg_addr);
        }
        else
        {
            reg_addr = (void __iomem *)HISI_VA_ADDRESS(SOC_AO_SCTRL_SC_MCU_VOTE2EN_ADDR(SOC_AO_SCTRL_BASE_ADDR));
            /*set SC_MCU_VOTE2EN bit,vote unlock,disable bus100M dfs*/
            reg_val = readl(reg_addr);
            writel((reg_val | BIT(BUS_DFS_BIT_ACPU)), reg_addr);
        }
    }
    return RET_OK;

}

int bus_minfreq_handle(unsigned int req_value)
{
    unsigned int reg_val;
    void __iomem *reg_addr;

    if(DFS_BUS_FREQ_MIN >= req_value)
    {
        reg_addr = (void __iomem *)HISI_VA_ADDRESS(SOC_AO_SCTRL_SC_MCU_VOTE1DIS_ADDR(SOC_AO_SCTRL_BASE_ADDR));
        /*set SC_MCU_VOTE1DIS bit,vote lock,enable bus150M dfs*/
        reg_val = readl(reg_addr);
        writel((reg_val | BIT(BUS_DFS_BIT_ACPU)), reg_addr);
    }
    else
    {
        reg_addr = (void __iomem *)HISI_VA_ADDRESS(SOC_AO_SCTRL_SC_MCU_VOTE1EN_ADDR(SOC_AO_SCTRL_BASE_ADDR));
        /*set SC_MCU_VOTE1EN bit,vote unlock,disable bus150M dfs*/
        reg_val = readl(reg_addr);
        writel((reg_val | BIT(BUS_DFS_BIT_ACPU)), reg_addr);
    }
    return RET_OK;

}
#endif

int buslow_maxfreq_handle(unsigned int req_value)
{
    return 0;
}

int bus_maxfreq_handle(unsigned int req_value)
{
    return 0;
}

local_t int buslow_minfreq_notify(struct notifier_block *b, unsigned long req_value, void *v)
{
    return buslow_minfreq_handle(req_value);
}

local_t int bus_minfreq_notify(struct notifier_block *b, unsigned long req_value, void *v)
{
    return bus_minfreq_handle(req_value);
}

local_t struct notifier_block buslow_minfreq_notifier = {
    .notifier_call = buslow_minfreq_notify,
};

local_t struct notifier_block bus_minfreq_notifier = {
    .notifier_call = bus_minfreq_notify,
};

/*****************************************************************************
 函 数 名  : pwrctrl_acpu_dfs_cmd
 功能描述  : ACPU DFS 请求
 输入参数  : 无
 输出参数  : 无
 返 回 值  : DFS_RET_OK - Success
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年4月1日
    作    者   : 杨诚186965
    修改内容   : 新生成函数

*****************************************************************************/
local_t int pwrctrl_acpu_dfs_cmd(void)
{
    struct ipc_msg msg;
    int ret = 0;

    (msg.cmd_mix).cmd_type = TYPE_FREQ;
    (msg.cmd_mix).cmd = CMD_SETTING;
    (msg.cmd_mix).cmd_obj = OBJ_AP;
    (msg.cmd_mix).cmd_src = OBJ_AP;
    msg.mode = ASYNC_CMD;

    ret = ipc_msg_send(OBJ_LPM3, &msg, SYNC_MODE);
    return ret;
}

/*****************************************************************************
 函 数 名  : pwrctrl_acpu_freq_limit_max
 功能描述  : 温饱ACPU DFS频率限制
 输入参数  : req_value - 限制请求值
             req_flag - 限制标志
 输出参数  : 无
 返 回 值  : DFS_RET_OK - Success
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年6月4日
    作    者   : 杨诚186965
    修改内容   : 新生成函数

*****************************************************************************/
int pwrctrl_acpu_freq_limit_max(unsigned int req_value, unsigned int req_flag)
{
    struct acpufreqinfo *p_freqinfo = (struct acpufreqinfo *)g_dfs_data_addr;

	if( 1!= g_adfs_module)
	{
        return -1;
    }

    p_freqinfo->dfs_temp_freq = req_value;

    if(1 == req_flag)
    {
        p_freqinfo->dfs_flag = ACPU_DFS_LOCK_FLAG;
    }
    else if(0 == req_flag)
    {
        p_freqinfo->dfs_flag = ACPU_DFS_UNLOCK_FLAG;
    }
    else
    {
        return -1;
    }

    return pwrctrl_acpu_dfs_cmd();

}

EXPORT_SYMBOL_GPL(pwrctrl_acpu_freq_limit_max);

/*===tele_mntn===*/
#if defined (CONFIG_HISILICON_PLATFORM_TELE_MNTN)
#if defined (CONFIG_HISILICON_PLATFORM_POWER_CONTROL)
static void tele_mntn_acpu_ddr_freqlink(unsigned int qos_id, unsigned int new_freq)
{
    PWC_TELE_MNTN_ACPU_DFS_DDR_QOS_STRU *qos = NULL;
    PWC_TELE_MNTN_ACPU_DFS_DDR_QOSINFO_STRU * info = NULL;

    if(!g_pPwcAcpuLog)
        return;

    qos = &(g_pPwcAcpuLog->dfsDdr.qos);
    info = &(qos->info);
    info->cmd_id = (short)PM_QOS_UPDATE_REQ;
    info->qos_id = (short)qos_id;
    if(current) {
        info->pid = current->pid;
        if(current->parent)
            info->ppid = current->parent->pid;
    }

    info->tag_val  = new_freq;
    info->new_freq = new_freq;
    qos->qosSliceTime = omTimerGet();
    (void)tele_mntn_write_log(TELE_MNTN_QOS_DDR_ACPU, sizeof(PWC_TELE_MNTN_ACPU_DFS_BOOST_STRU), (void *)qos);
}
#endif
#endif
/*===tele_mntn===*/



/*****************************************************************************
 函 数 名  : pwrctrl_dfs_qos_update
 功能描述  : ACPU qos处理
 输入参数  : dev_id -设备号
             req_value - 请求值
 输出参数  : 无
 返 回 值  : DFS_RET_OK - Success
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年4月1日
    作    者   : 杨诚186965
    修改内容   : 新生成函数

*****************************************************************************/
int pwrctrl_dfs_qos_update(unsigned int dev_id, int* req_id, unsigned int req_value)
{
    struct acpufreqinfo *p_freqinfo = (struct acpufreqinfo *)g_dfs_data_addr;
    int ret=0;
    int i=0;
    if(DEVICE_ID_ACPU == dev_id)
    {
    	p_freqinfo->dfs_acpu_freq = req_value;
    	ret=pwrctrl_acpu_dfs_cmd();
    	/*重新投需要的DDR 票*/
    	if((g_acpu_ddr_freqlink.acpu_not_suspend == 1)&&(g_acpu_ddr_freqlink.acpu_ddr_freq_switch == 1))
        {
        	for(i=0;i<10;i++)
        	{
        		if((g_acpu_ddr_freqlink.acpu_ddr_freqlink_cfg[i].cur_acpu_freq) == req_value)
        			break;
        	}
        	if(i == 10)
        	{
        		return -1;
        	}
                   /*最大DDR 票处理*/
        	if(g_acpu_ddr_freqlink.acpu_ddr_freqlink_cfg[i].ddr_max_freq_limit != 0)
            {
                ret=dfs_qos_update(DFS_QOS_ID_DDR_MAXFREQ,&(g_acpu_ddr_freqlink.acpu_ddr_maxfreq_req_id), g_acpu_ddr_freqlink.acpu_ddr_freqlink_cfg[i].ddr_max_freq_limit);
                g_acpu_ddr_freqlink.acpu_ddr_link_max_running_flag = 1;
/*===tele_mntn===*/
#if defined (CONFIG_HISILICON_PLATFORM_TELE_MNTN)
#if defined (CONFIG_HISILICON_PLATFORM_POWER_CONTROL)
                tele_mntn_acpu_ddr_freqlink(PM_QOS_MEMORY_THROUGHPUT_UP_THRESHOLD, g_acpu_ddr_freqlink.acpu_ddr_freqlink_cfg[i].ddr_max_freq_limit);
#endif
#endif
/*===tele_mntn===*/
            }
            else
            {
                if(g_acpu_ddr_freqlink.acpu_ddr_link_max_running_flag == 1)
                {
                    ret=pwrctrl_dfs_qos_update(DEVICE_ID_DDR_MAX,&(g_acpu_ddr_freqlink.acpu_ddr_maxfreq_req_id),ACPU_SUSPEND_RLQOS_DDR_MAX_VALUE);
                    g_acpu_ddr_freqlink.acpu_ddr_link_max_running_flag = 0;
/*===tele_mntn===*/
#if defined (CONFIG_HISILICON_PLATFORM_TELE_MNTN)
#if defined (CONFIG_HISILICON_PLATFORM_POWER_CONTROL)
                    tele_mntn_acpu_ddr_freqlink(PM_QOS_MEMORY_THROUGHPUT_UP_THRESHOLD, ACPU_SUSPEND_RLQOS_DDR_MAX_VALUE);
#endif
#endif
/*===tele_mntn===*/
                }
                else
                {
                    ret = RET_OK;
                }
            }
        	if(RET_OK != ret)
        	{
        		pr_err(" pwrctrl_dfs_qos_update MAX failed .\n");

        	}
                    /*最小DDR 票处理*/
            if(g_acpu_ddr_freqlink.acpu_ddr_freqlink_cfg[i].ddr_min_freq_limit != 0)
            {
                ret=dfs_qos_update(DFS_QOS_ID_DDR_MINFREQ,&(g_acpu_ddr_freqlink.acpu_ddr_minfreq_req_id),g_acpu_ddr_freqlink.acpu_ddr_freqlink_cfg[i].ddr_min_freq_limit);
                g_acpu_ddr_freqlink.acpu_ddr_link_min_running_flag = 1;
/*===tele_mntn===*/
#if defined (CONFIG_HISILICON_PLATFORM_TELE_MNTN)
#if defined (CONFIG_HISILICON_PLATFORM_POWER_CONTROL)
                tele_mntn_acpu_ddr_freqlink(PM_QOS_MEMORY_THROUGHPUT, g_acpu_ddr_freqlink.acpu_ddr_freqlink_cfg[i].ddr_min_freq_limit);
#endif
#endif
/*===tele_mntn===*/
            }
            else
            {
                if(g_acpu_ddr_freqlink.acpu_ddr_link_min_running_flag == 1)
                {
                    ret=pwrctrl_dfs_qos_update(DEVICE_ID_DDR_MIN,&(g_acpu_ddr_freqlink.acpu_ddr_minfreq_req_id),ACPU_SUSPEND_RLQOS_DDR_MIN_VALUE);
                    g_acpu_ddr_freqlink.acpu_ddr_link_min_running_flag = 0;
/*===tele_mntn===*/
#if defined (CONFIG_HISILICON_PLATFORM_TELE_MNTN)
#if defined (CONFIG_HISILICON_PLATFORM_POWER_CONTROL)
                    tele_mntn_acpu_ddr_freqlink(PM_QOS_MEMORY_THROUGHPUT, ACPU_SUSPEND_RLQOS_DDR_MIN_VALUE);
#endif
#endif
/*===tele_mntn===*/
                }
            else
            {
                ret = RET_OK;
            }

            }
        	if(RET_OK != ret)
        	{
        		pr_err(" pwrctrl_dfs_qos_update MIN failed .\n");

        	}
        }
    	return 0;
    }
    else if(DEVICE_ID_DDR_MIN == dev_id)
    {
        return dfs_qos_update(DFS_QOS_ID_DDR_MINFREQ, req_id, req_value);
    }
    else if(DEVICE_ID_DDR_MAX == dev_id)
    {
        return dfs_qos_update(DFS_QOS_ID_DDR_MAXFREQ, req_id, req_value);
    }
    else
    {
        return -1;
    }
}

void dt_insmod_buslow(void)
{
	struct device_node *root = NULL;
    const char *out_string;

    root = of_find_compatible_node(NULL, NULL, "hisilicon,hi6xxxx-pm-buslow");
    if (!root)
    {
        pr_err("%s of_find_compatible_node failed\n",__func__);
        return ;
    }

	if(of_property_read_string(root,"status",&out_string))
	{
		pr_err("%s of_property_read_string failed\n",__func__);
		return;
	}

	if(0 == strcmp("ok",out_string))
	{
	    pm_qos_add_notifier(PM_QOS_BUSLOW_MINFREQ, &buslow_minfreq_notifier);
        pr_info("%s\n",__func__);
	}

}

void dt_insmod_bus_dfs(void)
{
	struct device_node *root = NULL;
    const char *out_string;

    root = of_find_compatible_node(NULL, NULL, "hisilicon,hi6xxxx-pm-busdfs");
    if (!root)
    {
        pr_err("%s of_find_compatible_node failed\n",__func__);
        return ;
    }

	if(of_property_read_string(root,"status",&out_string))
	{
		pr_err("%s of_property_read_string failed\n",__func__);
		return;
	}

	if(0 == strcmp("ok",out_string))
	{
	    pm_qos_add_notifier(PM_QOS_BUS_MINFREQ, &bus_minfreq_notifier);
        pr_info("%s\n",__func__);
	}

}

int dt_insmod_dvfs(void)
{
	struct device_node *root = NULL;
    const char *out_string;

    root = of_find_compatible_node(NULL, NULL, "hi6xxx,generic-smp-cpufreq");
    if (!root)
    {
        pr_err("%s of_find_compatible_node failed\n",__func__);
        return -1;
    }

	if(of_property_read_string(root,"status",&out_string))
	{
		pr_err("%s of_property_read_string status failed\n",__func__);
		return -1;
	}

	if(0 == strcmp("ok",out_string))
	{
        pr_info("%s\n",__func__);
        return 0;
	}

	return -1;

}

int pwrctrl_dfs_init(void)
{
    void __iomem *reg_addr;
    struct acpufreqinfo *p_freqinfo;

    reg_addr = (void __iomem *)HISI_VA_ADDRESS(SOC_PERI_SCTRL_SC_RESERVED8_ADDR(SOC_PERI_SCTRL_BASE_ADDR));
    g_dfs_data_addr = ioremap(ACPU_DFS_FREQ_ADDR, ACPU_DFS_FREQ_ADDR_SIZE);

    p_freqinfo = (struct acpufreqinfo *)g_dfs_data_addr;
    p_freqinfo->dfs_flag = 0;
    p_freqinfo->dfs_acpu_freq = 0;
    p_freqinfo->dfs_temp_freq = 0;

    g_stPwcSwitch = (ST_PWC_SWITCH_STRU *)(reg_addr);

	dt_insmod_buslow();
	dt_insmod_bus_dfs();
	if(!dt_insmod_dvfs()){g_adfs_module = 1;}

    wake_lock_init(&acpu_freq_lock,WAKE_LOCK_SUSPEND,"acpu_freq_lock");

    ddr_min_req.pm_qos_class = 0;
    ddr_max_req.pm_qos_class = 0;
    pm_qos_add_request(&ddr_min_req, DFS_QOS_ID_DDR_MINFREQ, QOS_DDR_MIN_DEFAULT_VALUE + 2);
    pm_qos_add_request(&ddr_max_req, DFS_QOS_ID_DDR_MAXFREQ, QOS_DDR_MAX_DEFAULT_VALUE - 2);

    return RET_OK;
}
