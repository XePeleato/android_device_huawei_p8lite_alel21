/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_cmd_qc.c,v $ $Revision: 1.38 $
 *     $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_cmd_qc.c,v $
 *     $Author: kenneth $
 *     $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *      Design and implementation by Kenneth Jonsson <kenneth@interpeak.se>
 ******************************************************************************
 */

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 */
/* Configure network interface queue(s) */

/*
 ****************************************************************************
 * 2                    CONFIGURATION
 ****************************************************************************
 */

#ifdef IPCOM_KERNEL
#undef IPCOM_KERNEL
#endif
#include "ipnet_config.h"

/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#define IPCOM_USE_CLIB_PROTO
#include <ipcom_clib.h>
#include <ipcom_cmd.h>
#include <ipcom_getopt.h>
#include <ipcom_sock.h>

#include "ipnet.h"
#include "ipnet_cmd.h"
#include "ipnet_qos.h"


/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

struct Ipnet_cmd_qc_struct;

/*
 *===========================================================================
 *                      Ipnet_cmd_qc_op
 *===========================================================================
 * Description: Type specific operation.
 * Parameters:  p - Arguments to indetify the queue to show.
 * Returns:
 *
 */
typedef int (*Ipnet_cmd_qc_op)(struct Ipnet_cmd_qc_struct *p);


typedef struct Ipnet_cmd_qc_struct
{
    Ip_fd           fd;
    Ip_bool         silent;
    int             recurse_level;  /* Current depth in the filter hierarchically */
    int             argc;           /* Number of unparsed arguments */
    char **         argv;           /* Array of unparsed arguments */
    Ipnet_cmd_qc_op cmd_op;
    union
    {
        struct Ipnet_ifqueue        ifq;
        struct Ipnet_ifqueue_filter filter;
    } p;
    char  s[100];          /* String buffer to format output */
}
Ipnet_cmd_qc;


typedef struct Ipnet_cmd_qc_type_handler_struct
{
    const char      *type;     /* Queue type, i.e. "fifo", "htbc", ... */
    Ipnet_cmd_qc_op  add;      /* Pointer to a function that knows how to parse
                                  the queue specific arguments */
    Ipnet_cmd_qc_op  show;     /* Pointer to a function that knows how to show
                                  the queue specific parameters */
}
Ipnet_cmd_qc_type_handler;


typedef struct Ipnet_cmd_qc_op_handler_struct
{
    const char      *type;     /* Operation type, i.e. "filter", "queue", ... */
    const char      *op;       /* Operation, i.e. "add", "del", "show" */
    Ipnet_cmd_qc_op  f_op;     /* Pointer to a function that handles the operation */
}
Ipnet_cmd_qc_op_handler;

/*
 ****************************************************************************
 * 6                    EXTERN PROTOTYPES
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 7                    LOCAL PROTOTYPES
 ****************************************************************************
 */

#ifdef IPNET_USE_IFO_FIFO
IP_STATIC int ipnet_cmd_qc_parse_fifo(Ipnet_cmd_qc *p);
IP_STATIC int ipnet_cmd_qc_show_fifo(Ipnet_cmd_qc *p);
#endif

#ifdef IPNET_USE_IFO_DPAF
IP_STATIC int ipnet_cmd_qc_parse_dpaf(Ipnet_cmd_qc *p);
IP_STATIC int ipnet_cmd_qc_show_dpaf(Ipnet_cmd_qc *p);
#endif

#ifdef IPNET_USE_IFO_NETEMU
IP_STATIC int ipnet_cmd_qc_parse_netemu(Ipnet_cmd_qc *p);
IP_STATIC int ipnet_cmd_qc_show_netemu(Ipnet_cmd_qc *p);
#endif

#ifdef IPNET_USE_IFO_MBC
IP_STATIC int ipnet_cmd_qc_parse_mbc(Ipnet_cmd_qc *p);
IP_STATIC int ipnet_cmd_qc_show_mbc(Ipnet_cmd_qc *p);
#endif

#ifdef IPNET_USE_IFO_HTBC
IP_STATIC int ipnet_cmd_qc_parse_htbc(Ipnet_cmd_qc *p);
IP_STATIC int ipnet_cmd_qc_show_htbc(Ipnet_cmd_qc *p);
#endif

IP_STATIC int ipnet_cmd_qc_nop(Ipnet_cmd_qc *p);

IP_STATIC int ipnet_cmd_qc_add_queue(Ipnet_cmd_qc *p);
IP_STATIC int ipnet_cmd_qc_del_queue(Ipnet_cmd_qc *p);
IP_STATIC int ipnet_cmd_qc_show_queue(Ipnet_cmd_qc *p);
IP_STATIC int ipnet_cmd_qc_add_filter(Ipnet_cmd_qc *p);
IP_STATIC int ipnet_cmd_qc_del_filter(Ipnet_cmd_qc *p);
IP_STATIC int ipnet_cmd_qc_show_filter(Ipnet_cmd_qc *p);


/*
 ****************************************************************************
 * 8                    DATA
 ****************************************************************************
 */

IP_STATIC Ipnet_cmd_qc_type_handler ipnet_qc_type_handlers[] =
{
    { "none",   ipnet_cmd_qc_nop,          ipnet_cmd_qc_nop         },
#ifdef IPNET_USE_IFO_FIFO
    { "fifo",   ipnet_cmd_qc_parse_fifo,   ipnet_cmd_qc_show_fifo   },
#endif
#ifdef IPNET_USE_IFO_DPAF
    { "dpaf",   ipnet_cmd_qc_parse_dpaf,   ipnet_cmd_qc_show_dpaf   },
#endif
#ifdef IPNET_USE_IFO_NETEMU
    { "netemu", ipnet_cmd_qc_parse_netemu, ipnet_cmd_qc_show_netemu },
#endif
#ifdef IPNET_USE_IFO_NULL
    { "null",   ipnet_cmd_qc_nop,          ipnet_cmd_qc_nop         },
#endif
#ifdef IPNET_USE_IFO_MBC
    { "mbc",    ipnet_cmd_qc_parse_mbc,    ipnet_cmd_qc_show_mbc    },
#endif
#ifdef IPNET_USE_IFO_HTBC
    { "htbc",   ipnet_cmd_qc_parse_htbc,   ipnet_cmd_qc_show_htbc   },
#endif
};

