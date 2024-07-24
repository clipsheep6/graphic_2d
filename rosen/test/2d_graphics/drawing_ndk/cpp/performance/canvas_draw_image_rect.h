//
// Created on 2024/7/16.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#ifndef MYAPPLICATION_CANVAS_DRAW_IMAGE_RECT_H
#define MYAPPLICATION_CANVAS_DRAW_IMAGE_RECT_H

#include "test_base.h"
#include "test_common.h"
class OHCanvasDrawImageRect : public TestBase {  
public:
    explicit OHCanvasDrawImageRect(int type) : TestBase(type)
    {
        fileName_ = "OHCanvasDrawImageRect";
    };
    ~OHCanvasDrawImageRect() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class OHCanvasDrawImageRectPen : public TestBase {
public:
    explicit OHCanvasDrawImageRectPen(int type) : TestBase(type)
    {
        fileName_ = "OHCanvasDrawImageRectPen";
    };
    ~OHCanvasDrawImageRectPen() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class OHCanvasDrawImageRectBrush : public TestBase {
public:
    explicit OHCanvasDrawImageRectBrush(int type) : TestBase(type)
    {
        fileName_ = "OHCanvasDrawImageRectBrush";
    };
    ~OHCanvasDrawImageRectBrush() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

#endif //MYAPPLICATION_CANVAS_DRAW_IMAGE_RECT_H
