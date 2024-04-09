#include "dashing.h"
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_matrix.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_round_rect.h>
#include <native_drawing/drawing_round_rect.h>
#include <native_drawing/drawing_shader_effect.h>
#include <native_drawing/drawing_point.h>
#include <native_drawing/drawing_path_effect.h>
#include "test_common.h"
#include "common/log_common.h"

enum{
    kW=640,
    kH=340,
};


static void drawline(OH_Drawing_Canvas* canvas, int on, int off, OH_Drawing_Pen* pen,
                     float finalX = (600), float finalY = (0),
                     float phase = (0),
                     float startX = (0), float startY = (0)) {

    float intervals[] = {
        float(on),
        float(off),
    };
    OH_Drawing_PathEffect* effect= OH_Drawing_CreateDashPathEffect(intervals,2,phase);
    OH_Drawing_PenSetPathEffect(pen, effect);
    OH_Drawing_CanvasAttachPen(canvas, pen);
    OH_Drawing_CanvasDrawLine(canvas, startX, startY, finalX, finalY);

}

static void show_zero_len_dash(OH_Drawing_Canvas* canvas) {
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    drawline(canvas, 2, 2, pen, (0));
    OH_Drawing_PenSetWidth(pen, 2);
    OH_Drawing_CanvasTranslate(canvas, 0, 20);
    drawline(canvas, 4, 4, pen, (0));
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_PenDestroy(pen);
}

static void show_giant_dash(OH_Drawing_Canvas* canvas) {
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    drawline(canvas, 1, 1, pen, (20 * 1000));
    OH_Drawing_PenDestroy(pen);
}

Dashing::Dashing() {
    //skia dm file gm/fontregen.cpp
    bitmapWidth_ = kW;
    bitmapHeight_ = kH;
    fileName_ = "dashing";
}

void Dashing::OnTestFunction(OH_Drawing_Canvas* canvas)
{
// DRAWING_LOGI("OnTestFunction path = %{public}s",fileName_);
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_CanvasTranslate(canvas, 20, 20);
    OH_Drawing_CanvasTranslate(canvas, 0, 0.5);
    struct Intervals {
        int fOnInterval;
        int fOffInterval;
    };
    for (int width = 0; width <= 2; ++width) {
        for (const Intervals& data : {Intervals{1, 1},Intervals{4, 1}}){
            for(bool aa:{false,true}){
                int w = width * width * width;
                OH_Drawing_PenSetAntiAlias(pen, aa);
                OH_Drawing_PenSetWidth(pen, w);
                int scale = w ? w : 1;
                drawline(canvas, data.fOnInterval * scale, data.fOffInterval * scale,pen);
                OH_Drawing_CanvasTranslate(canvas, 0, 20);
            }
        }
    }
    show_giant_dash(canvas);
    OH_Drawing_CanvasTranslate(canvas, 0, 20);
    show_zero_len_dash(canvas);
    OH_Drawing_CanvasTranslate(canvas, 0, 20);
    OH_Drawing_PenSetWidth(pen, 8);
    drawline(canvas, 0, 0, pen);
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_PenDestroy(pen);
}


void make_path_line(OH_Drawing_Path* path,DrawRect rect)
{
    OH_Drawing_PathMoveTo(path, rect.fLeft, rect.fTop);
    OH_Drawing_PathLineTo(path, rect.fRight,  rect.fBottom);
}

void make_path_rect(OH_Drawing_Path* path,DrawRect rect)
{
    OH_Drawing_PathAddRect(path, rect.fLeft, rect.fTop, rect.fRight,  rect.fBottom, OH_Drawing_PathDirection::PATH_DIRECTION_CCW);
}

void make_path_oval(OH_Drawing_Path* path,DrawRect rect)
{
    OH_Drawing_PathAddOvalWithInitialPoint(path, OH_Drawing_RectCreate(rect.fLeft, rect.fTop, rect.fRight,  rect.fBottom),0, OH_Drawing_PathDirection::PATH_DIRECTION_CCW);
}

void make_path_star(OH_Drawing_Path* path,DrawRect rect)
{
    int n = 5;//star corner count
    float rad = -M_PI_2;
    const float drad = (n >> 1) * M_PI * 2 / n;

    OH_Drawing_PathMoveTo(path, 0, -1.0);
    for (int i = 1; i < n; i++) {
        rad += drad;
        OH_Drawing_PathLineTo(path,cos(rad), sin(rad));
    }    
//matrix  rect to rect not realize
//SkMatrix matrix = SkMatrix::RectToRect(path.getBounds(), bounds, SkMatrix::kCenter_ScaleToFit);
}
Dashing2::Dashing2() {
    //skia dm file gm/fontregen.cpp
    bitmapWidth_ = kW;
    bitmapHeight_ = kH;
    fileName_ = "dashing2";
}

void Dashing2::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    constexpr int gIntervals[] = {
        3,  // 3 dashes: each count [0] followed by intervals [1..count]
        2,  10, 10,
        4,  20, 5, 5, 5,
        2,  2, 2
    };    
    
    void (*gProc[])(OH_Drawing_Path* path,DrawRect rect) = {make_path_line, make_path_rect, make_path_oval, make_path_star,};
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetAntiAlias(pen, true);
    OH_Drawing_PenSetWidth(pen, 6.0);
    
    DrawRect bounds = {0, 0,120,120};
    bounds.offset(20, 20);
    float dx = bounds.width()*4.0/3;
    float dy = bounds.height()*4.0/3;
    
    const int* intervals = &gIntervals[1]; 
    for(int y=0; y<gIntervals[0];++y){
        float vals[sizeof(gIntervals)/sizeof(int)];
        int count = *intervals++;
        for(int i=0; i<count; ++i){
            vals[i] = *intervals++;   
        }
        float phase = vals[0]/2;
        OH_Drawing_PathEffect* dasheffect = OH_Drawing_CreateDashPathEffect(vals, count, phase);
        OH_Drawing_PenSetPathEffect(pen, dasheffect);
        OH_Drawing_CanvasAttachPen(canvas, pen);
        
        for(size_t x = 0; x < sizeof(gProc)/sizeof(gProc[0]); ++x){
            OH_Drawing_Path* path = OH_Drawing_PathCreate();
            DrawRect r = bounds;
            r.offset(x*dx, y*dy);
            gProc[x](path, r);
            OH_Drawing_CanvasDrawPath(canvas, path);
            OH_Drawing_PathDestroy(path);   
        }
    }
}