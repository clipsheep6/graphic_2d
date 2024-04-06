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

#include "info_collection/rs_layer_systhesis_mode_collection.h"

namespace OHOS {
namespace Rosen {
LayerSynthesisModeCollection& LayerSynthesisModeCollection::GetInstance()
{
    static LayerSynthesisModeCollection instance;
    return instance;
}

LayerSynthesisModeCollection::LayerSynthesisModeCollection()
{
}

LayerSynthesisModeCollection::~LayerSynthesisModeCollection() noexcept
{
}

void LayerSynthesisModeCollection::UpdateUniformOrOfflineComposeFrameNumberForDFX(size_t layerSize)
{
    if (layerSize == 1) {
        ++layerSynthesisModeInfo_.uniformRenderFrameNumber;
        return;
    }
    ++layerSynthesisModeInfo_.offlineComposeFrameNumber;
}

void LayerSynthesisModeCollection::UpdateRedrawFrameNumberForDFX()
{
    --layerSynthesisModeInfo_.offlineComposeFrameNumber;
    ++layerSynthesisModeInfo_.redrawFrameNumber;
}

LayerSynthesisModeInfo LayerSynthesisModeCollection::GetLayerSynthesisModeInfo()
{
    return layerSynthesisModeInfo_;
}

void LayerSynthesisModeCollection::ResetLayerSynthesisModeInfo()
{
    layerSynthesisModeInfo_ = LayerSynthesisModeInfo {};
}
} // namespace Rosen
} // namespace OHOS