#ifndef DRAW_BITMAP_RECT_H
#define DRAW_BITMAP_RECT_H
#include "test_base.h"

class DrawBitmapRect : public TestBase {
public:
    DrawBitmapRect() : TestBase() {}
    ~DrawBitmapRect() = default;

protected:
    void OnTestFunction(OH_Drawing_Canvas *canvas) override;
    void OnTestPerformance(OH_Drawing_Canvas *canvas) override;
};
#endif // DRAW_BITMAP_RECT_H