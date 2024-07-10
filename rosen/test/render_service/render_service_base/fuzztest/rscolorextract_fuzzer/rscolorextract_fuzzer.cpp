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

#include "rscolorextract_fuzzer.h"

#include <climits>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <hilog/log.h>
#include <iostream>
#include <memory>
#include <securec.h>
#include <unistd.h>

#include "image/pixmap.h"
#include "render/rs_color_extract.h"

namespace OHOS {
namespace Rosen {
namespace {
const uint8_t* g_data = nullptr;
size_t g_size = 0;
size_t g_pos;
int g_zero = 0;
int g_one = 1;
int g_two = 2;
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
bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    auto pixmap = std::make_shared<Drawing::Pixmap>();
    RSColorExtract rsColorExtract(pixmap);
    uint32_t color = GetData<uint32_t>();
    uint32_t r = GetData<uint32_t>();
    uint32_t g = GetData<uint32_t>();
    uint32_t b = GetData<uint32_t>();
    int lowerIndex = GetData<int>();
    std::pair<uint32_t, uint32_t> a(r, g);
    std::pair<uint32_t, uint32_t> c(r, b);
    RSColorExtract::GetARGB32ColorR(color);
    RSColorExtract::GetARGB32ColorG(color);
    RSColorExtract::GetARGB32ColorB(color);
    rsColorExtract.SetFeatureColorNum(color);
    rsColorExtract.GetNFeatureColors(color);
    rsColorExtract.CalcContrastToWhite();
    rsColorExtract.QuantizePixels(lowerIndex);
    rsColorExtract.CalcGrayMsd();
    RSColorExtract::QuantizedRed(color);
    RSColorExtract::QuantizedGreen(color);
    RSColorExtract::QuantizedBlue(color);
    RSColorExtract::QuantizeFromRGB888(color);
    RSColorExtract::ModifyWordWidth(g_zero, g_one, g_zero);
    RSColorExtract::ApproximateToRGB888(r, g, b);
    RSColorExtract::ApproximateToRGB888(color);
    RSColorExtract::cmp(a, c);
    RSColorExtract::Rgb2Gray(color);
    RSColorExtract::NormalizeRgb(color);
    RSColorExtract::CalcRelativeLum(color);
    return true;
}

bool DoSplitBoxes(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    
    std::shared_ptr<RSColorExtract> extract;
    std::priority_queue<RSColorExtract::VBox, std::vector<RSColorExtract::VBox>, std::less<RSColorExtract::VBox>>
        colorVBoxPriorityQueue;
    std::shared_ptr<Drawing::Pixmap> pixmap = nullptr;
    RSColorExtract colorExtract(pixmap);
    RSColorExtract::VBox colorExtractionBox(g_one, g_zero, &colorExtract);
    colorVBoxPriorityQueue.push(colorExtractionBox);
    colorExtract.SplitBoxes(colorVBoxPriorityQueue, g_zero);
    colorVBoxPriorityQueue.empty();
    extract->SplitBoxes(colorVBoxPriorityQueue, g_two);
    colorVBoxPriorityQueue.empty();

    RSColorExtract::VBox vBox(g_one, g_zero, &rsColorExtract);
    vBox.fitBox();
    vBox.GetVolume();
    vBox.CanSplit();
    vBox.GetColorCount();
    vBox.GetLongestColorDimension();
    vBox.FindSplitPoint();
    vBox.GetAverageColor();
    return true;
}
bool DoGenerateAverageColors(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    
    std::shared_ptr<RSColorExtract> extract;
    std::priority_queue<RSColorExtract::VBox, std::vector<RSColorExtract::VBox>, std::less<RSColorExtract::VBox>>
        colorVBoxPriorityQueue;
    std::shared_ptr<Drawing::Pixmap> pixmap = nullptr;   
    extract->GenerateAverageColors(colorVBoxPriorityQueue).empty();
    RSColorExtract colorExtract(pixmap);
    RSColorExtract::VBox colorExtractionBox(g_one, g_zero, &colorExtract);
    colorVBoxPriorityQueue.push(colorExtractionBox);
    extract->GenerateAverageColors(colorVBoxPriorityQueue).empty();
    return true;
}

} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::DoSomethingInterestingWithMyAPI(data, size);
    OHOS::Rosen::DoSplitBoxes(data, size);
    OHOS::Rosen::DoGenerateAverageColors(data, size);
    return 0;
}
