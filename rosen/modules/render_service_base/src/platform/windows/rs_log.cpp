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

#include "platform/common/rs_log.h"

#include <chrono>
#include <cstdarg>
#include <cstdio>
#include <ctime>
#include <string>
#include <thread>

#include <securec.h>
#include <securec_p.h>

namespace OHOS {
namespace Rosen {
namespace {
constexpr uint32_t MAX_TIME_SIZE = 32;
constexpr uint32_t MAX_BUFFER_SIZE = 4000;
} // namespace

std::string GetTimeStamp()
{
    time_t tt = time(nullptr);
    tm* t = localtime(&tt);
    char time[MAX_TIME_SIZE];

    int64_t nowMs = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();

    if (sprintf_s(time, MAX_TIME_SIZE, " %02d/%02d %02d:%02d:%02d.%03d", t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min,
            t->tm_sec, nowMs % 1000) < 0) {
        return std::string();
    }
    return std::string(time);
}

int RSLog::Output(RSLog::Level level, const char *func, int line, const char* format, ...)
{
    std::string levelStr;
    switch (level) {
        case LEVEL_INFO:
            levelStr = "I";
            break;
        case LEVEL_DEBUG:
            levelStr = "D";
            break;
        case LEVEL_WARN:
            levelStr = "W";
            break;
        case LEVEL_ERROR:
            levelStr = "E";
            break;
        case LEVEL_FATAL:
            levelStr = "F";
            break;
        default:
            break;
    }
    std::string newFmt(format);

    char buf[MAX_BUFFER_SIZE];
    va_list ap;
    va_start(ap, format);
    if (vsnprintfp_s(buf, sizeof(buf), sizeof(buf) - 1, false, newFmt.c_str(), ap) < 0 && errno == EINVAL) {
        va_end(ap);
        return EINVAL;
    }
    va_end(ap);

    std::string newTimeFmt("[%-3s %s] %s %-6d [%s](%s)(%d)");
    char timeBuf[MAX_BUFFER_SIZE];
    if (_snprintf_s(timeBuf, sizeof(timeBuf), sizeof(timeBuf) - 1, newTimeFmt.c_str(),
            " RS", levelStr.c_str(), GetTimeStamp().c_str(),
            std::this_thread::get_id(), tag_.c_str(), func, line) < 0) {
        return 1;
    }
    printf("%s %s\n", timeBuf, buf);
    fflush(stdout);
    return 0;
}
} // namespace Rosen
} // namespace OHOS
