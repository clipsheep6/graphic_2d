#ifndef DRAW_TEXTBLOB_H
#define DRAW_TEXTBLOB_H
#include "test_base.h"

class TextBlobCachedBench : public TestBase {
public:
    TextBlobCachedBench() : TestBase() {}
    ~TextBlobCachedBench() = default;
protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};
#endif // DRAW_TEXTBLOB_H