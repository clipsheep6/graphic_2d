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

#include "MemoryManager.h"

#include <malloc.h>
#include <SkGraphics.h>

#include "SkiaMemoryTracer.h"
#include "include/gpu/GrContext.h"

#include "pipeline/rs_main_thread.h"

namespace OHOS::Rosen {
constexpr uint32_t MEMUNIT_RATE = 1024;

void MemoryManager::DumpMemoryUsage(DfxString& log, const GrContext* grContext)
{
    //////////////////////////////CPU/////////////////////////
    log.AppendFormat("\n---------------\nSkia CPU Caches:\n");
    log.AppendFormat("Font Cache (CPU):\n");
    log.AppendFormat("  Size: %.2f kB \n", SkGraphics::GetFontCacheUsed() / MEMUNIT_RATE);
    log.AppendFormat("  Glyph Count: %d \n", SkGraphics::GetFontCacheCountUsed());

    std::vector<ResourcePair> cpuResourceMap = {
        { "skia/sk_resource_cache/bitmap_", "Bitmaps" },
        { "skia/sk_resource_cache/rrect-blur_", "Masks" },
        { "skia/sk_resource_cache/rects-blur_", "Masks" },
        { "skia/sk_resource_cache/tessellated", "Shadows" },
        { "skia/sk_resource_cache/yuv-planes_", "YUVPlanes" },
        { "skia/sk_resource_cache/budget_glyph_count", "Bitmaps" },
    };
    SkiaMemoryTracer cpuTracer(cpuResourceMap, true);
    SkGraphics::DumpMemoryStatistics(&cpuTracer);
    log.AppendFormat("CPU Cachesxx:\n");
    cpuTracer.LogOutput(log);
    log.AppendFormat("Total CPU memory usage:\n");
    cpuTracer.LogTotals(log);

    /////////////////////////////GPU/////////////////////////
#ifdef RS_ENABLE_GL
    log.AppendFormat("\n---------------\nSkia GPU Caches:\n");
    SkiaMemoryTracer gpuTracer("category", true);
    grContext->dumpMemoryStatistics(&gpuTracer);
    gpuTracer.LogOutput(log);
    log.AppendFormat("Total GPU memory usage:\n");
    gpuTracer.LogTotals(log);

    //////////////////////////ShaderCache///////////////////
    log.AppendFormat("\n---------------\nShader Caches:\n");
    std::shared_ptr<RenderContext> rendercontext = std::make_shared<RenderContext>();
    log.AppendFormat(rendercontext->GetShaderCacheSize().c_str());
#endif
}

void MemoryManager::DisableMallocCache(std::string& log)
{
#ifdef CONFIG_USE_JEMALLOC_DFX_INTF
    int ret1 = mallopt(M_SET_THREAD_CACHE, M_THREAD_CACHE_DISABLE);
    int ret2 = mallopt(M_DELAYED_FREE, M_DELAYED_FREE_DISABLE);
    log.append("disable: tcache and delay free, result = " + std::to_string(ret1) + ", " + std::to_string(ret2) + "\n");
#endif
}

void MemoryManager::EnableMallocCache(std::string& log)
{
#ifdef CONFIG_USE_JEMALLOC_DFX_INTF
    int ret1 = mallopt(M_SET_THREAD_CACHE, M_THREAD_CACHE_ENABLE);
    int ret2 = mallopt(M_DELAYED_FREE, M_DELAYED_FREE_ENABLE);
    log.append("enable: tcache and delay free, result = " + std::to_string(ret1) + ", " + std::to_string(ret2) + "\n");
#endif
}

} // namespace OHOS::Rosen