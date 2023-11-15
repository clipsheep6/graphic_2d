/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef HDI_BACKEND_DISPLAY_BUFFER_INTERFACE_H
#define HDI_BACKEND_DISPLAY_BUFFER_INTERFACE_H

#ifndef DRIVERS_INTERFACE_DISPLAY_ENABLE
#include <vector>
#include "buffer_handle.h"
#include "buffer_log.h"
#include "parcel.h"
#include "refbase.h"
#include "iremote_object.h"
#include "surface_type.h"
#endif

namespace OHOS {
namespace Rosen {
#ifndef DRIVERS_INTERFACE_DISPLAY_ENABLE
namespace {
struct AllocInfo {
    unsigned int width;
    unsigned int height;
    unsigned long usage;
    unsigned int format;
    unsigned int expectedSize;
};

struct VerifyAllocInfo {
    unsigned int width; 
    unsigned int height;
    unsigned long usage;
    unsigned int format;
};
} // namespace

class DisplayBufferInterface : public RefBase {
public:
    DisplayBufferInterface() = default;
    ~DisplayBufferInterface() = default;
    static DisplayBufferInterface* Get()
    {
        HLOGD("%{public}s: drivers_interface_display part is not enabled.", __func__);
        return nullptr;
    };

    bool AddDeathRecipient(const sptr<IRemoteObject::DeathRecipient>& recipient)
    {
        return false;
    };

    bool RemoveDeathRecipient()
    {
        return false;
    };

    int32_t AllocMem(const AllocInfo& info, BufferHandle*& handle)
    {
        return GRAPHIC_DISPLAY_FAILURE;
    }

    void FreeMem(const BufferHandle& handle)
    {
    }

    void *Mmap(const BufferHandle& handle)
    {
        return nullptr;
    }

    int32_t Unmap(const BufferHandle& handle)
    {
        return GRAPHIC_DISPLAY_FAILURE;
    }

    int32_t FlushCache(const BufferHandle& handle)
    {
        return GRAPHIC_DISPLAY_FAILURE;
    }

    int32_t InvalidateCache(const BufferHandle& handle)
    {
        return GRAPHIC_DISPLAY_FAILURE;
    }

    int32_t IsSupportedAlloc(
        const std::vector<VerifyAllocInfo>& infos, std::vector<bool>& supporteds)
    {
        return GRAPHIC_DISPLAY_FAILURE;
    }

};
#endif

} // namespace Rosen
} // namespace OHOS
#endif // HDI_BACKEND_DISPLAY_BUFFER_INTERFACE_H
