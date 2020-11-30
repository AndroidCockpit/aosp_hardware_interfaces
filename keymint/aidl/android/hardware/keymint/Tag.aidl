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

package android.hardware.keymint;

import android.hardware.keymint.TagType;

// TODO(seleneh) : note aidl currently does not support double nested enum definitions such as
// ROOT_OF_TRUST = TagType:BYTES | 704.  So we are forced to write definations as
// ROOT_OF_TRUST = (9 << 28) for now.  Will need to flip this back later when aidl support is added.

/**
 * Tag specifies various kinds of tags that can be set in KeyParameter to identify what kind of
 * data are stored in KeyParameter.
 */
@VintfStability
@Backing(type = "int")
enum Tag {
    /**
     * Tag::INVALID should never be set.  It means you hit an error.
     */
    INVALID = (0 << 28) | 0,

    /**
     * Tag::PURPOSE specifies the set of purposes for which the key may be used.  Possible values
     * are defined in the KeyPurpose enumeration.
     *
     * This tag is repeatable; keys may be generated with multiple values, although an operation has
     * a single purpose.  When begin() is called to start an operation, the purpose of the operation
     * is specified.  If the purpose specified for the operation is not authorized by the key (the
     * key didn't have a corresponding Tag::PURPOSE provided during generation/import), the
     * operation must fail with ErrorCode::INCOMPATIBLE_PURPOSE.
     *
     * Must be hardware-enforced.
     */
    PURPOSE = (2 << 28) | 1, /* TagType:ENUM_REP */

    /**
     * Tag::ALGORITHM specifies the cryptographic algorithm with which the key is used.  This tag
     * must be provided to generateKey and importKey, and must be specified in the wrapped key
     * provided to importWrappedKey.
     *
     * Must be hardware-enforced.
     */
    ALGORITHM = (1 << 28) | 2, /* TagType:ENUM */

    /**
     * Tag::KEY_SIZE pecifies the size, in bits, of the key, measuring in the normal way for the
     * key's algorithm.  For example, for RSA keys, Tag::KEY_SIZE specifies the size of the public
     * modulus.  For AES keys it specifies the length of the secret key material.  For 3DES keys it
     * specifies the length of the key material, not counting parity bits (though parity bits must
     * be provided for import, etc.).  Since only three-key 3DES keys are supported, 3DES
     * Tag::KEY_SIZE must be 168.
     *
     * Must be hardware-enforced.
     */
    KEY_SIZE = (3 << 28) | 3, /* TagType:UINT */

    /**
     * Tag::BLOCK_MODE specifies the block cipher mode(s) with which the key may be used.  This tag
     * is only relevant to AES and 3DES keys.  Possible values are defined by the BlockMode enum.
     *
     * This tag is repeatable for key generation/import.  For AES and 3DES operations the caller
     * must specify a Tag::BLOCK_MODE in the additionalParams argument of begin().  If the mode is
     * missing or the specified mode is not in the modes specified for the key during
     * generation/import, the operation must fail with ErrorCode::INCOMPATIBLE_BLOCK_MODE.
     *
     * Must be hardware-enforced.
     */
    BLOCK_MODE = (2 << 28) | 4,
    /* BlockMode. */ /* TagType:ENUM_REP */

    /**
     * Tag::DIGEST specifies the digest algorithms that may be used with the key to perform signing
     * and verification operations.  This tag is relevant to RSA, ECDSA and HMAC keys.  Possible
     * values are defined by the Digest enum.
     *
     * This tag is repeatable for key generation/import.  For signing and verification operations,
     * the caller must specify a digest in the additionalParams argument of begin().  If the digest
     * is missing or the specified digest is not in the digests associated with the key, the
     * operation must fail with ErrorCode::INCOMPATIBLE_DIGEST.
     *
     * Must be hardware-enforced.
     */
    DIGEST = (2 << 28) | 5, /* TagType:ENUM_REP */

    /**
     * Tag::PADDING specifies the padding modes that may be used with the key.  This tag is relevant
     * to RSA, AES and 3DES keys.  Possible values are defined by the PaddingMode enum.
     *
     * PaddingMode::RSA_OAEP and PaddingMode::RSA_PKCS1_1_5_ENCRYPT are used only for RSA
     * encryption/decryption keys and specify RSA OAEP padding and RSA PKCS#1 v1.5 randomized
     * padding, respectively.  PaddingMode::RSA_PSS and PaddingMode::RSA_PKCS1_1_5_SIGN are used
     * only for RSA signing/verification keys and specify RSA PSS padding and RSA PKCS#1 v1.5
     * deterministic padding, respectively.
     *
     * PaddingMode::NONE may be used with either RSA, AES or 3DES keys.  For AES or 3DES keys, if
     * PaddingMode::NONE is used with block mode ECB or CBC and the data to be encrypted or
     * decrypted is not a multiple of the AES block size in length, the call to finish() must fail
     * with ErrorCode::INVALID_INPUT_LENGTH.
     *
     * PaddingMode::PKCS7 may only be used with AES and 3DES keys, and only with ECB and CBC modes.
     *
     * In any case, if the caller specifies a padding mode that is not usable with the key's
     * algorithm, the generation or import method must return ErrorCode::INCOMPATIBLE_PADDING_MODE.
     *
     * This tag is repeatable.  A padding mode must be specified in the call to begin().  If the
     * specified mode is not authorized for the key, the operation must fail with
     * ErrorCode::INCOMPATIBLE_BLOCK_MODE.
     *
     * Must be hardware-enforced.
     */
    PADDING = (2 << 28) | 6, /* TagType:ENUM_REP */

