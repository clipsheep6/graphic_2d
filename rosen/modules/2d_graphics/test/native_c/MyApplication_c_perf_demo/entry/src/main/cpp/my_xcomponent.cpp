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

#include <bits/alltypes.h>
#include <chrono>
#include <native_drawing/drawing_font_collection.h>
#include <native_drawing/drawing_text_typography.h>
#include "common/log_common.h"
#include "my_xcomponent.h"

#include <sstream>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <memory>
#include <unordered_map>

#include "testcasefactory.h"

static std::unordered_map<std::string, MyXComponent *> g_instance;

bool ConvertStringFromJsValue(napi_env env, napi_value jsValue, std::string &value)
{
    size_t len = 0;
    if (napi_get_value_string_utf8(env, jsValue, nullptr, 0, &len) != napi_ok) {
        return false;
    }
    auto buffer = std::make_unique<char[]>(len + 1);
    size_t strLength = 0;
    if (napi_get_value_string_utf8(env, jsValue, buffer.get(), len + 1, &strLength) == napi_ok) {
        value = buffer.get();
        return true;
    }
    return false;
}

bool ConvertStringFromIntValue(napi_env env, napi_value jsValue, uint32_t &value) {

    return napi_get_value_uint32(env, jsValue, &value) == napi_ok;
}

static void OnSurfaceCreatedCB(OH_NativeXComponent *component, void *window)
{
    DRAWING_LOGI("OnSurfaceCreatedCB");
    if ((component == nullptr) || (window == nullptr)) {
        DRAWING_LOGE("OnSurfaceCreatedCB: component or window is null");
        return;
    }
    char idStr[OH_XCOMPONENT_ID_LEN_MAX + 1] = {'\0'};
    uint64_t idSize = OH_XCOMPONENT_ID_LEN_MAX + 1;
    if (OH_NativeXComponent_GetXComponentId(component, idStr, &idSize) != OH_NATIVEXCOMPONENT_RESULT_SUCCESS) {
        DRAWING_LOGE("OnSurfaceCreatedCB: Unable to get XComponent id");
        return;
    }
    std::string id(idStr);
    auto render = MyXComponent::GetInstance(id);
    OHNativeWindow *nativeWindow = static_cast<OHNativeWindow *>(window);
    render->SetNativeWindow(nativeWindow);

    uint64_t width;
    uint64_t height;
    int32_t xSize = OH_NativeXComponent_GetXComponentSize(component, window, &width, &height);
    if ((xSize == OH_NATIVEXCOMPONENT_RESULT_SUCCESS) && (render != nullptr)) {
        render->SetSceenHeight(height);
        render->SetScreenWidth(width);
//        DRAWING_LOGI("xComponent width = %{public}lu, height = %{public}lu", width, height);
    }
}

static void OnSurfaceDestroyedCB(OH_NativeXComponent *component, void *window)
{
    DRAWING_LOGI("OnSurfaceDestroyedCB");
    if ((component == nullptr) || (window == nullptr)) {
        DRAWING_LOGE("OnSurfaceDestroyedCB: component or window is null");
        return;
    }
    char idStr[OH_XCOMPONENT_ID_LEN_MAX + 1] = {'\0'};
    uint64_t idSize = OH_XCOMPONENT_ID_LEN_MAX + 1;
    if (OH_NativeXComponent_GetXComponentId(component, idStr, &idSize) != OH_NATIVEXCOMPONENT_RESULT_SUCCESS) {
        DRAWING_LOGE("OnSurfaceDestroyedCB: Unable to get XComponent id");
        return;
    }
    std::string id(idStr);
    MyXComponent::Release(id);
}

// 通过callback获取到宽高信息
void MyXComponent::SetScreenWidth(uint64_t width)
{
    screenWidth_ = width;
}

// 通过callback获取到宽高信息
void MyXComponent::SetSceenHeight(uint64_t height)
{
    screenHeight_ = height;
}

// 通过callback获取到window
void MyXComponent::SetNativeWindow(OHNativeWindow *nativeWindow)
{
    nativeWindow_ = nativeWindow;
}

