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

#include "gtest/gtest.h"

#include "animation/rs_particle_noise_field.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class ParticleNoiseFieldTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void ParticleNoiseFieldTest::SetUpTestCase() {}
void ParticleNoiseFieldTest::TearDownTestCase() {}
void ParticleNoiseFieldTest::SetUp() {}
void ParticleNoiseFieldTest::TearDown() {}

class PerlinNoise2DTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void PerlinNoise2DTest::SetUpTestCase() {}
void PerlinNoise2DTest::TearDownTestCase() {}
void PerlinNoise2DTest::SetUp() {}
void PerlinNoise2DTest::TearDown() {}

/**
 * @tc.name: isPointInField001
 * @tc.desc: test results of isPointInField
 * @tc.type: FUNC
 * @tc.require: yanyumeng
 */
HWTEST_F(ParticleNoiseFieldTest, isPointInField001, TestSize.Level1)
{
    ShapeType fieldShape = ShapeType::CIRCLE;
    Vector2f fieldSize = { 8.f, 8.f };   // for test
    Vector2f fieldCenter = { 2.f, 2.f }; // for test
    ParticleNoiseField noiseField(1, fieldShape, fieldSize, fieldCenter, 1, 1.f, 1.f, 1.f);
    bool res = noiseField.isPointInField(fieldSize, fieldShape, fieldCenter, 1.f, 1.f);
    EXPECT_TRUE(res == false);

    fieldShape = ShapeType::RECT;
    fieldSize = { 3.f, 2.f };
    fieldCenter = { 1.f, 1.f }; // for test
    res = noiseField.isPointInField(fieldSize, fieldShape, fieldCenter, 3.f, 2.f);
    EXPECT_TRUE(res);
}

/**
 * @tc.name: calculateEllipseEdgeDistance001
 * @tc.desc: test results of calculateEllipseEdgeDistance
 * @tc.type: FUNC
 * @tc.require: yanyumeng
 */
HWTEST_F(ParticleNoiseFieldTest, calculateEllipseEdgeDistance001, TestSize.Level1)
{
    ShapeType fieldShape = ShapeType::CIRCLE;
    Vector2f fieldSize = { 1.f, 2.f };
    Vector2f fieldCenter = { 2.f, 2.f };
    Vector2f direction = { 1.f, 1.f }; // for test
    ParticleNoiseField noiseField(1, fieldShape, fieldSize, fieldCenter, 1, 1.f, 1.f, 1.f);
    float res = noiseField.calculateEllipseEdgeDistance(direction);
    EXPECT_TRUE(res != 0.f);

    noiseField.fieldSize_ = { 0.f, 0.f };
    direction = { 0.f, 0.f }; // for test
    res = noiseField.calculateEllipseEdgeDistance(direction);
    EXPECT_TRUE(res == 0);
}

/**
 * @tc.name: calculateDistanceToRectangleEdge001
 * @tc.desc: test results of calculateDistanceToRectangleEdge
 * @tc.type: FUNC
 * @tc.require: yanyumeng
 */
HWTEST_F(ParticleNoiseFieldTest, calculateDistanceToRectangleEdge001, TestSize.Level1)
{
    ShapeType fieldShape = ShapeType::CIRCLE;
    Vector2f fieldSize = { 1.f, 1.f };
    Vector2f fieldCenter = { 2.f, 2.f }; // for test
    ParticleNoiseField noiseField(1, fieldShape, fieldSize, fieldCenter, 1, 1.f, 1.f, 1.f);
    Vector2f position = { 1.f, 1.f };
    Vector2f direction = { 2.f, 2.f };
    Vector2f center = { 2.f, 2.f };
    Vector2f size = { 1.f, 1.f }; // for test
    float res = noiseField.calculateDistanceToRectangleEdge(position, direction, center, size);
    EXPECT_TRUE(res != 2.f);

    center = { -8.f, -8.f }; // for test
    res = noiseField.calculateDistanceToRectangleEdge(position, direction, center, size);
    EXPECT_TRUE(res == 0.f);

    direction = { 0.f, 0.f }; // for test
    res = noiseField.calculateDistanceToRectangleEdge(position, direction, center, size);
    EXPECT_TRUE(res == 0.f);
}

