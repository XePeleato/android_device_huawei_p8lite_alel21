
#ifndef __TEST_DRV_MAILBOX_H__
#define __TEST_DRV_MAILBOX_H__

/*****************************************************************************
  1 头文件包含
*****************************************************************************/



#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 宏定义
*****************************************************************************/
#define TEST_MAILBOX_INIT_FLAG          (0x5aa55aa5)

#define TM_MAILBOX_BYTE_PER_KB          (1024)

#define Second_To_Millisecond           (1000)
 /*计算带宽的乘法上限*/
#define MAILBOX_BOARDST_BOUNDWIDTH_MUL_LIMIT   30

/*****************************************************************************
  3 枚举定义
*****************************************************************************/

/*****************************************************************************
  4 消息头定义
*****************************************************************************/


/*****************************************************************************
  5 消息定义
*****************************************************************************/


/*****************************************************************************
  6 STRUCT定义
*****************************************************************************/

/*****************************************************************************
 实 体 名  : struct tm_mb_handle
 功能描述  : 板侧测试模块控制句柄
*****************************************************************************/
struct tm_mb_handle
{
    void *          mutex;
    unsigned int   init_flag;
};

/*****************************************************************************
 实 体 名  : struct tm_mb_cb
 功能描述  : 板侧测试的用户回调句柄
*****************************************************************************/
struct tm_mb_cb
{
    void *           finish_sema;
    void *           sync_sema;
    unsigned int    start_slice;       /*for calculate boundwidth*/
    unsigned int    end_slice;         /*for calculate boundwidth*/
    unsigned int    prob_slice;        /*for calculate latency*/
    unsigned int    back_slice;        /*for calculate latency*/
    unsigned int    msg_count;
    unsigned int    task_count;
    unsigned int    check_ret;
};

/*****************************************************************************
 实 体 名  : test_mailbox_send_proc_func
 功能描述  : 板侧测试的入口函数
*****************************************************************************/
typedef void (*test_mailbox_send_proc_func)(
                unsigned int mailcode,
                unsigned int recvcode,
                unsigned int sendsize,
                unsigned int msgnum,
                unsigned int delaytick,
                unsigned int delaynum,
                unsigned int trytimes,
                struct tm_mb_cb * cb);

/*****************************************************************************
 实 体 名  : struct test_mailbox_proc
 功能描述  : 板侧测试的任务入参结构体
*****************************************************************************/
struct test_mailbox_proc
{
    test_mailbox_send_proc_func proc_cb;
    unsigned int mail_code;
    unsigned int recv_code;
    unsigned int send_size;
    unsigned int msg_num;
    unsigned int delay_tick;
    unsigned int delay_num;
    unsigned int try_times;
    struct tm_mb_cb * mb_cb;
};


/*****************************************************************************
  7 UNION定义
*****************************************************************************/

/*****************************************************************************
  8 OTHERS定义
*****************************************************************************/


extern unsigned int test_mailbox_msg_multi_send(
                unsigned int DstId,
                unsigned int CarrierID,
                unsigned int TaskNum,
                unsigned int MsgLen,
                unsigned int MsgNum,
                unsigned int Priority,
                unsigned int DelayTick,
                unsigned int DelayNum,
                unsigned int TryTimes
);

extern unsigned int test_mailbox_msg_multi_test(
                unsigned int DstId,
                unsigned int TaskNum,
                unsigned int MsgNum,
                unsigned int DelayNum);

extern unsigned int test_mailbox_msg_single_test(
                unsigned int DstId,
                unsigned int MsgLen,
                unsigned int MsgNum,
                unsigned int DelayNum);

extern void test_mailbox_msg_reg(unsigned int ChannelID);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of test_drv_mailbox.h */

