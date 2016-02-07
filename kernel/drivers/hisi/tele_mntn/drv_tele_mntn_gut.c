

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include  "drv_tele_mntn_gut.h"
#include  "drv_tele_mntn_common.h"
#include  "drv_tele_mntn_platform.h"


/*****************************************************************************
  2 全局变量定义
*****************************************************************************/

#ifdef WIN32
local_t u8_t g_pc_ut_test_buf[64] = {0};
#ifdef TELE_MNTN_AREA_ADDR
#undef TELE_MNTN_AREA_ADDR
#endif
#ifdef TELE_MNTN_AREA_SIZE
#undef TELE_MNTN_AREA_SIZE
#endif
#define TELE_MNTN_AREA_ADDR  ((u32_t)g_pc_ut_test_buf)
#define TELE_MNTN_AREA_SIZE  (sizeof(g_pc_ut_test_buf))
#endif

/*总控制句柄*/
local_t TELE_MNTN_HANDLE   g_tele_mntn_handle = {0};
local_t TELE_MNTN_CFG_STRU g_tele_mntn_cfg_tbl = {
    TELE_MNTN_AREA_ADDR,
    TELE_MNTN_AREA_SIZE,
    TELE_MNTN_AREA_SIZE*3/4,
    600,
    64,
    -1/*not used*/,
    IPC_SEM_TELE_MNTN,
    TELE_MNTN_NULL,
};
/*****************************************************************************
  3 函数实现
*****************************************************************************/

s32_t tele_mntn_init(void_t)
{
    s32_t ret = TELE_MNTN_OK;

    if (TELE_MNTN_INIT_MAGIC == g_tele_mntn_handle.init_flag) {
        ret = TELE_MNTN_ALREADY_INIT;
        TELE_MNTN_TRACE_OUT(ret, 0);
        return ret;
    }

    if(TELE_MNTN_OK != (ret = tele_mntn_common_init()))
    {
        TELE_MNTN_TRACE_OUT(ret, 0);
        return ret;
    }

    if(TELE_MNTN_OK != (ret = tele_mntn_init_cfg()))
    {
        TELE_MNTN_TRACE_OUT(ret, 0);
        return ret;
    }

    TELE_MNTN_MEMSET(&g_tele_mntn_handle, 0x00, sizeof(TELE_MNTN_HANDLE));

    g_tele_mntn_handle.queue.base = g_tele_mntn_cfg_tbl.data_virt_addr
        + TELE_MNTN_ALIGN((TELE_MNTN_HEAD_SIZE + (2*TELE_MNTN_PROTECT_LEN)), TELE_MNTN_ALIGN_SIZE);

    g_tele_mntn_handle.queue.length = g_tele_mntn_cfg_tbl.data_size
        - TELE_MNTN_ALIGN((TELE_MNTN_HEAD_SIZE + (2*TELE_MNTN_PROTECT_LEN)), TELE_MNTN_ALIGN_SIZE)
        - TELE_MNTN_ALIGN((2*TELE_MNTN_PROTECT_LEN), TELE_MNTN_ALIGN_SIZE);

#ifdef TELE_MNTN_DEBUG_SW
    g_tele_mntn_handle.debug.peak_left_size = g_tele_mntn_handle.queue.length;
#endif

    TELE_MNTN_INTLOCK_INIT(&g_tele_mntn_handle.lock);

    g_tele_mntn_handle.init_flag = TELE_MNTN_INIT_MAGIC;

    return ret;
}

s32_t tele_mntn_init_cfg(void_t)
{
    s32_t ret = TELE_MNTN_OK;

#if defined (BSP_CORE_CM3)
    struct tele_mntn_head *head = TELE_MNTN_NULL;
#endif
    g_tele_mntn_cfg_tbl.data_virt_addr = TELE_MNTN_PHY_TO_VIRT(g_tele_mntn_cfg_tbl.data_addr, g_tele_mntn_cfg_tbl.data_size);
    if(TELE_MNTN_NULL == g_tele_mntn_cfg_tbl.data_virt_addr)
    {
        ret = TELE_MNTN_MUTEX_VIRT_ADDR_NULL;
        return ret;
    }

#if defined (BSP_CORE_CM3)
    head       = (struct tele_mntn_head*)g_tele_mntn_cfg_tbl.data_virt_addr;

    TELE_MNTN_WRITE_REG(g_tele_mntn_cfg_tbl.data_virt_addr + TELE_MNTN_HEAD_SIZE, TELE_MNTN_PROTECT1);
    TELE_MNTN_WRITE_REG(g_tele_mntn_cfg_tbl.data_virt_addr + TELE_MNTN_HEAD_SIZE + TELE_MNTN_PROTECT_LEN, TELE_MNTN_PROTECT2);
    TELE_MNTN_WRITE_REG((g_tele_mntn_cfg_tbl.data_virt_addr + g_tele_mntn_cfg_tbl.data_size) - (2*TELE_MNTN_PROTECT_LEN),
                TELE_MNTN_PROTECT1);
    TELE_MNTN_WRITE_REG((g_tele_mntn_cfg_tbl.data_virt_addr + g_tele_mntn_cfg_tbl.data_size) - TELE_MNTN_PROTECT_LEN,
                TELE_MNTN_PROTECT2);

    head->protect_word1 = TELE_MNTN_PROTECT1;
    head->front         = 0;
    head->rear          = head->front;
    head->protect_word2 = TELE_MNTN_PROTECT2;
#endif

    return ret;
}



