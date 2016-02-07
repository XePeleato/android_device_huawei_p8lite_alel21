/* mirrorEnd.c - mirroring END device driver */

/*
 * Copyright (c) 2000-2006 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River license agreement.
 */

/*
modification history
--------------------
01w,12feb09,ggz  Change paths to bridge.h, mirrorEnd.h header files
01v,27nov07,tlu  Add support to set/update interface RUNNING/NOT RUNNING state
01u,25oct06,kch  Fixed warnings.
01t,15aug06,tlu  Ported to IPNET
01s,04feb06,kch  Fixed apigen doc errors and warnings.
01r,15apr05,svk  Update API descriptions
01q,07feb05,svk  Remove unused/obsolete include
01p,19jul04,myz  removed end_err_block_flag and moved muxTxRestart to bridge.
01o,07aug03,myz  zero out drvCtrl at init, call mirrorEndStop and undo last fix
		 in mirrorEndUnload
01n,03jun03,myz  clear the snarfCount in mirrorEndUnload
01m,22may03,svk  Add a missing cast, debug message changes
01l,06may03,svk  Remove the costly netJobAdd in mirrorEndSend()
01k,02may03,svk  Add multicast support, avoid a possible deadlock, 
                 address optimzation, re-format for coding convention
01j,17apr03,svk  Update copyright date
01i,22nov02,zhu  removed compiler warnings
01h,22nov02,zhu  added polling mode support
01g,18oct02,ep   removing gnu warning
01f,15apr02,zhu  changed arg types to call mirrorEndReceive
01e,15apr02,zhu  made changes to docs
01d,10apr02,svk  Replace another netJobAdd with mirrorEndReceive
01c,09apr02,svk  Replace a netJobAdd with mirrorEndReceive
01b,27sep01,zhu  TSR#257133 flow control
01a,10oct00,jhl  created
*/

/*
DESCRIPTION
This library implements a mirroring END device driver.

INCLUDE FILES: mirrorEnd.h

SEE ALSO: endLib

*/

/* includes */


#include <ipcom_sock.h>
#include <etherMultiLib.h>
#include <netLib.h>
#include <stdio.h>
#include <string.h>


/* Uncomment the line below to enable debug messages */
/*#define END_MIRROR_DEBUG */

#ifdef END_MIRROR_DEBUG

int endMirrorDebug = 1;
#define LOG_MSG \
        if (endMirrorDebug) \
		printf		
#else

#define LOG_MSG(a0,a1,a2,a3,a4,a5,a6)

#endif

#undef END_MACROS


#include "endLib.h"
#include "endMedia.h"
#include "lstLib.h"
#include "netBufLib.h"

#include "bridge.h"
#include "mirrorEnd.h"

/* defines */


#define     LENGTH_MIN_FBUF     9       /* min. size of the first buffer in a frame */
#define     SPEED               10000000/* ethernet speed */

#define     MAX_MIRROR_CHANNELS 2
#define     CHANNEL_DOWN    	0
#define     CHANNEL_UP      	1


/* macros */

/* A shortcut for getting the hardware address from the MIB II stuff. */

#define     END_HADDR(pEnd)     ((pEnd)->mib2Tbl.ifPhysAddress.phyAddress)
#define     END_HADDR_LEN(pEnd) ((pEnd)->mib2Tbl.ifPhysAddress.addrLength)

/* both pMac1 and pMac2 must point to a two byte boundary */

#define     MAC_ADDR_EQ(pMac1,pMac2)	\
                ( (pMac1[0] == pMac2[0]) && \
                  (pMac1[1] == pMac2[1]) && \
                  (pMac1[2] == pMac2[2]) ) 

/* pMacAddr must point to a two byte boundary */

#define     IS_BROADCAST(pMacAddr) \
                ( (pMacAddr[0] == bcastMacAddr[0]) && \
                  (pMacAddr[1] == bcastMacAddr[1]) && \
                  (pMacAddr[2] == bcastMacAddr[2]) )
                  
/* typedefs */

/* END control structure */
typedef struct end_ctrl 
    {
    END_OBJ     endObject;      /* base class */
    int         unit;           /* unit number */
    BOOL        promiscuous;    /* promiscuous on or off */
    BOOL        polling;        /* polling mode */
    END_OBJ *   pPhyEnd;
    UINT16      enetAddr[3];    /* ethernet address of this entry */
    } END_CTRL;

