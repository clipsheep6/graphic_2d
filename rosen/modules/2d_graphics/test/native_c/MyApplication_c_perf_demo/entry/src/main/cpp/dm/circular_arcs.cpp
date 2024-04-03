#include "circular_arcs.h"
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
#include <vector>

enum{
    kW=820,
    kH=1090,
};
CircularArcStrokeMatrix::CircularArcStrokeMatrix() {
    //skia dm file gm/circulararcs.cpp
    bitmapWidth_ = kW;
    bitmapHeight_ = kH;
    fileName_ = "circular_arc_stroke_matrix";
}


void CircularArcStrokeMatrix::OnTestFunction(OH_Drawing_Canvas* canvas)
{
// DRAWING_LOGI("OnTestFunction path = %{public}s",fileName_);
    static constexpr float kRadius = 40.f;
    static constexpr float kStrokeWidth = 5.f;
    static constexpr float kStart = 89.f;
    static constexpr float kSweep = 180.f/M_PI; // one radian
    
    std::vector<OH_Drawing_Matrix*> matrices;
    
    matrices.push_back(OH_Drawing_MatrixCreateRotation(kRadius, kRadius, 45.f));
    
    OH_Drawing_Matrix* mI = OH_Drawing_MatrixCreate();
    OH_Drawing_MatrixSetMatrix(mI,1,0,0 ,0,1,0, 0,0,1);
    matrices.push_back(mI);
    
    OH_Drawing_Matrix* m1 = OH_Drawing_MatrixCreate();
    OH_Drawing_MatrixSetMatrix(m1,  -1, 0, 2*kRadius,
                                    0, 1, 0, 
                                    0, 0, 1);
    OH_Drawing_Matrix* m2 = OH_Drawing_MatrixCreate();
    OH_Drawing_MatrixSetMatrix(m2,  1, 0, 0,
                                    0, -1, 2*kRadius, 
                                    0, 0, 1);    
    OH_Drawing_Matrix* m3 = OH_Drawing_MatrixCreate();
    OH_Drawing_MatrixSetMatrix(m3,  1, 0, 0,
                                    0, -1, 2*kRadius, 
                                    0, 0, 1);   
    OH_Drawing_Matrix* m4 = OH_Drawing_MatrixCreate();
    OH_Drawing_MatrixSetMatrix(m4,  0, -1, 2*kRadius, 
                                    -1, 0, 2*kRadius, 
                                    0, 0, 1);       
    OH_Drawing_Matrix* m5 = OH_Drawing_MatrixCreate();
    OH_Drawing_MatrixSetMatrix(m5,  0, -1, 2*kRadius,
                                    1, 0, 0, 
                                    0, 0, 1);       
    OH_Drawing_Matrix* m6 = OH_Drawing_MatrixCreate();
    OH_Drawing_MatrixSetMatrix(m6,  0, 1, 0,
                                    1, 0, 0, 
                                    0, 0, 1);       
    OH_Drawing_Matrix* m7= OH_Drawing_MatrixCreate();
    OH_Drawing_MatrixSetMatrix(m7,  0, 1, 0,
                                    -1,0, 2*kRadius, 
                                    0, 0, 1);       
    
    matrices.push_back(m1);
    matrices.push_back(m2);
    matrices.push_back(m3);
    matrices.push_back(m4);
    matrices.push_back(m5);
    matrices.push_back(m6);
    matrices.push_back(m7);
    int baseMatrixCnt = matrices.size();
    
    OH_Drawing_Matrix* tinyCW = OH_Drawing_MatrixCreateRotation(0.001f, kRadius, kRadius);
    for (int i = 0; i < baseMatrixCnt; ++i) {
        OH_Drawing_Matrix* mTotal= OH_Drawing_MatrixCreate();
        OH_Drawing_MatrixConcat(mTotal, matrices[i], tinyCW);
        matrices.push_back(mTotal);
    }    
    OH_Drawing_MatrixDestroy(tinyCW);
    
    OH_Drawing_Matrix* tinyCCW = OH_Drawing_MatrixCreateRotation(-0.001f, kRadius, kRadius);
    for (int i = 0; i < baseMatrixCnt; ++i) {
        OH_Drawing_Matrix* mTotal= OH_Drawing_MatrixCreate();
        OH_Drawing_MatrixConcat(mTotal, matrices[i], tinyCCW);
        matrices.push_back(mTotal);
    }   
    OH_Drawing_MatrixDestroy(tinyCCW);
    
    OH_Drawing_Matrix* cw45 = OH_Drawing_MatrixCreateRotation(45.f, kRadius, kRadius);
    for (int i = 0; i < baseMatrixCnt; ++i) {
        OH_Drawing_Matrix* mTotal= OH_Drawing_MatrixCreate();
        OH_Drawing_MatrixConcat(mTotal, matrices[i], cw45);
        matrices.push_back(mTotal);
    }  
    OH_Drawing_MatrixDestroy(cw45);
    
    int x = 0;
    int y = 0;
    static constexpr float kPad = 2*kStrokeWidth;    
    OH_Drawing_CanvasTranslate(canvas, kPad, kPad);
    auto bounds  = OH_Drawing_RectCreate(0, 0, 2*kRadius, 2*kRadius);
    
    
    for(auto cap:{LINE_ROUND_CAP,LINE_FLAT_CAP,LINE_SQUARE_CAP}){
        for(const auto m : matrices){
            auto pen = OH_Drawing_PenCreate();
            OH_Drawing_PenSetCap(pen, cap);
            OH_Drawing_PenSetAntiAlias(pen, true);
            OH_Drawing_PenSetWidth(pen, kStrokeWidth);
            OH_Drawing_CanvasSave(canvas);
            {
                OH_Drawing_CanvasTranslate(canvas, x * (2*kRadius + kPad), y * (2*kRadius + kPad));
                OH_Drawing_CanvasConcatMatrix(canvas, m);
                OH_Drawing_PenSetColor(pen, 0x80FF0000);//red
                OH_Drawing_CanvasAttachPen(canvas, pen);
                OH_Drawing_CanvasDrawArc(canvas, bounds, kStart, kSweep);
                OH_Drawing_PenSetColor(pen, 0x800000FF);//red
                OH_Drawing_CanvasAttachPen(canvas, pen);             
                OH_Drawing_CanvasDrawArc(canvas, bounds, kStart, kSweep-360.f);
            }
            OH_Drawing_CanvasRestore(canvas);
            OH_Drawing_CanvasDetachPen(canvas);
            OH_Drawing_PenDestroy(pen);
            ++x;
            if(x == baseMatrixCnt){
                x = 0;
                ++y;
            }
        }
    }
    
    for(auto m:matrices)
    {
        OH_Drawing_MatrixDestroy(m);
    }
    
}