//获取屏幕xcomponent的显示buffer地址
void MyXComponent::GetScreenBuffer()
{
    if (nativeWindow_ == nullptr) {
        DRAWING_LOGE("nativeWindow_ is nullptr");
        return;
    }
    // 这里的nativeWindow是从上一步骤中的回调函数中获得的
    // 通过 OH_NativeWindow_NativeWindowRequestBuffer 获取 OHNativeWindowBuffer 实例
    int ret = OH_NativeWindow_NativeWindowRequestBuffer(nativeWindow_, &buffer_, &fenceFd_);
    DRAWING_LOGI("request buffer ret = %{public}d", ret);
    // 通过 OH_NativeWindow_GetBufferHandleFromNative 获取 buffer 的 handle
    bufferHandle_ = OH_NativeWindow_GetBufferHandleFromNative(buffer_);
    // 使用系统mmap接口拿到bufferHandle的内存虚拟地址
    mappedAddr_ = static_cast<uint32_t *>(
        mmap(bufferHandle_->virAddr, bufferHandle_->size, PROT_READ | PROT_WRITE, MAP_SHARED, bufferHandle_->fd, 0));
    if (mappedAddr_ == MAP_FAILED) {
        DRAWING_LOGE("mmap failed");
    }
}

//显示到屏幕上
void MyXComponent::DisplayScreenCanvas()
{
    // 画完后获取像素地址，地址指向的内存包含画布画的像素数据
    void *bitmapAddr = OH_Drawing_BitmapGetPixels(screenBitmap_);
    uint32_t *value = static_cast<uint32_t *>(bitmapAddr);

    uint32_t *pixel = static_cast<uint32_t *>(mappedAddr_); // 使用mmap获取到的地址来访问内存
    if (pixel == nullptr) {
        DRAWING_LOGE("pixel is null");
        return;
    }
    if (value == nullptr) {
        DRAWING_LOGE("value is null");
        return;
    }
    for (uint32_t x = 0; x < screenWidth_; x++) {
        for (uint32_t y = 0; y < screenHeight_; y++) {
            *pixel++ = *value++;
        }
    }
    // 设置刷新区域，如果Region中的Rect为nullptr,或者rectNumber为0，则认为OHNativeWindowBuffer全部有内容更改。
    Region region {nullptr, 0};
    // 通过OH_NativeWindow_NativeWindowFlushBuffer 提交给消费者使用，例如：显示在屏幕上。
    OH_NativeWindow_NativeWindowFlushBuffer(nativeWindow_, buffer_, fenceFd_, region);
    // 内存使用完记得去掉内存映射
    int result = munmap(mappedAddr_, bufferHandle_->size);
    if (result == -1) {
        DRAWING_LOGE("munmap failed!");
    }
}

void MyXComponent::CreateScreenCanvas()
{
    uint32_t width = static_cast<uint32_t>(bufferHandle_->stride / 4);
    // 创建一个bitmap对象
    screenBitmap_ = OH_Drawing_BitmapCreate();
        
    // 定义bitmap的像素格式
    OH_Drawing_BitmapFormat cFormat {COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_OPAQUE};
    // 构造对应格式的bitmap
    OH_Drawing_BitmapBuild(screenBitmap_, width, screenHeight_, &cFormat);

    // 创建一个canvas对象
    screenCanvas_ = OH_Drawing_CanvasCreate();
    // 将画布与bitmap绑定，画布画的内容会输出到绑定的bitmap内存中
    OH_Drawing_CanvasBind(screenCanvas_, screenBitmap_);
    // 使用白色清除画布内容
    OH_Drawing_CanvasClear(screenCanvas_, OH_Drawing_ColorSetArgb(0x00, 0x00, 0x00, 0x00));
}

void MyXComponent::BitmapToScreenCanvas(OH_Drawing_Bitmap* bitmap)
{
    DRAWING_LOGE("MyXComponent  BitmapToScreenCanvas");
    if (screenCanvas_ == nullptr || bitmap == nullptr) {
        return;
    }
    OH_Drawing_CanvasDrawBitmap(screenCanvas_, bitmap, 0, 0);
}

void MyXComponent::TestPerformanceCpu(napi_env env, std::string caseName)
{
    DRAWING_LOGE("MyXComponent TestPerformanceCpu start");
    auto testCase = TestCaseFactory::GetPerformanceCpuCase(caseName);
    if (testCase == nullptr) {
        DRAWING_LOGE("failed to get testcase");
        return;
    }
    testCase->SetTestCount(testCount_);
    testCase->TestPerformanceCpu(env);
    usedTime_ = testCase->GetTime();
    BitmapToScreenCanvas(testCase->GetBitmap());
    DRAWING_LOGE("MyXComponent TestPerformanceCpu end");
}