/* globals */

int mirrorSendStatus = OK;

extern LIST bridgePortList;


/* locals */

LOCAL UINT16 bcastMacAddr[3] = { 0xFFFF, 0xFFFF, 0xFFFF };

LOCAL END_CTRL  drvCtrl[MAX_MIRROR_CHANNELS];   /* array of driver control */

LOCAL int channelState[MAX_MIRROR_CHANNELS] = {CHANNEL_DOWN, CHANNEL_DOWN };

/* prototypes */

END_OBJ*        mirrorEndLoad(char* initString);

LOCAL STATUS    mirrorEndStart(END_CTRL* pDrvCtrl);
LOCAL STATUS    mirrorEndStop(END_CTRL* pDrvCtrl);
LOCAL STATUS    mirrorEndUnload(END_CTRL* pDrvCtrl);
LOCAL int       mirrorEndIoctl(END_CTRL* pDrvCtrl, int cmd, caddr_t data);
LOCAL STATUS    mirrorEndSend(END_CTRL* pDrvCtrl, M_BLK* pMblk);
LOCAL void      mirrorEndReceive(END_CTRL* pDrvCtrl, M_BLK* pMblk);

LOCAL STATUS    mirrorEndMCastAddrAdd(END_CTRL* pDrvCtrl, char* pAddress);
LOCAL STATUS    mirrorEndMCastAddrDel(END_CTRL* pDrvCtrl, char* pAddress);
LOCAL STATUS    mirrorEndMCastAddrGet(END_CTRL* pDrvCtrl, MULTI_TABLE* pTable);

LOCAL STATUS    mirrorEndPollStart(END_CTRL* pDrvCtrl);
LOCAL STATUS    mirrorEndPollStop(END_CTRL* pDrvCtrl);
LOCAL STATUS    mirrorEndPollSend(END_CTRL* pDrvCtrl, M_BLK* pMblk);
LOCAL STATUS    mirrorEndPollReceive(END_CTRL* pDrvCtrl, M_BLK* pMblk);

LOCAL BOOL      isInMulticastList(END_CTRL* pDrvCtrl, UINT16* enetAddr);

extern STATUS bridgeNextPhyDevEndGet(char *,int *);

/*
 * Define the device function table.  This is static across all driver
 * instances.
 */

LOCAL NET_FUNCS netFuncs = 
    {
    (FUNCPTR)mirrorEndStart,            /* start func. */
    (FUNCPTR)mirrorEndStop,             /* stop func. */
    (FUNCPTR)mirrorEndUnload,           /* unload func. */
    (FUNCPTR)mirrorEndIoctl,            /* ioctl func. */
    (FUNCPTR)mirrorEndSend,             /* send func. */
    (FUNCPTR)mirrorEndMCastAddrAdd,     /* multicast add func. */
    (FUNCPTR)mirrorEndMCastAddrDel,     /* multicast delete func. */
    (FUNCPTR)mirrorEndMCastAddrGet,     /* multicast get fun. */
    (FUNCPTR)mirrorEndPollSend,         /* polling send func. */
    (FUNCPTR)mirrorEndPollReceive,      /* polling receive func.  */
    endEtherAddressForm,                /* Put address info into a packet.  */
    endEtherPacketDataGet,              /* Get a pointer to packet data. */
    endEtherPacketAddrGet               /* Get packet addresses. */
    };



/*******************************************************************************
* mirrorEndLoad - initialize the driver and device
*
* This routine initializes the driver and the device. All of the 
* device-specific parameters are passed in the <initString> parameter. 
* <initstring> is of the following format, where <unit> is the device
* driver unit number:
*
* <unit>:
*
* Mirror driver unit number can be 'MIRROR_STACK_UNIT_NUM' (0) or
* 'MIRROR_BRIDGE_UNIT_NUM' (1). These are defined in:
*
* target/h/wrn/bridge/mirrorEnd.h
*
* Typically, unit 0 is attached to the network stack and unit 1 is attached to
* the bridge.
*
* RETURNS: An END object pointer on success, or NULL on error
*
* ERRNO: N/A
*
*/

END_OBJ* mirrorEndLoad
    (
    char*   initString
    )
    {
    END_CTRL*   pDrvCtrl;

    if (initString == NULL)
        return NULL;
    
    if (initString[0] == 0)
        {
        bcopy((char *)MIRROR_DEV_NAME, initString, MIRROR_DEV_NAME_LEN);
        return NULL;
        }

    /* Parse InitString */
    switch (initString[0])
        {
        case '0':
            pDrvCtrl = &drvCtrl[0];
	    bzero((char *)pDrvCtrl,sizeof(END_CTRL)); 
            pDrvCtrl->unit = 0;
            SYS_ENET_ADDR_GET((char *) pDrvCtrl->enetAddr);

            break;
        
        case '1':
            pDrvCtrl = &drvCtrl[1];
	    bzero((char *)pDrvCtrl,sizeof(END_CTRL)); 
            pDrvCtrl->unit = 1;
            
            /* if unit ever REALLY needs a MAC address, change this appropriately */
            SYS_ENET_ADDR_GET((char *) pDrvCtrl->enetAddr);  
            
            break;
        
        default:
            return NULL;
        }

    /* Check if we are already attached */
    if (pDrvCtrl->endObject.attached == TRUE)
        return &pDrvCtrl->endObject;

    /* endObject initializations */
    if (END_OBJ_INIT(&pDrvCtrl->endObject, 
                     (void*)pDrvCtrl, 
                     MIRROR_DEV_NAME,
                     pDrvCtrl->unit, 
                     &netFuncs,
                     MIRROR_END_OBJ_STRING) == ERROR)
        {
        return NULL;
        }

    /* Initialize MIB2 entries */
    if (END_MIB_INIT(&pDrvCtrl->endObject, 
                     M2_ifType_ethernet_csmacd,
                     (UCHAR *) pDrvCtrl->enetAddr, 
                     6, 
                     ETHERMTU,
                     SPEED) == ERROR)
        {
        return NULL;
        }

    /* we need to import the memory pool from a real physical driver. At least
     * the WDB end driver is using it
     */

    if (pDrvCtrl->unit == 0)
        {
        char devName[END_NAME_MAX];
        int unit = 0;
        bzero(devName,END_NAME_MAX);

	if (bridgeNextPhyDevEndGet(devName,&unit) == ERROR)
            {
            printf("mirrorEndLoad: No physical device found\n");
            }
        else
	    {
	    END_OBJ * pEnd;

	    pEnd = endFindByName(devName,unit);

            if (pEnd != NULL)
	        pDrvCtrl->endObject.pNetPool = pEnd->pNetPool;

            pDrvCtrl->pPhyEnd = pEnd;
	    }
        }

    /* Mark the device ready */
    /* IFF_SCAT is not defined by default */
/*
    END_OBJ_READY(&pDrvCtrl->endObject,
                  IFF_NOTRAILERS | IP_IFF_BROADCAST | IFF_MULTICAST);
*/
    END_OBJ_READY(&pDrvCtrl->endObject,
                  IP_IFF_BROADCAST | IP_IFF_MULTICAST);

    /* Successful return */
    return &pDrvCtrl->endObject;
    }


/*******************************************************************************
* mirrorEndStart - start the device 
*
* This routine marks the interface as up.
*
* The complement of this routine is mirrorEndStop().  Once a unit is reset by
* mirrorEndStop(), it may be re-initialized to a running state by this routine.
* 
* RETURNS: OK if successful, otherwise ERROR
*
* ERRNO: N/A
*
* NOMANUAL
*/

LOCAL STATUS mirrorEndStart
    (
    END_CTRL*   pDrvCtrl
    )
    {
    /* initialize flag(s) */
    pDrvCtrl->polling     = FALSE;
    pDrvCtrl->promiscuous = FALSE;

    /* mark the channel state as "UP" */
    channelState[pDrvCtrl->unit] = CHANNEL_UP;
    
    /* raise the interface flags - mark the device as up */
    END_FLAGS_SET (&pDrvCtrl->endObject, IP_IFF_UP | IP_IFF_RUNNING);

    /* update the interface flags */
    mirrorEndRunningState();

    return OK;
    }


/*******************************************************************************
* mirrorEndStop - stop the device 
*
* This routine marks the interface as down.
*
* The complement of this routine is mirrorEndStart().  Once a unit is 
* stop in this routine, it may be re-initialized to a running state by 
* mirrorEndStart().
*
* RETURNS: OK or ERROR
*
* ERRNO: N/A
*
* NOMANUAL
*/

