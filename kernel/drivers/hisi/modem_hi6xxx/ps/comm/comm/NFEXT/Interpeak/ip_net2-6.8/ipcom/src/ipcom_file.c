/*
 ******************************************************************************
 *                     INTERPEAK SOURCE FILE
 *
 *   Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_file.c,v $ $Revision: 1.50 $
 *   $Source: /home/interpeak/CVSRoot/ipcom/src/ipcom_file.c,v $
 *   $Author: joel $
 *   $State: Exp $ $Locker:  $
 *
 *   INTERPEAK_COPYRIGHT_STRING
 *   Design and implementation by Lennart Bang <lob@interpeak.se>
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

#define IPCOM_FILE_ENABLE_REENTRANT

/*
 ****************************************************************************
 * 3                    INCLUDE FILES
 ****************************************************************************
 */

#if IPCOM_USE_FILE == IPCOM_FILE_IPCOM

#define IPCOM_USE_CLIB_PROTO
#include "ipcom_type.h"
#include "ipcom_cstyle.h"
#include "ipcom_clib.h"
#include "ipcom_err.h"
#include "ipcom_os.h"
#include "ipcom_syslog.h"
#include "ipcom_file.h"
#include "ipcom_time.h"
#include "ipcom_shell.h"


/*
 ****************************************************************************
 * 4                    DEFINES
 ****************************************************************************
 */

#define IPCOM_SYSLOG_PRIORITY    IPCOM_SYSLOG_IPCOM_PRIORITY
#define IPCOM_SYSLOG_FACILITY    IPCOM_LOG_IPCOM

#define IPCOM_FILE_DIRNAME_MAX     128

#define IPCOM_FILE_OBJT_FILE 0
#define IPCOM_FILE_OBJT_DIR  1
#define IPCOM_FILE_OBJT_ANY  2

#ifdef IPCOM_FILE_ENABLE_REENTRANT
#define IPFILE_ISLOCKED_ASSERT()    ip_assert(ipcom_file.code_mutex_count == 1)
#define IPFILE_LOCK()      do { ipcom_mutex_lock(ipcom_file.code_mutex); \
                                ip_assert(ipcom_file.code_mutex_count == 0); \
                                ipcom_file.code_mutex_count++; \
                           } while((0))

#define IPFILE_UNLOCK()    do { ip_assert(ipcom_file.code_mutex_count == 1); \
                                ipcom_file.code_mutex_count--; \
				                ipcom_mutex_unlock(ipcom_file.code_mutex); \
                           } while((0))
#else
#define IPFILE_LOCK()      IP_NOOP
#define IPFILE_UNLOCK()    IP_NOOP
#endif


/*
 ****************************************************************************
 * 5                    TYPES
 ****************************************************************************
 */

/*
 *===========================================================================
 *                         Ipcom_file_dirent
 *===========================================================================
 */
typedef struct Ipcom_file_dirent_struct
{
    char               path[IPCOM_FILE_DIRNAME_MAX];
    Ip_ino_t           last_ino;

    struct Ip_dirent   dirent;
}
Ipcom_file_dirent;


/*
 *===========================================================================
 *                         Ipcom_file_buf
 *===========================================================================
 */
typedef struct Ipcom_file_buf_struct
{
    struct Ipcom_file_buf_struct  *next;
    int    length;
    char   buf[512];
}
Ipcom_file_buf;


/*
 *===========================================================================
 *                         Ipcom_file
 *===========================================================================
 */
typedef struct Ipcom_file_struct
{
    Ipcom_file_buf   *data;
    int               tot_length;
}
Ipcom_file;


/*
 *===========================================================================
 *                         Ipcom_dir
 *===========================================================================
 */
typedef struct Ipcom_dir_struct
{
    Ip_u32   dummy;
}
Ipcom_dir;


/*
 *===========================================================================
 *                         Ipcom_filedir
 *===========================================================================
 */
typedef struct Ipcom_filedir_struct
{
    struct Ipcom_filedir_struct   *parent;
    struct Ipcom_filedir_struct   *child;
    struct Ipcom_filedir_struct   *next;

    char       *name;
    Ip_mode_t   mode;	/* Object type and access mode */
    Ip_ino_t    ino;
    int         flags;
#ifdef IPCOM_USE_TIME
    Ip_time_t   mtime;
    Ip_time_t   ctime;
#endif

    union Ipcom_filedir_union
    {
        Ipcom_dir   dir;
        Ipcom_file  file;
    }
    f;
}
Ipcom_filedir;

#define IPCOM_FILE_FLAG_ISFILE    1

#define IPCOM_FILE_IS_FILE(xfiledir)  IP_BIT_ISSET((xfiledir)->flags, IPCOM_FILE_FLAG_ISFILE)
#define IPCOM_FILE_IS_DIR(xfiledir)   !IPCOM_FILE_IS_FILE(xfiledir)


/*
 *===========================================================================
 *                         Ipcom_file_desc
 *===========================================================================
 */
typedef struct Ipcom_file_desc_struct
{
    Ipcom_filedir   *file;

    Ip_pid_t  pid;
    int       omode;
    int       offset;
}
Ipcom_file_desc;


/*
 *===========================================================================
 *                         Ipcom_file_proc
 *===========================================================================
 */
typedef struct Ipcom_file_proc_struct
{
    char   cwd[IPCOM_FILE_DIRNAME_MAX];
}
Ipcom_file_proc;


/*
 *===========================================================================
 *                   Global ipcom_file data
 *===========================================================================
 * Internal global Ipcom_file data.
 */
typedef struct Ipcom_file_data_struct
{
    /* IPCOM Memory file system. */
    Ip_ino_t                       fileino;
    struct Ipcom_file_desc_struct *filedesc[IPCOM_FILE_FD_SETSIZE];
    struct Ipcom_filedir_struct   *fileroot;

#ifdef IPCOM_FILE_ENABLE_REENTRANT
    Ipcom_mutex   code_mutex;
    int           code_mutex_count;
#endif
}
Ipcom_file_data;


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

IP_GLOBAL void   ipcom_file_proc_exit(Ipcom_proc *proc);

IP_STATIC Ip_err ipcom_file_init(void *unused);

IP_STATIC Ipcom_file_proc *ipcom_file_proc_fileinfo(void);
IP_STATIC char *ipcom_file_get_next(char *entry);

IP_STATIC int ipcom_file_make_name(Ipcom_filedir *filedir, char *name, Ip_size_t size);


/*
 ****************************************************************************
 * 8                    DATA
 ****************************************************************************
 */

IP_STATIC Ipcom_file_data   ipcom_file;

IP_STATIC Ipcom_once_t   ipcom_file_once = IPCOM_ONCE_INIT;


/*
 ****************************************************************************
 * 9                    STATIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipcom_file_get_desc
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ipcom_file_desc *
ipcom_file_get_desc(Ip_fd fd, int *errnop)
{
    Ipcom_file_desc  *desc;

    IPFILE_LOCK();

    if(fd < 3 || fd >= IPCOM_FILE_FD_SETSIZE || ipcom_file.filedesc[fd] == IP_NULL)
    {
        *errnop = IP_ERRNO_EBADF;
        return IP_NULL;
    }
    desc = ipcom_file.filedesc[fd];

    if(desc->file == IP_NULL)
    {
        *errnop = IP_ERRNO_ENOENT;  /*!! correct errno? */
        return IP_NULL;
    }

    return desc;
}


/*
 *===========================================================================
 *                    ipcom_file_fd_malloc
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipcom_file_fd_malloc(int *errnop)
{
  int i;
  Ipcom_file_desc  *desc;

  /* Do not use fd=0,1,2 to simplify socket/file desc code. */
  for (i = 3; i < IPCOM_FILE_FD_SETSIZE; i++)
      if(ipcom_file.filedesc[i] == IP_NULL)
          break;

  if (i >= IPCOM_FILE_FD_SETSIZE)
  {
      *errnop = IP_ERRNO_ENFILE;
      return -1;
  }

  desc = ipcom_malloc(sizeof(Ipcom_file_desc));
  if(desc == IP_NULL)
  {
      *errnop = IP_ERRNO_ENOMEM;
      return -1;
  }

  ipcom_memset(desc, 0, sizeof(Ipcom_file_desc));
  desc->pid = ipcom_getpid();

  ipcom_file.filedesc[i] = desc;
  return i;
}


/*
 *===========================================================================
 *                    ipcom_file_proc_fileinfo
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ipcom_file_proc *
ipcom_file_proc_fileinfo(void)
{
    Ipcom_proc *proc;

    proc = ipcom_proc_self();
    if(proc == IP_NULL)
        return IP_NULL;

    if(proc->fileinfo == IP_NULL)
    {
        proc->fileinfo = ipcom_malloc(sizeof(Ipcom_file_proc));
        if(proc->fileinfo == IP_NULL)
            return IP_NULL;
        ipcom_memset(proc->fileinfo, 0, sizeof(Ipcom_file_proc));
        ipcom_strcpy(proc->fileinfo->cwd, IPCOM_FILE_ROOT);
    }

    return proc->fileinfo;
}


/*
 *===========================================================================
 *                    ipcom_file_get_next
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC char *
ipcom_file_get_next(char *entry)
{
    ip_assert(*entry);
    for(; *entry != '\0' && *entry != '/'; entry++)
    {
    }
    ip_assert(*entry == '/');

    entry++;
    return entry;
}


/*
 *===========================================================================
 *                    ipcom_file_get_entry
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ipcom_filedir *
ipcom_file_get_entry(Ipcom_filedir *parent, char *name)
{
    Ipcom_filedir  *filedir;

    ip_assert(parent != IP_NULL);
    if(name == IP_NULL)
        return parent;
    if(ipcom_strcmp(name, "./") == 0)
        return parent;
    if(ipcom_strcmp(name, "../") == 0)
        return parent->parent;

    ip_assert(IPCOM_FILE_IS_DIR(parent));
    for(filedir = parent->child; filedir != IP_NULL; filedir = filedir->next)
        if(ipcom_strcmp(name, filedir->name) == 0)
        {
            break;
        }

    return filedir;
}


/*
 *===========================================================================
 *                    ipcom_file_getdir
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ipcom_filedir *
ipcom_file_getdir(char *tmp_path, char *path, char **entryp, int *errnop, Ipcom_file_proc **file_procp)
{
    Ipcom_filedir    *filedir;
    Ipcom_file_proc  *file_proc;
    char             *next;
    char              tkn;
    int               pathlen, rootlen;

    if(entryp != IP_NULL)
        *entryp = IP_NULL;

    /* Get process file handle. */
    file_proc = ipcom_file_proc_fileinfo();
    if(file_proc == IP_NULL)
    {
        *errnop = IP_ERRNO_ENOMEM;
        return IP_NULL;
    }
    if(file_procp != IP_NULL)
        *file_procp = file_proc;

    /* Use current directory unless '/' is used to indicate an absolute path. */
    if(path != IP_NULL && *path == '/')
        pathlen = 0;
    else
    {
        ip_assert(file_proc != IP_NULL);
        pathlen = ipcom_strlen(file_proc->cwd);
        ip_assert(pathlen < IPCOM_FILE_DIRNAME_MAX);
        ipcom_strcpy(tmp_path, file_proc->cwd);
    }

    if(path)
    {
        /* Copy in path while cleaning it. */
        for(; *path && (pathlen + 1 < IPCOM_FILE_DIRNAME_MAX); path++)
        {
            /* Discard multiple slashes. */
            if(*path == '/' && pathlen > 0 && tmp_path[pathlen-1] == '/')
                continue;
            /* Copy in character. */
            tmp_path[pathlen] = *path;
            pathlen++;
        }
        /* Verify length. */
        if(pathlen + 1 >= IPCOM_FILE_DIRNAME_MAX)
        {
            *errnop = IP_ERRNO_ENAMETOOLONG;
            return IP_NULL;
        }
        /* Make sure path ends with a '/' */
        if(pathlen > 0 && tmp_path[pathlen - 1] != '/')
        {
            tmp_path[pathlen] = '/';
            pathlen++;
        }
        /* Null terminate. */
        tmp_path[pathlen] = '\0';
        path = tmp_path;
    }

    ip_assert(pathlen < IPCOM_FILE_DIRNAME_MAX);

    /* Start at root directory. */
    rootlen = ipcom_strlen(IPCOM_FILE_ROOT);
    if(*path && ipcom_strncmp(path, IPCOM_FILE_ROOT, rootlen) == 0)
    {
        filedir = ipcom_file.fileroot;
        path    += rootlen;
        /*pathlen -= rootlen;*/
    }
    else
    {
        /* bad root directory. */
        *errnop = IP_ERRNO_ENOENT;
        return IP_NULL;
    }

    /* find file or directory. */
    for(; path != IP_NULL && *path;)
    {
        /* Get next entry. */
        next = ipcom_file_get_next(path);

        /* Check if we want to spare the last entry. */
        if(entryp != IP_NULL && *next == '\0')
        {
            *entryp = path;
            break;
        }

        /* Null terminate path (which is this entry) for string comparions. */
        tkn = *next;
        *next = '\0';

        if(ipcom_strcmp(path, "../") == 0)
        {
            /* Go up one directory. */
            if(filedir->parent == IP_NULL || IPCOM_FILE_IS_FILE(filedir))
            {
                *errnop = IP_ERRNO_ENOENT;
                return IP_NULL;
            }
            filedir = filedir->parent;
            ip_assert(IPCOM_FILE_IS_DIR(filedir));
        }
        else if (ipcom_strcmp(path, "./") == 0)
        {
            /* Stay in this directory. */
        }
        else
        {
            /* Check the directory contents for 'path' */
            for(filedir = filedir->child; filedir != IP_NULL; filedir = filedir->next)
                if(ipcom_strcmp(path, filedir->name) == 0)
                    break;
            if(filedir == IP_NULL)
                goto leave;
        }
        *next = tkn;
        path = next;
    }

 leave:
    if(filedir == IP_NULL)
        *errnop = IP_ERRNO_ENOENT;
    else if(IPCOM_FILE_IS_FILE(filedir))
    {
        *errnop = IP_ERRNO_ENOTDIR;
        filedir = IP_NULL;
    }

    return filedir;
}


