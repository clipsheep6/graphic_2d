#ifndef DRAW_ROUND_RECT_H
#define DRAW_ROUND_RECT_H
#include "rect_bench.h"
#include <bits/alltypes.h>

class DrawRountRect : public RectBench {
public:
    DrawRountRect(int shift):RectBench(shift) {};
    ~DrawRountRect(){};
protected:
    void onDraw(OH_Drawing_Canvas* canvas, uint32_t index) override;
};
#endif // DRAW_ROUND_RECT_H