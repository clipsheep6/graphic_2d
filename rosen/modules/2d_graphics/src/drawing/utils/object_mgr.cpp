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

#include "utils/object_mgr.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
void ObjectMgr::AddObject(void* obj)
{
    std::lock_guard<std::mutex> lock(mutex_);
    vector_.push_back(obj);
}

bool ObjectMgr::HasObject(void* obj)
{
    bool has = false;
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto it = vector_.begin(); it != vector_.end(); it++) {
        if (*it == obj) {
            has = true;
            break;
        }
    }
    return has;
}

bool ObjectMgr::RemoveObject(void* obj)
{
    bool removed = false;
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto it = vector_.begin(); it != vector_.end(); it++) {
        if (*it == obj) {
            vector_.erase(it);
            removed = true;
            break;
        }
    }
    return removed;
}

size_t ObjectMgr::ObjectCount()
{
    return vector_.size();
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
