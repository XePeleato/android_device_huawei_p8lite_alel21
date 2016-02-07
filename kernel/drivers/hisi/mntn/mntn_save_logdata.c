/*
 * Implement saving log in this file
 *
 * Copyright (c) 2013 Hisilicon Technologies CO., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/stat.h>
#include <linux/time.h>
#include <linux/mtd/hisi_nve_interface.h>
#include <linux/hisi/hi6xxx-platform.h>
#include <linux/hisi/reset.h>
#include "mntn_save_logdata.h"
#include "mntn_filesys.h"
#include <general_sram_map.h>
#include <soc_baseaddr_interface.h>
#include "soc_ao_sctrl_interface.h"
#include "MemoryMap.h"
#include "excDrv.h"

/********************************************************************
Define macros here
********************************************************************/
#define MNTN_RGZNAME_SZ 64
#define MNTN_FULLPATH_STRING_LEN		128
#define MNTN_TIME_STRING_LEN		20
#define MNTN_MDM_LOG_MAX     5
#define MNTN_HIFI_LOG_MAX     5
#define MNTN_MCU_LOG_MAX     5

#define MODEM_SYS_MEM_SAVE_SIZE 0x06F00000

/********************************************************************
Define global variables here
********************************************************************/

/********************************************************************
extern global variables here
********************************************************************/
extern unsigned int himntn_modem_resetlog;
#ifdef CONFIG_DEBUG_FS
extern void* g_dump_addr;
extern unsigned int g_dump_size;
#endif
/********************************************************************
extern functions here
*******************************************************************/
extern void mntn_rm_old_log(const char *path, unsigned int unumber);
extern unsigned int get_boot_into_recovery_flag(void);
extern void sort(void *base, size_t num, size_t size,
	  int (*cmp_func)(const void *, const void *),
	  void (*swap_func)(void *, void *, int size));
/********************************************************************
implement functions here
********************************************************************/


