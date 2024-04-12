#ifndef ARCOFZORROGM_H
#define ARCOFZORROGM_H
#include "../test_base.h"
#include <bits/alltypes.h>
#include <native_drawing/drawing_rect.h>
#include <native_drawing/drawing_canvas.h>

class ArcOfZorroGM : public TestBase {
public:
    ArcOfZorroGM();
    ~ArcOfZorroGM();

protected:
    void OnTestFunction(OH_Drawing_Canvas *canvas) override;
};
#endif // ARCOFZORROGM_H