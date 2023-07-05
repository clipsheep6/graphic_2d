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

#include "animation/rs_animation_callback.h"

#include "ui/rs_ui_director.h"

namespace OHOS {
namespace Rosen {
AnimationCallback::AnimationCallback(const std::function<void()>& callback) : callback_(callback) {}

AnimationCallback::~AnimationCallback() {
    Execute();
}

void AnimationCallback::Execute()
{
    if (callback_ != nullptr) {
        // Asynchronous execute the callback function, we make a copy of the callback function to avoid the callback
        // being destroyed.
        RSUIDirector::PostTask([callback = this->callback_]() { callback(); });
    }
    OnExecute();
}

AnimationFinishCallback::AnimationFinishCallback(const std::function<void()>& callback, bool isTimingSensitive)
    : AnimationCallback(callback), isTimingSensitive_(isTimingSensitive)
{}

void AnimationFinishCallback::OnExecute()
{
    // Avoid the callback being executed more than once.
    callback_ = nullptr;
}

AnimationRepeatCallback::~AnimationRepeatCallback()
{
    // Avoid the callback being executed on animation finish.
    callback_ = nullptr;
};
} // namespace Rosen
} // namespace OHOS
