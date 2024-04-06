/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef RS_LAYER_SYNTHESIS_MODE_COLLECTION_H
#define RS_LAYER_SYNTHESIS_MODE_COLLECTION_H

#include "common/rs_common_def.h"

namespace OHOS {
namespace Rosen {

struct LayerSynthesisModeInfo {
    int32_t uniformRenderFrameNumber;
    int32_t offlineComposeFrameNumber;
    int32_t redrawFrameNumber;
    LayerSynthesisModeInfo()
        : uniformRenderFrameNumber(), offlineComposeFrameNumber(), redrawFrameNumber() {}
    LayerSynthesisModeInfo(int32_t uniformRenderFrameNumber_, int32_t offlineComposeFrameNumber_,
        int32_t redrawFrameNumber_)
        : uniformRenderFrameNumber(uniformRenderFrameNumber_), offlineComposeFrameNumber(offlineComposeFrameNumber_),
          redrawFrameNumber(redrawFrameNumber_) {}
};

class RSB_EXPORT LayerSynthesisModeCollection {
public:
    static LayerSynthesisModeCollection& GetInstance();

    void UpdateUniformOrOfflineComposeFrameNumberForDFX(size_t layerSize);
    void UpdateRedrawFrameNumberForDFX();
    LayerSynthesisModeInfo GetLayerSynthesisModeInfo();
    void ResetLayerSynthesisModeInfo();

private:
    LayerSynthesisModeCollection();
    ~LayerSynthesisModeCollection() noexcept;
    LayerSynthesisModeCollection(const LayerSynthesisModeCollection&) = delete;
    LayerSynthesisModeCollection(const LayerSynthesisModeCollection&&) = delete;
    LayerSynthesisModeCollection& operator=(const LayerSynthesisModeCollection&) = delete;
    LayerSynthesisModeCollection& operator=(const LayerSynthesisModeCollection&&) = delete;

    LayerSynthesisModeInfo layerSynthesisModeInfo_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RS_LAYER_SYNTHESIS_MODE_COLLECTION_H