/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#include "videodisplaymanager.h"

using namespace OHOS::HDI::Display::V1_0;

namespace OHOS {
    Listener::~Listener()
    {
    }

    void Listener::OnBufferAvailable()
    {
        (void)timestamp;
        (void)damage;
    }

    VideoDisplayManager::VideoDisplayManager() : surface_(nullptr), listener(nullptr)
    {
    }

    VideoDisplayManager::~VideoDisplayManager()
    {
    }

    int32_t VideoDisplayManager::CreateLayer(const LayerInfo &layerInfo, uint32_t &layerId, sptr<Surface>& surface)
    {
        return DISPLAY_FAILURE;
    }

    void VideoDisplayManager::DestroyLayer(uint32_t layerId)
    {
    }

    sptr<IBufferProducer> VideoDisplayManager::AttachLayer(sptr<Surface>& surface, uint32_t layerId)
    {
        return nullptr;
    }

    void VideoDisplayManager::DetachLayer(uint32_t layerId)
    {
    }

    int32_t VideoDisplayManager::SetRect(uint32_t layerId, IRect rect)
    {
        return -1;
    }

    int32_t VideoDisplayManager::GetRect(uint32_t layerId, IRect &rect)
    {
        return -1;
    }

    int32_t VideoDisplayManager::SetZorder(uint32_t layerId, uint32_t zorder)
    {
        return -1;
    }

    int32_t VideoDisplayManager::GetZorder(uint32_t layerId, uint32_t &zorder)
    {
        return -1;
    }

    int32_t VideoDisplayManager::SetTransformMode(uint32_t layerId, TransformType type)
    {
        return -1;
    }

    int32_t VideoDisplayManager::SetVisable(uint32_t layerId, bool visible)
    {
        return -1;
    }

    int32_t VideoDisplayManager::IsVisable(uint32_t layerId, bool &visible)
    {
        return -1;
    }
} // namespace OHOS
