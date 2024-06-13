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

#include "js_font.h"

#include "native_value.h"

#include "js_drawing_utils.h"
#include "js_typeface.h"

namespace OHOS::Rosen {
namespace Drawing {
thread_local napi_ref JsFont::constructor_ = nullptr;
const std::string CLASS_NAME = "Font";
napi_value JsFont::Init(napi_env env, napi_value exportObj)
{
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("enableSubpixel", JsFont::EnableSubpixel),
        DECLARE_NAPI_FUNCTION("enableEmbolden", JsFont::EnableEmbolden),
        DECLARE_NAPI_FUNCTION("enableLinearMetrics", JsFont::EnableLinearMetrics),
        DECLARE_NAPI_FUNCTION("setBaselineSnap", JsFont::SetBaselineSnap),
        DECLARE_NAPI_FUNCTION("setEmbeddedBitmaps", JsFont::SetEmbeddedBitmaps),
        DECLARE_NAPI_FUNCTION("setForceAutoHinting", JsFont::SetForceAutoHinting),
        DECLARE_NAPI_FUNCTION("setSize", JsFont::SetSize),
        DECLARE_NAPI_FUNCTION("setTypeface", JsFont::SetTypeface),
        DECLARE_NAPI_FUNCTION("getTypeface", JsFont::GetTypeface),
        DECLARE_NAPI_FUNCTION("getSize", JsFont::GetSize),
        DECLARE_NAPI_FUNCTION("getMetrics", JsFont::GetMetrics),
        DECLARE_NAPI_FUNCTION("getWidths", JsFont::GetWidths),
        DECLARE_NAPI_FUNCTION("isBaselineSnap", JsFont::IsBaselineSnap),
        DECLARE_NAPI_FUNCTION("isEmbeddedBitmaps", JsFont::IsEmbeddedBitmaps),
        DECLARE_NAPI_FUNCTION("isForceAutoHinting", JsFont::IsForceAutoHinting),
        DECLARE_NAPI_FUNCTION("measureText", JsFont::MeasureText),
        DECLARE_NAPI_FUNCTION("setScaleX", JsFont::SetScaleX),
        DECLARE_NAPI_FUNCTION("setSkewX", JsFont::SetSkewX),
        DECLARE_NAPI_FUNCTION("textToGlyphs", JsFont::TextToGlyphs),
    };

    napi_value constructor = nullptr;
    napi_status status = napi_define_class(env, CLASS_NAME.c_str(), NAPI_AUTO_LENGTH, Constructor, nullptr,
                                           sizeof(properties) / sizeof(properties[0]), properties, &constructor);
    if (status != napi_ok) {
        ROSEN_LOGE("Failed to define Font class");
        return nullptr;
    }

    status = napi_create_reference(env, constructor, 1, &constructor_);
    if (status != napi_ok) {
        ROSEN_LOGE("Failed to create reference of constructor");
        return nullptr;
    }

    status = napi_set_named_property(env, exportObj, CLASS_NAME.c_str(), constructor);
    if (status != napi_ok) {
        ROSEN_LOGE("Failed to set constructor");
        return nullptr;
    }

    napi_property_descriptor staticProperty[] = {
        DECLARE_NAPI_STATIC_FUNCTION("createFont", JsFont::CreateFont),
    };
    status = napi_define_properties(env, exportObj, 1, staticProperty);
    if (status != napi_ok) {
        ROSEN_LOGE("Failed to define static function");
        return nullptr;
    }
    return exportObj;
}

napi_value JsFont::Constructor(napi_env env, napi_callback_info info)
{
    size_t argCount = 0;
    napi_value jsThis = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argCount, nullptr, &jsThis, nullptr);
    if (status != napi_ok) {
        ROSEN_LOGE("failed to napi_get_cb_info");
        return nullptr;
    }

    std::shared_ptr<Font> font = std::make_shared<Font>();
    font->SetTypeface(JsTypeface::LoadZhCnTypeface());
    JsFont *jsFont = new(std::nothrow) JsFont(font);

    status = napi_wrap(env, jsThis, jsFont,
                       JsFont::Destructor, nullptr, nullptr);
    if (status != napi_ok) {
        delete jsFont;
        ROSEN_LOGE("Failed to wrap native instance");
        return nullptr;
    }
    return jsThis;
}

void JsFont::Destructor(napi_env env, void *nativeObject, void *finalize)
{
    (void)finalize;
    if (nativeObject != nullptr) {
        JsFont *napi = reinterpret_cast<JsFont *>(nativeObject);
        delete napi;
    }
}

