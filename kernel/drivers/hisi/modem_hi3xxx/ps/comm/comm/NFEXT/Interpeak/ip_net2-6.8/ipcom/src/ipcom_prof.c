/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_prof.c,v $ $Revision: 1.20 $
 *     $Source: /home/interpeak/CVSRoot/ipcom/src/ipcom_prof.c,v $
 *     $Author: lob $ $Date: 2005-09-07 11:38:43 $
 *     $State: Exp $ $Locker:  $
 *
 *     INTERPEAK_COPYRIGHT_STRING
 *
 ******************************************************************************
 */

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 2                    CONFIGURATION
 ****************************************************************************
 */

#include "ipcom_config.h"


/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#define IPCOM_USE_CLIB_PROTO
#include "ipcom_clib.h"
#include "ipcom_clib2.h"
#include "ipcom_getopt.h"
#include "ipcom_list.h"
#include "ipcom_os.h"
#include "ipcom_prof.h"
#include "ipcom_shell.h"
#include "ipcom_time.h"
#include "ipcom_err.h"


/*
 ****************************************************************************
 * 4                    PROTOTYPES
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 5                    DEFINES
 ****************************************************************************
 */

#define IPCOM_PROF_ENTRIES 0x10000

/* Set to ipcom_printf for shell use, ipcom_debug_printf for stand-alone use */
#define PRINTF ipcom_printf


/*
 ****************************************************************************
 * 6                    TYPES
 ****************************************************************************
 */

typedef struct Ipcom_prof_listentry_struct
{
    Ipcom_list head;
    Ipcom_prof_entry entry;
    Ip_u64 us;
    Ip_u32 max;
    Ip_u32 min;
    Ip_u32 count;
}
Ipcom_prof_listentry;

typedef struct Ipcom_prof_listhead_struct
{
    Ipcom_list head;
    Ipcom_list list;
    Ip_u64 us;
    Ip_u32 max;
    Ip_u32 min;
    Ip_u32 count;
}
Ipcom_prof_listhead;

typedef struct Ipcom_prof_data_struct
{
    volatile Ip_s32 indx;
    volatile int stopped;
    Ip_u32 overhead;
    Ipcom_prof_entry data[IPCOM_PROF_ENTRIES];
}
Ipcom_prof_data;


/*
 ****************************************************************************
 * 7                    LOCAL PROTOTYPES
 ****************************************************************************
 */


/*
 ****************************************************************************
 * 8                    DATA
 ****************************************************************************
 */

IP_STATIC Ipcom_prof_data ipcom_prof_data;


/*
 ****************************************************************************
 * 9                    STATIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipcom_prof_init
 *===========================================================================
 * Description: Init profiler.
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err
ipcom_prof_init(void *ununsed)
{
    (void)ununsed;

    ipcom_prof_clear();
    ipcom_prof_data.overhead = 0;

    (void)ipcom_shell_add_cmd("prof", "prof [options] command",
                              "Interpeak profiler",
                              ipcom_cmd_prof,
                              IPCOM_PRIORITY_DEFAULT, IPCOM_PROC_STACK_DEFAULT);
    return IPCOM_SUCCESS;
}


/*
 ****************************************************************************
 * 10                    PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipcom_prof_clear
 *===========================================================================
 * Description: Clear profiler data.
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_prof_clear(void)
{
    Ip_u32 overhead = ipcom_prof_data.overhead;

    ipcom_memset(&ipcom_prof_data, 0, sizeof ipcom_prof_data);

    ipcom_prof_data.overhead = overhead;
}


/*
 *===========================================================================
 *                    ipcom_prof_stopped
 *===========================================================================
 * Description: Enable/disable profiling.
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipcom_prof_stopped(int new_prof)
{
    int stopped = ipcom_prof_data.stopped;

    ipcom_prof_data.stopped = new_prof;

    return stopped;
}


/*
 *===========================================================================
 *                    ipcom_prof_enter
 *===========================================================================
 * Description: Enter profiler data.
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_prof_enter(const char *msg, const char *file, int line, int flags)
{
    static Ipcom_once_t once = IPCOM_ONCE_INIT;
#ifdef IPCOM_USE_TIME
    struct Ip_timeval tv;
#endif

    (void)ipcom_once(&once, ipcom_prof_init, IP_NULL);

    if (ipcom_prof_data.stopped == 0 && msg != IP_NULL)
    {
        Ip_s32 indx = ipcom_prof_data.indx++;

        if (indx >= IPCOM_PROF_ENTRIES)
            ipcom_prof_data.indx--;
        else
        {
            Ipcom_prof_entry *pe = &ipcom_prof_data.data[indx];

#if defined(IPCOM_USE_INTERRUPT_TIME)
            {
                typedef struct TimeStruct
                {
                    Ip_s32 Seconds;
                    Ip_u32 Fraction;
                } Time;
                extern void INTERRUPT_GetHighResTime(Time *time);
                Time time;
                INTERRUPT_GetHighResTime(&time);
                pe->us = time.Fraction / 4295;
            }
#elif defined(IPCOM_USE_TIME)
            ipcom_microtime(&tv);
            pe->us = tv.tv_usec;
#else
            pe->us = 0;
#endif
            pe->msg = msg;
            pe->file = file;
            pe->line = (Ip_u16)line;
            pe->type = (msg[1] == ':' ? msg[0] : 0);
            pe->flags = (Ip_u8)flags;

            if (ipcom_prof_data.overhead == 0 && indx > 0 &&
                pe->type == 'R' && (pe - 1)->type == 'R')
            {
                ipcom_prof_data.overhead = pe->us - (pe - 1)->us;
            }

            if (pe->type == 'C')
            {
                ipcom_prof_clear();
            }
        }
    }
}


/*
 *===========================================================================
 *                    ipcom_prof_push
 *===========================================================================
 * Description: Enter profiler data.
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_prof_push(Ipcom_prof_entry *e)
{
    Ip_s32 indx;

    if ((indx = ipcom_prof_data.indx) <= 0)
    {
        ipcom_memset(e, 0, sizeof(*e));
    }
    else
    {
        *e = ipcom_prof_data.data[indx - 1];
    }
}


/*
 *===========================================================================
 *                    ipcom_prof_pop
 *===========================================================================
 * Description: Enter profiler data.
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_prof_pop(Ipcom_prof_entry *e)
{
    ipcom_prof_enter(e->msg, e->file, e->line, IPCOM_PROF_CONTINUED);
}


/*
 *===========================================================================
 *                    ipcom_prof_dump
 *===========================================================================
 * Description: Dump profiler data.
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_prof_dump(int start, int stop, int flags)
{
    int stopped = ipcom_prof_stopped(1);
    int i;

    if (stop < 0 || stop > ipcom_prof_data.indx)
    {
        stop = ipcom_prof_data.indx;
    }

    /* Plain dump of all entries */
    for (i = start; i < stop; i++)
    {
        Ipcom_prof_entry *pe = &ipcom_prof_data.data[i]; /*lint !e676 */
        Ip_u32 us;

        if ((flags & IPCOM_PROF_ABSOLUTE) == 0 && i < (stop - 1))
        {
            us = (pe + 1)->us - pe->us;
            if (us & 0x80000000)
            {
                us = 0;
            }
            else if ((flags & IPCOM_PROF_OVERHEAD) == 0)
            {
                if ((us -= ipcom_prof_data.overhead) & 0x80000000)
                {
                    us = 0;
                }
            }

        }
        else
        {
            us = pe->us;
        }

        if (flags & IPCOM_PROF_LINENUM)
        {
            PRINTF("%s(%d): ", pe->file, pe->line);
        }

        PRINTF("%lu us %s\n", (unsigned long)us, pe->msg);
    }

    (void)ipcom_prof_stopped(stopped);
}


