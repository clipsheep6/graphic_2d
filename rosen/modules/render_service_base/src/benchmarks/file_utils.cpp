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

#include "benchmarks/file_utils.h"
#include <fcntl.h>
#include <unistd.h>
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
namespace Benchmarks {
bool IsValidFile(const std::string& realPathStr, const std::string& validFile)
{
    return realPathStr.find(validFile) == 0;
}

std::string GetRealPath(const std::string& filePath)
{
    std::string realPathStr = "";
    char actualPath[PATH_MAX + 1] = {0};
    if (realpath(filePath.c_str(), actualPath) == nullptr) {
        RS_LOGE("The file path is empty!");
        return realPathStr;
    }
    realPathStr = actualPath;
    if (IsValidFile(realPathStr)) {
        return realPathStr;
    } else {
        RS_LOGE("The file path is not valid!");
        return "";
    }
}

bool WriteToFile(uintptr_t data, size_t size, const std::string& filePath)
{
    std::string realDclFilePathStr = GetRealPath(filePath);
    if (realDclFilePathStr.empty()) {
        return false;
    }
    int fd = open(realDclFilePathStr.c_str(), O_RDWR | O_CREAT, static_cast<mode_t>(0600));
    if (fd < 0) {
        RS_LOGE("%{public}s failed. file: %s, fd = %{public}d", __func__, realDclFilePathStr.c_str(), fd);
        return false;
    }
    ssize_t nwrite = write(fd, reinterpret_cast<uint8_t *>(data), size);
    if (nwrite < 0) {
        RS_LOGE("%{public}s failed to persist data = %d, size = %d,  fd = %{public}d", __func__, data, size, fd);
    }
    close(fd);
    return true;
}

bool WriteStringToFile(int fd, const std::string& str)
{
    const char* p = str.data();
    ssize_t remaining = static_cast<ssize_t>(str.size());
    while (remaining > 0) {
        ssize_t n = write(fd, p, remaining);
        if (n == -1) {
            return false;
        }
        p += n;
        remaining -= n;
    }
    p = nullptr;
    return true;
}

bool WriteStringToFile(const std::string& str, const std::string& filePath)
{
    std::string realDclFilePathStr = GetRealPath(filePath);
    if (realDclFilePathStr.empty()) {
        return false;
    }
    int fd = open(realDclFilePathStr.c_str(), O_RDWR | O_CREAT, static_cast<mode_t>(0600));
    if (fd < 0) {
        RS_LOGE("%{public}s failed. file: %s, fd = %{public}d", __func__, realDclFilePathStr.c_str(), fd);
        return false;
    }
    bool result = WriteStringToFile(fd, str);
    close(fd);
    return result;
}
}
} // namespace Rosen
} // namespace OHOS