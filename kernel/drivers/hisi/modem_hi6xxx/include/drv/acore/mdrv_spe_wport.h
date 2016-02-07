
#ifndef __MDRV_SPE_WPORT_H__
#define __MDRV_SPE_WPORT_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <linux/if_ether.h>
#include <linux/netdevice.h>

/* ioctl cmd code */
#define SPE_WPORT_IOCTL_SET_RX_CB       0xF001
#define SPE_WPORT_IOCTL_SET_MAC         0xF002

/* macro tools */
#define mdrv_spe_set_skb_ipv4(skb)      ((skb)->protocol = htons(ETH_P_IP))
#define mdrv_spe_set_skb_ipv6(skb)      ((skb)->protocol = htons(ETH_P_IPV6))

/* used for ioctl cmd: SPE_WPORT_IOCTL_SET_RX_CB */
typedef void (*spe_wport_rx_cb_t)(int port, struct sk_buff *skb);

/* used for mdrv_spe_wport_open */
typedef struct spe_wport_attr
{
    int is_bypass_mode;
    int is_ipf_port;
    struct net_device* net_dev;
} spe_wport_attr_t;


int mdrv_spe_wport_open(spe_wport_attr_t *attr);

void mdrv_spe_wport_close(int port);

int mdrv_spe_wport_xmit(int port, struct sk_buff *skb);

void mdrv_spe_wport_rx_done(struct sk_buff *skb);

int mdrv_spe_wport_ioctl(int port, unsigned int cmd, void* param);

int mdrv_spe_usb_eth_is_bypass(int eth_id);


#ifdef __cplusplus
}
#endif
#endif/* __MDRV_SPE_WPORT_H__ */

