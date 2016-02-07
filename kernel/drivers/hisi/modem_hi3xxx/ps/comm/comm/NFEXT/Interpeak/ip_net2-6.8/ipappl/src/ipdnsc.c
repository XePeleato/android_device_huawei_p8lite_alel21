/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipdnsc.c,v $ $Revision: 1.62 $
 *   $Source: /home/interpeak/CVSRoot/ipappl/src/ipdnsc.c,v $
 *   $Author: jhorteli $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Markus Carlstedt <markus@interpeak.se>
 ******************************************************************************
 */

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 * DNS main source file.
 */

/*
 ****************************************************************************
 * 2                    CONFIGURATION
 ****************************************************************************
 */

#include "ipdnsc_config.h"


/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#define IPCOM_USE_CLIB_PROTO
#include <ipcom_clib.h>
#include <ipcom_time.h>

#include "ipdnsc.h"
#include "ipdnsc_h.h"


/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

/* Query flags */
#define IPDNSC_QR_FLAG      0x8000
#define IPDNSC_OPCODE_FLAG  0x7800
#define IPDNSC_AA_FLAG      0x0400
#define IPDNSC_TC_FLAG      0x0200
#define IPDNSC_RD_FLAG      0x0100
#define IPDNSC_RA_FLAG      0x0080
#define IPDNSC_ZERO_FLAG    0x0070
#define IPDNSC_RCODE_FLAG   0x000f


/* Query types */
#define IPDNSC_QTYPE_A          1
#define IPDNSC_QTYPE_NS         2
#define IPDNSC_QTYPE_CNAME      5
#define IPDNSC_QTYPE_PTR        12
#define IPDNSC_QTYPE_HINFO      13
#define IPDNSC_QTYPE_MX         15
#define IPDNSC_QTYPE_AAAA       28


/* Query classes */
#define IPDNSC_QCLASS_INET      1


/* Query return codes */
#define IPDNSC_OK               0       /* General OK */
#define IPDNSC_ERR_GENERAL      -1      /* General error */
#define IPDNSC_SERV_TIMEOUT     -2      /* Server did not respond in time */
#define IPDNSC_SERV_NONAME      -3      /* The name in the query does not exist */
#define IPDNSC_SERV_FORMAT      -4      /* Format error */
#define IPDNSC_SERV_FAIL        -5      /* Name server error */
#define IPDNSC_SERV_NOTIMPL     -6      /* Not implemented */
#define IPDNSC_SERV_REFUSED     -7      /* Refused */
#ifdef IPCOM_DNSC_TC_SUPPORT
#define IPDNSC_SERV_TC_FLAG      -8
#endif

/* Maximum size */
#define IPDNSC_MAXLABEL     63
#define IPDNSC_MAXPKT       512


/* DNS cache type */
#define IPDNSC_CACHE_NAME   0
#define IPDNSC_CACHE_ADDR   1


/* DNS defaults */
#define IPDNSC_DEFAULT_DNS_PORT    53
#define IPDNSC_DEFAULT_DNS_TIMEOUT 10
#define IPDNSC_DEFAULT_DNS_RETRY    2


/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

/*
 *===========================================================================
 *                         Ipdnsc_hdr
 *===========================================================================
 * DNS protocol header structure.
 */
#include "ipcom_align16.h"
typedef IP_PACKED1 struct Ipdnsc_hdr_struct
{
    Ip_u16  id;             /* identification */
    Ip_u16  flags;          /* flags */
	Ip_u16  no_ques;        /* number of questions */
	Ip_u16  no_answ;        /* number of answers */
	Ip_u16  no_auth;        /* number of authority records */
	Ip_u16  no_addi;        /* number of resource records */
}
IP_PACKED2 Ipdnsc_hdr;
#include "ipcom_align16.h"


/*
 ****************************************************************************
 * 6                    EXTERN PROTOTYPES
 ****************************************************************************
 */

IP_EXTERN int ipcom_dns_init_ok;
IP_EXTERN IP_PUBLIC Ipdnsc_api ipcom_dns;


/*
 ****************************************************************************
 * 7                    LOCAL PROTOTYPES
 ****************************************************************************
 */

IP_PUBLIC Ip_err ipdnsc_create(void);
IP_PUBLIC Ip_err ipdnsc_start(void);
IP_PUBLIC const char *ipdnsc_version(void);


/*
 ****************************************************************************
 * 8                    DATA
 ****************************************************************************
 */

#ifdef IPCOM_USE_INET6
const struct Ip_in6_addr ipdnsc_in6addr_any =
    {{{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }}};
#endif

Ipdnsc_data ipdnsc;


/*
 ****************************************************************************
 * 9                   LOCAL FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipdnsc_ptr_name
 *===========================================================================
 * Description: Converts an internet address to a form suitable for
 *              DNS ptr queries and appends the top domain
 * Parameters:  buf - pointer to buffer where the result is stored
 *              addr - pointer to internet address in network order
 *              af - address family
 * Returns:     0 - OK
 *             -1 - Fail
 */
IP_STATIC int
ipdnsc_ptr_name(Ip_u8 *buf, Ip_u32 buflen, Ip_u8 *addr, Ip_s32 family)
{
#ifdef IPCOM_USE_INET6
    struct Ip_in6_addr *in6_addr;
    Ip_u8 nibble;
#endif
    int ret = 0, must_free = 1;
    int i, must_print_tens;
    Ip_u8 tmp;
    char *zone;

    if (buflen < 64)     /* 32(hex digits) + 32(.) */
        return -1;

    switch(family)
    {
        case IP_AF_INET:

            /* Reverse ipv4 address */
            for (i=3; i>=0; i--)
            {
                must_print_tens = IP_FALSE;

                /* possible hundreds digit */
                tmp = (Ip_u8)(addr[i] / 100);
                if (tmp)
                {
                    *buf++ = '0' + tmp;
                    must_print_tens = IP_TRUE;

                }
                tmp = (Ip_u8)(addr[i] % 100);

                /* possible tens digit */
                if ((tmp / 10) || must_print_tens)
                {
                    *buf++ = '0' + tmp / 10;

                }
                /* always print ones digit */
                *buf++ = '0' + tmp % 10;

                *buf++ = '.';
            }

            /* Append the top level domain */
            zone = ipcom_sysvar_get ("ipdnsc.ip4.zone", IP_NULL, IP_NULL);
            if (zone == IP_NULL)
            {
                must_free = 0;
                zone = "in-addr.arpa";   /* Default */
            }
            break;

#ifdef IPCOM_USE_INET6
        case IP_AF_INET6:

            in6_addr = (struct Ip_in6_addr *)addr;

            /* Split ipv6 address in dot-separated nibbles in reverse order */
            for (i=15; i>=0; i--)
            {
                nibble = in6_addr->in6.addr8[i] & 0x0f;
                *buf++ = nibble > 9 ? 'a' + nibble - 10 : '0' + nibble;
                *buf++ = '.';
                nibble = (Ip_u8)((in6_addr->in6.addr8[i] & 0xf0) >> 4);
                *buf++ = nibble > 9 ? 'a' + nibble - 10 : '0' + nibble;
                *buf++ = '.';
            }

            /* Append the top level domain */
            zone = ipcom_sysvar_get ("ipdnsc.ip6.zone", IP_NULL, IP_NULL);
            if (zone == IP_NULL)
            {
                must_free = 0;
                zone = "ip6.int";    /* Default */
            }
            break;
#endif /* IPCOM_USE_INET6 */
        default:
            return -1;
    }

    if (buflen <= 64 + ipcom_strlen(zone))
        ret = -1;
    else
        ipcom_strcpy((char *)buf, zone);

    if (must_free)
        ipcom_free(zone);
    return ret;
}


/*
 *===========================================================================
 *                    ipdnsc_encode_name
 *===========================================================================
 * Description: Converts a domain name to a form suitable for DNS
 *              queries
 * Parameters:  buf  - pointer to buffer where the result is stored
 *              name - the domain name to convert
 * Returns:     The number of bytes in the converted name or -1 if fail
 */
IP_STATIC Ip_s32
ipdnsc_encode_name(Ip_u8 *buf, Ip_u8 *name)
{
    Ip_u8 *ptr;
    Ip_s32 count, tot_count = 0;

    while (*name)
    {
        count = 0;
        ptr = buf;
        buf++;
        while (*name && (*name != '.'))
        {
            *buf++ = *name++;
            count++;
            if (count > IPDNSC_MAXLABEL)
                return -1;
        }
        if (*name != '\0')
            name++;
        *ptr = (Ip_u8)count;
        tot_count = tot_count + count + 1;
    }

    /* Add the terminating zero */
    *buf = 0;
    tot_count++;

    if (tot_count > IPDNSC_MAXNAME)
        return -1;

    return tot_count;
}


/*
 *===========================================================================
 *                    ipdnsc_decode_name
 *===========================================================================
 * Description: Restores a converted domain name to uncompressed and readable
 *              format.
 * Parameters:  name  - the restored domain name (null terminated string).
 *              buf   - pointer to buffer with the received name
 *              start - pointer to start of DNS message.
 *              end   - pointer to end of DNS message.
 * Returns:     The number of bytes in the compressed name or -1 if fail
 */
IP_STATIC Ip_s32
ipdnsc_decode_name(Ip_u8 *name, Ip_u8 *buf, Ip_u8 *start, Ip_u8 *end)
{
    Ip_u8 *ptr, *prev;
    Ip_s32 i, len, tot_len = 0, compress = 0;

    ptr = buf;
    while (*ptr && ptr < end)
    {
        /* Loop until we find a non-pointer */
        while ((*ptr & 0xc0) == 0xc0 && ptr < end)
        {
            prev = ptr;
            ptr = start + (IP_GET_NTOHS(ptr) & 0x3fff);
            if (ptr >= prev)
                return -1;      /* Do not allow forward jumps (avoid loops) */
            if (!compress)
            {
                tot_len += 2;
            }
            compress = 1;
        }
        /* Store the length of the label */
        if (ptr >= end)
            return -1;
        len = *ptr++;
        if (len > IPDNSC_MAXLABEL)
            return -1;
        if (!compress)
        {
            tot_len = tot_len + len + 1;
        }
        if (tot_len > IPDNSC_MAXNAME)
            return -1;
        /* Copy the label to name */
        for (i=0; i<len; i++)
        {
            if (ptr >= end)
                return -1;
            *name++ = *ptr++;
        }
        *name++ = '.';
    }

    if (!compress)
    {
        /* Increment for the last zero */
        tot_len++;
    }

    /* Null terminate the name string */
    if (tot_len)
        name--;
    *name = 0;
    return tot_len;
}


/*
 *===========================================================================
 *                    ipdnsc_create_query
 *===========================================================================
 * Description: Create a dns query message
 * Parameters:  dname - the name to query for.
 *              type  - the query type
 *              class - the query class
 *              buffer for the generated query
 * Returns:     length of query message or -1 for error
 */
IP_STATIC Ip_s32
ipdnsc_create_query(Ip_u8 *dname, Ip_u16 type, Ip_u16 class, Ip_u8 *buf)
{
    static Ip_u16 id_count = 0;
    Ipdnsc_hdr *dns_hdr;
    Ip_u8 *qp;
    Ip_s32 qlen;

    if (buf == IP_NULL)
    {
        return -1;
    }

    dns_hdr = (Ipdnsc_hdr *)buf;
    qp = (Ip_u8 *)(dns_hdr + 1);

    /* Assign values to the DNS message format */
    dns_hdr->id = IP_SWAP16(id_count);
    id_count++;
    dns_hdr->flags = IP_SWAP16(IPDNSC_RD_FLAG);
    dns_hdr->no_ques = IP_SWAP16(1);
    dns_hdr->no_answ = 0;
    dns_hdr->no_auth = 0;
    dns_hdr->no_addi = 0;

    /* Convert domain name to query format */
    qlen = ipdnsc_encode_name(qp, dname);
    if (qlen < 0)
    {
        return -1;
    }

    /* The remaining fields in the dns message may reside on any byte alignment */
    qp += qlen;
    /* Set the query type */
    IP_SET_HTONS(qp, type);

    /* Set the query class */
    qp += 2;
    IP_SET_HTONS(qp, class);

    return (sizeof(*dns_hdr) + qlen + 2 + 2);

}


/*
 *===========================================================================
 *                    ipdnsc_send_wait
 *===========================================================================
 * Description: Send a dns query and wait for the response. The supplied receive
 *              buffer must be large enough to hold the maximum sized DNS message
 *              (currently 512 bytes)
 * Parameters:  s_buf  - send buffer
 *              r_buf  - receive buffer
 *              s_size - the number of bytes to send
 *              r_size - the number of bytes that were received
 * Returns:     IPDNSC_OK
 *              IPDNSC_ERR_GENERAL
 *              IPDNSC_SERV_TIMEOUT
 *              IPDNSC_SERV_NONAME
 *              IPDNSC_SERV_FORMAT
 *              IPDNSC_SERV_FAIL
 *              IPDNSC_SERV_NOTIMPL
 *              IPDNSC_SERV_REFUSED
 */
IP_STATIC Ip_s32
ipdnsc_send_wait(Ip_u8 *s_buf, Ip_s32 s_size, Ip_u8 *r_buf, Ip_s32 *r_size, Ipdnsc_ns_addr *dns_addr, Ip_s32 timeout)
{
    Ip_s32 sock, count, num, retval = IPDNSC_OK;
    Ip_fd_set *read_set = IP_NULL;
    struct Ip_timeval tv;
    Ip_u16 s_id, r_id, s_flags, r_flags;

    /* Save id and flags */
    s_id = IP_SWAP16(*(Ip_u16 *)(s_buf));
    s_flags = IP_SWAP16(*(Ip_u16 *)(s_buf+2));

    /* Get a socket */
    sock = ipcom_socket(dns_addr->family, IP_SOCK_DGRAM, 0);
    if (sock < 0)
    {
        retval = IPDNSC_ERR_GENERAL;
        return retval;
    }

    if (dns_addr->family == IP_AF_INET)
    {

        struct Ip_sockaddr_in in_addr;

        /* Bind the socket */
        ipcom_memset(&in_addr, 0, sizeof(struct Ip_sockaddr_in));
        IPCOM_SA_LEN_SET(&in_addr, sizeof(struct Ip_sockaddr_in));
        in_addr.sin_family = IP_AF_INET;
        in_addr.sin_addr.s_addr = IP_INADDR_ANY;
        in_addr.sin_port = 0;
        if (ipcom_bind(sock, (struct Ip_sockaddr*) &in_addr, sizeof(struct Ip_sockaddr)) < 0)
        {
            retval = IPDNSC_ERR_GENERAL;
            goto out;
        }

        /* Send the query */
        ipcom_memset(&in_addr, 0, sizeof(struct Ip_sockaddr_in));
        IPCOM_SA_LEN_SET(&in_addr, sizeof(struct Ip_sockaddr_in));
        in_addr.sin_family = IP_AF_INET;
        ipcom_memcpy(&in_addr.sin_addr, &dns_addr->addr.inaddr, sizeof(in_addr.sin_addr));
        in_addr.sin_port = dns_addr->port;

        count = ipcom_sendto(sock, s_buf, s_size, 0,
                             (IP_CONST struct Ip_sockaddr *) &in_addr,
                             sizeof(struct Ip_sockaddr_in));


    }
#ifdef IPCOM_USE_INET6
    else if (dns_addr->family == IP_AF_INET6)
    {

        struct Ip_sockaddr_in6 in_addr6;

        /* Bind the socket */
        ipcom_memset(&in_addr6, 0, sizeof(struct Ip_sockaddr_in6));
        IPCOM_SA_LEN_SET(&in_addr6, sizeof(struct Ip_sockaddr_in6));
        in_addr6.sin6_family = IP_AF_INET6;
        in_addr6.sin6_addr = ipdnsc_in6addr_any;
        in_addr6.sin6_port = 0;
        if (ipcom_bind(sock, (struct Ip_sockaddr*)&in_addr6, sizeof(struct Ip_sockaddr_in6)) < 0)
        {
            retval = IPDNSC_ERR_GENERAL;
            goto out;
        }

        /* Send the query */
        ipcom_memset(&in_addr6, 0, sizeof(struct Ip_sockaddr_in6));
        IPCOM_SA_LEN_SET(&in_addr6, sizeof(struct Ip_sockaddr_in6));
        in_addr6.sin6_family = IP_AF_INET6;
        ipcom_memcpy(&in_addr6.sin6_addr, &dns_addr->addr.inaddr6, sizeof(in_addr6.sin6_addr));
        in_addr6.sin6_scope_id = dns_addr->ifindex;
        in_addr6.sin6_port = dns_addr->port;

        count = ipcom_sendto(sock, s_buf, s_size, 0,
                             (IP_CONST struct Ip_sockaddr *) &in_addr6,
                             sizeof(struct Ip_sockaddr_in6));
    }
#endif /* IPCOM_USE_INET6 */
    else
    {
        retval = IPDNSC_ERR_GENERAL;
        goto out;
    }

    /* Check number of sent bytes */
    if (count != s_size)
    {
        retval = IPDNSC_ERR_GENERAL;
        goto out;
    }

    /* Prepare the read set */
    read_set = ipcom_malloc(sizeof(Ip_fd_set));
    if (read_set == IP_NULL)
    {
        retval = IPDNSC_ERR_GENERAL;
        goto out;
    }
    ipcom_memset(read_set, 0, sizeof(Ip_fd_set));

    /* Set the timeout */
    tv.tv_sec = timeout;
    tv.tv_usec = 0;

    /* Wait for the response */
    IP_FD_SET(sock, read_set);
    num = ipcom_socketselect(sock+1, read_set, IP_NULL, IP_NULL, &tv);
    if (num == IP_SOCKERR)
    {
        retval = IPDNSC_ERR_GENERAL;
        goto out;
    }

    if (num == 0)
    {
        /* Timeout */
        retval = IPDNSC_SERV_TIMEOUT;
        goto out;
    }
    else if (num > 0 && IP_FD_ISSET(sock, read_set))
    {
        /* Data available */
        count = ipcom_recvfrom(sock, r_buf, IPDNSC_MAXPKT, 0, IP_NULL, IP_NULL);
    }
    else
    {
        retval = IPDNSC_ERR_GENERAL;
        goto out;
    }

    /* check received number of bytes */
    if (count < (Ip_s32)sizeof(Ipdnsc_hdr))
    {
        retval = IPDNSC_ERR_GENERAL;
        goto out;
    }

    /* Save id and flags */
    r_id = IP_SWAP16(*(Ip_u16 *)(r_buf));
    r_flags = IP_SWAP16(*(Ip_u16 *)(r_buf+2));

    /* Do some basic response checks */
    if (r_id != s_id)
    {
        /* Identifiction don't match */
        retval = IPDNSC_ERR_GENERAL;
        goto out;
    }
    if (!(r_flags & IPDNSC_QR_FLAG))
    {
        /* This was not a response */
        retval = IPDNSC_ERR_GENERAL;
        goto out;
    }
    if ((r_flags & IPDNSC_OPCODE_FLAG) != (s_flags & IPDNSC_OPCODE_FLAG))
    {
        /* Opcode not copied into response */
        retval = IPDNSC_ERR_GENERAL;
        goto out;
    }
    if (r_flags & IPDNSC_TC_FLAG)
    {
#ifndef IPCOM_DNSC_TC_SUPPORT
        /* Can't handle truncated messages */
        retval = IPDNSC_ERR_GENERAL;
        goto out;
#else
        retval = IPDNSC_SERV_TC_FLAG;
#endif
    }
    if ((r_flags & IPDNSC_RD_FLAG) != (s_flags & IPDNSC_RD_FLAG))
    {
        /* RD flag not copied into response */
        retval = IPDNSC_ERR_GENERAL;
        goto out;
    }
    if (r_flags & IPDNSC_ZERO_FLAG)
    {
        /* This field must be zero */
        retval = IPDNSC_ERR_GENERAL;
        goto out;
    }
    if (r_flags & IPDNSC_RCODE_FLAG)
    {
        /* Server returned error */
        if ((r_flags & IPDNSC_RCODE_FLAG) == 1)
            retval = IPDNSC_SERV_FORMAT;  /* Format error */
        else if ((r_flags & IPDNSC_RCODE_FLAG) == 2)
            retval = IPDNSC_SERV_FAIL;    /* Server failure */
        else if ((r_flags & IPDNSC_RCODE_FLAG) == 3)
            retval = IPDNSC_SERV_NONAME;  /* Name Error */
        else if ((r_flags & IPDNSC_RCODE_FLAG) == 4)
            retval = IPDNSC_SERV_NOTIMPL; /* Not Implemented */
        else if ((r_flags & IPDNSC_RCODE_FLAG) == 5)
            retval = IPDNSC_SERV_REFUSED; /* Refused */
        else
            retval = IPDNSC_ERR_GENERAL;
        goto out;
    }

    *r_size = count;


out:
    if (read_set != IP_NULL)
        ipcom_free(read_set);
    ipcom_socketclose(sock);
    return retval;
}

#ifdef IPCOM_DNSC_TC_SUPPORT
/*
 *===========================================================================
 *                    ipdnsc_tcp_send_wait
 *===========================================================================
 * Description: Send a dns query and wait for the response with using a TCP session. 
 * Parameters:  s_buf  - send buffer
 *              r_buf  - receive buffer
 *              s_size - the number of bytes to send
 *              r_size - the number of bytes that were received
 * Returns:     IPDNSC_OK
 *              IPDNSC_ERR_GENERAL
 *              IPDNSC_SERV_TIMEOUT
 *              IPDNSC_SERV_NONAME
 *              IPDNSC_SERV_FORMAT
 *              IPDNSC_SERV_FAIL
 *              IPDNSC_SERV_NOTIMPL
 *              IPDNSC_SERV_REFUSED
 */
IP_STATIC Ip_s32
ipdnsc_tcp_send_wait(Ip_u8 *s_buf, Ip_s32 s_size, Ip_u8 **r_buf_tcp, Ip_s32 *r_size, Ipdnsc_ns_addr *dns_addr, Ip_s32 timeout)
{
        Ip_s32 sock, count, num, retval = IPDNSC_OK;
        Ip_fd_set *read_set = IP_NULL;
        struct Ip_timeval tv;
        Ip_u16 s_id, r_id, s_flags, r_flags;
        Ip_u16 s_len, r_len = 0, r_total_len = 0;
        char *r_buf = IP_NULL;
    
        /* Save id and flags */
        s_id = IP_SWAP16(*(Ip_u16 *)(s_buf));
        s_flags = IP_SWAP16(*(Ip_u16 *)(s_buf+2));
    
        /* Get a socket */
        sock = ipcom_socket(dns_addr->family, IP_SOCK_STREAM, 0);
        if (sock < 0)
        {
            retval = IPDNSC_ERR_GENERAL;
            return retval;
        }

        ipcom_mutex_lock(ipdnsc.user_mutex);
        ipdnsc.current_users++;
        if(ipdnsc.current_users > ipcom_sysvar_get_as_int ("ipdnsc.max_users",   0))
            goto out;
        ipcom_mutex_unlock(ipdnsc.user_mutex);
        
        if (dns_addr->family == IP_AF_INET)
        {
    
            struct Ip_sockaddr_in in_addr;
    
            /* Bind the socket */
            ipcom_memset(&in_addr, 0, sizeof(struct Ip_sockaddr_in));
            IPCOM_SA_LEN_SET(&in_addr, sizeof(struct Ip_sockaddr_in));
            in_addr.sin_family = IP_AF_INET;
            in_addr.sin_addr.s_addr = IP_INADDR_ANY;
            in_addr.sin_port = 0;
            if (ipcom_bind(sock, (struct Ip_sockaddr*) &in_addr, sizeof(struct Ip_sockaddr)) < 0)
            {
                retval = IPDNSC_ERR_GENERAL;
                goto out;
            }
   
            /* Connect the DNS server */
            ipcom_memset(&in_addr, 0, sizeof(struct Ip_sockaddr_in));
            IPCOM_SA_LEN_SET(&in_addr, sizeof(struct Ip_sockaddr_in));
            in_addr.sin_family = IP_AF_INET;
            ipcom_memcpy(&in_addr.sin_addr, &dns_addr->addr.inaddr, sizeof(in_addr.sin_addr));
            in_addr.sin_port = dns_addr->port;
            if (ipcom_connect(sock, (struct Ip_sockaddr*) &in_addr, sizeof(struct Ip_sockaddr)) < 0)
            {
                retval = IPDNSC_SERV_FAIL;
                goto out;
            }

            /*send length of the DNS query*/
            s_len = IP_SWAP16((Ip_u16)s_size);
            count = ipcom_send(sock, &s_len, 2, 0);
            if(count != 2)
            {
                retval = IPDNSC_ERR_GENERAL;
                goto out;
            }    

            /*send the DNS query*/
            count = ipcom_send(sock, s_buf, s_size, 0);    
        }
#ifdef IPCOM_USE_INET6
        else if (dns_addr->family == IP_AF_INET6)
        {
    
            struct Ip_sockaddr_in6 in_addr6;
    
            /* Bind the socket */
            ipcom_memset(&in_addr6, 0, sizeof(struct Ip_sockaddr_in6));
            IPCOM_SA_LEN_SET(&in_addr6, sizeof(struct Ip_sockaddr_in6));
            in_addr6.sin6_family = IP_AF_INET6;
            in_addr6.sin6_addr = ipdnsc_in6addr_any;
            in_addr6.sin6_port = 0;
            if (ipcom_bind(sock, (struct Ip_sockaddr*)&in_addr6, sizeof(struct Ip_sockaddr_in6)) < 0)
            {
                retval = IPDNSC_ERR_GENERAL;
                goto out;
            }
    
            /* Send the query */
            ipcom_memset(&in_addr6, 0, sizeof(struct Ip_sockaddr_in6));
            IPCOM_SA_LEN_SET(&in_addr6, sizeof(struct Ip_sockaddr_in6));
            in_addr6.sin6_family = IP_AF_INET6;
            ipcom_memcpy(&in_addr6.sin6_addr, &dns_addr->addr.inaddr6, sizeof(in_addr6.sin6_addr));
            in_addr6.sin6_scope_id = dns_addr->ifindex;
            in_addr6.sin6_port = dns_addr->port;
            if (ipcom_connect(sock, (struct Ip_sockaddr*) &in_addr6, sizeof(struct Ip_sockaddr_in6)) < 0)
            {
                retval = IPDNSC_SERV_FAIL;
                goto out;
            }

            /*send length of the DNS query*/
            s_len = IP_SWAP16((Ip_u16)s_size);
            count = ipcom_send(sock, &s_len, 2, 0);
            if(count != 2)
            {
                retval = IPDNSC_ERR_GENERAL;
                goto out;
            }    

            /*send the DNS query*/
            count = ipcom_send(sock, s_buf, s_size, 0);
        }
#endif /* IPCOM_USE_INET6 */
        else
        {
            retval = IPDNSC_ERR_GENERAL;
            goto out;
        }
    
        /* Check number of sent bytes */
        if (count != s_size)
        {
            retval = IPDNSC_ERR_GENERAL;
            goto out;
        }
    
        /* Prepare the read set */
        read_set = ipcom_malloc(sizeof(Ip_fd_set));
        if (read_set == IP_NULL)
        {
            retval = IPDNSC_ERR_GENERAL;
            goto out;
        }

rerecv:
        ipcom_memset(read_set, 0, sizeof(Ip_fd_set));
    
        /* Set the timeout */
        tv.tv_sec = timeout;
        tv.tv_usec = 0;
    
        /* Wait for the response */
        IP_FD_SET(sock, read_set);
        num = ipcom_socketselect(sock+1, read_set, IP_NULL, IP_NULL, &tv);
        if (num == IP_SOCKERR)
        {
            retval = IPDNSC_ERR_GENERAL;
            goto out;
        }
    
        if (num == 0)
        {
            /* Timeout */
            retval = IPDNSC_SERV_TIMEOUT;
            goto out;
        }
        else if (num > 0 && IP_FD_ISSET(sock, read_set))
        {
            /* Data available */
            if(r_len == 0)
            {
                count = ipcom_recv(sock, &r_len, 2, 0);
                if(count < 2)
                {
                    retval = IPDNSC_ERR_GENERAL;
                    goto out;
                }

                r_len = IP_SWAP16(r_len);
                /* check received number of bytes */
                if (r_len < (Ip_s32)sizeof(Ipdnsc_hdr))
                {
                    retval = IPDNSC_ERR_GENERAL;
                    goto out;
                }
            }
            
            if(r_buf == IP_NULL)
            {
                r_buf = ipcom_malloc(r_len);
                if(r_buf == IP_NULL)
                {
                    retval = IPDNSC_ERR_GENERAL;
                    goto out;  
                }
                *r_buf_tcp = r_buf;                
            }
            
            count = ipcom_recv(sock, r_buf+r_total_len, r_len-r_total_len, 0);
            /* check received number of bytes */
            if (count <= 0)
            {
                retval = IPDNSC_ERR_GENERAL;
                goto out;
            }             
            
            r_total_len += count;
            if (r_total_len < r_len)
            {
                goto rerecv;
            }
            else if(r_total_len > r_len)
            {
                retval = IPDNSC_ERR_GENERAL;
                goto out;
            }
        }
        else
        {
            retval = IPDNSC_ERR_GENERAL;
            goto out;
        }
    
        /* Save id and flags */
        r_id = IP_SWAP16(*(Ip_u16 *)(r_buf));
        r_flags = IP_SWAP16(*(Ip_u16 *)(r_buf+2));
    
        /* Do some basic response checks */
        if (r_id != s_id)
        {
            /* Identifiction don't match */
            retval = IPDNSC_ERR_GENERAL;
            goto out;
        }
        if (!(r_flags & IPDNSC_QR_FLAG))
        {
            /* This was not a response */
            retval = IPDNSC_ERR_GENERAL;
            goto out;
        }
        if ((r_flags & IPDNSC_OPCODE_FLAG) != (s_flags & IPDNSC_OPCODE_FLAG))
        {
            /* Opcode not copied into response */
            retval = IPDNSC_ERR_GENERAL;
            goto out;
        }
        if ((r_flags & IPDNSC_RD_FLAG) != (s_flags & IPDNSC_RD_FLAG))
        {
            /* RD flag not copied into response */
            retval = IPDNSC_ERR_GENERAL;
            goto out;
        }
        if (r_flags & IPDNSC_ZERO_FLAG)
        {
            /* This field must be zero */
            retval = IPDNSC_ERR_GENERAL;
            goto out;
        }
        if (r_flags & IPDNSC_RCODE_FLAG)
        {
            /* Server returned error */
            if ((r_flags & IPDNSC_RCODE_FLAG) == 1)
                retval = IPDNSC_SERV_FORMAT;  /* Format error */
            else if ((r_flags & IPDNSC_RCODE_FLAG) == 2)
                retval = IPDNSC_SERV_FAIL;    /* Server failure */
            else if ((r_flags & IPDNSC_RCODE_FLAG) == 3)
                retval = IPDNSC_SERV_NONAME;  /* Name Error */
            else if ((r_flags & IPDNSC_RCODE_FLAG) == 4)
                retval = IPDNSC_SERV_NOTIMPL; /* Not Implemented */
            else if ((r_flags & IPDNSC_RCODE_FLAG) == 5)
                retval = IPDNSC_SERV_REFUSED; /* Refused */
            else
                retval = IPDNSC_ERR_GENERAL;
            goto out;
        }
    
        *r_size = r_total_len;
    
    
    out:
        if (read_set != IP_NULL)
            ipcom_free(read_set);
        ipcom_socketclose(sock);
        ipcom_mutex_lock(ipdnsc.user_mutex);
        ipdnsc.current_users--;
        ipcom_mutex_unlock(ipdnsc.user_mutex);
        return retval;

}
#endif

/*
 *===========================================================================
 *                    ipdnsc_parse_response
 *===========================================================================
 * Description: Parse a DNS name server response and create a host entry
 *              which will be filled in with the received data
 * Parameters:  dns_msg     - pointer to response
 *              msglen      - length of response.
 *              family      - the address family
 *              cache_ttl   - pointer to the time to cache
 *              num_answers - the number of answers received
 * Returns:     A pointer to an allocated host entry structure or NULL if failed
 */
IP_STATIC struct Ip_hostent*
ipdnsc_parse_response(Ip_u8* dns_msg, Ip_s32 msglen, Ip_s32 family, Ip_u32 *cache_ttl, Ip_s32 *num_answers)
{
    Ipdnsc_hdr *dns_hdr;
    struct Ip_hostent *he;
    char dname[IPDNSC_MAXNAME];
    Ip_u16 qtype, qclass;
    Ip_s32 len, i;
    Ip_u8 *ptr;

    *num_answers = -1;
    dns_hdr = (Ipdnsc_hdr *)dns_msg;

    /* Make sure there is only one question */
    if (IP_SWAP16(dns_hdr->no_ques) != 1)
        return IP_NULL;

    /* Make sure there was answers */
    *num_answers = IP_SWAP16(dns_hdr->no_answ);
    if (IP_SWAP16(dns_hdr->no_answ) == 0)
        return IP_NULL;

    /* Find the name */
    len = ipdnsc_decode_name((Ip_u8*)dname, dns_msg + sizeof(*dns_hdr), dns_msg, dns_msg + msglen);
    if (len < 0)
    {
        return IP_NULL;
    }

    /* The remaining fields in the dns message may reside on any byte alignment */

    /* Get query type and query class */
    ptr = dns_msg + sizeof(*dns_hdr) + len;
    qtype = (Ip_u16)(IP_GET_NTOHS(ptr));
    switch(qtype)
    {
        case IPDNSC_QTYPE_A:
            if (family != IP_AF_INET)
                return IP_NULL;
            break;
        case IPDNSC_QTYPE_AAAA:
            if (family != IP_AF_INET6)
                return IP_NULL;
            break;
        case IPDNSC_QTYPE_PTR:
            break;
        default:
            return IP_NULL;
    }
    qclass = (Ip_u16)(IP_GET_NTOHS(ptr+2));
    if (qclass != IPDNSC_QCLASS_INET)
        return IP_NULL;

    /* Create the host entry structure */
    he = ipdnsc_hostent_create(family);
    if (he == IP_NULL)
        return IP_NULL;

    /* Bump pointer to the first answer */
    ptr += 4;

    /* Parse the answers */
    for (i=0; i<IP_SWAP16(dns_hdr->no_answ); i++)
    {
        Ip_u32 ttl;
        Ip_u16 rrtype, rrclass, rrlen;

        /* Find the name */
        len = ipdnsc_decode_name((Ip_u8*)dname, ptr, dns_msg, dns_msg + msglen);
        if (len < 0)
        {
            goto err_out;
        }

        /* Get query type and query class */
        ptr += len;
        rrtype = (Ip_u16)(IP_GET_NTOHS(ptr));
        rrclass = (Ip_u16)(IP_GET_NTOHS(ptr+2));
        if (rrclass != IPDNSC_QCLASS_INET)
        {
            goto err_out;
        }

        /* Get time to live */
        ptr += 4;
        ttl = IP_GET_NTOHL(ptr);

        /* Get resource record data length */
        ptr += 4;
        rrlen = (Ip_u16)(IP_GET_NTOHS(ptr));

        /* Advance pointer to resource record data */
        ptr += 2;

        switch(rrtype)
        {
            case IPDNSC_QTYPE_A:
            case IPDNSC_QTYPE_AAAA:
                if (qtype != rrtype)
                {
                    /* Answer not relevant */
                    goto err_out;
                }
                if (rrtype == IPDNSC_QTYPE_A)
                {
                    if (rrlen != IPDNSC_INADDRSZ)
                    {
                        /* Error in resource record data length */
                        goto err_out;
                    }
                }
                else if (rrtype == IPDNSC_QTYPE_AAAA)
                {
                    if (rrlen != IPDNSC_IN6ADDRSZ)
                    {
                        /* Error in resource record data length */
                        goto err_out;
                    }
                }

                /* Insert the host address */
                if (ipdnsc_hostent_insert_addr(he, (char *)ptr))
                {
                    goto err_out;
                }

                /* Insert the host name */
                if (ipdnsc_hostent_insert_name(he, dname))
                {
                    goto err_out;
                }

                *cache_ttl = ttl;

                break;

            case IPDNSC_QTYPE_CNAME:
                if (qtype != IPDNSC_QTYPE_A && qtype != IPDNSC_QTYPE_AAAA)
                {
                    /* Answer not relevant */
                    goto err_out;
                }

                /* Insert the alias */
                if (ipdnsc_hostent_insert_alias(he, dname))
                {
                    goto err_out;
                }

                /* Find the primary name */
                len = ipdnsc_decode_name((Ip_u8*)dname, ptr, dns_msg, dns_msg + msglen);
                if ((len < 0) || (len != rrlen))
                {
                    /* Name or length errror */
                    goto err_out;
                }

                break;

            case IPDNSC_QTYPE_PTR:
                if (qtype != IPDNSC_QTYPE_PTR)
                {
                    /* Answer not relevant */
                    goto err_out;
                }

                /* Find the domain name */
                len = ipdnsc_decode_name((Ip_u8*)dname, ptr, dns_msg, dns_msg + msglen);
                if ((len < 0) || (len != rrlen))
                {
                    /* Name or length errror */
                    goto err_out;
                }

                /* Insert the host name */
                if (ipdnsc_hostent_insert_name(he, dname))
                {
                    goto err_out;
                }

                *cache_ttl = ttl;

                break;

            default:
                break;
        }

        /* Bump pointer to the next answer */
        ptr += rrlen;
    }

    /* Sanity checking */
    if (he && he->h_name == IP_NULL)
    {
        if (he->h_aliases[0] != 0)
        {
            /* Special case - CNAME record was received but no A or AAAA record */
            *num_answers = 0;
        }
        goto err_out;
    }

    return he;

err_out:

    ipdnsc_hostent_free(he);
    return IP_NULL;
}

#ifdef IPCOM_DNSC_TC_SUPPORT
/*
 *===========================================================================
 *                    ipdnsc_parse_response_2
 *===========================================================================
 * Description: Parse a DNS name server response and create a host entry
 *              which will be filled in with the received data
 * Parameters:  dns_msg     - pointer to response
 *              msglen      - length of response.
 *              family      - the address family
 *              cache_ttl   - pointer to the time to cache
 *              num_answers - the number of answers received
 * Returns:     A pointer to an allocated host entry structure or NULL if failed
 */
IP_STATIC struct Ip_hostent*
ipdnsc_parse_response_2(Ip_u8* dns_msg, Ip_s32 msglen, Ip_s32 family, Ip_u32 *cache_ttl, Ip_s32 *num_answers)
{
    Ipdnsc_hdr *dns_hdr;
    struct Ip_hostent *he;
    char dname[IPDNSC_MAXNAME];
    Ip_u16 qtype, qclass;
    Ip_s32 len, i;
    Ip_u8 *ptr;

    *num_answers = -1;
    dns_hdr = (Ipdnsc_hdr *)dns_msg;

    /* Make sure there is only one question */
    if (IP_SWAP16(dns_hdr->no_ques) != 1)
        return IP_NULL;

    /* Make sure there was answers */
    *num_answers = IP_SWAP16(dns_hdr->no_answ);
    
    if (IP_SWAP16(dns_hdr->no_answ) == 0)
        return IP_NULL;

    /* Find the name */
    len = ipdnsc_decode_name((Ip_u8*)dname, dns_msg + sizeof(*dns_hdr), dns_msg, dns_msg + msglen);
    if (len < 0)
    {
        return IP_NULL;
    }

    /* The remaining fields in the dns message may reside on any byte alignment */

    /* Get query type and query class */
    ptr = dns_msg + sizeof(*dns_hdr) + len;
    qtype = (Ip_u16)(IP_GET_NTOHS(ptr));
    switch(qtype)
    {
        case IPDNSC_QTYPE_A:
            if (family != IP_AF_INET)
                return IP_NULL;
            break;
        case IPDNSC_QTYPE_AAAA:
            if (family != IP_AF_INET6)
                return IP_NULL;
            break;
        case IPDNSC_QTYPE_PTR:
            break;
        default:
            return IP_NULL;
    }
    qclass = (Ip_u16)(IP_GET_NTOHS(ptr+2));
    if (qclass != IPDNSC_QCLASS_INET)
        return IP_NULL;

    /* Create the host entry structure */
    he = ipdnsc_hostent_create(family);
    if (he == IP_NULL)
        return IP_NULL;

    /* Bump pointer to the first answer */
    ptr += 4;

    /* Parse the answers */
    for (i=0; i<IP_SWAP16(dns_hdr->no_answ); i++)
    {
        Ip_u32 ttl;
        Ip_u16 rrtype, rrclass, rrlen;

        /* Find the name */
        len = ipdnsc_decode_name((Ip_u8*)dname, ptr, dns_msg, dns_msg + msglen);
        if (len < 0)
        {
            goto err_out;
        }

        /*Check answer's head length*/
        ptr += len;
        if(ptr+10 > dns_msg+msglen)
        {
            goto err_out;
        }
        
        /* Get query type and query class */
        rrtype = (Ip_u16)(IP_GET_NTOHS(ptr));
        rrclass = (Ip_u16)(IP_GET_NTOHS(ptr+2));
        if (rrclass != IPDNSC_QCLASS_INET)
        {
            goto err_out;
        }

        /* Get time to live */
        ptr += 4;
        ttl = IP_GET_NTOHL(ptr);

        /* Get resource record data length */
        ptr += 4;
        rrlen = (Ip_u16)(IP_GET_NTOHS(ptr));      
        
        /* Advance pointer to resource record data */
        ptr += 2;
        /*Check one answer's resource record  date length*/
        if(ptr+rrlen > dns_msg+msglen)
        {
            goto err_out;
        }

        switch(rrtype)
        {
            case IPDNSC_QTYPE_A:
            case IPDNSC_QTYPE_AAAA:
                if (qtype != rrtype)
                {
                    /* Answer not relevant */
                    goto err_out;
                }
                if (rrtype == IPDNSC_QTYPE_A)
                {
                    if (rrlen != IPDNSC_INADDRSZ)
                    {
                        /* Error in resource record data length */
                        goto err_out;
                    }
                }
                else if (rrtype == IPDNSC_QTYPE_AAAA)
                {
                    if (rrlen != IPDNSC_IN6ADDRSZ)
                    {
                        /* Error in resource record data length */
                        goto err_out;
                    }
                }

                /* Insert the host address */
                if (ipdnsc_hostent_insert_addr(he, (char *)ptr))
                {
                    goto err_out;
                }

                /* Insert the host name */
                if (ipdnsc_hostent_insert_name(he, dname))
                {
                    goto err_out;
                }

                *cache_ttl = ttl;

                break;

            case IPDNSC_QTYPE_CNAME:
                if (qtype != IPDNSC_QTYPE_A && qtype != IPDNSC_QTYPE_AAAA)
                {
                    /* Answer not relevant */
                    goto err_out;
                }

                /* Insert the alias */
                if (ipdnsc_hostent_insert_alias(he, dname))
                {
                    goto err_out;
                }

                /* Find the primary name */
                len = ipdnsc_decode_name((Ip_u8*)dname, ptr, dns_msg, dns_msg + msglen);
                if ((len < 0) || (len != rrlen))
                {
                    /* Name or length errror */
                    goto err_out;
                }

                break;

            case IPDNSC_QTYPE_PTR:
                if (qtype != IPDNSC_QTYPE_PTR)
                {
                    /* Answer not relevant */
                    goto err_out;
                }

                /* Find the domain name */
                len = ipdnsc_decode_name((Ip_u8*)dname, ptr, dns_msg, dns_msg + msglen);
                if ((len < 0) || (len != rrlen))
                {
                    /* Name or length errror */
                    goto err_out;
                }

                /* Insert the host name */
                if (ipdnsc_hostent_insert_name(he, dname))
                {
                    goto err_out;
                }

                *cache_ttl = ttl;

                break;

            default:
                break;
        }

        /* Bump pointer to the next answer */
        ptr += rrlen;
    }

    /* Sanity checking */
    if (he && he->h_name == IP_NULL)
    {
        if (he->h_aliases[0] != 0)
        {
            /* Special case - CNAME record was received but no A or AAAA record */
            *num_answers = 0;
        }
        goto err_out;
    }

    return he;

err_out:
    ipdnsc_hostent_free(he);
    return IP_NULL;
}

#endif

/*
 *===========================================================================
 *                    ipdnsc_do_query
 *===========================================================================
 * Description: Handle a complete DNS query including: creation of DNS message,
 *              sending the message, receiveing and parsing the response.
 * Parameters:  dname  - the name to resolve
 *              qtype  - the query type
 *              family - the address family
 *              error  - possible error code. will be set to zero if no error
 * Returns:     A pointer to an allocated host entry structure or NULL if failed
 */
#ifndef IPCOM_DNSC_TC_SUPPORT
IP_STATIC struct Ip_hostent *
ipdnsc_do_query(Ip_u8 *dname, Ip_u16 qtype, Ip_s32 family, Ipdnsc_ns_spec *dns, Ip_s32 *error,
                Ip_u32 *cache_ttl)
{
    struct Ip_hostent *he;
    Ipdnsc_ns_addr *dns_addr;
    Ip_u8 *s_buf;
    Ip_u8 *r_buf;
    Ip_s32 s_size, r_size = 0, err, retries, num_answers;

    /* Get buffer memory. */
    s_buf = ipcom_malloc(2 * IPDNSC_MAXPKT);
    if (s_buf == IP_NULL)
        return IP_NULL;
    r_buf = s_buf + IPDNSC_MAXPKT;

    /* Create the DNS query message */
    s_size = ipdnsc_create_query(dname, qtype, IPDNSC_QCLASS_INET, s_buf);
    if (s_size < 0)
    {
        *error = IPDNSC_NO_RECOVERY;
        goto fail;
    }

    /* Send the DNS query message and wait for a response */
    if (dns->primary_dns.family != 0)
        dns_addr = &dns->primary_dns;
    else if (dns->secondary_dns.family != 0)
        dns_addr = &dns->secondary_dns;
    else if (dns->tertiary_dns.family != 0)
        dns_addr = &dns->tertiary_dns;
    else if (dns->quaternary_dns.family != 0)
        dns_addr = &dns->quaternary_dns;
    else
    {
        *error = IPDNSC_NO_RECOVERY;
        goto fail;
    }
    retries = dns->retry;

resend:
    err = ipdnsc_send_wait(s_buf, s_size, r_buf, &r_size, dns_addr, dns->timeout);

    switch(err)
    {
        case IPDNSC_OK:
            break;

        case IPDNSC_SERV_FAIL:
        case IPDNSC_SERV_NOTIMPL:
        case IPDNSC_SERV_REFUSED:
            /* Skip further retries */
            retries = 0;
            /* Fall through */
        case IPDNSC_SERV_TIMEOUT:
            if (retries-- > 0 )
            {
                /* Retry same name server */
                goto resend;
            }
            else if (dns_addr == &dns->primary_dns && dns->secondary_dns.family != 0)
            {
                /* Jump to secondary name server */
                dns_addr = &dns->secondary_dns;
                retries = dns->retry;
                goto resend;
            }
            else if ((dns_addr == &dns->primary_dns || dns_addr == &dns->secondary_dns) && dns->tertiary_dns.family != 0)
            {
                /* Jump to tertiary name server */
                dns_addr = &dns->tertiary_dns;
                retries = dns->retry;
                goto resend;
            }
            else if ((dns_addr == &dns->primary_dns || dns_addr == &dns->secondary_dns || dns_addr == &dns->tertiary_dns)
                        && dns->quaternary_dns.family != 0)
            {
                /* Jump to quaternary name server */
                dns_addr = &dns->quaternary_dns;
                retries = dns->retry;
                goto resend;
            }
            else
            {
                /* Tried all nameservers */
                *error = IPDNSC_NO_RECOVERY;
            }
            goto fail;

        case IPDNSC_SERV_NONAME:
            *error = IPDNSC_HOST_NOT_FOUND;
            goto fail;

        case IPDNSC_ERR_GENERAL:
        case IPDNSC_SERV_FORMAT:
        default:
            *error = IPDNSC_NO_RECOVERY;
            goto fail;
    }

    /* Parse the name server response */
    he = ipdnsc_parse_response(r_buf, r_size, family, cache_ttl, &num_answers);

    *error = (he != IP_NULL) ? IPDNSC_OK : IPDNSC_NO_RECOVERY;

    /* Test the special case of no answers */
    if (num_answers == 0)
    {
        *error = IPDNSC_NO_ADDRESS;
    }

    ipcom_free(s_buf);
    return he;

 fail:
    ipcom_free(s_buf);
    return IP_NULL;
}

#else
IP_STATIC struct Ip_hostent *
ipdnsc_do_query(Ip_u8 *dname, Ip_u16 qtype, Ip_s32 family, Ipdnsc_ns_spec *dns, Ip_s32 *error,
                Ip_u32 *cache_ttl)
{
    struct Ip_hostent *he;
    Ipdnsc_ns_addr *dns_addr;
    Ip_u8 *s_buf;
    Ip_u8 *r_buf;
    Ip_s32 s_size, r_size = 0, err, retries, num_answers;
    Ip_bool use_tcp;
    Ip_u8 *r_buf_tcp = IP_NULL;

    /* Get buffer memory for UDP. */
    s_buf = ipcom_malloc(2 * IPDNSC_MAXPKT);
    if (s_buf == IP_NULL)
        return IP_NULL;
    r_buf = s_buf + IPDNSC_MAXPKT;

    /* Create the DNS query message */
    s_size = ipdnsc_create_query(dname, qtype, IPDNSC_QCLASS_INET, s_buf);
    if (s_size < 0)
    {
        *error = IPDNSC_NO_RECOVERY;
        goto fail;
    }

    /* Send the DNS query message and wait for a response */
    if (dns->primary_dns.family != 0)
        dns_addr = &dns->primary_dns;
    else if (dns->secondary_dns.family != 0)
        dns_addr = &dns->secondary_dns;
    else if (dns->tertiary_dns.family != 0)
        dns_addr = &dns->tertiary_dns;
    else if (dns->quaternary_dns.family != 0)
        dns_addr = &dns->quaternary_dns;
    else
    {
        *error = IPDNSC_NO_RECOVERY;
        goto fail;
    }
    retries = dns->retry;
    use_tcp = dns->use_tcp;

resend:
    if(use_tcp == IP_FALSE) /*udp*/
    {
        err = ipdnsc_send_wait(s_buf, s_size, r_buf, &r_size, dns_addr, dns->timeout);
    
        switch(err)
        {
            case IPDNSC_OK:
                break;
            case IPDNSC_SERV_TC_FLAG:
                use_tcp = IP_TRUE;
                goto resend;
            case IPDNSC_SERV_FAIL:
            case IPDNSC_SERV_NOTIMPL:
            case IPDNSC_SERV_REFUSED:
                /* Skip further retries */
                retries = 0;
                /* Fall through */
            case IPDNSC_SERV_TIMEOUT:
                if (retries-- > 0 )
                {
                    /* Retry same name server */
                    goto resend;
                }
                else if (dns_addr == &dns->primary_dns && dns->secondary_dns.family != 0)
                {
                    /* Jump to secondary name server */
                    dns_addr = &dns->secondary_dns;
                    retries = dns->retry;
                    goto resend;
                }
                else if ((dns_addr == &dns->primary_dns || dns_addr == &dns->secondary_dns) && dns->tertiary_dns.family != 0)
                {
                    /* Jump to tertiary name server */
                    dns_addr = &dns->tertiary_dns;
                    retries = dns->retry;
                    goto resend;
                }
                else if ((dns_addr == &dns->primary_dns || dns_addr == &dns->secondary_dns || dns_addr == &dns->tertiary_dns)
                            && dns->quaternary_dns.family != 0)
                {
                    /* Jump to quaternary name server */
                    dns_addr = &dns->quaternary_dns;
                    retries = dns->retry;
                    goto resend;
                }
                else
                {
                    /* Tried all nameservers */
                    *error = IPDNSC_NO_RECOVERY;
                }
                goto fail;

            case IPDNSC_SERV_NONAME:
                *error = IPDNSC_HOST_NOT_FOUND;
                goto fail;

            case IPDNSC_ERR_GENERAL:
            case IPDNSC_SERV_FORMAT:
            default:
                *error = IPDNSC_NO_RECOVERY;
                goto fail;
        }

        /* Parse the name server response */
        he = ipdnsc_parse_response(r_buf, r_size, family, cache_ttl, &num_answers);

        *error = (he != IP_NULL) ? IPDNSC_OK : IPDNSC_NO_RECOVERY;

        /* Test the special case of no answers */
        if (num_answers == 0)
        {
            *error = IPDNSC_NO_ADDRESS;
        }

        if(r_buf_tcp != IP_NULL)
        {
            ipcom_free(r_buf_tcp);
            r_buf_tcp = IP_NULL;
        }
        ipcom_free(s_buf);
        return he;
    }
    else /*tcp*/
    {
        if(r_buf_tcp != IP_NULL)
        {
            ipcom_free(r_buf_tcp);
            r_buf_tcp = IP_NULL;
        }
        
        err = ipdnsc_tcp_send_wait(s_buf, s_size, &r_buf_tcp, &r_size, dns_addr, dns->timeout);

            switch(err)
            {
                case IPDNSC_OK:
                    break;
                case IPDNSC_SERV_FAIL:
                case IPDNSC_SERV_NOTIMPL:
                case IPDNSC_SERV_REFUSED:
                case IPDNSC_SERV_TIMEOUT:
                    if (dns_addr == &dns->primary_dns && dns->secondary_dns.family != 0)
                    {
                        /* Jump to secondary name server */
                        dns_addr = &dns->secondary_dns;
                        retries = dns->retry;
                        use_tcp = dns->use_tcp;
                        goto resend;
                    }
                    else if ((dns_addr == &dns->primary_dns || dns_addr == &dns->secondary_dns) && dns->tertiary_dns.family != 0)
                    {
                        /* Jump to tertiary name server */
                        dns_addr = &dns->tertiary_dns;
                        retries = dns->retry;
                        use_tcp = dns->use_tcp;
                        goto resend;
                    }
                    else if ((dns_addr == &dns->primary_dns || dns_addr == &dns->secondary_dns || dns_addr == &dns->tertiary_dns)
                                && dns->quaternary_dns.family != 0)
                    {
                        /* Jump to quaternary name server */
                        dns_addr = &dns->quaternary_dns;
                        retries = dns->retry;
                        use_tcp = dns->use_tcp;
                        goto resend;
                    }
                    else
                    {
                        /* Tried all nameservers */
                        *error = IPDNSC_NO_RECOVERY;
                    }
                    goto fail;
                case IPDNSC_SERV_NONAME:
                    *error = IPDNSC_HOST_NOT_FOUND;
                    goto fail;
                
                case IPDNSC_ERR_GENERAL:
                case IPDNSC_SERV_FORMAT:
                default:
                    *error = IPDNSC_NO_RECOVERY;
                    goto fail;

                }

             /* Parse the name server response */
        he = ipdnsc_parse_response_2(r_buf_tcp, r_size, family, cache_ttl, &num_answers);

        *error = (he != IP_NULL) ? IPDNSC_OK : IPDNSC_NO_RECOVERY;

        /* Test the special case of no answers */
        if (num_answers == 0)
        {
            *error = IPDNSC_NO_ADDRESS;
        }

        if(r_buf_tcp != IP_NULL)
        {
            ipcom_free(r_buf_tcp);
            r_buf_tcp = IP_NULL;
        }
        ipcom_free(s_buf);
        return he;
    }
    
 fail:
    ipcom_free(s_buf);
    if(r_buf_tcp != IP_NULL)
    {
        ipcom_free(r_buf_tcp);
        r_buf_tcp = IP_NULL;
    }

    return IP_NULL;
}

#endif /*IPCOM_DNSC_TC_SUPPORT*/
/*
 *===========================================================================
 *                    ipdnsc_get_time
 *===========================================================================
 * Description: Retrives current dns time
 * Parameters:  None.
 * Returns:     Current time in seconds.
 */
IP_STATIC Ip_u32
ipdnsc_get_time(void)
{
#ifdef IPCOM_USE_TIME
    return ipcom_time(IP_NULL);
#else
    return 0;
#endif
}


/*
 *===========================================================================
 *                    ipdnsc_cache_remove
 *===========================================================================
 * Description: Removes an entry from the DNS cache
 * Parameters:  index to the cache entry to remove
 * Returns:     0 - OK
 *             -1 - Fail
 */
IP_STATIC Ip_s32
ipdnsc_cache_remove(Ipdnsc_cache *dns_cache)
{
    Ipdnsc_cache *tmp;

    /* Remove entry from list */
    tmp = ipdnsc.cache_head;
    if (tmp == dns_cache)
    {
        ipdnsc.cache_head = tmp->next;
    }
    else
    {
        while (tmp != IP_NULL)
        {
            if (tmp->next == dns_cache)
            {
                tmp->next = tmp->next->next;
                tmp = dns_cache;
                break;
            }
            tmp = tmp->next;
        }
    }

    /* Free the memory */
    if (tmp != IP_NULL)
    {
        ipdnsc_hostent_free(tmp->he);
        ipcom_free(tmp);
    }
    else
    {
        return -1;
    }

    return 0;
}


/*
 *===========================================================================
 *                    ipdnsc_cache_find_oldest
 *===========================================================================
 * Description: Finds the first entry in the DNS cache to expire
 * Parameters:  none.
 * Returns:     The cache entry.
 */
IP_STATIC Ipdnsc_cache *
ipdnsc_cache_find_oldest(void)
{
    Ipdnsc_cache *dns_cache, *dns_oldest;

    /* Search cache */
    dns_cache = ipdnsc.cache_head;
    dns_oldest = dns_cache;
    while (dns_cache != IP_NULL)
    {
        /* Check if older */
        if (dns_cache->expire < dns_oldest->expire)
        {
            dns_oldest = dns_cache;
        }
        dns_cache = dns_cache->next;
    }

    return dns_oldest;
}


/*
 *===========================================================================
 *                    ipdnsc_cache_find_expired
 *===========================================================================
 * Description: Finds the first expired entry in the DNS cache
 * Parameters:  none.
 * Returns:     The cache entry.
 */
IP_STATIC Ipdnsc_cache *
ipdnsc_cache_find_expired(void)
{
    Ipdnsc_cache *dns_cache;
    Ip_u32 now = ipdnsc_get_time();

    /* Search cache */
    dns_cache = ipdnsc.cache_head;
    while (dns_cache != IP_NULL)
    {
        /* Check if expired */
        if (dns_cache->expire < now)
        {
            return dns_cache;
        }
        dns_cache = dns_cache->next;
    }

    /* None expired */
    return IP_NULL;
}


/*
 *===========================================================================
 *                    ipdnsc_cache_clean
 *===========================================================================
 * Description: Remove an expired cache entry or the oldest entry.
 * Parameters:  none.
 * Returns:     0 - OK
 *             -1 - Fail
 */
IP_STATIC int
ipdnsc_cache_clean(void)
{
    Ipdnsc_cache *dns_cache;

    dns_cache = ipdnsc_cache_find_expired();
    if (dns_cache == IP_NULL)
    {
        dns_cache = ipdnsc_cache_find_oldest();
        if (dns_cache == IP_NULL || ipdnsc_cache_remove(dns_cache) != 0)
            return -1;
        ipdnsc.cache_entries--;
    }
    else
    {
        while (dns_cache != IP_NULL)
        {
            if (ipdnsc_cache_remove(dns_cache) != 0)
                return -1;

            ipdnsc.cache_entries--;
            dns_cache = ipdnsc_cache_find_expired();
        }
    }
    return 0;
}


/*
 *===========================================================================
 *                    ipdnsc_cache_add
 *===========================================================================
 * Description: Adds a new entry to the DNS cache. Will not check for
 *              duplicates
 * Parameters:  he  - pointer to the hostent structure to be added
 *              ttl - time to live for the new entry
 * Returns:     0 - OK
 *             -1 - Fail
 */
IP_STATIC Ip_s32
ipdnsc_cache_add(struct Ip_hostent *he, Ip_u32 type, Ip_u32 ttl)
{
    Ipdnsc_cache *dns_cache;
    Ip_u32 now = ipdnsc_get_time();

    if (ttl == 0)
    {
        /* No caching, just return ok */
        return 0;
    }

    if (ipdnsc.cache_entries >= IPDNSC_CONF_MAX_CACHE_ENTRIES)
    {
        if (ipdnsc_cache_clean() != 0)
        {
            return -1;
        }
    }

    /* Allocate the memory */
    dns_cache = ipcom_malloc(sizeof(*dns_cache));
    if (dns_cache == IP_NULL)
    {
        return -1;
    }
    dns_cache->he = ipdnsc_hostent_copy(he);
    if (dns_cache->he == IP_NULL)
    {
        ipcom_free(dns_cache);
        return -1;
    }

    if (ttl == 0xffffffff)
        dns_cache->expire = ttl;
    else
        dns_cache->expire = now + ttl;
    dns_cache->type = type;

    /* Insert entry first in the list */
    dns_cache->next = ipdnsc.cache_head;
    ipdnsc.cache_head = dns_cache;

    ipdnsc.cache_entries++;

    return 0;
}


/*
 *===========================================================================
 *                    ipdnsc_cache_find_addr
 *===========================================================================
 * Description: Checks if an address resides in the DNS cache
 * Parameters:  addr   - pointer to the requested address
 *              family - the address family
 * Returns:     Pointer a hostent structure including the requested address
 *              or IP_NULL if not found
 */
IP_STATIC struct Ip_hostent *
ipdnsc_cache_find_addr(Ip_u8 *addr, Ip_s32 family)
{
    Ipdnsc_cache *dns_cache;
    struct Ip_hostent *he = IP_NULL;
    Ip_u32 now = ipdnsc_get_time();

    /* Search cache */
    dns_cache = ipdnsc.cache_head;
    while (dns_cache != IP_NULL)
    {
        if (dns_cache->type == IPDNSC_CACHE_ADDR && dns_cache->he->h_addrtype == family)
        {
            if (!ipcom_memcmp(dns_cache->he->h_addr_list[0], addr, dns_cache->he->h_length))
            {
                /* Requested address found in the cache - check that it is still valid */
                if (dns_cache->expire >= now)
                {
                    he = ipdnsc_hostent_copy(dns_cache->he);
                    break;
                }
            }
        }
        dns_cache = dns_cache->next;
    }

    return he;
}


/*
 *===========================================================================
 *                    ipdnsc_cache_find_name
 *===========================================================================
 * Description: Checks if a name resides in the DNS cache
 * Parameters:  name - pointer to the requested name
 * Returns:     Pointer a hostent structure including the requested address
 *              or IP_NULL if not found
 */
IP_STATIC struct Ip_hostent *
ipdnsc_cache_find_name(Ip_u8 *name, Ip_s32 family)
{
    Ip_s32 i;
    Ipdnsc_cache *dns_cache;
    struct Ip_hostent *he = IP_NULL;
    Ip_u32 now = ipdnsc_get_time();

    /* Search cache */
    dns_cache = ipdnsc.cache_head;
    while (dns_cache != IP_NULL)
    {
        if (dns_cache->type == IPDNSC_CACHE_NAME && dns_cache->he->h_addrtype == family)
        {
            if (!ipcom_strcasecmp(dns_cache->he->h_name, (char *)name))
            {
                /* Requested name found in the cache - check that it is still valid */
                if (dns_cache->expire >= now)
                {
                    he = ipdnsc_hostent_copy(dns_cache->he);
                    break;
                }
            }
            else
            {
                /* Scan also for aliases */
                i=0;
                while (dns_cache->he->h_aliases[i] != IP_NULL)
                {
                    if (!ipcom_strcasecmp(dns_cache->he->h_aliases[i++], (char *)name))
                    {
                        /* Requested name found in the cache - check that it is still valid */
                        if (dns_cache->expire >= now)
                        {
                            he = ipdnsc_hostent_copy(dns_cache->he);
                            goto done;
                        }
                    }
                }
            }
        }
        dns_cache = dns_cache->next;
    }

done:
    return he;
}


/*
 *===========================================================================
 *                    ipdnsc_cache_known_hosts
 *===========================================================================
 * Description: Add the known hosts to the cache.
 * Parameters:  None
 * Returns:     IPCOM_SUCCESS or IPCOM_ERR_XXX error code.
 */
IP_STATIC Ip_err
ipdnsc_cache_known_hosts(void)
{
    struct Ip_hostent *he;
    Ip_u8 ipaddr[16];
    Ip_s32 family;
    struct host
    {
        char *addr;
        char *name;
    };
    struct host *hostp;
    struct host known_hosts[] = { IPDNSC_CONF_DNS_HOSTS };

    /* Add the host table to the cache */
    for (hostp = &known_hosts[0]; hostp->addr != IP_NULL; hostp++)
    {
        family = IP_AF_INET;
        /* Try an ipv4 address */
        if (ipcom_inet_pton(family, hostp->addr, ipaddr) <= 0)
        {
            /* Try an ipv6 address */
            family = IP_AF_INET6;
            if (ipcom_inet_pton(family, hostp->addr, ipaddr) <= 0)
            {
                /* Both failed, next entry */
                continue;
            }
        }

        /* Add the host table to the cache */
        he = ipdnsc_hostent_create(family);
        if (he == IP_NULL)
            return IPCOM_ERR_NO_MEMORY;

        /* Insert the host address */
        if (ipdnsc_hostent_insert_addr(he, (char *)ipaddr))
        {
            ipdnsc_hostent_free(he);
            return IPCOM_ERR_NO_MEMORY;
        }
        /* Insert the host name */
        if (ipdnsc_hostent_insert_name(he, hostp->name))
        {
            ipdnsc_hostent_free(he);
            return IPCOM_ERR_NO_MEMORY;
        }

        ipcom_mutex_lock(ipdnsc.mutex);
        /* Add the host entry to the cache */
        (void)ipdnsc_cache_add(he, IPDNSC_CACHE_NAME, 0xffffffff);
        /* Add the host entry to the cache */
        (void)ipdnsc_cache_add(he, IPDNSC_CACHE_ADDR, 0xffffffff);
        ipcom_mutex_unlock(ipdnsc.mutex);
        ipdnsc_hostent_free(he);
    }

    return IPCOM_SUCCESS;
}


/*
 ****************************************************************************
 * 10                   GLOBAL FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                      ipdnsc_get_ipv6_addrconfig
 *===========================================================================
 * Description: Parse an IPv6 address
 * Parameters:  addr6   - pointer to the returned IPv6 address.
 *              ifindex - pointer to the returned interface index.
 *              addrstr - IPv6 address to parse.
 * Returns:     0 = ok.
 *             -1 = failed.
 */
#ifdef IPCOM_USE_INET6
IP_GLOBAL Ip_s32
ipdnsc_get_ipv6_addrconfig(struct Ip_in6_addr *addr6, Ip_u32 *ifindex, char *addrstr)
{
    char *ifname;

    /* Get the address */
    if (ipcom_inet_pton(IP_AF_INET6, addrstr, addr6) != 1)
        return -1;

    /* Get the interface */
    *ifindex = 0;
    ifname = ipcom_strchr(addrstr, IPDNSC_IF_DELIMITER);
    if (ifname != IP_NULL && *ifname != '\0')
    {
        ifname++;
        if (ipcom_isdigit(*ifname))
        {
            *ifindex = (Ip_u32)ipcom_atoi(ifname);
        }
        else
        {
            *ifindex = ipcom_if_nametoindex(ifname);
            if (*ifindex == 0)
                return -1;
        }
    }

    return 0;
}
#endif /* IPCOM_USE_INET6 */


/*
 *===========================================================================
 *                    ipdnsc_default
 *===========================================================================
 * Description: Get the default DNS server information.
 * Parameters:  dns - pointer to the DNS server structure.
 * Returns:     0 = ok.
 *             -1 = failed.
 */
IP_GLOBAL Ip_s32
ipdnsc_default(Ipdnsc_ns_spec *dns)
{
    Ip_u16 port_n, i;
	char *ptr;
    Ipdnsc_ns_addr *ns_addr;

    ipcom_memset(dns, 0, sizeof(*dns));

    /* Configure DNS server address/ports */
    port_n = (Ip_u16)ipcom_sysvar_get_as_int ("ipdnsc.port", IPDNSC_DEFAULT_DNS_PORT);
    port_n = ip_htons(port_n);
    for (i = 0; i < 4; i++)
    {
        switch (i)
        {
        case 0:
	        ptr = ipcom_sysvar_get ("ipdnsc.primaryns", IP_NULL, IP_NULL);
            ns_addr = &dns->primary_dns;
            break;
        case 1:
            ptr = ipcom_sysvar_get ("ipdnsc.secondaryns", IP_NULL, IP_NULL);
            ns_addr = &dns->secondary_dns;
            break;
        case 2:
            ptr = ipcom_sysvar_get ("ipdnsc.tertiaryns", IP_NULL, IP_NULL);
            ns_addr = &dns->tertiary_dns;
            break;
        case 3:
            ptr = ipcom_sysvar_get ("ipdnsc.quaternaryns", IP_NULL, IP_NULL);
            ns_addr = &dns->quaternary_dns;
            break;
        default:
            return -1;
        }

	    if (ptr != IP_NULL)
	    {
		    ns_addr->family = IP_AF_INET;
		    if (ipcom_inet_pton (ns_addr->family,
							     ptr,
							     &ns_addr->addr) != 1)
		    {
#ifdef IPCOM_USE_INET6
                ns_addr->family = IP_AF_INET6;
                if (ipdnsc_get_ipv6_addrconfig(&ns_addr->addr.inaddr6, &ns_addr->ifindex, ptr) < 0)
#endif
                    /* Bad address */
                    ns_addr->family = 0;
            }
		    ipcom_free(ptr);
	    }
        ns_addr->port = port_n;
    }

    /* Configure other DNS parameters */
    dns->timeout = ipcom_sysvar_get_as_int ("ipdnsc.timeout", IPDNSC_DEFAULT_DNS_TIMEOUT);
    dns->retry   = ipcom_sysvar_get_as_int ("ipdnsc.retries", IPDNSC_DEFAULT_DNS_RETRY);
#ifdef IPCOM_DNSC_TC_SUPPORT
    dns->use_tcp = ipcom_sysvar_get_as_int ("ipdnsc.use_tcp", 0);
#endif    
    return 0;
}


/*
 *===========================================================================
 *                    ipdnsc_resolve_name
 *===========================================================================
 * Description: Resolve a name.
 * Parameters:  name      - name to resolve.
 *              family    - address family.
 *              use_cache - check to cache before performing name lookup.
 *              dns       - pointer to the DNS structure.
 *              error     - type of error if lookup fails.
 * Returns:     pointer to the hostent structure or IP_NULL if failed.
 */
IP_GLOBAL struct Ip_hostent*
ipdnsc_resolve_name(Ip_u8 *name, Ip_s32 family, Ip_s32 use_cache, Ipdnsc_ns_spec *dns, Ip_s32 *error_num)
{
    Ip_u16 qtype;
    Ip_u32 ttl;
    struct Ip_hostent *he;
    Ip_u8 hostname[IPDNSC_MAXNAME];
	char *domain;
    Ip_s32 len;
    Ip_bool initiated;

    ipcom_mutex_lock(ipdnsc.mutex);
    initiated = ipdnsc.init;
    ipcom_mutex_unlock(ipdnsc.mutex);
    if (initiated == IP_FALSE)
    {
        *error_num = IPDNSC_NO_RECOVERY;
        he = IP_NULL;
        goto done;
    }

    if (error_num == IP_NULL)
    {
        he = IP_NULL;
        goto done;
    }

    if (dns == IP_NULL)
    {
        *error_num = IPDNSC_NO_RECOVERY;
        he = IP_NULL;
        goto done;
    }

    /* Check family */
    switch(family)
    {
        case IP_AF_INET:
            qtype = IPDNSC_QTYPE_A;
            break;

        case IP_AF_INET6:
            qtype = IPDNSC_QTYPE_AAAA;
            break;

        default:
            *error_num = IPDNSC_NO_RECOVERY;
            he = IP_NULL;
            goto done;
    }

    /* Copy name to local buf */
    len = ipcom_strlen((char *)name);
    if (len <= 0 || len > (IPDNSC_MAXNAME-1))
    {
        *error_num = IPDNSC_NO_RECOVERY;
        he = IP_NULL;
        goto done;
    }
    ipcom_memcpy(hostname, name, len);
    hostname[len] = '\0';

    /* Check for fully qualified domain name */
    if (hostname[len-1] == '.')
    {
        /* Remove the last dot */
        hostname[len-1] = '\0';
    }
    else if (ipcom_strcasecmp((char *)hostname, "localhost") != 0 && ipcom_strcasecmp((char *)hostname, "localhost6") != 0)
    {
      	/* Append our domain if no dots */
        if (!ipcom_strchr((char *)hostname, '.'))
        {
			domain = ipcom_sysvar_get("ipdnsc.domainname", IP_NULL, IP_NULL);
			if (domain != IP_NULL && *domain != '\0')
			{
            	hostname[len++] = '.';
            	if ((len + ipcom_strlen(domain)) > (IPDNSC_MAXNAME-1))
            	{
              		ipcom_free (domain);
                	*error_num = IPDNSC_NO_RECOVERY;
                	he = IP_NULL;
                	goto done;
            	}
            	ipcom_strcpy((char *)&hostname[len], domain);
            	hostname[len + ipcom_strlen(domain)] = '\0';
            	ipcom_free (domain);
			}
            else
            {
                if (domain != IP_NULL)
                    ipcom_free (domain);
            }
        }
    }

    /* Check the cache */
    if (use_cache)
    {
        ipcom_mutex_lock(ipdnsc.mutex);
        he = ipdnsc_cache_find_name(hostname, family);
        ipcom_mutex_unlock(ipdnsc.mutex);
        if (he != IP_NULL)
        {
            *error_num = IPDNSC_OK;
            goto done;
        }
    }

    /* Do the query */
    he = ipdnsc_do_query(hostname, qtype, family, dns, error_num, &ttl);
    if (he == IP_NULL)
        goto done;

    /* Add the host entry to the cache */
    if (use_cache)
    {
        ipcom_mutex_lock(ipdnsc.mutex);
        (void)ipdnsc_cache_add(he, IPDNSC_CACHE_NAME, ttl);
        ipcom_mutex_unlock(ipdnsc.mutex);
    }

done:
    return he;
}


/*
 *===========================================================================
 *                    ipdnsc_resolve_addr
 *===========================================================================
 * Description: Resolve an address.
 * Parameters:  addr      - pointer to the address.
 *              family    - address family.
 *              use_cache - check to cache before performing name lookup.
 *              dns       - pointer to the DNS structure.
 *              error     - type of error if lookup fails.
 * Returns:     pointer to the hostent structure or IP_NULL if failed.
 */
IP_GLOBAL struct Ip_hostent*
ipdnsc_resolve_addr(Ip_u8 *addr, Ip_s32 family, Ip_s32 use_cache, Ipdnsc_ns_spec *dns, Ip_s32 *error_num)
{
    struct Ip_hostent *he;
    Ip_u8 ptr_name[128];
    Ip_u32 ttl;
    Ip_bool initiated;
    Ip_u16 qtype;

    ipcom_mutex_lock(ipdnsc.mutex);
    initiated = ipdnsc.init;
    ipcom_mutex_unlock(ipdnsc.mutex);
    if (initiated == IP_FALSE)
    {
        *error_num = IPDNSC_NO_RECOVERY;
        he = IP_NULL;
        goto done;
    }

    if (error_num == IP_NULL)
    {
        he = IP_NULL;
        goto done;
    }

    if (dns == IP_NULL)
    {
        *error_num = IPDNSC_NO_RECOVERY;
        he = IP_NULL;
        goto done;
    }

    /* Check family */
    switch(family)
    {
        case IP_AF_INET:
        case IP_AF_INET6:
            qtype = IPDNSC_QTYPE_PTR;
            break;

        default:
            *error_num = IPDNSC_NO_RECOVERY;
            he = IP_NULL;
            goto done;
    }

    /* Check the cache */
    if (use_cache)
    {
        ipcom_mutex_lock(ipdnsc.mutex);
        he = ipdnsc_cache_find_addr(addr, family);
        ipcom_mutex_unlock(ipdnsc.mutex);
        if (he != IP_NULL)
        {
            *error_num = IPDNSC_OK;
            goto done;
        }
    }

    /* Convert address to special domain name */
    if (ipdnsc_ptr_name(ptr_name, sizeof(ptr_name), addr, family))
    {
        *error_num = IPDNSC_NO_RECOVERY;
        he = IP_NULL;
        goto done;
    }

    /* Do the query */
    he = ipdnsc_do_query(ptr_name, qtype, family, dns, error_num, &ttl);
    if (he == IP_NULL)
        goto done;

    /* Insert the host address in the host address structure */
    if (ipdnsc_hostent_insert_addr(he, (char *)addr))
    {
        ipdnsc_hostent_free(he);
        *error_num = IPDNSC_NO_RECOVERY;
        he = IP_NULL;
        goto done;
    }

    /* Add the host entry to the cache */
    if (use_cache)
    {
        ipcom_mutex_lock(ipdnsc.mutex);
        (void)ipdnsc_cache_add(he, IPDNSC_CACHE_ADDR, ttl);
        ipcom_mutex_unlock(ipdnsc.mutex);
    }

done:
    return he;
}


/*
 ****************************************************************************
 * 11                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipdnsc_cache_flush
 *===========================================================================
 * Description: See ipdnsc.h for description.
 * Parameters:
 * Returns:
 */
IP_PUBLIC void
ipdnsc_cache_flush(void)
{
    ipcom_mutex_lock(ipdnsc.mutex);
    while (ipdnsc.cache_head != IP_NULL)
    {
        (void)ipdnsc_cache_remove(ipdnsc.cache_head);
        ipdnsc.cache_entries--;
    }
    ip_assert(ipdnsc.cache_entries == 0);
    ipcom_mutex_unlock(ipdnsc.mutex);
    (void)ipdnsc_cache_known_hosts();
}


/*
 *===========================================================================
 *                  ipdnsc_create
 *===========================================================================
 * Description: DNS client create function.
 * Parameters:  None.
 * Returns:     IPCOM_SUCCESS
 */
IP_PUBLIC Ip_err
ipdnsc_create(void)
{
    ipcom_memset(&ipdnsc, 0, sizeof(ipdnsc));

    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    ipdnsc_start
 *===========================================================================
 * Description: DNS client start function.
 * Parameters:  None.
 * Returns:     IPCOM_SUCCESS or IPCOM_ERR_XXX error code.
 */
IP_PUBLIC Ip_err
ipdnsc_start(void)
{
    Ip_err retval;

    if (ipcom_mutex_create(&ipdnsc.mutex) != IPCOM_SUCCESS)
        return IPCOM_ERR_NO_MEMORY;
#ifdef IPCOM_DNSC_TC_SUPPORT
    if (ipcom_mutex_create(&ipdnsc.user_mutex) != IPCOM_SUCCESS)
        return IPCOM_ERR_NO_MEMORY;
#endif

    /* Initialise DNS variables */
    ipcom_mutex_lock(ipdnsc.mutex);
    ipdnsc.cache_head = IP_NULL;
    ipcom_mutex_unlock(ipdnsc.mutex);

    retval = ipdnsc_cache_known_hosts();
    if (retval != IPCOM_SUCCESS)
    {
        (void)ipcom_mutex_delete(&ipdnsc.mutex);
        return retval;
    }

    ipcom_mutex_lock(ipdnsc.mutex);
    ipdnsc.init = IP_TRUE;

    ipcom_dns_init_ok = 1;
    ipcom_dns.getipnodebyname = ipdnsc_getipnodebyname;
    ipcom_dns.getipnodebyaddr = ipdnsc_getipnodebyaddr;
    ipcom_dns.freehostent = ipdnsc_freehostent;
    ipcom_dns.cache_flush = ipdnsc_cache_flush;
#ifdef IPCOM_DNSC_TC_SUPPORT
    ipdnsc.current_users = 0;
#endif

    ipcom_mutex_unlock(ipdnsc.mutex);

    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    ipdnsc_version
 *===========================================================================
 * Description: Get DNS client version string.
 * Parameters:  None.
 * Returns:     Pointer to the version string.
 */
IP_PUBLIC const char *
ipdnsc_version(void)
{
    return "@(#) IPDNSC $Name: VXWORKS_ITER18A_FRZ10 $ - INTERPEAK_COPYRIGHT_STRING";
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

