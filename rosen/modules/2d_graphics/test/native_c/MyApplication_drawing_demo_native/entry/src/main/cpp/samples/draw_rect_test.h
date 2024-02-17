#ifndef DRAW_RECT_TEST_H
#define DRAW_RECT_TEST_H
#include "test_base.h"

class DrawRectTest : public TestBase {
public:
    DrawRectTest() : TestBase() {}
    ~DrawRectTest() = default;
protected:
    void OnRecording(OH_Drawing_Canvas* canvas) override;
};
#endif // DRAW_RECT_TEST_H