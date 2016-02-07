

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include <linux/timer.h>
#include <linux/jiffies.h>
#include <linux/errno.h>
#include <linux/module.h>
#include <linux/workqueue.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/delay.h>
#include <linux/poll.h>
#include <linux/slab.h>
#include <linux/kallsyms.h>
#include <linux/semaphore.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <asm/io.h>
#include <linux/spinlock.h>
#include <linux/interrupt.h>
#include <linux/ioport.h>
#include <linux/err.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/list.h>

#include <linux/semaphore.h>
#include <linux/syscalls.h>

#include <linux/stat.h>
#include <asm-generic/fcntl.h>
#include "partition.h"

#include "BSP.h"
#include "rfilesystem_process.h"
#include "rfilesystem_transfer.h"
#include "rfilesystem_logunit.h"
#include "mdrv_udi.h"

#if defined (CONFIG_HISILICON_PLATFORM_POWER_CONTROL)
#include <linux/hisi/pm/hi6xxx-power-common.h>
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/
/*核心处理函数指针*/
fileOps rfileOpsSet[OPS_MAX];

char rw_buf[BUF_LEN];

/*当前只记录写入的数据帧数的原因是基于其他操作下都只有一帧*/
/*记录当前写入的帧数*/
int      g_curr_frame_num = 0;

/*记录masswrite写入的帧数*/
int      g_masswt_curr_frame_num = 0;

/*fwrite累计写入的包的长度*/
int g_fwrite_total_len  = 0;

/*mass write offset累计*/
int g_masswrite_offset = 0;

struct rfile_ops rfile_linux_ops = {
    .bsp_fopen      = bsp_linux_fopen,
    .bsp_fclose     = bsp_linux_fclose,
    .bsp_fread      = bsp_linux_fread,
    .bsp_fwrite     = bsp_linux_fwrite,
    .bsp_fseek      = bsp_linux_fseek,
    .bsp_remove     = bsp_linux_remove,
    .bsp_ftell      = bsp_linux_ftell,
    .bsp_mkdir      = bsp_linux_mkdir,
    .bsp_rmdir      = bsp_linux_rmdir,
    .bsp_opendir    = bsp_linux_opendir,
    .bsp_readdir    = bsp_linux_readdir,
    .bsp_closedir   = bsp_linux_closedir,
    .bsp_stat       = bsp_linux_stat,
    .bsp_access     = bsp_linux_access,
    .bsp_xcopy      = bsp_linux_xcopy,
    .bsp_xdelete    = bsp_linux_xdelete,
    .bsp_rename     = bsp_linux_rename,
    .bsp_mass_read  = bsp_mass_read,
    .bsp_mass_write = bsp_mass_write,
};

/*可维可测*/
unsigned int g_rfile_wt_expect_cur = 0;

unsigned int g_mass_wt_expect_cur = 0;

fp_list_t fp_list_file;

fp_list_t fp_list_dir;

/*打印开关*//*0:OFF  1:ON*/
int g_rfile_dbg_lvl = 0;

extern RFILECONTEXT_A rFileContext_a;

extern struct semaphore  sem_to_read_req;

extern struct partition partitions[];

#ifdef _DRV_LLT_
unsigned int g_ulmmc_suspend_flag = false;
#endif

extern unsigned int g_ulmmc_suspend_flag;

extern int g_rfilesystem_init;

/*FS接口内部互斥*/
extern struct semaphore     sem_fopen;
extern struct semaphore     sem_fclose;
extern struct semaphore     sem_fread;
extern struct semaphore     sem_fwrite;
extern struct semaphore     sem_fseek;
extern struct semaphore     sem_remove;
extern struct semaphore     sem_ftell;
extern struct semaphore     sem_rename;
extern struct semaphore     sem_access;
extern struct semaphore     sem_mkdir;
extern struct semaphore     sem_rmdir;
extern struct semaphore     sem_opendir;
extern struct semaphore     sem_readdir;
extern struct semaphore     sem_closedir;
extern struct semaphore     sem_stat;
extern struct semaphore     sem_xcopy;
extern struct semaphore     sem_xdelete;
extern struct semaphore     sem_mass_read;
extern struct semaphore     sem_mass_write;
/*****************************************************************************
  3 函数实现
*****************************************************************************/
void debug_print_mnt_header(RFILE_MNTN_HEADER *mntn_h)
{
    printk(KERN_ERR "mntn header:\n");
    printk(KERN_ERR "opsPid %d\n",mntn_h->opsPid);
    printk(KERN_ERR "totalFrame %d\n",mntn_h->totalFrame);
    printk(KERN_ERR "curFrame %d\n",mntn_h->curFrame);
    printk(KERN_ERR "curlen %d\n",mntn_h->curlen);
    printk(KERN_ERR "reserve %x\n",mntn_h->reserve);
}

void debug_rfile_set_level(int val)
{
    g_rfile_dbg_lvl = val;
}


void rfile_regProxyFunc(void)
{
    /*将处理函数注册到函数指针数组中去*/
    rfileOpsSet[FOPEN]          = fopenProxy;
	rfileOpsSet[FOPEN_RESP]     = NULL;
    rfileOpsSet[FSEEK]          = fseekProxy;
    rfileOpsSet[FSEEK_RESP]     = NULL;
    rfileOpsSet[FREAD]          = freadProxy;
	rfileOpsSet[FREAD_RESP]     = NULL;
    rfileOpsSet[FWRITE]         = fwriteProxy;
	rfileOpsSet[FWRITE_RESP]    = NULL;
    rfileOpsSet[FCLOSE]         = fcloseProxy;
	rfileOpsSet[FCLOSE_RESP]    = NULL;
    rfileOpsSet[REMOVE]         = fremoveProxy;
	rfileOpsSet[REMOVE_RESP]    = NULL;
    rfileOpsSet[FTELL]          = ftellProxy;
	rfileOpsSet[FTELL_RESP]     = NULL;
	rfileOpsSet[FMKDIR]         = fmkdirProxy;
	rfileOpsSet[FMKDIR_RESP]    = NULL;
	rfileOpsSet[FRMDIR]         = frmdirProxy;
	rfileOpsSet[FRMDIR_RESP]    = NULL;
	rfileOpsSet[FOPENDIR]       = fopendirProxy;
	rfileOpsSet[FOPENDIR_RESP]  = NULL;
	rfileOpsSet[FREADDIR]       = freaddirProxy;
	rfileOpsSet[FREADDIR_RESP]  = NULL;
	rfileOpsSet[FCLOSEDIR]      = fclosedirProxy;
	rfileOpsSet[FCLOSEDIR_RESP] = NULL;
	rfileOpsSet[FSTAT]          = fstatdirProxy;
	rfileOpsSet[FSTAT_RESP]     = NULL;
	rfileOpsSet[FACCESS]        = faccessProxy;
	rfileOpsSet[FACCESS_RESP]   = NULL;
	rfileOpsSet[FXCOPY]         = fxcopydirProxy;
	rfileOpsSet[FXCOPY_RESP]    = NULL;
	rfileOpsSet[FXDELETE]       = fxdeletedirProxy;
	rfileOpsSet[FXDELETE_RESP]  = NULL;
	rfileOpsSet[FRENAME]        = frenameProxy;
	rfileOpsSet[FRENAME_RESP]   = NULL;
	rfileOpsSet[FMASS_READ]     = fmassreadProxy;
	rfileOpsSet[FMASS_READ_RESP]    = NULL;
	rfileOpsSet[FMASS_WRITE]    = fmasswriteProxy;
	rfileOpsSet[FMASS_WRITE_RESP]   = NULL;

}
int  rfile_write_ccpu_pkt(int opspid,char *send_buf,unsigned int send_len)
{

    int size = 0;

    /*调用ICC的发送，将响应消息发给CCPU*/
    size = mdrv_udi_write(rFileContext_a.rFileIccFd,send_buf,send_len);
	if (size != send_len)
	{
        printk(KERN_ERR "ICC write failed : the size is %d\n",size);
        return RFILE_ERROR;
	}

    return RFILE_OK;
}
int rfile_read_ccpu_data(char* buf,unsigned int len)
{
    memcpy(buf,rFileContext_a.sendBuf,len);

    return RFILE_OK;
}



int rfile_read_ccpu_pkt( RFILE_MNTN_HEADER *mntn_header )
{
	int ret                 = -1;

	/*读取头信息*/
    ret = rfile_read_ccpu_data((char *)mntn_header,sizeof(RFILE_MNTN_HEADER));

    if (mntn_header->reserve != MNT_H_MAGIC)
    {
        printk(KERN_ERR "wrong magic num %x\n",mntn_header->reserve);
        debug_print_mnt_header(mntn_header);
        ret = RFILE_ERROR;
    }

    return ret;
}
int rfile_calc_read_frame_num(int send_size)
{
    int i, j;
    int frame_header_len = sizeof(RFILE_MNTN_HEADER)+ sizeof(RFILE_READ_HEAD);
    i= send_size/(BUF_LEN - frame_header_len);
    j= send_size%(BUF_LEN - frame_header_len);

    return (i + (j ? 1:0));
}




int rfile_calc_massread_frame_num(int send_size)
{
    int i, j;
    int frame_header_len = sizeof(RFILE_MNTN_HEADER)+ sizeof(RFILE_MASSREAD_HEAD);
    i= send_size/(BUF_LEN - frame_header_len);
    j= send_size%(BUF_LEN - frame_header_len);

    return (i + (j ? 1:0));
}


/******************************************************************************
*
* __sflags - return flags for a given mode.
*
* Return the (stdio) flags for a given mode.  Store the flags
* to be passed to an open() syscall through *optr.
* Return 0 on error.
*
* INCLUDE: stdio.h
*
* RETURNS: flags or ZERO
* NOMANUAL
*/

int rfile_sflags(char *	mode,int *	optr, int* appendflag)
{
    int ret;
    int m;
    int o;

    switch (*mode++)
	{
	case 'r':				/* open for reading */
		ret = __SRD;
		m = O_RDONLY;
		o = 0;
		break;

	case 'w':				/* open for writing */
		ret = __SWR;
		m = O_WRONLY;
		o = O_CREAT | O_TRUNC;
		break;

	case 'a':				/* open for appending */
		ret = __SWR;
		m = O_WRONLY;
		o = O_CREAT | O_APPEND;
		*appendflag = 1;
		break;

	default:				/* illegal mode */
		return (0);
	}

    /* [rwa]\+ or [rwa]b\+ means read and write */

    if ((*mode == '+') || (*mode == 'b' && mode[1] == '+'))
	{
	ret = __SRW;
	m = O_RDWR;
	}

    *optr = m | o| O_CLOEXEC;

	return (ret);
}
int rfile_xcopy_file(char* source_file,char* dest_file)
{
    int read_len   = 0;
    int writen_len = 0;
    int ret        = 0;
    int fd_orig;
    int fd;
    int fs;
     struct stat st;

    char *buf   = NULL;
    char *p_buf = NULL;

    buf = (char *)kmalloc(WR_BUFF_SIZE, GFP_KERNEL);
    if (!buf)
    {
        printk(KERN_ERR "rfile_xcopy_file: malloc fail!\n" );
        return RFILE_ERROR;
    }

    fd_orig = sys_open(dest_file, O_RDWR, 0);
    if(fd_orig >= 0)/*文件存在*/
    {
        /*关闭后删除*/
        ret = sys_close(fd_orig);
        if(ret < 0)
        {
            printk(KERN_INFO "rfile_xcopy_file:the sys_close ops fail, dest_file:%s, the ret is %x\n", dest_file, ret);
            kfree(buf);
            return RFILE_ERROR;
        }


        ret = sys_unlink(dest_file);
        if(ret < 0)
        {
            printk(KERN_INFO "rfile_xcopy_file:the sys unlink is fail, dest_file:%s, the ret is %x\n", dest_file, ret);
            kfree(buf);
            return RFILE_ERROR;
        }
    }

    if ((ret=sys_newlstat(source_file, &st)) < 0)
    {
        printk(KERN_INFO "rfile_xcopy_file:stat is fail,the pathname is %s,the ret is %x\n",source_file,ret);
        kfree(buf);
        return RFILE_ERROR;
    }

    /*创建目标文件*/
    fd = sys_open(dest_file, O_RDWR|O_CREAT, st.st_mode);
    if(fd < 0)
    {
        printk(KERN_INFO "rfile_xcopy_file: fd sys open fail!, dest_file:%s, the fd is %x\n", dest_file, fd);
        kfree(buf);
        return RFILE_ERROR;
    }

    fs = sys_open(source_file, O_RDWR, 0);
    if(fs < 0)
    {
        printk(KERN_INFO "rfile_xcopy_file: fs sys open fail!, source_file:%s, the fs is %x\n", source_file, fs);
        kfree(buf);
        sys_close(fd);
        return RFILE_ERROR;
    }
#ifdef _DRV_LLT_
    read_len = sys_read(fs,buf,WR_BUFF_SIZE);
#else
    while((read_len = sys_read(fs,buf,WR_BUFF_SIZE)))
#endif
    {
        if(read_len < 0)
        {
            printk(KERN_INFO "rfile_xcopy_file: fs sys read fail!, source_file:%s, the ret is %x\n", source_file, read_len);
            kfree(buf);
            sys_close(fd);
            sys_close(fs);
            return RFILE_ERROR;
        }

        if(read_len > 0)
        {
            p_buf = buf;

            while((writen_len = sys_write(fd,p_buf,read_len)))
            {
                if(read_len == writen_len)
                {
                    break;
                }
                else if(writen_len > 0)
                {
                    p_buf    += writen_len;
                    read_len -= writen_len;
                }

                if(writen_len < 0)
                {
                    printk(KERN_INFO "rfile_xcopy_file:fd sys_write error, dest_file:%s, the writen_len is %x \n", dest_file, writen_len);
                    kfree(buf);
                    sys_close(fd);
                    sys_close(fs);
                    return RFILE_ERROR;
                }
            }
        }
    }

    kfree(buf);

    ret = sys_fsync(fd);
    if(ret < 0)
    {
        printk(KERN_INFO "rfile_xcopy_file:sys_fsync fail, dest_file:%s, the ret is %x\n", dest_file, ret);
        sys_close(fd);
        sys_close(fs);
        return RFILE_ERROR;
    }

    sys_close(fd);
    sys_close(fs);

    printk(KERN_INFO "rfile_xcopy_file: OK! \n");

    return RFILE_OK;

}
int rfile_get_effectblk_addr(int fd,int usr_logic_blk,int blksize)
{
    /*开始读数据的物理起始地址*/


    /*坏块标记*/
    int bad_blk;

    /*非坏块累计变量*/
    int good_blk      = 0;

    long long  pos = sys_lseek(fd, 0, DRV_SEEK_CUR);

    while(good_blk < usr_logic_blk )
    {
        sys_lseek(fd, pos, SEEK_SET);

        bad_blk = sys_ioctl(fd, MEMGETBADBLOCK, (unsigned long )&pos);
        if(bad_blk < 0)
        {
            printk(KERN_INFO "rfile_get_effectblk_addr: sys ioctl fail,the ret is %x\n",bad_blk);

            return RFILE_ERROR;
        }

        if(bad_blk)
        {
           ;
        }
        else /*如果为非坏块，逻辑块数增加*/
        {
            good_blk++;
        }

        pos += blksize;
    }

    return pos-blksize;

}



int rfile_mass_write(int fd,char* write_buf,unsigned int blksize,unsigned int erase_addr)
{
    int ret = 0;
    struct erase_info_user erase_info = {0};

    erase_info.start  = erase_addr;
    erase_info.length = blksize;

    ret = sys_ioctl(fd, MEMERASE, (unsigned long)&erase_info);
    if(ret < 0)
    {
        printk(KERN_INFO "rfile_mass_write: sys erase  blk fail \n");

        return RFILE_ERROR;
    }

    RFILE_DEBUGPL(0x1,"rfile_mass_write: the erase_addr is %x \n",erase_addr);

    ret = sys_lseek(fd, erase_addr, SEEK_SET);
    if(ret<0)
    {
        printk(KERN_INFO "rfile_mass_write:the sys_lseek ops is fail,the ret is %x\n",ret);

        return RFILE_ERROR;
    }

    ret = sys_write(fd, write_buf, blksize);
    if(ret < 0)
    {
        printk(KERN_INFO "rfile_mass_write: sys write  blk fail,the ret is %x\n",ret);

        return RFILE_ERROR;
    }

    ret = sys_fsync(fd);
    if(ret < 0)
    {
        printk(KERN_INFO "rfile_mass_write:sys_fsync fail,the ret is %x\n",ret);
    }

    return RFILE_OK;

}



