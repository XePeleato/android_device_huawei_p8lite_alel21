/* usrGetnameinfo.c - getnameinfo initialization */

/*
 * Copyright (c) 2006 Wind River Systems, Inc.
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
01b,13feb07,dlk  Moved RTP getaddrinfo() initialization to
                 INCLUDE_GETNAMEINFO_SYSCTL component.
01a,29sep06,tlu  initial creation
*/

/*
DESCRIPTION
This file is the configlette for INCLUDE_GETADDRINFO. 

NOMANUAL
*/

STATUS usrGetaddrinfoInit (void)
    {
    return (OK);
    }
