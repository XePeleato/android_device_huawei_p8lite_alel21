
#include <asm/compiler.h>
#include "atfdriver.h"

noinline int atfd_hisi_fn_smc(u64 function_id, u64 arg0, u64 arg1,  u64 arg2)
{
    asm volatile(
            __asmeq("%0", "x0")
            __asmeq("%1", "x1")
            __asmeq("%2", "x2")
            __asmeq("%3", "x3")
            "smc    #0\n"
        : "+r" (function_id)
        : "r" (arg0), "r" (arg1), "r" (arg2));

    return function_id;
}