    /**
     * Tag::CALLER_NONCE specifies that the caller can provide a nonce for nonce-requiring
     * operations.  This tag is boolean, so the possible values are true (if the tag is present) and
     * false (if the tag is not present).
     *
     * This tag is used only for AES and 3DES keys, and is only relevant for CBC, CTR and GCM block
     * modes.  If the tag is not present in a key's authorization list, implementations must reject
     * any operation that provides Tag::NONCE to begin() with ErrorCode::CALLER_NONCE_PROHIBITED.
     *
     * Must be hardware-enforced.
     */
    CALLER_NONCE = (7 << 28) | 7, /* TagType:BOOL */

    /**
     * Tag::MIN_MAC_LENGTH specifies the minimum length of MAC that can be requested or verified
     * with this key for HMAC keys and AES keys that support GCM mode.
     *
     * This value is the minimum MAC length, in bits.  It must be a multiple of 8 bits.  For HMAC
     * keys, the value must be least 64 and no more than 512.  For GCM keys, the value must be at
     * least 96 and no more than 128.  If the provided value violates these requirements,
     * generateKey() or importKey() must return ErrorCode::UNSUPPORTED_KEY_SIZE.
     *
     * Must be hardware-enforced.
     */
    MIN_MAC_LENGTH = (3 << 28) | 8, /* TagType:UINT */

    // Tag 9 reserved

    /**
     * Tag::EC_CURVE specifies the elliptic curve.  EC key generation requests may have
     * Tag:EC_CURVE, Tag::KEY_SIZE, or both.  If both are provided and the size and curve do not
     * match, IKeyMintDevice must return ErrorCode::INVALID_ARGUMENT.
     *
     * Must be hardware-enforced.
     */
    EC_CURVE = (1 << 28) | 10, /* TagType:ENUM */

    /**
     * Tag::RSA_PUBLIC_EXPONENT specifies the value of the public exponent for an RSA key pair.
     * This tag is relevant only to RSA keys, and is required for all RSA keys.
     *
     * The value is a 64-bit unsigned integer that satisfies the requirements of an RSA public
     * exponent.  This value must be a prime number.  IKeyMintDevice implementations must support
     * the value 2^16+1 and may support other reasonable values.  If no exponent is specified or if
     * the specified exponent is not supported, key generation must fail with
     * ErrorCode::INVALID_ARGUMENT.
     *
     * Must be hardware-enforced.
     */
    RSA_PUBLIC_EXPONENT = (5 << 28) | 200, /* TagType:ULONG */

    // Tag 201 reserved

    /**
     * Tag::INCLUDE_UNIQUE_ID is specified during key generation to indicate that an attestation
     * certificate for the generated key should contain an application-scoped and time-bounded
     * device-unique ID.  See Tag::UNIQUE_ID.
     *
     * Must be hardware-enforced.
     */
    INCLUDE_UNIQUE_ID = (7 << 28) | 202, /* TagType:BOOL */

    /**
     * TODO(seleneh) this tag needs to be deleted from all codes.
     *
     * Tag::BLOB_USAGE_REQUIREMENTS specifies the necessary system environment conditions for the
     * generated key to be used.  Possible values are defined by the KeyBlobUsageRequirements enum.
     *
     * This tag is specified by the caller during key generation or import to require that the key
     * is usable in the specified condition.  If the caller specifies Tag::BLOB_USAGE_REQUIREMENTS
     * with value KeyBlobUsageRequirements::STANDALONE the IKeyMintDevice must return a key blob
     * that can be used without file system support.  This is critical for devices with encrypted
     * disks, where the file system may not be available until after a KeyMint key is used to
     * decrypt the disk.
     *
     * Must be hardware-enforced.
     */
    BLOB_USAGE_REQUIREMENTS = (1 << 28) | 301, /* TagType:ENUM */

    /**
     * Tag::BOOTLOADER_ONLY specifies only the bootloader can use the key.
     *
     * Any attempt to use a key with Tag::BOOTLOADER_ONLY from the Android system must fail with
     * ErrorCode::INVALID_KEY_BLOB.
     *
     * Must be hardware-enforced.
     */
    BOOTLOADER_ONLY = (7 << 28) | 302, /* TagType:BOOL */

    /**
     * Tag::ROLLBACK_RESISTANCE specifies that the key has rollback resistance, meaning that when
     * deleted with deleteKey() or deleteAllKeys(), the key is guaranteed to be permanently deleted
     * and unusable.  It's possible that keys without this tag could be deleted and then restored
     * from backup.
     *
     * This tag is specified by the caller during key generation or import to require.  If the
     * IKeyMintDevice cannot guarantee rollback resistance for the specified key, it must return
     * ErrorCode::ROLLBACK_RESISTANCE_UNAVAILABLE.  IKeyMintDevice implementations are not
     * required to support rollback resistance.
     *
     * Must be hardwared-enforced.
     */
    ROLLBACK_RESISTANCE = (7 << 28) | 303, /* TagType:BOOL */

    // Reserved for future use.
    HARDWARE_TYPE = (1 << 28) | 304, /* TagType:ENUM */

    /**
     * Keys tagged with EARLY_BOOT_ONLY may only be used, or created, during early boot, until
     * IKeyMintDevice::earlyBootEnded() is called.
     */
    EARLY_BOOT_ONLY = (7 << 28) | 305, /* TagType:BOOL */

