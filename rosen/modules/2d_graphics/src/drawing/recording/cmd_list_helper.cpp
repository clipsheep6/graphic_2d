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

#include "recording/cmd_list_helper.h"

#include "include/core/SkSerialProcs.h"

#ifdef SUPPORT_OHOS_PIXMAP
#include "pixel_map.h"
#endif
#include "recording/color_filter_cmd_list.h"
#include "recording/color_space_cmd_list.h"
#include "recording/draw_cmd_list.h"
#include "recording/image_filter_cmd_list.h"
#include "recording/mask_filter_cmd_list.h"
#include "recording/path_cmd_list.h"
#include "recording/path_effect_cmd_list.h"
#include "recording/region_cmd_list.h"
#include "recording/shader_effect_cmd_list.h"
#include "skia_adapter/skia_vertices.h"
#include "utils/log.h"

#include "skia_adapter/skia_picture.h"
#include "skia_adapter/skia_text_blob.h"
#include "skia_adapter/skia_typeface.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
static int ColorTypeToBytesPerPixel(ColorType colorType)
{
    // returns the number of bytes per pixel: 1byte, 2bytes, 4bytes
    switch (colorType) {
        case ColorType::COLORTYPE_ALPHA_8:
            return 1;
        case ColorType::COLORTYPE_RGB_565:
        case ColorType::COLORTYPE_ARGB_4444:
            return 2;
        case ColorType::COLORTYPE_RGBA_8888:
        case ColorType::COLORTYPE_BGRA_8888:
        case ColorType::COLORTYPE_N32:
            return 4;
        case ColorType::COLORTYPE_UNKNOWN:
        default:
            return 0;
    }
}

ImageHandle CmdListHelper::AddImageToCmdList(CmdList& cmdList, const Image& image)
{
    auto data = image.Serialize();
    if (data == nullptr || data->GetSize() == 0) {
        LOGE("image is valid!");
        return { 0 };
    }

    auto offset = cmdList.AddImageData(data->GetData(), data->GetSize());
    return { offset, data->GetSize() };
}

ImageHandle CmdListHelper::AddImageToCmdList(CmdList& cmdList, const std::shared_ptr<Image>& image)
{
    if (image == nullptr) {
        LOGE("image is nullptr!");
        return { 0 };
    }
    return CmdListHelper::AddImageToCmdList(cmdList, *image);
}

std::shared_ptr<Image> CmdListHelper::GetImageFromCmdList(const CmdList& cmdList, const ImageHandle& imageHandle)
{
    const void* ptr = cmdList.GetImageData(imageHandle.offset);
    if (imageHandle.size == 0 || ptr == nullptr) {
        LOGE("get image data failed!");
        return nullptr;
    }

    auto imageData = std::make_shared<Data>();
    imageData->BuildWithoutCopy(ptr, imageHandle.size);
    auto image = std::make_shared<Image>();
    if (image->Deserialize(imageData) == false) {
        LOGE("image deserialize failed!");
        return nullptr;
    }
    return image;
}

VerticesHandle CmdListHelper::AddVerticesToCmdList(CmdList& cmdList, const Vertices& vertices)
{
    std::shared_ptr<SkiaVertices> skiaVertices = vertices.GetImpl<SkiaVertices>();
    if (skiaVertices == nullptr) {
        LOGE("skiaVertices nullptr, %{public}s, %{public}d", __FUNCTION__, __LINE__);
        return { 0 };
    }

    auto data = skiaVertices->Serialize();
    if (data == nullptr || data->GetSize() == 0) {
        LOGE("vertices is valid!");
        return { 0 };
    }

    auto offset = cmdList.AddImageData(data->GetData(), data->GetSize());
    return { offset, data->GetSize() };
}

std::shared_ptr<Vertices> CmdListHelper::GetVerticesFromCmdList(
    const CmdList& cmdList, const VerticesHandle& verticesHandle)
{
    const void* ptr = cmdList.GetImageData(verticesHandle.offset);

    if (verticesHandle.size == 0 || ptr == nullptr) {
        LOGE("get vertices data failed!");
        return nullptr;
    }

    auto verticesData = std::make_shared<Data>();
    verticesData->BuildWithoutCopy(ptr, verticesHandle.size);
    auto vertices = std::make_shared<Vertices>();

    std::shared_ptr<SkiaVertices> skiaVertices = vertices->GetImpl<SkiaVertices>();
    if (skiaVertices == nullptr) {
        LOGE("skiaVertices nullptr, %{public}s, %{public}d", __FUNCTION__, __LINE__);
        return nullptr;
    }

    if (skiaVertices->Deserialize(verticesData) == false) {
        LOGE("vertices deserialize failed!");
        return nullptr;
    }
    return vertices;
}

