/*
 ******************************************************************************
 *                     HEADER FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipdnsc_h.h,v $ $Revision: 1.10 $
 *   $Source: /home/interpeak/CVSRoot/ipappl/src/ipdnsc_h.h,v $
 *   $Author: markus $ $Date: 2006-11-30 09:47:01 $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Ulf Olofsson <ulf@interpeak.se>
 ******************************************************************************
 */
#ifndef IPDNSC_H_H
#define IPDNSC_H_H

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 * DNS client local header file.
 */

/*
 ****************************************************************************
 * 2                    CONFIGURATION
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

/* Internet address lengths */
#define IPDNSC_INADDRSZ     4
#define IPDNSC_IN6ADDRSZ    16

/* Maximum length of a FQDN */
#define IPDNSC_MAXNAME      255

/* The delimiter between the address and the interface for scoped addresses
 * ex FE80::200:30FF:FE40:5060%eth0
 */
#define IPDNSC_IF_DELIMITER  '%'


/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

/*
 *===========================================================================
 *                         Ipdnsc_ns_addr
 *===========================================================================
 */
typedef struct Ipdnsc_ns_addr_struct
{
    Ip_s32 family;
    union
    {
        struct Ip_in_addr inaddr;       /* Network endian */
#ifdef IPCOM_USE_INET6
        struct Ip_in6_addr inaddr6;     /* Network endian */
#endif
    } addr;
#ifdef IPCOM_USE_INET6
    Ip_u32 ifindex;     /* Interface index for link local v6 addresses */
#endif
    Ip_u16 port;                        /* Network endian */
    Ip_u16 pad;
}
Ipdnsc_ns_addr;


/*
 *===========================================================================
 *                         Ipdnsc_ns_spec
 *===========================================================================
 */
typedef struct Ipdnsc_ns_spec_struct
{
    Ip_s32 timeout;
    Ip_s32 retry;   

#ifdef IPCOM_DNSC_TC_SUPPORT
    Ip_bool use_tcp;        /*决定当前是否使用TCP进行DNS解析*/
#endif

    Ipdnsc_ns_addr primary_dns;
    Ipdnsc_ns_addr secondary_dns;
    Ipdnsc_ns_addr tertiary_dns;
    Ipdnsc_ns_addr quaternary_dns;
}
Ipdnsc_ns_spec;


/*
 *===========================================================================
 *                         Ipdnsc_cache
 *===========================================================================
 */

typedef struct Ipdnsc_cache_struct
{
    struct Ipdnsc_cache_struct *next;
    Ip_u32 type;
    Ip_u32 expire;
    struct Ip_hostent *he;
}
Ipdnsc_cache;


/*
 *===========================================================================
 *                         Ipdnsc_data
 *===========================================================================
 */
typedef struct Ipdnsc_data_struct
{
    Ip_bool init;
    Ipcom_mutex mutex;
    Ipdnsc_cache *cache_head;
    Ip_u32 cache_entries;

#ifdef IPCOM_DNSC_TC_SUPPORT    
    Ipcom_mutex user_mutex;
    Ip_u32 current_users;      /*当前同时使用DNS client的用户数*/
#endif
}
Ipdnsc_data;


/*
 ****************************************************************************
 * 6                    LOCAL FUNCTIONS
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 10                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

IP_EXTERN Ipdnsc_data ipdnsc;
IP_GLOBAL struct Ip_hostent* ipdnsc_resolve_addr(Ip_u8 *addr, Ip_s32 family, Ip_s32 use_cache,
                                                 Ipdnsc_ns_spec *dns, Ip_s32 *error_num);
IP_GLOBAL struct Ip_hostent* ipdnsc_resolve_name(Ip_u8 *name, Ip_s32 family, Ip_s32 use_cache,
                                                 Ipdnsc_ns_spec *dns, Ip_s32 *error_num);
IP_GLOBAL void ipdnsc_hostent_free(struct Ip_hostent *he);
IP_GLOBAL struct Ip_hostent* ipdnsc_hostent_create(Ip_s32 type);
IP_GLOBAL Ip_s32 ipdnsc_hostent_insert_addr(struct Ip_hostent *he, char *addr);
IP_GLOBAL Ip_s32 ipdnsc_hostent_insert_alias(struct Ip_hostent *he, char *name);
IP_GLOBAL Ip_s32 ipdnsc_hostent_insert_name(struct Ip_hostent *he, char *name);
IP_GLOBAL struct Ip_hostent* ipdnsc_hostent_copy(struct Ip_hostent *he);
IP_GLOBAL Ip_s32 ipdnsc_default(Ipdnsc_ns_spec *dns);
IP_GLOBAL void ipdnsc_dns_tick(void);
IP_GLOBAL int ipdnsc_dns_init(void);
IP_GLOBAL int ipdnsc_cmd_nslookup(int argc, char **argv);
IP_GLOBAL Ip_err ipdnsc_start(void);
#ifdef IPCOM_USE_INET6
IP_GLOBAL Ip_s32 ipdnsc_get_ipv6_addrconfig(struct Ip_in6_addr *addr6, Ip_u32 *ifindex, char *addrstr);
#endif

#endif


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
