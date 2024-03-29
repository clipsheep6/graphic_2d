#ifndef BLUR_CIRCLES_GM_H
#define BLUR_CIRCLES_GM_H
#include "../test_base.h"
#include <bits/alltypes.h>

class BlurCirclesGM : public TestBase {
public:
    BlurCirclesGM();
    ~BlurCirclesGM() = default;
    
protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
    float ConvertRadiusToSigma(float radius);
};
#endif // BLUR_CIRCLES_GM_H
