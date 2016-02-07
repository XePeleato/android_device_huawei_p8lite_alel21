/* usrNetPool.c - the network buffer configuration */

/*
 * Copyright (c) 2001-2006 Wind River Systems, Inc.
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
01p,28dec06,kch  Removed references to the obsolete coreip virtual stack.
01o,08sep06,dlk  Remove INCLUDE_DS_ALLOC.
01n,25jun06,dlk  INCLUDE_DS_ALLOC separation.
01m,26jun05,dlk  Added nominal size 2048 row to system pool cluster desc
                 table.
01l,09feb05,wap  Allow sysctl init routines to be scaled out
01k,31aug04,vvv  updated sysctl component
01j,24aug04,vvv  merged from COMP_WN_IPV6_BASE6_ITER5_TO_UNIFIED_PRE_MERGE
01i,19jul04,niq  Move sysctl initialization to the component init routine.
01h,20feb04,vvv  added call to mbufSysctlInit()
01g,14aug03,niq  Merging from Accordion label ACCORDION_BASE6_MERGE_BASELINE
01f,13jan03,ann  merged Team-F1's ipsec changes
                 - added support for large clusters (4K to 64K).
01e,27jun02,ppp  merged the src from t3
01d,14jun02,pas  moved mbinit() from usrNetIpv4Init(), added system pool sizes
01c,06may02,ham  back to old netBufLib.
01b,15feb02,ham  consolidate pool config routines to usrNetPoolConfig().
01a,09dec01,ham  wrote.
*/

/*
 * mBlk, clBlk configuration table for network stack *SYSTEM* pool.
 * Used for network stack system structures such as routes, sockets,
 * protocol control blocks, interface addresses, mulitcast addresses,
 * and multicast routing entries.
 */
M_CL_CONFIG sysMblkClBlkDesc =
    {
    /*
      no. mBlks  no. clBlks  memArea  memSize
      -----------  ----------  -------  -------
    */
    NUM_SYS_MBLKS,  NUM_SYS_CLBLKS, PMA_SYSPOOL, PMS_SYSPOOL 
    };
   
/*
 * network stack *SYSTEM* cluster pool configuration table
 * Used for network stack system structures such as routes, sockets,
 * protocol control blocks, interface addresses, mulitcast addresses,
 * and multicast routing entries.
 */
CL_DESC sysClDescTbl [] =
    { 
    /*
      clusterSize  num           memArea       memSize
      -----------  ----          -------       -------
    */
    {SIZ_SYS_16,   NUM_SYS_16,   PMA_SYS_16,   PMS_SYS_16},
    {SIZ_SYS_32,   NUM_SYS_32,   PMA_SYS_32,   PMS_SYS_32},
    {SIZ_SYS_64,   NUM_SYS_64,   PMA_SYS_64,   PMS_SYS_64},
    {SIZ_SYS_128,  NUM_SYS_128,  PMA_SYS_128,  PMS_SYS_128},
    {SIZ_SYS_256,  NUM_SYS_256,  PMA_SYS_256,  PMS_SYS_256},
    {SIZ_SYS_512,  NUM_SYS_512,  PMA_SYS_512,  PMS_SYS_512},
    {SIZ_SYS_1024, NUM_SYS_1024, PMA_SYS_1024, PMS_SYS_1024},
    {SIZ_SYS_2048, NUM_SYS_2048, PMA_SYS_2048, PMS_SYS_2048}
    };

UINT sysClDescTblNumEnt = (NELEMENTS(sysClDescTbl));


/*
 * mBlk, clBlk configuration table for network stack *DATA* pool.
 * Only used for data transfer in the network stack.
 */
M_CL_CONFIG pktMblkClBlkDesc =
    {
    /*
      no. mBlks  no. clBlks memArea  memSize
      -----------  ---------- -------  -------
    */
    NUM_DAT_MBLKS,  NUM_DAT_CLBLKS, PMA_DATPOOL, PMS_DATPOOL
    };

/*
 * network stack *DATA* cluster pool configuration table
 * Only used for data transfer in the network stack.
 */
CL_DESC pktClDescTbl [] =
    {
    /*
      clusterSize  num  memArea  memSize
      -----------  ----  -------  -------
    */
    {  64,  NUM_DAT_64,    PMA_DAT_64,    PMS_DAT_64},
    { 128,  NUM_DAT_128,   PMA_DAT_128,   PMS_DAT_128},
    { 256,  NUM_DAT_256,   PMA_DAT_256,   PMS_DAT_256},
    { 512,  NUM_DAT_512,   PMA_DAT_512,   PMS_DAT_512},
    { 1024, NUM_DAT_1024,  PMA_DAT_1024,  PMS_DAT_1024},
    { 2048, NUM_DAT_2048,  PMA_DAT_2048,  PMS_DAT_2048},
    { 4096, NUM_DAT_4096,  PMA_DAT_4096,  PMS_DAT_4096},
    { 8192, NUM_DAT_8192,  PMA_DAT_8192,  PMS_DAT_8192},
    {16384, NUM_DAT_16384, PMA_DAT_16384, PMS_DAT_16384},
    {32768, NUM_DAT_32768, PMA_DAT_32768, PMS_DAT_32768},
    {65536, NUM_DAT_65536, PMA_DAT_65536, PMS_DAT_65536}
    };
   
UINT pktClDescTblNumEnt = (NELEMENTS(pktClDescTbl));

extern void dsMallocMapInit (void);

extern NET_POOL_CONFIG_PARAMS netPoolDefaultConfigParams;

void usrNetPoolConfig (void)
    {
    netPoolDefaultConfigParams.cfgh.len = sizeof (NET_POOL_CONFIG_PARAMS);

    netPoolDefaultConfigParams.sysMblkClBlkConf = &sysMblkClBlkDesc;
    netPoolDefaultConfigParams.sysClDesc = &sysClDescTbl[0];
    netPoolDefaultConfigParams.sysClDescNum = sysClDescTblNumEnt;

    netPoolDefaultConfigParams.pktMblkClBlkConf = &pktMblkClBlkDesc;
    netPoolDefaultConfigParams.pktClDesc = &pktClDescTbl[0];
    netPoolDefaultConfigParams.pktClDescNum = pktClDescTblNumEnt;

#ifdef INCLUDE_NET_SYSCTL
    /* Initialize the sysctl node */
    netPoolDefaultConfigParams.cfg_privInitSysctl   = mbufSysctlInit;
#else
    netPoolDefaultConfigParams.cfg_privInitSysctl   = NULL;
#endif


    /* initialize _pNetDPool, _pNetSysPool */
    mbinit (&netPoolDefaultConfigParams);
    }
