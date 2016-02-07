/*copyright (c) Huawei Technologies Co., Ltd. 1998-2014. All rights reserved.
 * 
 * File name: hw_power_monitor.c
 * Description: This file use to record power state for upper layer
 * Author: ivan.chengfeifei@huawei.com
 * Version: 0.1
 * Date:  2014/11/27
 */
#include <linux/types.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/string.h>
#include <linux/kobject.h>
#include <linux/debugfs.h>
#include <linux/hw_power_monitor.h>
#include "power.h"


#define IRQ_TOTAL  200

unsigned int suspend_total = 0;
bool freezing_wakeup_check = false;
static struct power_monitor_info pm_info[POWER_MONITOR_MAX];
static DEFINE_MUTEX(power_monitor_mutex);

struct dcurrent_s  {
char * devname;
int number;
};

#define CVS(name,num)	{\
.devname = name,\
.number = num,\
}



struct dcurrent_s dcurrent[] = {
  	CVS("a53_0_COMMRX0",0),	/* 42 */
  	CVS("a53_0_COMMRX1",0),	/* 43 */
  	CVS("a53_0_COMMRX2",0),	/* 44 */
  	CVS("a53_0_COMMRX3",0),	/* 45 */
  	CVS("a53_0_COMMTX0",0),	/* 46 */
  	CVS("a53_0_COMMTX1",0),	/* 47 */
  	CVS("a53_0_COMMTX2",0),	/* 48 */
  	CVS("a53_0_COMMTX3",0),	/* 49 */
        CVS("a53_1_interr",0),
        CVS("a53_1_exterr",0),
        CVS("a53_1_pmu0",0),
        CVS("a53_1_pmu1",0),
        CVS("a53_1_pmu2",0),
        CVS("a53_1_pmu3",0),
        CVS("a53_1_cti0",0),
        CVS("a53_1_cti1",0),
        CVS("a53_1_cti2",0),
        CVS("a53_1_cti3",0),
        CVS("a53_1_COMMRX0",0),
        CVS("a53_1_COMMRX1",0),
        CVS("a53_1_COMMRX2",0),
        CVS("a53_1_COMMRX3",0),
        CVS("a53_1_COMMTX0",0),
        CVS("a53_1_COMMTX1",0),
        CVS("a53_1_COMMTX2",0),
        CVS("a53_1_COMMTX3",0),	
        CVS("TIME00",0),
        CVS("TIME01",0),
        CVS("TIME10",0),
        CVS("TIME11",0),
        CVS("TIME20",0),
        CVS("TIME21",0),
        CVS("TIME30",0),
        CVS("TIME31",0),
        CVS("TIME40",0),
        CVS("TIME41",0),
        CVS("TIME50",0),
        CVS("TIME51",0),
        CVS("TIME60",0),
        CVS("TIME61",0),
        CVS("TIME70",0),
        CVS("TIME71",0),
        CVS("WatchDog0",0),
        CVS("WatchDog1",0),
   	    CVS("RTC0",0),
        CVS("RTC1",0),
        CVS("UART0",0),
        CVS("UART1",0),
        CVS("UART2",0),
        CVS("UART3",0),
        CVS("UART4",0),
        CVS("UART5",0),
        CVS("UART6",0),
        CVS("SPI0",0),
        CVS("SPI1",0),
        CVS("TSI0",0),
        CVS("TSI1",0),
        CVS("I2C0",0),
        CVS("I2C1",0),
        CVS("I2C2",0),
        CVS("I2C3",0),
        CVS("I2C4",0),
        CVS("I2C5",0),
        CVS("GPIO0",0),
        CVS("GPIO1",0),
        CVS("GPIO2",0),
        CVS("GPIO3",0),
        CVS("GPIO4",0),
        CVS("GPIO5",0),
        CVS("GPIO6",0),
        CVS("GPIO7",0),
        CVS("GPIO8",0),
        CVS("GPIO9",0),
        CVS("GPIO10",0),
        CVS("GPIO11",0),
        CVS("GPIO12",0),
        CVS("GPIO13",0),
        CVS("GPIO14",0),
        CVS("GPIO15",0),
        CVS("GPIO16",0),
        CVS("GPIO17",0),
        CVS("GPIO18",0),
        CVS("GPIO19",0),
        CVS("GPIO20",0),
        CVS("GPIO21",0),
        CVS("GPIO22",0),
        CVS("GPIO23",0),
        CVS("GPIO24",0),
        CVS("GPIO25",0),
        CVS("GPIO26",0),
        CVS("CoreSight_ETR_Full",0),
        CVS("CoreSight_ETF_Full",0),
        CVS("CCI400_err",0),
        CVS("SECENG_P",0),
        CVS("SECENG_S",0),
        CVS("NANDC",0),
        CVS("EMMC",0),
        CVS("SD",0),
        CVS("SDIO0",0),
        CVS("DDRC",0),
        CVS("SCTRL",0),
        CVS("PMCTRL",0),
        CVS("TSENSOR_a53_0",0),
        CVS("TSENSOR_a53_1",0),
        CVS("TSENSOR_G3D",0),
        CVS("TSENSOR_Modem",0),
        CVS("ISP",0),
        CVS("ISP-mmu",0),
        CVS("CSI-p",0),
        CVS("CSI-s",0),
        CVS("ASP",0),
        CVS("ASP-ns",0),
        CVS("G3D_JOB",0),
        CVS("G3D_MMU",0),
        CVS("G3D_GPU",0),
        CVS("JPEG",0),
        CVS("VPP",0),
        CVS("VPP_mmu",0),
        CVS("VENC",0),
        CVS("PMC-DDRC-DFS",0),
        CVS("PMC-DDRC-CFG",0),
        CVS("PMC-DVFS-a53_0",0),
        CVS("PMC-DVFS-a53_1",0),
        CVS("PMC-DVFS-G3D",0),
        CVS("PMC-AVS-a53_0",0),
        CVS("PMC-AVS-a53_1",0),
        CVS("PMC-AVS-G3D",0),
        CVS("PMC-AVS-IDLE-a53_0",0),
	CVS("PMC-AVS-IDLE-a53_1",0),
	CVS("PMC-AVS-IDLE-G3D",0),
	CVS("M3_IO_wd",0),
	CVS("M3_LP_wd",0),
	CVS("IPC_S_int0",0),
	CVS("IPC_S_int1",0),
	CVS("IPC_S_int4",0),
	CVS("IPC_S_mbx0",0),
	CVS("IPC_S_mbx1",0),
	CVS("IPC_S_mbx2",0),
	CVS("IPC_S_mbx3",0),
	CVS("IPC_S_mbx4",0),
	CVS("IPC_S_mbx5",0),
	CVS("IPC_S_mbx6",0),
	CVS("IPC_S_mbx7",0),
	CVS("IPC_S_mbx8",0),
	CVS("IPC_S_mbx9",0),
	CVS("IPC_S_mbx18",0),
	CVS("IPC_NS_int0",0),
	CVS("IPC_NS_int1",0),
	CVS("IPC_NS_int4",0),
	CVS("IPC_NS_int5",0),
	CVS("IPC_NS_int6",0),
	CVS("IPC_S_mbx0",0),
	CVS("IPC_NS_mbx1",0),
	CVS("IPC_NS_mbx2",0),
	CVS("IPC_NS_mbx3",0),
	CVS("IPC_NS_mbx4",0),
	CVS("IPC_NS_mbx5",0),
	CVS("IPC_NS_mbx6",0),
	CVS("IPC_NS_mbx7",0),
	CVS("IPC_NS_mbx8",0),
	CVS("IPC_NS_mbx9",0),
	CVS("IPC_NS_mbx18",0),
	CVS("aximon_cpufast_intr|aximon_soc_intr",0),
	CVS("MDM_WDOG_intr",0),
	CVS("ASP-IPC-ARM",0),
	CVS("ASP-IPC-MCPU",0),
	CVS("ASP-IPC-BBE16",0),
	CVS("ASP_WD",0),
	CVS("SCI0",0),
	CVS("SCI1",0),
	CVS("SOCP0",0),
	CVS("SOCP1",0),
    CVS("MDM2AP[0]",0),
    CVS("MDM2AP[1]",0),
  	CVS("MDM2AP[2]",0),     	/* 252 */
  	CVS("MDM2AP[3]",0),     	/* 253 */
  	CVS("GIC_IRQ_OUT[0]",0),	/* 254 */
	CVS("GIC_IRQ_OUT[1]",0),	/* 255 */
 	CVS("GIC_IRQ_OUT[2]",0),	/* 256 */
  	CVS("GIC_IRQ_OUT[3]",0),	/* 257 */
  	CVS("GIC_IRQ_OUT[4]",0),	/* 258 */
  	CVS("GIC_IRQ_OUT[5]",0),	/* 259 */
	CVS("GIC_IRQ_OUT[6]",0),	/* 260 */
	CVS("GIC_IRQ_OUT[7]",0),	/* 261 */
	CVS("GIC_FIQ_OUT[0]",0),	/* 262 */
	CVS("GIC_FIQ_OUT[1]",0),	/* 263 */
	CVS("GIC_FIQ_OUT[2]",0),	/* 264 */
	CVS("GIC_FIQ_OUT[3]",0),	/* 265 */
	CVS("GIC_FIQ_OUT[4]",0),	/* 266 */
	CVS("GIC_FIQ_OUT[5]",0),	/* 267 */
	CVS("GIC_FIQ_OUT[6]",0),	/* 268 */
	CVS("GIC_FIQ_OUT[7]",0), 	/* 269 */
	CVS("UICC_intr",0),		/* 270 */
	CVS("A53_0_COMMMIRQ0",0),	/* 271 */
	CVS("A53_0_COMMMIRQ1",0),	/* 272 */
	CVS("A53_0_COMMMIRQ2",0),	/* 273 */
	CVS("A53_0_COMMMIRQ3",0),	/* 274 */
	CVS("A53_1_COMMMIRQ0",0),	/* 275 */
	CVS("A53_1_COMMMIRQ1",0),	/* 276 */
	CVS("A53_1_COMMMIRQ2",0),	/* 277 */
	CVS("A53_1_COMMMIRQ3",0),	/* 278 */
};

