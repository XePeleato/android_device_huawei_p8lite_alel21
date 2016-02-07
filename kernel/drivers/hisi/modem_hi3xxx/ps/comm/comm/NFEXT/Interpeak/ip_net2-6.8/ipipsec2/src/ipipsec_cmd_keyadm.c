/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipipsec_cmd_keyadm.c,v $ $Revision: 1.57 $
 *   $Source: /home/interpeak/CVSRoot/ipipsec2/src/ipipsec_cmd_keyadm.c,v $
 *   $Author: ulf $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Lennart Bang <lob@interpeak.se>
 ******************************************************************************
 */

/*
 ****************************************************************************
 * 1  DESCRIPTION.
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
#include "ipipsec_config.h"


/*
 ****************************************************************************
 * 3  INCLUDE FILES.
 ****************************************************************************
 */

#define IPCOM_USE_CLIB_PROTO

/* IPCOM */
#include <ipcom_clib.h>
#if IPCOM_USE_SHELL != IPCOM_SHELL_NONE || defined(IP_PORT_LAS)
#include <ipcom_clib2.h>
#endif
#include <ipcom_sock.h>
#include <ipcom_inet.h>
#include <ipcom_os.h>

#include <ipcom_tags.h>
#include <ipcom_getopt.h>
#include <ipcom_sysvar.h>

/* Interpeak IPSEC */
#include "ipipsec.h"
#include "ipipsec_pfkeyv2.h"


/*
 ****************************************************************************
 * 4  MACROS/TYPES
 ****************************************************************************
 */

/* Should not be evenly dividable by 8 in order to detect truncated SADB messages */
#define MAX_RCVBUF_SIZE       (2048 - 1)


/*
 *===========================================================================
 *                         SADB_EXT_ BITMAP
 *===========================================================================
 */
#define SEBIT_SA                (1 << SADB_EXT_SA)
#define SEBIT_LIFETIME_CURRENT  (1 << SADB_EXT_LIFETIME_CURRENT)
#define SEBIT_LIFETIME_HARD     (1 << SADB_EXT_LIFETIME_HARD)
#define SEBIT_LIFETIME_SOFT     (1 << SADB_EXT_LIFETIME_SOFT)
#define SEBIT_ADDRESS_SRC       (1 << SADB_EXT_ADDRESS_SRC)
#define SEBIT_ADDRESS_DST       (1 << SADB_EXT_ADDRESS_DST)
#define SEBIT_ADDRESS_PROXY     (1 << SADB_EXT_ADDRESS_PROXY)
#define SEBIT_KEY_AUTH          (1 << SADB_EXT_KEY_AUTH)
#define SEBIT_KEY_ENCRYPT       (1 << SADB_EXT_KEY_ENCRYPT)
#define SEBIT_IDENTITY_SRC      (1 << SADB_EXT_IDENTITY_SRC)
#define SEBIT_IDENTITY_DST      (1 << SADB_EXT_IDENTITY_DST)
#define SEBIT_SENSITIVITY       (1 << SADB_EXT_SENSITIVITY)
#define SEBIT_PROPOSAL          (1 << SADB_EXT_PROPOSAL)
#define SEBIT_SUPPORTED         (1 << SADB_EXT_SUPPORTED)
#define SEBIT_SPIRANGE          (1 << SADB_EXT_SPIRANGE)
#define SEBIT_X_SRC_MASK        (1 << SADB_X_EXT_SRC_MASK)
#define SEBIT_X_DST_MASK        (1 << SADB_X_EXT_DST_MASK)
#define SEBIT_X_PROTOCOL        (1 << SADB_X_EXT_PROTOCOL)
#define SEBIT_X_SA2             (1 << SADB_X_EXT_SA2)
#define SEBIT_X_SRC_FLOW        (1 << SADB_X_EXT_SRC_FLOW)
#define SEBIT_X_DST_FLOW        (1 << SADB_X_EXT_DST_FLOW)
#define SEBIT_X_DST2            (1 << SADB_X_EXT_DST2)
#define SEBIT_X_FLOW_OPTIONS    (1 << SADB_X_EXT_FLOW_OPTIONS)
#define SEBIT_X_UDPENCAP        (1 << SADB_X_EXT_UDPENCAP)
#define SEBIT_X_SRCMAX_FLOW     (1 << SADB_X_EXT_SRCMAX_FLOW)
#define SEBIT_X_DSTMAX_FLOW     (1 << SADB_X_EXT_DSTMAX_FLOW)
#define SEBIT_X_EXT_SA_OPTIONS  (1 << SADB_X_EXT_SA_OPTIONS)

/* Groups of bits */
#define SEBIT_LIFETIME  (SEBIT_LIFETIME_CURRENT | SEBIT_LIFETIME_HARD | SEBIT_LIFETIME_SOFT)
#define SEBIT_ADDRESS   (SEBIT_ADDRESS_SRC | SEBIT_ADDRESS_DST | SEBIT_ADDRESS_PROXY)
#define SEBIT_KEY       (SEBIT_KEY_AUTH | SEBIT_KEY_ENCRYPT)
#define SEBIT_IDENTITY  (SEBIT_IDENTITY_SRC | SEBIT_IDENTITY_DST)
#define SEBIT_X_FLOW    (SEBIT_X_SRC_FLOW | SEBIT_X_DST_FLOW)
#define SEBIT_X_MAXFLOW (SEBIT_X_SRCMAX_FLOW | SEBIT_X_DSTMAX_FLOW)
#define SEBIT_X_MASK    (SEBIT_X_SRC_MASK | SEBIT_X_DST_MASK)


/*
 ****************************************************************************
 * 5  TYPES
 ****************************************************************************
 */


/*
 *===========================================================================
 *                      sadb_address_full
 *===========================================================================
 */
struct sadb_address_full
{
    struct sadb_address       addr;
    union Ip_sockaddr_union   sock;
    Ip_u8                     pad[8];
};


/*
 *===========================================================================
 *                         Ipipsec_sadb_extbits
 *===========================================================================
 */
typedef struct Extbits_struct
{
    Ip_u32   in_req;
    Ip_u32   in_opt;
}
Extbits;


/*
 *===========================================================================
 *                      Keyadm_sadb
 *===========================================================================
 */
typedef struct Keyadm_sadb_struct
{
    struct sadb_msg        *msg;
    struct sadb_sa         *sa;
    struct sadb_lifetime   *lifetime_current;
    struct sadb_lifetime   *lifetime_hard;
    struct sadb_lifetime   *lifetime_soft;
    struct sadb_address    *address_src;
    struct sadb_address    *address_dst;
    struct sadb_address    *address_proxy;
    struct sadb_key        *key_auth;
    struct sadb_key        *key_encrypt;
    struct sadb_ident      *ident_src;
    struct sadb_ident      *ident_dst;
    struct sadb_sens       *sens;
    struct sadb_prop       *prop;
    struct sadb_supported  *supported;
    struct sadb_spirange   *spirange;
    struct sadb_address    *src_mask;
    struct sadb_address    *dst_mask;
    struct sadb_protocol   *protocol;
    struct sadb_sa         *sa2;
    struct sadb_address    *src_flow;
    struct sadb_address    *dst_flow;
    struct sadb_address    *dst2;
    struct sadb_x_flowopt  *flowopt;
    struct sadb_x_udpencap *udpencap;
    struct sadb_address    *srcmax_flow;
    struct sadb_address    *dstmax_flow;
    struct sadb_x_saopt    *saopt;
}
Keyadm_sadb;


/*
 *===========================================================================
 *                         Variables - Vars
 *===========================================================================
 */
typedef struct Vars_struct
{
    int              argc;
    char           **argv;
    Ip_fd            fd;
    void            *exthdr[SADB_EXT_MAX+1];
    Keyadm_sadb     *sadb;
    struct sadb_msg  sadbmsg;

    Ip_u8           *orgbuf;
    char            *stdoutbuf;
    Ip_u8           *msgbuf;

    /* options */
    Ip_u32           minspi, maxspi;
    Ip_u32           spi_n, spi2_n;
    union Ip_sockaddr_union  srcaddr;
    union Ip_sockaddr_union  dstaddr;
    union Ip_sockaddr_union  proxyaddr;
    union Ip_sockaddr_union  dstaddr2;
    Ip_s8            satype, satype2;
    Ip_u8            have_satype;
    Ip_u8            flow_type;
    Ip_u8            dscp;
    Ip_u32           flow_id;
    union Ip_sockaddr_union  srcmask;
    union Ip_sockaddr_union  dstmask;
    union Ip_sockaddr_union  srcflow;
    union Ip_sockaddr_union  dstflow;
    union Ip_sockaddr_union  srcmaxflow;
    union Ip_sockaddr_union  dstmaxflow;
    union Ip_sockaddr_union  zeroaddr;
    Ip_u16           dstport_n, srcport_n;
    Ip_u16           dstmaxport_n, srcmaxport_n;
    Ip_u16           udpencap_port;   /*NOTE: port in host endian! */
    Ip_u8            transport_proto;
    Ip_u8            have_transport_proto;
    Ip_u8            ports0, ports1;
    int              replay;
    int              flow_priority, sa_priority;
    Ip_u32           ls_sec, lh_sec;

    Ip_s8            encrypt_type;
    Ip_u8            encrypt_len;
    struct sadb_key  encrypt_sadb;
    Ip_u8            encrypt_key[128];

    Ip_s8            auth_type;
    Ip_u8            auth_len;
    struct sadb_key  auth_sadb;
    Ip_u8            auth_key[32];

    Ip_u32           old;
    Ip_u32           bypass;
    Ip_u32           test;
    Ip_u32           extmap;
    int              silent, count;
    Ip_u32           sa_flags;
    Ip_u32           timeout_seconds;
}
Vars;


/*
 *===========================================================================
 *                         Command
 *===========================================================================
 */
typedef struct Command_struct
{
    char    *name;
    int    (*func)(Vars *vars);
    void   (*usage)(Vars *vars);
    int     type;
}
Command;


/*
 *===========================================================================
 *                         Ctrl
 *===========================================================================
 */
#define CTRL_GET 1
#define CTRL_SET 2
typedef struct Ctrl_struct
{
    char     *name;
    Ip_tag    id;
    Ip_u32    mode;        /* 1=get, 2=set, 3=get&set */
}
Ctrl;



/*
 ****************************************************************************
 * 6  PROTOTYPES.
 ****************************************************************************
 */

IP_PUBLIC int ipipsec_cmd_keyadm(int argc, char **argv);

static int keyadm_dump(Vars *vars);
static int keyadm_getspi(Vars *vars);
static int keyadm_flush(Vars *vars);
static int keyadm_get(Vars *vars);
static int keyadm_update(Vars *vars);
static int keyadm_add(Vars *vars);
static int keyadm_delete(Vars *vars);
static int keyadm_addflow(Vars *vars);
static int keyadm_delflow(Vars *vars);
static int keyadm_grpspis(Vars *vars);
static int keyadm_findflow(Vars *vars);
static int keyadm_dumpflow(Vars *vars);
static int keyadm_monitor(Vars *vars);
#ifdef IP_DEBUG
static int keyadm_register(Vars *vars);
#endif

static void keyadm_add_usage(Vars *vars);
static void keyadm_delete_usage(Vars *vars);
static void keyadm_get_usage(Vars *vars);
static void keyadm_grpspis_usage(Vars *vars);
static void keyadm_getspi_usage(Vars *vars);
static void keyadm_update_usage(Vars *vars);

static void keyadm_addflow_usage(Vars *vars);
static void keyadm_delflow_usage(Vars *vars);
static void keyadm_findflow_usage(Vars *vars);

static IP_CONST char *
keyadm_cmd_bits_to_str(char **map, Ip_u32 bits,
                       char *str, Ip_size_t str_len);


/*
 ****************************************************************************
 * 7  DATA.
 ****************************************************************************
 */

static IP_CONST char *keyadm_sa_flags_bitmap[] =
{
    "pfs",
    "halfiv",
    "tunnel",
    "chaindel",
    "0x10",
    "replaceflow",
    "ingress",
    "randompad",
    "noreplay",
    "udpencap",
    "egress",
    "tfc",
    "esn",
    "srcreq",
    "clrdf",
    "setdf",
    ""
};

static Ip_u32 sadbmsgseq = 1;

static IP_CONST Command commands[] =
{
    /* name,       function */
    { "dump",      keyadm_dump,     IP_NULL, SADB_DUMP },
    { "flush",     keyadm_flush,    IP_NULL, SADB_FLUSH },
    { "get",       keyadm_get,      keyadm_get_usage, SADB_GET },
    { "getspi",    keyadm_getspi,   keyadm_getspi_usage, SADB_GETSPI },
    { "update",    keyadm_update,   keyadm_update_usage, SADB_UPDATE },
    { "add",       keyadm_add,      keyadm_add_usage, SADB_ADD },
    { "delete",    keyadm_delete,   keyadm_delete_usage, SADB_DELETE },
    { "addflow",   keyadm_addflow,  keyadm_addflow_usage, SADB_X_ADDFLOW },
    { "delflow",   keyadm_delflow,  keyadm_delflow_usage, SADB_X_DELFLOW },
    { "group",     keyadm_grpspis,  keyadm_grpspis_usage, SADB_X_GRPSPIS },
    { "findflow",  keyadm_findflow, keyadm_findflow_usage, SADB_X_FINDFLOW },
    { "dumpflow",  keyadm_dumpflow, IP_NULL, SADB_X_DUMPFLOW },
    { "monitor",   keyadm_monitor,  IP_NULL, 0 },
#ifdef IP_DEBUG
    { "register",  keyadm_register, IP_NULL, SADB_REGISTER },
#endif
    {  0,          IP_NULL,         IP_NULL, 0 }
};

#define EXT_SADBADDR_SIZE_MIN   (int)(sizeof(struct sadb_address) + IP_ROUNDUP(sizeof(struct Ip_sockaddr_in),8))
#define EXT_SADBADDR_SIZE_IN    24 /* same expression as above */
#define EXT_SADBADDR_SIZE_IN6   40 /* 8+sizeof(struct Ip_sockaddr_in6) = 8+28 = 36 -> rounded up = 40 */

static IP_CONST int keyadm_ext_sizes[SADB_EXT_MAX+1] =
{   0,
    sizeof(struct sadb_sa),
    sizeof(struct sadb_lifetime), sizeof(struct sadb_lifetime), sizeof(struct sadb_lifetime),
    -EXT_SADBADDR_SIZE_MIN, -EXT_SADBADDR_SIZE_MIN, -EXT_SADBADDR_SIZE_MIN,
    -(int)sizeof(struct sadb_key), -(int)sizeof(struct sadb_key),
    -(int)sizeof(struct sadb_ident), -(int)sizeof(struct sadb_ident),
    -(int)sizeof(struct sadb_sens),
    -(int)sizeof(struct sadb_prop),
    -(int)sizeof(struct sadb_supported),
    sizeof(struct sadb_spirange),
    -EXT_SADBADDR_SIZE_MIN, -EXT_SADBADDR_SIZE_MIN,
    sizeof(struct sadb_protocol),
    sizeof(struct sadb_sa),
    -EXT_SADBADDR_SIZE_MIN, -EXT_SADBADDR_SIZE_MIN,
    -EXT_SADBADDR_SIZE_MIN,
    sizeof(struct sadb_x_flowopt), /* flowopt */
    sizeof(struct sadb_x_udpencap),
    -EXT_SADBADDR_SIZE_MIN, -EXT_SADBADDR_SIZE_MIN,
    sizeof(struct sadb_x_saopt), /* saopt */
};