/*
 *===========================================================================
 *                    ipcom_prof_summary
 *===========================================================================
 * Description: Print summary of profiler data.
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_prof_summary(int start, int stop, int flags)
{
    int stopped = ipcom_prof_stopped(1);
    Ipcom_prof_listentry *le;
    Ipcom_prof_listentry *le2;
    Ipcom_prof_listentry *next;
    Ipcom_list list;
    Ip_u64 totus;
    int i;

    IP_U64_CLEAR(totus);

    if (stop < 0 || stop > ipcom_prof_data.indx)
    {
        stop = ipcom_prof_data.indx;
    }

    ipcom_list_init(&list);

    /* Gather data for all entries */
    for (i = start; i < stop; i++)
    {
        Ipcom_prof_entry *pe = &ipcom_prof_data.data[i];  /*lint !e676 */
        Ip_u32 us = (i < (stop - 1) ? (pe + 1)->us - pe->us : pe->us);

        /* Handle special entries */
        if (pe->type == 'E' || pe->type == 'C')
        {
            /* Ignore endmarks and cyber marks */
            continue;
        }

        /* Handle out-of-order stuff */
        if (us & 0x80000000)
        {
            us = 0;
        }
        else if ((flags & IPCOM_PROF_OVERHEAD) == 0)
        {
            if ((us -= ipcom_prof_data.overhead) & 0x80000000)
            {
                us = 0;
            }
        }

        /* Check if already in list */
        for (le = IPCOM_LIST_FIRST(&list);
             le != IP_NULL;
             le = IPCOM_LIST_NEXT(&le->head))
        {
            if (le->entry.msg == pe->msg)
            {
                break;
            }
        }

        if (le == IP_NULL)
        {
            /* Init new listentry */
            le = ipcom_malloc(sizeof(*le));
            ipcom_memset(le, 0, sizeof(*le));
            le->entry = *pe;
            ipcom_list_insert_first(&list, &le->head);
        }

        /* Enter min/max/count */
        if (le->min == 0 || us < le->min)
        {
            le->min = us;
        }
        if (us > le->max)
        {
            le->max = us;
        }
        IP_U64_ADD32(le->us, us);
        IP_U64_ADD32(totus, us);
        if ((pe->flags & IPCOM_PROF_CONTINUED) == 0)
        {
            le->count++;
        }
    }

    /* Sort list by moving all entries to front of list */
    for (le = IPCOM_LIST_FIRST(&list); le != IP_NULL; le = next)
    {
        next = IPCOM_LIST_NEXT(&le->head);

        /* Find location to insert new entry */
        for (le2 = IPCOM_LIST_FIRST(&list);
             le2 != le;
             le2 = IPCOM_LIST_NEXT(&le2->head))
        {
            if (flags & IPCOM_PROF_COUNT)
            {
                if (le2->count < le->count)
                {
                    break;
                }
            }
            else if (IP_U64_GETHI(le2->us) < IP_U64_GETHI(le->us) ||
                     (IP_U64_GETHI(le2->us) == IP_U64_GETHI(le->us) &&
                      IP_U64_GETLO(le2->us) < IP_U64_GETLO(le->us)))
            {
                break;
            }
        }

        /* Don't bother if already last in sorted list */
        if (le != le2)
        {
            ipcom_list_remove(&le->head);
            ipcom_list_insert_before(&le2->head, &le->head);
        }
    }

    /* Print entries */
    while ((le = IPCOM_LIST_FIRST(&list)) != IP_NULL)
    {
        Ip_u64 a = le->us;
        Ip_u64 b = totus;

        if (flags & IPCOM_PROF_LINENUM)
        {
            PRINTF("%s(%d): ", le->entry.file, le->entry.line);
        }

        /* Shift until able to compute percentage in 32 bits */
        while ((IP_U64_GETHI(a) | IP_U64_GETHI(b)) != 0 ||
               ((IP_U64_GETLO(a) | IP_U64_GETLO(b)) & 0xff000000) != 0)
        {
            /* 64-bit shift... */
            IP_U64_SETLO(a, (IP_U64_GETLO(a) >> 1) | (IP_U64_GETHI(a) << 31));
            IP_U64_SETHI(a, IP_U64_GETHI(a) >> 1);
            IP_U64_SETLO(b, (IP_U64_GETLO(b) >> 1) | (IP_U64_GETHI(b) << 31));
            IP_U64_SETHI(b, IP_U64_GETHI(b) >> 1);
        }

        PRINTF("%lu%% %lu(%lu-%lu) us, %lu times, %s\n",
               (unsigned long)((IP_U64_GETLO(a) * 100 + 50) /
                               IP_U64_GETLO(b)),
               (unsigned long)(IP_U64_GETLO(le->us) / le->count),
               (unsigned long)le->min, (unsigned long)le->max,
               (unsigned long)le->count, le->entry.msg);  /*lint !e414 */

        ipcom_list_remove(&le->head);
        ipcom_free(le);
    }

    (void)ipcom_prof_stopped(stopped);
}


