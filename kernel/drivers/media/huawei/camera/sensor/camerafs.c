/*
 * camera utile class driver 
 *
 *  Author: 	Zhoujie (zhou.jie1981@163.com)
 *  Date:  	2013/01/16
 *  Version:	1.0
 *  History:	2013/01/16      Frist add driver for dual temperature Led,this is virtual device to manage dual temperature Led 
 *  
 * ----------------------------------------------------------------------------
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 * ----------------------------------------------------------------------------
 *
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/fs.h>
#include "cam_log.h"

typedef  struct  _camerafs_class {
	struct class *classptr;
	struct device *pDevice;
}camerafs_class;

static camerafs_class camerafs;

//static int brightness_level = 0;
static dev_t devnum;

#define CAMERAFS_NODE    "node"

//#define MAX_BRIGHTNESS_FORMMI   (9)



int register_camerafs_attr(struct device_attribute *attr);

static int __init camerafs_module_init(void)
{
	int ret;
        
        camerafs.classptr = NULL;
        camerafs.pDevice = NULL;

       ret = alloc_chrdev_region(&devnum, 0, 1, CAMERAFS_NODE);
	if(ret)
	{
		printk("error %s fail to alloc a dev_t!!!\n",__func__);
		return -1;
	}

	camerafs.classptr= class_create(THIS_MODULE, "camerafs");
	if (IS_ERR(camerafs.classptr)) {
		cam_err("class_create failed %d\n", ret);
		ret = PTR_ERR(camerafs.classptr);
		return -1;
	}

	camerafs.pDevice  = device_create(camerafs.classptr, NULL, devnum,NULL,"%s",CAMERAFS_NODE);
	if (IS_ERR(camerafs.pDevice)) {
		cam_err("class_device_create failed %s \n", CAMERAFS_NODE);
		ret = PTR_ERR(camerafs.pDevice);
		return -1;
	}
        cam_info("%s end",__func__);
	return 0;
}

int register_camerafs_attr(struct device_attribute *attr)
{
	int ret = 0;

	ret = device_create_file(camerafs.pDevice,attr);
	if (ret<0)
	{
              cam_err("camera fs creat dev attr[%s] fail", attr->attr.name);
		return -1;
	}
       cam_info("camera fs creat dev attr[%s] OK", attr->attr.name);
	return 0;
}

EXPORT_SYMBOL(register_camerafs_attr);

static void __exit camerafs_module_deinit(void)
{
	device_destroy(camerafs.classptr, devnum);
	class_destroy(camerafs.classptr);
	unregister_chrdev_region(devnum, 1);
}

module_init(camerafs_module_init);
module_exit(camerafs_module_deinit);
MODULE_AUTHOR("Jiezhou");
MODULE_DESCRIPTION("Camera fs virtul device");
MODULE_LICENSE("GPL");
