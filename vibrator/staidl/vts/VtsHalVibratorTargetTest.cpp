/*
 * Copyright (C) 2019 The Android Open Source Project
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
#include <aidl/Gtest.h>
#include <aidl/Vintf.h>

#include <android/hardware/vibrator/BnVibratorCallback.h>
#include <android/hardware/vibrator/IVibrator.h>
#include <binder/IServiceManager.h>
#include <binder/ProcessState.h>

#include <future>

using android::ProcessState;
using android::sp;
using android::String16;
using android::binder::Status;
using android::hardware::vibrator::BnVibratorCallback;
using android::hardware::vibrator::Effect;
using android::hardware::vibrator::EffectStrength;
using android::hardware::vibrator::IVibrator;

const std::vector<Effect> kEffects = {
        Effect::CLICK,       Effect::DOUBLE_CLICK, Effect::TICK,        Effect::THUD,
        Effect::POP,         Effect::HEAVY_CLICK,  Effect::RINGTONE_1,  Effect::RINGTONE_2,
        Effect::RINGTONE_3,  Effect::RINGTONE_4,   Effect::RINGTONE_5,  Effect::RINGTONE_6,
        Effect::RINGTONE_7,  Effect::RINGTONE_8,   Effect::RINGTONE_9,  Effect::RINGTONE_10,
        Effect::RINGTONE_11, Effect::RINGTONE_12,  Effect::RINGTONE_13, Effect::RINGTONE_14,
        Effect::RINGTONE_15, Effect::TEXTURE_TICK};

const std::vector<EffectStrength> kEffectStrengths = {EffectStrength::LIGHT, EffectStrength::MEDIUM,
                                                      EffectStrength::STRONG};

const std::vector<Effect> kInvalidEffects = {
        static_cast<Effect>(static_cast<int32_t>(*kEffects.begin()) - 1),
        static_cast<Effect>(static_cast<int32_t>(*kEffects.end()) + 1),
};

const std::vector<EffectStrength> kInvalidEffectStrengths = {
        static_cast<EffectStrength>(static_cast<int8_t>(*kEffectStrengths.begin()) - 1),
        static_cast<EffectStrength>(static_cast<int8_t>(*kEffectStrengths.end()) + 1),
};

class CompletionCallback : public BnVibratorCallback {
  public:
    CompletionCallback(const std::function<void()>& callback) : mCallback(callback) {}
    Status onComplete() override {
        mCallback();
        return Status::ok();
    }

  private:
    std::function<void()> mCallback;
};

class VibratorAidl : public testing::TestWithParam<std::string> {
  public:
    virtual void SetUp() override {
        vibrator = android::waitForDeclaredService<IVibrator>(String16(GetParam().c_str()));
        ASSERT_NE(vibrator, nullptr);
        ASSERT_TRUE(vibrator->getCapabilities(&capabilities).isOk());
    }

    sp<IVibrator> vibrator;
    int32_t capabilities;
};

TEST_P(VibratorAidl, OnThenOffBeforeTimeout) {
    EXPECT_TRUE(vibrator->on(2000, nullptr /*callback*/).isOk());
    sleep(1);
    EXPECT_TRUE(vibrator->off().isOk());
}

TEST_P(VibratorAidl, OnWithCallback) {
    if (!(capabilities & IVibrator::CAP_PERFORM_CALLBACK)) return;

    std::promise<void> completionPromise;
    std::future<void> completionFuture{completionPromise.get_future()};
    sp<CompletionCallback> callback =
            new CompletionCallback([&completionPromise] { completionPromise.set_value(); });
    uint32_t durationMs = 250;
    std::chrono::milliseconds timeout{durationMs * 2};
    EXPECT_TRUE(vibrator->on(durationMs, callback).isOk());
    EXPECT_EQ(completionFuture.wait_for(timeout), std::future_status::ready);
    EXPECT_TRUE(vibrator->off().isOk());
}

TEST_P(VibratorAidl, OnCallbackNotSupported) {
    if (!(capabilities & IVibrator::CAP_PERFORM_CALLBACK)) {
        sp<CompletionCallback> callback = new CompletionCallback([] {});
        EXPECT_EQ(Status::EX_UNSUPPORTED_OPERATION, vibrator->on(250, callback).exceptionCode());
    }
}

TEST_P(VibratorAidl, ValidateEffect) {
    for (Effect effect : kEffects) {
        for (EffectStrength strength : kEffectStrengths) {
            int32_t lengthMs = 0;
            Status status = vibrator->perform(effect, strength, nullptr /*callback*/, &lengthMs);
            EXPECT_TRUE(status.isOk() ||
                        status.exceptionCode() == Status::EX_UNSUPPORTED_OPERATION);
            if (status.isOk()) {
                EXPECT_GT(lengthMs, 0);
            } else {
                EXPECT_EQ(lengthMs, 0);
            }
        }
    }
}

TEST_P(VibratorAidl, ValidateEffectWithCallback) {
    if (!(capabilities & IVibrator::CAP_PERFORM_CALLBACK)) return;

    for (Effect effect : kEffects) {
        for (EffectStrength strength : kEffectStrengths) {
            std::promise<void> completionPromise;
            std::future<void> completionFuture{completionPromise.get_future()};
            sp<CompletionCallback> callback =
                    new CompletionCallback([&completionPromise] { completionPromise.set_value(); });
            int lengthMs;
            Status status = vibrator->perform(effect, strength, callback, &lengthMs);
            EXPECT_TRUE(status.isOk() ||
                        status.exceptionCode() == Status::EX_UNSUPPORTED_OPERATION);
            if (!status.isOk()) continue;

            std::chrono::milliseconds timeout{lengthMs * 2};
            EXPECT_EQ(completionFuture.wait_for(timeout), std::future_status::ready);
        }
    }
}

TEST_P(VibratorAidl, ValidateEffectWithCallbackNotSupported) {
    if (capabilities & IVibrator::CAP_PERFORM_CALLBACK) return;

    for (Effect effect : kEffects) {
        for (EffectStrength strength : kEffectStrengths) {
            sp<CompletionCallback> callback = new CompletionCallback([] {});
            int lengthMs;
            Status status = vibrator->perform(effect, strength, callback, &lengthMs);
            EXPECT_EQ(Status::EX_UNSUPPORTED_OPERATION, status.exceptionCode());
            EXPECT_EQ(lengthMs, 0);
        }
    }
}

TEST_P(VibratorAidl, InvalidEffectsUnsupported) {
    for (Effect effect : kInvalidEffects) {
        for (EffectStrength strength : kInvalidEffectStrengths) {
            int32_t lengthMs;
            Status status = vibrator->perform(effect, strength, nullptr /*callback*/, &lengthMs);
            EXPECT_EQ(status.exceptionCode(), Status::EX_UNSUPPORTED_OPERATION);
        }
    }
}

TEST_P(VibratorAidl, ChangeVibrationAmplitude) {
    if (capabilities & IVibrator::CAP_AMPLITUDE_CONTROL) {
        EXPECT_TRUE(vibrator->setAmplitude(1).isOk());
        EXPECT_TRUE(vibrator->on(2000, nullptr /*callback*/).isOk());
        EXPECT_TRUE(vibrator->setAmplitude(128).isOk());
        sleep(1);
        EXPECT_TRUE(vibrator->setAmplitude(255).isOk());
        sleep(1);
    }
}

TEST_P(VibratorAidl, AmplitudeOutsideRangeFails) {
    if (capabilities & IVibrator::CAP_AMPLITUDE_CONTROL) {
        EXPECT_EQ(Status::EX_ILLEGAL_ARGUMENT, vibrator->setAmplitude(-1).exceptionCode());
        EXPECT_EQ(Status::EX_ILLEGAL_ARGUMENT, vibrator->setAmplitude(0).exceptionCode());
        EXPECT_EQ(Status::EX_ILLEGAL_ARGUMENT, vibrator->setAmplitude(256).exceptionCode());
    }
}

TEST_P(VibratorAidl, AmplitudeReturnsUnsupportedMatchingCapabilities) {
    if ((capabilities & IVibrator::CAP_AMPLITUDE_CONTROL) == 0) {
        EXPECT_EQ(Status::EX_UNSUPPORTED_OPERATION, vibrator->setAmplitude(1).exceptionCode());
    }
}

TEST_P(VibratorAidl, ChangeVibrationExternalControl) {
    if (capabilities & IVibrator::CAP_EXTERNAL_CONTROL) {
        EXPECT_TRUE(vibrator->setExternalControl(true).isOk());
        sleep(1);
        EXPECT_TRUE(vibrator->setExternalControl(false).isOk());
        sleep(1);
    }
}

TEST_P(VibratorAidl, ExternalControlUnsupportedMatchingCapabilities) {
    if ((capabilities & IVibrator::CAP_EXTERNAL_CONTROL) == 0) {
        EXPECT_EQ(Status::EX_UNSUPPORTED_OPERATION,
                  vibrator->setExternalControl(true).exceptionCode());
    }
}

INSTANTIATE_TEST_SUITE_P(, VibratorAidl,
                         testing::ValuesIn(android::getAidlHalInstanceNames(IVibrator::descriptor)),
                         android::PrintInstanceNameToString);

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ProcessState::self()->setThreadPoolMaxThreadCount(1);
    ProcessState::self()->startThreadPool();
    return RUN_ALL_TESTS();
}