/*
 *===========================================================================
 *                    ipcom_file_valid_name
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipcom_file_valid_name(char *name)
{
    if(name == IP_NULL)
        return IP_ERRNO_EINVAL;
    if(ipcom_strcmp(name, "./") == 0)
        return IP_ERRNO_EEXIST;
    if(ipcom_strcmp(name, "../") == 0)
        return IP_ERRNO_EEXIST;

    return 0;
}


/*
 *===========================================================================
 *                    ipcom_file_make_name
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipcom_file_make_name(Ipcom_filedir *filedir, char *name, Ip_size_t size)
{
    Ip_size_t   totlen, newlen;

    if(filedir->parent)
        totlen = ipcom_file_make_name(filedir->parent, name, size);
    else
    {
        name[0] = '/';
        name[1] = '\0';
        totlen = 1;
    }

    newlen = ipcom_strlen(filedir->name);
    if(totlen + newlen < size)
        ipcom_strcat(name, filedir->name);

    return totlen + newlen;
}


/*
 *===========================================================================
 *                    ipcom_file_insert_entry
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipcom_file_insert_entry(Ipcom_filedir *parent, Ipcom_filedir *filedir)
{
    if(parent->child == IP_NULL)
    {
        parent->child = filedir;
    }
    else
    {
        /* Insert entry last in list. */
        Ipcom_filedir *tmpdir;
        for(tmpdir = parent->child; tmpdir->next != IP_NULL; tmpdir = tmpdir->next)
        {
        }
        tmpdir->next = filedir;
    }
    filedir->next = IP_NULL;
    filedir->parent = parent;
}


/*
 *===========================================================================
 *                    ipcom_file_create_entry
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ipcom_filedir *
ipcom_file_create_entry(Ipcom_filedir *parent, char *name)
{
    Ipcom_filedir *filedir;

    /* Create the new file/directory. */
    filedir = ipcom_malloc(sizeof(Ipcom_filedir));
    if(filedir == IP_NULL)
        return IP_NULL;

    ipcom_memset(filedir, 0, sizeof(Ipcom_filedir));
    filedir->name = ipcom_strdup(name);
    if(filedir->name == IP_NULL)
    {
        ipcom_free(filedir);
        return IP_NULL;
    }

    /* Insert new file/directory last in list. */
    filedir->ino = ipcom_file.fileino++;
#ifdef IPCOM_USE_TIME
    filedir->ctime = ipcom_time(0);
    filedir->mtime = filedir->ctime;
#endif
    ip_assert(filedir->ino != 0);

    ipcom_file_insert_entry(parent, filedir);

    return filedir;
}


/*
 *===========================================================================
 *                    ipcom_file_remove_entry
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipcom_file_remove_entry(Ipcom_filedir *filedir)
{
    ip_assert(filedir->parent != IP_NULL);

    if(filedir->parent->child == filedir)
    {
        filedir->parent->child = filedir->next;
    }
    else
    {
        Ipcom_filedir    *tmpdir;

        for(tmpdir = filedir->parent->child; tmpdir != IP_NULL; tmpdir = tmpdir->next)
            if(tmpdir->next == filedir)
                break;
        tmpdir->next = filedir->next;
    }
    filedir->parent = IP_NULL;
    filedir->next = IP_NULL;
}


/*
 *===========================================================================
 *                    ipcom_file_delete_entry
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipcom_file_delete_entry(Ipcom_filedir *filedir)
{
    Ipcom_file_desc  *desc;
    Ipcom_file_buf   *data;
    int i;

    /* Remove entry from list. */
    ipcom_file_remove_entry(filedir);

    /* Free file memory. */
    while(filedir->f.file.data != IP_NULL)
    {
        data = filedir->f.file.data;
        filedir->f.file.data = data->next;
        ipcom_free(data);
    }

    /* Clear open file descriptors access to this file/directory. */
    for (i = 3; i < IPCOM_FILE_FD_SETSIZE; i++)
    {
        desc = ipcom_file.filedesc[i];
        if(desc != IP_NULL && desc->file == filedir)
        {
            desc->file = IP_NULL;
        }
    }

    ipcom_free(filedir->name);
    ipcom_free(filedir);
}


/*
 *===========================================================================
 *                    ipcom_file_stat_init
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC void
ipcom_file_stat_init(Ipcom_filedir *filedir, struct Ip_stat *buf)
{
    ipcom_memset(buf, 0, sizeof(struct Ip_stat));

    buf->st_ino   = filedir->ino;
#ifdef IPCOM_USE_TIME
    buf->st_ctime = filedir->ctime;
    buf->st_mtime = filedir->mtime;
#endif

    if(IPCOM_FILE_IS_FILE(filedir))
    {
        buf->st_mode = IP_S_IFREG;
        buf->st_size = filedir->f.file.tot_length;
    }
    else
    {
        buf->st_mode = IP_S_IFDIR;
        buf->st_size = 512;
    }
    buf->st_mode |= 0x1ff;
}


/*
 *===========================================================================
 *                    ipcom_file_forward
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ipcom_file_buf **
ipcom_file_forward(Ipcom_filedir *filedir, int *offsetp)
{
    Ipcom_file_buf **datap;

    ip_assert(IPCOM_FILE_IS_FILE(filedir));
    ip_assert(*offsetp >= 0);
    ip_assert(*offsetp <= filedir->f.file.tot_length);

    for(datap = &filedir->f.file.data; *offsetp >= 512 && datap != IP_NULL; datap = &(*datap)->next)
    {
        ip_assert((*datap) != IP_NULL);
        ip_assert((*datap)->length == 512);
        *offsetp -= 512;
    }

    ip_assert(*offsetp >= 0);
    return datap;
}


/*
 *===========================================================================
 *                    ipcom_file_writebuf
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipcom_file_writebuf(Ip_fd fd, char *buf, int buflen, Ip_bool append)
{
    Ipcom_file_buf   *data, **datap;
    int  offset, retval, wrote, write;
    Ipcom_file_desc  *desc;
    Ipcom_filedir    *filedir = IP_NULL;

    if(buflen == 0)
        return 0;

    desc = ipcom_file_get_desc(fd, &retval);
    if(desc == IP_NULL)
    {
        retval = -retval;
        goto leave;
    }

    filedir = desc->file;
    ip_assert(filedir != IP_NULL);
    ip_assert(IPCOM_FILE_IS_FILE(filedir));

    if(IP_BIT_ISSET(desc->omode, IP_O_APPEND))
        desc->offset = filedir->f.file.tot_length;
    else if(desc->offset > filedir->f.file.tot_length)
    {
        /*!! Another process has truncated the file resulting
         * in this process trying to write outside the file.
         * What does UNIX do?
         * We force the 1st process to close the descriptor or
         * reset the offset to function again.
         */
        retval = -IP_ERRNO_EIO;
        goto leave;
    }

    offset = desc->offset;
    datap = ipcom_file_forward(filedir, &offset);
    for(wrote = 0; wrote < buflen;)
    {
        /* Need a new file_buf. */
        data = *datap;
        if(data == IP_NULL)
        {
            if(append == IP_FALSE)
                break;
            data = ipcom_malloc(sizeof(Ipcom_file_buf));
            if(data == IP_NULL)
                break;
            data->next = IP_NULL;
            data->length = 0;
            *datap = data;
            datap  = &data->next;
        }
        else
            datap = &(*datap)->next;

        write = IP_MIN(buflen - wrote, (int) (sizeof(data->buf) - data->length));
        if(append == IP_FALSE)
            write = IP_MIN(filedir->f.file.tot_length - desc->offset, write);
        ipcom_memcpy(&data->buf[data->length], buf, write);
        data->length += write;
        wrote  += write;
        buf    += write;
        desc->offset += write;
        if (desc->offset > filedir->f.file.tot_length)
            filedir->f.file.tot_length = desc->offset;
    }

    ip_assert(wrote <= buflen);
    retval = wrote;
    if(append == IP_TRUE && wrote == 0)
        retval = -IP_ERRNO_ENOMEM;

 leave:
    if(retval < 0)
    {
        ipcom_errno = -retval;
        IPCOM_LOG3(DEBUG, "ipcom_writebuf(%d) :: '%s' failed, errno = %d."IP_LF,
                   fd, filedir ? filedir->name : "<none>", ipcom_errno);
        retval = -1;
    }
    else
    {
        ip_assert(filedir != IP_NULL);
#ifdef IPCOM_USE_TIME
        filedir->mtime = ipcom_time(0);
#endif
        /*IPCOM_LOG4(DEBUG, "ipcom_writebuf(%d) :: '%s' wrote %d bytes (tot %d)."IP_LF,
          fd, filedir->name, retval, filedir->f.file.tot_length);*/
    }

    IPFILE_UNLOCK();
    return retval;
}


/*
 *===========================================================================
 *                    ipcom_file_readbuf
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipcom_file_readbuf(Ipcom_filedir *filedir, int xoffset, char *buf, int buflen)
{
    Ipcom_file_buf   *data, **datap;
    int  offset, tot_read, read;

    ip_assert(IPCOM_FILE_IS_FILE(filedir));

    if(buflen == 0)
        return 0;
    if(xoffset >= filedir->f.file.tot_length)
        return 0;

    offset = xoffset;
    datap = ipcom_file_forward(filedir, &offset);
    for(tot_read = 0; buflen > 0; datap = &(*datap)->next)
    {
        data = *datap;
        if(data == IP_NULL)
            break;

        ip_assert(data->length > 0);
        ip_assert(offset <= data->length);
        read = IP_MIN(buflen, data->length - offset);
        ip_assert(read >= 0);
        if(read == 0)
            break;
        ipcom_memcpy(buf, data->buf + offset, read); /*lint !e671 Possibly passing to function a negative value (-1), arg */
        tot_read += read;
        buflen   -= read;
        buf      += read;
        offset = 0;
    }

    return -tot_read;
}


/*
 *===========================================================================
 *                    ipcom_unlink_entry
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC int
ipcom_unlink_entry(const char *pathname, Ip_bool is_dir_only)
{
    Ipcom_filedir *filedir, *parent;
    int retval;
    char *entry;
    char tmp_path[IPCOM_FILE_DIRNAME_MAX];

    if (ipcom_once(&ipcom_file_once, ipcom_file_init, IP_NULL) != IPCOM_SUCCESS)
    {
        IP_PANIC();
        ipcom_errno = IP_ERRNO_EINTERNAL;
        return -1;
    }

    IPFILE_LOCK();

    /* Get the parent. */
    parent = ipcom_file_getdir(tmp_path, (char *)pathname, &entry, &retval, IP_NULL);
    if(parent == IP_NULL)
        goto leave;

    /* Get the file/directory. */
    filedir = ipcom_file_get_entry(parent, entry);
    if(filedir == IP_NULL)
    {
        retval = IP_ERRNO_ENOENT;
        goto leave;
    }

    /* Not legal to delete the root. */
    if(filedir->parent == IP_NULL)
    {
        retval = IP_ERRNO_EACCES;
        goto leave;
    }

    /* Not an empty directory. */
    if(filedir->child != IP_NULL)
    {
        retval = IP_ERRNO_ENOTEMPTY;
        goto leave;
    }

    /* ipcom_rmdir() can not remove a file. */
    if(IPCOM_FILE_IS_FILE(filedir) && is_dir_only)
    {
        retval = IP_ERRNO_ENOTDIR;
        goto leave;
    }

    /* Delete the entry. */
    ipcom_file_delete_entry(filedir);

    /* Success, fall through. */
    retval = 0;

 leave:
    if(retval)
    {
        ipcom_errno = retval;
        retval = -1;
    }
    IPFILE_UNLOCK();
    return retval;
}


