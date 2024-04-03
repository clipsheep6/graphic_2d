#ifndef LARGECLIPPEDPATH_H
#define LARGECLIPPEDPATH_H
#include "../test_base.h"
#include <bits/alltypes.h>
#include <native_drawing/drawing_rect.h>
#include <native_drawing/drawing_canvas.h>
#include <native_drawing/drawing_mask_filter.h>
#include <native_drawing/drawing_filter.h>
#include "common/log_common.h"
#include "test_common.h"

#define SK_ScalarPI SK_FloatPI
constexpr float SK_FloatPI = 3.14159265f;

class LargeClippedPath : public TestBase {
public:
    enum SkPathFillType {
        /* * Specifies that "inside" is computed by a non-zero sum of signed edge crossings */
        kWinding,
        /* * Specifies that "inside" is computed by an odd number of edge crossings */
        kEvenOdd,
        /* * Same as Winding, but draws outside of the path, rather than inside */
        kInverseWinding,
        /* * Same as EvenOdd, but draws outside of the path, rather than inside */
        kInverseEvenOdd
    };

    LargeClippedPath(SkPathFillType type) : fType(type)
    {
        bitmapWidth_ = 1000;
        bitmapHeight_ = 1000;
        fileName_ = "largeclippedpath";
    }
    
    ~LargeClippedPath() = default;

protected:
    void OnTestFunction(OH_Drawing_Canvas *canvas) override;
    SkPathFillType fType;
};
#endif // LARGECLIPPEDPATH_H