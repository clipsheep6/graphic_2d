/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "trace3d_loader_helper.h"

namespace trace3d {

void LogOutput(LOG_LEVEL level, const char *format, ...) {}

int FormatString(std::string &outStr, const char *format, va_list ap)
{
    return 0;
}

int FormatString(std::string &outStr, const char *format, ...)
{
    return 0;
}


size_t GetFileSize(const char *fileName)
{
    return 0;
}

size_t ReadFileData(const char *fileName, std::vector<uint8_t> &blob)
{
    return 0;
}

size_t WriteFileData(const char *fileName, const std::vector<uint8_t> &blob)
{
    return 0;
}

size_t StoreSharedLibrary(const char *libName, const std::vector<uint8_t> &blob)
{
    return 0;
}

void *DlopenSharedLibrary(const char *libFullName)
{
    return nullptr;
}

void *DlopenStoredSharedLibrary(const char *libName)
{
    return nullptr;
}

size_t TestStoredSharedLibrary(const char *libName)
{
    return 0;
}

void *DlopenBundledSharedLibrary(const char *libName)
{
    return nullptr;
}

size_t TestBundledSharedLibrary(const char *libName)
{
    return 0;
}

} // namespace trace3d
