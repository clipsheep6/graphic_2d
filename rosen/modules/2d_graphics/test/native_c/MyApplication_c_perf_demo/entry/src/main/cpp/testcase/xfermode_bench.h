#ifndef DRAW_TEXT_BLOB_CREATE_TEST_H
#define DRAW_TEXT_BLOB_CREATE_TEST_H
#include "test_base.h"


class XfermodeBench : public TestBase {
    int         index;
public:
    enum {
    FromText = 1,
    FromPosText ,
    FromString ,
    };
    XfermodeBench(int idx):index(idx){};
    ~XfermodeBench() = default;

protected:
    void OnTestFunction(OH_Drawing_Canvas *canvas) override;
    void OnTestPerformance(OH_Drawing_Canvas *canvas) override;
};
#endif // DRAW_BITMAP_TEST_H