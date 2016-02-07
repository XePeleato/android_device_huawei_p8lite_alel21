/*
******************************************************************************
*                     HEADER FILE
*
*     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_file.h,v $ $Revision: 1.47 $
*     $Source: /home/interpeak/CVSRoot/ipcom/include/ipcom_file.h,v $
*     $Author: jhorteli $ $Date: 2008-01-30 11:44:49 $
*     $State: Exp $ $Locker:  $
*
*     INTERPEAK_COPYRIGHT_STRING
*     Design and implementation by Lennart Bang <lob@interpeak.se>
******************************************************************************
*/
#ifndef IPCOM_FILE_H
#define IPCOM_FILE_H


/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 * Interpeak file system API.
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

#include "ipcom_type.h"
#include "ipcom_cstyle.h"
#include "ipcom_clibmap.h"
#include "ipcom_errno.h"

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
 *                    clibmap auto-fix
 *===========================================================================
 */

#if IPCOM_USE_FILE == IPCOM_FILE_IPCOM || \
    IPCOM_USE_SHELL == IPCOM_SHELL_IPCOM || \
    defined(IP_PORT_INTEGRITY)

#ifdef IPCOM_GETC
#undef IPCOM_GETC
#endif
#define IPCOM_GETC 1

#ifdef IPCOM_UNGETC
#undef IPCOM_UNGETC
#endif
#define IPCOM_UNGETC 1

#ifdef IPCOM_PUTC
#undef IPCOM_PUTC
#endif
#define IPCOM_PUTC 1

#ifdef IPCOM_FGETC
#undef IPCOM_FGETC
#endif
#define IPCOM_FGETC 1

#ifdef IPCOM_FPUTC
#undef IPCOM_FPUTC
#endif
#define IPCOM_FPUTC 1

#ifdef IPCOM_FGETS
#undef IPCOM_FGETS
#endif
#define IPCOM_FGETS 1

#ifdef IPCOM_FPUTS
#undef IPCOM_FPUTS
#endif
#define IPCOM_FPUTS 1

#ifdef IPCOM_GETCHAR
#undef IPCOM_GETCHAR
#endif
#define IPCOM_GETCHAR 1

#ifdef IPCOM_PUTCHAR
#undef IPCOM_PUTCHAR
#endif
#define IPCOM_PUTCHAR 1

#endif


/*
 *===========================================================================
 *                    constants
 *===========================================================================
 */
#define IP_EOF           (-1)
#define IP_BUFSIZ        1024
#define IP_PATH_MAX      1024


/*
 *===========================================================================
 *                    object types
 *===========================================================================
 */

/* Macros for testing object type. */
#define IP_S_ISDIR(mode)       ((mode) & IP_S_IFDIR)
#define IP_S_ISREG(mode)       ((mode) & IP_S_IFREG)

/* Bitvalues for object types. */
#define IP_S_IFMT        0x0000f000  /* Object Mask Types. */
#define IP_S_IFDIR       0x00004000  /* Object is directory. */
#define IP_S_IFBLK       0x00006000  /* Object is block special. */
#define IP_S_IFREG       0x00008000  /* Object is regular file. */
#define IP_S_IFLNK       0x0000A000  /* Object is symbolic link. */


/*
 *===========================================================================
 *                    object access
 *===========================================================================
 */

#define IP_S_IREAD       0000400   /* read permission, owner */
#define IP_S_IWRITE      0000200   /* write permission, owner */
#define IP_S_IEXEC       0000100   /* execute/search permission, owner */

#define IP_S_IARCH       0x00000200 /* Object has been modified. */

#define IP_S_IRUSR       IP_S_IREAD /* Read access for owner. */
#define IP_S_IWUSR       IP_S_IWRITE/* Write access for owner. */
#define IP_S_IXUSR       IP_S_IEXEC /* Execute/search access for owner. */

#define IP_S_IRGRP       IP_S_IREAD /* Read access for group. */
#define IP_S_IWGRP       IP_S_IWRITE/* Write access for group. */
#define IP_S_IXGRP       IP_S_IEXEC /* Execute/search access for group. */

#define IP_S_IROTH       IP_S_IREAD /* Read access for other. */
#define IP_S_IWOTH       IP_S_IWRITE/* Write access for other. */
#define IP_S_IXOTH       IP_S_IEXEC /* Execute/search access for other. */


/*
 *===========================================================================
 *                    file open modes
 *===========================================================================
 */

