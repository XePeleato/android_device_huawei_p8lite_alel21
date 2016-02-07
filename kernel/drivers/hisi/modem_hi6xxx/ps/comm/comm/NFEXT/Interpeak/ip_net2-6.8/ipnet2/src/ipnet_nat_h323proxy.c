/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_nat_h323proxy.c,v $ $Revision: 1.6 $
 *     $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_nat_h323proxy.c,v $
 *     $Author: kenneth $
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
 * Proxy (ALG) for the H.323 protocol.
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
#include <ipcom_list.h>

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

/* Lower port boundary for H.323 */
#define IPNET_NAT_H323_LOWER_EPHEMERAL_PORT_VALUE   1024

/* T1.120 standard port */
#define IPNET_NAT_T1_120_PORT                       1503

/* Timeout for RTP mappings */
#define IPNET_NAT_H323_MEDIA_TIMEOUT                15*60


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

/*
 ****************************************************************************
 * 9                   LOCAL FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipnet_nat_proxy_h245_msg
 *===========================================================================
 * Description: Track H.245 packets.
 * This function handles the H.245 call control packets.  NAT calls this function
 * after translating the address and port number in the IP and TCP headers, and
 * when the source or destination port is the H.245 port. The H.245 port was
 * negotiated during the H.225 call signaling session and registered to NAT.
 * The H.245 call control protocol is used in H.323 to negotiate call parameters
 * between two end-points.  For example, it negotiates the UDP connections for
 * the open logical channels for RTP and RTCP streams between the two endpoints.
 * In addition, it also negotiates the TCP connection for the T1.120 session.
 *
 * NOTE 1:
 * The H.245 message is ASN.1 encoded. However, due to resource and schedule
 * constraint, rather than employing an ASN.1 decoder, this function uses a simple
 * work-around alternative which appears to work just as well.  This work-around
 * strategy looks for the ip+port address in the H.245 payload by taking advantage
 * of the fact that the port number always follows immediately after the ip address.
 * Since the ALG can get the expected ip address from NAT, it can search byte by
 * byte for this ip address to locate where it is in the H.245 payload.
 *
 * Local host (L) <---------------> NAT <----------------> Global host (G)
 *
 * The tuple of IP address and TCP/UDP port number is sometimes called transport
 * address in some publications, and the same terminology will be used here.
 *
 * For an outbound packet, L may be sending its transport addresses to G to let
 * G make TCP/UDP connections to it.  Similarly, for an inbound packet, G may be
 * sending its transport addresses to L to let L make TCP/UDP connections to it.
 * Since only the L's transport addresses in the payload need to be translated,
 * it is sufficient to examine only the outbound packets.
 *
 * NOTE 2:
 * TCP sequence adjustment is not required since it is a binary substitution of
 * IP address and port number in the payload.  However, the checksum in the TCP
 * header must be adjusted when the TCP payload is modified.
 *
 * Parameters:  appdata   - pointer to application data.
 *              applen    - pointer to length of application data.
 *              param     - pointer to proxy parameters.
 * Returns:     1 = Packet modified.
 *              0 = Packet untouched.
 *             -1 = Drop packet.
 */
