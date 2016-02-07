/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *   Document no: @(#) $ipftpc.c$ $RCSfile: ipftpc_cmd_ftp.c,v $ $Revision: 1.14 $
 *   $Source: /home/interpeak/CVSRoot/ipappl/src/ipftpc_cmd_ftp.c,v $
 *   $Author: ulf $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Ulf Olofsson <ulf@interpeak.se>
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

#include "ipftpc_cmd_config.h"


/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#define IPCOM_USE_CLIB_PROTO
#include <ipcom_clib.h>
#include "ipcom_shell.h"

#define IPFTPC_CMD_C
#include "ipftpc_cmd.h"
#include "ipftpc_api.h"


/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

/* Do FTP command, check return code, print error message, take action */
#define IPFTPC_IF(func,cond,msg,act) /*lint -e665 */ \
do \
{ \
    int _ret; \
    if ((_ret = func) cond) \
    { \
        ipftpc_perror(vars, msg, _ret); \
        act; \
    } \
} \
while (0)

#ifdef IPCOM_USE_INET6
#define IPFTPC_PORT(s) *((s)->sa.sa_family == IP_AF_INET ? \
                       &(s)->sin.sin_port : &(s)->sin6.sin6_port)
#else
#define IPFTPC_PORT(s) ((s)->sin.sin_port)
#endif

#ifndef IP_AF_UNSPEC
#define IP_AF_UNSPEC 0
#endif

#define IPFTPC_FREE(x) \
        if ((x) != IP_NULL) do {ipcom_free(x); (x) = IP_NULL; (void)(x);} while (0)


/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

struct Ipftpc_vars_struct; /* dummy prot */

/* Command descriptor */
typedef struct Ipftpc_cmd_struct
{
    const char *name;                   /* Command name */
    int (*func)(struct Ipftpc_vars_struct *vars, int argc, char *argv[]);
                                        /* Command handler */
    Ip_u8 min;                          /* Min no of arguments */
    Ip_u8 max;                          /* Max no of arguments */
    const char *parms;                  /* Parms text */
    const char *help;                   /* Help text */
}
Ipftpc_cmd;

/* Session descriptor */
typedef struct Ipftpc_vars_struct
{
    Ipftpc_attr attr;                   /* Options */
    Ipftpc_addr addr;                   /* Server address */
    Ipftpc_id id;                       /* Session ID */
    const Ipftpc_cmd *cmd;              /* Current conmand */
    Ipcom_getline  *gl;
    char *server;                       /* Server name or address */
    int family;                         /* Allowed address family (0,4,6)*/
    int af;                             /* Allowed address family (IP_AF_XX) */
    int port;                           /* Port number */
    int verbose;                        /* Verbose level */
    int extended;                       /* Extended mode */
    int passive;                        /* Passive mode */
    int conexclude;                     /* Exclude console printouts */
    Ip_bool doedit;                     /* Use command-line editor */
    char cmdbuf[IPFTPC_CMDBUFSIZE];     /* Command buffer */
}
Ipftpc_vars;


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

IP_PUBLIC int ipftpc_cmd_main(int argc, char *argv[]);

#include "ipftpc_cmd_proto.h"


/*
 ****************************************************************************
 * 8                    DATA
 ****************************************************************************
 */

/* List of commmands */
IP_STATIC const Ipftpc_cmd ipftpc_cmdlist[] =
{
    {"append", ipftpc_do_append, 1, 2, "local-file [remote-file]",
        "Append local-file to remote-file on server"},

    {"ascii", ipftpc_do_ascii, 0, 0, "",
        "Select ASCII mode for file transfer"},

    {"binary", ipftpc_do_binary, 0, 0, "",
        "Select binary mode for file transfer"},

    {"cd", ipftpc_do_cd, 1, 1, "remote-dir",
        "Change server working directory to remote-dir"},

    {"cdup", ipftpc_do_cdup, 0, 0, "",
        "Change to parent server working directory"},

    {"close", ipftpc_do_close, 0, 0, "",
        "Close connection to FTP server"},

    {"delete", ipftpc_do_delete, 1, 1, "remote-file",
        "Delete remote-file on server"},

    {"dir", ipftpc_do_dir, 0, 1, "[remote-path]",
        "List file attributes in remote-path on server"},

    {"extended", ipftpc_do_extended, 0, 1, "[0-2]",
        "Extended mode for IPv4: 0:Off, 1:On->Off, 2:On"},

    {"family", ipftpc_do_family, 0, 1, "[0|4|6]",
        "Specify address family: Any|IPv4|IPv6"},

    {"get", ipftpc_do_get, 1, 2, "remote-file [local-file]",
        "Get remote-file from server and store in local-file"},

    {"help", ipftpc_do_help, 0, 1, "[command-name]",
        "Get command list, or help on command-name"},

    {"ipcheck", ipftpc_do_ipcheck, 0, 1, "[0|1]",
        "Require control and data address match: OFF|ON"},

    {"lbase", ipftpc_do_lbase, 0, 1, "[0-65535]",
        "Specify lower limit for client data port number"},

    {"lcd", ipftpc_do_lcd, 1, 1, "local-dir",
        "Change client working directory to local-dir"},

    {"lpwd", ipftpc_do_lpwd, 0, 0, "",
        "Print client working directory"},

    {"ls", ipftpc_do_ls, 0, 1, "[remote-path]",
        "List file names in remote-path on server"},

    {"mkdir", ipftpc_do_mkdir, 1, 1, "remote-dir",
        "Make directory remote-dir on server"},

    {"open", ipftpc_do_open, 1, 2, "server [port]",
        "Open connection to FTP server"},

    {"passive", ipftpc_do_passive, 0, 1, "[0-3]",
        "Passive mode: 0:Act, 1:Pasv->Act, 2:Pasv, 3:Lock Pasv"},

    {"port", ipftpc_do_port, 0, 1, "[1-65535]",
        "Select server port"},

    {"put", ipftpc_do_put, 1, 2, "local-file [remote-file]",
        "Put local-file to remote-file on server"},

    {"pwd", ipftpc_do_pwd, 0, 0, "",
        "Print server working directory"},

    {"quit", ipftpc_do_quit, 0, 0, "",
        "End FTP session"},

    {"quote", ipftpc_do_quote, 1, IPFTPC_MAX_ARGS - 1, "command ...",
        "Send verbatim command to remote server"},

    {"rbase", ipftpc_do_rbase, 0, 1, "[1-65535]",
        "Specify lower limit for server data port number"},

    {"rename", ipftpc_do_rename, 2, 2, "old-file new-file",
        "Rename old-file to new-file on server"},

    {"rhelp", ipftpc_do_rhelp, 0, 1, "[command-name]",
        "Get command list, or help on command-name from remote server"},

    {"rmdir", ipftpc_do_rmdir, 1, 1, "remote-dir",
        "Remove directory remote-dir on server"},

    {"rstatus", ipftpc_do_rstatus, 0, 1, "[remote-file]",
        "Get server status, or status of remote-file"},

    {"status", ipftpc_do_status, 0, 0, "",
        "Show current FTP status"},

    {"system", ipftpc_do_system, 0, 0, "",
        "Show server operating system"},

    {"verbose", ipftpc_do_verbose, 0, 1, "[0-3]",
        "Verbose level: 0:Brief, 1:Verbose, 2:Server, 3:Client"},

    {IP_NULL}
};


