#include <linux/module.h>
#include <linux/init.h>
#include <wlioctl.h>
#include <linux/string.h>
#include <linux/errno.h>
#include <hw_wifi.h>
#include "dhd_dbg.h"
#include <wl_cfg80211.h>
#ifdef HW_WIFI_WAKEUP_SRC_PARSE
#include <linux/ip.h>
#include <net/tcp.h>
#include <net/udp.h>
#include <net/icmp.h>
#include <linux/ieee80211.h>
#include <wl_android.h>
#include <proto/bcmevent.h>
#include <bcmendian.h>
#include <linux/types.h>
#include <linux/skbuff.h>
#include <linux/ipv6.h>
#endif

#define HW_BCN_TIMEOUT 10

#if defined(CONFIG_BCM4343)
    #include <hw_country_code_4343.h>
#elif defined(CONFIG_BCM4339)
    #include <hw_country_code_4339.h>
#else
    #include <hw_country_code.h>
#endif //end CONFIG_BCM4343

#ifdef HW_WIFI_WAKEUP_SRC_PARSE
#define MAX_MSG_LENGTH 30
volatile bool g_wifi_firstwake = FALSE;
#define IPADDR(addr) \
        ((unsigned char*)&addr)[0], \
        ((unsigned char*)&addr)[1], \
        ((unsigned char*)&addr)[2], \
        ((unsigned char*)&addr)[3]

#define IPV6_ADDRESS_SIZEINBYTES 0x10
#define IPV6_DESTOPTS_HDR_OPTIONSIZE 0x8

struct ieee8021x_hdr {
	u8 version;
	u8 type;
	__be16 length;
};

typedef struct IPV6RoutingHeaderFormatTag
{
	uint8_t ucNextHeader;
	uint8_t ucRoutingType;
	uint8_t ucNumAddresses;
	uint8_t ucNextAddress;
	uint32_t ulReserved;
	//uint8_t aucAddressList[0];

}IPV6RoutingHeader;

typedef struct IPV6FragmentHeaderFormatTag
{
	uint8_t ucNextHeader;
	uint8_t ucReserved;
	uint16_t usFragmentOffset;
	uint32_t  ulIdentification;
}IPV6FragmentHeader;

typedef struct IPV6DestOptionsHeaderFormatTag
{
	uint8_t ucNextHeader;
	uint8_t ucHdrExtLen;
	uint8_t ucDestOptions[6];
	//uint8_t udExtDestOptions[0];
}IPV6DestOptionsHeader;

typedef struct IPV6HopByHopOptionsHeaderFormatTag
{
	uint8_t ucNextHeader;
	uint8_t ucMisc[3];
	uint32_t ulJumboPayloadLen;
}IPV6HopByHopOptionsHeader;

typedef struct IPV6AuthenticationHeaderFormatTag
{
	uint8_t ucNextHeader;
	uint8_t ucLength;
	uint16_t usReserved;
	uint32_t  ulSecurityParametersIndex;
	//uint8_t  ucAuthenticationData[0];

}IPV6AuthenticationHeader;
#endif

/* Customized Locale convertor
*  input : ISO 3166-1 country abbreviation
*  output: customized cspec
*/
void get_customized_country_code_for_hw(char *country_iso_code, wl_country_t *cspec)
{
	int size, i;

	printk(KERN_ERR "enter : %s.\n", __FUNCTION__);
	size = ARRAYSIZE(hw_translate_custom_table);

	if (cspec == 0)
		 return;

	if (size == 0)
		 return;

	printk(KERN_ERR "input country code: %s.\n", country_iso_code);
	for (i = 0; i < size; i++) {
		if (strcmp(country_iso_code, hw_translate_custom_table[i].iso_abbrev) == 0) {
			memcpy(cspec->ccode, hw_translate_custom_table[i].custom_locale, WLC_CNTRY_BUF_SZ);
			memcpy(cspec->country_abbrev, hw_translate_custom_table[i].custom_locale, WLC_CNTRY_BUF_SZ);
			cspec->rev = hw_translate_custom_table[i].custom_locale_rev;
			printk(KERN_ERR "output country code: %s, ver: %d.\n", cspec->ccode, cspec->rev);
			return;
		}
	}
	
	return;
}


