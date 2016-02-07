

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
/*
 * 1 Header File Including
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/sched.h>
#include <net/sock.h>
#include <net/netlink.h>
#ifdef CONFIG_HWCONNECTIVITY
#include <linux/huawei/hw_connectivity.h>
#include <linux/mutex.h>
#endif
#include "oam_drv.h"

/*
 * 2 Global Variable Definition
 */

OAM_KERNEL_GLOBALS_STUR             gst_kerenlglobal;
oam_wifi_driver_tx_action_stru     *tx_action = NULL;  

/* wifi driver supply interface for Hi1101 */
/*********************************************************************************/
/* SDIO TX interface */
extern int hwifi_send(uint8 main_type, char sub_type, struct sk_buff *skb);

/* alloc skb interface for oam and test sdio */
extern struct sk_buff *hwifi_alloc_skb(unsigned int len);

/* free skb interface for oam and test sdio */
extern void hwifi_free_skb(struct sk_buff *skb);

/* register callback for test RX */
extern int hwifi_rx_extern_register(uint8 main_type,  int32 (*rx_cb)(int8 main_type, int8 sub_type, struct sk_buff *skb, void *param), void *param);

/* unregister interface */
extern void hwifi_rx_extern_unregister(unsigned char main_type);
/**********************************************************************************/

#ifdef SDT_OAM_FOR_1151
/*
 * Prototype    : wifi_tx_add_head 
 * Description  : add packet head to recv buf from driver.
 * input        : buf  -> ptr of buf
 *                uc_ftype -> packet type£¬example log or  reg value
 *                lenth-> packet length
 * output       : not
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/6/20
 *     Author       : 
 *     Modification : Created function
 *
 */
int32 wifi_tx_add_head(uint8 *buf, uint8 uc_ftype, uint8 dtype, uint16 lenth)
{
    int8 *ptr;
    uint16 len;

    if (NULL == buf)
    {
        OAM_ERROR("buffer is NULL");
        return -EINVAL;
    }
    if(uc_ftype == 1)
	gst_kerenlglobal.sn_num++;
	
    ptr  = buf;
    *ptr = PACKET_START_SIGNAL;
    ptr++;
    *ptr = uc_ftype;
    ptr++;
	*ptr = dtype;
    ptr++;
	*ptr = RESERVE;

    len  = lenth;
    ptr++;
    *ptr = LEN_LOW_BYTE(len);
    ptr++;
    *ptr = LEN_HIGH_BYTE(len);

    len  = gst_kerenlglobal.sn_num;
    ptr++;
    *ptr = LEN_LOW_BYTE(len);
    ptr++;
    *ptr = LEN_HIGH_BYTE(len);

    ptr = buf;
    ptr = ptr + lenth;
    ptr = ptr - sizeof(struct oam_end);
    *ptr = PACKET_END_SIGNAL;
    return 0;
}
#else
/*
 * Prototype    : oamkernel_recv_data
 * Description  : recv data form device
 *              : when oamdrv register wifi.ko, register callbak function
 * Input        : char main_type
                  char sub_type
                  struct sk_buff *skb
                  void *param
 * Return Value : int32
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/8/26
 *     Author       : kf74033
 *     Modification : Created function
 *
 */
int32 oamkernel_recv_data(int8 main_type, int8 sub_type, OS_SK_BUFF_STRU *pst_skb, void *param)
{
    struct sk_buff *skb = NULL;
    int32   l_ret = 0;

    if (NULL == pst_skb)
    {
        OAM_ERROR("para fail\n");
        return -EFAIL;
    }

    l_ret = pst_skb->len;

/*  OAM_INFO("entry:len=%d\n", l_ret);  */

    /* push curr skb to skb queue */
    /* push skb data to skb head queue tail */
    skb_queue_tail(&gst_kerenlglobal.rx_wifi_dbg_seq, pst_skb);

    if (gst_kerenlglobal.rx_wifi_dbg_seq.qlen > RX_WIFI_QUE_MAX_NUM)
    {   /* if rx sdio log queue is too large */
        /* if rx skb number equal to seting value,remove the first skb */
        skb = skb_dequeue(&gst_kerenlglobal.rx_wifi_dbg_seq);
        kfree_skb(skb);
        OAM_ERROR("wifi log queue is too large");
    }
    /* trigger work to send data to oam_app */
    queue_work(gst_kerenlglobal.oam_rx_workqueue, &gst_kerenlglobal.rx_wifi_work);

    return l_ret;
}
#endif

