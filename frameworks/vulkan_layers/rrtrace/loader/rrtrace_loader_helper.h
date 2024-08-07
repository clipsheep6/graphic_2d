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

#ifndef RRTRACE_LOADER_HELPER_H
#define RRTRACE_LOADER_HELPER_H

#include <cstdlib>
#include <string>
#include <map>
#include <cstdarg>

#include <dirent.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <securec.h>
#if defined(RRTRACE_LOADER_OHOS)
#include <hilog/log.h>
#include <parameters.h>
#endif

#if defined(RRTRACE_LOADER_OHOS)
#define RRTRACE_LOADER_API __attribute__((visibility("default"), used))
#else
#error "Unknown platform"
#endif

#define RRTRACE_APP_LIB_URI "/data/storage/el1/bundle/libs/arm64/"
#define RRTRACE_APP_DATA_URI "/data/storage/el2/base/"
#define RRTRACE_DATA_LOCAL_URI "/data/local/rrtrace/"
#define RRTRACE_SYSTEM_LIB_URI "/system/lib64/"
#define RRTRACE_SHM_URI "/dev/shm/"

#define RRTRACE_CAPTURE_PARAM "rrtrace.capture"

#undef LOG_DOMAIN
// The "0xD001405" is the domain ID for graphic module that alloted by the OS.
#define LOG_DOMAIN 0xD001405

#define LOADER_TAG "RRTRACELOADER"

#define RRTRACE_LOGI(__format, ...) HiLogPrint(LOG_APP, LOG_INFO, LOG_DOMAIN, LOADER_TAG, __format, __VA_ARGS__)
#define RRTRACE_LOGW(__format, ...) HiLogPrint(LOG_APP, LOG_WARN, LOG_DOMAIN, LOADER_TAG, __format, __VA_ARGS__)
#define RRTRACE_LOGE(__format, ...) HiLogPrint(LOG_APP, LOG_ERROR, LOG_DOMAIN, LOADER_TAG, __format, __VA_ARGS__)
#define RRTRACE_LOGD(__format, ...) HiLogPrint(LOG_APP, LOG_DEBUG, LOG_DOMAIN, LOADER_TAG, __format, __VA_ARGS__)

namespace rrtrace {

struct CaptureLib {
    const char *name{nullptr};
    const char *shmName{nullptr};
    const bool mainEntry{false};
    int shmFd{-1};
    void *handle{nullptr};
};

inline CaptureLib g_captureLibs[] = {
    {"libVkLayer_gfxreconstruct.so", "rrtrace.GfxRecon"},
    {"libAPITrace.so",               "rrtrace.ApiTrace"},
    {"libPATrace.so",                "rrtrace.PaTrace"},
    {"libVkLayer_RrTraceCapture.so", "rrtrace.Capture", true},
};

size_t GetFileSize(const char *fileName);
size_t ReadFileData(const char *fileName, std::vector<uint8_t> &blob);
size_t WriteFileData(const char *fileName, const std::vector<uint8_t> &blob);

size_t StoreSharedLibrary(const char *libName, const std::vector<uint8_t> &blob);
void *DlopenSharedLibrary(const char *libFullName);
void *DlopenStoredSharedLibrary(const char *libName);
size_t TestStoredSharedLibrary(const char *libName);
void *DlopenBundledSharedLibrary(const char *libName);
size_t TestBundledSharedLibrary(const char *libName);

} // namespace rrtrace

#endif // RRTRACE_LOADER_HELPER_H