uint hw_get_bcn_timeout(void)
{
	return HW_BCN_TIMEOUT;
}

static struct dsm_dev dsm_wifi = {
    .name = "dsm_wifi",
	.device_name = NULL,
	.ic_name = NULL,
	.module_name = NULL,
    .fops = NULL,
    .buff_size = 1024,
};

struct dsm_client *wifi_dsm_client = NULL;

void hw_register_wifi_dsm_client(void) {
    if(NULL == wifi_dsm_client) {
        wifi_dsm_client = dsm_register_client(&dsm_wifi);
    }
}

#define LOG_BUF_SIZE 512
void hw_wifi_dsm_client_notify(int dsm_id, const char *fmt, ...) {
    char buf[LOG_BUF_SIZE] = {0};
    va_list ap;

    va_start(ap, fmt);
    if(wifi_dsm_client && !dsm_client_ocuppy(wifi_dsm_client)) {
        if(fmt) {
            vsnprintf(buf, LOG_BUF_SIZE, fmt, ap);
            dsm_client_record(wifi_dsm_client, buf);
        }
        dsm_client_notify(wifi_dsm_client, dsm_id);
        printk(KERN_INFO "wifi dsm_client_notify success\n");
    } else {
        printk(KERN_INFO "wifi dsm_client_notify failed\n");
    }
    va_end(ap);
}

extern volatile bool hw_cfg80211_suspend;
static volatile int g_need_enable_intr = 0;
extern dhd_pub_t *hw_get_dhd_pub(struct net_device *dev);

static int wait_for_cfg80211_resume(void) {
	int wait_count = 10;
	while(wait_count > 0) {
		msleep(20);
		if(!hw_cfg80211_suspend) {
			HW_PRINT((WIFI_TAG"%s %d\n", __func__, wait_count));
			return wait_count;
		}
		wait_count--;
	}
	return 0;
}

/**
 * This function used in dhd_dpc_thread(dhd_linux.c)
 * when sdio host is in suspend state, skip this action in dpc thread
 * @return: 1 skip, 0 dont skip
 */
int hw_skip_dpc_in_suspend(void) {
	if((!hw_cfg80211_suspend) || (wait_for_cfg80211_resume() > 0)) {
		g_need_enable_intr = 0;
		return 0;
	} else {
		g_need_enable_intr = 1;
		HW_PRINT((WIFI_TAG"%s skip, cfg80211 is in suspend state\n", __func__));
		return 1;
	}
}

/**
 * This function used in wl_cfg80211_resume(wl_cfg80211.c)
 * when dpc thread is interrupted by suspend(see hw_skip_dpc_in_suspend), resched dpc again
 */
void hw_resched_dpc_ifneed(struct net_device *ndev) {
	dhd_pub_t *pub = NULL;
	if(g_need_enable_intr) {
		g_need_enable_intr = 0;
		pub = hw_get_dhd_pub(ndev);
		if(pub && pub->up) {
			HW_PRINT((WIFI_TAG"%s, resched dpc\n", __func__));
			dhd_sched_dpc(pub);
		}
	}
}

#ifdef HW_PATCH_DISABLE_TCP_TIMESTAMPS
/*  sysctl_tcp_timestamps is defined in net/ipv4/tcp_input.c
 *  here to check wlan0 network interface and
 *  when wlan0 is connected, try to disable tcp_timestamps by set it to 0.
 *  and when wlan0 is disconnected, restore to enable tcp_timestamps.
 */
