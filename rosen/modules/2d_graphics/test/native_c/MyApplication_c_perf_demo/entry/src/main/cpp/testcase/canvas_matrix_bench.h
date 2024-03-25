#ifndef DRAW_CANVAS_SCALE_TEST_H
#define DRAW_CANVAS_SCALE_TEST_H
#include "test_base.h"

class CanvasMatrixBench : public TestBase {
public:
    enum CanvasType{
        Translate,
        Scale,
        ConcatMatrix
    };
public:
    CanvasMatrixBench(CanvasType t):cType(t){};
    ~CanvasMatrixBench() = default;

protected:
    void OnTestFunction(OH_Drawing_Canvas *canvas) override;
    void OnTestPerformance(OH_Drawing_Canvas *canvas) override;
    CanvasType cType;
};
#endif // DRAW_BITMAP_TEST_H