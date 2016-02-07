/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_ipsec.c,v $ $Revision: 1.77 $
 *   $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_ipsec.c,v $
 *   $Author: ulf $
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

#ifndef IPCOM_KERNEL
#define IPCOM_KERNEL
#endif
#include "ipnet_config.h"


/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#ifdef IPIPSEC2

#define IPCOM_USE_CLIB_PROTO
#include <ipcom_type.h>
#include <ipcom_cstyle.h>
#include <ipcom_clib.h>
#include <ipcom_syslog.h>

#include "ipnet_h.h"
#include "ipnet_timeout.h"
#include "ipnet_usr_sock.h"

#include <ipipsec.h>
#include <ipipsec_pfkeyv2.h>


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

IP_EXTERN void    ipipsec_ioevent_eioxflushnetif(void);
IP_EXTERN int     ipipsec_ioevent_eioxlocaladdr(Ipipsec_eio_addr *eio_addr);
IP_EXTERN int     ipipsec_ioevent_sock_key_close(Ipcom_socket *sock);


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
 *                    ipipsec_pfkeyv2_raw_input
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipnet_pfkeyv2_raw_input(Ipcom_pkt *pkt, Ipipsec_eio_output *eio_output)
{
    Ip_fd         fd;
    int           retval;
    Ipnet_socket *sock;
    Ipcom_pkt    *clone = IP_NULL;

    for (fd = 0; fd < ipnet_conf_max_sockets; fd++)
    {
        sock = ipnet->sockets[fd];
        if (sock == IP_NULL || sock->ipcom.domain != IP_AF_KEY)
            continue;

        if (eio_output != IP_NULL)
        {
            if (IP_BIT_ISFALSE(sock->ipcom.ipsec_registered, 1 << eio_output->sadb_msg_satype))
                continue;
        }

        if (clone == IP_NULL)
            clone = ipnet_pkt_clone(pkt, IP_FALSE);
        if (clone == IP_NULL)
            break;

        clone->fd = (int)sock->ipcom.fd;
        retval = ipnet_queue_received_packet(clone, sock);
        if (retval == 0)
            clone = IP_NULL;
    }

    if (clone)
        ipcom_pkt_free(clone);
    ipcom_pkt_free(pkt);
}


/*
 *===========================================================================
 *                    ipnet_sock_key_close
 *===========================================================================
 * Description: Called when closing a KEY socket
 * Parameters:  sock - The socket that is being closed
 * Returns:     0 success
 *
 */
IP_STATIC int
ipnet_sock_key_close(Ipnet_socket *sock)
{
    return ipipsec_ioevent_sock_key_close(&sock->ipcom);
}


/*
 *===========================================================================
 *                    ipnet_sock_key_send
 *===========================================================================
 * Description: Inputs a key socket message to IPSEC.
 * Parameters:  sock - The socket to use when sending.
 *              msg - The send parameters.
 *              flags - IP_MSG_xxx flags.
 * Returns:     >=0 = application bytes sent, <0 = error code.
 *
 */
IP_STATIC int
ipnet_sock_key_send(Ipnet_socket *sock,
                    IP_CONST struct Ip_msghdr *msg,
                    Ipcom_pkt *pkt)
{
    int length, ret;

    length = pkt->end - pkt->start;
    ret = ipipsec_pfkeyv2_input(&sock->ipcom, msg, pkt);
    return ret < 0 ? ret : length;
}


/*
 ****************************************************************************
 * 10                   GLOBAL FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipnet_sock_key_register
 *===========================================================================
 * Description: Registers the IP_AF_KEY domain.
 * Parameters:
 * Returns:     0 = success, <0 = error code.
 *
 */
IP_GLOBAL int
ipnet_sock_key_register(void)
{
    Ipnet_sock_ops *ops;

    ops = ipcom_calloc(1, sizeof(*ops));
    if (ops == IP_NULL)
    {
        IPCOM_LOG0(CRIT, "Could not register the IP_AF_KEY domain, out of memory");
        IP_PANIC();
        return -IP_ERRNO_ENOMEM;
    }

    ipnet_usr_sock_get_ops(ops);
    ops->domain    = IP_AF_KEY;
    ops->type      = IP_SOCK_RAW;
    ops->proto     = PF_KEY_V2;
    ops->close     = ipnet_sock_key_close;
    ops->send      = ipnet_sock_key_send;
    ops->hdr_space = 8; /* to get 64 bit alignment. */

    return ipnet_sock_register_ops(ops);
}


