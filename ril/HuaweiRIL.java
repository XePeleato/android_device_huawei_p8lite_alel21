

/*
 * Copyright (C) 2015 The Android Open Source Project
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

import android.os.Parcel;
import android.os.SystemProperties;
import android.content.Context;

import android.telephony.SignalStrength;
import android.telephony.Rlog;

public class HuaweiRIL extends RIL implements CommandsInterface {

    public HuaweiRIL(Context context, int networkMode, int cdmaSubscription, Integer instanceId) {
        super(context, networkMode, cdmaSubscription, instanceId);
    }

    @Override
    protected Object
    responseSignalStrength(Parcel p) {
        int[] response = new int[14];
        for (int i = 0 ; i < 14 ; i++) {
            response[i] = p.readInt();
        }

        int gsmSignalStrength = response[0];
        int gsmBitErrorRate = response[1];
        int mWcdmaRscp = response[2]; // added by huawei
        int mWcdmaEcio = response[3]; // added by huawei
        int cdmaDbm = response[4];
        int cdmaEcio = response[5];
        int evdoDbm = response[6];
        int evdoEcio = response[7];
        int evdoSnr = response[8];
        int lteSignalStrength = response[9];
        int lteRsrp = response[10];
        int lteRsrq = response[11];
        int lteRssnr = response[12];
        int lteCqi = response[13];
        boolean gsmFlag = true;
        int mRat = 0; // added by huawei

        Rlog.e(RILJ_LOG_TAG, "gsmSignalStrength:" + gsmSignalStrength);
        Rlog.e(RILJ_LOG_TAG, "gsmBitErrorRate:" + gsmBitErrorRate);
        Rlog.e(RILJ_LOG_TAG, "mWcdmaRscp:" + mWcdmaRscp);
        Rlog.e(RILJ_LOG_TAG, "mWcdmaEcio:" + mWcdmaEcio);
        Rlog.e(RILJ_LOG_TAG, "cdmaDbm:" + cdmaDbm);
        Rlog.e(RILJ_LOG_TAG, "cdmaEcio:" + cdmaEcio);
        Rlog.e(RILJ_LOG_TAG, "evdoDbm:" + evdoDbm);
        Rlog.e(RILJ_LOG_TAG, "evdoEcio:" + evdoEcio);
        Rlog.e(RILJ_LOG_TAG, "evdoSnr:" + evdoSnr);
        Rlog.e(RILJ_LOG_TAG, "lteRsrp:" + lteRsrp);
        Rlog.e(RILJ_LOG_TAG, "lteRsrq:" + lteRsrq);
        Rlog.e(RILJ_LOG_TAG, "lteRssnr:" + lteRssnr);
        Rlog.e(RILJ_LOG_TAG, "lteCqi:" + lteCqi);
        Rlog.e(RILJ_LOG_TAG, "gsmFlag:" + gsmFlag);

        SignalStrength signalStrength = new SignalStrength(
            gsmSignalStrength, gsmBitErrorRate, cdmaDbm, cdmaEcio, evdoDbm,
            evdoEcio, evdoSnr, lteSignalStrength, lteRsrp, lteRsrq,
            lteRssnr, lteCqi, gsmFlag);
        return signalStrength;
    }
}
