

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include <linux/pwrctrl_power_state_manager.h>
#include <linux/hisi/pm/pwrctrl_multi_memcfg.h>



#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


unsigned int  *g_pwrctrl_sleep_state_votemap=NULL ;
#if 0
PWRCTRL_POWER_STAT_QOS_ID g_pwrctrl_power_stat_qos_id;
#endif
static DEFINE_SPINLOCK(power_stat_qos_lock);





s32_t  pwrctrl_request_power_state( s32_t module_id, s32_t state, s32_t *req_id)
{
	unsigned long flag = 0;
	if((module_id >= PWRCTRL_SLEEP_CLIENT_BUTT) || (module_id < 0) || (state >= PWRCTRL_SYS_STAT_BUTT)	\
		|| (state < 0) || (NULL == req_id))
	{
		return RET_ERR_PARAM_INVALID;
	}
	spin_lock_irqsave(&power_stat_qos_lock,flag);
	if((*(g_pwrctrl_sleep_state_votemap+state) & (0x1 << module_id)))
	{
		spin_unlock_irqrestore(&power_stat_qos_lock, flag);
		return RET_OK;
	}
	*(g_pwrctrl_sleep_state_votemap+state) |= (0x1 << module_id);
	*req_id = ((PWRCTRL_STATE_MAGIC << PWRCTRL_MAGIC_OFFSET) | ((state & 0xFF) << PWRCTRL_STATE_OFFSET)	\
		|((module_id & 0xFFFF) << PWRCTRL_MODULE_ID_OFFSET));
	spin_unlock_irqrestore(&power_stat_qos_lock, flag);
	return RET_OK;

	
}


s32_t  pwrctrl_release_power_state( s32_t req_id)

{
	s32_t magic;
	s32_t module_id;
	s32_t state;
	unsigned long flag = 0;

	magic = (req_id >> PWRCTRL_MAGIC_OFFSET) & 0xFF;
	state = (req_id >> PWRCTRL_STATE_OFFSET) & 0xFF;
	module_id = (req_id >> PWRCTRL_MODULE_ID_OFFSET) & 0xFFFF;

	if((module_id >= PWRCTRL_SLEEP_CLIENT_BUTT) || (module_id < 0) || (state >= PWRCTRL_SYS_STAT_BUTT)	\
		|| (state < 0) || (PWRCTRL_STATE_MAGIC != magic))
	{
		return RET_ERR_PARAM_INVALID;
	}

	spin_lock_irqsave(&power_stat_qos_lock,flag);
	if(!(*(g_pwrctrl_sleep_state_votemap+state)&(0x1 << module_id)))
	{
		spin_unlock_irqrestore(&power_stat_qos_lock, flag);
		return RET_OK;
	}
	*(g_pwrctrl_sleep_state_votemap+state) &= (~(0x1 << module_id) );

	spin_unlock_irqrestore(&power_stat_qos_lock, flag);
	return RET_OK;
}



static void  pwrctrl_power_state_manager_exit(void)
{
    printk(KERN_ERR "pwrctrl_power_state_manager_exit\n");

    if (g_pwrctrl_sleep_state_votemap){
        iounmap(g_pwrctrl_sleep_state_votemap);
    }
        return;
}




void get_power_stat_qos_value(s32_t state)
{
	unsigned int qos_value=0;
	qos_value=*(g_pwrctrl_sleep_state_votemap+state);
	printk(KERN_ERR "get_power_stat_qos_value:stat %d is 0x%x\n",state,qos_value);
}


s32_t   pwrctrl_power_state_manager_init(void)
{
	g_pwrctrl_sleep_state_votemap = (unsigned int*)ioremap(ACPU_POWER_STATE_QOS_ADDR,ACPU_POWER_STATE_QOS_SIZE);
	memset(g_pwrctrl_sleep_state_votemap,0,ACPU_POWER_STATE_QOS_SIZE);
	if(NULL == g_pwrctrl_sleep_state_votemap )
	{
		return -ENOMEM;
	}
	printk(KERN_ERR "pwrctrl_power_state_manager_init!\n");

	return RET_OK;
}





subsys_initcall(pwrctrl_power_state_manager_init);


MODULE_LICENSE("GPL");