/*
 ****************************************************************************
 * 11                   PUBLIC FUNCTIONS
 ****************************************************************************
 */


/*
 *===========================================================================
 *                    ipcom_ipsec_ioevent
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC void *
ipcom_ipsec_ioevent(int command, void *data)
{
    long int            ret = 0;
    Ip_fd               fd;
    Ipnet_socket       *sock;
    Ipipsec_eio_output *eio_output;

    switch(command)
    {
    case IPIPSEC_EIOXFDTOSOCK:
        ip_assert(ipnet_is_stack_task());
        /* Return pointer to 'Ipcom_sock' from 'fd' */
        fd = *((Ip_fd *)data);
        if(fd == IP_INVALID_SOCKET)
            return IP_NULL;
        ip_assert(fd > 2);
        ip_assert(fd < ipnet_conf_max_sockets);
        sock = ipnet->sockets[(int)fd];
        if(sock == IP_NULL)
            return IP_NULL;
        /* Only connected sockets can have a cache (since addresses may change). */
        if (IP_BIT_ISFALSE(sock->flags, IPNET_SOCKET_FLAG_CONNECTED))
            return IP_NULL;
        return sock;

#ifdef IPCOM_USE_INET
    case IPIPSEC_EIOXIP4ID:
        ret = ipcom_atomic_add_and_return(&ipnet->ip4.id, 1);
        *(int *)data = ret;
        return IP_NULL;
#endif

    case IPIPSEC_EIOXOUTPUT:
        ip_assert(ipnet_is_stack_task());
        eio_output = data;
        ip_assert(eio_output != IP_NULL);
        switch(eio_output->sndmode)
        {
        case IPIPSEC_SNDMODE_UNICAST :
            ret = ipnet_queue_received_packet(eio_output->pkt, eio_output->sock);
            if(ret < 0)
                ipcom_pkt_free(eio_output->pkt);
            break;

        case IPIPSEC_SNDMODE_BROADCAST :
            ipnet_pfkeyv2_raw_input(eio_output->pkt, IP_NULL);
            break;

        case IPIPSEC_SNDMODE_REGISTERED :
            ipnet_pfkeyv2_raw_input(eio_output->pkt, eio_output);
            break;

        default:
            IP_PANIC();
            IPCOM_LOG1(DEBUG,
                       "IPSEC: ipcom_ipsec_ioevent(IPIPSEC_EIOXOUTPUT) :: unknown sndmode (%ld)",
                       eio_output->sndmode);
            break;
        }
        break;

    default:
        IP_PANIC();
        break;
    }

    return (void *)ret;
}


/*
 *===========================================================================
 *                    ipipsec_timeout_schedule
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC void
ipipsec_timeout_schedule(Ip_u32                     sec,
                         Ipipsec_timeout_handler    handler,
                         void                      *cookie,
                         void                     **ptmo)
{
    ip_assert(ipnet_is_stack_task());
    (void) ipnet_timeout_schedule(sec * 1000,
                                  (Ipnet_timeout_handler) handler,
                                  cookie,
                                  (Ipnet_timeout **) ptmo);
}


/*
 *===========================================================================
 *                    ipipsec_timeout_cancel
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC void
ipipsec_timeout_cancel(void *tmo)
{
    ip_assert(ipnet_is_stack_task());
    ipnet_timeout_cancel(tmo);
}


/*
 *===========================================================================
 *                    ipipsec_seconds_now
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_u32
ipipsec_seconds_now(void)
{
    ip_assert(ipnet_is_stack_task());
    return ipnet_sec_now();
}


/*
 *===========================================================================
 *                    ipipsec_timeout_to_string
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC void
ipipsec_timeout_to_string(Ipipsec_timeout_handler handler,
                          Ipipsec_timeout_to_string_f to_str)
{
#ifdef IPNET_DEBUG
    ipnet_timeout_to_string((Ipnet_timeout_handler) handler,
                            (Ipnet_timeout_to_string_f) to_str);
#else
    (void)handler;
    (void)to_str;
#endif
}

#endif /* IPIPSEC2 */


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

