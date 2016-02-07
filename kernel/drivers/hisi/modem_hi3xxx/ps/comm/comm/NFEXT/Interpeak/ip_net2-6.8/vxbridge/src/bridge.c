/* bridge.c - simple learning bridge without STP */

/*
 * Copyright (c) 2000-2007 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River license agreement.
 */

/*
modification history
--------------------
01k,22sep09,tlu  Reset mirrorSendStatus to OK when continuing flooding 
		 a stack originated pkt while one port returns BLOCK err
01j,12feb09,ggz  Change paths to bridge.h, mirrorEnd.h header files 
01i,26sep08,tlu  Fix for defect 119553
01h,22nov07,tlu  Add call to update interface RUNNING/NOT RUNNING state
01g,23oct07,tlu  Fix the checking for tNetTask 
02f,12jul07,tlu  Fix for defect 97815
02e,05jan07,kch  Removed the obsolete coreip virtual stack references.
02d,25oct06,kch  Fixed warnings.
02c,19sep06,tlu  Un-comment ipDetach and ip6Detach functions
02b,05sep06,kch  Removed unnecessary coreip net/if_var.h include.
02a,15aug06,tlu  Ported the learning bridge to IPNET
01z,04feb06,kch  Fixed apigen doc errors and warnings.
01y,14jul05,myz  Fixed the broadcast failure when transmit buffers exhausted. 
01y,20jun05,svk  Detach from IPv6 stack in bridgeShutdown()
01x,15apr05,svk  Update API descriptions
01w,07feb05,svk  Make bridge version string const
01v,29oct04,svk  Add NOMANUAL to bridgeNextPhyDevEndGet()
01u,20oct04,myz  use _pNetDpool pool instead of driver's pool.
01t,19jul04,myz  improved END_ERR_BLOCK handling after field input.
01s,05may04,svk  If END_ERR_BLOCK happens more than once, prepend the packet
                 to driver sendq
01r,29apr04,myz  fixed spr92002 and spr96702
01q,20jun03,myz  removed the code for Patch muxTkBind override. This is not 
		 needed for protocol type MUX_PROTO_SNARF
01p,28may03,myz  added bridgeShutdown routine
01o,07may03,svk  Add support for static mac addresses in bridge station cache
01n,06may03,svk  Assume bridge port will not be removed when in use,
		 reformat for coding convention
01m,01may03,svk  Don't bridge EAPOL packet, handle tick wrap-around
01l,17apr03,svk  Implement version number, update copyright
01k,22nov02,zhu  added bridgeNextPhyDevEndGet function
01j,28oct02,zhu  reduce compiler warnings
01i,28oct02,zhu  fixed station cache aging timeout calculation
01h,18oct02,zhu  added protocol check
01g,19sep02,zhu  optimized bridge code
01f,16jul02,zhu  turned debug flag BRIDGE_DEBUG off
01e,06may02,zhu  fixing bugs in floodForwardPacket
01d,15apr02,zhu  made changes for docs
01c,04oct01,vks  MIPS alignment, station cache bugs 
01b,27sep01,zhu  TSR#257133 flow control
01a,28sep00,jhl  created
*/

/*
DESCRIPTION
This library implements a simple learning bridge without 
STP (Spanning Tree Protocol).

INCLUDE FILES: bridge.h mirrorEnd.h

SEE ALSO: muxLib
*/

#include <vxWorks.h>
#include <memLib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <taskLib.h>
#include <sysLib.h>
#include <tickLib.h>
#include <ipProto.h>
#include <muxLib.h>
#include <muxTkLib.h>
#include <end.h>
#include <private/muxLibP.h>
#include <netLib.h>

#ifdef INCLUDE_IPFIREWALL_SUPPORT
#define VXBRIDGE_INGRESS 1
#define VXBRIDGE_EGRESS  2
#define VXBRIDGE_INGRESS_EGRESS 3
#include <ipcom_clib.h>
#include <ipfirewall.h>
#include <ipfirewall_h.h>
#endif

#include "bridge.h"
#include "mirrorEnd.h"

#include "../../InterpeakMntn/include/InterpeakMntn.h"

/* Uncomment the line below to enable debug messages */
/* #define     BRIDGE_DEBUG */  

#ifdef BRIDGE_DEBUG
int bridgeDebug = 1;
#define DBG_PRINT(X) \
    do { \
    if (bridgeDebug) \
        printf X; \
    }while(0)
#else

#define DBG_PRINT(X)

#endif


/* constants */
#define     STATION_CACHE_SIZE         1024        /* must be a power of 2 */
#define     AGING_CHECK_DELAY_SECONDS  15
#define     AGING_TIMEOUT_SECONDS      (5 * 60)
#define     SNDERRQ_MAX_LEN            25


/* typedefs */


/*defs for BRIDGE_PORT_INFO::portType */
#define     BRIDGE_PORT_TYPE_END        0
#define     BRIDGE_PORT_TYPE_NPT        1


/* station cache entry */
typedef struct 
    {
    BOOL                inUse;          /* entry in use/available */
    BRIDGE_PORT_INFO*   pPortInfo;      /* port info for port this entry 
                                           is attached to */
    UINT32              tick;           /* sys tick this port was last heard
                                           from */
    UINT16              macAddr[3];     /* MAC address of this entry */
    UINT32              flags;          /* static or dynamic */
    } STATION_CACHE_ENTRY;


/* macros */

#define     STR_EQ(A,B)         ((strcmp((A), (B)) == 0) ? TRUE : FALSE)

/* both pMac1 and pMac2 must point to a two byte boundary */

#define     MAC_ADDR_EQ(pMac1,pMac2)    \
                ((pMac1[0] == pMac2[0]) && \
                 (pMac1[1] == pMac2[1]) && \
                 (pMac1[2] == pMac2[2])) 

/* pMacAddr must point to a two byte boundary */

#define     IS_BROADCAST(pMacAddr) \
                ((pMacAddr[0] == bcastMacAddr[0]) && \
                 (pMacAddr[1] == bcastMacAddr[1]) && \
                 (pMacAddr[2] == bcastMacAddr[2]))

#define IS_4BYTE_ALIGNED(pAddr) \
            (!((UINT32)(pAddr) & 0x3))
#define IS_2BYTE_ALIGNED(pAddr) \
            (!((UINT32)(pAddr) & 0x1))

#if (_BYTE_ORDER == _BIG_ENDIAN)
#define GETETHERTYPE(pMblk)  (*(UINT16*)(pMblk->mBlkHdr.mData + 12))
#else   /* (_BYTE_ORDER == _BIG_ENDIAN) */
#define GETETHERTYPE(pMblk)  (((*(UINT8*)(pMblk->mBlkHdr.mData + 12)) << 8) | \
                               (*(UINT8*)(pMblk->mBlkHdr.mData + 13)))
#endif  /* (_BYTE_ORDER == _BIG_ENDIAN) */


/* station cache manipulation macros */
#define     SC_ASSOCIATIVE_SET_SIZE     4


/* externals */

IMPORT NET_POOL_ID      _pNetDpool;
IMPORT int mirrorSendStatus;

/* Global data */
const char * bridgeVersion = BRIDGE_VERSION;

/* Local data */
LOCAL UINT16  bcastMacAddr[3] = { 0xFFFF, 0xFFFF, 0xFFFF };
LOCAL BOOL    bridgeInited = FALSE;
LIST    bridgePortList;

LOCAL STATION_CACHE_ENTRY*  pStationCache;

LOCAL SEM_ID    bridgePortListSemId;
LOCAL SEM_ID    stationCacheSemId;
LOCAL int       agingTaskId;
LOCAL END_OBJ*  pMirrorStackEnd = NULL;
LOCAL BRIDGE_PORT_INFO* pMirrorPortInfo = NULL;

#ifdef INCLUDE_IPFIREWALL_SUPPORT
LOCAL Ipcom_pkt *firewallPkt;
#endif

/* prototypes */
LOCAL BOOL    bridgeRcvRtn(void* pNetCallBackId, long type, M_BLK* pMblk, 
                           void* pSpareData);
LOCAL STATUS  bridgePortShutdownRtn(void* pNetCallBackId);
LOCAL STATUS  bridgeRestartRtn(void* pNetCallBackId);
LOCAL void    bridgeErrorRtn(void* pNetCallBackId, END_ERR* pError);
                          
LOCAL STATUS  packetForward(M_BLK* pMblk, BRIDGE_PORT_INFO* pDestPortInfo, 
                            BRIDGE_PORT_INFO* pSrcPortInfo);
LOCAL STATUS  packetFloodForward(M_BLK* pMblk, BRIDGE_PORT_INFO* pSrcPortInfo);

LOCAL void    bridgeAgingTask(void);

LOCAL void    stationCacheEntryAdd(UINT16* pMacAddr, 
				   BRIDGE_PORT_INFO* pPortInfo, UINT32 flags);
LOCAL void    stationCacheEntryDelete(STATION_CACHE_ENTRY* pSCEntry);
LOCAL STATION_CACHE_ENTRY* stationCacheEntryFind(UINT16* pMacAddr);

