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
#ifndef DFXSTRING_H
#define DFXSTRING_H
#include <stdarg.h>
#include <string>

namespace OHOS::Rosen {
constexpr int STRING_BUF_SIZE = 4096;
class DfxString {
public:
    DfxString() : str_() {}

    DfxString(const char* c) : str_(c) {}

    ~DfxString() = default;

    void AppendFormat(const char* fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        str_.append(AppendFormatInner(fmt, args));
        va_end(args);
    }

    std::string GetString() const
    {
        return str_;
    }

private:
    std::string AppendFormatInner(const char* fmt, va_list args);
    std::string str_;
};
} // namespace OHOS::Rosen
#endif