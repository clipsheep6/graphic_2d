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

#include "memory/rs_memory_manager.h"

#include <malloc.h>
#include "include/core/SkGraphics.h"
#include "rs_trace.h"

#include "memory/rs_dfx_string.h"
#include "skia_adapter/rs_skia_memory_tracer.h"
#include "skia_adapter/skia_graphics.h"
#include "memory/rs_memory_graphic.h"
#include "include/gpu/GrDirectContext.h"
#include "src/gpu/GrDirectContextPriv.h"

#include "common/rs_obj_abs_geometry.h"
#include "memory/rs_tag_tracker.h"
#ifdef NEW_RENDER_CONTEXT
#include "render_context/memory_handler.h"
#endif
#include "pipeline/rs_main_thread.h"
#include "pipeline/rs_surface_render_node.h"
#include "platform/common/rs_log.h"
#include "pipeline/parallel_render/rs_sub_thread_manager.h"

#ifdef USE_ROSEN_DRAWING
#include "image/gpu_context.h"
#endif

#ifdef RS_ENABLE_VK
#include "pipeline/rs_vk_image_manager.h"
#include "platform/ohos/backend/rs_vulkan_context.h"
#endif

namespace OHOS::Rosen {
namespace {
constexpr uint32_t MEMUNIT_RATE = 1024;
constexpr const char* MEM_RS_TYPE = "renderservice";
constexpr const char* MEM_CPU_TYPE = "cpu";
constexpr const char* MEM_GPU_TYPE = "gpu";
constexpr const char* MEM_JEMALLOC_TYPE = "jemalloc";
}

#ifndef USE_ROSEN_DRAWING
void MemoryManager::DumpMemoryUsage(DfxString& log, const GrDirectContext* grContext, std::string& type)
{
    if (type.empty() || type == MEM_RS_TYPE) {
        DumpRenderServiceMemory(log);
    }
    if (type.empty() || type == MEM_CPU_TYPE) {
        DumpDrawingCpuMemory(log);
    }
    if (type.empty() || type == MEM_GPU_TYPE) {
        DumpDrawingGpuMemory(log, grContext);
    }
    if (type.empty() || type == MEM_JEMALLOC_TYPE) {
        std::string out;
        DumpMallocStat(out);
        log.AppendFormat("%s\n... detail dump at hilog\n", out.c_str());
    }
}
#else
void MemoryManager::DumpMemoryUsage(DfxString& log, const Drawing::GPUContext* gpuContext, std::string& type)
{
    if (type.empty() || type == MEM_RS_TYPE) {
        DumpRenderServiceMemory(log);
    }
    if (type.empty() || type == MEM_CPU_TYPE) {
        DumpDrawingCpuMemory(log);
    }
    if (type.empty() || type == MEM_GPU_TYPE) {
        DumpDrawingGpuMemory(log, gpuContext);
    }
    if (type.empty() || type == MEM_JEMALLOC_TYPE) {
        std::string out;
        DumpMallocStat(out);
        log.AppendFormat("%s\n... detail dump at hilog\n", out.c_str());
    }
}
#endif // USE_ROSEN_DRAWING

#ifndef USE_ROSEN_DRAWING
void MemoryManager::ReleaseAllGpuResource(GrDirectContext* grContext, GrGpuResourceTag& tag)
{
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    if (!grContext) {
        RS_LOGE("ReleaseGpuResByTag fail, grContext is nullptr");
        return;
    }
    RS_TRACE_NAME_FMT("ReleaseAllGpuResource [Pid:%d Tid:%d Nid:%d Funcid:%d]",
        tag.fPid, tag.fTid, tag.fWid, tag.fFid);
    grContext->releaseByTag(tag);
#endif
}
#else
void MemoryManager::ReleaseAllGpuResource(Drawing::GPUContext* gpuContext, Drawing::GPUResourceTag& tag)
{
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    if (!gpuContext) {
        RS_LOGE("ReleaseGpuResByTag fail, gpuContext is nullptr");
        return;
    }
    RS_TRACE_NAME_FMT("ReleaseAllGpuResource [Pid:%d Tid:%d Nid:%d Funcid:%d]",
        tag.fPid, tag.fTid, tag.fWid, tag.fFid);
    gpuContext->ReleaseByTag(tag);
#endif
}
#endif

#ifndef USE_ROSEN_DRAWING
void MemoryManager::ReleaseAllGpuResource(GrDirectContext* grContext, pid_t pid)
{
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    GrGpuResourceTag tag(pid, 0, 0, 0);
    ReleaseAllGpuResource(grContext, tag);
#endif
}
#else
void MemoryManager::ReleaseAllGpuResource(Drawing::GPUContext* gpuContext, pid_t pid)
{
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    Drawing::GPUResourceTag tag(pid, 0, 0, 0);
    ReleaseAllGpuResource(gpuContext, tag);
#endif
}
#endif

#ifndef USE_ROSEN_DRAWING
void MemoryManager::ReleaseUnlockGpuResource(GrDirectContext* grContext, GrGpuResourceTag& tag)
{
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    if (!grContext) {
        RS_LOGE("ReleaseGpuResByTag fail, grContext is nullptr");
        return;
    }
    RS_TRACE_NAME_FMT("ReleaseUnlockGpuResource [Pid:%d Tid:%d Nid:%d Funcid:%d]",
        tag.fPid, tag.fTid, tag.fWid, tag.fFid);
    grContext->purgeUnlockedResourcesByTag(false, tag);
#endif
}
#else
void MemoryManager::ReleaseUnlockGpuResource(Drawing::GPUContext* gpuContext, Drawing::GPUResourceTag& tag)
{
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    if (!gpuContext) {
        RS_LOGE("ReleaseGpuResByTag fail, gpuContext is nullptr");
        return;
    }
    RS_TRACE_NAME_FMT("ReleaseUnlockGpuResource [Pid:%d Tid:%d Nid:%d Funcid:%d]",
        tag.fPid, tag.fTid, tag.fWid, tag.fFid);
    gpuContext->PurgeUnlockedResourcesByTag(false, tag);
#endif
}
#endif

#ifndef USE_ROSEN_DRAWING
void MemoryManager::ReleaseUnlockGpuResource(GrDirectContext* grContext, NodeId surfaceNodeId)
{
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    GrGpuResourceTag tag(ExtractPid(surfaceNodeId), 0, 0, 0);
    ReleaseUnlockGpuResource(grContext, tag); // clear gpu resource by pid
#endif
}
#else
void MemoryManager::ReleaseUnlockGpuResource(Drawing::GPUContext* grContext, NodeId surfaceNodeId)
{
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    Drawing::GPUResourceTag tag(ExtractPid(surfaceNodeId), 0, 0, 0);
    ReleaseUnlockGpuResource(grContext, tag);
#endif
}
#endif

#ifndef USE_ROSEN_DRAWING
void MemoryManager::ReleaseUnlockGpuResource(GrDirectContext* grContext, pid_t pid)
{
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    GrGpuResourceTag tag(pid, 0, 0, 0);
    ReleaseUnlockGpuResource(grContext, tag); // clear gpu resource by pid
#endif
}
#else
void MemoryManager::ReleaseUnlockGpuResource(Drawing::GPUContext* grContext, pid_t pid)
{
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    Drawing::GPUResourceTag tag(pid, 0, 0, 0);
    ReleaseUnlockGpuResource(grContext, tag); // clear gpu resource by pid
#endif
}
#endif

#ifndef USE_ROSEN_DRAWING
void MemoryManager::ReleaseUnlockGpuResource(GrDirectContext* grContext, bool scratchResourcesOnly)
{
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    if (!grContext) {
        RS_LOGE("ReleaseGpuResByTag fail, grContext is nullptr");
        return;
    }
    RS_TRACE_NAME_FMT("ReleaseUnlockGpuResource scratchResourcesOnly:%d", scratchResourcesOnly);
    grContext->purgeUnlockedResources(scratchResourcesOnly);
#endif
}
#else
void MemoryManager::ReleaseUnlockGpuResource(Drawing::GPUContext* gpuContext, bool scratchResourcesOnly)
{
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    if (!gpuContext) {
        RS_LOGE("ReleaseGpuResByTag fail, gpuContext is nullptr");
        return;
    }
    RS_TRACE_NAME_FMT("ReleaseUnlockGpuResource scratchResourcesOnly:%d", scratchResourcesOnly);
    gpuContext->PurgeUnlockedResources(scratchResourcesOnly);
#endif
}
#endif

#ifndef USE_ROSEN_DRAWING
void MemoryManager::ReleaseUnlockAndSafeCacheGpuResource(GrDirectContext* grContext)
{
#if defined (RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    if (!grContext) {
        RS_LOGE("ReleaseUnlockAndSafeCacheGpuResource fail, grContext is nullptr");
        return;
    }
    RS_TRACE_NAME_FMT("ReleaseUnlockAndSafeCacheGpuResource");
    grContext->purgeUnlockAndSafeCacheGpuResources();
#endif
}
#else
void MemoryManager::ReleaseUnlockAndSafeCacheGpuResource(Drawing::GPUContext* gpuContext)
{
#if defined (RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    if (!gpuContext) {
        RS_LOGE("ReleaseUnlockAndSafeCacheGpuResource fail, gpuContext is nullptr");
        return;
    }
    RS_TRACE_NAME_FMT("ReleaseUnlockAndSafeCacheGpuResource");
    gpuContext->PurgeUnlockAndSafeCacheGpuResources();
