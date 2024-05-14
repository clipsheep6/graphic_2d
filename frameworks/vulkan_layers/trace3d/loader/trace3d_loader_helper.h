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

//
// Desc: This file is a part of the Trace3D Capture Gles/Vk Layer Loader
//

#ifndef TRACE3D_LOADER_HELPER_H
#define TRACE3D_LOADER_HELPER_H

#include <cstdlib>
#include <string>
#include <map>
#include <cstdarg>

#if defined(TRACE3D_LOADER_WIN)
#include <windows.h>
#else
#include <dirent.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#if defined(TRACE3D_LOADER_HOS)
#include <android/log.h>
#elif defined(TRACE3D_LOADER_OHOS)
#include <hilog/log.h>
#endif
#endif

#if defined(TRACE3D_LOADER_WIN)
#define TRACE3D_LOADER_API __declspec(dllexport)
#elif defined(TRACE3D_LOADER_HOS) || defined(TRACE3D_LOADER_OHOS)
#define TRACE3D_LOADER_API __attribute__((visibility("default"), used))
#else
#error "Unknown platform"
#endif

#define TRACE3D_APP_LIB_URI "/data/storage/el1/bundle/libs/arm64/"
#define TRACE3D_APP_DATA_URI "/data/storage/el2/base/"
#define TRACE3D_DATA_LOCAL_URI "/data/local/trace3d/"
#define TRACE3D_SYSTEM_LIB_URI "/system/lib64/"
#define TRACE3D_SHM_URI "/dev/shm/"

#define TRACE3D_LOGI(__format, ...) trace3d::LogOutput(trace3d::LOG_LEVEL::INFO, __format, __VA_ARGS__);
#define TRACE3D_LOGW(__format, ...) trace3d::LogOutput(trace3d::LOG_LEVEL::WARN, __format, __VA_ARGS__);
#define TRACE3D_LOGE(__format, ...) trace3d::LogOutput(trace3d::LOG_LEVEL::ERR, __format, __VA_ARGS__);
#define TRACE3D_LOGD(__format, ...) trace3d::LogOutput(trace3d::LOG_LEVEL::DEBUG, __format, __VA_ARGS__);

namespace trace3d {

struct CaptureLib {
    const char *name{nullptr};
    const char *shmName{nullptr};
    const bool mainEntry{false};
    int shmFd{-1};
    void *handle{nullptr};
};

inline CaptureLib captureLibs[] = {
    {"libVkLayer_gfxreconstruct.so", "trace3d.GfxRecon"},
    {"libAPITrace.so",               "trace3d.ApiTrace"},
    {"libPATrace.so",                "trace3d.PaTrace"},
    {"libVkLayer_Trace3DCapture.so", "trace3d.Capture", true},
};

enum class LOG_LEVEL : uint32_t {
    INFO = 0,
    WARN,
    ERR,
    DEBUG,
};

void LogOutputRaw(LOG_LEVEL level, const char *message);
void LogOutput(LOG_LEVEL level, const char *format, ...);
int FormatString(std::string &outStr, const char *format, va_list ap);
int FormatString(std::string &outStr, const char *format, ...);

size_t GetFileSize(const char *fileName);
size_t ReadFileData(const char *fileName, std::vector<uint8_t> &blob);
size_t WriteFileData(const char *fileName, const std::vector<uint8_t> &blob);

size_t StoreSharedLibrary(const char *libName, const std::vector<uint8_t> &blob);
void *DlopenSharedLibrary(const char *libFullName);
void *DlopenStoredSharedLibrary(const char *libName);
size_t TestStoredSharedLibrary(const char *libName);
void *DlopenBundledSharedLibrary(const char *libName);
size_t TestBundledSharedLibrary(const char *libName);

} // namespace trace3d

#endif // TRACE3D_LOADER_HELPER_H
