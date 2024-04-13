#ifndef CLIP_CUBIC_GM_H
#define CLIP_CUBIC_GM_H
#include "../test_base.h"
#include <bits/alltypes.h>
#include <native_drawing/drawing_canvas.h>

class ClipCubicGm : public TestBase {
public:
    ClipCubicGm();
    ~ClipCubicGm();
protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
};
#endif // CLIP_CUBIC_GM_H