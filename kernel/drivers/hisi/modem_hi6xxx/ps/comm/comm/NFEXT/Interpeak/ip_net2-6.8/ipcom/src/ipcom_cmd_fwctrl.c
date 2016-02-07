/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_cmd_fwctrl.c,v $ $Revision: 1.20.22.1 $
 *     $Source: /home/interpeak/CVSRoot/ipcom/src/ipcom_cmd_fwctrl.c,v $
 *     $Author: ulf $
 *     $State: Exp $ $Locker:  $
 *
 *     INTERPEAK_COPYRIGHT_STRING
 ******************************************************************************
 */

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 *
 *  Shell command related to IP_FORWARDING.
 *
 *  The contents of this file is only valid if IPCOM_USE_FORWARDER is defined
 */

/*
 ****************************************************************************
 * 2                    CONFIGURATION
 ****************************************************************************
 */

#include "ipcom_config.h"
/* Refer to description above */
#ifdef IPCOM_USE_FORWARDER

/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#define IPCOM_USE_CLIB_PROTO
#include "ipcom_clib.h"
#include "ipcom_cstyle.h"
#include "ipcom_getopt.h"
#include "ipcom_os.h"
#include "ipcom_time.h"
#include "ipcom_type.h"
#include "ipcom_err.h"
#include "ipcom_netif.h"
#include "ipcom_shell.h"

#include <ipnet.h>
#include <ipnet_config.h>
#include <ipnet_netif.h>
#include <ipnet_eth.h>
#include <ipnet_route.h>
#include "ipnet_usr_sock.h"

#include "ipcom_forwarder.h"

/*
 ****************************************************************************
 * 4                    TYPES
 ****************************************************************************
 */



/*
 ****************************************************************************
 * 9                    STATIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipcom_forwarder_print_mac_addr
 *===========================================================================
 * Context:     shell
 * Description: Print a Ethernet MAC address in ASCII format on stdout
 * Parameters:
 * Returns:     0
 *
 */

IP_STATIC int
ipcom_forwarder_print_mac_addr(Ip_u8 *mac_addr)
{
    ipcom_printf("%02x:%02x:%02x:%02x:%02x:%02x",
                 (mac_addr[0]),
                 (mac_addr[1]),
                 (mac_addr[2]),
                 (mac_addr[3]),
                 (mac_addr[4]),
                 (mac_addr[5]));

    return 0;
}
/*
 *===========================================================================
 *                    ipcom_forwarder_print_ip4_addr
 *===========================================================================
 * Context:     shell
 * Description: Print an IPv4 address in ASCII format on stdout
 * Parameters:
 * Returns:     0
 *
 */

IP_STATIC int
ipcom_forwarder_print_ip4_addr(Ip_u32 ip4_addr)
{
    ip4_addr = ip_ntohl (ip4_addr);
    ipcom_printf("%d.%d.%d.%d", (ip4_addr >> 24), (ip4_addr >> 16) & 0xff, (ip4_addr >> 8) & 0xff, ip4_addr & 0xff);

    return 0;
}
/*
 *===========================================================================
 *                    ipcom_forwarder_print_ip6_addr
 *===========================================================================
 * Context:     shell
 * Description: Print an IPv6 address in ASCII format on stdout
 * Parameters:
 * Returns:     0
 *
 */