LOCAL BRIDGE_PORT_INFO* portListEntryFind(char* pDevName, int unitNum);
LOCAL STATUS bcastPktToStackSend (M_BLK_ID, BRIDGE_PORT_INFO*, 
                                  BRIDGE_PORT_INFO*);
LOCAL STATUS  packetToPortSend (M_BLK*, BRIDGE_PORT_INFO*,BRIDGE_PORT_INFO*);
#ifdef INCLUDE_IPFIREWALL_SUPPORT
LOCAL BOOL bridgePortFirewall(BRIDGE_PORT_INFO *pSrcPortInfo,
        BRIDGE_PORT_INFO *pDestPortInfo, UINT8 dir, M_BLK *pMblk);
#endif

void bridgePortListShow(void);
void bridgeStationCacheShow(void);

static void IPS_MNTN_BridgeTraceHook
(
    M_BLK*              pMblk,
    BRIDGE_PORT_INFO*   pDestPortInfo,
    BRIDGE_PORT_INFO*   pSrcPortInfo
);

/*******************************************************************************
* bridgeInit - initialize the bridge
*
* This routine initializes the bridge and station cache, and creates a bridge 
* port list and station cache access mutexes. This routine must be called 
* before you use any of the other bridge functionality.
*
* RETURNS: OK, or ERROR if bridge initialization fails
*
* ERRNO: N/A
*/

