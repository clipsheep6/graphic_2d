#ifndef SHADOW_UTILS_H
#define SHADOW_UTILS_H
#include "../test_base.h"
#include <bits/alltypes.h>
#include <native_drawing/drawing_rect.h>
#include <native_drawing/drawing_canvas.h>

class ShadowUtils : public TestBase {
public:
    enum ShadowMode {
        kDebugColorNoOccluders,
        kDebugColorOccluders,
        kGrayscale
    };
    ShadowUtils(ShadowMode m);
    ~ShadowUtils()=default;
protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
    void draw_paths(OH_Drawing_Canvas* canvas, ShadowMode mode);
    ShadowMode sMode;
};
#endif // AA_RECT_MODES_H