#define IP_O_RDONLY      0x0
#define IP_O_WRONLY      0x1
#define IP_O_RDWR        0x2
#define IP_O_APPEND      0x8
#define IP_O_CREAT       0x100
#define IP_O_TRUNC       0x0200  /* Unsupported! */
#define IP_O_EXCL        0x0400  /* Unsupported! */
#define IP_O_NONBLOCK    0x0800  /* Unsupported! */


/*
 *===========================================================================
 *                         seek 'whence'
 *===========================================================================
 */
#define IP_SEEK_SET       0       /* Seek from beginning of file.  */
#define IP_SEEK_CUR       1       /* Seek from current position.  */
#define IP_SEEK_END       2       /* Seek from end of file.  */


/*
 *===========================================================================
 *                         setvbuf 'mode'
 *===========================================================================
 */
#define IP__IOFBF         0       /* setvbuf should set fully buffered */
#define IP__IOLBF         1       /* setvbuf should set line buffered */
#define IP__IONBF         2       /* setvbuf should set unbuffered */


/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    Various simple types
 *===========================================================================
 */
typedef Ip_s32     Ip_mode_t;
typedef Ip_s32     Ip_off_t;
typedef Ip_u32     Ip_nlink_t;


/*
 *===========================================================================
 *                         struct Ip_stat
 *===========================================================================
 * Structure filled in by ipcom_fstat() and ipcom_stat() functions.
 *
 * NOTE: Needs to be same name as function since unix stat() takes struct
 * stat as an argument.
 */
#define Ip_stat ipcom_stat
struct Ip_stat
{
    Ip_mode_t  st_mode;   /* Object type and access mode. */
    Ip_ino_t   st_ino;    /* Inode number or similar unique id. for object. */
    Ip_dev_t   st_dev;    /* Device number (OSE: pid of volume manager). */
    Ip_nlink_t st_nlink;  /* Number of links to object (if implemented). */
    Ip_uid_t   st_uid;    /* User id. of object owner. */
    Ip_gid_t   st_gid;    /* Group id. of object owner. */
    Ip_off_t   st_size;   /* Size in bytes. */
    Ip_time_t  st_atime;  /* Time of latest access (if implemented). */
    Ip_time_t  st_mtime;  /* Time of latest modification. */
    Ip_time_t  st_ctime;  /* Time of creation (if implemented). */
};


/*
 *===========================================================================
 *                         Ip_dirent
 *===========================================================================
 */
struct Ip_dirent
{
    Ip_u32   d_namlen;       /* ipcom_strlen(d_name). */
    char     d_name[256];
};


/*
 *===========================================================================
 *                         Ip_termios
 *===========================================================================
 */
typedef Ip_u32   Ip_tcflag_t;
typedef Ip_u8    Ip_cc_t;
typedef Ip_u32   Ip_speed_t;

#define IP_NCCS  20

#define IP_ECHO         0000010

#define	IP_TCSANOW	    0
#define	IP_TCSADRAIN	1
#define	IP_TCSAFLUSH	2


/*
 ****************************************************************************
 * 6                    STANDARD FUNCTIONS
 ****************************************************************************
 */

IP_PUBLIC int        ipcom_fileopen(const char *pathname, int omode, Ip_mode_t attrs);
IP_PUBLIC int        ipcom_fileclose(Ip_fd fildes);
IP_PUBLIC Ip_ssize_t ipcom_fileread(Ip_fd fildes, void *buf, Ip_size_t nbyte);
IP_PUBLIC Ip_ssize_t ipcom_filewrite(Ip_fd fd, const void *buffer, Ip_size_t nbyte);

/* stdio.h */
IP_PUBLIC IP_FILE  *ipcom_fopen(const char *filename, const char *mode);
IP_PUBLIC int       ipcom_fclose(IP_FILE *stream);
IP_PUBLIC Ip_size_t ipcom_fwrite(const void *ptr, Ip_size_t size, Ip_size_t nitems, IP_FILE *stream);
IP_PUBLIC Ip_size_t ipcom_fread(void *ptr, Ip_size_t size, Ip_size_t nitems, IP_FILE *stream);
IP_PUBLIC int       ipcom_fflush(IP_FILE *file);

/* sys/stat.h */
IP_PUBLIC int ipcom_fstat(int fildes, struct Ip_stat *buf);
IP_PUBLIC int ipcom_stat(const char *pathname, struct Ip_stat *buf);

/* dirent.h */
IP_PUBLIC IP_DIR           *ipcom_opendir(const char *dirname);
IP_PUBLIC struct Ip_dirent *ipcom_readdir(IP_DIR *dirp);
IP_PUBLIC int               ipcom_closedir(IP_DIR *dirp);

