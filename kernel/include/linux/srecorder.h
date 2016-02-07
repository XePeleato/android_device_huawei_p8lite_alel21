/**
    @copyright: Huawei Technologies Co., Ltd. 2012-2013. All rights reserved.
    
    @file: srecorder.h
    
    @brief: define the global resources for SRecorder
    
    @version: 1.0 
    
    @author: QiDechun ID: 216641
    
    @date: 2013-01-29
    
    @history:
*/

#ifndef SRECORDER_H
#define SRECORDER_H


/*----includes-----------------------------------------------------------------------*/

#include <linux/version.h>
#include <srecorder/srecorder_basetypes.h>


/*----c++ support--------------------------------------------------------------------*/


/*----export macroes-----------------------------------------------------------------*/


/*----export prototypes---------------------------------------------------------------*/

#ifdef CONFIG_DUMP_LOGCAT
enum
{
    LOGCAT_MAIN = 0,
    LOGCAT_SYSTEM,
    LOGCAT_EVENTS,
    LOGCAT_RADIO,
    LOGCAT_TYPE_MAX
};

typedef struct __logcat_buf_info
{
    char *desc;
    unsigned long log_buf;
    unsigned long log_buf_len;
} logcat_buf_info;
#endif

typedef struct
{
    unsigned long srecorder_log_buf;
    unsigned long srecorder_log_buf_len;
#ifdef CONFIG_DUMP_LOGCAT
    logcat_buf_info logcat_buf_info[LOGCAT_TYPE_MAX];
#endif
    
    /* for redundancy */
    unsigned long srecorder_log_buf_bak;
    unsigned long srecorder_log_buf_len_bak;
    
    unsigned long crc32;
} platform_special_reserved_mem_info_t;

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 5, 0))
typedef struct kernel_log
{
    u64 ts_nsec;		/* timestamp in nanoseconds */
    u16 len;		/* length of entire record */
    u16 text_len;		/* length of text buffer */
    u16 dict_len;		/* length of dictionary buffer */
    u8 facility;		/* syslog facility */
    u8 flags:5;		/* internal record flags */
    u8 level:3;		/* syslog level */
} kernel_log_t, *pkernel_log_t;

typedef struct kernel_log_buf_info
{
    unsigned long log_buf;
    unsigned long log_first_seq;
    unsigned long log_first_idx;
    unsigned long log_next_seq;
    unsigned long log_next_idx;
    unsigned long log_buf_len;
} kernel_log_buf_info_t, *pkernel_log_buf_info_t;

typedef struct kernel_log_buf_content_header_info
{
    unsigned long header_len; /* the header may be added members from the end by vendors */
    unsigned long align_base; /* the header aligned base */
} kernel_log_buf_content_header_info_t, *pkernel_log_buf_content_header_info_t;

typedef struct srecorder_write_kernel_log_param
{
    char *psrecorder_buf;
    char *kernel_log_buf;
    unsigned long kernel_log_buf_len;
    unsigned long kernel_log_idx;
    unsigned long bytes_left_in_srecorder;
    unsigned long header_len; /* the header may be added members from the end by vendors */
    unsigned long align_base; /* the header aligned base */
} srecorder_write_kernel_log_param_t, *psrecorder_write_kernel_log_param_t;

enum srecorder_log_flags 
{
    SRECORDER_LOG_NOCONS	= 1,	/* already flushed, do not print to console */
    SRECORDER_LOG_NEWLINE	= 2,	/* text ended with a newline */
    SRECORDER_LOG_PREFIX	= 4,	/* text started with a prefix */
    SRECORDER_LOG_CONT	= 8,	/* text is a fragment of a continuation line */
};
#endif


/*----global variables-----------------------------------------------------------------*/


/*----export function prototypes--------------------------------------------------------*/

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 5, 0))
/**
    @function: void get_log_buf_header_info(kernel_log_buf_content_header_info_t *plog_buf_content_header_info)
    @brief: get kernel log buf header information
    
    @param: plog_buf_header_info
    
    @return: none

    @note: 
*/
void get_log_buf_header_info(kernel_log_buf_content_header_info_t *plog_buf_content_header_info);
#endif


