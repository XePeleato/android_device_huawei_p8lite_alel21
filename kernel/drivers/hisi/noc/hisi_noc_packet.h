#ifndef __HISI_NOC_PACKET
#define __HISI_NOC_PACKET

#include "hisi_noc.h"

#define PACKET_MAINCTL 			(0x0008)
#define PACKET_CFGCTL 			(0x000C)
#define PACKET_FILTERLUT		(0x0014)
#define PACKET_STATPERIOD		(0x0024)
#define PACKET_STATALARMMAX		(0x0030)
#define PACKET_STATALARMCLR		(0x0038)
#define PACKET_F0_ROUTEIDBASE		(0x0044)
#define PACKET_F0_ROUTEIDMASK		(0x0048)
#define PACKET_F0_ADDRBASE		(0x004C)
#define PACKET_F0_WINDOWSIZE		(0x0054)
#define PACKET_F0_SECURITYMASK		(0x005C)
#define PACKET_F0_OPCODE		(0x0060)
#define PACKET_F0_STATUS		(0x0064)
#define PACKET_F0_LENGTH		(0x0068)
#define PACKET_F0_URGENCY		(0x006C)
#define PACKET_F0_USERMASK		(0x0074)
#define PACKET_F1_ROUTEIDBASE		(0x0080)
#define PACKET_F1_ROUTEIDMASK		(0x0084)
#define PACKET_F1_ADDRBASE		(0x0088)
#define PACKET_F1_WINDOWSIZE		(0x0090)
#define PACKET_F1_SECURITYMASK		(0x0098)
#define PACKET_F1_OPCODE		(0x009C)
#define PACKET_F1_STATUS		(0x00A0)
#define PACKET_F1_LENGTH		(0x00A4)
#define PACKET_F1_URGENCY		(0x00A8)
#define PACKET_F1_USERMASK		(0x00B0)
#define PACKET_COUNTERS_0_SRC		(0x0138)
#define PACKET_COUNTERS_0_ALARMMODE	(0x013C)
#define PACKET_COUNTERS_0_VAL		(0x0140)
#define PACKET_COUNTERS_1_SRC		(0x014C)
#define PACKET_COUNTERS_1_ALARMMODE	(0x0150)
#define PACKET_COUNTERS_1_VAL		(0x0154)

#define PACKET_COUNTERS_2_SRC		(0x0160)
#define PACKET_COUNTERS_2_ALARMMODE	(0x0164)
#define PACKET_COUNTERS_2_VAL		(0x0168)
#define PACKET_COUNTERS_3_SRC		(0x0174)
#define PACKET_COUNTERS_3_ALARMMODE	(0x0178)
#define PACKET_COUNTERS_3_VAL		(0x017c)
#define PACKET_STARTGO          (0x0028)
void enable_packet_probe_by_name(const char *name);
void disable_packet_probe_by_name(const char *name);
void config_packet_probe(const char *name, const struct packet_configration *packet_cfg);

void noc_packet_probe_hanlder(const struct noc_node *node, void __iomem *base);
void enable_packet_probe(const struct noc_node *node, void __iomem *base);
void disable_packet_probe(void __iomem *base);
void noc_set_bit(void __iomem *base, unsigned int offset, unsigned int bit);
void noc_clear_bit(void __iomem *base, unsigned int offset, unsigned int bit);
void init_packet_info(struct noc_node *node);

#endif
