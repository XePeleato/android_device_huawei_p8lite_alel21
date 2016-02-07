
#include <linux/module.h>
#include <linux/slab.h>
#include "rfilesystem_process.h"
#include "rfilesystem_logunit.h"
#include <linux/hisi/reset.h>
#include "reset_secure.h"
#include "reset_ccoreloader.h"
#include <linux/io.h>
#include <linux/delay.h>
#include <linux/dma-mapping.h>

#include "MemoryMap.h"
#include <linux/hisi/hi6xxx-iomap.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

unsigned int phy_img_head_ccore = 0;
unsigned int phy_img_buf_ccore = 0;

int      g_load_modem_addr = 0;
int      g_load_bbe16_addr = 0;


/*****************************************************************************
变量引用
*****************************************************************************/


/*****************************************************************************
函数声明
*****************************************************************************/
extern int ccore_drv_read_bin(const char *partion_name, unsigned int offset, unsigned int length, char *buffer);

/*****************************************************************************
函数定义
*****************************************************************************/
#ifdef  CONFIG_TZDRIVER
/*The modem load and verfication is move into SEC_OS, look in load_image.c*/
#else
int is_binary_section_invalid(CCORE_IMAGE_SEC_STRU* section, CCORE_IMAGE_HEAD_STRU* header)
{



    if ((section->uhwSn >= header->uwSecNum) ||
        (section->uwSrcOffset + section->uwSize > header->uwImageSize))
    {
        return TRUE;
    }

    if ((section->enType >= CCORE_IMAGE_SEC_TYPE_BUTT) ||
        (section->enLoadAttrib >= CCORE_IMAGE_SEC_LOAD_BUTT))
    {
        return TRUE;
    }
    return FALSE;

}

/*****************************************************************************
 函 数 名  : ccorereset_loadcbin
 功能描述  : 加载CCORE映像，并解复位CCORE
 输入参数  : 无
 输出参数  : 无
 返 回 值  : int
*****************************************************************************/
int ccorereset_loadcbin(void)
{
    const char      *image_name = "modemimage";
    unsigned int    i = 0;
    char*   iviraddr = NULL;

    char*   iviraddr_dest1 = NULL;

    char*   iviraddr_src1 = NULL;


    int             iresult_read = 0;
    char            *header_buff = 0;

    CCORE_IMAGE_HEAD_STRU *header = NULL;

    header_buff = kmalloc(sizeof(CCORE_IMAGE_HEAD_STRU), GFP_ATOMIC);

    if (NULL == header_buff)
    {
        printk(KERN_ERR "%s: fail to malloc\n", __FUNCTION__);
        return BSP_RESET_ERROR;
    }
    hisi_io_memset((void*)header_buff, 0, sizeof(CCORE_IMAGE_HEAD_STRU));

    /*get modem info, esp size*/
    iresult_read = ccore_drv_read_bin((const char*)image_name, 0, sizeof(CCORE_IMAGE_HEAD_STRU), header_buff);
    if (iresult_read < 0)
    {
        printk(KERN_ERR "%s: fail to read the head of modem image\n", __FUNCTION__);
        kfree(header_buff);
        header_buff = NULL;
        return BSP_RESET_ERROR;
    }
    header = (CCORE_IMAGE_HEAD_STRU*)header_buff;
    printk(KERN_INFO "%s: image size = %d, sec num = %d\n", __FUNCTION__, header->uwImageSize, header->uwSecNum);

    for(i = 0; i < header->uwSecNum; i++)
    {
        /* check the sections */
        if (is_binary_section_invalid(&header->astSections[i], header))
        {
            printk(KERN_ERR "%s: invalid binary section.\n", __FUNCTION__);
        kfree(header_buff);
        header_buff = NULL;
            return BSP_RESET_ERROR;
        }

        /* 如果是BUFFER属性，段内存仅加载到缓存buffer中 */
        if (header->astSections[i].enLoadAttrib == CCORE_IMAGE_SEC_LOAD_BUFFER )
        {
            iviraddr_src1 = (char*)(&header->astSections[i]);

            printk(KERN_INFO "%s: buffer, sec src virt = %p\n", __FUNCTION__, iviraddr_src1);
            iviraddr_dest1 = ioremap_wc(ECS_GUDSP_LOAD_ADDR, (sizeof(CCORE_IMAGE_SEC_STRU) + header->astSections[i].uwSize));

            if (NULL == (void*)iviraddr_dest1)
            {
                printk(KERN_ERR "%s: fail to malloc in load c bin function\n", __FUNCTION__);
                kfree(header_buff);
                header_buff = NULL;
                return BSP_RESET_ERROR;
            }

            printk(KERN_INFO "%s: buffer, sec dest virt = %p\n", __FUNCTION__, iviraddr_dest1);

            hisi_io_memcpy((void*)iviraddr_dest1, (void*)iviraddr_src1, sizeof(CCORE_IMAGE_SEC_STRU));

            iresult_read = ccore_drv_read_bin((const char*)image_name, header->astSections[i].uwSrcOffset, header->astSections[i].uwSize, (char*)(iviraddr_dest1 + sizeof(CCORE_IMAGE_SEC_STRU)));
            printk(KERN_INFO "%s: size %d, iresult_read %d\n", __FUNCTION__, header->astSections[i].uwSize, iresult_read);
            iounmap((void*)iviraddr_dest1);
        }
        /* 如果是STATIC/MODEM_ENTRY属性，段内容仅加载到运行地址中 */
        else if ((header->astSections[i].enLoadAttrib == CCORE_IMAGE_SEC_LOAD_MODEM_ENTRY )
            || (header->astSections[i].enLoadAttrib == CCORE_IMAGE_SEC_LOAD_STATIC ))
        {
            iviraddr = ioremap_wc(header->astSections[i].uwDesOffset,header->astSections[i].uwSize);
            if (NULL == (void*)iviraddr)
            {
                printk(KERN_ERR "%s: fail to malloc in load c bin function\n", __FUNCTION__);
                kfree(header_buff);
                header_buff = NULL;
                return BSP_RESET_ERROR;
            }

            iresult_read = ccore_drv_read_bin((const char*)image_name, header->astSections[i].uwSrcOffset, header->astSections[i].uwSize, iviraddr);

            printk(KERN_INFO "%s: size %d, iresult_read %d\n", __FUNCTION__, header->astSections[i].uwSize, iresult_read);
            iounmap((void*)iviraddr);

            /*获得modem加载地址*/
            if (CCORE_IMAGE_SEC_LOAD_MODEM_ENTRY == header->astSections[i].enLoadAttrib)
            {
                g_load_modem_addr = header->astSections[i].uwDesOffset;
                printk(KERN_INFO "%s: modem load addr 0x%x\n", __FUNCTION__, (unsigned int)header->astSections[i].uwDesOffset);
            }
        }
        /* 如果是其他属性，不作任何处理 */
        else
        {
            printk(KERN_ERR "%s: do nothing In ccorereset_loadcbin\n" ,__FUNCTION__);
            /* do nothing */
        }
    }

    kfree(header_buff);
    header_buff = NULL;
    printk(KERN_INFO "%s: leave\n", __FUNCTION__);

   return BSP_RESET_OK;
}
/*****************************************************************************
 函 数 名  : ccorereset_loadcbin_loadaddr
 功能描述  : 获得c core映像加载地址
 输入参数  : 无
 输出参数  : 无
 返 回 值  : int
*****************************************************************************/
int ccorereset_loadcbin_loadaddr(void)
{
    return g_load_modem_addr;
}
#endif
/*****************************************************************************
 函 数 名  : ccorereset_loadbbebin
 功能描述  : 加载BBE16映像
 输入参数  : 无
 输出参数  : 无
 返 回 值  : int
*****************************************************************************/
int ccorereset_loadbbebin(void)
{
    int                    iRet = 0;
    char                   *image_name = "dsp";
    unsigned int           image_load_phy_addr = (GLOBAL_MEM_TDS_TABLE_ADDR);
    unsigned int           image_load_size = (GLOBAL_MEM_TDS_TABLE_SIZE + GLOBAL_MEM_LT_IMAGE_SIZE);
    char                   *buff = 0;

    g_load_bbe16_addr = image_load_phy_addr;

    buff = ioremap_wc(image_load_phy_addr,image_load_size);
    if (NULL == buff)
    {
        printk(KERN_ERR "%s: fail to remap space\n", __FUNCTION__);
        return BSP_RESET_ERROR;
    }
    hisi_io_memset((void*)buff, 0, image_load_size);
    printk(KERN_INFO "%s: read bbe16 bin to phy addr 0x%x, virt addr 0x%x, size = %d\n", __FUNCTION__, image_load_phy_addr, (unsigned int)buff, image_load_size);

    iRet = ccore_drv_read_bin((const char*)image_name, 0, image_load_size, (char *)buff);
    if(RFILE_OK != iRet)
    {
        printk(KERN_ERR "%s: read bbe16 bin fail\n", __FUNCTION__);
        iounmap((void*)buff);
    }

    iounmap((void*)buff);
    printk(KERN_INFO "%s: leave\n", __FUNCTION__);
    return iRet;
}

/*****************************************************************************
 函 数 名  : ccorereset_loadbbebin_loadaddr
 功能描述  : 获得bbe16映像加载地址
 输入参数  : 无
 输出参数  : 无
 返 回 值  : int
*****************************************************************************/
int ccorereset_loadbbebin_loadaddr(void)
{
    return g_load_bbe16_addr;
}

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif
