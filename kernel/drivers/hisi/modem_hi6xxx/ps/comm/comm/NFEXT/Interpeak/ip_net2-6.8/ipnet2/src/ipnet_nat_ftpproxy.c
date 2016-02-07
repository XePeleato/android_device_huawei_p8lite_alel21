/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_nat_ftpproxy.c,v $ $Revision: 1.41 $
 *     $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_nat_ftpproxy.c,v $
 *     $Author: markus $
 *     $State: Exp $ $Locker:  $
 *
 *     INTERPEAK_COPYRIGHT_STRING
 *     Design and implementation by Markus Carlstedt <markus@interpeak.se>
 ******************************************************************************
 */

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 * Proxy (ALG) for the FTP protocol.
 */

/*
 ****************************************************************************
 * 2                    CONFIGURATION
 ****************************************************************************
 */

#ifndef IPCOM_KERNEL
#define IPCOM_KERNEL
#endif
#include "ipnet_config.h"


/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#ifdef IPNET_USE_NAT

#define IPCOM_USE_CLIB_PROTO
#include <ipcom_clib.h>
#include <ipcom_sock.h>
#include <ipcom_inet.h>
#include <ipcom_syslog.h>

#include "ipnet_nat.h"


/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

#ifndef IPCOM_SYSLOG_PRIORITY
#define IPCOM_SYSLOG_PRIORITY    IPNET_SYSLOG_PRIORITY
#endif
#ifndef IPCOM_SYSLOG_FACILITY
#define IPCOM_SYSLOG_FACILITY    IPCOM_LOG_IPNET
#endif

#define FTP_AF_INET     '1'
#define FTP_AF_INET6    '2'
#define FTP_DELIMITER   '|'


/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

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

IP_STATIC IP_CONST char *port_cmd    = "PORT ";
IP_STATIC IP_CONST char *extport_cmd = "EPRT ";
IP_STATIC IP_CONST char *extpasv_cmd = "EPSV";
IP_STATIC IP_CONST char *pasv_rep    = "227 Entering Passive Mode ";
IP_STATIC IP_CONST char *extpasv_rep = "229 Entering Extended Passive Mode ";

/* temp buffer. reentrancy protected by IPNET code lock */
IP_STATIC char ftpbuf[128];


/*
 ****************************************************************************
 * 9                   LOCAL FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipnet_nat_proxy_ftp_port_cmd
 *===========================================================================
 * Description: Modify the PORT command.
 * Parameters:  appdata   - pointer to application data.
 *              applen    - pointer to length of application data.
 *              growspace - space available to extend application data.
 *              param     - pointer to proxy parameters.
 *              newdata   - pointer to pointer to new application data.
 * Returns:     1 = Packet modified.
 *              0 = Packet untouched.
 */
