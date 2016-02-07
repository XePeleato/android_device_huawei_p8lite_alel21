/* usrDot1xAuthCfg.c - Initialize 802.1X authenticator */

/* Copyright 2002-2003 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
-------------------
02j,04sep09,hfc  fix for WIND00179065
02i,06aug08,dwb  Fix for defect WIND00128956.  Fix Record WIND00130183.
02h,04dec06,dwb  Cleanup from the ipradius port
02g,31oct06,dwb  Removing call to wr sntp library
02f,04apr06,rb  Fix for SPR 116532 - Legacy 802.1X does not work with unicast
                 keys
02e,26sep05,eja  remove MIB binding if MIB support not required
02d,23sep05,eja  Break SNMP dependency
02c,31may05,eja  Fix for B0399
02b,30may05,eja  change config elements to reflect updated header file
02a,20may05,eja  Fix compiler warnings
01z,13may05,eja  Remove dependency on wlanConfig()
01y,12may05,rb  Added support for 104 bit WEP keys
01x,10may05,eja  Remove depenedencies on eapol lib, mempart lib, and mib lib
01w,05may05,eja  Fix BO304 - diab warnings
01v,03mar05,eja  Fix EAPOL call to reflect changed API
01u,17feb05,eja  Add memPartLinInit call to set up memory partition
01t,04feb05,eja  Added 80211i/WPA integration functionality
01s,31jan05,eja  Change Macro names
01r,17jan05,eja  Make changes to support wireline mode
01q,15oct04,dxb  Added check for empty wireless I/F parameter
01p,01oct04,eja  Change default values to support Atheros driver
01o,07aug04,rb   Added support for "Driver Type" parameter to ensure 
                 operation with 2.0 wireless driver
01n,11sep03,ggg  Correct output, add '\0' to NULL as bridge disabling
01m,10sep03,ggg  Allow NULL as wired interface name
01l,09sep03,ggg  Add default macro defs for command line builds
01k,21aug03,ggg  Updated routine name
01j,08aug03,ggg  Remove RADIUS client parameters and initialization
01i,07may03,ss   renamed params
01h,21apr03,ggg  File name, location change
01g,21feb03,ggg  Update copyright dates
01f,22jan03,ggg  Add key slot initializations for more new usage
01e,25jun02,ggg  Add callback install
01d,18jun02,ggg  Reduce number of arguments to usrDot1xInit routine
01c,11jun02,ggg  Remove use of subENDs
01b,11jun02,ggg  dot1xEapolInit parameter addition
01a,02jun02,ggg  Written
*/

/*
This module provides initialization of 802.1X authenticator.
*/

#include <vxWorks.h>
#include <stdio.h>
#include "wrn/dot1x/dot1xAuthLib.h"
#include "wrn/dot1x/dot1xAuthSvrLib.h"
#include "wrn/dot1x/dot1xMibLib.h"
#include "wrn/dot1x/dot1xEapolLib.h"
#include "wrn/dot1x/dot1xMemPartLib.h"
#include "wrn/dot1x/dot1xDebugLib.h"

#include <../../ipsntp/config/ipsntp_config.h>
#include <../../ipsntp/include/ipsntp.h>

/* Make sure we have at least one type of network support */
#if !defined(IPCOM_USE_INET) && !defined(IPCOM_USE_INET6)
#error "Must define at least IPCOM_USE_INET or IPCOM_USE_INET6"
#endif

/* Default debug levels */
#ifndef DOT1X_DEBUG_AREA
#define DOT1X_DEBUG_AREA DOT1X_AREA_ALL
#endif

#define DOT1X_AUTH_MODE_WIRELESS 1
#define DOT1X_AUTH_MODE_WIRELINE 2

/* Defined so non-project command line builds are OK */
#ifndef DOT1X_AUTH_UNCONTROLLED_INTERFACE_NAME
#define DOT1X_AUTH_UNCONTROLLED_INTERFACE_NAME "motfcc"
#endif /* DOT1X_AUTH_UNCONTROLLED_INTERFACE_NAME */

#ifndef DOT1X_AUTH_UNCONTROLLED_INTERFACE_NUM
#define DOT1X_AUTH_UNCONTROLLED_INTERFACE_NUM 0
#endif /* DOT1X_AUTH_UNCONTROLLED_INTERFACE_NUM */

