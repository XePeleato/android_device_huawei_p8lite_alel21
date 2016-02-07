/* usrNetBootConfig.c - Configure the network boot device */

/*
 * Copyright (c) 2001-2008 Wind River Systems, Inc.
 *
 * The right to copy, distribute or otherwise make use of this software
 * may be licensed only pursuant to the terms of an applicable Wind River
 * license agreement.
 */

/*
modification history
--------------------
02d,07jul08,pgh  Add support for SATA.
02c,15oct07,uol  Adjustment of fix for defect WIND00104919, limit interface
                 names to IFNAMSIZ to adhere to ipnet_cmd_ifconfig().
02b,09oct07,uol  Fixed defect WIND00104919, allow 64 byte device name in
                 usrNetBootConfig(), and return error if longer.
02a,27aug07,tkf  Add IPv6Only configuration support.
01z,28apr07,tkf  Merge from NOR-SMP sandbox.
01y,02feb07,tkf  Use ipcom_mcmd_ifconfig() and ipcom_mcmd_route() instead of
                 ipnet_cmd_ifconfig() and ipnet_cmd_route().
01x,29jan07,jmt  Add support for USB disks
01x,02feb07,tkf  Use function pointers to access ifconfig and route commands.
01w,05oct06,kch  Replaced do_autoconfig() and usrNetIpv6Attach() with
                 usrNetBootAutoConfig() and addGateway() with
                 usrNetBootAddGateway(). Updated usrNetBootConfig() to work
                 with IPNet.
01v,24feb05,spm  removed unneeded routeFormat values and macros (SPR #100995)
01u,24feb05,spm  performance updates and code cleanup (SPR #100995)
01t,30sep04,dlk  Removed obsolete declaration of random().
01s,28sep04,niq  Zero out the sockaddr structures in the addGateway routine.
01r,17sep04,niq  scale out routing sockets
01q,04aug04,vvv  check inet_addr() return value (SPR #94763)
01p,19may04,snd  Fixed SPR#89215,DHCP client unable to obtain lease when
                 booting from non-network device 
01o,07may04,vvv  fixed warnings (SPR #96364)
01n,10feb04,elp  usrNetBootConfig() returns without message when devName==NULL.
01m,06jan04,rp   fixed SPR 92684
01l,16oct02,ham  fixed improper default gateway configuration (SPR 83122).
01k,11oct02,ham  fixed improper netIoctl return value check.
01j,04oct02,ham  fixed improper ifioctl return value check (SPR 82778).
01i,18sep02,nee  removing ifr_type for SIOCSIFFLAGS
01h,22jul02,rvr  dynamic memory allocation/free for rtmbuf (teamf1)
01g,15jul02,nee  SIOCSIFADDR replaced by SIOCSIFADDR+SIOCSIFFLGS
01f,27feb02,ham  wrote addGateway to add default gateway by routing socket.
01e,06feb02,ham  renamed to usrNetBootConfig due to ifconfig.
01d,05dec01,ham  consolidated usrNetConfigIf.c.
*/

/* 
DESCRIPTION
This file is included by the configuration tool to setup a network device
according to the parameters in the boot line. It contains the initialization
routine for the INCLUDE_NET_BOOT_CONFIG component.
*/ 

typedef int (*ipcom_cmd_func_type)(int argc, char ** argv);

/* extern */

STATUS usrNetBootConfig ( char *, int, char *, int , char *); 

IMPORT ipcom_cmd_func_type ipnet_cmd_ifconfig_hook;
IMPORT ipcom_cmd_func_type ipnet_cmd_route_hook;

#if defined(PRJ_BUILD)
IMPORT BOOT_PARAMS sysBootParams;
#endif /* defined PRJ_BUILD */

/*
 * variable to determine, whether the vxWorks image is on
 * disk or not
 */
BOOL diskBoot = FALSE;


/*******************************************************************************
*
* usrCheckNetBootConfig - configure the network boot device, check if address
*                         field is present
*
* This routine is the initialization routine for the INCLUDE_NET_BOOT_CONFIG
* component. It assigns the IP address, netmask and default gateway if
* specified to the boot device.
*
* RETURNS: N/A
*
* NOMANUAL
*/