static IP_CONST Extbits  extbits[SADB_MAX+1] =
{
  /* FIELDS: (SADB_RESERVED) */
  { 0, /* in_req */
    0  /* in_opt */
  },
  /* SADB_GETSPI */
  { SEBIT_SPIRANGE | SEBIT_ADDRESS_SRC | SEBIT_ADDRESS_DST,
    SEBIT_SA | SEBIT_SPIRANGE | SEBIT_ADDRESS_SRC | SEBIT_ADDRESS_DST
  },
  /* SADB_UPDATE */
  { SEBIT_SA | SEBIT_ADDRESS_SRC | SEBIT_ADDRESS_DST,
    SEBIT_SA | SEBIT_LIFETIME | SEBIT_ADDRESS | SEBIT_KEY | SEBIT_IDENTITY | SEBIT_SENSITIVITY | SEBIT_X_EXT_SA_OPTIONS
  },
  /* SADB_ADD */
  { SEBIT_SA | SEBIT_ADDRESS_DST | SEBIT_ADDRESS_SRC,
    SEBIT_SA | SEBIT_LIFETIME | SEBIT_ADDRESS | SEBIT_KEY | SEBIT_IDENTITY | SEBIT_SENSITIVITY | SEBIT_X_UDPENCAP | SEBIT_X_EXT_SA_OPTIONS
  },
  /* SADB_DELETE */
  { SEBIT_SA | SEBIT_ADDRESS_DST | SEBIT_ADDRESS_SRC,
    SEBIT_SA | SEBIT_ADDRESS_SRC | SEBIT_ADDRESS_DST | SEBIT_X_EXT_SA_OPTIONS
  },
  /* SADB_GET */
  { SEBIT_SA | SEBIT_ADDRESS_SRC | SEBIT_ADDRESS_DST,
    SEBIT_SA | SEBIT_ADDRESS_SRC | SEBIT_ADDRESS_DST | SEBIT_X_EXT_SA_OPTIONS
  },
  /* SADB_ACQUIRE */
  { 0,
    SEBIT_ADDRESS | SEBIT_IDENTITY | SEBIT_SENSITIVITY | SEBIT_PROPOSAL
  },
  /* SADB_REGISTER */
  { 0,
    0
  },
  /* SADB_EXPIRE */
  { SEBIT_SA | SEBIT_ADDRESS_SRC | SEBIT_ADDRESS_DST,
    SEBIT_SA | SEBIT_ADDRESS_SRC | SEBIT_ADDRESS_DST
  },
  /* SADB_FLUSH */
  { 0,
    0
  },
  /* SADB_DUMP */
  { 0,
    0
  },
  /* SADB_X_PROMISC */
  { 0,
    0
  },
  /* SADB_X_ADDFLOW */
  { SEBIT_X_FLOW,
    SEBIT_SA | SEBIT_ADDRESS_DST | SEBIT_X_MASK | SEBIT_X_FLOW | SEBIT_ADDRESS_SRC | SEBIT_X_PROTOCOL | SEBIT_X_FLOW_OPTIONS | SEBIT_X_MAXFLOW | SEBIT_X_EXT_SA_OPTIONS
  },
  /* SADB_X_DELFLOW */
  { 0,
    SEBIT_SA | SEBIT_X_MASK | SEBIT_X_FLOW | SEBIT_X_PROTOCOL | SEBIT_ADDRESS_DST | SEBIT_X_MAXFLOW | SEBIT_X_EXT_SA_OPTIONS
  },
  /* SADB_X_GRPSPIS */
  { SEBIT_SA | SEBIT_X_SA2 | SEBIT_X_DST2 | SEBIT_ADDRESS_DST | SEBIT_X_PROTOCOL,
    SEBIT_SA | SEBIT_X_SA2 | SEBIT_X_DST2 | SEBIT_ADDRESS_DST | SEBIT_X_PROTOCOL | SEBIT_X_EXT_SA_OPTIONS | SEBIT_ADDRESS_SRC
  },
  /* SADB_X_BINDSA */
  { SEBIT_SA | SEBIT_X_SA2 | SEBIT_X_DST2 | SEBIT_ADDRESS_DST | SEBIT_X_PROTOCOL,
    SEBIT_SA | SEBIT_X_SA2 | SEBIT_X_DST2 | SEBIT_ADDRESS_DST | SEBIT_X_PROTOCOL | SEBIT_X_EXT_SA_OPTIONS
  },
  /* SADB_X_FINDFLOW */
  { SEBIT_X_FLOW /* not checked this way: SEBIT_X_PROTOCOL */,
    SEBIT_X_FLOW | SEBIT_X_PROTOCOL | SEBIT_SA | SEBIT_X_FLOW_OPTIONS | SEBIT_X_EXT_SA_OPTIONS | SEBIT_ADDRESS_DST
  },
  /* SADB_X_DUMPFLOW */
  { 0,
    SEBIT_SA | SEBIT_X_FLOW_OPTIONS
  }
};


static IP_CONST char *ext_types[SADB_EXT_MAX+2] =
{
  "RESERVED",
  "-spi",
  "LIFETIME_CURRENT",
  "LIFETIME_HARD",
  "LIFETIME_SOFT",
  "-src",
  "-dst",
  "ADDRESS_PROXY",
  "KEY_AUTH",
  "KEY_ENCRYPT",
  "IDENTITY_SRC",
  "IDENTITY_DST",
  "SENSITIVITY",
  "PROPOSAL",
  "SUPPORTED",
  "-minspi",
  "-srcmask",
  "-dstmask",
  "-proto2",
  "-spi2",
  "-srcflow",
  "-dstflow",
  "-dst2",
  "-flowopt",
  "UDPENCAP",
  "-srcmax(port)",
  "-dstmax(port)",
  "\n"
};

#define EXT_TYPE(x)   ((x > SADB_EXT_MAX) ? "ILLEGAL" : ext_types[x])


static IP_CONST char *msg_types[SADB_MAX+2] =
{
  "RESERVED",
  "GETSPI",
  "UPDATE",
  "ADD",
  "DELETE",
  "GET",
  "ACQUIRE",
  "REGISTER",
  "EXPIRE",
  "FLUSH",
  "DUMP",
  "X_PROMISC",
  "X_ADDFLOW",
  "X_DELFLOW",
  "X_GRPSPIS",
  "X_BINDSA",
  "X_FINDFLOW",
  "X_DUMPFLOW",
  "\n"
};
#define MSG_TYPE(x)   ((x > SADB_MAX) ? "ILLEGAL" : msg_types[x])

static IP_CONST char *msg_satypes[SADB_SATYPE_MAX+2] =
{
  "NONE",
  "UNSPEC_1",
  "AH",
  "ESP",
  "UNSPEC_4",
  "RSVP",
  "OSPFV2",
  "RIPV2",
  "MIP",
  "IPIP",
  "X_TCPSIGNATURE",
  "BYPASS",
  "\n"
};
#define MSG_SATYPE(x)   ((x > SADB_SATYPE_MAX) ? "ILLEGAL" : msg_satypes[x])

static IP_CONST char *sa_auth[SADB_AALG_MAX+2] =
{
  "NONE",
  "MD5HMAC",
  "SHA1HMAC",
  "MD5",
  "SHA1",
  "RMD160",
  "OLDMD5",
  "OLDSHA1",
  "AESMAC",
  "\n"
};

#define SA_AUTH(x)   ((x > SADB_AALG_MAX) ? "ILLEGAL" : sa_auth[x])

static IP_CONST char *sa_encrypt[SADB_EALG_MAX+2] =
{
  "NONE",
  "DES",
  "3DES",
  "BLF",
  "CAST",
  "SKIPJACK",
  "AES",
  "AESCTR",
  "\n"
};

#define SA_ENCRYPT(x)   ((x > SADB_EALG_MAX) ? "ILLEGAL" : sa_encrypt[x])

static IP_CONST char *sa_state[SADB_SASTATE_MAX+2] =
{
  "LARVAL",
  "MATURE",
  "DYING",
  "DEAD",
  "\n"
};

#define SA_STATE(x)   ((x > SADB_SASTATE_MAX) ? "ILLEGAL" : sa_state[x])


static IP_CONST char *flow_types[] =
{
    "unspec",
    "use",
    "acquire",
    "require",
    "bypass",
    "deny",
    "dontacq"
};

#define FLOW_TYPE(x)   ((x) > 6 ? "ILLEGAL" : flow_types[x])


/*
 ****************************************************************************
 * 8  FUNCTIONS.
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    keyadm_printf
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 */
static void
keyadm_printf(Vars *vars, const char *format, ...)
{
#if IPCOM_USE_SHELL != IPCOM_SHELL_NONE || defined(IP_PORT_LAS)
    if(vars->silent == 0)
    {
        va_list   ap;
        int    retval;

        va_start(ap, format);
        retval = ipcom_vsprintf(vars->stdoutbuf, format, ap);
        va_end(ap);

        ip_assert(retval < 512);
        if (retval > 0)
            (void)ipcom_printf(vars->stdoutbuf);
    }
#endif
}


/*
 *===========================================================================
 *                    keyadm_cmd_bits_to_str
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
static IP_CONST char *
keyadm_cmd_bits_to_str(char **map, Ip_u32 bits,
                       char *str, Ip_size_t str_len)
{
    int i;
    int len = 0;

    str[0] = '\0';

    for (i = 0; i < 32 && *map[i] != '\0'; i++)
        if (IP_BIT_ISSET(bits, 1 << i))
            len += ipcom_snprintf(str + len, str_len - len, "%s ", map[i]);

    return str;
}


/*
 *===========================================================================
 *                         keyadm_strcasecmp
 *===========================================================================
 */
static int
keyadm_strcasecmp(IP_CONST char *s1, IP_CONST char *s2)
{
    while (ipcom_toupper(*s1) == ipcom_toupper(*s2))
    {
        if (*s1 == '\0')
            return 0;
        s1++;
        s2++;
    }
    return ipcom_toupper(*s1) - ipcom_toupper(*s2);
}


/*
 *===========================================================================
 *                         keyadm_search_block
 *===========================================================================
 */
static int
keyadm_search_block(char *arg, IP_CONST char **list)
{
    int    i;

    for (i = 0; **(list + i) != '\n'; i++)
        if (!keyadm_strcasecmp(arg, *(list + i)))
            return (i);

    return (-1);
}


/*
 *===========================================================================
 *                         keyadm_hex_to_long
 *===========================================================================
 */
static int
keyadm_hex_to_long(Vars *vars, char *s, int *do_exit)
{
    char    ss[3];
    ss[0] = s[0];
    ss[1] = s[1];
    ss[2] = 0;

    if (!ipcom_isxdigit((int)s[0]) || !ipcom_isxdigit((int)s[1]))
    {
        keyadm_printf(vars, "keyadm: keys should be specified in hex digits"IP_LF);
        *do_exit = 1;
    }
    else
        *do_exit = 0;

    return ipcom_strtoul(ss, IP_NULL, 16);
}


/*
 *===========================================================================
 *                    keyadm_read_addr
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
static int
keyadm_read_addr(IP_CONST char *string, union Ip_sockaddr_union *sockaddr)
{
    int  af;

#if defined(IPCOM_USE_INET) && defined(IPCOM_USE_INET6)
    if (ipcom_strchr(string, ':'))
        af = IP_AF_INET6;
    else
        af = IP_AF_INET;
#elif defined(IPCOM_USE_INET6)
    af = IP_AF_INET6;
#else
    af = IP_AF_INET;
#endif

    return ipcom_getsockaddrbyaddrname(af, IP_TRUE /* do not use dns */, string, &sockaddr->sa);
}


/*
 *===========================================================================
 *                    keyadm_ipaddr_to_extaddr
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
static void
keyadm_ipaddr_to_extaddr(struct sadb_address_full *address, union Ip_sockaddr_union *sockaddr)
{
    int  addrsize;
#if defined(IPCOM_USE_INET) && defined(IPCOM_USE_INET6)
    addrsize = sockaddr->sa.sa_family == IP_AF_INET ? EXT_SADBADDR_SIZE_IN : EXT_SADBADDR_SIZE_IN6;
#elif defined(IPCOM_USE_INET6)
    addrsize = EXT_SADBADDR_SIZE_IN6;
#else
    addrsize = EXT_SADBADDR_SIZE_IN;
#endif
    address->addr.sadb_address_len = (uint16_t)(addrsize >> 3);
    address->addr.sadb_address_proto = 0; /*!! non-zero ports -> UDP OR TCP else 0 */
    address->addr.sadb_address_prefixlen = 0;
    address->addr.sadb_address_reserved = 0;

    ipcom_memcpy(&address->sock, sockaddr, sizeof(union Ip_sockaddr_union));
}


