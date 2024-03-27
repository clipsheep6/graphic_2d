#ifndef DRAW_BITMAP_H
#define DRAW_BITMAP_H
#include "../test_base.h"

class DrawBitmapAABench : public TestBase {
public:
    DrawBitmapAABench() : TestBase() {}
    ~DrawBitmapAABench() = default;

protected:
    void OnTestFunction(OH_Drawing_Canvas *canvas) override;
    void OnTestPerformance(OH_Drawing_Canvas *canvas) override;
};
#endif // DRAW_BITMAP_H