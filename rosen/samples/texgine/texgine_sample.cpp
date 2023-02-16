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

#include <texgine/typography.h>
#include <texgine/utils/exlog.h>
#include <texgine/utils/memory_usage_scope.h>
#include <texgine/utils/trace.h>

#include "feature_test/feature_test_framework.h"
#include "texgine_canvas.h"
#include "texgine_dash_path_effect.h"
#include "texgine_framework.h"

using namespace Texgine;

namespace {
uint32_t colors[] = {
    SK_ColorRED,
    SK_ColorYELLOW,
    SK_ColorGREEN,
    SK_ColorCYAN,
    SK_ColorBLUE,
    SK_ColorMAGENTA
};
} // namespace

void OnDraw(TexgineCanvas &canvas)
{
    TexginePaint actualBorderPaint;
    actualBorderPaint.SetStyle(TexginePaint::kStroke_Style);

    TexginePaint borderPaint = actualBorderPaint;
    const float intervals[2] = {1.0f, 1.0f};
    borderPaint.SetPathEffect(TexgineDashPathEffect::Make(intervals, 2, 0.0f));

    TexginePaint testBorderPaint = borderPaint;
    testBorderPaint.SetColor(0xff000000);

    TexginePaint rainbowPaint;
    rainbowPaint.SetStyle(TexginePaint::kFill_Style);

    canvas.Save();
    canvas.Translate(50, 50);
    double y = 0;

    const auto &tests = FeatureTestCollection::GetInstance().GetTests();
    for (const auto &ptest : tests) {
        if (ptest == nullptr) {
            continue;
        }

        double yStart = y;
        canvas.Save();
        canvas.Translate(50, 50);
        const auto &option = ptest->GetFeatureTestOption();
        const auto &typographies = ptest->GetTypographies();

        double maxHeight = 0;
        double x = 0;
        for (const auto &data : typographies) {
            const auto &typography = data.typography;
            if ((x + typography->GetMaxWidth() >= 800) || (x != 0 && data.atNewline)) {
                x = 0;
                y += maxHeight + option.marginTop;
                maxHeight = 0;
            }

            if (data.onPaint) {
                data.onPaint(data, canvas, x, y);
            } else {
                typography->Paint(canvas, x, y);
            }

            if (data.needRainbowChar.value_or(option.needRainbowChar)) {
                canvas.Save();
                canvas.Translate(x, y);
                Boundary boundary = {data.rainbowStart, data.rainbowEnd};
                auto boxes = typography->GetTextRectsByBoundary(boundary, data.hs, data.ws);
                int32_t rainbowColorIndex = 0;
                for (auto &box : boxes) {
                    rainbowPaint.SetColor(colors[rainbowColorIndex++]);
                    rainbowPaint.SetAlpha(255 * 0.2);
                    canvas.DrawRect(box.rect_, rainbowPaint);
                    rainbowPaint.SetColor(SK_ColorGRAY);
                    rainbowPaint.SetAlpha(255 * 0.3);
                    canvas.DrawRect(box.rect_, rainbowPaint);
                    rainbowColorIndex %= sizeof(colors) / sizeof(*colors);
                }
                canvas.Restore();
            }

            if (!data.comment.empty()) {
                TexgineFramework::DrawString(canvas, data.comment, x, y);
            }

            if (option.needBorder) {
                borderPaint.SetColor(option.colorBorder);
                auto rectLimit = TexgineRect::MakeXYWH(x, y, typography->GetMaxWidth(), typography->GetHeight());
                canvas.DrawRect(rectLimit, borderPaint);

                actualBorderPaint.SetColor(option.colorBorder);
                auto rectActual = TexgineRect::MakeXYWH(x, y, typography->GetActualWidth(), typography->GetHeight());
                canvas.DrawRect(rectActual, actualBorderPaint);
            }

            x += typography->GetMaxWidth() + option.marginLeft;
            maxHeight = std::max(maxHeight, typography->GetHeight());
        }
        y += maxHeight + option.marginTop + 50;

        canvas.Restore();
        auto rect = TexgineRect::MakeXYWH(0, yStart, 800, y - yStart);
        canvas.DrawRect(rect, testBorderPaint);
        TexgineFramework::DrawString(canvas, ptest->GetTestName(), 0, yStart);
    }
    canvas.Restore();
}

int main()
{
    const auto &tests = FeatureTestCollection::GetInstance().GetTests();
    for (const auto &ptest : tests) {
        if (ptest == nullptr) {
            continue;
        }

        MemoryUsageScope scope(ptest->GetTestName());
        ScopedTrace layoutScope(ptest->GetTestName());
        LOGSCOPED(sl, LOG2EX_DEBUG(), ptest->GetTestName());
        ptest->Layout();
        for (const auto &typography : ptest->GetTypographies()) {
            ReportMemoryUsage("typography", *typography.typography, true);
        }
    }

    TexgineFramework tf;
    tf.SetWindowWidth(720);
    tf.SetWindowHeight(1280);
    tf.SetWindowScale(720.0 / 900.0);
    tf.SetDrawFunc(OnDraw);
    tf.Run();
    return 0;
}
