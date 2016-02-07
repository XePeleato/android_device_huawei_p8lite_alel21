/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_nat_sipproxy.c,v $ $Revision: 1.3 $
 *     $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_nat_sipproxy.c,v $
 *     $Author: jonas $
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
 * Proxy (ALG) for the SIP protocol.
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
#include <ipcom_sock2.h>
#include <ipcom_inet.h>
#include <ipcom_syslog.h>
#include <ipcom_list.h>

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

/* SIP macros */
#define SIP_IS_STRING_SAME(ptr, str) (ipcom_memcmp(ptr,str,ipcom_strlen(str)) == 0)
#define SIP_TAG_TO_DATAPOS(ptr)      (ipcom_strlen(ptr) + 1)
#define SIP_SKIP_SPACES(ptr)          while (*(char *)(ptr) == ' ') \
                                         (char *)(ptr)++

/* SIP strings */
#define SIP_SIP_STR     "SIP"
#define SIP_200_OK_STR  "200 OK"
#define SIP_CONTACT_STR "Contact"
#define SIP_CONTLEN_STR "Content-Length"
#define SIP_INVITE_STR  "INVITE"
#define SIP_VIA_STR     "Via"
#define SIP_CONTYPE_STR "Content-Type"
#define SIP_APPSDP_STR  "application/sdp"
#define SIP_ACK_STR     "ACK"
#define SIP_BYE_STR     "BYE"
#define SIP_CANCEL_STR  "CANCEL"
#define SIP_MESSAGE_STR "MESSAGE"
#define SIP_CSEQ_STR    "CSeq"
#define SIP_CALLID_STR  "Call-ID"
#define SIP_AUDIO_STR   "m=audio"
#define SIP_RTCP_STR    "a=rtcp"

/* SIP message type */
#define SIP_MSG_TYPE_STATUS      1
#define SIP_MSG_TYPE_INVITE      2
#define SIP_MSG_TYPE_BYE         4
#define SIP_MSG_TYPE_ACK         8
#define SIP_MSG_TYPE_MESSAGE     16

/* SIP constants */
#define SIP_DEFAULT_PORT          5060
#define SIP_VIA_FIELD_IPADDR_DIS    14
#define SIP_CTAC_FIELD_IPADDR_DIS    7

#define SIP_ADDRESS_PORT_STRING      1
#define SIP_PORT_STRING              2

#define SIP_SEARCH_BY_ADDR_PORT      1
#define SIP_SEARCH_BY_CALLID         2

/* SIP timeouts in seconds */
#define IPNET_NAT_SIP_ENTRY_ESTABLISHED_TIMEOUT    24*60*60
#define IPNET_NAT_SIP_ENTRY_INVITE_TIMEOUT         2*60
#define IPNET_NAT_SIP_ENTRY_BYE_TIMEOUT            30
#define IPNET_NAT_SIP_ENTRY_MEDIA_TIMEOUT          5*60


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

IP_STATIC Ip_u8 sipbuf[1472];


/*
 ****************************************************************************
 * 9                   LOCAL FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipnet_nat_proxy_sip_faststrsearch
 *===========================================================================
 * Description: Search using the Brute Force algorithm
 *              The Brute Force algorithm is the simplest string search
 *              algorithm. It performs comparisons between a character in
 *              the pattern and a character in the text buffer from left to
 *              right. After each attempt it shifts the pattern by one position
 *              to the right. The Brute Force algorithm requires no
 *              pre-processing and no extra space. It has a O(Pattern Length
 *              x Text Buffer Length) worst-case time complexity.
 * Parameters:  pattern        - pattern to search for
 *              pattern_len    - length of the pattern
 *              buffer         - text buffer to search in
 *              buffer_len     - length of the text buffer
 *              case_sensitive - case-sensitive search
 * Returns:     A pointer to the located pattern, or a IP_NULL pointer if the
 *              pattern is not found
 */
IP_STATIC char *
ipnet_nat_proxy_sip_faststrsearch(char   *pattern,
                                  int     pattern_len,
                                  char   *buffer,
                                  int     buffer_len,
                                  Ip_bool case_sensitive)
{
    char *p_pattern;
    char *p_buffer;
    char *p_buffer_compare;
    char *p_pattern_end = pattern + pattern_len;
    char *p_buffer_end = buffer + buffer_len - pattern_len + 1;

    /* Validation check */

    if ((pattern == IP_NULL) || (pattern_len == 0) ||
        (buffer == IP_NULL) || (buffer_len == 0) ||
        (pattern_len > buffer_len))
    {
        return IP_NULL;
    }

    for (p_buffer = buffer; p_buffer < p_buffer_end; p_buffer++)
    {
        if (case_sensitive == IP_TRUE)
        {
            for (p_pattern = pattern, p_buffer_compare = p_buffer;
                 (p_pattern < p_pattern_end) && (*p_pattern == *p_buffer_compare);
                 p_pattern++, p_buffer_compare++)
                     ;
        }
        else
        {
            for (p_pattern = pattern, p_buffer_compare = p_buffer;
                 (p_pattern < p_pattern_end) &&
                 (ipcom_toupper((int)*p_pattern) == ipcom_toupper((int)*p_buffer_compare));
                 p_pattern++, p_buffer_compare++)
                     ;
        }

        if (p_pattern == p_pattern_end)
            return p_buffer;  /* match */
    }

    /* no match */
    return IP_NULL;
}


/*
 *===========================================================================
 *                    ipnet_nat_proxy_sip_keyfind
 *===========================================================================
 * Description: Find a string in a structured message.
 *              This routine searches a string in a structured message.
 *              It assumes the searched string always starts at the beginning
 *              of each line of data terminated by \r\n characters. A message
 *              should be terminated by \r\n preceded by the line terminator
 *              \r\n.
 * Parameters:  pdata     - pointer to message.
 *              pstr      - key to look for.
 *              ppos      - found string position relative to pdata.
 *              searchlen - searchlen.
 * Returns:     IP_TRUE  = found key.
 *              IP_FALSE = key not found.
 */
IP_STATIC Ip_bool
ipnet_nat_proxy_sip_keyfind(char *pdata,
                            char *pstr,
                            int  *ppos,
                            int  searchlen)
{
    char *pstart = pdata;
    int        i = 0;

    if (!searchlen)
        searchlen = 1000;  /* max default search length */

    while (IP_TRUE)
    {
        if (pstr)
        {
            /* skip all space */
            while (*pdata == ' ')
                pdata++;

            if (ipcom_memcmp(pdata, pstr, ipcom_strlen(pstr)) == 0)
                break;
        }

        /* advance to next line */
        for (i = 0; i < (searchlen - 1); i++)
        {
            if (pdata[i] == '\r')
                if (pdata[i+1] == '\n')
                    break;
        }

        if (i >= (searchlen - 1))
        {
            IPCOM_LOG1(ERR, "ipnet_nat_proxy_sip_keyfind() :: single message exceeds max len: %d", searchlen);
            *ppos = searchlen;  /* reaches the end */
            return IP_FALSE;
        }

        pdata += i + 2;
        searchlen -= (i + 2);

        if ((pdata[0] == '\r' && pdata[1] == '\n') || (searchlen <= 0))
        {
            /* reach the end mark \r\n\r\n */
            if (searchlen)
                pdata += 2;

            *ppos = pdata - pstart;

            return (pstr ? IP_FALSE : IP_TRUE);
        }
    }

    *ppos = pdata - pstart;
    return IP_TRUE;
}


/*
 *===========================================================================
 *                    ipnet_nat_proxy_sip_outboundmsgtypecheck
 *===========================================================================
 * Description: Find the type of outbound SIP message
 * Parameters:  psipmsg - pointer to buffer message
 *              ptype   - pointer to buffer for returned status
 * Returns:     Pointer to message after the type or IP_NULL
 *              if type not found.
 */
IP_STATIC char *
ipnet_nat_proxy_sip_outboundmsgtypecheck(char *psipmsg, int *ptype, int searchlen)
{
    int pos;

    /* check if the first 3 char is "SIP" */
    if (SIP_IS_STRING_SAME(psipmsg, SIP_SIP_STR))
    {
        /* yes, it is, check if it is status code 200 */
        psipmsg += 8;

        if (SIP_IS_STRING_SAME(psipmsg, SIP_200_OK_STR))
        {
            /* this is the status 200 OK message */
            psipmsg += ipcom_strlen(SIP_200_OK_STR);

            *ptype = SIP_MSG_TYPE_STATUS;

            /* check if the BYE code */
            if (ipnet_nat_proxy_sip_keyfind(psipmsg, SIP_CSEQ_STR, &pos, searchlen) == IP_TRUE)
            {
                char * pbye;

                pbye = psipmsg + pos + SIP_TAG_TO_DATAPOS(SIP_CSEQ_STR);

                SIP_SKIP_SPACES(pbye);

                /* skip the number field */
                while (*pbye != ' ')
                    pbye++;

                SIP_SKIP_SPACES(pbye);

                if (SIP_IS_STRING_SAME (pbye, SIP_BYE_STR))
                {
                    IPCOM_LOG0(DEBUG, "ipnet_nat_proxy_sip_outboundmsgtypecheck() :: STATUS(BYE) message");
                    *ptype = SIP_MSG_TYPE_BYE;
                }
                else if (SIP_IS_STRING_SAME (pbye, SIP_CANCEL_STR))
                {
                    IPCOM_LOG0(DEBUG, "ipnet_nat_proxy_sip_outboundmsgtypecheck() :: STATUS(BYE) message");
                    *ptype = SIP_MSG_TYPE_BYE;
                }
            }
            return psipmsg;
        }
        return IP_NULL;
    }

    /* not a status OK message, check it a INVITE message */
    if (SIP_IS_STRING_SAME(psipmsg, SIP_INVITE_STR))
    {

        IPCOM_LOG0(DEBUG, "ipnet_nat_proxy_sip_outboundmsgtypecheck() :: INVITE message");
        psipmsg += ipcom_strlen(SIP_INVITE_STR);
        *ptype = SIP_MSG_TYPE_INVITE;
        return psipmsg;
    }
    else if (SIP_IS_STRING_SAME(psipmsg, SIP_ACK_STR))
    {
        IPCOM_LOG0(DEBUG, "ipnet_nat_proxy_sip_outboundmsgtypecheck() :: ACK message");
        psipmsg += ipcom_strlen(SIP_ACK_STR);
        *ptype = SIP_MSG_TYPE_ACK;
        return psipmsg;
    }
    else if (SIP_IS_STRING_SAME(psipmsg, SIP_BYE_STR))
    {
        IPCOM_LOG0(DEBUG, "ipnet_nat_proxy_sip_outboundmsgtypecheck() :: BYE message");
        psipmsg += ipcom_strlen(SIP_BYE_STR);
        *ptype = SIP_MSG_TYPE_BYE;
        return psipmsg;
    }
    else if (SIP_IS_STRING_SAME(psipmsg, SIP_CANCEL_STR))
    {
        IPCOM_LOG0(DEBUG, "ipnet_nat_proxy_sip_outboundmsgtypecheck() :: Cancel message");
        psipmsg += ipcom_strlen(SIP_CANCEL_STR);
        *ptype = SIP_MSG_TYPE_BYE;
        return psipmsg;
    }
    else if (SIP_IS_STRING_SAME(psipmsg, SIP_MESSAGE_STR))
    {
        IPCOM_LOG0(DEBUG, "ipnet_nat_proxy_sip_outboundmsgtypecheck() :: MESSAGE message");
        psipmsg += ipcom_strlen(SIP_MESSAGE_STR);
        *ptype = SIP_MSG_TYPE_MESSAGE;
        return psipmsg;
    }

    return IP_NULL;  /* not the SIP messages we want */
}


