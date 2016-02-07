/*******************************************************************************
* All rights reserved, Copyright (C) huawei LIMITED 2012
*------------------------------------------------------------------------------
* File Name   : tc_client_driver.c
* Description :
* Platform    :
* Author      : qiqingchao
* Version     : V1.0
* Date        : 2012.12.10
* Notes       :
*
*------------------------------------------------------------------------------
* Modifications:
*   Date        Author          Modifications
*******************************************************************************/
/*******************************************************************************
 * This source code has been made available to you by HUAWEI on an
 * AS-IS basis. Anyone receiving this source code is licensed under HUAWEI
 * copyrights to use it in any way he or she deems fit, including copying it,
 * modifying it, compiling it, and redistributing it either with or without
 * modifications. Any person who transfers this source code or any derivative
 * work must include the HUAWEI copyright notice and this paragraph in
 * the transferred software.
*******************************************************************************/

#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/list.h>
#include <linux/mutex.h>
#include <linux/kthread.h>
#include <linux/freezer.h>
#include <linux/module.h>
#include <linux/atomic.h>
#include <linux/fs.h>
#include <linux/uaccess.h>


#include "tee_client_constants.h"
#include "teek_ns_client.h"
#include "agent.h"

//for secure agent
struct __smc_event_data{
    unsigned int agent_id;
    struct mutex work_lock;
    wait_queue_head_t wait_event_wq;
    int ret_flag;
    wait_queue_head_t send_response_wq;
    int send_flag;
    struct list_head head;
    void* buffer;
};
//struct __smc_event_data* smc_event_data;


struct __agent_control{
    spinlock_t lock;
    struct list_head agent_list;
};
static struct __agent_control agent_control;

static unsigned int exception_mem_addr;

struct __smc_event_data* find_event_control(unsigned long agent_id)
{
	struct __smc_event_data *event_data, *tmp_data=NULL;
	unsigned long flags;

	spin_lock_irqsave(&agent_control.lock, flags);
	list_for_each_entry(event_data, &agent_control.agent_list, head){
		if(event_data->agent_id == agent_id){
			tmp_data = event_data;
			break;
		}
	}
	spin_unlock_irqrestore(&agent_control.lock, flags);

	return tmp_data;
}

unsigned int TC_NS_incomplete_proceed(TC_NS_SMC_CMD *smc_cmd,
				      unsigned int agent_id,
				      uint8_t flags)
{
	unsigned int smc_cmd_phys;
	unsigned int ret = TEEC_PENDING2;
	struct __smc_event_data *event_data;
	int ret2;

	event_data = find_event_control(agent_id);

	if(event_data == NULL){
		//TODO if return, the pending task in S can't be resume!!
		TCERR("agent not exist\n");
		return TEEC_ERROR_GENERIC;
	}

	mutex_lock(&event_data->work_lock);

	/* just return for the first call */
	smc_cmd_phys = virt_to_phys((void*)smc_cmd);
	ret = TC_NS_SMC(smc_cmd_phys);
	if (ret != TEEC_PENDING2) {
		/*If TA crashed return err code to tzdriver*/
		TCERR("smc_call:TA tee_fs handle error:%d\n", ret);
		goto tee_error;
	}

	event_data->ret_flag = 1;
	/* Wake up the agent that will process the command */
	wake_up(&event_data->wait_event_wq);

	/* We must block until the agent is done. In Sync mode we ignore signals 
	 * because the call really has to finish until we let the process continue */
	if (flags & TC_CALL_SYNC) {
		wait_event(event_data->send_response_wq,
					event_data->send_flag);
	}
	else {
		ret2 = wait_event_interruptible(event_data->send_response_wq,
					event_data->send_flag);
		/* Wait event returns 0 for success(condition true) or -ERESTARTSYS for
		 * client interrupted */
		if (ret2){
			TCDEBUG("TC_NS_incomplete_proceed: client interrupted");
			ret = TEEC_CLIENT_INTR;
			goto tee_error;
		}
	}
	/* send the incomplete id */
	smc_cmd_phys = virt_to_phys((void*)smc_cmd);
	ret = TC_NS_SMC(smc_cmd_phys);

tee_error:
	event_data->send_flag = 0;
	mutex_unlock(&event_data->work_lock);
	return ret;
}

/*
 * Function:      is_agent_alive
 * Description:   This function check if the special agent is launched.Used For HDCP key.
 *                e.g. If sfs agent is not alive, you can not do HDCP key write to SRAM.
 * Parameters:   agent_id.
 * Return:      1:agent is alive
 *              0:agent not exsit.
 */