/*
 *===========================================================================
 *                    keyadm_send_command
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
static int
keyadm_send_command(Vars *vars, Ip_u8 type, Ip_u8 satype)
{
    int   retval = 0, i, length;
    Ip_u8   *orgbuf, *msgbuf;
    struct  sadb_address_full src, dst, dst2, proxy, srcflow, srcmaxflow, dstflow, dstmaxflow, srcmask, dstmask;
    struct sadb_spirange  spi;
    struct sadb_sa sa, sa2;
    struct sadb_protocol proto2;
    struct sadb_lifetime ls, lh;
    struct sadb_x_flowopt flowopt;
    struct sadb_x_saopt saopt;
    struct sadb_x_udpencap udpencap;

    /** Auto build some extension headers **/

    /* SADB_EXT_SA */
    if (IP_BIT_ISSET(vars->extmap, SEBIT_SA))
    {
        ipcom_memset(&sa, 0, sizeof(struct sadb_sa));
        vars->sadb->sa = &sa;
        vars->sadb->sa->sadb_sa_len     = sizeof(struct sadb_sa) / sizeof(Ip_u64);
        vars->sadb->sa->sadb_sa_exttype = SADB_EXT_SA;
        vars->sadb->sa->sadb_sa_spi     = vars->spi_n;
        vars->sadb->sa->sadb_sa_replay  = (Ip_u8)vars->replay;
        vars->sadb->sa->sadb_sa_auth    = IP_MAX(0, vars->auth_type);
        vars->sadb->sa->sadb_sa_encrypt = IP_MAX(0, vars->encrypt_type);
        vars->sadb->sa->sadb_sa_flags   = vars->sa_flags;
        if (type == SADB_UPDATE || type == SADB_ADD)
        {
            vars->sadb->sa->sadb_sa_state = SADB_SASTATE_MATURE;
            if (vars->old)
            {
                IP_BIT_SET(vars->sadb->sa->sadb_sa_flags, SADB_X_SAFLAGS_NOREPLAY);
                if (vars->satype == SADB_SATYPE_ESP)
                    IP_BIT_SET(vars->sadb->sa->sadb_sa_flags, SADB_X_SAFLAGS_RANDOMPADDING);
            }

            if (IP_BIT_ISSET(vars->sa_flags, SADB_X_SAFLAGS_UDPENCAP))
            {
                vars->sadb->udpencap = (struct sadb_x_udpencap *)&udpencap;
                vars->sadb->udpencap->sadb_x_udpencap_len      = sizeof(struct sadb_x_udpencap) / sizeof(Ip_u64);
                vars->sadb->udpencap->sadb_x_udpencap_exttype  = SADB_X_EXT_UDPENCAP;
                vars->sadb->udpencap->sadb_x_udpencap_port     = ip_htons(vars->udpencap_port);
                vars->sadb->udpencap->sadb_x_udpencap_reserved = 0;
            }
        }
    }

    /* SADB_EXT_SPIRANGE */
    if (vars->maxspi && IP_BIT_ISSET(vars->extmap, SEBIT_SPIRANGE))
    {
        vars->sadb->spirange = (struct sadb_spirange *)&spi;
        vars->sadb->spirange->sadb_spirange_len      = sizeof(struct sadb_spirange) / sizeof(Ip_u64);
        vars->sadb->spirange->sadb_spirange_exttype  = SADB_EXT_SPIRANGE;
        vars->sadb->spirange->sadb_spirange_min      = vars->minspi;
        vars->sadb->spirange->sadb_spirange_max      = vars->maxspi;
        vars->sadb->spirange->sadb_spirange_reserved = 0;
    }

    /* SADB_EXT_ADDRESS_SRC */
    if (IP_BIT_ISSET(vars->extmap, SEBIT_ADDRESS_SRC))
    {
        vars->sadb->address_src = (struct sadb_address *)&src;
        keyadm_ipaddr_to_extaddr(&src, &vars->srcaddr);
        vars->sadb->address_src->sadb_address_exttype = SADB_EXT_ADDRESS_SRC;
    }

    /* SADB_EXT_ADDRESS_DST */
    if (IP_BIT_ISSET(vars->extmap, SEBIT_ADDRESS_DST))
    {
        vars->sadb->address_dst = (struct sadb_address *)&dst;
        keyadm_ipaddr_to_extaddr(&dst, &vars->dstaddr);
        vars->sadb->address_dst->sadb_address_exttype = SADB_EXT_ADDRESS_DST;
    }

    /* SADB_EXT_ADDRESS_PROXY */
    if (IP_BIT_ISSET(vars->extmap, SEBIT_ADDRESS_PROXY))
    {
        vars->sadb->address_proxy = (struct sadb_address *)&proxy;
        keyadm_ipaddr_to_extaddr(&proxy, &vars->proxyaddr);
        vars->sadb->address_proxy->sadb_address_exttype = SADB_EXT_ADDRESS_PROXY;
    }

    /* SADB_EXT_KEY_ENCRYPT */
    if (IP_BIT_ISSET(extbits[type].in_opt, SEBIT_KEY) && vars->encrypt_len > 0)
    {
        vars->sadb->key_encrypt                    = &vars->encrypt_sadb;
        vars->sadb->key_encrypt->sadb_key_len      = (uint16_t)((sizeof(struct sadb_key) + IP_ROUNDUP(vars->encrypt_len, 8)) / sizeof(Ip_u64));
        vars->sadb->key_encrypt->sadb_key_exttype  = SADB_EXT_KEY_ENCRYPT;
        vars->sadb->key_encrypt->sadb_key_bits     = (uint16_t)(vars->encrypt_len * 8);
        vars->sadb->key_encrypt->sadb_key_reserved = 0;
    }

    /* SADB_EXT_KEY_AUTH */
    if (IP_BIT_ISSET(extbits[type].in_opt, SEBIT_KEY) && vars->auth_len > 0)
    {
        vars->sadb->key_auth                    = &vars->auth_sadb;
        vars->sadb->key_auth->sadb_key_len      = (uint16_t)((sizeof(struct sadb_key) + IP_ROUNDUP(vars->auth_len, 8)) / sizeof(Ip_u64));
        vars->sadb->key_auth->sadb_key_exttype  = SADB_EXT_KEY_AUTH;
        vars->sadb->key_auth->sadb_key_bits     = (uint16_t)(vars->auth_len * 8);
        vars->sadb->key_auth->sadb_key_reserved = 0;
    }

    /* SEBIT_X_SRC_FLOW */
    if (IP_BIT_ISSET(vars->extmap, SEBIT_X_SRC_FLOW))
    {
        vars->sadb->src_flow                       = (struct sadb_address *)&srcflow;
        keyadm_ipaddr_to_extaddr(&srcflow, &vars->srcflow);
        srcflow.sock.sin.sin_port                  = vars->srcport_n;
        vars->sadb->src_flow->sadb_address_exttype = SADB_X_EXT_SRC_FLOW;

        /* SEBIT_X_SRCMAX_FLOW */
        if (IP_BIT_ISSET(vars->extmap, SEBIT_X_SRCMAX_FLOW))
        {
            vars->sadb->srcmax_flow                       = (struct sadb_address *)&srcmaxflow;
            if (ipcom_memcmp(&vars->srcmaxflow, &vars->zeroaddr, sizeof(union Ip_sockaddr_union)))
                keyadm_ipaddr_to_extaddr(&srcmaxflow, &vars->srcmaxflow);
            else
                keyadm_ipaddr_to_extaddr(&srcmaxflow, &vars->srcflow);
            if (vars->srcmaxport_n != 0)
                srcmaxflow.sock.sin.sin_port              = vars->srcmaxport_n;
            else
                srcmaxflow.sock.sin.sin_port              = vars->srcport_n;
            vars->sadb->srcmax_flow->sadb_address_exttype = SADB_X_EXT_SRCMAX_FLOW;
        }
    }

    /* SEBIT_X_DST_FLOW */
    if (IP_BIT_ISSET(vars->extmap, SEBIT_X_DST_FLOW))
    {
        vars->sadb->dst_flow                       = (struct sadb_address *)&dstflow;
        keyadm_ipaddr_to_extaddr(&dstflow, &vars->dstflow);
        dstflow.sock.sin.sin_port                  = vars->dstport_n;
        vars->sadb->dst_flow->sadb_address_exttype = SADB_X_EXT_DST_FLOW;

        /* SEBIT_X_DSTMAX_FLOW */
        if (IP_BIT_ISSET(vars->extmap, SEBIT_X_DSTMAX_FLOW))
        {
            vars->sadb->dstmax_flow                       = (struct sadb_address *)&dstmaxflow;
            if (ipcom_memcmp(&vars->dstmaxflow, &vars->zeroaddr, sizeof(union Ip_sockaddr_union)))
                keyadm_ipaddr_to_extaddr(&dstmaxflow, &vars->dstmaxflow);
            else
                keyadm_ipaddr_to_extaddr(&dstmaxflow, &vars->dstflow);
            if (vars->dstmaxport_n != 0)
                dstmaxflow.sock.sin.sin_port              = vars->dstmaxport_n;
            else
                dstmaxflow.sock.sin.sin_port              = vars->dstport_n;
            vars->sadb->dstmax_flow->sadb_address_exttype = SADB_X_EXT_DSTMAX_FLOW;
        }
    }

    /* SEBIT_X_SRC_MASK */
    if (IP_BIT_ISSET(vars->extmap, SEBIT_X_SRC_MASK))
    {
        vars->sadb->src_mask = (struct sadb_address *)&srcmask;
        keyadm_ipaddr_to_extaddr(&srcmask, &vars->srcmask);
        vars->sadb->src_mask->sadb_address_exttype = SADB_X_EXT_SRC_MASK;
    }

    /* SEBIT_X_DST_MASK */
    if (IP_BIT_ISSET(vars->extmap, SEBIT_X_DST_MASK))
    {
        vars->sadb->dst_mask = (struct sadb_address *)&dstmask;
        keyadm_ipaddr_to_extaddr(&dstmask, &vars->dstmask);
        vars->sadb->dst_mask->sadb_address_exttype = SADB_X_EXT_DST_MASK;
    }

    /* SEBIT_X_DST2 */
    if (IP_BIT_ISSET(vars->extmap, SEBIT_X_DST2))
    {
        vars->sadb->dst2 = (struct sadb_address *)&dst2;
        keyadm_ipaddr_to_extaddr(&dst2, &vars->dstaddr2);
        vars->sadb->dst2->sadb_address_exttype = SADB_X_EXT_DST2;
    }

    /* SEBIT_X_SA2 */
    if (IP_BIT_ISSET(vars->extmap, SEBIT_X_SA2))
    {
        ipcom_memset(&sa2, 0, sizeof(struct sadb_sa));
        vars->sadb->sa2                  = &sa2;
        vars->sadb->sa2->sadb_sa_spi     = vars->spi2_n;
        vars->sadb->sa2->sadb_sa_len     = sizeof(struct sadb_sa) / sizeof(Ip_u64);
        vars->sadb->sa2->sadb_sa_exttype = SADB_X_EXT_SA2;
        vars->sadb->sa2->sadb_sa_state   = SADB_SASTATE_MATURE;
    }

    /* SEBIT_LIFETIME_SOFT - soft lifetime */
    if (IP_BIT_ISSET(vars->extmap, SEBIT_LIFETIME_SOFT))
    {
        ipcom_memset(&ls, 0, sizeof(struct sadb_lifetime));
        vars->sadb->lifetime_soft = &ls;
        vars->sadb->lifetime_soft->sadb_lifetime_len = sizeof(struct sadb_lifetime) / sizeof(Ip_u64);
        vars->sadb->lifetime_soft->sadb_lifetime_exttype = SADB_EXT_LIFETIME_SOFT;
        IP_U64_SETLO(vars->sadb->lifetime_soft->sadb_lifetime_addtime, vars->ls_sec);
    }

    /* SEBIT_LIFETIME_HARD - hard lifetime */
    if (IP_BIT_ISSET(vars->extmap, SEBIT_LIFETIME_HARD))
    {
        ipcom_memset(&lh, 0, sizeof(struct sadb_lifetime));
        vars->sadb->lifetime_hard = &lh;
        vars->sadb->lifetime_hard->sadb_lifetime_len = sizeof(struct sadb_lifetime) / sizeof(Ip_u64);
        vars->sadb->lifetime_hard->sadb_lifetime_exttype = SADB_EXT_LIFETIME_HARD;
        IP_U64_SETLO(vars->sadb->lifetime_hard->sadb_lifetime_addtime, vars->lh_sec);
    }

    /* "-proto2" SEBIT_X_PROTOCOL - security protocol */
    if (IP_BIT_ISSET(vars->extmap, SEBIT_X_PROTOCOL))
    {
        ipcom_memset(&proto2, 0, sizeof(struct sadb_protocol));
        vars->sadb->protocol                        = (struct sadb_protocol *)&proto2;
        vars->sadb->protocol->sadb_protocol_len     = sizeof(struct sadb_protocol) / sizeof(Ip_u64);
        vars->sadb->protocol->sadb_protocol_exttype = SADB_X_EXT_PROTOCOL;
        vars->sadb->protocol->sadb_protocol_proto   = vars->satype2;
    }
    /* "-transport" SEBIT_X_PROTOCOL - transport protocol */
    else if (vars->have_transport_proto || vars->ports0 || vars->ports1)
    {
        ipcom_memset(&proto2, 0, sizeof(struct sadb_protocol));
        vars->sadb->protocol                        = (struct sadb_protocol *)&proto2;
        vars->sadb->protocol->sadb_protocol_len     = sizeof(struct sadb_protocol) / sizeof(Ip_u64);
        vars->sadb->protocol->sadb_protocol_exttype = SADB_X_EXT_PROTOCOL;
        vars->sadb->protocol->sadb_protocol_proto   = vars->transport_proto;
        vars->sadb->protocol->sadb_protocol_ports0  = vars->ports0;
        vars->sadb->protocol->sadb_protocol_ports1  = vars->ports1;
    }

    /* SEBIT_X_FLOW_OPTIONS - flow type, id, direction and/or priority */
    if ((type == SADB_X_ADDFLOW || type == SADB_X_DELFLOW
         || type == SADB_X_FINDFLOW || type == SADB_X_DUMPFLOW)
        && IP_BIT_ISSET(vars->extmap, SEBIT_X_FLOW_OPTIONS))
    {
        ipcom_memset(&flowopt, 0, sizeof(struct sadb_x_flowopt));
        vars->sadb->flowopt                        = (struct sadb_x_flowopt *)&flowopt;
        vars->sadb->flowopt->sadb_flowopt_len      = sizeof(struct sadb_x_flowopt) / sizeof(Ip_u64);
        vars->sadb->flowopt->sadb_flowopt_exttype  = SADB_X_EXT_FLOW_OPTIONS;
        vars->sadb->flowopt->sadb_flowopt_type     = vars->flow_type;
        if (IP_BIT_ISSET(vars->sa_flags, SADB_X_SAFLAGS_EGRESS_SA))
            vars->sadb->flowopt->sadb_flowopt_direction = 2;
        if (IP_BIT_ISSET(vars->sa_flags, SADB_X_SAFLAGS_INGRESS_SA))
            vars->sadb->flowopt->sadb_flowopt_direction = 1;
        vars->sadb->flowopt->sadb_flowopt_priority = (char)vars->flow_priority;
        vars->sadb->flowopt->sadb_flowopt_dscp     = vars->dscp;
        vars->sadb->flowopt->sadb_flowopt_id       = vars->flow_id;
    }

    if (vars->sa_priority != 0)
    {
        ipcom_memset(&saopt, 0, sizeof(struct sadb_x_saopt));
        vars->sadb->saopt                      = (struct sadb_x_saopt *)&saopt;
        vars->sadb->saopt->sadb_saopt_len      = sizeof(struct sadb_x_saopt) / sizeof(Ip_u64);
        vars->sadb->saopt->sadb_saopt_exttype  = SADB_X_EXT_SA_OPTIONS;
        vars->sadb->saopt->sadb_saopt_priority = (char)vars->sa_priority;
    }

    /* Calculate message length */
    for (length = sizeof(struct sadb_msg), i = 1; i <= SADB_EXT_MAX; i++)
        if (vars->exthdr[i] != IP_NULL)
            length += (((struct sadb_ext *)vars->exthdr[i])->sadb_ext_len * sizeof(Ip_u64));

    /* Allocate memory for the PFKEY write */
    orgbuf = (Ip_u8 *)ipcom_malloc(length + sizeof(Ip_u64));
    if (orgbuf == IP_NULL)
    {
        keyadm_printf(vars, "keyadm: out of memory"IP_LF);
        return -1;
    }
    msgbuf = IP_PTR_ALIGN(orgbuf, 8);   /* 64-bit align msgbuf */
    ip_assert(((Ip_ptrdiff_t)msgbuf & 0x7L) == 0);
    ipcom_memset(msgbuf, 0, length);

    /* Set SADB_MSG */
    vars->sadb->msg->sadb_msg_version  = PF_KEY_V2;
    vars->sadb->msg->sadb_msg_type     = type;
    vars->sadb->msg->sadb_msg_errno    = 0;
    vars->sadb->msg->sadb_msg_satype   = satype;
    vars->sadb->msg->sadb_msg_reserved = 0;
    vars->sadb->msg->sadb_msg_len      = (uint16_t)(length / sizeof(Ip_u64));
    vars->sadb->msg->sadb_msg_seq      = sadbmsgseq++;
    vars->sadb->msg->sadb_msg_pid      = ipcom_getpid();

    /* Copy over all headers */
    ipcom_memcpy(msgbuf, vars->sadb->msg, sizeof(struct sadb_msg));
    for (length = sizeof(struct sadb_msg), i = 1; i <= SADB_EXT_MAX; i++)
        if (vars->exthdr[i] != IP_NULL)
        {
            ipcom_memcpy(msgbuf + length,
                         vars->exthdr[i],
                         ((struct sadb_ext *)vars->exthdr[i])->sadb_ext_len * sizeof(Ip_u64));
            length += ((struct sadb_ext *)vars->exthdr[i])->sadb_ext_len * sizeof(Ip_u64);
        }

    /* Write the SADB message */
    if (ipcom_send(vars->fd, (char*)msgbuf, length, 0) != length)
    {
        keyadm_printf(vars, "keyadm: ipcom_send() failed: %s (%d)"IP_LF,
                      ipcom_strerror(ipcom_errno), ipcom_errno);
        if (ipcom_errno == IP_ERRNO_EINVAL || ipcom_errno == IP_ERRNO_ENOMEM
            || ipcom_errno == IP_ERRNO_ENOBUFS)
            retval = -1;
    }

    if (orgbuf)
        ipcom_free(orgbuf);

    return retval;
}


