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

typedef uint32_t SkColor;
typedef unsigned U8CPU;

static constexpr inline SkColor SkColorSetARGB(U8CPU a, U8CPU r, U8CPU g, U8CPU b) {
    if (a <= 255 && r <= 255 && g <= 255 && b <= 255) {
        return (a << 24) | (r << 16) | (g << 8) | (b << 0);
    }
    return (a << 24) | (r << 16) | (g << 8) | (b << 0);
}

/* * Returns color value from 8-bit component values. Asserts if SK_DEBUG is defined
    if a, r, g, or b exceed 255. Since color is unpremultiplied, a may be smaller
    than the largest of r, g, and b.

    @param a  amount of alpha, from fully transparent (0) to fully opaque (255)
    @param r  amount of red, from no red (0) to full red (255)
    @param g  amount of green, from no green (0) to full green (255)
    @param b  amount of blue, from no blue (0) to full blue (255)
    @return   color and alpha, unpremultiplied
*/
/* * Represents fully opaque red.
 */
constexpr SkColor SK_ColorRED = SkColorSetARGB(0xFF, 0xFF, 0x00, 0x00);

/* * Represents fully opaque green. HTML lime is equivalent.
    Note that HTML green is equivalent to 0xFF008000.
*/
constexpr SkColor SK_ColorGREEN = SkColorSetARGB(0xFF, 0x00, 0xFF, 0x00);

/* * Represents fully opaque blue.
 */
constexpr SkColor SK_ColorBLUE = SkColorSetARGB(0xFF, 0x00, 0x00, 0xFF);

/* * Represents fully opaque yellow.
 */
constexpr SkColor SK_ColorYELLOW = SkColorSetARGB(0xFF, 0xFF, 0xFF, 0x00);


/** @return the number of entries in an array (not a pointer)
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