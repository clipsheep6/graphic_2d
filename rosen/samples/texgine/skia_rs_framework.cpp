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

#include "skia_framework.h"

#include <chrono>
#include <cmath>
#include <cstring>
#include <functional>
#include <iomanip>
#include <iostream>
#include <sstream>

#include <include/core/SkBitmap.h>
#include <include/core/SkCanvas.h>
#include <include/core/SkFontMetrics.h>
#include <include/core/SkFontMgr.h>
#include <include/core/SkPath.h>
#include <include/core/SkSurface.h>
#include <include/core/SkTextBlob.h>

#include <display_info.h>
#include <hitrace_meter.h>
#include <input_manager.h>
#include <securec.h>
#include <transaction/rs_interfaces.h>
#include <ui/rs_display_node.h>
#include <ui/rs_surface_node.h>
#include <utils/log.h>

using namespace OHOS;
using namespace OHOS::Rosen;
using namespace std::chrono_literals;
using MMIPE = MMI::PointerEvent;

#define TRACE_BEGIN(str) StartTrace(HITRACE_TAG_GRAPHIC_AGP, str);
#define TRACE_END() FinishTrace(HITRACE_TAG_GRAPHIC_AGP);
#define TRACE_COUNT(str, val) CountTrace(HITRACE_TAG_GRAPHIC_AGP, str, val);
#define TRACE_SCOPE(str) HitraceScoped trace(HITRACE_TAG_GRAPHIC_AGP, str)

struct RSData {
    std::shared_ptr<AppExecFwk::EventRunner> runner_ = nullptr;
    std::shared_ptr<VSyncReceiver> receiver_ = nullptr;
    std::shared_ptr<RSSurfaceNode> snode_ = nullptr;
    std::shared_ptr<RSDisplayNode> dnode_ = nullptr;
    std::function<void(void)> onVsync_ = nullptr;
    int64_t lastTime_ = 0;

    void RequestVsync()
    {
        TRACE_COUNT("RequestVsync", 1);
        auto func = [this](int64_t ts, void *data) {
            TRACE_COUNT("RequestVsync", 0);
            (void) ts;
            (void) data;
            if (this->onVsync_ != nullptr) {
                this->onVsync_();
            }
        };
        receiver_->RequestNextVSync({.callback_ = func});
    }
};

SkiaFramework::SkiaFramework()
{
    UpdateTraceLabel();
    data_ = new struct RSData();
}

SkiaFramework::~SkiaFramework()
{
    auto &rsdata = *reinterpret_cast<struct RSData *>(data_);
    rsdata.dnode_->RemoveChild(rsdata.snode_);
    rsdata.snode_ = nullptr;
    rsdata.dnode_ = nullptr;

    if (auto tp = RSTransactionProxy::GetInstance(); tp != nullptr) {
        tp->FlushImplicitTransaction();
    }
    delete reinterpret_cast<struct RSData *>(data_);
}

void SkiaFramework::SetDrawFunc(const DrawFunc &onDraw)
{
    onDraw_ = onDraw;
}

void SkiaFramework::SetResizeFunc(const ResizeFunc &onResize)
{
    onResize_ = onResize;
}

void SkiaFramework::SetGPUAllowance(bool allow)
{
    allowGPU_ = allow;
}

void SkiaFramework::SetWindowWidth(int width)
{
    windowWidth_ = width;
}

void SkiaFramework::SetWindowHeight(int height)
{
    windowHeight_ = height;
}

void SkiaFramework::SetWindowScale(double scale)
{
    windowScale_ = scale;
}

void SkiaFramework::SetWindowTitle(const std::string &title)
{
    windowTitle_ = title;
}

int SkiaFramework::GetWindowWidth() const
{
    return windowWidth_;
}

int SkiaFramework::GetWindowHeight() const
{
    return windowHeight_;
}

double SkiaFramework::GetWindowScale() const
{
    return windowScale_;
}

void SkiaFramework::Run()
{
    TRACE_BEGIN("Run");
    auto &rsdata = *reinterpret_cast<struct RSData *>(data_);
    // vsync
    rsdata.runner_ = AppExecFwk::EventRunner::Create(false);
    auto handler = std::make_shared<AppExecFwk::EventHandler>(rsdata.runner_);
    rsdata.receiver_ = RSInterfaces::GetInstance().CreateVSyncReceiver("vsync", handler);
    rsdata.receiver_->Init();

    // input
    auto mmi = MMI::InputManager::GetInstance();
    if (mmi == nullptr) {
        return;
    }

    mmi->AddInputEventFilter([this](std::shared_ptr<MMI::PointerEvent> pointerEvent) -> bool {
        TRACE_SCOPE("HandleInput");
        std::lock_guard<std::mutex> lock(propsMutex_);

        TRACE_BEGIN("HandleInputLocked");
        auto &rsdata = *reinterpret_cast<struct RSData *>(data_);
        const auto &ids = pointerEvent->GetPointerIds();
        const auto &time = pointerEvent->GetActionTime();
        const auto &action = pointerEvent->GetPointerAction();
        MMI::PointerEvent::PointerItem firstPointer = {};
        MMI::PointerEvent::PointerItem secondPointer = {};

        if (ids.size() == 1) {
            pointerEvent->GetPointerItem(ids[0], firstPointer);
            const auto &x = firstPointer.GetDisplayX();
            const auto &y = firstPointer.GetDisplayY();

            if (action == MMIPE::POINTER_ACTION_DOWN) {
                if (time - rsdata.lastTime_ <= 400 * 1000) {
                    right_ = false;
                    left_ = true;
                }

                if (right_ == false && left_ == false) {
                    right_ = true;
                }
                rsdata.lastTime_ = pointerEvent->GetActionTime();
            }

            if (left_ && action == MMIPE::POINTER_ACTION_DOWN) {
                dirty_ = true;
                clickx_ = x;
                clicky_ = y;
            }
            if (left_ && action == MMIPE::POINTER_ACTION_MOVE) {
                dirty_ = true;
                x_ = x;
                y_ = y;
            }
            if (left_ && action == MMIPE::POINTER_ACTION_UP) {
                dirty_ = true;
                left_ = false;
            }

            if (right_ && action == MMIPE::POINTER_ACTION_DOWN) {
                dirty_ = true;
                downRX_ = x;
                downRY_ = y;
            }
            if (right_ && action == MMIPE::POINTER_ACTION_MOVE) {
                dirty_ = true;
                x_ = x;
                y_ = y;

                mat_ = SkMatrix::MakeTrans(-diffx_, -diffy_).preConcat(mat_);
                diffx_ = x_ - downRX_;
                diffy_ = y_ - downRY_;
                mat_ = SkMatrix::MakeTrans(diffx_, diffy_).preConcat(mat_);
                UpdateInvertMatrix();
            }
            if (right_ && action == MMIPE::POINTER_ACTION_UP) {
                dirty_ = true;
                right_ = false;
                mat_ = SkMatrix::MakeTrans(-diffx_, -diffy_).preConcat(mat_);
                mat_ = SkMatrix::MakeTrans(x - downRX_, y - downRY_).preConcat(mat_);
                UpdateInvertMatrix();
                diffx_ = diffy_ = 0;
            }

        }

        if (ids.size() >= 2) {
            right_ = false;
            left_ = false;

            pointerEvent->GetPointerItem(ids[0], firstPointer);
            pointerEvent->GetPointerItem(ids[1], secondPointer);
            const auto &x1 = firstPointer.GetDisplayX();
            const auto &y1 = firstPointer.GetDisplayY();
            const auto &x2 = secondPointer.GetDisplayX();
            const auto &y2 = secondPointer.GetDisplayY();

            if (action == MMIPE::POINTER_ACTION_DOWN) {
                scalex_ = (x1 + x2) / 2;
                scaley_ = (y1 + y2) / 2;
                scalediff_ = sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
                scaleMat_ = mat_;
            }
            if (action == MMIPE::POINTER_ACTION_MOVE || action == MMIPE::POINTER_ACTION_UP) {
                dirty_ = true;
                auto scalediff = sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
                auto point = invmat_.mapXY(scalex_, scaley_);
                mat_ = scaleMat_;
                mat_ = mat_.preConcat(SkMatrix::MakeTrans(+point.x(), +point.y()));
                mat_ = mat_.preConcat(SkMatrix::MakeScale(scalediff / scalediff_.load()));
                mat_ = mat_.preConcat(SkMatrix::MakeTrans(-point.x(), -point.y()));
                UpdateInvertMatrix();
            }
        }

        rsdata.RequestVsync();
        pointerEvent->MarkProcessed();
        TRACE_END();
        return true;
    });

    // rs node
    rsdata.snode_ = RSSurfaceNode::Create({});
    rsdata.dnode_ = RSDisplayNode::Create({});
    rsdata.snode_->SetBounds(0, 0, windowWidth_, windowHeight_);
    rsdata.snode_->SetAlpha(1);
    rsdata.dnode_->AddChild(rsdata.snode_, -1);
    if (auto tp = RSTransactionProxy::GetInstance(); tp != nullptr) {
        tp->FlushImplicitTransaction();
    } else {
        return;
    }

    rsdata.onVsync_ = [this]() {
        if (dirty_ == false) {
            return;
        }
        dirty_ = false;

        TRACE_SCOPE("OnVsync");
        auto &rsdata = *reinterpret_cast<struct RSData *>(data_);

        sptr<Surface> surface = rsdata.snode_->GetSurface();
        if (surface == nullptr) {
            return;
        }

        sptr<SurfaceBuffer> buffer;
        int32_t releaseFence;
        BufferRequestConfig config = {
            .width = windowWidth_,
            .height = windowHeight_,
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

        auto addr = static_cast<uint8_t *>(buffer->GetVirAddr());
        LOGI("buffer width:%{public}d, height:%{public}d", buffer->GetWidth(), buffer->GetHeight());
        SkBitmap bitmap;
        auto imageInfo = SkImageInfo::Make(buffer->GetWidth(),
                                           buffer->GetHeight(),
                                           kRGBA_8888_SkColorType,
                                           kOpaque_SkAlphaType);
        bitmap.setInfo(imageInfo);
        bitmap.allocPixels();

        SkCanvas canvas(bitmap);
        canvas.resetMatrix();
        canvas.save();

        canvas.clear(SK_ColorWHITE);
        canvas.setMatrix(mat_);
        DrawBefore(canvas);
        if (onDraw_) {
            TRACE_SCOPE("OnDraw");
            onDraw_(canvas);
        }

        canvas.restore();
        DrawColorPicker(canvas, bitmap);
        DrawAfter(canvas);

        constexpr uint32_t stride = 4;
        uint32_t addrSize = buffer->GetWidth() * buffer->GetHeight() * stride;
        void* bitmapAddr = bitmap.getPixels();
        if (auto ret = memcpy_s(addr, addrSize, bitmapAddr, addrSize); ret != EOK) {
            LOGI("memcpy_s failed");
        }

        BufferFlushConfig flushConfig = {
            .damage = {
                .w = buffer->GetWidth(),
                .h = buffer->GetHeight(),
            },
        };
        ret = surface->FlushBuffer(buffer, -1, flushConfig);
        LOGI("flushBuffer ret is: %{public}s", SurfaceErrorStr(ret).c_str());
    };

    mat_ = mat_.preConcat(SkMatrix::MakeScale(windowScale_, windowScale_));
    rsdata.RequestVsync();
    TRACE_END();
    rsdata.runner_->Run();
}

void SkiaFramework::UpdateInvertMatrix()
{
    if (auto ret = mat_.invert(&invmat_); ret == false) {
        invmat_ = SkMatrix::I();
    }
}

void SkiaFramework::DrawString(SkCanvas &canvas, const std::string &str, double x, double y)
{
    SkPaint textPaint;
    textPaint.setAntiAlias(true);
    textPaint.setColor(0xff0066ff);
    textPaint.setStyle(SkPaint::kFill_Style);

    SkFont font;
    font.setTypeface(SkTypeface::MakeFromFile("/system/fonts/HarmonyOS_Sans_SC_Black.ttf"));
    font.setSize(16);
    canvas.drawString(str.c_str(), x, y, font, textPaint);
}

SkPoint3 SkiaFramework::MeasureString(const std::string &str)
{
    SkFont font;
    font.setTypeface(SkTypeface::MakeFromFile("/system/fonts/HarmonyOS_Sans_SC_Black.ttf"));
    font.setSize(16);
    auto width = font.measureText(str.data(), str.length(), SkTextEncoding::kUTF8);
    SkFontMetrics metrics;
    font.getMetrics(&metrics);
    return {width, -metrics.fAscent + metrics.fDescent, -metrics.fAscent};
}

void SkiaFramework::DrawBefore(SkCanvas &canvas)
{
    TRACE_SCOPE("DrawBefore");
    std::lock_guard<std::mutex> lock(propsMutex_);
    SkPaint paint;
    paint.setAntiAlias(true);
    paint.setColor(0x09000000);
    paint.setStyle(SkPaint::kStroke_Style);
    paint.setStrokeWidth(1);
    SkPaint paint2 = paint;
    paint2.setColor(0x22000000);

    SkPaint textPaint;
    textPaint.setAntiAlias(true);
    textPaint.setColor(0xff00007f);

    SkFont font;
    font.setTypeface(SkTypeface::MakeFromFile("/system/fonts/HarmonyOS_Sans_SC_Black.ttf"));
    font.setSize(16);

    auto rect = invmat_.mapRect(SkRect::MakeXYWH(0, 0, windowWidth_, windowHeight_));
    auto left = static_cast<int>(rect.left()) / 100 * 100;
    auto right = static_cast<int>(rect.right());
    auto top = static_cast<int>(rect.top()) / 100 * 100;
    auto bottom = static_cast<int>(rect.bottom());

    SkPath path;
    for (int i = left; i <= right; i += 20) {
        path.moveTo(i, -1e9);
        path.lineTo(i, 1e9);
    }

    for (int i = top; i <= bottom; i += 20) {
        path.moveTo(-1e9, i);
        path.lineTo(1e9, i);
    }
    canvas.drawPath(path, paint);

    SkPath path2;
    for (int i = left; i <= right; i += 100) {
        path2.moveTo(i, -1e9);
        path2.lineTo(i, 1e9);

        std::stringstream ss;
        ss << i;
        canvas.drawString(ss.str().c_str(),
                          i,
                          font.getSize() + 0,
                          font, textPaint);
    }

    for (int i = top; i <= bottom; i += 100) {
        path2.moveTo(-1e9, i);
        path2.lineTo(1e9, i);

        std::stringstream ss;
        ss << i;
        canvas.drawString(ss.str().c_str(),
                          0,
                          font.getSize() + i,
                          font, textPaint);
    }
    canvas.drawPath(path2, paint2);
}

void SkiaFramework::DrawColorPicker(SkCanvas &canvas, SkBitmap &bitmap)
{
    if (left_ == false) {
        return;
    }

    TRACE_SCOPE("DrawColorPicker");
    std::lock_guard<std::mutex> lock(propsMutex_);
    SkFont font;
    font.setTypeface(SkTypeface::MakeFromFile("/system/fonts/HarmonyOS_Sans_SC_Black.ttf"));
    font.setSize(24);

    SkPaint paint;
    paint.setAntiAlias(true);
    paint.setStyle(SkPaint::kFill_Style);
    SkPaint paint2;
    paint2.setAntiAlias(true);
    paint2.setColor(SK_ColorBLACK);
    paint2.setStyle(paint2.kStroke_Style);

    auto color = bitmap.getColor(x_, y_);
    paint.setColor(color);
    std::stringstream ss;
    ss << std::hex << std::setfill('0') << std::setw(6) << color;
    canvas.drawString(ss.str().c_str(), x_, y_, font, paint2);
    canvas.drawString(ss.str().c_str(), x_, y_, font, paint);
}

void SkiaFramework::DrawAfter(SkCanvas &canvas)
{
    if (left_ == false) {
        return;
    }

    TRACE_SCOPE("DrawAfter");
    std::lock_guard<std::mutex> lock(propsMutex_);
    SkPaint paint;
    paint.setAntiAlias(true);
    paint.setColor(0x33000000);
    paint.setStyle(SkPaint::kStroke_Style);
    paint.setStrokeWidth(1);

    SkPaint textPaint;
    textPaint.setAntiAlias(true);
    textPaint.setColor(0xff0000ff);
    textPaint.setStyle(SkPaint::kStroke_Style);
    textPaint.setStrokeWidth(1);

    SkFont font;
    font.setTypeface(SkTypeface::MakeFromFile("/system/fonts/HarmonyOS_Sans_SC_Black.ttf"));
    font.setSize(16);

    SkPath path;
    path.moveTo(x_, 0);
    path.lineTo(x_, 1e9);
    path.moveTo(0, y_);
    path.lineTo(1e9, y_);

    auto point = invmat_.mapXY(x_, y_);
    std::stringstream ss;
    ss << "(" << point.x() << ", " << point.y() << ")";
    canvas.drawString(ss.str().c_str(),
                      x_ + 10,
                      font.getSize() + y_ + 10,
                      font, textPaint);

    canvas.drawPath(path, paint);
}
