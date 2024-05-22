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

#include "js_region.h"
#include "draw/path.h"
#include "path_napi/js_path.h"
#include "js_common.h"
#include "native_value.h"
#include "js_drawing_utils.h"

namespace OHOS::Rosen {
namespace Drawing {
thread_local napi_ref JsRegion::constructor_ = nullptr;
const std::string CLASS_NAME = "Region";
napi_value JsRegion::Init(napi_env env, napi_value exportObj)
{
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("setRect", JsRegion::SetRect),
        DECLARE_NAPI_FUNCTION("setPath", JsRegion::SetPath),
        DECLARE_NAPI_FUNCTION("isContainsPoint", JsRegion::IsContainsPoint),
        DECLARE_NAPI_FUNCTION("isContainsRegion", JsRegion::IsContainsRegion),
    };

    napi_value constructor = nullptr;
    napi_status status = napi_define_class(env, CLASS_NAME.c_str(), NAPI_AUTO_LENGTH, Constructor, nullptr,
                                           sizeof(properties) / sizeof(properties[0]), properties, &constructor);
    if (status != napi_ok) {
        ROSEN_LOGE("JsRegion::Init Failed to define Region class");
        return nullptr;
    }

    status = napi_create_reference(env, constructor, 1, &constructor_);
    if (status != napi_ok) {
        ROSEN_LOGE("JsRegion::Init Failed to create reference of constructor");
        return nullptr;
    }

    status = napi_set_named_property(env, exportObj, CLASS_NAME.c_str(), constructor);
    if (status != napi_ok) {
        ROSEN_LOGE("JsRegion::Init Failed to set constructor");
        return nullptr;
    }

    return exportObj;
}

napi_value JsRegion::Constructor(napi_env env, napi_callback_info info)
{
    size_t argCount = 0;
    napi_value jsThis = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argCount, nullptr, &jsThis, nullptr);
    if (status != napi_ok) {
        ROSEN_LOGE("JsRegion::Constructor failed to napi_get_cb_info");
        return nullptr;
    }
    JsRegion* jsRegion = new(std::nothrow) JsRegion();
    status = napi_wrap(env, jsThis, jsRegion,
                       JsRegion::Destructor, nullptr, nullptr);
    if (status != napi_ok) {
        delete jsRegion;
        ROSEN_LOGE("JsRegion::Constructor Failed to wrap native instance");
        return nullptr;
    }

    return jsThis;
}

void JsRegion::Destructor(napi_env env, void* nativeObject, void* finalize)
{
    (void)finalize;
    if (nativeObject != nullptr) {
        JsRegion* napi = reinterpret_cast<JsRegion*>(nativeObject);
        delete napi;
    }
}

JsRegion::JsRegion()
{
    region_ = new Region();
}

JsRegion::~JsRegion()
{
    delete region_;
}

napi_value JsRegion::SetRect(napi_env env, napi_callback_info info)
{
    JsRegion* jsRegion = CheckParamsAndGetThis<JsRegion>(env, info);
    Region* region = jsRegion->GetRegion();
    if (region == nullptr) {
        ROSEN_LOGE("JsRegion::SetRect region is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_ONE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_ONE);
    CHECK_EACH_PARAM(ARGC_ZERO, napi_object);

    napi_valuetype valueType = napi_undefined;
    if (argv[ARGC_ZERO] == nullptr || napi_typeof(env, argv[ARGC_ZERO], &valueType) != napi_ok ||
        valueType != napi_object) {
        ROSEN_LOGE("JsRegion::SetRect Argv[0] is invalid");
        return NapiGetUndefined(env);
    }

    napi_value tempValue = nullptr;
    double left = 0.0;
    double top = 0.0;
    double right = 0.0;
    double bottom = 0.0;
    napi_get_named_property(env, argv[ARGC_ZERO], "left", &tempValue);
    bool isLeftOk = ConvertFromJsValue(env, tempValue, left);
    napi_get_named_property(env, argv[ARGC_ZERO], "right", &tempValue);
    bool isRightOk = ConvertFromJsValue(env, tempValue, right);
    napi_get_named_property(env, argv[ARGC_ZERO], "top", &tempValue);
    bool isTopOk = ConvertFromJsValue(env, tempValue, top);
    napi_get_named_property(env, argv[ARGC_ZERO], "bottom", &tempValue);
    bool isBottomOk = ConvertFromJsValue(env, tempValue, bottom);
    if (!(isLeftOk && isRightOk && isTopOk && isBottomOk)) {
        ROSEN_LOGE("JsCanvas::SetRect Argv[0] is invalid");
        return NapiGetUndefined(env);
    }

    Drawing::RectI rectI = Drawing::RectI(left, top, right, bottom);
    return CreateJsValue(env, region->SetRect(rectI));
}

napi_value JsRegion::SetPath(napi_env env, napi_callback_info info)
{
    JsRegion* jsRegion = CheckParamsAndGetThis<JsRegion>(env, info);
    Region* region = jsRegion->GetRegion();
    if (region == nullptr) {
        ROSEN_LOGE("JsRegion::SetPath region is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_TWO] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_TWO);
    CHECK_EACH_PARAM(ARGC_ZERO, napi_object);
    CHECK_EACH_PARAM(ARGC_ONE, napi_object);

    JsPath* jsPath = nullptr;
    napi_unwrap(env, argv[ARGC_ZERO], reinterpret_cast<void**>(&jsPath));
    if (jsPath == nullptr) {
        ROSEN_LOGE("JsRegion::SetPath jsPath is nullptr");
        return NapiGetUndefined(env);
    }
    JsRegion* clip = nullptr;
    napi_unwrap(env,argv[ARGC_ONE], reinterpret_cast<void**>(&clip));
    if(clip == nullptr){
        ROSEN_LOGE("JsRegion::SetPath clip is nullptr");
        return nullptr;
    }
    return CreateJsValue(env, region->SetPath(*jsPath->GetPath(), *clip->GetRegion()));
}

napi_value JsRegion::IsContainsPoint(napi_env env, napi_callback_info info)
{
    JsRegion* jsRegion = CheckParamsAndGetThis<JsRegion>(env, info);
    Region* region = jsRegion->GetRegion();
    if (region == nullptr) {
        ROSEN_LOGE("JsRegion::IsContainsPoint region is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_TWO] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_TWO);
    CHECK_EACH_PARAM(ARGC_ZERO, napi_number);
    CHECK_EACH_PARAM(ARGC_ONE, napi_number);

    int32_t x = 0;
    int32_t y = 0;
    if (!(ConvertFromJsValue(env, argv[ARGC_ZERO], x) && ConvertFromJsValue(env, argv[ARGC_ONE], y))) {
        ROSEN_LOGE("JsRegion::IsContainsPoint Argv is invalid");
        return NapiGetUndefined(env);
    }

    return CreateJsValue(env, region->Contains(x, y));
}

napi_value JsRegion::IsContainsRegion(napi_env env, napi_callback_info info)
{
    JsRegion* jsRegion = CheckParamsAndGetThis<JsRegion>(env, info);
    Region* region = jsRegion->GetRegion();
    if (region == nullptr) {
        ROSEN_LOGE("JsRegion::IsContainsRegion region is nullptr");
        return NapiThrowError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
    }

    napi_value argv[ARGC_ONE] = {nullptr};
    CHECK_PARAM_NUMBER_WITHOUT_OPTIONAL_PARAMS(argv, ARGC_ONE);
    CHECK_EACH_PARAM(ARGC_ZERO, napi_object);

    JsRegion* other = nullptr;
    napi_unwrap(env,argv[ARGC_ZERO], reinterpret_cast<void**>(&other));
    if(other == nullptr){
        ROSEN_LOGE("JsRegion::IsContainsRegion other is nullptr");
        return nullptr;
    }
    return CreateJsValue(env, region->IsContainsRegion(*other->GetRegion()));
}

Region* JsRegion::GetRegion()
{
    return region_;
}
} // namespace Drawing
} // namespace OHOS::Rosen
