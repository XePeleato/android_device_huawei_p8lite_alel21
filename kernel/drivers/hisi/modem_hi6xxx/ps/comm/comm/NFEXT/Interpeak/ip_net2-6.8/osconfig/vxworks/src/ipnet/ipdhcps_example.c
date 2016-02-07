/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipdhcps_example.c,v $ $Revision: 1.2 $
 *     $Source: /home/interpeak/CVSRoot/ipcom/port/vxworks/osconfig/vxworks/src/ipnet/Attic/ipdhcps_example.c,v $
 *     $Author: ulf $
 *     $State: Exp $ $Locker:  $
 *
 *     Copyright 2000-2007 Interpeak AB (http://www.interpeak.se). All rights reserved.
 *
 *     Design and implementation by Jonas Rendel <jonas@interpeak.se>
 ******************************************************************************
 */

/*
modification history
--------------------
01b,03jan07,kch  Added isc configuration file parsing example routines.
01a,11dec06,kch  Integrated to VxWorks build infrastructure.
*/

/*
 * DESCRIPTION
 *
 * This file contains example implementation of the callback routine that is
 * called when dhcp server is startup and upon termination. It demonstrates
 * how to save lease database at shutdown and how to restore lease database
 * at startup.
 *
 * These examples are provided as unsupported code.
 */

/* includes */
#include <ipcom_config.h>

#ifdef IPCOM_USE_CLIB_PROTO
#undef IPCOM_USE_CLIB_PROTO
#endif

#include <ipcom_clib.h>
#include <ipcom_os.h>
#include <ipcom_err.h>
#include <ipcom_tmo.h>
#include <ipcom_shell.h>
#include <ipcom_syslog.h>
#include <ipcom_file.h>

#include "ipdhcps.h"

/* local prototypes */
IP_STATIC int exapmle_restore_lease_db(void);
IP_STATIC int example_dump_lease_db(void);
IP_STATIC Ip_s32 example_lease_dump_tmo_func(struct Ipcom_tmo_struct *tmo,
                                             void *cookie);


/* IPCOM timeout handler used in the configuration example below. */
IP_STATIC Ipcom_tmo lease_dump_tmo;

#ifdef IPDHCPS_UNSUPPORTED_EXAMPLE

/****************************************************************************
 *
 * example_restore_lease_db - restore DHCP Server lease database
 * 
 * This is an example on how the contents of the IPDHCPS lease database can
 * be restored by initializing it with data stored in a file system.
 *
 * RETURNS:
 * 0 if success, -1 if failed.
 *
 * NOMANUAL
 */
IP_STATIC int example_restore_lease_db(void)
{
	int retval   = -1;
	IP_FILE *fd  = IP_NULL;
	struct Ip_stat statbuf;
	void *buf = IP_NULL;

	if(ipcom_stat("/home/jonas/dumpfile", &statbuf) == -1)
		goto leave;

	if((fd = ipcom_fopen("/home/jonas/dumpfile", "r")) == 0)
		goto leave;

	if((buf = ipcom_malloc(statbuf.st_size)) == IP_NULL)
		goto leave;

	if(ipcom_fread(buf, statbuf.st_size, 1, fd) <= 0)
		goto leave;

	ipdhcps_lease_db_restore(buf);

	retval = 0;

leave:

	if(fd)
		ipcom_fclose(fd);

	if(buf != IP_NULL)
		ipcom_free(buf);

	return retval;
}


/****************************************************************************
 *
 * example_dump_lease_db - dump DHCP server lease database
 *
 * This is an example on how to save the current contents of the IPDHCPS
 * lease database to a file system. The routine is adapted for a UNIX file
 * system.
 *
 * RETURNS:
 * 0 if success or -1 if failure
 *
 * NOMANUAL
 */
IP_STATIC int example_dump_lease_db(void)
{
	int retval  = -1;
	void *buf   = IP_NULL;
	IP_FILE *fd = IP_NULL;
	int n;

	if((n = ipdhcps_lease_db_dump(&buf)) < 0)
		goto leave;

	if((fd = ipcom_fopen("/home/jonas/dumpfile","w+")) == 0)
		goto leave;

	if((ipcom_fwrite(buf, n, 1, fd)) <= 0)
		goto leave;

	retval = 0;

leave:
	if(fd)
		ipcom_fclose(fd);
	if(buf != IP_NULL)
		ipcom_free(buf);
	return retval;
}

/****************************************************************************
 *
 * example_lease_dump_tmo_func - dump dhcp server database periodically
 *
 * This is an example on how the contents of the lease database can be dumped
 * periodically to a file during  execution of IPDHCPS.
 *
 * RETURNS:
 * 0 if success or -1 if failure
 *
 * NOMANUAL
 */

IP_STATIC Ip_s32 example_lease_dump_tmo_func(struct Ipcom_tmo_struct *tmo,
                                             void *cookie)
{
	if(example_dump_lease_db() == 0)
		return 0;
	else
		return 1;
}

#if defined (IPDHCPS_INCLUDE_CMD) && defined (IPDHCPS_INCLUDE_ISC_EXAMPLE)

IP_GLOBAL int ipdhcps_cmd_dhcps(int argc, char **argv);

typedef struct _example_token_buffer
{
    char    *llines;
    char    *lines;
    char    *ltokens;
    char    *tokens;

    /**/
    char    *spec;

    /**/
    Ip_bool eof;
    int     curline;
} example_token_buffer_t;



/****************************************************************************
 *
 * example_do_command - Execute a DHCPS command
 *
 * This routine execute a DHCPS command containing the parameters we're
 * interested in adding to the configuration.
 * 
 * .IP <format> 
 * The format string
 *
 * .IP <...>
 * The arguments for the format string
 * 
 * RETURNS:
 * N/A
 *
 * NOMANUAL
 */
IP_STATIC void
example_do_command(const char *format, ...)
{
    va_list     vargs;
    char        *cmd;
    int         argc;
    char        **argv = IP_NULL;

    if ((cmd = ipcom_malloc(256)) != IP_NULL)
    {
        va_start(vargs, format);
        ipcom_vsnprintf(cmd, 256, format, vargs);        
        va_end(vargs);       

        
        IPCOM_LOG1(INFO,"DHCP Executing '%s'", cmd);
        if(ipcom_parse_argstr(cmd, (int*)&argc, &argv) != IPCOM_SUCCESS)
        {
            IPCOM_LOG1(ERR,"failed to parse command '%s'", cmd);
            goto out;
        }

        if(ipdhcps_cmd_dhcps(argc, argv) != IPCOM_SUCCESS)
        {
            IPCOM_LOG1(ERR,"failed to execute command '%s'", cmd);
            goto out;
        }

out:
        if (argv != IP_NULL)
            ipcom_free(argv);
        if (cmd)
            ipcom_free(cmd);
    }
}


/*****************************************************************************
 *
 * example_get_line - Fetch the next available line into the tokenizer buffer
 *
 * .IP <tok>
 * The tokenizer buffer
 *
 * RETURNS:
 * IP_TRUE on success, IP_FALSE if we're at EOF.
 *
 * NOMANUAL
 */
IP_STATIC Ip_bool
example_get_line(example_token_buffer_t *tok)
{
    char *line;
    char *t;

restart:
    /* Already EOF */
    if (tok->eof)
        return IP_FALSE;

    /* Get next */
    line =  ipcom_strtok_r(tok->lines, "\r\n", &tok->llines);
    tok->curline++;
    tok->lines = IP_NULL;

    /* No more lines? */
    if (line == IP_NULL)
    {
        tok->eof = IP_TRUE;
        return IP_FALSE;
    }

    /* Discard comments */
    t = ipcom_strchr(line, '#');
    if (t)
        *t = '\0';

    /* Empty line; restart */
    if (ipcom_strlen(line) == 0)
        goto restart;

    /* Set the tokens; discard any stored values */
    tok->tokens     = line;
    tok->spec       = IP_NULL;

    return IP_TRUE;
}


/*****************************************************************************
 *
 * example_get_token - Get the next token from the tokenizer
 *
 * .IP <tok>
 * The tokenizer buffer
 *
 * .IP <nl>
 * IP_TRUE if we're allowed to consume newlines
 *
 * RETURNS:
 * A token if one is available, IP_NULL otherwise.
 *
 * NOMANUAL
 */
IP_STATIC char *
example_get_token(example_token_buffer_t *tok, Ip_bool nl)
{
    char *token;

    if (tok->eof)
    {
        /* We're out of tokens */
        return IP_FALSE;
    }
    else if (tok->spec)
    {
        /* There's a special token stored. ';' */
        token = tok->spec;
        tok->spec = IP_NULL;
    }
    else
    {
        char *t;
        while ((token = ipcom_strtok_r(tok->tokens, ",\t ", &tok->ltokens)) == IP_NULL)
        {
            if (nl && example_get_line(tok))
                continue;
            return IP_NULL;
        }
 
        /* Clear the tokens buffer */
        tok->tokens = IP_NULL;

        /* Need to separate semicolons; they're used as 'eol' */
        if (ipcom_strchr(token, ';') != IP_NULL)
            tok->spec = ";";

        token = ipcom_strtok_r(token, ";", &t);
    }

    return token;
}


/*****************************************************************************
 * 
 * example_check_token - Check that the next token matches what is required.
 *
 * .IP <tok>
 * The tokenizer buffer
 *
 * .IP <req>
 * The required token
 *
 * .IP <nl>
 * IP_TRUE if we're allowed to consume newlines
 *
 * RETURNS:
 * IP_TRUE on success, IP_FALSE otherwise
 *
 * NOMANUAL
 */
IP_STATIC Ip_bool
example_check_token(example_token_buffer_t *tok, const char *req, Ip_bool nl)
{
    char *token = example_get_token(tok, nl);
    if (token)
    {
        if (req && ipcom_strcasecmp(token, req) == 0)
            return IP_TRUE;
        return IP_FALSE;
    }

    return req? IP_FALSE : IP_TRUE;
}


/*****************************************************************************
 * 
 * example_concatenate_tokens - Concatenate all options on this line
 *
 * .IP <tok>
 * The tokenizer buffer
 *
 * RETURNS:
 * A dynamically allocated memory buffer containing all the tokenized options
 * on success, IP_NULL otherwise.
 *
 * NOMANUAL
 */
IP_STATIC char *
example_concatenate_tokens(example_token_buffer_t *tok)
{
    char *value;
    char *conc = ipcom_malloc(256);

    if (conc != IP_NULL)
    {
        int no_values = 0;

        conc[0] = '\0';
        while ((value = example_get_token(tok, IP_FALSE)) != IP_NULL)
        {
            if (ipcom_strcasecmp(value, ";") == 0)
                break;

            no_values++;
            ipcom_strncat(conc, value, 256);
            ipcom_strncat(conc, " ", 256);
        }

        /* No semicolon or no values */
        if (no_values == 0 || value == IP_NULL)
        {
            ipcom_free(conc);
            return IP_NULL;
        }

        return conc;
    }
    return IP_NULL;
}


/*****************************************************************************
 * 
 * example_parse_option - Parse a ISC option string
 *
 * .IP <tok>
 * The tokenizer buffer
 *
 * .IP <context>
 * The context the option resides in
 *
 * RETURNS:
 * 0 on success.
 *
 * NOMANUAL
 */
IP_STATIC Ip_s32
example_parse_option(example_token_buffer_t *tok, const char *context)
{
    /* Retrieve the option name */
    char *option = example_get_token(tok, IP_FALSE);
    if (option == IP_NULL)
    {
        IPCOM_LOG1(ERR, 
                   "DHCPS(%d): no option name specified",
                   tok->curline);
        return -1;

    }

    if (ipcom_strcasecmp(option, "domain-name-servers") == 0)
    {
        /* DNS servers option; retrieve the list of servers and pass it
         * to our dhcps command */
        char *value = example_concatenate_tokens(tok);

        if (value == IP_NULL)
        {
            IPCOM_LOG1(ERR, 
                       "DHCPS(%d): No servers specified or lacking ';' in domain-name-servers option",
                       tok->curline);
            return -1;
        }

        example_do_command("dhcps option add %s 5 %s", context, value);
        ipcom_free(value);
    }
    else if (ipcom_strcasecmp(option, "domain-name") == 0)
    {
        /* A domain name */
        char *value = example_get_token(tok, IP_FALSE);
        if (!example_check_token(tok, ";", IP_FALSE))
        {
            IPCOM_LOG1(ERR, 
                       "DHCPS(%d): No domain name specified or lacking ';' in domain-name option",
                       tok->curline);
            return -1;
        }
        
        example_do_command("dhcps option add %s 15 %s", context, value);
    }
    else if (ipcom_strcasecmp(option, "routers") == 0)
    {
        /* One or many routers */
        char *value = example_concatenate_tokens(tok);
        if (value == IP_NULL)
        {
            IPCOM_LOG1(ERR, 
                       "DHCPS(%d): No servers specified or lacking ';' in routers option",
                       tok->curline);
            return -1;
        }

        example_do_command("dhcps option add %s 3 %s", context, value);
        ipcom_free(value);
    }
    else
    {
        /* Unknown or unhandled option */
        IPCOM_LOG2(WARNING, 
                   "DHCPS(%d): Unknown option %s",
                   tok->curline, 
                   option);
        return -1;
    }

    return 0;
}

/*****************************************************************************
 * 
 * example_parse_subnet - Parse an ISC subnet directive
 *
 * .IP <tok>
 * The tokenizer buffer.
 *
 * RETURNS:
 * 0 on success
 *
 * NOMANUAL
 */
IP_STATIC Ip_s32
example_parse_subnet(example_token_buffer_t *tok)
{
    /* address NETMASK netmask { */
    char            *address, *netmask;
    char            *token;

    /* We need to retrieve the address, the NETMASK keyword and the
     * netmask value */
    address = example_get_token(tok, IP_FALSE);
    if (!example_check_token(tok, "netmask", IP_FALSE))
        return -1;
    netmask = example_get_token(tok, IP_FALSE);

    /**/
    if (address == IP_NULL || netmask == IP_NULL)
        return -1;

    /* The subnet block starts with a left bracer */
    if (!example_check_token(tok, "{", IP_TRUE))
        return -1;

    /* Add the subnet command */
    example_do_command("dhcps subnet add %s %s", address, netmask);

    /* Go through the subnet */
    while ((token = example_get_token(tok, IP_TRUE)) != IP_NULL)
    {
        int retval;

        /* End of subnet is a right bracer, exit successfully if found */
        if (ipcom_strcasecmp(token, "}") == 0)
            return 0;
        else if (ipcom_strcasecmp(token, "option") == 0)
        {
            /* Parse a subnet specific option */
            if ((retval = example_parse_option(tok, address)) != 0)
                return retval;            
        }        
        else if (ipcom_strcasecmp(token, "range") == 0)
        {
            /* Parse a pool range */
            char *start, *end;
            start   = example_get_token(tok, IP_FALSE);
            end     = example_get_token(tok, IP_FALSE);
            if (!example_check_token(tok, ";", IP_FALSE))
            {
                IPCOM_LOG1(ERR, 
                           "DHCPS(%d): Missing range start, end or ';' in range directive",
                           tok->curline);
                return -1;
            }

            example_do_command("dhcps pool add %s %s %s", address, start, end);
        }
        else if (ipcom_strcasecmp(token, "default-lease-time") == 0)
        {
            char *lease;
            lease   = example_get_token(tok, IP_FALSE);
            if (!example_check_token(tok, ";", IP_FALSE))
            {
                IPCOM_LOG1(ERR, 
                           "DHCPS(%d): lease time specified or ';' missing in default-lease-time directive",
                           tok->curline);
                return -1;
            }

            example_do_command("dhcps config set %s default-lease-time %s", address, lease);
        }
        else if (ipcom_strcasecmp(token, "max-lease-time") == 0)
        {
            char *lease;
            lease   = example_get_token(tok, IP_FALSE);
            if (!example_check_token(tok, ";", IP_FALSE))
            {
                IPCOM_LOG1(ERR, 
                           "DHCPS(%d): lease time specified or ';' missing in max-lease-time directive",
                           tok->curline);
                return -1;
            }

            example_do_command("dhcps config set %s max-lease-time %s", address, lease);
        }
    }
}


/*****************************************************************************
 *
 * example_parse_base -  Parse top-level ISC configuration items
 * 
 * .IP <tok>
 * The tokenizer buffer
 *
 * RETURNS:
 * 0 on success
 *
 * NOMANUAL
 */
IP_STATIC Ip_s32
example_parse_base(example_token_buffer_t *tok)
{
    for (;;)
    {
        char *token = example_get_token(tok, IP_TRUE);
        char *value;
        int ret;

        /* EOF; We've read the entire file - exit */
        if (token == IP_NULL)
            return 0;

        /* Check the token and see if we know it */
        if (ipcom_strcasecmp(token, "subnet") == 0)
        {
            ret = example_parse_subnet(tok);
        }
        else if (ipcom_strcasecmp(token, "option") == 0)
        {
            ret = example_parse_option(tok, "default");
        }
        else if (ipcom_strcasecmp(token, "default-lease-time") == 0)
        {
            value   = example_get_token(tok, IP_FALSE);
            if (!example_check_token(tok, ";", IP_FALSE))
            {
                IPCOM_LOG1(ERR, 
                           "DHCPS(%d): lease time specified or ';' missing in default-lease-time directive",
                           tok->curline);
                return -1;
            }

            example_do_command("dhcps config set default default-lease-time %s", value);
        }
        else if (ipcom_strcasecmp(token, "max-lease-time") == 0)
        {
            value   = example_get_token(tok, IP_FALSE);
            if (!example_check_token(tok, ";", IP_FALSE))
            {
                IPCOM_LOG1(ERR, 
                           "DHCPS(%d): lease time specified or ';' missing in max-lease-time directive",
                           tok->curline);
                return -1;
            }

            example_do_command("dhcps config set default max-lease-time %s", value);
        }
        else if (ipcom_strcasecmp(token, "authoritative") == 0 ||
                 ipcom_strcasecmp(token, "ddns-update-style") == 0 ||
                 ipcom_strcasecmp(token, "log-facility") == 0)
        {
            /* Unsupported ISC directives
             * Consume this line
             */
            IPCOM_LOG2(WARNING, 
                       "DHCPS(%d): Unhandled directive %s specified",
                       tok->curline,
                       token);
            example_get_line(tok);
        }
    }    
}

/*****************************************************************************
 *
 * example_cfgfile - Read an ISC DHCP configuration file
 * 
 * .IP <cfgfile>
 * The configuration file
 *
 * RETURNS:
 * 0 on success, -1 or errorcode otherwise.
 *
 * NOMANUAL
 */
IP_GLOBAL Ip_s32
example_isc_config(const char *cfgfile)
{
    int retval = -1;
    char *buf = IP_NULL;
    IP_FILE *fd  = IP_NULL;
    struct Ip_stat st;
    example_token_buffer_t tbuf;

    /* Failed to read */
    if (ipcom_stat(cfgfile, &st) != 0)
    {
        IPCOM_LOG1(ERR, 
                   "DHCPS: Failed to stat configuration file %s",
                   cfgfile);
        goto leave;
    }

    /* Failed to open */
    if((fd = ipcom_fopen(cfgfile,"r")) == 0)
    {
        IPCOM_LOG1(ERR, 
                   "DHCPS: Failed to open configuration file %s",
                   cfgfile);
        goto leave;
    }

    /* Allocate buffer */
    if ((buf = ipcom_malloc(st.st_size + 1)) == IP_NULL)
    {
        IPCOM_LOG1(ERR, 
                   "DHCPS: Failed to allocate memory for configuration file %s",
                   cfgfile);
        goto leave;
    }

    /* Read entire file */
    if((ipcom_fread(buf, 1, st.st_size, fd)) != st.st_size)
    {
        IPCOM_LOG1(ERR, 
                   "DHCPS: Failed to read configuration file %s",
                   cfgfile);
        goto leave;
    }

    /* Zero terminate; */
    buf[st.st_size] = '\0';

    /* Do parse the configuration file */
    ipcom_memset(&tbuf, 0, sizeof(tbuf));
    tbuf.lines = buf;
    example_get_line(&tbuf);

    /* Parse the file */
    retval = example_parse_base(&tbuf);

leave:
    if (fd)
        ipcom_fclose(fd);
    if(buf != IP_NULL)
        ipcom_free(buf);
    return retval;
}

#endif /* IPDHCPS_INCLUDE_CMD && IPDHCPS_INCLUDE_ISC_EXAMPLE */


/******************************************************************************
 *
 * ipdhcps_example_start_hook - example dhcps startup callback routine
 * 
 * This is an example implementation of the callback routine that is called
 * by the DHCP server startup.  
 *
 * RETURNS:
 * 0 if success or -1 if failure
 *
 * NOMANUAL
 */
IP_GLOBAL int ipdhcps_example_start_hook (void)
{
#ifdef IPDHCPS_USE_EXAMPLE    
    /* Restore lease database */
	example_restore_lease_db ();

	ipcom_tmo_request (&lease_dump_tmo,
					   example_lease_dump_tmo_func,
					   &lease_dump_tmo,
					   10*60*60*1000);
#endif /* IPDHCPS_USE_EXAMPLE */

#if defined (IPDHCPS_INCLUDE_CMD) && defined (IPDHCPS_INCLUDE_ISC_EXAMPLE)
    {
        char *cfgfile = ipcom_sysvar_get("ipdhcps.iscfile", IP_NULL, IP_NULL);
        if (cfgfile)
        {
            ret = example_isc_config(cfgfile);
            ipcom_free(cfgfile);
        }
    }    
#endif    

    return 0;
}

/******************************************************************************
 *
 * ipdhcps_example_stop_hook - example dhcp server terminate callback routine
 *
 * This is an example implementation of the callback routine that is called
 * by the DHCP server upon termination.
 *
 * RETURNS:
 * 0 if success or -1 if failure
 *
 * NOMANUAL
 */
IP_GLOBAL int ipdhcps_example_stop_hook (void)
{
	(void)ipcom_tmo_cancel (&lease_dump_tmo);
	example_dump_lease_db ();
	return 0;
}

#endif /* IPDHCPS_UNSUPPORTED_EXAMPLE */