IP_STATIC int
ipnet_nat_proxy_ftp_port_cmd(Ip_u8 *appdata,
                             int   *applen,
                             int    growspace,
                             Ipnet_nat_proxy_param *param,
                             Ip_u8 **newdata)
{
    Ipnet_nat_proxy_tuple proxy_tuple;
    char *suff, *cp;
    Ip_u8 port_msb, port_lsb;
    Ip_u16 port;
    Ip_u32 src_addr_n, nat_addr_n = ip_htonl(param->nat_addr);
    int num_commas, diff, newlen, tmp;

    /* Copy the 'PORT' command */
    ipcom_memset(ftpbuf, 0, sizeof(ftpbuf));
    if(*applen >= (int)sizeof(ftpbuf))
        return 0;
    ipcom_memcpy(ftpbuf, appdata, *applen);
    cp = ftpbuf;
    cp += ipcom_strlen(port_cmd);

    /* Count the commas */
    num_commas = 0;
    while(*cp != '\0')
    {
        if(*cp == ',')
        {
            num_commas++;
            if(num_commas < 4)
                *cp = '.';
            else if(num_commas == 4)
                *cp = '\0';
        }
        cp++;
    }
    if(num_commas != 5)
        return 0;

    /* Get the ip address */
    cp = ftpbuf;
    cp += ipcom_strlen(port_cmd);
    if(ipcom_inet_pton(IP_AF_INET, cp, &src_addr_n) != 1)
        return 0;

    /* Get most significant part of the port */
    cp += ipcom_strlen(cp)+1;
    tmp = (int)ipcom_strtol(cp, &suff, 10);
    if(tmp < 0 || tmp > 255 || cp == suff || *suff != ',')
        return 0;
    port_msb = (Ip_u8)tmp;
    cp = suff + 1;

    /* Get least significant part of the port */
    tmp = ipcom_strtol(cp, &suff, 10);
    if(tmp < 0 || tmp > 255 || cp == suff || *suff != '\r')
        return 0;
    port_lsb = (Ip_u8)tmp;
    port = (Ip_u16)((port_msb<<8) + port_lsb);
    cp = suff + 1;

    if(*cp != '\n')
        return 0;


    /* Add a mapping for the ftp data connection */
    ipcom_memset(&proxy_tuple, 0, sizeof(proxy_tuple));
    proxy_tuple.protocol     = param->tuple.protocol;
    proxy_tuple.private_addr = ip_ntohl(src_addr_n);
    proxy_tuple.private_port = port;
    proxy_tuple.public_addr  = param->tuple.public_addr;
    tmp = ipnet_nat_proxy_add_mapping(&proxy_tuple,
                                      0,
                                      param->mapping,
                                      IP_TRUE,         /* Use port translation */
                                      IP_TRUE,         /* Inbound session */
                                      IP_NULL,
                                      IP_NULL);
    if (tmp < 0)
        return 0;
    port_msb = (Ip_u8)((Ip_u16)tmp >> 8);
    port_lsb = (Ip_u8)tmp;

    /* Modify the buffer */
    cp = ftpbuf;
    cp += ipcom_strlen(port_cmd);
    if(ipcom_inet_ntop(IP_AF_INET, &nat_addr_n, cp, 16) == IP_NULL)
        return 0;
    while(*cp != 0)
    {
        if(*cp == '.')
            *cp = ',';  /* Change '.' to ',' */
        cp++;
    }
    *cp++ = ',';
    cp += ipcom_sprintf(cp, "%d", port_msb);    /* Add the port msb */
    *cp++ = ',';
    cp += ipcom_sprintf(cp, "%d", port_lsb);    /* Add the port msb */
    *cp++ = '\r';
    *cp++ = '\n';
    *cp = '\0';
    newlen = ipcom_strlen(ftpbuf);

    /* Update application data with the modified buffer */
    diff = newlen - *applen;
    if (diff > growspace)
    {
        /* Must allocate a new buffer */
        *newdata = ipcom_malloc(newlen);
        if (*newdata == IP_NULL)
        {
            IPCOM_LOG1(ERR, "ipnet_nat_proxy_ftp_port_cmd() :: ipcom_malloc(%d) failed",
                            *applen + diff);
            return -1;
        }
        ipcom_memcpy(*newdata, ftpbuf, newlen);
    }
    else
    {
        /* Let the current buffer grow */
        ipcom_memcpy(appdata, ftpbuf, newlen);
    }
    *applen = newlen;

    IPCOM_LOG2(DEBUG, "ipnet_nat_proxy_ftp_port_cmd() :: new cmd \"%s\" diff=%d", ftpbuf, diff);
    return 1;
}


/*
 *===========================================================================
 *                    ipnet_nat_proxy_ftp_extport_cmd
 *===========================================================================
 * Description: Modify the EPRT command.
 * Parameters:  appdata   - pointer to application data.
 *              applen    - pointer to length of application data.
 *              growspace - space available to extend application data.
 *              param     - pointer to proxy parameters.
 *              newdata   - pointer to pointer to new application data.
 * Returns:     1 = Packet modified.
 *              0 = Packet untouched.
 */
