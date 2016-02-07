/* usrNetRemoteCfg.c - network remote I/O support */

/* Copyright 1992 - 2004 Wind River Systems, Inc. */

/*
modification history
--------------------
01d,27aug07,tkf  Initialize remNymConnRetrials.
01c,14sep04,jmp	 Cleaned-up PASSFS specific code in usrNetRemoteCreate().
01b,22oct03,rp   vxSim fixes
01a,06mar02,ham  ported from TOR3_1-FCS (01d)
*/

/*
DESCRIPTION
This file is used to configure and initialize the VxWorks networking support
for remote I/O access. The file contents are added to the project 
configuration file when FOLDER_NET_REM_IO is included.  

NOMANUAL
*/

/* externs */
extern int remNumConnRetrials;

/*******************************************************************************
*
* usrNetRemoteCreate - enable remote I/O access over the network
*
* This routine creates a device for network remote I/O access if
* INCLUDE_NET_REM_IO. It is called by the configuration tool as the 
* initialization routine for that component.
* 
* RETURNS: N/A
*
* NOMANUAL
*/

void usrNetRemoteCreate (void)
    {

#if (!(defined(INCLUDE_PASSFS)) && !(defined(INCLUDE_NTPASSFS)))   
    char        devName [MAX_FILENAME_LENGTH];  /* device name */
    int 	protocol;
#endif	/* !INCLUDE_PASSFS && !INCLUDE_NTPASSFS */

    remLastResvPort = 1010;     /* pick an unused port number so we don't *
                                 * have to wait for the one used by the *
                                 * by the bootroms to time out */

#ifdef INCLUDE_FTP
    _func_ftpLs = ftpLs;               /* init ptr to ftp dir listing routine */
#else
    _func_ftpLs = NULL; 
#endif

#if defined(INET6) && !defined(INET)
    /*
     * Ipv6 Only Stack case: INCLUDE_FTP would not have been defined.
     * Use the IPv6 FTP dir listing if INCLUDE_FTP6 is defined.
     */
#ifdef INCLUDE_FTP6
    _func_ftpLs = ftp6Ls;               /* init ptr to ftp dir listing routine */
#else
    _func_ftpLs = NULL; 
#endif
    remNumConnRetrials = REM_NUM_CONN_RETRIALS;

#endif /* defined(INET6) && !defined(INET) */

#ifdef INCLUDE_REMLIB
    _func_remCurIdGet = (FUNCPTR) remCurIdGet;
    _func_remCurIdSet = (FUNCPTR) remCurIdSet;
#else
    _func_remCurIdGet = NULL;
    _func_remCurIdSet = NULL;
#endif

    /* initialize the network remote file driver */

    netDrv ();

#if (!(defined(INCLUDE_PASSFS)) && !(defined(INCLUDE_NTPASSFS)))
    /*
     * Create netDrv device.
     * NOTE: This is not done when PASSFS is included. Moreover, the device
     * names for passFs and netDrv are the same, e.g. <hostname>, this
     * would bring conflicts.
     */

    _func_netLsByName = netLsByName;	/* init ptr to netDrv listing routine */
    sprintf (devName, "%s:", sysBootParams.hostName);	/* make dev name */
    protocol = (sysBootParams.passwd[0] == EOS) ? 0 : 1;/* pick protocol */

    /* Create device. */

    netDevCreate (devName, sysBootParams.hostName, protocol);
#endif	/* !INCLUDE_PASSFS && !INCLUDE_NTPASSFS */

#ifdef INCLUDE_REMLIB
    /* Set the user id and current directory. */

    iam (sysBootParams.usr, sysBootParams.passwd);
#endif

#if (!(defined(INCLUDE_PASSFS)) && !(defined(INCLUDE_NTPASSFS)))
    /* Set the current default path, for PassFs this done by usrPassFsInit() */

    ioDefPathSet (devName);
#endif	/* !INCLUDE_PASSFS & !INCLUDE_NTPASSFS */
    return;
    }
