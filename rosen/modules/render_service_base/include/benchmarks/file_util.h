/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef RS_FILE_UTIL
#define RS_FILE_UTIL

#include <string>
#include "nlohmann/json.hpp"

namespace OHOS {
namespace Rosen {
bool WriteToFile(uintptr_t data, size_t size, const std::string& filePath);

} // namespace Rosen
} // namespace OHOS

#endif // RS_FILE_UTIL