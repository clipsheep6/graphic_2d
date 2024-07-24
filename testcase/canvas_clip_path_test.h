/*
Created on 2024/7/10.

Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
please include "napi/native_api.h".
*/
#ifndef MYAPPLICATION_CANVAS_CLIP_PATH_TEST_H
#define MYAPPLICATION_CANVAS_CLIP_PATH_TEST_H

#include "test_base.h"
class CanvasClipPathLine : public TestBase {
public:
    explicit CanvasClipPathLine(int type) : TestBase(type)
    {
        fileName_ = "CanvasClipPathLine";
    };
    ~CanvasClipPathLine() override {};
protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};
class CanvasClipPathLinePen : public TestBase {
public:
    explicit CanvasClipPathLinePen(int type) : TestBase(type)
    {
        fileName_ = "CanvasClipPathCurvePen";
    };
    ~CanvasClipPathLinePen() override {};
protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};
class CanvasClipPathLineBrush : public TestBase {
public:
    explicit CanvasClipPathLineBrush(int type) : TestBase(type)
    {
        fileName_ = "CanvasClipPathLineBrush";
    };
    ~CanvasClipPathLineBrush() override {};
protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};
class CanvasClipPathCurve : public TestBase {
public:
    explicit CanvasClipPathCurve(int type) : TestBase(type)
    {
        fileName_ = "CanvasClipPathCurve";
    };
    ~CanvasClipPathCurve() override {};
protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};
class CanvasClipPathCurvePen : public TestBase {
public:
    explicit CanvasClipPathCurvePen(int type) : TestBase(type)
    {
        fileName_ = "CanvasClipPathCurvePen";
    };
    ~CanvasClipPathCurvePen() override {};
protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};
class CanvasClipPathCurveBrush : public TestBase {
public:
    explicit CanvasClipPathCurveBrush(int type) : TestBase(type)
    {
        fileName_ = "CanvasClipPathCurveBrush";
    };
    ~CanvasClipPathCurveBrush() override {};
protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

#endif //MYAPPLICATION_CANVAS_CLIP_PATH_TEST_H
