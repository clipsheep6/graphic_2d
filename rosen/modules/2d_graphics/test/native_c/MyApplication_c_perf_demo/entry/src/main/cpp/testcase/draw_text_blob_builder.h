
#ifndef DRAW_TEXT_BLOB_BUILDER_H
#define DRAW_TEXT_BLOB_BUILDER_H
#include "test_base.h"
class DrawTextBlobBuilder : public TestBase {
  public:
    DrawTextBlobBuilder() : TestBase() {}
    ~DrawTextBlobBuilder() = default;

  protected:
    void OnTestFunction(OH_Drawing_Canvas *canvas) override;
    void OnTestPerformance(OH_Drawing_Canvas *canvas) override;
};
#endif // DRAW_TEXT_BLOB_BUILDER_H