IP_STATIC int
ipnet_nat_proxy_ftp_extport_cmd(Ip_u8 *appdata,
                                int   *applen,
                                int    growspace,
                                Ipnet_nat_proxy_param *param,
                                Ip_u8 **newdata)
{
    Ipnet_nat_proxy_tuple proxy_tuple;
    char *suff, *cp, afnum, del;
    Ip_u16 port;
    Ip_u32 src_addr_n, nat_addr_n = ip_htonl(param->nat_addr);
    int diff, newlen, tmp;
    Ip_u32 ptsrc[4];

    /* Copy the the 'EPRT' command */
    ipcom_memset(ftpbuf, 0, sizeof(ftpbuf));
    if(*applen >= (int)sizeof(ftpbuf))
        return 0;
    ipcom_memcpy(ftpbuf, appdata, *applen);
    cp = ftpbuf;
    cp += ipcom_strlen(extport_cmd);

    /* Get the delimiter */
    del = *cp++;
    if(del < 33 || del > 126)
        return 0;

    /* Check the protocol */
    afnum = *cp++;
    if (param->natpt == IP_FALSE && afnum != FTP_AF_INET)
        return 0;
    if (param->natpt == IP_TRUE && afnum != FTP_AF_INET6)
        return 0;

    /* Check next delimiter */
    if(*cp++ != del)
        return 0;

    /* Prepare for ip address extraction */
    while(*cp != del)
    {
        if(*cp == '\0')
            return 0;
        cp++;
    }
    *cp = '\0';

    /* Get the ip address */
    cp = ftpbuf;
    cp += ipcom_strlen(extport_cmd);
    cp += 3;
    if (param->natpt == IP_FALSE)
    {
        if(ipcom_inet_pton(IP_AF_INET, cp, &src_addr_n) != 1)
            return 0;
    }
    else
    {
        if(ipcom_inet_pton(IP_AF_INET6, cp, ptsrc) != 1)
            return 0;
        src_addr_n = ptsrc[3];
    }

    /* Get the port */
    cp += ipcom_strlen(cp)+1;
    tmp = ipcom_strtol(cp, &suff, 10);
    if(tmp < 0 || tmp > 0xffff || cp == suff || *suff != del)
        return 0;
    port = (Ip_u16)tmp;
    cp = suff + 1;
    if(*cp++ != '\r')
        return 0;
    if(*cp != '\n')
        return 0;

    /* Add a mapping for the ftp data connection */
    ipcom_memset(&proxy_tuple, 0, sizeof(proxy_tuple));
    proxy_tuple.protocol     = param->tuple.protocol;
    proxy_tuple.private_addr = ip_ntohl(src_addr_n);
    proxy_tuple.private_port = port;
    proxy_tuple.public_addr  = param->tuple.public_addr;
    tmp = ipnet_nat_proxy_add_mapping(&proxy_tuple,
                                      0,
                                      param->mapping,
                                      IP_TRUE,         /* Use port translation */
                                      IP_TRUE,         /* Inbound session */
                                      IP_NULL,
                                      IP_NULL);
    if (tmp < 0)
        return 0;
    port = (Ip_u16)tmp;

    /* Modify the buffer */
    cp = ftpbuf;
    cp += ipcom_strlen(extport_cmd);
    *cp++ = del;
    *cp++ = FTP_AF_INET;
    *cp++ = del;
    if(ipcom_inet_ntop(IP_AF_INET, &nat_addr_n, cp, 16) == IP_NULL)
        return 0;
    cp += ipcom_strlen(cp);
    *cp++ = del;
    cp += ipcom_sprintf(cp, "%d", port);
    *cp++ = del;
    *cp++ = '\r';
    *cp++ = '\n';
    *cp = '\0';
    newlen = ipcom_strlen(ftpbuf);

    /* Update application data with the modified buffer */
    diff = newlen - *applen;
    if (diff > growspace)
    {
        /* Must allocate a new buffer */
        *newdata = ipcom_malloc(*applen + diff);
        if (*newdata == IP_NULL)
        {
            IPCOM_LOG1(ERR, "ipnet_nat_proxy_ftp_port_cmd() :: ipcom_malloc(%d) failed",
                            *applen + diff);
            return -1;
        }
        ipcom_memcpy(*newdata, ftpbuf, newlen);
    }
    else
    {
        /* Let the current buffer grow */
        ipcom_memcpy(appdata, ftpbuf, newlen);
    }
    *applen = newlen;

    IPCOM_LOG2(DEBUG, "ipnet_nat_proxy_ftp_extport_cmd() :: new cmd \"%s\" diff=%d", ftpbuf, diff);
    return 1;
}


/*
 *===========================================================================
 *                    ipnet_nat_proxy_ftp_extpasv_cmd
 *===========================================================================
 * Description: Modify the EPSV command.
 * Parameters:  appdata   - pointer to application data.
 *              applen    - pointer to length of application data.
 *              growspace - space available to extend application data.
 *              param     - pointer to proxy parameters.
 *              newdata   - pointer to pointer to new application data.
 * Returns:     1 = Packet modified.
 *              0 = Packet untouched.
 */
