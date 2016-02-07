/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_drv_mux.c,v $ $Revision: 1.4 $
 *     $Source: /home/interpeak/CVSRoot/ipcom/port/vxworks/src/ipcom_drv_mux.c,v $
 *     $Author: kenneth $
 *     $State: Exp $ $Locker:  $
 *
 *     INTERPEAK_COPYRIGHT_STRING
 *     Design and implementation by Balaji Raghavan (braghava@windriver.com)
 ******************************************************************************
 */
/*
modification history
--------------------
01a,10jul08,brg  Created off ipcom_drv_eth.c
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


#define IPCOM_SKIP_NATIVE_SOCK_API

/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#if defined(IPNET) || defined(IPLITE)

#include "ipcom_config.h"

#ifdef IPCOM_USE_MUX_DRV

#include <vxWorks.h>
#include <semLib.h>
#include <jobQueueLib.h>
#include <netLib.h>
#include <ctype.h>
#include <endLib.h>

#include <ipcom_clib.h>
#include <ipcom_cstyle.h>
#include <ipcom_err.h>
#include <ipcom_netif.h>
#include <ipcom_os.h>
#include <ipcom_pkt.h>
#include <ipcom_shell.h>
#include <ipcom_sock2.h>
#include <ipcom_syslog.h>
#include <ipcom_type.h>
#include <ipcom_vxworks.h>


#ifdef IPNET
#include <ipnet_config.h>
#include <ipnet_netif.h>
#include <ipnet_eth.h>
/*!!should be IP_TOS_xx below */
#undef TOS_GET
#undef TOS_SET
#undef RT_PROTO_GET
#undef RT_PROTO_SET
#endif
#ifdef IPLITE
#include <iplite_config.h>
#include <iplite_netif.h>
#endif

#undef s6_addr /* Remove the ipcom definition, vxWorks 6.0 has a conflicting macro definition */
#undef ifa_dstaddr
#undef ifa_broadaddr

/* VxWorks include files */
#include <bootLib.h>
#include <end.h>
#include <endMedia.h>
#include <intLib.h>
#include <muxLib.h>
#include <muxTkLib.h>
#include <net/if.h>
#include <stdio.h>
#include <stdlib.h>
#include <sysLib.h>
#include <string.h>
#include <taskLib.h>
#include <endLib.h>

#if defined(IPCOM_VXWORKS_USE_MUX_L2) && defined(IPNET_USE_VLAN)
#include <muxL2Lib.h>
#endif

#define IPCOM_VXWORKS_ETH_IFACE_NAMES

#define IPNET_HAVE_UINT_TYPE
#if IP_PORT_VXWORKS >= 55
#define IPNET_HAVE_UINT64_TYPE
#endif

#if defined BOOT_LINE_ADRS
#define VX_BOOT_LINE ((char *)BOOT_LINE_ADRS)
#else
/* We could probably always use this case... */
IP_EXTERN char * sysBootLine;
#define VX_BOOT_LINE sysBootLine
#endif

#define _WRS_VXWORKS_VERSION ((_WRS_VXWORKS_MAJOR << 16) | \
                              (_WRS_VXWORKS_MINOR << 8) | \
                              (_WRS_VXWORKS_MAINT))

/*!! MUX2 not supported in initial 6.7 release. Only END(MUX) drivers
 * can be used*/
#undef IPCOM_USE_MUX2

#ifdef IPCOM_USE_MUX2
#include <vxAtomicLib.h>
extern void muxMblkFreeIpcomPktOutDone (int arg1, int arg2, int arg3);
#else
#if (_WRS_VXWORKS_VERSION >= 0x060000) && (_WRS_VXWORKS_VERSION < 0x060700)
#define IPCOM_HAVE_MUXTKSENDEND

/* Hmm, we also have _muxTkSendEnd(), _muxTkSendNpt()
 * for VxWorks 5.5.1 if PCD 2.0/PID 2.1/PNE 2.2 is installed,
 * and for VxWorks 5.5.2 we have equivalent _muxTkEndSend()/
 * _muxTkNptSend() functions.  How to detect those cases?
 */
#endif

#ifdef IPCOM_HAVE_MUXTKSENDEND
#include <private/muxLibP.h> /* for _muxTkSendEnd(), _muxTkSendNpt() */
#endif
#endif /* IPCOM_USE_MUX2 */

#ifndef IP_FASTTEXT
#define IP_FASTTEXT
#endif


/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

/* For testing mux drv against a loopback end device */
#define IPCOM_TEST_AGAINST_LOEND

#undef IPCOM_SYSLOG_PRIORITY
#define IPCOM_SYSLOG_PRIORITY    IPCOM_SYSLOG_IPCOM_PRIORITY
#undef IPCOM_SYSLOG_FACILITY
#define IPCOM_SYSLOG_FACILITY    IPCOM_LOG_IPCOM_DRV_ETH

/* Mux Driver   types: */
#define IPCOM_DRV_MUX_FRAME_IPV4           0x0800
#define IPCOM_DRV_MUX_FRAME_IPV6           0x86dd


#define IPCOM_DRV_MAX_BUF_DIFF 64

/*  Link layer Multicast  defines */
#define IPCOM_DRV_MUX_MAX_MULTIADDR 32
#define IPCOM_DRV_MUX_MULTIADDR_SIZE 6


/* Dummy global define needed by vxmux*/
FUNCPTR (*_func_muxIfFuncGet) (long ifType, long protocol,
                                      int  funcType);


/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

/*
 *===========================================================================
 *                         Ipcom_pdrv
 *===========================================================================
 */
typedef struct Ipcom_pdrv_struct
{
    char      drvname[IP_IFNAMSIZ];
#ifdef IPCOM_USE_MUX2
    int       drvunit;
#else
    Ip_u16    drvunit;
    Ip_u16    drvtype;              /* 0 = END, 1 = NPT */
#endif
    END_OBJ  *pEnd;                 /* END pointer */

    int       ref_count_promisc;

#ifdef IPCOM_USE_HW_CAPS
    END_CAPABILITIES cap;
#endif

#ifdef IPCOM_USE_MUX2
    atomic_t  shutdownStarted;
#endif

#ifdef IPCOM_USE_INET
    void      *mux_cookie_ipv4;
    void      *mux_cookie_arp;
#endif /* IPCOM_USE_INET */
#ifdef IPCOM_USE_INET6
    void      *mux_cookie_ipv6;
#endif

    /* Link layer multicast addresses */
    Ip_u8      mux_mlt[IPCOM_DRV_MUX_MAX_MULTIADDR][IPCOM_DRV_MUX_MULTIADDR_SIZE];

    /* Number of reference on each address, determines when we can stop
     * listen on a multicast address
     */
    Ip_u8      ref_mux_mlt[IPCOM_DRV_MUX_MAX_MULTIADDR];

}
Ipcom_pdrv;


/*
 ****************************************************************************
 * 6                    EXTERN PROTOTYPES
 ****************************************************************************
 */

/* IPNET and IPLITE implements these functions */
IP_PUBLIC void ipcom_stack_lock(void);
IP_PUBLIC void ipcom_stack_unlock(void);

#ifdef IPLITE
extern Iplite_conf_pkt_pool *iplite_conf_pkt_pool;
#endif


/*
 ****************************************************************************
 * 7                    LOCAL PROTOTYPES
 ****************************************************************************
 */

IP_STATIC int ipcom_drv_mux_ioctl(Ipcom_netif *netif, Ip_u32 command, void *data);
IP_FASTTEXT IP_STATIC int ipcom_drv_mux_output(Ipcom_netif *netif, Ipcom_pkt *pkt);

#ifdef IPCOM_USE_MUX2
IP_FASTTEXT IP_STATIC BOOL ipcom_drv_mux_recv_end2(void * arg, Ipcom_pkt * pkt);

IP_STATIC STATUS ipcom_drv_mux_shutdown_end2(PROTO_COOKIE unused, void * arg);
IP_FASTTEXT IP_STATIC STATUS ipcom_drv_mux_restart_end2(void * arg);
IP_STATIC void   ipcom_drv_mux_error_end2(void * arg, END_ERR* err);
#else
/* END */
IP_FASTTEXT IP_STATIC BOOL ipcom_drv_mux_recv_end(void *handle, long proto, M_BLK_ID pkt,
                                      LL_HDR_INFO* hdr_info, void *drv_id);

IP_STATIC STATUS ipcom_drv_mux_shutdown_end(void *end_obj, void *spare);
IP_FASTTEXT IP_STATIC STATUS ipcom_drv_mux_restart_end(void *end_obj, void *spare);
IP_STATIC void   ipcom_drv_mux_error_end(END_OBJ* end_obj, END_ERR* err, void *spare);

#endif /* IPCOM_USE_MUX2 */

IP_STATIC void   ipcom_unregister_dev_name_mapping(const char *ip_if_name);

