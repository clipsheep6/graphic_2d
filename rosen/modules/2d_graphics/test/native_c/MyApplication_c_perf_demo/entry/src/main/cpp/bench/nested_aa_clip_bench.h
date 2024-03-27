#ifndef DRAW_PATH_ADD_ROUND_RECT_TEST_H
#define DRAW_PATH_ADD_ROUND_RECT_TEST_H
#include "../test_base.h"
#include "..//test_common.h"

class NestedAAClipBench : public TestBase {
    OH_Drawing_Rect* fDrawRect;
    static const int kNestingDepth = 3;
    static const int kImageSize = 400;
    OH_Drawing_Point2D fSize[kNestingDepth+1];
    bool fDoAA = true;
    TestRend fRandom;
    
public:
    NestedAAClipBench() ;
    ~NestedAAClipBench() = default;

protected:
    void OnTestFunction(OH_Drawing_Canvas *canvas) override;
    void OnTestPerformance(OH_Drawing_Canvas *canvas) override;
    void recurse(OH_Drawing_Canvas* canvas,int depth,const OH_Drawing_Point2D& offset);
};
#endif // DRAW_BITMAP_TEST_H