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

package android.hardware.biometrics.fingerprint;

import android.hardware.biometrics.common.ICancellationSignal;
import android.hardware.keymaster.HardwareAuthToken;

/**
 * Operations that can be performed for unique sessions retrieved via IFingerprint#createSession.
 * Methods defined within this interface can be split into the following categories:
 *   1) Non-interrupting operations. These operations are handled by the HAL in FIFO order.
 *   1a) Cancellable operations. These are usually the operations that can execute for several
 *       minutes. To allow for cancellation, they return an instance of ICancellationSignal that
 *       lets the framework cancel them by calling ICancellationSignal#cancel. If such an operation
 *       is cancelled, it must notify the framework by calling ISessionCallback#onError with
 *       Error::CANCELED.
 *   1b) Non-cancellable operations. Such operations cannot be cancelled once started.
 *   2) Interrupting operations. These operations may be invoked by the framework immediately,
 *      regardless of whether another operation is executing. For example, on devices with sensors
 *      of FingerprintSensorType::UNDER_DISPLAY_*, ISession#onFingerDown may be invoked while the
 *      HAL is executing ISession#enroll, ISession#authenticate or ISession#detectInteraction.
 *
 * The lifecycle of a non-interrupting operation ends when one of its terminal callbacks is called.
 * For example, ISession#authenticate is considered completed when either of the following callbacks
 * is called: ISessionCallback#onError or ISessionCallback#onAuthenticationSucceeded.
 *
 * The lifecycle of an interrupting operation ends when it returns. Interrupting operations do not
 * have callbacks.
 *
 * ISession only supports execution of one non-interrupting operation at a time, regardless of
 * whether it's cancellable. The framework must wait for a corresponding callback indicating the end of
 * the current non-interrupting operation before a new non-interrupting operation can be started.
 */
@VintfStability
interface ISession {
    /**
     * Methods applicable to any fingerprint type.
     */

    /**
     * generateChallenge:
     *
     * Begins a secure transaction request. Note that the challenge by itself is not useful. It only
     * becomes useful when wrapped in a verifiable message such as a HardwareAuthToken.
     *
     * Canonical example:
     *   1) User requests an operation, such as fingerprint enrollment.
     *   2) Fingerprint enrollment cannot happen until the user confirms their lockscreen credential
     *      (PIN/Pattern/Password).
     *   3) However, the biometric subsystem does not want just "any" proof of credential
     *      confirmation. It needs proof that the user explicitly authenticated credential in order
     *      to allow addition of biometric enrollments.
     * To secure this path, the following path is taken:
     *   1) Upon user requesting fingerprint enroll, the framework requests
     *      ISession#generateChallenge
     *   2) Framework sends the challenge to the credential subsystem, and upon credential
     *      confirmation, a HAT is created, containing the challenge in the "challenge" field.
     *   3) Framework sends the HAT to the HAL, e.g. ISession#enroll.
     *   4) Implementation verifies the authenticity and integrity of the HAT.
     *   5) Implementation now has confidence that the user entered their credential to allow
     *      biometric enrollment.
     *
     * Note that this interface allows multiple in-flight challenges. Invoking generateChallenge
     * twice does not invalidate the first challenge. The challenge is invalidated only when:
     *   1) Its lifespan exceeds the HAL's internal challenge timeout
     *   2) IFingerprint#revokeChallenge is invoked
     *
     * For example, the following is a possible table of valid challenges:
     * ----------------------------------------------
     * | SensorId | UserId | ValidUntil | Challenge |
     * |----------|--------|------------|-----------|
     * | 0        | 0      | <Time1>    | <Random1> |
     * | 0        | 0      | <Time2>    | <Random2> |
     * | 1        | 0      | <Time3>    | <Random3> |
     * | 0        | 10     | <Time4>    | <Random4> |
     * ----------------------------------------------
     *
     */
    void generateChallenge();

    /**
     * revokeChallenge:
     *
     * Revokes a challenge that was previously generated. Note that if an invalid combination of
     * parameters is requested, the implementation must still notify the framework using the
     * provided callback.
     *
     * @param challenge Challenge that should be revoked.
     */
    void revokeChallenge(in long challenge);

