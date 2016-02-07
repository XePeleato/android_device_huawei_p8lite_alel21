/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_dmalloc.c,v $ $Revision: 1.62 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/src/ipcom_dmalloc.c,v $
 *   $Author: kenneth $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Lennart Bang <lob@interpeak.se>
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

#define IPCOM_KERNEL
#include "ipcom_config.h"

/* IPCOM_ALLOC_SPACE
 * Space after allocated buffers to detect bad memory writes.
 */
#define IPCOM_ALLOC_SPACE     16


/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#define IPCOM_DMALLOC_C

#define IPCOM_USE_CLIB_PROTO
#include "ipcom_type.h"
#include "ipcom_time.h"
#include "ipcom_cstyle.h"
#include "ipcom_clib.h"
#include "ipcom_getopt.h"
#include "ipcom_err.h"
#include "ipcom_os.h"


/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

#define IPCOM_MEM_MAXBUF 100

#ifdef IP_PORT_VXWORKS
/*
 * In the VxWorks command interpreter shell, 'mem list' conflicts
 * with a 'mem list' command that disassembles machine instructions.
 * So, we name the 'mem' command 'ipmem' in VxWorks.
 */
#define MEM "ipmem"
#else
#define MEM "mem"
#endif

/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */


/*
 *===========================================================================
 *                         Ipcom_membuf
 *===========================================================================
 * 32 bytes sized memory buffer header.
 */
typedef struct Ipcom_membuf_struct
{
    struct Ipcom_membuf_struct  *next;
    struct Ipcom_membuf_struct  *prev;
    Ip_size_t   size;   /* Size of user memory */
    Ip_u32      line;   /* The line on which the memory block was allocated */

    Ip_u32      msec;
    Ip_u32      id;
    Ip_u8       flags;
    Ip_u8       unused[3];
    Ip_u32      magic;

    /* User memory of size 'size'. */

    /* Ip_u8    pad[X]; */
    /* null terminated file name. */
    /* null terminated function (optional). */
}
Ipcom_membuf;

#define IPCOM_MEMBUF_MAGIC  0xbabebeef

#define IPCOM_MEMBUF_FLAG_FUNCTION    0x1  /* got function string. */


/*
 *===========================================================================
 *                         Ipcom_mem_summary
 *===========================================================================
 */
typedef struct Ipcom_mem_summary_struct
{
    int      totbufs;
    Ip_u32   totsize;
    Ip_u32   maxsize;
}
Ipcom_mem_summary;


/*
 *===========================================================================
 *                   Global ipcom_mem data
 *===========================================================================
 * Internal global Ipcom_mem data.
 */
typedef struct Ipcom_mem_data_struct
{
    Ip_time_t     boot_time;

    Ipcom_membuf  mem;
    Ip_u32        mark_id;
}
Ipcom_mem_data;


/*
 ****************************************************************************
 * 6                    EXTERN PROTOTYPES
 ****************************************************************************
 */

#ifdef ZEBOS
extern char *zebos_memory_name(int type);
#endif


/*
 ****************************************************************************
 * 7                    LOCAL PROTOTYPES
 ****************************************************************************
 */

#if IPCOM_USE_SHELL >= IPCOM_SHELL_IPCOM
IP_PUBLIC int ipcom_cmd_mem(int argc, char **argv);
#endif

IP_PUBLIC void *ipcom_dmalloc(Ip_size_t size, const char *file, const char *function, Ip_u32 line);
IP_PUBLIC void *ipcom_dcalloc(Ip_size_t nelem, Ip_size_t elsize, const char *file, const char *function, Ip_u32 line);
IP_PUBLIC void *ipcom_drealloc(void *ptr, Ip_size_t size, const char *file, const char *function, Ip_u32 line);
IP_PUBLIC char *ipcom_dstrdup(const char *s1, const char *file, const char *function, Ip_u32 line);
IP_PUBLIC void ipcom_dfree(void *ptr);


/*
 ****************************************************************************
 * 8                    DATA
 ****************************************************************************
 */

