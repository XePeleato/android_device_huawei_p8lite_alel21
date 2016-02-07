#ifndef __HISI_NOC_TRANSCATION
#define __HISI_NOC_TRANSCATION

#include "hisi_noc.h"
#ifdef CONFIG_HISI_NOC_HI3650_PLATFORM
 
#define TRANS_F_MODE			(0x0008)
#define TRANS_F_ADDRBASE_LOW		(0x000C)
#define TRANS_F_ADDRWINDOWSIZE		(0x0014)
#define TRANS_F_OPCODE			(0x0020)
#define TRANS_F_USERMASK		(0x0028)
#define TRANS_F_SECURITYMASK		(0x0030)
#define TRANS_P_EN			(0x0088)
#define TRANS_P_MODE			(0x008C)
#define TRANS_P_THRESHOLDS_0_0		(0x00AC)
#define TRANS_P_THRESHOLDS_0_1		(0x00B0)
#define TRANS_P_THRESHOLDS_0_2		(0x00B4)
#define TRANS_P_THRESHOLDS_0_3		(0x00B8)
#define TRANS_P_OVERFLOWSTATUS		(0x00EC)
#define TRANS_P_OVERFLOWRESET		(0x00F0)
#else
#define TRANS_P_EN			(0x0008)
#define TRANS_P_MODE			(0x000C)
#define TRANS_P_THRESHOLDS_0_0		(0x002C)
#define TRANS_P_THRESHOLDS_0_1		(0x0030)
#define TRANS_P_THRESHOLDS_0_2		(0x0034)
#define TRANS_P_THRESHOLDS_0_3		(0x0038)
#define TRANS_P_OVERFLOWSTATUS		(0x006C)
#define TRANS_P_OVERFLOWRESET		(0x0070)
#define TRANS_F_MODE			(0x0088)
#define TRANS_F_ADDRBASE_LOW		(0x008C)
#define TRANS_F_ADDRWINDOWSIZE		(0x0094)
#define TRANS_F_OPCODE			(0x00A0)
#define TRANS_F_USERMASK		(0x00A8)
#define TRANS_F_SECURITYMASK		(0x00B0)
#endif
#define TRANS_M_MAINCTL			(0x0408)
#define TRANS_M_CFGCTL			(0x040C)
#define TRANS_M_STATPERIOD		(0x0424)
#define TRANS_M_STATALARMMAX		(0x0430)
#define TRANS_M_STATALARMCLR		(0x0438)
#define TRANS_M_COUNTERS_0_SRC		(0x0538)
#define TRANS_M_COUNTERS_0_ALARMMODE	(0x053C)
#define TRANS_M_COUNTERS_0_VAL		(0x0540)
#define TRANS_M_COUNTERS_1_SRC		(0x054C)
#define TRANS_M_COUNTERS_1_ALARMMODE	(0x0550)
#define TRANS_M_COUNTERS_1_VAL		(0x0554)
#define TRANS_M_COUNTERS_2_SRC		(0x0560)
#define TRANS_M_COUNTERS_2_ALARMMODE	(0x0564)
#define TRANS_M_COUNTERS_2_VAL		(0x0568)
#define TRANS_M_COUNTERS_3_SRC		(0x0574)
#define TRANS_M_COUNTERS_3_ALARMMODE	(0x0578)
#define TRANS_M_COUNTERS_3_VAL		(0x057C)

void disable_transcation_probe (void __iomem *base);
void enable_transcation_probe(const struct noc_node *node, void __iomem *base);
void noc_transcation_probe_hanlder(const struct noc_node *node, void __iomem *base, unsigned int idx);
void init_transcation_info(struct noc_node *node);
void enable_transcation_probe_by_name(const char *name);
void disable_transcation_probe_by_name(const char *name);
void config_transcation_probe(const char *name, const struct transcation_configration *tran_cfg);
#endif
