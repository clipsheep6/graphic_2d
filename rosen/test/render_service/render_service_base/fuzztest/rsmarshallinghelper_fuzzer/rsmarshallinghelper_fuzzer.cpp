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

#include "rsmarshallinghelper_fuzzer.h"

#include <climits>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <hilog/log.h>
#include <parcel.h>
#include <securec.h>
#include <unistd.h>

#include "draw/canvas.h"
#include "recording/mask_cmd_list.h"
#include "pipeline/rs_draw_cmd.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "render/rs_shader.h"
#include "transaction/rs_marshalling_helper.h"
#include "platform/image_native/pixel_map.h"



namespace OHOS {
namespace Rosen {
using namespace Drawing;
auto rsMarshallingHelper = std::make_shared<RSMarshallingHelper>();

namespace {
const uint8_t* g_data = nullptr;
constexpr uint32_t MAX_ARRAY_SIZE = 5000;
constexpr size_t FUNCTYPE_SIZE = 4;
constexpr size_t MATRIXTYPE_SIZE = 5;
size_t g_size = 0;
size_t g_pos;
} // namespace

template<class T>
T GetData()
{
    T object {};
    size_t objectSize = sizeof(object);
    if (g_data == nullptr || objectSize > g_size - g_pos) {
        return object;
    }
    errno_t ret = memcpy_s(&object, objectSize, g_data + g_pos, objectSize);
    if (ret != EOK) {
        return {};
    }
    g_pos += objectSize;
    return object;
}

bool DoMarshallingHelper001(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    Parcel parcel;
    size_t length = GetData<size_t>() % MAX_ARRAY_SIZE;
    char* path = new char[length];
    for (size_t i = 0; i < length; i++) {
        path[i] = GetData<char>();
    }
    std::shared_ptr<Typeface> val = Typeface::MakeFromFile(path, length);
    rsMarshallingHelper->Marshalling(parcel, val);
    rsMarshallingHelper->Unmarshalling(parcel, val);
    if (path != nullptr) {
        delete [] path;
        path = nullptr;
    }
    return true;
}
bool DoMarshallingHelper002(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    Parcel parcel;
    auto dataVal = std::make_shared<Data>();
    size_t length = GetData<size_t>() % MAX_ARRAY_SIZE;
    char* dataText = new char[length];
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetData<char>();
    }
    dataVal->BuildWithoutCopy(dataText, length);
    rsMarshallingHelper->Marshalling(parcel, dataVal);
    rsMarshallingHelper->Unmarshalling(parcel, dataVal);
    rsMarshallingHelper->UnmarshallingWithCopy(parcel, dataVal);
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
    return true;
}

bool DoMarshallingHelper003(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    Parcel parcel;
    size_t length = GetData<size_t>() % MAX_ARRAY_SIZE;
    char* dataText = new char[length];
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetData<char>();
    }
    std::pair<const void*, size_t> cmdListData;
    cmdListData.first = static_cast<const void*>(dataText);
    cmdListData.second = length;
    bool isCopy = GetData<bool>();
    static std::shared_ptr<MaskCmdList> maskCmdList = MaskCmdList::CreateFromData(cmdListData, isCopy);
    rsMarshallingHelper->Marshalling(parcel, maskCmdList);
    rsMarshallingHelper->Unmarshalling(parcel, maskCmdList);
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
    return true;
}

bool DoMarshallingHelper004(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    Parcel parcel;
    Bitmap bitmap;
    int32_t width = static_cast<int>(data[0]);
    int32_t height = static_cast<int>(data[1]);
    BitmapFormat bitmapFormat = { COLORTYPE_ARGB_4444, ALPHATYPE_OPAQUE };
    bitmap.Build(width, height, bitmapFormat);
    rsMarshallingHelper->Marshalling(parcel, bitmap);
    rsMarshallingHelper->Unmarshalling(parcel, bitmap);
    return true;
}

bool DoMarshallingHelper005(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    Parcel parcel;
    // static RSB_EXPORT bool ReadColorSpaceFromParcel(Parcel& parcel, std::shared_ptr<Drawing::ColorSpace>& colorSpace);
    uint32_t funcType = GetData<uint32_t>();
    uint32_t matrixType = GetData<uint32_t>();
    std::shared_ptr<ColorSpace> colorSpace = ColorSpace::CreateRGB(
        static_cast<CMSTransferFuncType>(funcType % FUNCTYPE_SIZE),
        static_cast<CMSMatrixType>(matrixType % MATRIXTYPE_SIZE));
    rsMarshallingHelper->ReadColorSpaceFromParcel(parcel, colorSpace);
    return true;
}

bool DoMarshallingHelper006(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    Parcel parcel;
    // static RSB_EXPORT bool UnmarshallingNoLazyGeneratedImage(Parcel& parcel, std::shared_ptr<Drawing::Image>& val, void*& imagepixelAddr);
    void* imagePixelPtr = nullptr;
    Bitmap bitmap;
    int32_t width = static_cast<int32_t>(data[0]);
    int32_t height = static_cast<int32_t>(data[1]);
    BitmapFormat bitmapFormat = { COLORTYPE_ARGB_4444, ALPHATYPE_OPAQUE };
    bitmap.Build(width, height, bitmapFormat);
    auto image = std::make_shared<Image>();
    image->BuildFromBitmap(bitmap);
    rsMarshallingHelper->Marshalling(parcel, image);
    rsMarshallingHelper->UnmarshallingNoLazyGeneratedImage(parcel, image, imagePixelPtr);
    return true;
}

bool DoMarshallingHelper007(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    Parcel parcel;
    Bitmap bitmap;
    int width = static_cast<int>(data[0]);
    int height = static_cast<int>(data[1]);
    BitmapFormat bitmapFormat = { COLORTYPE_ARGB_4444, ALPHATYPE_OPAQUE };
    bitmap.Build(width, height, bitmapFormat);
    auto image = std::make_shared<Image>();
    image->BuildFromBitmap(bitmap);
    rsMarshallingHelper->Marshalling(parcel, image);
    rsMarshallingHelper->Unmarshalling(parcel, image);
    return true;
}

bool DoMarshallingHelper008(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    Parcel parcel;
    // static RSB_EXPORT bool Unmarshalling(Parcel& parcel, std::shared_ptr<Drawing::Image>& val, void*& imagepixelAddr);
    void* imagePixelPtr = nullptr;
    Bitmap bitmap;
    int32_t width = static_cast<int32_t>(data[0]);
    int32_t height = static_cast<int32_t>(data[1]);
    BitmapFormat bitmapFormat = { COLORTYPE_ARGB_4444, ALPHATYPE_OPAQUE };
    bitmap.Build(width, height, bitmapFormat);
    auto image = std::make_shared<Image>();
    image->BuildFromBitmap(bitmap);
    rsMarshallingHelper->Marshalling(parcel, image);
    rsMarshallingHelper->Unmarshalling(parcel, image, imagePixelPtr);
    return true;
}

bool DoMarshallingHelper009(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    Parcel parcel;
    // static RSB_EXPORT bool Unmarshalling(Parcel& parcel, std::shared_ptr<RSShader>& val);
    // static std::shared_ptr<ShaderEffect> CreateColorShader(ColorQuad color);
    // static std::shared_ptr<RSShader> CreateRSShader(const std::shared_ptr<Drawing::ShaderEffect>& drShader);

    ColorQuad color = GetData<ColorQuad>();
    std::shared_ptr<ShaderEffect> shaderEffect = ShaderEffect::CreateColorShader(color);
    std::shared_ptr<RSShader> shader = RSShader::CreateRSShader(shaderEffect);
    // auto shader = std::make_shared<RSShader>();
    // shader->CreateRSShader(nullptr);

    rsMarshallingHelper->Marshalling(parcel, shader);
    rsMarshallingHelper->Unmarshalling(parcel, shader);
    return true;
}

bool DoMarshallingHelper010(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    Parcel parcel;
    // static RSB_EXPORT bool Unmarshalling(Parcel& parcel, Drawing::Matrix& val);
    Matrix matrix;
    scalar scaleX = GetData<scalar>();
    scalar skewX = GetData<scalar>();
    scalar transX = GetData<scalar>();
    scalar skewY = GetData<scalar>();
    scalar scaleY = GetData<scalar>();
    scalar transY = GetData<scalar>();
    scalar persp0 = GetData<scalar>();
    scalar persp1 = GetData<scalar>();
    scalar persp2 = GetData<scalar>();
    matrix.SetMatrix(scaleX, skewX, transX, skewY, scaleY, transY, persp0, persp1, persp2);
    rsMarshallingHelper->Marshalling(parcel, matrix);
    rsMarshallingHelper->Unmarshalling(parcel, matrix);
    return true;
}

