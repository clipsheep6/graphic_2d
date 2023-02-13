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

#include <chrono>
#include <cinttypes>
#include <list>
#include <minwindef.h>
#include <psapi.h>
#include <sstream>
#include <windows.h>

#include "hilog/log.h"

using namespace std;
using namespace OHOS::HiviewDFX;

namespace {
using MarkerType = uint64_t;
#define UNUSED_PARAM
constexpr int VAR_NAME_MAX_SIZE = 256;
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HITRACE_TAG, "HitraceMeter"};
} // namespace

class TraceContent {
public:
    static TraceContent &GetInstance()
    {
        static TraceContent traceContent;
        return traceContent;
    }

    TraceContent()
    {
        char processName[MAX_PATH] = "Previewer.exe";
        GetModuleBaseName(GetCurrentProcess(), nullptr, processName, sizeof(processName));
        processName_ = processName;

        auto filename = processName_ + ".trace";
        fp_ = fopen(filename.c_str(), "w");
        if (fp_ == nullptr) {
            return;
        }

        fprintf(fp_, "# tracer: nop\n");
    }

    ~TraceContent()
    {
        if (fp_ != nullptr) {
            fclose(fp_);
        }
    }

    void AppendContent(char action, const std::string &content)
    {
        if (fp_ == nullptr) {
            return;
        }

        uint64_t nowUs = std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::steady_clock::now().time_since_epoch()).count();
        uint32_t pid = GetCurrentProcessId();
        uint32_t tid = GetCurrentThreadId();
        auto seconds = nowUs / 1000000;
        uint32_t ms = nowUs % 1000000;
        fprintf(fp_, "%s-%u (%u) [000] .... %" PRIu64 ".%u: tracing_mark_write: %c|%u|%s\n",
                processName_.c_str(), tid, tid, seconds, ms, action, pid, content.c_str());
    }

private:
    FILE *fp_ = nullptr;
    std::string processName_ = "Previewer.exe";
};

void AddHitraceMeterMarker(MarkerType type, uint64_t tag, const std::string& name, const std::string& value)
{
}

void UpdateTraceLabel()
{
}

void StartTrace(uint64_t label, const string& value, float limit UNUSED_PARAM)
{
    TraceContent::GetInstance().AppendContent('B', value);
}

void StartTraceDebug(bool isDebug, uint64_t label, const string& value, float limit UNUSED_PARAM)
{
    TraceContent::GetInstance().AppendContent('B', value);
}

void StartTraceArgs(uint64_t label, const char *fmt, ...)
{
    char name[VAR_NAME_MAX_SIZE] = {};
    va_list args;

    va_start(args, fmt);
    int ret = vsnprintf_s(name, sizeof(name), sizeof(name) - 1, fmt, args);
    va_end(args);
    if (ret < 0) {
        HiLog::Error(LABEL, "vsnprintf_s failed: %{public}d", errno);
        return;
    }
    StartTrace(label, name, -1);
}

void StartTraceArgsDebug(bool isDebug, uint64_t label, const char *fmt, ...)
{
    char name[VAR_NAME_MAX_SIZE] = {};
    va_list args;

    va_start(args, fmt);
    int ret = vsnprintf_s(name, sizeof(name), sizeof(name) - 1, fmt, args);
    va_end(args);
    if (ret < 0) {
        HiLog::Error(LABEL, "vsnprintf_s failed: %{public}d", errno);
        return;
    }
    StartTrace(label, name, -1);
}

void FinishTrace(uint64_t label)
{
    TraceContent::GetInstance().AppendContent('E', "");
}

void FinishTraceDebug(bool isDebug, uint64_t label)
{
    TraceContent::GetInstance().AppendContent('E', "");
}

void StartAsyncTrace(uint64_t label, const string& value, int32_t taskId, float limit UNUSED_PARAM)
{
    TraceContent::GetInstance().AppendContent('S', value + "|" + std::to_string(taskId));
}

