/*
 **************************************************************************************
 *       Filename:  mntn_video_print.c
 *       Description: print informatin according to different module/level
 *
 *       Version:  1.0
 *       Created:  2013-05-17 12:09:29
 *       Author:  c00220250
 *
 *       Revision:  initial draft;
 **************************************************************************************
 */
#include <asm/uaccess.h>
#include <linux/fs.h>
/*lint -e451*/#include <stdarg.h>/*lint +e451*/

#include <linux/init.h>
#include <linux/module.h>
#include <linux/memory.h>
#include <linux/slab.h>
#include <linux/err.h>
#include <linux/fcntl.h>
#include <linux/time.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#include "video_mntn_kernel.h"


#define FILE_NAME_MAX   (256)

static char *save_path = (char*) "/data/k3_camera/kernel";

/* used for record module information */
mntn_log_module_t g_mntn_log_module_id[EN_ID_MNTN_LOG_MODULE_MAX] = {
    {EN_VIDEO_LOG_LEVLE_DEBUG, "CAM_COM"}
};
mntn_data_save_module g_mntn_video_save_data_module[EN_ID_MNTN_DATA_MODULE_ID_MAX];


/*
 **************************************************************************
 * FunctionName: mntn_video_print;
 * Description : print log according module id and log level;
 * Input       : module_id -- module id, ref to mntn_video_print.h;
                 log_level -- if log_level minor module_id's log level
                                    log will not print;
                 fmt       -- log want to print;
 * Output      : NA;
 * ReturnValue : success printed parameter numbers;
 * Author      : c00220250
 * Date        : 2013/05/18
 * Other       : New function;
 **************************************************************************
 */

int mntn_video_print(uint32_t module_id, uint8_t log_level, char *fmt, ...)
{
    int ret = 0;

    va_list args;

    if (g_mntn_log_module_id[module_id].log_level >= log_level){
        va_start(args, fmt);

        /* return len */
        ret= vprintk(fmt, args);

        va_end(args);
    }
    return ret;
}


/*
 **************************************************************************
 * FunctionName: is_log_module_valid;
 * Description : check module inforamation whether legal when register;
 * Input       : module_id       -- module id, ref to mntn_video_print.h;
                 name            -- module name.named by user;
                 default_level   -- default print level when register module;
 * Output      : NA;
 * ReturnValue : bool type to indicate whether module information is legal;
 * Author      : c00220250;
 * Date        : 2013/06/03;
 * Other       : New function;
 **************************************************************************
 */

bool is_log_module_valid(uint32_t module_id, const char *name, uint8_t default_level)
{
    if ((module_id >= EN_ID_MNTN_LOG_MODULE_MAX)||(default_level >= EN_VIDEO_LOG_LEVEL_MAX)){
        return false;
    }
    return true;
}

/*
 **************************************************************************
 * FunctionName: clear_mntn_log_module;
 * Description : set g_mntn_log_module_id to 0;
 * Input       : NA;
 * Output      : NA;
 * ReturnValue : NA;
 * Author      : c00220250
 * Date        : 2013/06/04
 * Other       : New function;
 **************************************************************************
 */

void clear_mntn_log_module(void)
{
    memset(g_mntn_log_module_id, 0, ((sizeof(mntn_log_module_t))*EN_ID_MNTN_LOG_MODULE_MAX));
}

/*
 **************************************************************************
 * FunctionName: register_mntn_log_module;
 * Description : fill module name and default print level;
 * Input       : module_id       -- module id, ref to mntn_video_print.h;
                 name            -- module name.named by user;
                 default_level   -- default print level when register module;
                                    suggest set 0 when register;
 * Output      : NA;
 * ReturnValue : NA;
 * Author      : c00220250
 * Date        : 2013/06/04
 * Other       : New function;
 **************************************************************************
 */

bool register_mntn_log_module(uint32_t module_id, const char *name, uint8_t default_level)
{
    bool valid = is_log_module_valid(module_id, name, default_level);

    if (false == valid){
        return false;
    }

    strncpy(g_mntn_log_module_id[module_id].module_name, name, MAX_MODULE_NAME_LEN-1);
    g_mntn_log_module_id[module_id].module_name[MAX_MODULE_NAME_LEN-1] = '\0';

    g_mntn_log_module_id[module_id].log_level = default_level;

    return true;
}

/*
 **************************************************************************
 * FunctionName: init_mntn_log_module;
 * Description : register modules when Kernel start;                     \
                 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\
                 if add a new module, must register module here and add  \
                 the module id to the ENUM in mntn_video_print.h;        \
                 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 * Input       : NA;
 * Output      : NA;
 * ReturnValue : NA;
 * Author      : c00220250
 * Date        : 2013/06/03
 * Other       : New function;
 **************************************************************************
 */

