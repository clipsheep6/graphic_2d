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

#include "bitmap_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <securec.h>

#include "image/bitmap.h"
#include "draw/color.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr size_t DATA_MIN_SIZE = 2;
} // namespace

namespace Drawing {
bool BitmapFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }
    Bitmap bitmap;
    int width = static_cast<int>(data[0]);
    int height = static_cast<int>(data[1]);
    BitmapFormat bitmapFormat = { COLORTYPE_ARGB_4444, ALPHATYPE_OPAQUE };
    bitmap.Build(width, height, bitmapFormat);
    if (bitmap.GetWidth() != width || bitmap.GetHeight() != height || bitmap.GetPixels() == nullptr) {
        return false;
    }
    bitmap.ClearWithColor(COLORTYPE_UNKNOWN);
    bitmap.Free();
    return true;
}

bool BitmapFuzzTest01(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    Bitmap bitmap;
    BitmapFormat bitmapFormat = { COLORTYPE_ARGB_4444, ALPHATYPE_OPAQUE };
    bitmap.Build(GetObject<ImageInfo>(), GetObject<int32_t>());
    bitmap.GetWidth();
    bitmap.GetHeight();
    bitmap.GetRowBytes();
    bitmap.GetColorType();
    bitmap.GetAlphaType();
    bitmap.ExtractSubset(GetObject<Bitmap>(), GetObject<Rect>());
    bitmap.ReadPixels(GetObject<ImageInfo>(), nullptr, GetObject<size_t>(), GetObject<int32_t>(), GetObject<int32_t>());
    bitmap.PeekPixels(GetObject<Pixmap>());
    bitmap.ComputeByteSize();
    bitmap.GetPixels();
    bitmap.SetPixels(nullptr);
    bitmap.CopyPixels(GetObject<Bitmap>(), GetObject<int>(), GetObject<int>());
    bitmap.InstallPixels(GetObject<ImageInfo>(), nullptr, GetObject<size_t>());

    bitmap.IsImmutable();
    bitmap.SetImmutable();
    bitmap.GetColorType();
    bitmap.IsValid();
    bitmap.GetColor(GetObject<int>(), GetObject<int>());
    bitmap.GetFormat();
    bitmap.SetFormat(GetObject<BitmapFormat>());
    bitmap.SetInfo(GetObject<ImageInfo>());
    bitmap.GetImageInfo();
    bitmap.GetPixmap();
    bitmap.MakeImage();
    bitmap.Serialize();
    bitmap.ClearWithColor(COLORTYPE_UNKNOWN);
    bitmap.Free();
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::BitmapFuzzTest(data, size);
    OHOS::Rosen::Drawing::BitmapFuzzTest01(data, size);
    return 0;
}