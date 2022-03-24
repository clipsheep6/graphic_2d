/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "rs_window_animation_manager.h"

#include <memory>

#include <js_runtime_utils.h>
#include <platform/common/rs_log.h>
#include <singleton_container.h>
#include <window_adapter.h>

#include "rs_window_animation_utils.h"

namespace OHOS {
namespace Rosen {

using namespace AbilityRuntime;

const std::string RSWindowAnimationManager::TAG = "RSWindowAnimationManager";

NativeValue* RSWindowAnimationManager::Init(NativeEngine* engine, NativeValue* exportObj)
{
    ROSEN_LOGI_WITH_TAG(RSWindowAnimationManager::TAG, "Init");

    if (engine == nullptr || exportObj == nullptr) {
        ROSEN_LOGE_WITH_TAG(RSWindowAnimationManager::TAG, "Engine or exportObj is null!");
        return nullptr;
    }

    auto object = ConvertNativeValueTo<NativeObject>(exportObj);
    if (object == nullptr) {
        ROSEN_LOGE_WITH_TAG(RSWindowAnimationManager::TAG, "Object is null");
        return nullptr;
    }

    std::unique_ptr<RSWindowAnimationManager> windowAnimationManager(new RSWindowAnimationManager());
    object->SetNativePointer(windowAnimationManager.release(), RSWindowAnimationManager::Finalizer, nullptr);

    ROSEN_LOGI_WITH_TAG(RSWindowAnimationManager::TAG,"BindNativeFunction begin");
    BindNativeFunction(*engine, *object, "setWindowAnimationController", RSWindowAnimationManager::SetWindowAnimationController);
    ROSEN_LOGI_WITH_TAG(RSWindowAnimationManager::TAG, "BindNativeFunction end");

    return engine->CreateUndefined();
}

void RSWindowAnimationManager::Finalizer(NativeEngine* engine, void* data, void* hint)
{
    ROSEN_LOGI_WITH_TAG(RSWindowAnimationManager::TAG, "Finalizer is called");
    std::unique_ptr<RSWindowAnimationManager>(static_cast<RSWindowAnimationManager*>(data));
}

NativeValue* RSWindowAnimationManager::SetWindowAnimationController(NativeEngine* engine, NativeCallbackInfo* info)
{
    ROSEN_LOGI_WITH_TAG(RSWindowAnimationManager::TAG, "SetWindowAnimationController");
    auto me = CheckParamsAndGetThis<RSWindowAnimationManager>(engine, info);
    return (me != nullptr) ? me->OnSetWindowAnimationController(*engine, *info) : nullptr;
}

NativeValue* RSWindowAnimationManager::OnSetWindowAnimationController(NativeEngine& engine, NativeCallbackInfo& info)
{
    ROSEN_LOGI_WITH_TAG(RSWindowAnimationManager::TAG, "OnSetWindowAnimationController");

    // only support one param
    if (info.argc != 1) {
        ROSEN_LOGE_WITH_TAG(RSWindowAnimationManager::TAG, "No enough params!");
        return engine.CreateUndefined();
    }

    if (controller_ != nullptr) {
        ROSEN_LOGE_WITH_TAG(RSWindowAnimationManager::TAG, "Already had a controller, don't set repeatedly!");
        return engine.CreateUndefined();
    }

    if (handler_ == nullptr) {
        handler_ = std::make_shared<AppExecFwk::EventHandler>(AppExecFwk::EventRunner::GetMainEventRunner());
    }

    controller_ = new RSWindowAnimationController(engine, handler_);
    controller_->SetJsController(info.argv[0]);

    SingletonContainer::Get<WindowAdapter>().SetWindowAnimationController(controller_);

    return engine.CreateUndefined();
}

} // namespace Rosen
} // namespace OHOS
