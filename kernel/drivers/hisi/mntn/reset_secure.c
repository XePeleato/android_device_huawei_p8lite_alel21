/*
 *****************************************************************************
 * Copyright (c) 2001-2021, Huawei Tech. Co., Ltd. All rights reserved.
 *
 * File Name    : secure.c
 * Author       : chenyingguo 61362
 * Created      : 2012/9/13
 * Description  : secure boot for dx
 *
 *****************************************************************************
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *****************************************************************************
 */

/*
 *****************************************************************************
 * 1 Header File Including
 *****************************************************************************
 */
#include <linux/slab.h>
#include <asm/delay.h>
#include "soc_baseaddr_interface.h"
#include "soc_peri_sctrl_interface.h"
#ifdef CONFIG_ARM64
#include <linux/platform_device.h>
#include <linux/dma-mapping.h>
#endif

#if defined (CHIP_BB_HI6210)
#include "soc_modem_sctrl_interface.h"
#define SOC_SC_ON_BASE_ADDR         SOC_AO_SCTRL_BASE_ADDR
#define SOC_SC_ON_BASE_SIZE         (0x2000)
#endif
#include <soc_ao_sctrl_interface.h>

#define DX_SB_HASH_LENGTH_IN_WORDS      (4)

#include "crypto_driver_defs.h"
#include "dx_pal_types.h"
#include "dx_pal_types_plat.h"
#include "sb_hash.h"
#include "nvm.h"
#include "bootimagesverifier_def.h"
#include "bootimagesverifier_api.h"
#include "bootimagesverifier_parser.h"
#include "bootimagesverifier_swvalidation_anddecryption.h"
#include "secureboot_basetypes.h"
#include "reset_secure.h"
#include <linux/hisi/hi6xxx-iomap.h>
//#if 1
//#include "../../../../external/dx/secure_boot/boot_images_verifier/bootimagesverifier_def.h"

//#include "../../../../external/dx/secure_boot/boot_images_verifier/bootimagesverifier_parser.h"

//#include "../../../../external/dx/secure_boot/boot_images_verifier/bootimagesverifier_swvalidation.h"
//#include "../../../../external/dx/secure_boot/host/src/sbromlib/sbrom_management_defs.h"
//#endif
//#include "../../mmc/host/hi6620_mmc_raw.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*
 *****************************************************************************
 * 2 Global Variable Definition
 *****************************************************************************
 */
 /* VRL Magic number */
#define DX_VRL_MAGIC_NUMBER_DEFAULT_VALUE 0xE59FF052

#define TRUE    1
#define FALSE   0

#define ERROR  -1
#define OK      0

#define EMMC_BLOCK_SIZE                 512
#define FLASH_PAGE_SIZE 2048


/* 定义VRL的解析结构体 */
struct vrl_parse_stru g_vrl_parse_st;

/* DX安全校验所需的buffer指针 */
unsigned long *g_work_space = 0;

/* 记录VRL中的最小软件版本号 */
unsigned int g_sw_min_version = 0;

/* 记录当前需要使用的公钥索引 */
unsigned int g_otp_key_index = 0;

/* 防止串货的结构体 */
struct oem_stru g_oem_st;

/* 安全镜像的加载地址 */
unsigned int g_image_load_addr = 0;

/*用于标识是哪个映像*/
vrl_id  g_image_id = 0;
char *g_emmc_read_buf = NULL;



/*
 *****************************************************************************
 * 3 Function Definition
 *****************************************************************************
 */
char * get_emmc_buff(void)
{
    if (NULL == g_emmc_read_buf)
    {
        g_emmc_read_buf = (char*)kmalloc(FLASH_PAGE_SIZE*2, GFP_KERNEL);
        if (NULL == g_emmc_read_buf)
        {
            printk(KERN_ERR"fail to malloc for emmc buff\n");
        }
    }
    return g_emmc_read_buf;
}
#ifndef CONFIG_ARM64
/*
 *****************************************************************************
 * Prototype    : efuse_power_on
 * Description  : power on the ldo for efuse
 * Input        : None
 * Output       : None
 * Return Value : None
 * Author       : chenyingguo 61362
 *****************************************************************************
 */
