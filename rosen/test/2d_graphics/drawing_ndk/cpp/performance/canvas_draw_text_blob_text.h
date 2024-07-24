//
// Created on 2024/7/17.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#ifndef MYAPPLICATION_CANVAS_DRAW_TEXT_BLOB_TEXT_H
#define MYAPPLICATION_CANVAS_DRAW_TEXT_BLOB_TEXT_H
#include "test_base.h"
#include "test_common.h"
class CanvasDrawTextBlobLong : public TestBase {
public:
    explicit CanvasDrawTextBlobLong(int type) : TestBase(type)
    {
        fileName_ = "CanvasDrawTextBlobLong";
    };
    ~CanvasDrawTextBlobLong() override {};
protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};
class CanvasDrawTextBlobLongPen : public TestBase {
public:
    explicit CanvasDrawTextBlobLongPen(int type) : TestBase(type)
    {
        fileName_ = "CanvasDrawTextBlobLongPen";
    };
    ~CanvasDrawTextBlobLongPen() override {};
protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};
class CanvasDrawTextBlobLongBrush : public TestBase {
public:
    explicit CanvasDrawTextBlobLongBrush(int type) : TestBase(type)
    {
        fileName_ = "CanvasClipPathLineBrush";
    };
    ~CanvasDrawTextBlobLongBrush() override {};
protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};


class CanvasDrawTextBlobMax : public TestBase {
public:
    explicit CanvasDrawTextBlobMax(int type) : TestBase(type)
    {
        fileName_ = "CanvasDrawTextBlobMax";
    };
    ~CanvasDrawTextBlobMax() override {};
protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};
class CanvasDrawTextBlobMaxPen : public TestBase {
public:
    explicit CanvasDrawTextBlobMaxPen(int type) : TestBase(type)
    {
        fileName_ = "CanvasDrawTextBlobMaxPen";
    };
    ~CanvasDrawTextBlobMaxPen() override {};
protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};
class CanvasDrawTextBlobMaxBrush : public TestBase {
public:
    explicit CanvasDrawTextBlobMaxBrush(int type) : TestBase(type)
    {
        fileName_ = "CanvasDrawTextBlobMaxBrush";
    };
    ~CanvasDrawTextBlobMaxBrush() override {};
protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};
#endif //MYAPPLICATION_CANVAS_DRAW_TEXT_BLOB_TEXT_H
