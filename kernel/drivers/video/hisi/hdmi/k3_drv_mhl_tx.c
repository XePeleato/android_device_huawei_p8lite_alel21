/* Copyright (c) 2008-2013, Hisilicon Tech. Co., Ltd. All rights reserved.
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License version 2 and
* only version 2 as published by the Free Software Foundation.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
* 02110-1301, USA.
* file: k3_drv_mhl_tx.c
*/
#include <linux/delay.h>
#include <linux/io.h>

#include "k3_mhl_defs.h"
#include "k3_mhl_tx.h"
#include "k3_drv_mhl_tx.h"
#include "k3_hdmi_hw_tpi.h"

#include "k3_hdmi.h"

#define LOG_TAG "mhl-drv-tx"
#include "k3_hdmi_log.h"
#include "k3_hdcp.h"

extern mhl_tx_config g_mhl_tx_config;
extern hdmi_device hdmi;

#define POWER_STATE_D3 3
#define POWER_STATE_D0_NO_MHL 2
#define POWER_STATE_D0_MHL 0
#define POWER_STATE_FIRST_INIT 0xFF

static u8 s_fw_power_state = POWER_STATE_FIRST_INIT;   // To remember the current power state.
bool g_msc_abort_flag = false;   // false when it is okay to send a new command
static u8 s_ds_hpd_status = 0;   // Preserve Downstream HPD status

#if USE_PP_MODE
static u8 packed_pixel_status = 0;
static u32 pixel_clock_freq = 0;
#endif

u8 dev_cap[16] =
{
    DEVCAP_VAL_DEV_STATE,
    DEVCAP_VAL_MHL_VERSION,
    DEVCAP_VAL_DEV_CAT,
    DEVCAP_VAL_ADOPTER_ID_H,
    DEVCAP_VAL_ADOPTER_ID_L,
    DEVCAP_VAL_VID_LINK_MODE,
    DEVCAP_VAL_AUD_LINK_MODE,
    DEVCAP_VAL_VIDEO_TYPE,
    DEVCAP_VAL_LOG_DEV_MAP,
    DEVCAP_VAL_BANDWIDTH,
    DEVCAP_VAL_FEATURE_FLAG,
    DEVCAP_VAL_DEVICE_ID_H,
    DEVCAP_VAL_DEVICE_ID_L,
    DEVCAP_VAL_SCRATCHPAD_SIZE,
    DEVCAP_VAL_INT_STAT_SIZE,
    DEVCAP_VAL_RESERVED
};

#define DISABLE_DISCOVERY tpi_modify_reg_internal(TPI_INDEXED_PAGE_2, (REG_DISC_CTRL1 & 0x00FF)*4, BIT0, 0)
#define ENABLE_DISCOVERY tpi_modify_reg_internal(TPI_INDEXED_PAGE_2, (REG_DISC_CTRL1 & 0x00FF)*4, BIT0, BIT0)
#define STROBE_POWER_ON k3_mhl_reg_modify(REG_DISC_CTRL1, BIT1, 0)

#define I2C_INACCESSIBLE -1
#define I2C_ACCESSIBLE 1

static int k3_mhl_int4_isr(void);
static void k3_mhl_cbus_isr(void);

static void k3_mhl_write_Initial_values_to_register(void);
static void k3_mhl_init_cbus_regs(void);
static void k3_mhl_force_usb_id_switch_open(void);
static void k3_mhl_release_usb_id_switch_open(void);

static void k3_mhl_process_connection(void);
static void k3_mhl_process_disconnection(void);
static void k3_mhl_cbus_process_errors(u8 int1_status);

void hal_timer_wait(u8 m_sec)
{
    unsigned long time_usec = m_sec * 1000;
    usleep_range(time_usec, time_usec);
}

/********************************************************************************
* Function:    k3_mhl_reg_read
* Description: Read a one byte register.
* Data Accessed:
* Data Updated:
* Input:
* Output:
* Return:
*********************************************************************************/
u32 k3_mhl_reg_read(u32 virtual_addr)
{
    return (readl((volatile void *)k3_mhl_slave2_parallel_addr(virtual_addr)));
}

/********************************************************************************
* Function:    k3_mhl_reg_write
* Description: Write a one byte register.
* Data Accessed:
* Data Updated:
* Input:
* Output:
* Return:
*********************************************************************************/
void k3_mhl_reg_write(u32 virtual_addr, u32 value)
{
    writel(value, (volatile void *)k3_mhl_slave2_parallel_addr(virtual_addr));
}

/**********************************************************************************
* Function:      k3_mhl_reg_modify
* Description:   Reads the register, performs an AND function on the data using the mask parameter,
*                and an OR function on the data using the value ANDed with the mask. The result is
*                then written to the device register specified in the regAddr parameter.
* Data Accessed: regAddr - Sixteen bit register address, including device page.
*                mask    - Eight bit mask
*                value   - Eight bit data to be written, combined with mask.
* Data Updated:
* Input:
* Output:
* Return:
***********************************************************************************/
void k3_mhl_reg_modify(u32 virtual_addr, u32 mask, u32 value)
{
    u32 abyte;

    abyte = k3_mhl_reg_read(virtual_addr);
    abyte &= (~mask); // first clear all bits in mask
    abyte |= (mask & value); // then set bits from value
    k3_mhl_reg_write(virtual_addr, abyte);
}

/***********************************************************************************
* Function:       k3_mhl_reg_write_block
* Description:   Writes a block of data to sequential registers
* Data Accessed: regAddr - Sixteen bit register address, including device page.
* Data Updated:
* Input:
* Output:
* Others:   This function relies on the auto-increment model used by Silicon Image devices.
*           Because of this, if a FIFO register is encountered before the end of the requested count,
*           the data remaining from the count is read from the FIFO, NOT from subsequent registers.
************************************************************************************/
void k3_mhl_reg_write_block(u32 virtual_addr, u8 *pbuffer, u8 count)
{
    int i;
    for (i = 0; i < count; i++) {
        k3_mhl_reg_write(virtual_addr + i, *pbuffer);
        ++pbuffer;
    }
}
/***********************************************************************************
* Function:      k3_mhl_reg_read_block
* Description:   Reads a block of data from sequential registers.
* Data Accessed: regAddr - Sixteen bit register address, including device page.
* Data Updated:
* Input:
* Output:
* Others:        This function relies on the auto-increment model used by
*                Silicon Image devices.  Because of this, if a FIFO register
*                is encountered before the end of the requested count, the
*                data remaining from the count is read from the FIFO, NOT
*                from subsequent registers.
************************************************************************************/
void k3_mhl_reg_read_block(u32 virtual_addr, u8 *pbuffer, u8 count)
{
    int i;
    for (i = 0; i < count; i++) {
        *pbuffer = k3_mhl_reg_read(virtual_addr + i);
        ++pbuffer;
    }
}

/********************************************************************************
* Function:       k3_mhl_sw_tpi_bank0
* Description:
* Data Accessed:
* Data Updated:
* Input:
* Output:
* Return:
*********************************************************************************/
bool k3_mhl_sw_tpi_bank0(void)
{
    /* GAMUT_HEADER1 is located in the first byte of bank1 in software TPI mode.
       The default value is 0x0A. */
    return 0x0A != tpi_read_reg(0x0);
}

/********************************************************************************
* Function:       k3_mhl_slave2_parallel_addr
* Description:    virtualaddr to absolute address
* Data Accessed:
* Data Updated:
* Input:
* Output:
* Return:
*********************************************************************************/
u32 k3_mhl_slave2_parallel_addr(u32 slave_addr)
{
    u32 parallel_addr;
    u32 page;
    u32 register_offset;

    page = slave_addr & 0xFF00;
    register_offset = (slave_addr & 0x00FF);
    switch (page) {
        case TX_PAGE_TPI:
            //Make sure to be hardware TPI mode.
            if (!k3_mhl_hw_tpimode()) {
                if (k3_mhl_sw_tpi_bank0()) {
                    tpi_modify_reg (TPI_ENABLE, BIT7, 0x00);//Bank0
                } else  {
                    tpi_modify_reg (TPI_REG_BANK0, BIT0, 0x00);//Bank1
                    tpi_modify_reg (TPI_ENABLE, BIT7, 0x00);
                }
            }

            parallel_addr = MHL_CORE_SYS;
            break;
        case TX_PAGE_L0:
            //Make sure to be software TPI mode.
            if (k3_mhl_hw_tpimode()) {
                tpi_modify_reg(TPI_INDEXED_PAGE_REG, BIT7, 0xFF);
            }

            tpi_modify_reg(TPI_REG_BANK0, BIT0, 0x00);
            parallel_addr = MHL_CORE_SYS;
            logw("Warning: Visit Page0 SW TPI Mode. May do harm to HDCP!\n");
            break;
        case TX_PAGE_L1:
            //Make sure to be software TPI mode.
            if (k3_mhl_hw_tpimode()) {
                tpi_modify_reg(TPI_INDEXED_PAGE_REG, BIT7, 0xFF);
            }

            logw("Warning: Visit Page1 SW TPI Mode. May do harm to HDCP!\n");
            parallel_addr = PARALLEL_ADDR_TX_PAGE_L1;
            break;
        case TX_PAGE_L2:
            //Make sure to be software TPI mode.
            if (k3_mhl_hw_tpimode()) {
                tpi_modify_reg(TPI_INDEXED_PAGE_REG, BIT7, 0xFF);
            }

            //Switch to bank1 to get access.
            tpi_modify_reg(TPI_REG_BANK0, BIT1, 0x01);
            parallel_addr = MHL_CORE_SYS;
            logw("Warning: Visit Page2 SW TPI Mode. May do harm to HDCP!\n");
            break;
        case TX_PAGE_CEC:
            parallel_addr = PARALLEL_ADDR_TX_PAGE_CEC;
            break;
        case TX_PAGE_CBUS:
            parallel_addr = PARALLEL_ADDR_TX_PAGE_CBUS;
            break;
        default:
            parallel_addr = MHL_CORE_SYS;
            loge("parameter invalid.\n");
    }

    return (parallel_addr + register_offset*4);
}
/********************************************************************************
* Function:      k3_mhl_hw_tpimode
* Description:   Chip specific initialization.
                 This function is for SiI 8338 Initialization: HW Reset, Interrupt enable.
* Data Accessed:
* Data Updated:
* Input:
* Output:
* Return:
*********************************************************************************/
bool k3_mhl_hw_tpimode(void)
{
    /* TPI_DEV_ID should be 0xB4 and TPI_VENDOR_B3 should be 0x30 in Hardware TPI Mode. */
    bool ret = 0;
    u8 tpi_dev_id = 0;
    u8 tpi_vendor_b3 = 0;

    tpi_dev_id = tpi_read_reg(TPI_DEVICE_ID);
    tpi_vendor_b3 = tpi_read_reg(TPI_RESERVED2);

    ret = (0xB4 == tpi_dev_id) && (0x30 == tpi_vendor_b3);

    return ret;
}

/********************************************************************************
* Function:       k3_mhl_cbus_busy
* Description:
* Data Accessed:
* Data Updated:
* Input:
* Output:
* Return:
*********************************************************************************/
bool k3_mhl_cbus_busy(void)
{
    return g_mhl_tx_config.msc_cmd_in_progress;
}

/********************************************************************************
* Function:       k3_mhl_irq_enable
* Description:
* Data Accessed:
* Data Updated:
* Input:
* Output:
* Return:
*********************************************************************************/
void k3_mhl_irq_enable(void)
{
#if 0
    MASK_INTR_1_INTERRUPTS;
    MASK_INTR_4_INTERRUPTS;
    MASK_INTR_5_INTERRUPTS;
    MASK_CBUS1_INTERRUPTS;
    MASK_CBUS2_INTERRUPTS;
    UNMASK_INTR_1_INTERRUPTS;
    UNMASK_INTR_4_INTERRUPTS;
    UNMASK_INTR_5_INTERRUPTS;
    UNMASK_CBUS1_INTERRUPTS;
    UNMASK_CBUS2_INTERRUPTS;
#endif
}