/*
 *===========================================================================
 *                    ipnet_nat_proxy_sip_inboundmsgtypecheck
 *===========================================================================
 * Description: Find the type of inbound SIP message
 * Parameters:  psipmsg - pointer to buffer message
 *              ptype   - pointer to buffer for returned status
 * Returns:     Pointer to message after the type or IP_NULL
 *              if type not found.
 */
IP_STATIC char *
ipnet_nat_proxy_sip_inboundmsgtypecheck(char *psipmsg, int *ptype, int searchlen)
{
    int pos;

    /* check if the first 3 char is "SIP" */
    if (SIP_IS_STRING_SAME(psipmsg, SIP_SIP_STR))
    {
        /* yes, it is, check if it is status code 200 */
        psipmsg += 8;

        if (SIP_IS_STRING_SAME(psipmsg, SIP_200_OK_STR))
        {
            /* this is the status 200 OK message */
            psipmsg += ipcom_strlen(SIP_200_OK_STR);

            *ptype = SIP_MSG_TYPE_STATUS;

            /* check if the INVITE or BYE code */
            if (ipnet_nat_proxy_sip_keyfind(psipmsg, SIP_CSEQ_STR, &pos, searchlen) == IP_TRUE)
            {
                char * pbye;

                pbye = psipmsg + pos + SIP_TAG_TO_DATAPOS(SIP_CSEQ_STR);

                SIP_SKIP_SPACES(pbye);

                /* skip the number field */
                while (*pbye != ' ')
                    pbye++;

                SIP_SKIP_SPACES(pbye);

                if (SIP_IS_STRING_SAME (pbye, SIP_INVITE_STR))
                {
                    IPCOM_LOG0(DEBUG, "ipnet_nat_proxy_sip_inboundmsgtypecheck() :: STATUS(INVITE) message");
                    *ptype = SIP_MSG_TYPE_INVITE;
                }
                else if (SIP_IS_STRING_SAME (pbye, SIP_BYE_STR))
                {
                    IPCOM_LOG0(DEBUG, "ipnet_nat_proxy_sip_inboundmsgtypecheck() :: STATUS(BYE) message");
                    *ptype = SIP_MSG_TYPE_BYE;
                }
                else if (SIP_IS_STRING_SAME (pbye, SIP_CANCEL_STR))
                {
                    IPCOM_LOG0(DEBUG, "ipnet_nat_proxy_sip_inboundmsgtypecheck() :: STATUS(BYE) message");
                    *ptype = SIP_MSG_TYPE_BYE;
                }
            }
            return psipmsg;
        }
        return IP_NULL;
    }

    if (SIP_IS_STRING_SAME(psipmsg, SIP_BYE_STR))
    {
        IPCOM_LOG0(DEBUG, "ipnet_nat_proxy_sip_inboundmsgtypecheck() :: BYE message");
        psipmsg += ipcom_strlen(SIP_BYE_STR);
        *ptype = SIP_MSG_TYPE_BYE;
        return psipmsg;
    }
    else if (SIP_IS_STRING_SAME(psipmsg, SIP_MESSAGE_STR))
    {
        IPCOM_LOG0(DEBUG, "ipnet_nat_proxy_sip_inboundmsgtypecheck() :: MESSAGE message");
        psipmsg += ipcom_strlen(SIP_MESSAGE_STR);
        *ptype = SIP_MSG_TYPE_MESSAGE;
        return psipmsg;
    }

    return IP_NULL;  /* not the SIP messages we want */
}


