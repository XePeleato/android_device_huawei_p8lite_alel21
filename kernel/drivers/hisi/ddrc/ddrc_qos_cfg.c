

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include <linux/kernel.h>
#include <linux/io.h>
#include "soc_baseaddr_interface.h"
#include "soc_mddrc_axi_interface.h"
#include "ddrc_qos_cfg.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  2 全局变量定义
*****************************************************************************/


/*****************************************************************************
  3 函数实现
*****************************************************************************/

/*****************************************************************************
 函 数 名  : ddrc_set_port_pri
 功能描述  : 端口读写优先级配置
 输入参数  : ddrc_port_id port,     端口号
             unsigned int id_map,   优先级配置master id map bit选择
             ddrc_qos_pri_level read_lvl,   读优先级
             ddrc_qos_pri_level write_lvl,  写优先级
 输出参数  : 无
 返 回 值  : DDRC_QOS_CFG_OK 配置成功
             DDRC_QOS_CFG_ERROR 参数错误
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年6月12日
    作    者   : 苏庄銮 59026
    修改内容   : 新生成函数

*****************************************************************************/
int ddrc_set_port_pri(
                ddrc_port_id port,
                unsigned int id_map,
                unsigned int read_lvl,
                unsigned int write_lvl)
{
    SOC_MDDRC_AXI_AXI_QOSCFG0_UNION *qos_cfg0;
    SOC_MDDRC_AXI_AXI_WRPRI_UNION   *write_pri;
    SOC_MDDRC_AXI_AXI_RDPRI_UNION   *read_pri;

    if ( (port >= DDRC_PORT_BUTT) || (id_map >= 0xfff) ){
        ddrc_printk("invalid input.\n");
        return DDRC_QOS_CFG_ERROR;
    }

    qos_cfg0 = DDRC_REG_MAP(SOC_MDDRC_AXI_AXI_QOSCFG0_ADDR(SOC_MDDRC_AXI_BASE_ADDR, port));
    read_pri = DDRC_REG_MAP(SOC_MDDRC_AXI_AXI_RDPRI_ADDR(SOC_MDDRC_AXI_BASE_ADDR, port));
    write_pri = DDRC_REG_MAP(SOC_MDDRC_AXI_AXI_WRPRI_ADDR(SOC_MDDRC_AXI_BASE_ADDR, port));

    qos_cfg0->reg.id_map = id_map;
    read_pri->value      = read_lvl;
    write_pri->value     = write_lvl;

    DDRC_REG_UNMAP(qos_cfg0);
    DDRC_REG_UNMAP(read_pri);
    DDRC_REG_UNMAP(write_pri);

    return DDRC_QOS_CFG_OK;

}



/*****************************************************************************
 函 数 名  : ddrc_set_pri_deliver
 功能描述  : 端口优先级传递使能
 输入参数  : ddrc_port_id port,         端口号
             ddrc_qos_enable enable，   使能或者禁止
 输出参数  : 无
 返 回 值  : DDRC_QOS_CFG_ERROR 参数错误
             DDRC_QOS_CFG_OK    配置成功
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年6月12日
    作    者   : 苏庄銮 59026
    修改内容   : 新生成函数

*****************************************************************************/
int ddrc_set_pri_deliver(ddrc_port_id port, ddrc_qos_enable enable)
{
    SOC_MDDRC_AXI_AXI_QOSCFG0_UNION *qos_cfg0;

    if (port >= DDRC_PORT_BUTT ){
        ddrc_printk("invalid input.\n");
        return DDRC_QOS_CFG_ERROR;
    }

    qos_cfg0 = DDRC_REG_MAP(SOC_MDDRC_AXI_AXI_QOSCFG0_ADDR(SOC_MDDRC_AXI_BASE_ADDR, port));

    if (DDRC_QOS_DISABLE == enable) {
        qos_cfg0->reg.pri_deliver_en = 0;
    } else {
        qos_cfg0->reg.pri_deliver_en = 1;
    }

    DDRC_REG_UNMAP(qos_cfg0);
    return DDRC_QOS_CFG_OK;

}


