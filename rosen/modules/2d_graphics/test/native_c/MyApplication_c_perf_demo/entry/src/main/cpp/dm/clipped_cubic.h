#ifndef CLIPPED_CUBIC_H
#define CLIPPED_CUBIC_H
#include "../test_base.h"

#include <native_drawing/drawing_rect.h>
#include <native_drawing/drawing_canvas.h>

class ClippedCubic : public TestBase {
  public:
    ClippedCubic();
    ~ClippedCubic() = default;

  protected:
    void OnTestFunction(OH_Drawing_Canvas *canvas) override;
};
#endif // CLIPPED_CUBIC_H