#ifdef SDT_OAM_FOR_1151
/*
 * Prototype    : oamkernel_add_pkt_head
 * Description  : recv data from driver
 * Input        : char main_type
                  char sub_type
                  struct sk_buff *skb
                  void *param
 * Return Value : int32
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/6/20
 *     Author       : 
 *     Modification : Created function
 */
int32 oamkernel_add_pkt_head( OS_SK_BUFF_STRU *pst_skb, uint8 type, uint8 PrimID)
{
    struct  sk_buff *skb = NULL;
    int32   l_ret = 0;
    if (NULL == pst_skb)
    {
        OAM_ERROR("para fail\n");
        return -EFAIL;
    }
	
    l_ret = pst_skb->len;
    OAM_INFO("entry:len=%d\n", l_ret);

    skb = NULL;
    skb = skb_copy_expand(pst_skb, sizeof(struct oam_header),
                    sizeof(struct oam_end), GFP_ATOMIC);
    if (NULL == skb)
    {
        OAM_ERROR("can't allocate mem for new skb");
        return -EINVAL;
    }
	
    skb_push(skb,sizeof(struct oam_header));
    skb_put(skb,sizeof(struct oam_end));

    OAM_INFO("skb->len=%d\n", skb->len);

    wifi_tx_add_head(skb->data, type, PrimID, skb->len);

    /* push curr skb to skb queue */
    skb_queue_tail(&gst_kerenlglobal.rx_wifi_dbg_seq, skb);

    if (gst_kerenlglobal.rx_wifi_dbg_seq.qlen > RX_WIFI_QUE_MAX_NUM)
    {   /* if rx sdio log queue is too large */
        skb = skb_dequeue(&gst_kerenlglobal.rx_wifi_dbg_seq);
        kfree_skb(skb);
        OAM_ERROR("wifi log queue is too large");
    }
	hwifi_free_skb(pst_skb);
    queue_work(gst_kerenlglobal.oam_rx_workqueue, &gst_kerenlglobal.rx_wifi_work);
    return l_ret;
}


int32 oamkernel_up_log( OS_SK_BUFF_STRU *pst_skb)
{
    int32   l_ret = 0;
	
    if (NULL == pst_skb)
    {
        OAM_ERROR("para fail\n");
        return -EFAIL;
    }
    OAM_INFO("entry:len=%d\n", pst_skb->len);
    
	l_ret = oamkernel_add_pkt_head(pst_skb,LOG,OUTPUT_CONTENT);
    return l_ret;
}


int32 oamkernel_up_mem_data( OS_SK_BUFF_STRU *pst_skb)
{
    int32   l_ret = 0;
	
    if (NULL == pst_skb)
    {
        OAM_ERROR("para fail\n");
        return -EFAIL;
    }
	
	l_ret = oamkernel_add_pkt_head(pst_skb, MEM_READ_WRITE,DEV_ACK);
    return l_ret;
}

int32 oamkernel_up_reg_data( OS_SK_BUFF_STRU *pst_skb)
{
    int32   l_ret = 0;
    
    if (NULL == pst_skb)
    {
        OAM_ERROR("para fail\n");
        return -EFAIL;
    } 
	l_ret = oamkernel_add_pkt_head(pst_skb, REG_READ_WRITE,DEV_ACK);
    return l_ret;
}
#else
/* Prototype    : oamkernel_send_data
 * Description  : send data to device
 * Input        : int8 main_type
                  int8 sub_type
                  uint8 *puc_data
                  int32 len
 * Return Value : int32
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/8/26
 *     Author       : kf74033
 *     Modification : Created function
 *
 */