/*
 * Function: is_id_valid
 * Description: check ID valid or not
 * Input:  @id - id to check
 * Output:
 * Return: false -- invalid
 *         true -- valid
 */
static inline bool is_id_valid(int id)
{
	return (id >= AP_SLEEP && id < POWER_MONITOR_MAX);
}

/*
 * Function: report_handle
 * Description: Packet and Send data to power node
 * Input: id --- message mask
 :*        fmt -- string
 * Return: -1--failed, 0--success
 */
static int report_handle(unsigned int id,  va_list args, const char *fmt)
{
	int length = 0;
        int irq_number = 0;
	char buff[BUFF_SIZE] = {0};

	memset(&buff, 0, sizeof(buff));
	length = vscnprintf(buff, BUFF_SIZE - 1, fmt, args);
	if (length > 0) {
		length ++;
		pr_info("%s: id = %d length = %d buff = %s\n", __func__, id, length, buff);
	}

	if (BUFF_SIZE <= length) // or < ?
	{
		pr_err("%s: string too long!\n", __func__);
		return -ENAMETOOLONG;
	}

	mutex_lock(&power_monitor_mutex);
	pm_info[id].idx = id;
	pm_info[id].len = length;
	mutex_unlock(&power_monitor_mutex);
	switch (id) {
	case AP_SLEEP:
                if (pm_info[id].buffer != NULL)
                {
                        memset(pm_info[id].buffer,'\0',sizeof(pm_info[id].buffer));
                        strlcpy(pm_info[id].buffer, buff, sizeof(pm_info[id].buffer));
                        pm_info[id].count = 1; 
                }

		break;
	case MODEM_SLEEP:
                if (pm_info[id].buffer != NULL)
                {
                        
                	memset(pm_info[id].buffer,'\0',sizeof(pm_info[id].buffer));
                        strlcpy(pm_info[id].buffer, buff, sizeof(pm_info[id].buffer));
                        pm_info[id].count = 1;
                }
		break;
        case IOM3_SLEEP:
                if (pm_info[id].buffer != NULL)
                {
                        memset(pm_info[id].buffer,'\0',sizeof(pm_info[id].buffer));
                        strlcpy(pm_info[id].buffer, buff, sizeof(pm_info[id].buffer));
                        pm_info[id].count = 1;
                }

                break;
	case SUSPEND_FAILED:
		if (strncmp(buff, "[suspend_total]", 15) == 0){
			suspend_total ++;
		}
		else if (strncmp(buff, "[error_dev_name]:", 17) == 0){
                    if (pm_info[id].buffer != NULL)
                    {
                        memset(pm_info[id].buffer,'\0',sizeof(pm_info[id].buffer));
                        strlcpy(pm_info[id].buffer, buff, sizeof(pm_info[id].buffer));
                    }
		}
		break;
	case FREEZING_FAILED:
                   if (pm_info[id].buffer != NULL) 
                   {
                        pm_info[id].count ++;
                        memset(pm_info[id].buffer,'\0',sizeof(pm_info[id].buffer));
                        strlcpy(pm_info[id].buffer, buff, sizeof(pm_info[id].buffer));
                   }
		break;
	case WAKEUP_ABNORMAL:
                if (pm_info[id].buffer != NULL)
                {
                        memset(pm_info[id].buffer,'\0',sizeof(pm_info[id].buffer));
                        strlcpy(pm_info[id].buffer, buff, sizeof(pm_info[id].buffer));
                }
		break;
	case DRIVER_ABNORMAL:
                if (pm_info[id].buffer != NULL)
                {
                        memset(pm_info[id].buffer,'\0',sizeof(pm_info[id].buffer));
                        strlcpy(pm_info[id].buffer, buff, sizeof(pm_info[id].buffer));
                }
		break;
        case WAKEUP_IRQ:
               if (pm_info[id].buffer != NULL)
               {
                        memset(pm_info[id].buffer,'\0',sizeof(pm_info[id].buffer));
                        strlcpy(pm_info[id].buffer, buff, sizeof(pm_info[id].buffer));
                        for(irq_number = 0;irq_number < IRQ_TOTAL;irq_number++)
                        {
                              if(!strcmp(dcurrent[irq_number].devname,pm_info[id].buffer))  
                              {
                                  dcurrent[irq_number].number ++;
                                  break;
                              }  
                  
                       }
                       
               }
               break;
        case ICC_VOTE:
               if (pm_info[id].buffer != NULL)
               {
                        memset(pm_info[id].buffer,'\0',sizeof(pm_info[id].buffer));
                        strlcpy(pm_info[id].buffer, buff, sizeof(pm_info[id].buffer));
               }
               break;
        case SENSORHUB_EVENT:
               if (pm_info[id].buffer != NULL)
               {
                        memset(pm_info[id].buffer,'\0',sizeof(pm_info[id].buffer));
                        strlcpy(pm_info[id].buffer, buff, sizeof(pm_info[id].buffer));
               }

               break;
        case SOC_VOTE:
               if (pm_info[id].buffer != NULL)
               {
                        memset(pm_info[id].buffer,'\0',sizeof(pm_info[id].buffer));
                        strlcpy(pm_info[id].buffer, buff, sizeof(pm_info[id].buffer));
               }

               break;

	default:
		break;
	}

	return 0;
}