IP_GLOBAL Ip_err ipcom_drv_mux_init(const char *drvname, Ip_u32 drvunit, const char *ifname);
IP_GLOBAL int ipcom_drv_mux_exit(Ipcom_netif *netif);
IP_GLOBAL Ip_bool ipcom_vxworks_pkt_pool_hdr_ctor(void * unused,
                                                  Ipcom_pkt *pkt);

IP_GLOBAL const char *
ipcom_mux_get_ip_if_name(const char *mux_dev_name);

IP_GLOBAL void
ipcom_mux_register_dev_name_mapping(const char *mux_dev_name, const char *ip_if_name);

IP_GLOBAL struct Ip_sockaddr *
ipnet_ip4_addr_to_sockaddr(struct Ip_sockaddr_in *sin, Ip_u32 in_addr_n);


/*
 ****************************************************************************
 * 8                    DATA
 ****************************************************************************
 */


/*
 ****************************************************************************
 * 9                    LOCAL FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipcom_drv_mux_bind_to_dev
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err
ipcom_drv_mux_bind_to_dev(const char *dev_name_unit, const char *ifname)
{
    char *dev_name;
    int pos;
    int unit;
    Ip_err rc;

    dev_name = ipcom_strdup(dev_name_unit);
    pos = ipcom_strlen(dev_name);

    while (--pos >= 0 && dev_name[pos] >= '0' && dev_name[pos] <= '9')
        ; /* empty */
    ++pos;
    unit = atoi (&dev_name[pos]); /* returns 0 if empty string */
    dev_name[pos] = 0; /* trim off unit number (if any) */

    rc = ipcom_drv_mux_init(dev_name, unit, ifname);
    if (rc != IPCOM_SUCCESS)
    {
        IPCOM_LOG2(ERR, "ipcom_drv_mux_bind_to_dev :: ipcom_drv_mux_init() failed for %s%d",
                   dev_name, unit);
    }
    ipcom_free(dev_name);

    return rc;
}


/*
 *===========================================================================
 *                    ipcom_drv_mux_ip_attach
 *===========================================================================
 * Description: This routine is called to attach IP protocols to an interface.
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipcom_drv_mux_ip_attach( char * if_conf_devname, char * if_conf_ifname )
{
    if (if_conf_devname == IP_NULL)
    {
        IPCOM_LOG1(ERR, "ipcom_drv_mux_ip_attach :: no dev_name specified for if %s",
                   if_conf_ifname ? if_conf_ifname : "no if name specified");
        return IPCOM_ERR_FAILED;
    }
    return ipcom_drv_mux_bind_to_dev(if_conf_devname, if_conf_ifname);
}


#ifdef IPCOM_VXWORKS_DRV_NULLPOOL
/*
 *===========================================================================
 *                    ipcom_drv_mux_free_out_native_pkt
 *===========================================================================
 * Description: This function is called by the mux layer when a packet has been sent.
 *              For MUX2, this function is called as the packet free function
 *              for any packet passed to mux2Send().
 * Parameters:
 * Returns:
 *
 */
#ifdef IP_DEBUG
IP_STATIC void
ipcom_drv_mux_free_out_native_pkt(Ipcom_pkt * pkt, char * file, int line)
#else
IP_STATIC void
ipcom_drv_mux_free_out_native_pkt(Ipcom_pkt * pkt)
#endif
{
    Ipcom_netif * netif = pkt->done_arg;
#ifdef IP_DEBUG
    IPCOM_UNUSED_ARG(file);
    IPCOM_UNUSED_ARG(line);
#endif

    IPCOM_DRV_ETH_STATS_LINE(free_out_pkt++);
    ipcom_pkt_output_done(netif, pkt, 0);
}

#else /* !defined(IPCOM_VXWORKS_DRV_NULLPOOL) */
/*
 *===========================================================================
 *                    ipcom_drv_mux_free_out_pkt
 *===========================================================================
 * Description: This function is called by the mux layer when a packet has been sent.
 *              Outgoing packets in IPSTACK consist of the following components:
 *                M_BLK - CL_BLK - Ipcom_pkt
 *              To be more precise, the CL_BLK points to the data section in Ipcom_pkt,
 *              this way the memcpys are kept to a minimum. The free callback pointer in
 *              CL_BLK is set to this function.
 *              The third argument holds a flag that indicates whether the Ipcom_pkt
 *              should be freed.
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipcom_drv_mux_free_out_pkt(int arg1, int arg2, int arg3)
{
    IPCOM_UNUSED_ARG(arg3);

    IPCOM_DRV_ETH_STATS_LINE(free_out_pkt++);
    ipcom_pkt_output_done((Ipcom_netif *)arg2, (Ipcom_pkt *)arg1, 0);
}


/*
 *===========================================================================
 *                    ipcom_drv_mux_get_no_pkts
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipcom_drv_mux_get_no_pkts(void)
{
    int pkts;

#ifdef IPNET
    pkts = ipnet_pkt_pool_hdr_count_hint();
#endif

#ifdef IPLITE
    const Iplite_conf_pkt_pool *pkt_pool = iplite_conf_pkt_pool;
    int index;

    pkts = 0;
    for (index = 0; pkt_pool[index].mtu != 0 || pkt_pool[index].count != 0; index++)
    {
        pkts += pkt_pool[index].count;
    }
#endif

    IPCOM_LOG1(INFO, "ipcom_drv_mux :: configured with %d ipcom packets", pkts);

    return pkts;
}
#endif /* !defined(IPCOM_VXWORKS_DRV_NULLPOOL) */


#if !defined(IPCOM_USE_MUX2) || defined (IPCOM_USE_FORWARDER)
/*
 *===========================================================================
 *                    ipcom_drv_mux_free_in_pkt_data
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipcom_drv_mux_free_in_pkt_data(void *buf, M_BLK_ID m_blk_id)
{
    (void)buf;

    netMblkClFree(m_blk_id);
}
#endif /* !defined(IPCOM_USE_MUX2) || defined (IPCOM_USE_FORWARDER) */


/*
 *===========================================================================
 *                    ipcom_drv_mux_ioctl
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipcom_drv_mux_ioctl(Ipcom_netif *netif, Ip_u32 command, void *data)
{
    int i;
    int insert = -1;
    Ipcom_pdrv *pdrv = (Ipcom_pdrv *)netif->pdrv;
    void *mux_cookie;

    ip_assert(pdrv != IP_NULL);

#ifdef IPCOM_USE_INET
    mux_cookie = pdrv->mux_cookie_ipv4;
#else
    mux_cookie = pdrv->mux_cookie_ipv6;
#endif

    switch (command)
    {
    case IP_SIOCXOPEN:
    /*
     * If the device supports EIOCGIFMEDIA, trust it if it says
     * that the link is down. Otherwise, assume the link is up.
     */
    {
        END_MEDIA media;
        i = muxIoctl (mux_cookie, EIOCGIFMEDIA, (char *)&media);
        if (i != OK ||
        (media.endMediaStatus & (IFM_AVALID|IFM_ACTIVE)) == (IFM_AVALID|IFM_ACTIVE))
                (void) netif->link_ioevent(netif, IP_EIOXRUNNING, IP_NULL, 0);
    }
        break;

    case IP_SIOCXCLOSE:
        (void) netif->link_ioevent(netif, IP_EIOXSTOP, IP_NULL, 0);
        break;

    case IP_SIOCXCHGFLAGS:
        if (mux_cookie != IP_NULL)
        {
            long dev_flags;

            /*
             * First we will turn off all flags that are disabled in the
             * request. To do so we call muxIoctl() with the flags value
             * in the request but sign-extended to be negative since this
             * is what the driver ioctl requires to clear the correct bits.
             * To change the requested flags to be negative while preserving
             * the value, we set the MSB to 1.
             */
            dev_flags = (long) netif->flags | 0x80000000;
            (void) muxIoctl(mux_cookie, EIOCSFLAGS, (caddr_t) dev_flags);

            /* Next set all flags that are set in the request. */
            dev_flags = (long) netif->flags;
            (void) muxIoctl(mux_cookie, EIOCSFLAGS, (caddr_t) dev_flags);
        }
        break;

    case IP_SIOCXDETACH:
        return ipcom_drv_mux_exit(netif);

    case IP_SIOCXADDMULTI:
        for (i = IPCOM_DRV_MUX_MAX_MULTIADDR - 1; i >= 0 ; i--)
        {
            if (pdrv->ref_mux_mlt[i] != 0)
            {
                if (ipcom_memcmp(data, pdrv->mux_mlt[i], IPCOM_DRV_MUX_MULTIADDR_SIZE) == 0)
                    break;
            }
            else
                insert = i;
        }

        if (insert < 0)
            /* Cannot join any more Linklayer multicast addresses */
            return -IP_ERRNO_EADDRNOTAVAIL;

        if (i >= 0)
        {
            pdrv->ref_mux_mlt[i]++;
            return IPCOM_SUCCESS;
        }
        if (muxMCastAddrAdd(mux_cookie, data))
            return -IP_ERRNO_EADDRNOTAVAIL;

        pdrv->ref_mux_mlt[insert]++;
        ipcom_memcpy(pdrv->mux_mlt[insert], data, IPCOM_DRV_MUX_MULTIADDR_SIZE);
        break;

    case IP_SIOCXDELMULTI:
        for (i = 0; i < IPCOM_DRV_MUX_MAX_MULTIADDR; i++)
            if (pdrv->ref_mux_mlt[i] != 0 && ipcom_memcmp(data, pdrv->mux_mlt[i],  IPCOM_DRV_MUX_MULTIADDR_SIZE) == 0)
                break;
        if (i != IPCOM_DRV_MUX_MAX_MULTIADDR && --pdrv->ref_mux_mlt[i] == 0)
        {
            ipcom_memset(pdrv->mux_mlt[i], 0, IPCOM_DRV_MUX_MULTIADDR_SIZE);
            if (muxMCastAddrDel(mux_cookie, data))
                return -IP_ERRNO_EADDRNOTAVAIL;
        }

        break;

    case IP_SIOCXPROMISC:
