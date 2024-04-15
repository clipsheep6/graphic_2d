/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
 
#include "test_base.h"
#include <fcntl.h>
#include "common/log_common.h"

#include <chrono>
#include <sstream>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <memory>
#include <unordered_map>

#include <multimedia/image_framework/image_pixel_map_mdk.h>
#include <multimedia/image_framework/image_packer_mdk.h>


#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_matrix.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_round_rect.h>
#include <native_drawing/drawing_shader_effect.h>
#include <native_drawing/drawing_point.h>
#include <native_drawing/drawing_image.h>
#include <native_drawing/drawing_filter.h>
#include <native_drawing/drawing_sampling_options.h>
#include <native_drawing/drawing_path_effect.h>
#include <native_drawing/drawing_filter.h>
#include <native_drawing/drawing_mask_filter.h>

void TestBase::SetFileName(std::string fileName)
{
    fileName_ = fileName;
}

void TestBase::SetTestCount(uint32_t testCount) {
    testCount_ = testCount;
}

OH_Drawing_Bitmap* TestBase::GetBitmap()
{
    return bitmap_;
}

uint32_t TestBase::GetTime()
{
    return usedTime_;
}

void TestBase::TestFunctionCpu(napi_env env)
{
    CreateBitmapCanvas();
    OnTestFunction(bitmapCanvas_);
    BitmapCanvasToFile(env);
}


void TestBase::TestFunctionGpu(OH_Drawing_Canvas *canvas) { OnTestFunction(canvas); }

void TestBase::TestPerformanceGpu(OH_Drawing_Canvas *canvas) {
    auto timeZero = std::chrono::high_resolution_clock::now();
    auto start = std::chrono::high_resolution_clock::now();
    DRAWING_LOGE("DrawingApiTest Started: [%{public}lld]",
                 std::chrono::duration_cast<std::chrono::milliseconds>(start - timeZero).count());

    OnTestPerformance(canvas);

    auto end = std::chrono::high_resolution_clock::now();
    DRAWING_LOGE("DrawingApiTest Finished: [%{public}lld]",
                 std::chrono::duration_cast<std::chrono::milliseconds>(end - timeZero).count());
    DRAWING_LOGE("DrawingApiTest TotalApiCallCount: [%{public}u]", testCount_);
    usedTime_ = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    DRAWING_LOGE("DrawingApiTest TotalApiCallTime: [%{public}u]", usedTime_);
}

void TestBase::TestPerformanceCpu(napi_env env)
{
    CreateBitmapCanvas();
    auto timeZero = std::chrono::high_resolution_clock::now();
    auto start = std::chrono::high_resolution_clock::now();
    DRAWING_LOGE("DrawingApiTest Started: [%{public}lld]",
                 std::chrono::duration_cast<std::chrono::milliseconds>(start - timeZero).count());

    OnTestPerformance(bitmapCanvas_);

    auto end = std::chrono::high_resolution_clock::now();
    DRAWING_LOGE("DrawingApiTest Finished: [%{public}lld]",
                 std::chrono::duration_cast<std::chrono::milliseconds>(end - timeZero).count());
    DRAWING_LOGE("DrawingApiTest TotalApiCallCount: [%{public}u]", testCount_);
    usedTime_ = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    DRAWING_LOGE("DrawingApiTest TotalApiCallTime: [%{public}u]", usedTime_);
    BitmapCanvasToFile(env);
}

void TestBase::CreateBitmapCanvas()
{
    // 创建一个bitmap对象
    if (bitmap_) {
        OH_Drawing_BitmapDestroy(bitmap_);
    }
    bitmap_ = OH_Drawing_BitmapCreate();
    // 定义bitmap的像素格式
    OH_Drawing_BitmapFormat cFormat{COLOR_FORMAT_BGRA_8888, ALPHA_FORMAT_OPAQUE};
    // 构造对应格式的bitmap
    OH_Drawing_BitmapBuild(bitmap_, bitmapWidth_, bitmapHeight_, &cFormat);

    // 创建一个canvas对象
    if (bitmapCanvas_) {
        OH_Drawing_CanvasDestroy(bitmapCanvas_);
    }
    bitmapCanvas_ = OH_Drawing_CanvasCreate();
    // 将画布与bitmap绑定，画布画的内容会输出到绑定的bitmap内存中
    OH_Drawing_CanvasBind(bitmapCanvas_, bitmap_);
    // 使用白色清除画布内容
    OH_Drawing_CanvasClear(bitmapCanvas_, OH_Drawing_ColorSetArgb(backgroundA_, backgroundR_, backgroundG_, backgroundB_));
}