napi_value JsFont::CreateFont(napi_env env, napi_callback_info info)
{
    napi_value result = nullptr;
    napi_value constructor = nullptr;
    napi_status status = napi_get_reference_value(env, constructor_, &constructor);
    if (status != napi_ok) {
        ROSEN_LOGE("Failed to get the representation of constructor object");
        return nullptr;
    }

    status = napi_new_instance(env, constructor, 0, nullptr, &result);
    if (status != napi_ok) {
        ROSEN_LOGE("Failed to instantiate JavaScript font instance");
        return nullptr;
    }
    return result;
}

JsFont::~JsFont()
{
    m_font = nullptr;
}

napi_value JsFont::EnableSubpixel(napi_env env, napi_callback_info info)
{
    JsFont* me = CheckParamsAndGetThis<JsFont>(env, info);
    return (me != nullptr) ? me->OnEnableSubpixel(env, info) : nullptr;
}

napi_value JsFont::EnableEmbolden(napi_env env, napi_callback_info info)
{
    JsFont* me = CheckParamsAndGetThis<JsFont>(env, info);
    return (me != nullptr) ? me->OnEnableEmbolden(env, info) : nullptr;
}

napi_value JsFont::EnableLinearMetrics(napi_env env, napi_callback_info info)
{
    JsFont* me = CheckParamsAndGetThis<JsFont>(env, info);
    return (me != nullptr) ? me->OnEnableLinearMetrics(env, info) : nullptr;
}

napi_value JsFont::SetBaselineSnap(napi_env env, napi_callback_info info)
{
    JsFont* me = CheckParamsAndGetThis<JsFont>(env, info);
    return (me != nullptr) ? me->OnSetBaselineSnap(env, info) : nullptr;
}
napi_value JsFont::SetEmbeddedBitmaps(napi_env env, napi_callback_info info)
{
    JsFont* me = CheckParamsAndGetThis<JsFont>(env, info);
    return (me != nullptr) ? me->OnSetEmbeddedBitmaps(env, info) : nullptr;
}
napi_value JsFont::SetForceAutoHinting(napi_env env, napi_callback_info info)
{
    JsFont* me = CheckParamsAndGetThis<JsFont>(env, info);
    return (me != nullptr) ? me->OnSetForceAutoHinting(env, info) : nullptr;
}

napi_value JsFont::SetSize(napi_env env, napi_callback_info info)
{
    JsFont* me = CheckParamsAndGetThis<JsFont>(env, info);
    return (me != nullptr) ? me->OnSetSize(env, info) : nullptr;
}

napi_value JsFont::SetTypeface(napi_env env, napi_callback_info info)
{
    JsFont* me = CheckParamsAndGetThis<JsFont>(env, info);
    return (me != nullptr) ? me->OnSetTypeface(env, info) : nullptr;
}

napi_value JsFont::GetTypeface(napi_env env, napi_callback_info info)
{
    JsFont* me = CheckParamsAndGetThis<JsFont>(env, info);
    return (me != nullptr) ? me->OnGetTypeface(env, info) : nullptr;
}

napi_value JsFont::GetSize(napi_env env, napi_callback_info info)
{
    JsFont* me = CheckParamsAndGetThis<JsFont>(env, info);
    return (me != nullptr) ? me->OnGetSize(env, info) : nullptr;
}

napi_value JsFont::GetMetrics(napi_env env, napi_callback_info info)
{
    JsFont* me = CheckParamsAndGetThis<JsFont>(env, info);
    return (me != nullptr) ? me->OnGetMetrics(env, info) : nullptr;
}

napi_value JsFont::GetWidths(napi_env env, napi_callback_info info)
{
    JsFont* me = CheckParamsAndGetThis<JsFont>(env, info);
    return (me != nullptr) ? me->OnGetWidths(env, info) : nullptr;
}

napi_value JsFont::IsBaselineSnap(napi_env env, napi_callback_info info)
{
    JsFont* me = CheckParamsAndGetThis<JsFont>(env, info);
    return (me != nullptr) ? me->OnIsBaselineSnap(env, info) : nullptr;
}
napi_value JsFont::IsEmbeddedBitmaps(napi_env env, napi_callback_info info)
{
    JsFont* me = CheckParamsAndGetThis<JsFont>(env, info);
    return (me != nullptr) ? me->OnIsEmbeddedBitmaps(env, info) : nullptr;
}
napi_value JsFont::IsForceAutoHinting(napi_env env, napi_callback_info info)
{
    JsFont* me = CheckParamsAndGetThis<JsFont>(env, info);
    return (me != nullptr) ? me->OnIsForceAutoHinting(env, info) : nullptr;
}

