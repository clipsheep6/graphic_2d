#ifndef BLUR_LARGE_RRECTS_H
#define BLUR_LARGE_RRECTS_H
#include "../test_base.h"
#include <bits/alltypes.h>
#include <native_drawing/drawing_rect.h>
#include <native_drawing/drawing_canvas.h>
#include <native_drawing/drawing_mask_filter.h>
#include <native_drawing/drawing_filter.h>

class BlurLargeRrects : public TestBase {
public:
    BlurLargeRrects();
    ~BlurLargeRrects() = default;
protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
};
#endif // BLUR_LARGE_RRECTS_H