IP_STATIC Ipnet_cmd_qc_op_handler ipnet_cmd_qc_op_handlers[] =
{
    { "queue",  "add",  ipnet_cmd_qc_add_queue   },
    { "queue",  "del",  ipnet_cmd_qc_del_queue   },
    { "queue",  "show", ipnet_cmd_qc_show_queue  },
    { "filter", "add",  ipnet_cmd_qc_add_filter  },
    { "filter", "del",  ipnet_cmd_qc_del_filter  },
    { "filter", "show", ipnet_cmd_qc_show_filter },
};


/*
 ****************************************************************************
 * 9                    STATIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                     ipnet_cmd_qc_next_arg
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipnet_cmd_qc_next_arg(Ipnet_cmd_qc *p)
{
    ip_assert(p->argc > 0);
    p->argv++;
    p->argc--;
}


#ifdef IPNET_USE_IFO_NETEMU
/*
 *===========================================================================
 *                     ipnet_cmd_qc_prev_arg
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipnet_cmd_qc_prev_arg(Ipnet_cmd_qc *p)
{
    p->argv--;
    p->argc++;
}
#endif


/*
 *===========================================================================
 *                     ipnet_cmd_qc_output_indent
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipnet_cmd_qc_output_indent(Ipnet_cmd_qc *p)
{
    int i;

    for (i = 0; i < p->recurse_level; i++)
        ipcom_printf("\t");
}


/*
 *===========================================================================
 *                      ipnet_cmd_qc_show_children
 *===========================================================================
 * Description: Shows all children to a container queue.
 * Parameters:  p - The qc command structure to get the container.
 *              container - Pointer to the container part of the queue desc.
 * Returns:     0 = success
 *             <0 = error
 *
 */
#if defined(IPNET_USE_IFO_MBC) || defined(IPNET_USE_IFO_HTBC)
IP_STATIC int
ipnet_cmd_qc_show_children(Ipnet_cmd_qc *p, struct Ipnet_ifqueue_container *container)
{
    int i;
    int ret = 0;
    struct Ipnet_ifqueue_container c;

    ipcom_memcpy(&c, container, sizeof(c));
    p->recurse_level++;

    p->p.ifq.ifq_parent_id = p->p.ifq.ifq_id;
    for (i = 0; ret == 0 && i < c.child_count; i++)
    {
        p->p.ifq.ifq_id = c.child_ids[i];
        ret = ipnet_cmd_qc_show_queue(p);
    }

    p->recurse_level--;
    return ret;
}
#endif /* defined(IPNET_USE_IFO_MBC) || defined(IPNET_USE_IFO_HTBC) */


