#include "skbug_8955.h"
#include "test_common.h"
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_text_blob.h>
#include <native_drawing/drawing_font.h>
#include "common/log_common.h"

enum {
    kW = 100,
    kH = 100,
};

SkBug_8955::SkBug_8955() {
    // skia dm file gm/skbug_8955.cpp
    bitmapWidth_ = kW;
    bitmapHeight_ = kH;
    fileName_ = "skbug_8955";
}

OH_Drawing_TextBlob *makeFromText(const void *text, size_t byteLength, OH_Drawing_Font *font, OH_Drawing_TextBlobBuilder *textBlobBuilder, OH_Drawing_TextEncoding encoding) {
    const int count = OH_Drawing_FontCountText(font, text, byteLength, encoding);
    if (count < 1) {
        return nullptr;
    };
    OH_Drawing_TextBlobBuilderAllocRunPos(textBlobBuilder, font, count, nullptr);

    //  font.textToGlyphs、font.getPos接口缺失

    return OH_Drawing_TextBlobCreateFromText(text, byteLength, font, encoding);
}

void SkBug_8955::OnTestFunction(OH_Drawing_Canvas *canvas) {
    OH_Drawing_Brush *brush = OH_Drawing_BrushCreate();

    OH_Drawing_Font *font = OH_Drawing_FontCreate();
    OH_Drawing_FontSetTextSize(font, float(50));

    OH_Drawing_TextBlobBuilder *textBlobBuilder = OH_Drawing_TextBlobBuilderCreate();

    auto blob = makeFromText("+", 1, font, textBlobBuilder, TEXT_ENCODING_UTF8);

    OH_Drawing_CanvasSave(canvas);

    OH_Drawing_CanvasScale(canvas, 0, 0);

    OH_Drawing_CanvasAttachBrush(canvas, brush);

    OH_Drawing_CanvasDrawTextBlob(canvas, blob, 30, 60);
    OH_Drawing_CanvasRestore(canvas);
    OH_Drawing_CanvasDrawTextBlob(canvas, blob, 30, 60);

    OH_Drawing_CanvasDetachBrush(canvas);
    OH_Drawing_FontDestroy(font);
    OH_Drawing_TextBlobBuilderDestroy(textBlobBuilder);
    OH_Drawing_BrushDestroy(brush);
}
