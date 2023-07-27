/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include "sandbox_utils.h"
#include <cerrno>
#include <cstdlib>
#include <cstring>

#ifdef _WIN32
#include <windows.h>
#endif

namespace OHOS {
#if !defined(OHOS_LITE) && !defined(_WIN32) && !defined(__APPLE__) && !defined(__gnu_linux__)
const int PID_STR_SIZE = 5;
const int STATUS_LINE_SIZE = 1024;

static int FindAndConvertPid(char *buf)
{
    int count = 0;
    char pidBuf[11] = {0}; /* 11: 32 bits to the maximum length of a string */
    char *str = buf;
    while (*str != '\0') {
        if ((*str >= '0') && (*str <= '9')) {
            pidBuf[count] = *str;
            count++;
            str++;
            continue;
        }

        if (count > 0) {
            break;
        }
        str++;
    }
    return atoi(pidBuf);
}

class RealPidGetter {
    pid_t pid;
    int error;

public:
    RealPidGetter()
    {
        bool found = false;
        const char *path = "/proc/self/status";
        char buf[STATUS_LINE_SIZE] = {0};
        FILE *fp = fopen(path, "r");
        if (fp == nullptr) {
            pid = -1;
            error = errno;
            return;
        }

        while (!feof(fp)) {
            if (fgets(buf, STATUS_LINE_SIZE, fp) == nullptr) {
                pid = -1;
                error = errno;
                fclose(fp);
                return;
            }
            if (strncmp(buf, "Tgid:", PID_STR_SIZE) == 0) {
                found = true;
                break;
            }
        }
        (void)fclose(fp);

        if (found) {
            pid = static_cast<pid_t>(FindAndConvertPid(buf));
            error = 0;
        } else {
            pid = -1;
            error = EINVAL;
        }
    }

    pid_t getpid()
    {
        if (pid < 0) {
            errno = error;
        }
        return pid;
    }
};
#endif

pid_t GetRealPid(void)
{
#ifdef _WIN32
    return GetCurrentProcessId();
#elif defined(OHOS_LITE) || defined(__APPLE__) || defined(__gnu_linux__)
    return getpid();
#else
    static RealPidGetter getter;
    return getter.getpid();
#endif
}
} // namespace OHOS