int32 oamkernel_send_data(uint8 main_type, uint8 sub_type, uint8 *puc_data, int32 data_len)
{
    OS_SK_BUFF_STRU         *pst_skb;
    int8                    *p_buf;
    int                      l_len;   /* SDIO CRC ERROR */
    int32                    l_ret = 0;

    if (NULL == puc_data)
    {
        OAM_ERROR("NULL == puc_data.\n");

        return -EFAIL;
    }

    /* FOR CRC ERROR */
    l_len = data_len > 100 ? data_len: 100;

    pst_skb = hwifi_alloc_skb(l_len);
    if (NULL == pst_skb)
    {
        OAM_ERROR("allocate skb failed.\n");

        return -EFAIL;
    }

    p_buf = OS_SKB_PUT(pst_skb, data_len);
    OS_MEM_CPY(p_buf, puc_data, data_len);

    OAM_INFO("entry:len=%d\n", data_len);

    l_ret = hwifi_send(main_type, sub_type, pst_skb);

    return l_ret;
}
#endif

#ifdef SDT_OAM_FOR_1151

int32 oamkernel_send_data_to_driver(uint8 *puc_data, int32 data_len)
{
    OS_SK_BUFF_STRU         *pst_skb;
    int8                    *p_buf;
    int						 l_len;   /* SDIO CRC ERROR */
    int32                    l_ret = 0;

    OAM_INFO("entry\n");

    if (NULL == puc_data)
    {
        OAM_ERROR("NULL == puc_data.\n");
        return -EFAIL;
    }
    /* FOR CRC ERROR */
    l_len = data_len > 100 ? data_len: 100;

    pst_skb = hwifi_alloc_skb(l_len);      
    if (NULL == pst_skb)
    {
        OAM_ERROR("allocate skb failed.\n");
        return -EFAIL;
    }

    p_buf = OS_SKB_PUT(pst_skb, data_len);
    OS_MEM_CPY(p_buf, puc_data, data_len);
    OAM_INFO("entry:len=%d\n", data_len);

	l_len = p_buf[5]*MAX_NUM;
	l_len = p_buf[4] + l_len;
	l_len = l_len - HEAD_ADD_TAIL_LEN;
	switch(p_buf[1])
	{
	if(NULL == tx_action)
    {
    OAM_ERROR("tx_action is NULL \n");
    return -EFAIL;
    }   
	case MEM_READ_WRITE:
        l_ret = tx_action->p_hwifi_down_mem_cmd_func(&pst_skb->data[8], l_len);
		break;
	case REG_READ_WRITE:
        OAM_INFO("REG_READ_WRITE \n");
        l_ret = tx_action->p_hwifi_down_reg_cmd_func(&pst_skb->data[8], l_len);
        break;
    case CONFIG_CMD:
        l_ret = tx_action->p_hwifi_down_cfg_cmd_func(&pst_skb->data[8], l_len);
        break;
    default:
        OAM_ERROR("cmd type ERR!\n");
		break;
	}
	return 0;
}
#endif
/*
 * Prototype    : oamkernel_netlink_send
 * Description  : netlink send data to oam_app
 *              : send data to oam_app
 * Input        : uint8 *puc_data
                  int32 len
 * Return Value : int32
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/8/26
 *     Author       : kf74033
 *     Modification : Created function
 *
 */
