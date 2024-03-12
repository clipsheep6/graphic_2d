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

#ifndef OHOS_ROSEN_JS_TEXT_STYLE_H
#define OHOS_ROSEN_JS_TEXT_STYLE_H

#include <native_engine/native_engine.h>
#include <native_engine/native_value.h>
#include "js_text_utils.h"
#include "text_style.h"
#include "paragraph_style_napi/js_paragraphstyle.h"
#include "pen_napi/js_pen.h"
#include "brush_napi/js_brush.h"
#include "enum_napi/text_enum_napi.h"

namespace OHOS::Rosen {
class JsTextStyle final {
public:
    JsTextStyle();

    static napi_value Init(napi_env env, napi_value exportObj);
    static napi_value Constructor(napi_env env, napi_callback_info info);
    static void Destructor(napi_env env, void* nativeObject, void* finalize);
    static napi_value SetColor(napi_env env, napi_callback_info info);
    static napi_value SetTextDecoration(napi_env env, napi_callback_info info);
    static napi_value SetDecorationColor(napi_env env, napi_callback_info info);
    static napi_value SetTextDecorationStyle(napi_env env, napi_callback_info info);
    static napi_value SetDecorationThicknessScale(napi_env env, napi_callback_info info);
    static napi_value SetFontWeight(napi_env env, napi_callback_info info);
    static napi_value SetTextBaseline(napi_env env, napi_callback_info info);
    static napi_value SetFontFamilies(napi_env env, napi_callback_info info);
    static napi_value SetFontSize(napi_env env, napi_callback_info info);
    static napi_value SetLetterSpacing(napi_env env, napi_callback_info info);
    static napi_value SetWordSpacing(napi_env env, napi_callback_info info);
    static napi_value SetHeightScale(napi_env env, napi_callback_info info);
    static napi_value SetHalfLeading(napi_env env, napi_callback_info info);
    static napi_value SetHeightOnly(napi_env env, napi_callback_info info);
    static napi_value SetEllipsis(napi_env env, napi_callback_info info);
    static napi_value SetEllipsisModal(napi_env env, napi_callback_info info);
    static napi_value SetLocale(napi_env env, napi_callback_info info);
    static napi_value SetForegroundBrush(napi_env env, napi_callback_info info);
    static napi_value SetBackgroundBrush(napi_env env, napi_callback_info info);
    static napi_value SetForegroundPen(napi_env env, napi_callback_info info);
    static napi_value SetBackgroundPen(napi_env env, napi_callback_info info);
    static napi_value SetTextShadow(napi_env env, napi_callback_info info);
    std::shared_ptr<TextStyle> GetTextStyle();
private:
    static thread_local napi_ref constructor_;
    napi_value OnSetColor(napi_env env, napi_callback_info info);
    napi_value OnSetTextDecoration(napi_env env, napi_callback_info info);
    napi_value OnSetDecorationColor(napi_env env, napi_callback_info info);
    napi_value OnSetTextDecorationStyle(napi_env env, napi_callback_info info);
    napi_value OnSetDecorationThicknessScale(napi_env env, napi_callback_info info);
    napi_value OnSetFontWeight(napi_env env, napi_callback_info info);
    napi_value OnSetTextBaseline(napi_env env, napi_callback_info info);
    napi_value OnSetFontFamilies(napi_env env, napi_callback_info info);
    napi_value OnSetFontSize(napi_env env, napi_callback_info info);
    napi_value OnSetLetterSpacing(napi_env env, napi_callback_info info);
    napi_value OnSetWordSpacing(napi_env env, napi_callback_info info);
    napi_value OnSetHeightScale(napi_env env, napi_callback_info info);
    napi_value OnSetHalfLeading(napi_env env, napi_callback_info info);
    napi_value OnSetHeightOnly(napi_env env, napi_callback_info info);
    napi_value OnSetEllipsis(napi_env env, napi_callback_info info);
    napi_value OnSetEllipsisModal(napi_env env, napi_callback_info info);
    napi_value OnSetLocale(napi_env env, napi_callback_info info);
    napi_value OnSetForegroundBrush(napi_env env, napi_callback_info info);
    napi_value OnSetBackgroundBrush(napi_env env, napi_callback_info info);
    napi_value OnSetForegroundPen(napi_env env, napi_callback_info info);
    napi_value OnSetBackgroundPen(napi_env env, napi_callback_info info);
    napi_value OnSetTextShadow(napi_env env, napi_callback_info info);
    bool GetDoubleData(napi_env env, napi_callback_info info, double& target);
    bool GetUint32Data(napi_env env, napi_callback_info info, uint32_t& target);
    bool GetBoolData(napi_env env, napi_callback_info info, bool& target);
    bool GetNameData(napi_env env, napi_callback_info info, bool& target);
    bool GetBoolDataFromName(napi_env env, napi_value element, const char* name, bool& target);
    bool GetUint32DataFromName(napi_env env, napi_value element, const char* name,uint32_t& target);
    bool GetDoubleDataFromName(napi_env env, napi_value element, const char* name, double& target);
    void ScanNapiValue(napi_env env, napi_value argv, uint32_t content);
    std::shared_ptr<Drawing::Color> GetColorFromInt32(napi_env env, napi_value element, const char* name);
    std::shared_ptr<TextStyle> m_textStyle = nullptr;
};
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_JS_FONTCOLLECTION_H