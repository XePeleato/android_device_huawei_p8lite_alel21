/* usrNetSysctl.c - Network default configuration */

/*
 * Copyright (c) 1992-2007 Wind River Systems, Inc.
 *
 * The right to copy, distribute or otherwise make use of this software
 * may be licensed only pursuant to the terms of an applicable Wind River
 * license agreement. No license to Wind River intellectual property rights
 * is granted herein. All rights not licensed by Wind River are reserved
 * by Wind River.
 */

/*
modification history
--------------------
01j,13feb07,dlk  Remove 'foreign' argument to netCoreSysctlInit().
01i,26dec06,kch  Added hostnameSysctlSetup().
01h,06dec06,tkf  Cleanup for IPNET integration.
01g,13apr05,kch  Corrected virtualization problem.
01f,16sep04,vvv  fixed warning
01e,31aug04,vvv  fixed sysctl initialization
01d,06sep02,kal  should be VS Non opt component, not application
01c,05sep02,kal  fixed prototypes for vsLib
01b,20aug02,hgo  virtualization
01a,05feb02,ham  wrote.
*/

/*
DESCRIPTION

This configlette contains the initialization routine for the INCLUDE_NET_SYSCTL
component.

NOMANUAL
*/

/* includes */
#include <sys/sysctl.h>

IMPORT STATUS netCoreSysctlInit (void);
IMPORT FUNCPTR _netSysctl;
IMPORT STATUS netSysctl ();
IMPORT STATUS netSysctlInit (BOOL);

#ifdef INCLUDE_SYSCTL_CLI
IMPORT FUNCPTR _netSysctlCli;
IMPORT int netSysctlCli (int argc, char ** pArgv, char *  pOpts);
#endif /* INCLUDE_SYSCTL_CLI */

#ifdef INCLUDE_NET_HOST_SETUP
IMPORT void hostnameSysctlSetup (void);
#endif /* INCLUDE_NET_HOST_SETUP */


void usrNetSysctlInit (void)
{
#ifdef INCLUDE_IPNET
    _netSysctl = (FUNCPTR)netSysctl;

#ifdef INCLUDE_SYSCTL_CLI
    _netSysctlCli = (FUNCPTR)netSysctlCli;
#endif
#endif

    netCoreSysctlInit ();

#ifdef  INCLUDE_NET_HOST_SETUP
    hostnameSysctlSetup();
#endif /* INCLUDE_NET_HOST_SETUP */
}
