/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_pfile.c,v $ $Revision: 1.23 $
 *     $Source: /home/interpeak/CVSRoot/ipcom/port/vxworks/src/ipcom_pfile.c,v $
 *     $Author: ulf $ $Date: 2008-01-22 09:59:52 $
 *     $State: Exp $ $Locker:  $
 *
 *     INTERPEAK_COPYRIGHT_STRING
 *     Design and implementation by Lennart Bang <lob@interpeak.se>
 ******************************************************************************
 */


/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 * ipcom_file.c contains source code for mapping to vxworks filesystem.
 *
 */

/*
 ****************************************************************************
 * 2                    CONFIGURATION
 ****************************************************************************
 */
#define IPCOM_SKIP_NATIVE_SOCK_API

#include "ipcom_config.h"


/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#include "ipcom_type.h"
#include "ipcom_file.h"
#include "ipcom_syslog.h"
#include "ipcom_clib.h"
#include "ipcom_err.h"
#include "ipcom_shell.h"
#include "ipcom_os.h"
#include "ipcom_vxworks.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#include <taskLib.h>
#include <iosLib.h>
#include <errnoLib.h>


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
 * 9                    FUNCTIONS
 ****************************************************************************
 */

#if IPCOM_USE_FILE == IPCOM_FILE_NATIVE
/*
 *===========================================================================
 *                    ipcom_fileclose
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipcom_fileclose(Ip_fd fildes)
{
    return close((int)fildes);
}


/*
 *===========================================================================
 *                    ipcom_fileread
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_ssize_t
ipcom_fileread(Ip_fd fildes, void *buf, Ip_size_t nbyte)
{
    return (Ip_ssize_t)read((int)fildes, buf, (size_t)nbyte);
}


/*
 *===========================================================================
 *                    ipcom_fileopen
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */

/* TODO:
 * More stuff from unistd.h missing ...
 */
IP_PUBLIC int
ipcom_fileopen(const char *pathname, int omode, Ip_mode_t attrs)
{
    return open(pathname, (int)omode, (mode_t)attrs);
}


/*
 *===========================================================================
 *                    ipcom_fstat
 *===========================================================================
 * Description:
 * VxWork's struct stat is of a different size, compared to struct Ip_stat.
 * Furthermore the struct members come in a different order.
 *
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipcom_fstat(int fildes, struct Ip_stat *buf)
{
    struct stat vxworks_stat;
    int rc;
    rc = fstat((int)fildes, &vxworks_stat);
    if (rc == OK)
    {
        buf->st_mode = vxworks_stat.st_mode;
        buf->st_ino = vxworks_stat.st_ino;
        buf->st_dev = vxworks_stat.st_dev;
        buf->st_nlink = vxworks_stat.st_nlink;
        buf->st_uid = vxworks_stat.st_uid;
        buf->st_gid = vxworks_stat.st_gid;
        buf->st_size = vxworks_stat.st_size;
        buf->st_atime = vxworks_stat.st_atime;
        buf->st_mtime = vxworks_stat.st_mtime;
        buf->st_ctime = vxworks_stat.st_ctime;
    }
    return rc;
}


/*
 *===========================================================================
 *                    ipcom_stat
 *===========================================================================
 * Description:
 * VxWork's struct stat is of a different size, compared to struct Ip_stat.
 * Furthermore the struct members come in a different order.
 *
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipcom_stat(const char *pathname, struct Ip_stat *buf)
{
    struct stat vxworks_stat;
    int rc;

    rc = stat((char*) pathname, &vxworks_stat);
    if (rc == OK)
    {
        buf->st_mode = vxworks_stat.st_mode;
        buf->st_ino = vxworks_stat.st_ino;
        buf->st_dev = vxworks_stat.st_dev;
        buf->st_nlink = vxworks_stat.st_nlink;
        buf->st_uid = vxworks_stat.st_uid;
        buf->st_gid = vxworks_stat.st_gid;
        buf->st_size = vxworks_stat.st_size;
        buf->st_atime = vxworks_stat.st_atime;
        buf->st_mtime = vxworks_stat.st_mtime;
        buf->st_ctime = vxworks_stat.st_ctime;
    }
    return rc;
}


/*
 *===========================================================================
 *                    ipcom_fopen
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC IP_FILE *
ipcom_fopen(const char *filename, const char *mode)
{
    return (IP_FILE *)fopen(filename, mode);
}


/*
 *===========================================================================
 *                    ipcom_fclose
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipcom_fclose(IP_FILE *stream)
{
    return fclose((FILE *)stream);
}


/*
 *===========================================================================
 *                    ipcom_fwrite
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:     function returns the number of items written.
 *
 */