LOCAL STATUS mirrorEndStop
    (
    END_CTRL*   pDrvCtrl
    )
    {
    /* mark the channel state as "DOWN" */
    channelState[pDrvCtrl->unit] = CHANNEL_DOWN;
    
    /* mark the driver as down */
    END_FLAGS_CLR (&pDrvCtrl->endObject, IP_IFF_UP | IP_IFF_RUNNING);

    return OK;
    }


/******************************************************************************
* mirrorEndUnload - unload a driver from the system
*
* This function first brings down the device, and then frees any
* stuff that was allocated by the driver in the load function.
*
* RETURNS: OK always
*
* ERRNO: N/A
*
* NOMANUAL
*/

LOCAL STATUS mirrorEndUnload
    (
    END_CTRL*   pDrvCtrl
    )
    {
    mirrorEndStop(pDrvCtrl);

    END_OBJECT_UNLOAD(&pDrvCtrl->endObject);
    return OK;
    }


/*******************************************************************************
* mirrorEndIoctl - network interface control routine
*
* This routine implements the network interface control functions.
* It handles EIOCSIFADDR, EIOCGADDR, EIOCSFLAGS, EIOCGFLAGS,
* EIOCPOLLSTART, EIOCPOLLSTOP, EIOCGMIB2 and EIOCGFBUF ENDERRBLOCK commands.
*
* RETURNS: OK if successful, otherwise EINVAL
*
* ERRNO: N/A
*
* NOMANUAL
*/

LOCAL int mirrorEndIoctl
    (
    END_CTRL*   pDrvCtrl,
    int         cmd,
    caddr_t     data
    )
    {
    int         error = 0;
    long        value;
    END_OBJ*    pEndObj = &pDrvCtrl->endObject;
    END_MEDIA * pMedia;


    switch ((UINT)cmd)
        {
        case EIOCSADDR:
            if (data == NULL)
                {
                error = EINVAL;
                }
            else 
                {
                bcopy((char*)data, 
                      (char*)END_HADDR(pEndObj), 
                      END_HADDR_LEN(pEndObj));
                }
            break;

        case EIOCGADDR:
            if (data == NULL)
                {
                error = EINVAL;
                }
            else 
                {
                bcopy((char*)END_HADDR(pEndObj), 
                      (char*)data,
                      END_HADDR_LEN(pEndObj));
                }
            break;
   
        case EIOCSFLAGS:
            value = (long)data;
            if (value < 0)
                {
                value = -value;
                value--;
                END_FLAGS_CLR(pEndObj, value);
                }
            else 
                {
                END_FLAGS_SET(pEndObj, value);
                }
	    
            if (END_FLAGS_GET(pEndObj) & IP_IFF_PROMISC)
                {
                pDrvCtrl->promiscuous = TRUE;
                }
            else 
                {
                pDrvCtrl->promiscuous = FALSE;
                }

            break;

        case EIOCGFLAGS:
            if (data == NULL)
                {
                error = EINVAL;
                }
            else 
                {
                *(int*)data = END_FLAGS_GET(pEndObj);
                }
            break;

        case EIOCGIFMEDIA:
            if (data == NULL)
                error = EINVAL;
            else
                {
                value = END_FLAGS_GET(pEndObj);

                pMedia = (END_MEDIA *)data;
                pMedia->endMediaActive = IFM_AVALID;
                pMedia->endMediaStatus = IFM_AVALID;
		if (value & IP_IFF_RUNNING)
		{
		    LOG_MSG("ioctl with EIOCGIFMEDIA, set active bit \n",
			1, 2, 3, 4, 5, 6);
                    pMedia->endMediaStatus |= IFM_ACTIVE;
		}
		else
		{
		    LOG_MSG("ioctl with EIOCGIFMEDIA, clear active bit \n",
			1, 2, 3, 4, 5, 6);
                    pMedia->endMediaStatus &= ~IFM_ACTIVE;
                }
		}
            break;

        case EIOCPOLLSTART:
            error = mirrorEndPollStart(pDrvCtrl);
            break;

        case EIOCPOLLSTOP:
            error = mirrorEndPollStop(pDrvCtrl);
            break;

        case EIOCGMIB2:
            if (data == NULL)
                {
                error = EINVAL;
                }
            else 
                {
                bcopy((char*)&pDrvCtrl->endObject.mib2Tbl, 
                      (char*)data,
                      sizeof(pDrvCtrl->endObject.mib2Tbl));
                }
            break;

        case EIOCGFBUF:
            if (data == NULL)
                {
                error =  EINVAL;
                }
            else 
                {
                *(int*)data = LENGTH_MIN_FBUF;
                }
            break;

        default:
            error = EINVAL;
            break;
        }

    return error;
    }


