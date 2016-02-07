#define IP_SIWCOMMIT 0
#define IP_SIWFREQ 1
#define IP_GIWFREQ 2
#define IP_SIWMODE 3
#define IP_GIWMODE 4
#define IP_SIWAP 5
#define IP_GIWAP 6
#define IP_SIWSCAN 7
#define IP_GIWSCAN 8
#define IP_SIWESSID 9
#define IP_GIWESSID 10
#define IP_SIWENCRTYPE 11
#define IP_GIWENCRTYPE 12
#define IP_SIWAUTHTYPE 13
#define IP_GIWAUTHTYPE 14
#define IP_SIWENCRKEY 15
#define IP_GIWRSC 16
#define IP_SIWRSC 17
#define IP_SIWRESET 18
#define IP_SIWATTACHFUNC 19
#define IP_SIWREMOVEFUNC 20
#define IP_SIWOPENPORT 21
#define IP_GIWQUALITY 22
#define IP_SIWENCODE 23
#define IP_GIWENCODE 24
#define IP_GIWMAC 25
#define IP_SIWMAC 26
#define IP_SIWDATA 27
#define IP_SIWPOWER 28
#define IP_GIWPOWER 29
#define IP_SIWIDLE 30
#define IP_GIWIDLE 31

#define IP_WLAN_CMD (0x88000000)
#define IP_IS_WLAN_CMD(a) (((a) & (IP_WLAN_CMD)) == (IP_WLAN_CMD))

#define IP_WLAN_SIZE(a) ((a) & 0x3fff)
#define IP_WLAN_CODE(a) (((a) >> 16) & 0xff)

typedef struct Ipcom_dev_data_s
{
    char macaddr[6];

} Ipcom_dev_data_t;
