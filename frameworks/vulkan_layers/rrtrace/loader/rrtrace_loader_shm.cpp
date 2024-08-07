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

static bool ShmOpenLibrary(const std::string &libPath, CaptureLib &lib)
{
    if (lib.shmFd >= 0) {
        return true;
    }
    std::string libFullName = libPath + lib.name;
    std::vector<uint8_t> blob;

    size_t sizeBytes = GetFileSize(libFullName.c_str());
    sizeBytes = sizeBytes > 0 ? ReadFileData(libFullName.c_str(), blob) : 0;
    if (sizeBytes > 0 && blob.size() == sizeBytes) {
        size_t len = sizeBytes;

        lib.shmFd = shm_open(lib.shmName, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        if (lib.shmFd < 0) {
            RRTRACE_LOGE("%s:%d ERROR: shm_open '%s'\n", __FUNCTION__, __LINE__, lib.shmName);
            return false;
        }
        
        ftruncate(lib.shmFd, len);

        void *mem = mmap(nullptr, len, PROT_WRITE, MAP_SHARED, lib.shmFd, 0);
        if (mem) {
            errno_t ret = memcpy_s(mem, len, blob.data(), len);
            if (ret != EOK) {
                RRTRACE_LOGE("%s:%d ERROR: memcpy_s! Error code: %d\n", __FUNCTION__, __LINE__, ret);
            }
            munmap(mem, len);

            RRTRACE_LOGI("%s:%d shm_open '%s' fd:%d\n", __FUNCTION__, __LINE__, lib.shmName, lib.shmFd);
        } else {
            shm_unlink(lib.shmName);
            close(lib.shmFd);
            lib.shmFd = -1;

            RRTRACE_LOGE("%s:%d ERROR: mmap\n", __FUNCTION__, __LINE__);
        }
    }
    return lib.shmFd >= 0;
}

void ShmCaptureInit()
{
    for (auto &lib : g_captureLibs) {
        size_t foundSize = TestBundledSharedLibrary(lib.name);
        if (foundSize > 0) {
            RRTRACE_LOGI("%s:%d found bundle:'%s', size:%zu\n", __FUNCTION__, __LINE__, lib.name, foundSize);

            if (!ShmOpenLibrary(RRTRACE_APP_LIB_URI, lib)) {
            }
        }
    }
}

void ShmCaptureCleanup()
{
    for (auto &lib : g_captureLibs) {
        if (lib.shmFd >= 0) {
            shm_unlink(lib.shmName);
            close(lib.shmFd);
            lib.shmFd = -1;
        }
    }
}

} // namespace rrtrace
