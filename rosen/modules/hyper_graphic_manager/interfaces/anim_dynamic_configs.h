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

#ifndef HGM_ANIM_DYNAMIC_CONFIGS_H
#define HGM_ANIM_DYNAMIC_CONFIGS_H

namespace OHOS {
namespace Rosen {

enum class AnimType {
    TRANSLATE,
    SCALE,
    ROTATION,
};

class AnimaDynamicConfig {
public:
    AnimaDynamicConfig() : animType_(AnimType::SCALE), animName_("1"), minSpeed_(0),
        maxSpeed_(0), preferredFps_(0) {}

    AnimaDynamicConfig(AnimType animType, std::string animName, int32_t minSpeed, int32_t maxSpeed,
        int32_t preferredFps) : animType_(animType), animName_(animName), minSpeed_(minSpeed),
        maxSpeed_(maxSpeed), preferredFps_(preferredFps) {}

    AnimType GetAnimType()
    {
        return animType_;
    }

    std::string GetAnimName()
    {
        return animName_;
    }

    int32_t GetMinSpeed()
    {
        return minSpeed_;
    }

    int32_t GetMaxSpeed()
    {
        return maxSpeed_;
    }

    int32_t GetPreferredFps()
    {
        return preferredFps_;
    }

    void SetAnimType(AnimType type)
    {
        animType_ = type;
    }

    void SetAnimName(std::string name)
    {
        animName_ = name;
    }

    void SetMinSpeed(int32_t minSpeed)
    {
        minSpeed_ = minSpeed;
    }

    void SetMaxSpeed(int32_t maxSpeed)
    {
        maxSpeed_ = maxSpeed;
    }

    void SetPreferredFps(int32_t preferredFps)
    {
        preferredFps_ = preferredFps;
    }

private:
    AnimType animType_ = AnimType::SCALE;
    std::string animName_ = "1";
    int32_t minSpeed_ = 0;
    int32_t maxSpeed_ = 0;
    int32_t preferredFps_ = 0;
};

class AnimDynamicCfgs {
public:
    std::vector<AnimaDynamicConfig> GetAnimaDynamicConfigs()
    {
        return animDynamicCfgs_;
    }

    void AddAnimaDynamicConfigs(AnimaDynamicConfig config)
    {
        animDynamicCfgs_.push_back(config);
    }

    int GetAnimaDynamicConfigsCount()
    {
        return animDynamicCfgs_.size();
    }

private:
    std::vector<AnimaDynamicConfig> animDynamicCfgs_;
};
} // namespace Rosen
} // namespace OHOS
#endif // HGM_ANIM_DYNAMIC_CONFIGS_H