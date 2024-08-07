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

#include "js_run.h"
#include "napi_common.h"
#include "log_wrapper.h"
#include "canvas_napi/js_canvas.h"
#include "font_napi/js_font.h"
#include "recording/recording_canvas.h"

namespace OHOS::Rosen {
thread_local napi_ref JsRun::constructor_ = nullptr;
const std::string CLASS_NAME = "Run";
napi_value JsRun::Constructor(napi_env env, napi_callback_info info)
{
    size_t argCount = 0;
    napi_value jsThis = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argCount, nullptr, &jsThis, nullptr);
    if (status != napi_ok) {
        TEXT_LOGE("JsRun::Constructor failed to napi_get_cb_info");
        return nullptr;
    }

    JsRun* jsRun = new(std::nothrow) JsRun();
    if (!jsRun) {
        TEXT_LOGE("JsRun::Constructor failed to create JsRun");
        return nullptr;
    }

    status = napi_wrap(env, jsThis, jsRun,
        JsRun::Destructor, nullptr, nullptr);
    if (status != napi_ok) {
        delete jsRun;
        TEXT_LOGE("JsRun::Constructor Failed to wrap native instance");
        return nullptr;
    }

    return jsThis;
}

napi_value JsRun::Init(napi_env env, napi_value exportObj)
{
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("getGlyphCount", JsRun::GetGlyphCount),
        DECLARE_NAPI_FUNCTION("getGlyphs", JsRun::GetGlyphs),
        DECLARE_NAPI_FUNCTION("getPositions", JsRun::GetPositions),
        DECLARE_NAPI_FUNCTION("getOffsets", JsRun::GetOffsets),
        DECLARE_NAPI_FUNCTION("getFont", JsRun::GetFont),
        DECLARE_NAPI_FUNCTION("getStringRange", JsRun::GetStringRange),
        DECLARE_NAPI_FUNCTION("getStringIndices", JsRun::GetStringIndices),
        DECLARE_NAPI_FUNCTION("getImageBounds", JsRun::GetImageBounds),
        DECLARE_NAPI_FUNCTION("getTypographicBounds", JsRun::GetTypographicBounds),
        DECLARE_NAPI_FUNCTION("paint", JsRun::Paint),
    };

    napi_value constructor = nullptr;
    napi_status status = napi_define_class(env, CLASS_NAME.c_str(), NAPI_AUTO_LENGTH, Constructor, nullptr,
        sizeof(properties) / sizeof(properties[0]), properties, &constructor);
    if (status != napi_ok) {
        TEXT_LOGE("JsRun::Init Failed to define JsRun class");
        return nullptr;
    }

    status = napi_create_reference(env, constructor, 1, &constructor_);
    if (status != napi_ok) {
        TEXT_LOGE("JsRun::Init Failed to create reference of constructor");
        return nullptr;
    }

    status = napi_set_named_property(env, exportObj, CLASS_NAME.c_str(), constructor);
    if (status != napi_ok) {
        TEXT_LOGE("JsRun::Init Failed to set constructor");
        return nullptr;
    }

    return exportObj;
}

void JsRun::Destructor(napi_env env, void* nativeObject, void* finalize)
{
    (void)finalize;
    if (nativeObject != nullptr) {
        JsRun* jsRun = reinterpret_cast<JsRun*>(nativeObject);
        delete jsRun;
    }
}

napi_value JsRun::CreateRun(napi_env env, napi_callback_info info)
{
    napi_value result = nullptr;
    napi_value constructor = nullptr;
    napi_status status = napi_get_reference_value(env, constructor_, &constructor);
    if (status != napi_ok) {
        TEXT_LOGE("Failed to get the representation of constructor object");
        return nullptr;
    }

    status = napi_new_instance(env, constructor, 0, nullptr, &result);
    if (status != napi_ok) {
        TEXT_LOGE("Failed to instantiate JavaScript font instance");
        return nullptr;
    }
    return result;
}

JsRun::JsRun()
{
}

void JsRun::SetRun(std::unique_ptr<Run> run)
{
    run_ = std::move(run);
}

napi_value JsRun::GetGlyphCount(napi_env env, napi_callback_info info)
{
    JsRun* me = CheckParamsAndGetThis<JsRun>(env, info);
    return (me != nullptr) ? me->OnGetGlyphCount(env, info) : nullptr;
}

napi_value JsRun::OnGetGlyphCount(napi_env env, napi_callback_info info)
{
    if (!run_) {
        TEXT_LOGE("JsRun::OnGetGlyphCount run is nullptr");
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "JsRun::OnGetGlyphCount run is nullptr.");
    }
    int64_t count = static_cast<int64_t>(run_->GetGlyphCount());
    return CreateJsNumber(env, count);
}