/******************************************************************************
* mirrorEndRunningState - update the driver running state 
*
* This function gets the state from all bridge's underlying END devices, then 
* update the bridge's RUNNING state accordingly. If all underlying devices
* are inactive, the bridge interface is marked inactive, and its RUNNING bit
* is cleared. As long as one underlying device is active, the bridge interface
* will be marked as active, and its RUNNING bit is set.
*
* RETURNS: OK always
*
* ERRNO: N/A
*
* NOMANUAL
*/

STATUS mirrorEndRunningState(void)
    {
    BRIDGE_PORT_INFO*   pPortInfo;
    END_OBJ *pEnd0, *pEnd1;
    END_MEDIA media;
    int error;
    long flags;


    pEnd1 = endFindByName (MIRROR_DEV_NAME,MIRROR_STACK_UNIT_NUM);
    pEnd0 = endFindByName (MIRROR_DEV_NAME,MIRROR_BRIDGE_UNIT_NUM);

    if (pEnd1) 
	flags = END_FLAGS_GET(pEnd1);
    else
	return OK;

    for (pPortInfo = (BRIDGE_PORT_INFO*)lstFirst(&bridgePortList);
         (pPortInfo != NULL) && (strcmp(pPortInfo->name, MIRROR_DEV_NAME)!=0);
         pPortInfo = (BRIDGE_PORT_INFO*)lstNext((NODE*)pPortInfo))
        {
        muxIoctl(pPortInfo->pMuxBindCookie,EIOCGIFMEDIA,(char *)&media);	    
        LOG_MSG("mirrorEndRunningState: port %s, unit %d, status 0x%x\n",
                  pPortInfo->name, pPortInfo->unitNum, 
		  media.endMediaStatus, 4, 5, 6);

        if ((media.endMediaStatus & (IFM_AVALID|IFM_ACTIVE)) == 
				(IFM_AVALID|IFM_ACTIVE))
	    {
            if ((flags & IP_IFF_RUNNING) == 0)
	        {
	        LOG_MSG("mirrorEndRunningState: state changed to RUNNING\n",
			1, 2, 3, 4, 5, 6);

    	        /* raise both interface flags - mark the devices as RUNNING */
    	        if (pEnd0) END_FLAGS_SET (pEnd0, IP_IFF_RUNNING);
    	        END_FLAGS_SET (pEnd1, IP_IFF_RUNNING);

	        /* inform the stack about the interface UP state change */
	        jobQueueStdPost (netJobQueueId, NET_TASK_QJOB_PRI,
                         muxLinkUpNotify, pEnd1,
                         NULL, NULL, NULL, NULL);
	        }

		return OK;
	    }
        } 

    if (flags & IP_IFF_RUNNING)
        {
	LOG_MSG("mirrorEndRunningState: state changed to NOT RUNNING\n",
		1, 2, 3, 4, 5, 6);

        /* mark both drivers as NOT RUNNING */
        if (pEnd0)  END_FLAGS_CLR (pEnd0, IP_IFF_RUNNING);
        END_FLAGS_CLR (pEnd1, IP_IFF_RUNNING);

	/* inform the stack about the interface UP state change */
	jobQueueStdPost (netJobQueueId, NET_TASK_QJOB_PRI,
                    muxLinkDownNotify, pEnd1,
                    NULL, NULL, NULL, NULL);
        }

    return OK;
    }


/*******************************************************************************
* mirrorEndSend - output packet to network interface device
*
* This routine() takes a M_BLK* and sends off the data in the M_BLK*.
* The buffer must already have the addressing information properly installed
* in it. This is done by a higher layer.
*
* muxSend() calls this routine each time it wants to send a packet.
*
* RETURNS: OK , ERROR or END_ERR_BLOCK
*
* ERRNO: N/A
*
* NOMANUAL
*/

LOCAL STATUS mirrorEndSend
    (
    END_CTRL*   pDrvCtrl,
    M_BLK*      pMblk
    )
    {
    int         otherHalfOfMirror;
    int status = OK;

    LOG_MSG("mirrorEndSend: unit: %d, pollMode: %d, ",
            (int)pDrvCtrl->unit, 
            (int)pDrvCtrl->polling, 
            4, 5, 6, 7);
    LOG_MSG("dst:%02X:%02X:%02X:%02X:%02X:%02X, ",
            *(UCHAR *)(pMblk->mBlkHdr.mData),
            *((UCHAR *)(pMblk->mBlkHdr.mData) + 1),
            *((UCHAR *)(pMblk->mBlkHdr.mData) + 2),
            *((UCHAR *)(pMblk->mBlkHdr.mData) + 3),
            *((UCHAR *)(pMblk->mBlkHdr.mData) + 4),
            *((UCHAR *)(pMblk->mBlkHdr.mData) + 5));
    LOG_MSG("src:%02X:%02X:%02X:%02X:%02X:%02X\n",
            *((UCHAR *)(pMblk->mBlkHdr.mData) + 6),
            *((UCHAR *)(pMblk->mBlkHdr.mData) + 7),
            *((UCHAR *)(pMblk->mBlkHdr.mData) + 8),
            *((UCHAR *)(pMblk->mBlkHdr.mData) + 9),
            *((UCHAR *)(pMblk->mBlkHdr.mData) + 10),
            *((UCHAR *)(pMblk->mBlkHdr.mData) + 11));

    if (pDrvCtrl->polling == TRUE)
        {
        netMblkClChainFree (pMblk); /* free the given mBlk chain */
        return ERROR;
        }

    otherHalfOfMirror = pDrvCtrl->unit ^ 1;     /* 0->1, 1->0 */

    if (channelState[otherHalfOfMirror] == CHANNEL_UP)
        {
        /* other half of mirror is up, send packet there */

       
	if (pDrvCtrl->unit == MIRROR_STACK_UNIT_NUM)
            mirrorSendStatus = OK;

        /*
         * In the previous version of the Bridge, calling mirrorEndReceive
         * directly caused deadlock when bridge tries to flood multicasts
         * outgoing from local stack. One way to avoid the deadlock is to
         * call mirrorEndReceive from tNetTask by using netJobAdd. But this
         * causes performance hit, and also overloads the netJob ring buffer.
         *
         * The better solution is to call mirrorEndReceive directly, and
         * avoid deadlock by not taking the bridge port list semaphore when
         * flooding packets to all bridge ports. Taking this semaphore was
         * previously thought necessary to guard against removal of a bridge
         * port when the bridge is flooding packets to that port. The 
         * possibility of this occurring is very small, and can be avoided
         * if the user makes sure the bridge port is free of incoming and
         * outgoing traffic before removing the bridge port.
         */

        mirrorEndReceive(&drvCtrl[otherHalfOfMirror], pMblk);

        if (pDrvCtrl->unit == MIRROR_STACK_UNIT_NUM)
            status = mirrorSendStatus;
        }
    else 
        {
        /* other half of mirror is not up, toss the packet */

        netMblkClChainFree(pMblk);
        }

    /* Bump the statistic counter. */
    END_ERR_ADD(&pDrvCtrl->endObject, MIB2_OUT_UCAST, +1);

    return status;
    }

/*******************************************************************************
* isInMulticastList - is the address is a multicast address for this device?
*
* This routine checks whether the given address matches a multicast address
* from the current list of multicast addresses. 
*
* RETURNS: TRUE or FALSE
*
* ERRNO: N/A
*
* NOMANUAL
*/

LOCAL BOOL isInMulticastList
    (
    END_CTRL*	pDrvCtrl,     /* pointer to END_CTRL structure */
    UINT16*	enetAddr      /* address to check */
    )
    {
    ETHER_MULTI* pCurr;

    pCurr = (ETHER_MULTI *)lstFirst(&pDrvCtrl->endObject.multiList);

    while (pCurr != (ETHER_MULTI *)NULL)
        {
        if (((UINT32)(pCurr->addr) & 0x01) == 0) 
            {
            /* address is aligned on a 2 or 4 byte boundary */ 

            if (MAC_ADDR_EQ(((UINT16 *)(pCurr->addr)), enetAddr) == TRUE)
                return TRUE;
            }
        else
            {
            /* address is aligned on a byte-boundary */

            if (bcmp(pCurr->addr, (char *)enetAddr, 6) == 0)
                return TRUE;
            }

        pCurr = (ETHER_MULTI *)lstNext(&pCurr->node);
        }
        
    return FALSE;
    }

