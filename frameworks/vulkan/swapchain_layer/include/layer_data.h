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

#ifndef LAYER_DATA_H
#define LAYER_DATA_H

#include <memory>
#include <refbase.h>
#include <unordered_map>
#include <vulkan/vulkan.h>
#include "vk_layer_dispatch_table.h"
#include "vk_dispatch_table_helper.h"

namespace OHOS::SWAPCHAIN {
namespace {
    struct LayerData {
        VkInstance instance = VK_NULL_HANDLE;
        uint32_t instanceVersion = VK_API_VERSION_1_0;
        std::unique_ptr<VkLayerDispatchTable> deviceDispatchTable;
        std::unique_ptr<VkLayerInstanceDispatchTable> instanceDispatchTable;
        std::unordered_map<VkDebugUtilsMessengerEXT, VkDebugUtilsMessengerCreateInfoEXT> debugCallbacks;
        PFN_vkSetDeviceLoaderData fpSetDeviceLoaderData = nullptr;
    };
    struct LoaderVkLayerDispatchTable;
    typedef uintptr_t DispatchKey;
    std::unordered_map<DispatchKey, LayerData*> g_layerDataMap;
} // anonymous namespace

template <typename T>
inline DispatchKey GetDispatchKey(const T object)
{
    return reinterpret_cast<DispatchKey>(*reinterpret_cast<LoaderVkLayerDispatchTable* const*>(object));
}

template <typename DATA_T>
DATA_T* GetLayerDataPtr(DispatchKey dataKey, std::unordered_map<DispatchKey, DATA_T*> &g_layerDataMap)
{
    DATA_T* debugData;
    auto got = g_layerDataMap.find(dataKey);
    if (got == g_layerDataMap.end()) {
        debugData = new DATA_T;
        g_layerDataMap[dataKey] = debugData;
    } else {
        debugData = got->second;
    }
    return debugData;
}

template <typename DATA_T>
void FreeLayerDataPtr(DispatchKey dataKey, std::unordered_map<DispatchKey, DATA_T*> &g_layerDataMap)
{
    auto got = g_layerDataMap.find(dataKey);
    if (got == g_layerDataMap.end()) {
        return;
    }
    delete got->second;
    g_layerDataMap.erase(got);
}

}
#endif // LAYER_DATA_H
