/*
* vstool_if.c   --  VSTOOL if driver
*/
#define MERROR_LEVEL    1
#define MWARNING_LEVEL  1
#define MNOTICE_LEVEL   1
#define MINFO_LEVEL     0
#define MDEBUG_LEVEL    0
#define MLOG_TAG        "vstool"

#include <linux/init.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/list.h>
#include <linux/debugfs.h>
#include <asm/io.h>
#include "vstool_if.h"
#include "mntn_log.h"


/* Declare */
extern int scharger_write_regs(u8 value, u8 reg);
extern int scharger_read_regs(u8 *value, u8 reg);
extern unsigned int codec_reg_read(unsigned int reg);
extern void codec_reg_write(unsigned int reg, unsigned int value);
extern unsigned char hi6xxx_pmic_reg_read (unsigned int reg_addr);
extern void hi6xxx_pmic_reg_write (unsigned int reg_addr, unsigned char regval);


/* 
Definition 
*/
static struct vstool_info vstool_priv;
/* 暂存待读或写的寄存器，工具使用 */
static unsigned int g_vs_reg = VST_REG_INVALID;
static unsigned int g_vs_baseaddr = 0;
/* soc部分数目不定，故使用list */
static LIST_HEAD(soc_info_head);
static unsigned int g_soc_info_index = 0;

static struct vstool_ip_info ip_info[] = {
	/* scharger */
	{ "scharger",  0x00, 0x6B,  1, vstool_scharger_reg_read, vstool_scharger_reg_write },
	/* pmu codec */
	{ "pcodec",   0x1B1, 0x1E9, 1, vstool_pcodec_reg_read,   vstool_pcodec_reg_write },
	/* soc codec */
	{ "scodec",    0x00, 0xFC,  4, vstool_scodec_reg_read,   vstool_scodec_reg_write },
	/* smartstar pmu */
	{ "pmu",       0x00, 0xFD,  1, vstool_pmu_reg_read,      vstool_pmu_reg_write },
};




/*
scharger
input: 	unsigned int reg
output: unsigned int *pvalue
return is int, 0 is OK
*/
static int vstool_scharger_reg_read(unsigned int reg, unsigned int *pvalue)
{
	int retn;

	u8 modu_val;
	u8 modu_reg;
	int modu_retn;

	/* input */
	modu_reg = (u8)reg;

	modu_retn = scharger_read_regs(&modu_val, modu_reg);
	if (modu_retn != 0) {
		/* read error */
		retn = -1;
	} else {
		retn = 0;
	}

	/* output */
	*pvalue = (unsigned int)modu_val;

	return retn;
}

/*
scharger
input: 	unsigned int reg
		unsigned int value
return is void
*/
static void vstool_scharger_reg_write(unsigned int reg, unsigned int value)
{
	int retn;

	u8 modu_val;
	u8 modu_reg;
	int modu_retn;

	/* input */
	modu_reg = (u8)reg;
	modu_val = (u8)value;

	modu_retn = scharger_write_regs(modu_val, modu_reg);
	if (modu_retn != 0) {
		/* read error */
		retn = -1;
		mlog_e("scharger_write_regs fail ret=0x%x", modu_retn);
	} else {
		retn = 0;
	}

	return;
}


/*
pmu codec
input: 	unsigned int reg
output: unsigned int *pvalue
return is int, 0 is OK
*/
static int vstool_pcodec_reg_read(unsigned int reg, unsigned int *pvalue)
{
	unsigned int modu_val;
	unsigned int modu_reg;

	/* input */
	modu_reg = reg;
	modu_reg |= 0x1000/*PAGE_SmartStarCODEC*/;

	modu_val = codec_reg_read(modu_reg);

	/* output */
	*pvalue = modu_val;

	return 0;
}

