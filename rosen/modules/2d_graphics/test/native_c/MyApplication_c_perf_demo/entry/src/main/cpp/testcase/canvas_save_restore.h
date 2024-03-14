#ifndef CANVAS_SAVE_RESTORE_H
#define CANVAS_SAVE_RESTORE_H
#include "test_base.h"

class CanvasSaveRestore : public TestBase {
public:
    CanvasSaveRestore() : TestBase() {}
    ~CanvasSaveRestore() = default;
    int depth_ = 8;

protected:
    void OnTestFunction(OH_Drawing_Canvas *canvas) override;
    void OnTestPerformance(OH_Drawing_Canvas *canvas) override;
};
#endif // CANVAS_SAVE_RESTORE_H