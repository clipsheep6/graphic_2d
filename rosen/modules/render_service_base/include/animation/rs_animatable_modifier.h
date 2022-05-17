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

#include "animation/rs_animatable_prop.h"

class SkCanvas;

namespace OHOS {
namespace Rosen {
template<typename T>
class RSAnimatableModifier {
public:
    RSAnimatableModifier() = default;
    virtual ~RSAnimatableModifier() = default;

    virtual void Draw(SkCanvas* canvas, const RSAnimatableProp<T>& property) = 0;

    RSAnimatableProp<T> property_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_ANIMATABLE_MODIFIER_H