static int hw_dhd_wan0_connected = 0; /* 1: wlan0 connected*/
extern int sysctl_tcp_timestamps;
void hw_set_connect_status(struct net_device *ndev, int status)
{
    struct wireless_dev *wdev = NULL;

    if (NULL == ndev) {
        printk("interface is null, skip set status.\n");
        return;
    }

    wdev = ndev_to_wdev(ndev);
    if (NL80211_IFTYPE_STATION != wdev->iftype) {
        printk("interface type is %d, skip set sta status.\n", wdev->iftype);
        return;
    }

    hw_dhd_wan0_connected = status;
    /* if wlan0 disconnect, and tcp_ts is 0, restore it to 1 */
    if (0 == hw_dhd_wan0_connected && 0 == sysctl_tcp_timestamps) {
        sysctl_tcp_timestamps = 1;
        printk("wlan0 disconnected, restore tcp_timestamps.\n");
    }
}
#endif

/* this function should only be called when tcp_timestamps err in ipv4/tcp_input.c */
void hw_dhd_check_and_disable_timestamps(void)
{
#ifdef HW_PATCH_DISABLE_TCP_TIMESTAMPS
    /* disable tcp_timestamps to 0 only when wlan0 connected.*/
    printk("check wlan0 connect status = %d.\n", hw_dhd_wan0_connected);
    if (hw_dhd_wan0_connected) {
        sysctl_tcp_timestamps = 0;
        printk("wlan0 connected, disable tcp_timestamps.\n");
    }
#endif
}

#ifdef HW_WIFI_WAKEUP_SRC_PARSE
/***************************************************************************
*Function: 	wlan_send_nl_event
*Description: 	send the port number to the userspace use uevent.
*Input: 		struct net_device *net_dev: dhd net device.
			u16 port: port number.
*Output: 	null
*Return:		null
***************************************************************************/
static void wlan_send_nl_event(struct net_device *net_dev,  u16 port)
{
	struct device* dev = NULL;
	char *uevent[2];
	char msg[MAX_MSG_LENGTH];

	dev = &(net_dev->dev);
	memset(msg, 0, sizeof(msg));
	snprintf(msg, sizeof(msg), "WIFI_WAKE_PORT=%d",port);
	printk("%s: send msg: %s\n", __FUNCTION__, msg);
	uevent[0] = msg;
	uevent[1] = NULL;
	kobject_uevent_env(&(dev->kobj), KOBJ_CHANGE, (char**)&uevent);

	return;
}

/***************************************************************************
*Function: 	parse_ipv4_packet
*Description: 	if the packet is tcp/ip type, print ip type, ip address, ip port.
*Input: 		struct sk_buff *skb
*Output: 	null
*Return:		null
***************************************************************************/
static void parse_ipv4_packet(struct sk_buff *skb)
{
	const struct iphdr *iph;
	int iphdr_len = 0;
	struct tcphdr *th;
	struct udphdr *uh;
	struct icmphdr *icmph;

	printk("receive ipv4 packet.\n");
	iph = (struct iphdr *)skb->data;
	iphdr_len = iph->ihl*4;

	printk("src ip:%d.%d.%d.%d, dst ip:%d.%d.%d.%d\n", IPADDR(iph->saddr), IPADDR(iph->daddr));
	if (iph->protocol == IPPROTO_UDP){
		uh = (struct udphdr *)(skb->data + iphdr_len);
		printk("receive UDP packet, src port:%d, dst port:%d.\n", htons(uh->source), htons(uh->dest));
		wlan_send_nl_event(skb->dev, uh->dest);
	}else if(iph->protocol == IPPROTO_TCP){
		th = (struct tcphdr *)(skb->data + iphdr_len);
		printk("receive TCP packet, src port:%d, dst port:%d.\n", htons(th->source), htons(th->dest));
		wlan_send_nl_event(skb->dev, th->dest);
	}else if(iph->protocol == IPPROTO_ICMP){
		icmph = (struct icmphdr *)(skb->data + iphdr_len);
		printk("receive ICMP packet, type(%d):%s, code:%d.\n", icmph->type,
			((icmph->type == 0)?"ping reply":((icmph->type == 8)?"ping request":"other icmp pkt")), icmph->code);
	}else if(iph->protocol == IPPROTO_IGMP){
		printk("receive IGMP packet.\n");
	}else{
		printk("receive other IPv4 packet.\n");
	}

	return;
}

void dump_ipv6_addr(unsigned short *addr)
{
	int i =0;

	for (i = 0; i < (IPV6_ADDRESS_SIZEINBYTES/2); i++) {
		printk(":%lx", ntohs(addr[i]));
	}
	printk("\n");
}

static uint8_t *get_next_ipv6_chain_header(uint8_t **headerscan, uint8_t *headtype, int8_t *done, uint16_t *payload_len)
{
	uint16_t next_header_offset = 0;
	uint8_t * payload_ptr = *headerscan;
	uint8_t * return_header_ptr = *headerscan;

	if(headerscan == NULL || (*payload_len == 0) || (*done)){
		return NULL;
	}
	*done = 0;

	switch(*headtype){
	case NEXTHDR_HOP:
		{
			printk("IPv6 HopByHop Header.\n");
			next_header_offset += sizeof(IPV6HopByHopOptionsHeader);
		}
		break;
	case NEXTHDR_ROUTING:
		{
			IPV6RoutingHeader *pstIpv6RoutingHeader;
			printk("IPv6 Routing Header\n");
			pstIpv6RoutingHeader = (IPV6RoutingHeader *)payload_ptr;
			next_header_offset += sizeof(IPV6RoutingHeader);
			next_header_offset += pstIpv6RoutingHeader->ucNumAddresses * IPV6_ADDRESS_SIZEINBYTES;
		}
		break;
	case NEXTHDR_FRAGMENT:
		{
			printk("IPv6 Fragmentation Header\n");
			next_header_offset += sizeof(IPV6FragmentHeader);
		}
		break;
	case NEXTHDR_DEST:
		{
			IPV6DestOptionsHeader *pstIpv6DestOptsHdr = (IPV6DestOptionsHeader *)payload_ptr;
			int nTotalOptions = pstIpv6DestOptsHdr->ucHdrExtLen;
			printk("IPv6 DestOpts Header Header\n");
			next_header_offset += sizeof(IPV6DestOptionsHeader);
			next_header_offset += nTotalOptions * IPV6_DESTOPTS_HDR_OPTIONSIZE ;
		}
		break;
	case NEXTHDR_AUTH:
		{
			IPV6AuthenticationHeader *pstIpv6AuthHdr = (IPV6AuthenticationHeader *)payload_ptr;
			int nHdrLen = pstIpv6AuthHdr->ucLength;
			printk("IPv6 Authentication Header\n");
			next_header_offset += nHdrLen * 4;
		}
		break;
	case NEXTHDR_TCP:
	case NEXTHDR_UDP:
		{
			printk("tcp/udp/icmp Header: %d.\n", *headtype);
			*done = 1;
		}
		break;
	case NEXTHDR_ICMP:
		{
			printk("icmp Header: %d.\n", *headtype);
			*done = 1;
		}
		break;
	default:
		*done = 1;
		break;
	}

	if (*done == 0) {
		if (*payload_len <= next_header_offset) {
			*done = TRUE;
		} else {
			*headtype = *payload_ptr;
			payload_ptr += next_header_offset;
			(*payload_len) -= next_header_offset;
		}
	}

	*headerscan = payload_ptr;
	return return_header_ptr;
}