    /**
     * Tag::ACTIVE_DATETIME specifies the date and time at which the key becomes active, in
     * milliseconds since Jan 1, 1970.  If a key with this tag is used prior to the specified date
     * and time, IKeyMintDevice::begin() must return ErrorCode::KEY_NOT_YET_VALID;
     *
     * Need not be hardware-enforced.
     */
    ACTIVE_DATETIME = (6 << 28) | 400,
    /* Start of validity. */ /* TagType:DATE */

    /**
     * Tag::ORIGINATION_EXPIRE_DATETIME specifies the date and time at which the key expires for
     * signing and encryption purposes.  After this time, any attempt to use a key with
     * KeyPurpose::SIGN or KeyPurpose::ENCRYPT provided to begin() must fail with
     * ErrorCode::KEY_EXPIRED.
     *
     * The value is a 64-bit integer representing milliseconds since January 1, 1970.
     *
     * Need not be hardware-enforced.
     */
    ORIGINATION_EXPIRE_DATETIME = (6 << 28) | 401, /* TagType:DATE */

    /**
     * Tag::USAGE_EXPIRE_DATETIME specifies the date and time at which the key expires for
     * verification and decryption purposes.  After this time, any attempt to use a key with
     * KeyPurpose::VERIFY or KeyPurpose::DECRYPT provided to begin() must fail with
     * ErrorCode::KEY_EXPIRED.
     *
     * The value is a 64-bit integer representing milliseconds since January 1, 1970.
     *
     * Need not be hardware-enforced.
     */
    USAGE_EXPIRE_DATETIME = (6 << 28) | 402, /* TagType:DATE */

    /**
     * TODO(seleneh) this tag need to be deleted.
     *
     * TODO(seleneh) this tag need to be deleted.
     *
     * Tag::MIN_SECONDS_BETWEEN_OPS specifies the minimum amount of time that elapses between
     * allowed operations using a key.  This can be used to rate-limit uses of keys in contexts
     * where unlimited use may enable brute force attacks.
     *
     * The value is a 32-bit integer representing seconds between allowed operations.
     *
     * When a key with this tag is used in an operation, the IKeyMintDevice must start a timer
     * during the finish() or abort() call.  Any call to begin() that is received before the timer
     * indicates that the interval specified by Tag::MIN_SECONDS_BETWEEN_OPS has elapsed must fail
     * with ErrorCode::KEY_RATE_LIMIT_EXCEEDED.  This implies that the IKeyMintDevice must keep a
     * table of use counters for keys with this tag.  Because memory is often limited, this table
     * may have a fixed maximum size and KeyMint may fail operations that attempt to use keys with
     * this tag when the table is full.  The table must acommodate at least 8 in-use keys and
     * aggressively reuse table slots when key minimum-usage intervals expire.  If an operation
     * fails because the table is full, KeyMint returns ErrorCode::TOO_MANY_OPERATIONS.
     *
     * Must be hardware-enforced.
     */
    MIN_SECONDS_BETWEEN_OPS = (3 << 28) | 403, /* TagType:UINT */

    /**
     * Tag::MAX_USES_PER_BOOT specifies the maximum number of times that a key may be used between
     * system reboots.  This is another mechanism to rate-limit key use.
     *
     * The value is a 32-bit integer representing uses per boot.
     *
     * When a key with this tag is used in an operation, a key-associated counter must be
     * incremented during the begin() call.  After the key counter has exceeded this value, all
     * subsequent attempts to use the key must fail with ErrorCode::MAX_OPS_EXCEEDED, until the
     * device is restarted.  This implies that the IKeyMintDevice must keep a table of use
     * counters for keys with this tag.  Because KeyMint memory is often limited, this table can
     * have a fixed maximum size and KeyMint can fail operations that attempt to use keys with
     * this tag when the table is full.  The table needs to acommodate at least 8 keys.  If an
     * operation fails because the table is full, IKeyMintDevice must
     * ErrorCode::TOO_MANY_OPERATIONS.
     *
     * Must be hardware-enforced.
     */
    MAX_USES_PER_BOOT = (3 << 28) | 404, /* TagType:UINT */

    /**
     * Tag::USER_ID specifies the ID of the Android user that is permitted to use the key.
     *
     * Must not be hardware-enforced.
     */
    USER_ID = (3 << 28) | 501, /* TagType:UINT */

    /**
     * Tag::USER_SECURE_ID specifies that a key may only be used under a particular secure user
     * authentication state.  This tag is mutually exclusive with Tag::NO_AUTH_REQUIRED.
     *
     * The value is a 64-bit integer specifying the authentication policy state value which must be
     * present in the userId or authenticatorId field of a HardwareAuthToken provided to begin(),
     * update(), or finish().  If a key with Tag::USER_SECURE_ID is used without a HardwareAuthToken
     * with the matching userId or authenticatorId, the IKeyMintDevice must return
     * ErrorCode::KEY_USER_NOT_AUTHENTICATED.
     *
     * Tag::USER_SECURE_ID interacts with Tag::AUTH_TIMEOUT in a very important way.  If
     * Tag::AUTH_TIMEOUT is present in the key's characteristics then the key is a "timeout-based"
     * key, and may only be used if the difference between the current time when begin() is called
     * and the timestamp in the HardwareAuthToken is less than the value in Tag::AUTH_TIMEOUT * 1000
     * (the multiplier is because Tag::AUTH_TIMEOUT is in seconds, but the HardwareAuthToken
     * timestamp is in milliseconds).  Otherwise the IKeyMintDevice must returrn
     * ErrorCode::KEY_USER_NOT_AUTHENTICATED.
     *
     * If Tag::AUTH_TIMEOUT is not present, then the key is an "auth-per-operation" key.  In this
     * case, begin() must not require a HardwareAuthToken with appropriate contents.  Instead,
     * update() and finish() must receive a HardwareAuthToken with Tag::USER_SECURE_ID value in
     * userId or authenticatorId fields, and the current operation's operation handle in the
     * challenge field.  Otherwise the IKeyMintDevice must returrn
     * ErrorCode::KEY_USER_NOT_AUTHENTICATED.
     *
     * This tag is repeatable.  If repeated, and any one of the values matches the HardwareAuthToken
     * as described above, the key is authorized for use.  Otherwise the operation must fail with
     * ErrorCode::KEY_USER_NOT_AUTHENTICATED.
     *
     * Must be hardware-enforced.
     */
    USER_SECURE_ID = (10 << 28) | 502, /* TagType:ULONG_REP */

