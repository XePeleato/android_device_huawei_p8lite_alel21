/* usrNetM2SysLib.c - Initialization routine for the MIB2_SYSTEM library */

/* Copyright 1992 - 2004 Wind River Systems, Inc. */

/*
modification history
--------------------
01j,12apr02,zhr  Migrated to ipnet
01i,29sep04,vvv  fixed typo, compiler warnings (SPR #102073)
01h,19sep04,spm  updated virtual stack startup: restored use of published
                 init routine and removed packing/unpacking parameters
01g,22jun04,niq  Fix bug in passing a stack variable by reference
01f,28apr04,vvv  fixed object id configuration (SPR #94509)
01e,06nov03,rlm  Ran batch header update for header re-org.
01d,26nov02,ism  changed instance initialisation function
01c,04sep02,kal  use bzero()
01b,29aug02,ant  added m2SysDestructor()
01a,28aug02,ant  written
*/

/*
DESCRIPTION
This configlette contains the initialization routine for the
INCLUDE_MIB2_SYSTEM component. 

NOMANUAL
*/

#include <m2Lib.h>

/* externs */

#ifdef VIRTUAL_STACK
VS_REG_ID m2SysRegistrationNum;
extern STATUS m2SysDestructor (VSNUM vsnum);
#endif /* VIRTUAL_STACK */

LOCAL STATUS usrNetM2SysInit (void);

LOCAL STATUS usrNetM2SysStart (void)
    {
#ifdef VIRTUAL_STACK
    /*
     * Register with vsmanager to allow the MIB2 System group
     * to start in every virtual stack at the appropriate time.
     */

    vsComponentRegister (&m2SysRegistrationNum, "MIB2_SYS",
                         VS_SC_ENABLED, VS_SC_NON_OPTIONAL,
                         usrNetM2SysInit, 0, NULL, m2SysDestructor);
#else
    /*
     * When only one network stack exists, just
     * start the MIB2 System group immediately.
     */

    usrNetM2SysInit ();

#endif /* VIRTUAL_STACK */

    return (OK);
    }

LOCAL STATUS usrNetM2SysInit (void)
    {
    static M2_OBJECTID sysObjId = { MIB2SYS_OID_LEN, MIB2SYS_OID };
    return (m2SysInit (MIB2SYS_DESCR, MIB2SYS_CONTACT, MIB2SYS_LOCATION,
                       &sysObjId)); 
    }
