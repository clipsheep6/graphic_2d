#ifndef DRAW_TEXT_BLOB_CREATE_TEST_H
#define DRAW_TEXT_BLOB_CREATE_TEST_H
#include "test_base.h"


class DrawTextBlobCreate : public TestBase {
    int         index;
public:
    DrawTextBlobCreate(int idx):index(idx){};
    ~DrawTextBlobCreate() = default;

protected:
    void OnTestFunction(OH_Drawing_Canvas *canvas) override;
    void OnTestPerformance(OH_Drawing_Canvas *canvas) override;
};
#endif // DRAW_BITMAP_TEST_H