#if IPCOM_USE_SHELL >= IPCOM_SHELL_IPCOM && !defined(IP_PORT_LKM)
IP_STATIC const char *ipcom_dmalloc_product[] =
{
    "total",
    "ipcom total",
    "ipcom_pkt",
    "ipcom_sysvar",
    "ipcom_telnet",
    "ipcom_file",
    "ipcom_shell",
    "ipcom_os",
    "ipcom_tmo",
    "ipcom_func",
    "ipcom_auth",
    "ipcom_syslog",
    "ipcom_drv_eth",
    "ipcom_drv_ppp",
    "ipcom",
    "ipcrypto",
    "ipipsec",
    "ipike",
    "ipl2tp",
    "ipldapc",
    "iplite",
    "ipnat",
    "ippppoe",
    "ipradius",
    "iprip",
    "ipssh",
    "ipsslproxy",
    "ipssl",
    "ipftp",
    "ipfirewall",
    "ipdhcpd",
    "ipdhcpc",
    "ipwebs",
    "ipnetsnmp",
    "ipnet",
    "iptftp",
    "ipsntp",
    "ipdhcps6",
    "ipdhcps",
    "ipsnmp",
    "ipdhcpr",
    "ipdnsc",
    "ipemanate",
    "ipfreescale",
    "ipprism",
    "ipsafenet",
    "ip8021x",
    "ipeap",
    "iphwcrypto",
    "ipcavium",
    IP_NULL
};
#endif

IP_STATIC Ipcom_mem_data ipcom_mem;

IP_STATIC Ip_bool ipcom_mem_initialized = IP_FALSE;


/*
 ****************************************************************************
 * 9                    STATIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipcom_dmalloc_init
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipcom_dmalloc_init(void)
{
    Ip_u32   msr;

    if (ipcom_mem_initialized)
        return;

    msr = ipcom_interrupt_disable();

    /* Check again to avoid a small race condition. */
    if (ipcom_mem_initialized)
    {
        ipcom_interrupt_enable(msr);
        return;
    }

    ipcom_mem_initialized = IP_TRUE;

    ipcom_memset(&ipcom_mem, 0, sizeof(ipcom_mem));
    ipcom_mem.mem.next = &ipcom_mem.mem;
    ipcom_mem.mem.prev = &ipcom_mem.mem;
    ipcom_mem.mem.magic = IPCOM_MEMBUF_MAGIC;

    ipcom_interrupt_enable(msr);

#ifdef IPCOM_USE_TIME
    {
        struct Ip_tms tms;
        (void)ipcom_times(&tms);

        ipcom_mem.boot_time = ipcom_time(IP_NULL);
    }
#endif
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
 *                    ipcom_dmalloc
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC void *
ipcom_dmalloc(Ip_size_t size, const char *file, const char *function, Ip_u32 line)
{
    Ipcom_membuf  *membuf;
    Ip_u8         *buf;
    int     i;
    int     file_length;
    int     function_length;
    Ip_size_t memsize;
    Ip_u32   msr;

    ipcom_dmalloc_init();

#ifdef ZEBOS
    /* Special ZebOS memory debug fix. */
    char    funcbuf[32];
    if (function && (int)function < 1000)
    {
        ipcom_sprintf(funcbuf, "zebos%d", (int)function);
        function = (const char *)funcbuf;
    }
#endif /* ZEBOS */

    (void)file;
    (void)function;
    (void)line;

    /*ip_assert2(size > 0);*/
    if (size == 0)
        return IP_NULL;

    if (file)
    {
        for (i = ipcom_strlen(file) - 1; i >= 0; i--)
            if (file[i] == '/' || file[i] == '\\')
            {
                file = &file[i+1];  /*lint !e679 */
                break;
            }
        file_length = ipcom_strlen(file) + 1;
    }
    else
        file_length = 0;
    function_length = function ? (ipcom_strlen(function) + 1) : 0;
    memsize = sizeof(Ipcom_membuf) + size + IPCOM_ALLOC_SPACE + file_length + function_length;

    membuf = ipcom_malloc(memsize);
    if (membuf == IP_NULL)
        return IP_NULL;

    membuf->size = size;
    membuf->line = line;
    membuf->msec = ipcom_time(IP_NULL) - ipcom_mem.boot_time;
    membuf->id = ++ipcom_mem.mem.id;
    membuf->flags = 0;
    membuf->magic = IPCOM_MEMBUF_MAGIC;
    buf = (Ip_u8 *)membuf + sizeof(Ipcom_membuf);

    for (i = 0; i < IPCOM_ALLOC_SPACE; i++)
        buf[i+size] = 0xee;

    if (file)
        ipcom_strcpy((char *)(buf + size + IPCOM_ALLOC_SPACE), file);
    if (function)
    {
        ipcom_strcpy((char *)(buf + size + IPCOM_ALLOC_SPACE + file_length), function);
        IP_BIT_SET(membuf->flags, IPCOM_MEMBUF_FLAG_FUNCTION);
    }


    /* Insert membuf last in list. */
    msr = ipcom_interrupt_disable();
#ifdef IP_ASSERT2
    {
        Ipcom_membuf *tmp;
        for (tmp = ipcom_mem.mem.next; tmp != &ipcom_mem.mem; tmp = tmp->next)
        {
            ip_assert(tmp != membuf);
#ifdef IP_ASSERT3
            ipcom_bufcheck((Ip_u8 *)tmp + sizeof(Ipcom_membuf));
#endif
        }
    }
#endif /* IP_ASSERT2 */
    membuf->prev = ipcom_mem.mem.prev;
    membuf->next = &ipcom_mem.mem;
    membuf->next->prev = membuf;
    membuf->prev->next = membuf;
    ipcom_mem.mem.size += membuf->size;
    ipcom_mem.mem.line++;

    ipcom_interrupt_enable(msr);

    return buf;
}


/*
 *===========================================================================
 *                    ipcom_dmalloc_ptr
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC void *
ipcom_dmalloc_ptr(Ip_size_t size)
{
    return ipcom_dmalloc(size, IP_NULL, IP_NULL, 0);
}


/*
 *===========================================================================
 *                    ipcom_dcalloc
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC void *
ipcom_dcalloc(Ip_size_t nelem, Ip_size_t elsize, const char *file, const char *function, Ip_u32 line)
{
    Ip_size_t  tot;
    void *buf;

    tot = nelem * elsize;
    if (tot > 0)
    {
        buf = ipcom_dmalloc(tot, file, function, line);
        if (buf != IP_NULL)
        {
            ipcom_memset(buf, 0, tot);
            return buf;
        }
    }
    return IP_NULL;
}


/*
 *===========================================================================
 *                    ipcom_drealloc
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC void *
ipcom_drealloc(void *ptr, Ip_size_t size, const char *file, const char *function, Ip_u32 line)
{
    Ipcom_membuf  *membuf;
    void *buf;

    if (ptr != IP_NULL)
    {
        if (size == 0)
        {
            ipcom_dfree(ptr);
            return IP_NULL;
        }
        membuf = (Ipcom_membuf *)((Ip_u8 *)ptr - sizeof(Ipcom_membuf));
        if (size <= membuf->size)
            return ptr;

        buf = ipcom_dmalloc(size, file, function, line);
        if (buf != IP_NULL)
        {
            ipcom_memcpy(buf, ptr, membuf->size);
            ipcom_dfree(ptr);
        }
    }
    else
    {
        buf = ipcom_dmalloc(size, file, function, line);
    }

    return buf;
}


/*
 *===========================================================================
 *                         ipcom_dfree
 *===========================================================================
 * Description:   Free the dynamic memory pointer to by 'ptr'. If 'ptr'
 *                is IP_NULL, no operation is performed.
 * Parameters:    ptr - Pointer to the dynamic memory to free.
 * Returns:       .
 *
 */