/*
 * Function: power_monitor_report
 * Description: report data to power nodes
 * Input: id --- power radar nodes data struct
 *        fmt -- args from reported devices
 * Return: -x--failed, 0--success
 */
int power_monitor_report(unsigned int id, const char *fmt, ...)
{
	va_list args;
	int ret = -EINVAL;

	if (!is_id_valid(id)) {
		pr_err("%s: id %d is invalid!\n", __func__, id);
		return ret;
	}

	va_start(args, fmt);
	ret = report_handle(id, args, fmt);
	va_end(args);

	return ret;
}

EXPORT_SYMBOL(power_monitor_report);

static ssize_t ap_sleep_show(struct kobject *kobj,
				struct kobj_attribute *attr, char *buf)
{
	unsigned int id = AP_SLEEP;
        ssize_t ret = 0;
        pr_err(": %s \n", __func__);
        pr_err(": %s \n", pm_info[id].buffer);
        if (pm_info[id].buffer != NULL) return sprintf(buf, "%s", pm_info[id].buffer);
        return ret;
}

static ssize_t ap_sleep_store(struct kobject *kobj,
				struct kobj_attribute *attr,
				const char *buf, size_t n)
{
	unsigned int size = 0;
	unsigned int id = AP_SLEEP;

	if (sscanf(buf, "%d", &size) == 1) {
		pm_info[id].count = size;
		return n;
	}

	return -EINVAL;
}

