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
#include "js_paragraph.h"
#include "paragraph_builder_napi/js_paragraph_builder.h"
#include "../js_text_utils.h"
#include "draw/canvas.h"
#include "canvas_napi/js_canvas.h"
#include "text_line_napi/js_text_line.h"
#include "../drawing/js_drawing_utils.h"

namespace OHOS::Rosen {
std::unique_ptr<Typography> g_Typography = nullptr;
thread_local napi_ref JsParagraph::constructor_ = nullptr;
const std::string CLASS_NAME = "JsParagraph";

napi_value JsParagraph::Constructor(napi_env env, napi_callback_info info)
{
    ROSEN_LOGE(" clp-ark JsParagraph::Constructor 000 ");

    size_t argCount = 0;
    napi_value jsThis = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argCount, nullptr, &jsThis, nullptr);
    if (status != napi_ok) {
        ROSEN_LOGE(" clp-ark failed to napi_get_cb_info");
        return nullptr;
    }

    if (!g_Typography) {
        ROSEN_LOGE(" clp-ark JsParagraph::Constructor g_Typography is nullptr");
        return nullptr;
    }

    ROSEN_LOGE(" clp-ark JsParagraph::Constructor g_Typography.get() = %p ", g_Typography.get());
    JsParagraph *jsParagraph = new(std::nothrow) JsParagraph(std::move(g_Typography));

    status = napi_wrap(env, jsThis, jsParagraph,
        JsParagraph::Destructor, nullptr, nullptr);
    if (status != napi_ok) {
        delete jsParagraph;
        ROSEN_LOGE(" clp-ark Failed to wrap native instance");
        return nullptr;
    }
    ROSEN_LOGE(" clp-ark JsParagraph::Constructor 900 ");
    return jsThis;
}

napi_value JsParagraph::Init(napi_env env, napi_value exportObj)
{
    ROSEN_LOGE(" clp-ark JsParagraph::Init 000 ");

    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("layout", JsParagraph::Layout),
        DECLARE_NAPI_FUNCTION("paint", JsParagraph::Paint),
        DECLARE_NAPI_FUNCTION("getMaxWidth", JsParagraph::GetMaxWidth),
        DECLARE_NAPI_FUNCTION("getHeight", JsParagraph::GetHeight),
        DECLARE_NAPI_FUNCTION("getLongestLine", JsParagraph::GetLongestLine),
        DECLARE_NAPI_FUNCTION("getMinIntrinsicWidth", JsParagraph::GetMinIntrinsicWidth),
        DECLARE_NAPI_FUNCTION("getMaxIntrinsicWidth", JsParagraph::GetMaxIntrinsicWidth),
        DECLARE_NAPI_FUNCTION("getAlphabeticBaseline", JsParagraph::GetAlphabeticBaseline),
        DECLARE_NAPI_FUNCTION("getIdeographicBaseline", JsParagraph::GetIdeographicBaseline),
        DECLARE_NAPI_FUNCTION("getRectsForRange", JsParagraph::GetRectsForRange),
        DECLARE_NAPI_FUNCTION("etRectsForPlaceholders", JsParagraph::GetRectsForPlaceholders),
        DECLARE_NAPI_FUNCTION("getGlyphPositionAtCoordinate", JsParagraph::GetGlyphPositionAtCoordinate),
        DECLARE_NAPI_FUNCTION("getWordBoundary", JsParagraph::GetWordBoundary),
        DECLARE_NAPI_FUNCTION("getLineCount", JsParagraph::GetLineCount),
        DECLARE_NAPI_FUNCTION("getLineHeight", JsParagraph::GetLineHeight),
        DECLARE_NAPI_FUNCTION("getLineWidth", JsParagraph::GetLineWidth),
        DECLARE_NAPI_FUNCTION("didExceedMaxLines", JsParagraph::DidExceedMaxLines),
    };

    ROSEN_LOGE(" clp-ark JsParagraph::Init 100 ");

    napi_value constructor = nullptr;
    napi_status status = napi_define_class(env, CLASS_NAME.c_str(), NAPI_AUTO_LENGTH, Constructor, nullptr,
        sizeof(properties) / sizeof(properties[0]), properties, &constructor);
    if (status != napi_ok) {
        ROSEN_LOGE(" clp-ark Failed to define Paragraph class");
        return nullptr;
    }

    status = napi_create_reference(env, constructor, 1, &constructor_);
    if (status != napi_ok) {
        ROSEN_LOGE(" clp-ark Failed to create reference of result");
        return nullptr;
    }

    status = napi_set_named_property(env, exportObj, CLASS_NAME.c_str(), constructor);
    if (status != napi_ok) {
        ROSEN_LOGE(" clp-ark Failed to set result");
        return nullptr;
    }

    return exportObj;
}

