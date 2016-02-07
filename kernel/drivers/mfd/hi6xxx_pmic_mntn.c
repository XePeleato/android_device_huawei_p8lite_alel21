/*
 * Device driver for PMU DRIVER	 in HI6xxx SOC
 *
 * Copyright (c) 2011 Hisilicon Co. Ltd
 *
 * Dongbin Yu <fansaihua@huawei.com> : 2014-1-26
 *
 * In V8R1 the pmic is same with V9R1's driver,so it is multiplex.
 * That the reason why name of the c document is "hi6xxx".
 *
 * I only code the pmic arch,there are some thing which not finished because
 * of the litter time.In process of iterative development you must add some
 * function into the module.
 *
 *
 */
#include "hi6xxx_pmic_mntn_inner.h"
#include <linux/hisi/hi6xxx_mntn.h> /*BSP_MODU_PMU*/

#if defined (CONFIG_HUAWEI_DSM)
#include <huawei_platform/dsm/dsm_pub.h>

static struct dsm_dev dsm_pmu_ocp = {
	.name = "dsm_pmu_ocp",
	.device_name = NULL,
	.ic_name = NULL,
	.module_name = NULL,
	.fops = NULL,
	.buff_size = 1024,
};
static struct dsm_client *pmu_ocp_dclient = NULL;
#endif


static PMIC_MNTN_DESC* g_pmic_mntn = NULL;

static void pmic_dump_file(char *pstr)
{
    int     ilen = 0;
    unsigned long fs;
    long    fd = 0;
    long    ret = 0;

    if (NULL == pstr)
    {
        pr_err("%s: pstr is NULL\n",__func__);
        return ;
    }

    ilen = strlen(pstr);
    if(ilen<=0)
    {
       pr_err("%s, info len is zero.\n",__func__);
       return ;
    }

    fs = get_fs();
    set_fs(KERNEL_DS);
    fd = sys_open(PMIC_DUMP_FILE_PATH, O_CREAT|O_WRONLY|O_APPEND, 0664);
    if(fd == -ENOENT)
    {
       pr_info("%s,%s, fd is %ld Filesystem is not init ok Ignored.\n",
       __func__,PMIC_DUMP_FILE_PATH,fd);
       set_fs(fs);
       return ;
    }
    else if(fd < 0)
    {
       pr_err("%s:sys_open op fail, filename:%s, fd is %ld \n",
       __func__,PMIC_DUMP_FILE_PATH,fd);
       set_fs(fs);
       return ;
    }

    ret= sys_lseek(fd,0,SEEK_END);
    if(ret<0)
    {
        pr_err("%s :the sys_lseek ops is fail,the ret is %ld\n",__func__,ret);
        set_fs(fs);
        return ;
    }
    else if (ret>PMU_DUMP_FILE_MAX_SIZE)
    {
        (void)sys_unlink(PMIC_DUMP_FILE_PATH);
        fd = sys_open(PMIC_DUMP_FILE_PATH, O_CREAT|O_WRONLY|O_APPEND,0);
        if(fd == -ENOENT)
        {
           pr_info("%s,%s, fd is %ld Filesystem is not init ok Ignored.\n",
           __func__,PMIC_DUMP_FILE_PATH,fd);
           set_fs(fs);
           return ;
        }
        else if(fd < 0)
        {
           pr_err("%s:sys_open op fail, filename:%s, fd is %ld \n",
           __func__,PMIC_DUMP_FILE_PATH,fd);
           set_fs(fs);
           return ;
        }
    }

    ret = sys_write(fd,pstr,ilen);
    if(ret < 0)
    {
        pr_err("%s:the sys_write ops fail,the ret is %ld\n",__func__,ret);
        set_fs(fs);
        return ;
    }

    ret = sys_fsync(fd);
    if(ret < 0)
    {
        pr_err("%s:sys_fsync fail,the ret is %ld\n",__func__,ret);
    }

    ret = sys_close(fd);
    if(ret < 0)
    {
        pr_err("%s:the sys_close ops fail, the ret is %ld\n",__func__,ret);
        set_fs(fs);
        return ;
    }

    set_fs(fs);
}

static void pmic_show_cur_time(void)
{
    struct timespec now;
    struct timespec uptime;
    struct rtc_time rtc_timestamp;
    char printbuf[512]={'\0'};

    now = current_kernel_time();
    rtc_time_to_tm((unsigned long)now.tv_sec, &rtc_timestamp);
    do_posix_clock_monotonic_gettime(&uptime);

    snprintf(printbuf,512,"\n PMU LOGMESSAGE Timestamp = %lu.%03lu\n",
            (unsigned long)now.tv_sec,
            (unsigned long)(now.tv_nsec / 1000000));
    snprintf(printbuf+strlen(printbuf),512,"Current Time = "
            "%02d-%02d %02d:%02d:%lu.%03lu, "
            "Uptime = %lu.%03lu seconds\n",
            rtc_timestamp.tm_mon + 1, rtc_timestamp.tm_mday,
            rtc_timestamp.tm_hour, rtc_timestamp.tm_min,
            (unsigned long)rtc_timestamp.tm_sec,
            (unsigned long)(now.tv_nsec / 1000000),
            (unsigned long)uptime.tv_sec,
            (unsigned long)(uptime.tv_nsec/USEC_PER_SEC));

    pmic_dump_file(printbuf);
}

