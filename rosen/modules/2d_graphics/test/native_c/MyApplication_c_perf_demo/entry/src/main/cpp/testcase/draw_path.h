#ifndef DRAW_PATH_H
#define DRAW_PATH_H
#include "test_base.h"

class DrawPath : public TestBase {
public:
    DrawPath() : TestBase() {}
    ~DrawPath() = default;
protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};
#endif // DRAW_PATH_H