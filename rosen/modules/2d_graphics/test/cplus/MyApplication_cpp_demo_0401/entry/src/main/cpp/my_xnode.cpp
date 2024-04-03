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
#include <dlfcn.h>
#include <native_drawing/drawing_font_collection.h>
#include <native_drawing/drawing_text_typography.h>
#include "common/log_common.h"
#include "my_xnode.h"
#include "napi/native_api.h"
#include "napi/native_api.h"

#include <sstream>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <memory>
#include <unordered_map>


#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_rect.h>

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

napi_value MyXNode::NapiPerformance(napi_env env, napi_callback_info info)
{
    DRAWING_LOGE("MyXNode XNodeDraw::NapiPerformance");

    DRAWING_LOGI("NapiPerformance");
    if ((env == nullptr) || (info == nullptr)) {
        DRAWING_LOGE("NapiPerformance: env or info is null");
        return nullptr;
    }

    size_t argc = 4;
    napi_value argv[4] = {nullptr};
    if (napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr) != napi_ok) {
        DRAWING_LOGE("NapiPerformance: napi_get_cb_info fail");
        return nullptr;
    }
    void* contex;
    napi_unwrap(env, argv[0], &contex);
    OH_Drawing_Canvas* canvas = reinterpret_cast<OH_Drawing_Canvas*>(contex);
    if (canvas == nullptr) {
        DRAWING_LOGE("NapiPerformance: XNodeDraw get canvas fail");
        return nullptr;
    }

    std::string type = "";
    if (!ConvertStringFromJsValue(env, argv[1], type)) {
        DRAWING_LOGE("NapiPerformance: get caseName fail");
        return nullptr;
    }
    DRAWING_LOGI("NapiPerformance: type = %{public}s", type.c_str());
    
    std::string caseName = "";
    if (!ConvertStringFromJsValue(env, argv[2], caseName)) {
        DRAWING_LOGE("NapiPerformance: get caseName fail");
        return nullptr;
    }
    DRAWING_LOGI("NapiPerformance: caseName = %{public}s", caseName.c_str());
    
    uint32_t testCount;
    if (!ConvertStringFromIntValue(env, argv[3], testCount)) {
        DRAWING_LOGE("NapiPerformance: get caseName fail");
        return nullptr;
    }
    DRAWING_LOGI("NapiPerformance: testCount = %{public}u", testCount);
    
    uint32_t time = TestPerformance(canvas, type, caseName, testCount);
    napi_value value = nullptr;
    (void)napi_create_int32(env, time, &value);
    return value;
}

uint32_t MyXNode::TestPerformance(OH_Drawing_Canvas* canvas, std::string type, std::string caseName, uint32_t testCount)
{
    DRAWING_LOGE("MyXNode TestPerformance start");
    void* demoSo = dlopen("/system/lib/libdrawing_demo.z.so", RTLD_NOW | RTLD_LOCAL);
    if (demoSo == nullptr) {
        DRAWING_LOGE("xyj MyXNode TestPerformance failed to dlopen /system/lib/libdrawing_demo.z.so!");
        return -1;
    }
//    DRAWING_LOGE("xyj MyXNode TestPerformance success to dlopen!");
//    void *funcVoid = dlsym(demoSo, "testvoid");
//    if (funcVoid == nullptr) {
//        DRAWING_LOGE("xyj MyXNode TestPerformance failed to dlsym testvoid!");
//    } else {
//        DRAWING_LOGE("xyj MyXNode TestPerformance succese to dlsym void!");
//        using FUNC_VOID = void (*)(void);
//        FUNC_VOID func = (FUNC_VOID)funcVoid;
//        func();
//    }
//
//    void *funcInt = dlsym(demoSo, "testint");
//    if (funcInt == nullptr) {
//        DRAWING_LOGE("xyj MyXNode TestPerformance failed to dlsym funcInt!");
//    } else {
//        DRAWING_LOGE("xyj MyXNode TestPerformance succese to dlsym funcInt!");
//        using FUNC_INT = int (*)(int, int *);
//        FUNC_INT func = (FUNC_INT)funcInt;
//        int aa = 12;
//        int bb = 456;
//        int cc = func(aa, &bb);
//        DRAWING_LOGE("xyj MyXNode TestPerformance succese to dlsym funcInt! %{public}d", cc);
//    }
//    void *funcCanvas = dlsym(demoSo, "testcanvas");
//    if (funcCanvas == nullptr) {
//        DRAWING_LOGE("xyj MyXNode TestPerformance failed to dlsym funcCanvas!");
//    } else {
//        DRAWING_LOGE("xyj MyXNode TestPerformance succese to dlsym funcCanvas!");
//        using FUNC_CANVAS = int (*)(void *, int, int);
//        FUNC_CANVAS func = (FUNC_CANVAS)funcCanvas;
//        int cc = func(canvas, 22, 44);
//        DRAWING_LOGE("xyj MyXNode TestPerformance succese to dlsym funcCanvas! %{public}d", cc);
//    }

    void *funcD = dlsym(demoSo, "drawingDemo");
    if (funcD == nullptr) {
        DRAWING_LOGE("xyj MyXNode TestPerformance failed to dlsym func!");
    } else {
        DRAWING_LOGE("xyj MyXNode TestPerformance succese to dlsym func!");
        using FUNC = int (*)(void *, int, char* []);
        FUNC func = (FUNC)funcD;
        int argc = 5;
        char *argv[5];
        argv[0] = (char*)"drawing_demo";
        argv[1] = (char*)"performance";
        argv[2] = (char*)type.c_str();
        argv[3] = (char*)caseName.c_str();
        argv[4] = (char*)std::to_string(testCount).c_str();
        int ret = func(canvas, argc, argv);
        DRAWING_LOGE("xyj MyXNode TestPerformance succese to dlsym func! ret = %{public}d", ret);
    }

    DRAWING_LOGE("MyXNode TestPerformance start");
    return 0;
}

void MyXNode::Export(napi_env env, napi_value exports)
{
    DRAWING_LOGE("MyXNode MyXNode napi init");
    if ((env == nullptr) || (exports == nullptr)) {
        DRAWING_LOGE("Export: env or exports is null");
        return;
    }

    napi_property_descriptor desc[] = {
        {"TestPerformance", nullptr, MyXNode::NapiPerformance, nullptr, nullptr, nullptr, napi_default, nullptr},
    };
    if (napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc) != napi_ok) {
        DRAWING_LOGE("Export: napi_define_properties failed");
    }
}

EXTERN_C_START
static napi_value Init(napi_env env, napi_value exports) {
    DRAWING_LOGE("MyXNode myxnode napi init");
    MyXNode::Export(env, exports);
    return exports;
}
EXTERN_C_END

static napi_module demoModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = Init,
    .nm_modname = "myxnode",
    .nm_priv = ((void *)0),
    .reserved = {0},
};

extern "C" __attribute__((constructor)) void RegisterEntryModule(void) { napi_module_register(&demoModule); }