static int  of_get_exch_reg_attr(PMIC_MNTN_DESC* pmic_mntn,struct device *dev,
     struct device_node *np,struct pmic_regs* exch_regs,PMIC_EXCH_REG* exch_reg_desc)
{
    unsigned int *inacceptable_event = NULL;
    unsigned int *dmd_not_report_event = NULL;
    unsigned int reg_id = 0;
    PMIC_EXCH_REG* cur_exch_reg_desc = exch_reg_desc;
    unsigned int start_addr = exch_regs->start_addr;
    unsigned char bit_id = 0;
    char np_compatible_string[64]={0};

    for(reg_id = 0;reg_id<exch_regs->size;reg_id++,cur_exch_reg_desc++)
    {
        cur_exch_reg_desc->reg_addr = start_addr + reg_id;
        snprintf(np_compatible_string,60,"hisilicon,hi6552-pmic-mntn-event0x%0x",cur_exch_reg_desc->reg_addr);
        cur_exch_reg_desc->exch_reg_node = of_find_compatible_node(pmic_mntn->np,NULL,np_compatible_string);
        if(!cur_exch_reg_desc->exch_reg_node)
        {
            continue;
        }

#ifdef FINAL_RELEASE_MODE
        inacceptable_event = (unsigned int *)of_get_property(cur_exch_reg_desc->exch_reg_node,
            "hisilicon,inacceptable-event-final-release",NULL);
#else
        inacceptable_event = (unsigned int *)of_get_property(cur_exch_reg_desc->exch_reg_node,
            "hisilicon,inacceptable-event",NULL);
#endif
        if(inacceptable_event)
        {
            cur_exch_reg_desc->inacceptable_event = be32_to_cpu(*inacceptable_event);
        }
        else
        {
            dev_err(dev, "inacceptable_event, sorry, inacceptable_event is NULL!\n");
        }

        dmd_not_report_event = (unsigned int *)of_get_property(cur_exch_reg_desc->exch_reg_node,
        "hisilicon,dmd-not-report-event",NULL);
        if(dmd_not_report_event)
        {
            cur_exch_reg_desc->dmd_not_report_event = be32_to_cpu(*dmd_not_report_event);
        }
        else
        {
            dev_err(dev, "dmd_not_report_event, sorry, dmd_not_report_event is NULL\n");
        }

        cur_exch_reg_desc->event_bit_name =(char**)devm_kzalloc(dev,pmic_mntn->data_width*sizeof(char*),GFP_KERNEL);
        if(NULL == cur_exch_reg_desc->event_bit_name)
        {
            dev_err(dev,"%s devm_kzalloc event_bi_name err\n",__func__);
            return -ENOMEM;
        }

        for(bit_id = 0;bit_id < pmic_mntn->data_width;bit_id++)
        {
            (void)of_property_read_string_index(cur_exch_reg_desc->exch_reg_node,
            "hisilicon,event-bit-name",bit_id,
            (const char **)&cur_exch_reg_desc->event_bit_name[bit_id]);
        }

    }

    return 0;
}

static int of_get_pmic_smpl_attr(PMIC_MNTN_DESC* pmic_mntn, struct device *dev, struct device_node *np)
{
    PMIC_SMPL_DESC *cur_smpl_desc = pmic_mntn->smpl_desc;
    struct device_node *root = NULL;

    root = of_find_compatible_node(NULL, NULL, "hisilicon,hi6552-pmic-mntn-smpl");
    if (!root)
    {
        dev_err(dev,"%s pmic-mntn-smpl no root node\n",__func__);
        return 0;
    }

    of_property_read_u32_array(root, "hisilicon,smpl-ctrl-en", &cur_smpl_desc->enable, 0x1);

    of_property_read_u32_array(root, "hisilicon,smpl-ctrl-time", &cur_smpl_desc->time_cfg, 0x1);

    of_property_read_u32_array(root, "hisilicon,smpl-ctrl-en-reg", (u32 *)&cur_smpl_desc->en_ctrl, 0x3);

    of_property_read_u32_array(root, "hisilicon,smpl-ctrl-time-reg", (u32 *)&cur_smpl_desc->time_ctrl, 0x3);

    of_property_read_u32_array(root, "hisilicon,smpl-ctrl-record-reg", (u32 *)&cur_smpl_desc->record_ctrl, 0x3);

    return 0;
}

static int of_get_pmic_attr(PMIC_MNTN_DESC* pmic_mntn, struct device *dev,
                         struct device_node *np)
{
    unsigned int *data_width = NULL;
    unsigned int *otmp_set = NULL;

    int ret = 0;

    data_width = (unsigned int *)of_get_property(np, "hisilicon,data-width",NULL);
    pmic_mntn->data_width  = PMIC_REGISTER_DATA_WIDTH;
    if (data_width&&(*data_width))
    {
        pmic_mntn->data_width = be32_to_cpu(*data_width);
    }


    otmp_set = (unsigned int *)of_get_property(np, "hisilicon,otmp-set", NULL);
    pmic_mntn->otmp_set  = 0x02;
    if(otmp_set)
    {
        pmic_mntn->otmp_set = be32_to_cpu(*otmp_set);
    }

    (void)of_property_read_u32_array(np,"hisilicon,np-regs", (unsigned int *)(&pmic_mntn->np_regs),0x2);
    (void)of_property_read_u32_array(np,"hisilicon,irq-regs", (unsigned int *)(&pmic_mntn->irq_regs),0x2);
    (void)of_property_read_u32_array(np,"hisilicon,record-regs", (unsigned int *)(&pmic_mntn->record_regs),0x2);

