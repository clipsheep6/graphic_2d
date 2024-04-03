#include "clipped_cubic.h"
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_rect.h>
#include <native_drawing/drawing_brush.h>

enum {
    kW = 1240,
    kH = 390,
};

ClippedCubic::ClippedCubic() {
    // skia dm file gm/cubicpaths.cpp
    bitmapWidth_ = kW;
    bitmapHeight_ = kH;
    fileName_ = "clippedcubic";
}

void ClippedCubic::OnTestFunction(OH_Drawing_Canvas *canvas) {

    OH_Drawing_Path *path = OH_Drawing_PathCreate();
    OH_Drawing_PathMoveTo(path, float(0), float(0));
    OH_Drawing_PathCubicTo(path, float(140), float(150), float(40), float(10), float(170), float(150));

    OH_Drawing_Rect *bounds = OH_Drawing_RectCreate(float(0), float(0), float(170), float(150));

    OH_Drawing_Brush *brush = OH_Drawing_BrushCreate();
    OH_Drawing_CanvasAttachBrush(canvas, brush);

    for (float dy = -1; dy <= 1; dy += 1) {
        OH_Drawing_CanvasSave(canvas);
        for (float dx = -1; dx <= 1; dx += 1) {
            OH_Drawing_CanvasSave(canvas);

            OH_Drawing_CanvasClipRect(canvas, bounds, INTERSECT, true);

            OH_Drawing_CanvasTranslate(canvas, dx, dy);

            OH_Drawing_CanvasDrawPath(canvas, path);

            OH_Drawing_CanvasRestore(canvas);

            OH_Drawing_CanvasTranslate(canvas, 170, 0);
        }
        OH_Drawing_CanvasRestore(canvas);
        OH_Drawing_CanvasTranslate(canvas, 0, 150);
    }

    OH_Drawing_CanvasDetachBrush(canvas);
    OH_Drawing_BrushDestroy(brush);
    OH_Drawing_RectDestroy(bounds);
    OH_Drawing_PathDestroy(path);
}
