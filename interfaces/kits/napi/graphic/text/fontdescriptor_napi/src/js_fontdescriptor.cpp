/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <fstream>
#include "js_fontdescriptor.h"
#include "fontdescriptor_mgr.h"

#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <uv.h>

namespace OHOS::Rosen {

napi_value JsFontDescriptor::Init(napi_env env, napi_value exportObj)
{
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_STATIC_FUNCTION("matchingFontDescriptorsAsync", JsFontDescriptor::MatchingFontDescriptorsAsync),
    };
    NAPI_CHECK_AND_THROW_ERROR(
        napi_define_properties(env, exportObj, sizeof(properties) / sizeof(properties[0]), properties) == napi_ok,
        "napi_define_properties failed");
    return exportObj;
}

JsFontDescriptor::JsFontDescriptor() {}

template <typename T>
bool JsFontDescriptor::CheckAndConvertProperty(napi_env env, napi_value obj, const std::string& fieldName, T& out)
{
    bool hasValue = false;
    TEXT_ERROR_CHECK(napi_has_named_property(env, obj, fieldName.c_str(), &hasValue) == napi_ok, return false,
        "napi_has_named_property falied");
    if (hasValue) {
        napi_value napiVal = nullptr;
        TEXT_ERROR_CHECK(napi_get_named_property(env, obj, fieldName.c_str(), &napiVal) == napi_ok, return false,
            "napi_get_named_property falied");
        return ConvertFromJsValue(env, napiVal, out);
    }
    return true;
}

napi_value JsFontDescriptor::MatchingFontDescriptorsAsync(napi_env env, napi_callback_info info)
{
    size_t argc = ARGC_TWO;
    napi_value argv[ARGC_TWO] = {nullptr};
    NAPI_CHECK_AND_THROW_ERROR(napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr) == napi_ok,
        "napi_get_cb_info failed");

    std::shared_ptr<TextEngine::FontParser::FontDescriptor> desc
        = std::make_shared<TextEngine::FontParser::FontDescriptor>();

    NAPI_CHECK_AND_THROW_ERROR(CheckAndConvertProperty(env, argv[0], "postScriptName", desc->postScriptName),
        "converter postScriptName failed");
    NAPI_CHECK_AND_THROW_ERROR(CheckAndConvertProperty(env, argv[0], "fullName", desc->fullName),
        "converter fullName failed");
    NAPI_CHECK_AND_THROW_ERROR(CheckAndConvertProperty(env, argv[0], "fontFamily", desc->fontFamily),
        "converter fontFamily failed");
    NAPI_CHECK_AND_THROW_ERROR(CheckAndConvertProperty(env, argv[0], "fontSubfamily", desc->fontSubfamily),
        "converter fontSubfamily failed");
    NAPI_CHECK_AND_THROW_ERROR(CheckAndConvertProperty(env, argv[0], "weight", desc->weight),
        "converter weight failed");
    NAPI_CHECK_AND_THROW_ERROR(CheckAndConvertProperty(env, argv[0], "width", desc->width),
        "converter width failed");
    NAPI_CHECK_AND_THROW_ERROR(CheckAndConvertProperty(env, argv[0], "italic", desc->italic),
        "converter italic failed");
    NAPI_CHECK_AND_THROW_ERROR(CheckAndConvertProperty(env, argv[0], "monoSpace", desc->monoSpace),
        "converter monoSpace failed");
    NAPI_CHECK_AND_THROW_ERROR(CheckAndConvertProperty(env, argv[0], "symbolic", desc->symbolic),
        "converter symbolic failed");
    NAPI_CHECK_AND_THROW_ERROR(CheckAndConvertProperty(env, argv[0], "typeStyle", desc->typeStyle),
        "converter typeStyle failed");

    napi_value ret = nullptr;
    if (argc > 1) {
        napi_valuetype valueType = napi_undefined;
        NAPI_CHECK_AND_THROW_ERROR(napi_typeof(env, argv[1], &valueType) == napi_ok, "napi_typeof failed");
        if (valueType != napi_function) {
            return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        }
        ret = FontDescriptorMgrIns->MatchingFontDescriptors(env, desc, argv[1]);
    } else {
        ret = FontDescriptorMgrIns->MatchingFontDescriptors(env, desc);
    }

    return ret == nullptr ? NapiGetUndefined(env) : ret;
}

void JsFontDescriptor::EmitMatchFontDescriptorsResult(sptr<CallbackInfo> cb,
    std::set<std::shared_ptr<TextEngine::FontParser::FontDescriptor>>& matchResult)
{
    TEXT_ERROR_CHECK(cb != nullptr, return, "cb is nullptr");
    uv_loop_s *loop = nullptr;
    TEXT_ERROR_CHECK(napi_get_uv_event_loop(cb->env, &loop) == napi_ok, return, "napi_get_uv_event_loop failed");
    uv_work_t *work = new (std::nothrow) uv_work_t;
    TEXT_ERROR_CHECK(work != nullptr, return, "work is nullptr");
    cb->IncStrongRef(nullptr);
    cb->data.matchResult = matchResult;
    work->data = cb.GetRefPtr();

    int32_t ret = 0;
    if (cb->ref == nullptr) {
        ret = uv_queue_work_with_qos(
                loop, work,
                [](uv_work_t *work) {},
                MatchFontDescriptorsPromiseWork, 
                uv_qos_user_initiated);
    } else {
        ret = uv_queue_work_with_qos(
                loop, work,
                [](uv_work_t *work) {},
                MatchFontDescriptorsAsyncWork,
                uv_qos_user_initiated);
    }

    if (ret != 0) {
        TEXT_LOGE("uv_queue_work_with_qos failed");
        DeletePtr<uv_work_t *>(work);
        cb->DecStrongRef(nullptr);
    }
}

void JsFontDescriptor::MatchFontDescriptorsPromiseWork(uv_work_t *work, int32_t status)
{
    TEXT_ERROR_CHECK(work != nullptr, return, "work is nullptr");
    if (work->data == nullptr) {
        DeletePtr<uv_work_t *>(work);
        return;
    }

    sptr<CallbackInfo> cb(static_cast<CallbackInfo *>(work->data));
    
    DeletePtr<uv_work_t *>(work);
    cb->DecStrongRef(nullptr);

    napi_handle_scope scope = nullptr;
    napi_open_handle_scope(cb->env, &scope);
    TEXT_ERROR_CHECK(scope != nullptr, return, "scope is nullptr");

    napi_value callResult = CreateFontDescriptorArray(cb);
    if (callResult == nullptr) {
        napi_close_handle_scope(cb->env, scope);
        return;
    }

    NAPI_CHECK_AND_CLOSE_SCOPE(cb->env, napi_resolve_deferred(cb->env, cb->deferred, callResult), scope,);
    napi_close_handle_scope(cb->env, scope);
}

void JsFontDescriptor::MatchFontDescriptorsAsyncWork(uv_work_t *work, int32_t status)
{
    TEXT_ERROR_CHECK(work != nullptr, return, "work is nullptr");
    if (work->data == nullptr) {
        DeletePtr<uv_work_t *>(work);
        return;
    }

    sptr<CallbackInfo> cb(static_cast<CallbackInfo *>(work->data));
    DeletePtr<uv_work_t *>(work);
    cb->DecStrongRef(nullptr);

    napi_handle_scope scope = nullptr;
    napi_open_handle_scope(cb->env, &scope);
    TEXT_ERROR_CHECK(scope != nullptr, return, "scope is nullptr");

    napi_value callResult[ARGC_TWO] = { nullptr };
    NAPI_CHECK_AND_CLOSE_SCOPE(cb->env, napi_get_undefined(cb->env, &callResult[0]), scope,);
    callResult[1] = CreateFontDescriptorArray(cb);
    if (callResult[1] == nullptr) {
        napi_close_handle_scope(cb->env, scope);
        return;
    }

    napi_value handler = nullptr;
    NAPI_CHECK_AND_CLOSE_SCOPE(cb->env, napi_get_reference_value(cb->env, cb->ref, &handler), scope,);
    napi_value result = nullptr;
    NAPI_CHECK_AND_CLOSE_SCOPE(cb->env, napi_call_function(cb->env, nullptr, handler, ARGC_TWO, callResult, &result),
        scope,);
    NAPI_CHECK_AND_CLOSE_SCOPE(cb->env, napi_delete_reference(cb->env, cb->ref), scope,);
    napi_close_handle_scope(cb->env, scope);
}

