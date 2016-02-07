/* usrNetBridgeCfg.c - Simple Learning Bridge init routines */

/* Copyright 2002-2005 Wind River Systems, Inc. */

/*
modification history
--------------------
02b,26nov07,tlu  Replace INCLUDE_IPCOM_USE_INET6 by INET6
02a,14nov06,tlu  Remove un-used variable addr (defect 69139)
01z,18sep06,tlu  Un-comment ipDetach and ip6Detach
01y,15aug06,tlu  Ported to IPNET stack 
01x,24jun05,svk  Fix another diab compiler warning
01w,27may05,svk  Fix compilation warnings
01v,28apr05,zhu  Set IFF_IFNET6_UP before assigning IPv6 address
01u,19mar05,zhu  Changed the string format to be consistent with NAT and 
                 Firewall
01t,28mar05,svk  Allow bridge to attach to IPv6 stack
01s,02feb05,svk  WDB END handling unnecessary if WDB_END_DEVICE_NAME and
                 WDB_END_DEVICE_UNIT are defined
01r,19oct04,svk  Include config.h for command-line BSP build
01q,07oct04,svk  Detach bridge ports from IPv6 stack also
01p,13sep04,svk  Fix compilation warnings
01o,06jul04,zhu  Updated for dual stack
01n,02jul03,myz  Added INCLUDE_WDB_COMM_END macro to the WDB END check 
01m,03jun03,myz  Added WDB END handling when its interface is a bridge port
01l,30may03,myz  Modify promiscuousModeSet to use driver's ioctl to set the mode
01k,15may03,svk  Added includes needed for cmdline build, fixed warnings
01j,25apr03,zhu  updated copyright
01i,10may02,svk  Changed mirrorInit() and promiscuousModeSet() to be callable
		 from outside configlette
01h,25apr02,svk  Coding standard and linked list changes
01g,24apr02,vks  code-review changes: linked-list usage, project facility
		 parameter dependency removal (usrNetBridgeInit() signature),
		 several minor changes as per coding standards.
01f,22apr02,svk  Replace ports buffer on stack with malloc'ed buffer
01e,11apr02,svk  Add function to use socket ioctl.
01d,10apr02,svk  More minor changes.
01c,10apr02,vks  merged bridge and mirror configlettes
01b,10apr02,svk  Various minor changes.
01a,09apr02,svk  written
*/

/*
DESCRIPTION
This file is used to configure and initialize the Simple Learning Bridge.

INCLUDE FILES: bridge.h, mirrorEnd.h

NOMANUAL
*/

/* includes */


#include <ipProto.h>
#include "wrn/bridge/bridge.h"
#include "wrn/bridge/mirrorEnd.h"
#include "private/muxLibP.h"
#include <ipcom_sock.h>
#include <ipcom_sock2.h>
#include <ipcom_sock6.h>
#include <ipcom_vxworks.h>

/* defines */

#define MIRROR_NAME_MAX		15           /* max size of mirror name */
#define MIRROR_LOAD_STRING	""           /* used in mirror muxDevLoad */
#define PORT_SEPARATOR 		 ","         /* separator in port string */

/* typedefs */

typedef struct                         /* bridge node - BRIDGE_NODE */
    {
    NODE	node;                  /* must be first member for lstLib */
    char	devName[END_NAME_MAX]; /* device name */
    int		unit;                  /* unit number */
    } BRIDGE_NODE;


#ifdef INCLUDE_WDB
#if ((WDB_COMM_TYPE == WDB_COMM_END || defined(INCLUDE_WDB_COMM_END)) && \
     !defined(WDB_END_DEVICE_NAME) && !defined(WDB_END_DEVICE_UNIT))

LOCAL END_TBL_ENTRY* pSavedEndTbl = NULL;
LOCAL int savedTblEntries = 0;
LOCAL STATUS bridgeWdbEndEntryFind (BRIDGE_NODE *);
LOCAL void bridgeEndTblRestore (WIND_TCB *);
IMPORT END_TBL_ENTRY endDevTbl[];

#endif
#endif  /* INCLUDE_WDB */

/* imports */

IMPORT END_OBJ* mirrorEndLoad (char*, void*);
IMPORT void mirrorBaseParamsSet (char*, int);

