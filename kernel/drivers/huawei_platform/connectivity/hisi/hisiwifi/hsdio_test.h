/*
 *
 * Copyright (C), 2001-2012, Huawei Tech. Co., Ltd.
 *
 * File Name     : hsdio_test.h
 * Version       : initial iraft
 * Created       : 2012/9/6
 * Last Modified :
 * Description   : hsdio_test.c header file
 * Function List :
 * History       :
 * 1.Date        : 2012/9/5
 *   Modification: Created file
 *
 */
#ifndef __HSDIO_TEST_H__
#define __HSDIO_TEST_H__

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
/* HCC SUBTYPE FOR TEST */
#define HCC_SUBTYPE_TX                              2
#define HCC_SUBTYPE_LOOP                            3
/* hcc subtype definition max 7 in firmware */
#define HCC_SUBTYPE_UNKNOWN                         255

#define HSDIO_TEST_GEN_START                        1
#define HSDIO_TEST_GEN_STOP                         0


/* SDIO TEST PKTLEN LIMIT */
#define HSDIO_TEST_MAX_PKTLEN                       1514

/* SDIO TEST MAX PKT GEN */
#define HSDIO_TEST_MAX_PKTGEN                       10000000

/* SDIO TEST DATA FILL */
#define HSDIO_TEST_DATA_GEN                         0x5a

/* SDIO TEST DATA STOP */
#define HSDIO_TEST_DATA_STOP                        0xa5


#ifndef TRUE
#define TRUE                                        1
#endif

#ifndef FALSE
#define FALSE                                       0
#endif

/*
 * 3 Enum Type Definition
 */


/*
 * 4 Global Variable Declaring
 */
 
/*This struct is reference to the wl_config, sync with it.*/
typedef struct _hsdio_test_data_{
    int32                        mode_idx;
    uint32                      pkt_rcvd;/* packet received */
    uint32                      pkt_sent;/* packet sent */
    uint32                      pkt_gen; /* packet generate */
    uint32                      pkt_len; /* packet  length */
    uint64                       trans_time;
    uint64                      throughput;        
}hsdio_test_data;
 
struct  hsdio_test_event {
    struct mutex                mutex_lock;/* sdio test task lock */

    struct workqueue_struct    *test_workqueue;
    struct work_struct          test_work;
    hsdio_test_data     test_data;
    
    /* hsdio perf started */
    uint32                      started;

    /* hsdio perf statistic */
    ktime_t                     start_time;
    /*The last update time*/
    ktime_t                     last_time;

    /*To hsdio test sync*/
    struct completion test_done;

    /*sdio test thread and seam*/
};

struct hsdio_test_mode {
    const char *mode;
    uint32 test_type;
    const char *mode_desc;
};

typedef enum _hsdio_test_case_{
    HSDIO_TEST_CASE_TX,
    HSDIO_TEST_CASE_LOOP,
    HSDIO_TEST_CASE_COUNT
}hsdio_test_case;
/*
 * 5 Message Header Definition
 */


/*
 * 6 Message Definition
 */


/*
 * 7 STRUCT Type Definition
 */

 /*Function Declaration*/
 int hsdio_test_set_case(hsdio_test_data *data);
void hsdio_test_get_case(hsdio_test_data *data);
#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif
