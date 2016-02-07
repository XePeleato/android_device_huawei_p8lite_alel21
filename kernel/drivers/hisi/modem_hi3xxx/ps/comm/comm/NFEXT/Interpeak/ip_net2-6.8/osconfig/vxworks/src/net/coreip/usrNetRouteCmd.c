/* usrNetRouteCmd.c - Network route command configuration */

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
01d,01aug06,kch  Removed the unsupported call to routeCmdInit().
01c,07may04,vvv  fixed warnings (SPR #96364)
01b,24mar03,ann  added a call to routeCmdInit().
01a,05feb02,ham  wrote.
*/

/*
DESCRIPTION

This configlette contains the initialization routine for the
INCLUDE_ROUTECMD component.

NOMANUAL
*/

void usrNetRouteCmdInit (void)
    {
    char * routeString;

    /* Add 5 static routing entries */

    routeString = ROUTE_ENTRY_1;
    if ((routeString != NULL))
        routec (ROUTE_ENTRY_1);

    routeString = ROUTE_ENTRY_2;
    if ((routeString != NULL))
        routec (ROUTE_ENTRY_2);

    routeString = ROUTE_ENTRY_3;
    if ((routeString != NULL))
        routec (ROUTE_ENTRY_3);

    routeString = ROUTE_ENTRY_4;
    if ((routeString != NULL))
        routec (ROUTE_ENTRY_4);

    routeString = ROUTE_ENTRY_5;
    if ((routeString != NULL))
        routec (ROUTE_ENTRY_5);
    }
