/*
 ******************************************************************************
 *                     SOURCE FILE
 *
 *     Document no: @(#) $Name: VXWORKS_ITER18A_FRZ10 $ $RCSfile: ipcom_cmd_shellcmds.c,v $ $Revision: 1.10 $
 *     $Source: /home/interpeak/CVSRoot/ipcom/src/ipcom_cmd_shellcmds.c,v $
 *     $Author: ulf $
 *     $State: Exp $ $Locker:  $
 *
 *     INTERPEAK_COPYRIGHT_STRING
 *     Design and implementation by Markus Carlstedt <markus@interpeak.se>
 ******************************************************************************
 */

/*
 ****************************************************************************
 * 1                    DESCRIPTION
 ****************************************************************************
 *
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

/* Ipcom include files */
#define IPCOM_USE_CLIB_PROTO
#include "ipcom_cstyle.h"
#include "ipcom_clib.h"
#include "ipcom_time.h"
#include "ipcom_err.h"
#include "ipcom_os.h"
#include "ipcom_shell.h"
#include "ipcom_getopt.h"


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
 * 6                    EXTERN PROTOTYPES
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
 * 9                    LOCAL FUNCTIONS
 ****************************************************************************
 */

/*
 ****************************************************************************
 * 10                   PUBLIC FUNCTIONS
 ****************************************************************************
 */

/*
 *===========================================================================
 *                    ipcom_cmd_mkdir
 *===========================================================================
 * Description:     Shell command: Create directory
 * Parameters:      argc - number of arguments
 *                  argv - pointer to pointer to the arguments
 * Returns:         0 or -1
 *
 */
#if IPCOM_USE_FILE != IPCOM_FILE_NONE
IP_PUBLIC int
ipcom_cmd_mkdir(int argc, char **argv)
{
    if (argc <= 1)
    {
        ipcom_printf("mkdir: missing directory argument\n");
        return -1;
    }

    if (ipcom_mkdir(argv[1], 0777))
    {
        ipcom_printf("mkdir: cannot create directory '%s': %s\n", argv[1], ipcom_strerror(ipcom_errno));
        return -1;
    }

    return 0;
}
#endif /* IPCOM_USE_FILE != IPCOM_FILE_NONE */


/*
 *===========================================================================
 *                    ipcom_cmd_cd
 *===========================================================================
 * Description:     Shell command: Change directory
 * Parameters:      argc - number of arguments
 *                  argv - pointer to pointer to the arguments
 * Returns:         0
 *
 */
#if IPCOM_USE_FILE != IPCOM_FILE_NONE
IP_PUBLIC int
ipcom_cmd_cd(int argc, char **argv)
{
    if (argc <= 1)
    {
        if (ipcom_chdir(IPCOM_FILE_ROOT) != 0)
        {
            ipcom_printf("cd: No such directory\n");
            return -1;
        }
        return 0;
    }

    if (ipcom_chdir(argv[1]) != 0)
    {
        ipcom_printf("cd: '%s': No such directory\n", argv[1]);
        return -1;
    }

    return 0;
}
#endif /* IPCOM_USE_FILE != IPCOM_FILE_NONE */


/*
 *===========================================================================
 *                    ipcom_cmd_cat
 *===========================================================================
 * Description:     Shell command: Print file
 * Parameters:      argc - number of arguments
 *                  argv - pointer to pointer to the arguments
 * Returns:         0 or -1
 *
 */
#if IPCOM_USE_FILE != IPCOM_FILE_NONE
IP_PUBLIC int
ipcom_cmd_cat(int argc, char **argv)
{
    IP_FILE  *fp;
    Ip_size_t num;
    char     *buf;

    if (argc <= 1)
    {
        ipcom_printf("cat: missing file argument\n");
        return -1;
    }

    /* Open file */
    fp = ipcom_fopen(argv[1], "rb");
    if (fp == IP_NULL)
    {
        ipcom_printf("cat: cannot access:'%s'\n", argv[1]);
        return -1;
    }

    /* Get temporary memory */
    buf = ipcom_malloc(IPCOM_STDIO_BUFSIZE);
    if (buf == IP_NULL)
    {
        ipcom_printf("cat: out of memory\n");
        (void)ipcom_fclose(fp);
        return -1;
    }

    /* Print file contents */
    while((1))
    {
        num = ipcom_fread(buf, 1, IPCOM_STDIO_BUFSIZE, fp);
        if (num <= 0)
            break;
        if (ipcom_fwrite(buf, 1, num, ip_stdout) != num)
            break;
    }

    ipcom_free(buf);
    (void)ipcom_fclose(fp);

    return 0;
}
#endif /* IPCOM_USE_FILE != IPCOM_FILE_NONE */