static uint8_t get_ipv6_protocal_ports(uint8_t *payload, uint16_t payload_len, uint8_t next_header, uint16_t *src_port, uint16_t *des_port)
{
	int8_t done = 0;
	uint8_t *headerscan = payload;
	uint8_t *payload_header = NULL;
	uint8_t headtype;

	if(!payload || payload_len == 0)
		return;

	headtype = next_header;
	while(!done){
		payload_header = get_next_ipv6_chain_header(&headerscan, &headtype, &done, &payload_len);
		if(done){
			if(headtype == NEXTHDR_TCP || headtype == NEXTHDR_UDP){
				*src_port = *((uint16_t *)payload_header);
				*des_port = *((uint16_t *)(payload_header+2));
				printk("src_port:0x%x, des_port:0x%x.\n", ntohs(*src_port), ntohs(*des_port));
			}
			break;
		}
	}
}

static void parse_ipv6_packet(struct sk_buff *skb)
{
	struct ipv6hdr *nh;
	uint16_t src_port;
	uint16_t des_port;
	uint8_t *payload;

	nh = (struct ipv6hdr *)skb->data;
	printk("version: %d, payload length: %d, nh->nexthdr: %d. \n", nh->version, ntohs(nh->payload_len), nh->nexthdr);
	printk("ipv6 src addr: ");
	dump_ipv6_addr((unsigned short *)&(nh->saddr));
	printk("ipv6 dst addr: ");
	dump_ipv6_addr((unsigned short *)&(nh->daddr));
	payload = nh + sizeof(struct ipv6hdr);

	get_ipv6_protocal_ports(payload, nh->payload_len, nh->nexthdr, &src_port, &des_port);

	return;
}

/***************************************************************************
*Function: 	parse_arp_packet
*Description: 	if the packet if 802.11 type, print the type name and sub type name.
*Input: 		struct sk_buff *skb
*Output: 	null
*Return:		null
***************************************************************************/
static void parse_arp_packet(struct sk_buff *skb)
{
	const struct iphdr *iph;
	int iphdr_len = 0;
	struct arphdr *arp;

	iph = (struct iphdr *)skb->data;
	iphdr_len = iph->ihl*4;
	arp = (struct arphdr *)(skb->data + iphdr_len);
	printk("receive ARP packet, hardware type:%d, protocol type:%d, opcode:%d.\n", ntohs(arp->ar_hrd), ntohs(arp->ar_pro), ntohs(arp->ar_op));

	return;
}

/***************************************************************************
*Function: 	parse_8021x_packet
*Description:	if the packet if 802.1x type, print the type name and sub type name.
*Input:		struct sk_buff *skb
*Output: 	null
*Return:		null
***************************************************************************/
static void parse_8021x_packet(struct sk_buff *skb)
{
	struct ieee8021x_hdr *hdr = (struct ieee8021x_hdr *)(skb->data);

	printk("receive 802.1x frame: version:%d, type:%d, length:%d\n", hdr->version, hdr->type, htons(hdr->length));

	return;
}


/***************************************************************************
*Function: 	parse_packet
*Description: 	parse the packet from sdio when system waked up by wifi. identify the packet type.
*Input: 		struct sk_buff *skb
*Output: 	null
*Return:		null
***************************************************************************/
void parse_packet(struct sk_buff *skb)
{
	__be16 type;

	type = skb->protocol;
	printk(WIFI_WAKESRC_TAG"protocol type:0x%04x\n", ntohs(type));

	if(type == cpu_to_be16(ETH_P_IP)){
		parse_ipv4_packet(skb);
	}else if (type == cpu_to_be16(ETH_P_IPV6)){
		printk("receive ipv6 packet.\n");
		parse_ipv6_packet(skb);
	}else if(type == cpu_to_be16(ETH_P_ARP)){
		parse_arp_packet(skb);
	}else if(type == cpu_to_be16(ETHER_TYPE_BRCM)){ //same as ETH_P_LINK_CTL
		printk("receive bcm cust packet."); //bcm cust packet is bcm event.
	}else if(type == cpu_to_be16(ETH_P_PAE)){
		parse_8021x_packet(skb);
	}else{
		printk("receive other packet.\n");
	}

	return;
}
#endif