    pmic_mntn->np_regs.size = (pmic_mntn->np_regs.size > NP_MAX_SIZE)? NP_MAX_SIZE : pmic_mntn->np_regs.size;
    pmic_mntn->irq_regs.size = (pmic_mntn->irq_regs.size > IRQ_MAX_SIZE)? IRQ_MAX_SIZE :pmic_mntn->irq_regs.size;
    pmic_mntn->record_regs.size = (pmic_mntn->record_regs.size > RECORD_MAX_SIZE)? RECORD_MAX_SIZE :pmic_mntn->record_regs.size;

    pmic_mntn->exch_reg_size = pmic_mntn->np_regs.size + pmic_mntn->irq_regs.size + pmic_mntn->record_regs.size;
    pmic_mntn->exch_reg_desc = (PMIC_EXCH_REG*)devm_kzalloc(dev,pmic_mntn->exch_reg_size * sizeof(PMIC_EXCH_REG),GFP_KERNEL);
    if(NULL == pmic_mntn->exch_reg_desc)
    {
        dev_err(dev,"%s devm_kzalloc exch_reg_desc err\n",__func__);
        return -ENOMEM;
    }

    ret = of_get_exch_reg_attr(pmic_mntn,dev,np,&pmic_mntn->np_regs,pmic_mntn->exch_reg_desc);
    if(ret)
    {
        dev_err(dev,"%s of_get_exch_reg_attr 0x%0x err\n",__func__,pmic_mntn->np_regs.start_addr);
        return ret;
    }

    ret = of_get_exch_reg_attr(pmic_mntn,dev,np,&pmic_mntn->irq_regs,
          pmic_mntn->exch_reg_desc+pmic_mntn->np_regs.size);
    if(ret)
    {
        dev_err(dev,"%s of_get_exch_reg_attr 0x%0x err\n",__func__,pmic_mntn->irq_regs.start_addr);
        return ret;
    }

    ret = of_get_exch_reg_attr(pmic_mntn,dev,np,&pmic_mntn->record_regs,
          pmic_mntn->exch_reg_desc+pmic_mntn->np_regs.size+pmic_mntn->irq_regs.size);
    if(ret)
    {
        dev_err(dev,"%s of_get_exch_reg_attr 0x%0x err\n",__func__,pmic_mntn->record_regs.start_addr);
        return ret;
    }

    pmic_mntn->smpl_desc = (PMIC_SMPL_DESC *)devm_kzalloc(dev,sizeof(PMIC_SMPL_DESC),GFP_KERNEL);
    if(NULL == pmic_mntn->smpl_desc)
    {
        dev_err(dev,"%s devm_kzalloc smpl_desc err\n",__func__);
        return -ENOMEM;
    }

    ret = of_get_pmic_smpl_attr(pmic_mntn, dev, np);
    if(ret)
    {
        dev_err(dev,"%s of_get_pmic_smpl_attr err\n",__func__);
        return ret;
    }

    return 0;
}

static void pmic_mntn_reg_dump(PMIC_MNTN_DESC* pmic_mntn)
{
    unsigned int i = 0;
    unsigned int dump_start_addr = pmic_mntn->np_regs.start_addr;

    memset((void*)pmic_mntn->init_log_show,'\0',PMIC_PRINT_BUF_SIZE);
    sprintf(pmic_mntn->init_log_show,"\nPMIC REGISTER DUMP\n");
    strcat(pmic_mntn->init_log_show,"  addr    data \n");

    for(i = 0;i<pmic_mntn->np_regs.size;i++)
    {
        snprintf(pmic_mntn->init_log_show+strlen(pmic_mntn->init_log_show),PMIC_PRINT_BUF_SIZE,
        "  0x%0x    0x%0x \n",(dump_start_addr+i),pmic_mntn->np_data_buf[i]);
    }
    pr_err("%s",pmic_mntn->init_log_show);

    dump_start_addr = pmic_mntn->irq_regs.start_addr;
    for(i = 0;i<pmic_mntn->irq_regs.size;i++)
    {
        snprintf(pmic_mntn->init_log_show+strlen(pmic_mntn->init_log_show),PMIC_PRINT_BUF_SIZE,
        "  0x%0x    0x%0x \n",(dump_start_addr+i),pmic_mntn->irq_data_buf[i]);
    }

    dump_start_addr = pmic_mntn->record_regs.start_addr;
    for(i = 0;i<pmic_mntn->record_regs.size;i++)
    {
        snprintf(pmic_mntn->init_log_show+strlen(pmic_mntn->init_log_show),PMIC_PRINT_BUF_SIZE,
        "  0x%0x    0x%0x \n",(dump_start_addr+i),pmic_mntn->record_data_buf[i]);
    }

    snprintf(pmic_mntn->init_log_show+strlen(pmic_mntn->init_log_show),PMIC_PRINT_BUF_SIZE,
    "  0x%0x    0x%0x \n",pmic_mntn->smpl_desc->record_ctrl.reg_addr,pmic_mntn->smpl_desc->last_record);

}