/*
 *===========================================================================
 *                    keyadm_receive_command
 *===========================================================================
 * Description:
 * Parameters:   vars and timeout_seconds (0 = block)
 * Returns:
 *
 */
static int
keyadm_receive_command(Vars *vars, Ip_u32 timeout_seconds)
{
    int   length;
    int   retval = -IP_ERRNO_EINVAL;
    struct sadb_ext   *ext;

    /*Ip_u32   extmap = 0;*/

    /* Release previous memory */
    if (vars->orgbuf)
    {
        ipcom_free(vars->orgbuf);
        vars->orgbuf = IP_NULL;
    }

    /* Prepare ext array */
    ipcom_memset(&vars->sadb->sa, 0, SADB_EXT_MAX * sizeof(void*)); /*lint !e419 */

    /* Get buffer memory */
    vars->orgbuf = (Ip_u8 *)ipcom_malloc(MAX_RCVBUF_SIZE + sizeof(Ip_u64));
    if (vars->orgbuf == IP_NULL)
    {
        keyadm_printf(vars, "keyadm: out of memory, can't receive"IP_LF);
        goto leave;
    }
    vars->msgbuf = IP_PTR_ALIGN(vars->orgbuf, 8);   /* 64-bit align msgbuf */
    ip_assert(((Ip_ptrdiff_t)vars->msgbuf & 0x7L) == 0);
    ipcom_memset(vars->msgbuf, 0, MAX_RCVBUF_SIZE);

    if (timeout_seconds != 0)
    {
        /* Set receive timeout */
        struct Ip_timeval tv;
        tv.tv_sec  = timeout_seconds;
        tv.tv_usec = 0;
        length = ipcom_setsockopt(vars->fd, IP_SOL_SOCKET, IP_SO_RCVTIMEO, &tv, sizeof(tv));
        if (length < 0)
        {
            keyadm_printf(vars, "ipcom_setsockopt() IP_SO_RCVTIMEO ");
            return -IP_ERRNO_EINVAL;
        }
    }

    /* Read a SADB message */
    vars->sadb->msg = (struct sadb_msg *)vars->msgbuf;
    length = (int)ipcom_recv(vars->fd, (char *)vars->msgbuf, MAX_RCVBUF_SIZE, 0);
    if (length < (int)sizeof(struct sadb_msg))
    {
        if (length < 0)
        {
            if (ipcom_errno == IP_ERRNO_EWOULDBLOCK)
            {
                keyadm_printf(vars, "keyadm timeout"IP_LF);
                return -IP_ERRNO_EWOULDBLOCK;
            }

            keyadm_printf(vars, "keyadm: ipcom_recv() failed: %s"IP_LF, ipcom_strerror(ipcom_errno));
            keyadm_printf(vars, " (try raise key socket buffer size with -b option)"IP_LF);
        }
        else if (length >= 3 && vars->sadb->msg->sadb_msg_errno != 0)
            keyadm_printf(vars, "keyadm: ipcom_recv() msgerrno=%s"IP_LF, ipcom_strerror(vars->sadb->msg->sadb_msg_errno));
        else
            keyadm_printf(vars, "keyadm: ipcom_recv() truncated SADB (%d)"IP_LF, length);
        goto leave;
    }
    else if (length == MAX_RCVBUF_SIZE)
        keyadm_printf(vars, "keyadm: received too large SADB (%d)"IP_LF, length);

    /* Verify the SADB_MSG header */
    if (vars->sadb->msg->sadb_msg_version != PF_KEY_V2)
        keyadm_printf(vars, "keyadm: illegal sadb_msg_version (%d)"IP_LF, vars->sadb->msg->sadb_msg_version);
    else if (vars->sadb->msg->sadb_msg_type > SADB_MAX
             || vars->sadb->msg->sadb_msg_type == SADB_RESERVED)
        keyadm_printf(vars, "keyadm: illegal sadb_msg_type (%d)"IP_LF, vars->sadb->msg->sadb_msg_type);
    else if (vars->sadb->msg->sadb_msg_reserved != 0)
        keyadm_printf(vars, "keyadm: illegal sadb_msg_reserved (%d)"IP_LF, vars->sadb->msg->sadb_msg_reserved);
    else if ((int) (vars->sadb->msg->sadb_msg_len * sizeof(Ip_u64)) != length)
        keyadm_printf(vars, "keyadm: length mismatch (sadb_msg_len=%d, length=%d)"IP_LF,
                     vars->sadb->msg->sadb_msg_len * sizeof(Ip_u64), length);

    vars->msgbuf += sizeof(struct sadb_msg);
    length -= sizeof(struct sadb_msg);

    /* Verify the extensions */
    for (ext = (struct sadb_ext *)vars->msgbuf;
         length >= (int)sizeof(struct sadb_ext);
         ext = (struct sadb_ext *)vars->msgbuf)
    {
        int   ext_len;

        /* Extension headers must start at 64-bit alignment */
        if (((Ip_ptrdiff_t)ext & 0x7L) != 0)
            keyadm_printf(vars, "keyadm: bad alignment for extension %d"IP_LF, ext->sadb_ext_type);

        ext_len = ext->sadb_ext_len * sizeof(Ip_u64);
        if (ext_len == 0 || length < ext_len)
        {
            keyadm_printf(vars, "keyadm: illegal EXT length (%d)"IP_LF, ext_len);
            return -IP_ERRNO_EINVAL;
        }
        if (ext->sadb_ext_type == SADB_EXT_RESERVED)
        {
            keyadm_printf(vars, "keyadm: illegal EXT type (%d)"IP_LF, ext->sadb_ext_type);
            return -IP_ERRNO_EINVAL;
        }
        if (ext->sadb_ext_type > SADB_EXT_MAX)
        {
            keyadm_printf(vars, "keyadm: unknown EXT type (%d)"IP_LF, ext->sadb_ext_type);
            return -IP_ERRNO_EINVAL;
        }
        else
        {
            /*extmap |= (1 << ext->sadb_ext_type);*/

            /* [2.3] Only one instance of an extension header */
            if (vars->exthdr[ext->sadb_ext_type] != IP_NULL)
                keyadm_printf(vars, "keyadm: duplicate EXT type (%d)"IP_LF, ext->sadb_ext_type);
            else
                vars->exthdr[ext->sadb_ext_type] = ext;

            /* _Exact_ extension header size check */
            if (keyadm_ext_sizes[ext->sadb_ext_type] > 0 &&
               keyadm_ext_sizes[ext->sadb_ext_type] != ext_len)
                return -IP_ERRNO_EINVAL;

            /* _Minimum_ extension header size check */
            if (keyadm_ext_sizes[ext->sadb_ext_type] < 0 &&
               ext_len < -keyadm_ext_sizes[ext->sadb_ext_type])
                return -IP_ERRNO_EINVAL;

            /* next extension header */
            length -= ext_len;
            vars->msgbuf += ext_len;
        }
    }

    if (vars->sadb->msg->sadb_msg_errno != 0)
        (void)ipcom_errno_set(vars->sadb->msg->sadb_msg_errno);

    if (length != 0)
        keyadm_printf(vars, "keyadm: trailing SADB bytes (%d)"IP_LF, length);
    return 0;

 leave:
    return retval;
}