/*
 ****************************************************************************
 * 9                    LOCAL FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *      ipftpc_conout
 *===========================================================================
 * Description:
 *      Print command output console.
 *
 * Parameters:
 *      vars    Session variables.
 *      msg     Error message.
 *      eno     Error number.
 *
 * Returns:
 */
IP_STATIC int
ipftpc_conout(Ipftpc_attr *attr, const char *buf, int len)
{
    (void)attr;
    (void)len;

    ipcom_printf("%s", buf);

    return 0;
}


/*
 *===========================================================================
 *      ipftpc_ctrlin
 *===========================================================================
 * Description:
 *      Print control input to console.
 *
 * Parameters:
 *      vars    Session variables.
 *      msg     Error message.
 *      eno     Error number.
 *
 * Returns:
 */
IP_STATIC void
ipftpc_ctrlin(Ipftpc_attr *attr, const char *buf, int len)
{
    Ipftpc_vars *vars = attr->data;

    (void)len;

    /* Exclude printout? */
    if (vars->verbose < 3 && vars->conexclude != 0 &&
        vars->conexclude == ipcom_strtol(buf, IP_NULL, 10))
    {
        return;
    }

    ipcom_printf("%s\n", buf);
}


/*
 *===========================================================================
 *      ipftpc_ctrlout
 *===========================================================================
Description: *  Print control output to console.
 *
 * Parameters:
 *      vars    Session variables.
 *      msg     Error message.
 *      eno     Error number.
 *
 * Returns:
 */
IP_STATIC void
ipftpc_ctrlout(Ipftpc_attr *attr, const char *buf, int len)
{
    (void)attr;
    (void)len;

    ipcom_printf("---> %s\n", buf);
}


/*
 *===========================================================================
 *      ipftpc_perror
 *===========================================================================
 * Description:
 *      Print error message.
 *
 * Parameters:
 *      vars    Session variables.
 *      msg     Error message.
 *      eno     Error number.
 *
 * Returns:
 */
IP_STATIC void
ipftpc_perror(Ipftpc_vars *vars, const char *msg, int eno)
{
    char *lastreply = ipftpc_lastreply(vars->id);

    if (lastreply != IP_NULL && lastreply[0] != '\0')
    {
        /* A server reply is available... */
        if ((vars->attr.verbose & 1) == 0)
        {
            /* ...and not printed yet */
            ipcom_printf("%s\n", lastreply);
        }
    }
    else if (vars->verbose)
    {
        /* Print verbose */
        ipcom_printf("%s: (%s: %s)\n", msg,
                     ipftpc_strclass(eno), ipftpc_strerror(eno));
    }
    else
    {
        /* Print brief */
        ipcom_printf("%s\n", msg);
    }

    /* Close session if read error or x21 status (connection terminated) */
    if (IPFTPC_GET_C(eno) == IPFTPC_CREAD ||
        (IPFTPC_GET_C(eno) == IPFTPC_CFTPS && (eno % 100) == 21))
    {
        ipcom_printf("Connection closed\n");
        (void)ipftpc_close(vars->id);
        IPFTPC_FREE(vars->server);
        vars->id = 0;
    }
}


/*
 *===========================================================================
 *      ipftpc_setextended
 *===========================================================================
 * Description:
 *      Set extended mode.
 *
 * Parameters:
 *      extended New value.
 *
 * Returns:
 */
IP_STATIC int
ipftpc_setextended(Ipftpc_vars *vars, int extended)
{
    switch (extended)
    {
    case 0:
        vars->attr.extended = 0;
        vars->attr.fb_extended = 0;
        break;
    case 1:
        vars->attr.extended = 1;
        vars->attr.fb_extended = 1;
        break;
    case 2:
        vars->attr.extended = 1;
        vars->attr.fb_extended = 0;
        break;
    default:
        return 1;
    }

    vars->extended = extended;
    return 0;
}


/*
 *===========================================================================
 *      ipftpc_setpassive
 *===========================================================================
 * Description:
 *      Set passive mode.
 *
 * Parameters:
 *      passive New value.
 *
 * Returns:
 */
IP_STATIC int
ipftpc_setpassive(Ipftpc_vars *vars, int passive)
{
    switch (passive)
    {
    case 0:
        vars->attr.passive = 0;
        vars->attr.fb_passive = 0;
        break;
    case 1:
        vars->attr.passive = 1;
        vars->attr.fb_passive = 1;
        break;
    case 2:
    case 3:
        vars->attr.passive = 1;
        vars->attr.fb_passive = 0;
        break;
    default:
        return 1;
    }

    vars->passive = passive;
    return 0;
}


/*
 *===========================================================================
 *      ipftpc_setverbose
 *===========================================================================
 * Description:
 *      Set verbose mode.
 *
 * Parameters:
 *      verbose New value.
 *
 * Returns:
 */
IP_STATIC int
ipftpc_setverbose(Ipftpc_vars *vars, int verbose)
{
    int old = vars->attr.verbose;

    if (verbose != old)
    {
        vars->attr.verbose = verbose;
        IPFTPC_IF(ipftpc_setattr(vars->id, &vars->attr), != IPFTPC_ESUCCESS,
                  "Attribute error", break);
    }

    return old;
}


/*
 *===========================================================================
 *      ipftpc_flag
 *===========================================================================
 * Description:
 *      Prepare for handling flag in command routine.
 *
 * Parameters:
 *      vars    Session variables.
 *      func    Command routine.
 *      flag    Flag.
 *
 * Returns:
 */
IP_STATIC void
ipftpc_flag(Ipftpc_vars *vars,
            int (*func)(struct Ipftpc_vars_struct *vars, int argc, char *argv[]),
            int flag)
{
    static Ipftpc_cmd mycmd;
    static char fname[3] = "-?";
    const Ipftpc_cmd *cmd;

    /* Loop for help texts */
    for (cmd = ipftpc_cmdlist;
         cmd->name != IP_NULL && cmd->func != func;
         cmd++)
    {
    }

    /* Fabricate command entry to be used when handling flag */
    if (cmd->name != IP_NULL)
    {
        fname[1] = (char)flag;
        mycmd.name = fname;
        mycmd.parms = cmd->parms;
        vars->cmd = &mycmd;
    }
}


/*
 *===========================================================================
 *      ipftpc_do_append
 *===========================================================================
 * Description:
 *      append command.
 *
 * Parameters:
 *      vars    Session variables.
 *      argc    No of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 *      0 if success, 1 if error, -1 if exit required.
 */
IP_STATIC int
ipftpc_do_append(Ipftpc_vars *vars, int argc, char *argv[])
{
    if (vars->id == 0)
    {
        ipcom_printf("Not connected\n");
        return 1;
    }

    /* Use local filename for remote if none supplied */
    if (argc == 1)
    {
        argv[1] = argv[0];
    }

    IPFTPC_IF(ipftpc_put(vars->id, argv[0], argv[1], 1), / 100 != 2,
              "File transfer error", return 1);

    return 0;
}