IP_STATIC int
ipnet_nat_proxy_ftp_extpasv_cmd(Ip_u8 *appdata,
                                int   *applen,
                                int    growspace,
                                Ipnet_nat_proxy_param *param,
                                Ip_u8 **newdata)
{
    (void)growspace;
    (void)newdata;
    if (param->natpt == IP_TRUE && *applen == 8)    /* "EPSV<space>2\r\n" */
    {
        /* Check 'EPSV' command */
        if (ipcom_memcmp(appdata, "EPSV 2\r\n", 8) != 0)
            return 0;
        /* Modify the command */
        ipcom_memcpy(appdata, "EPSV 1\r\n", 8);
        IPCOM_LOG1(DEBUG, "ipnet_nat_proxy_ftp_extpasv_cmd() :: new cmd \"%s\"", "EPSV 1\r\n");
        return 1;
    }
    return 0;
}


/*
 *===========================================================================
 *                    ipnet_nat_proxy_ftp_pasv_rep
 *===========================================================================
 * Description: Modify the PASV reply.
 * Parameters:  appdata   - pointer to application data.
 *              applen    - pointer to length of application data.
 *              growspace - space available to extend application data.
 *              param     - pointer to proxy parameters.
 *              newdata   - pointer to pointer to new application data.
 * Returns:     1 = Packet modified.
 *              0 = Packet untouched.
 */
IP_STATIC int
ipnet_nat_proxy_ftp_pasv_rep(Ip_u8 *appdata,
                             int   *applen,
                             int    growspace,
                             Ipnet_nat_proxy_param *param,
                             Ip_u8 **newdata)
{
    Ipnet_nat_proxy_tuple proxy_tuple;
    char *suff, *cp;
    Ip_u8 port_msb, port_lsb;
    Ip_u16 port;
    Ip_u32 src_addr_n, nat_addr_n = ip_htonl(param->nat_addr);
    int num_commas, diff, newlen, tmp;

    /* Copy the the 'PASV reply' command */
    ipcom_memset(ftpbuf, 0, sizeof(ftpbuf));
    if(*applen >= (int)sizeof(ftpbuf))
        return 0;
    ipcom_memcpy(ftpbuf, appdata, *applen);
    cp = ftpbuf;
    cp += ipcom_strlen(pasv_rep);

    if(*cp++ != '(')
        return 0;
    /* Count the commas */
    num_commas = 0;
    while(*cp != '\0')
    {
        if(*cp == ',')
        {
            num_commas++;
            if(num_commas < 4)
                *cp = '.';
            else if(num_commas == 4)
                *cp = '\0';
        }
        cp++;
    }
    if(num_commas != 5)
        return 0;

    /* Get the ip address */
    cp = ftpbuf;
    cp += ipcom_strlen(pasv_rep)+1;
    if(ipcom_inet_pton(IP_AF_INET, cp, &src_addr_n) != 1)
        return 0;

    /* Get most significant part of the port */
    cp += ipcom_strlen(cp)+1;
    tmp = ipcom_strtol(cp, &suff, 10);
    if(tmp < 0 || tmp > 255 || cp == suff || *suff != ',')
        return 0;
    port_msb = (Ip_u8)tmp;
    cp = suff + 1;

    /* Get least significant part of the port */
    tmp = ipcom_strtol(cp, &suff, 10);
    if(tmp < 0 || tmp > 255 || cp == suff || *suff != ')')
        return 0;
    port_lsb = (Ip_u8)tmp;
    port = (Ip_u16)((port_msb<<8) + port_lsb);
    cp = suff + 1;

    /* Add a mapping for the ftp data connection */
    ipcom_memset(&proxy_tuple, 0, sizeof(proxy_tuple));
    proxy_tuple.protocol     = param->tuple.protocol;
    proxy_tuple.private_addr = ip_ntohl(src_addr_n);
    proxy_tuple.private_port = port;
    proxy_tuple.public_addr  = param->tuple.public_addr;
    tmp = ipnet_nat_proxy_add_mapping(&proxy_tuple,
                                      0,
                                      param->mapping,
                                      IP_TRUE,         /* Use port translation */
                                      IP_TRUE,         /* Inbound session */
                                      IP_NULL,
                                      IP_NULL);

    if (tmp < 0)
        return 0;
    port_msb = (Ip_u8)((Ip_u16)tmp >> 8);
    port_lsb = (Ip_u8)tmp;

    /* Modify the buffer */
    cp = ftpbuf;
    cp += ipcom_strlen(pasv_rep);
    *cp++ = '(';
    if(ipcom_inet_ntop(IP_AF_INET, &nat_addr_n, cp, 16) == IP_NULL)
        return 0;
    while(*cp != 0)
    {
        if(*cp == '.')
            *cp = ',';
        cp++;
    }
    *cp++ = ',';
    cp += ipcom_sprintf(cp, "%d", port_msb);
    *cp++ = ',';
    cp += ipcom_sprintf(cp, "%d", port_lsb);
    *cp++ = ')';
    *cp++ = '\r';
    *cp++ = '\n';
    *cp = '\0';
    newlen = ipcom_strlen(ftpbuf);

    /* Update application data with the modified buffer */
    diff = newlen - *applen;
    if (diff > growspace)
    {
        /* Must allocate a new buffer */
        *newdata = ipcom_malloc(*applen + diff);
        if (*newdata == IP_NULL)
        {
            IPCOM_LOG1(ERR, "ipnet_nat_proxy_ftp_port_cmd() :: ipcom_malloc(%d) failed",
                            *applen + diff);
            return -1;
        }
        ipcom_memcpy(*newdata, ftpbuf, newlen);
    }
    else
    {
        /* Let the current buffer grow */
        ipcom_memcpy(appdata, ftpbuf, newlen);
    }
    *applen = newlen;

    IPCOM_LOG2(DEBUG, "ipnet_nat_proxy_ftp_pasv_rep() :: new cmd \"%s\" diff=%d", ftpbuf, diff);
    return 1;
}