STATUS usrCheckNetBootConfig
    (
    char *      devName,                /* device name e.g. "fei" */
    int         unit,                   /* unit number */
    char *      addr,                   /* target ip address */
    int         netmask,                /* subnet mask */
    char *      gateway                 /* default gateway */
    )
    {
    /* Check if the vxWorks Image has been loaded from the disk */
    diskBoot = FALSE;

    /* find network boot device configuration */
    if ( (strncmp (sysBootParams.bootDev, "scsi", 4) == 0) ||
        (strncmp (sysBootParams.bootDev, "fs", 2) == 0) ||
        (strncmp (sysBootParams.bootDev, "ide", 3) == 0) ||
        (strncmp (sysBootParams.bootDev, "ata", 3) == 0) ||
        (strncmp (sysBootParams.bootDev, "fd", 2) == 0)  ||
        (strncmp (sysBootParams.bootDev, "tffs", 4) == 0) ||
        (strncmp (sysBootParams.bootDev, "usb", 3) == 0) )
          diskBoot = TRUE;

    /*
     * If we are booting from disk, then the target IP address
     * string need not be present. Make sure DHCP client option
     * is activated
     */

    if (diskBoot)
        {
        if((addr != NULL) && (addr[0] != EOS))
            return (usrNetBootConfig (devName, unit, addr, netmask, gateway));
        }

    return (OK);
}

#ifdef INET
/*******************************************************************************
*
* usrNetBootAddGateway - add default gateway for IPv4 network
*
* This routine is called from usrNetBootConfig() to add default gateway for 
* IPv4 network.
*
* RETURNS: OK or ERROR
*
* NOMANUAL
*/

LOCAL STATUS usrNetBootAddGateway
    (
    char *	dstaddr,   /* destination address */
    char *	gateway    /* gateway */
    )
    {
    char *argv[] = {
        "route",
        "add",
        "-silent",
        "-inet",
        "-static",
        "-net",
        "-prefixlen",
        IP_NULL,
        IP_NULL,
        IP_NULL,
        IP_NULL,
    };
    char prefix_len_str[4];
    int argc = 7;
    int ret;

    sprintf(prefix_len_str, "%d", 0);
    argv[argc++] = prefix_len_str;
    argv[argc++] = dstaddr;
    argv[argc++] = gateway;

    ret = ipnet_cmd_route_hook(argc, argv);
    if (ret == IP_ERRNO_EEXIST)
        return OK;
    return ret == IPCOM_SUCCESS ? OK : ERROR;
    }
#endif /* INET */
    
/*****************************************************************************
*
* usrNetBootAutoConfig - bring up the network boot device
* 
* This routine is called from usrNetBootConfig() to bring up the network boot
* device. In previous releases of the Wind River network stack, IPv4 and IPv6
* protocols can be attached to the device separately. In the current release,
* the device will be attached to all supported network-layer protocols built
* into the stack at library archive-build time.
*
* RETURNS: OK or ERROR
*
* NOMANUAL
*/

LOCAL STATUS usrNetBootAutoConfig
    (
    char * ifname     /* device name e.g. "fei0" */
    )
    {
    char *argv[] = {
        "ifconfig",
        "-silent",
        IP_NULL,
        "up",
        IP_NULL
    };

    argv[2] = ifname;
    return ipnet_cmd_ifconfig_hook(4, argv);
    }

/*******************************************************************************
*
* usrNetBootConfig - configure the network boot device
*
* This routine is the initialization routine for the INCLUDE_NET_BOOT_CONFIG
* component. It assigns the IP address, netmask and default gateway if
* specified to the boot device.
*
* RETURNS: N/A
*
* NOMANUAL
*/

STATUS usrNetBootConfig
    (
    char *      devName,                /* device name e.g. "fei" */
    int         unit,			/* unit number */
    char *      addr,                   /* target ip address */
    int         netmask,                /* subnet mask */
    char *	gateway			/* default gateway */
    )
    {
    char ifname [IFNAMSIZ + 10 + 1];    /* devName + unit + '\0' */
    char *prefix_len;
    char inet_prefix_len_str[8];
    int mask;
    int argc = 5; 
    char *argv[] = {
        "ifconfig",
        "-silent",
        IP_NULL,
        "inet",
        "add",
        IP_NULL,
        IP_NULL,
        IP_NULL,
        IP_NULL
    };

    /*
     * Do nothing if another device is already configured or an
     * error was detected in the boot parameters.
     */

    if (devName == NULL || devName[0] == EOS || addr == NULL || addr[0] == EOS ||
	strlen(devName) >= IFNAMSIZ)
        {
        if (_func_printErr)
            (*_func_printErr) ("usrNetBootConfig: Invalid Argument\n");
        return (ERROR);
        }

#if 0    
    /* ppp is not attached yet, just return */
    if (strncmp (devName, "ppp", 3) == 0)
        return (OK);
#endif

    /* build interface name */
    sprintf (ifname, "%s%d", devName, unit);
    argv[2] = ifname;

    /* interface name must not exceed IFNAMSIZ */
    if (strlen(ifname) >= IFNAMSIZ)
        {
        if (_func_printErr)
            (*_func_printErr) ("usrNetBootConfig: Interface name too long\n");
        return (ERROR);
        }

    /* set inet addr and subnet mask */
    argv[argc++] = addr;
    argv[argc++] = "prefixlen";

    mask = htonl(netmask);
    prefix_len = inet_prefix_len_str;
    sprintf(inet_prefix_len_str, "%d", ipcom_mask_to_prefixlen(&mask, 32));
    argv[argc++] = prefix_len;
    ipnet_cmd_ifconfig_hook(argc, argv);

    (void)usrNetBootAutoConfig (ifname);    

    /* set default gateway from give boot parameter */

#ifdef INET
    if (gateway != NULL && gateway[0] != EOS)
        {
        if (usrNetBootAddGateway ("0.0.0.0", gateway) == ERROR)
            {
            if (_func_printErr)
                (*_func_printErr) ("usrNetBootConfig: Failed addGateway\n");
            return ERROR;
            }
        }
#endif
    
    return OK;
    }

