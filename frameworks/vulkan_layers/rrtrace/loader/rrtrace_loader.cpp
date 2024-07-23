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

#include "rrtrace_loader_shm.h"
#include "rrtrace_loader_helper.h"

namespace rrtrace {

void* GetCaptureEntryLibHandle()
{
    for (auto &lib : g_captureLibs) {
        if (lib.handle && lib.mainEntry) {
            return lib.handle;
        }
    }
    return nullptr;
}

void* CaptureInit()
{
    std::string rrtraceCaptureParam = OHOS::system::GetParameter(RRTRACE_CAPTURE_PARAM, "0");
    if (rrtraceCaptureParam != "1") {
        RRTRACE_LOGI("%s:%d %s:'%s'. libVkLayer_RrTraceCapture.so will not be loaded\n",
            __FUNCTION__, __LINE__, RRTRACE_CAPTURE_PARAM, rrtraceCaptureParam.c_str());
        return nullptr;
    }
    
    ShmCaptureInit();

    void *libHandle = GetCaptureEntryLibHandle();
    if (libHandle) {
        return libHandle;
    }
    for (auto &lib : g_captureLibs) {
        if (lib.handle) {
            continue;
        }
        size_t foundSize = TestBundledSharedLibrary(lib.name);
        if (foundSize > 0) {
            RRTRACE_LOGI("%s:%d found bundle:'%s', size:%zu\n", __FUNCTION__, __LINE__, lib.name, foundSize);

            if (!lib.handle && lib.mainEntry) {
                libHandle = lib.handle = DlopenBundledSharedLibrary(lib.name);
            }
            continue;
        }
        
        foundSize = TestStoredSharedLibrary(lib.name);
        if (foundSize > 0) {
            RRTRACE_LOGI("%s:%d found stored:'%s', size:%zu\n", __FUNCTION__, __LINE__, lib.name, foundSize);
        }

        const std::string shmFullName = std::string(RRTRACE_SHM_URI) + lib.shmName;
        const size_t shmSize = GetFileSize(shmFullName.c_str());
        if (shmSize > 0 && foundSize != shmSize) {
            std::vector<uint8_t> blob;
    
            if (ReadFileData(shmFullName.c_str(), blob) > 0) {
                StoreSharedLibrary(lib.name, blob);
            }
            foundSize = TestStoredSharedLibrary(lib.name);
            if (foundSize > 0) {
                RRTRACE_LOGI("%s:%d found stored:'%s', size:%zu\n", __FUNCTION__, __LINE__, lib.name, foundSize);
            }
        }
        if (foundSize > 0) {
            if (!lib.handle && lib.mainEntry) {
                libHandle = lib.handle = DlopenStoredSharedLibrary(lib.name);
            }
        }
    }

    return libHandle;
}

void CaptureCleanup()
{
    for (auto &lib : g_captureLibs) {
        if (lib.handle) {
            dlclose(lib.handle);
            lib.handle = nullptr;
        }
    }
    ShmCaptureCleanup();
}

} // namespace rrtrace

__attribute__((destructor)) static void CleanupCaptureLoaderLib()
{
    rrtrace::CaptureCleanup();
}
