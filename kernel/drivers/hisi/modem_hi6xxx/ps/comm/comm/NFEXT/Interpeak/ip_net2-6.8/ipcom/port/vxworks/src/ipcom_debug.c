/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_debug.c,v $ $Revision: 1.17 $
 *     $Source: /home/interpeak/CVSRoot/ipcom/port/vxworks/src/ipcom_debug.c,v $
 *     $Author: jonas $ $Date: 2005-03-02 08:56:33 $
 *     $State: Exp $ $Locker:  $
 *
 *     INTERPEAK_COPYRIGHT_STRING
 *        Design and implementation by Lennart Bang <lob@interpeak.se>
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

#include <ipcom_type.h>
#include <ipcom_cstyle.h>
#include <ipcom_clib.h>
#include <ipcom_clib2.h>

#include <vxWorks.h>
#include <taskLib.h>
#include <stdio.h>


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

/* Global and public variable to store assert info in. */
#ifdef IP_ASSERT
IP_PUBLIC Ip_assert assert_data;
#endif


/*
 ****************************************************************************
 * 9                    FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipcom_debug_printf
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipcom_debug_printf(const char *format, /* args*/ ...)
{
    char *buf;
    va_list   ap;
    Ip_s32 retval;

    buf = ipcom_malloc(IPCOM_STDIO_BUFSIZE);
    if(!buf)
    {
        printf("ipcom_debug_printf: out of memory printing: %s", format);
        return 0;
    }

    /* Here we would like to use vsnprintf, but vxWorks doesn't support that.
     * we cannot use ipcom_vsnprintf either, since that function uses the
     * ip_assert macro (may cause endless recursion). */
    va_start(ap, format);
    retval = vsprintf(buf, format, ap);
    va_end(ap);
    if(retval >= IPCOM_STDIO_BUFSIZE)
    {
        printf(buf);
        printf("ipcom_debug_printf: buffer overwrite!!! Task name: %s id: %x"IP_LF,
               taskName(taskIdSelf()), taskIdSelf());
        (void)taskSuspend(0);
    }

    retval = printf("%s", buf);
    ipcom_free(buf);
    return retval;
}



/*
 *===========================================================================
 *                    ipcom_assert
 *===========================================================================
 * Description:   Called if the assert macro failed due to a false assertion.
 *                The function stores the expression, line and file number in
 *                the a global Ip_assert structure 'ip_assert' then optionally
 *                prints the message and halts the program.
 * Parameters:    expr - The assert expression that is false.
 *                file - Pointer to the file where the assert triggered.
 *                line - File line number to identify the assert.
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_assert(const char *expr, const char *file, Ip_u32 line)
{
#ifdef IP_ASSERT
    ipcom_debug_printf("*** Failed assertion '%s' at %s:%ld, task name: %s, Id: %x ***"IP_LF,
                       expr, file, line, taskName(taskIdSelf()), taskIdSelf());

    /* Fill in the last assert into the global 'assert_data' structure. */
    assert_data.expr = expr;
    assert_data.file = file;
    assert_data.line = line;

    (void)taskSuspend(0);
#else
    (void)expr;
    (void)file;
    (void)line;
#endif /* IP_ASSERT */
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */


