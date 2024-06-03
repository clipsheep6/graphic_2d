/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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
#include "effect_napi.h"
#include "image_napi_utils.h"
 
namespace {
    constexpr uint32_t NUM_0 = 0;
    constexpr uint32_t NUM_1 = 1;
    constexpr uint32_t NUM_2 = 2;
    constexpr uint32_t NUM_3 = 3;
    constexpr uint32_t NUM_4 = 4;
    constexpr uint32_t NUM_5 = 5;
    constexpr uint32_t NUM_6 = 6;
    constexpr uint32_t NUM_7 = 7;
    constexpr uint32_t NUM_8 = 8;
    constexpr uint32_t NUM_9 = 9;
}
 
namespace OHOS {
namespace Rosen {
static const std::string CLASS_NAME = "VisualEffect";
 
EffectNapi::EffectNapi()
{
}
 
EffectNapi::~EffectNapi()
{
}
 
thread_local napi_ref EffectNapi::sConstructor_ = nullptr;
 
napi_value EffectNapi::Init(napi_env env, napi_value exports)
{
    napi_property_descriptor static_prop[] = {
        DECLARE_NAPI_STATIC_FUNCTION("createEffect", CreateEffect),
        DECLARE_NAPI_STATIC_FUNCTION("createBrightnessBlender", CreateBrightnessBlender),
    };
 
    napi_value constructor = nullptr;
 
    napi_status status = napi_define_class(env, CLASS_NAME.c_str(),
                                           NAPI_AUTO_LENGTH, Constructor,
                                           nullptr,
                                           sizeof(static_prop) / sizeof(static_prop[0]), static_prop,
                                           &constructor);
    IMG_NAPI_CHECK_RET_D(IMG_IS_OK(status), nullptr, UIEFFECT_LOG_E("define class fail"));
 
    status = napi_create_reference(env, constructor, 1, &sConstructor_);
    if (!IMG_IS_OK(status)) {
        UIEFFECT_LOG_I("EffectNapi Init napi_create_reference falid");
        return nullptr;
    }
    napi_value global = nullptr;
    status = napi_get_global(env, &global);
    IMG_NAPI_CHECK_RET_D(IMG_IS_OK(status), nullptr, UIEFFECT_LOG_E("Init:get global fail"));
 
    status = napi_set_named_property(env, global, CLASS_NAME.c_str(), constructor);
    IMG_NAPI_CHECK_RET_D(IMG_IS_OK(status), nullptr, UIEFFECT_LOG_E("Init:set global named property fail"));
 
    status = napi_set_named_property(env, exports, CLASS_NAME.c_str(), constructor);
    IMG_NAPI_CHECK_RET_D(IMG_IS_OK(status), nullptr, UIEFFECT_LOG_E("set named property fail"));
 
    status = napi_define_properties(env, exports, IMG_ARRAY_SIZE(static_prop), static_prop);
    IMG_NAPI_CHECK_RET_D(IMG_IS_OK(status), nullptr, UIEFFECT_LOG_E("define properties fail"));
    return exports;
}
 
 
napi_value EffectNapi::Constructor(napi_env env, napi_callback_info info)
{
    size_t argCount = 0;
    napi_value jsThis = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argCount, nullptr, &jsThis, nullptr);
    if (status != napi_ok) {
        UIEFFECT_LOG_E("failed to napi_get_cb_info");
        return nullptr;
    }
 
