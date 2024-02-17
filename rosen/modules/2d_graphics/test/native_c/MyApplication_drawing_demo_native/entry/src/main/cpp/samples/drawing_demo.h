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
    static napi_value NapiExcuteTest(napi_env env, napi_callback_info info);
    static napi_value NapiExcuteAllTest(napi_env env, napi_callback_info info);
    

    void Draw(napi_env env, void* data);
    void DrawAll(napi_env env);

    static void Release(std::string &id);
    void SetWidth(uint64_t width);
    void SetHeight(uint64_t height);
    void SetNativeWindow(OHNativeWindow *nativeWindow);
    void Prepare();
    void Create();
    void DisPlay();
    void Export(napi_env env, napi_value exports);
    void RegisterCallback(OH_NativeXComponent *nativeXComponent);
    void Destroy();
    static DrawingDemo *GetInstance(std::string &id);
    std::string id_;

private:
    void CreateBitmapCanvas();
    void BitmapCanvasToFile(napi_env env);
    void DiasplayToScreen();

    OH_NativeXComponent_Callback renderCallback_;

    uint64_t width_ = 0;
    uint64_t height_ = 0;

    OH_Drawing_Bitmap *cBitmap_ = nullptr;
    OH_Drawing_Canvas *cCanvas_ = nullptr;

    OHNativeWindow *nativeWindow_ = nullptr;
    uint32_t *mappedAddr_ = nullptr;
    BufferHandle *bufferHandle_ = nullptr;
    struct NativeWindowBuffer *buffer_ = nullptr;
    int fenceFd_ = 0;

    //pixmap to file
    OH_Drawing_Bitmap* bitmap = nullptr;
    OH_Drawing_Canvas* canvas = nullptr;

    uint32_t width = DEFAULT_WIDTH;
    uint32_t height = DEFAULT_HEIGHT;
    uint32_t backgroundA_ = DEFAULT_BACKGROUND_COLOR;
    uint32_t backgroundR_ = DEFAULT_BACKGROUND_COLOR;
    uint32_t backgroundG_ = DEFAULT_BACKGROUND_COLOR;
    uint32_t backgroundB_ = DEFAULT_BACKGROUND_COLOR;
    std::string fileName_ = "default";
};


#endif