/*
pmu codec
input: 	unsigned int reg
		unsigned int value
return is void
*/
static void vstool_pcodec_reg_write(unsigned int reg, unsigned int value)
{
	unsigned int modu_val;
	unsigned int modu_reg;

	/* input */
	modu_reg = reg;
	modu_val = value;
	modu_reg |= 0x1000/*PAGE_SmartStarCODEC*/;

	codec_reg_write(modu_reg, modu_val);

	return;
}

/*
soc codec
input: 	unsigned int reg
output: unsigned int *pvalue
return is int, 0 is OK
*/
static int vstool_scodec_reg_read(unsigned int reg, unsigned int *pvalue)
{
	unsigned int modu_val;
	unsigned int modu_reg;

	/* input */
	modu_reg = reg;
	modu_reg |= 0x0000/*PAGE_SoCCODEC*/;

	modu_val = codec_reg_read(modu_reg);

	/* output */
	*pvalue = modu_val;

	return 0;
}

/*
soc codec
input: 	unsigned int reg
		unsigned int value
return is void
*/
static void vstool_scodec_reg_write(unsigned int reg, unsigned int value)
{
	unsigned int modu_val;
	unsigned int modu_reg;

	/* input */
	modu_reg = reg;
	modu_val = value;
	modu_reg |= 0x0000/*PAGE_SoCCODEC*/;

	codec_reg_write(modu_reg, modu_val);

	return;
}

/*
smartstar pmu
input: 	unsigned int reg
output: unsigned int *pvalue
return is int, 0 is OK
*/
static int vstool_pmu_reg_read(unsigned int reg, unsigned int *pvalue)
{
	unsigned int modu_reg;
	unsigned char modu_val;

	/* input */
	modu_reg = reg;

	modu_val = hi6xxx_pmic_reg_read(modu_reg);

	/* output */
	*pvalue = (unsigned int)modu_val;

	return 0;
}

/*
smartstar pmu
input: 	unsigned int reg
		unsigned int value
return is void
*/
static void vstool_pmu_reg_write(unsigned int reg, unsigned int value)
{
	unsigned int modu_reg;
	unsigned char modu_val;

	/* input */
	modu_reg = reg;
	modu_val = (unsigned char)value;

	hi6xxx_pmic_reg_write(modu_reg, modu_val);

	return;
}

/*
soc
input is unsigned int
return is void *
*/
static void __iomem * vstool_soc_get_vbaseaddr(unsigned int baseaddr_start)
{
	void __iomem *vaddr = NULL;
	struct list_head *pos;
	struct vstool_soc_info *psoc_info;

    list_for_each(pos, &soc_info_head) {
		/* get the soc info */
		psoc_info = list_entry(pos, struct vstool_soc_info, node);

		if (baseaddr_start == psoc_info->reg_baseaddr) {
			vaddr = psoc_info->vbaseaddr;
			break;
		}
    }

	return vaddr;
}

/*
soc
input:	unsigned int
		struct vstool_soc_info *
return is int, 0 is OK
*/
static int vstool_soc_get_info(unsigned int baseaddr_start, struct vstool_soc_info *isoc_info)
{
	int ret = -1;
	struct list_head *pos;
	struct vstool_soc_info *psoc_info;

    list_for_each(pos, &soc_info_head) {
		/* get the soc info */
		psoc_info = list_entry(pos, struct vstool_soc_info, node);

		if (baseaddr_start == psoc_info->reg_baseaddr) {
			isoc_info->reg_baseaddr = psoc_info->reg_baseaddr;
			isoc_info->reg_basize	= psoc_info->reg_basize;
			isoc_info->vbaseaddr	= psoc_info->vbaseaddr;
			isoc_info->reg_start	= psoc_info->reg_start;
			isoc_info->reg_end		= psoc_info->reg_end;
			isoc_info->reg_step		= psoc_info->reg_step;
			strncpy(isoc_info->reg_group_name, psoc_info->reg_group_name, VST_NAME_SIZE);
			isoc_info->reg_group_name[VST_NAME_SIZE-1] = '\0';

			ret = 0;
			break;
		}
    }

	return ret;
}

