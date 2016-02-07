


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif



/*
 * 1 Header File Including
 */
#include "hwifi_utils.h"
#include "station_mgmt.h"
#include "hwifi_event.h"
#include "hwifi_cfg80211.h"

/*
 * 2 Global Variable Definition
 */

struct sta_mgmt_ops
{
    int32   (*init)(struct sta_mgmt_struct *sta_mgmt);
    void    (*exit)(struct sta_mgmt_struct *sta_mgmt);

    struct  conn_sta_info* (*add)(struct sta_mgmt_struct *sta_mgmt,  int32 aid, uint8* mac);
    void    (*remove)(struct sta_mgmt_struct *sta_mgmt, struct conn_sta_info *sta);
    void    (*removeall)( struct sta_mgmt_struct *sta_mgmt);

    struct conn_sta_info*  (*find_by_aid)(struct sta_mgmt_struct *sta_mgmt, int32 aid);
    struct conn_sta_info*  (*find_by_mac)(struct sta_mgmt_struct *sta_mgmt, const uint8* mac);
};

/*
 * 3 Function Definition
 */

/*
 * Prototype    : hwifi_stations_add
 * Description  :
 * Input        : HWIFI_NET_ADAPTER_STRU  *priv
                int32 aid
                uint8* mac
 * Output       : None
 * Return Value : struct conn_sta_info *
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/8/13
 *     Author       : kf74588
 *     Modification : Created function
 *
 */
STATIC struct conn_sta_info * hwifi_stations_add(struct sta_mgmt_struct *sta_mgmt,
                                                       int32                   aid,
                                                       uint8                  *mac)
{

    HWIFI_ASSERT(NULL != sta_mgmt);

    if (aid >= 1 && aid <= MAX_CONN_STA_CNT)
    {
        struct conn_sta_info *sta;
        sta      = &sta_mgmt->stations[aid - 1];
        sta->aid = aid;
        memcpy(sta->mac, mac, MAC_LEN);
        HWIFI_DEBUG("Succed to add STA aid:%d mac :"MACFMT,aid ,MAC2STR(mac));

        return sta;
    }

    HWIFI_WARNING("Failed to add sta by aid:%d mac !"MACFMT,aid, MAC2STR(mac));

    return NULL;
}


/*
 * Prototype    : hwifi_stations_remove
 * Description  :
 * Input        : HWIFI_NET_ADAPTER_STRU  *priv
                struct conn_sta_info *sta
 * Output       : None
 * Return Value : void
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/8/13
 *     Author       : kf74588
 *     Modification : Created function
 *
 */
STATIC void hwifi_stations_remove(struct sta_mgmt_struct *sta_mgmt, struct conn_sta_info *sta)
{
    HWIFI_ASSERT(NULL != sta_mgmt);
    HWIFI_ASSERT(NULL != sta);

    HWIFI_DEBUG("Remove STA: aid=%d", sta->aid);

    memset(sta, 0, sizeof(struct conn_sta_info));
}

/*
 * Prototype    : hwifi_stations_removeall
 * Description  :
 * Input        : HWIFI_NET_ADAPTER_STRU  *priv
 * Output       : None
 * Return Value : void
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/8/13
 *     Author       : kf74588
 *     Modification : Created function
 *
 */
STATIC void hwifi_stations_removeall(struct sta_mgmt_struct *sta_mgmt)
{
    HWIFI_ASSERT(NULL != sta_mgmt);

    HWIFI_DEBUG("Removeall STA");
    memset(sta_mgmt->stations, 0, MAX_CONN_STA_CNT * sizeof(struct conn_sta_info));
}

/*
 * Prototype    : hwifi_stations_find_by_aid
 * Description  :
 * Input        : HWIFI_NET_ADAPTER_STRU  *priv
                int32 aid
 * Output       : None
 * Return Value : struct conn_sta_info*
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/8/13
 *     Author       : kf74588
 *     Modification : Created function
 *
 */
STATIC struct conn_sta_info* hwifi_stations_find_by_aid(struct sta_mgmt_struct *sta_mgmt, int32 aid)
{
    HWIFI_ASSERT(NULL != sta_mgmt);

    if (aid >= 1 && aid <= MAX_CONN_STA_CNT)
    {
        struct conn_sta_info *sta;

        sta = &sta_mgmt->stations[aid - 1];
        if (aid == sta->aid)
        {
            HWIFI_DEBUG("Succeed to find by aid:%d!",aid);
            return sta;
        }
    }

    HWIFI_WARNING("Failed to find by aid:%d!",aid);
    return NULL;
}

/*
 * Prototype    : hwifi_stations_find_by_mac
 * Description  :
 * Input        : HWIFI_NET_ADAPTER_STRU  *priv
                uint8* mac
 * Output       : None
 * Return Value : struct conn_sta_info*
 * Calls        :
 * Called By    :
 *
 *   History        :
 *   1.Date         : 2012/8/13
 *     Author       : kf74588
 *     Modification : Created function
 *
 */
STATIC struct conn_sta_info* hwifi_stations_find_by_mac(struct sta_mgmt_struct *sta_mgmt, const uint8* mac)
{
    int i;

    HWIFI_ASSERT(NULL != sta_mgmt);

    if(NULL == mac)
    {
        HWIFI_WARNING("Invalid NULL mac");
        return NULL;
    }

    if (0 == (mac[0] | mac[1] | mac[2] | mac[3] | mac[4] | mac[5]))
    {
        HWIFI_WARNING("Invalid all 0 mac!");
        return NULL;
    }

    for(i=0; i < MAX_CONN_STA_CNT; i++)
    {
        struct conn_sta_info *sta;

        sta = &sta_mgmt->stations[i];

        if (0 == memcmp(mac, sta->mac, MAC_LEN))
        {
            HWIFI_DEBUG("Succeed to find by mac"MACFMT,MAC2STR(mac));
            return sta;
        }
    }

    HWIFI_WARNING("find_by_mac " MACFMT "fail", MAC2STR(mac));
    return NULL;
}



STATIC struct sta_mgmt_ops g_mgmt_ops = {
    .add         = hwifi_stations_add,
    .remove      = hwifi_stations_remove,
    .removeall   = hwifi_stations_removeall,
    .find_by_aid = hwifi_stations_find_by_aid,
    .find_by_mac = hwifi_stations_find_by_mac,
};



int32 station_mgmt_init(struct sta_mgmt_struct *sta_mgmt)
{
    sta_mgmt->generation = 0;
    memset(sta_mgmt->stations, 0, sizeof(sta_mgmt->stations));

    return SUCC;
}



void removeall_conn_sta(struct sta_mgmt_struct *sta_mgmt)
{
    g_mgmt_ops.removeall(sta_mgmt);
}



void remove_conn_sta(struct sta_mgmt_struct *sta_mgmt, struct conn_sta_info *sta)
{
    g_mgmt_ops.remove(sta_mgmt, sta);
}



struct  conn_sta_info* add_conn_sta(struct sta_mgmt_struct *sta_mgmt,  int32 aid, uint8* mac)
{
    return g_mgmt_ops.add(sta_mgmt, aid, mac);
}


struct conn_sta_info*  find_by_aid(struct sta_mgmt_struct *sta_mgmt, int32 aid)
{
    return g_mgmt_ops.find_by_aid(sta_mgmt, aid);
}



struct conn_sta_info*  find_by_mac(struct sta_mgmt_struct *sta_mgmt, const uint8* mac)
{
    return g_mgmt_ops.find_by_mac(sta_mgmt, mac);
}

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

