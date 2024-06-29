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

#ifndef RS_COMMON_HOOK_H
#define RS_COMMON_HOOK_H
#include <functional>
namespace OHOS::Rosen {
class RsCommonHook {
private:
    std::function<void()> startNewAniamtionFunc_ = nullptr;

public:
    static RsCommonHook& Instance();
    void RegisterStartNewAnimationListner(std::function<void()> listner);
    void OnStartNewAnimation();
};
} // namespace OHOS::Rosen
#endif