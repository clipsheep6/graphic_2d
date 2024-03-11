/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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

#include <map>
#include <vector>
#include "utils/log.h"
#include "text_enum_napi.h"
namespace OHOS::Rosen {
struct JsEnumInt {
    std::string_view enumName;
    int32_t enumInt;
};

static const std::vector<struct JsEnumInt> g_textAlign = {
    { "LEFT", static_cast<int32_t>(TextAlign::LEFT) },
    { "RIGHT", static_cast<int32_t>(TextAlign::RIGHT) },
    { "CENTER", static_cast<int32_t>(TextAlign::CENTER) },
    { "JUSTIFY", static_cast<int32_t>(TextAlign::JUSTIFY) },
    { "START", static_cast<int32_t>(TextAlign::START) },
    { "END", static_cast<int32_t>(TextAlign::END) },
};

static const std::vector<struct JsEnumInt> g_textDecorationStyle = {
    { "SOLID", static_cast<int32_t>(TextDecorationStyle::SOLID) },
    { "DOUBLE", static_cast<int32_t>(TextDecorationStyle::DOUBLE) },
    { "DOTTED", static_cast<int32_t>(TextDecorationStyle::DOTTED) },
    { "DASHED", static_cast<int32_t>(TextDecorationStyle::DASHED) },
    { "WAVY", static_cast<int32_t>(TextDecorationStyle::WAVY) },
};

static const std::vector<struct JsEnumInt> g_fontWeight = {
    { "W100", static_cast<int32_t>(FontWeight::W100) },
    { "W200", static_cast<int32_t>(FontWeight::W200) },
    { "W300", static_cast<int32_t>(FontWeight::W300) },
    { "W400", static_cast<int32_t>(FontWeight::W400) },
    { "W500", static_cast<int32_t>(FontWeight::W500) },
    { "W600", static_cast<int32_t>(FontWeight::W600) },
    { "W700", static_cast<int32_t>(FontWeight::W700) },
    { "W800", static_cast<int32_t>(FontWeight::W800) },
    { "W900", static_cast<int32_t>(FontWeight::W900) },
};

static const std::vector<struct JsEnumInt> g_fontStyle = {
    { "NORMAL", static_cast<int32_t>(FontStyle::NORMAL) },
    { "ITALIC", static_cast<int32_t>(FontStyle::ITALIC) },
};

static const std::vector<struct JsEnumInt> g_textBaseline = {
    { "ALPHABETIC", static_cast<int32_t>(TextBaseline::ALPHABETIC) },
    { "IDEOGRAPHIC", static_cast<int32_t>(TextBaseline::IDEOGRAPHIC) },
};

static const std::vector<struct JsEnumInt> g_textDirection = {
    { "RTL", static_cast<int32_t>(TextDirection::RTL) },
    { "LTR", static_cast<int32_t>(TextDirection::LTR) },
};

static const std::vector<struct JsEnumInt> g_wordBreakType = {
    { "NORMAL", static_cast<int32_t>(WordBreakType::NORMAL) },
    { "BREAK_ALL", static_cast<int32_t>(WordBreakType::BREAK_ALL) },
    { "BREAK_WORD", static_cast<int32_t>(WordBreakType::BREAK_WORD) },
};

static const std::vector<struct JsEnumInt> g_breakStrategy = {
    { "GREEDY", static_cast<int32_t>(BreakStrategy::GREEDY) },
    { "HIGH_QUALITY", static_cast<int32_t>(BreakStrategy::HIGH_QUALITY) },
    { "BALANCED", static_cast<int32_t>(BreakStrategy::BALANCED) },
};

static const std::vector<struct JsEnumInt> g_ellipsisModal = {
    { "HEAD", static_cast<int32_t>(EllipsisModal::HEAD) },
    { "MIDDLE", static_cast<int32_t>(EllipsisModal::MIDDLE) },
    { "TAIL", static_cast<int32_t>(EllipsisModal::TAIL) },
};

static const std::vector<struct JsEnumInt> g_textDecoration = {
    { "NONE", static_cast<int32_t>(TextDecoration::NONE) },
    { "UNDERLINE", static_cast<int32_t>(TextDecoration::UNDERLINE) },
    { "OVERLINE", static_cast<int32_t>(TextDecoration::OVERLINE) },
    { "LINE_THROUGH", static_cast<int32_t>(TextDecoration::LINE_THROUGH) },
};

static const std::map<std::string_view, const std::vector<struct JsEnumInt>&> g_intEnumClassMap = {
    { "TextAlign", g_textAlign },
    { "TextDecorationStyle", g_textDecorationStyle },
    { "FontWeight", g_fontWeight },
    { "FontStyle", g_fontStyle },
    { "TextBaseline", g_textBaseline },
    { "TextDirection", g_textDirection },
    { "WordBreakType", g_wordBreakType },
    { "BreakStrategy", g_breakStrategy },
    { "EllipsisModal", g_ellipsisModal },
    { "TextDecoration", g_textDecoration },
};


napi_value JsEnum::JsEnumIntInit(napi_env env, napi_value exports)
{
    for (auto it = g_intEnumClassMap.begin(); it != g_intEnumClassMap.end(); it++) {
        auto &enumClassName = it->first;
        auto &enumItemVec = it->second;
        auto vecSize = enumItemVec.size();
        std::vector<napi_value> value;
        value.resize(vecSize);
        for (size_t index = 0; index < vecSize; ++index) {
            napi_create_int32(env, enumItemVec[index].enumInt, &value[index]);
        }

        std::vector<napi_property_descriptor> property;
        property.resize(vecSize);
        for (size_t index = 0; index < vecSize; ++index) {
            property[index] = napi_property_descriptor DECLARE_NAPI_STATIC_PROPERTY(
                enumItemVec[index].enumName.data(), value[index]);
        }

        auto napiConstructor = [](napi_env env, napi_callback_info info) {
            napi_value jsThis = nullptr;
            napi_get_cb_info(env, info, nullptr, nullptr, &jsThis, nullptr);
            return jsThis;
        };

        napi_value result = nullptr;
        napi_status napiStatus = napi_define_class(env, enumClassName.data(), NAPI_AUTO_LENGTH, napiConstructor,
            nullptr, property.size(), property.data(), &result);
        if (napiStatus != napi_ok) {
            LOGE("[NAPI] Failed to define enum");
            return nullptr;
        }

        napiStatus = napi_set_named_property(env, exports, enumClassName.data(), result);
        if (napiStatus != napi_ok) {
            LOGE("[NAPI] Failed to set result");
            return nullptr;
        }
    }
    return exports;
}

napi_value JsEnum::Init(napi_env env, napi_value exports)
{
    JsEnumIntInit(env, exports);
    return exports;
}
} // namespace OHOS::Rosen