static void pmic_exch_scan(PMIC_MNTN_DESC* pmic_mntn,unsigned int reg_addr,unsigned int reg_data,int scene )
{
    unsigned int id= 0;
    PMIC_EXCH_REG* exch_reg_desc =pmic_mntn->exch_reg_desc;
    unsigned int bit = 0;
    unsigned care_bit = 0;
    char* circuit_name = NULL;
    char* cur_str = NULL;

    pmic_mntn->dmd_event = 0;
    if( PMIC_HEALTH_SMPL_EXCH_HAPPENED == scene )
    {
        cur_str = pmic_mntn->init_log_show;

        snprintf(cur_str+strlen(cur_str),PMIC_PRINT_BUF_SIZE,"PMU EVENT TYPE:smpl_reboot\n");

        pmic_mntn->health |= BIT(scene);

        return;
    }

    for(id = 0;id<pmic_mntn->exch_reg_size;id++,exch_reg_desc++)
    {
        if(exch_reg_desc->reg_addr == reg_addr)
        {
            break;
        }
    }

    if(id == pmic_mntn->exch_reg_size)
    {
        pr_err("%s reg_addr err 0x%0x",__func__,reg_addr);
        return ;
    }

    for (bit = 0;bit <pmic_mntn->data_width;bit++)
    {
        care_bit = reg_data&BIT(bit);
        if(care_bit)
        {
            circuit_name = *(exch_reg_desc->event_bit_name+bit);
            if(circuit_name)
            {
                if(PMIC_HEALTH_NP_EXCH_HAPPENED == scene)
                {
                    cur_str = pmic_mntn->init_log_show;
                }
                else if (PMIC_HEALTH_IRQ_EXCH_HAPPENED == scene)
                {
                    cur_str = pmic_mntn->irq_log_show;
                }

                snprintf(cur_str+strlen(cur_str),
                PMIC_PRINT_BUF_SIZE,"PMU EVENT TYPE:%s\n",circuit_name);
            }

            if(care_bit&exch_reg_desc->inacceptable_event)
            {
                pmic_mntn->health |= BIT(scene);
            }

            if(care_bit & (~exch_reg_desc->dmd_not_report_event))
            {
                pmic_mntn->dmd_event = (unsigned char)BIT(0);
            }
        }
    }
}

static void pmic_mntn_exch_dect(PMIC_MNTN_DESC* pmic_mntn)
{
    unsigned int i = 0;
    char smpl_flag = 0;
    unsigned int np_start_addr = pmic_mntn->np_regs.start_addr;
    PMIC_SMPL_DESC *cur_smpl_desc = pmic_mntn->smpl_desc;
    for(i = 0;i< pmic_mntn->np_regs.size;i++)
    {
        if(0 != pmic_mntn->np_data_buf[i])
        {
            pmic_exch_scan(pmic_mntn,(np_start_addr+i),pmic_mntn->np_data_buf[i],PMIC_HEALTH_NP_EXCH_HAPPENED);
        }
    }

    smpl_flag = ((cur_smpl_desc->last_record >> cur_smpl_desc->record_ctrl.shift) & 0x1);
    if(1 == smpl_flag)
    {
        pmic_exch_scan(pmic_mntn, 0, 0, PMIC_HEALTH_SMPL_EXCH_HAPPENED);
    }
}

static void pmic_mntn_smpl_config(PMIC_MNTN_DESC* pmic_mntn)
{
    PMIC_SMPL_DESC *cur_smpl_desc = pmic_mntn->smpl_desc;
    unsigned char reg_value = 0;

    reg_value = hi6xxx_pmic_reg_read(cur_smpl_desc->en_ctrl.reg_addr);
    SET_REG_BIT(reg_value, cur_smpl_desc->en_ctrl.shift, cur_smpl_desc->en_ctrl.mask, cur_smpl_desc->enable);
    hi6xxx_pmic_reg_write(cur_smpl_desc->en_ctrl.reg_addr, reg_value);

    reg_value = hi6xxx_pmic_reg_read(cur_smpl_desc->time_ctrl.reg_addr);
    SET_REG_BIT(reg_value, cur_smpl_desc->time_ctrl.shift, cur_smpl_desc->time_ctrl.mask, cur_smpl_desc->time_cfg);
    hi6xxx_pmic_reg_write(cur_smpl_desc->time_ctrl.reg_addr, reg_value);

    return;
}

static struct record_2_disable_reg re2dis[] =
{
    {SOC_SMART_SCP_BUCK_RECORD1_ADDR(0),SOC_SMART_DISABLE1_BUCK_ADDR(0)},
    {SOC_SMART_OCP_BUCK_RECORD1_ADDR(0),SOC_SMART_DISABLE1_BUCK_ADDR(0)},
    {SOC_SMART_OCP_LDO1_8_RECORD2_ADDR(0),SOC_SMART_DISABLE2_LDO1_8_ADDR(0)},
    {SOC_SMART_OCP_LDO16_9_RECORD3_ADDR(0),SOC_SMART_DISABLE3_LDO9_16_ADDR(0)},
    {SOC_SMART_OCP_LDO22_17_RECORD4_ADDR(0),SOC_SMART_DISABLE4_LDO17_22_ADDR(0)},
    /*{SOC_SMART_OCP_CLASSD_LVS2_RECORD5_ADDR(0),SOC_SMART_DISABLE5_LVS2_ADDR(0)}, 特殊处理*/
};

