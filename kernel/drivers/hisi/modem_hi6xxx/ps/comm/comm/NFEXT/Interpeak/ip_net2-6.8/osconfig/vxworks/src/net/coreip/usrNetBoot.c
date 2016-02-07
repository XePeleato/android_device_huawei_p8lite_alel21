/* usrNetBoot.c - network initialization, phase 2: validate boot parameters */

/*
 * Copyright (c) 2002-2008 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River license agreement.
 */
 
/*
modification history
--------------------
01r,20nov08,pgh  Fix the parsing of the other bootline parameter.
01q,13aug08,dtr  Fix DevNameGet to use unit number.WIND00130307
01p,07jul08,pgh  Add support for SATA.
01o,25aug07,tkf  Add usrNetBootIpv6AddrExtract() as part of a fix for
                 CQ:WIND00102553).
01n,29jan07,jmt  Add USB support to usrNetDevNameGet
01m,06oct06,kch  Added netInit boolean.
01l,19sep06,mze  added check in usrNetBoot for INCLUDE_SM_NET for WIND00058841  
01k,12sep05,vvv  fixed build error (SPR #105297)
01j,10feb04,elp  print error message when no device is found (replaces
		 attachment failure message), skip passDev Vxsim boot device.
01i,14dec03,ijm  if boot device is not configured, try "other", SPR#70284
01h,17jun03,vvv  removed call to usrNetmaskGet() (SPR #88786)
01g,15may03,spm  updated to Tornado 2.2 FCS release (added SPR #73762 fix)
01f,07may03,syy  Fixed compiler warnings
01e,06mar02,ham  moved usrNetmaskGet() from usrNetBootUtil.c
01d,05mar02,ham  fixed compilation warnings.
01c,28feb02,ham  call usrNetmaskGet in usrNetBoot.
01b,28feb02,ham  call usrNetDevNameGet in usrNetBoot.
01a,26feb02,ham  ported TOR3_1-FCS (01g).
*/

/*
DESCRIPTION
This file is included by the configuration tool to start a network device
according to the parameters in the boot line. It contains the initialization 
routines for the usrNetworkBoot initialization group and associated
components.

NOMANUAL
*/

/* includes */
#include "string.h"
#include "bootLib.h"

/* externals */

IMPORT BOOT_PARAMS sysBootParams;

#ifdef INCLUDE_NET_INIT_SKIP
BOOL netInit = FALSE;
#endif

BOOL netDevBootFlag = FALSE;        /* 
                                     * Set TRUE once a device is started
                                     * or if processing can't continue.
                                     */

BOOL    netAttachFlag = FALSE;      /* BSD or END device available? */
char * 	pDevName;                   /* Selects name for device. */
int     uNum;                       /* Selects the unit number */
int     netmask;                    /* Device netmask. */

/* The following holds the network device name from other bootline parameter. */

LOCAL char  otherNetDev[BOOT_DEV_LEN];

#ifndef NETBOOTDEF
#define NETBOOTDEF

char * 	pAddrString;                /* Selects address field for device. */
char *  pTgtName;                   /* Selects name for the target */
#endif

#if (defined (INCLUDE_SMEND) || defined (INCLUDE_DHCPC))
BOOL    backplaneBoot = FALSE;
#endif

/*******************************************************************************
*
* usrNetmaskGet - remove DHCP timestamp information from address
*
* This routine stores any value for the netmask found in the client 
* address string in the bootline, if needed. It is the initialization
* routine for the INCLUDE_NETMASK_GET component which is automatically
* added when the INCLUDE_NET_INIT component is used. The netmask
* value is not stored if further processing was disabled by an earlier 
* routine.
*
* RETURNS: N/A
*
* NOMANUAL
*/

void usrNetmaskGet (void)
    {

    /* 
     * Extract the netmask if processing is not complete. The flag will be
     * set to disable further processing if an error is detected in the
     * address string. It will also be set if the PPP or SLIP device have 
     * been created, since those devices handle any netmask value 
     * independently.
     */

    if (!netDevBootFlag)
        {
        netmask = 0;
        bootNetmaskExtract (pAddrString, &netmask);
        }
    }