/*
soc
input is unsigned int
return is int, 0 is OK
*/
static int vstool_soc_init(unsigned int baseaddr_start, unsigned int size, 
					unsigned int reg_start, unsigned int reg_end, unsigned int reg_step, char *name)
{
	void __iomem *vaddr;
	struct vstool_soc_info *psoc_info;

	/* check input */

	/* get base addr */
	vaddr = vstool_soc_get_vbaseaddr(baseaddr_start);
	if (!vaddr) {
		vaddr = ioremap(baseaddr_start, size);
		if (!vaddr) {
			mlog_e("cannot map memory\n");
			return -ENOMEM;
		}
	} else {
		/* already exist */
		return 0;
	}

	/* we won't free the space and 1 baseaddr corresponding to 1 */
	psoc_info = kmalloc(sizeof(struct vstool_soc_info), GFP_KERNEL);
	psoc_info->reg_baseaddr	= baseaddr_start;
	psoc_info->reg_basize	= size;
	psoc_info->vbaseaddr	= vaddr;
	psoc_info->reg_start	= reg_start;
	psoc_info->reg_end		= reg_end;
	psoc_info->reg_step		= reg_step;
	strncpy(psoc_info->reg_group_name, name, VST_NAME_SIZE);
	psoc_info->reg_group_name[VST_NAME_SIZE-1] = '\0';

	list_add(&psoc_info->node, &soc_info_head);
	g_soc_info_index++;

	/* output */

	return 0;
}



/*
soc
input:	unsigned int baseaddr
		unsigned int reg
output:	unsigned int *pvalue
return is int, 0 is OK
*/
static int vstool_soc_reg_read(unsigned int baseaddr, unsigned int reg, unsigned int *pvalue)
{
	void __iomem *vaddr;
	unsigned int modu_reg;
	unsigned int modu_val;

	/* input */
	modu_reg = reg;

	/* get base addr */
	vaddr = vstool_soc_get_vbaseaddr(baseaddr);
	if (!vaddr) {
		mlog_e("vbaseaddr get failed\n");
		return -1;
	}

	modu_val = readl(vaddr + modu_reg);

	/* output */
	*pvalue = (unsigned int)modu_val;

	return 0;
}

/*
soc
input:	unsigned int baseaddr
		unsigned int reg
		unsigned int value
return is void
*/
static void vstool_soc_reg_write(unsigned int baseaddr, unsigned int reg, unsigned int value)
{
	void __iomem *vaddr;
	unsigned int modu_reg;
	unsigned int modu_val;

	/* input */
	modu_reg = reg;
	modu_val = value;

	/* get base addr */
	vaddr = vstool_soc_get_vbaseaddr(baseaddr);
	if (!vaddr) {
		mlog_e("vbaseaddr get failed\n");
		return;
	}

	writel(modu_val, vaddr + modu_reg);

	return;
}

/*
批量寄存器输出 公共接口 for ip_info
*/
static void vstool_rr_dump(char *buf, unsigned int index)
{
    unsigned int regi = 0, regi_val = 0;
	unsigned int reg_start, reg_end, reg_step;
	char *reg_group_name;
	vstool_reg_read_t func_reg_read;
	int ret;

	/* get the specific ip info */
	func_reg_read = ip_info[index].func_reg_read;
	reg_start = ip_info[index].reg_start;
	reg_end   = ip_info[index].reg_end;
	reg_step  = ip_info[index].reg_step;
	reg_group_name = ip_info[index].reg_group_name;

    memset(buf, 0, VSTOOL_DBG_SIZE_PAGE);

    snprintf(buf, VSTOOL_DBG_SIZE_PAGE, "Page %s:\n", reg_group_name);
    for (regi = reg_start; regi <= reg_end; regi += reg_step) {
		ret = func_reg_read(regi, &regi_val);
		if (ret != 0) {
			/* 读可判定成功或失败 */
			snprintf(buf, VSTOOL_DBG_SIZE_PAGE, "%s%#04x:error\n", buf, regi);
		} else {
			if (1 == reg_step) {
				snprintf(buf, VSTOOL_DBG_SIZE_PAGE, "%s%#04x:%#04x\n", buf, regi, regi_val);
			} else {
				snprintf(buf, VSTOOL_DBG_SIZE_PAGE, "%s%#04x:%#010x\n", buf, regi, regi_val);
			}
		}
    }

    snprintf(buf, VSTOOL_DBG_SIZE_PAGE, "%s<cat end>\n", buf);
    snprintf(buf, VSTOOL_DBG_SIZE_PAGE, "%s\n", buf);

}

