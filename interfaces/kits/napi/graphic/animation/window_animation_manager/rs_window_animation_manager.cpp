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
#include <rs_window_animation_log.h>
#include <singleton_container.h>
#include <window_adapter.h>

#include "rs_window_animation_utils.h"

namespace OHOS {
namespace Rosen {
using namespace AbilityRuntime;
constexpr size_t ARGC_ONE = 1;

NativeValue* RSWindowAnimationManager::Init(NativeEngine* engine, NativeValue* exportObj)
{
    WALOGD("Init");
    if (engine == nullptr || exportObj == nullptr) {
        WALOGE("Engine or exportObj is null!");
        return nullptr;
    }

    auto object = ConvertNativeValueTo<NativeObject>(exportObj);
    if (object == nullptr) {
        WALOGE("Object is null");
        return nullptr;
    }

    auto windowAnimationManager = std::make_unique<RSWindowAnimationManager>();
    object->SetNativePointer(windowAnimationManager.release(), RSWindowAnimationManager::Finalizer, nullptr);

    BindNativeFunction(*engine, *object, "setController", RSWindowAnimationManager::SetController);
    return nullptr;
}

void RSWindowAnimationManager::Finalizer(NativeEngine* engine, void* data, void* hint)
{
    std::unique_ptr<RSWindowAnimationManager>(static_cast<RSWindowAnimationManager*>(data));
}

NativeValue* RSWindowAnimationManager::SetController(NativeEngine* engine, NativeCallbackInfo* info)
{
    WALOGD("SetController");
    auto me = CheckParamsAndGetThis<RSWindowAnimationManager>(engine, info);
    return (me != nullptr) ? me->OnSetController(*engine, *info) : nullptr;
}

NativeValue* RSWindowAnimationManager::OnSetController(NativeEngine& engine, NativeCallbackInfo& info)
{
    WALOGD("OnSetController");
    // only support one param
    if (info.argc != ARGC_ONE) {
        WALOGE("No enough params!");
        return nullptr;
    }

    sptr<RSWindowAnimationController> controller = new RSWindowAnimationController(engine);
    controller->SetJsController(info.argv[0]);
    auto singleton = SingletonContainer::Get<WindowAdapter>();
    if (singleton == nullptr) {
        WALOGE("Set controller failed, because window adapter is null.");
        return nullptr;
    }
    singleton->SetWindowAnimationController(controller);
    return nullptr;
}
} // namespace Rosen
} // namespace OHOS
