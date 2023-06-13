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

#include "memory_handler.h"
#include "utils/log.h"
#include <set>
#include <cmath>

namespace OHOS {
namespace Rosen {
#ifndef USE_ROSEN_DRAWING
void MemoryHandler::ConfigureContext(GrContextOptions* context, const char* identity,
    const size_t size, const std::string& cacheFilePath, bool isUni)
{
    context->fAllowPathMaskCaching = true;
    auto &cache = ShaderCache::Instance();
    cache.SetFilePath(cacheFilePath);
    cache.InitShaderCache(identity, size, isUni);
    context->fPersistentCache = &cache;
}
#else
void MemoryHandler::ConfigureContext(Drawing::GPUContextOptions* context, const char* identity,
    const size_t size, const std::string& cacheFilePath, bool isUni)
{
    auto& cache = ShaderCache::Instance();
    cache.SetFilePath(cacheFilePath);
    cache.InitShaderCache(identity, size, isUni);
    context->SetPersistentCache(&cache);
}
#endif

std::string MemoryHandler::QuerryShader()
{
    const auto& cache = ShaderCache::Instance();
    if (!cache.IfInitialized()) {
        LOGW("ShaderCache is not intialized.");
    }
    size_t shaderRam = cache.QuerryShaderSize();
    size_t shaderNum = cache.QuerryShaderNum();
    std::string ramString = "ShaderCache RAM used:" + std::to_string(shaderRam / 1024) + " kB\n";
    ramString += "ShaderCache num is: " + std::to_string(shaderNum);
    return ramString;
}

std::string MemoryHandler::ClearShader()
{
    const auto& cache = ShaderCache::Instance();
    LOGW("All shaders are cleaned");
    size_t cleanedRam = cache.CleanAllShaders();
    std::string ramString = "All shaders are cleaned, RAM freed: " + std::to_string(cleanedRam);
    return ramString;
}
}   // namespace Rosen
}   // namespace OHOS