/* stdio.h */
IP_PUBLIC int      ipcom_ferror(IP_FILE *stream); /* IPSSL req. */
IP_PUBLIC int      ipcom_feof(IP_FILE *stream);

/* stdio.h */
IP_PUBLIC int      ipcom_fseek(IP_FILE *stream, int offset, int whence); /* IPSSL req. */
IP_PUBLIC void     ipcom_rewind(IP_FILE *stream);
IP_PUBLIC int      ipcom_ftell(IP_FILE *stream); /* IPSSL req. */
IP_PUBLIC Ip_off_t ipcom_lseek(Ip_fd fildes, Ip_off_t offset, int whence); /* IPSSL req. */

IP_PUBLIC IP_FILE *ipcom_fdopen(Ip_fd fd, const char *mode); /* posix */
IP_PUBLIC Ip_fd    ipcom_fileno(IP_FILE *stream);

/* stdio.h, unistd.h and sys/stat.h */
IP_PUBLIC int   ipcom_mkdir(const char *path, Ip_mode_t mode);
IP_PUBLIC int   ipcom_rmdir(const char *path);
IP_PUBLIC int   ipcom_chdir(const char *path);
IP_PUBLIC char *ipcom_getcwd(char *buf, Ip_size_t size);
IP_PUBLIC int   ipcom_unlink(const char *pathname);
IP_PUBLIC int   ipcom_rename(const char *old_file, const char *new_file);


/*
 ****************************************************************************
 * 7                    PORT/SRC FILE FUNCTIONS
 ****************************************************************************
 */


/*
 *===========================================================================
 *                         ipcom_fgetc
 *===========================================================================
 * port/src/ipcom_getput.c
 */

#if defined(IPCOM_FGETC) && IPCOM_FGETC != 1
#define ipcom_fgetc IPCOM_FGETC
#elif !defined(IPCOM_FGETC)
#define ipcom_fgetc fgetc
#endif

#if (defined(IPCOM_FGETC) && IPCOM_FGETC == 1) || defined(IPCOM_USE_CLIB_PROTO)
IP_PUBLIC int ipcom_fgetc(IP_FILE *stream);
#endif


/*
 *===========================================================================
 *                         ipcom_fputc
 *===========================================================================
 * port/src/ipcom_getput.c
 */

#if defined(IPCOM_FPUTC) && IPCOM_FPUTC != 1
#define ipcom_fputc IPCOM_FPUTC
#elif !defined(IPCOM_FPUTC)
#define ipcom_fputc fputc
#endif

#if (defined(IPCOM_FPUTC) && IPCOM_FPUTC == 1) || defined(IPCOM_USE_CLIB_PROTO)
IP_PUBLIC int   ipcom_fputc(int c, IP_FILE *stream);
#endif


/*
 *===========================================================================
 *                         ipcom_fgets
 *===========================================================================
 * port/src/ipcom_getput.c
 */

#if defined(IPCOM_FGETS) && IPCOM_FGETS != 1
#define ipcom_fgets IPCOM_FGETS
#elif !defined(IPCOM_FGETS)
#define ipcom_fgets fgets
#endif

#if (defined(IPCOM_FGETS) && IPCOM_FGETS == 1) || defined(IPCOM_USE_CLIB_PROTO)
IP_PUBLIC char *ipcom_fgets(char *buffer, int bufsize, IP_FILE *stream);
#endif


/*
 *===========================================================================
 *                         ipcom_fputs
 *===========================================================================
 * port/src/ipcom_getput.c
 */

#if defined(IPCOM_FPUTS) && IPCOM_FPUTS != 1
#define ipcom_fputs IPCOM_FPUTS
#elif !defined(IPCOM_FPUTS)
#define ipcom_fputs fputs
#endif

#if (defined(IPCOM_FPUTS) && IPCOM_FPUTS == 1) || defined(IPCOM_USE_CLIB_PROTO)
IP_PUBLIC int   ipcom_fputs(const char *str, IP_FILE *stream);
#endif


/*
 *===========================================================================
 *                         ipcom_getc
 *===========================================================================
 * port/src/ipcom_getput.c
 */

#if defined(IPCOM_GETC) && IPCOM_GETC != 1
#define ipcom_getc IPCOM_GETC
#elif !defined(IPCOM_GETC)
#define ipcom_getc getc
#endif

#if (defined(IPCOM_GETC) && IPCOM_GETC == 1) || defined(IPCOM_USE_CLIB_PROTO)
IP_PUBLIC int   ipcom_getc(IP_FILE *stream);
#endif