/********************************************************************************
* Function:      k3_mhl_reset_states
* Description:   Chip specific initialization.
                 This function is for SiI 8338 Initialization: HW Reset, Interrupt enable.
* Data Accessed:
* Data Updated:
* Input:
* Output:
* Return:
*********************************************************************************/
void k3_mhl_chip_initialize(void)
{
    IN_FUNCTION;

    /* false when it is okay to send a new command */
    g_mhl_tx_config.msc_cmd_in_progress = false;

    s_ds_hpd_status  = 0;
    //s_fw_power_state = POWER_STATE_D0_MHL;

#if USE_PP_MODE
    packed_pixel_status = 0;
    pixel_clock_freq = 0;
#endif

    /* setup device registers. Ensure RGND interrupt would happen. */
    k3_mhl_write_Initial_values_to_register();
    k3_mhl_init_cbus_regs();  //Write DEV_CAP

    OUT_FUNCTION;
}

/********************************************************************************
* Function:       k3_mhl_write_Initial_values_to_register
* Description:    Write Initial Register Values
* Data Accessed:
* Data Updated:
* Input:
* Output:
* Return:
*********************************************************************************/
static void k3_mhl_write_Initial_values_to_register(void)
{
    u8 reg_val = 0;

    IN_FUNCTION;

    /* HW TPI Enabled */
    //tpi_write_reg(TPI_ENABLE, 0x00);
    logi("TPI MODE OR NOT: %s.\n", k3_mhl_hw_tpimode() ? "YES" : "NO");

    /* Switch to SW TPI Mode */
    tpi_write_reg(TPI_INDEXED_PAGE_REG, 0x81);

    /* Enable HDMI mode */
    /* Enable Audio */
    tpi_write_reg(HDMI_AV_REG_OFFSET + HDMI_CORE_AV_HDMI_CTRL, 0x01);
    reg_val = tpi_read_reg(HDMI_AV_REG_OFFSET + HDMI_CORE_AV_HDMI_CTRL);
    logd("PAGE1 0x2F:0x%2x\n", reg_val);

    /* Switch to bank1 to get access */
    tpi_write_reg(TPI_REG_BANK0, 0x01);
    logi("mhl_hdmi is 0x%x\n", tpi_read_reg(TPI_REG_BANK0));

    tpi_write_reg(CBUS_DISC_CTRL2, 0xED);    //Skip RGND detect
    tpi_write_reg(CBUS_DISC_CTRL4, 0xA4);    //10K Pull up
    tpi_write_reg(CBUS_DISC_CTRL1, 0x27);  //74m
    //tpi_write_reg(CBUS_DISC_CTRL1, 0x27);   //Enable CBUS discovery

    //Switch to bank0 to get access.
    tpi_write_reg(TPI_REG_BANK0, 0x00);

    /* HW TPI Enabled */
    tpi_write_reg(TPI_ENABLE, 0x00);
    logi("TPI MODE OR NOT: %s.\n", k3_mhl_hw_tpimode() ? "YES" : "NO");

    //Set DDC master frequency = 100k
    tpi_write_reg_internal(TPI_INDEXED_PAGE_0, DDC_DELAY_CNT, 0x28);
    tpi_modify_reg(TPI_DEVICE_POWER_STATE_CTRL_REG, TX_POWER_STATE_MASK, TX_POWER_STATE_D0);

    OUT_FUNCTION;
    return;
}

/********************************************************************************
* Function:       k3_mhl_init_cbus_regs
* Description:    Initial CBus Register
* Data Accessed:
* Data Updated:
* Input:
* Output:
* Return:
*********************************************************************************/
static void k3_mhl_init_cbus_regs(void)
{
    IN_FUNCTION;

    /* Setup our devcap */
    k3_mhl_reg_write_block(REG_CBUS_DEVICE_CAP_0, dev_cap, DEVCAP_SIZE);

    OUT_FUNCTION;
}

/********************************************************************************
* Function:       k3_mhl_force_usb_id_switch_open
* Description:
* Data Accessed:
* Data Updated:
* Input:
* Output:
* Return:
*********************************************************************************/
static void k3_mhl_force_usb_id_switch_open(void)
{
    IN_FUNCTION;

    DISABLE_DISCOVERY;

    /* Force USB ID switch to open */
    tpi_modify_reg_internal(TPI_INDEXED_PAGE_2, REG_DISC_CTRL6 * 4, BIT6, BIT6);
    tpi_write_reg_internal(TPI_INDEXED_PAGE_2, REG_DISC_CTRL3 * 4, 0x86);

    OUT_FUNCTION;
}

/********************************************************************************
* Function:       k3_mhl_release_usb_id_switch_open
* Description:
* Data Accessed:
* Data Updated:
* Input:
* Output:
* Return:
*********************************************************************************/
static void k3_mhl_release_usb_id_switch_open(void)
{
    IN_FUNCTION;

    hal_timer_wait(50); // per spec
    tpi_modify_reg_internal(TPI_INDEXED_PAGE_2, (REG_DISC_CTRL6)*4, BIT6, 0x00);
    ENABLE_DISCOVERY;

    OUT_FUNCTION;
}

/******************************************************************************
* Function Name: k3_mhl_process_rgnd
* Function Description: H/W has detected impedance change and interrupted.
*                               We look for appropriate impedance range to call it MHL and enable the
*                               hardware MHL discovery logic. If not, disable MHL discovery to allow
*                               USB to work appropriately.
*                               00, 01 or 11 means USB;10 means 1K impedance (MHL);
*                               If 1K, then only proceed with wake up pulses
* Accepts: none
* Returns: none
* Globals: none
******************************************************************************/
void k3_mhl_process_rgnd(void)
{
    u8 rgndImpedance;

    IN_FUNCTION;

    /* Impedance detection has completed - process interrupt */
    rgndImpedance = tpi_read_reg_internal(TPI_INDEXED_PAGE_2, (REG_DISC_STAT2 & 0x00FF)*4) & 0x03;
    logd("RGND = 0x%02X : \n", (int)rgndImpedance);

    if (0x02 == rgndImpedance) {
        logi("MHL Device\n");
        k3_mhl_notify_rgnd_mhl();
    } else {
        // USB Established
        tpi_modify_reg_internal(TPI_INDEXED_PAGE_2, (REG_DISC_CTRL9 & 0x00FF)*4, BIT3, BIT3);
        logi("Non-MHL Device\n");
    }

    OUT_FUNCTION;

    return;
}

/********************************************************************************
* Function:       k3_mhl_tmds_control
* Description:    Control the TMDS output. MhlTx uses this to support RAP content on and off.
* Data Accessed:
* Data Updated:
* Input:
* Output:
* Return:
*********************************************************************************/
void k3_mhl_tmds_control(bool enable)
{
    IN_FUNCTION;

    if (enable == g_mhl_tx_config.tmds_output_enabled) {
        OUT_FUNCTION;
        return;
    }

    if (enable) {
        g_mhl_tx_config.tmds_output_enabled = true;
        logi("TMDS Output Enabled.\n");
        g_mhl_tx_config.disconnection_to_hdmi = false;
        g_mhl_tx_config.connection_to_hdmi = true;
        start_hdmi_process();
    } else {
        g_mhl_tx_config.tmds_output_enabled = false;
        k3_mhl_reg_modify(TPI_SYSTEM_CONTROL_DATA_REG/4, AV_MUTE_MASK, AV_MUTE_MUTED);
        logi("TMDS Output Disabled.\n");
        if (!g_mhl_tx_config.real_disconnection) {
            g_mhl_tx_config.disconnection_to_hdmi = true;
            g_mhl_tx_config.connection_to_hdmi = false;
            start_hdmi_process();
        }
    }

    OUT_FUNCTION;
}

/****************************************************************************
* Function:       k3_mhl_send_cbus_command
* Description:    Write the specified Sideband Channel command to the CBUS.Command can
*                 be a MSC_MSG command (RCP/RAP/RCPK/RCPE/RAPK), or another command
*                 such as READ_DEVCAP, SET_INT, WRITE_STAT, etc.
* Data Accessed:
* Data Updated:
* Input:    p_req - Pointer to a cbus_req_t structure containing the command to write
* Output:
* Return:   true  - successful write
*           false   - write failed
* Others:
*********************************************************************************/
bool k3_mhl_send_cbus_command(cbus_req_t *p_req)
{
    bool success = true;
    u8 startbit;

    IN_FUNCTION;

    g_mhl_tx_config.msc_cmd_in_progress = true;
    logd("Sending MSC command 0x%02x, 0x%02x, 0x%02x\n",
        p_req->command,
        (MHL_MSC_MSG == p_req->command) ? p_req->payload_u.msg_data[0] : p_req->offset_data,
        (MHL_MSC_MSG == p_req->command) ? p_req->payload_u.msg_data[1] : p_req->payload_u.msg_data[0]);

    // Set the offset and outgoing data byte right away
    k3_mhl_reg_write(REG_MSC_CMD_OR_OFFSET, p_req->offset_data);// set offset
    k3_mhl_reg_write(REG_CBUS_PRI_WR_DATA_1ST, p_req->payload_u.msg_data[0]);

    startbit = 0x00;
    switch (p_req->command) {
    case MHL_SET_INT: // Set one interrupt register = 0x60
        startbit = MSC_START_BIT_WRITE_REG;
        break;
    case MHL_WRITE_STAT: // Write one status register = 0x60 | 0x80
        startbit = MSC_START_BIT_WRITE_REG;
        break;
    case MHL_READ_DEVCAP: // Read one device capability register = 0x61
        startbit = MSC_START_BIT_READ_REG;
        break;
    case MHL_GET_STATE:         // 0x62 -
    case MHL_GET_VENDOR_ID:     // 0x63 - for vendor id
    case MHL_SET_HPD:           // 0x64 - Set Hot Plug Detect in follower
    case MHL_CLR_HPD:           // 0x65 - Clear Hot Plug Detect in follower
    case MHL_GET_SC1_ERRORCODE: // 0x69 - Get channel 1 command error code
    case MHL_GET_DDC_ERRORCODE: // 0x6A - Get DDC channel command error code.
    case MHL_GET_MSC_ERRORCODE: // 0x6B - Get MSC command error code.
    case MHL_GET_SC3_ERRORCODE: // 0x6D - Get channel 3 command error code.
        k3_mhl_reg_write(REG_MSC_CMD_OR_OFFSET, p_req->command);
        startbit = MSC_START_BIT_MSC_CMD;
        break;
    case MHL_MSC_MSG:
        k3_mhl_reg_write(REG_CBUS_PRI_WR_DATA_2ND, p_req->payload_u.msg_data[1]);
        k3_mhl_reg_write(REG_MSC_CMD_OR_OFFSET, p_req->command);
        startbit = MSC_START_BIT_VS_CMD;
        break;
    case MHL_WRITE_BURST:
        k3_mhl_reg_write(REG_MSC_WRITE_BURST_LEN, p_req->length - 1);
        // Now copy all bytes from array to local scratchpad
        if (NULL == p_req->payload_u.pdatabytes) {
            loge("Put pointer to WRITE_BURST data in req.pdatabytes!!!\n");
            success = false;
        } else {
            u8 *pdata = p_req->payload_u.pdatabytes;
            logd("Writing data into scratchpad\n");
            k3_mhl_reg_write_block(REG_CBUS_SCRATCHPAD_0, pdata, p_req->length);
            startbit = MSC_START_BIT_WRITE_BURST;
        }
        break;
    default:
        success = false;
        break;
    }

    /* Trigger the CBUS command transfer using the determined start bit. */
    if (success) {
        k3_mhl_reg_write(REG_MSC_COMMAND_START, startbit);
    }

    logd("%s\n", success ? "Success" : "Failed");
    OUT_FUNCTION;

    return success;
}
/********************************************************************************
* Function:       k3_mhl_deviceisr
* Description:    check mhl isr
* Data Accessed:
* Data Updated:
* Input:
* Output:
* Return:
* Others:
*********************************************************************************/
void k3_mhl_deviceisr(void)
{
    u8 int_m_status = 0;//master int status
    u8 i = 0;
#if USE_HDCP
    int state_hdcp = 0;
#endif

    IN_FUNCTION;
    /*Look at discovery interrupts if not yet connected.*/
    do {
        if (I2C_INACCESSIBLE == k3_mhl_int4_isr()) {
            loge("we are i2c_inaccessible\n");
            OUT_FUNCTION;
            return;// don't do any more I2C traffic until the next interrupt.
        }

        k3_mhl_cbus_isr();
#if USE_HDCP
        hdcp_handler(state_hdcp);
#endif

        k3_mhl_process_events();
        int_m_status = tpi_read_reg_internal(TPI_INDEXED_PAGE_0, (REG_INTR_STATE & 0x00FF)*4);

        if (0xFF == int_m_status) {
            int_m_status = 0;
            logd("exiting isr DUE TO int_m_status - 0xFF\n");
        }

        i++;
        int_m_status &= 0x01; //RG mask bit 0
        if (i > 60) {
            loge("force exit SiiDeviceIsr \n");
            break;
        } else if (i > 50) {
            tpi_write_reg_internal(TPI_INDEXED_PAGE_2, (REG_INTRM4 & 0x00FF)*4, 0x77);
            tpi_write_reg_internal(TPI_INDEXED_PAGE_2, (REG_INTRM5 & 0x00FF)*4, 0x1F);
            k3_mhl_reg_write(REG_CBUS_SET_INT_0, 0x0F);
            k3_mhl_reg_write(REG_CBUS_SET_INT_1, 0x02);
            k3_mhl_reg_write(REG_CBUS_INT_0, 0xFF);
            k3_mhl_reg_write(REG_CBUS_INT_1, 0xFE);
            k3_mhl_reg_write(REG_CBUS_CEC_ABORT_INT, 0xFF);
            k3_mhl_reg_write(REG_CBUS_DDC_ABORT_INT, 0x87);
            k3_mhl_reg_write(REG_CBUS_MSC_MT_ABORT_INT, 0x8F);
            k3_mhl_reg_write(REG_CBUS_MSC_MR_ABORT_INT, 0xBF);
#if USE_HDCP
            tpi_write_reg(TPI_INTERRUPT_STATUS_REG, BIT7 | BIT5);
#endif
            loge("something error in SiiDeviceIsr \n");
        }
    } while (int_m_status);

    OUT_FUNCTION;
}

