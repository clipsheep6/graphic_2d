
#ifndef BLUR_BENCH_H
#define BLUR_BENCH_H
#include "../test_base.h"
#include "..//test_common.h"
#include <native_drawing/drawing_mask_filter.h>

#define BLUR_MINI 0.01f
#define BLUR_SMALL 2.0f
#define BLUR_BIG 10.0f
#define BLUR_REAL 0.5f
#define BLUR_REALBIG 100.5f

class BlurBench : public TestBase {
    TestRend fRandom;
  public:
    float fRadius = 0;
    OH_Drawing_BlurType type_;
    BlurBench(float rab, OH_Drawing_BlurType type) {
        fRadius = rab;
        type_ = type;
    }
    ~BlurBench() = default;

  protected:
    void OnTestFunction(OH_Drawing_Canvas *canvas) override;
    void OnTestPerformance(OH_Drawing_Canvas *canvas) override;
};
#endif // BLUR_BENCH_H
