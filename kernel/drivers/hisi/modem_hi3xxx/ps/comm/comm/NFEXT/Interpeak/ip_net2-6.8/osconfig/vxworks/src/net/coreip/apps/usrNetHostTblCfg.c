/* usrNetHostTblCfg.c - host table initialization */

/*
 * Copyright (c) 2002-2006 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River license agreement.
 */

/*
modification history
--------------------
01l,25aug07,tkf  Add support for adding host using IPv6 addresses.
01k,13feb07,dlk  RTP host table sysctl initialization is done by
                 INCLUDE_HOST_TBL_SYSCTL.
01j,26dec06,kch  Removed references to the obsolete coreip virtual stack.
01i,12sep05,vvv  added definitions to fix build issue (SPR #105297)
01h,03mar05,wap  Add error to notify user of missing component
01g,08oct04,kc   Fixed compiler error for virtual stack.
01f,20sep04,dlk  Return a value from usrNetTblStart().
01e,19sep04,spm  cleanup file: added missing comment start
01d,19sep04,spm  updated host table initialization for virtual stack startup
01c,05dec03,vvv  added RTP support
01b,18nov02,hgo  make it an optional component
01a,24jul02,hgo  written
*/

/*
DESCRIPTION
This file is configlette for INCLUDE_HOST_TBL. 

NOMANUAL
*/

#include <hostLib.h>

#ifndef NETBOOTDEF
#define NETBOOTDEF

char *  pAddrString;                /* Selects address field for device. */
char *  pTgtName;                   /* Selects name for the target */
#endif

#ifdef INCLUDE_NET_BOOT_IPV6_CFG
IMPORT STATUS usrNetBootIpv6AddrExtract (char * pTgtAddr, char * pHstAddr,
                                         size_t addrLen, BOOL * pHstLinkLocal);
#endif

/*******************************************************************************
*
* usrNetHostTblSetup - assign initial host table entries
*
* This routine initializes the internal host table and adds the
* hostnames and IP addresses for the local vxWorks target and the
* boot host, if available. It also creates a default host entry for
* the loopback address.
*
* The boot host information is required for the installation of any
* network remote I/O device.
*
* RETURNS: OK, or ERROR if setup failed.
*
* NOMANUAL
*/

LOCAL STATUS usrNetHostTblSetup (void)
    {
    STATUS result;

    if (hostTblInit () == ERROR)
        return (ERROR);

#if defined(INET)
    /*
     * Create initial host table entries for the loopback address,
     * and for the local and remote IP addresses, if available.
     * The remote host information is required for the installation
     * of any network remote I/O device. The remaining startup sequence
     * may use a network I/O device, if it downloads a symbol table.
     */

    result = hostTblSetup (sysBootParams.hostName, sysBootParams.had,
                           pTgtName, pAddrString);

    return (result);
#endif /* defined(INET) */

#ifdef INCLUDE_NET_BOOT_IPV6_CFG
    {
#ifdef INET6_ADDR_LEN
#define USR_NET_IP6ADDR_LEN (INET6_ADDR_LEN + 18)
#else
#define USR_NET_IP6ADDR_LEN 64
#endif
        /*
         * Ipv6 Only Stack case: to allow symbol table download we need to
         * setup the static host entries using IPv6 addresses.
         */
        char * pTgtIpv6Addr;
        char * pHstIpv6Addr;

        if (((pTgtIpv6Addr = calloc (USR_NET_IP6ADDR_LEN, sizeof(char))) == NULL) ||
            ((pHstIpv6Addr = calloc (USR_NET_IP6ADDR_LEN, sizeof(char))) == NULL))
            return ERROR;

        pTgtIpv6Addr [0] = 0;
        pHstIpv6Addr [0] = 0;

        if (usrNetBootIpv6AddrExtract (pTgtIpv6Addr, pHstIpv6Addr,
                                       USR_NET_IP6ADDR_LEN, NULL) == OK)
            result = hostTblSetup (sysBootParams.hostName, pHstIpv6Addr,
                                   pTgtName, pTgtIpv6Addr);
        else
            result = ERROR;

        free (pTgtIpv6Addr);
        free (pHstIpv6Addr);
    }
#endif /* INCLUDE_NET_BOOT_IPV6_CFG */
    }

