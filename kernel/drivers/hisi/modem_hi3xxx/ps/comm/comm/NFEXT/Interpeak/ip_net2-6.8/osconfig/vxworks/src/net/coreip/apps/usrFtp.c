/* usrFtp.c - Support for FTP client */

/*
 * Copyright (c) 2002, 2004, 2006 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River license agreement.
 */

/*
modification history
--------------------
01j,26dec06,kch  Removed references to the obsolete coreip virtual stack.
01i,15feb06,mwv  remove the errnoSet (SPR#117656)
01h,16sep04,vvv  fixed warning
01g,22jul04,kc   Added missing #endif and fixed warnings.
01f,21jul04,spm  added virtual stack support
01e,21apr04,asr  fix compile warning
01d,25mar04,asr  added ftpLibInit call from usrFtpInit
01c,25feb04,ijm  bootrom/runtime starting ports should differ, SPR#93950
01b,23may02,elr  ftpLibDebugLevelSet (FTP_DEBUG_LEVEL) is now
                 ftpLibDebugOptionsSet (FTP_DEBUG_OPTIONS)
01a,20dec06,elr  created
*/

/*
DESCRIPTION
This file is used to include support for the ftp client.

NOMANUAL
*/

IMPORT STATUS ftpLibInit (long);
IMPORT UINT32 ftplTransientMaxRetryCount;
IMPORT UINT32 ftplTransientRetryInterval;
IMPORT STATUS ftpTransientFatalInstall();
IMPORT STATUS ftplDebugOptionsSet();

STATUS usrFtpInit (void);

#if (defined(INCLUDE_TCPV4) && defined(INCLUDE_NET_SYM_TBL))
/******************************************************************************
*
* advanceTcpPorts - advance TCP ports for runtime image
*
* This function advances the starting port for the runtime image by 2 to
* avoid delaying the loading of the symbol table.
* The bootrom FTP opens 2 sockets to download the runtime image.
* If the runtime image downloads the symbol table, the starting port,
* must be IPPORT_RESERVED_CFG + 2 or larger since
* IPPORT_RESERVED_CFG and IPPORT_RESERVED_CFG + 1 ports are in
* TIME_WAIT at the boot host.
*
* RETURNS: N/A
*
* NOMANUAL
*/

void advanceTcpPorts (void)
    {
    int fd;
    struct sockaddr_in sockaddr;
    int i;

    for (i=0; i<=1;++i)
        {
        if ((fd = socket (AF_INET, SOCK_STREAM, 0)) == ERROR)
            return;

        bzero ((char *)&sockaddr, sizeof (sockaddr));
        sockaddr.sin_family = AF_INET;
        sockaddr.sin_port = 0;
        sockaddr.sin_addr.s_addr = htonl (INADDR_ANY);

        bind (fd, (struct sockaddr *) &sockaddr, sizeof (sockaddr));
        close (fd);
        }

    return;

    }
#endif /* INCLUDE_NET_SYM_TBL */


/*******************************************************************************
*
* ftpTransientFatal - example applette to terminate FTP transient host responses
*
* ftpXfer will normally retry a command if the host responds with a 4xx
*     reply.   If this applette is installed,  it can immediately terminate
*     the retry sequence.
*
*
* RETURNS : TRUE - Terminate retry attempts
*           FALSE - Continue retry attempts
*
*
* SEE ALSO : ftpTransientFatalInstall(), ftpTransientConfigSet()
*
*/

LOCAL BOOL ftpTransientFatal 
    (
    UINT32 reply /* Three digit code defined in RFC #640 */
    )
    {
    switch (reply)
        {
        case (421): /* Service not available */
        case (450): /* File unavailable */
        case (451): /* error in processing */
        case (452): /* insufficient storage */
            { 
            /* yes, these are actually non-recoverable replies */
            return (TRUE); 
            break;
            }
            /* attempt to retry the last command */
        default:
        return (FALSE); 
        }
    }

/******************************************************************************
*
* usrFtpInit - initialize FTP parameters
*
* This routine configures ftp client parameters.
* defined by the user via the project facility.
*
* RETURNS: OK or ERROR
*
* NOMANUAL
*/

STATUS usrFtpInit (void)
    {
    ftpLibInit (FTP_TIMEOUT);
    ftplTransientMaxRetryCount = FTP_TRANSIENT_MAX_RETRY_COUNT;
    ftplTransientRetryInterval = FTP_TRANSIENT_RETRY_INTERVAL;
    ftpTransientFatalInstall ( FTP_TRANSIENT_FATAL );
    ftpLibDebugOptionsSet ( FTP_DEBUG_OPTIONS );

    return OK;
    }
