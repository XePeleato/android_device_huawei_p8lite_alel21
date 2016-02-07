/*
 ******************************************************************************
 *                     HEADER FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_wlan.h,v $ $Revision: 1.17 $
 *     $Source: /home/interpeak/CVSRoot/ipcom/include/ipcom_wlan.h,v $
 *     $Author: myakan $ $Date: 2008-08-13 19:56:14 $
 *     $State: Exp $ $Locker:  $
 *
 *     INTERPEAK_COPYRIGHT_STRING
 ******************************************************************************
 */
#ifndef IPCOM_WLAN_H
#define IPCOM_WLAN_H

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 * Wireless.
 */


/*
 ****************************************************************************
 * 2                    CONFIGURATION
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#include "ipcom_wifi.h"
#include "ipcom_netif.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

/* WLAN ioctls commands */
#define IP_SIOCSIWCOMMIT    IP_IO(WLAN, IP_SIWCOMMIT, W, Ipcom_iwreq)
#define IP_SIOCSIWFREQ      IP_IOW(WLAN, IP_SIWFREQ, W, Ipcom_iwreq)
#define IP_SIOCGIWFREQ      IP_IOR(WLAN, IP_GIWFREQ, R, Ipcom_iwreq)
#define IP_SIOCSIWMODE      IP_IOW(WLAN, IP_SIWMODE, W, Ipcom_iwreq)
#define IP_SIOCGIWMODE      IP_IOR(WLAN, IP_GIWMODE, R, Ipcom_iwreq)
#define IP_SIOCSIWAP        IP_IOW(WLAN, IP_SIWAP, W, Ipcom_iwreq)
#define IP_SIOCGIWAP        IP_IOR(WLAN, IP_GIWAP, R, Ipcom_iwreq)
#define IP_SIOCSIWSCAN      IP_IOW(WLAN, IP_SIWSCAN, W, Ipcom_iwreq)
#define IP_SIOCGIWSCAN      IP_IOR(WLAN, IP_GIWSCAN, R, Ipcom_iwreq_large_data)
#define IP_SIOCSIWESSID     IP_IOW(WLAN, IP_SIWESSID, W, Ipcom_iwreq_small_data)
#define IP_SIOCGIWESSID     IP_IOR(WLAN, IP_GIWESSID, R, Ipcom_iwreq_small_data)
#define IP_SIOCSIWENCRTYPE  IP_IOW(WLAN, IP_SIWENCRTYPE, W, Ipcom_iwreq)
#define IP_SIOCGIWENCRTYPE  IP_IOW(WLAN, IP_GIWENCRTYPE, R, Ipcom_iwreq)
#define IP_SIOCSIWAUTHTYPE  IP_IOW(WLAN, IP_SIWAUTHTYPE, W, Ipcom_iwreq)
#define IP_SIOCGIWAUTHTYPE  IP_IOR(WLAN, IP_GIWAUTHTYPE, R, Ipcom_iwreq)
#define IP_SIOCSIWENCRKEY   IP_IOW(WLAN, IP_SIWENCRKEY, W, Ipcom_iwreq_small_data)
#define IP_SIOCGIWRSC       IP_IOW(WLAN, IP_GIWRSC, R, Ipcom_iwreq_small_data)
#define IP_SIOCSIWRSC       IP_IOW(WLAN, IP_SIWRSC, W, Ipcom_iwreq_small_data)
#define IP_SIOCSIWRESET     IP_IOW(WLAN, IP_SIWRESET, W, Ipcom_iwreq)

#define IP_SIOCSIWATTACHFUNC IP_IOW(WLAN, IP_SIWATTACHFUNC, W, Ipcom_iwreq)
#define IP_SIOCSIWREMOVEFUNC IP_IOW(WLAN, IP_SIWREMOVEFUNC, W, Ipcom_iwreq)

#define IP_SIOCSIWOPENPORT   IP_IOW(WLAN, IP_SIWOPENPORT, W, Ipcom_iwreq)

#define IP_SIOCGIWQUALITY   IP_IOR(WLAN, IP_GIWQUALITY, R, Ipcom_iwreq_small_data)
#define IP_SIOCSIWENCODE    IP_IOW(WLAN, IP_SIWENCODE, W, Ipcom_iwreq_small_data)
#define IP_SIOCGIWENCODE    IP_IOW(WLAN, IP_GIWENCODE, R, Ipcom_iwreq_small_data)
#define IP_SIOCGIWMAC       IP_IOR(WLAN, IP_GIWMAC, R, Ipcom_iwreq_small_data)
#define IP_SIOCSIWMAC       IP_IOW(WLAN, IP_SIWMAC, W, Ipcom_iwreq_small_data)

