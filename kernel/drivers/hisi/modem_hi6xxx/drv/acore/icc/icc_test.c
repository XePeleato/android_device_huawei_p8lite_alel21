#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/semaphore.h>
#include <linux/kthread.h>
#include <linux/slab.h>

/*#include <mach/spinLockAmp.h>*/
#include "BSP.h"
/*#include <mach/common/bsp_memory.h>*/
//#include <mach/hardware.h>
#include "bsp_icc_drv.h"


#ifdef __cplusplus
extern "C" {
#endif

unsigned int g_u32TstRwDelay = 1;

/* 宏定义 */
#define ICC_TEST_LARGE_DATA_LEN   2000
#define ICC_TEST_LARGE_DATA_LEN_TMP (2000 * 10)
#define ICC_TEST_DEBUG  printk
#define iccTestDelay(a)     msleep((a)*10)
#define TST_SPEED_DATA_SIZE	(512)
#define ICC_INVALID_CHAN_ID       (20) /* Cshell 口不作保留操作 */
/* 局部变量定义 */
static BSP_BOOL g_bInit = BSP_FALSE;
static BSP_U8 g_senddata[4096];
static BSP_U32 g_sendlen = 0;
static BSP_U8 g_recvdata[4096];
static BSP_U32 g_asendlen[32] = {0};
static BSP_U8 *g_asenddata[32] = {0};
static struct task_struct *g_u32IccSendTaskId[32];
static struct semaphore g_semIccSendTask[32];
static struct semaphore g_semIccWriteCb[32];
static volatile BSP_U32 g_arecvtotal[32] = {0};
static volatile BSP_U32 g_arecvtotalb[32] = {0};
static volatile BSP_U32 g_asendtotal[32] = {0};
/* 外部变量声明 */
extern ICC_CHAN_DEV_S   *g_pstOpenList;
/* 外部函数声明 */
extern BSP_VOID ICC_SetTestMode(BSP_BOOL bEnable);
extern BSP_VOID ICC_ListGet(ICC_CHAN_DEV_S *pList, BSP_U32 u32ChanId, ICC_CHAN_DEV_S **pNode);

#define DEV_INIT()\
do{\
    if(!g_bInit)\
    {\
        if(OK != BSP_ICC_Init())\
        {\
            ICC_TEST_DEBUG("init fail...\n");\
            return ERROR;\
        }\
        ICC_SetTestMode(BSP_TRUE);\
        g_bInit = BSP_TRUE;\
    }\
}while(0)

#define CHECK_RET(a, b)\
do{\
    BSP_S32 x;\
    x = a;\
    if(x != b)\
    {\
        ICC_TEST_DEBUG("%s(%d): ret(0x%x) error...\n", __FUNCTION__, __LINE__, x);\
        return 1;\
    }\
}while(0)

#define CHECK_VAL(a, b)\
do{\
    if(a != b)\
    {\
        ICC_TEST_DEBUG("%s(%d): value error... 0x%x 0x%x\n", __FUNCTION__, __LINE__, a, b);\
        return ERROR;\
    }\
}while(0)

BSP_U32 event_cb(BSP_U32 id, BSP_U32 event, BSP_VOID* Param)
{
    ICC_TEST_DEBUG("chan[%d], event:%d\n", id,event);
    return OK;
}

BSP_U32 write_cb(BSP_U32 id)
{
    BSP_S32 ret = 0;

    if (!g_sendlen)
    {
        return OK;
    }

    ret = BSP_ICC_Write(id, g_senddata, g_sendlen);
    if ((ret < 0) || ((BSP_U32)ret > g_sendlen))
    {
        ICC_TEST_DEBUG("[%d],write cb fail!, 0x%x\n", __LINE__, ret);
        return OK;
    }
    else if ((BSP_U32)ret < g_sendlen)
    {
        g_sendlen = g_sendlen - ret;
    }
    else
    {
        ICC_TEST_DEBUG("[%d],write cb OK!, 0x%x\n", __LINE__, ret);
        g_sendlen = 0;
        up(&g_semIccSendTask[id]);
    }

    return OK;
}

BSP_U32 write_cb_large(BSP_U32 id)
{
    up(&g_semIccWriteCb[id]);
    return OK;
}

BSP_U32 read_cb(BSP_U32 id, BSP_S32 size)
{
    CHECK_RET(BSP_ICC_Read(id, g_recvdata, size), size);
    g_arecvtotal[id] +=size;
    if (g_arecvtotal[id] == g_arecvtotalb[id])
    {
        up(&g_semIccSendTask[id]);
        /*ICC_TEST_DEBUG("%s  %d  id=%d   release icc send task sem\n", __FUNCTION__, __LINE__, id);*/
    }
    return OK;
}

BSP_U32 read_cb_speed(BSP_U32 id, BSP_S32 size)
{
    BSP_S32 s32sendtime = 0;
    BSP_S32 s32recvtime = 0;
    BSP_S32 s32difftime = 0;
    CHECK_RET(BSP_ICC_Read(id, g_recvdata, size), size);
    g_arecvtotal[id] +=size;

    s32recvtime = 0;//(BSP_S32)omTimerGet();
    s32sendtime = g_recvdata[0] | (g_recvdata[1] << 8) | (g_recvdata[2] << 16) |(g_recvdata[3] << 24);
    s32difftime = (s32sendtime > s32recvtime) ?(s32sendtime - s32recvtime) :(0xFFFFFFFF + s32sendtime - s32recvtime);

    if(g_arecvtotal[id] == g_arecvtotalb[id])
    {
        /* 20代表(20*1/32768)，单位秒*/
        if(s32difftime < 20)
        {
            ICC_TEST_DEBUG("TEST RW SPEED CASE 1 PASSED，speed = %d\n",s32difftime);
            up(&g_semIccSendTask[id]);
        }
        else
        {
            ICC_TEST_DEBUG("TEST RW SPEED CASE 1 FAIL，speed = %d\n",s32difftime);
        }
    }

    return OK;
}

BSP_U32 read_cb_full(BSP_U32 id, BSP_S32 size)
{
    ICC_TEST_DEBUG("%s  %d  id=%d, size=%d\n",__FUNCTION__, __LINE__, id, size);
    up(&g_semIccSendTask[id]);
    return OK;
}

/* 打开非对称通道成功 */
BSP_S32 open_channel(BSP_U32 id)
{
    ICC_CHAN_ATTR_S attr;
    ICC_CHAN_DEV_S *pChan;

    DEV_INIT();

    attr.enChanMode = ICC_CHAN_MODE_STREAM;
    attr.u32Priority = 255;
    attr.u32TimeOut = 1000;
    attr.event_cb = BSP_NULL;
    attr.read_cb = BSP_NULL;
    attr.write_cb = BSP_NULL;
#ifdef BSP_CORE_MODEM
    attr.u32FIFOInSize = 128;
    attr.u32FIFOOutSize = 128;
#else
    attr.u32FIFOInSize = 128;
    attr.u32FIFOOutSize = 128;
#endif
    ICC_ListGet(g_pstOpenList, (BSP_U32)id, &pChan);
    if(!pChan)
    {
        CHECK_RET(BSP_ICC_Open(id, &attr), BSP_OK);
        ICC_TEST_DEBUG("%s  %d  id=%d\n",__FUNCTION__, __LINE__, id);
    }
    return OK;
}

BSP_S32 open_channel_large(BSP_U32 id)
{
    ICC_CHAN_ATTR_S attr;

    DEV_INIT();

    attr.enChanMode = ICC_CHAN_MODE_STREAM;
    attr.u32Priority = 255;
    attr.u32TimeOut = 1000;
    attr.u32FIFOInSize = 1024;
    attr.u32FIFOOutSize = 1024;
    attr.event_cb = event_cb;
    attr.read_cb = read_cb;
    attr.write_cb = write_cb_large;

    CHECK_RET(BSP_ICC_Open(id, &attr), BSP_OK);

    return OK;
}

BSP_S32 icc_write_task(void *data)
{
    int id = *(int*)data;
    BSP_U8 *p = g_asenddata[id];
    BSP_S32 ret;

    ICC_TEST_DEBUG("write task chan %d\n", id);


    p = kmalloc(ICC_TEST_LARGE_DATA_LEN, GFP_KERNEL);
    if (!p)
    {
        ICC_TEST_DEBUG("%s  %d  FAILED\n", __FUNCTION__, __LINE__);
        return ERROR;
    }
    g_asenddata[id] = p;

    g_asendlen[id] = g_arecvtotalb[id];

    while (g_asendlen[id])
    {
        ret = BSP_ICC_Write(id, g_asenddata[id], g_asendlen[id]);
        ICC_TEST_DEBUG("%s  %d ret = %d g_asendlen[%d] = %d\n",__FUNCTION__, __LINE__, ret, id, g_asendlen[id]);
        if ((ret < 0) || ((BSP_U32)ret > g_asendlen[id]))
        {
            ICC_TEST_DEBUG("%s  %d  FAILED\n", __FUNCTION__, __LINE__);
            return ERROR;
        }
        else if ((BSP_U32)ret < g_asendlen[id])
        {
            ICC_TEST_DEBUG("%s  %d\n",__FUNCTION__, __LINE__);
            g_asendtotal[id] += ret;
            g_asendlen[id] -= ret;
            down(&g_semIccWriteCb[id]);
            ICC_TEST_DEBUG("%s  %d\n",__FUNCTION__, __LINE__);
        }
        else
        {
            g_asendtotal[id] += ret;
            g_asendlen[id] = 0;
        }
    }
    ICC_TEST_DEBUG("%s  %d  chan[%d] write done! T = %d\n", __FUNCTION__, __LINE__, id, g_asendtotal[id]);

    ICC_TEST_DEBUG("%s  %d\n",__FUNCTION__, __LINE__);
    if (g_arecvtotal[id] != g_arecvtotalb[id])
    {
        down(&g_semIccSendTask[id]);
    }
    ICC_TEST_DEBUG("%s  %d\n",__FUNCTION__, __LINE__);
    kfree(p);
    return OK;
}
BSP_S32 icc_write_task_tmp(void *data)
{
    int id = *(int*)data;
    BSP_U8 *p = g_asenddata[id];
    BSP_S32 ret1 = 0;
    BSP_S32 ret2 = 0;
    /*ICC_TEST_DEBUG("write task chan %d\n", id);*/
    p = kmalloc(ICC_TEST_LARGE_DATA_LEN_TMP, GFP_KERNEL);
    if (!p)
    {
        ICC_TEST_DEBUG("%s  %d  FAILED\n", __FUNCTION__, __LINE__);
        return ERROR;
    }
    g_asenddata[id] = p;
    g_asendlen[id] = g_arecvtotalb[id];
    while (g_asendlen[id])
    {
        ret1 = BSP_ICC_Write(id, g_asenddata[id], 2000);
        ICC_TEST_DEBUG("%s  %d ret1 = %d g_asendlen[%d] = %d\n", __FUNCTION__, __LINE__, ret1, id, g_asendlen[id]);
        if((BSP_U32)ret1 < 2000)
        {
            if (0 != down_timeout(&g_semIccWriteCb[id], msecs_to_jiffies(2000)))
            {
                /*ICC_TEST_DEBUG("%s  %d  FAILED\n",__FUNCTION__, __LINE__);*/
            }
            ret2 = BSP_ICC_Write(id, g_asenddata[id], 2000 - ret1);
            ICC_TEST_DEBUG("%s  %d ret2 = %d g_asendlen[%d] = %d\n", __FUNCTION__, __LINE__, ret2, id, g_asendlen[id]);
        }
        g_asendlen[id] -= 2000;
        iccTestDelay(g_u32TstRwDelay);
    }
    /*ICC_TEST_DEBUG("%s  %d  chan[%d] write done! T = %d\n", __FUNCTION__, __LINE__, id, g_asendtotal[id]);*/
    /*ICC_TEST_DEBUG("%s  %d\n",__FUNCTION__, __LINE__);*/
    if (g_arecvtotal[id] != g_arecvtotalb[id])
    {
        down(&g_semIccSendTask[id]);
    }
    /*ICC_TEST_DEBUG("%s  %d\n",__FUNCTION__, __LINE__);*/

    kfree(p);

    return OK;
}

/* TEST:初始化ICC */
BSP_S32 BSP_ICC_ST_INIT_001(void)
{
    CHECK_RET(BSP_ICC_Init(), BSP_OK);
    g_bInit = BSP_TRUE;

    ICC_TEST_DEBUG("TEST INIT CASE 1 PASSED\n");
    return OK;
}

/* TEST:打开无效通道(号) */
BSP_S32 BSP_ICC_ST_OPEN_CLOSE_001(void)
{
    ICC_CHAN_ATTR_S attr;

    DEV_INIT();
    CHECK_RET(BSP_ICC_Open(32, &attr), BSP_ERR_ICC_INVALID_CHAN);

    ICC_TEST_DEBUG("TEST OPEN CLOSE CASE 1 PASSED\n");
    return OK;
}

/* TEST:打开错误的通道(异常参数) */
BSP_S32 BSP_ICC_ST_OPEN_CLOSE_002(void)
{
    ICC_CHAN_ATTR_S attr;

    DEV_INIT();
    CHECK_RET(BSP_ICC_Open(31, BSP_NULL), BSP_ERR_ICC_NULL);

    attr.enChanMode = (ICC_CHAN_MODE_E)2;
    attr.u32FIFOInSize = 32;
    attr.u32FIFOOutSize = 4096;
    attr.u32Priority = 255;
    attr.u32TimeOut = 20;
    attr.event_cb = BSP_NULL;
    attr.read_cb = BSP_NULL;
    attr.write_cb = BSP_NULL;
    CHECK_RET(BSP_ICC_Open(31, &attr), BSP_ERR_ICC_INVALID_PARAM);
    attr.enChanMode = ICC_CHAN_MODE_STREAM;

    attr.u32FIFOInSize = 28;
    CHECK_RET(BSP_ICC_Open(1, &attr), BSP_ERR_ICC_INVALID_PARAM);
    attr.u32FIFOInSize = 32;

    attr.u32FIFOInSize = 33;
    CHECK_RET(BSP_ICC_Open(1, &attr), BSP_ERR_ICC_INVALID_PARAM);
    attr.u32FIFOInSize = 32;

    attr.u32FIFOInSize = 65540 * 2;  /*s00249479 k310*/
    CHECK_RET(BSP_ICC_Open(1, &attr), BSP_ERR_ICC_INVALID_PARAM);
    attr.u32FIFOInSize = 32;

    attr.u32FIFOOutSize = 28;
    CHECK_RET(BSP_ICC_Open(2, &attr), BSP_ERR_ICC_INVALID_PARAM);
    attr.u32FIFOOutSize = 32;

    attr.u32FIFOOutSize = 65540 * 2;  /*s00249479 k310*/
    CHECK_RET(BSP_ICC_Open(2, &attr), BSP_ERR_ICC_INVALID_PARAM);
    attr.u32FIFOOutSize = 4096;

    attr.u32FIFOOutSize = 4095;
    CHECK_RET(BSP_ICC_Open(2, &attr), BSP_ERR_ICC_INVALID_PARAM);
    attr.u32FIFOOutSize = 4096;

    attr.u32Priority = 256;
    CHECK_RET(BSP_ICC_Open(2, &attr), BSP_ERR_ICC_INVALID_PARAM);
    attr.u32Priority = 255;

    ICC_TEST_DEBUG("TEST OPEN CLOSE CASE 2 PASSED\n");
    return OK;
}

/* TEST:单向打开通道 */
BSP_S32 BSP_ICC_ST_OPEN_CLOSE_003(void)
{
    ICC_CHAN_ATTR_S attr;

    DEV_INIT();

    attr.enChanMode = ICC_CHAN_MODE_STREAM;
    attr.u32FIFOInSize = 32;
    attr.u32FIFOOutSize = 4096;
    attr.u32Priority = 255;
    attr.u32TimeOut = 20;
    attr.event_cb = BSP_NULL;
    attr.read_cb = BSP_NULL;
    attr.write_cb = BSP_NULL;

    CHECK_RET(BSP_ICC_Open(1, &attr), BSP_ERR_ICC_TIMEOUT);

    ICC_TEST_DEBUG("TEST OPEN CLOSE CASE 3 PASSED\n");
    return OK;
}


/* TEST:打开对称通道 */
BSP_S32 BSP_ICC_ST_OPEN_CLOSE_004(void)
{
    ICC_CHAN_ATTR_S attr;

    DEV_INIT();

    attr.enChanMode = ICC_CHAN_MODE_STREAM;
    attr.u32Priority = 255;
    attr.u32TimeOut = 1000;
    attr.event_cb = BSP_NULL;
    attr.read_cb = BSP_NULL;
    attr.write_cb = BSP_NULL;
    attr.u32FIFOInSize = 1024;
    attr.u32FIFOOutSize = 1024;

    CHECK_RET(BSP_ICC_Open(1, &attr), BSP_OK);
    iccTestDelay(100);
    CHECK_RET(BSP_ICC_Close(1), BSP_OK);

    ICC_TEST_DEBUG("TEST OPEN CLOSE CASE 4 PASSED\n");
    return OK;
}


/* TEST:打开非对称通道 */
BSP_S32 BSP_ICC_ST_OPEN_CLOSE_005(void)
{
    ICC_CHAN_ATTR_S attr;

    DEV_INIT();

    attr.enChanMode = ICC_CHAN_MODE_STREAM;
    attr.u32Priority = 255;
    attr.u32TimeOut = 1000;
    attr.event_cb = BSP_NULL;
    attr.read_cb = BSP_NULL;
    attr.write_cb = BSP_NULL;
#ifdef BSP_CORE_MODEM
    attr.u32FIFOInSize = 32;
    attr.u32FIFOOutSize = 4096;
#else
    attr.u32FIFOInSize = 4096;
    attr.u32FIFOOutSize = 32;
#endif

    CHECK_RET(BSP_ICC_Open(1, &attr), BSP_OK);
    iccTestDelay(100);
    CHECK_RET(BSP_ICC_Close(1), BSP_OK);

    ICC_TEST_DEBUG("TEST OPEN CLOSE CASE 5 PASSED\n");
    return OK;
}


/* TEST:双方通道属性不同，打开失败 */
BSP_S32 BSP_ICC_ST_OPEN_CLOSE_006(void)
{
    ICC_CHAN_ATTR_S attr;

    DEV_INIT();

#ifdef BSP_CORE_MODEM
    attr.enChanMode = ICC_CHAN_MODE_STREAM;
    attr.u32Priority = 255;
    attr.u32TimeOut = 1000;
    attr.event_cb = BSP_NULL;
    attr.read_cb = BSP_NULL;
    attr.write_cb = BSP_NULL;
    attr.u32FIFOInSize = 32;
    attr.u32FIFOOutSize = 4096;
    CHECK_RET(BSP_ICC_Open(1, &attr), BSP_ERR_ICC_NEGOTIATE_FAIL);
#else
    attr.enChanMode = ICC_CHAN_MODE_STREAM;
    attr.u32Priority = 255;
    attr.u32TimeOut = 1000;
    attr.event_cb = BSP_NULL;
    attr.read_cb = BSP_NULL;
    attr.write_cb = BSP_NULL;
    attr.u32FIFOInSize = 4096;
    attr.u32FIFOOutSize = 32;

    attr.u32FIFOOutSize = 64;
    CHECK_RET(BSP_ICC_Open(1, &attr), BSP_ERR_ICC_INVALID_PARAM);
    attr.u32FIFOOutSize = 32;

    attr.u32FIFOInSize = 2048;
    CHECK_RET(BSP_ICC_Open(1, &attr), BSP_ERR_ICC_INVALID_PARAM);
    attr.u32FIFOInSize = 4096;

    attr.enChanMode = ICC_CHAN_MODE_PACKET;
    CHECK_RET(BSP_ICC_Open(1, &attr), BSP_ERR_ICC_INVALID_PARAM);
    attr.enChanMode = ICC_CHAN_MODE_STREAM;

    attr.u32Priority = 254;
    CHECK_RET(BSP_ICC_Open(1, &attr), BSP_ERR_ICC_INVALID_PARAM);
    attr.u32Priority = 255;
#endif

    ICC_TEST_DEBUG("TEST OPEN CLOSE CASE 6 PASSED\n");
    return OK;
}


/* TEST:重复打开通道 */
BSP_S32 BSP_ICC_ST_OPEN_CLOSE_007(void)
{
    ICC_CHAN_ATTR_S attr;

    DEV_INIT();

    attr.enChanMode = ICC_CHAN_MODE_STREAM;
    attr.u32Priority = 255;
    attr.u32TimeOut = 1000;
    attr.event_cb = BSP_NULL;
    attr.read_cb = BSP_NULL;
    attr.write_cb = BSP_NULL;
#ifdef BSP_CORE_MODEM
    attr.u32FIFOInSize = 32;
    attr.u32FIFOOutSize = 4096;
#else
    attr.u32FIFOInSize = 4096;
    attr.u32FIFOOutSize = 32;
#endif

    CHECK_RET(BSP_ICC_Open(1, &attr), BSP_OK);
    iccTestDelay(100);
    CHECK_RET(BSP_ICC_Open(1, &attr), BSP_ERR_ICC_OPENED);
    iccTestDelay(300);
    CHECK_RET(BSP_ICC_Close(1), BSP_OK);
    iccTestDelay(300);

    ICC_TEST_DEBUG("TEST OPEN CLOSE CASE 7 PASSED\n");
    return OK;
}

/* TEST:关闭无效通道 */
BSP_S32 BSP_ICC_ST_OPEN_CLOSE_008(void)
{
    DEV_INIT();

    CHECK_RET(BSP_ICC_Close(32), BSP_ERR_ICC_INVALID_CHAN);

    ICC_TEST_DEBUG("TEST OPEN CLOSE CASE 8 PASSED\n");
    return OK;
}

/* TEST:关闭未打开的通道 */
BSP_S32 BSP_ICC_ST_OPEN_CLOSE_009(void)
{
    DEV_INIT();

    CHECK_RET(BSP_ICC_Close(1), BSP_ERR_ICC_NOT_OPEN);

    ICC_TEST_DEBUG("TEST OPEN CLOSE CASE 9 PASSED\n");
    return OK;
}

/* TEST:正常打开关闭通道 */
BSP_S32 BSP_ICC_ST_OPEN_CLOSE_010(void)
{
    DEV_INIT();

    open_channel(1);
    iccTestDelay(100);
    CHECK_RET(BSP_ICC_Close(1), BSP_OK);

    ICC_TEST_DEBUG("TEST OPEN CLOSE CASE 10 PASSED\n");
    return OK;
}

/* TEST:打开关闭多个通道 */
BSP_S32 BSP_ICC_ST_OPEN_CLOSE_011(void)
{
    int i = 0;
    ICC_CHAN_DEV_S *pChan;
    BSP_BOOL  idleChan[32] = {0};

    DEV_INIT();

    for(i = 0; i < ICC_INVALID_CHAN_ID; i ++)
    {
        ICC_ListGet(g_pstOpenList, (BSP_U32)i, &pChan);
        if(pChan)
        {
            idleChan[i] = BSP_FALSE;
            continue;
        }
        idleChan[i] = BSP_TRUE;
        if(BSP_OK != open_channel(i))
        {
            ICC_TEST_DEBUG("TEST OPEN CASE 11 FAILED\n");
            return ERROR;
        }
    }

    iccTestDelay(100);
    for(i = 0; i < ICC_INVALID_CHAN_ID; i ++)
    {
        if(BSP_TRUE == idleChan[i])
        {
            ICC_TEST_DEBUG("%s  %d  id=%d\n",__FUNCTION__, __LINE__, i);
            CHECK_RET(BSP_ICC_Close(i), BSP_OK);
        }
    }

    ICC_TEST_DEBUG("TEST OPEN CLOSE CASE 11 PASSED\n");
    return OK;
}

/* TEST:A->C(C->A)某个通道上传输stream类型数据*/
BSP_S32 BSP_ICC_ST_RW_STREAM_001(void)
{
    int i = 0;
    ICC_CHAN_ATTR_S attr;

    DEV_INIT();

    g_arecvtotal[1] = 0;
    g_arecvtotalb[1] = 55;
    sema_init(&g_semIccSendTask[1], SEM_EMPTY);
    attr.enChanMode = ICC_CHAN_MODE_STREAM;
    attr.u32FIFOInSize = 128;
    attr.u32FIFOOutSize = 128;
    attr.u32Priority = 255;
    attr.u32TimeOut = 1000;
    attr.event_cb = event_cb;
    attr.read_cb = read_cb;
    attr.write_cb = write_cb;
    CHECK_RET(BSP_ICC_Open(1, &attr), BSP_OK);

    i = 0;
    while(i++ <100)
    {
        g_senddata[i] = i;
    }

    down(&g_semIccSendTask[1]);
    iccTestDelay(10);
    /* 分10 次连续发送总计55 字节数据*/
    i = 0;
    while(i++ <10)
    {
        CHECK_RET(BSP_ICC_Write(1, g_senddata, i), i);
    }

    iccTestDelay(300);
    CHECK_RET(BSP_ICC_Close(1), BSP_OK);

    ICC_TEST_DEBUG("TEST RW STREAM CASE 1 PASSED\n");
    return OK;
}


/* 读写测试，双向发送数据 */
BSP_S32 BSP_ICC_ST_RW_STREAM_002(void)
{
    int i = 0;
    int chanIndex = 0;
    BSP_BOOL idleChan[32] = {0};   /* 用以表示通道是否空闲 */
    ICC_CHAN_ATTR_S attr;
    ICC_CHAN_DEV_S *pChan;

    DEV_INIT();

    for(chanIndex = 0; chanIndex < ICC_INVALID_CHAN_ID; chanIndex++)
    {
        ICC_ListGet(g_pstOpenList, (BSP_U32)chanIndex, &pChan);
        if(pChan)
        {
            idleChan[chanIndex] = BSP_FALSE;
            g_arecvtotal[chanIndex] = 0;
            g_arecvtotalb[chanIndex] = 0;
            continue;
        }
        ICC_TEST_DEBUG("%s  %d  id=%d\n",__FUNCTION__, __LINE__, chanIndex);
        idleChan[chanIndex] = BSP_TRUE;
        g_arecvtotal[chanIndex] = 0;
        g_arecvtotalb[chanIndex] = 55;
        sema_init(&g_semIccSendTask[chanIndex], SEM_EMPTY);
        attr.enChanMode = ICC_CHAN_MODE_STREAM;
        attr.u32FIFOInSize = 128;
        attr.u32FIFOOutSize = 128;
        attr.u32Priority = 255;
        attr.u32TimeOut = 1000;
        attr.event_cb = event_cb;
        attr.read_cb = read_cb;
        attr.write_cb = write_cb;
        CHECK_RET(BSP_ICC_Open(chanIndex, &attr), BSP_OK);
    }

    do
    {
        iccTestDelay(100);
        for(chanIndex = 0; chanIndex < ICC_INVALID_CHAN_ID; chanIndex ++)
        {
            if(g_arecvtotal[chanIndex] != g_arecvtotalb[chanIndex])
            {
                break;
            }
        }
    }while(chanIndex != ICC_INVALID_CHAN_ID);

    i = 0;
    while(i++ <100)
    {
        g_senddata[i] = i;
    }

    iccTestDelay(10);
    /* 分15 次连续发送总计55 字节数据*/
    i = 0;
    while(i++ <10)
    {
        for(chanIndex=0; chanIndex<ICC_INVALID_CHAN_ID; chanIndex++)
        {
            if(BSP_TRUE == idleChan[chanIndex])
            {
                ICC_TEST_DEBUG("%s  %d  id=%d\n",__FUNCTION__, __LINE__, chanIndex);
                CHECK_RET(BSP_ICC_Write(chanIndex, g_senddata, i), i);
            }
        }
    }

    iccTestDelay(300);
    for(chanIndex = 0; chanIndex < ICC_INVALID_CHAN_ID; chanIndex++)
    {
        if(BSP_TRUE == idleChan[chanIndex])
        {
            ICC_TEST_DEBUG("%s  %d  id=%d\n",__FUNCTION__, __LINE__, chanIndex);
            CHECK_RET(BSP_ICC_Close(chanIndex), BSP_OK);
        }
    }

    ICC_TEST_DEBUG("TEST RW STREAM CASE 2 PASSED\n");
    return OK;
}


/* TEST:A <->C某个通道上传输stream类型数据*/
BSP_S32 BSP_ICC_ST_RW_STREAM_003(void)
{
    int i = 0;
    ICC_CHAN_ATTR_S attr;

    DEV_INIT();

    g_arecvtotal[1] = 0;
    g_arecvtotalb[1] = 55;
    sema_init(&g_semIccSendTask[1], SEM_EMPTY);
    attr.enChanMode = ICC_CHAN_MODE_STREAM;
    attr.u32FIFOInSize = 128;
    attr.u32FIFOOutSize = 128;
    attr.u32Priority = 255;
    attr.u32TimeOut = 1000;
    attr.event_cb = event_cb;
    attr.read_cb = read_cb;
    attr.write_cb = write_cb;
    CHECK_RET(BSP_ICC_Open(1, &attr), BSP_OK);


    i = 0;
    while(i++ <100)
    {
        g_senddata[i] = i;
    }

    iccTestDelay(10);
    /* 分10 次连续发送总计55 字节数据*/
    i = 0;
    while(i++ <10)
    {
        CHECK_RET(BSP_ICC_Write(1, g_senddata, i), i);
    }

    down(&g_semIccSendTask[1]);

    iccTestDelay(300);
    CHECK_RET(BSP_ICC_Close(1), BSP_OK);

    ICC_TEST_DEBUG("TEST RW STREAM CASE 3 PASSED\n");
    return OK;
}


/* TEST:A <->C多个通道上传输stream类型数据*/
BSP_S32 BSP_ICC_ST_RW_STREAM_004(void)
{
    int i = 0;
    int chanIndex = 0;
    BSP_BOOL idleChan[32] = {0};   /* 用以表示通道是否空闲 */
    ICC_CHAN_ATTR_S attr;
    ICC_CHAN_DEV_S *pChan;

    DEV_INIT();

    for(chanIndex = 0; chanIndex < ICC_INVALID_CHAN_ID; chanIndex++)
    {
        ICC_ListGet(g_pstOpenList, (BSP_U32)chanIndex, &pChan);
        if(pChan)
        {
            idleChan[chanIndex] = BSP_FALSE;
            g_arecvtotal[chanIndex] = 0;
            g_arecvtotalb[chanIndex] = 0;
            continue;
        }
        idleChan[chanIndex] = BSP_TRUE;
        g_arecvtotal[chanIndex] = 0;
        g_arecvtotalb[chanIndex] = 55;
        sema_init(&g_semIccSendTask[chanIndex], SEM_EMPTY);
        attr.enChanMode = ICC_CHAN_MODE_STREAM;
        attr.u32FIFOInSize = 128;
        attr.u32FIFOOutSize = 128;
        attr.u32Priority = 255;
        attr.u32TimeOut = 1000;
        attr.event_cb = event_cb;
        attr.read_cb = read_cb;
        attr.write_cb = write_cb;
        CHECK_RET(BSP_ICC_Open(chanIndex, &attr), BSP_OK);
    }

    i = 0;
    while(i++ <100)
    {
        g_senddata[i] = i;
    }

    iccTestDelay(100);
    /* 分10 次连续发送总计55 字节数据*/
    i = 0;
    while(i++ <10)
    {
        for(chanIndex=0; chanIndex<ICC_INVALID_CHAN_ID; chanIndex++)
        {
            if(BSP_TRUE == idleChan[chanIndex])
            {
                CHECK_RET(BSP_ICC_Write(chanIndex, g_senddata, i), i);
            }
        }
    }

    do
    {
        iccTestDelay(100);
        for(chanIndex = 0; chanIndex < ICC_INVALID_CHAN_ID; chanIndex ++)
        {
            if(g_arecvtotal[chanIndex] != g_arecvtotalb[chanIndex])
            {
                break;
            }
        }
    }while(chanIndex != ICC_INVALID_CHAN_ID);

    iccTestDelay(300);
    for(chanIndex=0; chanIndex<ICC_INVALID_CHAN_ID; chanIndex++)
    {
        if(BSP_TRUE == idleChan[chanIndex])
        {
            CHECK_RET(BSP_ICC_Close(chanIndex), BSP_OK);
        }
    }

    ICC_TEST_DEBUG("TEST RW STREAM CASE 4 PASSED\n");
    return OK;
}

/* TEST:A <->C多个通道上传输大量stream类型数据*/
BSP_S32 BSP_ICC_ST_RW_STREAM_005(void)
{
    int id = 1;
    char taskName[30] = {0};

    DEV_INIT();

    g_arecvtotal[1] = 0;
    g_asendtotal[1] = 0;
    g_arecvtotalb[1] = ICC_TEST_LARGE_DATA_LEN;
    sema_init(&g_semIccSendTask[1], SEM_EMPTY);
    sema_init(&g_semIccWriteCb[1], SEM_EMPTY);
    if(BSP_OK != open_channel_large(1))
        {
            ICC_TEST_DEBUG("%s  %d  FAILED\n", __FUNCTION__, __LINE__);
            return ERROR;
    }

    iccTestDelay(100);

    ICC_TEST_DEBUG(taskName,"iccTest%02d",1);
    g_u32IccSendTaskId[1] = kthread_run(icc_write_task, (void *)&id, taskName);
    if (IS_ERR(g_u32IccSendTaskId[1]))
        {
        kthread_stop(g_u32IccSendTaskId[1]);
            return ERROR;
    }
    ICC_TEST_DEBUG("%s  %d\n",__FUNCTION__, __LINE__);
    do
    {
        iccTestDelay(10);
    }while((0 != g_asendlen[1]) || (g_arecvtotal[1] != (g_arecvtotalb[1])));
    ICC_TEST_DEBUG("%s  %d\n",__FUNCTION__, __LINE__);
    iccTestDelay(300);
    CHECK_RET(BSP_ICC_Close(1), BSP_OK);
    ICC_TEST_DEBUG("TEST RW STREAM CASE 5 PASSED\n");
    return OK;
}


/*该用例下A核处理不过来buffer中的数据，初步分析是因为SFT主频太低只有24Mhz，因此调度
上来不及*/
BSP_S32 BSP_ICC_ST_RW_STREAM_006(BSP_S32 cnt)  /*s00249479 k310*/
{
    int id = 1;
    char taskName[30] = {0};
    DEV_INIT();
    g_arecvtotal[1] = 0;
    g_asendtotal[1] = 0;
    g_arecvtotalb[1] = ICC_TEST_LARGE_DATA_LEN_TMP * cnt; /*s00249479 k310*/
    sema_init(&g_semIccSendTask[1], SEM_EMPTY);
    sema_init(&g_semIccWriteCb[1], SEM_EMPTY);
    if(BSP_OK != open_channel_large(1))
    {
        /*ICC_TEST_DEBUG("%s  %d  FAILED\n", __FUNCTION__, __LINE__);*/
        return ERROR;
    }

    iccTestDelay(100);

    /*ICC_TEST_DEBUG(taskName,"iccTest%02d",1);*/
    g_u32IccSendTaskId[1] = kthread_run(icc_write_task_tmp, (void *)&id, taskName);
    if (IS_ERR(g_u32IccSendTaskId[1]))
    {
        kthread_stop(g_u32IccSendTaskId[1]);
        return ERROR;
    }
    /*ICC_TEST_DEBUG("%s  %d\n",__FUNCTION__, __LINE__);*/
    do
    {
        iccTestDelay(10);
        /*ICC_TEST_DEBUG("func = %s  line = %d  %d    %d  %d\n",__FUNCTION__, __LINE__, g_asendlen[1], g_arecvtotal[1], g_arecvtotalb[1]);*/
    }while((0 != g_asendlen[1]) || (g_arecvtotal[1] != (g_arecvtotalb[1])));
    /*ICC_TEST_DEBUG("%s  %d\n",__FUNCTION__, __LINE__);*/
    iccTestDelay(300);
    CHECK_RET(BSP_ICC_Close(1), BSP_OK);

    ICC_TEST_DEBUG("TEST RW STREAM CASE 6 PASSED\n");
    return OK;
}

/* TEST:A->C(C->A)某个通道上传输packet类型数据*/
BSP_S32 BSP_ICC_ST_RW_PACKET_001(void)
{
    int i = 0;
    ICC_CHAN_ATTR_S attr;

    DEV_INIT();

    g_sendlen = (252-ICC_PACKET_HEADER_LEN);
    g_arecvtotal[1] = 0;
    g_arecvtotalb[1] = (252-ICC_PACKET_HEADER_LEN) + 55;
    sema_init(&g_semIccSendTask[1], SEM_EMPTY);
    attr.enChanMode = ICC_CHAN_MODE_PACKET;
    attr.u32FIFOInSize = 256;
    attr.u32FIFOOutSize = 256;
    attr.u32Priority = 255;
    attr.u32TimeOut = 1000;
    attr.event_cb = event_cb;
    attr.read_cb = read_cb;
    attr.write_cb = write_cb;
    CHECK_RET(BSP_ICC_Open(1, &attr), BSP_OK);

    i=0;
    while(i++ <10)
    {
        g_senddata[i] = i;
    }
     down(&g_semIccSendTask[1]);

     i=0;
    while(i++ <10)
    {
        CHECK_RET(BSP_ICC_Write(1, g_senddata, i), i);
    }
    iccTestDelay(10);
    CHECK_RET(BSP_ICC_Write(1, g_senddata, (252-ICC_PACKET_HEADER_LEN+8)), BSP_ERR_ICC_PACKET_SIZE);
    iccTestDelay(10);
    CHECK_RET(BSP_ICC_Write(1, g_senddata, (252-ICC_PACKET_HEADER_LEN+4)), BSP_ERR_ICC_PACKET_SIZE);
    iccTestDelay(100);
    CHECK_RET(BSP_ICC_Write(1, g_senddata, (252-ICC_PACKET_HEADER_LEN)), (252-ICC_PACKET_HEADER_LEN));
    //CHECK_RET(BSP_ICC_Write(1, g_senddata, (252-ICC_PACKET_HEADER_LEN)), BSP_ERR_ICC_BUFFER_FULL);
     iccTestDelay(300);
    CHECK_RET(BSP_ICC_Close(1), BSP_OK);

    ICC_TEST_DEBUG("TEST RW PACKET CASE 1 PASSED\n");
    return OK;
}


/* TEST:A->C(C->A)多个通道上传输packet类型数据*/
BSP_S32 BSP_ICC_ST_RW_PACKET_002(void)
{
    int i = 0;
    int chanIndex = 0;
    BSP_BOOL idleChan[32] = {0};   /* 用以表示通道是否空闲 */
    ICC_CHAN_ATTR_S attr;
    ICC_CHAN_DEV_S *pChan;

    DEV_INIT();
    for(chanIndex = 0; chanIndex < ICC_INVALID_CHAN_ID; chanIndex++)
    {
        ICC_ListGet(g_pstOpenList, (BSP_U32)chanIndex, &pChan);
        if(pChan)
        {
            idleChan[chanIndex] = BSP_FALSE;
            g_arecvtotal[chanIndex] = 0;
            g_arecvtotalb[chanIndex] = 0;
            continue;
        }
        idleChan[chanIndex] = BSP_TRUE;
        g_arecvtotal[chanIndex] = 0;
        g_arecvtotalb[chanIndex] = 55;
        sema_init(&g_semIccSendTask[chanIndex], SEM_EMPTY);
        attr.enChanMode = ICC_CHAN_MODE_PACKET;
        attr.u32FIFOInSize = 256;
        attr.u32FIFOOutSize = 256;
        attr.u32Priority = 255;
        attr.u32TimeOut = 1000;
        attr.event_cb = event_cb;
        attr.read_cb = read_cb;
        attr.write_cb = write_cb;
        CHECK_RET(BSP_ICC_Open(chanIndex, &attr), BSP_OK);
    }

    i=0;
    while(i++ <10)
    {
        g_senddata[i] = i;
    }

    do
    {
        iccTestDelay(100);
        for(chanIndex = 0; chanIndex < ICC_INVALID_CHAN_ID; chanIndex ++)
        {
            if(g_arecvtotal[chanIndex] != g_arecvtotalb[chanIndex])
            {
                break;
            }
        }
    }while(chanIndex != ICC_INVALID_CHAN_ID);

    i=0;
    while(i++ <10)
    {
        for(chanIndex=0; chanIndex<ICC_INVALID_CHAN_ID; chanIndex++)
        {
            if(BSP_TRUE == idleChan[chanIndex])
            {
                CHECK_RET(BSP_ICC_Write(chanIndex, g_senddata, i), i);
            }
        }
    }

    iccTestDelay(300);
    for(chanIndex=0; chanIndex<ICC_INVALID_CHAN_ID; chanIndex++)
    {
        if(BSP_TRUE == idleChan[chanIndex])
        {
            CHECK_RET(BSP_ICC_Close(chanIndex), BSP_OK);
        }
    }

    ICC_TEST_DEBUG("TEST RW PACKET CASE 2 PASSED\n");
    return OK;
}


/* TEST:A <->C某个通道上传输packet类型数据*/
BSP_S32 BSP_ICC_ST_RW_PACKET_003(void)
{
    int i = 0;
    ICC_CHAN_ATTR_S attr;

    DEV_INIT();

    g_sendlen = (252-ICC_PACKET_HEADER_LEN);
    g_arecvtotal[1] = 0;
    g_arecvtotalb[1] = (252-ICC_PACKET_HEADER_LEN) + 55;
    sema_init(&g_semIccSendTask[1], SEM_EMPTY);
    attr.enChanMode = ICC_CHAN_MODE_PACKET;
    attr.u32FIFOInSize = 256;
    attr.u32FIFOOutSize = 256;
    attr.u32Priority = 255;
    attr.u32TimeOut = 1000;
    attr.event_cb = event_cb;
    attr.read_cb = read_cb;
    attr.write_cb = write_cb;
    CHECK_RET(BSP_ICC_Open(1, &attr), BSP_OK);

    i=0;
    while(i++ <10)
    {
        g_senddata[i] = i;
    }

    i=0;
    while(i++ <10)
    {
        CHECK_RET(BSP_ICC_Write(1, g_senddata, i), i);
    }
    iccTestDelay(10);
    CHECK_RET(BSP_ICC_Write(1, g_senddata, (252-ICC_PACKET_HEADER_LEN+8)), BSP_ERR_ICC_PACKET_SIZE);
    iccTestDelay(10);
    CHECK_RET(BSP_ICC_Write(1, g_senddata, (252-ICC_PACKET_HEADER_LEN+4)), BSP_ERR_ICC_PACKET_SIZE);
    iccTestDelay(100);
    CHECK_RET(BSP_ICC_Write(1, g_senddata, (252-ICC_PACKET_HEADER_LEN)), (252-ICC_PACKET_HEADER_LEN));
    //CHECK_RET(BSP_ICC_Write(1, g_senddata, (252-ICC_PACKET_HEADER_LEN)), BSP_ERR_ICC_BUFFER_FULL);

    down(&g_semIccSendTask[1]);

    iccTestDelay(300);
    CHECK_RET(BSP_ICC_Close(1), BSP_OK);

    ICC_TEST_DEBUG("TEST RW PACKET CASE 3 PASSED\n");
    return OK;
}


/* TEST:A <->C多个通道上传输packet类型数据*/
BSP_S32 BSP_ICC_ST_RW_PACKET_004(void)
{
    int i = 0;
    int chanIndex = 0;
    BSP_BOOL idleChan[32] = {0};   /* 用以表示通道是否空闲 */
    ICC_CHAN_ATTR_S attr;
    ICC_CHAN_DEV_S *pChan;

    DEV_INIT();
    for(chanIndex = 0; chanIndex < ICC_INVALID_CHAN_ID; chanIndex++)
    {
        ICC_ListGet(g_pstOpenList, (BSP_U32)chanIndex, &pChan);
        if(pChan)
        {
            idleChan[chanIndex] = BSP_FALSE;
            g_arecvtotal[chanIndex] = 0;
            g_arecvtotalb[chanIndex] = 0;
            continue;
        }
        idleChan[chanIndex] = BSP_TRUE;
        g_arecvtotal[chanIndex] = 0;
        g_arecvtotalb[chanIndex] = 55;
        sema_init(&g_semIccSendTask[chanIndex], SEM_EMPTY);
        attr.enChanMode = ICC_CHAN_MODE_PACKET;
        attr.u32FIFOInSize = 256;
        attr.u32FIFOOutSize = 256;
        attr.u32Priority = 255;
        attr.u32TimeOut = 1000;
        attr.event_cb = event_cb;
        attr.read_cb = read_cb;
        attr.write_cb = write_cb;
        CHECK_RET(BSP_ICC_Open(chanIndex, &attr), BSP_OK);
    }

    i=0;
    while(i++ <10)
    {
        g_senddata[i] = i;
    }

    iccTestDelay(100);
    i=0;
    while(i++ <10)
    {
        for(chanIndex=0; chanIndex<ICC_INVALID_CHAN_ID; chanIndex++)
        {
            if(BSP_TRUE == idleChan[chanIndex])
            {
                CHECK_RET(BSP_ICC_Write(chanIndex, g_senddata, i), i);
            }
        }
    }

    do
    {
        iccTestDelay(100);
        for(chanIndex = 0; chanIndex < ICC_INVALID_CHAN_ID; chanIndex ++)
        {
            ICC_TEST_DEBUG("%s  %d  id=%d\n",__FUNCTION__, __LINE__, chanIndex);
            if(g_arecvtotal[chanIndex] != g_arecvtotalb[chanIndex])
            {
                break;
            }
        }
    }while(chanIndex != ICC_INVALID_CHAN_ID);

    iccTestDelay(300);
    for(chanIndex=0; chanIndex<ICC_INVALID_CHAN_ID; chanIndex++)
    {
        if(BSP_TRUE == idleChan[chanIndex])
        {
            CHECK_RET(BSP_ICC_Close(chanIndex), BSP_OK);
        }
    }

    ICC_TEST_DEBUG("TEST RW PACKET CASE 4 PASSED\n");
    return OK;
}

/* TEST:A ->C发送512字节数据速率测试*/
BSP_S32 BSP_ICC_ST_SPEED_001(void)
{
    int i = 0;
    int sendtime = 0;
    ICC_CHAN_ATTR_S attr;
    DEV_INIT();

    g_arecvtotal[1] = 0;
    g_arecvtotalb[1] = TST_SPEED_DATA_SIZE;
    sema_init(&g_semIccSendTask[1], SEM_EMPTY);
    attr.enChanMode = ICC_CHAN_MODE_STREAM;
    attr.u32FIFOInSize = 1024;
    attr.u32FIFOOutSize = 1024;
    attr.u32Priority = 255;
    attr.u32TimeOut = 1000;
    attr.event_cb = event_cb;
    attr.read_cb = read_cb;
    attr.write_cb = write_cb;
    CHECK_RET(BSP_ICC_Open(1, &attr), BSP_OK);

    /*数据标签+ 数据实体，记录发送的时间戳*/
    sendtime = 0;//(BSP_S32)omTimerGet();
    g_senddata[i++] = (sendtime & 0xFF);
    g_senddata[i++] = ((sendtime >> 8) & 0xFF);
    g_senddata[i++] = ((sendtime >> 16) & 0xFF);
    g_senddata[i++] = ((sendtime >> 24) & 0xFF);

    while(i < TST_SPEED_DATA_SIZE)
    {
        g_senddata[i] = 0;
        i++;
    }

    CHECK_RET(BSP_ICC_Write(1, g_senddata, TST_SPEED_DATA_SIZE), TST_SPEED_DATA_SIZE);

    msleep(3000);
    CHECK_RET(BSP_ICC_Close(1), BSP_OK);

    ICC_TEST_DEBUG("TEST RW SPEED CASE 1 PASSED\n");
    return OK;
}
/* TEST:C ->A发送512字节数据速率测试*/
BSP_S32 BSP_ICC_ST_SPEED_002(void)
{
    ICC_CHAN_ATTR_S attr;
    DEV_INIT();
    g_arecvtotal[1] = 0;
    g_arecvtotalb[1] = TST_SPEED_DATA_SIZE;
    sema_init(&g_semIccSendTask[1], SEM_EMPTY);
    attr.enChanMode = ICC_CHAN_MODE_STREAM;
    attr.u32FIFOInSize = 1024;
    attr.u32FIFOOutSize = 1024;
    attr.u32Priority = 255;
    attr.u32TimeOut = 1000;
    attr.event_cb = event_cb;
    attr.read_cb = read_cb_speed;
    attr.write_cb = write_cb;
    CHECK_RET(BSP_ICC_Open(1, &attr), BSP_OK);

    down(&g_semIccSendTask[1]);
    return OK;
}
/* TEST:连接SDT或者通过shell口可查看packet数据头被自动加上时间戳*/
BSP_S32 BSP_ICC_ST_MNTN_001(void)
{
    int i = 0;
    ICC_CHAN_ATTR_S attr;

    DEV_INIT();

    attr.enChanMode = ICC_CHAN_MODE_PACKET;
    attr.u32FIFOInSize = 256;
    attr.u32FIFOOutSize = 256;
    attr.u32Priority = 255;
    attr.u32TimeOut = 1000;
    attr.event_cb = event_cb;
    attr.read_cb = read_cb;
    attr.write_cb = write_cb;
    CHECK_RET(BSP_ICC_Open(1, &attr), BSP_OK);

    g_arecvtotal[1] = 0;
    g_arecvtotalb[1] = 55;

    sema_init(&g_semIccSendTask[1], SEM_EMPTY);

    i=0;
    while(i++ <10)
    {
        g_senddata[i] = i;
    }

    i=0;
    while(i++ <10)
    {
        CHECK_RET(BSP_ICC_Write(1, g_senddata, i), i);
    }

    down(&g_semIccSendTask[1]);

    iccTestDelay(300);
    CHECK_RET(BSP_ICC_Close(1), BSP_OK);

    return OK;
}
/* TEST:通道打开关闭失败可维可测*/
BSP_S32 BSP_ICC_ST_MNTN_002(void)
{
    ICC_CHAN_ATTR_S attr;

    DEV_INIT();
    /* 利用打开无效通道，产生通道打开失败 */
    CHECK_RET(BSP_ICC_Open(32, &attr), BSP_ERR_ICC_INVALID_CHAN);

    return OK;
}
/* TEST:通道写错误可维可测*/
BSP_S32 BSP_ICC_ST_MNTN_003(void)
{
    int i = 0;
    ICC_CHAN_ATTR_S attr;

    DEV_INIT();

    attr.enChanMode = ICC_CHAN_MODE_STREAM;
    attr.u32FIFOInSize = 128;
    attr.u32FIFOOutSize = 128;
    attr.u32Priority = 255;
    attr.u32TimeOut = 1000;
    attr.event_cb = event_cb;
    attr.read_cb = read_cb;
    attr.write_cb = write_cb;
    CHECK_RET(BSP_ICC_Open(1, &attr), BSP_OK);
    g_arecvtotal[1] = 0;
    g_arecvtotalb[1] = 250;

    sema_init(&g_semIccSendTask[1], SEM_EMPTY);

    i=0;
    while(i++ <250)
    {
        g_senddata[i] = i;
    }

    iccTestDelay(10);
    /* 发送250 个字节数据*/
    CHECK_RET(BSP_ICC_Write(1, g_senddata,250 ), 128);

    down(&g_semIccSendTask[1]);

    iccTestDelay(300);
    CHECK_RET(BSP_ICC_Close(1), BSP_OK);

    return OK;
}
/* TEST:通道读错误可维可测*/
BSP_S32 BSP_ICC_ST_MNTN_004(void)
{
    BSP_U8 *dataBuf = NULL;

    DEV_INIT();
    dataBuf = kmalloc(100, GFP_KERNEL);

    /* 利用打开无效通道，产生通道打开失败 */
    CHECK_RET(BSP_ICC_Read(32, dataBuf, 100), BSP_ERR_ICC_INVALID_PARAM);

    kfree(dataBuf);

    return OK;
}
/* TEST:通道溢出可维可测*/
BSP_S32 BSP_ICC_ST_MNTN_005(void)
{
    int i = 0;
    ICC_CHAN_ATTR_S attr;

    DEV_INIT();

    attr.enChanMode = ICC_CHAN_MODE_PACKET;
    attr.u32FIFOInSize = 256;
    attr.u32FIFOOutSize = 256;
    attr.u32Priority = 255;
    attr.u32TimeOut = 1000;
    attr.event_cb = event_cb;
    attr.read_cb = read_cb_full;
    attr.write_cb = write_cb;
    CHECK_RET(BSP_ICC_Open(1, &attr), BSP_OK);

    sema_init(&g_semIccSendTask[1], SEM_EMPTY);

    i=0;
    while(i++ < 500)
    {
        g_senddata[i] = i;
    }

    i=0;
    while(i++ <10)
    {
        CHECK_RET(BSP_ICC_Write(1, g_senddata, 50), 50);
    }

    down(&g_semIccSendTask[1]);

    iccTestDelay(300);
    CHECK_RET(BSP_ICC_Close(1), BSP_OK);

    return OK;
}


#ifdef __cplusplus
}
#endif