#ifdef INCLUDE_NET_BOOT_IPV6_CFG
void usrCheckNetBootIpv6Config (void)
{
    /*
     * Booting over IPv6 address is not supported, which means that
     * the boot net device will not be configured for IPv6 address.
     * Therefore, if the user provided the IPv6 addresses in the
     * bootstring, we will configure the boot net device with the
     * addresses regardless of whether 'diskBoot' is TRUE or FALSE.
     *
     * This is so that we are able to download symbol table if
     * necessary.
     */
    char * pTgtIpv6Addr = NULL;
    char * pHstIpv6Addr = NULL;

    if ((pTgtIpv6Addr = calloc (USR_NET_IP6ADDR_LEN, sizeof(char))) &&
        (pHstIpv6Addr = calloc (USR_NET_IP6ADDR_LEN, sizeof(char))))
    {
        pTgtIpv6Addr [0] = 0;
        pHstIpv6Addr [0] = 0;
        if (usrNetBootIpv6AddrExtract (pTgtIpv6Addr, pHstIpv6Addr,
                                       USR_NET_IP6ADDR_LEN,
                                       NULL) == ERROR)
            printf ("Could not extract IPv6 address from boot string.\n");
        else
        {
            if (usrNetBootIpv6Config (pDevName, uNum, pTgtIpv6Addr, 0, 0)
                == ERROR)
            printf ("Could not configure IPv6 address on %s%d\n",
                    pDevName, uNum);
        }
    }
    else
    {
        /* Ignore error */
    }
    if (pTgtIpv6Addr)
        free (pTgtIpv6Addr);
    if (pHstIpv6Addr)
        free (pHstIpv6Addr);
}

/*******************************************************************************
*
* usrNetBootIpv6Config - configure the network boot device
*
* This routine is the initialization routine for the INCLUDE_NET_BOOT_CONFIG
* component. It assigns the IP address, netmask and default gateway if
* specified to the boot device.
*
* RETURNS: N/A
*
* NOMANUAL
*/

STATUS usrNetBootIpv6Config
    (
    char *      devName,                /* device name e.g. "fei" */
    int         unit,			/* unit number */
    char *      addr,                   /* target ip address */
    int         netmask,                /* subnet mask */
    char *	gateway			/* default gateway */
    )
    {
#define USR_NET_PREFIXLEN_DELIM '/'
    const char delim [] = {USR_NET_PREFIXLEN_DELIM, ' ', '\t', 0};
    char * inet6_addr;
    char * pNext;
    int argc = 6; 
    char ifname [20];
    char *prefix_len;
    char *argv[] = {
        "ifconfig",
        "-silent",
        IP_NULL,
        "inet6",
        "add",
        IP_NULL,
        IP_NULL,
        IP_NULL,
        IP_NULL
    };

    /*
     * Do nothing if another device is already configured or an
     * error was detected in the boot parameters.
     */

    if (devName == NULL || devName[0] == EOS || addr == NULL || addr[0] == EOS)
        {
        if (_func_printErr)
            (*_func_printErr) ("usrNetBootConfig: Invalid Argument\n");
        return (ERROR);
        }

#if 0    
    /* ppp is not attached yet, just return */
    if (strncmp (devName, "ppp", 3) == 0)
        return (OK);
#endif

    /* build interface name */

    sprintf (ifname, "%s%d", devName, unit);
    argv[2] = ifname;

    if ((inet6_addr = strtok_r (addr, delim, &prefix_len)))
    {
        argv [5] = inet6_addr;
        if (prefix_len && (prefix_len = strtok_r (prefix_len, delim, &pNext)))
        {
            argv [argc++] = "prefixlen";
            argv [argc++] = prefix_len;
        }
    }
    else
    {
        if (_func_printErr)
            (*_func_printErr) ("usrNetBootConfig: Invalid IPv6 address %s\n",
                               addr);
        return ERROR;
    }

    ipnet_cmd_ifconfig_hook(argc, argv);

    (void)usrNetBootAutoConfig (ifname);    

    return OK;
    }
#endif /* INCLUDE_NET_BOOT_IPV6_CFG */
