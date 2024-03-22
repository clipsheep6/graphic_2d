#include "draw_text_blob_builder.h"
#include <native_drawing/drawing_text_blob.h>
#include <native_drawing/drawing_font.h>
#include <native_drawing/drawing_rect.h>
#include <native_drawing/drawing_canvas.h>

void DrawTextBlobBuilder::OnTestFunction(OH_Drawing_Canvas *canvas) {
    OH_Drawing_Font *font = OH_Drawing_FontCreate();
    // 创建一个文本构造器对象
    OH_Drawing_TextBlobBuilder *builder = OH_Drawing_TextBlobBuilderCreate();
    OH_Drawing_TextBlobBuilderAllocRunPos(builder, font, 1, nullptr);
    // 用文本构造器创造一个文本对象
    OH_Drawing_TextBlob *textBlob = OH_Drawing_TextBlobBuilderMake(builder);
    OH_Drawing_CanvasDrawTextBlob(canvas, textBlob, 10, 10);
    // 释放内存
    OH_Drawing_TextBlobBuilderDestroy(builder);
    OH_Drawing_TextBlobDestroy(textBlob);
    OH_Drawing_FontDestroy(font);
}

void DrawTextBlobBuilder::OnTestPerformance(OH_Drawing_Canvas *canvas) {
    // TextBlobCachedBench
    OH_Drawing_Font *font = OH_Drawing_FontCreate();
    // 创建一个文本构造器对象
    OH_Drawing_TextBlobBuilder* builder = OH_Drawing_TextBlobBuilderCreate();
    OH_Drawing_TextBlobBuilderAllocRunPos(builder, font, 1, nullptr);
    // 用文本构造器创造一个文本对象
    OH_Drawing_TextBlob* textBlob = OH_Drawing_TextBlobBuilderMake(builder);
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_CanvasDrawTextBlob(canvas, textBlob, 10, 10);
    }
    // 释放内存
    OH_Drawing_TextBlobBuilderDestroy(builder);
    OH_Drawing_TextBlobDestroy(textBlob);
    OH_Drawing_FontDestroy(font);
}
