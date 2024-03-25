#ifndef DRAW_PEN_RECT_TEST_H
#define DRAW_PEN_RECT_TEST_H
#include "test_base.h"
#include "rect_bench.h"

class RectBenchForPen : public RectBench {
    int     fStroke;
    bool    fAA;
    bool    fPerspective;
public:
    RectBenchForPen(int shift,int stroke = 0,bool aa = true,bool perspective = false)
    :RectBench::RectBench(shift),fStroke(stroke),fAA(aa),fPerspective(perspective){}
    ~RectBenchForPen(){};
    virtual void onDrawPen(OH_Drawing_Canvas* canvas,OH_Drawing_Pen* pen, uint32_t index) {
        RectBench::onDraw(canvas, index);
    };
protected:
    void OnTestFunction(OH_Drawing_Canvas *canvas) override;
    void OnTestPerformance(OH_Drawing_Canvas *canvas) override;
    
};

class SrcModeRectBench : public RectBenchForPen {
public:
    SrcModeRectBench():RectBenchForPen(1,0,true,false){}
    ~SrcModeRectBench() = default;

protected:
    void onDrawPen(OH_Drawing_Canvas* canvas,OH_Drawing_Pen* pen, uint32_t index) override;
};
#endif // DRAW_BITMAP_TEST_H