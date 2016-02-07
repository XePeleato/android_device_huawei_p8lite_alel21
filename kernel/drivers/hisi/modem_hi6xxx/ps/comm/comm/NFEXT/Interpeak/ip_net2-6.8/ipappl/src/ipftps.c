/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *   Document no: @(#) $ipftps.c$ $RCSfile: ipftps.c,v $ $Revision: 1.67 $
 *   $Source: /home/interpeak/CVSRoot/ipappl/src/ipftps.c,v $
 *   $Author: gzadoyan $
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

#include "ipftps_config.h"

/* Timeout when sending data to socket */
#define IPFTPS_SEND_TIMEOUT 30


/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#define IPCOM_USE_CLIB_PROTO
#include <ipcom_clib.h>
#include <ipcom_clib2.h>
#include <ipcom_time.h>
#include <ipcom_os.h>
#include <ipcom_auth.h>
#include <ipcom_sock.h>
#include <ipcom_err.h>
#include <ipcom_syslog.h>
#include <ipcom_list.h>
#include <ipcom_ipd.h>
#include <ipcom_sysvar.h>

#include "ipftps.h"

#ifdef IP_PORT_VXWORKS
#include <vxWorks.h>
#include <pathLib.h>
#include <iosLib.h>
#include <dirent.h>
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

/* Public data */
typedef struct
{
#ifdef IPCOM_USE_INET
    Ipftps_sockcb sock4;                /* Main IPv4 server socket */
#endif
#ifdef IPCOM_USE_INET6
    Ipftps_sockcb sock6;                /* Main IPv6 server socket */
#endif
    long active_sessions;               /* Session number */
    long session_no;                    /* Session number */
}
Ipftps_data;


/*
 ****************************************************************************
 * 6                    EXTERN PROTOTYPES
 ****************************************************************************
 */

#ifdef IP_PORT_OSE5
extern int efs_donate_fd(int fd, Ip_pid_t to);
extern int efs_receive_fd(Ip_pid_t from);
#endif


/*
 ****************************************************************************
 * 7                    LOCAL PROTOTYPES
 ****************************************************************************
 */

#define IPFTPS_C
#include "ipftps_h.h"

IP_PUBLIC Ip_err ipftps_start(void);
IP_PUBLIC const char *ipftps_version(void);
IP_STATIC int ipftps_chdir(Ipftps_session *session, char *path);
IP_STATIC char* ipftps_wildcard_check(char *path, struct Ip_stat *st);  
IP_STATIC int ipftps_pattern_check(char *pattern, char *name);

/*
 ****************************************************************************
 * 8                    DATA
 ****************************************************************************
 */

/* List of commmands */
IP_STATIC const Ipftps_cmd ipftps_cmdlist[] =
{
  /* Name    Handler     MinMaxLoginArgtypeDescription */
    {"ABOR", ipftps_abor, 0, 0, 0, 1,  "Abort data transfer"},
    {"ACCT", ipftps_acct, 1, 1, 0, 1, "Select account"},
    {"APPE", ipftps_appe, 1, 1, 1, 1, "Append to file"},
    {"CDUP", ipftps_cdup, 0, 0, 0, 1, "Change to parent dir"},
    {"CWD",  ipftps_cwd,  1, 1, 1, 1, "Change directory"},
    {"DELE", ipftps_dele, 1, 1, 1, 1, "Delete file"},
    {"EPRT", ipftps_eprt, 3, 3, 0, 1, "Specify extended data port"},
    {"EPSV", ipftps_epsv, 0, 1, 0, 1, "Enter extended passive mode"},
    {"HELP", ipftps_help, 0, 1, 0, 0, "Help"},
    {"LIST", ipftps_list, 0, 1, 1, 1, "List file(s)"},
    {"MKD",  ipftps_mkd,  1, 1, 1, 1, "Make directory"},
    {"MODE", ipftps_mode, 1, 1, 0, 1, "Set transfer mode"},
    {"NLST", ipftps_nlst, 0, 1, 1, 1, "List file name(s)"},
    {"NOOP", ipftps_noop, 0, 0, 0, 1, "No operation"},
    {"PASS", ipftps_pass, 1, 1, 0, 0, "Login password"},
    {"PASV", ipftps_pasv, 0, 0, 0, 1, "Enter passive mode"},
    {"PORT", ipftps_port, 6, 6, 0, 1, "Specify data port"},
    {"PWD",  ipftps_pwd,  0, 0, 0, 1, "Print current directory"},
    {"QUIT", ipftps_quit, 0, 0, 0, 0, "Quit FTP session"},
    {"RETR", ipftps_retr, 1, 1, 1, 1, "Retrieve file"},
    {"RMD",  ipftps_rmd,  1, 1, 1, 1, "Remove directory"},
    {"RNFR", ipftps_rnfr, 1, 1, 1, 1, "Rename from"},
    {"RNTO", ipftps_rnto, 1, 1, 1, 1, "Rename to"},
    {"STAT", ipftps_stat, 0, 1, 1, 1, "Show current status"},
    {"STOR", ipftps_stor, 1, 1, 1, 1, "Store file"},
    {"STRU", ipftps_stru, 1, 1, 0, 1, "Set file structure"},
    {"SYST", ipftps_syst, 0, 0, 0, 1, "Show system type"},
    {"TYPE", ipftps_type, 1, 2, 0, 1, "Set representation type"},
    {"USER", ipftps_user, 1, 1, 0, 0, "Login username"},
    {"XCUP", ipftps_cdup, 0, 0, 0, 1, "Change to parent dir"},
    {"XCWD", ipftps_cwd,  1, 1, 1, 1, "Change directory"},
    {"ALLO", ipftps_allo, 1, 2, 0, 1, "Allocate storage"},
#ifdef IPFTPS_USE_TEST
    {"XFTP", ipftps_xftp, 1, 4, 0, 0,"Internal test"},
#endif
    {"XMKD", ipftps_mkd,  1, 1, 1, 1, "Make directory"},
    {"XPWD", ipftps_pwd,  0, 0, 0, 1,  "Print current directory"},
    {"XRMD", ipftps_rmd,  1, 1, 1, 1, "Remove directory"},
    {0, 0, 0}
};


/*
 *===========================================================================
 *                         ipftps
 *===========================================================================
 * Global data.
 */
IP_STATIC Ipftps_data   ipftps;


/*
 ****************************************************************************
 * 9                    LOCAL FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *      ipftps_filewrite_ascii
 *===========================================================================
 *
 * Description:
 *      Write ASCII buffer to file.
 *
 * Parameters:
 *      fd      File descriptor.
 *      buf     Buffer.
 *      nbuf    New buffer.
 *      bufsize Buffer size.
 *      orphant True if last buffer was terminated with '\r'.
 *
 * Returns:
 *      0 if success, 1 otherwise.
 */

IP_STATIC int
ipftps_filewrite_ascii(Ip_fd fd, char *buf, char *nbuf, int bufsize,
                       Ip_bool *orphant)
{
    char *cp = nbuf;
    int begin;
    int end;
    int no;

    /* Enter '\r' if not followed by '\n' */
    if (*orphant && buf[0] != '\n')
    {
        /* Output missing '\r' */
        *cp++ = '\r';
    }

    *orphant = 0;

    /* Loop for entire buffer */
    for (begin = end = 0; end < bufsize; end++)
    {
        if (buf[end] == '\r')
        {
            if (end == (bufsize - 1))
            {
                /* '\r' at end of buffer, remember */
                *orphant = 1;
            }
            else if (buf[end + 1] != '\n')
            {
                /* Not followed by '\n' - ignore */
                continue;
            }

            /* Enter everything up to '\r' */
            no = end - begin;
            ipcom_memcpy(cp, &buf[begin], no);
            cp += no;

            /* Continue at '\n' */
            begin = end + 1;
        }
    }

    /* Enter last part */
    no = end - begin;
    ipcom_memcpy(cp, &buf[begin], no);
    cp += no;

    /* Write to file */
    no = cp - nbuf;
    if (ipcom_filewrite(fd, nbuf, no) != no)
    {
        return 1;
    }

    return 0;
}


/*
 *===========================================================================
 *      ipftps_socketwrite_ascii
 *===========================================================================
 *
 * Description:
 *      Write ASCII buffer to socket.
 *
 * Parameters:
 *      fd      File descriptor.
 *      buf     Buffer.
 *      nbuf    New buffer.
 *      bufsize Buffer size.
 *
 * Returns:
 *      0 if success, 1 otherwise.
 */
IP_STATIC int
ipftps_socketwrite_ascii(Ip_fd fd, char *buf, char *nbuf, int bufsize)
{
    char *cp = nbuf;
    int begin;
    int end;
    int no;

    /* Loop for entire buffer */
    for (begin = end = 0; end < bufsize; end++)
    {
        if (buf[end] == '\n')
        {
            /* Enter everything up to '\n' */
            no = end - begin;
            ipcom_memcpy(cp, &buf[begin], no);
            cp += no;

            /* Enter newline */
            *cp++ = '\r';
            *cp++ = '\n';

            /* Continue after '\n' */
            begin = end + 1;
        }
    }

    /* Enter last part */
    no = end - begin;
    ipcom_memcpy(cp, &buf[begin], no);
    cp += no;

    /* Write to socket */
    no = cp - nbuf;
    if (ipcom_socketwrite(fd, nbuf, no) != no)
    {
        return 1;
    }

    return 0;
}


/*
 *===========================================================================
 *      ipftps_skipdrive
 *===========================================================================
 *
 * Description:
 *      Skip possible drive part of path.
 *
 * Parameters:
 *      path    Directory path.
 *
 * Returns:
 *      New path.
 */
IP_STATIC char *
ipftps_skipdrive(char *path)
{
    char *cp;

    if ((cp = ipcom_strchr(path, ':')) == IP_NULL)
    {
        cp = path;
    }
    else
    {
        cp++;
    }

    return cp;
}


/*
 *===========================================================================
 *      ipftps_dirindex
 *===========================================================================
 *
 * Description:
 *      Calculate level of directory path.
 *
 * Parameters:
 *      path    Directory path.
 *      idomin  Min flag.
 *
 * Returns:
 *      Final subdirectory level if idomin == 0,
 *      minimum subdirectory level if idomin == 1.
 */
IP_STATIC int
ipftps_dirindex(char *path, int idomin)
{
    int level = 0;
    int min = 0;
    char *start;
    char *stop;
    char *cp;

    cp = ipftps_skipdrive(path);

    while (*cp != '\0')
    {
        char c;

        start = cp;
        cp += ipcom_strcspn(cp, IPFTPS_DIRSEP);
        stop = cp;
        cp += ipcom_strspn(cp, IPFTPS_DIRSEP);
        c = *stop;
        *stop = '\0';

        if (ipcom_strcmp(start, "..") == 0)
        {
            level--;
        }
        else if (*start != '\0' && ipcom_strcmp(start, ".") != 0)
        {
            level++;
        }

        *stop = c;

        if (level < min)
        {
            min = level;
        }
    }

    return (idomin ? min : level);
}


/*
 *===========================================================================
 *      ipftps_dircmp
 *===========================================================================
 *
 * Description:
 *      Check if dir is OK wrt root.
 *
 * Parameters:
 *      root    Directory root.
 *      path    Directory path.
 *
 * Returns:
 *      0 if OK, 1 otherwise.
 */
IP_STATIC int
ipftps_dircmp(char *root, char *path)
{
    int len;
    int i;

    /*
    ** Skip possible drive letters in root, but
    ** only if path has no drive letters.
    */
    if (ipcom_strchr(path, ':') == IP_NULL)
    {
        root = ipftps_skipdrive(root);
    }

    len = ipcom_strlen(root);

    for (i = 0; i < len; i++)
    {
        if ((ipcom_strchr(IPFTPS_DIRSEP, root[i]) &&
             ipcom_strchr(IPFTPS_DIRSEP, path[i])) ||
            root[i] == path[i])
        {
            /* OK so far */
        }
        else
        {
            return 1;
        }
    }

    return 0;
}


/*
 *===========================================================================
 *      ipftps_root_check
 *===========================================================================
 *
 * Description:
 *      Check if path valid, and prepare for its use in next command.
 *
 * Parameters:
 *      session Session descriptor.
 *      path    Directory path.
 *
 * Returns:
 *      0 if path OK, 1 otherwise.
 */
IP_STATIC int
ipftps_root_check(Ipftps_session *session, char **path)
{
    char *newpath = ipftps_skipdrive(*path);
    int len;

    /* Device or drive letter? */
    if (newpath != *path)
    {
        /* Empty session root is required to change drive/device */
        if (session->root[0] != '\0')
        {
            return 1;
        }
        
        newpath = *path;
    }
    else if (ipcom_strchr(IPFTPS_DIRSEP, *newpath) != IP_NULL)
    {
        char *orgpath = newpath;
        
        /*
        ** Absolute path, remove initial slash
        ** to make it relative to root.
        */
        if (*++newpath == '\0')
        {
            *--newpath = '.';
        }

        /* Check for trespassing */
        if (ipftps_dirindex(newpath, 1) < 0)
        {
            /* Peeked above root */
            return 1;
        }

        if (session->root[0] == '\0' ||
            (ipcom_strchr(IPFTPS_DIRSEP, session->root[0]) != IP_NULL && session->root[1] == '\0'))
        {
            /*
            ** Session root is /, and it may not be safe to cd there
            ** since the file system may have its root at e.g. /ram.
            ** Use the absolute path directly.
            */
            if (newpath == orgpath)
            {
                *newpath = '/';
            }
            else
            {
                newpath = orgpath;
            }
        }
        else if (ipftps_chdir(session, session->root) == 0)
        {
            session->cwdroot = 1;
        }
        else
        {
            IPCOM_LOG1(WARNING, "Invalid root dir: %s", session->root);
            return 1;
        }
    }
    else
    {
        /* Check for trespassing */
        if ((ipftps_dirindex(session->cwd, 0) + ipftps_dirindex(newpath, 1)) <
            ipftps_dirindex(session->root, 0))
        {
            return 1;
        }

        /* Set CWD */
        if (ipftps_chdir(session, session->cwd) != 0)
        {
            IPCOM_LOG1(WARNING, "Invalid current directory: %s", session->cwd);
            return 1;
        }
    }

    /* Remove possible trailing slash */
    len = ipcom_strlen(newpath);
    if (len > 1 && ipcom_strchr(IPFTPS_DIRSEP, newpath[len - 1]) != IP_NULL)
    {
        newpath[len - 1] = '\0';
    }

    *path = newpath;

#ifdef IP_PORT_VXWORKS   
    {
        char *fullPath = IP_NULL;
        const char *pTail;
        char *tmpPath = IP_NULL;

        if ((fullPath = ipcom_malloc(IP_PATH_MAX)) == NULL)
        {
            return 1;
        }

        /* Use the absolute path for vxworks */
        if (pathCat(session->cwd, *path, fullPath)  == ERROR)
        {
           ipftps_free(fullPath);
           return 1; 
        }
    	pathCondense(fullPath);  
 
        tmpPath = *path;
        if ((tmpPath[strlen(tmpPath) - 1] == '/') && strcmp(fullPath, "/"))
        {
            ipcom_strcat(fullPath, "/");
        }
        
        /* Make sure that new path starts with a device name */
        iosDevFind (fullPath, &pTail);
        
        if (pTail != fullPath)
        {
            ipcom_strcpy(*path, fullPath);  
            ipftps_free(fullPath);
            return 0;
        }
    
        if (tmpPath[strlen(tmpPath) - 1] == '.')
        {
            ipcom_strcat(fullPath, "/");
            iosDevFind (fullPath, &pTail);
            if (pTail != fullPath)
            {
                ipcom_strcpy(*path, fullPath);  
                ipftps_free(fullPath);
                return 0;
            }
        }

        ipftps_free(fullPath);
        return 1; /* ENODEV */
    }
#endif
    
    return 0;
}


/*
 *===========================================================================
 *      ipftps_dir_check
 *===========================================================================
 *
 * Description:
 *      Handle potential cwd change.
 *
 * Parameters:
 *      session Session descriptor.
 *
 * Returns:
 *      0 if success, -1 otherwise.
 */
IP_STATIC int
ipftps_dir_check(Ipftps_session *session)
{
#ifndef IP_PORT_VXWORKS
    if (session->cwd != session->root)
    {
        ipftps_free(session->cwd);
    }

    if ((session->cwd = ipcom_malloc(IP_PATH_MAX)) == IP_NULL ||
        ipcom_getcwd(session->cwd, IP_PATH_MAX) == IP_NULL)
    {
        IPCOM_LOG0(ERR, "Out of memory, switched to root");

        /* Avoid malloc here */
        if (session->cwd != session->root)
        {
            ipftps_free(session->cwd);
        }
        session->cwd = session->root;
        return -1;
    } else
#endif

    if (ipftps_dircmp(ipftps_skipdrive(session->root),
                           ipftps_skipdrive(session->cwd)) != 0)
    {
        /*
         ** This is never supposed to happen, teardown
         ** session to prevent from security breaches.
         */
        IPCOM_LOG0(ERR, "Directory error, terminating session");
        session->authentications = 0;
        ipcom_errno = IP_ERRNO_ENOTDIR;
        return -1;
    }
    return 0;
}


/*
 *===========================================================================
 *      ipftps_sysget_int
 *===========================================================================
 *
 * Description:
 *      Get integer system variable.
 *
 * Parameters:
 *      name    Variable name.
 *      defval  Default value if not found.
 *      message Print message if not found.
 *
 * Returns:
 *      Variable value.
 */
IP_STATIC int
ipftps_sysget_int(const char *name, int defval, Ip_bool message)
{
    Ipftps_session *session = IP_NULL;
    char strval[12];
    Ip_size_t strval_len = sizeof(strval);

    (void)session;

    if (ipcom_sysvar_get(name, strval, &strval_len) == IP_NULL)
    {
        if (message)
        {
            IPCOM_LOG2(WARNING, "Missing sysvar %s, using %d",  name, defval);
        }

        return defval;
    }

    return ipcom_atoi(strval);
}


/*
 *===========================================================================
 *      ipftps_sysget_str
 *===========================================================================
 *
 * Description:
 *      Get string system variable.
 *
 * Parameters:
 *      name    Variable name.
 *      defval  Default value if not found.
 *      message Print message if not found.
 *
 * Returns:
 *      Variable value.
 */
IP_STATIC char *
ipftps_sysget_str(const char *name, char *buf, Ip_size_t *bufsize,
                  const char *defval, Ip_bool message)
{
    Ip_size_t oldsize = (buf == IP_NULL ? 0 : *bufsize);
    Ipftps_session *session = IP_NULL;
    char *ret;

    (void)session;

    if ((ret = ipcom_sysvar_get(name, buf, bufsize)) == IP_NULL)
    {
        if (oldsize > 0 && *bufsize > oldsize)
        {
            if (message)
            {
                IPCOM_LOG2(WARNING, "Buffer overflow reading sysvar %s, using %s",
                           name, defval);
            }
        }
        else if (message)
        {
            IPCOM_LOG2(WARNING, "Missing sysvar %s, using %s",
                       name, defval);
        }

        ret = (buf == IP_NULL ? ipcom_strdup(defval) :
                                ipcom_strcpy(buf, defval));
    }

    return ret;
}


/*
 *===========================================================================
 *      ipftps_ls
 *===========================================================================
 *
 * Description:
 *      List file (long format).
 *
 * Parameters:
 *      session Session descriptor.
 *      fd      File descriptor.
 *      path1   1st part of path.
 *      path2   2nd part of path.
 *
 * Returns:
 */
IP_STATIC int
ipftps_ls(Ipftps_session *session, Ip_fd fd, char *path1, char *path2)
{
    static const Ip_mode_t perm[] =
    {
        IP_S_IRUSR, IP_S_IWUSR, IP_S_IXUSR,
        IP_S_IRGRP, IP_S_IWGRP, IP_S_IXGRP,
        IP_S_IROTH, IP_S_IWOTH, IP_S_IXOTH
    };
    char mode[] = "-rwxrwxrwx";
    char timestr[16];
#ifdef IPCOM_USE_TIME
    Ip_time_t now;
    struct Ip_tm tm;
#endif
    struct Ip_stat st;
    char *opath;
    char *path;
    int p1len = ipcom_strlen(path1);
    int ret = 0;
    int i;
 
    if ((opath = path = ipcom_malloc(ipcom_strlen(path1) +
                                     ipcom_strlen(path2) + 2)) == IP_NULL)
    {
        IPCOM_LOG0(ERR, "Out of memory");
        ipftps_reply(session, "452 Out of memory"CL);
        ret = 1;
        goto leave2;
    }

    ipcom_strcpy(path, path1);
    ipcom_strcat(path, (p1len != 0 && path[p1len - 1] != '/' ? "/" : ""));
    ipcom_strcat(path, path2);

    if (ipcom_stat(path, &st) != 0)
    {
        ipftps_perror(session, 550);
        ret = 1;
        goto leave1;
    }

    /* Set mode */
    if (IP_S_ISDIR(st.st_mode))
        mode[0] = 'd';
#ifdef IP_S_ISCHR
    else if (IP_S_ISCHR(st.st_mode))
        mode[0] = 'c';
#endif
#ifdef IP_S_ISBLK
    else if (IP_S_ISBLK(st.st_mode))
        mode[0] = 'b';
#endif
#ifdef IP_S_ISFIFO
    else if (IP_S_ISFIFO(st.st_mode))
        mode[0] = 'p';
#endif
#ifdef IP_S_ISLNK
    else if (IP_S_ISLNK(st.st_mode))
        mode[0] = 'l';
#endif
#ifdef IP_S_ISSOCK
    else if (IP_S_ISSOCK(st.st_mode))
        mode[0] = 's';
#endif

    /* Set permissions */
    for (i = 0; i < (sizeof(perm) / sizeof perm[0]); i++)
    {
        if ((st.st_mode & perm[i]) == 0)
        {
            mode[1 + i] = '-';
        }
    }

    /* Create window for fine grain time */
#ifdef IPCOM_USE_TIME
    (void)ipcom_time(&now);
    (void)ipcom_localtime_r(&st.st_mtime, &tm);
    if (st.st_mtime < (now - (6L * IPFTPS_MONTHS)) ||
        st.st_mtime > (now + (1L * IPFTPS_HOURS)))
    {
        /* Low resolution */
        i = ipcom_strftime(timestr, sizeof(timestr), "%b %d  %Y", &tm);
    }
    else
    {
        /* High resolution */
        i = ipcom_strftime(timestr, sizeof(timestr), "%b %d %H:%M", &tm);
    }

    /* Terminate timestr. */
    if (i == 0)
        timestr[0] = '\0';
    else if (i == sizeof(timestr))
        timestr[sizeof(timestr) - 1] = '\0';
#else
    ipcom_strcpy(timestr, "-");
#endif

     ipftps_reply_sock(session, fd,
                       "%s %3d %-6d %-6d %10ld %12.12s %s%s"CL,
                       mode, st.st_nlink, st.st_uid, st.st_gid,
                       st.st_size, timestr,
                       (session->cwdroot && path1[0] == '\0' ? "/" : ""),
                       path2);

 leave1:
    ipcom_free(opath);

 leave2:
    return ret;
}


/*
 *===========================================================================
 *      ipftps_abor
 *===========================================================================
 *
 * Description:
 *      ABOR command.
 *
 * Parameters:
 *      session Session descriptor.
 *      argc    Number of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 */
IP_STATIC void
ipftps_abor(Ipftps_session *session, int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    if (session->dsock.fd == -1)
    {
        ipftps_reply(session, "226 No data connection"CL);
    }
    else
    {
        ipftps_dsock_close(session);
        ipftps_reply(session, "226 Closing data connection"CL);
    }
}


/*
 *===========================================================================
 *      ipftps_acct
 *===========================================================================
 *
 * Description:
 *      ACCT command.
 *
 * Parameters:
 *      session Session descriptor.
 *      argc    Number of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 */
IP_STATIC void
ipftps_acct(Ipftps_session *session, int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    ipftps_reply(session, "202 Command not implemented"CL);
}


/*
 *===========================================================================
 *      ipftps_appe
 *===========================================================================
 *
 * Description:
 *      APPE command.
 *
 * Parameters:
 *      session Session descriptor.
 *      argc    Number of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 */

IP_STATIC void
ipftps_appe(Ipftps_session *session, int argc, char *argv[])
{
    argv[argc++] = "a";

    ipftps_stor(session, argc, argv);
}


/*
 *===========================================================================
 *      ipftps_cdup
 *===========================================================================
 *
 * Description:
 *      CDUP command.
 *
 * Parameters:
 *      session Session descriptor.
 *      argc    Number of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 */

IP_STATIC void
ipftps_cdup(Ipftps_session *session, int argc, char *argv[])
{
	char *dotdotp;
    char dotdot[IP_PATH_MAX];
 
    (void)argc;
    (void)argv;
    
    ipcom_strcpy(dotdot,"..");
    dotdotp = dotdot;

    ipftps_cwd(session, 1, &dotdotp);
}


/*
 *===========================================================================
 *      ipftps_cwd
 *===========================================================================
 *
 * Description:
 *      CWD command.
 *
 * Parameters:
 *      session Session descriptor.
 *      argc    Number of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 */

IP_STATIC void
ipftps_cwd(Ipftps_session *session, int argc, char *argv[])
{
    (void)argc;

    if (ipftps_root_check(session, &argv[0]) != 0)
    {
        IPCOM_LOG2(WARNING, "Access denied to %s from %s",
                   argv[0], session->cwd);
        ipftps_reply(session, "550 Permission denied"CL);
    }
    else if (ipftps_chdir(session, argv[0]) != 0)
    {
        ipftps_perror(session, 550);
    }
    else if (ipftps_dir_check(session) != 0)
    {
        ipftps_perror(session, (ipcom_errno == IP_ERRNO_ENOMEM ? 452 : 550));
    }
    else
    {
        ipftps_reply(session, "250 CWD changed"CL);
    }
}


/*
 *===========================================================================
 *      ipftps_dele
 *===========================================================================
 *
 * Description:
 *      DELE command.
 *
 * Parameters:
 *      session Session descriptor.
 *      argc    Number of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 */

IP_STATIC void
ipftps_dele(Ipftps_session *session, int argc, char *argv[])
{
    (void)argc;

    if (session->readonly)
    {
        IPCOM_LOG0(WARNING, "File deletion denied");
        ipftps_reply(session, "550 Permission denied"CL);
    }
    else if (ipftps_root_check(session, &argv[0]) != 0)
    {
        IPCOM_LOG2(WARNING, "Access denied to %s from %s",
                   argv[0], session->cwd);
        ipftps_reply(session, "550 Permission denied"CL);
    }
    else if (ipcom_unlink(argv[0]) != 0)
    {
        ipftps_perror(session, 550);
    }
    else
    {
        ipftps_reply(session, "250 File deleted"CL);
    }
}


/*
 *===========================================================================
 *      ipftps_eprt
 *===========================================================================
 *
 * Description:
 *      EPRT command.
 *
 * Parameters:
 *      session Session descriptor.
 *      argc    Number of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 */
IP_STATIC void
ipftps_eprt(Ipftps_session *session, int argc, char *argv[])
{
    union Ip_sockaddr_union addr;
    Ip_u32 h[3];
    int i;

    (void)argc;

    if (session->epsvonly)
    {
        ipftps_reply(session, "501 In extended passive mode"CL);
        return;
    }

    session->passive = 0;

    /* Parse first and last argument */
    for (i = 0; i < 3; i += 2)
    {
        char *stop;

        h[i] = ipcom_strtol(argv[i], &stop, 10);

        if (*stop != '\0' || h[i] > 65535)
        {
            ipftps_reply(session, "501 Bad address family or port"CL);
            return;
        }
    }

    ipcom_memset(&addr, 0, sizeof(addr));

    switch (h[0])
    {
    case 1:
        /* IPv4 */
        IPCOM_SA_LEN_SET(&addr.sin, sizeof(struct Ip_sockaddr_in));
        addr.sin.sin_family = IP_AF_INET;

        if (ipcom_inet_pton(IP_AF_INET, argv[1], &addr.sin.sin_addr) != 1)
        {
            ipftps_reply(session, "501 Bad IPv4 address"CL);
            return;
        }
        addr.sin.sin_port = (Ip_u16)ip_htons(h[2]);
        break;

#ifdef IPCOM_USE_INET6
    case 2:
        /* IPv6 */
        ipcom_memset(&addr.sin6, 0, sizeof(addr.sin6));
        IPCOM_SA_LEN_SET(&addr.sin6, sizeof(struct Ip_sockaddr_in6));
        addr.sin6.sin6_family = IP_AF_INET6;
        if (ipcom_inet_pton(IP_AF_INET6, argv[1], &addr.sin6.sin6_addr) != 1)
        {
            ipftps_reply(session, "501 Bad IPv6 address"CL);
            return;
        }
        addr.sin6.sin6_port = (Ip_u16)ip_htons(h[2]);
        break;
#endif

    default:
        ipftps_reply(session, "522 Bad address family, use " IPFTPS_AFSTR CL);
        return;
    }

    if (session->proxy == 0 &&
        IPFTPS_ADDRCMP(&addr, &session->csock.paddr) != 0)
    {
        IPCOM_LOG1(WARNING, "EPRT IP address %s denied", argv[1]);
        ipftps_reply(session, "504 Invalid host"CL);
        return;
    }

    if (h[2] <= session->pportbase)
    {
        IPCOM_LOG1(WARNING, "EPRT port number %u denied",
                   (unsigned int)h[2]);
        ipftps_reply(session, "504 Invalid port"CL);
        return;
    }

    ipcom_memcpy(&session->dsock.paddr, &addr, sizeof(session->dsock.paddr));

    ipftps_reply(session, "200 Extended port OK"CL);
}


/*
 *===========================================================================
 *      ipftps_epsv
 *===========================================================================
 *
 * Description:
 *      EPSV command.
 *
 * Parameters:
 *      session Session descriptor.
 *      argc    Number of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 */

IP_STATIC void
ipftps_epsv(Ipftps_session *session, int argc, char *argv[])
{
    int af = (session->csock.paddr.sa.sa_family == IP_AF_INET ? 1 : 2);

    if (argc == 1)
    {
        if (ipcom_strcasecmp(argv[0], "ALL") == 0)
        {
            ipftps_reply(session, "200 Locked in extended passive mode"CL);
            session->epsvonly = 1;
            return;
        }
        else
        {
            char *stop;

            af = ipcom_strtol(argv[0], &stop, 10);

            if (*stop != '\0' || af < 1 || af > IPFTPS_AFMAX)
            {
                ipftps_reply(session, "522 Bad address family, use "
                             IPFTPS_AFSTR CL);
                return;
            }

            if (session->proxy == 0 && af != session->csock.paddr.sa.sa_family)
            {
                ipftps_reply(session, "522 Bad address family, use (%d)" CL,
                             (session->csock.paddr.sa.sa_family == IP_AF_INET ?
                              1 : 2));
                return;
            }
        }
    }

    session->dsock.paddr.sa.sa_family = (af == 1 ? IP_AF_INET : IP_AF_INET6);

    if (ipftps_dsock_open(session, 1) != 0)
    {
        return;
    }

    ipftps_reply(session,
                 "229 Entering extended passive mode (|||%u|)"CL,
                 (unsigned int)ip_ntohs(IPFTPS_PORT(&session->dsock.laddr)));
}


/*
 *===========================================================================
 *      ipftps_help
 *===========================================================================
 *
 * Description:
 *      HELP command.
 *
 * Parameters:
 *      session Session descriptor.
 *      argc    Number of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 */
IP_STATIC void
ipftps_help(Ipftps_session *session, int argc, char *argv[])
{
    const Ipftps_cmd *cmd;

    if (argc == 0)
    {
        ipftps_reply(session, "211- Command list:"CL);
    }

    /* Loop for help texts */
    for (cmd = ipftps_cmdlist; cmd->name != IP_NULL; cmd++)
    {
        if (argc == 0)
        {
            /* Help on all commands */
            ipftps_reply(session, " %4s - %s"CL, cmd->name, cmd->help);
        }
        else if (ipcom_strcasecmp(argv[0], cmd->name) == 0)
        {
            /* Help on specific command */
            ipftps_reply(session, "214 %s - %s"CL, cmd->name, cmd->help);
            return;
        }
    }

    /* Terminate */
    if (argc == 0)
    {
        ipftps_reply(session, "211 Use HELP <cmd> for help on cmd"CL);
    }
    else
    {
        ipftps_reply(session, "501 No help available"CL);
    }
}


/*
 *===========================================================================
 *      ipftps_list
 *===========================================================================
 *
 * Description:
 *      LIST command.
 *
 * Parameters:
 *      session Session descriptor.
 *      argc    Number of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 */

IP_STATIC void
ipftps_list(Ipftps_session *session, int argc, char *argv[])
{
    struct Ip_stat st;
    char *path = argv[0];
    struct Ip_dirent *ent;
    char dot[IP_PATH_MAX];
    int ret = 0;
    Ip_bool aflag = IP_FALSE; 
    
    ipcom_strcpy(dot,".");
    
    if (argc == 0)
    {
        path = dot;
    }
    else if ((ipcom_strncmp(path, "-l", 2) == 0) || 
             (ipcom_strncmp(path, "-al", 3) == 0)) 
    {
        if (ipcom_strncmp(path, "-al", 3) == 0)
        aflag = IP_TRUE; 
        path = dot;        
    }

    if (ipftps_root_check(session, &path) != 0)
    {
        IPCOM_LOG2(WARNING, "Access denied to %s from %s",
                   path, session->cwd);
        ipftps_reply(session, "550 Permission denied"CL);
        return;
    }

    if (ipcom_stat(path, &st) != 0)
    {
        ipftps_perror(session, 550);
        return;
    }

    /* Open data socket */
    if (ipftps_dsock_open(session, 0) != 0)
    {
        return;
    }

    /* Dir or file? */
    if (IP_S_ISDIR(st.st_mode))
    {
        IP_DIR *dir;

        dir = ipcom_opendir(path);
        if (dir == IP_NULL)
        {
            ipftps_perror(session, 550);
            ipftps_dsock_close(session);
            return;
        }

        /* Loop for files */
        while ((ent = ipcom_readdir(dir)) != 0)
        {
            if ((aflag == IP_TRUE) || (ent->d_name[0] != '.'))
            {
                if ((ret = ipftps_ls(session, session->dsock.fd, path, ent->d_name)) != 0)
                {
                    break;
                }
            }
        }

        if (ipcom_closedir(dir) != 0)
        {
            IPCOM_LOG2(WARNING, "Directory close failed for %s: %s",
                      path, ipcom_strerror(ipcom_errno));
        }
    }
    else
    {
        ret = ipftps_ls(session, session->dsock.fd, "", path);
    }

    ipftps_dsock_close(session);

    if (ret == 0)
    {
        ipftps_reply(session, "226 Transfer complete"CL);
    }
}


/*
 *===========================================================================
 *      ipftps_mkd
 *===========================================================================
 *
 * Description:
 *      MKD command.
 *
 * Parameters:
 *      session Session descriptor.
 *      argc    Number of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 */
IP_STATIC void
ipftps_mkd(Ipftps_session *session, int argc, char *argv[])
{
    char *path = argv[0];
#ifdef IP_PORT_VXWORKS   
    char *argv0 = IP_NULL;
#endif
    (void)argc;

#ifdef IP_PORT_VXWORKS   
    if(argv[0])
    {
        if ((argv0 = ipcom_malloc(IP_PATH_MAX)) == IP_NULL)
        {
            IPCOM_LOG0(ERR, "Out of memory");
            ipftps_reply(session, "452 Out of memory"CL);
            return;
        }
        ipcom_strcpy(argv0, argv[0]);
    }
#endif
    if (session->readonly)
    {
        IPCOM_LOG0(WARNING, "Make dir denied");
        ipftps_reply(session, "550 Permission denied"CL);
    }
    else if (ipftps_root_check(session, &path) != 0)
    {
        IPCOM_LOG2(WARNING, "Access denied to %s from %s",
                  argv[0], session->cwd);
        ipftps_reply(session, "550 Permission denied"CL);
    }
    else if (ipcom_mkdir(path, 0777) != 0)
    {
        ipftps_perror(session, 550);
    }
    else
    {
#ifdef IP_PORT_VXWORKS
        ipftps_reply(session, "257 \"%s\" directory created"CL, argv0);
#else
        ipftps_reply(session, "257 \"%s\" directory created"CL, argv[0]);
#endif
    }
#ifdef IP_PORT_VXWORKS   
    ipftps_free(argv0); 
#endif
}


/*
 *===========================================================================
 *      ipftps_mode
 *===========================================================================
 *
 * Description:
 *      MODE command.
 *
 * Parameters:
 *      session Session descriptor.
 *      argc    Number of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 */
IP_STATIC void
ipftps_mode(Ipftps_session *session, int argc, char *argv[])
{
    char *mode = argv[0];

    (void)argc;

    if (ipcom_toupper(mode[0]) != 'S' || mode[1] != '\0')
    {
        ipftps_reply(session, "504 Only S supported"CL);
    }
    else
    {
        ipftps_reply(session, "200 Stream mode selected"CL);
    }
}


/*
 *===========================================================================
 *      ipftps_nlst
 *===========================================================================
 *
 * Description:
 *      NLST command.
 *
 * Parameters:
 *      session Session descriptor.
 *      argc    Number of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 */
IP_STATIC void
ipftps_nlst(Ipftps_session *session, int argc, char *argv[])
{
    struct Ip_stat st;
    char *path = argv[0];
    struct Ip_dirent *ent;
    char dot[IP_PATH_MAX];
    char *pat = IP_NULL;
    
    ipcom_strcpy(dot,".");

    if (argc == 0)
    {
        path = dot;
    }
    else if((ipcom_strncmp(path, "-l", 2) == 0) || 
           (ipcom_strncmp(path, "-al", 3) == 0)) 
    {
        ipftps_list(session, argc, argv);                 
        return;
    }

    if (ipftps_root_check(session, &path) != 0)
    {
        IPCOM_LOG2(WARNING, "Access denied to %s from %s",
                  path, session->cwd);
        ipftps_reply(session, "550 Permission denied"CL);
        return;
    }

    if (ipcom_stat(path, &st) != 0)
    {  
        /* Stat failed. Check for a wild cards in the path */   
        if((pat = ipftps_wildcard_check(path, &st)) == IP_NULL)                  
        {
           ipftps_perror(session, 550);    
           return;          
        }
    }
    
    /* Open data socket */
    if (ipftps_dsock_open(session, 0) != 0)
    {
        ipcom_free(pat);
        return;
    }

    /* Dir or file? */
    if (IP_S_ISDIR(st.st_mode))
    {
        IP_DIR *dir;

        dir = ipcom_opendir(path);
        if (dir == IP_NULL)
        {
            ipftps_perror(session, 550);
            ipftps_dsock_close(session);
            ipcom_free(pat);
            return;
        }

        /* Loop for files */
        while ((ent = ipcom_readdir(dir)) != 0)
        {
            if (ent->d_name[0] != '.')
            {
                if(pat == IP_NULL)
                {   
                    ipftps_reply_sock(session, session->dsock.fd,
                                  " %s"CL, ent->d_name);
                }  
                else
                {
                    if (ipftps_pattern_check(pat, ent->d_name))
                    ipftps_reply_sock(session, session->dsock.fd,
                                  " %s"CL, ent->d_name);
                } 
            }
        }

        if (ipcom_closedir(dir) != 0)
        {
            IPCOM_LOG2(WARNING, "Directory close failed for %s: %s",
                       path, ipcom_strerror(ipcom_errno));
        }
    }
    else
    {
        ipftps_reply_sock(session, session->dsock.fd, " %s%s"CL,
                          (session->cwdroot ? "/" : ""), path);
    }

    ipcom_free(pat); 
    ipftps_dsock_close(session);

    ipftps_reply(session, "226 Transfer complete"CL);
}


/*
 *===========================================================================
 *      ipftps_noop
 *===========================================================================
 *
 * Description:
 *      NOOP command.
 *
 * Parameters:
 *      session Session descriptor.
 *      argc    Number of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 */
IP_STATIC void
ipftps_noop(Ipftps_session *session, int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    ipftps_reply(session, "200 OK"CL);
}


/*
 *===========================================================================
 *      ipftps_pass
 *===========================================================================
 *
 * Description:
 *      PASS command.
 *
 * Parameters:
 *      session Session descriptor.
 *      argc    Number of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 */
IP_STATIC void
ipftps_pass(Ipftps_session *session, int argc, char *argv[])
{
    (void)argc;

    if (session->login != 0)
    {
        ipftps_reply(session, "503 Already logged in"CL);
        return;
    }

    if (session->last == IP_NULL ||
        ipcom_strcasecmp(session->last->name, "USER") != 0 ||
        session->username == IP_NULL)
    {
        ipftps_reply(session, "503 Login with USER first"CL);
        return;
    }

    if (ipftps_authenticate(session, argv[0]) != 0)
    {
        int secs;

        if ((secs = ipftps_sysget_int("ipftps.authsleep", 5, 1)) != 0)
        {
            (void)ipcom_sleep(secs);
        }

        ipftps_reply(session, (--session->authentications <= 0 ?
            "421 Service not available, closing control connection"CL :
            "530 Authorization failed"CL));
        return;
    }

    /* Change to selected directory */
    if (ipftps_chdir(session, session->cwd) != 0 ||
        ipftps_dir_check(session) != 0)
    {
        IPCOM_LOG1(ERR, "Invalid working directory: %s", session->cwd);
        ipftps_reply(session, "421 Service not available, "
                     "closing control connection"CL);
        session->authentications = 0;
        return;
    }

    session->login = 1;

    ipftps_reply(session, "230 Login OK"CL);
}


/*
 *===========================================================================
 *      ipftps_pasv
 *===========================================================================
 *
 * Description:
 *      PASV command.
 *
 * Parameters:
 *      session Session descriptor.
 *      argc    Number of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 */
IP_STATIC void
ipftps_pasv(Ipftps_session *session, int argc, char *argv[])
{
    int h[4],p[2];
    Ip_u32 v4addr;
    Ip_u16 v4port;

    (void)argc;
    (void)argv;

    if (session->epsvonly)
    {
        ipftps_reply(session, "501 In extended passive mode"CL);
        return;
    }

    IPCOM_SA_LEN_SET(&session->dsock.paddr.sin, sizeof(struct Ip_sockaddr_in));
    session->dsock.paddr.sin.sin_family = IP_AF_INET;

    if (ipftps_dsock_open(session, 1) != 0)
    {
        return;
    }

    v4addr = ip_ntohl(session->dsock.laddr.sin.sin_addr.s_addr);
    h[0] = (v4addr >> 24) & 0xFF;
    h[1] = (v4addr >> 16) & 0xFF;
    h[2] = (v4addr >> 8) & 0xFF;
    h[3] = v4addr & 0xFF;

    v4port = ip_ntohs(session->dsock.laddr.sin.sin_port);
    p[0] = (v4port >> 8) & 0xFF;
    p[1] = v4port & 0xFF;

    ipftps_reply(session,
                 "227 Entering passive mode (%d,%d,%d,%d,%d,%d)"CL,
                 h[0], h[1], h[2], h[3], p[0], p[1]);
}


/*
 *===========================================================================
 *      ipftps_port
 *===========================================================================
 *
 * Description:
 *      PORT command.
 *
 * Parameters:
 *      session Session descriptor.
 *      argc    Number of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 */
IP_STATIC void
ipftps_port(Ipftps_session *session, int argc, char *argv[])
{
    Ip_u32 h[6];
    int i;
    Ip_u32 addr;
    Ip_u16 port;

    (void)argc;

    if (session->epsvonly)
    {
        ipftps_reply(session, "501 In extended passive mode"CL);
        return;
    }

    session->passive = 0;

    for (i = 0; i < 6; i++)
    {
        char *stop;

        h[i] = ipcom_strtol(argv[i], &stop, 10);

        if (*stop != '\0' || h[i] > 255)
        {
            ipftps_reply(session, "501 Bad IP address or port"CL);
            return;
        }
    }

    addr = (h[0] << 24) | (h[1] << 16) | (h[2] << 8) | h[3];
    port = (Ip_u16) ((h[4] << 8) | h[5]);

    if (session->proxy == 0 &&
        (session->csock.paddr.sa.sa_family != IP_AF_INET ||
         addr != ip_ntohl(session->csock.paddr.sin.sin_addr.s_addr)))
    {
        char buf[IPFTPS_IPV4_MAXLEN];
        Ip_u32 u = ip_htonl(addr);

        (void)ipcom_inet_ntop(IP_AF_INET, &u, buf, sizeof(buf));
        IPCOM_LOG1(WARNING, "PORT IP address %s denied", buf);
        ipftps_reply(session, "504 Invalid host"CL);
        return;
    }

    if (port <= session->pportbase)
    {
        IPCOM_LOG1(WARNING, "PORT port number %lu denied", (Ip_u32)port);
        ipftps_reply(session, "504 Invalid port"CL);
        return;
    }

    IPCOM_SA_LEN_SET(&session->dsock.paddr.sin, sizeof(struct Ip_sockaddr_in));
    session->dsock.paddr.sin.sin_family = IP_AF_INET;
    session->dsock.paddr.sin.sin_addr.s_addr = ip_htonl(addr);
    session->dsock.paddr.sin.sin_port = ip_htons(port);

    ipftps_reply(session, "200 Port OK"CL);
}


/*
 *===========================================================================
 *      ipftps_pwd
 *===========================================================================
 *
 * Description:
 *      PWD command.
 *
 * Parameters:
 *      session Session descriptor.
 *      argc    Number of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 */
IP_STATIC void
ipftps_pwd(Ipftps_session *session, int argc, char *argv[])
{
    char *wd;
    char *path;
    char *cp;

    (void)argc;
    (void)argv;

    if ((path = wd = ipcom_strdup(session->cwd)) == IP_NULL)
    {
        IPCOM_LOG0(ERR, "Out of memory");
        ipftps_reply(session, "452 Out of memory"CL);
        return;
    }

    /* Use forward slashes */
    for (cp = path + ipcom_strcspn(path, IPFTPS_DIRSEP); *cp != '\0'; )
    {
        *cp++ = '/';
        cp += ipcom_strcspn(cp, IPFTPS_DIRSEP);
    }

    /* Paranoid check if path OK */
    if (ipftps_dircmp(session->root, path) != 0)
    {
        /*
        ** This is never supposed to happen, teardown
        ** session to prevent from security breaches.
        */
        IPCOM_LOG0(ERR, "Directory error, terminating session");
        ipftps_reply(session, "421 Service not available, "
                     "closing control connection"CL);
        session->authentications = 0;
    }
    else
    {
        int rlen = ipcom_strlen(session->root);
        int plen = ipcom_strlen(path);

        /* Advance past root part of path */
        path += IP_MIN(rlen, plen);

        if (path > wd && *path != '/')
        {
            *--path = '/';
        }

        ipftps_reply(session, "257 \"%s\" is current directory"CL, path);
    }

    ipcom_free(wd);
}


/*
 *===========================================================================
 *      ipftps_quit
 *===========================================================================
 *
 * Description:
 *      QUIT command.
 *
 * Parameters:
 *      session Session descriptor.
 *      argc    Number of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 */
IP_STATIC void
ipftps_quit(Ipftps_session *session, int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    session->login = 0;

    ipftps_reply(session, "221 Bye"CL);

    session->authentications = 0;
}


/*
 *===========================================================================
 *      ipftps_retr
 *===========================================================================
 *
 * Description:
 *      RETR command.
 *
 * Parameters:
 *      session Session descriptor.
 *      argc    Number of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 */
IP_STATIC void
ipftps_retr(Ipftps_session *session, int argc, char *argv[])
{
    char *msg = "226 Transfer complete"CL;
    char *nbuf = IP_NULL;
    char *buf;
    struct Ip_stat st;
    Ip_fd fd;
    int perrno = 0;
    int num, no;
    Ip_s32 mode;
    Ip_u32 chunks = 0;

    (void)argc;

    if (ipftps_root_check(session, &argv[0]) != 0)
    {
        IPCOM_LOG2(WARNING, "Access denied to %s from %s",
                   argv[0], session->cwd);
        ipftps_reply(session, "550 Permission denied"CL);
        goto leave5;
    }

    if ((buf = ipcom_malloc(IPFTPS_DATABUFSIZE)) == IP_NULL)
    {
        IPCOM_LOG0(ERR, "Out of memory");
        ipftps_reply(session, "452 Out of memory"CL);
        goto leave5;
    }

    if (session->binary == 0 &&
        (nbuf = ipcom_malloc(2 * IPFTPS_DATABUFSIZE)) == IP_NULL)
    {
        IPCOM_LOG0(ERR, "Out of memory");
        ipftps_reply(session, "452 Out of memory"CL);
        goto leave4;
    }

    mode = IP_O_RDONLY
#ifdef IP_O_BINARY
        | (session->binary ? IP_O_BINARY : 0)
#endif
        ;

    if ((fd = ipcom_fileopen(argv[0], mode, 0)) < 0)
    {
        ipftps_perror(session, 550);
        goto leave3;
    }

    if (ipcom_fstat(fd, &st) != 0)
    {
        ipftps_perror(session, 550);
        goto leave2;
    }

    if (IP_S_ISREG(st.st_mode) == 0)
    {
        ipftps_reply(session, "550 No regular file"CL);
        goto leave2;
    }

    if (ipftps_dsock_open(session, 0) != 0)
    {
        goto leave2;
    }

    session->trtotal = st.st_size;
    session->trnow = 0;

    for (;;)
    {
        static struct Ip_timeval timeval = {0, 0};
        Ip_fd_set cmdset;

#ifdef IPFTPS_USE_WRITESELECT
        Ip_fd_set dataset;

        timeval.tv_sec = ipftps_sysget_int("ipftps.send_timeout", 30, 1);
        timeval.tv_usec = 0;
#endif

        if ((no = ipcom_fileread(fd, buf, IPFTPS_DATABUFSIZE)) == 0)
        {
            goto leave1;
        }
        else if (no < 0)
        {
            perrno = 451;
            goto leave1;
        }

    again:
#ifdef IPFTPS_USE_WRITESELECT
        /* Wait until ready for write, or async command */
        IP_FD_ZERO(&cmdset);
        IP_FD_SET(session->csock.fd, &cmdset);
        IP_FD_ZERO(&dataset);
        IP_FD_SET(session->dsock.fd, &dataset);
        num = ipcom_socketselect(IP_MAX(session->csock.fd, session->dsock.fd) + 1,
                                 &cmdset, &dataset, IP_NULL, &timeval);
        if (num == -1)
        {
            perrno = 451;
            goto leave1;
        }
        else if (num > 0 && IP_FD_ISSET(session->csock.fd, &cmdset))
        {
            if (ipftps_async(session) != 0)
            {
                msg = "226 Abort OK"CL;
                goto leave1;
            }

            goto again;
        }
        else if (num == 0 || !IP_FD_ISSET(session->dsock.fd, &dataset))
        {
            msg = "426 Transfer timeout"CL;
            goto leave1;
        }
#else
        /* Check for async command */
        IP_FD_ZERO(&cmdset);
        IP_FD_SET(session->csock.fd, &cmdset);
        num = ipcom_socketselect(session->csock.fd + 1, &cmdset, IP_NULL, IP_NULL, &timeval);
        if (num == -1)
        {
            perrno = 451;
            goto leave1;
        }
        else if (num > 0 && IP_FD_ISSET(session->csock.fd, &cmdset))
        {
            if (ipftps_async(session) != 0)
            {
                msg = "226 Abort OK"CL;
                goto leave1;
            }

            goto again;
        }
#endif
        if (session->binary == 0)
        {
            if (ipftps_socketwrite_ascii(session->dsock.fd,
                                         buf, nbuf, no) != 0)
            {
                perrno = 451;
                goto leave1;
            }
        }
        else if (ipcom_socketwrite(session->dsock.fd, buf, no) != no)
        {
            perrno = 451;
            goto leave1;
        }

        session->trnow += no;
        chunks++;
    }

 leave1:
    IPCOM_LOG2(DEBUG2, "Sent %lu bytes in %lu chunks",
               (unsigned long)session->trnow, (unsigned long)chunks);
    (void)chunks;

    /* Preserve ipcom_errno */
    no = ipcom_errno;
    ipftps_dsock_close(session);
    ipcom_errno = no;

    if (perrno == 0)
    {
        ipftps_reply(session, msg);
    }
    else
    {
        ipftps_perror(session, perrno);
    }

 leave2:
    (void)ipcom_fileclose(fd);

 leave3:
    ipftps_free(nbuf);

 leave4:
    ipcom_free(buf);

 leave5:
    return;
}


/*
 *===========================================================================
 *      ipftps_rmd
 *===========================================================================
 *
 * Description:
 *      RMD command.
 *
 * Parameters:
 *      session Session descriptor.
 *      argc    Number of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 */
IP_STATIC void
ipftps_rmd(Ipftps_session *session, int argc, char *argv[])
{
    (void)argc;

    if (session->readonly)
    {
        IPCOM_LOG0(WARNING, "Remove dir denied");
        ipftps_reply(session, "550 Permission denied"CL);
    }
    else if (ipftps_root_check(session, &argv[0]) != 0)
    {
        IPCOM_LOG2(WARNING, "Access denied to %s from %s",
                   argv[0], session->cwd);
        ipftps_reply(session, "550 Permission denied"CL);
    }
    else if (ipcom_rmdir(argv[0]) != 0)
    {
        ipftps_perror(session, 550);
    }
    else
    {
        ipftps_reply(session, "250 Directory removed"CL);
    }
}


/*
 *===========================================================================
 *      ipftps_rnfr
 *===========================================================================
 *
 * Description:
 *      RNFR command.
 *
 * Parameters:
 *      session Session descriptor.
 *      argc    Number of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 */
IP_STATIC void
ipftps_rnfr(Ipftps_session *session, int argc, char *argv[])
{
    (void)argc;

    ipftps_free(session->rnfr);

    if (session->readonly)
    {
        IPCOM_LOG0(WARNING, "Rename file denied");
        ipftps_reply(session, "550 Permission denied"CL);
    }
    else if (ipftps_root_check(session, &argv[0]) != 0)
    {
        IPCOM_LOG2(WARNING, "Access denied to %s from %s",
                   argv[0], session->cwd);
        ipftps_reply(session, "550 Permission denied"CL);
    }
    else if (ipcom_access(argv[0], 0) != 0)
    {
        ipftps_perror(session, 550);
    }
    else if ((session->rnfr = ipcom_strdup(argv[0])) == IP_NULL)
    {
        IPCOM_LOG0(ERR, "Out of memory");
        ipftps_reply(session, "452 Out of memory"CL);
    }
    else
    {
        ipftps_reply(session, "350 Continue with RNTO"CL);
    }
}


/*
 *===========================================================================
 *      ipftps_rnto
 *===========================================================================
 *
 * Description:
 *      RNTO command.
 *
 * Parameters:
 *      session Session descriptor.
 *      argc    Number of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 */
IP_STATIC void
ipftps_rnto(Ipftps_session *session, int argc, char *argv[])
{
    (void)argc;

    if (session->readonly)
    {
        IPCOM_LOG0(WARNING, "Rename file denied");
        ipftps_reply(session, "550 Permission denied"CL);
    }
    else if (session->last == IP_NULL ||
             ipcom_strcasecmp(session->last->name, "RNFR") != 0 ||
             session->rnfr == IP_NULL)
    {
        ipftps_reply(session, "503 Missing RNFR"CL);
    }
    else if (ipftps_root_check(session, &argv[0]) != 0)
    {
        IPCOM_LOG2(WARNING, "Access denied to %s from %s",
                  argv[0], session->cwd);
        ipftps_reply(session, "550 Permission denied"CL);
    }
    else if (ipcom_rename(session->rnfr, argv[0]) != 0)
    {
        ipftps_perror(session, 553);
    }
    else
    {
        ipftps_free(session->rnfr);
        ipftps_reply(session, "250 File renamed"CL);
    }
}


/*
 *===========================================================================
 *      ipftps_stat
 *===========================================================================
 *
 * Description:
 *      STAT command.
 *
 * Parameters:
 *      session Session descriptor.
 *      argc    Number of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 */
IP_STATIC void
ipftps_stat(Ipftps_session *session, int argc, char *argv[])
{
    if (argc == 0)
    {
        char buf[IPFTPS_IP_MAXLEN];

        (void)ipftps_ntop(&session->csock.laddr, buf, sizeof(buf));
        ipftps_reply(session, "211- Session status:"CL
                     " Server: %s port %d"CL, buf,
                     (unsigned)ip_ntohs(IPFTPS_PORT(&session->csock.laddr)));

        (void)ipftps_ntop(&session->csock.paddr, buf, sizeof(buf));
        ipftps_reply(session,
                     " Client: %s port %d"CL, buf,
                     (unsigned)ip_ntohs(IPFTPS_PORT(&session->csock.paddr)));

        ipftps_reply(session,
                     " Type: %s"CL
                     " Mode: Stream"CL
                     " Structure: File"CL
                     " Readonly: %s"CL
                     " Proxy FTP: %s"CL
                     "211 End of status"CL,
                     (session->binary ? "Binary" : "Ascii"),
                     (session->readonly ? "Yes" : "No"),
                     (session->proxy ? "Yes" : "No"));
    }
    else
    {
        struct Ip_stat st;
        char *path = argv[0];
        struct Ip_dirent *ent;

        if (ipftps_root_check(session, &path) != 0)
        {
            IPCOM_LOG2(WARNING, "Access denied to %s from %s",
                       path, session->cwd);
            ipftps_reply(session, "550 Permission denied"CL);
            return;
        }

        if (ipcom_stat(path, &st) != 0)
        {
            ipftps_perror(session, 550);
            return;
        }

        /* Dir or file? */
        if (IP_S_ISDIR(st.st_mode))
        {
            IP_DIR *dir;
            int ret = 0;

            if ((dir = ipcom_opendir(path)) == IP_NULL)
            {
                ipftps_perror(session, 550);
                return;
            }

            /* Loop for files */
            ipftps_reply(session, "212- Directory status"CL);
            while ((ent = ipcom_readdir(dir)) != 0)
            {
                if (ent->d_name[0] != '.')
                {
                    if ((ret = ipftps_ls(session, session->csock.fd, path, ent->d_name)) != 0)
                    {
                        break;
                    }
                }
            }

            if (ret == 0)
            {
                ipftps_reply(session, "212 Transfer complete"CL);
            }
            
            if (ipcom_closedir(dir) != 0)
            {
                IPCOM_LOG2(WARNING, "Directory close failed for %s: %s",
                           path, ipcom_strerror(ipcom_errno));
            }
        }
        else
        {
            ipftps_reply(session, "213- File status"CL);
            if (ipftps_ls(session, session->csock.fd, "", path) == 0)
            {
                ipftps_reply(session, "213 Transfer complete"CL);
            }
        }
    }
}


/*
 *===========================================================================
 *      ipftps_stor
 *===========================================================================
 *
 * Description:
 *      STOR command.
 *
 * Parameters:
 *      session Session descriptor.
 *      argc    Number of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 */
IP_STATIC void
ipftps_stor(Ipftps_session *session, int argc, char *argv[])
{
    char *msg = "226 Transfer complete"CL;
    char *nbuf = IP_NULL;
    char *buf;
    struct Ip_stat st;
    Ip_fd fd;
    int perrno = 0;
    int num, no;
    Ip_bool orphant = 0;
    Ip_s32 mode;
    Ip_u32 chunks = 0;

    if (session->readonly)
    {
        IPCOM_LOG0(WARNING, "Store file denied");
        ipftps_reply(session, "550 Permission denied"CL);
        goto leave5;
    }

    if (ipftps_root_check(session, &argv[0]) != 0)
    {
        IPCOM_LOG2(WARNING, "Access denied to %s from %s",
                   argv[0], session->cwd);
        ipftps_reply(session, "550 Permission denied"CL);
        goto leave5;
    }

    if ((buf = ipcom_malloc(IPFTPS_DATABUFSIZE)) == IP_NULL)
    {
        IPCOM_LOG0(ERR, "Out of memory");
        ipftps_reply(session, "452 Out of memory"CL);
        goto leave5;
    }

    if (session->binary == 0 &&
        (nbuf = ipcom_malloc(IPFTPS_DATABUFSIZE + 1)) == IP_NULL)
    {
        IPCOM_LOG0(ERR, "Out of memory");
        ipftps_reply(session, "452 Out of memory"CL);
        goto leave4;
    }

    /*
     * Remove file before opening it to simulate O_TRUNC,
     * which isn't supported by IPCOM.
     */
    (void)ipcom_unlink(argv[0]);

    mode = IP_O_RDWR | IP_O_CREAT | (argc > 1 ? IP_O_APPEND : 0)
#ifdef IP_O_BINARY
        | (session->binary ? IP_O_BINARY : 0)
#endif
        ;

    if ((fd = ipcom_fileopen(argv[0], mode,
              IP_S_IRUSR | IP_S_IWUSR | IP_S_IRGRP | IP_S_IROTH)) < 0)
    {
        ipftps_perror(session, 550);
        goto leave3;
    }

    if (ipcom_fstat(fd, &st) != 0)
    {
        ipftps_perror(session, 550);
        goto leave2;
    }

    if (ipftps_dsock_open(session, 0) != 0)
    {
        goto leave2;
    }

    session->trtotal = 0;
    session->trnow = 0;

    for (;;)
    {
        struct Ip_timeval timeval;
        Ip_fd_set set;

        IP_FD_ZERO(&set);
        IP_FD_SET(session->csock.fd, &set);
        IP_FD_SET(session->dsock.fd, &set);
        timeval.tv_sec = ipftps_sysget_int("ipftps.receive_timeout", 30, 1);
        timeval.tv_usec = 0;
        num = ipcom_socketselect(IP_MAX(session->csock.fd,
                                        session->dsock.fd) + 1,
                                 &set, IP_NULL, IP_NULL, &timeval);
        if (num == -1)
        {
            perrno = 451;
            goto leave1;
        }
        else if (num > 0 && IP_FD_ISSET(session->csock.fd, &set))
        {
            if (ipftps_async(session) != 0)
            {
                msg = "226 Abort OK"CL;
                goto leave1;
            }

            continue;
        }
        else if (num == 0 || !IP_FD_ISSET(session->dsock.fd, &set))
        {
            msg = "426 Transfer timeout"CL;
            goto leave1;
        }

        if ((no = ipcom_socketread(session->dsock.fd, buf,
                                   IPFTPS_DATABUFSIZE)) == 0)
        {
            goto leave1;
        }
        else if (no < 0)
        {
            perrno = 451;
            goto leave1;
        }

        if (session->binary == 0)
        {
            if (ipftps_filewrite_ascii(fd, buf, nbuf, no, &orphant) != 0)
            {
                perrno = 451;
                goto leave1;
            }
        }
        else if (ipcom_filewrite(fd, buf, no) != no)
        {
            perrno = 451;
            goto leave1;
        }

        session->trnow += no;
        chunks++;
    }

 leave1:
    /* Write orphant '\r' */
    if (orphant && perrno == 0 && ipcom_filewrite(fd, "\r", 1) != 1)
    {
        perrno = 451;
    }

    IPCOM_LOG2(DEBUG2, "Received %lu bytes in %lu chunks",
               (unsigned long)session->trnow, (unsigned long)chunks);
    (void)chunks;

    /* Preserve ipcom_errno */
    no = ipcom_errno;
    ipftps_dsock_close(session);
    ipcom_errno = no;

    if (perrno == 0)
    {
        ipftps_reply(session, msg);
    }
    else
    {
        ipftps_perror(session, perrno);
    }

 leave2:
    (void)ipcom_fileclose(fd);

 leave3:
    ipftps_free(nbuf);

 leave4:
    ipcom_free(buf);

 leave5:
    return;
}


/*
 *===========================================================================
 *      ipftps_stru
 *===========================================================================
 *
 * Description:
 *      STRU command.
 *
 * Parameters:
 *      session Session descriptor.
 *      argc    Number of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 */
IP_STATIC void
ipftps_stru(Ipftps_session *session, int argc, char *argv[])
{
    char *stru = argv[0];

    (void)argc;

    if (ipcom_toupper(stru[0]) != 'F' || stru[1] != '\0')
    {
        ipftps_reply(session, "504 Only F supported"CL);
    }
    else
    {
        ipftps_reply(session, "200 File structure selected"CL);
    }
}


/*
 *===========================================================================
 *      ipftps_syst
 *===========================================================================
 *
 * Description:
 *      SYST command.
 *
 * Parameters:
 *      session Session descriptor.
 *      argc    Number of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 */
IP_STATIC void
ipftps_syst(Ipftps_session *session, int argc, char *argv[])
{
    char ver[16];
    char sys[16];
    Ip_size_t versize = sizeof(ver);
    Ip_size_t syssize = sizeof(sys);

    (void)argc;
    (void)argv;

    ipftps_reply(session, "215 %s Type: L8 Version: IPFTPS %s"CL,
                 ipftps_sysget_str("ipftps.system", sys, &syssize, "UNIX", 1),
                 ipftps_sysget_str("ipftps.versionstr", ver, &versize, "-", 1));
}


/*
 *===========================================================================
 *      ipftps_type
 *===========================================================================
 *
 * Description:
 *      TYPE command.
 *
 * Parameters:
 *      session Session descriptor.
 *      argc    Number of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 */
IP_STATIC void
ipftps_type(Ipftps_session *session, int argc, char *argv[])
{
    char *space = " ";

    /* Dummy space to facilitate display */
    if (argc < 2)
    {
        argv[1] = space;
        space = "";
    }

    switch (ipcom_toupper(argv[0][0]))
    {
    case 'A':
        if (ipcom_strchr("NT ", ipcom_toupper(argv[1][0])) == IP_NULL)
        {
            goto leave;
        }
        session->binary = 0;
        break;
    case 'I':
        if (argc > 1)
        {
            goto leave;
        }
        session->binary = 1;
        break;
    case 'L':
        if (ipcom_toupper(argv[1][0]) != '8')
        {
            goto leave;
        }
        session->binary = 1;
        break;
    default:
        goto leave;
    }

    ipftps_reply(session, "200 Type %c%s%c%sselected"CL,
                 argv[0][0], space, argv[1][0], space);
    return;

 leave:
    ipftps_reply(session, "504 Only TYPE A [N|T], I and L 8 supported"CL);
}


/*
 *===========================================================================
 *      ipftps_user
 *===========================================================================
 *
 * Description:
 *      USER command.
 *
 * Parameters:
 *      session Session descriptor.
 *      argc    Number of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 */
IP_STATIC void
ipftps_user(Ipftps_session *session, int argc, char *argv[])
{
    (void)argc;

    if (session->login != 0)
    {
        ipftps_reply(session, "503 Cannot change user"CL);
        return;
    }

    ipftps_free(session->username);

    if ((session->username = ipcom_strdup(argv[0])) == IP_NULL)
    {
        IPCOM_LOG0(ERR, "Out of memory");
        ipftps_reply(session, "452 Out of memory"CL);
        return;
    }

    ipftps_reply(session, "331 Password required for user %s"CL, argv[0]);
}


/*
 *===========================================================================
 *      ipftps_xftp
 *===========================================================================
 *
 * Description:
 *      XFTP command.
 *
 *      WARNING--This function is designed to be used for test
 *      purposes and contains a number of commands that may be
 *      used to compromise security. It is of utmost importance
 *      that this function is disabled before deploying the FTP
 *      server. This is done by not defining IPFTPS_USE_TEST.
 *
 * Parameters:
 *      session Session descriptor.
 *      argc    Number of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 */
#ifdef IPFTPS_USE_TEST
IP_STATIC void
ipftps_xftp(Ipftps_session *session, int argc, char *argv[])
{
    char *path = argv[1];
    
    IPCOM_LOG0(EMERG,
              "**** SECURITY HAZARD - TEST FUNCTIONALITY ENABLED ****");

    if (ipcom_strcmp(argv[0], "creat") == 0)
    {
        int mode = IP_O_RDWR | IP_O_CREAT
#ifdef IP_O_BINARY
            | IP_O_BINARY
#endif
            ;
        Ip_fd fd;

#ifdef IP_PORT_VXWORKS        
    char *fullPath = IP_NULL;
    
    if(argv[1])
    {
        if ((fullPath = ipcom_malloc(IP_PATH_MAX)) == IP_NULL)
        {
            IPCOM_LOG0(ERR, "Out of memory");
            ipftps_reply(session, "452 Out of memory"CL);
            return;
        }
        ipcom_strcpy(fullPath, argv[1]);
    
        /* Use the absolute path for vxworks */
        if (pathCat(session->cwd, path, fullPath)  == ERROR)
           goto leave1; 
    	pathCondense(fullPath);  
        path = fullPath;
    }
#endif

        if ((fd = ipcom_fileopen(path, mode,
                                 IP_S_IRUSR | IP_S_IWUSR |
                                 IP_S_IRGRP | IP_S_IROTH)) < 0)
        {
            ipftps_perror(session, 550);
            goto leave1;
        }
        else
        {
            static const char data[] = "\r\n\n\n\r\r\n";
            int datalen = strlen(data);
            int len = ipcom_strtol(argv[2], 0, 10);
            int i;

            for (i = 0; i < len; i += datalen)
            {
                int writelen = IP_MIN(datalen, len - i);

                if (ipcom_filewrite(fd, data, writelen) != writelen)
                {
                    ipftps_perror(session, 451);
                    (void)ipcom_fileclose(fd);
leave1:
#ifdef IP_PORT_VXWORKS            
            ipftps_free(fullPath);
#endif            
                    return;
                }
            }

            (void)ipcom_fileclose(fd);
        }
    }
    else if (ipcom_strcmp(argv[0], "connect") == 0)
    {
        Ipftps_sockcb cb;
        int option = 1;

        ipcom_memset(&cb, 0, sizeof(cb));
        cb.fd = -1;
        cb.laddr.sa.sa_family = IP_AF_INET;
        cb.paddr.sa.sa_family = IP_AF_INET;

        if (ipcom_inet_pton(IP_AF_INET, argv[1],
                            &cb.laddr.sin.sin_addr) != 1 ||
            ipcom_inet_pton(IP_AF_INET, argv[2],
                            &cb.paddr.sin.sin_addr) != 1)
        {
            ipftps_reply(session, "501 Address error"CL);
            return;
        }

        cb.paddr.sin.sin_port = (Ip_u16)ipcom_strtol(argv[3], IP_NULL, 10);
        cb.paddr.sin.sin_port = ip_htons(cb.paddr.sin.sin_port);

        if ((cb.fd = ipcom_socket(IP_AF_INET, IP_SOCK_STREAM, IPFTPS_PROTO)) < 0 ||
            ipcom_setsockopt(cb.fd, IP_SOL_SOCKET, IP_SO_REUSEADDR,
                             &option, sizeof(option)) < 0 ||
            ipcom_bind(cb.fd, &cb.laddr.sa,
                       IPFTPS_SOCKADDRLEN(&cb.laddr)) < 0 ||
            ipcom_connect(cb.fd, &cb.paddr.sa,
                          IPFTPS_SOCKADDRLEN(&cb.paddr)) < 0)
        {
            ipftps_reply(session, "451 Socket error"CL);
        }
        else
        {
            ipftps_reply(session, "200 OK"CL);
        }

        if (cb.fd != -1)
        {
            ipcom_socketclose(cb.fd);
        }

        return;
    }
    else if (ipcom_strcmp(argv[0], "root") == 0)
    {
#ifdef IP_PORT_VXWORKS        
        if (session->cwd != session->root)
        {
            ipftps_free(session->root);
        }
        if(argc == 2)
        {
            ipcom_strcpy(session->cwd, argv[1]);
        }
        else
        {
            ipcom_strcpy(session->cwd, "/");
        }
        session->root = ipcom_strdup(argc == 2 ? argv[1] : "/");
#else 
        if (session->cwd != session->root)
        {
            ipftps_free(session->cwd);
        }
        ipftps_free(session->root);
        session->cwd = session->root = ipcom_strdup(argc == 2 ? argv[1] : "/");
#endif
    }
#ifdef IPCOM_USE_TIME
    else if (ipcom_strcmp(argv[0], "date") == 0)
    {
        char timestr[16];
        Ip_time_t now;
        struct Ip_tm tm;

        (void)ipcom_time(&now);
        (void)ipcom_localtime_r(&now, &tm);
        (void)ipcom_strftime(timestr, sizeof(timestr), "%b %d %H:%M", &tm);
        ipftps_reply(session, " %s"CL, timestr);
    }
#endif
    else if (ipcom_strcmp(argv[0], "readonly") == 0)
    {
        session->readonly = argv[1][0] - '0';
    }
    else if (ipcom_strcmp(argv[0], "proxy") == 0)
    {
        session->proxy = argv[1][0] - '0';
    }
    else if (ipcom_strcmp(argv[0], "lportbase") == 0)
    {
        session->lportbase = (Ip_u16)ipcom_strtol(argv[1], IP_NULL, 10);
    }
    else if (ipcom_strcmp(argv[0], "pportbase") == 0)
    {
        session->pportbase = (Ip_u16)ipcom_strtol(argv[1], IP_NULL, 10);
    }
    else
    {
        ipftps_reply(session, "501 Syntax error"CL);
    }

    ipftps_reply(session, "200 OK"CL);
}
#endif


/*
 *===========================================================================
 *      ipftps_ntop
 *===========================================================================
 *
 * Description:
 *      Print IP address using ntop.
 *
 * Parameters:
 *      addr    IP address.
 *      dest    Result buffer.
 *      len     Size of result buffer.
 *
 * Returns:
 *      Pointer to buffer if success, 0 otherwise.
 */
IP_STATIC const char *
ipftps_ntop(union Ip_sockaddr_union *addr, char *dest, int len)
{
    switch (addr->sa.sa_family)
    {
#ifdef IPCOM_USE_INET
    case IP_AF_INET:
        return ipcom_inet_ntop(addr->sa.sa_family,
                               &addr->sin.sin_addr, dest, len);
#endif
#ifdef IPCOM_USE_INET6
    case IP_AF_INET6:
        return ipcom_inet_ntop(addr->sa.sa_family,
                               &addr->sin6.sin6_addr, dest, len);
#endif

    default:
        ipcom_strcpy(dest, "?");
        break;
    }

    return (const char *)dest;
}


/*
 *===========================================================================
 *      ipftps_perror
 *===========================================================================
 *
 * Description:
 *      Send errno based error message to control socket.
 *
 * Parameters:
 *      session Session descriptor.
 *      no      FTP error number.
 *
 * Returns:
 *      Nothing.
 */
IP_STATIC void
ipftps_perror(Ipftps_session *session, int no)
{
    ipftps_reply(session, "%d %s"CL, no, ipcom_strerror(ipcom_errno));
}


/*
 *===========================================================================
 *      ipftps_dsock_open
 *===========================================================================
 *
 * Description:
 *      Open data socket.
 *
 * Parameters:
 *      session         Session descriptor.
 *      passive_open    True if opening a passive socket.
 *
 * Returns:
 *      0 if success, 1 otherwise.
 */

IP_STATIC int
ipftps_dsock_open(Ipftps_session *session, int passive_open)
{
    Ipftps_sockcb *dsock = &session->dsock;
    Ip_fd fd;
    int attempts;
    int num, eno = 0;
    int passive_accept = (passive_open == 0 && session->passive != 0);
    char buf[IPFTPS_IP_MAXLEN];
#ifdef IP_SO_LINGER
    struct Ip_linger ling;
#endif

    /* No active connections allowed when EPSV ALL is selected */
    if (session->epsvonly && passive_open == 0 && session->passive == 0)
    {
        ipftps_reply(session, "503 Missing EPSV after EPSV ALL"CL);
        goto leave2;
    }

    /* Handle passive mode */
    if (passive_accept)
    {
        struct Ip_timeval timeval;
        Ip_socklen_t len = sizeof(dsock->paddr);
        Ip_fd_set set;

        IP_FD_ZERO(&set);
        IP_FD_SET(session->dsock.fd, &set);
        timeval.tv_sec = ipftps_sysget_int("ipftps.receive_timeout", 30, 1);
        timeval.tv_usec = 0;
        num = ipcom_socketselect(session->dsock.fd + 1, &set,
                                 IP_NULL, IP_NULL, &timeval);
        if (num == -1)
        {
            eno = ipcom_errno;
            IPCOM_LOG1(ERR, "Cannot select on data socket: %s",
                       ipcom_strerror(eno));
            goto leave1;
        }
        else if (num == 0 || !IP_FD_ISSET(session->dsock.fd, &set))
        {
            IPCOM_LOG0(WARNING, "No connect from peer in passive mode");
            eno = 0;
            goto leave1;
        }

        fd = ipcom_accept(dsock->fd, &dsock->paddr.sa, &len);
        if (fd < 0)
        {
            eno = ipcom_errno;
            IPCOM_LOG2(ERR, "ipcom_accept() failed : %s [%d]",
                       ipcom_strerror(eno), eno);
            goto leave1;
        }

        /* Close original socket and replace with new */
        if (ipcom_socketclose(dsock->fd) != 0)
        {
            IPCOM_LOG1(ERR, "Cannot close socket: %s",
                       ipcom_strerror(ipcom_errno));
        }
        dsock->fd = fd;

        if (session->proxy == 0 &&
            IPFTPS_ADDRCMP(&dsock->paddr, &session->csock.paddr) != 0)
        {
            (void)ipftps_ntop(&dsock->paddr, buf, sizeof(buf));

            IPCOM_LOG1(WARNING, "Peer IP address %s denied", buf);
            ipftps_reply(session, "425 Invalid host"CL);
            eno = 0;
            goto leave1;
        }

        ipftps_reply(session, "125 Using existing data connection"CL);
    }
    else
    {
        int option;
        int port;

        if (dsock->fd != -1)
        {
            if (ipcom_socketclose(dsock->fd) != 0)
            {
                IPCOM_LOG1(ERR, "Cannot close socket: %s",
                           ipcom_strerror(ipcom_errno));
            }

            session->dsock.fd = -1;
            session->passive = 0;
        }

        if ((dsock->fd = ipcom_socket(dsock->paddr.sa.sa_family,
                                      IP_SOCK_STREAM, IPFTPS_PROTO)) < 0)
        {
            eno = ipcom_errno;
            IPCOM_LOG1(ERR, "Cannot allocate data socket: %s",
                       ipcom_strerror(eno));
            goto leave2;
        }

        option = 1;
        if (ipcom_setsockopt(dsock->fd, IP_SOL_SOCKET, IP_SO_REUSEADDR,
                             &option, sizeof(option)) < 0)
        {
            IPCOM_LOG1(WARNING, "Cannot reuse socket address: %s",
                       ipcom_strerror(ipcom_errno));
        }

#ifdef IP_SO_LINGER
        ling.l_onoff = 1;
        ling.l_linger = ipftps_sysget_int("ipftps.dsock_linger", 1, IP_NULL);
        if (ipcom_setsockopt(dsock->fd, IP_SOL_SOCKET, IP_SO_LINGER, &ling, sizeof(ling)) < 0)
        {
            IPCOM_LOG1(WARNING, "ipcom_setsockopt(IP_SO_LINGER) failed : %s",
                       ipcom_strerror(ipcom_errno));
        }
#endif /* IP_SO_LINGER */

        attempts = IPFTPS_BIND_ATTEMPTS;


        /* Loop for bind attempts */
        for (;;)
        {
            if (session->lportbase == 0)
            {
                /* Use port 20 */
                port = ipftps_sysget_int("ipftps.port_number", 21, 1) - 1;
            }
            else
            {
                /* Select random port above lportbase */
                port = (int)ipcom_random();
                port = ((unsigned long)port %
                        (65535U - session->lportbase)) +
                    session->lportbase + 1;
                ip_assert(port > session->lportbase);
                ip_assert(port <= 65535);
            }

            IPFTPS_PORT(&dsock->laddr) = (Ip_u16)ip_htons(port);

            if (ipcom_bind(dsock->fd, &dsock->laddr.sa,
                           IPFTPS_SOCKADDRLEN(&dsock->laddr)) < 0)
            {
                /* Reiterate if using random port numbers */
                if (session->lportbase != 0 && --attempts > 0)
                {
                    continue;
                }

                eno = ipcom_errno;
                IPCOM_LOG3(ERR, "Cannot bind data socket %s:%d: %s",
                           ipftps_ntop(&dsock->laddr, buf, sizeof(buf)), (int)port,
                           ipcom_strerror(eno));
                goto leave1;
            }

            break;
        }

#ifdef IPIPTOS_THROUGHPUT
        /* Setup for max throughput service */
        option = IPIPTOS_THROUGHPUT;
        if (ipcom_setsockopt(dsock->fd, IP_IPPROTO_IP, IPIP_TOS,
                             &option, sizeof(option)) < 0)
        {
            IPCOM_LOG1(WARNING,
                       "Cannot setup for max throughput service: %s",
                       ipcom_strerror(ipcom_errno));
        }
#endif

        /* Active or passive open */
        if (passive_open)
        {
            if (ipcom_listen(dsock->fd, 1) != 0)
            {
                eno = ipcom_errno;
                IPCOM_LOG1(ERR, "Cannot listen on data socket: %s",
                           ipcom_strerror(eno));
                goto leave1;
            }
            session->passive = 1;
        }
        else
        {
#ifdef IPFTPS_USE_IIS_150
            /*
             * The IIS FTP server sends the 150 message
             * before opening the data connection.
             */
            ipftps_reply(session, "150 Opening data connection"CL);
#endif
#ifdef IPCOM_USE_INET6
            if (dsock->paddr.sa.sa_family == IP_AF_INET6)
            {
                struct Ip_sockaddr_in6 laddr;
                Ip_socklen_t           laddr_len = sizeof(laddr);

                /* Get the scope from the local address */
                if (ipcom_getsockname(dsock->fd, (struct Ip_sockaddr *) &laddr, &laddr_len) < 0)
                {
                    eno = ipcom_errno;
                    IPCOM_LOG1(ERR, "Cannot get local address of socket: %s",
                               ipcom_strerror(eno));
                    goto leave1;
                }
                /* Use the same scope as the local address */
                dsock->paddr.sin6.sin6_scope_id = laddr.sin6_scope_id;
            }
#endif /* IPCOM_USE_INET6 */

            if (ipcom_connect(dsock->fd, &dsock->paddr.sa,
                              IPFTPS_SOCKADDRLEN(&dsock->paddr)) < 0)
            {
                eno = ipcom_errno;
                IPCOM_LOG1(ERR, "Cannot connect to data socket: %s",
                           ipcom_strerror(eno));
                goto leave1;
            }

#ifndef IPFTPS_USE_IIS_150
            ipftps_reply(session, "150 Opening data connection"CL);
#endif
        }
    }

    return 0;

 leave1:
    ipftps_dsock_close(session);

 leave2:
    if (eno != 0)
    {
        ipftps_reply(session, "425 %s"CL, ipcom_strerror(eno));
    }

    return 1;
}


/*
 *===========================================================================
 *      ipftps_dsock_close
 *===========================================================================
 *
 * Description:
 *      Close data socket.
 *
 * Parameters:
 *      session Session descriptor.
 *
 * Returns:
 *      Nothing.
 */
IP_STATIC void
ipftps_dsock_close(Ipftps_session *session)
{
    if (session->dsock.fd != -1 && ipcom_socketclose(session->dsock.fd) != 0)
    {
        IPCOM_LOG1(ERR, "Cannot close socket: %s",
                   ipcom_strerror(ipcom_errno));
    }

    /* Reset peer address to default */

    session->dsock.fd = -1;
    session->passive = 0;
}


/*
 *===========================================================================
 *      ipftps_write
 *===========================================================================
 *
 * Description:
 *      Write to socket, terminate session if control socket down.
 *
 * Parameters:
 *      session Session descriptor.
 *      fd      Socket descriptor.
 *      buf     Buffer.
 *      len     Buffer length.
 *
 * Returns:
 *      0 if success, 1 otherwise.
 */
IP_STATIC int
ipftps_write(Ipftps_session *session, Ip_fd fd, void *buf, int len)
{
    if (ipcom_socketwrite(fd, buf, len) != len)
    {
        IPCOM_LOG1(WARNING, "Socket write error: %s",
                   ipcom_strerror(ipcom_errno));

        /* Terminate session if control socket closed */
        if (fd == session->csock.fd)
        {
            session->authentications = 0;
        }
        return 1;
    }

    return 0;
}


/*
 *===========================================================================
 *      ipftps_reply_sock
 *===========================================================================
 *
 * Description:
 *      Reply to peer.
 *
 * Parameters:
 *      session Session descriptor.
 *      fd      Socket descriptor.
 *      fmt     Format string.
 *      ...     Additional parms.
 *
 * Returns:
 *      Nothing.
 */
IP_STATIC void
ipftps_reply_sock(Ipftps_session *session, Ip_fd fd, const char *fmt, ...)
{
    char buf[256];
    va_list argptr;
    int len;

    va_start(argptr, fmt);
    len = ipcom_vsnprintf(buf, sizeof(buf), fmt, argptr);
    va_end(argptr);

    if (len < 0)
    {
        IPCOM_LOG0(ERR, "Error in ipcom_vsnprintf(), out of memory?");
        ipcom_strcpy(buf, "452 Error in ipcom_vsnprintf(), out of memory?");
        len = ipcom_strlen(buf);
    }
    else if (len > sizeof(buf))
    {
        IPCOM_LOG0(WARNING, "Reply buffer overflow");
        buf[sizeof(buf) - 2] = '\r';
        buf[sizeof(buf) - 1] = '\n';
        len = sizeof(buf);
    }

    (void)ipftps_write(session, fd, buf, len);
}


/*
 *===========================================================================
 *      ipftps_reply
 *===========================================================================
 *
 * Description:
 *      Reply to the peer PI.
 *
 * Parameters:
 *      session Session descriptor.
 *      fmt     Format string.
 *      ...     Additional parms.
 *
 * Returns:
 *      Nothing.
 */
IP_STATIC void
ipftps_reply(Ipftps_session *session, const char *fmt, ...)
{
    char buf[256];
    va_list argptr;
    int len;

    va_start(argptr, fmt);
    len = ipcom_vsnprintf(buf, sizeof(buf), fmt, argptr);
    va_end(argptr);

    if (len < 0)
    {
        IPCOM_LOG0(ERR, "Error in ipcom_vsnprintf(), out of memory?");
        ipcom_strcpy(buf, "452 Error in ipcom_vsnprintf(), out of memory?");
        len = ipcom_strlen(buf);
    }
    else if (len > sizeof(buf))
    {
        IPCOM_LOG0(WARNING, "Reply buffer overflow");
        buf[sizeof(buf) - 2] = '\r';
        buf[sizeof(buf) - 1] = '\n';
        len = sizeof(buf);
    }

    (void)ipftps_write(session, session->csock.fd, buf, len);
}


/*
 *===========================================================================
 *      ipftps_telnet
 *===========================================================================
 *
 * Description:
 *      Handle telnet stuff.
 *
 * Parameters:
 *      session Session descriptor.
 *
 * Returns:
 *      0 if success, 1 otherwise.
 */
IP_STATIC int
ipftps_telnet(Ipftps_session *session)
{
    Ip_u8 *cp = (Ip_u8 *)(session->cmdbuf);
    int i = 0;

    /* Loop through entire command buffer */
    while (i < session->cmdbuflen)
    {
        int left = session->cmdbuflen - i;

        do
        {
            int skip = 0;
            int reply = 0;
            int j;

            /* Check for telnet */
            if (cp[0] == IPFTPS_DM)
            {
                /*
                ** This is a special case which will occur when
                ** inline handling of OOB data is missing. It
                ** seems a reasonable compromise to ignore all
                ** DMs in input even if not preceded by IAC.
                */
                skip = 1;
            }
            else if (cp[0] == IPFTPS_IAC && left >= 2)
            {
                switch (cp[1])
                {
                case IPFTPS_WILL:
                    if (left >= 3)
                    {
                        skip = 3;
                        reply = 1;
                        cp[1] = IPFTPS_DONT;
                    }
                    break;
                case IPFTPS_DO:
                    if (left >= 3)
                    {
                        skip = 3;
                        reply = 1;
                        cp[1] = IPFTPS_WONT;
                    }
                    break;
                case IPFTPS_WONT:
                case IPFTPS_DONT:
                    if (left >= 3)
                    {
                        skip = 3;
                    }
                    break;
                default:
                    skip = 2;
                    break;
                }

                /* Reply if negotiation detected */
                if (reply)
                {
                    (void)ipftps_write(session, session->csock.fd, cp, skip);
                }
            }

            /* Remove telnet stuff from command buffer */
            if (skip > 0)
            {
                for (j = 0; j < (left - skip); j++)
                {
                    cp[j] = cp[j + skip];
                }

                session->cmdbuflen -= skip;

                /* Continue without advancing indices */
                break;
            }

            i++;
            cp++;
        }
        while (0);
    }

    return 0;
}


/*
 *===========================================================================
 *      ipftps_get_command
 *===========================================================================
 *
 * Description:
 *      Get a complete command from the PI socket.
 *
 * Parameters:
 *      session Session descriptor.
 *
 * Returns:
 *      0 if success, 1 otherwise.
 */
IP_STATIC int
ipftps_get_command(Ipftps_session *session)
{
    int len;
    int num, i;

    /* Put possible last command first in buffer */
    for (i = 0; i < (session->cmdbuflen - session->cmdlen); i++)
    {
        session->cmdbuf[i] = session->cmdbuf[session->cmdlen + i];
    }
    session->cmdbuflen -= session->cmdlen;
    session->cmdlen = 0;

    /* Loop until command available */
    for (;;)
    {
        struct Ip_timeval timeval;
        Ip_fd_set set;

        timeval.tv_sec = ipftps_sysget_int("ipftps.session_timeout", 300, 1);
        timeval.tv_usec = 0;

        /* Handle telnet stuff */
        (void)ipftps_telnet(session);

        /* Check if there already is a pending command */
        for (i = 0; i < session->cmdbuflen; i++)
        {
            if (session->cmdbuf[i] == '\r')
            {
                if (i < (session->cmdbuflen - 1) &&
                    session->cmdbuf[i + 1] == '\n')
                {
                    session->cmdbuf[i] = '\0';
                    session->cmdlen = i + 2;
                    return 0;
                }
            }
        }

        /* Truncate if buffer is full but no command received */
        if (session->cmdbuflen >= IPFTPS_CMDBUFSIZE)
        {
            session->cmdbuf[session->cmdbuflen - 1] = '\0';
            session->cmdlen = session->cmdbuflen;
            return 0;
        }

        IP_FD_ZERO(&set);
        IP_FD_SET(session->csock.fd, &set);
        num = ipcom_socketselect(session->csock.fd + 1, &set,
                                 IP_NULL, IP_NULL, &timeval);
        if (num == -1)
        {
            IPCOM_LOG1(WARNING, "Cannot select on control socket: %s",
                       ipcom_strerror(ipcom_errno));
            break;
        }
        else if (num == 0 || !IP_FD_ISSET(session->csock.fd, &set))
        {
            IPCOM_LOG0(WARNING, "Session timeout");
            ipftps_reply(session, "421 Timeout, closing control connection"CL);
            break;
        }

        /* Get more data */
        if ((len = ipcom_socketread(session->csock.fd,
                                    session->cmdbuf + session->cmdbuflen,
                                    IPFTPS_CMDBUFSIZE - session->cmdbuflen)) == 0)
        {
            IPCOM_LOG0(INFO, "Connection closed by remote host");
            break;
        }
        else if (len < 0)
        {
            IPCOM_LOG1(WARNING, "Cannot read from control socket: %s",
                       ipcom_strerror(ipcom_errno));
            break;
        }

        session->cmdbuflen += len;
    }

    return 1;
}


/*
 *===========================================================================
 *      ipftps_parse_command
 *===========================================================================
 *
 * Description:
 *      Parse a command from the PI socket.
 *
 * Parameters:
 *      session Session descriptor.
 *
 * Returns:
 */
IP_STATIC int
ipftps_parse_command(Ipftps_session *session)
{
    char *argv[IPFTPS_MAX_ARGS];
    const Ipftps_cmd *cmd;
    char *stop;
    char *cp = session->cmdbuf;
    int i;
#ifdef IP_PORT_VXWORKS
    char *tmpPtr = IP_NULL;
    char *tmpPtr1 = IP_NULL;
#endif
 
    /* Ignore initial whitespace */
    cp += ipcom_strspn(cp, IPFTPS_DELIMITERS);
    if (*cp == '\0')
    {
        return 0;
    }

    /* Parse arguments */
    /* Get the command */
    for (i = 0; *cp != '\0' && i < 1; i++)
    {
        argv[i] = cp;
        cp += ipcom_strcspn(cp, IPFTPS_DELIMITERS);   
        stop = cp;
        cp += ipcom_strspn(cp, IPFTPS_DELIMITERS);
        *stop = '\0';
    }

    /* Scan through command list */
    for (cmd = ipftps_cmdlist; cmd->name != IP_NULL; cmd++)
    {
        if (ipcom_strcasecmp(cmd->name, argv[0]) == 0)
        {
            if(cmd->argtype == 0)            
            {
                for (i = i; *cp != '\0' && i < IPFTPS_MAX_ARGS; i++)
                {
                    argv[i] = cp;
                    cp += ipcom_strcspn(cp, IPFTPS_DELIMITERS);   
                    stop = cp;
                    cp += ipcom_strspn(cp, IPFTPS_DELIMITERS);
                    *stop = '\0';
                }
            }
            else 
            {   /* Command argument type is string */
                if (*cp != '\0')   
                {
                   argv[i] = cp;                        
                   i++; 
                }            
            }
            i--;

            /* Check if logged in */
            if (cmd->login != 0 && session->login == 0)
            {
                IPCOM_LOG1(WARNING, "Attempted %s without login",
                           cmd->name);
                ipftps_reply(session, "530 Login required"CL);
                return 0;
            }

            /* Check argument no */
            if (i < cmd->min || i > cmd->max)
            {
                ipftps_reply(session, "501 Invalid number of arguments"CL,
                             argv[0]);
                return 0;
            }

            session->cwdroot = 0;

#ifdef IP_PORT_VXWORKS 
if ((argv[1] != IP_NULL) && (i != 0))
    {
        tmpPtr = argv[1];
        argv[1] = ipcom_malloc(IP_PATH_MAX);
        tmpPtr1 = argv[1];  
        ipcom_strcpy(argv[1], tmpPtr);
    }
#endif
            /* Do command */
            cmd->func(session, i, argv + 1);
#ifdef IP_PORT_VXWORKS
    if (tmpPtr != IP_NULL)
    {
       ipcom_free(tmpPtr1); 
       argv[1] = tmpPtr; 
    }
#endif            
            session->last = cmd;

            return (session->authentications == 0 ? 1 : 0);
        }
    }

    ipftps_reply(session, "500 Invalid command: \"%s\""CL, argv[0]);

    return 0;
}


/*
 *===========================================================================
 *      ipftps_async
 *===========================================================================
 *
 * Description:
 *      Handle asynchronous commands during file transfer.
 *
 * Parameters:
 *      session Session descriptor.
 *
 * Returns:
 *      1 if an ABOR command is found, 0 otherwise.
 */
IP_STATIC int
ipftps_async(Ipftps_session *session)
{
    int closed = 0;
    int aborted = 0;
    int asynclen;
    int len;
    int i;

    /* Put possible last command first in buffer */
    for (i = 0; i < session->cmdlen; i++)
    {
        session->cmdbuf[i] = session->cmdbuf[session->cmdlen + i];
    }
    session->cmdbuflen -= session->cmdlen;
    session->cmdlen = 0;

    /* Get more data */
    if ((len = ipcom_socketread(session->csock.fd,
                                session->cmdbuf + session->cmdbuflen,
                                IPFTPS_CMDBUFSIZE - session->cmdbuflen)) > 0)
    {
        session->cmdbuflen += len;
    }
    else
    {
        IPCOM_LOG1(WARNING, "Cannot read from control socket: %s",
                   ipcom_strerror(ipcom_errno));
        closed = 1;
    }
    
    /* Handle telnet */
    (void)ipftps_telnet(session);

    /* Loop for multiple async commands */
    do
    {
        char *asyncpos;
        asynclen = 0;

#define IPFTPS_ABORLEN 6
#define IPFTPS_STATLEN 6

        /* Do a minimum parse */
        session->cmdbuf[session->cmdbuflen] = '\0';
        if ((asyncpos = ipcom_strstr(session->cmdbuf, "ABOR"CL)) != IP_NULL)
        {
            asynclen = IPFTPS_ABORLEN;
            aborted = 1;
        }
        else if ((asyncpos = ipcom_strstr(session->cmdbuf, "STAT"CL)) != IP_NULL)
        {
            asynclen = IPFTPS_STATLEN;

            if (session->trtotal == 0)
            {
                ipftps_reply(session, "213 Received %ld bytes"CL,
                             (long)session->trnow);
            }
            else
            {
                ipftps_reply(session, "213 Sent %ld of %ld bytes"CL,
                             (long)session->trnow, (long)session->trtotal);
            }
        }

        /*
        ** An async command that is handled above must be removed
        ** from the command buffer so it doesn't occur in the normal
        ** command stream later on.
        */
        if (asyncpos != IP_NULL)
        {
            for (i = (asyncpos - session->cmdbuf); i < (session->cmdbuflen - asynclen); i++)
            {
                session->cmdbuf[i] = session->cmdbuf[asynclen + i];
            }
        }

        session->cmdbuflen -= asynclen;
    }
    while (aborted == 0 && asynclen > 0);

    if (aborted)
    {
        ipftps_reply(session, "426 Transfer aborted"CL);
    }
    else if (closed)
    {
        ipftps_reply(session, "426 Control connection closed, transfer aborted"CL);
    }
    
    return aborted || closed;
}


/*
 *===========================================================================
 *      ipftps_chdir
 *===========================================================================
 *
 * Description:
 *      Change to the requested directory.
 *
 * Parameters:
 *      session Session descriptor.
 *      path    Path to change directory to
 *
 * Returns:
 *      Nothing.
 */
IP_STATIC int
ipftps_chdir(Ipftps_session *session, char *path)
{	
#ifdef IP_PORT_VXWORKS
 
    DIR *dir;
    char *cwd = IP_NULL;
    const char *pTail;
    
    /*
     * VxWorks filesystem does not check that the destination dir is valid--
     * this must be done by the clients.
     */
    if ((dir = opendir(path)) == NULL)
        return -1;

    (void)closedir(dir);
    
    /* 
     * Store the full path on a session by session basis for vxworks.
     * 
     * Since vxWorks is not a multi-user OS, changing directory via 
     * ipcom_chdir will change the cwd for every client connected
     * to the server in the same RTP.  To work around this limitation, we
     * store the full cwd for each session and use absolute paths when
     * issuing various commands (ie. mkdir, dele, etc).
     * 
     * We make use of the existing pathCat and pathCondense
     * routines to properly deal with path navigation elements 
     * ie '.' , '..' , '/../..', etc 
     */
	
    if ((cwd = ipcom_malloc(IP_PATH_MAX)) == NULL)
    {
        return -1;
    }
    
    if (ipcom_strcmp(session->cwd, path) != 0)
    {
        if (pathCat(session->cwd, path, cwd)  == ERROR)
        {
            ipftps_free(cwd);
            return -1; 
        }
        pathCondense(cwd);
    }
    else
    {
    	ipcom_strcpy(cwd, session->cwd);
        pathCondense(cwd);
    }

    if ((path[strlen(path) - 1] == '/') && strcmp(cwd, "/"))
    {
        ipcom_strcat(cwd, "/");
    }
    
    /* Make sure that new path starts with a device name */
    iosDevFind (cwd, &pTail);
    if (pTail != cwd)
    {
        ipcom_strcpy(session->cwd, cwd);
        ipftps_free(cwd);
        return 0;
    }

    if (path[strlen(path) - 1] == '.')
    {
        ipcom_strcat(cwd, "/");
        iosDevFind (cwd, &pTail);
        if (pTail != cwd)
        {
            ipcom_strcpy(session->cwd, cwd);
            ipftps_free(cwd);            
            return 0;
        }
    }

    ipftps_free(cwd);
    return -1; 
#else
	
    if (ipcom_chdir(path) != 0)
    {
        return -1;
    }

#endif
    
    return 0;
}


/*
 *===========================================================================
 *      ipftps_allo
 *===========================================================================
 *
 * Description:
 *      ALLO command.
 *
 * Parameters:
 *      session Session descriptor.
 *      argc    Number of arguments.
 *      argv    Argument vector.
 *
 * Returns:
 */
IP_STATIC void
ipftps_allo(Ipftps_session *session, int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    ipftps_reply(session, "202 Command not implemented"CL);
}

/*
 ****************************************************************************
 * 10                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *      ipftps_session
 *===========================================================================
 *
 * Description:
 *      FTP session handler subprocess.
 *
 * Parameters:
 *
 * Returns:
 */
IPCOM_PROCESS(ipftps_session)
{
    char buf[IPFTPS_IP_MAXLEN];
    Ipftps_session *session = IP_NULL;
    char ver[16];
    Ip_size_t versize = sizeof(ver);
    Ipcom_ipc ipc;
    int retries = 5;
    int retry_time = 1;
    Ip_size_t cwdsize = IP_PATH_MAX;

    ipcom_proc_init();

    /* Get session info from main */
    if (ipcom_ipc_open(&ipc, "ipftps", -1) != IPCOM_SUCCESS)
    {
        IPCOM_LOG0(ERR, "Failed to open IPC channel to 'ftps'");
        goto leave;
    }

    /* Try getting memory a couple of times to avoid locking the main process */
    while (--retries > 0 &&
           (session = ipcom_ipc_malloc(1)) == IP_NULL)
    {
        IPCOM_LOG0(ERR, "Failed to allocate IPC buffer, retry");
        (void)ipcom_sleep(retry_time);
        retry_time *= 2;
    }
    if (session == IP_NULL)
    {
        goto leave;
    }

    if (ipcom_ipc_send(&ipc, session) != IPCOM_SUCCESS)
    {
        IPCOM_LOG0(ERR, "Failed to send IPC buffer");
        goto leave;
    }
    if (ipcom_ipc_receive(IP_NULL, &session, -1) != IPCOM_SUCCESS)
    {
        IPCOM_LOG0(ERR, "Failed to receive IPC buffer");
        goto leave;
    }
#ifdef IP_PORT_OSE5
    session->csock.fd = efs_receive_fd(0);
    if (session->csock.fd == -1)
    {
        IPCOM_LOG0(ERR, "efs_receive_fd() failed");
        goto leave;
    }
#endif
    if (ipcom_ipc_close(&ipc) != IPCOM_SUCCESS)
    {
        IPCOM_LOG0(ERR, "Failed to close IPC channel");
        goto leave;
    }

    (void)ipftps_ntop(&session->csock.paddr, buf, sizeof(buf));
    IPCOM_LOG1(INFO, "Session handler start for %s", buf);

    /* Use control socket as default template for data socket */
    ipcom_memcpy(&session->dsock, &session->csock, sizeof(session->dsock));
    session->dsock.fd = -1;

    /* Set session defaults */
    session->readonly = ipftps_sysget_int("ipftps.readonly", 0, 1);
    session->proxy = ipftps_sysget_int("ipftps.proxy", 0, 1);
    session->pportbase = (Ip_u16)ipftps_sysget_int("ipftps.pportbase", 49151, 1);
    session->lportbase = (Ip_u16)ipftps_sysget_int("ipftps.lportbase", 1023, 1);
    session->authentications = ipftps_sysget_int("ipftps.authentications", 3, 1);
    session->root = ipftps_sysget_str("ipftps.root", IP_NULL, IP_NULL, "/", 1);
    
    session->cwd = ipcom_malloc(IP_PATH_MAX);    
    (void)ipftps_sysget_str("ipftps.dir", session->cwd, &cwdsize,
                                     IPCOM_FILE_ROOT, 1);

#ifdef IP_SO_OOBINLINE
    {
        int option = 1;

        /* Inline handling of OOB data */
        if (ipcom_setsockopt(session->csock.fd, IP_SOL_SOCKET,
                             IP_SO_OOBINLINE, &option, sizeof(option)) < 0)
        {
            IPCOM_LOG1(WARNING, "Cannot setup inline OOB data: %s",
                       ipcom_strerror(ipcom_errno));
        }
    }
#endif

#ifdef IPIPTOS_LOWDELAY
    {
        int option = IPIPTOS_LOWDELAY;

        /* Setup for low delay service */
        if (ipcom_setsockopt(session->csock.fd, IP_IPPROTO_IP,
                             IPIP_TOS, &option, sizeof(option)) < 0)
        {
            IPCOM_LOG1(WARNING, "Cannot setup for low delay service: %s",
                       ipcom_strerror(ipcom_errno));
        }
    }
#endif

    ipftps_reply(session, "220 Wind River FTP server %s ready."CL,
                 ipftps_sysget_str("ipftps.versionstr", ver, &versize, "-", 1));

    /* Command loop */
    while (ipftps_get_command(session) == 0 &&
           ipftps_parse_command(session) == 0)
    {
    }

leave:
    IPCOM_LOG0(INFO, "Session handler stop");

    /* Cleanup */
    if (session != IP_NULL)
    {
        /* Close data socket */
        ipftps_dsock_close(session); 

        /* Close control socket */
        if (session->csock.fd != IP_INVALID_SOCKET &&
            ipcom_socketclose(session->csock.fd) != 0)
        {
            IPCOM_LOG1(ERR, "Cannot close socket: %s",
                       ipcom_strerror(ipcom_errno));
        }

        /* Free session descriptor */
        if (session->cwd != session->root)
        {
            ipftps_free(session->cwd);
        }
        ipftps_free(session->root);
        ipftps_free(session->username);
        ipftps_free(session->rnfr);
        ipcom_ipc_free(session);
    }

    if (ipcom_ipc_isopen(&ipc))
    {
        (void)ipcom_ipc_close(&ipc);
    }

    /*!!race Decrement number of sessions. */
    if (--ipftps.active_sessions < 0)
        ipftps.active_sessions = 0;

    ipcom_proc_exit();
}


/*
 *===========================================================================
 *      ipftps_main
 *===========================================================================
 *
 * Description:
 *      FTP main process.
 *
 * Parameters:
 *
 * Returns:
 */
IPCOM_PROCESS(ipftps_main)
{
    Ipftps_session *session = IP_NULL;
    Ip_fd ipd_fd = IP_INVALID_SOCKET;
    char bind_str[IPFTPS_IP_MAXLEN];
    Ip_size_t bind_size;
    int ipd_init = 0;
    int option;
    Ip_u16 port_number;
    Ip_pid_t  pid;

    IPCOM_LOG0(INFO, "Starting server daemon");

    /* Init data */
    ipcom_proc_init();
    ipcom_memset(&ipftps, 0, sizeof(ipftps));
#ifdef IPCOM_USE_INET
    ipftps.sock4.fd = IP_INVALID_SOCKET;
#endif
#ifdef IPCOM_USE_INET6
    ipftps.sock6.fd = IP_INVALID_SOCKET;
#endif

    if (ipcom_ipc_install() != IPCOM_SUCCESS)
    {
        IPCOM_LOG1(ERR, "Cannot install IPC service: %s",
                   ipcom_strerror(ipcom_errno));
        goto leave;
    }

reconfigure:
    port_number = (Ip_u16)ipftps_sysget_int("ipftps.port_number", 21, 1);

#ifdef IPCOM_USE_INET
    /* Get IPv4 socket */
    ipftps.sock4.fd = ipcom_socket(IP_AF_INET, IP_SOCK_STREAM, IPFTPS_PROTO);
    if (ipftps.sock4.fd == IP_INVALID_SOCKET)
    {
        IPCOM_LOG1(ERR, "Cannot allocate IPv4 server socket: %s",
                   ipcom_strerror(ipcom_errno));
        goto done4;
    }

    option = 1;
    if (ipcom_setsockopt(ipftps.sock4.fd, IP_SOL_SOCKET, IP_SO_REUSEADDR,
                         &option, sizeof(option)) < 0)
    {
        IPCOM_LOG1(WARNING, "Cannot reuse IPv4 server socket address: %s",
                   ipcom_strerror(ipcom_errno));
    }

    /* Set IPv4 defaults */
    IPCOM_SA_LEN_SET(&ipftps.sock4.laddr.sin, sizeof(struct Ip_sockaddr_in));
    ipftps.sock4.laddr.sin.sin_family = IP_AF_INET;
    ipftps.sock4.laddr.sin.sin_addr.s_addr = ip_htonl(IP_INADDR_ANY);
    ipftps.sock4.laddr.sin.sin_port = ip_htons(port_number);

    /* Get local IPv4 bind address */
    bind_size = sizeof(bind_str);
    if (ipcom_sysvar_get("ipftps.local_ip4", bind_str, &bind_size) != IP_NULL)
    {
        if (ipcom_inet_pton(IP_AF_INET, bind_str,
                            &ipftps.sock4.laddr.sin.sin_addr) != 1)
        {
            IPCOM_LOG2(ERR, "Cannot convert IPv4 local address %s: %s",
                       bind_str, ipcom_strerror(ipcom_errno));
            goto fail4;
        }
    }

    /* Bind to FTP port */
    if (ipcom_bind(ipftps.sock4.fd, &ipftps.sock4.laddr.sa,
                   sizeof(ipftps.sock4.laddr.sin)) < 0)
    {
        IPCOM_LOG1(ERR, "Cannot bind IPv4 server socket: %s",
                   ipcom_strerror(ipcom_errno));
        goto fail4;
    }

    /* Enter listen state. */
    if (ipcom_listen(ipftps.sock4.fd, ipcom_sysvar_get_as_int("ipftps.socket_backlog", 0) != 0))
    {
        IPCOM_LOG1(ERR, "Cannot listen on IPv4 server socket: %s",
                   ipcom_strerror(ipcom_errno));
    fail4:
        (void)ipcom_socketclose(ipftps.sock4.fd);
        ipftps.sock4.fd = IP_INVALID_SOCKET;
    }

 done4:
#endif /* IPCOM_USE_INET */

#ifdef IPCOM_USE_INET6
    /* Get IPv6 socket */
    ipftps.sock6.fd = ipcom_socket(IP_AF_INET6, IP_SOCK_STREAM, IPFTPS_PROTO);
    if (ipftps.sock6.fd == IP_INVALID_SOCKET)
    {
        IPCOM_LOG1(ERR, "Cannot allocate IPv6 server socket: %s",
                   ipcom_strerror(ipcom_errno));
        goto done6;
    }

    option = 1;
    if (ipcom_setsockopt(ipftps.sock6.fd, IP_SOL_SOCKET, IP_SO_REUSEADDR,
                         &option, sizeof(option)) < 0)
    {
        IPCOM_LOG1(WARNING, "Cannot reuse IPv6 server socket address: %s",
                   ipcom_strerror(ipcom_errno));
    }

    /* Set IPv6 defaults */
    ipcom_memset(&ipftps.sock6.laddr.sin6, 0, sizeof(ipftps.sock6.laddr.sin6));
    IPCOM_SA_LEN_SET(&ipftps.sock6.laddr.sin6, sizeof(struct Ip_sockaddr_in6));
    ipftps.sock6.laddr.sin6.sin6_family = IP_AF_INET6;
    /* ipftps.sock6.laddr.sin6.sin6_addr already set to 'any' with memset() */
    ipftps.sock6.laddr.sin6.sin6_port = ip_htons(port_number);

    /* Get local IPv6 bind address */
    bind_size = sizeof(bind_str);
    if (ipcom_sysvar_get("ipftps.local_ip6", bind_str, &bind_size) != IP_NULL)
    {
        if (ipcom_inet_pton(IP_AF_INET, bind_str,
                            &ipftps.sock6.laddr.sin6.sin6_addr) != 1)
        {
            IPCOM_LOG2(ERR, "Cannot convert IPv6 local address %s: %s",
                       bind_str, ipcom_strerror(ipcom_errno));
            goto fail6;
        }
    }

    /* Bind to FTP port */
    if (ipcom_bind(ipftps.sock6.fd, &ipftps.sock6.laddr.sa,
                   sizeof(ipftps.sock6.laddr.sin6)) < 0)
    {
        IPCOM_LOG1(ERR, "Cannot bind IPv6 server socket: %s",
                   ipcom_strerror(ipcom_errno));
        goto fail6;
    }

    /* Enter listen state. */
    if (ipcom_listen(ipftps.sock6.fd, ipcom_sysvar_get_as_int("ipftps.socket_backlog", 0) != 0))
    {
        IPCOM_LOG1(ERR, "Cannot listen on IPv6 server socket: %s",
                   ipcom_strerror(ipcom_errno));
    fail6:
        (void)ipcom_socketclose(ipftps.sock6.fd);
        ipftps.sock6.fd = IP_INVALID_SOCKET;
    }

 done6:
#endif /* IPCOM_USE_INET6 */

    if (ipcom_ipd_init("ipftps", IPCOM_SUCCESS, &ipd_fd) != IPCOM_SUCCESS)
    {
        IPCOM_LOG1(ERR, "Cannot initialize ipd: %s",
                   ipcom_strerror(ipcom_errno));
        goto leave;
    }

    ipd_init = 1;

    for (;;)
    {
        char buf[IPFTPS_IP_MAXLEN];
        Ipftps_sockcb *sock;
        Ip_fd fd;
        Ip_fd_set set;
        Ip_socklen_t len;
        int num;
        int width;

        session = IP_NULL;

        IP_FD_ZERO(&set);
        IP_FD_SET(ipd_fd, &set);
        width = (int)ipd_fd;

#ifdef IPCOM_USE_INET
        if (ipftps.sock4.fd != IP_INVALID_SOCKET)
        {
            IP_FD_SET(ipftps.sock4.fd, &set);
            width = IP_MAX(width, ipftps.sock4.fd);
        }
#endif

#ifdef IPCOM_USE_INET6
        if (ipftps.sock6.fd != IP_INVALID_SOCKET)
        {
            IP_FD_SET(ipftps.sock6.fd, &set);
            width = IP_MAX(width, ipftps.sock6.fd);
        }
#endif

        num = ipcom_socketselect(width + 1, &set, IP_NULL, IP_NULL, IP_NULL);
        if (num < 0)
        {
            IPCOM_LOG1(ERR, "Cannot select on server sockets: %s",
                       ipcom_strerror(ipcom_errno));
            goto leave;
        }
        else if (num == 0)
        {
            /* Ill-behaved stack */
            continue;
        }
        else if (IP_FD_ISSET(ipd_fd, &set))
        {
            int event;

            /* IPD message */
            event = ipcom_ipd_input(ipd_fd);
            switch(event)
            {
            case IPCOM_IPD_EVENT_RECONFIGURE:
                IPCOM_LOG0(INFO, "Reconfiguring due to IPD request");
#ifdef IPCOM_USE_INET
                if (ipftps.sock4.fd != IP_INVALID_SOCKET)
                    ipcom_socketclose(ipftps.sock4.fd);
#endif
#ifdef IPCOM_USE_INET6
                if (ipftps.sock6.fd != IP_INVALID_SOCKET)
                    ipcom_socketclose(ipftps.sock6.fd);
#endif
                goto reconfigure;

            case IPCOM_IPD_EVENT_KILL:
                IPCOM_LOG0(INFO, "Aborting due to IPD request");
                goto leave;

            default:
                /* Ignore, back to main loop */
                continue;
            }
        }
#ifdef IPCOM_USE_INET
        else if (ipftps.sock4.fd != IP_INVALID_SOCKET
                 && IP_FD_ISSET(ipftps.sock4.fd, &set))
        {
            sock = &ipftps.sock4;
        }
#endif
#ifdef IPCOM_USE_INET6
        else if (ipftps.sock6.fd != IP_INVALID_SOCKET
                 && IP_FD_ISSET(ipftps.sock6.fd, &set))
        {
            sock = &ipftps.sock6;
        }
#endif
        else
        {
            IPCOM_LOG1(ERR, "Cannot select on server sockets: %s",
                       ipcom_strerror(ipcom_errno));
            goto leave;
        }

        /* Wait for connections */
        len = sizeof(sock->paddr);
        fd = ipcom_accept(sock->fd, &sock->paddr.sa, &len);
        if (fd < 0)
        {
            IPCOM_LOG2(ERR, "ipcom_accept() failed : %s [%d]",
                       ipcom_strerror(ipcom_errno), ipcom_errno);
            continue;
        }

        (void)ipftps_ntop(&sock->paddr, buf, sizeof(buf));
        IPCOM_LOG2(INFO, "%s connection from %s",
                   (sock->paddr.sa.sa_family == IP_AF_INET ? "IPv4" : "IPv6"),
                   buf);

        /*!!race Get no of sessions */
        num = ipftps.active_sessions;

#define IPFTPS_REPLY(x,y) \
        do \
        { \
            char *_msg = y; \
            (void)ipcom_socketwrite(x, _msg, ipcom_strlen(_msg)); \
        } \
        while (0)

        if (num >= ipftps_sysget_int("ipftps.max_sessions", 8, 1))
        {
            IPCOM_LOG0(WARNING, "Session limit is reached");
            IPFTPS_REPLY(fd, "421 Session limit is reached"CL);
        }
        else if ((session = ipcom_ipc_malloc(sizeof(*session))) == IP_NULL)
        {
            IPCOM_LOG0(ERR, "Out of memory");
            IPFTPS_REPLY(fd, "421 Out of memory"CL);
        }
        else
        {
            len = sizeof(sock->laddr);
            if (ipcom_getsockname(fd, &sock->laddr.sa, &len) != 0)
            {
                IPCOM_LOG0(ERR, "Cannot get server IP address");
                IPFTPS_REPLY(fd, "421 Missing server IP address"CL);
            }
            else
            {
                void *ignore = IP_NULL;
                Ipcom_ipc ipc;

                ipcom_memset(&ipc, 0, sizeof(ipc));

                ipcom_memset(session, 0, sizeof(*session));
                ipcom_memcpy(&session->csock, sock, sizeof(session->csock));
                session->csock.fd = fd;
                ipcom_sprintf(session->name, "ipftps_%lu", ++ipftps.session_no);

                /* Spawn session handler */
                if (ipcom_proc_create(session->name, ipftps_session,
                                      IPCOM_PROC_STACK_LARGE, &pid) !=
                    IPCOM_SUCCESS)
                {
                    IPCOM_LOG1(ERR, "Cannot spawn session handler: %s",
                               ipcom_strerror(ipcom_errno));
                    IPFTPS_REPLY(fd, "421 No session handler available"CL);
                }
                else if (ipcom_ipc_receive(&ipc, &ignore, -1) != IPCOM_SUCCESS)
                {
                    IPCOM_LOG1(ERR, "Cannot receive from session handler: %s",
                               ipcom_strerror(ipcom_errno));
                    IPFTPS_REPLY(fd, "421 Session handler setup failure"CL);
                }
                else
                {
                    ipcom_ipc_free(ignore);

                    if (ipcom_ipc_send(&ipc, session) != IPCOM_SUCCESS)
                    {
                        IPCOM_LOG1(ERR, "Cannot send to session handler: %s",
                                   ipcom_strerror(ipcom_errno));
                        IPFTPS_REPLY(fd, "421 Session handler setup failure"CL);
                    }
#ifdef IP_PORT_OSE5
                    /* OSE5 has process specific sockets -> donate child fd */
                    else if ((Ip_err)efs_donate_fd(fd, pid) != 0)
                    {
                        IPCOM_LOG0(ERR, "Cannot donate socket descriptor");
                        IPFTPS_REPLY(fd, "421 Session handler setup failure"CL);
                    }
#endif /* IP_PORT_OSE5 */
                    else
                    {
                        /*!!race Increment number of sessions. */
                        ipftps.active_sessions++;

                        /* Ok, skip cleanup */
                        (void)ipcom_ipc_close(&ipc);
                        continue;
                    }
                }

                if (ipcom_ipc_isopen(&ipc))
                {
                    (void)ipcom_ipc_close(&ipc);
                }
            }

            ipcom_ipc_free(session);
        }

        ipcom_socketclose(fd);
    }

 leave:
#ifdef IPCOM_USE_INET
    if (ipftps.sock4.fd != IP_INVALID_SOCKET)
        ipcom_socketclose(ipftps.sock4.fd);
#endif

#ifdef IPCOM_USE_INET6
    if (ipftps.sock6.fd != IP_INVALID_SOCKET)
        ipcom_socketclose(ipftps.sock6.fd);
#endif

    /* Have to call ipd init if not done yet  */
    if (ipd_init == 0 &&
        ipcom_ipd_init("ipftps", IPCOM_ERR_FAILED, &ipd_fd) == IPCOM_SUCCESS)
    {
        ipd_init = 1;
    }
    if (ipd_init != 0)
    {
        (void)ipcom_ipd_exit("ipftps", ipd_fd);
    }

    (void)ipcom_ipc_uninstall();
    ipcom_proc_exit();
}


/*
 *===========================================================================
 *                  ipftps_create
 *===========================================================================
 * Description:   Create FTP daemon.
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_err
ipftps_create(void)
{
    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *                   ipftps_start
 *===========================================================================
 * Description:   Start FTP daemon.
 * Parameters:
 * Returns:
 */
IP_PUBLIC Ip_err
ipftps_start(void)
{
    /* Spawn new daemon process */
    if (ipcom_proc_acreate("ipftps", ipftps_main, IP_NULL, IP_NULL))
    {
        return IPCOM_ERR_FAILED;
    }

    return IPCOM_SUCCESS;
}


/*
 *===========================================================================
 *      ipftps_wildcard_check
 *===========================================================================
 *
 * Description:
 *      Check wild cards in path.
 *
 * Parameters:
 *      path    File path.
 *      st      Stat struct.
 *
 * Returns:     Pointer to pattern buffer if wild cards are found.
 *              IP_NULL otherwise. Allocated buffer should be 
 *              removed by calling function.  
 */
IP_STATIC char*
ipftps_wildcard_check(char *path, struct Ip_stat *st)
{
    char* pat = IP_NULL;
    char* pattern = path;
    char  rootdir = '\0'; 

    if (path == IP_NULL) goto exit;

    pattern = ipcom_strrchr(path, '/');
    if(pattern == IP_NULL) ipcom_strrchr(path, '\\');  
        
    if (pattern != IP_NULL)
    {             
        /* Check for wild cards */ 
        if ((ipcom_strchr(pattern, '*') != 0) || (ipcom_strchr(pattern, '?') != 0))     
 	    {
            if(path == pattern)               
	        {
                /*  Could be a root dir */
                rootdir = *pattern;                  
            }
            *(pattern++) = '\0';
            pat = ipcom_malloc(ipcom_strlen(pattern) + 1);
               
            if (pat == IP_NULL) goto exit;
            ipcom_strcpy(pat, pattern);

            if(rootdir != '\0')
            {
                path[0] = rootdir;
                path[1] = '\0';
            }
           
            /* Validate the dir path.*/
            if (ipcom_stat(path, st) != 0)
            {
                ipcom_free(pat); 
                pat = IP_NULL;   
            } 
        }
    }
    else 
    { 
	    /* We might have file or dir. Check for wild cards */           
        if ((ipcom_strchr(path, '*') != 0) || (ipcom_strchr(path, '?') != 0))     
	{
            pat = ipcom_malloc(ipcom_strlen(path) + 1);                
            if (pat == IP_NULL) goto exit;
            ipcom_strcpy(pat, path);                              
            /* Set the path to current dir */  
            ipcom_strcpy(path, ".");
            /* Validate the dir path */
            if (ipcom_stat(path, st) != 0)
            {
                ipcom_free(pat); 
                pat = IP_NULL;
            } 
        }
    }  
exit:
    return pat;
}


/*
 *===========================================================================
 *      ipftps_pattern_check
 *===========================================================================
 *
 * Description:
 *      Compare file name pattern with actual file name.
 *
 * Parameters:
 *      pattern    File name pattern with wild cards.
 *      name       File name.
 *
 * Returns:        1 if pattern and file match. 0 otherwise.
 */
IP_STATIC int ipftps_pattern_check (char * pattern, char * name)
{
    char * pat, *next;
    const char anyone = '?' ;
    const char anymany = '*' ;
    const char *anymanystr = "*";
    
    pat = pattern;
    if ((ipcom_strcmp(pat, anymanystr) == 0) && (name[0] != '.'))
       return 1;

    for (next = name ; * next != '\0' ; next ++)
    {
        if (*pat == anyone)
        {
            pat ++ ;
        }
        else if (*pat == anymany)
        {
            if (next[0] == '.')        
                return 0 ;
            if (ipcom_toupper((int)pat[1]) == ipcom_toupper((int)next[1]))
                pat ++ ;
            else if (ipcom_toupper((int)pat[1]) == ipcom_toupper((int)next[0]))
                pat += 2 ;
            else
                continue ;
        }
        else if (ipcom_toupper((int)*pat) != ipcom_toupper((int)*next))
        {
            return 0 ;
        }
        else
        {
            pat ++ ;
        }
    }
 
    if ((*pat == '\0') || (pat[0] == anymany && pat[1] == '\0'))
        return 1;
    else
        return 0;
}


/*
 *===========================================================================
 *                    ipftps_version
 *===========================================================================
 * Description::
 * Parameters::
 * Returns:
 *
 */
IP_PUBLIC const char *
ipftps_version(void)
{
    return "@(#) IPFTPS $Name: VXWORKS_ITER18A_FRZ10 $ - INTERPEAK_COPYRIGHT_STRING";
}


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
