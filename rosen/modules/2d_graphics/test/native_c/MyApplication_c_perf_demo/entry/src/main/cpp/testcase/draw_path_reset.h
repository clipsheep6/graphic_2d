#ifndef DRAW_PATH_RESET_TEST_H
#define DRAW_PATH_RESET_TEST_H
#include "test_base.h"

class DrawPathReset : public TestBase {
public:
    DrawPathReset() : TestBase() {}
    ~DrawPathReset() = default;

protected:
    void OnTestFunction(OH_Drawing_Canvas *canvas) override;
    void OnTestPerformance(OH_Drawing_Canvas *canvas) override;
    void makePath(OH_Drawing_Path *path);
};
#endif // DRAW_PATH_RESET_TEST_H