/*******************************************************************************
* mirrorEndReceive - process an input frame
*
* This routine processes an input frame, then passes it up to the higher 
* level in a form it expects.  Buffer loaning, promiscuous mode are all 
* supported.  Trailer protocols is not supported.
*
* RETURNS: N/A
*
* ERRNO: N/A
*
* NOMANUAL
*/

LOCAL void mirrorEndReceive
    (
    END_CTRL*   pDrvCtrl,       /* pointer to END_CTRL structure */
    M_BLK*      pMblk
    )
    { 
    UINT16 * pTmp;
    UINT16   enetAddr[3];

    LOG_MSG("mirrorEndReceive: unit: %d, ", 
	    (int)pDrvCtrl->unit, 
	    2, 3, 4, 5, 6);
    LOG_MSG("dst:%02X:%02X:%02X:%02X:%02X:%02X, ",
            *(UCHAR *)(pMblk->mBlkHdr.mData),
            *((UCHAR *)(pMblk->mBlkHdr.mData) + 1),
            *((UCHAR *)(pMblk->mBlkHdr.mData) + 2),
            *((UCHAR *)(pMblk->mBlkHdr.mData) + 3),
            *((UCHAR *)(pMblk->mBlkHdr.mData) + 4),
            *((UCHAR *)(pMblk->mBlkHdr.mData) + 5));
    LOG_MSG("src:%02X:%02X:%02X:%02X:%02X:%02X\n",
            *((UCHAR *)(pMblk->mBlkHdr.mData) + 6),
            *((UCHAR *)(pMblk->mBlkHdr.mData) + 7),
            *((UCHAR *)(pMblk->mBlkHdr.mData) + 8),
            *((UCHAR *)(pMblk->mBlkHdr.mData) + 9),
            *((UCHAR *)(pMblk->mBlkHdr.mData) + 10),
            *((UCHAR *)(pMblk->mBlkHdr.mData) + 11));
        
   /* don't allow the packet through if in polling mode */

   if (pDrvCtrl->polling == TRUE)
       {
       netMblkClChainFree(pMblk);
       return;
       }

   /* if unit is not in promiscuous mode, filter packets to receive only
    * packets for our MAC address or broadcasts.
    */
    if (pDrvCtrl->promiscuous == FALSE)
        {
        pTmp = (UINT16 *)(pMblk->mBlkHdr.mData);
        if (((UINT32)(pTmp) & 0x01) == 0)
            {
            /* address is aligned on a 2 or 4 byte boundary */
            enetAddr[0] = pTmp[0];
            enetAddr[1] = pTmp[1];
            enetAddr[2] = pTmp[2];
            }
        else
            {
            /* address is aligned on a byte-boundary */
            bcopy((char *)pTmp, (char *)enetAddr, 6);
            }
            
        if ((MAC_ADDR_EQ(enetAddr, pDrvCtrl->enetAddr) == 0) &&
            (IS_BROADCAST(enetAddr) == 0) &&
            (isInMulticastList(pDrvCtrl, enetAddr) == FALSE))
            {
            LOG_MSG("Dest addr not my addr/broadcast/multicast - dropping!\n",
                    1, 2, 3, 4, 5, 6);
            netMblkClChainFree(pMblk);
            return;
            }
        }
   
    LOG_MSG("Sending up the packet to my network stack\n",
            1, 2, 3, 4, 5, 6);

    /* send up to protocol */
    END_RCV_RTN_CALL(&pDrvCtrl->endObject, pMblk);

    /* bump input packet counter */
    END_ERR_ADD(&pDrvCtrl->endObject, MIB2_IN_UCAST, +1);    
    }


/*******************************************************************************
* mirrorEndMCastAddrAdd - add a multicast address for the device
*
* Multicast makes no sense in this driver.  Keep the table for "get" calls.
*
* RETURNS : OK or ERROR
*
* ERRNO: N/A
*
* NOMANUAL
*/

LOCAL STATUS mirrorEndMCastAddrAdd
    (
    END_CTRL*   pDrvCtrl,
    char*       pAddr
    )
    {
    STATUS error;

    error = etherMultiAdd(&pDrvCtrl->endObject.multiList, pAddr);

    if (error == ENETRESET)
        {
        pDrvCtrl->endObject.nMulti++;
        error = OK;
        }

    return (error == OK) ? OK : ERROR;
    }


