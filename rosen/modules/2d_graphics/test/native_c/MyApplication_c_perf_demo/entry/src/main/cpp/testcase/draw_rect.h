#ifndef DRAW_RECT_H
#define DRAW_RECT_H
#include "test_base.h"
#include <bits/alltypes.h>
#include <native_drawing/drawing_rect.h>

class DrawRect : public TestBase {
public:
    DrawRect() : TestBase() {}
    DrawRect(int shift);
    ~DrawRect();
protected:
    enum {
        RAND_SIZE = 300,
        W = 720,
        H = 720,
        N = 300,
    };
    OH_Drawing_Rect* rects[RAND_SIZE];
    uint32_t colors[RAND_SIZE];
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};
#endif // DRAW_RECT_H