void efuse_power_on(void)
{
}

/*
 *****************************************************************************
 * Prototype    : efuse_power_off
 * Description  : power off the ldo for efuse
 * Input        : None
 * Output       : None
 * Return Value : None
 * Author       : chenyingguo 61362
 *****************************************************************************
 */
void efuse_power_off(void)
{
}


/*
 *****************************************************************************
 * Prototype    : test_bit_value
 * Description  : check bits in register whether are equal to the special value
 * Input        : addr：  register address
                  bit_mask：bits to be check
                  bit_value：special value
 * Output       : None
 * Return Value : TRUE: equal
                  FALSE:not equal
 * Author       : chenyingguo 61362
 *****************************************************************************
 */
static unsigned int test_bit_value(void* addr, unsigned int bit_mask, unsigned int bit_value)
{
#define TIME_DELAY_MAX    1000000       /*最大延迟时间,1秒*/
    unsigned int time;

    for (time = 0; time < TIME_DELAY_MAX; time++) {
        if (bit_value == (READ_REGISTER_ULONG(addr) & bit_mask)) {
            return TRUE;
        }
        udelay(1);
    }

    return FALSE;
}


/*
 *****************************************************************************
 * Prototype    : read_efuse
 * Description  : read special group of efuse
 * Input        : group：special group of efuse
                  buf：  data buffer
                  len：  length of the group
 * Output       : None
 * Return Value : OK/ERROR
 * Author       : chenyingguo 61362
 *****************************************************************************
 */
static int read_efuse(unsigned int group, unsigned int *buf, unsigned int len)
{
    unsigned int *ptmp = buf;
    unsigned int cnt;
    void *efuse_base_addr = (void*)ioremap_nocache(EFUSE_BASE_REG, 0x2000);
    void *soc_ao_sc_base_addr = (void*)ioremap_nocache(SOC_AO_SCTRL_BASE_ADDR, 0x2000);
    /* 入参判断 */
    if (0 == buf) {
        goto error;
    }

    if ((group > (EFUSE_MAX_SIZE / 4) )
        || ((group + len) > (EFUSE_MAX_SIZE / 4) )) {
        goto error;
    }

    /* 打开EFUSE clk */
    SETBITVALUE32(SOC_AO_SCTRL_SC_PERIPH_CLKEN4_ADDR(soc_ao_sc_base_addr), \
                (1 << SOC_AO_SCTRL_SC_PERIPH_CLKEN4_periph_clken4_10pclk_efusec_START), \
                (1 << SOC_AO_SCTRL_SC_PERIPH_CLKEN4_periph_clken4_10pclk_efusec_START));

    /*设置EFUSE接口时序为内部产生*/
    SETBITVALUE32(EFUSEC_CFG(efuse_base_addr), EFUSEC_SIG_SEL_MASK, EFUSEC_SIG_SEL_MASK);

    /* 循环读取Efuse值 */
    for (cnt = 0; cnt < len; cnt++) {
        /* 设置读取地址 */
        OUTREG32(EFUSE_GROUP(efuse_base_addr), group + cnt);

        /* 使能读 */
        SETBITVALUE32(EFUSEC_CFG(efuse_base_addr), EFUSEC_RD_EN_MASK, EFUSEC_RD_EN_MASK);

        /* 等待读使能设置成功，读使能超时返回错误 */
        if (!test_bit_value(EFUSEC_CFG(efuse_base_addr), EFUSEC_RD_EN_MASK, 0)) {
            goto error;
        }

        /* 等待读完成 */
        if (!test_bit_value(EFUSEC_STATUS(efuse_base_addr), EFUSEC_RD_STATUS, EFUSEC_RD_STATUS)) {
            goto error;
        }

        /* 读取数据 */
        *ptmp = INREG32(EFUSEC_DATA(efuse_base_addr));
        ptmp++;
    }

    /* AIB select */
    SETBITVALUE32(EFUSEC_CFG(efuse_base_addr), EFUSEC_SIG_SEL_INNER, 0);

    /* 关闭EFUSE clk */
    SETBITVALUE32(SOC_AO_SCTRL_SC_PERIPH_CLKDIS4_ADDR(soc_ao_sc_base_addr), \
                (1 << SOC_AO_SCTRL_SC_PERIPH_CLKEN4_periph_clken4_10pclk_efusec_START), \
                (1 << SOC_AO_SCTRL_SC_PERIPH_CLKEN4_periph_clken4_10pclk_efusec_START));

    iounmap((void*)soc_ao_sc_base_addr);
    iounmap((void*)efuse_base_addr);
    return OK;
error:

    /* 关闭EFUSE clk */
    SETBITVALUE32(SOC_AO_SCTRL_SC_PERIPH_CLKDIS4_ADDR(soc_ao_sc_base_addr), \
                (1 << SOC_AO_SCTRL_SC_PERIPH_CLKEN4_periph_clken4_10pclk_efusec_START), \
                (1 << SOC_AO_SCTRL_SC_PERIPH_CLKEN4_periph_clken4_10pclk_efusec_START));

    iounmap((void*)soc_ao_sc_base_addr);
    iounmap((void*)efuse_base_addr);
    return ERROR;
}


