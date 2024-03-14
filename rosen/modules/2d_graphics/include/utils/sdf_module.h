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

#ifndef SDF_MODULE_H
#define SDF_MODULE_H

#include <vector>
#include "utils/drawing_macros.h"
#include "utils/scalar.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

enum class SDF_TYPE {
    // Basic SDF primitives
    BEGIN = 0,
    SD_CIRCLE,
    SD_ELLIPSE,
    SD_SEGMENT,
    SD_ARC,
    SD_BOX,
    SD_EQUILATERAL_TRIANGLE,
    SD_RHOMBUS,
    SD_PARALLELOGRAM,
    SD_PENTAGON,
    SD_HEXAGON,
    SD_OCTOGON,
    // SDF Operators
    OP_ROUND,
    OP_UNION,
    OP_ONION,
    OP_SUBTRACTION,
    OP_INTERSECTION,
    OP_XOR,
    OP_SMOOTH_UNION,
    OP_SMOOTH_SUBSTRACTION,
    OP_SMOOTH_INTERSECTIOPN,
    // SDF Advanced
    SD_CAPSULE,
    SD_ARROW,
    SD_CROSS,
    SD_RING,
    SD_PART_RING_APPROX,
    SD_PART_RING,
    SD_QUESTION_MARK,
    END,
};

class DRAWING_API SDFModule {
public:
    inline SDFModule() noexcept;
    inline SDFModule(SDF_TYPE type) noexcept;
    inline ~SDFModule() noexcept;

    inline void SetShader(const std::string s);
    inline void SetParameter(const std::vector<float>& p);
    inline void SetTranslate(const std::vector<float>& p);
    inline void SetAnimateParameter(const std::vector<std::vector<std::pair<float, float>>>& p);
    inline void SetAnimateTranslate(const std::vector<std::vector<std::pair<float, float>>>& p);

    //friend inline const SDFModule operator+(const SDFModule& sdf1, const SDFModule& sdf2);

    SDFModule* leftModule_ = nullptr;
    SDFModule* rightModule_ = nullptr;

private:
    std::string shader_;
    SDF_TYPE type_;
    int val_;
    std::vector<float> para_;
    std::vector<float> trans_;
    std::vector<std::vector<std::pair<float, float>>> animatePara_;
    std::vector<std::vector<std::pair<float, float>>> animateTrans_;
    bool isAnimation_;
friend class SDFShape;
};

inline SDFModule::SDFModule() noexcept {}

inline SDFModule::SDFModule(SDF_TYPE type) noexcept : type_(type) {}

inline SDFModule::~SDFModule() noexcept {}


inline void SDFModule::SetShader(const std::string s)
{
    shader_ = s;
}

inline void SDFModule::SetParameter(const std::vector<float>& p)
{
    para_ = p;
}

inline void SDFModule::SetTranslate(const std::vector<float>& p)
{
    trans_ = p;
}

inline void SDFModule::SetAnimateParameter(const std::vector<std::vector<std::pair<float, float>>>& p)
{
    animatePara_ = p;
    isAnimation_ = true;
}

inline void SDFModule::SetAnimateTranslate(const std::vector<std::vector<std::pair<float, float>>>& p)
{
    animateTrans_ = p;
    isAnimation_ = true;
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif