#ifndef   MNTN_WATCHPOINT_IF_H
#define   MNTN_WATCHPOINT_IF_H


typedef struct watchpoint {
    unsigned int addr;
    unsigned int enable;
    unsigned int point_type;
    unsigned int addr_mask_bits;
    void* user_param;
    int (*watchpoint_callback)(unsigned int addr, void* regs, void* user_param);
} watchpoint_t;

/*watchpoint operation method*/
extern int wp_set_watchpoints(uint32_t addr, uint32_t osize, uint32_t rw_type, uint32_t cb, uint32_t cb_param, uint32_t cpus);
extern int wp_set_watchpoints_ex(watchpoint_t* wp, uint32_t cpus);
extern int wp_clear_watchpoints(uint32_t addr, uint32_t cpus);

#endif
