/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_strlib.c,v $ $Revision: 1.20 $
 *     $Source: /home/interpeak/CVSRoot/ipcom/src/ipcom_strlib.c,v $
 *     $Author: rime $ $Date: 2009-09-07 15:41:11 $
 *     $State: Exp $ $Locker:  $
 *
 *     INTERPEAK_COPYRIGHT_STRING
 *     Design and implementation by Roger Boden <roger@interpeak.se>
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
#include "ipcom_strlib.h"
#include "ipcom_clib.h"
#include "ipcom_err.h"
#include "ipcom_syslog.h"


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

#define IPCOM_SYSLOG_PRIORITY    IPCOM_SYSLOG_IPCOM_PRIORITY
#define IPCOM_SYSLOG_FACILITY    IPCOM_LOG_IPCOM


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
 *                    ipcom_parse_argstr
 *===========================================================================
 * Description: Parses a command string to the traditional argc and argv
 *              arguments.
 * Parameters:
 * Returns:
 */
/*lint -efunc(429,ipcom_parse_argstr)  Custodial pointer 'arg_str' (line X) has not been freed or returned*/
IP_PUBLIC Ip_err
ipcom_parse_argstr(char *arg_str, int *argc, char ***argv)
{
    char **tmp_argv;
    Ip_size_t argv_size = 32;
    int pos = 0, arg_str_len;
    Ip_err err_code;
    char endmark;
    enum { skipping_space, searching_end_of_arg } state;

    state = skipping_space;
    *argc = 0;
    arg_str_len = ipcom_strlen(arg_str);
    endmark = ' ';

    tmp_argv = ipcom_malloc(sizeof(char *) * argv_size);
    if(tmp_argv == IP_NULL)
    {
        err_code = IPCOM_ERR_FAILED;
        IPCOM_LOG0(ERR, "ipcom_parse_argstr :: out of memory 1");
        goto err;
    }
    ipcom_memset(tmp_argv, 0, sizeof(char *) * argv_size);

    while(pos < arg_str_len)
    {
        switch(state)
        {
        case skipping_space:
            if(arg_str[pos] == ' ' || arg_str[pos] == '\t')
                break;
            if(arg_str[pos] == '\'')
            {
                endmark = '\'';
                pos++;
            }
            else if(arg_str[pos] == '"')
            {
                endmark = '"';
                pos++;
            }
            else
                endmark = ' ';

            tmp_argv[*argc] = &(arg_str[pos]);
            (*argc)++;
            if(*argc == (int)argv_size)
            {
                /* Make space to fit in a terminating NULL pointer
                 * (and possible additional args) */
                tmp_argv = ipcom_realloc(tmp_argv, sizeof(char*)*argv_size*2);
                if( !tmp_argv )
                {
                    IPCOM_LOG0(ERR, "ipcom_parse_argstr :: out of memory 2");
                    err_code = IPCOM_ERR_FAILED;
                    goto err;
                }
                argv_size *=2;
            }
            state = searching_end_of_arg;
            break;

        case searching_end_of_arg:
            if(arg_str[pos] == endmark)
            {
                state = skipping_space;
                arg_str[pos] = 0;
            }
            break;
        }
        pos++;
    }

    if(state == searching_end_of_arg && (endmark == '\'' || endmark == '"'))
    {
        IPCOM_LOG1(ERR, "ipcom_parse_argstr() :: badly matched escape characters (%c)", endmark);
        err_code = IPCOM_ERR_INVALID_ARG;
        goto err;
    }
    tmp_argv[*argc] = IP_NULL;

    *argv = tmp_argv;
    return IPCOM_SUCCESS;

 err:
    if(tmp_argv != IP_NULL)
    {
        ipcom_free(tmp_argv);
    }
    *argc = 0;
    *argv = IP_NULL;
    return err_code;
}

/*
 *===========================================================================
 *                    ipcom_str_in_list
 *===========================================================================
 * Description: Search 'list' for 'str', where 'list' is a string of whitespace
 *              separated sub-strings.
 * Parameters:
 * Returns:     True (non-zero) if 'str' is part of 'list'
 */

IP_PUBLIC char *
ipcom_str_in_list(char *str, char *list)
{
    int n;
    char *s;

    for (s = ipcom_strstr(list, str); s; s = s + n, s = ipcom_strstr(s, str))
    {
        n = ipcom_strcspn(s, " \t");
        if (ipcom_strncmp(s, str, n) == 0)
        {
            return s;
        }
    }
    return IP_NULL;
}

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
