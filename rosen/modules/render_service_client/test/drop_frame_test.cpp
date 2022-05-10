/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.. All rights reserved.
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

#include <iostream>
#include <surface.h>

#include "command/rs_base_node_command.h"
#include "command/rs_display_node_command.h"
#include "command/rs_surface_node_command.h"
#include "common/rs_common_def.h"
#include "display_type.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkImageInfo.h"
#include "pipeline/rs_render_result.h"
#include "pipeline/rs_render_thread.h"
#include "ui/rs_canvas_node.h"
#include "ui/rs_surface_extractor.h"
#include "ui/rs_ui_director.h"
#include "transaction/rs_interfaces.h"
#include "ui/rs_display_node.h"
#include "ui/rs_surface_node.h"
#include "render_context/render_context.h"

using namespace OHOS;
using namespace OHOS::Rosen;
using namespace std;

constexpr int MICRO_SECS_PER_SECOND = 1000000; // 1s = 1000000 us
constexpr int SLEEP_TIME = 5000; // 5000 us
constexpr int TEST_TIME = MICRO_SECS_PER_SECOND * 10; // test duration 10 s
constexpr int SKSCALAR_W = 500; //rect width 1400
constexpr int SKSCALAR_H = 300; //rect height 1200

namespace OHOS::Rosen {
#ifdef ACE_ENABLE_GPU
    RenderContext* rc_ = nullptr;
#endif

namespace pipelineTestUtils {
    constexpr bool wrongExit = false;
    constexpr bool successExit = false;

    class ToDrawSurface {
    public:
        using drawFun = std::function<void(SkCanvas&, SkPaint&)>;
        ToDrawSurface()
        {
            // Do not hold it. Use it As ToDrawSurface::Sample().
        };

        inline ToDrawSurface& SetSurfaceNode(std::shared_ptr<RSSurfaceNode> &surfaceNode)
        {
            surfaceNode_ = surfaceNode;
            return *this;
        }

        inline ToDrawSurface& SetSurfaceNodeSize(SkRect surfaceGeometry)
        {
            surfaceGeometry_ = surfaceGeometry;
            return *this;
        }

        inline ToDrawSurface& SetBufferSizeAuto()
        {
            bufferSize_ = surfaceGeometry_;
            return *this;
        }

        inline ToDrawSurface& SetDraw(drawFun drawShape)
        {
            drawShape_ = drawShape;
            return *this;
        }

        bool DrawSurface()
        {
            std::cout("ToDrawSurface::DrawSurface() start\n");
            if (surfaceNode_ == nullptr) {
                std::cout("ToDrawSurface: surfaceNode_ is nullptr\n");
                return false;
            }
            auto x = surfaceGeometry_.x();
            auto y = surfaceGeometry_.y();
            auto width = surfaceGeometry_.width();
            auto height = surfaceGeometry_.height();
            surfaceNode_->SetBounds(x, y, width, height);
            std::shared_ptr<RSSurface> rsSurface = RSSurfaceExtractor::ExtractRSSurface(surfaceNode_);
            if (rsSurface == nullptr) {
                return wrongExit;
            }
#ifdef ACE_ENABLE_GPU
            // SetRenderContext must before rsSurface->RequestFrame, or it will failed.
            if (rc_) {
                rsSurface->SetRenderContext(rc_);
            } else {
                std::cout("ToDrawSurface: RenderContext is nullptr\n");
            }
#endif
            auto framePtr = rsSurface->RequestFrame(bufferSize_.width(), bufferSize_.height());
            if (!framePtr) {
                // SetRenderContext must before rsSurface->RequestFrame,
                //      or frameptr will be nullptr.
                std::cout("ToDrawSurface: frameptr is nullptr\n");
                return wrongExit;
            }
            auto canvas = framePtr->GetCanvas();
            if (!canvas) {
                std::cout("ToDrawSurface: canvas is nullptr\n");
                return wrongExit;
            }
            canvas->clear(SK_ColorTRANSPARENT);
            SkPaint paint;
            paint.setAntiAlias(true);
            paint.setStyle(SkPaint::kFill_Style);
            paint.setStrokeWidth(20); // 20  stroke width
            paint.setStrokeJoin(SkPaint::kRound_Join);
            paint.setColor(color_);
            ChangeColor();
            if (!drawShape_) {
                std::cout("ToDrawSurface: drawShape_ is nullptr\n");
                return wrongExit;
            }
            drawShape_(*(canvas), paint);
            framePtr->SetDamageRegion(0, 0, surfaceGeometry_.width(), surfaceGeometry_.height());
            rsSurface->FlushFrame(framePtr);
            return successExit;
            std::cout("ToDrawSurface::DrawSurface() end\n");
        }

