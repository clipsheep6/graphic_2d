/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "text_span.h"

#include <iomanip>
#include <stack>

#include <hb-icu.h>
#include <unicode/ubidi.h>

#include "font_collection.h"
#include "font_styles.h"
#include "measurer.h"
#include "texgine_dash_path_effect.h"
#include "texgine_exception.h"
#include "texgine_mask_filter.h"
#include "texgine_path.h"
#include "texgine_path_1d_path_effect.h"
#include "texgine/utils/exlog.h"
#ifdef LOGGER_ENABLE_SCOPE
#include "texgine/utils/trace.h"
#endif
#include "text_converter.h"
#include "word_breaker.h"
#include "utils/log.h"
namespace OHOS {
namespace Rosen {
namespace TextEngine {
using namespace Drawing;
#define MAXRGB 255
#define OFFSETY 3
#define HALF 0.5f
#define WIDTH_SCALAR 5.0f
#define HEIGHT_SCALAR 5.0f
#define DOTTED_ADVANCE 10.0f
#define WAVY_ADVANCE 6.0f
#define PHASE 0.0f
#define COUNT 2
#define MAX_BLURRADIUS 64u
#define POINTX0 0
#define POINTX1 1
#define POINTX2 2
#define POINTX3 3
#define POINTX4 4
#define POINTX5 5
#define POINTX6 6
#define POINTY0 0
#define POINTY2 2
#define POINTY4 4
#define POINTY6 6

std::shared_ptr<TextSpan> TextSpan::MakeEmpty()
{
    return std::make_shared<TextSpan>();
}

std::shared_ptr<TextSpan> TextSpan::MakeFromText(const std::string &text)
{
    auto span = MakeEmpty();
    auto decodeText = TextConverter::ToUTF16(text);
    span->AddUTF16Text(decodeText);
    return span;
}

std::shared_ptr<TextSpan> TextSpan::MakeFromText(const std::u16string &text)
{
    auto span = MakeEmpty();
    std::vector<uint16_t> u16;
    for (const auto &t : text) {
        u16.push_back(t);
    }
    span->AddUTF16Text(u16);
    return span;
}

std::shared_ptr<TextSpan> TextSpan::MakeFromText(const std::u32string &text)
{
    auto span = MakeEmpty();
    std::vector<uint32_t> u32;
    for (const auto &t : text) {
        u32.push_back(t);
    }
    auto decodeText = TextConverter::ToUTF16(u32);
    span->AddUTF16Text(decodeText);
    return span;
}

std::shared_ptr<TextSpan> TextSpan::MakeFromText(const std::vector<uint16_t> &text)
{
    auto span = MakeEmpty();
    span->AddUTF16Text(text);
    return span;
}

std::shared_ptr<TextSpan> TextSpan::MakeFromText(const std::vector<uint32_t> &text)
{
    auto span = MakeEmpty();
    auto decodeText = TextConverter::ToUTF16(text);
    span->AddUTF16Text(decodeText);
    return span;
}

std::shared_ptr<TextSpan> TextSpan::MakeFromCharGroups(const CharGroups &cgs)
{
    auto span = MakeEmpty();
    span->cgs_= cgs;
    return span;
}

void TextSpan::AddUTF16Text(const std::vector<uint16_t> &text)
{
    u16vect_.insert(u16vect_.end(), text.begin(), text.end());
}

std::shared_ptr<TextSpan> TextSpan::CloneWithCharGroups(const CharGroups &cgs)
{
    auto span = MakeEmpty();
    *span = *this;
    span->cgs_ = cgs;
    return span;
}

double TextSpan::GetHeight() const
{
    return *tmetrics_.fDescent_ - *tmetrics_.fAscent_;
}

double TextSpan::GetWidth() const
{
    return width_;
}

double TextSpan::GetPreBreak() const
{
    return preBreak_;
}

double TextSpan::GetPostBreak() const
{
    return postBreak_;
}

bool TextSpan::IsRTL() const
{
    return rtl_;
}

void TextSpan::Paint(std::shared_ptr<Drawing::RecordingCanvas> recordingCanvas, double offsetX, double offsetY, const TextStyle &xs, Drawing::Canvas& canvasSkia)
{
    auto skiaCanvas = canvasSkia.GetImpl<Drawing::SkiaCanvas>();
    TexginePaint paint;
    Brush brush;
    // paint.SetAntiAlias(true);
    brush.SetAntiAlias(true);
#ifndef USE_GRAPHIC_TEXT_GINE
    // paint.SetARGB(MAXRGB, MAXRGB, 0, 0);
    brush.SetARGB(MAXRGB, MAXRGB, 0, 0);
#else
    // paint.SetAlpha(255);
    brush.SetAlpha(255);
#endif
    // paint.SetColor(xs.color);
    brush.SetColor(xs.color);
    // paint.SetStyle(TexginePaint::FILL);
    if (xs.background.has_value()) {
        // auto rect = TexgineRect::MakeXYWH(offsetX, offsetY + *tmetrics_.fAscent_, width_,
        //     *tmetrics_.fDescent_ - *tmetrics_.fAscent_);
        Drawing::Rect rect(offsetX, offsetY + *tmetrics_.fAscent_, width_,
            *tmetrics_.fDescent_ - *tmetrics_.fAscent_);
        LOGEX_FUNC_LINE(ERROR) << "PlayBack One";
        recordingCanvas->DrawRect(rect);
        auto drawCmdList = recordingCanvas->GetDrawCmdList();
        LOGEX_FUNC_LINE(ERROR) << "PlayBack Two";
        drawCmdList->Playback(canvasSkia);
        //canvas.DrawRect(rect, xs.background.value());
    }

    if (xs.foreground.has_value()) {
        LOGEX_FUNC_LINE(ERROR) << "PlayBack Three";
#ifndef USE_GRAPHIC_TEXT_GINE
        paint = xs.foreground.value();
        SkPaint skPaint = paint.GetPaint();
        skiaCanvas->InitPaintData(skPaint);
#else
        // auto rect = TexgineRect::MakeXYWH(offsetX, offsetY + *tmetrics_.fAscent_, width_,
        //     *tmetrics_.fDescent_ - *tmetrics_.fAscent_);
        // canvas.DrawRect(rect, xs.foreground.value());
        Drawing::Rect rect(offsetX, offsetY + *tmetrics_.fAscent_, width_,
            *tmetrics_.fDescent_ - *tmetrics_.fAscent_);
        recordingCanvas->DrawRect(rect);
        auto drawCmdList = recordingCanvas->GetDrawCmdList();
        drawCmdList->Playback(canvasSkia);
#endif
    }
    // SkPaint skPaint = paint.GetPaint();
    // skiaCanvas->InitPaintData(skPaint);
    canvasSkia.AttachBrush(brush);
    // if (bool testSomething = (skPaint == (skiaCanvas->TestFillPaint()))) {
    //     LOGEX_FUNC_LINE(ERROR) << "skPaint == Yes11";
    // }
    // else {
    //     LOGEX_FUNC_LINE(ERROR) << "skPaint == No";
    // }

    CheckTextValidity();
    LOGEX_FUNC_LINE(ERROR) << "skPaint 1";
    LOGE("Recording | TextSpan |Ready DrawTextBlob ");
    recordingCanvas->DrawTextBlob(skTextBlob_, offsetX, offsetY);

    //recordingCanvas->DrawTextBlob(tempTextBlob,offsetX,offsetY);
    LOGEX_FUNC_LINE(ERROR) << "skPaint 2";
    auto drawCmdList = recordingCanvas->GetDrawCmdList();
    LOGEX_FUNC_LINE(ERROR) << "skPaint 3";
    drawCmdList->Playback(canvasSkia);
    canvasSkia.DetachBrush();
    //skCanvas->drawTextBlob(textBlob.GetImpl<SkiaTextBlob>()->GetSkTextBlob(), offsetX, offsetY, tempRecv);
    PaintDecoration(recordingCanvas, offsetX, offsetY, xs, canvasSkia);
}

void TextSpan::PaintDecoration(std::shared_ptr<Drawing::RecordingCanvas> recordingCanvas, double offsetX, double offsetY, const TextStyle &xs, Drawing::Canvas& customCanvas)
{
    double left = offsetX;
    double right = left + GetWidth();
    LOGEX_FUNC_LINE(ERROR) << "skPaint 4";
    if ((xs.decoration & TextDecoration::UNDERLINE) == TextDecoration::UNDERLINE) {
        double y = offsetY + *tmetrics_.fUnderlinePosition_;
        PaintDecorationStyle(recordingCanvas, left, right, y, xs, customCanvas);
    }
    if ((xs.decoration & TextDecoration::OVERLINE) == TextDecoration::OVERLINE) {
        double y = offsetY - abs(*tmetrics_.fAscent_);
        PaintDecorationStyle(recordingCanvas, left, right, y, xs, customCanvas);
    }
    if ((xs.decoration & TextDecoration::LINE_THROUGH) == TextDecoration::LINE_THROUGH) {
        double y = offsetY - (*tmetrics_.fCapHeight_ * HALF);
        PaintDecorationStyle(recordingCanvas, left, right, y, xs, customCanvas);
    }
    if ((xs.decoration & TextDecoration::BASELINE) == TextDecoration::BASELINE) {
        double y = offsetY;
        PaintDecorationStyle(recordingCanvas, left, right, y, xs, customCanvas);
    }
    LOGEX_FUNC_LINE(ERROR) << "skPaint 5";
}

void TextSpan::PaintDecorationStyle(std::shared_ptr<Drawing::RecordingCanvas> recordingCanvas, double left, double right, double y, const TextStyle &xs, Canvas& customCanvas)
{
    LOGEX_FUNC_LINE(ERROR) << "skPaint 6";
    TexginePaint paint;
    paint.SetAntiAlias(true);
    paint.SetARGB(MAXRGB, MAXRGB, 0, 0);
    paint.SetColor(xs.decorationColor.value_or(xs.color));
    paint.SetStrokeWidth(xs.decorationThicknessScale);

    auto skiaCanvas = customCanvas.GetImpl<SkiaCanvas>();
    auto skPaint = paint.GetPaint();
    skiaCanvas->InitPaintData(skPaint);
    auto drawCmdList = recordingCanvas->GetDrawCmdList();//chushihua

    Point pointL(static_cast<float>(left), static_cast<float>(y));
    Point pointR(static_cast<float>(right), static_cast<float>(y));
    LOGEX_FUNC_LINE(ERROR) << "skPaint 7";
    switch (xs.decorationStyle) {
        case TextDecorationStyle::SOLID:
            break;
        case TextDecorationStyle::DOUBLE:
            recordingCanvas->DrawLine(pointL,pointR);
            drawCmdList = recordingCanvas->GetDrawCmdList();
            drawCmdList->Playback(customCanvas);
            //canvas.DrawLine(left, y, right, y, paint);
            y += OFFSETY;
            break;
        case TextDecorationStyle::DOTTED: {
            TexginePath circle;
            auto rect = TexgineRect::MakeWH(WIDTH_SCALAR, HEIGHT_SCALAR);
            circle.AddOval(rect);
            paint.SetPathEffect(TexginePath1DPathEffect::Make(circle, DOTTED_ADVANCE, PHASE,
                TexginePath1DPathEffect::K_ROTATE_STYLE));
            skPaint = paint.GetPaint();
            skiaCanvas->InitPaintData(skPaint);
            break;
        }
        case TextDecorationStyle::DASHED: {
            const float intervals[2] = {WIDTH_SCALAR, HEIGHT_SCALAR};
            paint.SetPathEffect(TexgineDashPathEffect::Make(intervals, COUNT, PHASE));
            paint.SetStyle(TexginePaint::STROKE);
            SkPaint tempSkPaint = paint.GetPaint();
            skiaCanvas->InitPaintData(tempSkPaint);
            break;
        }
        case TextDecorationStyle::WAVY: {
            TexginePath wavy;
            float thickness = xs.decorationThicknessScale;
            wavy.MoveTo({POINTX0, POINTY2 - thickness});
            wavy.QuadTo({POINTX1, POINTY0 - thickness}, {POINTX2, POINTY2 - thickness});
            wavy.LineTo({POINTX3, POINTY4 - thickness});
            wavy.QuadTo({POINTX4, POINTY6 - thickness}, {POINTX5, POINTY4 - thickness});
            wavy.LineTo({POINTX6, POINTY2 - thickness});
            wavy.LineTo({POINTX6, POINTY2 + thickness});
            wavy.LineTo({POINTX5, POINTY4 + thickness});
            wavy.QuadTo({POINTX4, POINTY6 + thickness}, {POINTX3, POINTY4 + thickness});
            wavy.LineTo({POINTX2, POINTY2 + thickness});
            wavy.QuadTo({POINTX1, POINTY0 + thickness}, {POINTX0, POINTY2 + thickness});
            wavy.LineTo({POINTX0, POINTY2 - thickness});
            paint.SetPathEffect(TexginePath1DPathEffect::Make(wavy, WAVY_ADVANCE, PHASE,
                TexginePath1DPathEffect::K_ROTATE_STYLE));
            paint.SetStyle(TexginePaint::STROKE);
            skPaint = paint.GetPaint();
            skiaCanvas->InitPaintData(skPaint);
            break;
        }
    }
    LOGEX_FUNC_LINE(ERROR) << "skPaint 8";
    Point otherPointL(static_cast<float>(left), static_cast<float>(y));
    Point otherPointR(static_cast<float>(right), static_cast<float>(y));
    recordingCanvas->DrawLine(pointL,pointR);
    drawCmdList = recordingCanvas->GetDrawCmdList();
    drawCmdList->Playback(customCanvas);
    //canvas.DrawLine(left, y, right, y, paint);
}

// void TextSpan::PaintShadow(TexgineCanvas &canvas, double offsetX, double offsetY,
//     const std::vector<TextShadow> &shadows)
void TextSpan::PaintShadow(std::shared_ptr<Drawing::RecordingCanvas> recordingCanvas, double offsetX, double offsetY, 
const std::vector<TextShadow> &shadows, Canvas& customCanvas)
{
    auto skiaCanvas = customCanvas.GetImpl<SkiaCanvas>();
    LOGEX_FUNC_LINE(ERROR) << "ready into  for circle";
    for (const auto &shadow : shadows) {
        CheckTextValidity();
        LOGEX_FUNC_LINE(ERROR) << "PaintShadow for circleing";
        auto x = offsetX + shadow.offsetX;
        auto y = offsetY + shadow.offsetY;
        auto blurRadius = std::min(shadow.blurLeave, MAX_BLURRADIUS);
        TexginePaint paint;
        paint.SetAntiAlias(true);
        paint.SetColor(shadow.color);
        paint.SetMaskFilter(TexgineMaskFilter::MakeBlur(TexgineMaskFilter::K_NORMAL_SK_BLUR_STYLE, blurRadius));
        SkPaint tempSkPaint = paint.GetPaint();
        skiaCanvas->InitPaintData(tempSkPaint);
        recordingCanvas->DrawTextBlob(skTextBlob_,x,y);
        auto drawCmdList = recordingCanvas->GetDrawCmdList();
        drawCmdList->Playback(customCanvas);
        //customCanvas.DrawTextBlob(textBlob_, x, y, paint);
    }
}

void TextSpan::CheckTextValidity()
{
    if (textBlob_->DetectionEffectiveness())
    {
        LOGEX_FUNC_LINE(ERROR) << "PaintShadow textBlob_->detectionEffectiveness() == true";
        sk_sp<SkTextBlob> skTextBlob = textBlob_->GetTextBlob();
        auto skiaTextBlob = std::make_shared<SkiaTextBlob>();
        skiaTextBlob->SetSkTextBlob(std::move(skTextBlob));
        skTextBlob_.InitTextBlobImpl(std::move(skiaTextBlob));
    }
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
