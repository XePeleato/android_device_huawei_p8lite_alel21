/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_cmd.c,v $ $Revision: 1.29 $
 *     $Source: /home/interpeak/CVSRoot/ipcom/src/ipcom_cmd.c,v $
 *     $Author: ulf $
 *     $State: Exp $ $Locker:  $
 *
 *     INTERPEAK_COPYRIGHT_STRING
 *     Design and implementation by Kenneth Jonsson <kenneth@interpeak.se>
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
#include "ipcom_inet.h"
#include "ipcom_sock.h"
#include "ipcom_sock2.h"
#include "ipcom_netif.h"
#include "ipcom_cmd.h"


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

IP_STATIC IP_CONST char *
ipcom_cmd_link_ntop(struct Ip_sockaddr_dl *dl, char *str, Ip_size_t len);


/*
 ****************************************************************************
 * 8                    DATA
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 9                    STATIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipcom_cmd_link_ntop
 *===========================================================================
 * Description: Creates a string representation of the hardware address.
 * Parameters:  dl - The hardware address.
 *              strptr - Will hold the sting representation of the address.
 *              strlen - The length of the buffer pointer to by 'strptr'
 * Returns:     The string represenstion.
 *
 */
IP_STATIC IP_CONST char *
ipcom_cmd_link_ntop(struct Ip_sockaddr_dl *dl, char *str, Ip_size_t len)
{
    Ip_ssize_t alen;

    alen = dl->sdl_alen;

    if (alen > 0)
    {
        Ip_ssize_t x;
        Ip_ssize_t slen;
        int byte;

        for (x = 0; x < alen; x++)
        {
            slen = len - x * 3;
            if (slen < 0)
                break;
            byte = ((Ip_u8*) IP_SOCKADDR_DL_LLADDR(dl))[x];
            ipcom_snprintf(&str[x * 3], slen, "%02x%s", byte,
                x + 1 == alen ? "\0" : ":");
        }
    }
    else if (dl->sdl_index != 0)
        ipcom_snprintf(str, len, "link#%d", dl->sdl_index);
    else
        ipcom_snprintf(str, len, "*");

    return str;
}


/*
 *===========================================================================
 *                    ipcom_cmd_addr_to_str
 *===========================================================================
 * Description: Converts a IP address to a string representation.
 * Parameters:  domain - The address domain for addr (IP_AF_INET or
 *              IP_AF_INET6).
 *              addr - A pointer to the address.
 *              no_dns - IP_TRUE if the address should be resolved to a DNS name.
 *              str - A pointer to a buffer that will receive the string
 *              representation of the address.
 *              len - The length of the 'straddr' buffer.
 * Returns:     The buffer with a text version of the socket address.
 *
 */
IP_STATIC IP_CONST char *
ipcom_cmd_addr_to_str(int domain, void *addr, Ip_bool no_dns, char *str, Ip_size_t len)
{
#ifndef IP_PORT_LKM
    union Ip_sockaddr_union su;
#endif

    if(ipcom_inet_ntop(domain, addr, str, len) == IP_NULL)
    {
        return IP_NULL;
    }

    if(no_dns == 0)
    {
#ifndef IP_PORT_LKM
        if(ipcom_getsockaddrbyaddr(domain, str, (struct Ip_sockaddr *)&su) != 0)
        {
            return IP_NULL;
        }
        if(ipcom_getnameinfo((struct Ip_sockaddr *)&su, IPCOM_SA_LEN_GET(&su.sa), str, len, IP_NULL, 0, 0) != 0)
        {
            return IP_NULL;
        }
#endif
    }

    return str;

}


/*
 ****************************************************************************
 * 10                   PUBLIC FUNCTIONS
 ****************************************************************************
 */


/*
 *===========================================================================
 *                    ipcom_cmd_sockaddr_to_str
 *===========================================================================
 * Description: Converts a socket address to a string representation.
 * Parameters:  sa - The socket address to convert.
 *              include_port - Set to IP_TRUE if the port value should be included.
 *              no_dns - Set to IP_TRUE if no DNS lookup should be made
 *              for IP addresses.
 *              str - The buffer where to place the string representation.
 *              strlen - The length of 'str'.
 * Returns:     The buffer with a text version of the socket address.
 *
 */
