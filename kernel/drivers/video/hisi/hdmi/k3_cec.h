/***********************************************************************************/
/*  Copyright (c) 2002-2006, Silicon Image, Inc.  All rights reserved.             */
/*  No part of this work may be reproduced, modified, distributed, transmitted,    */
/*  transcribed, or translated into any language or computer format, in any form   */
/*  or by any means without written permission of: Silicon Image, Inc.,            */
/*  1060 East Arques Avenue, Sunnyvale, California 94085                           */
/***********************************************************************************/

#ifndef __K3_CEC_H__
#define __K3_CEC_H__

#define HI_SUCCESS 0
#define HI_FAILURE (-1)
#define SII_MAX_CMD_SIZE 16 // defnes number of operands

#define HDMI_CMD_CEC            "hdmi_cec: "

//------------------------------------------------------------------------------
// Registers in Page 8 for cec
//------------------------------------------------------------------------------
#define REG_CEC_DEV_ID    0x80  //CEC Device ID Register
#define REG_CEC_SPEC            0x81  //CEC Specification Register
#define REG_CEC_SUFF            0x82  //CEC Specification Suffix Register
#define REG_CEC_FW              0x83  //CEC Firmware Revision Register
#define REG_CEC_DEBUG_0     0x84  //CEC Debug Register#0
#define REG_CEC_DEBUG_1     0x85  //CEC Debug Register#1
#define REG_CEC_DEBUG_2     0x86  //CEC Debug Register#2
#define REG_CEC_DEBUG_3     0x87  //CEC Debug Register#3
#define BIT_SNOOP_EN            0x01
#define BIT_FLUSH_TX_FIFO       0x80

#define REG_CEC_TX_INIT     0x88  //CEC Tx Initialization Register

#define REG_CEC_TX_DEST     0x89  //CEC Tx Destination Register
#define BIT_SEND_POLL           0x80

#define REG_CEC_CONFIG_CPI      0x8e  //CEC SetUp Register
#define BIT_CEC_PASS_THROUGH    0x10

#define REG_CEC_TX_COMMAND      0x8f  //CEC Tx Command Register:opcode

#define REG_CEC_TX_OPERANDS_0  0x90  //15Bytes:CEC Tx Operand#0~#14 Register

#define REG_CEC_TRANSMIT_DATA  0x9f  //CEC Transmit Data Register
#define BIT_TX_AUTO_CALC        0x20
#define BIT_TRANSMIT_CMD        0x10

#define REG_CEC_RETRY_LIMIT    0xa0  //CEC Tx Retry Limit

#define REG_CEC_CAPTURE_ID0    0xa2  //2Btes:CEC Capture ID0 Register

#define REG_CEC_INT_ENABLE_0    0xa4  //CEC Interrupt Enable Register#0
#define REG_CEC_INT_ENABLE_1    0xa5  //CEC Interrupt Enable Register#2

#define REG_CEC_AUTO_PING_CTRL 0xE0
#define REG_CEC_AUTO_PING_MAP0 0xE1
#define REG_CEC_AUTO_PING_MAP1 0xE2

// 0xA6 CPI Interrupt Status Register (R/W)
#define REG_CEC_INT_STATUS_0   0xa6  //CEC Interrupt Status Register#0
#define BIT_CEC_LINE_STATE      0x80
#define BIT_TX_MESSAGE_SENT     0x20
#define BIT_TX_HOTPLUG          0x10
#define BIT_POWER_STAT_CHANGE   0x08
#define BIT_TX_FIFO_EMPTY       0x04
#define BIT_RX_MSG_RECEIVED     0x02
#define BIT_CMD_RECEIVED        0x01
// 0xA7 CPI Interrupt Status Register (R/W)
#define REG_CEC_INT_STATUS_1   0xa7  //CEC Interrupt Status Register#1
#define BIT_RX_FIFO_OVERRUN     0x08
#define BIT_SHORT_PULSE_DET     0x04
#define BIT_FRAME_RETRANSM_OV   0x02
#define BIT_START_IRREGULAR     0x01

#define REG_CEC_RX_CONTROL     0xac  //CEC RX Control Register
// CEC  CEC_RX_CONTROL bits
#define BIT_CLR_RX_FIFO_CUR     0x01
#define BIT_CLR_RX_FIFO_ALL     0x02

#define REG_CEC_RX_COUNT       0xad  //CEC Rx Count Register
#define BIT_MSG_ERROR           0x80

#define REG_CEC_RX_CMD_HEADER   0xae  //CEC Rx Command Header Register
#define CEC_RX_COMMAND          0xaf   //CEC Rx Command Register:opcode
#define REG_CEC_RX_OPERAND_0    0xb0  //15Bytes:CEC Rx Operand#0~#14  Register

#define REG_CEC_OP_ABORT1      0xc0  //CEC feature abort registers1
#define REG_CEC_OP_ABORT32     0xdf  //CEC feature abort registers32

#define REG_CEC_AUTO_PING_CTL  0xe0  //CEC Auto Ping Control Register
#define CEC_AUTO_PING_MAP0      0xe1  //CEC Auto Ping mapping Registers0
#define CEC_AUTO_PING_MAP1      0xe2  //CEC Auto Ping mapping Registers1


/**<  CEC */
/**< CEC Opcode:Please refer to CEC 15 Message Descriptions */
/**< General Protocol messages*/
#define CEC_OPCODE_FEATURE_ABORT                  0X00  /**< <Feature Abort> Used as a response to indicate that the device does not support the requested message type, or that it cannot execute it at the present time. */
#define CEC_OPCODE_ABORT_MESSAGE                  0XFF  /**< <Abort> Message This message is reserved for testing purposes. */
/**< One Touch Play Feature*/
#define CEC_OPCODE_ACTIVE_SOURCE                  0X82  /**< <Active Source> Used by a new source to indicate that it has started to transmit a stream OR used in response to a <Request Active Source>  */
#define CEC_OPCODE_IMAGE_VIEW_ON                  0X04  /**< <Image View On> Sent by a source device to the TV whenever it enters the active state (alternatively it may send <Text View On>). */
#define CEC_OPCODE_TEXT_VIEW_ON                   0X0D  /**< <Text View On> As <Image View On>, but should also remove any text, menus and PIP windows from the TV's display. */
/**< Routing Control Feature*/
#define CEC_OPCODE_INACTIVE_SOURCE                0X9D  /**< <Inactive Source> Used by the currently active source to inform the TV that it has no video to be presented to the user, or is going into standby as the result of a local user command on the device. */
#define CEC_OPCODE_REQUEST_ACTIVE_SOURCE          0X85  /**< <Request Active Source> Used by a new device to discover the status of the system. */
#define CEC_OPCODE_ROUTING_CHANGE                 0X80  /**< <Routing Change> Sent by a CEC Switch when it is manually switched to inform all other devices on the network that the active route below the switch has changed. */
#define CEC_OPCODE_ROUTING_INFORMATION            0X81  /**< <Routing Information> Sent by a CEC Switch to indicate the active route below the switch. */
#define CEC_OPCODE_SET_STREAM_PATH                0X86  /**< <Set Stream Path> Used by the TV to request a streaming path from the specified physical address. */
/**< Standby Feature*/
#define CEC_OPCODE_STANDBY                        0X36  /**< <Standby> Switches one or all devices into standby mode. Can be used as a broadcast message or be addressed to a specific device. See section CEC 13.3 for important notes on the use of this message */
/**< One Touch Record Feature*/
#define CEC_OPCODE_RECORD_OFF                     0X0B  /**< <Record Off> Requests a device to stop a recording. */
#define CEC_OPCODE_RECORD_ON                      0X09  /**< <Record On> Attempt to record the specified source. */
#define CEC_OPCODE_RECORD_STATUS                  0X0A  /**< <Record Status> Used by a Recording Device to inform the initiator of the message <Record On> about its status. */
#define CEC_OPCODE_RECORD_TV_SCREEN               0X0F  /**< <Record TV Screen> Request by the Recording Device to record the presently displayed source. */
/**< Timer Programming Feature*/
#define CEC_OPCODE_CLEAR_ANALOGUE_TIMER           0X33  /**< <Clear Analogue Timer> Used to clear an Analogue timer block of a device. */
#define CEC_OPCODE_CLEAR_DIGITAL_TIMER            0X99  /**< <Clear Digital Timer> Used to clear a Digital timer block of a device. */
#define CEC_OPCODE_CLEAR_EXTERNAL_TIMER           0XA1  /**< <Clear External Timer> Used to clear an External timer block of a device. */
#define CEC_OPCODE_SET_ANALOGUE_TIMER             0X34  /**< <Set Analogue Timer> Used to set a single timer block on an Analogue Recording Device. */
#define CEC_OPCODE_SET_DIGITAL_TIMER              0X97  /**< <Set Digital Timer> Used to set a single timer block on a Digital Recording Device. */
#define CEC_OPCODE_SET_EXTERNAL_TIMER             0XA2  /**< <Set External Timer> Used to set a single timer block to record from an external device. */
#define CEC_OPCODE_SET_TIMER_PROGRAM_TITLE        0X67  /**< <Set Timer Program Title> Used to set the name of a program associated with a timer block. Sent directly after sending a <Set Analogue Timer> or <Set Digital Timer> message. The name is then associated with that timer block. */
#define CEC_OPCODE_TIMER_CLEARED_STATUS           0X43  /**< <Timer Cleared Status> Used to give the status of a <Clear Analogue Timer>, <Clear Digital Timer> or <Clear External Timer> message. */
#define CEC_OPCODE_TIMER_STATUS                   0X35  /**< <Timer Status> Used to send timer status to the initiator of a <Set Timer> message. */
/**< System Information Feature*/
#define CEC_OPCODE_CEC_VERSION                    0X9E  /**< <CEC Version> Used to indicate the supported CEC version, in response to a <Get CEC Version> */
#define CEC_OPCODE_GET_CEC_VERSION                0X9F  /**< <Get CEC Version> Used by a device to enquire which version of CEC the target supports */
#define CEC_OPCODE_GIVE_PHYSICAL_ADDRESS          0X83  /**< <Give Physical Address> A request to a device to return its physical address. */
#define CEC_OPCODE_REPORT_PHYSICAL_ADDRESS        0X84  /**< <Report Physical Address> Used to inform all other devices of the mapping between physical and logical address of the initiator. */
#define CEC_OPCODE_GET_MENU_LANGUAGE              0X91  /**< <Get Menu Language> Sent by a device capable of character generation (for OSD and Menus) to a TV in order to discover the currently selected Menu language. Also used by a TV during installation to discover the currently set menu language of other devices. */
#define CEC_OPCODE_SET_MENU_LANGUAGE              0X32  /**< <Set Menu Language> Used by a TV or another device to indicate the menu language. */
/**< Deck Control Feature*/
#define CEC_OPCODE_DECK_CONTROL                   0X42  /**< <Deck Control> Used to control a device's media functions. */
#define CEC_OPCODE_DECK_STATUS                    0X1B  /**< <Deck Status> Used to provide a deck's status to the initiator of the <Give Deck Status> message. */
#define CEC_OPCODE_GIVE_DECK_STATUS               0X1A  /**< <Give Deck Status> Used to request the status of a device, regardless of whether or not it is the current active source. */
#define CEC_OPCODE_PLAY                           0X41  /**< <Play> Used to control the playback behaviour of a source device. */
/**< Tuner Control Feature*/
#define CEC_OPCODE_GIVE_TUNER_DEVICE_STATUS       0X08  /**< <Give Tuner Device Status> Used to request the status of a tuner device. */
#define CEC_OPCODE_SELECT_ANALOGUE_SERVICE        0X92  /**< <Select Analogue Service> Directly selects an Analogue TV service */
#define CEC_OPCODE_SELECT_DIGITAL_SERVICE         0X93  /**< <Select Digital Service> Directly selects a Digital TV, Radio or Data Broadcast Service */
#define CEC_OPCODE_TUNER_DEVICE_STATUS            0X07  /**< <Tuner Device Status> Use by a tuner device to provide its status to the initiator of the <Give Tuner Device Status> message. */
#define CEC_OPCODE_TUNER_STEP_DECREMENT           0X06  /**< <Tuner Step Decrement> Used to tune to next lowest service in a tuner's service list. Can be used for PIP. */
#define CEC_OPCODE_TUNER_STEP_INCREMENT           0X05  /**< <Tuner Step Increment> Used to tune to next highest service in a tuner's service list. Can be used for PIP. */
/**< Vendor Specific Command*/
#define CEC_OPCODE_DEVICE_VENDOR_ID               0X87  /**< <Device Vendor ID> Reports the vendor ID of this device. */
#define CEC_OPCODE_GIVE_DEVICE_VENDOR_ID          0X8C  /**< <Give Device Vendor ID> Requests the Vendor ID from a device. */
#define CEC_OPCODE_VENDOR_COMMAND                 0X89  /**< <Vendor Command> Allows vendor specific commands to be sent between two devices. */
#define CEC_OPCODE_VENDOR_COMMAND_WITH_ID         0XA0  /**< <Vendor Command With ID> Allows vendor specific commands to be sent between two devices or broadcast. */
#define CEC_OPCODE_VENDOR_REMOTE_BUTTON_DOWN      0X8A  /**< <Vendor Remote Button Down> Indicates that a remote control button has been depressed. */
#define CEC_OPCODE_VENDOR_REMOTE_BUTTON_UP        0X8B  /**< <Vendor Remote Button Up> Indicates that a remote control button (the last button pressed indicated by the Vendor Remote Button Down message) has been released. */
/**< OSD Display Feature*/
#define CEC_OPCODE_SET_OSD_STRING                 0X64  /**< <Set OSD String> Used to send a text message to output on a TV. */
#define CEC_OPCODE_GIVE_OSD_NAME                  0X46  /**< <Give OSD Name> Used to request the preferred OSD name of a device for use in menus associated with that device. */
#define CEC_OPCODE_SET_OSD_NAME                   0X47  /**< <Set OSD Name> Used to set the preferred OSD name of a device for use in menus associated with that device. */
/**< Device Menu Control Feature*/
#define CEC_OPCODE_MENU_REQUEST                   0X8D  /**< <Menu Request> A request from the TV for a device to show/remove a menu or to query if a device is currently showing a menu. */
#define CEC_OPCODE_MENU_STATUS                    0X8E  /**< <Menu Status> Used to indicate to the TV that the device is showing/has removed a menu and requests the remote control keys to be passed though. */
#define CEC_OPCODE_USER_CONTROL_PRESSED           0X44  /**< <User Control Pressed> Used to indicate that the user pressed a remote control button or switched from one remote control button to another. */
#define CEC_OPCODE_USER_CONTROL_RELEASED          0X45  /**< <User Control Released> Indicates that user released a remote control button (the last one indicated by the <User Control Pressed> message) */
/**< Power Status Feature*/
#define CEC_OPCODE_GIVE_DEVICE_POWER_STATUS       0X8F  /**< <Give Device Power Status> Used to determine the current power status of a target device */
#define CEC_OPCODE_REPORT_POWER_STATUS            0X90  /**< <Report Power Status> Used to inform a requesting device of the current power status */
/**< System Audio Control Feature*/
#define CEC_OPCODE_GIVE_AUDIO_STATUS              0X71  /**< <Give Audio Status> Requests an amplifier to send its volume and mute status */
#define CEC_OPCODE_GIVE_SYSTEM_AUDIO_MODE_STATUS  0x7D  /**< <Give System Audio Mode Status> Requests the status of the System Audio Mode */
#define CEC_OPCODE_REPORT_AUDIO_STATUS            0X7A  /**< <Report Audio Status> Reports an amplifier s volume and mute status */
#define CEC_OPCODE_SET_SYSTEM_AUDIO_MODE          0X72  /**< <Set System Audio Mode> Turns the System Audio Mode On or Off. */
#define CEC_OPCODE_SYSTEM_AUDIO_MODE_REQUEST      0X70  /**< <System Audio Mode Request> A device implementing System Audio Control and which has volume control RC buttons (eg TV or STB) requests to use System Audio Mode to the amplifier */
#define CEC_OPCODE_SYSTEM_AUDIO_MODE_STATUS       0X7E  /**< <System Audio Mode Status> Reports the current status of the System Audio Mode */
/**< Audio Rate Control Feature*/
#define CEC_OPCODE_SET_AUDIO_RATE                 0X9A  /**< <Set Audio Rate> Used to control audio rate from Source Device. */

#define CEC_OPCODE_POLLING_MESSAGE                0XFE/**<  POLL message have no opcode, So, we just use this value */

typedef enum _HDMI_CEC_LOGICALADD_E
{
    HDMI_CEC_LOGICALADD_TV               = 0X00,
    HDMI_CEC_LOGICALADD_RECORDDEV_1      = 0X01,
    HDMI_CEC_LOGICALADD_RECORDDEV_2      = 0X02,
    HDMI_CEC_LOGICALADD_TUNER_1          = 0X03,
    HDMI_CEC_LOGICALADD_PLAYDEV_1        = 0X04,
    HDMI_CEC_LOGICALADD_AUDIOSYSTEM      = 0X05,
    HDMI_CEC_LOGICALADD_TUNER_2          = 0X06,
    HDMI_CEC_LOGICALADD_TUNER_3          = 0X07,
    HDMI_CEC_LOGICALADD_PLAYDEV_2        = 0X08,
    HDMI_CEC_LOGICALADD_RECORDDEV_3      = 0X09,
    HDMI_CEC_LOGICALADD_TUNER_4          = 0X0A,
    HDMI_CEC_LOGICALADD_PLAYDEV_3        = 0X0B,
    HDMI_CEC_LOGICALADD_RESERVED_1       = 0X0C,
    HDMI_CEC_LOGICALADD_RESERVED_2       = 0X0D,
    HDMI_CEC_LOGICALADD_SPECIALUSE       = 0X0E,
    HDMI_CEC_LOGICALADD_BROADCAST        = 0X0F,
    HDMI_CEC_LOGICALADD_BUTT
}HDMI_CEC_LOGICALADD_E;

typedef enum _CEC_CMDTYPE_E
{
    HDMI_CEC_STRUCTCOMMAND,                    /**< CEC 结构命令 */
    HDMI_CEC_RAWCOMMAND,                       /**< CEC 原始命令 */
    HDMI_CEC_BUTT
}HDMI_CEC_CMDTYPE_E;

typedef struct _CEC_RAWDATA_S
{
    u8 length;
    u8 data[15];
}cec_rawdata;

typedef union _hdmi_cec_operand
{
    cec_rawdata               raw_data;
}hdmi_cec_operand;

/* CEC 命令结构 */

/** HDMI CEC 参数结构 */
typedef struct _hdmi_cec_param
{
    HDMI_CEC_LOGICALADD_E  en_src_addr;
    HDMI_CEC_LOGICALADD_E  en_dst_addr;
    hdmi_cec_operand      un_operand;
    u8                    opcode;
    u8                    reserved[3];
}hdmi_cec_param;


typedef enum
{
    K3_HDMI_CEC_LOGICADDR_TV          = 0x00,
    K3_HDMI_CEC_LOGICADDR_RecDev1     = 0x01,
    K3_HDMI_CEC_LOGICADDR_RecDev2     = 0x02,
    K3_HDMI_CEC_LOGICADDR_STB1        = 0x03,
    K3_HDMI_CEC_LOGICADDR_DVD1        = 0x04,
    K3_HDMI_CEC_LOGICADDR_AudSys      = 0x05,
    K3_HDMI_CEC_LOGICADDR_STB2        = 0x06,
    K3_HDMI_CEC_LOGICADDR_STB3        = 0x07,
    K3_HDMI_CEC_LOGICADDR_DVD2        = 0x08,
    K3_HDMI_CEC_LOGICADDR_RecDev3     = 0x09,
    K3_HDMI_CEC_LOGICADDR_Res1        = 0x0A,
    K3_HDMI_CEC_LOGICADDR_Res2        = 0x0B,
    K3_HDMI_CEC_LOGICADDR_Res3        = 0x0C,
    K3_HDMI_CEC_LOGICADDR_Res4        = 0x0D,
    K3_HDMI_CEC_LOGICADDR_FreeUse     = 0x0E,
    K3_HDMI_CEC_LOGICADDR_Unregister  = 0x0F
} K3_HDMI_CEC_LOGICADDR_T;


typedef struct
{
    u8 cec_count;
    u8 rx_next_count;
    u8 destorrx_header;
    u8 cec_opcode;
    u8 cec_operand[ SII_MAX_CMD_SIZE ];
} hdmi_cec_cmd;

typedef enum
{
    K3_HDMI_CEC_ERROR_STARTBITIRREGULAR = 0x01,
    K3_HDMI_CEC_ERROR_SHORTPLUSEDETECT = 0x04,
    K3_HDMI_CEC_ERROR_RXOVERFLOW = 0x08,
} K3_HDMI_CEC_ERRORS_E;

typedef enum
{
    K3_HDMI_CEC_TX_WAITCMD  = 0,    //CEC TX Wait message to Send Status
    K3_HDMI_CEC_TX_SENDING,         //CEC TX Sending message
    K3_HDMI_CEC_TX_SENDACKED,       //CEC TX Send and get Acked.
    K3_HDMI_CEC_TX_SENDFAILURE      //CEC TX Send Failure
} K3_HDMI_CEC_TX_STATUS_T;

typedef struct
{
    u8 rx_state;              //CEC RX Status
    u8 tx_state;              //CEC TX Status
    u8 cec_errors;            //CEC Error message
} cec_int;

typedef enum
{
    K3_HDMI_CEC_Idle,
    K3_HDMI_CEC_ReqPing,
    K3_HDMI_CEC_ReqCmd1,
    K3_HDMI_CEC_ReqCmd2,
    K3_HDMI_CEC_ReqCmd3
} HDMI_CEC_OP_E;


void hdmi_cec_event_handler(void );
u32 hdmi_cec_send_command(hdmi_cec_param  *p_cec_cmd);

void hdmi_cec_stop(void);
void hdmi_cec_start_use_thread(void);
void hdmi_cec_enable(void);
void hdmi_cec_disable(void);
char * cec_cmd_tostr(hdmi_cec_param* cec_cmd, char *str);
hdmi_cec_param* cec_str_tocmd(const char *str, hdmi_cec_param* cec_cmd);
void hdmi_cec_init(void);


#endif // __K3_CEC_H__



