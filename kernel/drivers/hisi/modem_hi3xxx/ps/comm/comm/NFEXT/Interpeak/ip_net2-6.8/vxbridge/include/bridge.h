/* bridge.h - simple learning bridge declarations */

/* Copyright 2000-2005 Wind River Systems, Inc. */

/*
modification history
--------------------
01h,21nov07,tlu  Move BRIDGE_PORT_INFO structure here
01g,07sep06,tlu  Increment version number
01f,21jul05,svk  Increment version number for Service Pack 1
01e,07feb05,svk  Increment version number
01d,19jul04,svk  Increment version number
01c,07may03,svk  Add support for static mac addresses in bridge station cache
01b,17apr03,svk  Implement version number, update copyright
01a,27sep00,jhl  created
*/


#ifndef __INCbridgeh
#define __INCbridgeh


#ifdef INCLUDE_IPFIREWALL_SUPPORT
#include <ipnet_netif.h>
#endif

#ifdef  __cplusplus
extern "C" {
#endif

#define BRIDGE_VERSION	"1.3.3"

#define BRIDGE_STATIC_FLAG  0x00000001  /* Station cache entry is static */

/* bridge port information */
typedef struct
    {
    NODE        node;               /* must be first member in the structure */
    char        name[END_NAME_MAX];
    int         unitNum;
#ifdef INCLUDE_IPFIREWALL_SUPPORT
    Ipnet_netif *netif;
#endif
    void*       pMuxBindCookie;
    char        portType;
    struct ifqueue errSndq;
    } BRIDGE_PORT_INFO;


IMPORT const char * bridgeVersion;

IMPORT STATUS   bridgeInit(void);
IMPORT STATUS   bridgePortAdd(char* pDevName, int unitNum);
IMPORT STATUS   bridgePortRemove(char* pDevName, int unitNum);
IMPORT STATUS   bridgeMacAdd(char* pDevName, int unitNum, char* pMacAddr,
			     UINT32 flags);
IMPORT STATUS   bridgeMacRemove(char* pMacAddr);

#ifdef  __cplusplus
}
#endif

#endif  /* __INCbridgeh */


/***************************************************************************
 * END OF FILE
 ***************************************************************************/

