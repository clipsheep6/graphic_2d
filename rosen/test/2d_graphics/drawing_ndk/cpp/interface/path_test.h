#ifndef PATH_TEST_H
#define PATH_TEST_H
#include "../test_base.h"
#include "test_common.h"
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_rect.h>
#include <native_drawing/drawing_canvas.h>

class PathTransformWithPerspectiveClip : public TestBase {
public:
    PathTransformWithPerspectiveClip(int type,bool bClip):TestBase(type),applyPerspectiveClip(bClip){
        fileName_ = "PathTransformWithPerspectiveClip";
    }
    ~PathTransformWithPerspectiveClip() = default;

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;//358 ms 100次
    bool applyPerspectiveClip;
};

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


class PathEffectCreateDashPathEffect : public TestBase {
public:
    PathEffectCreateDashPathEffect(int type):TestBase(type) {};
    ~PathEffectCreateDashPathEffect() = default;

protected:
    int rectWidth_ = 0;
    int rectHeight_ = 0;
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class PathCreate : public TestBase {
public:
    PathCreate(int type):TestBase(type) {};
    ~PathCreate() = default;

protected:
    int rectWidth_ = 0;
    int rectHeight_ = 0;
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};



class PathLineTo : public TestBase {
public:
    PathLineTo(int type):TestBase(type) {};
    ~PathLineTo() = default;

protected:
    int rectWidth_ = 0;
    int rectHeight_ = 0;
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class PathArcTo : public TestBase {
public:
    PathArcTo(int type):TestBase(type) {};
    ~PathArcTo() = default;

protected:
    int rectWidth_ = 0;
    int rectHeight_ = 0;
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class PathQuadTo : public TestBase {
public:
    PathQuadTo(int type):TestBase(type) {};
    ~PathQuadTo() = default;

protected:
    int rectWidth_ = 0;
    int rectHeight_ = 0;
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class PathCopy : public TestBase {
public:
    PathCopy(int type):TestBase(type) {};
    ~PathCopy() = default;

protected:
    int rectWidth_ = 0;
    int rectHeight_ = 0;
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class PathConicTo : public TestBase {
public:
    PathConicTo(int type):TestBase(type) {};
    ~PathConicTo() = default;

protected:
    int rectWidth_ = 0;
    int rectHeight_ = 0;
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class PathAddPathWithMatrixAndMode : public TestBase {
public:
    PathAddPathWithMatrixAndMode(int type):TestBase(type) {};
    ~PathAddPathWithMatrixAndMode() = default;

protected:
    int rectWidth_ = 0;
    int rectHeight_ = 0;
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};
class PathAddOvalWithInitialPoint : public TestBase {
    public:
    PathAddOvalWithInitialPoint(int type) : TestBase(type) {};
    ~PathAddOvalWithInitialPoint() = default;
    protected:
    int rectWidth_ = 0;
    int rectHeight_ = 0;
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
    private:
    TestRend renderer;
};

class PathAddArc : public TestBase{
    public:
    PathAddArc(int type) : TestBase(type) {};
    ~PathAddArc() = default;
    protected:
    int rectWidth_ = 0;
    int rectHeight_ = 0;
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
    private:
    TestRend renderer;
};
class PathTransform : public TestBase {
public:
    PathTransform(int type):TestBase(type) {};
    ~PathTransform() = default;
    
protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class PathAddOval : public TestBase {
public:
    PathAddOval(int type):TestBase(type) {};
    ~PathAddOval() = default;
    
protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class PathContains : public TestBase {
public:
    PathContains(int type):TestBase(type) {};
    ~PathContains() = default;
    
protected: 
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

#endif // CANVAS_TEST_H