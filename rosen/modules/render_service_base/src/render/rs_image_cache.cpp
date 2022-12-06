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

#include "render/rs_image_cache.h"
#include "memory/StringX.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
// modify the RSImageCache instance as global to extend life cycle, fix destructor crash
static RSImageCache gRSImageCacheInstance;

RSImageCache& RSImageCache::Instance()
{
    return gRSImageCacheInstance;
}

void RSImageCache::CacheSkiaImage(uint64_t uniqueId, sk_sp<SkImage> img)
{
    if (img) {
        std::lock_guard<std::mutex> lock(mutex_);
        skiaImageCache_.emplace(uniqueId, img);
    }
}

void RSImageCache::SetNodeId(uint64_t uniqueId, uint64_t nodeId)
{
    IdMap_.emplace(uniqueId, nodeId);
}

sk_sp<SkImage> RSImageCache::GetSkiaImageCache(uint64_t uniqueId) const
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = skiaImageCache_.find(uniqueId);
    if (it != skiaImageCache_.end()) {
        return it->second;
    }
    return nullptr;
}

void RSImageCache::ReleaseSkiaImageCache(uint64_t uniqueId)
{
    // release the skimage if nobody else holds it
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = skiaImageCache_.find(uniqueId);
    if (it != skiaImageCache_.end() && (!it->second || it->second->unique())) {
        skiaImageCache_.erase(it);
    }
    auto it2 = IdMap_.find(uniqueId);
    if (it2 != IdMap_.end()) {
        IdMap_.erase(it2);
    }
}

int CalcImageSize(sk_sp<SkImage> image) {
    int bytes = 0;
    auto colorType = image->colorType();
    switch (colorType) {
        case SkColorType::kRGBA_8888_SkColorType:
        case SkColorType::kRGB_888x_SkColorType:
        case SkColorType::kBGRA_8888_SkColorType:
        case SkColorType::kRGBA_1010102_SkColorType:
        case SkColorType::kRGB_101010x_SkColorType:
            bytes = 4;
            break;
        case SkColorType::kAlpha_8_SkColorType:
        case SkColorType::kGray_8_SkColorType:
            bytes = 1;
            break;
        case SkColorType::kARGB_4444_SkColorType:
        case SkColorType::kRGB_565_SkColorType:
            bytes = 2;
            break;
        case SkColorType::kRGBA_F16Norm_SkColorType:
        case SkColorType::kRGBA_F16_SkColorType:
            bytes = 8;
            break;
        case SkColorType::kRGBA_F32_SkColorType:
            bytes = 16;
            break;
        default:
            break;
    }
    return bytes * image->width() * image->height() / 1024; 
}

void RSImageCache::DumpMemoryStatistics(std::string& log, std::function<std::string (uint64_t)> func) 
{
    std::lock_guard<std::mutex> lock(mutex_);
    log.append("RSImageCache:\n");
    log.append("id\t\tnid\t\tpid\tsize\trefcnt\ttype\t\twindow\n");

    int totalSize = 0, size, count = 0;
    for(auto& [id, img] : skiaImageCache_) {
        size = CalcImageSize(img); // K
        totalSize += size / 1024;
        count++;
        // get
        uint64_t pid = id >> 32;
        auto ids = IdMap_.find(id);
        uint64_t nodeid = 0;
        if (ids != IdMap_.end()) {
            nodeid = ids->second;
        }
        auto winName = func(nodeid);
        log.append(std::to_string(id) + "\t" + std::to_string(nodeid) + "\t" + std::to_string(pid) + "\t" + std::to_string(size) + "\t" +
        std::to_string(img->getRefCnt()) + "\t" +img->name() + "\t" + winName + "\n");
    }
    log.append("Total Size = " + std::to_string(totalSize) + " MB (" + std::to_string(count) + " entries)\n");

    // log.append("  size is LOGIC, lazy image will decode by canvas.draw\n");
    // log.append("  window ?? : node not on any window\n");
    // log.append("  nid 0: rsimage not on any node, modifier not applied yet\n");
}
} // namespace Rosen
} // namespace OHOS