/* forward declarations */

LOCAL STATUS portsStringParse (char *, LIST*);
LOCAL STATUS bridgePortsInit(LIST*);
LOCAL void   devicePromiscuousModeSet(END_OBJ *);

STATUS mirrorInit (char *, int, char *, int, char*, int);
STATUS promiscuousModeSet (char *, int);

/*******************************************************************************
*
* usrNetBridgeInit - Simple Learning Bridge initialization
*
* This routine initializes the bridge. It first parses the bridge ports
* string argument, and populates a linked list. It then initializes the
* bridge, bridge ports, and the mirror device.
*
* RETURNS:
* OK, or
* ERROR if there is a problem in initializing bridge
*
* NOMANUAL
*/

STATUS usrNetBridgeInit
    (
    char*	pPortString, /* interfaces to be configured as bridge ports */ 
                             /* e.g., "fei0,fei1,elPci0" */
    char* 	pAddr,       /* IP address for this bridge */ 
    int		mask,        /* netmask for this bridge */
    char*	pAddrv6,     /* IPv6 address for this bridge */
    int		prefixLen    /* IPv6 prefix length for this bridge */
    )
    {
    LIST		bridgePortList; /* list of BRIDGE_NODEs */
    BRIDGE_NODE* 	pFirstNode;     /* first node in the list */

    /* Initialize the list */

    lstInit(&bridgePortList);

    /* Parse the port string and populate the list */

    if (portsStringParse(pPortString, &bridgePortList) != OK)
        {
        printf("portsStringParse failed!\n");
        lstFree(&bridgePortList);
        return ERROR;
        }

    /* Initialize the bridge */

    if (bridgeInit() != OK)
        {
        printf("bridgeInit failed!\n");
        lstFree(&bridgePortList);
        return ERROR;
        }

    /* Initialize the bridge ports */

    if (bridgePortsInit(&bridgePortList) != OK)
        {
        printf("bridgePortsInit failed!\n");
        lstFree(&bridgePortList);
        return ERROR;
        }

    /* Initialize the mirror interface.
     *
     * NOTE: The first bridge port's device name and number is used
     * to initialize the mirror interface.
     */

    pFirstNode = (BRIDGE_NODE*) lstFirst(&bridgePortList);
    if (mirrorInit(pFirstNode->devName, pFirstNode->unit, 
		   pAddr, mask, pAddrv6, prefixLen) != OK)
        {
        printf("mirrorInit failed!\n");
        lstFree(&bridgePortList);
        return ERROR;
        }

    /* Cleanup */

    lstFree(&bridgePortList);

    return OK;
    }

/*******************************************************************************
*
* portsStringParse - Parse the string for device names and unit numbers 
*
* This routine parses the given string to extract the device names and
* unit numbers. The extracted values are stored in a linked list. 
*
* RETURNS:
* OK, or
* ERROR if there is a problem in parsing the port string
*
* NOMANUAL
*/

