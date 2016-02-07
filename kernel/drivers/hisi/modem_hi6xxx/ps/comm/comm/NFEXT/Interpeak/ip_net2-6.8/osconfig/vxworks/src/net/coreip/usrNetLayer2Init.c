/* usrNetLayer2Init.c - Layer 2 network initialization routines */

/*
 * Copyright (c) 2005 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River license agreement.
 */

/*
modification history
--------------------
01a,11aug05,kch  written.
*/

/*
DESCRIPTION
-----------
Configlette to initialize Layer 2 network infrastructures.
*/

#include <muxL2Lib.h>

STATUS usrNetMuxL2Init (void)
    {
    STATUS  rc;

    bzero ((char *)&muxL2DefaultConfigParams, sizeof (MUX_L2_CONFIG_PARAMS));
    muxL2DefaultConfigParams.cfgh.len = sizeof (MUX_L2_CONFIG_PARAMS);
    muxL2DefaultConfigParams.cfg_priv_muxL2NumPorts = MUX_L2_NUM_PORTS_CFG;
    muxL2DefaultConfigParams.cfg_priv_muxL2MaxVlanSupport = 
                                                    MUX_L2_MAX_VLANS_CFG;

    rc = muxL2LibInit (&muxL2DefaultConfigParams);
    if (rc == ERROR)
        {
	    if (_func_printErr)
	        (*_func_printErr) ("usrNetMuxL2Init: muxL2LibInit failed\n");
        return ERROR;
        }

    return OK;
    }
