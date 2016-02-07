/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_fmemcpy.c,v $ $Revision: 1.11 $
 *     $Source: /home/interpeak/CVSRoot/ipcom/src/ipcom_fmemcpy.c,v $
 *     $Author: lob $
 *     $State: Exp $ $Locker:  $
 *
 *     INTERPEAK_COPYRIGHT_STRING
 *     fmemcpy() contributed by Fredrik Bredberg <frbr@enea.se>
 ******************************************************************************
 */

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 * ipcom_fmemcpy() copies tLength bytes from memory area pvSrc to pvDest.
 * It returns pvDest. ipcom_fmemcpy() is fully compatible with the ANSI-C89
 * memcpy() library function.
 *
 *
 */

/*
 ****************************************************************************
 * 2                    CONFIGURATION
 ****************************************************************************
 */

/* Define one of the below:
#define IPCOM_FAST_PRE_INC
#define IPCOM_FAST_POST_INC
#define IPCOM_FAST_ARRAY_INDEX
*/
#include "ipcom_config.h"

#if !defined(IPCOM_FAST_PRE_INC) && !defined(IPCOM_FAST_POST_INC) && !defined(IPCOM_FAST_ARRAY_INDEX)
#define IPCOM_FAST_ARRAY_INDEX
#endif


/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#define IPCOM_USE_CLIB_PROTO
#include "ipcom_type.h"
#include "ipcom_cstyle.h"


/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

#if !defined(IPCOM_FAST_PRE_INC) && !defined(IPCOM_FAST_POST_INC)
#if !defined(IPCOM_FAST_ARRAY_INDEX)
#if defined(__ppc)
#define IPCOM_FAST_PRE_INC
#elif defined(__m68k)
#define IPCOM_FAST_POST_INC
#ifndef IP_BIG_ENDIAN
#define IP_BIG_ENDIAN
#endif
#elif defined(__mips)
#define IPCOM_FAST_ARRAY_INDEX
#elif defined(__arm)
#define IPCOM_FAST_ARRAY_INDEX
#elif defined(__sparc)
#define IPCOM_FAST_ARRAY_INDEX
#ifndef IP_BIG_ENDIAN
#define IP_BIG_ENDIAN
#endif
#elif defined(__i386) || defined(_M_IX86)
#define IPCOM_FAST_ARRAY_INDEX
#ifndef IP_LITTLE_ENDIAN
#define IP_LITTLE_ENDIAN
#endif
#endif
#endif
#endif

#if defined(IP_BIG_ENDIAN)
#define SHL <<
#define SHR >>
#define BIG_LITTLE(x, y) (x)
#elif defined(IP_LITTLE_ENDIAN)
#define SHL >>
#define SHR <<
#define BIG_LITTLE(x, y) (y)
#else
#error Define either IP_BIG_ENDIAN or IP_LITTLE_ENDIAN
#endif

#if defined(IPCOM_FAST_PRE_INC)
#define PRE_LOOP_ADJUST_VALUE  -1
#define POST_LOOP_ADJUST_VALUE +1
#define CP_LONG(x)      (*++pulDest = *++pulSrc)
#define CP_LONG_INC(x)  (*++pulDest = *++pulSrc)
#define CP_CHAR(x)      (*++pucDest = *++pucSrc)
#define INC_VAL(x)      (*++x)
#define INC_INDEX(p, i) (void)0
#define CP_LONG_SH(i, l, r) { \
   ulTmp = ulSrc SHL (l);     \
   ulSrc = *++pulSrc;         \
   *++pulDest = (ulSrc SHR (r)) | ulTmp; }
#elif defined(IPCOM_FAST_POST_INC)
#define PRE_LOOP_ADJUST_VALUE  +0
#define POST_LOOP_ADJUST_VALUE +0
#define CP_LONG(x)      (*pulDest++ = *pulSrc++)
#define CP_LONG_INC(x)  (*pulDest++ = *pulSrc++)
#define CP_CHAR(x)      (*pucDest++ = *pucSrc++)
#define INC_VAL(x)      (*x++)
#define INC_INDEX(p, i) (void)0
#define CP_LONG_SH(i, l, r) { \
   ulTmp = ulSrc SHL (l);     \
   ulSrc = *pulSrc++;         \
   *pulDest++ = (ulSrc SHR (r)) | ulTmp; }
#elif defined(IPCOM_FAST_ARRAY_INDEX)
#define PRE_LOOP_ADJUST_VALUE  +0
#define POST_LOOP_ADJUST_VALUE +0
#define CP_LONG(x)      (pulDest[x] = pulSrc[x])
#define CP_LONG_INC(x)  (*pulDest++ = *pulSrc++)
#define CP_CHAR(x)      (*pucDest++ = *pucSrc++)
#define INC_VAL(x)      (*x++)
#define INC_INDEX(p, i) ((p) += (i))
#define CP_LONG_SH(i, l, r) { \
   ulTmp = ulSrc SHL (l);     \
   ulSrc = pulSrc[i];         \
   pulDest[i] = (ulSrc SHR (r)) | ulTmp; }
#else
#error Define either IPCOM_FAST_PRE_INC or IPCOM_FAST_POST_INC or IPCOM_FAST_ARRAY_INDEX
#endif

#define PRE_LOOP_ADJUST(x) ((x) PRE_LOOP_ADJUST_VALUE)
#define POST_LOOP_ADJUST(x) ((x) POST_LOOP_ADJUST_VALUE)


/*
 ****************************************************************************
 * 5                    PUBLIC FUNCTIONS
 ****************************************************************************
 */

IP_PUBLIC void *ipcom_fmemcpy(void *pvDest, const void *pvSrc, Ip_size_t tLength);


/*
 *===========================================================================
 *                    ipcom_fmemcpy
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC void *
ipcom_fmemcpy(void *pvDest, const void *pvSrc, Ip_size_t tLength)
{
    unsigned char *pucDest = (unsigned char *)pvDest;
    unsigned char *pucSrc  = (unsigned char *)pvSrc;
    unsigned long  ulTmp;

    if (tLength < 8)
    {
        if (tLength >= 4 &&
            ((((unsigned long)pucSrc | (unsigned long)pucDest) & 3) == 0))
        {
            *(unsigned long *)pucDest = *(unsigned long *)pucSrc;
            pucSrc  += PRE_LOOP_ADJUST(4);
            pucDest += PRE_LOOP_ADJUST(4);
            tLength -= 4;
            while (tLength--)
            {
                CP_CHAR(0);
            }
            return pvDest;
        }
        while (tLength--)
        {
            pucDest[tLength] = pucSrc[tLength];
        }
        return pvDest;
    }
    while ((Ip_ptrdiff_t)pucDest & 3)
    {
        *pucDest++ = *pucSrc++;
        tLength--;
    }
    ulTmp = (Ip_ptrdiff_t)pucSrc & 3;
    if (ulTmp == 0)
    {
        unsigned long *pulSrc  = PRE_LOOP_ADJUST((unsigned long *)pucSrc);
        unsigned long *pulDest = PRE_LOOP_ADJUST((unsigned long *)pucDest);
        Ip_size_t       tLen    = (tLength / 4) & 7;

        while (tLen--)
        {
            CP_LONG_INC(0);
        }
        tLen = tLength / 32;
        while (tLen--)
        {
            CP_LONG(7); CP_LONG(6);
            CP_LONG(5); CP_LONG(4);
            CP_LONG(3); CP_LONG(2);
            CP_LONG(1); CP_LONG(0);
            INC_INDEX(pulDest, 8);
            INC_INDEX(pulSrc, 8);
        }
        if (tLength & 2)
        {
            *(unsigned short *)POST_LOOP_ADJUST(pulDest) =
                *(unsigned short *)POST_LOOP_ADJUST(pulSrc);
            if ((tLength & 1) == 0)
                return pvDest;
            ((unsigned char *)POST_LOOP_ADJUST(pulDest))[2] =
                ((unsigned char *)POST_LOOP_ADJUST(pulSrc))[2];
            return pvDest;
        }
        if ((tLength & 1) == 0)
            return pvDest;
        *(unsigned char *)POST_LOOP_ADJUST(pulDest) =
            *(unsigned char *)POST_LOOP_ADJUST(pulSrc);
        return pvDest;
    }
    else
    {
        unsigned long *pulSrc;
        unsigned long *pulDest;
        Ip_size_t       tLen;
        unsigned long  ulSrc;

        pulDest = PRE_LOOP_ADJUST((unsigned long *)pucDest);
        tLen = (tLength / 4) & 7;
        if (ulTmp == 2)
        {
            ulSrc  = (*(unsigned short *)pucSrc) << BIG_LITTLE(0, 16);  /*lint !e647 */
            pulSrc = PRE_LOOP_ADJUST((unsigned long *)(pucSrc + 2));
            while (tLen--)
            {
                CP_LONG_SH(0, 16, 16);
                INC_INDEX(pulDest, 1);
                INC_INDEX(pulSrc, 1);
            }
            tLen = tLength / 32;
            while (tLen--)
            {
                CP_LONG_SH(0, 16, 16); CP_LONG_SH(1, 16, 16);
                CP_LONG_SH(2, 16, 16); CP_LONG_SH(3, 16, 16);
                CP_LONG_SH(4, 16, 16); CP_LONG_SH(5, 16, 16);
                CP_LONG_SH(6, 16, 16); CP_LONG_SH(7, 16, 16);
                INC_INDEX(pulDest, 8);
                INC_INDEX(pulSrc, 8);
            }
            if (tLength & 2)
            {
                *(unsigned short *)POST_LOOP_ADJUST(pulDest) =
                    (unsigned short)BIG_LITTLE(ulSrc, ulSrc >> 16);
                if ((tLength & 1) == 0)
                    return pvDest;
                ((unsigned char *)POST_LOOP_ADJUST(pulDest))[2] =
                    ((unsigned char *)POST_LOOP_ADJUST(pulSrc))[0];
                return pvDest;
            }
            if ((tLength & 1) == 0)
                return pvDest;
            *(unsigned char *)POST_LOOP_ADJUST(pulDest) =
                (unsigned char)BIG_LITTLE(ulSrc >> 8, ulSrc >> 16);
            return pvDest;
        }
        if (ulTmp == 1)
        {
            ulSrc  = (*pucSrc << BIG_LITTLE(16, 8)) |
                (*(unsigned short *)(pucSrc + 1) << BIG_LITTLE(0, 16));  /*lint !e647 */
            pulSrc = PRE_LOOP_ADJUST((unsigned long *)(pucSrc + 3));
            while (tLen--)
            {
                CP_LONG_SH(0, 8, 24);
                INC_INDEX(pulDest, 1);
                INC_INDEX(pulSrc, 1);
            }
            tLen = tLength / 32;
            while (tLen--)
            {
                CP_LONG_SH(0, 8, 24); CP_LONG_SH(1, 8, 24);
                CP_LONG_SH(2, 8, 24); CP_LONG_SH(3, 8, 24);
                CP_LONG_SH(4, 8, 24); CP_LONG_SH(5, 8, 24);
                CP_LONG_SH(6, 8, 24); CP_LONG_SH(7, 8, 24);
                INC_INDEX(pulDest, 8);
                INC_INDEX(pulSrc, 8);
            }
            if (tLength & 2)
            {
                *(unsigned short *)POST_LOOP_ADJUST(pulDest) =
                    (unsigned short)(ulSrc >> 8);
                if ((tLength & 1) == 0)
                    return pvDest;
                ((unsigned char *)POST_LOOP_ADJUST(pulDest))[2] =
                    (unsigned char)BIG_LITTLE(ulSrc, ulSrc >> 24);
                return pvDest;
            }
            if ((tLength & 1) == 0)
                return pvDest;
            *(unsigned char *)POST_LOOP_ADJUST(pulDest) =
                (unsigned char)BIG_LITTLE(ulSrc >> 16, ulSrc >> 8);
            return pvDest;
        }
        /* ((Ip_ptrdiff_t)pucSrc & 3) == 3 */
        ulSrc  = *pucSrc << BIG_LITTLE(0, 24);  /*lint !e647 */
        pulSrc = PRE_LOOP_ADJUST((unsigned long *)(pucSrc + 1));
        while (tLen--)
        {
            CP_LONG_SH(0, 24, 8);
            INC_INDEX(pulDest, 1);
            INC_INDEX(pulSrc, 1);
        }
        tLen = tLength / 32;
        while (tLen--)
        {
            CP_LONG_SH(0, 24, 8); CP_LONG_SH(1, 24, 8);
            CP_LONG_SH(2, 24, 8); CP_LONG_SH(3, 24, 8);
            CP_LONG_SH(4, 24, 8); CP_LONG_SH(5, 24, 8);
            CP_LONG_SH(6, 24, 8); CP_LONG_SH(7, 24, 8);
            INC_INDEX(pulDest, 8);
            INC_INDEX(pulSrc, 8);
        }
        if (tLength & 2)
        {
            *(unsigned short *)POST_LOOP_ADJUST(pulDest) =
                (unsigned short)
                BIG_LITTLE(ulSrc << 8 |
                           ((unsigned char *)POST_LOOP_ADJUST(pulSrc))[0],
                           ulSrc >> 24 |
                           ((unsigned char *)POST_LOOP_ADJUST(pulSrc))[0] << 8);  /*lint !e647 */
            if ((tLength & 1) == 0)
                return pvDest;
            ((unsigned char *)POST_LOOP_ADJUST(pulDest))[2] =
                ((unsigned char *)POST_LOOP_ADJUST(pulSrc))[1];
            return pvDest;
        }
        if ((tLength & 1) == 0)
            return pvDest;
        *(unsigned char *)POST_LOOP_ADJUST(pulDest) =
            (unsigned char)BIG_LITTLE(ulSrc, ulSrc >> 24);
        return pvDest;
    }
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