IP_STATIC int
ipnet_nat_proxy_h245_msg(Ip_u8 *appdata,
                         int    applen,
                         Ipnet_nat_proxy_param *param)
{
    Ipnet_nat_proxy_tuple proxy_tuple;
    Ip_u8   *data, *data_start;
    Ip_u16  port;
    Ip_u32  local_address, ip_addr;
    int     data_length;
    Ip_bool mod = IP_FALSE;

    if (param->incoming == IP_TRUE)
        return 0;
    data_length = applen;       /* length of TCP payload */
    if (data_length <= 0)
    {
        /* no payload to look at */
        return 0;
    }

    /* go to start of TCP payload */
    data = appdata;
    data_start = data;
    local_address = param->tuple.private_addr;    /* L's local ip */

    /* search for L's ip address in the H.245 payload for all possible
      TCP/UDP connections that may be negotiated.  For each one found, create
      create a static TCP/UDP control block to get the translated transport address.
      We must do this for all L's ip address found in the H.245 payload
      since we are not decoding the ASN.1 message thus don't really know
      which ones will actually be used.  A check is made to prevent creation
      of duplicate bind entries.  The ID of each created static entry is recorded
      so the ALG can delete them when the Netmeeting session is terminated */

    while ((data + 6) <= (data_start + data_length))
    {
        ip_addr = IP_GET_NTOHL(data);

        /* look for L's global address match */
        if (ip_addr == local_address)
        {
            port = IP_GET_NTOHS(data + 4);
            if (port > IPNET_NAT_H323_LOWER_EPHEMERAL_PORT_VALUE)
            {
                IPCOM_LOG2(DEBUG, "ipnet_nat_proxy_h245_msg() :: Local host address found in H245 payload. "
                                  "ip = 0x%08x, port = %d", ip_addr, port);

                /* Do not add a mapping for the TCP T1.120 port here. Should be taken care of by NAT rules */
                if (port != IPNET_NAT_T1_120_PORT)
                {
                    /* create a bind entry for this transport address if it hasn't been created yet */
                    ipcom_memset(&proxy_tuple, 0, sizeof(proxy_tuple));
                    proxy_tuple.protocol     = IP_IPPROTO_UDP;
                    proxy_tuple.private_addr = local_address;
                    proxy_tuple.private_port = port;
                    proxy_tuple.public_addr  = param->tuple.public_addr;
                    if (ipnet_nat_proxy_add_mapping(&proxy_tuple,
                                                    IPNET_NAT_H323_MEDIA_TIMEOUT,
                                                    param->mapping,
                                                    IP_FALSE,         /* NAT port = local source port */
                                                    IP_TRUE,          /* Inbound session */
                                                    IP_NULL,
                                                    IP_NULL) < 0)
                    {
                        IPCOM_LOG2(ERR, "ipnet_nat_proxy_h245_msg() :: Failed to add mapping for address = 0x%08x, port = %d",
                                         local_address, port);
                    }
                    else
                    {
                        IPCOM_LOG2(DEBUG, "ipnet_nat_proxy_h245_msg() :: Added mapping for address = 0x%08x, port = %d",
                                           local_address, port);
                    }
                }

                /* Replace L's local address with its global address in the payload */
                IP_SET_HTONL(data, param->nat_addr);
                data += 6;
                mod = IP_TRUE;

                IPCOM_LOG1(DEBUG, "ipnet_nat_proxy_h245_msg() :: "
                                  "Translated local address is (0x%08x)",
                                  param->nat_addr);
            }
            else    /* detected port number is not a valid ephemeral port value */
            {
                IPCOM_LOG1(INFO, "ipnet_nat_proxy_h245_msg() :: detected port invalid, ignore! port = %d", port);
                data++;
            }
        }
        else    /* ip_addr != local_address */
        {
            data++;
        }
    }   /* end while loop */

    return mod == IP_TRUE ? 1 : 0;
}


/*
 *===========================================================================
 *                    ipnet_nat_proxy_h245
 *===========================================================================
 * Description: NAT proxy for the H.245 protocol.
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
IP_STATIC int
ipnet_nat_proxy_h245(Ip_u8 *newhdr,
                    Ip_u8 *appdata,
                    int   *applen,
                    int    growspace,
                    Ipnet_nat_proxy_param *param,
                    Ip_u8 **newdata)
{
    (void)newhdr;
    (void)growspace;
    (void)newdata;

    /* Cannot handle fragments other than the first */
    if (param->fragoff != 0)
        return 0;

    /* H.245 proxy does not apply to NAT-PT rules */
    if(param->natpt == IP_TRUE)
        return 0;

    return ipnet_nat_proxy_h245_msg(appdata, *applen, param);
}


/*
 *===========================================================================
 *                    ipnet_nat_proxy_h225_msg
 *===========================================================================
 * Description: Track H.225 packets.
 * This function handles the H.225/Q.931 call signalling packets.  NAT calls
 * this function after translating the address and port number in the IP and
 * TCP headers, and when the source or destination port is the H.323 port
 * registered to NAT during the H.323 ALG registration (the standard port for
 * H.323 protocol is 1720).  The H.225 call signalling protocol is used in H.323
 * to establish connection between two end-points.
 *
 * This function must look at both an outbound as well as an inbound packet.
 * H.323 connection can be attempted from local to global endpoint or vice
 * versa.
 *
 * NOTE 1:
 * The fields in the H.225 message are ASN.1 encoded. However, due to schedule,
 * constraint, rather than employing an ASN.1 decoder, this function uses a simple
 * strategy to look for the ip+port address in the H.225 payload by taking advantage
 * of the fact that the port number always follows immediately after the ip address.
 * Since the ALG can get the expected ip address from NAT, it can search byte by
 * byte for this ip address to locate where it is in the H.225 payload..
 *
 * Local host (L) <---------------> NAT <----------------> Global host (G)
 *
 * The tuple of IP address and TCP/UDP port number is sometimes called transport
 * address in some publications, and the same terminology will be used here.
 *
 * Case 1:  Local (L) endpoint to global (G) endpoint connection
 *
 * L starts a TCP connection to G and the H.225 call signaling session starts.
 * The establishment of this TCP connection is all handled by NAT, so by now,
 * NAT should have the bind entry for this connection.  During the H.225 session,
 * L will embed its sourceCallSignalAddress (ip/port) and G's transport address
 * in the H.225 payload to G.  Thus, this function must parse for each H.225
 * outbound packet, look for L's transport address, and substitute it with the
 * translated address obtained from NAT.
 *
 * In addition, this function must also observe the H.225 inbound packets from G
 * to look for the H.245 transport address (ip/port) in the connect message from
 * G.  The port number provided by G here will serve as the H.245 port number.
 * L will use this port number to open the H.245 TCP connection with G.  So upon
 * obtaining the H.245 port number, this function must also register the H.245
 * ALG agent to NAT .
 *
 * Case 2:  Global (G) endpoint to local (L) endpoint connection
 *
 * G starts a TCP connection to L via the NAT's H.323 static entry and the H.225
 * call signaling session starts.  As in case 1, the establishment of this TCP
 * connection is all handled by NAT, so by now, NAT should have the bind entry
 * for this connection.  During the H.225 session, G will embed its sourceCall-
 * SignalAddress and L's global transport address in the H.225 payload.  So, this
 * function must examine all inbound H.225 packets and substitute L's global
 * transport address with its real transport address.
 *
 * In addition, this function must also observe the H.225 outbound packets
 * to look for L's H.245 transport address which is embedded in the connect
 * message from L to G. The port number embedded in this message will serve as
 * the H.245 port number.
 *
 * G will use this port number to open the H.245 TCP connection with L.  So, this
 * function must also register the H.245 ALG agent to NAT upon obtaining the
 * H.245 port number.  Furthermore, since the H.245 TCP connection will be
 * started from G to L, this function must create a new TCP bind entry for the
 * impending H.245 port connection.
 *
 * NOTE 2:
 * TCP sequence adjustment is not required since it is a binary substitution of
 * IP address and port number in the payload.  However, the checksum in the TCP
 * header must be adjusted when the TCP payload is modified.
 *
 * Parameters:  appdata   - pointer to application data.
 *              applen    - pointer to length of application data.
 *              param     - pointer to proxy parameters.
 * Returns:     1 = Packet modified.
 *              0 = Packet untouched.
 *             -1 = Drop packet.
 */
IP_STATIC int
ipnet_nat_proxy_h225_msg(Ip_u8 *appdata,
                         int    applen,
                         Ipnet_nat_proxy_param *param)
{
    Ipnet_nat_proxy_tuple proxy_tuple;
    Ip_u8   *data, *data_start;
    Ip_u16  local_port, port;
    Ip_u32  remote_address, local_address, ip_addr;
    int     data_length, tmp;
    Ip_bool mod = IP_FALSE;

    data_length = applen;       /* length of TCP payload */
    if (data_length <= 0)
    {
        /* no payload to look at */
        return 0;
    }

    /* go to start of TCP payload */
    data = appdata;
    data_start = data;
    local_address  = param->tuple.private_addr;
    local_port     = param->tuple.private_port;
    remote_address = param->tuple.public_addr;

    if (param->incoming == IP_FALSE)    /* outbound packet */
    {
        /* get the H.225 TCP bind descriptor using L's translated source transport
           address from the source address in the TCP/IP header.  From the bind
           descriptor, obtain L's real transport address and the session flow (i.e.
           who starts the connection first) of this connection.
        */
        if (param->inbound == IP_FALSE)     /* session started by L */
        {
            IPCOM_LOG0(DEBUG, "ipnet_nat_proxy_h225_msg() :: outbound packet, outbound session. "
                              "Look for sourceCallSignalAddress from local host.");

            while ((data + 6) <= (data_start + data_length))
            {
                ip_addr = IP_GET_NTOHL(data);

                /* look for L's sourceCallSignalAddress match */
                if (ip_addr == local_address)
                {
                    port = IP_GET_NTOHS(data + 4);

                    if (port == local_port)
                    {
                        /* replace with L's translated sourceCallSignalAdress */
                        IPCOM_LOG2(DEBUG, "ipnet_nat_proxy_h225_msg() :: "
                                          "Replace local address and port (0x%08x:%d) in H225 payload.",
                                           ip_addr, port);

                        IP_SET_HTONL(data, param->nat_addr);
                        IP_SET_HTONS(data + 4, param->nat_port);
                        data += 6;
                        mod = IP_TRUE;

                        IPCOM_LOG2(DEBUG, "ipnet_nat_proxy_h225_msg() :: "
                                          "Translated local address and port are (0x%08x:%d)",
                                           param->nat_addr, param->nat_port);
                    }
                    else    /* port != bind_info.local_transport */
                    {
                        data++;

                        IPCOM_LOG0(DEBUG, "ipnet_nat_proxy_h225_msg() :: matched local address found, "
                                          "but not the port number.");
                    }
                }
                else    /* ip_addr != bind_info.local_addr */
                {
                    data++;
                }
            }
        }
        else    /* session started by G */
        {
            IPCOM_LOG0(DEBUG, "ipnet_nat_proxy_h225_msg() :: outbound packet, inbound session. "
                              "Look for H245Address from local host.");

            while ((data + 6) <= (data_start + data_length))
            {
                ip_addr = IP_GET_NTOHL(data);

                /* look for L's H245Address port */
                if (ip_addr == local_address)
                {
                    port = IP_GET_NTOHS(data + 4);

                    IPCOM_LOG2(DEBUG, "ipnet_nat_proxy_h225_msg() :: "
                                      "Found H245Address (0x%08x:%d) in H225 payload.",
                                       ip_addr, port);

                    /* register H.245 ALG to NAT */
                    /* if NAPT, create a H.245 TCP bind entry to prepare for H.245
                       connection request from G */

                    ipcom_memset(&proxy_tuple, 0, sizeof(proxy_tuple));
                    proxy_tuple.protocol     = param->tuple.protocol;
                    proxy_tuple.private_addr = local_address;
                    proxy_tuple.private_port = port;
                    proxy_tuple.public_addr  = remote_address;

                    tmp = ipnet_nat_proxy_add_mapping(&proxy_tuple,
                                                      0,
                                                      param->mapping,
                                                      IP_TRUE,          /* Use port translation */
                                                      IP_TRUE,          /* Inbound session */
                                                      ipnet_nat_proxy_h245,
                                                      IP_NULL);
                    if (tmp < 0)
                    {
                        IPCOM_LOG2(ERR, "ipnet_nat_proxy_h225_msg() :: Failed to add mapping for address = 0x%08x, port = %d",
                                         local_address, port);
                    }
                    else
                    {
                        IPCOM_LOG2(DEBUG, "ipnet_nat_proxy_h225_msg() :: Added mapping for address = 0x%08x, port = %d",
                                           local_address, port);
                    }

                    IP_SET_HTONL(data, param->nat_addr);
                    IP_SET_HTONS(data + 4, (Ip_u16)tmp);
                    data += 6;
                    mod = IP_TRUE;

                    IPCOM_LOG2(DEBUG, "ipnet_nat_proxy_h225_msg() :: "
                                      "Translated local address and port are (0x%08x:%d)",
                                       param->nat_addr, tmp);

                }
                else    /* ip_addr != bind_info.local_addr */
                {
                    data++;
                }
            }/* while */
        }   /* else: session started by G */
    }
    else    /* inbound packet */
    {
        if (param->inbound == IP_FALSE)     /* session started by L */
        {

            IPCOM_LOG0(DEBUG, "ipnet_nat_proxy_h225_msg() :: inbound packet, outbound session. "
                              "Look for H245Address from remote host.");

            while ((data + 6) <= (data_start + data_length))
            {
                ip_addr = IP_GET_NTOHL(data);

                /* look for G's H245Address match */
                if (ip_addr == remote_address)
                {
                    port = IP_GET_NTOHS(data + 4);

                    IPCOM_LOG2(DEBUG, "ipnet_nat_proxy_h225_msg() :: "
                                      "H245Address from remote host found (0x%08x:%d).",
                                       ip_addr, port);

                    /* register H.245 ALG to NAT */
                    ipcom_memset(&proxy_tuple, 0, sizeof(proxy_tuple));
                    proxy_tuple.protocol     = param->tuple.protocol;
                    proxy_tuple.public_addr  = ip_addr;
                    proxy_tuple.public_port  = port;
                    proxy_tuple.private_addr = param->tuple.private_addr;

                    if (ipnet_nat_proxy_add_mapping(&proxy_tuple,
                                                    0,
                                                    param->mapping,
                                                    IP_TRUE,          /* Use port translation */
                                                    IP_FALSE,         /* Outbound session */
                                                    ipnet_nat_proxy_h245,
                                                    IP_NULL) < 0)
                    {
                        IPCOM_LOG2(ERR, "ipnet_nat_proxy_h225_msg() :: Failed to add mapping for address = 0x%08x, port = %d",
                                         remote_address, port);
                    }
                    else
                    {
                        IPCOM_LOG2(DEBUG, "ipnet_nat_proxy_h225_msg() :: Added mapping for address = 0x%08x, port = %d",
                                           remote_address, port);
                    }
                }
                data++;
            }
        }
        else    /* session started by G */
        {
            /* search for L's transport address in the H225 payload.
               If found, translate it to its real transport address. */
            IPCOM_LOG0(DEBUG, "ipnet_nat_proxy_h225_msg() :: inbound packet, inbound session."
                              "Translate global transport to its local transport.");

            while ((data + 6) <= (data_start + data_length))
            {
                ip_addr = IP_GET_NTOHL(data);

                /* look for L's translated address match */
                if (ip_addr == param->nat_addr)
                {
                    port = IP_GET_NTOHS(data + 4);

                    IPCOM_LOG2(DEBUG, "ipnet_nat_proxy_h225_msg() :: "
                                      "global ip match found in H.225 payload (0x%08x:%d).",
                                       ip_addr, port);

                    if (port == param->nat_port)
                    {
                        IP_SET_HTONL(data, local_address);
                        IP_SET_HTONS(data + 4, local_port);
                        data += 6;
                        mod = IP_TRUE;

                        IPCOM_LOG2(DEBUG, "ipnet_nat_proxy_h225_msg() :: "
                                          "After translation local_address and port are (0x%08x:%d).",
                                           local_address, local_port);
                    }
                    else    /* port != bind_info.global_transport */
                    {
                        data++;
                    }
                }
                else    /* ip_addr != bind_info.global_addr */
                {
                    data++;
                }
            }   /* while */
        }   /* else (session started by G) */
    }

    return mod == IP_TRUE ? 1 : 0;
}


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
 *                    ipnet_nat_proxy_h323
 *===========================================================================
 * Description: NAT proxy for the H.323 protocol.
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
ipnet_nat_proxy_h323(Ip_u8 *newhdr,
                    Ip_u8 *appdata,
                    int   *applen,
                    int    growspace,
                    Ipnet_nat_proxy_param *param,
                    Ip_u8 **newdata)
{
    (void)newhdr;
    (void)growspace;
    (void)newdata;

    /* Cannot handle fragments other than the first */
    if (param->fragoff != 0)
        return 0;

    /* H.323 proxy does not apply to NAT-PT rules */
    if(param->natpt == IP_TRUE)
        return 0;

    return ipnet_nat_proxy_h225_msg(appdata, *applen, param);
}

#else
int ipnet_nat_h323proxy_empty_file;
#endif /* IPNET_USE_NAT */


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
