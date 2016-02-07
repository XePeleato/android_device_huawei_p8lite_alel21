#ifndef __HISI_NOC_H
#define __HISI_NOC_H

#define MODULE_NAME	"HISI_NOC"
#define MAX_NOC_NODE_NAME_LEN	20
#define GET_NODE_FROM_ARRAY                 \
if (HISI_NOC_HI3XXX & g_config_hisi_noc_data->platform_id)      \
	node = hi3xxx_nodes_array[i];           \
else if (HISI_NOC_HI6XXX & g_config_hisi_noc_data->platform_id ) \
	node = hi6220_nodes_array[i];           \

/*used by k3*/
#ifdef  CONFIG_HISI_NOC_HI3650_PLATFORM
#define HI3XXX_NOC_MAX_IRQ_NR     64
#define HI3XXX_NOC_PCTRL_IRQ_MASK 0xF04000000807ABFBULL
#define PCTRL_NOC_IRQ_STAT1   0x094
#define PCTRL_NOC_IRQ_STAT2   0x0A0
#else
#define HI3XXX_NOC_MAX_IRQ_NR   33
#define HI3XXX_NOC_PCTRL_IRQ_MASK 0x00000001FFFFFFFFULL
#define PCTRL_NOC_IRQ_STAT1   0x094
#define PCTRL_NOC_IRQ_STAT2   0x098
#endif
#define HI3XXX_MAX_NODES_NR   33

#define PMCTRL_PERI_INT_STAT0_OFFSET   0x3A4
#define PMCTRL_PERI_INT_MASK_OFFSET   0x3A0

/*used by v8*/
#define HI6220_NOC_MAX_IRQ_NR   4
#define HI6220_MAX_NODES_NR    4
#define TIMER_PERIOD_MAX 300000
#define TIMES_MAX 10000
#define SYS_ERR_INT      74
#define MEDIA_ERR_INT    75
#define SYS_PACKET_INT   81
#define MEDIA_PACKET_INT 83
#define SOC_SYSNOC_SERVICE_AO_BASE_ADDR               (0xF900A000)
#define SOC_SYSNOC_SERVICE_BASE_ADDR                  (0xF9000000)
#define SOC_SYSNOC_SERVICE_DMA_BASE_ADDR              (0xF9008000)


#define HISI_NOC_HI6220 (1<<0)
#define HISI_NOC_HI3635 (1<<1)
#define HISI_NOC_HI3650 (1<<2)

#define HISI_NOC_HI3XXX (HISI_NOC_HI3635|HISI_NOC_HI3650)
#define HISI_NOC_HI6XXX (HISI_NOC_HI6220)

struct hisi_noc_data {
	unsigned int platform_id;
};


enum NOC_IRQ_TPYE {
	NOC_ERR_PROBE_IRQ,
	NOC_PACKET_PROBE_IRQ,
	NOC_TRANS_PROBE_IRQ,
};

struct hisi_noc_irqd {
	enum NOC_IRQ_TPYE type;
	struct noc_node *node;
};
struct hisi_noc_device {
	struct device		*dev;
	void __iomem		*sys_base;
	void __iomem        *media_base;
	unsigned int		hi6220_irq[4];
	void __iomem		*pctrl_base;
	void __iomem		*pmctrl_base;
	void __iomem		*pwrctrl_reg;
	unsigned int		hi3xxx_irq;
	unsigned int		timeout_irq;
};

struct	packet_configration {
	unsigned int		statperiod;
	unsigned int		statalarmmax;

	unsigned int		packet_counters_0_src;
	unsigned int		packet_counters_1_src;
	unsigned int        packet_counters_2_src;
	unsigned int        packet_counters_3_src;

	unsigned int		packet_counters_0_alarmmode;
	unsigned int		packet_counters_1_alarmmode;
    unsigned int		packet_counters_2_alarmmode;
    unsigned int		packet_counters_3_alarmmode;

	unsigned int		packet_filterlut;
	unsigned int		packet_f0_routeidbase;
	unsigned int		packet_f0_routeidmask;
	unsigned int		packet_f0_addrbase;
	unsigned int		packet_f0_windowsize;
	unsigned int		packet_f0_securitymask;
	unsigned int		packet_f0_opcode;
	unsigned int		packet_f0_status;
	unsigned int		packet_f0_length;
	unsigned int		packet_f0_urgency;
	unsigned int		packet_f0_usermask;

	unsigned int		packet_f1_routeidbase;
	unsigned int		packet_f1_routeidmask;
	unsigned int		packet_f1_addrbase;
	unsigned int		packet_f1_windowsize;
	unsigned int		packet_f1_securitymask;
	unsigned int		packet_f1_opcode;
	unsigned int		packet_f1_status;
	unsigned int		packet_f1_length;
	unsigned int		packet_f1_urgency;
	unsigned int		packet_f1_usermask;

};
struct transcation_configration {
	unsigned int		statperiod;
	unsigned int		statalarmmax;

	unsigned int		trans_f_mode;
	unsigned int		trans_f_addrbase_low;
	unsigned int		trans_f_addrwindowsize;
	unsigned int		trans_f_opcode;
	unsigned int		trans_f_usermask;
	unsigned int		trans_f_securitymask;

	unsigned int		trans_p_mode;
	unsigned int		trans_p_thresholds_0_0;
	unsigned int		trans_p_thresholds_0_1;
	unsigned int		trans_p_thresholds_0_2;
	unsigned int		trans_p_thresholds_0_3;

	unsigned int		trans_m_counters_0_src;
	unsigned int		trans_m_counters_1_src;
	unsigned int		trans_m_counters_2_src;
	unsigned int		trans_m_counters_3_src;

	unsigned int		trans_m_counters_0_alarmmode;
	unsigned int		trans_m_counters_1_alarmmode;
	unsigned int		trans_m_counters_2_alarmmode;
	unsigned int		trans_m_counters_3_alarmmode;
};
struct noc_node {
	char			*name;
	void __iomem		*base;
	unsigned int		bus_id;
	unsigned int		interrupt_num;
	unsigned int		pwrack_bit;
	unsigned int		eprobe_offset;
	bool			eprobe_autoenable;
	int			eprobe_hwirq;
   	int                 hwirq_type;
	struct	transcation_configration	tran_cfg;
	struct	packet_configration		packet_cfg;

};

struct noc_statstic_data {
    u64 time;
    u32 counter3;
    u32 counter2;
    u32 counter1;
    u32 counter0;
};

void __iomem * get_errprobe_base(const char *name);
struct noc_node * get_probe_node(const char *name);

/*used by v8*/
extern void __iomem *error_logger;
extern void __iomem *error_logger_media;

extern const struct hisi_noc_data *g_config_hisi_noc_data;

//add for Hi_3xxx
unsigned int noc_find_addr_from_routeid(unsigned int idx, int initflow, int targetflow, int targetsubrange);
typedef struct datapath_routid_addr {
    int init_flow;
    int targ_flow;
    int targ_subrange;
    unsigned int init_localaddr;
} ROUTE_ID_ADDR_STRU;

#endif
