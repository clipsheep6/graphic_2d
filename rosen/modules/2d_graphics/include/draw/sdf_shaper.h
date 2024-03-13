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

#ifndef SDF_SHAPER_H
#define SDF_SHAPER_H

#include "common/rs_macros.h"
#include "utils/drawing_macros.h"
#include "utils/sdf_module.h"

#include <memory>
#include <vector>
#include <stack>

namespace OHOS {
namespace Rosen {
namespace Drawing {


class DRAWING_API SDFShape
{
public:
    typedef void(SDFShape::*SDFPraseFunc)();
    SDFShape() noexcept;;
    ~SDFShape() {};

    void BuildShader();
    std::string Getshader() const
    {
        return shader_;
    }
    float GetTimePoint() const
    {
        return timePoint_;
    }
    void SetTimePonit(float time)
    {
        timePoint_ = time;
    }

    void AddPara(float para)
    {
        params_.push_back(para);
    }
    
    void UpdatePara()
    {
        params_.clear();
    }

    int GetParaNum() const
    {
        return paraCount_;
    }

    std::vector<float> GetPara() const
    {
        return params_;
    }

    std::vector<float> GetTransPara() const
    {
        return transParams_;
    }

    std::vector<float> GetColorPara() const
    {
        return color_;
    }

    SDFModule** GetRootNode() const
    {
        return rootPtr_;
    }
    
    void UpdateTime(float time);
    SDFShape* BuildShape();
    void SetColor(std::string fillColor, std::string strokeColor);
    
    SDFModule* createNewNode(SDFModule** node, int val, SDF_TYPE type) const;
private:
    
    void addCircle();
    void addEllipse();
    void addSegment();
    void addArc();
    void addBox();
    void addEquilateralTriangle();
    void addRhombus();
    void addParallelogram();
    void addPentagon();
    void addHexagon();
    void addOctogon();

    void opRound();
    void opUnion();
    void opOnion();
    void opSubtraction();
    void opIntersection();
    void opXor();
    void opSmoothUnion();
    void opSmoothSubstraction();
    void opSmoothIntersection();

    void addCapsule();
    void addArrow();
    void addCross();
    void addRing();
    void addPartRingApprox();
    void addPartRing();
    void addQuestionMark();

    void AddTransform(SDFModule* node);
    void DeleteTransform(SDFModule* node);
    void PostOrder(SDFModule* node);
    void PraseNode(SDFModule* node);
    void DeletePostOrder(SDFModule* node);
    void DeleteNode(SDFModule* node);

    float UpdatePara(std::vector<std::pair<float, float>> para, float time);
    std::array<SDFPraseFunc, static_cast<int>(SDF_TYPE::END)> sdfPraseLUT_ =
    {
        nullptr,
        &SDFShape::addCircle,
        &SDFShape::addEllipse,
        &SDFShape::addSegment,
        &SDFShape::addArc,
        &SDFShape::addBox,
        &SDFShape::addEquilateralTriangle,
        &SDFShape::addRhombus,
        &SDFShape::addParallelogram,
        &SDFShape::addPentagon,
        &SDFShape::addHexagon,
        &SDFShape::addOctogon,
        &SDFShape::opRound,
        &SDFShape::opUnion,
        &SDFShape::opOnion,
        &SDFShape::opSubtraction,
        &SDFShape::opIntersection,
        &SDFShape::opXor,
        &SDFShape::opSmoothUnion,
        &SDFShape::opSmoothSubstraction,
        &SDFShape::opSmoothIntersection,
        &SDFShape::addCapsule,
        &SDFShape::addArrow,
        &SDFShape::addCross,
        &SDFShape::addRing,
        &SDFShape::addPartRingApprox,
        &SDFShape::addPartRing,
        &SDFShape::addQuestionMark,
    };
    bool shapeSet_[static_cast<int>(SDF_TYPE::END)] = {false};
    
    std::stack<std::string> transfrom_;
    std::stack<int> transfromNums_;
    std::string shader_ = "";
    std::string functionShader_ = R"(
    vec2 rotate(vec2 p, in float a)
    {
        float c = cos(a);
        float s = sin(a);
        mat2 rot = mat2(c, s, -s, c);
        return rot * p;
    }

    vec2 translate(vec2 p, vec2 t)
    {
        return p - t;
    }

    vec2 transform(vec2 p, float a, vec2 t)
    {
        p = translate(p, t);
        p = rotate(p, a);
        return p;
    }
    )";
    std::string paraShader_ = R"(uniform float width;
    uniform float fillcolpara1;
    uniform float fillcolpara2;
    uniform float fillcolpara3;
    uniform float strokecolpara1;
    uniform float strokecolpara2;
    uniform float strokecolpara3;
    )";
    std::string shapeShader_ = "";
    float timePoint_ = 0.0;
    int shapeCount_ = 0;
    int paraCount_ = 0;
    int transCount_ = 0;
    int transParaCount_ = 0;
    std::vector<float> color_ = {0.80, 0.80, 0.80, 0.80, 0.80, 0.80};
    std::vector<float> params_;
    std::vector<std::vector<std::pair<float, float>>> animateParams_;
    std::vector<float> transParams_;
    std::vector<std::vector<std::pair<float, float>>> animateTransParams_;
    
    SDFModule* rootNode_ = nullptr;
    SDFModule** rootPtr_ = &rootNode_;

    std::string FILL_STROKE_SHADER = R"(
vec4 fillStroke(float d, float th, vec3 fillCol, vec3 strokeCol)
{
    float w = 2.0 / width;
    float a1 = step(0., -d);
    float a2 = smoothstep(-w-th, -th, d) * smoothstep(w+th, th, d);
    vec3 col = mix(fillCol, strokeCol, a2);
    float a = max(a1, a2);
    return vec4(col, a);
}

vec4 shading(float d)
{
    vec3 fillCol = vec3(fillcolpara1, fillcolpara2, fillcolpara3);
    vec3 strokeCol = vec3(strokecolpara1, strokecolpara2, strokecolpara3);
    const float thickness = 0.001;  // thickness of stroke

    vec4 col = vec4(0.);
    col = fillStroke(d, thickness, fillCol, strokeCol);

    return col;
}
)";

    std::string BEGIN_SHADER = R"(
vec4 main(vec2 fragCoord)
{
    vec2 p = fragCoord.xy / width;
    vec2 c = vec2(0.5);
    vec2 p0 = p - c;
)";

    std::string END_SHADER = R"(
    return vec4(col.rgb * col.a, col.a);
}
)";
};

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_PROPERTY_RS_SDF_SHAPER_H