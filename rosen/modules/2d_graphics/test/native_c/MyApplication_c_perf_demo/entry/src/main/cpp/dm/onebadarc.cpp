#include "onebadarc.h"
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_rect.h>

enum {
    kW = 100,
    kH = 100,
};

OneBadArc::OneBadArc() {
    // skia dm file gm/onebadarc.cpp
    bitmapWidth_ = kW;
    bitmapHeight_ = kH;
    fileName_ = "onebadarc";
}

union FloatIntUnion {
    float fFloat;
    int32_t fSignBitInt;
};

static inline float Bits2Float(int32_t floatAsBits) {
    FloatIntUnion data;
    data.fSignBitInt = floatAsBits;
    return data.fFloat;
}

void OneBadArc::OnTestFunction(OH_Drawing_Canvas *canvas) {
    OH_Drawing_Path *path = OH_Drawing_PathCreate();
    OH_Drawing_PathMoveTo(path, Bits2Float(0x41a00000), Bits2Float(0x41a00000));
    OH_Drawing_PathLineTo(path, Bits2Float(0x4208918c), Bits2Float(0x4208918c));
    OH_Drawing_PathConicTo(path, Bits2Float(0x41a00000), Bits2Float(0x42412318),
                           Bits2Float(0x40bb73a0), Bits2Float(0x4208918c),
                           Bits2Float(0x3f3504f3));
    OH_Drawing_PathQuadTo(path, Bits2Float(0x40bb73a0), Bits2Float(0x4208918c),
                          Bits2Float(0x40bb73a2), Bits2Float(0x4208918c));
    OH_Drawing_PathLineTo(path, Bits2Float(0x41a00000), Bits2Float(0x41a00000));
    OH_Drawing_PathClose(path);

    OH_Drawing_Pen *pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetColor(pen, 0xFFFF0000);
    OH_Drawing_PenSetWidth(pen, 15.f);
    OH_Drawing_PenSetAlpha(pen, 100);
    OH_Drawing_CanvasAttachPen(canvas, pen);

    OH_Drawing_CanvasTranslate(canvas, 20, 0);

    OH_Drawing_CanvasDrawPath(canvas, path);

    OH_Drawing_Rect *rect = OH_Drawing_RectCreate(60, 0, 100, 40);
    OH_Drawing_CanvasDrawArc(canvas, rect, 45, 90);

    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_PenDestroy(pen);
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_PathDestroy(path);
}