int is_agent_alive(unsigned int agent_id)
{
    if (find_event_control(agent_id))
        return 1;
    else
        return 0;
}

int TC_NS_wait_event(unsigned long agent_id)
{
	int ret= -EINVAL;
	struct __smc_event_data *event_data;

	event_data = find_event_control(agent_id);

	if (TC_NS_get_uid() != 0) {
		TCERR("It is a fake tee agent\n");
		return -EACCES;
	}

	if(event_data){
		/* wait event will return either 0 or -ERESTARTSYS so just
		 * return it further to the ioctl handler */
		ret = wait_event_interruptible(event_data->wait_event_wq,
					       event_data->ret_flag);
		event_data->ret_flag = 0;
	}

	return ret;
}

int TC_NS_send_event_reponse(unsigned long agent_id)
{
    struct __smc_event_data *event_data=find_event_control(agent_id);

    if (TC_NS_get_uid() != 0) {
        TCERR("It is a fake tee agent\n");
        return -1;
    }

    if(event_data){
        event_data->send_flag = 1;
	/* Wake up the client waiting */
        wake_up(&event_data->send_response_wq);
    }

    return 0;
}

int TC_NS_register_agent(TC_NS_DEV_File* dev_file, unsigned int agent_id,
			 TC_NS_Shared_MEM* shared_mem)
{
    TC_NS_SMC_CMD smc_cmd = {0};
    unsigned int smc_cmd_phys;
    struct __smc_event_data *event_data=NULL;
    int ret;
    int find_flag = 0;
    unsigned long flags;
    unsigned char uuid[17] = {0};
    TC_NS_Operation operation = {0};


    if (TC_NS_get_uid() != 0) {
        TCERR("It is a fake tee agent\n");
        ret = TEEC_ERROR_GENERIC;
        goto error;
    }


    spin_lock_irqsave(&agent_control.lock, flags);
    list_for_each_entry(event_data, &agent_control.agent_list, head){
        if(event_data->agent_id == agent_id){
            find_flag = 1;
            break;
        }
    }
    spin_unlock_irqrestore(&agent_control.lock, flags);

    if(find_flag){
        printk(KERN_WARNING"agent is exist\n");
        ret = TEEC_ERROR_GENERIC;
        goto error;
    }

    if(!shared_mem){
        printk(KERN_WARNING"shared mem is not exist\n");
        ret = TEEC_ERROR_GENERIC;
        goto error;
    }

    operation.paramTypes = TEE_PARAM_TYPE_VALUE_INPUT;
    operation.paramTypes = operation.paramTypes << 12;
    operation.params[0].value.a = virt_to_phys(shared_mem->kernel_addr);
    operation.params[0].value.b = SZ_4K;

    //mmap shared mem in secure_os
    //memset(&smc_cmd, 0, sizeof(TC_NS_SMC_CMD));
    uuid[0] = 1;
    smc_cmd.uuid_phys = virt_to_phys(uuid);
    smc_cmd.cmd_id = GLOBAL_CMD_ID_REGISTER_AGENT;
    smc_cmd.operation_phys = virt_to_phys(&operation);
    smc_cmd.agent_id = agent_id;

    smc_cmd_phys = virt_to_phys(&smc_cmd);

    ret = TC_NS_SMC(smc_cmd_phys);

    if(ret == TEEC_SUCCESS){
        event_data = NULL;
        event_data = kmalloc(sizeof(struct __smc_event_data), GFP_KERNEL);
        if(!event_data){
            ret = -ENOMEM;
            goto error;
        }
        event_data->agent_id = agent_id;
        mutex_init(&event_data->work_lock);
        event_data->ret_flag = 0;
        event_data->send_flag = 0;
        event_data->buffer = shared_mem->kernel_addr;
        init_waitqueue_head(&(event_data->wait_event_wq));
        init_waitqueue_head(&(event_data->send_response_wq));
        INIT_LIST_HEAD(&event_data->head);

        spin_lock_irqsave(&agent_control.lock, flags);
        list_add_tail(&event_data->head, &agent_control.agent_list);
        spin_unlock_irqrestore(&agent_control.lock, flags);
    }

error:
    return ret;
}

