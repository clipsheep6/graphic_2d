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
#include "memory/rs_memory_graphic.h"
#include "memory/rs_dfx_string.h"
#include <cstdarg>
#include <cstdio>
#include <securec.h>

namespace OHOS {
namespace Rosen {
std::string DfxString::AppendFormatInner(const char* fmt, va_list args)
    {
        int n;
        std::string result;
        va_list tmp_args;
        char buf[STRING_BUF_SIZE] = {0};
        va_copy(tmp_args, args);
        n = vsnprintf_s(buf, sizeof(buf), sizeof(buf) - 1, fmt, tmp_args);
        va_end(tmp_args);
        if (n < 0) {
            result.append("dump info length > 4096, error");
        } else {
            result.append(buf, n);
        }
        return result;
    }
}
}