napi_value JsRun::GetGlyphs(napi_env env, napi_callback_info info)
{
    JsRun* me = CheckParamsAndGetThis<JsRun>(env, info);
    return (me != nullptr) ? me->OnGetGlyphs(env, info) : nullptr;
}

napi_value JsRun::OnGetGlyphs(napi_env env, napi_callback_info info)
{
    if (!run_) {
        TEXT_LOGE("JsRun::OnGetGlyphs run is nullptr");
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "JsRun::OnGetGlyphs run is nullptr.");
    }

    size_t argc = ARGC_ONE;
    size_t start = 0;
    size_t end = 0;
    napi_value argv[ARGC_ONE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok) {
        TEXT_LOGE("JsRun::OnGetGlyphs Failed to get the info");
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    } else if (argc == ARGC_ONE){
        napi_valuetype valueType = napi_undefined;
        if (argv[0] == nullptr || napi_typeof(env, argv[0], &valueType) != napi_ok || valueType != napi_object) {
            TEXT_LOGE("JsParagraph::OnGetGlyphs Argv[0] is invalid");
            return NapiGetUndefined(env);
        }
        napi_value tempValue = nullptr;
        if (napi_get_named_property(env, argv[0], "start", &tempValue) != napi_ok) {
            TEXT_LOGE("JsParagraph::OnGetGlyphs start is invalid");
            return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Start invalid params.");
        }
        bool isStartOk = ConvertFromJsValue(env, tempValue, start);
        if (napi_get_named_property(env, argv[0], "end", &tempValue) != napi_ok) {
            TEXT_LOGE("JsParagraph::OnGetGlyphs end is invalid");
            return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "End invalid params.");
        }
        bool isEndOk = ConvertFromJsValue(env, tempValue, end);
        if (!isStartOk || !isEndOk ) {
            TEXT_LOGE("JsParagraph::OnGetGlyphs start or end is invalid");
            return NapiGetUndefined(env);
        }
    }
    std::vector<uint16_t> glyphs = run_->GetGlyphs(start, end);
    napi_value napiGlyphs = nullptr;
    NAPI_CALL(env, napi_create_array(env, &napiGlyphs));
    size_t glyphSize = glyphs.size();
    for (size_t index = 0; index < glyphSize; ++index) {
        NAPI_CALL(env, napi_set_element(env, napiGlyphs, index,
            CreateJsNumber(env, static_cast<uint32_t>(glyphs.at(index)))));
    }
    return napiGlyphs;
}

napi_value JsRun::GetPositions(napi_env env, napi_callback_info info)
{
    JsRun* me = CheckParamsAndGetThis<JsRun>(env, info);
    return (me != nullptr) ? me->OnGetPositions(env, info) : nullptr;
}

napi_value JsRun::OnGetPositions(napi_env env, napi_callback_info info)
{
    if (!run_) {
        TEXT_LOGE("JsRun::OnGetPositions run is nullptr");
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "JsRun::OnGetPositions run is nullptr.");
    }

    size_t argc = ARGC_ONE;
    size_t start = 0;
    size_t end = 0;
    napi_value argv[ARGC_ONE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok) {
        TEXT_LOGE("JsRun::OnGetPositions Failed to get the info");
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    } else if (argc == ARGC_ONE){
        napi_valuetype valueType = napi_undefined;
        if (argv[0] == nullptr || napi_typeof(env, argv[0], &valueType) != napi_ok || valueType != napi_object) {
            TEXT_LOGE("JsParagraph::OnGetPositions Argv[0] is invalid");
            return NapiGetUndefined(env);
        }
        napi_value tempValue = nullptr;
        if (napi_get_named_property(env, argv[0], "start", &tempValue) != napi_ok) {
            TEXT_LOGE("JsParagraph::OnGetPositions start is invalid");
            return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Start invalid params.");
        }
        bool isStartOk = ConvertFromJsValue(env, tempValue, start);
        if (napi_get_named_property(env, argv[0], "end", &tempValue) != napi_ok) {
            TEXT_LOGE("JsParagraph::OnGetPositions end is invalid");
            return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "End invalid params.");
        }
        bool isEndOk = ConvertFromJsValue(env, tempValue, end);
        if (!isStartOk || !isEndOk ) {
            TEXT_LOGE("JsParagraph::OnGetPositions start or end is invalid");
            return NapiGetUndefined(env);
        }
    }

    std::vector<Drawing::Point> positions = run_->GetPositions(start, end);
    napi_value napiPositions = nullptr;
    NAPI_CALL(env, napi_create_array(env, &napiPositions));
    size_t positionSize = positions.size();
    for (size_t index = 0; index < positionSize; ++index) {
        NAPI_CALL(env, napi_set_element(env, napiPositions, index,
            GetPointAndConvertToJsValue(env, positions.at(index))));
    }
    return napiPositions;
}