/*
 *===========================================================================
 *                    keyadm_print_command
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
static int
keyadm_print_command(Vars *vars)
{
    union Ip_sockaddr_union *sockaddr;
    union Ip_sockaddr_union *sockaddr_max;
    char str[128];
    int   addr_offset;
    Ip_time_t  addtime, usetime;
    Ip_u32 bytes;
    Ip_u16 min_port;
    Ip_u16 max_port;

    ip_assert(vars->sadb);
    ip_assert(vars->sadb->msg);

    keyadm_printf(vars, "SADB_MSG: msgtype=%s  err=%d  satype=%s  len=%d  seq=%d  pid=0x%x"IP_LF,
                  MSG_TYPE(vars->sadb->msg->sadb_msg_type),
                  vars->sadb->msg->sadb_msg_errno,
                  MSG_SATYPE(vars->sadb->msg->sadb_msg_satype),
                  vars->sadb->msg->sadb_msg_len * sizeof(Ip_u64),
                  vars->sadb->msg->sadb_msg_seq,
                  vars->sadb->msg->sadb_msg_pid);
    if (vars->sadb->msg->sadb_msg_errno)
    {
        keyadm_printf(vars, "   ERRNO: %s (%d)"IP_LF,
                      ipcom_strerror(vars->sadb->msg->sadb_msg_errno),
                      vars->sadb->msg->sadb_msg_errno);
    }

    /* SADB_EXT_SA */
    if (vars->sadb->sa)
    {
        keyadm_printf(vars, "      SA: spi=%x  sta=%s  auth=%s  enc=%s  replay=%d  %s"IP_LF,
                      ip_ntohl(vars->sadb->sa->sadb_sa_spi),
                      SA_STATE(vars->sadb->sa->sadb_sa_state),
                      SA_AUTH(vars->sadb->sa->sadb_sa_auth),
                      SA_ENCRYPT(vars->sadb->sa->sadb_sa_encrypt),
                      (int)vars->sadb->sa->sadb_sa_replay,
                      keyadm_cmd_bits_to_str((char **)keyadm_sa_flags_bitmap,
                                             vars->sadb->sa->sadb_sa_flags,
                                             str, sizeof(str)));
    }

    /* SADB_EXT_ADDRESS_SRC */
    if (vars->sadb->address_src)
    {
        sockaddr = (union Ip_sockaddr_union *)((Ip_u8 *)vars->sadb->address_src + sizeof(struct sadb_address));
        addr_offset = (sockaddr->sa.sa_family == IP_AF_INET) ? 2 : 6;

        keyadm_printf(vars, "     SRC: addr=%s",
                      ipcom_inet_ntop(sockaddr->sa.sa_family,
                                      &sockaddr->sa.sa_data[addr_offset],
                                      str, sizeof(str)));
        if (vars->sadb->address_src->sadb_address_proto != 0)
            keyadm_printf(vars, " proto=%s",
                          ipcom_ipproto_name(vars->sadb->address_src->sadb_address_proto));
        if (sockaddr->sa.sa_family == IP_AF_INET)
        {
            if (sockaddr->sin.sin_port)
                keyadm_printf(vars, " port=%d",
                              ip_ntohs(sockaddr->sin.sin_port));
        }
#ifdef IPCOM_USE_INET6
        if (sockaddr->sa.sa_family == IP_AF_INET6)
        {
            if (sockaddr->sin6.sin6_port)
                keyadm_printf(vars, " port=%d",
                              ip_ntohs(sockaddr->sin6.sin6_port));
        }
#endif
        keyadm_printf(vars, IP_LF);
    }

    /* SADB_EXT_ADDRESS_DST */
    if (vars->sadb->address_dst)
    {
        sockaddr = (union Ip_sockaddr_union *)((Ip_u8 *)vars->sadb->address_dst + sizeof(struct sadb_address));
        addr_offset = (sockaddr->sa.sa_family == IP_AF_INET) ? 2 : 6;
        if (sockaddr->sa.sa_family)
            keyadm_printf(vars, "     DST: addr=%s",
                          ipcom_inet_ntop(sockaddr->sa.sa_family, &sockaddr->sa.sa_data[addr_offset],
                                          str, sizeof(str)));
        if (vars->sadb->address_dst->sadb_address_proto != 0)
            keyadm_printf(vars, " proto=%s",
                          ipcom_ipproto_name(vars->sadb->address_dst->sadb_address_proto));
        if (sockaddr->sa.sa_family == IP_AF_INET)
        {
            if (sockaddr->sin.sin_port)
                keyadm_printf(vars, " port=%d",
                              ip_ntohs(sockaddr->sin.sin_port));

        }
#ifdef IPCOM_USE_INET6
        if (sockaddr->sa.sa_family == IP_AF_INET6)
        {
            if (sockaddr->sin6.sin6_port)
                keyadm_printf(vars, " port=%d",
                              ip_ntohs(sockaddr->sin6.sin6_port));
        }
#endif

        keyadm_printf(vars, IP_LF);
    }

    /* SADB_EXT_ADDRESS_PROXY */
    if (vars->sadb->address_proxy)
    {
        sockaddr = (union Ip_sockaddr_union *)((Ip_u8 *)vars->sadb->address_proxy + sizeof(struct sadb_address));
        addr_offset = (sockaddr->sa.sa_family == IP_AF_INET) ? 2 : 6;
        keyadm_printf(vars, "   PROXY: addr=%s"IP_LF,
                      ipcom_inet_ntop(sockaddr->sa.sa_family,
                                      &sockaddr->sa.sa_data[addr_offset],
                                      str, sizeof(str)));
    }

    /* SADB_X_EXT_PROTOCOL */
    if (vars->sadb->protocol)
    {
        char port0[16], port1[16];

        keyadm_printf(vars, "PROTOCOL: proto=%s",
                      ipcom_ipproto_name(vars->sadb->protocol->sadb_protocol_proto));

        if (vars->sadb->protocol->sadb_protocol_ports0)
            ipcom_sprintf(port0, "%d", vars->sadb->protocol->sadb_protocol_ports0);
        else
            ipcom_strcpy(port0, "any");
        if (vars->sadb->protocol->sadb_protocol_ports1)
            ipcom_sprintf(port1, "%d", vars->sadb->protocol->sadb_protocol_ports1);
        else
            ipcom_strcpy(port1, "any");
        switch (vars->sadb->protocol->sadb_protocol_proto)
        {
        case IP_IPPROTO_ICMP:
        case IP_IPPROTO_ICMPV6:
            keyadm_printf(vars, " type=%s code=%s", port0, port1);
            break;
        case IP_IPPROTO_MH:
            keyadm_printf(vars, " type=%s", port0);
            break;
        default:
            break;
        }
        keyadm_printf(vars, IP_LF);
    }

    /* SADB_X_EXT_SRC_FLOW */
    if (vars->sadb->src_flow)
    {
        sockaddr    = (union Ip_sockaddr_union *)((Ip_u8 *)vars->sadb->src_flow + sizeof(struct sadb_address));
        addr_offset = (sockaddr->sa.sa_family == IP_AF_INET) ? 2 : 6;
        min_port    = ip_ntohs(sockaddr->sin.sin_port);

        if (sockaddr->sa.sa_family)
            keyadm_printf(vars, " SRCFLOW: addr=%s",
                          ipcom_inet_ntop(sockaddr->sa.sa_family, &sockaddr->sa.sa_data[addr_offset],
                                          str, sizeof(str)));
        if (vars->sadb->srcmax_flow)
        {
            sockaddr_max = (union Ip_sockaddr_union *)((Ip_u8 *)vars->sadb->srcmax_flow + sizeof(struct sadb_address));
            if (sockaddr_max->sa.sa_family
                && ipcom_memcmp(&sockaddr->sa.sa_data[addr_offset], &sockaddr_max->sa.sa_data[addr_offset],
                                sockaddr_max->sa.sa_family == IP_AF_INET ? 4 : 16))
                keyadm_printf(vars, "-%s",
                              ipcom_inet_ntop(sockaddr_max->sa.sa_family, &sockaddr_max->sa.sa_data[addr_offset],
                                              str, sizeof(str)));
        }
        if (vars->sadb->src_mask)
        {
            sockaddr = (union Ip_sockaddr_union *)((Ip_u8 *)vars->sadb->src_mask + sizeof(struct sadb_address));
            addr_offset = (sockaddr->sa.sa_family == IP_AF_INET) ? 2 : 6;
            if (sockaddr->sa.sa_family)
                keyadm_printf(vars, "/%d",
                              ipipsec_addrmasklen(sockaddr->sa.sa_family, &sockaddr->sa.sa_data[addr_offset]));
        }
        if (vars->sadb->srcmax_flow)
        {
            sockaddr = (union Ip_sockaddr_union *)((Ip_u8 *)vars->sadb->srcmax_flow + sizeof(struct sadb_address));
            max_port = ip_ntohs(sockaddr->sin.sin_port);
        }
        else
            max_port = 0;
        if (min_port)
        {
            if (max_port)
                keyadm_printf(vars, " ports=%d-%d", min_port, max_port);
            else
                keyadm_printf(vars, " port=%d", min_port);
        }
        keyadm_printf(vars, IP_LF);
    }

    /* SADB_X_EXT_DST_FLOW */
    if (vars->sadb->dst_flow)
    {
        sockaddr = (union Ip_sockaddr_union *)((Ip_u8 *)vars->sadb->dst_flow + sizeof(struct sadb_address));
        addr_offset = (sockaddr->sa.sa_family == IP_AF_INET) ? 2 : 6;
        min_port = ip_ntohs(sockaddr->sin.sin_port);
        if (sockaddr->sa.sa_family)
            keyadm_printf(vars, " DSTFLOW: addr=%s",
                          ipcom_inet_ntop(sockaddr->sa.sa_family, &sockaddr->sa.sa_data[addr_offset],
                                          str, sizeof(str)));
        if (vars->sadb->dstmax_flow)
        {
            sockaddr_max = (union Ip_sockaddr_union *)((Ip_u8 *)vars->sadb->dstmax_flow + sizeof(struct sadb_address));
            if (sockaddr_max->sa.sa_family
                && ipcom_memcmp(&sockaddr->sa.sa_data[addr_offset], &sockaddr_max->sa.sa_data[addr_offset],
                                sockaddr_max->sa.sa_family == IP_AF_INET ? 4 : 16))
                keyadm_printf(vars, "-%s",
                              ipcom_inet_ntop(sockaddr_max->sa.sa_family, &sockaddr_max->sa.sa_data[addr_offset],
                                              str, sizeof(str)));
        }
        if (vars->sadb->dst_mask)
        {
            sockaddr = (union Ip_sockaddr_union *)((Ip_u8 *)vars->sadb->dst_mask + sizeof(struct sadb_address));
            addr_offset = (sockaddr->sa.sa_family == IP_AF_INET) ? 2 : 6;
            if (sockaddr->sa.sa_family)
                keyadm_printf(vars, "/%d",
                              ipipsec_addrmasklen(sockaddr->sa.sa_family, &sockaddr->sa.sa_data[addr_offset]));
        }
        if (vars->sadb->dstmax_flow)
        {
            sockaddr = (union Ip_sockaddr_union *)((Ip_u8 *)vars->sadb->dstmax_flow + sizeof(struct sadb_address));
            max_port = ip_ntohs(sockaddr->sin.sin_port);
        }
        else
            max_port = 0;
        if (min_port)
        {
            if (max_port)
                keyadm_printf(vars, " ports=%d-%d", min_port, max_port);
            else
                keyadm_printf(vars, " port=%d", min_port);
        }
        keyadm_printf(vars, IP_LF);
    }

    /* SADB_X_EXT_FLOW_OPTIONS */
    if (vars->sadb->flowopt)
    {
        keyadm_printf(vars, " FLOWOPT: type=%s prio=%d dscp=0x%x dir=%s id=%d"IP_LF,
                      FLOW_TYPE(vars->sadb->flowopt->sadb_flowopt_type),
                      (int)vars->sadb->flowopt->sadb_flowopt_priority,
                      vars->sadb->flowopt->sadb_flowopt_dscp,
                      vars->sadb->flowopt->sadb_flowopt_direction == 1 ? "in" :
                      vars->sadb->flowopt->sadb_flowopt_direction == 2 ? "out" : "?",
                      (int)vars->sadb->flowopt->sadb_flowopt_id);
    }

    /* SADB_X_EXT_SA_OPTIONS */
    if (vars->sadb->saopt)
    {
        keyadm_printf(vars, "   SAOPT: priority=%d"IP_LF,
                      vars->sadb->saopt->sadb_saopt_priority);
    }

    /* SADB_EXT_SPIRANGE */
    if (vars->sadb->spirange)
    {
        keyadm_printf(vars, "SPIRANGE: min=%d  max=%d"IP_LF,
                      vars->sadb->spirange->sadb_spirange_min,
                      vars->sadb->spirange->sadb_spirange_max);
    }

    /* SADB_EXT_LIFETIME_CURRENT */
    if (vars->sadb->lifetime_current)
    {
        addtime = (Ip_time_t)IP_U64_GETLO(vars->sadb->lifetime_current->sadb_lifetime_addtime);
        usetime = (Ip_time_t)IP_U64_GETLO(vars->sadb->lifetime_current->sadb_lifetime_usetime);
        bytes   = IP_U64_GETLO(vars->sadb->lifetime_current->sadb_lifetime_bytes);
        keyadm_printf(vars, " CURRENT: addtime=%ld usetime=%ld bytes=%ld"IP_LF,
                      addtime, usetime, bytes);
    }

    /* SADB_EXT_LIFETIME_SOFT */
    if (vars->sadb->lifetime_soft)
    {
        addtime = (Ip_time_t)IP_U64_GETLO(vars->sadb->lifetime_soft->sadb_lifetime_addtime);
        usetime = (Ip_time_t)IP_U64_GETLO(vars->sadb->lifetime_soft->sadb_lifetime_usetime);
        bytes   = IP_U64_GETLO(vars->sadb->lifetime_soft->sadb_lifetime_bytes);
        keyadm_printf(vars, "    SOFT: addtime=%ld usetime=%ld bytes=%ld"IP_LF,
                      addtime, usetime, bytes);
    }

    /* SADB_EXT_LIFETIME_HARD */
    if (vars->sadb->lifetime_hard)
    {
        addtime = (Ip_time_t)IP_U64_GETLO(vars->sadb->lifetime_hard->sadb_lifetime_addtime);
        usetime = (Ip_time_t)IP_U64_GETLO(vars->sadb->lifetime_hard->sadb_lifetime_usetime);
        bytes   = IP_U64_GETLO(vars->sadb->lifetime_hard->sadb_lifetime_bytes);
        keyadm_printf(vars, "    HARD: addtime=%ld usetime=%ld bytes=%ld"IP_LF,
                      addtime, usetime, bytes);
    }

    /* SADB_X_SAFLAGS_UDPENCAP */
    if (vars->sadb->udpencap)
        keyadm_printf(vars, "UDPENCAP: port=%d"IP_LF, ip_ntohs(vars->sadb->udpencap->sadb_x_udpencap_port));

    keyadm_printf(vars, "------------------------------------------"IP_LF);

    return vars->sadb->msg->sadb_msg_errno == 0 ? 0 : -1;
}


/*
 *===========================================================================
 *                    keyadm_get_usage
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
static void
keyadm_get_usage(Vars *vars)
{
    keyadm_printf(vars,
                  "Get (info about) a Security Association (SA)"IP_LF
                  "       usage: keyadm get <selectors>"IP_LF);
    keyadm_printf(vars,
                  "   mandatory selectors: "IP_LF
                  "       -spi <val>        SPI (in hex without the 0x)"IP_LF
                  "       -src <ip>         source IP address"IP_LF
                  "       -dst <ip>         destination IP address"IP_LF
                  "       -proto <name>     security protocol <esp|ah|ipip>"IP_LF
                  );
    keyadm_printf(vars,
                  "   optional selectors: "IP_LF
                  "       -sapri <val>      priority (-128=highest to 127=lowest)"IP_LF
                  "       -egress           output SA (default)"IP_LF
                  "       -ingress          input SA"IP_LF
                  );
}


/*
 *===========================================================================
 *                    keyadm_get
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
static int
keyadm_get(Vars *vars)
{
    if (vars->have_satype == 0)
    {
        keyadm_printf(vars, "keyadm: missing modifier '-proto'"IP_LF);
        return -1;
    }

    if (keyadm_send_command(vars, SADB_GET, vars->satype) == 0)
    {
        (void)keyadm_receive_command(vars, 0);
        return keyadm_print_command(vars);
    }

    return -1;
}


/*
 *===========================================================================
 *                    keyadm_dump
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
static int
keyadm_dump(Vars *vars)
{
    if (keyadm_send_command(vars, SADB_DUMP, vars->satype) == 0)
    {
        struct Ip_timeval tv;
        int count = 0;

        tv.tv_sec  = 3;
        tv.tv_usec = 0;

        if (ipcom_setsockopt(vars->fd, IP_SOL_SOCKET, IP_SO_RCVTIMEO, &tv, sizeof(tv)) < 0)
        {
            keyadm_printf(vars, "keyadm: setsockopt(IP_SO_RCVTIMEO,x) failed : %s ",
                          ipcom_strerror(ipcom_errno));
            return 0;
        }

        do
        {
            if (keyadm_receive_command(vars, 0) != 0)
                break;
            if (keyadm_print_command(vars) == 0)
                count++;

        }
        while (vars->sadb->msg->sadb_msg_errno == 0
               && vars->sadb->msg->sadb_msg_seq);

        keyadm_printf(vars, "Total of %d security associations"IP_LF, count);
    }

    return 0;
}


/*
 *===========================================================================
 *                    keyadm_flush
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
static int
keyadm_flush(Vars *vars)
{
    if (keyadm_send_command(vars, SADB_FLUSH, vars->satype) == 0)
    {
        (void)keyadm_receive_command(vars, 0);
        return keyadm_print_command(vars);
    }

    return -1;
}


/*
 *===========================================================================
 *                    keyadm_getspi_usage
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
static void
keyadm_getspi_usage(Vars *vars)
{
    keyadm_printf(vars,
                  "Create a larval Security Association (SA) with a (random) SPI"IP_LF
                  "       usage: keyadm getspi <selectors> <modifiers>"IP_LF);
    keyadm_printf(vars,
                  "   mandatory selectors: "IP_LF
                  "       -src <ip>         source IP address"IP_LF
                  "       -dst <ip>         destination IP address"IP_LF
                  "       -proto <name>     security protocol <esp|ah|ipip>"IP_LF
                  );
    keyadm_printf(vars,
                  "   optional selectors: "IP_LF
                  "       -sapri <val>      priority (-128=highest to 127=lowest)"IP_LF
                  "       -egress           output SA (default)"IP_LF
                  "       -ingress          input SA"IP_LF
                  );
                  keyadm_printf(vars,
                  "   mandatory modifiers:"IP_LF
                  "       -minspi <val>     min SPI value (in hex)"IP_LF
                  "       -maxspi <val>     max SPI value (in hex)"IP_LF
                  );
}


/*
 *===========================================================================
 *                    keyadm_getspi
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
static int
keyadm_getspi(Vars *vars)
{
    if (vars->have_satype == 0)
    {
        keyadm_printf(vars, "keyadm: missing modifier '-proto'"IP_LF);
        return -1;
    }

    if (keyadm_send_command(vars, SADB_GETSPI, vars->satype) == 0)
    {
        (void)keyadm_receive_command(vars, 0);
        return keyadm_print_command(vars);
    }

    return -1;
}


/*
 *===========================================================================
 *                         keyadm_sacheck
 *===========================================================================
 */
static int
keyadm_sacheck(Vars *vars, Ip_bool update)
{
    if (vars->test)
        return 0;

    if (vars->have_satype == 0)
    {
        keyadm_printf(vars, "keyadm: missing modifier '-proto'"IP_LF);
        return -1;
    }

    if (update == IP_FALSE)
    {
        if (vars->satype == SADB_SATYPE_ESP)
        {
            if (vars->encrypt_type == -1)
            {
                keyadm_printf(vars, "keyadm: missing modifier '-enc'"IP_LF);
                return -1;
            }
            if (vars->encrypt_type > 0 && vars->encrypt_len == 0)
            {
                keyadm_printf(vars, "keyadm: missing modifier '-ekey'"IP_LF);
                return -1;
            }
        }

        if (vars->satype == SADB_SATYPE_ESP || vars->satype == SADB_SATYPE_AH)
        {
            if (vars->auth_type == -1)
            {
                keyadm_printf(vars, "keyadm: missing modifier '-auth'"IP_LF);
                return -1;
            }
            if (vars->auth_type > 0 && vars->auth_len == 0)
            {
                keyadm_printf(vars, "keyadm: missing modifier '-akey'"IP_LF);
                return -1;
            }
        }
    }

    return 0;
}


