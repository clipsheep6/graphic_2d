//
// Created on 2024/4/11.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#ifndef CONIC_PATHS_H
#define CONIC_PATHS_H
#include "../test_base.h"
#include <bits/alltypes.h>
#include <native_drawing/drawing_rect.h>
#include <native_drawing/drawing_canvas.h>
#include <native_drawing/drawing_mask_filter.h>
#include <native_drawing/drawing_filter.h>
#include "common/log_common.h"
#include "test_common.h"

class ConicPaths : public TestBase {
public:
    ConicPaths() {
        bitmapWidth_ = 920;
        bitmapHeight_ = 960;
        fileName_ = "conicpaths";
    }
    ~ConicPaths() = default;
    void makepath();
    std::vector<DrawRect> pathsBounds;
protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
};

#endif //CONIC_PATHS_H
