/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipdnsc_cmd_nslookup.c,v $ $Revision: 1.20 $
 *   $Source: /home/interpeak/CVSRoot/ipappl/src/ipdnsc_cmd_nslookup.c,v $
 *   $Author: markus $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Markus Carlstedt <markus@interpeak.se>
 ******************************************************************************
 */

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 * nslookup shell command.
 */

/*
 ****************************************************************************
 * 2                    CONFIGURATION
 ****************************************************************************
 */

#include "ipdnsc_config.h"


/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#define IPCOM_USE_CLIB_PROTO
#include <ipcom_clib.h>
#include <ipcom_getopt.h>

#include "ipdnsc.h"
#include "ipdnsc_h.h"


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

IP_PUBLIC int ipdnsc_cmd_nslookup(int argc, char **argv);


/*
 ****************************************************************************
 * 8                    DATA
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 9                   LOCAL FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipdnsc_ping_print_usage
 *===========================================================================
 * Description: Prints usage information for the nslookup shell command.
 * Parameters:  None.
 * Returns:     Nothing.
 */
IP_STATIC void
ipdnsc_nslookup_print_usage(void)
{
    ipcom_printf("\n");
    ipcom_printf("NAME\n");
    ipcom_printf("     nslookup - query Internet name servers\n");
    ipcom_printf("\n");
    ipcom_printf("SYNOPSIS\n");
    ipcom_printf("     nslookup [-f] [-c] [-v version] [-t timeout] [-r retries] host [nameserver]\n");
    ipcom_printf("\n");
    ipcom_printf("DESCRIPTION\n");
    ipcom_printf("     nslookup queries an Internet name server for information abouts hosts.\n");
    ipcom_printf("     host may be a domain name or an Internet v4 or v6 address.\n");
    ipcom_printf("     [nameserver] is optional and provides the possibility to use a\n");
    ipcom_printf("     nameserver other than the default. [nameserver] must be specified\n");
    ipcom_printf("     by its Internet address.\n");
    ipcom_printf("\n");
    ipcom_printf("     -f      Flush the DNS resolver cache and then exit immediately.\n");
    ipcom_printf("     -c      Use the DNS resolver's local cache. Default is to always\n");
    ipcom_printf("             perform the name server lookup and not use the cache.\n");
    ipcom_printf("     -v version\n");
    ipcom_printf("             Specifies the type of address that is requested in a\n");
    ipcom_printf("             name to address lookup. Allowed versions are 4 and 6.\n");
    ipcom_printf("             If the -v flag is not given, nslookup first tries to find an\n");
    ipcom_printf("             ipv6 address and then, if not found, an ipv4 address.\n");
    ipcom_printf("     -t timeout\n");
    ipcom_printf("             Specifies how long to wait for an answer from the nameserver\n");
    ipcom_printf("             before retrying.\n");
    ipcom_printf("     -r retries\n");
    ipcom_printf("             Specifies the number of retries to use if the nameserver\n");
    ipcom_printf("             fails to answer.\n");
    ipcom_printf("\n");
    ipcom_printf("RETURN VALUES\n");
    ipcom_printf("     The nslookup command will exit with 0 on success, and non-zero on errors.\n");
}


/*
 ****************************************************************************
 * 10                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                         ipcom_cmd_nslookup
 *===========================================================================
 * Description: Shell command nslookup.
 * Parameters:  argc - number of arguments.
 *              argv - list of arguments.
 * Returns:     0 = success.
 *             -1 = failed.
 */
