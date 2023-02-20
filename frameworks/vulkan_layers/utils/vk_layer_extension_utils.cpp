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

#include <string.h>
#include "vk_layer_extension_utils.h"

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

/*
 * This file contains utility functions for layers
 */

VK_LAYER_EXPORT VkResult util_GetExtensionProperties(const uint32_t count, const VkExtensionProperties *layer_extensions,
                                                     uint32_t *pCount, VkExtensionProperties *pProperties) {
    uint32_t copy_size;

    if (pProperties == NULL || layer_extensions == NULL) {
        *pCount = count;
        return VK_SUCCESS;
    }

    copy_size = *pCount < count ? *pCount : count;
    memcpy(pProperties, layer_extensions, copy_size * sizeof(VkExtensionProperties));
    *pCount = copy_size;
    if (copy_size < count) {
        return VK_INCOMPLETE;
    }

    return VK_SUCCESS;
}

VK_LAYER_EXPORT VkResult util_GetLayerProperties(const uint32_t count, const VkLayerProperties *layer_properties, uint32_t *pCount,
                                                 VkLayerProperties *pProperties) {
    uint32_t copy_size;

    if (pProperties == NULL || layer_properties == NULL) {
        *pCount = count;
        return VK_SUCCESS;
    }

    copy_size = *pCount < count ? *pCount : count;
    memcpy(pProperties, layer_properties, copy_size * sizeof(VkLayerProperties));
    *pCount = copy_size;
    if (copy_size < count) {
        return VK_INCOMPLETE;
    }

    return VK_SUCCESS;
}
