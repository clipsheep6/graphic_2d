#include "draw_path_add_round_rect.h"
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_rect.h>
#include <native_drawing/drawing_point.h>
#include <native_drawing/drawing_round_rect.h>
#include "test_common.h"  
#include <time.h> 
#include "common/log_common.h"

void DrawPathAddRoundRect::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    // 创建一个画刷pen对象
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetColor(pen, 0xFF000000);//OH_Drawing_ColorSetArgb(0xFF, 0x00, 0x00, 0x00));
    OH_Drawing_PenSetAntiAlias(pen,true);
    OH_Drawing_CanvasAttachPen(canvas, pen);
    

    OH_Drawing_PenDestroy(pen);
    pen = nullptr;
}

void DrawPathAddRoundRect::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    //当前用例名 drawpathroundrect 测试 OH_Drawing_PathAddRoundRect  迁移基于skia AAClipBench.cpp->NestedAAClipBench

    // 创建一个画刷pen对象
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetColor(pen, 0xFF00FF00);
    OH_Drawing_PenSetAntiAlias(pen,true);
    OH_Drawing_CanvasAttachPen(canvas, pen);

    fRandom = TestRend(time(NULL));
    fDrawRect = OH_Drawing_RectCreate(0,0,kImageSize,kImageSize);
    fSize[0].x = (float)kImageSize;
    fSize[0].y = (float)kImageSize;
    for (int i = 1; i < kNestingDepth+1; ++i) {
        fSize[i].x = fSize[i-1].x/2;
        fSize[i].y = fSize[i-1].y/2;
    }   
    DRAWING_LOGI("DrawPathAddRoundRect::OnTestPerformance");
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_Point2D offset;
        offset.x = 0;
        offset.y = 0;
        //recurse
        this->recurse(canvas, 0, offset);
    }
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_PenDestroy(pen);
}

void DrawPathAddRoundRect::recurse(OH_Drawing_Canvas* canvas,int depth,const OH_Drawing_Point2D& offset)
{
    OH_Drawing_CanvasSave(canvas);
    OH_Drawing_Rect* temp = OH_Drawing_RectCreate(0,0,fSize[depth].x,fSize[depth].y);
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    OH_Drawing_RoundRect *roundRect = OH_Drawing_RoundRectCreate(temp, 3.0, 3.0);
    OH_Drawing_PathAddRoundRect(path, roundRect, OH_Drawing_PathDirection::PATH_DIRECTION_CCW);
    
    OH_Drawing_CanvasClipPath(canvas, path, OH_Drawing_CanvasClipOp::INTERSECT, fDoAA);
//    DRAWING_LOGI("drawpathroundrect  offset x = %{public}f ,y = %{public}f\n",offset.x,offset.y);
    if(kNestingDepth == depth)
    {
        OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
        OH_Drawing_PenSetColor(pen, 0xFF000000|fRandom.nextU());
        OH_Drawing_PenSetAntiAlias(pen,true);
        OH_Drawing_CanvasAttachPen(canvas, pen);
        OH_Drawing_CanvasDrawRect(canvas, fDrawRect);

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
    OH_Drawing_CanvasRestore(canvas);
}