#ifdef IPCOM_VXWORKS_DRV_ETH_PROMISCUOUS
        if (pdrv->ref_count_promisc > 0)
            break;
#endif
        if (*(Ip_bool*)data)
        {
            if (pdrv->ref_count_promisc == 0)
            {
                int flags = IFF_PROMISC;
                if (muxIoctl(mux_cookie, (int)EIOCSFLAGS, (caddr_t)flags) != OK)
                {
                    IPCOM_LOG2(ERR, "ipcom_drv_mux_ioctl :: muxIoctl (on) failed, errno=%d (0x%x)",
                               ipcom_errno, ipcom_errno);
                    return -IP_ERRNO_EINVAL;
                }
                IP_BIT_SET(netif->flags, IP_IFF_PROMISC);
            }
            pdrv->ref_count_promisc++;
        }
        else
        {
            if (pdrv->ref_count_promisc > 0)
            {
                pdrv->ref_count_promisc--;
                if (pdrv->ref_count_promisc == 0)
                {
                    int flags = ~IFF_PROMISC;
                    if (muxIoctl(mux_cookie, (int)EIOCSFLAGS, (caddr_t)flags) != OK)
                    {
                        IPCOM_LOG2(ERR, "ipcom_drv_mux_ioctl :: muxIoctl (off) failed, errno=%d (0x%x)",
                                   ipcom_errno, ipcom_errno);
                        return -IP_ERRNO_EINVAL;
                    }
                    IP_BIT_CLR(netif->flags, IP_IFF_PROMISC);
                }
            }
        }
        break;

    case IP_SIOCXETHGINET:
        {
        /* Parse boot string and extract IP,netmask,gw information */
        return -IP_ERRNO_EINVAL;
        }

    case IP_SIOCSIFLLADDR:
        {
            struct Ip_ifreq *ifreq = data;
            if (muxIoctl(mux_cookie, EIOCSADDR, (caddr_t)ifreq->ip_ifr_addr.sa_data) != OK)
                return -IP_ERRNO_EINVAL;
            break;
        }

    case IP_SIOCGIFLLADDR:
        {
            struct Ip_ifreq *ifreq = data;

            if (muxIoctl(mux_cookie, EIOCGADDR, (caddr_t)ifreq->ip_ifr_addr.sa_data) != OK)
                return -IP_ERRNO_EINVAL;
            break;
        }
    default:
        return -IP_ERRNO_EINVAL;
    }

    return 0;
}


/*
 *===========================================================================
 *                    ipcom_drv_mux_bind
 *===========================================================================
 * Description: Local helper routine to bind END drivers to the stack.
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void *
ipcom_drv_mux_bind(int drvtype,
                   const char *drvname, Ip_u32 drvunit,
                   void *RcvRtn,
                   void *ShutdownRtn,
                   void *TxRestartRtn,
                   void *ErrorRtn,
                   long type, char *desc, void *arg)
{
    void *cookie;

#ifdef IPCOM_USE_MUX2
    cookie = mux2Bind((char *)drvname, drvunit,
              RcvRtn, ShutdownRtn, TxRestartRtn, ErrorRtn,
              type, desc, arg);

    if (cookie == IP_NULL)
    {
        IPCOM_LOG4(ERR,
           "ipcom_drv_mux_bind :: mux2Bind failed for %s, "
           "drv=%s, unit=%d, errno=%x",
                   desc, drvname, drvunit, ipcom_errno);
    }
#else
    if (drvtype)
        cookie = IP_NULL; /* No support for NPT */
    else
        cookie = muxBind((char *)drvname, drvunit,
                         RcvRtn, ShutdownRtn, TxRestartRtn, ErrorRtn,
                         type, desc, arg);
    if (cookie == IP_NULL)
    {
        IPCOM_LOG5(ERR, "ipcom_drv_mux_bind :: %s failed for %s, drv=%s, unit=%d, errno=%x",
                   (drvtype) ? "muxTkBind" : "muxBind",
                   desc, drvname, drvunit, ipcom_errno);
    }
#endif /* IPCOM_USE_MUX2 */

    return cookie;
}

/*
 *===========================================================================
 *                    ipcom_drv_mux_unbind
 *===========================================================================
 * Description: Local helper routine to unbind END  drivers to the stack.
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipcom_drv_mux_unbind(long type,
                     const char *drvname, Ip_u32 drvunit,
                     void *RcvRtn,
                     char *desc,
                     void *cookie)
{
    STATUS  ret;

    if (cookie == IP_NULL)
        return;

    ipcom_errno = 0;

    ret = muxUnbind(cookie, type, RcvRtn);
    if (ret != OK)
    {
        IPCOM_LOG4(ERR, "ipcom_drv_mux_unbind :: muxUnbind failed for %s, drv=%s, unit=%d, errno=%x",
                   desc, drvname, drvunit, ipcom_errno);
    }
}

/*
 *===========================================================================
 *                    ipcom_drv_mux_init
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL Ip_err
ipcom_drv_mux_init(const char *drvname, Ip_u32 drvunit, const char *ifname)
{
    Ipcom_netif     *netif;
    Ipcom_pdrv      *pdrv;
    END_OBJ         *pEnd;
    FUNCPTR          recv_rtn;
    FUNCPTR          shutdown_rtn;
    FUNCPTR          restart_rtn;
    VOIDFUNCPTR      error_rtn;
    M2_INTERFACETBL *pMib2Tbl = IP_NULL;
    int              drvtype;
    void            *mux_cookie;
    char             drv_ifname [IP_IFNAMSIZ];
    char            *oldifname;
    unsigned int     flags;
    END_MEDIA        media;
#if IP_PORT_VXWORKS < 67 && defined(END_MIB_2233)
    M2_ID           *pM2Id    = IP_NULL;
#endif

    /* Get the END_OBJ associated with the pDevice and unit */
    if ((pEnd = endFindByName ((char *)drvname, drvunit)) == NULL)
    {
        IPCOM_LOG3(ERR, "ipcom_drv_mux_init :: endFindByName failed for IPv4, drv=%s, unit=%d, errno=%x",
                   drvname, drvunit, ipcom_errno);
        return IPCOM_ERR_INVALID_ARG;
    }

    if (((ifname != IP_NULL) && ipcom_strlen(ifname) >= IP_IFNAMSIZ) ||
        (ipcom_snprintf (drv_ifname, sizeof (drv_ifname),
                         "%s%d", drvname, drvunit) >= sizeof (drv_ifname)))
    {
        IP_PANIC();
        return IPCOM_ERR_INVALID_ARG;
    }

    /* Check if the END is already attached to IPCOM */
    oldifname = (char *)ipcom_mux_get_ip_if_name(drv_ifname);
    if (oldifname != IP_NULL)
    {
        if (ifname == IP_NULL || ipcom_strcmp(oldifname, ifname) == 0)
            return IPCOM_SUCCESS;
        IP_PANIC();
        return IPCOM_ERR_DUPLICATE;
    }

#ifdef IPCOM_VXWORKS_ETH_IFACE_NAMES
    if (ifname == IP_NULL || *ifname == '\0')
        ifname = drv_ifname;
#endif /* IPCOM_VXWORKS_ETH_IFACE_NAMES */

    /* Check if ifname is already assigned */
    if (ifname != IP_NULL && ipcom_if_nametoindex(ifname) != 0)
    {
        IP_PANIC();
        return IPCOM_ERR_DUPLICATE;
    }

    /* Allocate memory for the network interface structure */
    netif = ipcom_if_malloc(IP_IFT_LINK_STUB);
    if (netif == IP_NULL)
        return IPCOM_ERR_FAILED;

    if (ifname != IP_NULL)
        ipcom_strncpy(netif->name, ifname, IP_IFNAMSIZ);
    else
        ifname = netif->name;

    pdrv = ipcom_malloc(sizeof(*pdrv));
    if (pdrv == IP_NULL)
        goto fail;

    ipcom_memset(pdrv, 0, sizeof(*pdrv));

    netif->pdrv = pdrv;

    IP_BIT_SET(netif->flags, IP_IFF_SIMPLEX);

    /* Store MUX device name, unit, drvtype and pEnd pointer. */
    ipcom_strcpy(pdrv->drvname, drvname);
    pdrv->drvunit     = drvunit;
    pdrv->pEnd        = pEnd;

    /* Set up driver function pointers */
    netif->drv_ioctl  = ipcom_drv_mux_ioctl;
    netif->drv_output = ipcom_drv_mux_output;

#ifdef IPCOM_USE_MUX2
    /* END2 Driver Type Functions */
    recv_rtn     = ipcom_drv_mux_recv_end2;
    shutdown_rtn = ipcom_drv_mux_shutdown_end2;
    restart_rtn  = ipcom_drv_mux_restart_end2;
    error_rtn    = ipcom_drv_mux_error_end2;
    drvtype = -1; /* irrelevant */
#else
    /* Check to see the type of driver we have - END or NPT */

    drvtype = muxTkDrvCheck( (char *)drvname );
    pdrv->drvtype     = drvtype;

    IPCOM_LOG1(INFO, "ipcom_drv_mux_init :: drivertype attached is %d ",drvtype);

    if (!drvtype)
    {
        /* END Driver Type Functions */
        recv_rtn     = ipcom_drv_mux_recv_end;
        shutdown_rtn = ipcom_drv_mux_shutdown_end;
        restart_rtn  = ipcom_drv_mux_restart_end;
        error_rtn    = ipcom_drv_mux_error_end;
    }
    else
    {
        goto fail; /* NPT not supported */
    }
#endif /* IPCOM_USE_MUX2 */

    /* Set unit configuration. Subscribe to relevant IP frames. */
#ifdef IPCOM_USE_INET
    pdrv->mux_cookie_ipv4 = ipcom_drv_mux_bind(drvtype,
                                               (char *)drvname, drvunit, recv_rtn, shutdown_rtn, restart_rtn, error_rtn,
                                               IPCOM_DRV_MUX_FRAME_IPV4, "IPSTACK IPv4", (void *)netif);

    /* Clear Restart and error pointers. */
    restart_rtn = IP_NULL;
    error_rtn   = IP_NULL;

#endif /* IPCOM_USE_INET */

#ifdef IPCOM_USE_INET6
    pdrv->mux_cookie_ipv6 = ipcom_drv_mux_bind(drvtype,
                                               (char *)drvname, drvunit, recv_rtn, shutdown_rtn, restart_rtn, error_rtn,
                                               IPCOM_DRV_MUX_FRAME_IPV6, "IPSTACK IPv6", (void *)netif);

    /* Clear Restart and error pointers. */
    restart_rtn = IP_NULL;
    error_rtn   = IP_NULL;
#endif /* IPCOM_USE_INET6 */

#ifdef IPCOM_VXWORKS_DRV_ETH_PROMISCUOUS
    {
        Ip_bool on = 1;
        if (ipcom_drv_mux_ioctl(netif, IP_SIOCXPROMISC, &on) != IPCOM_SUCCESS)
        {
            IPCOM_LOG4(ERR, "ipcom_drv_mux_init :: ipcom_drv_mux_ioctl(IP_SIOCXPROMISC) failed for %s%d, errno=%d (0x%x)",
                       drvname, drvunit, ipcom_errno, ipcom_errno);
            goto fail;
        }
    }
#endif /* IPCOM_VXWORKS_DRV_ETH_PROMISCUOUS */

#ifdef IPCOM_USE_INET
    mux_cookie = pdrv->mux_cookie_ipv4;
#else
    mux_cookie = pdrv->mux_cookie_ipv6;
#endif

    /* Read Mux Driver layer address from driver. */
    if (muxIoctl(mux_cookie, EIOCGADDR, (caddr_t) netif->link_addr))
        /*         goto fail; */;

    /* Read driver's flags field */
    if (muxIoctl(mux_cookie, EIOCGFLAGS, (caddr_t) &flags) == OK)
    {
        if (!(flags & IFF_MULTICAST))
        {
            IP_BIT_CLR(netif->flags, IP_IFF_MULTICAST);
        }
    }
    if (muxIoctl (mux_cookie, EIOCGIFMEDIA, (char *)&media) == OK)
    {
        if ((media.endMediaStatus & (IFM_AVALID|IFM_ACTIVE)) == (IFM_AVALID|IFM_ACTIVE))
            IP_BIT_SET(netif->flags, IP_IFF_RUNNING);
        else
            IP_BIT_CLR(netif->flags, IP_IFF_RUNNING);
    }

    IPCOM_LOG2(DEBUG, "ipcom_drv_mux_init :: %s, flags = 0x%x", ifname, flags);

    pMib2Tbl = &pEnd->mib2Tbl;



#if IP_PORT_VXWORKS < 67
#ifdef END_MIB_2233
    if (pEnd->flags & END_MIB_2233)
    {
        if (muxIoctl(mux_cookie, EIOCGMIB2233, (caddr_t)&pM2Id) == OK)
            pMib2Tbl = &pM2Id->m2Data.mibIfTbl;
    }
#endif
#endif

    if (pMib2Tbl->ifMtu != 0)  /* just paranoia... */
    {
        /*
         * if driver provides a different mtu size, override the default.
         * This would be the case for smNet or driver that supports vlan
         * tagging.
         */
        if (netif->mtu != pMib2Tbl->ifMtu)
            netif->mtu = pMib2Tbl->ifMtu;
    }

#ifdef IPCOM_USE_HW_CAPS
    if (muxIoctl(mux_cookie, EIOCGIFCAP, (caddr_t) &pdrv->cap) != OK)
        ipcom_memset(&pdrv->cap, 0, sizeof(pdrv->cap));
    else
    {
#ifdef IPCOM_USE_HW_CHECKSUM
        if (IP_BIT_ISFALSE(pdrv->cap.cap_available, IFCAP_RXCSUM))
            pdrv->cap.csum_flags_rx = 0;

        if (IP_BIT_ISFALSE(pdrv->cap.cap_available, IFCAP_TXCSUM))
            pdrv->cap.csum_flags_tx = 0;
        if ((pdrv->cap.csum_flags_tx & (CSUM_TCP | CSUM_UDP)) == (CSUM_TCP | CSUM_UDP))
            IP_BIT_SET(netif->drv_capabilities, IPCOM_IF_DRV_CAP_IP4_TRANSPORT_CSUM_TX);
        if ((pdrv->cap.csum_flags_tx & (CSUM_TCPv6 | CSUM_UDPv6)) == (CSUM_TCPv6 | CSUM_UDPv6))
            IP_BIT_SET(netif->drv_capabilities, IPCOM_IF_DRV_CAP_IP6_TRANSPORT_CSUM_TX);

        IPCOM_LOG9(DEBUG, "ipcom_drv_mux_init :: %s has HW support for TX %s%s%s%s RX %s%s%s%s",
                   drvname,
                   IP_BIT_ISSET(pdrv->cap.csum_flags_tx, CSUM_TCP)   ? "TCPv4 " : "",
                   IP_BIT_ISSET(pdrv->cap.csum_flags_tx, CSUM_UDP)   ? "UDPv4 " : "",
                   IP_BIT_ISSET(pdrv->cap.csum_flags_tx, CSUM_TCPv6) ? "TCPv6 " : "",
                   IP_BIT_ISSET(pdrv->cap.csum_flags_tx, CSUM_UDPv6) ? "UDPv6 " : "",
                   IP_BIT_ISSET(pdrv->cap.csum_flags_rx, CSUM_TCP)   ? "TCPv4 " : "",
                   IP_BIT_ISSET(pdrv->cap.csum_flags_rx, CSUM_UDP)   ? "UDPv4 " : "",
                   IP_BIT_ISSET(pdrv->cap.csum_flags_rx, CSUM_TCPv6) ? "TCPv6 " : "",
                   IP_BIT_ISSET(pdrv->cap.csum_flags_rx, CSUM_UDPv6) ? "UDPv6 " : "");
#endif /* IPCOM_USE_HW_CHECKSUM */
    }
#endif /* IPCOM_USE_HW_CAPS */

    if (ipcom_if_attach(netif) < 0)
        goto fail;

    pMib2Tbl->ifIndex = netif->ifindex;

    ipcom_mux_register_dev_name_mapping(drv_ifname, ifname);


    return IPCOM_SUCCESS;

 fail:
    if (pdrv != IP_NULL)
        ipcom_free(pdrv);
    ipcom_if_free(netif);

    return IPCOM_ERR_FAILED;
}


