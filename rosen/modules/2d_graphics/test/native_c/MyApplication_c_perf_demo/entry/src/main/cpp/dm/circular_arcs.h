#ifndef CIRCULAR_ARCS_H
#define CIRCULAR_ARCS_H
#include "../test_base.h"
#include <bits/alltypes.h>
#include <native_drawing/drawing_rect.h>
#include <native_drawing/drawing_canvas.h>

class CircularArcStrokeMatrix : public TestBase {
public:
    CircularArcStrokeMatrix();
    ~CircularArcStrokeMatrix()=default;
protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
};
#endif // AA_RECT_MODES_H