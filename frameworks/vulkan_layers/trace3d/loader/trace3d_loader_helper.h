//
// Copyright (c) Huawei Technologies Co., Ltd. 2023-2024. All rights reserved.
//

//
// Desc: This file is a part of the Trace3D Capture Gles/Vk Layer Loader
//
// Author: Dmitry Zhuk Z00858759 <zhuk.dmitry@huawei.com>
//

#pragma once

#include <stdlib.h>
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
#include <signal.h>
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

#define TRACE3D_FORMAT_MSG(__format, __msg) \
    std::string __msg; \
    do { \
        va_list ap; \
        va_start(ap, __format); \
        trace3d::FormatString(__msg, __format, ap); \
        va_end(ap); \
    } while (0)

namespace trace3d {

struct CaptureLib {
    const char *name{nullptr};
    const char *shmName{nullptr};
    const bool mainEntry{false};
    int shmFd{-1};
    void *handle{nullptr};
};

static inline CaptureLib captureLibs[] = {
    {"libVkLayer_gfxreconstruct.so", "trace3d.GfxRecon"},
    {"libAPITrace.so",               "trace3d.ApiTrace"},
    {"libPATrace.so",                "trace3d.PaTrace"},
    {"libVkLayer_Trace3DCapture.so", "trace3d.Capture", true},
};

static inline int FormatString(std::string &outStr, const char *format, va_list ap) {
    int size = 0;
    size = vsnprintf(NULL, 0, format, ap) + 1;

    outStr.resize(size);

    size = vsnprintf(const_cast<char *const>(outStr.data()), outStr.size(), format, ap);
    return size;
}

static inline int FormatString(std::string &outStr, const char *format, ...) {
    va_list ap;
    va_start(ap, format);

    int size = FormatString(outStr, format, ap);

    va_end(ap);
    return size;
}

enum class LOG_LEVEL : uint32_t {
    INFO = 0,
    WARN,
    ERR,
    DEBUG,
};

static inline void LogOutputRaw(LOG_LEVEL level, const char *message) {
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

static inline void LogOutput(LOG_LEVEL level, const char *format, ...) {
    TRACE3D_FORMAT_MSG(format, msg);
    msg = std::string("LOADER") + " : " + msg;

    LogOutputRaw(level, msg.c_str());
}

static inline size_t GetFileSize(const char *fileName) {
    size_t size = 0;
    FILE *f = fopen(fileName, "rb");
    if (f) {
        fseek(f, 0L, SEEK_END);
        size = (size_t)ftell(f);
        fclose(f);
    }
    return size;
}

static inline size_t ReadFileData(const char *fileName, std::vector<uint8_t> &blob) {
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
    } else
        TRACE3D_LOGE("%s:%d ERROR: open read file '%s'\n", __FUNCTION__, __LINE__, fileName);
    return 0;
}

static inline size_t WriteFileData(const char *fileName, const std::vector<uint8_t> &blob) {
    FILE *f = fopen(fileName, "wb");
    if (f) {
        size_t writeLen = fwrite(blob.data(), 1, blob.size(), f);

        fflush(f);
        fclose(f);
        if (writeLen == blob.size())
            return writeLen;
        TRACE3D_LOGE("%s:%d ERROR: write file '%s' len:%d/%d Bytes\n", __FUNCTION__, __LINE__, fileName, blob.size(), writeLen);
    } else {
        TRACE3D_LOGE("%s:%d ERROR: open write file '%s'\n", __FUNCTION__, __LINE__, fileName);
    }
    return -1;
}

static inline size_t StoreSharedLibrary(const char *libName, const std::vector<uint8_t> &blob) {
    size_t ret = 0;
    std::string libFullName = std::string(TRACE3D_APP_DATA_URI) + libName;
    ret = WriteFileData(libFullName.c_str(), blob);
    if (ret > 0) {
        TRACE3D_LOGE("%s:%d Write '%s' len:%d Bytes\n", __FUNCTION__, __LINE__, libFullName.c_str(), ret);
    }
    return ret;
}

static inline void *DlopenSharedLibrary(const char *libFullName) {
    void *handle = 0;
    handle = dlopen(libFullName, RTLD_LAZY);

    size_t sizeBytes = GetFileSize(libFullName);
    if (sizeBytes > 0) {
        if (handle) {
            TRACE3D_LOGI("%s:%d dlopen '%s' handle:%p\n", __FUNCTION__, __LINE__, libFullName, handle);
        }
        else {
            TRACE3D_LOGE("%s:%d ERROR: dlopen '%s' err: '%s'\n", __FUNCTION__, __LINE__, libFullName, dlerror());
        }
    }
    return handle;
}

static inline void *DlopenStoredSharedLibrary(const char *libName) {
    const std::string libFullName = std::string(TRACE3D_APP_DATA_URI) + libName;
    return DlopenSharedLibrary(libFullName.c_str());
}

static inline size_t TestStoredSharedLibrary(const char *libName) {
    const std::string libFullName = std::string(TRACE3D_APP_DATA_URI) + libName;
    return GetFileSize(libFullName.c_str());
}

static inline void *DlopenBundledSharedLibrary(const char *libName) {
    void *libHandle = DlopenSharedLibrary((std::string(TRACE3D_APP_LIB_URI) + libName).c_str());
    if (!libHandle) {
        libHandle = DlopenSharedLibrary((std::string(TRACE3D_DATA_LOCAL_URI) + libName).c_str());
        if (!libHandle) {
            libHandle = DlopenSharedLibrary((std::string(TRACE3D_SYSTEM_LIB_URI) + libName).c_str());
        }
    }
    return libHandle;
}

static inline size_t TestBundledSharedLibrary(const char *libName) {
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
