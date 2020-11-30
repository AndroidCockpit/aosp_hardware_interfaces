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

#include "CommonUtils.h"

#include "HandleError.h"

#include <android-base/logging.h>
#include <android-base/unique_fd.h>
#include <nnapi/Result.h>
#include <nnapi/SharedMemory.h>
#include <nnapi/TypeUtils.h>
#include <nnapi/Types.h>
#include <nnapi/Validation.h>

#include <algorithm>
#include <any>
#include <functional>
#include <optional>
#include <variant>
#include <vector>

namespace android::hardware::neuralnetworks::utils {
namespace {

bool hasNoPointerData(const nn::Operand& operand);
bool hasNoPointerData(const nn::Model::Subgraph& subgraph);
bool hasNoPointerData(const nn::Request::Argument& argument);

template <typename Type>
bool hasNoPointerData(const std::vector<Type>& objects) {
    return std::all_of(objects.begin(), objects.end(),
                       [](const auto& object) { return hasNoPointerData(object); });
}

bool hasNoPointerData(const nn::DataLocation& location) {
    return std::visit([](auto ptr) { return ptr == nullptr; }, location.pointer);
}

bool hasNoPointerData(const nn::Operand& operand) {
    return hasNoPointerData(operand.location);
}

bool hasNoPointerData(const nn::Model::Subgraph& subgraph) {
    return hasNoPointerData(subgraph.operands);
}

bool hasNoPointerData(const nn::Request::Argument& argument) {
    return hasNoPointerData(argument.location);
}

void copyPointersToSharedMemory(nn::Operand* operand, nn::ConstantMemoryBuilder* memoryBuilder) {
    CHECK(operand != nullptr);
    CHECK(memoryBuilder != nullptr);

    if (operand->lifetime != nn::Operand::LifeTime::POINTER) {
        return;
    }

    const void* data = std::visit([](auto ptr) { return static_cast<const void*>(ptr); },
                                  operand->location.pointer);
    CHECK(data != nullptr);
    operand->lifetime = nn::Operand::LifeTime::CONSTANT_REFERENCE;
    operand->location = memoryBuilder->append(data, operand->location.length);
}

void copyPointersToSharedMemory(nn::Model::Subgraph* subgraph,
                                nn::ConstantMemoryBuilder* memoryBuilder) {
    CHECK(subgraph != nullptr);
    std::for_each(subgraph->operands.begin(), subgraph->operands.end(),
                  [memoryBuilder](auto& operand) {
                      copyPointersToSharedMemory(&operand, memoryBuilder);
                  });
}

}  // anonymous namespace

nn::Capabilities::OperandPerformanceTable makeQuantized8PerformanceConsistentWithP(
        const nn::Capabilities::PerformanceInfo& float32Performance,
        const nn::Capabilities::PerformanceInfo& quantized8Performance) {
    // In Android P, most data types are treated as having the same performance as
    // TENSOR_QUANT8_ASYMM. This collection must be in sorted order.
    std::vector<nn::Capabilities::OperandPerformance> operandPerformances = {
            {.type = nn::OperandType::FLOAT32, .info = float32Performance},
            {.type = nn::OperandType::INT32, .info = quantized8Performance},
            {.type = nn::OperandType::UINT32, .info = quantized8Performance},
            {.type = nn::OperandType::TENSOR_FLOAT32, .info = float32Performance},
            {.type = nn::OperandType::TENSOR_INT32, .info = quantized8Performance},
            {.type = nn::OperandType::TENSOR_QUANT8_ASYMM, .info = quantized8Performance},
            {.type = nn::OperandType::OEM, .info = quantized8Performance},
            {.type = nn::OperandType::TENSOR_OEM_BYTE, .info = quantized8Performance},
    };
    return nn::Capabilities::OperandPerformanceTable::create(std::move(operandPerformances))
            .value();
}

bool hasNoPointerData(const nn::Model& model) {
    return hasNoPointerData(model.main) && hasNoPointerData(model.referenced);
}

bool hasNoPointerData(const nn::Request& request) {
    return hasNoPointerData(request.inputs) && hasNoPointerData(request.outputs);
}

nn::GeneralResult<std::reference_wrapper<const nn::Model>> flushDataFromPointerToShared(
        const nn::Model* model, std::optional<nn::Model>* maybeModelInSharedOut) {
    CHECK(model != nullptr);
    CHECK(maybeModelInSharedOut != nullptr);

    if (hasNoPointerData(*model)) {
        return *model;
    }

    // Make a copy of the model in order to make modifications. The modified model is returned to
    // the caller through `maybeModelInSharedOut` if the function succeeds.
    nn::Model modelInShared = *model;

    nn::ConstantMemoryBuilder memoryBuilder(modelInShared.pools.size());
    copyPointersToSharedMemory(&modelInShared.main, &memoryBuilder);
    std::for_each(modelInShared.referenced.begin(), modelInShared.referenced.end(),
                  [&memoryBuilder](auto& subgraph) {
                      copyPointersToSharedMemory(&subgraph, &memoryBuilder);
                  });

    if (!memoryBuilder.empty()) {
        auto memory = NN_TRY(memoryBuilder.finish());
        modelInShared.pools.push_back(std::move(memory));
    }

    *maybeModelInSharedOut = modelInShared;
    return **maybeModelInSharedOut;
}

nn::GeneralResult<std::reference_wrapper<const nn::Request>> flushDataFromPointerToShared(
        const nn::Request* request, std::optional<nn::Request>* maybeRequestInSharedOut) {
    CHECK(request != nullptr);
    CHECK(maybeRequestInSharedOut != nullptr);

    if (hasNoPointerData(*request)) {
        return *request;
    }

    // Make a copy of the request in order to make modifications. The modified request is returned
    // to the caller through `maybeRequestInSharedOut` if the function succeeds.
    nn::Request requestInShared = *request;

    // Change input pointers to shared memory.
    nn::ConstantMemoryBuilder inputBuilder(requestInShared.pools.size());
    for (auto& input : requestInShared.inputs) {
        const auto& location = input.location;
        if (input.lifetime != nn::Request::Argument::LifeTime::POINTER) {
            continue;
        }

        input.lifetime = nn::Request::Argument::LifeTime::POOL;
        const void* data = std::visit([](auto ptr) { return static_cast<const void*>(ptr); },
                                      location.pointer);
        CHECK(data != nullptr);
        input.location = inputBuilder.append(data, location.length);
    }

    // Allocate input memory.
    if (!inputBuilder.empty()) {
        auto memory = NN_TRY(inputBuilder.finish());
        requestInShared.pools.push_back(std::move(memory));
    }

    // Change output pointers to shared memory.
    nn::MutableMemoryBuilder outputBuilder(requestInShared.pools.size());
    for (auto& output : requestInShared.outputs) {
        const auto& location = output.location;
        if (output.lifetime != nn::Request::Argument::LifeTime::POINTER) {
            continue;
        }

        output.lifetime = nn::Request::Argument::LifeTime::POOL;
        output.location = outputBuilder.append(location.length);
    }

    // Allocate output memory.
    if (!outputBuilder.empty()) {
        auto memory = NN_TRY(outputBuilder.finish());
        requestInShared.pools.push_back(std::move(memory));
    }

    *maybeRequestInSharedOut = requestInShared;
    return **maybeRequestInSharedOut;
}

nn::GeneralResult<void> unflushDataFromSharedToPointer(
        const nn::Request& request, const std::optional<nn::Request>& maybeRequestInShared) {
    if (!maybeRequestInShared.has_value() || maybeRequestInShared->pools.empty() ||
        !std::holds_alternative<nn::Memory>(maybeRequestInShared->pools.back())) {
        return {};
    }
    const auto& requestInShared = *maybeRequestInShared;

    // Map the memory.
    const auto& outputMemory = std::get<nn::Memory>(requestInShared.pools.back());
    const auto [pointer, size, context] = NN_TRY(map(outputMemory));
    const uint8_t* constantPointer =
            std::visit([](const auto& o) { return static_cast<const uint8_t*>(o); }, pointer);

    // Flush each output pointer.
    CHECK_EQ(request.outputs.size(), requestInShared.outputs.size());
    for (size_t i = 0; i < request.outputs.size(); ++i) {
        const auto& location = request.outputs[i].location;
        const auto& locationInShared = requestInShared.outputs[i].location;
        if (!std::holds_alternative<void*>(location.pointer)) {
            continue;
        }

        // Get output pointer and size.
        void* data = std::get<void*>(location.pointer);
        CHECK(data != nullptr);
        const size_t length = location.length;

        // Get output pool location.
        CHECK(requestInShared.outputs[i].lifetime == nn::Request::Argument::LifeTime::POOL);
        const size_t index = locationInShared.poolIndex;
        const size_t offset = locationInShared.offset;
        const size_t outputPoolIndex = requestInShared.pools.size() - 1;
        CHECK(locationInShared.length == length);
        CHECK(index == outputPoolIndex);

        // Flush memory.
        std::memcpy(data, constantPointer + offset, length);
    }

    return {};
}

std::vector<uint32_t> countNumberOfConsumers(size_t numberOfOperands,
                                             const std::vector<nn::Operation>& operations) {
    return nn::countNumberOfConsumers(numberOfOperands, operations);
}

nn::GeneralResult<hidl_handle> hidlHandleFromSharedHandle(const nn::SharedHandle& handle) {
    if (handle == nullptr) {
        return {};
    }

    std::vector<base::unique_fd> fds;
    fds.reserve(handle->fds.size());
    for (const auto& fd : handle->fds) {
        int dupFd = dup(fd);
        if (dupFd == -1) {
            return NN_ERROR(nn::ErrorStatus::GENERAL_FAILURE) << "Failed to dup the fd";
        }
        fds.emplace_back(dupFd);
    }

    native_handle_t* nativeHandle = native_handle_create(handle->fds.size(), handle->ints.size());
    if (nativeHandle == nullptr) {
        return NN_ERROR(nn::ErrorStatus::GENERAL_FAILURE) << "Failed to create native_handle";
    }
    for (size_t i = 0; i < fds.size(); ++i) {
        nativeHandle->data[i] = fds[i].release();
    }
    std::copy(handle->ints.begin(), handle->ints.end(), &nativeHandle->data[nativeHandle->numFds]);

    hidl_handle hidlHandle;
    hidlHandle.setTo(nativeHandle, /*shouldOwn=*/true);
    return hidlHandle;
}

nn::GeneralResult<nn::SharedHandle> sharedHandleFromNativeHandle(const native_handle_t* handle) {
    if (handle == nullptr) {
        return nullptr;
    }

    std::vector<base::unique_fd> fds;
    fds.reserve(handle->numFds);
    for (int i = 0; i < handle->numFds; ++i) {
        int dupFd = dup(handle->data[i]);
        if (dupFd == -1) {
            return NN_ERROR(nn::ErrorStatus::GENERAL_FAILURE) << "Failed to dup the fd";
        }
        fds.emplace_back(dupFd);
    }

    std::vector<int> ints(&handle->data[handle->numFds],
                          &handle->data[handle->numFds + handle->numInts]);

    return std::make_shared<const nn::Handle>(nn::Handle{
            .fds = std::move(fds),
            .ints = std::move(ints),
    });
}

nn::GeneralResult<hidl_vec<hidl_handle>> convertSyncFences(
        const std::vector<nn::SyncFence>& syncFences) {
    hidl_vec<hidl_handle> handles(syncFences.size());
    for (size_t i = 0; i < syncFences.size(); ++i) {
        handles[i] =
                NN_TRY(hal::utils::hidlHandleFromSharedHandle(syncFences[i].getSharedHandle()));
    }
    return handles;
}

}  // namespace android::hardware::neuralnetworks::utils
