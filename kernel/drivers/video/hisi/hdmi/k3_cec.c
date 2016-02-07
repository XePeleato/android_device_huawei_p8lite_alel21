/***********************************************************************************/
/*  Copyright (c) 2002-2006, Silicon Image, Inc.  All rights reserved.             */
/*  No part of this work may be reproduced, modified, distributed, transmitted,    */
/*  transcribed, or translated into any language or computer format, in any form   */
/*  or by any means without written permission of: Silicon Image, Inc.,            */
/*  1060 East Arques Avenue, Sunnyvale, California 94085                           */
/***********************************************************************************/

#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/kthread.h>

#include "k3_hdmi_hw.h"
#include "k3_hdmi.h"
#include "k3_cec.h"

#define LOG_TAG "k3_cec"
#include "k3_hdmi_log.h"

#define cec_log logi
#define cec_loge loge
#define cec_logi logi
#define cec_msg printk


extern hdmi_device hdmi;
#define CEC_CMD_POOL_SIZE 512

#define CEC_CMD_STR_LEN 128
typedef struct
{
    bool    cec_start;
    struct  task_struct  *cec_thread;
    u8      physical_addr[4];                     /**< CEC 物理地址 */
    u8      network[HDMI_CEC_LOGICALADD_BUTT];    /**< CEC 构建的网路结构，为1表示该设备能够响应CEC命令 */
    u8      logical_addr;                         /**< CEC 逻辑地址，默认为：0x04. */
    u8      cec_task ;
    u8      new_task;
    u8      tx_state;
    u8      result[16];

    hdmi_cec_cmd cec_cmd_pool[CEC_CMD_POOL_SIZE];
    u32 cmd_pool_rptr;
    u32 cmd_pool_wptr;
    u32 can_read_num;
    u32 get_cmd_counter;
    wait_queue_head_t cec_wait_queue_head ;
}hdmi_cec_attr;

static hdmi_cec_attr  g_cec_attr;
static bool g_cec_enable = false;  //default disable cec

enum _CEC_MESSAGE_E
{
    K3_HDMI_CEC_MESSAGE_RETURN_OK_WITH_RESPONES = 0X00,
    K3_HDMI_CEC_MESSAGE_RETURN_OK_WITHOUT_RESPONES,
    K3_HDMI_CEC_MESSAGE_RETURN_ERROR_NOSUPPORT,
    K3_HDMI_CEC_MESSAGE_RETURN_BUTT
};

u8 cec_register_read(u32 reg_addr)
{
    return (u8)read_reg(HDMI_CEC_BASE, reg_addr*4);
}

void cec_register_write(u32 reg_addr, u8 value)
{
    return write_reg(HDMI_CEC_BASE, reg_addr*4, value);
}

void cec_register_modify(u32 reg_addr, u8 mask, u8 value)
{
    u8 set_val = cec_register_read(reg_addr);
    set_val &= (~mask);        //first clear all bits in mask
    set_val |= (mask & value); //then set bits from value
    cec_register_write(reg_addr, set_val);
}

void cec_register_read_block(u32 reg_addr, u8* buffer, u8 length)
{
    int index = 0;

    for(index = 0; index < length; index++){
        buffer[index] = cec_register_read(reg_addr + index);
    }
}

void cec_register_write_block(u32 reg_addr, u8* buffer, u8 length)
{
    int index = 0;

    for(index = 0; index < length; index++){
        cec_register_write((reg_addr + index), buffer[index]);
    }
}

int cec_get_address(u8 *p_phy_addr, u8 *p_logical_addr)
{
    //Only invoke in private mode
    if (true != g_cec_attr.cec_start) {
        return HI_FAILURE;
    }
    memcpy(p_phy_addr, (g_cec_attr.physical_addr), 4);
    *p_logical_addr = g_cec_attr.logical_addr;

    return HI_SUCCESS;
}

char * cec_cmd_tostr(hdmi_cec_param* cec_cmd, char *str)
{
    int i = 0;
    int count = 0;

    count = snprintf(str, CEC_CMD_STR_LEN, "CECCMD:%d %d %d %d ", cec_cmd->en_src_addr, cec_cmd->en_dst_addr, cec_cmd->opcode, cec_cmd->un_operand.raw_data.length);
    for (i = 0; i < sizeof(cec_cmd->un_operand.raw_data.data); i++) {
        count += snprintf(str+count, CEC_CMD_STR_LEN-count, "%d ", cec_cmd->un_operand.raw_data.data[i]);
    }
    return str;
}

hdmi_cec_param* cec_str_tocmd(const char *str, hdmi_cec_param* cec_cmd)
{
    int count = 0;
    int i = 0;
    int opcode = 0;
    int length = 0;
    int data[15] = {0};

    memset (cec_cmd, 0, sizeof(hdmi_cec_param));

    count = sscanf(str, HDMI_CMD_CEC"%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d",
        (int *)&cec_cmd->en_src_addr, (int *)&cec_cmd->en_dst_addr, &opcode, &length,
        &data[0], &data[1], &data[2], &data[3], &data[4], &data[5], &data[6], &data[7],
        &data[8], &data[9], &data[10],&data[11], &data[12], &data[13], &data[14]);

    if (-1 == count) {
        loge("cec_str_tocmd failed!\n");
    }

    cec_cmd->opcode = (u8)opcode;
    cec_cmd->un_operand.raw_data.length = (u8)length;
    for (i = 0; i < 15; i++) {
        cec_cmd->un_operand.raw_data.data[i] = (u8)data[i];
    }

    return cec_cmd;
}

//------------------------------------------------------------------------------
// Function Name: cec_capture_id_set
// Function Description:
//
// Accepts: u8 logical_address
// Returns: error code
// Globals: none

// Affected Register(s): CEC_CAPTURE_ID
// Document Reference  : CEC Promgramming Interface (CPI) Programmer's Reference
// Warning: Only a single CEC device can be select with this interface even though
//          the all 16 devices can be selected.
//------------------------------------------------------------------------------
//
u8 cec_capture_id_set(u8 logical_address)
{
    u8 capture_address[2];
    u8 capture_addr_sel = 0x01;

    capture_address[0] = 0;
    capture_address[1] = 0;
    if (logical_address < 8) {
        capture_addr_sel = capture_addr_sel << logical_address;
        capture_address[0] = capture_addr_sel;
    } else {
        capture_addr_sel   = capture_addr_sel << (logical_address - 8);
        capture_address[1] = capture_addr_sel;
    }

    // Set Capture Address, 0x00 and 0x0f will be capture
    cec_register_write_block(REG_CEC_CAPTURE_ID0,capture_address,2);
    cec_register_write(REG_CEC_TX_INIT, logical_address);

    return 0;
}

//------------------------------------------------------------------------------
// Function Name: cec_send_ping
// Function Description: This command intiate sending a ping, and used for checking available
//                       CEC devices
//
// Accepts: cec_logical_addr
// Returns: none
// Globals: none
//------------------------------------------------------------------------------
void cec_send_ping (u8 cec_logical_addr)
{
    cec_register_write(REG_CEC_TX_DEST, BIT_SEND_POLL | cec_logical_addr);
}

//------------------------------------------------------------------------------
// Function Name: cec_set_command
// Function Description: This command sets data for CEC transmission
//
// Accepts: pcec_cmd
// Returns: 1 if error; 0 if no error.
// Globals: none
//------------------------------------------------------------------------------
u8 cec_set_command(hdmi_cec_cmd * pcec_cmd)
{
  u8 error = 0;
  u8 cec_int_status_reg[2];

  // Clear Tx Buffer
  cec_register_modify(REG_CEC_DEBUG_3, BIT_FLUSH_TX_FIFO, BIT_FLUSH_TX_FIFO);

  cec_log("\n TX: dest[0x%02X],OpCode[0x%02X], Count:%d, Oprand[0x%02X, %02X, %02X]\n",
      (int)pcec_cmd->destorrx_header,
      (int)pcec_cmd->cec_opcode,
      (int)pcec_cmd->cec_count,
      (int)pcec_cmd->cec_operand[0],
      (int)pcec_cmd->cec_operand[1],
      (int)pcec_cmd->cec_operand[2]);

    // Clear Tx-related buffers; write 1 to bits to be clear directly; writing 0 has no effect on the status bit
    cec_int_status_reg[0] = 0x64 ; // Clear Tx Transmit Buffer Full Bit, Tx msg Sent Event Bit, and Tx FIFO Empty Event Bit
    cec_int_status_reg[1] = 0x02 ; // Clear Tx Frame Retranmit Count Exceeded Bit.
    cec_register_write_block(REG_CEC_INT_STATUS_0, cec_int_status_reg, 2);

    // Write Source and Destination address
    cec_register_write(REG_CEC_TX_DEST,pcec_cmd->destorrx_header);

    // Send CEC Opcode AND up to 15 Operands
    cec_register_write(REG_CEC_TX_COMMAND, pcec_cmd->cec_opcode);
    cec_register_write_block(REG_CEC_TX_OPERANDS_0, pcec_cmd->cec_operand, pcec_cmd->cec_count);

    if (error) {
        cec_loge("\n cec_set_command(): Fail to write CEC opcode and operands\n") ;
    }

    // Write Operand count and activate send
    cec_register_write(REG_CEC_TRANSMIT_DATA, BIT_TRANSMIT_CMD | pcec_cmd->cec_count);

    return error;
}

//------------------------------------------------------------------------------
// Function Name: cec_get_command
// Function Description: This function gets data from CEC Reception
//
// Accepts: pcec_cmd
// Returns: none
// Globals: none
//------------------------------------------------------------------------------
u8 cec_get_command(hdmi_cec_cmd * pcec_cmd)
{
    u8 error = 0;
    u8 new_rx_state = 0;
    u8 cec_count;

    cec_count = pcec_cmd->cec_count  & 0x0f; // extract u8 counter, ignore frame counter

    if (!(pcec_cmd->cec_count & BIT_MSG_ERROR)) {
        pcec_cmd->destorrx_header = cec_register_read(REG_CEC_RX_CMD_HEADER);
        pcec_cmd->cec_opcode = cec_register_read(CEC_RX_COMMAND);
        cec_register_read_block(REG_CEC_RX_OPERAND_0, pcec_cmd->cec_operand, cec_count);

        cec_log("\n RX: HDR[0x%02X],OpCode[0x%02X], Count:%d, Oprand[0x%02X, %02X, %02X]\n",
          (int)pcec_cmd->destorrx_header,
          (int)pcec_cmd->cec_opcode,
          (int)cec_count,
          (int)pcec_cmd->cec_operand[0],
          (int)pcec_cmd->cec_operand[1],
          (int)pcec_cmd->cec_operand[2]);
    } else {
        error = 1;
    }
    // Clear CLR_RX_FIFO_CUR;
    // Clear current frame from Rx FIFO
    cec_register_modify(REG_CEC_RX_CONTROL, BIT_CLR_RX_FIFO_CUR, BIT_CLR_RX_FIFO_CUR);

    // Check if more frame in Rx FIFO, if yes get u8 count of next frame.
    pcec_cmd->rx_next_count = 0;

    new_rx_state = cec_register_read(REG_CEC_RX_COUNT);
    cec_logi("Nex Status:0x%x\n", new_rx_state);
    //if (pcec_cmd->cec_count & 0xF0)
    if (new_rx_state & 0xF0) {
        pcec_cmd->rx_next_count = cec_register_read(REG_CEC_RX_COUNT);
    }

    return error;
}

//------------------------------------------------------------------------------
// Function Name: K3_HDMI_CEC_IntProcessing
// Function Description: This function is called on interrupt events
//                       it makes interrut service
// Accepts: cec_int * pInt
// Returns: none
// Globals: none
//------------------------------------------------------------------------------
u8 cec_isr_processing (cec_int * pInt)
{
    u8 error = 0;
    u8 cec_int_status_reg[2] = {0};
    u8 tmp_rx_state = 0;

    // Get Interrupts
    pInt->tx_state   = 0;
    pInt->cec_errors = 0;
    pInt->rx_state   = 0;
    //Get Interrupt status
    cec_register_read_block(REG_CEC_INT_STATUS_0, cec_int_status_reg, 2);
    tmp_rx_state = cec_register_read(REG_CEC_RX_COUNT); //RX Count Status will be ignore?

    // Poll Interrupt and RxStatus
    if (((cec_int_status_reg[0] & 0x7F) || cec_int_status_reg[1]) || (tmp_rx_state & 0x70)) {
        cec_logi("\nA6A7Reg: %02X %02X\n",
            (int) cec_int_status_reg[0],
            (int) cec_int_status_reg[1]);

        if (cec_int_status_reg[1] & BIT_FRAME_RETRANSM_OV) {
           // Frame Retransmit Count Exceeded event
           cec_log("\n!CEC_A7_TX_RETRY_EXCEEDED![%02X][%02X]\n",
               (int) cec_int_status_reg[0],
               (int) cec_int_status_reg[1]);

            // flash TX otherwise after writing clear interrupt
            // BIT_FRAME_RETRANSM_OV the TX command will be re-send
           cec_register_modify(REG_CEC_DEBUG_3,BIT_FLUSH_TX_FIFO, BIT_FLUSH_TX_FIFO);
        }
        //
        // Clear set bits that are set, remove realtive interrupt bit
        //
        cec_register_write_block(REG_CEC_INT_STATUS_0,cec_int_status_reg,2);

        cec_logi("\nA6A7Reg: %02X %02X\n", (int) cec_int_status_reg[0], (int) cec_int_status_reg[1]);

        msleep(10);//Very Important, Must Add delay!
        // CEC RX Processing
        if ((cec_int_status_reg[0] & BIT_RX_MSG_RECEIVED) || (tmp_rx_state & 0x70)) {
            if (tmp_rx_state & 0x70) {
                if (! (cec_int_status_reg[0] & BIT_RX_MSG_RECEIVED)) {
                    cec_log("Interrupt CEC Rx Status do not set! [%02X][%02X], tmp_rx_state:[%02X]\n",
                        (int) cec_int_status_reg[0], (int) cec_int_status_reg[1], tmp_rx_state);
                }
            }
            // Save number of frames in Rx Buffer
            pInt->rx_state = cec_register_read(REG_CEC_RX_COUNT);
            cec_log("Receive Frame pInt->rx_state:0x%x\n", pInt->rx_state);
        }

        // RX Errors processing
        if (cec_int_status_reg[1] & BIT_SHORT_PULSE_DET) {
            pInt->cec_errors |= K3_HDMI_CEC_ERROR_SHORTPLUSEDETECT;
        }

        if (cec_int_status_reg[1] & BIT_START_IRREGULAR) {
            pInt->cec_errors |= K3_HDMI_CEC_ERROR_STARTBITIRREGULAR;
        }

        if (cec_int_status_reg[1] & BIT_RX_FIFO_OVERRUN) {// fixed per Uematsu san
            cec_log("CEC RX Overflow, Flush Rx FIFO\n");
            pInt->cec_errors |= K3_HDMI_CEC_ERROR_RXOVERFLOW;
            // Clear All frame from Rx FIFO
            cec_register_modify(REG_CEC_RX_CONTROL, BIT_CLR_RX_FIFO_ALL, BIT_CLR_RX_FIFO_ALL);
        }

        // TX Processing
        if (cec_int_status_reg[0] & BIT_TX_FIFO_EMPTY) {    //0x04
            pInt->tx_state = K3_HDMI_CEC_TX_WAITCMD;
        }

        if (cec_int_status_reg[0] & BIT_TX_MESSAGE_SENT) {  //0x20
            pInt->tx_state = K3_HDMI_CEC_TX_SENDACKED;
        }

        if (cec_int_status_reg[1] & BIT_FRAME_RETRANSM_OV) {  //0x02
            pInt->tx_state = K3_HDMI_CEC_TX_SENDFAILURE;
        }
    }

    return error;
}

/*----------------------------------END-----------------------------*/

//------------------------------------------------------------------------------
// Function Name:
// Function Description:
//------------------------------------------------------------------------------
void cec_print_logicaddr (u8 logic_addr)
{

    if (logic_addr <= K3_HDMI_CEC_LOGICADDR_Unregister) {
        switch (logic_addr) {
            case K3_HDMI_CEC_LOGICADDR_TV:
                cec_msg("TV");           break;
            case K3_HDMI_CEC_LOGICADDR_RecDev1:
                cec_msg("RecDev1");      break;
            case K3_HDMI_CEC_LOGICADDR_RecDev2:
                cec_msg("RecDev2");      break;
            case K3_HDMI_CEC_LOGICADDR_STB1:
                cec_msg("STB1");         break;
            case K3_HDMI_CEC_LOGICADDR_DVD1:
                cec_msg("DVD1");         break;
            case K3_HDMI_CEC_LOGICADDR_AudSys:
                cec_msg("AudSys");       break;
            case K3_HDMI_CEC_LOGICADDR_STB2:
                cec_msg("STB2");         break;
            case K3_HDMI_CEC_LOGICADDR_STB3:
                cec_msg("STB3");         break;
            case K3_HDMI_CEC_LOGICADDR_DVD2:
                cec_msg("DVD2");         break;
            case K3_HDMI_CEC_LOGICADDR_RecDev3:
                cec_msg("RecDev3");      break;
            case K3_HDMI_CEC_LOGICADDR_Res1:
                cec_msg("Res1");         break;
            case K3_HDMI_CEC_LOGICADDR_Res2:
                cec_msg("Res2");         break;
            case K3_HDMI_CEC_LOGICADDR_Res3:
                cec_msg("Res3");         break;
            case K3_HDMI_CEC_LOGICADDR_Res4:
                cec_msg("Res4");         break;
            case K3_HDMI_CEC_LOGICADDR_FreeUse:
                cec_msg("FreeUse");      break;
            case K3_HDMI_CEC_LOGICADDR_Unregister:
                cec_msg("BrocastMSG");    break;
        }
        cec_msg ("(%02X)", (int) logic_addr);
    }
}

static char* cec_opcode_str(int opcode)
{
    switch(opcode) {
        case CEC_OPCODE_FEATURE_ABORT                  : return "FEATURE_ABORT";
        case CEC_OPCODE_ABORT_MESSAGE                  : return "ABORT_MESSAGE";
        case CEC_OPCODE_ACTIVE_SOURCE                  : return "ACTIVE_SOURCE";
        case CEC_OPCODE_IMAGE_VIEW_ON                  : return "IMAGE_VIEW_ON";
        case CEC_OPCODE_TEXT_VIEW_ON                   : return "TEXT_VIEW_ON";
        case CEC_OPCODE_INACTIVE_SOURCE                : return "INACTIVE_SOURCE";
        case CEC_OPCODE_REQUEST_ACTIVE_SOURCE          : return "REQUEST_ACTIVE_SOURCE";
        case CEC_OPCODE_ROUTING_CHANGE                 : return "ROUTING_CHANGE";
        case CEC_OPCODE_ROUTING_INFORMATION            : return "ROUTING_INFORMATION";
        case CEC_OPCODE_SET_STREAM_PATH                : return "SET_STREAM_PATH";
        case CEC_OPCODE_STANDBY                        : return "STANDBY";
        case CEC_OPCODE_RECORD_OFF                     : return "RECORD_OFF";
        case CEC_OPCODE_RECORD_ON                      : return "RECORD_ON";
        case CEC_OPCODE_RECORD_STATUS                  : return "RECORD_STATUS";
        case CEC_OPCODE_RECORD_TV_SCREEN               : return "RECORD_TV_SCREEN";
        case CEC_OPCODE_CLEAR_ANALOGUE_TIMER           : return "CLEAR_ANALOGUE_TIMER";
        case CEC_OPCODE_CLEAR_DIGITAL_TIMER            : return "CLEAR_DIGITAL_TIMER";
        case CEC_OPCODE_CLEAR_EXTERNAL_TIMER           : return "CLEAR_EXTERNAL_TIMER";
        case CEC_OPCODE_SET_ANALOGUE_TIMER             : return "SET_ANALOGUE_TIMER";
        case CEC_OPCODE_SET_DIGITAL_TIMER              : return "SET_DIGITAL_TIMER";
        case CEC_OPCODE_SET_EXTERNAL_TIMER             : return "SET_EXTERNAL_TIMER";
        case CEC_OPCODE_SET_TIMER_PROGRAM_TITLE        : return "SET_TIMER_PROGRAM_TITLE";
        case CEC_OPCODE_TIMER_CLEARED_STATUS           : return "TIMER_CLEARED_STATUS";
        case CEC_OPCODE_TIMER_STATUS                   : return "TIMER_STATUS";
        case CEC_OPCODE_CEC_VERSION                    : return "CEC_VERSION";
        case CEC_OPCODE_GET_CEC_VERSION                : return "GET_CEC_VERSION";
        case CEC_OPCODE_GIVE_PHYSICAL_ADDRESS          : return "GIVE_PHYSICAL_ADDRESS";
        case CEC_OPCODE_REPORT_PHYSICAL_ADDRESS        : return "REPORT_PHYSICAL_ADDRESS";
        case CEC_OPCODE_GET_MENU_LANGUAGE              : return "GET_MENU_LANGUAGE";
        case CEC_OPCODE_SET_MENU_LANGUAGE              : return "SET_MENU_LANGUAGE";
        case CEC_OPCODE_DECK_CONTROL                   : return "DECK_CONTROL";
        case CEC_OPCODE_DECK_STATUS                    : return "DECK_STATUS";
        case CEC_OPCODE_GIVE_DECK_STATUS               : return "GIVE_DECK_STATUS";
        case CEC_OPCODE_PLAY                           : return "PLAY";
        case CEC_OPCODE_GIVE_TUNER_DEVICE_STATUS       : return "GIVE_TUNER_DEVICE_STATUS";
        case CEC_OPCODE_SELECT_ANALOGUE_SERVICE        : return "SELECT_ANALOGUE_SERVICE";
        case CEC_OPCODE_SELECT_DIGITAL_SERVICE         : return "SELECT_DIGITAL_SERVICE";
        case CEC_OPCODE_TUNER_DEVICE_STATUS            : return "TUNER_DEVICE_STATUS";
        case CEC_OPCODE_TUNER_STEP_DECREMENT           : return "TUNER_STEP_DECREMENT";
        case CEC_OPCODE_TUNER_STEP_INCREMENT           : return "TUNER_STEP_INCREMENT";
        case CEC_OPCODE_DEVICE_VENDOR_ID               : return "DEVICE_VENDOR_ID";
        case CEC_OPCODE_GIVE_DEVICE_VENDOR_ID          : return "GIVE_DEVICE_VENDOR_ID";
        case CEC_OPCODE_VENDOR_COMMAND                 : return "VENDOR_COMMAND";
        case CEC_OPCODE_VENDOR_COMMAND_WITH_ID         : return "VENDOR_COMMAND_WITH_ID";
        case CEC_OPCODE_VENDOR_REMOTE_BUTTON_DOWN      : return "VENDOR_REMOTE_BUTTON_DOWN";
        case CEC_OPCODE_VENDOR_REMOTE_BUTTON_UP        : return "VENDOR_REMOTE_BUTTON_UP";
        case CEC_OPCODE_SET_OSD_STRING                 : return "SET_OSD_STRING";
        case CEC_OPCODE_GIVE_OSD_NAME                  : return "GIVE_OSD_NAME";
        case CEC_OPCODE_SET_OSD_NAME                   : return "SET_OSD_NAME";
        case CEC_OPCODE_MENU_REQUEST                   : return "MENU_REQUEST";
        case CEC_OPCODE_MENU_STATUS                    : return "MENU_STATUS";
        case CEC_OPCODE_USER_CONTROL_PRESSED           : return "USER_CONTROL_PRESSED";
        case CEC_OPCODE_USER_CONTROL_RELEASED          : return "USER_CONTROL_RELEASED";
        case CEC_OPCODE_GIVE_DEVICE_POWER_STATUS       : return "GIVE_DEVICE_POWER_STATUS";
        case CEC_OPCODE_REPORT_POWER_STATUS            : return "REPORT_POWER_STATUS";
        case CEC_OPCODE_GIVE_AUDIO_STATUS              : return "GIVE_AUDIO_STATUS";
        case CEC_OPCODE_GIVE_SYSTEM_AUDIO_MODE_STATUS  : return "GIVE_SYSTEM_AUDIO_MODE_ST";
        case CEC_OPCODE_REPORT_AUDIO_STATUS            : return "REPORT_AUDIO_STATUS";
        case CEC_OPCODE_SET_SYSTEM_AUDIO_MODE          : return "SET_SYSTEM_AUDIO_MODE";
        case CEC_OPCODE_SYSTEM_AUDIO_MODE_REQUEST      : return "SYSTEM_AUDIO_MODE_REQUEST";
        case CEC_OPCODE_SYSTEM_AUDIO_MODE_STATUS       : return "SYSTEM_AUDIO_MODE_STATUS";
        case CEC_OPCODE_SET_AUDIO_RATE                 : return "SET_AUDIO_RATE";
        case CEC_OPCODE_POLLING_MESSAGE                : return "POLLING_MESSAGE";
        default: return "ERROR CODE";
    }
}

//------------------------------------------------------------------------------
// Function Name: cec_print_command
// Function Description:
//------------------------------------------------------------------------------
static void cec_print_command(hdmi_cec_cmd * cec_cmd)
{
    u8 i;
    cec_msg("\n\033[1;33m");
    cec_msg("From ");
    cec_print_logicaddr((cec_cmd->destorrx_header & 0xF0) >> 4);
    cec_msg(" To ");
    cec_print_logicaddr(cec_cmd->destorrx_header & 0x0F);
    cec_msg(" Opcode: ");
    cec_msg("%s", cec_opcode_str((int) cec_cmd->cec_opcode));
    cec_msg("[0x%02X]", (int) cec_cmd->cec_opcode);
    //cec_msg("(%02X) ", (int)(cec_cmd->cec_count & 0x0F));
    if (cec_cmd->cec_count & 0x0F) {
        cec_msg ("{");
        for (i = 0; i < (cec_cmd->cec_count & 0x0F); i++) {
            cec_msg("%02X ", (int) cec_cmd->cec_operand[i]);
        }
        cec_msg("}(");
        for (i = 0; i < (cec_cmd->cec_count & 0x0F); i++) {
            cec_msg("%c", (int) cec_cmd->cec_operand[i]);
        }
        cec_msg(")");
    }
    cec_msg("\033[0m\n");
}


