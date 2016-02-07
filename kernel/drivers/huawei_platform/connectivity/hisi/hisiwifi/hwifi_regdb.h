

#ifndef __HWIFI_REGDB_H__
#define __HWIFI_REGDB_H__

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
/* regulatory domain dfs radar standard */
typedef enum {
    DFS_RADAR_NULL  = 0,
    DFS_RADAR_FCC   = 1,
    DFS_RADAR_ETSI  = 2,
    DFS_RADAR_MKK   = 3 
}DFS_RADAR_STD_T;


/*
 * 4 Global Variable Declaring
 */
extern const struct ieee80211_regdomain *g_regdom_rules_db[];
extern uint32 g_regdom_rules_db_size;

extern uint8  g_regdom_dfs_radar_db[];

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

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of hwifi_regdb.h */
