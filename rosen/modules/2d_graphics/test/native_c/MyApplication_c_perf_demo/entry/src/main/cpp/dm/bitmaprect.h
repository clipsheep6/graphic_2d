#ifndef DEMO_DM_H
#define DEMO_DM_H
#include "../test_base.h"
#include <bits/alltypes.h>
#include <native_drawing/drawing_rect.h>
#include <native_drawing/drawing_canvas.h>
#include <native_drawing/drawing_bitmap.h>

class DrawBitmapRect4 : public TestBase {
public:
    DrawBitmapRect4(bool useIRect);
    ~DrawBitmapRect4()=default;
protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
    bool fUseIRect;    
    OH_Drawing_Bitmap* fBigBitmap;
};
#endif // AA_RECT_MODES_H