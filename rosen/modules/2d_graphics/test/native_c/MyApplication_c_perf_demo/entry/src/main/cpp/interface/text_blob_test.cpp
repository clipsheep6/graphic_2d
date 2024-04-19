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
    std::string text = "textblob";
    int len = text.length();
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    int count =  OH_Drawing_FontCountText(font,text.c_str(),len,OH_Drawing_TextEncoding::TEXT_ENCODING_UTF8);
    TestRend rand;
    
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_TextBlobBuilder* builder = OH_Drawing_TextBlobBuilderCreate();
        OH_Drawing_TextBlobBuilderDestroy(builder);
    }

    OH_Drawing_TextBlobBuilder* builder = OH_Drawing_TextBlobBuilderCreate();;
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(100, 100,200, 200);    
    OH_Drawing_TextBlobBuilderAllocRunPos(builder,font,count,rect);
    OH_Drawing_TextBlob *blob = OH_Drawing_TextBlobBuilderMake(builder);
    OH_Drawing_CanvasDrawTextBlob(canvas, blob, 0, 0);
    OH_Drawing_TextBlobBuilderDestroy(builder);
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_FontDestroy(font);
}

void TextBlobCreateFromText::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    std::string text = "textblob";
    int len = text.length();
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    TestRend rand;
    for (int i = 0; i < testCount_; i++) {
        int x = rand.nextRangeF(0, bitmapWidth_);
        int y = rand.nextRangeF(0, bitmapHeight_);
        
        OH_Drawing_TextBlob* blob = OH_Drawing_TextBlobCreateFromText(text.c_str(),len,font,TEXT_ENCODING_UTF8);
        OH_Drawing_CanvasDrawTextBlob(canvas, blob, x, y);
        OH_Drawing_TextBlobDestroy(blob);
    }
}

void TextBlobCreateFromPosText::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    std::string text = "textblob";
    int len = text.length();
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    TestRend rand;
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_Point2D pt;
        pt.x = rand.nextRangeF(0, bitmapWidth_);
        pt.y = rand.nextRangeF(0, bitmapHeight_);
        OH_Drawing_TextBlob* blob = OH_Drawing_TextBlobCreateFromPosText(text.c_str(),len,&pt,font,TEXT_ENCODING_UTF8);
        OH_Drawing_CanvasDrawTextBlob(canvas, blob, 0, 0);
        OH_Drawing_TextBlobDestroy(blob);
    }
}

void TextBlobCreateFromString::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    std::string text = "textblob";
    int len = text.length();
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    TestRend rand;
    for (int i = 0; i < testCount_; i++) {
        int x = rand.nextRangeF(0, bitmapWidth_);
        int y = rand.nextRangeF(0, bitmapHeight_);
        OH_Drawing_TextBlob* blob = OH_Drawing_TextBlobCreateFromString(text.c_str(),font,TEXT_ENCODING_UTF8);
        OH_Drawing_CanvasDrawTextBlob(canvas, blob,x, y);
        OH_Drawing_TextBlobDestroy(blob);
    }
}

void TextBlobGetBounds::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    std::string text = "textblob";
    int len = text.length();
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    TestRend rand;
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_Point2D pt;
        int x = rand.nextRangeF(0, bitmapWidth_);
        int y = rand.nextRangeF(0, bitmapHeight_);
        OH_Drawing_TextBlob* blob = OH_Drawing_TextBlobCreateFromString(text.c_str(),font,TEXT_ENCODING_UTF8);
        OH_Drawing_CanvasDrawTextBlob(canvas, blob, x, y);
        OH_Drawing_TextBlobDestroy(blob);
        OH_Drawing_Rect* r = OH_Drawing_RectCreate(0, 0, 0, 0);
        OH_Drawing_TextBlobGetBounds(blob, r);
//        DRAWING_LOGI("OnTestPerformance  %{public}f,%{public}f,%{public}f,%{public}f",OH_Drawing_RectGetLeft(r),OH_Drawing_RectGetTop(r),OH_Drawing_RectGetRight(r),OH_Drawing_RectGetBottom(r));
        OH_Drawing_RectDestroy(r);
        OH_Drawing_TextBlobDestroy(blob);
    }
}