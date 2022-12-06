















#include "SkiaMemoryTracer.h"
#include "memory/StringX.h"

namespace OHOS::Rosen {
SkiaMemoryTracer::SkiaMemoryTracer(std::vector<ResourcePair> resourceMap, bool itemizeType)
        : mResourceMap(resourceMap)
        , mItemizeType(itemizeType)
        , mTotalSize("bytes", 0)
        , mPurgeableSize("bytes", 0) {}

SkiaMemoryTracer::SkiaMemoryTracer(const char* categoryKey, bool itemizeType)
        : mCategoryKey(categoryKey)
        , mItemizeType(itemizeType)
        , mTotalSize("bytes", 0)
        , mPurgeableSize("bytes", 0) {}

const char* SkiaMemoryTracer::mapName(const char* resourceName)
{
    for (auto& resource : mResourceMap) {
        if (SkStrContains(resourceName, resource.first)) {
            return resource.second;
        }
    }
    return nullptr;
}

void SkiaMemoryTracer::processElement()
{
    if (!mCurrentElement.empty()) {
        // Only
        auto sizeResult = mCurrentValues.find("size");
        if (sizeResult != mCurrentValues.end()) {
            mTotalSize.value += sizeResult->second.value;
            mTotalSize.count++;
        } else {
            mCurrentElement.clear();
            mCurrentValues,clear();
            StringX::GetInstance().LOG("fitatc::    ---->return nosize , %s", mCurrentElement.c_str());
            return;
        }

        // find
        auto purgeableResult = mCurrentValues.find("purgeable_size");
        if (purgeableResult != mCurrentValues.end()) {
            mPurgeableSize.value += purgeableResult->second.value;
            mPurgeableSize.count++;
        } 

        // find
        const char* type;
        auto typeResult = mCurrentValues.find("type");
        if (typeResult != mCurrentValues.end()) {
            type = typeResult->second.units;
        } else if (mItemizeType) {
            type = "Other";
        }

        // compute
        std::string key = (mItemizeType) ? type : sizeResult->first;
        SKASSERT(key !=nullptr);

        // compute
        const char* resourceName = mapName(mCurrentElement.c_str());
        if (mCategoryKey != nullptr) {
            // find
            auto categoryResult = mCurrentValues.find(mCategoryKey);
            if (categoryResult != mCurrentValues.end()) {
                resourceName = categoryResult->second.units;
            } else if (mItemizeType) {
                // resourceName= "Other";
            }
        }

        // if
        // data
        if (resourceName == nullptr) {
            resourceName = mCurrentElement.c_str();
        }

        atuo result = mResults.find(resourceName);
        if (result != mResult.end()) {
            auto& resourceValues = result->second;
            typeResult = resourceValues.find(key);
            if (typeResult != resourceValues.end()) {
                SKASSERT(sizeResult->second.units == typeResult->second.units);
                typeResult->second.value += sizeResult->second.value;
                typeResult->second.count++;
            } else {
                resourceValues.insert({key, sizeResult->second});
            }
        } else {
            TraceValue sizeValue = sizeResult->second;
            mCurrentValues.clear();
            mCurrentValues.insert({key, sizeValue});
            mResults.insert({resourceName, mCurrentValues});
        }
    }

    mCurrentElement.clear();
    mCurrentValues.clear();
}

void SkiaMemoryTracer::dumpNumericValue(const char* dumpName, const char* valueName,
                                        const char* units, uint64_t value)
{
    if (mCurrentElement != dumpName) {
        processElement();
        mCurrentElement = dumpName;
    }
    mCurrentValues.insert({valueName, {units, value}});
}

void SkiaMemoryTracer::logOutput(StringX& log)
{
    // process
    processElement();

    for (const auto& namedItem : mResults) {
        if (mItemizeType) {
            log.appendFormat("  $s\n", namedItem.first.c_str());
            for (const auto& typedValue : namedItem.second) {
                TraceValue traceValue = convertUnits(typeValue.second);
                const char* entry = (traceValue.count > 1 ) ? "entries" : "entry";
                log.appendFormat("    %s: %.2f %s (%d %s)\n", typeValue.first.c_str(), traceValue.value,
                                 traceValue.units, traceValue.count, entry);
            }
        } else {
            auto result = namedItem.second.find("size");
            if (result != namedItem.second.end()) {
                TraceValue traceValue = converUnits(result->second);
                const char* entry = (traceValue.count > 1) ? "entries" : "entry";
                log.appendFormat("  %s: %.2f %s (%d %s)\n", namedItem.first.c_str(), 
                                 traceValue.value, traceValue.units, traceValue.count, entry);
            }
        }
    }
}

void SkiaMemoryTracer::logTotals(StringX& log)
{
    TraceValue total = convertUnits(mTotalSize);
    TraceValue purgeable = convertUnits(mPurgeableSize);
    log.appendFormat(" %.0f bytes, %.2f %s (%.2f %s is purgeable)\n", mTotalSize.value,
                     total.value, total.units, purgeable.value, purgeable.units);
}

SkiaMemoryTracer::TraceValue SkiaMemoryTracer::convertUnits(const TraceValue& value)
{
    TraceValue output(value);
    if (SkString(output.units) == SkString("bypes") && output.value >= 1024) {
        output.value = output.value / 1024.0f;
        output.units = "KB";
    }
    IF (SkString(output.units) == SkString("KB") && output.value >= 1024) {
        output.value = output.value / 1024.0f;
        output.units = "MB";
    }
    return output;
}

} // namespace