LOCAL STATUS portsStringParse 
    (
    char*	pPorts,          /* string to parse for bridge ports */
    LIST*	pBridgePortList  /* linked list to hold bridge ports */
    )
    {
    char*	pPortsBuf = NULL; /* buffer to hold copy of ports string */
    char*	ptr = NULL;       /* pointer used to copy ports string */
    char*	token = NULL;     /* one token in the ports string */
    STATUS	status = OK;      /* return status */
    BRIDGE_NODE*	pNode;    /* one node in the list */
    char * 	pHolder = NULL;	  /* string holder */
    char * 	pStrEnd = NULL;	  /* string end holder */

    /* Parameter check */

    if (pPorts == NULL)
        {
        printf("Bridge port string is NULL!\n");
        return ERROR;
        }
    if (pBridgePortList == NULL)
        {
        printf("List of bridge ports is NULL!\n");
        return ERROR;
        }

    /* Is the bridge port string configuration parameter specified? */

    if (strlen(pPorts) == 0)
        {
        printf("List of bridge ports not specified! -\n");
        printf("Please set the bridge configuration parameter BRIDGE_PORTS.\n");
        printf("This must a list of comma-separated device name and unit \
number pairs.\nFor example: \"fei,0,fei,1,elPci,0\"\n");
        return ERROR;
        }

    /* 
     * The for-loop below uses strtok() to parse the list of bridge ports.
     * strtok() modifies the string given to it. Therefore, make a copy of the
     * string and give strtok() the copy to work with.
     */

    pPortsBuf = malloc(strlen(pPorts) + 1);
    for (ptr = pPortsBuf; *pPorts != '\0'; pPorts++)
        {
        if (!isspace((int)*pPorts))  /* ignore space */
            *ptr++ = *pPorts;
        } 
    *ptr = '\0';

    for (token = strtok_r(pPortsBuf, PORT_SEPARATOR, &pHolder); token != NULL;
         token = strtok_r(NULL, PORT_SEPARATOR, &pHolder))
        {
        /* Create a new node */
        pNode = (BRIDGE_NODE*) malloc(sizeof(BRIDGE_NODE));
        bzero((char*) pNode, sizeof(BRIDGE_NODE));

        if (strlen(token) > END_NAME_MAX - 1)
            {
            printf("Device name too long in bridge port string: %s\n", token);
            status = ERROR;
            break;
            }

        strcpy(pNode->devName, token);

        if ((token = strtok_r(NULL, PORT_SEPARATOR, &pHolder)) == NULL)
            {
            printf("Unit number missing in bridge port string\n");
            printf("Specify device name and unit number pairs. e.g., fei,0,fei,1\n");
            status = ERROR;
            break;
            }
        pNode->unit = strtol(token, &pStrEnd, 10);
        if (*pStrEnd)
            {
            printf("Invalid unit number in bridge port string: %s\n", token);
            status = ERROR;
            break;
            }

        if (endFindByName(pNode->devName,pNode->unit) == NULL)
            {
            printf("Can't find device %s%d\n", pNode->devName, pNode->unit);
            status = ERROR;
            break;
            } 

	/* Add the node to the list */

        lstAdd(pBridgePortList, (NODE *) pNode);
	
        } /* for */

	
    /* Cleanup */

    if (pPortsBuf != NULL)
	free(pPortsBuf);

    return status;
    }

/*******************************************************************************
*
* bridgePortsInit - Bridge ports initialization
*
* This routine walks through the list of interfaces to initialize as 
* bridge ports and sets each interface to promicuous mode before adding it 
* as a bridge port.
*
* RETURNS:
* OK, or
* ERROR if there is a problem in initializing bridge ports
*
* NOMANUAL
*/

LOCAL STATUS bridgePortsInit 
    (
    LIST*	pBridgePortList  /* list of interfaces */
    )
    {
    STATUS		status = OK;   /* return status */
    BRIDGE_NODE*	pNode = NULL;  /* one node in the list */
    END_OBJ *      pEnd = NULL;
    char ifName[END_NAME_MAX * 2];
    char        * oldifname;

#ifdef INCLUDE_WDB
#if ((WDB_COMM_TYPE == WDB_COMM_END || defined(INCLUDE_WDB_COMM_END)) && \
     !defined(WDB_END_DEVICE_NAME) && !defined(WDB_END_DEVICE_UNIT))

    BRIDGE_NODE bridgeWdbIf;
#endif
#endif

    /* Parameter check */

    if (pBridgePortList == NULL)
        {
        printf("List of bridge ports is NULL!\n");
        return ERROR;
        }

#ifdef INCLUDE_WDB
#if ((WDB_COMM_TYPE == WDB_COMM_END || defined(INCLUDE_WDB_COMM_END)) && \
     !defined(WDB_END_DEVICE_NAME) && !defined(WDB_END_DEVICE_UNIT))

    /* The WDB END communication requires a standard IP stack attached to the
     * interface to retrive the IP address and answering ARP requests in the
     * task mode although it has its own UDP lite stack to handle the WDB 
     * traffics. With concerned interface joined as bridge port, that IP
     * stack is no longer available. So we need to use the bridge's virtual
     * interface mirror0 as WDB END's new interface if the original network 
     * interface becomes as a bridge port in order to keep WDB END communication
     * going. This also implies the mirror0 should be initialized before WDB. 
     * If the WDB END is intialized before the bridge, it may just work if 
     * the IP address is not changed. In that case, the WDB protocol is the 
     * first SNARF protocol to attach the MUX, therefore will intercept all
     * the traffic bound to that interface first 
     */

    /* find the original designated WDB END interface */

    bzero(bridgeWdbIf.devName,END_NAME_MAX);

    if (bridgeWdbEndEntryFind(&bridgeWdbIf) != OK)
	bridgeWdbIf.unit = NONE;
    else
        {
        /* Found the WDB interface. Do nothing if WDB is already initialized */

        if (taskNameToId("tWdbTask") != ERROR)
            bridgeWdbIf.unit = NONE;
        }
