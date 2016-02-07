/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipnet_nat_dnsproxy.c,v $ $Revision: 1.12 $
 *     $Source: /home/interpeak/CVSRoot/ipnet2/src/ipnet_nat_dnsproxy.c,v $
 *     $Author: kenneth $
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
 * Proxy (ALG) for the DNS protocol.
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

#ifdef IPNET_USE_NAT_PT

#define IPCOM_USE_CLIB_PROTO
#include <ipcom_clib.h>
#include <ipcom_sock.h>
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

/* DNS protocol flag field */
#define IPNET_NAT_DNS_QR_FLAG       0x8000
#define IPNET_NAT_DNS_OPCODE_FLAG   0x7800
#define IPNET_NAT_DNS_AA_FLAG       0x0400
#define IPNET_NAT_DNS_TC_FLAG       0x0200
#define IPNET_NAT_DNS_RD_FLAG       0x0100
#define IPNET_NAT_DNS_RA_FLAG       0x0080
#define IPNET_NAT_DNS_ZERO_FLAG     0x0070
#define IPNET_NAT_DNS_RCODE_FLAG    0x000f

/* DNS protocol query types */
#define IPNET_NAT_DNS_QTYPE_A       1
#define IPNET_NAT_DNS_QTYPE_CNAME   5
#define IPNET_NAT_DNS_QTYPE_PTR     12
#define IPNET_NAT_DNS_QTYPE_AAAA    28

/* DNS protocol query class */
#define IPNET_NAT_DNS_QCLASS_INET   1

/* Timeout in seconds for DNS transactions */
#define IPNET_NAT_DNS_TRANS_TIMEOUT         10

/* Maximum number of active DNS transactions held by the ALG */
#define IPNET_NAT_DNS_TRANS_MAX_ENTRIES     16


/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

/*
 *===========================================================================
 *                         Ipnet_nat_dns_hdr
 *===========================================================================
 */
#include <ipcom_align16.h>
typedef IP_PACKED1 struct Ipnet_nat_dns_hdr_struct
{
    Ip_u16 id;              /* identification */
    Ip_u16 flags;           /* flags */
    Ip_u16 no_ques;         /* number of questions */
    Ip_u16 no_answ;         /* number of answers */
    Ip_u16 no_auth;         /* number of authority records */
    Ip_u16 no_addi;         /* number of resource records */
}
IP_PACKED2 Ipnet_nat_dns_hdr;
#include <ipcom_align16.h>


/*
 *===========================================================================
 *                         Ipnet_nat_dns_transaction
 *===========================================================================
 */
typedef struct Ipnet_nat_dns_transaction_struct
{
    Ipcom_list list;            /* list pointer */
    Ip_u16     id;              /* identification */
    Ip_u16     srcport;         /* source port */
    Ip_u32     dstaddr;         /* destination address */
    int        type;            /* qtype */
    Ip_u8      ptrname[80];     /* zone for PTR */
    void       *tmo;            /* pointer to timeout */
}
Ipnet_nat_dns_transaction;


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

IP_STATIC Ip_u8 dnsbuf[512];
IP_STATIC Ip_u8 dnsname[255];
IP_STATIC Ipcom_list ipnet_nat_proxy_dns_list;


/*
 ****************************************************************************
 * 9                   LOCAL FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipnet_nat_proxy_dns_transaction_timeout
 *===========================================================================
 * Description: Timeout handler for DNS transactions timeouts
 * Parameters:  cookie - a cookie that is set when the timeout is added.
 * Returns:     nothing.
 */
IP_STATIC void
ipnet_nat_proxy_dns_transaction_timeout(void *cookie)
{
    Ipnet_nat_dns_transaction *trans = cookie;

    IPCOM_LOG4(DEBUG, "ipnet_nat_proxy_dns_transaction_timeout() :: expired transaction:"
                      "id=%d port=%d addr=0x%08x type=%d",
                       trans->id, trans->srcport, trans->dstaddr, trans->type);
    ipcom_list_remove(&trans->list);
    ipcom_free(trans);
}


/*
 *===========================================================================
 *                    ipnet_nat_proxy_dns_add_transaction
 *===========================================================================
 * Description: Add a DNS transaction to the list of active transactions.
 * Parameters:  type    - DNS query type
 *              dns_hdr - pointer to the DNS protocol header.
 *              param   - pointer to NAT proxy parameters.
 * Returns:     pointer to the transaction or IP_NULL if failed to add.
 */