    EffectNapi *effectNapi = new(std::nothrow) EffectNapi();
    if (effectNapi == nullptr) {
        UIEFFECT_LOG_E("EffectNapi Constructor is nullptr");
        return nullptr;
    }
    status = napi_wrap(env, jsThis, effectNapi, EffectNapi::Destructor, nullptr, nullptr);
    if (status != napi_ok) {
        delete effectNapi;
        UIEFFECT_LOG_E("Failed to wrap native instance");
        return nullptr;
    }
    return jsThis;
}
 
void EffectNapi::Destructor(napi_env env, void* nativeObject, void* finalize)
{
    EffectNapi *effectNapi = reinterpret_cast<EffectNapi*>(nativeObject);
 
    if (IMG_NOT_NULL(effectNapi)) {
        effectNapi->~EffectNapi();
    }
}
 
napi_value EffectNapi::CreateEffect(napi_env env, napi_callback_info info)
{
    VisualEffect* effectObj = new(std::nothrow) VisualEffect();
    if (effectObj == nullptr) {
        UIEFFECT_LOG_E("CreateEffect effectObj is nullptr");
        return nullptr;
    }
    napi_value object = nullptr;
    napi_create_object(env, &object);
    napi_wrap(
        env, object, effectObj,
        [](napi_env env, void* data, void* hint) {
            VisualEffect* effectObj = (VisualEffect*)data;
            delete effectObj;
        },
        nullptr, nullptr);
    napi_property_descriptor resultFuncs[] = {
        DECLARE_NAPI_FUNCTION("backgroundColorBlender", SetbackgroundColorBlender),
    };
    NAPI_CALL(env, napi_define_properties(env, object, sizeof(resultFuncs) / sizeof(resultFuncs[0]), resultFuncs));
    return object;
}
 
napi_value EffectNapi::CreateBrightnessBlender(napi_env env, napi_callback_info info)
{
    BrightnessBlender* blender = new(std::nothrow) BrightnessBlender();
    napi_value object = nullptr;
    napi_create_object(env, &object);
    napi_wrap(
        env, object, blender,
        [](napi_env env, void* data, void* hint) {
            BrightnessBlender* blenderObj = (BrightnessBlender*)data;
            delete blenderObj;
        },
        nullptr, nullptr);
 
    napi_value argValue[NUM_9] = {0};
    size_t argCount = NUM_9;
    napi_status status;
    IMG_JS_ARGS(env, info, status, argCount, argValue, object);
 
    if (argCount < NUM_8) {
        UIEFFECT_LOG_E("EffectNapi CreateBrightnessBlender object is Faild");
        return nullptr;
    }
    
    napi_set_named_property(env, object, "cubicRate", argValue[NUM_0]);
    napi_set_named_property(env, object, "quadRate", argValue[NUM_1]);
    napi_set_named_property(env, object, "linearRate", argValue[NUM_2]);
    napi_set_named_property(env, object, "degree", argValue[NUM_3]);
    napi_set_named_property(env, object, "saturation", argValue[NUM_4]);
    napi_set_named_property(env, object, "positiveCoeff", argValue[NUM_5]);
    napi_set_named_property(env, object, "negativeCoeff", argValue[NUM_6]);
    napi_set_named_property(env, object, "fraction", argValue[NUM_7]);
 
    if (object == nullptr) {
        UIEFFECT_LOG_E("EffectNapi CreateBrightnessBlender object is Faild");
    }
    return object;
}
 
static bool IsArrayForNapiValue(napi_env env, napi_value param, uint32_t &arraySize)
{
    bool isArray = false;
    arraySize = 0;
    if ((napi_is_array(env, param, &isArray) != napi_ok) || (isArray == false)) {
        return false;
    }
    if (napi_get_array_length(env, param, &arraySize) != napi_ok) {
        return false;
    }
    return true;
}
 
bool ParseJsDoubleValue(napi_value jsObject, napi_env env, const std::string& name, double& data)
{
    napi_value value = nullptr;
    napi_get_named_property(env, jsObject, name.c_str(), &value);
    napi_valuetype valueType = napi_undefined;
    napi_typeof(env, value, &valueType);
    if (valueType != napi_undefined) {
        if (napi_get_value_double(env, value, &data) != napi_ok) {
            UIEFFECT_LOG_E("[NAPI]Failed to convert parameter to data: %{public}s", name.c_str());
            return false;
        }
    } else {
        UIEFFECT_LOG_E("[NAPI]no property with: %{public}s", name.c_str());
        return false;
    }
    return true;
}
 
bool ParseJsVec3Value(napi_value jsObject, napi_env env, const std::string& name, Vector3f& vecTmp)
{
    napi_value param = nullptr;
    napi_get_named_property(env, jsObject, name.c_str(), &param);
 
    napi_valuetype valueType = napi_undefined;
    valueType = Media::ImageNapiUtils::getType(env, param);
    if (valueType == napi_undefined) {
        return true;
    }
    uint32_t arraySize = 0;
    if (!IsArrayForNapiValue(env, param, arraySize)) {
        UIEFFECT_LOG_E("GetRegionCoordinates get args fail, not array");
        return false;
    }
    if (arraySize < NUM_3) {
        UIEFFECT_LOG_E("GetRegionCoordinates coordinates num less than 4");
        return false;
    }
    for (size_t i = 0; i < NUM_3; i++) {
        napi_value jsValue;
        if ((napi_get_element(env, param, i, &jsValue)) != napi_ok) {
            UIEFFECT_LOG_E("GetRegionCoordinates get args fail");
            return false;
        }
        double value = 0.0;
        if (napi_get_value_double(env, jsValue, &value) == napi_ok) {
            vecTmp[i] = value;
        } else {
            UIEFFECT_LOG_E("GetRegionCoordinates region coordinates not double");
            return false;
        }
    }
    return true;
}
 
bool EffectNapi::ParseBrightnessBlender(
    napi_env env, napi_value jsObject, std::shared_ptr<BrightnessBlender> blender)
{
    double val;
    Vector3f tmpVector3;
    int parseTimes = 0;
    if (ParseJsDoubleValue(jsObject, env, "cubicRate", val)) {
        blender->SetCubicRate(static_cast<float>(val));
        parseTimes++;
    }
    if (ParseJsDoubleValue(jsObject, env, "quadRate", val)) {
        blender->SetQuadRate(static_cast<float>(val));
        parseTimes++;
    }
    if (ParseJsDoubleValue(jsObject, env, "linearRate", val)) {
        blender->SetLinearRate(static_cast<float>(val));
        parseTimes++;
    }
    if (ParseJsDoubleValue(jsObject, env, "degree", val)) {
        blender->SetDegree(static_cast<float>(val));
        parseTimes++;
    }
    if (ParseJsDoubleValue(jsObject, env, "saturation", val)) {
        blender->SetSaturation(static_cast<float>(val));
        parseTimes++;
    }
    if (ParseJsDoubleValue(jsObject, env, "fraction", val)) {
        blender->SetFraction(static_cast<float>(val));
        parseTimes++;
    }
    if (ParseJsVec3Value(jsObject, env, "positiveCoeff", tmpVector3)) {
        blender->SetPositiveCoeff(tmpVector3);
        parseTimes++;
    }
    if (ParseJsVec3Value(jsObject, env, "negativeCoeff", tmpVector3)) {
        blender->SetNegativeCoeff(tmpVector3);
        parseTimes++;
    }
    return (parseTimes == NUM_8);
}
 
napi_value EffectNapi::SetbackgroundColorBlender(napi_env env, napi_callback_info info)
{
    napi_status status;
    napi_value thisVar = nullptr;
    napi_value argValue[NUM_1] = {0};
    size_t argCount = NUM_1;
    IMG_JS_ARGS(env, info, status, argCount, argValue, thisVar);
 
    if (status != napi_ok) {
        UIEFFECT_LOG_E("EffectNapi SetbackgroundColorBlender parsr input Faild");
        return thisVar;
    }
    if (argCount != NUM_1) {
        UIEFFECT_LOG_E("EffectNapi SetbackgroundColorBlender the argCount does not equal NUM_1");
        return thisVar;
    }
 
    std::shared_ptr<BrightnessBlender> blender = std::make_shared<BrightnessBlender>();
    if (blender == nullptr) {
        UIEFFECT_LOG_E("EffectNapi SetbackgroundColorBlender blender is nullptr");
        return thisVar;
    }
    if (!ParseBrightnessBlender(env, argValue[0], blender)) {
        UIEFFECT_LOG_E("  SetbackgroundColorBlender input check fails");
        return thisVar;
    }
 
    std::shared_ptr<BackgroundColorEffectPara> para = std::make_shared<BackgroundColorEffectPara>();
    if (para == nullptr) {
        UIEFFECT_LOG_E("EffectNapi SetbackgroundColorBlender para is nullptr");
        return thisVar;
    }
    para->SetBlender(blender);
 
    VisualEffect* effectObj = nullptr;
    NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void**>(&effectObj)));
    if (effectObj == nullptr) {
        UIEFFECT_LOG_E("EffectNapi SetbackgroundColorBlender effectObj is nullptr");
        return thisVar;
    }
    effectObj->AddPara(para);
    return thisVar;
}
}  // namespace Rosen
}  // namespace OHOS