void StartAsyncTraceDebug(bool isDebug, uint64_t label, const string& value, int32_t taskId, float limit UNUSED_PARAM)
{
    TraceContent::GetInstance().AppendContent('S', value + "|" + std::to_string(taskId));
}

void StartAsyncTraceArgs(uint64_t label, int32_t taskId, const char *fmt, ...)
{
    char name[VAR_NAME_MAX_SIZE] = {};
    va_list args;

    va_start(args, fmt);
    int ret = vsnprintf_s(name, sizeof(name), sizeof(name) - 1, fmt, args);
    va_end(args);
    if (ret < 0) {
        HiLog::Error(LABEL, "vsnprintf_s failed: %{public}d", errno);
        return;
    }
    StartAsyncTrace(label, name, taskId, -1);
}

void StartAsyncTraceArgsDebug(bool isDebug, uint64_t label, int32_t taskId, const char *fmt, ...)
{
    char name[VAR_NAME_MAX_SIZE] = {};
    va_list args;

    va_start(args, fmt);
    int ret = vsnprintf_s(name, sizeof(name), sizeof(name) - 1, fmt, args);
    va_end(args);
    if (ret < 0) {
        HiLog::Error(LABEL, "vsnprintf_s failed: %{public}d", errno);
        return;
    }
    StartAsyncTrace(label, name, taskId, -1);
}

void FinishAsyncTrace(uint64_t label, const string& value, int32_t taskId)
{
    TraceContent::GetInstance().AppendContent('F', value + "|" + std::to_string(taskId));
}

void FinishAsyncTraceDebug(bool isDebug, uint64_t label, const string& value, int32_t taskId)
{
    TraceContent::GetInstance().AppendContent('F', value + "|" + std::to_string(taskId));
}

void FinishAsyncTraceArgs(uint64_t label, int32_t taskId, const char *fmt, ...)
{
    char name[VAR_NAME_MAX_SIZE] = {};
    va_list args;

    va_start(args, fmt);
    int ret = vsnprintf_s(name, sizeof(name), sizeof(name) - 1, fmt, args);
    va_end(args);
    if (ret < 0) {
        HiLog::Error(LABEL, "vsnprintf_s failed: %{public}d", errno);
        return;
    }
    FinishAsyncTrace(label, name, taskId);
}

void FinishAsyncTraceArgsDebug(bool isDebug, uint64_t label, int32_t taskId, const char *fmt, ...)
{
    char name[VAR_NAME_MAX_SIZE] = {};
    va_list args;

    va_start(args, fmt);
    int ret = vsnprintf_s(name, sizeof(name), sizeof(name) - 1, fmt, args);
    va_end(args);
    if (ret < 0) {
        HiLog::Error(LABEL, "vsnprintf_s failed: %{public}d", errno);
        return;
    }
    FinishAsyncTrace(label, name, taskId);
}

void MiddleTrace(uint64_t label, const string& beforeValue UNUSED_PARAM, const std::string& afterValue)
{
    TraceContent::GetInstance().AppendContent('E', "");
    TraceContent::GetInstance().AppendContent('B', afterValue);
}

void MiddleTraceDebug(bool isDebug, uint64_t label, const string& beforeValue UNUSED_PARAM, 
                        const std::string& afterValue)
{
    TraceContent::GetInstance().AppendContent('E', "");
    TraceContent::GetInstance().AppendContent('B', afterValue);
}

void CountTrace(uint64_t label, const string& name, int64_t count)
{
    TraceContent::GetInstance().AppendContent('C', name + "|" + std::to_string(count));
}

void CountTraceDebug(bool isDebug, uint64_t label, const string& name, int64_t count)
{
    TraceContent::GetInstance().AppendContent('C', name + "|" + std::to_string(count));
}

HitraceMeterFmtScoped::HitraceMeterFmtScoped(uint64_t label, const char *fmt, ...) : mTag(label)
{
}
