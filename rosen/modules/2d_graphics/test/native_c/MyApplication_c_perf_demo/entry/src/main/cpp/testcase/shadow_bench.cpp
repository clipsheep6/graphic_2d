#include "shadow_bench.h"
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_rect.h>
#include <native_drawing/drawing_canvas.h>
#include <native_drawing/drawing_round_rect.h>
#include <time.h>
#include "common/log_common.h"

#include "test_common.h"
enum {
    kWidth = 640,
    kHeight = 480,
    kRRSize = 50,
    kRRRadius = 6,
    kRRSpace = 8,
    kRRStep = kRRSize + kRRSpace,
    kElevation = 16,
    kNumRRects = ((kWidth - kRRSpace) / kRRStep)*((kHeight - kRRSpace) / kRRStep)
};

OH_Drawing_Path* rRectPaths[kNumRRects];
void ShadowBench::OnTestFunction(OH_Drawing_Canvas* canvas)
{
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetColor(pen, 0xFFFFFFFF);
    OH_Drawing_PenSetAntiAlias(pen,true);
    OH_Drawing_CanvasAttachPen(canvas, pen);
    
    OH_Drawing_Path* path = nullptr;
    OH_Drawing_Point3D planeParams = {0,0,kElevation};
    OH_Drawing_Point3D lightPos = {270,0,600};
    float lightRadius = 800;
    uint32_t ambientColor = 0x19000000;
    uint32_t spotColor = 0x40000000;
    uint32_t flags = 0;
    if(fTransparent)
        flags |=  OH_Drawing_CanvasShadowFlags::SHADOW_FLAGS_TRANSPARENT_OCCLUDER;
    if(fForceGeometric)
        flags |= OH_Drawing_CanvasShadowFlags::SHADOW_FLAGS_GEOMETRIC_ONLY;
    genRRects();

    path = rRectPaths[0];
    if(path != nullptr)
        OH_Drawing_CanvasDrawShadow(canvas,path,planeParams,lightPos,lightRadius,ambientColor,spotColor,(OH_Drawing_CanvasShadowFlags)flags);

    destoryRRects();
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_PenDestroy(pen);
    pen = nullptr;
}

void ShadowBench::OnTestPerformance(OH_Drawing_Canvas* canvas)
{   
    //当前用例名 drawcanvasdrawshadow 测试 OH_Drawing_CanvasDrawShadow  迁移基于skia ShadowBench.cpp->ShadowBench
    // skia case name : shadows_o_a
    // 创建一个画刷pen对象
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetColor(pen, 0xFFFFFFFF);
    OH_Drawing_PenSetAntiAlias(pen,true);
    OH_Drawing_CanvasAttachPen(canvas, pen);
    
    OH_Drawing_Path* path = nullptr;
    OH_Drawing_Point3D planeParams = {0,0,kElevation};
    OH_Drawing_Point3D lightPos = {270,0,600};
    float lightRadius = 800;
    uint32_t ambientColor = 0x19000000;
    uint32_t spotColor = 0x40000000;
    uint32_t flags = 0;
    if(fTransparent)
        flags |=  OH_Drawing_CanvasShadowFlags::SHADOW_FLAGS_TRANSPARENT_OCCLUDER;
    if(fForceGeometric)
        flags |= OH_Drawing_CanvasShadowFlags::SHADOW_FLAGS_GEOMETRIC_ONLY;
    genRRects();
    for (int i = 0; i < testCount_; i++) {
        path = rRectPaths[i%kNumRRects];
        if(path != nullptr)
            OH_Drawing_CanvasDrawShadow(canvas,path,planeParams,lightPos,lightRadius,ambientColor,spotColor,(OH_Drawing_CanvasShadowFlags)flags);
    }
    destoryRRects();
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_PenDestroy(pen);
    pen = nullptr;
}

void ShadowBench::genRRects()
{
    int i = 0;
    for (int x = kRRSpace; x < kWidth - kRRStep; x += kRRStep) {
        for (int y = kRRSpace; y < kHeight - kRRStep; y += kRRStep) {
            OH_Drawing_Path* path = OH_Drawing_PathCreate();
            OH_Drawing_RoundRect* rr = OH_Drawing_RoundRectCreate(OH_Drawing_RectCreate(x, y, x+kRRSize, y+kRRSize), kRRRadius, kRRRadius);
            OH_Drawing_PathAddRoundRect(path, rr, OH_Drawing_PathDirection::PATH_DIRECTION_CCW);
            rRectPaths[i] = path;
            ++i;
        }
    }
}

void ShadowBench::destoryRRects()
{
    for(int i=0;i<kNumRRects;i++)
    {
        OH_Drawing_Path* path = rRectPaths[i];
        if(path != nullptr)
            OH_Drawing_PathDestroy(path);
    }
}