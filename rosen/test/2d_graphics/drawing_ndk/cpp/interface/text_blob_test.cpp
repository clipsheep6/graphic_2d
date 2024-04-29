#include "text_blob_test.h"
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_font.h>
#include <native_drawing/drawing_matrix.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_round_rect.h>
#include <native_drawing/drawing_shader_effect.h>
#include <native_drawing/drawing_point.h>
#include <native_drawing/drawing_image.h>
#include <native_drawing/drawing_filter.h>
#include <native_drawing/drawing_sampling_options.h>
#include <native_drawing/drawing_text_blob.h>

#include "test_common.h"
#include "common/log_common.h"

void TextBlobBuilderCreate::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    const int count = 9;
    uint16_t glyphs[9] = { 65, 227, 283, 283, 299, 2, 94, 37, 84 };
    float posX[9] = { 0, 14.9 * 2, 25.84 * 2, 30.62 * 2, 35.4 * 2, 47.22 * 2, 52.62 * 2, 67.42 * 2, 81.7 * 2 };
    float posY[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    TestRend rand;
    float l,t,r,b;    
    for (int i = 0; i < testCount_; i++) {
        l = rand.nextULessThan(bitmapWidth_);
        t = rand.nextULessThan(bitmapHeight_);
        r = l + rand.nextULessThan(bitmapWidth_);
        b = t + rand.nextULessThan(bitmapHeight_);
        OH_Drawing_TextBlobBuilder* builder = OH_Drawing_TextBlobBuilderCreate();
        OH_Drawing_Rect* rect = OH_Drawing_RectCreate(l, t, r, b);    
        const OH_Drawing_RunBuffer* buffer = OH_Drawing_TextBlobBuilderAllocRunPos(builder,font,count,rect);
        for (int idx = 0; idx < count; idx++) {
            buffer->glyphs[idx] = glyphs[idx];
            buffer->pos[idx * 2] = posX[idx];
            buffer->pos[idx * 2 + 1] = posY[idx];
        }        
    OH_Drawing_TextBlob *blob = OH_Drawing_TextBlobBuilderMake(builder);
        OH_Drawing_CanvasDrawTextBlob(canvas, blob, rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_));
    OH_Drawing_TextBlobBuilderDestroy(builder);
    OH_Drawing_RectDestroy(rect);
        OH_Drawing_TextBlobDestroy(blob);
    }
    OH_Drawing_FontDestroy(font);
}

void TextBlobCreateFromText::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    std::string text = "TextBlobCreateFromText";
    int len = text.length();
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    TestRend rand;
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_TextBlob* blob = OH_Drawing_TextBlobCreateFromText(text.c_str(),len,font,TEXT_ENCODING_UTF8);
        OH_Drawing_CanvasDrawTextBlob(canvas, blob, rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_));
        OH_Drawing_TextBlobDestroy(blob);
    }
}

void TextBlobCreateFromPosText::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    std::string text = "TextBlobCreateFromPosText";
    int len = text.length();
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    TestRend rand;
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_Point2D pt;
        pt.x = rand.nextULessThan(bitmapWidth_);
        pt.y = rand.nextULessThan(bitmapHeight_);
        OH_Drawing_TextBlob* blob = OH_Drawing_TextBlobCreateFromPosText(text.c_str(),len,&pt,font,TEXT_ENCODING_UTF8);
        OH_Drawing_CanvasDrawTextBlob(canvas, blob, rand.nextULessThan(bitmapWidth_), rand.nextULessThan( bitmapHeight_));
        OH_Drawing_TextBlobDestroy(blob);
    }
}

void TextBlobCreateFromString::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    std::string text = "TextBlobCreateFromString";
    int len = text.length();
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    TestRend rand;
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_TextBlob* blob = OH_Drawing_TextBlobCreateFromString(text.c_str(),font,TEXT_ENCODING_UTF8);
        OH_Drawing_CanvasDrawTextBlob(canvas, blob,rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_));
        OH_Drawing_TextBlobDestroy(blob);
    }
}

void TextBlobGetBounds::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    std::string text = "TextBlobGetBounds";
    int len = text.length();
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    TestRend rand;
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_TextBlob* blob = OH_Drawing_TextBlobCreateFromString(text.c_str(),font,TEXT_ENCODING_UTF8);
        OH_Drawing_CanvasDrawTextBlob(canvas, blob, rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_));
        OH_Drawing_Rect* r = OH_Drawing_RectCreate(0, 0, 0, 0);
        OH_Drawing_TextBlobGetBounds(blob, r);
//        DRAWING_LOGI("OnTestPerformance  %{public}f,%{public}f,%{public}f,%{public}f",OH_Drawing_RectGetLeft(r),OH_Drawing_RectGetTop(r),OH_Drawing_RectGetRight(r),OH_Drawing_RectGetBottom(r));
        OH_Drawing_RectDestroy(r);
        OH_Drawing_TextBlobDestroy(blob);
    }
}