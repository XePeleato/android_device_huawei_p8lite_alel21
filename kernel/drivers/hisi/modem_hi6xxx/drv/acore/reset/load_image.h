/*copyright (c) Huawei Technologies Co., Ltd. 1998-2014. All rights reserved.
 *
 * File name: load_image.h
 * Description: This is the head file for load_image.c.
 * Author: zengguangyue@huawei.com
 * Version: 0.1
 * Date:  2015/03/03
 */

#ifndef _LOAD_IMAGE_H_
#define _LOAD_IMAGE_H_

#ifdef __cplusplus 
extern "C"
{
#endif 

#define PTN_VRL_MODEM_NAME               "vrl"
#define PTN_MODEM_NAME			    "modemimage"

#define sec_print_err(fmt, ...)   (printk(KERN_ERR "[sec]: <%s> "fmt, __FUNCTION__, ##__VA_ARGS__))

#define SEC_OK      			(0)
#define SEC_ERROR  				(-1)
/*modem image's first DDR address */
#define MODEM_IMAGE_ADDR             (0x3E00)
/*the modem vrl's index in the VRL partition */
#define MODEM_VRL_INDEX              0x1F 

#define  VRL_SIZE		(0x800)  /*VRL 1K+1K*/
	
#ifdef __cplusplus 
}
#endif 
#endif