s32_t tele_mntn_write_log(TELE_MNTN_TYPE_ID type_id, u32_t len, void_t * data)
{
    s32_t ret                     = TELE_MNTN_ERRO;
    u32_t data_len                = 0;
    TELE_MNTN_DATA_STRU cur_data  = {0};
    TELE_MNTN_HANDLE *cur_handle  = &g_tele_mntn_handle;
    TELE_MNTN_CFG_STRU *cur_table = &g_tele_mntn_cfg_tbl;
    TELE_MNTN_BUF_TYPE buf_type   = TELE_MNTN_BUF_INCONTROLLABLE;

    if(TELE_MNTN_INIT_MAGIC != cur_handle->init_flag)
    {
        ret = TELE_MNTN_NOT_INIT;
        return ret;
    }

    if(TELE_MNTN_OK != tele_mntn_is_func_on(type_id))
    {
        return TELE_MNTN_OK;
    }

    if(TELE_MNTN_OK == tele_mntn_is_func_on(TELE_MNTN_NVME_LOGCAT))
    {
        buf_type = TELE_MNTN_BUF_CONTROLLABLE;
    }

    if(len > cur_table->user_data_threshold)
    {
        ret = TELE_MNTN_USER_DATA_OVERFLOW;
        return ret;
    }

    TELE_MNTN_INT_LOCK(&cur_handle->lock, cur_handle->flag);
    TELE_MNTN_IPC_SEM_LOCK(cur_table->ipc_sem_num);

    data_len         = TELE_MNTN_ALIGN(len, TELE_MNTN_ALIGN_SIZE);
    cur_data.protect = TELE_MNTN_PROTECT;
    /*lint -e571*/
    cur_data.type_id = (u32_t)type_id;
    /*lint +e571*/
    cur_data.idex    = cur_handle->idex++;
    cur_data.cpu_id  = TELE_MNTN_CUR_CPUID;
    cur_data.rtc     = TELE_MNTN_RTC_TIME_GET();
    cur_data.slice   = TELE_MNTN_SLICE_TIME_GET();
    cur_data.len     = data_len;

    if(TELE_MNTN_OK != (ret = tele_mntn_write_buf(&cur_data, data, data_len, buf_type)))
    {
#ifdef TELE_MNTN_DEBUG_SW
        cur_handle->debug.discard_silce[(cur_handle->debug.discard_num) & (TELE_MNTN_DISCARD_SLICE_BUF_NUM -1)] = TELE_MNTN_SLICE_TIME_GET();
        cur_handle->debug.discard_num++;
#endif
        TELE_MNTN_IPC_SEM_UNLOCK(cur_table->ipc_sem_num);
        TELE_MNTN_INT_UNLOCK(&cur_handle->lock, cur_handle->flag);
        return ret;
    }

    TELE_MNTN_IPC_SEM_UNLOCK(cur_table->ipc_sem_num);
    TELE_MNTN_INT_UNLOCK(&cur_handle->lock, cur_handle->flag);

    return TELE_MNTN_OK;

}