/*
 *===========================================================================
 *                    keyadm_update_usage
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
static void
keyadm_update_usage(Vars *vars)
{
    keyadm_printf(vars,
                  "Update a Security Association (SA)"IP_LF
                  "       usage: keyadm update <selectors> <modifiers>"IP_LF);
    keyadm_printf(vars,
                  "   mandatory selectors: "IP_LF
                  "       -spi <val>        SPI (in hex without the 0x)"IP_LF
                  "       -src <ip>         source IP address"IP_LF
                  "       -dst <ip>         destination IP address"IP_LF
                  "       -proto <name>     security protocol <esp|ah|ipip>"IP_LF
                  );
    keyadm_printf(vars,
                  "   optional selectors: "IP_LF
                  "       -sapri <val>      priority (-128=highest to 127=lowest)"IP_LF
                  "       -egress           output SA (default)"IP_LF
                  "       -ingress          input SA"IP_LF
                  );
    keyadm_printf(vars,
                  "   mandatory modifiers:"IP_LF
                  "       -enc <alg>        encryption algorithm <des|3des|blf|cast|aes|aesctr>"IP_LF
                  "       -ekey <key>       encryption key (in hex, each char is 4 bits)"IP_LF
                  "       -auth <alg>       authentication algorithm <md5|sha1|rmd160|aesmac>"IP_LF
                  "       -akey <key>       authentication key (in hex, each char is 4 bits)"IP_LF
                  );
    keyadm_printf(vars,
                  "   optional modifiers:"IP_LF
                  "       -proxy <ip>       proxy IP address (for tunneling)"IP_LF
                  "       -replay <val>     replay counter (0-255, 0=disabled)"IP_LF
                  "       -forcetunnel      force tunneling mode"IP_LF
                  "       -esn              enable Extended Sequence Numbers (ESN)"IP_LF
                  "       -srcreq           source IP must match in SA lookup"IP_LF
                  );
    keyadm_printf(vars,
                  "       -tfc              enable TFC (ESP only)"IP_LF
                  "       -udpencap <port>  enable ESP UDP encap using <port> (ESP only)"IP_LF
                  "       -clrdf            clear DF-bit on outer IPv4 header"IP_LF
                  "       -setdf            set DF-bit on outer IPv4 header"IP_LF
                  "       -old              use old AH or ESP (deprecated)"IP_LF
                  "       -halfiv           use 4-byte IV in old ESP (deprecated)"IP_LF
                  );
    keyadm_printf(vars,
                  "       -ls <sec>         soft lifetime in seconds"IP_LF
                  "       -lh <sec>         hard lifetime in seconds"IP_LF
                  );

}


/*
 *===========================================================================
 *                    keyadm_update
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
static int
keyadm_update(Vars *vars)
{
    if (keyadm_sacheck(vars, IP_TRUE) != 0)
        return -1;

    if (keyadm_send_command(vars, SADB_UPDATE, vars->satype) == 0)
    {
        (void)keyadm_receive_command(vars, 0);
        return keyadm_print_command(vars);
    }

    return -1;
}


/*
 *===========================================================================
 *                    keyadm_add_usage
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
static void
keyadm_add_usage(Vars *vars)
{
    keyadm_printf(vars,
                  "Add a Security Association (SA)"IP_LF
                  "       usage: keyadm add <selectors> <modifiers>"IP_LF);
    keyadm_printf(vars,
                  "   mandatory selectors: "IP_LF
                  "       -spi <val>        SPI (in hex without the 0x)"IP_LF
                  "       -src <ip>         source IP address"IP_LF
                  "       -dst <ip>         destination IP address"IP_LF
                  "       -proto <name>     security protocol <esp|ah|ipip>"IP_LF
                  );
    keyadm_printf(vars,
                  "   optional selectors: "IP_LF
                  "       -sapri <val>      priority (-128=highest to 127=lowest)"IP_LF
                  "       -egress           output SA (default)"IP_LF
                  "       -ingress          input SA"IP_LF
                  );
    keyadm_printf(vars,
                  "   mandatory modifiers:"IP_LF
                  "       -enc <alg>        encryption algorithm <des|3des|blf|cast|aes|aesctr>"IP_LF
                  "       -ekey <key>       encryption key (in hex, each char is 4 bits)"IP_LF
                  "       -auth <alg>       authentication algorithm <md5|sha1|rmd160|aesmac>"IP_LF
                  "       -akey <key>       authentication key (in hex, each char is 4 bits)"IP_LF
                  );
    keyadm_printf(vars,
                  "   optional modifiers:"IP_LF
                  "       -proxy <ip>       proxy IP address (for tunneling)"IP_LF
                  "       -replay <val>     replay counter (0-255, 0=disabled)"IP_LF
                  );
    keyadm_printf(vars,
                  "       -forcetunnel      force tunneling mode"IP_LF
                  "       -ls <sec>         soft lifetime in seconds"IP_LF
                  "       -lh <sec>         hard lifetime in seconds"IP_LF
                  "       -esn              enable Extended Sequence Numbers (ESN)"IP_LF
                  );
    keyadm_printf(vars,
                  "       -srcreq           source IP must match in SA lookup"IP_LF
                  "       -tfc              enable TFC (ESP only)"IP_LF
                  "       -clrdf            clear DF-bit on outer IPv4 header"IP_LF
                  "       -setdf            set DF-bit on outer IPv4 header"IP_LF
                  "       -old              use old AH or ESP (deprecated)"IP_LF
                  "       -halfiv           use 4-byte IV in old ESP (deprecated)"IP_LF
                  );
}


/*
 *===========================================================================
 *                    keyadm_add
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
static int
keyadm_add(Vars *vars)
{
    if (keyadm_sacheck(vars, IP_FALSE) != 0)
        return -1;

    if (keyadm_send_command(vars, SADB_ADD, vars->satype) == 0)
    {
        (void)keyadm_receive_command(vars, 0);
        return keyadm_print_command(vars);
    }

    return -1;
}


/*
 *===========================================================================
 *                    keyadm_delete_usage
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
static void
keyadm_delete_usage(Vars *vars)
{
    keyadm_printf(vars,
                  "Delete a Security Association (SA)"IP_LF
                  "       usage: keyadm delete <selectors | -id <id>> [-chain]"IP_LF);
    keyadm_printf(vars,
                  "   mandatory selectors: "IP_LF
                  "       -spi <val>        SPI (in hex without the 0x)"IP_LF
                  "       -src <ip>         source IP address"IP_LF
                  "       -dst <ip>         destination IP address"IP_LF
                  "       -proto <name>     security protocol <esp|ah|ipip>"IP_LF
                  );
    keyadm_printf(vars,
                  "   optional selectors: "IP_LF
                  "       -sapri <val>      priority (-128=highest to 127=lowest)"IP_LF
                  "       -egress           output SA (default)"IP_LF
                  "       -ingress          input SA"IP_LF
                  );

    keyadm_printf(vars,
                  "   optional modifier:"IP_LF
                  "       -chain            used to remove the whole chain"IP_LF
                  );
}


/*
 *===========================================================================
 *                    keyadm_delete
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
static int
keyadm_delete(Vars *vars)
{
    if (vars->test == 0 && vars->have_satype == 0)
    {
        keyadm_printf(vars, "keyadm: missing modifier '-proto'"IP_LF);
        return -1;
    }

    if (keyadm_send_command(vars, SADB_DELETE, vars->satype) == 0)
    {
        (void)keyadm_receive_command(vars, 0);
        return keyadm_print_command(vars);
    }

    return -1;
}


/*
 *===========================================================================
 *                    keyadm_addflow_usage
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
static void
keyadm_addflow_usage(Vars *vars)
{
    keyadm_printf(vars,
                  "Add a Flow (Security Policy)"IP_LF
                  "       usage: keyadm addflow <selectors> <type> [SA selectors] [-replace]"IP_LF
                  );
    keyadm_printf(vars,
                  "   mandatory selectors:"IP_LF
                  "       -srcflow <ip>        source address"IP_LF
                  "       -dstflow <ip>        destination address"IP_LF
                  );
    keyadm_printf(vars,
                  "   optional selectors:"IP_LF
                  "       -srcmask <mask>      source mask (default all 1's)"IP_LF
                  "       -dstmask <mask>      destination mask (default all 1's)"IP_LF
                  "       -egress              output flow (default)"IP_LF
                  "       -ingress             input flow"IP_LF
                  );
    keyadm_printf(vars,
                  "       -srcmaxflow <ip>     source max address (for linear range)"IP_LF
                  "       -dstmaxflow <ip>     destination max address (for linear range)"IP_LF
                  "       -flowpri <prio>      flow priority (-128 to +127, -128=highest)"IP_LF
                  "       -dscp <val>          DSCP value (in hex without the 0x)"IP_LF
                  );
    keyadm_printf(vars,
                  "   optional IP protocol specific selectors:"IP_LF
                  "       -transport <val>     IP protocol (ANY=0, TCP=6, UDP=17 etc)"IP_LF
                  "       -srcport <port>      UDP/TCP source port"IP_LF
                  "       -dstport <port>      UDP/TCP destination port"IP_LF
                  "       -srcmaxport <port>   UDP/TCP source max port (for linear range)"IP_LF
                  "       -dstmaxport <port>   UDP/TCP destination port (for linear range)"IP_LF
                  );
    keyadm_printf(vars,
                  "       -icmptype <val>      ICMP type (specified as value)"IP_LF
                  "       -icmpcode <val>      ICMP code (specified as value)"IP_LF
                  "       -mhtype <val>        MH type (specified as value)"IP_LF
                  );
    keyadm_printf(vars,
                  "   optional type:"IP_LF
                  "       -use                 use flow (default)"IP_LF
                  "       -bypass              bypass flow"IP_LF
                  "       -deny                deny flow"IP_LF
                  "       -acquire             acquire flow"IP_LF
                  "       -require             require flow"IP_LF
                  );
    keyadm_printf(vars,
                  "   optional SA selectors (for '-use' flows): "IP_LF
                  "       -spi <val>           SPI (in hex without the 0x)"IP_LF
                  "       -src <ip>            source IP address"IP_LF
                  "       -dst <ip>            destination IP address"IP_LF
                  "       -proto <name>        security protocol <esp|ah|ipip>"IP_LF
                  "       -sapri <val>         priority (-128=highest to 127=lowest)"IP_LF
                  );
    keyadm_printf(vars,
                  "   optional modifier: "IP_LF
                  "       -replace             replace flow if duplicate"IP_LF
                  );
}


/*
 *===========================================================================
 *                    keyadm_addflow
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
static int
keyadm_addflow(Vars *vars)
{
    if (keyadm_send_command(vars, SADB_X_ADDFLOW, vars->satype) == 0)
    {
        (void)keyadm_receive_command(vars, 0);
        return keyadm_print_command(vars);
    }

    return -1;
}


/*
 *===========================================================================
 *                    keyadm_delflow_usage
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
static void
keyadm_delflow_usage(Vars *vars)
{
    keyadm_printf(vars,
                  "Delete a Flow (Security Policy)"IP_LF
                  "       usage: keyadm delflow <selectors> <type>"IP_LF
                  "              keyadm delflow -id <flowid>"IP_LF
                  );
    keyadm_printf(vars,
                  "   mandatory selectors:"IP_LF
                  "       -srcflow <ip>        source flow address"IP_LF
                  "       -srcmask <mask>      source flow mask"IP_LF
                  "       -dstflow <ip>        destination flow address"IP_LF
                  "       -dstmask <mask>      destination flow mask"IP_LF
                  );
    keyadm_printf(vars,
                  "   optional selectors:"IP_LF
                  "       -egress              output flow (default)"IP_LF
                  "       -ingress             input flow"IP_LF
                  "       -srcmaxflow <ip>     source flow max address (for linear range)"IP_LF
                  "       -dstmaxflow <ip>     destination flow max address (for linear range)"IP_LF
                  "       -flowpri <prio>      flow priority (-128 to +127, -128=highest)"IP_LF
                  "       -dscp <val>          DSCP value (in hex without the 0x)"IP_LF
                  );
    keyadm_printf(vars,
                  "   optional IP protocol specific selectors:"IP_LF
                  "       -transport <val>     IP protocol (ANY=0, TCP=6, UDP=17 etc)"IP_LF
                  "       -srcport <port>      UDP/TCP source port"IP_LF
                  "       -dstport <port>      UDP/TCP destination port"IP_LF
                  "       -srcmaxport <port>   UDP/TCP source max port (for linear range)"IP_LF
                  "       -dstmaxport <port>   UDP/TCP destination port (for linear range)"IP_LF
                  );
    keyadm_printf(vars,
                  "       -icmptype <val>      ICMP type (specified as value)"IP_LF
                  "       -icmpcode <val>      ICMP code (specified as value)"IP_LF
                  "       -mhtype <val>        MH type (specified as value)"IP_LF
                  );
    keyadm_printf(vars,
                  "   optional type:"IP_LF
                  "       -use                 use flow (default)"IP_LF
                  "       -bypass              bypass flow"IP_LF
                  "       -deny                deny flow"IP_LF
                  "       -acquire             acquire flow"IP_LF
                  "       -require             require flow"IP_LF
                  );
}


/*
 *===========================================================================
 *                    keyadm_delflow
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
static int
keyadm_delflow(Vars *vars)
{
    if (!(vars->flow_id != 0
          || ((vars->extmap & SEBIT_X_FLOW) == SEBIT_X_FLOW)))
    {
        keyadm_printf(vars, "keyadm: missing modifier(s) '-id' or '-srcflow and -dstflow'"IP_LF);
        return -1;
    }

    if (keyadm_send_command(vars, SADB_X_DELFLOW, vars->satype) == 0)
    {
        (void)keyadm_receive_command(vars, 0);
        return keyadm_print_command(vars);
    }

    return -1;
}


/*
 *===========================================================================
 *                    keyadm_findflow_usage
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
static void
keyadm_findflow_usage(Vars *vars)
{
    keyadm_printf(vars,
                  "Lookup a Flow (Security Policy)"IP_LF
                  "       usage: keyadm findflow <selectors>"IP_LF
                  );
    keyadm_printf(vars,
                  "   mandatory selectors:"IP_LF
                  "       -srcflow <ip>        source address"IP_LF
                  "       -dstflow <ip>        destination address"IP_LF
                  "       -transport <val>     IP protocol (ANY=0, TCP=6, UDP=17 etc)"IP_LF
                  );
    keyadm_printf(vars,
                  "   optional selectors:"IP_LF
                  "       -egress              output packet (default)"IP_LF
                  "       -ingress             input packet "IP_LF
                  "       -dscp <val>          DSCP value (in hex without the 0x)"IP_LF
                  "       -srcport <port>      UDP/TCP source port"IP_LF
                  "       -dstport <port>      UDP/TCP destination port"IP_LF
                  );
    keyadm_printf(vars,
                  "       -icmptype <val>      ICMP type (specified as value)"IP_LF
                  "       -icmpcode <val>      ICMP code (specified as value)"IP_LF
                  "       -mhtype <val>        MH type (specified as value)"IP_LF
                  );
}


/*
 *===========================================================================
 *                    keyadm_findflow
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
static int
keyadm_findflow(Vars *vars)
{
    if (vars->have_transport_proto == 0)
    {
        keyadm_printf(vars, "keyadm: missing modifier '-transport'"IP_LF);
        return -1;
    }

    if (keyadm_send_command(vars, SADB_X_FINDFLOW, vars->satype) == 0)
    {
        (void)keyadm_receive_command(vars, 0);
        return keyadm_print_command(vars);
    }

    return -1;
}


/*
 *===========================================================================
 *                    keyadm_dumpflow
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
static int
keyadm_dumpflow(Vars *vars)
{
    if (keyadm_send_command(vars, SADB_X_DUMPFLOW, vars->satype) == 0)
    {
        struct Ip_timeval tv;
        int count = 0;

        tv.tv_sec  = 3;
        tv.tv_usec = 0;

        if (ipcom_setsockopt(vars->fd, IP_SOL_SOCKET, IP_SO_RCVTIMEO, &tv, sizeof(tv)) < 0)
        {
            keyadm_printf(vars, "keyadm: setsockopt(IP_SO_RCVTIMEO,x) failed : %s ",
                          ipcom_strerror(ipcom_errno));
            return 0;
        }

        do
        {
            if (keyadm_receive_command(vars, 0) != 0)
                break;
            if (keyadm_print_command(vars) == 0)
                count++;
        }
        while (vars->sadb->msg->sadb_msg_errno == 0
               && vars->sadb->msg->sadb_msg_seq);

        keyadm_printf(vars, "Total of %d flows"IP_LF, count);
    }

    return 0;
}


/*
 *===========================================================================
 *                    keyadm_grpspis_usage
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
static void
keyadm_grpspis_usage(Vars *vars)
{
    keyadm_printf(vars,
                  "Group two Security Associations (SAs)"IP_LF
                  "       usage: keyadm group <SA1 selectors> <SA2 selectors>"IP_LF);
    keyadm_printf(vars,
                  "   1st SA mandatory selectors: "IP_LF
                  "       -spi <val>        SPI (in hex without the 0x)"IP_LF
                  "       -src <ip>         source IP address"IP_LF
                  "       -dst <ip>         destination IP address"IP_LF
                  "       -proto <name>     security protocol <esp|ah|ipip>"IP_LF
                  );
    keyadm_printf(vars,
                  "   1st SA optional selectors: "IP_LF
                  "       -sapri <val>      priority (-128=highest to 127=lowest)"IP_LF
                  "       -egress           output SA (default)"IP_LF
                  "       -ingress          input SA"IP_LF
                  );
    keyadm_printf(vars,
                  "   2nd SA mandatory selectors: "IP_LF
                  "       -spi2 <val>       SPI (in hex without the 0x)"IP_LF
                  "       -dst2 <ip>        destination IP address"IP_LF
                  "       -proto2 <name>    security protocol <esp|ah|ipip>"IP_LF
                  "                         NOTE: SA2 is always outbound"IP_LF
                  );
}


/*
 *===========================================================================
 *                    keyadm_grpspis
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
static int
keyadm_grpspis(Vars *vars)
{
    if (vars->test == 0)
    {
        if (vars->have_satype == 0)
        {
            keyadm_printf(vars, "keyadm: missing modifier '-proto'"IP_LF);
            return -1;
        }
        if (IP_BIT_ISFALSE(vars->extmap, SEBIT_X_DST2))
        {
            keyadm_printf(vars, "keyadm: missing modifier '-dst2'"IP_LF);
            return -1;
        }
    }

    if (keyadm_send_command(vars, SADB_X_GRPSPIS, vars->satype) == 0)
    {
        (void)keyadm_receive_command(vars, 0);
        return keyadm_print_command(vars);
    }

    return -1;
}


/*
 *===========================================================================
 *                    keyadm_register
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
#ifdef IP_DEBUG
static int
keyadm_register(Vars *vars)
{
    if (vars->have_satype == 0)
    {
        keyadm_printf(vars, "keyadm: missing modifier '-proto'"IP_LF);
        return -1;
    }

    if (keyadm_send_command(vars, SADB_REGISTER, vars->satype) == 0)
    {
        (void)keyadm_receive_command(vars, 0);
        return keyadm_print_command(vars);
    }

    return -1;
}
#endif  /* IP_DEBUG */