static void pmic_socp_clr_record(unsigned int record_reg,unsigned char record_data)
{
    unsigned int record_size = sizeof(re2dis)/sizeof(re2dis[0]);
    unsigned int id = 0;

    for(id = 0;id< record_size;id++)
    {
        if(record_reg == re2dis[id].record_reg)
        {
            /*close circuit*/
            hi6xxx_pmic_reg_write(re2dis[id].dis_reg,record_data);
            break;
        }
    }

    if(SOC_SMART_OCP_CLASSD_LVS2_RECORD5_ADDR(0) == record_reg)
    {
        SOC_SMART_OCP_CLASSD_LVS2_RECORD5_UNION classd_lvs2_record_data ;
        classd_lvs2_record_data.value = record_data;
        if(classd_lvs2_record_data.reg.ocp_classd)
        {
            unsigned char classd_ctrl3_data = 0;
            classd_ctrl3_data = hi6xxx_pmic_reg_read(SOC_SMART_CLASSD_CTRL3_ADDR(0));
            classd_ctrl3_data &= ~(BIT(SOC_SMART_CLASSD_CTRL3_en_classd_int_START));
            hi6xxx_pmic_reg_write(SOC_SMART_CLASSD_CTRL3_ADDR(0),classd_ctrl3_data);
        }

        if(classd_lvs2_record_data.reg.ocp_lvs2)
        {
            SOC_SMART_DISABLE5_LVS2_UNION dis_lvs2;
            dis_lvs2.value = 0;
            dis_lvs2.reg.dis_lvs2_int = 1;
            hi6xxx_pmic_reg_write(SOC_SMART_DISABLE5_LVS2_ADDR(0),dis_lvs2.value);
        }
    }

    hi6xxx_pmic_reg_write(record_reg,record_data);
}

static irqreturn_t pmic_ocp_scp_irq_hander(int irq, void *data)
{
    struct irq_desc *desc = NULL ;
    PMIC_MNTN_DESC  *pmu_mntn = (PMIC_MNTN_DESC*)data;

    desc = irq_to_desc(irq);
    if (!desc)
    {
        pr_err("%s irq_to_desc failed\n",__func__);
        return IRQ_NONE;
    }

    if(NULL != desc->irq_data.chip->irq_mask)
    {
        desc->irq_data.chip->irq_mask(&desc->irq_data);
    }

    queue_work(pmu_mntn->ocp_scp_wq, &pmu_mntn->ocp_scp_wk);
    return IRQ_HANDLED;
}

/*
* According to dsm_pub.h, PMU_OCP dsm num space is 100: 10100~10200 and the base num is 10100.
* Here the offset num is made up of two decimal digits XY: X is the reg num and Y is the reg bit num, XY is X*10 + Y.
* So, if reg num or reg bit num is more than 9, we cannot differentiate them, and this func will return 0. 
* Notice:Even if there are more than one ocps, this func can noly report one.
*/
static unsigned int pmic_get_ocp_scp_dsm_offset_num(unsigned int reg_id, unsigned char reg_data, unsigned int data_width)
{
#define MAX_REG_NUM (9)
#define MAX_BIT_NUM (9)
    unsigned int offset_num = 0, bit_num = 0;

    if(reg_id > MAX_REG_NUM)
        return 0;

    if(reg_data)
    {
        for(bit_num = 0; bit_num < data_width; bit_num++)
        {
            if(bit_num > MAX_BIT_NUM)
                return 0;
            if((1UL << bit_num) & reg_data)
            {
                offset_num = reg_id*10 + bit_num;
                break;
            }
        }
    }
    return offset_num;
}

static void pmic_ocp_scp_wq_hander(struct work_struct *work)
{
#if defined (CONFIG_HUAWEI_DSM)
    int dsm_error_found = -1;
#endif

    unsigned int reg_id = 0;
    volatile unsigned char socp_deb_data = 0;
    volatile unsigned char record_data = 0;
    struct irq_desc *desc = NULL;
    PMIC_MNTN_DESC* pmic_mntn = NULL;
    unsigned int cur_reg_addr = 0;
    unsigned int dsm_offset_num = 0;

 #if defined (CONFIG_HUAWEI_DSM)
	if (!dsm_client_ocuppy(pmu_ocp_dclient)) {
		dsm_error_found++;
	}
#endif

    pmic_mntn = container_of(work,PMIC_MNTN_DESC,ocp_scp_wk);
    socp_deb_data = hi6xxx_pmic_reg_read(SOC_SMART_OCP_DEB0_ADDR(0));

    pmic_show_cur_time();
    memset((void*)pmic_mntn->irq_log_show,'\0',PMIC_PRINT_BUF_SIZE);
    sprintf(pmic_mntn->irq_log_show,"PMIC SOCP DEB: 0x%x\n",socp_deb_data);

    for(reg_id = 0;reg_id < pmic_mntn->record_regs.size;reg_id++)
    {
        cur_reg_addr = pmic_mntn->record_regs.start_addr+reg_id;
        record_data = hi6xxx_pmic_reg_read(cur_reg_addr);
        if(record_data)
        {
            pmic_exch_scan(pmic_mntn,cur_reg_addr,record_data,PMIC_HEALTH_IRQ_EXCH_HAPPENED);
            pmic_socp_clr_record(cur_reg_addr,record_data);

#if defined (CONFIG_HUAWEI_DSM)
            if (pmic_mntn->dmd_event && (dsm_error_found >= 0)) {
                dsm_client_record(pmu_ocp_dclient, "PMU OCP DEB: 0x%x.\nPMU OCP SCP:addr\
                0x%x.\tdata 0x%x.\n", socp_deb_data, cur_reg_addr, record_data);
                dsm_error_found++;
                dsm_offset_num = pmic_get_ocp_scp_dsm_offset_num(reg_id, record_data, pmic_mntn->data_width);
            }
#endif
        }
    }

    pr_info("\n%s\n",pmic_mntn->irq_log_show);
    pmic_dump_file(pmic_mntn->irq_log_show);

    desc = irq_to_desc(pmic_mntn->ocp_scp_irq);
    if(!desc)
    {
        pr_err("%s irq_to_desc(pmic_mntn->ocp_scp_irq) err\n",__func__);
        return ;
    }

    if(NULL != desc->irq_data.chip->irq_unmask)
    {
        desc->irq_data.chip->irq_unmask(&desc->irq_data);
    }

    if(pmic_mntn->health&BIT(PMIC_HEALTH_IRQ_EXCH_HAPPENED))
    {
       systemError(BSP_MODU_PMU,EXCH_S_PMU,0,pmic_mntn->irq_log_show,strlen(pmic_mntn->irq_log_show));
    }

#if defined (CONFIG_HUAWEI_DSM)
	if (dsm_error_found > 0) {
		dsm_client_notify(pmu_ocp_dclient, DSM_PMU_OCP_ERROR_NO_BASE+dsm_offset_num);
	} else if (!dsm_error_found) {
		dsm_client_unocuppy(pmu_ocp_dclient);
	} else {
	/* dsm pmu_ocp_dclient ocuppy failed */
	}
#endif

}