/*
 *===========================================================================
 *                    ipcom_drv_mux_exit
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL int
ipcom_drv_mux_exit(Ipcom_netif *netif)
{
    FUNCPTR  recv_rtn;
    Ipcom_pdrv  *pdrv;
#ifndef IPCOM_USE_MUX2
    int      drvtype;
#endif

    pdrv = netif->pdrv;

    IPCOM_LOG3(INFO, "ipcom_drv_mux_exit :: detaching %s   [drv=%s, unit=%d]",
               netif->name, pdrv->drvname, pdrv->drvunit);

#ifdef IPCOM_USE_MUX2
    recv_rtn = (FUNCPTR)ipcom_drv_mux_recv_end2;
#else
    /* Check to see the type of driver we have - END or NPT */
    drvtype = muxTkDrvCheck(pdrv->drvname);
    if (!drvtype)
        recv_rtn = ipcom_drv_mux_recv_end;
    else
    {
        IP_PANIC(); /* Should never happen */
    }

#endif /* IPCOM_USE_MUX2 */

    /* Set unit configuration. Subscribe to relevant IP frames. */
#ifdef IPCOM_USE_INET
    ipcom_drv_mux_unbind(IPCOM_DRV_MUX_FRAME_IPV4,
                         pdrv->drvname, pdrv->drvunit,
                         recv_rtn,
                         "IPSTACK IPv4",
                         pdrv->mux_cookie_ipv4);

#endif /* IPCOM_USE_INET */

#ifdef IPCOM_USE_INET6
    ipcom_drv_mux_unbind(IPCOM_DRV_MUX_FRAME_IPV6,
                         pdrv->drvname, pdrv->drvunit,
                         recv_rtn,
                         "IPSTACK IPv6",
                         pdrv->mux_cookie_ipv6);
#endif /* IPCOM_USE_INET6 */


    /* Release port driver memory */
    ipcom_free(pdrv);
    netif->pdrv = IP_NULL;

    ipcom_unregister_dev_name_mapping(netif->name);

    return 0;
}


/*
****************************************************************************
* 9                   MUX CALLBACK FUNCTIONS
****************************************************************************
*/

#ifdef IPCOM_USE_MUX2
/*===========================================================================
 *                    ipcom_drv_mux_recv_end2
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_FASTTEXT IP_STATIC BOOL
ipcom_drv_mux_recv_end2(void *drv_id, Ipcom_pkt *pkt)
{
    Ipcom_netif  *netif;

    netif = (Ipcom_netif *)drv_id;

    /*
     * Apart from these two lines, which could possibly
     * be handled elsewhere, we could directly install
     * ipcom_pkt_input_queue() or ipcom_pkt_input() as
     * the MUX protocol receive routine, and save one
     * level of function call.
     *
     * Think about sharing ifindexes between the MUX
     * and the stack.
     */
    IPCOM_PKT_TRACE(pkt, IPCOM_PKT_ID_RX);
    pkt->ifindex = netif->ifindex;

#ifdef IPCOM_USE_INPUTD
    ipcom_pkt_input_queue(netif, pkt);
#else
    ipcom_pkt_input(netif, pkt);
#endif

    return TRUE;
}

/*===========================================================================
 *                    ipcom_drv_mux_detach
 *===========================================================================
 * Description: internal support routine for ipcom_drv_mux_shutdown_end2
 *              detaches from the specified interface.
 * Parameters:
 * Returns:
 *
 */

IP_STATIC void
ipcom_drv_mux_detach(int ifindex)
{
    struct Ip_ifreq ifreq;
    Ip_fd fd;

#ifdef IPCOM_USE_INET
    fd = ipcom_socket(IP_AF_INET, IP_SOCK_DGRAM, 0);
#else
    fd = ipcom_socket(IP_AF_INET6, IP_SOCK_DGRAM, 0);
#endif
    if (fd == IP_INVALID_SOCKET)
        goto err;

    ipcom_memset(&ifreq, 0, sizeof(struct Ip_ifreq));
    (void)ipcom_if_indextoname(ifindex, ifreq.ifr_name);

    if (ipcom_socketioctl(fd, IP_SIOCGIFFLAGS, &ifreq) < 0)
        goto err;

    IP_BIT_CLR(ifreq.ip_ifr_flags, IP_IFF_UP);
    if (ipcom_socketioctl(fd, IP_SIOCSIFFLAGS, &ifreq) < 0)
        goto err;

    if (ipcom_socketioctl(fd, IP_SIOCXDETACH, &ifreq) == 0)
        return;

 err:
    ipcom_printf("ipcom_drv_mux_detach: failed for %s: %s" IP_LF,
                 ifreq.ifr_name, ipcom_strerror(ipcom_errno));
    if (fd != IP_INVALID_SOCKET)
        ipcom_socketclose (fd);
    return;
}

/*===========================================================================
 *                    ipcom_drv_mux_shutdown_end2
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC STATUS
ipcom_drv_mux_shutdown_end2(PROTO_COOKIE unused, void * arg)
{
    int tid;
    Ipcom_netif * netif = arg;
    Ipcom_pdrv  *pdrv;
    atomicVal_t res;

    /*
     * A MUX2 protocol shutdown routine may return OK before it
     * actually unbinds from the device.  When the unbind happens,
     * a reference count is decremented, and when all references
     * to the END_OBJ are released, the device will be removed from
     * the MUX and the driver's unload routine will be called.
     * So, we don't actually have to complete the detach operation
     * in this context.
     *
     * Since we cannot actually unbind any of the protocols until
     * IPNet at a higher level has been told to detach from the interface,
     * we spawn a task to do that detach on the very first shutdown
     * call, and on subsequent calls we simply return OK; all protocols
     * will be unbound during the detach.
     *
     * Note, this routine executes in the network job queue of
     * the interface being shut down; the muxUnbind() calls also
     * must wait for the same job queue, to complete.
     */

    pdrv = netif->pdrv;
    res = vxAtomicSet (&pdrv->shutdownStarted, 1);
    if (res != 0)
        return OK;

    tid = taskSpawn ("tDetach", 100, 0, 10000, (FUNCPTR)ipcom_drv_mux_detach,
                     (int)netif->ifindex, 0,0,0,0,0,0,0,0,0);

    return (tid == ERROR ? ERROR : OK);
}

/*===========================================================================
 *                    ipcom_drv_mux_restart_end2
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_FASTTEXT IP_STATIC STATUS
ipcom_drv_mux_restart_end2(void *drv_id)
{
    Ipcom_netif *netif = (Ipcom_netif *)drv_id;

    ipcom_pkt_output_done(netif, IP_NULL, 0);
    IPCOM_DRV_ETH_STATS_LINE(restart++);

    return OK;
}

#else /* IPCOM_USE_MUX2 */

/*===========================================================================
 *                    ipcom_drv_mux_recv_end
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_FASTTEXT IP_STATIC BOOL
ipcom_drv_mux_recv_end(void *handle, long proto, M_BLK_ID m_blk,
                       LL_HDR_INFO* hdr_info, void *drv_id)
{
    Ipcom_netif  *netif;
    Ipcom_pkt    *pkt;

    IPCOM_DRV_ETH_STATS_LINE(recv_mbufs++);

    netif = (Ipcom_netif *)drv_id;

    /* Normally the END drivers do not pass multiple packets up, which means
     * we can ignore looping over the mNextPkt pointer. It would be more
     * likely to have multiple mblks per packet, but most of the drivers
     * do not pass multiple mblks for a single packet. */
    IPCOM_DRV_ETH_STATS_LINE(recv_pkts++);
    ip_assert(m_blk->mBlkHdr.mNext == NULL);

    /* Allocate a packet header */
    pkt = ipcom_pkt_malloc(0, IP_FLAG_FC_ISRCONTEXT);
    if (IP_UNLIKELY(pkt == IP_NULL))
    {
        IPCOM_DRV_ETH_STATS_LINE(recv_pkt_malloc_fail++);
        netMblkClFree(m_blk);
        return TRUE;        /* ??? */
    }

    /* Default to the normal case and ignore the strange buffer issue */
    pkt->start = (int)(m_blk->mBlkHdr.mData - m_blk->pClBlk->clNode.pClBuf);
    /* ipcom_pkt_malloc() already called ipcom_pkt_create() with default
     * arguments for the mtu = 0 case.  To avoid a second function call,
     * & redundant setting, we just fill in the changed arguments here.
     */
#ifdef IPCOM_TEST_AGAINST_LOEND
    /* Strip off Link layer header for input to link stub */
    pkt->start += hdr_info->dataOffset;
#endif

    pkt->data = (Ip_u8 *)m_blk->pClBlk->clNode.pClBuf;
    pkt->maxlen = m_blk->mBlkHdr.mLen + pkt->start; /* not really */
    pkt->end = m_blk->mBlkHdr.mLen + pkt->start;    /* really */
    pkt->data_freefunc =
        (Ipcom_pkt_freefunc)ipcom_drv_mux_free_in_pkt_data;
    pkt->data_freefunc_cookie = m_blk;

    IPCOM_PKT_TRACE(pkt, IPCOM_PKT_ID_RX);
    pkt->ifindex = netif->ifindex;

