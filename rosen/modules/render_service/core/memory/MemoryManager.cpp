



#include "MemoryManager.h"

#include <include/gpu/GrContexOptions.h>
#include <SkExecutor.h>
#include <SkGraphics.h>
#include "platform/common/rs_log.h"
#include "render/rs_image_cache.h"
#include "pipeline/rs_main_thread.h"
#include <parameters.h>

namespace OHOS::Rosen {
void MemoryManager::dumpMemoryUsage(StringX& log, const GrContext* grContext)
{
    if (!grContext) {
        log.appendFormat("No valid cache instance.\n");
        return;
    }       
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    

    
    
    //////////////////////////////CPU/////////////////////////
    log.appendFormat("\n----------------\nSkia CPU Caches:\n");
    log.appendFormat("Font Cache (CPU):\n");
    log.appendFormat("  Size: %.2f kB \n", SkGraphics::GetFontCacheUsed() / 1024.0f);
    log.appendFormat("  Glyph Count: %d \n", SkGraphics::GetFontCacheCountUsed());

    std::vector<ResourcePair> cpuResourceMap = {
        { "skia/sk_resource_cache/bitmap_", "Bitmaps" },
        { "skia/sk_resource_cache/rrect-blur", "Masks" },
        { "skia/sk_resource_cache/rects-blur", "Masks" },
        { "skia/sk_resource_cache/tessellated", "Shadows" },
        { "skia/sk_resource_cache/yuv-planes_", "YUVPlanes"},
        { "skia/sk_resource_cache/budget_glyph_count", "Bitmaps"},
    };
    SkiaMemoryTracer cpuTracer(cpuResourceMap, true);
    SkGraphics::DumpMemoryStatistics(&cpuTracer);
    log.appendFormat("CPU Cachesxx:\n");
    cpuTracer.logOutput(log);
    log.appendFormat("Total CPU memory usage:\n");
    cpuTracer.logTotals(log);

    /////////////////////////////GPU/////////////////////////
    log.appendFormat("\n----------------\nSkia GPU Caches:\n");
    SkiaMemoryTracer gpuTracer("category", true);
    grContext->dumpMemoryStatistics(&gpuTracer);
    gpuTracer.logOutput(log);
    log.appendFormat("Total GPU memory usage:\n");
    gpuTracer.logTotals(log);

    ////////////////////////////ShaderCache//////////////////
    log.appendFormat("\n----------------\nShader Caches:\n");
    std::shared_ptr<RenderContext> rendercontext = std::make_shared<RenderContext> ();
    log.appendFormat("ShaderCache RAM is used: %.2f KB\n", rendercontext->GetShaderCacheSize().first /1024.0f);
    log.appendFormat("ShaderCache Num is: %d \n", rendercontext->GetShaderCacheSize().second);
}
}