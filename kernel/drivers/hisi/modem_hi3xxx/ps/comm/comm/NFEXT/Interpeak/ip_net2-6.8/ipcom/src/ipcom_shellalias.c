
/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_shellalias.c,v $ $Revision: 1.3 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/src/ipcom_shellalias.c,v $
 *   $Author: lob $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *     Design and implementation by Joel Hortelius <joel.hortelius@interpeak.se>
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
#define IPCOM_SHELL_C

#if defined(IPCOM_USE_ALIAS) && IPCOM_USE_SHELL == IPCOM_SHELL_IPCOM

/* Ipcom include files */
#define IPCOM_USE_CLIB_PROTO
#include "ipcom_clib.h"
#include "ipcom_err.h"
#include "ipcom_os.h"
#include "ipcom_syslog.h"
#include "ipcom_sock.h"
#include "ipcom_strlib.h"
#include "ipcom_shell.h"
#include "ipcom_sysvar.h"


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
struct Ipcom_shellalias_alias
{
    Ipcom_list      next;

    char            *name;      /* Name of this alias   */
    char            *cmd;       /* Command string       */

    int             argc;       /* Number of arguments  */
    char            **argv;     /* Stored arguments     */

    Ip_bool         locked;     /* Parsed alias         */
};

struct Ipcom_shellalias_config
{
    char    *name;
    char    *cmd;
};


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
IP_STATIC int ipcom_shellalias_set(char *name, char *cmd);


/*
 ****************************************************************************
 * 8                    DATA
 ****************************************************************************
 */
static Ipcom_list    ipcom_shellalias_head;

/* Alias expansion is done:
 * If the first word of the command is an alias, it will be expanded, unless
 * that particular alias has already been used in this expansion. Recursive
 * expansion of aliases do not allows that the same alias is referenced twice.
 *
 * If no substitution is involved, the arguments passed in the command will
 * be appended to the expanded alias'ed command. If substitution is involved,
 * the arguments will be inserted where the substitution specifies, and
 * not anywhere else.
 *
 * If a command is prepended with \, the backslash will be removed by the
 * the alias expansion mechanism and then the expansion will terminate, allowing
 * the command to pass through without any expansion.
 *
 * Subsitution understands the following directives:
 *  !^      - First argument [Argument 1].
 *  !$      - Last argument [Argument 'argc - 1']
 *  !-y     - Argument range 0 - y
 *  !x-     - Argument range x - 'argc - 2'. Note that !$ is not included.
 *  !x-y    - Argument range x -y .
 *  !-      - Argument range 0 - 'argc - 2'. Note that !$ is not included.
 *  !*      - Same as !^-$ with the exception that if it does not match any
 *            arguments, it will be silently discarded whereas the other
 *            substitutions require a match.
 *  !n      - Argument n
 *  !n*     - Same as !n-$ with the exception that if no arguments match the
 *            the substitution it will be silenty discarded.
 */

/* Pre defined aliases */
static struct Ipcom_shellalias_config ipcom_shellalias_config[] =
{
#ifdef IPCOM_SHELLALIAS_EXAMPLE
    /* First argument expands !1, if there exists any more arguments, they will
     * be expanded in !2*, otherwise it'' be empty */
    { "qos6",   "qos -6 !1 policy !2*"          },
    /* Invoke alias qos6 with the appropriate command */
    { "ql",     "qos6 list"                     },
    { "qg",     "qos6 get"                      },
    /* Short for route */
    { "r",      "route"                         },
    /* Show a specific routing table */
    { "rt",     "r show -T"                     },
    { "ftp6",   "ftp 3ffe:302:111:3::10"        },
    /* Ping a specific host, prepend address with any arguments */
    { "p6",     "ping6 !* 3ffe:302:111:1::3"    },
#endif /* IPCOM_SHELLALIAS_EXAMPLE */
    { IP_NULL,  IP_NULL                         }
};

/*
 ****************************************************************************
 * 9                    STATIC FUNCTIONS
 ****************************************************************************
 */