static irqreturn_t pmic_otmp_irq_hander(int irq, void *data)
{

    struct irq_desc *desc = NULL ;
    PMIC_MNTN_DESC  *pmu_mntn = (PMIC_MNTN_DESC*)data;

    desc = irq_to_desc(irq);
    if (!desc)
    {
        pr_err("%s irq_to_desc failed\n",__func__);
        return IRQ_NONE;
    }

    if(NULL != desc->irq_data.chip->irq_mask)
    {
        desc->irq_data.chip->irq_mask(&desc->irq_data);
    }

    queue_work(pmu_mntn->otmp_wq, &pmu_mntn->otmp_wk);
    return IRQ_HANDLED;
}

static void pmic_otmp_wk_hander(struct work_struct *work)
{
    unsigned char data = 0;
    struct irq_desc *desc = NULL;
    PMIC_MNTN_DESC  *pmic_mntn;
    pmic_mntn = container_of(work,PMIC_MNTN_DESC,otmp_wk);

    data = hi6xxx_pmic_reg_read(SOC_SMART_IRQ1_ADDR(0));
    memset((void*)pmic_mntn->irq_log_show,'\0',PMIC_PRINT_BUF_SIZE);
    sprintf(pmic_mntn->irq_log_show,
    "PMIC OTMP EVENT happen addr 0x%0x data 0x%0x\n",SOC_SMART_IRQ1_ADDR(0),data);
    pmic_show_cur_time();
    pr_info("\n%s\n",pmic_mntn->irq_log_show);
    pmic_dump_file(pmic_mntn->irq_log_show);

    hi6xxx_pmic_reg_write(SOC_SMART_IRQ1_ADDR(0),BIT(SOC_SMART_IRQ1_otmp_d1r_START));

    desc = irq_to_desc(pmic_mntn->otmp_irq);
    if(!desc)
    {
        pr_err("%s irq_to_desc err\n",__func__);
        return ;
    }

    if(NULL != desc->irq_data.chip->irq_unmask)
    {
        desc->irq_data.chip->irq_unmask(&desc->irq_data);
    }

}

static PMIC_DEV_INFO pmic_dev_info = {NULL,NULL};
int hi6xxx_pmic_device_stat_notify(char *dev_name, int dev_stat)
{
    struct pmic_dev_stat *head = pmic_dev_info.head;
    struct pmic_dev_stat *cur = pmic_dev_info.tail;
    int len = 0;

    if(NULL == dev_name)
    {
        pr_err("%s dev_name is null\n",__func__);
        return -1;
    }

    len = strlen(dev_name);
    if( PMIC_DEV_NAME_LEN <= len)
    {
        pr_err("%s dev_name is too long\n",__func__);
        return -1;
    }

    if( (SSI_DEVICE_ERR != dev_stat) && (SSI_DEVICE_OK != dev_stat) )
    {
        pr_err("%s dev_stat err : %d\n",__func__,dev_stat);
        return -1;
    }

    if(!head)
    {
        head = kzalloc(sizeof(struct pmic_dev_stat), GFP_KERNEL);
        cur  = head;
        pmic_dev_info.head = head;
    }
    else
    {
        cur->next = kzalloc(sizeof(struct pmic_dev_stat), GFP_KERNEL);
        cur = cur->next;
    }
    pmic_dev_info.tail = cur;

    memcpy((void *)cur->dev_name, (void *)dev_name, len);
    cur->dev_stat = dev_stat;

    return 0;
}

static ssize_t pmic_mntn_show(struct kobject *kobj, struct kobj_attribute *attr,
            char *buf)
{
    int health = 0;

    if(g_pmic_mntn)
    {
        health = !!g_pmic_mntn->health;
    }

    return sprintf(buf, "%d\n", health);
}

static ssize_t pmic_mntn_store(struct kobject *kobj, struct kobj_attribute *attr,
             const char *buf, size_t count)
{
    unsigned int initrc_is_ok = 0;
    sscanf(buf, "%u", &initrc_is_ok);
    if((1 == initrc_is_ok)&&(g_pmic_mntn))
    {
       pmic_show_cur_time();
       pmic_dump_file(g_pmic_mntn->init_log_show);

       if(g_pmic_mntn->health&(BIT(PMIC_HEALTH_NP_EXCH_HAPPENED)|BIT(PMIC_HEALTH_SMPL_EXCH_HAPPENED)))
       {
           mach_call_usermodeshell(PMU_SHELL_SCRIPT,0);
       }
    }

    return count;
}

static ssize_t pmic_device_check_show(struct kobject *kobj, struct kobj_attribute *attr,
            char *buf)
{
    struct pmic_dev_stat *pmic_list = pmic_dev_info.head;
    char *temp = buf;
    ssize_t len = 0;
    ssize_t sum = 0;

    if(!pmic_list)
    {
        return sprintf(buf, "no device notified\n");
    }

    do
    {
        len = sprintf(temp, "%s : %d\n", pmic_list->dev_name, pmic_list->dev_stat);
        temp = temp + len;
        sum = sum + len;
        pmic_list = pmic_list->next;
    }while(pmic_list);

    return sum;
}

static struct kobj_attribute pmic_mntn_attribute =
    __ATTR(pmic_exch, 0660, pmic_mntn_show, pmic_mntn_store);

static struct kobj_attribute pmic_device_attribute =
    __ATTR(pmic_device_check, 0660, pmic_device_check_show, NULL);

static struct attribute *pmic_mntn_attrs[] = {
    &pmic_mntn_attribute.attr,
    &pmic_device_attribute.attr,
    NULL,   /* need to NULL terminate the list of attributes */
};

static struct attribute_group pmic_mntn_attr_group = {
    .attrs = pmic_mntn_attrs,
};


static struct of_device_id of_hi6xxx_pmic_mntn_match_tbl[] = {
    {
        .compatible = "hisilicon,hi6552-pmic-mntn",
    },
    {
        .compatible = "hisilicon,hi6553-pmic-mntn",
    },

    { /* end */ }
};

static int hi6xxx_pmic_mntn_probe(struct platform_device *pdev)
{
    struct device *dev = &pdev->dev;
    struct device_node *np = dev->of_node;
    PMIC_MNTN_DESC* pmic_mntn = NULL;
    char np_w1c[NP_MAX_SIZE];
    char irq_wlc[IRQ_MAX_SIZE];
    char record_wlc[RECORD_MAX_SIZE];
    int ret = 0;
    volatile unsigned char otmp = 0;

    memset((void*)np_w1c,0xff,sizeof(np_w1c));
    memset((void*)irq_wlc,0xff,sizeof(irq_wlc));
    memset((void*)record_wlc,0xff,sizeof(record_wlc));

    pmic_mntn = (PMIC_MNTN_DESC*)devm_kzalloc(dev, sizeof(*pmic_mntn), GFP_KERNEL);
    if(NULL == pmic_mntn)
    {
        dev_err(dev,"%s devm_kzalloc pmic_mntn err\n",__func__);
        return -ENOMEM;
    }

    g_pmic_mntn = pmic_mntn;
    pmic_mntn->np = np;
    pmic_mntn->health = PMIC_HEALTH_STATUS_NORMAL;

    pmic_mntn->init_log_show = (char*)devm_kzalloc(dev,PMIC_PRINT_BUF_SIZE, GFP_KERNEL);
    if(NULL == pmic_mntn->init_log_show)
    {
        dev_err(dev,"%s devm_kzalloc init_log_show err\n",__func__);
        return -ENOMEM;
    }

    pmic_mntn->irq_log_show = (char*)devm_kzalloc(dev,PMIC_PRINT_BUF_SIZE, GFP_KERNEL);
    if(NULL == pmic_mntn->irq_log_show)
    {
        dev_err(dev,"%s devm_kzalloc irq_log_show err\n",__func__);
        return -ENOMEM;
    }

    ret = of_get_pmic_attr(pmic_mntn,dev,np);
    if(ret)
    {
        dev_err(dev,"%s of_get_pmic_attr  err\n",__func__);
        return ret;
    }

    /*thsd_set_tmp*/
    otmp = hi6xxx_pmic_reg_read(SOC_SMART_BANDGAP_THSD_ADJ2_ADDR(0));
    otmp &= ~(0x03<<SOC_SMART_BANDGAP_THSD_ADJ2_thsd_set_tmp_START);
    otmp |= pmic_mntn->otmp_set<<SOC_SMART_BANDGAP_THSD_ADJ2_thsd_set_tmp_START;
    hi6xxx_pmic_reg_write(SOC_SMART_BANDGAP_THSD_ADJ2_ADDR(0),otmp);

    (void)hi6xxx_pmic_recv_data(pmic_mntn->np_regs.start_addr,pmic_mntn->np_data_buf,pmic_mntn->np_regs.size);
    (void)hi6xxx_pmic_send_data(pmic_mntn->np_regs.start_addr,np_w1c,sizeof(np_w1c));

    (void)hi6xxx_pmic_recv_data(pmic_mntn->irq_regs.start_addr,pmic_mntn->irq_data_buf,pmic_mntn->irq_regs.size);
    (void)hi6xxx_pmic_send_data(pmic_mntn->irq_regs.start_addr,irq_wlc,sizeof(irq_wlc));

    (void)hi6xxx_pmic_recv_data(pmic_mntn->record_regs.start_addr,pmic_mntn->record_data_buf,pmic_mntn->record_regs.size);
    (void)hi6xxx_pmic_send_data(pmic_mntn->record_regs.start_addr,record_wlc,sizeof(record_wlc));

    (void)hi6xxx_pmic_recv_data(pmic_mntn->smpl_desc->record_ctrl.reg_addr,&pmic_mntn->smpl_desc->last_record,0x1);
    (void)hi6xxx_pmic_send_data(pmic_mntn->smpl_desc->record_ctrl.reg_addr,&pmic_mntn->smpl_desc->last_record,0x1);

    pmic_mntn_reg_dump(pmic_mntn);
    pmic_mntn_exch_dect(pmic_mntn);

    pmic_mntn_smpl_config(pmic_mntn);

    pmic_mntn->ocp_scp_irq = platform_get_irq_byname(pdev, "socp");
    if(pmic_mntn->ocp_scp_irq  < 0)
    {
        dev_err(dev,"%s platform_get_irq_byname socp faile\n", __func__);
        return -1;
    }

    pmic_mntn->ocp_scp_wq = create_singlethread_workqueue("pmu-ocp-scp-wq");
    INIT_WORK(&pmic_mntn->ocp_scp_wk,pmic_ocp_scp_wq_hander);

    ret = devm_request_irq(dev,pmic_mntn->ocp_scp_irq, pmic_ocp_scp_irq_hander, IRQF_DISABLED|IRQF_NO_SUSPEND,
          "pmu-ocp_scp-irq",(void*)pmic_mntn);
    if(ret)
    {
        dev_err(dev,"%s,%d request_irq ocp_scp_irq faile\n", __func__, __LINE__);
        return -1;
    }

    pmic_mntn->otmp_irq = platform_get_irq_byname(pdev, "otmp");
    if (pmic_mntn->ocp_scp_irq  < 0)
    {
        dev_err(dev,"%s,platform_get_irq_byname otmp_irq faile\n",__func__);
        return -1;
    }

    pmic_mntn->otmp_wq = create_singlethread_workqueue("pmu-otmp-wq");
    INIT_WORK(&pmic_mntn->otmp_wk, (void *)pmic_otmp_wk_hander);

    ret = devm_request_irq(dev,pmic_mntn->otmp_irq,pmic_otmp_irq_hander, IRQF_DISABLED|IRQF_NO_SUSPEND,
        "pmu-otmp-irq", (void*)pmic_mntn);
    if(ret)
    {
        dev_err(dev,"%s request_irq otmp_irq err\n",__func__);
        return -1;
    }

    pmic_mntn->mntn_later_init_wq = create_singlethread_workqueue("pmu later mntn init");
    if (NULL == pmic_mntn->mntn_later_init_wq)
    {
        dev_err(dev,"%s mntn_later_init_wq create_workqueue failed\n",__func__);
        return -1;
    }

    pmic_mntn->mntn_kobj = kobject_create_and_add("pmic_mntn",kernel_kobj);
    if (NULL == pmic_mntn->mntn_kobj)
    {
       dev_err(dev,"%s kobject_create_and_add err\n",__func__);
       return -1;
    }

    ret = sysfs_create_group(pmic_mntn->mntn_kobj,&pmic_mntn_attr_group);
    if(ret)
    {
        dev_err(dev,"%s sysfs_create_group err\n",__func__);
        kobject_put(pmic_mntn->mntn_kobj);
        return -1;
    }

#if defined (CONFIG_HUAWEI_DSM)
	if (!pmu_ocp_dclient) {
		pmu_ocp_dclient = dsm_register_client(&dsm_pmu_ocp);
	}
 #endif

    return 0;
}