int rfile_get_massread_op_para(char                *buf,
                                     char                *partition_name,
                                     int                 *offset,
                                     int                 *length,
                                     RFILE_MASSREAD_HEAD *pmassread_head)
{
    if((NULL == buf)||(NULL == partition_name)||(NULL == offset)||(NULL == length)||(NULL == pmassread_head))
    {
        printk(KERN_INFO "rfile_get_massread_op_para: input para fial,buf= 0x%p,partition_name=0x%p \n",buf,partition_name);

        return RFILE_ERROR;
    }

    /*获取操作文件名*/
    strncpy(partition_name,buf+sizeof(RFILE_MASSREAD_HEAD),pmassread_head->partition_name_len);

    /*获取操作文件偏移量*/
    strncpy((char*)offset,buf+sizeof(RFILE_MASSREAD_HEAD)+pmassread_head->partition_name_len,sizeof(int));

    /*获取操作文件长度*/
    strncpy((char*)length,buf+sizeof(RFILE_MASSREAD_HEAD)+pmassread_head->partition_name_len+sizeof(int),sizeof(int));

    return RFILE_OK;
}
int rfile_get_masswrite_op_para(char                 *buf,
                                      char                 *partition_name,
                                      int                  *offset,
                                      int                  *length,
                                      RFILE_MASSWRITE_HEAD *pmasswrite_head)
{
    if((  NULL == buf)
       ||(NULL == partition_name)
       ||(NULL == offset)
       ||(NULL == length)
       ||(NULL == pmasswrite_head))
    {
        printk(KERN_INFO "rfile_get_massread_op_para: input para fial,buf= 0x%p,partition_name=0x%p \n",buf,partition_name);
        return RFILE_ERROR;
    }

    /*获取操作文件名*/
    strncpy(partition_name,buf+sizeof(RFILE_MASSREAD_HEAD),pmasswrite_head->partition_name_len);

    /*获取操作文件偏移量*/
    strncpy((char*)offset,buf+sizeof(RFILE_MASSREAD_HEAD)+pmasswrite_head->partition_name_len,sizeof(int));

    /*获取操作文件长度*/
    strncpy((char*)length,buf+sizeof(RFILE_MASSREAD_HEAD)+pmasswrite_head->partition_name_len+sizeof(int),sizeof(int));

    return RFILE_OK;
}
int rfile_mass_read_all(int                  fd,
                           int                  length,
                           char                *buffer_temp,
                           char                *buffer,
                           RFILE_MASS_RW_PARA   read_blk_para,
                           struct mtd_info_user mtd_info)
{
    int bad;
    int ret;
    int blkcount = 0;

    /*读数据起始地址 单位byte*/
    long long rd_addr = read_blk_para.physical_offset;
    long long beg     = read_blk_para.physical_offset;

    if((NULL == buffer_temp)||(NULL == buffer))
    {
       printk(KERN_INFO "rfile_mass_read_all: input para is fail,%p,%p \n",buffer_temp,buffer);

       return RFILE_ERROR;
    }

    /*开始逐块读取数据*/
    while(1)
    {
       /*如果超出总大小，跳出*/
       if(rd_addr >= mtd_info.size)
       {
           printk(KERN_ERR "rfile_mass_read_all: rd size is over \n");

           return RFILE_ERROR;
       }

       /*坏块判断，如果为坏块跳过，并且移动文件指针*/
       bad = sys_ioctl(fd, MEMGETBADBLOCK, (unsigned long)&rd_addr);
       if(bad)
       {
           beg += (mtd_info.erasesize);
           sys_lseek(fd, beg, SEEK_SET);
           rd_addr += (mtd_info.erasesize);
           continue;
       }

       beg += (mtd_info.erasesize);

       /*在一个blk内*/
       if(0 == (read_blk_para.blk_end - read_blk_para.blk_begin))
       {
           ret = sys_read(fd, buffer_temp,(mtd_info.erasesize));
           if(ret < 0)
           {
               printk(KERN_INFO "rfile_mass_read_all: sys read in one blk fail,the ret is %x\n",ret);

               return RFILE_ERROR;
           }

           memcpy(buffer,buffer_temp+read_blk_para.first_blk_offset,length);

           break;
       }

       /*跨blk*/
       /*最后一个blk，根据是否为blk整数倍，拷贝剩余page数据*/
       if(blkcount == (read_blk_para.blk_end - read_blk_para.blk_begin))
       {
           ret = sys_read(fd, buffer_temp, (mtd_info.erasesize));
           if(ret < 0)
           {
               printk(KERN_INFO "rfile_mass_read_all: sys read last blk fail,the ret is %x\n",ret);

               return RFILE_ERROR;
           }

           memcpy(buffer,buffer_temp,read_blk_para.last_blk_offset);

           break;

       }
       else if(blkcount == 0)/*第一个*/
       {
           ret = sys_read(fd, buffer_temp,(mtd_info.erasesize));
           if(ret < 0)
           {
               printk(KERN_INFO "rfile_mass_read_all: sys read first blk fail,the ret is %x\n",ret);

               return RFILE_ERROR;
           }

           memcpy(buffer,buffer_temp+read_blk_para.first_blk_offset,(mtd_info.erasesize-read_blk_para.first_blk_offset));

           buffer  += (mtd_info.erasesize-read_blk_para.first_blk_offset);

           rd_addr += (mtd_info.erasesize);

           blkcount++;
       }
       else/*中间blk*/
       {
           ret = sys_read(fd, buffer, (mtd_info.erasesize));
           if(ret < 0)
           {
               printk(KERN_INFO "rfile_mass_read_all: sys read during blk fail blkcout=%x,rd_addr= 0x%llx ,the ret is %x\n",blkcount,rd_addr,ret);

               return RFILE_ERROR;
           }

           buffer += (mtd_info.erasesize);

           rd_addr += (mtd_info.erasesize);

           blkcount++;
       }

       sys_lseek(fd, beg, SEEK_SET);

    }

    return RFILE_OK;
}
int rfile_mass_write_all(int                 fd,
                           int                  length,
                           char                *buffer,
                           char                *buffer_temp,
                           RFILE_MASS_RW_PARA   write_blk_para,
                           struct mtd_info_user mtd_info)
{
    int bad;
    int ret = -1;
    int blkcount = 0;

    long long rd_addr = write_blk_para.physical_offset;
    long long beg     = write_blk_para.physical_offset;

    if((NULL == buffer_temp)||(NULL == buffer))
    {
       printk(KERN_INFO "rfile_mass_write_all: input para is fail,%p,%p \n",buffer_temp,buffer);

       return RFILE_ERROR;
    }

    /*开始逐块读取数据*/
    while(1)
    {

       /*如果超出总大小，跳出*/
       if(rd_addr >= mtd_info.size)
       {
           printk(KERN_ERR "rfile_mass_write_all: wd size is over \n");

           return RFILE_ERROR;
       }

       /*坏块判断，如果为坏块跳过，并且移动文件指针*/
       bad = sys_ioctl(fd, MEMGETBADBLOCK, (unsigned long)(&rd_addr));
       if(bad)
       {
           printk(KERN_INFO "rfile_mass_write_all: bad blk no is %llx \n",rd_addr);
           beg += (mtd_info.erasesize);
           sys_lseek(fd, beg, SEEK_SET);
           rd_addr += (mtd_info.erasesize);
           continue;
       }

       beg += (mtd_info.erasesize);

       /*在一个blk内*/
       if(0 == (write_blk_para.blk_end-write_blk_para.blk_begin))
       {
           ret = sys_read(fd, buffer_temp, (mtd_info.erasesize));
           if(ret < 0)
           {
               printk(KERN_INFO "rfile_mass_write_all: sys read one blk fail,the ret is %x\n",ret);

               return RFILE_ERROR;
           }

            ret = sys_lseek(fd, 0, DRV_SEEK_CUR);
            if(ret<0)
            {
                printk(KERN_INFO "rfile_mass_write_all:the ftell is error,the ret is %x\n",ret);

                ret = RFILE_ERROR;
            }

           printk(KERN_INFO "rfile_mass_write_all:the ftell is %x\n",ret);

           memcpy(buffer_temp+write_blk_para.first_blk_offset,buffer,length);

           ret = rfile_mass_write(fd,buffer_temp,mtd_info.erasesize,rd_addr);
           if(ret < 0)
           {
               printk(KERN_ERR "rfile_mass_write_all: sys write one blk fail \n");

               return RFILE_ERROR;
           }

           break;
       }

       /*跨blk*/
       /*最后一个blk，根据是否为blk整数倍，拷贝剩余page数据*/
       if(blkcount == (write_blk_para.blk_end-write_blk_para.blk_begin))
       {
           ret = sys_read(fd, buffer_temp, (mtd_info.erasesize));
           if(ret < 0)
           {
               printk(KERN_INFO "rfile_mass_write_all: sys read last blk fail,the ret is %x\n",ret);

               return RFILE_ERROR;
           }

           memcpy(buffer_temp,buffer,write_blk_para.last_blk_offset);

           ret = rfile_mass_write(fd,buffer_temp,mtd_info.erasesize,rd_addr);
           if(ret < 0)
           {
               printk(KERN_ERR "rfile_mass_write_all: sys write last blk fail \n");

               return RFILE_ERROR;
           }

           break;

       }
       else if(blkcount == 0)/*第一个*/
       {
           ret = sys_read(fd, buffer_temp, (mtd_info.erasesize));
           if(ret < 0)
           {
               printk(KERN_INFO "rfile_mass_write_all: sys read first blk fail,the ret is %x\n",ret);

               return RFILE_ERROR;
           }

          memcpy(buffer_temp+write_blk_para.first_blk_offset,buffer,((mtd_info.erasesize)-write_blk_para.first_blk_offset));

          /*疑问需要验证 y00145322*/
          sys_lseek(fd, (beg-mtd_info.erasesize), SEEK_SET);

           ret = rfile_mass_write(fd,buffer_temp,mtd_info.erasesize,rd_addr);
           if(ret < 0)
           {
               printk(KERN_ERR "rfile_mass_write_all: sys write first blk fail \n");

               return RFILE_ERROR;
           }

          rd_addr += (mtd_info.erasesize);

          blkcount++;

       }
       else
       {
           ret = rfile_mass_write(fd,buffer_temp,mtd_info.erasesize,rd_addr);
           if(ret < 0)
           {
               printk(KERN_ERR "rfile_mass_write_all: sys write during blk fail,blkcount=%x,rd_addr=0x%llx \n",blkcount,rd_addr);

               return RFILE_ERROR;
           }

           buffer += (mtd_info.erasesize);

           rd_addr += (mtd_info.erasesize);

           blkcount++;
       }

       sys_lseek(fd, rd_addr, SEEK_SET);

    }

    return RFILE_OK;
}



int rfile_calc_begin_blk(int offset,int blksize)
{
    return (offset/blksize)+1;
}



int rfile_get_rw_info(RFILE_MASS_RW_PARA *rw_blk_info, int offset,int blksize,int total_len,long long phy_blk_beg)
{
       rw_blk_info->blk_begin        = (offset/blksize) + 1;
       rw_blk_info->blk_end          = (total_len%blksize) > 0 ?(total_len/blksize)+1:(total_len/blksize);
       rw_blk_info->first_blk_offset =  offset%blksize;
       rw_blk_info->last_blk_offset  = (total_len%blksize) > 0 ?(total_len%blksize):blksize;
       rw_blk_info->physical_offset  =  phy_blk_beg;

       return RFILE_OK;
}



int rfile_num2string(int num,char *string)
{

    int power;
    int i;

    i = num;

    if(NULL == string)
    {
        printk(KERN_ERR "rfile_num2string: string is null\n");
	 return RFILE_ERROR;
    }

    for(power =1;i>=10;i/=10)
    power*=10;

    for(;power>0;power/=10)
    {
       *string++ = '0' + num/power;
       num%=power;
    }

    *string = '\0';

	return RFILE_OK;
}



int rfile_xdelete(const char* pathname)
{
    struct stat st;
    struct linux_dirent* read_dirent = NULL;
    char *dn = NULL;

    int ret  = 0;
    int fd;
    int fail = 0;

    /*当层目录检查*/
    if(NULL == pathname)
    {
        printk(KERN_INFO "rfile_xdelete:the input is null\n");

        return RFILE_ERROR;
    }

    /* is it a file or directory? */
    if ((ret=sys_newlstat(pathname, &st)) < 0)
    {
        printk(KERN_INFO "rfile_xdelete:stat is fail,the pathname is %s,the ret is %x\n",pathname,ret);
        return RFILE_ERROR;
    }

    /* a file, so unlink it */
    if (!S_ISDIR(st.st_mode))
        return sys_unlink(pathname);

    read_dirent = (struct linux_dirent*)kmalloc(sizeof(struct linux_dirent_rfile), GFP_KERNEL);
    if(NULL == read_dirent)
    {
        printk(KERN_ERR "rfile_xdelete: read dirent malloc is null\n");
        return RFILE_ERROR;
    }

    dn = kmalloc(PATH_MAX, GFP_KERNEL);
    if(NULL == dn)
    {
        printk(KERN_ERR "rfile_xdelete: dn malloc is null\n");
        kfree(read_dirent);
        return RFILE_ERROR;
    }

    memset(read_dirent,0,sizeof(struct linux_dirent_rfile));

    memset(dn,0,PATH_MAX);

    fd = sys_open(pathname, O_RDONLY | O_DIRECTORY, 0);
    if(fd < 0)
    {
        printk(KERN_INFO "rfile_xdelete: sys_open op fail,the ret is %x\n",fd);
        kfree(read_dirent);
        kfree(dn);
        return RFILE_ERROR;
    }

    while((ret = sys_old_readdir(fd,(struct old_linux_dirent __user *) read_dirent,sizeof(struct linux_dirent)))!= 0)
    {
      if(!strcmp(read_dirent->d_name,"..")||!strcmp(read_dirent->d_name,"."))
         continue;

         snprintf(dn,PATH_MAX,"%s/%s", pathname, read_dirent->d_name);
         if(rfile_xdelete(dn)<0)
         {
             printk(KERN_ERR "rfile_xdelete: rfile_xdelete fail\n");
             fail = 1;
             break;
         }
    }

    if(fail)
    {
        sys_close(fd);
        kfree(read_dirent);
        kfree(dn);
        printk(KERN_INFO "rfile_xdelete: fail\n");
        return RFILE_ERROR;
    }

    if(sys_close(fd) < 0)
    {
       printk(KERN_INFO "rfile_xdelete: sys_close op fail\n");
       kfree(read_dirent);
       kfree(dn);
       return RFILE_ERROR;
    }

    kfree(read_dirent);

    kfree(dn);

    return sys_rmdir(pathname);

}

/*
 * check whether the fp has been opened
 * return vaule: 1, the fp is normal
 *               0, the fp is invalid
 */
void* fp_check_file(int fp_hashed)
{
    struct list_head *p;
    fp_list_t *tmp;

    list_for_each(p,&(fp_list_file.list)) {
        tmp = list_entry(p,fp_list_t,list);
        if (tmp->fp_hashed == fp_hashed)
                return tmp->fp_v;
    }
    return NULL;
}

int get_hashed_file_fp(void* fp_v)
{
	int hashed_fp = ((long)fp_v & 0xffffffff);   /*(int) (fp_v)*/
	while(!hashed_fp || fp_check_file(hashed_fp))
	{
		hashed_fp++;
	}
	return hashed_fp;
}

void fp_list_del_file(int fp_hashed)
{
    struct list_head *p, *n;
    fp_list_t *tmp;

    list_for_each_safe(p,n,&(fp_list_file.list)) {
        tmp = list_entry(p,fp_list_t,list);
        if (tmp->fp_hashed == fp_hashed)
        {
                list_del(&tmp->list);
                kfree(tmp);
        }
    }
}

int fp_list_add_file(void* fp_v)
{
        fp_list_t *fp_elemt;
        fp_elemt=(fp_list_t *)kmalloc((sizeof(fp_list_t)), GFP_KERNEL);
        if(fp_elemt == NULL)
        {
            printk(KERN_ERR "malloc fp_elemt failed\n");
            return 0;
        }
        fp_elemt->fp_v=fp_v;
        fp_elemt->fp_hashed=get_hashed_file_fp(fp_v);
        list_add(&(fp_elemt->list),&(fp_list_file.list));
        return fp_elemt->fp_hashed;
}

/*
 * check whether the fp has been opened
 * return vaule: 1, the fp is normal
 *               0, the fp is invalid
 */
void* fp_check_dir(int fp_hashed)
{
    struct list_head *p;
    fp_list_t *tmp;

    list_for_each(p,&(fp_list_dir.list)) {
        tmp = list_entry(p,fp_list_t,list);
        if (tmp->fp_hashed== fp_hashed)
                return tmp->fp_v;
    }
    return NULL;
}

int get_hashed_dir_fp(void* fp_v)
{
	int hashed_fp = ((long)fp_v & 0xffffffff);		/*(int)fp_v */
	while(!hashed_fp || fp_check_dir(hashed_fp))
	{
		hashed_fp++;
	}
	return hashed_fp;
}

void fp_list_del_dir(int fp_hashed)
{
    struct list_head *p, *n;
    fp_list_t *tmp;

    list_for_each_safe(p,n,&(fp_list_dir.list)) {
        tmp = list_entry(p,fp_list_t,list);
        if (tmp->fp_hashed== fp_hashed)
        {
                list_del(&tmp->list);
                kfree(tmp);
        }
    }
}

int fp_list_add_dir(void* fp_v)
{
        fp_list_t *fp_elemt;
        fp_elemt=(fp_list_t *)kmalloc((sizeof(fp_list_t)), GFP_KERNEL);
        if(fp_elemt == NULL)
        {
            printk(KERN_ERR "malloc fp_elemt failed\n");
            return 0;
        }
        fp_elemt->fp_v=fp_v;
        fp_elemt->fp_hashed=get_hashed_dir_fp(fp_v);
        list_add(&(fp_elemt->list),&(fp_list_dir.list));
        return fp_elemt->fp_hashed;

/*
 * check whether the fp has been opened
 * return vaule: 1, the fp is normal
 *               0, the fp is invalid
 */

}

/*Modify_for_c_reset, l00212112,20130511, starts*/

int bsp_ccpu_rest_rfile_handle(void)
{

    int ret = 0;
    struct list_head *p = 0;
    struct list_head *n = 0;
    fp_list_t *tmp;
    /*close dir or files*/

    list_for_each_safe(p,n,&(fp_list_dir.list))
    {
        tmp = list_entry(p,fp_list_t,list);
        ret = bsp_linux_closedir((DRV_DIR_S*)(tmp->fp_v));
        if(ret != RFILE_OK)
        {
            printk(KERN_ERR "the dir close fail.\n");
        }

        list_del(&tmp->list);
        kfree((const void*)tmp);
    }

    list_for_each_safe(p,n,&(fp_list_file.list))
    {
        tmp = list_entry(p,fp_list_t,list);
        ret = bsp_linux_fclose((RFILE*)(tmp->fp_v));
        if(ret != RFILE_OK)
        {
            printk(KERN_ERR "the file close fail.\n");
        }
        list_del(&tmp->list);
        kfree((const void*)tmp);
    }

    return RFILE_OK;
}
/*Modify_for_c_reset, l00212112,20130511, ends*/


