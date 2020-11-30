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

#ifndef HARDWARE_INTERFACES_KEYMINT_SUPPORT_INCLUDE_KEYMINT_TAGS_H_
#define HARDWARE_INTERFACES_KEYMINT_SUPPORT_INCLUDE_KEYMINT_TAGS_H_

#include <android/hardware/keymint/Algorithm.h>
#include <android/hardware/keymint/BlockMode.h>
#include <android/hardware/keymint/Digest.h>
#include <android/hardware/keymint/EcCurve.h>
#include <android/hardware/keymint/HardwareAuthenticatorType.h>
#include <android/hardware/keymint/KeyOrigin.h>
#include <android/hardware/keymint/KeyParameter.h>
#include <android/hardware/keymint/KeyPurpose.h>
#include <android/hardware/keymint/PaddingMode.h>
#include <android/hardware/keymint/SecurityLevel.h>
#include <android/hardware/keymint/Tag.h>
#include <android/hardware/keymint/TagType.h>

namespace android::hardware::keymint {

using android::hardware::keymint::KeyParameter;
using android::hardware::keymint::Tag;
using android::hardware::keymint::TagType;

// The following create the numeric values that KM_TAG_PADDING and KM_TAG_DIGEST used to have.  We
// need these old values to be able to support old keys that use them.
// TODO(seleneh) we should delete this code when we stop supporting keymaster1
// and deletes it.
static const int32_t KM_TAG_DIGEST_OLD = static_cast<int32_t>(TagType::ENUM) | 5;
static const int32_t KM_TAG_PADDING_OLD = static_cast<int32_t>(TagType::ENUM) | 7;

constexpr TagType typeFromTag(Tag tag) {
    return static_cast<TagType>(static_cast<uint32_t>(tag) & static_cast<uint32_t>(0xf0000000));
}

/**
 * TypedTag is a templatized version of Tag, which provides compile-time checking of
 * keymint tag types. Instances are convertible to Tag, so they can be used wherever
 * Tag is expected, and because they encode the tag type it's possible to create
 * function overloads that only operate on tags with a particular type.
 */
template <TagType tag_type, Tag tag>
struct TypedTag {
    inline TypedTag() {
        // Ensure that it's impossible to create a TypedTag instance whose 'tag' doesn't have type
        // 'tag_type'.  Attempting to instantiate a tag with the wrong type will result in a compile
        // error (no match for template specialization StaticAssert<false>), with no run-time cost.
        static_assert(typeFromTag(tag) == tag_type, "mismatch between tag and tag_type");
    }
    operator Tag() const { return tag; }
    int32_t maskedTag() { return static_cast<uint32_t>(tag) & 0x0FFFFFFF; }
};

template <Tag tag>
struct Tag2TypedTag {
    typedef TypedTag<typeFromTag(tag), tag> type;
};

#define DECLARE_TYPED_TAG(name)                                    \
    typedef typename Tag2TypedTag<Tag::name>::type TAG_##name##_t; \
    static TAG_##name##_t TAG_##name;

DECLARE_TYPED_TAG(ACTIVE_DATETIME);
DECLARE_TYPED_TAG(ALGORITHM);
DECLARE_TYPED_TAG(ALLOW_WHILE_ON_BODY);
DECLARE_TYPED_TAG(APPLICATION_DATA);
DECLARE_TYPED_TAG(APPLICATION_ID);
DECLARE_TYPED_TAG(ASSOCIATED_DATA);
DECLARE_TYPED_TAG(ATTESTATION_APPLICATION_ID);
DECLARE_TYPED_TAG(ATTESTATION_CHALLENGE);
DECLARE_TYPED_TAG(ATTESTATION_ID_BRAND);
DECLARE_TYPED_TAG(ATTESTATION_ID_DEVICE);
DECLARE_TYPED_TAG(ATTESTATION_ID_PRODUCT);
DECLARE_TYPED_TAG(ATTESTATION_ID_MANUFACTURER);
DECLARE_TYPED_TAG(ATTESTATION_ID_MODEL);
DECLARE_TYPED_TAG(AUTH_TIMEOUT);
DECLARE_TYPED_TAG(BLOCK_MODE);
DECLARE_TYPED_TAG(BOOTLOADER_ONLY);
DECLARE_TYPED_TAG(BOOT_PATCHLEVEL);
DECLARE_TYPED_TAG(CALLER_NONCE);
DECLARE_TYPED_TAG(CONFIRMATION_TOKEN);
DECLARE_TYPED_TAG(CREATION_DATETIME);
DECLARE_TYPED_TAG(DEVICE_UNIQUE_ATTESTATION);
DECLARE_TYPED_TAG(DIGEST);
DECLARE_TYPED_TAG(EARLY_BOOT_ONLY);
DECLARE_TYPED_TAG(EC_CURVE);
DECLARE_TYPED_TAG(HARDWARE_TYPE);
DECLARE_TYPED_TAG(IDENTITY_CREDENTIAL_KEY);
DECLARE_TYPED_TAG(INCLUDE_UNIQUE_ID);
DECLARE_TYPED_TAG(INVALID);
DECLARE_TYPED_TAG(KEY_SIZE);
DECLARE_TYPED_TAG(MAC_LENGTH);
DECLARE_TYPED_TAG(MAX_USES_PER_BOOT);
DECLARE_TYPED_TAG(MIN_MAC_LENGTH);
DECLARE_TYPED_TAG(MIN_SECONDS_BETWEEN_OPS);
DECLARE_TYPED_TAG(NONCE);
DECLARE_TYPED_TAG(NO_AUTH_REQUIRED);
DECLARE_TYPED_TAG(ORIGIN);
DECLARE_TYPED_TAG(ORIGINATION_EXPIRE_DATETIME);
DECLARE_TYPED_TAG(OS_PATCHLEVEL);
DECLARE_TYPED_TAG(OS_VERSION);
DECLARE_TYPED_TAG(PADDING);
DECLARE_TYPED_TAG(PURPOSE);
DECLARE_TYPED_TAG(RESET_SINCE_ID_ROTATION);
DECLARE_TYPED_TAG(ROLLBACK_RESISTANCE);
DECLARE_TYPED_TAG(ROOT_OF_TRUST);
DECLARE_TYPED_TAG(RSA_PUBLIC_EXPONENT);
DECLARE_TYPED_TAG(STORAGE_KEY);
DECLARE_TYPED_TAG(TRUSTED_CONFIRMATION_REQUIRED);
DECLARE_TYPED_TAG(TRUSTED_USER_PRESENCE_REQUIRED);
DECLARE_TYPED_TAG(UNIQUE_ID);
DECLARE_TYPED_TAG(UNLOCKED_DEVICE_REQUIRED);
DECLARE_TYPED_TAG(USAGE_EXPIRE_DATETIME);
DECLARE_TYPED_TAG(USER_AUTH_TYPE);
DECLARE_TYPED_TAG(USER_ID);
DECLARE_TYPED_TAG(USER_SECURE_ID);
DECLARE_TYPED_TAG(VENDOR_PATCHLEVEL);

template <typename... Elems>
struct MetaList {};

using all_tags_t = MetaList<
        TAG_INVALID_t, TAG_KEY_SIZE_t, TAG_MAC_LENGTH_t, TAG_CALLER_NONCE_t, TAG_MIN_MAC_LENGTH_t,
        TAG_RSA_PUBLIC_EXPONENT_t, TAG_INCLUDE_UNIQUE_ID_t, TAG_ACTIVE_DATETIME_t,
        TAG_ORIGINATION_EXPIRE_DATETIME_t, TAG_USAGE_EXPIRE_DATETIME_t,
        TAG_MIN_SECONDS_BETWEEN_OPS_t, TAG_MAX_USES_PER_BOOT_t, TAG_USER_ID_t, TAG_USER_SECURE_ID_t,
        TAG_NO_AUTH_REQUIRED_t, TAG_AUTH_TIMEOUT_t, TAG_ALLOW_WHILE_ON_BODY_t,
        TAG_UNLOCKED_DEVICE_REQUIRED_t, TAG_APPLICATION_ID_t, TAG_APPLICATION_DATA_t,
        TAG_CREATION_DATETIME_t, TAG_ROLLBACK_RESISTANCE_t, TAG_HARDWARE_TYPE_t,
        TAG_ROOT_OF_TRUST_t, TAG_ASSOCIATED_DATA_t, TAG_NONCE_t, TAG_BOOTLOADER_ONLY_t,
        TAG_OS_VERSION_t, TAG_OS_PATCHLEVEL_t, TAG_UNIQUE_ID_t, TAG_ATTESTATION_CHALLENGE_t,
        TAG_ATTESTATION_APPLICATION_ID_t, TAG_ATTESTATION_ID_BRAND_t, TAG_ATTESTATION_ID_DEVICE_t,
        TAG_ATTESTATION_ID_PRODUCT_t, TAG_ATTESTATION_ID_MANUFACTURER_t, TAG_ATTESTATION_ID_MODEL_t,
        TAG_RESET_SINCE_ID_ROTATION_t, TAG_PURPOSE_t, TAG_ALGORITHM_t, TAG_BLOCK_MODE_t,
        TAG_DIGEST_t, TAG_PADDING_t, TAG_ORIGIN_t, TAG_USER_AUTH_TYPE_t, TAG_EC_CURVE_t,
        TAG_BOOT_PATCHLEVEL_t, TAG_VENDOR_PATCHLEVEL_t, TAG_TRUSTED_CONFIRMATION_REQUIRED_t,
        TAG_TRUSTED_USER_PRESENCE_REQUIRED_t>;

template <typename TypedTagType>
struct TypedTag2ValueType;

#define MAKE_TAG_VALUE_ACCESSOR(tag_type, field_name)                              \
    template <Tag tag>                                                             \
    struct TypedTag2ValueType<TypedTag<tag_type, tag>> {                           \
        typedef decltype(static_cast<KeyParameter*>(nullptr)->field_name) type;    \
    };                                                                             \
    template <Tag tag>                                                             \
    inline auto accessTagValue(TypedTag<tag_type, tag>, const KeyParameter& param) \
            ->const decltype(param.field_name)& {                                  \
        return param.field_name;                                                   \
    }                                                                              \
    template <Tag tag>                                                             \
    inline auto accessTagValue(TypedTag<tag_type, tag>, KeyParameter& param)       \
            ->decltype(param.field_name)& {                                        \
        return param.field_name;                                                   \
    }

MAKE_TAG_VALUE_ACCESSOR(TagType::ULONG, longInteger)
MAKE_TAG_VALUE_ACCESSOR(TagType::ULONG_REP, longInteger)
MAKE_TAG_VALUE_ACCESSOR(TagType::DATE, longInteger)
MAKE_TAG_VALUE_ACCESSOR(TagType::UINT, integer)
MAKE_TAG_VALUE_ACCESSOR(TagType::UINT_REP, integer)
MAKE_TAG_VALUE_ACCESSOR(TagType::BOOL, boolValue)
MAKE_TAG_VALUE_ACCESSOR(TagType::BYTES, blob)
MAKE_TAG_VALUE_ACCESSOR(TagType::BIGNUM, blob)

//  TODO(seleneh) change these MAKE_TAG_ENUM_VALUE_ACCESSOR back to the 2 parameter
//  version when aidl supports union
#define MAKE_TAG_ENUM_VALUE_ACCESSOR(typed_tag, field_name, field_type)                 \
    template <>                                                                         \
    struct TypedTag2ValueType<decltype(typed_tag)> {                                    \
        typedef field_type type;                                                        \
    };                                                                                  \
    inline auto accessTagValue(decltype(typed_tag), const KeyParameter& param)          \
            ->const field_type& {                                                       \
        return *reinterpret_cast<const field_type*>(&param.field_name);                 \
    }                                                                                   \
    inline auto accessTagValue(decltype(typed_tag), KeyParameter& param)->field_type& { \
        return *reinterpret_cast<field_type*>(&param.field_name);                       \
    }

MAKE_TAG_ENUM_VALUE_ACCESSOR(TAG_ALGORITHM, integer, Algorithm)
MAKE_TAG_ENUM_VALUE_ACCESSOR(TAG_BLOCK_MODE, integer, BlockMode)
MAKE_TAG_ENUM_VALUE_ACCESSOR(TAG_DIGEST, integer, Digest)
MAKE_TAG_ENUM_VALUE_ACCESSOR(TAG_EC_CURVE, integer, EcCurve)
MAKE_TAG_ENUM_VALUE_ACCESSOR(TAG_ORIGIN, integer, KeyOrigin)
MAKE_TAG_ENUM_VALUE_ACCESSOR(TAG_PADDING, integer, PaddingMode)
MAKE_TAG_ENUM_VALUE_ACCESSOR(TAG_PURPOSE, integer, KeyPurpose)
MAKE_TAG_ENUM_VALUE_ACCESSOR(TAG_USER_AUTH_TYPE, integer, HardwareAuthenticatorType)
MAKE_TAG_ENUM_VALUE_ACCESSOR(TAG_HARDWARE_TYPE, integer, SecurityLevel)

template <TagType tag_type, Tag tag, typename ValueT>
inline KeyParameter makeKeyParameter(TypedTag<tag_type, tag> ttag, ValueT&& value) {
    KeyParameter param;
    param.tag = tag;
    param.longInteger = 0;
    accessTagValue(ttag, param) = std::forward<ValueT>(value);
    return param;
}

// the boolean case
template <Tag tag>
inline KeyParameter makeKeyParameter(TypedTag<TagType::BOOL, tag>) {
    KeyParameter param;
    param.tag = tag;
    param.boolValue = true;
    return param;
}

template <typename... Pack>
struct FirstOrNoneHelper;
template <typename First>
struct FirstOrNoneHelper<First> {
    typedef First type;
};
template <>
struct FirstOrNoneHelper<> {
    struct type {};
};

template <typename... Pack>
using FirstOrNone = typename FirstOrNoneHelper<Pack...>::type;

template <TagType tag_type, Tag tag, typename... Args>
inline KeyParameter Authorization(TypedTag<tag_type, tag> ttag, Args&&... args) {
    static_assert(tag_type != TagType::BOOL || (sizeof...(args) == 0),
                  "TagType::BOOL Authorizations do not take parameters. Presence is truth.");
    static_assert(tag_type == TagType::BOOL || (sizeof...(args) == 1),
                  "Authorization other then TagType::BOOL take exactly one parameter.");
    static_assert(
            tag_type == TagType::BOOL ||
                    std::is_convertible<
                            std::remove_cv_t<std::remove_reference_t<FirstOrNone<Args...>>>,
                            typename TypedTag2ValueType<TypedTag<tag_type, tag>>::type>::value,
            "Invalid argument type for given tag.");

    return makeKeyParameter(ttag, std::forward<Args>(args)...);
}

/**
 * This class wraps a (mostly return) value and stores whether or not the wrapped value is valid out
 * of band. Note that if the wrapped value is a reference it is unsafe to access the value if
 * !isOk(). If the wrapped type is a pointer or value and !isOk(), it is still safe to access the
 * wrapped value. In this case the pointer will be NULL though, and the value will be default
 * constructed.
 *
 * TODO(seleneh) replace this with std::optional.
 */
template <typename ValueT>
class NullOr {
    using internal_t = std::conditional_t<std::is_lvalue_reference<ValueT>::value,
                                          std::remove_reference_t<ValueT>*, ValueT>;