/**
 * @tc.name: ApplyField001
 * @tc.desc: test results of ApplyField
 * @tc.type: FUNC
 * @tc.require: yanyumeng
 */
HWTEST_F(ParticleNoiseFieldTest, ApplyField001, TestSize.Level1)
{
    ShapeType fieldShape = ShapeType::RECT;
    Vector2f fieldSize = { 3.f, 2.f };
    Vector2f fieldCenter = { 1.f, 1.f }; // for test
    ParticleNoiseField noiseField(1, fieldShape, fieldSize, fieldCenter, 1, 1.f, 1.f, 1.f);
    Vector2f position = { 3.f, 2.f };
    Vector2f res = noiseField.ApplyField(position);
    EXPECT_TRUE(res.x_ != 2.f);

    noiseField.fieldShape_ = ShapeType::ELLIPSE;
    res = noiseField.ApplyField(position);
    EXPECT_TRUE(res.x_ == 0.f);
}

/**
 * @tc.name: ApplyCurlNoise001
 * @tc.desc: test results of ApplyCurlNoise
 * @tc.type: FUNC
 * @tc.require: yanyumeng
 */
HWTEST_F(ParticleNoiseFieldTest, ApplyCurlNoise001, TestSize.Level1)
{
    ShapeType fieldShape = ShapeType::RECT;
    Vector2f fieldSize = { 3.f, 2.f };
    Vector2f fieldCenter = { 1.f, 1.f }; // for test
    ParticleNoiseField noiseField(1, fieldShape, fieldSize, fieldCenter, 1, 1.f, 1.f, 1.f);
    Vector2f position = { 3.f, 2.f };
    Vector2f res = noiseField.ApplyCurlNoise(position);
    EXPECT_TRUE(res.x_ != 2.f);

    noiseField.fieldShape_ = ShapeType::ELLIPSE;
    res = noiseField.ApplyCurlNoise(position);
    EXPECT_TRUE(res.x_ == 0.f);
}

/**
 * @tc.name: fade001
 * @tc.desc: test results of fade
 * @tc.type: FUNC
 * @tc.require: yanyumeng
 */
HWTEST_F(PerlinNoise2DTest, fade001, TestSize.Level1)
{
    PerlinNoise2D noise2D(1.f, 1.f, 1.f);
    float res = noise2D.fade(1.f);
    EXPECT_TRUE(res == 1.f);
}

/**
 * @tc.name: lerp001
 * @tc.desc: test results of lerp
 * @tc.type: FUNC
 * @tc.require: yanyumeng
 */
HWTEST_F(PerlinNoise2DTest, lerp001, TestSize.Level1)
{
    PerlinNoise2D noise2D(1.f, 1.f, 1.f);
    float res = noise2D.lerp(1.f, 1.f, 2.f);
    EXPECT_TRUE(res == 2.f);
}

/**
 * @tc.name: grad001
 * @tc.desc: test results of grad
 * @tc.type: FUNC
 * @tc.require: yanyumeng
 */
HWTEST_F(PerlinNoise2DTest, grad001, TestSize.Level1)
{
    PerlinNoise2D noise2D(1.f, 1.f, 1.f);
    float res = noise2D.grad(1, 1.f, 2.f);
    EXPECT_TRUE(res == 1.f);
}

/**
 * @tc.name: noise001
 * @tc.desc: test results of noise
 * @tc.type: FUNC
 * @tc.require: yanyumeng
 */
HWTEST_F(PerlinNoise2DTest, noise001, TestSize.Level1)
{
    PerlinNoise2D noise2D(1.f, 1.f, 1.f);
    float res = noise2D.noise(1.f, 2.f);
    EXPECT_TRUE(res != 1.f);
}

/**
 * @tc.name: curl001
 * @tc.desc: test results of curl
 * @tc.type: FUNC
 * @tc.require: yanyumeng
 */
HWTEST_F(PerlinNoise2DTest, curl001, TestSize.Level1)
{
    PerlinNoise2D noise2D(1.f, 1.f, 1.f);
    Vector2f res = noise2D.curl(1.f, 2.f);
    EXPECT_TRUE(res.x_ != 1.f);
}
} // namespace OHOS::Rosen