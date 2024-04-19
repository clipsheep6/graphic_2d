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
    
    for (int i = 0; i < testCount_; i++) {
        float l = rand.nextULessThan(bitmapWidth_);
        float t = rand.nextULessThan( bitmapHeight_);
        float r = l + rand.nextULessThan(bitmapWidth_);
        float b = t + rand.nextULessThan(bitmapHeight_);
        OH_Drawing_Path* path = OH_Drawing_PathCreate();
        OH_Drawing_PathAddRect(path, l, t, r, b, OH_Drawing_PathDirection::PATH_DIRECTION_CCW);        
        OH_Drawing_PathSetFillType(path, fType);
        OH_Drawing_CanvasDrawPath(canvas, path);
        OH_Drawing_PathDestroy(path);
    }
    
}

void PathGetLength::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    TestRend rand;
    float length = 0;
    float x,y;
    
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_Path* path = OH_Drawing_PathCreate();
        OH_Drawing_PathMoveTo(path, 0,0);
        x = rand.nextULessThan(bitmapWidth_);
        y = rand.nextULessThan(bitmapHeight_);        
        OH_Drawing_PathLineTo(path, x, y);
        length = OH_Drawing_PathGetLength(path, bClosed);
        OH_Drawing_CanvasDrawPath(canvas, path);
//        DRAWING_LOGI("OnTestFunction length = %{public}f",length);
        OH_Drawing_PathDestroy(path);
    }
    
}

void PathClose::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    TestRend rand;
    float length = 0;
    float x,y;
    
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_Path* path = OH_Drawing_PathCreate();
        OH_Drawing_PathMoveTo(path, 0,0);
        x = rand.nextULessThan(bitmapWidth_);
        y = rand.nextULessThan(bitmapHeight_);        
        OH_Drawing_PathLineTo(path, x, y);

        OH_Drawing_PathClose(path);
        OH_Drawing_CanvasDrawPath(canvas, path);
        OH_Drawing_PathDestroy(path);
    }
    
}

void PathOffset::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    TestRend rand;
    float length = 0;
    float x,y;
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_PathMoveTo(path, 0,0);
        x = rand.nextULessThan(bitmapWidth_);
        y = rand.nextULessThan(bitmapHeight_);        
        OH_Drawing_PathLineTo(path, x, y);

        x = rand.nextULessThan(bitmapWidth_);
        y = rand.nextULessThan(bitmapHeight_);     
        OH_Drawing_PathOffset(path, path, x, y);
        OH_Drawing_CanvasDrawPath(canvas, path);
    }
    OH_Drawing_PathDestroy(path);
}

void PathReset::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    TestRend rand;
    float length = 0;
    float x,y;
    
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_Path* path = OH_Drawing_PathCreate();
        OH_Drawing_PathMoveTo(path, 0,0);
        x = rand.nextULessThan(bitmapWidth_);
        y = rand.nextULessThan(bitmapHeight_);        
        OH_Drawing_PathLineTo(path, x, y);
        
        OH_Drawing_CanvasDrawPath(canvas, path);
        OH_Drawing_PathReset(path);
        OH_Drawing_PathDestroy(path);
    }
    
}