/*****************************************************************************
 函 数 名  : ddrc_set_rd_pri_apt
 功能描述  : 端口读优先级自适应配置
 输入参数  : ddrc_port_id port,     端口号
             unsigned int clk_num,  优先级调整周期(0-15),
                                    0表示禁止自适应, 否则clk_num*16个时钟周期
 输出参数  : 无
 返 回 值  : DDRC_QOS_CFG_ERROR 参数错误
             DDRC_QOS_CFG_OK    配置成功
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年6月12日
    作    者   : 苏庄銮 59026
    修改内容   : 新生成函数

*****************************************************************************/
int ddrc_set_rd_pri_apt(ddrc_port_id port, unsigned int clk_num)
{
    SOC_MDDRC_AXI_AXI_QOSCFG1_UNION *qos_cfg1;

    if ( (port >= DDRC_PORT_BUTT) || (clk_num > 0xf)){
        ddrc_printk("invalid input.\n");
        return DDRC_QOS_CFG_ERROR;
    }

    qos_cfg1 = DDRC_REG_MAP(SOC_MDDRC_AXI_AXI_QOSCFG1_ADDR(SOC_MDDRC_AXI_BASE_ADDR, port));
    qos_cfg1->reg.rd_pri_apt = clk_num;
    DDRC_REG_UNMAP(qos_cfg1);

    return DDRC_QOS_CFG_OK;

}

/*****************************************************************************
 函 数 名  : ddrc_set_wr_pri_apt
 功能描述  : 端口写优先级自适应配置
 输入参数  : ddrc_port_id port,     端口号
             unsigned int clk_num,  优先级调整周期(0-15),
                                    0表示禁止自适应, 否则clk_num*16个时钟周期
 输出参数  : 无
 返 回 值  : DDRC_QOS_CFG_ERROR 参数错误
             DDRC_QOS_CFG_OK    配置成功
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年6月12日
    作    者   : 苏庄銮 59026
    修改内容   : 新生成函数

*****************************************************************************/
int ddrc_set_wr_pri_apt(ddrc_port_id port, unsigned int clk_num)
{
    SOC_MDDRC_AXI_AXI_QOSCFG1_UNION *qos_cfg1;

    if ( (port >= DDRC_PORT_BUTT) || (clk_num > 0xf)){
        ddrc_printk("invalid input.\n");
        return DDRC_QOS_CFG_ERROR;
    }

    qos_cfg1 = DDRC_REG_MAP(SOC_MDDRC_AXI_AXI_QOSCFG1_ADDR(SOC_MDDRC_AXI_BASE_ADDR, port));
    qos_cfg1->reg.wr_pri_apt = clk_num;
    DDRC_REG_UNMAP(qos_cfg1);

    return DDRC_QOS_CFG_OK;
}

/*****************************************************************************
 函 数 名  : ddrc_set_rd_age_prd
 功能描述  : 端口读命令老化配置
 输入参数  : ddrc_port_id port,     端口号
             unsigned int clk_num,  优先级调整周期(0-15),
                                    0表示禁止老化, 否则老化时间为clk_num*16个时钟周期
 输出参数  : 无
 返 回 值  : DDRC_QOS_CFG_ERROR 参数错误
             DDRC_QOS_CFG_OK    配置成功
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年6月12日
    作    者   : 苏庄銮 59026
    修改内容   : 新生成函数

*****************************************************************************/
int ddrc_set_rd_age_prd(ddrc_port_id port, unsigned int clk_num)
{
    SOC_MDDRC_AXI_AXI_QOSCFG1_UNION *qos_cfg1;

    if ( (port >= DDRC_PORT_BUTT) || (clk_num > 0xf)){
        ddrc_printk("invalid input.\n");
        return DDRC_QOS_CFG_ERROR;
    }

    qos_cfg1 = DDRC_REG_MAP(SOC_MDDRC_AXI_AXI_QOSCFG1_ADDR(SOC_MDDRC_AXI_BASE_ADDR, port));
    qos_cfg1->reg.rd_age_prd = clk_num;
    DDRC_REG_UNMAP(qos_cfg1);

    return DDRC_QOS_CFG_OK;
}

