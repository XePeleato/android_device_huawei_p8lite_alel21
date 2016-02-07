/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_getput.c,v $ $Revision: 1.17 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/port/src/ipcom_getput.c,v $
 *   $Author: ulf $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by FirstName LastName <email@interpeak.se>
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

#if IPCOM_USE_SHELL == IPCOM_SHELL_NATIVE
#include <stdio.h>
#include <string.h>
#else
#define IPCOM_USE_CLIB_PROTO
#endif

#include "ipcom_clib.h"
#include "ipcom_clib2.h"
#include "ipcom_file.h"
#include "ipcom_shell.h"


/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

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
 *                    ipcom_fgetc
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
#if defined(IPCOM_FGETC) && IPCOM_FGETC == 1
IP_PUBLIC int
ipcom_fgetc(IP_FILE *stream)
{
#if IPCOM_USE_FILE != IPCOM_FILE_NONE
    char  character;

    if (ipcom_fread(&character, 1, 1, stream) == 1)
        return (int)character;
#elif IPCOM_USE_SHELL != IPCOM_SHELL_NONE
    char  character;

    if (stream == ip_stdin && ipcom_shell_fread(&character, 1, 1, stream) == 1)
        return (int)character;
#endif

    return IP_EOF;
}
#endif


/*
 *===========================================================================
 *                    ipcom_getc
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
#if defined(IPCOM_GETC) && IPCOM_GETC == 1
IP_PUBLIC int
ipcom_getc(IP_FILE *stream)
{
#if defined(IPCOM_FGETC) && IPCOM_FGETC == 1
    return ipcom_fgetc(stream);
#elif IPCOM_USE_SHELL == IPCOM_SHELL_NATIVE
    return ipcom_fgetc((FILE *)stream);
#else
    return ipcom_fgetc(stream);
#endif
}
#endif


/*
 *===========================================================================
 *                    ipcom_getchar
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
#if defined(IPCOM_GETCHAR) && IPCOM_GETCHAR == 1
IP_PUBLIC int
ipcom_getchar(void)
{
#if IPCOM_USE_FILE == IPCOM_FILE_IPCOM
    #if IPCOM_USE_SHELL != IPCOM_SHELL_NONE
        return ipcom_fgetc(ip_stdin);
    #else
        return IP_EOF;
    #endif
#elif IPCOM_USE_SHELL == IPCOM_SHELL_NATIVE
    return ipcom_fgetc(stdin);
#elif IPCOM_USE_SHELL != IPCOM_SHELL_NONE
    return ipcom_fgetc(ip_stdin);
#else
    return IP_EOF;
#endif
}
#endif


/*
 *===========================================================================
 *                    ipcom_fgets
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
#if defined(IPCOM_FGETS) && IPCOM_FGETS == 1
IP_PUBLIC char *
ipcom_fgets(char *buffer, int bufsize, IP_FILE *stream)
{
    char  c[1];
    char *cs;
    Ip_size_t  len;

    ip_assert(stream != IP_NULL);

    for (cs = buffer; bufsize > 1; cs++, bufsize--)
    {
#if IPCOM_USE_FILE != IPCOM_FILE_NONE
        len = ipcom_fread(c, 1, 1, stream);
#elif IPCOM_USE_SHELL != IPCOM_SHELL_NONE
        len = ipcom_shell_fread(c, 1, 1, stream);
#else
        len = 0;
#endif
        if (len != 1)
        {
            if (cs == buffer)
                return IP_NULL;  /* read-error or eof and no characters. */
            break;
        }

        if (*c == '\n')
        {
            *cs++ = '\n';
            break;
        }
        else if (*c == '\0')
            break;
        else
            *cs = *c;
    }

    ip_assert(bufsize > 0);
    *cs = '\0';
    return buffer;
}
#endif


/*
 *===========================================================================
 *                    ipcom_fputc
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:    Character written or IP_EOF on error.
 *
 */
#if defined(IPCOM_FPUTC) && IPCOM_FPUTC == 1
IP_PUBLIC int
ipcom_fputc(int c, IP_FILE *stream)
{
#if IPCOM_USE_FILE != IPCOM_FILE_NONE
    char   character = (char)c;

    if (ipcom_fwrite(&character, 1, 1, stream) == 1)
        return c;
#elif IPCOM_USE_SHELL != IPCOM_SHELL_NONE
    char   character = (char)c;

    if (ipcom_shell_fwrite(&character, 1, 1, stream) == 1)
        return c;
#endif

    return IP_EOF;
}
#endif


/*
 *===========================================================================
 *                    ipcom_putc
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
#if defined(IPCOM_PUTC) && IPCOM_PUTC == 1
IP_PUBLIC int
ipcom_putc(int c, IP_FILE *stream)
{
#if defined(IPCOM_FPUTC) && IPCOM_FPUTC == 1
    return ipcom_fputc(c, stream);
#elif IPCOM_USE_SHELL == IPCOM_SHELL_NATIVE
    return ipcom_fputc(c, (FILE *)stream);
#else
    return ipcom_fputc(c, stream);
#endif
}
#endif


/*
 *===========================================================================
 *                    ipcom_putchar
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
#if defined(IPCOM_PUTCHAR) && IPCOM_PUTCHAR == 1
IP_PUBLIC int
ipcom_putchar(int c)
{
#if IPCOM_USE_FILE == IPCOM_FILE_IPCOM
    #if IPCOM_USE_SHELL != IPCOM_SHELL_NONE
        return ipcom_fputc(c, ip_stdout);
    #else
        (void)c;
        return IP_EOF;
    #endif
#elif IPCOM_USE_SHELL == IPCOM_SHELL_NATIVE
    return ipcom_fputc(c, stdout);
#elif IPCOM_USE_SHELL != IPCOM_SHELL_NONE
    return ipcom_fputc(c, ip_stdout);
#else
    (void)c;
    return IP_EOF;
#endif
}
#endif


/*
 *===========================================================================
 *                    ipcom_fputs
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
#if defined(IPCOM_FPUTS) && IPCOM_FPUTS == 1
IP_PUBLIC int
ipcom_fputs(const char *str, IP_FILE *stream)
{
    Ip_size_t  length;

    length = (Ip_size_t)ipcom_strlen(str);
    (void)length;

#if IPCOM_USE_FILE != IPCOM_FILE_NONE
    if (ipcom_fwrite(str, length, 1, stream) == length)
        return (int)length;
#elif IPCOM_USE_SHELL != IPCOM_SHELL_NONE
    if (ipcom_shell_fwrite((IP_CONST void *)str, length, 1, stream) == length)
        return (int)length;
#endif

    return IP_EOF;
}
#endif


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

