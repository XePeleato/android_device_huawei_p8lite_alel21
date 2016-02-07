
/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : test_rfile_process.cpp
  版 本 号   : 初稿
  作    者   :
  生成日期   : 2013年01月29日
  最近修改   :
  功能描述   : UT测试脚本
  函数列表   :
  修改历史   :
  1.日    期   : 2013年01月29日
    作    者   :
    修改内容   : 创建文件

******************************************************************************/

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "gtest/gtest.h"
#include "llt_mockcpp.h"


using namespace testing;

#ifdef __cplusplus
extern "C"
{
#endif

/* 设备的打开参数 */
typedef struct tagUDI_OPEN_PARAM
{
	unsigned int devid;	     /* 设备ID */
	void   *pPrivate;		/* 模块特有的数据 */
} UDI_OPEN_PARAM_S;

typedef struct
{
   unsigned int opsPid;
   unsigned int totalFrame;
   unsigned int curFrame;
   unsigned int curlen;
   int reserve;
}RFILE_MNTN_HEADER;

typedef struct
{
    unsigned int opsType;
    int fp;
    unsigned int size;
}RFILE_READ_HEAD;

typedef struct
{
    unsigned int opsType;
    unsigned int partition_name_len;
    unsigned int offset;
    unsigned int length;
}RFILE_MASSREAD_HEAD;

/* dirent */
typedef struct dirent
{
    unsigned int       d_ino;
    char                d_name [256];
}DRV_DIRENT_STRU;


typedef struct
{
    int                 dd_fd;
    int                 dd_cookie;
    int                 dd_eof;
    DRV_DIRENT_STRU     dd_dirent;
}DRV_DIR_STRU;

typedef struct _stat
{
    unsigned int       st_dev;     /* Device ID number */
    unsigned int       st_ino;     /* File serial number */
    int                 st_mode;    /* Mode of file */
    unsigned int       st_nlink;   /* Number of hard links to file */
    unsigned short      st_uid;     /* User ID of file */
    unsigned short      st_gid;     /* Group ID of file */
    unsigned int       st_rdev;    /* Device ID if special file */
    signed   int       st_size;    /* File size in bytes */
    int                st_blksize; /* File system block size */
    unsigned int       st_blocks;  /* Number of blocks containing file */
    unsigned char       st_attrib;  /* DOSFS only - file attributes */
    int                 st_reserved1;  /* reserved for future use */
    int                 st_reserved2;  /* reserved for future use */
    int                 st_reserved3;  /* reserved for future use */
    int                 st_reserved4;
}DRV_STAT_STRU;

struct linux_dirent {
    unsigned long   d_ino;
    unsigned long   d_off;
    unsigned short  d_reclen;
    char            d_name[1];
};

#define DRV_NAME_MAX        255

struct linux_dirent_rfile {
    unsigned int   d_ino;
    unsigned int   d_off;
    unsigned short  d_reclen;
    char            d_name[DRV_NAME_MAX + 1];
};

typedef struct
{
    int blk_begin;
    int blk_end;
    int last_blk_offset;
    int first_blk_offset;
    int physical_offset;
    int reserv[4];
}RFILE_MASS_RW_PARA;

struct mtd_info_user {
 unsigned char type;
 unsigned int flags;
 unsigned int size;
 unsigned int erasesize;
 unsigned int writesize;
 unsigned int oobsize;
 unsigned int ecctype;
 unsigned int eccsize;
};
int test_fmasswriteProxy_two_frame_success( void );
int test_fmasswriteProxy_one_frame_success( void );
int test_fmasswriteProxy_wrong_pkg( void );
int test_fmassreadProxy_two_frame_success( void );
int test_fmassreadProxy_one_frame_success( void );
int test_fmassreadProxy_wrong_pkg( void );
extern int test_bsp_mass_read_path_err(void);
extern int test_bsp_mass_write_path_err(void);
extern int test_fwriteProxy_fwrite_checkfp_null(void);
extern int test_bsp_ccpu_rest_rfile_handle(void);
extern int test_fp_list_delete_dir_fail(void);
extern int test_fp_list_delete_dir_success(void);
extern int test_fp_list_add_dir_same(void);
extern int test_fp_list_check_dir_fail(void);
extern int test_fp_list_check_dir_success(void);
extern int test_fp_list_check_dir(void);
extern int test_fp_list_add_check_dir(void);
extern int test_fp_list_add_dir(void);
extern int test_fp_list_delete_file_fail(void);
extern int test_fp_list_delete_file_success(void);
extern int test_fp_list_add_file_same(void);
extern int test_fp_list_check_file_fail(void);
extern int test_fp_list_check_file_success(void);
extern int test_fp_list_check_file(void);
extern int test_fp_list_add_check_file(void);
extern int test_fp_list_add_file(void);
extern int test_debug_print_mnt_header( void );
extern int test_rfile_regProxyFunc( void );
extern int test_rfile_write_ccpu_pkt( void );
extern int test_rfile_read_ccpu_data( void );
extern int test_rfile_calc_read_frame_num( int bytes );
extern int test_rfile_calc_massread_frame_num( int bytes );
extern int test_rfile_sflags( char *mode,int *flag);
extern int test_rfile_xcopy_file( void  );
extern int test_rfile_get_effectblk_addr( void );
extern int test_rfile_mass_write( void );
extern int test_rfile_get_massread_op_para_buf_null( void );
extern int test_rfile_get_massread_op_para_partion_name_null( void );
extern int test_rfile_get_massread_op_para_offset_null( void );
extern int test_rfile_get_massread_op_para_length_null( void );
extern int test_rfile_get_massread_op_para_massread_head_null( void );
extern int test_rfile_get_massread_op_para_success( void );
extern int test_rfile_get_masswrite_op_para_buf_null( void );
extern int test_rfile_get_masswrite_op_para_partion_name_null( void );
extern int test_rfile_get_masswrite_op_para_offset_null( void );
extern int test_rfile_get_masswrite_op_para_length_null( void );
extern int test_rfile_get_masswrite_op_para_masswrite_head_null( void );
extern int test_rfile_get_masswrite_op_para_success( void );
extern int test_rfile_mass_read_all_buf_null( void );
extern int test_rfile_mass_read_all_buf_temp_null( void );
extern int test_rfile_mass_read_all_mem_over( void );
extern int test_rfile_mass_read_all_in_one_blk( void );
extern int test_rfile_mass_read_all_in_one_blk_total( void );
extern int test_rfile_mass_read_all_in_two_blk( void );
extern int test_rfile_mass_read_all_in_three_blk( void );
extern int test_rfile_mass_write_all_buf_null( void );
extern int test_rfile_mass_write_all_buf_temp_null( void );
extern int test_rfile_mass_write_all_mem_over( void );
extern int test_rfile_mass_write_all_in_one_blk( void );
extern int test_rfile_mass_write_all_in_one_blk_total( void );
extern int test_rfile_mass_write_all_in_two_blk( void );
extern int test_rfile_mass_write_all_in_three_blk( void );
extern int test_fopenProxy( void );
extern int test_fopenProxy_wrong_pkg( void );
extern int test_fopenProxy_pathlen_wrong( void );
extern int test_fopenProxy_mode_wrong( void );
extern int test_fopenProxy_fopen_unreg( void );
extern int test_fopenProxy_fopen_reg_success( void );
extern int test_fcloseProxy_wrong_pkg( void );
extern int test_fcloseProxy_para_err( void );
extern int test_fcloseProxy_fclose_unreg( void );
extern int test_fcloseProxy_fclose_success( void );
extern int test_freadProxy_wrong_pkg( void );
extern int test_freadProxy_one_fread_unreg( void );
extern int test_freadProxy_lessthan_ICC_BUF( void );
extern int test_freadProxy_morethan_ICC_BUF( void );
extern int test_fwriteProxy_wrong_pkg( void );
extern int test_fwriteProxy_fp_null( void );
extern int test_fwriteProxy_fwrite_unreg( void );
extern int test_fwriteProxy_fwrite_frame_not_last( void );
extern int test_fwriteProxy_fwrite_frame_last( void );
extern int test_fseekProxy_wrong_pkg( void );
extern int test_fseekProxy_fp_null( void );
extern int test_fseekProxy_fseek_unreg( void );
extern int test_fseekProxy_fseek_success( void );
extern int test_fremoveProxy_wrong_pkg( void );
extern int test_fremoveProxy_pathlen_zero( void );
extern int test_fremoveProxy_remov_unreg( void );
extern int test_fremoveProxy_remov_success( void );
extern int test_fmkdirProxy_wrong_pkg( void );
extern int test_fmkdirProxy_pathlen_null( void );
extern int test_fmkdirProxy_mkdir_unreg( void );
extern int test_fmkdirProxy_mkdir_success( void );
extern int test_frmdirProxy_wrong_pkg( void );
extern int test_frmdirProxy_pathlen_null( void );
extern int test_frmdirProxy_rmdir_unreg( void );
extern int test_frmdirProxy_rmdir_success( void );
extern int test_fopendirProxy_wrong_pkg( void );
extern int test_fopendirProxy_pathlen_null( void );
extern int test_fopendirProxy_opendir_unreg( void );
extern int test_fopendirProxy_opendir_success( void );

extern int test_freaddirProxy_dir_null( void );
extern int test_freaddirProxy_readdir_unreg( void );
extern int test_freaddirProxy_readdir_success( void );
extern int test_fstatdirProxy_wrong_pkg( void );
extern int test_fstatdirProxy_pathlen_null( void );
extern int test_fstatdirProxy_statdir_unreg( void );
extern int test_fstatdirProxy_statdir_success( void );
extern int test_fxdeletedirProxy_wrong_pkg( void );
extern int test_fxdeletedirProxy_pathlen_null( void );
extern int test_fxdeletedirProxy_xdeletedir_unreg( void );
extern int test_fxdeletedirProxy_xdeletedir_success( void );
extern int test_faccessProxy_accessdir_wrong_pkg( void );
extern int test_faccessProxy_accessdir_modelen_null( void );
extern int test_faccessProxy_accessdir_pathlen_null( void );
extern int test_faccessProxy_accessdir_accessdir_unreg( void );
extern int test_faccessProxy_accessdir_success( void );
extern int test_fxcopyProxy_wrong_pkg( void );
extern int test_fxcopyProxy_dest_pathlen_null( void );
extern int test_fxcopyProxy_source_pathlen_null( void );
extern int test_fxcopyProxy_xcopydir_unreg( void );
extern int test_fxcopydirProxy_xcopydir_success( void );
extern int test_frenameProxy_wrong_pkg( void );
extern int test_frenameProxy_new_pathlen_null( void );
extern int test_frenameProxy_old_pathlen_null( void );
extern int test_frenameProxy_renamedir_unreg( void );
extern int test_frenamedirProxy_renameir_success( void );
extern int test_bsp_linux_fopen_path_null( void );
extern int test_bsp_linux_fopen_mode_null( void );
extern int test_bsp_linux_fopen( void );
extern int test_bsp_linux_fclose_stream_null( void );
extern int test_bsp_linux_fclose( void );
extern int test_bsp_linux_fread_buf_null( void );
extern int test_bsp_linux_fread( void );
extern int test_bsp_linux_fwrite_buf_null( void );
extern int test_bsp_linux_fwrite( void );
extern int test_bsp_linux_fseek_stream_null( void );
extern int test_bsp_linux_fseek( void );
extern int test_bsp_linux_remove_pathname_null( void );
extern int test_bsp_linux_remove( void );
extern int test_bsp_linux_ftell_stream_null( void );
extern int test_bsp_linux_ftell( void );
extern int test_bsp_linux_mkdir_path_null( void );
extern int test_bsp_linux_mkdir( void );
extern int test_bsp_linux_rmdir_path_null( void );
extern int test_bsp_linux_rmdir( void );
extern int test_bsp_linux_opendir_path_null( void );
extern int test_bsp_linux_opendir( void );
extern int test_bsp_linux_readdir_path_null( void );
extern int test_bsp_linux_readdir( void );
extern int test_bsp_linux_closedir_dirent_null( void );
extern int test_bsp_linux_closedir( void );
extern int test_bsp_linux_stat_dirent_null( void );
extern int test_bsp_linux_stat( void );
extern int test_bsp_linux_access_dirent_null( void );
extern int test_bsp_linux_access( void );
extern int test_bsp_linux_xdelete_path_null( void );
extern int test_bsp_linux_xdelete( void );
extern int test_bsp_linux_rename_path_null( void );
extern int test_bsp_linux_rename( void );
extern int test_bsp_mass_read_path_null( void );
extern int test_bsp_mass_read( void );
extern int test_bsp_mass_write_path_null( void );
extern int test_bsp_mass_write( void );
extern int test_bsp_linux_xcopy_path_null( void );
extern int test_bsp_linux_xcopy( void );
extern int test_rfile_calc_begin_blk( void );
extern int test_rfile_get_rw_info( void );
extern int test_rfile_read_ccpu_pkt_reserve_fail( void );
extern int test_rfile_read_ccpu_pkt( void );
extern int test_rfile_mass_read_all_sys_ioctl_fail( void );
extern int test_rfile_mass_write_all_sys_ioctl_fail( void );
extern int test_freadProxy_fp_null( void );
extern int test_fwriteProxy_fwrite_writesize_exclude( void );
extern int test_ftellProxy_wrong_pkg( void );
extern int test_ftellProxy_fp_null( void );
extern int test_ftellProxy_fun_null( void );
extern int test_ftellProxy_success( void );
extern int test_fclosedirProxy_wrong_pkg( void );
extern int test_fclosedirProxy_dir_null( void );
extern int test_fclosedirProxy_fun_unreg( void );
extern int test_fclosedirProxy_success( void );
extern int test_freaddirProxy_wrong_pkg( void );
extern int test_fcloseProxy_fclose_fpcheck_fail( void );
extern int test_freadProxy_fpcheck_fail( void );
extern int test_fseekProxy_fseek_fpcheck_fail( void );
extern int test_ftellProxy_fpcheck_fail( void );
extern int test_freaddirProxy_readdir_fpcheck_fail( void );
extern int test_fclosedirProxy_fpcheck_fail( void );

extern unsigned int BSP_IFC_Send(void * pMspBody, unsigned int u32Len);
extern signed int udi_read(signed int handle, void* pMemObj, unsigned int u32Size);
extern signed int udi_write(signed int handle, void* pMemObj, unsigned int u32Size);
extern signed int udi_open(UDI_OPEN_PARAM_S *pParam);
extern void *__kmalloc(unsigned int size, int flags);
extern void *kmem_cache_alloc_trace(unsigned int size, int flags);
extern void *kmalloc(unsigned int size, int flags);
extern int sys_open();
extern int sys_close();
extern int sys_read();
extern int sys_write();
extern int sys_fsync();
extern int sys_lseek();
extern int sys_ioctl();
extern int sys_unlink();
extern int sys_mkdir();
extern int sys_rmdir();
extern int sys_getdents();
extern int sys_statfs();
extern int sys_access();
extern int sys_rename();
extern int sys_old_readdir();
extern int sys_newfstat();
extern int sys_newlstat();
extern void kfree(const void *p);
extern int rfile_mass_write(int fd,char* write_buf,unsigned int blksize,unsigned int erase_addr);
extern int rfile_write_ccpu_pkt(int opspid,char *send_buf,unsigned int send_len);
extern RFILE* bsp_linux_fopen( const char *fpath, const char *mode );
extern int bsp_linux_fclose(RFILE* stream);
extern int bsp_linux_fread( void *ptr, unsigned int count, RFILE *stream );
extern int bsp_linux_fwrite( void *ptr, unsigned int count, RFILE *stream );
extern int bsp_linux_fseek( RFILE *stream, long offset, int whence );
extern int bsp_linux_remove( const char *pathname );
extern int bsp_linux_mkdir( const char *path_dir);
extern int bsp_linux_rmdir( const char *path_dir);
extern int bsp_linux_ftell( RFILE *stream );
extern void* bsp_linux_opendir( const char *dirname );
extern int bsp_linux_readdir( DRV_DIR_STRU *dirp,DRV_DIRENT_STRU* buf );
extern int bsp_linux_closedir( DRV_DIR_STRU *dirp );
extern int bsp_linux_stat( const char *path_dir, DRV_STAT_STRU *buf );
extern int bsp_linux_xdelete( const char *source);
extern int bsp_linux_access( const char *path_dir, int amode);
extern int bsp_linux_xcopy( const char * source, const char * dest );
extern int bsp_linux_rename( const char * oldname, const char * newname );
extern int bsp_mass_read(const char *partionname,unsigned int offset,unsigned length,char *buffer);
extern int bsp_mass_write(const char *partionname,unsigned int offset,unsigned length,char *buffer);
extern int rfile_sflags(char *	mode,int *	optr, int* appendflag);
extern int rfile_get_effectblk_addr(int fd,int usr_logic_blk,int blksize);
extern int rfile_calc_begin_blk(int offset,int blksize);
extern int rfile_get_rw_info(RFILE_MASS_RW_PARA *rw_blk_info, int offset,int blksize,int total_len,int phy_blk_beg);
extern int rfile_mass_read_all(int                  fd,
        int                  length,
        char                *buffer_temp,
        char                *buffer,
        RFILE_MASS_RW_PARA   read_blk_para,
        struct mtd_info_user mtd_info);
extern int rfile_mass_write_all(int                 fd,
        int                  length,
        char                *buffer,
        char                *buffer_temp,
        RFILE_MASS_RW_PARA   write_blk_para,
        struct mtd_info_user mtd_info);
extern int rfile_xcopy_file(char* source_file,char* dest_file);
extern int rfile_read_ccpu_data(char* buf,unsigned int len);
extern int rfile_xdelete(const char* pathname);

extern int get_hashed_file_fp(void* fp_v);
extern int fp_list_add_file(int fp_v);
extern void* fp_check_file(int fp_v);
extern void fp_list_del_file(int fp_v);
extern int get_hashed_dir_fp(void* fp_v);
extern int fp_list_add_dir(void* fp_v);
extern void* fp_check_dir(int fp_v);
extern void fp_list_del_dir(int fp_v);

int test_rfile_xdelete(void);
int test_rfile_xdelete_null(void);
int test_rfile_num2string_success(void);
int test_rfile_num2string_null(void);


int set_fs(int a)
{
	return 1;
}

#define get_fs()  0x0
#define KERNEL_DS 0x0

#define BYTES_PER_FRAME   7*1024
#define ERR_NULL               0

#define O_RDONLY	00000000
#define O_WRONLY	00000001
#define O_RDWR		00000002

#define __SRD         0x0001
#define __SWR         0x0002
#define __SRW         0x0004

#define O_CREAT     00000100
#define O_TRUNC		00001000
#define O_APPEND	00002000
#define O_CLOEXEC       02000000 /* set close_on_exec */

#ifdef __cplusplus
}
#endif
TEST(test_test_freaddirProxy_wrong_pkg, UT001_wrong_pkg)
{
    int ret      = 0;

    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/

    ret = test_freaddirProxy_wrong_pkg();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
 }



