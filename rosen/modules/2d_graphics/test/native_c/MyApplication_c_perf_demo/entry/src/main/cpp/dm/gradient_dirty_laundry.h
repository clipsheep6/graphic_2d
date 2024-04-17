#ifndef GRADIENTSGM_H
#define GRADIENTSGM_H
#include "../test_base.h"
#include <bits/alltypes.h>
#include <native_drawing/drawing_rect.h>
#include <native_drawing/drawing_canvas.h>
#include <native_drawing/drawing_point.h>
#include <native_drawing/drawing_rect.h>
#include <native_drawing/drawing_shader_effect.h>
#include <time.h>
#include "common/log_common.h"
#include "test_common.h"

typedef float Scalar;
typedef uint32_t Color;
#define ScalarHalf 0.5f
#define Scalar1 1.0f
#define IntToScalar(x) static_cast<Scalar>(x)
#define ScalarAve(a, b) (((a) + (b)) * ScalarHalf)

template <typename T, size_t N> char (&SkArrayCountHelper(T (&array)[N]))[N];
#define SK_ARRAY_COUNT(array) (sizeof(SkArrayCountHelper(array)))

class GradientsGM : public TestBase {
public:
    GradientsGM();
    ~GradientsGM() = default;

protected:
    void OnTestFunction(OH_Drawing_Canvas *canvas) override;
    //  渐变的起点和终点坐标
    OH_Drawing_Point *firstPoint = OH_Drawing_PointCreate(0, 0);
    OH_Drawing_Point *secondPoint = OH_Drawing_PointCreate(IntToScalar(100), IntToScalar(100));
};
#endif // GRADIENTSGM_H