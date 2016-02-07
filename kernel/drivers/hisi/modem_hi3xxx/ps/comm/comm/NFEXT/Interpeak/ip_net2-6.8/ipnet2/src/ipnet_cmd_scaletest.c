/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_cmd_scaletest.c,v $ $Revision: 1.2 $
 *   $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_cmd_scaletest.c,v $
 *   $Author: kenneth $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Kenneth Jonsson <kenneth@interpeak.se>
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

#ifdef IPCOM_KERNEL
#undef IPCOM_KERNEL
#endif
#include "ipnet_config.h"


/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#ifdef IPNET_INCLUDE_CMD_SCALETEST

#define IPCOM_USE_CLIB_PROTO
#include <ipcom_clib.h>
#include <ipcom_cmd.h>
#include <ipcom_getopt.h>
#include <ipcom_time.h>

#include "ipnet.h"
#include "ipnet_h.h"
#include "ipnet_netif.h"
#include "ipnet_routesock.h"
#include "ipnet_cmd.h"

/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

/* Measure the time it take to add/delete the N:th route */
#define IPNET_ST_OP_ROUTE 1
/* Measure the time it take to add/delete the N:th network interface */
#define IPNET_ST_OP_IF    2
/* Measure the time it take to add/delete the N:th virtual router */
#define IPNET_ST_OP_VR    3
/* Measure the time it take to open/close the N:th socket */
#define IPNET_ST_OP_SOCK  4

/* Next measurement range is found by adding a constant to the current location */
#define IPNET_ST_STEP_ADD 1
/* Next measurement range is found by multiply a constant to the current location */
#define IPNET_ST_STEP_MUL 2


#define IPNET_ST_GET_VR(cmd) ((cmd)->current + 1)

/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

typedef struct Ipnet_cmd_scaletest_struct
{
    int     sock;
    int     op;        /* IPNET_ST_OP_xxx */
    int     step_op;   /* IPNET_ST_STEP_xxx */
    int     step;      /* Entries to add/mul before next measurement */
    int     range;     /* Number of entries to measure on */
    int     total;     /* Total number of entries to add */
    int     current;   /* Current number of added entries */
    Ip_bool is_adding; /* Is IP_TRUE when adding entries, IP_FALSE when deleting */
    Ip_bool is_timing;
    int     next_start;
    int     next_stop;
    struct Ip_timeval start;
    struct Ip_timeval stop;
    int     domain;
    int    *sockets;
    Ip_u16  vr;
}
Ipnet_cmd_scaletest;

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

/*
 ****************************************************************************
 * 8                    DATA
 ****************************************************************************
 */

IP_STATIC Ipcom_cmd_int_str_map ipnet_cmd_op[] =
{
    { IPNET_ST_OP_ROUTE, "route"  },
    { IPNET_ST_OP_IF,    "if"     },
    { IPNET_ST_OP_VR,    "vr"     },
    { IPNET_ST_OP_SOCK,  "socket" },
    { -1,                IP_NULL }
};

IP_STATIC Ipcom_cmd_int_str_map ipnet_cmd_step[] =
{
    { IPNET_ST_STEP_ADD, "add" },
    { IPNET_ST_STEP_MUL, "mul" },
    { -1,                IP_NULL }
};

