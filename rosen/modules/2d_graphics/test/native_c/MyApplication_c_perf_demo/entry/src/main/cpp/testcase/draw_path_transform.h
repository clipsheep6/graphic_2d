#ifndef DRAW_PATH_TRANSFORM_H
#define DRAW_PATH_TRANSFORM_H
#include "test_base.h"

class DrawPathTransform : public TestBase {
public:
    DrawPathTransform() : TestBase() {}
    ~DrawPathTransform() = default;

protected:
    void OnTestFunction(OH_Drawing_Canvas *canvas) override;
    void OnTestPerformance(OH_Drawing_Canvas *canvas) override;
    void makePathLine(OH_Drawing_Path *path);
    void makePathQuad(OH_Drawing_Path *path);
    void makePathConic(OH_Drawing_Path *path);
    void makePathCubic(OH_Drawing_Path *path);
};
#endif // DRAW_BITMAP_TEST_H