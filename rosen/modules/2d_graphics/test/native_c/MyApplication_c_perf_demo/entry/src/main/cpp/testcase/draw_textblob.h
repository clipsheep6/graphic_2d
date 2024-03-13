#ifndef DRAW_TEXTBLOB_TEST_H
#define DRAW_TEXTBLOB_TEST_H
#include "test_base.h"

class DrawTextBlob : public TestBase {
public:
    DrawTextBlob() : TestBase() {}
    ~DrawTextBlob() = default;
protected:
    void OnTestFunctionCpu(OH_Drawing_Canvas* canvas) override;
    void OnTestPerformanceCpu(OH_Drawing_Canvas* canvas) override;
};
#endif // DRAW_TEXTBLOB_TEST_H