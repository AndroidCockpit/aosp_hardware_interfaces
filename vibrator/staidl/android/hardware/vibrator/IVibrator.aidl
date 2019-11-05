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

package android.hardware.vibrator;

import android.hardware.vibrator.IVibratorCallback;
import android.hardware.vibrator.Effect;
import android.hardware.vibrator.EffectStrength;

@VintfStability
interface IVibrator {
    /**
     * Whether on w/ IVibratorCallback can be used w/ 'on' function
     */
    const int CAP_ON_CALLBACK = 1 << 0;
    /**
     * Whether on w/ IVibratorCallback can be used w/ 'perform' function
     */
    const int CAP_PERFORM_CALLBACK = 1 << 1;
    /**
     * Whether setAmplitude is supported.
     */
    const int CAP_AMPLITUDE_CONTROL = 1 << 2;
    /**
     * Whether setExternalControl is supported.
     */
    const int CAP_EXTERNAL_CONTROL = 1 << 3;

    /**
     * Determine capabilities of the vibrator HAL (CAP_* values)
     */
    int getCapabilities();

    /**
     * Turn off vibrator
     *
     * Cancel a previously-started vibration, if any.
     */
    void off();

    /**
     * Turn on vibrator
     *
     * This function must only be called after the previous timeout has expired or
     * was canceled (through off()).
     * @param timeoutMs number of milliseconds to vibrate.
     * @param callback A callback used to inform Frameworks of state change, if supported.
     */
    void on(in int timeoutMs, in IVibratorCallback callback);

    /**
     * Fire off a predefined haptic event.
     *
     * @param effect The type of haptic event to trigger.
     * @param strength The intensity of haptic event to trigger.
     * @param callback A callback used to inform Frameworks of state change, if supported.
     * @return The length of time the event is expected to take in
     *     milliseconds. This doesn't need to be perfectly accurate, but should be a reasonable
     *     approximation.
     */
    int perform(in Effect effect, in EffectStrength strength, in IVibratorCallback callback);

    /**
     * Sets the motor's vibrational amplitude.
     *
     * Changes the force being produced by the underlying motor.
     *
     * @param amplitude The unitless force setting. Note that this number must
     *                  be between 1 and 255, inclusive. If the motor does not
     *                  have exactly 255 steps, it must do it's best to map it
     *                  onto the number of steps it does have.
     */
    void setAmplitude(in int amplitude);

    /**
     * Enables/disables control override of vibrator to audio.
     *
     * When this API is set, the vibrator control should be ceded to audio system
     * for haptic audio. While this is enabled, issuing of other commands to control
     * the vibrator is unsupported and the resulting behavior is undefined. Amplitude
     * control may or may not be supported and is reflected in the return value of
     * supportsAmplitudeControl() while this is enabled. When this is disabled, the
     * vibrator should resume to an off state.
     *
     * @param enabled Whether external control should be enabled or disabled.
     */
    void setExternalControl(in boolean enabled);
}