/*****************************************************************************
 函 数 名  : ddrc_set_wr_age_prd
 功能描述  : 端口写命令老化配置
 输入参数  : ddrc_port_id port,     端口号
             unsigned int clk_num,  优先级调整周期(0-15),
                                    0表示禁止老化, 否则老化时间为clk_num*16个时钟周期
 输出参数  : 无
 返 回 值  : DDRC_QOS_CFG_ERROR 参数错误
             DDRC_QOS_CFG_OK    配置成功
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年6月12日
    作    者   : 苏庄銮 59026
    修改内容   : 新生成函数

*****************************************************************************/
int ddrc_set_wr_age_prd(ddrc_port_id port, unsigned int clk_num)
{
    SOC_MDDRC_AXI_AXI_QOSCFG1_UNION *qos_cfg1;

    if ( (port >= DDRC_PORT_BUTT) || (clk_num > 0xf)){
        ddrc_printk("invalid input.\n");
        return DDRC_QOS_CFG_ERROR;
    }

    qos_cfg1 = DDRC_REG_MAP(SOC_MDDRC_AXI_AXI_QOSCFG1_ADDR(SOC_MDDRC_AXI_BASE_ADDR, port));
    qos_cfg1->reg.wr_age_prd = clk_num;
    DDRC_REG_UNMAP(qos_cfg1);

    return DDRC_QOS_CFG_OK;
}

/*****************************************************************************
 函 数 名  : ddrc_set_wr_qos_tout
 功能描述  : 设置写命令超时
 输入参数  : ddrc_port_id port,     端口
             ddrc_qos_enable enable,超时使能
             unsigned int qosl_tout,低优先级超时时间, 1~0x3ff个时钟周期
             unsigned int qosh_tout,高优先级超时时间，同上
 输出参数  : 无
 返 回 值  : DDRC_QOS_CFG_ERROR 参数错误
             DDRC_QOS_CFG_OK    配置成功
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年6月12日
    作    者   : 苏庄銮 59026
    修改内容   : 新生成函数

*****************************************************************************/
int ddrc_set_wr_qos_tout(
                ddrc_port_id port,
                ddrc_qos_enable enable,
                unsigned int qosl_tout,
                unsigned int qosh_tout)
{
    SOC_MDDRC_AXI_AXI_WRQOS_UNION *qos_timeout;

    if ( (port >= DDRC_PORT_BUTT) || (qosl_tout < 1) || (qosl_tout > 0x3ff)){
        ddrc_printk("invalid input.\n");
        return DDRC_QOS_CFG_ERROR;
    }

    qos_timeout = DDRC_REG_MAP(SOC_MDDRC_AXI_AXI_WRQOS_ADDR(SOC_MDDRC_AXI_BASE_ADDR, port));

    if (DDRC_QOS_DISABLE == enable){
        qos_timeout->reg.wr_qos_en = 0;
    } else {
        qos_timeout->reg.wr_qosl_tout = qosl_tout;
        qos_timeout->reg.wr_qosh_tout = qosh_tout;
        qos_timeout->reg.wr_qos_en = 1;
    }

    DDRC_REG_UNMAP(qos_timeout);

    return DDRC_QOS_CFG_OK;
}

/*****************************************************************************
 函 数 名  : ddrc_set_wr_qos_tout
 功能描述  : 设置写命令超时
 输入参数  : ddrc_port_id port,     端口
             ddrc_qos_enable enable,超时使能
             unsigned int qosl_tout,低优先级超时时间, 1~0x3ff个时钟周期
             unsigned int qosh_tout,高优先级超时时间，同上
 输出参数  : 无
 返 回 值  : DDRC_QOS_CFG_ERROR 参数错误
             DDRC_QOS_CFG_OK    配置成功
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年6月12日
    作    者   : 苏庄銮 59026
    修改内容   : 新生成函数

*****************************************************************************/
int ddrc_set_rd_qos_tout(
                ddrc_port_id port,
                ddrc_qos_enable enable,
                unsigned int qosl_tout,
                unsigned int qosh_tout)
{
    SOC_MDDRC_AXI_AXI_RDQOS_UNION *qos_timeout;

    if ( (port >= DDRC_PORT_BUTT) || (qosl_tout < 1) || (qosl_tout > 0x3ff)){
        ddrc_printk("invalid input.\n");
        return DDRC_QOS_CFG_ERROR;
    }

    qos_timeout = DDRC_REG_MAP(SOC_MDDRC_AXI_AXI_RDQOS_ADDR(SOC_MDDRC_AXI_BASE_ADDR, port));

    if (DDRC_QOS_DISABLE == enable){
        qos_timeout->reg.rd_qos_en = 0;
    } else {
        qos_timeout->reg.rd_qosl_tout = qosl_tout;
        qos_timeout->reg.rd_qosh_tout = qosh_tout;
        qos_timeout->reg.rd_qos_en = 1;
    }

    DDRC_REG_UNMAP(qos_timeout);

    return DDRC_QOS_CFG_OK;
}