/*
 *===========================================================================
 *                    ipcom_prof_sequence
 *===========================================================================
 * Description: Print sequences in profiler data.
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_prof_sequence(int start, int stop, int flags)
{
    int stopped = ipcom_prof_stopped(1);
    Ipcom_list list;
    Ipcom_prof_listhead *head = IP_NULL;
    Ipcom_prof_listhead *seq;
    Ipcom_prof_listhead *next;
    Ipcom_prof_listentry *le;
    Ip_u64 totus;
    int i;

    IP_U64_CLEAR(totus);

    if (stop < 0 || stop > ipcom_prof_data.indx)
    {
        stop = ipcom_prof_data.indx;
    }

    ipcom_list_init(&list);

    /* Gather data for all entries */
    for (i = start; i < stop; i++)
    {
        Ipcom_prof_entry *pe = &ipcom_prof_data.data[i];  /*lint !e676 */
        Ip_u32 us = (i < (stop - 1) ? (pe + 1)->us - pe->us : pe->us);

        /* Allocate new list? */
        if (pe->type == 'B' && head == IP_NULL)
        {
            head = ipcom_malloc(sizeof(*head));
            ipcom_memset(head, 0, sizeof(*head));
            ipcom_list_init(&head->list);
            head->count = 1;
        }

        /* Handle special entries */
        if (pe->type == 'C' || head == IP_NULL)
        {
            /* Ignore cyber marks and entries outside begin/end */
            continue;
        }
        else if (pe->type == 'E')
        {
            /* Enter min/max for sequence */
            head->min = head->max = IP_U64_GETLO(head->us);

            /* Loop through all sequences and check for match */
            for (seq = IPCOM_LIST_FIRST(&list);
                 seq != IP_NULL;
                 seq = IPCOM_LIST_NEXT(&seq->head))
            {
                Ipcom_prof_listentry *le1;
                Ipcom_prof_listentry *le2;

                /* Check one sequence */
                for (le1 = IPCOM_LIST_FIRST(&head->list),
                         le2 = IPCOM_LIST_FIRST(&seq->list);
                     le1 != IP_NULL && le2 != IP_NULL;
                     le1 = IPCOM_LIST_NEXT(&le1->head),
                         le2 = IPCOM_LIST_NEXT(&le2->head))
                {
                    if (le1->entry.msg != le2->entry.msg)
                    {
                        break;
                    }
                }

                /* Match? */
                if (le1 == IP_NULL && le2 == IP_NULL)
                {
                    /* The two sequences match, merge */
                    for (le1 = IPCOM_LIST_FIRST(&head->list),
                             le2 = IPCOM_LIST_FIRST(&seq->list);
                         le1 != IP_NULL && le2 != IP_NULL;
                         le1 = IPCOM_LIST_NEXT(&le1->head),
                             le2 = IPCOM_LIST_NEXT(&le2->head))
                    {
                        /* Enter min/max/count */
                        if (IP_U64_GETLO(le1->us) < le2->min)
                        {
                            le2->min = IP_U64_GETLO(le1->us);
                        }
                        if (IP_U64_GETLO(le1->us) > le2->max)
                        {
                            le2->max = IP_U64_GETLO(le1->us);
                        }
                        IP_U64_ADD32(le2->us, IP_U64_GETLO(le1->us));
                        le2->count += 1;
                    }

                    /* One more occurrence of this sequence */
                    if (IP_U64_GETLO(head->us) < seq->min)
                    {
                        seq->min = IP_U64_GETLO(head->us);
                    }
                    if (IP_U64_GETLO(head->us) > seq->max)
                    {
                        seq->max = IP_U64_GETLO(head->us);
                    }
                    IP_U64_ADD32(seq->us, IP_U64_GETLO(head->us));
                    seq->count++;

                    /* Now release the merged sequence */
                    while ((le1 = IPCOM_LIST_FIRST(&head->list)) != IP_NULL)
                    {
                        ipcom_list_remove(&le1->head);
                        ipcom_free(le1);
                    }
                    ipcom_free(head);

                    /* Done checking sequences */
                    goto done;
                }
            }

            /* No match, enter new sequence in list */
            ipcom_list_insert_first(&list, &head->head);
        done:
            /* Indicate end of sequence */
            head = IP_NULL;
        }
        else
        {
            /* Init new listentry */
            le = ipcom_malloc(sizeof(*le));
            ipcom_memset(le, 0, sizeof(*le));
            le->entry = *pe;
            ipcom_list_insert_last(&head->list, &le->head);

            /* Handle out-of-order stuff */
            if (us & 0x80000000)
            {
                us = 0;
            }
            else if ((flags & IPCOM_PROF_OVERHEAD) == 0)
            {
                if ((us -= ipcom_prof_data.overhead) & 0x80000000)
                {
                    us = 0;
                }
            }

            /* Enter min/max/count */
            le->min = us;
            le->max = us;
            le->count = 1;
            IP_U64_SET(le->us, 0, us);
            IP_U64_ADD32(head->us, us);
            IP_U64_ADD32(totus, us);
        }
    }

    /* Sort list by moving all entries to front of list */
    for (seq = IPCOM_LIST_FIRST(&list); seq != IP_NULL; seq = next)
    {
        Ipcom_prof_listhead *seq2;

        next = IPCOM_LIST_NEXT(&seq->head);

        /* Find location to insert new entry */
        for (seq2 = IPCOM_LIST_FIRST(&list);
             seq2 != seq;
             seq2 = IPCOM_LIST_NEXT(&seq2->head))
        {
            if (flags & IPCOM_PROF_COUNT)
            {
                if (seq2->count < seq->count)
                {
                    break;
                }
            }
            else if (IP_U64_GETHI(seq2->us) < IP_U64_GETHI(seq->us) ||
                     (IP_U64_GETHI(seq2->us) == IP_U64_GETHI(seq->us) &&
                      IP_U64_GETLO(seq2->us) < IP_U64_GETLO(seq->us)))
            {
                break;
            }
        }

        /* Don't bother if already last in sorted list */
        if (seq != seq2)
        {
            ipcom_list_remove(&seq->head);
            ipcom_list_insert_before(&seq2->head, &seq->head);
        }
    }

    /* Print sequences */
    while ((seq = IPCOM_LIST_FIRST(&list)) != IP_NULL)
    {
        Ip_u64 a = seq->us;
        Ip_u64 b = totus;

        /* Shift until able to compute percentage in 32 bits */
        while ((IP_U64_GETHI(a) | IP_U64_GETHI(b)) != 0 ||
               ((IP_U64_GETLO(a) | IP_U64_GETLO(b)) & 0xff000000) != 0)
        {
            /* 64-bit shift... */
            IP_U64_SETLO(a, (IP_U64_GETLO(a) >> 1) | (IP_U64_GETHI(a) << 31));
            IP_U64_SETHI(a, IP_U64_GETHI(a) >> 1);
            IP_U64_SETLO(b, (IP_U64_GETLO(b) >> 1) | (IP_U64_GETHI(b) << 31));
            IP_U64_SETHI(b, IP_U64_GETHI(b) >> 1);
        }

        if (seq->count)
            PRINTF("\nSEQUENCE: %lu%% %lu(%lu-%lu) us, %lu times\n\n",
                   (unsigned long)((IP_U64_GETLO(a) * 100 + 50) /
                                   IP_U64_GETLO(b)),
                   (unsigned long)(IP_U64_GETLO(seq->us) / seq->count),
                   (unsigned long)seq->min, (unsigned long)seq->max,
                   (unsigned long)seq->count);   /*lint !e414 */

        /* Print one sequence */
        while ((le = IPCOM_LIST_FIRST(&seq->list)) != IP_NULL)
        {
            a = le->us;
            b = seq->us;

            if (flags & IPCOM_PROF_LINENUM)
            {
                PRINTF("%s(%d): ", le->entry.file, le->entry.line);
            }

            /* Shift until able to compute percentage in 32 bits */
            while ((IP_U64_GETHI(a) | IP_U64_GETHI(b)) != 0 ||
                   ((IP_U64_GETLO(a) | IP_U64_GETLO(b)) & 0xff000000) != 0)
            {
                /* 64-bit shift... */
                IP_U64_SETLO(a, (IP_U64_GETLO(a) >> 1) |
                             (IP_U64_GETHI(a) << 31));
                IP_U64_SETHI(a, IP_U64_GETHI(a) >> 1);
                IP_U64_SETLO(b, (IP_U64_GETLO(b) >> 1) |
                             (IP_U64_GETHI(b) << 31));
                IP_U64_SETHI(b, IP_U64_GETHI(b) >> 1);
            }

            PRINTF("%lu%% %lu(%lu-%lu) us, %lu times, %s\n",
                   (unsigned long)((IP_U64_GETLO(a) * 100 + 50) /
                                   IP_U64_GETLO(b)),
                   (unsigned long)(IP_U64_GETLO(le->us) / le->count),
                   (unsigned long)le->min, (unsigned long)le->max,
                   (unsigned long)le->count, le->entry.msg);

            ipcom_list_remove(&le->head);
            ipcom_free(le);
        }

        ipcom_list_remove(&seq->head);
        ipcom_free(seq);
    }

    (void)ipcom_prof_stopped(stopped);
}


