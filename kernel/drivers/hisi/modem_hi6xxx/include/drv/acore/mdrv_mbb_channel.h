#ifndef __MDRV_ACORE_MBB_CHANNEL_H__
#define __MDRV_ACORE_MBB_CHANNEL_H__

#define NETLINK_HW_LOGCAT   29

typedef enum _DEVICE_ID
{	
	DEVICE_ID_WAN = 11,       // WAN module = 11 
	DEVICE_ID_MAX	
} DEVICE_ID;	

// Define the device event structure (driver => APP) 
typedef struct _DEVICE_EVENT	
{
	DEVICE_ID device_id;
	int event_code;
	int len;
	char data[0];
} DEVICE_EVENT;

// Define the device event handler (APP => driver) 
typedef int (*device_event_dispatch)(void *data, int len);

// Export interface to other module

static inline int device_event_report(void *data, int len)
{
	return 0;
}


#endif

