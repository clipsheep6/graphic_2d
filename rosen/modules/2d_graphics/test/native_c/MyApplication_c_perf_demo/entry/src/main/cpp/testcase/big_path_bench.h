#ifndef DRAW_BIG_PATH_TEST_H
#define DRAW_BIG_PATH_TEST_H
#include "test_base.h"
#include <native_drawing/drawing_path.h>

class BigPathBench : public TestBase {
public:
    enum Align {
        kLeft_Align,
        kMiddle_Align,
        kRight_Align
    };
public:
    BigPathBench(Align align,bool round);
    ~BigPathBench();
protected:
    void OnTestFunction(OH_Drawing_Canvas *canvas) override;
    void OnTestPerformance(OH_Drawing_Canvas *canvas) override;
    Align       fAlign;
    bool        fRound;
    OH_Drawing_Path* fPath;
};
#endif // DRAW_BITMAP_TEST_H