/*
 *===========================================================================
 *                    ipcom_cmd_uuencode
 *===========================================================================
 * Description:     Shell command: Print file
 * Parameters:      argc - number of arguments
 *                  argv - pointer to pointer to the arguments
 * Returns:         0 or -1
 *
 */
#if IPCOM_USE_FILE != IPCOM_FILE_NONE
IP_PUBLIC int
ipcom_cmd_uuencode(int argc, char **argv)
{
#define CHUNK_SIZE 45

    IP_FILE *fp;

    if (argc <= 1)
    {
        ipcom_printf("uuencode: missing file argument\n");
        return -1;
    }

    /* Open file */
    fp = ipcom_fopen(argv[1], "rb");
    if (fp == IP_NULL)
    {
        ipcom_printf("uuencode: cannot access:'%s'\n", argv[1]);
        return -1;
    }

    ipcom_printf("begin 644 %s\n", argv[1]);

    /* Print file contents */
    while((1))
    {
        unsigned char in[CHUNK_SIZE + 2];
        char out[5];
        int num;
        int i;

        num = ipcom_fread(in, 1, CHUNK_SIZE, fp);
        if (num <= 0)
            break;

        ipcom_printf("%c", num + ' ');

        for (i = 0; i < num; i += 3)
        {
            Ip_u32 value;

            value = in[i + 0];
            value = (value << 8) + in[i + 1];
            value = (value << 8) + in[i + 2];

            out[0] = ((value >> 18) & 0x3F) + ' ';
            out[1] = ((value >> 12) & 0x3F) + ' ';
            out[2] = ((value >> 6) & 0x3F) + ' ';
            out[3] = (value & 0x3F) + ' ';
            out[4] = '\0';

            ipcom_printf("%s", out);
        }

        ipcom_printf("\n");
    }

    ipcom_printf(" \nend\n");
    (void)ipcom_fclose(fp);

    return 0;
}
#endif /* IPCOM_USE_FILE != IPCOM_FILE_NONE */


/*
 *===========================================================================
 *                    ipcom_cmd_cp
 *===========================================================================
 * Description:     Shell command: Copy file
 * Parameters:      argc - number of arguments
 *                  argv - pointer to pointer to the arguments
 * Returns:         0 or -1
 *
 */
#if IPCOM_USE_FILE != IPCOM_FILE_NONE
IP_PUBLIC int
ipcom_cmd_cp(int argc, char **argv)
{
    IP_FILE *src, *dst;
    Ip_size_t num;
    int   ret = 0;
    char *buf;

    /* Force specification of both files */
    if (argc <= 2)
    {
        ipcom_printf("cp: missing file arguments\n");
        return -1;
    }

    /* Open source file */
    src = ipcom_fopen(argv[1], "rb");
    if (src == IP_NULL)
    {
        ipcom_printf("cp: cannot access:'%s'\n", argv[1]);
        return -1;
    }

    /* Create destination file */
    dst = ipcom_fopen(argv[2], "wb");
    if (dst == IP_NULL)
    {
        ipcom_printf("cp: cannot access:'%s'\n", argv[2]);
        (void)ipcom_fclose(src);
        return -1;
    }

    /* Get temporary memory */
    buf = ipcom_malloc(IPCOM_STDIO_BUFSIZE);
    if (buf == IP_NULL)
    {
        ipcom_printf("cp: out of memory\n");
        ret = -1;
        goto leave;
    }

    /* Copy file contents */
    while((1))
    {
        num = ipcom_fread(buf, 1, IPCOM_STDIO_BUFSIZE, src);
        if (num <= 0)
            break;
        if (ipcom_fwrite(buf, 1, num, dst) != num)
        {
            ipcom_printf("cp: failed to write %d bytes\n", num);
            break;
        }
    }
    ipcom_free(buf);

 leave:
    (void)ipcom_fclose(src);
    (void)ipcom_fclose(dst);

    return ret;
}
#endif /* IPCOM_USE_FILE != IPCOM_FILE_NONE */


/*
 *===========================================================================
 *                    ipcom_cmd_pwd
 *===========================================================================
 * Description:     Shell command: Show current directory
 * Parameters:      argc - number of arguments
 *                  argv - pointer to pointer to the arguments
 * Returns:         0
 *
 */
#if IPCOM_USE_FILE != IPCOM_FILE_NONE
IP_PUBLIC int
ipcom_cmd_pwd(int argc, char **argv)
{
    char        *cbuf;
    Ip_size_t   csz = 80;

    (void)argc;
    (void)argv;

    for (;;)
    {
        cbuf = ipcom_malloc(csz);
        if (cbuf == IP_NULL)
            break;

        /**/
        if (ipcom_getcwd(cbuf, csz))
        {
            ipcom_printf(cbuf);
            ipcom_printf("\n");
            ipcom_free(cbuf);
            break;
        }

        ipcom_free(cbuf);
        if (csz == IP_PATH_MAX)
            break;
        csz = IP_MIN((csz * 2), IP_PATH_MAX);
    }

    return 0;
}
#endif /* IPCOM_USE_FILE != IPCOM_FILE_NONE */


/*
 *===========================================================================
 *                    ipcom_cmd_rm
 *===========================================================================
 * Description:     Shell command: Remove file.
 * Parameters:      argc - number of arguments
 *                  argv - pointer to pointer to the arguments
 * Returns:         0 or -1
 *
 */
#if IPCOM_USE_FILE != IPCOM_FILE_NONE
IP_PUBLIC int
ipcom_cmd_rm(int argc, char **argv)
{
    if (argc <= 1)
    {
        ipcom_printf("rm: missing file argument\n");
        return -1;
    }

    if (ipcom_unlink(argv[1]))
    {
        ipcom_printf("rm: cannot remove '%s': %s\n", argv[1], ipcom_strerror(ipcom_errno));
        return -1;
    }

    return 0;
}
#endif /* IPCOM_USE_FILE != IPCOM_FILE_NONE */


/*
 *===========================================================================
 *                    ipcom_cmd_rmdir
 *===========================================================================
 * Description:     Shell command: Remove directory
 * Parameters:      argc - number of arguments
 *                  argv - pointer to pointer to the arguments
 * Returns:         0 or -1
 *
 */
#if IPCOM_USE_FILE != IPCOM_FILE_NONE
IP_PUBLIC int
ipcom_cmd_rmdir(int argc, char **argv)
{
    if (argc <= 1)
    {
        ipcom_printf("rmdir: missing directory argument\n");
        return 0;
    }

    if (ipcom_rmdir(argv[1]))
    {
        ipcom_printf("rmdir: '%s': %s\n", argv[1], ipcom_strerror(ipcom_errno));
        return -1;
    }

    return 0;
}
#endif /* IPCOM_USE_FILE != IPCOM_FILE_NONE */


/*
 *===========================================================================
 *                    ipcom_cmd_mv
 *===========================================================================
 * Description:     Shell command: Move/rename file/directory
 * Parameters:      argc - number of arguments
 *                  argv - pointer to pointer to the arguments
 * Returns:         0 or -1
 *
 */
#if IPCOM_USE_FILE != IPCOM_FILE_NONE
IP_PUBLIC int
ipcom_cmd_mv(int argc, char **argv)
{
    if (argc <= 2)
    {
        ipcom_printf("mv: missing argument(s)\n");
        return 0;
    }

    if (ipcom_rename(argv[1], argv[2]))
    {
        ipcom_printf("mv: cannot move '%s' to '%s': %s\n", argv[1], argv[2], ipcom_strerror(ipcom_errno));
        return -1;
    }

    return 0;
}
#endif /* IPCOM_USE_FILE != IPCOM_FILE_NONE */


/*
 *===========================================================================
 *                    ipcom_cmd_ls
 *===========================================================================
 * Description:     Shell command: Remove directory
 * Parameters:      argc - number of arguments
 *                  argv - pointer to pointer to the arguments
 * Returns:         0 or -1
 *
 */
#if IPCOM_USE_FILE != IPCOM_FILE_NONE
IP_PUBLIC int
ipcom_cmd_ls(int argc, char **argv)
{
    IP_DIR *dirp;
    char *path;
    struct Ip_dirent *dir;
    struct Ip_stat sbuf;
    Ipcom_getopt opt;
    int c;
    int opt_l = 0;
    char  tbuf[128];
#ifdef IPCOM_USE_TIME
    int len;
#endif /* IPCOM_USE_TIME */

    /* Get options */
    ipcom_getopt_clear_r(&opt);
    while ((c = ipcom_getopt_r(argc, argv, "l", &opt)) != -1)
    {
        switch (c)
        {
        case 'l':
            opt_l = 1;
            break;
        default:
            ipcom_printf("ls: illegal option -- %c\n", (char)c);
            return -1;
        }
    }

    if (argc <= opt.optind)
        path = ".";
    else
        path = argv[opt.optind];

    /* Open the directory */
    dirp = ipcom_opendir(path);
    if (dirp == IP_NULL)
    {
        ipcom_printf("ls: '%s': No such directory\n", path);
        return -1;
    }

    /* Loop through the directory and print contents. */
    for (c = 1, dir = ipcom_readdir(dirp); dir != IP_NULL; dir = ipcom_readdir(dirp), c++)
    {
        if (opt_l)
        {
            if (ipcom_strlen(path) + ipcom_strlen(dir->d_name) + 2 <= sizeof(tbuf))
            {
                ipcom_strcpy(tbuf, path);
                /* Provide a separator if path was given without and format is known */
                if (path[ipcom_strlen(path)-1] != '/')
                    ipcom_strcat(tbuf, "/");
                ipcom_strcat(tbuf, dir->d_name);
                if (ipcom_stat(tbuf, &sbuf) == 0)
                {
#ifdef IPCOM_USE_TIME
                    ipcom_strcpy(tbuf, ipcom_ctime(&sbuf.st_mtime));
                    len = ipcom_strlen(tbuf);
                    tbuf[len-6] = '\0';
                    ipcom_printf("%6ld  %15s  %s\n", sbuf.st_size, tbuf + 4, dir->d_name);
#else
                    ipcom_printf("%6ld  %s\n", sbuf.st_size, dir->d_name);
#endif /* IPCOM_USE_TIME */
                }
            }
        }
        else
        {
            ipcom_printf("%s ", dir->d_name);
            if ((c % 3) == 0)
                ipcom_printf("\n");
        }
    }
    if (opt_l == 0 && ((c-1) % 3) != 0)
        ipcom_printf("\n");

    /* Close the directory */
    (void)ipcom_closedir(dirp);

    return 0;
}
#endif /* IPCOM_USE_FILE != IPCOM_FILE_NONE */


/*
 *===========================================================================
 *                    ipcom_cmd_date
 *===========================================================================
 * Description:     Shell command: Show/Set current date.
 * Parameters:      argc - number of arguments
 *                  argv - pointer to pointer to the arguments
 * Returns:         0 or -1
 *
 */
#ifdef IPCOM_USE_TIME
IP_PUBLIC int
ipcom_cmd_date(int argc, char **argv)
{
    struct Ip_tm tm, *tmp;  /* local time */
    struct Ip_timeval tv;   /* UTC time */
    char *del1, *del2;

    (void)ipcom_gettimeofday(&tv, IP_NULL);
    tmp = ipcom_localtime((Ip_time_t *)&tv.tv_sec);
    if(tmp == IP_NULL)
        return 0;
    ipcom_memcpy(&tm, tmp, sizeof(tm));

    if (argc <= 1)
    {
        char       buf[26];

        /* print datestring. */
        ipcom_printf("%s\n", ipcom_asctime_r(&tm, buf));
        return 0;
    }

    /* Find first delimiter */
    del1 = ipcom_strchr(argv[1], '-');
    if (del1 == IP_NULL)
        goto fail;

    /* Find second delimiter */
    del2 = ipcom_strrchr(argv[1], '-');
    if (del2 == IP_NULL)
        goto fail;

    /* Check that we really found two '-' */
    if (del1 == del2)
        goto fail;

    tm.tm_year = ipcom_atol(argv[1]);
    if (tm.tm_year < 1970 || tm.tm_year > 2105)
        goto fail;
    tm.tm_year -= 1900;

    tm.tm_mon = ipcom_atol(del1+1);
    if (tm.tm_mon < 1 || tm.tm_mon > 12)
        goto fail;
    tm.tm_mon--;

    tm.tm_mday = ipcom_atol(del2+1);
    if (tm.tm_mday < 1 || tm.tm_mday > 31)
        goto fail;

    tm.tm_wday = 0;
    tm.tm_yday = 0;

    /* Set the date */
    tv.tv_sec  = (long)ipcom_mktime(&tm);
    tv.tv_usec = 0;
    if (ipcom_settimeofday(&tv, IP_NULL) != 0)
    {
        ipcom_printf("date: failed\n");
        return -1;
    }

    ipcom_printf("ok\n");
    return 0;

 fail:
    ipcom_printf("date: bad format [yyyy-mm-dd]\n");
    return -1;
}
#endif /* IPCOM_USE_TIME */


/*
 *===========================================================================
 *                    ipcom_cmd_time
 *===========================================================================
 * Description:     Shell command: Show/Set current time.
 * Parameters:      argc - number of arguments
 *                  argv - pointer to pointer to the arguments
 * Returns:         0
 *
 */
#ifdef IPCOM_USE_TIME
IP_PUBLIC int
ipcom_cmd_time(int argc, char **argv)
{
    struct Ip_tm tm, *tmp;  /* local time */
    struct Ip_timeval tv;   /* UTC time */
    char *del1, *del2;

    (void)ipcom_gettimeofday(&tv, IP_NULL);
    tmp = ipcom_localtime((Ip_time_t *)&tv.tv_sec);
    if(tmp == IP_NULL)
        return 0;
    ipcom_memcpy(&tm, tmp, sizeof(tm));

    if (argc <= 1)
    {
        char       buf[26];

        /* print datestring. */
        ipcom_printf("%s\n", ipcom_asctime_r(&tm, buf));
        return 0;
    }

    /* Find first delimiter */
    del1 = ipcom_strchr(argv[1], ':');
    if (del1 == IP_NULL)
        goto fail;

    /* Find second delimiter */
    del2 = ipcom_strrchr(argv[1], ':');
    if (del2 == IP_NULL)
        goto fail;

    /* Check that we really found two ':' */
    if (del1 == del2)
        goto fail;

    tm.tm_hour = ipcom_atol(argv[1]);
    if (tm.tm_hour < 0 || tm.tm_hour > 23)
        goto fail;

    tm.tm_min = ipcom_atol(del1+1);
    if (tm.tm_min < 0 || tm.tm_min > 59)
        goto fail;

    tm.tm_sec = ipcom_atol(del2+1);
    if (tm.tm_sec < 0 || tm.tm_sec > 59)
        goto fail;

    /* Set the time */
    tv.tv_sec  = (long)ipcom_mktime(&tm);
    tv.tv_usec = 0;
    if (ipcom_settimeofday(&tv, IP_NULL) != 0)
    {
        ipcom_printf("time: failed\n");
        return -1;
    }

    ipcom_printf("ok\n");
    return 0;

 fail:
    ipcom_printf("time: bad format [hh:mm:ss]\n");
    return -1;
}
#endif /* IPCOM_USE_TIME */


#if IPCOM_USE_ENV != IPCOM_ENV_NONE
/*
 *===========================================================================
 *                    ipcom_cmd_getenv
 *===========================================================================
 * Description:     Shell command: Get an environment variable
 * Parameters:      argc - number of arguments
 *                  argv - pointer to pointer to the arguments
 * Returns:         0
 *
 */
IP_PUBLIC int
ipcom_cmd_getenv(int argc, char **argv)
{
    char *value;

    if (argc < 2)
    {
        ipcom_printf("getenv: missing argument(s)\n");
        return -1;
    }

    value = ipcom_getenv(argv[1]);
    if (value != IP_NULL)
        ipcom_printf("getenv: %s = %s\n", argv[1], value);
    else
        ipcom_printf("getenv: %s not found\n", argv[1]);
    return 0;
}


/*
 *===========================================================================
 *                    ipcom_cmd_setenv
 *===========================================================================
 * Description:     Shell command: Set an environment variable
 * Parameters:      argc - number of arguments
 *                  argv - pointer to pointer to the arguments
 * Returns:         0
 *
 */
IP_PUBLIC int
ipcom_cmd_setenv(int argc, char **argv)
{
    int ret;

    if (argc < 3)
    {
        ipcom_printf("setenv: missing argument(s)\n");
        return -1;
    }

#if IPCOM_VR_MAX > 1
    if (ipcom_strcmp(argv[1], "VR") == 0)
    {
        ipcom_printf("setenv: permission denied\n");
        return -1;
    }
#endif

    ret = ipcom_setenv(argv[1], argv[2], 1);
    ipcom_printf("setenv: %s = %s\n", argv[1], argv[2]);

    return ret;
}
#endif /* #if IPCOM_USE_ENV != IPCOM_ENV_NONE */


/*
 ****************************************************************************
 *                      END OF FILE
 ****************************************************************************
 */