IP_PUBLIC Ip_size_t
ipcom_fwrite(const void *ptr, Ip_size_t size, Ip_size_t nitems, IP_FILE *stream)
{
#if IPCOM_USE_SHELL == IPCOM_SHELL_IPCOM
    Ipcom_proc *proc;

    /* If we are executing in ipcom shell and the stream is stdin or stdout, use ipcom shell io */
    proc = ipcom_proc_find(taskIdSelf(), IP_FALSE);
    if (proc && (proc->shell_fd != IP_INVALID_SOCKET) && (stream == ip_stdout || stream == ip_stderr))
        return ipcom_shell_fwrite(ptr, size, nitems, stream);
#endif

    /* VXWORKS FILESYSTEM */
    return (Ip_size_t)fwrite(ptr, (size_t)size, (size_t)nitems, (FILE *)stream);
}


/*
 *===========================================================================
 *                    ipcom_fread
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_size_t
ipcom_fread(void *ptr, Ip_size_t size, Ip_size_t nitems, IP_FILE *stream)
{
#if IPCOM_USE_SHELL == IPCOM_SHELL_IPCOM
    if (stream == ip_stdin)
        return ipcom_shell_fread(ptr, size, nitems, stream);
#endif

    /* VXWORKS FILESYSTEM */
    return (Ip_size_t)fread(ptr, (size_t)size, (size_t)nitems, (FILE *)stream);
}


/*
 *===========================================================================
 *                    ipcom_opendir
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC IP_DIR *
ipcom_opendir(const char *dirname)
{
    return (IP_DIR *)opendir( (char*) dirname);
}


/*
 *===========================================================================
 *                    ipcom_readdir
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC struct Ip_dirent *
ipcom_readdir(IP_DIR *dirp)
{
    static struct Ip_dirent dir_entry;
    struct dirent *tmp;

    tmp = readdir((DIR *)dirp);
    if (tmp != IP_NULL)
    {
        dir_entry.d_namlen = ipcom_strlen(tmp->d_name);
        ipcom_strcpy(dir_entry.d_name, tmp->d_name);
        return &dir_entry;
    }
    else
        return IP_NULL;
}


/*
 *===========================================================================
 *                    ipcom_closedir
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipcom_closedir(IP_DIR *dirp)
{
    return closedir((DIR *)dirp);
}


/*
 *===========================================================================
 *                    ipcom_fileno
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_fd
ipcom_fileno(IP_FILE *stream)
{
    return (Ip_fd)fileno((FILE*)stream);
}


/*
 *===========================================================================
 *                    ipcom_unlink
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipcom_unlink(const char *pathname)
{
    return unlink( (char*)pathname );
}


/*
 *===========================================================================
 *                    ipcom_ferror
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipcom_ferror(IP_FILE *stream)
{
#if IPCOM_USE_SHELL == IPCOM_SHELL_IPCOM
    if (stream == ip_stdin)
        return 0;
#endif
    return ferror((FILE*)stream);
}


/*
 *===========================================================================
 *                    ipcom_feof
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipcom_feof(IP_FILE *stream)
{
#if IPCOM_USE_SHELL == IPCOM_SHELL_IPCOM
    if (stream == ip_stdin)
        return 0;
#endif
    return feof((FILE*)stream);
}


/*
 *===========================================================================
 *                    ipcom_fseek
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipcom_fseek(IP_FILE *stream, int offset, int whence)
{
    return fseek((FILE*)stream, (long int)offset, (int)whence);
}


/*
 *===========================================================================
 *                    ipcom_ftell
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipcom_ftell(IP_FILE *stream)
{
    return ftell((FILE*)stream);
}


/*
 *===========================================================================
 *                    ipcom_lseek
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_off_t
ipcom_lseek(Ip_fd fildes, Ip_off_t offset, int whence)
{
    return lseek((int)fildes, (off_t)offset, (int)whence);
}


/*
 *===========================================================================
 *                    ipcom_rename
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipcom_rename(const char *old, const char *new)
{
    return rename(old, new);
}


/*
 *===========================================================================
 *                    ipcom_fdopen
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC IP_FILE *
ipcom_fdopen(Ip_fd fd, const char *mode)
{
    return (IP_FILE*)fdopen((int)fd, mode);
}


/*
 *===========================================================================
 *                    ipcom_getcwd
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC char *
ipcom_getcwd(char *buf, Ip_size_t size)
{
    return getcwd(buf, (size_t)size);
}

/*
 *===========================================================================
 *                    ipcom_mkdir
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipcom_mkdir(const char *path, Ip_mode_t mode)
{
    int fd;
    /* umask? */
    fd = open (path, O_RDWR | O_CREAT, FSTAT_DIR | (mode & 0777));

    if (fd == ERROR)
	return (ERROR);

    return (close (fd));
}


