#ifndef DRAW_PATH_CUBIC_TO_TEST_H
#define DRAW_PATH_CUBIC_TO_TEST_H
#include "test_base.h"

class DrawPathCubicTo : public TestBase {
  public:
    DrawPathCubicTo() : TestBase() {}
    ~DrawPathCubicTo() = default;

  protected:
    void OnTestFunction(OH_Drawing_Canvas *canvas) override;
    void OnTestPerformance(OH_Drawing_Canvas *canvas) override;
    void makePathCubic(OH_Drawing_Path *path);
};
#endif // DRAW_PATH_CUBIC_TO_TEST_H
