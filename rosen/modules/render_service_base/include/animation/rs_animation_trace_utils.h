/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
#ifndef RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_ANIMATION_TRACE_UTILS_H
#define RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_ANIMATION_TRACE_UTILS_H

#include "animation/rs_render_property_animation.h"
#include "modifier/rs_render_property.h"

namespace OHOS {
namespace Rosen {
class RSB_EXPORT RSAnimationTraceUtils {
public:
    static RSAnimationTraceUtils &GetInstance()
    {
        return instance_;
    };
    void addAnimationCreateTrace(const int nodeId, const int propertyId, const int animationType,
        const std::shared_ptr<RSRenderPropertyBase> startValue, const std::shared_ptr<RSRenderPropertyBase> endValue);
    void addAnimationFrameTrace(const int nodeId, const int animationId, const int propertyId,
        const float fraction, const std::shared_ptr<RSRenderPropertyBase> value);
    void addTranstionFrameTrace(const int nodeId, const int animationId, const float fraction,
        const float valueFraction);
    void addTranstionEffectValueTrace(const int propertyId, const float fraction,
        const uint16_t type, const std::shared_ptr<RSRenderPropertyBase> value);
private:
    void AddTraceFlag(const std::string& str);
    std::string ParseRenderPropertyVaule(const std::shared_ptr<RSRenderPropertyBase> value);
    std::string ParseRenderPropertyVauleOther(const std::shared_ptr<RSRenderPropertyBase> value);
    static RSAnimationTraceUtils instance_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_ANIMATION_TRACE_UTILS_H
