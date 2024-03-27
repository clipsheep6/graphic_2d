#ifndef DRAW_TEXT_BLOB_CREATE_TEST_H
#define DRAW_TEXT_BLOB_CREATE_TEST_H
#include "../test_base.h"
#include <native_drawing/drawing_types.h>

class XfermodeBench : public TestBase {
    int         index;
public:
    enum {
    FromText = 1,
    FromPosText ,
    FromString ,
    };
    XfermodeBench(int idx,OH_Drawing_BlendMode m = BLEND_MODE_SCREEN):index(idx),mode(m){};
    ~XfermodeBench() = default;

protected:
    void OnTestFunction(OH_Drawing_Canvas *canvas) override;
    void OnTestPerformance(OH_Drawing_Canvas *canvas) override;
    OH_Drawing_BlendMode mode;
};
#endif // DRAW_BITMAP_TEST_H