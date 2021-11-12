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

#ifndef FRAMEWORKS_WM_INCLUDE_SINGLETON_DATA_DELEGATOR_H
#define FRAMEWORKS_WM_INCLUDE_SINGLETON_DATA_DELEGATOR_H

#include "singleton_container.h"

#include <string>

#define DEFINE_GLOBAL_DATA(name, ...) \
    using Type##name = __VA_ARGS__; \
    static inline SingletonDataDelegator<Type##name> name { #name }

namespace OHOS {
template<class T>
class SingletonDataDelegator {
public:
    SingletonDataDelegator(const std::string &name) : name_(name)
    {
        SingletonContainer::SetData<T>(name_, std::make_shared<T>());
    }
    ~SingletonDataDelegator() = default;

    const std::shared_ptr<T> &GetPtr()
    {
        return SingletonContainer::GetData<T>(name_);
    }

    T &Get()
    {
        return *SingletonContainer::GetData<T>(name_);
    }

private:
    std::string name_;
};
} // namespace OHOS

#endif // FRAMEWORKS_WM_INCLUDE_SINGLETON_DATA_DELEGATOR_H