TEST(test_test_freaddirProxy_dir_null, UT002_dir_null)
{
    int ret      = 0;

    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(rfile_write_ccpu_pkt).stubs().will(returnValue(-1));

    ret = test_freaddirProxy_dir_null();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    // 还原打的函数桩
    GlobalMockObject::reset();
 }



TEST(test_test_freaddirProxy_readdir_unreg, UT003_readdir_unreg)
{
    int ret      = 0;

    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(rfile_write_ccpu_pkt).stubs().will(returnValue(-1));

    ret = test_freaddirProxy_readdir_unreg();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    // 还原打的函数桩
    GlobalMockObject::reset();
 }


TEST(test_test_freaddirProxy_readdir_fpcheck_fail, UT004_readdir_fpcheck_fail)
{
    int ret      = 0;
    char buf[100] = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(rfile_write_ccpu_pkt).stubs().will(returnValue(-1));
    MOCKER(__kmalloc).stubs().will(returnValue((void*)buf));
    MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)buf));
    MOCKER(kfree).stubs().will(returnValue(0));
    MOCKER(bsp_linux_readdir).stubs().will(returnValue(0));

    ret = test_freaddirProxy_readdir_fpcheck_fail();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    // 还原打的函数桩
    GlobalMockObject::reset();
 }


TEST(test_test_freaddirProxy_readdir_success, UT005_readdir_success)
{
    int ret      = 0;
    char buf[100] = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(rfile_write_ccpu_pkt).stubs().will(returnValue(0));
    MOCKER(__kmalloc).stubs().will(returnValue((void*)buf));
    MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)buf));
    MOCKER(kfree).stubs().will(returnValue(0));
    MOCKER(bsp_linux_readdir).stubs().will(returnValue(0));
	
    ret = test_freaddirProxy_readdir_success();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(0,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    // 还原打的函数桩
    GlobalMockObject::reset();
 }



TEST(test_test_debug_print_mnt_header, UT001_debug_print_success)
{
    int ret = 0;
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    ret = test_debug_print_mnt_header();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(0,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
 }



TEST(test_test_rfile_regProxyFunc, UT001_rfile_reg_success)
{
    int ret = 0;
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    ret = test_rfile_regProxyFunc();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(0,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
 }



TEST(test_test_rfile_write_ccpu_pkt, UT001_rfile_resp_send_success)
{
    int ret = 0;
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(udi_write).stubs().will(returnValue(10));
    ret = test_rfile_write_ccpu_pkt();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(0,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    // 还原打的函数桩
    GlobalMockObject::reset();
 }



TEST(test_test_rfile_write_ccpu_pkt, UT002_rfile_resp_send_fail)
{
    int ret = 0;
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(udi_write).stubs().will(returnValue(0));
    ret = test_rfile_write_ccpu_pkt();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    // 还原打的函数桩
    GlobalMockObject::reset();
 }



TEST(test_test_rfile_read_ccpu_data, UT001_rfile_read_ccpu_data)
{
    int ret = 0;
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
   ret = test_rfile_read_ccpu_data();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(0,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
 }



TEST(test_test_rfile_calc_read_frame_num, UT001_rfile_frame_calc_less_than_64k)
{
    int ret = 0;
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
   ret = test_rfile_calc_read_frame_num(BYTES_PER_FRAME-sizeof(RFILE_MNTN_HEADER)-sizeof(RFILE_READ_HEAD)-1);

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
 }



TEST(test_test_rfile_calc_read_frame_num, UT002_rfile_frame_calc_equal_64k)
{
    int ret = 0;
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
   ret = test_rfile_calc_read_frame_num(BYTES_PER_FRAME);

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(2,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
 }



TEST(test_test_rfile_calc_read_frame_num, UT003_rfile_frame_calc_bigger_than_64k)
{
    int ret = 0;
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
   ret = test_rfile_calc_read_frame_num(3*(BYTES_PER_FRAME-sizeof(RFILE_MNTN_HEADER)-sizeof(RFILE_READ_HEAD)));

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(3,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
 }



TEST(test_test_rfile_calc_massread_frame_num, UT001_rfile_frame_calc_less_than_64k)
{
    int ret = 0;
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
   ret = test_rfile_calc_massread_frame_num(BYTES_PER_FRAME-sizeof(RFILE_MNTN_HEADER)-sizeof(RFILE_MASSREAD_HEAD)-1);

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
 }



TEST(test_test_rfile_calc_massread_frame_num, UT002_rfile_frame_calc_equal_64k)
{
    int ret = 0;
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
   ret = test_rfile_calc_massread_frame_num(BYTES_PER_FRAME);

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(2,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
 }



TEST(test_test_rfile_calc_massread_frame_num, UT003_rfile_frame_calc_bigger_than_64k)
{
    int ret = 0;
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
   ret = test_rfile_calc_massread_frame_num(3*(BYTES_PER_FRAME-sizeof(RFILE_MNTN_HEADER)-sizeof(RFILE_MASSREAD_HEAD)));

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(3,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
 }



TEST(test_test_rfile_sflags, UT001_mode_r)
{
    int ret = 0;
    int flag = 0;
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
   ret = test_rfile_sflags("r",&flag);

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(__SRD,ret);
    EXPECT_EQ(O_RDONLY|0|O_CLOEXEC,flag);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
 }



TEST(test_test_rfile_sflags, UT002_mode_w)
{
    int ret = 0;
    int flag = 0;
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
   ret = test_rfile_sflags("w",&flag);

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(__SWR,ret);
    EXPECT_EQ(O_WRONLY|O_CREAT | O_TRUNC|O_CLOEXEC,flag);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
 }



TEST(test_test_rfile_sflags, UT003_mode_a)
{
    int ret = 0;
    int flag = 0;
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
   ret = test_rfile_sflags("a",&flag);

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(__SWR,ret);
    EXPECT_EQ(O_WRONLY|O_CREAT | O_APPEND|O_CLOEXEC,flag);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
 }



TEST(test_test_rfile_sflags, UT004_mode_rb)
{
    int ret = 0;
    int flag = 0;
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
   ret = test_rfile_sflags("rb",&flag);

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(__SRD,ret);
    EXPECT_EQ(O_RDONLY|0|O_CLOEXEC,flag);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
 }



TEST(test_test_rfile_sflags, UT005_mode_wb)
{
    int ret = 0;
    int flag = 0;
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
   ret = test_rfile_sflags("wb",&flag);

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
   EXPECT_EQ(__SWR,ret);
   EXPECT_EQ(O_WRONLY|O_CREAT | O_TRUNC|O_CLOEXEC,flag);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
 }



TEST(test_test_rfile_sflags, UT006_mode_ab)
{
    int ret = 0;
    int flag = 0;
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
   ret = test_rfile_sflags("ab",&flag);

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
   EXPECT_EQ(__SWR,ret);
   EXPECT_EQ(O_WRONLY|O_CREAT | O_APPEND|O_CLOEXEC,flag);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
 }



TEST(test_test_rfile_sflags, UT007_mode_r_plus)
{
    int ret = 0;
    int flag = 0;
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
   ret = test_rfile_sflags("r+",&flag);

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
   EXPECT_EQ(__SRW,ret);
   EXPECT_EQ(O_RDWR|0|O_CLOEXEC,flag);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
 }



TEST(test_test_rfile_sflags, UT008_mode_w_plus)
{
    int ret = 0;
    int flag = 0;
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
   ret = test_rfile_sflags("w+",&flag);

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
   EXPECT_EQ(__SRW,ret);
   EXPECT_EQ(O_RDWR|O_CREAT | O_TRUNC|O_CLOEXEC,flag);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
 }



TEST(test_test_rfile_sflags, UT009_mode_a_plus)
{
    int ret = 0;
    int flag = 0;
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
   ret = test_rfile_sflags("a+",&flag);

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
   EXPECT_EQ(__SRW,ret);
   EXPECT_EQ(O_RDWR|O_CREAT | O_APPEND|O_CLOEXEC,flag);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
 }



TEST(test_test_rfile_sflags, UT010_mode_rb_plus)
{
    int ret = 0;
    int flag = 0;
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
   ret = test_rfile_sflags("rb+",&flag);

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
   EXPECT_EQ(__SRW,ret);
   EXPECT_EQ(O_RDWR|0|O_CLOEXEC,flag);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
 }



TEST(test_test_rfile_sflags, UT011_mode_wb_plus)
{
    int ret = 0;
    int flag = 0;
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
   ret = test_rfile_sflags("wb+",&flag);

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
   EXPECT_EQ(__SRW,ret);
   EXPECT_EQ(O_RDWR|O_CREAT | O_TRUNC|O_CLOEXEC,flag);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
 }



TEST(test_test_rfile_sflags, UT012_mode_ab_plus)
{
    int ret = 0;
    int flag = 0;
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
   ret = test_rfile_sflags("ab+",&flag);

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
   EXPECT_EQ(__SRW,ret);
   EXPECT_EQ(O_RDWR|O_CREAT | O_APPEND|O_CLOEXEC,flag);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
 }



TEST(test_test_rfile_sflags, UT013_mode_fail)
{
    int ret = 0;
    int flag = 0;
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    ret = test_rfile_sflags("u+",&flag);

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(0,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
 }


TEST(test_test_rfile_xcopy_file, UT001_kmalloc_fail)
{
    int ret = 0;
    int flag = 0;
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(__kmalloc).stubs().will(returnValue((void*) 0));
    MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*) 0));

    ret = test_rfile_xcopy_file();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    // 还原打的函数桩
    GlobalMockObject::reset();
 }



TEST(test_test_rfile_xcopy_file, UT002_sys_open_fail)
{
    int ret   = 0;
    int flag  = 0;
    char buf[1000] = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(__kmalloc).stubs().will(returnValue((void*) buf));
    MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*) buf));
    MOCKER(sys_open).stubs().will(returnValue(-1));
	MOCKER(sys_newlstat).stubs().will(returnValue(1));
	MOCKER(kfree).stubs().will(returnValue(0));
	
    ret = test_rfile_xcopy_file();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    // 还原打的函数桩
    GlobalMockObject::reset();
 }


TEST(test_test_rfile_xcopy_file, UT003_sys_close_fail)
{
    int ret   = 0;
    int flag  = 0;
    char buf[1000] = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(__kmalloc).stubs().will(returnValue((void*) buf));
    MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*) buf));
    MOCKER(sys_open).stubs().will(returnValue(0));
    MOCKER(sys_close).stubs().will(returnValue(-1));
	MOCKER(kfree).stubs().will(returnValue(0));
	
    ret = test_rfile_xcopy_file();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    // 还原打的函数桩
    GlobalMockObject::reset();
 }


TEST(test_test_rfile_xcopy_file, UT004_sys_unlink_fail)
{
    int ret   = 0;
    int flag  = 0;
    char buf[1000] = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(__kmalloc).stubs().will(returnValue((void*) buf));
    MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*) buf));
    MOCKER(sys_open).stubs().will(returnValue(0));
    MOCKER(sys_close).stubs().will(returnValue(0));
    MOCKER(sys_unlink).stubs().will(returnValue(-1));
	MOCKER(kfree).stubs().will(returnValue(0));

    ret = test_rfile_xcopy_file();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    // 还原打的函数桩
    GlobalMockObject::reset();
 }
 

TEST(test_test_rfile_xcopy_file, UT005_sys_newlstat_fail)
{
    int ret   = 0;
    int flag  = 0;
    char buf[1000] = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(__kmalloc).stubs().will(returnValue((void*) buf));
    MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*) buf));
    MOCKER(sys_open).stubs().will(returnValue(-1));
    MOCKER(sys_newlstat).stubs().will(returnValue(-1));
	MOCKER(kfree).stubs().will(returnValue(0));

    ret = test_rfile_xcopy_file();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    // 还原打的函数桩
    GlobalMockObject::reset();
 }
 

TEST(test_test_rfile_xcopy_file, UT006_sys_open3_fail)
{
    int ret   = 0;
    int flag  = 0;
    char buf[1000] = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(__kmalloc).stubs().will(returnValue((void*) buf));
    MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*) buf));
	MOCKER(sys_newlstat).stubs().will(returnValue(0));
    MOCKER(sys_open).stubs().will(returnValue(-1)).then(returnValue(1)).then(returnValue(-1));
	MOCKER(kfree).stubs().will(returnValue(0));

    ret = test_rfile_xcopy_file();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    // 还原打的函数桩
    GlobalMockObject::reset();
 }
 

TEST(test_test_rfile_xcopy_file, UT007_sys_read_fail)
{
    int ret   = 0;
    int flag  = 0;
    char buf[1000] = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(__kmalloc).stubs().will(returnValue((void*) buf));
    MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*) buf));
	MOCKER(sys_open).stubs().will(returnValue(-1)).then(returnValue(1)).then(returnValue(1));
	MOCKER(sys_newlstat).stubs().will(returnValue(0));
    MOCKER(sys_read).stubs().will(returnValue(-1));
	MOCKER(kfree).stubs().will(returnValue(0));

    ret = test_rfile_xcopy_file();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    // 还原打的函数桩
    GlobalMockObject::reset();
 }



TEST(test_test_rfile_xcopy_file, UT008_sys_write_fail)
{
    int ret   = 0;
    int flag  = 0;
    char buf[1000] = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(__kmalloc).stubs().will(returnValue((void*) buf));
    MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*) buf));
	MOCKER(sys_open).stubs().will(returnValue(-1)).then(returnValue(1)).then(returnValue(1));
	MOCKER(sys_newlstat).stubs().will(returnValue(0));
    MOCKER(sys_read).stubs().will(returnValue(1024));
    MOCKER(sys_write).stubs().will(returnValue(-1));
	MOCKER(kfree).stubs().will(returnValue(0));

    ret = test_rfile_xcopy_file();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    // 还原打的函数桩
    GlobalMockObject::reset();
 }


TEST(test_test_rfile_xcopy_file, UT009_sys_fsync_fail)
{
    int ret   = 0;
    int flag  = 0;
    char buf[1000] = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(__kmalloc).stubs().will(returnValue((void*) buf));
    MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*) buf));
	MOCKER(sys_open).stubs().will(returnValue(-1)).then(returnValue(1)).then(returnValue(1));
	MOCKER(sys_newlstat).stubs().will(returnValue(0));
    MOCKER(sys_read).stubs().will(returnValue(0));
    MOCKER(sys_fsync).stubs().will(returnValue(-1));
	MOCKER(kfree).stubs().will(returnValue(0));

    ret = test_rfile_xcopy_file();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    // 还原打的函数桩
    GlobalMockObject::reset();
 }
 
char* g_buffer = "hello world";

TEST(test_test_rfile_xcopy_file, UT010_xcopy_success)
{
    int ret   = 0;
    int flag  = 0;
    char *buf = (char*)malloc(1000);
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(__kmalloc).stubs().will(returnValue((void*) buf));
    MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*) buf));
	MOCKER(sys_open).stubs().will(returnValue(-1)).then(returnValue(1)).then(returnValue(1));
	MOCKER(sys_newlstat).stubs().will(returnValue(0));
    MOCKER(sys_read).stubs().will(returnValue(1000));
    MOCKER(sys_write).stubs().will(returnValue(100));
	MOCKER(sys_fsync).stubs().will(returnValue(1));
	MOCKER(kfree).stubs().will(returnValue(0));

    ret = test_rfile_xcopy_file();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(0,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    // 还原打的函数桩
    free(buf);
	GlobalMockObject::reset();
 }



