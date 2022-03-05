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

#ifndef PIPELINE_TEST_UTILS_H
#define PIPELINE_TEST_UTILS_H

#include <iostream>
#include <surface.h>

#include "command/rs_base_node_command.h"
#include "command/rs_display_node_command.h"
#include "command/rs_surface_node_command.h"
#include "common/rs_common_def.h"
#include "core/transaction/rs_interfaces.h"
#include "core/ui/rs_display_node.h"
#include "core/ui/rs_surface_node.h"
#include "display_type.h"
#include "pipeline/rs_render_result.h"
#include "pipeline/rs_render_thread.h"
#include "ui/rs_surface_extractor.h"
#include "ui/rs_ui_director.h"

#include "draw/canvas.h"
#include "draw/color.h"
#include "image/bitmap.h"
#include "utils/log.h"

namespace OHOS {
namespace Rosen {
namespace PipelineTestUtils {
class ToDrawSurface {
public:
    using TestFunc = std::function<void(Drawing::Canvas&, uint32_t, uint32_t)>;
    ToDrawSurface() {};
    virtual ~ToDrawSurface() {};

    inline ToDrawSurface& SetSurfaceNode(std::shared_ptr<RSSurfaceNode>& surfaceNode)
    {
        surfaceNode_ = surfaceNode;
        return *this;
    }

    inline ToDrawSurface& SetSurfaceNodeSize(int screenWidth, int screenHeight)
    {
        width_ = screenWidth;
        height_ = screenHeight;
        return *this;
    }

    inline ToDrawSurface& SetDraw(TestFunc drawFunc)
    {
        drawFunc_ = drawFunc;
        return *this;
    }

    void DoDraw(uint8_t* addr, uint32_t width, uint32_t height)
    {
        Drawing::Bitmap bitmap;
        Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_OPAQUYE };
        bitmap.Build(width, height, format);

        Drawing::Canvas canvas;
        canvas.Bind(bitmap);
        canvas.Clear(Drawing::Color::COLOR_WHITE);

        drawFunc_(canvas, width, height);

        constexpr uint32_t stride = 4;
        uint32_t addrSize = width * height * stride;
        auto ret = memcpy_s(addr, addrSize, bitmap.GetPixels(), addrSize);
        if (ret != EOK) {
            LOGI("memcpy_s failed");
        }
    }

    void DrawSurface(std::shared_ptr<RSSurfaceNode> surfaceNode, int32_t width, int32_t height)
    {
        sptr<Surface> surface = surfaceNode->GetSurface();
        if (surface == nullptr) {
            return;
        }

        sptr<SurfaceBuffer> buffer;
        int32_t releaseFence;
        BufferRequestConfig config = {
            .width = width,
            .height = height,
            .strideAlignment = 0x8,
            .format = PIXEL_FMT_RGBA_8888,
            .usage = HBM_USE_CPU_READ | HBM_USE_CPU_WRITE | HBM_USE_MEM_DMA,
        };

        SurfaceError ret = surface->RequestBuffer(buffer, releaseFence, config);
        LOGI("request buffer ret is: %{public}s", SurfaceErrorStr(ret).c_str());

        if (buffer == nullptr) {
            LOGE("request buffer failed: buffer is nullptr");
            return;
        }
        if (buffer->GetVirAddr() == nullptr) {
            LOGE("get virAddr failed: virAddr is nullptr");
            return;
        }

        auto addr = static_cast<uint8_t*>(buffer->GetVirAddr());
        LOGI("buffer width: %{public}d, height: %{public}d", buffer->GetWidth(), buffer->GetHeight());
        DoDraw(addr, buffer->GetWidth(), buffer->GetHeight());

        BufferFlushConfig flushConfig = {
          .damage = {
                  .w = buffer->GetWidth(),
                  .h = buffer->GetHeight(),
              },
        };
        ret = surface->FlushBuffer(buffer, -1, flushConfig);
        LOGI("flushBuffer ret is: %{public}s", SurfaceErrorStr(ret).c_str());
    }

    bool Run()
    {
        printf("ToDrawSurface::Run() start\n");

        if (surfaceNode_ == nullptr) {
            printf("DrawSurface: surfaceNode_ is nullptr\n");
            return false;
        }

        RSDisplayNodeConfig config;
        RSDisplayNode::SharedPtr displayNode = RSDisplayNode::Create(config);

        auto transactionProxy = RSTransactionProxy::GetInstance();
        if (transactionProxy == nullptr) {
            printf("DrawSurface: transactionProxy is nullptr\n");
            return false;
        }
        sleep(2); // wait 2s
        displayNode->AddChild(surfaceNode_, -1);
        surfaceNode_->SetBounds(0, 0, width_, height_);
        transactionProxy->FlushImplicitTransaction();
        DrawSurface(surfaceNode_, width_, height_);
        sleep(4); // wait 4s
        displayNode->RemoveChild(surfaceNode_);
        transactionProxy->FlushImplicitTransaction();
        printf("ToDrawSurface::Run() end\n");
        return true;
    }

    static __attribute__((used)) std::shared_ptr<RSSurfaceNode> CreateSurface()
    {
        RSSurfaceNodeConfig config;
        return RSSurfaceNode::Create(config);
    }

private:
    std::shared_ptr<RSSurfaceNode> surfaceNode_;
    int width_;
    int height_;
    TestFunc drawFunc_;
};
} // namespace PipelineTestUtils
} // namespace Rosen
} // namespace OHOS
#endif // PIPELINE_TEST_UTILS_H