int32 oamkernel_netlink_send (uint8 *puc_data, int32 data_len)
{
    OS_SK_BUFF_STRU         *pst_skb;
    OS_NLMSGHDR_STRU        *pst_nlh;
    int32                    l_ret;
    int32                    l_len;

/*  OAM_INFO("entry:len=%d\n", data_len);  */

    l_len = OS_NLMSG_SPACE(data_len);
    pst_skb = OS_ALLOC_SKB(l_len, OS_GFP_KERNEL);
    if (NULL == pst_skb)
    {
        OAM_ERROR("net_link: allocate failed.\n");
        return -EFAIL;
    }
    pst_nlh = OS_NLMSG_PUT(pst_skb, 0, 0, 0, data_len, 0);
    //OS_NETLINK_CB(pst_skb).pid = 0;                 /* from kernel */
    OS_MEM_CPY(OS_NLMSG_DATA(pst_nlh), puc_data, data_len);
	/* send data to netlink link */
    l_ret = OS_NETLINK_UNICAST(gst_kerenlglobal.pst_nlsk, pst_skb, gst_kerenlglobal.ul_usepid, OS_MSG_DONTWAIT);

    return l_ret;
}

/*
 * Prototype    : oamkernel_netlink_recv
 * Description  : netlink recv data form oam_app
 *              : recv data from oam_app
 * Input        : struct sk_buff *__skb
 * Return Value : void
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/8/26
 *     Author       : kf74033
 *     Modification : Created function
 *
 */
void oamkernel_netlink_recv (OS_SK_BUFF_STRU *__skb)
{
    OS_SK_BUFF_STRU                *pst_skb;
    OS_NLMSGHDR_STRU               *pst_nlh;
    OAM_NETLINK_MSG_HDR_ST          st_msg_hdr;
    int32                           l_len;

    if (NULL == __skb)
    {
        OAM_ERROR("para fail\n");
        return;
    }

    OAM_INFO("oamkernel_netlink_recv.\n");

    pst_skb = NULL;
    pst_nlh = NULL;

    OS_MEM_SET(gst_kerenlglobal.puc_data, 0, DATA_BUF_LEN);

    pst_skb = OS_SKB_GET(__skb);
    if(NULL == pst_skb)
    {
        OAM_ERROR("pst_skb is null");
        return;
    }
    if (pst_skb->len >= OS_NLMSG_SPACE(0))
    {
        pst_nlh = OS_NLMSG_HDR(pst_skb);
        l_len   = OS_NLMSG_PAYLOAD(pst_nlh, 0);
        OS_MEM_CPY(gst_kerenlglobal.puc_data, OS_NLMSG_DATA(pst_nlh), l_len);
        OS_MEM_CPY((void *)&st_msg_hdr, gst_kerenlglobal.puc_data, sizeof(st_msg_hdr));

        if (NETLINK_MSG_HELLO == st_msg_hdr.cmd)
        {
            gst_kerenlglobal.ul_usepid = pst_nlh->nlmsg_pid;   /*pid of sending process */

            OAM_INFO("pid is [%d], msg is [%s]\n",
                      gst_kerenlglobal.ul_usepid ,
                      &gst_kerenlglobal.puc_data[sizeof(st_msg_hdr)]);
        }
        else
        {
        	#ifdef SDT_OAM_FOR_1151
            oamkernel_send_data_to_driver(&gst_kerenlglobal.puc_data[sizeof(st_msg_hdr)], l_len - sizeof(st_msg_hdr));  
            #else
            /* send data to wifi.ko */
            oamkernel_send_data( HCC_OAM_TEST, 0, &gst_kerenlglobal.puc_data[sizeof(st_msg_hdr)], l_len - sizeof(st_msg_hdr));
            #endif
        }
    }

    if ( NULL != pst_skb)
    {
        OS_KFREE_SKB(pst_skb);
    }

    return;
}

/*
 * Prototype    : oamkernel_netlink_create
 * Description  : netlink create
 * Input        : void
 * Output       : None
 * Return Value : int32
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/8/26
 *     Author       : kf74033
 *     Modification : Created function
 *
 */