void MyXComponent::TestFunctionCpu(napi_env env, std::string caseName)
{
    if (caseName == "All") {
        for (auto map : TestCaseFactory::GetFunctionCpuCaseAll()) {
            std::shared_ptr<TestBase> testCase = map.second();
            if (testCase == nullptr) {
                DRAWING_LOGE("TestCase is null");
                return;
            }
            testCase->SetFileName(map.first);
            testCase->TestFunctionCpu(env);
        }
    } else {
        auto testCase = TestCaseFactory::GetFunctionCpuCase(caseName);
        if (testCase == nullptr) {
            DRAWING_LOGE("failed to get testcase");
            return;
        }
        testCase->SetFileName(caseName);
        testCase->TestFunctionCpu(env);
        BitmapToScreenCanvas(testCase->GetBitmap());
    }
}

void MyXComponent::SetTestCount(uint32_t testCount)
{ 
    testCount_ = testCount;
}

napi_value MyXComponent::NapiSetTestCount(napi_env env, napi_callback_info info)
{
    DRAWING_LOGI("NapiSetTestCount");
    if ((env == nullptr) || (info == nullptr)) {
        DRAWING_LOGE("NapiDrawPattern: env or info is null");
        return nullptr;
    }

    napi_value thisArg;
    size_t argc = 1;
    napi_value argv[1] = {nullptr};
    if (napi_get_cb_info(env, info, &argc, argv, &thisArg, nullptr) != napi_ok) {
        DRAWING_LOGE("NapiDrawPattern: napi_get_cb_info fail");
        return nullptr;
    }
    uint32_t testCount;
    if (!ConvertStringFromIntValue(env, argv[0], testCount)) {
        DRAWING_LOGE("NapiDrawPattern: get caseName fail");
        return nullptr;
    }
    DRAWING_LOGI("testCount = %{public}u", testCount);

    napi_value exportInstance;
    if (napi_get_named_property(env, thisArg, OH_NATIVE_XCOMPONENT_OBJ, &exportInstance) != napi_ok) {
        DRAWING_LOGE("NapiDrawPattern: napi_get_named_property fail");
        return nullptr;
    }

    OH_NativeXComponent *nativeXComponent = nullptr;
    if (napi_unwrap(env, exportInstance, reinterpret_cast<void **>(&nativeXComponent)) != napi_ok) {
        DRAWING_LOGE("NapiDrawPattern: napi_unwrap fail");
        return nullptr;
    }

    char idStr[OH_XCOMPONENT_ID_LEN_MAX + 1] = {'\0'};
    uint64_t idSize = OH_XCOMPONENT_ID_LEN_MAX + 1;
    if (OH_NativeXComponent_GetXComponentId(nativeXComponent, idStr, &idSize) != OH_NATIVEXCOMPONENT_RESULT_SUCCESS) {
        DRAWING_LOGE("NapiDrawPattern: Unable to get XComponent id");
        return nullptr;
    }
    DRAWING_LOGI("ID = %{public}s", idStr);
    std::string id(idStr);
    MyXComponent *render = MyXComponent().GetInstance(id);
    if (render != nullptr) {
        render->SetTestCount(testCount);
    } else {
        DRAWING_LOGE("render is nullptr");
    }
    return nullptr;
}

uint32_t MyXComponent::GetTime()
{ 
    return usedTime_;
}

napi_value MyXComponent::NapiGetTime(napi_env env, napi_callback_info info)
{
    DRAWING_LOGI("MyXComponent NapiGetTime");
    if ((env == nullptr) || (info == nullptr)) {
        DRAWING_LOGE("NapiDrawPattern: env or info is null");
        return nullptr;
    }

    napi_value thisArg;
    size_t argc = 1;
    napi_value argv[1] = {nullptr};
    if (napi_get_cb_info(env, info, &argc, argv, &thisArg, nullptr) != napi_ok) {
        DRAWING_LOGE("NapiDrawPattern: napi_get_cb_info fail");
        return nullptr;
    }

    napi_value exportInstance;
    if (napi_get_named_property(env, thisArg, OH_NATIVE_XCOMPONENT_OBJ, &exportInstance) != napi_ok) {
        DRAWING_LOGE("NapiDrawPattern: napi_get_named_property fail");
        return nullptr;
    }

    OH_NativeXComponent *nativeXComponent = nullptr;
    if (napi_unwrap(env, exportInstance, reinterpret_cast<void **>(&nativeXComponent)) != napi_ok) {
        DRAWING_LOGE("NapiDrawPattern: napi_unwrap fail");
        return nullptr;
    }

    char idStr[OH_XCOMPONENT_ID_LEN_MAX + 1] = {'\0'};
    uint64_t idSize = OH_XCOMPONENT_ID_LEN_MAX + 1;
    if (OH_NativeXComponent_GetXComponentId(nativeXComponent, idStr, &idSize) != OH_NATIVEXCOMPONENT_RESULT_SUCCESS) {
        DRAWING_LOGE("NapiDrawPattern: Unable to get XComponent id");
        return nullptr;
    }
    DRAWING_LOGI("ID = %{public}s", idStr);
    std::string id(idStr);
    MyXComponent *render = MyXComponent().GetInstance(id);
    if (render != nullptr) {
        DRAWING_LOGE("DrawingTest render->GetTime");
        napi_value value = nullptr;
        (void)napi_create_int32(env, render->GetTime(), &value);
        return value;
    } else {
        DRAWING_LOGE("DrawingTest render is nullptr");
    }
    return nullptr;
}

