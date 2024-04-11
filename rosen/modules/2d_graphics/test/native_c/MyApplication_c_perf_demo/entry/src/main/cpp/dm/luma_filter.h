#ifndef LUMA_FILTER_H
#define LUMA_FILTER_H
#include "../test_base.h"

#include <native_drawing/drawing_rect.h>
#include <native_drawing/drawing_canvas.h>

typedef uint32_t SkColor;
typedef unsigned int U8CPU;

class LumaFilter : public TestBase {
  public:
    LumaFilter();
    ~LumaFilter() = default;

  protected:
    void OnTestFunction(OH_Drawing_Canvas *canvas) override;
};
#endif // LUMA_FILTER_H
