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

#include "rs_profiler_utils.h"

#include <chrono>
#include <fcntl.h>
#include <filesystem>
#include <sstream>
#include <unistd.h>

#ifndef REPLAY_TOOL_CLIENT
#include "rs_profiler.h"

#include "platform/common/rs_log.h"
#endif

namespace OHOS::Rosen::Utils {

#ifndef REPLAY_TOOL_CLIENT

uint64_t RawNowNano()
{
    return static_cast<uint64_t>(
        std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch())
            .count());
}

uint64_t NowNano()
{
    return RSProfiler::PatchTime(RawNowNano());
}

#else

uint64_t RawNowNano()
{
    return 0;
}

uint64_t NowNano()
{
    return 0;
}

#endif

double Now()
{
    constexpr int nanosecInSec = 1'000'000'000;
    return (static_cast<double>(NowNano())) / nanosecInSec;
}

std::vector<std::string> Split(const std::string& input)
{
    std::istringstream iss(input);
    std::vector<std::string> parts { std::istream_iterator<std::string> { iss },
        std::istream_iterator<std::string> {} };
    return parts;
}

FILE* FileOpen(const std::string& path, const std::string& openOptions)
{
    const std::filesystem::path canonicalPath = std::filesystem::weakly_canonical(std::filesystem::path(path));
    auto file = fopen(canonicalPath.string().c_str(), openOptions.c_str());
    if (!IsFileValid(file)) {
        RS_LOGE("Cant open file '%s'!", path.c_str()); // NOLINT
    }
    return file;
}

void FileClose(FILE* file)
{
    if (fclose(file) != 0) {
        RS_LOGE("File close failed"); // NOLINT
    }
}

bool IsFileValid(FILE* file)
{
    return file;
}

size_t FileSize(FILE* file)
{
    if (!IsFileValid(file)) {
        return 0;
    }

    const int position = ftell(file);
    FileSeek(file, 0, SEEK_END);
    const size_t size = ftell(file);
    FileSeek(file, position, SEEK_SET);
    return size;
}

void FileSeek(FILE* file, int64_t offset, int origin)
{
    if (fseek(file, offset, origin) != 0) {
        RS_LOGE("Failed fseek in file"); // NOLINT
    }
}

void FileRead(FILE* file, void* data, size_t size)
{
    if (fread(data, size, 1, file) < 1) {
        RS_LOGE("Error while reading from file"); // NOLINT
    }
}

void FileWrite(FILE* file, const void* data, size_t size)
{
    if (fwrite(data, size, 1, file) < 1) {
        RS_LOGE("Error while writing to file"); // NOLINT
    }
}

// deprecated
void FileRead(void* data, size_t size, size_t count, FILE* file)
{
    if (fread(data, size, count, file) < count) {
        RS_LOGE("Error while reading from file"); // NOLINT
    }
}

// deprecated
void FileWrite(const void* data, size_t size, size_t count, FILE* file)
{
    if (fwrite(data, size, count, file) < count) {
        RS_LOGE("Error while writing to file"); // NOLINT
    }
}

} // namespace OHOS::Rosen::Utils