int fopenProxy(RFILE_MNTN_HEADER *mnt_h, char * data)
{
    /*默认失败*/
    int                  ret            = -1;
    char                *filename       = NULL;
    char                *mode           = NULL;
    RFILE                *fp             = NULL;
    RFILE_RESP_OPEN_MSG  res_open_msg   = {0};
    RFILE_OPEN_HEAD     *popen_head     = (RFILE_OPEN_HEAD *)data;
    RFILE_MNTN_HEADER   *mntn_header    = NULL;

    unsigned int         expect_cur     = 0;

    unsigned int         total_send_len = 0;
    int                  hashed_fp = 0;

    /*内存清零*/
    memset(rw_buf,0,BUF_LEN);

    /*可维可测信息*/
    mntn_header             =(RFILE_MNTN_HEADER *)rw_buf;
    mntn_header->opsPid     = mnt_h->opsPid;
    mntn_header->totalFrame = 1;
    mntn_header->curFrame   = 1;
    mntn_header->curlen     = sizeof(RFILE_RESP_OPEN_MSG);
    mntn_header->reserve    = MNT_H_MAGIC;

    /*可维可测信息*/
    if(mnt_h->curFrame != (++expect_cur))
    {
        printk(KERN_ERR "fopenProxy:error expect cur frame is %d, realy cur frame is %d\n",expect_cur,mnt_h->curFrame);

        return RFILE_ERROR;
    }

    /*入参判断，防止数据异常*/
    if(((popen_head->pathLen) == 0) || ((popen_head->modeLen) == 0))
    {
        printk(KERN_INFO "fopenProxy: pathlen %x or modelen %x fail\n",popen_head->pathLen,popen_head->modeLen);
        goto SEND_RESPONSE;
    }

    /*路径名*/
    filename = (char *)kmalloc((popen_head->pathLen),GFP_KERNEL);
    if (filename == NULL)
    {
        printk(KERN_ERR "fopenProxy:alloc path mem fail\n");
        goto SEND_RESPONSE;
    }

    strncpy(filename,data+sizeof(RFILE_OPEN_HEAD),popen_head->pathLen);

    /*模式字符*/
    mode=(char *)kmalloc((popen_head->modeLen),GFP_KERNEL);
    if (mode == NULL)
    {
        printk(KERN_ERR "fopenProxy:alloc mode mem fail\n");
        goto SEND_RESPONSE;
    }

    strncpy(mode,(data+sizeof(RFILE_OPEN_HEAD)+ popen_head->pathLen),popen_head->modeLen);

    RFILE_DEBUGPL(0x3,"fopenProxy: fpath is %s, mode is %s \n",filename,mode);
    
#if defined (CONFIG_HISILICON_PLATFORM_POWER_CONTROL)
    if(g_pwcAcpuWakeFlagRfile)
    {
        g_pwcAcpuWakeFlagRfile = 0;
        printk(KERN_ERR "%s:%d:rfile wakeup fpath: %s, mode is %s \n", __func__, __LINE__, filename, mode);
    }
#endif

    /*考虑到LINUX sys_open返回的值为一个整形数,当前在bsp_linux_fopen函数中进行封装，返回FILE**/
    if(NULL != rfile_linux_ops.bsp_fopen)
    {
        fp = (RFILE *)(rfile_linux_ops.bsp_fopen)(filename,mode);
    }
    else
    {
        printk(KERN_ERR "fopenProxy:fopen unregister \n");
        goto SEND_RESPONSE;
    }

    if(fp != 0)
    {
        hashed_fp = fp_list_add_file(fp);
    }


SEND_RESPONSE:

    res_open_msg.opsType = popen_head->opsType +1;

    /*把地址传输给CCPU,CCPU不访问该地址，只使用地址数*/
    res_open_msg.fp = hashed_fp;

    memcpy(rw_buf+sizeof(RFILE_MNTN_HEADER),&res_open_msg,sizeof(RFILE_RESP_OPEN_MSG));

    total_send_len = sizeof(RFILE_MNTN_HEADER)+sizeof(RFILE_RESP_OPEN_MSG) ;

    ret = rfile_write_ccpu_pkt(mnt_h->opsPid,rw_buf,total_send_len);

    if(filename!=NULL)
    {
        kfree(filename);
    }

    if(mode!=NULL)
    {
        kfree(mode);
    }

    return ret;
}
int fcloseProxy(RFILE_MNTN_HEADER *mnt_h, char * data)
{
    RFILE                           *fd  = NULL;
    int                              ret = -1;
    RFILE_RESP_CLOSE_MSG   res_close_msg = {0};
    RFILE_CLOSE_HEAD        *pclose_head = (RFILE_CLOSE_HEAD *) data;
    unsigned int              expect_cur = 0;
    unsigned int          total_send_len = 0;
    RFILE_MNTN_HEADER       *mntn_header = NULL;

    /*内存清零*/
    memset(rw_buf,0,BUF_LEN);

    mntn_header               = (RFILE_MNTN_HEADER *)rw_buf;
    mntn_header->opsPid       = mnt_h->opsPid;
    mntn_header->totalFrame   = mnt_h->totalFrame;
    mntn_header->curFrame     = 1;
    mntn_header->curlen       = sizeof(RFILE_RESP_CLOSE_MSG);
    mntn_header->reserve      = MNT_H_MAGIC;

    /*可维可测*/
    if(mnt_h->curFrame != (++expect_cur))
    {
        printk(KERN_ERR "fcloseProxy:error expect cur frame is %d, realy cur frame is %d\n",expect_cur,mnt_h->curFrame);

        return RFILE_ERROR;
    }

    /*入参判断*/
    if(0 == pclose_head->fp)
    {
        printk(KERN_INFO "fcloseProxy:the fp is null \n");
        goto SEND_RESPONSE;
    }

    /*fp为获取fopenprxy时创建的FILE结构体*/
    fd = fp_check_file(pclose_head->fp);

    /*获取fopen是*/
    RFILE_DEBUGPL(0x1,"fcloseProxy:the fs_Set is %p \n",fd);

    if (NULL == fd)/*改为NULL，NULL写前面*/
    {
        printk(KERN_ERR "fcloseProxy:fp check fail\n");

        goto SEND_RESPONSE;
    }


    if(NULL != rfile_linux_ops.bsp_fclose)
    {
        ret=(rfile_linux_ops.bsp_fclose)(fd);
        fp_list_del_file(pclose_head->fp);
    }
    else
    {
        printk(KERN_ERR "fcloseProxy:fclose unregister \n");
        goto SEND_RESPONSE;
    }

SEND_RESPONSE:

    res_close_msg.opsType = pclose_head->opsType+1;
    res_close_msg.ret     = ret;

    memcpy(rw_buf+sizeof(RFILE_MNTN_HEADER),&res_close_msg,sizeof(RFILE_RESP_CLOSE_MSG));

    total_send_len = sizeof(RFILE_MNTN_HEADER)+ sizeof(RFILE_RESP_CLOSE_MSG);

    ret = rfile_write_ccpu_pkt(mnt_h->opsPid,rw_buf,total_send_len);

    return ret;
}



int freadProxy(RFILE_MNTN_HEADER *mnt_h, char * data)
{
    int                           ret = 0;

    int                         rd_sz = 0;
    int                      head_len = 0;
    RFILE                         *fp = NULL;
    RFILE_RESP_READ_MSG  res_read_msg = {0};
    RFILE_READ_HEAD       *pread_head = (RFILE_READ_HEAD *) data;
    int                    totalframe = 0;
    char                *data_to_ccpu = rw_buf+sizeof(RFILE_MNTN_HEADER);
    int                total_send_len = 0;
    RFILE_MNTN_HEADER    *mntn_header = NULL;
    unsigned int           expect_cur = 0;

    /*可维可测信息*/
    if(mnt_h->curFrame != (++expect_cur))
    {
        printk(KERN_ERR "freadProxy:error expect cur frame is %d, realy cur frame is %d\n",expect_cur,mnt_h->curFrame);

        return RFILE_ERROR;
    }

    /*计算当前需要传输的次数*/
    totalframe = rfile_calc_read_frame_num(pread_head->size);

    /*内存清零*/
    memset(rw_buf,0,BUF_LEN);

    /*可维可测信息*/
    mntn_header             = (RFILE_MNTN_HEADER *)rw_buf;
    mntn_header->opsPid     = mnt_h->opsPid;
    mntn_header->totalFrame = totalframe;
    mntn_header->curFrame   = 0;
    mntn_header->reserve    = MNT_H_MAGIC;

    /*入参判断*/
    if(0 == pread_head->fp)
    {
        printk(KERN_INFO "freadProxy:the fp is null \n");

        return RFILE_ERROR;
    }

    if(NULL == rfile_linux_ops.bsp_fread)
    {
        printk(KERN_ERR "freadProxy:fread unregister \n");

        return RFILE_ERROR;
    }

    /*获取读数据的文件指针*/
    fp    = (RFILE *)fp_check_file(pread_head->fp);

    if (NULL == fp)/*NULL*/
    {
       printk(KERN_ERR "freadProxy:fp check fail\n");
	res_read_msg.opsType = pread_head->opsType +1;
	res_read_msg.fp      = pread_head->fp;
	res_read_msg.size    = -1;

	memcpy(rw_buf+sizeof(RFILE_MNTN_HEADER),&res_read_msg,sizeof(RFILE_RESP_READ_MSG));

	total_send_len = sizeof(RFILE_MNTN_HEADER)+ sizeof(RFILE_RESP_READ_MSG);

	ret = rfile_write_ccpu_pkt(mnt_h->opsPid,rw_buf,total_send_len);
	if(RFILE_OK != ret)
	{
	            printk(KERN_ERR "freadProxy:rfile_write_ccpu_pkt fail\n");
	}

        return RFILE_ERROR;
    }

    /*获取读数据的总长度*/
    rd_sz = pread_head->size;

    /*可维可测信息*/
    res_read_msg.opsType = pread_head->opsType +1;
    res_read_msg.fp      = pread_head->fp;

    /*每次读数据发送给CCPU的头信息*/
    head_len = sizeof(RFILE_MNTN_HEADER)+sizeof(RFILE_RESP_READ_MSG);

    while(totalframe--)
    {
        if (rd_sz > (BUF_LEN-head_len))
        {
            ret = (rfile_linux_ops.bsp_fread)(data_to_ccpu + sizeof(RFILE_RESP_READ_MSG),BUF_LEN-head_len,fp);
            if(ret != (BUF_LEN-head_len))
            {
                 printk(KERN_INFO "freadProxy: the union blk read %x is not equal what we want %x \n",ret,BUF_LEN-head_len);
            }

            res_read_msg.size = ret;

            /*每次读取完毕后，更新读取长度*/
            rd_sz-=(BUF_LEN-head_len);

            mntn_header->curFrame++;

            mntn_header->curlen = BUF_LEN - sizeof(RFILE_MNTN_HEADER);

            memcpy(rw_buf+sizeof(RFILE_MNTN_HEADER),&res_read_msg,sizeof(RFILE_RESP_READ_MSG));

            total_send_len = BUF_LEN;

            RFILE_DEBUGPL(0x1,"freadProxy: the read fp is %p ,the read len is %x, want size is %x \n",fp,mntn_header->curlen,rd_sz);

            ret = rfile_write_ccpu_pkt(mnt_h->opsPid,rw_buf,total_send_len);

            /*等待C核接收完毕，使用信号量*/
            down(&sem_to_read_req);

        }
        else
        {
            ret=(rfile_linux_ops.bsp_fread)(data_to_ccpu + sizeof(RFILE_RESP_READ_MSG),rd_sz,fp);
            if(ret != rd_sz)
            {
                 printk(KERN_INFO "freadProxy: the left blk read %x is not equal what we want %x \n",ret,rd_sz);
            }

            res_read_msg.size = ret;

            mntn_header->curFrame++;

            mntn_header->curlen = rd_sz + sizeof(RFILE_RESP_READ_MSG);

            memcpy(rw_buf+sizeof(RFILE_MNTN_HEADER),&res_read_msg,sizeof(RFILE_RESP_READ_MSG));

            total_send_len = rd_sz+sizeof(RFILE_MNTN_HEADER)+ sizeof(RFILE_RESP_READ_MSG);

            RFILE_DEBUGPL(0x1,"freadProxy: last frame the read fp is %p ,the read len is %x, want size is %x \n",fp,mntn_header->curlen,rd_sz);

            ret = rfile_write_ccpu_pkt(mnt_h->opsPid,rw_buf,total_send_len);
        }
    }

    return ret;

}