STATUS  bridgeInit(void)
    {
    int     index;
    
    if (bridgeInited == TRUE)
        return OK;
    
   /***************************************************************************
    * initialize the station cache
    ***************************************************************************/
    pStationCache = (STATION_CACHE_ENTRY*)calloc(STATION_CACHE_SIZE,
                                                 sizeof(STATION_CACHE_ENTRY));

    if (pStationCache == NULL)
        {
        DBG_PRINT(("bridgeInit: ERROR: Failure allocating station cache.\n"));
        return ERROR;
        }
    
    for (index = 0; index < STATION_CACHE_SIZE; ++index)
        pStationCache[index].inUse = FALSE;
    
   /***************************************************************************
    * initialize the bridge port information list
    ***************************************************************************/
    lstInit(&bridgePortList);

   /***************************************************************************
    * create bridge port list and station cache access mutexes
    ***************************************************************************/
    bridgePortListSemId = semMCreate(SEM_Q_PRIORITY | SEM_INVERSION_SAFE);
    
    if (bridgePortListSemId == NULL)
        {
        DBG_PRINT(("bridgeInit: ERROR: Failure creating bridge list mutex.\n"));
        free(pStationCache);
        return ERROR;
        }
    
    stationCacheSemId = semMCreate(SEM_Q_PRIORITY | SEM_INVERSION_SAFE);
    
    if (stationCacheSemId == NULL)
        {
        DBG_PRINT(("bridgeInit: ERROR: Failure creating bridge list mutex.\n"));
        free(pStationCache);
        semDelete(bridgePortListSemId);
        return ERROR;
        }
    
   /***************************************************************************
    * initialization complete 
    ***************************************************************************/
    agingTaskId = taskSpawn("tBridgeAger", 250, 0, 4096, 
                        (FUNCPTR)bridgeAgingTask, 
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    
    if (agingTaskId == ERROR)
        {
        DBG_PRINT(("bridgeInit: ERROR: Failure starting bridge aging task.\n"));
        free(pStationCache);
        semDelete(bridgePortListSemId);
        semDelete(stationCacheSemId);
        return ERROR;
        }

   pMirrorStackEnd = NULL;
   pMirrorPortInfo = NULL;

   /***************************************************************************
    * firewallPkt header 
    ***************************************************************************/
#ifdef INCLUDE_IPFIREWALL_SUPPORT 
	firewallPkt = ipcom_pkt_malloc(0, IP_FLAG_FC_ISRCONTEXT);
	if (IP_UNLIKELY(firewallPkt == IP_NULL))
	{
        DBG_PRINT(("bridgeInit: ERROR: Failure allocating firewallPkt.\n"));
        return ERROR;
	}
#endif

   /***************************************************************************
    * initialization complete 
    ***************************************************************************/
    bridgeInited = TRUE;
    
    return OK;
    }

/*******************************************************************************
* bridgeShutdown - shut down the bridge
*
* This routine performs the shutdown procedure of the bridge. It deletes the
* bridge aging task, removes the bridge protocol interface from the MUX layer,
* reverts the devices to non-promiscuous mode, unloads the mirror devices and
* frees all the allocated resources. Once this routine is called, the bridge
* can be restarted using its standard initialization routine.
*
* Do not call this routine concurrently with the bridgePortRemove() and
* bridgePortAdd() routines. No data packet should be sent to the bridge ports 
* by the local IP stack during the operation.
*
* RETURNS: OK, or ERROR if fail to shut down
*
* ERRNO: N/A
*/

STATUS bridgeShutdown(void)
    {
    BRIDGE_PORT_INFO* pPortInfo;
    BRIDGE_PORT_INFO* pNextPortInfo;
    END_OBJ *      pEnd;

    /* validate the operation */

    if (bridgeInited == FALSE)
        return ERROR;

    /* First set the device back to non promiscuous mode. This operation
     * should stop most of bridge traffic
     */

    pPortInfo = (BRIDGE_PORT_INFO*)lstFirst(&bridgePortList);

    while (pPortInfo != NULL)
        {
        muxIoctl(pPortInfo->pMuxBindCookie,EIOCSFLAGS,(caddr_t)(~IFF_PROMISC));

        pPortInfo = (BRIDGE_PORT_INFO *)lstNext((NODE*)pPortInfo);
        }

    /* delete tBridgeAger task */

    taskDelete(agingTaskId);

    /* Now remove the bridge ports  */

    pPortInfo = (BRIDGE_PORT_INFO*)lstFirst(&bridgePortList);

    while (pPortInfo != NULL)
        {
	pNextPortInfo = (BRIDGE_PORT_INFO *)lstNext((NODE*)pPortInfo);

	if (strcmp(MIRROR_DEV_NAME, pPortInfo->name) == 0)
	    {

	    /* must be mirror 1 bridge port */

	    pEnd = endFindByName (pPortInfo->name,pPortInfo->unitNum);

	    if (pEnd != NULL)
		{
		/* Need to set this field to some value different from pEnd
		 * to prevent the muxDevUnload from freeing the drvCtrl 
                 * descriptor which is statically allocated in our case
		 */

		pEnd->devObject.pDevice = NULL;

                muxDevUnload(pPortInfo->name,pPortInfo->unitNum);
		}
	    }
        else
	    {
            bridgePortShutdownRtn((void*)pPortInfo);
            }
        pPortInfo = pNextPortInfo;
        }

    /* detach from IP stack for MIRROR_STACK_UNIT */

    ipDetach(MIRROR_STACK_UNIT_NUM,MIRROR_DEV_NAME);
#ifdef INET6 
    ip6Detach(MIRROR_STACK_UNIT_NUM,MIRROR_DEV_NAME);
#endif  /* INET6 */ 


    /* unload the mirror end drivers */

    pEnd = endFindByName (MIRROR_DEV_NAME,MIRROR_STACK_UNIT_NUM);

    if (pEnd != NULL)
	{
	pEnd->devObject.pDevice = NULL;
        muxDevUnload(MIRROR_DEV_NAME,MIRROR_STACK_UNIT_NUM);
        } 
    /* free the station cache */

    free((void *)pStationCache);

    /* delete bridgePortListSemId and stationCacheSemId */

    semDelete(bridgePortListSemId);
    semDelete(stationCacheSemId);

    bridgeInited = FALSE;
    return OK;
    }
    
/*******************************************************************************
* bridgePortAdd -  add a device as a bridge port
*
* This routine adds a device as a bridge port to the bridge port list.
* bridgePortAdd() is called for the network interfaces that are attached to
* the bridge as ports, including 'mirror1'. This routine binds the port's 
* device driver through the MUX as a network service with type 
* 'MUX_PROTO_SNARF'.
*
* RETURNS: OK, or ERROR if port addition fails
*
* ERRNO: N/A
*/

STATUS  bridgePortAdd
    (
    char*   pDevName,           /* device name */
    int     unitNum             /* unit number */
    )
    {
    void*               pMuxBindCookie;
    BRIDGE_PORT_INFO*   pPortInfo;
    END_OBJ*            pEnd;
   
   /***************************************************************************
    * if bridge hasn't been init'ed, initialize it
    ***************************************************************************/
    if (bridgeInited == FALSE)
        bridgeInit();

   /***************************************************************************
    * verify that the port is already attached to the MUX
    ***************************************************************************/
    pEnd = endFindByName(pDevName, unitNum);
    
    if (pEnd == NULL)
        {
        DBG_PRINT(("bridgePortAdd: ERROR: \"%s%d\" is not attached to MUX.\n",
                   pDevName, unitNum));
        return ERROR;
        }
    
   /***************************************************************************
    * verify that this port is not already attached to the bridge
    ***************************************************************************/
    if (portListEntryFind(pDevName, unitNum) != NULL)
        {
        /* port is already in bridge port table */
        
        DBG_PRINT(("bridgePortAdd: ERROR: \"%s%d\" is already attached\n",
                   pDevName, unitNum));
        return ERROR;
        }
    
   /***************************************************************************
    * allocate space for bookkeeping info for the port
    ***************************************************************************/
    pPortInfo = (BRIDGE_PORT_INFO*)malloc(sizeof(BRIDGE_PORT_INFO));
    
    if (pPortInfo == NULL)
        {
        DBG_PRINT(("bridgePortAdd: ERROR: out of memory, malloc failure.\n"));
        return ERROR;
        }
    
   /***************************************************************************
    * bind bridge to the port as a SNARFer
    ***************************************************************************/
    pMuxBindCookie = muxTkBind(pDevName,
                               unitNum,
                               bridgeRcvRtn,
                               bridgePortShutdownRtn,
                               bridgeRestartRtn,
                               bridgeErrorRtn,
                               MUX_PROTO_SNARF,
                               "bridge",
                               pPortInfo,   /* pNetCallBackId */
                               NULL,        /* pNetSvcInfo */
                               NULL);       /* pNetDrvInfo */

    if (pMuxBindCookie == NULL)
        {
        DBG_PRINT(("bridgePortAdd: ERROR: muxTkBind() failed for \"%s%d\".\n",
               pDevName, unitNum));
        free(pPortInfo);
        return ERROR;
        }
    
   /***************************************************************************
    * save bookkeeping info for the port
    ***************************************************************************/
    bzero((char*)pPortInfo, sizeof(BRIDGE_PORT_INFO));
    
    bcopy(pDevName, pPortInfo->name, END_NAME_MAX - 1);
    pPortInfo->name[END_NAME_MAX - 1] = EOS;
    pPortInfo->unitNum = unitNum;
    pPortInfo->pMuxBindCookie = pMuxBindCookie;
    pPortInfo->errSndq.ifq_head    = 0;
    pPortInfo->errSndq.ifq_tail    = 0;
    pPortInfo->errSndq.ifq_len     = 0;
    pPortInfo->errSndq.ifq_maxlen  = SNDERRQ_MAX_LEN;
    pPortInfo->errSndq.ifq_drops   = 0;
 
    if (muxTkDrvCheck(pDevName) == TRUE)
        pPortInfo->portType = BRIDGE_PORT_TYPE_NPT;
    else 
        pPortInfo->portType = BRIDGE_PORT_TYPE_END;

#ifdef INCLUDE_IPFIREWALL_SUPPORT 
    /* Non-mirror interfaces */
    if(ipcom_strcmp(MIRROR_DEV_NAME,pDevName) != 0) {

        char ifName[END_NAME_MAX+5];

        if (ipAttach (unitNum,pDevName) != OK)
        {
            DBG_PRINT(("ipAttach failed for %s%d\n",pDevName,unitNum));
            return ERROR;
        }

        ipcom_snprintf(ifName,END_NAME_MAX+5,"%s%d",pDevName,unitNum);
        pPortInfo->netif =
            ipnet_if_nametonetif(ipnet_get_effective_vr(), ifName);

        if(pPortInfo->netif == IP_NULL) {
            DBG_PRINT(("INFO: netif for %s%d is NULL!!\n", pDevName,unitNum)); 
            return ERROR;
        }

        /* Hold a lock on the netif intstance until we're done with it */
        IPNET_IF_LOCK(pPortInfo->netif);

        DBG_PRINT(("INFO :  bridgePortAdd: attached \
                    %s, ifindex: %d\n", ifName,pPortInfo->netif->ipcom.ifindex));
    } else {
        pPortInfo->netif = IP_NULL;
    }
#endif

    
    semTake(bridgePortListSemId, WAIT_FOREVER);
    
    lstAdd(&bridgePortList, (NODE*)pPortInfo);
    
    semGive(bridgePortListSemId);
    
    return OK;
    }


/*******************************************************************************
* bridgePortRemove - remove a bridge port
*
* This routine removes an existing port from the bridge port list and unbinds
* the bridge service from the MUX layer for that port.
*
* RETURNS: OK, or ERROR if port removal fails
*
* ERRNO: N/A
*/

STATUS  bridgePortRemove
    (
    char*   pDevName,           /* device name */
    int     unitNum             /* unit number */
    )
    {
    BRIDGE_PORT_INFO*   pPortInfo;
    
   /***************************************************************************
    * if bridge hasn't been initialized, return ERROR
    ***************************************************************************/
    if (bridgeInited == FALSE)
        return ERROR;
    
   /***************************************************************************
    * find the port info and remove it from the bridge
    ***************************************************************************/
    pPortInfo = portListEntryFind(pDevName, unitNum);
    if (pPortInfo != NULL)
        return bridgePortShutdownRtn((void*)pPortInfo);

   /***************************************************************************
    * the port was not found in the list
    ***************************************************************************/
    return ERROR;
    }

/*******************************************************************************
* bridgeMacAdd - add a MAC address to the bridge station cache
*
* This routine provides the ability to add a MAC address to the bridge station
* cache directly instead of waiting for the bridge to automatically learn it.
* This routine is typically used to add a static entry to the station cache. 
*
* A static entry in the station cache represents a permanent mapping between
* a source MAC address and a bridge port. Static entries are useful if you
* already know the MAC address to bridge port mapping for your interfaces and
* want to set these at initialization time instead of waiting for the bridge
* to learn it. Static entries also prevent the MAC address of an interface
* from migrating from one port to another, as can happen if you have a
* network loop. If the migration happens, the bridge may stop receiving
* packets.
*
* RETURNS: OK, or ERROR if adding MAC address to station cache fails
*
* ERRNO: N/A
*/

STATUS bridgeMacAdd
    (
    char*    pDevName,    /* device name */
    int      unitNum,     /* unit number */
    char*    pMacAddr,    /* MAC address to add */ 
    UINT32   flags        /* static entry? */
    )
    {
    BRIDGE_PORT_INFO*       pPortInfo;  
    STATION_CACHE_ENTRY*    pSCEntry;   
    UINT16                  bridgeMacAddr[3];

    /* if bridge hasn't been initialized, return ERROR */
    
    if (bridgeInited == FALSE)
        return ERROR;
    
    /* Find the bridge port matching the device name and unit number */
    
    pPortInfo = portListEntryFind(pDevName, unitNum);        
    if (pPortInfo == NULL)
        {
        /* Only add MAC address for known bridge ports */
        
        return ERROR;
        }

    /* Copy in MAC address */
    
    bcopy(pMacAddr, (char*)bridgeMacAddr, 6);

    /* See if there is already an entry for this MAC address */
    
    pSCEntry = stationCacheEntryFind(bridgeMacAddr);    
    if (pSCEntry != NULL)
        {
        /* Found an entry - override it */
        
        pSCEntry->pPortInfo = pPortInfo;

        pSCEntry->tick = tickGet();

        pSCEntry->flags = flags;
        }
    else
        {
        /* Didn't find the entry so add it in */
        
        stationCacheEntryAdd(bridgeMacAddr, pPortInfo, flags);
        }

    return OK;
}

/*******************************************************************************
* bridgeMacRemove - remove a MAC address from the bridge station cache
*
* This routines provides the ability to remove a MAC address from the bridge
* station cache directly. This routine is typically used to remove a static
* entry from the station cache.
*
* RETURNS: OK, or ERROR if the MAC address is not found
*
* ERRNO: N/A
*/

STATUS  bridgeMacRemove
    (
    char*    pMacAddr
    )
    {
    UINT16                  bridgeMacAddr[3];
    STATION_CACHE_ENTRY*    pSCEntry;   

    /* Copy in MAC address */
    
    bcopy(pMacAddr, (char*)bridgeMacAddr, 6);
    
    /* Find the MAC address in the station cache */
    
    pSCEntry = stationCacheEntryFind(bridgeMacAddr);
    if (pSCEntry != NULL)
        {
        /* Found it - remove the entry */
        
        stationCacheEntryDelete(pSCEntry);
        return OK;
        }
        
    /* Didn't find the MAC address in the station cache */
    
    return ERROR;
    }
    

/*******************************************************************************
* bridgeRcvRtn - dispatch the received packet
*
* This routine performs the bridge's learning function as well as dispatching
* the incoming packet to the appropriate bridge ports. This routine is only
* invoked from the MUX layer function mux(Tk)Receive().
*
* RETURNS: TRUE, or FALSE if bridge receive routine fails
*
* ERRNO: N/A
*
* NOMANUAL
*/

LOCAL BOOL bridgeRcvRtn
    (
    void*       pNetCallBackId,  /* parameter assigned when muxTkBind called */ 
    long        type,            /* not used */
    M_BLK*      pMblk,           /* the received packet */
    void*       pSpareData       /* not used */
    )
    {
    UINT32               macAddr[4];    /* src and dst mac addresses holder */
    BRIDGE_PORT_INFO*    pSrcPortInfo;  /* incoming port info for the packet */
    BRIDGE_PORT_INFO*    pDestPortInfo; /* outgoing port info for the packet */
    STATION_CACHE_ENTRY* pSCEntry;      /* station cache entry */

    UINT16 * pTempAddr;
    UINT16 * pMacDstAddr;
    UINT16 * pMacSrcAddr;
    
    /* If it is a EAPOL packet, don't bridge it, send it on.... */

    if (GETETHERTYPE(pMblk) == 0x888E)
        return FALSE; 

    pSrcPortInfo = (BRIDGE_PORT_INFO*)pNetCallBackId;

    pTempAddr = (UINT16 *)(pMblk->mBlkHdr.mData);

    /* Extract the received frame's MAC addresses, which is possibely in the
     * non cached memory, to the at least two byte aligned local stack variable,
     * which most likely is in the cached memory, to acheive the optimal 
     * performace for this function.
     */

    if (IS_4BYTE_ALIGNED(pTempAddr))
        {
        /* the packet is at 4 byte boundary  */

        macAddr[0] = *(UINT32 *)pTempAddr;
        macAddr[1] = *((UINT32 *)pTempAddr + 1);
        macAddr[2] = *((UINT32 *)pTempAddr + 2);

        pMacDstAddr = (UINT16 *)macAddr;
        pMacSrcAddr = (UINT16 *)macAddr + 3;
        }
    else if (IS_2BYTE_ALIGNED(pTempAddr))
        {
        /* at two byte boundary */

        *((UINT16 *)macAddr + 1) = pTempAddr[0];
        macAddr[1] = *(UINT32 *)(pTempAddr + 1);
        macAddr[2] = *(UINT32 *)(pTempAddr + 3);
        *(UINT16 *)(macAddr + 3) = pTempAddr[5];

        pMacDstAddr = (UINT16 *)macAddr + 1;
        pMacSrcAddr = (UINT16 *)macAddr + 4;
        }
    else
        {
        /* at byte boundary, very unlikely */

        bcopy((char *)pTempAddr, (char *)macAddr,6);
        bcopy((char *)pTempAddr + 6, (char *)macAddr + 6, 6);
        pMacDstAddr = (UINT16 *)macAddr;
        pMacSrcAddr = (UINT16 *)macAddr + 3;
        }

    
   /***************************************************************************
    * ipfirewall mac level filtering 
    ***************************************************************************/
#ifdef INCLUDE_IPFIREWALL_SUPPORT_DISABLE
    {
        UINT8 *foo = (UINT8*)pMacDstAddr;
    
        DBG_PRINT(("INFO : netif lookup: bridgeRcvRtn got frame on "
          "%s%d, dest MAC addr: "
          "%02X:%02X:%02X:%02X:%02X:%02X\n",
          pSrcPortInfo->name,
          pSrcPortInfo->unitNum,
          foo[0],foo[1],foo[2],foo[3],foo[4],foo[5]));
    }
#endif

   /***************************************************************************
    * source address processing, perform bridge learning function
    ***************************************************************************/
    /* check if the source MAC address is in the station cache */
    
    pSCEntry = stationCacheEntryFind(pMacSrcAddr);

    if (pSCEntry != NULL)
        {
        /* source MAC address is in the station cache */

        if ((pSrcPortInfo != pSCEntry->pPortInfo) &&
            !(pSCEntry->flags & BRIDGE_STATIC_FLAG))
            {
	    /*
             * the source MAC address is now connected to a different port 
             * update the station cache 
             */

            pSCEntry->pPortInfo = pSrcPortInfo;
            }
        
        /* reset the station cache entry's tick count */

        pSCEntry->tick = tickGet();
        }
    else 
        {
	/*
         * source MAC address is not in the station cache;
         * create new entry in the station cache 
	 */

        stationCacheEntryAdd(pMacSrcAddr, pSrcPortInfo, 0);
        }

    
   /***************************************************************************
    * destination address processing
    ***************************************************************************/

    if (IS_BROADCAST(pMacDstAddr))
        {
        /* destination MAC address is broadcast, flood forward packet */

        if (packetFloodForward(pMblk, pSrcPortInfo) != OK) 
            return FALSE;
        }
    else 
        {
        /* destination MAC address is not broadcast */

        pSCEntry = stationCacheEntryFind(pMacDstAddr);
        
        if (pSCEntry != NULL)
            {
            pDestPortInfo = pSCEntry->pPortInfo;

            /* destination MAC address is in station cache */
            if (pSrcPortInfo == pDestPortInfo)
                {
                /* destination port is same as source port, discard packet */

                netMblkClChainFree(pMblk);
                }
            else 
                {
		/*
                 * destination port is not the same as source port, 
                 * forward packet 
                 */

                if (packetForward(pMblk, pDestPortInfo, pSrcPortInfo) != OK) 
                    return FALSE;
                }
            }
        else 
            {
	    /*
             * destination MAC address is not in station cache, 
             * flood forward packet 
             */

            if (packetFloodForward(pMblk, pSrcPortInfo) != OK) 
                return FALSE;
            }
        }
    
    return TRUE;
    }


/*******************************************************************************
* bridgePortShutdownRtn - shut down the bridge function on a particular port
*
* This routine removes the bridge service from the MUX interface for this
* port and frees the resources allocated. The bridge port must not be
* used after this routine executes successfully.
*
* RETURNS: OK always
*
* ERRNO: N/A
*
* NOMANUAL
*/

LOCAL STATUS  bridgePortShutdownRtn
    (
    void*   pNetCallBackId
    )
    {
    BRIDGE_PORT_INFO*   pPortInfo;
    int                 index;
    M_BLK_ID pMblk;
    
    pPortInfo = (BRIDGE_PORT_INFO*)pNetCallBackId;
    
   /***************************************************************************
    * unbind the bridge from this port
    ***************************************************************************/
    muxUnbind(pPortInfo->pMuxBindCookie,
              MUX_PROTO_SNARF,
              bridgeRcvRtn);

   /* give the muxUnbind some time to complete its job which is eventually done
    * in netTask context. The port should not be removed until it is unbinded
    */

   taskDelay(5); 
     
   /***************************************************************************
    * remove this port from the bridge port list
    ***************************************************************************/
    semTake(bridgePortListSemId, WAIT_FOREVER);
    
    lstDelete(&bridgePortList, (NODE*)pPortInfo);

   /***************************************************************************
    * remove all entries regarding this port from the station cache
    ***************************************************************************/
    for (index = 0; index < STATION_CACHE_SIZE; ++index)
        {
        if (pStationCache[index].inUse == TRUE)
            {
            if (pPortInfo == pStationCache[index].pPortInfo)
                stationCacheEntryDelete(&pStationCache[index]);
            }
        }

    /* remove all the queue packets if any */

    while(pPortInfo->errSndq.ifq_head)
        {
        /* Dequeue a packet. */

        IF_DEQUEUE (&pPortInfo->errSndq, pMblk);

        netMblkClChainFree(pMblk);
        }

#ifdef INCLUDE_IPFIREWALL_SUPPORT
    if(pPortInfo->netif != IP_NULL)
        IPNET_IF_UNLOCK(pPortInfo->netif);
    ipcom_pkt_free(firewallPkt);
#endif

    semGive(bridgePortListSemId);
    
    free(pPortInfo);
    
    return OK;
    }


/*******************************************************************************
* bridgeRestartRtn - bridge restart routine
*
* This is the entry point for the bridge restart routine.
*
* RETURNS: OK, or ERROR if bridge restart fails
*
* ERRNO: N/A
*
* NOMANUAL
*/

LOCAL STATUS  bridgeRestartRtn
    (
    void*   pNetCallBackId
    )
    {
    BRIDGE_PORT_INFO* pPortInfo;
    M_BLK_ID pMblk;
    int status = OK;

    pPortInfo = (BRIDGE_PORT_INFO *)pNetCallBackId;

    /* check if there is packets in the ERR_BLOCK send queue */

    while(pPortInfo->errSndq.ifq_head)
        {
        /* Dequeue a packet. */

        IF_DEQUEUE (&pPortInfo->errSndq, pMblk);

        status = packetToPortSend (pMblk, pPortInfo,NULL);

        if (status == END_ERR_BLOCK)
            break;
        }

    if (status == OK)
	{
        /* only after the queue is emptied, enable mirror0 restart */

        if (pMirrorStackEnd == NULL)
            {
            if ((pMirrorStackEnd = endFindByName(MIRROR_DEV_NAME,
                                   MIRROR_STACK_UNIT_NUM)) == NULL)
                {
                printf("ERROR: restart fails, can't find mirror0 device\n");
                return ERROR;
                }
            }

        /* already in tNetTask context */

        muxTxRestart(pMirrorStackEnd);
        }

    return OK;
    }


/*******************************************************************************
* bridgeErrorRtn - bridge Error routine 
*
* This is the entry point for the bridge error routine.
*
* RETURNS: N/A
*
* ERRNO: N/A
*
* NOMANUAL
*/

LOCAL void    bridgeErrorRtn
    (
    void*       pNetCallBackId, 
    END_ERR*    pError
    )
    {
    return;
    }

/*******************************************************************************
*
* pktToQueuePut - put the unsent packets to the queue
*
* Only the packets originated from another physical bridge port will be put
* into this queue. The packet originated from local stack will be returned
* back to the IP layer. Therefore this queue is only accessed by the restart
* and driver's receive routines. Both are assumed to be invoked from the
* 'tNetTask' context through netJobAdd() function.
*
* RETURNS: N/A
*
* ERRNO: N/A
*
* NOMANUAL
*/

LOCAL void pktToQueuePut
    (
    BRIDGE_PORT_INFO*   pPortInfo,
    M_BLK_ID            pMblk,
    BOOL                atHead
    )
    {
    /* packets not originated from the local stack */
    /* put it in the queue, and let restart routine send them out */

    if (IF_QFULL(&pPortInfo->errSndq))
        {
        IF_DROP(&pPortInfo->errSndq);
        netMblkClChainFree(pMblk);
        }
    else if (atHead == TRUE)
        {
        IF_PREPEND(&pPortInfo->errSndq,pMblk);
        }
    else
        {
        IF_ENQUEUE(&pPortInfo->errSndq,pMblk);
        }
    }

/*******************************************************************************
*
* packetToPortSend - send a packet to the given port
*
* This routine sends a packet to the given port
*
* RETURNS: OK or ERROR
*
* ERRNO: N/A
*
* NOMANUAL
*/

LOCAL int  packetToPortSend
    (
    M_BLK*              pMblk,
    BRIDGE_PORT_INFO*   pDestPortInfo,
    BRIDGE_PORT_INFO*   pSrcPortInfo
    )
    {
    int status = OK;
  
#ifdef INCLUDE_IPFIREWALL_SUPPORT
    if(bridgePortFirewall(pSrcPortInfo,pDestPortInfo,
                VXBRIDGE_INGRESS_EGRESS,pMblk) == FALSE) {
        return status;
    }
#endif

    /* Attempt to send the packet */

    switch (pDestPortInfo->portType)
        {
        case BRIDGE_PORT_TYPE_END:
            status = muxSend(pDestPortInfo->pMuxBindCookie, pMblk);
            break;

        case BRIDGE_PORT_TYPE_NPT:
            /* pMblk->mBlkHdr.mFlags |= M_L2HDR; */
            status = muxTkSend(pDestPortInfo->pMuxBindCookie, pMblk,
                               NULL, 0, NULL);
            break;

        default:
            netMblkClChainFree(pMblk);
            break;
        }

    if (status == END_ERR_BLOCK)
        {
        /*
         * The device is currently unable to transmit because of
         * insufficient resources. Put the packet in the error send 
         * queue for that device. When the device has enough resources 
         * to transmit, MUX will call the Bridge restart routine.
         * The restart routine will try to re-send the packet.
         */

        /*DBG_PRINT(("ERROR: packetForward: packet sent to %s%d returned \
                  END_ERR_BLOCK\n",
                  pDestPortInfo->name, pDestPortInfo->unitNum));*/

	/* Did this error originate from mirror1? */

        if (pMirrorPortInfo == NULL)
            {
            pMirrorPortInfo = portListEntryFind(MIRROR_DEV_NAME,
                                            MIRROR_BRIDGE_UNIT_NUM);

            if (pMirrorPortInfo == NULL)
                {
                printf("ERROR: packetToPortSend, can't find mirror port\n");
                return ERROR;
                }
            }

        if (pSrcPortInfo == pMirrorPortInfo)
            {
            /* packet originated from the local stack */

            DBG_PRINT(("END_ERR_BLOCK packet sent from mirror 1\n"));

            /* notify the mirrorSend routine */

            mirrorSendStatus = END_ERR_BLOCK;
            }
        else
            {

            /*
             * If the source port is NULL, it means the Bridge restart
             * routine has unsuccessfully tried to re-send the packet.
             * To preserve the transmit order, put the packet at the head
             * of the device's error send queue.
             *
             * Otherwise, this the first attempt to send the packet. Put
             * the packet at the tail of the device's error send queue.
             */

     	    if ( strncmp(taskName(taskIdSelf()), "tNet", 4) == 0)
                pktToQueuePut(pDestPortInfo, pMblk, (pSrcPortInfo == NULL));
	    else
                netJobAdd( (FUNCPTR)packetToPortSend, (int)pMblk, (int)pDestPortInfo, (int)pSrcPortInfo, 0, 0 );
            }
        }
    return status;
    }


/*******************************************************************************
* packetForward - forward the packet to the destination port
*
* This routine forwards the provided packet out the indicated destination
* port.  
*
* If the destination port is driven by a NPT device driver, the
* the transmission is accomplished by simply calling muxTkSend() and setting
* the M_L2HDR flag in the mBlk header.  The presence of this flag indicates
* to the NPT device driver that a fully formed physical layer header is
* already present in the packet.  Although this flag is mentioned in the NPT
* docs, it is not defined in any header files.
*
* If the destination port is driven by an END device driver, transmission
* cannot be accomplished by calling muxTkSend() since muxTkSend() will
* prepend a physical layer header with a source MAC address of the system
* itself.  However, muxSend() can be called to accomplish the transmission
* to an END driver.
*
* RETURNS: OK, or ERROR if bridge forward packets fails
*
* ERRNO: N/A
*
* NOMANUAL
*/

LOCAL STATUS  packetForward
    (
    M_BLK*              pMblk, 
    BRIDGE_PORT_INFO*   pDestPortInfo,
    BRIDGE_PORT_INFO*   pSrcPortInfo
    )
    {

    if (pDestPortInfo->errSndq.ifq_head)
        {
        /*
         * If the error send queue is not empty, there must be one restart
         * called later on. If the current function is called from a task
         * other than tNetTask(packet from local stack), it may get switched out
         * here to execute the restart routine in tNetTask context. But the
         * muxTxRestart on mirror0 device will get blocked by splnet semaphore
         * in ipTxStartup from ipTxRestart until this function completed. Then
         * it will find this packet in the IP ifqueue. So we're OK. 
         * If the current function is called from tNetTask context(packet from
         * another physical bridge port), the restart(always from tNetTask 
         * context) can't start until we're finished here.
         */

        if (pMirrorPortInfo == NULL)
            {
            pMirrorPortInfo = portListEntryFind(MIRROR_DEV_NAME,
                                            MIRROR_BRIDGE_UNIT_NUM);

            if (pMirrorPortInfo == NULL)
                {
                printf("ERROR: packetForward, can't find bridge mirror port\n");
                return ERROR;
                }
            }

        if (pSrcPortInfo == pMirrorPortInfo)
            {
            /* notify the mirrorSend routine */

            mirrorSendStatus = END_ERR_BLOCK;
            }
        else if ( strncmp(taskName(taskIdSelf()), "tNet", 4) == 0)
            pktToQueuePut(pDestPortInfo,pMblk,FALSE);
	else
            netJobAdd( (FUNCPTR)packetForward, (int)pMblk, (int)pDestPortInfo, (int)pSrcPortInfo, 0, 0 );
        }
    else
        packetToPortSend(pMblk, pDestPortInfo, pSrcPortInfo);

    return OK;
    }

/*******************************************************************************
* packetFloodForward - send packet to all the bridge ports except source port
*
* This routine sends packet to all the bridge ports except source port
*
* RETURNS: OK, or ERROR if bridge flood forward packets fails
*
* ERRNO: N/A
*
* NOMANUAL
*/

LOCAL STATUS  packetFloodForward
    (
    M_BLK*              pMblk, 
    BRIDGE_PORT_INFO*   pSrcPortInfo
    )
    {
    BRIDGE_PORT_INFO*   pDestPortInfo;
    M_BLK*              pMblkDup;
   
   /***************************************************************************
    * forward the packet to all ports except for the source port
    ***************************************************************************/
    for (pDestPortInfo = (BRIDGE_PORT_INFO*)lstFirst(&bridgePortList);
         pDestPortInfo != NULL;
         pDestPortInfo = (BRIDGE_PORT_INFO*)lstNext((NODE*)pDestPortInfo))
        {
        if (pDestPortInfo != pSrcPortInfo)
            {

            if (strcmp(MIRROR_DEV_NAME,pDestPortInfo->name) == 0)
                {
                /* must be mirror 1 bridge port.  */

                if (bcastPktToStackSend (pMblk,  pDestPortInfo, pSrcPortInfo)
                    != OK) 
                    /* don't attempt more forwarding if error is encountered */

                    break;
                }
            else
                {
                                
                /*duplicate the packet (FYI - this does not copy the payload) */

                pMblkDup = netMblkChainDup(_pNetDpool,
                                       pMblk,
                                       0,
                                       M_COPYALL,
                                       M_DONTWAIT);
                
                if (pMblkDup == NULL) 
                    /* don't attempt more forwarding if error is encountered */

                    break;
            
                if (packetForward(pMblkDup, pDestPortInfo, pSrcPortInfo) != OK)
                    {
                    /* don't attempt more forwarding if error is encountered */

                    netMblkClChainFree(pMblkDup);

                    break;
                    }
                if (mirrorSendStatus == END_ERR_BLOCK) 
                    {
                    if (pSrcPortInfo == pMirrorPortInfo)
                        {
                        /* packet originated from local stack and END_ERR_BLOCK
                         * code is return from the packetForward(). Then stop
                         * all forwardings. don't free the original pMblk which
                         * will be put in the ifq which is emptied by the 
                         * stack's restart routine
 			 * (fix for WIND0019553: continue flooding to other
			 *  ports, removed "return OK" and reset 
			 *  mirrorSendStatus to OK)
                         */
                        netMblkClChainFree(pMblkDup);
			mirrorSendStatus = OK;
                        }
                    }
                }
            }
        }  /* end for loop */
    
    /* forwarding has been with duplicates, free the original */

    netMblkClChainFree(pMblk);
    
    return OK;
    }

/*******************************************************************************
*
* bcastPktToStackSend - send the broadcast packet to the local stack
*
* This routine sends the broadcast packet to the local stack
*
* RETURNS: OK or ERROR
*
* ERRNO: N/A
*
* NOMANUAL
*/

LOCAL STATUS bcastPktToStackSend 
    (
    M_BLK_ID pMblk,
    BRIDGE_PORT_INFO*   pDestPortInfo,
    BRIDGE_PORT_INFO*   pSrcPortInfo
    )
    {
    M_BLK_ID pOrgMblk;
    M_BLK_ID p1stMblk;
    M_BLK_ID pNewMblk;
    M_BLK_ID pPreMblk;
   
    /* 
     * Need to make a real copy of the packet when it is delivered
     * to the IP stack because the IP stack may modify the IP
     * header before the packet is sent out to the other bridge
     * ports.
     */

    pOrgMblk = pMblk;
    p1stMblk = NULL;
    pPreMblk = NULL;

    while (pOrgMblk)
        {
        pNewMblk = netTupleGet(_pNetDpool,
                             pOrgMblk->pClBlk->clSize, M_DONTWAIT,
                             MT_DATA,0);
        if (pNewMblk == NULL ||
            (pNewMblk->pClBlk->clSize < pOrgMblk->mBlkHdr.mLen + 2) )
            {
            if (p1stMblk)
                netMblkClChainFree(p1stMblk);
            if (pNewMblk)
                netMblkClChainFree(pNewMblk);

            DBG_PRINT(("ERROR: can't get a buf\n"));
            return ERROR;
            }

        /* assign the packet header info for first mblk */

        if (p1stMblk == NULL)
            {
            p1stMblk = pNewMblk;
            pNewMblk->mBlkHdr.mFlags = pMblk->mBlkHdr.mFlags;
            pNewMblk->mBlkPktHdr     = pMblk->mBlkPktHdr;

            /* align the IP header at 4 byte boundary */

            pNewMblk->mBlkHdr.mData += 2;
            }

        /* copy the data */

        bcopy(pOrgMblk->mBlkHdr.mData, pNewMblk->mBlkHdr.mData,
                          pOrgMblk->mBlkHdr.mLen);
        pNewMblk->mBlkHdr.mLen = pOrgMblk->mBlkHdr.mLen;

        if (pPreMblk)
            pPreMblk->mBlkHdr.mNext = pNewMblk;
        pPreMblk = pNewMblk;
        pOrgMblk = pOrgMblk->mBlkHdr.mNext;
        }

    if (packetForward(p1stMblk, pDestPortInfo, pSrcPortInfo) != OK)
        {
        /* free the original */

        netMblkClChainFree(p1stMblk);

        return ERROR;
        }
    return OK;
    }

/*******************************************************************************
* bridgeAgingTask - perform the deletion of the obsolete station cache entry 
*
* This routine performs the deletion of the obsolete station cache entry
*
* RETURNS: N/A
*
* ERRNO: N/A
*
* NOMANUAL
*/

LOCAL void  bridgeAgingTask
    (
    void
    )
    {
    int     index;
    UINT32  timeoutTick;
    UINT32  currentTick;
    UINT32  deltaTick;

    /* convert timeout from seconds to vxWorks ticks */

    timeoutTick = sysClkRateGet() * AGING_TIMEOUT_SECONDS;
    
    while (TRUE)
        {
        taskDelay(sysClkRateGet() * AGING_CHECK_DELAY_SECONDS);
        currentTick = tickGet();
        
        for (index = 0; index < STATION_CACHE_SIZE; ++index)
            {
            if ((pStationCache[index].inUse == TRUE) &&
                !(pStationCache[index].flags & BRIDGE_STATIC_FLAG))
                {
                deltaTick = currentTick >= pStationCache[index].tick?  /* tick wrap around? */
                currentTick - pStationCache[index].tick :  /* no */
                (UINT32)0xffffffff - pStationCache[index].tick + currentTick;  /* yes */
                 
                if (deltaTick >= timeoutTick)
                    {
                    /****************************************
                     * station cache entry has timed out
                     ****************************************/

                    stationCacheEntryDelete(&pStationCache[index]);
                    }
                }
            }

	/* Update interface RUNNING state */
	mirrorEndRunningState();
        }

   /****************************************
    * function never returns
    ****************************************/
   }


/*******************************************************************************
* stationCacheEntryAdd - add one entry to station cache
*
* This routine adds one entry to the station cache.
*
* RETURNS: N/A
*
* ERRNO: N/A
*
* NOMANUAL
*/

LOCAL void  stationCacheEntryAdd
    (
    UINT16*             pMacAddr, 
    BRIDGE_PORT_INFO*   pPortInfo,
    UINT32              flags
    )
    {
    BOOL    found = FALSE;
    UINT32  baseIndex;
    UINT32  count;
    UINT32  index = 0;
    UINT32  oldestIndex;
    UINT32  oldestAge;
    UINT32  currentTick;
    UINT32  age;
    
    semTake(stationCacheSemId, WAIT_FOREVER);
    
   /***************************************************************************
    * if there's a free one in the associative set, use it
    ***************************************************************************/
    baseIndex = (UINT32)(pMacAddr[2]);
    baseIndex &= (STATION_CACHE_SIZE - 1);
    
    for (count = 0; count < SC_ASSOCIATIVE_SET_SIZE; ++count)
        {
        index = (baseIndex + count) & (STATION_CACHE_SIZE - 1);
        
        if (pStationCache[index].inUse == FALSE)
            {
            found = TRUE;
            break;
            }
        }
    
   /***************************************************************************
    * There isn't a free one in the associative set.  So, free the oldest one
    * and reuse it
    ***************************************************************************/
    if (found == FALSE)
        {
        oldestIndex = baseIndex;        /* fail safe */
        oldestAge  =  0;
        currentTick = tickGet();
        
        for (count = 0; count < SC_ASSOCIATIVE_SET_SIZE; ++count)
            {
            index = (baseIndex + count) & (STATION_CACHE_SIZE - 1);
            
            age = currentTick >= pStationCache[index].tick?  /* tick wrap around? */
                  currentTick - pStationCache[index].tick :  /* no */
                  (UINT32)0xffffffff - pStationCache[index].tick + currentTick;  /* yes */
                  
            if (age > oldestAge)
                {
                oldestAge = age;
                oldestIndex = index;
                }
            }
        
        index = oldestIndex;
        }

   /***************************************************************************
    * initialize the entry
    ***************************************************************************/
    pStationCache[index].inUse = TRUE;
    pStationCache[index].pPortInfo = pPortInfo;
    pStationCache[index].tick = tickGet();
    pStationCache[index].macAddr[0] = pMacAddr[0];
    pStationCache[index].macAddr[1] = pMacAddr[1];
    pStationCache[index].macAddr[2] = pMacAddr[2];
    pStationCache[index].flags = flags;

    semGive(stationCacheSemId);
    }


/*******************************************************************************
* stationCacheEntryDelete - delete one entry from station cache 
*
* This routine deletes one entry from the station cache.
*
* RETURNS: N/A
*
* ERRNO: N/A
*
* NOMANUAL
*/

LOCAL void  stationCacheEntryDelete
    (
    STATION_CACHE_ENTRY* pSCEntry
    )
    {
    semTake(stationCacheSemId, WAIT_FOREVER);

    pSCEntry->inUse = FALSE;
    pSCEntry->flags = 0;
    
    semGive(stationCacheSemId);
    }


/*******************************************************************************
* stationCacheEntryFind - find an entry matching the MAC address
*
* This routine finds an entry matching the MAC address from the station cache.
*
* RETURNS: station cache entry found
*
* ERRNO: N/A
*
* NOMANUAL
*/

LOCAL STATION_CACHE_ENTRY*  stationCacheEntryFind
    (
    UINT16*   pMacAddr
    )
    {
    UINT32  count;
    UINT32  index;
    UINT16 * pTemp;
    UINT32   baseIndex = 0;

    baseIndex = (UINT32)(pMacAddr[2]);
    baseIndex &= (STATION_CACHE_SIZE - 1);
    
    for (count = 0; count < SC_ASSOCIATIVE_SET_SIZE; ++count)
        {
        index = (baseIndex + count) & (STATION_CACHE_SIZE - 1);
        pTemp = (UINT16 *)(pStationCache[index].macAddr);

        if ((pStationCache[index].inUse == TRUE) &&
            MAC_ADDR_EQ(pMacAddr,pTemp))
	    {
            return &pStationCache[index];
	    }
        }

    return NULL;
    }


/*******************************************************************************
* bridgePortListShow - display a list of all bridge ports
* 
* This routine displays the contents of the bridge port information list.
*
* RETURNS: N/A
*
* ERRNO: N/A
*/

void    bridgePortListShow
    (
    void
    )
    {
    BRIDGE_PORT_INFO*   pPortInfo;
    
   /***************************************************************************
    * if bridge hasn't been initialized, return
    ***************************************************************************/
    if (bridgeInited == FALSE)
        return;
    
   /***************************************************************************
    * print out banner
    ***************************************************************************/
    printf("Bridge Port Info List:\n");
    printf("EntryAddr   DevName   Num  MuxCookie   Type\n");
    printf("-------------------------------------------\n");
    
   /***************************************************************************
    * print out the contents of each entry in the bridge port info list
    ***************************************************************************/
    for (pPortInfo = (BRIDGE_PORT_INFO*)lstFirst(&bridgePortList);
         pPortInfo != NULL;
         pPortInfo = (BRIDGE_PORT_INFO*)lstNext((NODE*)pPortInfo))
        {
        printf("0x%08X  %-8s   %1d   0x%08X  ",
               (UINT32)pPortInfo,
               pPortInfo->name,
               pPortInfo->unitNum,
               (UINT32)pPortInfo->pMuxBindCookie);
        
        switch (pPortInfo->portType)
            {
            case BRIDGE_PORT_TYPE_END:  printf("END");      break;
            case BRIDGE_PORT_TYPE_NPT:  printf("NPT");      break;
            default:                    printf("UNKNOWN");  break;
            }
        
        printf("\n");
        }
    }

/*****************************************************************************
*
* bridgeNextPhyDevEndGet - return the next END descriptor of physical device
*
* This routine returns the next END descriptor of physical device.
*
* RETURNS: OK or ERROR
*
* ERRNO: N/A
*
* NOMANUAL
*/

STATUS bridgeNextPhyDevEndGet 
    (
    char * pNameBuf,
    int  * pUnit
    )
    {
    BRIDGE_PORT_INFO * pPortInfo;

    if (pNameBuf[0] == 0)
        {
        /* get the first one */

        pPortInfo = (BRIDGE_PORT_INFO*)lstFirst(&bridgePortList);
        if (pPortInfo != NULL)
            {
            strcpy(pNameBuf,pPortInfo->name);
            *pUnit = pPortInfo->unitNum;
            return OK;
            }
        }
    else
        {
        /* get the next after the one specified by caller */

        for (pPortInfo = (BRIDGE_PORT_INFO*)lstFirst(&bridgePortList);
             pPortInfo != NULL;
             pPortInfo = (BRIDGE_PORT_INFO*)lstNext((NODE*)pPortInfo))
            {
            if ((strcmp(pPortInfo->name,pNameBuf) == 0) &&
                (*pUnit == pPortInfo->unitNum))
                {
                pPortInfo = (BRIDGE_PORT_INFO*)lstNext((NODE*)pPortInfo);
                if (pPortInfo == NULL)
                    return ERROR;
                else
                    {
                    strcpy(pNameBuf,pPortInfo->name);
                    *pUnit = pPortInfo->unitNum;
                    return OK;
                    }
                }
            }  /* end for loop */
        }

    return ERROR;
    }
    
/*******************************************************************************
* bridgeStationCacheShow - display all entries in bridge station cache
*
* This routine displays the contents of the bridge station cache. The cache
* contains what the bridge has learned and remembered about the mapping of
* MAC addresses and bridge ports.
*
* RETURNS: N/A
*
* ERRNO: N/A
*/

void    bridgeStationCacheShow
    (
    void
    )
    {
    int     index;
    unsigned char * pTemp; 
    
   /***************************************************************************
    * if bridge hasn't been initialized, return
    ***************************************************************************/
    if (bridgeInited == FALSE)
        return;
    
   /***************************************************************************
    * print out banner
    ***************************************************************************/
    printf("Bridge Station Cache Contents:\n");
    printf("MAC Address           Tick     pPortInfo       \n");
    printf("-----------------------------------------------\n");
    
   /***************************************************************************
    * print out contents of each allocated station cache entry
    ***************************************************************************/
    for (index = 0; index < STATION_CACHE_SIZE; ++index)
        {
        if (pStationCache[index].inUse == TRUE)
            {
            pTemp = (unsigned char *)(pStationCache[index].macAddr);
            printf("%02X:%02X:%02X:%02X:%02X:%02X  ", pTemp[0], pTemp[1],
                   pTemp[2],pTemp[3],pTemp[4],pTemp[5]);
            printf("0x%08X  ", pStationCache[index].tick);
            printf("%s ", (pStationCache[index].flags & BRIDGE_STATIC_FLAG)?
                   "STATIC ":"DYNAMIC");            
            if(pStationCache[index].pPortInfo)
                printf("%s%d\n", pStationCache[index].pPortInfo->name,pStationCache[index].pPortInfo->unitNum);
            else
                printf("0x%08X\n", (UINT32)pStationCache[index].pPortInfo);            }
        }
    
    }

 
/*******************************************************************************
* portListEntryFind - find the bridge port matching device name and unit number
*
* This routine finds  the bridge port matching device name and unit number
*
* RETURNS: bridge port entry if found, NULL otherwise
*
* ERRNO: N/A
*
* NOMANUAL
*/

LOCAL BRIDGE_PORT_INFO* portListEntryFind
    (
    char* pDevName,    /* device name */
    int   unitNum      /* unit number */
    )
    {
    BRIDGE_PORT_INFO* pPortInfo;
    
    for (pPortInfo = (BRIDGE_PORT_INFO*)lstFirst(&bridgePortList);
         pPortInfo != NULL;
         pPortInfo = (BRIDGE_PORT_INFO*)lstNext((NODE*)pPortInfo))
        {
        if ((STR_EQ(pDevName, pPortInfo->name)) && 
            (unitNum == pPortInfo->unitNum))
            {
            break;
            }
        }
        
    return pPortInfo;    
    }
    
/*******************************************************************************
* bridgePortFirewall
*
* This routine filters the ingress packet through ipfirewall mac rules 
*
* RETURNS: 1 = Block packet.
*          0 = Pass packet.
*
* ERRNO: N/A
*
* NOMANUAL
*/

#ifdef INCLUDE_IPFIREWALL_SUPPORT
LOCAL BOOL bridgePortFirewall
    (
        BRIDGE_PORT_INFO* pSrcPortInfo,
        BRIDGE_PORT_INFO* pDestPortInfo,
        UINT8 dir,
        M_BLK *pMblk
    )
    {

    int start;

    /* Make sure we have a valid END object for the mirror */
    if(pMirrorStackEnd == NULL) {
        return TRUE;
    }

    if (pMirrorPortInfo == NULL)
    {
        pMirrorPortInfo = portListEntryFind(MIRROR_DEV_NAME,
                MIRROR_BRIDGE_UNIT_NUM);

        if (pMirrorPortInfo == NULL)
        {
            printf("ERROR: pMirrorPortInfo, can't find mirror port\n");
            goto drop;
        }
    }
    

    if(pSrcPortInfo == pMirrorPortInfo) {
        /*DBG_PRINT(("Received packet from local stack via mirror1 ... by-passing filter\n"));*/
        return TRUE;
    }

    if(pDestPortInfo == pMirrorPortInfo) {
        /*DBG_PRINT(("Sending packet to local stack via mirror1 ... by-passing filter\n"));*/
        if(pSrcPortInfo != NULL) {
            if(pSrcPortInfo->netif != IP_NULL)
                pMblk->mBlkPktHdr.rcvif = (void *)&pSrcPortInfo->netif->ipcom;
            else
                pMblk->mBlkPktHdr.rcvif = IP_NULL;
        }
        return TRUE;
    }

    /*DBG_PRINT(("src: 0x%08X, dst: 0x%08X, mirror: 0x%08X\n",
                pSrcPortInfo,pDestPortInfo,pMirrorPortInfo));*/
#ifdef IPCOM_USE_HW_VLAN_TAGGING
     /*
      * If driver supports stripping vlan tag, ethernet header from the received
      * frame will be untagged and the vlan tag information will be stored in
      * mBlkPktHdr.vlan. Copy the vlan information to pkt->link_cookie.
      * For better performance, don't check the netif->drv_capabilities for
      * vlan tagging support. The driver shouldn't be setting the CSUM_VLAN
      * flag unless it does in fact support RX VLAN stripping and it is enabled.
      */
     if (IP_BIT_ISSET(pMblk->mBlkPktHdr.csum_flags, CSUM_VLAN))
     {
         IPNET_ETH_PKT_SET_VLAN_TAG(firewallPkt, pMblk->mBlkPktHdr.vlan);
         IP_BIT_SET(firewallPkt->flags, IPCOM_PKT_FLAG_VLAN_TAG);
     }
#endif /* IPCOM_USE_HW_VLAN_TAGGING */


     if (ipfirewall != IP_NULL) {
         
         Ip_u16 etherType;
         firewallPkt->start = (int)(pMblk->mBlkHdr.mData - pMblk->pClBlk->clNode.pClBuf);
         firewallPkt->data = (Ip_u8 *)pMblk->pClBlk->clNode.pClBuf;
         firewallPkt->maxlen = pMblk->mBlkHdr.mLen + firewallPkt->start;
         firewallPkt->end = pMblk->mBlkHdr.mLen + firewallPkt->start;
         firewallPkt->ipstart = firewallPkt->start + IPNET_ETH_HDR_SIZE;
         start = firewallPkt->start;

         etherType = ip_ntohs(*(Ip_u16 *)
                 &firewallPkt->data[firewallPkt->start + IPNET_ETH_ADDR_SIZE*2]);

         /* Process VLAN header */
         if (etherType == IPNET_ETH_P_8021Q_VLAN)
         {
             firewallPkt->ipstart += IPNET_VLAN_TAG_SIZE;
             etherType = ip_ntohs(*(Ip_u16 *)&firewallPkt->data[firewallPkt->start +
                                       IPNET_ETH_ADDR_SIZE*2 + IPNET_VLAN_TAG_SIZE]);
         }

         if( ((dir == VXBRIDGE_INGRESS) || (dir == VXBRIDGE_INGRESS_EGRESS))
                 && pSrcPortInfo != NULL ) {

             if(pSrcPortInfo->netif == IP_NULL) return TRUE;
             
             if (IP_UNLIKELY(IP_BIT_ISFALSE(pSrcPortInfo->netif->ipcom.flags,
                             IP_IFF_RUNNING)))
                 goto drop;

             /* Make sure we have the right ether type */
             if( (pSrcPortInfo->netif->ipcom.type != IP_IFT_ETHER) &&  
                     (pSrcPortInfo->netif->ipcom.type != IP_IFT_L2VLAN) )
                 goto drop;


             firewallPkt->ifindex = pSrcPortInfo->netif->ipcom.ifindex;

             /* MAC layer filtering */
             if(ipfirewall_mac_input_hook(&pSrcPortInfo->netif->ipcom,
                         firewallPkt) == 1) {
                 /* MAC filter blocked packed */
                 DBG_PRINT(("Ingress packet on bridge interface %s%d"
                             " was blocked by the MAC filter.\n",
                             pSrcPortInfo->name,pSrcPortInfo->unitNum));
                 goto drop;
             } else {
                 DBG_PRINT(("Ingress packet on bridge interface %s%d"
                             " was was allowed to pass by the MAC filter\n",
                             pSrcPortInfo->name,pSrcPortInfo->unitNum));
             }
         
             /* IP layer filtering */
             if ((etherType == IPNET_ETH_P_IP) ||
                     (etherType == IPNET_ETH_P_IPV6))
             {
                 if(etherType == IPNET_ETH_P_IP) {
                     IP_BIT_SET(firewallPkt->flags, IPCOM_PKT_FLAG_IPV4);
                     IP_BIT_CLR(firewallPkt->flags, IPCOM_PKT_FLAG_IPV6);
                 } else if (etherType == IPNET_ETH_P_IPV6) {
                     IP_BIT_SET(firewallPkt->flags, IPCOM_PKT_FLAG_IPV6);
                     IP_BIT_CLR(firewallPkt->flags, IPCOM_PKT_FLAG_IPV4);
                 }

                 firewallPkt->start = firewallPkt->ipstart;

                 if(ipfirewall_input_hook(&pSrcPortInfo->netif->ipcom,
                             firewallPkt,NULL) == 1) {
                     /* IP filter blocked packed */
                     DBG_PRINT(("Ingress packet on bridge interface %s%d"
                                 " was blocked by the IP filter.\n",
                                 pSrcPortInfo->name,pSrcPortInfo->unitNum));
                     goto drop;
                 } else {
                     DBG_PRINT(("Ingress packet on bridge interface %s%d"
                                 " was was allowed to pass by the IP filter\n",
                                 pSrcPortInfo->name,pSrcPortInfo->unitNum));
                 }

             }
         } 

         if( ((dir == VXBRIDGE_EGRESS) ||
                     (dir == VXBRIDGE_INGRESS_EGRESS)) &&
                 pDestPortInfo != NULL ) {

             if(pDestPortInfo->netif == IP_NULL) return TRUE;

             if (IP_UNLIKELY(IP_BIT_ISFALSE(pDestPortInfo->netif->ipcom.flags,
                             IP_IFF_RUNNING)))
                 goto drop;

             /* Make sure we have the right ether type */
             if( (pDestPortInfo->netif->ipcom.type != IP_IFT_ETHER) &&  
                     (pDestPortInfo->netif->ipcom.type != IP_IFT_L2VLAN) )
                 goto drop;

             firewallPkt->ifindex  = pDestPortInfo->netif->ipcom.ifindex;

             /* IP layer filtering */
             if ((etherType == IPNET_ETH_P_IP) ||
                     (etherType == IPNET_ETH_P_IPV6))
             {

                 if(etherType == IPNET_ETH_P_IP) {
                     IP_BIT_SET(firewallPkt->flags, IPCOM_PKT_FLAG_IPV4);
                     IP_BIT_CLR(firewallPkt->flags, IPCOM_PKT_FLAG_IPV6);
                 } else if (etherType == IPNET_ETH_P_IPV6) {
                     IP_BIT_SET(firewallPkt->flags, IPCOM_PKT_FLAG_IPV6);
                     IP_BIT_CLR(firewallPkt->flags, IPCOM_PKT_FLAG_IPV4);
                 }

                 firewallPkt->start = firewallPkt->ipstart;

                 if(ipfirewall_output_hook(&pDestPortInfo->netif->ipcom,
                             firewallPkt,NULL) == 1) {
                     /* IP filter blocked packed */
                     DBG_PRINT(("Egress packet on bridge interface %s%d"
                                 " was blocked by the IP filter\n",
                                 pDestPortInfo->name, pDestPortInfo->unitNum));
                     goto drop;
                 } else {
                     DBG_PRINT(("Egress packet on bridge interface %s%d"
                                 " was allowed to pass by the IP filter\n",
                                 pDestPortInfo->name, pDestPortInfo->unitNum));
                 }
             }

             firewallPkt->start = start;

             if(ipfirewall_mac_output_hook(&pDestPortInfo->netif->ipcom,
                         firewallPkt) == 1) {
                 /* MAC filter blocked packed */
                 DBG_PRINT(("Egress packet on bridge interface %s%d"
                             " was blocked by the MAC filter\n",
                             pDestPortInfo->name, pDestPortInfo->unitNum));
                 goto drop;
             } else {
                 DBG_PRINT(("Egress packet on bridge interface %s%d"
                             " was allowed to pass by the MAC filter\n",
                             pDestPortInfo->name, pDestPortInfo->unitNum));
             }
         }
     }

     return TRUE;

drop:
     netMblkClChainFree(pMblk);
     return FALSE;
    }
#endif

/*****************************************************************************
 函 数 名  : IPS_MNTN_BridgeTraceHook
 功能描述  : 捕获网桥中转
 输入参数  : pMblk              Mblk控制块
             pDestPortInfo      目的端口
             pSrcPortInfo       源端口
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年8月3日
    作    者   : 冯家麟
    修改内容   : 新生成函数
  2.日    期   : 2013年7月30日
    作    者   : caikai
    修改内容   : VoLTE特性需要将其移植至V9

*****************************************************************************/
static void IPS_MNTN_BridgeTraceHook
(
    M_BLK*              pMblk,
    BRIDGE_PORT_INFO*   pDestPortInfo,
    BRIDGE_PORT_INFO*   pSrcPortInfo
)
{
    IPS_MNTN_CCORE_CAPTURE_CB            pCaptureCb;
    IPS_MNTN_CCORE_CAPTURE_PARA_STRU     stIpsMntnCapturePara;

    if ( (NULL == pMblk)||(NULL == pDestPortInfo)||(NULL == pSrcPortInfo) )
    {
        printf("bridgeTraceHook pMblk=0x%x, pDestPortInfo=0x%x, pSrcPortInfo=0x%x",\
        pMblk,pDestPortInfo,pSrcPortInfo);
        return ;
    }

    pCaptureCb  = g_astCallbackTbl[IPS_MNTN_CCORE_BRIDGE_DATA_CB];

    stIpsMntnCapturePara.paucSrcPort      = pSrcPortInfo->name;
    stIpsMntnCapturePara.paucDestPort     = pDestPortInfo->name;
    stIpsMntnCapturePara.pucData          = pMblk->mBlkHdr.mData;
    stIpsMntnCapturePara.usLen            = (UINT16)pMblk->mBlkHdr.mLen;

    if ( NULL != pCaptureCb )
    {
        if ((strcmp(MIRROR_DEV_NAME,pSrcPortInfo->name) != 0) && (strcmp(MIRROR_DEV_NAME,pDestPortInfo->name) != 0))
        {
            pCaptureCb(&stIpsMntnCapturePara);
        }
    }
}

