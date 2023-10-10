/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.. All rights reserved.
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

#ifndef SKIA_GRAPHICS_H
#define SKIA_GRAPHICS_H

#include "include/core/SkGraphics.h"
#include "include/core/SkTraceMemoryDump.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class __attribute__((visibility("default"))) SkiaGraphics {
public:
    static size_t GetFontCacheUsed();
    static int GetFontCacheCountUsed();
    static void DumpMemoryStatistics(SkTraceMemoryDump* dump);
    static size_t GetResourceCacheTotalByteLimit();
    static size_t GetFontCacheLimit();
};
}
}
}
#endif