int32 oamkernel_netlink_create( void )
{
	struct netlink_kernel_cfg cfg;
    OAM_INFO("oamkernel_netlink_create\n");
	cfg.groups = 0;
	cfg.input = oamkernel_netlink_recv;
	cfg.cb_mutex = NULL;
    gst_kerenlglobal.pst_nlsk = OS_NETLINK_KERNEL_CREATE(&OS_INIT_NET, NETLINK_TEST, &cfg);
    //gst_kerenlglobal.pst_nlsk = OS_NETLINK_KERNEL_CREATE(&OS_INIT_NET, NETLINK_TEST, 0, oamkernel_netlink_recv, NULL, OS_THIS_MODULE);
    if (NULL == gst_kerenlglobal.pst_nlsk)
    {
        OAM_ERROR( "net_link: Cannot create netlink socket.\n");
        return -EFAIL;
    }

    OAM_INFO("netlink create ok.\n");

    return SUCC;
}

/**
 * Prototype    : oam_push_wifi_log_work 
 * Description  : oam rx wifi skb log data
 * input        : not
 * output       : not
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2013/04/16
 *     Author       : wx144390
 *     Modification : Created function
 *
 */
void oam_push_wifi_log_work(struct work_struct *work)
{
    struct sk_buff *skb;

    skb = NULL;
    /* if skb queue is null,and return */
    while((skb = skb_dequeue(&gst_kerenlglobal.rx_wifi_dbg_seq)))
    {   /* send skb to oam_app use netlink */
        oamkernel_netlink_send(skb->data, skb->len);
        kfree_skb(skb);
    }
    return;
}

#ifdef SDT_OAM_FOR_1151

struct sk_buff *hwifi_alloc_skb(unsigned int len)
{
	struct sk_buff *skb;

	skb  = dev_alloc_skb(len);
	if(NULL == skb)
	{
		OAM_WARNING("test tx skb alloc failed");
		return NULL;
	}
	return skb;
}

EXPORT_SYMBOL(hwifi_alloc_skb);

void hwifi_free_skb(struct sk_buff *skb)
{
    dev_kfree_skb_any(skb);

	return;
}
EXPORT_SYMBOL(hwifi_free_skb);
#endif
#define OAM_IS_START 1
uint32          g_oam_start = 0;
struct mutex    oam_enable_write_mutex;
/*
 * Prototype    : init_oamkernel
 * Description  : oam kernel initializtion
 * Input        : void
 * Return Value : int32
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/8/23
 *     Author       : kf74033
 *     Modification : Created function
 *
 */
int32 oam_start(void)
{
    int32   l_ret;
    int32   ret;

    OAM_INFO("into init_oamkernel\n");

    OS_MEM_SET((void *)&gst_kerenlglobal, 0, sizeof(gst_kerenlglobal));

    gst_kerenlglobal.puc_data = OS_KMALLOC_GFP(DATA_BUF_LEN);
    if (NULL == gst_kerenlglobal.puc_data)
    {
        OAM_ERROR("malloc gst_kerenlglobal.puc_data fail, len = %d.\n", DATA_BUF_LEN);
        return -EFAIL;
    }
    OS_MEM_SET(gst_kerenlglobal.puc_data, 0, DATA_BUF_LEN);

    l_ret = oamkernel_netlink_create();
    if (0 > l_ret)
    {
        OAM_ERROR("oamkernel init is ERR!");
        return -EFAIL;
    }

    gst_kerenlglobal.oam_rx_workqueue = create_singlethread_workqueue("oam_rx_queue");  
    INIT_WORK(&gst_kerenlglobal.rx_wifi_work, oam_push_wifi_log_work);

    skb_queue_head_init(&gst_kerenlglobal.rx_wifi_dbg_seq);
    #ifdef SDT_OAM_FOR_1151
	{
		ret = oam_sdt_register(
	                &tx_action, 
	                oamkernel_up_log, 
	                oamkernel_up_mem_data, 
	                oamkernel_up_reg_data);
	    if(10 == ret)   /* null ptr */
	    {
	        OAM_INFO("register failed\n");
	    }
	}
	#else
    ret = hwifi_rx_extern_register(HCC_OAM_TEST,  oamkernel_recv_data, NULL);
	if (ret < 0)
	{
        OAM_ERROR("hwifi_rx_extern_register is false!");
	}
    else
    {
        OAM_INFO("hwifi_rx_extern_register is ok!");
    }
    #endif
    OAM_INFO("oamkernel init over\n");

    return SUCC;

}