IP_STATIC Ipnet_nat_dns_transaction *
ipnet_nat_proxy_dns_add_transaction(int type,
                                    Ipnet_nat_dns_hdr *dns_hdr,
                                    Ipnet_nat_proxy_param *param,
                                    Ip_u8 *ptrname)
{
    Ipnet_nat_dns_transaction *trans;

    /* Check that there is room for another transaction */
    if(ipnet_nat_proxy_dns_list.size >= IPNET_NAT_DNS_TRANS_MAX_ENTRIES)
        return IP_NULL;
    trans = ipcom_malloc(sizeof(*trans));
    if (trans == IP_NULL)
        return IP_NULL;

    ipcom_memset(trans, 0, sizeof(*trans));
    trans->id      = (Ip_u16)IP_GET_NTOHS(&dns_hdr->id);
    trans->srcport = param->tuple.private_port;
    trans->dstaddr = param->tuple.public_addr;
    trans->type    = type;
    if (ptrname != IP_NULL)
        ipcom_strncpy((char *)trans->ptrname, (char *)ptrname, sizeof(trans->ptrname)-1);
    ipcom_list_insert_last(&ipnet_nat_proxy_dns_list, &trans->list);
    /* Add the timeout */
    if (ipnet_nat_proxy_timeout_schedule(IPNET_NAT_DNS_TRANS_TIMEOUT,
                                         ipnet_nat_proxy_dns_transaction_timeout,
                                         trans,
                                         &trans->tmo) < 0)
    {
        ipcom_list_remove(&trans->list);
        ipcom_free(trans);
        return IP_NULL;
    }
    return trans;
}


/*
 *===========================================================================
 *                    ipnet_nat_proxy_dns_remove_transaction
 *===========================================================================
 * Description: Remove a transaction from the list.
 * Parameters:  trans - pointer to the transaction.
 * Returns:     nothing.
 */
IP_STATIC void
ipnet_nat_proxy_dns_remove_transaction(Ipnet_nat_dns_transaction *trans)
{
    ipnet_nat_proxy_timeout_cancel(trans->tmo);
    ipcom_list_remove(&trans->list);
    ipcom_free(trans);
}


/*
 *===========================================================================
 *                    ipnet_nat_proxy_dns_find_transaction
 *===========================================================================
 * Description: Find a transaction in the list of transactions.
 * Parameters:  type    - DNS query type.
 *              dns_hdr - pointer to the DNS protocol header.
 *              param   - pointer to NAT proxy parameters.
 * Returns:     A pointer to transaction if found or else IP_NULL.
 */
IP_STATIC Ipnet_nat_dns_transaction *
ipnet_nat_proxy_dns_find_transaction(int type,
                                     Ipnet_nat_dns_hdr *dns_hdr,
                                     Ipnet_nat_proxy_param *param)
{
    Ipnet_nat_dns_transaction *trans;
    Ip_u16 id;

    id = (Ip_u16)IP_GET_NTOHS(&dns_hdr->id);
    for (trans = IPCOM_LIST_FIRST(&ipnet_nat_proxy_dns_list);
         trans != IP_NULL;
         trans = IPCOM_LIST_NEXT(&trans->list))
    {
        if (id == trans->id
            && param->tuple.private_port == trans->srcport
            && param->tuple.public_addr == trans->dstaddr
            && type == trans->type)
        {
            return trans;
        }
    }

    return IP_NULL;
}


/*
 *===========================================================================
 *                    ipnet_nat_proxy_dns_decode_name
 *===========================================================================
 * Description: Decode a DNS protocol encoded name to a domain name.
 * Parameters:  buf     - pointer to buffer with the encoded name.
 *              buflen  - size of buffer.
 *              offset  - the offset in buffer where the encoded name begins.
 *              name    - pointer to buffer for the decoded result.
 *              namelen - length of result buffer
 * Returns:     The number of parsed bytes or -1 if no name could be found.
 */
IP_STATIC int
ipnet_nat_proxy_dns_decode_name(Ip_u8 *buf, int buflen, int offset, Ip_u8 *name, int namelen)
{
    int i, j, len, prev;
    int parsed   = 0;
    int count    = 0;
    int ptr      = 0;

    i = offset;
    if (buflen - i < 1)
        return -1;                  /* Too little data */
    while (buf[i] != 0)
    {
        len = buf[i];
        if ((len & 0xc0) == 0xc0)
        {
            if (buflen - i < 2)
                return -1;          /* Too little data */
            prev = i;
            i = IP_GET_NTOHS(buf+i) & 0x3fff;
            if (i >= prev)
                return -1;          /* Do not allow forward pointers (avoid loops) */
            if (buflen - i < 1)
                return -1;          /* Too little data */
            if (!ptr)
                parsed += 2;
            ptr = 1;
            continue;
        }
        i++;
        if (buflen - i < len)
            return -1;              /* Too little data */
        for (j=0; j<len; j++)
        {
            if (count < namelen)
                name[count++] = buf[i];
            i++;
        }
        if (count < namelen)
            name[count++] = '.';
        if (buflen - i < 1)
            return -1;              /* Too little data */
        if (!ptr)
            parsed += len + 1;
    }

    if (!ptr)
        parsed++;                   /* Skip null terminator */
    if (count)
        name[count-1] = '\0';
    else
        name[0] = 0;
    return parsed;
}