#endif
#endif

    for (pNode = (BRIDGE_NODE*)lstFirst(pBridgePortList); pNode != NULL; 
         pNode = (BRIDGE_NODE*)lstNext((NODE*) pNode)) 
        {
        sprintf(ifName, "%s%d", pNode->devName, pNode->unit);
	oldifname = (char *)ipcom_get_ip_if_name(ifName);

	if (oldifname != IP_NULL)
    	{
	   ipDetach(pNode->unit, pNode->devName);
#ifdef INET6
           ip6Detach(pNode->unit, pNode->devName);
#endif  /* INET6 */
	} 

#ifdef INCLUDE_WDB
#if ((WDB_COMM_TYPE == WDB_COMM_END || defined(INCLUDE_WDB_COMM_END)) && \
     !defined(WDB_END_DEVICE_NAME) && !defined(WDB_END_DEVICE_UNIT))

        /* check if it is the interface used by WDB */

        if ( (strcmp(pNode->devName,bridgeWdbIf.devName) == 0)  &&
             (pNode->unit == bridgeWdbIf.unit) )
            {
            /* yes, one of the bridge port is used by WDB */

            /* The WDB END interface selection algorithm is that it searches
             * the endDevTbl to see if one of them matches the boot device.
             * If does, then the boot device is selected. If doesn't, then the
             * first one in the endDevTbl is selected. The mirror0 is always
             * not in the endDevTbl. In order to make mirror0 as WDB END
             * interface, we use this hack, first save the entire endDevTbl,
             * re-intialize the first entry with mirror0 and mark the second
             * entry as end entry, Then the endDevTbl will be restored back
             * when the "tWdbTask" is created. 
             */

            if (pSavedEndTbl == NULL)
                {
                END_TBL_ENTRY * pDevTbl;
                int count;
 
                for (count = 0, pDevTbl = endDevTbl; 
                         pDevTbl->endLoadFunc != END_TBL_END;
                         pDevTbl++, count++);

                count++;   /* add the end-mark entry */

		pSavedEndTbl =(END_TBL_ENTRY *)malloc(
					   count * sizeof(END_TBL_ENTRY));
		bcopy((char *)endDevTbl,(char *)pSavedEndTbl,
			  count * sizeof(END_TBL_ENTRY));

		savedTblEntries = count;

                endDevTbl[0].unit = MIRROR_STACK_UNIT_NUM;
                endDevTbl[0].endLoadFunc = mirrorEndLoad;
                endDevTbl[0].endLoadString = MIRROR_LOAD_STRING;
                endDevTbl[0].endLoan = FALSE;
                endDevTbl[0].pBSP    = NULL;

                endDevTbl[1].unit = 0;
                endDevTbl[1].endLoadFunc = END_TBL_END;
                endDevTbl[1].endLoadString = NULL;
                endDevTbl[1].endLoan = FALSE;
                endDevTbl[1].pBSP = NULL;
                endDevTbl[1].processed = FALSE;

		taskCreateHookAdd ((FUNCPTR)bridgeEndTblRestore);
                }
                
            }
#endif
#endif

        /* Set the port to promiscuous mode */

        pEnd = endFindByName(pNode->devName,pNode->unit);

        if (pEnd == NULL)
            {
            printf("Can't set device %s%d to promiscuous mode\n",
                   pNode->devName, pNode->unit);
            status = ERROR;
            break;
            } 

        netJobAdd ((FUNCPTR)devicePromiscuousModeSet,(int)pEnd,0,0,0,0);

        /* Add the port to the bridge */

        if (bridgePortAdd (pNode->devName, pNode->unit) != OK)
            {
            printf ("bridgePortAdd failed for device %s%d\n", 
                    pNode->devName, pNode->unit);
            status = ERROR;
            break;
            }

        } /* for */

    return status;
    }

