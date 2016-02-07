/*
 ******************************************************************************
 *                     INTERPEAK HEADER FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_align16.h,v $ $Revision: 1.7 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/include/ipcom_align16.h,v $
 *   $Author: uolofsso $ $Date: 2009-01-27 10:07:07 $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *
 ******************************************************************************
 */

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 * Instruct compiler to assume 16-bit alignment for the struct enclosed in
 * two ipcom_align16.h includes, and also to avoid padding. Example:
 *
 *      #include "ipcom_align16.h"
 *      typedef IP_PACKED1 struct mystruct
 *      {
 *          ...
 *      }
 *      IP_PACKED2 mystruct;
 *      #include "ipcom_align16.h"
 */

/*
 ****************************************************************************
 * 2                    DEFINES
 ****************************************************************************
 */

#if defined(_lint)
 #define IP_PACKED1
 #define IP_PACKED2

#elif defined (__GNUC__) || defined (__GCC32__)

#ifndef IP_PACKED1
 #define IP_PACKED1
 #define IP_PACKED2 __attribute__((packed, aligned(2)))
#else
 #undef IP_PACKED1
 #undef IP_PACKED2
#endif

#elif defined (__ghs__)

#ifndef IP_PACKED1
 #define IP_PACKED1
 #define IP_PACKED2
 #pragma pack(push,2)
 #pragma ghs nowarning 1545
#else
 #undef IP_PACKED1
 #undef IP_PACKED2
 #pragma pack(pop)
#endif

#elif defined (__CW32__)

#ifndef IP_PACKED1
 #define IP_PACKED1
 #define IP_PACKED2
 #pragma pack(push,2)
#else
 #undef IP_PACKED1
 #undef IP_PACKED2
 #pragma pack(pop)
#endif

#elif defined (__DCC__)

#ifndef IP_PACKED1
 #define IP_PACKED1
 #define IP_PACKED2
 #pragma pack(2)
#else
 #undef IP_PACKED1
 #undef IP_PACKED2
 #pragma pack(0,0,0)
#endif

#elif defined (IP_COMPILER_SHC)

#ifndef IP_PACKED1
 #define IP_PACKED1
 #define IP_PACKED2
 #pragma pack(2)
#else
 #undef IP_PACKED1
 #undef IP_PACKED2
 #pragma unpack()
#endif

#elif defined (_MSC_VER)

#ifndef IP_PACKED1
 #define IP_PACKED1
 #define IP_PACKED2
 #pragma warning(disable:4103)
 #pragma pack(push,CGX_PACK,2)
#else
 #undef IP_PACKED1
 #undef IP_PACKED2
 #pragma pack(pop,CGX_PACK)
#endif

#elif defined (__ARMCC_VERSION) /* WIND00139353 */

#ifndef IP_PACKED1
 #define IP_PACKED1
 #define IP_PACKED2
 #pragma pack(2)
#else
 #undef IP_PACKED1
 #undef IP_PACKED2
 #pragma pack(8)
#endif

#else

#error "COMPILER TYPE NOT AVAILABLE - MUST BE SPECIFIED"

#endif


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