    /**
     * Tag::NO_AUTH_REQUIRED specifies that no authentication is required to use this key.  This tag
     * is mutually exclusive with Tag::USER_SECURE_ID.
     *
     * Must be hardware-enforced.
     */
    NO_AUTH_REQUIRED = (7 << 28) | 503, /* TagType:BOOL */

    /**
     * Tag::USER_AUTH_TYPE specifies the types of user authenticators that may be used to authorize
     * this key.
     *
     * The value is one or more values from HardwareAuthenticatorType, ORed together.
     *
     * When IKeyMintDevice is requested to perform an operation with a key with this tag, it must
     * receive a HardwareAuthToken and one or more bits must be set in both the HardwareAuthToken's
     * authenticatorType field and the Tag::USER_AUTH_TYPE value.  That is, it must be true that
     *
     *    (token.authenticatorType & tag_user_auth_type) != 0
     *
     * where token.authenticatorType is the authenticatorType field of the HardwareAuthToken and
     * tag_user_auth_type is the value of Tag:USER_AUTH_TYPE.
     *
     * Must be hardware-enforced.
     */
    USER_AUTH_TYPE = (1 << 28) | 504, /* TagType:ENUM */

    /**
     * Tag::AUTH_TIMEOUT specifies the time in seconds for which the key is authorized for use,
     * after user authentication.  If
     * Tag::USER_SECURE_ID is present and this tag is not, then the key requies authentication for
     * every usage (see begin() for the details of the authentication-per-operation flow).
     *
     * The value is a 32-bit integer specifying the time in seconds after a successful
     * authentication of the user specified by Tag::USER_SECURE_ID with the authentication method
     * specified by Tag::USER_AUTH_TYPE that the key can be used.
     *
     * Must be hardware-enforced.
     */
    AUTH_TIMEOUT = (3 << 28) | 505, /* TagType:UINT */

    /**
     * Tag::ALLOW_WHILE_ON_BODY specifies that the key may be used after authentication timeout if
     * device is still on-body (requires on-body sensor).
     *
     * Cannot be hardware-enforced.
     */
    ALLOW_WHILE_ON_BODY = (7 << 28) | 506, /* TagType:BOOL */

    /**
     * TRUSTED_USER_PRESENCE_REQUIRED is an optional feature that specifies that this key must be
     * unusable except when the user has provided proof of physical presence.  Proof of physical
     * presence must be a signal that cannot be triggered by an attacker who doesn't have one of:
     *
     *    a) Physical control of the device or
     *
     *    b) Control of the secure environment that holds the key.
     *
     * For instance, proof of user identity may be considered proof of presence if it meets the
     * requirements.  However, proof of identity established in one security domain (e.g. TEE) does
     * not constitute proof of presence in another security domain (e.g. StrongBox), and no
     * mechanism analogous to the authentication token is defined for communicating proof of
     * presence across security domains.
     *
     * Some examples:
     *
     *     A hardware button hardwired to a pin on a StrongBox device in such a way that nothing
     *     other than a button press can trigger the signal constitutes proof of physical presence
     *     for StrongBox keys.
     *
     *     Fingerprint authentication provides proof of presence (and identity) for TEE keys if the
     *     TEE has exclusive control of the fingerprint scanner and performs fingerprint matching.
     *
     *     Password authentication does not provide proof of presence to either TEE or StrongBox,
     *     even if TEE or StrongBox does the password matching, because password input is handled by
     *     the non-secure world, which means an attacker who has compromised Android can spoof
     *     password authentication.
     *
     * Note that no mechanism is defined for delivering proof of presence to an IKeyMintDevice,
     * except perhaps as implied by an auth token.  This means that KeyMint must be able to check
     * proof of presence some other way.  Further, the proof of presence must be performed between
     * begin() and the first call to update() or finish().  If the first update() or the finish()
     * call is made without proof of presence, the keyMint method must return
     * ErrorCode::PROOF_OF_PRESENCE_REQUIRED and abort the operation.  The caller must delay the
     * update() or finish() call until proof of presence has been provided, which means the caller
     * must also have some mechanism for verifying that the proof has been provided.
     *
     * Only one operation requiring TUP may be in flight at a time.  If begin() has already been
     * called on one key with TRUSTED_USER_PRESENCE_REQUIRED, and another begin() comes in for that
     * key or another with TRUSTED_USER_PRESENCE_REQUIRED, KeyMint must return
     * ErrorCode::CONCURRENT_PROOF_OF_PRESENCE_REQUESTED.
     *
     * Must be hardware-enforced.
     */
    TRUSTED_USER_PRESENCE_REQUIRED = (7 << 28) | 507, /* TagType:BOOL */

