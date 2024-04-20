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
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;//712 ms 100次
    OH_Drawing_PathFillType fType;
};

class PathGetLength : public TestBase {
public:
    PathGetLength(int type,bool bForceClosed):TestBase(type),bClosed(bForceClosed){
        fileName_ = "PathGetLength";
    }
    ~PathGetLength() = default;

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;//768 ms 100次
    bool bClosed;
};

class PathClose : public TestBase {
public:
    PathClose(int type):TestBase(type){
        fileName_ = "PathClose";
    }
    ~PathClose() = default;

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;//1544 ms 100次
};

class PathOffset : public TestBase {
public:
    PathOffset(int type):TestBase(type){
        fileName_ = "PathOffset";
    }
    ~PathOffset() = default;

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;//454 ms 100次
};

class PathReset : public TestBase {
public:
    PathReset(int type):TestBase(type){
        fileName_ = "PathReset";
    }
    ~PathReset() = default;

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;//2613 ms 100次
};

class PathCubicTo : public TestBase {
public:
    PathCubicTo(int type):TestBase(type){
        fileName_ = "PathCubicTo";
    }
    ~PathCubicTo() = default;

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;//2041 ms 100次
};

class PathRMoveTo : public TestBase {
public:
    PathRMoveTo(int type):TestBase(type){
        fileName_ = "PathRMoveTo";
    }
    ~PathRMoveTo() = default;

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;//1299 ms 100次
};

class PathRLineTo : public TestBase {
public:
    PathRLineTo(int type):TestBase(type){
        fileName_ = "PathRLineTo";
    }
    ~PathRLineTo() = default;

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;//767 ms 100次
};

class PathRQuadTo : public TestBase {
public:
    PathRQuadTo(int type):TestBase(type){
        fileName_ = "PathRQuadTo";
    }
    ~PathRQuadTo() = default;

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;//1184 ms 100次
};

class PathRConicTo : public TestBase {
public:
    PathRConicTo(int type):TestBase(type){
        fileName_ = "PathRConicTo";
    }
    ~PathRConicTo() = default;

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;//931 ms 100次
};

class PathRCubicTo : public TestBase {
public:
    PathRCubicTo(int type):TestBase(type){
        fileName_ = "PathRCubicTo";
    }
    ~PathRCubicTo() = default;

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;//1124 ms 100次
};

class PathAddRect : public TestBase {
public:
    PathAddRect(int type):TestBase(type){
        fileName_ = "PathAddRect";
    }
    ~PathAddRect() = default;

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;//727 ms 100次
};

class PathAddRectWithInitialCorner : public TestBase {
public:
    PathAddRectWithInitialCorner(int type):TestBase(type){
        fileName_ = "PathAddRectWithInitialCorner";
    }
    ~PathAddRectWithInitialCorner() = default;

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;//739 ms 100次
};

class PathAddRoundRect : public TestBase {
public:
    PathAddRoundRect(int type):TestBase(type){
        fileName_ = "PathAddRoundRect";
    }
    ~PathAddRoundRect() = default;

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;//783 ms 100次
};

class PathAddPath : public TestBase {
public:
    PathAddPath(int type):TestBase(type){
        fileName_ = "PathAddPath";
    }
    ~PathAddPath() = default;

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;//2348 ms 100次
};

class PathAddPathWithMode : public TestBase {
public:
    PathAddPathWithMode(int type,OH_Drawing_PathAddMode mode):TestBase(type),addMode(mode){
        fileName_ = "PathAddPathWithMode";
    }
    ~PathAddPathWithMode() = default;

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;//2133 ms 100次
    OH_Drawing_PathAddMode addMode;
};

class PathAddPathWithOffsetAndMode : public TestBase {
public:
    PathAddPathWithOffsetAndMode(int type,OH_Drawing_PathAddMode mode):TestBase(type),addMode(mode){
        fileName_ = "PathAddPathWithOffsetAndMode";
    }
    ~PathAddPathWithOffsetAndMode() = default;

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;//3547 ms 100次
    OH_Drawing_PathAddMode addMode;
};
#endif // CANVAS_TEST_H