IP_PUBLIC IP_CONST char *
ipcom_cmd_sockaddr_to_str(struct Ip_sockaddr *sa, Ip_bool include_port,
                          Ip_bool no_dns, char *str, Ip_size_t len)
{
    switch(sa->sa_family)
    {
#ifdef IPCOM_USE_INET
    case IP_AF_INET :
        (void)ipcom_cmd_addr_to_str(IP_AF_INET, &((struct Ip_sockaddr_in *) sa)->sin_addr, no_dns, str, len);
        break;
#endif

#ifdef IPCOM_USE_INET6
    case IP_AF_INET6:
        {
            struct Ip_sockaddr_in6 *sa_in6;

            sa_in6 = (struct Ip_sockaddr_in6 *) sa;
            (void)ipcom_cmd_addr_to_str(IP_AF_INET6, &sa_in6->sin6_addr, no_dns, str, len);
            if (sa_in6->sin6_scope_id)
            {
                char if_name[IP_IFNAMSIZ];

                if (ipcom_if_indextoname(sa_in6->sin6_scope_id, if_name) == IP_NULL)
                    ipcom_snprintf(if_name, sizeof(if_name), "%ld", sa_in6->sin6_scope_id);
                ipcom_strncat(str, "%", len - ipcom_strlen(str));
                ipcom_strncat(str, if_name, len - ipcom_strlen(str));
            }
        }
        break;
#endif /* IPCOM_USE_INET6 */

    case IP_AF_LINK:
        (void)ipcom_cmd_link_ntop((struct Ip_sockaddr_dl *) sa, str, len);
        break;

    case IP_AF_MPLS:
        {
            struct Ip_sockaddr_mpls *sa_mpls = (struct Ip_sockaddr_mpls*)sa;
            ipcom_snprintf(str, len, "mpls#%lu", sa_mpls->smpls_key);
            break;
        }

    default:
        /* Unknown address family */
        IP_PANIC2();
        return "unknown address family";
    }

#if defined(IPCOM_USE_INET) || defined(IPCOM_USE_INET6)
    if (include_port && (sa->sa_family == IP_AF_INET || sa->sa_family == IP_AF_INET6))
    {
        Ip_u16 port;

        len -= ipcom_strlen(str);
        port = ip_ntohs(((struct Ip_sockaddr_in *) sa)->sin_port);
        if (port)
        {
            char str_port[12];

            ipcom_sprintf(str_port, ".%d", port);
            ipcom_strncat(str, str_port, len);
        }
        else
            ipcom_strncat(str, ".*", len);
    }
#endif

    return str;
}


/*
 *===========================================================================
 *                    ipcom_cmd_if_flags_to_str
 *===========================================================================
 * Description: Returns a string representation of the IP_IFF_xxx flags.
 * Parameters:  flags - The IP_IFF_xxx flags.
 *              str - The buffer to put the flag string form.
 *              len - Length of the str buffer.
 * Returns:     The string representation.
 *
 */
IP_PUBLIC IP_CONST char *
ipcom_cmd_if_flags_to_str(int flags, char *str, Ip_size_t len)
{
    ipcom_snprintf(str, len, "%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s",
                   flags & IP_IFF_UP ? "UP " : "",
                   flags & IP_IFF_RUNNING ? "RUNNING " : "",
                   flags & IP_IFF_LOOPBACK ? "LOOPBACK " : "",
                   flags & IP_IFF_SIMPLEX ? "SIMPLEX " : "",
                   flags & IP_IFF_POINTOPOINT ? "POINTOPOINT " : "",
                   flags & IP_IFF_BROADCAST ? "BROADCAST " : "",
                   flags & IP_IFF_MULTICAST ? "MULTICAST " : "",
                   flags & IP_IFF_X_DHCPRUNNING ? "DHCP " : "",
                   flags & IP_IFF_NOARP ? "NOARP " : "",
                   flags & IP_IFF_PROMISC ? "PROMISC " : "",
                   flags & IP_IFF_ALLMULTI ? "ALLMULTI " : "",
                   flags & IP_IFF_LINK0 ? "LINK0 " : "",
                   flags & IP_IFF_LINK1 ? "LINK1 " : "",
                   flags & IP_IFF_LINK2 ? "LINK2 " : "",
                   flags & IP_IFF_DEBUG ? "DEBUG " : "");
    return str;
}

