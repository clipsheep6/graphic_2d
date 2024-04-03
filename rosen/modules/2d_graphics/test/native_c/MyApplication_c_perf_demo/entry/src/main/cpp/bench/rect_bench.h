#ifndef RECT_BENCH_H
#define RECT_BENCH_H
#include "../test_base.h"
#include <bits/alltypes.h>
#include <native_drawing/drawing_rect.h>
#include <native_drawing/drawing_canvas.h>
#include <native_drawing/drawing_pen.h>

class RectBench : public TestBase {
public:
    RectBench(int shift,bool bBrush = true,int stroke = 0,bool aa = true,bool perspective = true);
    ~RectBench(){};
    virtual void setupPaint(OH_Drawing_Canvas* canvas,OH_Drawing_Pen* pen){}
    virtual void onDraw(OH_Drawing_Canvas* canvas, uint32_t index);
    virtual void onCleanUp();
protected:
    enum {
        RAND_SIZE = 300, //skia benchmark:300
        W = DEFAULT_WIDTH,
        H = DEFAULT_HEIGHT,
    };
    OH_Drawing_Rect* rects[RAND_SIZE];
    uint32_t widths[RAND_SIZE];
    uint32_t heights[RAND_SIZE];
    uint32_t colors[RAND_SIZE];
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
    int     fStroke;
    bool    fAA;
    bool    fPerspective;
    bool    fBrush;
};

class RRectBench : public RectBench {
public:
    RRectBench(int shift):RectBench(shift) {};
    ~RRectBench(){};
protected:
    void onDraw(OH_Drawing_Canvas* canvas, uint32_t index) override;
};

class OvalBench : public RectBench {
public:
    OvalBench(int shift):RectBench(shift) {};
    ~OvalBench(){};
protected:
    void onDraw(OH_Drawing_Canvas* canvas, uint32_t index) override;
};

class BlitMaskBench : public RectBench {
  public:
    double FLAGS_strokeWidth = -1.0;
    enum {
        W = 640,
        H = 480,
        N = 300,
    };
    enum PointMode {
        kPoints_PointMode,
        kLines_PointMode,
        kPolygon_PointMode,
    };
    enum kMaskType { kMaskOpaque = 0,
                     kMaskBlack,
                     kMaskColor,
                     KMaskShader };

    PointMode fMode;
    BlitMaskBench(PointMode mode, BlitMaskBench::kMaskType type) : RectBench(2), fMode(mode), _type(type) {};
    ~BlitMaskBench(){};

  protected:
    kMaskType _type;
    void OnTestFunction(OH_Drawing_Canvas *canvas) override;
    void OnTestPerformance(OH_Drawing_Canvas *canvas) override;
};

//class RectBenchForPen : public RectBench {
//    int     fStroke;
//    bool    fAA;
//    bool    fPerspective;
//public:
//    RectBenchForPen(int shift,int stroke = 0,bool aa = true,bool perspective = false)
//    :RectBench::RectBench(shift),fStroke(stroke),fAA(aa),fPerspective(perspective){}
//    ~RectBenchForPen(){};
//    virtual void setupPaint(OH_Drawing_Canvas* canvas,OH_Drawing_Pen* pen, uint32_t index) {
//        RectBench::onDraw(canvas, index);
//    };
//protected:
//    void OnTestFunction(OH_Drawing_Canvas *canvas) override;
//    void OnTestPerformance(OH_Drawing_Canvas *canvas) override;
//    
//};

class SrcModeRectBench : public RectBench {
public:
    SrcModeRectBench():RectBench(1,false,0,true,false){}
    ~SrcModeRectBench() = default;

protected:
    void setupPaint(OH_Drawing_Canvas* canvas,OH_Drawing_Pen* pen) override;
};

#endif // RECT_BENCH_H