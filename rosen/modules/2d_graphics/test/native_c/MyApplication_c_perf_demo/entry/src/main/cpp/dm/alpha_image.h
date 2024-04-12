#ifndef ALPHA_IMAGE_H
#define ALPHA_IMAGE_H
#include "../test_base.h"
#include <bits/alltypes.h>
#include <native_drawing/drawing_rect.h>
#include <native_drawing/drawing_canvas.h>

class AlphaImage : public TestBase {
public:
    AlphaImage();
    ~AlphaImage()=default;
protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
};

class AlphaImageAlphaTint : public TestBase {
public:
    AlphaImageAlphaTint();
    ~AlphaImageAlphaTint()=default;
protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
};
#endif // AA_RECT_MODES_H