/*******************************************************************************
*
* mirrorInit - Mirror interface initialization
*
* This routine initializes the mirror interface by performing the 
* following tasks:
* 
* 1. Passes the specified device name and unit number to mirrorBaseParamsSet().
*    These values are needed for mirrorEnetAddrGet().
* 2. Loads two mirror END drivers, and attaches one to the network stack and 
*    the other to the bridge.
* 3. Assigns specified IP address and mask to the mirror device 0 (the mirror 
*    attached to the IP stack).
*
* RETURNS:
* OK, or
* ERROR if there is a problem in initializing the mirror interface
*
* NOMANUAL
*/

STATUS mirrorInit
    (
    char*	pDevName,  /* name of a bridge port device */
    int		unit,      /* unit number of a bridge port device */
    char* 	pAddr,     /* IP address of bridge */
    int		mask,      /* netmask of bridge */
    char*	pAddrv6,   /* IPv6 address of bridge */
    int		prefixLen  /* IPv6 prefix length */
    )
    {
    void*	pMuxCookie;         /* cookie returned by muxDevLoad */
    char	mirrorDevName[MIRROR_NAME_MAX]; /* mirror name and number */
    END_OBJ*    pEnd;      /* end object from cookie */
    BOOL        gotAddrv4 = FALSE;  /* IPv4 address specified? */
    BOOL        gotAddrv6 = FALSE;  /* IPv6 address specified? */

    Ip_fd  fd;
    struct Ip_ifreq         ifr;


    /* Parameter check */

    if (pDevName == NULL)
        {
        printf("Name of bridge port device is NULL!\n");
        return ERROR;
        }

    /* Is the bridge address configuration parameter specified? */

    if ((pAddr != NULL) && (pAddr[0] != 0))
        gotAddrv4 = TRUE;
    if ((pAddrv6 != NULL) && (pAddrv6[0] != 0))
        gotAddrv6 = TRUE;

#ifdef INET6
    if (!gotAddrv4 && !gotAddrv6)
        {
        /* In dual mode, neither IPv4 nor IPv6 address was specified */

        printf("IP address of the bridge not specified! Specify IPv4"
	       " and/or IPv6 address -\n");
        printf("Please set the bridge configuration parameters BRIDGE_IP_ADDR"
	       " and/or\nBRIDGE_IPV6_ADDR. For example:\n");
        printf("BRIDGE_IP_ADDR - \"10.11.12.1\""
               " BRIDGE_IPV6_ADDR - \"2002:C000:0240::66\"\n");
        return ERROR;
	}
#else
    if (!gotAddrv4)
        {
	/* In IPv4-only mode, no IPv4 address was specified */

        printf("IP address of the bridge not specified! -\n");
        printf("Please set the bridge configuration parameter BRIDGE_IP_ADDR.\n");
        printf("For example: \"10.11.12.1\"\n");
        return ERROR;
        }
#endif  /* INET6 */

    /*
     * Set the device name and the unit number that will be 
     * used by mirrorEnetAddrGet()
     */ 

    mirrorBaseParamsSet (pDevName, unit);

    sprintf (mirrorDevName, "%s%d", MIRROR_DEV_NAME, MIRROR_STACK_UNIT_NUM);

    /* Load the network stack side of the mirror END driver */

    pMuxCookie = muxDevLoad (MIRROR_STACK_UNIT_NUM,
                             mirrorEndLoad,
                             MIRROR_LOAD_STRING,
                             FALSE,
                             NULL);
    if (pMuxCookie == NULL)
        {
        printf ("muxDevLoad failed for %s\n", mirrorDevName);
        return ERROR;
        }

    if (muxDevStart (pMuxCookie) != OK)
        {
        printf ("muxDevStart failed for %s\n", mirrorDevName);
        muxDevUnload (MIRROR_DEV_NAME, MIRROR_STACK_UNIT_NUM);
        return ERROR;
        }

    /* Attach TCP/IP stack to the mirror END */

#ifdef INET6
    if (gotAddrv4)
        {    
#endif
        if (ipAttach (MIRROR_STACK_UNIT_NUM, MIRROR_DEV_NAME) != OK)
	    {
            printf("ipAttach failed for %s\n", mirrorDevName);
	    muxDevUnload (MIRROR_DEV_NAME, MIRROR_STACK_UNIT_NUM);
	    return ERROR;
	    }

    	fd = ipcom_socket(IP_AF_INET, IP_SOCK_DGRAM, 0);
    	if (fd == IP_INVALID_SOCKET) {
	    printf("Failed to open a socket\n");
	    muxDevUnload (MIRROR_DEV_NAME, MIRROR_STACK_UNIT_NUM);
            return ERROR;
	}

        memset(&ifr, 0, sizeof(struct Ip_ifreq));
    	strcpy(ifr.ifr_name, ipcom_get_ip_if_name(mirrorDevName));


	/* Bring the interface up */
    	if (ipcom_socketioctl(fd, IP_SIOCGIFFLAGS, &ifr) < 0)
    	{
            printf("ifconfig: failed to get interface flags");
	    muxDevUnload (MIRROR_DEV_NAME, MIRROR_STACK_UNIT_NUM);
            return ERROR;
        }

	IP_BIT_SET(ifr.ip_ifr_flags, IP_IFF_UP);
    	if (ipcom_socketioctl(fd, IP_SIOCSIFFLAGS, &ifr) < 0)
    	{
            printf("ifconfig: failed to set interface flags");
	    muxDevUnload (MIRROR_DEV_NAME, MIRROR_STACK_UNIT_NUM);
            return ERROR;
    	}


        ((struct Ip_sockaddr_in *)&ifr.ip_ifr_addr)->sin_family = IP_AF_INET;
        ((struct Ip_sockaddr_in *)&ifr.ip_ifr_addr)->sin_len = sizeof(struct Ip_sockaddr_in);
        ((struct Ip_sockaddr_in *)&ifr.ip_ifr_addr)->sin_addr.s_addr = ipcom_inet_addr(pAddr);

    	/* Set IP address */
    	if (ipcom_socketioctl(fd, IP_SIOCSIFADDR, &ifr) == IP_SOCKERR)
    	{
	    printf("Failed to set address for %s\n", mirrorDevName);
	    muxDevUnload (MIRROR_DEV_NAME, MIRROR_STACK_UNIT_NUM);
            return ERROR;
    	}

        ((struct Ip_sockaddr_in *)&ifr.ip_ifr_addr)->sin_addr.s_addr = (Ip_u32)ip_htonl(mask);

        /* Set subnet mask. */
        if (ipcom_socketioctl(fd, IP_SIOCSIFNETMASK, &ifr) == IP_SOCKERR)
        {
            printf("Failed to set subnet mask for %s, errno %d\n", mirrorDevName, ipcom_errno);
            muxDevUnload (MIRROR_DEV_NAME, MIRROR_STACK_UNIT_NUM);
            return ERROR;
        }

#if 0
        if (ifMaskSet (mirrorDevName, mask)  != OK)
            {
            printf("ifMaskSet failed for %s\n", mirrorDevName);
     	    muxDevUnload (MIRROR_DEV_NAME, MIRROR_STACK_UNIT_NUM);
	    return ERROR;
            }
        
        if (ifAddrSet (mirrorDevName, pAddr) != OK)
            {
	    printf("ifAddrSet failed for %s\n", mirrorDevName);
	    muxDevUnload (MIRROR_DEV_NAME, MIRROR_STACK_UNIT_NUM);
	    return ERROR;
	    }
#endif

#ifdef INET6
        }

    if (gotAddrv6)
        {
	struct Ip_in6_addr inaddr6;
	struct Ip_in6_aliasreq ifareq6;
    	int i;

	if (ip6Attach (MIRROR_STACK_UNIT_NUM, MIRROR_DEV_NAME) != OK)
	    {
            printf("ip6Attach failed for %s\n", mirrorDevName);
	    muxDevUnload (MIRROR_DEV_NAME, MIRROR_STACK_UNIT_NUM);
	    return ERROR;
	    }

        fd = ipcom_socket(IP_AF_INET6, IP_SOCK_DGRAM, 0);
        if (fd == IP_INVALID_SOCKET) {
            printf("Failed to open a socket\n");
            muxDevUnload (MIRROR_DEV_NAME, MIRROR_STACK_UNIT_NUM);
            return ERROR;
        }

        memset(&ifareq6, 0, sizeof(struct Ip_in6_aliasreq));
        strcpy(ifareq6.ifra_name, ipcom_get_ip_if_name(mirrorDevName));

        if (ipcom_inet_pton(IP_AF_INET6, pAddrv6, &inaddr6) <= 0) {
	    printf("Failed to convert an IPv6 address\n");
	    muxDevUnload (MIRROR_DEV_NAME, MIRROR_STACK_UNIT_NUM);
            return ERROR;
	}

	ifareq6.ifra_addr.sin6_addr = inaddr6;
	ifareq6.ifra_addr.sin6_family = IP_AF_INET6;
	ifareq6.ifra_addr.sin6_len = sizeof(struct Ip_sockaddr_in6);

    	if (IP_IN6_IS_ADDR_LINK_LOCAL(&inaddr6))
        	ifareq6.ifra_addr.sin6_scope_id = ipcom_if_nametoindex(ifareq6.ifra_name);

    	ifareq6.ifra_prefixmask.sin6_family = IP_AF_INET6;
    	ifareq6.ifra_prefixmask.sin6_len = sizeof(struct Ip_sockaddr_in6);

    	for (i = 0; i < prefixLen; i++)
    	{
            IP_BIT_SET(((Ip_u16*) &ifareq6.ifra_prefixmask.sin6_addr)[i / 16], ip_htons(0x8000 >> (i % 16)));
    	}

    	/* Set lifetimes */
    	ifareq6.ifra_lifetime.ia6t_preferred   = (long)IPCOM_ADDR_INFINITE;
    	ifareq6.ifra_lifetime.ia6t_expire      = (long)IPCOM_ADDR_INFINITE;

    	/* Sanity */
    	ifareq6.ifra_flags             =   IP_IN6_IFF_TENTATIVE;

        /* Add IPv6 address */
        if (ipcom_socketioctl(fd, IP_SIOCAIFADDR_IN6, &ifareq6) == IP_SOCKERR)
        {
            printf("Failed to add IPv6 address for %s\n", mirrorDevName);
            muxDevUnload (MIRROR_DEV_NAME, MIRROR_STACK_UNIT_NUM);
            return ERROR;
        }

	
#if 0
        if (ifFlagChange (mirrorDevName, IFF_INET6_UP, TRUE) != OK)
            {
            printf("ifFlagChange (IFF_INET6_UP) failed for %s\n",
                   mirrorDevName);
            muxDevUnload (MIRROR_DEV_NAME, MIRROR_STACK_UNIT_NUM);
            return ERROR;
            }

	if (if6AddrAdd (mirrorDevName, pAddrv6, prefixLen, 0) != OK)
	    {
            printf("if6AddrAdd failed for %s\n", mirrorDevName);
	    muxDevUnload (MIRROR_DEV_NAME, MIRROR_STACK_UNIT_NUM);
	    return ERROR;
	    }
#endif
        }
#endif  /* INET6 */

    /* Load the bridge side of the mirror END driver */

    pMuxCookie = muxDevLoad (MIRROR_BRIDGE_UNIT_NUM,
                             mirrorEndLoad,
                             MIRROR_LOAD_STRING,
                             FALSE,
                             NULL);
    if (pMuxCookie == NULL)
        {
        printf("muxDevLoad failed for %s%d\n",
               MIRROR_DEV_NAME, MIRROR_BRIDGE_UNIT_NUM);
        return ERROR;
        }
    
    if (muxDevStart(pMuxCookie) != OK)
        {
        muxDevUnload (MIRROR_DEV_NAME, MIRROR_BRIDGE_UNIT_NUM);
        printf ("muxDevStart failed for %s%d\n",
                MIRROR_DEV_NAME, MIRROR_BRIDGE_UNIT_NUM);
        return ERROR;
        }

    /* Set the bridge side driver to promiscuous mode */

    pEnd = PDEVCOOKIE_TO_ENDOBJ(pMuxCookie);
    devicePromiscuousModeSet(pEnd);

    /* Add this mirror port to the bridge */

    if (bridgePortAdd (MIRROR_DEV_NAME, MIRROR_BRIDGE_UNIT_NUM) != OK)
        {
        printf ("bridgePortAdd failed for %s%d\n",
                MIRROR_DEV_NAME, MIRROR_BRIDGE_UNIT_NUM);
        muxDevStop (pMuxCookie);
        muxDevUnload (MIRROR_DEV_NAME, MIRROR_BRIDGE_UNIT_NUM);
        return ERROR;
        }

    return OK;
    }