/*
寄存器读写公共接口 for ip_info
对于不同的读取类型，入参分别如下:
1 无参: 默认输出PAGE
2 单寄存器读: echo "r reg" > rr，cat rr
3 单寄存器写: echo "w reg val"
*/
static int vstool_rr_read(struct file *file, char __user *user_buf,
                      size_t count, loff_t *ppos, unsigned int index)
{
    char *kn_buf = NULL;
    ssize_t byte_read = 0;
    unsigned int vs_reg_r = g_vs_reg;
	unsigned int vs_reg_val;
	int ret;
	vstool_reg_read_t func_reg_read;

	/* get the specific ip info */
	func_reg_read = ip_info[index].func_reg_read;

    kn_buf = kmalloc(VSTOOL_DBG_SIZE_PAGE, GFP_KERNEL);
    if (NULL == kn_buf) {
        mlog_e("kn_buf is null\n");
        return simple_read_from_buffer(user_buf, count, ppos, "kn_buf alloc fail\n", 18);
    }

    memset(kn_buf, 0, VSTOOL_DBG_SIZE_PAGE);
    if (VST_REG_INVALID != vs_reg_r) {
		/* Single Register Read */
		ret = func_reg_read(vs_reg_r, &vs_reg_val);
		if (ret != 0) {
			/* 读可判定成功或失败 */
			snprintf(kn_buf, VSTOOL_DBG_SIZE_PAGE, "%s%#06x:error\n", kn_buf, vs_reg_r);
		} else {
			snprintf(kn_buf, VSTOOL_DBG_SIZE_PAGE, "%s%#06x:%#010x\n", kn_buf, vs_reg_r, vs_reg_val);
		}

		snprintf(kn_buf, VSTOOL_DBG_SIZE_PAGE, "%s<cat end>\n", kn_buf);
    } else {
		/* Mass Register Read */
        vstool_rr_dump(kn_buf, index);
    }

    byte_read = simple_read_from_buffer(user_buf, count, ppos, kn_buf, strlen(kn_buf));

    kfree(kn_buf);
    kn_buf = NULL;

    return byte_read;
}

