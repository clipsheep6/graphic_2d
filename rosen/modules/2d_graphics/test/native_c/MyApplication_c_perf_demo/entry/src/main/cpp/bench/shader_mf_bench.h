#ifndef CANVAS_SAVE_LAYER_H
#define CANVAS_SAVE_LAYER_H
#include "../test_base.h"

class ShaderMFBench : public TestBase {
public:
    ShaderMFBench() : TestBase() {}
    ~ShaderMFBench() = default;
    int depth_ = 8;

protected:
    void OnTestFunction(OH_Drawing_Canvas *canvas) override;
    void OnTestPerformance(OH_Drawing_Canvas *canvas) override;
};
#endif // CANVAS_SAVE_LAYER_H