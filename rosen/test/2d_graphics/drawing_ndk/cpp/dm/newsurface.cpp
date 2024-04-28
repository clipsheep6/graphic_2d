/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "newsurface.h"
#include <multimedia/image_framework/image_mdk_common.h>
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_color_filter.h>
#include <native_drawing/drawing_filter.h>
#include <native_drawing/drawing_image.h>
#include <native_drawing/drawing_mask_filter.h>
#include <native_drawing/drawing_matrix.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_round_rect.h>
#include <native_drawing/drawing_round_rect.h>
#include <native_drawing/drawing_shader_effect.h>
#include <native_drawing/drawing_point.h>
#include <native_drawing/drawing_sampling_options.h>
#include "test_common.h"
#include "common/log_common.h"

enum {
    kW = 300,
    kH = 140,
};

NewSurfaceGM::NewSurfaceGM() {
    // skia dm file gm/newsurface.cpp
    bitmapWidth_ = kW;
    bitmapHeight_ = kH;
    fileName_ = "surfacenew";
}
// 用例名: surfacenew 测试 OH_Drawing_CanvasDrawBitmap
// 迁移基于skia gm\surface.cpp->newsurface.cpp
void NewSurfaceGM::OnTestFunction(OH_Drawing_Canvas *canvas) {
    uint32_t w = 100;
    uint32_t h = 100;
    OH_Drawing_Bitmap *bm = OH_Drawing_BitmapCreate();
    OH_Drawing_BitmapFormat format = {OH_Drawing_ColorFormat::COLOR_FORMAT_RGBA_8888,
                                      OH_Drawing_AlphaFormat::ALPHA_FORMAT_PREMUL};
    OH_Drawing_BitmapBuild(bm, w, h, &format);

    // 将画布与bitmap绑定，画布画的内容会输出到绑定的bitmap内存中
    OH_Drawing_Canvas *bitmapCanvas = OH_Drawing_CanvasCreate();
    OH_Drawing_CanvasBind(bitmapCanvas, bm);
    // 清除画布内容
    // 生成效果图与skia一样，只是色差有点区别，色彩数据用的源码，skia源码照片生成色差与数据不符。
    OH_Drawing_CanvasClear(bitmapCanvas, OH_Drawing_ColorSetArgb(0xFF, 0xFF, 0x00, 0x00));

    float left = 10;
    float top = 10;
    // 其中surf->makeSurface以及canvas->drawImage在OH接口中并未实现，故用OH_Drawing_CanvasDrawBitmap接口去把图绘制出来
    OH_Drawing_CanvasDrawBitmap(canvas, bm, left, top);
    float left_move = 120;
    float top_move = 10;
    OH_Drawing_CanvasDrawBitmap(canvas, bm, left_move, top_move);
    // 释放内存
    OH_Drawing_BitmapDestroy(bm);
    OH_Drawing_CanvasDestroy(bitmapCanvas);
}