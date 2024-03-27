#ifndef DRAW_SHADER_LINEAR_H
#define DRAW_SHADER_LINEAR_H
#include "../test_base.h"

class GradientBench : public TestBase {
public:
    static const int kSize = 400;
    bool bOval_ = true;
    int effectType_ = 0;
    int gDataIndex_ = 0;
    enum {
        LINEAR = 0,
        RADIAL,
        SWEEP,
    };
    GradientBench(bool bOval,int effectType,int gDataIndex)
    {
        bOval_ = bOval;
        effectType_ = effectType;
        gDataIndex_ = gDataIndex;
    }
    ~GradientBench() = default;

protected:
    void OnTestFunction(OH_Drawing_Canvas *canvas) override;
    void OnTestPerformance(OH_Drawing_Canvas *canvas) override;
};
#endif // DRAW_BITMAP_TEST_H