int TC_NS_unregister_agent(unsigned long agent_id)
{
    struct __smc_event_data *event_data=NULL;
    int ret;
    int find_flag = 0;
    unsigned long flags;
    TC_NS_SMC_CMD smc_cmd = {0};
    unsigned int smc_cmd_phys;
    unsigned char uuid[17] = {0};
    TC_NS_Operation operation = {0};
    if (TC_NS_get_uid() != 0) {
        TCERR("It is a fake tee agent\n");
        return TEEC_ERROR_GENERIC;
    }

    spin_lock_irqsave(&agent_control.lock, flags);
    list_for_each_entry(event_data, &agent_control.agent_list, head){
        if(event_data->agent_id == agent_id){
            find_flag = 1;
            list_del(&event_data->head);
            break;
        }
    }
    spin_unlock_irqrestore(&agent_control.lock, flags);

    if(!find_flag){
        TCERR(KERN_WARNING"agent is not found\n");
        return TEEC_ERROR_GENERIC;
    }

    operation.paramTypes = TEE_PARAM_TYPE_VALUE_INPUT;
    operation.paramTypes = operation.paramTypes << 12;
    operation.params[0].value.a= virt_to_phys(event_data->buffer);
    operation.params[0].value.b = SZ_4K;

    //memset(&smc_cmd, 0, sizeof(TC_NS_SMC_CMD));
    uuid[0] = 1;
    smc_cmd.uuid_phys = virt_to_phys(uuid);
    smc_cmd.cmd_id = GLOBAL_CMD_ID_UNREGISTER_AGENT;
    smc_cmd.operation_phys = virt_to_phys(&operation);
    smc_cmd.agent_id = agent_id;

    mutex_lock(&event_data->work_lock);
    smc_cmd_phys = virt_to_phys(&smc_cmd);

    ret = TC_NS_SMC(smc_cmd_phys);
    mutex_unlock(&event_data->work_lock);

    kfree(event_data);

    return ret;
}

int TC_NS_alloc_exception_mem(unsigned int agent_id)
{
    TC_NS_SMC_CMD smc_cmd = {0};
    unsigned int smc_cmd_phys;
    int ret;
    unsigned char uuid[17] = {0};
    void *exception_mem;
    TC_NS_Operation operation = {0};

    exception_mem = (void *)kzalloc(EXCEPTION_MEM_SIZE, GFP_KERNEL);
    exception_mem_addr = (char *)exception_mem;

    TCDEBUG(KERN_INFO"exception_mem virt addr = 0x%lx, phys addr = 0x%lx\n", exception_mem, virt_to_phys(exception_mem));

    //memset(&smc_cmd, 0, sizeof(TC_NS_SMC_CMD));
    uuid[0] = 1;
    smc_cmd.uuid_phys = virt_to_phys(uuid);
    smc_cmd.cmd_id = GLOBAL_CMD_ID_ALLOC_EXCEPTION_MEM;

    operation.paramTypes = TEE_PARAM_TYPE_VALUE_INPUT;
    operation.paramTypes = operation.paramTypes << 12;
    operation.params[0].value.a= virt_to_phys(exception_mem);
    operation.params[0].value.b = EXCEPTION_MEM_SIZE;

    smc_cmd.operation_phys = virt_to_phys(&operation);

    smc_cmd_phys = virt_to_phys(&smc_cmd);

    ret = TC_NS_SMC(smc_cmd_phys);

//error:
    return ret;
}


int TC_NS_store_exception_info(void)
{
    struct file *filep;
    //char exception_info[]="hello world.\r\n";
    char *p_excp_mem = exception_mem_addr;
    ssize_t write_len;
    //char read_buff[20]={0};
    mm_segment_t old_fs;
    loff_t pos = 0;

    /*exception handling, store trustedcore exception info to file*/
    filep=filp_open("/data/exception_log.log",O_CREAT|O_RDWR,0);
    if(IS_ERR(filep))
    {
        TCDEBUG("Failed to filp_open exception_log.log, filep=0x%x\n", (unsigned long)filep);
        return -1;
    }

    TCDEBUG("Succeed to filp_open exception_log.log, filep=0x%x\n", (unsigned long)filep);

    old_fs = get_fs();
    set_fs(KERNEL_DS);

    write_len = vfs_write(filep, (char __user*)p_excp_mem, EXCEPTION_MEM_SIZE, &pos);
    if (write_len < 0)
    {
        TCDEBUG("Failed to write to exception_log.log\n");
    }
    else
    {
        TCDEBUG("Succeed to Write hello world to exception_log.log, write_len=%d\n", write_len);
    }
    pos = 0;
    //read_len = vfs_read(filep, (char __user*)read_buff, 15, &pos);
    //TCDEBUG("read_buff is %s, read_len=%d\n", read_buff,read_len);

    set_fs(old_fs);
    filp_close(filep,0);
    return 0;
}

int agent_init(void)
{
    spin_lock_init(&agent_control.lock);
    INIT_LIST_HEAD(&agent_control.agent_list);

    return 0;
}