s32_t tele_mntn_write_buf(TELE_MNTN_DATA_STRU *head_data, void_t * data, u32_t len, TELE_MNTN_BUF_TYPE buf_type)
{
    TELE_MNTN_QUEUE_STRU *cur_queue = &g_tele_mntn_handle.queue;
    TELE_MNTN_HEAD_STRU *head       = (TELE_MNTN_HEAD_STRU*)g_tele_mntn_cfg_tbl.data_virt_addr;
    u32_t size_to_bottom            = 0;

    if(TELE_MNTN_PROTECT1 != head->protect_word1
    || TELE_MNTN_PROTECT2 != head->protect_word2)
    {
        return TELE_MNTN_PROTECT_ERR;
    }

    cur_queue->front = cur_queue->base + head->front * TELE_MNTN_ALIGN_SIZE;
    if(TELE_MNTN_BUF_CONTROLLABLE == buf_type)
    {
        cur_queue->rear  = cur_queue->base + head->rear * TELE_MNTN_ALIGN_SIZE;
    }
    else
    {
        cur_queue->rear  = cur_queue->front;
    }

    if(cur_queue->front >= cur_queue->rear)
    {
        size_to_bottom = (u32_t)(cur_queue->base + cur_queue->length - cur_queue->front);
        if(size_to_bottom <= len + TELE_MNTN_DATA_SIZE)
            cur_queue->front = cur_queue->base;
    }

    if(cur_queue->front < cur_queue->rear)
    {
        size_to_bottom = (u32_t)(cur_queue->rear - cur_queue->front);
    }

    if(size_to_bottom > len + TELE_MNTN_DATA_SIZE)
    {
        TELE_MNTN_MEMCPY((void_t *)cur_queue->front, (void_t *)head_data, TELE_MNTN_DATA_SIZE);
        cur_queue->front += TELE_MNTN_DATA_SIZE;
        TELE_MNTN_MEMCPY((void_t *)cur_queue->front, (void_t *)data, len);
        cur_queue->front += len;
        head->front = ((u32_t)(cur_queue->front - cur_queue->base))/TELE_MNTN_ALIGN_SIZE;
        if(TELE_MNTN_BUF_CONTROLLABLE != buf_type)
        {
            head->rear = head->front;
        }
    }
    else
    {
        return TELE_MNTN_BUF_FULL;
    }

    return TELE_MNTN_OK;

}

s32_t tele_mntn_wake_up_acore(void_t)
{
    TELE_MNTN_HANDLE *cur_handle = &g_tele_mntn_handle;
    TELE_MNTN_QUEUE_STRU *cur_queue = &g_tele_mntn_handle.queue;
    TELE_MNTN_HEAD_STRU *head       = (TELE_MNTN_HEAD_STRU*)g_tele_mntn_cfg_tbl.data_virt_addr;
    TELE_MNTN_CFG_STRU *cur_table = &g_tele_mntn_cfg_tbl;
    u8_t *cur_front = TELE_MNTN_NULL;
    u8_t *cur_rear = TELE_MNTN_NULL;
    u32_t left_size = 0;
    u32_t cur_slice = 0;

#ifdef TELE_MNTN_DEBUG_SW
    TELE_MNTN_DEBUG_STRU *cur_debug = &g_tele_mntn_handle.debug;
#endif
    if(TELE_MNTN_OK != tele_mntn_is_func_on((TELE_MNTN_TYPE_ID)TELE_MNTN_NVME_WAKEUP_ACPU))
    {
        return TELE_MNTN_OK;
    }
    if(TELE_MNTN_INIT_MAGIC != g_tele_mntn_handle.init_flag)
    {
        return TELE_MNTN_NOT_INIT;
    }

    cur_front = cur_queue->base + head->front * TELE_MNTN_ALIGN_SIZE;;
    cur_rear = cur_queue->base + head->rear * TELE_MNTN_ALIGN_SIZE;


    if(cur_front < cur_rear)
    {
        left_size = (u32_t)(cur_rear - cur_front) - TELE_MNTN_ALIGN_SIZE;
    }
    else
    {
        left_size = cur_queue->length - (u32_t)(cur_front - cur_rear) - TELE_MNTN_ALIGN_SIZE;
    }

    if(left_size < cur_table->wake_threshold)
    {
        cur_slice = TELE_MNTN_SLICE_TIME_GET();
        if((cur_slice - cur_handle->prior_wakeup_acore_slice) >= TELE_MNTN_SECOND_TO_SLICE(cur_table->wake_interval_threshold))
        {
#if defined (BSP_CORE_CM3)
            TELE_MNTN_IPC_INT_SEND(IPC_CORE_ACPU, cur_table->ipc_int_num);
#endif
            cur_handle->prior_wakeup_acore_slice = cur_slice;
        }

#ifdef TELE_MNTN_DEBUG_SW
        cur_debug->wakeup_accore_silce[(cur_debug->wakeup_accore_num)&(TELE_MNTN_WUACORE_SLICE_BUF_NUM-1)] = cur_slice;
        cur_debug->wakeup_accore_num++;
#endif

    }

#ifdef TELE_MNTN_DEBUG_SW
    if(left_size < cur_debug->peak_left_size)
    {
        cur_debug->peak_left_size = left_size;
        cur_debug->peak_left_slice = TELE_MNTN_SLICE_TIME_GET();
    }
#endif
    return TELE_MNTN_OK;
}

