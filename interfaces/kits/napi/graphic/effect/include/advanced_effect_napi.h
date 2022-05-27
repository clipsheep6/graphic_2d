/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef ADVANCED_EFFECT_NAPI_H
#define ADVANCED_EFFECT_NAPI_H

#include <string>
#include "napi/native_api.h"
#include "napi/native_common.h"
#include "napi/native_node_api.h"


namespace OHOS {
namespace Rosen {
class AdvancedEffect;
class AdvancedEffectNapi {
public:
    AdvancedEffectNapi();
    ~AdvancedEffectNapi();
    static napi_value Init(napi_env env, napi_value exports);
private:
    static napi_value Constructor(napi_env env, napi_callback_info info);
    static void Destructor(napi_env env, void* nativeObject, void* finalize_hint);
    
    // static func
    static napi_value Test(napi_env env, napi_callback_info info);

    // variables
    static thread_local napi_ref sConstructor_;
    napi_env env_;
    napi_ref wrapper_;
    // for create AdvancedEffect 
    static std::shared_ptr<AdvancedEffect> sAdvancedEffect_;
    std::shared_ptr<AdvancedEffect> nativeAdvancedEffect_;
};
}//Rosen
}//OHOS
#endif // ADVANCED_EFFECT_NAPI_H
