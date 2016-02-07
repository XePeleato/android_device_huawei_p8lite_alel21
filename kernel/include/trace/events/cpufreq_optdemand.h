#undef TRACE_SYSTEM
#define TRACE_SYSTEM cpufreq_optdemand

#if !defined(_TRACE_CPUFREQ_OPTDEMAND_H) || defined(TRACE_HEADER_MULTI_READ)
#define _TRACE_CPUFREQ_OPTDEMAND_H

#include <linux/tracepoint.h>

DECLARE_EVENT_CLASS(timer,
	TP_PROTO(u32 cpu_id, unsigned long cur),
	TP_ARGS(cpu_id, cur),

	TP_STRUCT__entry(
	    __field(u32, cpu_id)
	    __field(unsigned long, cur)
	   ),

	TP_fast_assign(
	    __entry->cpu_id = (u32) cpu_id;
	    __entry->cur = cur;
	),

	TP_printk("cpu=%u cur=%8lu",
	      __entry->cpu_id, __entry->cur)
);

DEFINE_EVENT(timer, cpufreq_optdemand_notyet,
	TP_PROTO(u32 cpu_id, unsigned long cur),
	TP_ARGS(cpu_id, cur)
);

DECLARE_EVENT_CLASS(loadeval,
	    TP_PROTO(unsigned long cpu_id, unsigned long *loads,
		     unsigned long cur, unsigned long targ),
		TP_ARGS(cpu_id, loads, cur, targ),

	    TP_STRUCT__entry(
		    __field(unsigned long, cpu_id)
		    __array(unsigned long, load, NR_CPUS)
		    __field(unsigned long, cur)
		    __field(unsigned long, targ)
	    ),

	    TP_fast_assign(
		    __entry->cpu_id = cpu_id;
		    memcpy(__entry->load, loads, NR_CPUS * sizeof(long));
		    __entry->cur = cur;
		    __entry->targ = targ;
	    ),

#if (NR_CPUS == 8)
	    TP_printk("cpu=%lu cur=%8lu targ=%8lu load=%3lu %3lu %3lu %3lu %3lu %3lu %3lu %3lu",
		      __entry->cpu_id, __entry->cur, __entry->targ, __entry->load[0], __entry->load[1],
			  __entry->load[2], __entry->load[3], __entry->load[4], __entry->load[5],
			  __entry->load[6], __entry->load[7])
#elif (NR_CPUS == 4)
	    TP_printk("cpu=%lu cur=%8lu targ=%8lu load=%3lu %3lu %3lu %3lu",
		      __entry->cpu_id, __entry->cur, __entry->targ, __entry->load[0], __entry->load[1],
			  __entry->load[2], __entry->load[3])
#else
	    TP_printk("cpu=%lu cur=%8lu targ=%8lu",
		      __entry->cpu_id, __entry->cur, __entry->targ)
#endif
);

DEFINE_EVENT(loadeval, cpufreq_optdemand_setrate,
	    TP_PROTO(unsigned long cpu_id, unsigned long *loads,
		     unsigned long cur, unsigned long targ),
	    TP_ARGS(cpu_id, loads, cur, targ)
);

DECLARE_EVENT_CLASS(boost,
	    TP_PROTO(unsigned long cpu_id, unsigned long cur, unsigned long targ),
		TP_ARGS(cpu_id, cur, targ),

	    TP_STRUCT__entry(
		    __field(unsigned long, cpu_id)
		    __field(unsigned long, cur)
		    __field(unsigned long, targ)
	    ),

	    TP_fast_assign(
		    __entry->cpu_id = cpu_id;
		    __entry->cur = cur;
		    __entry->targ = targ;
	    ),

	    TP_printk("cpu=%lu cur=%8lu targ=%8lu",
		      __entry->cpu_id, __entry->cur, __entry->targ)
);

DEFINE_EVENT(boost, cpufreq_optdemand_boost_setrate,
	    TP_PROTO(unsigned long cpu_id, unsigned long cur, unsigned long targ),
	    TP_ARGS(cpu_id, cur, targ)
);
TRACE_EVENT(cpufreq_optdemand_boost,
	    TP_PROTO(const char *s),
	    TP_ARGS(s),
	    TP_STRUCT__entry(
		    __string(s, s)
	    ),
	    TP_fast_assign(
		    __assign_str(s, s);
	    ),
	    TP_printk("%s", __get_str(s))
);

TRACE_EVENT(cpufreq_optdemand_unboost,
	    TP_PROTO(const char *s),
	    TP_ARGS(s),
	    TP_STRUCT__entry(
		    __string(s, s)
	    ),
	    TP_fast_assign(
		    __assign_str(s, s);
	    ),
	    TP_printk("%s", __get_str(s))
);

#endif /* _TRACE_CPUFREQ_OPTDEMAND_H */

/* This part must be outside protection */
#include <trace/define_trace.h>