/* Defined so non-project command line builds are OK */
#ifndef DOT1X_AUTH_CONTROLLED_INTERFACE_NAME
#define DOT1X_AUTH_CONTROLLED_INTERFACE_NAME "wls"
#endif /* DOT1X_AUTH_CONTROLLED_INTERFACE_NAME */

#ifndef DOT1X_AUTH_CONTROLLED_INTERFACE_NUM
#define DOT1X_AUTH_CONTROLLED_INTERFACE_NUM 0
#endif /* DOT1X_AUTH_WIRED_DOT1X_INTERFACE_NUM */

#ifndef DOT1X_AUTH_OPERATING_MODE
#define DOT1X_AUTH_OPERATING_MODE DOT1X_AUTH_MODE_WIRELESS
#endif /* DOT1X_AUTH_OPERATING_MODE */

#ifndef DOT1X_AUTH_LEGACY_WEP_TYPE
#define DOT1X_AUTH_LEGACY_WEP_TYPE 1
#endif /* DOT1X_AUTH_OPERATING_MODE */

#ifndef DOT1X_AUTH_PRIMARY_RADIUS_SERVER_DOMAIN
#define DOT1X_AUTH_PRIMARY_RADIUS_SERVER_DOMAIN 2
#endif /* DOT1X_AUTH_PRIMARY_RADIUS_SERVER_DOMAIN */ 

#ifndef DOT1X_AUTH_PRIMARY_RADIUS_SERVER_IPV4_ADDRESS
#define DOT1X_AUTH_PRIMARY_RADIUS_SERVER_IPV4_ADDRESS "10.1.1.1"
#endif  /* DOT1X_AUTH_PRIMARY_RADIUS_SERVER_IPV4_ADDRESS */

#ifndef DOT1X_AUTH_PRIMARY_RADIUS_SERVER_IPV6_ADDRESS
#define DOT1X_AUTH_PRIMARY_RADIUS_SERVER_IPV6_ADDRESS "3ffe:300::5"
#endif /*  DOT1X_AUTH_PRIMARY_RADIUS_SERVER_IPV6_ADDRESS */

#ifndef DOT1X_AUTH_PRIMARY_RADIUS_SERVER_SHARED_SECRET
#define DOT1X_AUTH_PRIMARY_RADIUS_SERVER_SHARED_SECRET "secret"
#endif /* DOT1X_AUTH_PRIMARY_RADIUS_SERVER_SHARED_SECRET */

#ifndef DOT1X_AUTH_PRIMARY_RADIUS_SERVER_PORT
#define DOT1X_AUTH_PRIMARY_RADIUS_SERVER_PORT 1812
#endif /* DOT1X_AUTH_PRIMARY_RADIUS_SERVER_PORT */

#ifndef DOT1X_AUTH_PRIMARY_RADIUS_RETRY_COUNT
#define DOT1X_AUTH_PRIMARY_RADIUS_RETRY_COUNT 1
#endif /* DOT1X_AUTH_PRIMARY_RADIUS_RETRY_COUNT */

#ifndef DOT1X_AUTH_PRIMARY_RADIUS_TIMEOUT
#define DOT1X_AUTH_PRIMARY_RADIUS_TIMEOUT 10 
#endif /* DOT1X_AUTH_PRIMARY_RADIUS_TIMEOUT */

#ifndef DOT1X_AUTH_SECONDARY_RADIUS_SERVER_DOMAIN
#define DOT1X_AUTH_SECONDARY_RADIUS_SERVER_DOMAIN 2
#endif /* DOT1X_AUTH_SECONDARY_RADIUS_SERVER_DOMAIN */ 

#ifndef DOT1X_AUTH_SECONDARY_RADIUS_SERVER_IPV4_ADDRESS
#define DOT1X_AUTH_SECONDARY_RADIUS_SERVER_IPV4_ADDRESS "10.1.2.1"
#endif  /* DOT1X_AUTH_SECONDARY_RADIUS_SERVER_IPV4_ADDRESS */

#ifndef DOT1X_AUTH_SECONDARY_RADIUS_SERVER_IPV6_ADDRESS
#define DOT1X_AUTH_SECONDARY_RADIUS_SERVER_IPV6_ADDRESS "3ffe:300::5"
#endif /*  DOT1X_AUTH_SECONDARY_RADIUS_SERVER_IPV6_ADDRESS */

#ifndef DOT1X_AUTH_SECONDARY_RADIUS_SERVER_SHARED_SECRET
#define DOT1X_AUTH_SECONDARY_RADIUS_SERVER_SHARED_SECRET "kallekula123"
#endif /* DOT1X_AUTH_SECONDARY_RADIUS_SERVER_SHARE_SECRET */

