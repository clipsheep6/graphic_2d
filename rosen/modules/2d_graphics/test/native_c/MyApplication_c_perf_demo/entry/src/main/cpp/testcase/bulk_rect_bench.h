#ifndef DRAW_BITMAP_RECT_H
#define DRAW_BITMAP_RECT_H
#include "test_base.h"

class BulkRectBench : public TestBase {
public:
    BulkRectBench(int type) : type_(type) {}
    ~BulkRectBench() = default;
    enum {
        BITMAP_RECT = 0,
        IMAGE_RECT,
    };

protected:
    int type_ = BITMAP_RECT;
    void OnTestFunction(OH_Drawing_Canvas *canvas) override;
    void OnTestPerformance(OH_Drawing_Canvas *canvas) override;
};
#endif // DRAW_BITMAP_RECT_H