/*
寄存器读写公共接口 for ip_info
对于不同的读取类型，入参分别如下:
1 无参: 默认输出PAGE
2 单寄存器读: echo "r reg" > rr，cat rr
3 单寄存器写: echo "w reg val"
4 批量寄存器读: echo p > rr, cat rr
*/
static int vstool_rr_write(struct file *file, const char __user *user_buf, 
						size_t count, loff_t *ppos, unsigned int index)
{
    char *kn_buf = NULL;
    ssize_t byte_writen = 0;
    int num = 0;
    unsigned int vs_reg = 0, vs_val = 0;
	vstool_reg_write_t func_reg_write;

	func_reg_write = ip_info[index].func_reg_write;

    kn_buf = kmalloc(VSTOOL_DBG_SIZE_PAGE, GFP_KERNEL);
    if (NULL == kn_buf) {
        mlog_e("kn_buf is null\n");
        return -EFAULT;
    }

    byte_writen = simple_write_to_buffer(kn_buf, VSTOOL_DBG_SIZE_PAGE, ppos, user_buf, count);

    if ('w' == kn_buf[0]) {
        /* 写单个寄存器, 2个参数 */
        num = sscanf(kn_buf, "w 0x%x 0x%x", &vs_reg, &vs_val);
        if (2 == num) {
            func_reg_write(vs_reg, vs_val);
            g_vs_reg = vs_reg;
        } else {
            g_vs_reg = VST_REG_INVALID;
            byte_writen = -EINVAL;
        }

    } else if ('r' == kn_buf[0]) {
        /* 读单个寄存器，记录待读取寄存器 */
        num = sscanf(kn_buf, "r 0x%x", &g_vs_reg);
        if (1 != num) {
            g_vs_reg = VST_REG_INVALID;
            byte_writen = -EINVAL;
        }
    } else if ('p' == kn_buf[0]) {
        /* 读PAGE */
        g_vs_reg = VST_REG_INVALID;
    } else {
        /* 异常 */
        g_vs_reg = VST_REG_INVALID;
        byte_writen = -EINVAL;
    }

    kfree(kn_buf);
    kn_buf = NULL;

    return byte_writen;
}

/*
SoC结构体信息查询 公共接口 for soc_info
*/
static void vstool_sr_show(char *buf)
{
	struct list_head *pos;
	struct vstool_soc_info *psoc_info;

    memset(buf, 0, VSTOOL_DBG_SIZE_HUGE);

	snprintf(buf, VSTOOL_DBG_SIZE_HUGE, "soc initialized: %d\n", g_soc_info_index);
    list_for_each(pos, &soc_info_head) {
		/* get the soc info */
		psoc_info = list_entry(pos, struct vstool_soc_info, node);

		snprintf(buf, VSTOOL_DBG_SIZE_HUGE, "%s<%s @%#04x>\n", buf, 
				psoc_info->reg_group_name, psoc_info->reg_baseaddr);
		snprintf(buf, VSTOOL_DBG_SIZE_HUGE, "%svbase:0x%p size:%#04x start:%#04x end:%#04x step:%#04x\n", buf, 
				psoc_info->vbaseaddr, psoc_info->reg_basize, 
				psoc_info->reg_start, psoc_info->reg_end, psoc_info->reg_step);
    }

    snprintf(buf, VSTOOL_DBG_SIZE_HUGE, "%s<cat end>\n", buf);
    snprintf(buf, VSTOOL_DBG_SIZE_HUGE, "%s\n", buf);

}

/*
批量寄存器输出 公共接口 for soc_info
*/
static void vstool_sr_dump(char *buf, unsigned int baseaddr)
{
    unsigned int regi = 0, regi_val = 0;
	unsigned int reg_start, reg_end, reg_step;
    unsigned int vs_baseaddr_r;
	char *reg_group_name;
	int ret;
	struct vstool_soc_info soc_info;

    memset(buf, 0, VSTOOL_DBG_SIZE_HUGE);

	/* get base addr */
	ret = vstool_soc_get_info(baseaddr, &soc_info);
	if (0 != ret) {
	    snprintf(buf, VSTOOL_DBG_SIZE_HUGE, "Page error, baseaddr %#010x not found\n", baseaddr);
		goto Out;
	}

	/* get the specific ip info */
	reg_start = soc_info.reg_start;
	reg_end   = soc_info.reg_end;
	reg_step  = soc_info.reg_step;
	reg_group_name = soc_info.reg_group_name;
	vs_baseaddr_r = soc_info.reg_baseaddr;

    snprintf(buf, VSTOOL_DBG_SIZE_HUGE, "Page %s @%#010x\n", reg_group_name, vs_baseaddr_r);
    for (regi = reg_start; regi <= reg_end; regi += reg_step) {
		ret = vstool_soc_reg_read(vs_baseaddr_r, regi, &regi_val);
		if (ret != 0) {
			/* 读可判定成功或失败 */
			snprintf(buf, VSTOOL_DBG_SIZE_HUGE, "%s%#06x:error not inited\n", buf, regi);
		} else {
			snprintf(buf, VSTOOL_DBG_SIZE_HUGE, "%s%#06x:%#010x\n", buf, regi, regi_val);
		}
    }

Out:
    snprintf(buf, VSTOOL_DBG_SIZE_HUGE, "%s<cat end>\n", buf);
    snprintf(buf, VSTOOL_DBG_SIZE_HUGE, "%s\n", buf);

}

/*
寄存器读写公共接口, for soc_info
对于不同的读取类型，入参分别如下:
1 无参: 默认输出none
2 单寄存器读: echo "r reg baseaddr" > soc，cat soc
3 单寄存器写: echo "w reg val baseaddr"
4 批量寄存器读: echo "p baseaddr" > soc, cat soc
*/
static int vstool_sr_read(struct file *file, char __user *user_buf,
                      size_t count, loff_t *ppos)
{
    char *kn_buf = NULL;
    ssize_t byte_read = 0;
    unsigned int vs_reg_r = g_vs_reg;
    unsigned int vs_baseaddr_r = g_vs_baseaddr;
	unsigned int vs_reg_val;
	int ret;

    kn_buf = kmalloc(VSTOOL_DBG_SIZE_HUGE, GFP_KERNEL);
    if (NULL == kn_buf) {
        mlog_e("kn_buf is null\n");
        return simple_read_from_buffer(user_buf, count, ppos, "kn_buf alloc fail\n", 18);
    }

    memset(kn_buf, 0, VSTOOL_DBG_SIZE_HUGE);
    if (VST_REG_INIT == vs_reg_r) {
		/* show Baseaddr INIT */
        vstool_sr_show(kn_buf);

    } else if (VST_REG_DUMP == vs_reg_r) {
		/* Mass Register Read */
        vstool_sr_dump(kn_buf, vs_baseaddr_r);

	} else if (VST_REG_INVALID != vs_reg_r) {
		/* Single Register Read */
		ret = vstool_soc_reg_read(vs_baseaddr_r, vs_reg_r, &vs_reg_val);
		if (ret != 0) {
			/* 读可判定成功或失败 */
			snprintf(kn_buf, VSTOOL_DBG_SIZE_HUGE, "%s%#06x:error, %#010x not initialized\n", kn_buf, vs_reg_r, vs_baseaddr_r);
		} else {
			snprintf(kn_buf, VSTOOL_DBG_SIZE_HUGE, "%s%#06x:%#010x\n", kn_buf, vs_reg_r, vs_reg_val);
		}
		snprintf(kn_buf, VSTOOL_DBG_SIZE_HUGE, "%s<cat end>\n", kn_buf);

    } else {
		snprintf(kn_buf, VSTOOL_DBG_SIZE_HUGE, "%snone\n", kn_buf);
    }

    byte_read = simple_read_from_buffer(user_buf, count, ppos, kn_buf, strlen(kn_buf));

    kfree(kn_buf);
    kn_buf = NULL;

    return byte_read;
}

/*
寄存器读写公共接口, for soc_info
对于不同的读取类型，入参分别如下:
1 无参: 默认输出none
2 单寄存器读: echo "r reg baseaddr" > soc，cat soc
3 单寄存器写: echo "w reg val baseaddr"
4 批量寄存器读: echo "p baseaddr" > soc, cat soc
*/
static int vstool_sr_write(struct file *file, const char __user *user_buf, 
						size_t count, loff_t *ppos)
{
    char *kn_buf = NULL;
    ssize_t byte_writen = 0;
    int num = 0;
    unsigned int vs_reg = 0, vs_val = 0;
    unsigned int vs_baseaddr = 0, vs_size = 0, vs_start = 0, vs_end = 0, vs_step = 0;
	char vs_name[VST_NAME_SIZE];

    kn_buf = kmalloc(VSTOOL_DBG_SIZE_PAGE, GFP_KERNEL);
    if (NULL == kn_buf) {
        mlog_e("kn_buf is null\n");
        return -EFAULT;
    }

    byte_writen = simple_write_to_buffer(kn_buf, VSTOOL_DBG_SIZE_PAGE, ppos, user_buf, count);

    if ('w' == kn_buf[0]) {
        /* 写单个寄存器, 3个参数 */
        num = sscanf(kn_buf, "w 0x%x 0x%x 0x%x", &vs_reg, &vs_val, &vs_baseaddr);
        if (3 == num) {
            vstool_soc_reg_write(vs_baseaddr, vs_reg, vs_val);
            g_vs_reg = vs_reg;
			g_vs_baseaddr = vs_baseaddr;
        } else {
            g_vs_reg = VST_REG_INVALID;
			g_vs_baseaddr = 0;
            byte_writen = -EINVAL;
        }

	} else if ('r' == kn_buf[0]) {
		/* 读单个寄存器，2个参数 */
		num = sscanf(kn_buf, "r 0x%x 0x%x", &g_vs_reg, &g_vs_baseaddr);
		if (2 != num) {
			g_vs_reg = VST_REG_INVALID;
			g_vs_baseaddr = 0;
			byte_writen = -EINVAL;
		}

	} else if ('i' == kn_buf[0]) {
		/* 初始化 */
		num = sscanf(kn_buf, "i 0x%x 0x%x 0x%x 0x%x 0x%x %15s", &vs_baseaddr, &vs_size, &vs_start, &vs_end, &vs_step, vs_name);
		if (6 == num) {
			if (0 != vstool_soc_init(vs_baseaddr, vs_size, vs_start, vs_end, vs_step, vs_name)) {
				/* error */
				byte_writen = -EINVAL;
			} else {
				g_vs_reg = VST_REG_INIT;
			}
		} else if (0 == num) {
			/* to display INIT */
			g_vs_reg = VST_REG_INIT;
		} else {
			byte_writen = -EINVAL;
		}

    } else if ('p' == kn_buf[0]) {
        /* 读PAGE */
		num = sscanf(kn_buf, "p 0x%x", &g_vs_baseaddr);
		if (1 == num) {
			g_vs_reg = VST_REG_DUMP;
		} else {
			g_vs_reg = VST_REG_INVALID;
			g_vs_baseaddr = 0;
			byte_writen = -EINVAL;
		}

    } else {
        /* 异常 */
        g_vs_reg = VST_REG_INVALID;
        byte_writen = -EINVAL;
    }

    kfree(kn_buf);
    kn_buf = NULL;

    return byte_writen;
}


static int vstool_scharger_read(struct file *file, char __user *user_buf,
                      size_t count, loff_t *ppos)
{
	return vstool_rr_read(file, user_buf, count, ppos, IDX_VST_SCHARGER);
}

static int vstool_scharger_write(struct file *file, const char __user *user_buf, size_t count, loff_t *ppos)
{
	return vstool_rr_write(file, user_buf, count, ppos, IDX_VST_SCHARGER);
}

/* scharger */
static const struct file_operations vstool_scharger_fops = {
    .read  = vstool_scharger_read,
    .write = vstool_scharger_write,
};

static int vstool_pcodec_read(struct file *file, char __user *user_buf,
                      size_t count, loff_t *ppos)
{
	return vstool_rr_read(file, user_buf, count, ppos, IDX_VST_PCODEC);
}

static int vstool_pcodec_write(struct file *file, const char __user *user_buf, size_t count, loff_t *ppos)
{
	return vstool_rr_write(file, user_buf, count, ppos, IDX_VST_PCODEC);
}

/* pmu codec */
static const struct file_operations vstool_pcodec_fops = {
    .read  = vstool_pcodec_read,
    .write = vstool_pcodec_write,
};

static int vstool_scodec_read(struct file *file, char __user *user_buf,
                      size_t count, loff_t *ppos)
{
	return vstool_rr_read(file, user_buf, count, ppos, IDX_VST_SCODEC);
}

static int vstool_scodec_write(struct file *file, const char __user *user_buf, size_t count, loff_t *ppos)
{
	return vstool_rr_write(file, user_buf, count, ppos, IDX_VST_SCODEC);
}

/* soc codec */
static const struct file_operations vstool_scodec_fops = {
    .read  = vstool_scodec_read,
    .write = vstool_scodec_write,
};


static int vstool_pmu_read(struct file *file, char __user *user_buf,
                      size_t count, loff_t *ppos)
{
	return vstool_rr_read(file, user_buf, count, ppos, IDX_VST_PMU);
}

static int vstool_pmu_write(struct file *file, const char __user *user_buf, size_t count, loff_t *ppos)
{
	return vstool_rr_write(file, user_buf, count, ppos, IDX_VST_PMU);
}

/* smartstar pmu */
static const struct file_operations vstool_pmu_fops = {
    .read  = vstool_pmu_read,
    .write = vstool_pmu_write,
};


static int vstool_soc_read(struct file *file, char __user *user_buf,
                      size_t count, loff_t *ppos)
{
	return vstool_sr_read(file, user_buf, count, ppos);
}

static int vstool_soc_write(struct file *file, const char __user *user_buf, size_t count, loff_t *ppos)
{
	return vstool_sr_write(file, user_buf, count, ppos);
}

/* soc */
static const struct file_operations vstool_soc_fops = {
    .read  = vstool_soc_read,
    .write = vstool_soc_write,
};


static int vstool_tool_init(void)
{

    vstool_priv.df_dir = debugfs_create_dir("vstool", NULL);
    if (!vstool_priv.df_dir) {
        mlog_e("create vstool debugfs dir\n");
        return -ENOMEM;
    }

    /* register，scharger读写接口 */
    if (!debugfs_create_file("scharger", VSTOOL_MODE, vstool_priv.df_dir, NULL, &vstool_scharger_fops)) {
        mlog_e("create vstool debugfs scharger\n");
        return -ENOMEM;
    }

    /* register，pmu codec读写接口 */
    if (!debugfs_create_file("pcodec", VSTOOL_MODE, vstool_priv.df_dir, NULL, &vstool_pcodec_fops)) {
        mlog_e("create vstool debugfs acodec\n");
        return -ENOMEM;
    }

    /* register，soc codec读写接口 */
    if (!debugfs_create_file("scodec", VSTOOL_MODE, vstool_priv.df_dir, NULL, &vstool_scodec_fops)) {
        mlog_e("create vstool debugfs acodec\n");
        return -ENOMEM;
    }

    /* register，smartstar pmu读写接口 */
    if (!debugfs_create_file("pmu", VSTOOL_MODE, vstool_priv.df_dir, NULL, &vstool_pmu_fops)) {
        mlog_e("create vstool debugfs pmu\n");
        return -ENOMEM;
    }

    /* register，soc读写接口 */
    if (!debugfs_create_file("soc", VSTOOL_MODE, vstool_priv.df_dir, NULL, &vstool_soc_fops)) {
        mlog_e("create vstool debugfs soc\n");
        return -ENOMEM;
    }

    return 0;
}

static int vstool_tool_uninit(void)
{
    if (NULL != vstool_priv.df_dir) {
        debugfs_remove(vstool_priv.df_dir);
        vstool_priv.df_dir = NULL;
    }

    return 0;
}

static int __init vstool_init(void)
{
	vstool_tool_init();

	return 0;
}
module_init(vstool_init);

static void __exit vstool_exit(void)
{
	vstool_tool_uninit();

	return;
}
module_exit(vstool_exit);

MODULE_DESCRIPTION("HISI VSTOOLIF driver");
MODULE_LICENSE("GPL");