power_attr(ap_sleep);


static ssize_t modem_sleep_show(struct kobject *kobj,
				struct kobj_attribute *attr, char *buf)
{
	unsigned int id = MODEM_SLEEP;
        ssize_t ret = 0;
        pr_err(": %s \n", __func__);
        pr_err(": %s \n", pm_info[id].buffer);
        if (pm_info[id].buffer != NULL) return sprintf(buf, "%s", pm_info[id].buffer);
        return ret;
}

static ssize_t modem_sleep_store(struct kobject *kobj,
				struct kobj_attribute *attr,
				const char *buf, size_t n)
{
	unsigned int size = 0;
	unsigned int id = MODEM_SLEEP;

	if (sscanf(buf, "%d", &size) == 1) {
		pm_info[id].count = size;
		return n;
	}

	return -EINVAL;
}

power_attr(modem_sleep);


static ssize_t iom3_sleep_show(struct kobject *kobj,
                                struct kobj_attribute *attr, char *buf)
{
        unsigned int id = IOM3_SLEEP;
        ssize_t ret = 0;
        pr_err(": %s \n", __func__);
        pr_err(": %s \n", pm_info[id].buffer);
        if (pm_info[id].buffer != NULL) return sprintf(buf, "%s", pm_info[id].buffer);
        return ret;
}

