#ifndef __HISI_ERR_PROBE
#define __HISI_ERR_PROBE

/* register offset*/
#define ERR_PORBE_ID_COREID_OFFSET		0x0
#define ERR_PORBE_ID_REVISIONID_OFFSET		0x4
#define ERR_PORBE_FAULTEN_OFFSET		0x8
#define ERR_PORBE_ERRVLD_OFFSET			0xC
#define ERR_PORBE_ERRCLR_OFFSET			0x10
#define ERR_PORBE_ERRLOG0_OFFSET		0x14
#define ERR_PORBE_ERRLOG1_OFFSET		0x18
#define ERR_PORBE_ERRLOG3_OFFSET		0x20
#define ERR_PORBE_ERRLOG4_OFFSET		0x24
#define ERR_PORBE_ERRLOG5_OFFSET		0x28
#define ERR_PORBE_ERRLOG7_OFFSET		0x30
#define ERR_PROBE_CLEAR_BIT	  BIT(0)
#define ERR_PORBE_ENABLE_BIT	  BIT(0)
#define ERR_PORBE_ERRVLD_BIT	  BIT(0)

/* mask bits used by k3*/
#define ERR_LOG0_ERRCODE_SHIFT	(8)
#define ERR_LOG0_OPC_SHIFT	(1)
#define ERR_LOG0_ERRCODE_MASK	((0x7) << ERR_LOG0_ERRCODE_SHIFT)
#define ERR_LOG0_OPC_MASK	((0xf) << ERR_LOG0_OPC_SHIFT)

/*used by v8*/
#define SOC_MEDIA_NOC_MEDIA_BUS_ERR_LOGGER_FAULTEN_ADDR(base)      ((base) + (0x0008))
#define SOC_SYSNOC_ERR_LOGGER_FAULTEN_ADDR(base)      ((base) + (0x0008))
#define SOC_ADE_S_BASE_ADDR    (0xF4100000)
#define HIMNTN_NOC_ERR_LOGGER_RESET 20
#define BSP_MODU_MNTN 0x25 /*if the value change, please also change BSP_MODU_MNTN of BSP.h*/ 
#define EXCH_S_NOC 0x2 /*if the value change, please also change EXCH_S_NOC of excDrv.h*/ 
/*global variables define here, starts*/
typedef enum
{
    NOC_INIT_NONE,
    NOC_INIT_MODEM,
    NOC_INIT_HIFI,
    NOC_INIT_MEDIA,
    NOC_INIT_MAX
} enoc_init_type;
extern struct notifier_block noc_on_sys_panic_nb ;
extern struct notifier_block noc_on_media_panic_nb;

/* declare functions */
void enable_err_probe(void __iomem *base);
void disable_err_probe(void __iomem *base);
void enable_err_probe_by_name(const char *name);
void disable_err_probe_by_name(char *name);

/*used by k3*/
void hi3xxx_noc_err_probe_hanlder(void __iomem *base, unsigned int idx);

/*used by v8*/
void hi6220_noc_err_probe_hanlder(void __iomem *base);
void dump_err_sys_logger(void);
void dump_err_media_logger(void);
void copy_err_sys_logger(void * uaddr,size_t len);
void copy_err_media_logger(void * uaddr,size_t len);
void call_noc_onoff(int ioper, unsigned int ionoff);
void ade_reg_addr_init(void);
void ade_reg_addr_exit(void);
void check_doreset_for_noc(void);
#ifdef CONFIG_ARCH_HI6XXX
void modem_reg_addr_init(void);
#endif
#endif