/*
 *===========================================================================
 *                    keyadm_monitor
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
static int
keyadm_monitor(Vars *vars)
{
    int i;

    if (vars->count == 0)
    {
        keyadm_printf(vars,
                      "Monitor PFKey kernel messages"IP_LF
                      "       usage: keyadm monitor [-c <count>] [-W <timeout>]"IP_LF
                      "   options: "IP_LF
                      "       -c <count>        maximum number of packets received"IP_LF
                      "       -W <timeout>      maximum timeout (in seconds) per read"IP_LF
                      "       -proto <name>     security protocol <esp|ah|ipip>"IP_LF
                      );
        return 0;
    }

    if (keyadm_send_command(vars, SADB_REGISTER, vars->satype) == 0)
    {
        if (keyadm_receive_command(vars, 0) != 0)
        {
            keyadm_printf(vars, "keyadm: failed to register"IP_LF);
            return -1;
        }
    }

    ipcom_printf(IP_LF"Starting MONITOR loop [satype=%s]"IP_LF,
                 MSG_SATYPE(vars->satype));

    for (i = 0; i < vars->count; i++)
    {
        if (keyadm_receive_command(vars, vars->timeout_seconds) != 0)
            break;
        (void)keyadm_print_command(vars);
    }

    return 0;
}


/*
 *===========================================================================
 *                         keyadm_run_command
 *===========================================================================
 */
static int
keyadm_run_command(int argc, char *name, Vars *vars)
{
    int   i;
    Ip_u32   ext;

    for (i = 0; commands[i].name; i++)
        if (ipcom_strcmp(name, commands[i].name) == 0)
        {
            if (argc == 2 && commands[i].usage != IP_NULL)
            {
                commands[i].usage(vars);
                return 0;
            }
            if (vars->test == 0 && commands[i].type > 0)
            {
                for (ext = 1; ext < SADB_EXT_MAX; ext++)
                    if (IP_BIT_ISSET(extbits[commands[i].type].in_req, (1 << ext))
                        && IP_BIT_ISFALSE(vars->extmap, (1 << ext)))
                    {
                        keyadm_printf(vars, "keyadm: missing modifier '%s'"IP_LF, EXT_TYPE(ext));
                        return -1;
                    }
                if (commands[i].type == SADB_GETSPI && vars->maxspi == 0)
                {
                    keyadm_printf(vars, "keyadm: missing modifier '-maxspi'"IP_LF);
                    return -1;
                }
            }
            return commands[i].func(vars);
        }

    keyadm_printf(vars, "keyadm: unknown command"IP_LF);
    return -1;
}


