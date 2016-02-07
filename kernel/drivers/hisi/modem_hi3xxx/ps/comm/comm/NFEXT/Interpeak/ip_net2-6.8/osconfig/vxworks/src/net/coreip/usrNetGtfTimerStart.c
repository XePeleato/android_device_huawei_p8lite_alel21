/* usrNetGtfTimerStart.c - the daemon init */

/* Copyright 1984 - 2001, 2005 Wind River Systems, Inc. */

/*
modification history
--------------------
01b,11apr05,sar  Replace global tm with gtfTimerManagerCB
01a,09dec01,ham  wrote.
*/

extern GTF_TIMER_MANAGER *gtfTimerManagerCB;

STATUS usrNetGtfTimerStart (void)
    {
    return (gtfTimerManagerRun(gtfTimerManagerCB));
    }
