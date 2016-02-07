

//**********************************************************************
//#include <mach/platform.h>
#include "vpp_hal.h"

#define LOG_TAG "VPP_HAL"
#include "vpp_log.h"

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
          This is defined for FPGA test and SDK
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

S_VPP_REGS_TYPE *pVoReg = NULL;


u8 __iomem *s_xl_mmio = NULL;

#define VPP_REG(__B__,__O__)   (*((unsigned volatile*)(__B__ + (long long)__O__)))

void log_reg_value(void)
{
    int i =0;
#if 1
    printk("\n\n******************* reg list start *******************\n");
    printk("****VOCTRL(0)          : 0x%0x \n\n", VPP_REG(s_xl_mmio,0x0));

    printk("****VOINTSTA(0x4)      : 0x%0x \n", VPP_REG(s_xl_mmio,0x4));
    printk("****VOMSKINTSTA(0x8)   : 0x%0x \n", VPP_REG(s_xl_mmio,0x8));
    printk("****VOINTMSK(0xc)      : 0x%0x \n\n", VPP_REG(s_xl_mmio,0xc));

    printk("****VOPARAUP(0x40)     : 0x%0x \n", VPP_REG(s_xl_mmio,0x40));
    printk("****VHDHCOEFAD(0x44)   : 0x%0x \n", VPP_REG(s_xl_mmio,0x44));
    printk("****VHDVCOEFAD(0x48)   : 0x%0x \n", VPP_REG(s_xl_mmio,0x48));
    printk("****VHDCTRL(0x100)     : 0x%0x \n", VPP_REG(s_xl_mmio,0x100));
    printk("****VHDUPD(0x104)      : 0x%0x \n", VPP_REG(s_xl_mmio,0x104));
    printk("****VHDLADDR(0x108)    : 0x%0x \n", VPP_REG(s_xl_mmio,0x108));
    printk("****VHDLCADDR(0x10C)   : 0x%0x \n", VPP_REG(s_xl_mmio,0x10c));
    printk("****VHDCADDR(0x110)    : 0x%0x \n", VPP_REG(s_xl_mmio,0x110));
    printk("****VHDCCADDR(0x114)   : 0x%0x \n\n", VPP_REG(s_xl_mmio,0x114));
    printk("****VHDNADDR(0x118)    : 0x%0x \n", VPP_REG(s_xl_mmio,0x118));
    printk("****VHDNCADDR(0x11C)   : 0x%0x \n\n", VPP_REG(s_xl_mmio,0x11c));

    printk("****VHDSTRIDE(0x124)   : 0x%0x \n", VPP_REG(s_xl_mmio,0x124));
    printk("****VHDIRESO(0x128)    : 0x%0x \n", VPP_REG(s_xl_mmio,0x128));
    printk("****VHDWBC1ADDR(0x13c) : 0x%0x \n", VPP_REG(s_xl_mmio,0x13c));
    printk("****VHDWBC1STRD(0x140) : 0x%0x \n", VPP_REG(s_xl_mmio,0x140));
    printk("****VHDVFPOS(0x168)    : 0x%0x \n\n", VPP_REG(s_xl_mmio,0x168));

    printk("****VHDDLPOS(0x164)    : 0x%0x \n", VPP_REG(s_xl_mmio,0x164));
    printk("****VHDVLPOS(0x16c)    : 0x%0x \n", VPP_REG(s_xl_mmio,0x16c));
    printk("****VHDCSCIDC(0x180)   : 0x%0x \n", VPP_REG(s_xl_mmio,0x180));
    printk("****VHDHSP(0x1C0)      : 0x%0x \n", VPP_REG(s_xl_mmio,0x1c0));
    printk("****VHDHLOFFSET(0x1c4) : 0x%0x \n", VPP_REG(s_xl_mmio,0x1c4));
    printk("****VHDHCOFFSET(0x1c8) : 0x%0x \n", VPP_REG(s_xl_mmio,0x1c8));
    printk("****VHDVSP(0x1d8)      : 0x%0x \n", VPP_REG(s_xl_mmio,0x1d8));
    printk("****VHDVSR(0x1dc)      : 0x%0x \n\n", VPP_REG(s_xl_mmio,0x1dc));

    printk("****VHDVOFFSE(0x1e0)   : 0x%0x \n", VPP_REG(s_xl_mmio,0x1e0));
    printk("****VHDZMEORESO(0x1e4) : 0x%0x \n", VPP_REG(s_xl_mmio,0x1e4));
    printk("****VHDZMEIRESO(0x1e8) : 0x%0x \n\n", VPP_REG(s_xl_mmio,0x1e8));

    printk("****VHDHSP_K3V3(0x1F0) : 0x%0x \n\n", VPP_REG(s_xl_mmio,0x1f0));

    printk("****VHDDIECTRL(0x300) : 0x%0x \n\n", VPP_REG(s_xl_mmio,0x300));
    printk("****VHDDIESTKADDR(0x3A4)      : 0x%0x \n", VPP_REG(s_xl_mmio,0x3a4));
    printk("****VHDDIESTLADDR(0x3A8)      : 0x%0x \n\n", VPP_REG(s_xl_mmio,0x3a8));
    printk("****VHDDIESTMADDR(0x3AC)      : 0x%0x \n", VPP_REG(s_xl_mmio,0x3ac));
    printk("****VHDDIESTNADDR(0x3B0)      : 0x%0x \n\n", VPP_REG(s_xl_mmio,0x3b0));
    printk("****VHDDIESTSQTRADDR(0x3B4)   : 0x%0x \n\n", VPP_REG(s_xl_mmio,0x3b4));

    for(i=0;i<11;i++){
        printk("****MMU_CH_PTBA(0x1000+%d*0x2C)   : 0x%0x \n", i,VPP_REG(s_xl_mmio,(0x1000+i*0x2c)));
        printk("****MMU_CH_PTVA(0x1004+%d*0x2C)   : 0x%0x \n", i,VPP_REG(s_xl_mmio,(0x1004+i*0x2c)));
        printk("****MMU_CH_INTE(0x1008+%d*0x2C)   : 0x%0x \n", i,VPP_REG(s_xl_mmio,(0x1008+i*0x2c)));
        printk("****MMU_CH_DBG(0x100c+%d*0x2C)   : 0x%0x \n", i,VPP_REG(s_xl_mmio,(0x100c+i*0x2c)));
        printk("****MMU_CH_INTC(0x1010+%d*0x2C)   : 0x%0x \n", i,VPP_REG(s_xl_mmio,(0x1010+i*0x2c)));
        printk("****MMU_CH_INTS(0x1014+%d*0x2C)   : 0x%0x \n", i,VPP_REG(s_xl_mmio,(0x1014+i*0x2c)));
        printk("****MMU_CH_STATUS(0x1018+%d*0x2C)   : 0x%0x \n", i,VPP_REG(s_xl_mmio,(0x1018+i*0x2c)));
        printk("****MMU_CH_VA(0x101c+%d*0x2C)   : 0x%0x \n", i,VPP_REG(s_xl_mmio,(0x101c+i*0x2c)));
        printk("****MMU_CH_PA(0x1020+%d*0x2C)   : 0x%0x \n", i,VPP_REG(s_xl_mmio,(0x1020+i*0x2c)));
        printk("****MMU_CH_START_ADDR(0x1024+%d*0x2C)   : 0x%0x \n", i,VPP_REG(s_xl_mmio,(0x1024+i*0x2c)));
        printk("****MMU_CH_END_ADDR(0x1028+%d*0x2C)   : 0x%0x \n\n", i,VPP_REG(s_xl_mmio,(0x1028+i*0x2c)));

    }
    printk("****MMU_TOP_CTL(0x1200)   : 0x%0x \n\n", VPP_REG(s_xl_mmio,0x1200));
    printk("****MMU_TOP_INTS(0x1204)   : 0x%0x \n\n", VPP_REG(s_xl_mmio,0x1204));

    printk("******************* reg list end *******************\n\n");
#else
#endif
    return;
}

unsigned int reg_read(unsigned int *addr)
{
    unsigned int value =0;

    BUG_ON(NULL == s_xl_mmio);

    value = VPP_REG(s_xl_mmio, addr);
    logd("****read  %p ret: 0x%#x\n", addr, value);

    return value;
}

void reg_write(unsigned int* addr, unsigned int value)
{
    BUG_ON(NULL == s_xl_mmio);

    VPP_REG(s_xl_mmio, addr) = value;
    logd("****write %p reg: 0x%#x\n", addr, value);

    return;
}

//初始化寄存器
int hal_init(struct device_node *node)
{
    s_xl_mmio = of_iomap(node, 0);
    if( NULL == s_xl_mmio)
    {
        loge("ioremap is failed");
        return -1;
    }

    return 0;
}

void hal_deinit(void)
{
    BUG_ON(NULL == s_xl_mmio);

    iounmap(s_xl_mmio);
    s_xl_mmio = NULL;

    return;
}

//查询视频层输入分辨率
void hal_die_layer_inreso(HAL_LAYER_E enLayer, unsigned int *pu32W, unsigned int *pu32H)
{
    if((NULL == pu32H) || (NULL == pu32W)){
        loge("input parameter is null!\n");
        return;
    }
    switch (enLayer)
    {
        case HAL_LAYER_VIDEO1:
        {
            U_VHDIRESO VHDIRESO;
            VHDIRESO.u32 = 0;

            VHDIRESO.u32 = reg_read(&(pVoReg->VHDIRESO.u32));
            *pu32H = VHDIRESO.bits.ih + 1;
            *pu32W = VHDIRESO.bits.iw + 1;

            break;
        }
        default:
        {
            loge("layer id error!\n");
            break;
        }
    }

    return;
}

/*
**************************************************************************
* FunctionName: hal_set_clk_gate_en
* Description : 使能VPP始终门控
* Input       : u32Data: 0:时钟门控关闭 1:时钟门控打开
* Output      : NA;
* ReturnValue : NA;
* Other       : NA;
**************************************************************************
*/
void hal_set_clk_gate_en(unsigned int u32Data)
{
    U_VOCTRL VOCTRL;
    VOCTRL.u32 = 0x0;

    VOCTRL.u32 = reg_read(&(pVoReg->VOCTRL.u32));
    VOCTRL.bits.vo_ck_gt_en = u32Data;
    reg_write(&(pVoReg->VOCTRL.u32), VOCTRL.u32);

    return;
}


/*
**************************************************************************
* FunctionName: hal_set_intf_csc_coef
* Description : 配置CSC参数
* Input       : enLayer:
*             : stCscCoef
* Output      : NA;
* ReturnValue : NA;
* Other       : NA;
**************************************************************************
*/
void  hal_set_intf_csc_coef(HAL_LAYER_E enLayer, HAL_CSCCOEF_S stCscCoef)
{
    U_VHDCSCIDC VHDCSCIDC;
    U_VHDCSCODC VHDCSCODC;
    U_VHDCSCP0 VHDCSCP0;
    U_VHDCSCP1 VHDCSCP1;
    U_VHDCSCP2 VHDCSCP2;
    U_VHDCSCP3 VHDCSCP3;
    U_VHDCSCP4 VHDCSCP4;

    VHDCSCIDC.u32 = 0x0;
    VHDCSCODC.u32 = 0x0;
    VHDCSCP0.u32 = 0x0;
    VHDCSCP1.u32 = 0x0;
    VHDCSCP2.u32 = 0x0;
    VHDCSCP3.u32 = 0x0;
    VHDCSCP4.u32 = 0x0;

    BUG_ON( enLayer != HAL_LAYER_VIDEO1 );

    switch(enLayer)
    {
        case HAL_LAYER_VIDEO1:
        {
            VHDCSCIDC.u32 = reg_read(&(pVoReg->VHDCSCIDC.u32));
            VHDCSCIDC.bits.cscidc2 = stCscCoef.csc_in_dc2;
            VHDCSCIDC.bits.cscidc1 = stCscCoef.csc_in_dc1;
            VHDCSCIDC.bits.cscidc0 = stCscCoef.csc_in_dc0;
            reg_write(&(pVoReg->VHDCSCIDC.u32), VHDCSCIDC.u32);

            VHDCSCODC.u32 = reg_read(&(pVoReg->VHDCSCODC.u32));
            VHDCSCODC.bits.cscodc2 = stCscCoef.csc_out_dc2;
            VHDCSCODC.bits.cscodc1 = stCscCoef.csc_out_dc1;
            VHDCSCODC.bits.cscodc0 = stCscCoef.csc_out_dc0;
            reg_write(&(pVoReg->VHDCSCODC.u32), VHDCSCODC.u32);

            VHDCSCP0.u32 = reg_read(&(pVoReg->VHDCSCP0.u32));
            VHDCSCP0.bits.cscp00 = stCscCoef.csc_coef00;
            VHDCSCP0.bits.cscp01 = stCscCoef.csc_coef01;
            reg_write(&(pVoReg->VHDCSCP0.u32), VHDCSCP0.u32);

            VHDCSCP1.u32 = reg_read(&(pVoReg->VHDCSCP1.u32));
            VHDCSCP1.bits.cscp02 = stCscCoef.csc_coef02;
            VHDCSCP1.bits.cscp10 = stCscCoef.csc_coef10;
            reg_write(&(pVoReg->VHDCSCP1.u32), VHDCSCP1.u32);

            VHDCSCP2.u32 = reg_read(&(pVoReg->VHDCSCP2.u32));
            VHDCSCP2.bits.cscp11 = stCscCoef.csc_coef11;
            VHDCSCP2.bits.cscp12 = stCscCoef.csc_coef12;
            reg_write(&(pVoReg->VHDCSCP2.u32), VHDCSCP2.u32);

            VHDCSCP3.u32 = reg_read(&(pVoReg->VHDCSCP3.u32));
            VHDCSCP3.bits.cscp20 = stCscCoef.csc_coef20;
            VHDCSCP3.bits.cscp21 = stCscCoef.csc_coef21;
            reg_write(&(pVoReg->VHDCSCP3.u32), VHDCSCP3.u32);

            VHDCSCP4.u32 = reg_read(&(pVoReg->VHDCSCP4.u32));
            VHDCSCP4.bits.cscp22 = stCscCoef.csc_coef22;
            reg_write(&(pVoReg->VHDCSCP4.u32), VHDCSCP4.u32);

            break;
        }
        default:
        {
            loge("layer id error!\n");
            break;
        }
    }

    return;
}

void hal_set_pd_defthd(HAL_LAYER_E enLayer)
{
    unsigned int u32HistThd[4]   = {8,16,32,64};
    unsigned int u32UmThd[3]     = {8,32,64};
    unsigned int u32PccThd[4]    = {16,32,64,128};
    unsigned int u32ItDiffThd[4] = {4,8,16,32};

    BUG_ON( enLayer != HAL_LAYER_VIDEO1 );

    switch(enLayer)
    {
        case HAL_LAYER_VIDEO1:
        {
            hal_set_pd_dir_mch    (enLayer, 0);
            hal_set_pd_stl_blk_thd (enLayer, 0);
            hal_set_pd_diff_thd   (enLayer, 30);
            hal_set_pd_coring_tkr (enLayer, 128);
            hal_set_pd_coring_norm(enLayer, 16);
            hal_set_pd_pcc_hthd   (enLayer, 16);

            hal_set_pd_lasi_thd   (enLayer, 20);
            hal_set_pd_edge_thd   (enLayer, 15);

            hal_set_pd_hist_thd   (enLayer, u32HistThd);
            hal_set_pd_um_thd     (enLayer, u32UmThd);
            hal_set_pd_pcc_vthd   (enLayer, u32PccThd);
            hal_set_pd_it_diff_thd (enLayer, u32ItDiffThd);

            break;
        }
        default:
        {
            loge("layer id error!\n");
            break ;
        }
    }

    return;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
          There are common functions for both EDA and FPGA, SDK
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//==============================================================================
void  hal_enable_layer(HAL_LAYER_E enLayer, unsigned int bEnable)
{
    U_VHDCTRL VHDCTRL;
    VHDCTRL.u32 = 0;

    BUG_ON( enLayer != HAL_LAYER_VIDEO1 );

    logd("enLayer = %d, bEnable = %d\n", enLayer, bEnable);

    switch(enLayer)
    {
        case HAL_LAYER_VIDEO1:
        {
            VHDCTRL.u32 = reg_read(&(pVoReg->VHDCTRL.u32));
            VHDCTRL.bits.surface_en = bEnable;
            reg_write(&(pVoReg->VHDCTRL.u32), VHDCTRL.u32);
            break;
        }
        default:
        {
            loge("layer id error!\n");
            break ;
        }
    }

    return;
}

void hal_set_layer_addr(HAL_LAYER_E enLayer,unsigned char  u8Chan,
                                         unsigned int u32LAddr,unsigned int u32CAddr,
                                         unsigned short u16LStr, unsigned short u16CStr)
{
    U_VHDCADDR VHDCADDR;
    U_VHDCCADDR VHDCCADDR;
    U_VHDLADDR VHDLADDR;
    U_VHDLCADDR VHDLCADDR;
    U_VHDNADDR VHDNADDR;
    U_VHDNCADDR VHDNCADDR;
    U_VHDSTRIDE VHDSTRIDE;
    VHDCADDR.u32 = 0x0;
    VHDCCADDR.u32 = 0x0;
    VHDLADDR.u32 = 0x0;
    VHDLCADDR.u32 = 0x0;
    VHDLCADDR.u32 = 0x0;
    VHDNCADDR.u32 = 0x0;
    VHDSTRIDE.u32 = 0x0;

    BUG_ON( enLayer != HAL_LAYER_VIDEO1 );
    switch(enLayer)
    {
        case HAL_LAYER_VIDEO1:
        {
            if(u8Chan == 0)
            {
                VHDCADDR.u32 = u32LAddr;
                VHDCCADDR.u32 = u32CAddr;
                reg_write(&(pVoReg->VHDCADDR.u32), VHDCADDR.u32);
                reg_write(&(pVoReg->VHDCCADDR.u32), VHDCCADDR.u32);
            }
            else if(u8Chan == 1)
            {
                VHDLADDR.u32 = u32LAddr;
                VHDLCADDR.u32 = u32CAddr;
                reg_write(&(pVoReg->VHDLADDR.u32), VHDLADDR.u32);
                reg_write(&(pVoReg->VHDLCADDR.u32), VHDLCADDR.u32);
            }
            else
            {
                VHDNADDR.u32 = u32LAddr;
                VHDNCADDR.u32 = u32CAddr;
                reg_write(&(pVoReg->VHDNADDR.u32), VHDNADDR.u32);
                reg_write(&(pVoReg->VHDNCADDR.u32), VHDNCADDR.u32);
            }

            VHDSTRIDE.u32 = reg_read(&(pVoReg->VHDSTRIDE.u32));
            VHDSTRIDE.bits.surface_stride = u16LStr;
            VHDSTRIDE.bits.surface_cstride = u16CStr;
            reg_write(&(pVoReg->VHDSTRIDE.u32), VHDSTRIDE.u32);

            break;
        }
        default:
        {
            loge("layer id error!\n");
            break ;
        }
    }

    return;
}

void  hal_set_layer_data_fmt(HAL_LAYER_E enLayer,
                                            VPP_PIXELFORMAT_E  enDataFmt,unsigned int w,unsigned int h)
{
    U_VHDCTRL VHDCTRL;
    VHDCTRL.u32 = 0x0;

    BUG_ON( enLayer != HAL_LAYER_VIDEO1 );

    switch(enLayer)
    {
        case HAL_LAYER_VIDEO1:
        {
            VHDCTRL.u32 = reg_read(&(pVoReg->VHDCTRL.u32));
            VHDCTRL.bits.ifmt = enDataFmt;
            if(enDataFmt == VPP_INPUTFMT_YCBCR_SEMIPLANAR_400 || enDataFmt == VPP_INPUTFMT_YCBCR_SEMIPLANAR_420
                || enDataFmt ==VPP_INPUTFMT_YCBCR_SEMIPLANAR_422 || enDataFmt ==  VPP_INPUTFMT_YCBCR_SEMIPLANAR_444)
            {
                VHDCTRL.bits.swp_chr_flag = 1;
            }

            else
            {

                VHDCTRL.bits.swp_chr_flag = 0;
            }

            reg_write(&(pVoReg->VHDCTRL.u32), VHDCTRL.u32);

            break;
        }
        default:
        {
            loge("layer id error!\n");
            return;
        }
    }

    if((enDataFmt == VPP_INPUTFMT_YCBCR_SEMIPLANAR_420)
    ||(enDataFmt == VPP_INPUTFMT_YCBCR_SEMIPLANAR_400)
    )
    {
        hal_set_zme_ver_type(enLayer, 1,w,h);
        hal_set_zme_copy(0);
    }
    else
    {
        logd("Ver Zme Enable,422 data format\n");
        hal_set_zme_ver_type(enLayer, 0,w,h);
        hal_set_zme_copy(0);
    }

    return;
}

void  hal_set_regupNoRatio(HAL_LAYER_E enLayer)
{
    U_VHDUPD VHDUPD;
    VHDUPD.u32 = 0x0;

    BUG_ON( enLayer != HAL_LAYER_VIDEO1 );

    switch(enLayer)
    {
        case HAL_LAYER_VIDEO1:
        {
            /* VHD layer register update */
            VHDUPD.bits.regup = 0x1;
            reg_write(&(pVoReg->VHDUPD.u32), VHDUPD.u32);
            break;
        }
        default:
        {
            loge("layer id error!\n");
            break ;
        }
    }

    return;
}


void  hal_set_read_mode(HAL_LAYER_E enLayer,
                                        HAL_DATARMODE_E enLRMode,
                                        HAL_DATARMODE_E enCRMode)
{
    U_VHDCTRL VHDCTRL;
    VHDCTRL.u32 = 0x0;

    BUG_ON( enLayer != HAL_LAYER_VIDEO1 );

    /* VHD read data mode */
    if(enLayer == HAL_LAYER_VIDEO1)
    {
        VHDCTRL.u32 = reg_read(&(pVoReg->VHDCTRL.u32));
        VHDCTRL.bits.lm_rmode = enLRMode;
        VHDCTRL.bits.chm_rmode = enCRMode;
        reg_write(&(pVoReg->VHDCTRL.u32), VHDCTRL.u32);
    }
    else
    {
        loge("set data read mode select wrong layer ID\n");
    }

    return;
}

void hal_set_layer_bkg(HAL_LAYER_E enLayer,HAL_BKCOLOR_S stBkg)
{
    U_VHDBK VHDBK;
    VHDBK.u32 = 0x0;

    BUG_ON( enLayer != HAL_LAYER_VIDEO1 );

    if(enLayer == HAL_LAYER_VIDEO1)
    {
        VHDBK.u32 = reg_read(&(pVoReg->VHDBK.u32));
        VHDBK.bits.vbk_alpha = stBkg.u8Bkg_a;
        VHDBK.bits.vbk_y = stBkg.u8Bkg_y;
        VHDBK.bits.vbk_cb = stBkg.u8Bkg_cb;
        VHDBK.bits.vbk_cr = stBkg.u8Bkg_cr;
        reg_write(&(pVoReg->VHDBK.u32), VHDBK.u32);
    }
    else
    {
        loge("Set background color select wrong channel ID\n");
    }

    return;
}

void hal_set_field_order(HAL_LAYER_E enLayer, VPP_VHD_FOD_E uFieldOrder)
{
    U_VHDCTRL VHDCTRL;
    VHDCTRL.u32 = 0x0;

    BUG_ON( enLayer != HAL_LAYER_VIDEO1 );

    if(enLayer == HAL_LAYER_VIDEO1)
    {
        VHDCTRL.u32 = reg_read(&(pVoReg->VHDCTRL.u32));
        VHDCTRL.bits.bfield_first = uFieldOrder;
        reg_write(&(pVoReg->VHDCTRL.u32), VHDCTRL.u32);
    }
    else
    {
        loge("hal_set_field_order select wrong channel ID\n");
    }

    return;
}

/* HAL set coef or lut read address */
void  hal_set_coef_addr(HAL_LAYER_E enLayer,
                                   HAL_COEFMODE_E enMode,
                                   unsigned int u32Addr)
{
    U_VHDHCOEFAD VHDHCOEFAD;
    U_VHDVCOEFAD VHDVCOEFAD;
    VHDHCOEFAD.u32 = 0x0;
    VHDVCOEFAD.u32 = 0x0;

    BUG_ON( enLayer != HAL_LAYER_VIDEO1 );

    switch(enLayer)
    {
        case HAL_LAYER_VIDEO1:
        {
            /* VHD layer coef addr */
            if(enMode == HAL_COEFMODE_HOR)
            {
                VHDHCOEFAD.bits.coef_addr = u32Addr;
                reg_write(&(pVoReg->VHDHCOEFAD.u32), VHDHCOEFAD.u32);
            }
            else if(enMode == HAL_COEFMODE_VER)
            {
                VHDVCOEFAD.bits.coef_addr = u32Addr;
                reg_write(&(pVoReg->VHDVCOEFAD.u32), VHDVCOEFAD.u32);
            }
            else
            {
                loge("VHD Wrong coef mode\n");
            }
            break;
        }
        default:
        {
            loge("layer id error!\n");
            break ;
        }
    }

    return;
}

void hal_set_wbc_wr_mode(HAL_LAYER_E enLayer, unsigned int u32Data)
{
    U_VHDWBC1STRD VHDWBC1STRD;

    BUG_ON( HAL_LAYER_WBC1 != enLayer );

    if (enLayer == HAL_LAYER_WBC1)
    {
        VHDWBC1STRD.u32 = reg_read(&(pVoReg->VHDWBC1STRD.u32));
        VHDWBC1STRD.bits.wbc1_wr_mode = u32Data;
        reg_write(&(pVoReg->VHDWBC1STRD.u32), VHDWBC1STRD.u32);
    }
    else
    {
        loge("layer id error!\n");
    }

    return;
}


/* HAL set coef or lut read update */
void  hal_set_layer_para_upd(HAL_LAYER_E enLayer,
                                  HAL_COEFMODE_E enMode)
{
    U_VOPARAUP VOPARAUP;
    VOPARAUP.u32 = 0x0;

    BUG_ON( enLayer != HAL_LAYER_VIDEO1 );

    VOPARAUP.u32 = reg_read(&(pVoReg->VOPARAUP.u32));

    switch(enLayer)
    {
        case HAL_LAYER_VIDEO1:
        {
            /* VHD layer coef addr */
            if(enMode == HAL_COEFMODE_HOR || enMode == HAL_COEFMODE_ALL)
            {
                VOPARAUP.bits.vhd_hcoef_upd = 0x1;
            }

            if(enMode == HAL_COEFMODE_VER || enMode == HAL_COEFMODE_ALL)
            {
                VOPARAUP.bits.vhd_vcoef_upd = 0x1;
            }

            break;
        }
        default:
        {
            loge("layer id error!\n");
            return;
        }
    }

    reg_write(&(pVoReg->VOPARAUP.u32), VOPARAUP.u32);

    return;
}

/* HAL zoom enable */
void  hal_set_zme_enable(HAL_LAYER_E enLayer,
                                    HAL_ZMEMODE_E enMode,
                                    unsigned int bEnable)
{
    U_VHDHSP VHDHSP;
    U_VHDVSP VHDVSP;
    VHDHSP.u32 = 0x0;
    VHDVSP.u32 = 0x0;

    BUG_ON( enLayer != HAL_LAYER_VIDEO1 );

    switch(enLayer)
    {
        case HAL_LAYER_VIDEO1:
        {
            /* VHD layer zoom enable */
            if((enMode == HAL_ZMEMODE_HORL)||(enMode == HAL_ZMEMODE_HOR)||(enMode == HAL_ZMEMODE_ALL))
            {
                VHDHSP.u32 = reg_read(&(pVoReg->VHDHSP.u32));
                VHDHSP.bits.hlmsc_en = bEnable;
                VHDHSP.bits.hlfir_en = 1;
                reg_write(&(pVoReg->VHDHSP.u32), VHDHSP.u32);
            }

            if((enMode == HAL_ZMEMODE_HORC)||(enMode == HAL_ZMEMODE_HOR)||(enMode == HAL_ZMEMODE_ALL))
            {
                VHDHSP.u32 = reg_read(&(pVoReg->VHDHSP.u32));
                VHDHSP.bits.hchmsc_en = bEnable;
                VHDHSP.bits.hchfir_en = 1;
                reg_write(&(pVoReg->VHDHSP.u32), VHDHSP.u32);
            }
            if((enMode == HAL_ZMEMODE_VERL)||(enMode == HAL_ZMEMODE_VER)||(enMode == HAL_ZMEMODE_ALL))
            {
                VHDVSP.u32 = reg_read(&(pVoReg->VHDVSP.u32));
                VHDVSP.bits.vlmsc_en = bEnable;
                VHDVSP.bits.vlfir_en = 1;
                reg_write(&(pVoReg->VHDVSP.u32), VHDVSP.u32);
            }

            if((enMode == HAL_ZMEMODE_VERC)||(enMode == HAL_ZMEMODE_VER)||(enMode == HAL_ZMEMODE_ALL))
            {
                VHDVSP.u32 = reg_read(&(pVoReg->VHDVSP.u32));
                VHDVSP.bits.vchmsc_en = bEnable;
                VHDVSP.bits.vchfir_en = 1;
                reg_write(&(pVoReg->VHDVSP.u32), VHDVSP.u32);
            }

            break;
        }
        default:
        {
            loge("layer id error!\n");
            break ;
        }
    }

    return;
}

void hal_set_zme_ratio(void)
{
    U_VHDHSP VHDHSP;
    U_VHDVSR VHDVSR;
    U_VHDVSP VHDVSP;
    U_VHDZMEIRESO VHDZMEIRESO;
    U_VHDZMEORESO VHDZMEORESO;

    unsigned int u32InWidth   = 0;
    unsigned int u32InHeight  = 0;
    unsigned int u32OutWidth  = 0;
    unsigned int u32OutHeight = 0;

    VHDHSP.u32 = reg_read(&(pVoReg->VHDHSP.u32));
    VHDVSP.u32 = reg_read(&(pVoReg->VHDVSP.u32));

    if( VHDHSP.bits.hlmsc_en | VHDHSP.bits.hchmsc_en | VHDVSP.bits.vlmsc_en
        | VHDVSP.bits.vchmsc_en)
    {
        VHDZMEIRESO.u32 = reg_read(&(pVoReg->VHDZMEIRESO.u32));
        VHDZMEORESO.u32 = reg_read(&(pVoReg->VHDZMEORESO.u32));

        u32InWidth    = VHDZMEIRESO.bits.iw + 1;
        u32InHeight   = VHDZMEIRESO.bits.ih + 1;
        u32OutWidth  = VHDZMEORESO.bits.ow + 1;
        u32OutHeight = VHDZMEORESO.bits.oh + 1;

        /* set VHD zoom ratio */
        VHDHSP.u32 = reg_read(&(pVoReg->VHDHSP.u32));
        VHDHSP.bits.hratio = (unsigned int)(u32InWidth*4096/u32OutWidth);
        reg_write(&(pVoReg->VHDHSP.u32), VHDHSP.u32);

        VHDVSR.u32 = reg_read(&(pVoReg->VHDVSR.u32));
        VHDVSR.bits.vratio = (unsigned int)(u32InHeight*4096/u32OutHeight);
        reg_write(&(pVoReg->VHDVSR.u32), VHDVSR.u32);
    }

    return;
}


void hal_set_regup(HAL_LAYER_E enLayer)
{
    U_VHDUPD VHDUPD;
    VHDUPD.u32 = 0x0;

    BUG_ON( enLayer != HAL_LAYER_VIDEO1 );

    switch(enLayer)
    {
        case HAL_LAYER_VIDEO1:
        {
            /* surface的寄存器更新。当软件配置完该层寄存器后写
                1更新，更新完成后，硬件自动清零*/
            VHDUPD.bits.regup = 0x1;
            //VHDUPD.u32 = 0x304a003;
            reg_write(&(pVoReg->VHDUPD.u32), VHDUPD.u32);
            break;
        }
        default:
        {
            loge("layer id error!\n");
            break ;
        }
    }

    return;
}


void hal_set_zme_fir_enable(HAL_LAYER_E enLayer, HAL_ZMEMODE_E enMode, unsigned int bEnable)
{
    U_VHDHSP VHDHSP;
    U_VHDVSP VHDVSP;
    VHDHSP.u32 = 0x0;
    VHDVSP.u32 = 0x0;

    BUG_ON( enLayer != HAL_LAYER_VIDEO1 );

    switch(enLayer)
    {
        case HAL_LAYER_VIDEO1:
        {
            /* VHD layer zoom enable */
            if((enMode == HAL_ZMEMODE_HORL)||(enMode == HAL_ZMEMODE_HOR)||(enMode == HAL_ZMEMODE_ALL))
            {
                VHDHSP.u32 = reg_read(&(pVoReg->VHDHSP.u32));
                VHDHSP.bits.hlfir_en = bEnable;
                reg_write(&(pVoReg->VHDHSP.u32), VHDHSP.u32);
            }

            if((enMode == HAL_ZMEMODE_HORC)||(enMode == HAL_ZMEMODE_HOR)||(enMode == HAL_ZMEMODE_ALL))
            {
                VHDHSP.u32 = reg_read(&(pVoReg->VHDHSP.u32));
                VHDHSP.bits.hchfir_en = bEnable;
                reg_write(&(pVoReg->VHDHSP.u32), VHDHSP.u32);
            }

            if((enMode == HAL_ZMEMODE_VERL)||(enMode == HAL_ZMEMODE_VER)||(enMode == HAL_ZMEMODE_ALL))
            {
                VHDVSP.u32 = reg_read(&(pVoReg->VHDVSP.u32));
                VHDVSP.bits.vlfir_en = bEnable;
                reg_write(&(pVoReg->VHDVSP.u32), VHDVSP.u32);
            }

            if((enMode == HAL_ZMEMODE_VERC)||(enMode == HAL_ZMEMODE_VER)||(enMode == HAL_ZMEMODE_ALL))
            {
                VHDVSP.u32 = reg_read(&(pVoReg->VHDVSP.u32));
                VHDVSP.bits.vchfir_en = bEnable;
                reg_write(&(pVoReg->VHDVSP.u32), VHDVSP.u32);
            }

            break;
        }
        default:
        {
            loge("layer id error!\n");
            break ;
        }
    }

    return;
}

/* HAL set Median filter enable */
void  hal_set_mid_enable(HAL_LAYER_E enLayer,
                                    HAL_ZMEMODE_E enMode,
                                    unsigned int bEnable)
{
    U_VHDHSP VHDHSP;
    U_VHDVSP VHDVSP;
    VHDHSP.u32 = 0x0;
    VHDVSP.u32 = 0x0;

    BUG_ON( enLayer != HAL_LAYER_VIDEO1 );

    switch(enLayer)
    {
        case HAL_LAYER_VIDEO1:
        {
            /* VHD layer zoom enable */
            if((enMode == HAL_ZMEMODE_HORL)||(enMode == HAL_ZMEMODE_HOR)||(enMode == HAL_ZMEMODE_ALL))
            {
                VHDHSP.u32 = reg_read(&(pVoReg->VHDHSP.u32));
                VHDHSP.bits.hlmid_en = bEnable;
                reg_write(&(pVoReg->VHDHSP.u32), VHDHSP.u32);
            }

            if((enMode == HAL_ZMEMODE_HORC)||(enMode == HAL_ZMEMODE_HOR)||(enMode == HAL_ZMEMODE_ALL))
            {
                VHDHSP.u32 = reg_read(&(pVoReg->VHDHSP.u32));
                VHDHSP.bits.hchmid_en = bEnable;
                reg_write(&(pVoReg->VHDHSP.u32), VHDHSP.u32);
            }

            if((enMode == HAL_ZMEMODE_VERL)||(enMode == HAL_ZMEMODE_VER)||(enMode == HAL_ZMEMODE_ALL))
            {
                VHDVSP.u32 = reg_read(&(pVoReg->VHDVSP.u32));
                VHDVSP.bits.vlmid_en = bEnable;
                reg_write(&(pVoReg->VHDVSP.u32), VHDVSP.u32);
            }

            if((enMode == HAL_ZMEMODE_VERC)||(enMode == HAL_ZMEMODE_VER)||(enMode == HAL_ZMEMODE_ALL))
            {
                VHDVSP.u32 = reg_read(&(pVoReg->VHDVSP.u32));
                VHDVSP.bits.vchmid_en = bEnable;
                reg_write(&(pVoReg->VHDVSP.u32), VHDVSP.u32);
            }

            break;
        }
        default:
        {
            loge("layer id error!\n");
            break ;
        }
    }

    return;
}

void hal_set_hfir_order(HAL_LAYER_E enLayer, unsigned int uHfirOrder)
{
    U_VHDHSP VHDHSP;
    VHDHSP.u32 = 0x0;

    BUG_ON( enLayer != HAL_LAYER_VIDEO1 );

    switch(enLayer)
    {
        case HAL_LAYER_VIDEO1:
        {
            VHDHSP.u32 = reg_read(&(pVoReg->VHDHSP.u32));
            VHDHSP.bits.hfir_order = uHfirOrder;
            reg_write(&(pVoReg->VHDHSP.u32), VHDHSP.u32);
            break;
        }
        default:
        {
            loge("layer id error!\n");
            break ;
        }
    }

    return;
}


void hal_set_zme_vertap(HAL_LAYER_E enLayer, HAL_ZMEMODE_E enMode, unsigned int uVerTap)
{
    U_VHDVSP VHDVSP;
    VHDVSP.u32 = 0x0;

    BUG_ON( enLayer != HAL_LAYER_VIDEO1 );

    if (  (enMode == HAL_ZMEMODE_HORL) || (enMode == HAL_ZMEMODE_HOR) \
       || (enMode == HAL_ZMEMODE_HORC))
    {
        loge("enMode is error!\n");
        return;
    }

    switch(enLayer)
    {
        case HAL_LAYER_VIDEO1:
        {
            if ((enMode == HAL_ZMEMODE_VERL) || (enMode == HAL_ZMEMODE_VER) || (enMode == HAL_ZMEMODE_ALL))
            {
                VHDVSP.u32 = reg_read(&(pVoReg->VHDVSP.u32));
                VHDVSP.bits.vsc_luma_tap = uVerTap;
                reg_write(&(pVoReg->VHDVSP.u32), VHDVSP.u32);
            }

            if ((enMode == HAL_ZMEMODE_VERC) || (enMode == HAL_ZMEMODE_VER) || (enMode == HAL_ZMEMODE_ALL))
            {
                VHDVSP.u32 = reg_read(&(pVoReg->VHDVSP.u32));
                VHDVSP.bits.vsc_chroma_tap = uVerTap;
                reg_write(&(pVoReg->VHDVSP.u32), VHDVSP.u32);
            }

            break;
        }
        default:
        {
            loge("layer id error!\n");
            break ;
        }
    }

    return;
}


void hal_set_zme_ver_type(HAL_LAYER_E enLayer, unsigned int uVerType,unsigned int w,unsigned int h)
{
    U_VHDHSP_K3V3        VHDHSP_K3V3;
    VHDHSP_K3V3.u32 = 0x0;

    BUG_ON( enLayer != HAL_LAYER_VIDEO1 );

    switch(enLayer)
    {
        case HAL_LAYER_VIDEO1:
        {
            VHDHSP_K3V3.u32 = reg_read(&(pVoReg->VHDHSP_K3V3.u32));
            VHDHSP_K3V3.bits.vsc_420_k3v3 = uVerType;
            VHDHSP_K3V3.bits.cvfir_in_height_k3v3 = h - 1;
            VHDHSP_K3V3.bits.chfir_in_width_k3v3 = w - 1;
            reg_write(&(pVoReg->VHDHSP_K3V3.u32), VHDHSP_K3V3.u32);
            break;
        }

        default:
        {
            loge("layer id error!\n");
            break ;
        }
    }

    return;
}

void hal_set_zme_copy(unsigned int vsc_copy)
{

    U_VHDHSP_K3V3        VHDHSP_K3V3;
    VHDHSP_K3V3.u32 = 0x0;

    VHDHSP_K3V3.u32 = reg_read(&(pVoReg->VHDHSP_K3V3.u32));
    VHDHSP_K3V3.bits.vsc_copy_k3v3 = vsc_copy;
    reg_write(&(pVoReg->VHDHSP_K3V3.u32), VHDHSP_K3V3.u32);

    return;
}

void hal_set_zme_core(HAL_LAYER_E enLayer, unsigned int vsc_core)
{
    U_VHDHSP_K3V3        VHDHSP_K3V3;
    VHDHSP_K3V3.u32 = 0x0;

    BUG_ON( enLayer != HAL_LAYER_VIDEO1 );

    switch(enLayer)
    {
        case HAL_LAYER_VIDEO1:
        {
            VHDHSP_K3V3.u32 = reg_read(&(pVoReg->VHDHSP_K3V3.u32));
            VHDHSP_K3V3.bits.vsc_core_gt = vsc_core;
            reg_write(&(pVoReg->VHDHSP_K3V3.u32), VHDHSP_K3V3.u32);

            break;
        }

        default:
        {
            loge("layer id error!\n");
            break ;
        }
    }

    return;
}

/* HAL set zoom inital phase */
void  hal_set_zme_phase(HAL_LAYER_E enLayer,
                                   HAL_ZMEMODE_E enMode,
                                   int s32Phase)
{
    U_VHDHLOFFSET VHDHLOFFSET;
    U_VHDHCOFFSET VHDHCOFFSET;
    U_VHDVOFFSET VHDVOFFSET;
    U_VSDHLOFFSET VSDHLOFFSET;
    U_VSDHCOFFSET VSDHCOFFSET;
    U_VSDVOFFSET VSDVOFFSET;
    VHDHLOFFSET.u32 = 0x0;
    VHDHCOFFSET.u32 = 0x0;
    VHDVOFFSET.u32 = 0x0;
    VSDHLOFFSET.u32 = 0x0;
    VSDHCOFFSET.u32 = 0x0;
    VSDVOFFSET.u32 = 0x0;

    BUG_ON( enLayer != HAL_LAYER_VIDEO1 );

    switch(enLayer)
    {
        case HAL_LAYER_VIDEO1:
        {
            /* VHD layer zoom enable */
            if((enMode == HAL_ZMEMODE_HORL) || (enMode == HAL_ZMEMODE_HOR)||(enMode == HAL_ZMEMODE_ALL))
            {
                VHDHLOFFSET.u32 = reg_read(&(pVoReg->VHDHLOFFSET.u32));
                VHDHLOFFSET.bits.hor_loffset = s32Phase;
                reg_write(&(pVoReg->VHDHLOFFSET.u32), VHDHLOFFSET.u32);
            }

            if((enMode == HAL_ZMEMODE_HORC) || (enMode == HAL_ZMEMODE_HOR)||(enMode == HAL_ZMEMODE_ALL))
            {
                VHDHCOFFSET.u32 = reg_read(&(pVoReg->VHDHCOFFSET.u32));
                VHDHCOFFSET.bits.hor_coffset = s32Phase;
                reg_write(&(pVoReg->VHDHCOFFSET.u32), VHDHCOFFSET.u32);
            }

            if((enMode == HAL_ZMEMODE_VERL)||(enMode == HAL_ZMEMODE_VER)||(enMode == HAL_ZMEMODE_ALL))
            {
                VHDVOFFSET.u32 = reg_read(&(pVoReg->VHDVOFFSET.u32));
                VHDVOFFSET.bits.vluma_offset = s32Phase;
                reg_write(&(pVoReg->VHDVOFFSET.u32), VHDVOFFSET.u32);
            }

            if((enMode == HAL_ZMEMODE_VERC)||(enMode == HAL_ZMEMODE_VER)||(enMode == HAL_ZMEMODE_ALL))
            {
                VHDVOFFSET.u32 = reg_read(&(pVoReg->VHDVOFFSET.u32));
                VHDVOFFSET.bits.vchroma_offset = s32Phase;
                reg_write(&(pVoReg->VHDVOFFSET.u32), VHDVOFFSET.u32);
            }

            break;
        }
        default:
        {
            loge("layer id error!\n");
            break;
        }
    }

    return;
}

void hal_set_zme_reso(HAL_LAYER_E enLayer, HAL_RECT_S stZmeReso)
{
    U_VHDZMEIRESO VHDZMEIRESO;
    U_VHDZMEORESO VHDZMEORESO;
    U_VSDZMEIRESO VSDZMEIRESO;
    U_VSDZMEORESO VSDZMEORESO;
    VHDZMEIRESO.u32 = 0x0;
    VHDZMEORESO.u32 = 0x0;
    VSDZMEIRESO.u32 = 0x0;
    VSDZMEORESO.u32 = 0x0;

    BUG_ON( enLayer != HAL_LAYER_VIDEO1 );

    switch(enLayer)
    {
        case HAL_LAYER_VIDEO1:
        {
            VHDZMEIRESO.u32 = reg_read(&(pVoReg->VHDZMEIRESO.u32));
            VHDZMEIRESO.bits.iw = stZmeReso.u32InWidth  - 1;
            VHDZMEIRESO.bits.ih = stZmeReso.u32InHeight - 1;
            reg_write(&(pVoReg->VHDZMEIRESO.u32), VHDZMEIRESO.u32);

            VHDZMEORESO.u32 = reg_read(&(pVoReg->VHDZMEORESO.u32));
            VHDZMEORESO.bits.ow = stZmeReso.u32OutWidth  - 1;
            VHDZMEORESO.bits.oh = stZmeReso.u32OutHeight - 1;
            reg_write(&(pVoReg->VHDZMEORESO.u32), VHDZMEORESO.u32);

            break;
        }
        default:
        {
            loge("layer id error!\n");
            break;
        }
    }

    return;
}


//=============================================================================//
// DIE CFG
//
void hal_die_addr(HAL_LAYER_E enLayer, unsigned int uDieAddr)
{
    U_VHDDIEADDR VHDDIEADDR;
    VHDDIEADDR.u32 = 0x0;

    BUG_ON( enLayer != HAL_LAYER_VIDEO1 );

    switch(enLayer)
    {
        case HAL_LAYER_VIDEO1:
        {
            VHDDIEADDR.u32 = uDieAddr;
            reg_write(&(pVoReg->VHDDIEADDR.u32), VHDDIEADDR.u32);
            break;
        }
        default:
        {
            loge("layer id error!\n");
            break;
        }
    }

    return;
}

void hal_set_die_enable(HAL_LAYER_E enLayer, unsigned int bDieLumEn, unsigned int bDieChmEn)
{
    U_VHDDIECTRL VHDDIECTRL;
    VHDDIECTRL.u32 = 0x0;

    BUG_ON( enLayer != HAL_LAYER_VIDEO1 );

    switch(enLayer)
    {
        case HAL_LAYER_VIDEO1:
        {
            VHDDIECTRL.u32 = reg_read(&(pVoReg->VHDDIECTRL.u32));
            VHDDIECTRL.bits.die_luma_en = bDieLumEn;
            VHDDIECTRL.bits.die_chroma_en = bDieChmEn;
            reg_write(&(pVoReg->VHDDIECTRL.u32), VHDDIECTRL.u32);

            logd("bDieLumEn = %d, bDieChmEn = %d\n", bDieLumEn, bDieChmEn);

            break;
        }
        default:
        {
            loge("layer id error!\n");
            break;
        }
    }

    return;
}

void hal_set_die_outsel(HAL_LAYER_E enLayer, unsigned int bOutSel)
{
    U_VHDDIECTRL VHDDIECTRL;
    VHDDIECTRL.u32 = 0x0;

    BUG_ON( enLayer != HAL_LAYER_VIDEO1 );

    switch(enLayer)
    {
        case HAL_LAYER_VIDEO1:
        {
            VHDDIECTRL.u32 = reg_read(&(pVoReg->VHDDIECTRL.u32));
            VHDDIECTRL.bits.die_out_sel_l = bOutSel;
            reg_write(&(pVoReg->VHDDIECTRL.u32), VHDDIECTRL.u32);

            break;
        }
        default:
        {
            loge("layer id error!\n");
            break ;
        }
    }

    return;
}

void hal_set_die_chm_outsel(HAL_LAYER_E enLayer, unsigned int bOutSel)
{
    U_VHDDIECTRL VHDDIECTRL;
    VHDDIECTRL.u32 = 0x0;

    BUG_ON( enLayer != HAL_LAYER_VIDEO1 );

    switch(enLayer)
    {
        case HAL_LAYER_VIDEO1:
        {
            VHDDIECTRL.u32 = reg_read(&(pVoReg->VHDDIECTRL.u32));
            VHDDIECTRL.bits.die_out_sel_c = bOutSel;
            reg_write(&(pVoReg->VHDDIECTRL.u32), VHDDIECTRL.u32);

            break;
        }
        default:
        {
            loge("layer id error!\n");
            break ;
        }
    }

    return;
}

void hal_set_die_chm_mode(HAL_LAYER_E enLayer, unsigned int bChmMode)
{
    U_VHDDIECTRL VHDDIECTRL;
    VHDDIECTRL.u32 = 0x0;

    BUG_ON( enLayer != HAL_LAYER_VIDEO1 );
    switch(enLayer)
    {
        case HAL_LAYER_VIDEO1:
        {
            VHDDIECTRL.u32 = reg_read(&(pVoReg->VHDDIECTRL.u32));
            VHDDIECTRL.bits.die_chmmode = bChmMode;
            reg_write(&(pVoReg->VHDDIECTRL.u32), VHDDIECTRL.u32);

            break;
        }
        default:
        {
            loge("layer id error!\n");
            break ;
        }
    }

    return;
}

void hal_set_die_stp_rst(HAL_LAYER_E enLayer, unsigned int bRstEn)
{
    U_VHDDIECTRL VHDDIECTRL;
    VHDDIECTRL.u32 = 0x0;

    BUG_ON( enLayer != HAL_LAYER_VIDEO1 );

    switch(enLayer)
    {
        case HAL_LAYER_VIDEO1:
        {
            VHDDIECTRL.u32 = reg_read(&(pVoReg->VHDDIECTRL.u32));
            VHDDIECTRL.bits.die_rst = bRstEn;
            reg_write(&(pVoReg->VHDDIECTRL.u32), VHDDIECTRL.u32);

            break;
        }
        default:
        {
            loge("layer id error!\n");
            break ;
        }
    }

    return;
}

void hal_set_die_stp_update(HAL_LAYER_E enLayer, unsigned int bUpdateEn)
{
    U_VHDDIECTRL VHDDIECTRL;
    VHDDIECTRL.u32 = 0x0;

    BUG_ON( enLayer != HAL_LAYER_VIDEO1 );

    switch(enLayer)
    {
        case HAL_LAYER_VIDEO1:
        {
            if (bUpdateEn == 0)
            {
                VHDDIECTRL.u32 = reg_read(&(pVoReg->VHDDIECTRL.u32));
                VHDDIECTRL.bits.stinfo_stop = 1;
                reg_write(&(pVoReg->VHDDIECTRL.u32), VHDDIECTRL.u32);
            }
            else
            {
                VHDDIECTRL.u32 = reg_read(&(pVoReg->VHDDIECTRL.u32));
                VHDDIECTRL.bits.stinfo_stop = 0;
                reg_write(&(pVoReg->VHDDIECTRL.u32), VHDDIECTRL.u32);
            }

            break;

        }
        default:
        {
            loge("layer id error!\n");
            break ;
        }
    }

    return;
}

void hal_set_die_dir_inten(HAL_LAYER_E enLayer, unsigned int dData)
{
    U_VHDDIEINTEN VHDDIEINTEN;
    VHDDIEINTEN.u32 = 0x0;

    BUG_ON( enLayer != HAL_LAYER_VIDEO1 );

    switch(enLayer)
    {
        case HAL_LAYER_VIDEO1:
        {
            VHDDIEINTEN.u32 = reg_read(&(pVoReg->VHDDIEINTEN.u32));
            VHDDIEINTEN.bits.dir_inten  = (unsigned int)dData;

            reg_write(&(pVoReg->VHDDIEINTEN.u32), VHDDIEINTEN.u32);

            break;
        }
        default:
        {
            loge("layer id error!\n");
            break ;
        }
    }

    return;
}

void hal_set_die_ver_dir_inten(HAL_LAYER_E enLayer, unsigned int dData)
{
    U_VHDDIEINTEN VHDDIEINTEN;
    VHDDIEINTEN.u32 = 0x0;

    BUG_ON( enLayer != HAL_LAYER_VIDEO1 );

    switch(enLayer)
    {
        case HAL_LAYER_VIDEO1:
        {
            VHDDIEINTEN.u32 = reg_read(&(pVoReg->VHDDIEINTEN.u32));
            VHDDIEINTEN.bits.dir_inten_ver  = (unsigned int)dData;
            reg_write(&(pVoReg->VHDDIEINTEN.u32), VHDDIEINTEN.u32);

            break;
        }
        default:
        {
            loge("layer id error!\n");
            break ;
        }
    }

    return;
}

void hal_set_die_ver_min_inten(HAL_LAYER_E enLayer, short s16Data)
{
    U_VHDDIEINTEN VHDDIEINTEN;
    VHDDIEINTEN.u32 = 0x0;

    BUG_ON( enLayer != HAL_LAYER_VIDEO1 );

    switch(enLayer)
    {
        case HAL_LAYER_VIDEO1:
        {
            VHDDIEINTEN.u32 = reg_read(&(pVoReg->VHDDIEINTEN.u32));
            VHDDIEINTEN.bits.ver_min_inten  = s16Data;
            reg_write(&(pVoReg->VHDDIEINTEN.u32), VHDDIEINTEN.u32);

            break;
        }
        default:
        {
            loge("layer id error!\n");
            break ;
        }
    }

    return;
}

void hal_set_die_scale(HAL_LAYER_E enLayer, unsigned char u8Data)
{
    U_VHDDIESCALE VHDDIESCALE;
    VHDDIESCALE.u32 = 0x0;

    BUG_ON( enLayer != HAL_LAYER_VIDEO1 );

    switch(enLayer)
    {
        case HAL_LAYER_VIDEO1:
        {
            VHDDIESCALE.u32 = reg_read(&(pVoReg->VHDDIESCALE.u32));
            VHDDIESCALE.bits.range_scale  = u8Data;
            reg_write(&(pVoReg->VHDDIESCALE.u32), VHDDIESCALE.u32);

            break;
        }
        default:
        {
            loge("layer id error!\n");
            break ;
        }
    }

    return;
}


void hal_set_die_ck_gain(HAL_LAYER_E enLayer, unsigned char u8CkId, unsigned int dData)
{
    U_VHDDIECHECK1 VHDDIECHECK1;
    U_VHDDIECHECK2 VHDDIECHECK2;
    VHDDIECHECK1.u32 = 0x0;
    VHDDIECHECK2.u32 = 0x0;

    BUG_ON( enLayer != HAL_LAYER_VIDEO1 );

    switch(enLayer)
    {
        case HAL_LAYER_VIDEO1:
        {
            if(u8CkId == 1)
            {
                VHDDIECHECK1.u32 = reg_read(&(pVoReg->VHDDIECHECK1.u32));
                VHDDIECHECK1.bits.ck_gain  = (unsigned int)dData;
                reg_write(&(pVoReg->VHDDIECHECK1.u32), VHDDIECHECK1.u32);
            }
            else if(u8CkId == 2)
            {
                VHDDIECHECK2.u32 = reg_read(&(pVoReg->VHDDIECHECK2.u32));
                VHDDIECHECK2.bits.ck_gain  = (unsigned int)dData;
                reg_write(&(pVoReg->VHDDIECHECK2.u32), VHDDIECHECK2.u32);
            }
            else
            {
                loge("Selected Wrong CHECK Module ID!\n");
            }

            break;
        }
        default:
        {
            loge("layer id error!\n");
            break ;
        }
    }

    return;
}

void hal_set_die_ck_gn_range(HAL_LAYER_E enLayer, unsigned char u8CkId, unsigned int dData)
{
    U_VHDDIECHECK1 VHDDIECHECK1;
    U_VHDDIECHECK2 VHDDIECHECK2;
    VHDDIECHECK1.u32 = 0x0;
    VHDDIECHECK2.u32 = 0x0;

    BUG_ON( enLayer != HAL_LAYER_VIDEO1 );

    switch(enLayer)
    {
        case HAL_LAYER_VIDEO1:
        {
            if(1 == u8CkId)
            {
                VHDDIECHECK1.u32 = reg_read(&(pVoReg->VHDDIECHECK1.u32));
                VHDDIECHECK1.bits.ck_range_gain  = (unsigned int)dData;
                reg_write(&(pVoReg->VHDDIECHECK1.u32), VHDDIECHECK1.u32);
            }
            else if(2 == u8CkId)
            {
                VHDDIECHECK2.u32 = reg_read(&(pVoReg->VHDDIECHECK2.u32));
                VHDDIECHECK2.bits.ck_range_gain  = (unsigned int)dData;
                reg_write(&(pVoReg->VHDDIECHECK2.u32), VHDDIECHECK2.u32);
            }
            else
            {
                loge("Selected Wrong check Module ID!\n");
            }

            break;
        }
        default:
        {
            loge("layer id error!\n");
            break ;
        }
    }

    return;
}

void hal_set_die_ck_max_range(HAL_LAYER_E enLayer, unsigned char u8CkId, unsigned char u8Data)
{
    U_VHDDIECHECK1 VHDDIECHECK1;
    U_VHDDIECHECK2 VHDDIECHECK2;
    VHDDIECHECK1.u32 = 0x0;
    VHDDIECHECK2.u32 = 0x0;

    BUG_ON( enLayer != HAL_LAYER_VIDEO1 );

    switch(enLayer)
    {
        case HAL_LAYER_VIDEO1:
        {
            if(u8CkId == 1)
            {
                VHDDIECHECK1.u32 = reg_read(&(pVoReg->VHDDIECHECK1.u32));
                VHDDIECHECK1.bits.ck_max_range  = u8Data;
                reg_write(&(pVoReg->VHDDIECHECK1.u32), VHDDIECHECK1.u32);
            }
            else if(u8CkId == 2)
            {
                VHDDIECHECK2.u32 = reg_read(&(pVoReg->VHDDIECHECK2.u32));
                VHDDIECHECK2.bits.ck_max_range  = u8Data;
                reg_write(&(pVoReg->VHDDIECHECK2.u32), VHDDIECHECK2.u32);
            }
            else
            {
                loge("Selected Wrong check Module ID!\n");
            }

            break;
        }
        default:
        {
            loge("layer id error!\n");
            break ;
        }
    }

    return;
}

void hal_set_die_dir_mult(HAL_LAYER_E enLayer, unsigned int u32Dir, unsigned int u32Data)
{
    U_VHDDIEDIR0_3      VHDDIEDIR0_3;
    U_VHDDIEDIR4_7      VHDDIEDIR4_7;
    U_VHDDIEDIR8_11     VHDDIEDIR8_11;
    U_VHDDIEDIR12_14    VHDDIEDIR12_14;
    VHDDIEDIR0_3.u32 = 0x0;
    VHDDIEDIR4_7.u32 = 0x0;
    VHDDIEDIR8_11.u32 = 0x0;
    VHDDIEDIR12_14.u32 = 0x0;

    BUG_ON( enLayer != HAL_LAYER_VIDEO1 );

    switch(enLayer)
    {
        case HAL_LAYER_VIDEO1:
        {
            if((u32Dir == VPP_DIE_DIRMULT_00) || (u32Dir == VPP_DIE_DIRMULT_00_03) || (u32Dir == VPP_DIE_DIRMULT_ALL))
            {
                VHDDIEDIR0_3.u32 = reg_read(&(pVoReg->VHDDIEDIR0_3.u32));
                VHDDIEDIR0_3.bits.dir0_mult  = u32Data;
                reg_write(&(pVoReg->VHDDIEDIR0_3.u32), VHDDIEDIR0_3.u32);
            }
            else if((u32Dir == VPP_DIE_DIRMULT_01) || (u32Dir == VPP_DIE_DIRMULT_00_03) || (u32Dir == VPP_DIE_DIRMULT_ALL))
            {
                VHDDIEDIR0_3.u32 = reg_read(&(pVoReg->VHDDIEDIR0_3.u32));
                VHDDIEDIR0_3.bits.dir1_mult  = u32Data;
                reg_write(&(pVoReg->VHDDIEDIR0_3.u32), VHDDIEDIR0_3.u32);
            }
            else if((u32Dir == VPP_DIE_DIRMULT_02) || (u32Dir == VPP_DIE_DIRMULT_00_03) || (u32Dir == VPP_DIE_DIRMULT_ALL))
            {
                VHDDIEDIR0_3.u32 = reg_read(&(pVoReg->VHDDIEDIR0_3.u32));
                VHDDIEDIR0_3.bits.dir2_mult  = u32Data;
                reg_write(&(pVoReg->VHDDIEDIR0_3.u32), VHDDIEDIR0_3.u32);
            }
            else if((u32Dir == VPP_DIE_DIRMULT_03) || (u32Dir == VPP_DIE_DIRMULT_00_03) || (u32Dir == VPP_DIE_DIRMULT_ALL))
            {
                VHDDIEDIR0_3.u32 = reg_read(&(pVoReg->VHDDIEDIR0_3.u32));
                VHDDIEDIR0_3.bits.dir3_mult  = u32Data;
                reg_write(&(pVoReg->VHDDIEDIR0_3.u32), VHDDIEDIR0_3.u32);
            }
            else if((u32Dir == VPP_DIE_DIRMULT_04) || (u32Dir == VPP_DIE_DIRMULT_04_07) || (u32Dir == VPP_DIE_DIRMULT_ALL))
            {
                VHDDIEDIR4_7.u32 = reg_read(&(pVoReg->VHDDIEDIR4_7.u32));
                VHDDIEDIR4_7.bits.dir4_mult  = u32Data;
                reg_write(&(pVoReg->VHDDIEDIR4_7.u32), VHDDIEDIR4_7.u32);
            }
            else if((u32Dir == VPP_DIE_DIRMULT_05) || (u32Dir == VPP_DIE_DIRMULT_04_07) || (u32Dir == VPP_DIE_DIRMULT_ALL))
            {
                VHDDIEDIR4_7.u32 = reg_read(&(pVoReg->VHDDIEDIR4_7.u32));
                VHDDIEDIR4_7.bits.dir5_mult  = u32Data;
                reg_write(&(pVoReg->VHDDIEDIR4_7.u32), VHDDIEDIR4_7.u32);
            }
            else if((u32Dir == VPP_DIE_DIRMULT_06) || (u32Dir == VPP_DIE_DIRMULT_04_07) || (u32Dir == VPP_DIE_DIRMULT_ALL))
            {
                VHDDIEDIR4_7.u32 = reg_read(&(pVoReg->VHDDIEDIR4_7.u32));
                VHDDIEDIR4_7.bits.dir6_mult  = u32Data;
                reg_write(&(pVoReg->VHDDIEDIR4_7.u32), VHDDIEDIR4_7.u32);
            }
            else if((u32Dir == VPP_DIE_DIRMULT_07) || (u32Dir == VPP_DIE_DIRMULT_04_07) || (u32Dir == VPP_DIE_DIRMULT_ALL))
            {
                VHDDIEDIR4_7.u32 = reg_read(&(pVoReg->VHDDIEDIR4_7.u32));
                VHDDIEDIR4_7.bits.dir7_mult  = u32Data;
                reg_write(&(pVoReg->VHDDIEDIR4_7.u32), VHDDIEDIR4_7.u32);
            }
            else if((u32Dir == VPP_DIE_DIRMULT_08) || (u32Dir == VPP_DIE_DIRMULT_08_11) || (u32Dir == VPP_DIE_DIRMULT_ALL))
            {
                VHDDIEDIR8_11.u32 = reg_read(&(pVoReg->VHDDIEDIR8_11.u32));
                VHDDIEDIR8_11.bits.dir8_mult  = u32Data;
                reg_write(&(pVoReg->VHDDIEDIR8_11.u32), VHDDIEDIR8_11.u32);
            }
            else if((u32Dir == VPP_DIE_DIRMULT_09) || (u32Dir == VPP_DIE_DIRMULT_08_11) || (u32Dir == VPP_DIE_DIRMULT_ALL))
            {
                VHDDIEDIR8_11.u32 = reg_read(&(pVoReg->VHDDIEDIR8_11.u32));
                VHDDIEDIR8_11.bits.dir9_mult  = u32Data;
                reg_write(&(pVoReg->VHDDIEDIR8_11.u32), VHDDIEDIR8_11.u32);
            }
            else if((u32Dir == VPP_DIE_DIRMULT_10) || (u32Dir == VPP_DIE_DIRMULT_08_11) || (u32Dir == VPP_DIE_DIRMULT_ALL))
            {
                VHDDIEDIR8_11.u32 = reg_read(&(pVoReg->VHDDIEDIR8_11.u32));
                VHDDIEDIR8_11.bits.dir10_mult  = u32Data;
                reg_write(&(pVoReg->VHDDIEDIR8_11.u32), VHDDIEDIR8_11.u32);
            }
            else if((u32Dir == VPP_DIE_DIRMULT_11) || (u32Dir == VPP_DIE_DIRMULT_08_11) || (u32Dir == VPP_DIE_DIRMULT_ALL))
            {
                VHDDIEDIR8_11.u32 = reg_read(&(pVoReg->VHDDIEDIR8_11.u32));
                VHDDIEDIR8_11.bits.dir11_mult  = u32Data;
                reg_write(&(pVoReg->VHDDIEDIR8_11.u32), VHDDIEDIR8_11.u32);
            }
            else if((u32Dir == VPP_DIE_DIRMULT_12) || (u32Dir == VPP_DIE_DIRMULT_12_14) || (u32Dir == VPP_DIE_DIRMULT_ALL))
            {
                VHDDIEDIR12_14.u32 = reg_read(&(pVoReg->VHDDIEDIR12_14.u32));
                VHDDIEDIR12_14.bits.dir12_mult  = u32Data;
                reg_write(&(pVoReg->VHDDIEDIR12_14.u32), VHDDIEDIR12_14.u32);
            }
            else if((u32Dir == VPP_DIE_DIRMULT_13) || (u32Dir == VPP_DIE_DIRMULT_12_14) || (u32Dir == VPP_DIE_DIRMULT_ALL))
            {
                VHDDIEDIR12_14.u32 = reg_read(&(pVoReg->VHDDIEDIR12_14.u32));
                VHDDIEDIR12_14.bits.dir13_mult  = u32Data;
                reg_write(&(pVoReg->VHDDIEDIR12_14.u32), VHDDIEDIR12_14.u32);
            }
            else if((u32Dir == VPP_DIE_DIRMULT_14) || (u32Dir == VPP_DIE_DIRMULT_12_14) || (u32Dir == VPP_DIE_DIRMULT_ALL))
            {
                VHDDIEDIR12_14.u32 = reg_read(&(pVoReg->VHDDIEDIR12_14.u32));
                VHDDIEDIR12_14.bits.dir14_mult  = u32Data;
                reg_write(&(pVoReg->VHDDIEDIR12_14.u32), VHDDIEDIR12_14.u32);
            }
            else
            {
                loge("Selected Wrong Dir ID!\n");
                return;
            }

            break;
        }
        default:
        {
            loge("layer id error!\n");
            break ;
        }
    }

    return;
}

void hal_set_die_ck_enh(HAL_LAYER_E enLayer, unsigned int u32Enh)
{
    U_VHDDIEINTEN VHDDIEINTEN;
    VHDDIEINTEN.u32 = 0x0;

    BUG_ON( enLayer != HAL_LAYER_VIDEO1 );

    switch(enLayer)
    {
        case HAL_LAYER_VIDEO1:
        {
            VHDDIEINTEN.u32 = reg_read(&(pVoReg->VHDDIEINTEN.u32));
            VHDDIEINTEN.bits.dir_ck_enh  = u32Enh;
            reg_write(&(pVoReg->VHDDIEINTEN.u32), VHDDIEINTEN.u32);

            break;
        }
        default:
        {
            loge("layer id error!\n");
            break ;
        }
    }

    return;
}

void hal_set_die_mf_max(HAL_LAYER_E enLayer, unsigned int u32LumMd, unsigned int u32ChmMd)
{
    U_VHDDIELMA2 VHDDIELMA2;
    VHDDIELMA2.u32 = 0x0;

    BUG_ON( enLayer != HAL_LAYER_VIDEO1 );

    switch(enLayer)
    {
        case HAL_LAYER_VIDEO1:
        {
            VHDDIELMA2.u32 = reg_read(&(pVoReg->VHDDIELMA2.u32));
            VHDDIELMA2.bits.luma_mf_max    = u32LumMd;
            VHDDIELMA2.bits.chroma_mf_max  = u32ChmMd;
            reg_write(&(pVoReg->VHDDIELMA2.u32), VHDDIELMA2.u32);

            break;
        }
        default:
        {
            loge("layer id error!\n");
            break ;
        }
    }

    return;
}


void hal_set_die_def_thd(HAL_LAYER_E enLayer)
{
    unsigned int ii = 0;
    unsigned char u8QThd[3]    = {16,48,96};
    unsigned char u8ReqThd[4]  = {0,32,96,255};
    unsigned char u8DirThd[15] = {40,24,32,27,18,15,12,11,9,8,7,6,5,5,3};

    BUG_ON( enLayer != HAL_LAYER_VIDEO1 );

    switch(enLayer)
    {
        case HAL_LAYER_VIDEO1:
        {
            hal_set_die_luma_mode   (enLayer, VPP_DIE_MODE_5FIELD);
            hal_set_die_chm_mode    (enLayer, VPP_DIE_MODE_5FIELD);

            hal_set_die_stp_update  (enLayer, true);
            hal_set_die_stp_rst     (enLayer, false);
            hal_set_die_outsel     (enLayer, false);

            hal_set_die_dir_inten   (enLayer,   1*4);
            hal_set_die_ver_dir_inten(enLayer,     2);
            hal_set_die_ver_min_inten(enLayer, -4000);

            hal_set_die_scale      (enLayer, 2);

            hal_set_die_ck_gain     (enLayer, 1, 1*4);
            hal_set_die_ck_gn_range  (enLayer, 1, 1*4);
            hal_set_die_ck_max_range (enLayer, 1,  30);

            hal_set_die_ck_gain     (enLayer, 2, 1*4);
            hal_set_die_ck_gn_range  (enLayer, 2, 1*4);
            hal_set_die_ck_max_range (enLayer, 2,  30);

            //new die default function
            hal_set_die_luma_q_tab   (enLayer, u8QThd);
            hal_set_die_luma_req_tab (enLayer, u8ReqThd);
            hal_set_die_scaleRatio (enLayer, 1*16);
            hal_set_die_win_size    (enLayer, 2);
            hal_set_die_sce_max     (enLayer, 0);
            hal_set_die_qrst_max    (enLayer, 0);

            hal_set_die_chm_ccr_enable(enLayer, 0);
            hal_set_die_chm_ma_offset (enLayer, 8);
            hal_set_die_xchm_max     (enLayer, 255);
            hal_set_die_ccr_detect   (enLayer, 8, 32, 8);
            hal_set_die_similar     (enLayer, 0, 255);

            hal_set_die_mf_max       (enLayer, 0, 0);
            hal_set_die_ck_enh       (enLayer, 1);

            for(ii=0;ii<15;ii++)
            {
                hal_set_die_dir_mult (enLayer, ii, u8DirThd[ii]);
            }

            break;
        }
        default:
        {
            loge("layer id error!\n");
            break ;
        }
    }

    return;
}

//pilot new die drivers
void hal_set_die_luma_mode(HAL_LAYER_E enLayer, unsigned int bLumaMode)
{
    U_VHDDIECTRL VHDDIECTRL;
    VHDDIECTRL.u32 = 0x0;

    BUG_ON( enLayer != HAL_LAYER_VIDEO1 );

    switch(enLayer)
    {
        case HAL_LAYER_VIDEO1:
        {
            VHDDIECTRL.u32 = reg_read(&(pVoReg->VHDDIECTRL.u32));
            VHDDIECTRL.bits.die_lmmode = bLumaMode;
            reg_write(&(pVoReg->VHDDIECTRL.u32), VHDDIECTRL.u32);

            break;
        }
        default:
        {
            loge("layer id error!\n");
            break ;
        }
    }

    return;
}

void hal_set_die_luma_q_tab(HAL_LAYER_E enLayer, unsigned char * up8Thd)
{
    U_VHDDIELMA0 VHDDIELMA0;
    VHDDIELMA0.u32 = 0x0;

    if( NULL == up8Thd ){
        loge("up8Thd is null!\n");
        return;
    }

    switch(enLayer)
    {
        case HAL_LAYER_VIDEO1:
        {
            VHDDIELMA0.u32 = reg_read(&(pVoReg->VHDDIELMA0.u32));
            VHDDIELMA0.bits.luma_qtbl1 = up8Thd[0];
            VHDDIELMA0.bits.luma_qtbl2 = up8Thd[1];
            VHDDIELMA0.bits.luma_qtbl3 = up8Thd[2];
            reg_write(&(pVoReg->VHDDIELMA0.u32), VHDDIELMA0.u32);

            break;
        }
        default:
        {
            loge("layer id error!\n");
            break ;
        }
    }

    return;
}

void hal_set_die_luma_req_tab(HAL_LAYER_E enLayer, unsigned char * up8Thd)
{
    U_VHDDIELMA1 VHDDIELMA1;
    VHDDIELMA1.u32 = 0x0;

    BUG_ON( enLayer != HAL_LAYER_VIDEO1 );

    switch(enLayer)
    {
        case HAL_LAYER_VIDEO1:
        {
            VHDDIELMA1.u32 = reg_read(&(pVoReg->VHDDIELMA1.u32));
            VHDDIELMA1.bits.luma_reqtbl0 = up8Thd[0];
            VHDDIELMA1.bits.luma_reqtbl1 = up8Thd[1];
            VHDDIELMA1.bits.luma_reqtbl2 = up8Thd[2];
            VHDDIELMA1.bits.luma_reqtbl3 = up8Thd[3];
            reg_write(&(pVoReg->VHDDIELMA1.u32), VHDDIELMA1.u32);

            break;
        }
        default:
        {
            loge("layer id error!\n");
            break ;
        }
    }

    return;
}

void hal_set_die_scaleRatio(HAL_LAYER_E enLayer, unsigned int u32Ratio)
{
    U_VHDDIELMA0 VHDDIELMA0;
    VHDDIELMA0.u32 = 0x0;

    BUG_ON( enLayer != HAL_LAYER_VIDEO1 );

    switch(enLayer)
    {
        case HAL_LAYER_VIDEO1:
        {
            VHDDIELMA0.u32 = reg_read(&(pVoReg->VHDDIELMA0.u32));
            VHDDIELMA0.bits.scale_ratio_ppd = u32Ratio;
            reg_write(&(pVoReg->VHDDIELMA0.u32), VHDDIELMA0.u32);

            break;
        }
        default:
        {
            loge("layer id error!\n");
            break ;
        }
    }

    return;
}

void hal_set_die_win_size(HAL_LAYER_E enLayer, unsigned int u32Size)
{
    U_VHDDIELMA2 VHDDIELMA2;
    VHDDIELMA2.u32 = 0x0;

    BUG_ON( enLayer != HAL_LAYER_VIDEO1 );

    switch(enLayer)
    {
        case HAL_LAYER_VIDEO1:
        {
            VHDDIELMA2.u32 = reg_read(&(pVoReg->VHDDIELMA2.u32));
            VHDDIELMA2.bits.luma_win_size = u32Size;
            reg_write(&(pVoReg->VHDDIELMA2.u32), VHDDIELMA2.u32);

            break;
        }
        default:
        {
            loge("layer id error!\n");
            break ;
        }
    }

    return;
}

void hal_set_die_sce_max(HAL_LAYER_E enLayer, unsigned int u32Sel)
{
    U_VHDDIELMA2 VHDDIELMA2;
    VHDDIELMA2.u32 = 0x0;

    BUG_ON( enLayer != HAL_LAYER_VIDEO1 );

    switch(enLayer)
    {
        case HAL_LAYER_VIDEO1:
        {
            VHDDIELMA2.u32 = reg_read(&(pVoReg->VHDDIELMA2.u32));
            VHDDIELMA2.bits.luma_scesdf_max = u32Sel;
            reg_write(&(pVoReg->VHDDIELMA2.u32), VHDDIELMA2.u32);

            break;
        }
        default:
        {
            loge("layer id error!\n");
            break ;
        }
    }

    return;
}

void hal_set_die_qrst_max(HAL_LAYER_E enLayer, unsigned int u32Sel)
{
    U_VHDDIELMA2 VHDDIELMA2;
    VHDDIELMA2.u32 = 0x0;

    BUG_ON( enLayer != HAL_LAYER_VIDEO1 );

    switch(enLayer)
    {
        case HAL_LAYER_VIDEO1:
        {
            VHDDIELMA2.u32 = reg_read(&(pVoReg->VHDDIELMA2.u32));
            VHDDIELMA2.bits.luma_qrst_max = u32Sel;
            reg_write(&(pVoReg->VHDDIELMA2.u32), VHDDIELMA2.u32);

            break;
        }
        default:
        {
            loge("layer id error!\n");
            break ;
        }
    }

    return;
}

void hal_set_die_st_enable(HAL_LAYER_E enLayer, VPP_DIE_STMD_E enMd, unsigned int u32Enable)
{
    U_VHDDIELMA2 VHDDIELMA2;
    VHDDIELMA2.u32 = 0x0;

    BUG_ON( enLayer != HAL_LAYER_VIDEO1 );

    switch(enLayer)
    {
        case HAL_LAYER_VIDEO1:
        {
            VHDDIELMA2.u32 = reg_read(&(pVoReg->VHDDIELMA2.u32));
            if(enMd == VPP_DIE_STMD_Q)
            {
                VHDDIELMA2.bits.die_st_qrst_en = u32Enable;
            }
            else if(enMd == VPP_DIE_STMD_N)
            {
                VHDDIELMA2.bits.die_st_n_en = u32Enable;
            }
            else if(enMd == VPP_DIE_STMD_M)
            {
                VHDDIELMA2.bits.die_st_m_en = u32Enable;
            }
            else if(enMd == VPP_DIE_STMD_L)
            {
                VHDDIELMA2.bits.die_st_l_en = u32Enable;
            }
            else if(enMd == VPP_DIE_STMD_K)
            {
                VHDDIELMA2.bits.die_st_k_en = u32Enable;
            }
            else
            {
                loge("Selected Wrong St Mode!\n");
            }
            reg_write(&(pVoReg->VHDDIELMA2.u32), VHDDIELMA2.u32);

            break;
        }
        default:
        {
            loge("layer id error!\n");
            break ;
        }
    }

    return;
}

void hal_set_die_st_addr(HAL_LAYER_E enLayer, VPP_DIE_STMD_E enMd, unsigned int u32Addr)
{
    U_VHDDIESTKADDR     VHDDIESTKADDR;
    U_VHDDIESTLADDR     VHDDIESTLADDR;
    U_VHDDIESTMADDR     VHDDIESTMADDR;
    U_VHDDIESTNADDR     VHDDIESTNADDR;
    U_VHDDIESTSQTRADDR  VHDDIESTSQTRADDR;
    VHDDIESTKADDR.u32 = 0x0;
    VHDDIESTLADDR.u32 = 0x0;
    VHDDIESTMADDR.u32 = 0x0;
    VHDDIESTNADDR.u32 = 0x0;
    VHDDIESTSQTRADDR.u32 = 0x0;

    BUG_ON( enLayer != HAL_LAYER_VIDEO1 );

    switch(enLayer)
    {
        case HAL_LAYER_VIDEO1:
        {
            if(enMd == VPP_DIE_STMD_Q)
            {
                VHDDIESTSQTRADDR.u32 = u32Addr;
                reg_write(&(pVoReg->VHDDIESTSQTRADDR.u32), VHDDIESTSQTRADDR.u32);
            }
            else if(enMd == VPP_DIE_STMD_N)
            {
                VHDDIESTNADDR.u32 = u32Addr;
                reg_write(&(pVoReg->VHDDIESTNADDR.u32), VHDDIESTNADDR.u32);
            }
            else if(enMd == VPP_DIE_STMD_M)
            {
                VHDDIESTMADDR.u32 = u32Addr;
                reg_write(&(pVoReg->VHDDIESTMADDR.u32), VHDDIESTMADDR.u32);
            }
            else if(enMd == VPP_DIE_STMD_L)
            {
                VHDDIESTLADDR.u32 = u32Addr;
                reg_write(&(pVoReg->VHDDIESTLADDR.u32), VHDDIESTLADDR.u32);
            }
            else if(enMd == VPP_DIE_STMD_K)
            {
                VHDDIESTKADDR.u32 = u32Addr;
                reg_write(&(pVoReg->VHDDIESTKADDR.u32), VHDDIESTKADDR.u32);
            }
            else
            {
                loge("Selected Wrong St Mode!\n");
            }

            break;
        }
        default:
        {
            loge("layer id error!\n");
            break ;
        }
    }

    return;
}


void hal_set_die_addr(HAL_LAYER_E enLayer, VPP_DIE_STMD_E enMd, unsigned int *pu32AddrCtrl, int nRepeat)
{
    unsigned int u32AddrCtrltmp = 0;
    unsigned int u32_w_addr = 0;
    unsigned int u32_st_addr = 0;
    unsigned int u32_k_addr = 0;
    unsigned int u32_qr_addr = 0;
    unsigned int u32_l_addr = 0;
    unsigned int u32_m_addr = 0;
    unsigned int u32_n_addr = 0;
    unsigned int ii = 0;

    BUG_ON( enLayer != HAL_LAYER_VIDEO1 );

    if (nRepeat == 0)
    {
        u32AddrCtrltmp = pu32AddrCtrl[9];
        for(ii=9;ii>0;ii--)
        {
            pu32AddrCtrl[ii] = pu32AddrCtrl[ii-1];
        }
        pu32AddrCtrl[0] = u32AddrCtrltmp;
    }

    u32_w_addr  = pu32AddrCtrl[0];
    u32_st_addr = pu32AddrCtrl[1];
    u32_k_addr  = pu32AddrCtrl[2];
    u32_qr_addr = pu32AddrCtrl[3];
    u32_l_addr  = pu32AddrCtrl[4];
    u32_m_addr  = pu32AddrCtrl[6];
    u32_n_addr  = pu32AddrCtrl[8];

    hal_die_addr(enLayer, u32_w_addr);
    hal_set_die_st_addr(enLayer, VPP_DIE_STMD_K, u32_k_addr);
    hal_set_die_st_addr(enLayer, VPP_DIE_STMD_L, u32_l_addr);
    hal_set_die_st_addr(enLayer, VPP_DIE_STMD_M, u32_m_addr);
    hal_set_die_st_addr(enLayer, VPP_DIE_STMD_N, u32_n_addr);

    if(enMd == VPP_DIE_STMD_KLQR || enMd == VPP_DIE_STMD_KLQRM)
    {
        hal_set_die_st_addr(enLayer, VPP_DIE_STMD_Q, u32_qr_addr);
    }
    else
    {
        hal_set_die_st_addr(enLayer, VPP_DIE_STMD_Q, u32_st_addr);
    }

    return;
}


void hal_set_die_chm_ccr_enable(HAL_LAYER_E enLayer, unsigned int u32En)
{
    U_VHDCCRSCLR0 VHDCCRSCLR0;
    VHDCCRSCLR0.u32 = 0x0;

    BUG_ON( enLayer != HAL_LAYER_VIDEO1 );

    switch(enLayer)
    {
        case HAL_LAYER_VIDEO1:
        {
            VHDCCRSCLR0.u32 = reg_read(&(pVoReg->VHDCCRSCLR0.u32));
            VHDCCRSCLR0.bits.chroma_ccr_en = u32En;
            reg_write(&(pVoReg->VHDCCRSCLR0.u32), VHDCCRSCLR0.u32);

            break;
        }
        default:
        {
            loge("layer id error!\n");
            break ;
        }
    }

    return;
}

void hal_set_die_chm_ma_offset(HAL_LAYER_E enLayer, unsigned int u32Off)
{
    U_VHDCCRSCLR0 VHDCCRSCLR0;
    VHDCCRSCLR0.u32 = 0x0;

    BUG_ON( enLayer != HAL_LAYER_VIDEO1 );

    switch(enLayer)
    {
        case HAL_LAYER_VIDEO1:
        {
            VHDCCRSCLR0.u32 = reg_read(&(pVoReg->VHDCCRSCLR0.u32));
            VHDCCRSCLR0.bits.chroma_ma_offset = u32Off;
            reg_write(&(pVoReg->VHDCCRSCLR0.u32), VHDCCRSCLR0.u32);

            break;
        }
        default:
        {
            loge("layer id error!\n");
            break ;
        }
    }

    return;
}

void hal_set_die_xchm_max(HAL_LAYER_E enLayer, unsigned int u32Max)
{
    U_VHDCCRSCLR1 VHDCCRSCLR1;
    VHDCCRSCLR1.u32 = 0x0;

    BUG_ON( enLayer != HAL_LAYER_VIDEO1 );

    switch(enLayer)
    {
        case HAL_LAYER_VIDEO1:
        {
            VHDCCRSCLR1.u32 = reg_read(&(pVoReg->VHDCCRSCLR1.u32));
            VHDCCRSCLR1.bits.max_xchroma = u32Max;
            reg_write(&(pVoReg->VHDCCRSCLR1.u32), VHDCCRSCLR1.u32);

            break;
        }
        default:
        {
            loge("layer id error!\n");
            break ;
        }
    }

    return;
}

void hal_set_die_ccr_detect(HAL_LAYER_E enLayer, unsigned int u32Thd, unsigned int u32Max, unsigned int u32Bld)
{
    U_VHDCCRSCLR0 VHDCCRSCLR0;
    U_VHDCCRSCLR1 VHDCCRSCLR1;
    VHDCCRSCLR0.u32 = 0x0;
    VHDCCRSCLR1.u32 = 0x0;

    BUG_ON( enLayer != HAL_LAYER_VIDEO1 );

    switch(enLayer)
    {
        case HAL_LAYER_VIDEO1:
        {
            VHDCCRSCLR0.u32 = reg_read(&(pVoReg->VHDCCRSCLR0.u32));
            VHDCCRSCLR1.u32 = reg_read(&(pVoReg->VHDCCRSCLR1.u32));

            VHDCCRSCLR0.bits.no_ccr_detect_thd = u32Thd;
            VHDCCRSCLR0.bits.no_ccr_detect_max = u32Max;

            VHDCCRSCLR1.bits.no_ccr_detect_blend = u32Bld;

            reg_write(&(pVoReg->VHDCCRSCLR0.u32), VHDCCRSCLR0.u32);
            reg_write(&(pVoReg->VHDCCRSCLR1.u32), VHDCCRSCLR1.u32);

            break;
        }
        default:
        {
            loge("layer id error!\n");
            break ;
        }
    }

    return;
}

void hal_set_die_similar(HAL_LAYER_E enLayer, unsigned int u32Thd, unsigned int u32Max)
{
    U_VHDCCRSCLR1 VHDCCRSCLR1;
    VHDCCRSCLR1.u32 = 0x0;

    BUG_ON( enLayer != HAL_LAYER_VIDEO1 );

    switch(enLayer)
    {
        case HAL_LAYER_VIDEO1:
        {
            VHDCCRSCLR1.u32 = reg_read(&(pVoReg->VHDCCRSCLR1.u32));
            VHDCCRSCLR1.bits.similar_thd = u32Thd;
            VHDCCRSCLR1.bits.similar_max = u32Max;
            reg_write(&(pVoReg->VHDCCRSCLR1.u32), VHDCCRSCLR1.u32);

            break;
        }
        default:
        {
            loge("layer id error!\n");
            break ;
        }
    }

    return;
}

void hal_set_die_st_mode(HAL_LAYER_E enLayer, unsigned int enMd)
{
    U_VHDDIELMA2 VHDDIELMA2;
    VHDDIELMA2.u32 = 0x0;

    BUG_ON( enLayer != HAL_LAYER_VIDEO1 );

    switch(enMd)
    {
        case VPP_DIE_STMD_KLM:
        {
            hal_set_die_st_enable(enLayer, VPP_DIE_STMD_Q, false);
            hal_set_die_st_enable(enLayer, VPP_DIE_STMD_N, false);
            hal_set_die_st_enable(enLayer, VPP_DIE_STMD_M, true);
            hal_set_die_st_enable(enLayer, VPP_DIE_STMD_L, true);
            hal_set_die_st_enable(enLayer, VPP_DIE_STMD_K, true);

            break;
        }
        case VPP_DIE_STMD_KLQR:
        {
            hal_set_die_st_enable(enLayer, VPP_DIE_STMD_Q, true);
            hal_set_die_st_enable(enLayer, VPP_DIE_STMD_N, false);
            hal_set_die_st_enable(enLayer, VPP_DIE_STMD_M, false);
            hal_set_die_st_enable(enLayer, VPP_DIE_STMD_L, true);
            hal_set_die_st_enable(enLayer, VPP_DIE_STMD_K, true);
            break;
        }
        case VPP_DIE_STMD_KLQRM:
        {
            hal_set_die_st_enable(enLayer, VPP_DIE_STMD_Q, true);
            hal_set_die_st_enable(enLayer, VPP_DIE_STMD_N, false);
            hal_set_die_st_enable(enLayer, VPP_DIE_STMD_M, true);
            hal_set_die_st_enable(enLayer, VPP_DIE_STMD_L, true);
            hal_set_die_st_enable(enLayer, VPP_DIE_STMD_K, true);
            break;
        }
        case VPP_DIE_STMD_STKLM:
        {
            hal_set_die_st_enable(enLayer, VPP_DIE_STMD_Q, false);
            hal_set_die_st_enable(enLayer, VPP_DIE_STMD_N, false);
            hal_set_die_st_enable(enLayer, VPP_DIE_STMD_M, true);
            hal_set_die_st_enable(enLayer, VPP_DIE_STMD_L, true);
            hal_set_die_st_enable(enLayer, VPP_DIE_STMD_K, true);
            break;
        }
        case VPP_DIE_STMD_DISALL:
        {
            hal_set_die_st_enable(enLayer, VPP_DIE_STMD_Q, false);
            hal_set_die_st_enable(enLayer, VPP_DIE_STMD_N, false);
            hal_set_die_st_enable(enLayer, VPP_DIE_STMD_M, false);
            hal_set_die_st_enable(enLayer, VPP_DIE_STMD_L, false);
            hal_set_die_st_enable(enLayer, VPP_DIE_STMD_K, false);
            break;
        }
        default:
        {
            loge("layer id error!\n");
            break ;
        }
    }

    VHDDIELMA2.u32 = reg_read(&(pVoReg->VHDDIELMA2.u32));
    VHDDIELMA2.bits.die_st_mode = enMd;
    reg_write(&(pVoReg->VHDDIELMA2.u32), VHDDIELMA2.u32);

    return;
}

void hal_set_pd_dir_mch(HAL_LAYER_E enLayer, unsigned int u32Data)
{
    U_VHDPDCTRL VHDPDCTRL;
    VHDPDCTRL.u32 = 0x0;

    BUG_ON( enLayer != HAL_LAYER_VIDEO1 );

    switch(enLayer)
    {
        case HAL_LAYER_VIDEO1:
        {
            VHDPDCTRL.u32 = reg_read(&(pVoReg->VHDPDCTRL.u32));
            VHDPDCTRL.bits.dir_mch_l = u32Data;
            reg_write(&(pVoReg->VHDPDCTRL.u32), VHDPDCTRL.u32);

            break;
        }
        default:
        {
            loge("layer id error!\n");
            break ;
        }
    }

    return;
}

void hal_set_pd_chm_dir_mch(HAL_LAYER_E enLayer, unsigned int u32Data)
{
    U_VHDPDCTRL VHDPDCTRL;
    VHDPDCTRL.u32 = 0x0;

    BUG_ON( enLayer != HAL_LAYER_VIDEO1 );

    switch(enLayer)
    {
        case HAL_LAYER_VIDEO1:
        {
            VHDPDCTRL.u32 = reg_read(&(pVoReg->VHDPDCTRL.u32));
            VHDPDCTRL.bits.dir_mch_c = u32Data;
            reg_write(&(pVoReg->VHDPDCTRL.u32), VHDPDCTRL.u32);

            break;
        }
        default:
        {
            loge("layer id error!\n");
            return;
        }
    }

    return;
}

void hal_set_pd_blk_pos(HAL_LAYER_E enLayer, unsigned int u32Mode, unsigned int u32PosX, unsigned int u32PosY)
{
    U_VHDPDBLKPOS0 VHDPDBLKPOS0;
    U_VHDPDBLKPOS1 VHDPDBLKPOS1;
    VHDPDBLKPOS0.u32 = 0x0;
    VHDPDBLKPOS1.u32 = 0x0;

    BUG_ON( enLayer != HAL_LAYER_VIDEO1 );

    u32PosX = (u32PosX == 0)?1:u32PosX;
    u32PosY = (u32PosY == 0)?1:u32PosY;

    switch(enLayer)
    {
        case HAL_LAYER_VIDEO1:
        {
            if(u32Mode == 0)
            {
                VHDPDBLKPOS0.u32 = reg_read(&(pVoReg->VHDPDBLKPOS0.u32));
                VHDPDBLKPOS0.bits.blk_x = u32PosX - 1;
                VHDPDBLKPOS0.bits.blk_y = u32PosY - 1;
                reg_write(&(pVoReg->VHDPDBLKPOS0.u32), VHDPDBLKPOS0.u32);
            }
            else if(u32Mode == 1)
            {
                VHDPDBLKPOS1.u32 = reg_read(&(pVoReg->VHDPDBLKPOS1.u32));
                VHDPDBLKPOS1.bits.blk_x = u32PosX - 1;
                VHDPDBLKPOS1.bits.blk_y = u32PosY - 1;
                reg_write(&(pVoReg->VHDPDBLKPOS1.u32), VHDPDBLKPOS1.u32);
            }
            else
            {
                loge("Selected Wrong Mode ID!\n");
            }

            break;
        }
        default:
        {
            loge("layer id error!\n");
            break ;
        }
    }

    return;
}

void hal_set_pd_stl_blk_thd(HAL_LAYER_E enLayer, unsigned int u32Data)
{
    U_VHDPDBLKTHD VHDPDBLKTHD;
    VHDPDBLKTHD.u32 = 0x0;

    BUG_ON( enLayer != HAL_LAYER_VIDEO1 );

    switch(enLayer)
    {
        case HAL_LAYER_VIDEO1:
        {
            VHDPDBLKTHD.u32 = reg_read(&(pVoReg->VHDPDBLKTHD.u32));
            VHDPDBLKTHD.bits.stillblk_thd = u32Data;
            reg_write(&(pVoReg->VHDPDBLKTHD.u32), VHDPDBLKTHD.u32);

            break;
        }
        default:
        {
            loge("layer id error!\n");
            break ;
        }
    }

    return;
}

void hal_set_pd_diff_thd(HAL_LAYER_E enLayer, unsigned int u32Data)
{
    U_VHDPDBLKTHD VHDPDBLKTHD;
    VHDPDBLKTHD.u32 = 0x0;

    BUG_ON( enLayer != HAL_LAYER_VIDEO1 );

    switch(enLayer)
    {
        case HAL_LAYER_VIDEO1:
        {
            VHDPDBLKTHD.u32 = reg_read(&(pVoReg->VHDPDBLKTHD.u32));
            VHDPDBLKTHD.bits.diff_movblk_thd = u32Data;
            reg_write(&(pVoReg->VHDPDBLKTHD.u32), VHDPDBLKTHD.u32);

            break;
        }
        default:
        {
            loge("layer id error!\n");
            break ;
        }
    }

    return;
}

void hal_set_pd_hist_thd(HAL_LAYER_E enLayer, unsigned int * u32Data)
{
    U_VHDPDHISTTHD VHDPDHISTTHD;
    VHDPDHISTTHD.u32 = 0x0;

    if (NULL == u32Data){
        loge("u32Data is null!\n");
        return;
    }

    switch(enLayer)
    {
        case HAL_LAYER_VIDEO1:
        {
            VHDPDHISTTHD.u32 = reg_read(&(pVoReg->VHDPDHISTTHD.u32));
            VHDPDHISTTHD.bits.hist_thd0 = u32Data[0];
            VHDPDHISTTHD.bits.hist_thd1 = u32Data[1];
            VHDPDHISTTHD.bits.hist_thd2 = u32Data[2];
            VHDPDHISTTHD.bits.hist_thd3 = u32Data[3];
            reg_write(&(pVoReg->VHDPDHISTTHD.u32), VHDPDHISTTHD.u32);

            break;
        }
        default:
        {
            loge("layer id error!\n");
            break ;
        }
    }

    return;
}

void hal_set_pd_um_thd(HAL_LAYER_E enLayer, unsigned int * u32Data)
{
    U_VHDPDUMTHD VHDPDUMTHD;
    VHDPDUMTHD.u32 = 0x0;

    if (NULL == u32Data) {
        loge("u32Data is null\n");
        return;
    }

    switch(enLayer)
    {
        case HAL_LAYER_VIDEO1:
        {
            VHDPDUMTHD.u32 = reg_read(&(pVoReg->VHDPDUMTHD.u32));
            VHDPDUMTHD.bits.um_thd0 = u32Data[0];
            VHDPDUMTHD.bits.um_thd1 = u32Data[1];
            VHDPDUMTHD.bits.um_thd2 = u32Data[2];
            reg_write(&(pVoReg->VHDPDUMTHD.u32), VHDPDUMTHD.u32);

            break;
        }
        default:
        {
            loge("layer id error!\n");
            break ;
        }
    }

    return;
}

void hal_set_pd_coring_tkr(HAL_LAYER_E enLayer, unsigned int u32Data)
{
    U_VHDPDPCCCORING VHDPDPCCCORING;
    VHDPDPCCCORING.u32 = 0x0;

    BUG_ON( enLayer != HAL_LAYER_VIDEO1 );

    switch(enLayer)
    {
        case HAL_LAYER_VIDEO1:
        {
            VHDPDPCCCORING.u32 = reg_read(&(pVoReg->VHDPDPCCCORING.u32));
            VHDPDPCCCORING.bits.coring_tkr = u32Data;
            reg_write(&(pVoReg->VHDPDPCCCORING.u32), VHDPDPCCCORING.u32);

            break;
        }
        default:
        {
            loge("layer id error!\n");
            break ;
        }
    }

    return;
}

void hal_set_pd_coring_blk(HAL_LAYER_E enLayer, unsigned int u32Data)
{
    U_VHDPDPCCCORING VHDPDPCCCORING;
    VHDPDPCCCORING.u32 = 0x0;

    BUG_ON( enLayer != HAL_LAYER_VIDEO1 );

    switch(enLayer)
    {
        case HAL_LAYER_VIDEO1:
        {
            VHDPDPCCCORING.u32 = reg_read(&(pVoReg->VHDPDPCCCORING.u32));
            VHDPDPCCCORING.bits.coring_blk = u32Data;
            reg_write(&(pVoReg->VHDPDPCCCORING.u32), VHDPDPCCCORING.u32);

            break;
        }
        default:
        {
            loge("layer id error!\n");
            break ;
        }
    }

    return;
}

void hal_set_pd_coring_norm(HAL_LAYER_E enLayer, unsigned int u32Data)
{
    U_VHDPDPCCCORING VHDPDPCCCORING;
    VHDPDPCCCORING.u32 = 0x0;

    BUG_ON( enLayer != HAL_LAYER_VIDEO1 );

    switch(enLayer)
    {
        case HAL_LAYER_VIDEO1:
        {
            VHDPDPCCCORING.u32 = reg_read(&(pVoReg->VHDPDPCCCORING.u32));
            VHDPDPCCCORING.bits.coring_norm = u32Data;
            reg_write(&(pVoReg->VHDPDPCCCORING.u32), VHDPDPCCCORING.u32);

            break;
        }
        default:
        {
            loge("layer id error!\n");
            break ;
        }
    }

    return;
}

void hal_set_pd_pcc_hthd(HAL_LAYER_E enLayer, unsigned int u32Data)
{
    U_VHDPDPCCHTHD VHDPDPCCHTHD;
    VHDPDPCCHTHD.u32 = 0x0;

    BUG_ON( enLayer != HAL_LAYER_VIDEO1 );

    switch(enLayer)
    {
        case HAL_LAYER_VIDEO1:
        {
            VHDPDPCCHTHD.u32 = reg_read(&(pVoReg->VHDPDPCCHTHD.u32));
            VHDPDPCCHTHD.bits.pcc_hthd = u32Data;
            reg_write(&(pVoReg->VHDPDPCCHTHD.u32), VHDPDPCCHTHD.u32);

            break;
        }
        default:
        {
            loge("layer id error!\n");
            break ;
        }
    }

    return;
}

void hal_set_pd_pcc_vthd(HAL_LAYER_E enLayer, unsigned int * u32Data)
{
    U_VHDPDPCCVTHD VHDPDPCCVTHD;
    VHDPDPCCVTHD.u32 = 0x0;

    BUG_ON( enLayer != HAL_LAYER_VIDEO1 );

    switch(enLayer)
    {
        case HAL_LAYER_VIDEO1:
        {
            VHDPDPCCVTHD.u32 = reg_read(&(pVoReg->VHDPDPCCVTHD.u32));
            VHDPDPCCVTHD.bits.pcc_vthd0 = u32Data[0];
            VHDPDPCCVTHD.bits.pcc_vthd1 = u32Data[1];
            VHDPDPCCVTHD.bits.pcc_vthd2 = u32Data[2];
            VHDPDPCCVTHD.bits.pcc_vthd3 = u32Data[3];
            reg_write(&(pVoReg->VHDPDPCCVTHD.u32), VHDPDPCCVTHD.u32);

            break;
        }
        default:
        {
            loge("layer id error!\n");
            break ;
        }
    }

    return;
}

void hal_set_pd_smt_enable(HAL_LAYER_E enLayer, unsigned int u32Data)
{
    U_VHDPDCTRL VHDPDCTRL;
    VHDPDCTRL.u32 = 0x0;

    BUG_ON( enLayer != HAL_LAYER_VIDEO1 );

    switch(enLayer)
    {
        case HAL_LAYER_VIDEO1:
        {
            VHDPDCTRL.u32 = reg_read(&(pVoReg->VHDPDCTRL.u32));
            VHDPDCTRL.bits.edge_smooth_en = u32Data;
            reg_write(&(pVoReg->VHDPDCTRL.u32), VHDPDCTRL.u32);

            break;
        }
        default:
        {
            loge("layer id error!\n");
            break ;
        }
    }

    return;
}

void hal_set_pd_smt_thd(HAL_LAYER_E enLayer, unsigned int u32Data)
{
    U_VHDPDCTRL VHDPDCTRL;
    VHDPDCTRL.u32 = 0x0;

    BUG_ON( enLayer != HAL_LAYER_VIDEO1 );

    switch(enLayer)
    {
        case HAL_LAYER_VIDEO1:
        {
            VHDPDCTRL.u32 = reg_read(&(pVoReg->VHDPDCTRL.u32));
            VHDPDCTRL.bits.edge_smooth_ratio = u32Data;
            reg_write(&(pVoReg->VHDPDCTRL.u32), VHDPDCTRL.u32);

            break;
        }
        default:
        {
            loge("layer id error!\n");
            break ;
        }
    }

    return;
}

void hal_set_pd_it_diff_thd(HAL_LAYER_E enLayer, unsigned int * u32Data)
{
    U_VHDPDITDIFFVTHD VHDPDITDIFFVTHD;
    VHDPDITDIFFVTHD.u32 = 0x0;

    BUG_ON( enLayer != HAL_LAYER_VIDEO1 );

    switch(enLayer)
    {
        case HAL_LAYER_VIDEO1:
        {
            VHDPDITDIFFVTHD.u32 = reg_read(&(pVoReg->VHDPDITDIFFVTHD.u32));
            VHDPDITDIFFVTHD.bits.itdiff_vthd0 = u32Data[0];
            VHDPDITDIFFVTHD.bits.itdiff_vthd1 = u32Data[1];
            VHDPDITDIFFVTHD.bits.itdiff_vthd2 = u32Data[2];
            VHDPDITDIFFVTHD.bits.itdiff_vthd3 = u32Data[3];
            reg_write(&(pVoReg->VHDPDITDIFFVTHD.u32), VHDPDITDIFFVTHD.u32);

            break;
        }
        default:
        {
            loge("layer id error!\n");
            break ;
        }
    }

    return;
}

void hal_set_pd_lasi_thd(HAL_LAYER_E enLayer, unsigned int u32Data)
{
    U_VHDPDLASITHD VHDPDLASITHD;
    VHDPDLASITHD.u32 = 0x0;

    BUG_ON( enLayer != HAL_LAYER_VIDEO1 );

    switch(enLayer)
    {
        case HAL_LAYER_VIDEO1:
        {
            VHDPDLASITHD.u32 = reg_read(&(pVoReg->VHDPDLASITHD.u32));
            VHDPDLASITHD.bits.lasi_thd = u32Data;
            reg_write(&(pVoReg->VHDPDLASITHD.u32), VHDPDLASITHD.u32);

            break;
        }
        default:
        {
            loge("layer id error!\n");
            break ;
        }
    }

    return;
}

void hal_set_pd_edge_thd(HAL_LAYER_E enLayer, unsigned int u32Data)
{
    U_VHDPDLASITHD VHDPDLASITHD;
    VHDPDLASITHD.u32 = 0x0;

    BUG_ON( enLayer != HAL_LAYER_VIDEO1 );

    switch(enLayer)
    {
        case HAL_LAYER_VIDEO1:
        {
            VHDPDLASITHD.u32 = reg_read(&(pVoReg->VHDPDLASITHD.u32));
            VHDPDLASITHD.bits.edge_thd = u32Data;
            reg_write(&(pVoReg->VHDPDLASITHD.u32), VHDPDLASITHD.u32);

            break;
        }
        default:
        {
            loge("layer id error!\n");
            break ;
        }
    }

    return;
}

void hal_set_wbc_enable(HAL_LAYER_E enLayer, unsigned int bEnable)
{
    U_VHDCTRL VHDCTRL;
    VHDCTRL.u32 = 0x0;

    BUG_ON( enLayer != HAL_LAYER_WBC1 );

    if (enLayer == HAL_LAYER_WBC1)
    {
        VHDCTRL.u32 = reg_read(&(pVoReg->VHDCTRL.u32));
        VHDCTRL.bits.wbc1_en = bEnable;
        reg_write(&(pVoReg->VHDCTRL.u32), VHDCTRL.u32);
    }
    else
    {
        loge("hal_set_wbc_enable enLayer error!\n");
    }

    return;
}

void hal_set_wbc_addr(HAL_LAYER_E enLayer, unsigned int u32Addr)
{
    U_VHDWBC1ADDR VHDWBC1ADDR;
    VHDWBC1ADDR.u32 = 0x0;

    BUG_ON( enLayer != HAL_LAYER_WBC1 );

    if (enLayer == HAL_LAYER_WBC1)
    {
        VHDWBC1ADDR.u32 = u32Addr;
        reg_write(&(pVoReg->VHDWBC1ADDR.u32), VHDWBC1ADDR.u32);
    }
    else
    {
        loge("enLayer error!\n");
    }

    return;
}


void hal_set_wbc_stride(HAL_LAYER_E enLayer, unsigned short u16Str)
{
    U_VHDWBC1STRD VHDWBC1STRD;
    VHDWBC1STRD.u32 = 0x0;

    BUG_ON( enLayer != HAL_LAYER_WBC1 );

    if (enLayer == HAL_LAYER_WBC1)

    {
        VHDWBC1STRD.u32 = reg_read(&(pVoReg->VHDWBC1STRD.u32));
        VHDWBC1STRD.bits.wbc1strd = u16Str;
        reg_write(&(pVoReg->VHDWBC1STRD.u32), VHDWBC1STRD.u32);
    }
    else
    {
        loge("enLayer error!\n");
    }

    return;
}

void hal_set_wbc_md(HAL_WBC_RESOSEL_E enMdSel)
{
    U_VHDCTRL VHDCTRL;
    VHDCTRL.u32 = 0x0;

    VHDCTRL.u32 = reg_read(&(pVoReg->VHDCTRL.u32));
    VHDCTRL.bits.resource_sel = enMdSel;
    reg_write(&(pVoReg->VHDCTRL.u32), VHDCTRL.u32);

    return;
}

void hal_set_wbc_out_intf(HAL_LAYER_E enLayer, HAL_DATARMODE_E enRdMode)
{
    U_VHDCTRL VHDCTRL;
    VHDCTRL.u32 = 0x0;

    BUG_ON( enLayer != HAL_LAYER_WBC1 );

    if (enLayer == HAL_LAYER_WBC1)
    {
        VHDCTRL.u32 = reg_read(&(pVoReg->VHDCTRL.u32));
        if(enRdMode == HAL_PROGRESSIVE)
            VHDCTRL.bits.ofl_inter = 0;
        else
        {
            VHDCTRL.bits.ofl_inter = 1;
            if(enRdMode == HAL_TOP)
                VHDCTRL.bits.ofl_btm = 0;
            else if (enRdMode == HAL_BOTTOM)
                VHDCTRL.bits.ofl_btm = 1;
            else
                loge("enRdMode error!\n");
        }
        reg_write(&(pVoReg->VHDCTRL.u32), VHDCTRL.u32);
    }
    else
    {
        loge("enLayer error!\n");
    }

    return;
}

void hal_set_wbc_spd(HAL_LAYER_E enLayer, unsigned int u16ReqSpd)
{
    U_VHDWBC1STRD VHDWBC1STRD;
    VHDWBC1STRD.u32 = 0x0;

    BUG_ON( enLayer != HAL_LAYER_WBC1 );

    if(u16ReqSpd >= 1024)
    {
        loge("Test Data out of legal range\n");
        return;
    }

    if (enLayer == HAL_LAYER_WBC1)
    {
        //set wbc1 write back channel data requst interval value
        VHDWBC1STRD.u32 = reg_read(&(pVoReg->VHDWBC1STRD.u32));
        VHDWBC1STRD.bits.req_interval = u16ReqSpd;
        reg_write(&(pVoReg->VHDWBC1STRD.u32), VHDWBC1STRD.u32);
    }
    else
    {
        loge("Select Wrong Layer ID!\n");
    }

    return;
}

void hal_set_wbc_out_fmt(HAL_LAYER_E enLayer, VPP_INTFDATAFMT_E stIntfFmt)
{
    U_VHDWBC1STRD VHDWBC1STRD;
    VHDWBC1STRD.u32 = 0x0;

    BUG_ON( enLayer != HAL_LAYER_WBC1 );

    if (enLayer == HAL_LAYER_WBC1)
    {
        VHDWBC1STRD.u32 = reg_read(&(pVoReg->VHDWBC1STRD.u32));
        VHDWBC1STRD.bits.wbc1_dft = stIntfFmt;
        reg_write(&(pVoReg->VHDWBC1STRD.u32), VHDWBC1STRD.u32);
    }
    else
    {
        loge("enLayer error!\n");
    }

    return;

}

void hal_set_lnk_lst_node_int_en(unsigned int u32Data)
{
    U_VHDCTRL VHDCTRL;
    VHDCTRL.u32 = 0x0;

    VHDCTRL.u32 = reg_read(&(pVoReg->VHDCTRL.u32));
    VHDCTRL.bits.wbc1_int_en = u32Data;
    reg_write(&(pVoReg->VHDCTRL.u32), VHDCTRL.u32);

    return;
}

void hal_set_lnk_wb_addr(unsigned int u32Addr)
{
    U_VHDDIESTADDR VHDDIESTADDR;
    VHDDIESTADDR.u32 = 0x0;

    VHDDIESTADDR.u32 = u32Addr;
    reg_write(&(pVoReg->VHDDIESTADDR.u32), VHDDIESTADDR.u32);

    return;
}

void hal_set_lnk_wb_enable(unsigned int u32Enable)
{
    U_VHDCTRL VHDCTRL;
    VHDCTRL.u32 = 0x0;

    VHDCTRL.u32 = reg_read(&(pVoReg->VHDCTRL.u32));
    VHDCTRL.bits.vhd_sta_wr_en = u32Enable;
    reg_write(&(pVoReg->VHDCTRL.u32), VHDCTRL.u32);

    return;
}

void hal_set_ver_ratio(HAL_LAYER_E enLayer, unsigned int uRatio)
{
    U_VHDVSR VHDVSR;
    VHDVSR.u32 = 0x0;

    BUG_ON( enLayer != HAL_LAYER_VIDEO1 );

    if(HAL_LAYER_VIDEO1 == enLayer)
    {
        VHDVSR.u32 = reg_read(&(pVoReg->VHDVSR.u32));
        VHDVSR.bits.vratio = uRatio;
        reg_write(&(pVoReg->VHDVSR.u32), VHDVSR.u32);
    }
    else
    {
        loge("Wrong layer ID\n");
    }

    return;
}

void hal_set_hor_ratio(HAL_LAYER_E enLayer, unsigned int uRatio)
{
    U_VHDHSP VHDHSP;
    VHDHSP.u32 = 0x0;

    BUG_ON( enLayer != HAL_LAYER_VIDEO1 );

    if(HAL_LAYER_VIDEO1 == enLayer)
    {
        VHDHSP.u32 = reg_read(&(pVoReg->VHDHSP.u32));
        VHDHSP.bits.hratio = uRatio;
        reg_write(&(pVoReg->VHDHSP.u32), VHDHSP.u32);
    }
    else
    {
        loge("Wrong layer ID\n");
    }

    return;
}

void hal_set_ifir_mode(HAL_LAYER_E enLayer, HAL_IFIRMODE_E enMode)
{
    U_VHDCTRL VHDCTRL;
    VHDCTRL.u32 = 0x0;

    BUG_ON( enLayer != HAL_LAYER_VIDEO1 );

    switch(enLayer)
    {
        case HAL_LAYER_VIDEO1:
        {
            VHDCTRL.u32 = reg_read(&(pVoReg->VHDCTRL.u32));
            VHDCTRL.bits.ifir_mode = enMode;
            reg_write(&(pVoReg->VHDCTRL.u32), VHDCTRL.u32);
            break;
        }
        default:
        {
            loge("Wrong layer ID\n");
            break ;
        }
    }

    return;
}

void hal_set_ifir_coef(HAL_LAYER_E enLayer, int * s32Coef)
{
    U_VHDIFIRCOEF01 VHDIFIRCOEF01;
    U_VHDIFIRCOEF23 VHDIFIRCOEF23;
    U_VHDIFIRCOEF45 VHDIFIRCOEF45;
    U_VHDIFIRCOEF67 VHDIFIRCOEF67;
    VHDIFIRCOEF01.u32 = 0x0;
    VHDIFIRCOEF23.u32 = 0x0;
    VHDIFIRCOEF45.u32 = 0x0;
    VHDIFIRCOEF67.u32 = 0x0;

    BUG_ON( enLayer != HAL_LAYER_VIDEO1 );

    switch(enLayer)
    {
        case HAL_LAYER_VIDEO1:
        {
            VHDIFIRCOEF01.u32 = reg_read(&(pVoReg->VHDIFIRCOEF01.u32));
            VHDIFIRCOEF23.u32 = reg_read(&(pVoReg->VHDIFIRCOEF23.u32));
            VHDIFIRCOEF45.u32 = reg_read(&(pVoReg->VHDIFIRCOEF45.u32));
            VHDIFIRCOEF67.u32 = reg_read(&(pVoReg->VHDIFIRCOEF67.u32));

            VHDIFIRCOEF01.bits.coef0 = s32Coef[0];
            VHDIFIRCOEF01.bits.coef1 = s32Coef[1];
            VHDIFIRCOEF23.bits.coef2 = s32Coef[2];
            VHDIFIRCOEF23.bits.coef3 = s32Coef[3];
            VHDIFIRCOEF45.bits.coef4 = s32Coef[4];
            VHDIFIRCOEF45.bits.coef5 = s32Coef[5];
            VHDIFIRCOEF67.bits.coef6 = s32Coef[6];
            VHDIFIRCOEF67.bits.coef7 = s32Coef[7];

            reg_write(&(pVoReg->VHDIFIRCOEF01.u32), VHDIFIRCOEF01.u32);
            reg_write(&(pVoReg->VHDIFIRCOEF23.u32), VHDIFIRCOEF23.u32);
            reg_write(&(pVoReg->VHDIFIRCOEF45.u32), VHDIFIRCOEF45.u32);
            reg_write(&(pVoReg->VHDIFIRCOEF67.u32), VHDIFIRCOEF67.u32);
            break;
        }

        default:
        {
            loge("Wrong layer ID\n");
            break ;
        }
    }

    return;
}

void hal_set_rd_bus_id(unsigned int bMode)
{
    U_VOCTRL VOCTRL;
    VOCTRL.u32 = 0x0;

    //set vou read bus ID
    //0:VHD ID0, 1:VHD ID1
    VOCTRL.u32 = reg_read(&(pVoReg->VOCTRL.u32));
    VOCTRL.bits.vo_id_sel = bMode;
    reg_write(&(pVoReg->VOCTRL.u32), VOCTRL.u32);

    return;
}

void hal_set_rd_out_std(unsigned int bIdMd, unsigned char u8OsData)
{
    U_VOCTRL VOCTRL;
    VOCTRL.u32 = 0x0;

    //set vou read data outstanding
    //bidMd -- 0:VHD ID0, 1:VHD ID1
    VOCTRL.u32 = reg_read(&(pVoReg->VOCTRL.u32));
    if(bIdMd == 0)
    {
        VOCTRL.bits.outstd_rid0 = u8OsData;
    }
    else
    {
        VOCTRL.bits.outstd_rid1 = u8OsData;
    }
    reg_write(&(pVoReg->VOCTRL.u32), VOCTRL.u32);

    return;
}

void hal_set_wr_out_std(unsigned char u8OsData)
{
    U_VOCTRL VOCTRL;
    VOCTRL.u32 = 0x0;

    //set vou write back data outstanding
    VOCTRL.u32 = reg_read(&(pVoReg->VOCTRL.u32));
    VOCTRL.bits.outstd_wid0 = u8OsData;
    reg_write(&(pVoReg->VOCTRL.u32), VOCTRL.u32);

    return;
}


void hal_set_acm_enable(HAL_ACMBLK_ID_E enAcmId, unsigned int bAcmEn)
{
    U_VHDACM0 VHDACM0;

    VHDACM0.u32 = 0x0;

    //set vou vhd acm enable
    VHDACM0.u32 = reg_read(&(pVoReg->VHDACM0.u32));

    switch(enAcmId)
    {
        case HAL_ACMBLK_ID0 :
        {
            VHDACM0.bits.acm0_en = bAcmEn;
            break;
        }
        case HAL_ACMBLK_ID1 :
        {
            VHDACM0.bits.acm1_en = bAcmEn;
            break;
        }
        case HAL_ACMBLK_ID2 :
        {
            VHDACM0.bits.acm2_en = bAcmEn;
            break;
        }
        case HAL_ACMBLK_ID3 :
        {
            VHDACM0.bits.acm3_en = bAcmEn;
            break;
        }
        case HAL_ACMBLK_ALL :
        {
            VHDACM0.bits.acm0_en = bAcmEn;
            VHDACM0.bits.acm1_en = bAcmEn;
            VHDACM0.bits.acm2_en = bAcmEn;
            VHDACM0.bits.acm3_en = bAcmEn;
            break;
        }
        default:
        {
            loge(" Wrong ACM block ID!\n");
            return;
        }
    }

    reg_write(&(pVoReg->VHDACM0.u32), VHDACM0.u32);

    return;
}

void hal_set_acm_coef(HAL_ACMBLK_ID_E enAcmId, HAL_ACMBLKINFO_S stCoef)
{
    U_VHDACM0 VHDACM0;
    U_VHDACM1 VHDACM1;
    U_VHDACM2 VHDACM2;
    U_VHDACM3 VHDACM3;
    U_VHDACM4 VHDACM4;
    U_VHDACM5 VHDACM5;
    U_VHDACM6 VHDACM6;
    U_VHDACM7 VHDACM7;

    VHDACM0.u32 = 0x0;
    VHDACM1.u32 = 0x0;
    VHDACM2.u32 = 0x0;
    VHDACM3.u32 = 0x0;
    VHDACM4.u32 = 0x0;
    VHDACM5.u32 = 0x0;
    VHDACM6.u32 = 0x0;
    VHDACM7.u32 = 0x0;


    if((enAcmId == HAL_ACMBLK_ID0)||(enAcmId == HAL_ACMBLK_ALL))
    {
        VHDACM0.u32 = reg_read(&(pVoReg->VHDACM0.u32));
        VHDACM1.u32 = reg_read(&(pVoReg->VHDACM1.u32));

        VHDACM0.bits.acm_fir_blk = stCoef.u8_uIndex;
        VHDACM0.bits.acm_sec_blk = stCoef.u8_vIndex;

        VHDACM0.bits.acm_a_u_off = (stCoef.s8_uoffset_a);
        VHDACM0.bits.acm_b_u_off = (stCoef.s8_uoffset_b);
        VHDACM0.bits.acm_c_u_off = (stCoef.s8_uoffset_c);
        VHDACM0.bits.acm_d_u_off = (stCoef.s8_uoffset_d);

        VHDACM1.bits.acm_a_v_off = (stCoef.s8_voffset_a);
        VHDACM1.bits.acm_b_v_off = (stCoef.s8_voffset_b);
        VHDACM1.bits.acm_c_v_off = (stCoef.s8_voffset_c);
        VHDACM1.bits.acm_d_v_off = (stCoef.s8_voffset_d);

        reg_write(&(pVoReg->VHDACM0.u32), VHDACM0.u32);
        reg_write(&(pVoReg->VHDACM1.u32), VHDACM1.u32);
    }

    if((enAcmId == HAL_ACMBLK_ID1)||(enAcmId == HAL_ACMBLK_ALL))
    {
        VHDACM2.u32 = reg_read(&(pVoReg->VHDACM2.u32));
        VHDACM3.u32 = reg_read(&(pVoReg->VHDACM3.u32));

        VHDACM2.bits.acm_fir_blk = stCoef.u8_uIndex;
        VHDACM2.bits.acm_sec_blk = stCoef.u8_vIndex;

        VHDACM2.bits.acm_a_u_off = (stCoef.s8_uoffset_a);
        VHDACM2.bits.acm_b_u_off = (stCoef.s8_uoffset_b);
        VHDACM2.bits.acm_c_u_off = (stCoef.s8_uoffset_c);
        VHDACM2.bits.acm_d_u_off = (stCoef.s8_uoffset_d);

        VHDACM3.bits.acm_a_v_off = (stCoef.s8_voffset_a);
        VHDACM3.bits.acm_b_v_off = (stCoef.s8_voffset_b);
        VHDACM3.bits.acm_c_v_off = (stCoef.s8_voffset_c);
        VHDACM3.bits.acm_d_v_off = (stCoef.s8_voffset_d);

        reg_write(&(pVoReg->VHDACM2.u32), VHDACM2.u32);
        reg_write(&(pVoReg->VHDACM3.u32), VHDACM3.u32);
    }

    if((enAcmId == HAL_ACMBLK_ID2)||(enAcmId == HAL_ACMBLK_ALL))
    {
        VHDACM4.u32 = reg_read(&(pVoReg->VHDACM4.u32));
        VHDACM5.u32 = reg_read(&(pVoReg->VHDACM5.u32));

        VHDACM4.bits.acm_fir_blk = stCoef.u8_uIndex;
        VHDACM4.bits.acm_sec_blk = stCoef.u8_vIndex;

        VHDACM4.bits.acm_a_u_off = (stCoef.s8_uoffset_a);
        VHDACM4.bits.acm_b_u_off = (stCoef.s8_uoffset_b);
        VHDACM4.bits.acm_c_u_off = (stCoef.s8_uoffset_c);
        VHDACM4.bits.acm_d_u_off = (stCoef.s8_uoffset_d);

        VHDACM5.bits.acm_a_v_off = (stCoef.s8_voffset_a);
        VHDACM5.bits.acm_b_v_off = (stCoef.s8_voffset_b);
        VHDACM5.bits.acm_c_v_off = (stCoef.s8_voffset_c);
        VHDACM5.bits.acm_d_v_off = (stCoef.s8_voffset_d);

        reg_write(&(pVoReg->VHDACM4.u32), VHDACM4.u32);
        reg_write(&(pVoReg->VHDACM5.u32), VHDACM5.u32);
    }

    if((enAcmId == HAL_ACMBLK_ID3)||(enAcmId == HAL_ACMBLK_ALL))
    {
        VHDACM6.u32 = reg_read(&(pVoReg->VHDACM6.u32));
        VHDACM7.u32 = reg_read(&(pVoReg->VHDACM7.u32));

        VHDACM6.bits.acm_fir_blk = stCoef.u8_uIndex;
        VHDACM6.bits.acm_sec_blk = stCoef.u8_vIndex;

        VHDACM6.bits.acm_a_u_off = (stCoef.s8_uoffset_a);
        VHDACM6.bits.acm_b_u_off = (stCoef.s8_uoffset_b);
        VHDACM6.bits.acm_c_u_off = (stCoef.s8_uoffset_c);
        VHDACM6.bits.acm_d_u_off = (stCoef.s8_uoffset_d);

        VHDACM7.bits.acm_a_v_off = (stCoef.s8_voffset_a);
        VHDACM7.bits.acm_b_v_off = (stCoef.s8_voffset_b);
        VHDACM7.bits.acm_c_v_off = (stCoef.s8_voffset_c);
        VHDACM7.bits.acm_d_v_off = (stCoef.s8_voffset_d);

        reg_write(&(pVoReg->VHDACM6.u32), VHDACM6.u32);
        reg_write(&(pVoReg->VHDACM7.u32), VHDACM7.u32);
    }

    return;
}

void hal_set_acm_test_enable(unsigned int bAcmEn)
{
    U_VHDACM1 VHDACM1;
    VHDACM1.u32 = 0x0;

    //set vou vhd acm enable
    VHDACM1.u32 = reg_read(&(pVoReg->VHDACM1.u32));
    VHDACM1.bits.acm_test_en = bAcmEn;
    reg_write(&(pVoReg->VHDACM1.u32), VHDACM1.u32);

    return;
}

void hal_set_time_out(HAL_LAYER_E enLayer, unsigned char u8TData)
{
    U_VHDCTRL VHDCTRL;
    VHDCTRL.u32 = 0x0;

    BUG_ON( enLayer != HAL_LAYER_VIDEO1 );

    if(u8TData >= 16)
    {
        loge("Timeout data out of legal range\n");
        return;
    }

    switch(enLayer)
    {
        case HAL_LAYER_VIDEO1:
        {
            VHDCTRL.u32 = reg_read(&(pVoReg->VHDCTRL.u32));
            VHDCTRL.bits.time_out = u8TData;
            reg_write(&(pVoReg->VHDCTRL.u32), VHDCTRL.u32);

            break;
        }

        default:
        {
            loge("layer id error!\n");
            break ;
        }
    }


    return;
}


void hal_set_acc_thd(HAL_LAYER_E enLayer, HAL_ACCTHD_S stAccThd)
{
    U_VHDACCTHD1 VHDACCTHD1;
    U_VHDACCTHD2 VHDACCTHD2;
    VHDACCTHD1.u32 = 0x0;
    VHDACCTHD2.u32 = 0x0;

    BUG_ON( enLayer != HAL_LAYER_VIDEO1 );

    switch(enLayer)
    {
        case HAL_LAYER_VIDEO1:
        {
            VHDACCTHD1.u32 = reg_read(&(pVoReg->VHDACCTHD1.u32));
            VHDACCTHD2.u32 = reg_read(&(pVoReg->VHDACCTHD2.u32));
            VHDACCTHD2.bits.acc_multiple = stAccThd.acc_multi   ;
            VHDACCTHD2.bits.thd_med_high = stAccThd.thd_med_high;
            VHDACCTHD1.bits.thd_med_low  = stAccThd.thd_med_low ;
            VHDACCTHD1.bits.thd_high     = stAccThd.thd_high    ;
            VHDACCTHD1.bits.thd_low      = stAccThd.thd_low     ;
            reg_write(&(pVoReg->VHDACCTHD2.u32), VHDACCTHD2.u32);
            reg_write(&(pVoReg->VHDACCTHD1.u32), VHDACCTHD1.u32);

            break;
        }

        default:
        {
            loge("layer id error!\n");
            break;
        }
    }

    return;
}

void hal_set_acc_tab(HAL_LAYER_E enLayer, unsigned int *upTable)
{
    U_VHDACCLOWN  VHDACCLOW[3];
    U_VHDACCMEDN  VHDACCMED[3];
    U_VHDACCHIGHN VHDACCHIGH[3];
    U_VHDACCMLN   VHDACCML[3];
    U_VHDACCMHN   VHDACCMH[3];

    unsigned int ii = 0;

    memset(VHDACCLOW, 0, sizeof(U_VHDACCLOWN)*3);
    memset(VHDACCMED, 0, sizeof(U_VHDACCMEDN)*3);
    memset(VHDACCHIGH, 0, sizeof(U_VHDACCHIGHN)*3);
    memset(VHDACCML, 0, sizeof(U_VHDACCMLN)*3);
    memset(VHDACCMH, 0, sizeof(U_VHDACCMHN)*3);


    BUG_ON( enLayer != HAL_LAYER_VIDEO1 );

    switch(enLayer)
    {
        case HAL_LAYER_VIDEO1:
        {
            for (ii = 0; ii < 3; ii++)
            {
                VHDACCLOW[ii].bits.table_data1n = upTable[0 + ii*3];
                VHDACCLOW[ii].bits.table_data2n = upTable[1 + ii*3];
                VHDACCLOW[ii].bits.table_data3n = upTable[2 + ii*3];
                reg_write(&(pVoReg->VHDACCLOWN[ii].u32), VHDACCLOW[ii].u32);
            }

            for (ii = 0; ii < 3; ii++)
            {
                VHDACCMED[ii].bits.table_data1n = upTable[9 + 0 + ii*3];
                VHDACCMED[ii].bits.table_data2n = upTable[9 + 1 + ii*3];
                VHDACCMED[ii].bits.table_data3n = upTable[9 + 2 + ii*3];
                reg_write(&(pVoReg->VHDACCMEDN[ii].u32), VHDACCMED[ii].u32);
            }

            for (ii = 0; ii < 3; ii++)
            {
                VHDACCHIGH[ii].bits.table_data1n = upTable[18 + 0 + ii*3];
                VHDACCHIGH[ii].bits.table_data2n = upTable[18 + 1 + ii*3];
                VHDACCHIGH[ii].bits.table_data3n = upTable[18 + 2 + ii*3];
                reg_write(&(pVoReg->VHDACCHIGHN[ii].u32), VHDACCHIGH[ii].u32);
            }

            for (ii = 0; ii < 3; ii++)
            {
                VHDACCML[ii].bits.table_data1n = upTable[27 + 0 + ii*3];
                VHDACCML[ii].bits.table_data2n = upTable[27 + 1 + ii*3];
                VHDACCML[ii].bits.table_data3n = upTable[27 + 2 + ii*3];
                reg_write(&(pVoReg->VHDACCMLN[ii].u32), VHDACCML[ii].u32);
            }

            for (ii = 0; ii < 3; ii++)
            {
                VHDACCMH[ii].bits.table_data1n = upTable[36 + 0 + ii*3];
                VHDACCMH[ii].bits.table_data2n = upTable[36 + 1 + ii*3];
                VHDACCMH[ii].bits.table_data3n = upTable[36 + 2 + ii*3];
                reg_write(&(pVoReg->VHDACCMHN[ii].u32), VHDACCMH[ii].u32);
            }

            break;
        }
        default:
        {
            loge("layer id error!\n");
            break;
        }
    }

    return;
}

void hal_set_acc_ctrl(HAL_LAYER_E enLayer, unsigned int uAccEn, unsigned int uAccMode)
{
    U_VHDACCTHD1 VHDACCTHD1;
    VHDACCTHD1.u32 = 0x0;

    BUG_ON( enLayer != HAL_LAYER_VIDEO1 );

    switch(enLayer)
    {
        case HAL_LAYER_VIDEO1:
        {
            VHDACCTHD1.u32 = reg_read(&(pVoReg->VHDACCTHD1.u32));
            VHDACCTHD1.bits.acc_en   = uAccEn  ;
            VHDACCTHD1.bits.acc_mode = uAccMode;
            reg_write(&(pVoReg->VHDACCTHD1.u32), VHDACCTHD1.u32);

            break;
        }
        default:
        {
            loge("layer id error!\n");
            break;
        }
    }

    return;
}

void hal_set_acc_weight_addr(unsigned int uAccAddr)
{
    U_ACCAD ACCAD;
    ACCAD.u32 = 0x0;

    ACCAD.bits.coef_addr = uAccAddr;
    reg_write(&(pVoReg->ACCAD.u32), ACCAD.u32);

    return;
}

void hal_set_acc_rst(HAL_LAYER_E enLayer, unsigned int uAccRst)
{
    U_VHDACCTHD2 VHDACCTHD2;
    VHDACCTHD2.u32 = 0x0;

    BUG_ON( enLayer != HAL_LAYER_VIDEO1 );

    switch(enLayer)
    {
        case HAL_LAYER_VIDEO1:
        {
            VHDACCTHD2.u32 = reg_read(&(pVoReg->VHDACCTHD2.u32));
            VHDACCTHD2.bits.acc_rst   = uAccRst  ;
            reg_write(&(pVoReg->VHDACCTHD2.u32), VHDACCTHD2.u32);

            break;
        }
        default:
        {
            loge("layer id error!\n");
            break;
        }
    }

    return;
}

//设置层色彩空间转换参数
void hal_set_layer_cscCoef(HAL_LAYER_E enChan, HAL_INTFCSCCOEF_S *pstCscCoef)
{
    switch(enChan)
    {
        case HAL_LAYER_VIDEO1:
        {
            U_VHDCSCP0 VHDCSCP0;
            U_VHDCSCP1 VHDCSCP1;
            U_VHDCSCP2 VHDCSCP2;
            U_VHDCSCP3 VHDCSCP3;
            U_VHDCSCP4 VHDCSCP4;

            VHDCSCP0.u32 = 0x0;
            VHDCSCP1.u32 = 0x0;
            VHDCSCP2.u32 = 0x0;
            VHDCSCP3.u32 = 0x0;
            VHDCSCP4.u32 = 0x0;

            VHDCSCP0.u32 = reg_read(&(pVoReg->VHDCSCP0.u32));
            VHDCSCP0.bits.cscp00 = pstCscCoef->csc_coef00;
            VHDCSCP0.bits.cscp01 = pstCscCoef->csc_coef01;
            reg_write(&(pVoReg->VHDCSCP0.u32), VHDCSCP0.u32);

            VHDCSCP1.u32 = reg_read(&(pVoReg->VHDCSCP1.u32));
            VHDCSCP1.bits.cscp02 = pstCscCoef->csc_coef02;
            VHDCSCP1.bits.cscp10 = pstCscCoef->csc_coef10;
            reg_write(&(pVoReg->VHDCSCP1.u32), VHDCSCP1.u32);

            VHDCSCP2.u32 = reg_read(&(pVoReg->VHDCSCP2.u32));
            VHDCSCP2.bits.cscp11 = pstCscCoef->csc_coef11;
            VHDCSCP2.bits.cscp12 = pstCscCoef->csc_coef12;
            reg_write(&(pVoReg->VHDCSCP2.u32), VHDCSCP2.u32);

            VHDCSCP3.u32 = reg_read(&(pVoReg->VHDCSCP3.u32));
            VHDCSCP3.bits.cscp20 = pstCscCoef->csc_coef20;
            VHDCSCP3.bits.cscp21 = pstCscCoef->csc_coef21;
            reg_write(&(pVoReg->VHDCSCP3.u32), VHDCSCP3.u32);

            VHDCSCP4.u32 = reg_read(&(pVoReg->VHDCSCP4.u32));
            VHDCSCP4.bits.cscp22 = pstCscCoef->csc_coef22;
            reg_write(&(pVoReg->VHDCSCP4.u32), VHDCSCP4.u32);

            break;
        }

        default:
        {
            loge("layer id error!\n");
            break;
        }
   }

   return;
}

//设置层CSC直流分量
void hal_set_layer_cscDcCoef(HAL_LAYER_E enLayer, HAL_INTFCSCDCCOEF_S *pstCscCoef)
{
    U_VHDCSCIDC VHDCSCIDC;
    U_VHDCSCODC VHDCSCODC;

    VHDCSCIDC.u32 = 0x0;
    VHDCSCODC.u32 = 0x0;

    BUG_ON( enLayer != HAL_LAYER_VIDEO1 );

    switch(enLayer)
    {
        case HAL_LAYER_VIDEO1:
        {
            VHDCSCIDC.u32 = reg_read(&(pVoReg->VHDCSCIDC.u32));
            VHDCSCIDC.bits.cscidc2 = pstCscCoef->csc_in_dc2;
            VHDCSCIDC.bits.cscidc1 = pstCscCoef->csc_in_dc1;
            VHDCSCIDC.bits.cscidc0 = pstCscCoef->csc_in_dc0;
            reg_write(&(pVoReg->VHDCSCIDC.u32), VHDCSCIDC.u32);

            VHDCSCODC.u32 = reg_read(&(pVoReg->VHDCSCODC.u32));
            VHDCSCODC.bits.cscodc2 = pstCscCoef->csc_out_dc2;
            VHDCSCODC.bits.cscodc1 = pstCscCoef->csc_out_dc1;
            VHDCSCODC.bits.cscodc0 = pstCscCoef->csc_out_dc0;
            reg_write(&(pVoReg->VHDCSCODC.u32), VHDCSCODC.u32);
            break;
        }

        default:
        {
            loge("layer id error!\n");
            break;
        }
    }

    return;
}


/*配置层CSC使能*/
void hal_set_layer_csc(HAL_LAYER_E enLayer, unsigned int bCscEn)
{
    BUG_ON( enLayer != HAL_LAYER_VIDEO1 );

    switch (enLayer)
    {
        case HAL_LAYER_VIDEO1:
        {
            U_VHDCSCIDC VHDCSCIDC;
            VHDCSCIDC.u32 = 0x0;

            VHDCSCIDC.u32 = reg_read(&(pVoReg->VHDCSCIDC.u32));
            VHDCSCIDC.bits.csc_en = bCscEn;
            reg_write(&(pVoReg->VHDCSCIDC.u32), VHDCSCIDC.u32);
            break;
        }
        default:
        {
            loge("layer id error!\n");
            break;
        }
    }

    return;
}


//配置源窗口
void  hal_set_layer_in_rect(HAL_LAYER_E enLayer, RECT_S stRect)
{
    BUG_ON( enLayer != HAL_LAYER_VIDEO1 );

    switch (enLayer)
    {
        case HAL_LAYER_VIDEO1:
        {
            U_VHDIRESO VHDIRESO;
            VHDIRESO.u32 = 0x0;

            /* input width and height */
            VHDIRESO.u32 = reg_read(&(pVoReg->VHDIRESO.u32));
            VHDIRESO.bits.iw = stRect.s32Width- 1;
            VHDIRESO.bits.ih = stRect.s32Height- 1;
            reg_write(&(pVoReg->VHDIRESO.u32), VHDIRESO.u32);

            break;
        }
        default:
        {
            loge("layer id error!\n");
            break;
        }
    }

    return;
}


//配置输出窗口
void  hal_set_layer_out_rect(HAL_LAYER_E enLayer, RECT_S stRect)
{
    BUG_ON( enLayer != HAL_LAYER_VIDEO1 );

    switch (enLayer)
    {
        case HAL_LAYER_VIDEO1:
        {
            U_VHDVFPOS VHDVFPOS;
            U_VHDVLPOS VHDVLPOS;

            VHDVFPOS.u32 = 0x0;
            VHDVLPOS.u32 = 0x0;

            /*video position */
            VHDVFPOS.u32 = reg_read(&(pVoReg->VHDVFPOS.u32));
            VHDVFPOS.bits.video_xfpos = stRect.s32X;
            VHDVFPOS.bits.video_yfpos = stRect.s32Y;
            reg_write(&(pVoReg->VHDVFPOS.u32), VHDVFPOS.u32);

            /* output width and height */
            VHDVLPOS.u32 = reg_read(&(pVoReg->VHDVLPOS.u32));
            VHDVLPOS.bits.video_xlpos = stRect.s32X + stRect.s32Width- 1;
            VHDVLPOS.bits.video_ylpos = stRect.s32Y + stRect.s32Height - 1;
            reg_write(&(pVoReg->VHDVLPOS.u32), VHDVLPOS.u32);

            break;
        }
        default:
        {
            loge("layer id error!\n");
            break ;
        }
    }

    return;
}

//设置视频显示区域窗口
void hal_set_layer_disp_rect(HAL_LAYER_E enLayer, RECT_S stRect)
{
    BUG_ON( enLayer != HAL_LAYER_VIDEO1 );

    switch(enLayer)
    {
        case HAL_LAYER_VIDEO1:
        {
            U_VHDDFPOS VHDDFPOS;
            U_VHDDLPOS VHDDLPOS;

            VHDDFPOS.u32 = 0x0;
            VHDDLPOS.u32 = 0x0;

            /* display position */
            VHDDFPOS.u32 = reg_read(&(pVoReg->VHDDFPOS.u32));
            VHDDFPOS.bits.disp_xfpos = stRect.s32X;
            VHDDFPOS.bits.disp_yfpos = stRect.s32Y;
            reg_write(&(pVoReg->VHDDFPOS.u32), VHDDFPOS.u32);

            VHDDLPOS.u32 = reg_read(&(pVoReg->VHDDLPOS.u32));
            VHDDLPOS.bits.disp_xlpos = stRect.s32X +stRect.s32Width -1;
            VHDDLPOS.bits.disp_ylpos = stRect.s32Y + stRect.s32Height -1;
            reg_write(&(pVoReg->VHDDLPOS.u32), VHDDLPOS.u32);
            break;
        }
        default:
        {
            loge("layer id error!\n");
            break ;
        }
    }

    return;
}

/*
**************************************************************************
* FunctionName: hal_set_layer_rect
* Description : 设置层窗口(这里只能查询HAL_LAYER_VIDEO1层)
* Input       : enLayer: HAL_LAYER_VIDEO1
*             : pstRect
* Output      : NA;
* ReturnValue : NA;
* Other       : NA;
**************************************************************************
*/
void  hal_set_layer_rect(HAL_LAYER_E enLayer, HAL_RECT_S  stVideoRect)
{
    U_VHDIRESO VHDIRESO;
    U_VHDDFPOS VHDDFPOS;
    U_VHDDLPOS VHDDLPOS;
    U_VHDVFPOS VHDVFPOS;
    U_VHDVLPOS VHDVLPOS;

    VHDIRESO.u32 = 0x0;
    VHDDFPOS.u32 = 0x0;
    VHDDLPOS.u32 = 0x0;
    VHDVFPOS.u32 = 0x0;
    VHDVLPOS.u32 = 0x0;

    BUG_ON( enLayer != HAL_LAYER_VIDEO1 );

    switch(enLayer)
    {
        case HAL_LAYER_VIDEO1:
        {
            /* input width and height */
            VHDIRESO.u32 = reg_read(&(pVoReg->VHDIRESO.u32));
            VHDIRESO.bits.iw = stVideoRect.u32InWidth - 1;
            VHDIRESO.bits.ih = stVideoRect.u32InHeight - 1;
            reg_write(&(pVoReg->VHDIRESO.u32), VHDIRESO.u32);

            VHDVLPOS.u32 = reg_read(&(pVoReg->VHDVLPOS.u32));
            VHDVLPOS.bits.video_xlpos = stVideoRect.s32VX + stVideoRect.u32OutWidth - 1;
            VHDVLPOS.bits.video_ylpos = stVideoRect.s32VY + stVideoRect.u32OutHeight - 1;
            reg_write(&(pVoReg->VHDVLPOS.u32), VHDVLPOS.u32);

            /* display position */
            VHDDFPOS.u32 = reg_read(&(pVoReg->VHDDFPOS.u32));
            VHDDFPOS.bits.disp_xfpos = stVideoRect.s32DXS;
            VHDDFPOS.bits.disp_yfpos = stVideoRect.s32DYS;
            reg_write(&(pVoReg->VHDDFPOS.u32), VHDDFPOS.u32);

            VHDDLPOS.u32 = reg_read(&(pVoReg->VHDDLPOS.u32));
            VHDDLPOS.bits.disp_xlpos = stVideoRect.s32DXL-1;
            VHDDLPOS.bits.disp_ylpos = stVideoRect.s32DYL-1;
            reg_write(&(pVoReg->VHDDLPOS.u32), VHDDLPOS.u32);

            /*video position */
            VHDVFPOS.u32 = reg_read(&(pVoReg->VHDVFPOS.u32));
            VHDVFPOS.bits.video_xfpos = stVideoRect.s32VX;
            VHDVFPOS.bits.video_yfpos = stVideoRect.s32VY;
            reg_write(&(pVoReg->VHDVFPOS.u32), VHDVFPOS.u32);



            break;
        }
        default:
        {
            loge("layer id error!\n");
            break ;
        }
    }

    return;
}

void  hal_set_int_mask(unsigned int u32MaskEn)
{
    U_VOINTMSK VOINTMSK;
    VOINTMSK.u32 = 0x0;

    /* Dispaly interrupt mask enable */
    VOINTMSK.u32 = reg_read(&(pVoReg->VOINTMSK.u32));
    VOINTMSK.u32 = VOINTMSK.u32 | u32MaskEn;
    reg_write(&(pVoReg->VOINTMSK.u32), VOINTMSK.u32);

    return;
}

void  hal_dis_int_mask(unsigned int u32MaskEn)
{
    U_VOINTMSK VOINTMSK;
    VOINTMSK.u32 = 0x0;

    /* Dispaly interrupt mask enable */
    VOINTMSK.u32 = reg_read(&(pVoReg->VOINTMSK.u32));
    VOINTMSK.u32 = VOINTMSK.u32 & (~u32MaskEn);
    reg_write(&(pVoReg->VOINTMSK.u32), VOINTMSK.u32);

    return;
}

void hal_set_int_enable(unsigned int u32MaskEn)
{
    hal_set_int_mask(u32MaskEn);

    if (HAL_INTMSK_WTEINT == u32MaskEn)
    {
        //FIXME:
    }
    else if (HAL_INTMSK_VTEINT == u32MaskEn)
    {
        U_VHDCTRL VHDCTRL;
        VHDCTRL.u32 = 0x0;

        VHDCTRL.u32 = reg_read(&(pVoReg->VHDCTRL.u32));
        VHDCTRL.bits.wbc1_int_en = 1;
        reg_write(&(pVoReg->VHDCTRL.u32), VHDCTRL.u32);
    }

    return;
}

void hal_set_int_disable(unsigned int u32MaskEn)
{
    hal_dis_int_mask(u32MaskEn);

    if (HAL_INTMSK_WTEINT == u32MaskEn)
    {
        //FIXME:
    }
    else if (HAL_INTMSK_VTEINT == u32MaskEn)
    {
        U_VHDCTRL VHDCTRL;
        VHDCTRL.u32 = 0x0;

        VHDCTRL.u32 = reg_read(&(pVoReg->VHDCTRL.u32));
        VHDCTRL.bits.wbc1_int_en = 0;
        reg_write(&(pVoReg->VHDCTRL.u32), VHDCTRL.u32);
    }

    return;
}

unsigned int hal_get_int_sta(unsigned int u32IntMsk)
{
    U_VOMSKINTSTA VOMSKINTSTA;
    VOMSKINTSTA.u32 = 0x0;

    /* read interrupt status */
    VOMSKINTSTA.u32 = reg_read(&(pVoReg->VOMSKINTSTA.u32));
    logd("intstatus = %d\n", VOMSKINTSTA.u32);

    return (VOMSKINTSTA.u32 & u32IntMsk);
}

void  hal_clear_int_sta(unsigned int u32IntMsk)
{
    /* read interrupt status */
    reg_write(&(pVoReg->VOMSKINTSTA.u32), u32IntMsk);
}

unsigned int hal_get_layer_enable(HAL_LAYER_E enLayer)
{
    unsigned int u32Enable = 0;

    BUG_ON( enLayer != HAL_LAYER_VIDEO1 );

    switch (enLayer)
    {
        case HAL_LAYER_VIDEO1:
        {
            U_VHDCTRL VHDCTRL;

            VHDCTRL.u32 = reg_read(&(pVoReg->VHDCTRL.u32));
            u32Enable = VHDCTRL.bits.surface_en;
            break;
        }
        default:
        {
            loge("layer id error!\n");
            break;
        }
    }

    return u32Enable;
}

void  hal_get_layer_out_rect(HAL_LAYER_E enLayer, RECT_S *pstRect)
{
    if (NULL == pstRect){
        loge("pstRect is null!\n");
        return;
    }

    switch (enLayer)
    {
        case HAL_LAYER_VIDEO1:
        {
            U_VHDVFPOS VHDVFPOS;
            U_VHDVLPOS VHDVLPOS;
            VHDVFPOS.u32 = 0x0;
            VHDVLPOS.u32 = 0x0;

            /*video position */
            VHDVFPOS.u32 = reg_read(&(pVoReg->VHDVFPOS.u32));
            pstRect->s32X = VHDVFPOS.bits.video_xfpos;
            pstRect->s32Y = VHDVFPOS.bits.video_yfpos;

            /* output width and height */
            VHDVLPOS.u32 = reg_read(&(pVoReg->VHDVLPOS.u32));
            pstRect->s32Width  = VHDVLPOS.bits.video_xlpos + 1 - VHDVFPOS.bits.video_xfpos;
            pstRect->s32Height = VHDVLPOS.bits.video_ylpos + 1 - VHDVFPOS.bits.video_yfpos;
            break;
        }

        default:
        {
            loge("layer id error!\n");
            break ;
        }
    }

    return;
}
//add for mmu
void hal_set_mmu_ch_ptba(unsigned int chnum,unsigned int pt_baddr)
{
    U_MMU_CH_PTBA   MMU_CH_PTBA;
    MMU_CH_PTBA.u32 = 0x0;

    MMU_CH_PTBA.u32 = reg_read(&(pVoReg->MMU_CH[chnum].MMU_CH_PTBA.u32));
    MMU_CH_PTBA.u32 = pt_baddr;
    reg_write(&(pVoReg->MMU_CH[chnum].MMU_CH_PTBA.u32), MMU_CH_PTBA.u32);

    return;
}
void hal_set_mmu_ch_ptva(unsigned int chnum,unsigned int pt_vaddr)
{
    U_MMU_CH_PTVA   MMU_CH_PTVA;
    MMU_CH_PTVA.u32 = 0x0;

    MMU_CH_PTVA.u32 = reg_read(&(pVoReg->MMU_CH[chnum].MMU_CH_PTVA.u32));
    MMU_CH_PTVA.u32= pt_vaddr;
    reg_write(&(pVoReg->MMU_CH[chnum].MMU_CH_PTVA.u32), MMU_CH_PTVA.u32);

    return;
}

void hal_enb_mmu_ch_inte(unsigned int chnum,unsigned int inte){
    U_MMU_CH_INTE    MMU_CH_INTE;
    MMU_CH_INTE.u32 = 0x0;

    MMU_CH_INTE.u32 = reg_read(&(pVoReg->MMU_CH[chnum].MMU_CH_INTE.u32));
    MMU_CH_INTE.bits.axi_err_inte = inte;
    MMU_CH_INTE.bits.tlb_miss_inte = inte;
    MMU_CH_INTE.bits.tlb_vbit_err_inte = inte;
    reg_write(&(pVoReg->MMU_CH[chnum].MMU_CH_INTE.u32), MMU_CH_INTE.u32);

    return;
}

void hal_clear_mmu_ch_intc(unsigned int chnum,unsigned int intc){
    U_MMU_CH_INTC    MMU_CH_INTC;
    MMU_CH_INTC.u32 = 0x0;

    MMU_CH_INTC.u32 = reg_read(&(pVoReg->MMU_CH[chnum].MMU_CH_INTC.u32));
    MMU_CH_INTC.bits.axi_err_intc = intc;
    MMU_CH_INTC.bits.tlb_miss_intc = intc;
    MMU_CH_INTC.bits.tlb_vbit_err_intc = intc;
    reg_write(&(pVoReg->MMU_CH[chnum].MMU_CH_INTC.u32), MMU_CH_INTC.u32);

    return;
}

unsigned int hal_get_mmu_ch_ints(unsigned int chnum)
{
    U_MMU_CH_INTS    MMU_CH_INTS;
    MMU_CH_INTS.u32 = 0x0;

    MMU_CH_INTS.u32 = reg_read(&(pVoReg->MMU_CH[chnum].MMU_CH_INTS.u32));

    return MMU_CH_INTS.u32;
}

unsigned int hal_get_mmu_ints()
{
    U_MMU_TOP_INTS MMU_TOP_INTS;
    MMU_TOP_INTS.u32 = 0x0;
    MMU_TOP_INTS.u32 = reg_read(&(pVoReg->MMU_TOP_INTS.u32));

    return MMU_TOP_INTS.u32;
}

void hal_set_mmu_burst(unsigned int bmd)
{
    U_MMU_TOP_CTL MMU_TOP_CTL;
    MMU_TOP_CTL.u32 = 0x0;

    MMU_TOP_CTL.u32 = reg_read(&(pVoReg->MMU_TOP_CTL.u32));
    MMU_TOP_CTL.bits.burst_split = bmd;
    reg_write(&(pVoReg->MMU_TOP_CTL.u32), MMU_TOP_CTL.u32);

    return;
}

void hal_set_mmu_core(unsigned int comd)
{
    U_MMU_TOP_CTL MMU_TOP_CTL;
    MMU_TOP_CTL.u32 = 0x0;

    MMU_TOP_CTL.u32 = reg_read(&(pVoReg->MMU_TOP_CTL.u32));
    MMU_TOP_CTL.bits.core_gt_ctl = comd;
    reg_write(&(pVoReg->MMU_TOP_CTL.u32), MMU_TOP_CTL.u32);

    return;
}

void hal_set_mmu_cfg(unsigned int cfgmd)
{
    U_MMU_TOP_CTL MMU_TOP_CTL;
    MMU_TOP_CTL.u32 = 0x0;

    MMU_TOP_CTL.u32 = reg_read(&(pVoReg->MMU_TOP_CTL.u32));
    MMU_TOP_CTL.bits.cfg_gt_ctl = cfgmd;
    reg_write(&(pVoReg->MMU_TOP_CTL.u32), MMU_TOP_CTL.u32);

    return;
}

void hal_set_mmu_tile_num(unsigned int width, unsigned int rwdata)
{
    U_MMU_TOP_CTL MMU_TOP_CTL;
    unsigned int tile_num = 0;
    MMU_TOP_CTL.u32 = 0x0;
    if (width <= 256){
         tile_num = 0;
    }
    else if (width <=512){
         tile_num = 1;
    }
    else if (width <= 1024){
         tile_num = 2;
    }
    else if (width <= 2048){
         tile_num = 3;
    }
    else if (width <= 4096){
         tile_num = 4;
    }
    else {
         tile_num = 5;
    }

    MMU_TOP_CTL.u32 = reg_read(&(pVoReg->MMU_TOP_CTL.u32));
    if (0 == rwdata){
        MMU_TOP_CTL.bits.x_tile_num = tile_num;
    }
    else if (1 == rwdata){
           MMU_TOP_CTL.bits.w_x_tile_num = tile_num;
    }
    else {
        loge("inout read or write is write\n");
    }
    reg_write(&(pVoReg->MMU_TOP_CTL.u32), MMU_TOP_CTL.u32);

    return;
}

void hal_set_mmu_bit7_xor(unsigned int bit7xor, unsigned int rwdata)
{
    U_MMU_TOP_CTL MMU_TOP_CTL;
    MMU_TOP_CTL.u32 = 0x0;

    MMU_TOP_CTL.u32 = reg_read(&(pVoReg->MMU_TOP_CTL.u32));
    if (0 == rwdata){
        MMU_TOP_CTL.bits.bit7_xor = bit7xor;
        }
    else if (1 == rwdata){
         MMU_TOP_CTL.bits.w_bit7_xor = bit7xor;
    }
    else{
        loge("inout read or write is write!\n");
    }
    reg_write(&(pVoReg->MMU_TOP_CTL.u32), MMU_TOP_CTL.u32);

    return;
}

void hal_set_mmu_out_tile_enb(unsigned int outile)
{
    U_MMU_TOP_CTL MMU_TOP_CTL;
    MMU_TOP_CTL.u32 = 0x0;

    MMU_TOP_CTL.u32 = reg_read(&(pVoReg->MMU_TOP_CTL.u32));
    MMU_TOP_CTL.bits.tile_out = outile;
    reg_write(&(pVoReg->MMU_TOP_CTL.u32), MMU_TOP_CTL.u32);

    return;
}

void hal_set_mmu_in_tile_enb(unsigned int intile)
{
    U_MMU_TOP_CTL MMU_TOP_CTL;
    MMU_TOP_CTL.u32 = 0x0;

    MMU_TOP_CTL.u32 = reg_read(&(pVoReg->MMU_TOP_CTL.u32));
    MMU_TOP_CTL.bits.tile_in = intile;
    reg_write(&(pVoReg->MMU_TOP_CTL.u32), MMU_TOP_CTL.u32);

    return;
}

void hal_set_mmu_enable(HAL_LAYER_E enLayer,VPP_DIE_MODE_E edie_mode,VPP_PIXELFORMAT_E pixformat,unsigned int dieable,unsigned int mmu_enb)
{
    U_MMU_TOP_CTL MMU_TOP_CTL;
    MMU_TOP_CTL.u32 = 0x0;

    BUG_ON( enLayer != HAL_LAYER_VIDEO1 );

    if(HAL_LAYER_VIDEO1 == enLayer)
    {
        MMU_TOP_CTL.u32 = reg_read(&(pVoReg->MMU_TOP_CTL.u32));
        if(1 == mmu_enb){

            if(VPP_INPUTFMT_CBYCRY_PACKAGE_422 == pixformat
                || VPP_INPUTFMT_YCBYCR_PACKAGE_422 == pixformat
                || VPP_INPUTFMT_YCRYCB_PACKAGE_422 == pixformat
                || VPP_INPUTFMT_CRYCBY_PACKAGE_422 == pixformat){

                    MMU_TOP_CTL.bits.vpp_mmu_en = MMU_PACKAGE_ENB;
            }
            else if(0 == dieable){

                MMU_TOP_CTL.bits.vpp_mmu_en = MMU_ENB_DIE_DIS;
            }else{

                if(VPP_DIE_MODE_5FIELD == edie_mode)
                {
                    MMU_TOP_CTL.bits.vpp_mmu_en = MMU_INENB_OUTENB_5FIELD;
                }
                else if(VPP_DIE_MODE_3FIELD == edie_mode)
                {
                    MMU_TOP_CTL.bits.vpp_mmu_en = MMU_INENB_OUTENB_3FIELD;
                }
             }
        }else{
            MMU_TOP_CTL.bits.vpp_mmu_en = MMU_INDIS_OUTDIS;
            }
        reg_write(&(pVoReg->MMU_TOP_CTL.u32), MMU_TOP_CTL.u32);
    }
    else
    {
        loge("Wrong layer ID\n");
    }

    return;
}


void hal_set_mmu_ch_start_addr(unsigned int chnum,unsigned int startaddr)
{
    U_MMU_CH_START_ADDR   MMU_CH_START_ADDR;
    MMU_CH_START_ADDR.u32 = 0x0;

    MMU_CH_START_ADDR.u32 = reg_read(&(pVoReg->MMU_CH[chnum].MMU_CH_START_ADDR.u32));
    MMU_CH_START_ADDR.u32= startaddr;
    reg_write(&(pVoReg->MMU_CH[chnum].MMU_CH_START_ADDR.u32), MMU_CH_START_ADDR.u32);

    return;
}

void hal_set_mmu_ch_end_addr(unsigned int chnum,unsigned int endaddr)
{
    U_MMU_CH_END_ADDR   MMU_CH_END_ADDR;
    MMU_CH_END_ADDR.u32 = 0x0;

    MMU_CH_END_ADDR.u32 = reg_read(&(pVoReg->MMU_CH[chnum].MMU_CH_END_ADDR.u32));
    MMU_CH_END_ADDR.u32= endaddr;
    reg_write(&(pVoReg->MMU_CH[chnum].MMU_CH_END_ADDR.u32), MMU_CH_END_ADDR.u32);

    return;
}

void hal_set_mmu_ch_addr(VPP_CONFIG_S *pVppConfig)
{
    if (VPP_INPUTFMT_CBYCRY_PACKAGE_422 == pVppConfig->vpp_in_img.pixformat
        || VPP_INPUTFMT_YCBYCR_PACKAGE_422 == pVppConfig->vpp_in_img.pixformat
        || VPP_INPUTFMT_YCRYCB_PACKAGE_422 == pVppConfig->vpp_in_img.pixformat
        || VPP_INPUTFMT_CRYCBY_PACKAGE_422 == pVppConfig->vpp_in_img.pixformat){

        hal_set_mmu_ch_ptba(0,pVppConfig->sPTableInfo.in_mmu_ch_ptba[1]);
        hal_set_mmu_ch_ptva(0,pVppConfig->sPTableInfo.in_mmu_ch_ptva[1]);
        hal_set_mmu_ch_ptba(10,pVppConfig->sPTableInfo.out_mmu_ch_ptba);
        hal_set_mmu_ch_ptva(10,pVppConfig->sPTableInfo.out_mmu_ch_ptva);

        hal_set_mmu_ch_start_addr(0,pVppConfig->vpp_in_img.buffer_bus_addr);
        hal_set_mmu_ch_start_addr(10,pVppConfig->vpp_out_img.buffer_bus_addr);

        hal_set_mmu_ch_end_addr(0,pVppConfig->vpp_in_img.buffer_bus_addr+pVppConfig->sPTableInfo.in_size);
        hal_set_mmu_ch_end_addr(10,pVppConfig->vpp_out_img.buffer_bus_addr+pVppConfig->sPTableInfo.out_size);

    }else if(0 == pVppConfig->bdie_enbale){
        hal_set_mmu_ch_ptba(0,pVppConfig->sPTableInfo.in_mmu_ch_ptba[1]);
        hal_set_mmu_ch_ptva(0,pVppConfig->sPTableInfo.in_mmu_ch_ptva[1]);
        hal_set_mmu_ch_ptba(1,pVppConfig->sPTableInfo.in_mmu_ch_ptba[1]);
        hal_set_mmu_ch_ptva(1,pVppConfig->sPTableInfo.in_mmu_ch_ptva[1]);
        hal_set_mmu_ch_ptba(10,pVppConfig->sPTableInfo.out_mmu_ch_ptba);
        hal_set_mmu_ch_ptva(10,pVppConfig->sPTableInfo.out_mmu_ch_ptva);

        hal_set_mmu_ch_start_addr(0,pVppConfig->vpp_die_addrinfo.cluma_addr);
        hal_set_mmu_ch_start_addr(1,pVppConfig->vpp_die_addrinfo.cchroma_addr);
        hal_set_mmu_ch_start_addr(10,pVppConfig->vpp_out_img.buffer_bus_addr);
        hal_set_mmu_ch_end_addr(0,pVppConfig->vpp_in_img.buffer_bus_addr+pVppConfig->sPTableInfo.in_size);
        hal_set_mmu_ch_end_addr(1,pVppConfig->vpp_in_img.buffer_bus_addr+pVppConfig->sPTableInfo.in_size);
        hal_set_mmu_ch_end_addr(10,pVppConfig->vpp_out_img.buffer_bus_addr+pVppConfig->sPTableInfo.out_size);
    }
    else{
        if(VPP_T_FIRST == pVppConfig->edie_fod)
        {
            hal_set_mmu_ch_ptba(0,pVppConfig->sPTableInfo.in_mmu_ch_ptba[0]);
            hal_set_mmu_ch_ptva(0,pVppConfig->sPTableInfo.in_mmu_ch_ptva[0]);
            hal_set_mmu_ch_ptba(1,pVppConfig->sPTableInfo.in_mmu_ch_ptba[0]);
            hal_set_mmu_ch_ptva(1,pVppConfig->sPTableInfo.in_mmu_ch_ptva[0]);
            hal_set_mmu_ch_ptba(2,pVppConfig->sPTableInfo.in_mmu_ch_ptba[1]);
            hal_set_mmu_ch_ptva(2,pVppConfig->sPTableInfo.in_mmu_ch_ptva[1]);
            hal_set_mmu_ch_ptba(3,pVppConfig->sPTableInfo.in_mmu_ch_ptba[1]);
            hal_set_mmu_ch_ptva(3,pVppConfig->sPTableInfo.in_mmu_ch_ptva[1]);
            hal_set_mmu_ch_ptba(4,pVppConfig->sPTableInfo.in_mmu_ch_ptba[2]);
            hal_set_mmu_ch_ptva(4,pVppConfig->sPTableInfo.in_mmu_ch_ptva[2]);
            hal_set_mmu_ch_ptba(5,pVppConfig->sPTableInfo.in_mmu_ch_ptba[2]);
            hal_set_mmu_ch_ptva(5,pVppConfig->sPTableInfo.in_mmu_ch_ptva[2]);
            hal_set_mmu_ch_ptba(6,pVppConfig->sPTableInfo.in_mmu_ch_ptba[0]);
            hal_set_mmu_ch_ptva(6,pVppConfig->sPTableInfo.in_mmu_ch_ptva[0]);
            hal_set_mmu_ch_ptba(7,pVppConfig->sPTableInfo.in_mmu_ch_ptba[1]);
            hal_set_mmu_ch_ptva(7,pVppConfig->sPTableInfo.in_mmu_ch_ptva[1]);
            hal_set_mmu_ch_ptba(8,pVppConfig->sPTableInfo.in_mmu_ch_ptba[1]);
            hal_set_mmu_ch_ptva(8,pVppConfig->sPTableInfo.in_mmu_ch_ptva[1]);
            hal_set_mmu_ch_ptba(9,pVppConfig->sPTableInfo.in_mmu_ch_ptba[0]);
            hal_set_mmu_ch_ptva(9,pVppConfig->sPTableInfo.in_mmu_ch_ptva[0]);
            hal_set_mmu_ch_ptba(10,pVppConfig->sPTableInfo.out_mmu_ch_ptba);
            hal_set_mmu_ch_ptva(10,pVppConfig->sPTableInfo.out_mmu_ch_ptva);

            hal_set_mmu_ch_start_addr(0,pVppConfig->vpp_die_addrinfo.lluma_addr);
            hal_set_mmu_ch_start_addr(1,pVppConfig->vpp_die_addrinfo.lchroma_addr);
            hal_set_mmu_ch_start_addr(2,pVppConfig->vpp_die_addrinfo.cchroma_addr);
            hal_set_mmu_ch_start_addr(3,pVppConfig->vpp_die_addrinfo.cchroma_addr);
            hal_set_mmu_ch_start_addr(4,pVppConfig->vpp_die_addrinfo.nluma_addr);
            hal_set_mmu_ch_start_addr(5,pVppConfig->vpp_die_addrinfo.nchroma_addr);
            hal_set_mmu_ch_start_addr(6,pVppConfig->vpp_die_addrinfo.lluma_addr);
            hal_set_mmu_ch_start_addr(7,pVppConfig->vpp_die_addrinfo.cluma_addr);
            hal_set_mmu_ch_start_addr(8,pVppConfig->vpp_die_addrinfo.cluma_addr);
            hal_set_mmu_ch_start_addr(9,pVppConfig->vpp_die_addrinfo.lchroma_addr);
            hal_set_mmu_ch_start_addr(10,pVppConfig->vpp_out_img.buffer_bus_addr);

            hal_set_mmu_ch_end_addr(0,pVppConfig->vpp_die_addrinfo.lluma_addr+pVppConfig->sPTableInfo.in_size);
            hal_set_mmu_ch_end_addr(1,pVppConfig->vpp_die_addrinfo.lluma_addr+pVppConfig->sPTableInfo.in_size);
            hal_set_mmu_ch_end_addr(2,pVppConfig->vpp_die_addrinfo.cluma_addr+pVppConfig->sPTableInfo.in_size);
            hal_set_mmu_ch_end_addr(3,pVppConfig->vpp_die_addrinfo.cluma_addr+pVppConfig->sPTableInfo.in_size);
            hal_set_mmu_ch_end_addr(4,pVppConfig->vpp_die_addrinfo.nluma_addr+pVppConfig->sPTableInfo.in_size);
            hal_set_mmu_ch_end_addr(5,pVppConfig->vpp_die_addrinfo.nluma_addr+pVppConfig->sPTableInfo.in_size);
            hal_set_mmu_ch_end_addr(6,pVppConfig->vpp_die_addrinfo.lluma_addr+pVppConfig->sPTableInfo.in_size);
            hal_set_mmu_ch_end_addr(7,pVppConfig->vpp_die_addrinfo.cluma_addr+pVppConfig->sPTableInfo.in_size);
            hal_set_mmu_ch_end_addr(8,pVppConfig->vpp_die_addrinfo.cluma_addr+pVppConfig->sPTableInfo.in_size);
            hal_set_mmu_ch_end_addr(9,pVppConfig->vpp_die_addrinfo.lluma_addr+pVppConfig->sPTableInfo.in_size);
            hal_set_mmu_ch_end_addr(10,pVppConfig->vpp_out_img.buffer_bus_addr+pVppConfig->sPTableInfo.out_size);


        }
        else if(VPP_B_FIRST == pVppConfig->edie_fod)
        {
            hal_set_mmu_ch_ptba(0,pVppConfig->sPTableInfo.in_mmu_ch_ptba[1]);
            hal_set_mmu_ch_ptva(0,pVppConfig->sPTableInfo.in_mmu_ch_ptva[1]);
            hal_set_mmu_ch_ptba(1,pVppConfig->sPTableInfo.in_mmu_ch_ptba[1]);
            hal_set_mmu_ch_ptva(1,pVppConfig->sPTableInfo.in_mmu_ch_ptva[1]);
            hal_set_mmu_ch_ptba(2,pVppConfig->sPTableInfo.in_mmu_ch_ptba[1]);
            hal_set_mmu_ch_ptva(2,pVppConfig->sPTableInfo.in_mmu_ch_ptva[1]);
            hal_set_mmu_ch_ptba(3,pVppConfig->sPTableInfo.in_mmu_ch_ptba[2]);
            hal_set_mmu_ch_ptva(3,pVppConfig->sPTableInfo.in_mmu_ch_ptva[2]);
            hal_set_mmu_ch_ptba(4,pVppConfig->sPTableInfo.in_mmu_ch_ptba[2]);
            hal_set_mmu_ch_ptva(4,pVppConfig->sPTableInfo.in_mmu_ch_ptva[2]);
            hal_set_mmu_ch_ptba(5,pVppConfig->sPTableInfo.in_mmu_ch_ptba[2]);
            hal_set_mmu_ch_ptva(5,pVppConfig->sPTableInfo.in_mmu_ch_ptva[2]);
            hal_set_mmu_ch_ptba(6,pVppConfig->sPTableInfo.in_mmu_ch_ptba[0]);
            hal_set_mmu_ch_ptva(6,pVppConfig->sPTableInfo.in_mmu_ch_ptva[0]);
            hal_set_mmu_ch_ptba(7,pVppConfig->sPTableInfo.in_mmu_ch_ptba[1]);
            hal_set_mmu_ch_ptva(7,pVppConfig->sPTableInfo.in_mmu_ch_ptva[1]);
            hal_set_mmu_ch_ptba(8,pVppConfig->sPTableInfo.in_mmu_ch_ptba[2]);
            hal_set_mmu_ch_ptva(8,pVppConfig->sPTableInfo.in_mmu_ch_ptva[2]);
            hal_set_mmu_ch_ptba(9,pVppConfig->sPTableInfo.in_mmu_ch_ptba[0]);
            hal_set_mmu_ch_ptva(9,pVppConfig->sPTableInfo.in_mmu_ch_ptva[0]);
            hal_set_mmu_ch_ptba(10,pVppConfig->sPTableInfo.out_mmu_ch_ptba);
            hal_set_mmu_ch_ptva(10,pVppConfig->sPTableInfo.out_mmu_ch_ptva);

            hal_set_mmu_ch_start_addr(0,pVppConfig->vpp_die_addrinfo.cluma_addr);
            hal_set_mmu_ch_start_addr(1,pVppConfig->vpp_die_addrinfo.cchroma_addr);
            hal_set_mmu_ch_start_addr(2,pVppConfig->vpp_die_addrinfo.cchroma_addr);
            hal_set_mmu_ch_start_addr(3,pVppConfig->vpp_die_addrinfo.nchroma_addr);
            hal_set_mmu_ch_start_addr(4,pVppConfig->vpp_die_addrinfo.nluma_addr);
            hal_set_mmu_ch_start_addr(5,pVppConfig->vpp_die_addrinfo.nchroma_addr);
            hal_set_mmu_ch_start_addr(6,pVppConfig->vpp_die_addrinfo.lluma_addr);
            hal_set_mmu_ch_start_addr(7,pVppConfig->vpp_die_addrinfo.cluma_addr);
            hal_set_mmu_ch_start_addr(8,pVppConfig->vpp_die_addrinfo.nluma_addr);
            hal_set_mmu_ch_start_addr(9,pVppConfig->vpp_die_addrinfo.lchroma_addr);
            hal_set_mmu_ch_start_addr(10,pVppConfig->vpp_out_img.buffer_bus_addr);

            hal_set_mmu_ch_end_addr(0,pVppConfig->vpp_die_addrinfo.cluma_addr+pVppConfig->sPTableInfo.in_size);
            hal_set_mmu_ch_end_addr(1,pVppConfig->vpp_die_addrinfo.cluma_addr+pVppConfig->sPTableInfo.in_size);
            hal_set_mmu_ch_end_addr(2,pVppConfig->vpp_die_addrinfo.cluma_addr+pVppConfig->sPTableInfo.in_size);
            hal_set_mmu_ch_end_addr(3,pVppConfig->vpp_die_addrinfo.nluma_addr+pVppConfig->sPTableInfo.in_size);
            hal_set_mmu_ch_end_addr(4,pVppConfig->vpp_die_addrinfo.nluma_addr+pVppConfig->sPTableInfo.in_size);
            hal_set_mmu_ch_end_addr(5,pVppConfig->vpp_die_addrinfo.nluma_addr+pVppConfig->sPTableInfo.in_size);
            hal_set_mmu_ch_end_addr(6,pVppConfig->vpp_die_addrinfo.lluma_addr+pVppConfig->sPTableInfo.in_size);
            hal_set_mmu_ch_end_addr(7,pVppConfig->vpp_die_addrinfo.cluma_addr+pVppConfig->sPTableInfo.in_size);
            hal_set_mmu_ch_end_addr(8,pVppConfig->vpp_die_addrinfo.nluma_addr+pVppConfig->sPTableInfo.in_size);
            hal_set_mmu_ch_end_addr(9,pVppConfig->vpp_die_addrinfo.lluma_addr+pVppConfig->sPTableInfo.in_size);
            hal_set_mmu_ch_end_addr(10,pVppConfig->vpp_out_img.buffer_bus_addr+pVppConfig->sPTableInfo.out_size);
        }

        else
        {
            loge("wrong edie_fod!\n");
        }
    }
    return;
}


void hal_set_mmu__start(unsigned int flag){

    U_MMU_TOP_CTL   MMU_TOP_CTL;
    MMU_TOP_CTL.u32 = 0x0;
    MMU_TOP_CTL.u32 = reg_read(&(pVoReg->MMU_TOP_CTL.u32));
    MMU_TOP_CTL.bits.frame_start = flag;
    reg_write(&(pVoReg->MMU_TOP_CTL.u32), MMU_TOP_CTL.u32);

    return;
}

