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

#include "rs_window_animation_controller.h"

#include <js_runtime_utils.h>
#include <platform/common/rs_log.h>

#include "rs_window_animation_utils.h"

namespace OHOS {
namespace Rosen {

using namespace AbilityRuntime;

const std::string RSWindowAnimationController::TAG = "RSWindowAnimationController";

RSWindowAnimationController::RSWindowAnimationController(NativeEngine& engine,
    const std::shared_ptr<AppExecFwk::EventHandler>& handler)
    : engine_(engine),
      handler_(handler)
{
}

void RSWindowAnimationController::SetJsController(NativeValue* jsController)
{
    jsConterller_ = std::unique_ptr<NativeReference>(engine_.CreateReference(jsController, 1));
}

void RSWindowAnimationController::OnTransition(const sptr<RSWindowAnimationTarget>& from, const sptr<RSWindowAnimationTarget>& to)
{
    ROSEN_LOGI_WITH_TAG(RSWindowAnimationController::TAG, "OnTransition");
    if (handler_ == nullptr) {
        ROSEN_LOGE_WITH_TAG(RSWindowAnimationController::TAG, "Handler is null!");
        return;
    }

    wptr<RSWindowAnimationController> controllerWptr = this;
    auto task = [controllerWptr, from, to]() {
        auto controllerSptr = controllerWptr.promote();
        if (controllerSptr == nullptr) {
            ROSEN_LOGE_WITH_TAG(RSWindowAnimationController::TAG, "Controller is null!");
            return;
        }

        controllerSptr->HandleOnTransition(from, to);
    };

    handler_->PostTask(task, AppExecFwk::EventHandler::Priority::IMMEDIATE);
}

void RSWindowAnimationController::HandleOnTransition(const sptr<RSWindowAnimationTarget>& from, const sptr<RSWindowAnimationTarget>& to)
{
    ROSEN_LOGI_WITH_TAG(RSWindowAnimationController::TAG, "HandleOnTransition");
    NativeValue* argv[] = {
        RSWindowAnimationUtils::CreateJsWindowAnimationTarget(engine_, *from),
        RSWindowAnimationUtils::CreateJsWindowAnimationTarget(engine_, *to)
    };
    CallJsFunction("onTransition", argv, 2);
}

void RSWindowAnimationController::CallJsFunction(const std::string& methodName, NativeValue* const* argv, size_t argc)
{
    ROSEN_LOGI_WITH_TAG(RSWindowAnimationController::TAG, "call js function:%s", methodName.c_str());
    if (jsConterller_ == nullptr) {
        ROSEN_LOGE_WITH_TAG(RSWindowAnimationController::TAG, "jsConterller_ is null!");
        return;
    }

    auto jsControllerValue = jsConterller_->Get();
    auto jsControllerObj = ConvertNativeValueTo<NativeObject>(jsControllerValue);
    if (jsControllerObj == nullptr) {
        ROSEN_LOGE_WITH_TAG(RSWindowAnimationController::TAG, "jsControllerObj is null!");
        return;
    }

    auto method = jsControllerObj->GetProperty(methodName.c_str());
    if (method == nullptr) {
        ROSEN_LOGE_WITH_TAG(RSWindowAnimationController::TAG, "Failed to get method from object!");
        return;
    }

    engine_.CallFunction(jsControllerValue, method, argv, argc);
}

} // namespace Rosen
} // namespace OHOS