napi_value MyXComponent::NapiFunctionCpu(napi_env env, napi_callback_info info) {
    DRAWING_LOGI("NapiFunctionCpu");
    if ((env == nullptr) || (info == nullptr)) {
        DRAWING_LOGE("NapiFunctionCpu: env or info is null");
        return nullptr;
    }

    napi_value thisArg;
    size_t argc = 1;
    napi_value argv[1] = {nullptr};
    if (napi_get_cb_info(env, info, &argc, argv, &thisArg, nullptr) != napi_ok) {
        DRAWING_LOGE("NapiFunctionCpu: napi_get_cb_info fail");
        return nullptr;
    }
    std::string caseName = "";
    if (!ConvertStringFromJsValue(env, argv[0], caseName)) {
        DRAWING_LOGE("NapiFunctionCpu: get caseName fail");
        return nullptr;
    }
    DRAWING_LOGI("caseName = %{public}s", caseName.c_str());

    napi_value exportInstance;
    if (napi_get_named_property(env, thisArg, OH_NATIVE_XCOMPONENT_OBJ, &exportInstance) != napi_ok) {
        DRAWING_LOGE("NapiFunctionCpu: napi_get_named_property fail");
        return nullptr;
    }

    OH_NativeXComponent *nativeXComponent = nullptr;
    if (napi_unwrap(env, exportInstance, reinterpret_cast<void **>(&nativeXComponent)) != napi_ok) {
        DRAWING_LOGE("NapiFunctionCpu: napi_unwrap fail");
        return nullptr;
    }

    char idStr[OH_XCOMPONENT_ID_LEN_MAX + 1] = {'\0'};
    uint64_t idSize = OH_XCOMPONENT_ID_LEN_MAX + 1;
    if (OH_NativeXComponent_GetXComponentId(nativeXComponent, idStr, &idSize) != OH_NATIVEXCOMPONENT_RESULT_SUCCESS) {
        DRAWING_LOGE("NapiFunctionCpu: Unable to get XComponent id");
        return nullptr;
    }
    DRAWING_LOGI("NapiFunctionCpu ID = %{public}s", idStr);
    std::string id(idStr);
    MyXComponent *render = MyXComponent().GetInstance(id);
    if (render != nullptr) {
        render->GetScreenBuffer();
        render->CreateScreenCanvas();
        render->TestFunctionCpu(env, caseName);
        render->DisplayScreenCanvas();
        render->Destroy();
    } else {
        DRAWING_LOGE("NapiFunctionCpu: render is nullptr");
    }
    return nullptr;
}

