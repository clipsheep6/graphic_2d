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

#include "rscolorpickercachetask_fuzzer.h"

#include <climits>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <memory>
#include <securec.h>
#include <unistd.h>

#include "draw/canvas.h"
#include "draw/surface.h"
#include "image/gpu_context.h"
#include "property/rs_color_picker_cache_task.h"
#include "utils/rect.h"
namespace OHOS {
namespace Rosen {
// using namespace Drawing;

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

bool DoInitSurface(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    auto colorPicker = std::make_shared<RSColorPickerCacheTask>();
    Drawing::GPUContext grContext;
    int value = GetData<int>();
    int16_t red = GetData<int16_t>();
    int16_t green = GetData<int16_t>();
    int16_t blue = GetData<int16_t>();
    int16_t alpha = GetData<int16_t>();
    int deviceHeight = GetData<int>();
    int deviceWidth = GetData<int>();
    int shadowColorStrategy = GetData<int>();
    RSColor color(red, green, blue, alpha);
    CacheProcessStatus cacheProcessStatus = (CacheProcessStatus)value;
    auto imageSnapshot = std::make_shared<Drawing::Image>();
    auto threadImage = std::make_shared<Drawing::Image>();
    auto dst = std::make_shared<Drawing::Pixmap>();
    colorPicker->SetIsShadow(true);
    colorPicker->InitSurface(&grContext);
    colorPicker->SetStatus(cacheProcessStatus);
    colorPicker->Render();
    colorPicker->InitTask(imageSnapshot);
    colorPicker->GetStatus();
    colorPicker->Notify();
    colorPicker->GetFirstGetColorFinished();
    colorPicker->GetWaitRelease();
    colorPicker->GetColor(color);
    colorPicker->CalculateColorAverage(color);
    colorPicker->GetColorAverage(color);
    colorPicker->SetDeviceSize(deviceWidth, deviceHeight);
    colorPicker->SetShadowColorStrategy(shadowColorStrategy);
    colorPicker->SetWaitRelease(true);
    colorPicker->GetDeviceSize(deviceWidth, deviceHeight);
    RSColorPickerCacheTask::PostPartialColorPickerTask(colorPicker, imageSnapshot);
    return true;
}

} // namespace Rosen
} // namespace OHOS
/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::DoInitSurface(data, size);
    return 0;
}