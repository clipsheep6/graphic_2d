#ifndef ADD_ARC_H
#define ADD_ARC_H
#include "../test_base.h"
#include <bits/alltypes.h>
#include <native_drawing/drawing_rect.h>
#include <native_drawing/drawing_canvas.h>

class AddArcMeas : public TestBase {
public:
    AddArcMeas();
    ~AddArcMeas();
protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
};
#endif // AA_RECT_MODES_H