#ifndef AA_CLIP_BENCH_H
#define AA_CLIP_BENCH_H
#include "../test_base.h"
#include <bits/alltypes.h>
#include <native_drawing/drawing_rect.h>
#include <native_drawing/drawing_canvas.h>

class AAClipBench : public TestBase {
public:
    AAClipBench(bool isPath, bool isAa);
    ~AAClipBench();
    virtual void onDraw(OH_Drawing_Canvas* canvas, uint32_t index) {};
protected:
    bool isPath_;
    bool isAa_;
    OH_Drawing_Rect *clipRect[2];
    OH_Drawing_Path *clipPath;
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};
#endif // AA_CLIP_BENCH_H