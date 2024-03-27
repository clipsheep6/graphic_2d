#ifndef DRAW_PATH_H
#define DRAW_PATH_H
#include "../test_base.h"

class PathBench : public TestBase {
public:
    PathBench() : TestBase() {}
    ~PathBench() = default;
    virtual void MakePath(OH_Drawing_Path* path) = 0;
protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class RectPathBench : public PathBench {
public:
    RectPathBench() {}
    ~RectPathBench() = default;
    void MakePath(OH_Drawing_Path* path) override;
};

class TrianglePathBench : public PathBench {
public:
    TrianglePathBench() {}
    ~TrianglePathBench() = default;
    void MakePath(OH_Drawing_Path* path) override;
};

class RotatedRectBench : public PathBench {
public:
    RotatedRectBench(int degrees) :fDegrees(degrees){}
    ~RotatedRectBench() = default;
    void MakePath(OH_Drawing_Path* path) override;
private:
    int fDegrees;
};

class LongCurvedPathBench :public PathBench {
public:
    LongCurvedPathBench() {}
    ~LongCurvedPathBench() = default;
    void MakePath(OH_Drawing_Path* path) override;
};
#endif // DRAW_PATH_H