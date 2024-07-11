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

#include "rslineargradientblurfilter_fuzzer.h"

#include <climits>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <hilog/log.h>
#include <securec.h>
#include <unistd.h>

#include "skia_adapter/skia_image.h"

#include "draw/canvas.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "render/rs_gradient_blur_para.h"
#include "render/rs_linear_gradient_blur_filter.h"
namespace OHOS {
namespace Rosen {
using namespace Drawing;

namespace {
const uint8_t* g_data = nullptr;
size_t g_size = 0;
size_t g_pos;
float g_zero = 0.0f;
float g_one = 1.0f;
float g_value = 0.1f;
float g_width = 100;
int g_number0 = 0;
int g_number1 = 1;
int g_number3 = 3;
int g_number5 = 5;
int g_number6 = 6;
int g_number7 = 7;
int g_number8 = 8;
int g_number10 = 10;
int g_number16 = 16;
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

bool DoOtherFunc(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::vector<std::pair<float, float>> fractionStops;
    fractionStops.push_back(std::make_pair(g_zero, g_zero));
    fractionStops.push_back(std::make_pair(g_one, g_one));
    int value = GetData<int>();
    float blurRadius = GetData<float>();
    GradientDirection direction = (GradientDirection)value;
    std::shared_ptr<RSLinearGradientBlurPara> linearGradientBlurPara =
        std::make_shared<RSLinearGradientBlurPara>(blurRadius, fractionStops, direction);
    auto filter = std::make_shared<RSLinearGradientBlurFilter>(linearGradientBlurPara, g_width, g_width);

    filter->GetDescription();
    filter->GetDetailedDescription();
    RSLinearGradientBlurFilter::MakeHorizontalMeanBlurEffect();
    RSLinearGradientBlurFilter::MakeVerticalMeanBlurEffect();
    return true;
}
bool DoDrawImageRect(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::vector<std::pair<float, float>> fractionStops;
    fractionStops.push_back(std::make_pair(g_zero, g_zero));
    fractionStops.push_back(std::make_pair(g_one, g_one));
    std::shared_ptr<RSLinearGradientBlurPara> linearGradientBlurPara =
        std::make_shared<RSLinearGradientBlurPara>(g_number16, fractionStops, GradientDirection::BOTTOM);
    auto filter = std::make_shared<RSLinearGradientBlurFilter>(linearGradientBlurPara, g_width, g_width);
    Drawing::Canvas canvas;
    Drawing::Rect src;
    Drawing::Rect dst;
    std::shared_ptr<Drawing::Image> image;
    filter->DrawImageRect(canvas, image, src, dst);
    linearGradientBlurPara->direction_ = GradientDirection::NONE;
    image = std::make_shared<Drawing::Image>();
    filter->DrawImageRect(canvas, image, src, dst);
    return true;
}
bool DoPreProcess(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::vector<std::pair<float, float>> fractionStops;
    fractionStops.push_back(std::make_pair(g_zero, g_zero));
    fractionStops.push_back(std::make_pair(g_one, g_one));
    int value = GetData<int>();
    float blurRadius = GetData<float>();
    GradientDirection direction = (GradientDirection)value;
    std::shared_ptr<RSLinearGradientBlurPara> linearGradientBlurPara =
        std::make_shared<RSLinearGradientBlurPara>(blurRadius, fractionStops, direction);
    auto filter = std::make_shared<RSLinearGradientBlurFilter>(linearGradientBlurPara, g_width, g_width);

    auto image = std::make_shared<Image>();
    filter->PreProcess(image);
    return true;
}
bool DoCompose(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::vector<std::pair<float, float>> fractionStops;
    fractionStops.push_back(std::make_pair(g_zero, g_zero));
    fractionStops.push_back(std::make_pair(g_one, g_one));
    int value = GetData<int>();
    float blurRadius = GetData<float>();
    GradientDirection direction = (GradientDirection)value;
    std::shared_ptr<RSLinearGradientBlurPara> linearGradientBlurPara =
        std::make_shared<RSLinearGradientBlurPara>(blurRadius, fractionStops, direction);
    auto filter = std::make_shared<RSLinearGradientBlurFilter>(linearGradientBlurPara, g_width, g_width);
    std::shared_ptr<RSDrawingFilterOriginal> other = nullptr;
    filter->Compose(other);
    return true;
}

bool DoSetGeometry(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::vector<std::pair<float, float>> fractionStops;
    fractionStops.push_back(std::make_pair(g_zero, g_zero));
    fractionStops.push_back(std::make_pair(g_one, g_one));
    int value = GetData<int>();
    float blurRadius = GetData<float>();
    GradientDirection direction = (GradientDirection)value;
    std::shared_ptr<RSLinearGradientBlurPara> linearGradientBlurPara =
        std::make_shared<RSLinearGradientBlurPara>(blurRadius, fractionStops, direction);
    auto filter = std::make_shared<RSLinearGradientBlurFilter>(linearGradientBlurPara, g_width, g_width);
    int32_t width1 = GetData<int32_t>();
    int32_t height1 = GetData<int32_t>();
    Canvas canvas(width1, height1);
    float geoWidth = GetData<float>();
    float geoHeight = GetData<float>();
    filter->SetGeometry(canvas, geoWidth, geoHeight);
    return true;
}
bool DoIsOffscreenCanvas(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::vector<std::pair<float, float>> fractionStops;
    fractionStops.push_back(std::make_pair(g_zero, g_zero));
    fractionStops.push_back(std::make_pair(g_one, g_one));
    int value = GetData<int>();
    float blurRadius = GetData<float>();
    GradientDirection direction = (GradientDirection)value;
    std::shared_ptr<RSLinearGradientBlurPara> linearGradientBlurPara =
        std::make_shared<RSLinearGradientBlurPara>(blurRadius, fractionStops, direction);
    auto filter = std::make_shared<RSLinearGradientBlurFilter>(linearGradientBlurPara, g_width, g_width);
    bool isOffscreenCanvas = GetData<bool>();
    filter->IsOffscreenCanvas(isOffscreenCanvas);
    return true;
}

bool DoTransformGradientBlurDirection(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    uint8_t direction = GetData<uint8_t>();
    uint8_t directionBias = GetData<uint8_t>();
    RSLinearGradientBlurFilter::TransformGradientBlurDirection(direction, directionBias);
    direction = g_number5;
    directionBias = g_number6;
    RSLinearGradientBlurFilter::TransformGradientBlurDirection(direction, directionBias);
    directionBias = g_number3;
    RSLinearGradientBlurFilter::TransformGradientBlurDirection(direction, directionBias);
    direction = g_number1;
    RSLinearGradientBlurFilter::TransformGradientBlurDirection(direction, directionBias);
    direction = g_number7;
    directionBias = g_number1;
    RSLinearGradientBlurFilter::TransformGradientBlurDirection(direction, directionBias);
    direction = g_number8;
    directionBias = g_number1;
    RSLinearGradientBlurFilter::TransformGradientBlurDirection(direction, directionBias);
    return true;
}
bool DoComputeScale(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    float width1 = GetData<float>();
    float height1 = GetData<float>();
    bool useMaskAlgorithm = GetData<bool>();
    RSLinearGradientBlurFilter::ComputeScale(width1, height1, useMaskAlgorithm);
    RSLinearGradientBlurFilter::ComputeScale(g_width, g_width, true);
    RSLinearGradientBlurFilter::ComputeScale(g_width, g_width, false);
    RSLinearGradientBlurFilter::ComputeScale(g_number10, g_number10, false);
    return true;
}
bool DoCalcDirectionBias(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    Matrix mat;
    RSLinearGradientBlurFilter::CalcDirectionBias(mat);
    mat.Set(Drawing::Matrix::Index::SKEW_X, -g_number1);
    mat.Set(Drawing::Matrix::Index::SKEW_Y, g_number1);
    RSLinearGradientBlurFilter::CalcDirectionBias(mat);
    mat.Set(Drawing::Matrix::Index::SCALE_X, -g_number1);
    mat.Set(Drawing::Matrix::Index::SCALE_Y, -g_number1);
    RSLinearGradientBlurFilter::CalcDirectionBias(mat);
    mat.Set(Drawing::Matrix::Index::SKEW_X, g_number1);
    mat.Set(Drawing::Matrix::Index::SKEW_Y, -g_number1);
    RSLinearGradientBlurFilter::CalcDirectionBias(mat);
    return true;
}
bool DoGetGradientDirectionPoints(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    Point points[2];
    float x = GetData<float>();
    float y = GetData<float>();
    float z = GetData<float>();
    float k = GetData<float>();
    Point point1(x, y);
    Point point2(z, k);
    points[0] = point1;
    points[1] = point2;
    int l = GetData<int>();
    int t = GetData<int>();
    int r = GetData<int>();
    int b = GetData<int>();
    RectI value(l, t, r, b);
    Rect clipBounds(value);
    int dir = GetData<int>();
    GradientDirection direction = (GradientDirection)dir;
    RSLinearGradientBlurFilter::GetGradientDirectionPoints(points, clipBounds, direction);
    RSLinearGradientBlurFilter::GetGradientDirectionPoints(points, clipBounds, GradientDirection::BOTTOM);
    RSLinearGradientBlurFilter::GetGradientDirectionPoints(points, clipBounds, GradientDirection::TOP);
    RSLinearGradientBlurFilter::GetGradientDirectionPoints(points, clipBounds, GradientDirection::RIGHT);
    RSLinearGradientBlurFilter::GetGradientDirectionPoints(points, clipBounds, GradientDirection::LEFT);
    RSLinearGradientBlurFilter::GetGradientDirectionPoints(points, clipBounds, GradientDirection::RIGHT_BOTTOM);
    RSLinearGradientBlurFilter::GetGradientDirectionPoints(points, clipBounds, GradientDirection::LEFT_TOP);
    RSLinearGradientBlurFilter::GetGradientDirectionPoints(points, clipBounds, GradientDirection::LEFT_BOTTOM);
    RSLinearGradientBlurFilter::GetGradientDirectionPoints(points, clipBounds, GradientDirection::RIGHT_TOP);
    RSLinearGradientBlurFilter::GetGradientDirectionPoints(points, clipBounds, GradientDirection::LEFT);
    return true;
}
bool DoMakeAlphaGradientShader(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::vector<std::pair<float, float>> fractionStops;
    std::shared_ptr<RSLinearGradientBlurPara> linearGradientBlurPara =
        std::make_shared<RSLinearGradientBlurPara>(g_number16, fractionStops, GradientDirection::BOTTOM);
    auto filter = std::make_shared<RSLinearGradientBlurFilter>(linearGradientBlurPara, g_width, g_width);
    Drawing::Rect clipBounds = { g_one, g_one, g_one, g_one };
    filter->MakeAlphaGradientShader(clipBounds, linearGradientBlurPara, g_number1);
    linearGradientBlurPara->fractionStops_.push_back(std::make_pair(g_one, g_one));
    linearGradientBlurPara->fractionStops_.push_back(std::make_pair(g_value, g_value));
    filter->MakeAlphaGradientShader(clipBounds, linearGradientBlurPara, g_number1);
    filter->MakeAlphaGradientShader(clipBounds, linearGradientBlurPara, g_number0);
    return true;
}
bool DoDrawMaskLinearGradientBlur(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::vector<std::pair<float, float>> fractionStops;
    fractionStops.push_back(std::make_pair(g_zero, g_zero));
    fractionStops.push_back(std::make_pair(g_one, g_one));
    int value = GetData<int>();
    float blurRadius = GetData<float>();
    GradientDirection direction = (GradientDirection)value;
    std::shared_ptr<RSLinearGradientBlurPara> linearGradientBlurPara =
        std::make_shared<RSLinearGradientBlurPara>(blurRadius, fractionStops, direction);
    auto filter = std::make_shared<RSLinearGradientBlurFilter>(linearGradientBlurPara, g_width, g_width);
    std::shared_ptr<Drawing::Image> imagef = nullptr;
    Drawing::Canvas canvas;
    Drawing::Rect dst = { g_one, g_one, g_one, g_one };
    auto image = std::make_shared<Drawing::Image>();
    auto blurFilter =
        std::static_pointer_cast<RSDrawingFilterOriginal>(linearGradientBlurPara->LinearGradientBlurFilter_);
    auto alphaGradientShader = std::make_shared<Drawing::ShaderEffect>();
    RSLinearGradientBlurFilter::DrawMaskLinearGradientBlur(imagef, canvas, blurFilter, alphaGradientShader, dst);
    return true;
}
bool DoMakeMaskLinearGradientBlurShader(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    auto srcImageShader = std::make_shared<ShaderEffect>();
    auto gradientShader = std::make_shared<ShaderEffect>();
    RSLinearGradientBlurFilter::MakeMaskLinearGradientBlurShader(srcImageShader, gradientShader);
    return true;
}
bool DoDrawMeanLinearGradientBlur(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::vector<std::pair<float, float>> fractionStops;
    fractionStops.push_back(std::make_pair(g_zero, g_zero));
    fractionStops.push_back(std::make_pair(g_one, g_one));
    std::shared_ptr<RSLinearGradientBlurPara> linearGradientBlurPara =
        std::make_shared<RSLinearGradientBlurPara>(g_number16, fractionStops, GradientDirection::BOTTOM);
    auto filter = std::make_shared<RSLinearGradientBlurFilter>(linearGradientBlurPara, g_width, g_width);
    Drawing::Canvas canvas = RSPaintFilterCanvas(std::make_shared<Drawing::Canvas>().get());
    canvas.gpuContext_ = std::make_shared<Drawing::GPUContext>();
    Drawing::Rect dst = { g_one, g_one, g_one, g_one };
    std::shared_ptr<Drawing::Image> image = nullptr;
    auto alphaGradientShader = std::make_shared<Drawing::ShaderEffect>();
    filter->DrawMeanLinearGradientBlur(image, canvas, g_zero, alphaGradientShader, dst);
    filter->MakeVerticalMeanBlurEffect();
    filter->MakeHorizontalMeanBlurEffect();
    image = std::make_shared<Drawing::Image>();
    image->imageImplPtr = std::make_shared<Drawing::SkiaImage>();
    filter->DrawMeanLinearGradientBlur(image, canvas, g_zero, alphaGradientShader, dst);
    return true;
}
bool DoDrawImageRectByDDGRGpuApiType(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::vector<std::pair<float, float>> fractionStops;
    fractionStops.push_back(std::make_pair(g_zero, g_zero));
    fractionStops.push_back(std::make_pair(g_one, g_one));
    std::shared_ptr<RSLinearGradientBlurPara> linearGradientBlurPara =
        std::make_shared<RSLinearGradientBlurPara>(g_number16, fractionStops, GradientDirection::BOTTOM);
    auto filter = std::make_shared<RSLinearGradientBlurFilter>(linearGradientBlurPara, g_width, g_width);
    Drawing::Canvas canvas;
    uint8_t directionBias = g_number0;
    Drawing::RectF clipIPadding;
    auto image = std::make_shared<Drawing::Image>();
    linearGradientBlurPara->direction_ = GradientDirection::NONE;
    RSLinearGradientBlurFilter::DrawImageRectByDDGRGpuApiType(
        canvas, directionBias, clipIPadding, image, linearGradientBlurPara);
    linearGradientBlurPara->useMaskAlgorithm_ = false;
    RSLinearGradientBlurFilter::DrawImageRectByDDGRGpuApiType(
        canvas, directionBias, clipIPadding, image, linearGradientBlurPara);
    return true;
}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::DoOtherFunc(data, size);
    OHOS::Rosen::DoDrawImageRect(data, size);
    OHOS::Rosen::DoPreProcess(data, size);
    OHOS::Rosen::DoCompose(data, size);
    OHOS::Rosen::DoSetGeometry(data, size);
    OHOS::Rosen::DoIsOffscreenCanvas(data, size);
    OHOS::Rosen::DoTransformGradientBlurDirection(data, size);
    OHOS::Rosen::DoComputeScale(data, size);
    OHOS::Rosen::DoCalcDirectionBias(data, size);
    OHOS::Rosen::DoGetGradientDirectionPoints(data, size);
    OHOS::Rosen::DoMakeAlphaGradientShader(data, size);
    OHOS::Rosen::DoDrawMaskLinearGradientBlur(data, size);
    OHOS::Rosen::DoMakeMaskLinearGradientBlurShader(data, size);
    OHOS::Rosen::DoDrawMeanLinearGradientBlur(data, size);
    OHOS::Rosen::DoDrawImageRectByDDGRGpuApiType(data, size);
    return 0;
}