/*
 *===========================================================================
 *                    ipnet_nat_proxy_sip_endmsgprocess
 *===========================================================================
 * Description: Cleanup the resource created for a given SIP session
 * Parameters:
 * Returns:     nothing.
 */
IP_STATIC void
ipnet_nat_proxy_sip_endmsgprocess(char *pcallid, Ipnet_nat_proxy_param *param)
{
    if (pcallid != IP_NULL)
        IPCOM_LOG1(DEBUG, "ipnet_nat_proxy_sip_endmsgprocess() :: callid=%s", pcallid);
    if (param != IP_NULL)
    {
        /* Update the mapping timeout */
        IPCOM_LOG1(DEBUG, "ipnet_nat_proxy_sip_endmsgprocess() :: setting mapping timeout to %d seconds",
                   IPNET_NAT_SIP_ENTRY_BYE_TIMEOUT);
        ipnet_nat_proxy_set_mapping_timeout(IPNET_NAT_SIP_ENTRY_BYE_TIMEOUT, param->mapping);
    }
}


/*
 *===========================================================================
 *                    ipnet_nat_proxy_sip_addrportstrparse
 *===========================================================================
 * Description: Parse the address/port string
 *              This routine parses a combination of address and port string.
 *              The address part can be terminated by ':', '\r' or '>'
 *              characters.
 * Parameters:  pstr    - pointer to message
 *              pipaddr - buffer for parsed IP address
 *              pport   - buffer for parsed port
 *              type    - type of search
 * Returns:     The next character position after the parsed string or
 *	            IP_NULL if parse fails
 */
IP_STATIC char *
ipnet_nat_proxy_sip_addrportstrparse(char   *pstr,
                                     Ip_u32 *pipaddr,
                                     int    *pport,
                                     int     type)
{
    int      i;
    char    *pend;
    Ip_bool  noport = IP_FALSE;
    char     tmpholder[20];

    if (type == SIP_ADDRESS_PORT_STRING)
    {
        for (i = 0; i < 20; i++)
        {
            if (pstr[i] == ':' || pstr[i] == '\r' || pstr[i] == '>')
                break;
            else
                tmpholder[i] = pstr[i];
        }

        if (i >= 20 || i == 0)
        {
            IPCOM_LOG0(ERR, "ipnet_nat_proxy_sip_payload_parse() :: ERROR: invalid string");
            return IP_NULL;
        }

        /* If the address terminated by these characters, then the parsed
         * string is considered to contain only the address informantion.
         */
        if (pstr[i] == '\r' || pstr[i] == '>')
            noport = IP_TRUE;

        tmpholder[i] = 0;

        *pipaddr = ipcom_inet_addr(tmpholder);
        if (*pipaddr == IP_INADDR_NONE)
            return IP_NULL;

        *pipaddr = ip_ntohl(*pipaddr);
        pstr += i;
    }

    if (type == SIP_PORT_STRING || (type == SIP_ADDRESS_PORT_STRING && noport == IP_FALSE))
    {
        if (type == SIP_ADDRESS_PORT_STRING)
            pstr++;

        /* find the port number */
        *pport = ipcom_strtol(pstr, &pend, 10);
        pstr = pend;
    }
    else
        *pport = 0;

    return pstr;
}


/*
 *===========================================================================
 *                    ipnet_nat_proxy_sip_callidstr
 *===========================================================================
 * Description: Extract the callid string
 * Parameters:  pdata - pointer to message.
 * Returns:     pointer to allocated buffer with callid string or
 *              IP_NULL if wrong format or out of memory.
 */
IP_STATIC char *
ipnet_nat_proxy_sip_callidstr(char *pmsg)
{
    char *pstart;
    char *pcallid;
    int   i;

    SIP_SKIP_SPACES(pmsg);

    pstart = pmsg;
    for (i = 0; i < 200; i++)
    {
        if (*pmsg != '\r')
            pmsg++;
        else
            break;
    }

    /* in case we have a wrong string format */
    if (i >= 200)
    {
        IPCOM_LOG0(ERR, "ipnet_nat_proxy_sip_callidstr() :: ERROR, wrong string format");
        return IP_NULL;
    }

    if ((pcallid = ipcom_malloc((pmsg - pstart) + 1)) == IP_NULL)
        return IP_NULL;

    ipcom_memcpy(pcallid, pstart, pmsg - pstart);
    *(pcallid + (pmsg - pstart)) = 0;

    IPCOM_LOG1(DEBUG, "ipnet_nat_proxy_sip_callidstr() :: callid=%s", pcallid);
    return pcallid;
}


/*
 *===========================================================================
 *                    ipnet_nat_proxy_sip_modmsg
 *===========================================================================
 * Description: Modify message and adjust length
 * Parameters:  newdata    - pointer to new data
 *              newlen     - length of new data
 *              olddata    - pointer to old data
 *              oldlen     - length of old data
 *              ppend      - pointer to pointer to last byte of message
 * Returns:     0  = ok
 *              -1 = message too long
 */
