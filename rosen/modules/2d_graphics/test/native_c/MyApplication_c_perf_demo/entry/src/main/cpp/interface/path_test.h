#ifndef PATH_TEST_H
#define PATH_TEST_H
#include "../test_base.h"
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_rect.h>
#include <native_drawing/drawing_canvas.h>

class PathSetFillType : public TestBase {
public:
    PathSetFillType(int type,OH_Drawing_PathFillType fillType):TestBase(type),fType(fillType){
        fileName_ = "PathSetFillType";
    }
    ~PathSetFillType() = default;

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;//3827ms
    OH_Drawing_PathFillType fType;
};

class PathGetLength : public TestBase {
public:
    PathGetLength(int type,bool bForceClosed):TestBase(type),bClosed(bForceClosed){
        fileName_ = "PathGetLength";
    }
    ~PathGetLength() = default;

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;//1098ms 100次
    bool bClosed;
};

class PathClose : public TestBase {
public:
    PathClose(int type):TestBase(type){
        fileName_ = "PathClose";
    }
    ~PathClose() = default;

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;//1095ms 100次
};

class PathOffset : public TestBase {
public:
    PathOffset(int type):TestBase(type){
        fileName_ = "PathOffset";
    }
    ~PathOffset() = default;

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;//1512ms 100次
};

class PathReset : public TestBase {
public:
    PathReset(int type):TestBase(type){
        fileName_ = "PathReset";
    }
    ~PathReset() = default;

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;//1105ms 100次
};
#endif // CANVAS_TEST_H