#ifndef DOT1X_AUTH_SECONDARY_RADIUS_SERVER_PORT
#define DOT1X_AUTH_SECONDARY_RADIUS_SERVER_PORT 1812
#endif /* DOT1X_AUTH_SECONDARY_RADIUS_SERVER_PORT */

#ifndef DOT1X_AUTH_SECONDARY_RADIUS_RETRY_COUNT
#define DOT1X_AUTH_SECONDARY_RADIUS_RETRY_COUNT 3
#endif /* DOT1X_AUTH_SECONDARY_RADIUS_RETRY_COUNT */

#ifndef DOT1X_AUTH_SECONDARY_RADIUS_TIMEOUT
#define DOT1X_AUTH_SECONDARY_RADIUS_TIMEOUT 3
#endif /* DOT1X_AUTH_SECONDARY_RADIUS_TIMEOUT */

/* Define the number of servers */
#ifdef INCLUDE_AUTHENTICATOR_SECONDARY_RADIUS_SERVER
#define DOT1X_AUTH_NUM_SERVERS 2
#else
#define DOT1X_AUTH_NUM_SERVERS 1
#endif

/* RADIUS Server Info */
LOCAL DOT1X_AUTH_RADIUS_SERVER_INFO dot1xAuthServerInfo[DOT1X_AUTH_NUM_SERVERS];

/* Public function */
STATUS usrDot1xAuthInit(VOID); 
STATUS usrDot1xAuthCfg
    (
    INT8 * networkPortIfName, /* network port interface name */
    INT32  networkPortUnitNumber, /* network port unit number */
    INT8 * dot1xPortIfName, /* 802.1X port interface name */
    INT32  dot1xPortUnitNumber, /* 802.1X port unit number */
    INT32  mode /* Mode of operation (i.e. wireline or wlan) */
    );

/* Mib module binding */
#ifdef INCLUDE_MIB_CONFIG
LOCAL DOT1X_MIB_MODULE_BINDING dot1xAuthMibModule;
#endif
LOCAL DOT1X_MIB_MODULE_BINDING * pDot1xAuthMibModule = NULL;



/*****************************************************************************
* usrDot1xAuthCfg - start up the 802.1X authenticator
*
* This routine calls the autheticator configuration function with the
* above defined parameters. 
*
* RETURNS: Status of the subsequent call to the callee
*/
STATUS usrDot1xAuthInit(VOID) 
    {

    /* Configure the authenticator */
    return(usrDot1xAuthCfg((INT8*)DOT1X_AUTH_UNCONTROLLED_INTERFACE_NAME,
                           (INT32)DOT1X_AUTH_UNCONTROLLED_INTERFACE_NUM,
                           (INT8*)DOT1X_AUTH_CONTROLLED_INTERFACE_NAME,
                           (INT32)DOT1X_AUTH_CONTROLLED_INTERFACE_NUM,
                           (INT32)DOT1X_AUTH_OPERATING_MODE)); 
    }

/*****************************************************************************
* usrDot1xAuthCfg - start up the 802.1X authenticator and all dependent 
* components. 
*
* This routine sets up the basic runtime configuration parameters and brings
* up all of the components on which it depends in order to start up the 
* 802.1X autheticator. If and error condition occurs during this call then
* a message indicating which error condition happened to the console and 
* then aborts the call returning ERROR to the callee.   
*
* RETURNS: OK or ERROR
*/
STATUS usrDot1xAuthCfg
    (
    INT8 *networkPortIfName, /* network port interface name */
    INT32 networkPortUnitNumber, /* network port unit number */
    INT8 *dot1xPortIfName, /* 802.1X port interface name */
    INT32 dot1xPortUnitNumber, /* 802.1X port unit number */
    INT32 mode /* Operating mode (i.e. wireline or wlan) */
    )
    {
    DOT1X_AUTH_SYS_CONFIG authConfig; /* Authenticator config structure */
    DOT1X_AUTH_IF_CONFIG ifConfig; /* Controlled port configuration */
    INT8 networkPortName[20]; /* network port interface name */
    INT8 dot1xPortName[20]; /* 802.1X port interface name */

#ifdef INCLUDE_MIB_CONFIG
    dot1xAuthMibModule.pInstallHandler = (void*)dot1xMibHandlerInstall;
    dot1xAuthMibModule.pRemoveHandler = (void*)dot1xMibHandlerRemove;
    pDot1xAuthMibModule = &dot1xAuthMibModule;
#endif

    /* Get current network port IP address, based on the interface
    name and unit number */
    (VOID)sprintf((char*)networkPortName, 
                  "%s%d", 
                  (int)networkPortIfName, 
                  networkPortUnitNumber);

    /* Get current 802.1X port IP address, based on the interface
    name and unit number */
    (VOID)sprintf((char*)dot1xPortName, 
                  "%s%d", 
                  (int)dot1xPortIfName, 
                  dot1xPortUnitNumber);

    /* Display some of the Show what is happening */
    printf("\nInitializing 802.1X-enabled mode\n");
    printf("Network Port Interface name :%s\n", networkPortName);
    printf("802.1X Port Interface name  :%s\n", dot1xPortName);
    printf("Bridging between            :%s and %s\n\n", 
           networkPortName, dot1xPortName);

    /* Create Framework configuration */
    authConfig.taskPriority = 80;
    authConfig.nbrOfPorts = 10;
    authConfig.eapolAsfAlert = NULL;
    authConfig.replayCntType = 1;
    authConfig.ntpExecMode = 0;
    authConfig.ntpSvrIpAddr = NULL;
    authConfig.ntpTimeGetFunc = NULL;
    authConfig.pMib = pDot1xAuthMibModule;

    /* Populate the primary server info */
    dot1xAuthServerInfo[0].domain = DOT1X_AUTH_PRIMARY_RADIUS_SERVER_DOMAIN;
    dot1xAuthServerInfo[0].port = DOT1X_AUTH_PRIMARY_RADIUS_SERVER_PORT;
#ifdef IPCOM_USE_INET
    if(dot1xAuthServerInfo[0].domain == IP_AF_INET)
    {
        if((dot1xAuthServerInfo[0].server_addr = (char *)
                malloc(strlen(DOT1X_AUTH_PRIMARY_RADIUS_SERVER_IPV4_ADDRESS))) == NULL)
        {
            printf("usrDot1xAuthCfg - Error initializing authentication server" 
                    " library, malloc failed\n");
            return(ERROR);
        }
        strcpy(dot1xAuthServerInfo[0].server_addr,
                DOT1X_AUTH_PRIMARY_RADIUS_SERVER_IPV4_ADDRESS);
    }
#endif
#ifdef IPCOM_USE_INET6
    else if(dot1xAuthServerInfo[0].domain == IP_AF_INET6)
    {
        if((dot1xAuthServerInfo[0].server_addr = (char *)
                malloc(strlen(DOT1X_AUTH_PRIMARY_RADIUS_SERVER_IPV6_ADDRESS))) == NULL)
        {
            printf("usrDot1xAuthCfg - Error initializing authentication server" 
                    " library, malloc failed\n");
            return(ERROR);
        }
        strcpy(dot1xAuthServerInfo[0].server_addr,
                DOT1X_AUTH_PRIMARY_RADIUS_SERVER_IPV6_ADDRESS);
    }
#endif
    else
        printf("usrDot1xAuthCfg - Error initializing authentication server" 
                " library, invalid domain %d\n",dot1xAuthServerInfo[0].domain);

    if((dot1xAuthServerInfo[0].shared_secret = (char *)
            malloc(strlen(DOT1X_AUTH_PRIMARY_RADIUS_SERVER_SHARED_SECRET)))
            == NULL)
    {
        printf("usrDot1xAuthCfg - Error initializing authentication server" 
               " library, malloc failed\n");
        return(ERROR);
    }
    strcpy(dot1xAuthServerInfo[0].shared_secret,
            DOT1X_AUTH_PRIMARY_RADIUS_SERVER_SHARED_SECRET);
    dot1xAuthServerInfo[0].retry_count = DOT1X_AUTH_PRIMARY_RADIUS_RETRY_COUNT;
    dot1xAuthServerInfo[0].timeout = DOT1X_AUTH_PRIMARY_RADIUS_TIMEOUT;

#ifdef INCLUDE_AUTHENTICATOR_SECONDARY_RADIUS_SERVER
    dot1xAuthServerInfo[0].endTag = DOT1X_AUTH_TAG_MORE_SERVERS;
#else
    dot1xAuthServerInfo[0].endTag = DOT1X_AUTH_TAG_LAST_SERVER;
#endif

#ifdef INCLUDE_AUTHENTICATOR_SECONDARY_RADIUS_SERVER
    /* Populate the secondary server info */
    dot1xAuthServerInfo[1].domain = DOT1X_AUTH_SECONDARY_RADIUS_SERVER_DOMAIN;
    dot1xAuthServerInfo[1].port = DOT1X_AUTH_SECONDARY_RADIUS_SERVER_PORT;
#ifdef IPCOM_USE_INET
    if(dot1xAuthServerInfo[1].domain == IP_AF_INET)
    {
        dot1xAuthServerInfo[1].server_addr = (char *)
            malloc(strlen(DOT1X_AUTH_SECONDARY_RADIUS_SERVER_IPV4_ADDRESS));
        strcpy(dot1xAuthServerInfo[1].server_addr,
                DOT1X_AUTH_SECONDARY_RADIUS_SERVER_IPV4_ADDRESS);
    }
#endif
#ifdef IPCOM_USE_INET6
    else if(dot1xAuthServerInfo[1].domain == IP_AF_INET6)
    {
        dot1xAuthServerInfo[1].server_addr = (char *)
            malloc(strlen(DOT1X_AUTH_SECONDARY_RADIUS_SERVER_IPV6_ADDRESS));
        strcpy(dot1xAuthServerInfo[1].server_addr,
                DOT1X_AUTH_SECONDARY_RADIUS_SERVER_IPV6_ADDRESS);
    }
#endif
    else
        printf("usrDot1xAuthCfg - Error initializing authentication server" 
                " library, invalid domain %d\n",dot1xAuthServerInfo[0].domain);

    dot1xAuthServerInfo[1].shared_secret = (char *)
            malloc(strlen(DOT1X_AUTH_SECONDARY_RADIUS_SERVER_SHARED_SECRET));
            strcpy(dot1xAuthServerInfo[1].shared_secret,
                    DOT1X_AUTH_SECONDARY_RADIUS_SERVER_SHARED_SECRET);
    dot1xAuthServerInfo[1].retry_count = DOT1X_AUTH_SECONDARY_RADIUS_RETRY_COUNT;
    dot1xAuthServerInfo[1].timeout = DOT1X_AUTH_SECONDARY_RADIUS_TIMEOUT;
    dot1xAuthServerInfo[1].endTag = DOT1X_AUTH_TAG_LAST_SERVER;
#endif
    
    /* Initialize the authentication server library */
    if(dot1xAuthServerLibInit(dot1xAuthServerInfo) == ERROR)
    {
        printf("usrDot1xAuthCfg - Error initializing authentication server" 
               " library, dot1xAuthServerLibInit failed\n");
        return(ERROR);
     }

    /* Initialize the backend authentication library */
    if(dot1xAuthInit(&authConfig) == ERROR)
        {
        printf("usrDot1xAuthCfg - Error initializing authentication"
               " library\n");
        return(ERROR);
        }

    /* Create interface configuration */
    switch(mode)
        {
        case DOT1X_AUTH_MODE_WIRELINE:
            ifConfig.type = DOT1X_AUTH_WIRED_IF;
            break;
        case DOT1X_AUTH_MODE_WIRELESS:
        default:
            ifConfig.type = DOT1X_AUTH_WIRELESS_IF;
            ifConfig.wepKeyType = DOT1X_AUTH_LEGACY_WEP_TYPE;
            break;
        }

    /* Standard configuration */
    ifConfig.operationType = DOT1X_AUTH_WIRELESS; 
    ifConfig.inactivityTimeout = 600;
    ifConfig.connectivityTimeout = 300;
    ifConfig.mcastKeyPeriod = 300;
    ifConfig.ucastKeySupportType = DOT1X_AUTH_REAL_UCAST_KEY; 
    ifConfig.ucastKeyIsSessionKey = FALSE; 
    ifConfig.staticWepKeySupport = FALSE;
    ifConfig.staticWepKey = NULL;
    ifConfig.ucastKeyPeriod = 300;
    ifConfig.ucastKeySlots = (1<<0);
    ifConfig.mcastKeySlots = (1<<3) | (1<<2) | (1<<1);

    /* Attach the framework to the 802.1X interface (i.e. controlled port) */
    if(dot1xAuthAttach((INT32)dot1xPortUnitNumber, (UCHAR*)dot1xPortIfName, 
                       (DOT1X_AUTH_IF_CONFIG*)&ifConfig) == ERROR)
        {
        printf("usrDot1xAuthCfg - Error attaching to 802.1X authenticator\n");
        return(ERROR);
        }

    /* Success */
    return(OK);
    }

