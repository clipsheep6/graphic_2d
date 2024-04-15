#include "path_interior.h"
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_matrix.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_round_rect.h>
#include <native_drawing/drawing_round_rect.h>
#include <native_drawing/drawing_shader_effect.h>
#include <native_drawing/drawing_point.h>
#include <rdma/bnxt_re-abi.h>
#include "test_common.h"
#include "common/log_common.h"

enum{
    kW=770,
    kH=770,
};

namespace  {
    void show(OH_Drawing_Canvas* canvas,OH_Drawing_Path* path)
    {
        OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
        OH_Drawing_BrushSetAntiAlias(brush, true);
        bool hasInterior = false;
        OH_Drawing_BrushSetColor(brush, hasInterior?color_to_565(0xFF8888FF):0xFF888888);//gray
        OH_Drawing_CanvasAttachBrush(canvas, brush);
        OH_Drawing_CanvasDrawPath(canvas, path);
    
        OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
        OH_Drawing_PenSetAntiAlias(pen, true);
        OH_Drawing_PenSetColor(pen, 0xFFFF0000);
        OH_Drawing_CanvasAttachPen(canvas, pen);
        OH_Drawing_CanvasDrawPath(canvas, path);
    
        OH_Drawing_CanvasDetachPen(canvas);
        OH_Drawing_CanvasDetachBrush(canvas);
        OH_Drawing_BrushDestroy(brush);
        OH_Drawing_PenDestroy(pen);
    }

    DrawRect inset(DrawRect& r){
        DrawRect rect = r;
        rect.inset(r.width()/8, r.height()/8);
        return rect;
    }
}

PathInterior::PathInterior() {
    //skia dm file gm/fontregen.cpp
    bitmapWidth_ = kW;
    bitmapHeight_ = kH;
    fileName_ = "pathinterior";
}


void PathInterior::OnTestFunction(OH_Drawing_Canvas* canvas)
{
// DRAWING_LOGI("OnTestFunction path = %{public}s",fileName_);
    OH_Drawing_CanvasClear(canvas, 0xFFDDDDDD);//gray
    OH_Drawing_CanvasTranslate(canvas, 8.5f, 8.5f);
    DrawRect rect = {0,0,80,80};
    const float RAD = rect.width()/8;
    
    int i = 0;
    for (int insetFirst = 0; insetFirst <= 1; ++insetFirst) {
        for (int doEvenOdd = 0; doEvenOdd <= 1; ++doEvenOdd) {
            for (int outerRR = 0; outerRR <= 1; ++outerRR) {
                for (int innerRR = 0; innerRR <= 1; ++innerRR) {
                    for (int outerCW = 0; outerCW <= 1; ++outerCW) {
                        for (int innerCW = 0; innerCW <= 1; ++innerCW) {
                            OH_Drawing_Path *path = OH_Drawing_PathCreate();
                            OH_Drawing_PathSetFillType(path, doEvenOdd ? PATH_FILL_TYPE_EVEN_ODD : PATH_FILL_TYPE_WINDING);
                            OH_Drawing_PathDirection outerDir = outerCW ? PATH_DIRECTION_CW : PATH_DIRECTION_CCW;
                            OH_Drawing_PathDirection innerDir = innerCW ? PATH_DIRECTION_CW : PATH_DIRECTION_CCW;
                            
                            DrawRect rc = insetFirst ? inset(rect) : rect;
                            OH_Drawing_Rect* r = OH_Drawing_RectCreate(rc.fLeft, rc.fTop, rc.fRight, rc.fBottom);
                            OH_Drawing_RoundRect* rr = OH_Drawing_RoundRectCreate(r, RAD, RAD);
                            if (outerRR) {
                                OH_Drawing_PathAddRoundRect(path, rr, outerDir);
                            } else {
                                OH_Drawing_PathAddRect(path, rc.fLeft, rc.fTop, rc.fRight, rc.fBottom, outerDir);
                            }
                            OH_Drawing_RoundRectDestroy(rr);
                            OH_Drawing_RectDestroy(r);

                            rc = insetFirst ? rect : inset(rect);
                            r = OH_Drawing_RectCreate(rc.fLeft, rc.fTop, rc.fRight, rc.fBottom);
                            rr = OH_Drawing_RoundRectCreate(r, RAD, RAD);
                            if (innerRR) {
                                OH_Drawing_PathAddRoundRect(path, rr, innerDir);
                            } else {
                                OH_Drawing_PathAddRect(path, rc.fLeft, rc.fTop, rc.fRight, rc.fBottom, innerDir);
                            }
                            OH_Drawing_RoundRectDestroy(rr);
                            OH_Drawing_RectDestroy(r);         
                            
                            float dx = (i / 8) * rect.width() * 6 / 5;
                            float dy = (i % 8) * rect.height() * 6 / 5;
                            i++;
                            OH_Drawing_PathOffset(path, path, dx, dy);
                            show(canvas, path);
                            OH_Drawing_PathDestroy(path);
                        }
                    }
                }
            }
        }
    }
}
