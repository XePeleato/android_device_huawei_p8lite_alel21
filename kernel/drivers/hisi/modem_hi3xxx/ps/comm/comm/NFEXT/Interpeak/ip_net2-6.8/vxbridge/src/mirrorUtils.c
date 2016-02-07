/* mirrorUtils.c - utility routines for the mirror END driver */

/* Copyright 1984-2005 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01i,14nov06,tlu  Fixed coverity warning (defect 70020) 
01h,04oct06,kch  Fixed warning.
01g,15aug06,tlu  Ported to IPNET
01f,15apr05,svk  Update API descriptions
01e,17apr03,svk  Update copyright date
01d,15apr02,zhu  made changes for docs
01c,04apr02,svk  Replace hard-coded device name and unit number
01b,12mar01,ann  fixing some compilation warnings
01a,22feb01,ann  written
*/

/*
DESCRIPTION
This library contains the utility routines for implementing the
mirror END driver.

INCLUDE FILES: muxLib.h
*/

#include <muxLib.h>
#include <string.h>

/* Uncomment the line below to enable debug messages */
/* #define     B_DEBUG */

#ifdef B_DEBUG

int b_debug = 1;
#define DBG_PRINT(X) \
    do { \
    if (b_debug) \
        printf X; \
    }while(0)
#else

#define DBG_PRINT(X)

#endif

LOCAL char* baseDeviceName = "";
LOCAL int   baseUnitNum = 0;

/*******************************************************************************
* mirrorEnetAddrGet - get the MAC address for an interface
*
* This routine copies the MAC address from the END object of the 
* interface specified. The data is copied into the buffer provided
* to this routine. 
*
* RETURNS: n/a
* 
*/

void mirrorEnetAddrGet 
    (
    char *    pAddress        /* Buffer for the MAC address */ 
    )
    {
    END_OBJ *  pEndObj = NULL;

    DBG_PRINT(("Calling mirrorEnetAddrGet\n"));

    pEndObj = endFindByName (baseDeviceName, baseUnitNum);

    if (pEndObj != NULL)
        bcopy ((char *)pEndObj->mib2Tbl.ifPhysAddress.phyAddress, 
           pAddress, pEndObj->mib2Tbl.ifPhysAddress.addrLength);
    else
        DBG_PRINT(("ERROR: mirrorEnetAddrGet -- can't find pEndObj\n"));      
    }

/*******************************************************************************
* mirrorBaseParamsSet - store the base device name and unit number
*
* This routine stores the base device name and unit number. It is needed for 
* mirrorEnetAddrGet() to work.
*
* RETURNS: n/a
*/

void mirrorBaseParamsSet 
    (
    char * devName,
    int    unit
    )
    {
    DBG_PRINT(("Calling mirrorBaseParamsSet\n"));

    baseDeviceName = devName;
    baseUnitNum = unit;
    }