/********************************************************************************
* Function:       k3_mhl_int4_isr
* Description:   Look for interrupts on INTR4 (MHL Interrupt Source 4 Register 0x51)
* Input:
* Output:
* Return:
* Others:       0   reg_intrm4_stat0        r/w SCDT status change. Asserted if set to 1. Write 1 to clear          intr_scdt
*               1   reg_intrm4_stat1        r/w RPWR5v status change. Asserted if set to 1. Write 1 to clear        intr_rpwr
*               2   reg_intrm4_stat2        r/w MHL mode established. Asserted if set to 1. Write 1 to clear        intr_mhl_est
*               3   reg_intrm4_stat3        r/w USB mode established. Asserted if set to 1. Write 1 to clear        intr_usb_est
*               4   reg_intrm4_stat4        r/w CBUS lockout window started. Asserted if set to 1. Write 1 to clear.    intr_lockout_start
*               5   reg_intrm4_stat5        r/w CBUS disconnect. Write 1 to clear.                          intr_cbusflt
*               6   reg_intrm4_stat6        r/w RGND Detection is done and RGND is available for read.          intr_rgndrdy
*               7   reg_intrm4_stat7        r   The status of pvt_heartbeat_max_fail                            pvt_heartbeat_max_fail

*********************************************************************************/
static int k3_mhl_int4_isr(void)
{
    u8 int4_status = 0;
    int return_value = I2C_ACCESSIBLE;

    IN_FUNCTION;

    /* read status */
    int4_status = tpi_read_reg_internal(TPI_INDEXED_PAGE_2, (REG_INTRM4 & 0x00FF)*4);
    /* clear all interrupts */
    tpi_write_reg_internal(TPI_INDEXED_PAGE_2, (REG_INTRM4 & 0x00FF)*4, int4_status);

    logd("INT4 Status = 0x%02x\n", int4_status);

    /*When I2C is inoperational (D3) and a previous interrupt brought us here,do nothing.*/
    if (!int4_status) {
        OUT_FUNCTION;
        return return_value;
    }

    if (0xFF == int4_status) {
        loge("i2c_inaccessible!!\n");
        return_value = I2C_INACCESSIBLE;
        OUT_FUNCTION;
        return return_value;
    }

    /* process MHL_EST interrupt */
    if (int4_status & BIT_INTRM4_MHL_EST) {
        /* MHL_EST_INT(mhl mode established) */
        logi("MHL ESTABLISH INTERRUPT!\n");
        k3_mhl_process_connection();
    } else if (int4_status & BIT_INTRM4_USB_EST) {
        /* process USB_EST interrupt */
        logd("uUSB-A type device detected.\n");
        tpi_write_reg_internal(TPI_INDEXED_PAGE_2, (REG_DISC_STAT2 & 0x00FF)*4, 0x80);
        return_value = I2C_INACCESSIBLE;
    }

    if (int4_status & BIT_INTRM4_CBUS_DISCONNECT) {
        logi("MHL CBUS DISCONNECT INTERRUPT!\n");
        k3_mhl_process_disconnection();
        return_value = I2C_INACCESSIBLE;
    }

    /* Can't succeed at these in D3 */
    if (s_fw_power_state != POWER_STATE_D3) {
        // CBUS Lockout interrupt?
        if (int4_status & BIT_INTRM4_CBUS_LKOUT) {
            logd("CBus Lockout\n");
            k3_mhl_force_usb_id_switch_open();
            k3_mhl_release_usb_id_switch_open();
        }
    }

    //process RGND interrupt,todo:need to judge power state
    if (int4_status & BIT6) {
        logd("RGND detection\n");
        k3_mhl_process_rgnd();
    }

    OUT_FUNCTION;

    return return_value;
}

/********************************************************************************
* Function:       k3_mhl_cbus_hpd_isr
* Description:    check mhl hpd isr
* Data Accessed:
* Data Updated:
* Input:
* Output:
* Return:
* Others:
*********************************************************************************/
static void k3_mhl_cbus_hpd_isr(u8 cbus_int_0, u8 cbus_status)
{
    u8 status = 0;

    IN_FUNCTION;

    if (cbus_int_0 & BIT_CBUS_HPD_CHANGE) {
        /* Check if a SET_HPD came from the downstream device. */
        if (BIT_CBUS_HPD & (s_ds_hpd_status ^ cbus_status)) {
            status = cbus_status & BIT_CBUS_HPD;
            logd("Downstream HPD changed to: 0x%02x\n", cbus_status);

            k3_mhl_notify_hpd_change(status);
            s_ds_hpd_status = cbus_status;
        }
    }

    OUT_FUNCTION;
}

/********************************************************************************
* Function:       k3_mhl_cbus_msc_req_done_isr
* Description:    check mhl msc req done isr
* Data Accessed:
* Data Updated:
* Input:
* Output:
* Return:
* Others:
*********************************************************************************/
static void k3_mhl_cbus_msc_req_done_isr(u8 cbus_int_0)
{
    u8 data = 0;

    IN_FUNCTION;

    /* MSC_REQ_DONE received. */
    if (cbus_int_0 & BIT_CBUS_MSC_MT_DONE) {
        logd("MSC_REQ_DONE\n");
        g_mhl_tx_config.msc_cmd_in_progress = false;

        /* only do this after cBusInt interrupts are cleared above */
        data = k3_mhl_reg_read(REG_CBUS_PRI_RD_DATA_1ST);
        k3_mhl_msc_command_done(data);

        if (g_mhl_tx_config.hdcp_start_pending) {
            g_mhl_tx_config.hdcp_start_pending = false;
            logd("Toggle TMDS HDCP .\n");
        }
    }

    OUT_FUNCTION;
}

/********************************************************************************
* Function:       k3_mhl_cbus_write_stat_isr
* Description:    check mhl write state isr
* Data Accessed:
* Data Updated:
* Input:
* Output:
* Return:
* Others:
*********************************************************************************/
static void k3_mhl_cbus_write_stat_isr(u8 cbus_int_0)
{
    u8 i;
    u8 status[4] = {0};

    IN_FUNCTION;

    if (cbus_int_0 & BIT_CBUS_MSC_MR_WRITE_STAT) {
        logd("CBUS INT:MSC_MR_WRITE_STAT\n");

        for (i = 0; i < 4; ++i) {
            status[i] = k3_mhl_reg_read(REG_CBUS_WRITE_STAT_0 + i);
            /* clear as we go */
            k3_mhl_reg_write((REG_CBUS_WRITE_STAT_0 + i), 0xFF); /* future status[i]*/
        }
        k3_mhl_got_status(status[0], status[1]);
    }

    OUT_FUNCTION;
}

/********************************************************************************
* Function:       k3_mhl_cbus_msc_msg_isr
* Description:    check mhl msc msg isr
* Data Accessed:
* Data Updated:
* Input:
* Output:
* Return:
* Others:
*********************************************************************************/
static void k3_mhl_cbus_msc_msg_isr(u8 cbus_int_0)
{
    u8 mscMsg[2];

    IN_FUNCTION;

    /* MSC_MSG (RCP/RAP) */
    if (cbus_int_0 & BIT_CBUS_MSC_MR_MSC_MSG) {
        logd("MSC_MSG Received\n");

        /* Two bytes arrive at registers 0x18 and 0x19 */
        mscMsg[0] = k3_mhl_reg_read(REG_CBUS_PRI_VS_CMD);
        mscMsg[1] = k3_mhl_reg_read(REG_CBUS_PRI_VS_DATA);
        logd("MSC MSG: 0x%02x 0x%02x\n", mscMsg[0], mscMsg[1]);
        k3_mhl_got_msc_msg(mscMsg[0], mscMsg[1]);
    }

    OUT_FUNCTION;
}

/********************************************************************************
* Function:       k3_mhl_cbus_write_burst_isr
* Description:    check mhl write burst receive isr
* Data Accessed:
* Data Updated:
* Input:
* Output:
* Return:
* Others:
*********************************************************************************/
static void k3_mhl_cbus_write_burst_isr(u8 cbus_int_0)
{
    IN_FUNCTION;

    /* MSC WRITE_BURST command received interrupt status */
    if (cbus_int_0 & BIT_CBUS_MSC_MR_WRITE_BURST) {
        /* Received WRITE_BURST complete */
        logd("MSC WRITE_BURST command received.\n");
    }

    OUT_FUNCTION;
}

/********************************************************************************
* Function:       k3_mhl_cbus_set_int_isr
* Description:    check mhl set int isr
* Data Accessed:
* Data Updated:
* Input:
* Output:
* Return:
* Others:
*********************************************************************************/
static void k3_mhl_cbus_set_int_isr(u8 cbus_int_0)
{
    u8 intr[4] = {0};

    IN_FUNCTION;

    if (cbus_int_0 & BIT_CBUS_MSC_MR_SET_INT) {
        logd("MHL INTR Received\n");
        k3_mhl_reg_read_block(REG_CBUS_SET_INT_0, intr, 4);
        /* We are interested only in first two bytes. */
        k3_mhl_got_intr(intr[0], intr[1]);

        /* Clear all interrupts */
        k3_mhl_reg_write_block(REG_CBUS_SET_INT_0, intr, 4);
    }

    OUT_FUNCTION;
}

/********************************************************************************
* Function:       k3_mhl_cbus_isr
* Description:    check mhl isr
* Data Accessed:
* Data Updated:
* Input:
* Output:
* Return:
* Others:
*********************************************************************************/
static void k3_mhl_cbus_isr(void)
{
    u8 cbus_int = k3_mhl_reg_read(REG_CBUS_INT_0);
    u8 cbus_status = k3_mhl_reg_read(REG_CBUS_STATUS);

    IN_FUNCTION;

    logd("MHL cbus_status is 0x%02x\n", cbus_status);

    if (cbus_int) {
        k3_mhl_reg_write(REG_CBUS_INT_0, cbus_int);
        logd("CBUS INTR_0: 0x%02x\n", cbus_int);

        k3_mhl_cbus_hpd_isr(cbus_int, cbus_status);

        k3_mhl_cbus_msc_req_done_isr(cbus_int);

        k3_mhl_cbus_write_stat_isr(cbus_int);

        k3_mhl_cbus_msc_msg_isr(cbus_int);

        k3_mhl_cbus_write_burst_isr(cbus_int);

        k3_mhl_cbus_set_int_isr(cbus_int);
    }

    cbus_int = k3_mhl_reg_read(REG_CBUS_INT_1);

    if (cbus_int) {
        logd("CBUS INTR_1: 0x%02X\n", cbus_int);
        k3_mhl_reg_write(REG_CBUS_INT_1, cbus_int);
        k3_mhl_cbus_process_errors(cbus_int);
    }

    OUT_FUNCTION;
}

/********************************************************************************
* Function:       k3_mhl_process_connection
* Description:
* Data Accessed:
* Data Updated:
* Input:
* Output:
* Return:
* Others:
*********************************************************************************/
static void k3_mhl_process_connection(void)
{
    u8 cbus_status = 0;

    IN_FUNCTION;

    cbus_status = k3_mhl_reg_read(REG_CBUS_STATUS);
    logd("MHL Cable Connected. CBUS_STATUS = 0x%02x\n", cbus_status);

    /* Discovery over-ride: reg_disc_ovride*/
    tpi_write_reg_internal(TPI_INDEXED_PAGE_2, (REG_MHLTX_CTL1 & 0x00FF)*4, 0x10);

    s_fw_power_state = POWER_STATE_D0_MHL;

    /* Keep the discovery enabled. Need RGND interrupt */
    ENABLE_DISCOVERY;

    k3_mhl_notify_connection(true);

    OUT_FUNCTION;
}

/********************************************************************************
* Function:       k3_mhl_process_disconnection
* Description:
* Data Accessed:
* Data Updated:
* Input:
* Output:
* Return:
* Others:
*********************************************************************************/
static void k3_mhl_process_disconnection(void)
{
    u8 int4_status;

    IN_FUNCTION;

    /* clear all interrupts */
    int4_status = tpi_read_reg_internal(TPI_INDEXED_PAGE_2, (REG_INTRM4 & 0x00FF)*4);
    tpi_write_reg_internal(TPI_INDEXED_PAGE_2, (REG_INTRM4 & 0x00FF)*4, int4_status);

    /* cable disconnect implies downstream HPD low */
    s_ds_hpd_status &= ~BIT_CBUS_HPD;
    k3_mhl_reg_write(REG_CBUS_STATUS, s_ds_hpd_status);

    g_mhl_tx_config.real_disconnection = true;
    k3_mhl_notify_hpd_change(0);

    if (POWER_STATE_D0_MHL == s_fw_power_state) {
        k3_mhl_notify_connection(false);
    }
#if defined (CONFIG_SWITCH_FSA9685)
    fsa9685_manual_detach();
#endif
    OUT_FUNCTION;
}

/********************************************************************************
* Function:       k3_mhl_cbus_process_ddc_errors
* Description:
* Data Accessed:
* Data Updated:
* Input:
* Output:
* Return:
* Others:
*********************************************************************************/
static void k3_mhl_cbus_process_ddc_errors(u8 int1_status)
{
    u8 ddc_abort_reason = 0;

    IN_FUNCTION;

    /* Chip got some incorrect DDC data.*/
    if (int1_status & BIT_CBUS_DDC_ABRT) {
        ddc_abort_reason = k3_mhl_reg_read(REG_CBUS_DDC_ABORT_INT);
        k3_mhl_reg_write(REG_CBUS_DDC_ABORT_INT, ddc_abort_reason);
        loge("CBUS DDC ABORT happened, reason:: 0x%02x\n", ddc_abort_reason);
        if (ddc_abort_reason & BIT_CBUS_DDC_PEER_ABORT) {
            g_msc_abort_flag = true;
        }
    }

    OUT_FUNCTION;
}

/********************************************************************************
* Function:       k3_mhl_cbus_process_cmd_errors
* Description:
* Data Accessed:
* Data Updated:
* Input:
* Output:
* Return:
* Others:
*********************************************************************************/
static void k3_mhl_cbus_process_cmd_errors(u8 int1_status)
{
    u8 msc_mt_abort_int_status = 0;

    IN_FUNCTION;

    /* Abort as transmitter / requester. */
    if (!(int1_status & BIT_CBUS_CMD_ABORT)) {
        OUT_FUNCTION;
        return;
    }

    logd("CBUS CMD ABORT:last command sent was not successful.\n");

    g_mhl_tx_config.msc_cmd_in_progress = false;
    DECREMENT_CBUS_REFERENCE_COUNT(k3_mhl_cbus_process_errors);
    msc_mt_abort_int_status = k3_mhl_reg_read(REG_CBUS_MSC_MT_ABORT_INT);

    if (!msc_mt_abort_int_status) {
        OUT_FUNCTION;
        return;
    }

    k3_mhl_reg_write(REG_CBUS_MSC_MT_ABORT_INT, msc_mt_abort_int_status);
    logd("CBUS:: MSC ABORTED as Requester: 0x%02x. \n", msc_mt_abort_int_status);

    logd("Bit0----Requestor MAXFAIL - retry threshold exceeded\n"
         "Bit1----Protocol Error\n"
         "Bit2----Requestor translation layer timeout\n"
         "Bit3----Undefined opcode\n"
         "Bit7----MSC Peer sent an ABORT\n");

    if (BIT_CBUS_MSC_MT_ABORT_INT_MSC_MT_PEER_ABORT & msc_mt_abort_int_status) {
        g_msc_abort_flag = true;
    }

    OUT_FUNCTION;
}

