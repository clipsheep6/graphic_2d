/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "buffer_manager.h"

#include <mutex>

#define CHECK_INIT() \
    do { \
        if (true) { \
            GSError ret = Init(); \
            if (ret != GSERROR_OK) { \
                return ret; \
            } \
        } \
    } while (0)

#define CHECK_BUFFER(buffer) \
    do { \
        if ((buffer) == nullptr) { \
            return GSERROR_INVALID_ARGUMENTS; \
        } \
    } while (0)

namespace OHOS {
sptr<BufferManager> BufferManager::GetInstance()
{
    if (instance == nullptr) {
        static std::mutex mutex;
        std::lock_guard<std::mutex> lock(mutex);
        if (instance == nullptr) {
            instance = new BufferManager();
        }
    }
    return instance;
}

GSError BufferManager::Init()
{
    return GSERROR_OK;
}

GSError BufferManager::Alloc(const BufferRequestConfig &config, sptr<SurfaceBuffer> &buffer)
{
    int32_t allocWidth = config.width;
    int32_t allocHeight = config.height;
    if (config.transform == TransformType::ROTATE_90 || config.transform == TransformType::ROTATE_270) {
        std::swap(allocWidth, allocHeight);
    }
    BufferHandle *handle = new BufferHandle();
    int32_t size = allocWidth * allocHeight * 0x4;
    *handle = {
        .fd = -1,
        .width = allocWidth,
        .stride = allocWidth * 0x4,
        .height = allocHeight,
        .size = size,
        .format = config.format,
        .usage = config.usage,
        .virAddr = reinterpret_cast<void *>(new char[size]),
        .phyAddr = 0,
        .key = -1,
        .reserveFds = 0,
        .reserveInts = 0,
    };
    buffer->SetBufferHandle(handle);
    buffer->SetSurfaceBufferWidth(allocWidth);
    buffer->SetSurfaceBufferHeight(allocHeight);
    buffer->SetSurfaceBufferColorGamut(config.colorGamut);
    buffer->SetSurfaceBufferTransform(config.transform);
    return GSERROR_OK;
}

GSError BufferManager::Map(sptr<SurfaceBuffer> &buffer)
{
    CHECK_INIT();
    CHECK_BUFFER(buffer);

    BufferHandle *handle = buffer->GetBufferHandle();
    if (handle == nullptr) {
        return GSERROR_INVALID_ARGUMENTS;
    }

    return GSERROR_OK;
}

GSError BufferManager::Unmap(sptr<SurfaceBuffer> &buffer)
{
    CHECK_INIT();
    CHECK_BUFFER(buffer);

    BufferHandle *handle = buffer->GetBufferHandle();
    if (handle == nullptr) {
        return GSERROR_INVALID_ARGUMENTS;
    }

    return GSERROR_OK;
}

GSError BufferManager::Unmap(BufferHandle *bufferHandle)
{
    CHECK_INIT();
    if (bufferHandle == nullptr) {
        return GSERROR_INVALID_ARGUMENTS;
    }

    return GSERROR_OK;
}

GSError BufferManager::FlushCache(sptr<SurfaceBuffer> &buffer)
{
    CHECK_INIT();
    CHECK_BUFFER(buffer);

    BufferHandle *handle = buffer->GetBufferHandle();
    if (handle == nullptr) {
        return GSERROR_INVALID_ARGUMENTS;
    }

    return GSERROR_OK;
}

GSError BufferManager::InvalidateCache(sptr<SurfaceBuffer> &buffer)
{
    CHECK_INIT();
    CHECK_BUFFER(buffer);

    BufferHandle *handle = buffer->GetBufferHandle();
    if (handle == nullptr) {
        return GSERROR_INVALID_ARGUMENTS;
    }

    return GSERROR_OK;
}

GSError BufferManager::Free(sptr<SurfaceBuffer> &buffer)
{
    CHECK_INIT();
    CHECK_BUFFER(buffer);

    BufferHandle *handle = buffer->GetBufferHandle();
    buffer->SetBufferHandle(nullptr);
    delete [](reinterpret_cast<char *>(handle->virAddr));
    delete handle;
    return GSERROR_OK;
}
} // namespace OHOS
