/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_ctype.c,v $ $Revision: 1.11 $
 *     $Source: /home/interpeak/CVSRoot/ipcom/port/src/ipcom_ctype.c,v $
 *     $Author: ulf $ $Date: 2008-10-13 11:09:39 $
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


/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#ifdef IP_PORT_WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#elif defined(IP_PORT_OSE) && defined(IP_TARGET_WIN32)
#include <windows.h>

#elif defined(IP_PORT_VXWORKS)
#include <vxWorks.h>
#include <ctype.h>

#else
#include <ctype.h>
#endif

#include "ipcom_clib.h"

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
 *                    ipcom_isspace
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
#if defined(IPCOM_ISSPACE) && IPCOM_ISSPACE == 1
IP_PUBLIC int
ipcom_isspace(int c)
{
  return isspace(c);
}
#endif


/*
 *===========================================================================
 *                    ipcom_isalpha
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
#if defined(IPCOM_ISALPHA) && IPCOM_ISALPHA == 1
IP_PUBLIC int
ipcom_isalpha(int c)
{
  return isalpha(c);
}
#endif


/*
 *===========================================================================
 *                    ipcom_isgraph
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
#if defined(IPCOM_ISGRAPH) && IPCOM_ISGRAPH == 1
IP_PUBLIC int
ipcom_isgraph(int c)
{
  return isgraph(c);
}
#endif


/*
 *===========================================================================
 *                    ipcom_isupper
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
#if defined(IPCOM_ISUPPER) && IPCOM_ISUPPER == 1
IP_PUBLIC int
ipcom_isupper(int c)
{
  return isupper(c);
}
#endif


/*
 *===========================================================================
 *                    ipcom_islower
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
#if defined(IPCOM_ISLOWER) && IPCOM_ISLOWER == 1
IP_PUBLIC int
ipcom_islower(int c)
{
  return islower(c);
}
#endif


/*
 *===========================================================================
 *                    ipcom_isdigit
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
#if defined(IPCOM_ISDIGIT) && IPCOM_ISDIGIT == 1
IP_PUBLIC int
ipcom_isdigit(int c)
{
  return isdigit(c);
}
#endif


/*
 *===========================================================================
 *                    ipcom_isprint
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
#if defined(IPCOM_ISPRINT) && IPCOM_ISPRINT == 1
IP_PUBLIC int
ipcom_isprint(int c)
{
  return isprint(c);
}
#endif


/*
 *===========================================================================
 *                    ipcom_isalnum
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
#if defined(IPCOM_ISALNUM) && IPCOM_ISALNUM == 1
IP_PUBLIC int
ipcom_isalnum(int c)
{
  return isalnum(c);
}
#endif


/*
 *===========================================================================
 *                    ipcom_iscntrl
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
#if defined(IPCOM_ISCNTRL) && IPCOM_ISCNTRL == 1
IP_PUBLIC int
ipcom_iscntrl(int c)
{
  return iscntrl(c);
}
#endif


/*
 *===========================================================================
 *                    ipcom_ispunct
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
#if defined(IPCOM_ISPUNCT) && IPCOM_ISPUNCT == 1
IP_PUBLIC int
ipcom_ispunct(int c)
{
  return ispunct(c);
}
#endif


/*
 *===========================================================================
 *                    ipcom_isxdigit
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
#if defined(IPCOM_ISXDIGIT) && IPCOM_ISXDIGIT == 1
IP_PUBLIC int
ipcom_isxdigit(int c)
{
  return isxdigit(c);
}
#endif


/*
 *===========================================================================
 *                    ipcom_isascii
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
#if defined(IPCOM_ISASCII) && IPCOM_ISASCII == 1
IP_PUBLIC int
ipcom_isascii(int c)
{
  return isascii(c);
}
#endif


/*
 *===========================================================================
 *                    ipcom_toupper
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
#if defined(IPCOM_TOUPPER) && IPCOM_TOUPPER == 1
IP_PUBLIC int
ipcom_toupper(int c)
{
  return toupper(c);
}
#endif


/*
 *===========================================================================
 *                    ipcom_tolower
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
#if defined(IPCOM_TOLOWER) && IPCOM_TOLOWER == 1
IP_PUBLIC int
ipcom_tolower(int c)
{
  return tolower(c);
}
#endif


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
