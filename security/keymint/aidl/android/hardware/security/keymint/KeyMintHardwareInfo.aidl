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

package android.hardware.security.keymint;

import android.hardware.security.keymint.SecurityLevel;

/**
 * KeyMintHardwareInfo is the hardware information returned by calling KeyMint getHardwareInfo()
 */
@VintfStability
parcelable KeyMintHardwareInfo {
    /**
     * Implementation version of the keymint hardware.  The version number is implementation
     * defined, and not necessarily globally meaningful.  The version is used to distinguish
     * between different versions of a given implementation.
     * TODO(seleneh) add the version related info to the code.
     */
    int versionNumber;

    /* securityLevel is the security level of the IKeyMintDevice implementation accessed
     * through this aidl package.  */
    SecurityLevel securityLevel;

    /* keyMintName is the name of the IKeyMintDevice implementation.  */
    @utf8InCpp String keyMintName;

    /* keyMintAuthorName is the name of the author of the IKeyMintDevice implementation
     *         (organization name, not individual). This name is implementation defined,
     *         so it can be used to distinguish between different implementations from the
     *         same author.
     */
    @utf8InCpp String keyMintAuthorName;
}
