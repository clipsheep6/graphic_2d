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

#include <hitrace_meter.h>
#include <include/core/SkCanvas.h>
#include <include/effects/SkDashPathEffect.h>
#include <texgine/typography.h>

#include "feature_test/feature_test_framework.h"
#include "skia_framework.h"

#define TRACE_BEGIN(str) StartTrace(HITRACE_TAG_GRAPHIC_AGP, str);
#define TRACE_END() FinishTrace(HITRACE_TAG_GRAPHIC_AGP);

namespace {
SkColor colors[] = {
    SK_ColorRED,
    SK_ColorYELLOW,
    SK_ColorGREEN,
    SK_ColorCYAN,
    SK_ColorBLUE,
    SK_ColorMAGENTA
};
} // namespace

void OnDraw(SkCanvas &canvas)
{
    SkPaint actualBorderPaint;
    actualBorderPaint.setStyle(SkPaint::kStroke_Style);

    SkPaint borderPaint = actualBorderPaint;
    const SkScalar intervals[2] = {1.0f, 1.0f};
    borderPaint.setPathEffect(SkDashPathEffect::Make(intervals, 2, 0.0f));

    SkPaint testBorderPaint = borderPaint;
    testBorderPaint.setColor(0xff000000);

    SkPaint rainbowPaint;
    rainbowPaint.setStyle(SkPaint::kFill_Style);

    canvas.save();
    canvas.translate(50, 50);
    double y = 0;

    const auto &tests = FeatureTestCollection::GetInstance().GetTests();
    for (const auto &ptest : tests) {
        if (ptest == nullptr) {
            continue;
        }
        TRACE_BEGIN(ptest->GetTestName());

        double yStart = y;
        canvas.save();
        canvas.translate(50, 50);
        const auto &test = *ptest;
        const auto &option = test.GetFeatureTestOption();
        const auto &typographies = test.GetTypographies();

        double maxHeight = 0;
        double x = 0;
        for (const auto &data : typographies) {
            const auto &typography = data.typography;
            if (x + typography->GetWidthLimit() >= 800) {
                x = 0;
                y += maxHeight + option.marginTop;
                maxHeight = 0;
            }

            TRACE_BEGIN("typography::Paint");
            typography->Paint(canvas, x, y);
            TRACE_END();

            if (data.needRainbowChar.value_or(option.needRainbowChar)) {
                TRACE_BEGIN("typography::Rainbow");
                canvas.save();
                canvas.translate(x, y);
                Texgine::Boundary boundary = {data.rainbowStart, data.rainbowEnd};
                auto boxes = typography->GetTextRectsByBoundary(boundary, data.hs, data.ws);
                int32_t rainbowColorIndex = 0;
                for (auto &box : boxes) {
                    rainbowPaint.setColor(colors[rainbowColorIndex++]);
                    rainbowPaint.setAlpha(255 * 0.2);
                    canvas.drawRect(box.rect_, rainbowPaint);
                    rainbowPaint.setColor(SK_ColorGRAY);
                    rainbowPaint.setAlpha(255 * 0.3);
                    canvas.drawRect(box.rect_, rainbowPaint);
                    rainbowColorIndex %= sizeof(colors) / sizeof(*colors);
                }
                canvas.restore();
                TRACE_END();
            }

            if (!data.comment.empty()) {
                TRACE_BEGIN("comment");
                SkiaFramework::DrawString(canvas, data.comment, x, y);
                TRACE_END();
            }

            if (option.needBorder) {
                TRACE_BEGIN("border");
                borderPaint.setColor(option.colorBorder);
                canvas.drawRect(SkRect::MakeXYWH(x, y, typography->GetWidthLimit(),
                    typography->GetHeight()), borderPaint);

                actualBorderPaint.setColor(option.colorBorder);
                canvas.drawRect(SkRect::MakeXYWH(x, y, typography->GetActualWidth(),
                    typography->GetHeight()), actualBorderPaint);
                TRACE_END();
            }

            x += typography->GetWidthLimit() + option.marginLeft;
            maxHeight = std::max(maxHeight, typography->GetHeight());
        }
        y += maxHeight + option.marginTop + 50;

        canvas.restore();
        TRACE_BEGIN("TestBorderName");
        canvas.drawRect(SkRect::MakeXYWH(0, yStart, 800, y - yStart), testBorderPaint);
        SkiaFramework::DrawString(canvas, ptest->GetTestName(), 0, yStart);
        TRACE_END();
        TRACE_END();
    }
    canvas.restore();
}

int main()
{
    SkiaFramework sf;
    sf.SetWindowWidth(720);
    sf.SetWindowHeight(1280);
    sf.SetWindowScale(720.0 / 900.0);
    sf.SetDrawFunc(OnDraw);
    sf.Run();
    return 0;
}
