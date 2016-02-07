/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_getopt.c,v $ $Revision: 1.27 $
 *     $Source: /home/interpeak/CVSRoot/ipcom/src/ipcom_getopt.c,v $
 *     $Author: jonas $ $Date: 2009-04-22 13:27:15 $
 *     $State: Exp $ $Locker:  $
 *
 *     INTERPEAK_COPYRIGHT_STRING
 *     Design and implementation by Lennart Bang <lob@interpeak.se>
 ******************************************************************************
 */

/*
 ****************************************************************************
 * 1                    BSD COPYRIGHT
 ****************************************************************************
 * Interpeak getopt() based on a NetBSD version. The code bas been rewritten
 * to support reentrant function versions and long options, e.g. '-name'.
 *
 *	$NetBSD: getopt.c,v 1.17 2000/01/22 22:19:19 mycroft Exp $
 * Copyright (c) 1987, 1993, 1994
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
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
#include "ipcom_getopt.h"
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
#define	BADCH	(int)'?'
#define	BADARG	(int)':'
#define	EMSG	""


/*
 ****************************************************************************
 * 7                    LOCAL PROTOTYPES
 ****************************************************************************
 */
IP_STATIC int ipcom_getopt_longopt(Ipcom_getopt *opt, char *name, char *delim);


/*
 ****************************************************************************
 * 8                    DATA
 ****************************************************************************
 */

/* Global IPCOM getopt data for the old non-reentrant getopt() functions. */
IP_PUBLIC Ipcom_getopt ipcom_getopt_data;


/*
 ****************************************************************************
 * 9                    FUNCTIONS
 ****************************************************************************
 */


/*
 *===========================================================================
 *                    ipcom_getopt_longopt
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipcom_getopt_longopt(Ipcom_getopt *opt, char *name, char *delim)
{
    int i, found;
    struct Ipcom_getopt_longopt_struct  *lopt;

    if (!*name)
        return -1;

    found = -1;
    for(i = 0, lopt = opt->long_options; lopt->name && *lopt->name; lopt++, i++)
    {
        if (ipcom_strncmp(lopt->name, name, delim - name) == 0)
        {
            /* Exact match. */
            if (ipcom_strlen(lopt->name) == (Ip_size_t)((Ip_ptrdiff_t)delim - (Ip_ptrdiff_t)name))
                return i;

            /* Partial match. */
            if (found == -1)
                found = i;
            else
                found = -2;  /* ambiguous command */
        }
    }
    return found;
}


/*
 *===========================================================================
 *                    ipcom_getopt_clear
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_getopt_clear_r(Ipcom_getopt *opt)
{
    opt->opterr   = 1;
    opt->optind   = 1;
    opt->optopt   = 0;
    opt->optreset = 0;
    opt->optarg   = IP_NULL;

    opt->place    = EMSG; 	/* option letter processing */

    opt->long_index    = 0;
    opt->long_only     = 0;
    opt->long_options  = IP_NULL;
}

IP_PUBLIC void
ipcom_getopt_clear(void)
{
    ipcom_getopt_clear_r(&ipcom_getopt_data);
}


/*
 *===========================================================================
 *                    ipcom_getopt
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipcom_getopt_r(int argc, char * const *argv, const char *optstring, Ipcom_getopt *opt)
{
    const char *oli;   /* option letter list index */

    ip_assert(argv);
    ip_assert(optstring);

    if (opt->optreset || !*opt->place)
    {
        /* update scanning pointer */
        opt->optreset = 0;

        if (opt->optind >= argc)
            goto emsg;
        opt->place = argv[opt->optind];
        if (*opt->place != '-' && *opt->place != '+')
            goto emsg;

        if (opt->place[1] && *++opt->place == '-'	/* found "--" */
            && opt->place[1] == '\0')
        {
            ++opt->optind;
        emsg:
            opt->place = EMSG;
            return -1;
        }
    }

    /* long option? */
    if (opt->long_options != IP_NULL &&
        (opt->long_only || *opt->place == '+'))
    {
        int indx;
        char *delim;

        /* allow an extra '-' before longoption name, i.e. '--optname' is ok. */
        if (*opt->place == '-')
            opt->place++;

        for(delim = opt->place; *delim && *delim != '='; delim++)
        {
            /* NOP */
        }

        indx = ipcom_getopt_longopt(opt, opt->place, delim);
        if (indx >= 0)
        {
            opt->long_index = indx;
            ++opt->optind;
            opt->place = EMSG;

            /* option does not take an argument. */
            if (*delim != '=' && opt->long_options[indx].has_arg == 0)
                opt->optarg = IP_NULL;
            /* no more arguments but optional argument is ok. */
            else if (*delim != '=' && argc == opt->optind && opt->long_options[indx].has_arg == 2)
                opt->optarg = IP_NULL;
            /* option requires an argument but no more exist. */
            else if (*delim != '=' && argc <= opt->optind)
            {
#if defined(IP_PORT_LAS) || IPCOM_USE_SHELL != IPCOM_SHELL_NONE
                if (opt->opterr)
                    (void)ipcom_fprintf(ip_stderr, "%s: long option `%s' requires an argument"IP_LF,
                                        argv[0], opt->long_options[indx].name);
#endif
                return BADCH;
            }
            else
            {
                if (opt->long_options[indx].flag)
                    *opt->long_options[indx].flag = opt->long_options[indx].val;

                if (*delim == '=')
                    opt->optarg = ++delim;
                else
                    opt->optarg = argv[opt->optind++];
            }
            return 0;
        }
        else
        {
            /* A single option letter, check it with standard options. */
            if (opt->place[1] == '\0' /*&& !opt->long_only*/)
                goto single_letter;

#if IPCOM_USE_SHELL != IPCOM_SHELL_NONE
            if (opt->opterr)
            {
                if (indx == -2)
                    ipcom_fprintf(ip_stderr, "%s: long option `%s' is ambiguous"IP_LF, argv[0], opt->place);
                else
                    ipcom_fprintf(ip_stderr, "%s: unrecognized long option `%s'"IP_LF, argv[0], opt->place);
            }
#endif
            return BADCH;
        }
        /*goto argument;*/
    }

    /* option letter okay? */
 single_letter:
    if (!*opt->place)
    {
#if IPCOM_USE_SHELL != IPCOM_SHELL_NONE
        if (opt->opterr)
            (void)ipcom_fprintf(ip_stderr, "%s: missing option name"IP_LF, argv[0]);
#endif
        return BADCH;
    }
    if ((opt->optopt = (int)*opt->place++) == (int)':' ||
        0 == (oli = ipcom_strchr(optstring, opt->optopt)))
    {
        /*
         * if the user didn't specify '-' as an option,
         * assume it means -1.
       */
        if (opt->optopt == (int)'-')
            return -1;

        if (!*opt->place)
            ++opt->optind;

#if IPCOM_USE_SHELL != IPCOM_SHELL_NONE
        if (opt->opterr && *optstring != ':')
            (void)ipcom_fprintf(ip_stderr, "%s: illegal option -- %c"IP_LF, argv[0], opt->optopt);
#endif

        return BADCH;
    }

    /* argument:  */
    /* don't need argument */
    if (*++oli != ':')
    {
        opt->optarg = IP_NULL;
        if (!*opt->place)
            ++opt->optind;
    }
    else
    {
        /* need an argument */
        if (*opt->place)			/* no white space */
            opt->optarg = opt->place;

        else if (argc <= ++opt->optind)
        {
            /* no arg */
            opt->place = EMSG;

            if (*optstring == ':')
                return BADARG;

#if IPCOM_USE_SHELL != IPCOM_SHELL_NONE
            if (opt->opterr)
                (void)ipcom_fprintf(ip_stderr, "%s: option requires an argument -- %c"IP_LF, argv[0], opt->optopt);
#endif

            return BADCH;
        }
        else
            opt->optarg = argv[opt->optind];   /* white space */

        opt->place = EMSG;
        ++opt->optind;
    }

    /* dump back option letter */
    return opt->optopt;
}


/*
 *===========================================================================
 *                    ipcom_getopt
 *===========================================================================
 * Context:
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipcom_getopt(int argc, char * const *argv, const char *optstring)
{
    return ipcom_getopt_r(argc, argv, optstring, &ipcom_getopt_data);
}


/*
 ****************************************************************************
 *                         END OF FILE
 ****************************************************************************
 */