/******************************************************************************
*
* usrNetDevNameGet - use alternate device name field if necessary
*
* This routine sets the network-related boot parameter processing to read
* the network device name from the "other" field instead of the "boot device"
* field when booting with a block device.
*
* RETURNS: N/A
*
* NOMANUAL
*/

void usrNetDevNameGet 
    (
    char ** pNetDev, 
    int *   pNetUnit
    )
    {

    *pNetDev = sysBootParams.bootDev;
    *pNetUnit = sysBootParams.unitNum;

    /* If booting from disk, configure additional network device, if any. */

    if ( (strncmp (sysBootParams.bootDev, "scsi", 4) == 0) ||
        (strncmp (sysBootParams.bootDev, "fs", 2) == 0) ||
        (strncmp (sysBootParams.bootDev, "ide", 3) == 0) ||
        (strncmp (sysBootParams.bootDev, "ata", 3) == 0) ||
        (strncmp (sysBootParams.bootDev, "fd", 2) == 0)  ||
        (strncmp (sysBootParams.bootDev, "tffs", 4) == 0) ||
        (strncmp (sysBootParams.bootDev, "usb", 3) == 0) )
        {
        if (sysBootParams.other [0] != EOS)
            {
            char *	pStr;
            int		unit;
            int     len = 0;

            /*
             * Fix up unitNum correctly. If the boot device
             * was specified in the other field, we need to derive
             * the unit number from here, rather than the one
             * derived from bootDev, since bootDev was a disk
             * device of some kind.
             *
             * The other field could also be used for other parameters,
             * in which case the device name is terminated by
             * BOOT_OTHER_FIELD_DELIMITER (';').
             */
 
            pStr = (char *)&sysBootParams.other;
            while (!isdigit ((int)*pStr) && (*pStr != EOS) &&
                   (*pStr != BOOT_OTHER_FIELD_DELIMITER))
                {
                pStr++;
                len++;
                }
 
            if (pStr != EOS && sscanf (pStr, "%d", &unit) == 1)
                {
                *pNetUnit = unit;
                }
            else
                {
                *pNetUnit = 0;
                }

            /* extract the net device name without the unit number */

            strncpy(otherNetDev, sysBootParams.other, len);
            otherNetDev[len] = EOS;
            *pNetDev = otherNetDev;
            }
        else
            {
	    *pNetDev = NULL;
	    if (_func_printErr)
	        (* _func_printErr) ("usrNetDevNameGet: no network device\n");
	    }
        }
    else if (strncmp (sysBootParams.bootDev, "passDev", 7) == 0)
    	{
	*pNetDev = NULL;
	}
    return;
    }

/*******************************************************************************
*
* usrNetBoot - device independent network boot handling
*
* This routine prepares for the network-related boot parameter processing.
* It is the initialization routine for the INCLUDE_NET_INIT component. 
*
* RETURNS: N/A
*
* NOMANUAL
*/

void usrNetBoot (void)
    {
    /* Set to access the bootline params by default. */

    pAddrString = sysBootParams.ead;
    pTgtName    = sysBootParams.targetName;
    uNum        = sysBootParams.unitNum;

    /*
     * Do nothing if another device is already initialized or an 
     * error was detected in the boot parameters.
     */

    if (netDevBootFlag == FALSE)
        usrNetDevNameGet (&pDevName, &uNum);
#ifdef INCLUDE_SM_NET
    if ((pDevName != NULL) &&
    ((strncmp (pDevName, "bp", 2) == 0) ||
     (strncmp (pDevName, "sm", 2) == 0)))
	{
	/* booting via backplane or SM networking */

	pAddrString = sysBootParams.bad;

	backplaneBoot = TRUE;
	}
#endif /*INCLUDE_SM_NET */

    return;
    }