napi_value JsFont::MeasureText(napi_env env, napi_callback_info info)
{
    JsFont* me = CheckParamsAndGetThis<JsFont>(env, info);
    return (me != nullptr) ? me->OnMeasureText(env, info) : nullptr;
}

napi_value JsFont::SetScaleX(napi_env env, napi_callback_info info)
{
    JsFont* me = CheckParamsAndGetThis<JsFont>(env, info);
    return (me != nullptr) ? me->OnSetScaleX(env, info) : nullptr;
}

napi_value JsFont::SetSkewX(napi_env env, napi_callback_info info)
{
    JsFont* me = CheckParamsAndGetThis<JsFont>(env, info);
    return (me != nullptr) ? me->OnSetSkewX(env, info) : nullptr;
}

napi_value JsFont::TextToGlyphs(napi_env env, napi_callback_info info)
{
    JsFont* me = CheckParamsAndGetThis<JsFont>(env, info);
    return (me != nullptr) ? me->OnTextToGlyphs(env, info) : nullptr;
}

napi_value JsFont::OnEnableSubpixel(napi_env env, napi_callback_info info)
{
    if (m_font == nullptr) {
        ROSEN_LOGE("JsFont::OnEnableSubpixel font is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_ONE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_ONE);

    bool isSubpixel = false;
    GET_BOOLEAN_PARAM(ARGC_ZERO, isSubpixel);

    m_font->SetSubpixel(isSubpixel);
    return nullptr;
}

napi_value JsFont::OnEnableEmbolden(napi_env env, napi_callback_info info)
{
    if (m_font == nullptr) {
        ROSEN_LOGE("JsFont::OnEnableEmbolden font is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_ONE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_ONE);

    bool isEmbolden = false;
    GET_BOOLEAN_PARAM(ARGC_ZERO, isEmbolden);

    m_font->SetEmbolden(isEmbolden);
    return nullptr;
}

napi_value JsFont::OnEnableLinearMetrics(napi_env env, napi_callback_info info)
{
    if (m_font == nullptr) {
        ROSEN_LOGE("JsFont::OnEnableLinearMetrics font is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_ONE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_ONE);

    bool isLinearMetrics = false;
    GET_BOOLEAN_PARAM(ARGC_ZERO, isLinearMetrics);

    m_font->SetLinearMetrics(isLinearMetrics);
    return nullptr;
}

napi_value JsFont::OnSetBaselineSnap(napi_env env, napi_callback_info info)
{
    if (m_font == nullptr) {
        ROSEN_LOGE("JsFont::OnSetBaselineSnap font is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_ONE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_ONE);

    bool baselineSnap = false;
    GET_BOOLEAN_PARAM(ARGC_ZERO, baselineSnap);

    m_font->SetBaselineSnap(baselineSnap);
    return nullptr;
}

napi_value JsFont::OnIsBaselineSnap(napi_env env, napi_callback_info info)
{
    if (m_font == nullptr) {
        ROSEN_LOGE("JsFont::OnIsBaselineSnap font is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    bool baselineSnap = m_font->IsBaselineSnap();
    return CreateJsValue(env, baselineSnap);
}

napi_value JsFont::OnSetEmbeddedBitmaps(napi_env env, napi_callback_info info)
{
    if (m_font == nullptr) {
        ROSEN_LOGE("JsFont::OnSetEmbeddedBitmaps font is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_ONE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_ONE);

    bool embeddedBitmaps = false;
    GET_BOOLEAN_PARAM(ARGC_ZERO, embeddedBitmaps);

    m_font->SetEmbeddedBitmaps(embeddedBitmaps);
    return nullptr;
}

napi_value JsFont::OnIsEmbeddedBitmaps(napi_env env, napi_callback_info info)
{
    if (m_font == nullptr) {
        ROSEN_LOGE("JsFont::OnIsEmbeddedBitmaps font is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    bool embeddedBitmaps = m_font->IsEmbeddedBitmaps();
    return CreateJsValue(env, embeddedBitmaps);
}

napi_value JsFont::OnSetForceAutoHinting(napi_env env, napi_callback_info info)
{
    if (m_font == nullptr) {
        ROSEN_LOGE("JsFont::OnSetForceAutoHinting font is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_ONE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_ONE);

    bool isForceAutoHinting = false;
    GET_BOOLEAN_PARAM(ARGC_ZERO, isForceAutoHinting);

    m_font->SetForceAutoHinting(isForceAutoHinting);
    return nullptr;
}

napi_value JsFont::OnIsForceAutoHinting(napi_env env, napi_callback_info info)
{
    if (m_font == nullptr) {
        ROSEN_LOGE("JsFont::OnIsForceAutoHinting font is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    bool isForceAutoHinting = m_font->IsForceAutoHinting();
    return CreateJsValue(env, isForceAutoHinting);
}

napi_value JsFont::OnSetSize(napi_env env, napi_callback_info info)
{
    if (m_font == nullptr) {
        ROSEN_LOGE("JsFont::OnSetSize font is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_ONE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_ONE);

    double textSize = 0.0;
    GET_DOUBLE_PARAM(ARGC_ZERO, textSize);

    m_font->SetSize((float)textSize);
    return nullptr;
}

napi_value JsFont::OnGetSize(napi_env env, napi_callback_info info)
{
    if (m_font == nullptr) {
        ROSEN_LOGE("JsFont::OnGetSize font is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    double textSize = m_font->GetSize();
    return GetDoubleAndConvertToJsValue(env, textSize);
}

napi_value JsFont::OnGetMetrics(napi_env env, napi_callback_info info)
{
    if (m_font == nullptr) {
        ROSEN_LOGE("JsFont::OnGetMetrics font is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    FontMetrics metrics;
    m_font->GetMetrics(&metrics);
    return GetFontMetricsAndConvertToJsValue(env, &metrics);
}

napi_value JsFont::OnSetTypeface(napi_env env, napi_callback_info info)
{
    if (m_font == nullptr) {
        ROSEN_LOGE("JsFont::OnSetTypeface font is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_ONE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_ONE);

    JsTypeface *jsTypeface = nullptr;
    GET_UNWRAP_PARAM(ARGC_ZERO, jsTypeface);

    m_font->SetTypeface(jsTypeface->GetTypeface());
    return nullptr;
}

napi_value JsFont::OnGetTypeface(napi_env env, napi_callback_info info)
{
    if (m_font == nullptr) {
        ROSEN_LOGE("JsFont::OnGetTypeface font is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    std::shared_ptr<Typeface> typeface = m_font->GetTypeface();
    return JsTypeface::CreateJsTypeface(env, typeface);
}

napi_value JsFont::OnGetWidths(napi_env env, napi_callback_info info)
{
    if (m_font == nullptr) {
        ROSEN_LOGE("JsFont::OnGetWidths font is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_THREE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_THREE);

    uint32_t verticesSize = 0;
    napi_get_array_length(env, argv[ARGC_ZERO], &verticesSize);
    if (verticesSize == 0) {
        return nullptr;
    }

    auto glyph = new uint16_t[verticesSize];
    for (uint32_t i = 0; i < verticesSize; i++) {
        napi_value tempglyph = nullptr;
        napi_get_element(env, argv[ARGC_ZERO], i, &tempglyph);
        uint32_t glyph_t = 0;
        bool isColorOk = ConvertFromJsValue(env, tempglyph, glyph_t);
        if (!isColorOk) {
            ROSEN_LOGE("JsFont::OnGetWidths Argv[ARGC_ZERO] is invalid");
            delete []glyph;
            return nullptr;
        }
        glyph[i] = glyph_t;
    }

    int count = 0;
    if (!ConvertFromJsValue(env, argv[ARGC_ONE], count)) {
        delete []glyph;
        ROSEN_LOGE("JsFont::OnGetWidths Argv[ARGC_ONE] is invalid");
        return nullptr;
    }

    uint32_t verticesSize_n = 0;
    napi_get_array_length(env, argv[ARGC_TWO], &verticesSize_n);
    if (verticesSize_n == 0) {
        delete []glyph;
        return nullptr;
    }

    auto width = new float[verticesSize_n];
    for (uint32_t i = 0; i < verticesSize_n; i++) {
        napi_value tempwidth = nullptr;
        napi_get_element(env, argv[ARGC_TWO], i, &tempwidth);
        double width_t = 0.f;
        bool isColorOk = ConvertFromJsValue(env, tempwidth, width_t);
        if (!isColorOk) {
            ROSEN_LOGE("JsFont::OnGetWidths Argv[ARGC_TWO] is invalid");
            delete []glyph;
            delete []width;
            return nullptr;
        }
        width[i] = width_t;
    }

    m_font->GetWidths(glyph, count, width);
    for (size_t i = 0; i < verticesSize; i++) {
        napi_value eleValue = CreateJsValue(env, glyph[i]);
        napi_set_element(env, argv[ARGC_ZERO], i, eleValue);
    }
    for (size_t i = 0; i < verticesSize_n; i++) {
        napi_value eleValue = CreateJsValue(env, width[i]);
        napi_set_element(env, argv[ARGC_TWO], i, eleValue);
    }

    delete []glyph;
    delete []width;
    return nullptr;
}

napi_value JsFont::OnMeasureText(napi_env env, napi_callback_info info)
{
    if (m_font == nullptr) {
        ROSEN_LOGE("JsFont::OnMeasureText font is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_TWO] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_TWO);

    std::string text = "";
    if (!ConvertFromJsValue(env, argv[ARGC_ZERO], text)) {
        ROSEN_LOGE("JsFont::OnMeasureText Argv[0] is invalid");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Parameter verification failed. The text input must be string.");
    }

    TextEncoding TextEncoding = TextEncoding::UTF8;
    if (!ConvertFromJsTextEncoding(env, TextEncoding, argv[1])) {
        ROSEN_LOGE("JsFont::OnMeasureText ConvertFromJsTextEncoding failed");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "Parameter verification failed. The TextEncoding input must be valid.");
    }

    double textSize = m_font->MeasureText(text.c_str(), text.length(), TextEncoding);
    return GetDoubleAndConvertToJsValue(env, textSize);
}

napi_value JsFont::OnSetScaleX(napi_env env, napi_callback_info info)
{
    if (m_font == nullptr) {
        ROSEN_LOGE("JsFont::OnSetScaleX font is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_ONE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_ONE);

    double scaleX = 0.0;
    GET_DOUBLE_PARAM(ARGC_ZERO, scaleX);

    JS_CALL_DRAWING_FUNC(m_font->SetScaleX(scaleX));
    return nullptr;
}

napi_value JsFont::OnSetSkewX(napi_env env, napi_callback_info info)
{
    if (m_font == nullptr) {
        ROSEN_LOGE("JsFont::OnSetSkewX font is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_ONE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_ONE);

    double skewX = 0.0;
    GET_DOUBLE_PARAM(ARGC_ZERO, skewX);

    JS_CALL_DRAWING_FUNC(m_font->SetSkewX(skewX));
    return nullptr;
}

napi_value JsFont::OnTextToGlyphs(napi_env env, napi_callback_info info)
{
    if (m_font == nullptr) {
        ROSEN_LOGE("JsFont::OnTextToGlyphs font is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_FOUR] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_FOUR);

    std::string text = "";
    if (!ConvertFromJsValue(env, argv[ARGC_ZERO], text)) {
        ROSEN_LOGE("JsFont::OnTextToGlyphs Argv[ARGC_ZERO] is invalid");
        return nullptr;
    }

    TextEncoding TextEncoding = TextEncoding::UTF8;
    if (!ConvertFromJsTextEncoding(env, TextEncoding, argv[ARGC_ONE])) {
        ROSEN_LOGE("JsFont::OnTextToGlyphs ConvertFromJsTextEncoding failed");
        return nullptr;
    }

    uint32_t verticesSize = 0;
    napi_get_array_length(env, argv[ARGC_TWO], &verticesSize);
    if (verticesSize == 0) {
        return nullptr;
    }

    auto glyph = new uint16_t[verticesSize];
    for (uint32_t i = 0; i < verticesSize; i++) {
        napi_value tempglyph = nullptr;
        napi_get_element(env, argv[ARGC_TWO], i, &tempglyph);
        uint32_t glyph_t = 0;
        bool isColorOk = ConvertFromJsValue(env, tempglyph, glyph_t);
        if (!isColorOk) {
            ROSEN_LOGE("JsFont::OnTextToGlyphs Argv[ARGC_TWO] is invalid");
            delete []glyph;
            return nullptr;
        }
        glyph[i] = glyph_t;
    }

    int maxGlyphCount = 0;
    if (!ConvertFromJsValue(env, argv[ARGC_THREE], maxGlyphCount)) {
        delete []glyph;
        ROSEN_LOGE("JsFont::OnTextToGlyphs Argv[ARGC_THREE] is invalid");
        return nullptr;
    }

    int texttoglyphs = m_font->TextToGlyphs(text.c_str(), text.length(), TextEncoding, glyph, maxGlyphCount);

    for (size_t i = 0; i < verticesSize; i++) {
        napi_value eleValue = CreateJsValue(env, glyph[i]);
        napi_set_element(env, argv[ARGC_TWO], i, eleValue);
    }

    delete []glyph;
    return CreateJsNumber(env, texttoglyphs);
}

std::shared_ptr<Font> JsFont::GetFont()
{
    return m_font;
}

void JsFont::SetFont(std::shared_ptr<Font> font)
{
    m_font = font;
}
} // namespace Drawing
} // namespace OHOS::Rosen