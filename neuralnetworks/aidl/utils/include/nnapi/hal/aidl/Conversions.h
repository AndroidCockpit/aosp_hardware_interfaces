/*
 * Copyright (C) 2021 The Android Open Source Project
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

#ifndef ANDROID_HARDWARE_INTERFACES_NEURALNETWORKS_AIDL_CONVERSIONS_H
#define ANDROID_HARDWARE_INTERFACES_NEURALNETWORKS_AIDL_CONVERSIONS_H

#include <aidl/android/hardware/neuralnetworks/BufferDesc.h>
#include <aidl/android/hardware/neuralnetworks/BufferRole.h>
#include <aidl/android/hardware/neuralnetworks/Capabilities.h>
#include <aidl/android/hardware/neuralnetworks/DataLocation.h>
#include <aidl/android/hardware/neuralnetworks/DeviceType.h>
#include <aidl/android/hardware/neuralnetworks/ErrorStatus.h>
#include <aidl/android/hardware/neuralnetworks/ExecutionPreference.h>
#include <aidl/android/hardware/neuralnetworks/Extension.h>
#include <aidl/android/hardware/neuralnetworks/ExtensionNameAndPrefix.h>
#include <aidl/android/hardware/neuralnetworks/ExtensionOperandTypeInformation.h>
#include <aidl/android/hardware/neuralnetworks/Memory.h>
#include <aidl/android/hardware/neuralnetworks/Model.h>
#include <aidl/android/hardware/neuralnetworks/Operand.h>
#include <aidl/android/hardware/neuralnetworks/OperandExtraParams.h>
#include <aidl/android/hardware/neuralnetworks/OperandLifeTime.h>
#include <aidl/android/hardware/neuralnetworks/OperandPerformance.h>
#include <aidl/android/hardware/neuralnetworks/OperandType.h>
#include <aidl/android/hardware/neuralnetworks/Operation.h>
#include <aidl/android/hardware/neuralnetworks/OperationType.h>
#include <aidl/android/hardware/neuralnetworks/OutputShape.h>
#include <aidl/android/hardware/neuralnetworks/PerformanceInfo.h>
#include <aidl/android/hardware/neuralnetworks/Priority.h>
#include <aidl/android/hardware/neuralnetworks/Request.h>
#include <aidl/android/hardware/neuralnetworks/RequestArgument.h>
#include <aidl/android/hardware/neuralnetworks/RequestMemoryPool.h>
#include <aidl/android/hardware/neuralnetworks/Subgraph.h>
#include <aidl/android/hardware/neuralnetworks/SymmPerChannelQuantParams.h>
#include <aidl/android/hardware/neuralnetworks/Timing.h>

#include <nnapi/Result.h>
#include <nnapi/Types.h>
#include <nnapi/hal/CommonUtils.h>

#include <vector>

namespace android::nn {

GeneralResult<OperandType> unvalidatedConvert(const aidl_hal::OperandType& operandType);
GeneralResult<OperationType> unvalidatedConvert(const aidl_hal::OperationType& operationType);
GeneralResult<DeviceType> unvalidatedConvert(const aidl_hal::DeviceType& deviceType);
GeneralResult<Priority> unvalidatedConvert(const aidl_hal::Priority& priority);
GeneralResult<Capabilities> unvalidatedConvert(const aidl_hal::Capabilities& capabilities);
GeneralResult<Capabilities::OperandPerformance> unvalidatedConvert(
        const aidl_hal::OperandPerformance& operandPerformance);
GeneralResult<Capabilities::PerformanceInfo> unvalidatedConvert(
        const aidl_hal::PerformanceInfo& performanceInfo);
GeneralResult<DataLocation> unvalidatedConvert(const aidl_hal::DataLocation& location);
GeneralResult<Operand> unvalidatedConvert(const aidl_hal::Operand& operand);
GeneralResult<Operand::ExtraParams> unvalidatedConvert(
        const std::optional<aidl_hal::OperandExtraParams>& optionalExtraParams);
GeneralResult<Operand::LifeTime> unvalidatedConvert(
        const aidl_hal::OperandLifeTime& operandLifeTime);
GeneralResult<Operand::SymmPerChannelQuantParams> unvalidatedConvert(
        const aidl_hal::SymmPerChannelQuantParams& symmPerChannelQuantParams);
GeneralResult<Operation> unvalidatedConvert(const aidl_hal::Operation& operation);
GeneralResult<Model> unvalidatedConvert(const aidl_hal::Model& model);
GeneralResult<Model::ExtensionNameAndPrefix> unvalidatedConvert(
        const aidl_hal::ExtensionNameAndPrefix& extensionNameAndPrefix);
GeneralResult<Model::OperandValues> unvalidatedConvert(const std::vector<uint8_t>& operandValues);
GeneralResult<Model::Subgraph> unvalidatedConvert(const aidl_hal::Subgraph& subgraph);
GeneralResult<OutputShape> unvalidatedConvert(const aidl_hal::OutputShape& outputShape);
GeneralResult<MeasureTiming> unvalidatedConvert(bool measureTiming);
GeneralResult<SharedMemory> unvalidatedConvert(const aidl_hal::Memory& memory);
GeneralResult<Timing> unvalidatedConvert(const aidl_hal::Timing& timing);
GeneralResult<BufferDesc> unvalidatedConvert(const aidl_hal::BufferDesc& bufferDesc);
GeneralResult<BufferRole> unvalidatedConvert(const aidl_hal::BufferRole& bufferRole);
GeneralResult<Request> unvalidatedConvert(const aidl_hal::Request& request);
GeneralResult<Request::Argument> unvalidatedConvert(
        const aidl_hal::RequestArgument& requestArgument);
GeneralResult<Request::MemoryPool> unvalidatedConvert(
        const aidl_hal::RequestMemoryPool& memoryPool);
GeneralResult<ErrorStatus> unvalidatedConvert(const aidl_hal::ErrorStatus& errorStatus);
GeneralResult<ExecutionPreference> unvalidatedConvert(
        const aidl_hal::ExecutionPreference& executionPreference);
GeneralResult<Extension> unvalidatedConvert(const aidl_hal::Extension& extension);
GeneralResult<Extension::OperandTypeInformation> unvalidatedConvert(
        const aidl_hal::ExtensionOperandTypeInformation& operandTypeInformation);
GeneralResult<SharedHandle> unvalidatedConvert(
        const ::aidl::android::hardware::common::NativeHandle& handle);

GeneralResult<ExecutionPreference> convert(
        const aidl_hal::ExecutionPreference& executionPreference);
GeneralResult<SharedMemory> convert(const aidl_hal::Memory& memory);
GeneralResult<Model> convert(const aidl_hal::Model& model);
GeneralResult<Operand> convert(const aidl_hal::Operand& operand);
GeneralResult<OperandType> convert(const aidl_hal::OperandType& operandType);
GeneralResult<Priority> convert(const aidl_hal::Priority& priority);
GeneralResult<Request::MemoryPool> convert(const aidl_hal::RequestMemoryPool& memoryPool);
GeneralResult<Request> convert(const aidl_hal::Request& request);

GeneralResult<std::vector<Operation>> convert(const std::vector<aidl_hal::Operation>& outputShapes);
GeneralResult<std::vector<SharedMemory>> convert(const std::vector<aidl_hal::Memory>& memories);

GeneralResult<std::vector<uint32_t>> toUnsigned(const std::vector<int32_t>& vec);

}  // namespace android::nn

namespace aidl::android::hardware::neuralnetworks::utils {

namespace nn = ::android::nn;

nn::GeneralResult<Memory> unvalidatedConvert(const nn::SharedMemory& memory);
nn::GeneralResult<OutputShape> unvalidatedConvert(const nn::OutputShape& outputShape);
nn::GeneralResult<ErrorStatus> unvalidatedConvert(const nn::ErrorStatus& errorStatus);

nn::GeneralResult<Memory> convert(const nn::SharedMemory& memory);
nn::GeneralResult<ErrorStatus> convert(const nn::ErrorStatus& errorStatus);
nn::GeneralResult<std::vector<OutputShape>> convert(
        const std::vector<nn::OutputShape>& outputShapes);

nn::GeneralResult<std::vector<int32_t>> toSigned(const std::vector<uint32_t>& vec);

}  // namespace aidl::android::hardware::neuralnetworks::utils

#endif  // ANDROID_HARDWARE_INTERFACES_NEURALNETWORKS_AIDL_CONVERSIONS_H
