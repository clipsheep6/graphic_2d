#ifndef DRAW_PATH_TRANSFORM_H
#define DRAW_PATH_TRANSFORM_H
#include "test_base.h"

class HairlinePathBench : public TestBase {
public:
    HairlinePathBench() : TestBase() {}
    ~HairlinePathBench() = default;
    virtual void MakePath(OH_Drawing_Path* path) = 0;

protected:
    void OnTestFunction(OH_Drawing_Canvas *canvas) override;
    void OnTestPerformance(OH_Drawing_Canvas *canvas) override;
    void makePathLine(OH_Drawing_Path *path);
    void makePathQuad(OH_Drawing_Path *path);
    void makePathConic(OH_Drawing_Path *path);
    void makePathCubic(OH_Drawing_Path *path);
};

class CubicPathBench : public HairlinePathBench {
public:
    CubicPathBench() {}
    ~CubicPathBench() = default;
    void MakePath(OH_Drawing_Path* path) override;
};
#endif // DRAW_BITMAP_TEST_H