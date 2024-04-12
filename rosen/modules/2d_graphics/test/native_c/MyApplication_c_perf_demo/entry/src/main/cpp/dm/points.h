#ifndef POINTS_H
#define POINTS_H
#include "../test_base.h"
#include <bits/alltypes.h>
#include <native_drawing/drawing_rect.h>
#include <native_drawing/drawing_canvas.h>
#include <native_drawing/drawing_point.h>
#include "test_common.h"

class Points : public TestBase {
public:
    Points();
    ~Points()=default;
protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
    void fill_pts(OH_Drawing_Point2D pts[], size_t n, TestRend* rand);
};
#endif // AA_RECT_MODES_H