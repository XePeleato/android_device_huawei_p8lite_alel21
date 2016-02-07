#ifndef __MDRV_RFILE_COMMON_H__
#define __MDRV_RFILE_COMMON_H__
#ifdef __cplusplus
extern "C"
{
#endif

    #pragma pack(4)

typedef unsigned int       DRV_INO_T;
typedef unsigned int       DRV_TIME;         /* type for file time fields */

#define DRV_NAME_MAX            255

#define DRV_S_IFDIR             0x4000          /* directory */
#define DRV_FILE_NULL            0              /* empty file*/
#define DRV_O_RDONLY            0               /* +1 == FREAD */
#define DRV_O_WRONLY            1               /* +1 == FWRITE */
#define DRV_O_RDWR              2               /* +1 == FREAD|FWRITE */
#define DRV_O_CREAT             0x0200          /* open with file create */
#define DRV_O_APPEND            0x0008          /* append (writes guaranteed at the end) */
#define DRV_O_TRUNC             0x0400          /* open with truncation */
#define DRV_O_EXCL              0x0800          /* error on open if file exists */
#define DRV_O_NONBLOCK          0x4000          /* non blocking I/O (POSIX style) */
#define DRV_O_NOCTTY            0x8000          /* don't assign a ctty on this open */

#define DRV_SEEK_SET            0               /* absolute offset, was L_SET */
#define DRV_SEEK_CUR            1               /* relative to current offset, was L_INCR */
#define DRV_SEEK_END            2               /* relative to end of file, was L_XTND */


    /* dirent */
    typedef struct tag_DRV_DIRENT_S
    {
        DRV_INO_T           d_ino;                      /* file serial number */
        char                d_name [DRV_NAME_MAX + 1];  /* file name, null-terminated */
    }DRV_DIRENT_S;

    /* Directory descriptor */
    typedef struct tag_DRV_DIR_S
    {
        int                 dd_fd;                      /* file descriptor for open directory */
        int                 dd_cookie;                  /* filesys-specific marker within dir */
        int                 dd_eof;                     /* readdir EOF flag */
        DRV_DIRENT_S        dd_dirent;                  /* obtained directory entry */
    }DRV_DIR_S;

    typedef struct tag_DRV_STAT_S
    {
        unsigned long       st_dev;     /* Device ID number */
        unsigned long       st_ino;     /* File serial number */
        int                 st_mode;    /* Mode of file */
        unsigned long       st_nlink;   /* Number of hard links to file */
        unsigned short      st_uid;     /* User ID of file */
        unsigned short      st_gid;     /* Group ID of file */
        unsigned long       st_rdev;    /* Device ID if special file */
        signed long long    st_size;    /* File size in bytes */
        DRV_TIME            st_atime;   /* Time of last access */
        DRV_TIME            st_mtime;   /* Time of last modification */
        DRV_TIME            st_ctime;   /* Time of last status change */
        long                st_blksize; /* File system block size */
        unsigned long       st_blocks;  /* Number of blocks containing file */
        unsigned char       st_attrib;  /* DOSFS only - file attributes */
        int                 st_reserved1;  /* reserved for future use */
        int                 st_reserved2;  /* reserved for future use */
        int                 st_reserved3;  /* reserved for future use */
        int                 st_reserved4;  /* reserved for future use */
    }DRV_STAT_S;

    void *mdrv_file_open(const char *path, const char *mode);
    int mdrv_file_close(void *fp);
    int mdrv_file_read(void *ptr, unsigned int size, unsigned int number, void *stream);
    int mdrv_file_write(void *ptr, unsigned int size, unsigned int number, void *stream);
    int mdrv_file_seek(void *stream, long offset, int whence);
    long mdrv_file_tell(void *stream);
    int mdrv_file_remove(const char *pathname);
    int mdrv_file_mkdir(const char *dirName);
    int mdrv_file_rmdir(const char *path);
    DRV_DIR_S* mdrv_file_opendir(const char *dirName);
    DRV_DIRENT_S* mdrv_file_readdir(DRV_DIR_S *dirp);
    int mdrv_file_closedir(DRV_DIR_S *dirp);
    int mdrv_file_get_stat(const char *path, DRV_STAT_S* buf);
    unsigned long mdrv_file_get_errno(void);
    int mdrv_file_access(const char *path, int amode);
    int mdrv_file_rename(const char *oldname, const char *newname);
    int mdrv_file_xcopy(const char *source,const char *dest);
    int mdrv_file_xdelete(const char *source);

    #pragma pack(0)
#ifdef __cplusplus
}
#endif
#endif
