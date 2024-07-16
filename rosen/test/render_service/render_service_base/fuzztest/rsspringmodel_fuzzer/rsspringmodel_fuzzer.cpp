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

#include "rsspringmodel_fuzzer.h"

#include <climits>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <hilog/log.h>
#include <memory>
#include <securec.h>
#include <unistd.h>

#include "animation/rs_spring_model.h"
#include "common/rs_color.h"
#include "common/rs_matrix3.h"
#include "common/rs_rect.h"
#include "modifier/rs_render_property.h"
#include "render/rs_filter.h"

namespace OHOS {
namespace Rosen {
namespace {
const uint8_t* g_data = nullptr;
size_t g_size = 0;
size_t g_pos;
float g_zero = 0.0f;
float g_one = 1.0f;
float g_two = 2.0f;
int g_number0 = 0;
int g_number255 = 255;
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

    float initialOffset = g_one;
    float initialVelocity = g_one;

    auto model1 = std::make_shared<RSSpringModel<float>>(g_zero, g_zero, initialOffset, initialVelocity, g_zero);
    auto result = model1->CalculateDisplacement(g_one);
    auto duration = model1->EstimateDuration();

    auto model2 = std::make_shared<RSSpringModel<float>>(g_one, g_one, initialOffset, initialVelocity, g_one);
    result = model2->CalculateDisplacement(g_one);
    duration = model2->EstimateDuration();

    auto model3 = std::make_shared<RSSpringModel<float>>(g_one, g_two, initialOffset, initialVelocity, g_one);
    result = model3->CalculateDisplacement(g_one);
    duration = model3->EstimateDuration();

    return true;
}
bool DoRSSpringModelColorTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    Color initialOffset = Color(g_number0, g_number0, g_number0);
    Color initialVelocity = Color(g_number255, g_number255, g_number255);

    auto model1 = std::make_shared<RSSpringModel<Color>>(g_zero, g_zero, initialOffset, initialVelocity, g_zero);
    auto result = model1->CalculateDisplacement(g_one);
    auto duration = model1->EstimateDuration();

    auto model2 = std::make_shared<RSSpringModel<Color>>(g_one, g_one, initialOffset, initialVelocity, g_one);
    result = model2->CalculateDisplacement(g_one);
    duration = model2->EstimateDuration();

    auto model3 = std::make_shared<RSSpringModel<Color>>(g_one, g_two, initialOffset, initialVelocity, g_one);
    result = model3->CalculateDisplacement(g_one);
    duration = model3->EstimateDuration();

    return true;
}
bool DoRSSpringModelMatrix3fTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    Matrix3f initialOffset = Matrix3f::ZERO;
    Matrix3f initialVelocity = Matrix3f::IDENTITY;

    auto model1 = std::make_shared<RSSpringModel<Matrix3f>>(g_zero, g_zero, initialOffset, initialVelocity, g_zero);
    auto result = model1->CalculateDisplacement(g_one);
    auto duration = model1->EstimateDuration();

    auto model2 = std::make_shared<RSSpringModel<Matrix3f>>(g_one, g_one, initialOffset, initialVelocity, g_one);
    result = model2->CalculateDisplacement(g_one);
    duration = model2->EstimateDuration();

    auto model3 = std::make_shared<RSSpringModel<Matrix3f>>(g_one, g_two, initialOffset, initialVelocity, g_one);
    result = model3->CalculateDisplacement(g_one);
    duration = model3->EstimateDuration();

    return true;
}
bool DoRSSpringModelVector2fTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    Vector2f initialOffset = Vector2f(g_zero, g_zero);
    Vector2f initialVelocity = Vector2f(g_one, g_one);

    auto model1 = std::make_shared<RSSpringModel<Vector2f>>(g_zero, g_zero, initialOffset, initialVelocity, g_zero);
    auto result = model1->CalculateDisplacement(g_one);
    auto duration = model1->EstimateDuration();

    auto model2 = std::make_shared<RSSpringModel<Vector2f>>(g_one, g_one, initialOffset, initialVelocity, g_one);
    result = model2->CalculateDisplacement(g_one);
    duration = model2->EstimateDuration();

    auto model3 = std::make_shared<RSSpringModel<Vector2f>>(g_one, g_two, initialOffset, initialVelocity, g_one);
    result = model3->CalculateDisplacement(g_one);
    duration = model3->EstimateDuration();

    return true;
}

bool DoRSSpringModelVector4fTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    Vector4f initialOffset = Vector4f(g_zero, g_zero, g_zero, g_zero);
    Vector4f initialVelocity = Vector4f(g_one, g_one, g_one, g_one);

    auto model1 = std::make_shared<RSSpringModel<Vector4f>>(g_zero, g_zero, initialOffset, initialVelocity, g_zero);
    auto result = model1->CalculateDisplacement(g_one);
    auto duration = model1->EstimateDuration();

    auto model2 = std::make_shared<RSSpringModel<Vector4f>>(g_one, g_one, initialOffset, initialVelocity, g_one);
    result = model2->CalculateDisplacement(g_one);
    duration = model2->EstimateDuration();

    auto model3 = std::make_shared<RSSpringModel<Vector4f>>(g_one, g_two, initialOffset, initialVelocity, g_one);
    result = model3->CalculateDisplacement(g_one);
    duration = model3->EstimateDuration();

    return true;
}
bool DoRSSpringModelQuaternionTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    Quaternion initialOffset = Quaternion(g_zero, g_zero, g_zero, g_zero);
    Quaternion initialVelocity = Quaternion(g_one, g_one, g_one, g_one);

    auto model1 = std::make_shared<RSSpringModel<Quaternion>>(g_zero, g_zero, initialOffset, initialVelocity, g_zero);
    auto result = model1->CalculateDisplacement(g_one);
    auto duration = model1->EstimateDuration();

    auto model2 = std::make_shared<RSSpringModel<Quaternion>>(g_one, g_one, initialOffset, initialVelocity, g_one);
    result = model2->CalculateDisplacement(g_one);
    duration = model2->EstimateDuration();

    auto model3 = std::make_shared<RSSpringModel<Quaternion>>(g_one, g_two, initialOffset, initialVelocity, g_one);
    result = model3->CalculateDisplacement(g_one);
    duration = model3->EstimateDuration();
    return true;
}
bool DoRSSpringModelVector4ColorTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    Vector4<Color> initialOffset =
        Vector4<Color>(Color(g_number0, g_number0, g_number0), Color(g_number0, g_number0, g_number0),
            Color(g_number0, g_number0, g_number0), Color(g_number0, g_number0, g_number0));
    Vector4<Color> initialVelocity =
        Vector4<Color>(Color(g_number255, g_number255, g_number255), Color(g_number255, g_number255, g_number255),
            Color(g_number255, g_number255, g_number255), Color(g_number255, g_number255, g_number255));
    auto model1 =
        std::make_shared<RSSpringModel<Vector4<Color>>>(g_zero, g_zero, initialOffset, initialVelocity, g_zero);
    auto result = model1->CalculateDisplacement(g_one);
    auto duration = model1->EstimateDuration();

    auto model2 = std::make_shared<RSSpringModel<Vector4<Color>>>(g_one, g_one, initialOffset, initialVelocity, g_one);
    result = model2->CalculateDisplacement(g_one);
    duration = model2->EstimateDuration();

    auto model3 = std::make_shared<RSSpringModel<Vector4<Color>>>(g_one, g_two, initialOffset, initialVelocity, g_one);
    result = model3->CalculateDisplacement(g_one);
    duration = model3->EstimateDuration();
    return true;
}
bool DoRSSpringModelRSFilterTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    auto model1 = std::make_shared<RSSpringModel<std::shared_ptr<RSFilter>>>(g_one, g_two, nullptr, nullptr, g_one);
    auto result = model1->CalculateDisplacement(g_one);
    auto duration = model1->EstimateDuration();

    auto initialOffset = RSFilter::CreateBlurFilter(g_zero, g_zero);
    auto initialVelocity = RSFilter::CreateBlurFilter(g_zero, g_zero);

    auto model2 = std::make_shared<RSSpringModel<std::shared_ptr<RSFilter>>>(
        g_zero, g_zero, initialOffset, initialVelocity, g_zero);
    result = model2->CalculateDisplacement(g_one);
    duration = model2->EstimateDuration();

    auto model3 =
        std::make_shared<RSSpringModel<std::shared_ptr<RSFilter>>>(g_one, g_one, initialOffset, initialVelocity, g_one);
    result = model3->CalculateDisplacement(g_one);
    duration = model3->EstimateDuration();

    auto model4 =
        std::make_shared<RSSpringModel<std::shared_ptr<RSFilter>>>(g_one, g_two, initialOffset, initialVelocity, g_one);
    result = model4->CalculateDisplacement(g_one);
    duration = model4->EstimateDuration();
    return true;
}
bool DoRSSpringModelRSRSRenderPropertyBaseTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    auto initialOffset = std::make_shared<RSRenderAnimatableProperty<float>>(g_zero);
    auto initialVelocity = std::make_shared<RSRenderAnimatableProperty<float>>(g_one);

    auto model1 = std::make_shared<RSSpringModel<std::shared_ptr<RSRenderPropertyBase>>>(
        g_zero, g_zero, initialOffset, initialVelocity, g_zero);
    auto result = model1->CalculateDisplacement(g_one);
    auto duration = model1->EstimateDuration();

    auto model2 = std::make_shared<RSSpringModel<std::shared_ptr<RSRenderPropertyBase>>>(
        g_one, g_one, initialOffset, initialVelocity, g_one);
    result = model2->CalculateDisplacement(g_one);
    duration = model2->EstimateDuration();

    auto model3 = std::make_shared<RSSpringModel<std::shared_ptr<RSRenderPropertyBase>>>(
        g_one, g_two, initialOffset, initialVelocity, g_one);
    result = model3->CalculateDisplacement(g_one);
    duration = model3->EstimateDuration();
    return true;
}
bool DoRSSpringModelRRectTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    RectF rect1;
    rect1.SetAll(g_zero, g_zero, g_zero, g_zero);
    RectF rect2;
    rect2.SetAll(g_one, g_one, g_one, g_one);
    RRect initialOffset(rect1, g_zero, g_zero);
    RRect initialVelocity(rect2, g_one, g_one);

    auto model1 = std::make_shared<RSSpringModel<RRect>>(g_zero, g_zero, initialOffset, initialVelocity, g_zero);
    auto result = model1->CalculateDisplacement(g_one);
    auto duration = model1->EstimateDuration();

    auto model2 = std::make_shared<RSSpringModel<RRect>>(g_one, g_one, initialOffset, initialVelocity, g_one);
    result = model2->CalculateDisplacement(g_one);
    duration = model2->EstimateDuration();

    auto model3 = std::make_shared<RSSpringModel<RRect>>(g_one, g_two, initialOffset, initialVelocity, g_one);
    result = model3->CalculateDisplacement(g_one);
    duration = model3->EstimateDuration();
    return true;
}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::DoSomethingInterestingWithMyAPI(data, size);
    OHOS::Rosen::DoRSSpringModelColorTest(data, size);
    OHOS::Rosen::DoRSSpringModelMatrix3fTest(data, size);
    OHOS::Rosen::DoRSSpringModelVector2fTest(data, size);
    OHOS::Rosen::DoRSSpringModelVector4fTest(data, size);
    OHOS::Rosen::DoRSSpringModelQuaternionTest(data, size);
    OHOS::Rosen::DoRSSpringModelVector4ColorTest(data, size);
    OHOS::Rosen::DoRSSpringModelRSFilterTest(data, size);
    OHOS::Rosen::DoRSSpringModelRSRSRenderPropertyBaseTest(data, size);
    OHOS::Rosen::DoRSSpringModelRRectTest(data, size);
    return 0;
}