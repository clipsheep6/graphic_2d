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
#include "canvas_napi/js_canvas.h"
#include "font_napi/js_font.h"
#include "js_text_utils.h"
#include "utils/log.h"
#include "../drawing/js_drawing_utils.h"

namespace OHOS::Rosen {
std::shared_ptr<Run> g_Run = nullptr;
std::shared_ptr<Typography> g_RunTypography = nullptr;
thread_local napi_ref JsRun::constructor_ = nullptr;
const std::string CLASS_NAME = "JsRun";
napi_value JsRun::Constructor(napi_env env, napi_callback_info info)
{
    size_t argCount = 0;
    napi_value jsThis = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argCount, nullptr, &jsThis, nullptr);
    if (status != napi_ok) {
        LOGE("JsRun::Constructor failed to napi_get_cb_info");
        return nullptr;
    }

    if (!g_Run || !g_RunTypography) {
        ROSEN_LOGE(" clp-ark JsRun::Constructor g_Run or g_RunTypography is nullptr");
        return nullptr;
    }

    JsRun* jsRun = new(std::nothrow) JsRun(g_Run, g_RunTypography);
    if (!jsRun) {
        LOGE("JsRun::Constructor failed to create JsRun");
        return nullptr;
    }
    status = napi_wrap(env, jsThis, jsRun,
        JsRun::Destructor, nullptr, nullptr);
    if (status != napi_ok) {
        delete jsRun;
        LOGE("JsRun::Constructor Failed to wrap native instance");
        return nullptr;
    }
    return jsThis;
}