/*
 *===========================================================================
 *                    ipipsec_cmd_keyadm_private
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipipsec_cmd_keyadm_private(Vars *vars, int argc, char **argv)
{
    int  i, c, doexit;
    int  ret = 1;
    Ipcom_getopt  opt;

    static struct Ip_option long_options[] =
        {
            {"spi", 1, 0, 0},
            {"src", 1, 0, 0},
            {"dst", 1, 0, 0},
            {"proto", 1, 0, 0},
            {"minspi", 1, 0, 0},
            {"maxspi", 1, 0, 0},
            {"ekey", 1, 0, 0},
            {"akey", 1, 0, 0},
            {"enc", 1, 0, 0},
            {"auth", 1, 0, 0},
            {"old", 0, 0, 0},
            {"halfiv", 0, 0, 0},
            {"forcetunnel", 0, 0, 0},
            {"srcmask", 1, 0, 0},
            {"dstmask", 1, 0, 0},
            {"srcflow", 1, 0, 0},
            {"dstflow", 1, 0, 0},
            {"spi2", 1, 0, 0},
            {"dst2", 1, 0, 0},
            {"proto2", 1, 0, 0},
            {"new", 0, 0, 0},
            {"ingress", 0, 0, 0},
            {"srcport", 1, 0, 0},
            {"dstport", 1, 0, 0},
            {"transport", 1, 0, 0},
            {"replace", 0, 0, 0},
            {"bypass", 0, 0, 0},
            {"replay", 1, 0, 0},
            {"chain", 0, 0, 0},
            {"proxy", 1, 0, 0},
            {"egress", 0, 0, 0},
            {"silent", 0, 0, 0},
            {"ls", 1, 0, 0},
            {"lh", 1, 0, 0},
            {"deny", 0, 0, 0},
            {"acquire", 0, 0, 0},
            {"require", 0, 0, 0},
            {"b", 1, 0, 0},
            {"srcmaxport", 1, 0, 0},
            {"dstmaxport", 1, 0, 0},
            {"tfc", 0, 0, 0},
            {"esn", 0, 0, 0},
            {"flowpri", 1, 0, 0},
            {"c", 1, 0, 0},
            {"id", 1, 0, 0},
            {"srcreq", 0, 0, 0},
            {"icmptype", 1, 0, 0},
            {"icmpcode", 1, 0, 0},
            {"sapri", 1, 0, 0},
            {"use", 1, 0, 0},
            {"W", 1, 0, 0},
            {"mhtype", 1, 0, 0},
            {"srcmaxflow", 1, 0, 0},
            {"dstmaxflow", 1, 0, 0},
            {"setdf", 0, 0, 0},
            {"clrdf", 0, 0, 0},
            {"dscp", 1, 0, 0},
            {"udpencap", 1, 0, 0},
            {0, 0, 0, 0}
        };

    /* prepare data */
    ipcom_memset(vars, 0, sizeof(Vars));
    vars->sadb         = (Keyadm_sadb *)vars->exthdr;
    vars->sadb->msg    = &vars->sadbmsg;
    vars->encrypt_type = -1;
    vars->auth_type    = -1;
    vars->argc         = argc;
    vars->argv         = argv;
    vars->fd           = -1;

    ipcom_getopt_clear_r(&opt);

    /* Check -silent flag first, to disable all prints */
    for (i = 0; i < argc; i++)
        if (ipcom_strstr(argv[i], "-silent"))
        {
            vars->silent = 1;
            opt.opterr   = 0;
            break;
        }
    if (vars->silent == 0)
    {
        vars->stdoutbuf = ipcom_malloc(512);
        if (vars->stdoutbuf == IP_NULL)
            return 1;
    }

    if (argc < 2)
        goto usage;

    vars->fd = ipcom_socket(IP_PF_KEY, IP_SOCK_RAW, PF_KEY_V2);
    if (vars->fd == IP_SOCKERR)
    {
        keyadm_printf(vars, "keyadm: ipcom_socket() failed (%s)"IP_LF, ipcom_strerror(ipcom_errno));
        goto leave2;
    }

    opt.long_only = 1;
    opt.long_options = long_options;

    while ((c = ipcom_getopt_r(argc - 1, argv + 1, "t", &opt)) != -1)
    {
        switch (c)
        {
        case 0:
            switch(opt.long_index)
            {
            case 0: /* -spi */
                vars->spi_n = ipcom_strtoul(opt.optarg, IP_NULL, 16);
                vars->spi_n = ip_htonl(vars->spi_n);
                IP_BIT_SET(vars->extmap, SEBIT_SA);
                break;
            case 1: /* -src */
                if (keyadm_read_addr(opt.optarg, &vars->srcaddr) != 0)
                {
                    keyadm_printf(vars, "keyadm: bad '-src' address"IP_LF);
                    goto leave;
                }
                IP_BIT_SET(vars->extmap, SEBIT_ADDRESS_SRC);
                break;
            case 2: /* -dst */
                if (keyadm_read_addr(opt.optarg, &vars->dstaddr) != 0)
                {
                    keyadm_printf(vars, "keyadm: bad '-dst' address"IP_LF);
                    goto leave;
                }
                IP_BIT_SET(vars->extmap, SEBIT_ADDRESS_DST);
                break;
            case 3: /* -proto */
                if ((vars->satype = (Ip_s8)keyadm_search_block(opt.optarg, msg_satypes)) == -1)
                {
                    keyadm_printf(vars, "keyadm: unknown security protocol type '%s'"IP_LF, opt.optarg);
                    goto leave;
                }
                vars->have_satype = 1;
                break;
            case 4: /* -minspi */
                vars->minspi = ipcom_strtoul(opt.optarg, IP_NULL, 16);
                IP_BIT_SET(vars->extmap, SEBIT_SPIRANGE);
                break;
            case 5: /* -maxspi */
                vars->maxspi = ipcom_strtoul(opt.optarg, IP_NULL, 16);
                break;
            case 6: /* -ekey */
                if (ipcom_strlen(opt.optarg) > (sizeof(vars->encrypt_key) * 2))
                {
                    keyadm_printf(vars, "keyadm: too long encryption key '%s'"IP_LF, opt.optarg);
                    goto leave;
                }
                vars->encrypt_len = (Ip_u8)(ipcom_strlen(opt.optarg) / 2);
                for (i = 0; i < vars->encrypt_len; i++)
                {
                    vars->encrypt_key[i] = (Ip_u8)keyadm_hex_to_long(vars, opt.optarg + 2 * i, &doexit);
                    if (doexit)
                        goto leave;
                }
                break;
            case 7: /* -akey */
                if (ipcom_strlen(opt.optarg) > (sizeof(vars->auth_key) * 2))
                {
                    keyadm_printf(vars, "keyadm: too long authentication key '%s'"IP_LF, opt.optarg);
                    goto leave;
                }
                vars->auth_len = (Ip_u8)(ipcom_strlen(opt.optarg) / 2);
                for (i = 0; i < vars->auth_len; i++)
                {
                    vars->auth_key[i] = (Ip_u8)keyadm_hex_to_long(vars, opt.optarg + 2 * i, &doexit);
                    if (doexit)
                        goto leave;
                }
                break;
            case 8: /* -enc */
                if ((vars->encrypt_type = (Ip_s8)keyadm_search_block(opt.optarg, sa_encrypt)) == -1)
                {
                    keyadm_printf(vars, "keyadm: unknown encryption algorithm '%s'"IP_LF, opt.optarg);
                    goto leave;
                }
                vars->satype = SADB_SATYPE_ESP;  /* auto pick ESP */
                vars->have_satype = 1;
                break;
            case 9: /* -auth */
                if ((vars->auth_type = (Ip_s8)keyadm_search_block(opt.optarg, sa_auth)) == -1)
                {
                    keyadm_printf(vars, "keyadm: unknown authentication algorithm '%s'"IP_LF, opt.optarg);
                    goto leave;
                }
                break;
            case 10: /* -old */
                vars->old = 1;
                break;
            case 11: /* -halfiv */
                IP_BIT_SET(vars->sa_flags, SADB_X_SAFLAGS_HALFIV);
                break;
            case 12: /* -forcetunnel */
                IP_BIT_SET(vars->sa_flags, SADB_X_SAFLAGS_TUNNEL);
                break;
            case 13: /* -srcmask */
                if (keyadm_read_addr(opt.optarg, &vars->srcmask) != 0)
                {
                    keyadm_printf(vars, "keyadm: bad '-srcmask' mask"IP_LF);
                    goto leave;
                }
                IP_BIT_SET(vars->extmap, SEBIT_X_SRC_MASK);
                break;
            case 14: /* -dstmask */
                if (keyadm_read_addr(opt.optarg, &vars->dstmask) != 0)
                {
                    keyadm_printf(vars, "keyadm: bad '-dstmask' mask"IP_LF);
                    goto leave;
                }
                IP_BIT_SET(vars->extmap, SEBIT_X_DST_MASK);
                break;
            case 15: /* -srcflow */
                if (keyadm_read_addr(opt.optarg, &vars->srcflow) != 0)
                {
                    keyadm_printf(vars, "keyadm: bad '-srcflow' flow"IP_LF);
                    goto leave;
                }
                IP_BIT_SET(vars->extmap, SEBIT_X_SRC_FLOW);
                break;
            case 16: /* -dstflow */
                if (keyadm_read_addr(opt.optarg, &vars->dstflow) != 0)
                {
                    keyadm_printf(vars, "keyadm: bad '-dstflow' flow"IP_LF);
                    goto leave;
                }
                IP_BIT_SET(vars->extmap, SEBIT_X_DST_FLOW);
                break;
            case 17: /* -spi2 */
                vars->spi2_n = ipcom_strtoul(opt.optarg, IP_NULL, 16);
                vars->spi2_n = ip_htonl(vars->spi2_n);
                IP_BIT_SET(vars->extmap, SEBIT_X_SA2);
                break;
            case 18: /* -dst2 */
                if (keyadm_read_addr(opt.optarg, &vars->dstaddr2) != 0)
                {
                    keyadm_printf(vars, "keyadm: bad '-dst2' address"IP_LF);
                    goto leave;
                }
                IP_BIT_SET(vars->extmap, SEBIT_X_DST2);
                break;
            case 19: /* -proto2 */
                if ((vars->satype2 = (Ip_s8)keyadm_search_block(opt.optarg, msg_satypes)) == -1)
                {
                    keyadm_printf(vars, "keyadm: unknown security protocol type '%s'"IP_LF, opt.optarg);
                    goto leave;
                }
                IP_BIT_SET(vars->extmap, SEBIT_X_PROTOCOL);
                break;
            case 20: /* new (redundant, included only to simplify scripts) */
                vars->old = 0;
                break;
            case 21: /* -ingress flow/sa */
                IP_BIT_SET(vars->sa_flags, SADB_X_SAFLAGS_INGRESS_SA);
                IP_BIT_SET(vars->extmap, SEBIT_SA);
                break;
            case 22: /* -srcport */
                if (*opt.optarg < '0' || *opt.optarg > '9')
                {
                    keyadm_printf(vars, "keyadm: flow srcport must be specified by number"IP_LF);
                    goto leave;
                }
                vars->srcport_n = (Ip_u16)ipcom_atoi(opt.optarg);
                vars->srcport_n = ip_htons(vars->srcport_n);
                break;
            case 23: /* -dstport */
                if (*opt.optarg < '0' || *opt.optarg > '9')
                {
                    keyadm_printf(vars, "keyadm: flow dstport must be specified by number"IP_LF);
                    goto leave;
                }
                vars->dstport_n = (Ip_u16)ipcom_atoi(opt.optarg);
                vars->dstport_n = ip_htons(vars->dstport_n);
                break;
            case 24: /* -transport (flow protocol) */
                if (*opt.optarg < '0' || *opt.optarg > '9')
                {
                    keyadm_printf(vars, "keyadm: transport protocol must be specified by its protocol number"IP_LF);
                    goto leave;
                }
                vars->transport_proto      = (Ip_u8)ipcom_atoi(opt.optarg);
                vars->have_transport_proto = 1;
                break;
            case 25: /* -replace flow */
                IP_BIT_SET(vars->sa_flags, SADB_X_SAFLAGS_REPLACEFLOW);
                break;
            case 26: /* -bypass output flow */
                vars->bypass = 1;
                vars->spi_n = 0;
                IP_BIT_SET(vars->extmap, SEBIT_SA); /* spi = 0 */
                IP_BIT_SET(vars->extmap, SEBIT_ADDRESS_DST);  /* dst = 0.0.0.0 */
                vars->satype = SADB_X_SATYPE_BYPASS;
                vars->have_satype = 1;
                break;
            case 27: /* -replay */
                if (*opt.optarg < '0' || *opt.optarg > '9')
                {
                    keyadm_printf(vars, "keyadm: replay counter must be a number"IP_LF);
                    goto leave;
                }
                vars->replay = ipcom_atoi(opt.optarg);
                vars->replay = IP_ROUNDUP(vars->replay, 8);
                if (vars->replay < 0 || vars->replay > 255)
                {
                    keyadm_printf(vars, "keyadm: replay counter must be between 0 and 248"IP_LF);
                    goto leave;
                }
                break;
            case 28: /* -chain */
                IP_BIT_SET(vars->sa_flags, SADB_X_SAFLAGS_CHAINDEL);
                break;
            case 29: /* -proxy */
                if (keyadm_read_addr(opt.optarg, &vars->proxyaddr) != 0)
                {
                    keyadm_printf(vars, "keyadm: bad '-proxy' address"IP_LF);
                    goto leave;
                }
                IP_BIT_SET(vars->extmap, SEBIT_ADDRESS_PROXY);
                break;
            case 30: /* -egress sa */
                IP_BIT_SET(vars->sa_flags, SADB_X_SAFLAGS_EGRESS_SA);
                IP_BIT_SET(vars->extmap, SEBIT_SA);
                break;
            case 31: /* -silent mode, no printfs */
                break;
            case 32: /* -ls = lifetime soft (in time) */
                vars->ls_sec = (Ip_u32)ipcom_atoi(opt.optarg);
                IP_BIT_SET(vars->extmap, SEBIT_LIFETIME_SOFT);
                break;
            case 33: /* -lh = lifetime hard (in time) */
                vars->lh_sec = (Ip_u32)ipcom_atoi(opt.optarg);
                IP_BIT_SET(vars->extmap, SEBIT_LIFETIME_HARD);
                break;
            case 34: /* -deny flow */
                IP_BIT_SET(vars->extmap, SEBIT_X_FLOW_OPTIONS);
                vars->flow_type = SADB_X_FLOW_TYPE_DENY;
                break;
            case 35: /* -acquire flow */
                IP_BIT_SET(vars->extmap, SEBIT_X_FLOW_OPTIONS);
                vars->flow_type = SADB_X_FLOW_TYPE_ACQUIRE;
                break;
            case 36: /* -require flow */
                IP_BIT_SET(vars->extmap, SEBIT_X_FLOW_OPTIONS);
                vars->flow_type = SADB_X_FLOW_TYPE_REQUIRE;
                break;
            case 37: /* b = socker buffer size */
                {
                    int sockbufsize = ipcom_atoi(opt.optarg);
                    if (sockbufsize > 0)
                    {
                        if (ipcom_setsockopt(vars->fd, IP_SOL_SOCKET, IP_SO_RCVBUF, (char *)&sockbufsize, sizeof(sockbufsize)) < 0)
                            keyadm_printf(vars, "keyadm: ipcom_setsockopt(RCVBUF) failed : %s"IP_LF, ipcom_strerror(ipcom_errno));
                    }
                }
                break;
            case 38: /* -srcmaxport */
                if (*opt.optarg < '0' || *opt.optarg > '9')
                {
                    keyadm_printf(vars, "keyadm: flow srcmaxport must be specified by number"IP_LF);
                    goto leave;
                }
                vars->srcmaxport_n = (Ip_u16)ipcom_atoi(opt.optarg);
                vars->srcmaxport_n = ip_htons(vars->srcmaxport_n);
                IP_BIT_SET(vars->extmap, SEBIT_X_SRCMAX_FLOW);
                break;
            case 39: /* -dstmaxport */
                if (*opt.optarg < '0' || *opt.optarg > '9')
                {
                    keyadm_printf(vars, "keyadm: flow dstmaxport must be specified by number"IP_LF);
                    goto leave;
                }
                vars->dstmaxport_n = (Ip_u16)ipcom_atoi(opt.optarg);
                vars->dstmaxport_n = ip_htons(vars->dstmaxport_n);
                IP_BIT_SET(vars->extmap, SEBIT_X_DSTMAX_FLOW);
                break;
            case 40: /* -tfc */
                IP_BIT_SET(vars->sa_flags, SADB_X_SAFLAGS_TFC);
                break;
            case 41: /* -esn */
                IP_BIT_SET(vars->sa_flags, SADB_X_SAFLAGS_ESN);
                break;
            case 42: /* -flowpri */
                vars->flow_priority = ipcom_atoi(opt.optarg);
                if (vars->flow_priority < -128 || vars->flow_priority > 127)
                {
                    keyadm_printf(vars, "keyadm: flow priority must be between -128 and 127"IP_LF);
                    goto leave;
                }
                IP_BIT_SET(vars->extmap, SEBIT_X_FLOW_OPTIONS);
                break;
            case 43: /* -c */
                vars->count = ipcom_atoi(opt.optarg);
                if (vars->count < 1)
                {
                    keyadm_printf(vars, "keyadm: count must be > 0"IP_LF);
                    goto leave;
                }
                break;
            case 44:
                vars->flow_id = (Ip_u32)ipcom_atol(opt.optarg);
                IP_BIT_SET(vars->extmap, SEBIT_X_FLOW_OPTIONS);
                break;
            case 45: /* -srcreq */
                IP_BIT_SET(vars->sa_flags, SADB_X_SAFLAGS_SRCREQ);
                break;
            case 46: /* -icmptype */
                vars->ports0 = (Ip_u8)ipcom_atoi(opt.optarg);
                break;
            case 47: /* -icmpcode */
                vars->ports1 = (Ip_u8)ipcom_atoi(opt.optarg);
                break;
            case 48: /* -sapri */
                vars->sa_priority = ipcom_atoi(opt.optarg);
                if (vars->sa_priority < -128 || vars->sa_priority > 127)
                {
                    keyadm_printf(vars, "keyadm: SA priority must be between -128 and 127"IP_LF);
                    goto leave;
                }
                break;
            case 49: /* -use */
                /* Use flow -> do not need to do anything */
                break;
            case 50: /* '-W' */
                vars->timeout_seconds = ipcom_atoi(opt.optarg);
                break;
            case 51: /* -mhtype */
                vars->ports0 = (Ip_u8)ipcom_atoi(opt.optarg);
                break;
            case 52: /* -srcmaxflow */
                if (keyadm_read_addr(opt.optarg, &vars->srcmaxflow) != 0)
                {
                    keyadm_printf(vars, "keyadm: bad '-srcmaxflow' flow"IP_LF);
                    goto leave;
                }
                IP_BIT_SET(vars->extmap, SEBIT_X_SRCMAX_FLOW);
                break;
            case 53: /* -dstmaxflow */
                if (keyadm_read_addr(opt.optarg, &vars->dstmaxflow) != 0)
                {
                    keyadm_printf(vars, "keyadm: bad '-dstmaxflow' flow"IP_LF);
                    goto leave;
                }
                IP_BIT_SET(vars->extmap, SEBIT_X_DSTMAX_FLOW);
                break;
            case 54: /* -setdf */
                IP_BIT_SET(vars->sa_flags, SADB_X_SAFLAGS_SETDF);
                break;
            case 55: /* -clrdf */
                IP_BIT_SET(vars->sa_flags, SADB_X_SAFLAGS_CLRDF);
                break;
            case 56: /* -dscp */
                vars->dscp = (Ip_u8)ipcom_strtoul(opt.optarg, IP_NULL, 16);
                IP_BIT_SET(vars->extmap, SEBIT_X_FLOW_OPTIONS);
                break;
            case 57: /* -udpencap */
                if (*opt.optarg < '0' || *opt.optarg > '9')
                {
                    keyadm_printf(vars, "keyadm: udpencap port must be specified by number"IP_LF);
                    goto leave;
                }
                vars->udpencap_port = (Ip_u16)ipcom_atoi(opt.optarg);
                IP_BIT_SET(vars->sa_flags, SADB_X_SAFLAGS_UDPENCAP);
                break;
            default:
                keyadm_printf(vars, "keyadm: unknown option %s", long_options[opt.long_index].name);
                if (opt.optarg)
                    keyadm_printf(vars, " with arg %s", opt.optarg);
                keyadm_printf(vars, IP_LF);
                goto leave;
            }
            break;
        case 't':
            /* testmode */
            vars->test = 1;
            keyadm_printf(vars, "using testmode"IP_LF);
            break;
        default:
            goto leave;
        }
    }


    /****** Initial checks ******/
    if (vars->test == 0)
    {
        if (vars->old && vars->encrypt_type > SADB_EALG_3DESCBC)
        {
            keyadm_printf(vars, "keyadm: unknown encryption algorithm '%s' for '-old' ESP"IP_LF,
                         SA_ENCRYPT(vars->encrypt_type));
            goto leave;
        }

        if (IP_BIT_ISSET(vars->sa_flags, SADB_X_SAFLAGS_HALFIV)
            && (vars->old == 0 || vars->satype != SADB_SATYPE_ESP))
        {
            keyadm_printf(vars, "keyadm: option '-halfiv' can be used only with old ESP"IP_LF);
            goto leave;
        }

        if (IP_BIT_ISSET(vars->sa_flags, SADB_X_SAFLAGS_EGRESS_SA)
            && IP_BIT_ISSET(vars->sa_flags, SADB_X_SAFLAGS_INGRESS_SA))
        {
            keyadm_printf(vars, "keyadm: cannot specify \"-egress\" and \"-ingress\" simultaneously"IP_LF);
            goto leave;
        }
        if (vars->satype != SADB_SATYPE_ESP && IP_BIT_ISSET(vars->sa_flags, SADB_X_SAFLAGS_TFC))
        {
            keyadm_printf(vars,
                          "keyadm: cannot specify \"-tfc\" for SAs using security protocols other than ESP"IP_LF);
            goto leave;
        }
    }


    /****** Run the command ******/
    ret = keyadm_run_command(argc, argv[1], vars);

    goto leave;


    /* Generic usage */
 usage:
    keyadm_printf(vars,
                  "IPsec PFKey admin command, version 3.0"IP_LF
                  "Copyright (c) 2000-2009, Interpeak AB <www.interpeak.com>"IP_LF
                  "   usage: keyadm <command> [modifier(s)...]"IP_LF
                  );
    keyadm_printf(vars,
                  "   General commands: "IP_LF
                  "       flush             remove all SAs and flows"IP_LF
                  "       monitor           monitor PFKey kernel messages"IP_LF
#ifdef IP_DEBUG
                  "       register          send register PFkey message"IP_LF
#endif
                  );
    keyadm_printf(vars,
                  "   SA commands: "IP_LF
                  "       add               add a Security Association (SA)"IP_LF
                  "       delete            delete a Security Association (SA)"IP_LF
                  "       get               get (info about) a Security Association (SA)"IP_LF
                  "       getspi            create a larval SA with a (random) SPI"IP_LF
                  "       update            update a Security Association (SA)"IP_LF
                  "       group             group two Security Associations (SAs)"IP_LF
                  "       dump              list SA database"IP_LF
                  );
    keyadm_printf(vars,
                  "   Flow commands: "IP_LF
                  "       addflow           add a Flow (Security Policy)"IP_LF
                  "       delflow           delete a Flow (Security Policy)"IP_LF
                  "       findflow          lookup a Flow (Security Policy)"IP_LF
                  "       dumpflow          list Flow (Security Policy) database"IP_LF
                  );
    keyadm_printf(vars,
                  "   Generic command modifiers: "IP_LF
                  "       -b <size>         set pfkey socket receive buffer size"IP_LF
                  "       -silent           do not send any output to stdout/stderr"IP_LF
                  );
    keyadm_printf(vars,
                  "   NOTE:"IP_LF
                  "       Type 'keyadm <command>' for command help menu!"IP_LF
                  "       (except for dump and flush which can be used without args)"IP_LF
                  );

 leave:
    if (vars->orgbuf)
        ipcom_free(vars->orgbuf);
    if (vars->fd != IP_INVALID_SOCKET)
        ipcom_socketclose(vars->fd);
 leave2:
    if (vars->stdoutbuf)
        ipcom_free(vars->stdoutbuf);
    return ret;
}


/*
 *===========================================================================
 *                    ipipsec_cmd_keyadm
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipipsec_cmd_keyadm(int argc, char **argv)
{
    Vars   *vars;
    int ret;

    /* Allocate the vars variable dynamically to reduce
     * stack space allocated
     */
    vars = ipcom_malloc(sizeof(*vars));
    if (vars == IP_NULL)
        return -1;
    ret  = ipipsec_cmd_keyadm_private(vars, argc, argv);
    ipcom_free(vars);
    return ret;
}


/*
 ****************************************************************************
 *                         END OF FILE
 *===========================================================================
 */

