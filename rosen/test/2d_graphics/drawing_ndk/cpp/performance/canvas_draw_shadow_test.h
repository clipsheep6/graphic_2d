//
// Created on 2024/7/18.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#ifndef MYAPPLICATION_CANVAS_DRAW_SHADOW_TEST_H
#define MYAPPLICATION_CANVAS_DRAW_SHADOW_TEST_H
#include "test_base.h"
#include "test_common.h"

class CanvasDrawShadowLine : public TestBase {
public:
    explicit CanvasDrawShadowLine(int type) : TestBase(type)
    {
        fileName_ = "CanvasDrawShadowLine";
    };
    ~CanvasDrawShadowLine() override {};
protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};
class CanvasDrawShadowLinePen : public TestBase {
public:
    explicit CanvasDrawShadowLinePen(int type) : TestBase(type)
    {
        fileName_ = "CanvasDrawShadowLinePen";
    };
    ~CanvasDrawShadowLinePen() override {};
protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};
class CanvasDrawShadowLineBrush : public TestBase {
public:
    explicit CanvasDrawShadowLineBrush(int type) : TestBase(type)
    {
        fileName_ = "CanvasClipPathLineBrush";
    };
    ~CanvasDrawShadowLineBrush() override {};
protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class CanvasDrawShadowCurve : public TestBase {
public:
    explicit CanvasDrawShadowCurve(int type) : TestBase(type)
    {
        fileName_ = "CanvasDrawShadowCurve";
    };
    ~CanvasDrawShadowCurve() override {};
protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};
class CanvasDrawShadowCurvePen : public TestBase {
public:
    explicit CanvasDrawShadowCurvePen(int type) : TestBase(type)
    {
        fileName_ = "CanvasDrawShadowCurvePen";
    };
    ~CanvasDrawShadowCurvePen() override {};
protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};
class CanvasDrawShadowCurveBrush : public TestBase {
public:
    explicit CanvasDrawShadowCurveBrush(int type) : TestBase(type)
    {
        fileName_ = "CanvasDrawShadowCurveBrush";
    };
    ~CanvasDrawShadowCurveBrush() override {};
protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

#endif //MYAPPLICATION_CANVAS_DRAW_SHADOW_TEST_H
