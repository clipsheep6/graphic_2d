//
// Created on 2024/7/16.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#ifndef MYAPPLICATION_CANVAS_DRAW_IMAGE_RECT_WITH_SRC_H
#define MYAPPLICATION_CANVAS_DRAW_IMAGE_RECT_WITH_SRC_H
#include "test_base.h"
#include "test_common.h"
class OhCanvasDrawImageRectWithSrc : public TestBase {
public:
    explicit OhCanvasDrawImageRectWithSrc(int type) : TestBase(type)
    {
        fileName_ = "OHCanvasDrawImageRectWithSrc";
    };
    ~OhCanvasDrawImageRectWithSrc() override {};

protected:
    int rectWidth_ = 0;  // 0 矩形宽度
    int rectHeight_ = 0; // 0 矩形高度
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

#endif //MYAPPLICATION_CANVAS_DRAW_IMAGE_RECT_WITH_SRC_H