/*******************************************************************************
*
* promiscuousModeSet - Change mode to promiscuous
*
* This routine sets the specified interface to promiscuous mode using the
* the driver's ioctl call.
*
* RETURNS: OK or ERROR if fail
*
* NOMANUAL
*/

STATUS promiscuousModeSet 
    (
    char * pEndName,  /* name of device */
    int    unit       /* unit number of device */
    )
    {
    END_OBJ * pEnd;

    pEnd = endFindByName(pEndName,unit);

    if (pEnd == NULL)
        {
        printf("%s%d: Device does not exist or its driver is not loaded\n",
               pEndName, unit);
        return ERROR;
        }
    
    devicePromiscuousModeSet(pEnd);

    return OK;
    }
 

/*****************************************************************************
*
* devicePromiscuousModeSet - Turn on hardware device's promiscuous mode
*
*/
    
LOCAL void devicePromiscuousModeSet
    ( 
    END_OBJ * pEnd       /* end object */
    )    
    {
    int ifFlags = 0;

    /* get the current interface flag settings */

    if (pEnd->pFuncTable->ioctl != NULL)
        pEnd->pFuncTable->ioctl (pEnd, (int)EIOCGFLAGS, (void *)&ifFlags);

    /* add the flag we want to set */

    ifFlags |= IP_IFF_PROMISC;

    /* add the IP_IFF_UP flag anyway, the ipDetach clears this flag. Mostly like
     * the driver won't perform the actual action of setting the device to the
     * promiscuous mode if this flag is not set.
     */

    ifFlags |= IP_IFF_UP;


    if (pEnd->pFuncTable->ioctl != NULL)
        pEnd->pFuncTable->ioctl (pEnd, (int)EIOCSFLAGS, (void *)ifFlags);
    }

