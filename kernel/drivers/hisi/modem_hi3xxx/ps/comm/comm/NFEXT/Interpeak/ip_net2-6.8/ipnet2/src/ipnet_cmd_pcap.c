/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_cmd_pcap.c,v $ $Revision: 1.3 $
 *   $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_cmd_pcap.c,v $
 *   $Author: rime $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Kenneth Jonsson <kenneth.jonsson@windriver.com>
 ******************************************************************************
 */


/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 * Shell command to start/stop packet capture on a network interface.
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

#ifdef IPNET_USE_PCAP

#define IPCOM_USE_CLIB_PROTO
#include <ipcom_clib.h>
#include <ipcom_cstyle.h>
#include <ipcom_getopt.h>
#include <ipcom_type.h>

#include "ipnet_cmd.h"
#include "ipnet_pcap.h"


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

/*
 ****************************************************************************
 * 8                    DATA
 ****************************************************************************
 */

IP_STATIC Ipcom_cmd_int_str_map ipnet_cmd_pcap_op[] = {
    { IPNET_PCAP_OP_START, "start" },
    { IPNET_PCAP_OP_STOP,  "stop"  },
    { -1,                  IP_NULL }
};


/*
 ****************************************************************************
 * 9                    STATIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipnet_cmd_pcap_print_usage
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipnet_cmd_pcap_print_usage(void)
{
    ipcom_printf(IP_LF);
    ipcom_printf("NAME" IP_LF);
    ipcom_printf("     pcap - start/stop packet capture on a network interface" IP_LF);
    ipcom_printf(IP_LF);
    ipcom_printf("SYNOPSIS" IP_LF);
    ipcom_printf("     pcap [ -f <filename> ] <ifname> start|stop" IP_LF);
    ipcom_printf(IP_LF);
    ipcom_printf("DESCRIPTION" IP_LF);
    ipcom_printf("     All packets sent/received on the interface will be written" IP_LF);
    ipcom_printf("     to a file that can be imported into Wireshark (formerly" IP_LF);
    ipcom_printf("     as Ethereal)" IP_LF);
    ipcom_printf(IP_LF);
    ipcom_printf("     start   Start capture on an interface." IP_LF);
    ipcom_printf("     stop    Stop capture on an interface." IP_LF);
    ipcom_printf("     -f fn   'fn' is the name of the file where all captured packets" IP_LF);
    ipcom_printf("             will be stored." IP_LF);
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
 *                    ipnet_cmd_pcap
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipnet_cmd_pcap(int argc, char **argv)
{
    Ipcom_getopt       opt;
    int                i;
    int                sock = IP_INVALID_SOCKET;
    struct Ip_ifreq    ifreq;
    Ipnet_pcap_ioctl_t pcap;
    Ip_u16             port = 0;

    ipcom_memset(&ifreq, 0, sizeof(ifreq));
    ipcom_memset(&pcap, 0, sizeof(pcap));
    ifreq.ip_ifr_data = &pcap;

    ipcom_getopt_clear_r(&opt);
    while ((i = ipcom_getopt_r(argc, argv, "f:a:p:", &opt)) != -1)
    {
        switch (i)
        {
        case 'a':
            if (ipcom_getsockaddrbyaddr(IP_AF_INET, opt.optarg, &pcap.d.dst.sa) < 0
                && ipcom_getsockaddrbyaddr(IP_AF_INET6, opt.optarg, &pcap.d.dst.sa) < 0)
            {
                ipcom_printf("%s is neither a valid IPv4 or IPv6 address" IP_LF, opt.optarg);
                goto cleanup;
            }
            break;
        case 'f':
            pcap.type = IPNET_PCAP_TYPE_FILE;
            ipcom_strncpy(pcap.d.filename, opt.optarg, IPNET_PCAP_FNAMSIZ);
            break;
        case 'p':
            pcap.type = IPNET_PCAP_TYPE_NET;
            port = ipcom_atoi(opt.optarg);
            port = ip_htons(port);
            break;
        }
    }

    if (argc - opt.optind < 2
        || 0 > (pcap.op = ipcom_cmd_str_to_key(ipnet_cmd_pcap_op, argv[opt.optind + 1])))
    {
        ipnet_cmd_pcap_print_usage();
        return 1;
    }

    ipcom_strncpy(ifreq.ifr_name, argv[opt.optind], IP_IFNAMSIZ);

#ifdef IPCOM_USE_INET6
    sock = ipcom_socket(IP_AF_INET6, IP_SOCK_DGRAM, 0);
#else
    sock = ipcom_socket(IP_AF_INET, IP_SOCK_DGRAM, 0);
#endif

    if (ipcom_socketioctl(sock, IP_SIOCXPCAP, &ifreq) < 0)
        ipcom_perror("ioctl(SIOCXPCAP)");

 cleanup:
    ipcom_socketclose(sock);
    return 0;
}

#endif /* IPNET_USE_PCAP */


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

