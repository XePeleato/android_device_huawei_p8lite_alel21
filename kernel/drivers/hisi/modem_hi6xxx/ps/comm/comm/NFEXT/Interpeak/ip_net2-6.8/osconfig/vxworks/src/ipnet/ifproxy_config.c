/* usrIfproxy.c - initialization of Ifproxy Network Device */

/*
 * Copyright (c) 2010 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River license agreement.
 */

/*
modification history
--------------------
01a,17may10,spw  written
*/

#include <vxWorks.h>


/* defines */

/* externals */

IMPORT STATUS ifproxy_drv_init(int ctrlPort, char *busName, char *portCfgStr);

/* locals */

/*******************************************************************************
*
* ifproxy_init - Initialization configlette for the Ifproxy Network Device
*
* This routine parses configuration details about the desired ifproxy driver
* to create.
* 
* busName is the desired MIPC bus to use for inter-OS communication between
* the management plane and the NAEs.
* 
* ctrlPort is the MIPC port to listen on for ifproxy NAE registration 
* requests.
*
* portCfgStr specifies the forwarder ports and cores configuration
* It is a string listing the ports that participate in accelerated
* forwarding, and the forwarder cores that will poll them.  The string
* should have the following format:
* 
* <ws>* [<ifpollspec> (';' <ws>* <ifpollspec>)* [';'] <ws>*]
* 
* Where:
*   ifpollspec := <ifname> (<ws>+ <cpuid>)* <ws>*
*   ifname := <alpha>+ <decimal>+
*   cpuid := <decimal>+
*   ws := any whitespace character (isspace())
*   alpha := any alphabetic character (A-Za-z : isalpha())
*   decimal := any decimal numeral (0-9 : isdigit())
*   
* Example:
* 
*   "proxy1 3; proxy2 3; proxy3 7; proxy4 7; proxy8 5 6"
* 
    Indicates that ports proxy2 and proxy3 are polled by a forwarder running 
*   on CPU 3, ports proxy4 and proxy5 are polled by a forwarder running on 
*   CPU 7, and port proxy8 is polled by forwarders running on CPUs 5 and 6. 
*   CPUs outside of the configured range will be ignored.  CPU 0 cannot run a 
*   forwarder ...
*
* This is executed only once at BOOT time
*
* RETURNS: N/A
*
* NOMANUAL
*/
STATUS ifproxy_init
    (
    char *    busName,    /* MIPC bus to use for inter-OS communication     */
    int       ctrlPort,   /* MIPC port for ifproxy registration requests    */
    char *    portCfgStr  /* Port configuration string                      */

    )
    {
	
	if (busName == NULL || portCfgStr == NULL)
		return ERROR;
	
	return ifproxy_drv_init(ctrlPort, busName, portCfgStr);
    }
