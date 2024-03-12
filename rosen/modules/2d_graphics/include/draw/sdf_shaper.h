#ifndef SDF_SHAPER_H
#define SDF_SHAPER_H

#include <memory>
#include <vector>
#include <stack>

#include "common/rs_macros.h"
#include "drawing/engine_adapter/impl_interface/path_impl.h"
#include "utils/drawing_macros.h"
#include "utils/matrix.h"
#include "utils/point.h"
#include "utils/rect.h"
#include "utils/sdf_module.h"


namespace OHOS {
namespace Rosen {
namespace Drawing {


class DRAWING_API SDFShape
{
public:
    typedef void(SDFShape::*SDFPraseFunc)(SDFModule*);
    SDFShape() noexcept;;
    ~SDFShape() {};

    void BuildShader();
    std::string Getshader() const
    {
        return m_shader;
    }
    float GetTimePoint() const
    {
        return m_timePoint;
    }
    void SetTimePonit(float time)
    {
        m_timePoint = time;
    }

    void AddPara(float para)
    {
        m_params.push_back(para);
    }
    
    void UpdatePara()
    {
        m_params.clear();
    }

    int GetParaNum() const
    {
        return m_paraCount;
    }

    std::vector<float> GetPara() const
    {
        return m_params;
    }

    std::vector<float> GetTransPara() const
    {
        return m_transParams;
    }

    std::vector<float> GetColorPara() const
    {
        return m_color;
    }

    SDFModule** GetRootNode() const
    {
        return m_rootPtr;
    }
    
    void UpdateTime(float time);
    SDFShape* BuildShape();
    void SetColor(std::string fillColor, std::string strokeColor);
    

    

    SDFModule* createNewNode(SDFModule** node, int val, SDF_TYPE type) const;
    

private:
    
    void addCircle(SDFModule* node);
    void addEllipse(SDFModule* node);
    void addSegment(SDFModule* node);
    void addArc(SDFModule* node);
    void addBox(SDFModule* node);
    void addEquilateralTriangle(SDFModule* node);
    void addRhombus(SDFModule* node);
    void addParallelogram(SDFModule* node);
    void addPentagon(SDFModule* node);
    void addHexagon(SDFModule* node);
    void addOctogon(SDFModule* node);

    void opRound(SDFModule* node);
    void opUnion(SDFModule* node);
    void opOnion(SDFModule* node);
    void opSubtraction(SDFModule* node);
    void opIntersection(SDFModule* node);
    void opXor(SDFModule* node);
    void opSmoothUnion(SDFModule* node);
    void opSmoothSubstraction(SDFModule* node);
    void opSmoothIntersection(SDFModule* node);

    void addCapsule(SDFModule* node);
    void addArrow(SDFModule* node);
    void addCross(SDFModule* node);
    void addRing(SDFModule* node);
    void addPartRingApprox(SDFModule* node);
    void addPartRing(SDFModule* node);
    void addQuestionMark(SDFModule* node);

    void AddTransform(SDFModule* node);
    void DeleteTransform(SDFModule* node);
    void PostOrder(SDFModule* node);
    void PraseNode(SDFModule* node);
    void DeletePostOrder(SDFModule* node);
    void DeleteNode(SDFModule* node);

    float UpdatePara(std::vector<std::pair<float, float>> para, float time);
    std::array<SDFPraseFunc, static_cast<int>(SDF_TYPE::END)> m_sdfPraseLUT = 
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
    bool m_shapeSet[static_cast<int>(SDF_TYPE::END)] = {false};
    
    std::stack<std::string> m_transfrom;
    std::stack<int> m_transfromNums;
    std::string m_name = "anonymous";
    std::string m_shader = "";
    std::string m_functionShader = R"(
    vec2 rotate(vec2 p, in float a)
    {
        float c = cos(a);
        float s = sin(a);
        mat2 rot = mat2(c,s,-s,c);
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
    std::string m_paraShader = R"(uniform float width;
    uniform float fillcolpara1;
    uniform float fillcolpara2;
    uniform float fillcolpara3;
    uniform float strokecolpara1;
    uniform float strokecolpara2;
    uniform float strokecolpara3;
    )";
    std::string m_shapeShader = "";
    float m_timePoint = 0.0;
    bool isPrimitive = true;
    int m_shapeCount = 0;
    int m_paraCount = 0;
    int m_transCount = 0;
    int m_transParaCount = 0;
    SDF_TYPE m_type;
    std::vector<float> m_color = {0.80, 0.80, 0.80, 0.80, 0.80, 0.80};
    std::vector<float> m_params;
    std::vector<std::vector<std::pair<float, float>>> m_animateParams;
    std::vector<float> m_transParams;
    std::vector<std::vector<std::pair<float, float>>> m_animateTransParams;
    //Matrix4 m_transfrom;
    
    SDFModule* m_rootNode = nullptr;
    SDFModule** m_rootPtr = &m_rootNode;
    std::vector<SDFShape*> m_shapes;

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