static int mntn_read_nogui_flag(void)
{
    int ret = 0;
    struct hisi_nve_info_user user_info;
    int nogui_flag = 0;

    user_info.nv_operation = 1;
    user_info.nv_number = 206;
    user_info.valid_size = 1;
    strncpy(user_info.nv_name, "NOGUI", sizeof("NOGUI"));
    ret = hisi_nve_direct_access(&user_info);
    if (ret)
    {
	 MNTN_FILESYS_PRINT(KERN_ERR"mntn_err: nve_direct_access read error(%d)", ret);
        return -1;
    }
    nogui_flag = user_info.nv_data[0];
    if (nogui_flag == 1)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

static int mntn_dump_sram_mcu_prepare(void)
{
    void __iomem * sys_ctrl_addr;	
    unsigned int sc_ctrl1;

    sys_ctrl_addr = ioremap(REG_BASE_SC_ON, REG_SC_ON_IOSIZE);
    if (NULL == sys_ctrl_addr)
    {
        MNTN_FILESYS_PRINT(KERN_ERR"mntn_err: fail to map before accessing mcu sram!\n");	
        return -1;
    };
    sc_ctrl1 = readl(SOC_AO_SCTRL_SC_SYS_CTRL1_ADDR(sys_ctrl_addr));
    sc_ctrl1 |= 0x80000000;
    sc_ctrl1 &= ~0x00008000;
    writel(sc_ctrl1, (SOC_AO_SCTRL_SC_SYS_CTRL1_ADDR(sys_ctrl_addr)));
    iounmap(sys_ctrl_addr);
    return 0;
}


static int mntn_dump_sram_mcu_finish(void)
{
    void __iomem * sys_ctrl_addr;
    unsigned int sc_ctrl1;

    sys_ctrl_addr = ioremap(REG_BASE_SC_ON, REG_SC_ON_IOSIZE);
    if (NULL == sys_ctrl_addr)
    {
        MNTN_FILESYS_PRINT(KERN_ERR"mntn_err: fail to map after accessing mcu sram!\n");	
        return -1;
    };
    sc_ctrl1 = readl(SOC_AO_SCTRL_SC_SYS_CTRL1_ADDR(sys_ctrl_addr));
    sc_ctrl1 |= 0x80008000;
    writel(sc_ctrl1, (SOC_AO_SCTRL_SC_SYS_CTRL1_ADDR(sys_ctrl_addr)));
    iounmap(sys_ctrl_addr);
    return 0;
}


void mntn_get_cur_time_str(char *poutstr, unsigned int ulen)
{
    struct tm     tm_rtc = {0};
    unsigned long    cur_secs = 0;

    if (NULL != poutstr)
    {
	cur_secs = get_seconds();
	cur_secs -= sys_tz.tz_minuteswest * 60;
	time_to_tm(cur_secs, 0, &tm_rtc);        
	snprintf(poutstr, ulen, "%lu%.2d%.2d-%.2d%.2d%.2d", 1900 + tm_rtc.tm_year, tm_rtc.tm_mon + 1, tm_rtc.tm_mday, tm_rtc.tm_hour, tm_rtc.tm_min, tm_rtc.tm_sec);
    }
}

int mntn_do_save_virtual_addr_log(unsigned char *viradd, unsigned int ulen, const char *ppath, const char *pfilename)
{
    int iret = 0;
    char    fullpath_arr[MNTN_FULLPATH_STRING_LEN + 1] = {0};
    long	fd = 0;
     int    bytes = 0;
    unsigned char    *pdatabuf = viradd;
    mm_segment_t old_fs = 0;

    if (NULL == pdatabuf || NULL == viradd || NULL == pfilename)
    {
        MNTN_FILESYS_PRINT(KERN_ERR"mntn_err: some parameter is NULL!\n");
        return -1;
    }   

    MNTN_FILESYS_PRINT(KERN_ERR"mntn_do_save_virtual_addr_log: viradd %p, length is %d\n", viradd, ulen);
    /*Write reset reason into history.log*/
    memset(fullpath_arr, 0, MNTN_FULLPATH_STRING_LEN);
    strncat(fullpath_arr, ppath, strlen(ppath));
    strncat(fullpath_arr, pfilename, strlen(pfilename));
    old_fs = get_fs();
    set_fs(KERNEL_DS);
    fd = sys_open(fullpath_arr, O_CREAT | O_RDWR, MNTN_LOG_FILE_PRO_VALUE);
    if (fd < 0)
    {
        MNTN_FILESYS_PRINT(KERN_ERR"mntn_err: Fail to open %s!, fd %lx\n", pfilename, fd);
	 set_fs(old_fs);
	 iret = -1;
	 goto oper_over2;
    }
   
    bytes = sys_write(fd, (void*)pdatabuf, ulen);
    if (bytes != ulen) 
    {
        MNTN_FILESYS_PRINT(KERN_ERR"mntn_err: Fail to write all the data into the file, %d/%d\n", bytes, ulen);
	 iret = -1;
	 goto oper_over1;
    }
    iret = sys_fsync(fd);
    if (iret < 0) 
    {
        MNTN_FILESYS_PRINT(KERN_ERR"mntn_err: Fail to sys_fsync data\n");
	 goto oper_over1;
    }
    iret += mntn_filesys_chown((const char *)fullpath_arr, MNTN_LOG_FILE_OWNER_UID, MNTN_LOG_FILE_OWNER_GID);
    if (0 != iret)
    {
        MNTN_FILESYS_PRINT(KERN_ERR"mntn_err: Fail to chown history.log!\n");
    }

oper_over1:
    (void)sys_close((unsigned int)fd);
oper_over2:
    set_fs(old_fs);
    MNTN_FILESYS_PRINT(KERN_ERR"mntn_do_save_virtual_addr_log: over\n");
    return iret;
}
int mntn_do_save_phy_addr_log(unsigned int phyadd, unsigned int ulen, const char *ppath, const char *pfilename)
{
    int iret = 0;
    unsigned char    *pdatabuf = NULL;

    pdatabuf = ioremap(phyadd, ulen);
    if (NULL == pdatabuf)
    {
        MNTN_FILESYS_PRINT(KERN_ERR"mntn_err: fail to ioremap when saving %s data!\n", pfilename);
        return -1;
    }   
    iret = mntn_do_save_virtual_addr_log(pdatabuf, ulen, ppath, pfilename);
    iounmap(pdatabuf);
    return iret;
}

int mntn_upadte_history_info(const char *ptimestr, const char *preason)
{
    int iret = 0;
    char    fullpath_arr[MNTN_FULLPATH_STRING_LEN + 1] = {0};
    char    countent_arr[MNTN_FULLPATH_STRING_LEN + 1] = {0};

    int    bytes = 0;
    uint    ulen = 0;
		
    memset(fullpath_arr, 0, MNTN_FULLPATH_STRING_LEN);
    strncat(fullpath_arr, MNTN_AP_LOGDIR, strlen(MNTN_AP_LOGDIR));
    strncat(fullpath_arr, MNTN_HISTORY_LOG, strlen(MNTN_HISTORY_LOG));

    /*Write reset reason into history.log*/
    memset(countent_arr, 0, sizeof(countent_arr));
    strncat(countent_arr, ptimestr, strlen(ptimestr));
    strncat(countent_arr, " : ", strlen(" : "));
    strncat(countent_arr, preason, strlen(preason));

    ulen = strlen(countent_arr);
    bytes = mntn_filesys_write_log((const char *)fullpath_arr, (void*)countent_arr, ulen, 0);
    if (bytes != ulen) 
    {
        MNTN_FILESYS_PRINT(KERN_ERR"mntn_err: Fail to write all the data into the file, %d/%d\n", bytes, ulen);
	 iret = -1;
    }
    iret += mntn_filesys_chown((const char *)fullpath_arr, MNTN_LOG_FILE_OWNER_UID, MNTN_LOG_FILE_OWNER_GID);
    if (0 != iret)
    {
        MNTN_FILESYS_PRINT(KERN_ERR"mntn_err: Fail to chown history.log!\n");
    }

    return iret;
}
int mntn_str_cmp(const void *a, const void *b)
{
    return strncmp((char *)a, (char *)b, MNTN_FILESYS_PURE_DIR_NAME_LEN);
}


void mntn_rm_old_log(const char *ppath, unsigned int unumber)
{
    char    fullpath_arr[MNTN_FULLPATH_STRING_LEN + 1] = {0};
    int       i = 0;
    int       iret = 0;
    unsigned int ulen = 0;
    int	tmp_cnt = MNTN_FILESYS_MAX_CYCLE * MNTN_FILESYS_PURE_DIR_NAME_LEN;	
    char    *pbuff = NULL;

    pbuff = kmalloc(tmp_cnt, GFP_KERNEL);
    if (NULL == pbuff)
    {
        MNTN_FILESYS_PRINT(KERN_ERR"mntn_err: fail to kmalloc when removing old log\n");    
        return;
    }
    memset((void*)pbuff, 0, tmp_cnt);
    tmp_cnt = mntn_filesys_dir_list(ppath, pbuff, tmp_cnt, DT_DIR);
    if (tmp_cnt >= unumber)
    {
        sort((void*)pbuff, tmp_cnt, MNTN_FILESYS_PURE_DIR_NAME_LEN, mntn_str_cmp, NULL);
        for (i = 0;i < (tmp_cnt - unumber + 1);i++)
        {
            strncat(fullpath_arr, ppath, strlen(ppath));
	     ulen = strlen(pbuff + i * MNTN_FILESYS_PURE_DIR_NAME_LEN);
            strncat(fullpath_arr, (const char*)(pbuff + i * MNTN_FILESYS_PURE_DIR_NAME_LEN), ulen);
	     strncat(fullpath_arr, "/", strlen("/"));
	     
            iret = mntn_filesys_rm_all_file(fullpath_arr);
            iret += mntn_filesys_rm_dir(fullpath_arr);
	     if (0 != iret)
            {
	         MNTN_FILESYS_PRINT(KERN_ERR"mntn_err: fail to rm dir, %d", i);
	     }
	     memset((void*)fullpath_arr, 0, sizeof(fullpath_arr));
        }	 
    }
    kfree(pbuff);
}


int mntn_need_save_log(ereset_module etype)
{
    unsigned int   uflag = 0;
    switch (etype)
    {
    case BSP_RESET_MODULE_CCORE:
	uflag = himntn_modem_resetlog;
	break;
    case BSP_RESET_MODULE_HIFI:
	uflag = himntn_hifi_resetlog;
	break;
    case BSP_RESET_MODULE_MCU:
	uflag = himntn_gobal_resetlog;
	break;
    default:
       MNTN_FILESYS_PRINT(KERN_ERR"mntn_err: Wrong module type when saving log data!\n");
	break;
    }

    if (0 == uflag)
    {
        return 0;
    }
    else
    {
        if ((get_boot_into_recovery_flag()==1)&& (mntn_read_nogui_flag()==0))
	{
	    return 0;
	}
	return 1;
    }
}

int mntn_mdm_reset_save_log(const char *preason)
{
    int iret = 0;
    char    time_arr[MNTN_TIME_STRING_LEN+1] = {0};
    char    fullpath_arr[MNTN_FULLPATH_STRING_LEN + 1] = {0};

    if (NULL == preason)
    {
        MNTN_FILESYS_PRINT(KERN_ERR"mntn_err: preason is NULL!\n");
	 return -1;
    }

    /*to remove old log data if there has been MAX_NUMBER logs.*/
    mntn_rm_old_log(MNTN_CP_LOGDIR, MNTN_MDM_LOG_MAX);

    /*Create dir named by time in cp_log/coredump*/
    mntn_get_cur_time_str(time_arr, MNTN_TIME_STRING_LEN);
    strncat(fullpath_arr, MNTN_CP_LOGDIR, strlen(MNTN_CP_LOGDIR));
    strncat(fullpath_arr, time_arr, strlen(time_arr));
    strncat(fullpath_arr, "/", strlen("/"));
    iret = mntn_filesys_create_dir(fullpath_arr, MNTN_CP_DIR_PRO_VALUE);
    if (0 != iret)
    {
        MNTN_FILESYS_PRINT(KERN_ERR"mntn_err: Fail to create cp log dir!\n");
	 goto oper_over;
    }
    /*change owner*/
    iret += mntn_filesys_chown((const char *)fullpath_arr, MNTN_LOG_DIR_OWNER_UID, MNTN_LOG_DIR_OWNER_GID);
    if (0 != iret)
    {
        MNTN_FILESYS_PRINT(KERN_ERR"mntn_err: Fail to chown cp log dir!\n");
    }

    if (mntn_need_save_log(BSP_RESET_MODULE_CCORE) == 0)
    {
        MNTN_FILESYS_PRINT(KERN_ERR"mntn_err: Needn't save all log data, just modem_log.bin\n");
        iret = mntn_do_save_phy_addr_log(MODEM_DUMP_LOG_ADDR, MODEM_DUMP_LOG_SIZE, (const char*)fullpath_arr, MNTN_MODEM_LOG_FILE);
        return iret;
    }
    /*save acpu sram data*/
    iret += mntn_do_save_phy_addr_log(SOC_SRAM_OFF_BASE_ADDR, SRAM_SIZE, (const char*)fullpath_arr, MNTN_SRAME_ACPU_FILE);

    /*save mcu sram data*/
    mntn_dump_sram_mcu_prepare();
    iret += mntn_do_save_phy_addr_log(REG_BASE_SRAM_MCU, REG_SRAM_MCU_IOSIZE, (const char*)fullpath_arr, MNTN_SRAME_MCU_FILE);
    mntn_dump_sram_mcu_finish();

    /*save bbe16 data*/
    iret += mntn_do_save_phy_addr_log(GLOBAL_MEM_SDR_ADDR, GLOBAL_MEM_SDR_SIZE, (const char*)fullpath_arr, MNTN_BBE16_SDR_FILE);
    iret += mntn_do_save_phy_addr_log(GLOBAL_MEM_TDS_TABLE_ADDR, GLOBAL_MEM_TDS_TABLE_SIZE, (const char*)fullpath_arr, MNTN_BBE16_TDSTABLE_FILE);
    iret += mntn_do_save_phy_addr_log(GLOBAL_MEM_LT_IMAGE_ADDR, GLOBAL_MEM_LT_IMAGE_SIZE, (const char*)fullpath_arr, MNTN_BBE16_IMGDDR_FILE);
    if(get_domain_access_status(ACCESS_DOMAIN_BBE16_DTCM) == 1)
    {
        iret += mntn_do_save_phy_addr_log(REG_BASE_DTCM_BBE16, REG_DTCM_BBE16_IOSIZE, (const char*)fullpath_arr, MNTN_BBE16_DTCM_FILE);    
    }

    /*save telemntn memory data*/
    mntn_do_save_phy_addr_log(TELE_MNTN_AREA_ADDR, TELE_MNTN_AREA_SIZE, (const char*)fullpath_arr, MNTN_TELEMNTN_FILE);
    if (get_domain_access_status(ACCESS_DOMAIN_PERI_SC) == 1)
    {
        /*save peri sc data*/
        iret = mntn_do_save_phy_addr_log(REG_BASE_SC_OFF, REG_SC_OFF_IOSIZE, (const char*)fullpath_arr, MNTN_PERI_SC_FILE);
    }

    /*if peri is power down, returen error*/
    if (get_domain_access_status(ACCESS_DOMAIN_MODEM_SC) == 1)
    {
        /*save modem sc data*/
        iret = mntn_do_save_phy_addr_log(SOC_MODEM_SCTRL_BASE_ADDR, SOC_MODEM_SC_SIZE, (const char*)fullpath_arr, MNTN_MODEM_SC_FILE);
    }

    /*save modem log data*/
    iret += mntn_do_save_phy_addr_log(MODEM_DUMP_LOG_ADDR, MODEM_DUMP_LOG_SIZE, (const char*)fullpath_arr, MNTN_MODEM_LOG_FILE);
    /*save modem memory data*/
    iret += mntn_do_save_phy_addr_log(MODEM_SYS_MEM_ADDR, MODEM_SYS_MEM_SAVE_SIZE, (const char*)fullpath_arr, MNTN_MODEM_MEMORY_FILE);

	
    /*Write reset reason into history.log*/
    iret += mntn_upadte_history_info((const char*)time_arr, preason);
oper_over:
    return iret;
}
int mntn_hifi_reset_save_log(const char *preason)
{
    int iret = 0;
    char    time_arr[MNTN_TIME_STRING_LEN+1] = {0};
    char    fullpath_arr[MNTN_FULLPATH_STRING_LEN + 1] = {0};

    if (NULL == preason)
    {
        MNTN_FILESYS_PRINT(KERN_ERR"mntn_err: preason is NULL!\n");
	 return -1;
    }

    if (mntn_need_save_log(BSP_RESET_MODULE_HIFI) == 0) 
    {
        MNTN_FILESYS_PRINT(KERN_ERR"mntn_err: Needn't save log data!\n");
        return 0;
    }
	
    /*to remove old log data if there has been MAX_NUMBER logs.*/
    mntn_rm_old_log(MNTN_HIFI_LOGDIR, MNTN_HIFI_LOG_MAX);

    /*Create dir named by time in hifi_log/coredump*/
    mntn_get_cur_time_str(time_arr, MNTN_TIME_STRING_LEN);
    strncat(fullpath_arr, MNTN_HIFI_LOGDIR, strlen(MNTN_HIFI_LOGDIR));
    strncat(fullpath_arr, time_arr, strlen(time_arr));
    strncat(fullpath_arr, "/", strlen("/"));
    iret = mntn_filesys_create_dir(fullpath_arr, MNTN_HIFI_DIR_PRO_VALUE);
    if (0 != iret)
    {
        MNTN_FILESYS_PRINT(KERN_ERR"mntn_err: Fail to create hifi log dir!\n");
        goto oper_over;
    }
    /*change owner*/
    iret += mntn_filesys_chown((const char *)fullpath_arr, MNTN_LOG_DIR_OWNER_UID, MNTN_LOG_DIR_OWNER_GID);
    if (0 != iret)
    {
        MNTN_FILESYS_PRINT(KERN_ERR"mntn_err: Fail to chown hifi log dir!\n");
    }

    /*save acpu sram data*/
    iret += mntn_do_save_phy_addr_log(SOC_SRAM_OFF_BASE_ADDR, SRAM_SIZE, (const char*)fullpath_arr, MNTN_SRAME_ACPU_FILE);
    /*save mcu sram data*/
    mntn_dump_sram_mcu_prepare();
    iret += mntn_do_save_phy_addr_log(REG_BASE_SRAM_MCU, REG_SRAM_MCU_IOSIZE, (const char*)fullpath_arr, MNTN_SRAME_MCU_FILE);
    mntn_dump_sram_mcu_finish();

    /*save a_mntn data*/
    iret += mntn_do_save_phy_addr_log(ANDROID_DUMP_LOG_ADDR, ANDROID_DUMP_LOG_SIZE, (const char*)fullpath_arr, MNTN_A_EXC_MEMORY_FILE);
    /*save hifi memory data*/
    iret += mntn_do_save_phy_addr_log(HIFI_SYS_MEM_ADDR, HIFI_SYS_MEM_SIZE, (const char*)fullpath_arr, MNTN_HIFI_MEMORY_FILE);
#ifdef CONFIG_DEBUG_FS
    iret += mntn_do_save_virtual_addr_log((unsigned char*)g_dump_addr, g_dump_size, (const char*)fullpath_arr, MNTN_HIFI_RH_FILE);
#endif
	
    /*Write reset reason into history.log*/
    iret += mntn_upadte_history_info((const char*)time_arr, preason);
oper_over:
    return iret;
}

int mntn_mcu_reset_save_log(const char *preason)
{
    int iret = 0;
    char    time_arr[MNTN_TIME_STRING_LEN+1] = {0};
    char    fullpath_arr[MNTN_FULLPATH_STRING_LEN + 1] = {0};

    if (NULL == preason)
    {
        MNTN_FILESYS_PRINT(KERN_ERR"mntn_err: preason is NULL!\n");
	 return -1;
    }

    if (mntn_need_save_log(BSP_RESET_MODULE_MCU) == 0) 
    {
        MNTN_FILESYS_PRINT(KERN_ERR"mntn_err: Needn't save log data!\n");
        return 0;
    }
    /*to remove old log data if there has been MAX_NUMBER logs.*/
    mntn_rm_old_log(MNTN_MCU_LOGDIR, MNTN_MDM_LOG_MAX);

    /*Create dir named by time in ap_log/coredump*/
    mntn_get_cur_time_str(time_arr, MNTN_TIME_STRING_LEN);
    strncat(fullpath_arr, MNTN_MCU_LOGDIR, strlen(MNTN_MCU_LOGDIR));
    strncat(fullpath_arr, time_arr, strlen(time_arr));
    strncat(fullpath_arr, "/", strlen("/"));

    MNTN_FILESYS_PRINT(KERN_ERR"mntn_err:save mcu log step 0!\n");
    iret = mntn_filesys_create_dir(fullpath_arr, MNTN_MCU_DIR_PRO_VALUE);
    if (0 != iret)
    {
        MNTN_FILESYS_PRINT(KERN_ERR"mntn_err: Fail to create cp log dir!\n");
        goto oper_over;
    }
    /*change owner*/
    iret += mntn_filesys_chown((const char *)fullpath_arr, MNTN_LOG_DIR_OWNER_UID, MNTN_LOG_DIR_OWNER_GID);
    if (0 != iret)
    {
        MNTN_FILESYS_PRINT(KERN_ERR"mntn_err: Fail to chown mcu log dir!\n");
    }

    /*save acpu sram data*/
    iret += mntn_do_save_phy_addr_log(SOC_SRAM_OFF_BASE_ADDR, SRAM_SIZE, (const char*)fullpath_arr, MNTN_SRAME_ACPU_FILE);
    /*save mcu sram data*/
    mntn_dump_sram_mcu_prepare();
    iret += mntn_do_save_phy_addr_log(REG_BASE_SRAM_MCU, REG_SRAM_MCU_IOSIZE, (const char*)fullpath_arr, MNTN_SRAME_MCU_FILE);
    mntn_dump_sram_mcu_finish();

    /*save telemntn memory data*/
    mntn_do_save_phy_addr_log(TELE_MNTN_AREA_ADDR, TELE_MNTN_AREA_SIZE, (const char*)fullpath_arr, MNTN_TELEMNTN_FILE);

    /*save mcu log data*/
    iret += mntn_do_save_phy_addr_log(MCU_DUMP_LOG_ADDR, MCU_DUMP_LOG_SIZE, (const char*)fullpath_arr, MNTN_MCU_LOG_FILE);

    /*save mcu memory data*/
    iret += mntn_do_save_phy_addr_log(MCU_SYS_MEM_ADDR, MCU_SYS_MEM_SIZE, (const char*)fullpath_arr, MNTN_MCU_MEMORY_FILE);

    /*save a_mntn data*/
    iret += mntn_do_save_phy_addr_log(ANDROID_DUMP_LOG_ADDR, ANDROID_DUMP_LOG_SIZE, (const char*)fullpath_arr, MNTN_A_EXC_MEMORY_FILE);

    /*Write reset reason into history.log*/
    iret += mntn_upadte_history_info((const char*)time_arr, preason);

    /*reboot, have to msleep, or fail to update history.log*/
    msleep(1000);
  /*  machine_restart((char*)"reboot");*/
    MNTN_FILESYS_PRINT(KERN_ERR"mntn_err:save mcu log over!\n");
oper_over:
    return iret;
}

EXPORT_SYMBOL(mntn_mdm_reset_save_log);
EXPORT_SYMBOL(mntn_hifi_reset_save_log);
EXPORT_SYMBOL(mntn_mcu_reset_save_log);



