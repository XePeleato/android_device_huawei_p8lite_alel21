

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include  "drv_tele_mntn_common.h"
#include  "drv_tele_mntn_platform.h"
#include  "general_sram_map.h"
#include  "drv_config.h"


/*****************************************************************************
  2 全局变量定义
*****************************************************************************/
#ifndef WIN32
local_t u64_t *tele_mntn_func_sw = TELE_MNTN_NULL;
#else
local_t u64_t pc_ut_st_tmp = ~((u64_t)0);
local_t u64_t *tele_mntn_func_sw = &pc_ut_st_tmp;
#endif
/*****************************************************************************
  3 函数实现
*****************************************************************************/


s32_t tele_mntn_is_func_on (TELE_MNTN_TYPE_ID type_id)
{
/*lint -e571*/
    if((TELE_MNTN_NULL != tele_mntn_func_sw)&&((*tele_mntn_func_sw) & BIT_64(type_id)))
    {
        return TELE_MNTN_OK;
    }
    return TELE_MNTN_ERRO;
/*lint +e571*/
}

s32_t tele_mntn_common_init (void_t)
{
#ifndef WIN32
    tele_mntn_func_sw = (u64_t *)TELE_MNTN_AXI_PHY_TO_VIRT(TELE_MNTN_ALIGN(CONFIG_TELE_MNTN_ADDR, sizeof(u64_t)), sizeof(u64_t));
#endif
    return TELE_MNTN_OK;
}

u64_t tele_mntn_func_sw_get (void_t)
{
    if(TELE_MNTN_NULL != tele_mntn_func_sw)
    {
        return (*tele_mntn_func_sw);
    }
    return 0;
}

u64_t tele_mntn_func_sw_set (u64_t sw)
{
    if(TELE_MNTN_NULL != tele_mntn_func_sw)
    {
        *tele_mntn_func_sw = sw;
        return (*tele_mntn_func_sw);
    }
    return 0;
}

u64_t tele_mntn_func_sw_bit_set (u32_t bit)
{
    if(TELE_MNTN_NULL != tele_mntn_func_sw)
    {
        *tele_mntn_func_sw = ((*tele_mntn_func_sw) & (~(BIT_64(bit))))|BIT_64(bit);
        return (*tele_mntn_func_sw);
    }
    return 0;
}

u64_t tele_mntn_func_sw_bit_clr (u32_t bit)
{
    if(TELE_MNTN_NULL != tele_mntn_func_sw)
    {
        *tele_mntn_func_sw = ((*tele_mntn_func_sw) & (~(BIT_64(bit))));
        return (*tele_mntn_func_sw);
    }
    return 0;
}
#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

