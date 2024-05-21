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

#ifndef TRACE3D_LOADER_HELPER_H
#define TRACE3D_LOADER_HELPER_H

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
#if defined(TRACE3D_LOADER_OHOS)
#include <hilog/log.h>
#endif

#if defined(TRACE3D_LOADER_OHOS)
#define TRACE3D_LOADER_API __attribute__((visibility("default"), used))
#else
#error "Unknown platform"
#endif

#undef LOG_DOMAIN
// The "0xD001405" is the domain ID for graphic module that alloted by the OS.
#define LOG_DOMAIN 0xD001405

#define LOADER_TAG "TRACE3DLOADER"

#define TRACE3D_LOGI(__format, ...) HiLogPrint(LOG_APP, LOG_INFO, LOG_DOMAIN, LOADER_TAG, __format, __VA_ARGS__)
#define TRACE3D_LOGW(__format, ...) HiLogPrint(LOG_APP, LOG_WARN, LOG_DOMAIN, LOADER_TAG, __format, __VA_ARGS__)
#define TRACE3D_LOGE(__format, ...) HiLogPrint(LOG_APP, LOG_ERROR, LOG_DOMAIN, LOADER_TAG, __format, __VA_ARGS__)
#define TRACE3D_LOGD(__format, ...) HiLogPrint(LOG_APP, LOG_DEBUG, LOG_DOMAIN, LOADER_TAG, __format, __VA_ARGS__)

namespace trace3d {

size_t GetFileSize(const char *fileName);

} // namespace trace3d

#endif // TRACE3D_LOADER_HELPER_H
