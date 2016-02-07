/* usrBootLine.c - Routines to parse the bootline */

/*
 * Copyright (c) 1998-2007 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River license agreement.
 */

/*
modification history
--------------------
01j,27aug07,tkf  Support IPv6 Only configuration.
01i,10apr07,rlg  Defect 92458 - removal of previous attempt to fix 
                 usrBootLineInit - needed to make fix in bootInit not here.
01h,21mar07,rlg  Defect WIND00091114 - usrBootLineInit has problems -
                 discovered in testing bootroms for 6.5
01g,09nov06,kch  Replaced net/inet.h with inetLib.h.
01f,06oct06,kch  Added net/inet.h include to fix warning.
01e,19may06,tkf  Add usrBootLineGet() routine.
01d,08aug05,wap  Change dependencies for micro stack
01d,08aug05,wap  Change dependencies for micro stack
01c,15jun04,j_b  handle case where bootline is specified on boot cmd line
01b,27feb02,ham  ported from TOR3_1-FCS (01a).
01a,18aug98,ann  created this configlette from usrNetwork.c
*/

/*
DESCRIPTION
This configlette is included when the INCLUDE_BOOT_LINE_INIT component
is chosen. It contains the routines to parse the bootline parameters.

NOMANUAL
*/

#include <inetLib.h>

/******************************************************************************
*
* usrBootLineInit - initialize system boot line
*
* Initializes system boot line as per specified start type.  If the boot
* line is empty or this is a COLD boot, i.e. with CLEAR option to clear
* memory, then the boot line is initialized from non-volatile ram, if any,
* otherwise from the compiled in default boot line.
*
* NOMANUAL
*/

void usrBootLineInit
    (
    int startType
    )
    {
    if (*BOOT_LINE_ADRS == EOS)
        {
        /*
         * empty boot line --
         * initialize boot line 
         */
        if ((sysNvRamGet (BOOT_LINE_ADRS, BOOT_LINE_SIZE, 0) == ERROR) ||
            (*BOOT_LINE_ADRS == EOS) || (*BOOT_LINE_ADRS == (char) -1))
            {
                /* no non-volatile RAM -- use default boot line */
   
                strcpy (BOOT_LINE_ADRS, DEFAULT_BOOT_LINE);
            }
        }
    }

/******************************************************************************
*
* usrBootLineCrack - interpret and verify boot line
*
* This routine interprets the specified boot line and checks the validity
* of certain parameters.  If there are errors, a diagnostic message is
* printed.
*
* NOMANUAL
*/

STATUS usrBootLineCrack 
    (
    char * bootString,
    BOOT_PARAMS *pParams
    )
    {
    FAST char *	pS;

    pS = bootStringToStruct (bootString, pParams);
    if (*pS != EOS)
	{
	bootParamsErrorPrint (bootString, pS);
	return (ERROR);
	}

#if defined(INET) && (defined(INCLUDE_NET_INIT) || defined(INCLUDE_NET_DEFER_INIT))
    /* check inet addresses */

    if ((checkInetAddrField (pParams->ead, TRUE) != OK) ||
	(checkInetAddrField (pParams->bad, TRUE) != OK) ||
	(checkInetAddrField (pParams->had, FALSE) != OK) ||
	(checkInetAddrField (pParams->gad, FALSE) != OK))
	{
	return (ERROR);
	}
#endif

    return (OK);
    }

#if defined(INET) && (defined(INCLUDE_NET_INIT) || defined(INCLUDE_NET_DEFER_INIT))
/******************************************************************************
*
* checkInetAddrField - check for valid inet address in boot field
*
* RETURNS: OK, or ERROR if invalid inet address
*
* NOMANUAL
*/

STATUS checkInetAddrField 
    (
    char *pInetAddr,
    BOOL subnetMaskOK
    )
    {
    char inetAddr [30];
    int netmask;

    bzero (inetAddr, 30);   /* SPR 6326 - prevents error if address invalid. */

    if (*pInetAddr == EOS)
	return (OK);

    strncpy (inetAddr, pInetAddr, sizeof (inetAddr));

    if (subnetMaskOK)
	{
	if (bootNetmaskExtract (inetAddr, &netmask) < 0)
	    {
	    printf ("Error: invalid netmask in boot field \"%s\".\n", inetAddr);
	    return (ERROR);
	    }
	}

    if (inet_addr (inetAddr) == (u_long) ERROR)
	{
	printf ("Error: invalid inet address in boot field \"%s\".\n",inetAddr);
	return (ERROR);
	}

    return (OK);
    }
#endif

/******************************************************************************
*
* usrBootLineParse - parse the boot line
*/ 

STATUS usrBootLineParse
    (
    char* bootString
    )
    {
    BOOT_PARAMS	params;

    usrBootLineInit (sysStartType);     /* crack the bootline */

    /* interpret boot command */
    if (bootString == NULL)
	bootString = BOOT_LINE_ADRS;

    if (usrBootLineCrack (bootString, &params) != OK)
	return (ERROR);

    /* fill in any default values specified in configAll */

    if ((params.hostName [0] == EOS) &&			/* host name */
        (strcmp (HOST_NAME_DEFAULT, "") != 0))
	{
	strncpy (params.hostName, HOST_NAME_DEFAULT, BOOT_HOST_LEN);
	printf ("Host Name: %s \n", params.hostName);
	}

    if ((params.targetName [0] == EOS) &&		/* targetname */
        (strcmp (TARGET_NAME_DEFAULT, "") != 0))
	{
	strncpy (params.targetName, TARGET_NAME_DEFAULT, BOOT_HOST_LEN);
	printf ("Target Name: %s \n", params.targetName);
	}

    if ((params.usr [0] == EOS) &&			/* user name (u) */
        (strcmp (HOST_USER_DEFAULT, "") != 0))
	{
	strncpy (params.usr, HOST_USER_DEFAULT, BOOT_USR_LEN);
	printf ("User: %s \n", params.usr);
	}

    if ((params.startupScript [0] == EOS) &&	/* startup script (s) */
        (strcmp (SCRIPT_DEFAULT, "") != 0))
	{
	strncpy (params.startupScript, SCRIPT_DEFAULT, BOOT_FILE_LEN);
	printf ("StartUp Script: %s \n", params.startupScript);
	}

    if ((params.other [0] == EOS) &&			/* other (o) */
        (strcmp (OTHER_DEFAULT, "") != 0))
	{
	strncpy (params.other, OTHER_DEFAULT, BOOT_OTHER_LEN);
	printf ("Other: %s \n", params.other);
	}

    if (params.passwd [0] == EOS)			/* password */
	strncpy (params.passwd, HOST_PASSWORD_DEFAULT, BOOT_PASSWORD_LEN);

    /* fill in system-wide variables */

    bcopy ((char *) &params, (char *) &sysBootParams, sizeof (sysBootParams));

    sysFlags = params.flags;
    strcpy (sysBootHost, params.hostName);	/* backwards compatibility */
    strcpy (sysBootFile, params.bootFile);	/* backwards compatibility */
    /* set processor number: may establish vme bus access, etc. */

    if (_procNumWasSet != TRUE)
	{
    	sysProcNumSet (params.procNum);
	_procNumWasSet = TRUE;
	}

    return OK;
    }

/********************************************************************************
* usrBootLineGet - Return the boot line address
*
* This routine returns the boot line address. This prevents allows to
* access the boot line without having to include config.h.
*
* NOMANUAL
*/
char * usrBootLineGet
    (
    )
{
    return (char *)BOOT_LINE_ADRS;
}
