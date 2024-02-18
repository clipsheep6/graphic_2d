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
#include <native_drawing/drawing_font_collection.h>
#include <native_drawing/drawing_text_typography.h>
#include "common/log_common.h"
#include "drawing_demo.h"

#include <multimedia/image_framework/image_pixel_map_mdk.h>
#include <multimedia/image_framework/image_packer_mdk.h>

#include <fcntl.h>
#include <sstream>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <memory>
#include <unordered_map>

#include "draw_rect_test.h"
#include "draw_path_test.h"

std::string DRAWRECT = "drawrect";
std::string DRAWPATH = "drawpath";

namespace {
std::unordered_map<std::string, std::function<std::shared_ptr<TestBase>()>> TestCaseMap = {
    {DRAWRECT, []() -> std::shared_ptr<TestBase> {return std::make_shared<DrawRectTest>();}},
    {DRAWPATH, []() -> std::shared_ptr<TestBase> {return std::make_shared<DrawPathTest>();}},
};
} // namespace

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
    auto render = DrawingDemo::GetInstance(id);
    OHNativeWindow *nativeWindow = static_cast<OHNativeWindow *>(window);
    render->SetNativeWindow(nativeWindow);

    uint64_t width;
    uint64_t height;
    int32_t xSize = OH_NativeXComponent_GetXComponentSize(component, window, &width, &height);
    if ((xSize == OH_NATIVEXCOMPONENT_RESULT_SUCCESS) && (render != nullptr)) {
        render->SetHeight(height);
        render->SetWidth(width);
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
    DrawingDemo::Release(id);
}

static std::unordered_map<std::string, DrawingDemo *> g_instance;

void DrawingDemo::SetWidth(uint64_t width)
{
    width_ = width;
}

void DrawingDemo::SetHeight(uint64_t height)
{
    height_ = height;
}

void DrawingDemo::SetNativeWindow(OHNativeWindow *nativeWindow)
{
    nativeWindow_ = nativeWindow;
}

void DrawingDemo::Prepare()
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

