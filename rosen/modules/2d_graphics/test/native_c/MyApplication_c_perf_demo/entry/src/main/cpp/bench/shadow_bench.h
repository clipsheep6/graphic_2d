#ifndef DRAW_CANVAS_DRAW_SHADOW_TEST_H
#define DRAW_CANVAS_DRAW_SHADOW_TEST_H
#include "../test_base.h"

class ShadowBench : public TestBase {
    bool    fTransparent;
    bool    fForceGeometric;
public:
    ShadowBench(bool transparent,bool forceGeometric) : fTransparent(transparent), fForceGeometric(forceGeometric) {}
    ~ShadowBench() = default;

protected:
    void OnTestFunction(OH_Drawing_Canvas *canvas) override;
    void OnTestPerformance(OH_Drawing_Canvas *canvas) override;
    void genRRects();
    void destoryRRects();
};
#endif // DRAW_BITMAP_TEST_H