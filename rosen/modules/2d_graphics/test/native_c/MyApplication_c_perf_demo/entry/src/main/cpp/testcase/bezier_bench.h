#ifndef DRAW_PEN_BEIZER_TEST_H
#define DRAW_PEN_BEIZER_TEST_H
#include "test_base.h"
#include <native_drawing/drawing_typeface.h>

class BezierBench : public TestBase {
public:    
    enum CapType
    {
        kButt_Cap,
        kSquare_Cap,
        kRound_Cap
    };
    
    enum JoinType
    {
        kRound_Join,
        kBevel_Join,
        kMiter_Join
    };
    
    enum DrawType
    {
        draw_quad,
        draw_cubic
    };
public:
    BezierBench(CapType c,JoinType j,float w,DrawType d) : capType(c),joinType(j),fWidth(w),drawType(d) {}
    ~BezierBench() = default;

protected:
    void OnTestFunction(OH_Drawing_Canvas *canvas) override;
    void OnTestPerformance(OH_Drawing_Canvas *canvas) override;
    CapType capType;
    JoinType joinType;
    DrawType drawType;
    float   fWidth;
};
#endif // DRAW_BITMAP_TEST_H