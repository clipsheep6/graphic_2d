#include "xfermode_bench.h"
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_font.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_rect.h>
#include <native_drawing/drawing_text_blob.h>
#include <time.h>
#include "common/log_common.h"
#include "test_common.h"

struct DrSize {
    float fWidth;
    float fHeight;
};

void XfermodeBench::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetColor(pen, 0xFF00FF00);
    OH_Drawing_PenSetAntiAlias(pen,true);
    OH_Drawing_CanvasAttachPen(canvas, pen);

    TestRend rand = TestRend();
    const char* text = "Hamburgefons";
    size_t len = strlen(text);
    DrSize size = {400,400};
    uint32_t thousand = 1000;

    // 在画布上画path的形状
    OH_Drawing_PenSetBlendMode(pen, mode);
    OH_Drawing_PenSetColor(pen, rand.nextU());
    OH_Drawing_CanvasAttachPen(canvas, pen);
    
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    OH_Drawing_FontSetTextSize(font, rand.nextRangeF(12, 96));
    float x = rand.nextRangeF(0, size.fWidth);
    float y = rand.nextRangeF(0, size.fHeight);
    OH_Drawing_TextBlob* blob =nullptr;
    OH_Drawing_Point2D p = {400,800};
    switch (index) {
        case FromText:
            blob = OH_Drawing_TextBlobCreateFromText(text, len , font, OH_Drawing_TextEncoding::TEXT_ENCODING_UTF8);
            break;
        case FromPosText:
            blob = OH_Drawing_TextBlobCreateFromPosText(text, len ,&p, font, OH_Drawing_TextEncoding::TEXT_ENCODING_UTF8);
            break;
        case FromString:
            blob = OH_Drawing_TextBlobCreateFromString(text,font,OH_Drawing_TextEncoding::TEXT_ENCODING_UTF8);
            break;
        default:
            blob = OH_Drawing_TextBlobCreateFromText(text, len , font, OH_Drawing_TextEncoding::TEXT_ENCODING_UTF8);
            break;
    }
    int iterations = std::min(thousand,testCount_);
    for(int j=0;j<iterations;++j){
        OH_Drawing_CanvasDrawTextBlob(canvas,blob,x,y); 
    }
    OH_Drawing_TextBlobDestroy(blob);
    OH_Drawing_FontDestroy(font);

    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_PenDestroy(pen);
    pen = nullptr;
}

void XfermodeBench::OnTestPerformance(OH_Drawing_Canvas* canvas)
{   
    //当前用例名 drawtextblobcreate 测试 OH_Drawing_TextBlobCreateFromString  迁移基于skia BlendmodeBench.cpp->XfermodeBench
    // skia case name : blendmicro_mask_Screen
    // 创建一个画刷pen对象
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetColor(pen, 0xFF00FF00);
    OH_Drawing_PenSetAntiAlias(pen,true);
    OH_Drawing_CanvasAttachPen(canvas, pen);

    TestRend rand = TestRend();
    const char* text = "Hamburgefons";
    size_t len = strlen(text);
    DrSize size = {400,400};
    uint32_t thousand = 100;
//    for (int i = 0; i < testCount_; i++) {
    while(testCount_ > 0){
        // 在画布上画path的形状
        OH_Drawing_PenSetBlendMode(pen, mode);
        OH_Drawing_PenSetColor(pen, 0xFF000000 | rand.nextU());
        OH_Drawing_CanvasAttachPen(canvas, pen);
        
        OH_Drawing_Font* font = OH_Drawing_FontCreate();
        OH_Drawing_FontSetTextSize(font, rand.nextRangeF(12, 96));
        float x = rand.nextRangeF(0, size.fWidth);
        float y = rand.nextRangeF(0, size.fHeight);
        OH_Drawing_TextBlob* blob =nullptr;
        OH_Drawing_Point2D p = {400,800};
        switch (index) {
            case FromText:
                blob = OH_Drawing_TextBlobCreateFromText(text, len , font, OH_Drawing_TextEncoding::TEXT_ENCODING_UTF8);
                break;
            case FromPosText:
                blob = OH_Drawing_TextBlobCreateFromPosText(text, len ,&p, font, OH_Drawing_TextEncoding::TEXT_ENCODING_UTF8);
                break;
            case FromString:
                blob = OH_Drawing_TextBlobCreateFromString(text,font,OH_Drawing_TextEncoding::TEXT_ENCODING_UTF8);
                break;
            default:
                blob = OH_Drawing_TextBlobCreateFromText(text, len , font, OH_Drawing_TextEncoding::TEXT_ENCODING_UTF8);
                break;
        }
        int iterations = std::min(thousand,testCount_);
        for(int j=0;j<iterations;++j){
            OH_Drawing_CanvasDrawTextBlob(canvas,blob,x,y); 
        }
        OH_Drawing_TextBlobDestroy(blob);
        OH_Drawing_FontDestroy(font);
        testCount_ -= iterations;
    }

    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_PenDestroy(pen);
    pen = nullptr;
}

