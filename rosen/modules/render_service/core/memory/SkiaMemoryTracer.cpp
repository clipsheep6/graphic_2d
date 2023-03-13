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

#include "SkiaMemoryTracer.h"

namespace OHOS::Rosen {
constexpr uint32_t MEMUNIT_RATE = 1024;

SkiaMemoryTracer::SkiaMemoryTracer(std::vector<ResourcePair> resourceMap, bool itemizeType)
    : resourceMap_(resourceMap), itemizeType_(itemizeType), totalSize_("bytes", 0), purgeableSize_("bytes", 0)
{}

SkiaMemoryTracer::SkiaMemoryTracer(const char* categoryKey, bool itemizeType)
    : categoryKey_(categoryKey), itemizeType_(itemizeType), totalSize_("bytes", 0), purgeableSize_("bytes", 0)
{}

const char* SkiaMemoryTracer::MapName(const char* resourceName)
{
    for (auto& resource : resourceMap_) {
        if (SkStrContains(resourceName, resource.first)) {
            return resource.second;
        }
    }
    return nullptr;
}

void SkiaMemoryTracer::ProcessElement()
{
    if (!currentElement_.empty()) {
        // Only count elements that contain "size", other values just provide metadata.
        auto sizeResult = currentValues_.find("size");
        if (sizeResult != currentValues_.end()) {
            totalSize_.value += sizeResult->second.value;
            totalSize_.count++;
        } else {
            currentElement_.clear();
            currentValues_.clear();
            return;
        }

        // find the purgeable size if one exists
        auto purgeableResult = currentValues_.find("purgeable_size");
        if (purgeableResult != currentValues_.end()) {
            purgeableSize_.value += purgeableResult->second.value;
            purgeableSize_.count++;
        }

        // find the type if one exists
        const char* type;
        auto typeResult = currentValues_.find("type");
        if (typeResult != currentValues_.end()) {
            type = typeResult->second.units;
        } else if (itemizeType_) {
            type = "Other";
        }

        // compute the type if we are itemizing or use the default "size" if we are not
        std::string key = (itemizeType_) ? type : sizeResult->first;

        // compute the top level element name using either the map or category key
        const char* resourceName = MapName(currentElement_.c_str());
        if (categoryKey_ != nullptr) {
            // find the category if one exists
            auto categoryResult = currentValues_.find(categoryKey_);
            if (categoryResult != currentValues_.end()) {
                resourceName = categoryResult->second.units;
            }
        }

        // if we don't have a resource name then we don't know how to label the
        // data and should abort.
        if (resourceName == nullptr) {
            resourceName = currentElement_.c_str();
        }

        auto result = results_.find(resourceName);
        if (result == results_.end()) {
            TraceValue sizeValue = sizeResult->second;
            currentValues_.clear();
            currentValues_.insert({ key, sizeValue });
            results_.insert({ resourceName, currentValues_ });
        } else {
            auto& resourceValues = result->second;
            typeResult = resourceValues.find(key);
            if (typeResult == resourceValues.end()) {
                resourceValues.insert({ key, sizeResult->second });
            } else {
                typeResult->second.value += sizeResult->second.value;
                typeResult->second.count++;
            }
        }
    }

    currentElement_.clear();
    currentValues_.clear();
}

void SkiaMemoryTracer::dumpNumericValue(const char* dumpName, const char* valueName, const char* units, uint64_t value)
{
    if (currentElement_ != dumpName) {
        ProcessElement();
        currentElement_ = dumpName;
    }
    currentValues_.insert({ valueName, { units, value } });
}

void SkiaMemoryTracer::LogOutput(DfxString& log)
{
    // process any remaining elements
    ProcessElement();

    for (const auto& namedItem : results_) {
        if (itemizeType_) {
            log.AppendFormat("  %s:\n", namedItem.first.c_str()); // skia/sk_glyph_cache
            for (const auto& typedValue : namedItem.second) {
                TraceValue traceValue = ConvertUnits(typedValue.second);
                const char* entry = (traceValue.count > 1) ? "entries" : "entry";
                log.AppendFormat("    %s: %.2f %s (%d %s)\n", typedValue.first.c_str(), traceValue.value,
                    traceValue.units, traceValue.count, entry);
            }
        } else {
            auto result = namedItem.second.find("size");
            if (result != namedItem.second.end()) {
                TraceValue traceValue = ConvertUnits(result->second);
                const char* entry = (traceValue.count > 1) ? "entries" : "entry";
                log.AppendFormat("  %s: %.2f %s (%d %s)\n", namedItem.first.c_str(), traceValue.value, traceValue.units,
                    traceValue.count, entry);
            }
        }
    }
}

float SkiaMemoryTracer::GetGLMemorySize() const
{
    return totalSize_.value;
}

void SkiaMemoryTracer::LogTotals(DfxString& log)
{
    TraceValue total = ConvertUnits(totalSize_);
    TraceValue purgeable = ConvertUnits(purgeableSize_);
    log.AppendFormat("    %.0f bytes, %.2f %s (%.2f %s is purgeable)\n", totalSize_.value, total.value, total.units,
        purgeable.value, purgeable.units);
}

SkiaMemoryTracer::TraceValue SkiaMemoryTracer::ConvertUnits(const TraceValue& value)
{
    TraceValue output(value);
    if (SkString(output.units) == SkString("bytes") && output.value >= MEMUNIT_RATE) {
        output.value = output.value / MEMUNIT_RATE;
        output.units = "KB";
    }
    if (SkString(output.units) == SkString("KB") && output.value >= MEMUNIT_RATE) {
        output.value = output.value / MEMUNIT_RATE;
        output.units = "MB";
    }
    return output;
}

} // namespace OHOS::Rosen