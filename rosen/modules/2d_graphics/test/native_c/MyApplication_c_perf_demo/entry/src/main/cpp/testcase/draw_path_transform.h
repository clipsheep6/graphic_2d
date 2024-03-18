#ifndef DRAW_PATH_TRANSFORM_H
#define DRAW_PATH_TRANSFORM_H
#include "test_base.h"

class DrawPathTransform : public TestBase {
public:
    DrawPathTransform() : TestBase() {}
    ~DrawPathTransform() = default;

protected:
    void OnTestFunction(OH_Drawing_Canvas *canvas) override;
    void OnTestPerformance(OH_Drawing_Canvas *canvas) override;
};
#endif // DRAW_BITMAP_TEST_H