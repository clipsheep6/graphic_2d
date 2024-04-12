//
// Created on 2024/4/9.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".
#include "../test_base.h"
#include <bits/alltypes.h>
#include <native_drawing/drawing_rect.h>
#include <native_drawing/drawing_canvas.h>

#ifndef STROKE_RECT_SHADER_H
#define STROKE_RECT_SHADER_H


class StrokeRectShader : public TestBase {
public:
    StrokeRectShader() {
        bitmapWidth_ = 690;
        bitmapHeight_ = 300;
    }
    ~StrokeRectShader() = default;
protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
};

#endif //STROKE_RECT_SHADER
