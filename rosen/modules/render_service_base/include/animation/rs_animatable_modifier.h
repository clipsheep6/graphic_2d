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

#ifndef RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_ANIMATABLE_MODIFIER_H
#define RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_ANIMATABLE_MODIFIER_H

#include <memory>

#include "animation/rs_animatable_prop.h"

class SkCanvas;

namespace OHOS {
namespace Rosen {

class RSContext;

class RSAnimatableModifier {
public:
    RSAnimatableModifier() = default;
    virtual ~RSAnimatableModifier() = default;

    virtual void Draw(RSContext& context) = 0;

    virtual void CreateRenderModifier() = 0;
};

template<typename T>
class RSModifier : public RSAnimatableModifier {
public:
    RSModifier(const std::shared_ptr<RSAnimatableProp<T>>& property) : property_(property)
    {}
    virtual ~RSModifier() = default;

    virtual void Draw(RSContext& context) {}

    virtual void CreateRenderModifier() {}

protected:
    std::shared_ptr<RSAnimatableProp<T>> property_;
};

class RSRotationModifier : public RSAnimatableModifier<float> {
public:
    RSRotationModifier(const std::shared_ptr<RSAnimatableProp<float>>& property) : RSModifier<float>(property)
    {}
    virtual ~RSRotationModifier() = default;

    virtual void Draw(RSContext& context) override
    {}

    virtual void CreateRenderModifier() override
    {}

};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_ANIMATABLE_MODIFIER_H
