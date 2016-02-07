/*
 ******************************************************************************
 *                     HEADER FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_getopt.h,v $ $Revision: 1.9 $
 *     $Source: /home/interpeak/CVSRoot/ipcom/include/ipcom_getopt.h,v $
 *     $Author: lob $ $Date: 2004-05-05 12:54:25 $
 *     $State: Exp $ $Locker:  $
 *
 *     INTERPEAK_COPYRIGHT_STRING
 *     Design and implementation by Lennart Bang <lob@interpeak.se>
 ******************************************************************************
 */
#ifndef IPCOM_GETOPT_H
#define IPCOM_GETOPT_H

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 * The ipcom_getopt() functions incrementally parses a command line argument
 * list argv and returns the next known option character. An option character
 * is known if it has been specified in the string of accepted option charac-
 * ters, optstring.
 *
 * The option string optstring may contain the following elements: individu-
 * al characters, and characters followed by a colon to indicate an option
 * argument is to follow.  For example, an option string "x" recognizes an
 * option ``-x'', and an option string "x:" recognizes an option and argu-
 * ment ``-x argument''.  It does not matter to getopt() if a following ar-
 * gument has leading white space.
 * 
 * On return from ipcom_getopt(), optarg points to an option argument, if it
 * is anticipated, and the variable optind contains the index to the next
 * argv argument for a subsequent call to getopt().  The variable optopt
 * saves the last known option character returned by getopt().
 * 
 * The variable opterr and optind are both initialized to 1.  The optind
 * variable may be set to another value before a set of calls to ipcom_getopt()
 * in order to skip over more or less argv entries.
 * 
 * In order to use getopt() to evaluate multiple sets of arguments, or to
 * evaluate a single set of arguments multiple times, the variable optreset
 * must be set to 1 before the second and each additional set of calls to
 * ipcom_getopt(), and the variable optind must be reinitialized.
 * 
 * The ipcom_getopt() function returns -1 when the argument list is exhausted.
 * The interpretation of options in the argument list may be cancelled by
 * the option `--' (double dash) which causes getopt() to signal the end of
 * argument processing and returns -1.  When all options have been processed
 * (i.e., up to the first non-option argument), ipcom_getopt() returns -1.
 *
 * Long options may be given by declaring and initializing a 'struct ipoption'
 * (acctually a 'struct Ipcom_getopt_longopt_struct' and setting 
 * _ip_getopt_long_options (for ipcom_getopt()) or 'opt->long_options' for
 * ipcom_getopt_r() to point to this long option structure.
 * Read the text under 'ipoption' for more information.
 *
 * NOTE: Before calling ipcom_getopt() the function ipcom_getopt_clear() must
 * be called to initialize the ipcom variables, optind, optarg etc.
 *
 * NOTE2: ipcom_getopt() is non-reentrant due to use of global variables. The
 * new reentrant version ipcom_getopt_r() should be used in all new code to
 * avoid race conditions! Note, ipcom_getopt_clear_r() must be called before
 * calling ipcom_getopt_r() for the first time.
 */

/*
 ****************************************************************************
 * 2                    CONFIGURATION
 ****************************************************************************
 * Code colliding with the 'ip_option' macro below should be compiled with
 * IPCOM_GETOPT_EXCLUDE_LONGOPT defined.
 */

#include "ipcom_config.h"


/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#include "ipcom_type.h"
#include "ipcom_cstyle.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

/*
 *===========================================================================
 *                         getopt macros
 *===========================================================================
 * Interpeak macros to support the old non-reentrant ipcom_getopt() function.
 */
#define ip_opterr   ipcom_getopt_data.opterr
#define ip_optind   ipcom_getopt_data.optind
#define ip_optopt   ipcom_getopt_data.optopt
#define ip_optreset ipcom_getopt_data.optreset
#define ip_optarg   ipcom_getopt_data.optarg

#define _ip_getopt_long_only    ipcom_getopt_data.long_only
#define _ip_getopt_long_options ipcom_getopt_data.long_options
#define ip_option_index         ipcom_getopt_data.long_index


/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

/*
 *===========================================================================
 *                         Ip_option
 *===========================================================================
 * Describe the long-named options requested by the application.
 * _ip_getopt_long_options (or for ipcom_getopt_r(): opt->long_options) is
 * a vector of `struct option' terminated by an element containing a name
 * which is zero.
 * The field `has_arg' is:
 * 0 if the option does not take an argument,
 * 1 if the option requires an argument,
 * 2 if the option takes an optional argument.
 * If the field `flag' is nonzero, it points to a variable that is set to
 * the value given in the field `val' when the option is found, but
 * left unchanged if the option is not found.  
 *
 * Long options start out with either: '--arg', '+arg' or '-arg'.
 * Long options may take parameters of the form -arg=param or -arg param.
 */
#ifndef IPCOM_GETOPT_EXCLUDE_LONGOPT
#define Ipcom_getopt_longopt_struct Ip_option
#endif

struct Ipcom_getopt_longopt_struct
{
  char   *name;     /* long option name. */
  int     has_arg;  /* 1 if option needs argument, else 0. */
  int    *flag;     /* unused. */
  int     val;      /* unused. */
};


/*
 *===========================================================================
 *                         Ipcom_getopt
 *===========================================================================
 * Placeholder for reentrant ipcom_getopt_r() data. The strucure must be
 * initialized by ipcom_getopt_clear_r() before calling ipcom_getopt_r().
 */
typedef struct Ipcom_getopt_struct
{
  int      opterr;     /* If error message should be printed (default 1). */
  int      optind;     /* Index into parent argv vector (default 1). */
  int      optopt;     /* Character checked for validity. */
  int      optreset;   /* set to 1 to reset getopt(). */
  char    *optarg;     /* Optional argument associated with option. */

  char    *place;

  int      long_index;
  int      long_only;
  struct Ipcom_getopt_longopt_struct *long_options;
}
Ipcom_getopt;


/*
 ****************************************************************************
 * 6                    VARIABLES
 ****************************************************************************
 */

/*
 *===========================================================================
 *                         ipcom_getopt_data
 *===========================================================================
 * Global ipcom_getopt() data for support of the old ipcom_getopt() function.
 */
IP_EXTERN Ipcom_getopt ipcom_getopt_data;


/*
 ****************************************************************************
 * 7                    FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                 NEW REENTRANT IPCOM_GETOPT_R
 *===========================================================================
 */

/* NOTE: Always call ipcom_getopt_clear_r() before the first parse. */
IP_PUBLIC void
ipcom_getopt_clear_r(Ipcom_getopt *opt);

IP_PUBLIC int
ipcom_getopt_r(int argc, char * const *argv, const char *optstring, Ipcom_getopt *opt);


/*
 *===========================================================================
 *                 OLD NON-REENTRANT IPCOM_GETOPT
 *===========================================================================
 */

/* NOTE: Always call ipcom_getopt_clear() before the first parse. */
IP_PUBLIC void
ipcom_getopt_clear(void);

IP_PUBLIC int
ipcom_getopt(int argc, char * const *argv, const char *optstring);


#ifdef __cplusplus
}
#endif

#endif

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