#endif
}
#endif

#ifndef USE_ROSEN_DRAWING
float MemoryManager::GetAppGpuMemoryInMB(GrDirectContext* grContext)
{
    if (!grContext) {
        return 0.f;
    }
#if defined (RS_ENABLE_GL) || defined (RS_ENABLE_VK)
    SkiaMemoryTracer trace("category", true);
    grContext->dumpMemoryStatistics(&trace);
    auto total = trace.GetGpuMemorySizeInMB();
    float rsMemSize = 0.f;
    for (uint32_t tagtype = RSTagTracker::TAG_SAVELAYER_DRAW_NODE; tagtype <= RSTagTracker::TAG_CAPTURE; tagtype++) {
        GrGpuResourceTag resourceTag(0, 0, 0, tagtype);
        SkiaMemoryTracer gpuTrace("category", true);
        grContext->dumpMemoryStatisticsByTag(&gpuTrace, resourceTag);
        rsMemSize += gpuTrace.GetGpuMemorySizeInMB();
    }
    return total - rsMemSize;
#else
    return 0.f;
#endif
}
#else
float MemoryManager::GetAppGpuMemoryInMB(Drawing::GPUContext* gpuContext)
{
    if (!gpuContext) {
        return 0.f;
    }
#if defined (RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    Drawing::TraceMemoryDump trace("category", true);
    gpuContext->DumpMemoryStatistics(&trace);
    auto total = trace.GetGpuMemorySizeInMB();
    float rsMemSize = 0.f;
    for (uint32_t tagtype = RSTagTracker::TAG_SAVELAYER_DRAW_NODE; tagtype <= RSTagTracker::TAG_CAPTURE; tagtype++) {
        Drawing::GPUResourceTag resourceTag(0, 0, 0, tagtype);
        Drawing::TraceMemoryDump gpuTrace("category", true);
        gpuContext->DumpMemoryStatisticsByTag(&gpuTrace, resourceTag);
        rsMemSize += gpuTrace.GetGpuMemorySizeInMB();
    }
    return total - rsMemSize;
#else
    return 0.f;
#endif
}
#endif

