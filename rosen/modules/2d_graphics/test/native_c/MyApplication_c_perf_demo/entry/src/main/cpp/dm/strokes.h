#ifndef STROKES_H
#define STROKES_H
#include "../test_base.h"
#include <bits/alltypes.h>
#include <native_drawing/drawing_rect.h>
#include <native_drawing/drawing_canvas.h>

class Strokes2 : public TestBase {
public:
    Strokes2();
    ~Strokes2()=default;
protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
    OH_Drawing_Path* fPath;
};
#endif // AA_RECT_MODES_H