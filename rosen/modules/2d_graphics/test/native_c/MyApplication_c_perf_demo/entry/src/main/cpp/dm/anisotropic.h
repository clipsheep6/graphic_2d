//
// Created on 2024/4/11.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#ifndef ANISOTROPIC_H
#define ANISOTROPIC_H
#include "../test_base.h"
#include "test_common.h"
#include <bits/alltypes.h>
#include <native_drawing/drawing_rect.h>
#include <native_drawing/drawing_canvas.h>

class Anisotropic : public TestBase {

uint32_t kImageSize     = 256;
uint32_t kSpacer        = 10;
uint32_t kNumVertImages = 5;
int kNumLines = 100;
float kAngleStep = 360.0f / kNumLines;
int kInnerOffset = 10;
public:
    Anisotropic() {
        bitmapWidth_ = 2 * kImageSize + 3 * kSpacer;
        bitmapHeight_ = kNumVertImages * kImageSize + (kNumVertImages + 1) * kSpacer;
        fileName_ = "anisotropic_hq";
    }
    ~Anisotropic() = default;
    
protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
    void BeforeDraw(OH_Drawing_Canvas *canvas);
    void Draw(OH_Drawing_Canvas* canvas, int x, int y, int xSize, int ySize);
};

#endif //ANISOTROPIC_H
