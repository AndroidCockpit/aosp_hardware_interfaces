/*
 * Copyright (C) 2020 The Android Open Source Project
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

#ifndef AUDIO_POLICY_CONFIGURATION_V7_0_ENUMS_H
#define AUDIO_POLICY_CONFIGURATION_V7_0_ENUMS_H

#include <sys/types.h>

#include <audio_policy_configuration_V7_0.h>

namespace audio::policy::configuration::V7_0 {

static inline size_t getChannelCount(AudioChannelMask mask) {
    switch (mask) {
        case AudioChannelMask::AUDIO_CHANNEL_NONE:
            return 0;
        case AudioChannelMask::AUDIO_CHANNEL_OUT_MONO:
        case AudioChannelMask::AUDIO_CHANNEL_IN_MONO:
        case AudioChannelMask::AUDIO_CHANNEL_INDEX_MASK_1:
            return 1;
        case AudioChannelMask::AUDIO_CHANNEL_OUT_STEREO:
        case AudioChannelMask::AUDIO_CHANNEL_OUT_MONO_HAPTIC_A:
        case AudioChannelMask::AUDIO_CHANNEL_OUT_HAPTIC_AB:
        case AudioChannelMask::AUDIO_CHANNEL_IN_STEREO:
        case AudioChannelMask::AUDIO_CHANNEL_IN_FRONT_BACK:
        case AudioChannelMask::AUDIO_CHANNEL_IN_VOICE_UPLINK_MONO:
        case AudioChannelMask::AUDIO_CHANNEL_IN_VOICE_DNLINK_MONO:
        case AudioChannelMask::AUDIO_CHANNEL_IN_VOICE_CALL_MONO:
        case AudioChannelMask::AUDIO_CHANNEL_INDEX_MASK_2:
            return 2;
        case AudioChannelMask::AUDIO_CHANNEL_OUT_2POINT1:
        case AudioChannelMask::AUDIO_CHANNEL_OUT_STEREO_HAPTIC_A:
        case AudioChannelMask::AUDIO_CHANNEL_OUT_MONO_HAPTIC_AB:
        case AudioChannelMask::AUDIO_CHANNEL_OUT_TRI:
        case AudioChannelMask::AUDIO_CHANNEL_OUT_TRI_BACK:
        case AudioChannelMask::AUDIO_CHANNEL_INDEX_MASK_3:
            return 3;
        case AudioChannelMask::AUDIO_CHANNEL_OUT_2POINT0POINT2:
        case AudioChannelMask::AUDIO_CHANNEL_OUT_3POINT1:
        case AudioChannelMask::AUDIO_CHANNEL_OUT_QUAD:
        case AudioChannelMask::AUDIO_CHANNEL_OUT_QUAD_BACK:
        case AudioChannelMask::AUDIO_CHANNEL_OUT_QUAD_SIDE:
        case AudioChannelMask::AUDIO_CHANNEL_OUT_SURROUND:
        case AudioChannelMask::AUDIO_CHANNEL_OUT_STEREO_HAPTIC_AB:
        case AudioChannelMask::AUDIO_CHANNEL_IN_2POINT0POINT2:
        case AudioChannelMask::AUDIO_CHANNEL_INDEX_MASK_4:
            return 4;
        case AudioChannelMask::AUDIO_CHANNEL_OUT_2POINT1POINT2:
        case AudioChannelMask::AUDIO_CHANNEL_OUT_3POINT0POINT2:
        case AudioChannelMask::AUDIO_CHANNEL_OUT_PENTA:
        case AudioChannelMask::AUDIO_CHANNEL_IN_2POINT1POINT2:
        case AudioChannelMask::AUDIO_CHANNEL_IN_3POINT0POINT2:
        case AudioChannelMask::AUDIO_CHANNEL_INDEX_MASK_5:
            return 5;
        case AudioChannelMask::AUDIO_CHANNEL_OUT_3POINT1POINT2:
        case AudioChannelMask::AUDIO_CHANNEL_OUT_5POINT1:
        case AudioChannelMask::AUDIO_CHANNEL_OUT_5POINT1_BACK:
        case AudioChannelMask::AUDIO_CHANNEL_OUT_5POINT1_SIDE:
        case AudioChannelMask::AUDIO_CHANNEL_IN_6:
        case AudioChannelMask::AUDIO_CHANNEL_IN_3POINT1POINT2:
        case AudioChannelMask::AUDIO_CHANNEL_IN_5POINT1:
        case AudioChannelMask::AUDIO_CHANNEL_INDEX_MASK_6:
            return 6;
        case AudioChannelMask::AUDIO_CHANNEL_OUT_6POINT1:
        case AudioChannelMask::AUDIO_CHANNEL_INDEX_MASK_7:
            return 7;
        case AudioChannelMask::AUDIO_CHANNEL_OUT_5POINT1POINT2:
        case AudioChannelMask::AUDIO_CHANNEL_OUT_7POINT1:
        case AudioChannelMask::AUDIO_CHANNEL_INDEX_MASK_8:
            return 8;
        case AudioChannelMask::AUDIO_CHANNEL_INDEX_MASK_9:
            return 9;
        case AudioChannelMask::AUDIO_CHANNEL_OUT_5POINT1POINT4:
        case AudioChannelMask::AUDIO_CHANNEL_OUT_7POINT1POINT2:
        case AudioChannelMask::AUDIO_CHANNEL_INDEX_MASK_10:
            return 10;
        case AudioChannelMask::AUDIO_CHANNEL_INDEX_MASK_11:
            return 11;
        case AudioChannelMask::AUDIO_CHANNEL_OUT_7POINT1POINT4:
        case AudioChannelMask::AUDIO_CHANNEL_INDEX_MASK_12:
            return 12;
        case AudioChannelMask::AUDIO_CHANNEL_INDEX_MASK_13:
            return 13;
        case AudioChannelMask::AUDIO_CHANNEL_INDEX_MASK_14:
            return 14;
        case AudioChannelMask::AUDIO_CHANNEL_INDEX_MASK_15:
            return 15;
        case AudioChannelMask::AUDIO_CHANNEL_INDEX_MASK_16:
            return 16;
        case AudioChannelMask::AUDIO_CHANNEL_INDEX_MASK_17:
            return 17;
        case AudioChannelMask::AUDIO_CHANNEL_INDEX_MASK_18:
            return 18;
        case AudioChannelMask::AUDIO_CHANNEL_INDEX_MASK_19:
            return 19;
        case AudioChannelMask::AUDIO_CHANNEL_INDEX_MASK_20:
            return 20;
        case AudioChannelMask::AUDIO_CHANNEL_INDEX_MASK_21:
            return 21;
        case AudioChannelMask::AUDIO_CHANNEL_INDEX_MASK_22:
            return 22;
        case AudioChannelMask::AUDIO_CHANNEL_INDEX_MASK_23:
            return 23;
        case AudioChannelMask::AUDIO_CHANNEL_INDEX_MASK_24:
            return 24;
        case AudioChannelMask::UNKNOWN:
            return 0;
            // No default to make sure all cases are covered.
    }
    // This is to avoid undefined behavior if 'mask' isn't a valid enum value.
    return 0;
}

static inline ssize_t getChannelCount(const std::string& mask) {
    return getChannelCount(stringToAudioChannelMask(mask));
}

static inline bool isOutputDevice(AudioDevice device) {
    switch (device) {
        case AudioDevice::UNKNOWN:
        case AudioDevice::AUDIO_DEVICE_NONE:
            return false;
        case AudioDevice::AUDIO_DEVICE_OUT_EARPIECE:
        case AudioDevice::AUDIO_DEVICE_OUT_SPEAKER:
        case AudioDevice::AUDIO_DEVICE_OUT_WIRED_HEADSET:
        case AudioDevice::AUDIO_DEVICE_OUT_WIRED_HEADPHONE:
        case AudioDevice::AUDIO_DEVICE_OUT_BLUETOOTH_SCO:
        case AudioDevice::AUDIO_DEVICE_OUT_BLUETOOTH_SCO_HEADSET:
        case AudioDevice::AUDIO_DEVICE_OUT_BLUETOOTH_SCO_CARKIT:
        case AudioDevice::AUDIO_DEVICE_OUT_BLUETOOTH_A2DP:
        case AudioDevice::AUDIO_DEVICE_OUT_BLUETOOTH_A2DP_HEADPHONES:
        case AudioDevice::AUDIO_DEVICE_OUT_BLUETOOTH_A2DP_SPEAKER:
        case AudioDevice::AUDIO_DEVICE_OUT_AUX_DIGITAL:
        case AudioDevice::AUDIO_DEVICE_OUT_HDMI:
        case AudioDevice::AUDIO_DEVICE_OUT_ANLG_DOCK_HEADSET:
        case AudioDevice::AUDIO_DEVICE_OUT_DGTL_DOCK_HEADSET:
        case AudioDevice::AUDIO_DEVICE_OUT_USB_ACCESSORY:
        case AudioDevice::AUDIO_DEVICE_OUT_USB_DEVICE:
        case AudioDevice::AUDIO_DEVICE_OUT_REMOTE_SUBMIX:
        case AudioDevice::AUDIO_DEVICE_OUT_TELEPHONY_TX:
        case AudioDevice::AUDIO_DEVICE_OUT_LINE:
        case AudioDevice::AUDIO_DEVICE_OUT_HDMI_ARC:
        case AudioDevice::AUDIO_DEVICE_OUT_SPDIF:
        case AudioDevice::AUDIO_DEVICE_OUT_FM:
        case AudioDevice::AUDIO_DEVICE_OUT_AUX_LINE:
        case AudioDevice::AUDIO_DEVICE_OUT_SPEAKER_SAFE:
        case AudioDevice::AUDIO_DEVICE_OUT_IP:
        case AudioDevice::AUDIO_DEVICE_OUT_BUS:
        case AudioDevice::AUDIO_DEVICE_OUT_PROXY:
        case AudioDevice::AUDIO_DEVICE_OUT_USB_HEADSET:
        case AudioDevice::AUDIO_DEVICE_OUT_HEARING_AID:
        case AudioDevice::AUDIO_DEVICE_OUT_ECHO_CANCELLER:
        case AudioDevice::AUDIO_DEVICE_OUT_BLE_HEADSET:
        case AudioDevice::AUDIO_DEVICE_OUT_BLE_SPEAKER:
        case AudioDevice::AUDIO_DEVICE_OUT_DEFAULT:
        case AudioDevice::AUDIO_DEVICE_OUT_STUB:
            return true;
        case AudioDevice::AUDIO_DEVICE_IN_COMMUNICATION:
        case AudioDevice::AUDIO_DEVICE_IN_AMBIENT:
        case AudioDevice::AUDIO_DEVICE_IN_BUILTIN_MIC:
        case AudioDevice::AUDIO_DEVICE_IN_BLUETOOTH_SCO_HEADSET:
        case AudioDevice::AUDIO_DEVICE_IN_WIRED_HEADSET:
        case AudioDevice::AUDIO_DEVICE_IN_AUX_DIGITAL:
        case AudioDevice::AUDIO_DEVICE_IN_HDMI:
        case AudioDevice::AUDIO_DEVICE_IN_VOICE_CALL:
        case AudioDevice::AUDIO_DEVICE_IN_TELEPHONY_RX:
        case AudioDevice::AUDIO_DEVICE_IN_BACK_MIC:
        case AudioDevice::AUDIO_DEVICE_IN_REMOTE_SUBMIX:
        case AudioDevice::AUDIO_DEVICE_IN_ANLG_DOCK_HEADSET:
        case AudioDevice::AUDIO_DEVICE_IN_DGTL_DOCK_HEADSET:
        case AudioDevice::AUDIO_DEVICE_IN_USB_ACCESSORY:
        case AudioDevice::AUDIO_DEVICE_IN_USB_DEVICE:
        case AudioDevice::AUDIO_DEVICE_IN_FM_TUNER:
        case AudioDevice::AUDIO_DEVICE_IN_TV_TUNER:
        case AudioDevice::AUDIO_DEVICE_IN_LINE:
        case AudioDevice::AUDIO_DEVICE_IN_SPDIF:
        case AudioDevice::AUDIO_DEVICE_IN_BLUETOOTH_A2DP:
        case AudioDevice::AUDIO_DEVICE_IN_LOOPBACK:
        case AudioDevice::AUDIO_DEVICE_IN_IP:
        case AudioDevice::AUDIO_DEVICE_IN_BUS:
        case AudioDevice::AUDIO_DEVICE_IN_PROXY:
        case AudioDevice::AUDIO_DEVICE_IN_USB_HEADSET:
        case AudioDevice::AUDIO_DEVICE_IN_BLUETOOTH_BLE:
        case AudioDevice::AUDIO_DEVICE_IN_HDMI_ARC:
        case AudioDevice::AUDIO_DEVICE_IN_ECHO_REFERENCE:
        case AudioDevice::AUDIO_DEVICE_IN_BLE_HEADSET:
        case AudioDevice::AUDIO_DEVICE_IN_DEFAULT:
        case AudioDevice::AUDIO_DEVICE_IN_STUB:
            return false;
            // No default to make sure all cases are covered.
    }
    // This is to avoid undefined behavior if 'device' isn't a valid enum value.
    return false;
}

static inline bool isOutputDevice(const std::string& device) {
    return isOutputDevice(stringToAudioDevice(device));
}

}  // namespace audio::policy::configuration::V7_0

#endif  // AUDIO_POLICY_CONFIGURATION_V7_0_ENUMS_H
