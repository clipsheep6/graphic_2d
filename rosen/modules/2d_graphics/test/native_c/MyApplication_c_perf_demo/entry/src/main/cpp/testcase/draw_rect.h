#ifndef DRAW_RECT_H
#define DRAW_RECT_H
#include "rect_bench.h"
#include <bits/alltypes.h>
#include <native_drawing/drawing_rect.h>

class DrawRect : public RectBench {
public:
    DrawRect(int shift):RectBench(shift) {};
    ~DrawRect(){};
protected:
    void onDraw(OH_Drawing_Canvas* canvas, uint32_t index) override;
};
#endif // DRAW_RECT_H