    private:
        void ChangeColor()
        {
            color_ = color_ == 0xff00ffff ? 0x61ff00ff : 0xff00ffff;
            return;
        }

        SkRect surfaceGeometry_ = {0.f, 0.f, 0.f, 0.f};
        SkRect bufferSize_ = {0.f, 0.f, 0.f, 0.f};
        drawFun drawShape_;
        uint32_t color_ = 0xff00ffff;
        std::shared_ptr<RSSurfaceNode> surfaceNode_ = nullptr;
    };

    static std::shared_ptr<RSSurfaceNode> CreateSurface()
    {
        RSSurfaceNodeConfig config;
        return RSSurfaceNode::Create(config);
    }
} // namespace pipelineTestUtils

class RSDropFrameTestCase {
public:
    inline static RSDropFrameTestCase& GetInstance()
    {
        static RSDropFrameTestCase c;
        return c;
    }

    void TestInit()
    {
        std::cout << "-------------------------------------------------------\n";
        std::cout << "RSDropFrameTestCase:: rs drop_frame_tets.cpp testInit Start\n";

#ifdef ACE_ENABLE_GPU
        std::cout << "ACE_ENABLE_GPU is enabled\n";
        isGPU_ = true;
#else
        std::cout << "ACE_ENABLE_GPU is disabled\n";
        isGPU_ = false;
#endif
        ScreenId id = RSInterfaces::GetInstance().GetDefaultScreenId();
        std::cout << "RS default screen id is " << id << ".\n";
        isInit_ = true;
        RenderContextInit();
        std::cout << "RSDropFrameTestCase:: rs drop_frame_tets.cpp testInit end\n";
        std::cout << "-------------------------------------------------------\n";
        return;
    }

    void TestCaseDefault()
    {
        std::cout << "-------------------------------------------------------\n";
        std::cout << "RSDropFrameTestCase:: rs drop_frame_tets.cpp testCase Start\n";
        if (!isInit_) {
            std::cout << "RSDropFrameTestCase:: TestInit failed testCase Stop\n";
            return;
        }
        auto surfaceNode1 = pipelineTestUtils::CreateSurface();
        auto drawEr = pipelineTestUtils::ToDrawSurface()
            .SetSurfaceNode(surfaceNode1)
            .SetSurfaceNodeSize(SkRect::MakeXYWH(0, 0, SKSCALAR_W, SKSCALAR_H))
            .SetBufferSizeAuto()
            .SetDraw([&](SkCanvas &canvas, SkPaint &paint) -> void {
                canvas.drawRect(
                    SkRect::MakeXYWH(0, 0, SKSCALAR_W, SKSCALAR_H),
                    paint);
            });
        drawEr.DrawSurface();
        RSDisplayNodeConfig config;
        RSDisplayNode::SharedPtr displayNode = RSDisplayNode::Create(config);
        displayNode->AddChild(surfaceNode1, -1);

        auto transactionProxy = RSTransactionProxy::GetInstance();
        if (transactionProxy != nullptr) {
            transactionProxy->FlushImplicitTransaction();
        }
        for (int index = 0; index <= TEST_TIME / SLEEP_TIME; index ++) {
            drawEr.DrawSurface();
            if (transactionProxy != nullptr) {
                transactionProxy->FlushImplicitTransaction();
            }
            usleep(SLEEP_TIME);
        }
        
        std::cout << "RSDropFrameTestCase:: rs drop_frame_tets.cpp testCaseDefault end\n";
        std::cout << "-------------------------------------------------------\n";
    }
private:
    RSDropFrameTestCase() = default;
    void RenderContextInit()
    {
#ifdef ACE_ENABLE_GPU
        std::cout << "ACE_ENABLE_GPU is true. \n";
        std::cout << "Init RenderContext start. \n";
        rc_ = RenderContextFactory::GetInstance().CreateEngine();
        if (rc_) {
            std::cout << "Init RenderContext success.\n";
            rc_->InitializeEglContext();
            isInit_ = true;
        } else {
            isInit_ = false;
            std::cout << "Init RenderContext failed, RenderContext is nullptr.\n";
        }
        std::cout << "Init RenderContext start.\n";
#endif
    }

    bool isInit_ = false;
    bool isGPU_ = false;
}; // class RSDropFrameTestCase
} // namespace OHOS::Rosen

int main()
{
    RSDropFrameTestCase::GetInstance().TestInit();
    RSDropFrameTestCase::GetInstance().TestCaseDefault();
    return 0;
}