/*****************************************************************************
 函 数 名  : get_efuse_value
 功能描述  : 获取efuse的值（支持跨group）
 输入参数  : unsigned int start_bit
             unsigned int * buffer
             unsigned int bit_cnt
 输出参数  : 无
 返 回 值  : int
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年3月4日
    作    者   : 陈迎国 61362
    修改内容   : 新生成函数

*****************************************************************************/
static int get_efuse_value(unsigned int start_bit, unsigned int * buffer, unsigned int bit_cnt)
{
    unsigned int group_start;
    unsigned int read_cnt;
    unsigned int unalinged_size;

    group_start    = start_bit/32;
    unalinged_size = start_bit%32;
    read_cnt       = (unalinged_size + 31)/32;
    read_cnt      += (bit_cnt - unalinged_size + 31)/32;

    if(ERROR == read_efuse(group_start, buffer, read_cnt)) {
        return ERROR;
    }

    return OK;
}
#else
extern int atfd_hisi_fn_smc(u64 function_id, u64 arg0, u64 arg1,  u64 arg2);
#endif

/*
 *****************************************************************************
 * Prototype    : check_oem_hw
 * Description  : check OEM_ID and HW_ID in efuse
 * Input        : None
 * Output       : None
 * Return Value : OK
                  ERROR
 * Author       : chenyingguo 61362
 *****************************************************************************
 */
