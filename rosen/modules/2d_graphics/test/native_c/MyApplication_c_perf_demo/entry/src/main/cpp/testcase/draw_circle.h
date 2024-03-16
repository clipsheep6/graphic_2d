#ifndef DRAW_CIRCLE_H
#define DRAW_CIRCLE_H
#include "test_base.h"

class DrawCircle : public TestBase {
public:
    DrawCircle() : TestBase() {}
    ~DrawCircle() = default;

protected:
    int count_ = 5;
    void OnTestFunction(OH_Drawing_Canvas *canvas) override;
    void OnTestPerformance(OH_Drawing_Canvas *canvas) override;
};
#endif // DRAW_CIRCLE_H