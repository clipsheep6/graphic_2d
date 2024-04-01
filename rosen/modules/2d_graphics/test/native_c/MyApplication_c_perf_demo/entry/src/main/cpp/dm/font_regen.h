#ifndef FONT_REGEN_H
#define FONT_REGEN_H
#include "../test_base.h"
#include <bits/alltypes.h>
#include <native_drawing/drawing_rect.h>
#include <native_drawing/drawing_canvas.h>

class BadApple : public TestBase {
public:
    BadApple();
    ~BadApple()=default;
protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
};
#endif // AA_RECT_MODES_H