int check_oem_hw(void)
{ 
    unsigned int carrier_id = 0;
#ifdef CONFIG_ARM64
    unsigned char *buffer = NULL;
    struct device *pdevice = NULL;
    dma_addr_t g_phy_addr = 0;
    int ret = OK;
#else
    unsigned char buffer[64];
    unsigned int start_bit = 0;
    unsigned char bit_cnt = 0;
    unsigned char * ptmp = NULL;
#endif

#ifdef CONFIG_ARM64
#define EFUSE_FN_GET_CARRIERID                                   0xc500000D
    pdevice = (struct device *)kmalloc(sizeof(struct device), GFP_KERNEL);
    if(NULL == pdevice)
    {
        printk(KERN_ERR "[SEC]check_oem_hw: kmalloc allocate memory error\n");
        return ERROR;
    }
     
    buffer = (unsigned char *)dma_alloc_coherent((void*)pdevice, 4, &g_phy_addr, GFP_KERNEL);
    if(NULL == buffer)
    {
        kfree(pdevice);
        printk(KERN_ERR "[SEC]check_oem_hw: dma_alloc_coherent allocate memory error\n");
        return ERROR;
    }

    ret = atfd_hisi_fn_smc(EFUSE_FN_GET_CARRIERID, (u64)g_phy_addr, 2, 0);
    if (OK != ret)
    {
        printk(KERN_ERR "[SEC]check_oem_hw: get_efuse_carrierid_value error, ret=%d\n", ret);
        dma_free_coherent(pdevice, 4, buffer, g_phy_addr);
        kfree(pdevice);
        return ERROR;
    }
	carrier_id = (unsigned int)(buffer[0] + (buffer[1] << 8));
    dma_free_coherent(pdevice, 4, buffer, g_phy_addr);
    kfree(pdevice);
#else
    /* read carrier id efuse unit */
    start_bit = EFUSE_CARRIER_ID_BIT_START;
    bit_cnt = EFUSE_CARRIER_ID_BIT_SIZE;
    if (ERROR == get_efuse_value(start_bit, (unsigned int *)buffer, bit_cnt))
    {
        printk(KERN_ERR "[SEC]check_oem_hw: get_efuse_value error, start=%d\n", start_bit);
        return ERROR;
    }

    ptmp                = buffer + ((start_bit % 32) / 8);
    carrier_id          = (unsigned int)(*ptmp + ((*(ptmp + 1)) << 8));
#endif
    printk(KERN_ERR "[SEC]check_oem_hw: carrier_id = 0x%x\n", carrier_id);

    g_oem_st.flag       = 1;
    g_oem_st.carrier_id = carrier_id;
    g_oem_st.hw_id      = 0;
    return OK;
}

/*
 *****************************************************************************
 * Prototype    : check_secure_mode
 * Description  : check whether is in secure mode
 * Input        : None
 * Output       : None
 * Return Value : TRUE: secure mode
                  FALSE: not secure mode
 * Author       : chenyingguo 61362
 *****************************************************************************
 */
int check_secure_mode(void)
{
    SOC_AO_SCTRL_SC_SYS_STAT1_UNION *p_sc_sys_stat1;
    char* ao_base_addr = NULL;
    unsigned int temp;
    static int ret = -1;
    /* 避免频繁的访问efuse */
    if (-1 != ret) {
        return ret;
    }
    ao_base_addr = (char*)ioremap_nocache(SOC_SC_ON_BASE_ADDR, SOC_SC_ON_BASE_SIZE);

    /* 检查系统状态寄存器，读取其中的安全模式bit */
    p_sc_sys_stat1 = (SOC_AO_SCTRL_SC_SYS_STAT1_UNION *)
      SOC_AO_SCTRL_SC_SYS_STAT1_ADDR(ao_base_addr);
    temp = p_sc_sys_stat1->reg.security_boot_flg;

    /* 如果该bit为0，返回FALSE，表示是非安全模式 */
    if (temp == 0) {
        ret = FALSE;
    }
    else {
        /* 如果该bit为1，需要检查efuse中的OEM_ID和HW_ID */
        if(OK == check_oem_hw()) {
            /* 安全模式*/
            ret = TRUE;
        }
        else {
            /* 非安全模式*/
            ret = FALSE;
        }
    }
    iounmap((void*)ao_base_addr);
    return ret;
}

/*
 *****************************************************************************
 * Prototype    : sec_emmc_read
 * Description  : emmc read function for secure verify of DX
 * Input        : addr: emmc address
                  data: data buffer
                  size: length of data buffer
                  context: unused
 * Output       : None
 * Return Value : OK/ERROR
 * Author       : chenyingguo 61362
 *****************************************************************************
 */
unsigned int sec_emmc_read(unsigned int addr, unsigned char * data, unsigned int size, void *context)
{
    unsigned int ret;
    unsigned int start_block;
    unsigned int block_count;
    unsigned int unalign_size;

    context = context;
    start_block = addr/EMMC_BLOCK_SIZE;
    unalign_size = addr%EMMC_BLOCK_SIZE;
    block_count = (unalign_size + size + EMMC_BLOCK_SIZE - 1 )/EMMC_BLOCK_SIZE;
    /* This API no longer supported and nobody used sec_emmc_read now */
    // ret = k3v2_mmc_raw_read((char*)data, start_block, block_count);
    ret = -1;

    if (-1 == ret) {
        return ERROR;
    }

    if (0 != unalign_size)
    {
        hisi_io_memcpy(data, data + unalign_size, size);
    }

    return OK;
}

/*
 *****************************************************************************
 * Prototype    : check_vrl
 * Description  : check vrl in emmc,if vrl is bad,restore from vrl backup area
 * Input        : vrl_addr: vrl address in vrl area
                  vrl_addr_bk: vrl backup address in vrl backup area
 * Output       : None
 * Return Value : OK/ERROR
 * Author       : chenyingguo 61362
 *****************************************************************************
 */
int check_vrl(unsigned int vrl_addr, unsigned int vrl_bk_addr)
{
    VRL_Header_t *vrl_header;
    unsigned char *vrl_buf = (unsigned char *)get_emmc_buff();
    unsigned char *vrl_bk_buf = vrl_buf + VRL_SIZE;
    unsigned int ret;

    /* 读取VRL数据 */
    ret = sec_emmc_read(vrl_addr, vrl_buf, VRL_SIZE, 0);
    ret |= sec_emmc_read(vrl_bk_addr, vrl_bk_buf, VRL_SIZE, 0);
    if (OK != ret) {
        return ERROR;
    }

    if (1 == g_oem_st.flag) {
        /* 校验Carrier ID和HW ID */
        if ((*(unsigned int *)vrl_buf != g_oem_st.carrier_id ) ||
             (*(unsigned int *)(vrl_buf + 4) != g_oem_st.hw_id)) {
                printk(KERN_ERR"check Carrier ID and HW ID failed!\n");
             return ERROR;;
         }
    }

    /* 检查VRL是否正确 */
    vrl_header = (VRL_Header_t *)(vrl_buf + ADDITIONAL_DATA_SIZE);
    if (vrl_header->magicNumber != DX_VRL_MAGIC_NUMBER_DEFAULT_VALUE) {
        vrl_header = (VRL_Header_t *)(vrl_bk_buf + ADDITIONAL_DATA_SIZE);
        if (vrl_header->magicNumber != DX_VRL_MAGIC_NUMBER_DEFAULT_VALUE) {
            return ERROR;
        }
        else {
 #if 0
            if (OK != secure_write_vrl(vrl_addr, vrl_bk_buf, VRL_SIZE)) {
                return ERROR;
            }
 #endif
        }
    }
    else {
        vrl_header = (VRL_Header_t *)(vrl_bk_buf + ADDITIONAL_DATA_SIZE);
        if (vrl_header->magicNumber != DX_VRL_MAGIC_NUMBER_DEFAULT_VALUE) {
#if 0
            if (OK != secure_write_vrl(vrl_bk_addr, vrl_buf, VRL_SIZE)) {
                (void)bprintf("secure_write_vrl write vrl bk failed!\n" );
            }
#endif
        }
    }

    return OK;
}

/*
 *****************************************************************************
 * Prototype    : check_vrl_valid
 * Description  : check whether the vrl is valid
 * Input        : buf
 * Output       : None
 * Return Value : OK/ERROR
 * Author       : chenyingguo 61362
 *****************************************************************************
 */
int check_vrl_valid(unsigned int buf)
{
    VRL_Header_t *vrl_header;
    vrl_header = (VRL_Header_t *)((unsigned long)buf);

    if (vrl_header->magicNumber == DX_VRL_MAGIC_NUMBER_DEFAULT_VALUE) {
        return OK;
    }
    else {
        return ERROR;
    }
}


/*
 *****************************************************************************
 * Prototype    : check_image_in_emmc
 * Description  : check image in emmc,use APIs of DX
 * Input        : secure_image_id: secure image id for vrl
 * Output       : None
 * Return Value : OK/ERROR
 * Author       : chenyingguo 61362
 *****************************************************************************
 */
