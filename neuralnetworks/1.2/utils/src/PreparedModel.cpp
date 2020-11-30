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

#include "PreparedModel.h"

#include "Callbacks.h"
#include "Conversions.h"
#include "Utils.h"

#include <android/hardware/neuralnetworks/1.0/types.h>
#include <android/hardware/neuralnetworks/1.1/types.h>
#include <android/hardware/neuralnetworks/1.2/IPreparedModel.h>
#include <android/hardware/neuralnetworks/1.2/types.h>
#include <nnapi/IPreparedModel.h>
#include <nnapi/Result.h>
#include <nnapi/Types.h>
#include <nnapi/hal/1.0/Conversions.h>
#include <nnapi/hal/CommonUtils.h>
#include <nnapi/hal/HandleError.h>
#include <nnapi/hal/ProtectCallback.h>

#include <memory>
#include <tuple>
#include <utility>
#include <vector>

namespace android::hardware::neuralnetworks::V1_2::utils {
namespace {

nn::GeneralResult<std::pair<std::vector<nn::OutputShape>, nn::Timing>>
convertExecutionResultsHelper(const hidl_vec<OutputShape>& outputShapes, const Timing& timing) {
    return std::make_pair(NN_TRY(validatedConvertToCanonical(outputShapes)),
                          NN_TRY(validatedConvertToCanonical(timing)));
}

nn::ExecutionResult<std::pair<std::vector<nn::OutputShape>, nn::Timing>> convertExecutionResults(
        const hidl_vec<OutputShape>& outputShapes, const Timing& timing) {
    return hal::utils::makeExecutionFailure(convertExecutionResultsHelper(outputShapes, timing));
}

}  // namespace

nn::GeneralResult<std::shared_ptr<const PreparedModel>> PreparedModel::create(
        sp<V1_2::IPreparedModel> preparedModel) {
    if (preparedModel == nullptr) {
        return NN_ERROR(nn::ErrorStatus::INVALID_ARGUMENT)
               << "V1_2::utils::PreparedModel::create must have non-null preparedModel";
    }

    auto deathHandler = NN_TRY(hal::utils::DeathHandler::create(preparedModel));
    return std::make_shared<const PreparedModel>(PrivateConstructorTag{}, std::move(preparedModel),
                                                 std::move(deathHandler));
}

PreparedModel::PreparedModel(PrivateConstructorTag /*tag*/, sp<V1_2::IPreparedModel> preparedModel,
                             hal::utils::DeathHandler deathHandler)
    : kPreparedModel(std::move(preparedModel)), kDeathHandler(std::move(deathHandler)) {}

nn::ExecutionResult<std::pair<std::vector<nn::OutputShape>, nn::Timing>>
PreparedModel::executeSynchronously(const V1_0::Request& request, MeasureTiming measure) const {
    nn::ExecutionResult<std::pair<std::vector<nn::OutputShape>, nn::Timing>> result =
            NN_ERROR(nn::ErrorStatus::GENERAL_FAILURE) << "uninitialized";
    const auto cb = [&result](V1_0::ErrorStatus status, const hidl_vec<OutputShape>& outputShapes,
                              const Timing& timing) {
        if (status != V1_0::ErrorStatus::NONE) {
            const auto canonical =
                    validatedConvertToCanonical(status).value_or(nn::ErrorStatus::GENERAL_FAILURE);
            result = NN_ERROR(canonical) << "executeSynchronously failed with " << toString(status);
        } else {
            result = convertExecutionResults(outputShapes, timing);
        }
    };

    const auto ret = kPreparedModel->executeSynchronously(request, measure, cb);
    NN_TRY(hal::utils::makeExecutionFailure(hal::utils::handleTransportError(ret)));

    return result;
}

nn::ExecutionResult<std::pair<std::vector<nn::OutputShape>, nn::Timing>>
PreparedModel::executeAsynchronously(const V1_0::Request& request, MeasureTiming measure) const {
    const auto cb = sp<ExecutionCallback>::make();
    const auto scoped = kDeathHandler.protectCallback(cb.get());

    const auto ret = kPreparedModel->execute_1_2(request, measure, cb);
    const auto status =
            NN_TRY(hal::utils::makeExecutionFailure(hal::utils::handleTransportError(ret)));
    if (status != V1_0::ErrorStatus::NONE) {
        const auto canonical =
                validatedConvertToCanonical(status).value_or(nn::ErrorStatus::GENERAL_FAILURE);
        return NN_ERROR(canonical) << "execute failed with " << toString(status);
    }

    return cb->get();
}

nn::ExecutionResult<std::pair<std::vector<nn::OutputShape>, nn::Timing>> PreparedModel::execute(
        const nn::Request& request, nn::MeasureTiming measure,
        const nn::OptionalTimePoint& /*deadline*/,
        const nn::OptionalTimeoutDuration& /*loopTimeoutDuration*/) const {
    // Ensure that request is ready for IPC.
    std::optional<nn::Request> maybeRequestInShared;
    const nn::Request& requestInShared = NN_TRY(hal::utils::makeExecutionFailure(
            hal::utils::flushDataFromPointerToShared(&request, &maybeRequestInShared)));

    const auto hidlRequest =
            NN_TRY(hal::utils::makeExecutionFailure(V1_0::utils::convert(requestInShared)));
    const auto hidlMeasure = NN_TRY(hal::utils::makeExecutionFailure(convert(measure)));

    nn::ExecutionResult<std::pair<std::vector<nn::OutputShape>, nn::Timing>> result =
            NN_ERROR(nn::ErrorStatus::GENERAL_FAILURE) << "uninitialized";
    const bool preferSynchronous = true;

    // Execute synchronously if allowed.
    if (preferSynchronous) {
        result = executeSynchronously(hidlRequest, hidlMeasure);
    }

    // Run asymchronous execution if execution has not already completed.
    if (!result.has_value()) {
        result = executeAsynchronously(hidlRequest, hidlMeasure);
    }

    // Flush output buffers if suxcessful execution.
    if (result.has_value()) {
        NN_TRY(hal::utils::makeExecutionFailure(
                hal::utils::unflushDataFromSharedToPointer(request, maybeRequestInShared)));
    }

    return result;
}

nn::GeneralResult<std::pair<nn::SyncFence, nn::ExecuteFencedInfoCallback>>
PreparedModel::executeFenced(
        const nn::Request& /*request*/, const std::vector<nn::SyncFence>& /*waitFor*/,
        nn::MeasureTiming /*measure*/, const nn::OptionalTimePoint& /*deadline*/,
        const nn::OptionalTimeoutDuration& /*loopTimeoutDuration*/,
        const nn::OptionalTimeoutDuration& /*timeoutDurationAfterFence*/) const {
    return NN_ERROR(nn::ErrorStatus::GENERAL_FAILURE)
           << "IPreparedModel::executeFenced is not supported on 1.2 HAL service";
}

std::any PreparedModel::getUnderlyingResource() const {
    sp<V1_0::IPreparedModel> resource = kPreparedModel;
    return resource;
}

}  // namespace android::hardware::neuralnetworks::V1_2::utils