/*
 *===========================================================================
 *                    ipcom_file_init
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_STATIC Ip_err
ipcom_file_init(void *unused)
{
    (void)unused;

    ipcom_memset(&ipcom_file, 0, sizeof(Ipcom_file_data));

#ifdef IPCOM_FILE_ENABLE_REENTRANT
    if(ipcom_mutex_create(&ipcom_file.code_mutex) != IPCOM_SUCCESS)
    {
        IP_PANIC();
        return IPCOM_ERR_FAILED;
    }
#endif

    ipcom_file.fileino = 1;
    ipcom_file.fileroot = ipcom_malloc(sizeof(Ipcom_filedir));
    if(ipcom_file.fileroot == IP_NULL)
    {
        IP_PANIC();
        return IPCOM_ERR_NO_MEMORY;
    }

    ipcom_memset(ipcom_file.fileroot, 0, sizeof(Ipcom_filedir));
    ipcom_file.fileroot->name = IPCOM_FILE_ROOT;
    ip_assert(ipcom_file.fileroot->name[0] == '/');
    ipcom_file.fileroot->name++;

    return IPCOM_SUCCESS;
}


/*
 ****************************************************************************
 * 10                   GLOBAL FUNCTIONS
 ****************************************************************************
 */


/*
 *===========================================================================
 *                    ipcom_file_proc_exit
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_GLOBAL void
ipcom_file_proc_exit(Ipcom_proc *proc)
{
    Ipcom_file_desc  *desc;
    int  i;

    /* Close open files. */
    for (i = 3; i < IPCOM_FILE_FD_SETSIZE; i++)
    {
        desc = ipcom_file.filedesc[i];
        if(desc != IP_NULL && desc->pid == proc->pid)
        {
            IPCOM_LOG2(DEBUG, "ipcom_file_proc_exit() :: auto close %d '%s'."IP_LF,
                          i, desc->file ? desc->file->name : "<null>");
            (void)ipcom_fileclose((Ip_fd)i);
        }
    }

    /* Release fileinfo. */
    if(proc->fileinfo != IP_NULL)
        ipcom_free(proc->fileinfo);
}


/*
 ****************************************************************************
 * 11                   PUBLIC FUNCTIONS
 ****************************************************************************
 */


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
    Ipcom_file_proc  *file_proc;
    Ipcom_filedir    *filedir;
    int retval;
    char tmp_path[IPCOM_FILE_DIRNAME_MAX];

    if (ipcom_once(&ipcom_file_once, ipcom_file_init, IP_NULL) != IPCOM_SUCCESS)
    {
        IP_PANIC();
        ipcom_errno = IP_ERRNO_EINTERNAL;
        return -1;
    }

    IPFILE_LOCK();

    filedir = ipcom_file_getdir(tmp_path, (char *)path, IP_NULL, &retval, &file_proc);
    if(filedir == IP_NULL)
        goto leave;

    /* Found the directory, update the proc entry with cwd. */
    retval = ipcom_file_make_name(filedir, file_proc->cwd, sizeof(file_proc->cwd));
    if(retval >= IPCOM_FILE_DIRNAME_MAX)
    {
        retval = IP_ERRNO_ENAMETOOLONG;
        goto leave;
    }
    retval = 0;

 leave:
    if(retval)
    {
        ipcom_errno = retval;
        retval = -1;
    }
    IPFILE_UNLOCK();
    return retval;
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
    Ipcom_file_proc  *file_proc;
    int retval;

    if (ipcom_once(&ipcom_file_once, ipcom_file_init, IP_NULL) != IPCOM_SUCCESS)
    {
        IP_PANIC();
        return IP_NULL;
    }

    IPFILE_LOCK();

    file_proc = ipcom_file_proc_fileinfo();
    if(file_proc == IP_NULL)
    {
        retval = IP_ERRNO_ENOMEM;
        goto fail;
    }

    /*!! do we need to verify the path? */
    if (buf == IP_NULL)
    {
        retval = IP_ERRNO_ERANGE;
        goto fail;
    }

    /* Check size. */
    if(ipcom_strlen(file_proc->cwd) >= size)
    {
        retval = IP_ERRNO_ERANGE;
        goto fail;
    }

    ipcom_strcpy(buf, file_proc->cwd);
    IPFILE_UNLOCK();
    return buf;

 fail:
    ipcom_errno = retval;
    IPFILE_UNLOCK();
    return IP_NULL;
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
    Ipcom_filedir *filedir, *parent;
    char *entry;
    int retval;
    char tmp_path[IPCOM_FILE_DIRNAME_MAX];

    (void)mode;

    if (ipcom_once(&ipcom_file_once, ipcom_file_init, IP_NULL) != IPCOM_SUCCESS)
    {
        IP_PANIC();
        ipcom_errno = IP_ERRNO_EINTERNAL;
        return -1;
    }

    IPFILE_LOCK();

    /* Get parent directory. */
    parent = ipcom_file_getdir(tmp_path, (char *)path, &entry, &retval, IP_NULL);
    if(parent == IP_NULL)
        goto leave;

    /* Verify 'entry name'. */
    retval = ipcom_file_valid_name(entry);
    if(retval != 0)
        goto leave;

    /* Check for duplicate. */
    filedir = ipcom_file_get_entry(parent, entry);
    if(filedir != IP_NULL)
    {
        retval = IP_ERRNO_EEXIST;
        goto leave;
    }

    /* Create the directory. */
    filedir = ipcom_file_create_entry(parent, entry);
    if(filedir == IP_NULL)
    {
        retval = IP_ERRNO_ENOMEM;
        goto leave;
    }

    /* Success. */
    IPFILE_UNLOCK();
    return 0;

 leave:
    ipcom_errno = retval;
    IPFILE_UNLOCK();
    return -1;
}


/*
 *===========================================================================
 *                    ipcom_rmdir
 *===========================================================================
 * Description:   Remove directory only.
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipcom_rmdir(const char *path)
{
    return ipcom_unlink_entry(path, IP_TRUE);
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
    Ipcom_filedir      *filedir;
    Ipcom_file_dirent  *dirent;
    int  retval;
    char tmp_path[IPCOM_FILE_DIRNAME_MAX];

    if (ipcom_once(&ipcom_file_once, ipcom_file_init, IP_NULL) != IPCOM_SUCCESS)
    {
        IP_PANIC();
        return IP_NULL;
    }

    IPFILE_LOCK();

    /* Get the directory. */
    filedir = ipcom_file_getdir(tmp_path, (char *)dirname, IP_NULL, &retval, IP_NULL);
    if(filedir == IP_NULL)
        goto fail;

    dirent = ipcom_malloc(sizeof(Ipcom_file_dirent));
    if(dirent == IP_NULL)
    {
        retval = IP_ERRNO_ENOMEM;
        goto fail;
    }

    dirent->last_ino = 0;
    retval = ipcom_file_make_name(filedir, dirent->path, sizeof(dirent->path));
    ip_assert(retval < IPCOM_FILE_DIRNAME_MAX);

    IPFILE_UNLOCK();
    return (IP_DIR *)dirent;

 fail:
    ipcom_errno = retval;
    IPFILE_UNLOCK();
    return IP_NULL;
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
    Ipcom_filedir      *filedir, *best;
    Ipcom_file_dirent  *dirent = (Ipcom_file_dirent *)dirp;
    int  retval;
    char tmp_path[IPCOM_FILE_DIRNAME_MAX];

    IPFILE_LOCK();

    /* Get the directory. */
    filedir = ipcom_file_getdir(tmp_path, dirent->path, IP_NULL, &retval, IP_NULL);
    if(filedir == IP_NULL)
        goto fail;

    /* Get next entry. */
    best = IP_NULL;
    for(filedir = filedir->child; filedir != IP_NULL; filedir = filedir->next)
    {
        ip_assert(filedir->ino != 0);
        if(filedir->ino <= dirent->last_ino)
            continue;
        if(best == IP_NULL || filedir->ino < best->ino)
            best = filedir;
    }

    if(best != IP_NULL)
    {
        dirent->last_ino = best->ino;
        dirent->dirent.d_namlen = ipcom_strlen(best->name);
        ipcom_strncpy(dirent->dirent.d_name, best->name, sizeof(dirent->dirent.d_name) - 1);
        if(IPCOM_FILE_IS_FILE(best))
            dirent->dirent.d_namlen--;
        dirent->dirent.d_name[dirent->dirent.d_namlen] = '\0';
        IPFILE_UNLOCK();
        return &dirent->dirent;
    }

    /* Not found, do not change errno. */
    IPFILE_UNLOCK();
    return IP_NULL;

 fail:
    ipcom_errno = retval;
    IPFILE_UNLOCK();
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
    ipcom_free(dirp);
    return 0;
}


/*
 *===========================================================================
 *                    ipcom_unlink
 *===========================================================================
 * Description:   Remove file or directory.
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipcom_unlink(const char *pathname)
{
    return ipcom_unlink_entry(pathname, IP_FALSE);
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
ipcom_rename(const char *xold, const char *xnew)
{
    Ipcom_filedir *old_filedir, *old_parent;
    Ipcom_filedir *new_filedir, *new_parent;
    int retval;
    char *entry;
    char *new_name;
    char tmp_path[IPCOM_FILE_DIRNAME_MAX];

    IPFILE_LOCK();

    /* Get old parent directory. */
    old_parent = ipcom_file_getdir(tmp_path, (char *)xold, &entry, &retval, IP_NULL);
    if(old_parent == IP_NULL)
        goto leave;

    /* Get old file/directory. */
    old_filedir = ipcom_file_get_entry(old_parent, entry);
    if(old_filedir == IP_NULL)
    {
        retval = IP_ERRNO_ENOENT;
        goto leave;
    }

    /* Get new parent directory. */
    new_parent = ipcom_file_getdir(tmp_path, (char *)xnew, &entry, &retval, IP_NULL);
    if(new_parent == IP_NULL)
        goto leave;

    /* Verify new name. */
    retval = ipcom_file_valid_name(entry);
    if(retval != 0)
        goto leave;

    /* Get new file/directory. */
    new_filedir = ipcom_file_get_entry(new_parent, entry);

    /* Special case, rename to same name. */
    if(old_filedir == new_filedir)
    {
        retval = 0;
        goto leave;
    }

    if(IPCOM_FILE_IS_FILE(old_filedir))
    {
        /* Old specifies a file. */
        if(new_filedir != IP_NULL && IPCOM_FILE_IS_DIR(new_filedir))
        {
            /* New specifies an existing directory. */
            retval = IP_ERRNO_EISDIR;
            goto leave;
        }
    }
    else
    {
        Ipcom_filedir *tmp_filedir;

        /* Old specifies a directory. */
        if(new_filedir)
        {
            if(IPCOM_FILE_IS_FILE(new_filedir))
            {
                /* New specifies an existing file. */
                retval = IP_ERRNO_ENOTDIR;
                goto leave;
            }
            if(new_filedir->child != IP_NULL)
            {
                /* New is a non-empty directory. */
                retval = IP_ERRNO_ENOTEMPTY;
                goto leave;
            }
        }

        for(tmp_filedir = new_parent->parent; tmp_filedir; tmp_filedir = tmp_filedir->parent)
            if(tmp_filedir == old_parent)
            {
                /* New can't contain a old path-prefix. */
                retval = IP_ERRNO_EINVAL;
                goto leave;
            }
    }

    /* Remove existing file/directory if any. */
    if(new_filedir != IP_NULL)
    {
        ip_assert(new_filedir->child == IP_NULL);
        ipcom_file_delete_entry(new_filedir);
    }

    /* Allocate new name memory. */
    new_name = ipcom_strdup((const char *)entry);
    if(new_name == IP_NULL)
    {
        retval = IP_ERRNO_ENOMEM;
        goto leave;
    }

    if(new_parent != old_parent)
    {
        /* Move entry from old to new directory. */
        ipcom_file_remove_entry(old_filedir);
        ipcom_file_insert_entry(new_parent, old_filedir);
    }

    /* Replace name. */
    ipcom_free(old_filedir->name);
    old_filedir->name = new_name;

    /* Success, fall through. */
    retval = 0;

 leave:
    if(retval)
    {
        ipcom_errno = retval;
        retval = -1;
    }
    IPFILE_UNLOCK();
    return retval;
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
    Ip_fd  fd;
    int omode = IP_O_RDWR;

    if (ipcom_once(&ipcom_file_once, ipcom_file_init, IP_NULL) != IPCOM_SUCCESS)
    {
        IP_PANIC();
        return IP_NULL;
    }

    if(ipcom_strchr(mode, 'w'))
        IP_BIT_SET(omode, IP_O_CREAT);
    if(ipcom_strchr(mode, 'a'))
        IP_BIT_SET(omode, IP_O_APPEND | IP_O_CREAT);

    fd = ipcom_fileopen(filename, omode, 0);
    if(fd < 3)
        return IP_NULL;
    return (IP_FILE *)fd;
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
    return ipcom_fileclose((Ip_fd)stream);
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
IP_PUBLIC int
ipcom_fileopen(const char *pathname, int omode, Ip_mode_t attrs)
{
    int  fd, retval;
    char *entry;
    Ipcom_filedir *filedir, *parent;
    char tmp_path[IPCOM_FILE_DIRNAME_MAX];

    if (ipcom_once(&ipcom_file_once, ipcom_file_init, IP_NULL) != IPCOM_SUCCESS)
    {
        IP_PANIC();
        ipcom_errno = IP_ERRNO_EINTERNAL;
        return -1;
    }

    (void)attrs;

    IPFILE_LOCK();

    /* Allocate a descriptor. */
    fd = ipcom_file_fd_malloc(&retval);
    if(fd == -1)
        goto fail;

    /* Get parent directory. */
    parent = ipcom_file_getdir(tmp_path, (char *)pathname, &entry, &retval, IP_NULL);
    if(parent == IP_NULL)
        goto fail;

    /* Get the file. */
    filedir = ipcom_file_get_entry(parent, entry);
    if(filedir == IP_NULL && !IP_BIT_ISSET(omode, IP_O_CREAT))
    {
        retval = IP_ERRNO_ENOENT;
        goto fail;
    }

    /* Found entry, check that it is a file and optionally NULL it. */
    if(filedir != IP_NULL)
    {
        if(IPCOM_FILE_IS_DIR(filedir))
        {
            retval = IP_ERRNO_EISDIR;
            goto fail;
        }

        /* Create and no append -> reset file contents. */
        if(IP_BIT_ISSET(omode, IP_O_CREAT) && !IP_BIT_ISSET(omode, IP_O_APPEND))
        {
            Ipcom_file_buf   *data;
            while(filedir->f.file.data != IP_NULL)
            {
                data = filedir->f.file.data;
                filedir->f.file.data = data->next;
                ipcom_free(data);
            }
            filedir->f.file.tot_length = 0;
#ifdef IPCOM_USE_TIME
            filedir->mtime = ipcom_time(0);
#endif
        }
    }

    /* No file -> create it. */
    if(filedir == IP_NULL)
    {
        /* Verify entry name. */
        retval = ipcom_file_valid_name(entry);
        if(retval != 0)
            goto fail;

        /* Create the entry. */
        filedir = ipcom_file_create_entry(parent, entry);
        if(filedir == IP_NULL)
        {
            retval = IP_ERRNO_ENOMEM;
            goto fail;
        }

        /* It is a file. */
        IP_BIT_SET(filedir->flags, IPCOM_FILE_FLAG_ISFILE);
        IPCOM_LOG2(DEBUG, "ipcom_fileopen(%d) :: '%s' [created]."IP_LF, fd, entry);
    }
    else
    {
        IPCOM_LOG2(DEBUG, "ipcom_fileopen(%d) :: '%s'."IP_LF, fd, entry);
    }

    /* Init the file descriptor. */
    ip_assert(ipcom_file.filedesc[fd] != IP_NULL);
    ipcom_file.filedesc[fd]->file  = filedir;
    ipcom_file.filedesc[fd]->omode = omode;

    /* Success. */
    IPFILE_UNLOCK();
    return fd;

 fail:
    if(fd != -1)
    {
        ip_assert(ipcom_file.filedesc[fd]);
        ipcom_free(ipcom_file.filedesc[fd]);
        ipcom_file.filedesc[fd] = IP_NULL;
    }
    ipcom_errno = retval;
    IPFILE_UNLOCK();
    return -1;
}


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
ipcom_fileclose(Ip_fd fd)
{
    Ipcom_file_desc  *desc;
    int retval;

    IPFILE_LOCK();

    if(fd < 3 || fd >= IPCOM_FILE_FD_SETSIZE || ipcom_file.filedesc[fd] == IP_NULL)
    {
        retval = IP_ERRNO_EBADF;
        goto leave;
    }
    desc = ipcom_file.filedesc[fd];

    if(desc->file != IP_NULL)
    {
        IPCOM_LOG2(DEBUG, "ipcom_fileclose(%d) :: file '%s'."IP_LF, fd, desc->file->name);
    }

    ipcom_file.filedesc[fd] = IP_NULL;
    ipcom_free(desc);

    IPFILE_UNLOCK();
    return 0;

 leave:
    ipcom_errno = retval;
    IPFILE_UNLOCK();
    return -1;
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
    (void)mode;  /* ignore and hope it is the same...*/

    return (IP_FILE *)fd;
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
    return (Ip_fd)stream;
}


/*
 *===========================================================================
 *                    ipcom_fstat
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipcom_fstat(int fd, struct Ip_stat *buf)
{
    Ipcom_file_desc  *desc;
    int retval;

    desc = ipcom_file_get_desc(fd, &retval);
    if(desc == IP_NULL)
        goto leave;

    ipcom_file_stat_init(desc->file, buf);

    IPFILE_UNLOCK();
    return 0;

 leave:
    ipcom_errno = retval;
    IPFILE_UNLOCK();
    return -1;
}


/*
 *===========================================================================
 *                    ipcom_stat
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC int
ipcom_stat(const char *pathname, struct Ip_stat *buf)
{
    Ipcom_filedir      *parent, *filedir;
    int  retval;
    char *entry;
    char tmp_path[IPCOM_FILE_DIRNAME_MAX];

    if (ipcom_once(&ipcom_file_once, ipcom_file_init, IP_NULL) != IPCOM_SUCCESS)
    {
        IP_PANIC();
        ipcom_errno = IP_ERRNO_EINTERNAL;
        return -1;
    }

    IPFILE_LOCK();

    /* Get parent directory. */
    parent = ipcom_file_getdir(tmp_path, (char *)pathname, &entry, &retval, IP_NULL);
    if(parent == IP_NULL)
        goto leave;

    /* Get the file/directory. */
    filedir = ipcom_file_get_entry(parent, entry);
    if(filedir == IP_NULL)
    {
        retval = IP_ERRNO_ENOENT;
        goto leave;
    }

    ipcom_file_stat_init(filedir, buf);

    IPFILE_UNLOCK();
    return 0;

 leave:
    ipcom_errno = retval;
    IPFILE_UNLOCK();
    return -1;
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
    (void)stream;
    return 0;
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
    Ipcom_file_desc  *desc;
    int retval;

#if IPCOM_USE_SHELL == IPCOM_SHELL_IPCOM
    if(stream == ip_stdin)
        return 0;
#endif

    desc = ipcom_file_get_desc((Ip_fd)stream, &retval);
    if(desc == IP_NULL)
        goto leave;

    ip_assert(desc->file != IP_NULL);
    ip_assert(IPCOM_FILE_IS_FILE(desc->file));
    ip_assert(desc->offset <= desc->file->f.file.tot_length);

    if(desc->offset >= desc->file->f.file.tot_length)
        retval = 1;
    else
        retval = 0;

    /* Success. */
    IPFILE_UNLOCK();
    return retval;

 leave:
    ipcom_errno = retval;
    IPFILE_UNLOCK();
    return -1;
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
    Ipcom_file_desc  *desc;
    int retval;

    desc = ipcom_file_get_desc((Ip_fd)stream, &retval);
    if(desc == IP_NULL)
        goto leave;

    retval = desc->offset;

    /* Success. */
    IPFILE_UNLOCK();
    return retval;

 leave:
    ipcom_errno = retval;
    IPFILE_UNLOCK();
    return -1;

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
    Ipcom_file_desc  *desc;
    int retval;

    desc = ipcom_file_get_desc((Ip_fd)stream, &retval);
    if(desc == IP_NULL)
        goto leave;

    ip_assert(desc->file != IP_NULL);
    ip_assert(IPCOM_FILE_IS_FILE(desc->file));

    switch(whence)
    {
    case IP_SEEK_SET :   /* start of the file. */
        if(offset < 0 || offset > desc->file->f.file.tot_length)
            goto einval;
        desc->offset = offset;
        break;
    case IP_SEEK_CUR :   /* the current position indicator. */
        if((desc->offset + offset) < 0 || (desc->offset + offset) > desc->file->f.file.tot_length)
            goto einval;
        desc->offset += offset;
        break;
    case IP_SEEK_END :   /* end-of-file. */
        if(offset > 0 || offset < -desc->file->f.file.tot_length)
            goto einval;
        desc->offset = desc->file->f.file.tot_length - offset;
        break;
    default:
    einval:
        retval = IP_ERRNO_EINVAL;
        goto leave;
    }

    ip_assert(desc->offset >= 0);
    ip_assert(desc->offset <= desc->file->f.file.tot_length);

    /* Success. */
    IPFILE_UNLOCK();
    return 0;

 leave:
    ipcom_errno = retval;
    IPFILE_UNLOCK();
    return -1;
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
    (void)ipcom_fseek(stream, 0, IP_SEEK_SET);
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
IP_PUBLIC Ip_off_t
ipcom_lseek(Ip_fd fd, Ip_off_t offset, int whence)
{
    Ipcom_file_desc  *desc;
    int retval;

    desc = ipcom_file_get_desc(fd, &retval);
    if(desc == IP_NULL)
        goto leave;

    ip_assert(desc->file != IP_NULL);
    ip_assert(IPCOM_FILE_IS_FILE(desc->file));

    switch(whence)
    {
    case IP_SEEK_SET :   /* start of the file. */
        if(offset < 0)
            goto einval;
        break;
    case IP_SEEK_CUR :   /* the current position indicator. */
        if((desc->offset + offset) < 0)
            goto einval;
        offset += desc->offset;
        break;
    case IP_SEEK_END :   /* end-of-file. */
        if(offset < -desc->file->f.file.tot_length)
            goto einval;
        offset = desc->file->f.file.tot_length - offset;
        break;
    default:
    einval:
        retval = IP_ERRNO_EINVAL;
        goto leave;
    }

    ip_assert(offset >= 0);

    /*!!fixme: allow outside max. */
    ip_assert(offset <= desc->file->f.file.tot_length);

    desc->offset = offset;

    /* Success. */
    IPFILE_UNLOCK();
    return offset;

 leave:
    ipcom_errno = retval;
    IPFILE_UNLOCK();
    return (Ip_off_t)-1;
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
    if((IP_FILE *)fd == ip_stdout || (IP_FILE *)fd == ip_stderr)
        return ipcom_shell_write(fd, buffer, nbyte);
#endif

    return (Ip_ssize_t)ipcom_file_writebuf(fd, (char *)buffer, (int)nbyte, IP_TRUE);
}


/*
 *===========================================================================
 *                    ipcom_fwrite
 *===========================================================================
 * Description:
 * Parameters:
 * Returns:
 *
 */
IP_PUBLIC Ip_size_t
ipcom_fwrite(const void *ptr, Ip_size_t size, Ip_size_t nitems, IP_FILE *stream)
{
    int retval;

    if (size == 0 || nitems == 0)
        return 0;

#if IPCOM_USE_SHELL == IPCOM_SHELL_IPCOM
    if(stream == ip_stdout || stream == ip_stderr)
        return ipcom_shell_fwrite(ptr, size, nitems, stream);
#endif

    retval = ipcom_file_writebuf((Ip_fd)stream, (char *)ptr, (int)(size * nitems), IP_TRUE);
    if (retval < 0)
        return 0;
    return (Ip_size_t)(retval / size);
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
ipcom_fileread(Ip_fd fd, void *buf, Ip_size_t nbyte)
{
    Ipcom_file_desc  *desc;
    int retval;

    desc = ipcom_file_get_desc(fd, &retval);
    if(desc == IP_NULL)
        goto leave;

    retval = ipcom_file_readbuf(desc->file, desc->offset, (char *)buf, (int)nbyte);

 leave:
    if(retval > 0)
    {
        ipcom_errno = retval;
        IPCOM_LOG2(DEBUG, "ipcom_fileread(%d) :: failed, errno = %d."IP_LF, fd, ipcom_errno);
        retval = -1;
    }
    else if(retval < 0)
    {
        retval = -retval;   /*!! number of bytes read (may be < nbyte due to smaller file). */
        /*IPCOM_LOG2(DEBUG, "ipcom_fileread(%d) :: read %d bytes."IP_LF, fd, retval);*/
        desc->offset += retval;
    }

    IPFILE_UNLOCK();
    return retval;
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
    Ip_size_t  ret;

#if IPCOM_USE_SHELL == IPCOM_SHELL_IPCOM
    if(stream == ip_stdin)
        return ipcom_shell_fread(ptr, size, nitems, stream);
#endif

    ret = (Ip_size_t)ipcom_fileread((Ip_fd)stream, ptr, size * nitems);
    return ret;
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
    if(file == ip_stdout || file == ip_stderr)
        return 0;
#endif

    return 0;
}

#else
int ipcom_file_empty_file;
#endif /* IPCOM_USE_FILE == IPCOM_FILE_IPCOM */


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */

