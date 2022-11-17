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

#ifndef RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_UI_ANIMATION_MANAGER_H
#define RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_UI_ANIMATION_MANAGER_H

#include <memory>
#include <unordered_map>

#include "common/rs_common_def.h"

namespace OHOS {
namespace Rosen {
class RSModifierManager;
class RSRenderAnimation;

class RS_EXPORT RSUIAnimationManager {
public:
    RSUIAnimationManager();
    virtual ~RSUIAnimationManager() = default;

    void AddAnimation(const std::shared_ptr<RSRenderAnimation>& animation);
    void RemoveAnimation(const AnimationId keyId);

    bool Animate(int64_t time);
    void Draw();

private:
    void OnAnimationFinished(const std::shared_ptr<RSRenderAnimation>& animation);

    std::shared_ptr<RSModifierManager> modifierManager_;
    std::unordered_map<AnimationId, std::shared_ptr<RSRenderAnimation>> animations_;

    template <typename T>
    friend class RSAnimatableProperty;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_UI_ANIMATION_MANAGER_H
