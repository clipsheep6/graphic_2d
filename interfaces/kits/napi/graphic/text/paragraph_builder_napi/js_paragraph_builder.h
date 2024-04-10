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

#ifndef OHOS_ROSEN_JS_PARAGRAPH_BUILDER_H
#define OHOS_ROSEN_JS_PARAGRAPH_BUILDER_H

#include <native_engine/native_engine.h>
#include <native_engine/native_value.h>
#include "rosen_text/typography_create.h"
#include "rosen_text/text_style.h"
#include "rosen_text/font_collection.h"

namespace OHOS::Rosen {
//namespace Drawing {

struct JsPlaceholderSpan {
    double width;
    double height;
    PlaceholderVerticalAlignment alignment;
    TextBaseline baseline;
    double baselineOffset;
};

class JsParagraphBuilder final {
public:
    JsParagraphBuilder(std::unique_ptr<TypographyCreate> typographyCreate);

    static napi_value Init(napi_env env, napi_value exportObj);
    static napi_value Constructor(napi_env env, napi_callback_info info);
    static void Destructor(napi_env env, void* nativeObject, void* finalize);
    //static napi_value DisableFallback(napi_env env, napi_callback_info info);
    static napi_value PushStyle(napi_env env, napi_callback_info info);
    static napi_value AddText(napi_env env, napi_callback_info info);
    static napi_value PopStyle(napi_env env, napi_callback_info info);
    static napi_value Build(napi_env env, napi_callback_info info);
    static napi_value CreateJsTypography(napi_env env, const std::unique_ptr<Typography> typography);
    std::unique_ptr<TypographyCreate> GetTypographyCreate();

    std::shared_ptr<FontCollection> GetFontCollection();
private:
    static thread_local napi_ref constructor_;
    //对传进来的 TextStyle对象进行解析
    //static TextStyle Convert(const JsTextStyle& jsStyle);
    napi_value OnDisableFallback(napi_env env, napi_callback_info info);
    std::unique_ptr<TypographyCreate> typographyCreate_ = nullptr;
    std::shared_ptr<FontCollection> fontCollection_ = nullptr;
    
};
//} // namespace Drawing
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_JS_PARAGRAPH_BUILDER_H