int check_image_in_emmc(int secure_image_id)
{
      unsigned int vrl_addr;
    unsigned int vrl_addr2;
    DX_BIM_VRL_TABLE_t vrlList;
    unsigned long index;
    unsigned long *work_space;
    void __iomem *soc_peri_sc_base_addr = ioremap_nocache(SOC_PERI_SCTRL_BASE_ADDR, 0x2000);
    DxError_t ret = OK;

    if (0 == g_work_space)
    {
        g_work_space = (unsigned long *)kmalloc(WORKSPACE_SIZE, GFP_KERNEL);
        if (0 == g_work_space)
        {
            return ERROR;
        }
    }

    work_space = g_work_space;


    /* 检查安全映像ID */
    if ((secure_image_id < 0) || (secure_image_id >= (int)ID_BUTTOM)) {
        printk(KERN_ERR"secure image id 0x%x is invalid!\n", secure_image_id);
        return ERROR;
    }

    /* 打开seceng clk */
    SETBITVALUE32(SOC_PERI_SCTRL_SC_PERIPH_CLKEN2_ADDR(soc_peri_sc_base_addr), \
            (1 << SOC_PERI_SCTRL_SC_PERIPH_CLKEN2_periph_clken2_seceng_acpu_START), \
            (1 << SOC_PERI_SCTRL_SC_PERIPH_CLKEN2_periph_clken2_seceng_acpu_START));

    /* 根据安全映像ID，找到映像对应的VRL在ptable中的地址 */
    vrl_addr  = VRL_AREA1_OFFSET + (unsigned int)secure_image_id*VRL_SIZE;
    vrl_addr2 = VRL_AREA2_OFFSET + (unsigned int)secure_image_id*VRL_SIZE;
    if (ERROR == check_vrl(vrl_addr, vrl_addr2)) {
        printk(KERN_ERR"check_vrl failed, id is 0x%x!\n", secure_image_id);
        return ERROR;
    }

    /* 填充vrlList，其中定义VRL的存储地址 */
    vrlList[0].VRL_Address = vrl_addr;
    vrlList[0].OTP_Key_Index = g_otp_key_index;
    vrlList[0].Magic_Number = 0;

    /* 调用DX_BIV_SwImageVerification，进行安全校验*/
    ret = DX_BIV_SwImageVerification((DX_SB_FlashRead)sec_emmc_read,
                                        0,
                                        SOC_SECENG_BASE_ADDR,
                                        vrlList,
                                        1,
                                        (DxUint32_t *)&index,
                                        (DxUint32_t *)work_space,
                                        WORKSPACE_SIZE);
    if(OK != ret) {
        printk(KERN_ERR"DX_BIV_SwImageVerification image id is 0x%x, return is 0x%x !\n",
                secure_image_id, ret);
    }
    kfree(g_work_space);
    g_work_space = 0;

    /* 关闭seceng clk */
    OUTREG32(SOC_PERI_SCTRL_SC_PERIPH_CLKDIS2_ADDR(soc_peri_sc_base_addr), \
            (1 << SOC_PERI_SCTRL_SC_PERIPH_CLKEN2_periph_clken2_seceng_acpu_START));

    iounmap(soc_peri_sc_base_addr);
    return (int)ret;
}
/*
 *****************************************************************************
 * Prototype    : parse_vrl
 * Description  : parse vrl,get the size of vrl,number of image
 * Input        : vrl_data:  pointer to the vrl buffer
 * Output       : vrl_size:  size of vrl
                  image_num: number of image
                  image_size:size of image
 * Return Value : OK/ERROR
 * Author       : chenyingguo 61362
 *****************************************************************************
 */
int parse_vrl(unsigned int * vrl_data, unsigned int *vrl_size, unsigned int *image_num,
               unsigned int *image_size)
{
    VRL_Header_t *vrl_header;
    unsigned char tmp;
    unsigned int img_num;
    unsigned int size = 0;
    VRL_SW_Version_t *vrl_sw_version;

    vrl_header = (VRL_Header_t *)((unsigned long)vrl_data + ADDITIONAL_DATA_SIZE);

    /* additional data,128Bytes */
    size += ADDITIONAL_DATA_SIZE;

    /* vrl header */
    size += sizeof(VRL_Header_t);

    /* RSA pubkey */
    size += RSA_KEY_SIZE;

    /* barrett n' */
    size += BARRETT_SIZE;

    /* software version */
    vrl_sw_version = (VRL_SW_Version_t *)((unsigned long)vrl_data + size);
    if (g_sw_min_version < vrl_sw_version->minAllowedSwVersion) {
        /* minAllowedSwVersion in secondary vrl is zero, so get from primary vrl */
        g_sw_min_version = vrl_sw_version->minAllowedSwVersion;
    }
    size += SW_VER_SIZE;

    /* secondary key hash */
    tmp = (vrl_header->vrlFlags) & 0xff;
    if(tmp == (int)HASH_SHA256_Alg_Output) {
        tmp = HASH_256_SIZE;
    }
    else if(tmp == (int)HASH_SHA256_Alg_128_Output) {
        tmp = HASH_128_SIZE;
    }
    else
    {
        return ERROR;
    }
    size += tmp;

    /* image info,include hash and load address */
    img_num = ((vrl_header->vrlSize)>>16) & 0xFFFF;
    if (img_num > 1) {
        return ERROR;
    }
    else if (1 == img_num) {
        g_image_load_addr = (*(unsigned int *)((unsigned long)vrl_data + size));
    }
    size += (tmp + IMG_LOADADDR_SIZE)*img_num;

    /* signature of vrl */
    size += RSA_KEY_SIZE;

    /* secondary vrl address */
    size += SECONDARY_VRLADDR_SIZE;

    /* image info,include store address and image size */
    if (img_num) {
        *image_size = (*(unsigned int *)((unsigned long)vrl_data + size + 4))*4;
    }
    size += IMG_INFO_SIZE*img_num;

    *vrl_size = size;
    *image_num = img_num;

    return OK;
}
/*
 *****************************************************************************
 * Prototype    : sec_ram_read
 * Description  : ram read function for secure verify of DX
 * Input        : addr: ram address
                  data: data buffer
                  size: length of data buffer
                  context: unused
 * Output       : None
 * Return Value : OK
 * Author       : chenyingguo 61362
 *****************************************************************************
 */
static unsigned int sec_ram_read(unsigned int addr, unsigned char * data,
                              unsigned int size, void *context)
{
    addr = addr;
    context = context;
    data = data;
    size = size;
    return 0;
}

 /*
 *****************************************************************************
 * Prototype    : check_image_in_ram
 * Description  : check image in ram,use APIs of DX
 * Input        : image_buf: image buffer in ram
 * Output       : None
 * Return Value : OK/ERROR
 * Author       : chenyingguo 61362
 *****************************************************************************
 */
