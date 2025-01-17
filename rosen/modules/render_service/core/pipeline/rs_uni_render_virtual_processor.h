/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef RS_CORE_PIPELINE_UNI_RENDER_MIRROR_PROCESSOR_H
#define RS_CORE_PIPELINE_UNI_RENDER_MIRROR_PROCESSOR_H

#include "rs_uni_render_processor.h"

namespace OHOS {
namespace Rosen {
constexpr uint32_t ROI_REGIONS_MAX_CNT = 8;
struct RoiRegionInfo {
    uint32_t startX = 0;
    uint32_t startY = 0;
    uint32_t width = 0;
    uint32_t height = 0;
};

struct RoiRegions {
    uint32_t regionCnt = 0;
    RoiRegionInfo regions[ROI_REGIONS_MAX_CNT];
};

class RSUniRenderVirtualProcessor : public RSUniRenderProcessor {
public:
    static inline constexpr RSProcessorType Type = RSProcessorType::UNIRENDER_VIRTUAL_PROCESSOR;
    RSProcessorType GetType() const override
    {
        return Type;
    }

    RSUniRenderVirtualProcessor() = default;
    ~RSUniRenderVirtualProcessor() noexcept override = default;

    bool InitForRenderThread(DrawableV2::RSDisplayRenderNodeDrawable& displayDrawable, ScreenId mirroredId,
        std::shared_ptr<RSBaseRenderEngine> renderEngine) override;
    void ProcessDisplaySurfaceForRenderThread(DrawableV2::RSDisplayRenderNodeDrawable& displayDrawable) override;
    void ProcessSurface(RSSurfaceRenderNode& node) override;
    void ProcessRcdSurface(RSRcdSurfaceRenderNode& node) override;
    void PostProcess() override;
    void ScaleMirrorIfNeed(RSDisplayRenderNode& node, RSPaintFilterCanvas& canvas);
    void Fill(RSPaintFilterCanvas& canvas,
        float mainWidth, float mainHeight, float mirrorWidth, float mirrorHeight);
    void UniScale(RSPaintFilterCanvas& canvas,
        float mainWidth, float mainHeight, float mirrorWidth, float mirrorHeight);

    std::unique_ptr<RSPaintFilterCanvas> GetCanvas()
    {
        return std::move(canvas_);
    }
    float GetMirrorScaleX() const
    {
        return mirrorScaleX_;
    }
    float GetMirrorScaleY() const
    {
        return mirrorScaleY_;
    }
    const Drawing::Matrix& GetCanvasMatrix() const
    {
        return canvasMatrix_;
    }
    void SetDirtyInfo(std::vector<RectI>& damageRegion);
    int32_t GetBufferAge() const;
    // when virtual screen partial refresh closed, use this function to reset RoiRegion in buffer
    GSError SetRoiRegionToCodec(std::vector<RectI>& damageRegion);
    bool RequestVirtualFrame(DrawableV2::RSDisplayRenderNodeDrawable& displayDrawable);
    void CalculateTransform(RSDisplayRenderNode& node);
    void CalculateTransform(DrawableV2::RSDisplayRenderNodeDrawable& displayDrawable);
    void ScaleMirrorIfNeed(const ScreenRotation angle, RSPaintFilterCanvas& canvas);
    void ProcessVirtualDisplaySurface(DrawableV2::RSDisplayRenderNodeDrawable& displayDrawable);
private:
    void CanvasInit(DrawableV2::RSDisplayRenderNodeDrawable& displayDrawable);
    void CanvasInit(RSDisplayRenderNode& node);
    void OriginScreenRotation(ScreenRotation screenRotation, float width, float height);

    sptr<Surface> producerSurface_;
    std::unique_ptr<RSRenderFrame> renderFrame_;
    std::unique_ptr<RSPaintFilterCanvas> canvas_;
    bool forceCPU_ = false;
    bool isExpand_ = false;
    float mirrorWidth_ = 0.f;
    float mirrorHeight_ = 0.f;
    float mainWidth_ = 0.f;
    float mainHeight_ = 0.f;
    float virtualScreenWidth_ = 0.f;
    float virtualScreenHeight_ = 0.f;
    float mirroredScreenWidth_ = 0.f;
    float mirroredScreenHeight_ = 0.f;
    bool updateFlag_ = false;
    bool canvasRotation_ = false;
    ScreenScaleMode scaleMode_ = ScreenScaleMode::INVALID_MODE;
    ScreenRotation screenRotation_ = ScreenRotation::ROTATION_0;
    ScreenRotation screenCorrection_ = ScreenRotation::ROTATION_0;
    float mirrorScaleX_ = 1.0f;
    float mirrorScaleY_ = 1.0f;
    Drawing::Matrix canvasMatrix_;
    sptr<RSScreenManager> screenManager_ = nullptr;
    ScreenId virtualScreenId_ = INVALID_SCREEN_ID;
    ScreenId mirroredScreenId_ = INVALID_SCREEN_ID;
};
} // namespace Rosen
} // namespace OHOS
#endif // RS_CORE_PIPELINE_UNI_RENDER_MIRROR_PROCESSOR_H
