/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_strto.c,v $ $Revision: 1.3 $
 *     $Source: /home/interpeak/CVSRoot/ipcom/port/src/ipcom_strto.c,v $
 *     $Author: ulf $ $Date: 2008-10-13 15:05:23 $
 *     $State: Exp $ $Locker:  $
 *
 *     INTERPEAK_COPYRIGHT_STRING
 *
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


/*
 ****************************************************************************
 * 4                    EXTERN PROTOTYPES
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 5                    DEFINES
 ****************************************************************************
 */

#define IPCOM_LONG_MAX        	((long)(((unsigned long) -1L) >> 1))
#define IPCOM_LONG_MIN		(~IPCOM_LONG_MAX)
#define IPCOM_ULONG_MAX		((unsigned long)~0UL)


/*
 ****************************************************************************
 * 6                    TYPES
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
 * 9                    LOCAL FUNCTIONS
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 10                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipcom_strtol
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
#if defined(IPCOM_STRTOL) && IPCOM_STRTOL == 1
IP_PUBLIC long
ipcom_strtol(const char *nptr, char **endptr, int base)
{
    const char   *s = nptr;
    unsigned long cutoff;
    unsigned long acc;
    int           neg = 0;
    int           cutlim;
    int           any;
    int           c;

    /*
     * Skip white space and pick up leading +/- sign if any.
     * If base is 0, allow 0x for hex and 0 for octal, else
     * assume decimal; if base is already 16, allow 0x.
     */
    do
    {
        c = *s++;
    }
    while (ipcom_isspace (c));

    if (c == '-')
    {
        neg = 1;
        c = *s++;
    }
    else if (c == '+')
    {
        c = *s++;
    }
    
    if (((base == 0) || (base == 16)) &&
        (c == '0') &&
        ((*s == 'x') || (*s == 'X')))
    {
        c = s[1];
        s += 2;
        base = 16;
    }

    if (base == 0)
    {
        base = (c == '0' ? 8 : 10);
    }
    
    /*
     * Compute the cutoff value between legal numbers and illegal
     * numbers.  That is the largest legal value, divided by the
     * base.  An input number that is greater than this value, if
     * followed by a legal input character, is too big.  One that
     * is equal to this value may be valid or not; the limit
     * between valid and invalid numbers is then based on the last
     * digit.  For instance, if the range for longs is
     * [-2147483648..2147483647] and the input base is 10,
     * cutoff will be set to 214748364 and cutlim to either
     * 7 (neg==0) or 8 (neg==1), meaning that if we have accumulated
     * a value > 214748364, or equal but the next digit is > 7 (or 8),
     * the number is too big, and we will return a range error.
     *
     * Set any if any `digits' consumed; make it negative to indicate
     * overflow.
     */

    cutoff = (neg ? -(unsigned long)IPCOM_LONG_MIN : IPCOM_LONG_MAX);
    cutlim = cutoff % (unsigned long)base;
    cutoff /= (unsigned long)base;

    for (acc = 0, any = 0;; c = *s++)
    {
        if (ipcom_isdigit (c))
        {
            c -= '0';
        }
        else if (ipcom_isalpha (c))
        {
            c -= (ipcom_isupper(c) ? 'A' - 10 : 'a' - 10);
        }
        else
        {
            break;
        }
        
        if (c >= base)
        {
            break;
        }
        
        if ((any < 0) || (acc > cutoff) || ((acc == cutoff) && (c > cutlim)))
        {
            any = -1;
        }
        else
        {
            any = 1;
            acc *= base;
            acc += c;
        }
    }

    if (any < 0)
    {
        acc = (neg ? IPCOM_LONG_MIN : IPCOM_LONG_MAX);
    }
    else if (neg)
    {
        acc = -acc;
    }
    
    if (endptr != 0)
    {
        *endptr = (char *)(any ? (s - 1) : nptr);
    }
    
    return acc;
}
#endif


/*
 *===========================================================================
 *                    ipcom_strtoul
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
#if defined(IPCOM_STRTOUL) && IPCOM_STRTOUL == 1
IP_PUBLIC unsigned long
ipcom_strtoul(const char *nptr, char **endptr, int base)
{
    const char   *s = nptr;
    unsigned long cutoff;
    unsigned long acc;
    int           neg = 0;
    int           cutlim;
    int           any;
    int           c;

    /*
     * See strtol for comments as to the logic used.
     */
    do
    {
        c = *s++;
    }
    while (ipcom_isspace (c));

    if (c == '-')
    {
        neg = 1;
        c = *s++;
    }
    else if (c == '+')
    {
        c = *s++;
    }
    
    if (((base == 0) || (base == 16)) &&
        (c == '0') &&
        ((*s == 'x') || (*s == 'X')))
    {
        c = s[1];
        s += 2;
        base = 16;
    }

    if (base == 0)
    {
        base = (c == '0' ? 8 : 10);
    }
    
    cutoff = (unsigned long)IPCOM_ULONG_MAX / (unsigned long)base;
    cutlim = (unsigned long)IPCOM_ULONG_MAX % (unsigned long)base;

    for (acc = 0, any = 0;; c = *s++)
    {
        if (ipcom_isdigit (c))
        {
            c -= '0';
        }
        else if (ipcom_isalpha (c))
        {
            c -= (ipcom_isupper (c) ? 'A' - 10 : 'a' - 10);
        }
        else
        {
            break;
        }
        
        if (c >= base)
        {
            break;
        }
        
        if ((any < 0) || (acc > cutoff) || ((acc == cutoff) && (c > cutlim)))
        {
            any = -1;
        }
        else
        {
            any = 1;
            acc *= base;
            acc += c;
        }
    }

    if (any < 0)
    {
        acc = IPCOM_ULONG_MAX;
    }
    else if (neg)
    {
        acc = -acc;
    }
    
    if (endptr != 0)
    {
        *endptr = (char *)(any ? (s - 1) : nptr);
    }
    
    return acc;
}
#endif


/*
 *===========================================================================
 *                    ipcom_atoi
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
#if defined(IPCOM_ATOI) && IPCOM_ATOI == 1
IP_PUBLIC int
ipcom_atoi(const char *s)
{
    return (int)ipcom_strtol (s, IP_NULL, 10);
}
#endif


/*
 *===========================================================================
 *                    ipcom_atol
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
#if defined(IPCOM_ATOL) && IPCOM_ATOL == 1
IP_PUBLIC long
ipcom_atol(const char *s)
{
    return ipcom_strtol (s, IP_NULL, 10);
}
#endif


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