/*
 *===========================================================================
 *                    ipcom_prof_void
 *===========================================================================
 * Description: Void command.
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_prof_void(void)
{
    IPCOM_PROF("Start print");
    PRINTF("void\n");
    IPCOM_PROF("Stop print");
}


/*
 *===========================================================================
 *	ipcom_cmd_prof
 *===========================================================================
 * Description: Profiler shell command.
 * Parameters:
 * Returns:
 */
IP_PUBLIC int
ipcom_cmd_prof(int argc, char *argv[])
{
    Ipcom_getopt o;
    int flags = 0;
    int begin = 0;
    int end = -1;
    int opt;

    ipcom_getopt_clear_r(&o);

    /* Get options */
    while ((opt = ipcom_getopt_r(argc, argv, "b:e:acloh", &o)) != -1)
    {
        switch (opt)
        {
        case 'b':
            begin = ipcom_atoi(o.optarg);
            break;
        case 'e':
            end = ipcom_atoi(o.optarg);
            break;
        case 'a':
            flags |= IPCOM_PROF_ABSOLUTE;
            break;
        case 'c':
            flags |= IPCOM_PROF_COUNT;
            break;
        case 'l':
            flags |= IPCOM_PROF_LINENUM;
            break;
        case 'o':
            flags |= IPCOM_PROF_OVERHEAD;
            break;
        case 'h':
            PRINTF("Usage: prof [options] command\n");
            PRINTF("Commands:\n");
            PRINTF(" clear  Clear profiler data\n");
            PRINTF(" start  Start profiling\n");
            PRINTF(" stop   Stop profiling\n");
            PRINTF(" info   Show profiling info\n");
            PRINTF(" dump   Print plain dump of profiler data\n");
            PRINTF(" sum    Print summary of profiler data\n");
            PRINTF(" seq    Print sequence info of profiler data\n");
            PRINTF(" void   Void command for shell profiling \n");
            PRINTF("Options:\n");
            PRINTF(" -a     Use absolute times where applicable\n");
            PRINTF(" -c     Sort output on count instead of time\n");
            PRINTF(" -l     Print linenumber for each entry\n");
            PRINTF(" -o     Include profiler overhead\n");
            PRINTF(" -h     Print this message\n");
            return 0;
        default:
            PRINTF("Invalid profiler option\n");
            return 1;
        }
    }

    /* Check command */
    if (argc <= o.optind)
    {
        PRINTF("Please enter a profiler command\n");
        return 1;
    }

    if (ipcom_strcmp(argv[o.optind], "clear") == 0)
    {
        ipcom_prof_clear();
    }
    else if (ipcom_strcmp(argv[o.optind], "start") == 0)
    {
        (void)ipcom_prof_stopped(0);
    }
    else if (ipcom_strcmp(argv[o.optind], "stop") == 0)
    {
        (void)ipcom_prof_stopped(1);
    }
    else if (ipcom_strcmp(argv[o.optind], "info") == 0)
    {
        PRINTF("%s, got %lu samples\n",
                     (ipcom_prof_data.stopped ? "Stopped" : "Running"),
                     ipcom_prof_data.indx);
        PRINTF("%lu us overhead/sample\n", ipcom_prof_data.overhead);
    }
    else if (ipcom_strcmp(argv[o.optind], "dump") == 0)
    {
        ipcom_prof_dump(begin, end, flags);
    }
    else if (ipcom_strcmp(argv[o.optind], "sum") == 0)
    {
        ipcom_prof_summary(begin, end, flags);
    }
    else if (ipcom_strcmp(argv[o.optind], "seq") == 0)
    {
        ipcom_prof_sequence(begin, end, flags);
    }
    else if (ipcom_strcmp(argv[o.optind], "void") == 0)
    {
        ipcom_prof_void();
    }
    else
    {
        PRINTF("Invalid profiler command\n");
        return 1;
    }

    return 0;
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
