/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#include "hitrace_meter.h"

using namespace std;

using MarkerType = uint64_t;
#define UNUSED_PARAM

void AddHitraceMeterMarker(MarkerType type, uint64_t tag, const std::string& name, const std::string& value)
{
}

void UpdateTraceLabel()
{
}

void StartTrace(uint64_t label, const string& value, float limit UNUSED_PARAM)
{
}

void StartTraceDebug(bool isDebug, uint64_t label, const string& value, float limit UNUSED_PARAM)
{
}

void StartTraceArgs(uint64_t label, const char *fmt, ...)
{
}

void StartTraceArgsDebug(bool isDebug, uint64_t label, const char *fmt, ...)
{
}

void FinishTrace(uint64_t label)
{
}

void FinishTraceDebug(bool isDebug, uint64_t label)
{
}

void StartAsyncTrace(uint64_t label, const string& value, int32_t taskId, float limit UNUSED_PARAM)
{
}

void StartAsyncTraceDebug(bool isDebug, uint64_t label, const string& value, int32_t taskId, float limit UNUSED_PARAM)
{
}

void StartAsyncTraceArgs(uint64_t label, int32_t taskId, const char *fmt, ...)
{
}

void StartAsyncTraceArgsDebug(bool isDebug, uint64_t label, int32_t taskId, const char *fmt, ...)
{
}

void FinishAsyncTrace(uint64_t label, const string& value, int32_t taskId)
{
}

void FinishAsyncTraceDebug(bool isDebug, uint64_t label, const string& value, int32_t taskId)
{
}

void FinishAsyncTraceArgs(uint64_t label, int32_t taskId, const char *fmt, ...)
{
}

void FinishAsyncTraceArgsDebug(bool isDebug, uint64_t label, int32_t taskId, const char *fmt, ...)
{
}

void MiddleTrace(uint64_t label, const string& beforeValue UNUSED_PARAM, const std::string& afterValue)
{
}

void MiddleTraceDebug(bool isDebug, uint64_t label, const string& beforeValue UNUSED_PARAM, 
                        const std::string& afterValue)
{
}

void CountTrace(uint64_t label, const string& name, int64_t count)
{
}

void CountTraceDebug(bool isDebug, uint64_t label, const string& name, int64_t count)
{
}

HitraceMeterFmtScoped::HitraceMeterFmtScoped(uint64_t label, const char *fmt, ...) : mTag(label)
{
}
