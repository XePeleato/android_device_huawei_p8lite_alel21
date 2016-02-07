/* usrNetDaemon.c - the daemon init */

/* Copyright 1984-2002, 2007, 2009 Wind River Systems, Inc. */

/*
modification history
--------------------
01i,20may09,dlk  Remove obsolete INCLUDE_SHELL_DEBUG setting of
                 _func_bdall.
01h,10jul07,dlk  Pass in network task daemon function to decouple
                 network stack from network daemon support.
01g,06may07,dlk  Correct typo.
01f,02may07,dlk  Multiple network daemon support.
01e,17oct02,ham  fixed vxWorks.st build error due to WDB symbols (SPR 83160).
01d,01apr02,ham  guard _func_* initialization with inclusion macro.
01c,28mar02,ann  added the code to enable system mode debugging when
                 tNetTask is spawned as BREAKABLE.
01b,21feb02,ham  changed in response to obsolete usrNetDaemonStart.
01a,09dec01,ham  wrote.
*/

extern unsigned int netJobNum;
extern int      netTaskId;
extern int      netTaskPriority;
extern int      netTaskOptions;
extern int      netTaskStackSize;

extern BOOL wdbIsNowExternal ();
extern FUNCPTR _func_wdbIsNowExternal;

#ifdef INCLUDE_IPCOM

STATUS ipcomNetTask
    (
    JOB_QUEUE_ID qId,
    int jobQueueIndex   /* unused */
    )
    {
    extern int ipcom_ima_stack_task (void);

    (void) jobQueueIndex;

    /* Declare this task a stack daemon. */

    if (ipcom_ima_stack_task() != 0)
	return ERROR;

    return jobQueueProcessFunc (qId); /* normally won't return */
    }

#endif /* INCLUDE_IPCOM */

STATUS usrNetDaemonInit
    (
    unsigned int	jobNum,
    unsigned int	pri,
    int			opts,
    unsigned int	stack
    )
    {
    netJobNum = jobNum;
    netTaskPriority = pri;
    netTaskOptions = opts;
    netTaskStackSize = stack;

    if ( !(netTaskOptions & VX_UNBREAKABLE) )
	{
#if defined(INCLUDE_WDB) && defined(INCLUDE_WDB_BP)
        _func_wdbIsNowExternal = (FUNCPTR)wdbIsNowExternal;
#endif
	}

#ifdef INCLUDE_IPCOM
    return (netLibInitialize(NUM_NET_DAEMONS, NET_DAEMONS_CPU_AFFINITY,
			     ipcomNetTask));
#else
    return (netLibInitialize(NUM_NET_DAEMONS, NET_DAEMONS_CPU_AFFINITY, NULL));
#endif /* INCLUDE_IPCOM */
    }
