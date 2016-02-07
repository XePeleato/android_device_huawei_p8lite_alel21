

#ifdef __cplusplus

extern "C" {

#endif
#include "bsp_timer_if.h"

extern int init_om_hardware_timer(void);


extern TIMER_INIT_FLAG g_om_timer_inited;

int start_om_hard_timer_test_uninited(unsigned int value)
{
    g_om_timer_inited = TIMER_UNINITED;
    return start_om_hard_timer(value);
}

int start_om_hard_timer_test_inited(unsigned int value)
{
    g_om_timer_inited = TIMER_INITED;
    return start_om_hard_timer(value);
}

int stop_om_hard_timer_test_uninited()
{
    g_om_timer_inited = TIMER_UNINITED;
    return stop_om_hard_timer();
}

int stop_om_hard_timer_test_inited()
{
    g_om_timer_inited = TIMER_INITED;
    return stop_om_hard_timer();
}

int get_om_hard_timer_curtime_test_uninited()
{
    g_om_timer_inited = TIMER_UNINITED;
    return get_om_hard_timer_curtime();
}

int get_om_hard_timer_curtime_test_inited()
{
    g_om_timer_inited = TIMER_INITED;
    return get_om_hard_timer_curtime();
}

int clear_om_hard_timer_int_test_uninited()
{
    g_om_timer_inited = TIMER_UNINITED;
    return clear_om_hard_timer_int();
}

int clear_om_hard_timer_int_test_inited()
{
    g_om_timer_inited = TIMER_INITED;
    return clear_om_hard_timer_int();
}


#ifdef __cplusplus

        }

#endif
