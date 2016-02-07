/*
 ******************************************************************************
 *                     INTERPEAK API HEADER FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_qos.h,v $ $Revision: 1.9 $
 *   $Source: /home/interpeak/CVSRoot/ipnet2/include/ipnet_qos.h,v $
 *   $Author: kenneth $ $Date: 2006-02-02 09:12:07 $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Kenneth Jonsson <kenneth@interpeak.se>
 ******************************************************************************
 */
#ifndef IPNET_QOS_H
#define IPNET_QOS_H

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 */
/* IPNET Quality of Service API */

/*
 ****************************************************************************
 * 2                    CONFIGURATION
 ****************************************************************************
 */

#include "ipnet_config.h"


/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#include <ipcom_type.h>
#include <ipcom_cstyle.h>
#include <ipcom_sock.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */
#define IPNET_DS_NAME_SIZE 16
#define IPNET_IFQ_BASE     192

#ifndef IPNET_PORT_SPECIFIC_DS_IOCTL
/* IOCTL:s used by IPNET QoS API */

#ifndef IPNET_DIFFSERV_CLASSIFIER_MODE_BA

/* Add a filter for differentiated services meter/marker */
#define IPNET_SIOCXADSFILTER   IP_IOWR(DS, 0, W, struct Ipnet_ds_filter)
/* Delete a filter for differentiated services meter/marker */
#define IPNET_SIOCXDDSFILTER   IP_IOWR(DS, 1, W, struct Ipnet_ds_filter)

#endif /* IPNET_DIFFSERV_CLASSIFIER_MODE_BA */

/* Create a new differentiated services meter/marker entity */
#define IPNET_SIOCXDSCREATE    IP_IOWR(DS, 2, W, struct Ipnet_ds)
/* Destroys a differentiated services meter/marker entity */
#define IPNET_SIOCXDSDESTROY   IP_IOWR(DS, 3, W, struct Ipnet_ds)
/* Add a filter for differentiated services meter/marker */
#define IPNET_SIOCXADSMAP      IP_IOWR(DS, 4, W, struct Ipnet_ds_map)
/* Delete a filter for differentiated services meter/marker */
#define IPNET_SIOCXDDSMAP      IP_IOWR(DS, 5, W, struct Ipnet_ds_map)


/* Get the queue type for an interface */
#define IPNET_SIOCGIFQUEUE     IP_IOWR(NETIF, (IPNET_IFQ_BASE + 0), R, struct Ipnet_ifqueue)
/* Set the queue type for an interface */
#define IPNET_SIOCSIFQUEUE     IP_IOWR(NETIF, (IPNET_IFQ_BASE + 1), W, struct Ipnet_ifqueue)
/* Add a filter to an interface queue */
#define IPNET_SIOCXAIFQFILTER  IP_IOWR(NETIF, (IPNET_IFQ_BASE + 2), W, struct Ipnet_ifqueue_filter)
/* Delete a filter from an interface queue */
#define IPNET_SIOCXDIFQFILTER  IP_IOWR(NETIF, (IPNET_IFQ_BASE + 3), W, struct Ipnet_ifqueue_filter)

#endif /* IPNET_PORT_SPECIFIC_IOCTL */


/*
 ****************************************************************************
 * 5                 CLASSIFIER DEFINES
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    Ipnet_classifier_rule
 *===========================================================================
 *
 */
struct Ipnet_classifier_rule
{
    Ip_u16           mask;            /* Set IPNET_CLS_RULE_xxx bit for each field that must match */
    Ip_u8            ds;              /* Traffic class (IPv6) or TOS (IPv4) (== DS field for DiffServ) */
    Ip_u8            proto;           /* Protocol in the IP header */
    Ip_u16           sport_low;       /* Source port for UDP and TCP protocols [low..high] */
    Ip_u16           sport_high;      /* Source port for UDP and TCP protocols */
    Ip_u16           dport_low;       /* Destination port for UDP and TCP protocols [low..high] */
    Ip_u16           dport_high;      /* Destination port for UDP and TCP protocols */
    Ip_u8            af;              /* Address family of the addresses (IP_AF_INET or IP_AF_INET6) */
    Ip_u8            saddr_prefixlen; /* Source prefixlen, set to 0 for exact match of 'saddr' */
    Ip_u8            daddr_prefixlen; /* Destination prefixlen, set to 0 for exact match of 'daddr' */
    union Ip_in_addr_union saddr;     /* Source address or source network */
    union Ip_in_addr_union daddr;     /* Destination address or destination network */
};

#define IPNET_CLS_RULE_DS      (1 << 0)
#define IPNET_CLS_RULE_PROTO   (1 << 1)
#define IPNET_CLS_RULE_SADDR   (1 << 2)
#define IPNET_CLS_RULE_DADDR   (1 << 3)
#define IPNET_CLS_RULE_SPORT   (1 << 4)
#define IPNET_CLS_RULE_DPORT   (1 << 5)



