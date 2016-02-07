/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_inet_pton.c,v $ $Revision: 1.17.6.1.4.2 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/src/ipcom_inet_pton.c,v $
 *   $Author: ulf $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Kenneth Jonsson <kenneth@interpeak.se>
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
#include "ipcom_clib.h"
#include "ipcom_sock.h"
#ifdef IPCOM_USE_INET6
#include "ipcom_sock6.h"
#endif

/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

#define IPCOM_DEEP_CPY(pdst, psrc) ipcom_memcpy(pdst, psrc, sizeof(*pdst))

/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

#ifdef IPCOM_USE_INET
struct Ipcom_inet_pton_ctx {
    char              *str;
    int                next_part;
    Ip_u32             part[4];
};
#endif

#ifdef IPCOM_USE_INET6
struct Ipcom_inet6_pton_ctx {
    char               *str;
    int                *next_part;
    Ip_u16             *part;
    int                 next_head_part;
    Ip_u16              head_part[8];
    int                 next_tail_part;
    Ip_u16              tail_part[8];
#ifdef IPCOM_USE_INET
    Ip_bool             has_inaddr;
    struct Ip_in_addr   inaddr;
#endif
};
#endif /* IPCOM_USE_INET6 */


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

/*
 ****************************************************************************
 * 9                    STATIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                        ipcom_is_valid_addr_end
 *===========================================================================
 * Description: Checks if the character is a valid address delimiter.
 * Parameters:  tkn - first character after an IP address.
 * Returns:     IP_TRUE - is a valid character to end an address
 *              IP_FALSE - not valid
 */
IP_STATIC Ip_bool
ipcom_is_valid_addr_end(char tkn)
{
    return tkn == '\0'
        || tkn == '/'         /* support for "A/prefix" format */
        || tkn == '%'         /* support for "A%scope" format */
        || ipcom_isspace(tkn);
}


#ifdef IPCOM_USE_INET
/*
 *===========================================================================
 *                            ipcom_parse_num
 *===========================================================================
 * Description: Parses a hex, octal or decimal number
 * Parameters:  ctx - parser context
 *              limit - maximum allowed value of the number
 * Returns:     IP_FALSE - failed to parse the number or out of limit
 *              IP_TRUE - success
 */
IP_STATIC Ip_bool
ipcom_parse_num(struct Ipcom_inet_pton_ctx *ctx, Ip_u32 limit)
{
    char *endptr;

    ctx->part[ctx->next_part] = ipcom_strtoul(ctx->str, &endptr, 0);
    if (ctx->str == endptr)
        /* No digits where found */
        return IP_FALSE;

    if (ctx->part[ctx->next_part] > limit)
        /* Not within limit */
        return IP_FALSE;

    ctx->str = endptr;
    ctx->next_part++;
    return IP_TRUE;
}


/*
 *===========================================================================
 *                          ipcom_parse_num8
 *===========================================================================
 * Description: Parses a 8-bit number
 * Parameters:  ctx - parser context
 * Returns:     IP_FALSE - failed to parse number
 *              IP_TRUE - success
 */
IP_STATIC Ip_bool
ipcom_parse_num8(struct Ipcom_inet_pton_ctx *ctx)
{
    return ipcom_parse_num(ctx, 0xff);
}


/*
 *===========================================================================
 *                           ipcom_parse_num16
 *===========================================================================
 * Description: Parses a 16-bit number
 * Parameters:  ctx - parser context
 * Returns:     IP_FALSE - failed to parse number
 *              IP_TRUE - success
 */
IP_STATIC Ip_bool
ipcom_parse_num16(struct Ipcom_inet_pton_ctx *ctx)
{
    return ipcom_parse_num(ctx, 0xffff);
}


/*
 *===========================================================================
 *                           ipcom_parse_num24
 *===========================================================================
 * Description: Parses a 24-bit number
 * Parameters:  ctx - parser context
 * Returns:     IP_FALSE - failed to parse number
 *              IP_TRUE - success
 */
IP_STATIC Ip_bool
ipcom_parse_num24(struct Ipcom_inet_pton_ctx *ctx)
{
    return ipcom_parse_num(ctx, 0xffffff);
}


/*
 *===========================================================================
 *                           ipcom_parse_num32
 *===========================================================================
 * Description: Parses a 32-bit number
 * Parameters:  ctx - parser context
 * Returns:     IP_FALSE - failed to parse number
 *              IP_TRUE - success
 */