IP_STATIC int
ipnet_nat_proxy_sip_modmsg(char *newdata, int newlen,
                           char *olddata, int oldlen,
                           char **ppend)
{
    char *pmax  = (char *)sipbuf + sizeof(sipbuf) - 1;
    int diff    = newlen - oldlen;
    int movelen = (*ppend  + 1) - (olddata + oldlen);

    ip_assert(movelen >= 0);
    if (pmax - *ppend < diff)
    {
        IPCOM_LOG0(ERR, "ipnet_nat_proxy_sip_modmsg() :: message to long");
        return -1;
    }

    /* Make space for new data */
    ipcom_memmove(olddata + newlen, olddata + oldlen, movelen);
    *ppend += diff;

    /* Copy in new data */
    ipcom_memcpy(olddata, newdata, newlen);

    return 0;
}


/*
 *===========================================================================
 *                    ipnet_nat_proxy_sip_localaddrprocess
 *===========================================================================
 * Description: Process the private transport address
 * Parameters:  pmsgstr    - pointer to message
 *              pipaddrstr - pointer to buffer for the local address string
 *              pgaddrstr  - pointer to buffer for the gloab address string
 *              pcallid    - pointer to the call id string
 *              type       - type of search
 *              ppend      - pointer to pointer to last byte of message
 * Returns:     The next character position after the parsed string or
 *	            IP_NULL if parse fails
 */
IP_STATIC char *
ipnet_nat_proxy_sip_localaddrprocess(char *pmsgstr,
                                     char *pipaddrstr,
                                     char *pgaddrstr,
                                     char *pcallid,
                                     int  type,
                                     Ipnet_nat_proxy_param *param,
                                     char **ppend)
{
    int localport, newport;
    int len, diff;
    Ip_u32 ipaddr;
    char *pstart;
    char tmpholder[30];
    Ipnet_nat_proxy_tuple proxy_tuple;

    (void)pcallid;
    SIP_SKIP_SPACES(pmsgstr);

    pstart = pmsgstr;
    pmsgstr = ipnet_nat_proxy_sip_addrportstrparse(pmsgstr, &ipaddr, &localport, type);

    if (pmsgstr == IP_NULL)
        return IP_NULL;

    if (!localport)
        localport = SIP_DEFAULT_PORT;

    if (type == SIP_ADDRESS_PORT_STRING)
    {
        ipaddr = ip_htonl(ipaddr);
        (void)ipcom_inet_ntop(IP_AF_INET, &ipaddr, pipaddrstr, 16);
        ipaddr = ip_ntohl(ipaddr);
    }
    else  /* PORT string only */
    {
        ipaddr = ipcom_inet_addr(pipaddrstr);
        ipaddr = ip_ntohl(ipaddr);
    }

    /* make sure it is the private address, otherwise no translation */

    if (ipaddr != param->tuple.private_addr)
        return pmsgstr;

    /* create a bind entry for this transport address if it hasn't been created yet */
    if (param->tuple.private_port != localport)
    {
        ipcom_memset(&proxy_tuple, 0, sizeof(proxy_tuple));
        proxy_tuple.protocol     = IP_IPPROTO_UDP;
        proxy_tuple.private_addr = ipaddr;
        proxy_tuple.private_port = localport;
        newport = ipnet_nat_proxy_add_mapping(&proxy_tuple,
                                              IPNET_NAT_SIP_ENTRY_MEDIA_TIMEOUT,
                                              param->mapping,
                                              IP_FALSE,         /* Use port translation */
                                              IP_TRUE,          /* Inbound session */
                                              IP_NULL,
                                              IP_NULL);

        if (newport < 0)
        {
            IPCOM_LOG2(ERR, "ipnet_nat_proxy_sip_localaddrprocess() :: Failed to add mapping for address = 0x%08x, port = %d",
                       ipaddr, localport);
        }
        else
        {
            IPCOM_LOG2(DEBUG, "ipnet_nat_proxy_sip_localaddrprocess() :: Added mapping for address = 0x%08x, port = %d",
                       ipaddr, localport);
        }
    }
    else
    {
        newport = param->nat_port;
        IPCOM_LOG2(DEBUG, "ipnet_nat_proxy_sip_localaddrprocess() :: Mapping already existed for address = 0x%08x, port = %d",
                   ipaddr, localport);
    }

    /* compose the new modified string */

    len = 0;
    if (type == SIP_ADDRESS_PORT_STRING)
    {
        ipaddr = ip_htonl(param->nat_addr);
        (void)ipcom_inet_ntop(IP_AF_INET, &ipaddr, tmpholder, 16);
        ipcom_strcpy(pgaddrstr, tmpholder);
        len = ipcom_strlen(tmpholder);
        tmpholder[len++]= ':';
    }
    ipcom_sprintf(tmpholder + len, "%d", newport);

    IPCOM_LOG1(DEBUG, "ipnet_nat_proxy_sip_localaddrprocess() new str: %s", tmpholder);


    if (ipnet_nat_proxy_sip_modmsg(tmpholder, ipcom_strlen(tmpholder),
                                   pstart, pmsgstr - pstart, ppend) < 0)
    {
        return IP_NULL;
    }

    diff = ipcom_strlen(tmpholder) - (pmsgstr - pstart);
    return pmsgstr + diff;
}


/*
 *===========================================================================
 *                    ipnet_nat_proxy_sip_payload_parse
 *===========================================================================
 * Description: Parse SIP payload.
 * Parameters:  appdata   - pointer to application data.
 *              applen    - pointer to length of application data.
 *              growspace - space available to extend application data.
 *              param     - pointer to proxy parameters.
 *              newdata   - pointer to pointer to new application data.
 * Returns:     IP_TRUE  = Packet modified.
 *              IP_FALSE = Packet untouched.
 */
IP_STATIC Ip_bool
ipnet_nat_proxy_sip_payload_parse(Ip_u8 *appdata,
                                  int   *applen,
                                  int    growspace,
                                  Ipnet_nat_proxy_param *param,
                                  Ip_u8 **newdata)

{
    char    *psipmsg = (char *)appdata;
    char    *pcallid = IP_NULL;
    char    *pendstr, *pstartstr, *psearch;
    Ip_bool  retcode = IP_FALSE;
    int      pos, sdpdatalen, searchlen, msgtype = 0;
    char     tmpholder[50];
    char     laddrstr[20];
    char     gaddrstr[20];
    char    *pend = (char *)sipbuf + *applen - 1;
    char    *sdplen_start, *sdplen_end, *sdp_end;
    int      newlen, diff;

    if (param->incoming == IP_TRUE)
    {
        /* Incoming message*/
        goto parseFalseExit;
    }

    if (param->inbound == IP_FALSE)
    {
        /* Outbound session */
        if ((psipmsg = ipnet_nat_proxy_sip_outboundmsgtypecheck(psipmsg, &msgtype, *applen)) == IP_NULL)
            return IP_FALSE;   /* not the message we're looking for */
    }
    else
    {
        /* Inbound session */
        if ((psipmsg = ipnet_nat_proxy_sip_inboundmsgtypecheck(psipmsg, &msgtype, *applen)) == IP_NULL)
            return IP_FALSE;   /* not the message we're looking for */
    }

    /* find the call-id field */
    if (ipnet_nat_proxy_sip_keyfind(psipmsg, SIP_CALLID_STR, &pos, *applen - (psipmsg - (char *)appdata)) == IP_FALSE)
    {
        IPCOM_LOG0(ERR, "ipnet_nat_proxy_sip_payload_parse() :: no callid field");
        return IP_FALSE;
    }

    /* get callid string */
    pcallid = ipnet_nat_proxy_sip_callidstr(psipmsg + pos + SIP_TAG_TO_DATAPOS(SIP_CALLID_STR));
    if (pcallid == IP_NULL)
    {
        IPCOM_LOG0(ERR, "ipnet_nat_proxy_sip_payload_parse() :: failed to parse callid string");
        return IP_FALSE;
    }

    if (msgtype == SIP_MSG_TYPE_INVITE)
    {
        if (param->inbound == IP_FALSE)
        {
            /* Update the mapping timeout */
            IPCOM_LOG1(DEBUG, "ipnet_nat_proxy_sip_payload_parse() :: setting mapping timeout to %d seconds",
                       IPNET_NAT_SIP_ENTRY_INVITE_TIMEOUT);
            ipnet_nat_proxy_set_mapping_timeout(IPNET_NAT_SIP_ENTRY_INVITE_TIMEOUT, param->mapping);
        }
        else
        {
            /* ALG will not detect the ACK for inbound calls so use the INVITE response to set established timeout */
            IPCOM_LOG1(DEBUG, "ipnet_nat_proxy_sip_payload_parse() :: setting mapping timeout to %d seconds",
                       IPNET_NAT_SIP_ENTRY_ESTABLISHED_TIMEOUT);
            ipnet_nat_proxy_set_mapping_timeout(IPNET_NAT_SIP_ENTRY_ESTABLISHED_TIMEOUT, param->mapping);
        }
    }
    else if (msgtype == SIP_MSG_TYPE_ACK)
    {
        /* Update the mapping timeout */
        IPCOM_LOG1(DEBUG, "ipnet_nat_proxy_sip_payload_parse() :: setting mapping timeout to %d seconds",
                   IPNET_NAT_SIP_ENTRY_ESTABLISHED_TIMEOUT);
        ipnet_nat_proxy_set_mapping_timeout(IPNET_NAT_SIP_ENTRY_ESTABLISHED_TIMEOUT, param->mapping);
    }

    /* Copy the message to scratch buffer and set pointer */
    if (*applen > (int)sizeof(sipbuf))
    {
        IPCOM_LOG0(ERR, "ipnet_nat_proxy_sip_payload_parse() :: message to long");
        goto parseFalseExit;
    }
    ipcom_memcpy(sipbuf, appdata, *applen);
    psipmsg = (char *)sipbuf + (psipmsg - (char *)appdata);

    /* first the via field
     * For response message, the Via field will not contain the private address.
     * The localAddrProcess() will check if the address is private.
     */

    laddrstr[0] = gaddrstr[0] = 0;
    if (ipnet_nat_proxy_sip_keyfind(psipmsg, SIP_VIA_STR, &pos, (pend - psipmsg) + 1) == IP_TRUE)
	{
        /* advance to IP/Port string */

        psipmsg += (pos + ipcom_strlen(SIP_VIA_STR) + SIP_VIA_FIELD_IPADDR_DIS);

        IPCOM_LOG0(DEBUG, "ipnet_nat_proxy_sip_payload_parse() :: parse via field");

        psipmsg = ipnet_nat_proxy_sip_localaddrprocess(psipmsg, laddrstr, gaddrstr, pcallid,
                                                       SIP_ADDRESS_PORT_STRING, param, &pend);

        if (psipmsg == IP_NULL)
            goto parseFalseExit;

    }

    /* find the contact field */

    if (ipnet_nat_proxy_sip_keyfind (psipmsg, SIP_CONTACT_STR, &pos, (pend - psipmsg) + 1) == IP_TRUE)
    {
        /* advance to IP/Port string */

        psipmsg += (pos + ipcom_strlen(SIP_CONTACT_STR) + SIP_CTAC_FIELD_IPADDR_DIS);

        IPCOM_LOG0(DEBUG, "ipnet_nat_proxy_sip_payload_parse() :: parse contact field");

        psipmsg = ipnet_nat_proxy_sip_localaddrprocess(psipmsg, laddrstr, gaddrstr, pcallid,
                                                       SIP_ADDRESS_PORT_STRING, param, &pend);

        if (psipmsg == IP_NULL)
            goto parseFalseExit;
    }

    /* exit if there isn't any private info in SIP message header */

    if (laddrstr[0] == 0 || gaddrstr[0] == 0)
        goto parseFalseExit;

    /* When program reaches here, it indicates at least one place in the
     * packet should be modified. Use parseFalseExit only if it is an error
     */

    /* find the Content-Type field */

    if (ipnet_nat_proxy_sip_keyfind (psipmsg, SIP_CONTYPE_STR, &pos, (pend - psipmsg) + 1) == IP_FALSE)
        goto parseTrueExit;

    /* advance to start of data field */

    psipmsg += (pos + SIP_TAG_TO_DATAPOS(SIP_CONTYPE_STR));

    SIP_SKIP_SPACES(psipmsg);

    /*check the application/sdp type, if not, exit */

    if (!SIP_IS_STRING_SAME(psipmsg, SIP_APPSDP_STR))
        goto parseTrueExit;

    /* find the content length field */

    if (ipnet_nat_proxy_sip_keyfind (psipmsg, SIP_CONTLEN_STR, &pos, (pend - psipmsg) + 1) == IP_FALSE)
        goto parseTrueExit;

    IPCOM_LOG0(DEBUG, "ipnet_nat_proxy_sip_payload_parse() :: parse Content-Length field");

    /* reach to the content length field, advance to length data */

    psipmsg += (pos + SIP_TAG_TO_DATAPOS(SIP_CONTLEN_STR));

    SIP_SKIP_SPACES(psipmsg);

    sdpdatalen = ipcom_strtol(psipmsg, &pendstr, 10);

    IPCOM_LOG1(DEBUG, "ipnet_nat_proxy_sip_payload_parse() :: data length = %d", sdpdatalen);

    if (sdpdatalen == 0)
        goto parseTrueExit;

    /* store start and end of sdp datalength string as well as end of message */
    sdplen_start = psipmsg;
    sdplen_end   = pendstr;
    sdp_end      = pend;

    /* advance to the start of SDP data */

    if (ipnet_nat_proxy_sip_keyfind (psipmsg, IP_NULL, &pos, (pend - psipmsg) + 1) == IP_FALSE)
    {
        IPCOM_LOG0(DEBUG, "ipnet_nat_proxy_sip_payload_parse() :: Can't find header end mark");
        goto parseTrueExit;
    }

    /* start of the SIP data field */

    psipmsg += pos;
    searchlen = sdpdatalen;

    /* search the local IP address, and replace it with global address */

    pstartstr = psipmsg;
    while (IP_TRUE)
    {
        psearch = ipnet_nat_proxy_sip_faststrsearch(laddrstr, ipcom_strlen(laddrstr),
                                                    pstartstr, searchlen, IP_FALSE);
        if (psearch != IP_NULL)
        {
            IPCOM_LOG1(DEBUG, "ipnet_nat_proxy_sip_payload_parse() :: Local IP address in SDP: %s", laddrstr);

            if (ipnet_nat_proxy_sip_modmsg(gaddrstr, ipcom_strlen(gaddrstr),
                                           psearch, ipcom_strlen(laddrstr), &pend) < 0)
            {
                goto parseFalseExit;
            }

            searchlen = searchlen - (psearch + ipcom_strlen(gaddrstr) - pstartstr);
            pstartstr = psearch + ipcom_strlen(gaddrstr);
        }
        else
            break;
    }

    IPCOM_LOG1(DEBUG, "ipnet_nat_proxy_sip_payload_parse() :: the last search length %d", searchlen);

    /* search for the audio port */

    if (ipnet_nat_proxy_sip_keyfind (psipmsg, SIP_AUDIO_STR, &pos, sdpdatalen) == IP_TRUE)
    {
        /* advance to the port field */

        psipmsg += (pos + SIP_TAG_TO_DATAPOS(SIP_AUDIO_STR));

        SIP_SKIP_SPACES(psipmsg);
        ipnet_nat_proxy_sip_localaddrprocess(psipmsg, laddrstr, gaddrstr, pcallid, SIP_PORT_STRING, param, &pend);
    }

    /* search for the audio control port */

    if (ipnet_nat_proxy_sip_keyfind (psipmsg, SIP_RTCP_STR, &pos, sdpdatalen) == IP_TRUE)
	{
        /* advance to the port field */

        psipmsg += (pos + SIP_TAG_TO_DATAPOS(SIP_RTCP_STR));

        SIP_SKIP_SPACES(psipmsg);
        ipnet_nat_proxy_sip_localaddrprocess(psipmsg, laddrstr, gaddrstr, pcallid, SIP_PORT_STRING, param, &pend);
    }

    /* adjust the length node */

    sdpdatalen += pend - sdp_end;
    ipcom_sprintf(tmpholder, "%d", sdpdatalen);
    if (ipnet_nat_proxy_sip_modmsg(tmpholder, ipcom_strlen(tmpholder),
                                   sdplen_start, sdplen_end - sdplen_start, &pend) < 0)
    {
        goto parseFalseExit;
    }

parseTrueExit:
    /* Update application data with the modified buffer */
    newlen = pend - (char *)sipbuf + 1;
    diff = newlen - *applen;
    if (diff > growspace)
    {
        /* Must allocate a new buffer */
        *newdata = ipcom_malloc(*applen + diff);
        if (*newdata == IP_NULL)
        {
            IPCOM_LOG1(ERR, "ipnet_nat_proxy_sip_payload_parse() :: ipcom_malloc(%d) failed",
                            *applen + diff);
            goto parseFalseExit;
        }
        ipcom_memcpy(*newdata, sipbuf, newlen);
    }
    else
    {
        /* Let the current buffer grow */
        ipcom_memcpy(appdata, sipbuf, newlen);
    }
    *applen = newlen;
    IPCOM_LOG1(DEBUG, "ipnet_nat_proxy_sip_msg() :: the delta length = %d ", diff);
    retcode = IP_TRUE;

parseFalseExit:
    if (msgtype == SIP_MSG_TYPE_BYE)
        ipnet_nat_proxy_sip_endmsgprocess(pcallid, param);
    if (pcallid)
        ipcom_free(pcallid);
    return retcode;
}