TEST(test_test_rfile_get_effectblk_addr, UT001_get_success)
{
    int ret   = 0;

    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
	MOCKER(sys_lseek).stubs().will(returnValue(0));
    MOCKER(sys_ioctl).stubs().will(returnValue(0));

    ret = test_rfile_get_effectblk_addr();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(0,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    // 还原打的函数桩
    GlobalMockObject::reset();
 }



TEST(test_test_rfile_get_effectblk_addr, UT002_get_fail)
{
    int ret   = 0;

    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
	MOCKER(sys_lseek).stubs().will(returnValue(0));
    MOCKER(sys_ioctl).stubs().will(returnValue(-1));

    ret = test_rfile_get_effectblk_addr();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    // 还原打的函数桩
    GlobalMockObject::reset();
 }



TEST(test_test_rfile_mass_write, UT001_sys_ioctl_fail)
{
    int ret   = 0;

    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(sys_ioctl).stubs().will(returnValue(-1));

    ret = test_rfile_mass_write();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    // 还原打的函数桩
    GlobalMockObject::reset();
 }


TEST(test_test_rfile_mass_write, UT002_sys_lseek_fail)
{
    int ret   = 0;

    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(sys_ioctl).stubs().will(returnValue(0));
	MOCKER(sys_lseek).stubs().will(returnValue(-1));
    ret = test_rfile_mass_write();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    // 还原打的函数桩
    GlobalMockObject::reset();
 }
 
 

TEST(test_test_rfile_mass_write, UT003_sys_write_fail)
{
    int ret   = 0;

    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(sys_ioctl).stubs().will(returnValue(0));
	MOCKER(sys_lseek).stubs().will(returnValue(0));
    MOCKER(sys_write).stubs().will(returnValue(-1));

    ret = test_rfile_mass_write();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    // 还原打的函数桩
    GlobalMockObject::reset();
 }


TEST(test_test_rfile_mass_write, UT004_sys_fsync_fail)
{
    int ret   = 0;

    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(sys_ioctl).stubs().will(returnValue(0));
	MOCKER(sys_lseek).stubs().will(returnValue(0));
    MOCKER(sys_write).stubs().will(returnValue(0));
	MOCKER(sys_fsync).stubs().will(returnValue(-1));
	
    ret = test_rfile_mass_write();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(0,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    // 还原打的函数桩
	GlobalMockObject::reset();
 }


TEST(test_test_rfile_mass_write, UT003_mass_write_success)
{
    int ret   = 0;

    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(sys_ioctl).stubs().will(returnValue(0));
	MOCKER(sys_lseek).stubs().will(returnValue(0));
    MOCKER(sys_write).stubs().will(returnValue(0));
	MOCKER(sys_fsync).stubs().will(returnValue(0));

    ret = test_rfile_mass_write();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(0,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    // 还原打的函数桩
    GlobalMockObject::reset();
 }



TEST(test_test_rfile_get_massread_op_para, UT001_buf_null)
{
    int ret   = 0;

    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    ret = test_rfile_get_massread_op_para_buf_null();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
 }


TEST(test_test_rfile_get_massread_op_para, UT002_partion_name_null)
{
    int ret   = 0;

    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    ret = test_rfile_get_massread_op_para_partion_name_null();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
 }



TEST(test_test_rfile_get_massread_op_para, UT003_offset_null)
{
    int ret   = 0;

    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    ret = test_rfile_get_massread_op_para_offset_null();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
 }



TEST(test_test_rfile_get_massread_op_para, UT004_length_null)
{
    int ret   = 0;

    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    ret = test_rfile_get_massread_op_para_length_null();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
 }



TEST(test_test_rfile_get_massread_op_para, UT005_massread_head_null)
{
    int ret   = 0;

    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    ret = test_rfile_get_massread_op_para_massread_head_null();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
 }



TEST(test_test_rfile_get_massread_op_para, UT006_success)
{
    int ret   = 0;

    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    ret = test_rfile_get_massread_op_para_success();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(0,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
 }



TEST(test_test_rfile_get_masswrite_op_para, UT001_buf_null)
{
    int ret   = 0;

    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    ret = test_rfile_get_masswrite_op_para_buf_null();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
 }


TEST(test_test_rfile_get_masswrite_op_para, UT002_partion_name_null)
{
    int ret   = 0;

    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    ret = test_rfile_get_masswrite_op_para_partion_name_null();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
 }



TEST(test_test_rfile_get_masswrite_op_para, UT003_offset_null)
{
    int ret   = 0;

    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    ret = test_rfile_get_masswrite_op_para_offset_null();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
 }



TEST(test_test_rfile_get_masswrite_op_para, UT004_length_null)
{
    int ret   = 0;

    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    ret = test_rfile_get_masswrite_op_para_length_null();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
 }



TEST(test_test_rfile_get_masswrite_op_para, UT005_masswrite_head_null)
{
    int ret   = 0;

    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    ret = test_rfile_get_masswrite_op_para_masswrite_head_null();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
 }



TEST(test_test_rfile_get_masswrite_op_para, UT006_success)
{
    int ret   = 0;

    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    ret = test_rfile_get_masswrite_op_para_success();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(0,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
 }


TEST(test_test_rfile_mass_read_all_buf_null, UT001_buf_null)
{
    int ret   = 0;

    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    ret = test_rfile_mass_read_all_buf_null();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
 }



TEST(test_test_rfile_mass_read_all_buf_temp_null, UT002_buf_temp_null)
{
    int ret   = 0;

    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    ret = test_rfile_mass_read_all_buf_temp_null();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
 }


TEST(test_test_rfile_mass_read_all_mem_over, UT003_mem_over)
{
    int ret   = 0;

    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    ret = test_rfile_mass_read_all_mem_over();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
 }



TEST(test_test_rfile_mass_read_all_in_one_blk, UT004_in_one_blk_fail)
{
    int ret   = 0;

    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(sys_read).stubs().will(returnValue(-1));
	MOCKER(sys_lseek).stubs().will(returnValue(0));
	MOCKER(sys_ioctl).stubs().will(returnValue(0));
    ret = test_rfile_mass_read_all_in_one_blk();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }


TEST(test_test_rfile_mass_read_all_in_one_blk, UT005_in_one_blk_success)
{
    int ret   = 0;

    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(sys_read).stubs().will(returnValue(0));
	MOCKER(sys_ioctl).stubs().will(returnValue(0));
    ret = test_rfile_mass_read_all_in_one_blk();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(0,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }



TEST(test_test_rfile_mass_read_all_in_one_blk, UT006_total_success)
{
    int ret   = 0;

    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(sys_read).stubs().will(returnValue(0));
	MOCKER(sys_ioctl).stubs().will(returnValue(0));
    ret = test_rfile_mass_read_all_in_one_blk_total();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(0,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }



TEST(test_test_rfile_mass_read_all_in_two_blk, UT007_two_blk_success)
{
    int ret   = 0;

    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(sys_read).stubs().will(returnValue(0));
	MOCKER(sys_ioctl).stubs().will(returnValue(0));
    ret = test_rfile_mass_read_all_in_two_blk();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(0,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }



TEST(test_test_rfile_mass_read_all_in_two_blk, UT008_two_blk_fail)
{
    int ret   = 0;

    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(sys_read).stubs().will(returnValue(-1));
	MOCKER(sys_ioctl).stubs().will(returnValue(0));
    ret = test_rfile_mass_read_all_in_two_blk();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }


TEST(test_test_rfile_mass_read_all_in_three_blk, UT009_three_blk_fail1)
{
    int ret   = 0;

    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
	MOCKER(sys_ioctl).stubs().will(returnValue(0));
    MOCKER(sys_read).stubs().will(returnValue(0)).then(returnValue(-1));
    ret = test_rfile_mass_read_all_in_three_blk();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }


TEST(test_test_rfile_mass_read_all_in_three_blk, UT009_three_blk_fail2)
{
    int ret   = 0;

    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(sys_read).stubs().will(returnValue(0)).then(returnValue(0)).then(returnValue(-1));
	MOCKER(sys_ioctl).stubs().will(returnValue(0));
    ret = test_rfile_mass_read_all_in_three_blk();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }
 

TEST(test_test_rfile_mass_read_all_in_three_blk, UT009_three_blk_success)
{
    int ret   = 0;

    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(sys_read).stubs().will(returnValue(0));
	MOCKER(sys_ioctl).stubs().will(returnValue(0));
    ret = test_rfile_mass_read_all_in_three_blk();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(0,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }



TEST(test_test_rfile_mass_read_all_sys_ioctl_fail, UT010_sys_ioctl_fail)
{
    int ret   = 0;

    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(sys_ioctl).stubs().will(returnValue(1));

    ret = test_rfile_mass_read_all_sys_ioctl_fail();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }



TEST(test_test_rfile_mass_write_all_buf_null, UT001_buf_null)
{
    int ret   = 0;

    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    ret = test_rfile_mass_write_all_buf_null();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
 }


TEST(test_test_rfile_mass_write_all_buf_temp_null, UT002_buf_temp_null)
{
    int ret   = 0;

    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    ret = test_rfile_mass_write_all_buf_temp_null();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
 }



TEST(test_test_rfile_mass_write_all_mem_over, UT003_mem_over)
{
    int ret   = 0;

    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    ret = test_rfile_mass_write_all_mem_over();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
 }



TEST(test_test_rfile_mass_write_all_in_one_blk, UT004_in_one_blk_fail_sys_read)
{
    int ret   = 0;

    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(sys_read).stubs().will(returnValue(-1));
	MOCKER(sys_ioctl).stubs().will(returnValue(0));
	MOCKER(sys_lseek).stubs().will(returnValue(0));
	MOCKER(rfile_mass_write).stubs().will(returnValue(-1));
    ret = test_rfile_mass_write_all_in_one_blk();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }


TEST(test_test_rfile_mass_write_all_in_one_blk, UT005_in_one_blk_fail_rfile_mass_write)
{
    int ret   = 0;

    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
	MOCKER(sys_ioctl).stubs().will(returnValue(0));
	MOCKER(sys_lseek).stubs().will(returnValue(0));
	MOCKER(sys_read).stubs().will(returnValue(0));
    MOCKER(rfile_mass_write).stubs().will(returnValue(-1));
    ret = test_rfile_mass_write_all_in_one_blk();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }


TEST(test_test_rfile_mass_write_all_in_one_blk, UT006_in_one_blk_fseek_fail)
{
    int ret   = 0;

    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
	MOCKER(sys_ioctl).stubs().will(returnValue(0));
	MOCKER(sys_read).stubs().will(returnValue(0));
    MOCKER(sys_lseek).stubs().will(returnValue(-1));
	MOCKER(rfile_mass_write).stubs().will(returnValue(-1));
	
    ret = test_rfile_mass_write_all_in_one_blk();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }


TEST(test_test_rfile_mass_write_all_in_one_blk, UT006_in_one_blk_success)
{
    int ret   = 0;

    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
	MOCKER(sys_ioctl).stubs().will(returnValue(0));
	MOCKER(sys_lseek).stubs().will(returnValue(0));
	MOCKER(sys_read).stubs().will(returnValue(0));
    MOCKER(rfile_mass_write).stubs().will(returnValue(0));
    ret = test_rfile_mass_write_all_in_one_blk();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(0,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }



TEST(test_test_rfile_mass_write_all_in_one_blk, UT007_total_success)
{
    int ret   = 0;

    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
	MOCKER(sys_ioctl).stubs().will(returnValue(0));
	MOCKER(sys_lseek).stubs().will(returnValue(0));
	MOCKER(sys_read).stubs().will(returnValue(0));
    ret = test_rfile_mass_write_all_in_one_blk_total();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(0,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
	GlobalMockObject::reset();
 }


TEST(test_test_rfile_mass_write_all_in_two_blk, UT008_two_blk_success)
{
    int ret   = 0;

    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
	MOCKER(sys_ioctl).stubs().will(returnValue(0));
	MOCKER(sys_lseek).stubs().will(returnValue(0));
	MOCKER(sys_read).stubs().will(returnValue(0));
	
    ret = test_rfile_mass_write_all_in_two_blk();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(0,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }



TEST(test_test_rfile_mass_write_all_in_two_blk_fail, UT009_two_blk_fail)
{
    int ret   = 0;

    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
	MOCKER(sys_ioctl).stubs().will(returnValue(0));
	MOCKER(sys_lseek).stubs().will(returnValue(0));
	MOCKER(sys_read).stubs().will(returnValue(0));
    MOCKER(rfile_mass_write).stubs().will(returnValue(-1));

    ret = test_rfile_mass_write_all_in_two_blk();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }



TEST(test_test_rfile_mass_write_all_in_two_blk, UT010_two_blk_fail)
{
    int ret   = 0;

    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
	MOCKER(sys_ioctl).stubs().will(returnValue(0));
	MOCKER(sys_lseek).stubs().will(returnValue(0));
	MOCKER(sys_read).stubs().will(returnValue(-1));
    ret = test_rfile_mass_write_all_in_two_blk();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }



TEST(test_test_rfile_mass_write_all_in_three_blk, UT011_three_blk_blk3read_fail)
{
    int ret   = 0;

    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
	MOCKER(sys_ioctl).stubs().will(returnValue(0));
	MOCKER(sys_lseek).stubs().will(returnValue(0));
    MOCKER(sys_read).stubs().will(returnValue(0)).then(returnValue(-1));
    ret = test_rfile_mass_write_all_in_three_blk();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }

 


TEST(test_test_rfile_mass_write_all_in_three_blk, UT011_three_blk_blk3masswrite_fail)
{
    int ret   = 0;

    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
	MOCKER(sys_ioctl).stubs().will(returnValue(0));
	MOCKER(sys_lseek).stubs().will(returnValue(0));
	MOCKER(sys_read).stubs().will(returnValue(0));
    MOCKER(rfile_mass_write).stubs().will(returnValue(0)).then(returnValue(0)).then(returnValue(-1));
    ret = test_rfile_mass_write_all_in_three_blk();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }
 


TEST(test_test_rfile_mass_write_all_in_three_blk, UT011_three_blk_blk2masswrite_fail)
{
    int ret   = 0;

    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
	MOCKER(sys_ioctl).stubs().will(returnValue(0));
	MOCKER(sys_lseek).stubs().will(returnValue(0));
	MOCKER(sys_read).stubs().will(returnValue(0));
    MOCKER(rfile_mass_write).stubs().will(returnValue(0)).then(returnValue(-1));
    ret = test_rfile_mass_write_all_in_three_blk();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }



TEST(test_test_rfile_mass_write_all_in_three_blk, UT011_three_blk_success)
{
    int ret   = 0;

    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
	MOCKER(sys_ioctl).stubs().will(returnValue(0));
	MOCKER(sys_lseek).stubs().will(returnValue(0));
	MOCKER(sys_read).stubs().will(returnValue(0));
    ret = test_rfile_mass_write_all_in_three_blk();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(0,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }



TEST(test_test_rfile_mass_write_all_sys_ioctl_fail, UT011_sys_ioctl_fail)
{
    int ret   = 0;

    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(sys_ioctl).stubs().will(returnValue(-1));
	MOCKER(sys_lseek).stubs().will(returnValue(0));
	MOCKER(sys_read).stubs().will(returnValue(0));
    ret = test_rfile_mass_write_all_sys_ioctl_fail();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }



TEST(test_test_fopenProxy_wrong_pkg, UT001_wrong_pkg)
{
    int ret   = 0;

    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    ret = test_fopenProxy_wrong_pkg();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
 }


TEST(test_test_fopenProxy_malloc_fail, UT002_malloc_fail)
{
    int ret   = 0;
	
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(__kmalloc).stubs().will(returnValue((void*) NULL));
    MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*) NULL));
    MOCKER(rfile_write_ccpu_pkt).stubs().will(returnValue(-1));

    ret = test_fopenProxy();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }
 

TEST(test_test_fopenProxy_malloc_fail, UT003_mode_malloc_fail)
{
    int ret   = 0;
	char buf[100] = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(__kmalloc).stubs().will(returnValue((void*) buf)).then(returnValue((void*) 0));
    MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*) buf)).then(returnValue((void*) 0));
    MOCKER(rfile_write_ccpu_pkt).stubs().will(returnValue(-1));
	MOCKER(kfree).stubs().will(returnValue(0));
    ret = test_fopenProxy();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }


TEST(test_test_fopenProxy_fopen_unreg, UT003_fopen_unreg)
{
    int ret      = 0;
	char buf[20] = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/

    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(__kmalloc).stubs().will(returnValue((void*)&buf[0]));
    MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)&buf[0]));
    MOCKER(kfree).stubs().will(returnValue(0));
    MOCKER(rfile_write_ccpu_pkt).stubs().will(returnValue(-1));
    MOCKER(fp_list_add_file).stubs().will(returnValue(0));
    ret = test_fopenProxy_fopen_unreg();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();

 }


TEST(test_test_fopenProxy_pathlen_wrong, UT004_pathlen_wrong)
{
    int ret   = 0;
    void *buf = malloc(20);
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(__kmalloc).stubs().will(returnValue((void*)buf));
    MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)buf));
    MOCKER(rfile_write_ccpu_pkt).stubs().will(returnValue(-1));
	MOCKER(kfree).stubs().will(returnValue(0));
    ret = test_fopenProxy_pathlen_wrong();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
	free(buf);
    GlobalMockObject::reset();
 }



TEST(test_test_fopenProxy_modelen_wrong, UT005_modelen_wrong)
{
    int ret   = 0;
    void *buf = malloc(20);
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(__kmalloc).stubs().will(returnValue((void*)buf));
    MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)buf));
	MOCKER(kfree).stubs().will(returnValue(0));
    MOCKER(rfile_write_ccpu_pkt).stubs().will(returnValue(-1));
    ret = test_fopenProxy_mode_wrong();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
	free(buf);
    GlobalMockObject::reset();
 }


TEST(test_test_fopenProxy_open, UT006_fopen_unreg)
{
    int ret   = 0;
    void *buf = malloc(20);
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(__kmalloc).stubs().will(returnValue((void*)buf));
    MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)buf));
    MOCKER(kfree).stubs().will(returnValue(0));
    MOCKER(rfile_write_ccpu_pkt).stubs().will(returnValue(-1));
    ret = test_fopenProxy_fopen_unreg();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
	free(buf);
    GlobalMockObject::reset();
 }


TEST(test_test_fopenProxy_open_success, UT007_fopen_success)
{
    int ret   = 0;
    void *buf = malloc(20);
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(__kmalloc).stubs().will(returnValue((void*)buf));
    MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)buf));
    MOCKER(kfree).stubs().will(returnValue(0));
    MOCKER(rfile_write_ccpu_pkt).stubs().will(returnValue(0));
    MOCKER(bsp_linux_fopen).stubs().will(returnValue((RFILE*)buf));
    MOCKER(fp_list_add_file).stubs().will(returnValue(0));

    ret = test_fopenProxy_fopen_reg_success();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(0,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
	free(buf);
    GlobalMockObject::reset();
 }



TEST(test_test_fopenProxy_open_fail, UT008_fopen_fail)
{
    int ret   = 0;
    void *buf = malloc(100);
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(__kmalloc).stubs().will(returnValue((void*)0));
    MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)0));
    MOCKER(kfree).stubs().will(returnValue(0));
    MOCKER(rfile_write_ccpu_pkt).stubs().will(returnValue(-1));
    MOCKER(bsp_linux_fopen).stubs().will(returnValue((RFILE*)buf));

    ret = test_fopenProxy_fopen_reg_success();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
	free(buf);
    GlobalMockObject::reset();
 }


TEST(test_test_fcloseProxy_wrong_pakg, UT001_wrong_pkg)
{
    int ret   = 0;
    char buf[20] = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    ret = test_fcloseProxy_wrong_pkg();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
 }


TEST(test_test_fcloseProxy_para_err, UT002_para_error)
{
    int ret   = 0;
    char buf[20] = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(rfile_write_ccpu_pkt).stubs().will(returnValue(-1));
    ret = test_fcloseProxy_para_err();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }



TEST(test_test_fcloseProxy_fclose_unreg, UT003_fclose_unreg)
{
    int ret   = 0;
    void* buf = malloc(20);
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(rfile_write_ccpu_pkt).stubs().will(returnValue(-1));
//    MOCKER(fp_check_file).stubs().will(returnValue((void*)buf));
    MOCKER(fp_list_del_file).stubs().will(returnValue(0));
	MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)buf));

    ret = test_fcloseProxy_fclose_unreg();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
	free(buf);
    GlobalMockObject::reset();
 }


TEST(test_test_fcloseProxy_fclose_fail, UT004_fclose_fpcheck_fail)
{
    int ret   = 0;
    void* buf = malloc(20);
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(rfile_write_ccpu_pkt).stubs().will(returnValue(0));
    MOCKER(bsp_linux_fclose).stubs().will(returnValue(0));
    MOCKER(fp_check_file).stubs().will(returnValue((void*)0));
    MOCKER(fp_list_del_file).stubs().will(returnValue(0));

    ret = test_fcloseProxy_fclose_fpcheck_fail();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(0,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
	free(buf);
    GlobalMockObject::reset();
 }


TEST(test_test_fcloseProxy_fclose_success, UT004_fclose_success)
{
    int ret   = 0;
    char buf[40] = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(rfile_write_ccpu_pkt).stubs().will(returnValue(0));
    MOCKER(bsp_linux_fclose).stubs().will(returnValue(0));
    MOCKER(__kmalloc).stubs().will(returnValue((void*)buf));
	MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)buf));
    MOCKER(fp_list_del_file).stubs().will(returnValue(0));

    ret = test_fcloseProxy_fclose_success();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(0,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }


TEST(test_test_freadProxy_wrong_pkg, UT001_wrong_pkg)
{
    int ret   = 0;
    char buf[20] = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/

    ret = test_freadProxy_wrong_pkg();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
 }


TEST(test_test_freadProxy_fread_unreg, UT002_fread_unreg)
{
    int ret   = 0;
    char buf[20] = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/

    ret = test_freadProxy_one_fread_unreg();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
 }


TEST(test_test_freadProxy_fpcheckfail, UT003_fpcheckfail)
{
    int ret   = 0;
    void* buf = malloc(20);
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(rfile_write_ccpu_pkt).stubs().will(returnValue(0));
    MOCKER(bsp_linux_fread).stubs().will(returnValue(BYTES_PER_FRAME-sizeof(RFILE_READ_HEAD)-sizeof(RFILE_MNTN_HEADER)));
    MOCKER(fp_check_file).stubs().will(returnValue((void*)0));
	MOCKER(__kmalloc).stubs().will(returnValue((void*)buf));
	MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)buf));

    ret = test_freadProxy_fpcheck_fail();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
	free(buf);
    GlobalMockObject::reset();
 }
 

TEST(test_test_freadProxy_fpcheckfail, UT003_fpcheckfail_write_ccpufail)
{
    int ret   = 0;
    void* buf = malloc(20);
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(rfile_write_ccpu_pkt).stubs().will(returnValue(-1));
    MOCKER(bsp_linux_fread).stubs().will(returnValue(BYTES_PER_FRAME-sizeof(RFILE_READ_HEAD)-sizeof(RFILE_MNTN_HEADER)));
    MOCKER(fp_check_file).stubs().will(returnValue((void*)0));
	MOCKER(__kmalloc).stubs().will(returnValue((void*)buf));
	MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)buf));

    ret = test_freadProxy_fpcheck_fail();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
	free(buf);
    GlobalMockObject::reset();
 }
 

TEST(test_test_freadProxy_lessthan_ICC_BUF, UT003_lessthan_ICCBUF)
{
    int ret   = 0;
    char buf[40] = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(rfile_write_ccpu_pkt).stubs().will(returnValue(0));
    MOCKER(bsp_linux_fread).stubs().will(returnValue(BYTES_PER_FRAME-sizeof(RFILE_READ_HEAD)-sizeof(RFILE_MNTN_HEADER)));
    MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)buf));
    MOCKER(__kmalloc).stubs().will(returnValue((void*)buf));
	
    ret = test_freadProxy_lessthan_ICC_BUF();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(0,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/

    GlobalMockObject::reset();
 }



TEST(test_test_freadProxy_morethan_ICC_BUF, UT004_morethan_ICCBUF)
{
    int ret   = 0;
    void* buf = malloc(40);
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(rfile_write_ccpu_pkt).stubs().will(returnValue(0));
    MOCKER(bsp_linux_fread).stubs().will(returnValue(0));
//    MOCKER(fp_check_file).stubs().will(returnValue((void*)buf));
	MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)buf));
	MOCKER(__kmalloc).stubs().will(returnValue((void*)buf));
	
    ret = test_freadProxy_morethan_ICC_BUF();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(0,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
	free(buf);
    GlobalMockObject::reset();
 }



TEST(test_test_freadProxy_fp_null, UT005_fp_null)
{
    int ret   = 0;
    char buf[20] = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(rfile_write_ccpu_pkt).stubs().will(returnValue(0));
    MOCKER(bsp_linux_fread).stubs().will(returnValue(0));

    ret = test_freadProxy_fp_null();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }



TEST(test_test_fwriteProxy_wrong_pkg, UT001_wrong_pkg)
{
    int ret   = 0;
    char buf[20] = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/

    ret = test_fwriteProxy_wrong_pkg();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
 }



TEST(test_test_fwriteProxy_fp_null, UT002_fp_null)
{
    int ret   = 0;
    char buf[20] = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/

    ret = test_fwriteProxy_fp_null();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
 }



TEST(test_test_fwriteProxy_fwrite_unreg, UT003_fwrite_unreg)
{
    int ret   = 0;
    char buf[20] = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/

    ret = test_fwriteProxy_fwrite_unreg();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
 }


TEST(test_test_fwriteProxy_fwrite, UT004_fwrite_fpcheck_null)
{
    int ret   = 0;
    char buf[20] = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
//	MOCKER(fp_check_file).stubs().will(returnValue((void*)0));
	MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)buf));
    ret = test_fwriteProxy_fwrite_checkfp_null();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
	GlobalMockObject::reset();
 }
 

TEST(test_test_fwriteProxy_fwrite, UT004_fwrite_fpcheck_null_writeccpu_fail)
{
    int ret   = 0;
    char buf[20] = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
//	MOCKER(fp_check_file).stubs().will(returnValue((void*)0));
	MOCKER(rfile_write_ccpu_pkt).stubs().will(returnValue(-1));
	MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)buf));
	
    ret = test_fwriteProxy_fwrite_checkfp_null();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
	GlobalMockObject::reset();
 }


TEST(test_test_fwriteProxy_fwrite_frame_not_last, UT004_not_last)
{
    int ret   = 0;
    void* buf = malloc(40);
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(bsp_linux_fwrite).stubs().will(returnValue(0));
//    MOCKER(fp_check_file).stubs().will(returnValue((void*)buf));
	MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)buf));
	MOCKER(__kmalloc).stubs().will(returnValue((void*)buf));

    ret = test_fwriteProxy_fwrite_frame_not_last();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(0,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
	free(buf);
    GlobalMockObject::reset();
 }



TEST(test_test_fwriteProxy_fwrite_frame_not_last_err, UT005_not_last_err)
{
    int ret   = 0;
    void* buf = malloc(40);
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(bsp_linux_fwrite).stubs().will(returnValue(-1));
    MOCKER(rfile_write_ccpu_pkt).stubs().will(returnValue(-1));
	MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)buf));
	MOCKER(__kmalloc).stubs().will(returnValue((void*)buf));

    ret = test_fwriteProxy_fwrite_frame_not_last();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
	free(buf);
    GlobalMockObject::reset();
 }




TEST(test_test_fwriteProxy_fwrite_frame_last, UT006_last)
{
    int ret   = 0;
    RFILE buf = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(bsp_linux_fwrite).stubs().will(returnValue(0));
    MOCKER(rfile_write_ccpu_pkt).stubs().will(returnValue(0));
//    MOCKER(fp_check_file).stubs().will(returnValue((void*)&buf));
	MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)&buf));
	MOCKER(__kmalloc).stubs().will(returnValue((void*)&buf));

    ret = test_fwriteProxy_fwrite_frame_last();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(0,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/

    GlobalMockObject::reset();
 }



TEST(test_test_fwriteProxy_fwrite_frame_writesize_exclude, UT007_writesize_exclude)
{
    int ret   = 0;
    void* buf = malloc(40);
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(bsp_linux_fwrite).stubs().will(returnValue(0));
//    MOCKER(fp_check_file).stubs().will(returnValue((void*)buf));
	MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)buf));
	MOCKER(__kmalloc).stubs().will(returnValue((void*)buf));

    ret = test_fwriteProxy_fwrite_writesize_exclude();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
	free(buf);
    GlobalMockObject::reset();
}



TEST(test_test_fwriteProxy_fwrite_frame_last_err, UT008_last_err)
{
    int ret   = 0;
    void* buf = malloc(20);
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(bsp_linux_fwrite).stubs().will(returnValue(-1));
    MOCKER(rfile_write_ccpu_pkt).stubs().will(returnValue(-1));
//    MOCKER(fp_check_file).stubs().will(returnValue((void*)buf));
	MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)buf));

    ret = test_fwriteProxy_fwrite_frame_last();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
	free(buf);
    GlobalMockObject::reset();
 }



TEST(test_test_fseekProxy_wrong_pkg, UT001_wrong_pkg)
{
    int ret   = 0;
    char buf[20] = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/

    ret = test_fseekProxy_wrong_pkg();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
 }



TEST(test_test_fseekProxy_fp_null, UT002_fp_null)
{
    int ret   = 0;
    char buf[20] = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(rfile_write_ccpu_pkt).stubs().will(returnValue(-1));

    ret = test_fseekProxy_fp_null();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }



TEST(test_test_fseekProxy_fseek_unreg, UT003_fseek_unreg)
{
    int ret   = 0;
    char buf[20] = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(rfile_write_ccpu_pkt).stubs().will(returnValue(-1));

    ret = test_fseekProxy_fseek_unreg();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }

 

TEST(test_test_fseekProxy_fseek_fpcheck_fail, UT004_fseek_fpcheck_fail)
{
    int ret   = 0;
    void* buf = malloc(20);
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(rfile_write_ccpu_pkt).stubs().will(returnValue(-1));
    MOCKER(bsp_linux_fseek).stubs().will(returnValue(0));
	MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)buf));
//    MOCKER(fp_check_file).stubs().will(returnValue((void*)0));


    ret = test_fseekProxy_fseek_fpcheck_fail();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
	free(buf);
    GlobalMockObject::reset();
 }


TEST(test_test_fseekProxy_fseek_success, UT004_fseek_success)
{
    int ret   = 0;
    void* buf = malloc(20);
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(rfile_write_ccpu_pkt).stubs().will(returnValue(0));
    MOCKER(bsp_linux_fseek).stubs().will(returnValue(0));
//    MOCKER(fp_check_file).stubs().will(returnValue((void*)buf));
	MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)buf));

    ret = test_fseekProxy_fseek_success();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(0,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
	free(buf);
    GlobalMockObject::reset();
 }


TEST(test_test_fremoveProxy_wrong_pkg, UT001_wrong_pkg)
{
    int ret   = 0;
    char buf[20] = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/

    ret = test_fremoveProxy_wrong_pkg();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
 }


TEST(test_test_fremoveProxy_pathlen_zero, UT002_pathlen_zero)
{
    int ret   = 0;
    char buf[20] = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
	MOCKER(rfile_write_ccpu_pkt).stubs().will(returnValue(-1));
    ret = test_fremoveProxy_pathlen_zero();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
	GlobalMockObject::reset();
 }



TEST(test_test_fremoveProxy_remove_unreg, UT003_remove_unreg)
{
    int ret   = 0;
    char buf[20] = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
	MOCKER(rfile_write_ccpu_pkt).stubs().will(returnValue(-1));
    ret = test_fremoveProxy_remov_unreg();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
	GlobalMockObject::reset();
 }



TEST(test_test_fremoveProxy_remove_success, UT004_remove_success)
{
    int ret   = 0;
    char buf[40] = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(rfile_write_ccpu_pkt).stubs().will(returnValue(0));
    MOCKER(bsp_linux_remove).stubs().will(returnValue(0));
	MOCKER(bsp_linux_access).stubs().will(returnValue(0));
	MOCKER(__kmalloc).stubs().will(returnValue((void*)buf));
    MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)buf));
	MOCKER(kfree).stubs().will(returnValue(0));

    ret = test_fremoveProxy_remov_success();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(0,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }



TEST(test_test_fremoveProxy_remove_kmalloc_fail, UT005_remove_kmalloc_fail)
{
    int ret   = 0;
    char buf[20] = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(rfile_write_ccpu_pkt).stubs().will(returnValue(-1));
    MOCKER(bsp_linux_remove).stubs().will(returnValue(0));
    MOCKER(__kmalloc).stubs().will(returnValue((void*)0));
    MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)0));

    ret = test_fremoveProxy_remov_success();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }



TEST(test_test_fmkdirProxy_wrong_pkg, UT001_wrong_pkg)
{
    int ret      = 0;
    char buf[20] = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/

    ret = test_fmkdirProxy_wrong_pkg();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
 }



TEST(test_test_fmkdirProxy_pathlen_null, UT002_pathlen_null)
{
    int ret      = 0;
    char buf[20] = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(rfile_write_ccpu_pkt).stubs().will(returnValue(-1));

    ret = test_fmkdirProxy_pathlen_null();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }



TEST(test_test_fmkdirProxy_mkdir_unreg, UT003_mkdir_unreg)
{
    int ret      = 0;
    char buf[20] = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(rfile_write_ccpu_pkt).stubs().will(returnValue(-1));

    ret = test_fmkdirProxy_mkdir_unreg();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }



TEST(test_test_fmkdirProxy_mkdir_success, UT004_mkdir_success)
{
    int ret      = 0;
    void* buf = malloc(20);
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(rfile_write_ccpu_pkt).stubs().will(returnValue(0));
    MOCKER(__kmalloc).stubs().will(returnValue((void*)buf));
    MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)buf));
    MOCKER(kfree).stubs().will(returnValue(0));
    MOCKER(bsp_linux_mkdir).stubs().will(returnValue(0));

    ret = test_fmkdirProxy_mkdir_success();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(0,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
	free(buf);
    GlobalMockObject::reset();
 }



TEST(test_test_fmkdirProxy_mkdir_kmalloc_fail, UT004_mkdir_fail)
{
    int ret      = 0;
    char buf[20] = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(rfile_write_ccpu_pkt).stubs().will(returnValue(-1));
    MOCKER(__kmalloc).stubs().will(returnValue((void*)0));
    MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)0));
    MOCKER(kfree).stubs().will(returnValue(0));
    MOCKER(bsp_linux_mkdir).stubs().will(returnValue(1));

    ret = test_fmkdirProxy_mkdir_success();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }



TEST(test_test_fmkdirProxy_mkdir_fail, UT005_mkdir_fail)
{
    int ret      = 0;
    char buf[20] = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(rfile_write_ccpu_pkt).stubs().will(returnValue(-1));
    MOCKER(__kmalloc).stubs().will(returnValue((void*)buf));
    MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)buf));
    MOCKER(kfree).stubs().will(returnValue(0));
    MOCKER(bsp_linux_mkdir).stubs().will(returnValue(0));

    ret = test_fmkdirProxy_mkdir_success();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }



TEST(test_test_frmdirProxy_wrong_pkg, UT001_wrong_pkg)
{
    int ret      = 0;

    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/

    ret = test_frmdirProxy_wrong_pkg();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
 }



TEST(test_test_fopendirProxy_wrong_pkg, UT001_wrong_pkg)
{
    int ret      = 0;

    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/

    ret = test_fopendirProxy_wrong_pkg();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
 }



TEST(test_test_fopendirProxy_pathlen_null, UT002_pathlen_null)
{
    int ret      = 0;

    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(rfile_write_ccpu_pkt).stubs().will(returnValue(-1));

    ret = test_fopendirProxy_pathlen_null();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }



TEST(test_test_fopendirProxy_opendir_unreg, UT003_opendir_unreg)
{
    int ret      = 0;

    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(rfile_write_ccpu_pkt).stubs().will(returnValue(-1));

    ret = test_fopendirProxy_opendir_unreg();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }



TEST(test_test_fopendirProxy_opendir_fp_zero, UT004_opendir_success_fp_zero)
{
    int ret      = 0;
    char buf[20] = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(rfile_write_ccpu_pkt).stubs().will(returnValue(-1));
    MOCKER(bsp_linux_opendir).stubs().will(returnValue((void*)0));

    ret = test_fopendirProxy_opendir_success();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }


TEST(test_test_fopendirProxy_opendir_success, UT004_opendir_fp_success)
{
    int ret      = 0;
    char buf[400] = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
	MOCKER(__kmalloc).stubs().will(returnValue((void*)buf));
    MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)buf));
    MOCKER(rfile_write_ccpu_pkt).stubs().will(returnValue(0));
    MOCKER(fp_list_add_dir).stubs().will(returnValue(0));
    MOCKER(bsp_linux_opendir).stubs().will(returnValue((void*)buf));
	MOCKER(kfree).stubs().will(returnValue(0));
	
    ret = test_fopendirProxy_opendir_success();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(0,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }


TEST(test_test_fopendirProxy_opendir_kmalloc_fail, UT004_opendir_fail)
{
    int ret      = 0;
    char buf[20] = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(rfile_write_ccpu_pkt).stubs().will(returnValue(-1));
    MOCKER(__kmalloc).stubs().will(returnValue((void*)buf));
    MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)buf));
    MOCKER(kfree).stubs().will(returnValue(0));
    MOCKER(bsp_linux_opendir).stubs().will(returnValue((void*)0));

    ret = test_fopendirProxy_opendir_success();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }



TEST(test_test_fstatdirProxy_wrong_pkg, UT001_wrong_pkg)
{
    int ret      = 0;
    char buf[20] = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/

    ret = test_fstatdirProxy_wrong_pkg();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
 }



TEST(test_test_fstatdirProxy_pathlen_null, UT002_pathlen_null)
{
    int ret      = 0;
    char buf[20] = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(rfile_write_ccpu_pkt).stubs().will(returnValue(-1));

    ret = test_fstatdirProxy_pathlen_null();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }



TEST(test_test_fstatdirProxy_statdir_unreg, UT003_statdir_unreg)
{
    int ret      = 0;
    char buf[20] = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(rfile_write_ccpu_pkt).stubs().will(returnValue(-1));

    ret = test_fstatdirProxy_statdir_unreg();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }



TEST(test_test_fstatdirProxy_statdir_success, UT004_statdir_success)
{
    int ret      = 0;
    char buf[20] = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(rfile_write_ccpu_pkt).stubs().will(returnValue(0));
    MOCKER(__kmalloc).stubs().will(returnValue((void*)buf));
    MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)buf));
    MOCKER(kfree).stubs().will(returnValue(0));
    MOCKER(bsp_linux_stat).stubs().will(returnValue(0));

    ret = test_fstatdirProxy_statdir_success();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(0,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }



TEST(test_test_fstatdirProxy_statdir_kmalloc_fail, UT005_statdir_fail)
{
    int ret      = 0;
    char buf[20] = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(rfile_write_ccpu_pkt).stubs().will(returnValue(-1));
    MOCKER(__kmalloc).stubs().will(returnValue((void*)0));
    MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)0));
    MOCKER(kfree).stubs().will(returnValue(0));
    MOCKER(bsp_linux_stat).stubs().will(returnValue(0));

    ret = test_fstatdirProxy_statdir_success();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }



TEST(test_test_fxdeletedirProxy_wrong_pkg, UT001_wrong_pkg)
{
    int ret      = 0;
    char buf[20] = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/

    ret = test_fxdeletedirProxy_wrong_pkg();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
 }



