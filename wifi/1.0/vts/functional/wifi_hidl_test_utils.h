/*
 * Copyright (C) 2016 The Android Open Source Project
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

#pragma once

#include <android/hardware/wifi/1.0/IWifi.h>
#include <android/hardware/wifi/1.0/IWifiApIface.h>
#include <android/hardware/wifi/1.0/IWifiChip.h>
#include <android/hardware/wifi/1.0/IWifiNanIface.h>
#include <android/hardware/wifi/1.0/IWifiP2pIface.h>
#include <android/hardware/wifi/1.0/IWifiRttController.h>
#include <android/hardware/wifi/1.0/IWifiStaIface.h>

#include <getopt.h>

#include <VtsHalHidlTargetTestEnvBase.h>
// Helper functions to obtain references to the various HIDL interface objects.
// Note: We only have a single instance of each of these objects currently.
// These helper functions should be modified to return vectors if we support
// multiple instances.
// TODO(b/143892896): Remove the default value as part of the cleanup.
android::sp<android::hardware::wifi::V1_0::IWifi> getWifi(
    const std::string& instance_name = "");
android::sp<android::hardware::wifi::V1_0::IWifiChip> getWifiChip(
    const std::string& instance_name = "");
android::sp<android::hardware::wifi::V1_0::IWifiApIface> getWifiApIface(
    const std::string& instance_name = "");
android::sp<android::hardware::wifi::V1_0::IWifiNanIface> getWifiNanIface(
    const std::string& instance_name = "");
android::sp<android::hardware::wifi::V1_0::IWifiP2pIface> getWifiP2pIface(
    const std::string& instance_name = "");
android::sp<android::hardware::wifi::V1_0::IWifiStaIface> getWifiStaIface(
    const std::string& instance_name = "");
android::sp<android::hardware::wifi::V1_0::IWifiRttController>
getWifiRttController(const std::string& instance_name = "");
// Configure the chip in a mode to support the creation of the provided
// iface type.
bool configureChipToSupportIfaceType(
    const android::sp<android::hardware::wifi::V1_0::IWifiChip>& wifi_chip,
    android::hardware::wifi::V1_0::IfaceType type,
    android::hardware::wifi::V1_0::ChipModeId* configured_mode_id);
// Used to trigger IWifi.stop() at the end of every test.
void stopWifi(const std::string& instance_name = "");

class WifiHidlEnvironment : public ::testing::VtsHalHidlTargetTestEnvBase {
   protected:
    virtual void HidlSetUp() override {
        stopWifi();
        sleep(5);
    }

   public:
    // Whether NaN feature is supported on the device.
    bool isNanOn = false;
    // Whether SoftAp feature is supported on the device.
    bool isSoftApOn = false;

    void usage(char* me, char* arg) {
        fprintf(stderr,
                "unrecognized option: %s\n\n"
                "usage: %s <gtest options> <test options>\n\n"
                "test options are:\n\n"
                "-N, --nan_on: Whether NAN feature is supported\n"
                "-S, --softap_on: Whether SOFTAP feature is supported\n",
                arg, me);
    }

    int initFromOptions(int argc, char** argv) {
        static struct option options[] = {{"nan_on", no_argument, 0, 'N'},
                                          {"softap_on", no_argument, 0, 'S'},
                                          {0, 0, 0, 0}};

        int c;
        while ((c = getopt_long(argc, argv, "NS", options, NULL)) >= 0) {
            switch (c) {
                case 'N':
                    isNanOn = true;
                    break;
                case 'S':
                    isSoftApOn = true;
                    break;
                default:
                    usage(argv[0], argv[optind]);
                    return 2;
            }
        }

        if (optind < argc) {
            usage(argv[0], argv[optind]);
            return 2;
        }

        return 0;
    }
};