IP_STATIC int
ipcom_forwarder_print_ip6_addr(Ip_u32 *ip6_addr)
{
    unsigned int i;
    int si, zcl, zcs, zml, zms;
    unsigned int ip6_a[8];
    unsigned char *s = (unsigned char *)ip6_addr;
    char ip6_addr_sz[50];

    for (i = 0, zms = 0, zml = 0, zcs = -1, zcl = 0; i < sizeof(ip6_a)/sizeof(ip6_a[0]); i++, s += 2)
    {
        ip6_a[i] = (s[0] << 8) + s[1];

        if (ip6_a[i])
        {
            if (zcl > zml)
            {
                zms = zcs;
                zml = zcl;
            }
            zcl = 0;
            zcs = -1;
        }
        else
        {
            zcl++;
            zcs = (zcs < 0) ? (int)i : zcs;
        }
    }

    if (zcl > zml)
    {
        zms = zcs;
        zml = zcl;
    }

    for (i = 0, si = 0; i < sizeof(ip6_a)/sizeof(ip6_a[0]); i++)
    {
        if ((int)i == zms)
        {
            si += sprintf(ip6_addr_sz + si, ":%s", si ? "" : ":");
            i += zml - 1;
        }
        else
        {
            si += sprintf(ip6_addr_sz + si, "%x%s", ip6_a[i], (i == sizeof(ip6_a)/sizeof(ip6_a[0]) - 1) ? "" : ":");
        }
    }

    ipcom_printf("%s", ip6_addr_sz);

    return 0;
}

/*
 *===========================================================================
 *                    ipcom_forwarder_print_stats
 *===========================================================================
 * Context:     shell
 * Description: Print statistics from an interface
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipcom_forwarder_print_stats(void *port_cookie, char *ifname, int reset)
{
    port_statistics_t ps;

    if (reset)
    {
        ipcom_forwarder_reset_statistics(port_cookie);
        return;
    }

    ipcom_forwarder_get_statistics(port_cookie, &ps);

    ipcom_printf("%-8s\n", ifname);

    ipcom_printf("%-8sTotal Rx:     %lu (%llu)\n", "", ps.packets,
                 (Ip_u64)ps.octets - (4 * ps.packets)); /* 4 => Do not count CRC bytes in packets */
    ipcom_printf("%-8sSW handled:   %lu (%lu)\n%-8sHW offloaded: %lu (%lu)\n",
                 "",
                 ps.non_fw_frames,
                 ps.non_fw_bytes,
                 "",
                 ps.packets - ps.non_fw_frames,
                 (Ip_u64)ps.octets - (4 * ps.packets) - ps.non_fw_bytes); /* 4 => Ethernet CRC is included  in 'octets' but not in 'non_fw_bytes' */

    ipcom_printf("%-8sFrames size:  64: %lu, 127: %lu, 255: %lu, 511: %lu, 1023: %lu, 1519: %lu\n",
                 "",
                 ps.len_64_packets,
                 ps.len_65_127_packets,
                 ps.len_128_255_packets,
                 ps.len_256_511_packets,
                 ps.len_512_1023_packets,
                 ps.len_1024_1518_packets,
                 ps.len_1519_max_packets);

    ipcom_printf("%-8sBc: %lu, Mc: %lu, Dr: %lu (%lu)\n",
                 "",
                 ps.broadcast_packets,
                 ps.multicast_packets,
                 ps.dropped_packets,
                 ps.dropped_octets);
    ipcom_printf("\n");
}

/*
 *===========================================================================
 *                    ipcom_forwarder_fwctrl
 *===========================================================================
 * Context:     shell
 * Description: Forwarder control command
 * Parameters:
 * Returns:     0
 *
 */