/*
 *===========================================================================
 *                     ipnet_cmd_qc_get_type_handler
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ipnet_cmd_qc_type_handler *
ipnet_cmd_qc_get_type_handler(const char *type)
{
    int i;
    int count = sizeof(ipnet_qc_type_handlers) / sizeof(ipnet_qc_type_handlers[0]);

    for (i = 0; i < count; i++)
        if (ipcom_strcmp(ipnet_qc_type_handlers[i].type, type) == 0)
            return &ipnet_qc_type_handlers[i];

    ipcom_printf("'%s' is an unknown queue type, known types are: ", type);
    for (i = 0; i < count; i++)
        ipcom_printf("%s, ", ipnet_qc_type_handlers[i].type);
    ipcom_printf(IP_LF);

    return IP_NULL;
}


/*
 *===========================================================================
 *                     ipnet_cmd_qc_get_op_func
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ipnet_cmd_qc_op
ipnet_cmd_qc_get_op_func(const char *type, const char *op)
{
    int i;
    int count = sizeof(ipnet_cmd_qc_op_handlers) / sizeof(ipnet_cmd_qc_op_handlers[0]);

    for (i = 0; i < count; i++)
        if (ipcom_strcmp(ipnet_cmd_qc_op_handlers[i].type, type) == 0
            && ipcom_strcmp(ipnet_cmd_qc_op_handlers[i].op, op) == 0)
            return ipnet_cmd_qc_op_handlers[i].f_op;

    ipcom_printf("'%s %s' is an unknown operation, knowed operations are:"IP_LF, type, op);
    for (i = 0; i < count; i++)
        ipcom_printf("  '%s %s', " IP_LF,
                     ipnet_cmd_qc_op_handlers[i].type,
                     ipnet_cmd_qc_op_handlers[i].op);

    return IP_NULL;
}


/*
 *===========================================================================
 *                    ipnet_cmd_qc_parse_number
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_bool
ipnet_cmd_qc_parse_number(const char *str_num, Ip_u32 *num)
{
    char *end;
    long  lnum;

    lnum = ipcom_strtol(str_num, &end, 10);
    if (lnum < 0)
        *num = 0;
    else
        *num = (Ip_u32) lnum;

    if (end && ipcom_strlen(end) > 0)
    {
        if (ipcom_strcmp(end, "k") == 0 || ipcom_strcmp(end, "kb") == 0)
            *num *= 1000;
        else if (ipcom_strcmp(end, "M") == 0 || ipcom_strcmp(end, "Mb") == 0)
            *num *= 1000000;
        else
        {
            ipcom_printf("'%s' is an unsupported number suffix"IP_LF, str_num);
            return IP_FALSE;
        }
    }
    return IP_TRUE;
}


/*
 *===========================================================================
 *                    ipnet_cmd_qc_format_number
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
#if defined(IPNET_USE_IFO_FIFO) || defined(IPNET_USE_IFO_DPAF) \
    || defined(IPNET_USE_IFO_NETEMU) || defined(ipnet_cmd_qc_format_number)
IP_STATIC const char *
ipnet_cmd_qc_format_number(char *str_num, int str_len, Ip_u32 num)
{
    if (num / 10000000 > 0)
        ipcom_snprintf(str_num, str_len, "%uM", (unsigned) (num / 1000000));
    else if (num / 10000 > 0)
        ipcom_snprintf(str_num, str_len, "%uk", (unsigned) (num / 1000));
    else
        ipcom_snprintf(str_num, str_len, "%u", (unsigned) num);
    return str_num;
}
#endif


/*
 *===========================================================================
 *                    ipnet_cmd_qc_parse_addr
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipnet_cmd_qc_parse_addr(const char *str, Ip_u8 *domain,
                        union Ip_in_addr_union *addr, int *prefixlen)
{
    int domains[] = {
#ifdef IPCOM_USE_INET
        IP_AF_INET,
#endif
#ifdef IPCOM_USE_INET6
        IP_AF_INET6
#endif
    };
    Ip_size_t i;

    if (*domain)
    {
        if (ipcom_getinaddrprefixbyaddr(*domain, str, addr, prefixlen) == 0
            || ipcom_inet_pton(*domain, str, addr) == 1)
            return 0;

        ipcom_printf("%s is not a valid address for domain %d"IP_LF, str, *domain);
        return -1;
    }

    for (i = 0; i < sizeof(domains) / sizeof(domains[0]); i++)
    {
        if (ipcom_getinaddrprefixbyaddr(domains[i], str, addr, prefixlen) == 0
            || ipcom_inet_pton(domains[i], str, addr) == 1)
        {
            *domain = (Ip_u8) domains[i];
            return 0;
        }
    }

    ipcom_printf("%s is not a valid address for domain %d"IP_LF, str, *domain);
    return -1;
}


/*
 *===========================================================================
 *                    ipnet_cmd_qc_parse_port_range
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipnet_cmd_qc_parse_port_range(const char *str, Ip_u16 *lo_port, Ip_u16 *hi_port)
{
    char *end;

    if (*str != '-')
        *lo_port = (Ip_u16) ipcom_strtol(str, &end, 10);
    else
    {
        *lo_port = 0;
        end = (char *) str + 1;
    }

    if (end == IP_NULL || ipcom_strlen(end) == 0)
        *hi_port = *lo_port;
    else
    {
        if (*end == '-')
            end++;
        if (ipcom_strlen(end) == 0)
            *hi_port = 0xffff;
        else
            *hi_port = (Ip_u16) ipcom_strtol(end, &end, 10);
    }
    if (*hi_port < *lo_port)
        return -IP_ERRNO_EINVAL;

    return 0;
}


/*
 *===========================================================================
 *                    ipnet_cmd_qc_nop
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipnet_cmd_qc_nop(Ipnet_cmd_qc *p)
{
    (void) p;
    return 0;
}


#ifdef IPNET_USE_IFO_FIFO
/*
 *===========================================================================
 *                    ipnet_cmd_qc_parse_fifo
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipnet_cmd_qc_help_fifo(void)
{
    ipcom_printf("Usage: ... fifo limit <positive_number>"IP_LF);
    return -IP_ERRNO_EINVAL;
}

/*
 *===========================================================================
 *                    ipnet_cmd_qc_parse_fifo
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipnet_cmd_qc_parse_fifo(Ipnet_cmd_qc *p)
{
    struct Ipnet_ifqueue_fifo *f = &p->p.ifq.ifq_data.fifo;

    if (p->argc != 2 || ipcom_strcmp(*p->argv, "limit") != 0)
        return ipnet_cmd_qc_help_fifo();

    ipnet_cmd_qc_next_arg(p);
    if (!ipnet_cmd_qc_parse_number(*p->argv, &f->fifo_limit))
        return -IP_ERRNO_EINVAL;
    if (f->fifo_limit == 0)
    {
        ipcom_printf("The queue limit must be >0"IP_LF);
        return -IP_ERRNO_EINVAL;
    }
    ipnet_cmd_qc_next_arg(p);
    return 0;
}


/*
 *===========================================================================
 *                      ipnet_cmd_qc_show_fifo
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipnet_cmd_qc_show_fifo(Ipnet_cmd_qc *p)
{
    ipcom_printf("  limit: %s"IP_LF,
                 ipnet_cmd_qc_format_number(p->s, sizeof(p->s), p->p.ifq.ifq_data.fifo.fifo_limit));
    return 0;
}
#endif /* IPNET_USE_IFO_FIFO */


#ifdef IPNET_USE_IFO_DPAF
/*
 *===========================================================================
 *                    ipnet_cmd_qc_parse_dpaf
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipnet_cmd_qc_help_dpaf(void)
{
    ipcom_printf("Usage: ... dpaf limit <number>"IP_LF);
    return -IP_ERRNO_EINVAL;
}

/*
 *===========================================================================
 *                    ipnet_cmd_qc_parse_dpaf
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipnet_cmd_qc_parse_dpaf(Ipnet_cmd_qc *p)
{
    struct Ipnet_ifqueue_dpaf *d = &p->p.ifq.ifq_data.dpaf;

    if (p->argc != 2 || ipcom_strcmp(*p->argv, "limit") != 0)
        return ipnet_cmd_qc_help_dpaf();

    ipnet_cmd_qc_next_arg(p);
    if (!ipnet_cmd_qc_parse_number(*p->argv, &d->dpaf_limit))
        return -IP_ERRNO_EINVAL;
    if (d->dpaf_limit == 0)
    {
        ipcom_printf("The queue limit must be >0"IP_LF);
        return -IP_ERRNO_EINVAL;
    }
    ipnet_cmd_qc_next_arg(p);
    return 0;
}


/*
 *===========================================================================
 *                      ipnet_cmd_qc_show_dpaf
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipnet_cmd_qc_show_dpaf(Ipnet_cmd_qc *p)
{
    ipcom_printf("  limit: %s"IP_LF,
                 ipnet_cmd_qc_format_number(p->s, sizeof(p->s), p->p.ifq.ifq_data.dpaf.dpaf_limit));
    return 0;
}
#endif /* IPNET_USE_IFO_DPAF */


#ifdef IPNET_USE_IFO_NETEMU
/*
 *===========================================================================
 *                    ipnet_cmd_qc_parse_bit_string
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_bool
ipnet_cmd_qc_parse_bit_string(const char *bit_str, Ip_u32 *pattern, Ip_u8 *pattern_len)
{
    char *ctx;
    char *bit;
    char *dup_bit_str = ipcom_strdup(bit_str);

    if (dup_bit_str == IP_NULL)
    {
        ipcom_printf("Failed to duplicate string '%s'"IP_LF, bit_str);
        return IP_FALSE;
    }

    *pattern = 0;
    *pattern_len = 0;
    bit = ipcom_strtok_r(dup_bit_str, ",", &ctx);
    if (bit == IP_NULL)
    {
        ipcom_free(dup_bit_str);
        ipcom_printf("Invalid format of the pattern, must be a comma separated list of 0 and 1"IP_LF);
        return IP_FALSE;
    }

    while (bit != IP_NULL)
    {
        if (ipcom_atoi(bit))
            IP_BIT_SET(*pattern, 1 << *pattern_len);
        ++*pattern_len;
        bit = ipcom_strtok_r(IP_NULL, ",", &ctx);
    }

    ipcom_free(dup_bit_str);
    return IP_TRUE;
}


/*
 *===========================================================================
 *                    ipnet_cmd_qc_parse_netemu
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipnet_cmd_qc_help_netemu(void)
{
    ipcom_printf("Usage: ... netemu limit <number> [min_latency <msec>] [max_latency <msec>] ");
    ipcom_printf("[drop <number> [random] [pattern X,X,..,X]] [corrupt <number> [random]]"IP_LF);
    return -IP_ERRNO_EINVAL;
}


/*
 *===========================================================================
 *                    ipnet_cmd_qc_parse_netemu
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipnet_cmd_qc_parse_netemu(Ipnet_cmd_qc *p)
{
    struct Ipnet_ifqueue_netemu *n = &p->p.ifq.ifq_data.netemu;

    if (p->argc == 0)
        return ipnet_cmd_qc_help_netemu();

    while (p->argc)
    {
        if (ipcom_strcmp(*p->argv, "limit") == 0)
        {
            ipnet_cmd_qc_next_arg(p);
            if (!ipnet_cmd_qc_parse_number(*p->argv, &n->netemu_limit))
                return -1;
        }
        else if (ipcom_strcmp(*p->argv, "min_latency") == 0)
        {
            ipnet_cmd_qc_next_arg(p);
            if (!ipnet_cmd_qc_parse_number(*p->argv, &n->netemu_min_latency))
                return -1;
        }
        else if (ipcom_strcmp(*p->argv, "max_latency") == 0)
        {
            ipnet_cmd_qc_next_arg(p);
            if (!ipnet_cmd_qc_parse_number(*p->argv, &n->netemu_max_latency))
                return -1;
        }
        else if (ipcom_strcmp(*p->argv, "drop") == 0)
        {
            ipnet_cmd_qc_next_arg(p);
            if (!ipnet_cmd_qc_parse_number(*p->argv, &n->netemu_drop_probability))
                return -1;
            for (;;)
            {
                ipnet_cmd_qc_next_arg(p);
                if (*p->argv == IP_NULL)
                {
                    ipnet_cmd_qc_prev_arg(p);
                    break;
                }

                if (ipcom_strcmp(*p->argv, "random") == 0)
                    n->netemu_random_drop = IP_TRUE;
                else if (ipcom_strcmp(*p->argv, "pattern") == 0)
                {
                    ipnet_cmd_qc_next_arg(p);
                    if (!ipnet_cmd_qc_parse_bit_string(*p->argv,
                                                       &n->netemu_drop_pattern,
                                                       &n->netemu_drop_pattern_len))
                        return -1;
                }
                else
                {
                    ipnet_cmd_qc_prev_arg(p);
                    break;
                }
            }
        }
        else if (ipcom_strcmp(*p->argv, "corrupt") == 0)
        {
            ipnet_cmd_qc_next_arg(p);
            if (!ipnet_cmd_qc_parse_number(*p->argv, &n->netemu_corrupt_probability))
                return -1;
            ipnet_cmd_qc_next_arg(p);
            if (*p->argv != IP_NULL && ipcom_strcmp(*p->argv, "random") == 0)
                n->netemu_random_corrupt = IP_TRUE;
            else
                ipnet_cmd_qc_prev_arg(p);
        }
        else
        {
            ipcom_printf("'%s' is not a NETEMU parameter"IP_LF, *p->argv);
            return ipnet_cmd_qc_help_netemu();
        }
        ipnet_cmd_qc_next_arg(p);
    }

    if (n->netemu_limit <= 0)
    {
        ipcom_printf("The limit must be specified and >0" IP_LF);
        return ipnet_cmd_qc_help_netemu();
    }

    if (n->netemu_min_latency > n->netemu_max_latency)
    {
        ipcom_printf("The minum latency must be equal to or smaller than the max latency" IP_LF);
        return ipnet_cmd_qc_help_netemu();
    }

    return 0;
}


/*
 *===========================================================================
 *                      ipnet_cmd_qc_show_netemu
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipnet_cmd_qc_show_netemu(Ipnet_cmd_qc *p)
{
    struct Ipnet_ifqueue_netemu *n = &p->p.ifq.ifq_data.netemu;

    ipcom_printf("  limit: %s",
                 ipnet_cmd_qc_format_number(p->s, sizeof(p->s), n->netemu_limit));

    if (n->netemu_min_latency)
        ipcom_printf(", latency: %ld..%ld ms",
                     n->netemu_min_latency, n->netemu_max_latency);
    if (n->netemu_drop_probability)
        ipcom_printf(", drop 1 out of %ld%s",
                     n->netemu_drop_probability,
                     n->netemu_random_drop ? " (random)" : "");
    if (n->netemu_drop_probability)
        ipcom_printf(", corrupt 1 out of %ld%s",
                     n->netemu_corrupt_probability,
                     n->netemu_random_corrupt ? " (random)" : "");
    ipcom_printf(IP_LF);
    return 0;
}
#endif /* IPNET_USE_IFO_NETEMU */


#ifdef IPNET_USE_IFO_MBC
/*
 *===========================================================================
 *                    ipnet_cmd_qc_parse_mbc
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipnet_cmd_qc_help_mbc(void)
{
    ipcom_printf("Usage: ... mbc bands <number> [default_band <number>]"IP_LF);
    return -IP_ERRNO_EINVAL;
}


/*
 *===========================================================================
 *                    ipnet_cmd_qc_parse_mbc
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipnet_cmd_qc_parse_mbc(Ipnet_cmd_qc *p)
{
    struct Ipnet_ifqueue_mbc *m = &p->p.ifq.ifq_data.mbc;

    if (p->argc == 0)
        return ipnet_cmd_qc_help_mbc();

    m->mbc_bands        = 0;
    m->mbc_default_band = 1;

    while (p->argc)
    {
        if (ipcom_strcmp(*p->argv, "bands") == 0)
        {
            ipnet_cmd_qc_next_arg(p);
            if (!ipnet_cmd_qc_parse_number(*p->argv, &m->mbc_bands))
                return -1;
        }
        else if (ipcom_strcmp(*p->argv, "default_band") == 0)
        {
            ipnet_cmd_qc_next_arg(p);
            if (!ipnet_cmd_qc_parse_number(*p->argv, &m->mbc_default_band))
                return -1;
        }
        else
        {
            ipcom_printf("'%s' is not a MBC parameter"IP_LF, *p->argv);
            return ipnet_cmd_qc_help_mbc();
        }
        ipnet_cmd_qc_next_arg(p);
    }

    if (m->mbc_bands < 2 || m->mbc_bands > IPNET_IFQ_CONTAINER_MAX_COUNT)
    {
        ipcom_printf("The number of bands for MBC must be between 2 and %d"IP_LF,
                     IPNET_IFQ_CONTAINER_MAX_COUNT);
        return ipnet_cmd_qc_help_mbc();
    }

    return 0;
}


/*
 *===========================================================================
 *                      ipnet_cmd_qc_show_mbc
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipnet_cmd_qc_show_mbc(Ipnet_cmd_qc *p)
{
    struct Ipnet_ifqueue_mbc *m = &p->p.ifq.ifq_data.mbc;

    ipcom_printf("  bands: %ld, default_band: %ld"IP_LF,
                 m->mbc_bands,
                 m->mbc_default_band);
    return ipnet_cmd_qc_show_children(p, &m->mbc_container);
}
#endif /* IPNET_USE_IFO_MBC */


#ifdef IPNET_USE_IFO_HTBC
/*
 *===========================================================================
 *                    ipnet_cmd_qc_format_rate
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC const char *
ipnet_cmd_qc_format_rate(char *str_num, int str_len, Ip_u32 num)
{
    if (num / 10000000 > 0)
        ipcom_snprintf(str_num, str_len, "%uMbps", (unsigned) (num / 1000000));
    else if (num / 10000 > 0)
        ipcom_snprintf(str_num, str_len, "%ukbps", (unsigned) (num / 1000));
    else
        ipcom_snprintf(str_num, str_len, "%ubps", (unsigned) (num));
    return str_num;
}


/*
 *===========================================================================
 *                    ipnet_cmd_qc_parse_rate
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_bool
ipnet_cmd_qc_parse_rate(const char *str_rate, Ip_u32 *rate)
{
    char   *end;

    *rate = (Ip_u32) ipcom_strtol(str_rate, &end, 10);
    if (end && ipcom_strlen(end) > 0)
    {
        if (ipcom_strcmp(end, "kbps") == 0)
            *rate *= 1000;
        else if (ipcom_strcmp(end, "Mbps") == 0)
            *rate *= 1000000;
        else if (ipcom_strcmp(end, "kbit") == 0)
            *rate *= 1000 / 8;
        else if (ipcom_strcmp(end, "Mbit") == 0)
            *rate *= 1000000 / 8;
        else if (ipcom_strcmp(end, "bit") == 0)
            *rate /= 8;
        else if (ipcom_strcmp(end, "bps") == 0)
            ;
        else
        {
            ipcom_printf("'%s' is an unsupported rate suffix"IP_LF, end);
            return IP_FALSE;
        }
    }
    return IP_TRUE;
}


/*
 *===========================================================================
 *                    ipnet_cmd_qc_parse_HTBC
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipnet_cmd_qc_help_htbc(void)
{
    ipcom_printf("Usage: ... htbc rate <rate> [burst <number>]"IP_LF);
    return -IP_ERRNO_EINVAL;
}


/*
 *===========================================================================
 *                    ipnet_cmd_qc_parse_htbc
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipnet_cmd_qc_parse_htbc(Ipnet_cmd_qc *p)
{
    struct Ipnet_ifqueue_htbc *h = &p->p.ifq.ifq_data.htbc;

    if (p->argc == 0)
        return ipnet_cmd_qc_help_htbc();

    h->htbc_byterate    = 0;
    h->htbc_token_limit = 0;

    while (p->argc)
    {
        if (ipcom_strcmp(*p->argv, "rate") == 0)
        {
            ipnet_cmd_qc_next_arg(p);
            if (!ipnet_cmd_qc_parse_rate(*p->argv, &h->htbc_byterate))
                return -1;
        }
        else if (ipcom_strcmp(*p->argv, "burst") == 0)
        {
            ipnet_cmd_qc_next_arg(p);
            if (!ipnet_cmd_qc_parse_number(*p->argv, &h->htbc_token_limit))
                return -1;
        }
        else
        {
            ipcom_printf("'%s' is not a HTBC parameter"IP_LF, *p->argv);
            return ipnet_cmd_qc_help_htbc();
        }
        ipnet_cmd_qc_next_arg(p);
    }

    if (h->htbc_byterate == 0)
    {
        ipcom_printf("The data rate for HTBC must be >0"IP_LF);
        return ipnet_cmd_qc_help_htbc();
    }

    return 0;
}


/*
 *===========================================================================
 *                      ipnet_cmd_qc_show_htbc
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipnet_cmd_qc_show_htbc(Ipnet_cmd_qc *p)
{
    struct Ipnet_ifqueue_htbc *h = &p->p.ifq.ifq_data.htbc;

    ipcom_printf("  rate: %s, ",
                 ipnet_cmd_qc_format_rate(p->s, sizeof(p->s), h->htbc_byterate));
    ipcom_printf("burst: %s, default id: %d"IP_LF,
                 ipnet_cmd_qc_format_number(p->s, sizeof(p->s), h->htbc_token_limit),
                 h->htbc_default_id);
    return ipnet_cmd_qc_show_children(p, &h->htbc_container);
}
#endif /* IPNET_USE_IFO_HTBC */


