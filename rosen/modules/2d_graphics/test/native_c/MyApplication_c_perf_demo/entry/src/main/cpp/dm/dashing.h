#ifndef DASHING_H
#define DASHING_H
#include "../test_base.h"
#include <bits/alltypes.h>
#include <native_drawing/drawing_rect.h>
#include <native_drawing/drawing_canvas.h>

class Dashing : public TestBase {
public:
    Dashing();
    ~Dashing()=default;
protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
};

class Dashing2 : public TestBase {
public:
    Dashing2();
    ~Dashing2()=default;
protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
};

class Dashing4 : public TestBase {
public:
    Dashing4();
    ~Dashing4()=default;
protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
};
#endif // AA_RECT_MODES_H