#ifndef DRAW_RECT_H
#define DRAW_RECT_H
#include "test_base.h"

class DrawRect : public TestBase {
public:
    DrawRect() : TestBase() {}
    ~DrawRect() = default;
protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};
#endif // DRAW_RECT_H