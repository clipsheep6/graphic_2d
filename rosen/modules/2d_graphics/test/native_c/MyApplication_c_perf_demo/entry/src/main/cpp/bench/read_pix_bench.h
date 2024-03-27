#ifndef DRAW_CANVAS_READ_PIXELS_H
#define DRAW_CANVAS_READ_PIXELS_H
#include "../test_base.h"

class ReadPixBench : public TestBase {
    bool bToBitmap_;
public:
    ReadPixBench(bool bToBitmap) : bToBitmap_(bToBitmap) {}
    ~ReadPixBench() = default;

protected:
    void OnTestFunction(OH_Drawing_Canvas *canvas) override;
    void OnTestPerformance(OH_Drawing_Canvas *canvas) override;
};
#endif // DRAW_BITMAP_TEST_H