/*
 * Copyright (C) 2015 The CyanogenMod Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.android.internal.telephony;

import static com.android.internal.telephony.RILConstants.*;

import android.content.Context;

import android.os.Message;
import android.os.Parcel;
import android.os.SystemProperties;
import android.os.AsyncResult;

import android.telephony.Rlog;
import android.telephony.SignalStrength;

import com.android.internal.telephony.uicc.IccIoResult;
import com.android.internal.telephony.dataconnection.ApnProfileOmh;
import com.android.internal.telephony.dataconnection.ApnSetting;
import com.android.internal.telephony.dataconnection.DataProfile;

import android.media.AudioManager;

import java.util.ArrayList;

public class HuaweiRIL extends RIL implements CommandsInterface {

    private AudioManager audioManager;

    public HuaweiRIL(Context context, int networkMode, int cdmaSubscription) {
        this(context, networkMode, cdmaSubscription, null);
    }

    public HuaweiRIL(Context context, int networkMode, int cdmaSubscription, Integer instanceId) {
        super(context, networkMode, cdmaSubscription, instanceId);
        audioManager = (AudioManager)mContext.getSystemService(Context.AUDIO_SERVICE);
    }

    static final boolean RILJ_LOGD = true;
    static final boolean RILJ_LOGV = true; // STOPSHIP if true

    static final int NETWORK_TYPE_TDS = 0x11;
    static final int NETWORK_TYPE_TDS_HSDPA = 0x12; //=> 8
    static final int NETWORK_TYPE_TDS_HSUPA = 0x13; //=> 9

    // RIL_REQUEST
    static final int RIL_REQUEST_GET_POL_CAPABILITY = 0x810;
    static final int RIL_REQUEST_GET_POL_LIST = 0x811;
    static final int RIL_REQUEST_HW_BALONG_BASE = 0x7d0;
    static final int RIL_REQUEST_HW_CGSMS_MESSAGE = 0x7e0;
    static final int RIL_REQUEST_HW_DATA_CONNECTION_ATTACH = 0x7dc;
    static final int RIL_REQUEST_HW_DATA_CONNECTION_DETACH = 0x7db;
    static final int RIL_REQUEST_HW_DEVICE_BASE = 0x1f4;
    static final int RIL_REQUEST_HW_GET_DATA_CALL_PROFILE = 0x1f7;
    static final int RIL_REQUEST_HW_GET_DATA_PROFILE = 0x7dd;
    static final int RIL_REQUEST_HW_GET_DATA_SUBSCRIPTION = 0x1fb;
    static final int RIL_REQUEST_HW_GET_EOPLMN_LIST = 0x7f7;
    static final int RIL_REQUEST_HW_GET_HANDLE_DETECT = 0x7ef;
    static final int RIL_REQUEST_HW_GET_IMEI_VERIFY_STATUS = 0x7e5;
    static final int RIL_REQUEST_HW_GET_ISMCOEX = 0x813;
    static final int RIL_REQUEST_HW_GET_PSDOMAIN_AUTOATTACH_TYPE = 0x7e8;
    static final int RIL_REQUEST_HW_GET_QOS_STATUS = 0x1ff;
    static final int RIL_REQUEST_HW_GET_SIMLOCK_STATUS = 0x81a;
    static final int RIL_REQUEST_HW_GET_SIM_CAPACITY = 0x7de;
    static final int RIL_REQUEST_HW_GET_SIM_SLOT_CFG = 0x7ed;
    static final int RIL_REQUEST_HW_GET_UICC_FILE = 0x824;
    static final int RIL_REQUEST_HW_GET_UICC_SUBSCRIPTION = 0x1fa;
    static final int RIL_REQUEST_HW_GET_USER_SERVICE_STATE = 0x7ea;
    static final int RIL_REQUEST_HW_GET_VOICECALL_BACKGROUND_STATE = 0x7e4;
    static final int RIL_REQUEST_HW_HANDLE_DETECT = 0x7ee;
    static final int RIL_REQUEST_HW_IMS_ADD_CONFERENCE_MEMBER = 0x81c;
    static final int RIL_REQUEST_HW_IMS_ANSWER = 0x806;
    static final int RIL_REQUEST_HW_IMS_CANCEL_USSD = 0x805;
    static final int RIL_REQUEST_HW_IMS_CONFERENCE = 0x801;
    static final int RIL_REQUEST_HW_IMS_DIAL = 0x7fb;
    static final int RIL_REQUEST_HW_IMS_DTMF = 0x807;
    static final int RIL_REQUEST_HW_IMS_DTMF_START = 0x808;
    static final int RIL_REQUEST_HW_IMS_DTMF_STOP = 0x809;
    static final int RIL_REQUEST_HW_IMS_EXPLICIT_CALL_TRANSFER = 0x80a;
    static final int RIL_REQUEST_HW_IMS_GET_CURRENT_CALLS = 0x7fc;
    static final int RIL_REQUEST_HW_IMS_HANGUP = 0x7fd;
    static final int RIL_REQUEST_HW_IMS_HANGUP_FOREGROUND_RESUME_BACKGROUND = 0x7ff;
    static final int RIL_REQUEST_HW_IMS_HANGUP_WAITING_OR_BACKGROUND = 0x7fe;
    static final int RIL_REQUEST_HW_IMS_IMPU = 0x815;
    static final int RIL_REQUEST_HW_IMS_IMSVOPS_IND = 0x80c;
    static final int RIL_REQUEST_HW_IMS_LAST_CALL_FAIL_CAUSE = 0x803;
    static final int RIL_REQUEST_HW_IMS_REGISTRATION_STATE = 0x1f5;
    static final int RIL_REQUEST_HW_IMS_REG_STATE_CHANGE = 0x80b;
    static final int RIL_REQUEST_HW_IMS_SEND_SMS = 0x1f6;
    static final int RIL_REQUEST_HW_IMS_SEND_USSD = 0x804;
    static final int RIL_REQUEST_HW_IMS_SET_CALL_WAITING = 0x81f;
    static final int RIL_REQUEST_HW_IMS_SET_MUTE = 0x80d;
    static final int RIL_REQUEST_HW_IMS_SWITCH_WAITING_OR_HOLDING_AND_ACTIVE = 0x800;
    static final int RIL_REQUEST_HW_IMS_UDUB = 0x802;
    static final int RIL_REQUEST_HW_MODEM_POWER = 0x7d2;
    static final int RIL_REQUEST_HW_MODIFY_CALL_CONFIRM = 0x204;
    static final int RIL_REQUEST_HW_MODIFY_CALL_INITIATE = 0x203;
    static final int RIL_REQUEST_HW_MODIFY_DATA_PROFILE = 0x7da;
    static final int RIL_REQUEST_HW_MODIFY_QOS = 0x200;
    static final int RIL_REQUEST_HW_MONITOR_SIM_IN_SLOT_IND = 0x7eb;
    static final int RIL_REQUEST_HW_QUERY_CARDTYPE = 0x210;
    static final int RIL_REQUEST_HW_QUERY_EMERGENCY_NUMBERS = 0x20a;
    static final int RIL_REQUEST_HW_QUERY_GSM_NMR_INFO = 0x820;
    static final int RIL_REQUEST_HW_RELEASE_QOS = 0x1fe;
    static final int RIL_REQUEST_HW_RESET_ALL_CONNECTIONS = 0x7e1;
    static final int RIL_REQUEST_HW_RESTRAT_RILD = 0x7d5;
    static final int RIL_REQUEST_HW_RESUME_QOS = 0x202;
    static final int RIL_REQUEST_HW_RISE_CDMA_CUTOFF_FREQ = 0x20c;
    static final int RIL_REQUEST_HW_RRC_CONTROL = 0x7e2;
    static final int RIL_REQUEST_HW_SENDAPDU = 0x207;
    static final int RIL_REQUEST_HW_SETUP_QOS = 0x1fd;
    static final int RIL_REQUEST_HW_SET_AUDIO_CHANNEL = 0x208;
    static final int RIL_REQUEST_HW_SET_DATA_SUBSCRIPTION = 0x1f9;
    static final int RIL_REQUEST_HW_SET_EMERGENCY_NUMBERS = 0x7d1;
    static final int RIL_REQUEST_HW_SET_EOPLMN_LIST = 0x7f8;
    static final int RIL_REQUEST_HW_SET_ISMCOEX = 0x814;
    static final int RIL_REQUEST_HW_SET_LONG_MESSAGE = 0x7df;
    static final int RIL_REQUEST_HW_SET_LTE_RELEASE_VERSION = 0x20f;
    static final int RIL_REQUEST_HW_SET_NCELL_MONITOR_STATE = 0x7e9;
    static final int RIL_REQUEST_HW_SET_NETWORK_RAT_AND_SRVDOMAIN_CFG = 0x7e6;
    static final int RIL_REQUEST_HW_SET_POWER_GRADE = 0x206;
    static final int RIL_REQUEST_HW_SET_PSDOMAIN_AUTOATTACH_TYPE = 0x7e7;
    static final int RIL_REQUEST_HW_SET_SIM_LESS = 0x7d3;
    static final int RIL_REQUEST_HW_SET_SIM_SLOT_CFG = 0x7ec;
    static final int RIL_REQUEST_HW_SET_SUBSCRIPTION_MODE = 0x1fc;
    static final int RIL_REQUEST_HW_SET_TRANSMIT_POWER = 0x7d4;
    static final int RIL_REQUEST_HW_SET_UICC_SUBSCRIPTION = 0x1f8;
    static final int RIL_REQUEST_HW_SET_VOICECALL_BACKGROUND_STATE = 0x7e3;
    static final int RIL_REQUEST_HW_SIM_CLOSE_CHANNEL = 0x7d8;
    static final int RIL_REQUEST_HW_SIM_GET_ATR = 0x7f0;
    static final int RIL_REQUEST_HW_SIM_OPEN_CHANNEL = 0x7d7;
    static final int RIL_REQUEST_HW_SIM_TRANSMIT_BASIC = 0x7d6;
    static final int RIL_REQUEST_HW_SIM_TRANSMIT_CHANNEL = 0x7d9;
    static final int RIL_REQUEST_HW_SUSPEND_QOS = 0x201;
    static final int RIL_REQUEST_HW_UICC_AUTH = 0x821;
    static final int RIL_REQUEST_HW_UICC_FILE_UPDATE = 0x823;
    static final int RIL_REQUEST_HW_UICC_GBA_BOOTSTRAP = 0x822;
    static final int RIL_REQUEST_HW_UICC_KS_NAF = 0x825;
    static final int RIL_REQUEST_HW_SET_PCM = 521;
    static final int RIL_REQUEST_SET_POL_ENTRY = 0x812;

    // RIL_UNSOL
    static final int RIL_UNSOL_HW_BALONG_BASE = 0xbb8;
    static final int RIL_UNSOL_HW_BALONG_MODEM_RESET_EVENT = 0xbcf;
    static final int RIL_UNSOL_HW_CG_SWITCH_RECOVERY = 0x5ea;
    static final int RIL_UNSOL_HW_CS_CHANNEL_INFO_IND = 0xbbb;
    static final int RIL_UNSOL_HW_DEVICE_BASE = 0x5dc;
    static final int RIL_UNSOL_HW_DIALUP_STATE_CHANGED = 0x5e6;
    static final int RIL_UNSOL_HW_ECCNUM = 0xbbd;
    static final int RIL_UNSOL_HW_IMS_CALL_RING = 0xbc4;
    static final int RIL_UNSOL_HW_IMS_ON_SS = 0xbc9;
    static final int RIL_UNSOL_HW_IMS_ON_USSD = 0xbc8;
    static final int RIL_UNSOL_HW_PLMN_SEARCH_INFO_IND = 3010;
    static final int RIL_UNSOL_HW_IMS_RESPONSE_CALL_STATE_CHANGED = 0xbc3;
    static final int RIL_UNSOL_HW_IMS_RESPONSE_HANDOVER = 0xbc6;
    static final int RIL_UNSOL_HW_IMS_RINGBACK_TONE = 0xbc5;
    static final int RIL_UNSOL_HW_IMS_SRV_STATUS_UPDATE = 0xbc7;
    static final int RIL_UNSOL_HW_IMS_SUPP_SVC_NOTIFICATION = 0xbca;
    static final int RIL_UNSOL_HW_IMS_VOICE_BAND_INFO = 0xbcb;
    static final int RIL_UNSOL_HW_MODIFY_CALL = 0x5e5;
    static final int RIL_UNSOL_HW_NCELL_MONITOR = 0xbbc;
    static final int RIL_UNSOL_HW_NETWORK_REJECT_CASE = 0xbbe;
    static final int RIL_UNSOL_HW_ON_SS = 0x5e1;
    static final int RIL_UNSOL_HW_QOS_STATE_CHANGED_IND = 0x5e4;
    static final int RIL_UNSOL_HW_RESIDENT_NETWORK_CHANGED = 0xbb9;
    static final int RIL_UNSOL_HW_RESPONSE_DATA_NETWORK_STATE_CHANGED = 0x5e0;
    static final int RIL_UNSOL_HW_RESPONSE_IMS_NETWORK_STATE_CHANGED = 0x5de;
    static final int RIL_UNSOL_HW_RESPONSE_SIMLOCK_STATUS_CHANGED = 0x5e9;
    static final int RIL_UNSOL_HW_RESPONSE_SIM_TYPE = 0x5dd;
    static final int RIL_UNSOL_HW_RESPONSE_TETHERED_MODE_STATE_CHANGED = 0x5df;
    static final int RIL_UNSOL_HW_SIM_PNP = 0xbba;
    static final int RIL_UNSOL_HW_STK_CC_ALPHA_NOTIFY = 0x5e2;
    static final int RIL_UNSOL_HW_TETHERED_MODE_STATE_CHANGED = 0x5eb;
    static final int RIL_UNSOL_HW_TRIGGER_SETUP_DATA_CALL = 0x5ec;
    static final int RIL_UNSOL_HW_UICC_SUBSCRIPTION_STATUS_CHANGED = 0x5e3;
    static final int RIL_UNSOL_HW_UIM_LOCKCARD = 0xbcc;

    static String 
    requestToString(int request) {
        switch (request) {
            case RIL_REQUEST_HW_SET_PCM: return "HW_SET_PCM";
            default: return RIL.requestToString(request);
        }
    }

    static String 
    huaweiResponseToString(int request) {
        switch (request) {
            case RIL_UNSOL_HW_PLMN_SEARCH_INFO_IND: return "HW_PLMN_SEARCH_INFO_IND";
            case RIL_UNSOL_HW_RESIDENT_NETWORK_CHANGED: return "HW_RESIDENT_NETWORK_CHANGED";
            case RIL_UNSOL_HW_ECCNUM: return "HW_ECCNUM";
            case RIL_UNSOL_HW_CS_CHANNEL_INFO_IND: return "HW_CS_CHANNEL_INFO_IND";
            default:  return "<unknown response: "+request+">";
        }
    }

    protected void huaweiUnsljLogRet(int response, Object ret) {
        riljLog("[UNSL]< " + huaweiResponseToString(response) + " " + retToString(response, ret));
    }

    @Override
    protected RILRequest
    processSolicited (Parcel p) {
        int serial, error;
        boolean found = false;

        serial = p.readInt();
        error = p.readInt();

        RILRequest rr;

        rr = findAndRemoveRequestFromList(serial);

        if (rr == null) {
            Rlog.w(RILJ_LOG_TAG, "Unexpected solicited response! sn: "
                            + serial + " error: " + error);
            return null;
        }

        Object ret = null;

        if (error == 0 || p.dataAvail() > 0) {
            // either command succeeds or command fails but with data payload
            try {switch (rr.mRequest) {
            /*
 cat libs/telephony/ril_commands.h \
 | egrep "^ *{RIL_" \
 | sed -re 's/\{([^,]+),[^,]+,([^}]+).+/case \1: ret = \2(p); break;/'
             */
            case RIL_REQUEST_GET_SIM_STATUS: ret =  responseIccCardStatus(p); break;
            case RIL_REQUEST_ENTER_SIM_PIN: ret =  responseInts(p); break;
            case RIL_REQUEST_ENTER_SIM_PUK: ret =  responseInts(p); break;
            case RIL_REQUEST_ENTER_SIM_PIN2: ret =  responseInts(p); break;
            case RIL_REQUEST_ENTER_SIM_PUK2: ret =  responseInts(p); break;
            case RIL_REQUEST_CHANGE_SIM_PIN: ret =  responseInts(p); break;
            case RIL_REQUEST_CHANGE_SIM_PIN2: ret =  responseInts(p); break;
            case RIL_REQUEST_ENTER_DEPERSONALIZATION_CODE: ret =  responseInts(p); break;
            case RIL_REQUEST_GET_CURRENT_CALLS: ret =  responseCallList(p); break;
            case RIL_REQUEST_DIAL: ret =  responseVoid(p); break;
            case RIL_REQUEST_GET_IMSI: ret =  responseString(p); break;
            case RIL_REQUEST_HANGUP: ret =  responseVoid(p); break;
            case RIL_REQUEST_HANGUP_WAITING_OR_BACKGROUND: ret =  responseVoid(p); break;
            case RIL_REQUEST_HANGUP_FOREGROUND_RESUME_BACKGROUND: {
                if (mTestingEmergencyCall.getAndSet(false)) {
                    if (mEmergencyCallbackModeRegistrant != null) {
                        riljLog("testing emergency call, notify ECM Registrants");
                        mEmergencyCallbackModeRegistrant.notifyRegistrant();
                    }
                }
                ret =  responseVoid(p);
                break;
            }
            case RIL_REQUEST_SWITCH_WAITING_OR_HOLDING_AND_ACTIVE: ret =  responseVoid(p); break;
            case RIL_REQUEST_CONFERENCE: ret =  responseVoid(p); break;
            case RIL_REQUEST_UDUB: ret =  responseVoid(p); break;
            case RIL_REQUEST_LAST_CALL_FAIL_CAUSE: ret =  responseInts(p); break;
            case RIL_REQUEST_SIGNAL_STRENGTH: ret =  responseSignalStrength(p); break;
            case RIL_REQUEST_VOICE_REGISTRATION_STATE: ret =  responseStrings(p); break;
            case RIL_REQUEST_DATA_REGISTRATION_STATE: ret =  responseStrings(p); break;
            case RIL_REQUEST_OPERATOR: ret =  responseStrings(p); break;
            case RIL_REQUEST_RADIO_POWER: ret =  responseVoid(p); break;
            case RIL_REQUEST_DTMF: ret =  responseVoid(p); break;
            case RIL_REQUEST_SEND_SMS: ret =  responseSMS(p); break;
            case RIL_REQUEST_SEND_SMS_EXPECT_MORE: ret =  responseSMS(p); break;
            case RIL_REQUEST_SETUP_DATA_CALL: ret =  responseSetupDataCall(p); break;
            case RIL_REQUEST_SIM_IO: ret =  responseICC_IO(p); break;
            case RIL_REQUEST_SEND_USSD: ret =  responseVoid(p); break;
            case RIL_REQUEST_CANCEL_USSD: ret =  responseVoid(p); break;
            case RIL_REQUEST_GET_CLIR: ret =  responseInts(p); break;
            case RIL_REQUEST_SET_CLIR: ret =  responseVoid(p); break;
            case RIL_REQUEST_QUERY_CALL_FORWARD_STATUS: ret =  responseCallForward(p); break;
            case RIL_REQUEST_SET_CALL_FORWARD: ret =  responseVoid(p); break;
            case RIL_REQUEST_QUERY_CALL_WAITING: ret =  responseInts(p); break;
            case RIL_REQUEST_SET_CALL_WAITING: ret =  responseVoid(p); break;
            case RIL_REQUEST_SMS_ACKNOWLEDGE: ret =  responseVoid(p); break;
            case RIL_REQUEST_GET_IMEI: ret =  responseString(p); break;
            case RIL_REQUEST_GET_IMEISV: ret =  responseString(p); break;
            case RIL_REQUEST_ANSWER: ret =  responseVoid(p); break;
            case RIL_REQUEST_DEACTIVATE_DATA_CALL: ret =  responseVoid(p); break;
            case RIL_REQUEST_QUERY_FACILITY_LOCK: ret =  responseInts(p); break;
            case RIL_REQUEST_SET_FACILITY_LOCK: ret =  responseInts(p); break;
            case RIL_REQUEST_CHANGE_BARRING_PASSWORD: ret =  responseVoid(p); break;
            case RIL_REQUEST_QUERY_NETWORK_SELECTION_MODE: ret =  responseInts(p); break;
            case RIL_REQUEST_SET_NETWORK_SELECTION_AUTOMATIC: ret =  responseVoid(p); break;
            case RIL_REQUEST_SET_NETWORK_SELECTION_MANUAL: ret =  responseVoid(p); break;
            case RIL_REQUEST_QUERY_AVAILABLE_NETWORKS : ret =  responseOperatorInfos(p); break;
            case RIL_REQUEST_DTMF_START: ret =  responseVoid(p); break;
            case RIL_REQUEST_DTMF_STOP: ret =  responseVoid(p); break;
            case RIL_REQUEST_BASEBAND_VERSION: ret =  responseString(p); break;
            case RIL_REQUEST_SEPARATE_CONNECTION: ret =  responseVoid(p); break;
            case RIL_REQUEST_SET_MUTE: ret =  responseVoid(p); break;
            case RIL_REQUEST_GET_MUTE: ret =  responseInts(p); break;
            case RIL_REQUEST_QUERY_CLIP: ret =  responseInts(p); break;
            case RIL_REQUEST_LAST_DATA_CALL_FAIL_CAUSE: ret =  responseInts(p); break;
            case RIL_REQUEST_DATA_CALL_LIST: ret =  responseDataCallList(p); break;
            case RIL_REQUEST_RESET_RADIO: ret =  responseVoid(p); break;
            case RIL_REQUEST_OEM_HOOK_RAW: ret =  responseRaw(p); break;
            case RIL_REQUEST_OEM_HOOK_STRINGS: ret =  responseStrings(p); break;
            case RIL_REQUEST_SCREEN_STATE: ret =  responseVoid(p); break;
            case RIL_REQUEST_SET_SUPP_SVC_NOTIFICATION: ret =  responseVoid(p); break;
            case RIL_REQUEST_WRITE_SMS_TO_SIM: ret =  responseInts(p); break;
            case RIL_REQUEST_DELETE_SMS_ON_SIM: ret =  responseVoid(p); break;
            case RIL_REQUEST_SET_BAND_MODE: ret =  responseVoid(p); break;
            case RIL_REQUEST_QUERY_AVAILABLE_BAND_MODE: ret =  responseInts(p); break;
            case RIL_REQUEST_STK_GET_PROFILE: ret =  responseString(p); break;
            case RIL_REQUEST_STK_SET_PROFILE: ret =  responseVoid(p); break;
            case RIL_REQUEST_STK_SEND_ENVELOPE_COMMAND: ret =  responseString(p); break;
            case RIL_REQUEST_STK_SEND_TERMINAL_RESPONSE: ret =  responseVoid(p); break;
            case RIL_REQUEST_STK_HANDLE_CALL_SETUP_REQUESTED_FROM_SIM: ret =  responseInts(p); break;
            case RIL_REQUEST_EXPLICIT_CALL_TRANSFER: ret =  responseVoid(p); break;
            case RIL_REQUEST_SET_PREFERRED_NETWORK_TYPE: ret =  responseVoid(p); break;
            case RIL_REQUEST_GET_PREFERRED_NETWORK_TYPE: ret =  responseGetPreferredNetworkType(p); break;
            case RIL_REQUEST_GET_NEIGHBORING_CELL_IDS: ret = responseCellList(p); break;
            case RIL_REQUEST_SET_LOCATION_UPDATES: ret =  responseVoid(p); break;
            case RIL_REQUEST_CDMA_SET_SUBSCRIPTION_SOURCE: ret =  responseVoid(p); break;
            case RIL_REQUEST_CDMA_SET_ROAMING_PREFERENCE: ret =  responseVoid(p); break;
            case RIL_REQUEST_CDMA_QUERY_ROAMING_PREFERENCE: ret =  responseInts(p); break;
            case RIL_REQUEST_SET_TTY_MODE: ret =  responseVoid(p); break;
            case RIL_REQUEST_QUERY_TTY_MODE: ret =  responseInts(p); break;
            case RIL_REQUEST_CDMA_SET_PREFERRED_VOICE_PRIVACY_MODE: ret =  responseVoid(p); break;
            case RIL_REQUEST_CDMA_QUERY_PREFERRED_VOICE_PRIVACY_MODE: ret =  responseInts(p); break;
            case RIL_REQUEST_CDMA_FLASH: ret =  responseVoid(p); break;
            case RIL_REQUEST_CDMA_BURST_DTMF: ret =  responseVoid(p); break;
            case RIL_REQUEST_CDMA_SEND_SMS: ret =  responseSMS(p); break;
            case RIL_REQUEST_CDMA_SMS_ACKNOWLEDGE: ret =  responseVoid(p); break;
            case RIL_REQUEST_GSM_GET_BROADCAST_CONFIG: ret =  responseGmsBroadcastConfig(p); break;
            case RIL_REQUEST_GSM_SET_BROADCAST_CONFIG: ret =  responseVoid(p); break;
            case RIL_REQUEST_GSM_BROADCAST_ACTIVATION: ret =  responseVoid(p); break;
            case RIL_REQUEST_CDMA_GET_BROADCAST_CONFIG: ret =  responseCdmaBroadcastConfig(p); break;
            case RIL_REQUEST_CDMA_SET_BROADCAST_CONFIG: ret =  responseVoid(p); break;
            case RIL_REQUEST_CDMA_BROADCAST_ACTIVATION: ret =  responseVoid(p); break;
            case RIL_REQUEST_CDMA_VALIDATE_AND_WRITE_AKEY: ret =  responseVoid(p); break;
            case RIL_REQUEST_CDMA_SUBSCRIPTION: ret =  responseStrings(p); break;
            case RIL_REQUEST_CDMA_WRITE_SMS_TO_RUIM: ret =  responseInts(p); break;
            case RIL_REQUEST_CDMA_DELETE_SMS_ON_RUIM: ret =  responseVoid(p); break;
            case RIL_REQUEST_DEVICE_IDENTITY: ret =  responseStrings(p); break;
            case RIL_REQUEST_GET_SMSC_ADDRESS: ret = responseString(p); break;
            case RIL_REQUEST_SET_SMSC_ADDRESS: ret = responseVoid(p); break;
            case RIL_REQUEST_EXIT_EMERGENCY_CALLBACK_MODE: ret = responseVoid(p); break;
            case RIL_REQUEST_REPORT_SMS_MEMORY_STATUS: ret = responseVoid(p); break;
            case RIL_REQUEST_REPORT_STK_SERVICE_IS_RUNNING: ret = responseVoid(p); break;
            case RIL_REQUEST_CDMA_GET_SUBSCRIPTION_SOURCE: ret =  responseInts(p); break;
            case RIL_REQUEST_GET_DATA_CALL_PROFILE: ret =  responseGetDataCallProfile(p); break;
            case RIL_REQUEST_ISIM_AUTHENTICATION: ret =  responseString(p); break;
            case RIL_REQUEST_ACKNOWLEDGE_INCOMING_GSM_SMS_WITH_PDU: ret = responseVoid(p); break;
            case RIL_REQUEST_STK_SEND_ENVELOPE_WITH_STATUS: ret = responseICC_IO(p); break;
            case RIL_REQUEST_VOICE_RADIO_TECH: ret = responseInts(p); break;
            case RIL_REQUEST_GET_CELL_INFO_LIST: ret = responseCellInfoList(p); break;
            case RIL_REQUEST_SET_UNSOL_CELL_INFO_LIST_RATE: ret = responseVoid(p); break;
            case RIL_REQUEST_SET_INITIAL_ATTACH_APN: ret = responseVoid(p); break;
            case RIL_REQUEST_SET_DATA_PROFILE: ret = responseVoid(p); break;
            case RIL_REQUEST_IMS_REGISTRATION_STATE: ret = responseInts(p); break;
            case RIL_REQUEST_IMS_SEND_SMS: ret =  responseSMS(p); break;
            case RIL_REQUEST_SIM_TRANSMIT_APDU_BASIC: ret =  responseICC_IO(p); break;
            case RIL_REQUEST_SIM_OPEN_CHANNEL: ret  = responseInts(p); break;
            case RIL_REQUEST_SIM_CLOSE_CHANNEL: ret  = responseVoid(p); break;
            case RIL_REQUEST_SIM_TRANSMIT_APDU_CHANNEL: ret = responseICC_IO(p); break;
            case RIL_REQUEST_SIM_GET_ATR: ret = responseString(p); break;
            case RIL_REQUEST_NV_READ_ITEM: ret = responseString(p); break;
            case RIL_REQUEST_NV_WRITE_ITEM: ret = responseVoid(p); break;
            case RIL_REQUEST_NV_WRITE_CDMA_PRL: ret = responseVoid(p); break;
            case RIL_REQUEST_NV_RESET_CONFIG: ret = responseVoid(p); break;
            case RIL_REQUEST_SET_UICC_SUBSCRIPTION: ret = responseVoid(p); break;
            case RIL_REQUEST_ALLOW_DATA: ret = responseVoid(p); break;
            case RIL_REQUEST_GET_HARDWARE_CONFIG: ret = responseHardwareConfig(p); break;
            case RIL_REQUEST_SIM_AUTHENTICATION: ret =  responseICC_IOBase64(p); break;
            case RIL_REQUEST_SHUTDOWN: ret = responseVoid(p); break;
            // HUAWEI REQUESTS
            case RIL_REQUEST_HW_SET_PCM: ret = responseVoid(p); break;
            default:
                throw new RuntimeException("Unrecognized solicited response: " + rr.mRequest);
            //break;
            }} catch (Throwable tr) {
                // Exceptions here usually mean invalid RIL responses

                Rlog.w(RILJ_LOG_TAG, rr.serialString() + "< "
                        + requestToString(rr.mRequest)
                        + " exception, possible invalid RIL response", tr);

                if (rr.mResult != null) {
                    AsyncResult.forMessage(rr.mResult, null, tr);
                    rr.mResult.sendToTarget();
                }
                return rr;
            }
        }

        // Here and below fake RIL_UNSOL_RESPONSE_SIM_STATUS_CHANGED, see b/7255789.
        // This is needed otherwise we don't automatically transition to the main lock
        // screen when the pin or puk is entered incorrectly.
        switch (rr.mRequest) {
            case RIL_REQUEST_ENTER_SIM_PUK:
            case RIL_REQUEST_ENTER_SIM_PUK2:
                if (mIccStatusChangedRegistrants != null) {
                    if (RILJ_LOGD) {
                        riljLog("ON enter sim puk fakeSimStatusChanged: reg count="
                                + mIccStatusChangedRegistrants.size());
                    }
                    mIccStatusChangedRegistrants.notifyRegistrants();
                }
                break;
        }

        if (error != 0) {
            switch (rr.mRequest) {
                case RIL_REQUEST_ENTER_SIM_PIN:
                case RIL_REQUEST_ENTER_SIM_PIN2:
                case RIL_REQUEST_CHANGE_SIM_PIN:
                case RIL_REQUEST_CHANGE_SIM_PIN2:
                case RIL_REQUEST_SET_FACILITY_LOCK:
                    if (mIccStatusChangedRegistrants != null) {
                        if (RILJ_LOGD) {
                            riljLog("ON some errors fakeSimStatusChanged: reg count="
                                    + mIccStatusChangedRegistrants.size());
                        }
                        mIccStatusChangedRegistrants.notifyRegistrants();
                    }
                    break;
            }

            rr.onError(error, ret);
        } else {

            if (RILJ_LOGD) riljLog(rr.serialString() + "< " + requestToString(rr.mRequest)
                    + " " + retToString(rr.mRequest, ret));

            if (rr.mResult != null) {
                AsyncResult.forMessage(rr.mResult, ret, null);
                rr.mResult.sendToTarget();
            }
        }
        return rr;
    }

    @Override
    protected void
    processUnsolicited (Parcel p) {
        int response;
        Object ret;
        int dataPosition = p.dataPosition();

        response = p.readInt();

        switch(response) {
        /*
				cat libs/telephony/ril_unsol_commands.h \
				| egrep "^ *{RIL_" \
				| sed -re 's/\{([^,]+),[^,]+,([^}]+).+/case \1: \2(rr, p); break;/'
        */
        case RIL_UNSOL_HW_PLMN_SEARCH_INFO_IND: ret =  responseVoid(p); break;
        case RIL_UNSOL_HW_RESIDENT_NETWORK_CHANGED: ret = responseVoid(p); break;
        case RIL_UNSOL_HW_ECCNUM: ret = responseVoid(p); break;
        case RIL_UNSOL_HW_CS_CHANNEL_INFO_IND: ret = responseInts(p); break;

        default:
            // Rewind the Parcel
            p.setDataPosition(dataPosition);

            // Forward responses that we are not overriding to the super class
            super.processUnsolicited(p);
            return;
        }

        switch(response) {
            case RIL_UNSOL_HW_PLMN_SEARCH_INFO_IND:
                if (RILJ_LOGD) unsljLog(response);
                break;
            case RIL_UNSOL_HW_RESIDENT_NETWORK_CHANGED:
                if (RILJ_LOGD) unsljLog(response);
                break;
            case RIL_UNSOL_HW_ECCNUM:
                if (RILJ_LOGD) unsljLog(response);
                break;
            case RIL_UNSOL_HW_CS_CHANNEL_INFO_IND:
                if (RILJ_LOGD) huaweiUnsljLogRet(response, ret);
                setAmrWb(((int[])ret)[0]);
                break;
        }
    }

    @Override
    protected Object
    responseSignalStrength(Parcel p) {

        Rlog.e(RILJ_LOG_TAG, "responseSignalStrength(): data size = " + p.dataSize());

        int[] response = new int[16];
        for (int i = 0 ; i < 16 ; i++) {
            response[i] = p.readInt();
        }

        int gsmSignalStrength = response[0]; // Valid values are (0-31, 99) as defined in TS 27.007 8.5
        int gsmBitErrorRate = response[1]; // bit error rate (0-7, 99) as defined in TS 27.007 8.5
        int mWcdmaRscp = response[2]; // added by huawei
        int mWcdmaEcio = response[3]; // added by huawei
        int cdmaDbm = response[4];
        int cdmaEcio = response[5];
        int evdoDbm = response[6]; // -75 to -105, 99
        int evdoEcio = response[7];
        int evdoSnr = response[8]; // Valid values are 0-8.  8 is the highest signal to noise ratio
        int lteSignalStrength = response[9]; // 0 to 12, 63
        int lteRsrp = response[10]; // -85 to -140, -44
        int lteRsrq = response[11]; // -3 to -20
        int lteRssnr = response[12]; // 130 to -30, -200
        int lteCqi = response[13];
        int mGsm = response[14];
        int mRat = response[15]; // added by huawei       

        Rlog.e(RILJ_LOG_TAG, "---------- HEX ----------");
        Rlog.e(RILJ_LOG_TAG, "gsmSignalStrength:" + String.format("%x", gsmSignalStrength));
        Rlog.e(RILJ_LOG_TAG, "gsmBitErrorRate:" + String.format("%x", gsmBitErrorRate));
        Rlog.e(RILJ_LOG_TAG, "mWcdmaRscp:" + String.format("%x", mWcdmaRscp));
        Rlog.e(RILJ_LOG_TAG, "mWcdmaEcio:" + String.format("%x", mWcdmaEcio));
        Rlog.e(RILJ_LOG_TAG, "cdmaDbm:" + String.format("%x", cdmaDbm));
        Rlog.e(RILJ_LOG_TAG, "cdmaEcio:" + String.format("%x", cdmaEcio));
        Rlog.e(RILJ_LOG_TAG, "evdoDbm:" + String.format("%x", evdoDbm));
        Rlog.e(RILJ_LOG_TAG, "evdoEcio:" + String.format("%x", evdoEcio));
        Rlog.e(RILJ_LOG_TAG, "evdoSnr:" + String.format("%x", evdoSnr));
        Rlog.e(RILJ_LOG_TAG, "lteSignalStrength:" + String.format("%x", lteSignalStrength));
        Rlog.e(RILJ_LOG_TAG, "lteRsrp:" + String.format("%x", lteRsrp));
        Rlog.e(RILJ_LOG_TAG, "lteRsrq:" + String.format("%x", lteRsrq));
        Rlog.e(RILJ_LOG_TAG, "lteRssnr:" + String.format("%x", lteRssnr));
        Rlog.e(RILJ_LOG_TAG, "lteCqi:" + String.format("%x", lteCqi));
        Rlog.e(RILJ_LOG_TAG, "mGsm:" + String.format("%x", mGsm));
        Rlog.e(RILJ_LOG_TAG, "mRat:" + String.format("%x", mRat));

        if (lteRsrp != 0) // LTE
        {
            if (lteRsrp > -44) lteSignalStrength = 64; // None or Unknown
            else if (lteRsrp >= -85) lteSignalStrength = 63; // Great
            else if (lteRsrp >= -95) lteSignalStrength = 11; // Good
            else if (lteRsrp >= -105) lteSignalStrength = 7; // Moderate
            else if (lteRsrp >= -115) lteSignalStrength = 4; // Poor
            else if (lteRsrp >= -140) lteSignalStrength = 64; // None or Unknown
        }
        else if (gsmSignalStrength == 0 && lteRsrp == 0) // 3G
        {  
            lteRsrp = (mWcdmaRscp & 0xFF) - 256;
            lteRsrq = (mWcdmaEcio & 0xFF) - 256;

            if (lteRsrp > -44) { // None or Unknown
                lteSignalStrength = 64;
                lteRssnr = -200;
            } else if (lteRsrp >= -85) { // Great
                lteSignalStrength = 63;
                lteRssnr = 300;
            } else if (lteRsrp >= -95) { // Good
                lteSignalStrength = 11;
                lteRssnr = 129;
            } else if (lteRsrp >= -105) { // Moderate
                lteSignalStrength = 7;
                lteRssnr = 44;
            } else if (lteRsrp >= -115) { // Poor
                lteSignalStrength = 4;
                lteRssnr = 9;
            } else if (lteRsrp >= -140) { // None or Unknown
                lteSignalStrength = 64;
                lteRssnr = -200;
            }
        }
        else if (mWcdmaRscp == 0 && lteRsrp == 0) // 2G
        {         
            lteRsrp = (gsmSignalStrength & 0xFF) - 256;

            if (lteRsrp > -44) { // None or Unknown
                lteSignalStrength = 64;
                lteRsrq = -21;
                lteRssnr = -200;
            } else if (lteRsrp >= -85) { // Great
                lteSignalStrength = 63;
                lteRsrq = -3;
                lteRssnr = 300;
            } else if (lteRsrp >= -95) { // Good
                lteSignalStrength = 11;
                lteRsrq = -7;
                lteRssnr = 129;
            } else if (lteRsrp >= -105) { // Moderate
                lteSignalStrength = 7;
                lteRsrq = -12;
                lteRssnr = 44;
            } else if (lteRsrp >= -115) { // Poor
                lteSignalStrength = 4;
                lteRsrq = -17;
                lteRssnr = 9;
            } else if (lteRsrp >= -140) { // None or Unknown
                lteSignalStrength = 64;
                lteRsrq = -21;
                lteRssnr = -200;
            }
        }

        gsmSignalStrength = 0;
        gsmBitErrorRate = 0;
        cdmaDbm = -1;
        cdmaEcio = -1;
        evdoDbm = -1;
        evdoEcio = -1;
        evdoSnr = -1;

        Rlog.e(RILJ_LOG_TAG, "---------- MOD ----------");
        Rlog.e(RILJ_LOG_TAG, "lteSignalStrength:" + lteSignalStrength);
        Rlog.e(RILJ_LOG_TAG, "lteRsrp:" + lteRsrp);
        Rlog.e(RILJ_LOG_TAG, "lteRsrq:" + lteRsrq);
        Rlog.e(RILJ_LOG_TAG, "lteRssnr:" + lteRssnr);
        Rlog.e(RILJ_LOG_TAG, "lteCqi:" + lteCqi);
        Rlog.e(RILJ_LOG_TAG, "-------------------------");

        SignalStrength signalStrength = new SignalStrength(
            gsmSignalStrength, gsmBitErrorRate, cdmaDbm, cdmaEcio, evdoDbm, 
            evdoEcio, evdoSnr, lteSignalStrength, -lteRsrp, -lteRsrq, 
            lteRssnr, lteCqi, true);

        return signalStrength;
    }

    protected ArrayList<ApnSetting> responseGetDataCallProfile(Parcel p) {
        int nProfiles = p.readInt();
        if (RILJ_LOGD) riljLog("# data call profiles:" + nProfiles);

        ArrayList<ApnSetting> response = new ArrayList<ApnSetting>(nProfiles);

        int profileId = 0;
        int priority = 0;
        for (int i = 0; i < nProfiles; i++) {
            profileId = p.readInt();
            priority = p.readInt();
            ApnProfileOmh profile = new ApnProfileOmh(profileId, priority);
            if (RILJ_LOGD) {
                riljLog("responseGetDataCallProfile()" +
                        profile.getProfileId() + ":" + profile.getPriority());
            }
            response.add(profile);
        }

        return response;
    }

   protected Object
   responseHardwareConfig(Parcel p) {
      int num;
      ArrayList<HardwareConfig> response;
      HardwareConfig hw;

      num = p.readInt();
      response = new ArrayList<HardwareConfig>(num);

      if (RILJ_LOGV) {
         riljLog("responseHardwareConfig: num=" + num);
      }
      for (int i = 0 ; i < num ; i++) {
         int type = p.readInt();
         switch(type) {
            case HardwareConfig.DEV_HARDWARE_TYPE_MODEM: {
               hw = new HardwareConfig(type);
               hw.assignModem(p.readString(), p.readInt(), p.readInt(),
                  p.readInt(), p.readInt(), p.readInt(), p.readInt());
               break;
            }
            case HardwareConfig.DEV_HARDWARE_TYPE_SIM: {
               hw = new HardwareConfig(type);
               hw.assignSim(p.readString(), p.readInt(), p.readString());
               break;
            }
            default: {
               throw new RuntimeException(
                  "RIL_REQUEST_GET_HARDWARE_CONFIG invalid hardward type:" + type);
            }
         }

         response.add(hw);
      }

      return response;
   }

    protected Object
    responseICC_IOBase64(Parcel p) {
        int sw1, sw2;
        Message ret;

        sw1 = p.readInt();
        sw2 = p.readInt();

        String s = p.readString();

        if (RILJ_LOGV) riljLog("< iccIO: "
                + " 0x" + Integer.toHexString(sw1)
                + " 0x" + Integer.toHexString(sw2) + " "
                + s);


        return new IccIoResult(sw1, sw2, android.util.Base64.decode(s, android.util.Base64.DEFAULT));
    }

    /**
     * Set audio parameter "incall_wb" for HD-Voice (Wideband AMR).
     *
     * @param state: 1 = unsupported, 2 = supported.
     */
    private void setAmrWb(int state) {
        if (state == 2) {
            Rlog.d(RILJ_LOG_TAG, "setAmrWb(): setting audio parameter - incall_wb=on");
            audioManager.setParameters("incall_wb=on");
        } else {
            Rlog.d(RILJ_LOG_TAG, "setAmrWb(): setting audio parameter - incall_wb=off");
            audioManager.setParameters("incall_wb=off");
        }
    }

    public void setModemPcm(boolean on, Message result) {
        RILRequest rr;
        
        rr = RILRequest.obtain(RIL_REQUEST_HW_SET_PCM, result);

        rr.mParcel.writeInt(1);
        rr.mParcel.writeInt(on ? 1 : 0);

        if (RILJ_LOGD) riljLog(rr.serialString() + "> " + requestToString(rr.mRequest));

        send(rr);
    }
}
