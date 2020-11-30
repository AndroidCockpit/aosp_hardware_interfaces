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

#include "Service.h"

#include <android-base/logging.h>
#include <android/hardware/neuralnetworks/1.0/IDevice.h>
#include <android/hardware/neuralnetworks/1.1/IDevice.h>
#include <android/hardware/neuralnetworks/1.2/IDevice.h>
#include <android/hardware/neuralnetworks/1.3/IDevice.h>
#include <android/hidl/manager/1.2/IServiceManager.h>
#include <hidl/ServiceManagement.h>
#include <nnapi/IDevice.h>
#include <nnapi/Result.h>
#include <nnapi/TypeUtils.h>
#include <nnapi/Types.h>
#include <nnapi/hal/1.0/Service.h>
#include <nnapi/hal/1.1/Service.h>
#include <nnapi/hal/1.2/Service.h>
#include <nnapi/hal/1.3/Service.h>

#include <functional>
#include <memory>
#include <string>
#include <type_traits>
#include <unordered_set>
#include <vector>

namespace android::hardware::neuralnetworks::service {
namespace {

using getDeviceFn = std::add_pointer_t<nn::GeneralResult<nn::SharedDevice>(const std::string&)>;

void getDevicesForVersion(const std::string& descriptor, getDeviceFn getDevice,
                          std::vector<nn::SharedDevice>* devices,
                          std::unordered_set<std::string>* registeredDevices) {
    CHECK(devices != nullptr);
    CHECK(registeredDevices != nullptr);

    const auto names = getAllHalInstanceNames(descriptor);
    for (const auto& name : names) {
        if (const auto [it, unregistered] = registeredDevices->insert(name); unregistered) {
            auto maybeDevice = getDevice(name);
            if (maybeDevice.has_value()) {
                auto device = std::move(maybeDevice).value();
                CHECK(device != nullptr);
                devices->push_back(std::move(device));
            } else {
                LOG(ERROR) << "getDevice(" << name << ") failed with " << maybeDevice.error().code
                           << ": " << maybeDevice.error().message;
            }
        }
    }
}

std::vector<nn::SharedDevice> getDevices() {
    std::vector<nn::SharedDevice> devices;
    std::unordered_set<std::string> registeredDevices;

    getDevicesForVersion(V1_3::IDevice::descriptor, &V1_3::utils::getDevice, &devices,
                         &registeredDevices);
    getDevicesForVersion(V1_2::IDevice::descriptor, &V1_2::utils::getDevice, &devices,
                         &registeredDevices);
    getDevicesForVersion(V1_1::IDevice::descriptor, &V1_1::utils::getDevice, &devices,
                         &registeredDevices);
    getDevicesForVersion(V1_0::IDevice::descriptor, &V1_0::utils::getDevice, &devices,
                         &registeredDevices);

    return devices;
}

}  // namespace
}  // namespace android::hardware::neuralnetworks::service

namespace android::nn::hal {

std::vector<nn::SharedDevice> getDevices() {
    return hardware::neuralnetworks::service::getDevices();
}

}  // namespace android::nn::hal