/*****************************************************************************
 函 数 名  : ddrc_set_flux_cfg
 功能描述  : 端口流量控制配置
 输入参数  : ddrc_port_id port,         端口号
             ddrc_qos_enable port_en,   流量控制使能
             unsigned int port_cfg,     端口允许最大带宽(0~0x3ff)
             ddrc_qos_enable ovfl_en,   流量溢出使能
             unsigned int ovfl_lvl,     溢出允许水线
 输出参数  : 无
 返 回 值  : DDRC_QOS_CFG_ERROR 参数错误
             DDRC_QOS_CFG_OK    配置成功
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年6月12日
    作    者   : 苏庄銮 59026
    修改内容   : 新生成函数

*****************************************************************************/
int ddrc_set_flux_cfg(
                ddrc_port_id port,
                ddrc_qos_enable port_en,
                unsigned int port_cfg,
                ddrc_qos_enable ovfl_en,
                unsigned int ovfl_lvl)
{
    SOC_MDDRC_AXI_AXI_FLUX0_UNION *flux0;

    if ( (port >= DDRC_PORT_BUTT) || (port_cfg > 0x3ff) || (ovfl_lvl > 0x10)){
        ddrc_printk("invalid input.\n");
        return DDRC_QOS_CFG_ERROR;
    }

    flux0 = DDRC_REG_MAP(SOC_MDDRC_AXI_AXI_FLUX0_ADDR(SOC_MDDRC_AXI_BASE_ADDR, port));

    if (DDRC_QOS_DISABLE == port_en){
        flux0->reg.flux_port_en0 = 0;
    } else {

        if (DDRC_QOS_DISABLE == ovfl_en) {
            flux0->reg.flux_ovfl_en0 = 0;
        } else {
            flux0->reg.flux_ovfl_lvl0 = ovfl_lvl;
            flux0->reg.flux_ovfl_en0 = 1;
        }

        flux0->reg.flux_cfg0 = port_cfg;
        flux0->reg.flux_port_en0 = 1;
    }

    DDRC_REG_UNMAP(flux0);

    return DDRC_QOS_CFG_OK;
}

/*****************************************************************************
 函 数 名  : ddrc_set_ostd_mode
 功能描述  : 端口绿色通道模式设置
 输入参数  : ddrc_port_id port,         端口号
             ddrc_qos_ostd_mode mode,   模式
 输出参数  : 无
 返 回 值  : DDRC_QOS_CFG_ERROR 参数错误
             DDRC_QOS_CFG_OK    配置成功
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年6月12日
    作    者   : 苏庄銮 59026
    修改内容   : 新生成函数

*****************************************************************************/
int ddrc_set_ostd_mode(ddrc_port_id port, ddrc_qos_ostd_mode mode)
{
    SOC_MDDRC_AXI_AXI_OSTDCFG2_UNION *ostdcfg2;
    unsigned int reg_value;

    if ( (port >= DDRC_PORT_BUTT) || (mode > DDRC_OSTD_MODE_BUTT) ){
        ddrc_printk("invalid input.\n");
        return DDRC_QOS_CFG_ERROR;
    }

    ostdcfg2 = DDRC_REG_MAP(SOC_MDDRC_AXI_AXI_OSTDCFG2_ADDR(SOC_MDDRC_AXI_BASE_ADDR));

    reg_value = readl(ostdcfg2);
    reg_value = reg_value & ~((unsigned int) 0x3 << port*2);
    reg_value = reg_value |  ((unsigned int)mode << port*2);
    writel(reg_value, ostdcfg2);

    DDRC_REG_UNMAP(ostdcfg2);

    return DDRC_QOS_CFG_OK;
}