/*
 *===========================================================================
 *                    ipcom_shellalias_init
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err
ipcom_shellalias_init(void *unused)
{
    int i;
    (void)unused;

    ipcom_list_init(&ipcom_shellalias_head);

    for (i = 0; ipcom_shellalias_config[i].name != IP_NULL; i++)
        ipcom_shellalias_set(ipcom_shellalias_config[i].name, ipcom_shellalias_config[i].cmd);

    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    ipcom_shellalias_setup
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err
ipcom_shellalias_setup()
{
    Ip_err                          err;
    static Ipcom_once_t             once = IPCOM_ONCE_INIT;

    err = ipcom_once(&once, ipcom_shellalias_init, IP_NULL);
    if (err != IPCOM_SUCCESS)
        return err;

    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    ipcom_shellalias_find
 *===========================================================================
 * Description: Find a given alias based on name.
 * Parameters:  name        - The alias name.
 * Returns:     The alias if found.
 *
 */
IP_STATIC struct Ipcom_shellalias_alias *
ipcom_shellalias_find(const char *name)
{
    struct Ipcom_shellalias_alias   *alias;

    /* Go through the aliases */
    for (alias = IPCOM_LIST_FIRST(&ipcom_shellalias_head);
         alias != IP_NULL;
         alias = IPCOM_LIST_NEXT(&alias->next))
    {
        if (name == IP_NULL)
        {
            int i;
            ipcom_printf("%-15s\t'", alias->name);
            for (i = 0; i < alias->argc; i++)
            {
                ipcom_printf("%s%s",
                             alias->argv[i],
                             (i == alias->argc - 1)? "" : " ");
            }
            ipcom_printf("'"IP_LF);
        }
        /* Did it match? */
        else if (ipcom_strcmp(alias->name, name) == 0)
        {
            /* Return it */
            return alias;
        }
    }

    /* No go */
    return IP_NULL;
}



/*
 *===========================================================================
 *                    ipcom_shellalias_remove
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipcom_shellalias_remove(const char *name)
{
    struct Ipcom_shellalias_alias *alias = ipcom_shellalias_find(name);

    /* We did locate it */
    if (alias != IP_NULL)
    {
        /* Remove it from the list */
        ipcom_list_remove(&alias->next);

        /* Free any allocated name */
        ipcom_free(alias->name);

        /* Free the command */
        ipcom_free(alias->cmd);

        /* Kill the vector */
        ipcom_free(alias->argv);

        /* Free the structure */
        ipcom_free(alias);

        /* Found it */
        return IPCOM_SUCCESS;
    }

    /* No such entry */
    return -IP_ERRNO_ENOENT;
}


/*
 *===========================================================================
 *                    ipcom_shellalias_remove
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipcom_shellalias_set(char   *name,
                     char   *cmd)
{
    struct Ipcom_shellalias_alias *alias;

    if (ipcom_strcmp(name, "alias") == 0 ||
        ipcom_strcmp(name, "unalias") == 0)
        return -IPCOM_ERR_INVALID_ARG;

    /* Remove any old references */
    ipcom_shellalias_remove(name);

    /* Allocate a new alias structure */
    alias = ipcom_calloc(1, sizeof(*alias));

    /* Could we allocate? */
    if (alias != IP_NULL)
    {
        Ip_err  retval;
        int     argc;
        char    **argv = IP_NULL;

        /* Allocate the name */
        alias->name = ipcom_strdup(name);
        if (alias->name == IP_NULL)
            goto fail;

        /* Allocate the command */
        alias->cmd  = ipcom_strdup(cmd);
        if (alias->cmd == IP_NULL)
            goto fail;

        /* Parse the string */
        retval = ipcom_parse_argstr(alias->cmd, (int *)&argc, &argv);
        switch(retval)
        {
        case IPCOM_SUCCESS:
            break;

        case IPCOM_ERR_INVALID_ARG:
            ipcom_printf("Unmatched string delimiters"IP_LF);
            goto fail;

        case IPCOM_ERR_FAILED:
        default:
            goto fail;
        }

        /* Store */
        alias->argv = argv;
        alias->argc = argc;

        /* Link the alias into the major list */
        ipcom_list_insert_first(&ipcom_shellalias_head, &alias->next);

        /* Success */
        return IPCOM_SUCCESS;
    }

    /* No memory */
    return -IP_ERRNO_ENOMEM;

fail:
    /* Did we manage to allocate the name? */
    if (alias->name != IP_NULL)
        ipcom_free(alias->name);

    /* Free the stored command */
    if (alias->cmd != IP_NULL)
        ipcom_free(alias->cmd);

    /* Free the vector */
    if (alias->argv != IP_NULL)
        ipcom_free(alias->argv);

    /* Free the structure */
    ipcom_free(alias);
    return -IP_ERRNO_ENOMEM;
}


/*
 *===========================================================================
 *                    ipcom_shellalias_list
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipcom_shellalias_list()
{
    /* Dump */
    ipcom_shellalias_find(IP_NULL);

    /* Don't bother */
    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                    ipcom_shellalias_substitute
 *===========================================================================
 * Description: Determines what substitutins should be made
 * Parameters:  argc    - Old argc
 *              argv    - Old argv
 *              alias   - The alias we're working with
 *              nargv   - The new alias argv string
 * Returns:     >= 0 The total number of arguments, < 0 error code
 *
 */
IP_STATIC int
ipcom_shellalias_substitute(int                             argc,
                            char                            **argv,
                            struct Ipcom_shellalias_alias   *alias,
                            char                            **nargv)
{
    int                             nargc   = 0;
    int                             i;
    Ip_bool                         subst   = IP_FALSE;

    /* Do hunt for substitutions */
    for (i = 0; i < alias->argc; i++)
    {
        char            *ptr = alias->argv[i];
        long            start;
        long            end;
        Ip_bool         wildcard = IP_FALSE;

        /* Substitution? */
        if (*ptr++ == '!')
        {
            /* See what kind of substitution we're dealing with */
            switch (*ptr)
            {
            case '\0':
                /* Just a simple '!' */
                break;
            case '^':
                start = 1;
                ptr++;
                goto parse_range;
            case '$':
                start   = argc - 1;
                end     = argc - 1;
                goto substitute_range;
            case '*':
                wildcard = IP_TRUE;
                start   = 1;
                end     = argc - 1;
                goto substitute_range;
            case '-':
                /* This is a -number subtitution, which indicates 0 - num substitution. */
                start = 0;
                goto parse_range;
            default:
                if (ipcom_isdigit(*ptr))
                {
                    /* Resolve */
                    start = ipcom_strtol(ptr, &ptr, 10);

parse_range:
                    /* Is it a range or a single value? */
                    if (*ptr == '-')
                    {
                        ptr++;

                        /* It's a range */
                        if (*ptr == '\0')
                            end = argc - 2;
                        else if (*ptr == '$')
                            end = argc - 1;
                        else if (ipcom_isdigit(*ptr))
                        {
                            /* Resolve */
                            end = ipcom_strtol(ptr, &ptr, 10);
                        }
                        else
                        {
                            ipcom_printf("alias: [%s] !x- - substitution requested, but argument following '-' is erronous"IP_LF,
                                         alias->name);
                            return -IPCOM_ERR_INVALID_ARG;
                        }
                    }
                    else if (*ptr == '*')
                    {
                        wildcard = IP_TRUE;
                        end = argc - 1;
                    }
                    else
                        end = start;

substitute_range:
                    /* OK */
                    subst = IP_TRUE;

                    /* Verify if we've gotten the required arguments */
                    if (start < 0 || start >= argc)
                    {
                        /* We've gotten a wildcard match, but we aint got the arguments. Continue */
                        if (wildcard)
                            continue;

                        ipcom_printf("alias: [%s] !x - substitution requested, but insufficient arguments supplied"IP_LF,
                                     alias->name);
                        return -IPCOM_ERR_INVALID_ARG;
                    }

                    /* Verify that we have the required number of arguments */
                    if (end >= argc)
                    {
                        ipcom_printf("alias: [%s] !x-y - substitution requested, but insufficient arguments supplied"IP_LF,
                                     alias->name);
                        return -IPCOM_ERR_INVALID_ARG;
                    }

                    /* Sanity that start <= end */
                    if (end < start)
                    {
                        ipcom_printf("alias: [%s] !x-y - substitution requested, but x > y"IP_LF,
                                     alias->name);
                        return -IPCOM_ERR_INVALID_ARG;
                    }


                    /* Go through the arugments */
                    for (; start <= end; start++)
                    {
                        if (nargv != IP_NULL)
                            nargv[nargc] = argv[start];

                        nargc++;
                    }

                    continue;
                }
            }
        }

        if (nargv != IP_NULL)
            nargv[nargc] = alias->argv[i];
        nargc++;
    }

    /* No substitutions? */
    if (subst == IP_FALSE)
    {
        /* Add all arguments to the end of the command */
        for (i = 1; i < argc; i++)
        {
            if (nargv != IP_NULL)
                nargv[nargc] = argv[i];

            nargc++;
        }
    }


    /* Return the number of arguments */
    return nargc;
}

/*
 ****************************************************************************
 * 10                   GLOBAL FUNCTIONS
 ****************************************************************************
 */


/*
 *===========================================================================
 *                    ipcom_shellalias_alias
 *===========================================================================
 * Description: Add an alias.
 * Parameters:  argc    - The number of arguments
 *              argv    - The arguments.
 * Returns:
 *
 */
IP_PUBLIC int
ipcom_shellalias_alias(int argc, char **argv)
{
    Ip_err err;

    if ((err = ipcom_shellalias_setup()) != IPCOM_SUCCESS)
        return err;

    if (argc == 1)
    {
        /* List all aliases */
        return ipcom_shellalias_list();
    }
    else if (argc != 3)
    {
        ipcom_printf("alias: syntax: alias name \"cmd\""IP_LF);
        return -1;
    }


    return ipcom_shellalias_set(argv[1], argv[2]);
}


/*
 *===========================================================================
 *                    ipcom_shellalias_alias
 *===========================================================================
 * Description: Add an alias.
 * Parameters:  argc    - The number of arguments
 *              argv    - The arguments.
 * Returns:
 *
 */
IP_PUBLIC int
ipcom_shellalias_unalias(int argc, char **argv)
{
    Ip_err err;

    if ((err = ipcom_shellalias_setup()) != IPCOM_SUCCESS)
        return err;

    if (argc != 2)
    {
        ipcom_printf("unalias: syntax: unalias name"IP_LF);
        return -1;
    }

    /* Remove this partiuclar alias */
    return ipcom_shellalias_remove(argv[1]);
}



/*
 *===========================================================================
 *                    ipcom_shellalias_expand
 *===========================================================================
 * Description: Expand a command
 * Parameters:  argc    - The number of arguments
 *              argv    - The arguments.
 * Returns:     IPCOM_SUCCESS if successful.
 *
 */
IP_PUBLIC int
ipcom_shellalias_expand(int *argc, char ***argv)
{
    char                            **aargv = *argv;
    int                             aargc   = *argc;
    struct Ipcom_shellalias_alias   *alias;
    char                            **nargv = IP_NULL;
    int                             nargc   = 0;
    int                             ret;
    Ip_err                          err;

    /* Setup */
    if ((err = ipcom_shellalias_setup()) != IPCOM_SUCCESS)
        return err;

    /* Don't expand if it begins with a '\' */
    if (aargv[0][0] == '\\')
    {
        aargv[0]++;
        return IPCOM_SUCCESS;
    }

    /* Locate */
    alias = ipcom_shellalias_find(aargv[0]);

    /* Nothing to do? */
    if (alias == IP_NULL)
        return IPCOM_SUCCESS;

    /* Do not attempt to recursively parse the same alias */
    if (alias->locked)
        return IPCOM_SUCCESS;

    /* Lock this alias */
    alias->locked = IP_TRUE;

    /* Verify the substitutions */
    if ((nargc = ipcom_shellalias_substitute(aargc,aargv,alias,IP_NULL)) < 0)
    {
        alias->locked = IP_FALSE;
        return nargc;
    }

    /* We located an alias. We need to reallocate the vector so that we
     * can insert all arguments in the current command after the arguments
     * added by the alias */
    nargv = ipcom_malloc(sizeof(char *) * (nargc + 1));

    /* Failed to reallocate vector? */
    if (nargv == IP_NULL)
    {
        alias->locked = IP_FALSE;
        return -IP_ERRNO_ENOMEM;
    }

    /* NULL Terminate, some commands require this */
    nargv[nargc] = IP_NULL;

    /* Perform the previously calculated subistitutions */
    ipcom_shellalias_substitute(aargc,aargv,alias,nargv);

    /* Free previous argument vector */
    ipcom_free(*argv);

    /* Set new command vector */
    *argv = nargv;
    *argc = nargc;

#if 0
    for (ret = 0; ret < nargc; ret++)
        ipcom_printf("%s ", nargv[ret]);
    ipcom_printf(IP_LF);
#endif

    /* Do recursively expand */
    ret = ipcom_shellalias_expand(argc, argv);

    /* Lock this alias */
    alias->locked = IP_FALSE;

    return ret;
}


/*
 ****************************************************************************
 * 11                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

#else
int ipcom_shellalias_empty_file;
#endif /* defined(IPCOM_USE_ALIAS) && IPCOM_USE_SHELL == IPCOM_SHELL_IPCOM */


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