    /**
     * enroll:
     *
     * A request to add a fingerprint enrollment.
     *
     * At any point during enrollment, if a non-recoverable error occurs, the HAL must notify the
     * framework via ISessionCallback#onError with the applicable enrollment-specific error.
     *
     * Before capturing fingerprint data, the implementation must first verify the authenticity and
     * integrity of the provided HardwareAuthToken. In addition, it must check that the challenge
     * within the provided HardwareAuthToken is valid. See ISession#generateChallenge. If any of
     * the above checks fail, the framework must be notified via ISessionCallback#onError and the
     * HAL must notify the framework when it returns to the idle state. See
     * Error::UNABLE_TO_PROCESS.
     *
     * During enrollment, the implementation may notify the framework via
     * ISessionCallback#onAcquired with messages that may be used to guide the user. This callback
     * can be invoked multiple times if necessary. Similarly, the framework may be notified of
     * enrollment progress changes via ISessionCallback#onEnrollmentProgress. Once the framework is
     * notified that there are 0 "remaining" steps, the framework may cache the "enrollmentId". See
     * ISessionCallback#onEnrollmentProgress for more info. The HAL must notify the framework once
     * it returns to the idle state.
     *
     * When a finger is successfully added and before the framework is notified of remaining=0, the
     * implementation MUST update and associate this (sensorId, userId) pair with a new new
     * entropy-encoded random identifier. See ISession#getAuthenticatorId for more information.
     *
     * @param hat See above documentation.
     */
    ICancellationSignal enroll(in HardwareAuthToken hat);

    /**
     * authenticate:
     *
     * A request to start looking for fingerprints to authenticate.
     *
     * At any point during authentication, if a non-recoverable error occurs, the HAL must notify
     * the framework via ISessionCallback#onError with the applicable authentication-specific error.
     *
     * During authentication, the implementation may notify the framework via
     * ISessionCallback#onAcquired with messages that may be used to guide the user. This callback
     * can be invoked multiple times if necessary.
     *
     * The HAL must notify the framework of accepts/rejects via ISessionCallback#onAuthentication*.
     *
     * The authentication lifecycle ends when either
     *   1) A fingerprint is accepted, and ISessionCallback#onAuthenticationSucceeded is invoked, or
     *   2) Any non-recoverable error occurs (such as lockout). See the full list of
     *      authentication-specific errors in the Error enum.
     *
     * Note that upon successful authentication, the lockout counter for this (sensorId, userId)
     * pair must be cleared.
     *
     * Note that upon successful authentication, ONLY sensors configured as SensorStrength::STRONG
     * are allowed to create and send a HardwareAuthToken to the framework. See the Android CDD for
     * more details. For SensorStrength::STRONG sensors, the HardwareAuthToken's "challenge" field
     * must be set with the operationId passed in during #authenticate. If the sensor is NOT
     * SensorStrength::STRONG, the HardwareAuthToken MUST be null.
     *
     * @param operationId For sensors configured as SensorStrength::STRONG, this must be used ONLY
     *                    upon successful authentication and wrapped in the HardwareAuthToken's
     *                    "challenge" field and sent to the framework via
     *                    ISessionCallback#onAuthenticated. The operationId is an opaque identifier
     *                    created from a separate secure subsystem such as, but not limited to
     *                    KeyStore/KeyMaster. The HardwareAuthToken can then be used as an
     *                    attestation for the provided operation. For example, this is used
     *                    to unlock biometric-bound auth-per-use keys (see
     *                    setUserAuthenticationParameters in KeyGenParameterSpec.Builder and
     *                    KeyProtection.Builder.
     */
    ICancellationSignal authenticate(in long operationId);

    /**
     * detectInteraction:
     *
     * A request to start looking for fingerprints without performing matching. Must only be called
     * if SensorProps#supportsDetectInteraction is true. If invoked on implementations that do not
     * support this functionality, the HAL must respond with ISession#onError(UNABLE_TO_PROCESS, 0).
     *
     * The framework will use this method in cases where determing user presence is required, but
     * identifying/authentication is not. For example, when the device is encrypted (first boot) or
     * in lockdown mode.
     *
     * At any point during detectInteraction, if a non-recoverable error occurs, the HAL must notify
     * the framework via ISessionCallback#onError with the applicable error.
     *
     * The implementation must only check for a fingerprint-like image was detected (e.g. to
     * minimize interactions due to non-fingerprint objects), and the lockout counter must not
     * be modified.
     *
     * Upon detecting any fingerprint, the implementation must invoke
     * ISessionCallback#onInteractionDetected.
     *
     * The lifecycle of this operation ends when either
     * 1) Any fingerprint is detected and the framework is notified via
     *    ISessionCallback#onInteractiondetected
     * 2) The operation was cancelled by the framework (see ICancellationSignal)
     * 3) The HAL ends the operation, for example when a subsequent operation pre-empts this one.
     *
     * Note that if the operation is canceled, the implementation must notify the framework via
     * ISessionCallback#onError with Error::CANCELED.
     *
     */
    ICancellationSignal detectInteraction();

    /*
     * enumerateEnrollments:
     *
     * A request to enumerate (list) the enrollments for this (sensorId, userId) pair. The
     * framework typically uses this to ensure that its cache is in sync with the HAL.
     *
     * The implementation must then notify the framework with a list of enrollments applicable
     * for the current session via ISessionCallback#onEnrollmentsEnumerated.
     *
     */
    void enumerateEnrollments();

    /**
     * removeEnrollments:
     *
     * A request to remove the enrollments for this (sensorId, userId) pair.
     *
     * After removing the enrollmentIds from everywhere necessary (filesystem, secure subsystems,
     * etc), the implementation must notify the framework via ISessionCallback#onEnrollmentsRemoved.
     *
     */
    void removeEnrollments(in int[] enrollmentIds);

    /**
     * getAuthenticatorId:
     *
     * MUST return 0 via ISessionCallback#onAuthenticatorIdRetrieved for sensors that are configured
     * as SensorStrength::WEAK or SensorStrength::CONVENIENCE.
     *
     * The following only applies to sensors that are configured as SensorStrength::STRONG.
     *
     * The authenticatorId is a (sensorId, user)-specific identifier which can be used during key
     * generation and key import to to associate a key (in KeyStore / KeyMaster) with the current
     * set of enrolled fingerprints. For example, the following public Android APIs allow for keys
     * to be invalidated when the user adds a new enrollment after the key was created:
     * KeyGenParameterSpec.Builder.setInvalidatedByBiometricEnrollment and
     * KeyProtection.Builder.setInvalidatedByBiometricEnrollment.
     *
     * In addition, upon successful fingerprint authentication, the signed HAT that is returned to
     * the framework via ISessionCallback#onAuthenticated must contain this identifier in the
     * authenticatorId field.
     *
     * Returns an entropy-encoded random identifier associated with the current set of enrollments
     * via ISessionCallback#onAuthenticatorIdRetrieved. The authenticatorId
     *   1) MUST change whenever a new fingerprint is enrolled
     *   2) MUST return 0 if no fingerprints are enrolled
     *   3) MUST not change if a fingerprint is deleted.
     *   4) MUST be an entropy-encoded random number
     *
     */
    void getAuthenticatorId();

    /**
     * invalidateAuthenticatorId:
     *
     * This method only applies to sensors that are configured as SensorStrength::STRONG. If invoked
     * by the framework for sensor of other strengths, the HAL should immediately invoke
     * ISessionCallback#onAuthenticatorIdInvalidated.
     *
     * The following only applies to sensors that are configured as SensorStrength::STRONG.
     *
     * When invoked by the framework, the implementation must perform the following sequence of
     * events:
     *   1) Update the authenticatorId with a new entropy-encoded random number
     *   2) Persist the new authenticatorId to non-ephemeral storage
     *   3) Notify the framework that the above is completed, via
     *      ISessionCallback#onAuthenticatorInvalidated
     *
     * A practical use case of invalidation would be when the user adds a new enrollment to a sensor
     * managed by a different HAL instance. The public android.security.keystore APIs bind keys to
     * "all biometrics" rather than "fingerprint-only" or "face-only" (see #getAuthenticatorId
     * for more details). As such, the framework would coordinate invalidation across multiple
     * biometric HALs as necessary.
     *
     */
    void invalidateAuthenticatorId();

    /**
     * resetLockout:
     *
     * Requests the implementation to clear the lockout counter. Upon receiving this request, the
     * implementation must perform the following:
     *   1) Verify the authenticity and integrity of the provided HAT
     *   2) Verify that the timestamp provided within the HAT is relatively recent (e.g. on the
     *      order of minutes, not hours).
     * If either of the checks fail, the HAL must invoke ISessionCallback#onError with
     * Error::UNABLE_TO_PROCESS and return to the idling state.
     *
     * Upon successful verification, the HAL must clear the lockout counter and notify the framework
     * via ISessionCallback#onLockoutCleared.
     *
     * Note that lockout is user AND sensor specific. In other words, there is a separate lockout
     * state for each (user, sensor) pair. For example, the following is a valid state on a
     * multi-sensor device:
     * ------------------------------------------------------------------
     * | SensorId | UserId | FailedAttempts | LockedOut | LockedUntil   |
     * |----------|--------|----------------|-----------|---------------|
     * | 0        | 0      | 1              | false     | x             |
     * | 1        | 0      | 5              | true      | <future_time> |
     * | 0        | 10     | 0              | false     | x             |
     * | 1        | 10     | 0              | false     | x             |
     * ------------------------------------------------------------------
     *
     * Lockout may be cleared in the following ways:
     *   1) ISession#resetLockout
     *   2) After a period of time, according to a rate-limiter.
     *
     * Note that the "FailedAttempts" counter must be cleared upon successful fingerprint
     * authentication. For example, if SensorId=0 UserId=0 FailedAttempts=1, and a successful
     * fingerprint authentication occurs, the counter for that (SensorId, UserId) pair must be reset
     * to 0.
     *
     * In addition, lockout states MUST persist after device reboots, HAL crashes, etc.
     *
     * See the Android CDD section 7.3.10 for the full set of lockout and rate-limiting
     * requirements.
     *
     * @param hat HardwareAuthToken See above documentation.
     */
    void resetLockout(in HardwareAuthToken hat);

    /*
     * Close this session and allow the HAL to release the resources associated with this session.
     *
     * A session can only be closed when the HAL is idling, i.e. not performing any of the
     * non-interruptable operations. If the HAL is busy performing a cancellable operation, the
     * operation must be explicitly cancelled with a call to ICancellationSignal#cancel before
     * the session can be closed.
     *
     * After a session is closed, the HAL must notify the framework by calling
     * ISessionCallback#onSessionClosed.
     *
     * All sessions must be explicitly closed. Calling IFingerprint#createSession while there is an
     * active session is considered an error.
     *
     */
    void close();

    /**
     * Methods for notifying the under-display fingerprint sensor about external events.
     */

    /**
     * onPointerDown:
     *
     * This method only applies to sensors that are configured as
     * FingerprintSensorType::UNDER_DISPLAY_*. If invoked erroneously by the framework for sensors
     * of other types, the HAL must treat this as a no-op and return immediately.
     *
     * For sensors of type FingerprintSensorType::UNDER_DISPLAY_*, this method is used to notify the
     * HAL of display touches. This method can be invoked when the HAL is performing any one of:
     * ISession#authenticate, ISession#enroll, ISession#detectInteraction.
     *
     * Note that the framework will only invoke this method if the event occurred on the display on
     * which this sensor is located.
     *
     * Note that for sensors which require illumination such as
     * FingerprintSensorType::UNDER_DISPLAY_OPTICAL, and where illumination is handled below the
     * framework, this is a good time to start illuminating.
     *
     * @param pointerId See android.view.MotionEvent#getPointerId
     * @param x The distance in pixels from the left edge of the display.
     * @param y The distance in pixels from the top edge of the display.
     * @param minor See android.view.MotionEvent#getTouchMinor
     * @param major See android.view.MotionEvent#getTouchMajor
     */
    void onPointerDown(in int pointerId, in int x, in int y, in float minor, in float major);

    /**
     * onPointerUp:
     *
     * This method only applies to sensors that are configured as
     * FingerprintSensorType::UNDER_DISPLAY_*. If invoked for sensors of other types, the HAL must
     * treat this as a no-op and return immediately.
     *
     * @param pointerId See android.view.MotionEvent#getPointerId
     */
    void onPointerUp(in int pointerId);

    /*
     * onUiReady:
     *
     * This method only applies to sensors that are configured as
     * FingerprintSensorType::UNDER_DISPLAY_OPTICAL. If invoked for sensors of other types, the HAL
     * must treat this as a no-op and return immediately.
     *
     * For FingerprintSensorType::UNDER_DISPLAY_OPTICAL where illumination is handled above the
     * HAL, the framework will invoke this method to notify that the illumination has started.
     */
    void onUiReady();
}
