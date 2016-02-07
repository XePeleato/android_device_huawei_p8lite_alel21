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
 * @file si_cpHdcp.c
 *
 * @brief Definition of the Foo API.
 *
 *****************************************************************************/
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/slab.h>

#include "k3_edid.h"
#include "k3_hdmi_hw_tpi.h"
#define LOG_TAG "hdmi-hdcp"
#include "k3_hdmi.h"
#include "k3_hdmi_log.h"
#include "k3_hdcp.h"

#define AKSV_SIZE          5
#define NUM_OF_ONES_IN_KSV 20

hdcp_info hdcp_para = {0};
extern u8 edid[HDMI_EDID_MAX_LENGTH];
extern hdmi_device hdmi;

static bool are_aksv_ok(void);
static bool is_hdcp_supported(void);
static void hdcp_on(void);

/******************************************************************************
* Function:       get_ksv
* Description:    Collect all downstrean KSV for verification
* Data Accessed:
* Data Updated:
* Input:
* Output:
* Return:         true if KSVs collected successfully. False if not.
* Others:         The buffer is limited to KSV_ARRAY_SIZE due to the 8051 implementation.
*******************************************************************************/
#if READKSV
static bool get_ksv(void)
{
    u8 i;
    u16 KeyCount;
    u8 KSV_Array[KSV_ARRAY_SIZE];

    IN_FUNCTION;

    i = tpi_read_reg(DDC_BSTATUS_ADDR_L);
    KeyCount = (i & DEVICE_COUNT_MASK) * 5;
    if (KeyCount != 0) {
        //ReadBlockHdcp(DDC_KSV_FIFO_ADDR, KeyCount, KSV_Array);
    }

    OUT_FUNCTION;
    return true;
}
#endif
/******************************************************************************
* Function:       is_repeater
* Description:    Test if sink is a repeater
* Data Accessed:
* Data Updated:
* Input:
* Output:
* Return:         true if sink is a repeater. false if not.
* Others:
*******************************************************************************/
#if IS_REPEATER
static bool is_repeater(void)
{
    u8 reg;

    IN_FUNCTION;

    reg = tpi_read_reg(TPI_HDCP_QUERY_DATA_REG);

    if (reg & HDCP_REPEATER_MASK) {
        return true;
    }

    OUT_FUNCTION;
    return false;           // not a repeater
}
#endif
static void restart_hdcp(void)
{
    IN_FUNCTION;
    tpi_modify_reg(TPI_HDCP_CONTROL_DATA_REG, PROTECTION_LEVEL_MASK, PROTECTION_LEVEL_MIN);
    msleep(100);//Let TX sends one frame at least to inform RX.
    tpi_modify_reg(TPI_HDCP_CONTROL_DATA_REG, PROTECTION_LEVEL_MASK, PROTECTION_LEVEL_MAX);
    OUT_FUNCTION;
}

/******************************************************************************
* Function:       is_hdcp_supported
* Description:    Check Tx revision number to find if this Tx supports HDCP
                  by reading the HDCP revision number from TPI register 0x30.
* Data Accessed:
* Data Updated:
* Input:
* Output:
* Return:         true if Tx supports HDCP. false if not.
* Others:
*******************************************************************************/
static bool is_hdcp_supported(void)
{
    u8 hdcp_rev;
    bool hdcp_supported;

    IN_FUNCTION;

    hdcp_supported = true;

    // Check Device ID
    hdcp_rev = (u8)tpi_read_reg(TPI_HDCP_REVISION_DATA_REG);

    if (hdcp_rev != (HDCP_MAJOR_REVISION_VALUE | HDCP_MINOR_REVISION_VALUE))  {
        hdcp_supported = false;
    }

    // Even if HDCP is supported check for incorrect Device ID
    if (tpi_read_reg(TPI_AKSV_1_REG) == 0x90
        && tpi_read_reg(TPI_AKSV_2_REG) == 0x22
        && tpi_read_reg(TPI_AKSV_3_REG) == 0xA0
        && tpi_read_reg(TPI_AKSV_4_REG) == 0x00
        && tpi_read_reg(TPI_AKSV_5_REG) == 0x00) {
        hdcp_supported = false;
    }

    OUT_FUNCTION;
    return hdcp_supported;
}

static void HdcpToggleTmdsForHdcpAuthentication(void)
{
    logd("Toggle HDCP Authentication.\n");
    // a three step process is mandatory
    tpi_modify_reg(TPI_SYSTEM_CONTROL_DATA_REG, AV_MUTE_MASK, AV_MUTE_MUTED);
    tpi_modify_reg(TPI_SYSTEM_CONTROL_DATA_REG, TMDS_OUTPUT_CONTROL_MASK, TMDS_OUTPUT_CONTROL_POWER_DOWN);
    tpi_modify_reg(TPI_SYSTEM_CONTROL_DATA_REG, TMDS_OUTPUT_CONTROL_MASK, TMDS_OUTPUT_CONTROL_ACTIVE);

    //SiiDrvReadModifyWriteTPI(TPI_SYSTEM_CONTROL_DATA_REG, AV_MUTE_MASK, AV_MUTE_NORMAL);
    hdcp_para.started = false;
}

/******************************************************************************
* Function:       hdcp_on
* Description:    Switch hdcp on.
* Data Accessed:
* Data Updated:
* Input:
* Output:
* Return:
* Others:
*******************************************************************************/
static void hdcp_on(void)
{
    u8 uReg_TPI_HW_DBG1 = 0;
    u8 uReg_TPI_HW_DBG3 = 0;
    u8 uTimeOut         = 100;
    u32 regdata_hdcp_con;
    IN_FUNCTION;

    logd("TPI_SYSTEM_CONTROL_DATA_REG(0x1A):0x%x\n",tpi_read_reg(TPI_SYSTEM_CONTROL_DATA_REG));
    regdata_hdcp_con= tpi_read_reg(TPI_HDCP_CONTROL_DATA_REG);
    logd("pre regdata_hdcp_con is 0x%x.\n",regdata_hdcp_con);

    regdata_hdcp_con= tpi_read_reg(TPI_HDCP_CONTROL_DATA_REG);
    logd("after regdata_hdcp_con is 0x%x.\n",regdata_hdcp_con);

    //Step 1: Enable HDCP. Drive hardware to read BKSV.
    if (hdcp_para.started == false) {
        logi("HDCP -> Started\n");
        tpi_modify_reg(TPI_HDCP_CONTROL_DATA_REG, PROTECTION_LEVEL_MASK, PROTECTION_LEVEL_MAX);

        //Step 2: Tell if read BKSV successfully.
        while ((!((uReg_TPI_HW_DBG1 & BIT0) | (uReg_TPI_HW_DBG3 & BIT0))) && uTimeOut) {
            mdelay(10);
            uReg_TPI_HW_DBG1 = tpi_read_reg(TPI_HW_DBG1_ADDR);
            uReg_TPI_HW_DBG3 = tpi_read_reg(TPI_HW_DBG3_ADDR);
            uTimeOut--;
        }

        if (0x00 == uTimeOut) {
            loge("HDCP->Read BKSV Timeout.\n");
            hdcp_para.started = false;
        }
        else if (uReg_TPI_HW_DBG1 & BIT0) {
            logi("HDCP->Read BKSV Done. Sink does supports HDCP.\n");
            hdcp_para.started = true;
        }
        else if (uReg_TPI_HW_DBG3 & BIT0) {
            logi("HDCP->Read BKSV Error. Sink Does NOT supports HDCP.\n");
            hdcp_para.started = false;
        }

        if (false == hdcp_para.started) {
            HdcpToggleTmdsForHdcpAuthentication();
            hdcp_para.link_protection_level= EXTENDED_LINK_PROTECTION_NONE | LOCAL_LINK_PROTECTION_NONE;
        }
    }

    OUT_FUNCTION;
}

/******************************************************************************
* Function:       are_aksv_ok
* Description:    Check if AKSVs contain 20 '0' and 20 '1'
* Data Accessed:
* Data Updated:
* Input:
* Output:
* Return:         true if 20 zeros and 20 ones found in AKSV. false OTHERWISE
* Others:
*******************************************************************************/
static bool are_aksv_ok(void)
{
    u8 b_data[AKSV_SIZE];
    u8 num_of_ones = 0;
    u8 i;
    u8 j;

    IN_FUNCTION;

    for (i = 0; i < AKSV_SIZE; i++) {
        b_data[i] = (u8)tpi_read_reg(TPI_AKSV_1_REG + i*4);//modify by zhang
    }

    for (i = 0; i < AKSV_SIZE; i++) {
        for (j = 0; j < BYTE_SIZE; j++) {
            if (b_data[i] & 0x01) {
                num_of_ones++;
            }

            b_data[i] >>= 1;
        }
    }

    if (num_of_ones != NUM_OF_ONES_IN_KSV) {
        return false;
    }

    OUT_FUNCTION;
    return true;
}

/******************************************************************************
* Function:       hdcp_init
* Description:    Tests Tx and Rx support of HDCP. If found, checks if
                  and attempts to set the security level accordingly.
* Data Accessed:
* Data Updated:
* Input:
* Output:
* Return:
* Others:
*******************************************************************************/
void hdcp_init(void (*hdcp_notify)(char *result))
{
    IN_FUNCTION;

    //hdcp_para.hdcp_enable = true;//controlled by hdcp store
    hdcp_para.hdcp_notify = hdcp_notify;
    hdcp_para.tx_supports = false;
    hdcp_para.askv_valid = false;
    hdcp_para.started = false;
    hdcp_para.link_protection_level = EXTENDED_LINK_PROTECTION_NONE | LOCAL_LINK_PROTECTION_NONE;

    logi("hdcp init.\n");
    /* This is TX-related... need only be done once.*/
    if (!is_hdcp_supported()) {
        /* The TX does not support HDCP, so authentication will never be attempted.
           Video will be shown as soon as TMDS is enabled.*/
        loge("HDCP -> TX does not support HDCP\n");
        return;
    }

    hdcp_para.tx_supports = true;

    /* This is TX-related... need only be done once.*/
    if (!are_aksv_ok()) {
        /* The TX supports HDCP, but does not have valid AKSVs.
           Video will not be shown.*/
        loge("HDCP -> Illegal AKSV\n");
        return;
    }

    hdcp_para.askv_valid = true;

    logi("HDCP -> Supported by TX, AKSVs valid\n");

    OUT_FUNCTION;
}

void hdcp_handler(u8 intr)
{
    u8 QueryData;
    u8 LinkStatus;
    u8 RegImage;
    u8 NewLinkProtectionLevel;

    IN_FUNCTION;

    if (false == hdmi.connected) {
        return;
    }

    logd("TPI_SYSTEM_CONTROL_DATA_REG(0x1A):0x%x\n", tpi_read_reg(TPI_SYSTEM_CONTROL_DATA_REG));

    if (hw_support_mhl()) {
        intr = tpi_read_reg(TPI_INTERRUPT_STATUS_REG);
    }
    logd("TPI_INTERRUPT_STATUS_REG:0x%x\n", intr);
    //Clear Interrupt Status.
    if (intr & (~(BIT2 | BIT3))) {
        tpi_write_reg(TPI_INTERRUPT_STATUS_REG, intr & (~(BIT2 | BIT3)));
    }

    if (!hdcp_para.hdcp_enable) {
        tpi_modify_reg(TPI_SYSTEM_CONTROL_DATA_REG, AV_MUTE_MASK, AV_MUTE_NORMAL);
        logd("hdcp is not enabled.\n");
        return;
    }

    if (!hdcp_para.tx_supports || !hdcp_para.askv_valid) {
        return;
    }

    if ((hdcp_para.link_protection_level == (EXTENDED_LINK_PROTECTION_NONE | LOCAL_LINK_PROTECTION_NONE))
        && (!hdcp_para.started)) {
        QueryData = tpi_read_reg(TPI_HDCP_QUERY_DATA_REG);
        logd("TPI_HDCP_QUERY_DATA_REG(0x29): 0x%x\n", QueryData);
        if (QueryData & PROTECTION_TYPE_MASK) {  // Is HDCP avaialable
            hdcp_on();
        }
    }

    // Check if Link Status has changed:
    if (intr & SECURITY_CHANGE_EVENT) {
        logi("HDCP -> ");

        LinkStatus  = tpi_read_reg(TPI_HDCP_QUERY_DATA_REG);
        LinkStatus &= LINK_STATUS_MASK;

        switch (LinkStatus)
        {
            case LINK_STATUS_NORMAL:
                logi("Link = Normal\n");
                break;

            case LINK_STATUS_LINK_LOST:
                logi("Link = Lost\n");
                restart_hdcp();
                break;

            case LINK_STATUS_RENEGOTIATION_REQ:
                logi("Link = Renegotiation Required\n");
                restart_hdcp();
                break;

            case LINK_STATUS_LINK_SUSPENDED:
                logi("Link = Suspended\n");
                tpi_modify_reg(TPI_SYSTEM_CONTROL_DATA_REG, LINK_INTEGRITY_MODE_MASK,
                    LINK_INTEGRITY_DYNAMIC);
                restart_hdcp();
                break;
        }
    }

    // Check if HDCP state has changed:
    if (intr & HDCP_CHANGE_EVENT) {
        RegImage = tpi_read_reg(TPI_HDCP_QUERY_DATA_REG);

        NewLinkProtectionLevel = RegImage & (EXTENDED_LINK_PROTECTION_MASK | LOCAL_LINK_PROTECTION_MASK);
        if (NewLinkProtectionLevel != hdcp_para.link_protection_level) {
            logi("HDCP -> ");

            hdcp_para.link_protection_level = NewLinkProtectionLevel;

            switch (hdcp_para.link_protection_level)
            {
                case (EXTENDED_LINK_PROTECTION_NONE | LOCAL_LINK_PROTECTION_NONE):
                    logi("Protection = None\n");
                    hdcp_para.hdcp_notify(HDCP_AUTH_FAILED);
                    break;

                case LOCAL_LINK_PROTECTION_SECURE:
                    hdcp_para.hdcp_notify(HDCP_AUTH_SUCCESS);
                    tpi_modify_reg(TPI_SYSTEM_CONTROL_DATA_REG, AV_MUTE_MASK, AV_MUTE_NORMAL);
                    logi("Protection = Local, Video Unmuted\n");
                    break;

                case (EXTENDED_LINK_PROTECTION_SECURE | LOCAL_LINK_PROTECTION_SECURE):
                    hdcp_para.hdcp_notify(HDCP_AUTH_SUCCESS);
                    logi("Protection = Extended\n");
                    tpi_modify_reg(TPI_SYSTEM_CONTROL_DATA_REG, AV_MUTE_MASK, AV_MUTE_NORMAL);
                    break;

                default:
                    hdcp_para.hdcp_notify(HDCP_AUTH_FAILED);
                    logi("Protection = Extended but not Local?\n");
                    break;
            }
        }
    }

    OUT_FUNCTION;
}

/******************************************************************************
* Function:       hdcp_off
* Description:    Switch hdcp off.
* Data Accessed:
* Data Updated:
* Input:
* Output:
* Return:
* Others:
*******************************************************************************/
void hdcp_off(void)
{
    IN_FUNCTION;

    if (hdcp_para.started) {
        // AV MUTE
        tpi_modify_reg(TPI_SYSTEM_CONTROL_DATA_REG, AV_MUTE_MASK, AV_MUTE_MUTED);
        tpi_modify_reg(TPI_HDCP_CONTROL_DATA_REG, PROTECTION_LEVEL_MASK,PROTECTION_LEVEL_MIN);

        hdcp_para.started = false;
        hdcp_para.link_protection_level = EXTENDED_LINK_PROTECTION_NONE | LOCAL_LINK_PROTECTION_NONE;
    }

    OUT_FUNCTION;
}

bool hdcp_get_enable(void)
{
    return hdcp_para.hdcp_enable;
}

void hdcp_set_enable(bool enable)
{
    hdcp_para.hdcp_enable = enable;
}
bool hdcp_get_start(void)
{
    return hdcp_para.started;
}