IP_STATIC Ip_bool
ipcom_parse_num32(struct Ipcom_inet_pton_ctx *ctx)
{
    return ipcom_parse_num(ctx, 0xffffffff);
}


/*
 *===========================================================================
 *                            ipcom_parse_dot
 *===========================================================================
 * Description: Parses a single dot ('.')
 * Parameters:  ctx - parser context
 * Returns:     IP_FALSE - failed to parse a dot
 *              IP_TRUE - success
 */
IP_STATIC Ip_bool
ipcom_parse_dot(struct Ipcom_inet_pton_ctx *ctx)
{
    if (ctx->str[0] != '.')
        return IP_FALSE;
    ++ctx->str;
    return IP_TRUE;
}


/*
 *===========================================================================
 *                         ipcom_inet_ctx_to_addr
 *===========================================================================
 * Description: Converts the context to a binary form of a IPv4 address
 * Parameters:  ctx - parser context
 *              addr - pointer to where the binary form should be stored
 * Returns:     0 - parser error detected
 *              1 - success
 */
IP_STATIC int
ipcom_inet_ctx_to_addr(struct Ipcom_inet_pton_ctx *ctx,
                       struct Ip_in_addr *addr)
{
    if (!ipcom_is_valid_addr_end(*ctx->str))
        /* Garbage at end of address */
        return 0;

    switch (ctx->next_part)
    {
    case 1:
        /* num32 */
        addr->s_addr = ctx->part[0];
        break;
    case 2:
        /* num8.num24 */
        addr->s_addr = ((ctx->part[0] << 24)
                        + ctx->part[1]);
        break;
    case 3:
        /* num8.num8.num16 */
        addr->s_addr = ((ctx->part[0] << 24)
                        + (ctx->part[1] << 16)
                        + ctx->part[2]);
        break;
    default:
        /* num8.num8.num8.num8 */
        addr->s_addr = ((ctx->part[0] << 24)
                        + (ctx->part[1] << 16)
                        + (ctx->part[2] << 8)
                        + ctx->part[3]);
        break;
    }
    addr->s_addr = ip_htonl(addr->s_addr);
    return 1;
}


/*
 *===========================================================================
 *                            ipcom_parse_inet_init
 *===========================================================================
 * Description: Initializes the parser context.
 * Parameters:  ctx - context placeholder.
 *              strptr - string to parse
 * Returns:
 */
IP_STATIC void
ipcom_parse_inet_init(struct Ipcom_inet_pton_ctx *ctx, IP_CONST char *strptr)
{
    ctx->next_part = 0;
    ctx->str       = (char *) strptr;
}


/*
 *===========================================================================
 *                         ipcom_parse_inet_addr
 *===========================================================================
 * Description: Tries to parse a string as an IPv4 address
 * Parameters:  strptr - pointer to string
 *              addrptr - pointer to where the binary representation should
 *                        be stored.
 * Returns:     0 = not a valid IPv4 address
 *              1 = parse successful
 */
IP_STATIC int
ipcom_parse_inet_addr(IP_CONST char *strptr, void *addrptr)
{
    struct Ipcom_inet_pton_ctx ctx;

    /*
     * <inet-addr> ::= <num8>.<num8>.<num8>.<num8>
     *                 | <num8>.<num8>.<num16>
     *                 | <num8>.<num24>
     *                 | <num32>
     *
     * num8  is any number between 0-255
     * num16 is any number between 0-65535
     * num24 is any number between 0-16777215
     * num32 is any number between 0-4294967295
     */

    /* Try num8.num8.num8.num8 */
    ipcom_parse_inet_init(&ctx, strptr);
    if (ipcom_parse_num8(&ctx)
        && ipcom_parse_dot(&ctx)
        && ipcom_parse_num8(&ctx)
        && ipcom_parse_dot(&ctx)
        && ipcom_parse_num8(&ctx)
        && ipcom_parse_dot(&ctx)
        && ipcom_parse_num8(&ctx))
        return ipcom_inet_ctx_to_addr(&ctx, addrptr);

    /* Try num8.num8.num16 */
    ipcom_parse_inet_init(&ctx, strptr);
    if (ipcom_parse_num8(&ctx)
        && ipcom_parse_dot(&ctx)
        && ipcom_parse_num8(&ctx)
        && ipcom_parse_dot(&ctx)
        && ipcom_parse_num16(&ctx))
        return ipcom_inet_ctx_to_addr(&ctx, addrptr);

    /* Try num8.num24 */
    ipcom_parse_inet_init(&ctx, strptr);
    if (ipcom_parse_num8(&ctx)
        && ipcom_parse_dot(&ctx)
        && ipcom_parse_num24(&ctx))
        return ipcom_inet_ctx_to_addr(&ctx, addrptr);

    /* Try num32 */
    ipcom_parse_inet_init(&ctx, strptr);
    if (ipcom_parse_num32(&ctx))
        return ipcom_inet_ctx_to_addr(&ctx, addrptr);

    return 0;
}

#endif /* IPCOM_USE_INET */


#ifdef IPCOM_USE_INET6
/*
 *===========================================================================
 *                          ipcom_parse_colon
 *===========================================================================
 * Description: Parses a single dot (':')
 * Parameters:  ctx - parser context
 * Returns:     IP_FALSE - failed to parse a colon
 *              IP_TRUE - success
 */
IP_STATIC Ip_bool
ipcom_parse_colon(struct Ipcom_inet6_pton_ctx *ctx)
{
    if (ctx->str[0] != ':')
        return IP_FALSE;
    ++ctx->str;
    return IP_TRUE;
}


/*
 *===========================================================================
 *                        ipcom_parse_double_colon
 *===========================================================================
 * Description: Parses a double colon, update context to parse tail part
 *              if the call is successful.
 * Parameters:  ctx - parse context
 * Returns:     IP_FALSE - failed to parse double colon
 *              IP_TRUE - success
 */
IP_STATIC Ip_bool
ipcom_parse_double_colon(struct Ipcom_inet6_pton_ctx *ctx)
{
    if (ctx->next_part == &ctx->next_tail_part)
        /* More than one double colon */
        return IP_FALSE;

    ctx->next_part      = &ctx->next_tail_part;
    ctx->part           = ctx->tail_part;
    return ipcom_parse_colon(ctx) && ipcom_parse_colon(ctx);
}


/*
 *===========================================================================
 *                            ipcom_parse_hex4
 *===========================================================================
 * Description: Parses a 16-bit hex string.
 * Parameters:  ctx - parse context
 * Returns:     IP_FALSE - failed to parse number or too big number
 *              IP_TRUE - success
 */
IP_STATIC Ip_bool
ipcom_parse_hex4(struct Ipcom_inet6_pton_ctx *ctx)
{
    char         *endptr;
    unsigned long hex4;

    if (*ctx->next_part >= 8)
        /* Too many hex4 entries */
        return IP_FALSE;

    if (ipcom_strncmp(ctx->str, "0x", 2) == 0)
        /* IPv6 hex digits must not be prefixed with "0x" */
        return IP_FALSE;

    hex4 = ipcom_strtoul(ctx->str, &endptr, 16);

    if (ctx->str == endptr)
        /* Could not ready any number */
        return IP_FALSE;

    if (*endptr == '.')
        /* Looks like a part of an IPv4 address */
        return IP_FALSE;

    ctx->str = endptr;

    if (hex4 > 0xffff)
        /* Value too big */
        return IP_FALSE;

    ctx->part[*ctx->next_part] = (Ip_u16) hex4;
    ++(*ctx->next_part);

    return IP_TRUE;
}


/*
 *===========================================================================
 *                           ipcom_parse_hexseq
 *===========================================================================
 * Description: Parses a sequence of hex4 elements
 * Parameters:  ctx - parse context
 * Returns:     IP_FALSE - failed to parse
 *              IP_TRUE - success
 *
 */
IP_STATIC Ip_bool
ipcom_parse_hexseq(struct Ipcom_inet6_pton_ctx *ctx)
{
    /* <hexseq> ::= <hex4> *( ":" <hex4> ) */
    if (!ipcom_parse_hex4(ctx))
        return IP_FALSE;

    for (;;)
    {
        struct Ipcom_inet6_pton_ctx rollback_point;

        IPCOM_DEEP_CPY(&rollback_point, ctx);
        if (!ipcom_parse_colon(ctx)
            || !ipcom_parse_hex4(ctx))
        {
            IPCOM_DEEP_CPY(ctx, &rollback_point);
            break;
        }
    }

    return IP_TRUE;
}


/*
 *===========================================================================
 *                          ipcom_parse_hexpart
 *===========================================================================
 * Description: Parses the hex + colon part of the IPv6 address.
 * Parameters:  ctx - parse context
 * Returns:     IP_FALSE - failed to parse
 *              IP_TRUE - success
 */
IP_GLOBAL Ip_bool
ipcom_parse_hexpart(struct Ipcom_inet6_pton_ctx *ctx)
{
    struct Ipcom_inet6_pton_ctx rollback_point;
    Ip_bool                     success = IP_FALSE;

    /*
     * Try to parse this:
     * <hexseq> | <hexseq> "::" [ <hexseq> ] | "::" [ <hexseq> ]
     */
    IPCOM_DEEP_CPY(&rollback_point, ctx);
    if (ipcom_parse_hexseq(ctx))
    {
        /*
         * Success with one of <hexseq> | <hexseq> "::" [ <hexseq> ]
         */
        success = IP_TRUE;
        IPCOM_DEEP_CPY(&rollback_point, ctx);
    }

    if (ipcom_parse_double_colon(ctx))
    {
        /*
         * Success with one of <hexseq> "::" [ <hexseq> ] | "::" [ <hexseq> ]
         */
        success = IP_TRUE;
        IPCOM_DEEP_CPY(&rollback_point, ctx);
        if (ipcom_parse_hexseq(ctx))
            /*
             * Success with optional <hexseq> at end of expression.
             */
            IPCOM_DEEP_CPY(&rollback_point, ctx);
    }

    IPCOM_DEEP_CPY(ctx, &rollback_point);
    return success;
}


/*
 *===========================================================================
 *                        ipcom_inet6_ctx_to_addr
 *===========================================================================
 * Description: Combines the head and tail part of the IPv6 address
 *              and stores the result in "addrptr".
 * Parameters:  ctx - parse context
 *              addrptr - buffer where the IPv6 address should be stored
 * Returns:     0 = parse error, not a valid IPv6 address
 *              1 = success
 */
IP_STATIC int
ipcom_inet6_ctx_to_addr(struct Ipcom_inet6_pton_ctx *ctx, void *addrptr)
{
    Ip_u16 *addr = addrptr;
    int     i;

    if (!ipcom_is_valid_addr_end(*ctx->str))
        return 0;

#ifdef IPCOM_USE_INET
    if (ctx->has_inaddr)
    {
        if (ctx->next_tail_part <= 6)
        {
            Ip_u16 *dst16 = &ctx->tail_part[ctx->next_tail_part];
            ipcom_memcpy(dst16,
                         &ctx->inaddr,
                         sizeof(ctx->inaddr));
            /* All data is stored in host endian up to this point, the
               IPv4 address is in network byte-order. */
            dst16[0] = ip_ntohs(dst16[0]);
            dst16[1] = ip_ntohs(dst16[1]);
        }
        ctx->next_tail_part += 2;
    }
#endif

    if (ctx->next_head_part + ctx->next_tail_part > 8)
        /* Too many parts */
        return 0;

    while (ctx->next_head_part + ctx->next_tail_part < 8)
        ctx->head_part[ctx->next_head_part++] = 0;

    for (i = 0; i < ctx->next_head_part; i++)
        *addr++ = ip_htons(ctx->head_part[i]);
    for (i = 0; i < ctx->next_tail_part; i++)
        *addr++ = ip_htons(ctx->tail_part[i]);

    return 1;
}


/*
 *===========================================================================
 *                         ipcom_parse_inet_part
 *===========================================================================
 * Description: Parses an IPv4 address at the end of a IPv6 address
 * Parameters:  ctx - parse context
 * Returns:     IP_FALSE - parse failed
 *              IP_TRUE - success
 */
#ifdef IPCOM_USE_INET
IP_STATIC Ip_bool
ipcom_parse_inet_part(struct Ipcom_inet6_pton_ctx *ctx)
{
    if (ipcom_parse_inet_addr(ctx->str, &ctx->inaddr))
    {
        /* IPv4 address was present at the end */
        ctx->str += ipcom_strlen(ctx->str);
        ctx->has_inaddr = IP_TRUE;
        return IP_TRUE;
    }
    return IP_FALSE;
}
#endif /* IPCOM_USE_INET */


/*
 *===========================================================================
 *                         ipcom_parse_inet6_init
 *===========================================================================
 * Description: Initializes the inet6 parser.
 * Parameters:  ctx - parser context
 *              strptr - string to parse
 * Returns:
 *
 */
IP_STATIC void
ipcom_parse_inet6_init(struct Ipcom_inet6_pton_ctx *ctx, IP_CONST char *strptr)
{
    ctx->str            = (char *) strptr;
    ctx->part           = ctx->head_part;
    ctx->next_part      = &ctx->next_head_part;
    ctx->next_head_part = 0;
    ctx->next_tail_part = 0;
}


/*
 *===========================================================================
 *                        ipcom_parse_inet6_addr
 *===========================================================================
 * Description: Tries to parse a string as an IPv6 address
 * Parameters:  strptr - pointer to string
 *              addrptr - pointer to where the binary representation should
 *                        be stored.
 * Returns:     0 = not a valid IPv6 address
 *              1 = parse successful
 */
IP_STATIC int
ipcom_parse_inet6_addr(IP_CONST char *strptr, void *addrptr)
{
    struct Ipcom_inet6_pton_ctx ctx;

    /*
     * The IPv6 address format is described in RFC 2373.
     *
     * <inet6-addr> ::= <hexpart> [ [ ":" ] <inet4-addr> ] | "::" <inet4-addr>
     * <hexpart>    ::= <hexseq> | <hexseq> "::" [ <hexseq> ] | "::" [ <hexseq> ]
     * <hexseq>     ::= <hex4> *( ":" <hex4> )
     * <hex4>       ::= 1*4HEXDIGIT
     */

    ipcom_parse_inet6_init(&ctx, strptr);
    if (ipcom_parse_hexpart(&ctx))
    {
#ifdef IPCOM_USE_INET
        ctx.has_inaddr = IP_FALSE;
        /*
         * The colon will be missing if the IPv4 address is preceded
         * with a double colon.
         */
        (void)ipcom_parse_colon(&ctx);
        /*
         * The returns value must not be checked since the IPv4
         * address is optional. The ipcom_inet6_ctx_to_addr() will
         * fail if there are character that was not processed at the
         * end.
         */
        (void)ipcom_parse_inet_part(&ctx);
#endif
        if (ipcom_inet6_ctx_to_addr(&ctx, addrptr))
            return 1;
    }

#ifdef IPCOM_USE_INET
    ipcom_parse_inet6_init(&ctx, strptr);
    if (ipcom_parse_double_colon(&ctx)
        && ipcom_parse_inet_part(&ctx))
        return ipcom_inet6_ctx_to_addr(&ctx, addrptr);
#endif /* IPCOM_USE_INET */

    return 0;
}
#endif /* IPCOM_USE_INET6 */


/*
 ****************************************************************************
 * 10                   GLOBAL FUNCTIONS
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 11                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipcom_inet_pton
 *===========================================================================
 * Description: Converts a string representation of a address to its binary
 *              form.
 * Parameters:  domain - The domain for the addresss (IP_AF_INET or
 *              IP_AF_INET6).
 *              strptr - A string representation of the address.
 *              addrptr - Points to the buffer that will receive the
 *              binary for of the address.
 * Returns:     1 = success, 0 = the string was not a valid address
 *              <0 = error (ipcom_errno set)
 *
 */
IP_PUBLIC int
ipcom_inet_pton(int family, IP_CONST char *strptr, void *addrptr)
{
    union Ip_in_addr_union addr;
    int                    ret;

    switch(family)
    {
#ifdef IPCOM_USE_INET
    case IP_AF_INET:
        ret = ipcom_parse_inet_addr(strptr, &addr);
        ipcom_memcpy(addrptr, &addr, sizeof(addr.in));
        return ret;
#endif /* IPCOM_USE_INET */

#ifdef IPCOM_USE_INET6
    case IP_AF_INET6:
        ret = ipcom_parse_inet6_addr(strptr, &addr);
        ipcom_memcpy(addrptr, &addr, sizeof(addr.in6));
        return ret;
#endif /* IPCOM_USE_INET6 */

    case IP_AF_LINK:
        {
            struct Ip_sockaddr_dl *dl = (struct Ip_sockaddr_dl *) addrptr;
            int str_index;
            int data_index;

            ipcom_memset(dl, 0, sizeof(struct Ip_sockaddr_dl));
            for (data_index = 0, str_index = 0; strptr[str_index] != '\0'; data_index++)
            {
                int b;
                if (ipcom_sscanf(&strptr[str_index], "%x", (unsigned*)&b) != 1)
                    break;

                dl->sdl_data[data_index] = (char) b;
                while (strptr[str_index++] != ':')
                    if (strptr[str_index] == '\0')
                        break;
            }

            dl->sdl_family = IP_AF_LINK;
            dl->sdl_alen   = (Ip_u8) data_index;
            IPCOM_SA_LEN_SET(dl,(ip_offsetof(struct Ip_sockaddr_dl, sdl_data)
                                 + IP_MAX(sizeof(dl->sdl_data), dl->sdl_alen)));
            return 1;
        }

    default:
        return ipcom_errno_set(IP_ERRNO_EAFNOSUPPORT);
    }
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