IP_STATIC int
ipcom_forwarder_fwctrl(int argc, char **argv)
{
    int i;
    char *e;
    int id = 0;
    char name[25];
    volatile int *rc = ipnet_usr_sock_get_route_cache_id();

    int fw_enable = ipcom_forwarder_get_fw_enable();
    int fw_started = ipcom_forwarder_get_fw_started();


entry:
    if (argc < 2)
    {
        ipcom_printf("Usage  fwctrl fw [enable|disable|start|stop]\n");
        ipcom_printf("       fwctrl stats [<ifname>] [reset]\n");
        ipcom_printf("       fwctrl cache properties\n");
        ipcom_printf("       fwctrl cache show [-4] [-6]\n");
        ipcom_printf("       fwctrl flow show\n");
        ipcom_printf("       fwctrl flow invalidate -4 <entry>|-6 <entry>|all\n");
#ifdef IPCOM_USE_FORWARDER_IPSEC
        ipcom_printf("       fwctrl sa show\n");
#endif
        ipcom_printf("       fwctrl seed [<value>]\n");
        ipcom_printf("       fwctrl util\n");
        return 0;
    }

    if (!ipcom_strcmp(argv[1], "fw"))
    {
        if (argc < 3)
        {
            ipcom_printf("fw is \'%s\' and \'%s\'\n", fw_enable ? "enabled" : "disabled", fw_started ? "started" : "stopped");
            return 0;
        }
        if (!ipcom_strcmp(argv[2], "enable"))
        {
            fw_enable = 1;
            return 0;
        }
        if (!ipcom_strcmp(argv[2], "disable"))
        {
            fw_enable = 0;
            (*rc)++; /* Increment global "route cache instance" => invalidate all entries in fw cache */
            return 0;
        }
        if (!ipcom_strcmp(argv[2], "start") && !fw_started)
        {
            ipcom_forwarder_enter_poll();
            ipcom_forwarder_start_stop_fw(IPCOM_SIG_START_FW);
            return 0;
        }
        if (!ipcom_strcmp(argv[2], "stop") && fw_started)
        {
            ipcom_forwarder_start_stop_fw(IPCOM_SIG_STOP_FW);
            ipcom_forwarder_exit_poll();
            return 0;
        }
        argc = 0;

        goto entry;
    }

    if (!ipcom_strcmp(argv[1], "seed"))
    {
        Ip_u32 seed;
        if (argc > 2)
        {
            seed = ipcom_strtoul(argv[2], &e, 0);
            ipcom_forwarder_set_seed(seed);
        }
        ipcom_printf("seed: %x\n", ipcom_forwarder_get_seed());
        return 0;
    }
    if (!ipcom_strcmp(argv[1], "util"))
    {
        ipcom_printf("util: %d%%\n", ipcom_forwarder_cache_util(IPCOM_FWD_V4_V6));
        return 0;
    }

    if (!ipcom_strcmp(argv[1], "stats"))
    {
        int reset = 0;

        name[0] = '\0';
        if (argc > 2 && ipcom_strcmp(argv[2], "reset"))
        {
            ipcom_strcpy(name, argv[2]);
        }
        if ((argc > 2 && !ipcom_strcmp(argv[2], "reset")) || (argc > 3 && !ipcom_strcmp(argv[3], "reset")))
        {
            reset = 1;
        }

        {
            unsigned int j;
            char ifname[IP_IFNAMSIZ];
            void * fwd;

            for (j = 0; j < MAX_FW_PORTS; ++j)
            {
                fwd = ipcom_forwarder_get_port_cookie(j);
                if (fwd == IP_NULL ||
                    ipcom_forwarder_port_name_get(fwd, ifname) != 0)
                    continue;
                
                if (name[0] == '\0' || ipcom_strcmp(name, ifname) == 0)
                    ipcom_forwarder_print_stats(fwd, ifname, reset);
            }
        }

#ifdef IPCOM_USE_FORWARDER_IPSEC
        if (reset == 0)
        {
            ipcom_printf("IPsec packets (out) ok:      %llu\n", ipsec_out_ok);
            ipcom_printf("                    skipped: %llu\n", ipsec_out_skip);
            ipcom_printf("                    failed:  %llu\n", ipsec_out_fail);
            ipcom_printf("IPsec packets (in)  ok:      %llu\n", ipsec_in_ok);
            ipcom_printf("                    skipped: %llu\n", ipsec_in_skip);
            ipcom_printf("                    failed:  %llu\n", ipsec_in_fail);
            ipcom_printf("IPsec packets (recrypt) ok:     %llu\n", ipsec_recrypt_ok);
            ipcom_printf("                        failed: %llu\n", ipsec_recrypt_fail);
        }
        else
        {
            ipsec_out_ok   = 0;
            ipsec_out_skip = 0;
            ipsec_out_fail = 0;
            ipsec_in_ok    = 0;
            ipsec_in_skip  = 0;
            ipsec_in_fail  = 0;
            ipsec_recrypt_ok   = 0;
            ipsec_recrypt_fail = 0;
        }
#endif

        return 0;
    }

    if (argc < 3)
    {
        argc = 0;

        goto entry;
    }

    if (!ipcom_strcmp(argv[1], "cache"))
    {
        /* cache properties */
        if (!ipcom_strcmp(argv[2], "properties"))
        {
            ipcom_printf("IPv4:       entries: %d, buckets: %d",
                         ipcom_fw.fw_v4_entries * IPCOM_FORWARDER_TRUE_CACHE_WAYS, ipcom_fw.fw_v4_entries);
            if (IPCOM_FORWARDER_CACHE_WAYS > IPCOM_FORWARDER_TRUE_CACHE_WAYS)
            {
                ipcom_printf(", linear probe: %d\n", IPCOM_FORWARDER_CACHE_WAYS);
            }
            else
            {
                ipcom_printf(", ways: %d\n", IPCOM_FORWARDER_TRUE_CACHE_WAYS);
            }

            ipcom_printf("IPv6, VLAN: entries: %d, buckets: %d",
                         ipcom_fw.fw_v6_entries * IPCOM_FORWARDER_TRUE_CACHE_WAYS, ipcom_fw.fw_v6_entries);
            if (IPCOM_FORWARDER_CACHE_WAYS > IPCOM_FORWARDER_TRUE_CACHE_WAYS)
            {
                ipcom_printf(", linear probe: %d\n", IPCOM_FORWARDER_CACHE_WAYS);
            }
            else
            {
                ipcom_printf(", ways: %d\n", IPCOM_FORWARDER_TRUE_CACHE_WAYS);
            }
        }

        /* cache show */
        if (!ipcom_strcmp(argv[2], "show"))
        {
            int valid_v4 = 0;
            int valid_v6 = 0;

            if (argc == 3 || (argc > 3 && !ipcom_strcmp(argv[3], "-4")))
            {
                for (i = 0; i < ipcom_fw.fw_v4_entries; i += 32 / IPCOM_FORWARDER_TRUE_CACHE_WAYS)
                {
                    int j;

                        ipcom_printf("%5d: ", i);

                    for (j = 0; j < 32; j++)
                    {
                        char c = '-';
                        if (ipcom_fw.fw_cache_tbl_v4[IPCOM_FORWARDER_TRUE_CACHE_WAYS * i + j].ip4.rci == *rc)
                        {
                            c = 'X';
                            valid_v4++;
                        }
                        ipcom_printf("%s%c", (j % IPCOM_FORWARDER_TRUE_CACHE_WAYS) ? "" : " ", c);
                    }
                    ipcom_printf("\n");
                }
                ipcom_printf(" Used: %d of %d entries (%d%%), buckets: %d, %s: %d\n",
                             valid_v4,
                             ipcom_fw.fw_v4_entries * IPCOM_FORWARDER_TRUE_CACHE_WAYS,
                             valid_v4 * 100 / (ipcom_fw.fw_v4_entries * IPCOM_FORWARDER_TRUE_CACHE_WAYS),
                             ipcom_fw.fw_v4_entries,
                             (IPCOM_FORWARDER_TRUE_CACHE_WAYS == IPCOM_FORWARDER_CACHE_WAYS) ? "ways" : "linear probe",
                             IPCOM_FORWARDER_CACHE_WAYS
                    );
            }
            if ((argc > 3 && !ipcom_strcmp(argv[3], "-6")))
            {
                for (i = 0; i < ipcom_fw.fw_v6_entries; i += 32 / IPCOM_FORWARDER_TRUE_CACHE_WAYS)
                {
                    int j;

                    ipcom_printf("%5d:", i);

                    for (j = 0; j < 32; j++)
                    {
                        char c = '-';
                        if (ipcom_fw.fw_cache_tbl_v6[IPCOM_FORWARDER_TRUE_CACHE_WAYS * i + j].ip6.rci == *rc)
                        {
#ifdef IPCOM_USE_FORWARDER_VLAN
                            switch (ipcom_fw.fw_cache_tbl_v6[IPCOM_FORWARDER_TRUE_CACHE_WAYS * i + j].ip6.type)
                            {
                            case IPCOM_FORWARDER_PACKET_TYPE_IPV6:
                                c = '6';
                                break;
                            case IPCOM_FORWARDER_PACKET_TYPE_IPV4_VLAN:
                                c = 'v';
                                break;
                            case IPCOM_FORWARDER_PACKET_TYPE_IPV6_VLAN:
                                c = 'V';
                                break;
                            default:
                                c = '?';
                            }
#else
                            c = 'X';
#endif
                            valid_v6++;
                        }
                        ipcom_printf("%s%c", (j % IPCOM_FORWARDER_TRUE_CACHE_WAYS) ? "" : " ", c);
                    }
                    ipcom_printf("\n");
                }
                ipcom_printf(" Used: %d of %d entries (%d%%), buckets: %d, %s: %d\n",
                             valid_v6,
                             ipcom_fw.fw_v6_entries * IPCOM_FORWARDER_TRUE_CACHE_WAYS,
                             valid_v6 * 100 / (ipcom_fw.fw_v6_entries * IPCOM_FORWARDER_TRUE_CACHE_WAYS),
                             ipcom_fw.fw_v6_entries,
                             (IPCOM_FORWARDER_TRUE_CACHE_WAYS == IPCOM_FORWARDER_CACHE_WAYS) ? "ways" : "linear probe",
                             IPCOM_FORWARDER_CACHE_WAYS
                    );
            }

        }
    }


    /* Flow command */
    if (!ipcom_strcmp(argv[1], "flow"))
    {
        /* flow show */
        if (!ipcom_strcmp(argv[2], "show"))
        {
            int valid_v4 = 0;
            int valid_v6 = 0;

            for (i = 0; i < ipcom_fw.fw_v4_entries * IPCOM_FORWARDER_TRUE_CACHE_WAYS; i++)
            {
                if (ipcom_fw.fw_cache_tbl_v4[i].ip4.rci != *rc)
                {
                    /* Entry is not valid */
                    continue;
                }
                valid_v4++;
                if (!(i % IPCOM_FORWARDER_TRUE_CACHE_WAYS))
                {
                    ipcom_printf("%5d: ", i / IPCOM_FORWARDER_TRUE_CACHE_WAYS);
                }
                else
                {
                    ipcom_printf("       ");
                }
                ipcom_printf("->");
                ipcom_forwarder_print_ip4_addr(ipcom_fw.fw_cache_tbl_v4[i].ip4.addr);
                ipcom_printf(" next hop: ");
                ipcom_forwarder_print_mac_addr(&ipcom_fw.fw_cache_tbl_v4[i].mac_if.dst_mac_u8[2]);

#ifdef IPCOM_USE_FORWARDER_IPSEC
                if (ipcom_fw.fw_cache_tbl_v4[i].mac_if.enc[1] == IPCOM_FWD_IPSEC_ENCRYPT)
                    ipcom_printf(" -- IPsec encrypt. SA index=%d. MTU=%d",
                                 ipcom_fw.fw_cache_tbl_v4[i].ipsec.key, ipcom_fw.fw_cache_tbl_v4[i].ipsec.mtu);
                if (ipcom_fw.fw_cache_tbl_v4[i].mac_if.enc[1] == IPCOM_FWD_IPSEC_DECRYPT)
                    ipcom_printf(" -- IPsec decrypt");
#endif
                ipcom_printf("\n");
            }
            for (i = 0; i < ipcom_fw.fw_v6_entries * IPCOM_FORWARDER_TRUE_CACHE_WAYS; i++)
            {
                if (ipcom_fw.fw_cache_tbl_v6[i].ip6.rci != *rc)
                {
                    /* Entry is not valid */
                    continue;
                }
                valid_v6++;
                if (!(i % IPCOM_FORWARDER_TRUE_CACHE_WAYS))
                {
                    ipcom_printf("%5d: ", i / IPCOM_FORWARDER_TRUE_CACHE_WAYS);
                }
                else
                {
                    ipcom_printf("       ");
                }
                ipcom_printf("->");

#ifdef IPCOM_USE_FORWARDER_VLAN
                if (ipcom_fw.fw_cache_tbl_v6[i].ip6.type == IPCOM_FORWARDER_PACKET_TYPE_IPV4_VLAN)
                {
                    ipcom_forwarder_print_ip4_addr(((fw_cache_entry_vlan_t *)ipcom_fw.fw_cache_tbl_v6)[i].ipvlan.a.addr_v4);
                    ipcom_printf(" VLAN ID: 0x%x(%d)",
                                 ipcom_fw.fw_cache_tbl_v6[i].ip6.vlan_id,
                                 ipcom_fw.fw_cache_tbl_v6[i].ip6.vlan_id);
                }
                else if (ipcom_fw.fw_cache_tbl_v6[i].ip6.type == IPCOM_FORWARDER_PACKET_TYPE_IPV6_VLAN)
                {
                    ipcom_forwarder_print_ip6_addr((Ip_u32 *)&ipcom_fw.fw_cache_tbl_v6[i].ip6.addr);
                    ipcom_printf(" VLAN ID: 0x%x(%d)",
                                 ipcom_fw.fw_cache_tbl_v6[i].ip6.vlan_id,
                                 ipcom_fw.fw_cache_tbl_v6[i].ip6.vlan_id);
                }
                else
                    ipcom_forwarder_print_ip6_addr((Ip_u32 *)&ipcom_fw.fw_cache_tbl_v6[i].ip6.addr);
#else
                ipcom_forwarder_print_ip6_addr((Ip_u32 *)&ipcom_fw.fw_cache_tbl_v6[i].ip6.addr);
#endif
                ipcom_printf(" next hop: ");
                ipcom_forwarder_print_mac_addr(&ipcom_fw.fw_cache_tbl_v6[i].mac_if.dst_mac_u8[2]);

#ifdef IPCOM_USE_FORWARDER_IPSEC
                if (ipcom_fw.fw_cache_tbl_v6[i].mac_if.enc[1] == IPCOM_FWD_IPSEC_ENCRYPT)
                    ipcom_printf(" -- IPsec encrypt. SA index=%d. MTU=%d",
                                 ipcom_fw.fw_cache_tbl_v6[i].ipsec.key, ipcom_fw.fw_cache_tbl_v6[i].ipsec.mtu);
                if (ipcom_fw.fw_cache_tbl_v6[i].mac_if.enc[1] == IPCOM_FWD_IPSEC_DECRYPT)
                    ipcom_printf(" -- IPsec decrypt");
#endif
                ipcom_printf("\n");
            }

            if (ipcom_fw.fw_v4_entries)
            {
                ipcom_printf("\nValid IPv4 entries: %d of %d, %d%%\n",
                             valid_v4,
                             ipcom_fw.fw_v4_entries,
                             valid_v4 * 100 / ipcom_fw.fw_v4_entries);
            }
            if (ipcom_fw.fw_v6_entries)
            {
#ifdef IPCOM_USE_FORWARDER_VLAN
                ipcom_printf("Valid IPv6, VLAN (IPv4 and IPv6) entries: %d of %d, %d%%\n",
                             valid_v6,
                             ipcom_fw.fw_v6_entries,
                             valid_v6 * 100 / ipcom_fw.fw_v6_entries);
#else
                ipcom_printf("Valid IPv6 entries: %d of %d, %d%%\n",
                             valid_v6,
                             ipcom_fw.fw_v6_entries,
                             valid_v6 * 100 / ipcom_fw.fw_v6_entries);
#endif
            }
            ipcom_printf("\n");
        }
        /* invalidate flow */
        if (!ipcom_strcmp(argv[2], "invalidate"))
        {
            int v4_v6 = IPCOM_FWD_V4_V6;
            if (argc < 4)
            {
invalidate_usage:
                ipcom_printf("Error: too few arguments\n");
                argc = 0;

                goto entry;
            }
            id = -1;
            if (ipcom_strcmp(argv[3], "all"))
            {
                if (argc < 5)
                    goto invalidate_usage;
                if (ipcom_strcmp(argv[3], "-4") == 0)
                    v4_v6 = IPCOM_FWD_V4;
                else if (ipcom_strcmp(argv[3], "-6") == 0)
                    v4_v6 = IPCOM_FWD_V6;
                else
                {
                    argc = 0;
                    goto entry;
                }

                id = ipcom_strtol(argv[4], &e, 0);
                if (e[0] != '\0')
                {
                    ipcom_printf("Error: invalid flow <id> %s\n", argv[3]);

                    return 1;
                }
            }
            ipcom_forwarder_invalidate(id, v4_v6);
        }
    }

#ifdef IPCOM_USE_FORWARDER_IPSEC
    /* SA command */
    if (!ipcom_strcmp(argv[1], "sa") && !ipcom_strcmp(argv[2], "show"))
    {
        char *buf;

        buf = ipcom_malloc(256);
        if (buf != IP_NULL)
        {
            /* Show output SAs */
            ipcom_printf("Output SAs:\n");
            ipcom_printf("-----------\n");
            for (i = 0; i < ipcom_fw.fw_ipsec_entries; i++)
            {
                if (ipcom_fw.fw_cache_tbl_enc[i].cache_id != 0)
                {
                    ipcom_printf("%3d: ", i);
                    ipcom_forwarder_ipsec_sprint_sa(IP_FALSE, buf, &ipcom_fw.fw_cache_tbl_enc[i]);
                    ipcom_printf(buf);
                }
            }
            ipcom_printf("\n", i);

            /* Show input SAs */
            ipcom_printf("Input SAs:\n");
            ipcom_printf("-----------\n");
            for (i = 0; i < ipcom_fw.fw_ipsec_entries; i++)
            {
                if (ipcom_fw.fw_cache_tbl_dec[i].cache_id != 0)
                {
                    ipcom_printf("%3d: ", i);
                    ipcom_forwarder_ipsec_sprint_sa(IP_TRUE, buf, &ipcom_fw.fw_cache_tbl_dec[i]);
                    ipcom_printf(buf);
                }
            }

            ipcom_free(buf);
        }
    }
#endif

    return 0;
}


/*
 ****************************************************************************
 * 10                   PUBLISHED FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipcom_forwarder_cmd_add
 *===========================================================================
 * Context:     init
 * Description: Register forwarding shell command
 *
 */
IP_GLOBAL void ipcom_forwarder_cmd_add(void)
{
    (void)ipcom_shell_add_cmd("fwctrl", "fwctrl", "Control and view forwarder properties",
                              ipcom_forwarder_fwctrl,
                              IPCOM_PRIORITY_DEFAULT, IPCOM_PROC_STACK_SMALL);
}

#else
int ipcom_cmd_fwctr_empty_file;
#endif /* IPCOM_USE_FORWARDER */


/*
 *===========================================================================
 *                    End of file
 *===========================================================================
 */
