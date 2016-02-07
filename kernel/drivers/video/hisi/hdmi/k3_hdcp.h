/******************************************************************************
 *
 * Copyright 2013, Silicon Image, Inc.  All rights reserved.
 * No part of this work may be reproduced, modified, distributed, transmitted,
 * transcribed, or translated into any language or computer format, in any form
 * or by any means without written permission of: Silicon Image, Inc., 1140
 * East Arques Avenue, Sunnyvale, California 94085
 *
 *****************************************************************************/
/**
 * @file Hdcp.h
 *
 * @brief Definition of the Foo API.
 *
 *****************************************************************************/

#ifndef __K3_HDCP_H__
#define  __K3_HDCP_H__

#define READKSV 0
#define IS_REPEATER 0

typedef struct _hdcp_info {
    bool tx_supports;                // True if the TX supports HDCP... False otherwise.
    bool askv_valid;                 // True if the TX has valid AKSVs... False otherwise.
    bool started;                    // True if the HDCP enable bit has been set... False otherwise.
    u8 link_protection_level;
    bool override;                   // HDCP Override
    bool hdcp_enable;
    void (*hdcp_notify)(char *result);
} hdcp_info;

#define HDCP_AUTH_SUCCESS   "HDCP_STATE=hdcp_auth_success"
#define HDCP_AUTH_FAILED    "HDCP_STATE=hdcp_auth_failed"

#define KSV_ARRAY_SIZE          128

// DDC Bus Bit Masks
//==================
#define BIT_DDC_HDMI            0x80
#define BIT_DDC_REPEATER        0x40
#define BIT_DDC_FIFO_RDY        0x20
#define DEVICE_COUNT_MASK       0x7F

// KSV Buffer Size
//================
#define DEVICE_COUNT            128    // May be tweaked as needed

#define AUDIO_ERROR_EVENT       0x10
#define SECURITY_CHANGE_EVENT   0x20
#define V_READY_EVENT           0x40
#define HDCP_CHANGE_EVENT       0x80

#define TPI_HW_DBG1_ADDR (0x79*4)
#define TPI_HW_DBG3_ADDR (0x7B*4)

void hdcp_init(void (*hdcp_notify)(char *result));
void hdcp_handler(u8 intr);
void hdcp_off(void);

bool hdcp_get_enable(void);
void hdcp_set_enable(bool enable);
bool hdcp_get_start(void);


#endif /*__SI_CPHDCP_H__*/


