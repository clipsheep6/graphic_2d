#ifndef PATH_INTERIOR_H
#define PATH_INTERIOR_H
#include "../test_base.h"
#include <bits/alltypes.h>
#include <native_drawing/drawing_rect.h>
#include <native_drawing/drawing_canvas.h>

class PathInterior : public TestBase {
public:
    PathInterior();
    ~PathInterior()=default;
protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;

};
#endif // AA_RECT_MODES_H