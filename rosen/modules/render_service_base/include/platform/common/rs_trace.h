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

#ifndef RENDER_SERVICE_BASE_CORE_COMMON_RS_TRACE_H
#define RENDER_SERVICE_BASE_CORE_COMMON_RS_TRACE_H

#include <string>

namespace OHOS {
namespace Rosen {
constexpr uint64_t HITRACE_TAG_GRAPHIC_AGP = (1ULL << 38);

void RSStartTrace(uint64_t label, const std::string& value, float limit = -1);
void RSFinishTrace(uint64_t label);
void RSHitraceScoped(uint64_t label, const std::string& value);
void RSHitraceMeterFmtScoped(uint64_t tag, const char *fmt, ...);
void RSStartAsyncTrace(uint64_t label, const std::string& value, int32_t taskId, float limit = -1);
void RSFinishAsyncTrace(uint64_t label, const std::string& value, int32_t taskId);
void RSCountTrace(uint64_t label, const std::string& name, int64_t count);

#ifndef ROSEN_TRACE_DISABLE
#define ROSEN_TRACE_BEGIN(tag, name) RSStartTrace(tag, name)
#define RS_TRACE_BEGIN(name) ROSEN_TRACE_BEGIN(HITRACE_TAG_GRAPHIC_AGP, name)
#define ROSEN_TRACE_END(tag) RSFinishTrace(tag)
#define RS_TRACE_END() ROSEN_TRACE_END(HITRACE_TAG_GRAPHIC_AGP)
#define RS_TRACE_NAME(name) RSHitraceScoped(HITRACE_TAG_GRAPHIC_AGP, name)
#define RS_TRACE_NAME_FMT(fmt, ...) RSHitraceMeterFmtScoped(HITRACE_TAG_GRAPHIC_AGP, fmt, ##__VA_ARGS__)
#define RS_ASYNC_TRACE_BEGIN(name, value) RSStartAsyncTrace(HITRACE_TAG_GRAPHIC_AGP, name, value)
#define RS_ASYNC_TRACE_END(name, value) RSFinishAsyncTrace(HITRACE_TAG_GRAPHIC_AGP, name, value)
#define RS_TRACE_INT(name, value) RSCountTrace(HITRACE_TAG_GRAPHIC_AGP, name, value)
#define RS_TRACE_FUNC() RS_TRACE_NAME(__func__)
#else
#define ROSEN_TRACE_BEGIN(tag, name)
#define RS_TRACE_BEGIN(name)
#define ROSEN_TRACE_END(tag)
#define RS_TRACE_END()
#define RS_TRACE_NAME(name)
#define RS_TRACE_NAME_FMT(fmt, ...)
#define RS_ASYNC_TRACE_BEGIN(name, value)
#define RS_ASYNC_TRACE_END(name, value)
#define RS_TRACE_INT(name, value)
#define RS_TRACE_FUNC()
#endif
}
}
#endif // RENDER_SERVICE_BASE_CORE_COMMON_RS_TRACE_H