static u32 cec_rx_msg_operation(u8 init, u8 dest, u8 opcode, u8 *p_operand, u8 operand_leangth, hdmi_cec_cmd *cec_frame)
{
    u32 ret = K3_HDMI_CEC_MESSAGE_RETURN_ERROR_NOSUPPORT;//Default Return is Error NoSupprot!
    u32 rec = 0;
    int index = 0;
    u8 phy_addr[4] = {0};
    u8 logic_addr = 0;

    switch(opcode) {
    /*General Protocol messages*/
    case CEC_OPCODE_FEATURE_ABORT:                   //0X00
        cec_log("FEATURE ABORT init:%d, dest:%d, opcode:0x%x\n", init, dest, opcode);
        ret = K3_HDMI_CEC_MESSAGE_RETURN_OK_WITHOUT_RESPONES; //need not response
        break;
    case CEC_OPCODE_ABORT_MESSAGE:                   //0XFF
        cec_log("ABORT MESSAGE\n");
        ret = K3_HDMI_CEC_MESSAGE_RETURN_OK_WITHOUT_RESPONES; //need not response
        break;

    /*One Touch Play Feature*/
    case CEC_OPCODE_ACTIVE_SOURCE:                   //0X82
        cec_log("ACTIVE_SOURCE\n");
        cec_log("It should be source cmd, ignore!\n");
        break;
    case CEC_OPCODE_IMAGE_VIEW_ON:                   //0X04
        cec_log("IMAGE_VIEW_ON\n");
        cec_log("It should be source cmd, ignore!\n");
        break;
    case CEC_OPCODE_TEXT_VIEW_ON:                    //0X0D
        cec_log("TEXT_VIEW_ON\n");
        cec_log("It should be source cmd, ignore!\n");
        break;

    /*Routing Control Feature*/
    case CEC_OPCODE_INACTIVE_SOURCE:                 //0X9D
        cec_log("INACTIVE_SOURCE, No supprot\n");
        ret = K3_HDMI_CEC_MESSAGE_RETURN_ERROR_NOSUPPORT;
        break;
    case CEC_OPCODE_REQUEST_ACTIVE_SOURCE:        //0X85
    //When a device(TV) comes out of the standy state, it maybe broacast a
    //<Request Active Source> message to discover active source.
    //the active source device shall respond by broadcasting an <Active Source> message.
        rec = cec_get_address((u8*)phy_addr, &logic_addr);
        if (rec != HI_SUCCESS) {
            break;
        }
        cec_log("REQUEST_ACTIVE_SOURCE, return Active Source\n");
        //we should send back a message of ACTIVE_SOURCE
        cec_frame->cec_opcode   = CEC_OPCODE_ACTIVE_SOURCE;
        cec_frame->destorrx_header = init;
        cec_frame->cec_count          = 2;
        cec_frame->cec_operand[0]     = ((phy_addr[0] << 4) & 0xf0) | (phy_addr[1] & 0x0f); // [Physical Address(A.B.C.D):A B]
        cec_frame->cec_operand[1]     = ((phy_addr[2] << 4) & 0xf0) | (phy_addr[3] & 0x0f) ; // [Physical Address(A.B.C.D):C D]
        ret = K3_HDMI_CEC_MESSAGE_RETURN_OK_WITH_RESPONES;
        break;
    case CEC_OPCODE_ROUTING_CHANGE:                  //0X80
        cec_log("ROUTING_CHANGE, it should be brocast message\n");
        break;
    case CEC_OPCODE_ROUTING_INFORMATION:             //0X81
        cec_log("ROUTING_INFORMATION, it should be brocast message\n");
        break;
    case CEC_OPCODE_SET_STREAM_PATH:                 //0X86
    //the <Set Stream Path> is sent by the TV to the source device
    //to request it to broacst its path using an <Active Source> message.
        cec_log("SET_STREAM_PATH\n");
        rec = cec_get_address(phy_addr, &logic_addr);
        if (rec != HI_SUCCESS) {
            break;
        }
        cec_log("REQUEST_ACTIVE_SOURCE, return Active Source\n");
        //we should send back a message of ACTIVE_SOURCE
        cec_frame->cec_opcode   = CEC_OPCODE_ACTIVE_SOURCE;
        cec_frame->destorrx_header = init;
        cec_frame->cec_count          = 3;
        cec_frame->cec_operand[0]     = ((phy_addr[0] << 4) & 0xf0) | (phy_addr[1] & 0x0f); // [Physical Address(A.B.C.D):A B]
        cec_frame->cec_operand[1]     = ((phy_addr[2] << 4) & 0xf0) | (phy_addr[3] & 0x0f) ; // [Physical Address(A.B.C.D):C D]
        cec_frame->cec_operand[2]     = logic_addr ; // [Device Type] = 3 Tuner1
        ret = K3_HDMI_CEC_MESSAGE_RETURN_OK_WITH_RESPONES;
        break;
    /*Standby Feature*/
    case CEC_OPCODE_STANDBY:                         //0X36
        cec_log("STANDBY! It should brocast message\n");
        break;
    /*One Touch Record Feature*/
    case CEC_OPCODE_RECORD_OFF:                      //0X0B
        break;
    case CEC_OPCODE_RECORD_ON:                       //0X09
        break;
    case CEC_OPCODE_RECORD_STATUS:                   //0X0A
        break;
    case CEC_OPCODE_RECORD_TV_SCREEN:                //0X0F
        break;
    /*Timer Programming Feature*/
    case CEC_OPCODE_CLEAR_ANALOGUE_TIMER:            //0X33
        break;
    case CEC_OPCODE_CLEAR_DIGITAL_TIMER:             //0X99
        break;
    case CEC_OPCODE_CLEAR_EXTERNAL_TIMER:            //0XA1
        break;
    case CEC_OPCODE_SET_ANALOGUE_TIMER:              //0X34
        break;
    case CEC_OPCODE_SET_DIGITAL_TIMER:               //0X97
        break;
    case CEC_OPCODE_SET_EXTERNAL_TIMER:              //0XA2
        break;
    case CEC_OPCODE_SET_TIMER_PROGRAM_TITLE:         //0X67
        break;
    case CEC_OPCODE_TIMER_CLEARED_STATUS:            //0X43
        break;
    case CEC_OPCODE_TIMER_STATUS:                    //0X35
        break;

    /*System Information Feature*/
    case CEC_OPCODE_CEC_VERSION:                     //0X9E
        cec_log("CEC_VERSION:\n");
        for(index = 0; index < operand_leangth; index ++) {
            if (((p_operand[index] >= 'a') && (p_operand[index] <= 'z'))
             || ((p_operand[index] >= 'A') && (p_operand[index] <= 'Z'))) {
                cec_log("%c", p_operand[index]);
            } else {
                cec_log("%02x", p_operand[index]);
            }
        }
        cec_log("\n");
        break;
    case CEC_OPCODE_GET_CEC_VERSION:                 //0X9F
        cec_log("GET_CEC_VERSION, Response\n");
        //we should send back a message of ACTIVE_SOURCE
        cec_frame->cec_opcode   = CEC_OPCODE_CEC_VERSION;
        cec_frame->destorrx_header = init;
        cec_frame->cec_count          = 3;
        cec_frame->cec_operand[0]     = '1';
        cec_frame->cec_operand[1]     = '3' ;
        cec_frame->cec_operand[2]     = 'a' ;

        ret = K3_HDMI_CEC_MESSAGE_RETURN_OK_WITH_RESPONES;
        break;
    case CEC_OPCODE_GIVE_PHYSICAL_ADDRESS:           //0X83
        cec_log("GIVE_PHYSICAL_ADDRESS, Response\n");
        rec = cec_get_address(phy_addr, &logic_addr);

        if (rec != HI_SUCCESS)
        {
            break;
        }
        cec_log("return REPORT_PHYSICAL_ADDRESS\n");
        //we should send back a message of ACTIVE_SOURCE
        cec_frame->cec_opcode   = CEC_OPCODE_REPORT_PHYSICAL_ADDRESS;
        cec_frame->destorrx_header = init;
        cec_frame->cec_count          = 3;
        cec_frame->cec_operand[0]     = ((phy_addr[0] << 4) & 0xf0) | (phy_addr[1] & 0x0f); // [Physical Address(A.B.C.D):A B]
        cec_frame->cec_operand[1]     = ((phy_addr[2] << 4) & 0xf0) | (phy_addr[3] & 0x0f) ; // [Physical Address(A.B.C.D):C D]
        cec_frame->cec_operand[2]     = logic_addr;
        ret = K3_HDMI_CEC_MESSAGE_RETURN_OK_WITH_RESPONES;
        break;
    case CEC_OPCODE_REPORT_PHYSICAL_ADDRESS:         //0X84
        cec_log("REPORT_PHYSICAL_ADDRESS\n");
        if (operand_leangth != 3) {
            cec_log("Error oprand length:%d\n", operand_leangth);
        }
        cec_log("PhysicalAddr:%01d.%01d.%01d.%01d, LogicalAddr:%d\n", (p_operand[0] & 0xf0) >> 4, (p_operand[0] & 0x0f), \
         (p_operand[1] & 0xf0) >> 4, (p_operand[1] & 0x0f), p_operand[2]);

        ret = K3_HDMI_CEC_MESSAGE_RETURN_OK_WITHOUT_RESPONES;
        break;
    case CEC_OPCODE_GET_MENU_LANGUAGE:               //0X91
        cec_log("GET_MENU_LANGUAGE, response\n");
        cec_frame->cec_opcode   = CEC_OPCODE_SET_MENU_LANGUAGE;
        cec_frame->destorrx_header = init;
        cec_frame->cec_count          = 3;
        //Language: 3 ASCII Bytes as defined in ISO/FDIS 639-2
        cec_frame->cec_operand[0]     = 'c';
        cec_frame->cec_operand[1]     = 'h' ;
        cec_frame->cec_operand[2]     = 'i' ;

        ret = K3_HDMI_CEC_MESSAGE_RETURN_OK_WITH_RESPONES;
        break;
    case CEC_OPCODE_SET_MENU_LANGUAGE:               //0X32
        cec_log("SET_MENU_LANGUAGE:");
        for(index = 0; index < operand_leangth; index ++) {
            if (((p_operand[index] >= 'a') && (p_operand[index] <= 'z'))
             || ((p_operand[index] >= 'A') && (p_operand[index] <= 'Z'))) {
                cec_log("%c", p_operand[index]);
            } else {
                cec_log("%02x", p_operand[index]);
            }
        }
        cec_log("\n");
        break;

    /*Deck Control Feature*/
    case CEC_OPCODE_DECK_CONTROL:                    //0X42
        cec_log("DECK_CONTROL\n");
        break;
    case CEC_OPCODE_DECK_STATUS:                     //0X1B
        cec_log("DECK_STATUS\n");
        break;
    case CEC_OPCODE_GIVE_DECK_STATUS:                //0X1A
        cec_log("GIVE_DECK_STATUS\n");
        break;
    case CEC_OPCODE_PLAY:                            //0X41
        cec_log("PLAY\n");
        break;

    /*Tuner Control Feature*/
    case CEC_OPCODE_GIVE_TUNER_DEVICE_STATUS:        //0X08
        break;
    case CEC_OPCODE_SELECT_ANALOGUE_SERVICE:         //0X92
        break;
    case CEC_OPCODE_SELECT_DIGITAL_SERVICE:          //0X93
        break;
    case CEC_OPCODE_TUNER_DEVICE_STATUS:             //0X07
        break;
    case CEC_OPCODE_TUNER_STEP_DECREMENT:            //0X06
        break;
    case CEC_OPCODE_TUNER_STEP_INCREMENT:            //0X05
        break;
    /*Vendor Specific Command*/
    case CEC_OPCODE_DEVICE_VENDOR_ID:                //0X87
        cec_log("DEVICE_VENDOR_ID:");

        for(index = 0; index < operand_leangth; index ++) {
            cec_log("%02x ", p_operand[index]);
        }

        cec_log("\n");
        break;
    case CEC_OPCODE_GIVE_DEVICE_VENDOR_ID:           //0X8C
        cec_log("GIVE_DEVICE_VENDOR_ID, Response\n");
        cec_frame->cec_opcode   = CEC_OPCODE_DEVICE_VENDOR_ID;
        cec_frame->destorrx_header = 0x0f;
        cec_frame->cec_count          = 3;
        cec_frame->cec_operand[0]     = 'h';
        cec_frame->cec_operand[1]     = 'i' ;
        cec_frame->cec_operand[2]     = 's' ;

        ret = K3_HDMI_CEC_MESSAGE_RETURN_OK_WITH_RESPONES;
        break;
    case CEC_OPCODE_VENDOR_COMMAND:                  //0X89
        break;
    case CEC_OPCODE_VENDOR_COMMAND_WITH_ID:          //0XA0
        break;
    case CEC_OPCODE_VENDOR_REMOTE_BUTTON_DOWN:       //0X8A
        break;
    case CEC_OPCODE_VENDOR_REMOTE_BUTTON_UP:         //0X8B
        break;

    /*OSD Display Feature*/
    case CEC_OPCODE_SET_OSD_STRING:                  //0X64
        cec_log("SET_OSD_NAME:%s\n", p_operand);
        ret = K3_HDMI_CEC_MESSAGE_RETURN_OK_WITHOUT_RESPONES;
        break;
    case CEC_OPCODE_GIVE_OSD_NAME:                   //0X46
        cec_log("GIVE_DEVICE_VENDOR_ID, Response\n");
        cec_frame->cec_opcode   = CEC_OPCODE_SET_OSD_STRING;
        cec_frame->destorrx_header = init;
        cec_frame->cec_count          = 3;
        cec_frame->cec_operand[0]     = 's';
        cec_frame->cec_operand[1]     = 't' ;
        cec_frame->cec_operand[2]     = 'b' ;

        ret = K3_HDMI_CEC_MESSAGE_RETURN_OK_WITH_RESPONES;
        break;
    case CEC_OPCODE_SET_OSD_NAME:                    //0X47
        cec_log("SET_OSD_NAME:");

        for(index = 0; index < operand_leangth; index ++) {
            if (((p_operand[index] >= 'a') && (p_operand[index] <= 'z'))
             || ((p_operand[index] >= 'A') && (p_operand[index] <= 'Z'))) {
                cec_log("%c", p_operand[index]);
            } else {
                cec_log("%02x", p_operand[index]);
            }
        }

        cec_log("\n");
        break;

    /*Device Menu Control Feature*/
    case CEC_OPCODE_MENU_REQUEST:                    //0X8D
        cec_log("MENU_REQUEST\n");
        if (operand_leangth < 1) {
            cec_log("Invalid operand_leangth:%d\n", operand_leangth);
            break;
        }

        // Send back Menu STATUS
        cec_frame->cec_opcode   = CEC_OPCODE_MENU_STATUS;
        cec_frame->destorrx_header = init;
        cec_frame->cec_count          = 1;

        if (p_operand[0] == 0x00) {
            cec_log("Active\n");
            cec_frame->cec_operand[0]     = 0X00;//Active
        } else if (p_operand[0] == 0x01) {
            cec_log("Deactive\n");
            cec_frame->cec_operand[0]     = 0X01;//Deactive
        } else if (p_operand[0] == 0x02) {
            cec_log("Query\n");
        } else {
            cec_log("Unknown, p_operand[0]:0x%x\n", p_operand[0]);
            break;
        }

        ret = K3_HDMI_CEC_MESSAGE_RETURN_OK_WITH_RESPONES;
        break;
    case CEC_OPCODE_MENU_STATUS:                     //0X8E
        cec_log("MENU_STATUS:\n");
        if (operand_leangth < 1) {
            cec_log("Invalid operand_leangth:%d\n", operand_leangth);
            break;
        }
        if (p_operand[0] == 0x00) {
            cec_log("Active\n");
        } else if (p_operand[0] == 0x01) {
            cec_log("Deactive\n");
        }

        ret = K3_HDMI_CEC_MESSAGE_RETURN_OK_WITHOUT_RESPONES;
        break;
    case CEC_OPCODE_USER_CONTROL_PRESSED:            //0X44
        cec_log("USER_CONTROL_PRESSED\n");
        cec_log("Press RC:0x%02x\n", p_operand[0]);
        ret = K3_HDMI_CEC_MESSAGE_RETURN_OK_WITHOUT_RESPONES;
        break;
    case CEC_OPCODE_USER_CONTROL_RELEASED:           //0X45
        cec_log("USER_CONTROL_RELEASED\n");
        cec_log("Release RC:0x%02x\n", p_operand[0]);
        ret = K3_HDMI_CEC_MESSAGE_RETURN_OK_WITHOUT_RESPONES;
        break;

    /*Power Status Feature*/
    case CEC_OPCODE_GIVE_DEVICE_POWER_STATUS:        //0X8F
        cec_log("GIVE_DEVICE_POWER_STATUS, response\n");
        cec_frame->cec_opcode   = CEC_OPCODE_SET_OSD_STRING;
        cec_frame->destorrx_header = init;
        cec_frame->cec_count          = 1;
        cec_frame->cec_operand[0]     = 0X00;//Power on

        ret = K3_HDMI_CEC_MESSAGE_RETURN_OK_WITH_RESPONES;
        break;
    case CEC_OPCODE_REPORT_POWER_STATUS:             //0X90
        cec_log("REPORT_POWER_STATUS\n");
        if (operand_leangth < 1) {
            cec_log("Invalid operand_leangth:%d\n", operand_leangth);
            break;
        }
        if (p_operand[0] == 0x00) {
            cec_log("Power On\n");
        } else if (p_operand[0] == 0x01) {
            cec_log("Stand By\n");
        } else {
            cec_log("Unknown, p_operand[0]:0x%x\n", p_operand[0]);
        }
        ret = K3_HDMI_CEC_MESSAGE_RETURN_OK_WITHOUT_RESPONES;
        break;

    /*System Audio Control Feature*/
    case CEC_OPCODE_GIVE_AUDIO_STATUS:               //0X71
        cec_log("GIVE_AUDIO_STATUS\n");
        break;
    case CEC_OPCODE_GIVE_SYSTEM_AUDIO_MODE_STATUS:   //0x7D
        break;
    case CEC_OPCODE_REPORT_AUDIO_STATUS:             //0X7A
        break;
    case CEC_OPCODE_SET_SYSTEM_AUDIO_MODE:           //0X72
        break;
    case CEC_OPCODE_SYSTEM_AUDIO_MODE_REQUEST:       //0X70
        break;
    case CEC_OPCODE_SYSTEM_AUDIO_MODE_STATUS:        //0X7E
        break;

    /*Audio Rate Control Feature*/
    case CEC_OPCODE_SET_AUDIO_RATE:                  //0X9A
        break;
    default:
        return 3;
    }
    return ret;
}

// Check received MSG validation.
// return 0 if msg is 0K, return 1 if msg is 1nva1id
static u8 cec_msg_validate(hdmi_cec_cmd * cec_cmd)
{
    u32 result = 0;
    hdmi_cec_cmd cec_frame;
    u8 cec_rx_init = 0;
    u8 cec_rx_dest = 0;

    u8 phy_addr[4];
    u8 logic_addr = 0;
    u32 rec;

    rec = cec_get_address(phy_addr, &logic_addr);

    if (rec != HI_SUCCESS) {
        return 1;
    }

    //Deal with CEC Rx message
    cec_rx_dest = (cec_cmd->destorrx_header & 0x0f);
    cec_rx_init = (cec_cmd->destorrx_header & 0xf0) >> 4;

    if (cec_rx_init == logic_addr) {
        cec_log("Selft Message, ignore\n");
        return 1;
    }
    if ((cec_rx_dest != logic_addr) && (cec_rx_dest != K3_HDMI_CEC_LOGICADDR_Unregister)) {
        cec_log("Message is not sent to us cec_rx_dest:%d\n", cec_rx_dest);
        return 1;
    }

    memset(&cec_frame, 0, sizeof(hdmi_cec_cmd));
    result = cec_rx_msg_operation(cec_rx_init, cec_rx_dest, cec_cmd->cec_opcode, cec_cmd->cec_operand, (cec_cmd->cec_count & 0x0F), &cec_frame);
    if (result == K3_HDMI_CEC_MESSAGE_RETURN_OK_WITH_RESPONES) {
        cec_log("response!\n");
        //z00222844 default is disable, I don't know why.
        cec_set_command(&cec_frame);
        return 0;
    } else if (result == K3_HDMI_CEC_MESSAGE_RETURN_OK_WITHOUT_RESPONES) {
        return 0;//direct return!
    } else if (result == K3_HDMI_CEC_MESSAGE_RETURN_ERROR_NOSUPPORT) {
        return 0;
    }

    //Deal with Unsupport Message!    //
    // Do not reply to Broadcast msgs, otherwise send Feature Abort
    // for all unsupported features.
    if ((cec_cmd->destorrx_header & 0x0F) != 0x0F) {
        /*All devices shall support the message <Feature Abort>.
          It is used to allow devices to indicate if they do not support an opcode that has been
          sent to them, if it is unable to deal with this message.
          <Feature Abort> has 2 parameters: opcode and the reason of its reject of the frame.

          <Feature Abort> is used as a response to any failure!
        */
        // Unsupported opcode; send feature abort
        //<Feature Abort>has 2 parameters, the opcode and a reason for its rejection of the frame.
        memset(&cec_frame, 0, sizeof(hdmi_cec_cmd));
        cec_frame.cec_opcode         = CEC_OPCODE_FEATURE_ABORT;//0x00;
        cec_frame.destorrx_header = (cec_cmd->destorrx_header & 0xf0) >> 4 ;
        cec_frame.cec_operand[0]     = cec_cmd->cec_opcode;
        cec_frame.cec_operand[1]     = 0x00; //Unrecognized opcode:0x00
        cec_frame.cec_count = 2;
        //z00222844 default is disable, I don't know why.
        cec_set_command(&cec_frame);
        cec_log("\n!Unsupported cec_opcode:0x%x Feature!\n", cec_cmd->cec_opcode);
    } else {
        // Unsupported Broadcast Msg
        cec_log("\n! Unsupported Brocast Msg!\n");
    }

    return 1;
}

void get_buf_status(void)
{
    unsigned int i = 0;
    if (g_cec_attr.cmd_pool_rptr == g_cec_attr.cmd_pool_wptr) {
        g_cec_attr.cmd_pool_rptr = 0;
        g_cec_attr.cmd_pool_wptr = 0;
        for(i=0; i < CEC_CMD_POOL_SIZE; i++)
            memset(&g_cec_attr.cec_cmd_pool[i], 0, sizeof(hdmi_cec_cmd));
    }
}

void store_cec_cmd(hdmi_cec_cmd *rx_cmd)
{
    get_buf_status();
    memcpy(&g_cec_attr.cec_cmd_pool[g_cec_attr.cmd_pool_wptr], rx_cmd, sizeof(hdmi_cec_cmd));

    if (g_cec_attr.cmd_pool_wptr == CEC_CMD_POOL_SIZE - 1) {
        g_cec_attr.cmd_pool_wptr = 0;
    } else {
        g_cec_attr.cmd_pool_wptr++;
    }

    if (g_cec_attr.cmd_pool_rptr == g_cec_attr.cmd_pool_wptr) {
        g_cec_attr.can_read_num = CEC_CMD_POOL_SIZE;
    } else if (g_cec_attr.cmd_pool_rptr <  g_cec_attr.cmd_pool_wptr) {
        g_cec_attr.can_read_num = g_cec_attr.cmd_pool_wptr - g_cec_attr.cmd_pool_rptr;
    } else {
        g_cec_attr.can_read_num = CEC_CMD_POOL_SIZE -(g_cec_attr.cmd_pool_rptr - g_cec_attr.cmd_pool_wptr);
    }
}

unsigned int  get_cec_cmd(hdmi_cec_param *rx_cmd, unsigned int num)
{
    unsigned int i = 0;
    if (g_cec_attr.get_cmd_counter == 0)
        init_waitqueue_head(&g_cec_attr.cec_wait_queue_head);

    /*if no cec ,delay 200 jiffies.*/
    wait_event_interruptible_timeout(g_cec_attr.cec_wait_queue_head, g_cec_attr.can_read_num > 0, 3*HZ);

    g_cec_attr.get_cmd_counter ++;

    if (g_cec_attr.can_read_num > 0) {
        rx_cmd[i].en_src_addr = g_cec_attr.cec_cmd_pool[g_cec_attr.cmd_pool_rptr].destorrx_header & 0xf0;
        rx_cmd[i].en_dst_addr = g_cec_attr.cec_cmd_pool[g_cec_attr.cmd_pool_rptr].destorrx_header & 0x0f;
        rx_cmd[i].opcode = g_cec_attr.cec_cmd_pool[g_cec_attr.cmd_pool_rptr].cec_opcode;
        rx_cmd[i].un_operand.raw_data.length = g_cec_attr.cec_cmd_pool[g_cec_attr.cmd_pool_rptr].cec_count;
        memcpy(&rx_cmd[i].un_operand.raw_data.data, g_cec_attr.cec_cmd_pool[g_cec_attr.cmd_pool_rptr].cec_operand, 15);

        if (g_cec_attr.cmd_pool_rptr == CEC_CMD_POOL_SIZE - 1) {
            g_cec_attr.cmd_pool_rptr = 0;
        } else {
            g_cec_attr.cmd_pool_rptr ++;
        }
        i = 1;
    }

    if (g_cec_attr.cmd_pool_rptr == g_cec_attr.cmd_pool_wptr) {
        g_cec_attr.can_read_num = 0;
    } else if (g_cec_attr.cmd_pool_rptr <  g_cec_attr.cmd_pool_wptr) {
        g_cec_attr.can_read_num = g_cec_attr.cmd_pool_wptr - g_cec_attr.cmd_pool_rptr;
    } else {
        g_cec_attr.can_read_num = CEC_CMD_POOL_SIZE -(g_cec_attr.cmd_pool_rptr - g_cec_attr.cmd_pool_wptr);
    }

    return  i;
}



//------------------------------------------------------------------------------
// Function Name: ParsingReceivedData
//
// Function Description:
// 1. CEC7.3 Frame Validation
// 2. Protocol Extension
// 3. CEC12.3 Feature Abort
// 4. Amber Alert i.e. call to function specified by the handler that is expecting
//    a reply from the specified device, LA, LogicalAddress.
//------------------------------------------------------------------------------
static void cec_parsing_received_data (u8 rx_state)
{
    u8 aux_data = 0;
    u8 i = 0;
    hdmi_cec_cmd cec_cmd;
    hdmi_cec_param rx_cmd;

    //
    // CEC_RX_COUNT Register:  RX_ERROR | CEC_RX_CMD_CNT | CEC_RX_byte_CNT
    // See CPI document for details.
    //
    aux_data = rx_state & 0xF0;
    if (aux_data) {
        memset(&cec_cmd, 0, sizeof(hdmi_cec_cmd));
        cec_cmd.cec_count = rx_state;
        aux_data >>=4;
        cec_log ("\n %i frames in RX FIFO\n", (int) aux_data);
        /* Get each Frame from RX FIFO */
        for (i = 0; i < aux_data; i++) {
            cec_log ("Deal with %i frames in RX FIFO\n", (int) aux_data);
            cec_get_command(&cec_cmd);

            if (((cec_cmd.destorrx_header & 0xf0) >> 4) == g_cec_attr.logical_addr) {
                cec_log("We should ignore this message, it is sent from ourself\n");
            } else {
                cec_print_command(&cec_cmd);
                // Check BC msg && init==0x00
                // Validation Message
                if (0 == cec_msg_validate(&cec_cmd)) {
                    #define CEC_CMD_STR_LEN 128
                    char cmd_str[CEC_CMD_STR_LEN] = {0};
                    memset(cmd_str, 0, CEC_CMD_STR_LEN);
                    //store_cec_cmd(&cec_cmd);
                    rx_cmd.en_src_addr = cec_cmd.destorrx_header & 0xf0;
                    rx_cmd.en_dst_addr = cec_cmd.destorrx_header & 0x0f;
                    rx_cmd.opcode = cec_cmd.cec_opcode;
                    rx_cmd.un_operand.raw_data.length = cec_cmd.cec_count & 0x0F;
                    memcpy(&rx_cmd.un_operand.raw_data.data, cec_cmd.cec_operand, 15);
                    cec_cmd_tostr(&rx_cmd, cmd_str);
                    hdmi_control_notify_cec_cmd(cmd_str);
                }
            }
            /* Get the next message */
            memset(&cec_cmd, 0, sizeof(hdmi_cec_cmd));
            if (cec_cmd.rx_next_count)
                cec_cmd.cec_count = cec_cmd.rx_next_count;
        }
    }
    return;
}

//------------------------------------------------------------------------------
// Function Name: userSWTtask
// Function Description:
// This is function handler event from the CEC RX buffer.
// CEC Software Connection #5.
//------------------------------------------------------------------------------


void hdmi_cec_event_handler(void)
{
    static u8 i = 0;
    cec_int cecint;
    int index = 0;

    if (g_cec_attr.new_task != g_cec_attr.cec_task) {
        g_cec_attr.new_task = g_cec_attr.cec_task;
        i = 0;
    }
    //cec_log("xxx ping g_cec_attr.tx_state:%d, K3_HDMI_CEC_TX_WAITCMD:%d\n", tx_state, K3_HDMI_CEC_TX_WAITCMD);
    if (g_cec_attr.cec_task) {
        //cec_log ("\nCEC_Event_Handler CECTask %x %x\n",             g_cec_attr.cec_task, g_cec_attr.tx_state);
    }

    // Process CEC Events; at this point doesn't do much
    cec_isr_processing(&cecint);

    if (g_cec_attr.cec_task == K3_HDMI_CEC_ReqPing) {
        cec_log("ping g_cec_attr.tx_state:%d, K3_HDMI_CEC_TX_WAITCMD:%d\n", g_cec_attr.tx_state, K3_HDMI_CEC_TX_WAITCMD);
        //
        // Test function 1: Ping Button Pressed
        //
        if (g_cec_attr.tx_state == K3_HDMI_CEC_TX_WAITCMD) {
            cec_log("request ping\n");
            cec_log("\n %X", (int)i);
            cec_send_ping(i);

            g_cec_attr.tx_state = K3_HDMI_CEC_TX_SENDING;
        } else if (g_cec_attr.tx_state == K3_HDMI_CEC_TX_SENDING) {
            if (cecint.tx_state == K3_HDMI_CEC_TX_SENDFAILURE) {
                g_cec_attr.result[i] = 0;

                cec_log (" NoAck \n");
                cec_print_logicaddr(i);
                i++;
                g_cec_attr.tx_state = K3_HDMI_CEC_TX_WAITCMD;
            }
            if (cecint.tx_state == K3_HDMI_CEC_TX_SENDACKED) {
                g_cec_attr.result[i] = 1;

                cec_log (" Ack \n"); cec_print_logicaddr(i);
                i++;
                g_cec_attr.tx_state = K3_HDMI_CEC_TX_WAITCMD;

            }
            if (i >= K3_HDMI_CEC_LOGICADDR_Unregister) {
                g_cec_attr.cec_task = K3_HDMI_CEC_Idle;
                cec_log("\nend of send command\n");

                for(index = 0;index < 16; index ++) {
                    cec_log("index:%d, result:%d\n", index, g_cec_attr.result[index]);
                }
                cec_log("end\n");

                i = 0;
            }
        }
    } else if (g_cec_attr.cec_task == K3_HDMI_CEC_ReqCmd1) {
        //  Test function 2: Send test message
        if (cecint.tx_state == K3_HDMI_CEC_TX_SENDFAILURE) {
            cec_log (" NoAck ");
            g_cec_attr.tx_state = K3_HDMI_CEC_TX_WAITCMD;
            g_cec_attr.cec_task = K3_HDMI_CEC_Idle;
        }
        if (cecint.tx_state == K3_HDMI_CEC_TX_SENDACKED) {
            cec_log (" Ack ");
            g_cec_attr.tx_state = K3_HDMI_CEC_TX_WAITCMD;
            g_cec_attr.cec_task = K3_HDMI_CEC_Idle;
        }
    }

    // Check for incoming CEC frames in the Rx Fifo.
    if (cecint.rx_state) {
        cec_parsing_received_data(cecint.rx_state);
    }
}

u32 hdmi_cec_send_command(hdmi_cec_param  *p_cec_cmd)
{
    hdmi_cec_cmd cec_frame;

    memset(&cec_frame, 0, sizeof(hdmi_cec_cmd));

    if (!g_cec_enable) {
        loge("Hdmi cec is not enable, do nothing.");
        return -1;
    }

    cec_register_write(REG_CEC_TX_INIT, g_cec_attr.logical_addr);

    if (p_cec_cmd->opcode == CEC_OPCODE_POLLING_MESSAGE) {
        cec_send_ping(p_cec_cmd->en_dst_addr);
        g_cec_attr.cec_task = K3_HDMI_CEC_ReqPing;

        cec_log("\nSend Ping cmd\n");
        return 0;
    }

    cec_frame.cec_opcode         = p_cec_cmd->opcode;
    cec_frame.destorrx_header = p_cec_cmd->en_dst_addr;
    cec_frame.cec_count          = p_cec_cmd->un_operand.raw_data.length;
    memcpy((cec_frame.cec_operand), (p_cec_cmd->un_operand.raw_data.data), p_cec_cmd->un_operand.raw_data.length);

    g_cec_attr.cec_task = K3_HDMI_CEC_ReqCmd1;

    cec_set_command(&cec_frame);

    return HI_SUCCESS;
}

