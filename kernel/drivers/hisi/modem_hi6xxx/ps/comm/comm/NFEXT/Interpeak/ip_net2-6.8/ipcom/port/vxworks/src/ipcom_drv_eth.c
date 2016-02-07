/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_drv_eth.c,v $ $Revision: 1.149.10.6 $
 *     $Source: /home/interpeak/CVSRoot/ipcom/port/vxworks/src/ipcom_drv_eth.c,v $
 *     $Author: dlkrejsa $
 *     $State: Exp $ $Locker:  $
 *
 *     INTERPEAK_COPYRIGHT_STRING
 *     Design and implementation by Roger Boden <roger@interpeak.se>
 ******************************************************************************
 */
/*
modification history
--------------------
01w,04dec09,pgh  Fix boot device parsing
01w,02sep09,tlu  Assign mBlk->pClBlk->pNetPool pointer (WIND00162061)
01v,29oct08,rme  Lower prioriry of FW jobs to enable timers
01u,27oct08,rme  Added packet input machanism for HW forwarding
01w,02sep09,tlu  Assign mBlk->pClBlk->pNetPool pointer (WIND00162061)
01t,23sep08,hfc  fix for defect WIND00134460,EIOCSADDR IOCTL does not change the MAC address.
01s,09aug07,kch  Added MUX-L2 and hardware vlan tagging support (WIND00101045).
01r,29jun07,kch  Removed ipcom_debug_printf() from ipcom_drv_eth_restart_end().
01q,24apr07,tkf  Removed inclusion of hostLib.h (for supporting IPv6-Only
                 build).
01p,22feb07,dlk  Removing unneeded call to netBufLibInit() (WIND00088261).
01o,13feb07,dwb  Enabling NULL_BUF_CLUSTER_REFERENCE_COUNT as a fix for
                 WIND00083063 and WIND00084107
01n,31jan07,tkf  Fix network initialization failure when target is booted from
                 a storage device.
01m,19jan07,dlk  Remove dependency upon taskShow library, and remove unneeded
         ipcom_drv_eth_get_nettask_pid() routine.
01l,08jan07,kch  Fixed ipcom_drv_eth_shutdown_end() to return ERROR rather
                 than suspending the calling task.
01k,28nov06,kch  Fixed ipcom_drv_eth_init() to update the attached interface
                 mtu and multicast capability based on the driver setting.
01j,09oct06,dlk  Do not clear m_blk->pClBlk->pNetPool in ipcom_drv_eth_output()
                 in IPCOM_VXWORKS_DRV_NULLPOOL case.
01i,13sep06,dlk  Support ipDetach(). Fix incorrect error case free in
                 ipcom_register_dev_name_mapping().
01h,10sep06,kch  Added string.h and net/if.h include statements.
01g,09sep06,kch  Merged changes from September 08 code drop.
01f,06sep06,dlk  Define IPCOM_VXWORKS_ETH_IFACE_NAMES to use VxWorks-style
                 interface names if alternate names are not specified.
                 Untabify.
01e,04sep06,dlk  Remove call to ipcom_register_dev_name_mapping() from
                 ipcom_drv_eth_ip_attach().  Formatting corrections.
                 Minor robustness improvements in ipcom_drv_eth_bind_to_dev().
01d,31aug06,kch  Fixed ipcom_drv_eth_init() to return error if endFindByName()
                 failed.
01c,26aug06,kch  Corrected code merge problems in ipcom_drv_eth_init()
                 and ipcom_drv_eth_bind_to_devs().
01b,13jul06,dlk  Added code for mapping between Ipcom
                 style interface names (eth0) and VxWorks interface names
                 (fei0).
01a,11jul06,tkf  Convert the netmask extrcated from bootline string to network
                 byte order. Added WRS copyright notice and modification
                 history.
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

#ifdef IPCOM_USE_FORWARDER
#include <ipcom_forwarder.h>
#endif

#ifdef IPWLAN
#ifdef IPPRISM
#include <ipcom_ipwlan.h>
#else
#include <ipwlan_wlan.h>
#endif
#endif

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

#include "../../../../InterpeakMntn/include/InterpeakMntn.h"


#if defined(IPCOM_VXWORKS_USE_MUX_L2) && defined(IPNET_USE_VLAN)
#include <muxL2Lib.h>
#endif

#ifdef IPIFPROXY
#include <ipifproxy.h>
#define IPCOM_DRV_ETH_FRAME_MIPCOE ETH_P_MIPC
#define IPCOM_DRV_MIPCOE_DESC      "IPIFPROXY MIPC over Ethernet"
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

#undef IPCOM_SYSLOG_PRIORITY
#define IPCOM_SYSLOG_PRIORITY    IPCOM_SYSLOG_IPCOM_PRIORITY
#undef IPCOM_SYSLOG_FACILITY
#define IPCOM_SYSLOG_FACILITY    IPCOM_LOG_IPCOM_DRV_ETH

/* Ethernet types: */
#define IPCOM_DRV_ETH_FRAME_IPV4           0x0800
#define IPCOM_DRV_ETH_FRAME_IPV6           0x86dd
#define IPCOM_DRV_ETH_FRAME_ARP            0x0806
#define IPCOM_DRV_ETH_FRAME_RARP           0x8035
#define IPCOM_DRV_ETH_FRAME_PPPOE_DIS      0x8863
#define IPCOM_DRV_ETH_FRAME_PPPOE_SES      0x8864
#define IPCOM_DRV_ETH_FRAME_8021Q_VLAN     0x8100
#define IPCOM_DRV_ETH_FRAME_EAPOL          0x888e
#define IPCOM_DRV_ETH_FRAME_MPLS_UNICAST   0x8847
#define IPCOM_DRV_ETH_FRAME_MPLS_MULTICAST 0x8848

#define IPCOM_DRV_MAX_BUF_DIFF 64

IPS_MNTN_CCORE_CAPTURE_CB   g_astCallbackTbl[IPS_MNTN_CCORE_CB_TYPE_BUTT];

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
#ifdef IPNET_USE_RARP
    void      *mux_cookie_rarp;
#endif
#endif /* IPCOM_USE_INET */
#ifdef IPCOM_USE_INET6
    void      *mux_cookie_ipv6;
#endif
#ifdef IPPPP
    void      *mux_cookie_pppoe_dis;
    void      *mux_cookie_pppoe_ses;
#endif
#ifdef IPNET_USE_VLAN
    void      *mux_cookie_vlan;
#endif
#ifdef IP8021X
    void      *mux_cookie_eapol;
#endif
#ifdef IPMPLS
    void      *mux_cookie_mpls_u;
    void      *mux_cookie_mpls_m;
#endif
#ifdef IPIFPROXY
    void      *mux_cookie_mipcoe;
#endif


    /* Ethernet multicast addresses */
    Ip_u8      eth_mlt[IPCOM_DRV_ETH_MAX_MULTIADDR][6];

    /* Number of reference on each address, determines when we can stop
     * listen on a multicast address
     */
    Ip_u8      ref_eth_mlt[IPCOM_DRV_ETH_MAX_MULTIADDR];
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

IP_GLOBAL void vxmux_calculate_transport_checksum(Ipcom_pkt *pkt);

#if defined(IPCOM_USE_FORWARDER) && defined(IPCOM_FORWARDER_LOCAL_STACK)
IP_EXTERN int ipcom_fw_register_netif_cf(void *netif);
IP_EXTERN void ipcom_fw_deregister_netif_cf(void *netif);
IP_EXTERN Ip_err ipcom_forwarder_ioctl_cf(void *netif, int code, void *fwreq);
#endif

/*
 ****************************************************************************
 * 7                    LOCAL PROTOTYPES
 ****************************************************************************
 */

IP_STATIC int ipcom_drv_eth_ioctl(Ipcom_netif *netif, Ip_u32 command, void *data);
IP_FASTTEXT IP_STATIC int ipcom_drv_eth_output(Ipcom_netif *netif, Ipcom_pkt *pkt);

#ifdef IPCOM_USE_MUX2
IP_FASTTEXT IP_STATIC BOOL ipcom_drv_eth_recv_end2(void * arg, Ipcom_pkt * pkt);

IP_STATIC STATUS ipcom_drv_eth_shutdown_end2(PROTO_COOKIE unused, void * arg);
IP_FASTTEXT IP_STATIC STATUS ipcom_drv_eth_restart_end2(void * arg);
IP_STATIC void   ipcom_drv_eth_error_end2(void * arg, END_ERR* err);
#else
/* END */
IP_FASTTEXT IP_STATIC BOOL ipcom_drv_eth_recv_end(void *handle, long proto, M_BLK_ID pkt,
                                      LL_HDR_INFO* hdr_info, void *drv_id);

IP_STATIC STATUS ipcom_drv_eth_shutdown_end(void *end_obj, void *spare);
IP_FASTTEXT IP_STATIC STATUS ipcom_drv_eth_restart_end(void *end_obj, void *spare);
IP_STATIC void   ipcom_drv_eth_error_end(END_OBJ* end_obj, END_ERR* err, void *spare);

/* NPT */
IP_FASTTEXT IP_STATIC BOOL ipcom_drv_eth_recv_npt(void *handle, long proto, M_BLK_ID pkt,
                                                  void * spare);

IP_STATIC STATUS ipcom_drv_eth_shutdown_npt(void * handle);
IP_FASTTEXT IP_STATIC STATUS ipcom_drv_eth_restart_npt(void *drv_id);
IP_STATIC void   ipcom_drv_eth_error_npt(void *handle, END_ERR* err);
#endif /* IPCOM_USE_MUX2 */

IP_STATIC void   ipcom_unregister_dev_name_mapping(const char *ip_if_name);

IP_GLOBAL int ipcom_drv_eth_exit(Ipcom_netif *netif);
IP_GLOBAL Ip_bool ipcom_vxworks_pkt_pool_hdr_ctor(void * unused,
                                                  Ipcom_pkt *pkt);

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
 *                    ipcom_drv_eth_bind_to_dev
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err
ipcom_drv_eth_bind_to_dev(const char *dev_name_unit, const char *ifname)
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

    rc = ipcom_drv_eth_init(dev_name, unit, ifname);
    if (rc != IPCOM_SUCCESS)
    {
        IPCOM_LOG2(ERR, "ipcom_drv_eth_bind_to_dev :: ipcom_drv_eth_init() failed for %s%d",
                   dev_name, unit);
    }
    ipcom_free(dev_name);

    return rc;
}


/*
 *===========================================================================
 *                    ipcom_check_fs_devs
 *===========================================================================
 * Description: Check if the specified boot device is a file system device
 * Parameters: Pointer to the BOOT_PARAMS structure.
 * Returns: Specified boot device if it is not a file system device or
 *          network device name if specified in "other" or NULL.
 *
 */
IP_STATIC char *
ipcom_check_fs_devs(BOOT_PARAMS * boot_param)
{
    char *  dev_name = NULL;
    char    bootDev[BOOT_DEV_LEN];
    int     i;

    /* Extract the boot device name. */

    for (i = 0; i < BOOT_DEV_LEN; i++)
    {
        if (isalpha ((int)boot_param->bootDev[i]))
            bootDev[i] = boot_param->bootDev[i];
        else
        {
            bootDev[i] = 0;
            break;
        }
    }

    if ((strcmp(bootDev, "scsi") == 0) ||
        (strcmp(bootDev, "fs") == 0) ||
        (strcmp(bootDev, "ide") == 0) ||
        (strcmp(bootDev, "ata") == 0) ||
        (strcmp(bootDev, "fd") == 0)  ||
        (strcmp(bootDev, "tffs") == 0) ||
        (strcmp(bootDev, "tsfs") == 0) ||
        (strcmp(bootDev, "usb") == 0) )
    {
        if (boot_param->other [0] != EOS)
        {
            char * str;
            int  unit;

            str = (char *)&boot_param->other;
            while (!ipcom_isdigit((int)*str) && *str != EOS)
                str++;

            if (str != EOS && sscanf (str, "%d", &unit) == 1)
            {
                boot_param->unitNum = unit;
                *str = EOS;
            }

            dev_name = boot_param->other;
        }
        else
        {
            dev_name = NULL;
            if (_func_printErr)
                (* _func_printErr) ("usrNetDevNameGet: no network device\n");
        }
    }
    else if (strcmp(bootDev, "passDev") == 0)
    {
        dev_name = NULL;
    }
    else
        dev_name = boot_param->bootDev;

    return dev_name;
}


/*
 *===========================================================================
 *                    ipcom_drv_eth_bind_to_devs
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err
ipcom_drv_eth_bind_to_devs(void)
{
    BOOT_PARAMS boot_params;
    char *boot_str;
#ifdef IPNET
    Ip_err ret;
    char drvname[8 + IP_IFNAMSIZ];
#endif

    boot_str = (char *)VX_BOOT_LINE;
    bootStringToStruct(boot_str, &boot_params);

#ifdef IPNET
    {
        IP_CONST char **if_conf = ipnet_conf_interfaces;
        IP_CONST char *if_conf_ifname;
        IP_CONST char *if_conf_devname;
        IP_CONST char * key;
#ifdef IPCOM_VXWORKS_ETH_IFACE_NAMES
        IP_CONST char **ifname_slot;
#endif

        while (*if_conf)
        {
            if_conf_ifname = IP_NULL;
            if_conf_devname = IP_NULL;
#ifdef IPCOM_VXWORKS_ETH_IFACE_NAMES
            ifname_slot = NULL;
#endif
            while (*if_conf)
            {
                key = ipcom_strstr (*if_conf, "ifname");
                if (key)
                {
                    key += 6; /* strlen ("ifname") */
                    while (*key == ' ' || *key == '\t')
                        ++key;
                    if_conf_ifname = key;
#ifdef IPCOM_VXWORKS_ETH_IFACE_NAMES
                    ifname_slot = if_conf;
#endif
                }
                key = ipcom_strstr (*if_conf, "devname");
                if (key)
                {
                    key += 7;  /* strlen ("devname") */
                    while (*key == ' ' || *key == '\t')
                        ++key;
                    if_conf_devname = key;
                }
                if_conf++;
            }
            if_conf++;

            if (if_conf_ifname != IP_NULL &&
                ipcom_strncmp(if_conf_ifname, "lo", 2) == 0)
                continue; /* Skip loopback interface */

            if (if_conf_devname == IP_NULL)
            {
                IPCOM_LOG1(ERR, "ipcom_drv_eth_bind_to_devs :: no dev_name specified for if %s",
                           if_conf_ifname ? if_conf_ifname : "no if name specified");
                continue;
            }

            if (ipcom_strcmp(if_conf_devname, "driver") == 0)
            {
                char *dev_name;

                if ((dev_name = ipcom_check_fs_devs(&boot_params)) == NULL)
                {
                    IPCOM_LOG0(ERR, "ipcom_drv_eth_bind_to_devs :: devname too long");
                    goto fail;
                }

                if_conf_devname = &drvname[7];
                if (ipcom_snprintf (&drvname[7], IP_IFNAMSIZ, "%s%d",
                              dev_name, boot_params.unitNum) >= IP_IFNAMSIZ)
                {
                    IPCOM_LOG0(ERR, "ipcom_drv_eth_bind_to_devs :: devname too long");
                    goto fail;
                }
            }
            ret = ipcom_drv_eth_bind_to_dev(if_conf_devname, if_conf_ifname);

            if (ret != IPCOM_SUCCESS)
            {
fail:
                IPCOM_LOG1(ERR, "ipcom_drv_eth_bind_to_devs :: failed for %s",
                           if_conf_devname);
            }
#ifdef IPCOM_VXWORKS_ETH_IFACE_NAMES
            else if (if_conf_ifname != NULL && *if_conf_ifname == '\0')
            {
                memcpy (drvname, "ifname ", 7);
                if (if_conf_devname != &drvname[7])
                    strcpy (&drvname[7], if_conf_devname);
                key = ipcom_strdup (drvname);
                if (key != NULL)
                    *ifname_slot = (const char *)key;
            }
#endif
        }
    }
#else /* IPLITE */
    {
        IP_EXTERN IP_CONST Iplite_conf_ifdata iplite_conf_interfaces[];
        IP_CONST Iplite_conf_ifdata* ifdata = iplite_conf_interfaces;

        for (;ifdata->ifname;ifdata++)
        {
            if (ipcom_strcmp(ifdata->devname, "driver") == 0)
            {
                /* Get MUX device name from boot string */
                if (ipcom_drv_eth_init(boot_params.bootDev, boot_params.unitNum, ifdata->ifname) != IPCOM_SUCCESS)
                {
                    IPCOM_LOG2(ERR, "ipcom_drv_eth_bind_to_mux :: ipcom_drv_eth_init() failed for %s%d",
                               boot_params.bootDev, boot_params.unitNum);
                }
            }
            else
            {
                ip_assert(ifdata->devname);
                if (ifdata->devname == IP_NULL)
                {
                    IPCOM_LOG0(ERR, "ipcom_drv_eth_bind_to_mux :: no devname specified");
                    continue;
                }
                ipcom_drv_eth_bind_to_dev(ifdata->devname, ifdata->ifname);
            }
        }
    }
#endif
    return IPCOM_SUCCESS;
}

#ifdef IPCOM_VXWORKS_DRV_NULLPOOL
/*
 *===========================================================================
 *                    ipcom_drv_eth_free_out_native_pkt
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
ipcom_drv_eth_free_out_native_pkt(Ipcom_pkt * pkt, char * file, int line)
#else
IP_STATIC void
ipcom_drv_eth_free_out_native_pkt(Ipcom_pkt * pkt)
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
 *                    ipcom_drv_eth_free_out_pkt
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
ipcom_drv_eth_free_out_pkt(int arg1, int arg2, int arg3)
{
    IPCOM_UNUSED_ARG(arg3);

    IPCOM_DRV_ETH_STATS_LINE(free_out_pkt++);
    ipcom_pkt_output_done((Ipcom_netif *)arg2, (Ipcom_pkt *)arg1, 0);
}


/*
 *===========================================================================
 *                    ipcom_drv_eth_get_no_pkts
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipcom_drv_eth_get_no_pkts(void)
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

    IPCOM_LOG1(INFO, "ipcom_drv_eth :: configured with %d ipcom packets", pkts);

    return pkts;
}
#endif /* !defined(IPCOM_VXWORKS_DRV_NULLPOOL) */


#if !defined(IPCOM_USE_MUX2)
/*
 *===========================================================================
 *                    ipcom_drv_eth_free_in_pkt_data
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipcom_drv_eth_free_in_pkt_data(void *buf, M_BLK_ID m_blk_id)
{
    (void)buf;

    netMblkClFree(m_blk_id);
}
#endif /* !defined(IPCOM_USE_MUX2) */


