



#ifndef WGR_GRAPHIC_MEMORYMANAGER_H
#define WGR_GRAPHIC_MEMORYMANAGER_H
#include <string>
#include <include/gpu/GrContext.h>
#include "SkiaMemoryTracer.h"
#include "memory/StringX.h"
namespace OHOS::Rosen {
class MemoryManager {
public:
    static void dumpMemoryUsage(StringX& log, const GrContext* grContext);
    static std::unordered_map<uint64_t, std::string> dumpPidNames(StringX& log);
};
} //namespace
#endif // WGR_GRAPHIC_MEMORYMANAGER_H