#ifdef IPCOM_USE_HW_CHECKSUM_RX
    if (IP_BIT_ISSET(m_blk->mBlkPktHdr.csum_flags, CSUM_DATA_VALID))
    {
        pkt->chk = m_blk->mBlkPktHdr.csum_data;
        if (IP_BIT_ISFALSE(m_blk->mBlkPktHdr.csum_flags, CSUM_PSEUDO_HDR))
            IP_BIT_SET(pkt->flags, IPCOM_PKT_FLAG_TL_CHECKSUM);
        else if (pkt->chk == 0xffff)
            IP_BIT_SET(pkt->flags, IPCOM_PKT_FLAG_HW_CHECKSUM);
    }
#endif /* IPCOM_USE_HW_CHECKSUM_RX */

#ifdef IPCOM_USE_INPUTD
    ipcom_pkt_input_queue(netif, pkt);
#else
    ipcom_pkt_input(netif, pkt);
#endif

    return TRUE;
}



/*===========================================================================
 *                    ipcom_drv_mux_shutdown_end
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC STATUS
ipcom_drv_mux_shutdown_end(void *end_obj, void *spare)
{
    /* Not implemented. Pull the emergency break. */
    IPCOM_LOG0(ERR, "ipcom_drv_mux_shutdown_end :: MUX shutdown callback not implemented\n");
    printf("ipcom_drv_mux_shutdown_end :: MUX shutdown callback not implemented\n");

    return ERROR;
}


/*===========================================================================
 *                    ipcom_drv_mux_restart_end
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_FASTTEXT IP_STATIC STATUS
ipcom_drv_mux_restart_end(void *end_obj, void *drv_id)
{
    Ipcom_netif *netif = (Ipcom_netif *)drv_id;

    ipcom_pkt_output_done(netif, IP_NULL, 0);
    IPCOM_DRV_ETH_STATS_LINE(restart++);

    return OK;
}


#endif /* IPCOM_USE_MUX2 */


/*===========================================================================
 *                    ipcom_drv_mux_error_end[2]
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
#ifdef IPCOM_USE_MUX2
ipcom_drv_mux_error_end2(void * arg, END_ERR *err)
#else
ipcom_drv_mux_error_end(END_OBJ *end_obj, END_ERR *err, void *spare)
#endif
{
#ifdef IPCOM_USE_MUX2
    Ipcom_netif *netif = (Ipcom_netif *)arg;
#else
    Ipcom_netif *netif = (Ipcom_netif *)spare;
#endif

    IPCOM_DRV_ETH_STATS_LINE(error_end++);

    IPCOM_LOG3(INFO, "ipcom_drv_mux_error_end :: %s '%s' errCode = %d",
               netif->name, err->pMesg ? err->pMesg : "", err->errCode);

    switch(err->errCode)
    {
    case END_ERR_FLAGS:
        /*
         * In case muxDevStop() brings the END down, treat that
         * as a link-down condition.  We don't want to affect the
         * administrative IP_IFF_UP flag that applies at the IPnet level.
         * (Actually, it might be OK to do so, but if muxDevStart() is
         * done later, we don't know whether or not to set IP_IFF_UP
         * again; maybe it was supposed to be administratively down.)
         * When muxDevStart() happens, we should get an END_ERR_LINKUP
         * at that time, so we don't need to handle that here.
         */
        {
            long flags = (long)err->pSpare;
            if ((flags & IFF_UP) == 0 &&
                IP_BIT_ISSET(netif->flags, IP_IFF_RUNNING))
                goto link_down;
            break;
        }

    case END_ERR_UP:
        (void)netif->link_ioevent(netif, IP_EIOXUP, IP_NULL, IP_FLAG_FC_STACKCONTEXT);
        break;

    case END_ERR_DOWN:
        (void)netif->link_ioevent(netif, IP_EIOXDOWN, IP_NULL, IP_FLAG_FC_STACKCONTEXT);
        break;

    case END_ERR_LINKDOWN:
    link_down:
        (void)netif->link_ioevent(netif, IP_EIOXSTOP, IP_NULL, IP_FLAG_FC_STACKCONTEXT);
        break;

    case END_ERR_RESET:
        /* The MAC address is reported after device reset */
        /* Fall through */

    case END_ERR_LINKUP:
        /*!! We need to call stop first to compensate for drivers with bad boot flags */
        (void)netif->link_ioevent(netif, IP_EIOXSTOP, IP_NULL, IP_FLAG_FC_STACKCONTEXT);
        (void)netif->link_ioevent(netif, IP_EIOXRUNNING, IP_NULL, IP_FLAG_FC_STACKCONTEXT);
        break;
    default:
        break;
    }
}

/*
 *===========================================================================
 *               ipcom_drv_calculate_transport_checksum
 *===========================================================================
 * Description: Calculates and writes down the transport layer checksum.
 * Parameters:  pkt - The packet containing the data.
 *                    pkt->tr_start points to the beginning of the checksum,
 *                    pkt->ipstart to the beginning of the IPv6 header
 *                    and pkt->chk contains the offset from pkt->tr_start
 *                    to where the checksum should be stored.
 * Returns:
 *
 */
#ifdef IPCOM_USE_HW_CHECKSUM_TX
IP_GLOBAL void
ipcom_drv_calculate_transport_checksum(Ipcom_pkt *pkt)
{
    Ip_u32  sum;
    Ip_u16 *chksum;

    ip_assert((pkt->tlstart & 1) == 0);
    ip_assert((pkt->chk & 1) == 0);
    ip_assert((pkt->chk + pkt->tlstart + (int) sizeof(Ip_u16)) <= pkt->end);

    chksum = (Ip_u16 *) &pkt->data[pkt->tlstart + pkt->chk];
    sum = ipcom_in_checksum_update(&pkt->data[pkt->tlstart], pkt->end - pkt->tlstart);
    *chksum = ipcom_in_checksum_finish(sum);
    /*
     * Clear IPCOM_PKT_FLAG_HW_CHECKSUM so we don't do this again if transmit
     * stalls and the packet is again passed to the driver send routine.
     */
    IP_BIT_CLR (pkt->flags, IPCOM_PKT_FLAG_HW_CHECKSUM);
}
#endif /* IPCOM_USE_HW_CHECKSUM_TX */

/*
 *===========================================================================
 *                    ipcom_drv_mux_output
 *===========================================================================
 * Description: Outputs an Mux Driver   frame.
 * Parameters:  netif - The network interface to use when sending the frame.
 *              pkt - The frame send. pkt->start is the offset to the
 *              Mux Driver   frame header.
 * Returns:     0 = success, <0 = error code.
 *
 */
