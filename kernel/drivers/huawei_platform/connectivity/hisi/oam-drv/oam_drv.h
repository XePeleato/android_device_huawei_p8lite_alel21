/*
 *
 * Copyright (C), 2001-2021, Huawei Tech. Co., Ltd.
 *
 * File Name     : oam_drv.h
 * Version	     : Initial Draft
 * Created	     : 2012/8/26
 * Last Modified :
 * Description   : oam_drv.c header file
 * Function List :
 * History 	     :
 * 1.Date		 : 2012/8/26
 *   Modification: Created file
 *
 */

#ifndef __OAM_DRV_H__
#define __OAM_DRV_H__

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

#define OAM_DEBUG(fmt, arg...) do { \
                printk(KERN_DEBUG "OAM::[%s:%d]"fmt"\n",__FUNCTION__,__LINE__, ##arg); \
            } while(0)

#define OAM_INFO(fmt, arg...) do { \
                printk(KERN_INFO "OAM::[%s:%d]"fmt"\n",__FUNCTION__,__LINE__,##arg); \
            } while(0)

#define OAM_WARNING(fmt, arg...) do { \
                printk(KERN_WARNING "OAM::[%s:%d]"fmt"\n",__FUNCTION__,__LINE__,##arg); \
            } while(0)

#define OAM_ERROR(fmt, arg...) do { \
                printk(KERN_ERR "OAM::[%s:%d]"fmt"\n",__FUNCTION__,__LINE__,##arg); \
            } while(0)

typedef unsigned char                       uint8;
typedef char                                int8;
typedef unsigned short                      uint16;
typedef signed short                        int16;
typedef unsigned int                        uint32;
typedef signed int                          int32;
typedef struct sk_buff                      oal_netbuf_stru;


/*Hi1101 and Hi1151 version micro definition*/
/* #define SDT_OAM_FOR_1151 */

/* string */
#define OS_MEM_SET(dst, data, size)         memset(dst, data, size)
#define OS_MEM_CPY(dst, src, size)          memcpy(dst, src, size)
#define OS_MEM_CMP(dst, src, size)          memcmp(dst, src, size)
#define OS_STR_CMP(dst, src)                strcmp(dst, src)
#define OS_STR_LEN(s)                       strlen(s)
#define OS_STR_CPY(dst, src)                strcpy(dst, src)
#define OS_STR_CAT(dst, src)                strcat(dst, src)
#define OS_STR_CHR(str, chr)                strchr(str, chr)

/* net */
#define OS_SKB_PUT(skb, len)                skb_put(skb, len)
#define OS_ALLOC_SKB(len, type)             alloc_skb(len, type)
#define OS_SKB_GET(skb)                     skb_get(skb)
#define OS_KFREE_SKB(skb)                   kfree_skb(skb)

/* netlink */
#define OS_NETLINK_CB(skb)                  NETLINK_CB(skb)
#define OS_NLMSG_DATA(nlh)                  NLMSG_DATA(nlh)
#define OS_NLMSG_SPACE(len)                 NLMSG_SPACE(len)
#define OS_NLMSG_HDR(pst_skb)               nlmsg_hdr(pst_skb)
#define OS_NLMSG_PAYLOAD(nlh, len)          NLMSG_PAYLOAD(nlh, len)
#define OS_NLMSG_DATA(nlh)                  NLMSG_DATA(nlh)
#define OS_SOCK_RELEASE(sock)               sock_release(sock)
#define OS_NETLINK_KERNEL_CREATE(net, unit, cfg)\
                netlink_kernel_create(net, unit, cfg)
#define OS_NETLINK_UNICAST(ssk, skb, pid, nonblock)\
                netlink_unicast(ssk, skb, pid, nonblock)
#define OS_NLMSG_PUT(skb, pid, seq, type, payload, flags)\
                nlmsg_put(skb, pid, seq, type, payload, flags)

#define OS_GFP_ATOMIC                       GFP_ATOMIC
#define OS_GFP_KERNEL                       GFP_KERNEL

#define OS_MSG_DONTWAIT                     MSG_DONTWAIT
#define OS_INIT_NET                         init_net
#define OS_THIS_MODULE                      THIS_MODULE

/* memory */
#define OS_MEM_KFREE(p)                     kfree(p)
#define OS_KMALLOC_GFP(size)                kmalloc(size, GFP_KERNEL)

#define NETLINK_TEST                        22
#define HCC_OAM_TEST                        0x06
#define DATA_BUF_LEN                        2048
#define SDTCMD_MAX_LEN                      100

/*head and end */
#define PACKET_START_SIGNAL         		(0x7e)
#define PACKET_END_SIGNAL           		(0x7e)
#define RESERVE                             (0x00)
#define LEN_LOW_BYTE(len)           		((uint8)(len & 0xff))
#define LEN_HIGH_BYTE(len)          		((uint8)((len & 0xff00)>>8))

