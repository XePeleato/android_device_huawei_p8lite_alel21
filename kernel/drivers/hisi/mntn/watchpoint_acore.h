#ifndef   MNTN_WATCHPOINT_ACORE_H
#define   MNTN_WATCHPOINT_ACORE_H


enum acpu_num {
    ACPU0,
    ACPU1,
    ACPU2,
    ACPU3,
    ACPUMAX
};


typedef struct watchpoint_cmp_addr {
    unsigned long ref_addr;
    unsigned int  is_caller_in_stack;
} watchpoint_cmp_addr_t;

#endif
