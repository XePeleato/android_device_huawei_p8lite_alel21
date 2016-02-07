/* usrNetArpLib.c - arpLib configuration */

/* Copyright 1984 - 2004 Wind River Systems, Inc. */

/*
modification history
--------------------
01d,07may04,vvv  fixed warnings (SPR #96364)
01c,12aug03,niq  Merging from Accordion label ACCORDION_BASE6_MERGE_BASELINE
01b,06sep02,ppp  making a call to arpLibInit for that module to be included
01a,05feb02,ham  wrote.
*/

/*
DESCRIPTION

This configlette contains the initialization routine for the
INCLUDE_ARP_API component.

NOMANUAL
*/

/* includes */
#include <arpLib.h>

extern void arpLibInit(void);

void usrNetArpLibInit (void)
    {
    char * arpHost;
    char * arpAddr;

    arpLibInit();

    /* Add 5 static arp entries */

    arpHost = ARP_ENTRY_1_HOST;
    arpAddr = ARP_ENTRY_1_ADDR;

    if (((arpHost != NULL)) && ((arpAddr != NULL)))
        arpAdd (ARP_ENTRY_1_HOST, ARP_ENTRY_1_ADDR, ATF_PERM);

    arpHost = ARP_ENTRY_2_HOST;
    arpAddr = ARP_ENTRY_2_ADDR;

    if (((arpHost != NULL)) && ((arpAddr != NULL)))
        arpAdd (ARP_ENTRY_2_HOST, ARP_ENTRY_2_ADDR, ATF_PERM);

    arpHost = ARP_ENTRY_3_HOST;
    arpAddr = ARP_ENTRY_3_ADDR;

    if (((arpHost != NULL)) && ((arpAddr != NULL)))
        arpAdd (ARP_ENTRY_3_HOST, ARP_ENTRY_3_ADDR, ATF_PERM);

    arpHost = ARP_ENTRY_4_HOST;
    arpAddr = ARP_ENTRY_4_ADDR;

    if (((arpHost != NULL)) && ((arpAddr != NULL)))
        arpAdd (ARP_ENTRY_4_HOST, ARP_ENTRY_4_ADDR, ATF_PERM);

    arpHost = ARP_ENTRY_5_HOST;
    arpAddr = ARP_ENTRY_5_ADDR;

    if (((arpHost != NULL)) && ((arpAddr != NULL)))
        arpAdd (ARP_ENTRY_5_HOST, ARP_ENTRY_5_ADDR, ATF_PERM);

    return;
    }
