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

#include "rrtrace_loader_helper.h"

namespace rrtrace {

size_t GetFileSize(const char *fileName)
{
    size_t size = 0;
    FILE *f = fopen(fileName, "rb");
    if (f) {
        fseek(f, 0L, SEEK_END);
        size = (size_t)ftell(f);
        fclose(f);
    }
    return size;
}

size_t ReadFileData(const char *fileName, std::vector<uint8_t> &blob)
{
    FILE *f = fopen(fileName, "rb");
    if (f) {
        fseek(f, 0L, SEEK_END);
        size_t len = (size_t)ftell(f);
        fseek(f, 0L, SEEK_SET);

        RRTRACE_LOGI("%s:%d Opened file '%s' len:%zu Bytes\n", __FUNCTION__, __LINE__, fileName, len);

        blob.resize(len);
        size_t readLen = fread(blob.data(), 1, len, f);

        fclose(f);
        if (readLen == len) {
            return readLen;
        }
        RRTRACE_LOGE("%s:%d ERROR: read file '%s' len:%zu/%zu Bytes\n", __FUNCTION__, __LINE__, fileName, len, readLen);
    } else {
        RRTRACE_LOGE("%s:%d ERROR: open read file '%s'\n", __FUNCTION__, __LINE__, fileName);
    }
    return 0;
}

size_t WriteFileData(const char *fileName, const std::vector<uint8_t> &blob)
{
    FILE *f = fopen(fileName, "wb");
    if (f) {
        size_t writeLen = fwrite(blob.data(), 1, blob.size(), f);

        fflush(f);
        fclose(f);
        if (writeLen == blob.size()) {
            return writeLen;
        }
        RRTRACE_LOGE("%s:%d ERROR: write file '%s' len:%zu/%zu Bytes\n",
            __FUNCTION__, __LINE__, fileName, blob.size(), writeLen);
    } else {
        RRTRACE_LOGE("%s:%d ERROR: open write file '%s'\n", __FUNCTION__, __LINE__, fileName);
    }
    return -1;
}

size_t StoreSharedLibrary(const char *libName, const std::vector<uint8_t> &blob)
{
    size_t ret = 0;
    std::string libFullName = std::string(RRTRACE_APP_DATA_URI) + libName;
    ret = WriteFileData(libFullName.c_str(), blob);
    if (ret > 0) {
        RRTRACE_LOGE("%s:%d Write '%s' len:%zu Bytes\n", __FUNCTION__, __LINE__, libFullName.c_str(), ret);
    }
    return ret;
}

void *DlopenSharedLibrary(const char *libFullName)
{
    void *handle = nullptr;
    handle = dlopen(libFullName, RTLD_LAZY);

    size_t sizeBytes = GetFileSize(libFullName);
    if (sizeBytes > 0) {
        if (handle) {
            RRTRACE_LOGI("%s:%d dlopen '%s' handle:%p\n", __FUNCTION__, __LINE__, libFullName, handle);
        } else {
            RRTRACE_LOGE("%s:%d ERROR: dlopen '%s' err: '%s'\n", __FUNCTION__, __LINE__, libFullName, dlerror());
        }
    }
    return handle;
}

void *DlopenStoredSharedLibrary(const char *libName)
{
    const std::string libFullName = std::string(RRTRACE_APP_DATA_URI) + libName;
    return DlopenSharedLibrary(libFullName.c_str());
}

size_t TestStoredSharedLibrary(const char *libName)
{
    const std::string libFullName = std::string(RRTRACE_APP_DATA_URI) + libName;
    return GetFileSize(libFullName.c_str());
}

void *DlopenBundledSharedLibrary(const char *libName)
{
    void *libHandle = DlopenSharedLibrary((std::string(RRTRACE_APP_LIB_URI) + libName).c_str());
    if (!libHandle) {
        libHandle = DlopenSharedLibrary((std::string(RRTRACE_DATA_LOCAL_URI) + libName).c_str());
        if (!libHandle) {
            libHandle = DlopenSharedLibrary((std::string(RRTRACE_SYSTEM_LIB_URI) + libName).c_str());
        }
    }
    return libHandle;
}

size_t TestBundledSharedLibrary(const char *libName)
{
    size_t sizeBytes = GetFileSize((std::string(RRTRACE_APP_LIB_URI) + libName).c_str());
    if (!sizeBytes) {
        sizeBytes = GetFileSize((std::string(RRTRACE_DATA_LOCAL_URI) + libName).c_str());
        if (!sizeBytes) {
            sizeBytes = GetFileSize((std::string(RRTRACE_SYSTEM_LIB_URI) + libName).c_str());
        }
    }
    return sizeBytes;
}

} // namespace rrtrace