/*
 *===========================================================================
 *                   ipnet_drv_eth_sync_with_end_flags
 *===========================================================================
 * Description: Synchronizes the network interface flags with the END object
 * Parameters:  netif -
 *              mux_cookie -
 * Returns:
 *
 */
IP_STATIC void
ipnet_drv_eth_sync_with_end_flags(Ipcom_netif *netif, void *mux_cookie)
{
    unsigned int flags;
    END_MEDIA media;

    /* Read driver's flags field */
    if (muxIoctl(mux_cookie, EIOCGFLAGS, (caddr_t) &flags) == OK)
    {
        if (IP_BIT_ISSET(flags, IFF_BROADCAST))
            IP_BIT_SET(netif->flags, IP_IFF_BROADCAST);
        else
            IP_BIT_CLR(netif->flags, IP_IFF_BROADCAST);

        if (IP_BIT_ISSET(flags, IFF_MULTICAST))
            IP_BIT_SET(netif->flags, IP_IFF_MULTICAST);
        else
            IP_BIT_CLR(netif->flags, IP_IFF_MULTICAST);

        if (IP_BIT_ISSET(flags, IFF_NOARP))
            IP_BIT_SET(netif->flags, IP_IFF_NOARP);
        else
            IP_BIT_CLR(netif->flags, IP_IFF_NOARP);

#ifdef IPWLAN
        if (IP_BIT_ISSET(flags, IFF_RUNNING))
            /* Read link status from driver. */
            IP_BIT_SET(netif->flags, IP_IFF_RUNNING);
        if (flags & IFF_UP)
        {
            /* Read link status from driver. */
            IP_BIT_SET(netif->flags, IP_IFF_UP);
        }
#endif
    }

    /*
     * If the device supports EIOCGIFMEDIA, trust it if it says
     * that the link is down. Otherwise, assume the link is up.
     */
    if (muxIoctl (mux_cookie, EIOCGIFMEDIA, (char *)&media) != OK
        || (media.endMediaStatus & (IFM_AVALID|IFM_ACTIVE)) == (IFM_AVALID|IFM_ACTIVE))
    {
        (void) netif->link_ioevent(netif, IP_EIOXRUNNING, IP_NULL, 0);
    }

    IPCOM_LOG2(DEBUG, "ipnet_drv_eth_sync_with_end_flags :: %s, flags = 0x%x", netif->name, flags);
}


/*
 *===========================================================================
 *                    ipcom_drv_wlan_ioctl
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
#ifdef IPWLAN
IP_STATIC int
ipcom_drv_wlan_ioctl(Ipcom_netif *netif, Ip_u32 command, void *data)
{
    void   *mux_cookie;
    Ipcom_pdrv *pdrv = (Ipcom_pdrv *)netif->pdrv;
    struct Ip_ifreq *ifreq = (struct Ip_ifreq *)data;

    switch(command)
    {
        case IP_SIOCXOPEN:
            break;

        case IP_SIOCSIWATTACHFUNC:
            netif->wlan_ioctl = ((Ipcom_iwreq *)data)->u.data.pointer;
            break;

        case IP_SIOCSIWREMOVEFUNC:
            netif->wlan_ioctl = IP_NULL;
            break;
        default:
#ifdef IPCOM_USE_INET
            mux_cookie = pdrv->mux_cookie_ipv4;
#else
            mux_cookie = pdrv->mux_cookie_ipv6;
#endif

            if (IP_IOC_GROUP(command) == IP_IOCG_WLAN && netif->wlan_ioctl)
            {
                if (netif->wlan_ioctl(mux_cookie, command, data) != OK)
                {
                    IPCOM_LOG3(ERR, "ipcom_drv_wlan_ioctl :: wlan_ioctl(command = %x) failed, errno=%d (0x%x)",
                               command, ipcom_errno, ipcom_errno);
                    return -IP_ERRNO_EINVAL;
                }
            }
            else
            {
                return ipcom_drv_eth_ioctl(netif, command, data);
            }

    }
    return 0;
}
#endif /* IPWLAN */


/*
 *===========================================================================
 *                    ipcom_drv_eth_ioctl
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipcom_drv_eth_ioctl(Ipcom_netif *netif, Ip_u32 command, void *data)
{
    int         insert = -1;
    Ipcom_pdrv *pdrv = (Ipcom_pdrv *)netif->pdrv;
    void       *mux_cookie;
    int         i;

    ip_assert(pdrv != IP_NULL);

#ifdef IPCOM_USE_INET
    mux_cookie = pdrv->mux_cookie_ipv4;
#else
    mux_cookie = pdrv->mux_cookie_ipv6;
#endif

    switch (command)
    {
    case IP_SIOCXOPEN:
        ipnet_drv_eth_sync_with_end_flags(netif, mux_cookie);
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
        return ipcom_drv_eth_exit(netif);

    case IP_SIOCXADDMULTI:
        for (i = IPCOM_DRV_ETH_MAX_MULTIADDR - 1; i >= 0 ; i--)
        {
            if (pdrv->ref_eth_mlt[i] != 0)
            {
                if (ipcom_memcmp(data, pdrv->eth_mlt[i], 6) == 0)
                    break;
            }
            else
                insert = i;
        }

        if (insert < 0)
            /* Cannot join any more Ethernet multicast addresses */
            return -IP_ERRNO_EADDRNOTAVAIL;

        if (i >= 0)
        {
            pdrv->ref_eth_mlt[i]++;
            return IPCOM_SUCCESS;
        }
        if (muxMCastAddrAdd(mux_cookie, data))
            return -IP_ERRNO_EADDRNOTAVAIL;

        pdrv->ref_eth_mlt[insert]++;
        ipcom_memcpy(pdrv->eth_mlt[insert], data, 6);
        break;

    case IP_SIOCXDELMULTI:
        for (i = 0; i < IPCOM_DRV_ETH_MAX_MULTIADDR; i++)
            if (pdrv->ref_eth_mlt[i] != 0 && ipcom_memcmp(data, pdrv->eth_mlt[i], 6) == 0)
                break;
        if (i != IPCOM_DRV_ETH_MAX_MULTIADDR && --pdrv->ref_eth_mlt[i] == 0)
        {
            ipcom_memset(pdrv->eth_mlt[i], 0, 6);
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
                    IPCOM_LOG2(ERR, "ipcom_drv_eth_ioctl :: muxIoctl (on) failed, errno=%d (0x%x)",
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
                        IPCOM_LOG2(ERR, "ipcom_drv_eth_ioctl :: muxIoctl (off) failed, errno=%d (0x%x)",
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
            BOOT_PARAMS boot_param;
            char *boot_str, *boot_ip, *boot_ip_copy;
            char *addr_str;
            char *saveptr;
            struct Ip_ethreq *ethreq = data;
            char *ret_val;
            char    bootDev[BOOT_DEV_LEN];
            int     i;

            boot_str = (char *)VX_BOOT_LINE;
            ret_val = bootStringToStruct(boot_str, &boot_param);
            if (*ret_val != EOS)
                return -IP_ERRNO_EINVAL;

            if (IP_BIT_ISSET(boot_param.flags, SYSFLG_AUTOCONFIG))
            {
                /* Autoconfigure with DHCP */
                ethreq->ethru.inet.addr.s_addr = 0xffffffff;
                ethreq->ethru.inet.gateway.s_addr = 0xffffffff;
                break;
            }

            /* Extract the boot device name without the unit number. */
            for (i = 0; i < BOOT_DEV_LEN; i++)
            {
                if (isalpha ((int)boot_param.bootDev[i]))
                    bootDev[i] = boot_param.bootDev[i];
                else
                {
                    bootDev[i] = 0;
                    break;
                }
            }

            if (ipcom_strcmp(bootDev, "sm") == 0)
                /* Use backplane address for shared memory boot device */
                boot_ip = boot_param.bad;
            else
                boot_ip = boot_param.ead;

            /* Address */
            boot_ip_copy = ipcom_strdup(boot_ip);
            if (boot_ip_copy == IP_NULL)
                return -IP_ERRNO_ENOMEM;
            addr_str = ipcom_strtok_r(boot_ip_copy, ":", &saveptr);

            if (ipcom_inet_pton(IP_AF_INET,
                                addr_str,
                                &ethreq->ethru.inet.addr) <= 0)
            {
                ipcom_free(boot_ip_copy);
                return -IP_ERRNO_EINVAL;
            }
            ipcom_free(boot_ip_copy);

            /* Netmask */
            if (bootNetmaskExtract(boot_ip, (int*)&(ethreq->ethru.inet.mask.s_addr)) == 1)
                ethreq->ethru.inet.mask.s_addr = ip_htonl(ethreq->ethru.inet.mask.s_addr);
            else
                ethreq->ethru.inet.mask.s_addr = IP_NETMASK(ethreq->ethru.inet.addr.s_addr);

            /* Gateway */
            if (*boot_param.gad)
                ethreq->ethru.inet.gateway.s_addr = ipcom_inet_addr(boot_param.gad);
            else
                ethreq->ethru.inet.gateway.s_addr = 0; /* no gateway set in boot string */
            break;
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

#if defined(IPCOM_VXWORKS_USE_MUX_L2) && defined(IPNET_USE_VLAN)
    case IP_SIOCXETHGVLAN:
    case IP_SIOCXETHSVLAN:
        {
            struct Ip_ethreq *ethreq = data;
            MUX_L2_IOCTL_REQ  l2req;

            ipcom_memset(&l2req, 0, sizeof(MUX_L2_IOCTL_REQ));
            l2req.data.portVlanInfo.vid = ethreq->ethru.vlan.vid;

            if (command == IP_SIOCXETHGVLAN)
            {
                if (muxL2Ioctl(mux_cookie, MUXL2IOCGVLANINFO, (caddr_t)&l2req) != OK)
                    return -IP_ERRNO_EINVAL;

                ethreq->ethru.vlan.is_member = l2req.data.portVlanInfo.isMember;
                if (ethreq->ethru.vlan.is_member)
                {
                    ethreq->ethru.vlan.tagged
                        = (l2req.data.portVlanInfo.etype == EGRESS_TAGGED_FRAMES ? IP_TRUE : IP_FALSE);
                }
            }
            else
            {
                int cmd;

                if (ethreq->ethru.vlan.is_member == IP_FALSE)
                    cmd = MUXL2IOCSVLANLEAVE;
                else
                {
                    cmd = MUXL2IOCSVLANJOIN;
                    l2req.data.portVlanMember.etype = (ethreq->ethru.vlan.tagged == IP_TRUE
                                                       ? EGRESS_TAGGED_FRAMES
                                                       : EGRESS_UNTAGGED_FRAMES);
                }
                if (muxL2Ioctl(mux_cookie, cmd, (caddr_t)&l2req) != OK)
                    return -IP_ERRNO_EINVAL;
            }
        }
        break;

    case IP_SIOCXETHSPORT:
        if (muxL2Ioctl(mux_cookie, MUXL2IOCSPORTATTACH, NULL) != OK)
            return -IP_ERRNO_EINVAL;
        break;
#endif /* IPCOM_VXWORKS_USE_MUX_L2 && IPNET_USE_VLAN */

#if defined(IPCOM_USE_FORWARDER) && defined(IPCOM_FORWARDER_LOCAL_STACK)

    case IP_SIOCGHWSTATS:
    case IP_SIOCSRESETHWSTATS:

        return ipcom_forwarder_ioctl_cf(netif, command, data);

#endif

    case IP_SIOCSIFMTU:
        {
            struct Ip_ifreq *ifreq = data;

            /*
             * Request to change the MTU.
             */
            i = muxIoctl(mux_cookie,
                         EIOCSIFMTU,
                         (char *)ifreq->ifr_ifru.ifru_mtu);

            /* If we received an error; attempt to resolve if it was indeed
             * an error, or if the MTU hooks aren't implemented. There seems
             * to be a matching gap between set/get regarding end ethernet
             * drivers and MTU implementations, so try fetch the MTU, if that
             * fails, its not supported.
             */
            if (i != OK)
            {
                long mtu;

                i = muxIoctl(mux_cookie, EIOCGIFMTU, (char *)&mtu);

                /* If successful, go ERANGE. The original error would have
                 * been an EINVAL due to bad MTU input. If it fails, it means
                 * that the driver's not implemented MTU, so let IPNET deal
                 * with it. Emulate success.
                 */
                return i == OK ? -IP_ERRNO_ERANGE : 0;
            }

            return 0;
        }

    default:
        return -IP_ERRNO_EINVAL;
    }

    return 0;
}


/*
 *===========================================================================
 *                    ipcom_drv_eth_bind
 *===========================================================================
 * Description: Local helper routine to bind END or NPT drivers to the stack.
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void *
ipcom_drv_eth_bind(int drvtype,
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
           "ipcom_drv_eth_bind :: mux2Bind failed for %s, "
           "drv=%s, unit=%d, errno=%x",
                   desc, drvname, drvunit, ipcom_errno);
    }
#else
    if (drvtype)
        cookie = muxTkBind((char *)drvname, drvunit,
                           RcvRtn, ShutdownRtn, TxRestartRtn, ErrorRtn,
                           type, desc, arg, IP_NULL, IP_NULL);
    else
        cookie = muxBind((char *)drvname, drvunit,
                         RcvRtn, ShutdownRtn, TxRestartRtn, ErrorRtn,
                         type, desc, arg);
    if (cookie == IP_NULL)
    {
        IPCOM_LOG5(ERR, "ipcom_drv_eth_bind :: %s failed for %s, drv=%s, unit=%d, errno=%x",
                   (drvtype) ? "muxTkBind" : "muxBind",
                   desc, drvname, drvunit, ipcom_errno);
    }
#endif /* IPCOM_USE_MUX2 */

    return cookie;
}

/*
 *===========================================================================
 *                    ipcom_drv_eth_unbind
 *===========================================================================
 * Description: Local helper routine to unbind END or NPT drivers to the stack.
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipcom_drv_eth_unbind(long type,
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
        IPCOM_LOG4(ERR, "ipcom_drv_eth_unbind :: muxUnbind failed for %s, drv=%s, unit=%d, errno=%x",
                   desc, drvname, drvunit, ipcom_errno);
    }
}

#ifdef IPCOM_VXWORKS_DRV_NULLPOOL
/*
 *===========================================================================
 *                    ipcom_vxworks_pkt_pool_hdr_ctor
 *===========================================================================
 * Description: VxWorks pool constructor.
 * Parameters:  unused - unused memory cache pointer argument
 *              pkt - packet that are being constructed.
 * Returns:     IP_TRUE if the constructor succeeds,
 *              IP_FALSE if the constructor fails
 *
 */
IP_GLOBAL Ip_bool
ipcom_vxworks_pkt_pool_hdr_ctor(void * unused, Ipcom_pkt *pkt)
{

    Ipcom_port_pkt * ppkt = (Ipcom_port_pkt *)pkt;

    IPCOM_UNUSED_ARG(unused);

    ppkt->pkt.next_fragment = IP_NULL;
    ppkt->pkt.next_original = IP_NULL;
    ppkt->pkt.next_part = IP_NULL;

    ppkt->poolId = ipcom_port.net_pool;
    ppkt->ml.mBlk.pClBlk = &ppkt->ml.clBlk;
    ppkt->ml.clBlk.pNetPool = ipcom_port.net_pool;

    return IP_TRUE;
}
#endif /* IPCOM_VXWORKS_DRV_NULLPOOL */