    /** Tag::TRUSTED_CONFIRMATION_REQUIRED is only applicable to keys with KeyPurpose SIGN, and
     *  specifies that this key must not be usable unless the user provides confirmation of the data
     *  to be signed.  Confirmation is proven to keyMint via an approval token.  See
     *  CONFIRMATION_TOKEN, as well as the ConfirmatinUI HAL.
     *
     * If an attempt to use a key with this tag does not have a cryptographically valid
     * CONFIRMATION_TOKEN provided to finish() or if the data provided to update()/finish() does not
     * match the data described in the token, keyMint must return NO_USER_CONFIRMATION.
     *
     * Must be hardware-enforced.
     */
    TRUSTED_CONFIRMATION_REQUIRED = (7 << 28) | 508, /* TagType:BOOL */

    /**
     * Tag::UNLOCKED_DEVICE_REQUIRED specifies that the key may only be used when the device is
     * unlocked.
     *
     * Must be software-enforced.
     */
    UNLOCKED_DEVICE_REQUIRED = (7 << 28) | 509, /* TagType:BOOL */

    /**
     * Tag::APPLICATION_ID.  When provided to generateKey or importKey, this tag specifies data
     * that is necessary during all uses of the key.  In particular, calls to exportKey() and
     * getKeyCharacteristics() must provide the same value to the clientId parameter, and calls to
     * begin must provide this tag and the same associated data as part of the inParams set.  If
     * the correct data is not provided, the method must return ErrorCode::INVALID_KEY_BLOB.
     *
     * The content of this tag must be bound to the key cryptographically, meaning it must not be
     * possible for an adversary who has access to all of the secure world secrets but does not have
     * access to the tag content to decrypt the key without brute-forcing the tag content, which
     * applications can prevent by specifying sufficiently high-entropy content.
     *
     * Must never appear in KeyCharacteristics.
     */
    APPLICATION_ID = (9 << 28) | 601, /* TagType:BYTES */

    /*
     * Semantically unenforceable tags, either because they have no specific meaning or because
     * they're informational only.
     */

    /**
     * Tag::APPLICATION_DATA.  When provided to generateKey or importKey, this tag specifies data
     * that is necessary during all uses of the key.  In particular, calls to exportKey() and
     * getKeyCharacteristics() must provide the same value to the appData parameter, and calls to
     * begin must provide this tag and the same associated data as part of the inParams set.  If
     * the correct data is not provided, the method must return ErrorCode::INVALID_KEY_BLOB.
     *
     * The content of this tag msut be bound to the key cryptographically, meaning it must not be
     * possible for an adversary who has access to all of the secure world secrets but does not have
     * access to the tag content to decrypt the key without brute-forcing the tag content, which
     * applications can prevent by specifying sufficiently high-entropy content.
     *
     * Must never appear in KeyCharacteristics.
     */
    APPLICATION_DATA = (9 << 28) | 700, /* TagType:BYTES */

    /**
     * Tag::CREATION_DATETIME specifies the date and time the key was created, in milliseconds since
     * January 1, 1970.  This tag is optional and informational only.
     *
     * Tag::CREATED is informational only, and not enforced by anything.  Must be in the
     * software-enforced list, if provided.
     */
    CREATION_DATETIME = (6 << 28) | 701, /* TagType:DATE */

    /**
     * Tag::ORIGIN specifies where the key was created, if known.  This tag must not be specified
     * during key generation or import, and must be added to the key characteristics by the
     * IKeyMintDevice.  The possible values are defined in the KeyOrigin enum.
     *
     * Must be hardware-enforced.
     */
    ORIGIN = (1 << 28) | 702, /* TagType:ENUM */

    // 703 is unused.

    /**
     * Tag::ROOT_OF_TRUST specifies the root of trust associated with the key used by verified boot
     * to validate the system.  It describes the boot key, verified boot state, boot hash, and
     * whether device is locked.  This tag is never provided to or returned from KeyMint in the
     * key characteristics.  It exists only to define the tag for use in the attestation record.
     *
     * Must never appear in KeyCharacteristics.
     */
    ROOT_OF_TRUST = (9 << 28) | 704, /* TagType:BYTES */

    /**
     * Tag::OS_VERSION specifies the system OS version with which the key may be used.  This tag is
     * never sent to the IKeyMintDevice, but is added to the hardware-enforced authorization list
     * by the TA.  Any attempt to use a key with a Tag::OS_VERSION value different from the
     * currently-running OS version must cause begin(), getKeyCharacteristics() or exportKey() to
     * return ErrorCode::KEY_REQUIRES_UPGRADE.  See upgradeKey() for details.
     *
     * The value of the tag is an integer of the form MMmmss, where MM is the major version number,
     * mm is the minor version number, and ss is the sub-minor version number.  For example, for a
     * key generated on Android version 4.0.3, the value would be 040003.
     *
     * The IKeyMintDevice HAL must read the current OS version from the system property
     * ro.build.version.release and deliver it to the secure environment when the HAL is first
     * loaded (mechanism is implementation-defined).  The secure environment must not accept another
     * version until after the next boot.  If the content of ro.build.version.release has additional
     * version information after the sub-minor version number, it must not be included in
     * Tag::OS_VERSION.  If the content is non-numeric, the secure environment must use 0 as the
     * system version.
     *
     * Must be hardware-enforced.
     */
    OS_VERSION = (3 << 28) | 705, /* TagType:UINT */

    /**
     * Tag::OS_PATCHLEVEL specifies the system security patch level with which the key may be used.
     * This tag is never sent to the keyMint TA, but is added to the hardware-enforced
     * authorization list by the TA.  Any attempt to use a key with a Tag::OS_PATCHLEVEL value
     * different from the currently-running system patchlevel must cause begin(),
     * getKeyCharacteristics() or exportKey() to return ErrorCode::KEY_REQUIRES_UPGRADE.  See
     * upgradeKey() for details.
     *
     * The value of the tag is an integer of the form YYYYMM, where YYYY is the four-digit year of
     * the last update and MM is the two-digit month of the last update.  For example, for a key
     * generated on an Android device last updated in December 2015, the value would be 201512.
     *
     * The IKeyMintDevice HAL must read the current system patchlevel from the system property
     * ro.build.version.security_patch and deliver it to the secure environment when the HAL is
     * first loaded (mechanism is implementation-defined).  The secure environment must not accept
     * another patchlevel until after the next boot.
     *
     * Must be hardware-enforced.
     */
    OS_PATCHLEVEL = (3 << 28) | 706, /* TagType:UINT */

    /**
     * Tag::UNIQUE_ID specifies a unique, time-based identifier.  This tag is never provided to or
     * returned from KeyMint in the key characteristics.  It exists only to define the tag for use
     * in the attestation record.
     *
     * When a key with Tag::INCLUDE_UNIQUE_ID is attested, the unique ID is added to the attestation
     * record.  The value is a 128-bit hash that is unique per device and per calling application,
     * and changes monthly and on most password resets.  It is computed with:
     *
     *    HMAC_SHA256(T || C || R, HBK)
     *
     * Where:
     *
     *    T is the "temporal counter value", computed by dividing the value of
     *      Tag::CREATION_DATETIME by 2592000000, dropping any remainder.  T changes every 30 days
     *      (2592000000 = 30 * 24 * 60 * 60 * 1000).
     *
     *    C is the value of Tag::ATTESTATION_APPLICATION_ID that is provided to attestKey().
     *
     *    R is 1 if Tag::RESET_SINCE_ID_ROTATION was provided to attestKey or 0 if the tag was not
     *      provided.
     *
     *    HBK is a unique hardware-bound secret known to the secure environment and never revealed
     *    by it.  The secret must contain at least 128 bits of entropy and be unique to the
     *    individual device (probabilistic uniqueness is acceptable).
     *
     *    HMAC_SHA256 is the HMAC function, with SHA-2-256 as the hash.
     *
     * The output of the HMAC function must be truncated to 128 bits.
     *
     * Must be hardware-enforced.
     */
    UNIQUE_ID = (9 << 28) | 707, /* TagType:BYTES */

    /**
     * Tag::ATTESTATION_CHALLENGE is used to deliver a "challenge" value to the attestKey() method,
     * which must place the value in the KeyDescription SEQUENCE of the attestation extension.  See
     * attestKey().
     *
     * Must never appear in KeyCharacteristics.
     */
    ATTESTATION_CHALLENGE = (9 << 28) | 708, /* TagType:BYTES */

    /**
     * Tag::ATTESTATION_APPLICATION_ID identifies the set of applications which may use a key, used
     * only with attestKey().
     *
     * The content of Tag::ATTESTATION_APPLICATION_ID is a DER-encoded ASN.1 structure, with the
     * following schema:
     *
     * AttestationApplicationId ::= SEQUENCE {
     *     packageInfoRecords SET OF PackageInfoRecord,
     *     signatureDigests   SET OF OCTET_STRING,
     * }
     *
     * PackageInfoRecord ::= SEQUENCE {
     *     packageName        OCTET_STRING,
     *     version            INTEGER,
     * }
     *
     * See system/security/keystore/keystore_attestation_id.cpp for details of construction.
     * IKeyMintDevice implementers do not need to create or parse the ASN.1 structure, but only
     * copy the tag value into the attestation record.  The DER-encoded string must not exceed 1 KiB
     * in length.
     *
     * Cannot be hardware-enforced.
     */
    ATTESTATION_APPLICATION_ID = (9 << 28) | 709, /* TagType:BYTES */

    /**
     * Tag::ATTESTATION_ID_BRAND provides the device's brand name, as returned by Build.BRAND in
     * Android, to attestKey().  This field must be set only when requesting attestation of the
     * device's identifiers.
     *
     * If the device does not support ID attestation (or destroyAttestationIds() was previously
     * called and the device can no longer attest its IDs), any key attestation request that
     * includes this tag must fail with ErrorCode::CANNOT_ATTEST_IDS.
     *
     * Must never appear in KeyCharacteristics.
     */
    ATTESTATION_ID_BRAND = (9 << 28) | 710, /* TagType:BYTES */

    /**
     * Tag::ATTESTATION_ID_DEVICE provides the device's device name, as returned by Build.DEVICE in
     * Android, to attestKey().  This field must be set only when requesting attestation of the
     * device's identifiers.
     *
     * If the device does not support ID attestation (or destroyAttestationIds() was previously
     * called and the device can no longer attest its IDs), any key attestation request that
     * includes this tag must fail with ErrorCode::CANNOT_ATTEST_IDS.
     *
     * Must never appear in KeyCharacteristics.
     */
    ATTESTATION_ID_DEVICE = (9 << 28) | 711, /* TagType:BYTES */

    /**
     * Tag::ATTESTATION_ID_PRODUCT provides the device's product name, as returned by Build.PRODUCT
     * in Android, to attestKey().  This field must be set only when requesting attestation of the
     * device's identifiers.
     *
     * If the device does not support ID attestation (or destroyAttestationIds() was previously
     * called and the device can no longer attest its IDs), any key attestation request that
     * includes this tag must fail with ErrorCode::CANNOT_ATTEST_IDS.
     *
     * Must never appear in KeyCharacteristics.
     */
    ATTESTATION_ID_PRODUCT = (9 << 28) | 712, /* TagType:BYTES */

    /**
     * Tag::ATTESTATION_ID_SERIAL the device's serial number.  This field must be set only when
     * requesting attestation of the device's identifiers.
     *
     * If the device does not support ID attestation (or destroyAttestationIds() was previously
     * called and the device can no longer attest its IDs), any key attestation request that
     * includes this tag must fail with ErrorCode::CANNOT_ATTEST_IDS.
     *
     * Must never appear in KeyCharacteristics.
     */
    ATTESTATION_ID_SERIAL = (9 << 28) | 713, /* TagType:BYTES */

    /**
     * Tag::ATTESTATION_ID_IMEI provides the IMEIs for all radios on the device to attestKey().
     * This field must be set only when requesting attestation of the device's identifiers.
     *
     * If the device does not support ID attestation (or destroyAttestationIds() was previously
     * called and the device can no longer attest its IDs), any key attestation request that
     * includes this tag must fail with ErrorCode::CANNOT_ATTEST_IDS.
     *
     * Must never appear in KeyCharacteristics.
     */
    ATTESTATION_ID_IMEI = (9 << 28) | 714, /* TagType:BYTES */

    /**
     * Tag::ATTESTATION_ID_MEID provides the MEIDs for all radios on the device to attestKey().
     * This field must be set only when requesting attestation of the device's identifiers.
     *
     * If the device does not support ID attestation (or destroyAttestationIds() was previously
     * called and the device can no longer attest its IDs), any key attestation request that
     * includes this tag must fail with ErrorCode::CANNOT_ATTEST_IDS.
     *
     * Must never appear in KeyCharacteristics.
     */
    ATTESTATION_ID_MEID = (9 << 28) | 715, /* TagType:BYTES */

    /**
     * Tag::ATTESTATION_ID_MANUFACTURER provides the device's manufacturer name, as returned by
     * Build.MANUFACTURER in Android, to attstKey().  This field must be set only when requesting
     * attestation of the device's identifiers.
     *
     * If the device does not support ID attestation (or destroyAttestationIds() was previously
     * called and the device can no longer attest its IDs), any key attestation request that
     * includes this tag must fail with ErrorCode::CANNOT_ATTEST_IDS.
     *
     * Must never appear in KeyCharacteristics.
     */
    ATTESTATION_ID_MANUFACTURER = (9 << 28) | 716, /* TagType:BYTES */

    /**
     * Tag::ATTESTATION_ID_MODEL provides the device's model name, as returned by Build.MODEL in
     * Android, to attestKey().  This field must be set only when requesting attestation of the
     * device's identifiers.
     *
     * If the device does not support ID attestation (or destroyAttestationIds() was previously
     * called and the device can no longer attest its IDs), any key attestation request that
     * includes this tag must fail with ErrorCode::CANNOT_ATTEST_IDS.
     *
     * Must never appear in KeyCharacteristics.
     */
    ATTESTATION_ID_MODEL = (9 << 28) | 717, /* TagType:BYTES */

    /**
     * Tag::VENDOR_PATCHLEVEL specifies the vendor image security patch level with which the key may
     * be used.  This tag is never sent to the keyMint TA, but is added to the hardware-enforced
     * authorization list by the TA.  Any attempt to use a key with a Tag::VENDOR_PATCHLEVEL value
     * different from the currently-running system patchlevel must cause begin(),
     * getKeyCharacteristics() or exportKey() to return ErrorCode::KEY_REQUIRES_UPGRADE.  See
     * upgradeKey() for details.
     *
     * The value of the tag is an integer of the form YYYYMMDD, where YYYY is the four-digit year of
     * the last update, MM is the two-digit month and DD is the two-digit day of the last
     * update.  For example, for a key generated on an Android device last updated on June 5, 2018,
     * the value would be 20180605.
     *
     * The IKeyMintDevice HAL must read the current vendor patchlevel from the system property
     * ro.vendor.build.security_patch and deliver it to the secure environment when the HAL is first
     * loaded (mechanism is implementation-defined).  The secure environment must not accept another
     * patchlevel until after the next boot.
     *
     * Must be hardware-enforced.
     */
    VENDOR_PATCHLEVEL = (3 << 28) | 718, /* TagType:UINT */

    /**
     * Tag::BOOT_PATCHLEVEL specifies the boot image (kernel) security patch level with which the
     * key may be used.  This tag is never sent to the keyMint TA, but is added to the
     * hardware-enforced authorization list by the TA.  Any attempt to use a key with a
     * Tag::BOOT_PATCHLEVEL value different from the currently-running system patchlevel must
     * cause begin(), getKeyCharacteristics() or exportKey() to return
     * ErrorCode::KEY_REQUIRES_UPGRADE.  See upgradeKey() for details.
     *
     * The value of the tag is an integer of the form YYYYMMDD, where YYYY is the four-digit year of
     * the last update, MM is the two-digit month and DD is the two-digit day of the last
     * update.  For example, for a key generated on an Android device last updated on June 5, 2018,
     * the value would be 20180605.  If the day is not known, 00 may be substituted.
     *
     * During each boot, the bootloader must provide the patch level of the boot image to the secure
     * envirionment (mechanism is implementation-defined).
     *
     * Must be hardware-enforced.
     */
    BOOT_PATCHLEVEL = (3 << 28) | 719, /* TagType:UINT */