void DrawingDemo::DisPlay()
{
    // 画完后获取像素地址，地址指向的内存包含画布画的像素数据
    void *bitmapAddr = OH_Drawing_BitmapGetPixels(cBitmap_);
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
    for (uint32_t x = 0; x < width_; x++) {
        for (uint32_t y = 0; y < height_; y++) {
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

void DrawingDemo::Create()
{
    uint32_t width = static_cast<uint32_t>(bufferHandle_->stride / 4);
    // 创建一个bitmap对象
    cBitmap_ = OH_Drawing_BitmapCreate();
        
    // 定义bitmap的像素格式
    OH_Drawing_BitmapFormat cFormat {COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_OPAQUE};
    // 构造对应格式的bitmap
    OH_Drawing_BitmapBuild(cBitmap_, width, height_, &cFormat);

    // 创建一个canvas对象
    cCanvas_ = OH_Drawing_CanvasCreate();
    // 将画布与bitmap绑定，画布画的内容会输出到绑定的bitmap内存中
    OH_Drawing_CanvasBind(cCanvas_, cBitmap_);
    // 使用白色清除画布内容
    OH_Drawing_CanvasClear(cCanvas_, OH_Drawing_ColorSetArgb(0x00, 0x00, 0x00, 0x00));
}

void DrawingDemo::CreateBitmapCanvas()
{
    // 创建一个bitmap对象
    if (bitmap) {
        OH_Drawing_BitmapDestroy(bitmap);
    }
    bitmap = OH_Drawing_BitmapCreate();
    // 定义bitmap的像素格式
    OH_Drawing_BitmapFormat cFormat{COLOR_FORMAT_BGRA_8888, ALPHA_FORMAT_OPAQUE};
    // 构造对应格式的bitmap
    OH_Drawing_BitmapBuild(bitmap, width, height, &cFormat);

    // 创建一个canvas对象
    if (canvas) {
        OH_Drawing_CanvasDestroy(canvas);
    }
    canvas = OH_Drawing_CanvasCreate();
    // 将画布与bitmap绑定，画布画的内容会输出到绑定的bitmap内存中
    OH_Drawing_CanvasBind(canvas, bitmap);
    // 使用白色清除画布内容
    OH_Drawing_CanvasClear(canvas, OH_Drawing_ColorSetArgb(backgroundA_, backgroundR_, backgroundG_, backgroundB_));
}

void DrawingDemo::BitmapCanvasToFile(napi_env env)
{
    //创建pixmap
    napi_value pixelMap = nullptr;
    struct OhosPixelMapCreateOps createOps;
    createOps.width = width;
    createOps.height = height;
    createOps.pixelFormat = 3;
    createOps.alphaType = 0;
    createOps.editable = 1;
    size_t bufferSize = createOps.width * createOps.height * 4;
    void *bitmapAddr = OH_Drawing_BitmapGetPixels(bitmap);
    int32_t res = OH_PixelMap_CreatePixelMap(env, createOps, (uint8_t *)bitmapAddr, bufferSize, &pixelMap);
    if (res != IMAGE_RESULT_SUCCESS || pixelMap == nullptr) {
        DRAWING_LOGE("failed to OH_PixelMap_CreatePixelMap width = %{public}u, height = %{public}u", width, height);
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
    opts.format = "image/jpeg";
    // 配置编码质量（必须）
    opts.quality = 100;
    // 打开需要输出的文件（请确保应用有权限访问这个路径）
    std::string path = "/data/storage/el2/base/files/" + fileName_ + ".jpg";
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
}

void DrawingDemo::DiasplayToScreen()
{
    if (cCanvas_ == nullptr || bitmap == nullptr) {
        return;
    }
    OH_Drawing_CanvasDrawBitmap(cCanvas_, bitmap, 100, 100); // offset 100*100
}

void DrawingDemo::Draw(napi_env env, std::string caseName)
{
//    std::string castName = static_cast<char *>(data);
    if (TestCaseMap.find(caseName) == TestCaseMap.end()) {
        DRAWING_LOGE("castName error");
        return;
    }
    
    std::shared_ptr<TestBase> testCase = TestCaseMap.find(caseName)->second();
    testCase->GetSize(width, height);
    uint32_t color = 0;
    testCase->GetBackgroundColor(color);
    backgroundA_ = ((color >> 24) & 0xff);
    backgroundR_ = ((color >> 16) & 0xff);
    backgroundG_ = ((color >> 8) & 0xff);
    backgroundB_ = (color & 0xff);
    fileName_ = caseName;
    
    CreateBitmapCanvas();
    testCase->Recording(canvas);
    
    BitmapCanvasToFile(env);
    DiasplayToScreen();
}


void DrawingDemo::DrawAll(napi_env env)
{
    for (auto map : TestCaseMap) {
        std::string castName = map.first;
        
        std::shared_ptr<TestBase> testCase = map.second();
        testCase->GetSize(width, height);
        uint32_t color = 0;
        testCase->GetBackgroundColor(color);
        backgroundA_ = ((color >> 24) & 0xff);
        backgroundR_ = ((color >> 16) & 0xff);
        backgroundG_ = ((color >> 8) & 0xff);
        backgroundB_ = (color & 0xff);
        fileName_ = castName;
        
        CreateBitmapCanvas();
        testCase->Recording(canvas);
        BitmapCanvasToFile(env);
    }
}

napi_value DrawingDemo::NapiExcuteAllTest(napi_env env, napi_callback_info info)
{
    DRAWING_LOGI("NapiDrawText");
    if ((env == nullptr) || (info == nullptr)) {
        DRAWING_LOGE("NapiDrawPattern: env or info is null");
        return nullptr;
    }

    napi_value thisArg;
    if (napi_get_cb_info(env, info, nullptr, nullptr, &thisArg, nullptr) != napi_ok) {
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
    DrawingDemo *render = DrawingDemo().GetInstance(id);
    if (render != nullptr) {
        render->DrawAll(env);
    } else {
        DRAWING_LOGE("render is nullptr");
    }
    return nullptr;
}

napi_value DrawingDemo::NapiExcuteTest(napi_env env, napi_callback_info info)
{
    DRAWING_LOGI("NapiDrawText");
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
    DrawingDemo *render = DrawingDemo().GetInstance(id);
    if (render != nullptr) {
        render->Prepare();
        render->Create();
        render->Draw(env, caseName);
        render->DisPlay();
        render->Destroy();
    } else {
        DRAWING_LOGE("render is nullptr");
    }
    return nullptr;
}

DrawingDemo::~DrawingDemo()
{
    // 销毁创建的对象
    // 销毁canvas对象
    OH_Drawing_CanvasDestroy(cCanvas_);
    cCanvas_ = nullptr;
    // 销毁bitmap对象
    OH_Drawing_BitmapDestroy(cBitmap_);
    cBitmap_ = nullptr;

    buffer_ = nullptr;
    bufferHandle_ = nullptr;
    nativeWindow_ = nullptr;
    mappedAddr_ = nullptr;
}

void DrawingDemo::Destroy()
{
    // 销毁创建的对象
    // 销毁canvas对象
    OH_Drawing_CanvasDestroy(cCanvas_);
    cCanvas_ = nullptr;
    // 销毁bitmap对象
    OH_Drawing_BitmapDestroy(cBitmap_);
}

void DrawingDemo::Release(std::string &id)
{
    DrawingDemo *render = DrawingDemo::GetInstance(id);
    if (render != nullptr) {
        delete render;
        render = nullptr;
        g_instance.erase(g_instance.find(id));
    }
}

void DrawingDemo::Export(napi_env env, napi_value exports)
{
    if ((env == nullptr) || (exports == nullptr)) {
        DRAWING_LOGE("Export: env or exports is null");
        return;
    }

    napi_property_descriptor desc[] = {
        {"drawAll", nullptr, DrawingDemo::NapiExcuteAllTest, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"drawCase", nullptr, DrawingDemo::NapiExcuteTest, nullptr, nullptr, nullptr, napi_default, nullptr},
    };
    if (napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc) != napi_ok) {
        DRAWING_LOGE("Export: napi_define_properties failed");
    }
}

void DrawingDemo::RegisterCallback(OH_NativeXComponent *nativeXComponent)
{
    DRAWING_LOGI("register callback");
    renderCallback_.OnSurfaceCreated = OnSurfaceCreatedCB;
    renderCallback_.OnSurfaceDestroyed = OnSurfaceDestroyedCB;
    // Callback must be initialized
    renderCallback_.DispatchTouchEvent = nullptr;
    renderCallback_.OnSurfaceChanged = nullptr;
    OH_NativeXComponent_RegisterCallback(nativeXComponent, &renderCallback_);
}

DrawingDemo *DrawingDemo::GetInstance(std::string &id)
{
    if (g_instance.find(id) == g_instance.end()) {
        DrawingDemo *render = new DrawingDemo(id);
        g_instance[id] = render;
        return render;
    } else {
        return g_instance[id];
    }
}
