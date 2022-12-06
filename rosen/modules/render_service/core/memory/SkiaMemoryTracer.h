















#pragma once

#include <SkString.h>
#include <SkTraceMemoryDump.h>
#include <unordered_map>
#include <vector>

#include "memory/StringX.h"

namespace OHOS::Rosen {
typedef std::pair<const char*, const char*> ResourcePair;

class SkiaMemoryTracer : public SkTraceMemoryDump {
public:
    SkiaMemoryTracer(std::vector<ResourcePair> resourceMap, bool itemizeType);
    SkiaMemoryTracer(const char* categoryKey, bool itemizeType);
    ~SkiaMemoryTracer() override {}

    void logOutput(StringX& log);
    void logTotals(StringX& log);

    void dumpNumericValue(const char* dumpName, const char* valueName, const char* units,
                          uint64_t value) override;

    void dumpStringValue(const char* dumpName, const char* valueName, const char* value) override
    {
        // for
        dumpNumericValue(dumpName, valueName, value, 0);
    }

    LevelOfDetail getRequestedDetails() const override
    {
        return SkTraceMemoryDump::kLight_LevelOfDetail;
    }

    bool shouldDumpWrappedObjects() const override { return true; }
    void setMemoryBacking(const char*, const char*, const char*) override {}
    void setDiscardableMemoryBacking(const char*, const SkDiscardableMemory&) override {}

private:
    struct TraceValue {
        TraceValue(const char* units, uint64_t value) : units(units), value(value), count(1) {}
        TraceValue(const TraceValue& v) : units(v.units), value(v.value), count(v.count) {}

        const char* units;
        float value;
        int count;
    };

    const char* mapName(const char* resourceName);
    void processElement();
    TraceValue convertUnits(const TraceValue& value);

    const std::vector<ResourcePair> mResourceMap;
    const char* mCategoryKey = nullptr;
    const bool mItemizeType;

    // variables
    TraceValue mTotalSize;
    TraceValue mPurgeableSize;

    // variables
    std::string mCurrentElement;
    std::unordered_map<std::string, TraceValue> mCurrentValues;

    // variables
    std::unordered_map<std::string, std::unordered_map<std::string, TraceValue>> mResults;
};

} // namespace