#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 5, 0))
/**
    @function: void get_log_buf_info(kernel_log_buf_info_t *pkernel_log_buf_info)
    @brief: get kernel log ring buffer's start address,write index and length.
    
    @param: pkernel_log_buf_info
    
    @return: none

    @note: 
*/
void get_log_buf_info(kernel_log_buf_info_t *pkernel_log_buf_info);
#else
/**
    @function: void get_log_buf_info(unsigned long *plog_buf, unsigned long*plog_end, unsigned long*plog_buf_len)
    @brief: get kernel log ring buffer's start address,write index and length.
    
    @param: plog_buf save ring buffer's start address
    @param: plog_end save ring buffer's write index
    @param: plog_buf_len save ring buffer's length
    
    @return: none

    @note: 
*/
void get_log_buf_info(unsigned long *plog_buf, 
    unsigned long *plog_end, 
    unsigned long *plog_buf_len);
#endif


/**
    @function: void get_srecorder_log_buf_info(unsigned long *psrecorder_log_buf, unsigned long*psrecorder_log_buf_len)
    @brief: get the start address and length of SRecorder's reserved memory.

    @param: psrecorder_log_buf Save start address of SRecorder's reserved memory
    @param: psrecorder_log_buf_len Save length of SRecorder's reserved memory
    
    @return: none

    @note: 
*/
void get_srecorder_log_buf_info(
    unsigned long *psrecorder_log_buf, 
    unsigned long *psrecorder_log_buf_len);


/**
    @function: char* get_srecorder_temp_buf_addr(void)
    @brief: get address of the temp buffer saving the log captured before reboot.

    @param: none
    
    @return: none

    @note: 
*/
char* get_srecorder_temp_buf_addr(void);


/**
    @function: unsigned long srecorder_get_crc32(unsigned char const *pbuf, unsigned long data_len)
    @brief: do crc32 check

    @param: pbuf data tobe checked
    @param: data_len data length
    
    @return: crc32 value

    @note: 
*/
unsigned long srecorder_get_crc32(unsigned char const *pbuf, unsigned long data_len);


#ifdef CONFIG_SRECORDER_DUMP_LOG_TO_STORAGECARD
/**
    @function: char *alloc_buf_for_srecorder(unsigned long buf_len)
    @brief: allocate memory for SRecorder by vmalloc

    @param: size memory size of the buffer to be allocated
    
    @return: the start address of the buffer allocated

    @note: 
*/
char *alloc_buf_for_srecorder(unsigned long size);


/**
    @function: bool get_srecorder_log_buf(char *panic_reason, char **pbuf, unsigned long *plog_len)
    @brief: get SRecorder's valid log start addres and its length

    @param: panic_reason crash reason.
    @param: pbuf valid log buffer's start address.
    @param: plog_len save valid log length
    
    @return: true - successfully, false - failed

    @note: 
*/
bool get_srecorder_log_buf(char *panic_reason, char **pbuf, unsigned long *plog_len);
#endif


#ifdef CONFIG_DUMP_LOGCAT
/**
    @function: bool get_logcat_buf_info(logcat_buf_info *plogcat_buf_info)
    @brief: get logcat's buffers and their length

    @param: plogcat_buf_info 
    
    @return: true - successfully, false - failed

    @note: 
*/
bool get_logcat_buf_info(logcat_buf_info *plogcat_buf_info);
#endif

/**
    @function: void srecorder_write_reserved_mem_header(bool normal_reset, 
        bool need_flush_cache, unsigned long magic_number, int valid_log_len)
    @brief: 填写SRecorder保留内存头部信息

    @param: normal_reset 系统是否即将正常重启
    @param: need_flush_cache 是否需要刷新缓存
    @param: magic_number 魔数
    @param: valid_log_len SRecorder抓到的有效数据长度
    
    @return: none

    @note:
*/
void srecorder_write_reserved_mem_header(bool normal_reset, 
    bool need_flush_cache, unsigned long magic_number, int valid_log_len);

bool srecorder_reserve_special_mem_successfully(void);

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 5, 0))
int srecorder_get_all_syslog(char *buf, int size);
#endif

#ifdef CONFIG_DUMP_SYS_INFO
unsigned long get_cpu_name(void);
unsigned long get_machine_name(void);
unsigned long get_all_bdevs(void);
unsigned long get_bdev_lock(void);
unsigned long get_nr_swapfiles(void);
unsigned long get_swap_lock(void);
unsigned long get_swap_info(void);
unsigned long get_vmap_area_lock(void);
unsigned long get_vmap_area_list(void);
#endif

#endif /* SRECORDER_H */

