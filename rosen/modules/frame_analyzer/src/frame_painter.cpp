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

#include "frame_painter.h"

#include <map>

#include "hilog/log.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkColor.h"

#include "frame_collector.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr ::OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, 0xD001400, "FramePainter" };
constexpr float FLUENCY_THRESHOLDS = 0.8f;
constexpr float NORMAL_THRESHOLDS = 1.0f;
constexpr float FREEZE_THRESHOLDS = 1.5f;


struct Threshold {
    uint32_t color;
    float percentFrametime;
};

static constexpr std::array<Threshold, 3> THRESHOLDS {
    Threshold{.color = static_cast<int32_t>(FrameAnalyerColor::LightGreen), .percentFrametime = FLUENCY_THRESHOLDS},
    Threshold{.color = static_cast<int32_t>(FrameAnalyerColor::Lime), .percentFrametime = NORMAL_THRESHOLDS},
    Threshold{.color = static_cast<int32_t>(FrameAnalyerColor::Red), .percentFrametime = FREEZE_THRESHOLDS},
};
} // namespace

FramePainter::FramePainter(FrameCollector &collector) : collector_(collector)
{
}

void FramePainter::Draw(SkCanvas &canvas)
{
    if (collector_.IsEnabled() == false) {
        return;
    }

    auto width = canvas.imageInfo().width();
    auto height = canvas.imageInfo().height();
    auto heightPerMs = height / frameTotalMs;
    ::OHOS::HiviewDFX::HiLog::Debug(LABEL, "FramePainter::Draw %{public}dx%{public}d", width, height);

    const auto normalFPS = FrameCollector::GetInstance().GetFps();
    auto bars = GenerateTimeBars(width, height, normalFPS);
    SkPaint paint;
    paint.setStyle(paint.kFill_Style);
    for (const auto &[isHeavy, color, x, y, w, h] : bars) {
        constexpr uint32_t heavyFrameAlpha = 0x7f;
        constexpr uint32_t lightFrameAlpha = 0x3f;
        auto alpha = isHeavy ? heavyFrameAlpha : lightFrameAlpha;
        constexpr uint32_t alphaOffset = 24; // ARGB
        paint.setColor(color | (alpha << alphaOffset));
        canvas.drawRect(SkRect::MakeXYWH(x, y, w, h), paint);
    }

    DrawFPSLine(canvas, normalFPS, heightPerMs);
}

void FramePainter::DrawFPSLine(SkCanvas &canvas, uint32_t normalFps, uint32_t heightPerMs)
{
    if (normalFps == 0) {
        return;
    }

    SkPaint paint;
    auto width = canvas.imageInfo().width();
    auto height = canvas.imageInfo().height();

    constexpr auto OneSecondInMs = 1000.0;
    constexpr uint32_t lineAlpha = 0xFF;
    constexpr uint32_t alphaOffset = 24;
    for (auto& t : THRESHOLDS) {
        auto fps = normalFps * t.percentFrametime;
        auto bottom = OneSecondInMs / fps * heightPerMs;
        auto lineOffset = heightPerMs / 0x2; // vertical align center
        auto fpsLine = SkRect::MakeXYWH(0, height - (bottom - lineOffset), width, heightPerMs);

        paint.setStyle(paint.kFill_Style);
        paint.setColor(t.color | (lineAlpha << alphaOffset));
        canvas.drawRect(fpsLine, paint);
    }
}

std::vector<struct FramePainter::TimeBar> FramePainter::GenerateTimeBars(
    uint32_t width, uint32_t height, uint32_t fps)
{
    std::vector<struct TimeBar> bars;

    auto heightPerMs = height / frameTotalMs;
    constexpr auto reservedSize = 2.0;
    auto barWidth = width / (frameQueueMaxSize + reservedSize);
    auto offsetX = barWidth * frameQueueMaxSize - barWidth;

    auto frameQueue = collector_.LockGetFrameQueue();
    for (auto rit = frameQueue.rbegin(); rit != frameQueue.rend(); rit++) {
        constexpr auto OneSecondInMs = 1000.0;
        auto isHeavy = SumTimesInMs(*rit) >= (OneSecondInMs / fps);
        auto offsetY = height;
        constexpr auto loopstart = static_cast<size_t>(FrameEventType::LoopStart);
        constexpr auto loopend = static_cast<size_t>(FrameEventType::LoopEnd);
        constexpr size_t frameEventTypeInterval = 2;
        for (size_t i = loopstart; i < loopend; i += frameEventTypeInterval) {
            auto it = frameEventColorMap.find(static_cast<FrameEventType>(i));
            if (it == frameEventColorMap.end()) {
                continue;
            }
            auto color = it->second;

            constexpr double nanosecondsToMilliseconds = 1e6;
            double diffMs = (rit->times[i + 1] - rit->times[i]) / nanosecondsToMilliseconds;
            if (diffMs < 0) {
                ::OHOS::HiviewDFX::HiLog::Warn(LABEL, "FramePainter::Draw %{public}zu range is negative", i);
                continue;
            } else if (diffMs == 0) {
                continue;
            }

            auto diffHeight = diffMs * heightPerMs;
            offsetY -= diffHeight;
            struct TimeBar bar = {
                .isHeavy = isHeavy,
                .color = color,
                .posX = offsetX,
                .posY = offsetY,
                .width = barWidth,
                .height = diffHeight,
            };
            bars.push_back(bar);
        }
        offsetX -= barWidth;
    }
    collector_.UnlockFrameQueue();

    return bars;
}

double FramePainter::SumTimesInMs(const struct FrameInfo &info)
{
    auto sumMs = 0.0;
    constexpr auto loopstart = static_cast<size_t>(FrameEventType::LoopStart);
    constexpr auto loopend = static_cast<size_t>(FrameEventType::LoopEnd);
    constexpr size_t frameEventTypeInterval = 2;
    for (size_t i = loopstart; i < loopend; i += frameEventTypeInterval) {
        const auto &cm = frameEventColorMap;
        if (cm.find(static_cast<FrameEventType>(i)) == cm.end()) {
            continue;
        }

        constexpr double nanosecondsToMilliseconds = 1e6;
        double diffMs = (info.times[i + 1] - info.times[i]) / nanosecondsToMilliseconds;
        if (diffMs <= 0) {
            continue;
        }

        sumMs += diffMs;
    }

    return sumMs;
}
} // namespace Rosen
} // namespace OHOS