    struct pointer_initializer {
        static std::nullptr_t init() { return nullptr; }
    };
    struct value_initializer {
        static ValueT init() { return ValueT(); }
    };
    struct value_pointer_deref_t {
        static ValueT& deref(ValueT& v) { return v; }
    };
    struct reference_deref_t {
        static auto& deref(internal_t v) { return *v; }
    };
    using initializer_t = std::conditional_t<std::is_lvalue_reference<ValueT>::value ||
                                                     std::is_pointer<ValueT>::value,
                                             pointer_initializer, value_initializer>;
    using deref_t = std::conditional_t<std::is_lvalue_reference<ValueT>::value, reference_deref_t,
                                       value_pointer_deref_t>;

  public:
    NullOr() : value_(initializer_t::init()), null_(true) {}
    template <typename T>
    NullOr(T&& value, typename std::enable_if<
                              !std::is_lvalue_reference<ValueT>::value &&
                                      std::is_same<std::decay_t<ValueT>, std::decay_t<T>>::value,
                              int>::type = 0)
        : value_(std::forward<ValueT>(value)), null_(false) {}
    template <typename T>
    NullOr(T& value, typename std::enable_if<
                             std::is_lvalue_reference<ValueT>::value &&
                                     std::is_same<std::decay_t<ValueT>, std::decay_t<T>>::value,
                             int>::type = 0)
        : value_(&value), null_(false) {}

