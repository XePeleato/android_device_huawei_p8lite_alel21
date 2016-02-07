
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/io.h>
#include "rfilesystem_process.h"
#include <linux/hisi/reset.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
宏以及变量定义
*****************************************************************************/

/*****************************************************************************
函数声明
*****************************************************************************/

/*****************************************************************************
函数定义
*****************************************************************************/

/*************************************************************************
*文件系统接口
**************************************************************************/
/*************************************************************************
 函 数 名   : BSP_fopen
 功能描述   : 打开一个文件
 输入参数   : const char *path -要打开的文件路径及文件名
                            const char *mode-打开模式，流形态
 输出参数   :
 返 回 值   : 成功返回文件指针，失败返回NULL
*************************************************************************/
RFILE *BSP_fopen(const char *path, const char *mode)
{
    return (bsp_linux_fopen(path, mode));
}
/*************************************************************************
 函 数 名   : BSP_fseek
 功能描述   : 重定位文件内部位置指针
 输入参数   : RFILE *stream-要操作的文件流
                            long offset-要移动的偏移地址
                            int whence-位置指针的起始位置
 输出参数   :
 返 回 值   : 成功返回0，失败返回-1
*************************************************************************/
int BSP_fseek(RFILE *stream, long offset, int whence)
{
    return (bsp_linux_fseek(stream, offset, whence));
}
/*************************************************************************
 函 数 名   : BSP_ftell
 功能描述   : 返回当前文件位置，也就是返回FILE指针当前位置
 输入参数   : RFILE *stream-要操作的文件流
 输出参数   :
 返 回 值   : 成功返回当前读写位置，失败返回-1
*************************************************************************/
long BSP_ftell(RFILE *stream)
{
    return (bsp_linux_ftell(stream));
}
/*************************************************************************
 函 数 名   : BSP_fread
 功能描述   : 从一个文件流中读取数据
                            读取count个元素，每个元素size字节
 输入参数   : void *buffer-用于接收数据的内存地址
                                大小至少是size*count字节
                            unsigned int size-单个元素大小，单位是字节
                            unsigned int count-素的个数，每个元素是size字节
                            RFILE *stream-输入流
 输出参数   :
 返 回 值   : 成功返回实际读取的元素数，,失败返回 -1
                        如果返回值与count（不是count*size）不相同
                        则可能文件结尾或发生错误
*************************************************************************/
int BSP_fread(void*buf,unsigned int size,unsigned int count, RFILE *stream)
{
    int iresult = bsp_linux_fread(buf, (count*size), stream);

    iresult = ((iresult == (count*size))?count:iresult);
    return iresult;
}
/*************************************************************************
 函 数 名   : BSP_fwrite
 功能描述   : 向一个文件流写入数据块
 输入参数   : const void *buffer-要输出数据的地址
                             unsigned int size-要写入内容的单字节数
                             unsigned int count-要进行写入size字节的数据项的个数
                             RFILE *stream-目标文件指针
 输出参数   :
 返 回 值   : 成功返回写入的数据量，失败返回-1
*************************************************************************/
int BSP_fwrite(const void* buf,unsigned int size,unsigned int count,RFILE *stream)
{
    int iresult = bsp_linux_fwrite((void *)buf, (count*size), stream);

    iresult = ((iresult == (count*size))?count:iresult);
    return iresult;
}
/*************************************************************************
 函 数 名   : BSP_fclose
 功能描述   : 关闭一个文件流
 输入参数   : RFILE *fp-要操作的文件指针
 输出参数   :
 返 回 值   : 成功返回0，失败返回-1
*************************************************************************/
int BSP_fclose(RFILE *fp)
{
    return (bsp_linux_fclose(fp));
}

void *mdrv_file_open(const char *path, const char *mode)
{
    return (void *)BSP_fopen(path, mode);
}

int mdrv_file_seek(void *stream, long offset, int whence)
{
    return BSP_fseek((RFILE *)stream, offset, whence);
}

long mdrv_file_tell(void *stream)
{
    return BSP_ftell((RFILE *)stream);
}

int mdrv_file_read(void *ptr, unsigned int size, unsigned int number, void *stream)
{
    return BSP_fread(ptr,size,number, (RFILE *)stream) ;
}
int mdrv_file_write(void *ptr, unsigned int size, unsigned int number, void *stream)
{
    return BSP_fwrite(ptr,size,number,(RFILE *)stream);
}

int mdrv_file_close(void *fp)
{
    return BSP_fclose((RFILE *)fp);
}


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif
