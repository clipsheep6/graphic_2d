#ifndef DRAW_PATH_LINE_TO_TEST_H
#define DRAW_PATH_LINE_TO_TEST_H
#include "test_base.h"

class DrawPathLineTo : public TestBase {
public:
    DrawPathLineTo() : TestBase() {}
    ~DrawPathLineTo() = default;

protected:
    void OnTestFunction(OH_Drawing_Canvas *canvas) override;
    void OnTestPerformance(OH_Drawing_Canvas *canvas) override;
};
#endif // DRAW_PATH_LINE_TO_TEST_H
