/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_checksum.c,v $ $Revision: 1.27 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/port/src/ipcom_checksum.c,v $
 *   $Author: kenneth $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *     Design and implementation by Kenneth Jonsson <kenneth@interpeak.se>
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
#include "ipcom_inet.h"
#include "ipcom_sock.h"


/*
 ****************************************************************************
 * 4                    DEFINES
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
 * 5                    TYPES
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 6                    EXTERN PROTOTYPES
 ****************************************************************************
 */

#ifdef IPCOM_USE_ASM
IP_PUBLIC Ip_u32
ipcom_in_checksum_update2_asm(void *pbuf, Ip_size_t bytes);
IP_PUBLIC Ip_u32
ipcom_in_checksum_memcpy_asm(void *dst, void *src, Ip_size_t bytes);
#endif

#ifdef IP_PORT_LKM
IP_EXTERN Ip_u32
ipcom_in_checksum_update2(void *pbuf, Ip_size_t bytes);
#endif
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
 *                    ipcom_in_checksum_update
 *===========================================================================
 * Description:   Computes the internet checksum (RFC 1071). NOTE
 *                it does not compute the 1 complement checksum.
 * Parameters:    pbuf and bytes.
 * Returns:       A standard internet checksum..
 *
 */
IP_PUBLIC Ip_u32
ipcom_in_checksum_update(void *pbuf, Ip_size_t bytes)
{
    Ip_u32    sum;
#if defined(IP_PORT_LKM) && !defined(IP_TARGET_ARM) && !defined(IP_TARGET_MIPS)
    sum = ipcom_in_checksum_update2(pbuf, bytes);
    /* One folding step MUST be done to avoid overflow in IPNET/IPLITE */
    return (sum & 0xffff) + (sum >> 16);
#else
    Ip_u32    nwords;
    Ip_u16   *buf;
#ifdef IPCOM_USE_ASM
    if (IP_LIKELY(((long)pbuf & 0x1) == 0))
    {
        sum = ipcom_in_checksum_update2_asm(pbuf, bytes);
        /* One folding step MUST be done to avoid overflow in IPNET/IPLITE */
        return (sum & 0xffff) + (sum >> 16);
    }
#endif
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
#endif /* IP_PORT_LKM && !IP_TARGET_ARM*/
}


/*
 *===========================================================================
 *                    ipcom_in_checksum_finish
 *===========================================================================
 * Description:   Computes the one complement internet checksum (RFC 1071).
 * Parameters:    sum - The accumulated 32 bit internet sum.
 * Returns:       A one complement standard internet checksum.
 *
 */
IP_PUBLIC Ip_u16
ipcom_in_checksum_finish(Ip_u32 sum)
{
    /* Fold 32-bit sum to 16 bits. */
    while (sum >> 16)
        sum = (sum & 0xffff) + (sum >> 16);

    return (Ip_u16)~(Ip_u16)sum;
}


/*
 *===========================================================================
 *                    ipcom_in_checksum
 *===========================================================================
 * Description: Calculates the checksum on the IPv4 header.
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_u16
ipcom_in_checksum(void *buf, Ip_size_t bytes)
{
    Ip_u32   sum;

    sum = ipcom_in_checksum_update(buf, bytes);

    /* inline ipcom_in_checksum_finish code: */
    while (sum >> 16)
        sum = (sum & 0xffff) + (sum >> 16);

    return (Ip_u16)~(Ip_u16)sum;
}


/*
 *===========================================================================
 *                    ipcom_in_checksum_memcpy
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
#if !defined(IP_PORT_LKM) || (defined(IP_PORT_LKM) && (defined(IP_TARGET_ARM) || defined(IP_TARGET_MIPS)))
IP_PUBLIC Ip_u32
ipcom_in_checksum_memcpy(void *dst, void *src, Ip_size_t len)
{
#ifdef IPCOM_USE_ASM
    Ip_u32 sum = ipcom_in_checksum_memcpy_asm(dst, src, len);
    /* One folding step MUST be done to avoid overflow in IPNET/IPLITE */
    return (sum & 0xffff) + (sum >> 16);
#elif defined(IPBOUNDS) || defined(__BOUNDS_CHECKING_ON) || defined(IP_PORT_LKM)
    (void)ipcom_copy_from_user(dst, src, len);
    return ipcom_in_checksum_update(dst, len);
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
    Ip_bool dst_addr_is_odd = (Ip_ptrdiff_t) dst & 0x1;

    initial_octets = IP_MIN(((-(Ip_ptrdiff_t) dst) & 0x3), (int) len);
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

    if (((Ip_ptrdiff_t) dst_u32 & 0x3) == ((Ip_ptrdiff_t) src_u32 & 0x3))
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
        octet_unalignment = (Ip_ptrdiff_t) src_u32 & 0x3;
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
            + ((Ip_u8 *) &chksum)[3];  /*lint !e647 */
    }

    return chksum;
#endif /* IPCOM_USE_ASM */
}
#endif


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

