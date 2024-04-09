/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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


#include "sync_report.h"

#include <dlfcn.h>
#include <cstdio>

namespace OHOS {
namespace Rosen {
namespace {
#if (defined(__aarch64__) || defined(__x86_64__))
    const std::string FRAME_AWARE_SO_PATH = "/system/lib64/platformsdk/libframe_ui_intf.z.so";
#else
    const std::string FRAME_AWARE_SO_PATH = "/system/lib/platformsdk/libframe_ui_intf.z.so";
#endif
}
SyncReport& SyncReport::GetInstance()
{
    static SyncReport instance;
    return instance;
}

SyncReport::SyncReport() {}

SyncReport::~SyncReport()
{
    CloseLibrary();
}

void SyncReport::Init()
{
    int ret = LoadLibrary();
    if (!ret) {
        return;
    }
    initFunc_ = (InitFunc)LoadSymbol("Init");
    if (initFunc_ != nullptr) {
        initFunc_();
    }
}

bool SyncReport::LoadLibrary()
{
    if (!frameSchedSoLoaded_) {
        frameSchedHandle_ = dlopen(FRAME_AWARE_SO_PATH.c_str(), RTLD_LAZY);
        if (frameSchedHandle_ == nullptr) {
            return false;
        }
        frameSchedSoLoaded_ = true;
    }
    return true;
}

void SyncReport::CloseLibrary()
{
    if (dlclose(frameSchedHandle_) != 0) {
        return;
    }
    frameSchedHandle_ = nullptr;
    frameSchedSoLoaded_ = false;
}

void *SyncReport::LoadSymbol(const char *symName)
{
    if (!frameSchedSoLoaded_) {
        return nullptr;
    }

    void *funcSym = dlsym(frameSchedHandle_, symName);
    if (funcSym == nullptr) {
        return nullptr;
    }
    return funcSym;
}

void SyncReport::MonitorGpuStart(uint32_t bufferCount, int flushEndTime)
{
    if (monitorGpuStartFunc_ == nullptr) {
        monitorGpuStartFunc_ = LoadSymbol("MonitorGpuStart");
    }
    if (monitorGpuStartFunc_ != nullptr) {
        auto monitorGpuStartFunc = reinterpret_cast<void (*)(uint32_t, int)>(monitorGpuStartFunc_);
        monitorGpuStartFunc(bufferCount, flushEndTime);
    }
}

void SyncReport::MonitorGpuEnd()
{
    if (monitorGpuEndFunc_ == nullptr) {
        monitorGpuEndFunc_ = LoadSymbol("MonitorGpuEnd");
    }
    if (monitorGpuEndFunc_ != nullptr) {
        auto monitorGpuEndFunc = reinterpret_cast<void (*)()>(monitorGpuEndFunc_);
        monitorGpuEndFunc();
    }
}

void SyncReport::SetFrameParam(int requestId, int load, int schedFrameNum, int value)
{
    if (setFrameParamFunc_ == nullptr) {
        setFrameParamFunc_ = LoadSymbol("SetFrameParam");
    }

    if (setFrameParamFunc_ != nullptr) {
        auto setFrameParamFunc = reinterpret_cast<void (*)(int, int, int, int)>(setFrameParamFunc_);
        setFrameParamFunc(requestId, load, schedFrameNum, value);
    }
}
} // namespace Rosen
} // namespace OHOS