IP_PUBLIC int
ipdnsc_cmd_nslookup(int argc, char **argv)
{
    struct Ip_hostent *he1, *he2 = IP_NULL;
    struct Ip_in_addr host_addr;
#ifdef IPCOM_USE_INET6
    struct Ip_in6_addr host_addr6;
#endif
    Ipdnsc_ns_spec dns;
    int address_lookup, use_cache, i, version, host_family = IP_AF_INET;
    Ip_s32 err_num = IPDNSC_NO_RECOVERY;
    char buf[40];

    if(argc <= 1)
    {
        /* Interactive mode not supported - user must supply a hostname or internet
        address and optionally a name server */
        goto usage;
    }

    /* Get the default name server parameters */
    if (ipdnsc_default(&dns))
    {
        ipcom_printf("Could not find name server.\n");
        return -1;
    }
    /* Use only the primary name server */
    ipcom_memset(&dns.secondary_dns.addr, 0, sizeof(dns.secondary_dns.addr));
    dns.secondary_dns.family = 0;
    ipcom_memset(&dns.tertiary_dns.addr, 0, sizeof(dns.tertiary_dns.addr));
    dns.tertiary_dns.family = 0;
    ipcom_memset(&dns.quaternary_dns.addr, 0, sizeof(dns.quaternary_dns.addr));
    dns.quaternary_dns.family = 0;
    address_lookup = IP_FALSE;
    use_cache = IP_FALSE;
    version = 0;

    ipcom_getopt_clear();
    while ((i = ipcom_getopt(argc, argv, "fcv:t:r:")) != -1)
    {
        switch (i)
        {
        case 'f':
            ipdnsc_cache_flush();
            return 0;
        case 'c':
            use_cache = IP_TRUE;
            break;
        case 'v':
            version = ipcom_atoi(ip_optarg);
            if(version != 4 && version != 6)
            {
                ipcom_printf("Version must be either 4 or 6\n");
                return -1;
            }
            break;

        case 't':
            dns.timeout = ipcom_atoi(ip_optarg);
            if(dns.timeout < 1 || dns.timeout > 60)
            {
                ipcom_printf("Timeout must be between 1 and 60 seconds\n");
                return -1;
            }
            break;

        case 'r':
            dns.retry = ipcom_atoi(ip_optarg);
            if (dns.retry < 0 || dns.retry > 5)
            {
                ipcom_printf("Retries must be between 0 and 5.\n");
                return -1;
            }
            break;
        default:
            ipcom_printf("'%c' is a unknown switch\n", i);
            return -1;
        }
    }

    /* Get the hostname or host address */
    if(argv[ip_optind])
    {
        /* Check if first argument is a presentation form ipv4 or ipv6 address */
        if(ipcom_inet_pton(IP_AF_INET, argv[ip_optind], &host_addr) > 0)
        {
            address_lookup = IP_TRUE;
            host_family = IP_AF_INET;
        }
#ifdef IPCOM_USE_INET6
        else if(ipcom_inet_pton(IP_AF_INET6, argv[ip_optind], &host_addr6) > 0)
        {
            address_lookup = IP_TRUE;
            host_family = IP_AF_INET6;
        }
#endif
    }
    else
    {
        ipcom_printf("Interactive mode not supported.\n");
        return -1;
    }

    /* Get the optional name server */
    if(argv[ip_optind+1])
    {
        /* Configure the primary DNS */
        dns.primary_dns.family = IP_AF_INET;
        if (ipcom_inet_pton (dns.primary_dns.family,
                             argv[ip_optind+1],
                             &dns.primary_dns.addr) != 1)
        {
#ifdef IPCOM_USE_INET6
            dns.primary_dns.family = IP_AF_INET6;
            if(ipdnsc_get_ipv6_addrconfig(&dns.primary_dns.addr.inaddr6, &dns.primary_dns.ifindex, argv[ip_optind+1]) < 0)
#endif
                /* Bad address */
                dns.primary_dns.family = 0;
        }

        if(dns.primary_dns.family == 0)
        {
            ipcom_printf("Name server must be an internet address in presentation form.\n");
            return -1;
        }
    }

    /* Lookup the address of the DNS server. Never use the cache */
    he1 = ipdnsc_resolve_addr((Ip_u8 *)&dns.primary_dns.addr, dns.primary_dns.family, IP_FALSE, &dns, &err_num);
    if(!he1)
    {
        if(ipcom_inet_ntop(dns.primary_dns.family, &dns.primary_dns.addr, buf, sizeof(buf)))
        {
            ipcom_printf("*** Can't find server name for address %s : ", buf);
        }

        switch(err_num)
        {
            case IPDNSC_HOST_NOT_FOUND:
                ipcom_printf("Non-existent host/domain.\n");
                break;
            case IPDNSC_TRY_AGAIN:
                ipcom_printf("Try Again.\n");
                break;
            case IPDNSC_NO_RECOVERY:
                ipcom_printf("No Recovery.\n");
                break;
            case IPDNSC_NO_DATA:
                ipcom_printf("No Data.\n");
                break;
            default:
                ipcom_printf("Unknown(%d)\n", err_num);
                break;
        }
        /* Print the hostname */
        ipcom_printf("Server:  unknown\n");
        /* Print the address */
        if(ipcom_inet_ntop(dns.primary_dns.family, &dns.primary_dns.addr, buf, sizeof(buf)))
            ipcom_printf("Address: %s\n", buf);
        else
            return -1;
    }
    else
    {
        /* Print the hostname */
        ipcom_printf("Server:  %s\n", he1->h_name);
        /* Print the address */
        if(ipcom_inet_ntop(he1->h_addrtype, he1->h_addr_list[0], buf, sizeof(buf)))
        {
            ipcom_printf("Address: %s\n", buf);
        }
        else
        {
            ipdnsc_hostent_free(he1);
            return -1;
        }
    }
    ipcom_printf("\n");

    /* Lookup the hostname or host address. */
    if(address_lookup)
    {
        if(host_family == IP_AF_INET)
        {
            he2 = ipdnsc_resolve_addr((Ip_u8 *)&host_addr, host_family, use_cache, &dns, &err_num);
        }
#ifdef IPCOM_USE_INET6
        else if(host_family == IP_AF_INET6)
        {
            he2 = ipdnsc_resolve_addr((Ip_u8 *)&host_addr6, host_family, use_cache, &dns, &err_num);
        }
#endif
    }
    else
    {
        if(version == 4)
        {
            host_family = IP_AF_INET;
            he2 = ipdnsc_resolve_name((Ip_u8 *)argv[ip_optind], host_family, use_cache, &dns, &err_num);
        }
#ifdef IPCOM_USE_INET6
        else if(version == 6)
        {
            host_family = IP_AF_INET6;
            he2 = ipdnsc_resolve_name((Ip_u8 *)argv[ip_optind], host_family, use_cache, &dns, &err_num);
        }
#endif
        else
        {
#ifdef IPCOM_USE_INET6
            /* First try to find a v6 address */
            host_family = IP_AF_INET6;
            he2 = ipdnsc_resolve_name((Ip_u8 *)argv[ip_optind], host_family, use_cache, &dns, &err_num);
            if(!he2 && (err_num == IPDNSC_HOST_NOT_FOUND || err_num == IPDNSC_NO_ADDRESS))
            {
                host_family = IP_AF_INET;
                /* If not found try a v4 address */
                he2 = ipdnsc_resolve_name((Ip_u8 *)argv[ip_optind], host_family, use_cache, &dns, &err_num);
            }
#else
            host_family = IP_AF_INET;
            he2 = ipdnsc_resolve_name((Ip_u8 *)argv[ip_optind], host_family, use_cache, &dns, &err_num);
#endif
        }
    }

    if(!he2)
    {
        ipcom_printf("*** %s can't find %s : ", he1 != IP_NULL ? he1->h_name : buf, argv[ip_optind]);
        switch(err_num)
        {
            case IPDNSC_HOST_NOT_FOUND:
                ipcom_printf("Non-existent host/domain.\n");
                break;
            case IPDNSC_TRY_AGAIN:
                ipcom_printf("Try Again.\n");
                break;
            case IPDNSC_NO_RECOVERY:
                ipcom_printf("No Recovery.\n");
                break;
            case IPDNSC_NO_DATA:
                ipcom_printf("No Data.\n");
                break;
            default:
                ipcom_printf("Unknown(%d)\n", err_num);
                break;
        }
        ipcom_printf("\n");
        ipdnsc_hostent_free(he1);
        return 0;
    }

    /* Print the hostname */
    ipcom_printf("Name:    %s\n", he2->h_name);
    /* Print the addresses */
    i=0;
    if(he2->h_addr_list != IP_NULL)
        while(he2->h_addr_list[i] != IP_NULL)
        {
            if(ipcom_inet_ntop(he2->h_addrtype, he2->h_addr_list[i], buf, sizeof(buf)))
            {
                if(!i)
                    ipcom_printf("Address: %s\n", buf);
                else
                    ipcom_printf("         %s\n", buf);
            }
            i++;
        }
    /* Print the aliases */
    i=0;
    if(he2->h_aliases != IP_NULL)
        while(he2->h_aliases[i] != IP_NULL)
        {
            if(!i)
                ipcom_printf("Aliases: %s\n", he2->h_aliases[i]);
            else
                ipcom_printf("         %s\n", he2->h_aliases[i]);
            i++;
        }

    ipcom_printf("\n");

    ipdnsc_hostent_free(he1);
    ipdnsc_hostent_free(he2);

    return 0;

usage:
    ipdnsc_nslookup_print_usage();
    return 0;
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
