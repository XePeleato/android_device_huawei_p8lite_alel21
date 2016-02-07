#ifndef MNTN_WATCHPOINT_COMMON_H
#define  MNTN_WATCHPOINT_COMMON_H

/* 
define
*/

#define MAX_DUBUG_MATCHPOINT   (16)
#define MAX_CPU		       (4)
#define ACPU_DEBUG_REG_BASE_ADDR  (0xF6580000)
#define SOC_MODEM_BASE_ADDR (0xF6600000)
#define ACPU0_DEBUG_REG_OFFSET (0x00010000)
#define ACPU1_DEBUG_REG_OFFSET (0x00012000)
#define ACPU2_DEBUG_REG_OFFSET (0x00014000)
#define ACPU3_DEBUG_REG_OFFSET  (0x00016000)
/*distance of the same register between neighboring ACPU*/
#define ACPU_DEBUG_REG_DIST        (0x00002000) 

/*The offset of the specific register.*/
#define DBGDIDR (0)
#define DBGWFAR (6)
#define DBGDSCR (34)    /* Debug Status and Control Register */
#define DBGWVR (96)     /*The numbers from 96 to 111 are reserved for DBGWVR see p2254 in ARM architecture reference book*/
#define DBGWCR (112)    /*The numbers from 112 to 127 are reserved for DBGWCR*/
#define DBGOSLAR (192) /*OS Lock Access*/
#define DBGOSLSR (193)
#define DBGPRCR (196)   /*Device Powerdown and Reset Control*/
#define DBGLAR (1004)    /*Lock Access */
#define DBGLSR (1005)    /*Lock Status*/
#define DBGAUTHSTATUS (1006)  /*Authentication Status*/
#define DBGDEVID (1010)

#define DBGDIDR_WRPs_SHIFT (28) /*The number of implemented watchpoints is one more than the value of this field.*/
#define DBGDIDR_VER_SHIFT (16)    /*The Debug architecture version. 4 bits*/
#define DBGDIDR_DEVID_SHIFT (15) /*Debug Device ID Register, DBGDEVID, implemented.*/

#define DBGDSCR_MDBGen_SHIFT (15)  /*Monitor debug-mode enable, 1bit */
#define DBGDSCR_MOE_SHIFT (2) /*Method of Debug entry. For details of this field see Method of Debug entry on page C11-2219. 4bits*/

#define DBGWCR_MASK_SHIFT (24)  /* bit 24 to bit 28 */
#define DBGWCR_BAS_SHIFT (5)    /* bit 5 to bit 12 */
#define DBGWCR_LSC_SHIFT (3)    /* bit 3 to bit 4 */
#define DBGWCR_PAC_SHIFT (1)    /* bit 1 to bit 2 */

#define DBGDEVID_WPAddrMask_SHIFT (4)
#define DEBUG_INVALID_ADDR  (0)
#define DEBUG_EXCEPTION_TYPE   (2)

#define DBGWCR_EN            (1)
#define DBGWCR_DIS           (0)

#define WP_READ_VALUE_ERROR         (0xEEEE2202)     /*EEEERROR*/
#ifdef CONFIG_DEBUG_FS
#define WP_DBG_SIZE_PAGE            (2048)
#define WP_DBG_SIZE_ONE_SHOW        (512)
#endif

/* 
enum, struct 
*/
enum wp_type {
    WP_RESERVED,
    WP_MATCH_READ,
    WP_MATCH_WRITE,
    WP_MATCH_RW
};

enum wp_cfg_type {
    WP_WT,      /* write through: write buffer && register */
    WP_WB,      /* write buffer */
};


/* 
function declare
*/

extern void direct_enable_monitor_mode(unsigned int cpu);
extern void direct_set_all_watchpoints_one_cpu(unsigned int cpu);
extern void start_all_watchpoint_one_cpu(unsigned int cpu, enum wp_cfg_type ct);
extern void stop_all_watchpoint_one_cpu(unsigned int cpu, enum wp_cfg_type ct);
extern unsigned int find_and_init_one_watchpoint(watchpoint_t* wpi, unsigned int cpu, enum wp_cfg_type ct);
extern unsigned int find_and_clear_one_watchpoint(unsigned int addr, unsigned int cpu, enum wp_cfg_type ct);
extern void clear_all_watchpoint_one_cpu(unsigned int cpu, enum wp_cfg_type ct);
extern unsigned int get_max_watchpoint_number(unsigned int cpu);
extern int do_watchpoint_callback(unsigned int addr, unsigned int cpu, void* sys_param);
extern void show_watchpoint_regs(unsigned int cpu, char *buf);
extern void show_watchpoint_info(unsigned int cpu, char *buf);

#endif