ImageHandle CmdListHelper::AddBitmapToCmdList(CmdList& cmdList, const Bitmap& bitmap)
{
    auto format = bitmap.GetFormat();
    auto bpp = ColorTypeToBytesPerPixel(format.colorType);
    auto bitmapSize = bitmap.GetHeight() * bitmap.GetWidth() * bpp;
    if (bitmapSize == 0) {
        LOGE("bitmap is valid!");
        return { 0 };
    }

    auto offset = cmdList.AddImageData(bitmap.GetPixels(), bitmapSize);
    return { offset, bitmapSize, bitmap.GetWidth(), bitmap.GetHeight(), format.colorType, format.alphaType };
}

std::shared_ptr<Bitmap> CmdListHelper::GetBitmapFromCmdList(const CmdList& cmdList, const ImageHandle& bitmapHandle)
{
    const void* ptr = cmdList.GetImageData(bitmapHandle.offset);
    if (bitmapHandle.size == 0 || ptr == nullptr) {
        LOGE("get bitmap data failed!");
        return nullptr;
    }

    BitmapFormat format = { bitmapHandle.colorType, bitmapHandle.alphaType };
    auto bitmap = std::make_shared<Bitmap>();
    bitmap->Build(bitmapHandle.width, bitmapHandle.height, format);
    bitmap->SetPixels(const_cast<void*>(cmdList.GetImageData(bitmapHandle.offset)));

    return bitmap;
}

ImageHandle CmdListHelper::AddPixelMapToCmdList(CmdList& cmdList, const std::shared_ptr<Media::PixelMap>& pixelMap)
{
#ifdef SUPPORT_OHOS_PIXMAP
    auto index = cmdList.AddPixelMap(pixelMap);
    return { index };
#else
    LOGE("Not support drawing Media::PixelMap");
    return { 0 };
#endif
}

std::shared_ptr<Media::PixelMap> CmdListHelper::GetPixelMapFromCmdList(
    const CmdList& cmdList, const ImageHandle& pixelMapHandle)
{
#ifdef SUPPORT_OHOS_PIXMAP
        return (const_cast<CmdList&>(cmdList)).GetPixelMap(pixelMapHandle.offset);
#else
    LOGE("Not support drawing Media::PixelMap");
    return nullptr;
#endif
}

ImageHandle CmdListHelper::AddImageObjectToCmdList(CmdList& cmdList, const std::shared_ptr<ExtendImageObject>& object)
{
#ifdef SUPPORT_OHOS_PIXMAP
    auto index = cmdList.AddImageObject(object);
    return { index };
#else
    LOGE("Not support drawing ExtendImageObject");
    return { 0 };
#endif
}

std::shared_ptr<Media::PixelMap> CmdListHelper::GetImageObjectFromCmdList(
    const CmdList& cmdList, const ImageHandle& objectHandle)
{
#ifdef SUPPORT_OHOS_PIXMAP
        return (const_cast<CmdList&>(cmdList)).GetImageObject(objectHandle.offset);
#else
    LOGE("Not support drawing ExtendImageObject");
    return nullptr;
#endif
}

ImageHandle CmdListHelper::AddPictureToCmdList(CmdList& cmdList, const Picture& picture)
{
    auto data = picture.GetImpl<SkiaPicture>()->Serialize();
    if (data == nullptr || data->GetSize() == 0) {
        LOGE("picture is valid!");
        return { 0 };
    }

    auto offset = cmdList.AddImageData(data->GetData(), data->GetSize());
    return { offset, data->GetSize() };
}

std::shared_ptr<Picture> CmdListHelper::GetPictureFromCmdList(const CmdList& cmdList, const ImageHandle& pictureHandle)
{
    const void* ptr = cmdList.GetImageData(pictureHandle.offset);
    if (ptr == nullptr) {
        LOGE("get picture data failed!");
        return nullptr;
    }

    auto pictureData = std::make_shared<Data>();
    pictureData->BuildWithoutCopy(ptr, pictureHandle.size);
    auto picture = std::make_shared<Picture>();
    if (picture->GetImpl<SkiaPicture>()->Deserialize(pictureData) == false) {
        LOGE("picture deserialize failed!");
        return nullptr;
    }
    return picture;
}

ImageHandle CmdListHelper::AddCompressDataToCmdList(CmdList& cmdList, const std::shared_ptr<Data>& data)
{
    if (data == nullptr || data->GetSize() == 0) {
        LOGE("data is valid!");
        return { 0 };
    }

    auto offset = cmdList.AddImageData(data->GetData(), data->GetSize());
    return { offset, data->GetSize() };
}

std::shared_ptr<Data> CmdListHelper::GetCompressDataFromCmdList(const CmdList& cmdList, const ImageHandle& imageHandle)
{
    const void* ptr = cmdList.GetImageData(imageHandle.offset);
    if (imageHandle.size == 0 || ptr == nullptr) {
        LOGE("get image data failed!");
        return nullptr;
    }

    auto imageData = std::make_shared<Data>();
    imageData->BuildWithoutCopy(ptr, imageHandle.size);
    return imageData;
}

CmdListHandle CmdListHelper::AddChildToCmdList(CmdList& cmdList, const std::shared_ptr<CmdList>& child)
{
    CmdListHandle childHandle = { 0 };

    if (child == nullptr) {
        LOGE("child is invalid!");
        return childHandle;
    }

    childHandle.type = child->GetType();

    auto childData = child->GetData();
    if (childData.first != nullptr && childData.second != 0) {
        childHandle.offset = cmdList.AddCmdListData(childData);
        childHandle.size = childData.second;
    } else {
        return childHandle;
    }

    auto childImageData = child->GetAllImageData();
    if (childImageData.first != nullptr && childImageData.second != 0) {
        childHandle.imageOffset = cmdList.AddImageData(childImageData.first, childImageData.second);
        childHandle.imageSize = childImageData.second;
    }

    return childHandle;
}

ImageHandle CmdListHelper::AddTextBlobToCmdList(CmdList& cmdList, const TextBlob* textBlob)
{
    if (!textBlob) {
        LOGE("textBlob nullptr, %{public}s, %{public}d", __FUNCTION__, __LINE__);
        return { 0 };
    }
    std::shared_ptr<SkiaTextBlob> skiaTextBlob = textBlob->GetImpl<SkiaTextBlob>();
    if (!skiaTextBlob) {
        LOGE("skiaTextBlob nullptr, %{public}s, %{public}d", __FUNCTION__, __LINE__);
        return { 0 };
    }
    SkSerialProcs serialProcs;
    serialProcs.fTypefaceProc = &SkiaTypeface::SerializeTypeface;
    auto data = skiaTextBlob->Serialize(serialProcs);
    if (!data || data->GetSize() == 0) {
        LOGE("textBlob serialize invalid, %{public}s, %{public}d", __FUNCTION__, __LINE__);
        return { 0 };
    }

    auto offset = cmdList.AddImageData(data->GetData(), data->GetSize());
    return { offset, data->GetSize() };
}

std::shared_ptr<TextBlob> CmdListHelper::GetTextBlobFromCmdList(const CmdList& cmdList,
    const ImageHandle& textBlobHandle)
{
    const void* data = cmdList.GetImageData(textBlobHandle.offset);
    if (!data) {
        LOGE("textBlob data nullptr, %{public}s, %{public}d", __FUNCTION__, __LINE__);
        return nullptr;
    }

    auto textBlobData = std::make_shared<Data>();
    textBlobData->BuildWithoutCopy(data, textBlobHandle.size);
    SkDeserialProcs deserialProcs;
    deserialProcs.fTypefaceProc = &SkiaTypeface::DeserializeTypeface;
    return SkiaTextBlob::Deserialize(textBlobData->GetData(), textBlobData->GetSize(), deserialProcs);
}

ImageHandle CmdListHelper::AddDataToCmdList(CmdList& cmdList, const Data* srcData)
{
    if (!srcData) {
        LOGE("data nullptr, %{public}s, %{public}d", __FUNCTION__, __LINE__);
        return { 0 };
    }

    auto data = srcData->Serialize();
    if (data == nullptr || data->GetSize() == 0) {
        LOGE("srcData is invalid!");
        return { 0 };
    }

    auto offset = cmdList.AddImageData(data->GetData(), data->GetSize());
    return { offset, data->GetSize() };
}

std::shared_ptr<Data> CmdListHelper::GetDataFromCmdList(const CmdList& cmdList, const ImageHandle& imageHandle)
{
    const void* ptr = cmdList.GetImageData(imageHandle.offset);
    if (imageHandle.size == 0 || ptr == nullptr) {
        LOGE("get data failed!");
        return nullptr;
    }

    auto imageData = std::make_shared<Data>();
    imageData->BuildWithoutCopy(ptr, imageHandle.size);
    return imageData;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
