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

#ifndef SAMPLE_BITMAP_H
#define SAMPLE_BITMAP_H
#include <ace/xcomponent/native_interface_xcomponent.h>
#include <arpa/nameser.h>
#include <bits/alltypes.h>
#include <native_window/external_window.h>
#include <native_drawing/drawing_bitmap.h>
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_canvas.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_rect.h>
#include <cstdint>
#include <map>
#include <sys/mman.h>
#include <string>
#include "napi/native_api.h"
#include "test_base.h"

class DrawingDemo {
public:
    DrawingDemo() = default;
    ~DrawingDemo();
    explicit DrawingDemo(std::string id) : id_(id) {}
    static DrawingDemo *GetInstance(std::string &id);
    void Destroy();

    void Export(napi_env env, napi_value exports);
    static napi_value NapiSetTestCount(napi_env env, napi_callback_info info);
    static napi_value NapiGetTime(napi_env env, napi_callback_info info);
    static napi_value NapiFunctionCpu(napi_env env, napi_callback_info info);
    static napi_value NapiPerformanceCpu(napi_env env, napi_callback_info info);

    // display to screen
    void RegisterCallback(OH_NativeXComponent *nativeXComponent);
    void SetNativeWindow(OHNativeWindow *nativeWindow);
    void SetScreenWidth(uint64_t width);
    void SetSceenHeight(uint64_t height);
    void GetScreenBuffer();
    void CreateScreenCanvas();
    void DisplayScreenCanvas();

    // for test
    void SetTestCount(uint32_t testCount);
    uint32_t GetTime();
    void TestFunctionCpu(napi_env env, std::string caseName);
    void TestFunctionCpuOne(napi_env env, std::shared_ptr<TestBase> testCase, std::string caseName);
    void TestPerformanceCpu(napi_env env, std::string caseName);
    // void DrawAll(napi_env env);

    static void Release(std::string &id);
    std::string id_;

private:
    // cpu bitmap canvas
    void CreateBitmapCanvas();
    void BitmapCanvasToFile(napi_env env);
    void BitmapCanvasToScreenCanvas();


    // This is the size of the display area, not the size of the entire screen
    OHNativeWindow *nativeWindow_ = nullptr;
    uint64_t screenWidth_ = 0;
    uint64_t screenHeight_ = 0;
    OH_Drawing_Bitmap *screenBitmap_ = nullptr;
    OH_Drawing_Canvas *screenCanvas_ = nullptr;
    OH_NativeXComponent_Callback renderCallback_;
    uint32_t *mappedAddr_ = nullptr;
    BufferHandle *bufferHandle_ = nullptr;
    struct NativeWindowBuffer *buffer_ = nullptr;
    int fenceFd_ = 0;

    //pixmap to file
    OH_Drawing_Bitmap* bitmap_ = nullptr;
    OH_Drawing_Canvas* bitmapCanvas_ = nullptr;

    // bitmapcanvas cfg
    uint32_t bitmapWidth_ = DEFAULT_WIDTH;
    uint32_t bitmapHeight_ = DEFAULT_HEIGHT;
    uint32_t backgroundA_ = DEFAULT_BACKGROUND_COLOR;
    uint32_t backgroundR_ = DEFAULT_BACKGROUND_COLOR;
    uint32_t backgroundG_ = DEFAULT_BACKGROUND_COLOR;
    uint32_t backgroundB_ = DEFAULT_BACKGROUND_COLOR;
    std::string fileName_ = "default";
    uint32_t testCount_ = DEFAULT_TESTCOUNT;
    uint32_t usedTime_ = 0;
};


#endif