IP_PUBLIC void
ipcom_dfree(void *ptr)
{
    Ipcom_membuf  *membuf;
    Ip_u32   msr;

    if (ptr == IP_NULL)
        return;

    ipcom_bufcheck(ptr);

    membuf = (Ipcom_membuf *)((Ip_u8 *)ptr - sizeof(Ipcom_membuf));

    msr = ipcom_interrupt_disable();

#ifdef IP_ASSERT2
    {
        Ipcom_membuf *tmp;
        Ip_bool       found = IP_FALSE;

        for (tmp = ipcom_mem.mem.next; tmp != &ipcom_mem.mem; tmp = tmp->next)
        {
#ifdef IP_ASSERT3
            ipcom_bufcheck((Ip_u8 *)tmp + sizeof(Ipcom_membuf));
#endif
            if (tmp == membuf)
                found = IP_TRUE;
        }
        ip_assert(found);
    }
#endif /* IP_ASSERT2 */

    /* remove entry from list. */
    membuf->next->prev = membuf->prev;
    membuf->prev->next = membuf->next;
    ipcom_mem.mem.size -= membuf->size;
    ip_assert(ipcom_mem.mem.line > 0);
    ipcom_mem.mem.line--;

    ipcom_interrupt_enable(msr);

    /* garble memory to cause crash if access pointer inside it */
    ipcom_memset(ptr, 0xfe, membuf->size);

    ipcom_free(membuf);
}


/*
 *===========================================================================
 *                    ipcom_dstrdup
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC char *
ipcom_dstrdup(const char *s1, const char *file, const char *function, Ip_u32 line)
{
    char *s2;

    s2 = ipcom_dmalloc(ipcom_strlen(s1) + 1, file, function, line);
    if (s2 == IP_NULL)
        return IP_NULL;

    ipcom_strcpy(s2, s1);
    return s2;
}


/*
 *===========================================================================
 *                    ipcom_bufcheck
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_bufcheck(void *ptr)
{
#ifdef IP_ASSERT
    int i;
    Ipcom_membuf  *membuf;

    membuf = (Ipcom_membuf *)((Ip_u8 *)ptr - sizeof(Ipcom_membuf));
    ip_assert(membuf->magic == IPCOM_MEMBUF_MAGIC);
    ip_assert(membuf->next);
    ip_assert(membuf->prev);
    ip_assert(membuf->size > 0);

    for (i = 0; i < IPCOM_ALLOC_SPACE; i++)
    {
        ip_assert(((Ip_u8 *)ptr)[membuf->size + i] == 0xee);
    }
#else
    (void)ptr;
#endif /* IP_ASSERT */
}


/*
 *===========================================================================
 *                    ipcom_cmd_mem
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
#if IPCOM_USE_SHELL >= IPCOM_SHELL_IPCOM && !defined(IP_PORT_LKM)
IP_PUBLIC int
ipcom_cmd_mem(int argc, char **argv)
{
    int i, c;
    Ipcom_getopt  opt;
    Ipcom_membuf  *membuf;
    Ip_u32 msr;
    char buf[128];
    char filebuf[48];
    char *inckey[10];
    int incnum = 0;
    char *exckey[10];
    char *funckey;
    char *nofunckey;
    char *file;
    char *function;
    int excnum = 0;
    int all = 0;
    int show_addr = 0;
    int show_time = 0;
    int show_id = 0;
    Ip_u32  minsize = 0;
    Ip_u32  maxsize = 0xffffffff;
    Ip_u32  mintime = 0;
    Ip_u32  maxtime = 0xffffffff;
    Ip_u32 minid;
    Ip_u32 maxid;

    static struct Ip_option long_options[] =
    {
        {"all", 0, 0, 0},
        {"file", 1, 0, 0},
        {"nofile", 1, 0, 0},
        {"function", 1, 0, 0},
        {"addr", 0, 0, 0},
        {"minsize", 1, 0, 0},
        {"maxsize", 1, 0, 0},
        {"time", 0, 0, 0},
        {"mintime", 1, 0, 0},
        {"maxtime", 1, 0, 0},
        {"id", 0, 0, 0},
        {"nofunction", 1, 0, 0},
        {0, 0, 0, 0}
    };

    if (ipcom_mem_initialized == IP_FALSE)
    {
        ipcom_printf(MEM ": List is empty"IP_LF);
        return 0;
    }

    ipcom_memset(inckey, 0, sizeof(inckey));
    ipcom_memset(exckey, 0, sizeof(exckey));
    funckey = IP_NULL;
    nofunckey = IP_NULL;

    if (argc < 2)
    {
    usage:
        ipcom_fprintf(ip_stderr,
                      "Interpeak " MEM " command, version 1.1"IP_LF
                      "usage:  "IP_LF
                      "   " MEM " list [-flags]  - list buffers"IP_LF
                      "   " MEM " summary        - show memory summary"IP_LF
                      "   " MEM " mark <val>     - set id mark (val = 0 to clear)"IP_LF
#if !defined(IP_PORT_OSE) && !defined(IP_PORT_OSECK) && !defined(IP_PORT_OSE5)
                      "   " MEM " max            - print max size of buffer"IP_LF
#endif
#ifdef ZEBOS
                      "   " MEM " zebos          - show zebos memory summary"IP_LF
#endif
                      "list flags:  "IP_LF);
        ipcom_fprintf(ip_stderr,
                      "   -all               ignore mark when listing"IP_LF
                      "   -file <file>       list entries from file <file>"IP_LF
                      "   -nofile <file>     do not list entries from file <file>"IP_LF
                      "   -function <func>   list entries from function <func>"IP_LF
                      "   -nofunction <func> do nost list entries from function <func>"IP_LF
                      "   -minsize <val>     list entries of size > <val>"IP_LF
                      "   -maxsize <val>     list entries of size < <val>"IP_LF);
        ipcom_fprintf(ip_stderr,
                      "   -mintime <val>     list entries of time > <val>"IP_LF
                      "   -maxtime <val>     list entries of time < <val>"IP_LF
                      "   -time              show buffer allocation time"IP_LF
                      "   -addr              show buffer address"IP_LF
                      "   -id                show buffer id"IP_LF
                      IP_LF
                      "   note: there may be up to 10 -file and -nofile options"IP_LF
                      IP_LF);
        return 0;
    }

    /* Parse options. */
    ipcom_getopt_clear_r(&opt);
    opt.long_only = 1;
    opt.long_options = long_options;
    while ((c = ipcom_getopt_r(argc - 1, argv + 1, "h", &opt)) != -1)
    {
        switch (c)
        {
        case 'h' :
            goto usage;
        case 0 :
            switch(opt.long_index)
            {
            case 0 : /* all */
                all = 1;
                break;
            case 1 : /* file */
                inckey[incnum++] = opt.optarg;
                break;
            case 2 : /* nofile */
                exckey[excnum++] = opt.optarg;
                break;
            case 3 : /* function */
                funckey = opt.optarg;
                break;
            case 4 : /* addr */
                show_addr = 1;
                break;
            case 5 : /* minsize */
                minsize = ipcom_atol(opt.optarg);
                break;
            case 6 : /* maxsize */
                maxsize = ipcom_atol(opt.optarg);
                break;
            case 7 : /* time */
                show_time = 1;
                break;
            case 8 : /* mintime */
                mintime = ipcom_atol(opt.optarg);
                break;
            case 9 : /* maxtime */
                maxtime = ipcom_atol(opt.optarg);
                break;
            case 10 : /* id */
                show_id = 1;
                break;
            case 11 : /* nofunction */
                nofunckey = opt.optarg;
                break;
            default:
                ipcom_printf (MEM ": unknown long option %s", long_options[opt.long_index].name);
                if (opt.optarg)
                    ipcom_printf(" with arg %s", opt.optarg);
                ipcom_printf(IP_LF);
                return -1;
            }
            break;
        default:
            ipcom_printf(MEM ": unknown option %c"IP_LF, (char)c);
            return -1;
        }
    }

    /* Set max and min id. */
    msr = ipcom_interrupt_disable();
    minid = all ? 0 : ipcom_mem.mark_id;
    maxid = ipcom_mem.mem.id;
    ipcom_interrupt_enable(msr);

    /* list command. */
    if (ipcom_strcmp(argv[1], "list") == 0)
    {
        int length;
        int totbufs = 0, shownbufs = 0;
        Ip_u32 totsize = 0, shownsize = 0;
        Ip_bool have_function = IP_FALSE;

        msr = ipcom_interrupt_disable();
        for (membuf = ipcom_mem.mem.next; membuf != &ipcom_mem.mem; membuf = membuf->next)
            if (IP_BIT_ISSET(membuf->flags, IPCOM_MEMBUF_FLAG_FUNCTION))
            {
                have_function = IP_TRUE;
                break;
            }
        ipcom_interrupt_enable(msr);

        if (have_function)
            ipcom_printf("%-30s %-30s %-6s",
                         "File:line", "Function", "Size");
        else
            ipcom_printf("%-30s %-6s",
                         "File:line", "Size");
        if (show_time)
            ipcom_printf("%-10s", "Msec");
        if (show_id)
            ipcom_printf("%-8s", "Id");
        if (show_addr)
            ipcom_printf("%-8s", "Ptr");
        ipcom_printf(IP_LF);

    next:
        msr = ipcom_interrupt_disable();
        for (membuf = ipcom_mem.mem.next; membuf != &ipcom_mem.mem; membuf = membuf->next)
        {
            if (membuf->id > minid && membuf->id <= maxid)
            {
                ipcom_bufcheck((Ip_u8 *)membuf + sizeof(Ipcom_membuf));

                totbufs++;
                totsize += membuf->size;
                minid = membuf->id;

                file = (char *)((Ip_u8 *)membuf + sizeof(Ipcom_membuf) + membuf->size + IPCOM_ALLOC_SPACE);
                ipcom_snprintf(filebuf, sizeof(filebuf), "%s:%ld", file, membuf->line);
                if (IP_BIT_ISSET(membuf->flags, IPCOM_MEMBUF_FLAG_FUNCTION))
                    function = file + ipcom_strlen(file) + 1;
                else
                    function = IP_NULL;

                *buf = '\0';

                /* Sizes. */
                if (membuf->size < minsize || membuf->size > maxsize)
                    goto skip;

                /* Time. */
                if (membuf->msec < mintime || membuf->msec > maxtime)
                    goto skip;

                /* Function. */
                if (funckey && function && !ipcom_strstr(function, funckey))
                    goto skip;

                /* Nofunction. */
                if (nofunckey && function && ipcom_strstr(function, nofunckey))
                    goto skip;

                /* Exclude these files: */
                for (i = 0; i < excnum; i++)
                    if (ipcom_strstr(file, exckey[i]))
                        goto skip;

                /* Include these files: */
                for (i = 0; i < incnum; i++)
                    if (!ipcom_strstr(file, inckey[i]))
                        goto skip;

                shownbufs++;
                shownsize += membuf->size;
                if (have_function)
                    length = ipcom_snprintf(buf, sizeof(buf),
                                            "%-30s %-30s %-6lu",
                                            filebuf,
                                            function ? function : "",
                                            (unsigned long) membuf->size);
                else
                    length = ipcom_snprintf(buf, sizeof(buf),
                                            "%-30s %-6lu",
                                            filebuf,
                                            (unsigned long) membuf->size);
                if (show_time)
                    length += ipcom_snprintf(buf + length, sizeof(buf) - length,
                                             "%-10ld",
                                             membuf->msec);
                if (show_id)
                    length += ipcom_snprintf(buf + length, sizeof(buf) - length,
                                             "%-8ld",
                                             membuf->id);
                if (show_addr)
                    length += ipcom_snprintf(buf + length, sizeof(buf) - length,
                                             "%-8p",
                                             membuf + 1);
            skip:
                ipcom_interrupt_enable(msr);
                if (*buf)
                {
                    ipcom_printf(buf);
                    ipcom_printf(IP_LF);
                }
                goto next;
            }
        }

        ipcom_interrupt_enable(msr);

        ipcom_printf(IP_LF);
        if (totbufs != shownbufs)
            ipcom_printf("Shown: %d buffers totalling %ld bytes.\n", shownbufs, shownsize);
        ipcom_printf("Total: %d buffers totalling %ld bytes.\n", totbufs, totsize);
    }

    /* summary command. */
    else if (ipcom_strcmp(argv[1], "summary") == 0)
    {
        Ipcom_mem_summary  *sum;
        Ip_size_t count;

        for (count = 0; ipcom_dmalloc_product[count] != IP_NULL; count++)
        {
        }

        sum = ipcom_malloc((count + 1) * sizeof(Ipcom_mem_summary));
        if (sum == IP_NULL)
        {
            ipcom_printf(MEM ": out of memory"IP_LF);
            return -1;
        }
        ipcom_memset(sum, 0, (count + 1) * sizeof(Ipcom_mem_summary));

        msr = ipcom_interrupt_disable();
        for (membuf = ipcom_mem.mem.next; membuf != &ipcom_mem.mem; membuf = membuf->next)
            if (membuf->id > minid && membuf->id <= maxid)
            {
                file = (char *)((Ip_u8 *)membuf + sizeof(Ipcom_membuf) + membuf->size + IPCOM_ALLOC_SPACE);

                for (i = 2; ipcom_dmalloc_product[i] != IP_NULL; i++)
                    if (ipcom_strncmp(file, ipcom_dmalloc_product[i], ipcom_strlen(ipcom_dmalloc_product[i])) == 0)
                        break;
                sum[i].totbufs++;
                sum[i].totsize += membuf->size;
                sum[i].maxsize = IP_MAX(sum[i].maxsize, membuf->size);
                if (ipcom_strncmp(file, "ipcom", 5) == 0)
                {
                    sum[1].totbufs++;
                    sum[1].totsize += membuf->size;
                    sum[1].maxsize = IP_MAX(sum[0].maxsize, membuf->size);
                }
                sum[0].totbufs++;
                sum[0].totsize += membuf->size;
                sum[0].maxsize = IP_MAX(sum[0].maxsize, membuf->size);
            }
        ipcom_interrupt_enable(msr);

        ipcom_printf("%-15s%-8s%-8s%-8s"IP_LF,
                     "Product", "NumBufs", "MaxSize", "TotSize");
        ipcom_printf("=======================================\n");
        for (i = 2; ipcom_dmalloc_product[i] != IP_NULL; i++)
            if (sum[i].totbufs)
                ipcom_printf("%-15s%-8d%-8ld%-8ld\n",
                             ipcom_dmalloc_product[i],
                             sum[i].totbufs,
                             sum[i].maxsize,
                             sum[i].totsize);
        if (sum[i].totbufs)
            ipcom_printf("%-15s%-8d%-8ld%-8ld\n",
                         "unknown",
                         sum[i].totbufs,
                         sum[i].maxsize,
                         sum[i].totsize);
        ipcom_printf("=======================================\n");
        ipcom_printf("%-15s%-8d%-8ld%-8ld\n",
                     "IPCOM total",
                     sum[1].totbufs,
                     sum[1].maxsize,
                     sum[1].totsize);
        ipcom_printf("%-15s%-8d%-8ld%-8ld\n",
                     "TOTAL",
                     sum[0].totbufs,
                     sum[0].maxsize,
                     sum[0].totsize);
        ipcom_free(sum);
    }

    /* mark command. */
    else if (ipcom_strcmp(argv[1], "mark") == 0)
    {
        if (argc < opt.optind + 2)
            ipcom_mem.mark_id = ipcom_mem.mem.id;
        else
            ipcom_mem.mark_id = ipcom_atol(argv[opt.optind+1]); /*lint !e679 */
        ipcom_printf(MEM ": mark id set to %ld.\n", ipcom_mem.mark_id);
    }

    /* max command. */
#if !defined(IP_PORT_OSE) && !defined(IP_PORT_OSECK) && !defined(IP_PORT_OSE5)
    else if (ipcom_strcmp(argv[1], "max") == 0)
    {
        void *arr[IPCOM_MEM_MAXBUF];
        Ip_size_t tot = 0;
        Ip_size_t sz;
        int indx = 0;

        /* Allocate as large a buffer as possible. */
        for (sz = 0x1000000; sz != 0; sz >>= 1)
        {
            while (indx < IPCOM_MEM_MAXBUF &&
                   (arr[indx] = ipcom_malloc(sz)) != IP_NULL)
            {
                tot += sz;
                indx++;
            }

            if (indx >= IPCOM_MEM_MAXBUF)
            {
                ipcom_printf(MEM ": overflow, increase IPCOM_MEM_MAXBUF in src/ipcom_dmalloc.c"IP_LF);
                break;
            }
        }

        /* Free in opposite order which hopefully is more heap friendly */
        while (--indx >= 0)
            ipcom_free(arr[indx]);

        ipcom_printf("Max buffer size available: %ld bytes"IP_LF, (Ip_u32)tot);
    }
#endif

#ifdef ZEBOS
    else if (ipcom_strcmp(argv[1], "zebos") == 0)
    {
        Ipcom_mem_summary  *sum;

#define ZEBOS_MAX 400
        sum = ipcom_malloc(ZEBOS_MAX * sizeof(Ipcom_mem_summary));
        if (sum == IP_NULL)
        {
            ipcom_printf(MEM ": out of memory"IP_LF);
            return -1;
        }
        ipcom_memset(sum, 0, ZEBOS_MAX * sizeof(Ipcom_mem_summary));
        msr = ipcom_interrupt_disable();
        for (membuf = ipcom_mem.mem.next; membuf != &ipcom_mem.mem; membuf = membuf->next)
            if (membuf->id > minid && membuf->id <= maxid)
            {
                file = (char *)((Ip_u8 *)membuf + sizeof(Ipcom_membuf) + membuf->size + IPCOM_ALLOC_SPACE);
                if (IP_BIT_ISSET(membuf->flags, IPCOM_MEMBUF_FLAG_FUNCTION))
                    function = file + ipcom_strlen(file) + 1;
                else
                    function = IP_NULL;
                if (function && ipcom_strncmp(function, "zebos", 5) == 0)
                {
                    i = ipcom_atoi(function + 5);
                    if (i < ZEBOS_MAX - 1)
                    {
                        sum[i].totbufs++;
                        sum[i].totsize += membuf->size;
                        sum[i].maxsize = IP_MAX(sum[i].maxsize, membuf->size);
                        sum[ZEBOS_MAX - 1].totbufs++;
                        sum[ZEBOS_MAX - 1].totsize += membuf->size;
                        sum[ZEBOS_MAX - 1].maxsize = IP_MAX(sum[ZEBOS_MAX - 1].maxsize, membuf->size);
                    }
                }
            }
        ipcom_interrupt_enable(msr);

        ipcom_printf("%-30s%-8s%-8s%-8s"IP_LF,
                     "Product", "NumBufs", "MaxSize", "TotSize");
        ipcom_printf("======================================================\n");
        for (i = 0; i < ZEBOS_MAX - 1; i++)
            if (sum[i].totbufs)
                ipcom_printf("%-30s%-8ld%-8ld%-8ld\n",
                             zebos_memory_name(i),
                             sum[i].totbufs,
                             sum[i].maxsize,
                             sum[i].totsize);
        ipcom_printf("======================================================\n");
        ipcom_printf("%-30s%-8ld%-8ld%-8ld\n",
                     "TOTAL",
                     sum[ZEBOS_MAX - 1].totbufs,
                     sum[ZEBOS_MAX - 1].maxsize,
                     sum[ZEBOS_MAX - 1].totsize);
        ipcom_free(sum);
    }
#endif /* ZEBOS */

    /* unknown command. */
    else
        ipcom_printf (MEM ": unknown command '%s'"IP_LF, argv[1]);

    return 0;
}
#endif /* #if IPCOM_USE_SHELL >= IPCOM_SHELL_IPCOM && !defined(IP_PORT_LKM) */


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
