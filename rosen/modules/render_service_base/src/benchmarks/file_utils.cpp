/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "benchmarks/file_util.h"
#include <sstream>
#include <string>
#include <fstream>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <linux/limits.h>
#include "securec.h"

#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {

bool GetRealPath(const std::string& path, std::string& realPath)
{
    char resolvedPath[PATH_MAX] = { 0 };
    if (path.size() > PATH_MAX || !realpath(path.c_str(), resolvedPath)) {
        RS_LOGE("%{public}s realpath for %s failed", __func__, path.c_str());
        return false;
    }
    realPath = std::string(resolvedPath);
    return true;
}

bool WriteToFile(uintptr_t data, size_t size, const std::string& filePath)
{
    std::string realPath;
    if (!GetRealPath(filePath, realPath) || access(realPath.c_str(), W_OK)) {
        return false;
    }
    int fd = open(realPath.c_str(), O_WRONLY | O_CLOEXEC);
    if (fd < 0) {
        RS_LOGE("%{public}s failed. file: %s, fd = %{public}d", __func__, realPath.c_str(), fd);
        return false;
    }
    ssize_t nwrite = write(fd, reinterpret_cast<uint8_t *>(data), size);
    if (nwrite < 0) {
        RS_LOGE("%{public}s failed to persist data: %p, size = %d,  fd = %{public}d", __func__, data, size, fd);
    }
    close(fd);
    return true;
}
} // namespace Rosen
} // namespace OHOS