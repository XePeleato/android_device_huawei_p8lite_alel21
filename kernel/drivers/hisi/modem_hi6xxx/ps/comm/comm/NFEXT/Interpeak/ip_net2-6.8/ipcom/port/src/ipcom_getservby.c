/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_getservby.c,v $ $Revision: 1.13 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/port/src/ipcom_getservby.c,v $
 *   $Author: joel $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *     Design and implementation by Markus Carlstedt <markus@interpeak.se>
 ******************************************************************************
 */


/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 2                    CONFIGURATION
 ****************************************************************************
 */

#include "ipcom_config.h"

/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#define IPCOM_USE_CLIB_PROTO
#include "ipcom_type.h"
#include "ipcom_cstyle.h"
#include "ipcom_clib.h"   
#include "ipcom_sock.h"
#include "ipcom_sock2.h"


/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

/*
 *===========================================================================
 *                         Ipcom_service
 *===========================================================================
 */
struct Ipcom_service
{
    char *name;
    int port;
    char *proto;
    char *alias1;
    char *alias2;
};
 

/*
 ****************************************************************************
 * 6                    EXTERN PROTOTYPES
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 7                    LOCAL PROTOTYPES
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 8                    DATA
 ****************************************************************************
 */

IP_STATIC IP_CONST struct Ipcom_service services[] = 
{
    {"echo",            7,       "tcp",     IP_NULL,            IP_NULL},
    {"echo",            7,       "udp",     IP_NULL,            IP_NULL},
    {"discard",         9,       "tcp",     "sink",             "null"},            
    {"discard",         9,       "udp",     "sink",             "null"},
    {"ftp-data",        20,      "tcp",     IP_NULL,            IP_NULL},
    {"ftp",             21,      "tcp",     IP_NULL,            IP_NULL},
    {"ssh",             22,      "tcp",     IP_NULL,            IP_NULL},                          
    {"ssh",             22,      "udp",     IP_NULL,            IP_NULL},                          
    {"telnet",          23,      "tcp",     IP_NULL,            IP_NULL},
    {"domain",          53,      "tcp",     "nameserver",       IP_NULL},      
    {"domain",          53,      "udp",     "nameserver",       IP_NULL},
    {"bootps",          67,      "tcp",     IP_NULL,            IP_NULL},                          
    {"bootps",          67,      "udp",     IP_NULL,            IP_NULL},
    {"bootpc",          68,      "tcp",     IP_NULL,            IP_NULL},                          
    {"bootpc",          68,      "udp",     IP_NULL,            IP_NULL},
    {"tftp",            69,      "udp",     IP_NULL,            IP_NULL},
    {"www",             80,      "tcp",     "http",             IP_NULL},            
    {"www",             80,      "udp",     IP_NULL,            IP_NULL},                          
    {"kerberos",        88,      "tcp",     "kerberos5",        "krb5"},
    {"kerberos",        88,      "udp",     "kerberos5",        "krb5"},
    {"sftp",            115,     "tcp",     IP_NULL,            IP_NULL},
    {"ntp",             123,     "tcp",     IP_NULL,            IP_NULL},
    {"ntp",             123,     "udp",     IP_NULL,            IP_NULL},                         
    {"snmp",            161,     "udp",     IP_NULL,            IP_NULL},                         
    {"snmp-trap",       162,     "udp",     "snmptrap",         IP_NULL},                         
    {"bgp",             179,     "tcp",     IP_NULL,            IP_NULL},                         
    {"bgp",             179,     "udp",     IP_NULL,            IP_NULL},                         
    {"ldap",            389,     "tcp",     IP_NULL,            IP_NULL},                    
    {"ldap",            389,     "udp",     IP_NULL,            IP_NULL},                    
    {"https",           443,     "tcp",     IP_NULL,            IP_NULL},                       
    {"https",           443,     "udp",     IP_NULL,            IP_NULL},                         
    {IP_NULL,           0,      IP_NULL,    IP_NULL,            IP_NULL}
};


/*
 ****************************************************************************
 * 9                    STATIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipcom_find_service_byname
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC struct Ipcom_service *
ipcom_find_service_byname(const char *name, const char *proto)
{
    int i = 0;

    while(services[i].name != IP_NULL)
    {
        if(ipcom_strcmp(services[i].name, name) == 0
            || (services[i].alias1 != IP_NULL && ipcom_strcmp(services[i].alias1, name) == 0)
            || (services[i].alias2 != IP_NULL && ipcom_strcmp(services[i].alias2, name) == 0))
        {
            if(proto == IP_NULL || ipcom_strcmp(services[i].proto, proto) == 0)
                return (struct Ipcom_service *)&services[i];
        }
        ++i;
    }
    return IP_NULL;
}


/*
 *===========================================================================
 *                    ipcom_find_service_byport
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC struct Ipcom_service *
ipcom_find_service_byport(int port, char *proto)
{
    int i = 0;

    while(services[i].name != IP_NULL)
    {
        if(services[i].port == port)
        {
            if(proto == IP_NULL || ipcom_strcmp(services[i].proto, proto) == 0)
                return (struct Ipcom_service *)&services[i];
        }
        ++i;
    }
    return IP_NULL;
}


/*
 ****************************************************************************
 * 11                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipcom_getservbyname
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC struct Ip_servent *
ipcom_getservbyname(const char *name, const char *proto)
{
    static struct Ip_servent se;
    static char *s_aliases[3] = { IP_NULL, IP_NULL, IP_NULL };
    struct Ipcom_service *sp;

    if(name == IP_NULL)
        return IP_NULL;
    sp = ipcom_find_service_byname(name, proto);
    if(sp == IP_NULL)
        return IP_NULL;

    se.s_name    = sp->name;
    s_aliases[0] = sp->alias1;
    s_aliases[1] = sp->alias2;
    se.s_aliases = s_aliases;
    se.s_port    = (int)ip_htons(sp->port);
    se.s_proto   = sp->proto;

    return &se;
}


/*
 *===========================================================================
 *                    ipcom_getservbyport
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC struct Ip_servent *
ipcom_getservbyport(int port, char *proto)
{
    static struct Ip_servent se;
    static char *s_aliases[3] = { IP_NULL, IP_NULL, IP_NULL };
    struct Ipcom_service *sp;

    sp = ipcom_find_service_byport((int)ip_ntohs(port), proto);
    if(sp == IP_NULL)
        return IP_NULL;

    se.s_name    = sp->name;
    s_aliases[0] = sp->alias1;
    s_aliases[1] = sp->alias2;
    se.s_aliases = s_aliases;
    se.s_port    = (int)ip_htons(sp->port);
    se.s_proto   = sp->proto;

    return &se;
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

