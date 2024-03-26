#ifndef DRAW_PEN_BEIZER_TEST_H
#define DRAW_PEN_BEIZER_TEST_H
#include "test_base.h"
#include <native_drawing/drawing_typeface.h>
#include <native_drawing/drawing_pen.h>

class BezierBench : public TestBase {
public:
    enum DrawType
    {
        draw_quad,
        draw_cubic
    };
public:
    BezierBench(OH_Drawing_PenLineCapStyle c,OH_Drawing_PenLineJoinStyle j,float w,DrawType d) : capType(c),joinType(j),fWidth(w),drawType(d) {}
    ~BezierBench() = default;

protected:
    void OnTestFunction(OH_Drawing_Canvas *canvas) override;
    void OnTestPerformance(OH_Drawing_Canvas *canvas) override;
    OH_Drawing_PenLineCapStyle capType;
    OH_Drawing_PenLineJoinStyle joinType;
    DrawType drawType;
    float   fWidth;
};
#endif // DRAW_BITMAP_TEST_H