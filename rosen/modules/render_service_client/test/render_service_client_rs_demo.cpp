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

std::unique_ptr<RSSurfaceFrame> framePtr;
#ifdef ACE_ENABLE_GPU
RenderContext* rc_ = nullptr;
#endif
namespace
{
    constexpr bool wrongExit = false;
    constexpr bool successExit = false;
    struct Circle
    {
        Circle(float x, float y, float r)
         :x_(x), y_(y), r_(r) {};
        float x_ = 0.0f;
        float y_ = 0.0f;
        float r_ = 0.0f;
    }; //
} // anonymous namespace

bool DrawSurface(
    SkRect surfaceGeometry, uint32_t color, SkRect shapeGeometry, std::shared_ptr<RSSurfaceNode> surfaceNode)
{
    auto x = surfaceGeometry.x();
    auto y = surfaceGeometry.y();
    auto width = surfaceGeometry.width();
    auto height = surfaceGeometry.height();
    surfaceNode->SetBounds(x, y, width, height);
    std::shared_ptr<RSSurface> rsSurface = RSSurfaceExtractor::ExtractRSSurface(surfaceNode);
    if (rsSurface == nullptr) {
        return wrongExit;
    }
#ifdef ACE_ENABLE_GPU
    if (rc_) {
        rsSurface->SetRenderContext(rc_);
    }
#endif
    auto frame = rsSurface->RequestFrame(width, height);
    framePtr = std::move(frame);
    if (!framePtr) {
        printf("DrawSurface frameptr is nullptr");
        return wrongExit;
    }
    auto canvas = framePtr->GetCanvas();
    if (!canvas) {
        printf("DrawSurface canvas is nullptr");
        return wrongExit;
    }
    canvas->clear(SK_ColorTRANSPARENT);
    SkPaint paint;
    paint.setAntiAlias(true);
    paint.setStyle(SkPaint::kFill_Style);
    paint.setStrokeWidth(20);
    paint.setStrokeJoin(SkPaint::kRound_Join);
    paint.setColor(color);

    canvas->drawRect(shapeGeometry, paint);
    framePtr->SetDamageRegion(0, 0, width, height);
    auto framePtr1 = std::move(framePtr);
    rsSurface->FlushFrame(framePtr1);
    return successExit;
}

bool DrawSurfaceCircle(
    SkRect surfaceGeometry, float bufferW, float bufferH, uint32_t color, Circle circle, std::shared_ptr<RSSurfaceNode> surfaceNode)
{
    auto x = surfaceGeometry.x();
    auto y = surfaceGeometry.y();
    auto width = surfaceGeometry.width();
    auto height = surfaceGeometry.height();
    printf("[%f %f %f %f]\n", x, y, width, height);
    printf("[%f %f %f]\n", circle.x_, circle.y_, circle.r_);
    surfaceNode->SetBounds(x, y, width, height);
    std::shared_ptr<RSSurface> rsSurface = RSSurfaceExtractor::ExtractRSSurface(surfaceNode);
    if (rsSurface == nullptr) {
        return wrongExit;
    }
#ifdef ACE_ENABLE_GPU
    if (rc_) {
        rsSurface->SetRenderContext(rc_);
    }
#endif
    auto frame = rsSurface->RequestFrame(bufferW, bufferH);
    framePtr = std::move(frame);
    if (!framePtr) {
        printf("DrawSurface frameptr is nullptr");
        return wrongExit;
    }
    auto canvas = framePtr->GetCanvas();
    if (!canvas) {
        printf("DrawSurface canvas is nullptr");
        return wrongExit;
    }
    canvas->clear(SK_ColorTRANSPARENT);
    SkPaint paint;
    paint.setAntiAlias(true);
    paint.setStyle(SkPaint::kFill_Style);
    paint.setStrokeWidth(20);
    paint.setStrokeJoin(SkPaint::kRound_Join);
    paint.setColor(color);

    canvas->drawCircle(circle.x_, circle.y_, circle.r_, paint);
    framePtr->SetDamageRegion(0, 0, width, height);
    auto framePtr1 = std::move(framePtr);
    rsSurface->FlushFrame(framePtr1);
    return successExit;
}

std::shared_ptr<RSSurfaceNode> CreateSurface()
{
    RSSurfaceNodeConfig config;
    return RSSurfaceNode::Create(config);
}

// Toy DMS.
using DisplayId = ScreenId;
class MyDMS {
private:
    struct Display {
        DisplayId id;
        RSScreenModeInfo activeMode;
    };
    std::unordered_map<DisplayId, Display> displays_;
    mutable std::recursive_mutex mutex_;
    RSInterfaces& rsInterface_;
    DisplayId defaultDisplayId_;

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
    MyDMS() : rsInterface_(RSInterfaces::GetInstance())
    {
        Init();
    }
    ~MyDMS() noexcept = default;

    DisplayId GetDefaultDisplayId()
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        return defaultDisplayId_;
    }

    std::optional<RSScreenModeInfo> GetDisplayActiveMode(DisplayId id) const
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        if (displays_.count(id) == 0) {
            std::cout << "MyDMS: No display " << id << "!" << std::endl;
            return {};
        }
        return displays_.at(id).activeMode;
    }

    void OnDisplayConnected(ScreenId id)
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        displays_[id] = Display { id, rsInterface_.GetScreenActiveMode(id) };
        std::cout << "MyDMS: Display " << id << " connected." << std::endl;
    }

    void OnDisplayDisConnected(ScreenId id)
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        if (displays_.count(id) == 0) {
            std::cout << "MyDMS: No display " << id << "!" << std::endl;
        } else {
            std::cout << "MyDMS: Display " << id << " disconnected." << std::endl;
            displays_.erase(id);
            if (id == defaultDisplayId_) {
                defaultDisplayId_ = rsInterface_.GetDefaultScreenId();
                std::cout << "MyDMS: DefaultDisplayId changed, new DefaultDisplayId is" << defaultDisplayId_ << "."
                          << std::endl;
            }
        }
    }
};
MyDMS g_dms;

int main()
{
    std::cout << "Render service Client rs Demo.cpp start\n";
#ifdef ACE_ENABLE_GPU
    std::cout << "ace_enable_gpu is enabled\n";
#else
    std::cout << "ace_enable_gpu is disabled\n";
#endif
    DisplayId id = g_dms.GetDefaultDisplayId();
    std::cout << "RS default screen id is " << id << ".\n";

    auto activeModeInfo = g_dms.GetDisplayActiveMode(id);
    int screenWidth = 0;
    int screenheight = 0;
    if (activeModeInfo) {
        screenWidth = activeModeInfo->GetScreenWidth();
        screenheight = activeModeInfo->GetScreenHeight();
        std::cout << "Display " << id << " active mode info:\n";
        std::cout << "Width: " << screenWidth << ", Height: " << screenheight;
        std::cout << ", FreshRate: " << activeModeInfo->GetScreenFreshRate() << "Hz.\n";
    } else {
        std::cout << "Display " << id << " has no active mode!\n";
        return 0;
    }
    std::cout << "-------------------------------------------------------\n";

    auto surfaceLauncher = CreateSurface();
    auto surfaceNode1 = CreateSurface();
    surfaceNode1->SetAlpha(0.5);
    auto surfaceNode2 = CreateSurface();
    surfaceNode2->SetAlpha(0.3);
#ifdef ACE_ENABLE_GPU
    rc_ = RenderContextFactory::GetInstance().CreateEngine();
    if (rc_) {
        rc_->InitializeEglContext();
    }
#endif
    DrawSurface(SkRect::MakeXYWH(0, 0, screenWidth, screenheight), 
        0xffffe4c4, SkRect::MakeXYWH(0, 0, screenWidth, screenheight), surfaceLauncher);

    DrawSurfaceCircle(SkRect::MakeXYWH(0, 0, screenWidth * 0.2f, screenWidth * 0.2f), screenWidth * 0.3f, screenWidth * 0.3f, 0xffff0000,
        Circle(screenWidth * 0.15f, screenWidth * 0.15f, screenWidth * 0.12f), surfaceNode1);

    DrawSurfaceCircle(SkRect::MakeXYWH(screenWidth * 0.2f, screenWidth * 0.2f, screenWidth * 0.2f, screenWidth * 0.2f), screenWidth * 0.15f, screenWidth * 0.15f, 0xff3f0033,
        Circle(screenWidth * 0.075f, screenWidth * 0.075f, screenWidth * 0.075f), surfaceNode2);
    RSDisplayNodeConfig config;
    RSDisplayNode::SharedPtr displayNode = RSDisplayNode::Create(config);
    displayNode->AddChild(surfaceLauncher, -1);
    displayNode->AddChild(surfaceNode1, -1);
    displayNode->AddChild(surfaceNode2, -1);
    auto transactionProxy = RSTransactionProxy::GetInstance();
    std::cout << "Render service Client rs Demo.cpp wait for 100s\n";
    sleep(100);
    if (transactionProxy != nullptr) {
        transactionProxy->FlushImplicitTransaction();
    }
    int positionX = 80;
    int positionY = 80;
    while (1) {
        displayNode->RemoveChild(surfaceNode1);
        if (transactionProxy != nullptr) {
            transactionProxy->FlushImplicitTransaction();
        }
        sleep(4);
        if (positionY == 80 && (positionX >= 80 && positionX <= 2160)) {
            positionX += 80;
        } else if ((positionX == 2240) && (positionY >= 80 && positionY <= 1200)) {
            positionY += 80;
        } else if ((positionX >= 160 && positionX <= 2240) && (positionY == 1280)) {
            positionX -= 80;
        } else if ((positionX == 80) && (positionY >= 160 && positionY <= 1280)) {
            positionY -= 80;
        }
        displayNode->AddChild(surfaceNode1, -1);
        DrawSurface(SkRect::MakeXYWH(positionX, positionY, 200, 200), 0xffff0000, SkRect::MakeXYWH(0, 0, 200, 200),
            surfaceNode1);
        auto transactionProxy = RSTransactionProxy::GetInstance();
        if (transactionProxy != nullptr) {
            transactionProxy->FlushImplicitTransaction();
        }
        sleep(4);
    }
    std::cout << "Render service Client rs Demo.cpp end, test success\n";
    return 0;
}
