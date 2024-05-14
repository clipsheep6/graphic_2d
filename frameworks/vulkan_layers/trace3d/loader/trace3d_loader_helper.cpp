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

#include "trace3d_loader_helper.h"

#define TRACE3D_FORMAT_MSG(__format, __msg) \
    std::string __msg; \
    do { \
        va_list ap; \
        va_start(ap, __format); \
        trace3d::FormatString(__msg, __format, ap); \
        va_end(ap); \
    } while (0)

namespace trace3d {

void LogOutputRaw(LOG_LEVEL level, const char *message)
{
#if defined(TRACE3D_LOADER_HOS)
    int prio = ANDROID_LOG_UNKNOWN;
    switch (level) {
        case LOG_LEVEL::DEBUG:
            prio = ANDROID_LOG_DEBUG;
            break;
        case LOG_LEVEL::ERR:
            prio = ANDROID_LOG_ERROR;
            break;
        case LOG_LEVEL::WARN:
            prio = ANDROID_LOG_WARN;
            break;
        case LOG_LEVEL::INFO:
            prio = ANDROID_LOG_INFO;
            break;
        default:
            break;
    }
    __android_log_write(prio, "TRACE3D", message);
#elif defined(TRACE3D_LOADER_OHOS)
    LogLevel priority = LogLevel::LOG_INFO;
    switch (level) {
        case LOG_LEVEL::DEBUG:
            priority = LOG_WARN;
            break; // Set WARN level for OHOS, because DEBUG level not worked
        case LOG_LEVEL::ERR:
            priority = LOG_ERROR;
            break;
        case LOG_LEVEL::WARN:
            priority = LOG_WARN;
            break;
        case LOG_LEVEL::INFO:
            priority = LOG_INFO;
            break;
        // case LOG_LEVEL::FATAL: priority = LOG_FATAL; break;
        default:
            break;
    }
    // OH_LOG_Print(LOG_APP, priority, 1, "TRACE3D", "%s", message);
#endif
}

void LogOutput(LOG_LEVEL level, const char *format, ...)
{
    TRACE3D_FORMAT_MSG(format, msg);
    msg = std::string("LOADER") + " : " + msg;

    LogOutputRaw(level, msg.c_str());
}

int FormatString(std::string &outStr, const char *format, va_list ap)
{
    int size = 0;
    size = vsnprintf(NULL, 0, format, ap) + 1;

    outStr.resize(size);

    size = vsnprintf(const_cast<char *const>(outStr.data()), outStr.size(), format, ap);
    return size;
}

int FormatString(std::string &outStr, const char *format, ...)
{
    va_list ap;
    va_start(ap, format);

    int size = FormatString(outStr, format, ap);

    va_end(ap);
    return size;
}


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

        TRACE3D_LOGI("%s:%d Opened file '%s' len:%d Bytes\n", __FUNCTION__, __LINE__, fileName, len);

        blob.resize(len);
        size_t readLen = fread(blob.data(), 1, len, f);

        fclose(f);
        if (readLen == len) {
            return readLen;
        }
        TRACE3D_LOGE("%s:%d ERROR: read file '%s' len:%d/%d Bytes\n", __FUNCTION__, __LINE__, fileName, len, readLen);
    } else {
        TRACE3D_LOGE("%s:%d ERROR: open read file '%s'\n", __FUNCTION__, __LINE__, fileName);
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
        TRACE3D_LOGE("%s:%d ERROR: write file '%s' len:%d/%d Bytes\n",
            __FUNCTION__, __LINE__, fileName, blob.size(), writeLen);
    } else {
        TRACE3D_LOGE("%s:%d ERROR: open write file '%s'\n", __FUNCTION__, __LINE__, fileName);
    }
    return -1;
}

size_t StoreSharedLibrary(const char *libName, const std::vector<uint8_t> &blob)
{
    size_t ret = 0;
    std::string libFullName = std::string(TRACE3D_APP_DATA_URI) + libName;
    ret = WriteFileData(libFullName.c_str(), blob);
    if (ret > 0) {
        TRACE3D_LOGE("%s:%d Write '%s' len:%d Bytes\n", __FUNCTION__, __LINE__, libFullName.c_str(), ret);
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
            TRACE3D_LOGI("%s:%d dlopen '%s' handle:%p\n", __FUNCTION__, __LINE__, libFullName, handle);
        } else {
            TRACE3D_LOGE("%s:%d ERROR: dlopen '%s' err: '%s'\n", __FUNCTION__, __LINE__, libFullName, dlerror());
        }
    }
    return handle;
}

void *DlopenStoredSharedLibrary(const char *libName)
{
    const std::string libFullName = std::string(TRACE3D_APP_DATA_URI) + libName;
    return DlopenSharedLibrary(libFullName.c_str());
}

size_t TestStoredSharedLibrary(const char *libName)
{
    const std::string libFullName = std::string(TRACE3D_APP_DATA_URI) + libName;
    return GetFileSize(libFullName.c_str());
}

void *DlopenBundledSharedLibrary(const char *libName)
{
    void *libHandle = DlopenSharedLibrary((std::string(TRACE3D_APP_LIB_URI) + libName).c_str());
    if (!libHandle) {
        libHandle = DlopenSharedLibrary((std::string(TRACE3D_DATA_LOCAL_URI) + libName).c_str());
        if (!libHandle) {
            libHandle = DlopenSharedLibrary((std::string(TRACE3D_SYSTEM_LIB_URI) + libName).c_str());
        }
    }
    return libHandle;
}

size_t TestBundledSharedLibrary(const char *libName)
{
    size_t sizeBytes = GetFileSize((std::string(TRACE3D_APP_LIB_URI) + libName).c_str());
    if (!sizeBytes) {
        sizeBytes = GetFileSize((std::string(TRACE3D_DATA_LOCAL_URI) + libName).c_str());
        if (!sizeBytes) {
            sizeBytes = GetFileSize((std::string(TRACE3D_SYSTEM_LIB_URI) + libName).c_str());
        }
    }
    return sizeBytes;
}

} // namespace trace3d