/********************************************************************************
* Function:       k3_mhl_cbus_process_msc_errors
* Description:
* Data Accessed:
* Data Updated:
* Input:
* Output:
* Return:
* Others:
*********************************************************************************/
static void k3_mhl_cbus_process_msc_errors(u8 int1_status)
{
    u8 msc_mr_abort_int_status = 0;

    IN_FUNCTION;

    /* Abort as responser. */
    if (!(int1_status & BIT_CBUS_MSC_ABORT)) {
        OUT_FUNCTION;
        return;
    }

    msc_mr_abort_int_status = k3_mhl_reg_read(REG_CBUS_MSC_MR_ABORT_INT);
    if (!msc_mr_abort_int_status) {
        OUT_FUNCTION;
        return;
    }

    k3_mhl_reg_write(REG_CBUS_MSC_MR_ABORT_INT, msc_mr_abort_int_status);
    logd("CBUS:: MSC ABORTED as Responser, 0x%02x\n", msc_mr_abort_int_status);

    logd("Bit0----Requestor MAXFAIL - retry threshold exceeded\n"
         "Bit1----Protocol Error\n"
         "Bit2----Requestor translation layer timeout\n"
         "Bit3----Undefined opcode\n"
         "Bit7----MSC Peer sent an ABORT\n");

    if (BIT_CBUS_MSC_MR_ABORT_INT_MSC_MR_PEER_ABORT & msc_mr_abort_int_status) {
        g_msc_abort_flag = true;
    }

    OUT_FUNCTION;
}

/********************************************************************************
* Function:       k3_mhl_cbus_process_errors
* Description:
* Data Accessed:
* Data Updated:
* Input:
* Output:
* Return:
* Others:
*********************************************************************************/
static void k3_mhl_cbus_process_errors(u8 int1_status)
{
    /* At this point, we only need to look at the abort interrupts. */
    int1_status &= (BIT_CBUS_MSC_ABORT | BIT_CBUS_DDC_ABRT | BIT_CBUS_CMD_ABORT);

    IN_FUNCTION;

    if (0 == int1_status) {
        OUT_FUNCTION;
        return;
    }

    k3_mhl_cbus_process_ddc_errors(int1_status);

    k3_mhl_cbus_process_cmd_errors(int1_status);

    k3_mhl_cbus_process_msc_errors(int1_status);

    OUT_FUNCTION;
}

/********************************************************************************
* Function:    k3_mhl_pow_bit_change
* Description: Alert the driver that the peer's POW bit has changed so that it can take
               action if necessary.
* Data Accessed:
* Data Updated:
* Input:
* Output:
* Return:
* Others:
*********************************************************************************/
void k3_mhl_pow_bit_change(bool enable)
{
    IN_FUNCTION;

    // MHL peer device has it's own power
    if (enable) {
        tpi_modify_reg_internal(TPI_INDEXED_PAGE_2, (REG_DISC_CTRL8)*4, BIT2, 0xFF);
        logd("POW bit 0->1, set DISC_CTRL8[2] = 1\n");
    }

    OUT_FUNCTION;
}

#if USE_PP_MODE
void k3_mhl_tx_drv_apply_PPmode_settings(void)
{
    IN_FUNCTION;

    if (packed_pixel_status) {
        /* Packed Pixel mode enabled. */
        logi("Using 16-bit mode (Packed Pixel)\n");
        tpi_modify_reg_internal(TPI_INDEXED_PAGE_2, (REG_MHLTX_CTR10&0xFF)*4,
            BIT_VID_MODE_m1080p_MASK, BIT_VID_MODE_m1080p_ENABLE);
    } else {
        /* Normal Mode, Packed Pixel mode disabled. */
        logi("Using 24-bit mode (non-Packed Pixel)\n");
        tpi_modify_reg_internal(TPI_INDEXED_PAGE_2, (REG_MHLTX_CTR10&0xFF)*4,
            BIT_VID_MODE_m1080p_MASK, BIT_VID_MODE_m1080p_DISABLE);
    }

    OUT_FUNCTION;
}

void k3_mhl_tx_drv_set_pixel_clk_freq(u32 pixel_clkfreq)
{
    logi("Setting pixel clock frequency = 0x%x\n", pixel_clkfreq);
    pixel_clock_freq = pixel_clkfreq;
}

void k3_mhl_tx_drv_set_packed_pixel_status(int support_packed_pixel)
{
    logi("Setting Packed Pixel = 0x%x\n", support_packed_pixel);
    packed_pixel_status = support_packed_pixel;
}

u8 k3_mhl_tx_drv_get_PPstatus(void)
{
    return packed_pixel_status;
}

void k3_mhl_tx_drv_set_3D_mode(bool s3D,u8 s3d_structure)
{
    IN_FUNCTION;

    if (s3D == true && s3d_structure == HDMI_S3D_FRAME_PACKING) {
        logd("Using frame packing\n");
        tpi_modify_reg_internal(TPI_INDEXED_PAGE_2, (REG_EN_3D_CONV&0xFF)*4, BIT_VID_OVRRD_3DCONV_EN_MASK, BIT_VID_OVRRD_3DCONV_EN_FRAME_PACK);
        tpi_write_reg_internal(TPI_INDEXED_PAGE_2, (REG_CTRL_3D_CONV&0xFF)*4, 0x04);
    } else {
        logd("NOT using frame packing\n");
        tpi_modify_reg_internal(TPI_INDEXED_PAGE_2, (REG_EN_3D_CONV&0xFF)*4, BIT_VID_OVRRD_3DCONV_EN_MASK, BIT_VID_OVRRD_3DCONV_EN_NORMAL);
        tpi_write_reg_internal(TPI_INDEXED_PAGE_2, (REG_CTRL_3D_CONV&0xFF)*4, 0x00);
    }

    OUT_FUNCTION;
}
#endif