u32 cec_auto_ping(u32 *pstatus)
{
    #define CEC_AUTO_PING_COUNT 10
    u32 status  = 0;
    u32 timeout = 0;
    u32 error   = HI_SUCCESS;
    bool thread_break = false;

    hdmi_sys_lock();
    if (hdmi_is_connect()) {
        /* CEC Set Up Register */
        cec_register_write(REG_CEC_CONFIG_CPI, 0x4);

        cec_register_write(REG_CEC_AUTO_PING_CTRL, 0x02); //0xcc 0xe0

        cec_register_write(REG_CEC_AUTO_PING_CTRL, 0x01);  //start cec ping
    }
    hdmi_sys_unlock();

    while(timeout ++ < CEC_AUTO_PING_COUNT) {
        msleep(1000);
        if (kthread_should_stop()) {
            thread_break = true;
            break;
        }
        hdmi_sys_lock();
        if (hdmi_is_connect()) {
            status = (u32)cec_register_read(REG_CEC_AUTO_PING_CTRL);

            logw("REG_CEC_AUTO_PING_CTRL:0x%x timeout:%d\n", status,timeout);
            if (0x80 == (status & 0x80))
            {
                hdmi_sys_unlock();
                cec_log("REG_CEC_AUTO_PING_CTRL:0x%x\n", status);
                break;
            }
        }
        hdmi_sys_unlock();
    }

    if (thread_break || timeout >= CEC_AUTO_PING_COUNT || (!hdmi_is_connect())) {
        cec_loge("AutoPing timeout or disconnected\n");
        return HI_FAILURE;
    }

    status = (u32)cec_register_read(REG_CEC_AUTO_PING_MAP0);          //0xcc 0xe1
    //cec_log("AUTO_PING_MAP0:0x%x\n",status);
    status = (u32)cec_register_read(REG_CEC_AUTO_PING_MAP1) + status; //0xcc 0xe2

    cec_logi("CEC Auto Ping Result:0x%x\n", status);
    cec_register_write(REG_CEC_AUTO_PING_CTRL, 0x00);

    *pstatus = status;

    return error;
}

u32 cec_init_irqs(void)
{
    //Set CEC Interrupt bit
    cec_register_write(REG_CEC_INT_ENABLE_0, 0x27);
    cec_register_write(REG_CEC_INT_ENABLE_1, 0x0f);
    //Set Logical Address and Caputure address
    cec_capture_id_set(g_cec_attr.logical_addr);

//    write_reg(HDMI_CORE_SYS, HDMI_CORE_SYS_INTR4_MASK, BIT_INTR4_CEC);
    return 0;
}

u32 cec_deinit_irqs(void)
{
    //Clean CEC Interrupt bit
//    write_reg(HDMI_CORE_SYS, HDMI_CORE_SYS_INTR4_MASK, 0);
    cec_register_write(REG_CEC_INT_ENABLE_0, 0x00);
    cec_register_write(REG_CEC_INT_ENABLE_1, 0x00);

    return 0;
}

void hdmi_cec_init(void)
{
    memset(&g_cec_attr, 0, sizeof(g_cec_attr));
}

int hdmi_cec_thread(void* parm)
{
    parm = parm;//ignore compire warnning
    logi("enter hdmi_cec_thread\n");
    while (g_cec_attr.cec_start) {
        if (kthread_should_stop()) {
            break;
        }

        hdmi_sys_lock();
        hdmi_cec_event_handler();
        hdmi_sys_unlock();
        msleep(50);
    }
    return 0;
}

int hdmi_cec_start(void* parm)
{
    bool ret = false;
    hdmi_cec_param cec_cmd;

    if (!g_cec_enable) {
        loge("Cec not enable.\n");
        goto done;
    }

    if ((!g_cec_attr.cec_start) && hdmi_is_connect()) {
        u32 cur_status = 0;
        u32 index = 0;
        memset(&g_cec_attr.network, 0, sizeof(g_cec_attr.network));
        memset(&g_cec_attr.physical_addr, 0, sizeof(g_cec_attr.physical_addr));
        memset(&g_cec_attr.result, 0, sizeof(g_cec_attr.result));
        g_cec_attr.cec_task = K3_HDMI_CEC_Idle;
        g_cec_attr.logical_addr = HDMI_CEC_LOGICALADD_PLAYDEV_1;
        g_cec_attr.new_task = K3_HDMI_CEC_Idle;
        g_cec_attr.tx_state = K3_HDMI_CEC_TX_WAITCMD;

        /*We need to read edid and then get cec addr */
        //hdmi_read_edid();

        /*We need to do AutoPing */
        cec_auto_ping(&cur_status);
        if (0x01 == (cur_status & 0x01)) {
            //Build up CEC Status!
            ret = hdmi_get_cec_addr(g_cec_attr.physical_addr);
            if ((!hdmi.edid_set) || (!ret) || (0 == g_cec_attr.physical_addr[0])) {
                cec_logi("edid not set or get phy addr fail\n");
                goto done;
            }


            //CEC Network
            for(index = 0; index < HDMI_CEC_LOGICALADD_BUTT; index ++) {
                if ((cur_status & (1 << index)) != 0) {
                    g_cec_attr.network[index] = true;
                }
            }
            //Logical Address
            if (g_cec_attr.network[HDMI_CEC_LOGICALADD_PLAYDEV_1] == true) {           //bit3
                if (g_cec_attr.network[HDMI_CEC_LOGICALADD_PLAYDEV_2] == true) {       //bit6
                    if (g_cec_attr.network[HDMI_CEC_LOGICALADD_PLAYDEV_3] == true) {   //bit7
                        g_cec_attr.logical_addr = 0x0f;//Brocast Address!

                    } else {
                        g_cec_attr.logical_addr = HDMI_CEC_LOGICALADD_PLAYDEV_3;
                    }
                } else {
                        g_cec_attr.logical_addr = HDMI_CEC_LOGICALADD_PLAYDEV_2;
                }
            } else {
                g_cec_attr.logical_addr = HDMI_CEC_LOGICALADD_PLAYDEV_1;
            }
            cec_logi("CEC is build up logical_addr:%d phy_addr:%d.%d.%d.%d\n",g_cec_attr.logical_addr, g_cec_attr.physical_addr[0],
                g_cec_attr.physical_addr[1],g_cec_attr.physical_addr[2],g_cec_attr.physical_addr[3]);
            //We can open CEC!

            hdmi_sys_lock();
            if (hdmi_is_connect()){

                 //Should send out Brocast messsage of <Report Physical Address> !
                 memset(&cec_cmd, 0, sizeof(cec_cmd));
                 cec_cmd.en_dst_addr = HDMI_CEC_LOGICALADD_BROADCAST;
                 cec_cmd.opcode = CEC_OPCODE_REPORT_PHYSICAL_ADDRESS;
                 cec_cmd.un_operand.raw_data.length  = 2;
                 cec_cmd.un_operand.raw_data.data[0] = ((g_cec_attr.physical_addr[0] << 4) & 0xf0) | (g_cec_attr.physical_addr[1] & 0x0f); // [Physical Address(A.B.C.D):A B]
                 cec_cmd.un_operand.raw_data.data[0] = ((g_cec_attr.physical_addr[2] << 4) & 0xf0) | (g_cec_attr.physical_addr[3] & 0x0f) ; // [Physical Address(A.B.C.D):C D]
                 hdmi_cec_send_command(&cec_cmd);
                 //Should send out Brocast message of <Vendor Device ID>!

                g_cec_attr.cec_start = true;
            }
            hdmi_sys_unlock();

            hdmi_cec_thread(NULL);
        }
    }

done:
    memset(&g_cec_attr, 0, sizeof(g_cec_attr));
    return 0;
}

void hdmi_cec_start_use_thread()
{
    //static struct  task_struct  *cec_start_thread = NULL;
    logi("hdmi_cec_start_use_thread enable:%d start:%d thread:%p\n", g_cec_enable, g_cec_attr.cec_start, g_cec_attr.cec_thread);
    if (!g_cec_enable) {
        loge("CEC is not enable, then do nothing\n");
        return;
    }

    if (g_cec_attr.cec_start) {
        logi("CEC is running, then do nothing\n");
        return;
    }

    if (g_cec_attr.cec_thread) {
        logi("CEC is starting, then do nothing\n");
        return;
    }

    g_cec_attr.cec_thread = kthread_create(hdmi_cec_start, NULL, "hdmi_start_cec");
    if (g_cec_attr.cec_thread == NULL) {
        loge("Unable to start cec kernel thread.\n");
        return;
    }
    wake_up_process(g_cec_attr.cec_thread);
}

void hdmi_cec_stop()
{
    g_cec_attr.cec_start = false;
    if (g_cec_attr.cec_thread) {
        int ret = kthread_stop(g_cec_attr.cec_thread);
        logi("end HDMI CEC thread. ret = %d\n" , ret);
    }

    memset(&g_cec_attr, 0, sizeof(g_cec_attr));
}

void hdmi_cec_enable()
{
    hdmi_sys_lock();
    g_cec_enable = true;
    hdmi_cec_start_use_thread();
    hdmi_sys_unlock();
}

void hdmi_cec_disable()
{
    hdmi_sys_lock();
    g_cec_enable = false;
    hdmi_cec_stop();
    hdmi_sys_unlock();
}