napi_value JsRun::Init(napi_env env, napi_value exportObj)
{
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_STATIC_FUNCTION("getGlyphCount", JsRun::GetGlyphCount),
        DECLARE_NAPI_STATIC_FUNCTION("getGlyphs", JsRun::GetGlyphs),
        DECLARE_NAPI_STATIC_FUNCTION("getPositions", JsRun::GetPositions),
        DECLARE_NAPI_STATIC_FUNCTION("getOffsets", JsRun::GetOffsets),
        DECLARE_NAPI_STATIC_FUNCTION("getFont", JsRun::GetFont),
    };

    napi_value constructor = nullptr;
    napi_status status = napi_define_class(env, CLASS_NAME.c_str(), NAPI_AUTO_LENGTH, Constructor, nullptr,
        sizeof(properties) / sizeof(properties[0]), properties, &constructor);
    if (status != napi_ok) {
        LOGE("JsRun::Init Failed to define JsRun class");
        return nullptr;
    }

    status = napi_create_reference(env, constructor, 1, &constructor_);
    if (status != napi_ok) {
        LOGE("JsRun::Init Failed to create reference of constructor");
        return nullptr;
    }

    status = napi_set_named_property(env, exportObj, CLASS_NAME.c_str(), constructor);
    if (status != napi_ok) {
        LOGE("JsRun::Init Failed to set constructor");
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

JsRun::JsRun(std::shared_ptr<Run> run, std::shared_ptr<Typography> paragraph)
    : run_(run), paragraphCurrent_(paragraph)
{
}

napi_value JsRun::GetGlyphCount(napi_env env, napi_callback_info info)
{
    JsRun* me = CheckParamsAndGetThis<JsRun>(env, info);
    return (me != nullptr) ? me->OnGetGlyphCount(env, info) : nullptr;
}

napi_value JsRun::OnGetGlyphCount(napi_env env, napi_callback_info info)
{
    if (!run_) {
        LOGE("JsRun::OnGetGlyphCount run is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "JsRun::OnGetGlyphCount run is nullptr.");
    }
    return CreateJsNumber(env, static_cast<int64_t>(run_->GetGlyphCount()));
}

napi_value JsRun::GetGlyphs(napi_env env, napi_callback_info info)
{
    JsRun* me = CheckParamsAndGetThis<JsRun>(env, info);
    return (me != nullptr) ? me->OnGetGlyphs(env, info) : nullptr;
}

napi_value JsRun::OnGetGlyphs(napi_env env, napi_callback_info info)
{
    if (!run_) {
        LOGE("JsRun::OnGetGlyphs run is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "JsRun::OnGetGlyphs run is nullptr.");
    }

    std::vector<uint16_t> glyphs = run_->GetGlyphs();
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
        LOGE("JsRun::OnGetPositions run is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "JsRun::OnGetPositions run is nullptr.");
    }

    std::vector<Drawing::Point> positions = run_->GetPositions();
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
        LOGE("JsRun::OnGetOffsets run is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "JsRun::OnGetOffsets run is nullptr.");
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
        LOGE("JsRun::OnGetFont run is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "JsRun::OnGetFont run is nullptr.");
    }

    std::shared_ptr<Drawing::Font> fontPtr = std::make_shared<Drawing::Font>(run_->GetFont());
    if (!fontPtr) {
        LOGE("JsRun::OnGetFont fontPtr is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "JsRun::OnGetFont fontPtr is nullptr.");
    }

    Drawing::JsFont* jsFont = new(std::nothrow) Drawing::JsFont(fontPtr);
    if (jsFont == nullptr) {
        LOGE("OnGetFont jsFont is null!");
        return NapiGetUndefined(env);
    }

    napi_value resultValue = nullptr;
    napi_create_object(env, &resultValue);
    if (!resultValue) {
        LOGE("OnGetFont resultValue is NULL!");
        return NapiGetUndefined(env);
    }
    napi_wrap(env, resultValue, jsFont, Drawing::JsFont::Destructor, nullptr, nullptr);
    if (!resultValue) {
        LOGE("OnGetFont resultValue is null!");
        delete jsFont;
        return NapiGetUndefined(env);
    }
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
        LOGE("JsRun::OnPaint run is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "JsRun::OnPaint run is nullptr.");
    }
    size_t argc = ARGC_THREE;
    napi_value argv[ARGC_THREE] = {nullptr};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok || argc < ARGC_THREE) {
        LOGE("JsRun::OnPaint Argc is invalid: %{public}zu", argc);
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }
    Drawing::JsCanvas* jsCanvas = nullptr;
    double x = 0.0;
    double y = 0.0;
    napi_unwrap(env, argv[0], reinterpret_cast<void **>(&jsCanvas));
    if (jsCanvas == nullptr ||
        !(ConvertFromJsValue(env, argv[ARGC_ONE], x) && ConvertFromJsValue(env, argv[ARGC_TWO], y))) {
        LOGE("JsRun::OnPaint Argv is invalid");
        return NapiGetUndefined(env);
    }
    run_->Paint(jsCanvas->GetCanvas(), x, y);
    return NapiGetUndefined(env);
}

napi_value JsRun::CreateJsRun(napi_env env, std::shared_ptr<Run> run,
    std::shared_ptr<Typography> paragraph)
{
    ROSEN_LOGE(" clp-ark JsRun::CreateJsRun 000 run.get() = %p constructor_ = %p ",
        run.get(), constructor_);

    napi_value constructor = nullptr;
    napi_value result = nullptr;
    napi_status status = napi_get_reference_value(env, constructor_, &constructor);
    if (status == napi_ok) {
        g_Run = run;
        g_RunTypography = paragraph;
        status = napi_new_instance(env, constructor, 0, nullptr, &result);
        if (status == napi_ok) {
            ROSEN_LOGE(" clp-ark JsRun::CreateJsRun OnCreateJsTypography 800   ");
            return result;
        } else {
            ROSEN_LOGE(" clp-ark JsRun::CreateJsRun New instance could not be obtained");
        }
    }

    ROSEN_LOGE(" clp-ark JsRun::CreateJsRun CreateJsRun 900   ");
    return result;
}
} // namespace OHOS::Rosen