s32_t tele_mntn_get_head(struct tele_mntn_queue *queue)
{
    s32_t ret = TELE_MNTN_OK;
    TELE_MNTN_QUEUE_STRU *cur_queue = &g_tele_mntn_handle.queue;
    TELE_MNTN_HEAD_STRU *head       = (TELE_MNTN_HEAD_STRU*)g_tele_mntn_cfg_tbl.data_virt_addr;

    if(TELE_MNTN_IS_NULL(queue))
    {
        ret = TELE_MNTN_PARA_INVALID;
        return ret;
    }

    if(TELE_MNTN_INIT_MAGIC != g_tele_mntn_handle.init_flag)
    {
        ret = TELE_MNTN_NOT_INIT;
        return ret;
    }

    if(TELE_MNTN_PROTECT1 != head->protect_word1
    || TELE_MNTN_PROTECT2 != head->protect_word2)
    {
        ret = TELE_MNTN_PROTECT_ERR;
        return ret;
    }

    queue->base   = cur_queue->base;
    queue->length = cur_queue->length;
    queue->front  = cur_queue->base + head->front * TELE_MNTN_ALIGN_SIZE;
    queue->rear   = cur_queue->base + head->rear * TELE_MNTN_ALIGN_SIZE;


    return ret;

}


s32_t tele_mntn_set_head(struct tele_mntn_queue *queue)
{
    s32_t ret = TELE_MNTN_OK;
    TELE_MNTN_HEAD_STRU *head       = (TELE_MNTN_HEAD_STRU*)g_tele_mntn_cfg_tbl.data_virt_addr;

    if(TELE_MNTN_IS_NULL(queue))
    {
        ret = TELE_MNTN_PARA_INVALID;
        return ret;
    }

    if(TELE_MNTN_INIT_MAGIC != g_tele_mntn_handle.init_flag)
    {
        ret = TELE_MNTN_NOT_INIT;
        return ret;
    }

    if(TELE_MNTN_PROTECT1 != head->protect_word1
    || TELE_MNTN_PROTECT2 != head->protect_word2)
    {
        ret = TELE_MNTN_PROTECT_ERR;
        return ret;
    }

    head->rear = ((u32_t)(queue->rear - queue->base))/TELE_MNTN_ALIGN_SIZE;

    return ret;

}

#ifdef TELE_MNTN_DEBUG_SW
void_t tele_mntn_debug_out(void_t)
{
    u32_t debug_idex = 0;
    TELE_MNTN_DEBUG_STRU *cur_debug = &g_tele_mntn_handle.debug;
    TELE_MNTN_TRACE_OUT(TELE_MNTN_BUF_FULL, cur_debug->discard_num);
    for(debug_idex = 0; debug_idex < TELE_MNTN_DISCARD_SLICE_BUF_NUM; debug_idex ++)
    {
        TELE_MNTN_TRACE_OUT(TELE_MNTN_BUF_FULL, cur_debug->discard_silce[debug_idex]);
    }

    TELE_MNTN_TRACE_OUT(TELE_MNTN_WAKEUP_ACORE_SUCC, cur_debug->wakeup_accore_num);
    for(debug_idex = 0; debug_idex < TELE_MNTN_WUACORE_SLICE_BUF_NUM; debug_idex ++)
    {
        TELE_MNTN_TRACE_OUT(TELE_MNTN_WAKEUP_ACORE_SUCC, cur_debug->wakeup_accore_silce[debug_idex]);
    }

    TELE_MNTN_TRACE_OUT(TELE_MNTN_PROTECT_ERR, g_tele_mntn_cfg_tbl.data_addr);
    TELE_MNTN_TRACE_OUT(TELE_MNTN_PROTECT_ERR, g_tele_mntn_cfg_tbl.data_size);
    TELE_MNTN_TRACE_OUT(TELE_MNTN_PROTECT_ERR, g_tele_mntn_cfg_tbl.wake_threshold);
    TELE_MNTN_TRACE_OUT(TELE_MNTN_PROTECT_ERR, g_tele_mntn_cfg_tbl.wake_interval_threshold);
    TELE_MNTN_TRACE_OUT(TELE_MNTN_PROTECT_ERR, g_tele_mntn_cfg_tbl.user_data_threshold);
    TELE_MNTN_TRACE_OUT(TELE_MNTN_PROTECT_ERR, g_tele_mntn_cfg_tbl.ipc_int_num);
    TELE_MNTN_TRACE_OUT(TELE_MNTN_PROTECT_ERR, g_tele_mntn_cfg_tbl.ipc_sem_num);
    TELE_MNTN_TRACE_OUT(TELE_MNTN_PROTECT_ERR, (udy_t)(g_tele_mntn_cfg_tbl.data_virt_addr));

}
#endif

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

