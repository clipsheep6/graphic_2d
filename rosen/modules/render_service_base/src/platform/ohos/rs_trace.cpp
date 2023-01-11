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
#include "platform/common/rs_trace.h"

#include <cstdarg>

#include "hitrace_meter.h"

namespace OHOS {
namespace Rosen {
void RSStartTrace(uint64_t label, const std::string& value, float limit)
{
    StartTrace(label, value, limit);
}
void RSFinishTrace(uint64_t label)
{
    FinishTrace(label);
}
void RSHitraceScoped(uint64_t label, const std::string& value)
{
    HITRACE_METER_NAME(label, value);
}
void RSHitraceMeterFmtScoped(uint64_t tag, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    HITRACE_METER_FMT(tag, fmt, args);
}
void RSStartAsyncTrace(uint64_t label, const std::string& value, int32_t taskId, float limit)
{
    StartAsyncTrace(label, value, taskId, limit);
}
void RSFinishAsyncTrace(uint64_t label, const std::string& value, int32_t taskId)
{
    FinishAsyncTrace(label, value, taskId);
}
void RSCountTrace(uint64_t label, const std::string& name, int64_t count)
{
    CountTrace(label, name, count);
}
} // namespace Rosen
} // namespace OHOS
