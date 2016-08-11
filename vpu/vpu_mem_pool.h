#ifndef _VPU_MEM_POOL_H_
#define _VPU_MEM_POOL_H_

#ifdef __cplusplus
extern "C" {
#endif

#define ENABLE_VPU_MEMORY_POOL_ALLOCATOR    1
#define VPU_MEMORY_POOL_MANAGER_ENABLE      1

#define vpu_display_mem_pool_FIELDS \
 	int (*commit_hdl)(vpu_display_mem_pool *p, int hdl, int size); \
 	void* (*get_free)(vpu_display_mem_pool *p); \
    int (*inc_used)(vpu_display_mem_pool *p, int hdl); \
    int (*put_used)(vpu_display_mem_pool *p, int hdl); \
    int (*reset)(vpu_display_mem_pool *p); \
    int (*get_unused_num)(vpu_display_mem_pool *p); \
    int buff_size;\
    float version; \
    int res[18];

typedef struct vpu_display_mem_pool vpu_display_mem_pool;
struct vpu_display_mem_pool {
    vpu_display_mem_pool_FIELDS
};

vpu_display_mem_pool* open_vpu_memory_pool();
void close_vpu_memory_pool(vpu_display_mem_pool *p);

#if ENABLE_VPU_MEMORY_POOL_ALLOCATOR
int create_vpu_memory_pool_allocator(vpu_display_mem_pool **ipool, int num, int size);
void release_vpu_memory_pool_allocator(vpu_display_mem_pool *ipool);
#endif

#ifdef __cplusplus
}
#endif

#endif