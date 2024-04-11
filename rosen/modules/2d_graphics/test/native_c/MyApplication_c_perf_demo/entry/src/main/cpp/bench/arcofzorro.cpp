#include "arcofzorro.h"
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_matrix.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_round_rect.h>
#include <native_drawing/drawing_round_rect.h>
#include <native_drawing/drawing_shader_effect.h>
#include <native_drawing/drawing_point.h>
#include "test_common.h"
#include "common/log_common.h"


ArcOfZorroGM::ArcOfZorroGM()
{
    // skia dm file gm/arcofzorro.cpp
    bitmapWidth_ = 1000;
    bitmapHeight_ = 1000;
    fileName_ = "arcofzorro";
}

ArcOfZorroGM::~ArcOfZorroGM() {}

void ArcOfZorroGM::OnTestFunction(OH_Drawing_Canvas *canvas)
{
    // 用例名: arcofzorro 测试 OH_Drawing_CanvasDrawArc
    // 迁移基于skia arcofzorro->arcofzorro
    DRAWING_LOGI("ArcOfZorroGM::OnTestFunction start");
    // 使用指定颜色设置清空画布底色
    OH_Drawing_CanvasClear(canvas, 0xFFCCCCCC);
    // 创建一个矩形对象
    TestRend rand;
    float x = 10;
    float y = 10;
    float w = 200;
    float h = 200;
    OH_Drawing_Rect *rect = OH_Drawing_RectCreate(x, y, x + w, y + h);
    // 创建画笔pen对象
    OH_Drawing_Pen *pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetAntiAlias(pen, true);
    OH_Drawing_PenSetWidth(pen, 35);

    int xOffset = 0;
    int yOffset = 0;
    int direction = 0;
    for (float arc = 134.0f; arc < 136.0f; arc += 0.01f) {
        DRAWING_LOGI("ArcOfZorroGM::arc");
        uint32_t color = rand.nextU();
        color |= 0xff000000;
        OH_Drawing_PenSetColor(pen, color_to_565(color));
        OH_Drawing_CanvasSave(canvas);
        OH_Drawing_CanvasTranslate(canvas, xOffset, yOffset);
        OH_Drawing_CanvasDrawArc(canvas, rect, 0, arc);
        OH_Drawing_CanvasAttachPen(canvas, pen);
        OH_Drawing_CanvasRestore(canvas);

        switch (direction) {
            case 0:
                DRAWING_LOGI("ArcOfZorroGM::OnTestFunction direction0");
                xOffset += 10;
                if (xOffset >= 700) {
                    direction = 1;
                }
                break;
            case 1:
                DRAWING_LOGI("ArcOfZorroGM::OnTestFunction direction1");
                xOffset -= 10;
                yOffset += 10;
                if (xOffset < 50) {
                    direction = 2;
                }
                break;
            case 2:
                DRAWING_LOGI("ArcOfZorroGM::OnTestFunction direction2");
                xOffset += 10;
                break;
        }
    }
    DRAWING_LOGI("ArcOfZorroGM::OnTestFunction end");
    OH_Drawing_CanvasRestore(canvas);
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_RectDestroy(nullptr);
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_PenDestroy(pen);
    pen = nullptr;
}
