#include "random_path_bench.h"
#include <string>
#include <time.h>
#include <bits/alltypes.h>
#include <array>
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_bitmap.h>
#include <native_drawing/drawing_canvas.h>
#include <native_drawing/drawing_matrix.h>
#include <native_drawing/drawing_rect.h>
#include <native_drawing/drawing_point.h>
#include "test_common.h"
#include "common/log_common.h"

using namespace std;

array<OH_Drawing_Point2D, kNumPoints> fPoints;
array<int, kPathCnt> fVerbCnts;
array<int, kNumVerbs> fVerbs;

void RandomPathBench::restartMakingPaths()
{
    fCurrPath = 0;
    fCurrVerb = 0;
    fCurrPoint = 0;
}

void RandomPathBench::createData(int minVerbs, int maxVerbs, bool allowMoves, OH_Drawing_Rect *bounds)
{
    OH_Drawing_Rect *tempBounds;
    TestRend rand = TestRend();
    fVerbCnts.empty();
    for (int i = 0; i < kNumVerbCnts; ++i) {
        fVerbCnts[i] = (int)rand.nextRangeF(minVerbs, maxVerbs + 1);
    }
    fVerbs.empty();
    for (int i = 0; i < kNumVerbs; ++i) {
        do {
            fVerbs[i] = static_cast<Verb>(rand.nextULessThan(Verb::kDone_Verb));
        } while (!allowMoves && Verb::kMove_Verb == fVerbs[i]);
    }
    fPoints.empty();
    for (int i = 0; i < kNumPoints; ++i) {
        OH_Drawing_Point2D p;
        fPoints[i].x = rand.nextRangeF(0, DEFAULT_SCREEN_WIDTH);
        fPoints[i].y = rand.nextRangeF(0, DEFAULT_SCREEN_WIDTH);
    }
    restartMakingPaths();
}

void RandomPathBench::makePath(OH_Drawing_Path *path)
{
    int vCount = fVerbCnts[(fCurrPath++) & (kNumVerbCnts - 1)];
    for (int v = 0; v < vCount; ++v) {
        int verb = fVerbs[(fCurrVerb++) & (kNumVerbs - 1)];

        switch (verb) {
            case Verb::kMove_Verb:
                OH_Drawing_PathMoveTo(path, fPoints[(fCurrPoint++) & (kNumPoints - 1)].x,
                    fPoints[(fCurrPoint++) & (kNumPoints - 1)].y);
                break;
            case Verb::kLine_Verb:
                OH_Drawing_PathLineTo(path, fPoints[(fCurrPoint++) & (kNumPoints - 1)].x,
                    fPoints[(fCurrPoint++) & (kNumPoints - 1)].y);
                break;
            case Verb::kQuad_Verb:
                OH_Drawing_PathQuadTo(path, fPoints[(fCurrPoint + 0) & (kNumPoints - 1)].x,
                    fPoints[(fCurrPoint + 0) & (kNumPoints - 1)].y, fPoints[(fCurrPoint + 1) & (kNumPoints - 1)].x,
                    fPoints[(fCurrPoint + 1) & (kNumPoints - 1)].y);
                fCurrPoint += 2;
                break;
            case Verb::kConic_Verb:
                OH_Drawing_PathQuadTo(path, fPoints[(fCurrPoint + 0) & (kNumPoints - 1)].x,
                    fPoints[(fCurrPoint + 0) & (kNumPoints - 1)].y, fPoints[(fCurrPoint + 1) & (kNumPoints - 1)].x,
                    fPoints[(fCurrPoint + 1) & (kNumPoints - 1)].y);
                fCurrPoint += 2;
                break;
            case Verb::kCubic_Verb:
                OH_Drawing_PathCubicTo(path, fPoints[(fCurrPoint + 0) & (kNumPoints - 1)].x,
                    fPoints[(fCurrPoint + 0) & (kNumPoints - 1)].y, fPoints[(fCurrPoint + 1) & (kNumPoints - 1)].x,
                    fPoints[(fCurrPoint + 1) & (kNumPoints - 1)].y, fPoints[(fCurrPoint + 2) & (kNumPoints - 1)].x,
                    fPoints[(fCurrPoint + 2) & (kNumPoints - 1)].y);
                fCurrPoint += 3;
                break;
            case Verb::kClose_Verb:
                OH_Drawing_PathClose(path);
                break;
            default:
                OH_Drawing_PathClose(path);
                break;
        }
    }
}
