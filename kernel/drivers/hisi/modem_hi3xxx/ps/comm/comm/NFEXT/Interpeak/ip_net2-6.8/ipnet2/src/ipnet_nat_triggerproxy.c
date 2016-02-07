/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_nat_triggerproxy.c,v $ $Revision: 1.5 $
 *     $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_nat_triggerproxy.c,v $
 *     $Author: markus $
 *     $State: Exp $ $Locker:  $
 *
 *     INTERPEAK_COPYRIGHT_STRING
 *     Design and implementation by Markus Carlstedt <markus@interpeak.se>
 ******************************************************************************
 */

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 * Proxy (ALG) for port triggering.
 */

/*
 ****************************************************************************
 * 2                    CONFIGURATION
 ****************************************************************************
 */

#ifndef IPCOM_KERNEL
#define IPCOM_KERNEL
#endif
#include "ipnet_config.h"


/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#ifdef IPNET_USE_NAT

#define IPCOM_USE_CLIB_PROTO
#include <ipcom_clib.h>
#include <ipcom_sock.h>
#include <ipcom_inet.h>
#include <ipcom_syslog.h>

#include "ipnet_nat.h"


/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

#ifndef IPCOM_SYSLOG_PRIORITY
#define IPCOM_SYSLOG_PRIORITY    IPNET_SYSLOG_PRIORITY
#endif
#ifndef IPCOM_SYSLOG_FACILITY
#define IPCOM_SYSLOG_FACILITY    IPCOM_LOG_IPNET
#endif


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

/* Example of port triggering usage. The port 12345 will be opened for
   incoming connections for 2 minutes (120 seconds) */
Ipnet_nat_trigger example_trigger = {IP_IPPROTO_TCP,
                                     12345,
                                     12345,
                                     120,
                                     IP_NULL,
                                     IP_NULL};

/*
 ****************************************************************************
 * 9                   LOCAL FUNCTIONS
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 10                    GLOBAL FUNCTIONS
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 11                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipnet_nat_proxy_trigger
 *===========================================================================
 * Description: NAT proxy for port triggering.
 * Parameters:  newhdr    - pointer to optionally write new IP header.
 *              appdata   - pointer to application data.
 *              applen    - pointer to length of application data.
 *              growspace - space available to extend application data.
 *              param     - pointer to proxy parameters.
 *              newdata   - pointer to pointer to new application data.
 * Returns:     1 = Packet modified.
 *              0 = Packet untouched.
 *             -1 = Drop packet.
 */
IP_PUBLIC int
ipnet_nat_proxy_trigger(Ip_u8 *newhdr,
                        Ip_u8 *appdata,
                        int   *applen,
                        int    growspace,
                        Ipnet_nat_proxy_param *param,
                        Ip_u8 **newdata)
{
    Ipnet_nat_trigger *trigger;
    Ipnet_nat_proxy_tuple proxy_tuple;
    Ip_u32 addr;
    Ip_u16 port;
#ifdef IPCOM_USE_SYSLOG
    char buf[16];
#endif

    (void)newhdr;
    (void)appdata;
    (void)applen;
    (void)growspace;
    (void)newdata;

    /* Cannot handle fragments other than the first */
    if (param->fragoff != 0)
        return 0;

    /* TRIGGER proxy only applicable for outgoing packets */
    if(param->incoming == IP_FALSE)
    {
        /* Sanity check */
        trigger = param->cookie;
        if (trigger == IP_NULL
            || (trigger->protocol != IP_IPPROTO_TCP && trigger->protocol != IP_IPPROTO_UDP)
            || (trigger->portlo > trigger->porthi))
        {
            IPCOM_LOG0(ERR, "ipnet_nat_proxy_trigger() :: trigger failed sanity check!");
            return 0;
        }

        for (port = trigger->portlo; port <= trigger->porthi; port++)
        {
            /* Open the ports for incoming traffic */
            addr = ip_ntohl(param->tuple.private_addr);
            ipcom_memset(&proxy_tuple, 0, sizeof(proxy_tuple));
            proxy_tuple.protocol     = trigger->protocol;
            proxy_tuple.private_addr = param->tuple.private_addr;
            proxy_tuple.private_port = port;
            if (ipnet_nat_proxy_add_mapping(&proxy_tuple,
                                            trigger->timeout,
                                            param->mapping,
                                            IP_FALSE,         /* NAT port = local source port */
                                            IP_TRUE,          /* Inbound session */
                                            trigger->func,
                                            trigger->cookie) < 0)
            {
                IPCOM_LOG3(ERR, "ipnet_nat_proxy_trigger() :: failed to open %s port %d to %s",
                            trigger->protocol == IP_IPPROTO_TCP ? "TCP" : "UDP",
                            port,
                            ipcom_inet_ntop(IP_AF_INET, &addr, buf, sizeof(buf)) != IP_NULL ? buf : "unknown");
            }
            else
            {
                IPCOM_LOG3(DEBUG, "ipnet_nat_proxy_trigger() :: opening %s port %d to %s",
                            trigger->protocol == IP_IPPROTO_TCP ? "TCP" : "UDP",
                            port,
                            ipcom_inet_ntop(IP_AF_INET, &addr, buf, sizeof(buf)) != IP_NULL ? buf : "unknown");
            }
        }
    }
    return 0;
}

#else
int ipnet_nat_triggerproxy_empty_file;
#endif /* IPNET_USE_NAT */


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