/*******************************************************************************
* mirrorEndMCastAddrDel - delete a multicast address for the device
*
* This routine deletes a multicast address from the current list of
* multicast addresses.
*
* RETURNS : OK or ERROR
*
* ERRNO: N/A
*
* NOMANUAL
*/

LOCAL STATUS mirrorEndMCastAddrDel
    (
    END_CTRL *  pDrvCtrl,       /* pointer to END_CTRL structure */
    char *      pAddr           /* Address to delete from the table. */
    )
    {
    STATUS error;

    error = etherMultiDel(&pDrvCtrl->endObject.multiList, pAddr);

    if (error == ENETRESET)
        {
        pDrvCtrl->endObject.nMulti--;
        error = OK;
        }

    return (error == OK) ? OK : ERROR;
    }


/*******************************************************************************
* mirrorEndMCastAddrGet - get the current multicast address list
*
* This routine returns the current multicast address list in <pTable>
*
* RETURNS: Multicast address list or NULL
*
* ERRNO: N/A
*
* NOMANUAL
*/

LOCAL STATUS mirrorEndMCastAddrGet
    (
    END_CTRL*       pDrvCtrl,
    MULTI_TABLE*    pTable
    )
    {
    return etherMultiGet(&pDrvCtrl->endObject.multiList, pTable);
    }


/*******************************************************************************
* mirrorEndPollStart - start polling mode
*
* Using polled mode for the mirror driver makes no sense.  Do not use it.
*
* RETURNS: OK
*
* ERRNO: N/A
*
* NOMANUAL
*/

LOCAL STATUS mirrorEndPollStart
    (
    END_CTRL*   pDrvCtrl
    )
    {
    /* Set the polling flag */
    pDrvCtrl->polling = TRUE;

    /* need to set the real device, we either put all devices associated with
     * this vritual mirror device into poll mode, or simply put one
     * currently only put one.
     */

    (*pDrvCtrl->pPhyEnd->pFuncTable->ioctl)(pDrvCtrl->pPhyEnd,EIOCPOLLSTART,0);
    return OK;
    }


/*******************************************************************************
* mirrorEndPollStop - stop polling mode
*
* This routine stops the mirrorEnd from the polling mode.
*
* RETURNS: OK always
*
* ERRNO: N/A
*
* NOMANUAL
*/

LOCAL STATUS mirrorEndPollStop
    (
    END_CTRL*   pDrvCtrl
    )
    {
    /* reset the polling flag */
    pDrvCtrl->polling = FALSE;

    (*pDrvCtrl->pPhyEnd->pFuncTable->ioctl)(pDrvCtrl->pPhyEnd,EIOCPOLLSTOP,0);
    return OK;
    }


/******************************************************************************
* mirrorEndPollSend - transmit a packet in polled mode
*
* This routine is called by a user to try and send a packet on the
* device. It sends a packet directly on the network from the caller without
* going through the normal processes of queuing a packet on an output queue
* and the waiting for the device to decide to transmit it.
*
* If it detects a transmission error, the restart command is issued.
*
* These routine should not call any kernel functions.
*
* RETURNS: OK or EAGAIN
*
* ERRNO: N/A
*
* NOMANUAL
*/

LOCAL STATUS mirrorEndPollSend
    (
    END_CTRL*   pDrvCtrl,
    M_BLK*      pMblk
    )
    {
    STATUS status;

    /* mirror 1 should not call this function */

    if (pDrvCtrl->unit != 0)
	return ERROR;

    /* call the poll routine in the real device */

    status = pDrvCtrl->pPhyEnd->pFuncTable->pollSend(pDrvCtrl->pPhyEnd, 
	                                             pMblk);

    /* Bump the statistic counter. */
    END_ERR_ADD(&pDrvCtrl->endObject, MIB2_OUT_UCAST, +1);

    return OK;
    }


/*******************************************************************************
* mirrorEndPollReceive - receive a packet in polled mode
*
* This routine receives a packet in polled mode
*
* RETURNS: OK or ERROR
*
* ERRNO: N/A
*
* NOMANUAL
*/

LOCAL STATUS mirrorEndPollReceive
    (
    END_CTRL*   pDrvCtrl,
    M_BLK*      pMblk
    )
    {
    return pDrvCtrl->pPhyEnd->pFuncTable->pollRcv(pDrvCtrl->pPhyEnd, 
	                                          pMblk);
    }
