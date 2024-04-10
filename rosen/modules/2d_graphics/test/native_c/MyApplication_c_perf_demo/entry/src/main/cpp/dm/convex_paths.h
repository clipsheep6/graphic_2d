
#ifndef Convex_Paths_H
#define Convex_Paths_H
#include "../test_base.h"
#include <bits/alltypes.h>
#include <native_drawing/drawing_rect.h>
#include <native_drawing/drawing_canvas.h>
#include <native_drawing/drawing_mask_filter.h>
#include <native_drawing/drawing_filter.h>
#include "common/log_common.h"
#include "test_common.h"


#define SkIntToScalar(x)        static_cast<SkScalar>(x)

class ConvexPaths : public TestBase {
public:
    ConvexPaths() {
        bitmapWidth_ = 1200;
        bitmapHeight_ = 1100;
        fileName_ = "convexpaths";
    }
    ~ConvexPaths() = default;
    void MakePath();
protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
};

#endif //Convex_Paths_H