/*
 ****************************************************************************
 * 6                 DIFFERENTIATE SERVICESES
 ****************************************************************************
 */

/* Connects a DS rule with a DS meter/marker.
   Each meter/marker can be pointed to by more than one rule */
struct Ipnet_ds_map
{
    int filter_id; /* The filter ID when running in DiffServ Multi-Field mode.
                      The value of the DS (i.e. TOS or TCLASS) field when running in
                      Behavior Aggregate mode */
    int ds_id;     /* The ID of the Differentiated Services handler to apply for
                      traffic matching the filter */
};


/* Rule to match traffic */
struct Ipnet_ds_filter
{
    int                          id;   /* Id for this filter. Read only for add operation */
    struct Ipnet_classifier_rule rule; /* The rule for this filter */
};


/* name="SimpleMarker" - Structure for the simple marker, matching traffic will be assigned
   the specified DS field and the specified drop_precedence */
struct Ipnet_ds_sm
{
    Ip_u8    mask;            /* IPNET_DS_SM_xxx bits that determines if what should be done
                                 with matching packets */
    Ip_u8    ds_value;        /* Differentiated Services Field to set (if IPNET_DS_SM_DS_VAL is set) */
    Ip_u8    drop_precedence; /* The drop precedence, which has to be one of the
                                 IPCOM_PKT_DROP_PRECEDENCE_xxx constants (if IPNET_DS_SM_DROP_P is set) */
};

#define IPNET_DS_SM_DS_VAL ((Ip_u8) (1 << 0))
#define IPNET_DS_SM_DROP_P ((Ip_u8) (1 << 1))


/* name="srTCM" - Structure for the single rate Three Color Marker */
struct Ipnet_ds_srtcm
{
    Ip_u8    mode;      /* Mode this meter is operating in, one of the
                           IPNET_DS_SRTCM_MODE_xxx constants */
    Ip_u32   CIR;       /* Committed Information Rate (bytes/second) */
    Ip_u32   CBS;       /* Committed Burst Size (bytes) */
    Ip_u32   EBS;       /* Excess Burst Size (bytes) */
    Ip_u8    ds_green;  /* Differentiated Services Field to set for green packets */
    Ip_u8    ds_yellow; /* Differentiated Services Field to set for yellow packets */
    Ip_u8    ds_red;    /* Differentiated Services Field to set for red packets */
};

/* The mode that the srTCM can operate in */
#define IPNET_DS_SRTCM_MODE_COLOR_BLIND 1
#define IPNET_DS_SRTCM_MODE_COLOR_AWARE 2


/* Differentiated Services ID structure */
struct Ipnet_ds
{
    int  id;                       /* ID of the diffserv handler. Read only for create operation */
    char name[IPNET_DS_NAME_SIZE]; /* The name of the diffserv handler */
    union Ipnet_ds_data
    {
        struct Ipnet_ds_sm    sm;
        struct Ipnet_ds_srtcm srtcm;
    } d;
};


/*
 ****************************************************************************
 * 7              LEAF NETWORK INTERFACE QUEUES
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    Ipnet_ifqueue_fifo
 *===========================================================================
 * Parameter struct used for "fifo" type queue.
 */
struct Ipnet_ifqueue_fifo
{
    Ip_u32 fifo_limit; /* Max number of packets that can be queued */
};


/*
 *===========================================================================
 *                    Ipnet_ifqueue_dpaf
 *===========================================================================
 * Parameter struct used for the Drop Precedence Aware FIFO queue type.
 * This queue type uses more memory and is a little slower compared to the
 * FIFO queue.
 */
struct Ipnet_ifqueue_dpaf
{
    Ip_u32 dpaf_limit; /* Max number of packets (regardless of drop
                          precedence) that can be queued */
};


/*
 *===========================================================================
 *                    Ipnet_ifqueue_netemu
 *===========================================================================
 * Parameter struct used for "netemu" type queue.
 */
struct Ipnet_ifqueue_netemu
{
    Ip_u32  netemu_limit;               /* Max number of packets that can be queued */
    Ip_u32  netemu_min_latency;         /* Minimum latency added to packets in milliseconds */
    Ip_u32  netemu_max_latency;         /* Maximum latency added to packets in milliseconds */
    Ip_bool netemu_random_drop;         /* IP_FALSE -> drop every drop_probability packet
                                           IP_TRUE  -> there is 1/every drop_probability
                                           chance that at packet is dropped */
    Ip_u32  netemu_drop_probability;    /* 1 means every packet dropped, 10 means that
                                           every 10 packet is dropped */
    Ip_u32  netemu_drop_pattern;        /* Bit patter where set bit means drop packet
                                           ex: Bit 0 and bit 2 set, all other cleared */
    Ip_u8   netemu_drop_pattern_len;    /* Number of valid bits in "netemu_drop_pattern",
                                           setting the length to zero will have the same effect
                                           as setting bit 0 in the pattern and the length to 1 */
    Ip_bool netemu_random_corrupt;      /* IP_FALSE -> corrupt every corrupt_probability packet
                                           IP_TRUE  -> there is 1/every corrupt_probability
                                           chance that a packet is corrupted */
    Ip_u32  netemu_corrupt_probability; /* 1 means every packet corrupted, 10 means tha
                                           every 10 packet is corrupted */
};



