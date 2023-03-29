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

#include "memory/MemoryManager.h"

#include <malloc.h>
#include <SkGraphics.h>

#include "SkiaMemoryTracer.h"
#include "include/gpu/GrContext.h"
#include "src/gpu/GrContextPriv.h"

#include "common/rs_obj_abs_geometry.h"
#include "memory/rs_tag_tracker.h"
#include "pipeline/rs_main_thread.h"
#include "platform/common/rs_log.h"

namespace OHOS::Rosen {
namespace {
constexpr uint32_t MEMUNIT_RATE = 1024;
constexpr const char* MEM_RS_TYPE = "renderservice";
constexpr const char* MEM_CPU_TYPE = "cpu";
constexpr const char* MEM_GPU_TYPE = "gpu";
constexpr const char* MEM_JEMALLOC_TYPE = "jemalloc";
}
static std::unordered_map<NodeId, std::string> trashMap;

void MemoryManager::DumpMemoryUsage(DfxString& log, const GrContext* grContext, std::string& type)
{
    if (type.empty() || type == MEM_RS_TYPE) {
        DumpRenderServiceMemory(log);
    }
    if (type.empty() || type == MEM_CPU_TYPE) {
        DumpDrawingCpuMemory(log);
    }
    if (type.empty() || type == MEM_GPU_TYPE) {
        std::string gpuInfo;
        DumpDrawingGpuMemory(log, grContext, gpuInfo);
    }
    if (type.empty() || type == MEM_JEMALLOC_TYPE) {
        std::string out;
        DumpMallocStat(out);
        log.AppendFormat("%s\n... detail dump at hilog\n", out.c_str());
    }
}

void MemoryManager::DumpPidMemory(DfxString& log, int pid)
{
    MemoryTrack::Instance().DumpMemoryStatistics(log, pid);
}

void MemoryManager::ReleaseGpuResByTag(GrContext* grContext, GrGpuResourceTag& tag)
{
#ifdef RS_ENABLE_GL    
    if(!grContext) {
        RS_LOGE("ReleaseGpuResByTag fail, grContext is nullptr");
    }
    grContext->releaseByTag(tag);
#endif
}

MemoryGraphic MemoryManager::CountPidMemory(int pid, const GrContext* grContext)
{
    MemoryGraphic totalMemGraphic;

    // Count mem of RS
    totalMemGraphic.SetPid(pid);
    MemoryGraphic rsMemGraphic = MemoryTrack::Instance().CountRSMemory(pid);
    totalMemGraphic += rsMemGraphic;

#ifdef RS_ENABLE_GL
    // Count mem of Skia GPU
    if (grContext) {
        SkiaMemoryTracer gpuTracer("category", true);
        GrGpuResourceTag tag(pid, 0, 0, 0);
        grContext->dumpMemoryStatisticsByTag(&gpuTracer, tag);
        float gpuMem = gpuTracer.GetGLMemorySize();
        totalMemGraphic.IncreaseGpuMemory(gpuMem);
    }
#endif

    return totalMemGraphic;
}

void MemoryManager::CountMemory(std::vector<pid_t> pids, const GrContext* grContext, std::vector<MemoryGraphic>& mems)
{
    auto countMem = [&grContext, &mems] (pid_t pid) {
       mems.emplace_back(CountPidMemory(pid, grContext));
    };
    // Count mem of Skia GPU
    std::for_each(pids.begin(), pids.end(), countMem);
}

static std::tuple<uint64_t, std::string, RectI> FindGeoById(uint64_t nodeId)
{
    const auto& nodeMap = RSMainThread::Instance()->GetContext().GetNodeMap();
    auto node = nodeMap.GetRenderNode<RSRenderNode>(nodeId);
    uint64_t wId = nodeId;
    std::string wName = "NONE";
    RectI wframe;
    if (!node) {
        return { wId, wName, wframe };
    }
    wframe = std::static_pointer_cast<RSObjAbsGeometry>(node->GetRenderProperties().GetBoundsGeometry())->GetAbsRect();
    // Obtain the window according to childId
    auto parent = node->GetParent().lock();
    while(parent) {
        if (parent->IsInstanceOf<RSSurfaceRenderNode>()) {
            const auto& surfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(parent);
            wName = surfaceNode->GetName();
            break;
        }
        parent = parent->GetParent().lock();
    }
    return { wId, wName, wframe };
}

void MemoryManager::DumpRenderServiceMemory(DfxString& log)
{
    log.AppendFormat("\n----------\nRenderService caches:\n");
    MemoryTrack::Instance().DumpMemoryStatistics(log, FindGeoById);
}

void MemoryManager::DumpDrawingCpuMemory(DfxString& log)
{
    // CPU
    log.AppendFormat("\n----------\nSkia CPU caches:\n");
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

    // cache limit
    size_t cacheLimit = SkGraphics::GetResourceCacheTotalByteLimit();
    size_t fontCacheLimit = SkGraphics::GetFontCacheLimit();
    log.AppendFormat("\ncpu cache limit = %zu ( fontcache = %zu ):\n", cacheLimit, fontCacheLimit);
}

void MemoryManager::DumpGpuCache(DfxString& log, const GrContext* grContext, GrGpuResourceTag* tag, std::string& name)
{
    if (!grContext) {
        log.AppendFormat("grContext is nullptr.\n");
        return;
    }
#ifdef RS_ENABLE_GL
    log.AppendFormat("\n---------------\nSkia GPU Caches:%s\n", name.c_str());
    SkiaMemoryTracer gpuTracer("category", true);
    if (tag) {
        grContext->dumpMemoryStatisticsByTag(&gpuTracer, *tag);
    } else {
        grContext->dumpMemoryStatistics(&gpuTracer);
    }
    gpuTracer.LogOutput(log);
    log.AppendFormat("Total GPU memory usage:\n");
    gpuTracer.LogTotals(log);
#endif
}

void MemoryManager::DumpAllWindow(DfxString& log, const GrContext* grContext)
{
    if (!grContext) {
        log.AppendFormat("No valid gpu cache instance.\n");
        return;
    }
#ifdef RS_ENABLE_GL
    const auto& nodeMap = RSMainThread::Instance()->GetContext().GetNodeMap();
    nodeMap.TraverseSurfaceNodes([&log, &grContext](const std::shared_ptr<RSSurfaceRenderNode> node) {
        GrGpuResourceTag tag(ExtractPid(node->GetId()), 0, node->GetId(), 0);
        std::string name = node->GetName() + " " + std::to_string(node->GetId());
        DumpGpuCache(log, grContext, &tag, name);
    });
#endif
}

