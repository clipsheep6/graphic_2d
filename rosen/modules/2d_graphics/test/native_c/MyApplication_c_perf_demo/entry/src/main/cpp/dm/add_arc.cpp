#include "add_arc.h"
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_matrix.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_round_rect.h>
#include <native_drawing/drawing_round_rect.h>
#include <native_drawing/drawing_shader_effect.h>
#include <native_drawing/drawing_point.h>
#include "test_common.h"
#include "common/log_common.h"
#include <math.h>
#include <unistd.h>
#include <math.h>

#define R   400

struct RECT{
    float x;
    float y;
    float w;
    float h;
    void inset(float dx,float dy)
    {
        x+=dx;
        y+=dy;
        w-=2*dx;
        h-=2*dy;
    }
} ;


AddArcMeas::AddArcMeas() {
    //skia dm file gm/addarc.cpp
    bitmapWidth_ = 2*R + 40;
    bitmapHeight_ = 2*R + 40;
    fileName_ = "addarc_meas";
}

AddArcMeas::~AddArcMeas() {

}

void AddArcMeas::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_CanvasTranslate(canvas, R+20, R+20);
    OH_Drawing_Pen *pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetAntiAlias(pen, true);
    OH_Drawing_CanvasAttachPen(canvas, pen);

    OH_Drawing_Pen *measPen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetAntiAlias(measPen, true);
    OH_Drawing_PenSetColor(measPen, 0xFFFF0000);
    
    OH_Drawing_Rect* oval = OH_Drawing_RectCreate(-R, -R, R, R);
    OH_Drawing_CanvasDrawOval(canvas, oval);
    
    for (float deg = 0; deg < 360; deg += 10) {
        const float rad = (deg)/182*M_PI;
        float rx = cos(rad) * R;
        float ry = sin(rad) * R;
        OH_Drawing_CanvasAttachPen(canvas, pen);
        OH_Drawing_CanvasDrawLine(canvas, 0, 0, rx, ry);

        OH_Drawing_Path* meas = OH_Drawing_PathCreate();
        OH_Drawing_PathAddArc(meas, oval, 0, deg);
        float arcLen = rad * R;
        
        OH_Drawing_CanvasAttachPen(canvas, measPen);
        OH_Drawing_CanvasDrawLine(canvas, 0, 0, rx, ry);//没有getPosTan工具函数，先用已有的point替代
//        SkPoint pos; 
//        if (meas.getPosTan(arcLen, &pos, nullptr)) {
//            canvas->drawLine({0, 0}, pos, measPaint);
//        }
        OH_Drawing_PathDestroy(meas);
    }
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_PenDestroy(pen);
    OH_Drawing_PenDestroy(measPen);
    OH_Drawing_RectDestroy(oval);
    pen = nullptr;
    measPen = nullptr;
}

AddArc::AddArc()
{
    bitmapWidth_ = 1040;
    bitmapHeight_ = 1040;
    fileName_ = "addarc";    
}

void AddArc::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_CanvasTranslate(canvas, 20, 20);
    RECT rc = {0, 0, 1000, 1000};
 
    OH_Drawing_Pen *pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetAntiAlias(pen, true);
    OH_Drawing_PenSetWidth(pen, 15);
    
    const float inset = OH_Drawing_PenGetWidth(pen)+4;
    const float sweepAngle = 345;
    TestRend rand;
    TestRend randRotate;
    float sign = 1;
    
    while (rc.w > OH_Drawing_PenGetWidth(pen)*3)
    {
        OH_Drawing_PenSetColor(pen, color_to_565(rand.nextU()|(0xFF<<24)));
        OH_Drawing_CanvasAttachPen(canvas, pen);
        float startAngle = rand.nextUScalar1()*360;
        float speed = sqrtf(16/rc.w)*0.5f;
        fRotate = randRotate.nextRangeF(1, 360);//mock skia dm onAnimate behavior
        startAngle += fRotate * 360 * speed * sign;
        OH_Drawing_Path* path = OH_Drawing_PathCreate();
        OH_Drawing_Rect* r = OH_Drawing_RectCreate(rc.x,rc.y,rc.x+rc.w,rc.y+rc.h);
        OH_Drawing_PathAddArc(path,r,startAngle, sweepAngle);
        OH_Drawing_CanvasDrawPath(canvas, path);
        rc.inset(inset, inset);
        sign = -sign;
        OH_Drawing_RectDestroy(r);
        OH_Drawing_PathDestroy(path);
    }
    OH_Drawing_PenDestroy(pen);
    pen = nullptr;
}
