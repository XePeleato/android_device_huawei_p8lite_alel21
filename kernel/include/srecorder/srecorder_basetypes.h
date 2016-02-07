/**
    @copyright: Huawei Technologies Co., Ltd. 2012-2013. All rights reserved.
    
    @file: srecorder_basetypes.h
    
    @brief: define the prototypes for SRecorder
    
    @version: 1.0 
    
    @author: QiDechun ID: 216641
    
    @date: 2013-01-29
    
    @history:
*/

#ifndef SRECORDER_BASETYPES_H
#define SRECORDER_BASETYPES_H


/*----includes-----------------------------------------------------------------------*/


/*----c++ support--------------------------------------------------------------------*/


/*----export macroes-----------------------------------------------------------------*/

/*==================================================================*/
/*                                          define log dump switch begin                                                                          */
/*==================================================================*/

/*==================================================================*/
/*                    define log dump switch end                                                                                                  */
/*==================================================================*/

#define CRASH_REASON_KEYWORD "===============crash reason time==============="
#define SYSINFO_KEYWORD "===============sys info==============="
#define DMESG_KEYWORD "===============dmesg==============="
#define ALL_CPU_CALL_STACK_KEYWORD "===============all cpu call stack==============="
#define ALL_PS_INFO_KEYWORD "===============all ps info==============="
#define CURRENT_PS_BACKTRACE_KEYWORD "===============current ps backtrace==============="
#define SLABINFO_KEYWORD "===============slabinfo==============="
#define MODEM_ERR_KEYWORD "===============modem err==============="
#define MODEM_ERR_F3_KEYWORD "===============modem err f3==============="
#define LOGCAT_KEYWORD "===============logcat==============="

#ifndef GET_MIN
#define GET_MIN(a, b) (((a) > (b)) ? (b) : (a))
#endif

#define SRECORDER_MAGIC_NUM (0x20122102)
#define INVALID_SRECORDER_MAGIC_NUM (0xbadbadab)
#define SRECORDER_ATAG_ZONE_DAMAGED (0xdeaddead)
#define SRECORDER_RESERVED_ZONE_DAMAGED (0xdeadbeef)
#define SRECORDER_DUMP_LOG_TO_STORAGECARD (0x900dbeef)
#define ABNORMAL_RESET (1)
#define NORMAL_RESET (0)
#define CRC32_SEED_VALUE (0xFFFFFFFF)

#define KERNEL_TEXT_OFFSET (0x8000)
#define INITIAL_PAGE_TABLE_SIZE (0x4000)

#ifndef CONFIG_SRECORDER_LOG_BUF_LEN
#define CONFIG_SRECORDER_LOG_BUF_LEN 0x40000
#endif

#define SRECORDER_TIMER_EXPIRE_PERIOD (5) 

#define SRECORDER_KERNEL_LOG_NEED_BE_REPARSED (0x4a4aa4a4)
#define SRECORDER_KERNEL_LOG_NEED_NOT_BE_REPARSED (0xb1b11b1b)


/*----export prototypes---------------------------------------------------------------*/

/* log type */
typedef enum
{
    CRASH_REASON_TIME_BIT0 = 0, 
    SYS_INFO_BIT1,
    DMESG_BIT2,
    ALL_CPU_STACK_BIT3,
    ALL_PS_INFO_BIT4,
    CURRENT_PS_BACKTRACE_BIT5,
    SLABINFO_BIT6,
    MODEM_ERR_BIT7,
    MODEM_ERR_F3_BIT8,
    LOGCAT_BIT9,
    LOG_TYPE_COUNT,
    LOG_TYPE_MAX = 64,
    INVALID_LOG_TYPE = LOG_TYPE_MAX
} srecorder_info_type_e;

/* header struct for SRecorder's reserved memory */
typedef struct __srecorder_reserved_mem_header_t
{
    unsigned long version; /* the version of the SRecorder */
    unsigned long magic_num; /* the default magic number is 0x20122102 */
    unsigned long data_length; /* log length except this header */

    /*===========================================================*/
    /*       allocate 7 members for abnormal reset begin         */
    /*===========================================================*/
    unsigned long reset_flag; /* system reset_flag, 1 - abnormal reset, 0 - normal reset */
    unsigned long log_buf; /* phys addr of __log_buf */
    unsigned long log_end; /* phys addr of log_end */
    unsigned long log_buf_len; /* phys addr of log_buf_len */
    unsigned long reserved_mem_size; /* SRecorder reserved mem max length */
    unsigned long dump_ctrl_bits[2]; /* dump control bits */
    
    /*===========================================================*/
    /*           members for kerenl 3.5 and above begin          */
    /*===========================================================*/
    unsigned long log_first_idx_phys_addr; /* phys addr of log_first_idx */
    unsigned long log_next_idx_phys_addr; /* phys addr of log_next_idx */
    unsigned long log_next_seq_phys_addr;
    unsigned long log_first_idx; /* phys addr of log_end */
    unsigned long log_next_idx; /* phys addr of log_end */
    unsigned long log_next_seq[2];
    unsigned long log_content_header_size;
    unsigned long log_need_reparsed; /* it isn't used in 3.4 and below */
    /*===========================================================*/
    /*           members for kerenl 3.5 and above end            */
    /*===========================================================*/
    unsigned long crc32; /* crc32 check value */
    /*===========================================================*/
    /*       allocate 7 members for abnormal reset end           */
    /*===========================================================*/
    
    char reserved[256];
    
    /*the region following this struct is assigned for the data region*/
}  srecorder_reserved_mem_header_t, *psrecorder_reserved_mem_header_t;

/* define the header struct for log */
typedef struct __info_header
{
    srecorder_info_type_e type;
    unsigned long crc32;
    unsigned long data_len;
} srecorder_info_header_t, *psrecorder_info_header_t;


/*----global variables-----------------------------------------------------------------*/


/*----export function prototypes--------------------------------------------------------*/


#endif /* SRECORDER_BASETYPES_H */

