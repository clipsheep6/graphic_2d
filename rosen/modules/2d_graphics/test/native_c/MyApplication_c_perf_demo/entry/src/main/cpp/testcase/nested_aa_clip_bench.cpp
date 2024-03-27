#include "nested_aa_clip_bench.h"
#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_rect.h>
#include <native_drawing/drawing_point.h>
#include <native_drawing/drawing_round_rect.h>
#include "test_common.h"  
#include <time.h> 
#include "common/log_common.h"

NestedAAClipBench::NestedAAClipBench() 
{
    fDrawRect = OH_Drawing_RectCreate(0,0,kImageSize,kImageSize);
    fSize[0].x = (float)kImageSize;
    fSize[0].y = (float)kImageSize;
    for (int i = 1; i < kNestingDepth+1; ++i) {
        fSize[i].x = fSize[i-1].x/2;
        fSize[i].y = fSize[i-1].y/2;
    }      
}

void NestedAAClipBench::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    DRAWING_LOGI("NestedAAClipBench::OnTestFunction");
    OH_Drawing_Point2D offset;
    offset.x = 0;
    offset.y = 0;
    //recurse
    this->recurse(canvas, 0, offset);
}

void NestedAAClipBench::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    //当前用例名 drawpathaddroundrect 测试 OH_Drawing_PathAddRoundRect  迁移基于skia AAClipBench.cpp->NestedAAClipBench
    // skia case name : nested_aaclip_AA
    // 创建一个画刷pen对象
  
    DRAWING_LOGI("NestedAAClipBench::OnTestPerformance");
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_Point2D offset;
        offset.x = 0;
        offset.y = 0;
        this->recurse(canvas, 0, offset);
    }
}

void NestedAAClipBench::recurse(OH_Drawing_Canvas* canvas,int depth,const OH_Drawing_Point2D& offset)
{
    OH_Drawing_CanvasSave(canvas);
    OH_Drawing_Rect* temp = OH_Drawing_RectCreate(offset.x,offset.y,fSize[depth].x+offset.x,fSize[depth].y+offset.y);
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    OH_Drawing_RoundRect *roundRect = OH_Drawing_RoundRectCreate(temp, 10.0, 10.0);
    OH_Drawing_PathAddRoundRect(path, roundRect, OH_Drawing_PathDirection::PATH_DIRECTION_CCW);
    OH_Drawing_CanvasClipPath(canvas, path, OH_Drawing_CanvasClipOp::INTERSECT, fDoAA);
    if(kNestingDepth == depth)
    {
        OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
        OH_Drawing_BrushSetColor(brush, 0xFF000000|fRandom.nextU());
        OH_Drawing_BrushSetAntiAlias(brush,true);
        OH_Drawing_CanvasAttachBrush(canvas, brush);
        OH_Drawing_CanvasDrawPath(canvas, path);
        OH_Drawing_CanvasDetachBrush(canvas);
        OH_Drawing_BrushDestroy(brush);
    } else {
        OH_Drawing_Point2D childOffset = offset;
        this->recurse(canvas, depth+1, childOffset);
        
        childOffset.x += fSize[depth+1].x;
        childOffset.y += fSize[depth+1].y;
        this->recurse(canvas, depth+1,  childOffset);
        
        childOffset.x = offset.x+fSize[depth+1].x;
        childOffset.y = offset.y;
        this->recurse(canvas,depth+1,childOffset);
        
        childOffset.x = offset.x;
        childOffset.y = offset.y + fSize[depth+1].y;
        this->recurse(canvas,depth+1,childOffset);
    }
    OH_Drawing_PathDestroy(path);
    OH_Drawing_RectDestroy(temp);
    OH_Drawing_RoundRectDestroy(roundRect);
    OH_Drawing_CanvasRestore(canvas);
}
