

#ifndef __CFG_EVENT_RX_H__
#define __CFG_EVENT_RX_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*
 * 1 Other Header File Including
 */

#include "common.h"


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

extern int32 report_scanned_bss(struct cfg_struct *cfg, struct scanned_bss_event *event);
extern int32 report_scan_done(struct cfg_struct *cfg, struct scan_complete_stru *event);
extern int32 report_connect_event(struct cfg_struct *cfg, struct hwifi_conn_result_stru *event);
extern int32 report_disconnect_event(struct cfg_struct *cfg, struct hwifi_disconn_result_stru *event);
extern int32 report_stats_result(struct cfg_struct *cfg, struct hwifi_stats_result_stru *event);
extern int32 report_sta_join_event(struct cfg_struct *cfg, struct hwifi_sta_join_stru *event);
extern int32 report_sta_leave_event(struct cfg_struct *cfg, struct hwifi_sta_leave_stru *event);
extern int32 report_change_mode_resp(struct cfg_struct *cfg, struct hwifi_set_mode_stru *event);
extern int32 report_set_param_resp(struct cfg_struct *cfg, struct hwifi_change_res_stru *event);
extern int32 report_mgmt_frame(struct cfg_struct *cfg, struct mgmt_report_event_stru *event);
extern int32 report_init_param_resp(struct cfg_struct *cfg, struct hwifi_init_res_stru *event);
extern int32 report_recv_channel_info(struct cfg_struct *cfg, int8 *buf, int32 len);
extern int32 report_status_ready_param_resp(struct cfg_struct *cfg, struct hwifi_status_ready_stru *event);
int32 hwifi_dev_ready_event(struct cfg_struct *cfg, struct hwifi_status_ready_stru *event);

extern int32 report_mic_error_event(struct cfg_struct *cfg, struct mic_error_report_stru *event);

#ifdef __ROAM__
extern int32 report_roamed_event(struct cfg_struct *cfg, struct hwifi_roamed_event *event);
#endif

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of cfg_event_rx.h */
