/*
* vstool_if.h   --  VSTOOL if driver
*/

#ifndef _VSTOOL_H
#define _VSTOOL_H


/*****************************************************************************
* MACRO DEFINE BEGIN
*****************************************************************************/

#define VST_INVALID_IDX             (0xFFFF)
#define VST_REG_INVALID             (0xFFF0)
#define VST_REG_INIT                (0xFFF1)
#define VST_REG_DUMP                (0xFFF2)


#define VSTOOL_DBG_SIZE_PAGE		(4096)
#define VSTOOL_DBG_SIZE_HUGE		(40960)

#define VST_NAME_SIZE				(16)

#define VSTOOL_MODE					(0664)

/* corresponding to vstool_ip_info */
#define IDX_VST_SCHARGER			0
#define IDX_VST_PCODEC				1
#define IDX_VST_SCODEC				2
#define IDX_VST_PMU					3


/* for ip_info */
typedef int (*vstool_reg_read_t)(unsigned int, unsigned int *);
typedef void (*vstool_reg_write_t)(unsigned int, unsigned int);

struct vstool_ip_info {
	/* ip name */
	char *reg_group_name;
	/* register range */
	unsigned int reg_start;
	unsigned int reg_end;
	/* register step */
	unsigned int reg_step;
	/* register read func */
	vstool_reg_read_t func_reg_read;
	/* register write func */
	vstool_reg_write_t func_reg_write;
};

struct vstool_soc_info {
    struct list_head node;
	/* soc part name */
	char reg_group_name[VST_NAME_SIZE];
	/* base addr */
	void __iomem *vbaseaddr;
	unsigned int reg_baseaddr;
	unsigned int reg_basize;
	/* register range */
	unsigned int reg_start;
	unsigned int reg_end;
	/* register step */
	unsigned int reg_step;
};

struct vstool_info {
    struct dentry *df_dir;        /* debugfs dir*/
};


/* Declare */
static int vstool_scharger_reg_read(unsigned int reg, unsigned int *pvalue);
static void vstool_scharger_reg_write(unsigned int reg, unsigned int value);
static int vstool_pcodec_reg_read(unsigned int reg, unsigned int *pvalue);
static void vstool_pcodec_reg_write(unsigned int reg, unsigned int value);
static int vstool_scodec_reg_read(unsigned int reg, unsigned int *pvalue);
static void vstool_scodec_reg_write(unsigned int reg, unsigned int value);
static int vstool_pmu_reg_read(unsigned int reg, unsigned int *pvalue);
static void vstool_pmu_reg_write(unsigned int reg, unsigned int value);


#endif /* _VSTOOL_H */
