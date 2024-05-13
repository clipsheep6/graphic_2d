/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, Hardware
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "gtest/gtest.h"

#include "drawing_image.h"
#include "drawing_matrix.h"
#include "drawing_point.h"
#include "drawing_sampling_options.h"
#include "drawing_shader_effect.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class NativeDrawingShaderEffectTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void NativeDrawingShaderEffectTest::SetUpTestCase() {}
void NativeDrawingShaderEffectTest::TearDownTestCase() {}
void NativeDrawingShaderEffectTest::SetUp() {}
void NativeDrawingShaderEffectTest::TearDown() {}

/*
 * @tc.name: NativeDrawingShaderEffectTest_CreateLinearGradient
 * @tc.desc: test for CreateLinearGradient.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingShaderEffectTest, NativeDrawingShaderEffectTest_CreateLinearGradient, TestSize.Level1)
{
    OH_Drawing_Point* start = OH_Drawing_PointCreate(350.f, 350.f);
    OH_Drawing_Point* end = OH_Drawing_PointCreate(550.f, 550.f);
    uint32_t colors[] = {0xff00ff00, 0xff0000ff};
    float pos[] = {0, 1};
    OH_Drawing_ShaderEffect* shader = OH_Drawing_ShaderEffectCreateLinearGradient(start, end, colors, pos, 2, CLAMP);
    EXPECT_NE(shader, nullptr);
    OH_Drawing_ShaderEffectDestroy(shader);
    shader = OH_Drawing_ShaderEffectCreateLinearGradient(start, end, colors, nullptr, 2, CLAMP);
    EXPECT_NE(shader, nullptr);
    OH_Drawing_ShaderEffectDestroy(shader);
    shader = OH_Drawing_ShaderEffectCreateLinearGradient(nullptr, end, colors, nullptr, 2, CLAMP);
    EXPECT_EQ(shader, nullptr);
    shader = OH_Drawing_ShaderEffectCreateLinearGradient(start, nullptr, colors, nullptr, 2, CLAMP);
    EXPECT_EQ(shader, nullptr);
    shader = OH_Drawing_ShaderEffectCreateLinearGradient(start, end, nullptr, nullptr, 2, CLAMP);
    EXPECT_EQ(shader, nullptr);
    OH_Drawing_PointDestroy(start);
    OH_Drawing_PointDestroy(end);
}

/*
 * @tc.name: NativeDrawingShaderEffectTest_CreateRadialGradient
 * @tc.desc: test for CreateRadialGradient.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingShaderEffectTest, NativeDrawingShaderEffectTest_CreateRadialGradient, TestSize.Level1)
{
    OH_Drawing_Point* center = OH_Drawing_PointCreate(350.f, 350.f);
    uint32_t colors[] = {0xff00ff00, 0xff0000ff};
    float pos[] = {0, 1};
    float radius = 10.f;
    OH_Drawing_ShaderEffect* shader =
        OH_Drawing_ShaderEffectCreateRadialGradient(center, radius, colors, pos, 2, CLAMP);
    EXPECT_NE(shader, nullptr);
    OH_Drawing_ShaderEffectDestroy(shader);
    shader = OH_Drawing_ShaderEffectCreateRadialGradient(center, radius, colors, nullptr, 2, CLAMP);
    EXPECT_NE(shader, nullptr);
    OH_Drawing_ShaderEffectDestroy(shader);
    shader = OH_Drawing_ShaderEffectCreateRadialGradient(nullptr, radius, colors, nullptr, 2, CLAMP);
    EXPECT_EQ(shader, nullptr);
    shader = OH_Drawing_ShaderEffectCreateRadialGradient(center, radius, nullptr, nullptr, 2, CLAMP);
    EXPECT_EQ(shader, nullptr);
    OH_Drawing_PointDestroy(center);
}

/*
 * @tc.name: NativeDrawingShaderEffectTest_CreateSweepGradient
 * @tc.desc: test for CreateSweepGradient
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingShaderEffectTest, NativeDrawingShaderEffectTest_CreateSweepGradient, TestSize.Level1)
{
    OH_Drawing_Point* center = OH_Drawing_PointCreate(350.f, 350.f);
    uint32_t colors[] = {0xff00ff00, 0xff0000ff};
    float pos[] = {0, 1};
    OH_Drawing_ShaderEffect* shader = OH_Drawing_ShaderEffectCreateSweepGradient(center, colors, pos, 2, CLAMP);
    EXPECT_NE(shader, nullptr);
    OH_Drawing_ShaderEffectDestroy(shader);
    shader = OH_Drawing_ShaderEffectCreateSweepGradient(center, colors, nullptr, 2, CLAMP);
    EXPECT_NE(shader, nullptr);
    OH_Drawing_ShaderEffectDestroy(shader);
    shader = OH_Drawing_ShaderEffectCreateSweepGradient(nullptr, colors, pos, 2, CLAMP);
    EXPECT_EQ(shader, nullptr);
    shader = OH_Drawing_ShaderEffectCreateSweepGradient(center, nullptr, pos, 2, CLAMP);
    EXPECT_EQ(shader, nullptr);
    OH_Drawing_PointDestroy(center);
}

/*
 * @tc.name: NativeDrawingShaderEffectTest_CreateImageShader
 * @tc.desc: test for CreateImageShader
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingShaderEffectTest, NativeDrawingShaderEffectTest_CreateImageShader, TestSize.Level1)
{
    OH_Drawing_Image* image = OH_Drawing_ImageCreate();
    OH_Drawing_SamplingOptions* samplingOptions =
        OH_Drawing_SamplingOptionsCreate(FILTER_MODE_NEAREST, MIPMAP_MODE_NONE);
    OH_Drawing_Matrix* matrix = OH_Drawing_MatrixCreate();
    OH_Drawing_ShaderEffect* shader =
        OH_Drawing_ShaderEffectCreateImageShader(image, CLAMP, CLAMP, samplingOptions, matrix);
    EXPECT_NE(shader, nullptr);
    OH_Drawing_ShaderEffectDestroy(shader);
    shader = OH_Drawing_ShaderEffectCreateImageShader(image, CLAMP, CLAMP, samplingOptions, nullptr);
    EXPECT_NE(shader, nullptr);
    OH_Drawing_ShaderEffectDestroy(shader);
    shader = OH_Drawing_ShaderEffectCreateImageShader(nullptr, CLAMP, CLAMP, samplingOptions, nullptr);
    EXPECT_EQ(shader, nullptr);
    shader = OH_Drawing_ShaderEffectCreateImageShader(image, CLAMP, CLAMP, nullptr, nullptr);
    EXPECT_EQ(shader, nullptr);
    OH_Drawing_ImageDestroy(image);
    OH_Drawing_SamplingOptionsDestroy(samplingOptions);
    OH_Drawing_MatrixDestroy(matrix);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS