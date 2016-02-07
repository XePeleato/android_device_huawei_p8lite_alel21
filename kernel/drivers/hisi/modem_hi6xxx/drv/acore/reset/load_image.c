/*copyright (c) Huawei Technologies Co., Ltd. 1998-2014. All rights reserved.
 *
 * File name: load_image.c
 * Description: This file use to security verification when  modem reboot singly.
 * Author: zengguangyue@huawei.com
 * Version: 0.1
 * Date:  2015/03/03
 */
#include "product_config.h"
#include <linux/module.h>
#include <linux/io.h>
#include "load_image.h"
#include "reset_ccoreloader.h"
#include <linux/hisi/hi6xxx-iomap.h>
#include <linux/slab.h>
#include <teek_client_api.h>
#include <teek_client_id.h>

#define SECBOOT_BUFLEN  (0x100000)
static  u8 SECBOOT_BUFFER[SECBOOT_BUFLEN]; /*1M Bytes*/
typedef enum SVC_SECBOOT_IMG_TYPE SECBOOT_IMG_TYPE;

extern int ccore_drv_read_bin(const char *partion_name, unsigned int offset, unsigned int length, char *buffer);

/*
 * Function name:TEEK_init.
 * Discription:Init the TEEC and get the context
 * Parameters:
 *      @ session: the bridge from unsec world to sec world.
 *      @ context: context.
 * return value:
 *      @ TEEC_SUCCESS-->success, others-->failed.
 */
static s32 TEEK_init(TEEC_Session *session, TEEC_Context *context)
{
	TEEC_Result result;
	TEEC_UUID svc_uuid = TEE_SERVICE_SECBOOT;  

	result = TEEK_InitializeContext(
			               NULL,
			               context);

	if(result != TEEC_SUCCESS) {
		sec_print_err("TEEK_InitializeContext failed!\n");
		result = SEC_ERROR;
		goto error;
	}

	result = TEEK_OpenSession(
                context,
                session,
                &svc_uuid,
                TEEC_LOGIN_PUBLIC,
                NULL,
                NULL,
                NULL);

	if (result != TEEC_SUCCESS) 
	{
		sec_print_err("TEEK_OpenSession failed!\n");
		result = SEC_ERROR;
		TEEK_FinalizeContext(context);
	}  
	
error:

	return result;
}


/*
 * Function name:reset_soc_image.
 * Discription:reset the core in sec_OS, like modem and hifi core  .
 * Parameters:
 *      @ session: the bridge from unsec world to sec world.
 *      @ image: the core to reset.
 * return value:
 *      @ TEEC_SUCCESS-->success, others-->failed.
 */
s32 reset_soc_image(TEEC_Session *session,
		  SECBOOT_IMG_TYPE  image)
{
	TEEC_Result result;
	TEEC_Operation operation;
	u32 origin;
	
	operation.started = 1;
	operation.cancel_flag = 0;
	operation.paramTypes = TEEC_PARAM_TYPES(
						TEEC_VALUE_INPUT,
						TEEC_NONE,
						TEEC_NONE,
						TEEC_NONE);	
	
        operation.params[0].value.a = image; /*MODEM/HIFI*/
        result = TEEK_InvokeCommand(
                                    session,
                                   SECBOOT_CMD_ID_RESET_IMAGE ,
                                    &operation,
                                    &origin);
                                    
          if (result != TEEC_SUCCESS) {
    		sec_print_err("reset  failed, result is 0x%x!\n", result);
    		result = SEC_ERROR;
    	}

	return result;
}

/*
 * Function name:trans_vrl_to_os.
 * Discription:transfer vrl data to sec_OS
 * Parameters:
 *      @ session: the bridge from unsec world to sec world.
 *      @ image: the data of the image to transfer.
 *      @ buf: the buf in  kernel to transfer
 *      @ size: the size to transfer.
 * return value:
 *      @ TEEC_SUCCESS-->success, others--> failed.
 */
static s32 trans_vrl_to_os(TEEC_Session *session,
                        SECBOOT_IMG_TYPE  image,
		  void * buf,
		  const unsigned int size)
{
	TEEC_Result result;
	TEEC_Operation operation;
	u32 origin;

	operation.started = 1;
	operation.cancel_flag = 0;
	operation.paramTypes = TEEC_PARAM_TYPES(
						TEEC_VALUE_INPUT,
						TEEC_VALUE_INPUT,
						TEEC_NONE,
						TEEC_NONE);
						
	operation.params[0].value.a = image;
	operation.params[1].value.a = (u32)virt_to_phys(buf);
	operation.params[1].value.b = size;
	
	result = TEEK_InvokeCommand(
				session,
				SECBOOT_CMD_ID_COPY_VRL_TYPE,
				&operation,
				&origin);
				
	if (result != TEEC_SUCCESS) {
		sec_print_err("invoke failed!\n");
		result = SEC_ERROR;
	}

	return result;
}

/*
 * Function name:trans_data_to_os.
 * Discription:transfer image data to sec_OS
 * Parameters:
 *      @ session: the bridge from unsec world to sec world.
 *      @ image: the data of the image to transfer.
 *      @ run_addr: the image entry address.
 *      @ buf: the buf in  kernel to transfer
 *      @ offset: the offset to run_addr.
 *      @ size: the size to transfer.
 * return value:
 *      @ TEEC_SUCCESS-->success, others--> failed.
 */
static s32 trans_data_to_os(TEEC_Session *session,
                        SECBOOT_IMG_TYPE  image,
                        u32 run_addr,
		  void * buf,
		  const unsigned int offset,
		  const unsigned int size)
{
	TEEC_Result result;
	TEEC_Operation operation;
	u32 origin;

	operation.started = 1;
	operation.cancel_flag = 0;
	operation.paramTypes = TEEC_PARAM_TYPES(
						TEEC_VALUE_INPUT,
						TEEC_VALUE_INPUT,
						TEEC_VALUE_INPUT,
						TEEC_NONE);
						
	operation.params[0].value.a = image;
	operation.params[1].value.a = run_addr;
	operation.params[1].value.b = offset;
	operation.params[2].value.a = (u32)virt_to_phys(buf);
	operation.params[2].value.b = size;

	result = TEEK_InvokeCommand(
				session,
				SECBOOT_CMD_ID_COPY_DATA_TYPE,
				&operation,
				&origin);
				
	if (result != TEEC_SUCCESS) {
		sec_print_err("invoke failed!\n");
		result = SEC_ERROR;
	}

	return result;
}

/*
 * Function name:load_data_to_os.
 * Discription:cut the  image data to 1M per block, and trans them to  sec_OS.
 * Parameters:
 *      @ session: the bridge from unsec world to sec world.
 *      @ image: the data of the image to transfer.
 *      @ part_name: image's partition name. 
 *      @ run_addr: the image entry address.
 *      @ offset: the offset to run_addr.
 *      @ sizeToRead: total  size of the image.
 * return value:
 *      @ SEC_OK-->success, others--> failed.
 */
static s32 load_data_to_os(TEEC_Session *session,  
                        SECBOOT_IMG_TYPE  image, 
                        char* part_name, 
                        u32 run_addr,
                        u32 offset, 
                        u32 sizeToRead)
{
	u32 read_bytes;
	u32 end_bytes;
	u32 copy_bytes;
	u32 timers;
	u32 i;
	s32 ret = SEC_ERROR;	

	/*split the size to be read to each 64K bytes.*/
	timers = sizeToRead / SECBOOT_BUFLEN;	
	if (sizeToRead % SECBOOT_BUFLEN)		
	timers = timers + 1;

	end_bytes = sizeToRead;
	
	/*call flash_read each time to read to memDst.*/
	for (i = 0; i < timers; i++) 
	{		
		if (end_bytes >= SECBOOT_BUFLEN)
			read_bytes = SECBOOT_BUFLEN;
		else
			read_bytes = end_bytes;
			
		if (ccore_drv_read_bin(part_name, offset + i * SECBOOT_BUFLEN, read_bytes, (void *)SECBOOT_BUFFER)) {
			sec_print_err("%s: err: flash_read\n", __func__);
			return SEC_ERROR;
		}

		if (end_bytes >= SECBOOT_BUFLEN) {
			if (SECBOOT_BUFLEN != read_bytes) {
				sec_print_err("%s: err: SECBOOT_BUFLEN:%d != read_bytes:%d\r\n", __func__, SECBOOT_BUFLEN, read_bytes);
				return SEC_ERROR;
			}
			copy_bytes = SECBOOT_BUFLEN;
		} else {
			if (end_bytes > read_bytes) {
				sec_print_err("%s: err: end_bytes:%d!= read_bytes:%d\r\n", __func__, end_bytes, read_bytes);
				return SEC_ERROR;
			}
			copy_bytes = end_bytes;
		}
                     
		ret = trans_data_to_os(session, image, run_addr, (void *)(SECBOOT_BUFFER), (i * SECBOOT_BUFLEN), copy_bytes);	
		if (SEC_ERROR == ret)
		{
			sec_print_err("modem image trans to os is failed, error code 0x%x\r\n", ret);
			return SEC_ERROR;
		}

		end_bytes -= copy_bytes;
	}
	
	if (0 != end_bytes) {
		sec_print_err("%s: end_bytes = 0x%x\n", __func__, end_bytes);
		return SEC_ERROR;
	}
	
	return SEC_OK;
}
 
/*
 * Function name:load_modem_vrl_to_os.
 * Discription:load modem  image's VRL  data to sec_OS
 * Parameters:
 *      @ session: the bridge from unsec world to sec world.
 * return value:
 *      @ SEC_OK--> success, others--> failed.
 */
static s32 load_modem_vrl_to_os(TEEC_Session *session)
{
        s32 ret = SEC_ERROR;

	/**
           *get modem vrl info
           *the modem vrl info is allocated in the address of 0x8F800 in vrl partition
           */
	ret  = ccore_drv_read_bin((const char*)PTN_VRL_MODEM_NAME, (MODEM_VRL_INDEX * VRL_SIZE), VRL_SIZE, (void *)SECBOOT_BUFFER);
	if (ret < 0)
	{
		sec_print_err( "fail to read the vrl of modem image\n");
		return SEC_ERROR;
	}
        
	/* trans the vrl to secure os. */
	ret = trans_vrl_to_os(session, MODEM, (void *)SECBOOT_BUFFER, VRL_SIZE);
	if (SEC_ERROR == ret)
	{
		sec_print_err("modem image trans to os is failed, error code 0x%x\r\n", ret);
		return SEC_ERROR;
	}
	
	return SEC_OK;
}


/*
 * Function name:load_modem_data_to_os.
 * Discription:load modem  image  data to sec_OS
 * Parameters:
 *      @ session: the bridge from unsec world to sec world.
 * return value:
 *      @ SEC_OK--> success, others--> failed.
 */
static s32 load_modem_data_to_os(TEEC_Session *session)
{
	s32 ret = SEC_ERROR;
	CCORE_IMAGE_HEAD_STRU *header = NULL;
	const char      *image_name = PTN_MODEM_NAME;

	header  = (CCORE_IMAGE_HEAD_STRU *)kmalloc(sizeof(CCORE_IMAGE_HEAD_STRU), GFP_KERNEL);
	if(NULL  == header)
	{
    		sec_print_err("header  malloc fail\n");
    		goto err_out;
	}
	hisi_io_memset((void*)header, 0, sizeof(CCORE_IMAGE_HEAD_STRU));
	/*get the elf*/
	ret = ccore_drv_read_bin((const char*)image_name, 0, sizeof(CCORE_IMAGE_HEAD_STRU), (char *)header);
	if (ret < 0)
	{
		sec_print_err("fail to read the head of modem image\n");
		ret = SEC_ERROR;
		goto err_out;
	}

	/*load modem data to sec os*/
	ret = load_data_to_os(session, MODEM, PTN_MODEM_NAME,MODEM_IMAGE_ADDR, 0,header->uwImageSize);
	if(SEC_ERROR == ret)
	{
		sec_print_err("load_data_to_os fail!\n");
		goto err_out;
	}
	
err_out:	
	if(NULL != header)
	{
		kfree(header);
		header = NULL;
	}
	return ret;
}


/*
 * Function name:start_soc_image.
 * Discription:start the image verification, if success, unreset the soc
 * Parameters:
 *      @ session: the bridge from unsec world to sec world.
 *      @ image: the image to verification and unreset
 *      @ run_addr: the image entry address 
 * return value:
 *      @ TEEC_SUCCESS-->success, others-->failed.
 */
s32 start_soc_image(TEEC_Session *session,
                        SECBOOT_IMG_TYPE  image,
                        u32 run_addr)
{
	TEEC_Result result;
	TEEC_Operation operation;
	u32 origin;

	operation.started = 1;
	operation.cancel_flag = 0;
	operation.paramTypes = TEEC_PARAM_TYPES(
						TEEC_VALUE_INPUT,
						TEEC_VALUE_INPUT,
						TEEC_NONE,
						TEEC_NONE);		
         operation.params[0].value.a = image;
         operation.params[1].value.a = run_addr;
         operation.params[1].value.b = 0;/*SECBOOT_LOCKSTATE , not used currently*/
         result = TEEK_InvokeCommand(
                                        session,
                                       SECBOOT_CMD_ID_VERIFY_DATA_TYPE,
                                        &operation,
                                        &origin);
                                        
          if (result != TEEC_SUCCESS) {
    		sec_print_err("start  failed, result is 0x%x!\n", result);
    		result = SEC_ERROR;
    	}
    	
	return result;
}

/*
 * Function name:load_check_sec_image.
 * Discription:load the image to secureOS and the SecureOS verify it.
 * Parameters:
 *      @ void.
 *Called in:
 *      @ ccorereset_task
 * return value:
 *      @ SEC_OK-->success, others-->failed.
 */
s32 load_check_sec_image(void)
{
	TEEC_Session session;
	TEEC_Context context;
	s32 ret = SEC_ERROR;
	
	ret = TEEK_init(&session, &context);
	if(SEC_ERROR == ret)
	{
		sec_print_err("TEEK_InitializeContext failed!\n");
		return ret;
	}

	ret = reset_soc_image(&session, MODEM);
	if(SEC_ERROR == ret)
	{
		sec_print_err("reset_soc_image  fail!\n");
		goto error;
	}
	
	ret = load_modem_vrl_to_os(&session);
	if(SEC_ERROR == ret)
	{
		sec_print_err("load_modem_vrl_to_os fail!\n");
		goto error;
	}

	ret = load_modem_data_to_os(&session);
	if(SEC_ERROR == ret)
	{
		sec_print_err("load_modem_data_to_os fail!\n");
		goto error;
	}
	/*end of trans all data, start verify, if success, unreset the SOC*/
	ret = start_soc_image(&session,MODEM,MODEM_IMAGE_ADDR);
	if(SEC_ERROR == ret)
	{
		sec_print_err("start_soc_image  fail!\n");
		goto error;
	}
	
	sec_print_err("SEC_OS start_soc_image  success!\n");
error:	
	TEEK_CloseSession(&session);
	TEEK_FinalizeContext(&context);
	
	return ret;
}
/*
 * Function name:ccorereset_doreset.
 * Discription:reset the modem in SEC_OS.
 * Parameters:
 *      @ void.
 *Called in:
 *      @ modem_reset_set
 * return value:
 *      @ SEC_OK-->success, others-->failed.
 */
int ccorereset_doreset(void)
{
	TEEC_Session session;
	TEEC_Context context;
	s32 ret = SEC_ERROR;
	
	ret = TEEK_init(&session, &context);
	if(SEC_ERROR == ret)
	{
		sec_print_err("TEEK_InitializeContext failed!\n");
		return ret;
	}
	
	ret = reset_soc_image(&session, MODEM);
	if(SEC_ERROR == ret)
	{
		sec_print_err("reset_soc_image  fail!\n");
		goto error;
	}
	
	sec_print_err("SEC_OS reset_soc_image  success!\n");
error:	
	TEEK_CloseSession(&session);
	TEEK_FinalizeContext(&context);
	
	return ret;	
}
