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

        inline ToDrawSurface& SetBufferSize(int width, int height)
        {
            bufferSize_ = SkRect::MakeXYWH(0, 0, width, height);
            return *this;
        }

        inline ToDrawSurface& SetBufferSizeAuto()
        {
            bufferSize_ = surfaceGeometry_;
            return *this;
        }

        inline ToDrawSurface& SetBufferSize(SkRect bufferSize)
        {
            // bufferSize has no XY
            bufferSize_ = bufferSize;
            return *this;
        }

        inline ToDrawSurface& SetDraw(drawFun drawShape)
        {
            drawShape_ = drawShape;
            return *this;
        }

        bool DrawSurface()
        {
            printf("ToDrawSurface::DrawSurface() start\n");
            if (surfaceNode_ == nullptr) {
                printf("ToDrawSurface: surfaceNode_ is nullptr\n");
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
                printf("ToDrawSurface: RenderContext is nullptr\n");
            }
#endif
            auto framePtr = rsSurface->RequestFrame(bufferSize_.width(), bufferSize_.height());
            if (!framePtr) {
                // SetRenderContext must before rsSurface->RequestFrame,
                //      or frameptr will be nullptr.
                printf("ToDrawSurface: frameptr is nullptr\n");
                return wrongExit;
            }
            auto canvas = framePtr->GetCanvas();
            if (!canvas) {
                printf("ToDrawSurface: canvas is nullptr\n");
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
                printf("ToDrawSurface: drawShape_ is nullptr\n");
                return wrongExit;
            }
            drawShape_(*(canvas), paint);
            framePtr->SetDamageRegion(0, 0, surfaceGeometry_.width(), surfaceGeometry_.height());
            rsSurface->FlushFrame(framePtr);
            return successExit;
            printf("ToDrawSurface::DrawSurface() end\n");
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

// Toy DMS.
using DisplayId = ScreenId;
class DmsMock {
public:
    inline static DmsMock& GetInstance()
    {
        static DmsMock c;
        return c;
    }

    ~DmsMock() noexcept = default;

    DisplayId GetDefaultDisplayId()
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        return defaultDisplayId_;
    }

    std::optional<RSScreenModeInfo> GetDisplayActiveMode(DisplayId id) const
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        if (displays_.count(id) == 0) {
            cout << "DmsMock: No display " << id << "!" << endl;
            return {};
        }
        return displays_.at(id).activeMode;
    }

    void OnDisplayConnected(ScreenId id)
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        displays_[id] = Display { id, rsInterface_.GetScreenActiveMode(id) };
        std::cout << "DmsMock: Display " << id << " connected." << endl;
    }

    void OnDisplayDisConnected(ScreenId id)
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        if (displays_.count(id) == 0) {
            cout << "DmsMock: No display " << id << "!" << endl;
        } else {
            std::cout << "DmsMock: Display " << id << " disconnected." << endl;
            displays_.erase(id);
            if (id == defaultDisplayId_) {
                defaultDisplayId_ = rsInterface_.GetDefaultScreenId();
                std::cout << "DmsMock: DefaultDisplayId changed, new DefaultDisplayId is" << defaultDisplayId_ << "."
                          << endl;
            }
        }
    }

private:
    struct Display {
        DisplayId id;
        RSScreenModeInfo activeMode;
    };
    std::unordered_map<DisplayId, Display> displays_;
    mutable std::recursive_mutex mutex_;
    RSInterfaces& rsInterface_;
    DisplayId defaultDisplayId_;

    DmsMock() : rsInterface_(RSInterfaces::GetInstance())
    {
        Init();
    }

    void Init()
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        (void)rsInterface_.SetScreenChangeCallback([this](ScreenId id, ScreenEvent event) {
            switch (event) {
                case ScreenEvent::CONNECTED: {
                    this->OnDisplayConnected(id);
                    break;
                }
                case ScreenEvent::DISCONNECTED: {
                    this->OnDisplayDisConnected(id);
                    break;
                }
                default:
                    break;
            }
        });

        defaultDisplayId_ = rsInterface_.GetDefaultScreenId();
        displays_[defaultDisplayId_] =
            Display { defaultDisplayId_, rsInterface_.GetScreenActiveMode(defaultDisplayId_) };
    }
}; // class DmsMock


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

#ifdef ACE_ENABLE_GL
        std::cout << "ACE_ENABLE_GL is enabled\n";
#else
        std::cout << "ACE_ENABLE_GL is disabled\n";
#endif
        DisplayId id = DmsMock::GetInstance().GetDefaultDisplayId();
        std::cout << "RS default screen id is " << id << ".\n";
        auto activeModeInfo = DmsMock::GetInstance().GetDisplayActiveMode(id);
        if (activeModeInfo) {
            screenWidth_ = activeModeInfo->GetScreenWidth();
            screenheight_ = activeModeInfo->GetScreenHeight();
            screenRefreshRate_ = static_cast<int>(activeModeInfo->GetScreenRefreshRate());
            std::cout << "Display " << id << " active mode info:\n";
            std::cout << "Width: " << screenWidth_ << ", Height: " << screenheight_;
            std::cout << ", RefreshRate: " << screenRefreshRate_ << "Hz.\n";
            isInit_ = true;
        } else {
            std::cout << "Display " << id << " has no active mode!\n";
            isInit_ = false;
        }
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
            .SetSurfaceNodeSize(SkRect::MakeXYWH(screenWidth_ * 0.4f, screenheight_ * 0.4f, 500, 300))
            .SetBufferSizeAuto()
            .SetDraw([&](SkCanvas &canvas, SkPaint &paint) -> void {
                canvas.drawRect(
                    SkRect::MakeXYWH(0, 0, 500, 300),
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
        
        std::cout << "Compatible test end\n";
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
    int screenWidth_ = 0;
    int screenheight_ = 0;
    int screenRefreshRate_ = 0;
}; // class RSDropFrameTestCase
} // namespace OHOS::Rosen

int main()
{
    RSDropFrameTestCase::GetInstance().TestInit();
    RSDropFrameTestCase::GetInstance().TestCaseDefault();
    return 0;
}
