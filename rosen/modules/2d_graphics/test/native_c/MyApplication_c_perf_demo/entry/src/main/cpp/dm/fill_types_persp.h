#ifndef FILL_TYPES_PERSP_H
#define FILL_TYPES_PERSP_H
#include "../test_base.h"
#include <bits/alltypes.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_rect.h>
#include <native_drawing/drawing_canvas.h>

class FillTypesPersp : public TestBase {
public:
    FillTypesPersp();
    ~FillTypesPersp()=default;
    void makePath();
protected:
    void showPath(OH_Drawing_Canvas* canvas, int x, int y, OH_Drawing_PathFillType ft, float scale, OH_Drawing_Brush* brush);
    void showFour(OH_Drawing_Canvas* canvas, float scale, bool aa);
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
    OH_Drawing_Path* fPath;
};
#endif // AA_RECT_MODES_H