#ifndef RANDOM_PATH_BENCH_H
#define RANDOM_PATH_BENCH_H
#include "test_base.h"
#include <bits/alltypes.h>
#include <native_drawing/drawing_rect.h>
#include <native_drawing/drawing_canvas.h>

enum Verb {
    kMove_Verb = 0,
    kLine_Verb,
    kQuad_Verb,
    kConic_Verb,
    kCubic_Verb,
    kClose_Verb,
    kDone_Verb = kClose_Verb + 1
};

enum {
    kNumVerbCnts = 1 << 5,
    kNumVerbs = 1 << 5,
    kNumPoints = 1 << 5,
};

enum {
    // must be a pow 2
    kPathCnt = 1 << 5,
};

class RandomPathBench : public TestBase {
public:
    RandomPathBench() : TestBase() {}
    ~RandomPathBench() = default;
    int fCurrPath;
    int fCurrVerb;
    int fCurrPoint;


protected:
    void restartMakingPaths();
    void createData(int minVerbs, int maxVerbs, bool allowMoves, OH_Drawing_Rect *bounds);
    void makePath(OH_Drawing_Path *path);

};
#endif // RANDOM_PATH_BENCH_H