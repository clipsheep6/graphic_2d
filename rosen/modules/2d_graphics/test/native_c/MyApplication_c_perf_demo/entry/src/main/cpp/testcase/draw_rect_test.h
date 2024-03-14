#ifndef DRAW_RECT_TEST_H
#define DRAW_RECT_TEST_H
#include "test_base.h"

class DrawRectTest : public TestBase {
public:
    DrawRectTest() : TestBase() {}
    ~DrawRectTest() = default;
protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};
#endif // DRAW_RECT_TEST_H