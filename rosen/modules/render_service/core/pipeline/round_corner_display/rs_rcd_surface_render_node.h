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

#ifndef RENDER_SERVICE_CORE_RS_RCD_SURFACE_RENDER_NODE_H
#define RENDER_SERVICE_CORE_RS_RCD_SURFACE_RENDER_NODE_H

#include <ibuffer_consumer_listener.h>
#include <memory>
#include <surface.h>

#include "common/rs_rect.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_surface_handler.h"
#include "pipeline/round_corner_display/rs_round_corner_config.h"
#ifdef NEW_RENDER_CONTEXT
#include "rs_render_surface.h"
#include "render_context_base.h"
#else
#include "platform/drawing/rs_surface.h"
#include "render_context/render_context.h"
#endif
#include "sync_fence.h"
#include <filesystem>
#include <SkBitmap.h>



namespace OHOS {
namespace Rosen {


enum class RCDSurfaceType : uint32_t {
    BOTTOM,
    TOP,
    INVALID
};

struct RcdExtInfo {
    bool surfaceCreated = false;
    RectI srcRect_;
    RectI dstRect_;

    RectF surfaceBounds;
    RectF frameBounds;
    RectF frameViewPort;

    RCDSurfaceType surfaceType = RCDSurfaceType::INVALID;

    void Clear()
    {
        surfaceBounds.Clear();
        frameBounds.Clear();
        frameViewPort.Clear();
    }

    float GetFrameOffsetX() const
    {
        return frameViewPort.GetLeft() - surfaceBounds.GetLeft();
    }

    float GetFrameOffsetY() const
    {
        return frameViewPort.GetTop() - surfaceBounds.GetTop();
    }
};

struct RcdSourceInfo {
    uint32_t bufferWidth = 0;
    uint32_t bufferHeight = 0;
    uint32_t bufferSize = 0;
};

struct HardwareLayerInfo {
    std::filesystem::path pathBin;
    int bufferSize;
    int cldWidth;
    int cldHeight;
};

class RSRcdSurfaceRenderNode : public RSRenderNode, public RSSurfaceHandler {
public:
    using WeakPtr = std::weak_ptr<RSRcdSurfaceRenderNode>;
    using SharedPtr = std::shared_ptr<RSRcdSurfaceRenderNode>;

    RSRcdSurfaceRenderNode(NodeId id, RCDSurfaceType type, const std::weak_ptr<RSContext>& context = {});
    ~RSRcdSurfaceRenderNode() override;

    const RectI& GetSrcRect() const;
    const RectI& GetDstRect() const;

    bool CreateSurface(sptr<IBufferConsumerListener> listener);
    bool IsSurfaceCreated() const;
#ifdef NEW_RENDER_CONTEXT
    std::shared_ptr<RSRenderSurface> GetRSSurface() const;
#else
    std::shared_ptr<RSSurface> GetRSSurface() const;
#endif
    sptr<IBufferConsumerListener> GetConsumerListener() const;
    RcdSourceInfo rcdSourceInfo;
    void SetRcdBufferWidth(uint32_t width);
    void SetRcdBufferHeight(uint32_t height);
    void SetRcdBufferSize(uint32_t buffersize);
    BufferRequestConfig GetHardenBufferRequestConfig() const;
    void PrepareHardwareResourceBuffer(rs_rcd::RoundCornerLayer* layerInfo);
    bool SetHardwareResourceToBuffer();

    bool FillHardwareResource(HardwareLayerInfo &cldLayerInfo, int height, int width, int stride, uint8_t *img);

    SkBitmap layerBitmap;
    HardwareLayerInfo cldLayerInfo;

    void ClearBufferCache();

    void ResetCurrFrameState();
    void Reset();

    bool IsBottomSurface() const;
    bool IsTopSurface() const;
    bool IsInvalidSurface() const;

    float GetFrameOffsetX() const;
    float GetFrameOffsetY() const;

private:
    float GetSurfaceWidth() const;
    float GetSurfaceHeight() const;

    uint32_t GetRcdBufferWidth() const;
    uint32_t GetRcdBufferHeight() const;
    uint32_t GetRcdBufferSize() const;

#ifdef NEW_RENDER_CONTEXT
    std::shared_ptr<RSRenderSurface> surface_;
#else
    std::shared_ptr<RSSurface> surface_;
#endif
    sptr<IBufferConsumerListener> consumerListener_;

    RcdExtInfo rcdExtInfo_;
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_CORE_RS_RCD_SURFACE_RENDER_NODE_H