IP_FASTTEXT IP_STATIC int
ipcom_drv_mux_output(Ipcom_netif *netif, Ipcom_pkt *pkt)
{
#ifndef IPCOM_USE_MUX2
    M_BLK_ID    m_blk;
    Ipcom_pkt  *pkt_part = pkt;
    Ipcom_pkt  *pkt_prev_part;
    M_BLK_ID    prev_m_blk = IP_NULL;
#endif

#ifdef IPCOM_USE_INET
    Ipnet_pkt_ip      *ip4_hdr;
    struct Ip_sockaddr_in dst_sock_addr_in;
#endif
#ifdef IPCOM_USE_INET6
    Ipnet_pkt_ip6     *ip6_hdr;
    struct Ip_sockaddr_in6 dst_sock_addr_in6;
#endif

    int         retval = ERROR;
    void        *mux_cookie;
    Ip_u16      frametype = 0,pkttype = 0;
    /* Required for mux address form */
    M_BLK       dstaddr_blk;
    M_BLK_ID    dstaddr_blk_id,srcaddr_blk_id;



    dstaddr_blk_id = &dstaddr_blk;
    srcaddr_blk_id = &dstaddr_blk;

    frametype = (Ip_u16)((((pkt->data[pkt->start] & 0xf0))==0x40)?IPCOM_DRV_MUX_FRAME_IPV4:IPCOM_DRV_MUX_FRAME_IPV6);

    if (IP_UNLIKELY(IP_BIT_ISSET(pkt->flags, IPCOM_PKT_FLAG_BROADCAST)))
    {
        pkttype = 2;
    }
    else if (IP_UNLIKELY(IP_BIT_ISSET(pkt->flags, IPCOM_PKT_FLAG_MULTICAST)))
    {
        pkttype = 1;
    }



#ifndef IPCOM_USE_MUX2
#ifdef IPCOM_VXWORKS_DRV_NULLPOOL
    /* Transform a scatter list of Ipcom_pkt to a scatter list of
       M_BLK:s */
    do
    {
        m_blk = &((Ipcom_port_pkt *)pkt_part)->ml.mBlk;

        m_blk->pClBlk->clNode.pClBuf = (char *)pkt_part->data;
        m_blk->pClBlk->clSize        = pkt_part->maxlen;
        m_blk->pClBlk->clRefCnt      = 1;
        m_blk->mBlkHdr.mData         = (char *)&pkt_part->data[pkt_part->start];
        m_blk->mBlkHdr.mLen          = pkt_part->end - pkt_part->start;
        m_blk->mBlkPktHdr.len        = m_blk->mBlkHdr.mLen;
        m_blk->mBlkPktHdr.csum_flags = 0;

        m_blk->mBlkHdr.mNext = IP_NULL;
        if (prev_m_blk)
            prev_m_blk->mBlkHdr.mNext = m_blk;
        prev_m_blk = m_blk;

        pkt_prev_part = pkt_part;
        pkt_part = pkt_part->next_part;
        pkt_prev_part->next_part = IP_NULL;
    }
    while (IP_NULL != pkt_part );

    m_blk = &((Ipcom_port_pkt *)pkt)->ml.mBlk;

#else
    CL_BLK_ID   cl_blk;

    m_blk = netMblkGet(&ipcom_port.net_pool, M_DONTWAIT, FALSE);
    if (m_blk == IP_NULL)
    {
        IPCOM_DRV_ETH_STATS_LINE(send_out_of_mbufs++);
        goto enobufs;
    }

    cl_blk = netClBlkGet(&(ipcom_port.net_pool),M_DONTWAIT);
    if (cl_blk == IP_NULL)
    {
        netMblkFree(&(ipcom_port.net_pool), m_blk);
        IPCOM_DRV_ETH_STATS_LINE(send_out_of_clblk++);
        goto enobufs;
    }

    netClBlkJoin(cl_blk, (char *)&pkt->data[pkt->start],
                 pkt->maxlen, (FUNCPTR)ipcom_drv_mux_free_out_pkt, (int)pkt, (int)netif, 1);
    netMblkClJoin(m_blk, cl_blk);
    m_blk->mBlkHdr.mLen = pkt->end - pkt->start;
    m_blk->mBlkPktHdr.len = m_blk->mBlkHdr.mLen;
    m_blk->mBlkHdr.mType = MT_DATA;

#endif /* #ifdef IPCOM_VXWORKS_DRV_NULLPOOL */

#ifdef IPCOM_USE_HW_CHECKSUM_TX
    if (IP_BIT_ISSET(pkt->flags, IPCOM_PKT_FLAG_HW_CHECKSUM))
    {
        /*
         * pkt->tlstart is the offset of the start of the transport header.
         * pkt->ipstart is the offset of the start of the IP header.
         */
        CSUM_IP_HDRLEN(m_blk) = pkt->tlstart - pkt->ipstart;
        CSUM_IPHDR_OFFSET(m_blk) = pkt->ipstart - pkt->start;

        /*
         * pkt->chk has the byte offset of the checksum field
         * from the start of the transport header.
         * CSUM_XPORT_HDRLEN() is needed only for TCP segmentation offload,
         * not supported yet.
         */
        m_blk->mBlkPktHdr.csum_data = pkt->chk; /* for CSUM_XPORT_CSUM_OFF() */
#ifdef IPCOM_USE_INET6
#ifdef IPCOM_USE_INET
        if (pkt->data[pkt->ipstart] >= 0x60)  /* if IPv6 or later */
#endif
        {
            /* Given the limited number of transport protocols
             * that support HW checksum offload, pkt->chk is
             * a good indication of what transport protocol is
             * in use.  Also, in the IPv6 case, it saves us from
             * having to chase through extension headers to find
             * the real transport protocol.
             */
            if (pkt->chk == 16)
                m_blk->mBlkPktHdr.csum_flags |= CSUM_TCPv6;
            else if (pkt->chk == 6)
                m_blk->mBlkPktHdr.csum_flags |= CSUM_UDPv6;
            else
                /* protocol not supported by VxWorks HW-driver model */
                ipcom_drv_calculate_transport_checksum(pkt);
        }
#endif /* IPCOM_USE_INET6 */
#ifdef IPCOM_USE_INET
#ifdef IPCOM_USE_INET6
        else
#endif
        {
            if (pkt->chk == 16)
                m_blk->mBlkPktHdr.csum_flags |= CSUM_TCP;
            else if (pkt->chk == 6)
                m_blk->mBlkPktHdr.csum_flags |= CSUM_UDP;
            else
                /* protocol not supported by VxWorks HW-driver model */
                ipcom_drv_calculate_transport_checksum(pkt);
        }
#endif /* IPCOM_USE_INET */
    }
#endif /* IPCOM_USE_HW_CHECKSUM_TX */
#endif /* !defined (IPCOM_USE_MUX) */

    /*
     * TODO:  Also, it's bad to store a reference to netif in the
     * M_BLK or pkt since by the time the packet is freed, netif
     * might possibly have been detached (?), in which case the netif
     * reference would be obsolete. Granted, this would be rare.
     * It could also be avoided by increasing the reference count
     * on the netif.
     * pkt contains pkt->vr_index and pkt->ifindex from which
     * netif could be recovered, with more work, but that
     * might impact performance.
     */
    /*
     * To ensure that the done() routine gets called exactly
     * once for any packet passed to mux2Send() (when the return
     * value isn't -IP_ERRNO_EWOULDBLOCK), all code paths reachable
     * from mux2Send() must call the packet's done() routine, or
     * else if some code path does not, before it is called the caller
     * should increment pkt's reference count before calling
     * the done routine(), so as to force the send completion
     * processing, but keep the packet to pass to the code that will
     * not call done().  Also, in the unlikely event any VxWorks code
     * in the output path needs to share a reference to an Ipcom_pkt with
     * a second context, it must also increment the reference count twice
     * (rather than just once), call the done() routine, and change the
     * done member back to the default of ipcom_pkt_free() before sharing
     * the packet, to prevent more than one call to ipcom_pkt_output_done().
     * A bit ugly.
     */
    pkt->done = ipcom_drv_mux_free_out_native_pkt;
    pkt->done_arg = netif; /* TODO */

    IPCOM_DRV_ETH_STATS_LINE(send_pkts++);

#ifdef IPCOM_USE_MUX2
    retval = mux2Send (((Ipcom_pdrv *)netif->pdrv)->pEnd, pkt);

    switch(retval)
    {
    case 0:
        break;

    case -IP_ERRNO_EWOULDBLOCK:
        IPCOM_DRV_ETH_STATS_LINE(send_pkt_blocked++);
        return retval;

    case -IP_ERRNO_ENETDOWN:
        /* mux2Send() consumed the packet in this case */
        return retval;

    default:
        IPCOM_DRV_ETH_STATS_LINE(send_pkt_err_unexp++);
        IPCOM_LOG1(ERR, "ipcom_drv_mux_output :: unexpected return code from mux[Tk]Send(): %d", retval);
        IP_PANIC2();
        ipcom_pkt_output_done(netif, pkt, IP_FLAG_FC_STACKCONTEXT);
        break;   /* Ignore this error in upper layers or applications may abort */
    }

    return 0;
#else /* !defined (IPCOM_MUX_SEND) */


#ifdef IPCOM_HAVE_MUXTKSENDEND
    if (!((Ipcom_pdrv *)netif->pdrv)->drvtype)
    {
        END_OBJ *pEnd;

        pEnd = ((Ipcom_pdrv *)netif->pdrv)->pEnd;

        retval = _muxTkSendEnd(pEnd, m_blk,
                               IP_NULL, 0, IP_NULL,
                               pEnd->pFuncTable->send);
    }
#else
    /*
     * We should be able to use either the IPv4 or the
     * IPv6 binding cookie, which is fortunate since we don't
     * save them separately above.
     */
    {
#ifdef IPCOM_USE_INET
        mux_cookie = ((Ipcom_pdrv *)netif->pdrv)->mux_cookie_ipv4;
#else
        mux_cookie = ((Ipcom_pdrv *)netif->pdrv)->mux_cookie_ipv6;
#endif


#ifdef IPCOM_TEST_AGAINST_LOEND
        /* Call MuxAddress Form to form Link layer */
        /* Copy frametype ipv4 or ipv6 to reserverved for loopEnd */

        /* In the usual case one should call the address resolution
           routine registered in the mux and create the dstaddr MBLK
           How the link layer is created based on this dstaddr_blk
           depends on the driver.
           Similarly setup the srcaddr_blk_id
        */

        dstaddr_blk_id->mBlkHdr.reserved = frametype;
        srcaddr_blk_id->mBlkHdr.reserved = frametype;
#endif

#ifdef IPCOM_USE_INET
        if(frametype == IPCOM_DRV_MUX_FRAME_IPV4 && netif->drv_ip_resolve)
        {
            ip4_hdr = (Ipnet_pkt_ip*) &pkt->data[pkt->ipstart];
            ipnet_ip4_addr_to_sockaddr(&dst_sock_addr_in,((struct Ip_in_addr*)&ip4_hdr->dst)->s_addr);

            /* Call the customers registered address resolution routine */
            /* Expected resolved address in dstadd_blk_id */
            /* TODO: check return in case of delayed resolution */
            if( netif->drv_ip_resolve(mux_cookie,m_blk, &dst_sock_addr_in,
                                      dstaddr_blk_id, pkttype, frametype, pkt->route_info) != 0 )
            {
                /* ARP delayed + END driver takes ownership of m_blk */
                return 0;
            }

        }
#endif
#ifdef IPCOM_USE_INET6
        if(frametype == IPCOM_DRV_MUX_FRAME_IPV6 && netif->drv_ip6_resolve)
        {
            ip6_hdr = (Ipnet_pkt_ip6*) &pkt->data[pkt->ipstart];
            ipnet_ip6_create_sockaddr(&dst_sock_addr_in6,((struct Ip_in6_addr*)&ip6_hdr->dst),
                                      pkt->ifindex);

            /* Call the customers registered address resolution routine */
            /* Expected resolved address in dstadd_blk_id */
            /* TODO: check return in case of delayed resolution */

            if (netif->drv_ip6_resolve(mux_cookie,m_blk, &dst_sock_addr_in6,
                                       dstaddr_blk_id,pkttype, frametype, pkt->route_info) != 0)
            {
                /* ARP delayed + ownership transfer of m_blk */
                return 0;
            }
        }
#endif

        /* Call muxAddressForm to create link layer address */
        /* mux_cookie , m_blk , mblk_id containing source llinfo, mblk_id
           containing dst llinfo */
        m_blk = muxAddressForm(mux_cookie,m_blk,srcaddr_blk_id,dstaddr_blk_id);

        if(m_blk == IP_NULL)
            ipcom_printf("Creating Link layer address failed \n");


        if (!((Ipcom_pdrv *)netif->pdrv)->drvtype)
            retval = muxSend(mux_cookie, m_blk);

    }
#endif /* IPCOM_HAVE_MUXTKSENDEND */

    switch(retval)
    {
    case OK:
        break;

    case END_ERR_BLOCK:
        IPCOM_DRV_ETH_STATS_LINE(send_pkt_blocked++);
#ifndef IPCOM_VXWORKS_DRV_NULLPOOL
        /* Return the M_BLK and CL_BLK to the pool, without freeing the Ipcom_pkt */
        cl_blk->pClFreeRtn = NULL;
        netMblkClFree(m_blk);
#endif
        return -IP_ERRNO_EWOULDBLOCK;

    case ERROR:
        IPCOM_DRV_ETH_STATS_LINE(send_pkt_err++);
        IPCOM_LOG1(WARNING, "ipcom_drv_mux_output :: muxSend(): errno: %d", ipcom_errno);
        /* Note: the driver frees the m_blk and calls ipcom_pkt_output_done() */
        break;   /* Ignore this error in upper layers or applications may abort */

    default:
        IPCOM_DRV_ETH_STATS_LINE(send_pkt_err_unexp++);
        IPCOM_LOG1(ERR, "ipcom_drv_mux_output :: unexpected return code from mux[Tk]Send(): %d", retval);
        IP_PANIC2();
        break;   /* Ignore this error in upper layers or applications may abort */
    }

    return 0;
#endif /* !defined (IPCOM_MUX_SEND) */
}


