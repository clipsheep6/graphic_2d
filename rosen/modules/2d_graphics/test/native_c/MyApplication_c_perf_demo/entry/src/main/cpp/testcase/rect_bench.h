#ifndef RECT_BENCH_H
#define RECT_BENCH_H
#include "test_base.h"
#include <bits/alltypes.h>
#include <native_drawing/drawing_rect.h>
#include <native_drawing/drawing_canvas.h>

class RectBench : public TestBase {
public:
    RectBench(int shift);
    ~RectBench();
    virtual void onDraw(OH_Drawing_Canvas* canvas, uint32_t index);
protected:
    enum {
        RAND_SIZE = 300, //skia benchmark:300
        W = DEFAULT_WIDTH,
        H = DEFAULT_HEIGHT,
    };
    OH_Drawing_Rect* rects[RAND_SIZE];
    uint32_t widths[RAND_SIZE];
    uint32_t heights[RAND_SIZE];
    uint32_t colors[RAND_SIZE];
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class RRectBench : public RectBench {
public:
    RRectBench(int shift):RectBench(shift) {};
    ~RRectBench(){};
protected:
    void onDraw(OH_Drawing_Canvas* canvas, uint32_t index) override;
};

class OvalBench : public RectBench {
public:
    OvalBench(int shift):RectBench(shift) {};
    ~OvalBench(){};
protected:
    void onDraw(OH_Drawing_Canvas* canvas, uint32_t index) override;
};
#endif // RECT_BENCH_H