    bool isOk() const { return !null_; }

    const ValueT& value() const& { return deref_t::deref(value_); }
    ValueT& value() & { return deref_t::deref(value_); }
    ValueT&& value() && { return std::move(deref_t::deref(value_)); }

  private:
    internal_t value_;
    bool null_;
};

template <typename T>
std::remove_reference_t<T> NullOrOr(T&& v) {
    if (v.isOk()) return v;
    return {};
}

template <typename Head, typename... Tail>
std::remove_reference_t<Head> NullOrOr(Head&& head, Tail&&... tail) {
    if (head.isOk()) return head;
    return NullOrOr(std::forward<Tail>(tail)...);
}

template <typename Default, typename Wrapped>
std::remove_reference_t<Wrapped> defaultOr(NullOr<Wrapped>&& optional, Default&& def) {
    static_assert(std::is_convertible<std::remove_reference_t<Default>,
                                      std::remove_reference_t<Wrapped>>::value,
                  "Type of default value must match the type wrapped by NullOr");
    if (optional.isOk()) return optional.value();
    return def;
}

template <TagType tag_type, Tag tag>
inline NullOr<const typename TypedTag2ValueType<TypedTag<tag_type, tag>>::type&> authorizationValue(
        TypedTag<tag_type, tag> ttag, const KeyParameter& param) {
    if (tag != param.tag) return {};
    return accessTagValue(ttag, param);
}

}  // namespace android::hardware::keymint

namespace std {

using namespace android::hardware::keymint;

// Aidl generates KeyParameter operator<, >, ==, != for cpp translation but not ndk
// translations.  So we cannot straight forward overload these operators.
// However we need our custom comparison for KeyParameters.  So we will
// overload std::less, equal_to instead.
template <>
struct std::less<KeyParameter> {
    bool operator()(const KeyParameter& a, const KeyParameter& b) const {
        if (a.tag != b.tag) return a.tag < b.tag;
        int retval;
        switch (typeFromTag(a.tag)) {
            case TagType::INVALID:
            case TagType::BOOL:
                return false;
            case TagType::ENUM:
            case TagType::ENUM_REP:
            case TagType::UINT:
            case TagType::UINT_REP:
                return a.integer < b.integer;
            case TagType::ULONG:
            case TagType::ULONG_REP:
            case TagType::DATE:
                return a.longInteger < b.longInteger;
            case TagType::BIGNUM:
            case TagType::BYTES:
                // Handle the empty cases.
                if (a.blob.size() == 0) return b.blob.size() != 0;
                if (b.blob.size() == 0) return false;
                retval = memcmp(&a.blob[0], &b.blob[0], std::min(a.blob.size(), b.blob.size()));
                // if one is the prefix of the other the longer wins
                if (retval == 0) return a.blob.size() < b.blob.size();
                // Otherwise a is less if a is less.
                else
                    return retval < 0;
        }
        return false;
    }
};

template <>
struct std::equal_to<KeyParameter> {
    bool operator()(const KeyParameter& a, const KeyParameter& b) const {
        if (a.tag != b.tag) {
            return false;
        }
        switch (typeFromTag(a.tag)) {
            case TagType::INVALID:
            case TagType::BOOL:
                return true;
            case TagType::ENUM:
            case TagType::ENUM_REP:
            case TagType::UINT:
            case TagType::UINT_REP:
                return a.integer == b.integer;
            case TagType::ULONG:
            case TagType::ULONG_REP:
            case TagType::DATE:
                return a.longInteger == b.longInteger;
            case TagType::BIGNUM:
            case TagType::BYTES:
                if (a.blob.size() != b.blob.size()) return false;
                return a.blob.size() == 0 || memcmp(&a.blob[0], &b.blob[0], a.blob.size()) == 0;
        }
        return false;
    }
};

}  // namespace std

#endif  // HARDWARE_INTERFACES_KEYMINT_SUPPORT_INCLUDE_KEYMINT_TAGS_H_