int fwriteProxy(RFILE_MNTN_HEADER *mnt_h, char * data)
{
    int                            ret = -1;
    int                          wt_sz = 0;
    RFILE                           *fp = NULL;
    RFILE_RESP_WRITE_MSG res_write_msg = {0};
    RFILE_WRITE_HEAD      *pwrite_head = (RFILE_WRITE_HEAD *) data;

    int total_send_len                 = 0;
    RFILE_MNTN_HEADER     *mntn_header = NULL;

    memset(rw_buf,0,BUF_LEN);

    mntn_header               = (RFILE_MNTN_HEADER *)rw_buf;
    mntn_header->opsPid       = mnt_h->opsPid;
    mntn_header->totalFrame   = mnt_h->totalFrame;
    mntn_header->curFrame     = 1;
    mntn_header->curlen       = sizeof(RFILE_RESP_WRITE_MSG);
    mntn_header->reserve      = MNT_H_MAGIC;

    res_write_msg.opsType    = pwrite_head->opsType+1;
    res_write_msg.fp         = pwrite_head->fp;
    wt_sz                    = pwrite_head->size; /*必须为<=BUF_LEN-可谓可测头-写文件头*/

    /*
    可维可测，基于CCPU写入的文件是同一个一次写
    不允许一次文件操作不结束操作另外一个文件
    */
    if(mnt_h->curFrame != (++g_rfile_wt_expect_cur))
    {
        printk(KERN_ERR "fwriteProxy:error expect cur frame is %d, realy cur frame is %d\n",g_rfile_wt_expect_cur,mnt_h->curFrame);
        g_rfile_wt_expect_cur = 0;
        return RFILE_ERROR;
    }

    /*入参判断*/
    if(0 == pwrite_head->fp)
    {
        printk(KERN_INFO "fwriteProxy:the fp is null \n");
        g_rfile_wt_expect_cur = 0;
        return RFILE_ERROR;
    }

    if(NULL == rfile_linux_ops.bsp_fwrite)
    {
        printk(KERN_INFO "fwriteProxy:fwrite unregister \n");
        g_rfile_wt_expect_cur = 0;
        return RFILE_ERROR;
    }

    if(wt_sz > BUF_LEN-sizeof(RFILE_MNTN_HEADER)-sizeof(RFILE_WRITE_HEAD))
    {
        printk(KERN_INFO "fwriteProxy:write data size is over \n");
        g_rfile_wt_expect_cur = 0;
        return RFILE_ERROR;
    }

    /*获取fopen时打开的文件操作符*/
    fp = (RFILE *)fp_check_file(pwrite_head->fp);

    if (NULL == fp)/**NULL*/
    {
        printk(KERN_ERR "fwriteProxy:fp check fail\n");
	g_rfile_wt_expect_cur = 0;

	res_write_msg.ret    = -1;

	memcpy(rw_buf+sizeof(RFILE_MNTN_HEADER),&res_write_msg,sizeof(RFILE_RESP_WRITE_MSG));

	total_send_len = sizeof(RFILE_MNTN_HEADER)+ sizeof(RFILE_RESP_WRITE_MSG);

	ret = rfile_write_ccpu_pkt(mnt_h->opsPid,rw_buf,total_send_len);
	if(RFILE_OK != ret)
	{
	        printk(KERN_ERR "fwriteProxy:rfile_write_ccpu_pkt fail\n");
	}

        return RFILE_ERROR;
    }

    if(mnt_h->curFrame < mnt_h->totalFrame)
    {
        /*需要判断当前写入模式，如果为非追加，需要偏移文件指针*/
        ret = (rfile_linux_ops.bsp_fwrite)(data+sizeof(RFILE_WRITE_HEAD),(unsigned int)wt_sz,(void*)(fp));
        if(wt_sz != ret)
        {
            printk(KERN_INFO "fwriteProxy: the union blk write %x is not equal what we want %x \n",ret,wt_sz);
        }

        g_fwrite_total_len = g_fwrite_total_len + wt_sz;

        res_write_msg.ret = g_fwrite_total_len;

        g_curr_frame_num++;

        mntn_header->curFrame = 1;

        RFILE_DEBUGPL(0x1,"fwriteProxy: the write fp is %p ,the write len is %x,the frame no is %x, totalfram is %x \n",fp,wt_sz,g_curr_frame_num,mnt_h->totalFrame);

        if(ret<0)
        {
            g_rfile_wt_expect_cur = 0;

            res_write_msg.ret = RFILE_ERROR;
            memcpy(rw_buf+sizeof(RFILE_MNTN_HEADER),&res_write_msg,sizeof(RFILE_RESP_WRITE_MSG));

            total_send_len = sizeof(RFILE_MNTN_HEADER)+sizeof(RFILE_RESP_WRITE_MSG);

            ret = rfile_write_ccpu_pkt(mnt_h->opsPid,(char *)rw_buf,total_send_len);
        }
        else
        {
            ret = 0;
        }

    }
    else
    {
        /*需要判断当前写入模式，如果为非追加，需要偏移文件指针*/
        ret= (rfile_linux_ops.bsp_fwrite)(data+sizeof(RFILE_WRITE_HEAD),(unsigned int)wt_sz,(void*)(fp));
        if(wt_sz != ret)
        {
            printk(KERN_INFO "fwriteProxy: the write size %x is not equal what we want %x \n",ret,wt_sz);
        }

        g_fwrite_total_len = g_fwrite_total_len + wt_sz;

        if(ret >=0)
        {
        res_write_msg.ret = g_fwrite_total_len;
        }
        else
        {
            res_write_msg.ret = RFILE_ERROR;
        }

        g_curr_frame_num++;

        mntn_header->curFrame = 1;
        mntn_header->totalFrame =1;

        /*最后一包后，总长度清零*/
        g_fwrite_total_len = 0;

        /*可维可测*/
        g_rfile_wt_expect_cur = 0;

        memcpy(rw_buf+sizeof(RFILE_MNTN_HEADER),&res_write_msg,sizeof(RFILE_RESP_WRITE_MSG));

        total_send_len = sizeof(RFILE_MNTN_HEADER)+sizeof(RFILE_RESP_WRITE_MSG) ;

        RFILE_DEBUGPL(0x1,"fwriteProxy: the write fp is %p ,the write len is %x, the last frmae no is %x, total frame is %x \n",fp,wt_sz,g_curr_frame_num,mnt_h->totalFrame);

         g_curr_frame_num = 0;

        ret = rfile_write_ccpu_pkt(mnt_h->opsPid,(char *)rw_buf,total_send_len);

    }

    if(mnt_h->totalFrame > 1)
    {
        rfile_a_give_c_sem();
    }

    return ret;
}
int fseekProxy(RFILE_MNTN_HEADER *mnt_h, char * data)
{
    int                     ret            = -1;
    RFILE                   *fp             = NULL;
    RFILE_RESP_SEEK_MSG     res_seek_msg   = {0};
    RFILE_SEEK_HEAD        *pseek_head     = (RFILE_SEEK_HEAD *)data;
    unsigned int            expect_cur     = 0;

    RFILE_MNTN_HEADER      *mntn_header    = NULL;
    int                     total_send_len = 0;

    memset(rw_buf,0,BUF_LEN);

    /*可维可测信息*/
    mntn_header             =(RFILE_MNTN_HEADER *)rw_buf;
    mntn_header->opsPid     = mnt_h->opsPid;
    mntn_header->totalFrame = 1;
    mntn_header->curFrame   = 1;
    mntn_header->curlen     = sizeof(RFILE_RESP_SEEK_MSG);
    mntn_header->reserve    = MNT_H_MAGIC;

    /*可维可测信息*/
    if(mnt_h->curFrame != (++expect_cur))
    {
        printk(KERN_ERR "fseekProxy:error expect cur frame is %d, realy cur frame is %d\n",expect_cur,mnt_h->curFrame);

        return RFILE_ERROR;
    }

    /*入参判断*/
    if(0 == pseek_head->fp)
    {
        printk(KERN_INFO "fseekProxy:the fp is null \n");
        goto SEND_RESPONSE;
    }

    if(NULL == rfile_linux_ops.bsp_fseek)
    {
        printk(KERN_INFO "fseekProxy:fseek unregister \n");
        goto SEND_RESPONSE;
    }

    /*获取fopen时返回的FILE**/
    fp=(RFILE *)fp_check_file(pseek_head->fp);

    if (NULL == fp)/*NULL*/
    {
        printk(KERN_ERR "fseekProxy:fp check fail\n");

        goto SEND_RESPONSE;
    }

    RFILE_DEBUGPL(0x1,"fseekProxy: fp is %p, offset is %x, whence is %x\n",fp,pseek_head->offset,pseek_head->whence);


    ret=(rfile_linux_ops.bsp_fseek)(fp,pseek_head->offset,pseek_head->whence);


SEND_RESPONSE:

    res_seek_msg.opsType = pseek_head->opsType+1;
    res_seek_msg.ret     = ret;

    memcpy(rw_buf+sizeof(RFILE_MNTN_HEADER),&res_seek_msg,sizeof(RFILE_RESP_SEEK_MSG));

    total_send_len = sizeof(RFILE_MNTN_HEADER) + sizeof(RFILE_RESP_SEEK_MSG);

    ret = rfile_write_ccpu_pkt(mnt_h->opsPid,(char *)rw_buf,total_send_len);

    return ret;
}
int fremoveProxy(RFILE_MNTN_HEADER *mnt_h, char * data)
{
    int                          ret = -1;
    char                   *pathname = NULL;
    unsigned int          expect_cur = 0;
    int               total_send_len = 0;
    RFILE_MNTN_HEADER   *mntn_header = NULL;

    RFILE_RESP_REMOVE_MSG res_remove_msg = {0};
    RFILE_REMOVE_HEAD      *premove_head = (RFILE_REMOVE_HEAD *)data;


    memset(rw_buf,0,BUF_LEN);

    mntn_header               = (RFILE_MNTN_HEADER *)rw_buf;
    mntn_header->opsPid       = mnt_h->opsPid;
    mntn_header->totalFrame   = mnt_h->totalFrame;
    mntn_header->curFrame     = 1;
    mntn_header->curlen       = sizeof(RFILE_RESP_REMOVE_MSG);
    mntn_header->reserve      = MNT_H_MAGIC;


    /*可维可测*/
    if(mnt_h->curFrame != (++expect_cur))
    {
        printk(KERN_ERR "fremoveProxy:error expect cur frame is %d, realy cur frame is %d\n",expect_cur,mnt_h->curFrame);

        return RFILE_ERROR;
    }

    /*入参判断，防止数据异常*/
    if((premove_head->pathLen) == 0)
    {
        printk(KERN_INFO "fremoveProxy: pathlen %x  fail\n",premove_head->pathLen);
        goto SEND_RESPONSE;
    }

    if(NULL == rfile_linux_ops.bsp_remove)
    {
        printk(KERN_INFO "fremoveProxy: fremove unregister \n");
        goto SEND_RESPONSE;
    }


    pathname = (char *)kmalloc((premove_head->pathLen),GFP_KERNEL);
    if (pathname == NULL)
    {
        printk(KERN_ERR "fremoveProxy:alloc pathname mem fail\n");
        goto SEND_RESPONSE;
    }

    strncpy(pathname,data+sizeof(RFILE_REMOVE_HEAD),premove_head->pathLen);

    RFILE_DEBUGPL(0x5,"fremoveProxy: pathname is %s\n",pathname);

    /*文件没有打开*/
    if( (rfile_linux_ops.bsp_access)( pathname, 0 ) != -1 )
    {
        ret=(rfile_linux_ops.bsp_remove)(pathname);
    }

SEND_RESPONSE:

    res_remove_msg.opsType=premove_head->opsType +1;
    res_remove_msg.ret = ret;

    memcpy(rw_buf+sizeof(RFILE_MNTN_HEADER),&res_remove_msg,sizeof(RFILE_RESP_REMOVE_MSG));

    total_send_len = sizeof(RFILE_MNTN_HEADER)+ sizeof(RFILE_RESP_REMOVE_MSG);

    ret = rfile_write_ccpu_pkt(mnt_h->opsPid,rw_buf,total_send_len);

    if(pathname!=NULL)
    {
        kfree(pathname);
    }

    return ret;
}
int ftellProxy(RFILE_MNTN_HEADER *mnt_h, char * data)
{
    int                               ret = -1;
    RFILE                             *fp  = NULL;
    RFILE_RESP_TELL_MSG      res_tell_msg = {0};
    RFILE_TELL_HEAD           *ptell_head = (RFILE_TELL_HEAD *)data;

    int                    total_send_len = 0;
    RFILE_MNTN_HEADER        *mntn_header = NULL;
    unsigned int               expect_cur = 0;

    memset(rw_buf,0,BUF_LEN);

    mntn_header              = (RFILE_MNTN_HEADER *)rw_buf;
    mntn_header->opsPid      = mnt_h->opsPid;
    mntn_header->totalFrame  = mnt_h->totalFrame;
    mntn_header->curFrame    = 1;
    mntn_header->curlen      = sizeof(RFILE_RESP_TELL_MSG);
    mntn_header->reserve     = MNT_H_MAGIC;


    /*可维可测*/
    if(mnt_h->curFrame != (++expect_cur))
    {
        printk(KERN_ERR "ftellProxy:error expect cur frame is %d, realy cur frame is %d\n",expect_cur,mnt_h->curFrame);

        return RFILE_ERROR;
    }

    /*入参判断*/
    if(0 == ptell_head->fp)
    {
        printk(KERN_INFO "ftellProxy:the fp is null \n");
        goto SEND_RESPONSE;
    }

    if(NULL == rfile_linux_ops.bsp_ftell)
    {
        printk(KERN_INFO "ftellProxy:ftell unregister \n");
        goto SEND_RESPONSE;
    }

    fp=(RFILE *)fp_check_file(ptell_head->fp);

    RFILE_DEBUGPL(0x1,"ftellProxy: fp is %p\n",fp);

    if (NULL == fp)/*NULL*/
    {
        printk(KERN_ERR "ftellProxy:fp check fail\n");

        goto SEND_RESPONSE;
    }

    ret=(rfile_linux_ops.bsp_ftell)(fp);

SEND_RESPONSE:

    res_tell_msg.opsType = ptell_head->opsType +1;
    res_tell_msg.ret     = ret;

    memcpy(rw_buf+sizeof(RFILE_MNTN_HEADER),&res_tell_msg,sizeof(RFILE_RESP_TELL_MSG));

    total_send_len = sizeof(RFILE_MNTN_HEADER)+ sizeof(RFILE_RESP_TELL_MSG);

    ret = rfile_write_ccpu_pkt(mnt_h->opsPid,rw_buf,total_send_len);

    return ret;
}
int frenameProxy( RFILE_MNTN_HEADER *mnt_h, char * data )
{
    int                                   ret = -1;
    RFILE_RESP_RENAME_MSG      res_rename_msg = {0};
    RFILE_RENAME_HEAD           *prename_head = (RFILE_RENAME_HEAD *)data;
    char                        *oldname_path = NULL;
    char                        *newname_path = NULL;

    int                        total_send_len = 0;
    RFILE_MNTN_HEADER            *mntn_header = NULL;
    unsigned int    expect_cur = 0;
    memset(rw_buf,0,BUF_LEN);

    mntn_header                = (RFILE_MNTN_HEADER *)rw_buf;
    mntn_header->opsPid        = mnt_h->opsPid;
    mntn_header->totalFrame    = mnt_h->totalFrame;
    mntn_header->curFrame      = 1;
    mntn_header->curlen        = sizeof(RFILE_RESP_RENAME_MSG);
    mntn_header->reserve       = MNT_H_MAGIC;



    /*可维可测*/
    if(mnt_h->curFrame != (++expect_cur))
    {
        printk(KERN_ERR "frenameProxy: error expect cur frame is %d, realy cur frame is %d\n",expect_cur,mnt_h->curFrame);

        return RFILE_ERROR;
    }

    /*入参判断，防止数据异常*/
    if(((prename_head->new_path_len) ==0) || ((prename_head->old_path_len) == 0))
    {
        printk(KERN_INFO "frenameProxy: new pathlen %x or old pathlen %x fail\n",prename_head->new_path_len,prename_head->old_path_len);
        goto SEND_RESPONSE;
    }

    if(NULL == rfile_linux_ops.bsp_rename)
    {
        printk(KERN_INFO "frenameProxy: xcopy unregister \n");
        goto SEND_RESPONSE;
    }

    oldname_path = (char *)kmalloc((prename_head->old_path_len),GFP_KERNEL);
    if (oldname_path == NULL)
    {
        printk(KERN_ERR "frenameProxy:alloc old path mem fail\n");
        goto SEND_RESPONSE;
    }

    strncpy(oldname_path,data+sizeof(RFILE_RENAME_HEAD),prename_head->old_path_len);

    newname_path = (char *)kmalloc((prename_head->new_path_len),GFP_KERNEL);
    if (newname_path == NULL)
    {
        printk(KERN_ERR "frenameProxy:alloc new path mem fail\n");
        goto SEND_RESPONSE;
    }

    strncpy(newname_path,data+sizeof(RFILE_RENAME_HEAD)+(prename_head->old_path_len),prename_head->new_path_len);

    RFILE_DEBUGPL(0x9,"frenameProxy: old pathname is %s, new pathname is %s\n",oldname_path,newname_path);

    ret=(rfile_linux_ops.bsp_rename)(oldname_path,newname_path);

SEND_RESPONSE:

    res_rename_msg.opsType=prename_head->opsType +1;
    res_rename_msg.ret=ret;

    memcpy(rw_buf+sizeof(RFILE_MNTN_HEADER),&res_rename_msg,sizeof(RFILE_RESP_RENAME_MSG));

    total_send_len = sizeof(RFILE_MNTN_HEADER)+ sizeof(RFILE_RESP_RENAME_MSG);

    ret = rfile_write_ccpu_pkt(mnt_h->opsPid,rw_buf,total_send_len);

    if(oldname_path!=NULL)
    {
        kfree(oldname_path);
    }

    if(newname_path!=NULL)
    {
        kfree(newname_path);
    }

    return ret;
}



int faccessProxy( RFILE_MNTN_HEADER *mnt_h, char * data )
{
    int                               ret = -1;
    char                        *path_dir = NULL;
    int                              mode = 0;
    RFILE_RESP_ACCESS_MSG  res_access_msg = {0};
    RFILE_ACCESS_HEAD       *paccess_head = (RFILE_ACCESS_HEAD *)data;

    int                    total_send_len = 0;
    RFILE_MNTN_HEADER        *mntn_header = NULL;
    unsigned int     expect_cur = 0;
    memset(rw_buf,0,BUF_LEN);

    mntn_header                 = (RFILE_MNTN_HEADER *)rw_buf;
    mntn_header->opsPid         = mnt_h->opsPid;
    mntn_header->totalFrame     = mnt_h->totalFrame;
    mntn_header->curFrame       = 1;
    mntn_header->curlen         = sizeof(RFILE_RESP_ACCESS_MSG);
    mntn_header->reserve        = MNT_H_MAGIC;



    /*可维可测*/
    if(mnt_h->curFrame != (++expect_cur))
    {
        printk(KERN_ERR "faccessProxy:error expect cur frame is %d, realy cur frame is %d\n",expect_cur,mnt_h->curFrame);

        return RFILE_ERROR;
    }

    /*入参判断，防止数据异常*/
    if(((paccess_head->pathLen) ==0) || ((paccess_head->modeLen) == 0))
    {
        printk(KERN_INFO "faccessProxy: pathlen %x or modelen %x fail\n",paccess_head->pathLen,paccess_head->modeLen);
        goto SEND_RESPONSE;
    }

    if(NULL == rfile_linux_ops.bsp_access)
    {
        printk(KERN_INFO "faccessProxy: readdir unregister \n");
        goto SEND_RESPONSE;
    }

    path_dir=(char *)kmalloc((paccess_head->pathLen),GFP_KERNEL);
    if (path_dir == NULL)
    {
        printk(KERN_ERR "faccessProxy:alloc path mem fail\n");
        goto SEND_RESPONSE;
    }

    strncpy(path_dir,data+sizeof(RFILE_ACCESS_HEAD),paccess_head->pathLen);

    strncpy((char*)&mode,(data+sizeof(RFILE_ACCESS_HEAD)+ paccess_head->pathLen),paccess_head->modeLen);

    RFILE_DEBUGPL(0x11,"faccessProxy: path is %s, mode is %x\n",path_dir,mode);

    ret=(rfile_linux_ops.bsp_access)(path_dir,mode);

SEND_RESPONSE:

    res_access_msg.opsType=paccess_head->opsType +1;
    res_access_msg.ret=ret;

    memcpy(rw_buf+sizeof(RFILE_MNTN_HEADER),&res_access_msg,sizeof(RFILE_RESP_ACCESS_MSG));

    total_send_len = sizeof(RFILE_MNTN_HEADER)+ sizeof(RFILE_RESP_ACCESS_MSG);

    ret = rfile_write_ccpu_pkt(mnt_h->opsPid,rw_buf,total_send_len);

    if(path_dir!=NULL)
    {
        kfree(path_dir);
    }

    return ret;
}



int fmkdirProxy( RFILE_MNTN_HEADER *mnt_h, char * data )
{
    int                                   ret = -1;
    RFILE_RESP_MKDIR_MSG        res_mkdir_msg = {0};
    RFILE_MKDIR_HEAD             *pmkdir_head = (RFILE_MKDIR_HEAD *)data;
    char                            *pathname = NULL;
    unsigned int                   expect_cur = 0;

    int                        total_send_len = 0;
    RFILE_MNTN_HEADER            *mntn_header = NULL;

    memset(rw_buf,0,BUF_LEN);

    mntn_header                 = (RFILE_MNTN_HEADER *)rw_buf;
    mntn_header->opsPid         = mnt_h->opsPid;
    mntn_header->totalFrame     = mnt_h->totalFrame;
    mntn_header->curFrame       = 1;
    mntn_header->curlen         = sizeof(RFILE_RESP_MKDIR_MSG);
    mntn_header->reserve        = MNT_H_MAGIC;


    /*可维可测*/
    if(mnt_h->curFrame != (++expect_cur))
    {
        printk(KERN_ERR "fmkdirProxy:error expect cur frame is %d, realy cur frame is %d\n",expect_cur,mnt_h->curFrame);

        return RFILE_ERROR;
    }

    /*入参判断，防止数据异常*/
    if((pmkdir_head->pathlen) == 0)
    {
        printk(KERN_INFO "fmkdirProxy: pathlen %x  fail\n",pmkdir_head->pathlen);
        goto SEND_RESPONSE;
    }

    if(NULL == rfile_linux_ops.bsp_mkdir)
    {
        printk(KERN_INFO "fmkdirProxy: mkdir unregister \n");
        goto SEND_RESPONSE;
    }

    pathname = (char *)kmalloc((pmkdir_head->pathlen),GFP_KERNEL);
    if (pathname == NULL)
    {
        printk(KERN_ERR "fmkdirProxy:alloc path_dir mem fail\n");
        goto SEND_RESPONSE;
    }

    strncpy(pathname,data+sizeof(RFILE_MKDIR_HEAD),pmkdir_head->pathlen);

    RFILE_DEBUGPL(0x21,"fmkdirProxy: pathname is %s\n",pathname);

    ret=(rfile_linux_ops.bsp_mkdir)(pathname);

SEND_RESPONSE:

    res_mkdir_msg.opsType = pmkdir_head->opsType +1;
    res_mkdir_msg.ret     = ret;

    memcpy(rw_buf+sizeof(RFILE_MNTN_HEADER),&res_mkdir_msg,sizeof(RFILE_RESP_MKDIR_MSG));

    total_send_len = sizeof(RFILE_MNTN_HEADER)+ sizeof(RFILE_RESP_MKDIR_MSG);

    ret = rfile_write_ccpu_pkt(mnt_h->opsPid,rw_buf,total_send_len);

    if(pathname!=NULL)
    {
        kfree(pathname);
    }

    return ret;
}



