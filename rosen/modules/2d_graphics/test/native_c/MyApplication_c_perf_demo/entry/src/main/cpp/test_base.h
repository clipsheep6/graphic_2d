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

#include "napi/native_api.h"
#include <string>
#include <bits/alltypes.h>
#include <native_drawing/drawing_canvas.h>
#include <native_drawing/drawing_bitmap.h>
#include <native_drawing/drawing_color.h>

#ifndef TEST_BASE_H
#define TEST_BASE_H

#define DEFAULT_WIDTH 720
#define DEFAULT_HEIGHT 720
#define DEFAULT_TESTCOUNT 1
#define DEFAULT_BACKGROUND_COLOR 0xFFFFFFFF


class TestBase {
public:
    TestBase(){};
    TestBase(int styleType):styleType_(styleType){};
    virtual ~TestBase() = default;

    // void Recording(OH_Drawing_Canvas* canvas);
    void SetFileName(std::string fileName);
    void SetTestCount(uint32_t testCount);
    OH_Drawing_Bitmap* GetBitmap();
    uint32_t GetTime();

    void TestFunctionCpu(napi_env env);
    void TestFunctionGpu(OH_Drawing_Canvas* canvas);
    void TestPerformanceCpu(napi_env env);
    void TestPerformanceGpu(OH_Drawing_Canvas* canvas);
    // CPU drawing function test, save drawing results to image (filename_)
    virtual void OnTestFunction(OH_Drawing_Canvas* canvas) {};
    // CPU drawing performance test, execute critical interface testCount_ times repeatedly
    virtual void OnTestPerformance(OH_Drawing_Canvas* canvas) {};

    enum {
        DRAW_STYLE_1,
    };
    int styleType_ = 0;
    void StyleSettings(OH_Drawing_Canvas* canvas, int32_t type);
protected:
    // virtual void OnRecording(OH_Drawing_Canvas* canvas) = 0;
    // cpu bitmap canvas
    void CreateBitmapCanvas();
    void BitmapCanvasToFile(napi_env env);

    //pixmap to file
    OH_Drawing_Bitmap* bitmap_ = nullptr;
    OH_Drawing_Canvas* bitmapCanvas_ = nullptr;
    
    // bitmapcanvas cfg
    uint32_t bitmapWidth_ = DEFAULT_WIDTH;
    uint32_t bitmapHeight_ = DEFAULT_HEIGHT;
    uint32_t background_ = DEFAULT_BACKGROUND_COLOR;
    uint32_t backgroundA_ = DEFAULT_BACKGROUND_COLOR;
    uint32_t backgroundR_ = DEFAULT_BACKGROUND_COLOR;
    uint32_t backgroundG_ = DEFAULT_BACKGROUND_COLOR;
    uint32_t backgroundB_ = DEFAULT_BACKGROUND_COLOR;
    std::string fileName_ = "default";
    uint32_t testCount_ = DEFAULT_TESTCOUNT;
    uint32_t usedTime_ = 0;
};

#endif // TEST_BASE_H