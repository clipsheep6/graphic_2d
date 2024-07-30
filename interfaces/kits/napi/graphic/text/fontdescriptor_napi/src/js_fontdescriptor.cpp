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

#include "js_fontdescriptor.h"
#include "napi_async_work.h"
#include "napi_common.h"

namespace OHOS::Rosen {
napi_value JsFontDescriptor::Init(napi_env env, napi_value exportObj)
{
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_STATIC_FUNCTION("matchingFontDescriptorsAsync", JsFontDescriptor::MatchingFontDescriptorsAsync),
    };
    
    NAPI_CHECK_AND_THROW_ERROR(
        napi_define_properties(env, exportObj, sizeof(properties) / sizeof(properties[0]), properties) == napi_ok,
        TextErrorCode::ERROR_INVALID_PARAM, "napi_define_properties failed");
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
    struct MatchFontDescriptorsContext : public ContextBase {
        FontDescriptorPtr matchDesc {nullptr};
        std::set<FontDescriptorPtr> matchResult;
    };

    sptr<MatchFontDescriptorsContext> cb = new (std::nothrow) MatchFontDescriptorsContext();
    auto inputParser = [env, cb](size_t argc, napi_value *argv) {
        TEXT_ERROR_CHECK(argc == ARGC_ONE, return, "wrong number of parameters");
        cb->matchDesc = std::make_shared<TextEngine::FontParser::FontDescriptor>();
        TEXT_ERROR_CHECK(CheckAndConvertProperty(env, argv[0], "postScriptName", cb->matchDesc->postScriptName), return,
            "converter postScriptName failed");
        TEXT_ERROR_CHECK(CheckAndConvertProperty(env, argv[0], "fullName", cb->matchDesc->fullName), return,
            "converter fullName failed");
        TEXT_ERROR_CHECK(CheckAndConvertProperty(env, argv[0], "fontFamily", cb->matchDesc->fontFamily), return,
            "converter fontFamily failed");
        TEXT_ERROR_CHECK(CheckAndConvertProperty(env, argv[0], "fontSubfamily", cb->matchDesc->fontSubfamily), return,
            "converter fontSubfamily failed");
        TEXT_ERROR_CHECK(CheckAndConvertProperty(env, argv[0], "weight", cb->matchDesc->weight), return,
            "converter weight failed");
        TEXT_ERROR_CHECK(CheckAndConvertProperty(env, argv[0], "width", cb->matchDesc->width), return,
            "converter width failed");
        TEXT_ERROR_CHECK(CheckAndConvertProperty(env, argv[0], "italic", cb->matchDesc->italic), return,
            "converter italic failed");
        TEXT_ERROR_CHECK(CheckAndConvertProperty(env, argv[0], "monoSpace", cb->matchDesc->monoSpace), return,
            "converter monoSpace failed");
        TEXT_ERROR_CHECK(CheckAndConvertProperty(env, argv[0], "symbolic", cb->matchDesc->symbolic), return,
            "converter symbolic failed");
        TEXT_ERROR_CHECK(CheckAndConvertProperty(env, argv[0], "typeStyle", cb->matchDesc->typeStyle), return,
            "converter typeStyle failed");
        cb->status = napi_ok;
    };
    cb->GetCbInfo(env, info, inputParser);
    auto complete = [env, cb](napi_value& output) {
        output = JsFontDescriptor::CreateFontDescriptorArray(env, cb->matchResult);
    };
    return NapiAsyncWork::Enqueue(env, cb, "MatchingFontDescriptorsAsync", nullptr, complete);
}

bool JsFontDescriptor::SetProperty(napi_env env, napi_value object, const char* name, napi_value value)
{
    TEXT_ERROR_CHECK(napi_set_named_property(env, object, name, value) == napi_ok, return false,
        "set %{public}s failed", name);
    return true;
}

bool JsFontDescriptor::CreateAndSetProperties(napi_env env, napi_value fontDescriptor, FontDescriptorPtr item)
{
    std::vector<std::pair<const char*, std::variant<int, double, bool, std::string>>> properties = {
        {"path", item->path},
        {"postScriptName", item->postScriptName},
        {"fullName", item->fullName},
        {"fontFamily", item->fontFamily},
        {"fontSubfamily", item->fontSubfamily},
        {"weight", item->weight},
        {"width", item->width},
        {"italic", item->italic},
        {"monoSpace", item->monoSpace},
        {"symbolic", item->symbolic},
        {"size", item->size},
        {"typeStyle", item->typeStyle},
    };

    for (const auto& prop : properties) {
        TEXT_CHECK(std::visit([&](auto& p) -> bool {
            TEXT_CHECK(SetProperty(env, fontDescriptor, prop.first, CreateJsValue(env, p)), return false);
            return true;
        }, prop.second), return false);
    }
    return true;
}

napi_value JsFontDescriptor::CreateFontDescriptorArray(napi_env env, std::set<FontDescriptorPtr>& result)
{
    TEXT_ERROR_CHECK(env != nullptr, return nullptr, "env is nullptr");
    napi_value descArray = nullptr;
    TEXT_ERROR_CHECK(napi_create_array(env, &descArray) == napi_ok, return nullptr, "napi_create_array failed");
    uint32_t index = 0;
    for (const auto& item : result) {
        napi_value fontDescriptor = nullptr;
        TEXT_ERROR_CHECK(napi_create_object(env, &fontDescriptor) == napi_ok, return nullptr,
            "napi_create_object failed");
        TEXT_CHECK(CreateAndSetProperties(env, fontDescriptor, item), return nullptr);
        TEXT_ERROR_CHECK(napi_set_element(env, descArray, index++, fontDescriptor) == napi_ok, return nullptr,
            "napi_set_element failed");
    }
    return descArray;
}
}