#define RX_WIFI_QUE_MAX_NUM                 100
#define MAX_NUM                             256
#define HEAD_ADD_TAIL_LEN                   9
/*data type and primID pattern*/
#define LOG									0x01
#define MEM_READ_WRITE						0x09
#define REG_READ_WRITE						0x15
#define CONFIG_CMD                          0x0b

#define PC_TO_DEV_DATA						0x01
#define DEV_ACK								0x02
#define	DEV_ACTIVE_UPLOAD					0x03
#define OUTPUT_CONTENT						0x04
/*
 * 3 Enum Type Definition
 */
enum return_type
{
    SUCC = 0,
    EFAIL,
};

enum OM_NETLINK_MSG_TYPE_ENUM
{
    NETLINK_MSG_HELLO = 0,               /* netlink connect hello */
    NETLINK_MSG_SDTCMD,                  /* std with device */
    NETLINK_MSG_SDTCMD_OPS              /* device to SDT need encapsulation */
};

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
typedef struct oam_netlink_msg_hdr_stru
{
    uint32       cmd;
    uint32       len;
} OAM_NETLINK_MSG_HDR_ST;

typedef struct oam_header
{
    uint8    uc_flag;          /* 帧分隔符0X7E */
    uint8    uc_ftype;         /* 帧类型 */
    uint8    uc_dtype;         /* 通讯原语 */
    uint8    uc_a;             /* 保留 */
    uint16   us_len;           /* 长度 = 数据包长度 - 4 ( 数据包头尾(0x7E) + 长度本身*/
    uint16   us_num;           /* 序列号 */
}OAM_HEADER_STUR;

typedef struct
{
    uint32	ulMemAddr;
    uint16  usMemLen;
    uint8   ucMode;
    uint8   ucReserve;
}QUERY_MEM_MSG_STRU;

typedef struct oam_end
{
    uint8    uc_flag;          /* 帧分隔符0X7E */
}OAM_END_STUR;

typedef struct sk_buff                  OS_SK_BUFF_STRU;
typedef struct nlmsghdr                 OS_NLMSGHDR_STRU;
typedef struct sock                     OS_SOCK_STRU;

typedef struct 
{
	int32			ul_registered;
	int32			(* p_hwifi_down_mem_cmd_func)(uint8 *puc_buf, uint16 us_len);
    int32           (* p_hwifi_down_reg_cmd_func)(uint8 *puc_buf, uint16 us_len);
    int32           (* p_hwifi_down_cfg_cmd_func)(uint8 *puc_buf, uint16 us_len);
    int32 			(* p_hwifi_up_log_func)(oal_netbuf_stru *pst_skb);
    int32			(* p_hwifi_up_mem_data_func)(oal_netbuf_stru *pst_skb);
    int32           (* p_hwifi_up_reg_data_func)(oal_netbuf_stru *pst_skb);
	void            *context;

}oam_wifi_driver_tx_action_stru;

typedef int32  (* oal_sdt_func)(oal_netbuf_stru *pst_netbuf);

typedef struct oam_kernel_globals
{
    struct workqueue_struct                 *oam_rx_workqueue;
    struct work_struct                      rx_wifi_work;
    struct sk_buff_head                     rx_wifi_dbg_seq;
    OS_SOCK_STRU                            *pst_nlsk;
    uint8                                   *puc_data;
    uint32                                  ul_usepid;
	uint16						            sn_num;
}OAM_KERNEL_GLOBALS_STUR;

/*
 * 8 UNION Type Definition
 */


/*
 * 9 OTHERS Definition
 */


/*
 * 10 Function Declare
 */
extern void __exit exit_oamkernel(void);
extern int32 __init init_oamkernel(void);
extern int32 oamkernel_netlink_create( void );
extern void  oamkernel_netlink_recv (OS_SK_BUFF_STRU *__skb);
extern int32 oamkernel_netlink_send (uint8 *puc_data, int32 data_len);
extern int32 oamkernel_recv_data(int8 main_type, int8 sub_type, OS_SK_BUFF_STRU *pst_skb, void *param);
extern int32 oamkernel_send_data(uint8 main_type, uint8 sub_type, uint8 *puc_data, int32 data_len);
extern int32 oamkernel_send_data_to_driver(uint8 *puc_data, int32 data_len);

extern uint32  oam_sdt_register(
                oam_wifi_driver_tx_action_stru     **pst_tx_action,
                oal_sdt_func                        p_up_log_func,
                oal_sdt_func                        p_up_mem_data_func,
                oal_sdt_func                        p_up_reg_data_func);
extern uint32  oam_sdt_unregister(oam_wifi_driver_tx_action_stru **pst_tx_action);
extern int32 oamkernel_up_log( OS_SK_BUFF_STRU *pst_skb);
extern int32 oamkernel_up_reg_data( OS_SK_BUFF_STRU *pst_skb);
extern int32 oamkernel_up_mem_data(OS_SK_BUFF_STRU *pst_skb);

#ifdef __cplusplus
	#if __cplusplus
		}
	#endif
#endif

#endif /* end of oam_kernel.h */