/*
 *===========================================================================
 *                    ipcom_drv_eth_init
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL Ip_err
ipcom_drv_eth_init(const char *drvname, Ip_u32 drvunit, const char *ifname)
{
    Ipcom_netif *netif;
    Ipcom_pdrv  *pdrv;
    END_OBJ     *pEnd;
    FUNCPTR      recv_rtn;
    FUNCPTR      shutdown_rtn;
    FUNCPTR      restart_rtn;
    VOIDFUNCPTR  error_rtn;
    M2_INTERFACETBL *pMib2Tbl = IP_NULL;
    int      drvtype;
    void    *mux_cookie;
    char     drv_ifname [IP_IFNAMSIZ];
    char    *oldifname;

    /* Get the END_OBJ associated with the pDevice and unit */
    if ((pEnd = endFindByName ((char *)drvname, drvunit)) == NULL)
    {
        IPCOM_LOG3(ERR, "ipcom_drv_eth_init :: endFindByName failed for IPv4, drv=%s, unit=%d, errno=%x",
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
    oldifname = (char *)ipcom_get_ip_if_name(drv_ifname);
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
    netif = ipcom_if_malloc(IP_IFT_ETHER);
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
    netif->drv_ioctl  = ipcom_drv_eth_ioctl;
    netif->drv_output = ipcom_drv_eth_output;

#if defined(IPCOM_USE_FORWARDER) && defined(IPCOM_FORWARDER_LOCAL_STACK)
    (void)ipcom_fw_register_netif_cf(netif);
#endif

#ifdef IPCOM_USE_MUX2
    /* END2 Driver Type Functions */
    recv_rtn     = ipcom_drv_eth_recv_end2;
    shutdown_rtn = ipcom_drv_eth_shutdown_end2;
    restart_rtn  = ipcom_drv_eth_restart_end2;
    error_rtn    = ipcom_drv_eth_error_end2;
    drvtype = -1; /* irrelevant */
#else
    /* Check to see the type of driver we have - END or NPT */

    drvtype = muxTkDrvCheck( (char *)drvname );
    pdrv->drvtype     = drvtype;

    if (drvtype)
    {
        /* NPT Driver Type Functions */
        recv_rtn     = ipcom_drv_eth_recv_npt;
        shutdown_rtn = ipcom_drv_eth_shutdown_npt;
        restart_rtn  = ipcom_drv_eth_restart_npt;
        error_rtn    = ipcom_drv_eth_error_npt;
    }
    else
    {
        /* END Driver Type Functions */
        recv_rtn     = ipcom_drv_eth_recv_end;
        shutdown_rtn = ipcom_drv_eth_shutdown_end;
        restart_rtn  = ipcom_drv_eth_restart_end;
        error_rtn    = ipcom_drv_eth_error_end;
    }
#endif /* IPCOM_USE_MUX2 */

#ifdef IPWLAN
    if (ifname[0] == 'w')
    {
        netif->type = IP_IFT_IEEE80211;
        netif->drv_ioctl = ipcom_drv_wlan_ioctl;
    }
#endif

    /* Set unit configuration. Subscribe to relevant IP frames. */
#ifdef IPCOM_USE_INET
    pdrv->mux_cookie_ipv4 = ipcom_drv_eth_bind(drvtype,
                     (char *)drvname, drvunit, recv_rtn, shutdown_rtn, restart_rtn, error_rtn,
                     IPCOM_DRV_ETH_FRAME_IPV4, "IPSTACK IPv4", (void *)netif);

    /* Clear Restart and error pointers. */
    restart_rtn = IP_NULL;
    error_rtn   = IP_NULL;

    pdrv->mux_cookie_arp = ipcom_drv_eth_bind(drvtype,
                     (char *)drvname, drvunit, recv_rtn, shutdown_rtn, restart_rtn, error_rtn,
                     IPCOM_DRV_ETH_FRAME_ARP, "IPSTACK ARP", (void *)netif);

#ifdef IPNET_USE_RARP
    pdrv->mux_cookie_rarp = ipcom_drv_eth_bind(drvtype,
                     (char *)drvname, drvunit, recv_rtn, shutdown_rtn, restart_rtn, error_rtn,
                     IPCOM_DRV_ETH_FRAME_RARP, "IPSTACK RARP", (void *)netif);
#endif
#endif /* IPCOM_USE_INET */

#ifdef IPCOM_USE_INET6
    pdrv->mux_cookie_ipv6 = ipcom_drv_eth_bind(drvtype,
                     (char *)drvname, drvunit, recv_rtn, shutdown_rtn, restart_rtn, error_rtn,
                     IPCOM_DRV_ETH_FRAME_IPV6, "IPSTACK IPv6", (void *)netif);

    /* Clear Restart and error pointers. */
    restart_rtn = IP_NULL;
    error_rtn   = IP_NULL;
#endif /* IPCOM_USE_INET6 */

#ifdef IPPPP
    pdrv->mux_cookie_pppoe_dis = ipcom_drv_eth_bind(drvtype,
                     (char *)drvname, drvunit, recv_rtn, shutdown_rtn, restart_rtn, error_rtn,
                     IPCOM_DRV_ETH_FRAME_PPPOE_DIS, "IPSTACK PPPOE DIS", (void *)netif);

    pdrv->mux_cookie_pppoe_ses = ipcom_drv_eth_bind(drvtype,
                     (char *)drvname, drvunit, recv_rtn, shutdown_rtn, restart_rtn, error_rtn,
                     IPCOM_DRV_ETH_FRAME_PPPOE_SES, "IPSTACK PPPOE SES", (void *)netif);
#endif

#ifdef IPNET_USE_VLAN
    pdrv->mux_cookie_vlan = ipcom_drv_eth_bind(drvtype,
                     (char *)drvname, drvunit, recv_rtn, shutdown_rtn, restart_rtn, error_rtn,
                     IPCOM_DRV_ETH_FRAME_8021Q_VLAN, "IPSTACK 802.1Q VLAN", (void *) netif);
#endif

#ifdef IP8021X
    pdrv->mux_cookie_eapol = ipcom_drv_eth_bind(drvtype,
                     (char *)drvname, drvunit, recv_rtn, shutdown_rtn, restart_rtn, error_rtn,
                     IPCOM_DRV_ETH_FRAME_EAPOL, "IPSTACK IEEE EAPOL", (void *) netif);
#endif

#ifdef IPMPLS
    pdrv->mux_cookie_mpls_u = ipcom_drv_eth_bind(drvtype,
             (char *)drvname, drvunit, recv_rtn, shutdown_rtn, restart_rtn, error_rtn,
                     IPCOM_DRV_ETH_FRAME_MPLS_UNICAST, "IPSTACK MPLS UNICAST", (void *) netif);

    pdrv->mux_cookie_mpls_m = ipcom_drv_eth_bind(drvtype,
                     (char *)drvname, drvunit, recv_rtn, shutdown_rtn, restart_rtn, error_rtn,
                     IPCOM_DRV_ETH_FRAME_MPLS_MULTICAST, "IPSTACK MPLS MULTICAST", (void *) netif);
#endif

#ifdef IPIFPROXY
    pdrv->mux_cookie_mipcoe
        = ipcom_drv_eth_bind(drvtype, (char *)drvname, drvunit,
                             recv_rtn, shutdown_rtn, restart_rtn, error_rtn,
                             IPCOM_DRV_ETH_FRAME_MIPCOE, IPCOM_DRV_MIPCOE_DESC,
                             (void *) netif);
#endif

#ifdef IPCOM_VXWORKS_DRV_ETH_PROMISCUOUS
    {
        Ip_bool on = 1;
        if (ipcom_drv_eth_ioctl(netif, IP_SIOCXPROMISC, &on) != IPCOM_SUCCESS)
        {
            IPCOM_LOG4(ERR, "ipcom_drv_eth_init :: ipcom_drv_eth_ioctl(IP_SIOCXPROMISC) failed for %s%d, errno=%d (0x%x)",
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

    /* Read Ethernet address from driver. */
    if (muxIoctl(mux_cookie, EIOCGADDR, (caddr_t) netif->link_addr))
        goto fail;

    /* Read driver's flags field */
    ipnet_drv_eth_sync_with_end_flags(netif, mux_cookie);

    pMib2Tbl = &pEnd->mib2Tbl;

#if IP_PORT_VXWORKS < 67
#ifdef END_MIB_2233
    if (pEnd->flags & END_MIB_2233)
    {
        M2_ID *pM2Id = IP_NULL;
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

#if defined(IPCOM_VXWORKS_USE_MUX_L2) && defined(IPNET_USE_VLAN)
    {
        MUX_L2_IOCTL_REQ     l2req;

        /*
         * When a port is attached to L2, MUX-L2 normally send an
         * END_ERR_L2NOTIFY to notify upper layer if the mtu size should
         * be adjusted to accommodate for vlan tag in the transmitting
         * frames. If a port is attached to L2 before this routine is called
         * to attach the interface tto MUX, ipcom_drv_eth_error_end() will
         * ignore the END_ERR_L2NOTIFY notification. To resolve this issue,
         * it is necessary to query L2 if the port is L2-enabled.
         */
        ipcom_memset(&l2req, 0, sizeof(MUX_L2_IOCTL_REQ));
        if (muxL2Ioctl(mux_cookie, MUXL2IOCGPORTINFO, (caddr_t)&l2req) == OK &&
            netif->mtu != l2req.data.portInfo.newMtu)
            netif->mtu = l2req.data.portInfo.newMtu;
    }
#endif /* IPCOM_VXWORKS_USE_MUX_L2 && IPNET_USE_VLAN */

#ifdef IPCOM_USE_HW_CAPS
    if (muxIoctl(mux_cookie, EIOCGIFCAP, (caddr_t) &pdrv->cap) != OK)
        ipcom_memset(&pdrv->cap, 0, sizeof(pdrv->cap));
    else
    {
#ifdef IPCOM_USE_HW_VLAN_TAGGING
        /* check if hardware support slightly larger than normal frames */
        if (IP_BIT_ISSET(pdrv->cap.cap_available, IFCAP_VLAN_MTU))
            IP_BIT_SET(netif->drv_capabilities, IPCOM_IF_DRV_CAP_VLAN_MTU);
        /* check if hardware support VLAN tag insertion and stripping */
        if (IP_BIT_ISSET(pdrv->cap.cap_available, IFCAP_VLAN_HWTAGGING))
        {
            if (IP_BIT_ISSET(pdrv->cap.csum_flags_tx, CSUM_VLAN))
                IP_BIT_SET(netif->drv_capabilities, IPCOM_IF_DRV_CAP_VLAN_HWTAGGING_TX);
            if (IP_BIT_ISSET(pdrv->cap.csum_flags_rx, CSUM_VLAN))
                IP_BIT_SET(netif->drv_capabilities, IPCOM_IF_DRV_CAP_VLAN_HWTAGGING_RX);
        }
#endif /* IPCOM_USE_HW_VLAN_TAGGING */

#ifdef IPCOM_USE_HW_CHECKSUM
        if (IP_BIT_ISFALSE(pdrv->cap.cap_available, IFCAP_RXCSUM))
            pdrv->cap.csum_flags_rx = 0;

        if (IP_BIT_ISFALSE(pdrv->cap.cap_available, IFCAP_TXCSUM))
            pdrv->cap.csum_flags_tx = 0;
        if ((pdrv->cap.csum_flags_tx & (CSUM_TCP | CSUM_UDP)) == (CSUM_TCP | CSUM_UDP))
            IP_BIT_SET(netif->drv_capabilities, IPCOM_IF_DRV_CAP_IP4_TRANSPORT_CSUM_TX);
        if ((pdrv->cap.csum_flags_tx & (CSUM_TCPv6 | CSUM_UDPv6)) == (CSUM_TCPv6 | CSUM_UDPv6))
            IP_BIT_SET(netif->drv_capabilities, IPCOM_IF_DRV_CAP_IP6_TRANSPORT_CSUM_TX);

        IPCOM_LOG9(DEBUG, "ipcom_drv_eth_init :: %s has HW support for TX %s%s%s%s RX %s%s%s%s",
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

    ipcom_register_dev_name_mapping(drv_ifname, ifname);


    return IPCOM_SUCCESS;

 fail:
    if (pdrv != IP_NULL)
        ipcom_free(pdrv);
    ipcom_if_free(netif);

    return IPCOM_ERR_FAILED;
}


/*
 *===========================================================================
 *                    ipcom_drv_eth_exit
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL int
ipcom_drv_eth_exit(Ipcom_netif *netif)
{
    FUNCPTR  recv_rtn;
    Ipcom_pdrv  *pdrv;
#ifndef IPCOM_USE_MUX2
    int      drvtype;
#endif

    pdrv = netif->pdrv;

    IPCOM_LOG3(INFO, "ipcom_drv_eth_exit :: detaching %s   [drv=%s, unit=%d]",
               netif->name, pdrv->drvname, pdrv->drvunit);

#if defined(IPCOM_USE_FORWARDER) && defined(IPCOM_FORWARDER_LOCAL_STACK)
    ipcom_fw_deregister_netif_cf(netif);
#endif

#ifdef IPCOM_USE_MUX2
    recv_rtn = (FUNCPTR)ipcom_drv_eth_recv_end2;
#else
    /* Check to see the type of driver we have - END or NPT */
    drvtype = muxTkDrvCheck(pdrv->drvname);
    if (drvtype)
        recv_rtn = ipcom_drv_eth_recv_npt;
    else
        recv_rtn = ipcom_drv_eth_recv_end;
#endif /* IPCOM_USE_MUX2 */

    /* Set unit configuration. Subscribe to relevant IP frames. */
#ifdef IPCOM_USE_INET
    ipcom_drv_eth_unbind(IPCOM_DRV_ETH_FRAME_IPV4,
                       pdrv->drvname, pdrv->drvunit,
                       recv_rtn,
                       "IPSTACK IPv4",
                       pdrv->mux_cookie_ipv4);

    ipcom_drv_eth_unbind(IPCOM_DRV_ETH_FRAME_ARP,
                       pdrv->drvname, pdrv->drvunit,
                       recv_rtn,
                       "IPSTACK ARP",
                       pdrv->mux_cookie_arp);

#ifdef IPNET_USE_RARP
    ipcom_drv_eth_unbind(IPCOM_DRV_ETH_FRAME_RARP,
                       pdrv->drvname, pdrv->drvunit,
                       recv_rtn,
                       "IPSTACK RARP",
                       pdrv->mux_cookie_rarp);
#endif
#endif /* IPCOM_USE_INET */

#ifdef IPCOM_USE_INET6
    ipcom_drv_eth_unbind(IPCOM_DRV_ETH_FRAME_IPV6,
                       pdrv->drvname, pdrv->drvunit,
                       recv_rtn,
                       "IPSTACK IPv6",
                       pdrv->mux_cookie_ipv6);
#endif /* IPCOM_USE_INET6 */

#ifdef IPPPP
    ipcom_drv_eth_unbind(IPCOM_DRV_ETH_FRAME_PPPOE_DIS,
                       pdrv->drvname, pdrv->drvunit,
                       recv_rtn,
                       "IPSTACK PPPOE DIS",
                       pdrv->mux_cookie_pppoe_dis);

    ipcom_drv_eth_unbind(IPCOM_DRV_ETH_FRAME_PPPOE_SES,
                       pdrv->drvname, pdrv->drvunit,
                       recv_rtn,
                       "IPSTACK PPPOE SES",
                       pdrv->mux_cookie_pppoe_ses);
#endif

#ifdef IPNET_USE_VLAN
    ipcom_drv_eth_unbind(IPCOM_DRV_ETH_FRAME_8021Q_VLAN,
                       pdrv->drvname, pdrv->drvunit,
                       recv_rtn,
                       "IPSTACK 802.1Q VLAN",
                       pdrv->mux_cookie_vlan);
#endif

#ifdef IP8021X
    ipcom_drv_eth_unbind(IPCOM_DRV_ETH_FRAME_EAPOL,
                       pdrv->drvname, pdrv->drvunit,
                       recv_rtn,
                       "IPSTACK IEEE EAPOL",
                       pdrv->mux_cookie_eapol);
#endif

#ifdef IPIFPROXY
    ipcom_drv_eth_unbind(IPCOM_DRV_ETH_FRAME_MIPCOE,
                         pdrv->drvname, pdrv->drvunit,
                         recv_rtn,
                         IPCOM_DRV_MIPCOE_DESC,
                         pdrv->mux_cookie_mipcoe);
#endif

#ifdef IPMPLS
    ipcom_drv_eth_unbind(IPCOM_DRV_ETH_FRAME_MPLS_UNICAST,
                       pdrv->drvname, pdrv->drvunit,
                       recv_rtn,
                       "IPSTACK MPLS UNICAST",
                       pdrv->mux_cookie_mpls_u);

    ipcom_drv_eth_unbind(IPCOM_DRV_ETH_FRAME_MPLS_MULTICAST,
                       pdrv->drvname, pdrv->drvunit,
                       recv_rtn,
                       "IPSTACK MPLS MULTICAST",
                       pdrv->mux_cookie_mpls_m);
#endif

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
 *                    ipcom_drv_eth_recv_end2
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_FASTTEXT IP_STATIC BOOL
ipcom_drv_eth_recv_end2(void *drv_id, Ipcom_pkt *pkt)
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
 *                    ipcom_drv_eth_detach
 *===========================================================================
 * Description: internal support routine for ipcom_drv_eth_shutdown_end2
 *              detaches from the specified interface.
 * Parameters:
 * Returns:
 *
 */

IP_STATIC void
ipcom_drv_eth_detach(int ifindex)
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
    ipcom_printf("ipcom_drv_eth_detach: failed for %s: %s" IP_LF,
         ifreq.ifr_name, ipcom_strerror(ipcom_errno));
    if (fd != IP_INVALID_SOCKET)
    ipcom_socketclose (fd);
    return;
}

/*===========================================================================
 *                    ipcom_drv_eth_shutdown_end2
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC STATUS
ipcom_drv_eth_shutdown_end2(PROTO_COOKIE unused, void * arg)
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

    tid = taskSpawn ("tDetach", 100, 0, 10000, (FUNCPTR)ipcom_drv_eth_detach,
             (int)netif->ifindex, 0,0,0,0,0,0,0,0,0);

    return (tid == ERROR ? ERROR : OK);
}

/*===========================================================================
 *                    ipcom_drv_eth_restart_end2
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_FASTTEXT IP_STATIC STATUS
ipcom_drv_eth_restart_end2(void *drv_id)
{
    Ipcom_netif *netif = (Ipcom_netif *)drv_id;

    ipcom_pkt_output_done(netif, IP_NULL, 0);
    IPCOM_DRV_ETH_STATS_LINE(restart++);

    return OK;
}

#else /* IPCOM_USE_MUX2 */

/*===========================================================================
 *                    ipcom_drv_eth_recv_end
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_FASTTEXT IP_STATIC BOOL
ipcom_drv_eth_recv_end(void *handle, long proto, M_BLK_ID m_blk,
                       LL_HDR_INFO* hdr_info, void *drv_id)
{
    Ipcom_netif  *netif;
    Ipcom_pdrv   *pdrv;
    Ipcom_pkt    *pkt;

    IPCOM_DRV_ETH_STATS_LINE(recv_mbufs++);

    netif = (Ipcom_netif *)drv_id;
    pdrv = (Ipcom_pdrv *)netif->pdrv;

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

    pkt->data   = (Ip_u8 *)m_blk->pClBlk->clNode.pClBuf;
    pkt->maxlen = TUP_CL_LEN_GET(m_blk);
    pkt->end    = m_blk->mBlkHdr.mLen + pkt->start;
    pkt->data_freefunc =
        (Ipcom_pkt_freefunc)ipcom_drv_eth_free_in_pkt_data;
    pkt->data_freefunc_cookie = m_blk;

    IPCOM_PKT_TRACE(pkt, IPCOM_PKT_ID_RX);
    pkt->ifindex = netif->ifindex;

#ifdef IPCOM_USE_HW_VLAN_TAGGING
    /*
     * If driver supports stripping vlan tag, ethernet header from the received
     * frame will be untagged and the vlan tag information will be stored in
     * mBlkPktHdr.vlan. Copy the vlan information to pkt->link_cookie.
     * For better performance, don't check the netif->drv_capabilities for
     * vlan tagging support. The driver shouldn't be setting the CSUM_VLAN
     * flag unless it does in fact support RX VLAN stripping and it is enabled.
     */
    if (IP_BIT_ISSET(m_blk->mBlkPktHdr.csum_flags, CSUM_VLAN))
    {
        IPNET_ETH_PKT_SET_VLAN_TAG(pkt, m_blk->mBlkPktHdr.vlan);
        IP_BIT_SET(pkt->flags, IPCOM_PKT_FLAG_VLAN_TAG);
    }
#endif /* IPCOM_USE_HW_VLAN_TAGGING */

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
 *                    ipcom_drv_eth_recv_npt
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_FASTTEXT IP_STATIC BOOL
ipcom_drv_eth_recv_npt(void *drv_id, long proto, M_BLK_ID m_blk, void *spare)
{
    /* point back to the mac header, and adjust the length */
#if IP_PORT_VXWORKS >= 60
    m_blk->mBlkHdr.mData    -= m_blk->mBlkHdr.offset1;
    m_blk->mBlkHdr.mLen     += m_blk->mBlkHdr.offset1;
    m_blk->mBlkPktHdr.len   += m_blk->mBlkHdr.offset1;
#else
    m_blk->mBlkHdr.mData    -= m_blk->mBlkHdr.reserved;
    m_blk->mBlkHdr.mLen     += m_blk->mBlkHdr.reserved;
    m_blk->mBlkPktHdr.len   += m_blk->mBlkHdr.reserved;
#endif

    return ipcom_drv_eth_recv_end(drv_id, proto, m_blk, IP_NULL, drv_id);
}


/*===========================================================================
 *                    ipcom_drv_eth_shutdown_end
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC STATUS
ipcom_drv_eth_shutdown_end(void *end_obj, void *spare)
{
    /* Not implemented. Pull the emergency break. */
    IPCOM_LOG0(ERR, "ipcom_drv_eth_shutdown_end :: MUX shutdown callback not implemented\n");
    printf("ipcom_drv_eth_shutdown_end :: MUX shutdown callback not implemented\n");

    return ERROR;
}


/*===========================================================================
 *                    ipcom_drv_eth_shutdown_npt
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC STATUS
ipcom_drv_eth_shutdown_npt(void *handle)
{
    Ipcom_netif     *netif;

    netif = (Ipcom_netif *)handle;

    return ipcom_drv_eth_shutdown_end(((Ipcom_pdrv *)netif->pdrv)->pEnd, IP_NULL);
}


/*===========================================================================
 *                    ipcom_drv_eth_restart_end
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_FASTTEXT IP_STATIC STATUS
ipcom_drv_eth_restart_end(void *end_obj, void *drv_id)
{
    Ipcom_netif *netif = (Ipcom_netif *)drv_id;

    ipcom_pkt_output_done(netif, IP_NULL, 0);
    IPCOM_DRV_ETH_STATS_LINE(restart++);

    return OK;
}


/*===========================================================================
 *                    ipcom_drv_eth_restart_npt
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_FASTTEXT IP_STATIC STATUS
ipcom_drv_eth_restart_npt(void *drv_id)
{
    Ipcom_netif     *netif;

    netif = (Ipcom_netif *)drv_id;

    return ipcom_drv_eth_restart_end( ((Ipcom_pdrv *)netif->pdrv)->pEnd, drv_id );
}
#endif /* IPCOM_USE_MUX2 */


/*===========================================================================
 *                    ipcom_drv_eth_error_end[2]
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
#ifdef IPCOM_USE_MUX2
ipcom_drv_eth_error_end2(void * arg, END_ERR *err)
#else
ipcom_drv_eth_error_end(END_OBJ *end_obj, END_ERR *err, void *spare)
#endif
{
#ifdef IPCOM_USE_MUX2
    Ipcom_netif *netif = (Ipcom_netif *)arg;
#else
    Ipcom_netif *netif = (Ipcom_netif *)spare;
#endif

    IPCOM_DRV_ETH_STATS_LINE(error_end++);

    IPCOM_LOG3(INFO, "ipcom_drv_eth_error_end :: %s '%s' errCode = %d",
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
#if defined(IPNET) && defined(IPWLAN)
        if (netif->type == IP_IFT_IEEE80211)
        {
            struct Ip_ifreq ifreq;
            Ipcom_dev_data_t *data = err->pSpare;
            memcpy(netif->macaddr, data->macaddr, sizeof(netif->macaddr));
            memcpy(ifreq.ip_ifr_addr.sa_data, data->macaddr, sizeof(data->macaddr));
            ipnet_if_link_ioctl((Ipnet_netif *)netif, IP_SIOCSIFLLADDR, &ifreq);
            break;
        }
#endif
        /* The MAC address is reported after device reset */
        /* Fall through */

    case END_ERR_LINKUP:
        /*!! We need to call stop first to compensate for drivers with bad boot flags */
        (void)netif->link_ioevent(netif, IP_EIOXSTOP, IP_NULL, IP_FLAG_FC_STACKCONTEXT);
        (void)netif->link_ioevent(netif, IP_EIOXRUNNING, IP_NULL, IP_FLAG_FC_STACKCONTEXT);
        break;
#if defined(IPCOM_VXWORKS_USE_MUX_L2) && defined(IPNET_USE_VLAN)
    case END_ERR_L2NOTIFY:
        /*
         * link layer notifications associated with 802.1Q vlan. For now, these
         * notifications are sent for ethernet interface type only.
         */
        netif->mtu = ((L2NOTIFY_PARAMS *)err->pSpare)->newMtu;
        break;

    case END_ERR_L2PVID_NOTIFY:
    case END_ERR_L2VID_NOTIFY:
        {
            Ipcom_eth_vlanreq    vlanreq = {0, 0, 0, 0};
            L2VID_NOTIFY_PARAMS  *l2params = err->pSpare;

            if (err->errCode == END_ERR_L2PVID_NOTIFY)
            {
                vlanreq.vid_is_pvid = IP_TRUE; /* port-based config */
                vlanreq.pri = l2params->pri;
            }

            vlanreq.vlan_tag_required = l2params->sendVlanTag;
            vlanreq.vid = l2params->vid;
            (void)netif->link_ioevent(netif, IP_EIOXVLAN, &vlanreq,
                                      IP_FLAG_FC_STACKCONTEXT);
        }
        break;
#endif /* IPCOM_VXWORKS_USE_MUX_L2 && IPNET_USE_VLAN */

    default:
        break;
    }
}

#ifndef IPCOM_USE_MUX2

/*===========================================================================
 *                    ipcom_drv_eth_error_npt
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipcom_drv_eth_error_npt(void *handle, END_ERR *err)
{
    Ipcom_netif     *netif;

    netif = (Ipcom_netif *)handle;

    ipcom_drv_eth_error_end(((Ipcom_pdrv *)netif->pdrv)->pEnd, err, netif);
}
#endif /* !defined (IPCOM_USE_MUX2) */


/*
 *===========================================================================
 *                    ipcom_drv_eth_output
 *===========================================================================
 * Description: Outputs an Ethernet frame.
 * Parameters:  netif - The network interface to use when sending the frame.
 *              pkt - The frame send. pkt->start is the offset to the
 *              Ethernet frame header.
 * Returns:     0 = success, <0 = error code.
 *
 */
IP_FASTTEXT IP_STATIC int
ipcom_drv_eth_output(Ipcom_netif *netif, Ipcom_pkt *pkt)
{
    END_OBJ *pEnd;
#ifndef IPCOM_USE_MUX2
    M_BLK_ID    m_blk;
    Ipcom_pkt  *pkt_part = pkt;
    Ipcom_pkt  *pkt_prev_part;
    M_BLK_ID    prev_m_blk = IP_NULL;
#endif
    int         retval;

    IPS_MNTN_CCORE_CAPTURE_CB            pCaptureCb;
    IPS_MNTN_CCORE_CAPTURE_PARA_STRU     stIpsMntnCapturePara;

#ifdef IPCOM_VXWORKS_PAD_SHORT_ETHERNET_FRAMES
    if ((pkt->end - pkt->start) < 60)
        pkt->end += 60 - (pkt->end - pkt->start);
#endif

    pCaptureCb  = g_astCallbackTbl[IPS_MNTN_CCORE_OUTPUT_DATA_CB];

    stIpsMntnCapturePara.paucNetIfName    = (unsigned char *)(netif->name);
    stIpsMntnCapturePara.pucData          = &(pkt->data[pkt->start]);
    stIpsMntnCapturePara.usLen            = (UINT16)(pkt->end - pkt->start);

    if ( NULL != pCaptureCb )
    {
        pCaptureCb(&stIpsMntnCapturePara);
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
                 pkt->maxlen, (FUNCPTR)ipcom_drv_eth_free_out_pkt, (int)pkt, (int)netif, 1);
    netMblkClJoin(m_blk, cl_blk);
    m_blk->mBlkHdr.mLen = pkt->end - pkt->start;
    m_blk->mBlkPktHdr.len = m_blk->mBlkHdr.mLen;
    m_blk->mBlkHdr.mType = MT_DATA;
#endif /* #ifdef IPCOM_VXWORKS_DRV_NULLPOOL */

#ifdef IPCOM_USE_HW_VLAN_TAGGING
    if (IP_BIT_ISSET(pkt->flags, IPCOM_PKT_FLAG_VLAN_TAG))
    {
        m_blk->mBlkPktHdr.vlan = IPNET_ETH_PKT_GET_VLAN_TAG(pkt);
        m_blk->mBlkPktHdr.csum_flags |= CSUM_VLAN;
    }
#endif /* #ifdef IPCOM_USE_HW_VLAN_TAGGING */

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
                vxmux_calculate_transport_checksum(pkt);
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
                vxmux_calculate_transport_checksum(pkt);
        }
#endif /* IPCOM_USE_INET */
    }
#endif /* IPCOM_USE_HW_CHECKSUM_TX */
#endif /* !defined (IPCOM_USE_MUX2) */

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
    pkt->done = ipcom_drv_eth_free_out_native_pkt;
    pkt->done_arg = netif; /* TODO */

    IPCOM_DRV_ETH_STATS_LINE(send_pkts++);

    pEnd = ((Ipcom_pdrv *)netif->pdrv)->pEnd;

#ifdef IPCOM_USE_MUX2
    retval = mux2Send (pEnd, pkt);

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
        IPCOM_LOG1(ERR, "ipcom_drv_eth_output :: unexpected return code from mux[Tk]Send(): %d", retval);
        IP_PANIC2();
        ipcom_pkt_output_done(netif, pkt, IP_FLAG_FC_STACKCONTEXT);
        break;   /* Ignore this error in upper layers or applications may abort */
    }

    return 0;
#else /* !defined (IPCOM_MUX_SEND) */
#ifdef IPCOM_HAVE_MUXTKSENDEND
    if (((Ipcom_pdrv *)netif->pdrv)->drvtype)
        retval = _muxTkSendNpt(pEnd, m_blk,
                               IP_NULL, 0, IP_NULL,
                               pEnd->pFuncTable->send);
    else
        retval = _muxTkSendEnd(pEnd, m_blk,
                               IP_NULL, 0, IP_NULL,
                               pEnd->pFuncTable->send);
#else
    /*
     * We should be able to use either the IPv4 or the
     * IPv6 binding cookie, which is fortunate since we don't
     * save them separately above.
     * Possible problem: the convention that a muxTkSend()
     * call with a NULL dstMacAddr argument made to an NPT driver
     * means that the link header is already pre-attached was not well
     * established before VxWorks 6.2. Earlier NPT drivers may not
     * handle this case as desired.
     */
    {
#ifdef IPCOM_USE_INET
        void* mux_cookie;
        mux_cookie = ((Ipcom_pdrv *)netif->pdrv)->mux_cookie_ipv4;
#else
        void* mux_cookie;
        mux_cookie = ((Ipcom_pdrv *)netif->pdrv)->mux_cookie_ipv6;
#endif
        if (((Ipcom_pdrv *)netif->pdrv)->drvtype)
            retval = muxTkSend(mux_cookie, m_blk,
                               IP_NULL, 0, IP_NULL);
        else
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
        IPCOM_LOG1(WARNING, "ipcom_drv_eth_output :: muxSend(): errno: %d", ipcom_errno);
        /* Note: the driver frees the m_blk and calls ipcom_pkt_output_done() */
        break;   /* Ignore this error in upper layers or applications may abort */

    default:
        IPCOM_DRV_ETH_STATS_LINE(send_pkt_err_unexp++);
        IPCOM_LOG1(ERR, "ipcom_drv_eth_output :: unexpected return code from mux[Tk]Send(): %d", retval);
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
 *                    ipcom_drv_eth_ip_attach
 *===========================================================================
 * Description: This routine is called to attach IP protocols to an interface.
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipcom_drv_eth_ip_attach( char * if_conf_devname, char * if_conf_ifname )
{
    if (if_conf_devname == IP_NULL)
    {
        IPCOM_LOG1(ERR, "ipcom_drv_eth_ip_attach :: no dev_name specified for if %s",
                   if_conf_ifname ? if_conf_ifname : "no if name specified");
        return IPCOM_ERR_FAILED;
    }
    return ipcom_drv_eth_bind_to_dev(if_conf_devname, if_conf_ifname);
}


/*
 *===========================================================================
 *                    ipcom_drv_eth_create
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipcom_drv_eth_create(void)
{

#ifdef IPCOM_USE_MUX2
    /*
     * Perform late end2VxBusConnect() operation now that the ipnet packet
     * pool should be ready.
     */
    if (_func_end2VxBusConnect)
        _func_end2VxBusConnect ();
#endif /* IPCOM_USE_MUX2 */

    /* Bind to devices */
    if (ipcom_drv_eth_bind_to_devs() != IPCOM_SUCCESS)
    {
        IPCOM_LOG1(ERR, "ipcom_drv_eth_create :: ipcom_drv_eth_bind_to_devs() failed, errno=%d", ipcom_errno);
    }

#if defined(IPCOM_USE_INET) &&                              \
    defined(_WRS_VXWORKS_MAJOR) && _WRS_VXWORKS_MAJOR < 6
    /* Hmm, this if condition is odd - _WRS_VXWORKS_MAJOR et. al.
     * were introduced in VxWorks 6.0, and would not be defined
     * earlier, so this code most likely won't be hit...
     */
    {
        BOOT_PARAMS boot_params;
        char *boot_str;

        boot_str = VX_BOOT_LINE;
        bootStringToStruct(boot_str, &boot_params);
        hostAdd("localhost", "127.0.0.1");

        /* The vxWorks native stack registers host and target IP addresses.
         * Needed by remote IO. Let's do the same.
         */
        if (boot_params.hostName[0] != EOS && boot_params.had[0] != EOS)
            hostAdd(boot_params.hostName, boot_params.had);
        if (boot_params.targetName[0] != EOS && boot_params.ead[0] != EOS)
            hostAdd(boot_params.hostName, boot_params.had);
    }
#endif /* IPCOM_USE_INET */

    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    ipcom_register_dev_name_mapping
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL void
ipcom_register_dev_name_mapping(const char *mux_dev_name, const char *ip_if_name)
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
                IPCOM_LOG2(ERR, "ipcom_register_dev_name_mapping :: failed to register mapping: %s<->%s",
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
ipcom_get_ip_if_name(const char *mux_dev_name)
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
 *                    ipcom_drv_eth_cmd_stats
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
#ifdef IPCOM_DRV_ETH_STATS
IP_PUBLIC int
ipcom_drv_eth_cmd_stats(int argc, char **argv)
{
    ipcom_printf("recv_mbufs:           %-8u\n", ipcom_port.eth_stats.recv_mbufs);
    ipcom_printf("recv_pkts:            %-8u\n", ipcom_port.eth_stats.recv_pkts);
    ipcom_printf("recv_lost:            %-8u\n", ipcom_port.eth_stats.recv_lost);
    ipcom_printf("recv_lost_pkt_create: %-8u\n", ipcom_port.eth_stats.recv_lost_pkt_create);
    ipcom_printf("recv_strange_buf:     %-8u\n", ipcom_port.eth_stats.recv_strange_buf);
    ipcom_printf("recv_pkt_malloc_fail: %-8u\n", ipcom_port.eth_stats.recv_pkt_malloc_fail);
    ipcom_printf("send_pkts:            %-8u\n", ipcom_port.eth_stats.send_pkts);
    ipcom_printf("send_pkt_blocked:     %-8u\n", ipcom_port.eth_stats.send_pkt_blocked);
    ipcom_printf("send_pkt_err:         %-8u\n", ipcom_port.eth_stats.send_pkt_err);
    ipcom_printf("send_pkt_err_unexp:   %-8u\n", ipcom_port.eth_stats.send_pkt_err_unexp);
    ipcom_printf("send_out_of_mbufs:    %-8u\n", ipcom_port.eth_stats.send_out_of_mbufs);
    ipcom_printf("send_out_of_clblk:    %-8u\n", ipcom_port.eth_stats.send_out_of_clblk);
    ipcom_printf("restart:              %-8u\n", ipcom_port.eth_stats.restart);
    ipcom_printf("error_end:            %-8u\n", ipcom_port.eth_stats.error_end);
    ipcom_printf("free_out_pkt:         %-8u\n", ipcom_port.eth_stats.free_out_pkt);

    return IPCOM_SUCCESS;
}
#endif /* IPCOM_DRV_ETH_STATS */

#if defined(IPCOM_USE_FORWARDER) && defined(IPCOM_FORWARDER_LOCAL_STACK)
IP_PUBLIC void *
ipcom_fw_netif_to_end(Ipcom_netif *netif)
{
    Ipcom_pdrv * pdrv;
    if (netif == IP_NULL ||
	netif->type != IP_IFT_ETHER ||
	(pdrv = netif->pdrv) == IP_NULL)
	return IP_NULL;

    return pdrv->pEnd;
}
#endif

#endif /* defined(IPNET) || defined(IPLITE) */

/*****************************************************************************
 函 数 名  : IPS_MNTN_CCORE_InterpeakMntnInit
 功能描述  : 初始化Interpeak可维可测模块
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 成功:IPS_MNTN_OK，失败为IPS_MNTN_ERROR
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年8月3日
    作    者   : 冯家麟
    修改内容   : 新生成函数
  2.日    期   : 2013年7月30日
    作    者   : caikai
    修改内容   : VoLTE特性需要将其移植至V9

*****************************************************************************/
int IPS_MNTN_CCORE_InterpeakMntnInit(void)
{
    /*初始化Interpeak可维可测模块*/
    ipcom_memset(g_astCallbackTbl, 0, IPS_MNTN_CCORE_CB_TYPE_BUTT * sizeof(IPS_MNTN_CCORE_CAPTURE_CB));

    return IPS_MNTN_CCORE_OK;
}


/*****************************************************************************
 函 数 名  : IPS_MNTN_CCORE_CCORE_RegExtFunc
 功能描述  : 提供给协议栈注册回调函数接口
 输入参数  : pCaptureCb            捕获报文或者配置命令的函数指针
             enCbType              pCaptureCb对应的类型
 输出参数  : 无
 返 回 值  : 成功:IPS_MNTN_OK，失败为IPS_MNTN_ERROR
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年8月3日
    作    者   : 冯家麟
    修改内容   : 新生成函数
  2.日    期   : 2013年7月30日
    作    者   : caikai
    修改内容   : VoLTE特性需要将其移植至V9

*****************************************************************************/
int IPS_MNTN_CCORE_RegExtFunc(IPS_MNTN_CCORE_CAPTURE_CB pCaptureCb, IPS_MNTN_CCORE_CB_TYPE_ENUM_UINT16 enCbType)
{
    if ( enCbType >= IPS_MNTN_CCORE_CB_TYPE_BUTT )
    {
        return IPS_MNTN_CCORE_ERROR;
    }

    g_astCallbackTbl[enCbType]  = pCaptureCb;

    return IPS_MNTN_CCORE_OK;
}

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

