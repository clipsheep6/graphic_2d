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

#include "animation/rs_transition_effect.h"

#include "animation/rs_render_transition_effect.h"

namespace OHOS {
namespace Rosen {
const std::shared_ptr<const RSTransitionEffect> RSTransitionEffect::OPACITY = RSTransitionEffect::Create()->Opacity(0);

const std::shared_ptr<const RSTransitionEffect> RSTransitionEffect::SCALE =
    RSTransitionEffect::Create()->Scale({ 0.f, 0.f, 0.f });

const std::shared_ptr<const RSTransitionEffect> RSTransitionEffect::EMPTY = RSTransitionEffect::Create();

std::shared_ptr<RSTransitionEffect> RSTransitionEffect::Create()
{
    return std::shared_ptr<RSTransitionEffect>(new RSTransitionEffect());
}

std::shared_ptr<RSTransitionEffect> RSTransitionEffect::Opacity(float opacity)
{
    auto opacityEffect = std::make_shared<RSTransitionFade>(opacity);
    transitionInEffects_.push_back(opacityEffect);
    transitionOutEffects_.push_back(opacityEffect);
    return shared_from_this();
}

std::shared_ptr<RSTransitionEffect> RSTransitionEffect::Scale(const Vector3f& scale)
{
    auto scaleEffect = std::make_shared<RSTransitionScale>(scale.x_, scale.y_, scale.z_);
    transitionInEffects_.push_back(scaleEffect);
    transitionOutEffects_.push_back(scaleEffect);
    return shared_from_this();
}

std::shared_ptr<RSTransitionEffect> RSTransitionEffect::Translate(const Vector3f& translate)
{
    auto translateEffect = std::make_shared<RSTransitionTranslate>(translate.x_, translate.y_, translate.z_);
    transitionInEffects_.push_back(translateEffect);
    transitionOutEffects_.push_back(translateEffect);
    return shared_from_this();
}

std::shared_ptr<RSTransitionEffect> RSTransitionEffect::Rotate(const Vector4f& axisAngle)
{
    auto rotateEffect = std::make_shared<RSTransitionRotate>(axisAngle.x_, axisAngle.y_, axisAngle.z_, axisAngle.w_);
    transitionInEffects_.push_back(rotateEffect);
    transitionOutEffects_.push_back(rotateEffect);
    return shared_from_this();
}

std::shared_ptr<RSTransitionEffect> RSTransitionEffect::Asymmetric(
    const std::shared_ptr<RSTransitionEffect>& transitionIn, const std::shared_ptr<RSTransitionEffect>& transitionOut)
{
    return std::shared_ptr<RSTransitionEffect>(new RSTransitionEffect(transitionIn, transitionOut));
}

RSTransitionEffect::RSTransitionEffect(
    const std::shared_ptr<RSTransitionEffect>& transitionIn, const std::shared_ptr<RSTransitionEffect>& transitionOut)
    : transitionInEffects_(transitionIn->GetTransitionInEffects()),
      transitionOutEffects_(transitionOut->GetTransitionOutEffects())
{}

const std::vector<std::shared_ptr<RSRenderTransitionEffect>>& RSTransitionEffect::GetTransitionInEffects() const
{
    return transitionInEffects_;
}

const std::vector<std::shared_ptr<RSRenderTransitionEffect>>& RSTransitionEffect::GetTransitionOutEffects() const
{
    return transitionOutEffects_;
}
} // namespace Rosen
} // namespace OHOS
