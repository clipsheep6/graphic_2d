#include "testcase/draw_path_reset.h"
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_rect.h>
#include <native_drawing/drawing_point.h>
#include "test_common.h"

enum {
    kNumVerbCnts = 1 << 5,
    kNumVerbs = 1 << 5,
    kNumPoints = 1 << 5,
};

void DrawPathReset::OnTestFunction(OH_Drawing_Canvas *canvas) {
    OH_Drawing_Rect *bounds = OH_Drawing_RectCreate(0, 0, 1.0f, 1.0f);
    OH_Drawing_Path *path = OH_Drawing_PathCreate();
    OH_Drawing_Pen *pen = OH_Drawing_PenCreate();
    OH_Drawing_CanvasAttachPen(canvas, pen);
    makePath(path);
    OH_Drawing_CanvasDrawPath(canvas, path);
    OH_Drawing_PathReset(path);
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_RectDestroy(bounds);
    OH_Drawing_PathDestroy(path);
    OH_Drawing_PenDestroy(pen);
    pen = nullptr;
} 


void DrawPathReset::OnTestPerformance(OH_Drawing_Canvas* canvas){
    // path_create
    OH_Drawing_Rect *bounds = OH_Drawing_RectCreate(0, 0, 1.0f, 1.0f);
    OH_Drawing_Path *path = OH_Drawing_PathCreate();
    OH_Drawing_Pen *pen = OH_Drawing_PenCreate();
    OH_Drawing_CanvasAttachPen(canvas, pen);
     for (int i = 0; i < testCount_; i++) {
            if (i % 1000 == 0) {
                OH_Drawing_PathReset(path);
            }
            makePath(path);
        }
    OH_Drawing_CanvasDrawPath(canvas, path);
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_RectDestroy(bounds);
    OH_Drawing_PathDestroy(path);
    OH_Drawing_PenDestroy(pen);
    pen = nullptr;
}

void DrawPathReset::makePath(OH_Drawing_Path *path) {
    TestRend rand = TestRend();
    int fVerbCnts[kNumVerbCnts];
    int fVerbs[kNumVerbs];
    float fPoints[kNumPoints];
    int fCurrPath = 0;
    int fCurrVerb = 0;
    int fCurrPoint = 0;
    for (int i = 0; i < kNumVerbCnts; ++i) {
                fVerbCnts[i] = rand.nextRangeU(10, 100);
    }
    for (int i = 0; i < kNumVerbs; ++i) {
                fVerbs[i] = rand.nextULessThan(6);
    }
    for (int i = 0; i < kNumPoints; ++i) {
                fPoints[i] = rand.nextRangeF(0, 1.0f);
    }

        int vCount = fVerbCnts[(fCurrPath++) & (kNumVerbCnts - 1)];
        for (int v = 0; v < vCount; ++v) {
            int verb = fVerbs[(fCurrVerb++) & (kNumVerbs - 1)];
            switch (verb) {
            case 1:
                OH_Drawing_PathMoveTo(path, fPoints[(fCurrPoint + 0) & (kNumPoints - 1)], fPoints[(fCurrPoint + 1) & (kNumPoints - 1)]);
                fCurrPoint += 2;
                break;
            case 2:
                OH_Drawing_PathLineTo(path, fPoints[(fCurrPoint + 0) & (kNumPoints - 1)], fPoints[(fCurrPoint + 1) & (kNumPoints - 1)]);
                fCurrPoint += 2;
                break;
            case 3:
                OH_Drawing_PathQuadTo(path,
                                      fPoints[(fCurrPoint + 0) & (kNumPoints - 1)],
                                      fPoints[(fCurrPoint + 1) & (kNumPoints - 1)],
                                      fPoints[(fCurrPoint + 2) & (kNumPoints - 1)],
                                      fPoints[(fCurrPoint + 3) & (kNumPoints - 1)]);
                fCurrPoint += 4;
                break;
            case 4:
                OH_Drawing_PathCubicTo(path,
                                       fPoints[(fCurrPoint + 0) & (kNumPoints - 1)],
                                       fPoints[(fCurrPoint + 1) & (kNumPoints - 1)],
                                       fPoints[(fCurrPoint + 2) & (kNumPoints - 1)],
                                       fPoints[(fCurrPoint + 3) & (kNumPoints - 1)],
                                       fPoints[(fCurrPoint + 4) & (kNumPoints - 1)],
                                       fPoints[(fCurrPoint + 5) & (kNumPoints - 1)]);
                fCurrPoint += 6;
                break;
            case 5:
                OH_Drawing_PathClose(path);
                break;
            default:
                OH_Drawing_PathLineTo(path, fPoints[(fCurrPoint + 0) & (kNumPoints - 1)], fPoints[(fCurrPoint + 1) & (kNumPoints - 1)]);
                fCurrPoint += 2;
                break;
            }
        }
}
