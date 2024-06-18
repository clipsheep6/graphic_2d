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
#include "pipeline/rs_draw_cmd.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "render/rs_shader.h"
#include "recording/mask_cmd_list.h"
#include "transaction/rs_marshalling_helper.h"

namespace OHOS {
namespace Rosen {
using namespace Drawing;
auto rsMarshallingHelper = std::make_shared<RSMarshallingHelper>();

namespace {
const uint8_t* g_data = nullptr;
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
    std::shared_ptr<Typeface> val = Typeface::MakeDefault();
    rsMarshallingHelper->Unmarshalling(parcel, val);
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
    rsMarshallingHelper->Unmarshalling(parcel, dataVal);
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
    auto dataVal = std::make_shared<Data>();
    rsMarshallingHelper->UnmarshallingWithCopy(parcel, dataVal);
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
    auto colorSpace = std::make_shared<ColorSpace>();
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
    void* imagepixelAddr = nullptr;
    auto image = std::make_shared<Image>();
    rsMarshallingHelper->UnmarshallingNoLazyGeneratedImage(parcel, image, imagepixelAddr);
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
    auto image = std::make_shared<Image>();
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
    void* imagepixelAddr = nullptr;
    auto image = std::make_shared<Image>();
    rsMarshallingHelper->Unmarshalling(parcel, image, imagepixelAddr);
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
    auto shader = std::make_shared<RSShader>();
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

    Parcel parcel;
    Matrix matrix;
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
    auto drawCmdList = std::make_shared<DrawCmdList>();
    rsMarshallingHelper->Unmarshalling(parcel, drawCmdList);
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
    auto extendImageObject = std::make_shared<RSExtendImageObject>();
    rsMarshallingHelper->Unmarshalling(parcel, extendImageObject);
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
    auto extendImageBaseObj = std::make_shared<RSExtendImageBaseObj>();
    rsMarshallingHelper->Unmarshalling(parcel, extendImageBaseObj);
    return true;
}

bool DoMarshallingHelper014(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    Parcel parcel;
    auto maskCmdList = std::make_shared<MaskCmdList>();
    rsMarshallingHelper->Unmarshalling(parcel, maskCmdList);
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
    OHOS::Rosen::DoMarshallingHelper014(data, size);
    return 0;
}