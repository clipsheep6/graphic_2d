#ifndef DRAW_IMAGE_RECT
#define DRAW_IMAGE_RECT
#include "test_base.h"

class DrawImageRect : public TestBase {
public:
    DrawImageRect() : TestBase() {}
    ~DrawImageRect() = default;

protected:
    void OnTestFunction(OH_Drawing_Canvas *canvas) override;
    void OnTestPerformance(OH_Drawing_Canvas *canvas) override;
};
#endif // DRAW_IMAGE_RECT