/*
 *===========================================================================
 *                      ipnet_cmd_qc_parse_queue_type
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipnet_cmd_qc_parse_queue_type(Ipnet_cmd_qc *p)
{
    Ipnet_cmd_qc_type_handler *h;

    if (p->argc < 1)
    {
        ipcom_printf("no queue type specified" IP_LF);
        return -IP_ERRNO_EINVAL;
    }

    ipcom_strcpy(p->p.ifq.ifq_type, *p->argv);
    ipnet_cmd_qc_next_arg(p);
    h = ipnet_cmd_qc_get_type_handler(p->p.ifq.ifq_type);
    if (h)
        return h->add(p);

    return -IP_ERRNO_EINVAL;
}


/*
 *===========================================================================
 *                      ipnet_cmd_qc_parse_dev
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipnet_cmd_qc_parse_dev(Ipnet_cmd_qc *p)
{
    if (p->argc < 2 || ipcom_strcmp(*p->argv, "dev") != 0)
    {
        ipcom_printf("expected 'dev <if>'" IP_LF);
        return -IP_ERRNO_EINVAL;
    }
    ipnet_cmd_qc_next_arg(p);
    if (ipcom_if_nametoindex(*p->argv) == 0)
    {
        ipcom_printf("'%s' is not a valid interface name" IP_LF, *p->argv);
        return -IP_ERRNO_EINVAL;

    }
    ipcom_strcpy(p->p.ifq.ifq_name, *p->argv);
    ipnet_cmd_qc_next_arg(p);
    return 0;
}


/*
 *===========================================================================
 *                      ipnet_cmd_qc_parse_parent
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipnet_cmd_qc_parse_parent(Ipnet_cmd_qc *p)
{
    p->p.ifq.ifq_parent_id = IP_IFQ_ID_NONE;

    if (p->argc < 1)
        return 0;

    if (ipcom_strcmp(*p->argv, "root") == 0)
    {
        ipnet_cmd_qc_next_arg(p);
        return 0;
    }

    if (ipcom_strcmp(*p->argv, "parent") == 0)
    {
        ipnet_cmd_qc_next_arg(p);
        if (p->argc < 1)
        {
            ipcom_printf("Expected [ root | parent <q-id> ]"IP_LF);
            return -1;
        }
        p->p.ifq.ifq_parent_id = ipcom_atoi(*p->argv);
        ipnet_cmd_qc_next_arg(p);
    }
    return 0;
}


/*
 *===========================================================================
 *                      ipnet_cmd_qc_parse_id
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipnet_cmd_qc_parse_id(Ipnet_cmd_qc *p)
{
    p->p.ifq.ifq_id = (p->cmd_op == ipnet_cmd_qc_show_queue
                       ? IP_IFQ_ID_NONE
                       : IP_IFQ_ID_DEFAULT);

    if (p->argc < 2 || ipcom_strcmp(*p->argv, "handle") != 0)
        return 0;

    ipnet_cmd_qc_next_arg(p);
    p->p.ifq.ifq_id = ipcom_atoi(*p->argv);
    ipnet_cmd_qc_next_arg(p);
    return 0;
}


/*
 *===========================================================================
 *                      ipnet_cmd_qc_show_queue
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipnet_cmd_qc_show_queue(Ipnet_cmd_qc *p)
{
    Ipnet_cmd_qc_type_handler *h;

    if (ipcom_socketioctl(p->fd, IPNET_SIOCGIFQUEUE, &p->p.ifq) < 0)
    {
        ipcom_printf("Failed to get the queue: %s"IP_LF, ipcom_strerror(ipcom_errno));
        return -1;
    }

    h = ipnet_cmd_qc_get_type_handler(p->p.ifq.ifq_type);
    if (h == IP_NULL)
    {
        ipcom_printf("No 'show' handler implemented for queue type '%s'"IP_LF,
                     p->p.ifq.ifq_type);
        return -1;
    }

    ipnet_cmd_qc_output_indent(p);
    ipcom_printf("%s/%d[%d] queue at %s"IP_LF,
                 p->p.ifq.ifq_type,
                 p->p.ifq.ifq_id,
                 p->p.ifq.ifq_parent_id,
                 p->p.ifq.ifq_name);
    ipnet_cmd_qc_output_indent(p);

    return h->show(p);
}


/*
 *===========================================================================
 *                      ipnet_cmd_qc_add_queue
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipnet_cmd_qc_add_queue(Ipnet_cmd_qc *p)
{
    if (ipnet_cmd_qc_parse_queue_type(p) < 0)
        return -1;

    if (ipcom_socketioctl(p->fd, IPNET_SIOCSIFQUEUE, &p->p.ifq) < 0)
    {
        ipcom_printf("Failed to add the queue: %s"IP_LF, ipcom_strerror(ipcom_errno));
        return -1;
    }
    return 0;
}


/*
 *===========================================================================
 *                      ipnet_cmd_qc_del_queue
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipnet_cmd_qc_del_queue(Ipnet_cmd_qc *p)
{
    ipcom_strcpy(p->p.ifq.ifq_type, "none");
    if (ipcom_socketioctl(p->fd, IPNET_SIOCSIFQUEUE, &p->p.ifq) < 0)
    {
        ipcom_printf("Failed to delete the queue: %s"IP_LF, ipcom_strerror(ipcom_errno));
        return -1;
    }
    return 0;
}


/*
 *===========================================================================
 *                      ipnet_cmd_qc_parse_op
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipnet_cmd_qc_parse_op(Ipnet_cmd_qc *p)
{
    if (p->argc < 2)
    {
        ipcom_printf("To few arguments, expected [ queue | filter ] [ show | add | del ]"IP_LF);
        return 0;
    }

    p->cmd_op = ipnet_cmd_qc_get_op_func(p->argv[0], p->argv[1]);
    ipnet_cmd_qc_next_arg(p);
    ipnet_cmd_qc_next_arg(p);
    return p->cmd_op ? 0 : -1;
}


/*
 *===========================================================================
 *                      ipnet_cmd_qc_parse_filter
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipnet_cmd_qc_parse_filter(Ipnet_cmd_qc *p)
{
    int    ret = 0;
    Ip_u32 n;
    struct Ipnet_classifier_rule *fr = &p->p.filter.filter_rule;
    int    prefixlen;

    if (p->argc < 4)
    {
        ipcom_printf("No filter rules specified"IP_LF);
        return -1;
    }

    while (ret == 0 && p->argc >= 2)
    {
        const char *type = p->argv[0];
        const char *val = p->argv[1];

        if (ipcom_strcmp(type, "flowid") == 0)
            break;
        ipnet_cmd_qc_next_arg(p);
        ipnet_cmd_qc_next_arg(p);

        if (ipcom_strcmp("srcaddr", type) == 0)
        {
            IP_BIT_SET(fr->mask, IPNET_CLS_RULE_SADDR);
            ret = ipnet_cmd_qc_parse_addr(val,
                                          &fr->af,
                                          &fr->saddr,
                                          &prefixlen);
            fr->saddr_prefixlen = (Ip_u8) prefixlen;
            continue;
        }
        if (ipcom_strcmp("dstaddr", type) == 0)
        {
            IP_BIT_SET(fr->mask, IPNET_CLS_RULE_DADDR);
            ret = ipnet_cmd_qc_parse_addr(val,
                                          &fr->af,
                                          &fr->daddr,
                                          &prefixlen);
            fr->daddr_prefixlen = (Ip_u8) prefixlen;
            continue;
        }

        if (ipcom_strcmp("srcport", type) == 0)
        {
            IP_BIT_SET(fr->mask, IPNET_CLS_RULE_SPORT);
            ret = ipnet_cmd_qc_parse_port_range(val,
                                                &fr->sport_low,
                                                &fr->sport_high);
            continue;
        }
        if (ipcom_strcmp("dstport", type) == 0)
        {
            IP_BIT_SET(fr->mask, IPNET_CLS_RULE_DPORT);
            ret = ipnet_cmd_qc_parse_port_range(val,
                                                &fr->dport_low,
                                                &fr->dport_high);
            continue;
        }

        if (!ipnet_cmd_qc_parse_number(val, &n))
            return -1;

        if (ipcom_strcmp("proto", type) == 0)
        {
            IP_BIT_SET(fr->mask, IPNET_CLS_RULE_PROTO);
            fr->proto = (Ip_u8) n;
        }
        else if (ipcom_strcmp("tclass", type) == 0
                 || ipcom_strcmp("tos", type) == 0
                 || ipcom_strcmp("ds", type) == 0)
        {
            IP_BIT_SET(fr->mask, IPNET_CLS_RULE_DS);
            fr->ds = (Ip_u8) n;
        }
        else
            break;
    }

    if (ret)
        return ret;

    if (p->argc == 0
        || ipcom_strcmp(*p->argv, "flowid") != 0)
    {
        ipcom_printf("No 'flowid' specified"IP_LF);
        return -1;
    }

    ipnet_cmd_qc_next_arg(p);
    if (!ipnet_cmd_qc_parse_number(*p->argv, &n))
        return -1;
    p->p.filter.filter_child_queue_id = (int) n;
    ipnet_cmd_qc_next_arg(p);

    return 0;
}


/*
 *===========================================================================
 *                      ipnet_cmd_qc_set_filter_id
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipnet_cmd_qc_set_filter_id(Ipnet_cmd_qc *p)
{
    int id = p->p.ifq.ifq_id;
    int queue_id = p->p.ifq.ifq_parent_id;

    p->p.filter.filter_id       = id;
    p->p.filter.filter_queue_id = queue_id;
    ipcom_memset(&p->p.filter.filter_rule, 0, sizeof(p->p.filter.filter_rule));
}


/*
 *===========================================================================
 *                      ipnet_cmd_qc_add_filter
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipnet_cmd_qc_add_filter(Ipnet_cmd_qc *p)
{
    ipnet_cmd_qc_set_filter_id(p);
    if (ipnet_cmd_qc_parse_filter(p) < 0)
        return -1;

    if (ipcom_socketioctl(p->fd, IPNET_SIOCXAIFQFILTER, &p->p.filter) < 0)
    {
        ipcom_printf("Failed to add the filter: %s"IP_LF, ipcom_strerror(ipcom_errno));
        return -1;
    }

    return 0;
}


/*
 *===========================================================================
 *                      ipnet_cmd_qc_del_filter
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipnet_cmd_qc_del_filter(Ipnet_cmd_qc *p)
{
    ipnet_cmd_qc_set_filter_id(p);
    if (ipcom_socketioctl(p->fd, IPNET_SIOCXDIFQFILTER, &p->p.filter) < 0)
    {
        ipcom_printf("Failed to delete the filter: %s"IP_LF, ipcom_strerror(ipcom_errno));
        return -1;
    }

    return 0;
}


/*
 *===========================================================================
 *                      ipnet_cmd_qc_add_filter
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipnet_cmd_qc_show_filter(Ipnet_cmd_qc *p)
{
    ipnet_cmd_qc_set_filter_id(p);
    ipcom_printf("Not implemented"IP_LF);
    return 0;
}


/*
 *===========================================================================
 *                    ipnet_cmd_qc_print_usage
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipnet_cmd_qc_print_usage(void)
{
    ipcom_printf("NAME"IP_LF);
    ipcom_printf("   qc - configure network interface output queue(s)"IP_LF);
    ipcom_printf(IP_LF);
    ipcom_printf("SYNOPSIS"IP_LF);
    ipcom_printf("   qc [-V vr] queue [ show | add | del ] dev <if> [ root | parent <q-id> ] [ handle <q-id> ] <type> [type specific opt]"IP_LF);
    ipcom_printf("   qc [-V vr] filter [ show | add | del ] dev <if> parent <q-id> handle <f-id> [filter args] flowid <q-id>"IP_LF);
    ipcom_printf(IP_LF);
    ipcom_printf("DESCRIPTION"IP_LF);
    ipcom_printf("   qc is used to control which queues that is added to network interfaces"IP_LF);
    ipcom_printf("   and is used to control the filters rules on container queues."IP_LF);
    ipcom_printf("   The filter parser knows the following fields:"IP_LF);
    ipcom_printf("       proto <number>"IP_LF);
    ipcom_printf("       tclass <number> ('tclass' can be substituted for 'tos'"IP_LF);
    ipcom_printf("       srcport <range>"IP_LF);
    ipcom_printf("       dstport <range>"IP_LF);
    ipcom_printf("       srcaddr <addr>[/<prefix>]"IP_LF);
    ipcom_printf("       dstaddr <addr>[/<prefix>]"IP_LF);
    ipcom_printf(IP_LF);
    ipcom_printf("SWITCHES"IP_LF);
    ipcom_printf("   -V vr        Apply the operation on the specified virtual router."IP_LF);
    ipcom_printf("COMMANDS"IP_LF);
    ipcom_printf("   show         Show the configuration for a queue or filter."IP_LF);
    ipcom_printf("   add ...      Add/replace a queue or filter."IP_LF);
    ipcom_printf("   del ...      Delete a queue or filter."IP_LF);
    ipcom_printf(IP_LF);
    ipcom_printf("VALUES"IP_LF);
    ipcom_printf("   <if>     Name of the interface."IP_LF);
    ipcom_printf("   <q-id>   ID of an output queue, must be unique per interface, unless it should replace an"IP_LF);
    ipcom_printf("            existing queue."IP_LF);
    ipcom_printf("   <f-id>   ID of a filter, must be unique per queue unless is should replace an existing filter."IP_LF);
    ipcom_printf("   <type>   Queue type, e.g. fifo, htbc."IP_LF);
    ipcom_printf("   <range>  Numeric range, format is a, a-, -b or a-b."IP_LF);
    ipcom_printf("   <addr>   IPv4 or IPv6 address. Note that source and destination address must be specified for the same"IP_LF);
    ipcom_printf("            domain per rule."IP_LF);
    ipcom_printf("   <prefix> Number of bites used for the network identifier."IP_LF);
    ipcom_printf("EXAMPLES"IP_LF);
    ipcom_printf("     (1) Add a HTBC as root to eth0, max bandwith is 2 Mbit with a burst of 100k byte: " IP_LF);
    ipcom_printf("         > qc queue add dev eth0 root handle 3 htbc rate 2Mbit burst 100kb" IP_LF);
    ipcom_printf("     (2) Add a FIFO that can queue up to 50 packets to the HTBC added in (1): " IP_LF);
    ipcom_printf("         > qc queue add dev eth0 parent 3 handle 31 fifo limit 50" IP_LF);
    ipcom_printf("     (3) Add a filter with ID 5 to queue (1) so that all TCP packets end up in (2): " IP_LF);
    ipcom_printf("         > qc filter dev eth0 parent 1 handle 5 proto 6 flowid 31" IP_LF);
    ipcom_printf("     (4) Add another filter to queue (1) so that all UDP packets sent to 2001::/16 ends up in (2): " IP_LF);
    ipcom_printf("         > qc filter dev eth0 parent 1 handle 3 proto 17 srcaddr 2001::/16 flowid 31" IP_LF);
    ipcom_printf(IP_LF);
}


/*
 ****************************************************************************
 * 10                   GLOBAL FUNCTIONS
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 11                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                      ipnet_cmd_qc
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipnet_cmd_qc(int argc, char **argv)
{
    Ipnet_cmd_qc cmd;
    int          ret;

    ipcom_memset(&cmd, 0, sizeof(cmd));
    cmd.argc = argc;
    cmd.argv = argv;
    ipnet_cmd_qc_next_arg(&cmd);

#ifdef IPCOM_USE_INET
    cmd.fd = ipcom_socket(IP_AF_INET, IP_SOCK_DGRAM, 0);
#else
    cmd.fd = ipcom_socket(IP_AF_INET6, IP_SOCK_DGRAM, 0);
#endif
    if (cmd.fd == IP_SOCKERR)
    {
        IP_PANIC2();
        goto errout;
    }

    if (cmd.argc == 0)
    {
        ipnet_cmd_qc_print_usage();
        goto errout;
    }

    do
    {
        ret = 0;
        if (ipcom_strcmp(*cmd.argv, "-V") == 0)
        {
            int val;

            ipnet_cmd_qc_next_arg(&cmd);
            if (cmd.argc == 0)
            {
                ipnet_cmd_qc_print_usage();
                goto errout;
            }

            val = ipcom_atoi(*cmd.argv);
            ipnet_cmd_qc_next_arg(&cmd);

            ret = ipcom_setsockopt(cmd.fd, IP_SOL_SOCKET, IP_SO_X_VR, &val, sizeof(val));
            if (ret < 0)
            {
                ipcom_printf("ipcom_setsockopt() IP_SO_X_VR ");
                goto errout;
            }
            ret = 1;
        }

        if (ipcom_strcmp(*cmd.argv, "-silent") == 0)
        {
            cmd.silent = IP_TRUE;
            ipnet_cmd_qc_next_arg(&cmd);
            ret = 1;
        }
    } while (ret);

    if (ipnet_cmd_qc_parse_op(&cmd) < 0)
        goto errout;
    if (ipnet_cmd_qc_parse_dev(&cmd) < 0)
        goto errout;
    if (ipnet_cmd_qc_parse_parent(&cmd) < 0)
        goto errout;
    if (ipnet_cmd_qc_parse_id(&cmd) < 0)
        goto errout;

    ret = cmd.cmd_op(&cmd);
 leave:
    if (cmd.fd != IP_INVALID_SOCKET)
        (void)ipcom_socketclose(cmd.fd);
    return ret;

 errout:
    ret = -1;
    goto leave;
}

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