int check_image_in_ram(unsigned int *image_buf)
{
    DxUint32_t swVersion;
    unsigned int ret;
    HASH_Result_t pubKeyHASH;
    VRL_SecAddress_t secVrlAddress_ptr;
    DxUint32_t secondaryNHash_ptr[HASH_128_SIZE/4];
    unsigned int image_num;
    struct vrl_parse_stru vrl_parse_st;
    VRL_Parser_CompsData_t swCompsData;
    DxUint8_t isSecExist = 0;

    char *virAddr_64 = (char *)ioremap_nocache(SOC_SECENG_BASE_ADDR, 0x1000);
    /* DX dont support 64bit address */
    unsigned long virAddr = (unsigned long) virAddr_64;
    if (!virAddr) {
      printk("check_image_in_ram failed in ioremap physical addr 0x%x\n", SOC_SECENG_BASE_ADDR);
      return ERROR;
    }
    /* 调用NVM_GetSwVersion获取efuse中的最小版本号 */
    ret = NVM_GetSwVersion((unsigned int)virAddr, &swVersion);
    if(DX_OK != ret) {
        (void)printk("NVM_GetSwVersion failed, return is 0x%x !\n", ret);
        goto error;
    }

    /* 调用DX_BIV_GetPrimaryKeyHash获取efuse中的公钥Hash */
    ret = DX_BIV_GetPrimaryKeyHash(NVM_ReadHASHPubKey, (unsigned int)virAddr,
                                   pubKeyHASH, 0, 0);
    if(DX_OK != ret) {
        (void)printk("DX_BIV_GetPrimaryKeyHash failed, return is 0x%x !\n", ret);
        goto error;
    }

    /* 调用DX_BIV_VRLParser校验image_buf中的一级VRL，校验时获取到二级公钥的HASH */
    ret = DX_BIV_VRLParser((DxUint8_t)DX_TRUE,
                          (unsigned int) virAddr,
                           pubKeyHASH,
                           swVersion,
                           (DxUint32_t *)image_buf,
                           &secVrlAddress_ptr,
                           secondaryNHash_ptr);
    if(DX_OK != ret) {
        (void)printk("DX_BIV_VRLParser failed, return is 0x%x !\n", ret);
        goto error;
    }

    /* 计算一级VRL的大小 */
    ret = (unsigned int)parse_vrl(image_buf, &vrl_parse_st.vrl1_size,
                    &image_num, &vrl_parse_st.image_size);
    if(DX_OK != ret) {
        (void)printk("parse primary vrl failed, return is 0x%x !\n", ret);
        goto error;
    }

    if(image_num != 0)
    {
        (void)printk("image_num 0x%x in primary vrl is bad !\n", image_num);
        goto error;
    }

    /* 调用DX_BIV_VRLParser校验image_buf中的二级VRL */
    ret = DX_BIV_VRLParser((DxUint8_t)DX_FALSE,
                           (unsigned int)virAddr,
                           (DxUint32_t *)pubKeyHASH,
                           swVersion,
                           (DxUint32_t *)((unsigned long)image_buf + vrl_parse_st.vrl1_size),
                           (VRL_SecAddress_t *)&secVrlAddress_ptr,
                           secondaryNHash_ptr);
    if(DX_OK != ret) {
        (void)printk("DX_BIV_VRLParser failed, goto is 0x%x !\n", ret);
        goto error;
    }

    /* 计算二级VRL的大小 */
    ret = (unsigned int)parse_vrl((unsigned int *)((unsigned long)image_buf +
                     vrl_parse_st.vrl1_size),
                     (unsigned int *)&vrl_parse_st.vrl2_size,
                     (unsigned int *)&image_num,
                     (unsigned int *)&vrl_parse_st.image_size);
    if(DX_OK != ret) {
        (void)printk("parse secondary vrl failed, return is 0x%x !\n", ret);
        goto error;
    }

    if(image_num != 1)
    {
        (void)printk("image_num 0x%x in secondary vrl is bad !\n", image_num);
        goto error;
    }

	/* 调用DX_BIV_SWCompValidation校验二级VRL中带的映像 */
    ret = DX_BIV_SWCompValidation((DX_SB_FlashRead)sec_ram_read,
                                  0,
                                  (unsigned int)virAddr,
                                  &swCompsData,
                                  &isSecExist,
                                  (DxUint32_t *)((unsigned long)image_buf + (unsigned long)vrl_parse_st.vrl1_size),
                                  VRL_SIZE);
    if(DX_OK != ret) {
        (void)printk("image is invalid, ret 0x%x!\n", ret);
        goto error;
    }
    else {
        g_vrl_parse_st = vrl_parse_st;
	    iounmap((void*)virAddr_64);
        return OK;
    }
 error:
    iounmap((void*)virAddr_64);
    return ERROR;
}
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

