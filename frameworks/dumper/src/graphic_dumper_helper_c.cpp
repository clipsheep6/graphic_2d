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

#include <graphic_dumper_helper_c.h>

#include <cstdarg>
#include <mutex>

#include <securec.h>

#include "graphic_dumper_helper_impl.h"

namespace OHOS {
const int INFO_SIZE_MAX = 4096;
bool printInfo = true;
void ConfigChangeFuncPtr(const std::string &key, const std::string &val)
{
    if (key.find("info") != std::string::npos) {
        if (val.compare("true") == 0) {
            printInfo = true;
        } else {
            printInfo = false;
        }
    }
}
int SendInfo(const char* tag, const char *fmt, ...)
{
    static std::mutex mutex;
    std::lock_guard<std::mutex> lock(mutex);
    static char info[INFO_SIZE_MAX];
    va_list args;
    va_start(args, fmt);
    int ret = vsnprintf_s(info, sizeof(info), (sizeof(info) - 1), fmt, args);
    if (ret < 0) {
        return 0;
    }
    va_end(args);
    return ::OHOS::GraphicDumperHelperImpl::GetInstance()->SendInfo(std::string(tag), "%s", info);
}
int AddConfigChangeListener(const char* tag, OnConfigChangeFuncPtr func)
{
    return ::OHOS::GraphicDumperHelperImpl::GetInstance()->
    AddConfigChangeListener(std::string(tag), ConfigChangeFuncPtr);
}
int RemoveConfigChangeListener(const int listenerId)
{
    return ::OHOS::GraphicDumperHelperImpl::GetInstance()->RemoveConfigChangeListener(listenerId);
}
int AddDumpListener(const char* tag, OnDumpFuncPtr func)
{
    return ::OHOS::GraphicDumperHelperImpl::GetInstance()->AddDumpListener(std::string(tag), func);
}
int RemoveDumpListener(const int listenerId)
{
    return ::OHOS::GraphicDumperHelperImpl::GetInstance()->RemoveDumpListener(listenerId);
}
} // namespace OHOS
