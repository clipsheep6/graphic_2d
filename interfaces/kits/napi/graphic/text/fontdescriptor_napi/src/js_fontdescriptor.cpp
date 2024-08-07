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
#include "fontdescriptor_mgr.h"
#include "napi_async_work.h"
#include "napi_common.h"

namespace OHOS::Rosen {
napi_value JsFontDescriptor::Init(napi_env env, napi_value exportObj)
{
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_STATIC_FUNCTION("matchingFontDescriptors", JsFontDescriptor::MatchingFontDescriptorsAsync),
		DECLARE_NAPI_STATIC_FUNCTION("getSystemFontList", JsFontDescriptor::GetSystemFontList),
        DECLARE_NAPI_STATIC_FUNCTION("getFontDescriptorByName", JsFontDescriptor::GetFontDescriptorByName),
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

    auto executor = [cb]() {
        FontDescriptorMgrIns->MatchingFontDescriptors(cb->matchDesc, cb->matchResult);
    };

    auto complete = [env, cb](napi_value& output) {
        output = JsFontDescriptor::CreateFontDescriptorArray(env, cb->matchResult);
    };
    return NapiAsyncWork::Enqueue(env, cb, "MatchingFontDescriptorsAsync", executor, complete);
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

napi_value JsFontDescriptor::CreateFontDescriptor(napi_env env, FontDescriptorPtr& result)
{
    TEXT_ERROR_CHECK(env != nullptr, return nullptr, "env is nullptr");
    napi_value fontDescriptor = nullptr;
    TEXT_ERROR_CHECK(napi_create_object(env, &fontDescriptor) == napi_ok, return nullptr,
        "napi_create_object failed");
    TEXT_CHECK(CreateAndSetProperties(env, fontDescriptor, result), return nullptr);
    return fontDescriptor;
}

napi_value JsFontDescriptor::GetSystemFontList(napi_env env, napi_callback_info info)
{
    struct GetSystemFontListContext : public ContextBase {
        TextEngine::FontParser::SystemFontType systemFontType;
        std::set<std::string> fontList;
    };

    sptr<GetSystemFontListContext> cb = new (std::nothrow) GetSystemFontListContext();

    auto inputParser = [env, cb](size_t argc, napi_value *argv) {
        TEXT_ERROR_CHECK(argv != nullptr, return, "inputParser argv is nullptr");
        TEXT_ERROR_CHECK(argc == ARGC_ONE, , "wrong number of parameters");
        NAPI_CHECK_ARGS_RETURN_VOID(cb, ConvertFromJsValue(env, argv[0], cb->systemFontType), napi_invalid_arg,
             "inputParser fullName is invalid", TextErrorCode::ERROR_INVALID_PARAM);
        cb->status = napi_ok;
    };

    cb->GetCbInfo(env, info, inputParser);

    auto executor = [cb]() {
        FontDescriptorMgrIns->GetSystemFontList(cb->systemFontType, cb->fontList);
    };

    auto complete = [env, cb](napi_value& output) {
        output = JsFontDescriptor::CreateFontList(env, cb->fontList);
    };

    return NapiAsyncWork::Enqueue(env, cb, "GetSystemFontList", executor, complete);
}

napi_value JsFontDescriptor::CreateFontList(napi_env env, std::set<std::string>& fontList)
{
    TEXT_ERROR_CHECK(env != nullptr, return nullptr, "env is nullptr");
    napi_value fullNameArray = nullptr;
    TEXT_ERROR_CHECK(napi_create_array(env, &fullNameArray) == napi_ok, return nullptr, "napi_create_array failed");
    uint32_t index = 0;
    for (const auto& item : fontList) {
        napi_value fullName = nullptr;
        TEXT_ERROR_CHECK(napi_create_string_utf8(env, item.c_str(), NAPI_AUTO_LENGTH, &fullName)
            == napi_ok, return nullptr, "%{public}d: napi_create_string_utf8 failed", __LINE__);
        TEXT_ERROR_CHECK(napi_set_element(env, fullNameArray, index++, fullName) == napi_ok, return nullptr,
            "%{public}d: napi_set_element failed", __LINE__);
    }
    return fullNameArray;
}

napi_value JsFontDescriptor::GetFontDescriptorByName(napi_env env, napi_callback_info info)
{
    struct GetFontDescriptorContext : public ContextBase {
        std::string fullName;
        FontDescriptorPtr resultDesc {nullptr};
    };

    sptr<GetFontDescriptorContext> cb = new (std::nothrow) GetFontDescriptorContext();

    auto inputParser = [env, cb](size_t argc, napi_value *argv) {
        TEXT_ERROR_CHECK(argv != nullptr, return, "inputParser argv is nullptr");
        TEXT_ERROR_CHECK(argc == ARGC_ONE, , "wrong number of parameters");
        NAPI_CHECK_ARGS_RETURN_VOID(cb, ConvertFromJsValue(env, argv[0], cb->fullName), napi_invalid_arg,
            "inputParser fullName is invalid", TextErrorCode::ERROR_INVALID_PARAM);
        cb->status = napi_ok;
    };

    cb->GetCbInfo(env, info, inputParser);

    auto executor = [cb]() {
        FontDescriptorMgrIns->GetFontDescriptorByName(cb->fullName, cb->resultDesc);
    };

    auto complete = [env, cb](napi_value& output) {
        output = JsFontDescriptor::CreateFontDescriptor(env, cb->resultDesc);
    };

    return NapiAsyncWork::Enqueue(env, cb, "GetFontDescriptorByName", executor, complete);
}
}