#define IP_SIOCSIWDATA      IP_IOWR(WLAN, IP_SIWDATA, W, Ipcom_iwreq_large_data) /* Generic data transfer */
    
#define IP_SIOCSIWPOWER     IP_IOW(WLAN, IP_SIWPOWER, W, Ipcom_iwreq)
#define IP_SIOCGIWPOWER     IP_IOR(WLAN, IP_GIWPOWER, R, Ipcom_iwreq)

#define IP_SIOCSIWIDLE      IP_IOW(WLAN, IP_SIWIDLE, W, Ipcom_iwreq)
#define IP_SIOCGIWIDLE      IP_IOR(WLAN, IP_GIWIDLE, R, Ipcom_iwreq)

#define IP_SIOCIWFIRSTPRIV  100


/* IP_SIOCSIWMODE: Modes of operation */
#define IP_IW_MODE_AUTO	    0                   /* Let the driver decide */
#define IP_IW_MODE_ADHOC	1	                /* Single cell network */
#define IP_IW_MODE_INFRA	2	                /* Multi cell network, roaming, ... */
#define IP_IW_MODE_MASTER	3	                /* Synchronisation master or Access Point */

/* End marker in arrays */
#define IP_IW_END 0

/* Frequency flags */
#define IP_IW_FREQ_AUTO		0x00	        /* Let the driver decides */
#define IP_IW_FREQ_FIXED		0x01	        /* Force a specific value */

/* IW_AUTH_80211_AUTH_ALG values */
#define IP_IW_AUTH_ALG_OPEN_SYSTEM	        1
#define IP_IW_AUTH_ALG_SHARED_KEY	        2
#define IP_IW_AUTH_ALG_WPA_PSK  	        3
#define IP_IW_AUTH_ALG_WPA  	            4
#define IP_IW_AUTH_ALG_WPA2_PSK  	        5
#define IP_IW_AUTH_ALG_WPA2  	            6

/* IP_SIOCSIWENCRTYPE: */
#define IP_IW_ENCR_ALG_NONE	            1
#define IP_IW_ENCR_ALG_WEP	            2
#define IP_IW_ENCR_ALG_TKIP	            3
#define IP_IW_ENCR_ALG_CCMP	            4


#define IPCOM_WLAN_MAX_KEY_SIZE 32
#define IPCOM_WLAN_MAX_WEP_KEY_SIZE 13

#define IP_IW_KEY_FLAG_STA_KEY 1
#define IP_IW_KEY_FLAG_AP_KEY  2
#define IP_IW_KEY_FLAG_NOKEY   4

#define IP_IW_ENCODE_FLAGS 0xff00
#define IP_IW_ENCODE_DISABLED 0x8000	/* Encoding disabled */
#define IP_IW_ENCODE_OPEN 0x2000
#define IP_IW_ENCODE_NOKEY	0x0800

#define IWREQ_UNION_SIZE (64)
/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

typedef struct Ipcom_wlan_func_st
{
    int (*link_input)(Ipcom_netif *, Ipcom_pkt *);

} Ipcom_wlan_func;

typedef struct Ipcom_iw_point
{
    void *pointer;        /* Pointer to the data  (in user space) */
    Ip_u16 length;        /* number of fields or size in bytes */
    Ip_u16 flags;         /* Optional params */

} Ipcom_iw_point;

typedef struct Ipcom_iw_param
{
    Ip_s32  value;
    Ip_u8   fixed;		/* Hardware should not use auto select */
    Ip_u8   disabled;	/* Disable the feature */
    Ip_u16  flags;		/* Various specific flags (if any) */

} Ipcom_iw_param;

typedef struct Ipcom_iw_quality
{
    Ip_u8   qual;       /* link quality (%retries, SNR, %missed beacons or better...) */
    Ip_s8   level;      /* signal level (dBm) */
    Ip_u8   noise;      /* noise level (dBm) */
    Ip_u8   updated;    /* Flags to know if updated */
    Ip_u8   bssid[6];   /* BSSID to request quality values for, zero => the currently associated */

} Ipcom_iw_quality;