#ifdef INCLUDE_WDB
#if ((WDB_COMM_TYPE == WDB_COMM_END || defined(INCLUDE_WDB_COMM_END)) && \
     !defined(WDB_END_DEVICE_NAME) && !defined(WDB_END_DEVICE_UNIT))


/*****************************************************************************
*
* bridgeWdbEndEntryFind - Find the WDB END interface 
*
*/

LOCAL STATUS bridgeWdbEndEntryFind
    (
    BRIDGE_NODE * pGetEntry
    )
    {
    END_TBL_ENTRY * pDevTbl;
    char devName[END_NAME_MAX + 1];

    for (pDevTbl = endDevTbl; pDevTbl->endLoadFunc != NULL; pDevTbl++)
        {
        /* get the name of the device by passing argument devName = '\0' */

        bzero (devName, END_NAME_MAX + 1);
        if (pDevTbl->endLoadFunc(devName, NULL) != 0)
            return (ERROR);

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
            return (ERROR);
        else
            pDevTbl = endDevTbl;
        }

    pGetEntry->unit          = pDevTbl->unit;
    pDevTbl->endLoadFunc(pGetEntry->devName, NULL);
    return OK;
    }

/*****************************************************************************
*
* bridgeEndTblRestore - Restore the endDevTbl when WDB task is created
*
*/

LOCAL void bridgeEndTblRestore 
    (
    WIND_TCB * pNewTcb
    )
    {
    /* restore back the endTbl entry */

#if (defined(_WRS_VXWORKS_MAJOR) && (_WRS_VXWORKS_MAJOR >= 6)) /* VxW 6.x */
    if (strcmp(pNewTcb->objCore.name,"tWdbTask") == 0)
#else
    if (strcmp(pNewTcb->name,"tWdbTask") == 0)
#endif
	{
	/* the WDB task has been created, restore back the endTbl */

        bcopy((char *)pSavedEndTbl, (char *)endDevTbl,
              savedTblEntries * sizeof(END_TBL_ENTRY));

	free((void *)pSavedEndTbl);
	taskCreateHookDelete((FUNCPTR)bridgeEndTblRestore);
        }
    }

#endif
#endif