/*
 *===========================================================================
 *      ipftpc_do_ascii
 *===========================================================================
 * Description:
 *      ascii command.
 *
 * Parameters:
 *      vars    Session variables.
 *      argc    No of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 *      0 if success, 1 if error, -1 if exit required.
 */
IP_STATIC int
ipftpc_do_ascii(Ipftpc_vars *vars, int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    if (vars->id == 0)
    {
        ipcom_printf("Not connected\n");
        return 1;
    }

    IPFTPC_IF(ipftpc_cmd(vars->id, "TYPE A"), / 100 != 2,
              "Ascii mode failed", return 1);

    return 0;
}


/*
 *===========================================================================
 *      ipftpc_do_binary
 *===========================================================================
 * Description:
 *      binary command.
 *
 * Parameters:
 *      vars    Session variables.
 *      argc    No of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 *      0 if success, 1 if error, -1 if exit required.
 */

IP_STATIC int
ipftpc_do_binary(Ipftpc_vars *vars, int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    if (vars->id == 0)
    {
        ipcom_printf("Not connected\n");
        return 1;
    }

    IPFTPC_IF(ipftpc_cmd(vars->id, "TYPE I"), / 100 != 2,
              "Binary mode failed", return 1);

    return 0;
}


/*
 *===========================================================================
 *      ipftpc_do_cd
 *===========================================================================
 * Description:
 *      cd command.
 *
 * Parameters:
 *      vars    Session variables.
 *      argc    No of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 *      0 if success, 1 if error, -1 if exit required.
 */
IP_STATIC int
ipftpc_do_cd(Ipftpc_vars *vars, int argc, char *argv[])
{
    (void)argc;

    if (vars->id == 0)
    {
        ipcom_printf("Not connected\n");
        return 1;
    }

    IPFTPC_IF(ipftpc_cmd(vars->id, "CWD %s", argv[0]), / 100 != 2,
              "Directory change failed", return 1);

    return 0;
}


/*
 *===========================================================================
 *      ipftpc_do_cdup
 *===========================================================================
 * Description:
 *      cdup command.
 *
 * Parameters:
 *      vars    Session variables.
 *      argc    No of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 *      0 if success, 1 if error, -1 if exit required.
 */
IP_STATIC int
ipftpc_do_cdup(Ipftpc_vars *vars, int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    if (vars->id == 0)
    {
        ipcom_printf("Not connected\n");
        return 1;
    }

    IPFTPC_IF(ipftpc_cmd(vars->id, "CDUP"), / 100 != 2,
              "Directory change failed", return 1);

    return 0;
}


/*
 *===========================================================================
 *      ipftpc_do_close
 *===========================================================================
 * Description:
 *      close command.
 *
 * Parameters:
 *      vars    Session variables.
 *      argc    No of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 *      0 if success, 1 if error, -1 if exit required.
 */
IP_STATIC int
ipftpc_do_close(Ipftpc_vars *vars, int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    if (vars->id == 0)
    {
        ipcom_printf("Not connected\n");
        return 1;
    }

    IPFTPC_IF(ipftpc_cmd(vars->id, "QUIT"), / 100 != 2,
              "QUIT error", break);

    /* ipftpc_perror may free it; must verify */
    if (vars->id != 0)
    {
        IPFTPC_IF(ipftpc_close(vars->id), != IPFTPC_ESUCCESS,
                  "Close error", break);
        IPFTPC_FREE(vars->server);
        vars->id = 0;
    }

    return 0;
}


/*
 *===========================================================================
 *      ipftpc_do_delete
 *===========================================================================
 * Description:
 *      delete command.
 *
 * Parameters:
 *      vars    Session variables.
 *      argc    No of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 *      0 if success, 1 if error, -1 if exit required.
 */
IP_STATIC int
ipftpc_do_delete(Ipftpc_vars *vars, int argc, char *argv[])
{
    (void)argc;

    if (vars->id == 0)
    {
        ipcom_printf("Not connected\n");
        return 1;
    }

    IPFTPC_IF(ipftpc_cmd(vars->id, "DELE %s", argv[0]), / 100 != 2,
              "File deletion failed", return 1);

    return 0;
}


/*
 *===========================================================================
 *      ipftpc_do_dir
 *===========================================================================
 * Description:
 *      dir command.
 *
 * Parameters:
 *      vars    Session variables.
 *      argc    No of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 *      0 if success, 1 if error, -1 if exit required.
 */
IP_STATIC int
ipftpc_do_dir(Ipftpc_vars *vars, int argc, char *argv[])
{
    if (vars->id == 0)
    {
        ipcom_printf("Not connected\n");
        return 1;
    }

    IPFTPC_IF(ipftpc_list(vars->id, (argc == 0 ? "LIST" : "LIST %s"),
                          argv[0]), / 100 != 2,
              "Directory list failed", return 1);

    return 0;
}


/*
 *===========================================================================
 *      ipftpc_do_extended
 *===========================================================================
 * Description:
 *      extended command.
 *
 * Parameters:
 *      vars    Session variables.
 *      argc    No of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 *      0 if success, 1 if error, -1 if exit required.
 */
IP_STATIC int
ipftpc_do_extended(Ipftpc_vars *vars, int argc, char *argv[])
{
    if (argc == 0)
    {
        ipcom_printf("Extended mode: %d\n", vars->extended);
        return 0;
    }
    else if (argv[0][1] == '\0')
    {
        if (argv[0][0] == '0' && vars->passive == 3)
        {
            if (vars->id == 0)
            {
                ipcom_printf("Extended passive mode is now disabled\n");
                (void)ipftpc_setpassive(vars, 2);
            }
            else
            {
                ipcom_printf("Server locked in extended passive mode\n");
                return 1;
            }
        }

        if (ipftpc_setextended(vars, argv[0][0] - '0') != 0)
        {
            goto leave;
        }

        /* Update API if connected */
        if (vars->id != 0)
        {
            IPFTPC_IF(ipftpc_setattr(vars->id, &vars->attr),
                      != IPFTPC_ESUCCESS, "Attribute error", return 1);
        }

        return 0;
    }

 leave:
    ipcom_printf("Syntax error, usage: %s %s\n",
                 vars->cmd->name, vars->cmd->parms);
    return 1;
}


/*
 *===========================================================================
 *      ipftpc_do_family
 *===========================================================================
 * Description:
 *      family command.
 *
 * Parameters:
 *      vars    Session variables.
 *      argc    No of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 *      0 if success, 1 if error, -1 if exit required.
 */
IP_STATIC int
ipftpc_do_family(Ipftpc_vars *vars, int argc, char *argv[])
{
    int af;

    if (argc == 0)
    {
        ipcom_printf("Address family: %d\n", vars->family);
        return 0;
    }
    else if (argv[0][1] == '\0')
    {
        switch (argv[0][0])
        {
        case '0':
            af = IP_AF_UNSPEC;
            break;
        case '4':
            af = IP_AF_INET;
            break;
        case '6':
            af = IP_AF_INET6;
            break;
        default:
            goto leave;
        }

        if (vars->id == 0)
        {
            vars->family = argv[0][0] - '0';
            vars->af = af;
        }
        else
        {
            ipcom_printf("Cannot set address family for an open connection\n");
            return 1;
        }

        return 0;
    }

 leave:
    ipcom_printf("Syntax error, usage: %s %s\n",
                 vars->cmd->name, vars->cmd->parms);
    return 1;
}