/*****************************************************************************
 函 数 名  : ddrc_set_ostd_lvl
 功能描述  : 端口ostd水线设置
 输入参数  : ddrc_port_id port,             端口号
             unsigned int  port_ostd_lvl,   水线(0~12)
 输出参数  : 无
 返 回 值  : DDRC_QOS_CFG_ERROR 参数错误
             DDRC_QOS_CFG_OK    配置成功
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年6月12日
    作    者   : 苏庄銮 59026
    修改内容   : 新生成函数

*****************************************************************************/
int ddrc_set_ostd_lvl(ddrc_port_id port, unsigned int  port_ostd_lvl)
{
    SOC_MDDRC_AXI_AXI_OSTDCFG0_UNION *ostdcfg0;

    if ( (port >= DDRC_PORT_BUTT) || (port_ostd_lvl > 12)){
        ddrc_printk("invalid input.\n");
        return DDRC_QOS_CFG_ERROR;
    }

    ostdcfg0 = DDRC_REG_MAP(SOC_MDDRC_AXI_AXI_OSTDCFG0_ADDR(SOC_MDDRC_AXI_BASE_ADDR, port));

    ostdcfg0->reg.port_ostd_lvl0 = port_ostd_lvl;

    DDRC_REG_UNMAP(ostdcfg0);

    return DDRC_QOS_CFG_OK;
}

/*****************************************************************************
 函 数 名  : ddrc_set_dmc_ostd_lvl
 功能描述  : dmc ostd水线设置
 输入参数  : unsigned int dmc_ostd_lvl, dmc水线
 输出参数  : 无
 返 回 值  : DDRC_QOS_CFG_ERROR 参数错误
             DDRC_QOS_CFG_OK    配置成功
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年6月12日
    作    者   : 苏庄銮 59026
    修改内容   : 新生成函数

*****************************************************************************/
int ddrc_set_dmc_ostd_lvl(unsigned int dmc_ostd_lvl)
{
    SOC_MDDRC_AXI_AXI_OSTDCFG1_UNION *ostdcfg1;

    if (dmc_ostd_lvl > 12){
        ddrc_printk("invalid input.\n");
        return DDRC_QOS_CFG_ERROR;
    }

    ostdcfg1 = DDRC_REG_MAP(SOC_MDDRC_AXI_AXI_OSTDCFG1_ADDR(SOC_MDDRC_AXI_BASE_ADDR));
    ostdcfg1->reg.dmc_ostd_lvl0 = dmc_ostd_lvl;
    DDRC_REG_UNMAP(ostdcfg1);

    return DDRC_QOS_CFG_OK;
}

/*****************************************************************************
 函 数 名  : ddrc_qos_init
 功能描述  : ddr qos 初始化配置
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年6月12日
    作    者   : 苏庄銮 59026
    修改内容   : 新生成函数

*****************************************************************************/
void ddrc_qos_init(void)
{
    ddrc_set_port_pri(DDRC_PORT_AXI0, 0x210, 0x00000000, 0x00000000);
    ddrc_set_port_pri(DDRC_PORT_AXI1, 0x3a9, 0x11111111, 0x11111111);
    ddrc_set_port_pri(DDRC_PORT_AXI2, 0xedc, 0x22222222, 0x22222222);
    ddrc_set_port_pri(DDRC_PORT_AXI3, 0x3dc, 0x33333333, 0x33333333);
    ddrc_set_port_pri(DDRC_PORT_AXI4, 0x210, 0x44444444, 0x44444444);
    ddrc_set_port_pri(DDRC_PORT_AXI5, 0x210, 0x44444444, 0x44444444);
    ddrc_set_port_pri(DDRC_PORT_AXI6, 0xfed, 0x55555555, 0x55555555);
    ddrc_set_port_pri(DDRC_PORT_AXI7, 0x210, 0x66666666, 0x66666666);
    ddrc_set_port_pri(DDRC_PORT_AXI8, 0x210, 0x77777777, 0x77777777);
}

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif



