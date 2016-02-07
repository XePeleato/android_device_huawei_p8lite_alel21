/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_arch.c,v $ $Revision: 1.3 $
 *     $Source: /home/interpeak/CVSRoot/ipcom/arch/src/ipcom_arch.c,v $
 *     $Author: kenneth $ $Date: 2004-09-23 06:34:43 $
 *     $State: Exp $ $Locker:  $
 *
 *     INTERPEAK_COPYRIGHT_STRING
 *     Design and implementation by Lennart Bang <lob@interpeak.se>
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

#if defined(IP_PORT_INTEGRITY) && !defined(IP_TARGET_PPC)

/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */


#include <string.h>

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

/*
 *===========================================================================
 *                    IPCOM_CPY_32_BITS_ALIGNED
 *===========================================================================
 * Description: Copies 32-bits of data where the source and destination
 *              addresses both are aligned on a 32-bit boundary.
 *
 */
#define IPCOM_CPY_32_BITS_ALIGNED \
    data = *dst_u32++ = *src_u32++; \
    chksum += (Ip_u16) data + (data >> 16); \


/*
 *===========================================================================
 *                    IPCOM_CPY_32_BITS_UNALIGNED
 *===========================================================================
 * Description: Copies 32-bits of data where the source and destination
 *              addresses cannot be aligned on a 32-bit boundary at the same
 *              time. Data is read and written in 32-bit chunks at a 32-bit
 *              boundary.
 *
 */
#ifdef IP_BIG_ENDIAN
#define IPCOM_CPY_32_BITS_UNALIGNED(shift1, shift2) \
    data = src_data << shift1; \
    src_data = *src_u32++; \
    data = (src_data >> shift2) | data; \
    chksum += (Ip_u16) data; \
    *dst_u32++ = data; \
    chksum += (data >> 16);
#else /* IP_LITTLE_ENDIAN */
#define IPCOM_CPY_32_BITS_UNALIGNED(shift1, shift2) \
    data = src_data >> shift1; \
    src_data = *src_u32++; \
    data = (src_data << shift2) | data; \
    chksum += (Ip_u16) data; \
    *dst_u32++ = data; \
    chksum += (data >> 16);
#endif


/*
 *===========================================================================
 *                    IPCOM_UNALIGNED_CPY_LOOP
 *===========================================================================
 * Description: Copies data as long as a full 32-bit word can be written.
 *
 */
#define IPCOM_UNALIGNED_CPY_LOOP(shift1, shift2) \
    i = (len - octet_unalignment) >> 5; \
    while (i--) \
    { \
        IPCOM_CPY_32_BITS_UNALIGNED(shift1, shift2); \
        IPCOM_CPY_32_BITS_UNALIGNED(shift1, shift2); \
        IPCOM_CPY_32_BITS_UNALIGNED(shift1, shift2); \
        IPCOM_CPY_32_BITS_UNALIGNED(shift1, shift2); \
        IPCOM_CPY_32_BITS_UNALIGNED(shift1, shift2); \
        IPCOM_CPY_32_BITS_UNALIGNED(shift1, shift2); \
        IPCOM_CPY_32_BITS_UNALIGNED(shift1, shift2); \
        IPCOM_CPY_32_BITS_UNALIGNED(shift1, shift2); \
    } \
    i = ((len - octet_unalignment) & 0x1f) >> 2; \
    while (i--) \
    { \
        IPCOM_CPY_32_BITS_UNALIGNED(shift1, shift2); \
    }


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

IP_PUBLIC void *
ipcom_memcpy_asm(void *s1, const void *s2, Ip_size_t n);

IP_PUBLIC Ip_u32
ipcom_in_checksum_update2_asm(void *pbuf, Ip_size_t bytes);

IP_PUBLIC Ip_u32
ipcom_in_checksum_memcpy_asm(void *dst, void *src, Ip_size_t len);


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
 *                    ipcom_memcpy_asm
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC void *
ipcom_memcpy_asm(void *s1, const void *s2, Ip_size_t n)
{
    return memcpy(s1, s2, n);
}


/*
 *===========================================================================
 *                    ipcom_in_checksum_update2_asm
 *===========================================================================
 * Description:   Computes the internet checksum (RFC 1071). NOTE
 *                it does not compute the 1 complement checksum.
 * Parameters:    pbuf and bytes.
 * Returns:       A standard internet checksum..
 *
 */
IP_PUBLIC Ip_u32
ipcom_in_checksum_update2_asm(void *pbuf, Ip_size_t bytes)
{
    Ip_u32    nwords;
    Ip_u16   *buf;
    Ip_u32    sum;

    buf = (Ip_u16 *) pbuf;
    sum = 0;

    for (nwords = (Ip_u32)bytes >> 5; nwords > 0; nwords--)
    {
        sum += *buf++; sum += *buf++; sum += *buf++; sum += *buf++;
        sum += *buf++; sum += *buf++; sum += *buf++; sum += *buf++;
        sum += *buf++; sum += *buf++; sum += *buf++; sum += *buf++;
        sum += *buf++; sum += *buf++; sum += *buf++; sum += *buf++;
    }
    for (nwords = (Ip_u32)(bytes & 0x1f) >> 1; nwords > 0; nwords--)
        sum += *buf++;

    /* Add left-over byte, if any. */
    if (bytes & 0x01)
        sum += *buf & ip_htons(0xff00);

    return sum;
}


/*
 *===========================================================================
 *                    ipcom_in_checksum_memcpy_asm
 *===========================================================================
 * Description: Copies data and calculate the internet checksum at the same
 *              time.
 * Parameters:  dst - Destination buffer.
 *              src - Source buffer.
 *              len - Number of octets to copy.
 * Returns:     The intermediate internet checksum, use
 *              ipcom_in_checksum_finish() to get the actual checksum.
 *
 */
IP_PUBLIC Ip_u32
ipcom_in_checksum_memcpy_asm(void *dst, void *src, Ip_size_t len)
{
#if defined(IPBOUNDS) || defined(__BOUNDS_CHECKING_ON)
    (void)ipcom_memcpy(dst, src, len);
    return ipcom_in_checksum_update2_asm(dst, len);
#else
    Ip_u8  *dst_u8;
    Ip_u8  *src_u8;
    Ip_u32 *dst_u32;
    Ip_u32 *src_u32;
    Ip_u32  data;
    Ip_u32  src_data;
    Ip_u32  chksum = 0;
    int     i;
    int     initial_octets;
    int     octet_unalignment = 0;
    Ip_bool dst_addr_is_odd = (int) dst & 0x1;

    initial_octets = IP_MIN(((-(int) dst) & 0x3), (int) len);
    if (initial_octets == 0)
    {
        dst_u32 = (Ip_u32 *) dst;
        src_u32 = (Ip_u32 *) src;
    }
    else
    {
        dst_u8 = (Ip_u8 *) dst;
        src_u8 = (Ip_u8 *) src;
        for (i = 0; i < initial_octets; ++i)
        {
            /* Copy initial octets (so that dst will be 32-bit aligned) */
            data = *dst_u8++ = *src_u8++;
#ifdef IP_BIG_ENDIAN
            if ((i ^ dst_addr_is_odd) & 0x1)
                chksum += data;
            else
                chksum += data << 8;
#else /* IP_LITTLE_ENDIAN */
            if ((i ^ dst_addr_is_odd) & 0x1)
                chksum += data << 8;
            else
                chksum += data;
#endif
        }
        dst_u32 = (Ip_u32 *) dst_u8;
        src_u32 = (Ip_u32 *) src_u8;
        len -= initial_octets;
    }
    /* dst is now 32-bit aligned */

    if (((int) dst_u32 & 0x3) == ((int) src_u32 & 0x3))
    {
        /* Both src and dst is now on an even 32-bit boundary */
        i = (len >> 5);
        while (i--)
        {
            /* Big main copy loop */
            IPCOM_CPY_32_BITS_ALIGNED;
            IPCOM_CPY_32_BITS_ALIGNED;
            IPCOM_CPY_32_BITS_ALIGNED;
            IPCOM_CPY_32_BITS_ALIGNED;
            IPCOM_CPY_32_BITS_ALIGNED;
            IPCOM_CPY_32_BITS_ALIGNED;
            IPCOM_CPY_32_BITS_ALIGNED;
            IPCOM_CPY_32_BITS_ALIGNED;
        }

        i = (len & 0x1f) >> 2;
        while (i--)
        {
            /* Small main copy loop */
            IPCOM_CPY_32_BITS_ALIGNED;
        }

        len -= len & ~0x3;
    }
    else if (len >= 4)
    {
        /* dst and src cannot be aligned */
        octet_unalignment = (int) src_u32 & 0x3;
        src_u32 = (Ip_u32 *) ((Ip_u8 *) src_u32 - octet_unalignment);
        src_data = *src_u32++;
        /* src is now 32-bit aligned */

        switch (octet_unalignment)
        {
        case 1:
            IPCOM_UNALIGNED_CPY_LOOP(8, 24);
            break;
        case 2:
            IPCOM_UNALIGNED_CPY_LOOP(16, 16);
            break;
        case 3:
            IPCOM_UNALIGNED_CPY_LOOP(24, 8);
            break;
        default:
            IP_PANIC();
            break;
        }
        --src_u32;
        len -= ((len - octet_unalignment) & ~0x3);
    }

    if (len)
    {
        /* Copy data that cannot be written in 32-bit chunks */
        src_u8 = (Ip_u8 *) src_u32 + octet_unalignment;
        dst_u8 = (Ip_u8 *) dst_u32;
        for (i = 0; i < (int) len; ++i)
        {
            data = *dst_u8++ = *src_u8++;
#ifdef IP_BIG_ENDIAN
            if (i & 0x1)
                chksum += data;
            else
                chksum += data << 8;
#else /* IP_LITTLE_ENDIAN */
            if (i & 0x1)
                chksum += data << 8;
            else
                chksum += data;
#endif
        }
    }

    if (dst_addr_is_odd)
    {
        /* The checksum has the low and hi bytes swaped */
        return (((Ip_u8 *) &chksum)[0] << 8)
            + ((Ip_u8 *) &chksum)[1]
            + (((Ip_u8 *) &chksum)[2] << 8)
            + ((Ip_u8 *) &chksum)[3];
    }

    return chksum;
#endif /* #if defined(IPBOUNDS) || defined(__BOUNDS_CHECKING_ON) */
}

#else
int ipcom_arch_empty_file;
#endif /* IP_PORT_INTEGRITY */


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