    /**
     * DEVICE_UNIQUE_ATTESTATION is an argument to IKeyMintDevice::attestKey().  It indicates that
     * attestation using a device-unique key is requested, rather than a batch key.  When a
     * device-unique key is used, only the attestation certificate is returned; no additional
     * chained certificates are provided.  It's up to the caller to recognize the device-unique
     * signing key.  Only SecurityLevel::STRONGBOX IKeyMintDevices may support device-unique
     * attestations.  SecurityLevel::TRUSTED_ENVIRONMENT IKeyMintDevices must return
     * ErrorCode::INVALID_ARGUMENT if they receive DEVICE_UNIQUE_ATTESTATION.
     * SecurityLevel::STRONGBOX IKeyMintDevices need not support DEVICE_UNIQUE_ATTESTATION, and
     * return ErrorCode::CANNOT_ATTEST_IDS if they do not support it.
     *
     * IKeyMintDevice implementations that support device-unique attestation MUST add the
     * DEVICE_UNIQUE_ATTESTATION tag to device-unique attestations.
     */
    DEVICE_UNIQUE_ATTESTATION = (7 << 28) | 720, /* TagType:BOOL */

    /**
     * IDENTITY_CREDENTIAL_KEY is never used by IKeyMintDevice, is not a valid argument to key
     * generation or any operation, is never returned by any method and is never used in a key
     * attestation.  It is used in attestations produced by the IIdentityCredential HAL when that
     * HAL attests to Credential Keys.  IIdentityCredential produces KeyMint-style attestations.
     */
    IDENTITY_CREDENTIAL_KEY = (7 << 28) | 721, /* TagType:BOOL */

    /**
     * To prevent keys from being compromised if an attacker acquires read access to system / kernel
     * memory, some inline encryption hardware supports protecting storage encryption keys in
     * hardware without software having access to or the ability to set the plaintext keys.
     * Instead, software only sees wrapped version of these keys.
     *
     * STORAGE_KEY is used to denote that a key generated or imported is a key used for storage
     * encryption. Keys of this type can either be generated or imported or secure imported using
     * keyMint. exportKey() can be used to re-wrap storage key with a per-boot ephemeral key
     * wrapped key once the key characteristics are enforced.
     *
     * Keys with this tag cannot be used for any operation within keyMint.
     * ErrorCode::INVALID_OPERATION is returned when a key with Tag::STORAGE_KEY is provided to
     * begin().
     */
    STORAGE_KEY = (7 << 28) | 722, /* TagType:BOOL */

    /**
     * Tag::ASSOCIATED_DATA Provides "associated data" for AES-GCM encryption or decryption.  This
     * tag is provided to update and specifies data that is not encrypted/decrypted, but is used in
     * computing the GCM tag.
     *
     * Must never appear KeyCharacteristics.
     */
    ASSOCIATED_DATA = (9 << 28) | 1000, /* TagType:BYTES */

    /**
     * Tag::NONCE is used to provide or return a nonce or Initialization Vector (IV) for AES-GCM,
     * AES-CBC, AES-CTR, or 3DES-CBC encryption or decryption.  This tag is provided to begin during
     * encryption and decryption operations.  It is only provided to begin if the key has
     * Tag::CALLER_NONCE.  If not provided, an appropriate nonce or IV must be randomly generated by
     * KeyMint and returned from begin.
     *
     * The value is a blob, an arbitrary-length array of bytes.  Allowed lengths depend on the mode:
     * GCM nonces are 12 bytes in length; AES-CBC and AES-CTR IVs are 16 bytes in length, 3DES-CBC
     * IVs are 8 bytes in length.
     *
     * Must never appear in KeyCharacteristics.
     */
    NONCE = (9 << 28) | 1001, /* TagType:BYTES */

    /**
     * Tag::MAC_LENGTH provides the requested length of a MAC or GCM authentication tag, in bits.
     *
     * The value is the MAC length in bits.  It must be a multiple of 8 and at least as large as the
     * value of Tag::MIN_MAC_LENGTH associated with the key.  Otherwise, begin() must return
     * ErrorCode::INVALID_MAC_LENGTH.
     *
     * Must never appear in KeyCharacteristics.
     */
    MAC_LENGTH = (3 << 28) | 1003, /* TagType:UINT */

    /**
     * Tag::RESET_SINCE_ID_ROTATION specifies whether the device has been factory reset since the
     * last unique ID rotation.  Used for key attestation.
     *
     * Must never appear in KeyCharacteristics.
     */
    RESET_SINCE_ID_ROTATION = (7 << 28) | 1004, /* TagType:BOOL */

    /**
     * Tag::CONFIRMATION_TOKEN is used to deliver a cryptographic token proving that the user
     * confirmed a signing request.  The content is a full-length HMAC-SHA256 value.  See the
     * ConfirmationUI HAL for details of token computation.
     *
     * Must never appear in KeyCharacteristics.
     */
    CONFIRMATION_TOKEN = (9 << 28) | 1005, /* TagType:BYTES */
}