TEST(test_test_fxdeletedirProxy_pathlen_null, UT002_pathlen_null)
{
    int ret      = 0;
    char buf[20] = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(rfile_write_ccpu_pkt).stubs().will(returnValue(-1));

    ret = test_fxdeletedirProxy_pathlen_null();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }



TEST(test_test_fxdeletedirProxy_xdeletedir_unreg, UT003_xdeletedir_unreg)
{
    int ret      = 0;
    char buf[20] = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(rfile_write_ccpu_pkt).stubs().will(returnValue(-1));

    ret = test_fxdeletedirProxy_xdeletedir_unreg();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }



TEST(test_test_fxdeletedirProxy_xdeletedir_success, UT004_xdeletedir_success)
{
    int ret      = 0;
    void* buf = malloc(20);
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(rfile_write_ccpu_pkt).stubs().will(returnValue(0));
    MOCKER(__kmalloc).stubs().will(returnValue((void*)buf));
    MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)buf));
    MOCKER(kfree).stubs().will(returnValue(0));
    MOCKER(bsp_linux_xdelete).stubs().will(returnValue(0));

    ret = test_fxdeletedirProxy_xdeletedir_success();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(0,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
	free(buf);
    GlobalMockObject::reset();
 }



TEST(test_test_fxdeletedirProxy_xdeletedir_kmalloc, UT005_xdeletedir_fail)
{
    int ret      = 0;
    char buf[20] = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(rfile_write_ccpu_pkt).stubs().will(returnValue(-1));
    MOCKER(__kmalloc).stubs().will(returnValue((void*)0));
    MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)0));
    MOCKER(kfree).stubs().will(returnValue(0));
    MOCKER(bsp_linux_xdelete).stubs().will(returnValue(0));

    ret = test_fxdeletedirProxy_xdeletedir_success();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }



TEST(test_test_faccessdirProxy_wrong_pkg, UT001_wrong_pkg)
{
    int ret      = 0;
    char buf[20] = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/

    ret = test_faccessProxy_accessdir_wrong_pkg();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
 }



TEST(test_test_faccessdirProxy_pathlen_null, UT002_pathlen_null)
{
    int ret      = 0;
    char buf[20] = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(rfile_write_ccpu_pkt).stubs().will(returnValue(-1));

    ret = test_faccessProxy_accessdir_pathlen_null();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }



TEST(test_test_faccessdirProxy_access_unreg, UT003_access_unreg)
{
    int ret      = 0;
    char buf[20] = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(rfile_write_ccpu_pkt).stubs().will(returnValue(-1));

    ret = test_faccessProxy_accessdir_accessdir_unreg();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }



TEST(test_test_faccessdirProxy_access_success, UT004_access_success)
{
    int ret      = 0;
    void* buf = malloc(20);
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(rfile_write_ccpu_pkt).stubs().will(returnValue(0));
    MOCKER(__kmalloc).stubs().will(returnValue((void*)buf));
    MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)buf));
    MOCKER(kfree).stubs().will(returnValue(0));
    MOCKER(bsp_linux_access).stubs().will(returnValue(0));

    ret = test_faccessProxy_accessdir_success();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(0,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
	free(buf);
    GlobalMockObject::reset();
 }



TEST(test_test_faccessdirProxy_access_kmalloc_fail, UT004_access_fail)
{
    int ret      = 0;
    char buf[20] = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(rfile_write_ccpu_pkt).stubs().will(returnValue(-1));
    MOCKER(__kmalloc).stubs().will(returnValue((void*)0));
    MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)0));
    MOCKER(kfree).stubs().will(returnValue(0));
    MOCKER(bsp_linux_access).stubs().will(returnValue(0));

    ret = test_faccessProxy_accessdir_success();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }



TEST(test_test_fxcopydirProxy_wrong_pkg, UT001_wrong_pkg)
{
    int ret      = 0;
    char buf[20] = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/

    ret = test_fxcopyProxy_wrong_pkg();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
 }



TEST(test_test_fxcopydirProxy_pathlen_null, UT002_pathlen_null)
{
    int ret      = 0;
    char buf[20] = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(rfile_write_ccpu_pkt).stubs().will(returnValue(-1));

    ret = test_fxcopyProxy_source_pathlen_null();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }



TEST(test_test_fxcopydirProxy_xcopy_unreg, UT003_xcopy_unreg)
{
    int ret      = 0;
    char buf[20] = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(rfile_write_ccpu_pkt).stubs().will(returnValue(-1));

    ret = test_fxcopyProxy_xcopydir_unreg();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }



TEST(test_test_fxcopydirProxy_xcopy_success, UT004_xcopy_success)
{
    int ret      = 0;
    void* buf = malloc(20);
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(rfile_write_ccpu_pkt).stubs().will(returnValue(0));
    MOCKER(__kmalloc).stubs().will(returnValue((void*)buf));
    MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)buf));
    MOCKER(kfree).stubs().will(returnValue(0));
    MOCKER(bsp_linux_xcopy).stubs().will(returnValue(0));

    ret = test_fxcopydirProxy_xcopydir_success();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(0,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
	free(buf);
    GlobalMockObject::reset();
 }



TEST(test_test_fxcopydirProxy_xcopy_kmalloc_fail, UT005_xcopy_fail)
{
    int ret      = 0;
    char buf[20] = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(rfile_write_ccpu_pkt).stubs().will(returnValue(-1));
    MOCKER(__kmalloc).stubs().will(returnValue((void*)0));
    MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)0));
    MOCKER(kfree).stubs().will(returnValue(0));
    MOCKER(bsp_linux_xcopy).stubs().will(returnValue(0));

    ret = test_fxcopydirProxy_xcopydir_success();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }

 

TEST(test_test_fxcopydirProxy_xcopy_kmalloc2_fail, UT006_xcopy_fail)
{
    int ret      = 0;
    char buf[20] = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(rfile_write_ccpu_pkt).stubs().will(returnValue(-1));
    MOCKER(__kmalloc).stubs().will(returnValue((void*)buf)).then(returnValue((void*)0));
    MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)buf)).then(returnValue((void*)0));
    MOCKER(kfree).stubs().will(returnValue(0));
    MOCKER(bsp_linux_xcopy).stubs().will(returnValue(0));

    ret = test_fxcopydirProxy_xcopydir_success();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }


TEST(test_test_frenamedirProxy_wrong_pkg, UT001_wrong_pkg)
{
    int ret      = 0;
    char buf[20] = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/

    ret = test_frenameProxy_wrong_pkg();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
 }



TEST(test_test_frenamedirProxy_pathlen_null, UT002_pathlen_null)
{
    int ret      = 0;
    char buf[20] = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(rfile_write_ccpu_pkt).stubs().will(returnValue(-1));

    ret = test_frenameProxy_old_pathlen_null();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }



TEST(test_test_frenamedirProxy_rename_unreg, UT003_rename_unreg)
{
    int ret      = 0;
    char buf[20] = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(rfile_write_ccpu_pkt).stubs().will(returnValue(-1));

    ret = test_frenameProxy_renamedir_unreg();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }



TEST(test_test_frenamedirProxy_rename_success, UT004_rename_success)
{
    int ret      = 0;
    char buf[20] = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(rfile_write_ccpu_pkt).stubs().will(returnValue(0));
    MOCKER(__kmalloc).stubs().will(returnValue((void*)buf));
    MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)buf));
    MOCKER(kfree).stubs().will(returnValue(0));
    MOCKER(bsp_linux_rename).stubs().will(returnValue(0));

    ret = test_frenamedirProxy_renameir_success();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(0,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }



TEST(test_test_frenamedirProxy_rename_kmalloc_fail, UT005_rename_fail)
{
    int ret      = 0;
    char buf[20] = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(rfile_write_ccpu_pkt).stubs().will(returnValue(-1));
    MOCKER(__kmalloc).stubs().will(returnValue((void*)0));
    MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)0));
    MOCKER(kfree).stubs().will(returnValue(0));
    MOCKER(bsp_linux_rename).stubs().will(returnValue(0));

    ret = test_frenamedirProxy_renameir_success();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }


TEST(test_test_frenamedirProxy_rename_kmalloc2_fail, UT006_rename_fail)
{
    int ret      = 0;
    char buf[20] = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(rfile_write_ccpu_pkt).stubs().will(returnValue(-1));
    MOCKER(__kmalloc).stubs().will(returnValue((void*)buf)).then(returnValue((void*)0));
    MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)buf)).then(returnValue((void*)0));
    MOCKER(kfree).stubs().will(returnValue(0));
    MOCKER(bsp_linux_rename).stubs().will(returnValue(0));

    ret = test_frenamedirProxy_renameir_success();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }



TEST(test_test_bsp_linux_fopen_path_null, UT001_path_null)
{
    int ret      = 0;

    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/

    ret = test_bsp_linux_fopen_path_null();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(ERR_NULL,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
 }



TEST(test_test_bsp_linux_fopen_mode_null, UT002_mode_null)
{
    int ret      = 0;
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/

    ret = test_bsp_linux_fopen_mode_null();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(ERR_NULL,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
 }


TEST(test_test_bsp_linux_fopen_sflags_fail, UT003_sflags_fail)
{
    int ret      = 0;
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(rfile_sflags).stubs().will(returnValue(0));

    ret = test_bsp_linux_fopen();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(ERR_NULL,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }
 

TEST(test_test_bsp_linux_fopen_malloc_fail, UT003_malloc_fail)
{
    int ret      = 0;
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(__kmalloc).stubs().will(returnValue((void*)0));
	MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)0));
    MOCKER(rfile_sflags).stubs().will(returnValue(1));

    ret = test_bsp_linux_fopen();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(ERR_NULL,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }



TEST(test_test_bsp_linux_fopen_sys_open_fail, UT004_sys_open_fail)
{
    int ret      = 0;
	char buf[100] = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(rfile_sflags).stubs().will(returnValue(1));
    MOCKER(sys_open).stubs().will(returnValue(-1));
	MOCKER(__kmalloc).stubs().will(returnValue((void*)buf));
	MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)buf));
	MOCKER(kfree).stubs().will(returnValue(0));

    ret = test_bsp_linux_fopen();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(ERR_NULL,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }


TEST(test_test_bsp_linux_fopen_success, UT004_sys_open_success)
{
    int ret      = 0;
    char buf[100] = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(__kmalloc).stubs().will(returnValue((void*)buf));
	MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)buf));
    MOCKER(kfree).stubs().will(returnValue(0));
    MOCKER(rfile_sflags).stubs().will(returnValue(1));
    MOCKER(sys_open).stubs().will(returnValue(0));

    ret = test_bsp_linux_fopen();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(ERR_NULL,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }




/*****************************************************************************
  (1)桩函数定义
*****************************************************************************/


/*****************************************************************************
  (2)UT函数实现
*****************************************************************************/
TEST(test_test_bsp_linux_fclose_stream_null, UT001_stream_null)
{
    int ret      = 0;

    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/

    ret = test_bsp_linux_fclose_stream_null();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
 }



/*****************************************************************************
  (1)桩函数定义
*****************************************************************************/


/*****************************************************************************
  (2)UT函数实现
*****************************************************************************/
TEST(test_test_bsp_linux_fclose_sys_close_fail, UT002_sys_close_fail)
{
    int ret      = 0;
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(sys_close).stubs().will(returnValue(-1));
    MOCKER(kfree).stubs().will(returnValue(0));
	
    ret = test_bsp_linux_fclose();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
	GlobalMockObject::reset();
 }



TEST(test_test_bsp_linux_fclose_sys_close_success, UT003_sys_close_success)
{
    int ret      = 0;

    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(sys_close).stubs().will(returnValue(0));
    MOCKER(kfree).stubs().will(returnValue(0));

    ret = test_bsp_linux_fclose();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(0,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }



TEST(test_test_bsp_linux_fread_buf_null, UT001_buf_null)
{
    int ret      = 0;

    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/

    ret = test_bsp_linux_fread_buf_null();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
 }



TEST(test_test_bsp_linux_fread_sys_read_fail, UT002_read_fail)
{
    int ret      = 0;

    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(sys_read).stubs().will(returnValue(-1));

    ret = test_bsp_linux_fread();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }



TEST(test_test_bsp_linux_fread_sys_read_success, UT003_read_success)
{
    int ret      = 0;

    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(sys_read).stubs().will(returnValue(2));

    ret = test_bsp_linux_fread();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(0,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }


TEST(test_test_bsp_linux_fwrite_buf_null, UT001_buf_null)
{
    int ret      = 0;

    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/

    ret = test_bsp_linux_fwrite_buf_null();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
 }



TEST(test_test_bsp_linux_fwrite_sys_write_fail, UT002_write_fail)
{
    int ret      = 0;

    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(sys_write).stubs().will(returnValue(-1));

    ret = test_bsp_linux_fwrite();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }


TEST(test_test_bsp_linux_fwrite_sys_write_fsync_fail, UT003_write_fsync_fail)
{
    int ret      = 0;

    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(sys_write).stubs().will(returnValue(2));
	MOCKER(sys_fsync).stubs().will(returnValue(-1));
    ret = test_bsp_linux_fwrite();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(0,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }


TEST(test_test_bsp_linux_fwrite_sys_write_success, UT004_write_success)
{
    int ret      = 0;

    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(sys_write).stubs().will(returnValue(2));
	MOCKER(sys_fsync).stubs().will(returnValue(0));

    ret = test_bsp_linux_fwrite();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(0,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }


TEST(test_test_bsp_linux_fseek_stream_null, UT001_stream_null)
{
    int ret      = 0;

    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/

    ret = test_bsp_linux_fseek_stream_null();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
 }



TEST(test_test_bsp_linux_fseek_sys_lseek_fail, UT002_seek_fail)
{
    int ret      = 0;

    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(sys_lseek).stubs().will(returnValue(-1));

    ret = test_bsp_linux_fseek();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }



TEST(test_test_bsp_linux_fseek_sys_lseek_success, UT003_seek_success)
{
    int ret      = 0;

    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(sys_lseek).stubs().will(returnValue(0));

    ret = test_bsp_linux_fseek();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(0,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }


TEST(test_test_bsp_linux_remove_pathname_null, UT001_pathname_null)
{
    int ret      = 0;

    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/

    ret = test_bsp_linux_remove_pathname_null();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
 }



TEST(test_test_bsp_linux_remove_sys_remove_fail, UT002_remove_fail)
{
    int ret      = 0;

    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(sys_unlink).stubs().will(returnValue(-1));

    ret = test_bsp_linux_remove();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }



TEST(test_test_bsp_linux_remove_sys_remove_success, UT003_remove_success)
{
    int ret      = 0;

    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(sys_unlink).stubs().will(returnValue(0));

    ret = test_bsp_linux_remove();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(0,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }


TEST(test_test_bsp_linux_ftell_stream_null, UT001_stream_null)
{
    int ret      = 0;

    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/

    ret = test_bsp_linux_ftell_stream_null();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
 }



TEST(test_test_bsp_linux_ftell_sys_ftell_fail, UT002_ftell_fail)
{
    int ret      = 0;

    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(sys_lseek).stubs().will(returnValue(-1));

    ret = test_bsp_linux_ftell();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }



TEST(test_test_bsp_linux_ftell_sys_ftell_success, UT003_ftell_success)
{
    int ret      = 0;
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(sys_lseek).stubs().will(returnValue(0));

    ret = test_bsp_linux_ftell();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(0,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }



TEST(test_test_bsp_linux_mkdir_path_null, UT001_path_null)
{
    int ret      = 0;
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/

    ret = test_bsp_linux_mkdir_path_null();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
 }



TEST(test_test_bsp_linux_mkdir_sys_mkdir_fail, UT002_mkdir_fail)
{
    int ret      = 0;
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(sys_mkdir).stubs().will(returnValue(-1));

    ret = test_bsp_linux_mkdir();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }



TEST(test_test_bsp_linux_mkdir_sys_mkdir_success, UT003_mkdir_success)
{
    int ret      = 0;
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(sys_mkdir).stubs().will(returnValue(0));

    ret = test_bsp_linux_mkdir();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(0,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }




TEST(test_test_bsp_linux_rmdir_path_null, UT001_path_null)
{
    int ret      = 0;
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/

    ret = test_bsp_linux_rmdir_path_null();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
 }



TEST(test_test_bsp_linux_rmdir_sys_rmdir_fail, UT002_rmdir_fail)
{
    int ret      = 0;
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(sys_rmdir).stubs().will(returnValue(-1));

    ret = test_bsp_linux_rmdir();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }



TEST(test_test_bsp_linux_rmdir_sys_rmdir_success, UT003_rmdir_success)
{
    int ret      = 0;
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(sys_rmdir).stubs().will(returnValue(0));

    ret = test_bsp_linux_rmdir();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(0,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }



TEST(test_test_bsp_linux_opendir_path_null, UT001_path_null)
{
    int ret      = 0;
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/

    ret = test_bsp_linux_opendir_path_null();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
 }


TEST(test_test_bsp_linux_opendir_kmalloc_fail, UT002_kmalloc_fail)
{
    int ret      = 0;
    char *buf = (char*)malloc(1000);
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(sys_open).stubs().will(returnValue(-1));
    MOCKER(__kmalloc).stubs().will(returnValue((void*)0));
	MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)0));
    MOCKER(kfree).stubs().will(returnValue(1));

    ret = test_bsp_linux_opendir();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    free(buf);
    GlobalMockObject::reset();
 }



TEST(test_test_bsp_linux_opendir_sys_open_fail, UT003_open_fail)
{
    int ret      = 0;
    char *buf = (char*)malloc(1000);
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(sys_open).stubs().will(returnValue(-1));
    MOCKER(__kmalloc).stubs().will(returnValue((void*)buf));
	MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)buf));
    MOCKER(kfree).stubs().will(returnValue(1));

    ret = test_bsp_linux_opendir();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    free(buf);
    GlobalMockObject::reset();
 }



TEST(test_bsp_linux_opendir_sys_open_success, UT004_sys_open_success)
{
    int ret      = 0;
    DRV_DIR_STRU* DIR = (DRV_DIR_STRU*) malloc(sizeof(DRV_DIR_STRU));
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(sys_open).stubs().will(returnValue(1));
    MOCKER(__kmalloc).stubs().will(returnValue((void*)DIR));
	MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)DIR));

    ret = test_bsp_linux_opendir();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(0,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
	free(DIR);
    GlobalMockObject::reset();
 }



TEST(test_test_bsp_linux_readdir_path_null, UT001_path_null)
{
    int ret      = 0;
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/

    ret = test_bsp_linux_readdir_path_null();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
 }



TEST(test_test_bsp_linux_readdir_sys_readdir_fail, UT002_sys_getdents_fail)
{
    int ret      = 0;
    char *buf = (char*)malloc(1000);
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(sys_old_readdir).stubs().will(returnValue(-1));
    MOCKER(__kmalloc).stubs().will(returnValue((void*)buf));
	MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)buf));
    MOCKER(kfree).stubs().will(returnValue(1));


    ret = test_bsp_linux_readdir();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    free(buf);
    GlobalMockObject::reset();
 }


TEST(test_test_bsp_linux_readdir_kmalloc_fail, UT003_sys_getdents_kmalloc_fail)
{
    int ret      = 0;
    struct linux_dirent_rfile* dirent = (linux_dirent_rfile*)malloc(sizeof(linux_dirent_rfile));
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(sys_old_readdir).stubs().will(returnValue(1));
    MOCKER(__kmalloc).stubs().will(returnValue((void*)0));
	MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)0));
    MOCKER(kfree).stubs().will(returnValue(1));

    ret = test_bsp_linux_readdir();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    free(dirent);
    GlobalMockObject::reset();
 }


TEST(test_test_bsp_linux_readdir_sys_readolddir_fail, UT003_sys_readolddir_fail)
{
    int ret      = 0;
    struct linux_dirent_rfile* dirent = (linux_dirent_rfile*)malloc(sizeof(linux_dirent_rfile));
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(sys_old_readdir).stubs().will(returnValue(-1));
    MOCKER(__kmalloc).stubs().will(returnValue((void*)dirent));
	MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)dirent));
    MOCKER(kfree).stubs().will(returnValue(1));

    ret = test_bsp_linux_readdir();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    free(dirent);
    GlobalMockObject::reset();
 }
 

TEST(test_test_bsp_linux_readdir_sys_readdir_success, UT003_sys_getdents_success)
{
    int ret      = 0;
    struct linux_dirent_rfile* dirent = (linux_dirent_rfile*)malloc(sizeof(linux_dirent_rfile));
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(sys_old_readdir).stubs().will(returnValue(1));
    MOCKER(__kmalloc).stubs().will(returnValue((void*)dirent));
	MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)dirent));
    MOCKER(kfree).stubs().will(returnValue(1));

    ret = test_bsp_linux_readdir();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(0,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
	free(dirent);
    GlobalMockObject::reset();
 }



TEST(test_test_bsp_linux_closedir_dirent_null, UT001_dirent_null)
{
    int ret      = 0;
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/

    ret = test_bsp_linux_closedir_dirent_null();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
 }



TEST(test_test_bsp_linux_closedir_sys_closedir_fail, UT002_sys_closedir_fail)
{
    int ret      = 0;
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(sys_close).stubs().will(returnValue(-1));
    MOCKER(kfree).stubs().will(returnValue(0));

    ret = test_bsp_linux_closedir();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }


TEST(test_bsp_linux_closedir_sys_closedir_success, UT003_sys_closedir_success)
{
    int ret      = 0;
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(sys_close).stubs().will(returnValue(1));
    MOCKER(kfree).stubs().will(returnValue(1));

    ret = test_bsp_linux_closedir();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(0,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }


TEST(test_test_bsp_linux_stat_dirent_null, UT001_dirent_null)
{
    int ret      = 0;
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/

    ret = test_bsp_linux_stat_dirent_null();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
 }



TEST(test_test_bsp_linux_stat_sys_stat_fail, UT002_sys_stat_fail)
{
    int ret      = 0;

    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(sys_newlstat).stubs().will(returnValue(-1));

    ret = test_bsp_linux_stat();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
}



TEST(test_test_bsp_linux_stat_sys_stat_success, UT003_sys_stat_success)
{
    int ret      = 0;

    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(sys_newlstat).stubs().will(returnValue(1));


    ret = test_bsp_linux_stat();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(0,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();

}



TEST(test_test_bsp_linux_access_dirent_null, UT001_dirent_null)
{
    int ret      = 0;
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/

    ret = test_bsp_linux_access_dirent_null();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
 }



TEST(test_test_bsp_linux_access_sys_access_fail, UT002_sys_access_fail)
{
    int ret      = 0;

    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(sys_access).stubs().will(returnValue(-1));

    ret = test_bsp_linux_access();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
}



TEST(test_test_bsp_linux_access_sys_access_success, UT003_sys_access_success)
{
    int ret      = 0;

    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(sys_access).stubs().will(returnValue(0));


    ret = test_bsp_linux_access();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(0,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();

}



TEST(test_test_bsp_linux_xdelete_path_null, UT001_path_null)
{
    int ret      = 0;
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/

    ret = test_bsp_linux_xdelete_path_null();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
 }



TEST(test_test_bsp_linux_xdelete_sys_xdelete_fail, UT002_sys_xdelete_fail)
{
    int ret      = 0;

    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(rfile_xdelete).stubs().will(returnValue(-1));

    ret = test_bsp_linux_xdelete();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
}



TEST(test_test_bsp_linux_xdelete_sys_xdelete_success, UT003_sys_xdelete_success)
{
    int ret      = 0;

    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/

    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(rfile_xdelete).stubs().will(returnValue(1));


    ret = test_bsp_linux_xdelete();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(0,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();

}



TEST(test_test_bsp_linux_rename_path_null, UT001_path_null)
{
    int ret      = 0;
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/

    ret = test_bsp_linux_rename_path_null();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
 }



TEST(test_test_bsp_linux_rename_sys_rename_fail, UT002_sys_rename_fail)
{
    int ret      = 0;

    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(sys_rename).stubs().will(returnValue(-1));

    ret = test_bsp_linux_rename();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
}



TEST(test_test_bsp_linux_rename_sys_rename_success, UT003_sys_rename_success)
{
    int ret      = 0;

    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(sys_rename).stubs().will(returnValue(1));


    ret = test_bsp_linux_rename();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(0,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();

}


TEST(test_test_bsp_mass_read_path_null, UT001_path_null)
{
    int ret      = 0;
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/

    ret = test_bsp_mass_read_path_null();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
 }


TEST(test_test_bsp_mass_read_kmalloc_fail, UT001_path_kmalloc_fail)
{
    int ret      = 0;
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(__kmalloc).stubs().will(returnValue((void*)0));
	MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)0));

    ret = test_bsp_mass_read();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
	GlobalMockObject::reset();
 }
 

TEST(test_test_bsp_mass_read_path_err, UT001_path_kmalloc_path_err)
{
    int ret      = 0;
	char buf[300] = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(__kmalloc).stubs().will(returnValue((void*)buf));
	MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)buf));
	MOCKER(kfree).stubs().will(returnValue(0));
    ret = test_bsp_mass_read_path_err();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
	GlobalMockObject::reset();
 }
 

TEST(test_test_bsp_mass_read_sys_open_fail, UT002_sys_open_fail)
{
    int ret      = 0;
	char buf[300] = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
	MOCKER(__kmalloc).stubs().will(returnValue((void*)buf));
	MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)buf));
	MOCKER(kfree).stubs().will(returnValue(0));
    MOCKER(sys_open).stubs().will(returnValue(-1));

    ret = test_bsp_mass_read();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }



TEST(test_test_bsp_mass_read_sys_ioctl_fail, UT003_sys_lseek_fail)
{
    int ret      = 0;
	char buf[300] = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
	MOCKER(__kmalloc).stubs().will(returnValue((void*)buf));
	MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)buf));
	MOCKER(kfree).stubs().will(returnValue(0));
    MOCKER(sys_open).stubs().will(returnValue(1));
    MOCKER(sys_lseek).stubs().will(returnValue(-1));

    ret = test_bsp_mass_read();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }



TEST(test_test_bsp_mass_read_kmalloc_fail, UT004_sys_read_fail)
{
    int ret      = 0;
	char buf[300] = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
	MOCKER(__kmalloc).stubs().will(returnValue((void*)buf));
	MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)buf));
	MOCKER(kfree).stubs().will(returnValue(0));
    MOCKER(sys_open).stubs().will(returnValue(1));
    MOCKER(sys_lseek).stubs().will(returnValue(1));
    MOCKER(sys_read).stubs().will(returnValue(-1));


    ret = test_bsp_mass_read();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }



TEST(test_test_bsp_mass_read_kmalloc_fail, UT005_sys_close_fail)
{
    int ret      = 0;
	char buf[300] = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
	MOCKER(__kmalloc).stubs().will(returnValue((void*)buf));
	MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)buf));
	MOCKER(kfree).stubs().will(returnValue(0));
    MOCKER(sys_open).stubs().will(returnValue(1));
    MOCKER(sys_lseek).stubs().will(returnValue(1));
    MOCKER(sys_read).stubs().will(returnValue(1));
    MOCKER(sys_close).stubs().will(returnValue(-1));


    ret = test_bsp_mass_read();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
	GlobalMockObject::reset();
 }


TEST(test_test_bsp_mass_read_success, UT005_success)
{
    int ret      = 0;
	char buf[300] = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
	MOCKER(__kmalloc).stubs().will(returnValue((void*)buf));
	MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)buf));
	MOCKER(kfree).stubs().will(returnValue(0));
    MOCKER(sys_open).stubs().will(returnValue(1));
    MOCKER(sys_lseek).stubs().will(returnValue(1));
    MOCKER(sys_read).stubs().will(returnValue(1));
    MOCKER(sys_close).stubs().will(returnValue(1));


    ret = test_bsp_mass_read();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(0,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
	GlobalMockObject::reset();
 }
 

TEST(test_test_bsp_mass_write_path_null, UT001_path_null)
{
    int ret      = 0;
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/

    ret = test_bsp_mass_write_path_null();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
 }


TEST(test_test_bsp_mass_write_path_err, UT002_path_err)
{
    int ret      = 0;
	char buf[DRV_NAME_MAX+1] = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
	MOCKER(__kmalloc).stubs().will(returnValue((void*)buf));
	MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)buf));
	MOCKER(kfree).stubs().will(returnValue(0));
	
    ret = test_bsp_mass_write_path_err();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }
 

TEST(test_test_bsp_mass_write_sys_open_fail, UT002_sys_open_fail)
{
    int ret      = 0;
	char buf[DRV_NAME_MAX+1] = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
	MOCKER(__kmalloc).stubs().will(returnValue((void*)buf));
	MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)buf));
    MOCKER(sys_open).stubs().will(returnValue(-1));
	MOCKER(kfree).stubs().will(returnValue(0));
    ret = test_bsp_mass_write();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }



TEST(test_test_bsp_mass_write_sys_lseek_fail, UT003_sys_lseek_fail)
{
    int ret      = 0;
	char buf[300] = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
	MOCKER(__kmalloc).stubs().will(returnValue((void*)buf));
	MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)buf));
	MOCKER(kfree).stubs().will(returnValue(0));
    MOCKER(sys_open).stubs().will(returnValue(1));
    MOCKER(sys_lseek).stubs().will(returnValue(-1));

    ret = test_bsp_mass_write();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }



TEST(test_test_bsp_mass_write_sys_write_fail, UT004_sys_write_fail)
{
    int ret      = 0;
	char buf[300] = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
	MOCKER(__kmalloc).stubs().will(returnValue((void*)buf));
	MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)buf));
	MOCKER(kfree).stubs().will(returnValue(0));
    MOCKER(sys_open).stubs().will(returnValue(1));
    MOCKER(sys_lseek).stubs().will(returnValue(1));
    MOCKER(sys_write).stubs().will(returnValue(-1));

    ret = test_bsp_mass_write();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }


TEST(test_test_bsp_mass_write_sys_fsync_fail, UT005_sys_fsync_fail)
{
    int ret      = 0;
	char buf[300] = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
	MOCKER(__kmalloc).stubs().will(returnValue((void*)buf));
	MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)buf));
	MOCKER(kfree).stubs().will(returnValue(0));
    MOCKER(sys_open).stubs().will(returnValue(1));
    MOCKER(sys_lseek).stubs().will(returnValue(1));
    MOCKER(sys_write).stubs().will(returnValue(1));
    MOCKER(sys_fsync).stubs().will(returnValue(-1));
	MOCKER(sys_close).stubs().will(returnValue(-1));
    ret = test_bsp_mass_write();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }


TEST(test_test_bsp_mass_write_sys_close_fail, UT005_sys_close_fail)
{
    int ret      = 0;
	char buf[300] = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
	MOCKER(__kmalloc).stubs().will(returnValue((void*)buf));
	MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)buf));
	MOCKER(kfree).stubs().will(returnValue(0));
    MOCKER(sys_open).stubs().will(returnValue(1));
    MOCKER(sys_lseek).stubs().will(returnValue(1));
    MOCKER(sys_write).stubs().will(returnValue(1));
    MOCKER(sys_close).stubs().will(returnValue(-1));

    ret = test_bsp_mass_write();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }




TEST(test_test_bsp_mass_write_kmalloc_fail, UT006_kmalloc_fail)
{
    int ret      = 0;
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(sys_open).stubs().will(returnValue(1));
    MOCKER(__kmalloc).stubs().will(returnValue((void*)0));
	MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)0));
    MOCKER(kfree).stubs().will(returnValue(0));


    ret = test_bsp_mass_write();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }


TEST(test_test_bsp_mass_write_sys_success, UT005_sys_success)
{
    int ret      = 0;
	char buf[300] = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
	MOCKER(__kmalloc).stubs().will(returnValue((void*)buf));
	MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)buf));
	MOCKER(kfree).stubs().will(returnValue(0));
    MOCKER(sys_open).stubs().will(returnValue(1));
    MOCKER(sys_lseek).stubs().will(returnValue(1));
    MOCKER(sys_write).stubs().will(returnValue(1));
    MOCKER(sys_close).stubs().will(returnValue(0));

    ret = test_bsp_mass_write();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(0,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }


TEST(test_test_bsp_linux_xcopy_path_null, UT001_path_null)
{
    int ret      = 0;
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/

    ret = test_bsp_linux_xcopy_path_null();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
 }


TEST(test_test_bsp_linux_xcopy_kmalloc_fail, UT002_kmalloc_fail)
{
    int ret      = 0;
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(__kmalloc).stubs().will(returnValue((void*)0));
	MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)0));

    ret = test_bsp_linux_xcopy();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }
 

TEST(test_test_bsp_linux_xcopy_kmalloc_fail, UT002_kmalloc2_fail)
{
    int ret      = 0;
	char buf[300] = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(__kmalloc).stubs().will(returnValue((void*)buf)).then(returnValue((void*)0));
	MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)buf)).then(returnValue((void*)0));
	MOCKER(kfree).stubs().will(returnValue(0));
	
    ret = test_bsp_linux_xcopy();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }


TEST(test_test_bsp_linux_xcopy_kmalloc_fail, UT002_kmalloc3_fail)
{
    int ret      = 0;
	char buf[300] = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(__kmalloc).stubs().will(returnValue((void*)buf)).then(returnValue((void*)buf)).then(returnValue((void*)0));
	MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)buf)).then(returnValue((void*)buf)).then(returnValue((void*)0));
	MOCKER(kfree).stubs().will(returnValue(0));
    ret = test_bsp_linux_xcopy();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }


TEST(test_test_bsp_linux_xcopy_sys_stat_fail, UT003_sys_stat_fail)
{
    int ret      = 0;
	char buf[300] = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
	MOCKER(__kmalloc).stubs().will(returnValue((void*)buf)).then(returnValue((void*)buf)).then(returnValue((void*)buf));
	MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)buf)).then(returnValue((void*)buf)).then(returnValue((void*)buf));
	MOCKER(kfree).stubs().will(returnValue(0));
    MOCKER(sys_mkdir).stubs().will(returnValue(1));
    MOCKER(sys_newlstat).stubs().will(returnValue(-1));

    ret = test_bsp_linux_xcopy();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }


TEST(test_test_bsp_linux_xcopy_sys_open_fail, UT003_sys_open_fail)
{
    int ret      = 0;
	char buf[300] = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
	MOCKER(__kmalloc).stubs().will(returnValue((void*)buf)).then(returnValue((void*)buf)).then(returnValue((void*)buf));
	MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)buf)).then(returnValue((void*)buf)).then(returnValue((void*)buf));
	MOCKER(kfree).stubs().will(returnValue(0));
    MOCKER(sys_mkdir).stubs().will(returnValue(1));
    MOCKER(sys_open).stubs().will(returnValue(-1));
	MOCKER(sys_newlstat).stubs().will(returnValue(1));
	MOCKER(rfile_xcopy_file).stubs().will(returnValue(-1));

    ret = test_bsp_linux_xcopy();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }
 

TEST(test_test_bsp_linux_xcopy_sys_mkdir_fail, UT003_sys_mkdir_fail)
{
    int ret      = 0;
	char buf[300] = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
	MOCKER(__kmalloc).stubs().will(returnValue((void*)buf)).then(returnValue((void*)buf)).then(returnValue((void*)buf));
	MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)buf)).then(returnValue((void*)buf)).then(returnValue((void*)buf));
	MOCKER(kfree).stubs().will(returnValue(0));
    MOCKER(sys_open).stubs().will(returnValue(-1));
	MOCKER(sys_mkdir).stubs().will(returnValue(-1));
	MOCKER(sys_newlstat).stubs().will(returnValue(1));
	MOCKER(rfile_xcopy_file).stubs().will(returnValue(-1));
	
    ret = test_bsp_linux_xcopy();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }



TEST(test_test_bsp_linux_xcopy_kmalloc_fail, UT004_kmalloc_fail)
{
    int ret      = 0;
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(sys_mkdir).stubs().will(returnValue(1));
    MOCKER(sys_open).stubs().will(returnValue(1));
    MOCKER(__kmalloc).stubs().will(returnValue((void*)0));
	MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)0));
	MOCKER(kfree).stubs().will(returnValue(0));
	MOCKER(sys_newlstat).stubs().will(returnValue(1));
	MOCKER(rfile_xcopy_file).stubs().will(returnValue(-1));

    ret = test_bsp_linux_xcopy();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }


TEST(test_test_bsp_linux_xcopy_sys_kmalloc_fail, UT005_sys_kmalloc4_fail)
{
    int ret      = 0;
	char buf[20] = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(sys_mkdir).stubs().will(returnValue(1));
    MOCKER(sys_open).stubs().will(returnValue(1));
    MOCKER(__kmalloc).stubs().will(returnValue((void*)buf)).then(returnValue((void*)buf)).then(returnValue((void*)buf)).then(returnValue((void*)0));
	MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)buf)).then(returnValue((void*)buf)).then(returnValue((void*)buf)).then(returnValue((void*)0));
    MOCKER(kfree).stubs().will(returnValue(0));
    MOCKER(sys_getdents).stubs().will(returnValue(-1));
	MOCKER(sys_newlstat).stubs().will(returnValue(1));
	MOCKER(rfile_xcopy_file).stubs().will(returnValue(-1));

    ret = test_bsp_linux_xcopy();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }


TEST(test_test_bsp_linux_xcopy_sys_getdents_fail, UT005_sys_getdents_fail)
{
    int ret      = 0;
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(sys_mkdir).stubs().will(returnValue(1));
    MOCKER(sys_open).stubs().will(returnValue(1));
    MOCKER(__kmalloc).stubs().will(returnValue((void*)1));
	MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)1));
    MOCKER(kfree).stubs().will(returnValue(0));
    MOCKER(sys_getdents).stubs().will(returnValue(-1));
    MOCKER(rfile_xcopy_file).stubs().will(returnValue(-1));
	MOCKER(sys_newlstat).stubs().will(returnValue(1));

    ret = test_bsp_linux_xcopy();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }



TEST(test_test_bsp_linux_xcopy_sys_getdents_lastone, UT006_sys_getdents_lastone)
{
    int ret      = 0;
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(sys_mkdir).stubs().will(returnValue(1));
    MOCKER(sys_open).stubs().will(returnValue(1));
    MOCKER(__kmalloc).stubs().will(returnValue((void*)1));
    MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)1));
    MOCKER(kfree).stubs().will(returnValue(0));
    MOCKER(sys_getdents).stubs().will(returnValue(0));
	MOCKER(sys_newlstat).stubs().will(returnValue(1));
	MOCKER(rfile_xcopy_file).stubs().will(returnValue(0));

    ret = test_bsp_linux_xcopy();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(0,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }


TEST(test_test_bsp_linux_xcopy_sys_getdents_one_DIR, UT007_sys_open3_fail)
{
    int ret                    = 0;
    unsigned long  buf[DRV_NAME_MAX+1]     = {0x4,0x1,0x1};
	unsigned long buf1[DRV_NAME_MAX+1]     = {0x4,0x1,0x1};

    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(sys_mkdir).stubs().will(returnValue(1));
    MOCKER(sys_open).stubs().will(returnValue(1)).then(returnValue(1)).then(returnValue(-1));
    MOCKER(__kmalloc).stubs().will(returnValue((void*)buf)).then(returnValue((void*)buf)).then(returnValue((void*)buf)).then(returnValue((void*)buf1));
    MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)buf)).then(returnValue((void*)buf)).then(returnValue((void*)buf)).then(returnValue((void*)buf1));
    MOCKER(kfree).stubs().will(returnValue(0));
    MOCKER(sys_getdents).stubs().will(returnValue(1));
    MOCKER(rfile_xcopy_file).stubs().will(returnValue(0));
	MOCKER(sys_newlstat).stubs().will(returnValue(1));
	
    ret = test_bsp_linux_xcopy();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(0,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }


TEST(test_test_bsp_linux_xcopy_sys_getdents_one_DIR, UT007_sys_open3_mkdir_fail)
{
    int ret                    = 0;
    unsigned long  buf[DRV_NAME_MAX+1]     = {0x4,0x1,0x1};
	unsigned long  buf1[DRV_NAME_MAX+1]     = {0x4,0x1,0x1};


    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(sys_mkdir).stubs().will(returnValue(-1));
    MOCKER(sys_open).stubs().will(returnValue(1)).then(returnValue(1)).then(returnValue(-1));
    MOCKER(__kmalloc).stubs().will(returnValue((void*)buf)).then(returnValue((void*)buf)).then(returnValue((void*)buf)).then(returnValue((void*)buf1));
    MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)buf)).then(returnValue((void*)buf)).then(returnValue((void*)buf)).then(returnValue((void*)buf1));
    MOCKER(kfree).stubs().will(returnValue(0));
    MOCKER(sys_getdents).stubs().will(returnValue(1));
    MOCKER(rfile_xcopy_file).stubs().will(returnValue(-1));
	MOCKER(sys_newlstat).stubs().will(returnValue(1));

    ret = test_bsp_linux_xcopy();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }
 

TEST(test_test_bsp_linux_xcopy_sys_getdents_one_DIR, UT007_sys_getdents_one_DIR)
{
    int ret                    = 0;
    unsigned long  buf[DRV_NAME_MAX+1]     = {0x4,0x1,0x1};
	unsigned long  buf1[DRV_NAME_MAX+1]     = {0x4,0x1,0x1};

    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(sys_mkdir).stubs().will(returnValue(1));
    MOCKER(sys_open).stubs().will(returnValue(1));
    MOCKER(__kmalloc).stubs().will(returnValue((void*)buf)).then(returnValue((void*)buf)).then(returnValue((void*)buf)).then(returnValue((void*)buf1));
    MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)buf)).then(returnValue((void*)buf)).then(returnValue((void*)buf)).then(returnValue((void*)buf1));
    MOCKER(kfree).stubs().will(returnValue(0));
    MOCKER(sys_getdents).stubs().will(returnValue(1));
    MOCKER(sys_newlstat).stubs().will(returnValue(1));
    MOCKER(rfile_xcopy_file).stubs().will(returnValue(0));
	
    ret = test_bsp_linux_xcopy();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(0,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }



TEST(test_test_bsp_linux_xcopy_sys_getdents_one_REG, UT009_sys_getdents_one_REG)
{
    int ret                    = 0;
    unsigned long  buf[DRV_NAME_MAX+1]     = {0x8,0x1,0x1};
	unsigned long  buf1[DRV_NAME_MAX+1]     = {0x8,0x1,0x1};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(sys_mkdir).stubs().will(returnValue(1));
    MOCKER(sys_open).stubs().will(returnValue(1));
    MOCKER(__kmalloc).stubs().will(returnValue((void*)buf)).then(returnValue((void*)buf)).then(returnValue((void*)buf)).then(returnValue((void*)buf1));
    MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)buf)).then(returnValue((void*)buf)).then(returnValue((void*)buf)).then(returnValue((void*)buf1));
    MOCKER(kfree).stubs().will(returnValue(0));
    MOCKER(sys_getdents).stubs().will(returnValue(1));
    MOCKER(rfile_xcopy_file).stubs().will(returnValue(0));
	MOCKER(sys_newlstat).stubs().will(returnValue(1));


    ret = test_bsp_linux_xcopy();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(0,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }



TEST(test_test_bsp_linux_xcopy_sys_getdents_one_REG_fail, UT009_sys_getdents_one_REG_fail)
{
    int ret                    = 0;
    unsigned long  buf[DRV_NAME_MAX+1]     = {0x8,0x1,0x1};
	unsigned long  buf1[DRV_NAME_MAX+1]     = {0x8,0x1,0x1};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(sys_mkdir).stubs().will(returnValue(1));
    MOCKER(sys_open).stubs().will(returnValue(1));
    MOCKER(__kmalloc).stubs().will(returnValue((void*)buf)).then(returnValue((void*)buf)).then(returnValue((void*)buf)).then(returnValue((void*)buf1));
    MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)buf)).then(returnValue((void*)buf)).then(returnValue((void*)buf)).then(returnValue((void*)buf1));
    MOCKER(kfree).stubs().will(returnValue(0));
    MOCKER(sys_getdents).stubs().will(returnValue(1));
    MOCKER(rfile_xcopy_file).stubs().will(returnValue(-10));
	MOCKER(sys_newlstat).stubs().will(returnValue(1));

    ret = test_bsp_linux_xcopy();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }


TEST(test_test_bsp_linux_xcopy_sys_getdents_one_fail, UT008_sys_getdents_one_fail)
{
    int ret                    = 0;
    unsigned long  buf[DRV_NAME_MAX+1]     = {0x3,0x1,0x1};
	unsigned long  buf1[DRV_NAME_MAX+1]     = {0x3,0x1,0x1};
	DRV_STAT_STRU ut_stat;
	ut_stat.st_mode = 999;
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(sys_mkdir).stubs().will(returnValue(1));
    MOCKER(sys_open).stubs().will(returnValue(1));
    MOCKER(__kmalloc).stubs().will(returnValue((void*)buf)).then(returnValue((void*)buf)).then(returnValue((void*)buf)).then(returnValue((void*)buf1));
    MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)buf)).then(returnValue((void*)buf)).then(returnValue((void*)buf)).then(returnValue((void*)buf1));
    MOCKER(kfree).stubs().will(returnValue(0));
    MOCKER(sys_getdents).stubs().will(returnValue(1));
	MOCKER(rfile_xcopy_file).stubs().will(returnValue(-1));
	
	
    ret = test_bsp_linux_xcopy();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }


TEST(test_test_frmdirProxy_pathlen_null, UT002_pathlen_null)
{
    int ret      = 0;

    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(rfile_write_ccpu_pkt).stubs().will(returnValue(-1));

    ret = test_frmdirProxy_pathlen_null();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }



TEST(test_test_frmdirProxy_rmdir_unreg, UT003_rmdir_unreg)
{
    int ret      = 0;

    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(rfile_write_ccpu_pkt).stubs().will(returnValue(-1));

    ret = test_frmdirProxy_rmdir_unreg();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }



TEST(test_test_frmdirProxy_rmdir_success, UT004_rmdir_success)
{
    int ret      = 0;

    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(rfile_write_ccpu_pkt).stubs().will(returnValue(0));
    MOCKER(bsp_linux_rmdir).stubs().will(returnValue(0));

    ret = test_frmdirProxy_rmdir_success();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(0,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }



TEST(test_test_frmdirProxy_rmdir_kmalloc_fail, UT004_rmdir_fail)
{
    int ret      = 0;

    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(rfile_write_ccpu_pkt).stubs().will(returnValue(0));
    MOCKER(__kmalloc).stubs().will(returnValue((void*)0));    
    MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)0));
    MOCKER(bsp_linux_rmdir).stubs().will(returnValue(0));

    ret = test_frmdirProxy_rmdir_success();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(0,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }



TEST(test_test_rfile_calc_begin_blk_success, UT001_calc_begin_blk_success)
{
    int ret      = 0;

    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/

    ret = test_rfile_calc_begin_blk();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(0,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
 }



TEST(test_test_rfile_get_rw_info_success, UT001_get_rw_info_success)
{
    int ret      = 0;

    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/

    ret = test_rfile_get_rw_info();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(0,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
 }



TEST(test_test_rfile_read_ccpu_pkt_fail, UT001_rfile_read_ccpu_pkt_fail)
{
    int ret      = 0;

    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/

    MOCKER(rfile_read_ccpu_data).stubs().will(returnValue(1));

    ret = test_rfile_read_ccpu_pkt_reserve_fail();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }



TEST(test_test_rfile_read_ccpu_pkt, UT001_read_ccpu_pkt_success)
{
    int ret      = 0;

    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/

    MOCKER(rfile_read_ccpu_data).stubs().will(returnValue(0));

    ret = test_rfile_read_ccpu_pkt();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(0,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
 }



TEST(test_test_ftellProxy_wrong_pkg, UT001_wrong_pkg)
{
    int ret      = 0;

    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/

    ret = test_ftellProxy_wrong_pkg();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
}



TEST(test_test_ftellProxy_fp_null, UT002_fp_null)
{
    int ret      = 0;

    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/

    ret = test_ftellProxy_fp_null();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
}



TEST(test_test_ftellProxy_fun_null, UT003_fun_null)
{
    int ret      = 0;

    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/

    ret = test_ftellProxy_fun_null();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
}



TEST(test_test_ftellProxy_fpcheckfail, UT004_fpcheckfail)
{
    int ret      = 0;
	FILE     fp    = {0};
	char buf[40] = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(rfile_write_ccpu_pkt).stubs().will(returnValue(0));
    MOCKER(fp_check_file).stubs().will(returnValue((void*)0));
	MOCKER(__kmalloc).stubs().will(returnValue((void*)buf));
    MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)buf));
	
    ret = test_ftellProxy_fpcheck_fail();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(0,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
}



TEST(test_test_ftellProxy_success, UT005_success)
{
    int ret      = 0;
	RFILE     fp    = {0};
	char buf[40] = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(rfile_write_ccpu_pkt).stubs().will(returnValue(0));
	MOCKER(bsp_linux_ftell).stubs().will(returnValue(0));
	MOCKER(__kmalloc).stubs().will(returnValue((void*)buf));
    MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)buf));
	
    ret = test_ftellProxy_success();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(0,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
}



TEST(test_test_fclosedirProxy_wrong_pkg, UT001_wrong_pkg)
{
    int ret      = 0;

    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/

    ret = test_fclosedirProxy_wrong_pkg();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
}



TEST(test_test_fclosedirProxy_dir_null, UT002_dir_null)
{
    int ret      = 0;

    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(rfile_write_ccpu_pkt).stubs().will(returnValue(-1));

    ret = test_fclosedirProxy_dir_null();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
}



TEST(test_test_fclosedirProxy_fun_unreg, UT003_unreg)
{
    int ret      = 0;

    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(rfile_write_ccpu_pkt).stubs().will(returnValue(-1));

    ret = test_fclosedirProxy_fun_unreg();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
}


TEST(test_test_fclosedirProxy_fpcheck_fail, UT004_fpcheck_fail)
{
    int ret      = 0;
	RFILE fp		={0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(rfile_write_ccpu_pkt).stubs().will(returnValue(-1));
    MOCKER(kfree).stubs().will(returnValue(0));
    MOCKER(fp_check_dir).stubs().will(returnValue((void*)0));
    MOCKER(fp_list_del_dir).stubs().will(returnValue(0));
    MOCKER(bsp_linux_closedir).stubs().will(returnValue(0));
    ret = test_fclosedirProxy_fpcheck_fail();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
}


TEST(test_test_fclosedirProxy_closefail, UT004_closefail)
{
    int ret      = 0;
	RFILE fp		={0};
	char buf[40] = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(rfile_write_ccpu_pkt).stubs().will(returnValue(-1));
	MOCKER(__kmalloc).stubs().will(returnValue((void*)buf));
    MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)buf));
    MOCKER(kfree).stubs().will(returnValue(0));
    MOCKER(fp_list_del_dir).stubs().will(returnValue(0));
	MOCKER(bsp_linux_closedir).stubs().will(returnValue(-1));
	
    ret = test_fclosedirProxy_success();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
}


TEST(test_test_fclosedirProxy_success, UT004_success)
{
    int ret      = 0;
	RFILE fp		={0};
	char buf[40] = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    MOCKER(rfile_write_ccpu_pkt).stubs().will(returnValue(0));
	MOCKER(__kmalloc).stubs().will(returnValue((void*)buf));
    MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)buf));
    MOCKER(kfree).stubs().will(returnValue(0));
    MOCKER(fp_list_del_dir).stubs().will(returnValue(0));
	MOCKER(bsp_linux_closedir).stubs().will(returnValue(0));
    ret = test_fclosedirProxy_success();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(0,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
    GlobalMockObject::reset();
}

/*****************************************************************************
被测函数  :  test_rfile_num2string_null
 被测功能  : 
 用例条件  :

 修改历史      :
  1.日    期   : 2014年5月06日
    作    者   : 全佰行q00241663
    修改内容   : 新生成UT用例

*****************************************************************************/
TEST(test_test_rfile_num2string_null, UT001_null)
{
    int ret      = 0;
	RFILE fp		={0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/

    ret = test_rfile_num2string_null();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/

}

/*****************************************************************************
被测函数  :  test_rfile_num2string_success
 被测功能  : 
 用例条件  :

 修改历史      :
  1.日    期   : 2014年5月06日
    作    者   : 全佰行q00241663
    修改内容   : 新生成UT用例

*****************************************************************************/
TEST(test_test_rfile_num2string_success, UT002_success)
{
    int ret      = 0;
	RFILE fp		={0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/

    ret = test_rfile_num2string_success();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(0,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/

}

/*****************************************************************************
被测函数  :  test_rfile_xdelete_null
 被测功能  : 
 用例条件  :

 修改历史      :
  1.日    期   : 2014年5月06日
    作    者   : 全佰行q00241663
    修改内容   : 新生成UT用例

*****************************************************************************/
TEST(test_test_rfile_xdelete_null, UT001_null)
{
    int ret      = 0;
	RFILE fp		={0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/

    ret = test_rfile_xdelete_null();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/

}

/*****************************************************************************
被测函数  :  test_fp_list_add_file
 被测功能  : 
 用例条件  :

 修改历史      :
  1.日    期   : 2014年5月06日
    作    者   : 全佰行q00241663
    修改内容   : 新生成UT用例

*****************************************************************************/
TEST(test_fp_list_add_file, UT001_kmalloc_fail)
{
    int ret      = 0;
	char buf[20]  = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
	MOCKER(__kmalloc).stubs().will(returnValue((void*)0));
	MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)0));
	
    ret = test_fp_list_add_file();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
	GlobalMockObject::reset();
}

/*****************************************************************************
被测函数  :  test_fp_list_add_file
 被测功能  : 
 用例条件  :

 修改历史      :
  1.日    期   : 2014年5月06日
    作    者   : 全佰行q00241663
    修改内容   : 新生成UT用例

*****************************************************************************/
TEST(test_fp_list_add_file, UT002_addfile_same)
{
    int ret      = 0;
	char buf[100]  = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
	MOCKER(__kmalloc).stubs().will(returnValue((void*)buf));
	MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)buf));
    ret = test_fp_list_add_file_same();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(0,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
	GlobalMockObject::reset();
}

/*****************************************************************************
被测函数  :  test_fp_list_check_file
 被测功能  : 
 用例条件  :

 修改历史      :
  1.日    期   : 2014年5月06日
    作    者   : 全佰行q00241663
    修改内容   : 新生成UT用例

*****************************************************************************/
TEST(test_fp_list_check_file, UT001_checkfile_fail)
{
    int ret      = 0;
	char buf[100]  = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/

    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
	MOCKER(__kmalloc).stubs().will(returnValue((void*)buf));
	MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)buf));
    ret = test_fp_list_check_file_fail();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
	GlobalMockObject::reset();
}


/*****************************************************************************
被测函数  :  test_fp_list_check_file
 被测功能  : 
 用例条件  :

 修改历史      :
  1.日    期   : 2014年5月06日
    作    者   : 全佰行q00241663
    修改内容   : 新生成UT用例

*****************************************************************************/
TEST(test_fp_list_check_file, UT002_checkfile_success)
{
    int ret      = 0;
	char buf[100]  = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/

    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
	MOCKER(__kmalloc).stubs().will(returnValue((void*)buf));
	MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)buf));
    ret = test_fp_list_check_file_success();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(0,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
	GlobalMockObject::reset();
}

