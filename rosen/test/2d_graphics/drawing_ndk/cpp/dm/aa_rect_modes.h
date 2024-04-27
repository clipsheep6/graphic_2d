#ifndef AA_RECT_MODES_H
#define AA_RECT_MODES_H
#include "../test_base.h"
#include <bits/alltypes.h>
#include <native_drawing/drawing_rect.h>
#include <native_drawing/drawing_canvas.h>

class AARectModes : public TestBase {
public:
    AARectModes();
    ~AARectModes();
protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
};
#endif // AA_RECT_MODES_H