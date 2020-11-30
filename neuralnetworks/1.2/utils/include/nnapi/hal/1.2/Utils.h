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

#ifndef ANDROID_HARDWARE_INTERFACES_NEURALNETWORKS_1_2_UTILS_H
#define ANDROID_HARDWARE_INTERFACES_NEURALNETWORKS_1_2_UTILS_H

#include "nnapi/hal/1.2/Conversions.h"

#include <android-base/logging.h>
#include <android/hardware/neuralnetworks/1.2/types.h>
#include <nnapi/Result.h>
#include <nnapi/TypeUtils.h>
#include <nnapi/Types.h>
#include <nnapi/Validation.h>
#include <nnapi/hal/1.0/Conversions.h>
#include <nnapi/hal/1.1/Conversions.h>

#include <limits>

namespace android::hardware::neuralnetworks::V1_2::utils {

constexpr auto kDefaultMesaureTiming = MeasureTiming::NO;
constexpr auto kNoTiming = Timing{.timeOnDevice = std::numeric_limits<uint64_t>::max(),
                                  .timeInDriver = std::numeric_limits<uint64_t>::max()};
constexpr auto kVersion = nn::Version::ANDROID_Q;

template <typename Type>
nn::Result<void> validate(const Type& halObject) {
    const auto maybeCanonical = nn::convert(halObject);
    if (!maybeCanonical.has_value()) {
        return nn::error() << maybeCanonical.error().message;
    }
    const auto version = NN_TRY(nn::validate(maybeCanonical.value()));
    if (version > utils::kVersion) {
        return NN_ERROR() << "Insufficient version: " << version << " vs required "
                          << utils::kVersion;
    }
    return {};
}

template <typename Type>
bool valid(const Type& halObject) {
    const auto result = utils::validate(halObject);
    if (!result.has_value()) {
        LOG(ERROR) << result.error();
    }
    return result.has_value();
}

template <typename Type>
decltype(nn::convert(std::declval<Type>())) validatedConvertToCanonical(const Type& halObject) {
    auto canonical = NN_TRY(nn::convert(halObject));
    const auto maybeVersion = nn::validate(canonical);
    if (!maybeVersion.has_value()) {
        return nn::error() << maybeVersion.error();
    }
    const auto version = maybeVersion.value();
    if (version > utils::kVersion) {
        return NN_ERROR() << "Insufficient version: " << version << " vs required "
                          << utils::kVersion;
    }
    return canonical;
}

}  // namespace android::hardware::neuralnetworks::V1_2::utils

#endif  // ANDROID_HARDWARE_INTERFACES_NEURALNETWORKS_1_2_UTILS_H