#ifndef USE_ROSEN_DRAWING
void MemoryManager::DumpPidMemory(DfxString& log, int pid, const GrDirectContext* grContext)
{
    //MemoryTrack::Instance().DumpMemoryStatistics(log, pid);
    MemoryGraphic mem = CountPidMemory(pid, grContext);
    log.AppendFormat("GPU Mem(MB):%f\n", mem.GetGpuMemorySize() / (MEMUNIT_RATE * MEMUNIT_RATE));
    log.AppendFormat("CPU Mem(MB):%f\n", mem.GetCpuMemorySize() / (MEMUNIT_RATE * MEMUNIT_RATE));
    log.AppendFormat("Total Mem(MB):%f\n", mem.GetTotalMemorySize() / (MEMUNIT_RATE * MEMUNIT_RATE));
}
#else
void MemoryManager::DumpPidMemory(DfxString& log, int pid, const Drawing::GPUContext* gpuContext)
{
    MemoryGraphic mem = CountPidMemory(pid, gpuContext);
    log.AppendFormat("GPU Mem(MB):%f\n", mem.GetGpuMemorySize() / (MEMUNIT_RATE * MEMUNIT_RATE));
    log.AppendFormat("CPU Mem(KB):%f\n", mem.GetCpuMemorySize() / (MEMUNIT_RATE * MEMUNIT_RATE));
    log.AppendFormat("Total Mem(MB):%f\n", mem.GetTotalMemorySize() / (MEMUNIT_RATE * MEMUNIT_RATE));
}
#endif

