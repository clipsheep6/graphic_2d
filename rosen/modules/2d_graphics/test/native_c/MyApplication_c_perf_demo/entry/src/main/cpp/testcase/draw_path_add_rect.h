#ifndef DRAW_PATH_ADD_RECT_TEST_H
#define DRAW_PATH_ADD_RECT_TEST_H
#include "test_base.h"

class DrawPathAddRect : public TestBase {
public:
    DrawPathAddRect() : TestBase() {}
    ~DrawPathAddRect() = default;
protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};
#endif // DRAW_PATH_ADD_RECT_TEST_H