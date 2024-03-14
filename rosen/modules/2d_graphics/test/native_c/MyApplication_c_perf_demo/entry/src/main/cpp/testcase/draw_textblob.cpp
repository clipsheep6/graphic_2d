#include "draw_textblob.h"
#include <bits/alltypes.h>
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_font.h>
#include <native_drawing/drawing_text_blob.h>
#include <native_drawing/drawing_typeface.h>
#include <native_drawing/drawing_text_blob.h>
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_rect.h>
#include <native_drawing/drawing_canvas.h>
#include <native_drawing/drawing_types.h>
#include "common/log_common.h"


void DrawTextBlob::OnTestFunction(OH_Drawing_Canvas *canvas)
{
    // 创建字体，并设置文字大小
    OH_Drawing_Font *font = OH_Drawing_FontCreate();
    if (true) {
        // 创建要显示的字符
        size_t size = 45;
        const char *buffer = "Keep your sentences short, but not overly so.";
        // 通过字符和对应的编码格式创建默认排版的文本
        OH_Drawing_TextBlob *textBlob =
            OH_Drawing_TextBlobCreateFromText(buffer, size, font,
            OH_Drawing_TextEncoding::TEXT_ENCODING_UTF8);

        // 将文本绘制到画布上
        OH_Drawing_CanvasDrawTextBlob(canvas, textBlob, 0, 100);
        // 释放内存
        OH_Drawing_TextBlobDestroy(textBlob);
    } else {
        // 创建文本构造器
        OH_Drawing_TextBlobBuilder *builder = OH_Drawing_TextBlobBuilderCreate();
        const int count = 9;
        uint16_t glyphs[9] = {65,227,283,283,299,2,94,37,84};
        float posX[9] = {0, 14.9 * 2, 25.84 * 2, 30.62 * 2, 35.4 * 2, 47.22 * 2, 52.62 * 2, 67.42 * 2, 81.7 * 2};
        float posY[9] = {0, 0,0,0,0,0,0,0,0};
    
        // 申请一块内存
        const OH_Drawing_RunBuffer *runBuffer = OH_Drawing_TextBlobBuilderAllocRunPos(builder, font, count, nullptr);
        // glyphs、posX和posY是开发者自排版产生的数据，使用该数据填充内存
        for (int idx = 0; idx < count; idx++) {
            runBuffer->glyphs[idx] = glyphs[idx];
            runBuffer->pos[idx * 2] = posX[idx];
            runBuffer->pos[idx * 2 + 1] = posY[idx];
        }
        // 通过文本构造器创建文本
        OH_Drawing_TextBlob *textBlob = OH_Drawing_TextBlobBuilderMake(builder);
        // 释放内存
        OH_Drawing_TextBlobBuilderDestroy(builder);
    }
    OH_Drawing_FontDestroy(font);
}

void DrawTextBlob::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    // 创建字体，并设置文字大小
    OH_Drawing_Font *font = OH_Drawing_FontCreate();
    // 创建要显示的字符
    size_t size = 45;
    const char *buffer = "Keep your sentences short, but not overly so.";
    // 通过字符和对应的编码格式创建默认排版的文本
    OH_Drawing_TextBlob *textBlob =
        OH_Drawing_TextBlobCreateFromText(buffer, size, font, OH_Drawing_TextEncoding::TEXT_ENCODING_UTF8);
    
    // 将文本绘制到画布上
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_CanvasDrawTextBlob(canvas, textBlob, 0, 100);
    }
    // 释放内存
    OH_Drawing_TextBlobDestroy(textBlob);
    OH_Drawing_FontDestroy(font);
}

