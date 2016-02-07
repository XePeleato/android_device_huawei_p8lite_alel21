

#ifndef __HWIFI_NETDEV_H__
#define __HWIFI_NETDEV_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*
 * 1 Other Header File Including
 */

/*
 * 2 Macro Definition
 */
#define ETHER_HEADER_SIZE           14
#define ETHER_TYPE_MIN              0x0600
#define ETHER_TYPE_IP               0x0800
#define ETHER_TYPE_ARP              0x0806
#define ETHER_TYPE_8021Q            0x8100
#define ETHER_TYPE_802_1X           0x888e
#define ETHER_TYPE_802_1X_PREAUTH   0x88c7

/*
 * 3 Enum Type Definition
 */


/*
 * 4 Global Variable Declaring
 */


/*
 * 5 Message Header Definition
 */


/*
 * 6 Message Definition
 */


/*
 * 7 STRUCT Type Definition
 */
struct hi110x_device;
struct device;
struct net_device;


/* ehter header */
struct ether_header
{
    uint8   dmac[MAC_LEN];
    uint8   smac[MAC_LEN];
    uint16  ether_type;
} __attribute__((packed));

/* vlan tag ether header */
struct ethervlan_header
{
    uint8   dmac[MAC_LEN];
    uint8   smac[MAC_LEN];
    uint16  vlan_type;
    uint16  vlan_tag;
    uint16  ether_type;
} __attribute__((packed));

/*
 * 8 UNION Type Definition
 */


/*
 * 9 OTHERS Definition
 */


/*
 * 10 Function Declare
 */

extern struct net_device* hwifi_netdev_register(struct hi110x_device *hi110x_dev, struct device *dev);
extern void hwifi_netdev_unregister(struct net_device* ndev);
int32 hwifi_wlan_open(struct net_device *ndev);
int32 hwifi_wlan_close(struct net_device *ndev);

int32 hwifi_sm_power_handle(struct hi110x_device *hi110x_dev, int32 state, int32 p2p_powermgmt_flag);

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of hwifi_netdev.h */
