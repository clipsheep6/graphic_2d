#ifndef ALPHA_GRADIENTS_H
#define ALPHA_GRADIENTS_H
#include "../test_base.h"
#include "test_common.h"
#include <native_drawing/drawing_rect.h>
#include <native_drawing/drawing_canvas.h>

typedef uint32_t SkColor;
typedef unsigned int U8CPU;

class AlphaGradients : public TestBase {
  public:
    AlphaGradients();
    ~AlphaGradients() = default;

  protected:
    void OnTestFunction(OH_Drawing_Canvas *canvas) override;
    void draw_grad(OH_Drawing_Canvas *canvas, DrawRect &r, uint32_t c0, uint32_t c1, bool doPreMul);
};
#endif // ALPHA_GRADIENTS_H