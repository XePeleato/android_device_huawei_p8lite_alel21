/* wdbEnd.c - WDB END communication initialization library */

/* Copyright 1998 - 2003 Wind River Systems, Inc. */

/*
modification history
--------------------
01g,15may03,spm  reversed version 01f change to permit command line build
01f,20feb03,ann  copy the IP address of the interface from the boot params
                 instead of making a call to ifAddrGet in the WDB END driver.
01e,20mar02,jhw  Remove network initialization calls. (SPR 73517)
01d,27feb02,j_s  return error if end device table is empty (SPR 73604).
01c,21nov01,jhw  Get WDB END device from sysBootParams (SPR 71595).
01b,30jul99,pul  add NPT support
01a,18may98,dbt   written based on usrWdb.c ver 02t
*/

/*
DESCRIPTION
Initializes the END network connection for the WDB agent.

NOMANUAL
*/

/* includes */

#include "configNet.h"

/* defines */

#if	WDB_MTU > WDB_END_PKT_MTU
#undef	WDB_MTU
#define	WDB_MTU	WDB_END_PKT_MTU
#endif	/* WDB_MTU > WDB_END_PKT_MTU */

/* externals */

IMPORT END_TBL_ENTRY endDevTbl[];

/******************************************************************************
*
* wdbCommDevInit - initialize the END connection
*
* This routine initializes the END connection used by the WDB agent.
*
* RETURNS :
* OK or ERROR if the END connection can not be initialized.
*
* NOMANUAL
*/

STATUS wdbCommDevInit
    (
    WDB_COMM_IF *	pCommIf,
    char ** 		ppWdbInBuf,
    char **		ppWdbOutBuf
    )
    {
    static WDB_END_PKT_DEV	wdbEndPktDev;	/* END packet device */
    END_TBL_ENTRY *		pDevTbl;
    END_OBJ *			pEnd = NULL;
    char			devName[END_NAME_MAX];
    static uint_t		wdbInBuf [WDB_MTU/4];
    static uint_t		wdbOutBuf [WDB_MTU/4];

    /* update input & output buffer pointers */

    *ppWdbInBuf = (char *) wdbInBuf;
    *ppWdbOutBuf = (char *) wdbOutBuf;

    /* update communication interface mtu */

    wdbCommMtu = WDB_MTU;

    /* find the END Device Table entry corresponding to the boot device */
    
    for(pDevTbl = endDevTbl; pDevTbl->endLoadFunc != NULL; pDevTbl++)
    	{
	/* get the name of the device by passing argument devName = '\0' */

        bzero (devName, END_NAME_MAX);
        if (pDevTbl->endLoadFunc(devName, NULL) != 0)
            {
            if (_func_logMsg != NULL)
                _func_logMsg ("could not get device name!\n", 0, 0, 0,
                              0, 0, 0);
            return (ERROR);
            }

	/* compare the name of the device to the boot device selected */

	if (strncmp (sysBootParams.bootDev, (const char *) devName,
		     strlen((const char *) devName)) == 0)
	    {
	    /* Verify that the device unit number matches */

	    if (pDevTbl->unit == sysBootParams.unitNum)
		break;
	    }
	}

    /* if no END Device found, default to first valid table entry */

    if (pDevTbl->endLoadFunc == NULL)
	{
	if (endDevTbl->endLoadFunc == NULL)
	    {
            if (_func_logMsg != NULL)
                _func_logMsg ("no device in END device table!\n", 0, 0, 0,
                              0, 0, 0);
            return (ERROR);
	    }
	else
    	    pDevTbl = endDevTbl;
	}

    /* Check END device initialization by netLibInit */ 

    if (!pDevTbl->processed)
        {
	if (_func_logMsg != NULL)
	    _func_logMsg ("Network END device not initialized!\n",
	    		  0,0,0,0,0,0);
	return (ERROR);
	}

    /* get the END_OBJ */

    pEnd = endFindByName (devName, pDevTbl->unit);
        
    if (pEnd == NULL)
        {
        if (_func_logMsg != NULL)
            _func_logMsg ("Could not find device %s unit %d!\n",
                          sysBootParams.bootDev, sysBootParams.unitNum,
			  0,0,0,0);
        return (ERROR);
        }
    
    if (wdbEndPktDevInit(&wdbEndPktDev, udpRcv,
                     (char *)pEnd->devObject.name,
                     pEnd->devObject.unit) == ERROR)
	return (ERROR);

    if (udpCommIfInit(pCommIf, &wdbEndPktDev.wdbDrvIf) == ERROR)
	return (ERROR);

    return (OK);
    }
