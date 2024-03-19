#ifndef DRAW_SHADER_LINEAR_H
#define DRAW_SHADER_LINEAR_H
#include "test_base.h"

class DrawShaderEffect : public TestBase {
    static const int kSize = 400;
    bool bOval_ = true;
    int effectType_ = 0;
    int gDataIndex_ = 0;
public:
    DrawShaderEffect(bool bOval,int effectType,int gDataIndex)
    {
        bOval_ = bOval;
        effectType_ = effectType;
        gDataIndex_ = gDataIndex;
    }
    ~DrawShaderEffect() = default;

protected:
    void OnTestFunction(OH_Drawing_Canvas *canvas) override;
    void OnTestPerformance(OH_Drawing_Canvas *canvas) override;
};
#endif // DRAW_BITMAP_TEST_H