 void MemoryManager::DumpTrashWindow(DfxString& log, const GrContext* grContext)
 {
    if (!grContext) {
        log.AppendFormat("No valid gpu cache instance.\n");
        return;
    }
#ifdef RS_ENABLE_GL
    for (auto& trash : trashMap) {
        GrGpuResourceTag tag(ExtractPid(trash.first), 0, trash.first, 0);
        std::string trashName = "*" + trash.second + " " + std::to_string(trash.first);
        DumpGpuCache(log, grContext, &tag, trashName);
    }
#endif
 }

void MemoryManager::CollectTrashInfo(pid_t pid)
{
    const auto& nodeMap = RSMainThread::Instance()->GetContext().GetNodeMap();
    nodeMap.TraverseSurfaceNodes([&pid](const std::shared_ptr<RSSurfaceRenderNode> node) {
        if (ExtractPid(node->GetId()) == pid) {
            trashMap.emplace(node->GetId(), node->GetName());
        }
    });
}

void MemoryManager::DumpDrawingGpuMemory(DfxString& log, const GrContext* grContext, std::string& tag)
{
    if (!grContext) {
        log.AppendFormat("No valid gpu cache instance.\n");
        return;
    }
    /////////////////////////////GPU/////////////////////////
#ifdef RS_ENABLE_GL
    // total
    DumpGpuCache(log, grContext, nullptr, tag);

    if (tag.empty()) {
        // Get memory of window by tag
        DumpAllWindow(log, grContext);

        // get memory of rs
        for (uint32_t tagtype = RSTagTracker::TAG_DRAW_SURFACENODE; tagtype <= RSTagTracker::TAG_CAPTURE; tagtype++) {
            GrGpuResourceTag tag(0, 0, 0, tagtype);
            std::string tagType = RSTagTracker::TagType2String(static_cast<RSTagTracker::TAGTYPE>(tagtype));
            DumpGpuCache(log, grContext, &tag, tagType);
        }

        // collect trash
        DumpTrashWindow(log, grContext);
    }

    // cache limit
    size_t cacheLimit = 0;
    size_t cacheUsed = 0;
    grContext->getResourceCacheLimits(nullptr, &cacheLimit);
    grContext->getResourceCacheUsage(nullptr, &cacheUsed);
    log.AppendFormat("\ngpu limit = %zu ( used = %zu ):\n", cacheLimit, cacheUsed);

    //////////////////////////ShaderCache///////////////////
    log.AppendFormat("\n---------------\nShader Caches:\n");
    std::shared_ptr<RenderContext> rendercontext = std::make_shared<RenderContext>();
    log.AppendFormat(rendercontext->GetShaderCacheSize().c_str());

    // gpu stat
    log.AppendFormat("\n---------------\ndumpGpuStats:\n");
    SkString stat;
    grContext->priv().dumpGpuStats(&stat);
    log.AppendFormat("%s\n", stat.c_str());
#endif
}

void MemoryManager::DumpMallocStat(std::string& log)
{
    malloc_stats_print([](void *fp, const char* str) {
        if (!fp) {
            RS_LOGE("DumpMallocStat fp is nullptr");
            return;
        }
        std::string* sp = static_cast<std::string*>(fp);
        if (str) {
            // cause log only support 2096 len. we need to only output critical log
            // and only put total log in RSLOG
            // get allocated string
            if (strncmp(str, "Allocated", strlen("Allocated")) == 0) {
                sp->append(str);
            }
            RS_LOGW("[mallocstat]:%s", str);
        }
    }, &log, nullptr);
}
} // namespace OHOS::Rosen