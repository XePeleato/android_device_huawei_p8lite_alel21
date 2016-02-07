/*
 ******************************************************************************
 *                     HEADER FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipftpc_cmd_proto.h,v $ $Revision: 1.7 $
 *   $Source: /home/interpeak/CVSRoot/ipappl/include/ipftpc_cmd_proto.h,v $
 *   $Author: lob $ $Date: 2004-10-01 10:28:33 $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Ulf Olofsson <ulf@interpeak.se>
 ******************************************************************************
 */
#ifndef IPFTPC_CMD_PROTO_H
#define IPFTPC_CMD_PROTO_H

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

/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#ifdef __cplusplus
extern "C" {
#endif

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
 * 6                    LOCAL FUNCTIONS
 ****************************************************************************
 */

#ifdef IPFTPC_CMD_C

/*
 *===========================================================================
 *      ipftpc_conout
 *===========================================================================
 * Description:
 *      Print command output console.
 *
 * Parameters
 *      vars    Session variables.
 *      msg     Error message.
 *      eno     Error number.
 *
 * Returns:
 */
IP_STATIC int
ipftpc_conout(Ipftpc_attr *attr, const char *buf, int len);


/*
 *===========================================================================
 *      ipftpc_ctrlin
 *===========================================================================
 * Description:
 *      Print control input to console.
 *
 * Parameters
 *      vars    Session variables.
 *      msg     Error message.
 *      eno     Error number.
 *
 * Returns:
 */
IP_STATIC void
ipftpc_ctrlin(Ipftpc_attr *attr, const char *buf, int len);


/*
 *===========================================================================
 *      ipftpc_ctrlout
 *===========================================================================
 * Description:
 *      Print control output to console.
 *
 * Parameters
 *      vars    Session variables.
 *      msg     Error message.
 *      eno     Error number.
 *
 * Returns:
 */
IP_STATIC void
ipftpc_ctrlout(Ipftpc_attr *attr, const char *buf, int len);


/*
 *===========================================================================
 *      ipftpc_perror
 *===========================================================================
 * Description:
 *      Print error message.
 *
 * Parameters
 *      vars    Session variables.
 *      msg     Error message.
 *      eno     Error number.
 *
 * Returns:
 */
IP_STATIC void
ipftpc_perror(Ipftpc_vars *vars, const char *msg, int eno);


/*
 *===========================================================================
 *      ipftpc_setextended
 *===========================================================================
 * Description:
 *      Set extended mode.
 *
 * Parameters
 *      extended New value.
 *
 * Returns:
 */
IP_STATIC int
ipftpc_setextended(Ipftpc_vars *vars, int extended);


/*
 *===========================================================================
 *      ipftpc_setpassive
 *===========================================================================
 * Description:
 *      Set passive mode.
 *
 * Parameters
 *      passive New value.
 *
 * Returns:
 */
IP_STATIC int
ipftpc_setpassive(Ipftpc_vars *vars, int passive);


/*
 *===========================================================================
 *      ipftpc_setverbose
 *===========================================================================
 * Description:
 *      Set verbose mode.
 *
 * Parameters
 *      verbose New value.
 *
 * Returns:
 */
IP_STATIC int
ipftpc_setverbose(Ipftpc_vars *vars, int verbose);


/*
 *===========================================================================
 *      ipftpc_flag
 *===========================================================================
 * Description:
 *      Prepare for handling flag in command routine.
 *
 * Parameters
 *      vars    Session variables.
 *      func    Command routine.
 *      flag    Flag.
 *
 * Returns:
 */
IP_STATIC void
ipftpc_flag(Ipftpc_vars *vars,
            int (*func)(struct Ipftpc_vars_struct *vars, int argc, char *argv[]),
            int flag);


/*
 *===========================================================================
 *      ipftpc_do_append
 *===========================================================================
 * Description:
 *      append command.
 *
 * Parameters
 *      vars    Session variables.
 *      argc    No of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 *      0 if success, 1 if error, -1 if exit required.
 */
IP_STATIC int
ipftpc_do_append(Ipftpc_vars *vars, int argc, char *argv[]);


/*
 *===========================================================================
 *      ipftpc_do_ascii
 *===========================================================================
 * Description:
 *      ascii command.
 *
 * Parameters
 *      vars    Session variables.
 *      argc    No of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 *      0 if success, 1 if error, -1 if exit required.
 */
IP_STATIC int
ipftpc_do_ascii(Ipftpc_vars *vars, int argc, char *argv[]);


/*
 *===========================================================================
 *      ipftpc_do_binary
 *===========================================================================
 * Description:
 *      binary command.
 *
 * Parameters
 *      vars    Session variables.
 *      argc    No of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 *      0 if success, 1 if error, -1 if exit required.
 */
IP_STATIC int
ipftpc_do_binary(Ipftpc_vars *vars, int argc, char *argv[]);


/*
 *===========================================================================
 *      ipftpc_do_cd
 *===========================================================================
 * Description:
 *      cd command.
 *
 * Parameters
 *      vars    Session variables.
 *      argc    No of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 *      0 if success, 1 if error, -1 if exit required.
 */
IP_STATIC int
ipftpc_do_cd(Ipftpc_vars *vars, int argc, char *argv[]);


/*
 *===========================================================================
 *      ipftpc_do_cdup
 *===========================================================================
 * Description:
 *      cdup command.
 *
 * Parameters
 *      vars    Session variables.
 *      argc    No of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 *      0 if success, 1 if error, -1 if exit required.
 */
IP_STATIC int
ipftpc_do_cdup(Ipftpc_vars *vars, int argc, char *argv[]);


/*
 *===========================================================================
 *      ipftpc_do_close
 *===========================================================================
 * Description:
 *      close command.
 *
 * Parameters
 *      vars    Session variables.
 *      argc    No of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 *      0 if success, 1 if error, -1 if exit required.
 */
IP_STATIC int
ipftpc_do_close(Ipftpc_vars *vars, int argc, char *argv[]);


/*
 *===========================================================================
 *      ipftpc_do_delete
 *===========================================================================
 * Description:
 *      delete command.
 *
 * Parameters
 *      vars    Session variables.
 *      argc    No of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 *      0 if success, 1 if error, -1 if exit required.
 */
IP_STATIC int
ipftpc_do_delete(Ipftpc_vars *vars, int argc, char *argv[]);


/*
 *===========================================================================
 *      ipftpc_do_dir
 *===========================================================================
 * Description:
 *      dir command.
 *
 * Parameters
 *      vars    Session variables.
 *      argc    No of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 *      0 if success, 1 if error, -1 if exit required.
 */
IP_STATIC int
ipftpc_do_dir(Ipftpc_vars *vars, int argc, char *argv[]);


/*
 *===========================================================================
 *      ipftpc_do_extended
 *===========================================================================
 * Description:
 *      extended command.
 *
 * Parameters
 *      vars    Session variables.
 *      argc    No of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 *      0 if success, 1 if error, -1 if exit required.
 */
IP_STATIC int
ipftpc_do_extended(Ipftpc_vars *vars, int argc, char *argv[]);


/*
 *===========================================================================
 *      ipftpc_do_family
 *===========================================================================
 * Description:
 *      family command.
 *
 * Parameters
 *      vars    Session variables.
 *      argc    No of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 *      0 if success, 1 if error, -1 if exit required.
 */
IP_STATIC int
ipftpc_do_family(Ipftpc_vars *vars, int argc, char *argv[]);


/*
 *===========================================================================
 *      ipftpc_do_get
 *===========================================================================
 * Description:
 *      get command.
 *
 * Parameters
 *      vars    Session variables.
 *      argc    No of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 *      0 if success, 1 if error, -1 if exit required.
 */
IP_STATIC int
ipftpc_do_get(Ipftpc_vars *vars, int argc, char *argv[]);


/*
 *===========================================================================
 *      ipftpc_do_help
 *===========================================================================
 * Description:
 *      help command.
 *
 * Parameters
 *      vars    Session variables.
 *      argc    No of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 *      0 if success, 1 if error, -1 if exit required.
 */
IP_STATIC int
ipftpc_do_help(Ipftpc_vars *vars, int argc, char *argv[]);


/*
 *===========================================================================
 *      ipftpc_do_ipcheck
 *===========================================================================
 * Description:
 *      ipcheck command.
 *
 * Parameters
 *      vars    Session variables.
 *      argc    No of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 *      0 if success, 1 if error, -1 if exit required.
 */
IP_STATIC int
ipftpc_do_ipcheck(Ipftpc_vars *vars, int argc, char *argv[]);


/*
 *===========================================================================
 *      ipftpc_do_lbase
 *===========================================================================
 * Description:
 *      lbase command.
 *
 * Parameters
 *      vars    Session variables.
 *      argc    No of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 *      0 if success, 1 if error, -1 if exit required.
 */
IP_STATIC int
ipftpc_do_lbase(Ipftpc_vars *vars, int argc, char *argv[]);


/*
 *===========================================================================
 *      ipftpc_do_lcd
 *===========================================================================
 * Description:
 *      lcd command.
 *
 * Parameters
 *      vars    Session variables.
 *      argc    No of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 *      0 if success, 1 if error, -1 if exit required.
 */
IP_STATIC int
ipftpc_do_lcd(Ipftpc_vars *vars, int argc, char *argv[]);


/*
 *===========================================================================
 *      ipftpc_do_lpwd
 *===========================================================================
 * Description:
 *      lpwd command.
 *
 * Parameters
 *      vars    Session variables.
 *      argc    No of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 *      0 if success, 1 if error, -1 if exit required.
 */
IP_STATIC int
ipftpc_do_lpwd(Ipftpc_vars *vars, int argc, char *argv[]);


/*
 *===========================================================================
 *      ipftpc_do_ls
 *===========================================================================
 * Description:
 *      ls command.
 *
 * Parameters
 *      vars    Session variables.
 *      argc    No of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 *      0 if success, 1 if error, -1 if exit required.
 */
IP_STATIC int
ipftpc_do_ls(Ipftpc_vars *vars, int argc, char *argv[]);


/*
 *===========================================================================
 *      ipftpc_do_mkdir
 *===========================================================================
 * Description:
 *      mkdir command.
 *
 * Parameters
 *      vars    Session variables.
 *      argc    No of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 *      0 if success, 1 if error, -1 if exit required.
 */
IP_STATIC int
ipftpc_do_mkdir(Ipftpc_vars *vars, int argc, char *argv[]);


/*
 *===========================================================================
 *      ipftpc_do_open
 *===========================================================================
 * Description:
 *      open command.
 *
 * Parameters
 *      vars    Session variables.
 *      argc    No of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 *      0 if success, 1 if error, -1 if exit required.
 */
IP_STATIC int
ipftpc_do_open(Ipftpc_vars *vars, int argc, char *argv[]);


/*
 *===========================================================================
 *      ipftpc_do_passive
 *===========================================================================
 * Description:
 *      passive command.
 *
 * Parameters
 *      vars    Session variables.
 *      argc    No of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 *      0 if success, 1 if error, -1 if exit required.
 */
IP_STATIC int
ipftpc_do_passive(Ipftpc_vars *vars, int argc, char *argv[]);


/*
 *===========================================================================
 *      ipftpc_do_port
 *===========================================================================
 * Description:
 *      port command.
 *
 * Parameters
 *      vars    Session variables.
 *      argc    No of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 *      0 if success, 1 if error, -1 if exit required.
 */
IP_STATIC int
ipftpc_do_port(Ipftpc_vars *vars, int argc, char *argv[]);


/*
 *===========================================================================
 *      ipftpc_do_put
 *===========================================================================
 * Description:
 *      put command.
 *
 * Parameters
 *      vars    Session variables.
 *      argc    No of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 *      0 if success, 1 if error, -1 if exit required.
 */
IP_STATIC int
ipftpc_do_put(Ipftpc_vars *vars, int argc, char *argv[]);


/*
 *===========================================================================
 *      ipftpc_do_pwd
 *===========================================================================
 * Description:
 *      pwd command.
 *
 * Parameters
 *      vars    Session variables.
 *      argc    No of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 *      0 if success, 1 if error, -1 if exit required.
 */
IP_STATIC int
ipftpc_do_pwd(Ipftpc_vars *vars, int argc, char *argv[]);


/*
 *===========================================================================
 *      ipftpc_do_quit
 *===========================================================================
 * Description:
 *      quit command.
 *
 * Parameters
 *      vars    Session variables.
 *      argc    No of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 *      0 if success, 1 if error, -1 if exit required.
 */
IP_STATIC int
ipftpc_do_quit(Ipftpc_vars *vars, int argc, char *argv[]);


/*
 *===========================================================================
 *      ipftpc_do_quote
 *===========================================================================
 * Description:
 *      quote command.
 *
 * Parameters
 *      vars    Session variables.
 *      argc    No of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 *      0 if success, 1 if error, -1 if exit required.
 */
IP_STATIC int
ipftpc_do_quote(Ipftpc_vars *vars, int argc, char *argv[]);


/*
 *===========================================================================
 *      ipftpc_do_rbase
 *===========================================================================
 * Description:
 *      rbase command.
 *
 * Parameters
 *      vars    Session variables.
 *      argc    No of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 *      0 if success, 1 if error, -1 if exit required.
 */
IP_STATIC int
ipftpc_do_rbase(Ipftpc_vars *vars, int argc, char *argv[]);


/*
 *===========================================================================
 *      ipftpc_do_rename
 *===========================================================================
 * Description:
 *      rename command.
 *
 * Parameters
 *      vars    Session variables.
 *      argc    No of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 *      0 if success, 1 if error, -1 if exit required.
 */
IP_STATIC int
ipftpc_do_rename(Ipftpc_vars *vars, int argc, char *argv[]);


/*
 *===========================================================================
 *      ipftpc_do_rhelp
 *===========================================================================
 * Description:
 *      rhelp command.
 *
 * Parameters
 *      vars    Session variables.
 *      argc    No of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 *      0 if success, 1 if error, -1 if exit required.
 */
IP_STATIC int
ipftpc_do_rhelp(Ipftpc_vars *vars, int argc, char *argv[]);

/*
 *===========================================================================
 *      ipftpc_do_rmdir
 *===========================================================================
 * Description:
 *      rmdir command.
 *
 * Parameters
 *      vars    Session variables.
 *      argc    No of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 *      0 if success, 1 if error, -1 if exit required.
 */
IP_STATIC int
ipftpc_do_rmdir(Ipftpc_vars *vars, int argc, char *argv[]);


/*
 *===========================================================================
 *      ipftpc_do_rstatus
 *===========================================================================
 * Description:
 *      rstatus command.
 *
 * Parameters
 *      vars    Session variables.
 *      argc    No of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 *      0 if success, 1 if error, -1 if exit required.
 */
IP_STATIC int
ipftpc_do_rstatus(Ipftpc_vars *vars, int argc, char *argv[]);


/*
 *===========================================================================
 *      ipftpc_do_status
 *===========================================================================
 * Description:
 *      status command.
 *
 * Parameters
 *      vars    Session variables.
 *      argc    No of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 *      0 if success, 1 if error, -1 if exit required.
 */
IP_STATIC int
ipftpc_do_status(Ipftpc_vars *vars, int argc, char *argv[]);


/*
 *===========================================================================
 *      ipftpc_do_system
 *===========================================================================
 * Description:
 *      system command.
 *
 * Parameters
 *      vars    Session variables.
 *      argc    No of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 *      0 if success, 1 if error, -1 if exit required.
 */
IP_STATIC int
ipftpc_do_system(Ipftpc_vars *vars, int argc, char *argv[]);


/*
 *===========================================================================
 *      ipftpc_do_verbose
 *===========================================================================
 * Description:
 *      verbose command.
 *
 * Parameters
 *      vars    Session variables.
 *      argc    No of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 *      0 if success, 1 if error, -1 if exit required.
 */
IP_STATIC int
ipftpc_do_verbose(Ipftpc_vars *vars, int argc, char *argv[]);


/*
 *===========================================================================
 *      ipftpc_getline
 *===========================================================================
 * Description:
 *      Get command line.
 *
 * Parameters
 *
 * Returns:
 */
IP_STATIC int
ipftpc_getline(Ipftpc_vars *vars, const char *prompt, char *buf, int size);


/*
 *===========================================================================
 *      ipftpc_cmdloop
 *===========================================================================
 * Description:
 *      Loop and get commands.
 *
 * Parameters
 *
 * Returns:
 */
IP_STATIC void
ipftpc_cmdloop(Ipftpc_vars *vars);


#endif

/*
 ****************************************************************************
 * 10                   PUBLIC FUNCTIONS
 ****************************************************************************
 */


#ifdef __cplusplus
}
#endif

#endif
/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