napi_value JsRun::GetOffsets(napi_env env, napi_callback_info info)
{
    JsRun* me = CheckParamsAndGetThis<JsRun>(env, info);
    return (me != nullptr) ? me->OnGetOffsets(env, info) : nullptr;
}

napi_value JsRun::OnGetOffsets(napi_env env, napi_callback_info info)
{
    if (!run_) {
        TEXT_LOGE("JsRun::OnGetOffsets run is nullptr");
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "JsRun::OnGetOffsets run is nullptr.");
    }

    std::vector<Drawing::Point> offsets = run_->GetOffsets();
    napi_value napiOffsets = nullptr;
    NAPI_CALL(env, napi_create_array(env, &napiOffsets));
    size_t offsetSize = offsets.size();
    for (size_t index = 0; index < offsetSize; ++index) {
        NAPI_CALL(env, napi_set_element(env, napiOffsets, index,
            GetPointAndConvertToJsValue(env, offsets.at(index))));
    }
    return napiOffsets;
}

napi_value JsRun::GetFont(napi_env env, napi_callback_info info)
{
    JsRun* me = CheckParamsAndGetThis<JsRun>(env, info);
    return (me != nullptr) ? me->OnGetFont(env, info) : nullptr;
}

napi_value JsRun::OnGetFont(napi_env env, napi_callback_info info)
{
    if (!run_) {
        TEXT_LOGE("JsRun::OnGetFont run is nullptr");
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "JsRun::OnGetFont run is nullptr.");
    }

    std::shared_ptr<Drawing::Font> fontPtr = std::make_shared<Drawing::Font>(run_->GetFont());
    if (!fontPtr) {
        TEXT_LOGE("JsRun::OnGetFont fontPtr is nullptr");
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "JsRun::OnGetFont fontPtr is nullptr.");
    }

    napi_value resultValue  = Drawing::JsFont::CreateFont(env, info);
    if (!resultValue) {
        TEXT_LOGE("JsRun::OnGetFont JsFont::CreateFont resultValue is null");
        return nullptr;
    }
    Drawing::JsFont* jsFont = nullptr;
    napi_unwrap(env, resultValue, reinterpret_cast<void**>(&jsFont));
    if (!jsFont) {
        TEXT_LOGE("JsRun::OnGetFont napi_unwrap jsFont is null");
        return nullptr;
    }
    jsFont->SetFont(fontPtr);
    return resultValue;
}

napi_value JsRun::Paint(napi_env env, napi_callback_info info)
{
    JsRun* me = CheckParamsAndGetThis<JsRun>(env, info);
    return (me != nullptr) ? me->OnPaint(env, info) : nullptr;
}

napi_value JsRun::OnPaint(napi_env env, napi_callback_info info)
{
    if (run_ == nullptr) {
        TEXT_LOGE("JsRun::OnPaint run is nullptr");
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "JsRun::OnPaint run is nullptr.");
    }
    size_t argc = ARGC_THREE;
    napi_value argv[ARGC_THREE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_THREE) {
        TEXT_LOGE("JsRun::OnPaint Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    Drawing::JsCanvas* jsCanvas = nullptr;
    double x = 0.0;
    double y = 0.0;
    napi_unwrap(env, argv[0], reinterpret_cast<void **>(&jsCanvas));
    if (!jsCanvas || !jsCanvas->GetCanvas() ||
        !(ConvertFromJsValue(env, argv[ARGC_ONE], x) && ConvertFromJsValue(env, argv[ARGC_TWO], y))) {
        TEXT_LOGE("JsRun::OnPaint Argv is invalid");
        return NapiGetUndefined(env);
    }
    run_->Paint(jsCanvas->GetCanvas(), x, y);

    return NapiGetUndefined(env);
}

void JsRun::SetParagraph(std::shared_ptr<Typography> paragraph)
{
    paragraph_ = paragraph;
}

napi_value JsRun::GetStringRange(napi_env env, napi_callback_info info)
{
    JsRun* me = CheckParamsAndGetThis<JsRun>(env, info);
    return (me != nullptr) ? me->OnGetStringRange(env, info) : nullptr;
}

napi_value JsRun::OnGetStringRange(napi_env env, napi_callback_info info)
{
    if (!run_) {
        TEXT_LOGE("JsRun::OnGetStringRange run is nullptr");
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "JsRun::OnGetStringRange run is nullptr.");
    }
    uint32_t location = 0;
    uint32_t length = 0;
    run_->GetStringRange(&location, &length);
    napi_value objValue = nullptr;
    napi_create_object(env, &objValue);
    if (objValue != nullptr) {
        napi_set_named_property(env, objValue, "start", CreateJsNumber(env, location));
        napi_set_named_property(env, objValue, "end", CreateJsNumber(env, length));
    }
    return objValue;
}

napi_value JsRun::GetStringIndices(napi_env env, napi_callback_info info)
{
    JsRun* me = CheckParamsAndGetThis<JsRun>(env, info);
    return (me != nullptr) ? me->OnGetStringIndices(env, info) : nullptr;
}

napi_value JsRun::OnGetStringIndices(napi_env env, napi_callback_info info)
{
    if (!run_) {
        TEXT_LOGE("JsRun::OnGetStringIndices run is nullptr");
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "JsRun::OnGetStringIndices run is nullptr.");
    }

    size_t argc = ARGC_ONE;
    size_t start = 0;
    size_t end = 0;
    napi_value argv[ARGC_ONE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok) {
        TEXT_LOGE("JsRun::OnGetStringIndices Failed to get the info");
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    } else if (argc == ARGC_ONE){
        napi_valuetype valueType = napi_undefined;
        if (argv[0] == nullptr || napi_typeof(env, argv[0], &valueType) != napi_ok || valueType != napi_object) {
            TEXT_LOGE("JsParagraph::OnGetStringIndices Argv[0] is invalid");
            return NapiGetUndefined(env);
        }
        napi_value tempValue = nullptr;
        if (napi_get_named_property(env, argv[0], "start", &tempValue) != napi_ok) {
            TEXT_LOGE("JsParagraph::OnGetStringIndices start is invalid");
            return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "Start invalid params.");
        }
        bool isStartOk = ConvertFromJsValue(env, tempValue, start);
        if (napi_get_named_property(env, argv[0], "end", &tempValue) != napi_ok) {
            TEXT_LOGE("JsParagraph::OnGetStringIndices start is invalid");
            return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "End invalid params.");
        }
        bool isEndOk = ConvertFromJsValue(env, tempValue, end);
        if (!isStartOk || !isEndOk ) {
            TEXT_LOGE("JsParagraph::OnGetStringIndices start or end is invalid");
            return NapiGetUndefined(env);
        }
    }

    std::vector<uint32_t> stringIndices = run_->GetStringIndices(start, end);
    napi_value napiStringIndices = nullptr;
    NAPI_CALL(env, napi_create_array(env, &napiStringIndices));
    size_t stringIndicesSize = stringIndices.size();
    for (size_t index = 0; index < stringIndicesSize; ++index) {
        NAPI_CALL(env, napi_set_element(env, napiStringIndices, index,
            CreateJsNumber(env, static_cast<uint32_t>(stringIndices.at(index)))));
    }
    return napiStringIndices;
}

napi_value JsRun::GetImageBounds(napi_env env, napi_callback_info info)
{
    JsRun* me = CheckParamsAndGetThis<JsRun>(env, info);
    return (me != nullptr) ? me->OnGetImageBounds(env, info) : nullptr;
}

napi_value JsRun::OnGetImageBounds(napi_env env, napi_callback_info info)
{
    if (!run_) {
        TEXT_LOGE("JsRun::OnGetImageBounds run is nullptr");
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "JsRun::OnGetImageBounds run is nullptr.");
    }
    Drawing::Rect imageBounds = run_->GetImageBounds();
    return GetRectAndConvertToJsValue(env, imageBounds);
}

napi_value JsRun::GetTypographicBounds(napi_env env, napi_callback_info info)
{
    JsRun* me = CheckParamsAndGetThis<JsRun>(env, info);
    return (me != nullptr) ? me->OnGetTypographicBounds(env, info) : nullptr;
}

napi_value JsRun::OnGetTypographicBounds(napi_env env, napi_callback_info info)
{
    if (!run_) {
        TEXT_LOGE("JsRun::OnGetTypographicBounds run is nullptr");
        return NapiThrowError(env, TextErrorCode::ERROR_INVALID_PARAM, "JsRun::OnGetTypographicBounds run is nullptr.");
    }

    float ascent = 0.0;
    float descent = 0.0;
    float leading = 0.0;
    float width = run_->GetTypographicBounds(&ascent, &descent, &leading);
    return GetTypographicBoundsAndConvertToJsValue(env, ascent, descent, leading, width);
}
} // namespace OHOS::Rosen
