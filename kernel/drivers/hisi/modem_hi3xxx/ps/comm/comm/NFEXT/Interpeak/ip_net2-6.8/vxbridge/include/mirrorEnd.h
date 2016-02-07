/* mirrorEnd.h - Mirroring END device driver declarations */

/* Copyright 2000-2003 Wind River Systems, Inc. */

/*
modification history
--------------------
01c,21nov07,tlu  Add function proto to set interface RUNNING state
01b,17apr03,svk  Update copyright
01a,10oct00,jhl  created
*/


#ifndef __INCmirrorEndh
#define __INCmirrorEndh


#ifdef  __cplusplus
extern "C" {
#endif

IMPORT void mirrorEnetAddrGet (char *);
IMPORT STATUS mirrorEndRunningState(void);


/* defines */

#define     MIRROR_END_OBJ_STRING   "Mirroring Enhanced Network Driver"
#define     MIRROR_DEV_NAME         "mirror"
#define     MIRROR_DEV_NAME_LEN     7

#define     MIRROR_STACK_UNIT_NUM        0
#define     MIRROR_BRIDGE_UNIT_NUM       1

#define     ENDERRBLOCK       2000  /* for END_ERR_BLOCK */

#define     MIRROR_END_ERR_UNBLOCK         0x666
#define     MIRROR_END_ERR_BLOCK           0x888
#define     MIRROR_END_ERR_BLOCK_CHECK     0x999


#define     MIRROR_END_ERR_BLOCK_TRUE       100 
#define     MIRROR_END_ERR_BLOCK_FALSE      200 

#define SYS_ENET_ADDR_GET(address) mirrorEnetAddrGet (address);

#ifdef  __cplusplus
}
#endif


#endif  /* __INCmirrorEndh */


/***************************************************************************
 * END OF FILE
 ***************************************************************************/