void init_mntn_log_module(void)
{
    clear_mntn_log_module();

    /* register new module here */
    register_mntn_log_module(EN_ID_MNTN_CAM_COM_LOG_MODULE, "CAM_COM", EN_VIDEO_LOG_LEVLE_INFO);
    register_mntn_log_module(EN_ID_MNTN_CAM_DUMPREG_LOG_MODULE, "DUMPREG", EN_VIDEO_LOG_LEVLE_OFF);
    register_mntn_log_module(EN_ID_MNTN_BLVIDEO_DRV_DEC, "VIDEO_DEC", EN_VIDEO_LOG_LEVLE_INFO);
    register_mntn_log_module(EN_ID_MNTN_BLVIDEO_DRV_ENC, "VIDEO_ENC", EN_VIDEO_LOG_LEVLE_INFO);
    register_mntn_log_module(EN_ID_MNTN_BLVIDEO_DRV_MEMALLOC, "VIDEO_MEMALLOC", EN_VIDEO_LOG_LEVLE_INFO);
    register_mntn_log_module(EN_ID_MNTN_BLVIDEO_DRV_MEMALLOC_MEMHEAP, "VIDEO_MEMALLOC_MEMHEAP", EN_VIDEO_LOG_LEVLE_INFO);


    register_mntn_log_module(EN_ID_MNTN_TEST_LOG_MODULE,    "TEST",    EN_VIDEO_LOG_LEVLE_OFF);
}


/*
 **************************************************************************
 * FunctionName: is_data_module_valid;
 * Description : check module inforamation whether legal when register;
 * Input       : moduleId       -- module id, ref to mntn_video_print.h;
                 moduleName           -- module name.named by user;
                 frameNum       -- default frame number when register module;
                 imageOrVideo   -- image or video;
 * Output      : NA;
 * ReturnValue : bool type to indicate whether module information is legal;
 * Author      : l00202100;
 * Date        : 2013/06/07;
 * Other       : New function;
 **************************************************************************
 */

bool is_data_module_valid(unsigned moduleId, const char *moduleName, int frameNum,mntn_data_save_type imageOrVideo)
{
    if ((moduleId >= EN_ID_MNTN_DATA_MODULE_ID_MAX)||(frameNum < 0)){
        return false;
    }
    return true;
}


/*
 **************************************************************************
 * FunctionName: register_mntn_video_save_data_module;
 * Description : set gMntnVideoSaveDataModule[moduleId] to the right param;
 * Input       : moduleId       -- module id, ref to mntn_video_print.h;
                 moduleName     -- module name.named by user;
                 frameNum       -- default frame number when register module;
                 imageOrVideo   -- image or video;
 * Output      : NA;
 * ReturnValue : NA;
 * Author      : l00202100
 * Date        : 2013/06/07
 * Other       : New function;
 **************************************************************************
 */
bool register_mntn_video_save_data_module(unsigned moduleId, const char *moduleName, unsigned frameNum,mntn_data_save_type imageOrVideo)
{
    bool valid = is_data_module_valid(moduleId, moduleName, frameNum,imageOrVideo);

    if (false == valid){
        return false;
    }

    strncpy(g_mntn_video_save_data_module[moduleId].ModuleName, moduleName, MAX_MODULE_NAME_LEN-1);
    g_mntn_video_save_data_module[moduleId].ModuleName[MAX_MODULE_NAME_LEN-1] = '\0';
    g_mntn_video_save_data_module[moduleId].FrameNum= frameNum;
    g_mntn_video_save_data_module[moduleId].ImageOrVideo= imageOrVideo;

    return true;
}


/*
 **************************************************************************
 * FunctionName: clear_mntn_video_save_data_module;
 * Description : set gMntnVideoSaveDataModule to 0;
 * Input       : NA;
 * Output      : NA;
 * ReturnValue : NA;
 * Author      : l00202100
 * Date        : 2013/06/07
 * Other       : New function;
 **************************************************************************
 */
void clear_mntn_video_save_data_module(void)
{
    memset(g_mntn_video_save_data_module, 0, ((sizeof(mntn_data_save_module))*EN_ID_MNTN_DATA_MODULE_ID_MAX));
}


/*
 **************************************************************************
 * FunctionName: init_mntn_video_module;
 * Description : init gMntnVideoSaveDataModule[moduleId];
 * Input       : NA;
 * Output      : NA;
 * ReturnValue : NA;
 * Author      : l00202100
 * Date        : 2013/06/07
 * Other       : New function;
 **************************************************************************
 */
void init_mntn_video_module(void)
{
    clear_mntn_video_save_data_module();
    register_mntn_video_save_data_module(EN_ID_MNTN_DATA_MODULE_ID_ENC, "ENC", 0, EN_VIDEO_IMAGE);
}


/*
 **************************************************************************
 * FunctionName: mntn_save_image_data_to_file;
 * Description : init gMntnVideoSaveDataModule[moduleId];
 * Input       : FileName     -- file name.maked by code;
                 Buffer       -- the address which you want to save;
                 Size         -- the buffer's size;
 * Output      : NA;
 * ReturnValue : NA;
 * Author      : l00202100
 * Date        : 2013/06/07
 * Other       : New function;
 **************************************************************************
 */
