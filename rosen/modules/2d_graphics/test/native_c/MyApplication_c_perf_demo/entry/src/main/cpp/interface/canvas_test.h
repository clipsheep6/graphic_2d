#ifndef CANVAS_TEST_H
#define CANVAS_TEST_H
#include "../test_base.h"
#include <bits/alltypes.h>
#include <native_drawing/drawing_rect.h>
#include <native_drawing/drawing_canvas.h>

class CanvasDrawRect : public TestBase {
public:
    CanvasDrawRect(int type):TestBase(type) {};
    ~CanvasDrawRect() = default;

protected:
    int rectWidth_ = 0;
    int rectHeight_ = 0;
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};
#endif // CANVAS_TEST_H