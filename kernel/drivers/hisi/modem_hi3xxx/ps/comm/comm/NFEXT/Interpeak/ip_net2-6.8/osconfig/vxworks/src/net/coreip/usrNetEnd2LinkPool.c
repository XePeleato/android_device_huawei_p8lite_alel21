/* usrNetEnd2LinkPool.c - legacy netBufLib pool support for END2 drivers */

/*
 * Copyright (c) 2008 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River license agreement.
 */

/*
modification history
--------------------
01a,08may08,dlk  Written.
*/

NET_POOL_ID usrLinkPoolCreate
    (
    int nTuples,
    int clSize
    )
    {
    NETBUF_CFG netBufCfg;
    NETBUF_CL_DESC clDescTbl;
    NET_POOL_ID pPool;

    if (nTuples < 1)
        return NULL;

    bzero ((char *)&netBufCfg, sizeof(netBufCfg));
    bzero ((char *)&clDescTbl, sizeof(clDescTbl));

    netBufCfg.pName = NULL;
    netBufCfg.attributes = ATTR_AC_SH_ISR;
    netBufCfg.ctrlNumber = nTuples;
    netBufCfg.ctrlPartId = NULL;
    netBufCfg.bMemExtraSize = 0;
    netBufCfg.pClDescTbl = &clDescTbl;
    netBufCfg.pClDescTbl->clNum = nTuples;
    netBufCfg.pClDescTbl->clSize = clSize;
    netBufCfg.clDescTblNumEnt = 1;

    pPool = netPoolCreate (&netBufCfg, _pLinkPoolFuncTbl);

    return pPool;
    }
