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
#include "js_paragraph_builder.h"
#include "../text/js_text_utils.h"
#include "rosen_text/typography_create.h"
#include "fontcollection_napi/js_fontcollection.h"
#include "paragraph_napi/js_paragraph.h"
#include "utils/log.h"

namespace OHOS::Rosen {

thread_local napi_ref JsParagraphBuilder::constructor_ = nullptr;
const std::string CLASS_NAME = "ParagraphBuilder";
napi_value JsParagraphBuilder::Constructor(napi_env env, napi_callback_info info)
{
    size_t argCount = ARGC_TWO;
    napi_value jsThis = nullptr;
    napi_value argv[ARGC_TWO] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argCount, argv, nullptr, nullptr);
    if (status != napi_ok || argCount < ARGC_ONE || argCount > ARGC_TWO) {
        LOGE("JsParagraphBuilder::Constructor Argc is invalid: %{public}zu", argCount);
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }


    TypographyStyle typographyStyle;
    if (!GetParagraphStyleFromJS(env, argv[0], typographyStyle)) {
        return nullptr;
    }

    // 通过函数获取typographyStyle
    //napi_unwrap(env, argv[0], reinterpret_cast<void**>(&typographyStyle));

    std::shared_ptr<FontCollection> fontCollection = nullptr;
    // FontCollection* fontCollection = nullptr;
    status = napi_unwrap(env, argv[1], reinterpret_cast<void**>(&fontCollection));
    if (status != napi_ok) {
        LOGE("JsParagraphBuilder::Constructor get fontCollection error");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Get fontCollection error.");
    }

    std::unique_ptr<TypographyCreate> typographyCreate = TypographyCreate::Create(typographyStyle, fontCollection);
    if (!typographyCreate) {
        LOGE("JsParagraphBuilder::Constructor TypographyCreate Create error");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "TypographyCreate Create error.");
    }

    JsParagraphBuilder* jsParagraphBuilder = new(std::nothrow) JsParagraphBuilder(std::move(typographyCreate));
    if (!jsParagraphBuilder) {
        LOGE("JsParagraphBuilder::Constructor jsParagraphBuilder Create error");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "JsParagraphBuilder Create error.");
    }

    status = napi_wrap(env, jsThis, jsParagraphBuilder,
        JsParagraphBuilder::Destructor, nullptr, nullptr);
    if (status != napi_ok) {
        delete jsParagraphBuilder;
        LOGE("JsParagraphBuilder::Constructor Failed to wrap native instance");
        return nullptr;
    }
    return jsThis;
}

napi_value JsParagraphBuilder::Init(napi_env env, napi_value exportObj)
{
    napi_property_descriptor properties[] = {
        // DECLARE_NAPI_FUNCTION("disableFallback", JsParagraphBuilder::DisableFallback),
        DECLARE_NAPI_FUNCTION("constructor", Constructor),
        DECLARE_NAPI_FUNCTION("pushStyle", PushStyle),
        DECLARE_NAPI_FUNCTION("addText", AddText),
        DECLARE_NAPI_FUNCTION("popStyle", PopStyle),
        DECLARE_NAPI_FUNCTION("build", Build),
    };

    napi_value constructor = nullptr;
    napi_status status = napi_define_class(env, CLASS_NAME.c_str(), NAPI_AUTO_LENGTH, Constructor, nullptr,
        sizeof(properties) / sizeof(properties[0]), properties, &constructor);
    if (status != napi_ok) {
        LOGE("JsParagraphBuilder::Init Failed to define FontCollection class");
        return nullptr;
    }

    status = napi_create_reference(env, constructor, 1, &constructor_);
    if (status != napi_ok) {
        LOGE("JsParagraphBuilder::Init Failed to create reference of constructor");
        return nullptr;
    }

    status = napi_set_named_property(env, exportObj, CLASS_NAME.c_str(), constructor);
    if (status != napi_ok) {
        LOGE("JsParagraphBuilder::Init Failed to set constructor");
        return nullptr;
    }
    return exportObj;
}

void JsParagraphBuilder::Destructor(napi_env env, void* nativeObject, void* finalize)
{
    (void)finalize;
    if (nativeObject != nullptr) {
        JsParagraphBuilder* napi = reinterpret_cast<JsParagraphBuilder*>(nativeObject);
        delete napi;
    }
}