int frmdirProxy( RFILE_MNTN_HEADER *mnt_h, char * data )
{
    int                             ret = -1;
    RFILE_RESP_RMDIR_MSG  res_rmdir_msg = {0};
    RFILE_RMDIR_HEAD       *prmdir_head = (RFILE_RMDIR_HEAD *)data;
    char                      *pathname = NULL;
    unsigned int             expect_cur = 0;

    int                  total_send_len = 0;
    RFILE_MNTN_HEADER      *mntn_header = NULL;

    memset(rw_buf,0,BUF_LEN);

    mntn_header               = (RFILE_MNTN_HEADER *)rw_buf;
    mntn_header->opsPid       = mnt_h->opsPid;
    mntn_header->totalFrame   = mnt_h->totalFrame;
    mntn_header->curFrame     = 1;
    mntn_header->curlen         = sizeof(RFILE_RESP_RMDIR_MSG);
    mntn_header->reserve      = MNT_H_MAGIC;


    /*可维可测*/
    if(mnt_h->curFrame != (++expect_cur))
    {
        printk(KERN_ERR "frmdirProxy: error expect cur frame is %d, realy cur frame is %d\n",expect_cur,mnt_h->curFrame);

        return RFILE_ERROR;
    }

    /*入参判断，防止数据异常*/
    if((prmdir_head->pathlen) == 0)
    {
        printk(KERN_INFO "frmdirProxy: pathlen %x  fail\n",prmdir_head->pathlen);
        goto SEND_RESPONSE;
    }

    if(NULL == rfile_linux_ops.bsp_rmdir)
    {
        printk(KERN_INFO "frmdirProxy: rmdir unregister \n");
        goto SEND_RESPONSE;
    }

    pathname = (char *)kmalloc((prmdir_head->pathlen),GFP_KERNEL);
    if (pathname == NULL)
    {
        printk(KERN_ERR "frmdirProxy:alloc path_dir mem fail\n");
        goto SEND_RESPONSE;
    }

    strncpy(pathname,data+sizeof(RFILE_RMDIR_HEAD),prmdir_head->pathlen);

    RFILE_DEBUGPL(0x41,"frmdirProxy: pathname is %s\n",pathname);

    /*增加限制条件*/
    ret=(rfile_linux_ops.bsp_rmdir)(pathname);

SEND_RESPONSE:

    res_rmdir_msg.opsType=prmdir_head->opsType +1;
    res_rmdir_msg.ret=ret;

    memcpy(rw_buf+sizeof(RFILE_MNTN_HEADER),&res_rmdir_msg,sizeof(RFILE_RESP_RMDIR_MSG));

    total_send_len = sizeof(RFILE_MNTN_HEADER)+ sizeof(RFILE_RESP_RMDIR_MSG);

    ret = rfile_write_ccpu_pkt(mnt_h->opsPid,rw_buf,total_send_len);

    if(pathname!=NULL)
    {
        kfree(pathname);
    }

    return ret;
}
int fopendirProxy( RFILE_MNTN_HEADER *mnt_h, char * data )
{
    int                                 ret = -1;
    void					*fp;
    RFILE_RESP_OPENDIR_MSG  res_opendir_msg = {0};
    RFILE_OPENDIR_HEAD       *popendir_head = (RFILE_OPENDIR_HEAD *)data;
    char                          *dirname = NULL;
    unsigned int                 expect_cur = 0;
    int					hashed_fp = 0;
    int                      total_send_len = 0;
    RFILE_MNTN_HEADER          *mntn_header = NULL;

    memset(rw_buf,0,BUF_LEN);

    mntn_header              = (RFILE_MNTN_HEADER *)rw_buf;
    mntn_header->opsPid      = mnt_h->opsPid;
    mntn_header->totalFrame  = mnt_h->totalFrame;
    mntn_header->curFrame    = 1;
    mntn_header->curlen      = sizeof(RFILE_RESP_OPENDIR_MSG);
    mntn_header->reserve     = MNT_H_MAGIC;


    /*可维可测*/
    if(mnt_h->curFrame != (++expect_cur))
    {
        printk(KERN_ERR "fopendirProxy: error expect cur frame is %d, realy cur frame is %d\n",expect_cur,mnt_h->curFrame);

        return RFILE_ERROR;
    }

    /*入参判断，防止数据异常*/
    if((popendir_head->pathlen) == 0)
    {
        printk(KERN_INFO "fopendirProxy: pathlen %x  fail\n",popendir_head->pathlen);
        goto SEND_RESPONSE;
    }

    if(NULL == rfile_linux_ops.bsp_opendir)
    {
        printk(KERN_INFO "fopendirProxy: opendir unregister \n");
        goto SEND_RESPONSE;
    }

    dirname = (char *)kmalloc((popendir_head->pathlen),GFP_KERNEL);
    if (dirname == NULL)
    {
        printk(KERN_ERR "fopendirProxy: alloc path_dir mem fail\n");
        goto SEND_RESPONSE;
    }

    strncpy(dirname,(data+sizeof(RFILE_OPENDIR_HEAD)),popendir_head->pathlen);

    RFILE_DEBUGPL(0x81,"fopendirProxy: dirname is %s\n",dirname);

    fp=(rfile_linux_ops.bsp_opendir)(dirname);

    if(NULL != fp)
    {
        hashed_fp = fp_list_add_dir(fp);
    }


SEND_RESPONSE:

    res_opendir_msg.opsType = popendir_head->opsType +1;
    res_opendir_msg.ret     = hashed_fp;

    memcpy(rw_buf+sizeof(RFILE_MNTN_HEADER),&res_opendir_msg,sizeof(RFILE_RESP_OPENDIR_MSG));

    total_send_len = sizeof(RFILE_MNTN_HEADER)+ sizeof(RFILE_RESP_OPENDIR_MSG);

    ret = rfile_write_ccpu_pkt(mnt_h->opsPid,rw_buf,total_send_len);

    if(dirname!=NULL)
    {
        kfree(dirname);
    }

    return ret;
}



int freaddirProxy( RFILE_MNTN_HEADER *mnt_h, char * data )
{
    int                                 ret = 0;
    DRV_DIRENT_S                     dirbuf = {0};
    int                              retval = 0;
    RFILE_RESP_READDIR_MSG  res_readdir_msg = {0};
    RFILE_READDIR_HEAD       *preaddir_head = (RFILE_READDIR_HEAD *)data;
    DRV_DIR_S          *pdir;
    int                      total_send_len = 0;
    RFILE_MNTN_HEADER          *mntn_header = NULL;
    unsigned int                 expect_cur = 0;
    memset(rw_buf,0,BUF_LEN);



    mntn_header                = (RFILE_MNTN_HEADER *)rw_buf;
    mntn_header->opsPid        = mnt_h->opsPid;
    mntn_header->totalFrame    = mnt_h->totalFrame;
    mntn_header->curFrame      = 1;
    mntn_header->curlen        = sizeof(RFILE_RESP_READDIR_MSG);
    mntn_header->reserve       = MNT_H_MAGIC;

    /*可维可测*/
    if(mnt_h->curFrame != (++expect_cur))
    {
        printk(KERN_ERR "freaddirProxy:error expect cur frame is %d, realy cur frame is %d\n",expect_cur,mnt_h->curFrame);

        return RFILE_ERROR;
    }

    /*入参判断，防止数据异常*/
    if((preaddir_head->dir) == 0)
    {
        printk(KERN_INFO "freaddirProxy: dir %x  fail\n",preaddir_head->dir);
        goto SEND_RESPONSE;
    }

    if(NULL == rfile_linux_ops.bsp_readdir)
    {
        printk(KERN_INFO "freaddirProxy: readdir unregister \n");
        goto SEND_RESPONSE;
    }

    RFILE_DEBUGPL(0x101,"freaddirProxy: dir is %x\n",preaddir_head->dir);
    pdir = (DRV_DIR_S*)fp_check_dir(preaddir_head->dir);
    if (NULL == pdir)
    {
        printk(KERN_ERR "freaddirProxy:fp check fail\n");

        goto SEND_RESPONSE;
    }

    ret = (rfile_linux_ops.bsp_readdir)(pdir,&dirbuf);

SEND_RESPONSE:

    res_readdir_msg.opsType = preaddir_head->opsType +1;
    res_readdir_msg.retval  = ret;

    memcpy(&res_readdir_msg.ret,&dirbuf,sizeof(DRV_DIRENT_S));

    memcpy(rw_buf+sizeof(RFILE_MNTN_HEADER),&res_readdir_msg,sizeof(RFILE_RESP_READDIR_MSG));

    total_send_len = sizeof(RFILE_MNTN_HEADER)+ sizeof(RFILE_RESP_READDIR_MSG);

    retval = rfile_write_ccpu_pkt(mnt_h->opsPid,rw_buf,total_send_len);

    return retval;
}



int fclosedirProxy( RFILE_MNTN_HEADER *mnt_h, char * data )
{
    int                                   ret = -1;
    RFILE_RESP_CLOSEDIR_MSG  res_closedir_msg = {0};
    RFILE_CLOSEDIR_HEAD       *pclosedir_head = (RFILE_CLOSEDIR_HEAD *)data;
    DRV_DIR_S  			      *pdir;

    int                        total_send_len = 0;
    RFILE_MNTN_HEADER            *mntn_header = NULL;
    unsigned int    expect_cur = 0;
    memset(rw_buf,0,BUF_LEN);

    mntn_header                = (RFILE_MNTN_HEADER *)rw_buf;
    mntn_header->opsPid        = mnt_h->opsPid;
    mntn_header->totalFrame    = mnt_h->totalFrame;
    mntn_header->curFrame      = 1;
    mntn_header->curlen        = sizeof(RFILE_RESP_CLOSEDIR_MSG);
    mntn_header->reserve       = MNT_H_MAGIC;



    /*可维可测*/
    if(mnt_h->curFrame != (++expect_cur))
    {
        printk(KERN_ERR "fclosedirProxy:error expect cur frame is %d, realy cur frame is %d\n",expect_cur,mnt_h->curFrame);

        return  RFILE_ERROR;
    }

    /*入参判断，防止数据异常*/
    if((pclosedir_head->dir) == 0)
    {
        printk(KERN_INFO "fclosedirProxy: dir %x  fail\n",pclosedir_head->dir);
        goto SEND_RESPONSE;
    }

    if(NULL == rfile_linux_ops.bsp_closedir)
    {
        printk(KERN_INFO "fclosedirProxy: closedir unregister \n");
        goto SEND_RESPONSE;
    }

    RFILE_DEBUGPL(0x201,"fclosedirProxy: dir is %x\n",pclosedir_head->dir);
    pdir =  (DRV_DIR_S *)fp_check_dir(pclosedir_head->dir);
    if (NULL == pdir)
    {
        printk(KERN_ERR "fclosedirProxy:fp check fail\n");

        goto SEND_RESPONSE;
    }

    ret = (rfile_linux_ops.bsp_closedir)(pdir);

    if(ret == 0)
    {
        fp_list_del_dir((int)(pclosedir_head->dir));
    }
    else
    {
        printk(KERN_INFO "fclosedirProxy: dir close fail\n");
    }

SEND_RESPONSE:

    res_closedir_msg.opsType = pclosedir_head->opsType +1;
    res_closedir_msg.ret     = ret;

    memcpy(rw_buf+sizeof(RFILE_MNTN_HEADER),&res_closedir_msg,sizeof(RFILE_RESP_CLOSEDIR_MSG));

    total_send_len = sizeof(RFILE_MNTN_HEADER)+ sizeof(RFILE_RESP_CLOSEDIR_MSG);

    ret = rfile_write_ccpu_pkt(mnt_h->opsPid,rw_buf,total_send_len);

    return ret;
}



int fstatdirProxy( RFILE_MNTN_HEADER *mnt_h, char * data )
{
    int                                  ret = -1;
    RFILE_RESP_STAT_MSG         res_stat_msg = {0};
    RFILE_STAT_HEAD              *pstat_head = (RFILE_STAT_HEAD *)data;

    char                           *pathname  = NULL;
    DRV_STAT_S                      stat_dir  = {0};

    int                       total_send_len = 0;
    RFILE_MNTN_HEADER           *mntn_header = NULL;
    unsigned int  expect_cur = 0;
    memset(rw_buf,0,BUF_LEN);

    mntn_header              = (RFILE_MNTN_HEADER *)rw_buf;
    mntn_header->opsPid      = mnt_h->opsPid;
    mntn_header->totalFrame  = mnt_h->totalFrame;
    mntn_header->curFrame    = 1;
    mntn_header->curlen      = sizeof(RFILE_RESP_STAT_MSG);
    mntn_header->reserve     = MNT_H_MAGIC;



    /*可维可测*/
    if(mnt_h->curFrame != (++expect_cur))
    {
        printk(KERN_ERR "fstatdirProxy:error expect cur frame is %d, realy cur frame is %d\n",expect_cur,mnt_h->curFrame);

        return RFILE_ERROR;
    }

    /*入参判断，防止数据异常*/
    if((pstat_head->pathlen) == 0)
    {
        printk(KERN_INFO "fstatdirProxy: pathlen %x  fail\n",pstat_head->pathlen);
        goto SEND_RESPONSE;
    }

    if(NULL == rfile_linux_ops.bsp_stat)
    {
        printk(KERN_INFO "fstatdirProxy: statdir unregister \n");
        goto SEND_RESPONSE;
    }

    pathname = (char *)kmalloc((pstat_head->pathlen),GFP_KERNEL);
    if (pathname == NULL)
    {
        printk(KERN_ERR "fstatdirProxy:alloc path_name mem fail\n");
        ret = -1;
        goto SEND_RESPONSE;
    }

    strncpy(pathname,data+sizeof(RFILE_STAT_HEAD),pstat_head->pathlen);

    RFILE_DEBUGPL(0x401,"fstatdirProxy: pathname is %s\n",pathname);

    ret = (rfile_linux_ops.bsp_stat)(pathname,&stat_dir);

SEND_RESPONSE:

    res_stat_msg.opsType = pstat_head->opsType +1;
    res_stat_msg.retval  = ret;

    memcpy(&res_stat_msg.ret,&stat_dir,sizeof(DRV_STAT_S));

    memcpy(rw_buf+sizeof(RFILE_MNTN_HEADER),&res_stat_msg,sizeof(RFILE_RESP_STAT_MSG));

    total_send_len = sizeof(RFILE_MNTN_HEADER)+ sizeof(RFILE_RESP_STAT_MSG);

    ret = rfile_write_ccpu_pkt(mnt_h->opsPid,rw_buf,total_send_len);

    if(pathname!=NULL)
    {
        kfree(pathname);
    }

    return ret;

}



int fxcopydirProxy( RFILE_MNTN_HEADER *mnt_h, char * data )
{
    int                                  ret = -1;
    RFILE_RESP_XCOPY_MSG       res_xcopy_msg = {0};
    RFILE_XCOPY_HEAD            *pxcopy_head = (RFILE_XCOPY_HEAD *)data;
    char                    *source_path_dir = NULL;
    char                      *dest_path_dir = NULL;

    int                       total_send_len = 0;
    RFILE_MNTN_HEADER           *mntn_header = NULL;
    unsigned int  expect_cur = 0;
    memset(rw_buf,0,BUF_LEN);

    mntn_header              = (RFILE_MNTN_HEADER *)rw_buf;
    mntn_header->opsPid      = mnt_h->opsPid;
    mntn_header->totalFrame  = mnt_h->totalFrame;
    mntn_header->curFrame    = 1;
    mntn_header->curlen      = sizeof(RFILE_RESP_XCOPY_MSG);
    mntn_header->reserve     = MNT_H_MAGIC;



    /*可维可测*/
    if(mnt_h->curFrame != (++expect_cur))
    {
        printk(KERN_ERR "fxcopydirProxy: error expect cur frame is %d, realy cur frame is %d\n",expect_cur,mnt_h->curFrame);

        return RFILE_ERROR;
    }

    /*入参判断，防止数据异常*/
    if(((pxcopy_head->source_path_len) ==0) || ((pxcopy_head->dest_path_len) == 0))
    {
        printk(KERN_INFO "fxcopydirProxy: sour pathlen %x or dest pathlen %x fail\n",pxcopy_head->source_path_len,pxcopy_head->dest_path_len);
        goto SEND_RESPONSE;
    }

    if(NULL == rfile_linux_ops.bsp_xcopy)
    {
        printk(KERN_INFO "fxcopydirProxy: xcopy unregister \n");
        goto SEND_RESPONSE;
    }

    source_path_dir =(char *)kmalloc((pxcopy_head->source_path_len),GFP_KERNEL);
    if (source_path_dir == NULL)
    {
        printk(KERN_ERR "fxcopydirProxy: alloc source_path_dir mem fail\n");
        goto SEND_RESPONSE;
    }

    strncpy(source_path_dir,data+sizeof(RFILE_XCOPY_HEAD),pxcopy_head->source_path_len);

    dest_path_dir =(char *)kmalloc((pxcopy_head->dest_path_len),GFP_KERNEL);
    if (dest_path_dir == NULL)
    {
        printk(KERN_ERR "fxcopydirProxy:alloc dest_path_dir mem fail\n");
        goto SEND_RESPONSE;
    }

    strncpy(dest_path_dir,(data+sizeof(RFILE_OPEN_HEAD)+ pxcopy_head->source_path_len),pxcopy_head->dest_path_len);

    RFILE_DEBUGPL(0x801,"fxcopydirProxy: source_path_dir is %s,dest_path_dir is %s\n",source_path_dir,dest_path_dir);

    ret=(rfile_linux_ops.bsp_xcopy)(source_path_dir,dest_path_dir);


SEND_RESPONSE:

    res_xcopy_msg.opsType = pxcopy_head->opsType +1;
    res_xcopy_msg.ret = ret;

    memcpy(rw_buf+sizeof(RFILE_MNTN_HEADER),&res_xcopy_msg,sizeof(RFILE_RESP_XCOPY_MSG));

    total_send_len = sizeof(RFILE_MNTN_HEADER)+ sizeof(RFILE_RESP_XCOPY_MSG);

    ret = rfile_write_ccpu_pkt(mnt_h->opsPid,rw_buf,total_send_len);

    if(source_path_dir!=NULL)
    {
        kfree(source_path_dir);
    }

    if(dest_path_dir!=NULL)
    {
        kfree(dest_path_dir);
    }

    return ret;
}



int fxdeletedirProxy( RFILE_MNTN_HEADER *mnt_h, char * data )
{
    int                                 ret = -1;
    RFILE_RESP_XDELETE_MSG  res_xdelete_msg = {0};
    RFILE_XDELETE_HEAD       *pxdelete_head = (RFILE_XDELETE_HEAD *)data;
    char                          *pathname = NULL;

    int                      total_send_len = 0;
    RFILE_MNTN_HEADER          *mntn_header = NULL;
    unsigned int  expect_cur = 0;
    memset(rw_buf,0,BUF_LEN);

    mntn_header              = (RFILE_MNTN_HEADER *)rw_buf;
    mntn_header->opsPid      = mnt_h->opsPid;
    mntn_header->totalFrame  = mnt_h->totalFrame;
    mntn_header->curFrame    = 1;
    mntn_header->curlen      = sizeof(RFILE_RESP_XDELETE_MSG);
    mntn_header->reserve     = MNT_H_MAGIC;



    /*可维可测*/
    if(mnt_h->curFrame != (++expect_cur))
    {
        printk(KERN_ERR "fxdeletedirProxy:error expect cur frame is %d, realy cur frame is %d\n",expect_cur,mnt_h->curFrame);

        return RFILE_ERROR;
    }

    /*入参判断，防止数据异常*/
    if((pxdelete_head->source_path_len) == 0)
    {
        printk(KERN_INFO "fxdeletedirProxy: pathlen %x  fail\n",pxdelete_head->source_path_len);
        goto SEND_RESPONSE;
    }

    if(NULL == rfile_linux_ops.bsp_xdelete)
    {
        printk(KERN_INFO "fxdeletedirProxy: deletedir unregister \n");
        goto SEND_RESPONSE;
    }


    pathname=(char *)kmalloc((pxdelete_head->source_path_len),GFP_KERNEL);
    if (pathname == NULL)
    {
        printk(KERN_ERR "fxdeletedirProxy:alloc path mem fail\n");
        goto SEND_RESPONSE;
    }

    strncpy(pathname,(data+sizeof(RFILE_XDELETE_HEAD)),pxdelete_head->source_path_len);

    RFILE_DEBUGPL(0x1001,"fxdeletedirProxy: pathname is %s\n",pathname);

    ret=(rfile_linux_ops.bsp_xdelete)(pathname);

SEND_RESPONSE:

    res_xdelete_msg.opsType=pxdelete_head->opsType +1;
    res_xdelete_msg.ret=ret;

    memcpy(rw_buf+sizeof(RFILE_MNTN_HEADER),&res_xdelete_msg,sizeof(RFILE_RESP_XDELETE_MSG));

    total_send_len = sizeof(RFILE_MNTN_HEADER)+ sizeof(RFILE_RESP_XDELETE_MSG);

    ret = rfile_write_ccpu_pkt(mnt_h->opsPid,rw_buf,total_send_len);

    if(pathname!=NULL)
    {
        kfree(pathname);
    }

    return ret;
}



int fmassreadProxy( RFILE_MNTN_HEADER *mnt_h, char * data )
{
    int                                    ret = -1;
    int                                 retval = 0;
    RFILE_RESP_MASSREAD_MSG   res_massread_msg = {0};
    RFILE_MASSREAD_HEAD        *pmassread_head = (RFILE_MASSREAD_HEAD *)data;
    char                       *partition_name = NULL;
    unsigned int                        offset = 0;
    unsigned int                        length = 0;
    int                             totalframe = 0;
    unsigned int                      head_len = 0;
    int                                  rd_sz = 0;


    int                         total_send_len = 0;
    RFILE_MNTN_HEADER             *mntn_header = NULL;
    char                        *mass_data_buf = NULL;
    char                        *temp_buf = NULL;
    unsigned int  expect_cur = 0;
    memset(rw_buf,0,BUF_LEN);

    mntn_header              = (RFILE_MNTN_HEADER *)rw_buf;
    mntn_header->opsPid      = mnt_h->opsPid;
    mntn_header->totalFrame  = mnt_h->totalFrame;
    mntn_header->curFrame    = 0;
    mntn_header->reserve     = MNT_H_MAGIC;



    head_len = sizeof(RFILE_MNTN_HEADER)+sizeof(RFILE_RESP_MASSREAD_MSG);

    /*可维可测*/
    if(mnt_h->curFrame != (++expect_cur))
    {
        printk(KERN_ERR "fmassreadProxy:error expect cur frame is %d, realy cur frame is %d\n",expect_cur,mnt_h->curFrame);

        return RFILE_ERROR;
    }

    /*申请资源*/
    partition_name = (char *)kmalloc((pmassread_head->partition_name_len),GFP_KERNEL);
    if (partition_name == NULL)
    {
        printk(KERN_ERR "fmassreadProxy :alloc partition_name mem fail\n");
        goto SEND_RESPONSE;
    }

    /*获取操作参数*/
    strncpy(partition_name,(data+sizeof(RFILE_MASSREAD_HEAD)),pmassread_head->partition_name_len);

    offset = pmassread_head->offset;

    length = pmassread_head->length;

    RFILE_DEBUGPL(0x1,"fmassreadProxy: partition_name is %s, offset is %x, length is %x\n",partition_name,offset,length);

    //实际数据长度
    rd_sz = length;

    /*申请数据缓存*/
    mass_data_buf = (char *)kmalloc(length,GFP_KERNEL);
    if (mass_data_buf == NULL)
    {
        printk(KERN_ERR "fmassreadProxy : alloc mass_data_buf mem fail\n");
        goto SEND_RESPONSE;
    }
    temp_buf = mass_data_buf;
    
    /*计算当前需要传输的次数*/
    totalframe = rfile_calc_massread_frame_num(length);

    mntn_header->totalFrame = totalframe;

    /*读出全部数据*/
    ret=(rfile_linux_ops.bsp_mass_read)(partition_name,offset,length,mass_data_buf);
    if(ret == -1)
    {
        printk(KERN_INFO "fmassreadProxy : bsp_mass_read fail\n");
        goto SEND_RESPONSE;
    }

    /*循环发送*/
    while(totalframe--)
    {
        /*读取数据大于一次ICC传输大小*/
        if (rd_sz > (BUF_LEN-head_len))
        {
            res_massread_msg.opsType = pmassread_head->opsType +1;
            res_massread_msg.ret  = ret;

            res_massread_msg.size = ret; /*CCPU 判断该值*/

            mntn_header->curFrame++;

            memcpy(rw_buf+sizeof(RFILE_MNTN_HEADER),&res_massread_msg,sizeof(RFILE_RESP_MASSREAD_MSG));

            memcpy(rw_buf+sizeof(RFILE_MNTN_HEADER)+sizeof(RFILE_RESP_MASSREAD_MSG),mass_data_buf,(BUF_LEN-head_len));

            total_send_len = BUF_LEN;

            rd_sz -= (BUF_LEN-head_len);

            mass_data_buf += (BUF_LEN-head_len);

			mntn_header->curlen = BUF_LEN - sizeof(RFILE_MNTN_HEADER);

            retval = rfile_write_ccpu_pkt(mnt_h->opsPid,rw_buf,total_send_len);

            /*等待C核接收完毕，使用信号量*/
            down(&sem_to_read_req);

        }
        else
        {
            res_massread_msg.opsType = pmassread_head->opsType +1;

            res_massread_msg.ret = ret;

            res_massread_msg.size = ret; /*CCPU 判断该值*/

            mntn_header->curFrame++;

            memcpy(rw_buf+sizeof(RFILE_MNTN_HEADER),&res_massread_msg,sizeof(RFILE_RESP_MASSREAD_MSG));

            memcpy(rw_buf+sizeof(RFILE_MNTN_HEADER)+sizeof(RFILE_RESP_MASSREAD_MSG),mass_data_buf,rd_sz);

            total_send_len = rd_sz+sizeof(RFILE_MNTN_HEADER)+ sizeof(RFILE_RESP_MASSREAD_MSG);

			mntn_header->curlen = rd_sz + sizeof(RFILE_RESP_MASSREAD_MSG);

            retval = rfile_write_ccpu_pkt(mnt_h->opsPid,rw_buf,total_send_len);
        }
    }


    if(partition_name!=NULL)
    {
        kfree(partition_name);
    }

    if(temp_buf!=NULL)
    {
        kfree(temp_buf);
    }

    return RFILE_OK;

SEND_RESPONSE:

    res_massread_msg.opsType = pmassread_head->opsType +1;
    res_massread_msg.ret     = 0;
    res_massread_msg.size    = ret;

    memcpy(rw_buf+sizeof(RFILE_MNTN_HEADER),&res_massread_msg,sizeof(RFILE_RESP_MASSREAD_MSG));

    total_send_len = sizeof(RFILE_MNTN_HEADER)+ sizeof(RFILE_RESP_MASSREAD_MSG);

    ret = rfile_write_ccpu_pkt(mnt_h->opsPid,rw_buf,total_send_len);

    if(partition_name!=NULL)
    {
        kfree(partition_name);
    }

    if(temp_buf!=NULL)
    {
        kfree(temp_buf);
    }

    return ret;
}
int fmasswriteProxy( RFILE_MNTN_HEADER *mnt_h, char * data )
{
    int                                        ret = -1;
    RFILE_RESP_MASSWRITE_MSG     res_masswrite_msg = {0};
    RFILE_MASSWRITE_HEAD          *pmasswrite_head = (RFILE_MASSWRITE_HEAD *)data;
    char                           *partition_name = NULL;
    unsigned int                            offset = 0;
    unsigned int                            length = 0;



    int                             total_send_len = 0;
    RFILE_MNTN_HEADER                 *mntn_header = NULL;


    memset(rw_buf,0,BUF_LEN);

    mntn_header               = (RFILE_MNTN_HEADER *)rw_buf;
    mntn_header->opsPid       = mnt_h->opsPid;
    mntn_header->totalFrame   = mnt_h->totalFrame;
    mntn_header->curFrame     = 1;
    mntn_header->curlen       = sizeof(RFILE_RESP_MASSWRITE_MSG);
    mntn_header->reserve      = MNT_H_MAGIC;

    res_masswrite_msg.opsType=pmasswrite_head->opsType+1;


    /*可维可测*/
    if(mnt_h->curFrame != (++g_mass_wt_expect_cur))
    {
        printk(KERN_ERR "fmasswriteProxy:error expect cur frame is %d, realy cur frame is %d\n",g_mass_wt_expect_cur,mnt_h->curFrame);
        g_mass_wt_expect_cur = 0;
        return RFILE_ERROR;
    }

    /*申请资源*/
    partition_name = (char *)kmalloc((pmasswrite_head->partition_name_len),GFP_KERNEL);
    if (partition_name == NULL)
    {
        printk(KERN_ERR "fmasswriteProxy:alloc partition_name mem fail\n");
        g_mass_wt_expect_cur = 0;
        return RFILE_ERROR;
    }

    /*获取操作参数*/
    strncpy(partition_name,(data+sizeof(RFILE_MASSWRITE_HEAD)),pmasswrite_head->partition_name_len);

    offset = pmasswrite_head->offset;

    length = pmasswrite_head->size;

    RFILE_DEBUGPL(0x1,"fmasswriteProxy: partition_name is %s, offset is %x, length is %x,the total frame is %x\n",partition_name,offset,length,mnt_h->totalFrame);


    if(mnt_h->curFrame == 1)
    {
       g_masswrite_offset = offset;
    }

    if(mnt_h->curFrame < mnt_h->totalFrame)
    {
        ret=(rfile_linux_ops.bsp_mass_write)(partition_name,g_masswrite_offset,length,data+sizeof(RFILE_MASSWRITE_HEAD)+pmasswrite_head->partition_name_len);
        if(-1 == ret)
        {
            printk(KERN_INFO "fmasswriteProxy: bsp_mass_write fail\n");
        }

        g_masswrite_offset = g_masswrite_offset + length;

        res_masswrite_msg.ret = ret;

        g_masswt_curr_frame_num++;

        mntn_header->curFrame = 1;

        RFILE_DEBUGPL(0x1,"fmasswriteProxy: the curframe is %x\n",g_masswt_curr_frame_num);

        if(ret<0)
        {
            g_masswt_curr_frame_num = 0;

            res_masswrite_msg.size  = ret;

            memcpy(rw_buf+sizeof(RFILE_MNTN_HEADER),&res_masswrite_msg,sizeof(RFILE_RESP_MASSWRITE_MSG));

            total_send_len = sizeof(RFILE_MNTN_HEADER)+sizeof(RFILE_RESP_MASSWRITE_MSG);

            ret = rfile_write_ccpu_pkt(mnt_h->opsPid,(char *)rw_buf,total_send_len);
        }
        else
        {
            ret = RFILE_OK;
        }

    }
    else
    {
        ret=(rfile_linux_ops.bsp_mass_write)(partition_name,g_masswrite_offset,length,data+sizeof(RFILE_MASSWRITE_HEAD)+pmasswrite_head->partition_name_len);
        if(-1 == ret)
        {
            printk(KERN_INFO "fmasswriteProxy: last bsp_mass_write fail\n");;
        }

        res_masswrite_msg.ret = ret;

        res_masswrite_msg.size  = ret;/*CCPU判断该值*/

        g_masswt_curr_frame_num++;

        mntn_header->curFrame = 1;
        mntn_header->totalFrame = 1;

        RFILE_DEBUGPL(0x1,"fmasswriteProxy: the last curframe is %x,the expect frame is %x\n",g_masswt_curr_frame_num,g_mass_wt_expect_cur);

        g_masswrite_offset = 0;

        g_masswt_curr_frame_num = 0;

        /*可维可测*/
        g_mass_wt_expect_cur = 0;

        memcpy(rw_buf+sizeof(RFILE_MNTN_HEADER),&res_masswrite_msg,sizeof(RFILE_RESP_MASSWRITE_MSG));

        total_send_len = sizeof(RFILE_MNTN_HEADER)+sizeof(RFILE_RESP_MASSWRITE_MSG) ;

        ret = rfile_write_ccpu_pkt(mnt_h->opsPid,(char *)rw_buf,total_send_len);

    }


    if (NULL != partition_name)
    {
       kfree(partition_name);
    }

    if(mnt_h->totalFrame > 1)
    {
        rfile_a_give_c_sem();
    }

    return ret;

}
RFILE* bsp_linux_fopen( const char *filename, const char *mode )
{
    int		flags;
    int		oflags;
    int     ret = 0;
    int     appendflag = 0;
    unsigned long fs;
    RFILE *f;
    if((NULL == filename)||(NULL == mode))
    {
       printk(KERN_INFO "bsp_linux_fopen:the input para is error \n");
       return NULL;
    }

    if (FALSE == g_rfilesystem_init)
    {
       printk(KERN_ERR "%s: rfile system hasn't initiated!\n", __FUNCTION__);
       return NULL;
    }

    /*防重入*/
    down(&sem_fopen);

    RFILE_WAKE_SYNC_EMMC;

    /*通过mode，转换为相应十六进制码*/
    if ((flags = rfile_sflags ((char *)mode, &oflags, &appendflag)) == 0)
    {
        up(&sem_fopen);
    	return (NULL);
	}

    f = (RFILE*)kmalloc(sizeof(RFILE), GFP_KERNEL);
    if(NULL == f)
    {
        printk(KERN_ERR "bsp_linux_fopen:kmalloc fail \n");
        up(&sem_fopen);
        return NULL;
    }

    memset(f,0,sizeof(RFILE));

    /* 改变内存访问权限 */
    fs = get_fs();
    set_fs(KERNEL_DS);

    /*对于V9，ret 实际类型为FILE*，当前传输地址*/
    ret = sys_open(filename, oflags, 00664/*S_IRWXU*/);
    if(ret < 0)
    {
       printk(KERN_INFO "bsp_linux_fopen:sys_open op fail, filename:%s, ret is %x \n", filename, ret);
       kfree(f);
       set_fs(fs);
       up(&sem_fopen);
       return NULL;
    }

    f->_file = ret;
    f->_flag = appendflag;
    f->fs_set = fs;

    set_fs(fs);

    up(&sem_fopen);

    return f;

}
int bsp_linux_fclose(RFILE* stream)
{
    int ret = 0;
    int f   = 0;
    unsigned long fs;

    if(NULL == stream)
    {
        printk(KERN_INFO "bsp_linux_fclose:the para is null\n");

        return RFILE_ERROR;
    }

    if (FALSE == g_rfilesystem_init)
    {
       printk(KERN_ERR "%s: rfile system hasn't initiated!\n", __FUNCTION__);
       return RFILE_ERROR;
    }

    down(&sem_fclose);

    RFILE_WAKE_SYNC_EMMC;

    /* 改变内存访问权限 */
    fs = get_fs();
    set_fs(KERNEL_DS);

    /*转换成f指针，为之前open时发送到CCPU的f指针*/
    f = stream->_file;

    ret = sys_close(f);
    if(ret < 0)
    {
        printk(KERN_INFO "bsp_linux_fclose:the sys_close ops fail, the ret is %x\n",ret);
        kfree(stream);
        set_fs(fs);
        up(&sem_fclose);
        return RFILE_ERROR;
    }

    kfree(stream);

    set_fs(fs);

    up(&sem_fclose);

    return RFILE_OK;
}





int bsp_linux_fread( void *ptr, unsigned int count, RFILE *stream )
{
    int ret = 0;
    int f   = 0;
    unsigned long fs;

    if((NULL == ptr)||(count == 0)||(NULL == stream))
    {
        printk(KERN_INFO "bsp_linux_fread:the input para is error\n");

        return RFILE_ERROR;
    }

    if (FALSE == g_rfilesystem_init)
    {
       printk(KERN_ERR "%s: rfile system hasn't initiated!\n", __FUNCTION__);
       return RFILE_ERROR;
    }

    down(&sem_fread);

    RFILE_WAKE_SYNC_EMMC;

    /* 改变内存访问权限 */
    fs = get_fs();
    set_fs(KERNEL_DS);

    f = stream->_file;

    ret = sys_read(f, (char*)ptr, count);
    if(ret < 0)
    {
        printk(KERN_INFO "bsp_linux_fread: the sys_read ops fail,the ret is %x\n",ret);
        set_fs(fs);
        up(&sem_fread);

        return RFILE_ERROR;
    }

    set_fs(fs);
    up(&sem_fread);

    return ret;
}
int bsp_linux_fwrite( void *buf, unsigned int count, RFILE *fp )
{
    int ret = 0;
    int retval = 0;
    int fd  = 0;
    unsigned long fs;

    if((NULL == buf)||(count == 0)||(NULL == fp))
    {
        printk(KERN_INFO "bsp_linux_fwrite:the input para is error\n");

        return RFILE_ERROR;
    }
    if (FALSE == g_rfilesystem_init)
    {
       printk(KERN_ERR "%s: rfile system hasn't initiated!\n", __FUNCTION__);
       return RFILE_ERROR;
    }

    down(&sem_fwrite);

    RFILE_WAKE_SYNC_EMMC;

    /* 改变内存访问权限 */
    fs = get_fs();
    set_fs(KERNEL_DS);

    fd = fp->_file;

    ret = sys_write(fd, buf, count);
    if(ret < 0)
    {
        printk(KERN_INFO "bsp_linux_fwrite:the sys_write ops fail,the ret is %x\n",ret);
        set_fs(fs);
        up(&sem_fwrite);

        return RFILE_ERROR;
    }

    retval = sys_fsync(fd);
    if(retval < 0)
    {
        printk(KERN_INFO "bsp_linux_fwrite:sys_fsync fail,the ret is %x\n",retval);
    }

    set_fs(fs);
    up(&sem_fwrite);

    return ret;
}
int bsp_linux_fseek( RFILE *stream, long offset, int whence )
{

    int ret = 0;
    int f   = 0;
    unsigned long fs;

    if((NULL == stream)||((DRV_SEEK_SET != whence)&&(DRV_SEEK_CUR != whence)&&(DRV_SEEK_END != whence)))
    {
       printk(KERN_INFO "bsp_linux_fseek:the input para is error\n");

       return RFILE_ERROR;
    }

    if (FALSE == g_rfilesystem_init)
    {
       printk(KERN_ERR "%s: rfile system hasn't initiated!\n", __FUNCTION__);
       return RFILE_ERROR;
    }

    down(&sem_fseek);

    RFILE_WAKE_SYNC_EMMC;

    /* 改变内存访问权限 */
    fs = get_fs();
    set_fs(KERNEL_DS);

    f = stream->_file;

    ret= sys_lseek(f, offset, whence);
    if(ret<0)
    {
        printk(KERN_INFO "bsp_linux_fseek:the sys_lseek ops is fail,the ret is %x\n",ret);
        set_fs(fs);
        up(&sem_fseek);

        return RFILE_ERROR;
    }

    set_fs(fs);
    up(&sem_fseek);

    return RFILE_OK;

}
int bsp_linux_remove( const char *pathname )
{
    int ret = 0;
    unsigned long fs = 0;


    if(NULL == pathname)
    {
       printk(KERN_INFO "bsp_linux_remove:the input para is error\n");

       return RFILE_ERROR;
    }

    if (FALSE == g_rfilesystem_init)
    {
       printk(KERN_ERR "%s: rfile system hasn't initiated!\n", __FUNCTION__);
       return RFILE_ERROR;
    }

    down(&sem_remove);

    RFILE_WAKE_SYNC_EMMC;

    /* 改变内存访问权限 */
    fs = get_fs();
    set_fs(KERNEL_DS);

    ret = sys_unlink(pathname);
    if(ret < 0)
    {
        printk(KERN_INFO "bsp_linux_remove:the sys unlink is fail,filename:%s,the ret is %x\n",pathname,ret);
        set_fs(fs);
        up(&sem_remove);

        return RFILE_ERROR;
    }

    set_fs(fs);
    up(&sem_remove);

    return RFILE_OK;
}
int bsp_linux_ftell( RFILE *stream )
{

    int ret = 0;
    int fd  = 0;
    unsigned long fs;

    if(NULL == stream)
    {
        printk(KERN_INFO "bsp_linux_ftell:the input para is null\n");

        return RFILE_ERROR;
    }

    if (FALSE == g_rfilesystem_init)
    {
       printk(KERN_ERR "%s: rfile system hasn't initiated!\n", __FUNCTION__);
       return RFILE_ERROR;
    }

    down(&sem_ftell);

    RFILE_WAKE_SYNC_EMMC;

    /* 改变内存访问权限 */
    fs = get_fs();
    set_fs(KERNEL_DS);

    fd = stream->_file;

    ret = sys_lseek(fd, 0, DRV_SEEK_CUR);
    if(ret<0)
    {
        printk(KERN_INFO "bsp_linux_ftell:the ftell is error,the ret is %x\n",ret);
        set_fs(fs);
        ret = RFILE_ERROR;
    }

    set_fs(fs);
    up(&sem_ftell);

    return ret;
}
int bsp_linux_rename( const char * oldname, const char * newname )
{
    int ret = 0;
    unsigned long fs = 0;

    if((NULL == oldname)||(NULL == newname))
    {
        printk(KERN_INFO "bsp_linux_rename:the input is null\n");
        return RFILE_ERROR;
    }

    if (FALSE == g_rfilesystem_init)
    {
       printk(KERN_ERR "%s: rfile system hasn't initiated!\n", __FUNCTION__);
       return RFILE_ERROR;
    }

    down(&sem_rename);

    RFILE_WAKE_SYNC_EMMC;

    /* 改变内存访问权限 */
    fs = get_fs();
    set_fs(KERNEL_DS);

    ret = sys_rename(oldname,newname);
    if(ret < 0)
    {
        printk(KERN_INFO "bsp_linux_rename:the renamer is fail,old filename:%s,new filename:%s,the ret is %x \n",oldname,newname,ret);
        set_fs(fs);
        up(&sem_rename);
        return RFILE_ERROR;
    }

    set_fs(fs);
    up(&sem_rename);

    return RFILE_OK;
}
int bsp_linux_access( const char *filename, int amode)
{
    int ret = 0;
    unsigned long fs = 0;

    if(NULL == filename)
    {
        printk(KERN_INFO "bsp_linux_access:the input is null\n");

        return RFILE_ERROR;
    }

    if (FALSE == g_rfilesystem_init)
    {
       printk(KERN_ERR "%s: rfile system hasn't initiated!\n", __FUNCTION__);
       return RFILE_ERROR;
    }

    down(&sem_access);

    RFILE_WAKE_SYNC_EMMC;

    /* 改变内存访问权限 */
    fs = get_fs();
    set_fs(KERNEL_DS);

    ret = sys_access(filename, amode);
    if(0 != ret)
    {
        printk(KERN_INFO "bsp_linux_access:the access is fail,filename:%s,the ret is %x\n",filename,ret);
        set_fs(fs);
        up(&sem_access);

        return RFILE_ERROR;
    }

    set_fs(fs);
    up(&sem_access);

    return ret;
}
int bsp_linux_mkdir( const char *pathname)
{
    int ret = 0;
   /* int  fd;*/
    unsigned long fs = 0;


    if(NULL == pathname)
    {
       printk(KERN_INFO "bsp_linux_mkdir:the input is null\n");

       return RFILE_ERROR;
    }

    if (FALSE == g_rfilesystem_init)
    {
       printk(KERN_ERR "%s: rfile system hasn't initiated!\n", __FUNCTION__);
       return RFILE_ERROR;
    }

    down(&sem_mkdir);

    RFILE_WAKE_SYNC_EMMC;

    /* 改变内存访问权限 */
    fs = get_fs();
    set_fs(KERNEL_DS);

    ret = sys_mkdir(pathname,0775);
    if(ret < 0)
    {
        printk(KERN_INFO "bsp_linux_mkdir:the the mkdir is fail,filename:%s,the ret is %x\n",pathname,ret);
        set_fs(fs);
        up(&sem_mkdir);
        return RFILE_ERROR;
    }

#if 0
    /*创建目标目录*/
    /*如果文件目录不存在就创建*/
    fd = sys_open((const char *)pathname, O_RDONLY | O_DIRECTORY, 0);
    if(fd < 0)
    {

        ret = sys_mkdir(pathname,0775);
        if(ret < 0)
        {
            printk(KERN_INFO "bsp_linux_mkdir:the the mkdir is fail,the ret is %x\n",ret);
            set_fs(fs);

            return RFILE_ERROR;
        }
    }/*根文件目录存在*/
    else
    {
        sys_close(fd);
    }
#endif

    set_fs(fs);
    up(&sem_mkdir);

    return RFILE_OK;
}
int bsp_linux_rmdir( const char *pathname)
{
    int ret = 0;
    unsigned long fs = 0;

    if(NULL == pathname)
    {
        printk(KERN_INFO "bsp_linux_rmdir:the input is null\n");

        return RFILE_ERROR;
    }

    if (FALSE == g_rfilesystem_init)
    {
       printk(KERN_ERR "%s: rfile system hasn't initiated!\n", __FUNCTION__);
       return RFILE_ERROR;
    }

    down(&sem_rmdir);

    RFILE_WAKE_SYNC_EMMC;

    /* 改变内存访问权限 */
    fs = get_fs();
    set_fs(KERNEL_DS);

    ret = sys_rmdir(pathname);
    if(ret < 0)
    {
        printk(KERN_INFO "bsp_linux_rmdir:the rmdir is fail,filename:%s,the ret is %x\n",pathname,ret);
        set_fs(fs);
        up(&sem_rmdir);
        return RFILE_ERROR;
    }
    up(&sem_rmdir);

    set_fs(fs);

    return RFILE_OK;
}
void* bsp_linux_opendir( const char *dirname )
{

    DRV_DIR_S *DIR = NULL;
    int ret = 0;
    unsigned long fs = 0;

    if(NULL == dirname)
    {
        printk(KERN_INFO "bsp_linux_opendir:the input is null\n");
        return 0;
    }

    if (FALSE == g_rfilesystem_init)
    {
       printk(KERN_ERR "%s: rfile system hasn't initiated!\n", __FUNCTION__);
       return 0;
    }

    down(&sem_opendir);

    RFILE_WAKE_SYNC_EMMC;

    DIR = (DRV_DIR_S*)kmalloc(sizeof(DRV_DIR_S), GFP_KERNEL);
    if(NULL == DIR)
    {
        printk(KERN_ERR "bsp_linux_opendir:the DIR malloc is fail\n");
        up(&sem_opendir);

        return 0;
    }

    /* 改变内存访问权限 */
    fs = get_fs();
    set_fs(KERNEL_DS);

    ret = sys_open(dirname, O_RDONLY | O_DIRECTORY, 0);
    if(ret < 0)
    {
        printk(KERN_INFO "bsp_linux_opendir:the sys_open is fail, dirname:%s, the ret is %x\n", dirname, ret);
        kfree(DIR);
        set_fs(fs);
        up(&sem_opendir);

        return 0;
    }

    DIR->dd_fd = ret;

    set_fs(fs);
    up(&sem_opendir);

    return (void*)DIR;

}
int bsp_linux_readdir( DRV_DIR_S *dirp,DRV_DIRENT_S* buf )
{

    int ret = 0;
    int len = 0;
    unsigned long fs = 0;

    struct linux_dirent* read_dirent = NULL;

    if((NULL == dirp) || (NULL == buf))
    {
        printk(KERN_INFO "bsp_linux_readdir:the input is null\n");
        return RFILE_ERROR;
    }
    if (FALSE == g_rfilesystem_init)
    {
       printk(KERN_ERR "%s: rfile system hasn't initiated!\n", __FUNCTION__);
       return RFILE_ERROR;
    }

    down(&sem_readdir);

    RFILE_WAKE_SYNC_EMMC;

    read_dirent = (struct linux_dirent*)kmalloc(sizeof(struct linux_dirent_rfile), GFP_KERNEL);
    if(NULL == read_dirent)
    {
        printk(KERN_ERR "bsp_linux_readdir:malloc is null\n");
        up(&sem_readdir);
        return RFILE_ERROR;
    }
    memset(read_dirent,0,sizeof(struct linux_dirent_rfile));

    /* 改变内存访问权限 */
    fs = get_fs();
    set_fs(KERNEL_DS);

    /*在openddir时，如果为V3R3就把fd也传输过去，用户使用是返回中获取*/
    while(1)
    {
        memset(read_dirent,0,sizeof(struct linux_dirent_rfile));
        ret = sys_old_readdir(dirp->dd_fd,(struct old_linux_dirent __user *)read_dirent,sizeof(struct linux_dirent));
        if(ret < 0)
        {
            printk(KERN_INFO "bsp_linux_readdir:the sys_getdents is fail,the ret is %x\n",ret);
            kfree(read_dirent);
            set_fs(fs);
            up(&sem_readdir);
            return RFILE_ERROR;
        }

        if(0 == ret)
        {
            break;
        }

        if((0!=strcmp(read_dirent->d_name,".."))&&(0!=strcmp(read_dirent->d_name,".")))
        {
            break;
        }
    }

    buf->d_ino = read_dirent->d_ino;
    len = strlen(read_dirent->d_name)+1;
    memcpy(buf->d_name,read_dirent->d_name,len);

    kfree(read_dirent);

    set_fs(fs);
    up(&sem_readdir);

    return ret;

}



int bsp_linux_closedir( DRV_DIR_S *dirp )
{

    int ret = 0;
    int f   = 0;
    unsigned long fs = 0;

    if(NULL == dirp)
    {
        printk(KERN_INFO "bsp_linux_closedir:the input is null\n");

        return RFILE_ERROR;
    }
    if (FALSE == g_rfilesystem_init)
    {
       printk(KERN_ERR "%s: rfile system hasn't initiated!\n", __FUNCTION__);
       return RFILE_ERROR;
    }

    down(&sem_closedir);

    RFILE_WAKE_SYNC_EMMC;

    /* 改变内存访问权限 */
    fs = get_fs();
    set_fs(KERNEL_DS);

    f = dirp->dd_fd;

    ret = sys_close(f);
    if(ret < 0)
    {
        printk(KERN_INFO "bsp_linux_closedir: closedir is fail,the ret is %x\n",ret);
        set_fs(fs);
        kfree(dirp);
        up(&sem_closedir);

        return RFILE_ERROR;
    }

    kfree(dirp);

    set_fs(fs);
    up(&sem_closedir);

    return RFILE_OK;

}
int bsp_linux_stat( const char *pathname, DRV_STAT_S *buf )
{
    int ret          = 0;
    unsigned long fs = 0;

    struct stat state = {0};

    if((NULL == pathname)||(NULL == buf))
    {
       printk(KERN_INFO "bsp_linux_stat:the input para is null");

       return RFILE_ERROR;
    }
    if (FALSE == g_rfilesystem_init)
    {
       printk(KERN_ERR "%s: rfile system hasn't initiated!\n", __FUNCTION__);
       return RFILE_ERROR;
    }
    down(&sem_stat);

    RFILE_WAKE_SYNC_EMMC;

    /* 改变内存访问权限 */
    fs = get_fs();
    set_fs(KERNEL_DS);

    if ((ret=sys_newlstat(pathname, &state)) < 0)
    {
        printk(KERN_INFO "bsp_linux_stat:stat is fail,the pathname is %s,the ret is %x\n",pathname,ret);
        set_fs(fs);
        up(&sem_stat);
        return RFILE_ERROR;
    }

    buf->st_dev     = state.st_dev;
    buf->st_ino     = state.st_ino;
    buf->st_mode    = state.st_mode;
    buf->st_nlink   = state.st_nlink;
    buf->st_uid     = state.st_uid;
    buf->st_gid     = state.st_gid;
    buf->st_rdev    = state.st_rdev;
    buf->st_size    = state.st_size;
    buf->st_atime   = state.st_atime;
    buf->st_mtime   = state.st_mtime;
    buf->st_ctime   = state.st_ctime;
    buf->st_blksize = state.st_blksize;
    buf->st_blocks  = state.st_blocks;

    set_fs(fs);
    up(&sem_stat);

    return RFILE_OK;
}
int bsp_linux_xcopy( const char * source, const char * dest )
{
    int ret = 0;
    int dir_fd = 0;
    int fd;
    int nread;
    int pos = 0;
    int mem_cmp_ret1 = 0;
    int mem_cmp_ret2 = 0;
    unsigned long fs;
    char d_type = 0;
    char *dir_ent_chain = NULL;

    char *file_source_name = NULL;
    char *file_dest_name   = NULL;

    /*获取目录信息，传输给dest buffer*/
    char *dir_name         = NULL;
    struct stat st;

    struct linux_dirent *dir_ent_temp = NULL;

    if((NULL == source)||(NULL == dest))
    {
        printk(KERN_INFO "bsp_linux_xcopy:the source %p or dest %p is null \n",source,dest);	/*int to address???*/

        return RFILE_ERROR;
    }
    if (FALSE == g_rfilesystem_init)
    {
       printk(KERN_ERR "%s: rfile system hasn't initiated!\n", __FUNCTION__);
       return RFILE_ERROR;
    }

    down(&sem_xcopy);

    RFILE_WAKE_SYNC_EMMC;

    dir_name = (char *)kmalloc(DRV_NAME_MAX, GFP_KERNEL);
    if (!dir_name)
    {
        printk(KERN_ERR "bsp_linux_xcopy: dir name malloc fail!\n" );
        up(&sem_xcopy);
        return RFILE_ERROR;
    }

    file_source_name = (char *)kmalloc(DRV_NAME_MAX, GFP_KERNEL);
    if (!file_source_name)
    {
        printk(KERN_ERR "bsp_linux_xcopy: file_source_name malloc fail!\n" );
        kfree(dir_name);
        up(&sem_xcopy);
        return RFILE_ERROR;
    }

    file_dest_name = (char *)kmalloc(DRV_NAME_MAX, GFP_KERNEL);
    if (!file_dest_name)
    {
        printk(KERN_ERR "bsp_linux_xcopy: file_dest_name malloc fail!\n" );
        kfree(dir_name);
        kfree(file_source_name);
        up(&sem_xcopy);
        return RFILE_ERROR;
    }

    /* 改变内存访问权限 */
    fs = get_fs();
    set_fs(KERNEL_DS);

    /*拷贝文件*/
    /* is it a file or directory? */
    if ((ret=sys_newlstat(source, &st)) < 0)
    {
        printk(KERN_INFO "bsp_linux_xcopy:stat is fail,the pathname is %s,the ret is %x\n",source,ret);
        goto error;
    }

    printk(KERN_INFO "bsp_linux_xcopy: the file mod is %x\n",st.st_mode);

    /* a file, so copy it */
    if (S_ISREG(st.st_mode))
    {
        ret = rfile_xcopy_file((char *)source,(char *)dest);
        if(ret < 0)
        {
            printk(KERN_ERR "bsp_linux_xcopy: usr copy file,rfile_xcopy_file fail, source:%s, dest:%s, the ret is %x\n", source, dest, ret);
            goto error;
        }
        else
        {
            printk(KERN_INFO "bsp_linux_xcopy: usr copy file,rfile_xcopy_file success \n");
            goto success;
        }
    }

    /*拷贝目录*/
    /*创建根目标目录*/
    /*如果根文件目录不存在就创建*/
    fd = sys_open((const char *)dest, O_RDONLY | O_DIRECTORY, 0);
    if(fd < 0)
    {

        ret = sys_mkdir(dest,0775);
        if(ret < 0)
        {
            printk(KERN_INFO "bsp_linux_xcopy:the dest mkdir is fail,the ret is %x\n",ret);
            kfree(dir_name);
            kfree(file_source_name);
            kfree(file_dest_name);
            up(&sem_xcopy);
            return RFILE_ERROR;
        }
    }/*根文件目录存在*/
    else
    {
        sys_close(fd);
    }

    dir_fd = sys_open(source, O_RDONLY | O_DIRECTORY, 0);
    if (dir_fd < 0)
    {
        printk(KERN_INFO "bsp_linux_xcopy: path open fail!.,the ret is %x\n",dir_fd);
        set_fs(fs);
        kfree(dir_name);
        kfree(file_source_name);
        kfree(file_dest_name);
        up(&sem_xcopy);

        return RFILE_ERROR;
    }

    dir_ent_chain = (char *)kmalloc(DIR_ENTRY_SIZE, GFP_KERNEL);
    if (!dir_ent_chain)
    {
        printk(KERN_ERR "bsp_linux_xcopy: malloc fail!\n" );
        set_fs(fs);
        kfree(dir_name);
        kfree(file_source_name);
        kfree(file_dest_name);
        up(&sem_xcopy);

        return RFILE_ERROR;
    }

#ifdef _DRV_LLT_
#else
    for(;;)
#endif
    {
        /* 获取指定路径下多个文件节点 */
        nread = sys_getdents(dir_fd, (struct linux_dirent *)dir_ent_chain, DIR_ENTRY_SIZE);
        if (nread < 0)
        {
            printk(KERN_INFO "bsp_linux_xcopy: get direct entries fail,the ret is %x\n",nread);
            goto error;
        }

        /* 取完全部节点 */
        if (0 == nread)
        {
            printk(KERN_INFO "bsp_linux_xcopy: all direct entries finish \n");
            #ifdef _DRV_LLT_
            #else
            break;
            #endif
        }

        /* 遍历文件节点，寻找目标文件 */
        for (pos = 0; pos < nread; )
        {
            dir_ent_temp = (struct linux_dirent *)(dir_ent_chain + pos);
            d_type = *(dir_ent_chain + pos + dir_ent_temp->d_reclen - 1);

            if (DT_REG == d_type)/*DT_REG 8*/
            {
                /*复制文件*/
                memset(file_dest_name,0,DRV_NAME_MAX);
                memset(file_source_name,0,DRV_NAME_MAX);

                memcpy(file_source_name,source,strlen(source));

                memcpy(file_dest_name,dest,strlen(dest));

                strncat(file_dest_name,"/",DRV_NAME_MAX-1);

                strncat(file_source_name,"/",DRV_NAME_MAX-1);


                strncat(file_source_name,dir_ent_temp->d_name,DRV_NAME_MAX-1);

                strncat(file_dest_name,dir_ent_temp->d_name,DRV_NAME_MAX-1);

                ret = rfile_xcopy_file(file_source_name,file_dest_name);
                if(ret < 0)
                {
                    printk(KERN_ERR "bsp_linux_xcopy: rfile_xcopy_file fail, file_source:%s, file_dest:%s \n", file_source_name, file_dest_name);
                    goto error;
                }

            }
            else if(DT_DIR == d_type)/*DT_DIR 4*/
            {
                /*复制目录*/
                memset(dir_name,0,DRV_NAME_MAX);

                memcpy(dir_name,dest,strlen(dest));

                strncat(dir_name,"/",DRV_NAME_MAX-1);

                mem_cmp_ret1 = memcmp(dir_ent_temp->d_name,".",1);

                mem_cmp_ret2 = memcmp(dir_ent_temp->d_name,"..",2);

                printk(KERN_INFO "the d_name is %s \n",dir_ent_temp->d_name);

                if((mem_cmp_ret1 == 0)||(mem_cmp_ret2 == 0))
                {
                    printk(KERN_INFO "bsp_linux_xcopy: the dir is . or ..\n");
                }
                else
                {
                    strncat(dir_name,dir_ent_temp->d_name,DRV_NAME_MAX-1);

                    printk(KERN_INFO "the dir name is %s \n",dir_name);

                    /*如果文件目录不存在就创建*/
                    fd = sys_open((const char *)dir_name, O_RDONLY | O_DIRECTORY, 0);
                    if(fd < 0)
                    {
                        ret = sys_mkdir(dir_name,0775);
                        if(ret < 0)
                        {
                            printk(KERN_INFO "bsp_linux_xcopy:the DT_DIR mkdir is fail,the ret is %x\n",ret);
                            goto error;
                        }
                    }/*文件目录存在*/
                    else
                    {
                        sys_close(fd);
                    }
                }

            }
            else
            {
                printk(KERN_INFO "bsp_linux_xcopy: d_type is not file or dir, pos is %x \n",pos);
                goto error;
            }

            /* 处理下一文件 */
            pos += dir_ent_temp->d_reclen;

        }
    }

success:

    if(dir_ent_chain!=NULL)
    {
        kfree(dir_ent_chain);
    }

    if(file_dest_name!=NULL)
    {
        kfree(file_dest_name);
    }

    if(file_source_name!=NULL)
    {
        kfree(file_source_name);
    }

    if(dir_name!=NULL)
    {
        kfree(dir_name);
    }

    sys_close(dir_fd);
    set_fs(fs);
    up(&sem_xcopy);

    return RFILE_OK;

error:

    if(dir_ent_chain!=NULL)
    {
        kfree(dir_ent_chain);
    }

    if(file_dest_name!=NULL)
    {
        kfree(file_dest_name);
    }

    if(file_source_name!=NULL)
    {
        kfree(file_source_name);
    }

    if(dir_name!=NULL)
    {
        kfree(dir_name);
    }

    sys_close(dir_fd);
    set_fs(fs);
    up(&sem_xcopy);

    return RFILE_ERROR;
}



int bsp_linux_xdelete( const char *pathname)
{
    int ret = 0;
    unsigned long fs = 0;

    /*当层目录检查*/
    if(NULL == pathname)
    {
        printk(KERN_INFO "bsp_linux_xdelete:the input is null\n");

        return RFILE_ERROR;
    }
    if (FALSE == g_rfilesystem_init)
    {
       printk(KERN_ERR "%s: rfile system hasn't initiated!\n", __FUNCTION__);
       return RFILE_ERROR;
    }
    down(&sem_xdelete);


    RFILE_WAKE_SYNC_EMMC;


    /* 改变内存访问权限 */
    fs = get_fs();
    set_fs(KERNEL_DS);

    ret = rfile_xdelete(pathname);
    if(ret < 0)
    {
        printk(KERN_ERR "bsp_linux_xdelete:the rmdir is fail, pathname:%s\n", pathname);
        set_fs(fs);
        up(&sem_xdelete);

        return RFILE_ERROR;
    }

    set_fs(fs);
    up(&sem_xdelete);

    return RFILE_OK;
}



int bsp_mass_read(const char *partionname,unsigned int offset,unsigned length,char *buffer)
{

    int fd;
    int ret          = -1;
    int i            = 0;
    int arry_number  = 0;
    char *pathname   = NULL;
    char blk_number[DRV_BLK_MAX] = {0};
    unsigned long fs;

    if((NULL == partionname)||(NULL == buffer))
    {
        printk(KERN_INFO "bsp_mass_read:the partionname %p or buffer %p is null \n",partionname,buffer);

        return RFILE_ERROR;
    }
    if (FALSE == g_rfilesystem_init)
    {
       printk(KERN_ERR "%s: rfile system hasn't initiated!\n", __FUNCTION__);
       return RFILE_ERROR;
    }
    down(&sem_mass_read);

    RFILE_WAKE_SYNC_EMMC;

    /* 改变内存访问权限,资源1 */
    fs = get_fs();
    set_fs(KERNEL_DS);

    /*获取资源2*/
    pathname = (char *)kmalloc(DRV_NAME_MAX, GFP_KERNEL);
    if (!pathname)
    {
        printk(KERN_ERR "bsp_mass_read: pathname malloc fail!\n" );
        /*释放资源1*/
        set_fs(fs);
        up(&sem_mass_read);
        return RFILE_ERROR;
    }

    /*从C核访问的目录文件转换为A核目录文件*/
    arry_number = sizeof(partitions)/sizeof(struct partition);
    /*打开文件*/

    for(i = 0;i<arry_number;i++)
    {
		if(NULL == partitions[i].name)
        {
            printk(KERN_INFO "partitions[%d].name: now we get a NULL pointer!\n",i);
			continue;
        }
        /*相等*/
        if(!strcmp(partionname,partitions[i].name))
        {
            memset(pathname,0,DRV_NAME_MAX);

            strncat(pathname,BLK_DEV,DRV_NAME_MAX-1);

            rfile_num2string(i,blk_number);

            strncat(pathname,blk_number,DRV_NAME_MAX-1);

            break;
        }
        else
        {
            printk(KERN_INFO "bsp_mass_read: pathname %s isnot what we want!\n",partitions[i].name);
        }

    }

    if(i == arry_number)
    {
        printk(KERN_INFO "bsp_mass_read: pathname isnot in partion table!\n" );
        goto error;
    }
	fd = sys_open(pathname,O_RDONLY,0);
    if(fd < 0)
    {
        printk(KERN_INFO "bsp_mass_read: sys_open fail,the fd is %x\n",fd);
        goto error;
    }


    ret= sys_lseek(fd, offset, DRV_SEEK_SET);
    if(ret<0)
    {
        printk(KERN_INFO "bsp_mass_read:the sys_lseek ops is fail,the ret is %x\n",ret);
        goto error;
    }

    ret = sys_read(fd, (char*)buffer, length);
    if(ret < 0)
    {
        printk(KERN_INFO "bsp_mass_read: the sys_read ops fail,the ret is %x\n",ret);
        goto error;
    }

    ret = sys_close(fd);
    if(ret < 0)
    {
        printk(KERN_INFO "bsp_mass_read: the sys_close ops fail,the ret is %x\n",ret);
        goto error;
    }

    /*释放资源1*/
    set_fs(fs);

    /*释放资源2*/
    if(NULL != pathname)
    {
       kfree(pathname);
    }

    printk(KERN_INFO "bsp_mass_read:the mass_read ops success\n");
    up(&sem_mass_read);

    return RFILE_OK;

error:

    /*释放资源1*/
    set_fs(fs);

    /*释放资源2*/
    if(NULL != pathname)
    {
       kfree(pathname);
    }
    up(&sem_mass_read);

    return RFILE_ERROR;

}




int bsp_mass_write(const char *partionname,unsigned int offset,unsigned length,char *buffer)
{

    int fd;
    int ret          = -1;
    int i            = 0;
    int arry_number  = 0;
    char *pathname   = NULL;
    char blk_number[DRV_BLK_MAX] = {0};
    unsigned long   fs;

    if((NULL == partionname)||(NULL == buffer))
    {
        printk(KERN_INFO "bsp_mass_write:the partionname %p or buffer %p is null \n",partionname,buffer);

        return RFILE_ERROR;
    }
    if (FALSE == g_rfilesystem_init)
    {
       printk(KERN_ERR "%s: rfile system hasn't initiated!\n", __FUNCTION__);
       return RFILE_ERROR;
    }
    down(&sem_mass_write);

    RFILE_WAKE_SYNC_EMMC;

    /* 改变内存访问权限,资源1*/
    fs = get_fs();
    set_fs(KERNEL_DS);

    /*资源2*/
    pathname = (char *)kmalloc(DRV_NAME_MAX, GFP_KERNEL);
    if (!pathname)
    {
        printk(KERN_ERR "bsp_mass_write: pathname malloc fail!\n" );
        /*释放资源1 */
        set_fs(fs);
        up(&sem_mass_write);

        return RFILE_ERROR;
    }

    /*从C核访问的目录文件转换为A核目录文件*/
    arry_number = sizeof(partitions)/sizeof(struct partition);

    /*最后一个为空*/
    arry_number = arry_number - 1;

    for(i = 0;i<arry_number;i++)
    {
        /*相等*/
        if(!strcmp(partionname,partitions[i].name))
        {
            memset(pathname,0,DRV_NAME_MAX);

            strncat(pathname,BLK_DEV,DRV_NAME_MAX-1);

            rfile_num2string(i,blk_number);

            strncat(pathname,blk_number,DRV_NAME_MAX-1);

            break;
        }
        else
        {
            RFILE_DEBUGPL(0x2001,"bsp_mass_write: pathname %s isnot what we want!\n",partitions[i].name);
        }

    }

    if(i == arry_number)
    {
        printk(KERN_INFO "bsp_mass_write: pathname isnot in partion table!\n" );
        goto error;
    }

    fd = sys_open(pathname,O_RDWR,0);
    if(fd < 0)
    {
        printk(KERN_INFO "bsp_mass_write: sys_open fail\n");
        goto error;
    }

    ret= sys_lseek(fd, offset, DRV_SEEK_SET);
    if(ret<0)
    {
        printk(KERN_INFO "bsp_mass_write:the sys_lseek ops is fail,the ret is %x\n",ret);
        goto error;
    }

    ret = sys_write(fd, buffer, length);
    if(ret < 0)
    {
        printk(KERN_INFO "bsp_linux_fwrite:the sys_write ops fail,the ret is %x\n",ret);
        goto error;
    }

    ret = sys_fsync(fd);
    if(ret < 0)
    {
        printk(KERN_INFO "bsp_mass_write:sys_fsync fail,the ret is %x\n",ret);
    }

    ret = sys_close(fd);
    if(ret < 0)
    {
        printk(KERN_INFO "bsp_mass_write: the sys_close ops fail,the ret is %x\n",ret);
        goto error;
    }

    /*释放资源1 */
    set_fs(fs);

    /*释放资源2*/
    if(NULL != pathname)
    {
       kfree(pathname);
    }

    RFILE_DEBUGPL(0x1,"bsp_mass_write:the mass_write ops success\n");

    up(&sem_mass_write);

    return RFILE_OK;

error:

    /*释放资源1 */
    set_fs(fs);

    /*释放资源2*/
    if(NULL != pathname)
    {
       kfree(pathname);
    }
    up(&sem_mass_write);

    return RFILE_ERROR;

}


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