void TestBase::BitmapCanvasToFile(napi_env env)
{
    DRAWING_LOGE("BitmapCanvasToFile");
    //创建pixmap
    napi_value pixelMap = nullptr;
    struct OhosPixelMapCreateOps createOps;
    createOps.width = bitmapWidth_;
    createOps.height = bitmapHeight_;
    createOps.pixelFormat = 3;
    createOps.alphaType = 0;
    createOps.editable = 1;
    size_t bufferSize = createOps.width * createOps.height * 4;
    void *bitmapAddr = OH_Drawing_BitmapGetPixels(bitmap_);
    int32_t res = OH_PixelMap_CreatePixelMap(env, createOps, (uint8_t *)bitmapAddr, bufferSize, &pixelMap);
    if (res != IMAGE_RESULT_SUCCESS || pixelMap == nullptr) {
        DRAWING_LOGE(" failed to OH_PixelMap_CreatePixelMap width = %{public}u, height = %{public}u", bitmapWidth_, bitmapHeight_);
        return;
    }

    // 使用napi_value 承接创建的编码器对象
    napi_value packer;
    // 通过 napi_env 创建编码器，返回result为 IMAGE_RESULT_SUCCESS则创建成功
    int32_t result = OH_ImagePacker_Create(env, &packer);
    if (result != IMAGE_RESULT_SUCCESS) {
        DRAWING_LOGE("failed to OH_ImagePacker_Create");
        return;
    }
    // 通过 napi_env 及上述创建的编码器对象初始化原生实例对象
    ImagePacker_Native* nativePacker = OH_ImagePacker_InitNative(env, packer);
    if (nativePacker == nullptr) {
        DRAWING_LOGE("failed to OH_ImagePacker_InitNative");
        return;
    }
    // 编码参数
    ImagePacker_Opts opts;
    // 配置编码格式（必须）
    opts.format = "image/png";
    // 配置编码质量（必须）
    opts.quality = 100;
    // 打开需要输出的文件（请确保应用有权限访问这个路径）
    std::string path = "/data/storage/el2/base/files/" + fileName_ + ".png";
    int fd = open(path.c_str(), O_RDWR | O_CREAT);  
    if (fd <= 0) {
        DRAWING_LOGE("failed to open fd = %{public}d", fd);
        return;
    }

    // 开始对输入source进行编码过程，返回result为 IMAGE_RESULT_SUCCESS则编码成功
    result = OH_ImagePacker_PackToFile(nativePacker, pixelMap, &opts, fd);
    if (result != IMAGE_RESULT_SUCCESS) {
        DRAWING_LOGE("failed to OH_ImagePacker_PackToFile");
        close(fd);
        return;
    }

    // 调用OH_ImagePacker_Release, 销毁编码器
    int32_t ret = OH_ImagePacker_Release(nativePacker);
    // 关闭输出文件
    close(fd);
    DRAWING_LOGE("end");
}

void TestBase::StyleSettings(OH_Drawing_Canvas* canvas, int32_t type)
{
    if (type == DRAW_STYLE_1) {
        OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
        OH_Drawing_Pen *pen = OH_Drawing_PenCreate();

        OH_Drawing_BrushSetAntiAlias(brush, true);
        OH_Drawing_BrushSetColor(brush, 0xFFFF0000);
        OH_Drawing_BrushSetAlpha(brush, 0xF0);
        OH_Drawing_BrushSetBlendMode(brush, BLEND_MODE_SRC);

        OH_Drawing_PenSetAntiAlias(pen, true);
        OH_Drawing_PenSetColor(pen, 0xFFFF0000);
        OH_Drawing_PenSetAlpha(pen, 0xF0);
        OH_Drawing_PenSetBlendMode(pen, BLEND_MODE_SRC);

        OH_Drawing_MaskFilter* mask = OH_Drawing_MaskFilterCreateBlur(NORMAL, 10.0, true);
        OH_Drawing_Filter* filter = OH_Drawing_FilterCreate();
        OH_Drawing_FilterSetMaskFilter(filter, mask);
        OH_Drawing_BrushSetFilter(brush, filter);
        OH_Drawing_PenSetFilter(pen, filter);
        OH_Drawing_MaskFilterDestroy(mask);
        OH_Drawing_FilterDestroy(filter);


        OH_Drawing_Point *center = OH_Drawing_PointCreate(100, 100);
        uint32_t colors[] = {0xFFFF0000, 0xFF00FF00, 0xFF0000FF};
        float pos[] = {0,0.5,1.0};
        OH_Drawing_ShaderEffect* effect = OH_Drawing_ShaderEffectCreateRadialGradient(center, 100, colors, pos, 3, OH_Drawing_TileMode::CLAMP);
        OH_Drawing_BrushSetShaderEffect(brush, effect);
        OH_Drawing_PenSetShaderEffect(pen, effect);
        OH_Drawing_ShaderEffectDestroy(effect);
        OH_Drawing_PointDestroy(center);


        OH_Drawing_PenSetMiterLimit(pen, 10.0);
        OH_Drawing_PenSetJoin(pen, LINE_ROUND_JOIN);
        OH_Drawing_PenSetCap(pen, LINE_ROUND_CAP);
        float vals[2] = {1, 1};
        OH_Drawing_PathEffect *pathEffect = OH_Drawing_CreateDashPathEffect(vals, 2, 0);
        OH_Drawing_PenSetPathEffect(pen, pathEffect);
        OH_Drawing_PathEffectDestroy(pathEffect);

        OH_Drawing_CanvasAttachPen(canvas, pen);
        OH_Drawing_CanvasAttachBrush(canvas, brush);
        OH_Drawing_BrushDestroy(brush);
        OH_Drawing_PenDestroy(pen);
    }
}