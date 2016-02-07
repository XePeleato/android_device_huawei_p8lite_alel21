#ifndef __HISI_NOC_BUS_H
#define __HISI_NOC_BUS_H

/*used by v8*/
#define SYSNOC_INITFLOW_ARRAY_SIZE		15
#define SYSNOC_TARGETFLOW_ARRAY_SIZE	15
#define MEDIA_INITFLOW_ARRAY_SIZE		7
#define MEDIA_TARGETFLOW_ARRAY_SIZE		12
#define HI6220_MAX_NOC_BUSES_NR	2

#ifdef CONFIG_HISI_NOC_HI3650_PLATFORM
#define CFG_INITFLOW_ARRAY_SIZE		22
#define CFG_TARGETFLOW_ARRAY_SIZE	33
#define VIVO_INITFLOW_ARRAY_SIZE		9
#define VIVO_TARGETFLOW_ARRAY_SIZE	    8
#define VCODEC_INITFLOW_ARRAY_SIZE		3
#define VCODEC_TARGETFLOW_ARRAY_SIZE	    7
#else
/* noc buses info used by k3*/
#define CFG_INITFLOW_ARRAY_SIZE		17
#define CFG_TARGETFLOW_ARRAY_SIZE	28
#define VIVO_INITFLOW_ARRAY_SIZE		12
#define VIVO_TARGETFLOW_ARRAY_SIZE		8
#define VCODEC_INITFLOW_ARRAY_SIZE		5
#define VCODEC_TARGETFLOW_ARRAY_SIZE		8
#endif
#define HI3XXX_MAX_NOC_BUSES_NR	3

/* keep target route id, initiator flow id etc*/
struct noc_bus_info {
	char *name;
	unsigned int initflow_mask;
	unsigned int initflow_shift;

	unsigned int targetflow_mask;
	unsigned int targetflow_shift;

	unsigned int targ_subrange_mask;
	unsigned int seq_id_mask;

	char **initflow_array;
	unsigned int initflow_array_size;

	char **targetflow_array;
	unsigned int targetflow_array_size;
};

/*used by v8*/
extern char *sysnoc_initflow_array[SYSNOC_INITFLOW_ARRAY_SIZE];
extern char *sysnoc_targetflow_array[SYSNOC_TARGETFLOW_ARRAY_SIZE];
extern char *media_initflow_array[MEDIA_INITFLOW_ARRAY_SIZE];
extern char *media_targetflow_array[MEDIA_TARGETFLOW_ARRAY_SIZE];
extern struct noc_bus_info hi6220_noc_buses_info[HI6220_MAX_NOC_BUSES_NR];

/*used by k3*/
extern char *cfg_initflow_array[CFG_INITFLOW_ARRAY_SIZE];
extern char *cfg_targetflow_array[CFG_TARGETFLOW_ARRAY_SIZE];
extern char *vivo_initflow_array[VIVO_INITFLOW_ARRAY_SIZE];
extern char *vivo_targetflow_array[VIVO_TARGETFLOW_ARRAY_SIZE];
extern char *vcodec_initflow_array[VCODEC_INITFLOW_ARRAY_SIZE];
extern char *vcodec_targetflow_array[VCODEC_TARGETFLOW_ARRAY_SIZE];
extern struct noc_bus_info hi3xxx_noc_buses_info[HI3XXX_MAX_NOC_BUSES_NR];

#endif