static ssize_t iom3_sleep_store(struct kobject *kobj,
                                struct kobj_attribute *attr,
                                const char *buf, size_t n)
{
        unsigned int size = 0;
        unsigned int id = IOM3_SLEEP;

        if (sscanf(buf, "%d", &size) == 1) {
                pm_info[id].count = size;
                return n;
        }

        return -EINVAL;
}

power_attr(iom3_sleep);

static ssize_t suspend_failed_show(struct kobject *kobj,
				struct kobj_attribute *attr, char *buf)
{
	unsigned int id = SUSPEND_FAILED;
        ssize_t ret = sprintf(buf, "%d", pm_info[id].count);
        pr_err(": %s \n", __func__);
        pr_err(": %s \n", pm_info[id].buffer);
        pm_info[id].count = 0;
        return ret;
}

static ssize_t suspend_failed_store(struct kobject *kobj,
				struct kobj_attribute *attr,
				const char *buf, size_t n)
{
	unsigned int size = 0;
	unsigned int id = SUSPEND_FAILED;

	if (sscanf(buf, "%d", &size) == 1) {
		pm_info[id].count = size;
		return n;
	}

	return -EINVAL;
}

power_attr(suspend_failed);

static ssize_t freezing_failed_show(struct kobject *kobj,
				struct kobj_attribute *attr, char *buf)
{
	unsigned int id = FREEZING_FAILED;
        ssize_t ret = 0;        
        
