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
 
#include "ge_water_ripple_filter.h"
 
#include "ge_log.h"
#include "chrono"
 
namespace OHOS {
namespace Rosen {
// Advanced Filter
#define PROPERTY_HIGPU_VERSION "const.gpu.vendor"
#define PROPERTY_DEBUG_SUPPORT_AF "persist.sys.graphic.supports_af"
namespace {
 
static std::shared_ptr<Drawing::RuntimeEffect> g_waterRippleEffect = nullptr;
 
 
} // namespace
 
 
GEWaterRippleFilter::GEWaterRippleFilter(const Drawing::GEWaterRippleFilterParams& params)
    : progress_(params.progress), waveNum_(params.waveNum), rippleCenterX_(params.rippleCenterX), 
    rippleCenterY_(params.rippleCenterY)
{
    if (!InitWaterRippleEffect()) {
        LOGE("GEWaterRippleFilter::GEWaterRippleFilter failed when initializing RingEffect.");
        return;
    }
}
 
std::shared_ptr<Drawing::Image> GEWaterRippleFilter::ProcessImage(Drawing::Canvas& canvas,
    const std::shared_ptr<Drawing::Image> image, const Drawing::Rect& src, const Drawing::Rect& dst)
{
    if (image == nullptr) {
        LOGE("input is invalid");
        return nullptr;
    }
 
    Drawing::Matrix matrix;
    auto shader = Drawing::ShaderEffect::CreateImageShader(*image, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), matrix);
    if (shader == nullptr) {
        LOGI("shader create failed");
        return nullptr;
    }
    if (g_waterRippleEffect == nullptr) {
        if (InitWaterRippleEffect() == false) {
            LOGE("g_waterRippleEffect init failed");
            return nullptr;
        }
        LOGE("g_waterRippleEffect first time create");
    }
    auto imageInfo = image->GetImageInfo();
    float height = imageInfo.GetHeight();
    float width = imageInfo.GetWidth();
    LOGD("input image heigth = %{public}f, width = %{public}f", height, width);
    Drawing::RuntimeShaderBuilder builder(g_waterRippleEffect);
 
    builder.SetChild("image", shader);
    builder.SetUniform("iResolution", width, height);
    builder.SetUniform("progress", progress_);               
    builder.SetUniform("waveNum", waveNum_);       
    builder.SetUniform("rippleCenter", rippleCenterX_, rippleCenterY_);   
 
    auto invertedImage = builder.MakeImage(canvas.GetGPUContext().get(), nullptr, imageInfo, false);
    if (invertedImage == nullptr) {
        LOGE("make image failed");
        return nullptr;
    }
    return invertedImage;
}
 
bool GEWaterRippleFilter::InitWaterRippleEffect()
{
 
    static std::string blurString(R"(
        uniform shader image;
        uniform half2 iResolution;
        uniform half progress;
        uniform half waveNum;
        uniform half2 rippleCenter;
 
        const half basicSlope = 0.5;
        const half gAmplSupress = 0.003;
        const half waveFreq = 31.0;
        const half wavePropRatio = 2.0;
        const half ampSupArea = 0.2;
        const half intensity = 0.15;
 
        half calcWave(half dis)
        {  	
            half preWave = (waveNum == 1.) ? 1. : smoothstep(0., -0.3, dis);
            half waveForm = (waveNum == 1.) ? smoothstep(-0.4, -0.2, dis) * smoothstep(0., -0.2, dis) : (waveNum == 2.) ? smoothstep(-0.6, -0.3, dis) * preWave : smoothstep(-0.9, -0.6, dis) * step(abs(dis + 0.45), 0.45) * preWave;
            return sin(waveFreq * dis) * waveForm;
        }
 
        half waveGenerator(half propDis, half t)
        {   
            half dis = propDis - wavePropRatio * t;
            half h = 1e-3;
            half d1 = dis - h;
            half d2 = dis + h; 
            return (calcWave(d2) - calcWave(d1)) / (2. * h);
        }
 
        half4 main(vec2 fragCoord)
        {
            half shortEdge = min(iResolution.x, iResolution.y);
            half2 uv = fragCoord.xy / iResolution.xy;
            half2 uvHomo = fragCoord.xy / shortEdge;
            half2 resRatio = iResolution.xy / shortEdge;
 
            half progSlope = basicSlope + 0.1 * waveNum;
            half t = progSlope * progress;
            half ampDecayByT = pow((1. - t), 5.);
            half layerSup = 0.8 + (0.2 - 0.5*t) * step(t, 0.4);
 
            half2 waveCenter = rippleCenter * resRatio;
            half propDis = distance(uvHomo, waveCenter);
            half2 v = uvHomo - waveCenter;
            
            half ampSupByDis = smoothstep(0., ampSupArea, propDis);
            half2 circles = normalize(v) * (waveGenerator(propDis, t) * ampDecayByT * ampSupByDis * gAmplSupress);
            half3 norm = vec3(circles, sqrt(1. - dot(circles, circles)));
            
            half2 expandUV = (uv - intensity * norm.xy) * iResolution.xy;
            half3 color = image.eval(expandUV).rgb * layerSup;
            color += 6. * pow(dot(norm.yz, vec2(0.894427, 0.4472)), 7.);
            
            return half4(color, 1.0);
        }
    )");
 
 
    if (g_waterRippleEffect == nullptr) {
        LOGD("g_waterRippleEffect is null,first call");
        g_waterRippleEffect = Drawing::RuntimeEffect::CreateForShader(blurString);
        LOGD("g_waterRippleEffect maked");
        if (g_waterRippleEffect == nullptr) {
            LOGE("g_waterRippleEffect create failed");
            return false;
        }
    }
    return true;
}
 
} // namespace Rosen
} // namespace OHOS