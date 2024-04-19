#ifndef DRAWBITMAPRECT_H
#define DRAWBITMAPRECT_H
#include "../test_base.h"
#include <bits/alltypes.h>
#include <native_drawing/drawing_rect.h>
#include <native_drawing/drawing_canvas.h>

typedef float Scalar;
struct Rect {
    Scalar fLeft;   // !< smaller x-axis bounds
    Scalar fTop;    // !< smaller y-axis bounds
    Scalar fRight;  // !< larger x-axis bounds
    Scalar fBottom; // !< larger y-axis bounds
};

class DrawBitmapRect2 : public TestBase {
public:
    DrawBitmapRect2();
    ~DrawBitmapRect2() = default;

protected:
    void OnTestFunction(OH_Drawing_Canvas *canvas) override;
};
#endif // DRAWBITMAPRECT_H