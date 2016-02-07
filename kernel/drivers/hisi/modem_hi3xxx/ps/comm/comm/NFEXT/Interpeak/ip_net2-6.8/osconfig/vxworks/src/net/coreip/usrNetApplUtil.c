/* usrNetApplUtil.c - applUtilLib initialization */

/*
 * Copyright (c) 2001-2006 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River license agreement.
 */

/*
modification history
--------------------
01d,26dec06,kch  Removed references to the obsolete coreip virtual stack.
01c,04may05,dlk  Fix SPR #108879. Remove erroneous SEM_Q_FIFO flag
                 in semMCreate() and semOpen() calls.
01b,09mar05,dlk  Initialize the logging semaphore here rather than in
                 applUtilLib.c for better scalability (SPR #104991).
01a,12sep04,dlk  written.
*/

/*
DESCRIPTION

Configlette for applUtilLib (Application/Stack logging facility)
initialization.

Note, this file is included by both usrNetwork.c and bootConfig.c.
Make sure that any changes you make here do not break the command
line vxWorks and bootrom builds.
*/

/* externs */

extern SEM_ID applUtilLogSem; /* non-virtualized */

extern STATUS applUtilInstInit (void *);

/****************************************************************************
*
* usrNetApplUtilInit() - initialize the application/stack logging utility
*
* RETURNS: N/A
*/

void usrNetApplUtilInit (void)
    {
    APPL_UTIL_CFG * pCfg;
    SEM_ID semId;
    APPL_UTIL_CFG cfg;

    pCfg = &cfg;
    pCfg->cfgh.len = sizeof (*pCfg);
    pCfg->fd = APPL_LOG_FD_CFG;
    pCfg->level = APPL_LOG_LEVEL_CFG;
    pCfg->fdKern = APPL_KERN_LOG_FD_CFG;
    pCfg->levelKern = APPL_KERN_LOG_LEVEL_CFG;

#if (_WRS_VXWORKS_MAJOR >= 6 && defined (INCLUDE_RTP))

    /* create the semaphore */
    semId = semOpen ("/applUtilLogSem",
		     SEM_TYPE_MUTEX,
		     SEM_FULL,					/* ignored */
		     SEM_Q_PRIORITY | SEM_INVERSION_SAFE | SEM_DELETE_SAFE,
		     OM_CREATE | OM_EXCL, /* mode; creating */
		     NULL /* context, not used */);

#else  /* _WRS_VXWORKS_MAJOR < 6, or no RTP support */

    semId = semMCreate (SEM_Q_PRIORITY | SEM_INVERSION_SAFE | SEM_DELETE_SAFE);

#endif /* _WRS_VXWORKS_MAJOR < 6, or no RTP support */

#ifndef _FREE_VERSION
    if (semId == NULL)
	{
	if (_func_printErr != NULL)
	    _func_printErr ("usrNetApplUtilInit: can't create semaphore\n");
	}
#endif

    applUtilLogSem = semId;
    applUtilInstInit (pCfg);
    }

