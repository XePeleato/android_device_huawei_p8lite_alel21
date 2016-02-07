/* usrNetM2IfLib.c - Initialization routine for the MIB2_IF library */

/* Copyright 1992 - 2004 Wind River Systems, Inc. */

/*
modification history
--------------------
01j,13oct08,dlk  Fix typo causing build error.
01i,09Oct08,zhr  Fixed WIND00130141 compiler warning 
01h,12may08,zhr  Updated for GPP 
01h,12apr02,zhr  Migrated to ipnet
01g,30sep04,dlk  Fix warning.
01f,19sep04,spm  updated virtual stack startup: restored use of published
                 init routine and removed packing/unpacking parameters
01e,06nov03,rlm  Ran batch header update for header re-org.
01d,26nov02,ism  changed instance initialisation function
01c,04sep02,kal  fixed bzero
01b,29aug02,ant  added m2IfDestructor()
01a,28aug02,ant  written
*/

/*
DESCRIPTION
This configlette contains the initialization routine for the
INCLUDE_MIB2_IF component.

NOMANUAL
*/

#include <m2Lib.h>

/* externs */

#ifdef VIRTUAL_STACK
VS_REG_ID m2IfRegistrationNum;
extern STATUS m2IfDestructor (VSNUM vsnum);
#endif /* VIRTUAL_STACK */

IMPORT int    m2If64BitCounters;

#ifdef INCLUDE_MIB2_IFMGT
LOCAL STATUS usrNetM2IfInit (void)
    {
    return m2IfInit (MIB2IF_TRAP_RTN, MIB2IF_TRAP_ARG);
    }
#endif

LOCAL STATUS usrNetM2IfStart (void)
    {
#ifdef INCLUDE_MIB2_IFMGT
#ifdef VIRTUAL_STACK
    VS_STATUS vsstat;
#endif

    m2If64BitCounters = MIB2IF_LONG_COUNTERS;

#ifdef VIRTUAL_STACK
    /*
     * Register with vsmanager to allow the MIB2 Interface Layer
     * to start in every virtual stack at the appropriate time.
     */

    vsstat = vsComponentRegister (&m2IfRegistrationNum, "MIB2_IF",
				  VS_SC_ENABLED, VS_SC_NON_OPTIONAL,
				  usrNetM2IfInit, 0, NULL, m2IfDestructor);
    return (vsstat == VS_OK ? OK : ERROR);
#else
    /*
     * When only one network stack exists, just
     * start the MIB2 Interface Layer immediately.
     */

    return usrNetM2IfInit (); /* MIB2 Variables for Interface Layer */
#endif
#else 
    return (OK);
#endif /* INCLUDE_MIB2_IFMGT */
    }

