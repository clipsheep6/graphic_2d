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

#ifndef ROSEN_MODULES_TEXGINE_EXPORT_TEXGINE_UTILS_MEMORY_OBJECT_H
#define ROSEN_MODULES_TEXGINE_EXPORT_TEXGINE_UTILS_MEMORY_OBJECT_H

#include <string>

namespace OHOS {
namespace Rosen {
namespace TextEngine {
class MemoryObject {
protected:
    virtual ~MemoryObject() = default;
    virtual void ReportMemoryUsage(const std::string &member, bool needThis) const = 0;
};
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_MODULES_TEXGINE_EXPORT_TEXGINE_UTILS_MEMORY_OBJECT_H
