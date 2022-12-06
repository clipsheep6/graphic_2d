



#ifndef WGR_GRAPHIC_MEMORYMANAGER_H
#define WGR_GRAPHIC_MEMORYMANAGER_H
#include <map>
#include <string>
#include <include/gpu/GrContext.h>
#include "SkiaMemoryTracer.h"
#include "memory/StringX.h"
#include "pipeline/rs_cold_start_thread.h"
#include "pipeline/rs_surface_render_node.h"

namespace OHOS::Rosen {
class MemoryManager {
public:
    static void dumpMemoryUsage(StringX& log, const GrContext* grContext, std::map<NodeId, std::unique_ptr<RSColdStartThread>>& coldstartthreadmap);
    static std::unordered_map<uint64_t, std::string> dumpPidNames(StringX& log);
};
} //namespace
#endif // WGR_GRAPHIC_MEMORYMANAGER_H