/* usrNetM2IpLib.c - Initialization routine for the MIB2_IP library */

/* Copyright 1992 - 2004 Wind River Systems, Inc. */

/*
modification history
--------------------
01h,12apr02,zhr  Migrated to ipnet
01g,20oct04,vvv  fixed warning (SPR #102122)
01f,19sep04,spm  updated virtual stack startup: restored use of published
                 init routine and removed packing/unpacking parameters
01e,06nov03,rlm  Ran batch header update for header re-org.
01d,26nov02,ism  changed instance initialisation function
01c,04sep02,kal  use bzero()
01b,29aug02,ant  added m2IpDestructor()
01a,28aug02,ant  written
*/

/*
DESCRIPTION
This configlette contains the initialization routine for the
INCLUDE_MIB2_IP component.

NOMANUAL
*/

#include <m2Lib.h>

/* externs */

#ifdef VIRTUAL_STACK
VS_REG_ID m2IpRegistrationNum;
#endif /* VIRTUAL_STACK */

LOCAL STATUS usrNetM2IpInit (void);

LOCAL STATUS usrNetM2IpStart (void)
    {
#ifdef VIRTUAL_STACK
    /*
     * Register with vsmanager to allow the MIB2 IP Layer
     * to start in every virtual stack at the appropriate time.
     */

    return (vsComponentRegister (&m2IpRegistrationNum, "MIB2_IP",
                                 VS_SC_ENABLED, VS_SC_NON_OPTIONAL,
                                 usrNetM2IpInit, 0, NULL, NULL));
#else
    /*
     * When only one network stack exists, just
     * start the MIB2 IP Layer immediately.
     */

    return (usrNetM2IpInit ());

#endif /* VIRTUAL_STACK */
    }

LOCAL STATUS usrNetM2IpInit (void)
    {
    return m2IpInit (MIB2IP_MAX_ROUTE_TBL_SIZE);
    }
