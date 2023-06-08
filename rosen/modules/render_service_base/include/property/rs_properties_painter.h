/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_CLIENT_CORE_PROPERTY_RS_PROPERTIES_PAINTER_H
#define RENDER_SERVICE_CLIENT_CORE_PROPERTY_RS_PROPERTIES_PAINTER_H

#include "common/rs_macros.h"
#include "property/rs_properties.h"
#ifndef USE_ROSEN_DRAWING
#include "pipeline/rs_draw_cmd_list.h"
#else
#include "recording/draw_cmd_list.h"
#include "draw/surface.h"
#endif

namespace OHOS {
namespace Rosen {
#ifndef USE_ROSEN_DRAWING
class RSSkiaFilter;
#else
class RSDrawingFilter;
#endif
class RSPaintFilterCanvas;

class RSB_EXPORT RSPropertiesPainter {
public:
#ifndef USE_ROSEN_DRAWING
    static void Clip(SkCanvas& canvas, RectF rect, bool isAntiAlias = true);
    static void SetBgAntiAlias(bool forceBgAntiAlias);
    static bool GetBgAntiAlias();
    static void DrawBackground(const RSProperties& properties, RSPaintFilterCanvas& canvas, bool isAntiAlias = true);
    static void DrawBorder(const RSProperties& properties, SkCanvas& canvas);
    static void DrawFrame(const RSProperties& properties, RSPaintFilterCanvas& canvas, DrawCmdListPtr& drawCmdList);
    static void GetShadowDirtyRect(RectI& dirtyShadow, const RSProperties& properties,
        const RRect* rrect = nullptr, bool isAbsCoordinate = true);
    static void DrawShadow(const RSProperties& properties, RSPaintFilterCanvas& canvas, const RRect* rrect = nullptr);
    static void DrawFilter(const RSProperties& properties, RSPaintFilterCanvas& canvas,
        std::shared_ptr<RSSkiaFilter>& filter, const std::unique_ptr<SkRect>& rect = nullptr,
        SkSurface* sKSurface = nullptr);
    static void DrawLinearGradientBlurFilter(const RSProperties& properties,
                                RSPaintFilterCanvas& canvas, const std::unique_ptr<SkRect>& rect);
    static void DrawForegroundColor(const RSProperties& properties, SkCanvas& canvas);
    static void DrawMask(const RSProperties& properties, SkCanvas& canvas);
    static void DrawMask(const RSProperties& properties, SkCanvas& canvas, SkRect maskBounds);
    static bool GetGravityMatrix(Gravity gravity, RectF rect, float w, float h, SkMatrix& mat);
    static SkRRect RRect2SkRRect(const RRect& rr);
    static SkRect Rect2SkRect(const RectF& r);
    static int GetAndResetBlurCnt();
    static SkColor CalcAverageColor(sk_sp<SkImage> imageSnapshot);

    static void DrawPixelStretch(const RSProperties& properties, RSPaintFilterCanvas& canvas);
    // functions that are dedicated to driven render [start]
    static RectF GetCmdsClipRect(DrawCmdListPtr& cmds);
    static void DrawFrameForDriven(const RSProperties& properties, RSPaintFilterCanvas& canvas, DrawCmdListPtr& cmds);
    // functions that are dedicated to driven render [end]
    static void DrawSpherize(const RSProperties& properties, RSPaintFilterCanvas& canvas,
        const sk_sp<SkSurface>& spherizeSurface);

    static void DrawColorFilter(const RSProperties& properties, SkCanvas* canvas);
private:
    inline static int g_blurCnt = 0;
    static void DrawColorfulShadowInner(const RSProperties& properties, RSPaintFilterCanvas& canvas, SkPath& path);
    static void DrawShadowInner(const RSProperties& properties, RSPaintFilterCanvas& canvas, SkPath& path);
#ifdef NEW_SKIA
    static sk_sp<SkShader> MakeAlphaGradientShader(const SkRect clipBounds,
                                            const std::shared_ptr<RSLinearGradientBlurPara> para);
    static sk_sp<SkShader> MakeHorizontalMeanBlurShader(float radiusIn,
                                            sk_sp<SkShader> shader, sk_sp<SkShader> gradientShader);
    static sk_sp<SkShader>MakeVerticalMeanBlurShader(float radiusIn,
                                            sk_sp<SkShader> shader, sk_sp<SkShader> gradientShader);
#endif
#else
    static void Clip(Drawing::Canvas& canvas, RectF rect);
    static void SetBgAntiAlias(bool forceBgAntiAlias);
    static bool GetBgAntiAlias();
    static void DrawBackground(const RSProperties& properties, RSPaintFilterCanvas& canvas);
    static void DrawBorder(const RSProperties& properties, Drawing::Canvas& canvas);
    static void DrawFrame(const RSProperties& properties, RSPaintFilterCanvas& canvas,
        Drawing::DrawCmdListPtr& drawCmdList);
    static void GetShadowDirtyRect(RectI& dirtyShadow, const RSProperties& properties, const RRect* rrect = nullptr);
    static void DrawShadow(const RSProperties& properties, RSPaintFilterCanvas& canvas, const RRect* rrect = nullptr);
    static void DrawFilter(const RSProperties& properties, RSPaintFilterCanvas& canvas,
        std::shared_ptr<RSDrawingFilter>& filter, const std::unique_ptr<Drawing::Rect>& rect = nullptr,
        Drawing::Surface* surface = nullptr);
    static void DrawForegroundColor(const RSProperties& properties, Drawing::Canvas& canvas);
    static void DrawMask(const RSProperties& properties, Drawing::Canvas& canvas);
    static void DrawMask(const RSProperties& properties, Drawing::Canvas& canvas, Drawing::Rect maskBounds);
    static bool GetGravityMatrix(Gravity gravity, RectF rect, float w, float h, Drawing::Matrix& mat);
    static Drawing::RoundRect RRect2DrawingRRect(const RRect& rr);
    static Drawing::Rect Rect2DrawingRect(const RectF& r);
    static int GetAndResetBlurCnt();
    static Drawing::ColorQuad CalcAverageColor(std::shared_ptr<Drawing::Image> imageSnapshot);

    static void DrawPixelStretch(const RSProperties& properties, RSPaintFilterCanvas& canvas);
    // functions that are dedicated to driven render [start]
    static RectF GetCmdsClipRect(Drawing::DrawCmdListPtr& cmds);
    static void DrawFrameForDriven(const RSProperties& properties, RSPaintFilterCanvas& canvas,
        Drawing::DrawCmdListPtr& cmds);
    // functions that are dedicated to driven render [end]
    static void DrawSpherize(const RSProperties& properties, RSPaintFilterCanvas& canvas,
        const std::shared_ptr<Drawing::Surface>& spherizeSurface);
private:
    inline static int g_blurCnt = 0;
    static void DrawColorfulShadowInner(
        const RSProperties& properties, RSPaintFilterCanvas& canvas, Drawing::Path& path);
    static void DrawShadowInner(const RSProperties& properties, RSPaintFilterCanvas& canvas, Drawing::Path& path);
#endif
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_PROPERTY_RS_PROPERTIES_PAINTER_H