/*
 *===========================================================================
 *                         ipcom_ungetc
 *===========================================================================
 * port/src/ipcom_getput.c
 */

#if defined(IPCOM_UNGETC) && IPCOM_UNGETC != 1
#define ipcom_ungetc IPCOM_UNGETC
#elif !defined(IPCOM_UNGETC)
#define ipcom_ungetc ungetc
#endif

#if (defined(IPCOM_UNGETC) && IPCOM_UNGETC == 1) || defined(IPCOM_USE_CLIB_PROTO)
IP_PUBLIC int   ipcom_ungetc(int c, IP_FILE *stream);
#endif



/*
 *===========================================================================
 *                         ipcom_getchar
 *===========================================================================
 * port/src/ipcom_getput.c
 */

#if defined(IPCOM_GETCHAR) && IPCOM_GETCHAR != 1
#define ipcom_getchar IPCOM_GETCHAR
#elif !defined(IPCOM_GETCHAR)
#define ipcom_getchar getchar
#endif

#if (defined(IPCOM_GETCHAR) && IPCOM_GETCHAR == 1) || defined(IPCOM_USE_CLIB_PROTO)
IP_PUBLIC int   ipcom_getchar(void);
#endif


/*
 *===========================================================================
 *                         ipcom_putc
 *===========================================================================
 * port/src/ipcom_getput.c
 */

#if defined(IPCOM_PUTC) && IPCOM_PUTC != 1
#define ipcom_putc IPCOM_PUTC
#elif !defined(IPCOM_PUTC)
#define ipcom_putc putc
#endif

#if (defined(IPCOM_PUTC) && IPCOM_PUTC == 1) || defined(IPCOM_USE_CLIB_PROTO)
IP_PUBLIC int   ipcom_putc(int c, IP_FILE *stream);
#endif


/*
 *===========================================================================
 *                         ipcom_putchar
 *===========================================================================
 * port/src/ipcom_getput.c
 */

#if defined(IPCOM_PUTCHAR) && IPCOM_PUTCHAR != 1
#define ipcom_putchar IPCOM_PUTCHAR
#elif !defined(IPCOM_PUTCHAR)
#define ipcom_putchar putchar
#endif

#if (defined(IPCOM_PUTCHAR) && IPCOM_PUTCHAR == 1) || defined(IPCOM_USE_CLIB_PROTO)
IP_PUBLIC int   ipcom_putchar(int c);
#endif


/*
 *===========================================================================
 *                         misc
 *===========================================================================
 */

/* port/src/ipcom_access.c */
IP_PUBLIC Ip_mode_t ipcom_umask(Ip_mode_t cmask);
IP_PUBLIC int       ipcom_access(const char *path, int amode);
IP_PUBLIC int       ipcom_chmod(const char *path, Ip_mode_t mode);

/* port/src/ipcom_setvbuf.c */
IP_PUBLIC int   ipcom_setvbuf(IP_FILE *file, char *buffer, int mode, Ip_size_t bufsize); /* IPSSL req. */


/*
 ****************************************************************************
 * 8                    PORT OVERRIDE INCLUDE
 ****************************************************************************
 */

#if IPCOM_USE_FILE == IPCOM_FILE_NATIVE
#include "ipcom_pfile.h"
#endif


/*
 ****************************************************************************
 * 9                    PORT SPECIFIC TYPES
 ****************************************************************************
 * These types must be after the include of ipcom_pfile.h since sizes may
 * be altered in ipcom_pfile.h.
 */

/*
 *===========================================================================
 *                         Ip_termios
 *===========================================================================
 */
struct Ip_termios
{
    Ip_tcflag_t   c_iflag;	       /* Input modes. */
    Ip_tcflag_t   c_oflag;	       /* Output modes. */
    Ip_tcflag_t   c_cflag;	       /* Control modes. */
    Ip_tcflag_t   c_lflag;	       /* Local modes. */
    Ip_cc_t       c_cc[IP_NCCS];  /* Control characters. */
    Ip_speed_t    c_ispeed;	   /* Input speed. */
    Ip_speed_t    c_ospeed;	   /* Output speed. */
};

/* port/src/ipcom_termios.c */
#if IPCOM_USE_FILE != IPCOM_FILE_IPCOM
IP_PUBLIC int ipcom_tcgetattr(Ip_fd fd, struct Ip_termios *termios_p);
IP_PUBLIC int ipcom_tcsetattr(Ip_fd fd, int optional_actions, const struct Ip_termios *termios_p);
#endif


#ifdef __cplusplus
}
#endif

#endif

/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */


