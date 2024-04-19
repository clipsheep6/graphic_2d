#ifndef NEWSURFACEGM_H
#define NEWSURFACEGM_H
#include "../test_base.h"
#include <bits/alltypes.h>
#include <native_drawing/drawing_rect.h>
#include <native_drawing/drawing_canvas.h>

class NewSurfaceGM : public TestBase {
public:
    NewSurfaceGM();
    ~NewSurfaceGM() = default;

protected:
    void OnTestFunction(OH_Drawing_Canvas *canvas) override;
};
#endif // NEWSURFACEGM_H