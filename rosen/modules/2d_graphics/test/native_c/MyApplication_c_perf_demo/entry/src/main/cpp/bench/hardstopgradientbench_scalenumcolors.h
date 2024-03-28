#ifndef HARDSTOPGRADIENTBENCH_SCALENUMCOLORS_TEST_H
#define HARDSTOPGRADIENTBENCH_SCALENUMCOLORS_TEST_H
#include "test_base.h"

typedef uint32_t SkColor;
typedef unsigned U8CPU;

static constexpr inline SkColor SkColorSetARGB(U8CPU a, U8CPU r, U8CPU g, U8CPU b)
{
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

class HardStopGradientBench_ScaleNumColors : public TestBase {
public:
    static const int kSize = 500;
    int effectType_ = 0;
    int fColorCount_ = 0;
    const char *fName;
    enum {
        kClamp = 0,
        kRepeat,
        kMirror,
    };
    HardStopGradientBench_ScaleNumColors(int effectType, int fColorCount)
    {
        effectType_ = effectType;
        fColorCount_ = fColorCount;
    }
    ~HardStopGradientBench_ScaleNumColors() = default;

protected:
    void OnTestFunction(OH_Drawing_Canvas *canvas) override;
    void OnTestPerformance(OH_Drawing_Canvas *canvas) override;
};
#endif // HARDSTOPGRADIENTBENCH_SCALENUMCOLORS_TEST_H