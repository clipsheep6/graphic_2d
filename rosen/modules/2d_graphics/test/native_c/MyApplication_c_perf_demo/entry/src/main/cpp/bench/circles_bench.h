#ifndef DRAW_PATH_ARC_TO_TEST_H
#define DRAW_PATH_ARC_TO_TEST_H
#include "../test_base.h"

class CirclesBench : public TestBase {
public:
    CirclesBench() : TestBase() {}
    ~CirclesBench() = default;

protected:
    void OnTestFunction(OH_Drawing_Canvas *canvas) override;
    void OnTestPerformance(OH_Drawing_Canvas *canvas) override;
};
#endif // DRAW_BITMAP_TEST_H