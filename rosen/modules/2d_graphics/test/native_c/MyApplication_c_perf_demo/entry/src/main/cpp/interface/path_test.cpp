#include "path_test.h"
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_matrix.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_round_rect.h>
#include <native_drawing/drawing_shader_effect.h>
#include <native_drawing/drawing_point.h>
#include <native_drawing/drawing_image.h>
#include <native_drawing/drawing_filter.h>
#include <native_drawing/drawing_sampling_options.h>

#include "test_common.h"
#include "common/log_common.h"

void PathSetFillType::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    TestRend rand;
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    for (int i = 0; i < testCount_; i++) {
        float l = rand.nextULessThan(bitmapWidth_);
        float t = rand.nextULessThan( bitmapHeight_);
        float r = l + rand.nextULessThan(bitmapWidth_);
        float b = t + rand.nextULessThan(bitmapHeight_);
        OH_Drawing_PathAddRect(path, l, t, r, b, OH_Drawing_PathDirection::PATH_DIRECTION_CCW);        
        OH_Drawing_PathSetFillType(path, fType);
        OH_Drawing_CanvasDrawPath(canvas, path);
        OH_Drawing_PathReset(path);
    }
    OH_Drawing_PathDestroy(path);
}

void PathGetLength::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    TestRend rand;
    float length = 0;
    float x,y;
    
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    for (int i = 0; i < testCount_; i++) {         
        OH_Drawing_PathMoveTo(path, rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_));
        OH_Drawing_PathLineTo(path, rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_));
        length = OH_Drawing_PathGetLength(path, bClosed);
        OH_Drawing_CanvasDrawPath(canvas, path);
        OH_Drawing_PathReset(path);
        DRAWING_LOGI("OnTestFunction length = %{public}f",length);
    }
    OH_Drawing_PathDestroy(path);
}

void PathClose::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    TestRend rand;
    float length = 0;
    float x,y;
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    for (int i = 0; i < testCount_; i++) {         
        OH_Drawing_PathMoveTo(path, rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_));
        OH_Drawing_PathLineTo(path, rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_));
        OH_Drawing_PathLineTo(path, rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_));
        OH_Drawing_PathClose(path);
        OH_Drawing_CanvasDrawPath(canvas, path);
        OH_Drawing_PathReset(path);
    }
    OH_Drawing_PathDestroy(path);
}

void PathOffset::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    TestRend rand;
    float length = 0;
    float x,y;
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    OH_Drawing_Path* pathDst = OH_Drawing_PathCreate();
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_PathMoveTo(path, rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_));
        OH_Drawing_PathLineTo(path, rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_));
        OH_Drawing_PathLineTo(path, rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_));
        OH_Drawing_PathOffset(path, pathDst, rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_));
        OH_Drawing_PathOffset(path, pathDst, rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_));        
        OH_Drawing_CanvasDrawPath(canvas, pathDst);
        OH_Drawing_PathReset(path);
        OH_Drawing_PathReset(pathDst);
    }
    OH_Drawing_PathDestroy(pathDst);
    OH_Drawing_PathDestroy(path);
}

void PathReset::OnTestPerformance(OH_Drawing_Canvas *canvas) {
    TestRend rand;
    float length = 0;
    float x,y;
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_PathMoveTo(path, rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_));
        OH_Drawing_PathLineTo(path, rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_));
        OH_Drawing_PathLineTo(path, rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_));
        OH_Drawing_PathLineTo(path, rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_));
        OH_Drawing_PathLineTo(path, rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_));          
        OH_Drawing_CanvasDrawPath(canvas, path);
        OH_Drawing_PathReset(path);
    }
    OH_Drawing_PathDestroy(path);
}

void PathCubicTo::OnTestPerformance(OH_Drawing_Canvas *canvas) {
    TestRend rand;
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_PathMoveTo(path, rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_));
        OH_Drawing_PathCubicTo(path, rand.nextULessThan(bitmapWidth_),rand.nextULessThan(bitmapHeight_),rand.nextULessThan(bitmapWidth_), 
                                        rand.nextULessThan(bitmapHeight_),rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_));
        OH_Drawing_CanvasDrawPath(canvas, path);
        OH_Drawing_PathReset(path);
    }
    OH_Drawing_PathDestroy(path);
}

void PathRMoveTo::OnTestPerformance(OH_Drawing_Canvas *canvas) {
    TestRend rand;
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_PathMoveTo(path, rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_));
        OH_Drawing_PathRMoveTo(path, rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_));
        OH_Drawing_PathRLineTo(path, rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_));
        OH_Drawing_CanvasDrawPath(canvas, path);
        OH_Drawing_PathReset(path);
    }
    OH_Drawing_PathDestroy(path);
}

void PathRLineTo::OnTestPerformance(OH_Drawing_Canvas *canvas) {
    TestRend rand;
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_PathMoveTo(path, rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_));
        OH_Drawing_PathRLineTo(path, rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_));
        OH_Drawing_CanvasDrawPath(canvas, path);
        OH_Drawing_PathReset(path);
    }
    OH_Drawing_PathDestroy(path);
}

void PathRQuadTo::OnTestPerformance(OH_Drawing_Canvas *canvas) {
    TestRend rand;
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_PathMoveTo(path, rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_));
        OH_Drawing_PathRQuadTo(path, rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_), 
                                    rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_));
        OH_Drawing_CanvasDrawPath(canvas, path);
        OH_Drawing_PathReset(path);
    }
    OH_Drawing_PathDestroy(path);
}

