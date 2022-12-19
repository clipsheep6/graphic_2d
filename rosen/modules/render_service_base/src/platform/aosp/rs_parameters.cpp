/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "platform/common/rs_parameters.h"

namespace OHOS {
namespace Rosen {
namespace system {
std::string GetParameter(const std::string& key, const std::string& def)
{
    return {};
}
}

int GetParameter(const char *key, const char *def, char *value, uint32_t len)
{
    return 0;
}

int WatchParameter(const char *keyPrefix, ParameterChgPtr callback, void *context)
{
    return 0;
}
} // namespace Rosen
} // namespace OHOS
