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

#include "native_buffer_utils.h"
#include "platform/common/rs_log.h"

namespace OHOS::Rosen {
namespace NativeBufferUtils {
void DeleteVkImage(void* context)
{
    VulkanCleanupHelper* cleanupHelper = static_cast<VulkanCleanupHelper*>(context);
    if (cleanupHelper != nullptr) {
        cleanupHelper->UnRef();
    }
}

bool GetNativeBufferFormatProperties(const RsVulkanContext& vkContext, VkDevice device, OH_NativeBuffer* nativeBuffer,
                                     VkNativeBufferFormatPropertiesOHOS* nbFormatProps,
                                     VkNativeBufferPropertiesOHOS* nbProps)
{
    nbFormatProps->sType = VK_STRUCTURE_TYPE_NATIVE_BUFFER_FORMAT_PROPERTIES_OHOS;
    nbFormatProps->pNext = nullptr;

    nbProps->sType = VK_STRUCTURE_TYPE_NATIVE_BUFFER_PROPERTIES_OHOS;
    nbProps->pNext = nbFormatProps;

    VkResult err = vkContext.vkGetNativeBufferPropertiesOHOS(device, nativeBuffer, nbProps);
    if (VK_SUCCESS != err) {
        ROSEN_LOGE("NativeBufferUtils: vkGetNativeBufferPropertiesOHOS Failed ! %d", err);
        return false;
    }
    return true;
}

bool CreateVkImage(const RsVulkanContext& vkContext, VkImage* image,
    const VkNativeBufferFormatPropertiesOHOS& nbFormatProps, const VkExtent3D& imageSize,
    VkImageUsageFlags usageFlags = 0)
{
    VkExternalFormatOHOS externalFormat;
    externalFormat.sType = VK_STRUCTURE_TYPE_EXTERNAL_FORMAT_OHOS;
    externalFormat.pNext = nullptr;
    externalFormat.externalFormat = 0;

    if (nbFormatProps.format == VK_FORMAT_UNDEFINED) {
        externalFormat.externalFormat = nbFormatProps.externalFormat;
    }

    const VkExternalMemoryImageCreateInfo externalMemoryImageInfo {
        VK_STRUCTURE_TYPE_EXTERNAL_MEMORY_IMAGE_CREATE_INFO, // sType
        &externalFormat, // pNext
        VK_EXTERNAL_MEMORY_HANDLE_TYPE_OHOS_NATIVE_BUFFER_BIT_OHOS, // handleTypes
    };

    VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL;

    VkImageCreateFlags flags = 0;

    const VkImageCreateInfo imageCreateInfo = {
        VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        &externalMemoryImageInfo,
        flags,
        VK_IMAGE_TYPE_2D,
        nbFormatProps.format,
        imageSize,
        1,
        1,
        VK_SAMPLE_COUNT_1_BIT,
        tiling,
        usageFlags,
        VK_SHARING_MODE_EXCLUSIVE,
        0,
        0,
        VK_IMAGE_LAYOUT_UNDEFINED,
    };

    VkResult err = vkContext.vkCreateImage(vkContext.GetDevice(), &imageCreateInfo, nullptr, image);
    if (err != VK_SUCCESS) {
        ROSEN_LOGE("NativeBufferUtils: vkCreateImage failed");
        return false;
    }
    return true;
}

bool AllocateDeviceMemory(const RsVulkanContext& vkContext, VkDeviceMemory* memory, VkImage& image,
    OH_NativeBuffer* nativeBuffer, VkNativeBufferPropertiesOHOS& nbProps)
{
    VkPhysicalDeviceMemoryProperties2 physicalDeviceMemProps;
    physicalDeviceMemProps.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PROPERTIES_2;
    physicalDeviceMemProps.pNext = nullptr;

    uint32_t foundTypeIndex = 0;
    VkDevice device = vkContext.GetDevice();
    VkPhysicalDevice physicalDevice = vkContext.GetPhysicalDevice();
    vkContext.vkGetPhysicalDeviceMemoryProperties2(physicalDevice, &physicalDeviceMemProps);
    uint32_t memTypeCnt = physicalDeviceMemProps.memoryProperties.memoryTypeCount;
    bool found = false;
    for (uint32_t i = 0; i < memTypeCnt; ++i) {
        if (nbProps.memoryTypeBits & (1 << i)) {
            const VkPhysicalDeviceMemoryProperties& pdmp = physicalDeviceMemProps.memoryProperties;
            uint32_t supportedFlags = pdmp.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
            if (supportedFlags == VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) {
                foundTypeIndex = i;
                found = true;
                break;
            }
        }
    }
    if (!found) {
        ROSEN_LOGE("NativeBufferUtils: no fit memory type, memoryTypeBits is %{public}u", nbProps.memoryTypeBits);
        vkContext.vkDestroyImage(device, image, nullptr);
        return false;
    }

    VkImportNativeBufferInfoOHOS nbImportInfo;
    nbImportInfo.sType = VK_STRUCTURE_TYPE_IMPORT_NATIVE_BUFFER_INFO_OHOS;
    nbImportInfo.pNext = nullptr;
    nbImportInfo.buffer = nativeBuffer;

    VkMemoryDedicatedAllocateInfo dedicatedAllocInfo;
    dedicatedAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_DEDICATED_ALLOCATE_INFO;
    dedicatedAllocInfo.pNext = &nbImportInfo;
    dedicatedAllocInfo.image = image;
    dedicatedAllocInfo.buffer = VK_NULL_HANDLE;

    VkMemoryAllocateInfo allocInfo = {
        VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO, &dedicatedAllocInfo, nbProps.allocationSize, foundTypeIndex,
    };

    VkResult err = vkContext.vkAllocateMemory(device, &allocInfo, nullptr, memory);
    if (err != VK_SUCCESS) {
        vkContext.vkDestroyImage(device, image, nullptr);
        ROSEN_LOGE("NativeBufferUtils: vkAllocateMemory Fail");
        return false;
    }
    return true;
}

bool BindImageMemory(VkDevice device, const RsVulkanContext& vkContext, VkImage& image, VkDeviceMemory& memory)
{
    VkBindImageMemoryInfo bindImageInfo;
    bindImageInfo.sType = VK_STRUCTURE_TYPE_BIND_IMAGE_MEMORY_INFO;
    bindImageInfo.pNext = nullptr;
    bindImageInfo.image = image;
    bindImageInfo.memory = memory;
    bindImageInfo.memoryOffset = 0;

    VkResult err = vkContext.vkBindImageMemory2(device, 1, &bindImageInfo);
    if (err != VK_SUCCESS) {
        ROSEN_LOGE("NativeBufferUtils: vkBindImageMemory2 failed");
        vkContext.vkDestroyImage(device, image, nullptr);
        vkContext.vkFreeMemory(device, memory, nullptr);
        return false;
    }
    return true;
}

#ifndef USE_ROSEN_DRAWING
bool MakeFromNativeWindowBuffer(sk_sp<GrDirectContext> skContext, NativeWindowBuffer* nativeWindowBuffer,
    NativeSurfaceInfo& nativeSurface, int width, int height)
#else
bool MakeFromNativeWindowBuffer(std::shared_ptr<Drawing::GPUContext> skContext, NativeWindowBuffer* nativeWindowBuffer,
    NativeSurfaceInfo& nativeSurface, int width, int height)
#endif
{
    OH_NativeBuffer* nativeBuffer = OH_NativeBufferFromNativeWindowBuffer(nativeWindowBuffer);
    if (nativeBuffer == nullptr) {
        ROSEN_LOGE("MakeFromNativeWindowBuffer: OH_NativeBufferFromNativeWindowBuffer failed");
        return false;
    }

    auto& vkContext = RsVulkanContext::GetSingleton();

    VkDevice device = vkContext.GetDevice();

    VkNativeBufferFormatPropertiesOHOS nbFormatProps;
    VkNativeBufferPropertiesOHOS nbProps;
    if (!GetNativeBufferFormatProperties(vkContext, device, nativeBuffer, &nbFormatProps, &nbProps)) {
        return false;
    }

    VkImageUsageFlags usageFlags = VK_IMAGE_USAGE_SAMPLED_BIT;
    if (nbFormatProps.format != VK_FORMAT_UNDEFINED) {
        usageFlags = usageFlags | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT
            | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    }

    VkImage image;
    if (!CreateVkImage(vkContext, &image, nbFormatProps, {width, height, 1}, usageFlags)) {
        return false;
    }

    VkDeviceMemory memory;
    if (!AllocateDeviceMemory(vkContext, &memory, image, nativeBuffer, nbProps)) {
        return false;
    }

    if (!BindImageMemory(device, vkContext, image, memory)) {
        return false;
    }

#ifndef USE_ROSEN_DRAWING
    GrVkImageInfo image_info;
    image_info.fImage = image;
    image_info.fImageTiling = VK_IMAGE_TILING_OPTIMAL;
    image_info.fImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    image_info.fFormat = nbFormatProps.format;
    image_info.fImageUsageFlags = usageFlags;
    image_info.fSampleCount = 1;
    image_info.fLevelCount = 1;

    GrBackendRenderTarget backend_render_target(width, height, 0, image_info);
    SkSurfaceProps props(0, SkPixelGeometry::kUnknown_SkPixelGeometry);

    SkColorType colorType = kRGBA_8888_SkColorType;

    if (nbFormatProps.format == VK_FORMAT_A2B10G10R10_UNORM_PACK32) {
        colorType = kRGBA_1010102_SkColorType;
    }

    nativeSurface.skSurface = SkSurface::MakeFromBackendRenderTarget(
        skContext.get(), backend_render_target, kTopLeft_GrSurfaceOrigin, colorType,
        SkColorSpace::MakeSRGB(), &props, DeleteVkImage, new VulkanCleanupHelper(RsVulkanContext::GetSingleton(),
        image, memory));
#else
    Drawing::TextureInfo texture_info;
    texture_info.SetWidth(width);
    texture_info.SetHeight(height);
    std::shared_ptr<Drawing::VKTextureInfo> vkTextureInfo = std::make_shared<Drawing::VKTextureInfo>();
    vkTextureInfo->vkImage = image;
    vkTextureInfo->imageTiling = VK_IMAGE_TILING_OPTIMAL;
    vkTextureInfo->imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    vkTextureInfo->format = nbFormatProps.format;
    vkTextureInfo->imageUsageFlags = usageFlags;
    vkTextureInfo->sampleCount = 1;
    vkTextureInfo->levelCount = 1;
    texture_info.SetVKTextureInfo(vkTextureInfo);
    
    Drawing::ColorType colorType = Drawing::ColorType::COLORTYPE_RGBA_8888;
    if (nbFormatProps.format == VK_FORMAT_A2B10G10R10_UNORM_PACK32) {
        colorType = Drawing::ColorType::COLORTYPE_RGBA_1010102;
    }

    nativeSurface.drawingSurface = Drawing::Surface::MakeFromBackendRenderTarget(
        skContext.get(),
        texture_info,
        Drawing::TextureOrigin::TOP_LEFT,
        colorType,
        nullptr,
        DeleteVkImage,
        new VulkanCleanupHelper(RsVulkanContext::GetSingleton(),
            image, memory));
#endif

    nativeSurface.image = image;
    nativeSurface.nativeWindowBuffer = nativeWindowBuffer;

    return true;
}

GrVkYcbcrConversionInfo GetYcbcrInfo(VkNativeBufferFormatPropertiesOHOS& nbFormatProps)
{
    GrVkYcbcrConversionInfo ycbcrInfo = {
        .fFormat = nbFormatProps.format,
        .fExternalFormat = nbFormatProps.externalFormat,
        .fYcbcrModel = nbFormatProps.suggestedYcbcrModel,
        .fYcbcrRange = nbFormatProps.suggestedYcbcrRange,
        .fXChromaOffset = nbFormatProps.suggestedXChromaOffset,
        .fYChromaOffset = nbFormatProps.suggestedYChromaOffset,
        .fChromaFilter = VK_FILTER_NEAREST,
        .fForceExplicitReconstruction = VK_FALSE,
        .fFormatFeatures = nbFormatProps.formatFeatures
    };

    if (VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT & nbFormatProps.formatFeatures) {
        ycbcrInfo.fChromaFilter = VK_FILTER_LINEAR;
    }
    return ycbcrInfo;
}

#ifndef USE_ROSEN_DRAWING
GrBackendTexture MakeBackendTextureFromNativeBuffer(NativeWindowBuffer* nativeWindowBuffer,
    int width, int height, bool isRenderable)
#else
Drawing::BackendTexture MakeBackendTextureFromNativeBuffer(NativeWindowBuffer* nativeWindowBuffer,
    int width, int height)
#endif
{
    OH_NativeBuffer* nativeBuffer = OH_NativeBufferFromNativeWindowBuffer(nativeWindowBuffer);
    if (!nativeBuffer) {
        ROSEN_LOGE("MakeBackendTextureFromNativeBuffer: OH_NativeBufferFromNativeWindowBuffer failed");
        return {};
    }

    auto& vkContext = RsVulkanContext::GetSingleton();
    VkDevice device = vkContext.GetDevice();

    VkNativeBufferFormatPropertiesOHOS nbFormatProps;
    VkNativeBufferPropertiesOHOS nbProps;
    if (!GetNativeBufferFormatProperties(vkContext, device, nativeBuffer, &nbFormatProps, &nbProps)) {
        return {};
    }

    VkImageUsageFlags usageFlags = VK_IMAGE_USAGE_SAMPLED_BIT;
    if (nbFormatProps.format != VK_FORMAT_UNDEFINED) {
        usageFlags = usageFlags | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        if (isRenderable) {
            usageFlags |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        }
    }

    VkImage image;
    if (!CreateVkImage(vkContext, &image, nbFormatProps, {width, height, 1}, usageFlags)) {
        return {};
    }

    VkDeviceMemory memory;
    if (!AllocateDeviceMemory(vkContext, &memory, image, nativeBuffer, nbProps)) {
        return {};
    }

    if (!BindImageMemory(device, vkContext, image, memory)) {
        return {};
    }

#ifndef USE_ROSEN_DRAWING
    GrVkAlloc alloc;
    alloc.fMemory = memory;
    alloc.fOffset = nbProps.allocationSize;

    GrVkImageInfo imageInfo;
    imageInfo.fImage = image;
    imageInfo.fAlloc = alloc;
    imageInfo.fImageTiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.fImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.fFormat = nbFormatProps.format;
    imageInfo.fImageUsageFlags = usageFlags;
    imageInfo.fLevelCount = 1;
    imageInfo.fCurrentQueueFamily = VK_QUEUE_FAMILY_EXTERNAL;
    imageInfo.fYcbcrConversionInfo = GetYcbcrInfo(nbFormatProps);
    imageInfo.fSharingMode = VK_SHARING_MODE_EXCLUSIVE;

    return GrBackendTexture(width, height, imageInfo);
#else
    Drawing::BackendTexture backendTexture(true);
    Drawing::TextureInfo textureInfo;
    textureInfo.SetWidth(width);
    textureInfo.SetHeight(height);

    std::shared_ptr<Drawing::VKTextureInfo> imageInfo = std::make_shared<Drawing::VKTextureInfo>();
    imageInfo->vkImage = image;
    imageInfo->vkAlloc.memory = memory;
    imageInfo->vkAlloc.size = nbProps.allocationSize;
    imageInfo->imageTiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo->imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo->format = nbFormatProps.format;
    imageInfo->imageUsageFlags = usageFlags;
    imageInfo->levelCount = 1;
    imageInfo->currentQueueFamily = VK_QUEUE_FAMILY_EXTERNAL;
    imageInfo->ycbcrConversionInfo.format = nbFormatProps.format;
    imageInfo->ycbcrConversionInfo.externalFormat = nbFormatProps.externalFormat;
    imageInfo->ycbcrConversionInfo.ycbcrModel = nbFormatProps.suggestedYcbcrModel;
    imageInfo->ycbcrConversionInfo.ycbcrRange = nbFormatProps.suggestedYcbcrRange;
    imageInfo->ycbcrConversionInfo.xChromaOffset = nbFormatProps.suggestedXChromaOffset;
    imageInfo->ycbcrConversionInfo.yChromaOffset = nbFormatProps.suggestedYChromaOffset;
    imageInfo->ycbcrConversionInfo.chromaFilter = VK_FILTER_NEAREST;
    imageInfo->ycbcrConversionInfo.forceExplicitReconstruction = VK_FALSE;
    imageInfo->ycbcrConversionInfo.formatFeatures = nbFormatProps.formatFeatures;
    if (VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT & nbFormatProps.formatFeatures) {
        imageInfo->ycbcrConversionInfo.chromaFilter = VK_FILTER_LINEAR;
    }
    imageInfo->sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    textureInfo.SetVKTextureInfo(imageInfo);
    backendTexture.SetTextureInfo(textureInfo);
    return backendTexture;
#endif
}
} // namespace NativeBufferUtils
} // namespace OHOS::Rosen