void mntn_save_image_data_to_file(char* FileName, void* Buffer, unsigned long Size)
{

    ssize_t         ret = 0;
    struct file    *fp  = NULL;
    mm_segment_t    fs;
    loff_t          pos = 0;

    printk("before filp_open %s", FileName);
    fp = (struct file *) filp_open(FileName, O_RDWR | O_CREAT, 0644);   /* 0644，八进制，表示权限 */   /* lint need convert to (struct file *) explicitly */
    if (IS_ERR(fp)) {
        printk("filp_open %s error, fp = %d !", FileName, (int)fp);
    }

    fs = get_fs();
    set_fs(KERNEL_DS);
    ret = vfs_write(fp, Buffer, Size, &pos);
    if (ret != Size) {
        printk("write file: %s error [write: %d, size; %ld]", FileName, (int)ret, Size);
    }

    filp_close(fp, NULL);
    set_fs(fs);
}


/*
 **************************************************************************
 * FunctionName: mntn_save_video_data_to_file;
 * Description : init gMntnVideoSaveDataModule[moduleId];
 * Input       : FileName     -- file name.maked by code;
                 Buffer       -- the address which you want to save;
                 Size         -- the buffer's size;
 * Output      : NA;
 * ReturnValue : NA;
 * Author      : l00202100
 * Date        : 2013/06/07
 * Other       : New function;
 **************************************************************************
 */
void mntn_save_video_data_to_file(char* FileName, void* Buffer, unsigned long Size)
{
    ssize_t         ret = 0;
    struct file    *fp  = NULL;
    mm_segment_t    fs;
    loff_t          pos = 0;

    printk("before filp_open %s", FileName);
    fp = (struct file *) filp_open(FileName, O_RDWR | O_CREAT | O_APPEND, 0644);   /* 0644，八进制，表示权限 */   /* lint need convert to (struct file *) explicitly */
    if (IS_ERR(fp)) {
        printk("filp_open %s error, fp = %d !", FileName, (int)fp);
    }

    fs = get_fs();
    set_fs(KERNEL_DS);
    ret = vfs_write(fp, Buffer, Size, &pos);
    if (ret != Size) {
        printk("write file: %s error [write: %d, size; %ld]", FileName, (int)ret, Size);
    }

    filp_close(fp, NULL);
    set_fs(fs);
}


/*
 **************************************************************************
 * FunctionName: mntn_save_data;
 * Description : init gMntnVideoSaveDataModule[moduleId];
 * Input       : FileName     -- file name.maked by code;
                 Buffer       -- the address which you want to save;
                 Size         -- the buffer's size;
                 Width        -- Frame's width
                 Hight        -- Frame's hight
                 SaveTypes    -- save types:include image and video;
 * Output      : NA;
 * ReturnValue : NA;
 * Author      : l00202100
 * Date        : 2013/06/07
 * Other       : New function;
 **************************************************************************
 */
void mntn_save_data(mntn_data_module_id_type ModuleId, void *Buffer, const int Size, int Width, int Hight, char *SaveTypes)
{
    char Pname[FILE_NAME_MAX] = {0};
    struct timeval Tv;

    memset(&Tv, 0, sizeof(Tv));

    if ((NULL == Buffer) || (0 == Size))
    {
        printk("Buffer info error!");
        return;
    }

    if ((unsigned int)ModuleId >= EN_ID_MNTN_DATA_MODULE_ID_MAX)
    {
        printk("module id %d error!", ModuleId);
        return;
    }

    if(g_mntn_video_save_data_module[ModuleId].FrameNum == 0)
    {
        return;
    }

    do_gettimeofday(&Tv);

    if(g_mntn_video_save_data_module[ModuleId].ImageOrVideo == EN_VIDEO_IMAGE)
    {
        snprintf(Pname,
                 FILE_NAME_MAX,
                 "%s/mntn_kernel_%03d_img_%04d_%04d_%04d_%s_%02d_%06d.dump",
                 save_path,
                 ModuleId,
                 g_mntn_video_save_data_module[ModuleId].FrameNum,
                 Width,
                 Hight,
                 SaveTypes,
                 (int)Tv.tv_sec,
                 (int)Tv.tv_usec);
        mntn_save_image_data_to_file(Pname,Buffer, Size);
        g_mntn_video_save_data_module[ModuleId].FrameNum--;
    }
    else
    {
        snprintf(Pname,
                 FILE_NAME_MAX,
                 "%s/mntn_kern_video_%3d_%04d_%04d_%s.dump",
                 save_path,
                 ModuleId,
                 Width,
                 Hight,
                 SaveTypes);
        mntn_save_video_data_to_file(Pname,Buffer, Size);
        g_mntn_video_save_data_module[ModuleId].FrameNum--;
    }


    return;
}

EXPORT_SYMBOL(g_mntn_log_module_id);
EXPORT_SYMBOL(mntn_video_print);
EXPORT_SYMBOL(init_mntn_log_module);

EXPORT_SYMBOL(g_mntn_video_save_data_module);
EXPORT_SYMBOL(init_mntn_video_module);
EXPORT_SYMBOL(mntn_save_data);