typedef struct Ipcom_iw_key_rsc_st
{
    Ip_u8 rsc[8];
    Ip_u32 index;
    Ip_bool tx;    /* 1 for transmit counter, 0 for receive */
    Ip_u8 addr[6];
} Ipcom_iw_key_rsc;

typedef struct Ipcom_iw_key_st
{
    Ip_u32 type;
    Ip_u32 flags;
    Ip_u32 index;
    int len;
    Ip_u8 key[IPCOM_WLAN_MAX_KEY_SIZE];
    Ip_u8 addr[6];
    Ip_bool key_rsc_set; /* set to 1 if key_rsc included */
    Ipcom_iw_key_rsc key_rsc;
} Ipcom_iw_key;

typedef struct Ipcom_iw_freq_st
{
    Ip_s32 m;
    Ip_s16 e;
    Ip_u8 i;
    Ip_u8 flags;

} Ipcom_iw_freq;

typedef union Ipcom_iwreq_data_u
{
    Ip_u32 encr_type;
    Ip_u32 auth_type;
    Ipcom_iw_key key;
    Ipcom_iw_key_rsc key_rsc;
    struct Ip_sockaddr addr;
    Ipcom_iw_point essid;
    Ipcom_iw_point encode;
    Ipcom_iw_param param;
    Ipcom_iw_point data;
    Ip_u32 mode;
    Ipcom_iw_freq freq;
    Ipcom_iw_quality quality;
    Ip_u8 fixed_size[IWREQ_UNION_SIZE]; /* RIME: make 'iwreq' and 'Ipcom_iwreq' same size */
    Ip_u8 macAddr[6];

} Ipcom_iwreq_data;

typedef struct iwreq_s
{
    char if_name[IP_IFNAMSIZ];
    Ipcom_iwreq_data u;

} Ipcom_iwreq;


#define MAX_IWREQ_SMALL_DATA (50)
#define MAX_IWREQ_LARGE_DATA (4000)

typedef struct iwreq_small_data_s
{
    char if_name[IP_IFNAMSIZ];
    Ipcom_iwreq_data u;
    char data[MAX_IWREQ_SMALL_DATA];

} Ipcom_iwreq_small_data;

typedef struct iwreq_large_data_s
{
    char if_name[IP_IFNAMSIZ];
    Ipcom_iwreq_data u;
    char data[MAX_IWREQ_LARGE_DATA];

} Ipcom_iwreq_large_data;


/* Wireless IOCTL used in Ipcom_iw_event */
#define SIOCGIWAP       0x8B15
#define SIOCGIWESSID    0x8B1B
#define SIOCGIWMODE     0x8B07
#define SIOCGIWFREQ     0x8B05
#define SIOCGIWENCODE   0x8B2B
#define SIOCGIWRATE     0x8B21
#define SIOCGIWSPY	    0x8B11

typedef struct iw_event_s
{
	Ip_u16		len;			/* Real lenght of this stuff */
	Ip_u16		cmd;			/* Wireless IOCTL */
	Ipcom_iwreq_data u;		    /* IOCTL fixed payload */

} Ipcom_iw_event;


/* Size of the Event prefix (including padding and alignement junk) */
#define IP_IW_EV_LCP_LEN	(sizeof(Ipcom_iw_event) - sizeof(Ipcom_iwreq_data))
/* Size of the various events */
#define IP_IW_EV_CHAR_LEN	(IP_IW_EV_LCP_LEN + IFNAMSIZ)
#define IP_IW_EV_UINT_LEN	(IP_IW_EV_LCP_LEN + sizeof(Ip_u32))
#define IP_IW_EV_FREQ_LEN	(IP_IW_EV_LCP_LEN + sizeof(Ipcom_iw_freq))
#define IP_IW_EV_POINT_LEN	(IP_IW_EV_LCP_LEN + sizeof(Ipcom_iw_point))
#define IP_IW_EV_PARAM_LEN	(IP_IW_EV_LCP_LEN + sizeof(Ipcom_iw_param))
#define IP_IW_EV_ADDR_LEN	(IP_IW_EV_LCP_LEN + sizeof(struct Ip_sockaddr))
#define IP_IW_EV_QUAL_LEN	(IP_IW_EV_LCP_LEN + sizeof(Ipcom_iw_quality))



#ifdef __cplusplus
}
#endif

#endif

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

