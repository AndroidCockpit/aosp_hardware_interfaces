/*
 * Copyright 2020 The Android Open Source Project
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

#include <android/hardware/keymint/ErrorCode.h>
#include <android/hardware/keymint/IKeyMintDevice.h>

#include <keymintSupport/attestation_record.h>
#include <keymintSupport/authorization_set.h>
#include <keymintSupport/openssl_utils.h>

namespace android {
namespace hardware {
namespace keymint {

using android::hardware::keymint::KeyParameter;
using android::hardware::keymint::Tag;
using android::hardware::keymint::TAG_ALGORITHM;

class AuthorizationSet;

/**
 * The OID for Android attestation records.  For the curious, it breaks down as follows:
 *
 * 1 = ISO
 * 3 = org
 * 6 = DoD (Huh? OIDs are weird.)
 * 1 = IANA
 * 4 = Private
 * 1 = Enterprises
 * 11129 = Google
 * 2 = Google security
 * 1 = certificate extension
 * 17 = Android attestation extension.
 */
static const char kAttestionRecordOid[] = "1.3.6.1.4.1.11129.2.1.17";

enum keymint_verified_boot_t {
    KM_VERIFIED_BOOT_VERIFIED = 0,
    KM_VERIFIED_BOOT_SELF_SIGNED = 1,
    KM_VERIFIED_BOOT_UNVERIFIED = 2,
    KM_VERIFIED_BOOT_FAILED = 3,
};

struct RootOfTrust {
    SecurityLevel security_level;
    vector<uint8_t> verified_boot_key;
    vector<uint8_t> verified_boot_hash;
    keymint_verified_boot_t verified_boot_state;
    bool device_locked;
};

struct AttestationRecord {
    RootOfTrust root_of_trust;
    uint32_t attestation_version;
    SecurityLevel attestation_security_level;
    uint32_t keymint_version;
    SecurityLevel keymint_security_level;
    std::vector<uint8_t> attestation_challenge;
    AuthorizationSet software_enforced;
    AuthorizationSet hardware_enforced;
    std::vector<uint8_t> unique_id;
};

ErrorCode parse_attestation_record(const uint8_t* asn1_key_desc, size_t asn1_key_desc_len,
                                   uint32_t* attestation_version,  //
                                   SecurityLevel* attestation_security_level,
                                   uint32_t* keymint_version, SecurityLevel* keymint_security_level,
                                   std::vector<uint8_t>* attestation_challenge,
                                   AuthorizationSet* software_enforced,
                                   AuthorizationSet* tee_enforced,  //
                                   std::vector<uint8_t>* unique_id);

ErrorCode parse_root_of_trust(const uint8_t* asn1_key_desc, size_t asn1_key_desc_len,
                              std::vector<uint8_t>* verified_boot_key,
                              keymint_verified_boot_t* verified_boot_state, bool* device_locked,
                              std::vector<uint8_t>* verified_boot_hash);

}  // namespace keymint
}  // namespace hardware
}  // namespace android