/*
 ****************************************************************************
 * 10                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipcom_drv_mux_create
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipcom_drv_mux_create(void)
{

#ifdef IPCOM_USE_MUX2
    /*
     * Perform late end2VxBusConnect() operation now that the ipnet packet
     * pool should be ready.
     */
    if (_func_end2VxBusConnect)
        _func_end2VxBusConnect ();
#endif /* IPCOM_USE_MUX2 */

    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    ipcom_mux_register_dev_name_mapping
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL void
ipcom_mux_register_dev_name_mapping(const char *mux_dev_name, const char *ip_if_name)
{
#ifdef IPCOM_USE_SOCK_COMPAT
    int i;

    ip_assert(mux_dev_name);
    ip_assert(ip_if_name);

    for (i = 0; i < IPCOM_DRV_MAX_IFS; i++)
    {
        if (ipcom_port.if_map[i].mux_dev_name == IP_NULL)
        {
            ipcom_port.if_map[i].mux_dev_name = ipcom_strdup(mux_dev_name);
            ipcom_port.if_map[i].ip_if_name = ipcom_strdup(ip_if_name);

            if (ipcom_port.if_map[i].mux_dev_name == IP_NULL || ipcom_port.if_map[i].ip_if_name == IP_NULL)
            {
                IPCOM_LOG2(ERR, "ipcom_mux_register_dev_name_mapping :: failed to register mapping: %s<->%s",
                           mux_dev_name, ip_if_name);
                if (ipcom_port.if_map[i].mux_dev_name != IP_NULL)
                {
                    ipcom_free((void*)ipcom_port.if_map[i].mux_dev_name);
                    ipcom_port.if_map[i].mux_dev_name = IP_NULL;
                }
                if (ipcom_port.if_map[i].ip_if_name != IP_NULL)
                {
                    ipcom_free((void*)ipcom_port.if_map[i].ip_if_name);
                    ipcom_port.if_map[i].ip_if_name = IP_NULL;
                }
            }
            return;
        }
    }

    IP_PANIC();
#else
    (void)mux_dev_name;
    (void)ip_if_name;
#endif /* IPCOM_USE_SOCK_COMPAT */
}


/*
 *===========================================================================
 *                    ipcom_unregister_dev_name_mapping
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipcom_unregister_dev_name_mapping(const char *ip_if_name)
{
#ifdef IPCOM_USE_SOCK_COMPAT
    int i;
    const char * name;

    ip_assert(ip_if_name);

    for (i = 0; i < IPCOM_DRV_MAX_IFS; i++)
    {
        name = ipcom_port.if_map[i].ip_if_name;
        if (name == NULL ||
            ipcom_strcmp (name, ip_if_name) != 0)
            continue;

        ipcom_free ((void *)name);
        ipcom_port.if_map[i].ip_if_name = IP_NULL;
        ipcom_free((void *)ipcom_port.if_map[i].mux_dev_name);
        ipcom_port.if_map[i].mux_dev_name = IP_NULL;
        return;
    }

    IP_PANIC();
#else
    (void)ip_if_name;
#endif /* IPCOM_USE_SOCK_COMPAT */
}


/*
 *===========================================================================
 *                    ipcom_get_ip_if_name
 *===========================================================================
 * Description:   Used by windnet compat layer
 * Parameters:
 * Returns:
 *
 */
#ifdef IPCOM_USE_SOCK_COMPAT
IP_GLOBAL const char *
ipcom_mux_get_ip_if_name(const char *mux_dev_name)
{
    int i;

    ip_assert(mux_dev_name);

    for (i = 0; i < IPCOM_DRV_MAX_IFS; i++)
    {
        if (ipcom_port.if_map[i].mux_dev_name == IP_NULL)
            continue;

        if (ipcom_strcmp(ipcom_port.if_map[i].mux_dev_name, mux_dev_name) == 0)
        {
            ip_assert(ipcom_port.if_map[i].ip_if_name);
            return ipcom_port.if_map[i].ip_if_name;
        }
    }

    return IP_NULL;
}
#endif /* IPCOM_USE_SOCK_COMPAT */


/*
 *===========================================================================
 *                    ipcom_drv_mux_init_arp_hooks
 *===========================================================================
 * Description: Initialize custom arp resolution hooks in IPNET
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL Ip_err
ipcom_drv_mux_init_arp_hooks(int vr, const char *dev_name,FUNCPTR ipresolve, FUNCPTR ip6resolve, FUNCPTR ip6create)
{
    Ipcom_netif *netif;
    char *ifname;

    ifname = (char* )ipcom_mux_get_ip_if_name(dev_name);

    if (ifname == IP_NULL)
        return IPCOM_ERR_FAILED;

    netif = ipcom_if_nametonetif(vr, ifname);

    if (netif == IP_NULL)
        return IPCOM_ERR_FAILED;

#ifdef IPCOM_USE_INET
    netif->drv_ip_resolve  = (Ipcom_drv_ip_resolve) ipresolve;
#endif

#ifdef IPCOM_USE_INET6
    netif->drv_ip6_resolve  = (Ipcom_drv_ip6_resolve) ip6resolve;
    netif->drv_ip6_create_addr = (Ipcom_drv_ip6_create_addr) ip6create;
#endif

    return IPCOM_SUCCESS;
}

#endif /* IPCOM_USE_MUX_DRV */

#endif /* defined(IPNET) || defined(IPLITE) */




/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