static int hi6xxx_pmic_mntn_remove(struct platform_device *pdev)
{
    return 0;
}

#ifdef CONFIG_PM
static int hi6xxx_pmic_mntn_suspend(struct platform_device *pdev, pm_message_t pm)
{
    return 0;
}

static int hi6xxx_pmic_mntn_resume(struct platform_device *pdev)
{
    return 0;
}
#endif

static struct platform_driver pmic_mntn_driver = {
    .driver = {
        .name = "hisi,hi6xxx-pmic-mntn",
        .owner = THIS_MODULE,
        .of_match_table = of_hi6xxx_pmic_mntn_match_tbl,
    },
    .probe  = hi6xxx_pmic_mntn_probe,
    .remove = hi6xxx_pmic_mntn_remove,
#ifdef CONFIG_PM
    .suspend    = hi6xxx_pmic_mntn_suspend,
    .resume     = hi6xxx_pmic_mntn_resume,
#endif
};

static int __init hi6xxx_pmic_mntn_init(void)
{
    int ret = 0;
    pr_info("\n===============[in %s ]=============\n",__func__);

    ret = platform_driver_register(&pmic_mntn_driver);
    if (ret)
    {
        pr_err("%s: platform_driver_register failed \n",__func__);
    }

    return ret;
}

static void __exit hi6xxx_pmic_mntn_exit(void)
{
    platform_driver_unregister(&pmic_mntn_driver);
}

module_init(hi6xxx_pmic_mntn_init);
module_exit(hi6xxx_pmic_mntn_exit);

MODULE_AUTHOR("fansaihua<fansaihua@huawei.com>");
MODULE_DESCRIPTION("HI6xxx PMU MNTN  driver");
MODULE_LICENSE("GPL V2");

#if 0
/*以下为测试用例*/
static int dpm_scan(unsigned int reg_addr,unsigned int reg_data,int scene)
{
    memset((void*)g_pmic_mntn->init_log_show,'\0',PMIC_PRINT_BUF_SIZE);
    pmic_exch_scan(g_pmic_mntn,reg_addr,reg_data,scene);
    pr_err("%s\n",g_pmic_mntn->init_log_show);
    pmic_dump_file(g_pmic_mntn->init_log_show);
    return 0;
}


static int dpm_regdump(void)
{
    pmic_mntn_reg_dump(g_pmic_mntn);
    pmic_mntn_exch_dect(g_pmic_mntn);
    pmic_dump_file(g_pmic_mntn->init_log_show);
    return 0;
}
#endif