/*
 *===========================================================================
 *      ipftpc_do_get
 *===========================================================================
 * Description:
 *      get command.
 *
 * Parameters:
 *      vars    Session variables.
 *      argc    No of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 *      0 if success, 1 if error, -1 if exit required.
 */
IP_STATIC int
ipftpc_do_get(Ipftpc_vars *vars, int argc, char *argv[])
{
    if (vars->id == 0)
    {
        ipcom_printf("Not connected\n");
        return 1;
    }

    /* Use remote filename for local if none supplied */
    if (argc == 1)
    {
        argv[1] = argv[0];
    }

    IPFTPC_IF(ipftpc_get(vars->id, argv[1], argv[0]), / 100 != 2,
              "File transfer error", return 1);

    return 0;
}


/*
 *===========================================================================
 *      ipftpc_do_help
 *===========================================================================
 * Description:
 *      help command.
 *
 * Parameters:
 *      vars    Session variables.
 *      argc    No of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 *      0 if success, 1 if error, -1 if exit required.
 */
IP_STATIC int
ipftpc_do_help(Ipftpc_vars *vars, int argc, char *argv[])
{
    const Ipftpc_cmd *cmd;
    int len = (argc == 0 ? 0 : ipcom_strlen(argv[0]));
    int hit = 0;

    (void)vars;

    if (argc == 0)
    {
        ipcom_printf("Available commands:\n");
    }

    /* Loop for help texts */
    for (cmd = ipftpc_cmdlist; cmd->name != IP_NULL; cmd++)
    {
        if (argc == 0)
        {
            /* Help on all commands */
            ipcom_printf(" %s\n", cmd->name);
        }
        else if (ipcom_strncmp(argv[0], cmd->name, len) == 0)
        {
            /* Help on specific command */
            ipcom_printf(" %s %s -- %s\n", cmd->name, cmd->parms, cmd->help);
            hit = 1;
        }
    }

    if (argc != 0 && hit == 0)
    {
        ipcom_printf("Invalid command: %s\n", argv[0]);
    }

    return 0;
}


/*
 *===========================================================================
 *      ipftpc_do_ipcheck
 *===========================================================================
 * Description:
 *      ipcheck command.
 *
 * Parameters:
 *      vars    Session variables.
 *      argc    No of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 *      0 if success, 1 if error, -1 if exit required.
 */
IP_STATIC int
ipftpc_do_ipcheck(Ipftpc_vars *vars, int argc, char *argv[])
{
    if (argc == 0)
    {
        ipcom_printf("IP address check: %d\n", vars->attr.ipcheck);
        return 0;
    }
    else if (argv[0][1] == '\0')
    {
        switch (argv[0][0])
        {
        case '0':
        case '1':
            break;
        default:
            goto leave;
        }

        vars->attr.ipcheck = argv[0][0] - '0';

        /* Update API if connected */
        if (vars->id != 0)
        {
            IPFTPC_IF(ipftpc_setattr(vars->id, &vars->attr),
                      != IPFTPC_ESUCCESS, "Attribute error", return 1);
        }

        return 0;
    }

 leave:
    ipcom_printf("Syntax error, usage: %s %s\n",
                 vars->cmd->name, vars->cmd->parms);
    return 1;
}


/*
 *===========================================================================
 *      ipftpc_do_lbase
 *===========================================================================
 * Description:
 *      lbase command.
 *
 * Parameters:
 *      vars    Session variables.
 *      argc    No of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 *      0 if success, 1 if error, -1 if exit required.
 */
IP_STATIC int
ipftpc_do_lbase(Ipftpc_vars *vars, int argc, char *argv[])
{
    char *stop;
    unsigned int lbase;

    if (argc == 0)
    {
        ipcom_printf("Client data port base: %u\n",
                     (unsigned int)vars->attr.lportbase);
        return 0;
    }

    lbase = ipcom_strtol(argv[0], &stop, 10);
    if (*stop != '\0')
    {
        ipcom_printf("Client port base syntax error: %s\n", argv[0]);
    }
    else if (lbase > 65535)
    {
        ipcom_printf("Client port base %u denied\n", lbase);
    }
    else
    {
        vars->attr.lportbase = (Ip_u16)lbase;

        /* Update API if connected */
        if (vars->id != 0)
        {
            IPFTPC_IF(ipftpc_setattr(vars->id, &vars->attr),
                      != IPFTPC_ESUCCESS, "Attribute error", return 1);
        }

        return 0;
    }

    return 1;
}


/*
 *===========================================================================
 *      ipftpc_do_lcd
 *===========================================================================
 * Description:
 *      lcd command.
 *
 * Parameters:
 *      vars    Session variables.
 *      argc    No of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 *      0 if success, 1 if error, -1 if exit required.
 */
IP_STATIC int
ipftpc_do_lcd(Ipftpc_vars *vars, int argc, char *argv[])
{
    (void)argc;

    if (ipcom_chdir(argv[0]) != 0)
    {
        ipcom_printf("Directory error: %s\n", argv[0]);
        return 1;
    }

    if (vars->verbose)
    {
        return ipftpc_do_lpwd(vars, 0, IP_NULL);
    }

    return 0;
}


/*
 *===========================================================================
 *      ipftpc_do_lpwd
 *===========================================================================
 * Description:
 *      lpwd command.
 *
 * Parameters:
 *      vars    Session variables.
 *      argc    No of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 *      0 if success, 1 if error, -1 if exit required.
 */
IP_STATIC int
ipftpc_do_lpwd(Ipftpc_vars *vars, int argc, char *argv[])
{
    char *cwd;

    (void)vars;
    (void)argc;
    (void)argv;

    if ((cwd = ipcom_malloc(IP_PATH_MAX)) == IP_NULL)
    {
        ipcom_printf("Out of memory\n");
        return 1;
    }

    if (ipcom_getcwd(cwd, IP_PATH_MAX) == IP_NULL)
    {
        ipcom_free(cwd);
        ipcom_printf("Cannot get current directory\n");
        return 1;
    }

    ipcom_printf("Current client directory is \"%s\"\n", cwd);

    ipcom_free(cwd);

    return 0;
}


/*
 *===========================================================================
 *      ipftpc_do_ls
 *===========================================================================
 * Description:
 *      ls command.
 *
 * Parameters:
 *      vars    Session variables.
 *      argc    No of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 *      0 if success, 1 if error, -1 if exit required.
 */
IP_STATIC int
ipftpc_do_ls(Ipftpc_vars *vars, int argc, char *argv[])
{
    if (vars->id == 0)
    {
        ipcom_printf("Not connected\n");
        return 1;
    }

    IPFTPC_IF(ipftpc_list(vars->id, (argc == 0 ? "NLST" : "NLST %s"),
                          argv[0]), / 100 != 2,
              "Directory list error", return 1);

    return 0;
}


/*
 *===========================================================================
 *      ipftpc_do_mkdir
 *===========================================================================
 * Description:
 *      mkdir command.
 *
 * Parameters:
 *      vars    Session variables.
 *      argc    No of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 *      0 if success, 1 if error, -1 if exit required.
 */
IP_STATIC int
ipftpc_do_mkdir(Ipftpc_vars *vars, int argc, char *argv[])
{
    (void)argc;

    if (vars->id == 0)
    {
        ipcom_printf("Not connected\n");
        return 1;
    }

    IPFTPC_IF(ipftpc_cmd(vars->id, "MKD %s", argv[0]), / 100 != 2,
              "Directory creation failed", return 1);

    return 0;
}


/*
 *===========================================================================
 *      ipftpc_do_open
 *===========================================================================
 * Description:
 *      open command.
 *
 * Parameters:
 *      vars    Session variables.
 *      argc    No of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 *      0 if success, 1 if error, -1 if exit required.
 */
IP_STATIC int
ipftpc_do_open(Ipftpc_vars *vars, int argc, char *argv[])
{
    char username[IPFTPC_MAX_USERNAME];
    char password[IPFTPC_MAX_PASSWORD];
    struct Ip_addrinfo hints;
    struct Ip_addrinfo *res = IP_NULL;

    /* Setup */
    ipcom_memset(&hints, 0, sizeof(hints));
    hints.ai_family = vars->af;
    hints.ai_socktype = IP_SOCK_STREAM;

    ipcom_memset(&vars->addr, 0, sizeof(vars->addr));

    /* Resolve host name */
    if (vars->af != IP_AF_INET6 && 
        ipcom_getsockaddrbyaddr(IP_AF_INET, argv[0], &vars->addr.sa) == 0)
    {
        /* IPv4 numeric */
    }
#ifdef IPCOM_USE_INET6
    else if (vars->af != IP_AF_INET && 
             ipcom_getsockaddrbyaddr(IP_AF_INET6, argv[0], &vars->addr.sa) == 0)
    {
        /* IPv6 numeric */
    }
#endif
    else if (ipcom_getaddrinfo(argv[0], "ftp", &hints, &res) == 0 && res != IP_NULL
             && res->ai_addr != IP_NULL)
    {
        /* Generic IPv4 & IPv6 DNS lookup */
        vars->addr.sa.sa_family = (Ip_u8)res->ai_family;
        switch (vars->addr.sa.sa_family)
        {
        case IP_AF_INET:
            vars->addr.sin.sin_addr.s_addr =
                ((struct Ip_sockaddr_in *)res->ai_addr)->sin_addr.s_addr;
            break;
#ifdef IPCOM_USE_INET6
        case IP_AF_INET6:
            ipcom_memcpy(&vars->addr.sin6.sin6_addr,
                         &((struct Ip_sockaddr_in6 *)res->ai_addr)->sin6_addr,
                         sizeof(vars->addr.sin6.sin6_addr));
            /* Must have the scope as well */
            vars->addr.sin6.sin6_scope_id = ((struct Ip_sockaddr_in6 *)res->ai_addr)->sin6_scope_id;
            break;
#endif
        default:
            ipcom_printf("DNS lookup returned invalid family: %d\n",
                         (int)res->ai_family);
            goto leave;
        }
    }
    else
    {
        /* No match */
        ipcom_printf("Unknown host: %s\n", argv[0]);
        goto leave;
    }

    if (res != IP_NULL)
    {
        ipcom_freeaddrinfo(res);
        res = IP_NULL;
    }

    /* Get optional port number */
    if (argc >= 2 && ipftpc_do_port(vars, 1, argv + 1) != 0)
    {
        goto leave;
    }

    /* Set port */
    IPFTPC_PORT(&vars->addr) = (Ip_u16)ip_htons(vars->port);

    /* Connect to server */
    IPFTPC_IF(ipftpc_open(&vars->id, &vars->addr, &vars->attr),
              != IPFTPC_ESUCCESS, "Open failed", goto leave);

    vars->server = ipcom_strdup(argv[0]);
    ipcom_printf("Connected to %s\n", argv[0]);

    /* Get username and password */
#define IPFTPC_USER "User: "
#define IPFTPC_PASSWORD "Password: "
    ipcom_printf(IPFTPC_USER);
    (void)ipcom_fflush(ip_stdout);
    (void)ipcom_getline(IP_INVALID_SOCKET, username, sizeof(username), IP_TRUE, vars->gl,
                        strlen(IPFTPC_USER));
    ipcom_printf(IPFTPC_PASSWORD);
    (void)ipcom_fflush(ip_stdout);
    (void)ipcom_getline(IP_INVALID_SOCKET, password, sizeof(password), IP_FALSE, vars->gl,
                        strlen(IPFTPC_PASSWORD));
    ipcom_printf("\n");
    (void)ipcom_fflush(ip_stdout);

    /* Login */
    vars->conexclude = 331;
    IPFTPC_IF(ipftpc_login(vars->id, username, password), / 100 != 2,
              "Login failed", return 1);
    vars->conexclude = 0;

    /* Immediately enter extended passive mode, if selected */
    if (vars->passive == 3)
    {
        IPFTPC_IF(ipftpc_cmd(vars->id, "EPSV ALL"), / 100 != 2,
                  "Extended passive mode failed", goto noepsv);
        return 0;

noepsv:
        if (vars->id != 0)
        {
            /* Fallback to normal passive */
            (void)ipftpc_setpassive(vars, 1);
            (void)ipftpc_setextended(vars, 0);
            IPFTPC_IF(ipftpc_setattr(vars->id, &vars->attr),
                      != IPFTPC_ESUCCESS, "Attribute error", return 1);
        }
    }

    return 0;

leave:
    if (vars->id != 0)
    {
        (void)ipftpc_close(vars->id);
    }

    if (res != IP_NULL)
        ipcom_freeaddrinfo(res);

    return 1;
}


/*
 *===========================================================================
 *      ipftpc_do_passive
 *===========================================================================
 * Description:
 *      passive command.
 *
 * Parameters:
 *      vars    Session variables.
 *      argc    No of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 *      0 if success, 1 if error, -1 if exit required.
 */
IP_STATIC int
ipftpc_do_passive(Ipftpc_vars *vars, int argc, char *argv[])
{
    if (argc == 0)
    {
        ipcom_printf("Passive mode: %d\n", vars->passive);
        return 0;
    }
    else if (argv[0][1] == '\0')
    {
        if (vars->id != 0 && vars->passive == 3)
        {
            ipcom_printf("Server locked in extended passive mode\n");
            return argv[0][0] != '3';
        }

        /* Lock server in extended passive mode? */
        if (argv[0][0] == '3' && vars->id != 0)
        {
            IPFTPC_IF(ipftpc_cmd(vars->id, "EPSV ALL"), / 100 != 2,
                      "Extended passive mode failed, using normal passive",
                      argv[0][0] = '2'; goto failed);

            /* Server accepted EPSV ALL, set extended mode officially */
            (void)ipftpc_setextended(vars, 2);
        }

    failed:
        if (ipftpc_setpassive(vars, argv[0][0] - '0') != 0)
        {
            goto leave;
        }

        /* Update API if connected */
        if (vars->id != 0)
        {
            IPFTPC_IF(ipftpc_setattr(vars->id, &vars->attr),
                      != IPFTPC_ESUCCESS, "Attribute error", return 1);
        }

        return 0;
    }

 leave:
    ipcom_printf("Syntax error, usage: %s %s\n",
                 vars->cmd->name, vars->cmd->parms);
    return 1;
}


/*
 *===========================================================================
 *      ipftpc_do_port
 *===========================================================================
 * Description:
 *      port command.
 *
 * Parameters:
 *      vars    Session variables.
 *      argc    No of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 *      0 if success, 1 if error, -1 if exit required.
 */
IP_STATIC int
ipftpc_do_port(Ipftpc_vars *vars, int argc, char *argv[])
{
    char *stop;
    unsigned int port;

    if (argc == 0)
    {
        ipcom_printf("Use server port: %u\n", (unsigned int)vars->port);
        return 0;
    }

    port = ipcom_strtol(argv[0], &stop, 10);
    if (*stop != '\0')
    {
        ipcom_printf("Port number syntax error: %s\n", argv[0]);
    }
    else if (port < 1 || port > 65535)
    {
        ipcom_printf("Port number %u denied\n", port);
    }
    else if (vars->id != 0)
    {
        ipcom_printf("Cannot set port number on an open connection\n");
    }
    else
    {
        vars->port = port;
        return 0;
    }

    return 1;
}


/*
 *===========================================================================
 *      ipftpc_do_put
 *===========================================================================
 * Description:
 *      put command.
 *
 * Parameters:
 *      vars    Session variables.
 *      argc    No of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 *      0 if success, 1 if error, -1 if exit required.
 */
IP_STATIC int
ipftpc_do_put(Ipftpc_vars *vars, int argc, char *argv[])
{
    if (vars->id == 0)
    {
        ipcom_printf("Not connected\n");
        return 1;
    }

    /* Use local filename for remote if none supplied */
    if (argc == 1)
    {
        argv[1] = argv[0];
    }

    IPFTPC_IF(ipftpc_put(vars->id, argv[0], argv[1], 0), / 100 != 2,
              "File transfer error", return 1);

    return 0;
}


/*
 *===========================================================================
 *      ipftpc_do_pwd
 *===========================================================================
 * Description:
 *      pwd command.
 *
 * Parameters:
 *      vars    Session variables.
 *      argc    No of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 *      0 if success, 1 if error, -1 if exit required.
 */
IP_STATIC int
ipftpc_do_pwd(Ipftpc_vars *vars, int argc, char *argv[])
{
    int old;

    (void)argc;
    (void)argv;

    if (vars->id == 0)
    {
        ipcom_printf("Not connected\n");
        return 1;
    }

    old = ipftpc_setverbose(vars, vars->attr.verbose | 1);

    IPFTPC_IF(ipftpc_cmd(vars->id, "PWD"), / 100 != 2,
              "Directory display failed", return 1);

    (void)ipftpc_setverbose(vars, old);

    return 0;
}


/*
 *===========================================================================
 *      ipftpc_do_quit
 *===========================================================================
 * Description:
 *      quit command.
 *
 * Parameters:
 *      vars    Session variables.
 *      argc    No of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 *      0 if success, 1 if error, -1 if exit required.
 */
IP_STATIC int
ipftpc_do_quit(Ipftpc_vars *vars, int argc, char *argv[])
{
    if (vars->id != 0)
    {
        (void)ipftpc_do_close(vars, argc, argv);
    }

    return -1;
}


/*
 *===========================================================================
 *      ipftpc_do_quote
 *===========================================================================
 * Description:
 *      quote command.
 *
 * Parameters:
 *      vars    Session variables.
 *      argc    No of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 *      0 if success, 1 if error, -1 if exit required.
 */
IP_STATIC int
ipftpc_do_quote(Ipftpc_vars *vars, int argc, char *argv[])
{
    if (vars->id == 0)
    {
        ipcom_printf("Not connected\n");
        return 1;
    }

    switch (argc)
    {
    case 1:
        (void)ipftpc_cmd(vars->id, "%s",
                         argv[0]);
        break;
    case 2:
        (void)ipftpc_cmd(vars->id, "%s %s",
                         argv[0], argv[1]);
        break;
    case 3:
        (void)ipftpc_cmd(vars->id, "%s %s %s",
                         argv[0], argv[1], argv[2]);
        break;
    case 4:
        (void)ipftpc_cmd(vars->id, "%s %s %s %s",
                         argv[0], argv[1], argv[2], argv[3]);
        break;
    case 5:
        (void)ipftpc_cmd(vars->id, "%s %s %s %s %s",
                         argv[0], argv[1], argv[2], argv[3], argv[4]);
        break;
    default:
        ipcom_printf("Only 5 arguments allowed\n");
        return 1;
    }

    return 0;
}


/*
 *===========================================================================
 *      ipftpc_do_rbase
 *===========================================================================
 * Description:
 *      rbase command.
 *
 * Parameters:
 *      vars    Session variables.
 *      argc    No of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 *      0 if success, 1 if error, -1 if exit required.
 */
IP_STATIC int
ipftpc_do_rbase(Ipftpc_vars *vars, int argc, char *argv[])
{
    char *stop;
    unsigned int rbase;

    if (argc == 0)
    {
        ipcom_printf("Server data port base: %u\n",
                     (unsigned int)vars->attr.pportbase);
        return 0;
    }

    rbase = ipcom_strtol(argv[0], &stop, 10);
    if (*stop != '\0')
    {
        ipcom_printf("Server port base syntax error: %s\n", argv[0]);
    }
    else if (rbase < 1 || rbase > 65535)
    {
        ipcom_printf("Server port base %u denied\n", rbase);
    }
    else
    {
        vars->attr.pportbase = (Ip_u16)rbase;

        /* Update API if connected */
        if (vars->id != 0)
        {
            IPFTPC_IF(ipftpc_setattr(vars->id, &vars->attr),
                      != IPFTPC_ESUCCESS, "Attribute error", return 1);
        }

        return 0;
    }

    return 1;
}


/*
 *===========================================================================
 *      Ipftp_do_rename
 *===========================================================================
 * Description:
 *      rename command.
 *
 * Parameters:
 *      vars    Session variables.
 *      argc    No of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 *      0 if success, 1 if error, -1 if exit required.
 */
IP_STATIC int
ipftpc_do_rename(Ipftpc_vars *vars, int argc, char *argv[])
{
    (void)argc;

    if (vars->id == 0)
    {
        ipcom_printf("Not connected\n");
        return 1;
    }

    vars->conexclude = 350;
    IPFTPC_IF(ipftpc_cmd(vars->id, "RNFR %s", argv[0]), / 100 != 3,
              "Rename from failed", return 1);
    vars->conexclude = 0;
    IPFTPC_IF(ipftpc_cmd(vars->id, "RNTO %s", argv[1]), / 100 != 2,
              "Rename to failed", return 1);

    return 0;
}


/*
 *===========================================================================
 *      ipftpc_do_rhelp
 *===========================================================================
 * Description:
 *      rhelp command.
 *
 * Parameters:
 *      vars    Session variables.
 *      argc    No of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 *      0 if success, 1 if error, -1 if exit required.
 */
IP_STATIC int
ipftpc_do_rhelp(Ipftpc_vars *vars, int argc, char *argv[])
{
    int old;

    if (vars->id == 0)
    {
        ipcom_printf("Not connected\n");
        return 1;
    }

    old = ipftpc_setverbose(vars, vars->attr.verbose | 1);

    IPFTPC_IF(ipftpc_cmd(vars->id, (argc == 0 ? "HELP" : "HELP %s"),
                         argv[0]), / 100 != 2, "Help failed", return 1);

    (void)ipftpc_setverbose(vars, old);

    return 0;
}


/*
 *===========================================================================
 *      ipftpc_do_rmdir
 *===========================================================================
 * Description:
 *      rmdir command.
 *
 * Parameters:
 *      vars    Session variables.
 *      argc    No of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 *      0 if success, 1 if error, -1 if exit required.
 */
IP_STATIC int
ipftpc_do_rmdir(Ipftpc_vars *vars, int argc, char *argv[])
{
    (void)argc;

    if (vars->id == 0)
    {
        ipcom_printf("Not connected\n");
        return 1;
    }

    IPFTPC_IF(ipftpc_cmd(vars->id, "RMD %s", argv[0]), / 100 != 2,
              "Directory deletion failed", return 1);

    return 0;
}


/*
 *===========================================================================
 *      ipftpc_do_rstatus
 *===========================================================================
 * Description:
 *      rstatus command.
 *
 * Parameters:
 *      vars    Session variables.
 *      argc    No of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 *      0 if success, 1 if error, -1 if exit required.
 */
IP_STATIC int
ipftpc_do_rstatus(Ipftpc_vars *vars, int argc, char *argv[])
{
    int old;

    if (vars->id == 0)
    {
        ipcom_printf("Not connected\n");
        return 1;
    }

    old = ipftpc_setverbose(vars, vars->attr.verbose | 1);

    IPFTPC_IF(ipftpc_cmd(vars->id, (argc == 0 ? "STAT" : "STAT %s"),
                         argv[0]), / 100 != 2, "Status failed", return 1);

    (void)ipftpc_setverbose(vars, old);

    return 0;
}


/*
 *===========================================================================
 *      ipftpc_do_status
 *===========================================================================
 * Description:
 *      status command.
 *
 * Parameters:
 *      vars    Session variables.
 *      argc    No of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 *      0 if success, 1 if error, -1 if exit required.
 */
IP_STATIC int
ipftpc_do_status(Ipftpc_vars *vars, int argc, char *argv[])
{
    static const char *ipftpc_strfamily[] =
        {
            "IPv4 and IPv6", "1", "2", "3", "IPv4", "5", "IPv6"
        };
    static const char *ipftpc_strpassive[] =
        {
            "off", "on (fallback to active mode)", "on", "locked"
        };
    static const char *ipftpc_strextended[] =
        {
            "off", "on (fallback to non-extended mode)", "on"
        };
    static const char *ipftpc_stronoff[] =
        {
            "off", "on"
        };

    (void)argc;
    (void)argv;

    ipcom_printf("Interpeak FTP client, version " IPFTPC_VERSIONSTR "\n");

    if (vars->id == 0)
    {
        ipcom_printf("Not connected\n");
    }
    else
    {
        ipcom_printf("Connected to %s\n", vars->server);
    }

    ipcom_printf("Allowed address families: %s\n",
                 ipftpc_strfamily[vars->family]);
    ipcom_printf("Connect to server port %u\n",
                 (unsigned int)vars->port);
    ipcom_printf("Passive mode: %s\n",
                 ipftpc_strpassive[vars->passive]);
    ipcom_printf("Extended mode: %s\n",
                 ipftpc_strextended[vars->extended]);
    ipcom_printf("IP address match for control and data: %s\n",
                 ipftpc_stronoff[vars->attr.ipcheck]);
    if (vars->attr.lportbase == 0)
    {
        ipcom_printf("Client OS selects data port\n");
    }
    else
    {
        ipcom_printf("Use client data ports above %u\n",
                     (unsigned int)vars->attr.lportbase);
    }
    ipcom_printf("Require server data ports above %u\n",
                 (unsigned int)vars->attr.pportbase);
    ipcom_printf("Verbose level: %d\n",
                 (int)vars->verbose);
    ipcom_printf("Command-line editing: %s\n",
                 ipftpc_stronoff[vars->doedit]);

    return 0;
}


/*
 *===========================================================================
 *      ipftpc_do_system
 *===========================================================================
 * Description:
 *      system command.
 *
 * Parameters:
 *      vars    Session variables.
 *      argc    No of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 *      0 if success, 1 if error, -1 if exit required.
 */
IP_STATIC int
ipftpc_do_system(Ipftpc_vars *vars, int argc, char *argv[])
{
    int old;

    (void)argc;
    (void)argv;

    if (vars->id == 0)
    {
        ipcom_printf("Not connected\n");
        return 1;
    }

    old = ipftpc_setverbose(vars, vars->attr.verbose | 1);

    IPFTPC_IF(ipftpc_cmd(vars->id, "SYST"), / 100 != 2,
              "System failed", return 1);

    (void)ipftpc_setverbose(vars, old);

    return 0;
}


/*
 *===========================================================================
 *      ipftpc_do_verbose
 *===========================================================================
 * Description:
 *      verbose command.
 *
 * Parameters:
 *      vars    Session variables.
 *      argc    No of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 *      0 if success, 1 if error, -1 if exit required.
 */
IP_STATIC int
ipftpc_do_verbose(Ipftpc_vars *vars, int argc, char *argv[])
{
    if (argc == 0)
    {
        ipcom_printf("Verbose level: %d\n", vars->verbose);
        return 0;
    }
    else if (argv[0][1] == '\0')
    {
        switch (argv[0][0])
        {
        case '0':
        case '1':
            vars->attr.verbose = 0;
            break;
        case '2':
            vars->attr.verbose = 1;
            break;
        case '3':
            vars->attr.verbose = 3;
            break;
        default:
            goto leave;
        }

        vars->verbose = argv[0][0] - '0';

        /* Update API if connected */
        if (vars->id != 0)
        {
            IPFTPC_IF(ipftpc_setattr(vars->id, &vars->attr),
                      != IPFTPC_ESUCCESS, "Attribute error", return 1);
        }

        return 0;
    }

leave:
    ipcom_printf("Syntax error, usage: %s %s\n",
                 vars->cmd->name, vars->cmd->parms);
    return 1;
}


/*
 *===========================================================================
 *      ipftpc_getline
 *===========================================================================
 * Description:
 *      Get command line.
 *
 * Parameters:
 *
 * Returns:
 */
IP_STATIC int
ipftpc_getline(Ipftpc_vars *vars, const char *prompt, char *buf, int size)
{
    if (vars->doedit)
    {
        /* FIXME--Insert edit routine here */
    }

    ipcom_printf(prompt);
    (void)ipcom_fflush(ip_stdout);

    return ipcom_getline(IP_INVALID_SOCKET, buf, size, IP_TRUE, vars->gl, ipcom_strlen(prompt));
}


/*
 *===========================================================================
 *      ipftpc_cmdloop
 *===========================================================================
 * Description:
 *      Loop and get commands.
 *
 * Parameters:
 *
 * Returns:
 */
IP_STATIC void
ipftpc_cmdloop(Ipftpc_vars *vars)
{
    char *argv[IPFTPC_MAX_ARGS];
    const Ipftpc_cmd *cmd;
    char *stop;
    int i;

    /* Get command line */
    while (ipftpc_getline(vars, "ftp> ", vars->cmdbuf,
                          sizeof(vars->cmdbuf) - 1) >= 0)
    {
        char *cp = vars->cmdbuf;
        char *a0;
        int toplen = 0;
        int topambig = 0;

        vars->cmd = IP_NULL;

        /* Ignore initial whitespace */
        cp += ipcom_strspn(cp, IPFTPC_WHITESPACE);
        if (*cp == '\0')
        {
            continue;
        }

        /* Split into arguments */
        for (i = 0; *cp != '\0' && i < IPFTPC_MAX_ARGS; i++)
        {
            argv[i] = cp;
            cp += ipcom_strcspn(cp, IPFTPC_WHITESPACE);
            stop = cp;
            cp += ipcom_strspn(cp, IPFTPC_WHITESPACE);
            *stop = '\0';
        }

        a0 = argv[0];

        /* Scan through command list */
        for (cmd = ipftpc_cmdlist; cmd->name != IP_NULL; cmd++)
        {
            int j;

            /* Determine how long match */
            for (j = 0; a0[j] != '\0' && a0[j] == cmd->name[j]; j++)
            {
            }

            if (a0[j] != '\0')
            {
                /* Does not match */
            }
            else if (cmd->name[j] == '\0')
            {
                /* Exact match */
                vars->cmd = cmd;
                topambig = 0;
                break;
            }
            else if (j > toplen)
            {
                /* Best so far */
                toplen = j;
                vars->cmd = cmd;
                topambig = 0;
            }
            else if (j == toplen)
            {
                /* Ambiguous */
                topambig = 1;
            }
        }

        /* Ignore first argument (command name) */
        i--;

        /* Check result */
        if (vars->cmd == IP_NULL)
        {
            ipcom_printf("Invalid command: %s\n", a0);
        }
        else if (topambig)
        {
            ipcom_printf("Ambiguous command: %s\n", a0);
        }
        else if (i < vars->cmd->min || i > vars->cmd->max)
        {
            ipcom_printf("Syntax error, usage: %s %s\n",
                         vars->cmd->name, vars->cmd->parms);
        }
        else
        {
            /* Do command */
            if (vars->cmd->func(vars, i, argv + 1) < 0)
            {
                break;
            }
        }
    }
}


/*
 ****************************************************************************
 * 10                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *      ipftpc_cmd_main
 *===========================================================================
 * Description:
 *      FTP command handler.
 *
 * Parameters:
 * Returns:
 */
IP_PUBLIC int
ipftpc_cmd_main(int argc, char *argv[])
{
    char *zero = "0";
    Ipftpc_vars *vars;
    int err = 0;
    Ipcom_getopt o;
    int opt;

    /* Get session vars */
    if ((vars = ipcom_malloc(sizeof(*vars))) == IP_NULL)
    {
        ipcom_printf("Out of memory\n");
        err = 1;
        goto leave;
    }

    /* Initialize */
    ipcom_memset(vars, 0, sizeof(*vars));
    vars->attr.ipcheck = 1;
    vars->attr.verbose = 1;
    vars->attr.pportbase = 1023;
    vars->attr.lportbase = 0;
    vars->attr.conout = ipftpc_conout;
    vars->attr.ctrlin = ipftpc_ctrlin;
    vars->attr.ctrlout = ipftpc_ctrlout;
    vars->attr.data = vars;
    vars->family = IP_AF_UNSPEC;
    vars->af = 0;
    vars->port = 21;
    vars->verbose = 2;
    vars->doedit = 1;
    (void)ipftpc_setpassive(vars, 3);
    (void)ipftpc_setextended(vars, 1);
    vars->gl = ipcom_getline_init(10);
    if (vars->gl == IP_NULL)
    {
        goto leave;
    }

    ipcom_getopt_clear_r(&o);

    /* Get options */
    while ((opt = ipcom_getopt_r(argc, argv, "P:a:p:x:il:r:ev:h", &o)) != -1)
    {
        switch (opt)
        {
        case 'P':
            ipftpc_flag(vars, ipftpc_do_port, opt);
            err |= ipftpc_do_port(vars, 1, &o.optarg);
            break;
        case 'a':
            ipftpc_flag(vars, ipftpc_do_family, opt);
            err |= ipftpc_do_family(vars, 1, &o.optarg);
            break;
        case 'p':
            ipftpc_flag(vars, ipftpc_do_passive, opt);
            err |= ipftpc_do_passive(vars, 1, &o.optarg);
            break;
        case 'x':
            ipftpc_flag(vars, ipftpc_do_extended, opt);
            err |= ipftpc_do_extended(vars, 1, &o.optarg);
            break;
        case 'l':
            ipftpc_flag(vars, ipftpc_do_lbase, opt);
            err |= ipftpc_do_lbase(vars, 1, &o.optarg);
            break;
        case 'r':
            ipftpc_flag(vars, ipftpc_do_rbase, opt);
            err |= ipftpc_do_rbase(vars, 1, &o.optarg);
            break;
        case 'v':
            ipftpc_flag(vars, ipftpc_do_verbose, opt);
            err |= ipftpc_do_verbose(vars, 1, &o.optarg);
            break;
        case 'i':
            ipftpc_flag(vars, ipftpc_do_ipcheck, opt);
            err |= ipftpc_do_ipcheck(vars, 1, &zero);
            break;
        case 'e':
            vars->doedit = 0;
            break;
        case 'h':
            ipcom_printf("Usage: ipftp [options] [host]\n");
            ipcom_printf("Options:\n");
            ipcom_printf(" -P <port>   Set server port\n");
            ipcom_printf(" -a <family> Set address family\n");
            ipcom_printf(" -p <pmode>  Set passive mode\n");
            ipcom_printf(" -x <xmode>  Set extended mode\n");
            ipcom_printf(" -l <base>   Use client ports from base\n");
            ipcom_printf(" -r <base>   Allow server ports from base\n");
            ipcom_printf(" -v <vmode>  Set verbose mode\n");
            ipcom_printf(" -i Disable IP address match for control and data\n");
            ipcom_printf(" -e Disable command-line editing\n");
            goto leave;
        default:
            err = 1;
            break;
        }
    }

    /* Exit if error in flags */
    if (err)
    {
        goto leave;
    }

    /* Open host if address supplied */
    if (argc > o.optind)
    {
        (void)ipftpc_do_open(vars, 1, argv + o.optind);
    }

    /* Command loop */
    ipftpc_cmdloop(vars);
    err = 0;

    /* Dispose */
 leave:
    if (vars != IP_NULL)
    {
        if (vars->gl)
        {
            ipcom_getline_exit(vars->gl);
        }
        ipcom_free(vars);
    }

    return err;
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
