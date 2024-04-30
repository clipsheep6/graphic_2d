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

#ifndef RENDER_SERVICE_BASE_DRAWABLE_RS_PROPERTY_DRAWABLE_UTILS_H
#define RENDER_SERVICE_BASE_DRAWABLE_RS_PROPERTY_DRAWABLE_UTILS_H

#include "property/rs_properties.h"

namespace OHOS {
namespace Rosen {
class RSPropertyDrawableUtils {
public:
    static Drawing::RoundRect RRect2DrawingRRect(const RRect& rr);
    static Drawing::Rect Rect2DrawingRect(const RectF& r);
    static RRect GetRRectForDrawingBorder(
        const RSProperties& properties, const std::shared_ptr<RSBorder>& border, const bool& isOutline);
    static RRect GetInnerRRectForDrawingBorder(
        const RSProperties& properties, const std::shared_ptr<RSBorder>& border, const bool& isOutline);
    static bool PickColor(Drawing::Canvas& canvas, const std::shared_ptr<RSColorPickerCacheTask>& colorPickerTask,
        Drawing::Path& drPath, Drawing::Matrix& matrix, RSColor& colorPicked);
    static void GetDarkColor(RSColor& color);
    static void CeilMatrixTrans(Drawing::Canvas* canvas);
    static void BeginForegroundFilter(RSPaintFilterCanvas& canvas, const RectF& bounds);
    static void DrawForegroundFilter(RSPaintFilterCanvas& canvas, const std::shared_ptr<RSFilter>& rsFilter);
    static void DrawFilter(Drawing::Canvas* canvas, const std::shared_ptr<RSFilter>& rsFilter,
        const std::unique_ptr<RSFilterCacheManager>& cacheManager, const bool isForegroundFilter,
        bool shouldClearFilteredCache);
    static void DrawBackgroundEffect(RSPaintFilterCanvas* canvas, const std::shared_ptr<RSFilter>& rsFilter,
        const std::unique_ptr<RSFilterCacheManager>& cacheManager, bool shouldClearFilteredCache);
    static void DrawColorFilter(Drawing::Canvas* canvas, const std::shared_ptr<Drawing::ColorFilter>& colorFilter);
    static void DrawLightUpEffect(Drawing::Canvas* canvas, const float lightUpEffectDegree);
    static void DrawDynamicDim(Drawing::Canvas* canvas, const float dynamicDimDegree);
    static std::shared_ptr<Drawing::ShaderEffect> MakeDynamicDimShader(float dynamicDimDeg,
        std::shared_ptr<Drawing::ShaderEffect> imageShader);
    static std::shared_ptr<Drawing::ShaderEffect> MakeBinarizationShader(float low, float high, float thresholdLow,
        float thresholdHigh, std::shared_ptr<Drawing::ShaderEffect> imageShader);
    static std::shared_ptr<Drawing::RuntimeBlenderBuilder> MakeDynamicFgBrightnessBuilder();
    static std::shared_ptr<Drawing::RuntimeBlenderBuilder> MakeDynamicBgBrightnessBuilder();
    static std::shared_ptr<Drawing::Blender> MakeDynamicBrightnessBlender(
            const RSDynamicBrightnessPara& params, const float fract, bool foreground = true);
    static void DrawBinarization(Drawing::Canvas* canvas, const std::optional<Vector4f>& aiInvert);
    static void DrawPixelStretch(Drawing::Canvas* canvas, const std::optional<Vector4f>& pixelStretch,
        const RectF& boundsRect, const bool boundsGeoValid);
    static Drawing::Path CreateShadowPath(const std::shared_ptr<RSPath> rsPath,
        const std::shared_ptr<RSPath>& clipBounds, const RRect& rrect);
    static void DrawShadow(Drawing::Canvas* canvas, Drawing::Path& path, const float& offsetX, const float& offsetY,
        const float& elevation, const bool& isFilled, Color spotColor);
    static void DrawUseEffect(RSPaintFilterCanvas* canvas);

    static bool IsDangerousBlendMode(int blendMode, int blendApplyType);
    static void BeginBlender(RSPaintFilterCanvas& canvas, std::shared_ptr<Drawing::Blender> blender,
        int blendModeApplyType, bool isDangerous);
    static void EndBlender(RSPaintFilterCanvas& canvas, int blendModeApplyType);

    static Color CalculateInvertColor(const Color& backgroundColor);
    static Color GetInvertBackgroundColor(RSPaintFilterCanvas& canvas, bool needClipToBounds,
        const Vector4f& boundsRect, const Color& backgroundColor);
    RSB_EXPORT static int GetAndResetBlurCnt();

private:
    static std::shared_ptr<Drawing::RuntimeEffect> binarizationShaderEffect_;
    static std::shared_ptr<Drawing::RuntimeEffect> dynamicDimShaderEffect_;
    static std::shared_ptr<Drawing::RuntimeEffect> dynamicFgBrightnessBlenderEffect_;
    static std::shared_ptr<Drawing::RuntimeEffect> dynamicBgBrightnessBlenderEffect_;
    inline static int g_blurCnt = 0;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_DRAWABLE_RS_PROPERTY_DRAWABLE_UTILS_H