        pr_err(": %s \n", __func__);
        pr_err(": %s \n", pm_info[id].buffer);
        if (pm_info[id].buffer != NULL) 
        {
            ret = sprintf(buf, "freezing_failed:%s,times:%d",pm_info[id].buffer,pm_info[id].count);
            pm_info[id].count = 0;
        }
        return ret;
}

static ssize_t freezing_failed_store(struct kobject *kobj,
				struct kobj_attribute *attr,
				const char *buf, size_t n)
{
	unsigned int size = 0;
	unsigned int id = FREEZING_FAILED;

	if (sscanf(buf, "%d", &size) == 1) {
		pm_info[id].count = size;
		return n;
	}

	return -EINVAL;
}

power_attr(freezing_failed);

static ssize_t wakeup_abnormal_show(struct kobject *kobj,
				struct kobj_attribute *attr, char *buf)
{
	unsigned int id = WAKEUP_IRQ;

        pr_err(": %s \n", __func__);
        pr_err(": %s \n", pm_info[id].buffer);
	return sprintf(buf, "wakeupirq:%s", pm_info[id].buffer);
}

static ssize_t wakeup_abnormal_store(struct kobject *kobj,
				struct kobj_attribute *attr,
				const char *buf, size_t n)
{
	unsigned int size = 0;
	unsigned int id = WAKEUP_IRQ;

	if (sscanf(buf, "%d", &size) == 1) {
		pm_info[id].count = size;
		return n;
	}

	return -EINVAL;
}

power_attr(wakeup_abnormal);

static ssize_t driver_abnormal_show(struct kobject *kobj,
				struct kobj_attribute *attr, char *buf)
{
	unsigned int id = DRIVER_ABNORMAL;

        pr_err(": %s \n", __func__);
        pr_err(": %s \n", pm_info[id].buffer);
	return sprintf(buf, "%d", pm_info[id].count);
}

static ssize_t driver_abnormal_store(struct kobject *kobj,
				struct kobj_attribute *attr,
				const char *buf, size_t n)
{
	unsigned int size = 0;
	unsigned int id = DRIVER_ABNORMAL;

	if (sscanf(buf, "%d", &size) == 1) {
		pm_info[id].count = size;
		return n;
	}

	return -EINVAL;
}

power_attr(driver_abnormal);

static ssize_t wakeup_irq_show(struct kobject *kobj,
                                struct kobj_attribute *attr, char *buf)
{
        char irq_buffer[130]={0};
        char show_irq_buffer[256]={0};
        unsigned int id = WAKEUP_IRQ;
        ssize_t ret = 0;
        int irq_number = 0; 
        int irq_buffer_length = 0;
        int show_irq_buffer_length = 0;

        memset(&irq_buffer, 0, sizeof(irq_buffer));
        memset(&show_irq_buffer, 0, sizeof(show_irq_buffer));

        for(irq_number = 0;irq_number < IRQ_TOTAL;irq_number++)
        {
            if (dcurrent[irq_number].number > 0)
            {    
                   sprintf(irq_buffer, "%s:%d \n",dcurrent[irq_number].devname,dcurrent[irq_number].number);
                   irq_buffer_length = strlen(irq_buffer);
                   show_irq_buffer_length = strlen(show_irq_buffer);
                   if((irq_buffer_length+show_irq_buffer_length) < 255) strcat(show_irq_buffer,irq_buffer);
                   dcurrent[irq_number].number = 0;
                   memset(&irq_buffer, 0, sizeof(irq_buffer));
            }
        }
        
        if (show_irq_buffer != NULL)  return sprintf(buf, "wakeup irq: %s",show_irq_buffer);
        else return ret;  

}

static ssize_t wakeup_irq_store(struct kobject *kobj,
                                struct kobj_attribute *attr,
                                const char *buf, size_t n)
{
        unsigned int size = 0;
        unsigned int id = WAKEUP_IRQ;

        if (sscanf(buf, "%d", &size) == 1) {
                pm_info[id].count = size;
                return n;
        }

        return -EINVAL;
}

