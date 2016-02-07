/* usrNetEndLib.c - End Init routines */

/* Copyright (c) 2002, 2004-2008 Wind River Systems, Inc.
 *
 * The right to copy, distribute or otherwise make use of this software
 * may be licensed only pursuant to the terms of an applicable Wind River
 * license agreement. No license to Wind River intellectual property rights
 * is granted herein. All rights not licensed by Wind River are reserved
 * by Wind River.
 */

/*
modification history
--------------------
01m,17jun08,dlk  Reference endDevTbl only if INCLUDE_END is defined.
01l,22mar08,dlk  Initial support for END2 drivers.
01k,30jul07,mze  extern usrTipcHendAttach and fix Copyright and mod history
01j,03apr07,d_c  Method update on behalf of Radzy
01i,12jun06,wap  Modified usrNetEndLibInit() to initialize function pointer
                 _func_m2PollStatsIfPoll with END_POLL_STATS_ROUTINE_HOOK
                 configuration.
01h,21sep05,dsk  added usrTipcHendAttach 
01g,02sep05,mdo  vxBus: Add vxb prefix
01f,08aug05,mdo  Change WINDBUS to VXBUS
01e,08aug05,wap  Change polled stats hook into a component
01e,28jul04,rcs  Added vxbus support
01d,04oct04,wap  Add missing polling stats hook (SPR #101616)
01c,11mar02,ham  set ON to the processed flag after sucessful muxDevStart
01b,28feb02,ham  added netDevBootFlag.
01a,28jan02,ham  revised from TOR3_1-FCS(01f).
*/


#ifdef INCLUDE_VXBUS
#include <hwif/vxbus/vxBus.h>
#endif

/*
DESCRIPTION

This configlette contains the initialization routine for the
INCLUDE_END component.

NOMANUAL
*/

#ifdef INCLUDE_END_POLLED_STATS
IMPORT FUNCPTR  _func_m2PollStatsIfPoll; /* defined in funcBind.c */
#ifndef END_POLL_STATS_ROUTINE_HOOK
#define END_POLL_STATS_ROUTINE_HOOK     m2PollStatsIfPoll
#endif
#endif

#ifdef INCLUDE_TIPC_HEND_INIT
extern STATUS usrTipcHendAttach (void); /* defined in target/config/comps/src/usrTipc.c */
#endif

#if defined (INCLUDE_END2) && defined(INCLUDE_VXBUS)
LOCAL void end2VxBusConnect (void)
    {
    /* Add in mux ENDs from bus subsystem */
    vxbDevMethodRun((UINT32)&mux2DevConnect_desc, NULL);
    }
#endif /* defined (INCLUDE_END2) && defined(INCLUDE_VXBUS) */

STATUS usrNetEndLibInit()
    {
#ifdef INCLUDE_END
    int         count;
    END_TBL_ENTRY * pDevTbl;
    void *      pCookie = NULL;
#endif

#ifdef INCLUDE_TIPC_HEND_INIT
    usrTipcHendAttach();
#endif

#ifdef INCLUDE_END_POLLED_STATS
    /* initialize the _m2PollStatsIfPoll function pointer */
    _func_m2PollStatsIfPoll = (FUNCPTR)END_POLL_STATS_ROUTINE_HOOK;
#endif

#ifdef INCLUDE_VXBUS
#ifdef INCLUDE_END2
#ifdef INCLUDE_VXMUX_PKT_POOL_MIN
    /*
     * Perform late end2VxBusConnect() operation now that the VXMUX
     * minimal packet pool should be ready.
     */
    end2VxBusConnect ();
#else
    /*
     * For now, delay connection (load/start) of END2 drivers until the
     * ipnet packt pool is ready.
     */
    _func_end2VxBusConnect = end2VxBusConnect;
#endif
#endif /* INCLUDE_END2 */
    /* Add in mux ENDs from bus subsystem */
    vxbDevMethodRun((UINT32)&muxDevConnect_desc, NULL);
#endif /* INCLUDE_VXBUS */
 
#ifdef INCLUDE_END
    /* Add in mux ENDs from BSP. */
    for (count = 0, pDevTbl = endDevTbl; pDevTbl->endLoadFunc != END_TBL_END;
         pDevTbl++, count++)
        {
        /* Make sure that WDB has not already installed the device. */
        if (!pDevTbl->processed)
            {
            pCookie = muxDevLoad (pDevTbl->unit,
                                  pDevTbl->endLoadFunc,
                                  pDevTbl->endLoadString,
                                  pDevTbl->endLoan, pDevTbl->pBSP);
            if (pCookie == NULL)
        {
            if (_func_printErr)
                    (* _func_printErr)
            ("muxDevLoad failed for device entry %d!\n", count);
        }
            else
                {
                if (muxDevStart(pCookie) == ERROR)
            {
            if (_func_printErr)
                        (* _func_printErr)
                    ("muxDevStart failed for entry %d!\n", count);
            }
        else
                    {
                    pDevTbl->processed = TRUE;
#ifdef INCLUDE_END_POLLED_STATS
                    endPollStatsInit (pCookie, END_POLL_STATS_ROUTINE_HOOK);
#endif /* INCLUDE_END_POLLED_STATS */
                    }
                }
            }
        }
#endif /* INCLUDE_END */

#ifdef INCLUDE_NET_BOOT
    netDevBootFlag = TRUE;
#endif /* INCLUDE_NET_BOOT */
    return (OK);
    }

