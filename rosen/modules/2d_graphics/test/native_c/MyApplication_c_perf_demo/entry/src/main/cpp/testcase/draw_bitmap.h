#ifndef DRAW_BITMAP_H
#define DRAW_BITMAP_H
#include "test_base.h"

class DrawBitmap : public TestBase {
public:
    DrawBitmap() : TestBase() {}
    ~DrawBitmap() = default;

protected:
    void OnTestFunction(OH_Drawing_Canvas *canvas) override;
    void OnTestPerformance(OH_Drawing_Canvas *canvas) override;
};
#endif // DRAW_BITMAP_H