/*
 *===========================================================================
 *                  ipcom_cmd_if_caps_to_str
 *===========================================================================
 * Description: Returns a string representation of the IPCOM_IF_DRV_CAP_xxx
 *              flags
 * Parameters:  caps - The IPCOM_IF_DRV_CAP_xxx flags
 *              str - The buffer to put the flag string form.
 *              len - Length of the str buffer.
 * Returns:     The string representation.
 *
 */
IP_PUBLIC IP_CONST char *
ipcom_cmd_if_caps_to_str(int caps, char *str, Ip_size_t len)
{
#define IFF_CAP_ISSET(what, disp) IP_BIT_ISSET(caps, what) ? disp : ""

    ipcom_snprintf(str, len, "%s%s%s%s%s%s%s",
        IFF_CAP_ISSET(IPCOM_IF_DRV_CAP_IP4_TRANSPORT_CSUM_TX, "TXCSUM "),
        IFF_CAP_ISSET(IPCOM_IF_DRV_CAP_IP6_TRANSPORT_CSUM_TX, "TX6CSUM "),
        IFF_CAP_ISSET(IPCOM_IF_DRV_CAP_IP4_TSO, "TCPSEG_OFFLOAD "),
        IFF_CAP_ISSET(IPCOM_IF_DRV_CAP_IP6_TSO, "TCP6SEG_OFFLOAD "),
        IFF_CAP_ISSET(IPCOM_IF_DRV_CAP_VLAN_MTU, "VLAN_MTU "),
        IFF_CAP_ISSET(IPCOM_IF_DRV_CAP_VLAN_HWTAGGING_TX, "VLAN_TXHWTAG "),
        IFF_CAP_ISSET(IPCOM_IF_DRV_CAP_VLAN_HWTAGGING_RX, "VLAN_RXHWTAG ")
            );
    return str;
}

/*
 *===========================================================================
 *                    ipcom_cmd_key_to_str
 *===========================================================================
 * Description: Converts the key to the string representation in map.
 * Parameters:  map - The int to string map to use.
 *              key - The protocol number.
 *              str - The buffer where the 'key' numeric value will be stored
 *              if 'key' is not present in the map.
 *              str_len - The length of the 'str' buffer.
 * Returns:     The string representation.
 *
 */
IP_PUBLIC IP_CONST char *
ipcom_cmd_key_to_str(Ipcom_cmd_int_str_map *map, int key,
                     char *str, Ip_size_t str_len)
{
    int i;

    for (i = 0; map[i].key != -1; i++)
        if (map[i].key == key)
            return map[i].str;
    if (str == IP_NULL)
    {
        IP_PANIC();
        return IP_NULL;
    }

    ipcom_snprintf(str, str_len, "%d", key);
    return str;
}


/*
 *===========================================================================
 *                    ipcom_cmd_str_to_key
 *===========================================================================
 * Description: Converts the string representation in map to the numeric value.
 * Parameters:  map - The int to string map to use.
 *              key - The protocol number.
 * Returns:     The string representation.
 *
 */
IP_PUBLIC int
ipcom_cmd_str_to_key(IP_CONST Ipcom_cmd_int_str_map *map, IP_CONST char *str)
{
    int val;
    if (ipcom_cmd_str_to_key2(map, str, &val) < 0)
        return -1;
    return val;
}


/*
 *===========================================================================
 *                    ipcom_cmd_str_to_key2
 *===========================================================================
 * Description: Converts the string representation in map to the numeric value.
 * Parameters:  map - The int to string map to use.
 *              key - The protocol number.
 *              val - Pointer to where the value should be stored.
 * Returns:
 *
 */
IP_PUBLIC int
ipcom_cmd_str_to_key2(IP_CONST Ipcom_cmd_int_str_map *map,
                      IP_CONST char *str,
                      int *val)
{
    int i;

    if (str)
    {
        for (i = 0; map[i].str != IP_NULL; i++)
            if (ipcom_strcasecmp(str, map[i].str) == 0)
            {
                *val = map[i].key;
                return 0;
            }
    }

    return -1;
}

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */


