/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
// temporary debug
#include "foundation/graphic/standard/rosen/modules/render_service_base/src/platform/ohos/rs_surface_frame_ohos.h"
#include "foundation/graphic/standard/rosen/modules/render_service_base/src/platform/ohos/rs_surface_ohos.h"

using namespace OHOS;
using namespace OHOS::Rosen;
using namespace std;

namespace OHOS::Rosen {
#ifdef ACE_ENABLE_GPU
    RenderContext* rc_ = nullptr;
#endif

namespace pipelineTestUtils {
    constexpr bool wrongExit = false;
    constexpr bool successExit = false;
    struct Circle {
        Circle(float x, float y, float r)
            :x_(x), y_(y), r_(r) {};
        float x_ = 0.0f;
        float y_ = 0.0f;
        float r_ = 0.0f;
    }; // class Circle

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

        inline ToDrawSurface& SetShapeColor(uint32_t color)
        {
            color_ = color;
            return *this;
        }

        inline ToDrawSurface& SetDraw(drawFun drawShape)
        {
            drawShape_ = drawShape;
            return *this;
        }

        bool Run()
        {
            printf("ToDrawSurface::Run() start\n");
            if (surfaceNode_ == nullptr) {
                printf("DrawSurface: surfaceNode_ is nullptr\n");
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
                printf("DrawSurface: RenderContext is nullptr\n");
            }
#endif
            auto framePtr = rsSurface->RequestFrame(bufferSize_.width(), bufferSize_.height());
            if (!framePtr) {
                // SetRenderContext must before rsSurface->RequestFrame,
                //      or frameptr will be nullptr.
                printf("DrawSurface: frameptr is nullptr\n");
                return wrongExit;
            }
            auto canvas = framePtr->GetCanvas();
            if (!canvas) {
                printf("DrawSurface: canvas is nullptr\n");
                return wrongExit;
            }
            canvas->clear(SK_ColorTRANSPARENT);
            SkPaint paint;
            paint.setAntiAlias(true);
            paint.setStyle(SkPaint::kFill_Style);
            paint.setStrokeWidth(20);
            paint.setStrokeJoin(SkPaint::kRound_Join);
            paint.setColor(color_);
            if (!drawShape_) {
                printf("DrawSurface: drawShape_ is nullptr\n");
                return wrongExit;
            }
            drawShape_(*(canvas), paint);
            framePtr->SetDamageRegion(0, 0, surfaceGeometry_.width(), surfaceGeometry_.height());
            rsSurface->FlushFrame(framePtr);
            return successExit;
            printf("ToDrawSurface::Run() end\n");
        }
    private:
        static void Sample()
        {
            auto surfaceNode = RSSurfaceNode::Create(RSSurfaceNodeConfig());
            pipelineTestUtils::ToDrawSurface()
                .SetSurfaceNode(surfaceNode)
                .SetShapeColor(0xff00ffff)
                .SetSurfaceNodeSize(SkRect::MakeXYWH(80, 500, 500, 300))
                .SetBufferSizeAuto()
                .SetDraw([&](SkCanvas &canvas, SkPaint &paint) -> void {
                    canvas.drawRect(
                        SkRect::MakeXYWH(0, 0, 500, 300),
                        paint);
                })
                .Run();
        }
        SkRect surfaceGeometry_;
        SkRect bufferSize_;
        drawFun drawShape_;
        uint32_t color_ = 0;
        std::shared_ptr<RSSurfaceNode> surfaceNode_;
    }; // class ToDrawSurface

    static std::shared_ptr<RSSurfaceNode> CreateSurface()
    {
        RSSurfaceNodeConfig config;
        return RSSurfaceNode::Create(config);
    }
} // namespace pipelineTestUtils

// Toy DMS.
using DisplayId = ScreenId;
class DmsMock {
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
        rsInterface_.SetScreenChangeCallback([this](ScreenId id, ScreenEvent event) {
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
}; // class DmsMock

class RSDemoTestCase {
public:
    inline static RSDemoTestCase& GetInstance()
    {
        static RSDemoTestCase c;
        return c;
    }

    void Testhello() const
    {
        std::cout << "Render service Client rs Demo.cpp Start\n";
        std::cout << "If you want to get more information, \n";
        std::cout << "please type 'hilog | grep Rs' in another hdc shell window\n";
        std::cout << "-------------------------------------------------------\n";
    }

    void TestInit()
    {
        isInit_ = true;
        std::cout << "-------------------------------------------------------\n";
        std::cout << "Render service Client rs Demo.cpp testInit Start\n";

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
            screenFreshRate_ = activeModeInfo->GetScreenFreshRate();
            std::cout << "Display " << id << " active mode info:\n";
            std::cout << "Width: " << screenWidth_ << ", Height: " << screenheight_;
            std::cout << ", FreshRate: " << screenFreshRate_ << "Hz.\n";
        } else {
            std::cout << "Display " << id << " has no active mode!\n";
        }
        RenderContextInit();
        std::cout << "Render service Client rs Demo.cpp testInit end\n";
        std::cout << "-------------------------------------------------------\n";
        return;
    }

    void TestCaseDefault()
    {
        std::cout << "-------------------------------------------------------\n";
        std::cout << "Render service Client rs Demo.cpp testCaseDefault Start\n";
        auto surfaceNode1 = pipelineTestUtils::CreateSurface();
        auto surfaceNode2 = pipelineTestUtils::CreateSurface();

        pipelineTestUtils::ToDrawSurface()
            .SetSurfaceNode(surfaceNode1)
            .SetShapeColor(0xff00ffff)
            .SetSurfaceNodeSize(SkRect::MakeXYWH(screenWidth_ * 0.3f, screenheight_ * 0.3f, screenWidth_ * 0.2f, screenheight_ * 0.2f))
            .SetBufferSizeAuto()
            .SetDraw([&](SkCanvas &canvas, SkPaint &paint) -> void {
                canvas.drawRect(
                    SkRect::MakeXYWH(0, 0, screenWidth_ * 0.2f, screenheight_ * 0.2f),
                    paint);
            })
            .Run();
        pipelineTestUtils::ToDrawSurface()
            .SetSurfaceNode(surfaceNode2)
            .SetShapeColor(0xffff0000)
            .SetSurfaceNodeSize(SkRect::MakeXYWH(screenWidth_ * 0.6f, screenheight_ * 0.6f, screenWidth_ * 0.2f, screenheight_ * 0.2f))
            .SetBufferSizeAuto()
            .SetDraw([&](SkCanvas &canvas, SkPaint &paint) -> void {
                canvas.drawRect(
                    SkRect::MakeXYWH(0, 0, screenWidth_ * 0.2f, screenheight_ * 0.2f),
                    paint);
            })
            .Run();

        RSDisplayNodeConfig config;
        RSDisplayNode::SharedPtr displayNode = RSDisplayNode::Create(config);
        displayNode->AddChild(surfaceNode1, -1);
        displayNode->AddChild(surfaceNode2, -1);
        std::cout << "Hardware Processer test start\n";
        auto transactionProxy = RSTransactionProxy::GetInstance();
        if (transactionProxy != nullptr) {
            transactionProxy->FlushImplicitTransaction();
        }
        for (float alpha = 0; alpha <= 1.f; alpha += 0.2f) {
            printf("printf alpha=%f \n", alpha);
            surfaceNode2->SetAlpha(alpha);
            if (transactionProxy != nullptr) {
                transactionProxy->FlushImplicitTransaction();
            }
            sleep(1);
        }
        for (float scale = 0; scale < 2.f; scale += 0.2f) {
            printf("scale=%f\n", scale);
            surfaceNode2->SetScaleX(scale);
            if (transactionProxy != nullptr) {
                transactionProxy->FlushImplicitTransaction();
            }
            sleep(1);
        }
        surfaceNode2->SetScaleX(1.f);
        std::cout << "Hardware Processer test end\n";
        std::cout << "Compatible Processer test start\n";
        for (float rotate = 0; rotate <= 180.f; rotate += 15) {
            displayNode->RemoveChild(surfaceNode2);
            if (transactionProxy != nullptr) {
                transactionProxy->FlushImplicitTransaction();
            }
            displayNode->AddChild(surfaceNode2, -1);
            pipelineTestUtils::ToDrawSurface()
                .SetSurfaceNode(surfaceNode2)
                .SetShapeColor(0xffff0000)
                .SetSurfaceNodeSize(SkRect::MakeXYWH(screenWidth_ * 0.6f, screenheight_ * 0.6f, screenWidth_ * 0.2f, screenheight_ * 0.2f))
                .SetBufferSizeAuto()
                .SetDraw([&](SkCanvas &canvas, SkPaint &paint) -> void {
                    canvas.drawRect(
                        SkRect::MakeXYWH(0, 0, screenWidth_ * 0.2f, screenheight_ * 0.2f),
                        paint);
                })
            .Run();
            printf("roate=%f\n", rotate);
            surfaceNode2->SetRotation(rotate);
            if (transactionProxy != nullptr) {
                transactionProxy->FlushImplicitTransaction();
            }
            sleep(1);
        }
        surfaceNode2->SetRotation(180.f);
        for (float alpha = 0; alpha <= 1.f; alpha += 0.2f) {
            displayNode->RemoveChild(surfaceNode2);
            if (transactionProxy != nullptr) {
                transactionProxy->FlushImplicitTransaction();
            }
            displayNode->AddChild(surfaceNode2, -1);
            pipelineTestUtils::ToDrawSurface()
                .SetSurfaceNode(surfaceNode2)
                .SetShapeColor(0xffff0000)
                .SetSurfaceNodeSize(SkRect::MakeXYWH(screenWidth_ * 0.6f, screenheight_ * 0.6f, screenWidth_ * 0.2f, screenheight_ * 0.2f))
                .SetBufferSizeAuto()
                .SetDraw([&](SkCanvas &canvas, SkPaint &paint) -> void {
                    canvas.drawRect(
                        SkRect::MakeXYWH(0, 0, screenWidth_ * 0.2f, screenheight_ * 0.2f),
                        paint);
                })
            .Run();
            printf("printf alpha=%f \n", alpha);
            surfaceNode2->SetAlpha(alpha);
            if (transactionProxy != nullptr) {
                transactionProxy->FlushImplicitTransaction();
            }
            sleep(1);
        }
        for (float scale = 0; scale < 2.f; scale += 0.2f) {
            displayNode->RemoveChild(surfaceNode2);
            if (transactionProxy != nullptr) {
                transactionProxy->FlushImplicitTransaction();
            }
            displayNode->AddChild(surfaceNode2, -1);
            pipelineTestUtils::ToDrawSurface()
                .SetSurfaceNode(surfaceNode2)
                .SetShapeColor(0xffff0000)
                .SetSurfaceNodeSize(SkRect::MakeXYWH(screenWidth_ * 0.6f, screenheight_ * 0.6f, screenWidth_ * 0.2f, screenheight_ * 0.2f))
                .SetBufferSizeAuto()
                .SetDraw([&](SkCanvas &canvas, SkPaint &paint) -> void {
                    canvas.drawRect(
                        SkRect::MakeXYWH(0, 0, screenWidth_ * 0.2f, screenheight_ * 0.2f),
                        paint);
                })
            .Run();
            printf("scale=%f\n", scale);
            surfaceNode2->SetScaleX(scale);
            if (transactionProxy != nullptr) {
                transactionProxy->FlushImplicitTransaction();
            }
            sleep(1);
        }
        std::cout << "Compatible Processer test end\n";
        displayNode->RemoveFromTree();
        if (transactionProxy != nullptr) {
            transactionProxy->FlushImplicitTransaction();
        }
        std::cout << "Render service Client rs Demo.cpp testCaseDefault end\n";
        std::cout << "-------------------------------------------------------\n";
    }
private:
    RSDemoTestCase() = default;
    void RenderContextInit()
    {
#ifdef ACE_ENABLE_GPU
        std::cout << "ACE_ENABLE_GPU is true. \n";
        std::cout << "Init RenderContext start. \n";
            rc_ = RenderContextFactory::GetInstance().CreateEngine();
            if (rc_) {
                std::cout << "Init RenderContext success.\n";
                rc_->InitializeEglContext();
            } else {
                std::cout << "Init RenderContext failed, RenderContext is nullptr.\n";
            }
        std::cout << "Init RenderContext start.\n";
#endif
    }

    bool isInit_ = false;
    bool isGPU_ = false;
    int screenWidth_ = 0;
    int screenheight_ = 0;
    int screenFreshRate_ = 0;
}; // class RSDemoTestCase
} // namespace OHOS::Rosen

int main()
{
    RSDemoTestCase::GetInstance().Testhello();
    RSDemoTestCase::GetInstance().TestInit();
    RSDemoTestCase::GetInstance().TestCaseDefault();
    return 0;
}
