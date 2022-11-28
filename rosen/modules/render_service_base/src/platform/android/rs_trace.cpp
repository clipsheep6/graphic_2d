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

#include "platform/common/rs_trace.h"

namespace OHOS {
namespace Rosen {
void RSStartTrace(uint64_t label, const std::string& value, float limit)
{
    return;
}

void RSFinishTrace(uint64_t label)
{
    return;
}

void RSHitraceScoped(uint64_t label, const std::string& value)
{
    return;
}

void RSHitraceMeterFmtScoped(uint64_t tag, const char *fmt, ...)
{
    return;
}

void RSStartAsyncTrace(uint64_t label, const std::string& value, int32_t taskId, float limit)
{
    return;
}

void RSFinishAsyncTrace(uint64_t label, const std::string& value, int32_t taskId)
{
    return;
}

void RSCountTrace(uint64_t label, const std::string& name, int64_t count)
{
    return;
}
} // namespace Rosen
} // namespace OHOS