/*
 ****************************************************************************
 * 9                    STATIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipnet_cmd_scaletest_start
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipnet_cmd_scaletest_start(Ipnet_cmd_scaletest *cmd)
{
    ipcom_printf("\t%s %8d - %-8d",
                 cmd->is_adding ? "add" : "del",
                 cmd->next_start,
                 cmd->next_stop);
    cmd->is_timing    = IP_TRUE;
    cmd->stop.tv_usec = 0;
    cmd->stop.tv_sec  = 0;
    ipcom_microtime(&cmd->start);
}


/*
 *===========================================================================
 *                    ipnet_cmd_scaletest_stop
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipnet_cmd_scaletest_stop(Ipnet_cmd_scaletest *cmd)
{
    ipcom_microtime(&cmd->stop);
    cmd->is_timing = IP_FALSE;
    ipcom_printf(" took %d usec"IP_LF,
                 (cmd->stop.tv_usec - cmd->start.tv_usec)
                 + (cmd->stop.tv_sec - cmd->start.tv_sec) * 1000000);
}


/*
 *===========================================================================
 *                    ipnet_cmd_scaletest_next_range
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipnet_cmd_scaletest_next_range(Ipnet_cmd_scaletest *cmd)
{

    if (cmd->is_adding)
    {
        switch (cmd->step_op)
        {
        case IPNET_ST_STEP_ADD:
            cmd->next_start += cmd->step;
            break;
        case IPNET_ST_STEP_MUL:
            cmd->next_start *= cmd->step;
            break;
        default:
            break;
        }
        cmd->next_stop = cmd->next_start + cmd->range - 1;
    }
    else
    {
        if (cmd->next_stop > cmd->next_start)
            /* Direction was swapped */
            cmd->next_stop = cmd->next_start;

        switch (cmd->step_op)
        {
        case IPNET_ST_STEP_ADD:
            cmd->next_stop -= cmd->step;
            break;
        case IPNET_ST_STEP_MUL:
            cmd->next_stop /= cmd->step;
            break;
        default:
            break;
        }
        cmd->next_start = cmd->next_stop + cmd->range - 1;
    }
}


/*
 *===========================================================================
 *                    ipnet_cmd_scaletest_vr
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_bool
ipnet_cmd_scaletest_vr(Ipnet_cmd_scaletest *cmd)
{
    unsigned long request;
    Ip_u16        vr = IPNET_ST_GET_VR(cmd);

    if (cmd->is_adding)
        request = IP_SIOCADDVR;
    else
        request = IP_SIOCDELVR;

    if (ipcom_socketioctl(cmd->sock, request, &vr) < 0)
    {
        ipcom_printf("Failed to %s VR: %s"IP_LF,
                     cmd->is_adding ? "add" : "delete",
                     ipcom_strerror(ipcom_errno));
        return IP_FALSE;
    }
    return IP_TRUE;
}


/*
 *===========================================================================
 *                    ipnet_cmd_scaletest_route
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_bool
ipnet_cmd_scaletest_route(Ipnet_cmd_scaletest *cmd)
{
    Ipnet_cmd_route r;
    int             ret;

    ipcom_memset(&r, 0, sizeof(r));
    r.no_dns   = IP_TRUE;
    r.domain   = cmd->domain;
    r.rt_flags = IPNET_RTF_STATIC | IPNET_RTF_REJECT | IPNET_RTF_HOST | IPNET_RTF_UP | IPNET_RTF_DONE;
    r.silent   = IP_TRUE;
#ifdef IPCOM_USE_INET
    if (r.domain == IP_AF_INET)
    {
        struct Ip_sockaddr_in *s = &r.dst.sa_in;
        s->sin_family = IP_AF_INET;
        s->sin_len    = sizeof(struct Ip_sockaddr_in);
        s->sin_addr.s_addr = ip_htonl(cmd->current + 1);
    }
#endif
#ifdef IPCOM_USE_INET6
    if (r.domain == IP_AF_INET6)
    {
        struct Ip_sockaddr_in6 *s = &r.dst.sa_in6;
        s->sin6_family = IP_AF_INET6;
        s->sin6_len    = sizeof(struct Ip_sockaddr_in6);
        s->sin6_addr.in6.addr32[2] = ip_htonl(cmd->current + 1);
    }
#endif

    if (cmd->is_adding)
        ret = ipnet_cmd_route_add(cmd->sock, &r);
    else
        ret = ipnet_cmd_route_delete(cmd->sock, &r);

    return ret == 0;
}


/*
 *===========================================================================
 *                    ipnet_cmd_scaletest_if
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_bool
ipnet_cmd_scaletest_if(Ipnet_cmd_scaletest *cmd)
{
    unsigned long   request;
    struct Ip_ifreq ifreq;

    if (cmd->is_adding)
        request = IP_SIOCIFCREATE;
    else
        request = IP_SIOCIFDESTROY;

    ipcom_sprintf(ifreq.ifr_name, "lo%d", cmd->current + 1);
    if (ipcom_socketioctl(cmd->sock, request, &ifreq) < 0)
    {
        ipcom_printf("Failed to %s interface: %s"IP_LF,
                     cmd->is_adding ? "add" : "delete",
                     ipcom_strerror(ipcom_errno));
        return IP_FALSE;
    }
    return IP_TRUE;
}


/*
 *===========================================================================
 *                    ipnet_cmd_scaletest_sock
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_bool
ipnet_cmd_scaletest_sock(Ipnet_cmd_scaletest *cmd)
{
    if (cmd->sockets == IP_NULL)
    {
        Ip_size_t s = (cmd->total + cmd->range)* sizeof(*cmd->sockets);

        cmd->sockets = ipcom_malloc(s);
        if (cmd->sockets == IP_NULL)
        {
            ipcom_printf("Failed to allocate %d bytes of memory"IP_LF, s);
            return IP_FALSE;
        }
    }
    if (cmd->is_adding == IP_FALSE)
        (void)ipcom_socketclose(cmd->sockets[cmd->current]);
    else
    {
        cmd->sockets[cmd->current] = ipcom_socket(cmd->domain, IP_SOCK_DGRAM, 0);
        if (cmd->sockets[cmd->current] == IP_INVALID_SOCKET)
        {
            ipcom_printf("Failed to open socket: %s"IP_LF,
                         ipcom_strerror(ipcom_errno));
            return IP_FALSE;
        }
    }

    return IP_TRUE;
}


/*
 *===========================================================================
 *                    ipnet_cmd_scaletest_print_usage
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipnet_cmd_scaletest_print_usage()
{
    int i;

    ipcom_printf(IP_LF);
    ipcom_printf("NAME" IP_LF);
    ipcom_printf("     scaletest - measure the scaling of selected IPNET operations" IP_LF);
    ipcom_printf(IP_LF);
    ipcom_printf("SYNOPSIS" IP_LF);
    ipcom_printf("     scaletest <op> [-n <N>] [-r <N>] [-s <N>] [-o <no>] " IP_LF);
    ipcom_printf(IP_LF);
    ipcom_printf("DESCRIPTION" IP_LF);
    ipcom_printf("     scaletest will measure the time it takes to repeate an operation" IP_LF);
    ipcom_printf("     a number of times." IP_LF);
    ipcom_printf(IP_LF);
    ipcom_printf("     <op>    Operation to measure, can be [ ");
    for (i = 0; ipnet_cmd_op[i].str != IP_NULL; i++)
        ipcom_printf("%s ", ipnet_cmd_op[i].str);
    ipcom_printf("]"IP_LF);
    ipcom_printf("     -n <N>  total number of entries to add" IP_LF);
    ipcom_printf("     -r <N>  number of entries included in each measurement" IP_LF);
    ipcom_printf("     -s <N>  number of entries to add before next measurement" IP_LF);
    ipcom_printf("     -o <no> how the next measurement range is calculated" IP_LF);
    ipcom_printf("     -4      use the AF_INET domain" IP_LF);
    ipcom_printf("     -6      use the AF_INET6 domain" IP_LF);
    ipcom_printf("EXAMPLES" IP_LF);
    ipcom_printf("     Measure the time it take to add 10 routes when FIB contains" IP_LF);
    ipcom_printf("     0, 10, 100, 1000 and 10000 route entries" IP_LF);
    ipcom_printf("       > scaletest route -n 10000 -r 10 -t 10 -o mul" IP_LF);
    ipcom_printf("     Measure the time it take to open 20 sockets when stack has" IP_LF);
    ipcom_printf("     0, 100, 200, 300, 400, 500 socket open" IP_LF);
    ipcom_printf("       > scaletest route -n 500 -r 20 -t 100 -o add" IP_LF);
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
 *                    ipnet_cmd_scaletest
 *===========================================================================
 * Description: The entry point for the 'scaletest' shell command.
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipnet_cmd_scaletest(int argc, char **argv)
{
    Ipnet_cmd_scaletest cmd;
    int                 i;
    Ipcom_getopt        opt;
    struct Ip_timeval   start;
    struct Ip_timeval   stop;
    Ip_bool (*test_func)(Ipnet_cmd_scaletest *);

    ipcom_getopt_clear_r(&opt);

    ipcom_memset(&cmd, 0, sizeof(cmd));
    cmd.is_adding = IP_TRUE;
    cmd.step_op   = IPNET_ST_STEP_ADD;
    cmd.step      = 100;
    cmd.range     = 10;
    cmd.total     = 1000;
    cmd.sock      = ipcom_socket(IP_AF_ROUTE, IP_SOCK_RAW, 0);
#ifdef IPCOM_USE_INET
    cmd.domain    = IP_AF_INET;
#else
    cmd.domain    = IP_AF_INET6;
#endif

    if (cmd.sock == IP_INVALID_SOCKET)
    {
        ipcom_printf("Failed to open AF_ROUTE socket :%s"IP_LF,
                     ipcom_strerror(ipcom_errno));
        goto cleanup;
    }

    if (opt.optind >= argc)
    {
        ipnet_cmd_scaletest_print_usage();
        goto cleanup;
    }

    cmd.op = ipcom_cmd_str_to_key(ipnet_cmd_op, argv[opt.optind++]);
    if (cmd.op == -1)
    {
        ipcom_printf("Operation %s is not valid"IP_LF, *argv);
        goto cleanup;
    }
    if (cmd.op != IPNET_ST_OP_ROUTE)
        if (ipcom_shutdown(cmd.sock, IP_SHUT_RD) < 0)
        {
            ipcom_printf("Read shutdown failed :%s"IP_LF,
                         ipcom_strerror(ipcom_errno));
            goto cleanup;
        }

    while ((i = ipcom_getopt_r(argc, argv, "46n:o:r:s:", &opt)) != -1)
    {
        switch (i)
        {
        case '4':
            cmd.domain = IP_AF_INET;
            break;
        case '6':
            cmd.domain = IP_AF_INET6;
            break;
        case 'n':
            cmd.total = ipcom_atoi(opt.optarg);
            break;
        case 'o':
            cmd.step_op = ipcom_cmd_str_to_key(ipnet_cmd_step, opt.optarg);
            if (cmd.step_op == -1)
            {
                ipcom_printf("Step operation %s is not valid"IP_LF, opt.optarg);
                goto cleanup;
            }
            break;
        case 'r':
            cmd.range = ipcom_atoi(opt.optarg);
            break;
        case 's':
            cmd.step = ipcom_atoi(opt.optarg);
            break;
        default:
            goto cleanup;
        }
    }

    switch (cmd.op)
    {
    case IPNET_ST_OP_ROUTE:
        test_func = ipnet_cmd_scaletest_route;
        break;
    case IPNET_ST_OP_IF:
        test_func = ipnet_cmd_scaletest_if;
        break;
    case IPNET_ST_OP_VR:
        test_func = ipnet_cmd_scaletest_vr;
        break;
    case IPNET_ST_OP_SOCK:
        test_func = ipnet_cmd_scaletest_sock;
        break;
    default:
        IP_PANIC();
        goto cleanup;
    }

    ipcom_printf("%d entries will be added and deleted, each measure range has %d entries"IP_LF,
                 cmd.total, cmd.range);

    cmd.next_start = (cmd.step_op == IPNET_ST_STEP_ADD ? 1 : cmd.range);
    cmd.next_stop = cmd.next_start + cmd.range - 1;

    ipcom_microtime(&start);
    do {
        if (cmd.is_timing == IP_FALSE && cmd.current == cmd.next_start)
            ipnet_cmd_scaletest_start(&cmd);

        if (test_func(&cmd) == IP_FALSE)
        {
            cmd.is_adding = IP_FALSE;
            for (;cmd.current >= 0; --cmd.current)
                /* Remove all entries added before the error */
                (void) test_func(&cmd);
            break;
        }

        if (cmd.is_timing && cmd.current == cmd.next_stop)
        {
            ipnet_cmd_scaletest_stop(&cmd);
            ipnet_cmd_scaletest_next_range(&cmd);
        }

        cmd.current += (cmd.is_adding ? 1 : -1);

        if (cmd.current >= cmd.total && cmd.is_timing == IP_FALSE)
        {
            cmd.is_adding = IP_FALSE;
            ipnet_cmd_scaletest_next_range(&cmd);
            cmd.current--;
        }

    } while (cmd.current >= 0);
    ipcom_microtime(&stop);
    ipcom_printf("Total time %d msec"IP_LF,
                 (stop.tv_usec - start.tv_usec) / 1000 + (stop.tv_sec - start.tv_sec) * 1000);


 cleanup:
    ipcom_free(cmd.sockets);
    ipcom_socketclose(cmd.sock);
    return 0;
}


#endif /* IPNET_INCLUDE_CMD_SCALETEST */

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

