#ifndef BUGFIX7792_H
#define BUGFIX7792_H
#include "../test_base.h"
#include <bits/alltypes.h>
#include <native_drawing/drawing_rect.h>
#include <native_drawing/drawing_canvas.h>

// 源代码用例名bug7792位置gm\pathfill.cpp.这里用BugFix7792类
class BugFix7792 : public TestBase {
public:
    BugFix7792();
    ~BugFix7792();

protected:
    void OnTestFunction(OH_Drawing_Canvas *canvas) override;
};
#endif // BUGFIX7792_H