/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.. All rights reserved.
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

#ifndef SKIA_STATIC_FACTORY_H
#define SKIA_STATIC_FACTORY_H

#include <cstdint>

#include "draw/surface.h"
#include "text/text_blob.h"
#include "text/typeface.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaStaticFactory {
public:
    static std::shared_ptr<TextBlob> MakeFromText(const void* text, size_t byteLength,
        const Font& font, TextEncoding encoding);
    static std::shared_ptr<TextBlob> MakeFromRSXform(const void* text, size_t byteLength,
        const RSXform xform[], const Font& font, TextEncoding encoding);
    static std::shared_ptr<Typeface> MakeFromFile(const char path[]);
#ifdef ACE_ENABLE_GPU
    static std::shared_ptr<Surface> MakeRenderTarget(GPUContext* gpuContext, bool budgeted, const ImageInfo& imageInfo);
#endif
    static std::shared_ptr<Surface> MakeRaster(const ImageInfo& imageInfo);
    static std::shared_ptr<Surface> MakeRasterDirect(const ImageInfo& imageInfo, void* pixels, size_t rowBytes);
    static std::shared_ptr<Surface> MakeRasterN32Premul(int32_t width, int32_t height);

    static std::shared_ptr<TextBlob> DeserializeTextBlob(const void* data, size_t size);
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif // SKIA_STATIC_FACTORY_H