#ifdef INET6
/*******************************************************************************
*
* usrNetBootIpv6AddrExtract - extract host and target IPv6 addresses.
*
* This routine extracts target and host IPv6 addresses stored in the 'other'
* field of the bootline string and returns them in 'pTgtAddr' and 'pHstAddr'
* respectively.
*
* At least one of 'pTgtAddr' or 'pHstAddr' should point to a non-NULL memory
* location, otherwise ERROR is returned.
*
* If either or both of 'pTgtAddr' and 'pHstAddr' are non-NULL, the routine
* assumes the size of each of the the buuffer is equal to 'addrLen'.
*
* INTERNAL
*
* The following description explains why the function is implemented the way
* it is implemented.
*
* Currently the 'other' field is used to store the name of the interface that
* the user wants to initialize in the case of the target is booted via
* non-network boot device. In this case, the interface name should be the
* first in the field. The target and host IPv6 addresses and any other user
* fiels should be delimited from the interface name field and from each other
* using BOOT_OTHER_FIELD_DELIMITER (';').
*
* The target IPv6 address field starts with 'ead6=' and is followed by the
* IPv6 address string.
*
* The host IPv6 address field starts with 'had6=' and is followed by the
* IPv6 address string.
*
* NOTE
*    The user will not be able to configure host and target IPv6 addresses
*    whose total lengths are greater than BOOT_OTHER_LEN (80). If any other
*    user fileds are configured in the other field the allowable total length
*    is even smaller by the length of the extra fields.
*
* RETURNS: OK or ERROR if the arguments are invalid.
*
* NOMANUAL
*/
STATUS usrNetBootIpv6AddrExtract
    (
    char * pTgtAddr,
    char * pHstAddr,
    size_t addrLen,
    BOOL * pHstLinkLocal
    )
{
    char * pOther;
    char * pNext = NULL;
    const char delim [] = {BOOT_OTHER_FIELD_DELIMITER,'\t',' ','\0'};
    char * pTok;
    STATUS status = OK;
    BOOL chkLinkLocal = FALSE;

    if (!pTgtAddr && !pHstAddr)
        return ERROR;

    if (pTgtAddr)
        pTgtAddr [0] = 0;

    if (pHstAddr)
        pHstAddr [0] = 0;

    /*
     * Host and target IPv6 addresses are expected to be stored in the
     * other field of the boot line. Copy the other field since this function
     * will modify it.
     */
    if((pOther = calloc (sizeof (sysBootParams.other), sizeof(char))) == NULL)
        return ERROR;

    if (pHstLinkLocal)
        *pHstLinkLocal = FALSE;

    strncpy (pOther, sysBootParams.other, sizeof (sysBootParams.other));
    pTok = strtok_r (pOther, delim, &pNext);
    while (pTok != NULL)
    {
        char * pSrc = pTok;
        char * pDst = NULL;

        if (pTgtAddr && strncmp (pTok, "ead6=", 4) == 0)
        {
            if (pTgtAddr [0])
            {
                if (_func_printErr)
                    _func_printErr ("Warning: Found two or more entries of"
                                    "\"ead6\";Ignoring previous\n");
            }
            pDst = pTgtAddr;
        }
        else if (pHstAddr && strncmp (pTok, "had6=", 4) == 0)
        {
            if (pHstAddr [0])
            {
                if (_func_printErr)
                    _func_printErr ("Warning: Found two or more entries of"
                                    "\"had6\";Ignoring previous\n");
            }
            pDst = pHstAddr;
            chkLinkLocal = TRUE;
        }

        if (pDst)
        {
            char * pRHS = NULL;
            int i;

            if ((strtok_r (pSrc, " =", &pRHS) == NULL) ||
                ((pSrc = strtok_r (NULL, " =", &pRHS)) == NULL))
            {
                if (_func_printErr)
                    _func_printErr ("Invalid configuration (). \n", pTok);
                    status = ERROR;
                goto freeMem;
            }

            if (pHstLinkLocal && (chkLinkLocal == TRUE) && strchr (pSrc, '%'))
                *pHstLinkLocal = TRUE;

            for (i = 0; i < addrLen - 1; i++)
            {
                if (!pSrc [i])
                    break;
                pDst [i] = pSrc[i];
            }
            pDst [i] = 0;
        }
        pTok = strtok_r (NULL, delim, &pNext);
    }

    if (pHstAddr && !pHstAddr [0])
    {
        if (_func_printErr)
            _func_printErr ("Warning: No host IPv6 address specified in"
                            "bootline.\n");
    }

    if (pTgtAddr && !pTgtAddr [0])
    {
        if (_func_printErr)
            _func_printErr ("Warning: No target IPv6 address specified in"
                            "bootline.\n");
    }

freeMem:
    free (pOther);

    return status;
}
#endif /* INET6 */