void PathRConicTo::OnTestPerformance(OH_Drawing_Canvas *canvas) {
    TestRend rand;
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_PathMoveTo(path, rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_));
        OH_Drawing_PathRConicTo(path, rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_), 
                                        rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_),rand.nextULessThan(10));
        OH_Drawing_CanvasDrawPath(canvas, path);
        OH_Drawing_PathReset(path);
    }
    OH_Drawing_PathDestroy(path);
}

void PathRCubicTo::OnTestPerformance(OH_Drawing_Canvas *canvas) {
    TestRend rand;
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_PathMoveTo(path, rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_));
        OH_Drawing_PathRCubicTo(path, rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_), 
                                        rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_),
                                            rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_));
        OH_Drawing_CanvasDrawPath(canvas, path);
        OH_Drawing_PathReset(path);
    }
    OH_Drawing_PathDestroy(path);
}

void PathAddRect::OnTestPerformance(OH_Drawing_Canvas *canvas) {
    TestRend rand;

    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_PathAddRect(path, rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_),
                                    rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_), PATH_DIRECTION_CCW);
        OH_Drawing_CanvasDrawPath(canvas, path);
        OH_Drawing_PathReset(path);
    }
    OH_Drawing_PathDestroy(path);
}

void PathAddRectWithInitialCorner::OnTestPerformance(OH_Drawing_Canvas *canvas) {
    TestRend rand;
    float l,t,r,b;
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    for (int i = 0; i < testCount_; i++) {
        l = rand.nextULessThan(bitmapWidth_);
        t = rand.nextULessThan( bitmapHeight_);
        r = l + rand.nextULessThan(bitmapWidth_);
        b = t + rand.nextULessThan(bitmapHeight_);
        OH_Drawing_Rect* rc= OH_Drawing_RectCreate(l, t, r, b);
        OH_Drawing_PathAddRectWithInitialCorner(path, rc, OH_Drawing_PathDirection::PATH_DIRECTION_CCW, 0);
        OH_Drawing_CanvasDrawPath(canvas, path);
        OH_Drawing_PathReset(path);
        OH_Drawing_RectDestroy(rc);
    }
    OH_Drawing_PathDestroy(path);
}

void PathAddRoundRect::OnTestPerformance(OH_Drawing_Canvas *canvas) {
    TestRend rand;
    float l,t,r,b;
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    for (int i = 0; i < testCount_; i++) {
        l = rand.nextULessThan(bitmapWidth_);
        t = rand.nextULessThan( bitmapHeight_);
        r = l + rand.nextULessThan(bitmapWidth_);
        b = t + rand.nextULessThan(bitmapHeight_);
        OH_Drawing_Rect* rc= OH_Drawing_RectCreate(l, t, r, b);
        OH_Drawing_RoundRect* rrc = OH_Drawing_RoundRectCreate(rc, rand.nextULessThan(10), rand.nextULessThan(20));
        OH_Drawing_PathAddRoundRect(path, rrc, PATH_DIRECTION_CCW);
        OH_Drawing_CanvasDrawPath(canvas, path);
        OH_Drawing_PathReset(path);
        OH_Drawing_RectDestroy(rc);
        OH_Drawing_RoundRectDestroy(rrc);
    }
    OH_Drawing_PathDestroy(path);
}

void PathAddPath::OnTestPerformance(OH_Drawing_Canvas *canvas) {
    TestRend rand;
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    OH_Drawing_Path* srcPath = OH_Drawing_PathCreate();
    
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_PathMoveTo(srcPath, rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_));
        OH_Drawing_PathLineTo(srcPath, rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_));
        OH_Drawing_PathMoveTo(path, rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_));
        OH_Drawing_PathLineTo(path, rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_));    
        OH_Drawing_Matrix* metrix = OH_Drawing_MatrixCreateRotation(rand.nextULessThan(360),rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_));
        OH_Drawing_PathAddPath(path, srcPath,metrix);
        OH_Drawing_CanvasDrawPath(canvas, path);
        OH_Drawing_PathReset(path);
        OH_Drawing_PathReset(srcPath);
        OH_Drawing_MatrixDestroy(metrix);
    }
    OH_Drawing_PathDestroy(path);
    OH_Drawing_PathDestroy(srcPath);
}

void PathAddPathWithMode::OnTestPerformance(OH_Drawing_Canvas *canvas) {
    TestRend rand;
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    OH_Drawing_Path* srcPath = OH_Drawing_PathCreate();
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_PathMoveTo(srcPath, rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_));
        OH_Drawing_PathLineTo(srcPath, rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_));
        OH_Drawing_PathMoveTo(path, rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_));
        OH_Drawing_PathLineTo(path, rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_));        
        OH_Drawing_PathAddPathWithMode(path, srcPath, addMode);
        OH_Drawing_CanvasDrawPath(canvas, path);
        OH_Drawing_PathReset(path);
        OH_Drawing_PathReset(srcPath);
    }
    OH_Drawing_PathDestroy(path);
    OH_Drawing_PathDestroy(srcPath);
}

void PathAddPathWithOffsetAndMode::OnTestPerformance(OH_Drawing_Canvas *canvas) {
    TestRend rand;
    float l,t,r,b;
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    OH_Drawing_Path* srcPath = OH_Drawing_PathCreate();
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_PathMoveTo(srcPath, rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_));
        OH_Drawing_PathLineTo(srcPath, rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_));
        OH_Drawing_PathMoveTo(path, rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_));
        OH_Drawing_PathLineTo(path, rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_));  
        
        OH_Drawing_PathAddPathWithOffsetAndMode(path, srcPath, rand.nextULessThan(bitmapWidth_), rand.nextULessThan(bitmapHeight_), addMode);
        OH_Drawing_CanvasDrawPath(canvas, path);
        OH_Drawing_PathReset(path);
    }
    OH_Drawing_PathDestroy(path);
    OH_Drawing_PathDestroy(srcPath);
}