/*
 ****************************************************************************
 * 8              CONTAINER NETWORK INTERFACE QUEUES
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    Ipnet_ifqueue_filter
 *===========================================================================
 * Determines which interface queue a packet will be queued at in the
 * container queue identified by the filter ID.
 * Diffserv traffic should only use the 'ds' field in the rule, but it is
 * possible to use all supported fields.
 */
struct Ipnet_ifqueue_filter
{
    char   filter_ifname[IP_IFNAMSIZ]; /* Interface name, e.g. "gif1", "gre1" */
    int    filter_id;                  /* Will be set to the ID of the filter for add
                                          and is used to identify the filter on delete */
    int    filter_queue_id;            /* The ID of the queue this filter should
                                          be attached to */
    /* Following fields are only needed for filter add operations */
    int    filter_child_queue_id;      /* The ID of the child that packet matching
                                          this rule should be queued at */
    struct Ipnet_classifier_rule filter_rule;
};


#define IPNET_IFQ_CONTAINER_MAX_COUNT 64

/*
 *===========================================================================
 *                    Ipnet_ifqueue_container
 *===========================================================================
 * Read only structure that will be filled in by IP_SIOCGIFQUEUE operation.
 */
struct Ipnet_ifqueue_container
{
    int child_count; /* Number of valid entries in the 'child_ids' array */
    int child_ids[IPNET_IFQ_CONTAINER_MAX_COUNT]; /* ID of the childs */
};


/*
 *===========================================================================
 *                    Ipnet_ifqueue_mbc
 *===========================================================================
 * Parameter struct used for "mbc" type queues.
 * This container queue type has 2 or more "bands", filter rules determines
 * which band is used when enqueueing. The rule for dequeueing works like
 * this; packet will be dequeued from band 0 until empty,
 * packets will then be dequeued from band 1 until empty, etc.
 */
struct Ipnet_ifqueue_mbc
{
    struct Ipnet_ifqueue_container mbc_container; /* Read only */
    Ip_u32 mbc_bands;        /* Number of bands, must be 2 or more */
    Ip_u32 mbc_default_band; /* Index of the band to use for packets that
                                do not match any rule [0..prio_bands) */
};


/*
 *===========================================================================
 *                    Ipnet_ifqueue_htbc
 *===========================================================================
 * Parameter struct used for "htbc" type queues.
 * The default queue will get all traffic that does not match some other
 * classify rule.
 * Dequeueing is done in by Round-Robin through all childs.
 */
struct Ipnet_ifqueue_htbc
{
    struct Ipnet_ifqueue_container htbc_container; /* Read only */
    Ip_u32  htbc_byterate;    /* Maximum bitrate for this queue */
    Ip_u32  htbc_token_limit; /* Maximum number of tokens this queue
                                 can have, MUST be >MTU of the interface */
    int     htbc_default_id;  /* The ID of the default queue (read-only) */
};


/*
 *===========================================================================
 *                    Ipnet_ifqueue - ioctl
 *===========================================================================
 * struct used with IP_SIOCxIFQUEUE ioctls.
 */
struct Ipnet_ifqueue
{
    char ifq_name[IP_IFNAMSIZ];  /* if name, e.g. "gif1", "gre1" */
    char ifq_type[IP_IFNAMSIZ];  /* queue type e.g. "fifo", "red" */
    int  ifq_id;                 /* Must be an ID that unique on the specified interface or
                                    IP_IFQ_ID_NONE for get operation when the root queue should be returned */
    int ifq_parent_id;           /* ID of the parent queue or IP_IFQ_ID_NONE
                                    if this is directly attached to the interface */
    int ifq_count;               /* Will contain the current number of queued packets
                                    after a IP_SIOCGIFQUEUE call */
    union Ipnet_ifqueue_data {
        struct Ipnet_ifqueue_fifo   fifo;
        struct Ipnet_ifqueue_dpaf   dpaf;
        struct Ipnet_ifqueue_netemu netemu;
        struct Ipnet_ifqueue_htbc   htbc;
        struct Ipnet_ifqueue_mbc    mbc;
    } ifq_data;
};

#define IP_IFQ_ID_NONE    0 /* Must be used as parent ID if the queue is a root queue */
#define IP_IFQ_ID_DEFAULT 1 /* Default ID, should be used on root queues where no specific ID is required */


#ifdef __cplusplus
}
#endif

#endif

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
