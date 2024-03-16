#ifndef DRAW_OVAL_H
#define DRAW_OVAL_H
#include "rect_bench.h"
#include <bits/alltypes.h>
#include <native_drawing/drawing_rect.h>

class DrawOval : public RectBench {
public:
    DrawOval(int shift):RectBench(shift) {};
    ~DrawOval(){};
protected:
    void onDraw(OH_Drawing_Canvas* canvas, uint32_t index) override;
};
#endif // DRAW_OVAL_H