/*
 *===========================================================================
 *                    ipcom_rmdir
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipcom_rmdir(const char *dirName)
{
    /*
     * Hmm. This duplicates code in usrFsLib.c to avoid bringing the
     * whole of usrFsLib.c into the image.
     */
    int           fd;
    int           error;
    STATUS        status;
    struct stat   stat;

    if ((fd = open (dirName, O_RDONLY, 0)) == ERROR)
        return (ERROR);

    if (fstat (fd, &stat) != OK)        /* Obtain file stats on <dirName>. */
    {
        error = errnoGet ();
        close (fd);
        errnoSet (error);
        return (ERROR);
    }

    if (!S_ISDIR (stat.st_mode))        /* If it is not a directory, then  */
    {                               /* set <errno> and return ERROR.   */
        close (fd);
        errnoSet (ENOTDIR);
        return (ERROR);
    }

    status = ioctl (fd, FIORMDIR, (int) dirName);
    error = errnoGet ();
    close (fd);

    if ((status != OK) && ((error == ENOTSUP) || (error == ENOSYS)))
        return (remove (dirName));

    return (status);
}


/*
 *===========================================================================
 *                    ipcom_chdir
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipcom_chdir(const char *path)
{
#if 1
    DIR *dir;

    /*
     * VxWorks filesystem does not check that the destination dir is valid--
     * this must be done by the clients.
     */
    if ((dir = opendir(path)) == NULL)
        return -1;

    (void)closedir(dir);

    return chdir((char*)path);
#else
    /*!! Old code due to problems with relative paths in older vxWorks kernel.
     * Unsure if we still need it??  /Lennart Bang 2006-09-27
     */
    char buf[128];
    char *tmp;

    /* Check if the path is an absolute path */
    if (iosDevFind((char*)path, (const char **)&tmp) != NULL && path != tmp)
        return chdir((char*)path);

    /* Convert relative path to absolute path */
    if (getcwd(buf, sizeof(buf)) == NULL)
    {
        IPCOM_LOG1(ERR, "ipcom_chdir :: getcwd failed, errno: %x", errno);
        return -1;
    }

    if (ipcom_strlen(buf) + 1 + ipcom_strlen(path) + 1 > sizeof(buf))
    {
        IPCOM_LOG0(ERR, "ipcom_chdir :: constructed absolute path would exceed max length");
        return -1;
    }
    if (buf[ipcom_strlen(buf)-1] != '/')
        ipcom_strcat(buf, "/");
    ipcom_strcat(buf, path);

    return chdir(buf);
#endif
}


/*
 *===========================================================================
 *                    ipcom_fflush
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipcom_fflush(IP_FILE *file)
{
#if IPCOM_USE_SHELL == IPCOM_SHELL_IPCOM
    if (file == ip_stdout || file == ip_stderr)
        return 0;
#endif

    return fflush((FILE*)file);
}


/*
 *===========================================================================
 *                    ipcom_filewrite
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_ssize_t
ipcom_filewrite(Ip_fd fd, const void *buffer, Ip_size_t nbyte)
{
#if IPCOM_USE_SHELL == IPCOM_SHELL_IPCOM
    if (fd == IP_FD_STDOUT || fd == IP_FD_STDERR)
        return ipcom_shell_write(fd, buffer, nbyte);
#endif

    return write((int)fd, (void*)buffer, (size_t)nbyte);
}


/*
 *===========================================================================
 *                    ipcom_rewind
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC void
ipcom_rewind(IP_FILE *stream)
{
    fseek((FILE*)stream, 0L, SEEK_SET);
}

#endif /* IPCOM_USE_FILE == IPCOM_FILE_NATIVE */

#if IPCOM_USE_SHELL == IPCOM_SHELL_NATIVE
/*
 *===========================================================================
 *                    ipcom_shell_fread
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_size_t
ipcom_shell_fread(void *ptr, Ip_size_t size, Ip_size_t nitems, IP_FILE *stream)
{
    return (Ip_size_t)fread(ptr, (size_t)size, (size_t)nitems, (FILE *)stream);
}


/*
 *===========================================================================
 *                    ipcom_shell_fwrite
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_size_t
ipcom_shell_fwrite(IP_CONST void *ptr, Ip_size_t size, Ip_size_t nitems, IP_FILE *stream)
{
    return (Ip_size_t)fwrite(ptr, (size_t)size, (size_t)nitems, (FILE *)stream);
}

#endif /* IPCOM_USE_SHELL == IPCOM_SHELL_NATIVE */


#if IPCOM_USE_FILE != IPCOM_FILE_NATIVE && IPCOM_USE_SHELL != IPCOM_SHELL_NATIVE
int ipcom_pfile_empty_file;
#endif
/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */


