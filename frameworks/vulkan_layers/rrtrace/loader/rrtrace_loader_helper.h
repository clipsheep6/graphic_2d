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

#ifndef RRTRACE_LOADER_HELPER_H
#define RRTRACE_LOADER_HELPER_H

#include <cstdlib>
#include <string>
#include <map>
#include <cstdarg>

#include <dirent.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <securec.h>
#if defined(RRTRACE_LOADER_OHOS)
#include <hilog/log.h>
#endif

#if defined(RRTRACE_LOADER_OHOS)
#define RRTRACE_LOADER_API __attribute__((visibility("default"), used))
#else
#error "Unknown platform"
#endif

namespace rrtrace {

size_t GetFileSize(const char *fileName);

} // namespace rrtrace

#endif // RRTRACE_LOADER_HELPER_H