#ifndef USE_ROSEN_DRAWING
MemoryGraphic MemoryManager::CountSubMemory(int pid, const GrDirectContext* grContext)
{
    MemoryGraphic memoryGraphic;
    auto subThreadManager = RSSubThreadManager::Instance();
    std::vector<MemoryGraphic> subMems = subThreadManager->CountSubMem(pid);
    for (const auto& mem : subMems) {
        memoryGraphic += mem;
    }
    return memoryGraphic;
}
#else
MemoryGraphic MemoryManager::CountSubMemory(int pid, const Drawing::GPUContext* gpuContext)
{
    MemoryGraphic memoryGraphic;
    auto subThreadManager = RSSubThreadManager::Instance();
    std::vector<MemoryGraphic> subMems = subThreadManager->CountSubMem(pid);
    for (const auto& mem : subMems) {
        memoryGraphic += mem;
    }
    return memoryGraphic;
}
#endif

#ifndef USE_ROSEN_DRAWING
MemoryGraphic MemoryManager::CountPidMemory(int pid, const GrDirectContext* grContext)
{
    MemoryGraphic totalMemGraphic;

    // Count mem of RS
    totalMemGraphic.SetPid(pid);

#if defined (RS_ENABLE_GL) || defined (RS_ENABLE_VK)
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
#else
MemoryGraphic MemoryManager::CountPidMemory(int pid, const Drawing::GPUContext* gpuContext)
{
    MemoryGraphic totalMemGraphic;

    // Count mem of RS
    totalMemGraphic.SetPid(pid);

#if defined (RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    // Count mem of Skia GPU
    if (gpuContext) {
        Drawing::TraceMemoryDump gpuTracer("category", true);
        Drawing::GPUResourceTag tag(pid, 0, 0, 0);
        gpuContext->DumpMemoryStatisticsByTag(&gpuTracer, tag);
        float gpuMem = gpuTracer.GetGLMemorySize();
        totalMemGraphic.IncreaseGpuMemory(gpuMem);
    }
#endif

    return totalMemGraphic;
}
#endif

#ifndef USE_ROSEN_DRAWING
void MemoryManager::CountMemory(std::vector<pid_t> pids, const GrDirectContext* grContext,
    std::vector<MemoryGraphic>& mems)
{
    auto countMem = [&grContext, &mems] (pid_t pid) {
        mems.emplace_back(CountPidMemory(pid, grContext));
    };
    // Count mem of Skia GPU
    std::for_each(pids.begin(), pids.end(), countMem);
}
#else
void MemoryManager::CountMemory(
    std::vector<pid_t> pids, const Drawing::GPUContext* gpuContext, std::vector<MemoryGraphic>& mems)
{
    auto countMem = [&gpuContext, &mems] (pid_t pid) {
        mems.emplace_back(CountPidMemory(pid, gpuContext));
    };
    // Count mem of Skia GPU
    std::for_each(pids.begin(), pids.end(), countMem);
}
#endif

static std::tuple<uint64_t, std::string, RectI> FindGeoById(uint64_t nodeId)
{
    const auto& nodeMap = RSMainThread::Instance()->GetContext().GetNodeMap();
    auto node = nodeMap.GetRenderNode<RSRenderNode>(nodeId);
    uint64_t windowId = nodeId;
    std::string windowName = "NONE";
    RectI nodeFrameRect;
    if (!node) {
        return { windowId, windowName, nodeFrameRect };
    }
    nodeFrameRect =
        (node->GetRenderProperties().GetBoundsGeometry())->GetAbsRect();
    // Obtain the window according to childId
    auto parent = node->GetParent().lock();
    while (parent) {
        if (parent->IsInstanceOf<RSSurfaceRenderNode>()) {
            const auto& surfaceNode = RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(parent);
            windowName = surfaceNode->GetName();
            windowId = surfaceNode->GetId();
            break;
        }
        parent = parent->GetParent().lock();
    }
    return { windowId, windowName, nodeFrameRect };
}

void MemoryManager::DumpRenderServiceMemory(DfxString& log)
{
    log.AppendFormat("\n----------\nRenderService caches:\n");
    MemoryTrack::Instance().DumpMemoryStatistics(log, FindGeoById);
}

void MemoryManager::DumpDrawingCpuMemory(DfxString& log)
{
#ifndef USE_ROSEN_DRAWING
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
#else
    // CPU
    log.AppendFormat("\n----------\nSkia CPU caches:\n");
    log.AppendFormat("Font Cache (CPU):\n");
    log.AppendFormat("  Size: %.2f kB \n", Drawing::SkiaGraphics::GetFontCacheUsed() / MEMUNIT_RATE);
    log.AppendFormat("  Glyph Count: %d \n", Drawing::SkiaGraphics::GetFontCacheCountUsed());

    std::vector<ResourcePair> cpuResourceMap = {
        { "skia/sk_resource_cache/bitmap_", "Bitmaps" },
        { "skia/sk_resource_cache/rrect-blur_", "Masks" },
        { "skia/sk_resource_cache/rects-blur_", "Masks" },
        { "skia/sk_resource_cache/tessellated", "Shadows" },
        { "skia/sk_resource_cache/yuv-planes_", "YUVPlanes" },
        { "skia/sk_resource_cache/budget_glyph_count", "Bitmaps" },
    };
    SkiaMemoryTracer cpuTracer(cpuResourceMap, true);
    Drawing::SkiaGraphics::DumpMemoryStatistics(&cpuTracer);
    log.AppendFormat("CPU Cachesxx:\n");
    cpuTracer.LogOutput(log);
    log.AppendFormat("Total CPU memory usage:\n");
    cpuTracer.LogTotals(log);

    // cache limit
    size_t cacheLimit = Drawing::SkiaGraphics::GetResourceCacheTotalByteLimit();
    size_t fontCacheLimit = Drawing::SkiaGraphics::GetFontCacheLimit();
    log.AppendFormat("\ncpu cache limit = %zu ( fontcache = %zu ):\n", cacheLimit, fontCacheLimit);
#endif
}

#ifndef USE_ROSEN_DRAWING
void MemoryManager::DumpGpuCache(
    DfxString& log, const GrDirectContext* grContext, GrGpuResourceTag* tag, std::string& name)
{
    if (!grContext) {
        log.AppendFormat("grContext is nullptr.\n");
        return;
    }
    /////////////////////////////GPU/////////////////////////
#if defined (RS_ENABLE_GL) || defined (RS_ENABLE_VK)
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
#else
void MemoryManager::DumpGpuCache(
    DfxString& log, const Drawing::GPUContext* gpuContext, Drawing::GPUResourceTag* tag, std::string& name)
{
    if (!gpuContext) {
        log.AppendFormat("gpuContext is nullptr.\n");
        return;
    }
    /* GPU */
#if defined (RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    log.AppendFormat("\n---------------\nSkia GPU Caches:%s\n", name.c_str());
    Drawing::TraceMemoryDump gpuTracer("category", true);
    if (tag) {
        gpuContext->DumpMemoryStatisticsByTag(&gpuTracer, *tag);
    } else {
        gpuContext->DumpMemoryStatistics(&gpuTracer);
    }
    gpuTracer.LogOutput(log);
    log.AppendFormat("Total GPU memory usage:\n");
    gpuTracer.LogTotals(log);
#endif
}
#endif

#ifndef USE_ROSEN_DRAWING
void MemoryManager::DumpAllGpuInfo(DfxString& log, const GrDirectContext* grContext)
{
    if (!grContext) {
        log.AppendFormat("No valid gpu cache instance.\n");
        return;
    }
#if defined (RS_ENABLE_GL) || defined (RS_ENABLE_VK)
    const auto& nodeMap = RSMainThread::Instance()->GetContext().GetNodeMap();
    nodeMap.TraverseSurfaceNodes([&log, &grContext](const std::shared_ptr<RSSurfaceRenderNode> node) {
        GrGpuResourceTag tag(ExtractPid(node->GetId()), 0, node->GetId(), 0);
        std::string name = node->GetName() + " " + std::to_string(node->GetId());
        DumpGpuCache(log, grContext, &tag, name);
    });
#endif
}
#else
void MemoryManager::DumpAllGpuInfo(DfxString& log, const Drawing::GPUContext* gpuContext)
{
    if (!gpuContext) {
        log.AppendFormat("No valid gpu cache instance.\n");
        return;
    }
#if defined (RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    const auto& nodeMap = RSMainThread::Instance()->GetContext().GetNodeMap();
    nodeMap.TraverseSurfaceNodes([&log, &gpuContext](const std::shared_ptr<RSSurfaceRenderNode> node) {
        Drawing::GPUResourceTag tag(ExtractPid(node->GetId()), 0, node->GetId(), 0);
        std::string name = node->GetName() + " " + std::to_string(node->GetId());
        DumpGpuCache(log, gpuContext, &tag, name);
    });
#endif
}
#endif

#ifndef USE_ROSEN_DRAWING
void MemoryManager::DumpDrawingGpuMemory(DfxString& log, const GrDirectContext* grContext)
{
    if (!grContext) {
        log.AppendFormat("No valid gpu cache instance.\n");
        return;
    }
    /////////////////////////////GPU/////////////////////////
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    std::string gpuInfo;
    // total
    DumpGpuCache(log, grContext, nullptr, gpuInfo);
    // Get memory of window by tag
    DumpAllGpuInfo(log, grContext);
    for (uint32_t tagtype = RSTagTracker::TAG_SAVELAYER_DRAW_NODE; tagtype <= RSTagTracker::TAG_CAPTURE; tagtype++) {
        GrGpuResourceTag tag(0, 0, 0, tagtype);
        std::string tagType = RSTagTracker::TagType2String(static_cast<RSTagTracker::TAGTYPE>(tagtype));
        DumpGpuCache(log, grContext, &tag, tagType);
    }
    // cache limit
    size_t cacheLimit = 0;
    size_t cacheUsed = 0;
    grContext->getResourceCacheLimits(nullptr, &cacheLimit);
    grContext->getResourceCacheUsage(nullptr, &cacheUsed);
    log.AppendFormat("\ngpu limit = %zu ( used = %zu ):\n", cacheLimit, cacheUsed);

    //////////////////////////ShaderCache///////////////////
    log.AppendFormat("\n---------------\nShader Caches:\n");
#ifdef NEW_RENDER_CONTEXT
    log.AppendFormat(MemoryHandler::QuerryShader().c_str());
#else
    std::shared_ptr<RenderContext> rendercontext = std::make_shared<RenderContext>();
    log.AppendFormat(rendercontext->GetShaderCacheSize().c_str());
#endif
    // gpu stat
    log.AppendFormat("\n---------------\ndumpGpuStats:\n");
    SkString stat;
    grContext->priv().dumpGpuStats(&stat);

    log.AppendFormat("%s\n", stat.c_str());
#endif
}
#else
void MemoryManager::DumpDrawingGpuMemory(DfxString& log, const Drawing::GPUContext* gpuContext)
{
    if (!gpuContext) {
        log.AppendFormat("No valid gpu cache instance.\n");
        return;
    }
    /* GPU */
#ifdef RS_ENABLE_GL
    if (RSSystemProperties::GetGpuApiType() != GpuApiType::OPENGL) {
        return;
    }
    std::string gpuInfo;
    // total
    DumpGpuCache(log, gpuContext, nullptr, gpuInfo);
    // Get memory of window by tag
    DumpAllGpuInfo(log, gpuContext);
    for (uint32_t tagtype = RSTagTracker::TAG_SAVELAYER_DRAW_NODE; tagtype <= RSTagTracker::TAG_CAPTURE; tagtype++) {
        Drawing::GPUResourceTag tag(0, 0, 0, tagtype);
        std::string tagType = RSTagTracker::TagType2String(static_cast<RSTagTracker::TAGTYPE>(tagtype));
        DumpGpuCache(log, gpuContext, &tag, tagType);
    }
    // cache limit
    size_t cacheLimit = 0;
    size_t cacheUsed = 0;
    gpuContext->GetResourceCacheLimits(nullptr, &cacheLimit);
    gpuContext->GetResourceCacheUsage(nullptr, &cacheUsed);
    log.AppendFormat("\ngpu limit = %zu ( used = %zu ):\n", cacheLimit, cacheUsed);

    /* ShaderCache */
    log.AppendFormat("\n---------------\nShader Caches:\n");
#ifdef NEW_RENDER_CONTEXT
    log.AppendFormat(MemoryHandler::QuerryShader().c_str());
#else
    std::shared_ptr<RenderContext> rendercontext = std::make_shared<RenderContext>();
    log.AppendFormat(rendercontext->GetShaderCacheSize().c_str());
#endif
    // gpu stat
    log.AppendFormat("\n---------------\ndumpGpuStats:\n");
    std::string stat;
    gpuContext->DumpGpuStats(stat);

    log.AppendFormat("%s\n", stat.c_str());
#endif // RS_ENABLE_GL
}
#endif // USE_ROSEN_DRAWING

void MemoryManager::DumpMallocStat(std::string& log)
{
    malloc_stats_print(
        [](void* fp, const char* str) {
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
                RS_LOGW("[mallocstat]:%{public}s", str);
            }
        },
        &log, nullptr);
}
} // namespace OHOS::Rosen