/*
 *===========================================================================
 *                    ipnet_nat_proxy_ftp_extpasv_rep
 *===========================================================================
 * Description: Modify the EPSV reply.
 * Parameters:  appdata   - pointer to application data.
 *              applen    - pointer to length of application data.
 *              growspace - space available to extend application data.
 *              param     - pointer to proxy parameters.
 *              newdata   - pointer to pointer to new application data.
 * Returns:     1 = Packet modified.
 *              0 = Packet untouched.
 */
IP_STATIC int
ipnet_nat_proxy_ftp_extpasv_rep(Ip_u8 *appdata,
                                int   *applen,
                                int    growspace,
                                Ipnet_nat_proxy_param *param,
                                Ip_u8 **newdata)
{
    Ipnet_nat_proxy_tuple proxy_tuple;
    char *suff, *cp;
    Ip_u16 port;
    int tmp, i, diff, newlen;

    /* Copy the the 'EPSV reply' command */
    ipcom_memset(ftpbuf, 0, sizeof(ftpbuf));
    if(*applen >= (int)sizeof(ftpbuf))
        return 0;
    ipcom_memcpy(ftpbuf, appdata, *applen);
    cp = ftpbuf;
    cp += ipcom_strlen(extpasv_rep);

    /* Verify EPSV reply */
    if(*cp++ != '(')
        return 0;
    for(i=0; i<3; i++)
        if(*cp++ != FTP_DELIMITER)
            return 0;
    /* Get the port */
    tmp = ipcom_strtol(cp, &suff, 10);
    if(tmp < 0 || tmp > 0xffff || cp == suff || *suff != FTP_DELIMITER)
        return 0;
    port = (Ip_u16)tmp;
    cp = suff + 1;
    if(*cp++ != ')')
        return 0;

    /* Add a mapping for the ftp data connection */
    ipcom_memset(&proxy_tuple, 0, sizeof(proxy_tuple));
    proxy_tuple.protocol     = param->tuple.protocol;
    proxy_tuple.private_addr = param->tuple.private_addr;
    proxy_tuple.private_port = port;
    proxy_tuple.public_addr  = param->tuple.public_addr;
    tmp = ipnet_nat_proxy_add_mapping(&proxy_tuple,
                                      0,
                                      param->mapping,
                                      IP_TRUE,         /* Use port translation */
                                      IP_TRUE,         /* Inbound session */
                                      IP_NULL,
                                      IP_NULL);

    if (tmp < 0)
        return 0;
    port = (Ip_u16)tmp;

    /* Modify the buffer */
    cp = ftpbuf;
    cp += ipcom_strlen(extpasv_rep);
    *cp++ = '(';
    for(i=0; i<3; i++)
        *cp++ = FTP_DELIMITER;
    cp += ipcom_sprintf(cp, "%d", port);
    *cp++ = FTP_DELIMITER;
    *cp++ = ')';
    *cp++ = '\r';
    *cp++ = '\n';
    *cp = '\0';
    newlen = ipcom_strlen(ftpbuf);

    /* Update application data with the modified buffer */
    diff = newlen - *applen;
    if (diff > growspace)
    {
        /* Must allocate a new buffer */
        *newdata = ipcom_malloc(*applen + diff);
        if (*newdata == IP_NULL)
        {
            IPCOM_LOG1(ERR, "ipnet_nat_proxy_ftp_extpasv_rep() :: ipcom_malloc(%d) failed",
                            *applen + diff);
            return -1;
        }
        ipcom_memcpy(*newdata, ftpbuf, newlen);
    }
    else
    {
        /* Let the current buffer grow */
        ipcom_memcpy(appdata, ftpbuf, newlen);
    }
    *applen = newlen;

    IPCOM_LOG2(DEBUG, "ipnet_nat_proxy_ftp_extpasv_rep() :: new cmd \"%s\" diff=%d", ftpbuf, diff);
    return 1;
}


/*
 ****************************************************************************
 * 10                    GLOBAL FUNCTIONS
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 11                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipnet_nat_proxy_ftp
 *===========================================================================
 * Description: NAT proxy for the FTP protocol.
 * Parameters:  newhdr    - pointer to optionally write new IP header.
 *              appdata   - pointer to application data.
 *              applen    - pointer to length of application data.
 *              growspace - space available to extend application data.
 *              param     - pointer to proxy parameters.
 *              newdata   - pointer to pointer to new application data.
 * Returns:     1 = Packet modified.
 *              0 = Packet untouched.
 *             -1 = Drop packet.
 */
IP_PUBLIC int
ipnet_nat_proxy_ftp(Ip_u8 *newhdr,
                    Ip_u8 *appdata,
                    int   *applen,
                    int    growspace,
                    Ipnet_nat_proxy_param *param,
                    Ip_u8 **newdata)
{
    (void)newhdr;

    /* Cannot handle fragments other than the first */
    if (param->fragoff != 0)
        return 0;

    /* FTP proxy only applicable for outgoing packets */
    if(param->incoming == IP_FALSE)
    {
        if (param->inbound == IP_FALSE)
        {
            /* Check PORT command */
            if(*applen >= (int)ipcom_strlen(port_cmd) &&
                ipcom_strncasecmp((char *)appdata, port_cmd, ipcom_strlen(port_cmd)) == 0)
            {
                return ipnet_nat_proxy_ftp_port_cmd(appdata,
                                                    applen,
                                                    growspace,
                                                    param,
                                                    newdata);
            }

            /* Check EPRT command */
            if(*applen >= (int)ipcom_strlen(extport_cmd) &&
                ipcom_strncasecmp((char *)appdata, extport_cmd, ipcom_strlen(extport_cmd)) == 0)
            {
                return ipnet_nat_proxy_ftp_extport_cmd(appdata,
                                                       applen,
                                                       growspace,
                                                       param,
                                                       newdata);
            }

            /* Check EPSV command */
            if(*applen >= (int)ipcom_strlen(extpasv_cmd) &&
                ipcom_strncasecmp((char *)appdata, extpasv_cmd, ipcom_strlen(extpasv_cmd)) == 0)
            {
                return ipnet_nat_proxy_ftp_extpasv_cmd(appdata,
                                                       applen,
                                                       growspace,
                                                       param,
                                                       newdata);
            }
        }
        else
        {
            /* Check PASV reply */
            if(*applen >= (int)ipcom_strlen(pasv_rep) &&
                ipcom_strncasecmp((char *)appdata, pasv_rep, ipcom_strlen(pasv_rep)) == 0)
            {
                return ipnet_nat_proxy_ftp_pasv_rep(appdata,
                                                    applen,
                                                    growspace,
                                                    param,
                                                    newdata);
            }

            /* Check EPSV reply */
            if(*applen >= (int)ipcom_strlen(extpasv_rep) &&
                ipcom_strncasecmp((char *)appdata, extpasv_rep, ipcom_strlen(extpasv_rep)) == 0)
            {
                return ipnet_nat_proxy_ftp_extpasv_rep(appdata,
                                                       applen,
                                                       growspace,
                                                       param,
                                                       newdata);
            }
        }
    }
    return 0;
}

#else
int ipnet_nat_ftpproxy_empty_file;
#endif /* IPNET_USE_NAT */


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
