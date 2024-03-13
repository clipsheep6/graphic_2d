#ifndef DRAW_BITMAP_TEST_H
#define DRAW_BITMAP_TEST_H
#include "test_base.h"

class DrawBitmap : public TestBase {
public:
    DrawBitmap() : TestBase() {}
    ~DrawBitmap() = default;

protected:
    void OnTestFunctionCpu(OH_Drawing_Canvas *canvas) override;
    void OnTestPerformanceCpu(OH_Drawing_Canvas *canvas) override;
};
#endif // DRAW_BITMAP_TEST_H