/*
 *===========================================================================
 *                    ipnet_nat_proxy_dns_encode_name
 *===========================================================================
 * Description: Encode a domain name for the DNS protocol.
 * Parameters:  buf    - pointer to buffer with the encoded result.
 *              buflen - lenght of result buffer.
 *              offset - offset in result buffer where the encoded name begins.
 *              name   - the domain name to encode.
 * Returns:     The length of encoded name or -1 if failed.
 */
IP_STATIC int
ipnet_nat_proxy_dns_encode_name(Ip_u8 *buf, int buflen, int offset, Ip_u8 *name)
{
    int i, j, k, count;

    i = offset;
    k = 0;
    while (name[k] != '\0')
    {
        j = i++;                    /* Save index for length byte */
        count = 0;
        while (name[k] != '\0' && name[k] != '.')
        {
            if (buflen - i < 1)
                return -1;          /* Too small buffer */
            buf[i++] = name[k++];
            count++;
        }
        if (name[k] == '\0')
        {
            if (buflen - i < 1)
                return -1;          /* Too small buffer */
            buf[i++] = '\0';
        }
        else
            k++;
        buf[j] = count;
    }

    return i - offset;
}


/*
 *===========================================================================
 *                    ipnet_nat_proxy_dns_ptr_name
 *===========================================================================
 * Description: Convert an Internet address to a domain name.
 * Parameters:  buf    - buffer to store the domain name in.
 *              buflen - length of domain name buffer.
 *              addr   - pointer to Internet address.
 *              family - address family.
 *              zone   - top level domain.
 * Returns:     The length of the domain name or -1 if failed.
 */
IP_STATIC int
ipnet_nat_proxy_dns_ptr_name(Ip_u8 *buf, int buflen, Ip_u8 *addr, int family, Ip_u8 *zone)
{
    int i, j, second, zonelen;
    Ip_u8 tmp;

    zonelen = ipcom_strlen((char *)zone) + 1;
    i = 0;
    if (family == IP_AF_INET)
    {
        /* Reverse ipv4 address */
        for (j=3; j>=0; j--)
        {
            second = 0;

            /* possible hundreds digit */
            tmp = addr[j] / 100;
            if (tmp)
            {
                if (buflen - i < 1)
                    return -1;                  /* Too small buffer */
                buf[i++] = '0' + tmp;
                second = 1;
            }
            tmp = addr[j] % 100;

            /* possible tens digit */
            if ((tmp / 10) || second)
            {
                if (buflen - i < 1)
                    return -1;                  /* Too small buffer */
                buf[i++] = '0' + tmp / 10;
            }

            /* always print ones digit */
            if (buflen - i < 2)
                return -1;                      /* Too small buffer */
            buf[i++] = '0' + tmp % 10;
            buf[i++] = '.';
        }

        /* Append the zone */
        if (buflen - i < zonelen)
            return -1;                          /* Too small buffer */
        ipcom_strcpy((char *)&buf[i], (char *)zone);
        i += zonelen;
    }
    return i;
}


/*
 *===========================================================================
 *                    ipnet_nat_proxy_dns_parse_questions
 *===========================================================================
 * Description: Parses and modifies DNS questions in a DNS packet.
 * Parameters:  buf     - pointer to buffer with the DNS questions.
 *              buflen  - length of buffer with DNS questions.
 *              offset  - offset in the buffer where the DNS questions begin.
 *              newlen  - pointer to the length of the message if modified.
 *              dns_hdr - pointer to the DNS protocol header.
 *              param   - pointer to NAT proxy parameters.
 * Returns:     The number of bytes parsed or -1 if failed.
 */
IP_STATIC Ip_s32
ipnet_nat_proxy_dns_parse_questions(Ip_u8 *buf,
                                int buflen,
                                int offset,
                                int *newlen,
                                Ipnet_nat_dns_hdr *dns_hdr,
                                Ipnet_nat_proxy_param *param)
{
    int i, j, k, count, origoffset, numq, numa, request;
    Ip_u16 type, cla, flags;
    Ip_u8 addr[4];
    Ipnet_nat_dns_transaction *trans;
    int newbuflen = sizeof(dnsbuf);
    Ip_u8 c, *zone;

    numq = IP_GET_NTOHS(&dns_hdr->no_ques);
    if (numq != 1)
    {
        IPCOM_LOG0(WARNING, "ipnet_nat_proxy_dns_parse_questions() :: supports only one question per message");
        return -1;
    }
    numa = IP_GET_NTOHS(&dns_hdr->no_answ);
    flags = IP_GET_NTOHS(&dns_hdr->flags);
    request = (flags & IPNET_NAT_DNS_QR_FLAG) != 0 ? 0 : 1;
    origoffset = offset;
    for (i=0; i<numq; i++)
    {
        /* Get the name */
        count = ipnet_nat_proxy_dns_decode_name(buf, buflen, offset, dnsname, sizeof(dnsname));
        if (count < 0)
        {
            IPCOM_LOG0(WARNING, "ipnet_nat_proxy_dns_parse_questions() :: could not decode dns name");
            return -1;
        }

        /* Copy the name */
        if (newbuflen - *newlen < count)
        {
            IPCOM_LOG0(WARNING, "ipnet_nat_proxy_dns_parse_questions() :: no space left in modified buffer");
            return -1;
        }
        ipcom_memcpy(&dnsbuf[*newlen], buf+offset, count);
        *newlen += count;
        offset += count;

        /* Check space for type and class */
        if (buflen - offset < 4)
        {
            IPCOM_LOG0(WARNING, "ipnet_nat_proxy_dns_parse_questions() :: message too short to parse question");
            return -1;
        }

        /* Get the type */
        type = (Ip_u16)(IP_GET_NTOHS(buf+offset));
        switch(type)
        {
            case IPNET_NAT_DNS_QTYPE_AAAA:
                if (request)
                {
                    trans = ipnet_nat_proxy_dns_add_transaction(IPNET_NAT_DNS_QTYPE_A, dns_hdr, param, IP_NULL);
                    if (trans == IP_NULL)
                    {
                        IPCOM_LOG0(ERR, "ipnet_nat_proxy_dns_parse_questions() :: could not add transaction to list");
                        return -1;
                    }
                    else
                    {
                        IPCOM_LOG4(DEBUG, "ipnet_nat_proxy_dns_parse_questions() :: added transaction:"
                                          "id=%d port=%d addr=0x%08x type=%d",
                                           trans->id, trans->srcport, trans->dstaddr, trans->type);

                    }
                    type = IPNET_NAT_DNS_QTYPE_A;      /* Change type to A */
                }
                break;
            case IPNET_NAT_DNS_QTYPE_A:
                if (!request)
                {
                    trans = ipnet_nat_proxy_dns_find_transaction(IPNET_NAT_DNS_QTYPE_A, dns_hdr, param);
                    if (trans != IP_NULL)
                    {
                        IPCOM_LOG4(DEBUG, "ipnet_nat_proxy_dns_parse_questions() :: found transaction:"
                                          "id=%d port=%d addr=0x%08x type=%d",
                                           trans->id, trans->srcport, trans->dstaddr, trans->type);
                        ipnet_nat_proxy_dns_remove_transaction(trans);
                    }
                    else
                    {
                        return -1;
                    }
                    type = IPNET_NAT_DNS_QTYPE_AAAA;   /* Change type back to AAAA */
                }
                break;
            case IPNET_NAT_DNS_QTYPE_PTR:
                if (request)
                {
                    zone = (Ip_u8 *)ipcom_strstr((char *)dnsname, "ip6.int");
                    if (zone == IP_NULL)
                        zone = (Ip_u8 *)ipcom_strstr((char *)dnsname, "ip6.arpa");
                    if (zone == IP_NULL)
                    {
                        IPCOM_LOG0(DEBUG, "ipnet_nat_proxy_dns_parse_questions() :: unhandled zone in PTR request");
                        return -1;
                    }
                    /* Extract IPv4 part */
                    if (ipcom_strlen((char *)dnsname) != 64 + ipcom_strlen((char *)zone))
                    {
                        IPCOM_LOG0(WARNING, "ipnet_nat_proxy_dns_parse_questions() :: invalid name in PTR request");
                        return -1;
                    }
                    k=0;
                    for (j=3; j>=0; j--)
                    {
                        c = ipcom_tolower(dnsname[k]);
                        c = c > '9' ? c - 'a' + 10 : c - '0';
                        addr[j] = c;
                        k += 2;
                        c = ipcom_tolower(dnsname[k]);
                        c = c > '9' ? c - 'a' + 10 : c - '0';
                        c <<= 4;
                        addr[j] += c;
                        k += 2;
                    }

                    trans = ipnet_nat_proxy_dns_add_transaction(IPNET_NAT_DNS_QTYPE_PTR, dns_hdr, param, dnsname);
                    if (trans == IP_NULL)
                    {
                        IPCOM_LOG0(ERR, "ipnet_nat_proxy_dns_parse_questions() :: could not add transaction to list");
                        return -1;
                    }
                    else
                    {
                        IPCOM_LOG4(DEBUG, "ipnet_nat_proxy_dns_parse_questions() :: added transaction:"
                                          "id=%d port=%d addr=0x%08x type=%d",
                                           trans->id, trans->srcport, trans->dstaddr, trans->type);
                    }

                    /* Convert address to PTR name */
                    if (ipnet_nat_proxy_dns_ptr_name(dnsname, sizeof(dnsname), addr, IP_AF_INET, (Ip_u8 *)"in-addr.arpa") < 0)
                    {
                        ipnet_nat_proxy_dns_remove_transaction(trans);
                        IPCOM_LOG0(WARNING, "ipnet_nat_proxy_dns_parse_questions() :: could not encode PTR name");
                        return -1;
                    }
                    *newlen -= count;   /* Move index back to before the name */
                    count = ipnet_nat_proxy_dns_encode_name(dnsbuf, newbuflen, *newlen, dnsname);
                    if (count < 0)
                    {
                        ipnet_nat_proxy_dns_remove_transaction(trans);
                        IPCOM_LOG0(WARNING, "ipnet_nat_proxy_dns_parse_questions() :: could not encode dns name");
                        return -1;
                    }
                    *newlen += count;
                }
                else
                {
                    trans = ipnet_nat_proxy_dns_find_transaction(IPNET_NAT_DNS_QTYPE_PTR, dns_hdr, param);
                    if (trans != IP_NULL)
                    {
                        IPCOM_LOG4(DEBUG, "ipnet_nat_proxy_dns_parse_questions() :: found transaction:"
                                          "id=%d port=%d addr=0x%08x type=%d",
                                           trans->id, trans->srcport, trans->dstaddr, trans->type);
                        *newlen -= count;   /* Move index back to before the name */
                        count = ipnet_nat_proxy_dns_encode_name(dnsbuf, newbuflen, *newlen, trans->ptrname);
                        if (count < 0)
                        {
                            ipnet_nat_proxy_dns_remove_transaction(trans);
                            IPCOM_LOG0(WARNING, "ipnet_nat_proxy_dns_parse_questions() :: could not encode dns name");
                            return -1;
                        }
                        *newlen += count;
                        if (numa == 0)
                        {
                            /* Remove the transaction if there are no answers */
                            ipnet_nat_proxy_dns_remove_transaction(trans);
                        }
                    }
                    else
                    {
                        return -1;
                    }
                }
                break;
            default:
                return -1;
        }
        /* Copy the type */
        if (newbuflen - *newlen < 2)
        {
            IPCOM_LOG0(WARNING, "ipnet_nat_proxy_dns_parse_questions() :: no space left in modified buffer");
            return -1;
        }
        IP_SET_HTONS(&dnsbuf[*newlen], type);
        *newlen += 2;
        offset += 2;

        /* Get the class */
        cla = (Ip_u16)(IP_GET_NTOHS(buf+offset));
        if (cla != IPNET_NAT_DNS_QCLASS_INET)
        {
            IPCOM_LOG1(WARNING, "ipnet_nat_proxy_dns_parse_questions() :: unhandled class: %d", cla);
            return -1;
        }
        /* Copy the class */
        if (newbuflen - *newlen < 2)
        {
            IPCOM_LOG0(WARNING, "ipnet_nat_proxy_dns_parse_questions() :: no space left in modified buffer");
            return -1;
        }
        IP_SET_HTONS(&dnsbuf[*newlen], cla);
        *newlen += 2;
        offset += 2;
    }

    return offset - origoffset;
}


/*
 *===========================================================================
 *                    ipnet_nat_proxy_dns_parse_answers
 *===========================================================================
 * Description: Parses and modifies DNS answers in a DNS packet.
 * Parameters:  buf     - pointer to buffer with the DNS questions.
 *              buflen  - length of buffer with DNS questions.
 *              offset  - offset in the buffer where the DNS questions begin.
 *              newlen  - pointer to the length of the message if modified.
 *              dns_hdr - pointer to the DNS protocol header.
 *              param   - pointer to NAT proxy parameters.
 * Returns:     The number of bytes parsed or -1 if failed.
 */
IP_STATIC Ip_s32
ipnet_nat_proxy_dns_parse_answers(Ip_u8 *buf,
                              int buflen,
                              int offset,
                              int *newlen,
                              Ipnet_nat_dns_hdr *dns_hdr,
                              Ipnet_nat_proxy_param *param)
{
    int i, count, origoffset, numa;
    Ip_u16 type, cla, rlen;
    int newbuflen = sizeof(dnsbuf);
    Ipnet_nat_dns_transaction *trans;

    numa = IP_GET_NTOHS(&dns_hdr->no_answ);
    origoffset = offset;
    for (i=0; i<numa; i++)
    {
        /* Get the name */
        count = ipnet_nat_proxy_dns_decode_name(buf, buflen, offset, dnsname, sizeof(dnsname));
        if (count < 0)
        {
            IPCOM_LOG0(WARNING, "ipnet_nat_proxy_dns_parse_answers() :: could not decode dns name");
            return -1;
        }

        /* Copy the name */
        if (newbuflen - *newlen < count)
        {
            IPCOM_LOG0(WARNING, "ipnet_nat_proxy_dns_parse_answers() :: no space left in modified buffer");
            return -1;
        }
        ipcom_memcpy(&dnsbuf[*newlen], buf+offset, count);
        *newlen += count;
        offset  += count;

        /* Check space for type, class, ttl and record length */
        if (buflen - offset < 10)
        {
            IPCOM_LOG0(WARNING, "ipnet_nat_proxy_dns_parse_answers() :: message too short to parse answer");
            return -1;
        }

        /* Get the type */
        type = (Ip_u16)(IP_GET_NTOHS(buf+offset));
        switch(type)
        {
            case IPNET_NAT_DNS_QTYPE_A:
                type = IPNET_NAT_DNS_QTYPE_AAAA;
                break;
            case IPNET_NAT_DNS_QTYPE_PTR:
                trans = ipnet_nat_proxy_dns_find_transaction(IPNET_NAT_DNS_QTYPE_PTR, dns_hdr, param);
                if (trans != IP_NULL)
                {
                    IPCOM_LOG4(DEBUG, "ipnet_nat_proxy_dns_parse_answers() :: found transaction:"
                                      "id=%d port=%d addr=0x%08x type=%d",
                                       trans->id, trans->srcport, trans->dstaddr, trans->type);
                    *newlen -= count;   /* Move index back to before the name */
                    count = ipnet_nat_proxy_dns_encode_name(dnsbuf, newbuflen, *newlen, trans->ptrname);
                    if (count < 0)
                    {
                        ipnet_nat_proxy_dns_remove_transaction(trans);
                        IPCOM_LOG0(WARNING, "ipnet_nat_proxy_dns_parse_questions() :: could not encode dns name");
                        return -1;
                    }
                    *newlen += count;
                    if (i+1 == numa)
                    {
                        /* Remove transaction for last answer */
                        ipnet_nat_proxy_dns_remove_transaction(trans);
                    }
                }
                break;
            default:
                break;
        }
        /* Copy the type */
        if (newbuflen - *newlen < 2)
        {
            IPCOM_LOG0(WARNING, "ipnet_nat_proxy_dns_parse_answers() :: no space left in modified buffer");
            return -1;
        }
        IP_SET_HTONS(&dnsbuf[*newlen], type);
        *newlen += 2;
        offset  += 2;

        /* Get the class */
        cla = (Ip_u16)(IP_GET_NTOHS(buf+offset));
        if (cla != IPNET_NAT_DNS_QCLASS_INET)
        {
            IPCOM_LOG1(WARNING, "ipnet_nat_proxy_dns_parse_answers() :: unhandled class: %d", cla);
            return -1;
        }
        /* Copy the class */
        if (newbuflen - *newlen < 2)
        {
            IPCOM_LOG0(WARNING, "ipnet_nat_proxy_dns_parse_answers() :: no space left in modified buffer");
            return -1;
        }
        IP_SET_HTONS(&dnsbuf[*newlen], cla);
        *newlen += 2;
        offset  += 2;

        /* Copy the ttl */
        if (newbuflen - *newlen < 4)
        {
            IPCOM_LOG0(WARNING, "ipnet_nat_proxy_dns_parse_answers() :: no space left in modified buffer");
            return -1;
        }
        ipcom_memcpy(&dnsbuf[*newlen], buf+offset, 4);
        *newlen += 4;
        offset  += 4;

        /* Get the record length */
        rlen = (Ip_u16)(IP_GET_NTOHS(buf+offset));
        if (buflen - offset < rlen)
        {
            IPCOM_LOG0(WARNING, "ipnet_nat_proxy_dns_parse_answers() :: message too short to parse answer");
            return -1;
        }

        /* Copy the record length */
        if (newbuflen - *newlen < 2)
        {
            IPCOM_LOG0(WARNING, "ipnet_nat_proxy_dns_parse_answers() :: no space left in modified buffer");
            return -1;
        }
        if (type == IPNET_NAT_DNS_QTYPE_AAAA && rlen == 4)
        {
            /* Modify record length and make space for the AAAA record */
            IP_SET_HTONS(&dnsbuf[*newlen], 16);
            *newlen += 2;
            /* Insert AAAA record data  */
            if (newbuflen - *newlen < 12)
            {
                IPCOM_LOG0(WARNING, "ipnet_nat_proxy_dns_parse_answers() :: no space left in modified buffer");
                return -1;
            }
            ipcom_memcpy(&dnsbuf[*newlen], param->prefix, 12);
            *newlen += 12;
        }
        else
        {
            IP_SET_HTONS(&dnsbuf[*newlen], rlen);
            *newlen += 2;
        }
        offset += 2;

        /* Copy the record */
        if (newbuflen - *newlen < rlen)
        {
            IPCOM_LOG0(WARNING, "ipnet_nat_proxy_dns_parse_answers() :: no space left in modified buffer");
            return -1;
        }
        ipcom_memcpy(&dnsbuf[*newlen], buf+offset, rlen);
        *newlen += rlen;
        offset  += rlen;
    }

    return offset - origoffset;
}


/*
 *===========================================================================
 *                    ipnet_nat_proxy_dns_reply
 *===========================================================================
 * Description: Parse and modify DNS reply.
 * Parameters:  appdata   - pointer to application data.
 *              applen    - pointer to length of application data.
 *              growspace - space available to extend application data.
 *              param     - pointer to proxy parameters.
 *              newdata   - pointer to pointer to new application data.
 * Returns:     1 = Packet modified.
 *              0 = Packet untouched.
 */
IP_STATIC int
ipnet_nat_proxy_dns_reply(Ip_u8 *appdata,
                          int   *applen,
                          int    growspace,
                          Ipnet_nat_proxy_param *param,
                          Ip_u8 **newdata)
{
    Ipnet_nat_dns_hdr *dns_hdr;
    Ip_u16 flags;
    int offset, diff, qlen;
    int newbuflen = sizeof(dnsbuf);
    int newlen    = 0;

    /* Check that at least a header is included */
    if (*applen < (int)sizeof(*dns_hdr))
    {
        IPCOM_LOG0(WARNING, "ipnet_nat_proxy_dns_reply() :: message too short for dns header");
        return 0;
    }
    dns_hdr = (Ipnet_nat_dns_hdr *)appdata;
    flags = IP_GET_NTOHS(&dns_hdr->flags);
    if((flags & IPNET_NAT_DNS_QR_FLAG)     != IPNET_NAT_DNS_QR_FLAG || /* Not a reply */
       (flags & IPNET_NAT_DNS_OPCODE_FLAG) != 0 ||                     /* Not a standard query */
       (flags & IPNET_NAT_DNS_TC_FLAG)     != 0)                       /* Truncated */
    {
        IPCOM_LOG0(WARNING, "ipnet_nat_proxy_dns_reply() :: invalid header");
        return 0;
    }

    /* Copy the header */
    if (newbuflen - newlen < (int)sizeof(*dns_hdr))
    {
        IPCOM_LOG0(WARNING, "ipnet_nat_proxy_dns_reply() :: no space left in modified buffer");
        return 0;
    }
    ipcom_memcpy(&dnsbuf[newlen], dns_hdr, sizeof(*dns_hdr));
    newlen += sizeof(*dns_hdr);

    /* Get the questions */
    offset = sizeof(*dns_hdr);
    qlen = ipnet_nat_proxy_dns_parse_questions(appdata, *applen, offset, &newlen, dns_hdr, param);
    if (qlen < 0)
        return 0;
    offset += qlen;

    /* Get the answers */
    qlen = ipnet_nat_proxy_dns_parse_answers(appdata, *applen, offset, &newlen, dns_hdr, param);
    if (qlen < 0)
        return 0;
    offset += qlen;

    /* Skip authority and additional records */
    dns_hdr = (Ipnet_nat_dns_hdr *)&dnsbuf[0];
    IP_SET_HTONS(&dns_hdr->no_auth, 0);
    IP_SET_HTONS(&dns_hdr->no_addi, 0);

    /* Update application data with the modified buffer */
    diff = newlen - *applen;
    if (diff > growspace)
    {
        /* Must allocate a new buffer */
        *newdata = ipcom_malloc(*applen + diff);
        if (*newdata == IP_NULL)
        {
            IPCOM_LOG1(ERR, "ipnet_nat_proxy_dns_reply() :: ipcom_malloc(%d) failed",
                            *applen + diff);
            return -1;
        }
        ipcom_memcpy(*newdata, dnsbuf, newlen);
    }
    else
    {
        /* Let the current buffer grow */
        ipcom_memcpy(appdata, dnsbuf, newlen);
    }
    *applen = newlen;
    return 1;
}


/*
 *===========================================================================
 *                    ipnet_nat_proxy_dns_request
 *===========================================================================
 * Description: Parse and modify DNS request.
 * Parameters:  appdata   - pointer to application data.
 *              applen    - pointer to length of application data.
 *              growspace - space available to extend application data.
 *              param     - pointer to proxy parameters.
 *              newdata   - pointer to pointer to new application data.
 * Returns:     1 = Packet modified.
 *              0 = Packet untouched.
 */
IP_STATIC int
ipnet_nat_proxy_dns_request(Ip_u8 *appdata,
                            int   *applen,
                            int    growspace,
                            Ipnet_nat_proxy_param *param,
                            Ip_u8 **newdata)
{
    Ipnet_nat_dns_hdr *dns_hdr;
    Ip_u16 flags;
    int offset, diff, qlen, remainder;
    int newbuflen = sizeof(dnsbuf);
    int newlen    = 0;

    /* Check that at least a header is included */
    if (*applen < (int)sizeof(*dns_hdr))
    {
        IPCOM_LOG0(WARNING, "ipnet_nat_proxy_dns_request() :: message too short for dns header");
        return 0;
    }
    dns_hdr = (Ipnet_nat_dns_hdr *)appdata;
    flags = IP_GET_NTOHS(&dns_hdr->flags);
    if((flags & IPNET_NAT_DNS_QR_FLAG)     != 0 ||  /* Not a request */
       (flags & IPNET_NAT_DNS_OPCODE_FLAG) != 0 ||  /* Not a standard query */
       (flags & IPNET_NAT_DNS_TC_FLAG)     != 0)    /* Truncated */
    {
        IPCOM_LOG0(WARNING, "ipnet_nat_proxy_dns_request() :: invalid header");
        return 0;
    }

    /* Copy the header */
    if (newbuflen - newlen < (int)sizeof(*dns_hdr))
    {
        IPCOM_LOG0(WARNING, "ipnet_nat_proxy_dns_request() :: no space left in modified buffer");
        return 0;
    }
    ipcom_memcpy(&dnsbuf[newlen], dns_hdr, sizeof(*dns_hdr));
    newlen += sizeof(*dns_hdr);

    /* Get the questions */
    offset = sizeof(*dns_hdr);
    qlen   = ipnet_nat_proxy_dns_parse_questions(appdata, *applen, offset, &newlen, dns_hdr, param);
    if (qlen < 0)
        return 0;
    offset += qlen;

    /* Copy the rest of the message */
    remainder = *applen - offset;
    if (remainder)
    {
        if (newbuflen - newlen < remainder)
        {
            IPCOM_LOG0(WARNING, "ipnet_nat_proxy_dns_request() :: no space left in modified buffer");
            return 0;
        }
        ipcom_memcpy(&dnsbuf[newlen], appdata+offset, remainder);
        newlen += remainder;
        offset += remainder;
    }

    /* Update application data with the modified buffer */
    diff = newlen - *applen;
    if (diff > growspace)
    {
        /* Must allocate a new buffer */
        *newdata = ipcom_malloc(*applen + diff);
        if (*newdata == IP_NULL)
        {
            IPCOM_LOG1(ERR, "ipnet_nat_proxy_dns_request() :: ipcom_malloc(%d) failed",
                            *applen + diff);
            return -1;
        }
        ipcom_memcpy(*newdata, dnsbuf, newlen);
    }
    else
    {
        /* Let the current buffer grow */
        ipcom_memcpy(appdata, dnsbuf, newlen);
    }
    *applen = newlen;
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
 *                    ipnet_nat_proxy_dns
 *===========================================================================
 * Description: NAT proxy for the DNS protocol.
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
ipnet_nat_proxy_dns(Ip_u8 *newhdr,
                    Ip_u8 *appdata,
                    int   *applen,
                    int    growspace,
                    Ipnet_nat_proxy_param *param,
                    Ip_u8 **newdata)
{
    IP_STATIC int init = 0;

    (void)newhdr;

    /* Cannot handle fragments other than the first */
    if (param->fragoff != 0)
        return 0;

    /* DNS proxy only applicable for NAT-PT */
    if(param->natpt == IP_TRUE)
    {
        if (!init)
        {
            ipcom_list_init(&ipnet_nat_proxy_dns_list);
            init = 1;
        }
        if (param->incoming == IP_FALSE)
        {
            return ipnet_nat_proxy_dns_request(appdata, applen, growspace, param, newdata);
        }
        else
        {
            return ipnet_nat_proxy_dns_reply(appdata, applen, growspace, param, newdata);
        }
    }
    return 0;
}

#else
int ipnet_nat_dnsproxy_empty_file;
#endif /* IPNET_USE_NAT_PT */


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
