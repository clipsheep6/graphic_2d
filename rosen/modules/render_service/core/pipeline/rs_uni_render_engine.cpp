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

#include "pipeline/rs_uni_render_engine.h"
#include "pipeline/rs_uni_render_util.h"
#include "pipeline/round_corner_display/rs_round_corner_display.h"
#include "common/rs_singleton.h"
#ifdef USE_VIDEO_PROCESSING_ENGINE
#include "metadata_helper.h"
#endif

namespace OHOS {
namespace Rosen {
void RSUniRenderEngine::DrawSurfaceNodeWithParams(RSPaintFilterCanvas& canvas, RSSurfaceRenderNode& node,
    BufferDrawParam& params, PreProcessFunc preProcess, PostProcessFunc postProcess)
{
    canvas.Save();
    canvas.ConcatMatrix(params.matrix);
    if (!params.useCPU) {
        RegisterDeleteBufferListener(node.GetConsumer());
        RegisterDeleteBufferListener(node);
        DrawImage(canvas, params);
    } else {
        DrawBuffer(canvas, params);
    }
    canvas.Restore();
}

#ifdef USE_VIDEO_PROCESSING_ENGINE
GraphicColorGamut RSUniRenderEngine::ComputeTargetColorGamut(const std::vector<LayerInfoPtr>& layers)
{
    using namespace HDI::Display::Graphic::Common::V1_0;
    GraphicColorGamut colorGamut = GRAPHIC_COLOR_GAMUT_SRGB;
    for (auto& layer : layers) {
        auto buffer = layer->GetBuffer();
        if (buffer == nullptr) {
            RS_LOGW("RSHardwareThread::ComputeTargetColorGamut The buffer of layer is nullptr");
            continue;
        }
        CM_ColorSpaceInfo colorSpaceInfo;
        if (MetadataHelper::GetColorSpaceInfo(buffer, colorSpaceInfo) != GSERROR_OK) {
            RS_LOGD("RSHardwareThread::ComputeTargetColorGamut Get color space failed");
            continue;
        }
        if (colorSpaceInfo.primaries != COLORPRIMARIES_SRGB) {
            colorGamut = GRAPHIC_COLOR_GAMUT_DISPLAY_P3;
            break;
        }
    }
    return colorGamut;
}
#endif

void RSUniRenderEngine::DrawLayers(RSPaintFilterCanvas& canvas, const std::vector<LayerInfoPtr>& layers, bool forceCPU,
    const ScreenInfo& screenInfo)
{
    bool isTopGpuDraw = false;
    bool isBottomGpuDraw = false;
    GraphicColorGamut colorGamut = ComputeTargetColorGamut(layers);
    for (const auto& layer : layers) {
        if (layer == nullptr) {
            continue;
        }
        if (layer->GetCompositionType() == GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE ||
            layer->GetCompositionType() == GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE_CLEAR) {
            continue;
        }
        if (layer->GetSurface()->GetName() == "RCDTopSurfaceNode") {
            isTopGpuDraw = true;
            continue;
        }
        if (layer->GetSurface()->GetName() == "RCDBottomSurfaceNode") {
            isBottomGpuDraw = true;
            continue;
        }
        Drawing::AutoCanvasRestore acr(canvas, true);
        auto dstRect = layer->GetLayerSize();
        Drawing::Rect clipRect = Drawing::Rect(static_cast<float>(dstRect.x), static_cast<float>(dstRect.y),
            static_cast<float>(dstRect.w) + static_cast<float>(dstRect.x),
            static_cast<float>(dstRect.h) + static_cast<float>(dstRect.y));
        canvas.ClipRect(clipRect, Drawing::ClipOp::INTERSECT, false);
        // prepare BufferDrawParam
        auto params = RSUniRenderUtil::CreateLayerBufferDrawParam(layer, forceCPU);
        params.matrix.PostScale(screenInfo.GetRogWidthRatio(), screenInfo.GetRogHeightRatio());
        params.screenId = screenInfo.id;
        params.targetColorGamut = colorGamut;
        auto screenManager = CreateOrGetScreenManager();
        params.screenBrightnessNits = screenManager->GetScreenBrightnessNits(params.screenId);
        DrawHdiLayerWithParams(canvas, layer, params);
        // Dfx for redraw region
        if (RSSystemProperties::GetHwcRegionDfxEnabled()) {
            RectI dst(dstRect.x, dstRect.y, dstRect.w, dstRect.h);
            RSUniRenderUtil::DrawRectForDfx(canvas, dst, Drawing::Color::COLOR_YELLOW, 0.4f,
                layer->GetSurface()->GetName());
        }
    }

    if (isTopGpuDraw && RSSingleton<RoundCornerDisplay>::GetInstance().GetRcdEnable()) {
        RSSingleton<RoundCornerDisplay>::GetInstance().DrawTopRoundCorner(&canvas);
    }

    if (isBottomGpuDraw && RSSingleton<RoundCornerDisplay>::GetInstance().GetRcdEnable()) {
        RSSingleton<RoundCornerDisplay>::GetInstance().DrawBottomRoundCorner(&canvas);
    }
}

void RSUniRenderEngine::DrawHdiLayerWithParams(RSPaintFilterCanvas& canvas, const LayerInfoPtr& layer,
    BufferDrawParam& params)
{
    canvas.ConcatMatrix(params.matrix);
    if (!params.useCPU) {
        RegisterDeleteBufferListener(layer->GetSurface(), true);
        DrawImage(canvas, params);
    } else {
        DrawBuffer(canvas, params);
    }
}
} // namespace Rosen
} // namespace OHOS
