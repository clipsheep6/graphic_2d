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

#ifndef ROSEN_RENDER_SERVICE_BASE_PLATFORM_OHOS_RS_RESOURCE_REF_HOLDER_H
#define ROSEN_RENDER_SERVICE_BASE_PLATFORM_OHOS_RS_RESOURCE_REF_HOLDER_H

#include "nocopyable.h"

namespace OHOS {
namespace Rosen {
using ReleaseFunctionType = void (*)(void*);
using ReleaseContextType = void*;

struct ResourceReleaseHelper {
    ReleaseFunctionType releaseFunc_{nullptr};
    ReleaseContextType releaseContext_{nullptr};
};

template <typename ResourceType>
class SharedResourceContext {
public:
    DISALLOW_COPY_AND_MOVE(SharedResourceContext);

    explicit SharedResourceContext(std::shared_ptr<ResourceType> resource)
        : resource_(std::move(resource)) {}

private:
    std::shared_ptr<ResourceType> resource_;
};

template <typename ResourceType>
inline void DeleteSharedResourceContext(void* resourceContext)
{
    SharedResourceContext<ResourceType>* context = static_cast<SharedResourceContext<ResourceType>*>(resourceContext);
    if (context != nullptr) {
        delete context;
        context = nullptr;
    }
}

template <typename ResourceType>
class SharedResourceRefHolder {
public:
    DISALLOW_COPY_AND_MOVE(SharedResourceRefHolder);

    SharedResourceRefHolder() = default;

    explicit SharedResourceRefHolder(const std::shared_ptr<ResourceType>& resource)
    {
        Ref(resource);
    }

    void Ref(const std::shared_ptr<ResourceType>& resource)
    {
        if (context_ == nullptr) {
            context_ = new SharedResourceContext<ResourceType>(resource);
        }
    }

    void UnRef()
    {
        if (context_ != nullptr) {
            delete context_;
            context_ = nullptr;
        }
    }

    void ConditionalUnRef(const std::shared_ptr<bool>& needUnRef)
    {
        if (!needUnRef || !(*needUnRef)) {
            return;
        }
        UnRef();
    }

    ResourceReleaseHelper GetReleaseHelper() const
    {
        ResourceReleaseHelper releaseHelper = {
            .releaseFunc_ = deleter_,
            .releaseContext_ = context_
        };
        return releaseHelper;
    }

private:
    static inline constexpr ReleaseFunctionType deleter_ = [](void* resourceContext) {
        DeleteSharedResourceContext<ResourceType>(resourceContext);
    };

    SharedResourceContext<ResourceType>* context_{nullptr};
};
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_RENDER_SERVICE_BASE_PLATFORM_OHOS_RS_RESOURCE_REF_HOLDER_H
