/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#include "advanced_effect_napi.h"
#include "color_picker_napi.h"
#include "media_errors.h"
#include "hilog/log.h"
#include "effect_napi_utils.h"
#include "advanced_effect.h"


using OHOS::HiviewDFX::HiLog;
namespace {
//constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, LOG_DOMAIN, "EffectNapi"};
//    constexpr uint32_t NUM_0 = 0;
//    constexpr uint32_t NUM_1 = 1;
//    constexpr uint32_t NUM_2 = 2;
//    constexpr uint32_t NUM_3 = 3;
//    constexpr uint32_t NUM_4 = 4;
}

namespace OHOS {
namespace Rosen {

static const std::string CLASS_NAME = "AdvancedEffect";
thread_local napi_ref AdvancedEffectNapi::sConstructor_ = nullptr;
std::shared_ptr<AdvancedEffect> AdvancedEffectNapi::sAdvancedEffect_ = nullptr;

struct AdvancedEffectAsyncContext {
    napi_env env;
    napi_async_work work;
    napi_deferred deferred;
    napi_ref callbackRef;
    uint32_t status;
    AdvancedEffectNapi *nConstructor; 
    std::shared_ptr<AdvancedEffect> rAdvancedEffect;
    uint32_t resultUint32;
};

AdvancedEffectNapi::AdvancedEffectNapi()
    :env_(nullptr), wrapper_(nullptr)
{

}

AdvancedEffectNapi::~AdvancedEffectNapi()
{
    if (nativeAdvancedEffect_ != nullptr) {
        nativeAdvancedEffect_ = nullptr;
    }
    if (wrapper_ != nullptr) {
        napi_delete_reference(env_, wrapper_);
    }
}

napi_value AdvancedEffectNapi::Test(napi_env env, napi_callback_info info)
{
    return nullptr;
}

napi_value AdvancedEffectNapi::Init(napi_env env, napi_value exports)
{
    napi_property_descriptor props[] = {
        DECLARE_NAPI_FUNCTION("test", Test),
    };

    napi_property_descriptor static_prop[] = {
        DECLARE_NAPI_FUNCTION("test", Test),
    };

    napi_value constructor = nullptr;

    VALUE_NAPI_CHECK_RET_D(VALUE_IS_OK(
        napi_define_class(env, CLASS_NAME.c_str(), NAPI_AUTO_LENGTH,
                          Constructor, nullptr, VALUE_ARRAY_SIZE(props),
                          props, &constructor)),
        nullptr,
        HiLog::Error(LABEL, "define class fail")
    );

    VALUE_NAPI_CHECK_RET_D(VALUE_IS_OK(
        napi_create_reference(env, constructor, 1, &sConstructor_)),
        nullptr,
        HiLog::Error(LABEL, "create reference fail")
    );

    napi_value global = nullptr;
    VALUE_NAPI_CHECK_RET_D(VALUE_IS_OK(
        napi_get_global(env, &global)),
        nullptr,
        HiLog::Error(LABEL, "Init:get global fail")
    );

    VALUE_NAPI_CHECK_RET_D(VALUE_IS_OK(
        napi_set_named_property(env, global, CLASS_NAME.c_str(), constructor)),
        nullptr,
        HiLog::Error(LABEL, "Init:set global named property fail")
    );

    VALUE_NAPI_CHECK_RET_D(VALUE_IS_OK(
        napi_set_named_property(env, exports, CLASS_NAME.c_str(), constructor)),
        nullptr,
        HiLog::Error(LABEL, "set named property fail")
    );

    VALUE_NAPI_CHECK_RET_D(VALUE_IS_OK(
        napi_define_properties(env, exports, VALUE_ARRAY_SIZE(static_prop), static_prop)),
        nullptr,
        HiLog::Error(LABEL, "define properties fail")
    );

    HiLog::Debug(LABEL, "Init success");
    return exports;
}


napi_value AdvancedEffectNapi::Constructor(napi_env env, napi_callback_info info)
{
    napi_value undefineVar = nullptr;
    napi_get_undefined(env, &undefineVar);

    napi_status status;
    napi_value thisVar = nullptr;
    napi_get_undefined(env, &thisVar);

    HiLog::Debug(LABEL, "Constructor IN");
    VALUE_JS_NO_ARGS(env, info, status, thisVar);

    VALUE_NAPI_CHECK_RET(VALUE_IS_READY(status, thisVar), undefineVar);
    std::unique_ptr<AdvancedEffectNapi> pAdvancedEffectNapi = std::make_unique<AdvancedEffectNapi>();

    VALUE_NAPI_CHECK_RET(VALUE_NOT_NULL(pAdvancedEffectNapi), undefineVar);

    pAdvancedEffectNapi->env_ = env;
    pAdvancedEffectNapi->nativeAdvancedEffect_ = sAdvancedEffect_;

    status = napi_wrap(env, thisVar, reinterpret_cast<void*>(pAdvancedEffectNapi.get()),
                        AdvancedEffectNapi::Destructor, nullptr, &(pAdvancedEffectNapi->wrapper_));
    VALUE_NAPI_CHECK_RET_D(VALUE_IS_OK(status), undefineVar, HiLog::Error(LABEL, "Failure wrapping js to native napi"));

    pAdvancedEffectNapi.release();
    sAdvancedEffect_ = nullptr;

    return thisVar;
}

void AdvancedEffectNapi::Destructor(napi_env env, void *nativeObject, void *finalize)
{
    AdvancedEffectNapi *pImgSrcNapi = reinterpret_cast<AdvancedEffectNapi*>(nativeObject);
    if (pImgSrcNapi != nullptr) {
        pImgSrcNapi->~AdvancedEffectNapi();
    }
} 

}  // namespace Media
}  // namespace OHOS