napi_value MyXComponent::NapiPerformanceCpu(napi_env env, napi_callback_info info) {
    DRAWING_LOGI("NapiPerformanceCpu");
    if ((env == nullptr) || (info == nullptr)) {
        DRAWING_LOGE("NapiDrawPattern: env or info is null");
        return nullptr;
    }

    napi_value thisArg;
    size_t argc = 1;
    napi_value argv[1] = {nullptr};
    if (napi_get_cb_info(env, info, &argc, argv, &thisArg, nullptr) != napi_ok) {
        DRAWING_LOGE("NapiDrawPattern: napi_get_cb_info fail");
        return nullptr;
    }
    std::string caseName = "";
    if (!ConvertStringFromJsValue(env, argv[0], caseName)) {
        DRAWING_LOGE("NapiDrawPattern: get caseName fail");
        return nullptr;
    }
    DRAWING_LOGI("caseName = %{public}s", caseName.c_str());

    napi_value exportInstance;
    if (napi_get_named_property(env, thisArg, OH_NATIVE_XCOMPONENT_OBJ, &exportInstance) != napi_ok) {
        DRAWING_LOGE("NapiDrawPattern: napi_get_named_property fail");
        return nullptr;
    }

    OH_NativeXComponent *nativeXComponent = nullptr;
    if (napi_unwrap(env, exportInstance, reinterpret_cast<void **>(&nativeXComponent)) != napi_ok) {
        DRAWING_LOGE("NapiDrawPattern: napi_unwrap fail");
        return nullptr;
    }

    char idStr[OH_XCOMPONENT_ID_LEN_MAX + 1] = {'\0'};
    uint64_t idSize = OH_XCOMPONENT_ID_LEN_MAX + 1;
    if (OH_NativeXComponent_GetXComponentId(nativeXComponent, idStr, &idSize) != OH_NATIVEXCOMPONENT_RESULT_SUCCESS) {
        DRAWING_LOGE("NapiDrawPattern: Unable to get XComponent id");
        return nullptr;
    }
    DRAWING_LOGI("ID = %{public}s", idStr);
    std::string id(idStr);
    MyXComponent *render = MyXComponent().GetInstance(id);
    if (render != nullptr) {
        render->GetScreenBuffer();
        render->CreateScreenCanvas();
        render->TestPerformanceCpu(env, caseName);
        render->DisplayScreenCanvas();
        render->Destroy();
    } else {
        DRAWING_LOGE("render is nullptr");
    }
    DRAWING_LOGE("DrawingTest NapiPerformanceCpu end");
    
    return nullptr;
}

MyXComponent::~MyXComponent()
{
    // 销毁创建的对象
    // 销毁canvas对象
    if (screenCanvas_) {
        OH_Drawing_CanvasDestroy(screenCanvas_);
        screenCanvas_ = nullptr;
    }
    // 销毁bitmap对象
    if (screenBitmap_) {
        OH_Drawing_BitmapDestroy(screenBitmap_);
        screenBitmap_ = nullptr;
    }

    buffer_ = nullptr;
    bufferHandle_ = nullptr;
    nativeWindow_ = nullptr;
    mappedAddr_ = nullptr;
}

void MyXComponent::Destroy()
{
    // 销毁创建的对象
    // 销毁canvas对象
    if (screenCanvas_) {
        OH_Drawing_CanvasDestroy(screenCanvas_);
        screenCanvas_ = nullptr;
    }
    // 销毁bitmap对象
    if (screenBitmap_) {
        OH_Drawing_BitmapDestroy(screenBitmap_);
        screenBitmap_ = nullptr;
    }
}

void MyXComponent::Release(std::string &id)
{
    MyXComponent *render = MyXComponent::GetInstance(id);
    if (render != nullptr) {
        delete render;
        render = nullptr;
        g_instance.erase(g_instance.find(id));
    }
}

void MyXComponent::Export(napi_env env, napi_value exports)
{
    if ((env == nullptr) || (exports == nullptr)) {
        DRAWING_LOGE("Export: env or exports is null");
        return;
    }

    napi_property_descriptor desc[] = {
        {"setTestCount", nullptr, MyXComponent::NapiSetTestCount, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"getTime", nullptr, MyXComponent::NapiGetTime, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"testFunctionCpu", nullptr, MyXComponent::NapiFunctionCpu, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"testPerformanceCpu", nullptr, MyXComponent::NapiPerformanceCpu, nullptr, nullptr, nullptr, napi_default, nullptr},
    };
    if (napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc) != napi_ok) {
        DRAWING_LOGE("Export: napi_define_properties failed");
    }
}

void MyXComponent::RegisterCallback(OH_NativeXComponent *nativeXComponent)
{
    DRAWING_LOGI("register callback");
    renderCallback_.OnSurfaceCreated = OnSurfaceCreatedCB;
    renderCallback_.OnSurfaceDestroyed = OnSurfaceDestroyedCB;
    // Callback must be initialized
    renderCallback_.DispatchTouchEvent = nullptr;
    renderCallback_.OnSurfaceChanged = nullptr;
    OH_NativeXComponent_RegisterCallback(nativeXComponent, &renderCallback_);
}

MyXComponent *MyXComponent::GetInstance(std::string &id)
{
    if (g_instance.find(id) == g_instance.end()) {
        MyXComponent *render = new MyXComponent(id);
        g_instance[id] = render;
        return render;
    } else {
        return g_instance[id];
    }
}
