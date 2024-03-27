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
#include "utils/log.h"
#include "js_paragraphstyle.h"
#include "js_text_utils.h"

namespace OHOS::Rosen {
thread_local napi_ref JsParagraphStyle::constructor_ = nullptr;
const std::string CLASS_NAME = "ParagraphStyle";
napi_value JsParagraphStyle::Constructor(napi_env env, napi_callback_info info)
{
    LOGE("ParagraphStyleTest| into Constructor");
    size_t argCount = 0;
    napi_value jsThis = nullptr;
    if (napi_get_cb_info(env, info, &argCount, nullptr, &jsThis, nullptr) != napi_ok) {
        return nullptr;
    }

    JsParagraphStyle* jsParagraphStyle = new(std::nothrow) JsParagraphStyle();
    if (napi_wrap(env, jsThis, jsParagraphStyle, JsParagraphStyle::Destructor,
        nullptr, nullptr) != napi_ok) {
        delete jsParagraphStyle;
        return nullptr;
    }
    return jsThis;
}

napi_value JsParagraphStyle::Init(napi_env env, napi_value exportObj)
{
    LOGE("ParagraphStyleTest| into Init");
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_GETTER_SETTER("structStyle", JsParagraphStyle::JsGetStructStyle,
            JsParagraphStyle::JsSetStructStyle),
    };

    napi_value constructor = nullptr;
    napi_status status = napi_define_class(env, CLASS_NAME.c_str(), NAPI_AUTO_LENGTH, Constructor, nullptr,
        sizeof(properties) / sizeof(properties[0]), properties, &constructor);
    if (status != napi_ok) {
        return nullptr;
    }

    if (napi_create_reference(env, constructor, 1, &constructor_) != napi_ok) {
        return nullptr;
    }

    if (napi_set_named_property(env, exportObj, CLASS_NAME.c_str(), constructor) != napi_ok) {
        return nullptr;
    }
    return exportObj;
}

void JsParagraphStyle::Destructor(napi_env env, void *nativeObject, void *finalize)
{
    (void)finalize;
    if (nativeObject != nullptr) {
        JsParagraphStyle *napi = reinterpret_cast<JsParagraphStyle *>(nativeObject);
        delete napi;
    }
}

JsParagraphStyle::JsParagraphStyle()
{
    m_paragraphStyle = std::make_shared<TypographyStyle>();
}

bool JsParagraphStyle::SetStrutStyleFontFamilies(napi_env env, napi_value fontFamiliesField)
{
    uint32_t arrayLength = 0;
    if (napi_get_array_length(env, fontFamiliesField, &arrayLength) != napi_ok) {
        return false;
    }

    for (uint32_t i = 0; i < arrayLength; i++) {
        napi_value element;
        if (napi_get_element(env, fontFamiliesField, i, &element) != napi_ok) {
            return false;
        }

        size_t bufferSize = 0;
        if (napi_get_value_string_utf8(env, element, nullptr, 0, &bufferSize) != napi_ok) {
            return false;
        }

        size_t strLen = 0;
        auto buffer = std::make_unique<char[]>(bufferSize + 1);
        if (buffer.get() == nullptr) {
            return false;
        }
        if (napi_get_value_string_utf8(env, element, buffer.get(),
            bufferSize + 1, &strLen) != napi_ok) {
            return false;
        }
        std::string value(buffer.get());
        LOGE("ParagraphStyleTest| set name = %s",value.c_str());
        m_paragraphStyle->lineStyleFontFamilies.push_back(value);
    }
    return true;
}

napi_value JsParagraphStyle::JsSetStructStyle(napi_env env, napi_callback_info info)
{
    JsParagraphStyle* me = CheckParamsAndGetThis<JsParagraphStyle>(env, info);
    return (me != nullptr) ? me->OnSetStructStyle(env, info) : nullptr;
}

napi_value JsParagraphStyle::JsGetStructStyle(napi_env env, napi_callback_info info)
{
    JsParagraphStyle* me = CheckParamsAndGetThis<JsParagraphStyle>(env, info);
    return (me != nullptr) ? me->OnGetStructStyle(env, info) : nullptr;
}

napi_value JsParagraphStyle::OnGetStructStyle(napi_env env, napi_callback_info info)
{
    LOGE("ParagraphStyleTest| into OnGetStructStyle");
    if (m_paragraphStyle == nullptr) {
        return nullptr;
    }

    napi_value objValue = nullptr;
    napi_create_object(env, &objValue);

    std::vector<std::string>& fontFamily = m_paragraphStyle->lineStyleFontFamilies;
    napi_value jsArray;
    napi_create_array_with_length(env, fontFamily.size(), &jsArray);
    if (!fontFamily.size() || objValue==nullptr) {
        return nullptr;
    }
    for (size_t further = 0; further < fontFamily.size(); further++) {
        napi_value jsValue;
        napi_create_string_utf8(env, fontFamily[further].c_str(), NAPI_AUTO_LENGTH, &jsValue);
        napi_set_element(env, jsArray, further, jsValue);
    }
    napi_set_named_property(env, objValue, "fontFamilies", jsArray);
    return objValue;
}

napi_value JsParagraphStyle::OnSetStructStyle(napi_env env, napi_callback_info info)
{
    LOGE("ParagraphStyleTest| into OnSetStructStyle");
    if (m_paragraphStyle == nullptr) {
        return nullptr;
    }

    size_t argc = ARGC_ONE;
    napi_value argv[ARGC_ONE] = {nullptr};
    if (napi_get_cb_info(env, info, &argc, argv, nullptr,
        nullptr) != napi_ok || argc < ARGC_ONE) {
        return nullptr;
    }
    napi_value fontFamiliesField = nullptr;
    if (napi_get_named_property(env, argv[0], "fontFamilies",
        &fontFamiliesField) != napi_ok) {
        return nullptr;
    }
    if (!SetStrutStyleFontFamilies(env, fontFamiliesField)) {
        return nullptr;
    }
    return NapiGetUndefined(env);
}
} // namespace OHOS::Rosen