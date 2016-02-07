#ifndef __MDRV_COMMON_MEMORY_H__
#define __MDRV_COMMON_MEMORY_H__

#ifdef __cplusplus
extern "C"
{
#endif

/* 内存池类型, Flags标记用 */
typedef enum tagMEM_POOL_TYPE
{
    MEM_NORM_DDR_POOL = 0,
    MEM_ICC_DDR_POOL,
    MEM_ICC_AXI_POOL,
    MEM_POOL_MAX
}MEM_POOL_TYPE;

#ifdef __cplusplus
}
#endif
#endif
