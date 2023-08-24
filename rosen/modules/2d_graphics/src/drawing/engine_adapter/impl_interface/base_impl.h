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

#ifndef BASE_IMPL_H
#define BASE_IMPL_H

#include <memory>

#include "adapter_type.h"
#include "utils/drawing_macros.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class BaseImpl : public std::enable_shared_from_this<BaseImpl> {
public:
    static inline constexpr AdapterType TYPE = AdapterType::NONE;
    BaseImpl() noexcept {}
    virtual ~BaseImpl() {}

    virtual AdapterType GetType() const
    {
        return AdapterType::NONE;
    }

    template<typename T>
    bool IsInstanceOf()
    {
        return (GetType() == T::TYPE);
    }

    template<typename T>
    std::shared_ptr<T> DowncastingTo()
    {
        return (IsInstanceOf<T>()) ? std::static_pointer_cast<T>(shared_from_this()) : nullptr;
    }
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif
