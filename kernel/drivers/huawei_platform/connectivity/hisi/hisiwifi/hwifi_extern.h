

#ifndef __HWIFI_EXTERN_H__
#define __HWIFI_EXTERN_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*
 * 1 Other Header File Including
 */

#include <linux/skbuff.h>
#include "common.h"
#include "hwifi_hcc.h"

/*
 * 2 Macro Definition
 */


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


/*
 * 8 UNION Type Definition
 */


/*
 * 9 OTHERS Definition
 */


/*
 * 10 Function Declare
 */
int32  hwifi_rx_extern_register(uint8 mtype, hcc_rx_cb_func rx_cb, void *context);
void hwifi_rx_extern_unregister(unsigned char main_type);
int  hwifi_send(uint8 main_type, uint8 sub_type, struct sk_buff *skb);
struct sk_buff *hwifi_alloc_skb(uint32 len);
void hwifi_free_skb(struct sk_buff *skb);
extern void hwifi_union_log_dump(char* print_loglevel);

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of hwifi_extern.h */