napi_value JsParagraphBuilder::PushStyle(napi_env env, napi_callback_info info)
{
    JsParagraphBuilder* jsParagraphBuilder = CheckParamsAndGetThis<JsParagraphBuilder>(env, info);
    std::shared_ptr<TypographyCreate> typographyCreate = jsParagraphBuilder->GetTypographyCreate();
    if (typographyCreate == nullptr) {
        return nullptr;
    }
    size_t argc = ARGC_ONE;
    napi_value argv[ARGC_ONE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    //检查 JS侧的参数 
    if (status != napi_ok || argc < ARGC_ONE) {
        LOGE("JsParagraphBuilder::PushStyle Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    //检查 PushStyle的首个参数
    napi_valuetype valueType = napi_undefined;
    if (argv[0] == nullptr || napi_typeof(env, argv[0], &valueType) != napi_ok || valueType != napi_object) {
        LOGE("JsParagraphBuilder::PushStyle Argv[0] is invalid");
        return NapiGetUndefined(env);
    }
    TextStyle textStyle;
    if (GetTextStyleFromJS(env, argv[0], textStyle)) {
        typographyCreate->PushStyle(textStyle);
    }
    return NapiGetUndefined(env);
}

napi_value JsParagraphBuilder::AddText(napi_env env, napi_callback_info info)
{
    JsParagraphBuilder* jsParagraphBuilder = CheckParamsAndGetThis<JsParagraphBuilder>(env, info);
    std::shared_ptr<TypographyCreate> typographyCreate = jsParagraphBuilder->GetTypographyCreate();
    if (typographyCreate == nullptr) {
        return nullptr;
    }
    size_t argc = ARGC_ONE;
    napi_value argv[ARGC_ONE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    //检查 JS侧的参数 
    if (status != napi_ok || argc < ARGC_ONE) {
        LOGE("JsParagraphBuilder::AddText Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    //检查 PushStyle的首个参数
    napi_valuetype valueType = napi_undefined;
    if (argv[0] == nullptr || napi_typeof(env, argv[0], &valueType) != napi_ok || valueType != napi_object) {
        LOGE("JsParagraphBuilder::AddText Argv[0] is invalid");
        return NapiGetUndefined(env);
    }
    std::string text = "";
    if (ConvertFromJsValue(env, argv[0], text)) {
        typographyCreate->AppendText(Str8ToStr16(text));
    }
    return NapiGetUndefined(env);
}

napi_value JsParagraphBuilder::PopStyle(napi_env env, napi_callback_info info)
{
    JsParagraphBuilder* jsParagraphBuilder = CheckParamsAndGetThis<JsParagraphBuilder>(env, info);
    std::shared_ptr<TypographyCreate> typographyCreate = jsParagraphBuilder->GetTypographyCreate();
    if (typographyCreate == nullptr) {
        return nullptr;
    }
    typographyCreate->PopStyle();
    return NapiGetUndefined(env);
}

napi_value JsParagraphBuilder::Build(napi_env env, napi_callback_info info)
{
    JsParagraphBuilder* jsParagraphBuilder = CheckParamsAndGetThis<JsParagraphBuilder>(env, info);
    std::shared_ptr<TypographyCreate> typographyCreate = jsParagraphBuilder->GetTypographyCreate();
    if (typographyCreate == nullptr) {
        return nullptr;
    }
    // std::unique_ptr<Typography> typography(typographyCreate->CreateTypography());

    return CreateJsTypography(env, typographyCreate->CreateTypography());
}

napi_value JsParagraphBuilder::CreateJsTypography(napi_env env, std::unique_ptr<Typography> typography)
{
    napi_value constructor = nullptr;
    napi_value result = nullptr;
    napi_status status = napi_get_reference_value(env, constructor_, &constructor);
    if (status == napi_ok) {
        JsParagraph* jsParagraph = new(std::nothrow) JsParagraph(std::move(typography));
        status = napi_wrap(env, result, jsParagraph, JsParagraph::Destructor, nullptr, nullptr);
        if (status != napi_ok) {
            delete jsParagraph;
            LOGE("JsParagraph::Constructor Failed to wrap native instance");
            return nullptr;
        }
    }
    return result;
}

std::unique_ptr<TypographyCreate> JsParagraphBuilder::GetTypographyCreate()
{
    std::unique_ptr<TypographyCreate> typographyCreate = std::move(typographyCreate_);
    return typographyCreate;
}

// std::unique_ptr<TypographyCreate> JsParagraphBuilder::GetTypographyCreate()
// {
//     return std::make_unique<TypographyCreate>(std::move(typographyCreate_));;
// }

std::shared_ptr<FontCollection> JsParagraphBuilder::GetFontCollection()
{
    return fontCollection_;
}

JsParagraphBuilder::JsParagraphBuilder(std::unique_ptr<TypographyCreate> typographyCreate)
    : typographyCreate_(std::move(typographyCreate))
{

}

// napi_value JsParagraphBuilder::DisableFallback(napi_env env, napi_callback_info info)
// {
//     JsParagraphBuilder* me = CheckParamsAndGetThis<JsParagraphBuilder>(env, info);
//     return (me != nullptr) ? me->OnDisableFallback(env, info) : nullptr;
// }

// napi_value JsParagraphBuilder::OnDisableFallback(napi_env env, napi_callback_info info)
// {
//     if (fontCollection_ == nullptr) {
//         LOGE("JsParagraphBuilder::OnClose path is nullptr");
//         return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
//     }

//     fontCollection_->DisableFallback();
//     return NapiGetUndefined(env);
// }

//} // namespace Drawing
} // namespace OHOS::Rosen
