

#ifndef _TC_NS_CLIENT_H_
#define _TC_NS_CLIENT_H_

typedef struct {
    unsigned int method;
    unsigned int mdata;
} TC_NS_ClientLogin;

typedef union {
    struct {
        unsigned int *buffer;
        unsigned int offset;
        unsigned int *size_addr;
    } memref;
    struct {
        unsigned int *a_addr;
        unsigned int *b_addr;
    } value;
} TC_NS_ClientParam;

typedef struct {
    unsigned int code;
    unsigned int origin;
} TC_NS_ClientReturn;

typedef struct {
    unsigned char uuid[16];
    unsigned int session_id;
    unsigned int cmd_id;
    TC_NS_ClientReturn returns;
    TC_NS_ClientLogin login;
    TC_NS_ClientParam params[4];
    unsigned int paramTypes;
    bool started;
} TC_NS_ClientContext;

#define IMG_LOAD_FIND_NO_DEV_ID  0xFFFF00A5
#define IMG_LOAD_FIND_NO_SHARE_MEM 0xFFFF00A6
#define IMG_LOAD_SECURE_RET_ERROR 0xFFFF00A7

#define TST_CMD_01 (1)
#define TST_CMD_02 (2)
#define TST_CMD_03 (3)
#define TST_CMD_04 (4)
#define TST_CMD_05 (5)

#define TC_NS_CLIENT_IOCTL_SES_OPEN_REQ \
    _IOW(TC_NS_CLIENT_IOC_MAGIC, 1, TC_NS_ClientContext)
#define TC_NS_CLIENT_IOCTL_SES_CLOSE_REQ \
    _IOWR(TC_NS_CLIENT_IOC_MAGIC, 2, TC_NS_ClientContext)
#define TC_NS_CLIENT_IOCTL_SEND_CMD_REQ \
    _IOWR(TC_NS_CLIENT_IOC_MAGIC, 3, TC_NS_ClientContext)
#define TC_NS_CLIENT_IOCTL_SHRD_MEM_RELEASE \
    _IOWR(TC_NS_CLIENT_IOC_MAGIC, 4, unsigned int)
#define TC_NS_CLIENT_IOCTL_WAIT_EVENT \
    _IOWR(TC_NS_CLIENT_IOC_MAGIC, 5, unsigned int)
#define TC_NS_CLIENT_IOCTL_SEND_EVENT_REPONSE \
    _IOWR(TC_NS_CLIENT_IOC_MAGIC, 6, unsigned int)
#define TC_NS_CLIENT_IOCTL_REGISTER_AGENT \
    _IOWR(TC_NS_CLIENT_IOC_MAGIC, 7, unsigned int)
#define TC_NS_CLIENT_IOCTL_UNREGISTER_AGENT \
    _IOWR(TC_NS_CLIENT_IOC_MAGIC, 8, unsigned int)
#define TC_NS_CLIENT_IOCTL_LOAD_APP_REQ \
    _IOWR(TC_NS_CLIENT_IOC_MAGIC, 9, TC_NS_ClientContext)
#define TC_NS_CLIENT_IOCTL_NEED_LOAD_APP \
    _IOWR(TC_NS_CLIENT_IOC_MAGIC, 10, TC_NS_ClientContext)
#define TC_NS_CLIENT_IOCTL_LOAD_APP_EXCEPT \
	_IOWR(TC_NS_CLIENT_IOC_MAGIC, 11, unsigned int)
#define TC_NS_CLIENT_IOCTL_ALLOC_EXCEPTING_MEM \
        _IOWR(TC_NS_CLIENT_IOC_MAGIC, 12, unsigned int)
#define TC_NS_CLIENT_IOCTL_CANCEL_CMD_REQ \
        _IOWR(TC_NS_CLIENT_IOC_MAGIC, 13, TC_NS_ClientContext)
#define TC_NS_CLIENT_IOCTL_LOGIN \
        _IOWR(TC_NS_CLIENT_IOC_MAGIC, 14, int)
#define TC_NS_CLIENT_IOCTL_TST_CMD_REQ \
        _IOWR(TC_NS_CLIENT_IOC_MAGIC, 15, int)
#endif
