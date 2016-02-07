

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/


#ifndef __DDRC_QOS_CFG_H__
#define __DDRC_QOS_CFG_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif



/*****************************************************************************
  2 宏定义
*****************************************************************************/
#define ddrc_printk(format, arg...)     printk(format, ## arg)

#define DDRC_REG_MAP(phyAddr)           ioremap(phyAddr, sizeof(unsigned long))
#define DDRC_REG_UNMAP(virAddr)         iounmap(virAddr)

/*****************************************************************************
  3 枚举定义
*****************************************************************************/

/* ddrc axi port  */
typedef enum {
    DDRC_PORT_AXI0,
    DDRC_PORT_AXI1,
    DDRC_PORT_AXI2,
    DDRC_PORT_AXI3,
    DDRC_PORT_AXI4,
    DDRC_PORT_AXI5,
    DDRC_PORT_AXI6,
    DDRC_PORT_AXI7,
    DDRC_PORT_AXI8,
    DDRC_PORT_BUTT
} ddrc_port_id;


/* qos enbale */
typedef enum {
    DDRC_QOS_DISABLE,
    DDRC_QOS_ENABLE,
} ddrc_qos_enable;

/* qos cfg return value */
typedef enum {
    DDRC_QOS_CFG_OK,
    DDRC_QOS_CFG_ERROR
} ddrc_qos_cfg_ret;

/* port ostd_mode */
typedef enum {
    DDRC_OSTD_MODE_DISABLE,
    DDRC_OSTD_MODE_GREEN,
    DDRC_OSTD_MODE_NOGREEN1,
    DDRC_OSTD_MODE_NOGREEN2,
    DDRC_OSTD_MODE_BUTT
} ddrc_qos_ostd_mode;


/*****************************************************************************
  4 消息头定义
*****************************************************************************/


/*****************************************************************************
  5 消息定义
*****************************************************************************/


/*****************************************************************************
  6 STRUCT定义
*****************************************************************************/


/*****************************************************************************
  7 UNION定义
*****************************************************************************/


/*****************************************************************************
  8 OTHERS定义
*****************************************************************************/


/*****************************************************************************
  9 全局变量声明
*****************************************************************************/


/*****************************************************************************
  10 函数声明
*****************************************************************************/

extern void ddrc_qos_init(void);
extern int ddrc_set_dmc_ostd_lvl(unsigned int dmc_ostd_lvl);
extern int ddrc_set_flux_cfg(
                       ddrc_port_id port,
                       ddrc_qos_enable port_en,
                       unsigned int port_cfg,
                       ddrc_qos_enable ovfl_en,
                       unsigned int ovfl_lvl);
extern int ddrc_set_ostd_lvl(ddrc_port_id port, unsigned int  port_ostd_lvl);
extern int ddrc_set_ostd_mode(ddrc_port_id port, ddrc_qos_ostd_mode mode);
extern int ddrc_set_port_pri(
                       ddrc_port_id port,
                       unsigned int id_map,
                       unsigned int read_lvl,
                       unsigned int write_lvl);
extern int ddrc_set_pri_deliver(ddrc_port_id port, ddrc_qos_enable enable);
extern int ddrc_set_rd_age_prd(ddrc_port_id port, unsigned int clk_num);
extern int ddrc_set_rd_pri_apt(ddrc_port_id port, unsigned int clk_num);
extern int ddrc_set_rd_qos_tout(
                       ddrc_port_id port,
                       ddrc_qos_enable enable,
                       unsigned int qosl_tout,
                       unsigned int qosh_tout);
extern int ddrc_set_wr_age_prd(ddrc_port_id port, unsigned int clk_num);
extern int ddrc_set_wr_pri_apt(ddrc_port_id port, unsigned int clk_num);
extern int ddrc_set_wr_qos_tout(
                       ddrc_port_id port,
                       ddrc_qos_enable enable,
                       unsigned int qosl_tout,
                       unsigned int qosh_tout);


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of ddrc_qos_cfg.h */
