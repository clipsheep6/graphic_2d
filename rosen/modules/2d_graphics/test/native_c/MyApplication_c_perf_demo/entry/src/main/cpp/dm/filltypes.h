#ifndef FILLTYPEGM_H
#define FILLTYPEGM_H
#include "../test_base.h"
#include <bits/alltypes.h>
#include <native_drawing/drawing_rect.h>
#include <native_drawing/drawing_canvas.h>
#include <native_drawing/drawing_path.h>

typedef float SkScalar;
#define SK_Scalar1 1.0f
#define SK_ScalarHalf 0.5f

class FillTypeGM : public TestBase {
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

    FillTypeGM();
    ~FillTypeGM() = default;

protected:
    SkPathFillType ft;
    // 创建路径path对象
    OH_Drawing_Path *path = OH_Drawing_PathCreate();
    void OnTestFunction(OH_Drawing_Canvas *canvas) override;
    void showPath(OH_Drawing_Canvas *canvas, int x, int y, SkPathFillType ft, SkScalar scale, OH_Drawing_Brush *brush);
    void showFour(OH_Drawing_Canvas *canvas, SkScalar scale, OH_Drawing_Brush *brush);
};
#endif // FILLTYPEGM_H