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

typedef float SkScalar;
#define SK_ScalarHalf 0.5f
#define SK_Scalar1 1.0f
#define SkIntToScalar(x) static_cast<SkScalar>(x)
#define SkScalarAve(a, b) (((a) + (b)) * SK_ScalarHalf)

/* * @return the number of entries in an array (not a pointer)
 */
template <typename T, size_t N> char (&SkArrayCountHelper(T (&array)[N]))[N];
#define SK_ARRAY_COUNT(array) (sizeof(SkArrayCountHelper(array)))

class GradientsGM : public TestBase {
public:
    GradientsGM();
    ~GradientsGM() = default;

protected:
    void OnTestFunction(OH_Drawing_Canvas *canvas) override;
};
#endif // GRADIENTSGM_H