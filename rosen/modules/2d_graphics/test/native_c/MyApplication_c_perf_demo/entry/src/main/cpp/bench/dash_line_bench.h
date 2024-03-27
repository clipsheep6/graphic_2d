#ifndef DRAW_LINE_H
#define DRAW_LINE_H
#include "../test_base.h"

class DashLineBench : public TestBase {
public:
    DashLineBench() : TestBase() {};
    ~DashLineBench() = default;
    
    protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};
#endif // DRAW_LINE_H