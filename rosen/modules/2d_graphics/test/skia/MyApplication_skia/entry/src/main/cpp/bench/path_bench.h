#ifndef DRAW_PATH_H
#define DRAW_PATH_H
#include "../test_base.h"
#include "SkPath.h"

class PathBench : public TestBase {
public:
    PathBench() : TestBase() {}
    ~PathBench() = default;
    virtual void MakePath(SkPath* path) = 0;
protected:
    void OnTestFunction(SkCanvas* canvas) override;
    void OnTestPerformance(SkCanvas* canvas) override;
};

class RectPathBench : public PathBench {
public:
    RectPathBench() {}
    ~RectPathBench() = default;
    void MakePath(SkPath* path) override;
};

class TrianglePathBench : public PathBench {
public:
    TrianglePathBench() {}
    ~TrianglePathBench() = default;
    void MakePath(SkPath* path) override;
};

class RotatedRectBench : public PathBench {
public:
    RotatedRectBench(int degrees) :fDegrees(degrees){}
    ~RotatedRectBench() = default;
    void MakePath(SkPath* path) override;
private:
    int fDegrees;
};

class LongCurvedPathBench :public PathBench {
public:
    LongCurvedPathBench() {}
    ~LongCurvedPathBench() = default;
    void MakePath(SkPath* path) override;
};
#endif // DRAW_PATH_H