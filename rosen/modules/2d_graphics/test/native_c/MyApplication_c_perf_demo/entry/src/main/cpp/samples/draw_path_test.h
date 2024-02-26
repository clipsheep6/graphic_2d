#ifndef DRAW_PATH_TEST_H
#define DRAW_PATH_TEST_H
#include "test_base.h"

class DrawPathTest : public TestBase {
public:
    DrawPathTest() : TestBase() {}
    ~DrawPathTest() = default;
protected:
    void OnTestFunctionCpu(OH_Drawing_Canvas* canvas) override;
    void OnTestPerformanceCpu(OH_Drawing_Canvas* canvas) override;
};
#endif // DRAW_PATH_TEST_H