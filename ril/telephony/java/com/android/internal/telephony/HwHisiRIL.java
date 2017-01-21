/*
 * Copyright (c) 2012-2013, The Linux Foundation. All rights reserved.
 * Not a Contribution.
 *
 * Copyright (C) 2006 The Android Open Source Project
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
import android.os.Parcel;

import android.telephony.Rlog;
import android.telephony.SignalStrength;

public class HwHisiRIL extends RIL {
    private final boolean DBG = false;

    public HwHisiRIL(Context context, int networkMode, int cdmaSubscription, Integer instanceId) {
        super(context, networkMode, cdmaSubscription, instanceId);
    }

    private static final String RILJ_LOG_TAG = "RILJ-HwHisiRil";

    @Override
    protected Object
    responseFailCause(Parcel p) {
        int numInts;
        int response[];

        numInts = p.readInt();
        response = new int[numInts];
        for (int i = 0 ; i < numInts ; i++) {
            response[i] = p.readInt();
        }
        LastCallFailCause failCause = new LastCallFailCause();
        failCause.causeCode = response[0];
        if (p.dataAvail() > 0) {
          failCause.vendorCause = p.readString();
        }
        return failCause;
    }

    @Override
    protected void
    send(RILRequest rr) {
        if (rr.mRequest >= 132) {
            Rlog.i(RILJ_LOG_TAG, ": Unsupported request " + rr.mRequest);
            rr.onError(REQUEST_NOT_SUPPORTED, null);
            rr.release();
        } else {
            super.send(rr);
        }
    }

    @Override
    protected Object
    responseSignalStrength(Parcel p) {

        int gsmSignalStrength = p.readInt();
        int gsmBitErrorRate = p.readInt();
        int wcdmaRscp = p.readInt();
        int wcdmaEcio = p.readInt();
        int cdmaDbm = p.readInt();
        int cdmaEcio = p.readInt();
        int evdoDbm = p.readInt();
        int evdoEcio = p.readInt();
        int evdoSnr = p.readInt();
        int lteSignalStrength = p.readInt();
        int lteRsrp = p.readInt();
        int lteRsrq = p.readInt();
        int lteRssnr = p.readInt();
        int lteCqi = p.readInt();

        if (gsmSignalStrength != -1) {
            if (DBG) {
                Rlog.i(RILJ_LOG_TAG, ": original gsmSignalStrength: " + gsmSignalStrength);
            }
            gsmSignalStrength = -(gsmSignalStrength - 113) / 2;
        }

        SignalStrength ss = new SignalStrength(
                        wcdmaRscp <= 0 ? gsmSignalStrength : wcdmaRscp,
                        gsmBitErrorRate,
                        cdmaDbm, cdmaEcio,
                        evdoDbm, evdoEcio, evdoSnr,
                        lteSignalStrength, lteRsrp, lteRsrq, lteRssnr, lteCqi,
                        wcdmaRscp, true);

        if (DBG) {
            Rlog.i(RILJ_LOG_TAG, ss.toString() + " " + gsmSignalStrength);
        }

        return ss;
    }
}