/*****************************************************************************
被测函数  :  test_fp_list_delete_file
 被测功能  : 
 用例条件  :

 修改历史      :
  1.日    期   : 2014年5月06日
    作    者   : 全佰行q00241663
    修改内容   : 新生成UT用例

*****************************************************************************/
TEST(test_fp_list_delete_file, UT001_deletefile_fail)
{
    int ret      = 0;
	char buf[100]  = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/

    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
	MOCKER(__kmalloc).stubs().will(returnValue((void*)buf));
	MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)buf));
	MOCKER(kfree).stubs().will(returnValue(0));
    ret = test_fp_list_delete_file_fail();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(0,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
	GlobalMockObject::reset();
}

/*****************************************************************************
被测函数  :  test_fp_list_delete_file
 被测功能  : 
 用例条件  :

 修改历史      :
  1.日    期   : 2014年5月06日
    作    者   : 全佰行q00241663
    修改内容   : 新生成UT用例

*****************************************************************************/
TEST(test_fp_list_delete_file, UT002_deletefile_success)
{
    int ret      = 0;
	char buf[100]  = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/

    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
	MOCKER(__kmalloc).stubs().will(returnValue((void*)buf));
	MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)buf));
	MOCKER(kfree).stubs().will(returnValue(0));
    ret = test_fp_list_delete_file_success();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(0,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
	GlobalMockObject::reset();
}

/*****************************************************************************
被测函数  :  test_fp_list_add_dir
 被测功能  : 
 用例条件  :

 修改历史      :
  1.日    期   : 2014年5月06日
    作    者   : 全佰行q00241663
    修改内容   : 新生成UT用例

*****************************************************************************/
TEST(test_fp_list_add_dir, UT001_kmalloc_fail)
{
    int ret      = 0;
	char buf[100]  = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
	MOCKER(__kmalloc).stubs().will(returnValue((void*)0));
	MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)0));
    ret = test_fp_list_add_dir();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
	GlobalMockObject::reset();
}

/*****************************************************************************
被测函数  :  test_fp_list_add_dir
 被测功能  : 
 用例条件  :

 修改历史      :
  1.日    期   : 2014年5月06日
    作    者   : 全佰行q00241663
    修改内容   : 新生成UT用例

*****************************************************************************/
TEST(test_fp_list_add_dir, UT002_adddir_same)
{
    int ret      = 0;
	char buf[100]  = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/


    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
	MOCKER(__kmalloc).stubs().will(returnValue((void*)buf));
	MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)buf));
    ret = test_fp_list_add_dir_same();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(0,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
	GlobalMockObject::reset();
}

/*****************************************************************************
被测函数  :  test_fp_list_check_file
 被测功能  : 
 用例条件  :

 修改历史      :
  1.日    期   : 2014年5月06日
    作    者   : 全佰行q00241663
    修改内容   : 新生成UT用例

*****************************************************************************/
TEST(test_fp_list_check_dir, UT001_checkdir_fail)
{
    int ret      = 0;
	char buf[100]  = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/

    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
	MOCKER(__kmalloc).stubs().will(returnValue((void*)buf));
	MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)buf));
    ret = test_fp_list_check_dir_fail();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
	GlobalMockObject::reset();
}


/*****************************************************************************
被测函数  :  test_fp_list_check_file
 被测功能  : 
 用例条件  :

 修改历史      :
  1.日    期   : 2014年5月06日
    作    者   : 全佰行q00241663
    修改内容   : 新生成UT用例

*****************************************************************************/
TEST(test_fp_list_check_dir, UT002_checkdir_success)
{
    int ret      = 0;
	char buf[100]  = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/

    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
	MOCKER(__kmalloc).stubs().will(returnValue((void*)buf));
	MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)buf));
    ret = test_fp_list_check_dir_success();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(0,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
	GlobalMockObject::reset();
}

/*****************************************************************************
被测函数  :  test_fp_list_delete_file
 被测功能  : 
 用例条件  :

 修改历史      :
  1.日    期   : 2014年5月06日
    作    者   : 全佰行q00241663
    修改内容   : 新生成UT用例

*****************************************************************************/
TEST(test_fp_list_delete_dir, UT001_deletedir_fail)
{
    int ret      = 0;
	char buf[100]  = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/

    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
	MOCKER(__kmalloc).stubs().will(returnValue((void*)buf));
	MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)buf));
	MOCKER(kfree).stubs().will(returnValue(0));
    ret = test_fp_list_delete_dir_fail();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(0,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
	GlobalMockObject::reset();
}

/*****************************************************************************
被测函数  :  test_fp_list_delete_dir
 被测功能  : 
 用例条件  :

 修改历史      :
  1.日    期   : 2014年5月06日
    作    者   : 全佰行q00241663
    修改内容   : 新生成UT用例

*****************************************************************************/
TEST(test_fp_list_delete_dir, UT002_deletedir_success)
{
    int ret      = 0;
	char buf[100]  = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/

    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
	MOCKER(__kmalloc).stubs().will(returnValue((void*)buf));
	MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)buf));
	MOCKER(kfree).stubs().will(returnValue(0));
    ret = test_fp_list_delete_dir_success();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(0,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
	GlobalMockObject::reset();
}

/*****************************************************************************
被测函数  :  test_bsp_ccpu_rest_rfile_handle
 被测功能  : 
 用例条件  :

 修改历史      :
  1.日    期   : 2014年5月06日
    作    者   : 全佰行q00241663
    修改内容   : 新生成UT用例

*****************************************************************************/
TEST(test_bsp_ccpu_rest_rfile_handle, UT001_close1_fail)
{
    int ret      = 0;
	char buf[100]  = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/

    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
	MOCKER(bsp_linux_closedir).stubs().will(returnValue(-1));
	MOCKER(bsp_linux_fclose).stubs().will(returnValue(0));
	MOCKER(kfree).stubs().will(returnValue(0));
    ret = test_bsp_ccpu_rest_rfile_handle();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(0,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
	GlobalMockObject::reset();
}

/*****************************************************************************
被测函数  :  test_bsp_ccpu_rest_rfile_handle
 被测功能  : 
 用例条件  :

 修改历史      :
  1.日    期   : 2014年5月06日
    作    者   : 全佰行q00241663
    修改内容   : 新生成UT用例

*****************************************************************************/
TEST(test_bsp_ccpu_rest_rfile_handle, UT002_fclose_fail)
{
    int ret      = 0;
	char buf[100]  = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/

    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
	MOCKER(bsp_linux_closedir).stubs().will(returnValue(0));
	MOCKER(bsp_linux_fclose).stubs().will(returnValue(-1));
	MOCKER(kfree).stubs().will(returnValue(0));
    ret = test_bsp_ccpu_rest_rfile_handle();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(0,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
	GlobalMockObject::reset();
}

/*****************************************************************************
被测函数  :  test_bsp_ccpu_rest_rfile_handle
 被测功能  : 
 用例条件  :

 修改历史      :
  1.日    期   : 2014年5月06日
    作    者   : 全佰行q00241663
    修改内容   : 新生成UT用例

*****************************************************************************/
TEST(test_bsp_ccpu_rest_rfile_handle, UT003_reset_success)
{
    int ret      = 0;
	char buf[100]  = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/

    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
	MOCKER(bsp_linux_closedir).stubs().will(returnValue(0));
	MOCKER(bsp_linux_fclose).stubs().will(returnValue(0));
	MOCKER(kfree).stubs().will(returnValue(0));
    ret = test_bsp_ccpu_rest_rfile_handle();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(0,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
	GlobalMockObject::reset();
}

/*****************************************************************************
被测函数  :  test_fmassreadProxy
 被测功能  : 
 用例条件  :

 修改历史      :
  1.日    期   : 2014年5月06日
    作    者   : 全佰行q00241663
    修改内容   : 新生成UT用例

*****************************************************************************/
TEST(test_fmassreadProxy, UT001_wrong_pkg)
{
    int ret      = 0;
	char buf[100]  = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/

    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    ret = test_fmassreadProxy_wrong_pkg();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
}

/*****************************************************************************
被测函数  :  test_fmassreadProxy
 被测功能  : 
 用例条件  :

 修改历史      :
  1.日    期   : 2014年5月06日
    作    者   : 全佰行q00241663
    修改内容   : 新生成UT用例

*****************************************************************************/
TEST(test_fmassreadProxy, UT002_kmalloc_fail)
{
    int ret      = 0;
	char buf[100]  = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/

    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
	MOCKER(__kmalloc).stubs().will(returnValue((void*)0));
	MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)0));
	MOCKER(kfree).stubs().will(returnValue(0));
	MOCKER(rfile_write_ccpu_pkt).stubs().will(returnValue(-1));
    ret = test_fmassreadProxy_one_frame_success();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
	GlobalMockObject::reset();
}

/*****************************************************************************
被测函数  :  test_fmassreadProxy
 被测功能  : 
 用例条件  :

 修改历史      :
  1.日    期   : 2014年5月06日
    作    者   : 全佰行q00241663
    修改内容   : 新生成UT用例

*****************************************************************************/
TEST(test_fmassreadProxy, UT002_kmalloc2_fail)
{
    int ret      = 0;
	char buf[100]  = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/

    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
	MOCKER(__kmalloc).stubs().will(returnValue((void*)buf)).then(returnValue((void*)0));
	MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)buf)).then(returnValue((void*)0));
	MOCKER(kfree).stubs().will(returnValue(0));
	MOCKER(rfile_write_ccpu_pkt).stubs().will(returnValue(-1));
    ret = test_fmassreadProxy_one_frame_success();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
	GlobalMockObject::reset();
}

/*****************************************************************************
被测函数  :  test_fmassreadProxy
 被测功能  : 
 用例条件  :

 修改历史      :
  1.日    期   : 2014年5月06日
    作    者   : 全佰行q00241663
    修改内容   : 新生成UT用例

*****************************************************************************/
TEST(test_fmassreadProxy, UT002_bspmassread_fail)
{
    int ret      = 0;
	char buf[100]  = {0};
	char buf2[7*1024+1]  = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/

    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
	MOCKER(__kmalloc).stubs().will(returnValue((void*)buf)).then(returnValue((void*)buf2));
	MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)buf)).then(returnValue((void*)buf2));
	MOCKER(kfree).stubs().will(returnValue(0));
	MOCKER(rfile_write_ccpu_pkt).stubs().will(returnValue(-1));
	MOCKER(bsp_mass_read).stubs().will(returnValue(-1));
    ret = test_fmassreadProxy_one_frame_success();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
	GlobalMockObject::reset();
}

/*****************************************************************************
被测函数  :  test_fmassreadProxy
 被测功能  : 
 用例条件  :

 修改历史      :
  1.日    期   : 2014年5月06日
    作    者   : 全佰行q00241663
    修改内容   : 新生成UT用例

*****************************************************************************/
TEST(test_fmassreadProxy, UT002_bspmassread_one_frame_success)
{
    int ret      = 0;
	char buf[100]  = {0};
	char buf2[7*1024+1]  = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/

    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
	MOCKER(__kmalloc).stubs().will(returnValue((void*)buf)).then(returnValue((void*)buf2));
	MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)buf)).then(returnValue((void*)buf2));
	MOCKER(kfree).stubs().will(returnValue(0));
	MOCKER(rfile_write_ccpu_pkt).stubs().will(returnValue(0));
	MOCKER(bsp_mass_read).stubs().will(returnValue(1));
    ret = test_fmassreadProxy_one_frame_success();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(0,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
	GlobalMockObject::reset();
}


/*****************************************************************************
被测函数  :  test_fmassreadProxy
 被测功能  : 
 用例条件  :

 修改历史      :
  1.日    期   : 2014年5月06日
    作    者   : 全佰行q00241663
    修改内容   : 新生成UT用例

*****************************************************************************/
TEST(test_fmassreadProxy, UT002_bspmassread_two_frame_success)
{
    int ret      = 0;
	char buf[100]  = {0};
	char buf2[7*1024+1]  = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/

    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
	MOCKER(__kmalloc).stubs().will(returnValue((void*)buf)).then(returnValue((void*)buf2));
	MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)buf)).then(returnValue((void*)buf2));
	MOCKER(kfree).stubs().will(returnValue(0));
	MOCKER(rfile_write_ccpu_pkt).stubs().will(returnValue(0));
	MOCKER(bsp_mass_read).stubs().will(returnValue(1));
    ret = test_fmassreadProxy_two_frame_success();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(0,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
	GlobalMockObject::reset();
}

/*****************************************************************************
被测函数  :  test_fmasswriteProxy
 被测功能  : 
 用例条件  :

 修改历史      :
  1.日    期   : 2014年5月06日
    作    者   : 全佰行q00241663
    修改内容   : 新生成UT用例

*****************************************************************************/
TEST(test_fmasswriteProxy, UT001_wrong_pkg)
{
    int ret      = 0;
	char buf[100]  = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/

    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
    ret = test_fmasswriteProxy_wrong_pkg();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
}

/*****************************************************************************
被测函数  :  test_fmassreadProxy
 被测功能  : 
 用例条件  :

 修改历史      :
  1.日    期   : 2014年5月06日
    作    者   : 全佰行q00241663
    修改内容   : 新生成UT用例

*****************************************************************************/
TEST(test_fmasswriteProxy, UT002_kmalloc_fail)
{
    int ret      = 0;
	char buf[100]  = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/

    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
	MOCKER(__kmalloc).stubs().will(returnValue((void*)0));
	MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)0));
	MOCKER(kfree).stubs().will(returnValue(0));
	MOCKER(rfile_write_ccpu_pkt).stubs().will(returnValue(-1));
    ret = test_fmasswriteProxy_one_frame_success();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
	GlobalMockObject::reset();
}


/*****************************************************************************
被测函数  :  test_fmassreadProxy
 被测功能  : 
 用例条件  :

 修改历史      :
  1.日    期   : 2014年5月06日
    作    者   : 全佰行q00241663
    修改内容   : 新生成UT用例

*****************************************************************************/
TEST(test_fmasswriteProxy, UT003_bspmasswrite_fail)
{
    int ret      = 0;
	char buf[100]  = {0};
	char buf2[7*1024+1]  = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/

    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
	MOCKER(__kmalloc).stubs().will(returnValue((void*)buf));
	MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)buf));
	MOCKER(kfree).stubs().will(returnValue(0));
	MOCKER(rfile_write_ccpu_pkt).stubs().will(returnValue(-1));
	MOCKER(bsp_mass_write).stubs().will(returnValue(-1));
    ret = test_fmasswriteProxy_one_frame_success();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
	GlobalMockObject::reset();
}

/*****************************************************************************
被测函数  :  test_fmassreadProxy
 被测功能  : 
 用例条件  :

 修改历史      :
  1.日    期   : 2014年5月06日
    作    者   : 全佰行q00241663
    修改内容   : 新生成UT用例

*****************************************************************************/
TEST(test_fmasswriteProxy, UT003_bspmasswrite_fail2)
{
    int ret      = 0;
	char buf[100]  = {0};
	char buf2[7*1024+1]  = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/

    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
	MOCKER(__kmalloc).stubs().will(returnValue((void*)buf));
	MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)buf));
	MOCKER(kfree).stubs().will(returnValue(0));
	MOCKER(rfile_write_ccpu_pkt).stubs().will(returnValue(-1));
	MOCKER(bsp_mass_write).stubs().will(returnValue(-2));
    ret = test_fmasswriteProxy_one_frame_success();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
	GlobalMockObject::reset();
}

/*****************************************************************************
被测函数  :  test_fmasswriteProxy
 被测功能  : 
 用例条件  :

 修改历史      :
  1.日    期   : 2014年5月06日
    作    者   : 全佰行q00241663
    修改内容   : 新生成UT用例

*****************************************************************************/
TEST(test_fmasswriteProxy, UT002_bspmasswrite_one_frame_success)
{
    int ret      = 0;
	char buf[100]  = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/

    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
	MOCKER(__kmalloc).stubs().will(returnValue((void*)buf));
	MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)buf));
	MOCKER(kfree).stubs().will(returnValue(0));
	MOCKER(rfile_write_ccpu_pkt).stubs().will(returnValue(0));
	MOCKER(bsp_mass_write).stubs().will(returnValue(1));
    ret = test_fmasswriteProxy_one_frame_success();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(0,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
	GlobalMockObject::reset();
}

/*****************************************************************************
被测函数  :  test_fmasswriteProxy
 被测功能  : 
 用例条件  :

 修改历史      :
  1.日    期   : 2014年5月06日
    作    者   : 全佰行q00241663
    修改内容   : 新生成UT用例

*****************************************************************************/
TEST(test_fmasswriteProxy, UT002_bspmasswrite_two_frame_success)
{
    int ret      = 0;
	char buf[100]  = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/

    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
	MOCKER(__kmalloc).stubs().will(returnValue((void*)buf));
	MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)buf));
	MOCKER(kfree).stubs().will(returnValue(0));
	MOCKER(rfile_write_ccpu_pkt).stubs().will(returnValue(0));
	MOCKER(bsp_mass_write).stubs().will(returnValue(1));
    ret = test_fmasswriteProxy_two_frame_success();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(0,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
	GlobalMockObject::reset();
}

/*****************************************************************************
被测函数  :  test_fmasswriteProxy
 被测功能  : 
 用例条件  :

 修改历史      :
  1.日    期   : 2014年5月06日
    作    者   : 全佰行q00241663
    修改内容   : 新生成UT用例

*****************************************************************************/
TEST(test_fmasswriteProxy, UT002_bspmasswrite_two_frame_fail)
{
    int ret      = 0;
	char buf[100]  = {0};
    /*************************************************************************
      (1)初始化测试环境
    *************************************************************************/

    /*************************************************************************
      (2)设置被测函数形参
    *************************************************************************/


    /*************************************************************************
      (3)执行被测函数
    *************************************************************************/
	MOCKER(__kmalloc).stubs().will(returnValue((void*)buf));
	MOCKER(kmem_cache_alloc_trace).stubs().will(returnValue((void*)buf));
	MOCKER(kfree).stubs().will(returnValue(0));
	MOCKER(rfile_write_ccpu_pkt).stubs().will(returnValue(-1));
	MOCKER(bsp_mass_write).stubs().will(returnValue(-1));
    ret = test_fmasswriteProxy_two_frame_success();

    /*************************************************************************
      (4)检测测试结果
    *************************************************************************/
    EXPECT_EQ(-1,ret);

    /*************************************************************************
      (5)清理测试环境
    *************************************************************************/
	GlobalMockObject::reset();
}
