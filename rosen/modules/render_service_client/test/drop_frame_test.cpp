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
constexpr int SKSCALAR_W = 500; // rect width 500
constexpr int SKSCALAR_H = 300; // rect height 300

namespace OHOS::Rosen {
#ifdef ACE_ENABLE_GPU
    RenderContext* rc_ = nullptr;
#endif


template <typename Duration>
using SysTime = std::chrono::time_point<std::chrono::system_clock, Duration>;
using SysMicroSeconds = SysTime<std::chrono::microseconds>;
using SysMiliSeconds = SysTime<std::chrono::milliseconds>;

uint64_t MicroSecondsSinceEpoch()
{
    SysMicroSeconds tmp = std::chrono::system_clock::now();
    return tmp.time_since_epoch().count();
}

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

        inline ToDrawSurface& SetBounds()
        {
            RS_LOGI("drop_frame_test::ToDrawSurface::SetBounds()");
            //printf("%llu ToDrawSurface::SetBounds()\n ",MiliSecondsSinceEpoch());
            // std::cout << "ToDrawSurface::DrawSurface() start\n";
            if (surfaceNode_ == nullptr) {
                RS_LOGI("drop_frame_test::ToDrawSurface::SetBounds() surfaceNode_ is nullptr");
                std::cout << "ToDrawSurface:SetBounds() surfaceNode_ is nullptr\n";
                return *this;
            }
            auto x = surfaceGeometry_.x();
            auto y = surfaceGeometry_.y();
            auto width = surfaceGeometry_.width();
            auto height = surfaceGeometry_.height();
            surfaceNode_->SetBounds(x, y, width, height);
            return *this;
        }

        inline ToDrawSurface& InitRsSurface()
        {
            if (surfaceNode_ == nullptr) {
                RS_LOGI("drop_frame_test::ToDrawSurface::InitRsSurface surfaceNode_ is nullptr");
                std::cout << "ToDrawSurface: surfaceNode_ is nullptr\n";
                return *this;
            }
            std::shared_ptr<RSSurface> rsSurface_ = RSSurfaceExtractor::ExtractRSSurface(surfaceNode_);
            if (rsSurface_ == nullptr) {
                RS_LOGI("drop_frame_test::ToDrawSurface::InitRsSurface ExtractRSSurface() failed");
                std::cout << "drop_frame_test::ToDrawSurface::InitRsSurface ExtractRSSurface() failed";
                return *this;
            }
            RS_LOGI("drop_frame_test::ToDrawSurface::InitRsSurface surface queue size is %d",surfaceNode_->GetSurface()->GetQueueSize());
            printf("drop_frame_test::ToDrawSurface::InitRsSurface surface queue size is %d",surfaceNode_->GetSurface()->GetQueueSize());
            
#ifdef ACE_ENABLE_GPU
            // SetRenderContext must before rsSurface->RequestFrame, or it will failed.
            if (rc_) {
                rsSurface_->SetRenderContext(rc_);
                RS_LOGI("drop_frame_test::ToDrawSurface::InitRsSurface SetRenderContext() success");
                printf("drop_frame_test::ToDrawSurface::InitRsSurface SetRenderContext() success");
            } else {
                //std::cout << "ToDrawSurface: RenderContext is nullptr\n";
                RS_LOGI("drop_frame_test::ToDrawSurface::InitRsSurface RenderContext is nullptr");
                printf("drop_frame_test::ToDrawSurface::InitRsSurface RenderContext is nullptr");
            }
#endif
            return *this;
        }

         bool DrawSurface()
        {
            if (surfaceNode_ == nullptr) {
                RS_LOGI("drop_frame_test::ToDrawSurface::DrawSurface surfaceNode_ is nullptr");
                std::cout << "ToDrawSurface: surfaceNode_ is nullptr\n";
                return wrongExit;
            }
            std::shared_ptr<RSSurface> rsSurface_ = RSSurfaceExtractor::ExtractRSSurface(surfaceNode_);
            if (rsSurface_ == nullptr) {
                RS_LOGI("drop_frame_test::ToDrawSurface::DrawSurface ExtractRSSurface() failed");
                std::cout << "drop_frame_test::ToDrawSurface::DrawSurface ExtractRSSurface() failed";
                return wrongExit;
            }
            RS_LOGI("drop_frame_test::ToDrawSurface::DrawSurface surface queue size is %d",surfaceNode_->GetSurface()->GetQueueSize());
            printf("drop_frame_test::ToDrawSurface::DrawSurface surface queue size is %d",surfaceNode_->GetSurface()->GetQueueSize());
            
#ifdef ACE_ENABLE_GPU
            // SetRenderContext must before rsSurface->RequestFrame, or it will failed.
            if (rc_) {
                rsSurface_->SetRenderContext(rc_);
                RS_LOGI("drop_frame_test::ToDrawSurface::DrawSurface SetRenderContext() success");
                printf("drop_frame_test::ToDrawSurface::DrawSurface SetRenderContext() success");
            } else {
                //std::cout << "ToDrawSurface: RenderContext is nullptr\n";
                RS_LOGI("drop_frame_test::ToDrawSurface::DrawSurface RenderContext is nullptr");
                printf("drop_frame_test::ToDrawSurface::DrawSurface RenderContext is nullptr");
                return wrongExit;
            }
#endif


            auto framePtr_ = rsSurface_->RequestFrame(bufferSize_.width(), bufferSize_.height());
            if (!framePtr_) {
                // SetRenderContext must before rsSurface->RequestFrame,
                //      or frameptr will be nullptr.
                RS_LOGI("drop_frame_test::ToDrawSurface::DrawSurface frameptr is nullptr");
                std::cout << "ToDrawSurface: frameptr is nullptr\n";
                return wrongExit;
            }
            //printf("%llu ToDrawSurface::RequestFrame() finish\n ",MicroSecondsSinceEpoch());
            RS_LOGI("drop_frame_test::ToDrawSurface::RequestFrame() finish");
            auto canvas = framePtr_->GetCanvas();
            if (!canvas) {
                std::cout << "ToDrawSurface: canvas is nullptr\n";
                RS_LOGI("drop_frame_test::ToDrawSurface:: canvas is nullptr ");
                return wrongExit;
            }
            SkPaint backPaint;
            backPaint.setColor(color_);
            canvas->drawRect(SkRect::MakeXYWH(0.0, 0.0, SKSCALAR_W, SKSCALAR_H), backPaint);
            RS_LOGI("drop_frame_test::ToDrawSurface:: drawRect() finish ");
            printf("%llu ToDrawSurface::drawShape_() finish\n ",MicroSecondsSinceEpoch());
            //framePtr->SetDamageRegion(0, 0, surfaceGeometry_.width(), surfaceGeometry_.height());
            //RS_LOGI("drop_frame_test::ToDrawSurface:: SetDamageRegion() finish ");
            //printf("%llu ToDrawSurface::SetDamageRegion() finish\n ",MiliSecondsSinceEpoch());
            rsSurface_->FlushFrame(framePtr_);
            RS_LOGI("drop_frame_test::ToDrawSurface:: FlushFrame finish ");
            printf("%llu ToDrawSurface::DrawSurface() end\n ",MicroSecondsSinceEpoch());
            //std::cout << "ToDrawSurface::DrawSurface() end\n";
            return successExit;
        }

    private:
        void ChangeColor()
        {
            color_ = color_ == 0xff00ffff ? 0x61ff00ff : 0xff00ffff;
            RS_LOGI("drop_frame_test::ToDrawSurface:: ChangeColor() is %u  ",color_);
            //printf("%llu ToDrawSurface::DrawSurface() color is %u\n ",MicroSecondsSinceEpoch(),color_);
            // std::cout << "ToDrawSurface::color is " << color_ << " \n";
            return;
        }

        SkRect surfaceGeometry_ = {0.f, 0.f, 0.f, 0.f};
        SkRect bufferSize_ = {0.f, 0.f, 0.f, 0.f};
        drawFun drawShape_;
        uint32_t color_ = 0xff00ffff;
        std::shared_ptr<RSSurfaceNode> surfaceNode_ = nullptr;
        std::shared_ptr<OHOS::Rosen::RSSurface> rsSurface_ = nullptr;
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
            .SetBounds()
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
            // if (transactionProxy != nullptr) {
            //     transactionProxy->FlushImplicitTransaction();
            // }
            //usleep(SLEEP_TIME);
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