napi_value JsFontDescriptor::CreateFontDescriptorArray(sptr<CallbackInfo> cb)
{
    TEXT_ERROR_CHECK(cb != nullptr, return nullptr, "cb is nullptr");
    TEXT_ERROR_CHECK(cb->env != nullptr, return nullptr, "cb->env is nullptr");
    napi_value descArray = nullptr;
    TEXT_ERROR_CHECK(napi_create_array(cb->env, &descArray) == napi_ok, return nullptr, "napi_create_array failed");
    uint32_t index = 0;
    for (const auto& item : cb->data.matchResult) {
        napi_value fontDescriptor = nullptr;
        TEXT_ERROR_CHECK(napi_create_object(cb->env, &fontDescriptor) == napi_ok, return nullptr,
            "%{public}d: napi_create_object failed", __LINE__);
        napi_value path = nullptr;
        TEXT_ERROR_CHECK(napi_create_string_utf8(cb->env, item->path.c_str(), NAPI_AUTO_LENGTH, &path) == napi_ok,
            return nullptr, "%{public}d: napi_create_string_utf8 failed", __LINE__);
        TEXT_ERROR_CHECK(napi_set_named_property(cb->env, fontDescriptor, "path", path) == napi_ok, return nullptr,
            "%{public}d: napi_set_named_property failed", __LINE__);

        napi_value postScriptName = nullptr;
        TEXT_ERROR_CHECK(
            napi_create_string_utf8(cb->env, item->postScriptName.c_str(), NAPI_AUTO_LENGTH, &postScriptName)
                == napi_ok, return nullptr, "%{public}d: napi_create_string_utf8 failed", __LINE__);
        TEXT_ERROR_CHECK(napi_set_named_property(cb->env, fontDescriptor, "postScriptName", postScriptName) == napi_ok,
            return nullptr, "%{public}d: napi_set_named_property failed", __LINE__);

        napi_value fullName = nullptr;
        TEXT_ERROR_CHECK(napi_create_string_utf8(cb->env, item->fullName.c_str(), NAPI_AUTO_LENGTH, &fullName)
            == napi_ok, return nullptr, "%{public}d: napi_create_string_utf8 failed", __LINE__);
        TEXT_ERROR_CHECK(napi_set_named_property(cb->env, fontDescriptor, "fullName", fullName) == napi_ok,
            return nullptr, "%{public}d: napi_set_named_property failed", __LINE__);

        napi_value fontFamily = nullptr;
        TEXT_ERROR_CHECK(napi_create_string_utf8(cb->env, item->fontFamily.c_str(), NAPI_AUTO_LENGTH, &fontFamily)
            == napi_ok, return nullptr, "%{public}d: napi_create_string_utf8 failed", __LINE__);
        TEXT_ERROR_CHECK(napi_set_named_property(cb->env, fontDescriptor, "fontFamily", fontFamily) == napi_ok,
            return nullptr, "%{public}d: napi_set_named_property failed", __LINE__);

        napi_value fontSubfamily = nullptr;
        TEXT_ERROR_CHECK(napi_create_string_utf8(cb->env, item->fontSubfamily.c_str(), NAPI_AUTO_LENGTH, &fontSubfamily)
            == napi_ok, return nullptr, "%{public}d: napi_create_string_utf8 failed", __LINE__);
        TEXT_ERROR_CHECK(napi_set_named_property(cb->env, fontDescriptor, "fontSubfamily", fontSubfamily)
            == napi_ok, return nullptr, "%{public}d: napi_set_named_property failed", __LINE__);

        napi_value weight = nullptr;
        TEXT_ERROR_CHECK(napi_create_int32(cb->env, item->weight, &weight) == napi_ok, return nullptr,
            "%{public}d: napi_create_int32 failed", __LINE__);
        TEXT_ERROR_CHECK(napi_set_named_property(cb->env, fontDescriptor, "weight", weight) == napi_ok, return nullptr,
            "%{public}d: napi_set_named_property failed", __LINE__);

        napi_value width = nullptr;
        TEXT_ERROR_CHECK(napi_create_int32(cb->env, item->width, &width) == napi_ok, return nullptr,
            "%{public}d: napi_create_int32 failed", __LINE__);
        TEXT_ERROR_CHECK(napi_set_named_property(cb->env, fontDescriptor, "width", width) == napi_ok, return nullptr,
            "%{public}d: napi_set_named_property failed", __LINE__);

        napi_value italic = nullptr;
        TEXT_ERROR_CHECK(napi_create_int32(cb->env, item->italic, &italic) == napi_ok, return nullptr,
            "%{public}d: napi_create_int32 failed", __LINE__);
        TEXT_ERROR_CHECK(napi_set_named_property(cb->env, fontDescriptor, "italic", italic) == napi_ok, return nullptr,
            "%{public}d: napi_set_named_property failed", __LINE__);

        napi_value monoSpace = nullptr;
        TEXT_ERROR_CHECK(napi_get_boolean(cb->env, item->monoSpace, &monoSpace) == napi_ok, return nullptr,
            "%{public}d: napi_get_boolean failed", __LINE__);
        TEXT_ERROR_CHECK(napi_set_named_property(cb->env, fontDescriptor, "monoSpace", monoSpace) == napi_ok,
            return nullptr, "%{public}d: napi_set_named_property failed", __LINE__);

        napi_value symbolic = nullptr;
        TEXT_ERROR_CHECK(napi_get_boolean(cb->env, item->symbolic, &symbolic) == napi_ok, return nullptr,
            "%{public}d: napi_get_boolean failed", __LINE__);
        TEXT_ERROR_CHECK(napi_set_named_property(cb->env, fontDescriptor, "symbolic", symbolic) == napi_ok,
            return nullptr, "%{public}d: napi_set_named_property failed", __LINE__);

        napi_value size = nullptr;
        TEXT_ERROR_CHECK(napi_create_double(cb->env, item->size, &size) == napi_ok, return nullptr,
            "%{public}d: napi_create_double failed", __LINE__);
        TEXT_ERROR_CHECK(napi_set_named_property(cb->env, fontDescriptor, "size", size) == napi_ok, return nullptr,
            "%{public}d: napi_set_named_property failed", __LINE__);

        TEXT_ERROR_CHECK(napi_set_element(cb->env, descArray, index++, fontDescriptor) == napi_ok, return nullptr,
            "%{public}d: napi_set_element failed", __LINE__);
    }
    return descArray;
}
}