bool DoMarshallingHelper011(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    Parcel parcel;
    size_t length = GetData<size_t>() % MAX_ARRAY_SIZE;
    char* dataText = new char[length];
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetData<char>();
    }
    std::pair<const void*, size_t> cmdListData;
    cmdListData.first = static_cast<const void*>(dataText);
    cmdListData.second = length;
    bool isCopy = GetData<bool>();
    static std::shared_ptr<DrawCmdList> drawCmdList = DrawCmdList::CreateFromData(cmdListData, isCopy);
    rsMarshallingHelper->Marshalling(parcel, drawCmdList);
    rsMarshallingHelper->Unmarshalling(parcel, drawCmdList);
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
    return true;
    return true;
}

bool DoMarshallingHelper012(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    Parcel parcel;
    Bitmap bitmap;
    auto image = std::make_shared<Image>();
    int32_t width = static_cast<int32_t>(data[0]);
    int32_t height = static_cast<int32_t>(data[1]);
    BitmapFormat bitmapFormat = { COLORTYPE_ARGB_4444, ALPHATYPE_OPAQUE };
    bitmap.Build(width, height, bitmapFormat);
    image->BuildFromBitmap(bitmap);

    auto dataVal = std::make_shared<Data>();
    size_t length = GetData<size_t>() % MAX_ARRAY_SIZE;
    char* dataText = new char[length];
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetData<char>();
    }
    dataVal->BuildWithoutCopy(dataText, length);
    AdaptiveImageInfo adaptiveImageInfo;
    adaptiveImageInfo.fitNum = GetData<int32_t>();
    adaptiveImageInfo.repeatNum = GetData<int32_t>();
    for (int i = 0; i < 4; i++) {
        Point point;
        int32_t x = GetData<int32_t>();
        int32_t y = GetData<int32_t>();
        point.Set(x, y);
        adaptiveImageInfo.radius[i] = point;
    }
    adaptiveImageInfo.scale = GetData<double>();
    adaptiveImageInfo.uniqueId = GetData<uint32_t>();
    adaptiveImageInfo.width = GetData<int32_t>();
    adaptiveImageInfo.height = GetData<int32_t>();
    adaptiveImageInfo.dynamicRangeMode = GetData<uint32_t>();
    auto extendImageObject = std::make_shared<RSExtendImageObject>(image, dataVal, adaptiveImageInfo);
    rsMarshallingHelper->Marshalling(parcel, extendImageObject);
    rsMarshallingHelper->Unmarshalling(parcel, extendImageObject);
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
    return true;
}

bool DoMarshallingHelper013(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    Parcel parcel;
    int32_t left = GetData<int32_t>();
    int32_t top = GetData<int32_t>();
    int32_t right = GetData<int32_t>();
    int32_t bottom = GetData<int32_t>();
    Rect src = {
        left,
        top,
        right,
        bottom,
    };

    int32_t leftT = GetData<int32_t>();
    int32_t topT = GetData<int32_t>();
    int32_t rightT = GetData<int32_t>();
    int32_t bottomT = GetData<int32_t>();
    Rect dst = {
        leftT,
        topT,
        rightT,
        bottomT,
    };
    auto extendImageBaseObj = std::make_shared<RSExtendImageBaseObj>(nullptr, src, dst);
    rsMarshallingHelper->Marshalling(parcel, extendImageBaseObj);
    rsMarshallingHelper->Unmarshalling(parcel, extendImageBaseObj);
    return true;
}

} // namespace Rosen
} // namespace OHOS
/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::DoMarshallingHelper001(data, size);
    OHOS::Rosen::DoMarshallingHelper002(data, size);
    OHOS::Rosen::DoMarshallingHelper003(data, size);
    OHOS::Rosen::DoMarshallingHelper004(data, size);
    OHOS::Rosen::DoMarshallingHelper005(data, size);
    OHOS::Rosen::DoMarshallingHelper006(data, size);
    OHOS::Rosen::DoMarshallingHelper007(data, size);
    OHOS::Rosen::DoMarshallingHelper008(data, size);
    OHOS::Rosen::DoMarshallingHelper009(data, size);
    OHOS::Rosen::DoMarshallingHelper010(data, size);
    OHOS::Rosen::DoMarshallingHelper011(data, size);
    OHOS::Rosen::DoMarshallingHelper012(data, size);
    OHOS::Rosen::DoMarshallingHelper013(data, size);
    return 0;
}