/*
 *===========================================================================
 *                    ipnet_nat_proxy_sip_msg
 *===========================================================================
 * Description: Track SIP packets.
 * Parameters:  appdata   - pointer to application data.
 *              applen    - pointer to length of application data.
 *              growspace - space available to extend application data.
 *              param     - pointer to proxy parameters.
 *              newdata   - pointer to pointer to new application data.
 * Returns:     1 = Packet modified.
 *              0 = Packet untouched.
 *             -1 = Drop packet.
 */
IP_STATIC int
ipnet_nat_proxy_sip_msg(Ip_u8 *appdata,
                        int   *applen,
                        int    growspace,
                        Ipnet_nat_proxy_param *param,
                        Ip_u8 **newdata)

{
    Ip_bool status;
    int ret = 1;

    IPCOM_LOG5(DEBUG, "ipnet_nat_proxy_sip_msg(%s:%s) :: BEGIN public=0x%x, private=0x%x len=%d",
                       param->incoming == IP_TRUE ? "INCOMING" : "OUTGOING",
                       param->inbound == IP_TRUE ? "INBOUND" : "OUTBOUND",
                       param->tuple.public_addr,
                       param->tuple.private_addr,
                       *applen);

    /* Inspect the payload */
    status = ipnet_nat_proxy_sip_payload_parse(appdata, applen, growspace, param, newdata);
    if (status == IP_FALSE)
    {
        IPCOM_LOG0(DEBUG, "ipnet_nat_proxy_sip_msg() :: message header did not match, no translation");
        ret = 0;
    }

    IPCOM_LOG5(DEBUG, "ipnet_nat_proxy_sip_msg(%s:%s) :: END public=0x%x, private=0x%x len=%d",
                       param->incoming == IP_TRUE ? "INCOMING" : "OUTGOING",
                       param->inbound == IP_TRUE ? "INBOUND" : "OUTBOUND",
                       param->tuple.public_addr,
                       param->tuple.private_addr,
                       *applen);

    return ret;
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
 *                    ipnet_nat_proxy_sip
 *===========================================================================
 * Description: NAT proxy for the SIP protocol.
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
ipnet_nat_proxy_sip(Ip_u8 *newhdr,
                    Ip_u8 *appdata,
                    int   *applen,
                    int    growspace,
                    Ipnet_nat_proxy_param *param,
                    Ip_u8 **newdata)
{
    (void)newhdr;
    (void)growspace;
    (void)newdata;

    /* Cannot handle fragments other than the first */
    if (param->fragoff != 0)
        return 0;

    /*  proxy does not apply to NAT-PT rules */
    if(param->natpt == IP_TRUE)
        return 0;

    return ipnet_nat_proxy_sip_msg(appdata, applen, growspace, param, newdata);
}

#else
int ipnet_nat_sipproxy_empty_file;
#endif /* IPNET_USE_NAT */


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
