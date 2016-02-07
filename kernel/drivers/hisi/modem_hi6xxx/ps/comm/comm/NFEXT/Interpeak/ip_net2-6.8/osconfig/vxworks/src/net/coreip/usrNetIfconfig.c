/* usrNetIfconfig.c - Network default configuration */

/*
 * Copyright (c) 2002 - 2006 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River license agreement.
 */


/*
modification history
--------------------
01e,01aug06,kch  Removed the unsupported call to ifconfigInit().
01d,25aug04,vvv  fixed typo
01c,07may04,vvv  fixed warnings (SPR #96364)
01b,07mar03,ann  added call to ifconfigInit()
01a,05feb02,ham  wrote.
*/

/*
DESCRIPTION

This configlette contains the initialization routine for the
INCLUDE_IFCONFIG component.

NOMANUAL
*/

void usrNetIfconfigInit (void)
    {
    char * ifString;

    /* Configure up to 10 interfaces with ifconfig */

    ifString = IFCONFIG_ENTRY_1;
    if ((ifString != NULL))
        ifconfig (IFCONFIG_ENTRY_1);

    ifString = IFCONFIG_ENTRY_2;
    if ((ifString != NULL))
        ifconfig (IFCONFIG_ENTRY_2);

    ifString = IFCONFIG_ENTRY_3;
    if ((ifString != NULL))
        ifconfig (IFCONFIG_ENTRY_3);

    ifString = IFCONFIG_ENTRY_4;
    if ((ifString != NULL))
        ifconfig (IFCONFIG_ENTRY_4);

    ifString = IFCONFIG_ENTRY_5;
    if ((ifString != NULL))
        ifconfig (IFCONFIG_ENTRY_5);

    ifString = IFCONFIG_ENTRY_6;
    if ((ifString != NULL))
        ifconfig (IFCONFIG_ENTRY_6);

    ifString = IFCONFIG_ENTRY_7;
    if ((ifString != NULL))
        ifconfig (IFCONFIG_ENTRY_7);

    ifString = IFCONFIG_ENTRY_8;
    if ((ifString != NULL))
        ifconfig (IFCONFIG_ENTRY_8);

    ifString = IFCONFIG_ENTRY_9;
    if ((ifString != NULL))
        ifconfig (IFCONFIG_ENTRY_9);

    ifString = IFCONFIG_ENTRY_10;
    if ((ifString != NULL))
        ifconfig (IFCONFIG_ENTRY_10);

    }