power_attr(wakeup_irq);


static ssize_t icc_vote_show(struct kobject *kobj,
                                struct kobj_attribute *attr, char *buf)
{
        unsigned int id = ICC_VOTE;
        ssize_t ret = 0;
        if (pm_info[id].buffer != NULL) ret = sprintf(buf, "icc_vote:%s", pm_info[id].buffer);
	return ret;

}

static ssize_t icc_vote_store(struct kobject *kobj,
                                struct kobj_attribute *attr,
                                const char *buf, size_t n)
{
        unsigned int size = 0;
        unsigned int id = ICC_VOTE;

        if (sscanf(buf, "%d", &size) == 1) {
                pm_info[id].count = size;
                return n;
        }

        return -EINVAL;
}

power_attr(icc_vote);

static ssize_t sensorhub_event_show(struct kobject *kobj,
                                struct kobj_attribute *attr, char *buf)
{
        unsigned int id = SENSORHUB_EVENT;
        ssize_t ret = 0;
        if (pm_info[id].buffer != NULL) return sprintf(buf, "sensorhub event:%s", pm_info[id].buffer);
	return ret;

}

static ssize_t sensorhub_event_store(struct kobject *kobj,
                                struct kobj_attribute *attr,
                                const char *buf, size_t n)
{
        unsigned int size = 0;
        unsigned int id = SENSORHUB_EVENT;

        if (sscanf(buf, "%d", &size) == 1) {
                pm_info[id].count = size;
                return n;
        }

        return -EINVAL;
}

power_attr(sensorhub_event);


static ssize_t soc_vote_show(struct kobject *kobj,
                                struct kobj_attribute *attr, char *buf)
{
        unsigned int id = SOC_VOTE;
        ssize_t ret = 0;
        if (pm_info[id].buffer != NULL) return sprintf(buf, "soc vote:%s", pm_info[id].buffer);
        return ret;

}

static ssize_t soc_vote_store(struct kobject *kobj,
                                struct kobj_attribute *attr,
                                const char *buf, size_t n)
{
        unsigned int size = 0;
        unsigned int id = SOC_VOTE;

        if (sscanf(buf, "%d", &size) == 1) {
                pm_info[id].count = size;
                return n;
        }

        return -EINVAL;
}

power_attr(soc_vote);


static struct attribute * monitor_attrs[] = {
	&ap_sleep_attr.attr,
	&modem_sleep_attr.attr,
        &iom3_sleep_attr.attr,
	&suspend_failed_attr.attr,
	&freezing_failed_attr.attr,
	&wakeup_abnormal_attr.attr,
	&driver_abnormal_attr.attr,
        &wakeup_irq_attr.attr,
        &icc_vote_attr.attr,
        &sensorhub_event_attr.attr,
        &soc_vote_attr.attr,
	NULL,
};

static struct attribute_group monitor_attr_group = {
	.name = "monitor", /* Directory of power monitor */
	.attrs = monitor_attrs,
};

static int __init power_monitor_init(void)
{
	int ret = -1;
	int i = 0, length = 0;

	/* power_kobj is created in kernel/power/main.c */
	if (!power_kobj){
		pr_err("%s: power_kobj is null!\n", __func__);
		return -ENOMEM;
	}

	/* Initialized struct data */
	length = sizeof(struct power_monitor_info);
	for (i = 0; i < POWER_MONITOR_MAX; i++) {
		memset(&pm_info[i], 0, length);
                pm_info[i].count = 0;
	}

	/* create all nodes under power sysfs */
	ret = sysfs_create_group(power_kobj, &monitor_attr_group);
	if (ret < 0) {
		pr_err("%s: sysfs_create_group power_kobj error\n", __func__);
	} else {
		pr_info("%s: sysfs_create_group power_kobj success\n", __func__);
	}

	return ret;
}

core_initcall(power_monitor_init);




