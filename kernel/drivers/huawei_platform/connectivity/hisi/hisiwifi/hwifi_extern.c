


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*
 * 1 Header File Including
 */
#include <linux/sched.h>
#include <linux/netdevice.h>

#include "hwifi_extern.h"
#include "common.h"
#include "hi110x.h"
#include "hwifi_utils.h"


/*
 * 2 Global Variable Definition
 */



int32 hwifi_rx_extern_register(uint8 mtype, hcc_rx_cb_func rx_cb, void *context)
{
    if((NULL == g_hi110x_dev) || (NULL == g_hi110x_dev->hcc))
    {
        HWIFI_WARNING("Invalid NULL hi110x or hcc!");
        return -EFAIL;
    }

    return hcc_rx_register(g_hi110x_dev->hcc, mtype, rx_cb, context);
}
EXPORT_SYMBOL(hwifi_rx_extern_register);


void hwifi_rx_extern_unregister(uint8 mtype)
{
    if((NULL == g_hi110x_dev) || (NULL == g_hi110x_dev->hcc))
    {
        HWIFI_WARNING("Invalid NULL hi110x or hcc!");
        return ;
    }

    hcc_rx_unregister(g_hi110x_dev->hcc, mtype);
}
EXPORT_SYMBOL(hwifi_rx_extern_unregister);


struct sk_buff *hwifi_alloc_skb(uint32 len)
{
    struct sk_buff *skb;
    skb  = dev_alloc_skb(len + HCC_SKB_TAIL_ALGIN_LEN);
    if(NULL == skb)
    {
        HWIFI_WARNING("Failed to alloc skb!");
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


int32 hwifi_send(uint8 main_type, uint8 sub_type, struct sk_buff *skb)
{
    /*int ret = 0;*/
    /*
     * g_hi110x_dev need not be sync, because g_hi110x_dev can only be none when sdio removed.
     * but if there is a module depend on this module, it can't be removed.
     */
    if ((NULL == g_hi110x_dev) || (NULL == skb) || (NULL == g_hi110x_dev->hcc))
    {
        HWIFI_ERROR("Invalid hi110x or hcc or skb!");
        if(skb)
            dev_kfree_skb_any(skb);
        return -EFAIL;
    }

    /* TBD: cancel suspend */   

    if (SUCC != hcc_tx(g_hi110x_dev->hcc, main_type, sub_type, skb))
    {
        HWIFI_ERROR("Failed to send hcc msg!");
        dev_kfree_skb_any(skb);
        return -EFAIL;
    }

    return SUCC;
}
EXPORT_SYMBOL(hwifi_send);


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