static ssize_t oam_start_write(struct file *filp, const char __user *buffer, size_t len, loff_t *off)
{
    OAM_DEBUG("oam_start_write enter!");
    mutex_lock(&oam_enable_write_mutex);
    if (OAM_IS_START == g_oam_start) {
        OAM_ERROR("oam has started!");
        mutex_unlock(&oam_enable_write_mutex);
        return -EINVAL;
    }

    g_oam_start = OAM_IS_START;
    mutex_unlock(&oam_enable_write_mutex);

    OAM_INFO("oam start begin\n");
    if (0 != oam_start())
    {
        OAM_ERROR("oam start fail!");
        return -EINVAL;
    }

    OAM_INFO("oam start ok\n");
    return len;
}

static const struct file_operations oam_proc_start = {
	.owner = THIS_MODULE,
	.write = oam_start_write,
};

#define OAM_START_PROC_DIR "oam_enable"
#define OAM_START_PROC_FILE "oam_start"
int32 __init init_oamkernel(void)
{
    int             ret = 0;
    struct proc_dir_entry *oam_start_dir = NULL;
    struct proc_dir_entry *oam_start_file = NULL;

#ifdef CONFIG_HWCONNECTIVITY
    if (!isMyConnectivityChip(CHIP_TYPE_HI110X)) {
        OAM_ERROR("cfg oam chip type is not match, skip driver init");
        return -EINVAL;
    } else {
        OAM_INFO("cfg oam type is matched with hi110x, continue");
    }
#endif

    oam_start_dir = proc_mkdir(OAM_START_PROC_DIR, NULL);
    if (!oam_start_dir) {
		OAM_ERROR("oam start create proc dir failed.\n");	
		ret = -ENOMEM;
        return ret;
    }

	oam_start_file = proc_create(OAM_START_PROC_FILE, S_IWUSR|S_IWGRP, oam_start_dir, &oam_proc_start);
    if (!oam_start_file) {
		OAM_ERROR("oam start create proc file failed.\n");	
		ret = -ENOMEM;
        return ret;
    }

    mutex_init(&oam_enable_write_mutex);
    OAM_INFO("oam start create proc file ok.\n");	
	return ret;

}

/*
 * Prototype    : exit_oamkernel
 * Description  : oam kernel exit
 * Input        : void
 * Return Value : void
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/5/23
 *     Author       : kf74033
 *     Modification : Created function
 *
 */
void __exit exit_oamkernel(void)
{

	if (OAM_IS_START != g_oam_start)
	{
		return;
	}
    #ifdef SDT_OAM_FOR_1151
	{
	oam_sdt_unregister(&tx_action);
    kfree(tx_action);
	}
	#else
	hwifi_rx_extern_unregister(HCC_OAM_TEST);
    #endif
    if (gst_kerenlglobal.pst_nlsk != NULL)
    {
        OS_SOCK_RELEASE(gst_kerenlglobal.pst_nlsk->sk_socket);
    }

    OS_MEM_KFREE(gst_kerenlglobal.puc_data);

    destroy_workqueue(gst_kerenlglobal.oam_rx_workqueue);
    skb_queue_purge(&gst_kerenlglobal.rx_wifi_dbg_seq);

    OAM_INFO("oamkernel remove ok.\n");

    return;
}
module_init(init_oamkernel);
module_exit(exit_oamkernel);
MODULE_LICENSE("GPL");

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