void JsParagraph::Destructor(napi_env env, void *nativeObject, void *finalize)
{
    (void)finalize;
    if (nativeObject != nullptr) {
        JsParagraph *napi = reinterpret_cast<JsParagraph *>(nativeObject);
        delete napi;
    }
}

napi_value JsParagraph::Layout(napi_env env, napi_callback_info info)
{
    ROSEN_LOGE(" clp-ark JsParagraph::Layout 000 ");
    JsParagraph* me = CheckParamsAndGetThis<JsParagraph>(env, info);
    return (me != nullptr) ? me->OnLayout(env, info) : nullptr;
}

napi_value JsParagraph::OnLayout(napi_env env, napi_callback_info info)
{
    ROSEN_LOGE(" clp-ark JsParagraph::OnLayout 000 paragraphCurrent_ = %p, paragraphOld_ = %p ", paragraphCurrent_.get(),  paragraphOld_.get());
    if (!paragraphOld_ || !paragraphCurrent_) {
        ROSEN_LOGE(" clp-ark JsParagraph::OnLayout paragraphOld pr paragraphCurrent is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    size_t argc = ARGC_ONE;
    napi_value argv[ARGC_ONE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_ONE) {
        ROSEN_LOGE(" clp-ark JsParagraph::OnLayout Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    double width = 0.0;
    if (!(ConvertFromJsValue(env, argv[0], width))) {
        ROSEN_LOGE(" clp-ark JsParagraph::OnLayout Argv is invalid");
        return NapiGetUndefined(env);
    }

    ROSEN_LOGE(" clp-ark JsParagraph::OnLayout 800 width = %f ", width);
    //paragraphOld_ = paragraphOld_->CloneSelf();
    //paragraphCurrent_ = paragraphOld_->CloneSelf();
    paragraphCurrent_->Layout(width);
    
    // paragraphOld_->Layout(width);

    ROSEN_LOGE(" clp-ark JsParagraph::OnLayout 900 width = %f ", width);
    return NapiGetUndefined(env);
}

napi_value JsParagraph::Paint(napi_env env, napi_callback_info info)
{
    JsParagraph* me = CheckParamsAndGetThis<JsParagraph>(env, info);
    return (me != nullptr) ? me->OnPaint(env, info) : nullptr;
}

napi_value JsParagraph::OnPaint(napi_env env, napi_callback_info info)
{
    ROSEN_LOGE(" clp-ark JsParagraph::OnPaint 000 paragraphCurrent_ = %p, paragraphOld_ = %p ", paragraphCurrent_.get(),  paragraphOld_.get());


    if (paragraphCurrent_ == nullptr) {
        ROSEN_LOGE(" clp-ark JsParagraph::OnPaint paragraphCurrent_ is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    size_t argc = ARGC_THREE;
    napi_value argv[ARGC_THREE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_THREE) {
        ROSEN_LOGE(" clp-ark JsParagraph::OnPaint Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    Drawing::JsCanvas* jsCanvas = nullptr;
    double x = 0.0;
    double y = 0.0;
    napi_unwrap(env, argv[0], reinterpret_cast<void **>(&jsCanvas));
    if (jsCanvas == nullptr ||
        !(ConvertFromJsValue(env, argv[ARGC_ONE], x) && ConvertFromJsValue(env, argv[ARGC_TWO], y))) {
        ROSEN_LOGE(" clp-ark JsParagraph::OnPaint Argv is invalid");
        return NapiGetUndefined(env);
    }
    ROSEN_LOGE(" clp-ark JsParagraph::OnPaint 800 jsCanvas = %p  x = %f  y = %f", jsCanvas, x, y);


    paragraphCurrent_->Paint(jsCanvas->GetCanvas(), x, y);
    return NapiGetUndefined(env);
}

napi_value JsParagraph::GetMaxWidth(napi_env env, napi_callback_info info)
{
    JsParagraph* me = CheckParamsAndGetThis<JsParagraph>(env, info);
    return (me != nullptr) ? me->OnGetMaxWidth(env, info) : nullptr;
}

napi_value JsParagraph::OnGetMaxWidth(napi_env env, napi_callback_info info)
{
    if (paragraphCurrent_ == nullptr) {
        ROSEN_LOGE(" clp-ark JsParagraph::OnGetMaxWidth paragraphCurrent_ is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    double maxWidth = paragraphCurrent_->GetMaxWidth();
    return CreateJsNumber(env, maxWidth);
}

napi_value JsParagraph::GetHeight(napi_env env, napi_callback_info info)
{
    JsParagraph* me = CheckParamsAndGetThis<JsParagraph>(env, info);
    return (me != nullptr) ? me->OnGetHeight(env, info) : nullptr;
}

napi_value JsParagraph::OnGetHeight(napi_env env, napi_callback_info info)
{
    if (paragraphCurrent_ == nullptr) {
        ROSEN_LOGE(" clp-ark JsParagraph::OnGetHeight paragraphCurrent_ is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    double height = paragraphCurrent_->GetHeight();
    return CreateJsNumber(env, height);
}

napi_value JsParagraph::GetLongestLine(napi_env env, napi_callback_info info)
{
    JsParagraph* me = CheckParamsAndGetThis<JsParagraph>(env, info);
    return (me != nullptr) ? me->OnGetLongestLine(env, info) : nullptr;
}

napi_value JsParagraph::OnGetLongestLine(napi_env env, napi_callback_info info)
{
    if (paragraphCurrent_ == nullptr) {
        ROSEN_LOGE(" clp-ark JsParagraph::OnGetLongestLine paragraphCurrent_ is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    double longestLine = paragraphCurrent_->GetActualWidth();
    return CreateJsNumber(env, longestLine);
}

napi_value JsParagraph::GetMinIntrinsicWidth(napi_env env, napi_callback_info info)
{
    JsParagraph* me = CheckParamsAndGetThis<JsParagraph>(env, info);
    return (me != nullptr) ? me->OnGetMinIntrinsicWidth(env, info) : nullptr;
}

napi_value JsParagraph::OnGetMinIntrinsicWidth(napi_env env, napi_callback_info info)
{
    if (paragraphCurrent_ == nullptr) {
        ROSEN_LOGE(" clp-ark JsParagraph::OnGetMinIntrinsicWidth paragraphCurrent_ is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    double minIntrinsicWidth = paragraphCurrent_->GetMinIntrinsicWidth();
    return CreateJsNumber(env, minIntrinsicWidth);
}

napi_value JsParagraph::GetMaxIntrinsicWidth(napi_env env, napi_callback_info info)
{
    JsParagraph* me = CheckParamsAndGetThis<JsParagraph>(env, info);
    return (me != nullptr) ? me->OnGetMaxIntrinsicWidth(env, info) : nullptr;
}

napi_value JsParagraph::OnGetMaxIntrinsicWidth(napi_env env, napi_callback_info info)
{
    if (paragraphCurrent_ == nullptr) {
        ROSEN_LOGE(" clp-ark JsParagraph::OnGetMaxIntrinsicWidth paragraphCurrent_ is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    double maxIntrinsicWidth = paragraphCurrent_->GetMaxIntrinsicWidth();
    return CreateJsNumber(env, maxIntrinsicWidth);
}

napi_value JsParagraph::GetAlphabeticBaseline(napi_env env, napi_callback_info info)
{
    JsParagraph* me = CheckParamsAndGetThis<JsParagraph>(env, info);
    return (me != nullptr) ? me->OnGetAlphabeticBaseline(env, info) : nullptr;
}

napi_value JsParagraph::OnGetAlphabeticBaseline(napi_env env, napi_callback_info info)
{
    if (paragraphCurrent_ == nullptr) {
        ROSEN_LOGE(" clp-ark JsParagraph::OnGetAlphabeticBaseline paragraphCurrent_ is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    double alphabeticBaseline = paragraphCurrent_->GetAlphabeticBaseline();
    return CreateJsNumber(env, alphabeticBaseline);
}

napi_value JsParagraph::GetIdeographicBaseline(napi_env env, napi_callback_info info)
{
    JsParagraph* me = CheckParamsAndGetThis<JsParagraph>(env, info);
    return (me != nullptr) ? me->OnGetIdeographicBaseline(env, info) : nullptr;
}

napi_value JsParagraph::OnGetIdeographicBaseline(napi_env env, napi_callback_info info)
{
    if (paragraphCurrent_ == nullptr) {
        ROSEN_LOGE(" clp-ark JsParagraph::OnGetIdeographicBaseline paragraphCurrent_ is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    double ideographicBaseline = paragraphCurrent_->GetIdeographicBaseline();
    return CreateJsNumber(env, ideographicBaseline);
}

napi_value JsParagraph::GetRectsForRange(napi_env env, napi_callback_info info)
{
    JsParagraph* me = CheckParamsAndGetThis<JsParagraph>(env, info);
    return (me != nullptr) ? me->OnGetRectsForRange(env, info) : nullptr;
}

napi_value JsParagraph::OnGetRectsForRange(napi_env env, napi_callback_info info)
{
    if (paragraphCurrent_ == nullptr) {
        ROSEN_LOGE(" clp-ark JsParagraph::OnGetRectsForRange paragraphCurrent_ is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    size_t argc = ARGC_THREE;
    napi_value argv[ARGC_THREE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_THREE) {
        ROSEN_LOGE(" clp-ark JsParagraph::OnGetRectsForRange Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    napi_valuetype valueType = napi_undefined;
    if (argv[0] == nullptr || napi_typeof(env, argv[0], &valueType) != napi_ok || valueType != napi_object) {
        ROSEN_LOGE(" clp-ark JsParagraph::OnGetRectsForRange Argv[0] is invalid");
        return NapiGetUndefined(env);
    }
    napi_value tempValue = nullptr;
    size_t start = 0;
    size_t end = 0;
    TextRectWidthStyle wstyle;
    TextRectHeightStyle hstyle;
    napi_get_named_property(env, argv[0], "start", &tempValue);
    bool isStartOk = ConvertFromJsValue(env, tempValue, start);
    napi_get_named_property(env, argv[0], "end", &tempValue);
    bool isEndOk = ConvertFromJsValue(env, tempValue, end);
    if (!(isStartOk && isEndOk && ConvertFromJsValue(env, argv[ARGC_ONE], wstyle) &&
        ConvertFromJsValue(env, argv[ARGC_TWO], hstyle))) {
        ROSEN_LOGE(" clp-ark JsParagraph::OnGetRectsForRange Argv is invalid");
        return NapiGetUndefined(env);
    }
    std::vector<TextRect> rectsForRange = paragraphCurrent_->GetTextRectsByBoundary(start, end, hstyle, wstyle);
    napi_value returnrectsForRange = nullptr;
    NAPI_CALL(env, napi_create_array(env, &returnrectsForRange));
    int num = static_cast<int>(rectsForRange.size());
    for (int index = 0; index < num; ++index) {
        napi_value tempValue2 = CreateTextRectJsValue(env, rectsForRange[index]);
        NAPI_CALL(env, napi_set_element(env, returnrectsForRange, index, tempValue2));
    }
    return returnrectsForRange;
}

napi_value JsParagraph::GetRectsForPlaceholders(napi_env env, napi_callback_info info)
{
    JsParagraph* me = CheckParamsAndGetThis<JsParagraph>(env, info);
    return (me != nullptr) ? me->OnGetRectsForPlaceholders(env, info) : nullptr;
}

napi_value JsParagraph::OnGetRectsForPlaceholders(napi_env env, napi_callback_info info)
{
    if (paragraphCurrent_ == nullptr) {
        ROSEN_LOGE(" clp-ark JsParagraph::OnGetRectsForPlaceholders paragraphCurrent_ is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    std::vector<TextRect> rectsForPlaceholders = paragraphCurrent_->GetTextRectsOfPlaceholders();
    napi_value returnPlaceholders = nullptr;
    NAPI_CALL(env, napi_create_array(env, &returnPlaceholders));
    int num = static_cast<int>(rectsForPlaceholders.size());
    for (int index = 0; index < num; ++index) {
        napi_value tempValue = CreateTextRectJsValue(env, rectsForPlaceholders[index]);
        NAPI_CALL(env, napi_set_element(env, returnPlaceholders, index, tempValue));
    }
    return returnPlaceholders;
}

napi_value JsParagraph::GetGlyphPositionAtCoordinate(napi_env env, napi_callback_info info)
{
    JsParagraph* me = CheckParamsAndGetThis<JsParagraph>(env, info);
    return (me != nullptr) ? me->OnGetGlyphPositionAtCoordinate(env, info) : nullptr;
}

napi_value JsParagraph::OnGetGlyphPositionAtCoordinate(napi_env env, napi_callback_info info)
{
    if (paragraphCurrent_ == nullptr) {
        ROSEN_LOGE(" clp-ark JsParagraph::OnGetGlyphPositionAtCoordinate paragraphCurrent_ is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    size_t argc = ARGC_TWO;
    napi_value argv[ARGC_TWO] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_TWO) {
        ROSEN_LOGE(" clp-ark JsParagraph::OnGetGlyphPositionAtCoordinate Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    double dx = 0.0;
    double dy = 0.0;
    if (!(ConvertFromJsValue(env, argv[0], dx) && ConvertFromJsValue(env, argv[1], dy))) {
        ROSEN_LOGE(" clp-ark JsParagraph::OnGetGlyphPositionAtCoordinate Argv is invalid");
        return NapiGetUndefined(env);
    }
    IndexAndAffinity positionWithAffinity = paragraphCurrent_->GetGlyphIndexByCoordinate(dx, dy);
    return GetPositionWithAffinityAndConvertToJsValue(env, &positionWithAffinity);
}

napi_value JsParagraph::GetWordBoundary(napi_env env, napi_callback_info info)
{
    JsParagraph* me = CheckParamsAndGetThis<JsParagraph>(env, info);
    return (me != nullptr) ? me->OnGetWordBoundary(env, info) : nullptr;
}

napi_value JsParagraph::OnGetWordBoundary(napi_env env, napi_callback_info info)
{
    if (paragraphCurrent_ == nullptr) {
        ROSEN_LOGE(" clp-ark JsParagraph::OnGetWordBoundary paragraphCurrent_ is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    size_t argc = ARGC_ONE;
    napi_value argv[ARGC_ONE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_ONE) {
        ROSEN_LOGE(" clp-ark JsParagraph::OnGetWordBoundary Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    size_t offset = 0;
    if (!(ConvertFromJsValue(env, argv[0], offset))) {
        ROSEN_LOGE(" clp-ark JsParagraph::OnGetWordBoundary Argv is invalid");
        return NapiGetUndefined(env);
    }
    Boundary range = paragraphCurrent_->GetWordBoundaryByIndex(offset);
    return GetRangeAndConvertToJsValue(env, &range);
}

napi_value JsParagraph::GetLineCount(napi_env env, napi_callback_info info)
{
    JsParagraph* me = CheckParamsAndGetThis<JsParagraph>(env, info);
    return (me != nullptr) ? me->OnGetLineCount(env, info) : nullptr;
}

napi_value JsParagraph::OnGetLineCount(napi_env env, napi_callback_info info)
{
    if (paragraphCurrent_ == nullptr) {
        ROSEN_LOGE(" clp-ark JsParagraph::OnGetLineCount paragraphCurrent_ is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    size_t lineCount = paragraphCurrent_->GetLineCount();
    return CreateJsNumber(env, lineCount);
}

napi_value JsParagraph::GetLineHeight(napi_env env, napi_callback_info info)
{
    JsParagraph* me = CheckParamsAndGetThis<JsParagraph>(env, info);
    return (me != nullptr) ? me->OnGetLineHeight(env, info) : nullptr;
}

napi_value JsParagraph::OnGetLineHeight(napi_env env, napi_callback_info info)
{
    if (paragraphCurrent_ == nullptr) {
        ROSEN_LOGE(" clp-ark JsParagraph::OnGetLineHeight paragraphCurrent_ is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    size_t argc = ARGC_ONE;
    napi_value argv[ARGC_ONE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_ONE) {
        ROSEN_LOGE(" clp-ark JsParagraph::OnGetLineHeight Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    int lineNumber = 0;
    if (!(ConvertFromJsValue(env, argv[0], lineNumber))) {
        ROSEN_LOGE(" clp-ark JsParagraph::OnGetLineHeight Argv is invalid");
        return NapiGetUndefined(env);
    }
    double lineHeight = paragraphCurrent_->GetLineHeight(lineNumber);
    return CreateJsNumber(env, lineHeight);
}

napi_value JsParagraph::GetLineWidth(napi_env env, napi_callback_info info)
{
    JsParagraph* me = CheckParamsAndGetThis<JsParagraph>(env, info);
    return (me != nullptr) ? me->OnGetLineWidth(env, info) : nullptr;
}

napi_value JsParagraph::OnGetLineWidth(napi_env env, napi_callback_info info)
{
    if (paragraphCurrent_ == nullptr) {
        ROSEN_LOGE(" clp-ark JsParagraph::OnGetLineWidth paragraphCurrent_ is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    size_t argc = ARGC_ONE;
    napi_value argv[ARGC_ONE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_ONE) {
        ROSEN_LOGE(" clp-ark JsParagraph::OnGetLineWidth Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    int lineNumber = 0;
    if (!(ConvertFromJsValue(env, argv[0], lineNumber))) {
        ROSEN_LOGE(" clp-ark JsParagraph::OnGetLineWidth Argv is invalid");
        return NapiGetUndefined(env);
    }
    double lineWidth = paragraphCurrent_->GetLineWidth(lineNumber);
    return CreateJsNumber(env, lineWidth);
}

napi_value JsParagraph::DidExceedMaxLines(napi_env env, napi_callback_info info)
{
    JsParagraph* me = CheckParamsAndGetThis<JsParagraph>(env, info);
    return (me != nullptr) ? me->OnDidExceedMaxLines(env, info) : nullptr;
}

napi_value JsParagraph::OnDidExceedMaxLines(napi_env env, napi_callback_info info)
{
    if (paragraphCurrent_ == nullptr) {
        ROSEN_LOGE(" clp-ark JsParagraph::OnDidExceedMaxLines paragraphCurrent_ is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    bool didExceedMaxLines = paragraphCurrent_->DidExceedMaxLines();
    return CreateJsNumber(env, didExceedMaxLines);
}

JsParagraph::JsParagraph(std::shared_ptr<Typography> typography)
    : paragraphCurrent_(typography), paragraphOld_(typography)
{
    ROSEN_LOGE(" clp-ark JsParagraph::JsParagraph paragraphCurrent_ is typography.get() = %p  paragraphCurrent_ = %p paragraphOld_ = %p ",
        typography.get(), paragraphCurrent_.get(), paragraphOld_.get());
}

JsParagraph::~JsParagraph()
{
    //paragraphOld_ = nullptr;
}

std::shared_ptr<Typography> JsParagraph::GetParagraph()
{
    std::shared_ptr<Typography> typography = std::move(paragraphCurrent_);
    return typography;
}

napi_value JsParagraph::CreateJsTypography(napi_env env, std::unique_ptr<Typography> typography)
{
    ROSEN_LOGE(" clp-ark JsParagraph::OnCreateJsTypography OnCreateJsTypography 000 typography.get() = %p constructor_ = %p ",
        typography.get(), constructor_);


    napi_value constructor = nullptr;
    napi_value result = nullptr;
    napi_status status = napi_get_reference_value(env, constructor_, &constructor);
    if (status == napi_ok) {
        g_Typography = std::move(typography);
        status = napi_new_instance(env, constructor, 0, nullptr, &result);
        if (status == napi_ok) {
            ROSEN_LOGE(" clp-ark JsParagraph::OnCreateJsTypography OnCreateJsTypography 800   ");
            return result;
        } else {
            ROSEN_LOGE(" clp-ark CreateJsTypography: New instance could not be obtained");
        }
    }

    ROSEN_LOGE(" clp-ark JsParagraph::OnCreateJsTypography OnCreateJsTypography 900   ");
    return result;
}


napi_value JsParagraph::GetTextLines(napi_env env, napi_callback_info info)
{
    JsParagraph* me = CheckParamsAndGetThis<JsParagraph>(env, info);
    return (me != nullptr) ? me->OnGetTextLines(env, info) : nullptr;
}

napi_value JsParagraph::OnGetTextLines(napi_env env, napi_callback_info info)
{
    if (paragraphCurrent_ == nullptr) {
        ROSEN_LOGE(" clp-ark JsParagraph::OnGetTextLines paragraph_ is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    
    std::vector<std::unique_ptr<TextLineBase>> textlineArr = paragraphCurrent_->GetTextLines();
    if (textlineArr.empty()) {
        ROSEN_LOGE(" clp-ark JsParagraph::OnGetTextLines textlineArr is empty");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    napi_value array = nullptr;
    napi_create_array_with_length(env, textlineArr.size(), &array);
    uint32_t index = 0;
    for (std::unique_ptr<TextLineBase>& item : textlineArr) {
        napi_value itemObject = JsTextLine::CreateJsTextLine(env, std::move(item), paragraphCurrent_);
        if (!itemObject) {
            ROSEN_LOGE(" clp-ark JsParagraph::OnGetTextLines itemObject is null!");
            continue;
        }
        napi_set_element(env, array, index++, itemObject);
    }
    return array;
}
} // namespace OHOS::Rosen