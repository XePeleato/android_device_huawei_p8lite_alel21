/* usrGetnameinfo.c - getnameinfo initialization */

/*
 * Copyright (c) 2003-2006 Wind River Systems, Inc.
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
01f,13feb07,dlk  Moved RTP getnameinfo() initialization to
                 INCLUDE_GETNAMEINFO_SYSCTL component.
01e,26dec06,kch  Removed references to the obsolete coreip virtual stack.
01d,02oct06,tlu  Remove un-necessary gethostbyaddrInit() and virtual stack
01c,20may05,niq  Fix virtual stack initialization error
01b,03mar05,wap  Add error to notify user of missing component
01a,05dec03,vvv  written
*/

/*
DESCRIPTION
This file is the configlette for INCLUDE_GETNAMEINFO. 

NOMANUAL
*/

STATUS usrGetnameinfoInit (void)
    {
    return (OK);
    }
